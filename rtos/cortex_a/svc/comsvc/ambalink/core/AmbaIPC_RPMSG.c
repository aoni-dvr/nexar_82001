/**
 * @file AmbaIPC_RPMSG.c
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details RPMSG wrapper
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"

#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"
#include "AmbaLinkPrivate.h"
#include "AmbaRTSL_MMU.h"
#include "AmbaPrint.h"
#include "AmbaIPC_RPMSG.h"
#include "vq.h"
#include "AmbaUtility.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"
#include "AmbaLink_core.h"

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
#include "AmbaIPC_LinkCtrl.h"
#include "AmbaIPC_Hiber.h"
#endif

#if defined(CONFIG_CPU_CORTEX_R52)
#include "AmbaSYSErrorManager.h"
#endif

#ifdef CONFIG_QNX
//#define IpcDebug(fmt, ...) printf(fmt "\n", __VA_ARGS__)
#define IpcDebug(fmt, ...)
#endif

#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
#include "AmbaSafety_IPC.h"
#define LEN_CRC     (4u)
#define CRC_NG      (1u)
#define MSG_ACK     (1u)
#endif /* CONFIG_AMBALINK_RPMSG_ASIL */

#define CRC_OK      (0u)
#define MSG_NML     (0u)

static inline UINT32 get_rpmsg_size(void)
{
#if !defined(CONFIG_AMBALINK_RPMSG_G2)
/* A53<->A53 without R52 */
return AmbaLinkCtrl.RpmsgBufSize;
#else
/* Ambalink@R52<*>R53 + OpenAMP@A53<->A53  */
return AmbaLinkCtrl.G2_RpmsgBufSize;
#endif
}

static inline INT32 vq_get_buf(struct AmbaIPC_VQ_s *vq, void **buf, UINT32 *len)
{
#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    return vq_get_used_buf(vq, buf, len);
#else
    return vq_get_avail_buf(vq, buf, len);
#endif
}

static inline void vq_add_buf(struct AmbaIPC_VQ_s *vq, UINT16 idx, UINT32 len)
{
#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
        vq_add_avail_buf(vq, idx, len);
#else
        vq_add_used_buf(vq, idx, len);
#endif
}

static inline UINT32 wait_events(AMBA_KAL_EVENT_FLAG_t *flag, UINT32 bits)
{
    UINT32 dummy;
    UINT32 ret;
    ret = AmbaKAL_EventFlagGet(flag, bits, 1U, 1U, &dummy, AMBA_KAL_WAIT_FOREVER);
    return ret;
}

static inline void send_events(AMBA_KAL_EVENT_FLAG_t *flag, UINT32 bits)
{
    (void)AmbaKAL_EventFlagSet(flag, bits);
}

#define RPCLNT_ID_01    0U
#define RPCLNT_NUM      1U

#ifdef RPMSG_DEBUG
static inline UINT32 get_time(void)
{
    return AmbaCSL_TmrGetCurrentVal(PROFILE_TIMER);
}
#endif

static inline void *U32toVoid(UINT32 Addr)
{
    void *ptr;
#ifdef AMBA_MISRA_FIX_H
    ptr = NULL;
    if (AmbaWrap_memcpy(&ptr, &Addr, sizeof(Addr))!= 0U) { }
#else
    ptr = (void *)Addr;
#endif
    return ptr;
}

static struct AmbaIPC_RPCLNT_s G_rpclnt_table[RPCLNT_NUM] __attribute__((section(".bss.noinit")));

static struct AmbaIPC_RPDEV_s *g_registered_rpdev[64] __attribute__((section(".bss.noinit")));
static UINT32 g_registered_cnt = 0U;
static UINT32 AmbaHiberRestore = 0U;

#ifdef RPMSG_DEBUG
AMBA_RPMSG_PROFILE_s *svq_profile, *rvq_profile;
AMBA_RPMSG_STATISTIC_s *rpmsg_stat;
#endif
// FIXME: need a lock to protect the g_registered_cnt;
#if defined(IPC_DEBUG_TIMEOUT)
static void IPCPrint(const char *str, const char *name, UINT32 id, UINT32 len)
{
    char id_s[8] = "", len_s[8] = "";

    if (id != 0U) {
        (void)IO_UtilityUInt32ToStr(id_s, sizeof(id_s), id, 10U);
    }
    if (len != 0U) {
        (void)IO_UtilityUInt32ToStr(len_s, sizeof(len_s), len, 10U);
    }
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, ANSI_YELLOW "%s ch %s id %s len %s" ANSI_RESET, str, name, id_s, len_s, NULL);
}
#endif

/* for ASIL-IPC:
 *     NULL MsgHandler is not allowed in ASIL server
 *     if app did not call AmbaIPC_Alloc(), loop here & wait for R52 monitor to trigger CEHU
 */
static struct AmbaIPC_RPDEV_s *rpdev_lookup(UINT32 src, UINT32 dst, const char *name)
{
    struct AmbaIPC_RPDEV_s *rpdev = NULL;
    UINT32 i;

#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    /* server got NS request, wait for app MsgHandler */
    if (name != NULL) {
        while (g_registered_cnt == 0U) {
            (void) AmbaKAL_TaskSleep(1);
        }
    }
#endif

    i = 0U;
    while (i < g_registered_cnt) {
        struct AmbaIPC_RPDEV_s *rp = g_registered_rpdev[i];
        if (name == NULL) {
            if ((rp->src == src) && (rp->dst == dst)) {
                rpdev = rp;
            }
        } else {
            /* name service: search by name */
            if (0 == AmbaUtility_StringCompare(name, rp->name, AmbaUtility_StringLength(rp->name))) {
                rpdev = rp;
            }
        }
        if (rpdev != NULL) {
#if defined(IPC_DEBUG_TIMEOUT)
            if (name != NULL) {
                IPCPrint("IPC registered", name, 0, 0);
            }
#endif
            IpcDebug("IPC %s(%d) found rpdev %p %s idx %d src %d dst %d priv %p", __func__, __LINE__, rp, rp->name, i, rp->src, rp->dst, rp->priv);
            break;
        }
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        /* server got NS request, wait for app MsgHandler */
        if (name != NULL) {
            /* reach end of array, trigger re-search */
            if (i == (g_registered_cnt - 1U)) {
                (void) AmbaKAL_TaskSleep(1);
                i = 0U;
                continue;
            }
        }
#endif
        i++;
    }

    return rpdev;
}

#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
/* Ack message keep the same header except data and flags */
static void rpdev_send_ack(const struct rpmsg_hdr *hdr_ack)
{
    // TODO: more than one device.
    struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    INT32 idx;
    UINT32 buf_len;
    struct rpmsg_hdr *hdr   = NULL;
    struct AmbaIPC_VQ_s *vq;
    void *ptr;
    /* Make sure sizeof(crc) is equal to LEN_CRC. */
    UINT32 crc;
    void *p;
    const void *q;

    if (hdr_ack == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: NULL ack header.", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }
    else {
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&vq, &rpclnt->svq);
#else
        vq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
#endif

        idx = vq_get_buf(vq, &ptr, &buf_len);

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&hdr, &ptr);
#else
        hdr = (struct rpmsg_hdr *)ptr;
#endif

        if (idx >= 0) {
            hdr->src  = hdr_ack->src;
            hdr->dst  = hdr_ack->dst;
            hdr->id = hdr_ack->id;
            IpcDebug("IPC %s(%d) #%d port %d -> %d", __func__, __LINE__, hdr->id, hdr->src, hdr->dst);
            hdr->len  = hdr_ack->len;
            hdr->flags  = hdr_ack->flags;

            /* Ack message keep the same header except data and flags */
            hdr->flags |= RPMSG_HDR_FLAGS_ACK;
            /* Calculate CRC for header without data[4] */
#ifdef AMBA_MISRA_FIX_H
            AmbaMisra_TypeCast(&ptr, &hdr);
#else
            ptr = (void *)hdr;
#endif
            crc = AmbaIPC_crc32(ptr, (sizeof(struct rpmsg_hdr) - 4U));
            IpcDebug("IPC %s(%d) buf %d ack crc 0x%08x", __func__, __LINE__, idx, crc);
            p = hdr->data;
            q = &crc;
            if (AmbaWrap_memcpy(p, q, LEN_CRC)!= 0U) { }

            vq_add_buf(vq, (UINT16)idx, buf_len);

            vq->kick(rpclnt);
        } else {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: can NOT get vq for ack.", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
        }
    }
}
#endif /* CONFIG_AMBALINK_RPMSG_ASIL */

static INT32 rpdev_send_offchannel(const struct AmbaIPC_RPDEV_s *rpdev,
                                 UINT32 src, UINT32 dst, const void *data, UINT32 len)
{
    INT32 ret = 0;
    INT32 idx;
    UINT32 buf_len;
    UINT32 total_len;
    struct rpmsg_hdr *hdr = NULL;
    const struct AmbaIPC_RPCLNT_s *rpclnt = rpdev->rpclnt;
    struct AmbaIPC_VQ_s *vq;
    void *ptr;
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    static UINT32 id_rpmsg = 0;
#endif
#ifdef RPMSG_DEBUG
    UINT32 to_get_buffer;
#endif

#ifdef RPMSG_DEBUG
    to_get_buffer = ToGetSvqBuf_Profile();
#endif

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&vq, &rpclnt->svq);
#else
    vq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
#endif

    idx = vq_get_buf(vq, &ptr, &buf_len);

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&hdr, &ptr);
#else
    hdr = (struct rpmsg_hdr *)ptr;
#endif

#ifdef RPMSG_DEBUG
    GetSvqBufDone_Profile(to_get_buffer, idx);
#endif

    total_len = ((UINT32)len + (UINT32)sizeof(struct rpmsg_hdr) - sizeof(hdr->data));
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    total_len += LEN_CRC;
#endif
    if ((idx >= 0) &&
        (get_rpmsg_size() >= total_len)) {
        hdr->src  = src;
        hdr->dst  = dst;
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        hdr->id = id_rpmsg;
        IpcDebug("IPC %s(%d) #%d", __func__, __LINE__, hdr->id);
        id_rpmsg++;
#else
        hdr->reserved = 0;
#endif
        if (len > (UINT16)AMBALINK_UINT16_MAX) {
            IpcDebug("IPC %s(%d) len > %d", __func__, __LINE__, AMBALINK_UINT16_MAX);
            hdr->len  = (UINT16)AMBALINK_UINT16_MAX;
        } else {
            hdr->len  = (UINT16)len;
        }
        hdr->flags  = 0;

        if (data != NULL) {
#ifdef CONFIG_QNX
            /*bus error, rpmsg_core got killed*/
            memcpy_isr(hdr->data, data, (UINT32)len);
#else
#ifdef AMBA_MISRA_FIX_H
            void *hdr_data_addr;
            const void *hdr_data_ptr = &(hdr->data[0]);
            AmbaMisra_TypeCast(&hdr_data_addr, &hdr_data_ptr);
            if (AmbaWrap_memcpy(hdr_data_addr, data, (UINT32)hdr->len)!= 0U) { }
#else
            if (AmbaWrap_memcpy(hdr->data, data, (UINT32)hdr->len)!= 0U) { }
#endif
#endif
        }
        IpcDebug("IPC %s(%d) port %d -> %d", __func__, __LINE__, hdr->src, hdr->dst);

#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        {
            /* Make sure sizeof(crc) is equal to LEN_CRC. */
            UINT32 crc;
            void *p;
            const void *q;
            /* start of: p = &(hdr->data[len]); */
            UINT8 *str;
            UINT32 crc_offset;
#ifdef AMBA_MISRA_FIX_H
            AmbaMisra_TypeCast(&str, &hdr);
#else
            str = (UINT8 *)hdr;
#endif
            crc_offset = ((UINT32)len + (UINT32)sizeof(struct rpmsg_hdr) - sizeof(hdr->data));
            str = &(str[crc_offset]);
#ifdef AMBA_MISRA_FIX_H
            AmbaMisra_TypeCast(&p, &str);
#else
            p = (void *)str;
#endif
            /* end of: p = &(hdr->data[len]); */

            if (data != NULL) {
                crc = AmbaIPC_crc32(hdr, (hdr->len + sizeof(struct rpmsg_hdr) - sizeof(hdr->data)));
                IpcDebug("IPC %s(%d) buf %d data crc 0x%08x", __func__, __LINE__, idx, crc);
                /*p = &(hdr->data[len]);*/
                q = &crc;
                if (AmbaWrap_memcpy(p, q, LEN_CRC)!= 0U) { }
            } else {
                /* Must not be here. */
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s send NULL data %s", ANSI_YELLOW, ANSI_RESET, NULL, NULL, NULL);
                AmbaPrint_Flush();
                ret = -1;
            }
#if defined(IPC_DEBUG_TIMEOUT)
            IPCPrint("IPC sending", rpdev->name, hdr->id, (UINT32)hdr->len);
#endif
            /* add msg into list */
            (void)AmbaSafety_IPCEnq(hdr->id);
        }
#endif /* CONFIG_AMBALINK_RPMSG_ASIL */

        vq_add_buf(vq, (UINT16)idx, buf_len);

#ifdef RPMSG_DEBUG
        svq_profile[idx].SvqToSendInterrupt = get_time();
#endif
        vq->kick(rpdev->rpclnt);
#ifdef RPMSG_DEBUG
        svq_profile[idx].SvqSendInterrupt = get_time();
#endif
    } else {
        ret = -1;
    }

    return ret;
}

static INT32 rpdev_trysend(const struct AmbaIPC_RPDEV_s *rpdev, const void *data, UINT32 len)
{
    return rpdev_send_offchannel(rpdev, rpdev->src, rpdev->dst, data, len);
}

static struct AmbaIPC_RPCLNT_s *rpclnt_sync(const char *bus_name)
{
    struct AmbaIPC_RPCLNT_s *rpclnt;

    if (IO_UtilityStringCompare(bus_name, "c0_and_c1",
        IO_UtilityStringLength("c0_and_c1")) == 0) {
        rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
        if (rpclnt->inited == 0) {
            (void)wait_events(&rpclnt->svq_flag, 0x2);
        }
    } else {
        rpclnt = NULL;
    }

    return rpclnt;
}

static void rpdev_rvq_cb(struct AmbaIPC_VQ_s *vq)
{
    const struct rpmsg_hdr *hdr;
    void *ptr;
    struct AmbaIPC_RPDEV_s *rpdev;
    UINT32 len;
    INT32 idx;
#ifdef RPMSG_PROFILE
    struct profile_data data;
#endif // RPMSG_PROFILE
    // For CONFIG_AMBALINK_RPMSG_ASIL, value will be set each time.
    UINT32 isCrcOK = CRC_OK;
    UINT32 isMsgAck = MSG_NML;

    for(;;) {
#ifdef RPMSG_PROFILE
        data.ToGetRvqBuffer = get_time();
#endif // RPMSG_PROFILE

        idx = vq_get_buf(vq, &ptr, &len);

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&hdr, &ptr);
#else
        hdr = (struct rpmsg_hdr *)ptr;
#endif

#ifdef RPMSG_PROFILE
        data.GetRvqBuffer = get_time();
#endif // RPMSG_PROFILE

        if (idx < 0) {
            break;
        }

#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        {
            UINT32 crc;
            const void *p, *q;
            UINT32 err;
            UINT32 v;

            if ((hdr->flags & RPMSG_HDR_FLAGS_ACK) == RPMSG_HDR_FLAGS_ACK) {
                /* Ack message keep the same header except data */
                crc = AmbaIPC_crc32(ptr, (sizeof(struct rpmsg_hdr) - 4U));
                p = hdr->data;
                q = &crc;
                err = AmbaWrap_memcmp(p, q, LEN_CRC, &v);
                if ((err != 0u) || (v != 0u)) {
#if defined(CONFIG_CPU_CORTEX_R52)
                    (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_EPROTO);
#else
                    /*A53: notify R52 CEHU */
                    (void)AmbaSafety_IPCSetSafeState(IPC_ERR_EPROTO);
#endif
                    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Ack CRC error", __func__, NULL, NULL, NULL, NULL);
                    AmbaPrint_Flush();
                    isCrcOK = CRC_NG;
                } else {
                    IpcDebug("IPC %s(%d) buf %d ack crc 0x%08x", __func__, __LINE__, idx, crc);
#if defined(IPC_DEBUG_TIMEOUT)
                    rpdev = rpdev_lookup(hdr->dst, hdr->src, NULL);
                    if (rpdev != NULL) {
                        IPCPrint("IPC acked", rpdev->name, hdr->id, (UINT32)hdr->len);
                    }
#endif
                    /* remove msg from list */
                    (void)AmbaSafety_IPCDeq(hdr->id);
                    isCrcOK = CRC_OK;
                }
                isMsgAck = MSG_ACK;
            } else {
                crc = AmbaIPC_crc32(ptr, (hdr->len + sizeof(struct rpmsg_hdr) - sizeof(hdr->data)));
                p = &(hdr->data[hdr->len]);
                q = &crc;
                err = AmbaWrap_memcmp(p, q, LEN_CRC, &v);
                if ((err != 0u) || (v != 0u)) {
#if defined(CONFIG_CPU_CORTEX_R52)
                    (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_EPROTO);
#else
                    /*A53: notify R52 CEHU */
                    (void)AmbaSafety_IPCSetSafeState(IPC_ERR_EPROTO);
#endif
                    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: CRC error", __func__, NULL, NULL, NULL, NULL);
                    AmbaPrint_Flush();
                    isCrcOK = CRC_NG;
                } else {
                    IpcDebug("IPC %s(%d) buf %d data crc 0x%08x", __func__, __LINE__, idx, crc);
                    isCrcOK = CRC_OK;
                }
                isMsgAck = MSG_NML;
            }
        }
#endif /* CONFIG_AMBALINK_RPMSG_ASIL */

        if ((isCrcOK == CRC_OK) && (isMsgAck == MSG_NML)) {
            const UINT8 *pld = &(hdr->data[0]);
            IpcDebug("IPC %s(%d) port %d -> %d", __func__, __LINE__, hdr->src, hdr->dst);
            rpdev = rpdev_lookup(hdr->dst, hdr->src, NULL);
            if (rpdev != NULL) {
#ifdef RPMSG_PROFILE
                data.ToRecvData = get_time();
#endif // RPMSG_PROFILE

#ifdef AMBA_MISRA_FIX_H
                AmbaMisra_TypeCast(&ptr, &pld);
#else
                ptr = (void *)pld;
#endif

#if defined(IPC_DEBUG_TIMEOUT)
                IPCPrint("IPC callback start", rpdev->name, hdr->id, (UINT32)hdr->len);
#endif
                rpdev->cb(rpdev, ptr, (UINT32)hdr->len, rpdev->priv, hdr->src);
#if defined(IPC_DEBUG_TIMEOUT)
                IPCPrint("IPC callback end", rpdev->name, hdr->id, (UINT32)hdr->len);
#endif

#ifdef RPMSG_PROFILE
                data.RecvData = get_time();
#endif // RPMSG_PROFILE
            }
            else {
#if defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
                const char *chr, *chr2;
                void *ptr2;
                struct rpmsg_channel_info info;
                const struct rpmsg_channel_info *pinfo = &info;
                struct AmbaIPC_RPCLNT_s *rpclnt;
                const struct rpmsg_ns_msg *msg;

                if (hdr->len == sizeof(struct rpmsg_ns_msg)) {
#ifdef AMBA_MISRA_FIX_H
                    AmbaMisra_TypeCast(&msg, &pld);
#else
                    msg = (struct rpmsg_ns_msg *)pld;
#endif
                    rpdev = rpdev_lookup(hdr->src, RPMSG_NS_ADDR, msg->name);
                } else {
                    IpcDebug("IPC %s(%d) this is not a valid NS msg", __func__, __LINE__);
                    rpdev = NULL;
                }

                if (rpdev != NULL) {
                    if (hdr->len == sizeof(struct rpmsg_channel_info)) {
                        rpdev->dst = hdr->src;
                        send_events(&rpdev->flag, 0x1);
                        // Send our port....
                        info.src = 0xFFFFFFFFU;
                        info.dst = rpdev->dst;
                        chr2 = &(info.name[0]);
#ifdef AMBA_MISRA_FIX_H
                        AmbaMisra_TypeCast(&ptr2, &chr2);
                        AmbaMisra_TouchUnused(&info.dst);
#else
                        ptr2 = (void *)chr2;
#endif
                        chr = &(rpdev->name[0]);
#ifdef AMBA_MISRA_FIX_H
                        AmbaMisra_TypeCast(&ptr, &chr);
#else
                        ptr = (void *)chr;
#endif
                        if (AmbaWrap_memcpy(ptr2, ptr, RPMSG_NAME_LEN)!= 0U) { }
                        /* Send response and no-wait */
#ifdef AMBA_MISRA_FIX_H
                        AmbaMisra_TypeCast(&ptr, &pinfo);
#else
                        ptr = (void *)pinfo;
#endif
                        IpcDebug("IPC %s(%d) %s re-use dst %d for handle %p", __func__, __LINE__, rpdev->name , rpdev->dst, rpdev);
                        (void)rpdev_trysend(rpdev, ptr, (INT32)sizeof(struct rpmsg_channel_info));
                    }
                } else {
                    // ASIL server will never go through below to create new channel
                    if (hdr->len == sizeof(struct rpmsg_channel_info)) {
                        struct AmbaIPC_RPDEV_s *newdev;
#ifdef AMBA_MISRA_FIX_H
                        AmbaMisra_TypeCast(&msg, &pld);
#else
                        msg = (struct rpmsg_ns_msg *)pld;
#endif
                        newdev = rpdev_alloc(msg->name, 0, RpmsgCB, NULL);
                        info.src = 0xFFFFFFFFU;
#ifdef AMBA_MISRA_FIX_H
                        AmbaMisra_TouchUnused(&info.src);
#endif

                        rpclnt = rpclnt_sync("c0_and_c1");
                        if ((newdev != NULL) && (rpclnt != NULL)) {
                            newdev->rpclnt = rpclnt;
                            newdev->dst = hdr->src;
                            info.dst = newdev->dst;
#ifdef AMBA_MISRA_FIX_H
                            AmbaMisra_TouchUnused(&info.dst);
#endif
                            if (AmbaWrap_memcpy(&(info.name[0]), &(newdev->name[0]), RPMSG_NAME_LEN)!= 0U) { }
                            /* Send response and no-wait */
#ifdef AMBA_MISRA_FIX_H
                            AmbaMisra_TypeCast(&ptr, &pinfo);
#else
                            ptr = (void *)pinfo;
#endif
                            IpcDebug("IPC %s(%d) %s new channel %p src %d -> dst %d", __func__, __LINE__, newdev->name, newdev, newdev->src, newdev->dst);
                            (void)rpdev_trysend(newdev, ptr, (INT32)sizeof(struct rpmsg_channel_info));
                        } else {
                            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: rpdev_alloc() failed", __func__, NULL, NULL, NULL, NULL);
                            AmbaPrint_Flush();
                        }
                    }
                }
#else  //CONFIG_AMBALINK_RPMSG_ROLE_MAST
                rpdev = rpdev_lookup(hdr->dst, RPMSG_NS_ADDR, NULL);
                if (rpdev != NULL) {
                    if (hdr->len == sizeof(struct rpmsg_channel_info)) {
                        rpdev->dst = hdr->src;
                        IpcDebug("IPC %s(%d) %s learned dst %d for handle %p", __func__, __LINE__, rpdev->name, rpdev->dst, rpdev);
#if defined(IPC_DEBUG_TIMEOUT)
                        IPCPrint("IPC registered", rpdev->name, 0, 0);
#endif
                        send_events(&rpdev->flag, 0x1);
                    }
                }
                else {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "drop src %d dst %d: NULL MsgHandler", hdr->src, hdr->dst, 0U, 0U, 0U);
                }
#endif
            }
        }

#ifdef RPMSG_PROFILE
        data.ReleaseRvq = get_time();
        /* before return the used buffer, we store statistics to avoid the same buffer used again
        to overwrite the original statistics. */
        SyncProfileData(&data, idx);
#endif // RPMSG_PROFILE
        vq_add_buf(vq, (UINT16)idx, get_rpmsg_size());
#ifdef RPMSG_PROFILE
        data.RvqDone = get_time();
        record_rpmsg_stats(idx, data);
#endif // RPMSG_PROFILE

#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
        /* Send back ack if CRC is OK */
        if ((isCrcOK == CRC_OK) && (isMsgAck == MSG_NML)) {
#if defined(IPC_DEBUG_TIMEOUT)
            IPCPrint("IPC acking", rpdev->name, hdr->id, (UINT32)hdr->len);
#endif
            /* Ack message keep the same header except data and flags */
            rpdev_send_ack(hdr);
        }
#endif /* CONFIG_AMBALINK_RPMSG_ASIL */
    }
}

static void rpdev_svq_cb(struct AmbaIPC_VQ_s *vq)
{
    vq_complete(vq);
}

static void rpclnt_kick_rvq(void *data)
{
    const struct AmbaIPC_RPCLNT_s *rpclnt;
    const struct AmbaIPC_VQ_s *vq;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)data;
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&rpclnt, &data);
    AmbaMisra_TypeCast(&vq, &rpclnt->rvq);
#else
    rpclnt = (struct AmbaIPC_RPCLNT_s *)data;
    vq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif

    if (vq_kick_prepare(vq) != 0U) {
        IpcDebug("%s rvq tx %s", ANSI_YELLOW, ANSI_RESET);
        IpcSPWriteBit(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_SET_OFFSET,
            (UINT32)(rpclnt->rvq_tx_irq - (UINT32)AXI_SOFT_IRQ0));
    }
}

static void rpclnt_kick_svq(void *data)
{
    const struct AmbaIPC_RPCLNT_s *rpclnt;
    const struct AmbaIPC_VQ_s *vq;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)data;
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&rpclnt, &data);
    AmbaMisra_TypeCast(&vq, &rpclnt->svq);
#else
    rpclnt = (struct AmbaIPC_RPCLNT_s *)data;
    vq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
#endif

    if (vq_kick_prepare(vq) != 0U) {
        IpcDebug("%s svq tx %s", ANSI_BLUE, ANSI_RESET);
        IpcSPWriteBit(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_SET_OFFSET,
            (UINT32)(rpclnt->svq_tx_irq - (UINT32)AXI_SOFT_IRQ0));
    }
}

// Disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")
static void rpclnt_svq_rx_isr(UINT32 args, UINT32 UserArg)
{
    struct AmbaIPC_RPCLNT_s *rpclnt = (struct AmbaIPC_RPCLNT_s*)&G_rpclnt_table[0];

    (void)UserArg;
    send_events(&rpclnt->svq_flag, 0x1);

    IpcSPWriteBit(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_CLEAR_OFFSET,
        (UINT32)(args - (UINT32)AXI_SOFT_IRQ0));
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    (void) AmbaSafety_IPCIrqHdlr();
#endif
}

static void rpclnt_rvq_rx_isr(UINT32 args, UINT32 UserArg)
{
    struct AmbaIPC_RPCLNT_s *rpclnt = (struct AmbaIPC_RPCLNT_s*)&G_rpclnt_table[0];

    (void)UserArg;
    send_events(&rpclnt->rvq_flag, 0x1);

    IpcSPWriteBit(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_CLEAR_OFFSET,
        (UINT32)(args - (UINT32)AXI_SOFT_IRQ0));
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    (void) AmbaSafety_IPCIrqHdlr();
#endif
}
#pragma GCC pop_options
// Disable NEON registers usage in ISR

static void rpclnt_complete_registration(struct AmbaIPC_RPCLNT_s *rpclnt)
{
    void *VirtAddr;
    struct AmbaIPC_VQ_s *rvq;

    VirtAddr = IpcPhysToVirt(rpclnt->rvq_buf_phys);

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rvq, &rpclnt->rvq);
#else
        rvq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif

    (void)vq_init_unused_bufs(rvq, VirtAddr, get_rpmsg_size());

#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    {
        struct AmbaIPC_VQ_s *svq;

        vq_init_avail(rvq, (UINT16)rvq->vring.num);

        // Remote should do it well, but ...
        VirtAddr = IpcPhysToVirt(rpclnt->svq_buf_phys);

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&svq, &rpclnt->svq);
#else
        svq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
#endif
        (void)vq_init_unused_bufs(svq, VirtAddr, get_rpmsg_size());

        vq_init_avail(svq, 0U);
        vq_init_used(svq, get_rpmsg_size(), (UINT16)svq->vring.num);
    }
#endif

    rpclnt->inited = 1;
    send_events(&rpclnt->svq_flag, 0x2);
}

static void *rpclnt_task_rvq(void *param)
{
    struct AmbaIPC_RPCLNT_s *rpclnt;
    struct AmbaIPC_VQ_s *rvq;

    (void)param;
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rpclnt, &param);
        AmbaMisra_TypeCast(&rvq, &rpclnt->rvq);
#else
        rpclnt = (struct AmbaIPC_RPCLNT_s *)param;
        rvq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif

    for (;;) {
        UINT32 ret;
        ret = wait_events(&rpclnt->rvq_flag, 0x1);
        if (ret != KAL_ERR_NONE) {
            IpcDebug("In %s, wait_events error: %d", __func__, ret);
            break;
        }
        IpcDebug("%s rvq rx %s", ANSI_RED, ANSI_RESET);
        if (rvq->cb != NULL){
            rvq->cb(rvq);
        }
    }
    return NULL;
}

static void *rpclnt_task_svq(void *param)
{
    struct AmbaIPC_RPCLNT_s *rpclnt;
    struct AmbaIPC_VQ_s *svq;

    (void)param;
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rpclnt, &param);
        AmbaMisra_TypeCast(&svq, &rpclnt->svq);
#else
        rpclnt = (struct AmbaIPC_RPCLNT_s *)param;
        svq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
#endif

#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    {
        const struct AmbaIPC_VQ_s *rvq;

        // Prepare ring well for remote.
        rpclnt_complete_registration(rpclnt);

        // Announce
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rvq, &rpclnt->rvq);
#else
        rvq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif
        rvq->kick(rpclnt);

        // No wating for announce from remote/client.

#ifndef CONFIG_QNX
        if (AmbaHiberRestore != 0U) {
            AmbaLink_RpmsgInitDone();
        }
#endif
    }
#else
    if (AmbaHiberRestore == 0U) {
        (void)wait_events(&rpclnt->svq_flag, 0x1);
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "rpmsg host is online", NULL, NULL, NULL, NULL, NULL);
        //AmbaPrint_Flush();
    }
    rpclnt_complete_registration(rpclnt);

#ifndef CONFIG_QNX
    if (AmbaHiberRestore != 0U) {
        AmbaLink_RpmsgInitDone();
    }
#endif
#endif

    for (;;) {
        UINT32 ret;
        ret = wait_events(&rpclnt->svq_flag, 0x1);
        if (ret != KAL_ERR_NONE) {
            IpcDebug("In %s, wait_events error: %d", __func__, ret);
            break;
        }
        IpcDebug("%s svq rx %s", ANSI_GREEN, ANSI_RESET);
        if (svq->cb != NULL) {
            svq->cb(svq);
        }
    }
    return NULL;
}

static void rpclnt_register(struct AmbaIPC_RPCLNT_s *rpclnt)
{
    void *EntryArg;
    UINT32 status;
    const AMBA_LINK_TASK_CTRL_s *pTask;
    const void *VirtAddr;
    void *pStackBase;
    static UINT8 RvqStack[0x4000] GNU_SECTION_NOZEROINIT;
    const UINT8 *pRvqStack = RvqStack;
    static UINT8 SvqStack[0x4000] GNU_SECTION_NOZEROINIT;
    const UINT8 *pSvqStack = SvqStack;
    static char RvqFlag[16] = "RvqFlag";
    static char SvqFlag[16] = "SvqFlag";

    VirtAddr = IpcPhysToVirt(rpclnt->svq_vring_phys);
    rpclnt->svq = vq_create(rpdev_svq_cb,
                            rpclnt_kick_svq,
                            rpclnt->svq_num_bufs,
                            VirtAddr,
                            rpclnt->svq_vring_algn, VQ_ALLOC_RPC_SVQ);

    VirtAddr = IpcPhysToVirt(rpclnt->rvq_vring_phys);
    rpclnt->rvq = vq_create(rpdev_rvq_cb,
                            rpclnt_kick_rvq,
                            rpclnt->rvq_num_bufs,
                            VirtAddr,
                            rpclnt->rvq_vring_algn, VQ_ALLOC_RPC_RVQ);

    /* create an event flag */
    status = AmbaKAL_EventFlagCreate(&rpclnt->rvq_flag, RvqFlag);
    if (status != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "can't not create rvq_flag %d", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
    status = AmbaKAL_EventFlagCreate(&rpclnt->svq_flag, SvqFlag);
    if (status != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "can't not create svq_flag %d", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    /* create rvq_task */
    if (AmbaWrap_memset(&RvqStack[0], 0, sizeof(RvqStack))!= 0U) { }
    if (AmbaWrap_memset(&SvqStack[0], 0, sizeof(SvqStack))!= 0U) { }

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&EntryArg, &rpclnt);
    AmbaMisra_TypeCast(&pStackBase, &pRvqStack);
#else
    EntryArg = (void *)rpclnt;
    pStackBase = (void *)pRvqStack;
#endif
    (void)EntryArg;
    status = AmbaKAL_TaskCreate(&rpclnt->rvq_thread,
                                rpclnt->rvq_name,
                                rpclnt->rvq_tsk_prio,
                                rpclnt_task_rvq,
                                EntryArg,
                                pStackBase,
                                (UINT32)sizeof(RvqStack),
                                0U);
    if (status != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "can't not create rvq thread %d", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_RVQ];

#ifdef CONFIG_QNX
    status = AmbaKAL_TaskSetSmpAffinity(&rpclnt->rvq_thread,pTask->CoreSel);
#else
    status = AmbaKAL_TaskSetSmpAffinity(&rpclnt->rvq_thread,(pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
#endif
    if (status != 0U) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    status = AmbaKAL_TaskResume(&rpclnt->rvq_thread);
    if (status != 0U) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (0x%x)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    /* create svq_task */
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&EntryArg, &rpclnt);
    AmbaMisra_TypeCast(&pStackBase, &pSvqStack);
#else
    EntryArg = (void *)rpclnt;
    pStackBase = (void *)pSvqStack;
#endif
    status = AmbaKAL_TaskCreate(&rpclnt->svq_thread,
                                rpclnt->svq_name,
                                rpclnt->svq_tsk_prio,
                                rpclnt_task_svq,
                                EntryArg,
                                pStackBase,
                                (UINT32)sizeof(SvqStack),
                                0U);
    if (status != 0U) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "can't not create svq thread %d", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_SVQ];

#ifdef CONFIG_QNX
    status = AmbaKAL_TaskSetSmpAffinity(&rpclnt->svq_thread,pTask->CoreSel);
#else
    status = AmbaKAL_TaskSetSmpAffinity(&rpclnt->svq_thread,(pTask->CoreSel) & AMBA_KAL_CPU_CORE_MASK);
#endif
    if (status != 0U) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    status = AmbaKAL_TaskResume(&rpclnt->svq_thread);
    if (status != 0U) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (0x%x)", status, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }

    (void)AmbaIPC_INTEnable();
}

static void rpclnt_init_c0_and_c1(void)
{
    struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    AMBA_LINK_TASK_CTRL_s *pTask;
#if defined(CONFIG_AMBALINK_RPMSG_G2)
    UINT32 size = (UINT32)((AmbaLinkCtrl.G2_RpmsgNumBuf * AmbaLinkCtrl.G2_RpmsgBufSize) >> 1U);
#else
    UINT32 size = (UINT32)((AmbaLinkCtrl.RpmsgNumBuf * AmbaLinkCtrl.RpmsgBufSize) >> 1U);
#endif
    pTask = &AmbaLinkTaskCtrl[LINK_TASK_RVQ];

    rpclnt->inited    = 0;

    rpclnt->rvq_tsk_prio    = pTask->Priority;
    rpclnt->rvq_tsk_sz      = pTask->StackSize;
    rpclnt->rvq_name        = pTask->TaskName;

    pTask = &AmbaLinkTaskCtrl[LINK_TASK_SVQ];

    rpclnt->svq_tsk_prio    = pTask->Priority;
    rpclnt->svq_tsk_sz      = pTask->StackSize;
    rpclnt->svq_name        = pTask->TaskName;

    /*
       Usage of RPMSG memory layout
       ============================
       Client   base    Host
                Buffers --->
           <--- Buffers
                Ring_Descriptor --->
                Ring_Avail
                Ring_Used
       ======== Padding ========
           <--- Ring_Descriptor
                Ring_Avail
                Ring_Used
     */

#ifndef CONFIG_AMBALINK_RPMSG_G2
    rpclnt->svq_tx_irq      = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C0_TO_C1_KICK;
    rpclnt->svq_rx_irq      = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C1_TO_C0_ACK;
    rpclnt->rvq_tx_irq      = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C0_TO_C1_ACK;
    rpclnt->rvq_rx_irq      = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C1_TO_C0_KICK;

    rpclnt->svq_num_bufs    = (UINT32)(AmbaLinkCtrl.RpmsgNumBuf >> 1U);
    rpclnt->rvq_num_bufs    = (UINT32)(AmbaLinkCtrl.RpmsgNumBuf >> 1U);

#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    rpclnt->rvq_buf_phys = U32toVoid(AmbaLinkCtrl.VringHostAndClntBufAddr);
    rpclnt->svq_buf_phys = U32toVoid(AmbaLinkCtrl.VringHostAndClntBufAddr + size);

    rpclnt->rvq_vring_phys = U32toVoid(AmbaLinkCtrl.VringClntToHostAddr);
    rpclnt->svq_vring_phys = U32toVoid(AmbaLinkCtrl.VringHostToClntAddr);
#else
    rpclnt->svq_buf_phys = U32toVoid(AmbaLinkCtrl.VringHostAndClntBufAddr);
    rpclnt->rvq_buf_phys = U32toVoid(AmbaLinkCtrl.VringHostAndClntBufAddr + size);

    rpclnt->svq_vring_phys = U32toVoid(AmbaLinkCtrl.VringClntToHostAddr);

    rpclnt->rvq_vring_phys = U32toVoid(AmbaLinkCtrl.VringHostToClntAddr);
#endif // CONFIG_AMBALINK_RPMSG_ROLE_MAST

#else // Use Group-2
    rpclnt->svq_num_bufs    = (UINT16)((AmbaLinkCtrl.G2_RpmsgNumBuf >> 1U)&(UINT16)AMBALINK_UINT16_MAX);
    rpclnt->rvq_num_bufs    = (UINT16)((AmbaLinkCtrl.G2_RpmsgNumBuf >> 1U)&(UINT16)AMBALINK_UINT16_MAX);

#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    /* For CV2FS, R52 (C2) is Master */
    rpclnt->svq_tx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C2_TO_C1_KICK;
    rpclnt->svq_rx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C1_TO_C2_ACK;
    rpclnt->rvq_tx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C2_TO_C1_ACK;
    rpclnt->rvq_rx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C1_TO_C2_KICK;

    rpclnt->rvq_buf_phys = U32toVoid(AmbaLinkCtrl.G2_BufAddr);
    rpclnt->svq_buf_phys = U32toVoid(AmbaLinkCtrl.G2_BufAddr + size);
    rpclnt->rvq_vring_phys = U32toVoid(AmbaLinkCtrl.G2_S2MAddr);
    rpclnt->svq_vring_phys = U32toVoid(AmbaLinkCtrl.G2_M2SAddr);
#else
    /* For CV2FS, A53 (C1) is Slave */
    rpclnt->svq_tx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C1_TO_C2_KICK;
    rpclnt->svq_rx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C2_TO_C1_ACK;
    rpclnt->rvq_tx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C1_TO_C2_ACK;
    rpclnt->rvq_rx_irq = (UINT32)AXI_SOFT_IRQ0 + AIPC_IRQ_C2_TO_C1_KICK;

    rpclnt->svq_buf_phys = U32toVoid(AmbaLinkCtrl.G2_BufAddr);
    rpclnt->rvq_buf_phys = U32toVoid(AmbaLinkCtrl.G2_BufAddr + size);

    rpclnt->svq_vring_phys = U32toVoid(AmbaLinkCtrl.G2_S2MAddr);
    rpclnt->rvq_vring_phys = U32toVoid(AmbaLinkCtrl.G2_M2SAddr);
#endif
#endif // CONFIG_AMBALINK_RPMSG_G2

    rpclnt->svq_vring_algn = RPMSG_VRING_ALIGN;
    rpclnt->rvq_vring_algn = RPMSG_VRING_ALIGN;

    rpclnt_register(rpclnt);

#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "RPMSG in Master/Host mode ", NULL, NULL, NULL, NULL, NULL);
#else
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "RPMSG in Client/Remote mode ", NULL, NULL, NULL, NULL, NULL);
#endif
}

INT32 rpdev_send(const struct AmbaIPC_RPDEV_s *rpdev, const void *data, UINT32 len)
{
    struct AmbaIPC_VQ_s *vq;
    INT32 ret = 0;

    if (rpdev == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s NULL rpdev_send(): rpdev %s", ANSI_YELLOW, ANSI_RESET, NULL, NULL, NULL);
        AmbaPrint_Flush();
        ret = -1;
    }
    else {
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&vq, &rpdev->rpclnt->svq);
#else
        vq = (struct AmbaIPC_VQ_s *)rpdev->rpclnt->svq;
#endif

        while (rpdev_trysend(rpdev, data, len) != 0) {
            vq_wait_for_completion(vq);
        }
    }

    return ret;
}

INT32 rpdev_register(struct AmbaIPC_RPDEV_s *rpdev, const char *bus_name)
{
    struct rpmsg_ns_msg nsm;
    struct AmbaIPC_RPCLNT_s *rpclnt;
    INT32 ret = 0;
    const char *chr, *chr2;
    void *ptr;
    const void *ptr2;

    if (rpdev->dst != RPMSG_NS_ADDR) {
        IpcDebug("IPC %s(%d) %s re-use dst %d for handle %p", __func__, __LINE__, rpdev->name, rpdev->dst, rpdev);
    }
    else {
        chr = &(nsm.name[0]);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&ptr, &chr);
#else
        ptr = (void *)chr;
#endif

        chr2 = &(rpdev->name[0]);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&ptr2, &chr2);
#else
        ptr2 = (void *)chr2;
#endif
        if (AmbaWrap_memcpy(ptr, ptr2, RPMSG_NAME_LEN)!= 0U) { }

        nsm.addr  = rpdev->src;
        nsm.flags = rpdev->flags;

        rpclnt = rpclnt_sync(bus_name);

        if (rpclnt != NULL) {
            rpdev->rpclnt = rpclnt;

            if (AmbaHiberRestore == 0U) {
                const struct rpmsg_ns_msg *pnsm = &nsm;
#ifdef AMBA_MISRA_FIX_H
                AmbaMisra_TypeCast(&ptr, &pnsm);
#else
                ptr = (void *)pnsm;
#endif

#if !defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
                (void)rpdev_send_offchannel(rpdev, rpdev->src, RPMSG_NS_ADDR, ptr, (INT32)sizeof(nsm));
                (void)wait_events(&rpdev->flag, 0x1);
#endif
            }
        }
        else {
            ret = -1;
        }
    }

    return ret;
}

INT32 rpdev_unregister(struct AmbaIPC_RPDEV_s *rpdev)
{
    AmbaLink_Free(MEM_POOL_IPC_BUF, rpdev);
    return 0;
}

struct AmbaIPC_RPDEV_s *rpdev_alloc(const char *name, UINT32 flags, rpdev_cb cb, void *priv)
{
    struct AmbaIPC_RPDEV_s *rpdev = NULL;
    UINT32 len_name, len;
    UINT32 i;

    len_name = AmbaUtility_StringLength(name);
    for (i = 0U; i < g_registered_cnt; i++) {
        len = AmbaUtility_StringLength(g_registered_rpdev[i]->name);
        if (len_name != len) {
            continue;
        }
        if (AmbaUtility_StringCompare(name, g_registered_rpdev[i]->name, len) == 0) {
            rpdev = g_registered_rpdev[i];
            IpcDebug("IPC %s(%d) %s re-use handle %p", __func__, __LINE__, rpdev->name, rpdev);
            break;
        }
    }

    if (rpdev == NULL) {
        static char FlagName[16] = "RpdevFlag";
        const void *ptr = AmbaLink_Malloc(MEM_POOL_IPC_BUF, (UINT32)sizeof(struct AmbaIPC_RPDEV_s));
        if (ptr != NULL) {
#ifdef AMBA_MISRA_FIX_H
            AmbaMisra_TypeCast(&rpdev, &ptr);
#else
            rpdev = (struct AmbaIPC_RPDEV_s *)ptr;
#endif
            rpdev->magicID = RPMSG_MAGIC_ID;
            rpdev->src   = RPMSG_RESERVED_ADDRESSES + g_registered_cnt;
            rpdev->dst   = RPMSG_NS_ADDR;
            rpdev->flags = flags;
            rpdev->cb    = cb;
            rpdev->priv  = priv;
            IpcDebug("IPC %s(%d) %s malloc new handle %p cb %p priv %p src %d dst %d", __func__, __LINE__, name, rpdev, rpdev->cb, rpdev->priv, rpdev->src, rpdev->dst);

            (void)AmbaKAL_EventFlagCreate(&rpdev->flag, FlagName);

            /* update g_registered_rpdev AFTERWARD in case someone got wrong data */
            IO_UtilityStringCopy(rpdev->name, RPMSG_NAME_LEN, name);
            g_registered_rpdev[g_registered_cnt] = rpdev;
            g_registered_cnt++;
        }
    } else {
        rpdev->priv = priv;
    }

    return rpdev;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  RpmsgCB
 *
 *  @Description::  This is the standard RPMSG callback. We just call the
 *      AMBA_IPC_MSG_HANDLER_f which is kept as "priv" when we alloc the channel
 *
\*----------------------------------------------------------------------------*/
void RpmsgCB(struct AmbaIPC_RPDEV_s *rpdev, void *data, UINT32 len,
                    void *priv, UINT32 src)
{
    AMBA_IPC_MSG_CTRL_s msg;
    AMBA_IPC_MSG_HANDLER_f handler;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)priv;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&handler, &priv);
#else
    handler = (AMBA_IPC_MSG_HANDLER_f)priv;
#endif

    (void)src;
    msg.Length = len;
    msg.pMsgData = data;
    if (handler != NULL) {
        IpcDebug("IPC %s(%d) %s handle %p cb %p priv %p", __func__, __LINE__, rpdev->name, rpdev, rpdev->cb, rpdev->priv);
        (void)handler(rpdev, &msg);
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "drop %s: NULL MsgHandler", rpdev->name, NULL, NULL, NULL, NULL);
//#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
#if 0  /* not bus err, app err should be covered by app layer */
#if defined(CONFIG_CPU_CORTEX_R52)
        (void)AmbaSEMGR_ModuleErrorReport(AMBALINK_MODULE_ID, 0U, 0U, IPC_ERR_ETIMEDOUT);
#else
        /*A53: notify R52 CEHU */
        (void)AmbaSafety_IPCSetSafeState(IPC_ERR_ETIMEDOUT);
#endif
#endif
    }
}

UINT32 AmbaIPC_INTEnable(void)
{
    const struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    AMBA_INT_CONFIG_s IntConfig;
    UINT32 Ret = 0U;

    IntConfig.TriggerType   = AMBA_INT_HIGH_LEVEL_TRIGGER;      /* Sensitivity type */
    IntConfig.IrqType       = AMBA_INT_IRQ;                     /* Interrupt type: IRQ or FIQ */
    IntConfig.CpuTargets    = AmbaLinkCtrl.AmbaLinkRunTarget;   /* Target cores */

    Ret |= AmbaINT_Config(rpclnt->svq_rx_irq, &IntConfig, rpclnt_svq_rx_isr, 0U);
    Ret |= AmbaINT_Enable(rpclnt->svq_rx_irq);

    IntConfig.TriggerType   = AMBA_INT_HIGH_LEVEL_TRIGGER;      /* Sensitivity type */
    IntConfig.IrqType       = AMBA_INT_IRQ;                     /* Interrupt type: IRQ or FIQ */
    IntConfig.CpuTargets    = AmbaLinkCtrl.AmbaLinkRunTarget;   /* Target cores */

    Ret |= AmbaINT_Config(rpclnt->rvq_rx_irq, &IntConfig, rpclnt_rvq_rx_isr, 0U);
    Ret |= AmbaINT_Enable(rpclnt->rvq_rx_irq);

    return Ret;
}

UINT32 AmbaIPC_INTDisable(void)
{
    const struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    UINT32 Ret = 0U;

    Ret |= AmbaINT_Disable(rpclnt->svq_rx_irq);
    Ret |= AmbaINT_Disable(rpclnt->rvq_rx_irq);

    return Ret;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_UnregisterChannel
 *
 *  @Description::  Clean up a registered channel
 *
 *  @Input      ::
 *      channel: rpmsg channel
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*----------------------------------------------------------------------------*/
INT32 AmbaIPC_UnregisterChannel(AMBA_IPC_HANDLE Channel)
{
    struct AmbaIPC_RPDEV_s *rpdev;
    INT32 ret = -1;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)Channel;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&rpdev, &Channel);
#else
    rpdev = (struct AmbaIPC_RPDEV_s*)Channel;
#endif

    if (rpdev != NULL) {
        if (rpdev->magicID == RPMSG_MAGIC_ID) {
            ret = rpdev_unregister(rpdev);
#ifdef CONFIG_OPENAMP
        } else {
            extern INT32 AmbaOamp_UnregisterChannel(void *);

            ret = AmbaOamp_UnregisterChannel(Channel);
#endif
        }
    }
    return ret;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_TrySend
 *
 *  @Description::  Send message across the channel
 *
 *  @Input      ::
 *      channel: rpmsg channel
 *      pData:   message buffer
 *      Length:  message length
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
\*----------------------------------------------------------------------------*/
INT32 AmbaIPC_TrySend(AMBA_IPC_HANDLE Channel, const void *pData, INT32 Length)
{
    const struct AmbaIPC_RPDEV_s *rpdev;
    INT32 ret = -1;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)pData;
    (void)Channel;

    if (Length <= 0) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: Length <= 0 ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    } else {
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rpdev, &Channel);
#else
        rpdev = (struct AmbaIPC_RPDEV_s*)Channel;
#endif

        if (rpdev != NULL) {
            if (rpdev->magicID == RPMSG_MAGIC_ID) {
                ret = rpdev_trysend(rpdev, pData, (UINT32)Length);
#ifdef CONFIG_OPENAMP
            } else {
                extern INT32 AmbaOamp_TrySend(void *, const void *, INT32);

                ret = AmbaOamp_TrySend(Channel, pData, (UINT32)Length);
#endif
            }
        }
    }

    return ret;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpmsgSuspend
 *
 *  @Description::  Suspend RPMSG module
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RpmsgSuspend(void)
{
#if defined(CONFIG_AMBALINK_RPMSG_HIBER)
    struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    struct AmbaIPC_VQ_s *svq;
    struct AmbaIPC_VQ_s *rvq;
    struct AmbaIPC_VRING_s *svr;
    struct AmbaIPC_VRING_s *rvr;
    UINT32 Align = 0x1000;
    UINT32 MaxSize;
    void *VirtAddr;
    UINT32 *pUint32;
    UINT8 *pTmp;
    UINT32 i;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&svq, &rpclnt->svq);
    AmbaMisra_TypeCast(&rvq, &rpclnt->rvq);
#else
    svq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
    rvq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif

    svr = &svq->vring;
    rvr = &rvq->vring;

    AmbaIPC_HiberResetSuspendBackupSize();

    MaxSize = vring_size(rvr->num, Align);

    pTmp = AmbaLink_Malloc(MEM_POOL_VRING_BUF, MaxSize);
    if (pTmp == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaLink_Malloc failed!! ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return;
    }
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "vq size 0x%08x\n", sizeof(struct AmbaIPC_VQ_s) * 2, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    if (LINUX_IS_HIBER_MODE()) {
        /* Fixed the mismatch index of rpmsg between Linux and ThreadX. */
        /* There is an rpmsg sent after Linux hibernation image created. */
        /* We skip the last index and save the rpmsg info. */
        rvq->last_avail_idx--;
        rvq->vring.avail->idx--;
        rvq->vring.avail->ring[rvq->vring.avail->idx] = 0;
        rvq->vring.used->idx--;
    }
    /* ---------------------------------------------------------------- */

    if (AmbaWrap_memcpy(pTmp, rpclnt->svq, sizeof(struct AmbaIPC_VQ_s))!= 0U) { }
    if (AmbaWrap_memcpy(pTmp + sizeof(struct AmbaIPC_VQ_s), rpclnt->rvq, sizeof(struct AmbaIPC_VQ_s))!= 0U) { }
    (void)AmbaIPC_HiberSaveRpmsgInfo(pTmp, sizeof(struct AmbaIPC_VQ_s) * 2);

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "rpmsg buf size 0x%08x\n", g_registered_cnt * AmbaLinkCtrl.RpmsgBufSize * 2, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    pUint32 = (UINT32 *) pTmp;
    *pUint32 = g_registered_cnt;

    (void)AmbaIPC_HiberSaveRpmsgInfo(pTmp, sizeof(g_registered_cnt));
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, " vr size 0x%08x\n", vring_size(rvr->num, Align) * 2, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    VirtAddr = IpcPhysToVirt(rpclnt->svq_vring_phys);
    (void)AmbaIPC_HiberSaveRpmsgInfo((UINT8 *)VirtAddr, vring_size(rvr->num, Align));
    VirtAddr = IpcPhysToVirt(rpclnt->rvq_vring_phys);
    (void)AmbaIPC_HiberSaveRpmsgInfo((UINT8 *)VirtAddr, vring_size(svr->num, Align));

    pUint32 = (UINT32 *) pTmp;

    for (i = 0U; i < g_registered_cnt; i++) {
        *pUint32++ = g_registered_rpdev[i]->src;
        *pUint32++ = g_registered_rpdev[i]->dst;
    }

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "rpdev size 0x%08x\n", (sizeof(UINT32) << 1) * i, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    (void)AmbaIPC_HiberSaveRpmsgInfo(pTmp, (sizeof(UINT32) << 1U) * i);

    AmbaLink_Free(MEM_POOL_VRING_BUF, pTmp);
#endif
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpmsgRestoreData
 *
 *  @Description::  Restore RPMSG modules' data structure.
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RpmsgRestoreData(void)
{
#if defined(CONFIG_AMBALINK_RPMSG_HIBER)
    struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    struct AmbaIPC_VQ_s *svq;
    struct AmbaIPC_VQ_s *rvq;
    struct AmbaIPC_VQ_s *pVq;
    struct AmbaIPC_VRING_s *svr;
    struct AmbaIPC_VRING_s *rvr;
    UINT32 Align = 0x1000;
    UINT32 MaxSize;
    void *VirtAddr;
    UINT8 *pTmp;
    UINT32 *pUint32;
    UINT32 i;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&svq, &rpclnt->svq);
    AmbaMisra_TypeCast(&rvq, &rpclnt->rvq);
#else
    svq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
    rvq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif

    svr = &svq->vring;
    rvr = &rvq->vring;

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s!\n", NULL, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();
    AmbaIPC_HiberResetSuspendBackupSize();

    MaxSize = vring_size(rvr->num, Align);

    pTmp = AmbaLink_Malloc(MEM_POOL_VRING_BUF, MaxSize);
    if (pTmp == NULL) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaLink_Malloc failed!! ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return;
    }

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "vq size 0x%08x\n", sizeof(struct AmbaIPC_VQ_s) * 2, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    (void)AmbaIPC_HiberRestoreRpmsgInfo(pTmp, sizeof(struct AmbaIPC_VQ_s) * 2);
    pVq = (struct AmbaIPC_VQ_s *) pTmp;
    if (AmbaWrap_memcpy(&svq->vring, &pVq->vring, sizeof(struct AmbaIPC_VRING_s))!= 0U) { }
    svq->last_avail_idx = pVq->last_avail_idx;
    svq->last_used_idx = pVq->last_used_idx;

    pVq = (struct AmbaIPC_VQ_s *) (pTmp + sizeof(struct AmbaIPC_VQ_s));
    if (AmbaWrap_memcpy(&rvq->vring, &pVq->vring, sizeof(struct AmbaIPC_VRING_s))!= 0U) { }
    rvq->last_avail_idx = pVq->last_avail_idx;
    rvq->last_used_idx = pVq->last_used_idx;

    pUint32 = (UINT32 *) pTmp;
    (void)AmbaIPC_HiberRestoreRpmsgInfo(pTmp, sizeof(g_registered_cnt));

    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "rpmsg buf size 0x%08x\n", g_registered_cnt * AmbaLinkCtrl.RpmsgBufSize * 2, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();

    if (*pUint32 != g_registered_cnt) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "rpmsg info restore failed (registered cnt = %d)!!", g_registered_cnt, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
        goto AmbaIPC_RpmsgRestore_Exit;
    }
#if 0 // TODO: save it
    VirtAddr = IpcPhysToVirt(rpclnt->svq_buf_phys);
    AmbaIPC_HiberRestoreRpmsgInfo(VirtAddr, g_registered_cnt * AmbaLinkCtrl.RpmsgBufSize);
    VirtAddr = IpcPhysToVirt(rpclnt->rvq_buf_phys);
    AmbaIPC_HiberRestoreRpmsgInfo(VirtAddr, g_registered_cnt * AmbaLinkCtrl.RpmsgBufSize);
#endif
    //AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
    //AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "vr size 0x%08x\n", vring_size(rvr->num, Align) * 2, 0U, 0U, 0U, 0U);
    //AmbaPrint_Flush();
    VirtAddr = IpcPhysToVirt(rpclnt->svq_vring_phys);
    (void)AmbaIPC_HiberRestoreRpmsgInfo((UINT8 *)VirtAddr, vring_size(rvr->num, Align));
    VirtAddr = IpcPhysToVirt(rpclnt->rvq_vring_phys);
    (void)AmbaIPC_HiberRestoreRpmsgInfo((UINT8 *)VirtAddr, vring_size(svr->num, Align));

    (void)AmbaIPC_HiberRestoreRpmsgInfo(pTmp, (sizeof(UINT32) << 1) * g_registered_cnt);

    for (i = 0U; i < g_registered_cnt; i++) {
        if (g_registered_rpdev[i]->src == *pUint32++) {
            g_registered_rpdev[i]->dst = *pUint32++;
        } else {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "rpmsg rpdev info restore failed (%d)!!", g_registered_cnt, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            goto AmbaIPC_RpmsgRestore_Exit;
        }
    }

    AmbaLink_Free(MEM_POOL_VRING_BUF, pTmp);

    AmbaIPC_HiberRestoreRpmsgFinish();

    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Restore rpmsg info finish.", NULL, NULL, NULL, NULL, NULL);

AmbaIPC_RpmsgRestore_Exit:
    return;
#endif
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpmsgRestoreStatus
 *
 *  @Description::  Return rpmsg restore status.
 *
\*----------------------------------------------------------------------------*/
UINT32 AmbaIPC_RpmsgRestoreStatus(void)
{
    return AmbaHiberRestore;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpmsgRestoreEnable
 *
 *  @Description::  Enable to restore RPMSG module
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RpmsgRestoreEnable(UINT32 Enable)
{
    AmbaHiberRestore = Enable;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_RpmsgInit
 *
 *  @Description::  Init RPMSG module
 *
\*----------------------------------------------------------------------------*/
void AmbaIPC_RpmsgInit(void)
{
    if (AmbaWrap_memset(G_rpclnt_table, 0, sizeof(G_rpclnt_table))!= 0U) { }
    if (AmbaWrap_memset(g_registered_rpdev, 0, sizeof(g_registered_rpdev))!= 0U) { }

#ifdef RPMSG_DEBUG
    AmbaCSL_TmrSetEnable(PROFILE_TIMER, 0);
    AmbaCSL_TmrSetReloadVal(PROFILE_TIMER, 0xFFFFFFFFU);
    AmbaCSL_TmrSetCurrentVal(PROFILE_TIMER, 0xFFFFFFFFU);
    AmbaCSL_TmrSetEnable(PROFILE_TIMER, 1);
    InitRpmsgProfile();
#endif

    rpclnt_init_c0_and_c1();
}

void AmbaIPC_dump(void)
{
    const struct AmbaIPC_RPCLNT_s *rpclnt = &G_rpclnt_table[RPCLNT_ID_01];
    const struct AmbaIPC_VQ_s *svq, *rvq;
    const struct AmbaIPC_VRING_s *rvr, *svr;
    UINT32 j;
    UINT16 i, idx;
    const struct vring_desc *desc;
    const void *VirtAddr;
    void *PhysAddr;
//    UINT32 len;
    const struct rpmsg_hdr *hdr;
    UINT16 svqLastIdx, rvqLastIdx, svrUsedIdx, rvrUsedIdx, svrAvailIdx, rvrAvailIdx;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&svq, &rpclnt->svq);
    AmbaMisra_TypeCast(&rvq, &rpclnt->rvq);
#else
    svq = (struct AmbaIPC_VQ_s *)rpclnt->svq;
    rvq = (struct AmbaIPC_VQ_s *)rpclnt->rvq;
#endif

    svr = &svq->vring;
    rvr = &rvq->vring;

    svqLastIdx = (svq->last_avail_idx % svr->num)&(UINT16)AMBALINK_UINT16_MAX;
    svrUsedIdx = (svr->used->idx % svr->num)&(UINT16)AMBALINK_UINT16_MAX;
    svrAvailIdx = (svr->avail->idx % svr->num)&(UINT16)AMBALINK_UINT16_MAX;
    rvqLastIdx = (rvq->last_avail_idx % rvr->num)&(UINT16)AMBALINK_UINT16_MAX;
    rvrUsedIdx = (rvr->used->idx % rvr->num)&(UINT16)AMBALINK_UINT16_MAX;
    rvrAvailIdx = (rvr->avail->idx % rvr->num)&(UINT16)AMBALINK_UINT16_MAX;

    /* svq */
    AmbaPrint_PrintUInt5(ANSI_BLUE"[rtos Tx count] svq->last_avail_idx = %d/%d svr->used->idx = %d/%d"ANSI_RESET,
        svqLastIdx, svr->num, svrUsedIdx, svr->num, 0U);
    AmbaPrint_PrintUInt5(ANSI_BLUE"[linux Rx count] svr->avail->idx = %d/%d"ANSI_RESET,
        svrAvailIdx, svr->num, 0U, 0U, 0U);

    /* rvq */
    AmbaPrint_PrintUInt5(ANSI_RED"[linux Tx count] rvr->avail->idx = %d/%d"ANSI_RESET,
        rvrAvailIdx, rvr->num, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5(ANSI_RED"[rtos Rx count] rvq->last_avail_idx = %d/%d rvr->used->idx = %d/%d"ANSI_RESET,
        rvqLastIdx, rvr->num, rvrUsedIdx, rvr->num, 0U);

    /* flags */
    AmbaPrint_PrintUInt5("svr->avail->flags 0x%x", svr->avail->flags, 0U, 0U, 0U, 0U);
    AmbaPrint_PrintUInt5("rvr->used->flags 0x%x", rvr->used->flags, 0U, 0U, 0U, 0U);

    /* svq data dump */
    for (i = 0U; i < svqLastIdx; i++) {
        idx  = svr->avail->ring[i];
        desc = &(svr->desc[idx]);
        //PhysAddr = (void *)desc->addr;
        if (AmbaWrap_memcpy(&PhysAddr, &desc->addr, sizeof(PhysAddr))!= 0U) { }
        VirtAddr = IpcPhysToVirt(PhysAddr);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&hdr, &VirtAddr);
        AmbaMisra_TypeCast(&j, &VirtAddr);
#else
        hdr = (struct rpmsg_hdr *)VirtAddr;
        j = (UINT32)VirtAddr;
#endif
//        len = desc->len;
        AmbaPrint_PrintUInt5("svr->avail[%d] 0x%08X: src: 0x%08X, dst: 0x%08X, len: %d"ANSI_RESET,
                i, j, hdr->src, hdr->dst, hdr->len);
    }

    /* rvq data dump */
    for (i = 0U; i < rvqLastIdx; i++) {
        idx  = rvr->avail->ring[i];
        desc = &(rvr->desc[idx]);
        //PhysAddr = (void *)desc->addr;
        if (AmbaWrap_memcpy(&PhysAddr, &desc->addr, sizeof(PhysAddr))!= 0U) { }
        VirtAddr = IpcPhysToVirt(PhysAddr);
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&hdr, &VirtAddr);
        if (AmbaWrap_memcpy(&j, &VirtAddr, sizeof(j))!= 0U) { }
#else
        hdr = (struct rpmsg_hdr *)VirtAddr;
        j = (UINT32)VirtAddr;
#endif
//        len = desc->len;
        AmbaPrint_PrintUInt5("rvr->avail[%d] 0x%08X: src: 0x%08X, dst: 0x%08X, len: %d",
                i, j, hdr->src, hdr->dst, hdr->len);
    }
}

#ifdef CONFIG_OPENAMP
void AmbaOamp_kick(UINT32 id)
{
    switch (id) {
        /* #define VRING_IRQ_C0_TO_C1_KICK         AXI_SOFT_IRQ(0) */
    case 0:
        /* #define VRING_IRQ_C0_TO_C1_ACK          AXI_SOFT_IRQ(1) */
    case 1:
        /* #define VRING_IRQ_C1_TO_C0_KICK         AXI_SOFT_IRQ(2) */
    case 2:
        /* #define VRING_IRQ_C1_TO_C0_ACK          AXI_SOFT_IRQ(3) */
    case 3:
        IpcSPWriteBit(AHB_SCRATCHPAD_BASE + AHB_SP_SWI_SET_OFFSET, id);
        break;
    default:
        // no action
        break;
    }
}
#endif // CONFIG_OPENAMP

