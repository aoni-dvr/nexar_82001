/**
*  @file errmgr.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
*  @details AmbaCV error manager
*
*/

#include "os_api.h"
#include "cvsched_drv_errno.h"
#include "cvapi_ambacv_flexidag.h"
#include "schdr.h"
#if defined(ASIL_SAFETY_MONITOR)
#include "AmbaINT.h"
#include "AmbaIPC.h"
#endif


#if defined(ASIL_SAFETY_MONITOR)
#define CV_IPC_CMD_TYPE_CONFIG          (1U)
#define CV_IPC_CMD_TYPE_REPORT_ERROR    (2U)
#define CV_IPC_CMD_TYPE_TIMESTAMP       (3U)
#define CV_IPC_CMD_TYPE_HEARTBEAT       (4U)

typedef struct {
    uint32_t Type;
    uint32_t Data[7U];
} CV_IPC_CMD_s;

typedef struct {
    uint32_t Type;
    uint32_t ModuleID;
    uint32_t BitMask;
} CV_IPC_CMD_CONFIG_s;

typedef struct {
    uint32_t Type;
    uint32_t ModuleID;
    uint32_t InstanceID;
    uint32_t ApiID;
    uint32_t ErrorID;
} CV_IPC_CMD_REPORT_ERROR_s;

typedef struct {
    uint32_t Type;
    uint32_t ModuleID;
    uint32_t SlotID;
    uint32_t ErrorID;
    uint32_t Flag;
    uint32_t Timeout;
} CV_IPC_CMD_TIMESTAMP_s;

typedef struct {
    UINT32 Type;
    UINT32 IntId;
    UINT64 Addr;
} CV_IPC_CMD_HEARTBEAT_s;


typedef struct {
    AMBA_IPC_HANDLE             Ipc;
    pmutex_t                    Mutex;
} ERRMGR_CTRL_s;

typedef struct {
    uint32_t MgrInit;
    uint32_t ErrMask[CV_MODULE_ID_NUM];
    uint32_t TimestampFlag[CV_TIMESTAMP_SLOT_NUM];
} ERRMGR_STAT_s;

static ERRMGR_CTRL_s MgrCtrl GNU_SECTION_NOZEROINIT;
static ERRMGR_STAT_s MgrStat = {0};

static uint32_t errmgr_message_proc_config(const CV_IPC_CMD_s *pCmd)
{
    uint32_t Rval = ERRCODE_NONE;
    const CV_IPC_CMD_CONFIG_s *pBitMaskCmd = NULL;

    if (pCmd == NULL) {
        console_printU5("[ERROR] errmgr_message_proc_config(): pCmd == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&pBitMaskCmd, &pCmd);
        MgrStat.ErrMask[pBitMaskCmd->ModuleID - CV_MODULE_ID_BASE] = pBitMaskCmd->BitMask;
    }

    return Rval;
}

static INT32 errmgr_message_handler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    uint32_t Rval;
    const CV_IPC_CMD_s *Cmd;

    AmbaMisra_TouchUnused(IpcHandle);
    AmbaMisra_TouchUnused(pMsgCtrl);
    if (pMsgCtrl == NULL) {
        console_printU5("[ERROR] errmgr_message_handler(): pMsgCtrl == NULL", 0U, 0U, 0U, 0U, 0U);
    } else if (pMsgCtrl->Length > sizeof(CV_IPC_CMD_s)) {
        console_printU5("[ERROR] errmgr_message_handler(): Invalid ipc cmd size (%d, max %d)", pMsgCtrl->Length, sizeof(CV_IPC_CMD_s), 0U, 0U, 0U);
    } else {
        AmbaMisra_TypeCast(&Cmd, &pMsgCtrl->pMsgData);
        if (Cmd == NULL) {
            console_printU5("[ERROR] errmgr_message_handler(): Cmd == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            switch (Cmd->Type) {
            case CV_IPC_CMD_TYPE_CONFIG:
                Rval = errmgr_message_proc_config(Cmd);
                if (Rval != ERRCODE_NONE) {
                    console_printU5("[ERROR] errmgr_message_handler(): errmgr_message_proc_config fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                }
                break;
            default:
                console_printU5("[ERROR] errmgr_message_handler(): unknown type 0x%x", Cmd->Type, 0U, 0U, 0U, 0U);
                break;
            }
        }
    }

    return 0;
}
#endif

uint32_t errmgr_ipc_send_timestamp_start(uint32_t ModuleID, uint32_t SlotID, uint32_t ErrorID, uint32_t Timeout)
{
    uint32_t Rval = ERRCODE_NONE;

#if defined(ASIL_SAFETY_MONITOR)
    CV_IPC_CMD_TIMESTAMP_s Cmd;

    if (MgrStat.MgrInit == 0U) {
        console_printU5("[ERROR] errmgr_ipc_send_timestamp_start(): errmgr not init", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_IPC_UNAVAILABLE;
    } else if ((ModuleID < CV_MODULE_ID_BASE) || (ModuleID >= (CV_MODULE_ID_BASE + CV_MODULE_ID_NUM))) {
        console_printU5("[ERROR] errmgr_ipc_send_timestamp_start(): unknown module id 0x%x", ModuleID, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_MODULE_ID_UNKNOW;
    } else if (SlotID >= CV_TIMESTAMP_SLOT_NUM) {
        console_printU5("[ERROR] errmgr_ipc_send_timestamp_start(): slot id is out of range", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_TIMESTAMP_SLOT_OUT_OF_RANGE;
    } else {
        Cmd.Type = CV_IPC_CMD_TYPE_TIMESTAMP;
        Cmd.ModuleID = ModuleID;
        Cmd.SlotID = SlotID;
        Cmd.ErrorID = ErrorID;
        Cmd.Flag = CV_TIMESTAMP_FLAG_START;
        Cmd.Timeout = Timeout;
        Rval = (uint32_t)AmbaIPC_Send(MgrCtrl.Ipc, &Cmd, (int32_t)sizeof(Cmd));
        if (Rval != 0U) {
            console_printU5("[ERROR] errmgr_ipc_send_timestamp_start(): AmbaIPC_Send fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
            Rval = ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL;
        } else {
            Rval = thread_mutex_lock(&MgrCtrl.Mutex);
            if (Rval != ERRCODE_NONE) {
                console_printU5("[ERROR] errmgr_ipc_send_timestamp_start(): thread_mutex_lock fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            } else {
                MgrStat.TimestampFlag[SlotID] = Cmd.Flag;
                Rval = thread_mutex_unlock(&MgrCtrl.Mutex);
                if (Rval != ERRCODE_NONE) {
                    console_printU5("[ERROR] errmgr_ipc_send_timestamp_start(): thread_mutex_unlock fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                    Rval = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                }
            }
        }
    }
#else
    (void) ModuleID;
    (void) SlotID;
    (void) ErrorID;
    (void) Timeout;
#endif

    return Rval;
}

uint32_t errmgr_ipc_send_timestamp_stop(uint32_t ModuleID, uint32_t SlotID, uint32_t ErrorID)
{
    uint32_t Rval = ERRCODE_NONE;

#if defined(ASIL_SAFETY_MONITOR)
    CV_IPC_CMD_TIMESTAMP_s Cmd;

    if (MgrStat.MgrInit == 0U) {
        console_printU5("[ERROR] errmgr_ipc_send_timestamp_stop(): errmgr not init", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_IPC_UNAVAILABLE;
    } else if ((ModuleID < CV_MODULE_ID_BASE) || (ModuleID >= (CV_MODULE_ID_BASE + CV_MODULE_ID_NUM))) {
        console_printU5("[ERROR] errmgr_ipc_send_timestamp_stop(): unknown module id 0x%x", ModuleID, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_MODULE_ID_UNKNOW;
    } else if (SlotID >= CV_TIMESTAMP_SLOT_NUM) {
        console_printU5("[ERROR] errmgr_ipc_send_timestamp_stop(): slot id is out of range", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_TIMESTAMP_SLOT_OUT_OF_RANGE;
    } else {
        Cmd.Type = CV_IPC_CMD_TYPE_TIMESTAMP;
        Cmd.ModuleID = ModuleID;
        Cmd.SlotID = SlotID;
        Cmd.ErrorID = ErrorID;
        Cmd.Flag = CV_TIMESTAMP_FLAG_STOP;
        Cmd.Timeout = 0U;
        Rval = (uint32_t)AmbaIPC_Send(MgrCtrl.Ipc, &Cmd, (int32_t)sizeof(Cmd));
        if (Rval != 0U) {
            console_printU5("[ERROR] errmgr_ipc_send_timestamp_stop(): AmbaIPC_Send fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
            Rval = ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL;
        } else {
            Rval = thread_mutex_lock(&MgrCtrl.Mutex);
            if (Rval != ERRCODE_NONE) {
                console_printU5("[ERROR] errmgr_ipc_send_timestamp_stop(): thread_mutex_lock fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
            } else {
                MgrStat.TimestampFlag[SlotID] = Cmd.Flag;
                Rval = thread_mutex_unlock(&MgrCtrl.Mutex);
                if (Rval != ERRCODE_NONE) {
                    console_printU5("[ERROR] errmgr_ipc_send_timestamp_stop(): thread_mutex_unlock fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                    Rval = ERR_DRV_SCHDR_MUTEX_UNLOCK_FAIL;
                }
            }
        }
    }
#else
    (void) ModuleID;
    (void) SlotID;
    (void) ErrorID;
#endif

    return Rval;
}

uint32_t errmgr_get_timestamp_slot(uint32_t *SlotID)
{
    uint32_t Rval = ERRCODE_NONE;
#if defined(ASIL_SAFETY_MONITOR)
    uint32_t Break = 0U;
    uint32_t Count = 0U;
    static uint32_t Index = 0U;

    if (MgrStat.MgrInit == 0U) {
        console_printU5("[ERROR] errmgr_get_timestamp_slot(): errmgr not init", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_IPC_UNAVAILABLE;
    } else {
        Rval = thread_mutex_lock(&MgrCtrl.Mutex);
        if (Rval != ERRCODE_NONE) {
            console_printU5("[ERROR] errmgr_get_timestamp_slot(): thread_mutex_lock fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
            Rval = ERR_DRV_SCHDR_MUTEX_LOCK_FAIL;
        } else {
            while (Break == 0U) {
                if (MgrStat.TimestampFlag[Index] == CV_TIMESTAMP_FLAG_STOP) {
                    *SlotID = Index;
                    Break = 1U;
                }

                if (Index >= (CV_TIMESTAMP_SLOT_NUM - 1U)) {
                    Index = 0U;
                } else {
                    Index++;
                }

                Count++;
                if (Count > CV_TIMESTAMP_SLOT_NUM) {
                    console_printU5("[ERROR] errmgr_get_timestamp_slot(): timestamp array is full, please enlarge slot num", 0U, 0U, 0U, 0U, 0U);
                    Break = 1U;
                    Rval = ERR_INTF_ERRMGR_TIMESTAMP_ENTRY_UNAVAILABLE;
                }
            }

            if (thread_mutex_unlock(&MgrCtrl.Mutex) != ERRCODE_NONE) {
                console_printU5("[ERROR] errmgr_get_timestamp_slot(): thread_mutex_unlock fail", 0U, 0U, 0U, 0U, 0U);
            }
        }
    }
#else
    (void) SlotID;
    thread_unused(SlotID);

#endif

    return Rval;
}

uint32_t errmgr_report_error(uint32_t ModuleID, uint32_t ErrorID)
{
    uint32_t Rval = ERRCODE_NONE;
    uint32_t ErrBase = 0U;
#if defined(ASIL_SAFETY_MONITOR)
    CV_IPC_CMD_REPORT_ERROR_s Cmd;
#endif

    switch (ModuleID) {
    case CV_MODULE_ID_DRIVER:
        if(is_drv_interface_err(ErrorID) == 1U) {
            ErrBase = CV_BITMASK_INTF;
        } else if (is_drv_scheduler_err(ErrorID) == 1U) {
            ErrBase = CV_BITMASK_SCHDR;
            schdr_sys_state.state= FLEXIDAG_SCHDR_DRV_SAFE;
        } else if (is_drv_flexidag_err(ErrorID) == 1U) {
            ErrBase = CV_BITMASK_FLEXIDAG;
        } else {
            console_printU5("[ERROR] errmgr_report_error(): unknown error id 0x%x", ErrorID, 0U, 0U, 0U, 0U);
            Rval = ERR_INTF_ERRMGR_ERROR_ID_UNKNOW;
        }
        break;
    case CV_MODULE_ID_UCODE:
        ErrBase = CV_BITMASK_SCHDR;
        break;
    default:
        console_printU5("[ERROR] errmgr_message_handler(): unknown module id 0x%x", ModuleID, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_MODULE_ID_UNKNOW;
        break;
    }

#if defined(ASIL_SAFETY_MONITOR)
    if (MgrStat.MgrInit == 0U) {
        console_printU5("[ERROR] errmgr_report_error(): errmgr not init", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_IPC_UNAVAILABLE;
    } else {
        if (MgrStat.ErrMask[ModuleID - CV_MODULE_ID_BASE] != 0U) {
            if ((MgrStat.ErrMask[ModuleID - CV_MODULE_ID_BASE] & ErrBase) != 0U) {
                Cmd.Type = CV_IPC_CMD_TYPE_REPORT_ERROR;
                Cmd.ModuleID = ModuleID;
                Cmd.InstanceID = 0U;
                Cmd.ApiID = 0U;
                Cmd.ErrorID = ErrorID;
                Rval = (uint32_t)AmbaIPC_Send(MgrCtrl.Ipc, &Cmd, (int32_t)sizeof(Cmd));
                if (Rval != 0U) {
                    console_printU5("[ERROR] errmgr_report_error(): AmbaIPC_Send fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                    Rval = ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL;
                }
            }
        }
    }
#else
    (void) ErrBase;
    (void) ModuleID;
    (void) ErrorID;
#endif

    return Rval;
}

uint32_t errmgr_ipc_send_heartbeat_config(uint64_t Addr)
{
    uint32_t Rval = ERRCODE_NONE;
#if defined(ASIL_SAFETY_MONITOR)
    CV_IPC_CMD_HEARTBEAT_s Cmd;

    if (MgrStat.MgrInit == 0U) {
        console_printU5("[ERROR] errmgr_ipc_send_heartbeat_config(): errmgr not init", 0U, 0U, 0U, 0U, 0U);
        Rval = ERR_INTF_ERRMGR_IPC_UNAVAILABLE;
    } else {
        Cmd.Type = CV_IPC_CMD_TYPE_HEARTBEAT;
        Cmd.IntId = AMBA_INT_SPI_ID196_VORC_THREAD3_IRQ;
        Cmd.Addr = (uint64_t)Addr + 0x20000U;
        Rval = (uint32_t)AmbaIPC_Send(MgrCtrl.Ipc, &Cmd, (int32_t)sizeof(Cmd));
        if (Rval != 0U) {
            console_printU5("[ERROR] errmgr_ipc_send_heartbeat_config(): AmbaIPC_Send fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
            Rval = ERR_DRV_SCHDR_ERRMGR_IPC_SEND_FAIL;
        }
    }
#else
    (void) Addr;
#endif

    return Rval;
}

/**
* initialization of AmbaCV error manager
* @return 0 - OK, others - error code
*/
uint32_t errmgr_init(void)
{
    uint32_t Rval = ERRCODE_NONE;
#if defined(ASIL_SAFETY_MONITOR)
    uint32_t i;

    if (MgrStat.MgrInit == 0U) {
        for (i=0U; i < CV_MODULE_ID_NUM; i++) {
            MgrStat.ErrMask[i] = CV_BITMASK_ALL;
        }

        MgrCtrl.Ipc = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "CVMonitorIpc", errmgr_message_handler);
        if (MgrCtrl.Ipc == NULL) {
            console_printU5("[ERROR] errmgr_init(): AmbaIPC_Alloc fail", 0U, 0U, 0U, 0U, 0U);
            Rval = ERR_DRV_SCHDR_ERRMGR_IPC_ALLOC_FAIL;
        } else {
            Rval = (uint32_t)AmbaIPC_RegisterChannel(MgrCtrl.Ipc, NULL);
            if (Rval != 0U) {
                console_printU5("[ERROR] errmgr_init(): AmbaIPC_RegisterChannel fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_DRV_SCHDR_ERRMGR_IPC_REGISRER_FAIL;
            }
        }

        if (Rval == ERRCODE_NONE) {
            Rval = thread_mutex_init(&MgrCtrl.Mutex);
            if (Rval != ERRCODE_NONE) {
                console_printU5("[ERROR] errmgr_init(): thread_mutex_init fail return 0x%x", Rval, 0U, 0U, 0U, 0U);
                Rval = ERR_DRV_SCHDR_MUTEX_INIT_FAIL;
            }
        }

        if (Rval == ERRCODE_NONE) {
            MgrStat.MgrInit = 1U;
        }
    } else {
        console_printU5("errmgr_init(): already created.", 0U, 0U, 0U, 0U, 0U);
    }
#endif

    return Rval;
}
