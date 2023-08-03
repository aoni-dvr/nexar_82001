/**
 *  @file SvcCvMainTask.c
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
 *  @details svc cv main task
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaFS.h"
#include "AmbaSD.h"
#include "AmbaShell_Utility.h"

#include "AmbaDMA_Def.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_VideoDec.h"

/* svc-cv */
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_protection.h"
#include "schdr_api.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcShell.h"
#include "SvcCmd.h"
#include "SvcResCfg.h"
#include "SvcSysStat.h"
#include "SvcCvFlow.h"
#include "SvcCvFlowUtil.h"
#include "SvcTiming.h"
#include "SvcClock.h"

/* svc-app */
#include "SvcCvMainTask.h"
#include "SvcUserPref.h"
#include "SvcTaskList.h"
#include "SvcAppStat.h"

#define SVC_LOG_CV_MAIN_TASK "CV_MAIN_TASK"

#define SVC_CV_MAIN_STAT_LOAD_DONE              (0x1U)
#define SVC_CV_MAIN_STAT_NONE                   (0x2U)
#define SVC_CV_MAIN_STAT_TIMEOUT                (0x4U)
#define SVC_CV_MAIN_STAT_DONE                   (0x8U)
#define SVC_CV_MAIN_STAT_BOOT_RESUME            (0x10U)

#define SVC_CV_MAIN_BOOT_TASK_STACK_SIZE        (0xB000U)
#define SVC_CV_MAIN_MAX_CALLBACK_FUNC           (0x4U)

typedef struct {
    SVC_CV_MAIN_BOOT_FLAG_s     BootFlag;
    UINT32                      SchdrReset  :1;
    UINT32                      DefCvTblOn  :1;     /* Mainly for loading CV BIST */
    UINT32                      Reserved    :30;
} SVC_CV_MAIN_INIT_s;

static UINT32 CvMainInit = 0U;
static SVC_TASK_CTRL_s CvMainBootTaskCtrl GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t CvMainEventFlag;
static SVC_CV_MAIN_INIT_s CvMainInitFlag = {0}; /* Initialized */
static AMBA_CV_STANDALONE_SCHDR_CFG_s CvSchdrCfg = {
    .flexidag_slot_num = 10,
    .cavalry_slot_num  = 0,
#ifdef CONFIG_ICAM_CV_FLEXIDAG_TASK_CORE_SELECT
    .cpu_map           = CONFIG_ICAM_CV_FLEXIDAG_TASK_CORE_SELECT,
#else
    .cpu_map           = 0xE,
#endif
    .log_level         = LVL_DEBUG,
#if defined(CONFIG_CV_EXTERNAL_MSG_THREAD)
    .flag              = (INT32) (SCHDR_FASTBOOT | SCHDR_NO_INTERNAL_THREAD),
#else
    .flag              = (INT32) SCHDR_FASTBOOT,
#endif
    .boot_cfg          = (1 + FLEXIDAG_ENABLE_BOOT_CFG),
    .autorun_interval  = 0,
    .autorun_fex_cfg   = 0,
};
static SVC_CV_MAIN_TASK_CALLBACK_f CvMainCallback[SVC_CV_MAIN_TASK_FUNC_TYPES][SVC_CV_MAIN_MAX_CALLBACK_FUNC] GNU_SECTION_NOZEROINIT;

static void CvMain_SetStatus(UINT32 Status);
static UINT32 CvMain_WaitCvStatus(UINT32 Flags, UINT32 Timeout);
static void CvMain_ShowCvBootFlag(const SVC_CV_MAIN_BOOT_FLAG_s *pBootFlag);
static UINT32 CvMain_Stop(void);

static UINT32 CvMain_WaitCvStatus(UINT32 Flags, UINT32 Timeout)
{
    UINT32 RetVal;
    UINT32 ActualFlags;

    RetVal = AmbaKAL_EventFlagGet(&CvMainEventFlag, Flags, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, Timeout);
    if (KAL_ERR_NONE != RetVal) {
        RetVal = SVC_NG;
    } else {
        RetVal = SVC_OK;
    }

    return RetVal;
}

static void CvMain_SetStatus(UINT32 Status)
{
    SVC_APP_STAT_CV_BOOT_s StatusCvBoot = {0};
    UINT32 Valid = 1U, Issue = 1U;

    switch (Status) {
        case SVC_CV_MAIN_STAT_DONE:
            StatusCvBoot.Status = SVC_APP_STAT_CV_BOOT_DONE;
            break;
        case SVC_CV_MAIN_STAT_TIMEOUT:
            StatusCvBoot.Status = SVC_APP_STAT_CV_BOOT_TIME_OUT;
            break;
        case SVC_CV_MAIN_STAT_NONE:
            StatusCvBoot.Status = SVC_APP_STAT_CV_BOOT_NONE;
            break;
        case SVC_CV_MAIN_STAT_LOAD_DONE:
        case SVC_CV_MAIN_STAT_BOOT_RESUME:
            Issue = 0U;
            break;
        default:
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Unkonwn Status(%d)", Status, 0U);
            Valid = 0U;
            Issue = 0U;
            break;
    }

    if (Valid == 1U) {
        if (SVC_OK != AmbaKAL_EventFlagSet(&CvMainEventFlag, Status)) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "BootDone Event Flag set failed", 0U, 0U);
        }

        if (Issue == 1U) {
            if (SVC_OK != SvcSysStat_Issue(SVC_APP_STAT_CV_BOOT, &StatusCvBoot)) {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "BootDone Status issue failed", 0U, 0U);
            }
        }
    }
}

#if 0
static UINT32 CvMain_RunCallback(UINT32 FuncType, SVC_CV_MAIN_STATUS_s *pStatus)
{
    UINT32 RetVal = SVC_OK, i;

    (void) pStatus;

    for (i = 0; i < SVC_CV_MAIN_MAX_CALLBACK_FUNC; i++) {
        if (NULL != CvMainCallback[FuncType][i]) {
            RetVal |= CvMainCallback[FuncType][i](pStatus);
        }
    }

    return RetVal;
}
#endif

static void CvMain_ShowCvBootFlag(const SVC_CV_MAIN_BOOT_FLAG_s *pBootFlag)
{
    char LogBuf[64];

    LogBuf[0] = '\0';
    if (pBootFlag->Framework == SVC_CV_MAIN_BOOT_FLAG_FLEXIDAG) {
        AmbaUtility_StringAppend(LogBuf, 64U, "FlexiDAG ");
    } else {
        AmbaUtility_StringAppend(LogBuf, 64U, "UNKNOWN ");
    }

    if (pBootFlag->Source == SVC_CV_MAIN_BOOT_FLAG_SD) {
        AmbaUtility_StringAppend(LogBuf, 64U, "SD ");
    } else if (pBootFlag->Source == SVC_CV_MAIN_BOOT_FLAG_CMDMODE) {
        AmbaUtility_StringAppend(LogBuf, 64U, "CmdMode ");
    } else {
        AmbaUtility_StringAppend(LogBuf, 64U, "ROMFS ");
    }

    AmbaPrint_PrintStr5("[CV_MAIN_TASK]: %s", LogBuf, NULL, NULL, NULL, NULL);
}

#if defined(CONFIG_CV_EXTERNAL_MSG_THREAD)
static void* cv_process_msg_tsk(void* arg)
{
    UINT32 msg_num, i;
    UINT32 retcode;
    UINT32 message_type, message_retcode;
    AMBA_CV_FLEXIDAG_HANDLE_s *phandle;
    const volatile UINT32 condition = 1U;

    AmbaMisra_TouchUnused(arg);
    while(condition == 1U) {
        retcode = AmbaCV_SchdrWaitMsg(&msg_num);
        AmbaMisra_TouchUnused(&retcode);
        if(retcode == ERRCODE_NONE) {
            for(i = 0U; i < msg_num; i++) {
                retcode = AmbaCV_SchdrProcessMsg(&phandle, &message_type, &message_retcode);
                AmbaMisra_TouchUnused(&retcode);
#if 0
                if(retcode == ERRCODE_NONE) {
                    if(phandle == NULL) {
                        AmbaPrint_PrintUInt5("cv_process_msg_tsk() : scheduler msg type (0x%x) ret (0x%x)", message_type, message_retcode, 0U, 0U, 0U);
                    } else {
                        UINT64 addr = 0UL;

                        AmbaMisra_TypeCast(&addr, &phandle);
                        AmbaPrint_PrintUInt5("cv_process_msg_tsk() : flexidag (0x%x) msg type (0x%x) ret (0x%x)", (UINT32)addr, message_type, message_retcode, 0U, 0U);
                    }
                }
#endif
            }
        } else {
            AmbaPrint_PrintUInt5("[ERROR] cv_process_msg_tsk() : AmbaCV_SchdrWaitMsg fail ret (0x%x)", retcode, 0U, 0U, 0U, 0U);
            break;
        }
    }

    return NULL;
}
#endif

#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
static UINT32 CvMain_Boot_SD(void)
{
    UINT32               RetVal;
    char                 ScriptName[32];
    AMBA_FS_FILE_INFO_s  Fstat;
    SVC_APP_STAT_STG_s   SdStatus;
    SVC_USER_PREF_s     *pSvcUserPref;
    UINT32               WaitStatus = SVC_APP_STAT_STG_C_READY;

    SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "BootCV: Load from SD", 0U, 0U);

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK == RetVal) {
        if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL0) {
            ScriptName[0] = 'c';
            ScriptName[1] = '\0';
            WaitStatus = SVC_APP_STAT_STG_C_READY;
        } else if (pSvcUserPref->MainStgChan == AMBA_SD_CHANNEL1) {
            ScriptName[0] = 'd';
            ScriptName[1] = '\0';
            WaitStatus = SVC_APP_STAT_STG_D_READY;
        } else {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Unsupported storage", 0U, 0U);
            RetVal = SVC_NG;
        }

        if (SVC_OK == RetVal) {
            do {
                RetVal = SvcSysStat_Get(SVC_APP_STAT_STG, &SdStatus);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Get SVC_APP_STAT_STG failed", 0U, 0U);
                    break;
                }
                RetVal = AmbaKAL_TaskSleep(1);
            } while ((SdStatus.Status & WaitStatus) == 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "SvcUserPref_Get failed", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        AmbaUtility_StringAppend(ScriptName, (UINT32)sizeof(ScriptName), ":\\cvexec.ash");
        RetVal = AmbaFS_GetFileInfo(ScriptName, &Fstat);
        if (AMBA_FS_ERR_NONE == RetVal) {
            if (SVC_OK != SvcShell_ExecScript(ScriptName)) {
                //SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "SvcShell_ExecScript warning", 0U, 0U);
            }
            RetVal = SVC_OK;
        } else {
            SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "GetFileInfo failed(0x%x)", RetVal, 0U);
            RetVal = SVC_NG;
        }
    }

    if (SVC_OK != RetVal) {
        SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "BootCV: No CV script.", 0U, 0U);
    }

    return RetVal;
}
#endif

static UINT32 CvMain_Boot_Romfs(SVC_CV_MAIN_INIT_s *pInit)
{
    UINT32 RetVal;
    uint32_t SchdrState;

    RetVal = AmbaCV_SchdrState(&SchdrState);
    SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "BootCV: Load from ROMFS. SchdrState = %d", SchdrState, 0U);

    if (OK == RetVal) {
        if (SchdrState == FLEXIDAG_SCHDR_OFF) {
            #if defined(CONFIG_ICAM_TIMING_LOG)
            SvcTime(SVC_TIME_CV_VISORC_BOOT_START, "CV VisOrc Boot START");
            #endif
            RetVal = (UINT32) AmbaCV_StandAloneSchdrStart(&CvSchdrCfg);
            if (OK == RetVal) {
                RetVal = (UINT32)AmbaCV_ProtectInit((UINT32)CvSchdrCfg.cpu_map);
                if (RetVal != SVC_OK) {
                    SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_ProtectInit error(%d)", RetVal, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_StandAloneSchdrStart error(%d)", RetVal, 0U);
            }
        } else {
            uint32_t SchdrDrvState;

            RetVal = AmbaCV_SchdrDrvState(&SchdrDrvState);

            if((RetVal == OK) && (SchdrDrvState == FLEXIDAG_SCHDR_DRV_OFF)) {
                AMBA_CV_FLEXIDAG_SCHDR_CFG_s  Cfg;

                Cfg.cpu_map   = CvSchdrCfg.cpu_map;
                Cfg.log_level = CvSchdrCfg.log_level;

                RetVal = AmbaCV_FlexidagSchdrStart(&Cfg);
                if (OK == RetVal) {
                    RetVal = (UINT32)AmbaCV_ProtectInit((UINT32)CvSchdrCfg.cpu_map);
                    if (RetVal != SVC_OK) {
                        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_ProtectInit error(%d)", RetVal, 0U);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_FlexidagSchdrStart error(%d)", RetVal, 0U);
                }
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK,"AmbaCV_SchdrState error(%d)", RetVal, 0U);
        RetVal = SVC_NG;
    }

    AmbaMisra_TouchUnused(pInit);

    return RetVal;
}

static UINT32 CvMain_Boot_CmdMode(void)
{
    UINT32 RetVal;

    SvcLog_OK(SVC_LOG_CV_MAIN_TASK, "BootCV: Enter Cmd mode. Use 'svc_cv continue' to resume.", 0U, 0U);
    RetVal  = CvMain_WaitCvStatus(SVC_CV_MAIN_STAT_BOOT_RESUME, AMBA_KAL_WAIT_FOREVER);
    RetVal |= AmbaKAL_EventFlagClear(&CvMainEventFlag, SVC_CV_MAIN_STAT_BOOT_RESUME);

    return RetVal;
}

static void* CvMain_BootTaskEntry(void* EntryArg)
{
    const ULONG             *pArg;
    ULONG                   Arg;
    UINT32                  RetVal;
    SVC_CV_MAIN_INIT_s      *pInit = NULL;
    UINT32                  CVBooted = 0;
    SVC_CV_MAIN_BOOT_FLAG_s CVBootFlag;

    AmbaMisra_TouchUnused(EntryArg);

    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pInit, &Arg);

    RetVal = AmbaWrap_memcpy(&CVBootFlag, &pInit->BootFlag, sizeof(SVC_CV_MAIN_BOOT_FLAG_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaWrap_memcpy NG", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "@@ BootCV start", 0U, 0U);
    CvMain_ShowCvBootFlag(&CVBootFlag);

    if (CVBootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_CMDMODE) {
        RetVal = CvMain_Boot_CmdMode();
        if (RetVal == SVC_OK) {
            CVBooted = 1;
        }
    } else {
        if (CVBootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_ROMFS) {
            /* Wait SvcCvMainTask_Load is executed */
            RetVal = CvMain_WaitCvStatus(SVC_CV_MAIN_STAT_LOAD_DONE, 5000);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Wait PRELOAD_VISORC_DONE error(%d)", RetVal, 0U);
            }

            RetVal = CvMain_Boot_Romfs(pInit);
            if (RetVal == SVC_OK) {
                CVBooted = 1;
            }
        }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
        if (CVBooted == 0U) {
            RetVal = CvMain_Boot_SD();
            if (RetVal == SVC_OK) {
                CVBooted = 1;
            }
        }
#endif
    }

    if (CVBooted == 1U) {
        SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "@@ BootCV done", 0U, 0U);
        CvMain_SetStatus(SVC_CV_MAIN_STAT_DONE);
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_CV_VISORC_BOOT_DONE, "CV VisOrc Boot DONE");
        #endif
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "@@ BootCV failed", 0U, 0U);
        CvMain_SetStatus(SVC_CV_MAIN_STAT_NONE);
#ifndef CONFIG_QNX
        AmbaPrint_Flush();
#endif
    }

#if defined(CONFIG_LINUX)
    AmbaKAL_TaskSleep(1000);
#endif

    return NULL;
}

static UINT32 CvMain_Load(void)
{
    UINT32 RetVal;
    uint32_t SchdrState;

    RetVal = AmbaCV_SchdrState(&SchdrState);
    if (OK == RetVal) {
        if (SchdrState == FLEXIDAG_SCHDR_OFF) {
            RetVal = SvcCvFlow_AddVisOrcBin("orcvp_fdag.bin");

            if (CvMainInitFlag.DefCvTblOn == 1U) {
                if (SVC_OK != SvcCvFlow_AddCVTable("visorc_cvtable.tbar")) {
                    SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "SvcCvFlow_AddCvTable error(%d)", RetVal, 0U);
                }
            }

            if (SVC_OK == RetVal) {
                CvMain_SetStatus(SVC_CV_MAIN_STAT_LOAD_DONE);
            } else {
                SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "BootCV: Load VISION ORC failed(0x%x)", RetVal, 0U);
            }
        } else {
            CvMain_SetStatus(SVC_CV_MAIN_STAT_LOAD_DONE);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_SchdrState error(%d)", RetVal, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

static UINT32 CvMain_Stop(void)
{
    UINT32 RetVal = SVC_OK;

    if (CvMainInitFlag.SchdrReset == 1U) {
        RetVal = AmbaCV_SchdrShutdown(0);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_SchdrShutdown failed(0x%x)", RetVal, 0U);
        }
        CvMainInitFlag.SchdrReset = 0U;
    }

    return RetVal;
}

/**
 *  Init cv framework
 *  @return error code
 */
UINT32 SvcCvMainTask_Init(void)
{
    UINT32 RetVal;
    SVC_USER_PREF_s *pSvcUserPref;
    static char CvMainEventFlagName[32] = "CvMainEventFlag";

    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_INIT_START, "CV init START");
    #endif

#if defined(CONFIG_ICAM_CV_STEREO) || defined(CONFIG_ICAM_CV_FEX)
    SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_FEX | SVC_CLK_FEA_BIT_FMA);
#endif

    /* install cv commands */
    {
        AMBA_SHELL_COMMAND_s  SvcCmdCv;
        UINT32  Rval;
        extern void AmbaShell_CommandAmbaCV(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

#if defined(CONFIG_ICAM_PROJECT_CNNTESTBED)
        SvcCmdCv.pName    = "cvut";
#else
        SvcCmdCv.pName    = "cv";
#endif
        SvcCmdCv.MainFunc = AmbaShell_CommandAmbaCV;
        SvcCmdCv.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmdCv);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "## fail to install cv commands", 0U, 0U);
        }
    }

    /* Initialize */
    RetVal = AmbaWrap_memset(CvMainCallback, 0, sizeof(CvMainCallback));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "memset failed", 0U, 0U);
    }

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "SvcUserPref_Get() failed with %d", RetVal, 0U);
    } else {
        RetVal = AmbaWrap_memcpy(&CvMainInitFlag.BootFlag, &pSvcUserPref->CVBooFlag, sizeof(UINT32));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaWrap_memcpy NG", 0U, 0U);
        }
    }

#if defined (CONFIG_ICAM_CV_LOAD_DEFAULT_CV_TABLE)
    CvMainInitFlag.DefCvTblOn = 1U;
#endif

    /* Create event flag for cv status */
    RetVal = AmbaKAL_EventFlagCreate(&CvMainEventFlag, CvMainEventFlagName);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Event flag create failed with %d", RetVal, 0U);
    }
    if (SVC_OK != AmbaKAL_EventFlagClear(&CvMainEventFlag, 0xFFFFFFFFU)) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Event Flag clear failed", 0U, 0U);
    }

    /* Enable ARM SCHDR console log */
    if (pSvcUserPref->CVSchdrLog == 1U) {
        RetVal = AmbaPrint_ModuleSetAllowList(SCHDR_PRINT_MODULE_ID, (UINT8)1U);
        RetVal |= AmbaPrint_ModuleSetAllowList(CVTASK_PRINT_MODULE_ID, (UINT8)1U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Set print list failed", 0U, 0U);
        }
    }

#if defined(CONFIG_CV_EXTERNAL_MSG_THREAD)
    /* create cv msg task for scheduler */
    {
        #define CV_MSG_TASK_STACK_SIZE   0x4000
        static SVC_TASK_CTRL_s CvMsgTaskCtrl GNU_SECTION_NOZEROINIT;
        static UINT8 CvMsgTaskStack[CV_MSG_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;

        CvMsgTaskCtrl.Priority   = 40;
        CvMsgTaskCtrl.EntryFunc  = cv_process_msg_tsk;
        CvMsgTaskCtrl.pStackBase = CvMsgTaskStack;
        CvMsgTaskCtrl.StackSize  = (UINT32)sizeof(CvMsgTaskStack);
        CvMsgTaskCtrl.CpuBits    = 0x1;
        CvMsgTaskCtrl.EntryArg   = 0U;
        RetVal = SvcTask_Create("cv_proccess_msg", &CvMsgTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "cv_proccess_msg isn't created", 0U, 0U);
        }
    }
#endif

    CvMainInit = 1U;
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_CV_INIT_DONE, "CV init DONE");
    #endif

    return RetVal;
}

/**
 *  Load cv framework
 *  @return error code
 */
UINT32 SvcCvMainTask_Load(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;

    SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "SvcCvMainTask_Load", 0U, 0U);

    if (CvMainInit == 1U) {
        if (CvFlowNum > 0U) {
            if (CvMainInitFlag.BootFlag.Source == SVC_CV_MAIN_BOOT_FLAG_ROMFS) {
                RetVal = CvMain_Load();
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "CvMain_Load error(%d)", RetVal, 0U);
                }
            } else {
                /* Load by user commands in CvMain_BootTask. */
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "CvMain has not intialized", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Start cv feature
 *  @return error code
 */
UINT32 SvcCvMainTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    static UINT8 CvMainBootTaskStack[SVC_CV_MAIN_BOOT_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    const SVC_CV_MAIN_INIT_s *pInit = &CvMainInitFlag;

    SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "SvcCvMainTask_Start", 0U, 0U);

    if (CvMainInit == 1U) {
        if (CvFlowNum == 0U) {
            CvMain_SetStatus(SVC_CV_MAIN_STAT_DONE);
        } else {
            /* Initialize task to boot CV */
            CvMainBootTaskCtrl.Priority   = SVC_APP_CV_BOOT_TASK_PRI;
            CvMainBootTaskCtrl.EntryFunc  = CvMain_BootTaskEntry;
            CvMainBootTaskCtrl.pStackBase = CvMainBootTaskStack;
            CvMainBootTaskCtrl.StackSize  = SVC_CV_MAIN_BOOT_TASK_STACK_SIZE;
            CvMainBootTaskCtrl.CpuBits    = SVC_APP_CV_BOOT_TASK_CPU_BITS;
            AmbaMisra_TypeCast(&(CvMainBootTaskCtrl.EntryArg), &pInit);

            RetVal = SvcTask_Create("SvcCvMainTask", &CvMainBootTaskCtrl);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "fail to SvcTask_Create", 0U, 0U);
            }
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "CvMain has not intialized", 0U, 0U);
    }

    return RetVal;
}

/**
 *  Stop cv feature
 *  @return error code
 */
UINT32 SvcCvMainTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    uint32_t SchdrState;
    UINT32 ClearFlags = 0U;

    SvcLog_DBG(SVC_LOG_CV_MAIN_TASK, "SvcCvMainTask_Stop", 0U, 0U);

    if (CvFlowNum > 0U) {
        RetVal = CvMain_Stop();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "CvMain_Stop error(%d)", RetVal, 0U);
        }

        RetVal = SvcTask_Destroy(&CvMainBootTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "fail to SvcTask_Destroy", 0U, 0U);
        }
    }

    RetVal = AmbaCV_SchdrState(&SchdrState);
    if (OK == RetVal) {
        ClearFlags = (SchdrState == FLEXIDAG_SCHDR_OFF) ? 0xFFFFFFFFU : (0xFFFFFFFFU & (~SVC_CV_MAIN_STAT_LOAD_DONE));
        if (SVC_OK != AmbaKAL_EventFlagClear(&CvMainEventFlag, ClearFlags)) {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Clear Event Flag failed", 0U, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaCV_SchdrState error(%d)", RetVal, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  enable VPU built-in-self-test
 */
void SvcCvMainTask_EnableVpuBist(UINT32 AutoRunInterval, UINT8 DisableFex)
{
    UINT32 Rval;
    SVC_CV_MAIN_BIST_CFG_s Cfg;
    Cfg.AutoRunInterval = AutoRunInterval;
    Cfg.DisableFex = DisableFex;
    if (DisableFex == 0U) {
        SvcClock_FeatureCtrl(1U, SVC_CLK_FEA_BIT_FEX | SVC_CLK_FEA_BIT_FMA);
    }
    Rval = SvcCvMainTask_Ctrl("def_tbl_on", NULL);
    Rval |= SvcCvMainTask_Ctrl("schdr_bist_cfg", &Cfg);
    if (Rval != SVC_OK) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "SvcCvMainTask_Ctrl def_tbl_on return 0x%x", Rval, 0U);
    } else {
        SvcLog_OK(SVC_LOG_CV_MAIN_TASK, "SvcCvMainTask_Ctrl def_tbl_on with AutoRunInterval %u DisableFex %u", Cfg.AutoRunInterval, (UINT32)(Cfg.DisableFex));
    }
}

/**
 *  Commands to control booting flow
 *  @param[in] pCmd command
 *  @param[in] pParam configuration for the command
 *  @return error code
 */
UINT32 SvcCvMainTask_Ctrl(const char *pCmd, void *pParam)
{
    UINT32 RetVal = SVC_OK;
    const SVC_CV_MAIN_BIST_CFG_s *pBISTCfg;

    if (CvMainInit == 1U) {
        if (0 == SvcWrap_strcmp("continue", pCmd)) {
            CvMain_SetStatus(SVC_CV_MAIN_STAT_BOOT_RESUME);
        } else if (0 == SvcWrap_strcmp("schdr_reset", pCmd)) {
            CvMainInitFlag.SchdrReset = 1U;
        } else {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "SvcCvMainTask_Ctrl failed", 0U, 0U);
        }
    } else {
        if (0 == SvcWrap_strcmp("def_tbl_on", pCmd)) {
            CvMainInitFlag.DefCvTblOn = 1U;
        } else if (0 == SvcWrap_strcmp("schdr_bist_cfg", pCmd)) {
            if (pParam != NULL) {
                AmbaMisra_TypeCast(&pBISTCfg, &pParam);
                CvSchdrCfg.autorun_interval = (int32_t)pBISTCfg->AutoRunInterval;
                CvSchdrCfg.autorun_fex_cfg = (int32_t)pBISTCfg->DisableFex;
            } else {
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "CvMain has not intialized", 0U, 0U);
            RetVal = SVC_NG;
        }
    }

    AmbaMisra_TouchUnused(pParam);

    return RetVal;
}

/**
 *  Get bootflag of cv main task
 *  @param[in] pFlag pointer to get bootflag
 *  @return error code
 */
UINT32 SvcCvMainTask_GetBootFlag(SVC_CV_MAIN_BOOT_FLAG_s *pFlag)
{
    UINT32 RetVal = SVC_OK;

    if (CvMainInit == 1U) {
        if (pFlag != NULL) {
            RetVal = AmbaWrap_memcpy(pFlag, &CvMainInitFlag.BootFlag, sizeof(SVC_CV_MAIN_BOOT_FLAG_s));
            if (RetVal != SVC_OK) {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "AmbaWrap_memcpy fail", 0U, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "GetBootFlag pFlag = NULL", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "CvMain has not intialized", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Register callback function to execute on main task
 *  @param[in] FuncType function type
 *  @param[in] pFunc callback function
 *  @return error code
 */
UINT32 SvcCvMainTask_RegisterFunc(UINT32 FuncType, SVC_CV_MAIN_TASK_CALLBACK_f pFunc)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i = 0;

    if (1U == CvMainInit) {
        if ((FuncType < SVC_CV_MAIN_TASK_FUNC_TYPES) && (NULL != pFunc)) {
            for (i = 0; i < SVC_CV_MAIN_MAX_CALLBACK_FUNC; i++) {
                if (NULL == CvMainCallback[FuncType][i]) {
                    CvMainCallback[FuncType][i] = pFunc;
                    break;
                }
            }

            if (i >= SVC_CV_MAIN_MAX_CALLBACK_FUNC) {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "RegisterFunc error(%d)", i, 0U);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "RegisterFunc invalid param", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "not inited", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  UnRegister callback function
 *  @param[in] FuncType function type
 *  @param[in] pFunc callback function
 *  @return error code
 */
UINT32 SvcCvMainTask_UnRegisterFunc(UINT32 FuncType, SVC_CV_MAIN_TASK_CALLBACK_f pFunc)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i = 0;

    if (1U == CvMainInit) {
        if ((FuncType < SVC_CV_MAIN_TASK_FUNC_TYPES) && (NULL != pFunc)) {
            for (i = 0; i < SVC_CV_MAIN_MAX_CALLBACK_FUNC; i++) {
                if (pFunc == CvMainCallback[FuncType][i]) {
                    CvMainCallback[FuncType][i] = NULL;
                    break;
                }
            }

            if (i >= SVC_CV_MAIN_MAX_CALLBACK_FUNC) {
                SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "UnRegisterFunc error(%d)", i, 0U);
                RetVal = SVC_NG;
            }
        } else {
            SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "UnRegisterFunc invalid param", 0U, 0U);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "not inited", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/**
 *  Wait CV src is ready or idle
 *  @return error code
 */
UINT32 SvcCvMainTask_WaitSrcReady(void)
{
    UINT32 RetVal;
    UINT32 ActualFlags = 0U;

    /* Get event flag, maybe timeout */
    RetVal = AmbaKAL_EventFlagGet(&CvMainEventFlag, SVC_CV_MAIN_STAT_DONE, AMBA_KAL_FLAGS_ALL, AMBA_KAL_FLAGS_CLEAR_NONE, &ActualFlags, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_CV_MAIN_TASK, "Event flag get failed with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_CV_MAIN_TASK, "Cv src is ready", 0U, 0U);
    }

    return RetVal;
}
