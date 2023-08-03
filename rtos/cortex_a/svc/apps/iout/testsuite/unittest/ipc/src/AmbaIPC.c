/**
 * @file AmbaIPC.c
 *  IPC module
 *
 * @defgroup ipc title
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
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaIPC_RPMSG.h"
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
#include "AmbaSafety_IPC.h"
#endif

#define DEBUG_OPENAMP (0)
#if DEBUG_OPENAMP
#include <AmbaPrint.h>
#endif

static UINT32 IPCInited;

/**
 * @ingroup ipc
 * IPC module initialization
 *
 * @return ipc error number
 */
UINT32 AmbaIPC_Init(void)
{
    UINT32 ret = 0U;

    /* check */
    if (IPCInited != 0U) {
        ret = IPC_ERR_EINVAL;
    } else {
        /* code start */
        AmbaLink_Init();

#if defined(CONFIG_AMBALINK_RPMSG_G2)
        ret |= AmbaLink_RpmsgInit(RPMSG_DEV_AMBA);
#endif /* #if defined(CONFIG_AMBALINK_RPMSG_G2) */

#if 0 /* skip booting linux & init rpmsg g1 */
        AmbaLink_OS();

#if defined(CONFIG_OPENAMP)
        ret |= AmbaLink_RpmsgInit(RPMSG_DEV_OAMP);
#elif !defined(CONFIG_AMBALINK_RPMSG_G2)
        ret |= AmbaLink_RpmsgInit(RPMSG_DEV_AMBA);
#endif
#endif

        if (ret == 0U) {
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
            (void) AmbaSafety_IPCInit();
#endif
            /* MUST set IPCInited afterwards to block AmbaIPC_Send */
            IPCInited = 1U;
        }
    }

    return ret;
}

/**
 * @ingroup ipc
 * Allocate an IPC channel and specify received message handler
 *
 * @param [in] rpmsgID Device ID, e.g., RPMSG_DEV_OAMP or RPMSG_DEV_AMBA
 * @param [in] pName channel name
 * @param [in] MsgHandler channel message handler
 * @return AMBA_IPC_HANDLE
 */
AMBA_IPC_HANDLE AmbaIPC_Alloc(UINT32 rpmsgID, const char *pName, AMBA_IPC_MSG_HANDLER_f MsgHandler)
{
    const struct AmbaIPC_RPDEV_s *rpdev = NULL;
    void *pri;
    AMBA_IPC_HANDLE ptr;

    /* arg check */
    if ((rpmsgID != RPMSG_DEV_OAMP) && (rpmsgID != RPMSG_DEV_AMBA)) {
        ptr = NULL;
    } else if (pName == NULL) {
        ptr = NULL;
    } else if (MsgHandler == NULL) {
        ptr = NULL;
    } else if (IPCInited == 0U) {
        ptr = NULL;
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    } else if (0U != AmbaSafety_IPCGetSafeState(NULL)) {
        ptr = NULL;
#endif
    } else {
        /* code start */

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&pri, &MsgHandler);
#else
        pri = (void *)MsgHandler;
#endif

        switch (rpmsgID) {
        case RPMSG_DEV_AMBA:
            rpdev = rpdev_alloc(pName, 0, RpmsgCB, pri);
            break;
#ifdef CONFIG_OPENAMP
        case RPMSG_DEV_OAMP: {
            extern void * AmbaOamp_Alloc(const char *, AMBA_IPC_MSG_HANDLER_f);

            return AmbaOamp_Alloc(pName, MsgHandler);
        }
#endif
        default:
            rpdev = NULL;
            break;
        }

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&ptr, &rpdev);
#else
        ptr = (void *)rpdev;
#endif
    }

    return ptr;
}

/**
 * @ingroup ipc
 * Register a RPMsg channel and send IPC registration message to remote processor
 *
 * @param [in] Channel IPC handle
 * @param [in] pRemote client rpmsg bus name, specify NULL to use system default
 * @return ipc error number
 */
INT32 AmbaIPC_RegisterChannel(AMBA_IPC_HANDLE Channel, const char *pRemote)
{
    struct AmbaIPC_RPDEV_s *rpdev;
    INT32 ret = -1;
    UINT32 errRet;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)Channel;

    /* arg check */
    if (Channel == NULL) {
        errRet = IPC_ERR_EINVAL;
        AmbaMisra_TypeCast(&ret, &errRet);
    } else if (IPCInited == 0U) {
        errRet = IPC_ERR_EINVAL;
        AmbaMisra_TypeCast(&ret, &errRet);
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    } else if (0U != AmbaSafety_IPCGetSafeState(NULL)) {
        errRet = IPC_ERR_OSERR;
        AmbaMisra_TypeCast(&ret, &errRet);
#endif
    } else {
        /* code start */

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rpdev, &Channel);
#else
        rpdev = (struct AmbaIPC_RPDEV_s*)Channel;
#endif

        if (rpdev != NULL) {
            if (rpdev->magicID == RPMSG_MAGIC_ID) {
                const char *name = "c0_and_c1";
                if (pRemote == NULL) {
                    ret = rpdev_register(rpdev, name);
                } else {
                    ret = rpdev_register(rpdev, pRemote);
                }
#ifdef CONFIG_OPENAMP
            } else {
                extern INT32 AmbaOamp_RegisterChannel(void *, const char *);

                ret = AmbaOamp_RegisterChannel(Channel, pRemote);
#endif
            }
        }
    }

    return ret;
}

/**
 * @ingroup ipc
 * Send message to remote processor
 * If no buffer is available, it will be blocked until a buffer becomes available.
 * pData will be copied to buffer.
 *
 * @param [in] Channel IPC handle
 * @param [in] pData message buffer
 * @param [in] Length message length
 * @return ipc error number
 */
INT32 AmbaIPC_Send(AMBA_IPC_HANDLE Channel, void *pData, INT32 Length)
{
    const struct AmbaIPC_RPDEV_s *rpdev;
    INT32 ret = -1;
    UINT32 errRet;

    /* [MISRA Rule 8.13] could be declared as pointing to const */
    AmbaMisra_TouchUnused(pData);
    (void)Channel;

    /* arg check */
    if (Channel == NULL) {
        errRet = IPC_ERR_EINVAL;
        AmbaMisra_TypeCast(&ret, &errRet);
    } else if (pData == NULL) {
        errRet = IPC_ERR_EINVAL;
        AmbaMisra_TypeCast(&ret, &errRet);
    } else if (Length <= 0) {
        errRet = IPC_ERR_EINVAL;
        AmbaMisra_TypeCast(&ret, &errRet);
    } else if (IPCInited == 0U) {
        errRet = IPC_ERR_EINVAL;
        AmbaMisra_TypeCast(&ret, &errRet);
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
    } else if (0U != AmbaSafety_IPCGetSafeState(NULL)) {
        errRet = IPC_ERR_OSERR;
        AmbaMisra_TypeCast(&ret, &errRet);
#endif
    } else {
        /* code start */

#ifdef AMBA_MISRA_FIX_H
        AmbaMisra_TypeCast(&rpdev, &Channel);
#else
        rpdev = (struct AmbaIPC_RPDEV_s*)Channel;
#endif

        if (rpdev != NULL) {
            if (rpdev->magicID == RPMSG_MAGIC_ID) {
                ret = rpdev_send(rpdev, pData, (UINT32)Length);
#ifdef CONFIG_OPENAMP
            } else {
                extern INT32 AmbaOamp_Send(void *, void *, INT32);

                /* openamp returns number of bytes sent or negative error value on failure */
                ret = AmbaOamp_Send(Channel, pData, (UINT32)Length);
                if (ret >= 0) {
                    ret = 0;
                } else {
#if DEBUG_OPENAMP
                    AmbaPrint_PrintStr5("%s: Get error: ", __func__, NULL, NULL, NULL, NULL);
                    AmbaPrint_PrintInt5("%d\n", ret, 0u, 0u, 0u,0u);
                    AmbaPrint_Flush();
#endif // DEBUG_OPENAMP

                    /* Would be RPMSG_ERR_PARAM, RPMSG_ERR_DEV_STATE,
                       RPMSG_ERR_BUFF_SIZE, RPMSG_ERR_NO_BUFF */
                    errRet = IPC_ERR_EINVAL;
                    AmbaMisra_TypeCast(&ret, &errRet);
                }
#endif // CONFIG_OPENAMP
            }
        }
    }
    return ret;
}
