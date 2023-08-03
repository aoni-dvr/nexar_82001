/**
 * @file AmbaIPC.h
 * API prototype
 *
 * @ingroup ipc
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AMBA_IPC_H
#define AMBA_IPC_H

#define RPMSG_DEV_AMBA              (0U)
#define RPMSG_DEV_OAMP              (1U)

#define AMBALINK_MODULE_ID          ((UINT16)(AMBALINK_ERR_BASE >> 16U))
#define IPC_ERR_NONE                0U
#define IPC_ERR_EINTR               (AMBALINK_ERR_BASE + 4U)      /* interrupt error */
#define IPC_ERR_EINVAL              (AMBALINK_ERR_BASE + 22U)     /* Invalid argument */
#define IPC_ERR_EREMOTE             (AMBALINK_ERR_BASE + 66U)     /* remote error */
#define IPC_ERR_EPROTO              (AMBALINK_ERR_BASE + 71U)     /* protocol error */
#define IPC_ERR_ENODATA             (AMBALINK_ERR_BASE + 61U)     /* No data available */
#define IPC_ERR_ETIMEDOUT           (AMBALINK_ERR_BASE + 110U)    /* timed out */
#define IPC_ERR_OSERR               (AMBALINK_ERR_BASE + 500U)    /* System resource not available */
#define IPC_ERR_TSERR               (AMBALINK_ERR_BASE + 501U)    /* timestamp error */

/* all OS define the same mutex id */
#define AMBA_IPC_MUTEX_I2C_CHANNEL0     0U
#define AMBA_IPC_MUTEX_I2C_CHANNEL1     1U
#define AMBA_IPC_MUTEX_I2C_CHANNEL2     2U
#define AMBA_IPC_MUTEX_SPI_CHANNEL0     3U
#define AMBA_IPC_MUTEX_SPI_CHANNEL1     4U
#define AMBA_IPC_MUTEX_SPI_CHANNEL2     5U
#define AMBA_IPC_MUTEX_SD0              6U
#define AMBA_IPC_MUTEX_SD1              7U
#define AMBA_IPC_MUTEX_SD2              8U
#define AMBA_IPC_MUTEX_NAND             9U
#define AMBA_IPC_MUTEX_GPIO            10U
#define AMBA_IPC_MUTEX_PLL             11U
#define AMBA_IPC_MUTEX_SPINOR          12U
#define AMBA_IPC_MUTEX_ENET            13U
#define AMBA_IPC_MUTEX_RPMSG           14U
#define AMBA_IPC_MUTEX_VIN             15U
#define AMBA_IPC_MUTEX_VOUT            16U
#define AMBA_IPC_MUTEX_DSPMON          17U
#define AMBA_IPC_MUTEX_IPMON           18U
#define AMBA_IPC_NUM_MUTEX             19U

UINT32  AmbaIPC_MutexTake(UINT32 MutexID, UINT32 Timeout);
UINT32  AmbaIPC_MutexGive(UINT32 MutexID);

typedef void *AMBA_IPC_HANDLE;  /**<  RPMSG Channel */

typedef struct {
    UINT32  Length;        /**< The length of the received data */
    void    *pMsgData;     /**< The pointer to the received data */
} AMBA_IPC_MSG_CTRL_s;

/**
 * @brief This function is invoked when the channel receives the message.
 *
 * @param [in] IpcHandle RPMSG channel pointer
 * @param [in] pMsgCtrl The pointer to the received data
 *
 * @return 0 - OK, others - NG
 */
typedef INT32 (*AMBA_IPC_MSG_HANDLER_f)(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl);
UINT32 AmbaIPC_Init(void);
AMBA_IPC_HANDLE AmbaIPC_Alloc(UINT32 rpmsgID, const char *pName, AMBA_IPC_MSG_HANDLER_f MsgHandler);
INT32 AmbaIPC_RegisterChannel(AMBA_IPC_HANDLE Channel, const char *pRemote);
INT32 AmbaIPC_Send(AMBA_IPC_HANDLE Channel, void *pData, INT32 Length);
#if defined(CONFIG_AMBALINK_RPMSG_ASIL)
UINT32 AmbaSafety_IPCSetSafeState(UINT32 State);
UINT32 AmbaSafety_IPCGetSafeState(UINT32 *pState);
#if defined(CONFIG_CPU_CORTEX_R52)
UINT32 AmbaSafety_IPCAsilChecker(UINT32 *ModuleID, UINT32 *InstanceID, UINT32 *ApiID, UINT32 *ErrorID);
#endif
#endif
#endif /* AMBA_IPC_H */
