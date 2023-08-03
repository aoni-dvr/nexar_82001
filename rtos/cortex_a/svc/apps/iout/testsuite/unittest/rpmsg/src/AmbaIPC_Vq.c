/**
 * @file AmbaIPC_Vq.c
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
 * @details The data structure and management of Vq are implemented here.
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaLinkPrivate.h"
#include "AmbaLink.h"
#include "vq.h"
#include "AmbaWrap.h"



#ifdef CONFIG_QNX
//#define IpcDebug(fmt, ...) printf(fmt "\n", __VA_ARGS__)
#define IpcDebug(fmt, ...)
#endif

void vq_wait_for_completion(struct AmbaIPC_VQ_s *vq)
{
    UINT32 actual_events;

    (void)AmbaKAL_EventFlagGet(&vq->flag, 0x1, 1U, 1U,
                               &actual_events, AMBA_KAL_WAIT_FOREVER);
}

void vq_complete(struct AmbaIPC_VQ_s *vq)
{
    (void)AmbaKAL_EventFlagSet(&vq->flag, 0x1);
}

static inline void vq_clean_d_cache_region(const void *addr, const UINT32 size)
{
    /* Same cluster: inner shareable */
    /* Cross cluster: non-cached area */
    const void *dummy;
    dummy = addr;
    (void)dummy;
    (void)size;
}

static inline void vq_flush_d_cache_region(const void *addr, const UINT32 size)
{
    /* Same cluster: inner shareable */
    /* Cross cluster: non-cached area */
    const void *dummy;
    dummy = addr;
    (void)dummy;
    (void)size;
}

static inline void vq_lock_init(struct AmbaIPC_VQ_s *vq)
{
    static char MutexName[16] = "vq";
    (void)AmbaKAL_MutexCreate(&vq->lock, MutexName);
}

static inline void vq_lock(struct AmbaIPC_VQ_s *vq, UINT32 flag)
{
    (void)flag;

    (void)AmbaKAL_MutexTake(&vq->lock, AMBA_KAL_WAIT_FOREVER);
}

static inline void vq_unlock(struct AmbaIPC_VQ_s *vq, UINT32 flag)
{
    (void)flag;

    (void)AmbaKAL_MutexGive(&vq->lock);
}

UINT32 vq_kick_prepare(const struct AmbaIPC_VQ_s *vq)
{
    UINT32 ret = 0U; /* do not notify remote */
    UINT16 no_notify;
#ifdef CONFIG_AMBALINK_RPMSG_ROLE_MAST
    vq_flush_d_cache_region(&vq->vring.used->flags,
                            sizeof(vq->vring.used->flags));
    no_notify = vq->vring.used->flags & VRING_USED_F_NO_NOTIFY;
#else
    vq_flush_d_cache_region(&vq->vring.avail->flags,
                            sizeof(vq->vring.avail->flags));
    no_notify = vq->vring.avail->flags & VRING_AVAIL_F_NO_INTERRUPT;
#endif

    if (no_notify == 0U) {
        ret = 1;  /* notify remote */
    }
    return ret;
}

void vq_enable_used_notify(const struct AmbaIPC_VQ_s *vq)
{
    vq_flush_d_cache_region(&vq->vring.used->flags,
                            sizeof(vq->vring.used->flags));

    vq->vring.used->flags &= ~VRING_USED_F_NO_NOTIFY;

    vq_clean_d_cache_region(&vq->vring.used->flags,
                            sizeof(vq->vring.used->flags));
}

void vq_disable_used_notify(const struct AmbaIPC_VQ_s *vq)
{
    vq_flush_d_cache_region(&vq->vring.used->flags,
                            sizeof(vq->vring.used->flags));

    vq->vring.used->flags |= VRING_USED_F_NO_NOTIFY;

    vq_clean_d_cache_region(&vq->vring.used->flags,
                            sizeof(vq->vring.used->flags));
}

UINT32 vq_more_avail_buf(const struct AmbaIPC_VQ_s *vq)
{
    const struct vring_avail *avail = vq->vring.avail;
    UINT32 ret;

    vq_flush_d_cache_region(avail, sizeof(*avail));

    if (vq->last_avail_idx != avail->idx) {
        ret = 1U; /* more buffer available */
    } else {
        ret = 0U; /* no buffer available */
    }

    return ret;
}

struct AmbaIPC_VQ_s *vq_create(void (*cb)(struct AmbaIPC_VQ_s *vq),
                               void (*kick)(void *data),
                               UINT16 num,
                               const void *p, UINT32 align, const UINT32 vqid)
{
    static char FlagName[16] = "VqFlag";
    static struct AmbaIPC_VQ_s ipc_vq[VQ_ALLOC_MAX];
    struct AmbaIPC_VQ_s *vq;

    if(vqid >= VQ_ALLOC_MAX) {
        vq = NULL;
    } else {
        vq = &ipc_vq[vqid];

        (void)AmbaKAL_EventFlagCreate(&vq->flag, FlagName);
        vq_lock_init(vq);

        vq->cb             = cb;
        vq->kick           = kick;
        vq->last_avail_idx = 0U;
        vq->last_used_idx  = 0U;

        vring_init(&vq->vring, num, p, align);
    }

    return vq;
}

/*
 * vq_get_avail_buf and vq_add_used_buf usually used in pairs to
 * grab a "available" buffer, use it, and then tell the HOST, we
 * finish our usage.
 */
INT32 vq_get_avail_buf(struct AmbaIPC_VQ_s *vq, void **buf, UINT32 *len)
{
    INT32 idx;
    UINT16 last_avail_idx;
    const struct vring_avail *avail;
    const struct vring_desc *desc;
    UINT32 flag = 0;
    void *ptr;

    vq_lock(vq, flag);

    if (vq_more_avail_buf(vq) == 0U) {
        idx = -1;
    } else {
        IpcDebug("IPC %s(%d) remote host avail %d > local client %d", __func__, __LINE__, vq->vring.avail->idx, vq->last_avail_idx);
        last_avail_idx = (UINT16)((vq->last_avail_idx % vq->vring.num)&(UINT16)AMBALINK_UINT16_MAX);
        vq->last_avail_idx++;
        avail = vq->vring.avail;

        vq_flush_d_cache_region(avail, sizeof(*avail));
        vq_flush_d_cache_region(&avail->ring[last_avail_idx], sizeof(*avail->ring));

        idx = (INT32)avail->ring[last_avail_idx];
        desc = &vq->vring.desc[idx];
        vq_flush_d_cache_region(desc, sizeof(*desc));

        //ptr = (void *)desc->addr;
        if (AmbaWrap_memcpy(&ptr, &desc->addr, sizeof(ptr))!= 0U) { }

        *buf = IpcPhysToVirt(ptr);
        *len = desc->len;

        vq_flush_d_cache_region(*buf, *len);
    }

    vq_unlock(vq, flag);
    return idx;
}

void vq_add_used_buf(struct AmbaIPC_VQ_s *vq, UINT16 idx, UINT32 len)
{
    struct vring_used_elem *used_elem;
    struct vring_used *used;
    UINT32 flag = 0;
    const void *ptr;

    vq_lock(vq, flag);

    //ptr = (void *)vq->vring.desc[idx].addr;
    if (AmbaWrap_memcpy(&ptr, &vq->vring.desc[idx].addr, sizeof(ptr))!= 0U) { }

    /* Clean the cache of buffer of the used buffer */
    vq_clean_d_cache_region(ptr, len);

    /* Update the used descriptor and clean its cache region */
    used    = vq->vring.used;
    used_elem = &used->ring[used->idx % vq->vring.num];
    used_elem->id = (UINT32) idx;
    used_elem->len  = len;
    vq_clean_d_cache_region(used_elem, sizeof(*used_elem));

    /*
     * Update the used descriptor index and clean its cache region.
     * This step has to be done as the last step.  The order matters.
     */
    used->idx++;
    IpcDebug("IPC %s(%d) used++ %d last_used_idx %d", __func__, __LINE__, used->idx, vq->last_used_idx);
    vq_clean_d_cache_region(used, sizeof(*used));

    vq_unlock(vq, flag);
}


/*
 * vq_get_used_buf and vq_add_avail_buf usually used in pairs to
 * grab a "available" buffer, use it, and then tell the Remote, we
 * finish our usage.
 */
static UINT32 vq_more_used_buf(const struct AmbaIPC_VQ_s *vq)
{
    UINT32 ret = 0U;
    const struct vring_used *used = vq->vring.used;

    vq_flush_d_cache_region(used, sizeof(*used));

    if (vq->last_used_idx != used->idx) {
        ret = 1U; /* more buffer available */
    } else {
        ret = 0U; /* no buffer available */
    }

    return ret;
}

INT32 vq_get_used_buf(struct AmbaIPC_VQ_s *vq, void **buf, UINT32 *len)
{
    INT32 idx;
    UINT16 last_used_idx;
    const struct vring_used *used;
    const struct vring_desc *desc;
    UINT32 flag = 0;
    void *ptr;

    vq_lock(vq, flag);

    if (vq_more_used_buf(vq) == 0U) {
        idx = -1;
    } else {
        IpcDebug("IPC %s(%d) remote client used %d > local host %d", __func__, __LINE__, vq->vring.used->idx, vq->last_used_idx);
        last_used_idx = (UINT16)((vq->last_used_idx % vq->vring.num)&(UINT16)AMBALINK_UINT16_MAX);
        vq->last_used_idx++;
        used = vq->vring.used;

        vq_flush_d_cache_region(used, sizeof(*used));
        vq_flush_d_cache_region(&used->ring[last_used_idx], sizeof(*used->ring));

#if 0 // TODO: Buffer leakage if client/remote do not return buffer to used.
        idx = used->ring[last_used_idx].id;
#else
        idx = (INT32)last_used_idx;
#endif
        desc = &vq->vring.desc[idx];
        vq_flush_d_cache_region(desc, sizeof(*desc));

        //ptr = (void *)desc->addr;
        if (AmbaWrap_memcpy(&ptr, &desc->addr, sizeof(ptr))!= 0U) { }

        *buf = IpcPhysToVirt(ptr);
        *len = desc->len;

        vq_flush_d_cache_region(*buf, *len);
    }

    vq_unlock(vq, flag);
    return idx;
}

void vq_add_avail_buf(struct AmbaIPC_VQ_s *vq, UINT16 idx, UINT32 len)
{
    struct vring_avail *avail;
    UINT32 flag = 0;
    const void *ptr;
    UINT16 avail_idx;

    vq_lock(vq, flag);

    //ptr = (void *)vq->vring.desc[idx].addr;
    if (AmbaWrap_memcpy(&ptr, &vq->vring.desc[idx].addr, sizeof(ptr))!= 0U) { }

    /* Clean the cache of buffer of the avail buffer */
    vq_clean_d_cache_region(ptr, len);

    /* Update the avail descriptor and clean its cache region */
    avail    = vq->vring.avail;
    avail_idx = (UINT16)((avail->idx % vq->vring.num)&(UINT16)AMBALINK_UINT16_MAX);
    avail->ring[avail_idx] = idx;

    /*
     * Update the avail descriptor index and clean its cache region.
     * This step has to be done as the last step.  The order matters.
     */
    avail->idx++;
    IpcDebug("IPC %s(%d) avail++ %d last_avail_idx %d", __func__, __LINE__, avail->idx, vq->last_avail_idx);
    vq_clean_d_cache_region(avail, sizeof(*avail));

    vq_unlock(vq, flag);
}

/*
 * Linux HOST rpmsg implementation only setup the RX buffer descriptors on HOST.
 * Buffer descriptors for HOST TX (CLIENT RX) are left for CLIENT to setup.
 *
 * A very IMPORTANT thing should be noted: Though the Linux HOST does not setup
 * the HOST TX descriptors, it "zeros out" them during vring initialization.
 * So this API should be synchronized and called only after the Linux HOST has
 * finished their initialization.
 */
UINT32 vq_init_unused_bufs(struct AmbaIPC_VQ_s *vq, void *buf, UINT32 len)
{
    struct vring_desc *desc;
    UINT32 flag = 0;
    UINT32 i;
    const void *virt = IpcVirtToPhys(buf);
    char *cbuf;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&cbuf, &buf);
#else
    cbuf = (char *)buf;
#endif

    vq_lock(vq, flag);

    for (i = 0; i < vq->vring.num; i++) {
        desc = &vq->vring.desc[i];
        desc->addr = 0U;
        //desc->addr = (UINT64)virt;
        if (AmbaWrap_memcpy(&desc->addr, &virt, sizeof(virt))!= 0U) { }
        desc->len = len;
        cbuf = &cbuf[len];
#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&buf, &cbuf);
#else
        buf = (void *)cbuf;
#endif
        virt = IpcVirtToPhys(buf);
    }
    desc = &vq->vring.desc[0];
    vq_clean_d_cache_region(desc, sizeof(*desc) * vq->vring.num);

    vq_unlock(vq, flag);

    return 0;
}

/* Set up rx buffers for remote */
void vq_init_avail(struct AmbaIPC_VQ_s *vq, UINT16 idx)
{
    struct vring_avail *avail = vq->vring.avail;
    UINT32 flag = 0;
    UINT16 i;
    UINT16 num;

    vq_lock(vq, flag);

    //num = (UINT16)vq->vring.num;
    if (AmbaWrap_memcpy(&num, &vq->vring.num, sizeof(num))!= 0U) { }

    for (i = 0U; i < num; i++) {
        // assign to index of descriptor
        avail->ring[i] = i;
    }
    avail->idx = idx;

    vq_clean_d_cache_region(&(avail->ring[0]), sizeof(avail->ring[0]) * vq->vring.num);

    vq_unlock(vq, flag);
}

/* Set up tx buffers for Host/Local/Master */
void vq_init_used(struct AmbaIPC_VQ_s *vq, UINT32 len, UINT16 idx)
{
    struct vring_used *used = vq->vring.used;
    UINT32 flag = 0;
    UINT32 i;

    vq_lock(vq, flag);

    for (i = 0U; i < vq->vring.num; i++) {
        // assign to index of descriptor
        used->ring[i].id = i;
        used->ring[i].len = len;
    }
    used->idx = idx;

    vq_clean_d_cache_region(&(used->ring[0]), sizeof(used->ring[0]) * vq->vring.num);

    vq_unlock(vq, flag);
}
