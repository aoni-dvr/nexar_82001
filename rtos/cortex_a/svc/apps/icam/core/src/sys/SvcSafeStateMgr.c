/**
*  @file SvcSafeStateMgr.c
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
*  @details svc IPC functions
*
*/

#include "AmbaSYS.h"
#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
#include "AmbaIPC.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Liveview_Def.h"
#if defined(CONFIG_ENABLE_DSP_MONITOR)
#include "AmbaDSP_Monitor.h"
#endif
#if defined(CONFIG_BUILD_IP_MONITOR)
#include "Amba_IPMonitor.h"
#endif
#if defined(CONFIG_BUILD_CV)
#include "cvapi_ambacv_flexidag.h"
#endif
#if defined(CONFIG_ICAM_FUSA_USED)
#include "AmbaFusa.h"
#endif
#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
#include "AmbaVIN_Priv.h"
#include "SvcImg.h"
#endif
#include "AmbaVIN.h"

#endif
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcPlat.h"
#include "SvcSafeStateMgr.h"

#define SVC_LOG_ASSM     "SVC_ASSM"

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)

#define SVC_IPC_CMD_WAIT_SYS_READY     (0U)
#define SVC_IPC_CMD_SET_CLK            (1U)
#define SVC_IPC_CMD_ENABLE_FEATURE     (2U)
#define SVC_IPC_CMD_DISABLE_FEATURE    (3U)
#define SVC_IPC_CMD_GET_ERR_ALL        (4U)
#define SVC_IPC_CMD_GET_ERR_0          (5U)
#define SVC_IPC_CMD_GET_ERR_1          (6U)
#define SVC_IPC_CMD_CHECK_SENSOR_EMBD  (7U)
#define SVC_IPC_CMD_ENABLE_VP_BIST     (8U)
#define SVC_IPC_CMD_ENABLE_FUSA        (9U)
#define SVC_IPC_CMD_INFORM_IK_RROR    (10U)
#define SVC_IPC_CMD_SET_PWR_MONTOR    (11U)
#ifdef CONFIG_ICAM_PROJECT_SHMOO
#define SVC_IPC_CMD_SND_DDRSHM_INFO   (12U)
#endif
#define SVC_IPC_CMD_CHK_DDRSHM_STATUS (13U)
#define SVC_IPC_CMD_SET_IDSP_WDT      (14U)
#define SVC_IPC_CMD_INFORM_LIV_RDY    (15U)
#define SVC_IPC_CMD_PREPARE_LIV_STOP  (16U)

#define SVC_IPC_NUM_CLK                (4U)

#define WAIT_R52_ACK_TIMEOUT           (5000U)

typedef struct {
    UINT32                  CmdCode;
    SVC_SAFE_STATE_ERR_0_s  ErrInfo0;
    SVC_SAFE_STATE_ERR_1_s  ErrInfo1;
} SVC_IPC_SYS_ERROR_s;

typedef struct {          //!< share the same structure for MSG_QUE and IPC
    UINT32      CmdCode;
    UINT32      Data[15]; //!< the maximun size for one MSG is 4 * 16
} SVC_IPC_SYS_CMD_s;

typedef struct {
    UINT32     ClkID;
    UINT32     ActualFreq;
} SVC_IPC_CLK_s;

typedef struct {
    AMBA_IPC_HANDLE         Sys;
    AMBA_KAL_MSG_QUEUE_t    TxQue;
    SVC_IPC_SYS_CMD_s       TxQueBuf[10];
    AMBA_KAL_MSG_QUEUE_t    RxQue;
    SVC_IPC_SYS_CMD_s       RxQueBuf[10];
    AMBA_KAL_MUTEX_t        SysMutex;
    SVC_IPC_SYS_CMD_s       IpcCmd;
    SVC_TASK_CTRL_s         Task;
} SVC_ASSM_CTRL_s;

static SVC_ASSM_CTRL_s  SvcSafeStateMgr GNU_SECTION_NOZEROINIT;

static SVC_IPC_CLK_s    SvcIpcClkMap[SVC_IPC_NUM_CLK] = {
    {AMBA_SYS_CLK_CORE,   0},
    {AMBA_SYS_CLK_IDSP,   0},
    {AMBA_SYS_CLK_VISION, 0},
    {AMBA_SYS_CLK_FEX,    0},
};

static UINT8 ASSM_DebugOn = 0;
#endif

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
static SVC_SAFE_STATE_CALLBACK_f pEnableVpuBist = NULL;
static SVC_SAFE_STATE_DDRSHM_CALLBACK_f pChkDdrShm = NULL;
#endif

/**
* register safe state manager callback functions
* @param [in] pCallback callback functions
* @return none
*/
void SvcSafeStateMgr_RegisterCallback(const SVC_SAFE_STATE_CALLBACK_s *pCallback)
{
    if (pCallback != NULL) {
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
        pEnableVpuBist = pCallback->pEnableVpuBist;
        pChkDdrShm = pCallback->pChkDdrShm;
#else
        // misra-c
#endif
    }
}

/**
* enable SvcSafeStateMgr debug message
* @param [in] Enable 1 - enable; 0 - disable
*/
void SvcSafeStateMgr_Debug(UINT8 Enable)
{
#if !defined(CONFIG_FWPROG_R52SYS_ENABLE)
    static UINT8 ASSM_DebugOn = 0;
#endif
    ASSM_DebugOn = Enable;
    SvcLog_OK(SVC_LOG_ASSM, "SvcSafeStateMgr debug %u", ASSM_DebugOn, 0U);
}

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
static INT32 IpcSys_MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    UINT32 Rval;
    SVC_IPC_SYS_ERROR_s Info = {0};
    SVC_IPC_SYS_CMD_s Cmd;

    AmbaMisra_TouchUnused(IpcHandle);
    AmbaMisra_TouchUnused(pMsgCtrl);
    AmbaSvcWrap_MisraMemcpy(&Info, pMsgCtrl->pMsgData, pMsgCtrl->Length);

    SvcLog_OK(SVC_LOG_ASSM, "[IpcSys_MsgHandler] get %u length %u", Info.CmdCode, pMsgCtrl->Length);
    if (Info.CmdCode == SVC_IPC_CMD_GET_ERR_ALL) {
        Cmd.CmdCode = SVC_IPC_CMD_GET_ERR_0;
        AmbaSvcWrap_MisraMemcpy(&Cmd.Data[0], &Info.ErrInfo0, sizeof(SVC_SAFE_STATE_ERR_0_s));
        Rval = AmbaKAL_MsgQueueSend(&SvcSafeStateMgr.RxQue, &Cmd, 1000);
        if (SVC_OK == Rval) {
            Cmd.CmdCode = SVC_IPC_CMD_GET_ERR_1;
            AmbaSvcWrap_MisraMemcpy(&Cmd.Data[0], &Info.ErrInfo1, sizeof(SVC_SAFE_STATE_ERR_1_s));
            Rval = AmbaKAL_MsgQueueSend(&SvcSafeStateMgr.RxQue, &Cmd, 1000);
        }
    } else {
        if ((Info.CmdCode == SVC_IPC_CMD_ENABLE_FUSA) ||
            (Info.CmdCode == SVC_IPC_CMD_INFORM_IK_RROR) ||
            (Info.CmdCode == SVC_IPC_CMD_ENABLE_VP_BIST) ||
            (Info.CmdCode == SVC_IPC_CMD_CHK_DDRSHM_STATUS)) {
            Rval = AmbaKAL_MsgQueueSend(&SvcSafeStateMgr.RxQue, &Info, 1000);
        } else {
            Rval = AmbaKAL_MsgQueueSend(&SvcSafeStateMgr.TxQue, &Info, 1000);
        }
    }
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_ASSM, "AmbaKAL_MsgQueueSend failed! return 0x%x", Rval, 0U);
    }
    return 0;
}

static void SvcSafeStateMgr_GetError(const AMBA_SEMGR_ERROR_INFO_s *pErrorInfo)
{
    UINT32 i;
    const AMBA_SEMGR_CHIP_ERROR_s *pChipErr;

    pChipErr = &(pErrorInfo->ChipError);
    SvcLog_DBG(SVC_LOG_ASSM, "System Error Occurs !", 0, 0);
    SvcLog_DBG(SVC_LOG_ASSM, "    Flags: 0x%X", pErrorInfo->Flags, 0U);
    for (i = 0U; i < 5U; i++) {
        SvcLog_DBG(SVC_LOG_ASSM, "    CEHU error bit[%d] = 0x%X", i, pChipErr->CehuErrorArray[i]);
    }
    for (i = 0U; i < 4U; i++) {
        SvcLog_DBG(SVC_LOG_ASSM, "    ECRU error bit[%d] = 0x%X", i, pChipErr->EcruErrorArray[i]);
    }
    {
        INT32 Err;
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_GET_ERR_ALL;
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_GetError] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 4);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_GetError] AmbaIPC_Send return 0x%x", (UINT32)Err, 0U);
        }
    }
}

static void StoreClkActualFreq(UINT32 ClkID, UINT32 ActualFreq)
{
    UINT8 i;
    for (i = 0; i < SVC_IPC_NUM_CLK; i ++) {
        if (SvcIpcClkMap[i].ClkID == ClkID) {
            SvcIpcClkMap[i].ActualFreq = ActualFreq;
            break;
        }
    }
}

static void SvcSafeStateMgr_InformIkError(void)
{
#if defined(CONFIG_BUILD_COMSVC_IMGFRW)
    UINT32 VinId;
    for (VinId = 0; VinId < AMBA_DSP_MAX_VIN_NUM; VinId ++) {
        SvcImg_SyncEnable(VinId, 0);
    }
#endif
    if (ASSM_DebugOn == 1U) {
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_InformIkError] got IK error at R52", 0, 0U);
    }
}

#ifdef CONFIG_ICAM_SENSOR_ASIL_ENABLED
UINT32 SvcSafeStateMgr_CheckVinEmbd(const SVC_SAFE_STATE_MGR_VIN_EMBD_s *pInfo)
{
    UINT32 Rval;
    INT32 Err;
    SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_CHECK_SENSOR_EMBD;
    SvcSafeStateMgr.IpcCmd.Data[0] = pInfo->VinID;
    SvcSafeStateMgr.IpcCmd.Data[1] = pInfo->SensorID;
    SvcSafeStateMgr.IpcCmd.Data[2] = pInfo->FrameCnt;
    SvcSafeStateMgr.IpcCmd.Data[3] = pInfo->EmbCrc;
    SvcSafeStateMgr.IpcCmd.Data[4] = pInfo->DataCrc;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[CheckSensorEmbeddedData] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 24);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[CheckSensorEmbeddedData] AmbaIPC_Send return %u", (UINT32)Err, 0U);
            Rval = (UINT32)Err;
        }
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[CheckSensorEmbeddedData] AmbaKAL_MutexGive return %u", Rval, 0U);
        }
    }
    return Rval;
}
#endif

static void ReturnDdrShmStatus(UINT32 Flag)
{
    UINT32 Rval;
    INT32 Err;
    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_CHK_DDRSHM_STATUS;
        SvcSafeStateMgr.IpcCmd.Data[0] = Flag;

        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[ReturnDdrShmStatus] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 8);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[ReturnDdrShmStatus] AmbaIPC_Send return %u", (UINT32)Err, 0U);
            Rval = (UINT32)Err;
        }
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[ReturnDdrShmStatus] AmbaKAL_MutexGive return %u", Rval, 0U);
        }
    }
}

static void *SvcSafeStateMgr_TaskEntry(void *EntryArg)
{
    ULONG ArgVal = 0U;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

    while (ArgVal != 0xCafeU) {
        SVC_IPC_SYS_CMD_s Cmd;
        UINT32 Rval;
        AMBA_SEMGR_ERROR_INFO_s ErrInfo;
        Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.RxQue, &Cmd, AMBA_KAL_WAIT_FOREVER);
        if (Rval == OK) {
            if (ASSM_DebugOn == 1U) {
                SvcLog_OK(SVC_LOG_ASSM, "AmbaKAL_MsgQueueReceive get %u", Cmd.CmdCode, 0U);
            }
            switch (Cmd.CmdCode) {
                case SVC_IPC_CMD_GET_ERR_0:
                    AmbaSvcWrap_MisraMemcpy(&ErrInfo, &Cmd.Data[0], sizeof(SVC_SAFE_STATE_ERR_0_s));
                    Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.RxQue, &Cmd, 5000);
                    if (Rval == SVC_OK) {
                        if (Cmd.CmdCode != SVC_IPC_CMD_GET_ERR_1) {
                            SvcLog_NG(SVC_LOG_ASSM, "msg que order wrong! CmdCode %u", Cmd.CmdCode, 0U);
                        } else {
                            ULONG Addr;
                            void *pTemp = &ErrInfo;
                            AmbaMisra_TypeCast(&Addr, &pTemp);
                            Addr += sizeof(SVC_SAFE_STATE_ERR_0_s);
                            AmbaMisra_TypeCast(&pTemp, &Addr);
                            AmbaSvcWrap_MisraMemcpy(pTemp, &Cmd.Data[0], sizeof(SVC_SAFE_STATE_ERR_1_s));
                            SvcSafeStateMgr_GetError(&ErrInfo);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_ASSM, "wait msg fail 0x%x", Rval, 0U);
                    }
                    break;
                case SVC_IPC_CMD_CHECK_SENSOR_EMBD:
                    break;
                case SVC_IPC_CMD_ENABLE_VP_BIST:
                    if (pEnableVpuBist != NULL) {
                        UINT8 DisableFex;
                        if (Cmd.Data[1] <= 1U) {
                            DisableFex = (UINT8)Cmd.Data[1];
                            pEnableVpuBist(Cmd.Data[0], DisableFex);
                            SvcLog_DBG(SVC_LOG_ASSM, "VPU BIST enabled", 0, 0U);
                        } else {
                            SvcLog_NG(SVC_LOG_ASSM, "SVC_IPC_CMD_ENABLE_VP_BIST with wrong cmd data %u", Cmd.Data[1], 0U);
                        }
                    }
                    break;
                case SVC_IPC_CMD_INFORM_IK_RROR:
                    SvcSafeStateMgr_InformIkError();
                    break;
                case SVC_IPC_CMD_CHK_DDRSHM_STATUS:
                    if (pChkDdrShm != NULL) {
                        UINT32 Flag = 0;
                        ULONG Addr;
#ifdef CONFIG_ARM64
                        Addr = (ULONG)Cmd.Data[1] + ((ULONG)Cmd.Data[2] << 32UL);
#else
                        Addr = (ULONG)Cmd.Data[1];
#endif
                        pChkDdrShm(Cmd.Data[0], Addr, &Flag);
                        SvcLog_DBG(SVC_LOG_ASSM, "pChkDdrShm %u %u", Cmd.Data[0], Cmd.Data[1]);
                        if (Cmd.Data[0] == 0U) {
                            SvcLog_DBG(SVC_LOG_ASSM, "pChkDdrShm get %u", Flag, 0U);
                            ReturnDdrShmStatus(Flag);
                        }
                    }
                    break;
                case SVC_IPC_CMD_ENABLE_FUSA:
#if defined(CONFIG_ICAM_FUSA_USED) && !defined(CONFIG_QNX)
                    AmbaCortexA53FusaInit(Cmd.Data[0], Cmd.Data[1]);
                    SvcLog_DBG(SVC_LOG_ASSM, "AmbaCortexA53FusaInit done", 0, 0U);
#endif
                    break;
                default:
                    SvcLog_NG(SVC_LOG_ASSM, "not supported ipc cmd 0x%x", Cmd.CmdCode, 0U);
                    break;
            }
        }

        AmbaMisra_TouchUnused(&ArgVal);
    }

    return NULL;
}
#endif

/**
* initialization of svc ipc
* @return 0 - OK, 1 - NG
*/
UINT32 SvcSafeStateMgr_Init(const SVC_SAFE_STATE_MGR_TSK_CFG_s *pCfg)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    INT32 Err;
    UINT32 Rval;
    UINT16 Module;
    static char TxQueName[] = "SvcSafeStateMgrTxQue";
    static char RxQueName[] = "SvcSafeStateMgrRxQue";
    static char SysMutexName[] = "SvcSafeStateMgrMutex";
    static UINT8 SafeStateMgrStack[SVC_SAFE_STATE_MGR_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT8 SvcSafeStateMgr_Created = 0;

    if (SvcSafeStateMgr_Created == 0U) {
        SvcLog_DBG(SVC_LOG_ASSM, "SvcSafeStateMgr_Init() start", 0U, 0U);
        Rval = AmbaWrap_memset(&SvcSafeStateMgr, 0, sizeof(SvcSafeStateMgr));
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_ASSM, "AmbaWrap_memset() return 0x%x", Rval, 0U);
        }
        if (ASSM_DebugOn == 1U) {
            Module = (UINT16)(AMBALINK_ERR_BASE >> 16U);
            Rval = AmbaPrint_ModuleSetAllowList(Module, 1);
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaPrint_ModuleSetAllowList %u return %u", Module, Rval);
            }
        }
#if !defined(CONFIG_QNX)
        SvcLog_DBG(SVC_LOG_ASSM, "AmbaIPC_Init() start", 0U, 0U);
        Rval = AmbaIPC_Init();
        if (SVC_OK != Rval) {
            SvcLog_NG(SVC_LOG_ASSM, "AmbaIPC_Init() return 0x%x", Rval, 0U);
        } else {
            SvcLog_DBG(SVC_LOG_ASSM, "AmbaIPC_Init() done", 0U, 0U);
        }
#endif
#if defined(CONFIG_ENABLE_DSP_MONITOR)
        if (Rval == OK) {
            SvcLog_DBG(SVC_LOG_ASSM, "AmbaDSP_MonitorInit() start", 0U, 0U);
            Rval = AmbaDSP_MonitorInit();
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaDSP_MonitorInit return %d", Rval, 0);
            } else {
                SvcLog_DBG(SVC_LOG_ASSM, "AmbaDSP_MonitorInit() done", 0U, 0U);
            }
        }
#endif

#if defined(CONFIG_BUILD_IP_MONITOR)
        if (Rval == OK) {
            SvcLog_DBG(SVC_LOG_ASSM, "AmbaIP_MonitorInit() start", 0U, 0U);
            Rval = AmbaIP_MonitorInit();
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaIP_MonitorInit return %d", Rval, 0);
            } else {
                SvcLog_DBG(SVC_LOG_ASSM, "AmbaIP_MonitorInit() done", 0U, 0U);
            }
        }
#endif

#if defined(CONFIG_ENABLE_CV_MONITOR)
        if (SVC_OK == Rval) {
            SvcLog_DBG(SVC_LOG_ASSM, "AmbaCV_IpcInit() start", 0U, 0U);
            Rval = AmbaCV_IpcInit();
            if (Rval != OK) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaCV_IpcInit return %d", Rval, 0U);
            } else {
                SvcLog_DBG(SVC_LOG_ASSM, "AmbaCV_IpcInit() done", 0U, 0U);
            }
        }
#endif

        if (SVC_OK == Rval) {
            SvcLog_DBG(SVC_LOG_ASSM, "AmbaIPC_Alloc() start", 0U, 0U);
            SvcSafeStateMgr.Sys = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "SvcSafeStateMgr_SYS", IpcSys_MsgHandler); /* the string "SvcSafeStateMgr_SYS" should be the same in R52 */
            //SvcLog_OK(SVC_LOG_ASSM, "AmbaIPC_Alloc return 0x%x", *((UINT32*)SvcSafeStateMgr.Sys), 0U);
            SvcLog_DBG(SVC_LOG_ASSM, "AmbaIPC_Alloc() done", 0U, 0U);

            SvcLog_DBG(SVC_LOG_ASSM, "AmbaIPC_RegisterChannel() start", 0U, 0U);
            Err = AmbaIPC_RegisterChannel(SvcSafeStateMgr.Sys, NULL);
            SvcLog_OK(SVC_LOG_ASSM, "AmbaIPC_RegisterChannel return %d", (UINT32)Err, 0U);

            Rval = AmbaKAL_MsgQueueCreate(&SvcSafeStateMgr.TxQue, TxQueName, sizeof(SVC_IPC_SYS_CMD_s), SvcSafeStateMgr.TxQueBuf, sizeof(SvcSafeStateMgr.TxQueBuf));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaKAL_MsgQueueCreate failed! return 0x%x", Rval, 0U);
            }
        }
        if (SVC_OK == Rval) {
            Rval = AmbaKAL_MsgQueueCreate(&SvcSafeStateMgr.RxQue, RxQueName, sizeof(SVC_IPC_SYS_CMD_s), SvcSafeStateMgr.RxQueBuf, sizeof(SvcSafeStateMgr.RxQueBuf));
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaKAL_MsgQueueCreate failed! return 0x%x", Rval, 0U);
            }
        }
        if (SVC_OK == Rval) {
            Rval = AmbaKAL_MutexCreate(&SvcSafeStateMgr.SysMutex, SysMutexName);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_ASSM, "AmbaKAL_MutexCreate failed! return 0x%x", Rval, 0U);
            }
        }
        if (SVC_OK == Rval) {
            /* task create */
            SvcSafeStateMgr.Task.Priority    = pCfg->Priority;
            SvcSafeStateMgr.Task.EntryFunc   = SvcSafeStateMgr_TaskEntry;
            SvcSafeStateMgr.Task.EntryArg    = 0U;
            SvcSafeStateMgr.Task.pStackBase  = SafeStateMgrStack;
            SvcSafeStateMgr.Task.StackSize   = SVC_SAFE_STATE_MGR_STACK_SIZE;
            SvcSafeStateMgr.Task.CpuBits     = pCfg->CpuBits;
            Rval = SvcTask_Create("SvcSafeStateMgrTask", &SvcSafeStateMgr.Task);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_ASSM, "SvcSafeStateMgrTask created failed! return 0x%x", Rval, 0U);
            }
        }
        SvcSafeStateMgr_Created = 1;
        SvcLog_DBG(SVC_LOG_ASSM, "SvcSafeStateMgr_Init() done", 0U, 0U);
    } else {
        SvcLog_DBG(SVC_LOG_ASSM, "SvcSafeStateMgr already created.", 0U, 0U);
        Rval = SVC_OK;
    }
    return Rval;
#else
    AmbaMisra_TouchUnused(&pCfg);
    return SVC_OK;
#endif
}

/**
* wait system ready via ipc
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_WaitSysReady(void)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_WAIT_SYS_READY;

        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_WaitSysReady] start", 0, 0U);
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_WaitSysReady] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 4);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_WaitSysReady] AmbaIPC_Send 0x%x return %d", SvcSafeStateMgr.IpcCmd.CmdCode, (UINT32)Err);
        }
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    if (Rval == OK) {
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_WaitSysReady] done", 0, 0U);
    }
    return Rval;
#else
    return SVC_OK;
#endif
}

/**
* setup of system clocks via ipc
* @param [in] ClkID clock index
* @param [in] DesiredFreq desired frequency
* @param [in] pActualFreq pointer to actural frequency
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_SetClkFreq(UINT32 ClkID, UINT32 DesiredFreq, UINT32 * pActualFreq)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;
    SVC_IPC_SYS_CMD_s Cmd;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_SET_CLK;
        SvcSafeStateMgr.IpcCmd.Data[0] = ClkID;
        SvcSafeStateMgr.IpcCmd.Data[1] = DesiredFreq;

        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetClkFreq] start", 0, 0U);
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetClkFreq] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 12);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetClkFreq] AmbaIPC_Send return %u", (UINT32)Err, 0U);
        }
        AmbaSvcWrap_MisraMemset(&Cmd, 0, sizeof(Cmd));
        Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.TxQue, &Cmd, WAIT_R52_ACK_TIMEOUT);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetClkFreq] AmbaKAL_MsgQueueReceive return %u", Rval, 0U);
            Rval = OK;
        }
        *pActualFreq = Cmd.Data[0];
        StoreClkActualFreq(ClkID, Cmd.Data[0]);
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetClkFreq] ActualFreq %u", *pActualFreq, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
#else
    return AmbaSYS_SetClkFreq(ClkID, DesiredFreq, pActualFreq);
#endif
}

/**
* get of system clocks
* @param [in] ClkID clock index
* @param [out] pFreq pointer to user frequency variable
* @return SVC_OK - OK, SVC_NG - ClkID not found
*/
UINT32 SvcSafeStateMgr_GetClkFreq(UINT32 ClkID, UINT32 *pFreq)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT8 i;
    UINT32 Rval = SVC_NG;
    for (i = 0; i < SVC_IPC_NUM_CLK; i ++) {
        if (SvcIpcClkMap[i].ClkID == ClkID) {
            *pFreq = SvcIpcClkMap[i].ActualFreq;
            Rval = SVC_OK;
            break;
        }
    }
    if (Rval == SVC_NG) {
        SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_GetClkFreq] ClkID %u not found", ClkID, 0U);
    }
    return Rval;
#else
    return AmbaSYS_GetClkFreq(ClkID, pFreq);
#endif
}

/**
* feature enable via ipc
* @param [in] FeatureBits bits of feature
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_EnableFeature(UINT32 SysFeature)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;
    SVC_IPC_SYS_CMD_s Cmd;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_ENABLE_FEATURE;
        SvcSafeStateMgr.IpcCmd.Data[0] = SysFeature;

        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableFeature] start", 0, 0U);
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableFeature] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 8);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableFeature] AmbaIPC_Send return %d", (UINT32)Err, 0U);
        }
        Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.TxQue, &Cmd, WAIT_R52_ACK_TIMEOUT);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableFeature] AmbaKAL_MsgQueueReceive return %u", Rval, 0U);
            Rval = OK;
        }
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableFeature] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
#else
    return AmbaSYS_EnableFeature(SysFeature);
#endif
}

/**
* feature disable via ipc
* @param [in] FeatureBits bits of feature
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_DisableFeature(UINT32 SysFeature)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;
    SVC_IPC_SYS_CMD_s Cmd;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_DISABLE_FEATURE;
        SvcSafeStateMgr.IpcCmd.Data[0] = SysFeature;

        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_DisableFeature] start", 0, 0U);
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_DisableFeature] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 8);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_DisableFeature] AmbaIPC_Send return %d", (UINT32)Err, 0U);
        }
        Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.TxQue, &Cmd, WAIT_R52_ACK_TIMEOUT);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_DisableFeature] AmbaKAL_MsgQueueReceive return %u", Rval, 0U);
            Rval = OK;
        }
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_DisableFeature] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
#else
    return AmbaSYS_DisableFeature(SysFeature);
#endif
}

UINT32 SvcSafeStateMgr_EnableVpBist(void)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_ENABLE_VP_BIST;
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableVpBist] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 4);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableVpBist] AmbaIPC_Send return %d", (UINT32)Err, 0U);
        }
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_EnableVpBist] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
#else
    return SVC_OK;
#endif
}

UINT32 SvcSafeStateMgr_SetPwrMonitor(const SVC_PWR_MONITOR_CFG_s *pCfg)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_SET_PWR_MONTOR;
        SvcSafeStateMgr.IpcCmd.Data[0] = pCfg->ModuleID;
        SvcSafeStateMgr.IpcCmd.Data[1] = pCfg->Enable;
        SvcSafeStateMgr.IpcCmd.Data[2] = pCfg->UpperBound;
        SvcSafeStateMgr.IpcCmd.Data[3] = pCfg->LowerBound;
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetPwrMonitor] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 20);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetPwrMonitor] AmbaIPC_Send return %d", (UINT32)Err, 0U);
        }
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetPwrMonitor] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
#else
    AmbaMisra_TouchUnused(&pCfg);
    return SVC_OK;
#endif
}


#if defined(CONFIG_ICAM_PROJECT_SHMOO)
/**
* send dram shmoo info
* @param [in] pCtrl dram shmoo control parameters
* @param [in] pMisc dram shmoo misc parameters
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_SndDrmShmInfo(void *pCtrl, void *pMisc, UINT32 CtrlSize, UINT32 MiscSize)
{
    UINT32  Rval;
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Size = 4U + CtrlSize + MiscSize;
    ULONG   TempAddr;
    INT32   Err;
    UINT32  *pCmdDataWp = &SvcSafeStateMgr.IpcCmd.Data[0];

    SvcLog_DBG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SndDrmShmInfo] size to send %u, capacity %u", Size, sizeof(SvcSafeStateMgr.IpcCmd));

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);

    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_SND_DDRSHM_INFO;
        AmbaSvcWrap_MisraMemcpy(&SvcSafeStateMgr.IpcCmd.Data[0], pCtrl, CtrlSize);
        AmbaMisra_TypeCast(&TempAddr, &pCmdDataWp);
        TempAddr += (ULONG)CtrlSize;
        AmbaMisra_TypeCast(&pCmdDataWp, &TempAddr);
        AmbaSvcWrap_MisraMemcpy(pCmdDataWp, pMisc, MiscSize);

        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SndDrmShmInfo] start", 0, 0U);
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SndDrmShmInfo] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, (INT32)Size);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SndDrmShmInfo] AmbaIPC_Send return %u", (UINT32)Err, 0U);
        }
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }

#else
    Rval = SVC_OK;
    AmbaMisra_TouchUnused(pCtrl);
    AmbaMisra_TouchUnused(pMisc);
    AmbaMisra_TouchUnused(&CtrlSize);
    AmbaMisra_TouchUnused(&MiscSize);
#endif
    return Rval;
}
#endif

/**
* set idsp watchdog timeout
* @param [in] TimeoutMs timeout in msec
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_SetIDspWDT(UINT32 TimeoutMs)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32  Rval;
    INT32   Err;
    SVC_IPC_SYS_CMD_s Cmd;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_SET_IDSP_WDT;
        SvcSafeStateMgr.IpcCmd.Data[0] = TimeoutMs;

        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetIDspWDT] (%u ms) start", TimeoutMs, 0U);
        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetIDspWDT] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 8);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetIDspWDT] AmbaIPC_Send return %u", (UINT32)Err, 0U);
        }
        Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.TxQue, &Cmd, WAIT_R52_ACK_TIMEOUT);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetIDspWDT] AmbaKAL_MsgQueueReceive return %u", Rval, 0U);
            Rval = OK;
        }
        SvcLog_OK(SVC_LOG_ASSM, "[SvcSafeStateMgr_SetIDspWDT] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
#else
    AmbaMisra_TouchUnused(&TimeoutMs);
    return OK;
#endif
}

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
static UINT32 InformLiveviewStart(void)
{
    UINT32  Rval;
    INT32   Err;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_INFORM_LIV_RDY;

        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[InformLiveviewStart] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 4);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[InformLiveviewStart] AmbaIPC_Send return %u", (UINT32)Err, 0U);
        }
        SvcLog_OK(SVC_LOG_ASSM, "[InformLiveviewStart] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
}

static UINT32 PrepareLiveviewStop(void)
{
    UINT32  Rval;
    INT32   Err;
    SVC_IPC_SYS_CMD_s Cmd;

    Rval = AmbaKAL_MutexTake(&SvcSafeStateMgr.SysMutex, KAL_WAIT_FOREVER);
    if (Rval == OK) {
        SvcSafeStateMgr.IpcCmd.CmdCode = SVC_IPC_CMD_PREPARE_LIV_STOP;

        if (ASSM_DebugOn == 1U) {
            SvcLog_OK(SVC_LOG_ASSM, "[PrepareLiveviewStop] AmbaIPC_Send %u", SvcSafeStateMgr.IpcCmd.CmdCode, 0U);
        }
        Err = AmbaIPC_Send(SvcSafeStateMgr.Sys, &SvcSafeStateMgr.IpcCmd, 4);
        if (Err != 0) {
            SvcLog_NG(SVC_LOG_ASSM, "[PrepareLiveviewStop] AmbaIPC_Send return %u", (UINT32)Err, 0U);
        }
        Rval = AmbaKAL_MsgQueueReceive(&SvcSafeStateMgr.TxQue, &Cmd, WAIT_R52_ACK_TIMEOUT);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_ASSM, "[PrepareLiveviewStop] AmbaKAL_MsgQueueReceive return %u", Rval, 0U);
            Rval = OK;
        }
        SvcLog_OK(SVC_LOG_ASSM, "[PrepareLiveviewStop] done", 0, 0U);
    }
    if (Rval == OK) {
        Rval = AmbaKAL_MutexGive(&SvcSafeStateMgr.SysMutex);
    }
    return Rval;
}
#endif

/**
* liveview start process
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_LiveviewStart(void)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32 VinID, Rval;
    for (VinID = 0; VinID < AMBA_NUM_VIN_CHANNEL; VinID ++) {
        Rval = AmbaDiag_VinResetStatus(VinID, NULL);
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_ASSM, "## AmbaDiag_VinResetStatus VinID %u return %u", VinID, Rval);
        }
    }
    if (OK == Rval) {
        Rval = InformLiveviewStart();
        if (OK != Rval) {
            SvcLog_NG(SVC_LOG_ASSM, "## InformLiveviewStart return %u", Rval, 0U);
        }
    }
    return Rval;
#else
    return OK;
#endif
}

/**
* liveview stop process
* @return 0 - OK, others - error code
*/
UINT32 SvcSafeStateMgr_LiveviewStop(void)
{
#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
    UINT32 Rval;
    Rval = PrepareLiveviewStop();
    if (OK != Rval) {
        SvcLog_NG(SVC_LOG_ASSM, "## PrepareLiveviewStop return %u", Rval, 0U);
    }
    return Rval;
#else
    return OK;
#endif
}
