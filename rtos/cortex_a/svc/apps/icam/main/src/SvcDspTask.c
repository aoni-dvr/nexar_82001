/**
 *  @file SvcDspTask.c
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
 *  @details svc dsp task
 *
 */

#include ".svc_autogen"

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaSvcWrap.h"

#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"
#include "SvcDSP.h"
#include "SvcTaskList.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"

#include "SvcTask.h"
#include "SvcUcode.h"
#include "SvcSysStat.h"
#include "SvcUserPref.h"
#include "SvcCmdDSP.h"
#include "SvcResCfg.h"
#if defined(CONFIG_BUILD_CV)
#include "SvcCvFlow.h"
#endif
#include "SvcVinSrc.h"
#include "SvcSafeStateMgr.h"

#include "SvcAppStat.h"
#include "SvcDspTask.h"
#include "SvcTiming.h"
#include "SvcRecTask.h"

#define SVC_LOG_DSP_TASK "DSP_TASK"

#define DSP_BOOT_TASK_STACK_SIZE        (0x10000U)

#define DSP_BOOT_TASK_SRC_UCODE_LIV     (0x01U)
#define DSP_BOOT_TASK_SRC_UCODE_ALL     (0x02U)
#define DSP_BOOT_TASK_SRC_VIN           (0x04U)
#define DSP_BOOT_TASK_SRC_VOUT          (0x08U)
#define DSP_BOOT_TASK_SRC_CAMCTRL       (0x10U)
#define DSP_BOOT_TASK_SRC_BOOT_DONE     (0x20U)

static void* DspBootTask_Entry(void* EntryArg);
static void DspBootTask_WaitSrcDone(void);
static void DspBootTask_WaitDspDone(void);

static AMBA_KAL_EVENT_FLAG_t DspBootTaskSrcEvent;

/**
 * Init dsp task needed resource
 * return 0-OK, 1-NG
 */
UINT32 SvcDspTask_Init(void)
{
    static char DspBootTaskSrcEventName[32] = "DspBootTaskSrcEvent";
    UINT32 RetVal;
    UINT32 CtrlID = 0U;

    SvcLog_DBG(SVC_LOG_DSP_TASK, "@@ Init Begin", 0U, 0U);
    RetVal = AmbaKAL_EventFlagCreate(&DspBootTaskSrcEvent, DspBootTaskSrcEventName);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DSP_TASK, "Create event flag failed %d", RetVal, 0U);
    }

    RetVal |= SvcSysStat_Register(SVC_APP_STAT_VIN,     SvcDspTask_StatusCB, &CtrlID);
    RetVal |= SvcSysStat_Register(SVC_APP_STAT_VOUT,    SvcDspTask_StatusCB, &CtrlID);
    RetVal |= SvcSysStat_Register(SVC_APP_STAT_CAMCTRL, SvcDspTask_StatusCB, &CtrlID);
    RetVal |= SvcSysStat_Register(SVC_APP_STAT_DSP_BOOT, SvcDspTask_StatusCB, &CtrlID);

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DSP_TASK, "@@ Svc Status Registered failed", 0U, 0U);
    }

    SvcLog_DBG(SVC_LOG_DSP_TASK, "@@ Init Done", 0U, 0U);
    return RetVal;
}

/**
 * Start the dsp task. The task mainly boot up the DSP
 * @param [in] 0 for AMBA_DSP_SYS_STATE_LIVEVIEW, 1 for AMBA_DSP_SYS_STATE_PLAYBACK
 * return 0-OK, 1-NG
 */
UINT32 SvcDspTask_Start(UINT8 SysState)
{
    static SVC_TASK_CTRL_s DspBootTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8 DspBootTaskStack[DSP_BOOT_TASK_STACK_SIZE] GNU_SECTION_NOZEROINIT;
    static UINT32 TaskCreated = 0U;

    UINT32 RetVal;

    if (TaskCreated > 0U) {
        RetVal = SvcTask_Destroy(&DspBootTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspBootTask destroy failed", 0U, 0U);
        } else {
            TaskCreated = 0U;
        }
    }

    if (TaskCreated == 0U) {
        /* trigger task to Boot dsp */
        DspBootTaskCtrl.Priority   = SVC_DSP_BOOT_TASK_PRI;
        DspBootTaskCtrl.EntryFunc  = DspBootTask_Entry;
        DspBootTaskCtrl.pStackBase = DspBootTaskStack;
        DspBootTaskCtrl.StackSize  = DSP_BOOT_TASK_STACK_SIZE;
        DspBootTaskCtrl.CpuBits    = SVC_DSP_BOOT_TASK_CPU_BITS;
        DspBootTaskCtrl.EntryArg   = (UINT32) SysState;

        RetVal = SvcTask_Create("SvcDspBootTask", &DspBootTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspBootTask create failed", 0U, 0U);
        } else {
            TaskCreated = 1U;
            DspBootTask_WaitDspDone();
        }
    }

    return RetVal;
}

/**
* status callback function
* @param [in] StatIdx index of status
* @param [in] pInfo information
* @return none
*/
void SvcDspTask_StatusCB(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    SVC_APP_STAT_UCODE_s   *pUcodeStatus = NULL;
    SVC_APP_STAT_VIN_s     *pVinStatus = NULL;
    SVC_APP_STAT_VOUT_s    *pVoutStatus = NULL;
    SVC_APP_STAT_CAMCTRL_s *pCamCtrlStatus = NULL;
    SVC_APP_STAT_DSP_BOOT_s *pDspBootStatus = NULL;

    AmbaMisra_TouchUnused(pUcodeStatus);
    AmbaMisra_TouchUnused(pVinStatus);
    AmbaMisra_TouchUnused(pVoutStatus);
    AmbaMisra_TouchUnused(pCamCtrlStatus);
    AmbaMisra_TouchUnused(pDspBootStatus);
    AmbaMisra_TouchUnused(pInfo);

    switch (StatIdx) {
        case SVC_APP_STAT_UCODE:
            AmbaMisra_TypeCast(&pUcodeStatus, &pInfo);
            if (pUcodeStatus->Status == SVC_APP_STAT_UCODE_LIV_DONE) {
                RetVal = AmbaKAL_EventFlagSet(&DspBootTaskSrcEvent, DSP_BOOT_TASK_SRC_UCODE_LIV);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspTask_StatusCB() err, AmbaKAL_EventFlagSet DSP_BOOT_TASK_SRC_UCODE_LIV failed with 0x%x", RetVal, 0U);
                }
            }

            if (pUcodeStatus->Status == SVC_APP_STAT_UCODE_ALL_DONE) {
                RetVal = AmbaKAL_EventFlagSet(&DspBootTaskSrcEvent, DSP_BOOT_TASK_SRC_UCODE_ALL);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspTask_StatusCB() err, AmbaKAL_EventFlagSet DSP_BOOT_TASK_SRC_UCODE_ALL failed with 0x%x", RetVal, 0U);
                }
            }
        break;
        case SVC_APP_STAT_VIN:
            AmbaMisra_TypeCast(&pVinStatus, &pInfo);
            if (pVinStatus->Status == SVC_APP_STAT_VIN_READY) {
                RetVal = AmbaKAL_EventFlagSet(&DspBootTaskSrcEvent, DSP_BOOT_TASK_SRC_VIN);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspTask_StatusCB() err, AmbaKAL_EventFlagSet DSP_BOOT_TASK_SRC_VIN failed with 0x%x", RetVal, 0U);
                }
            }
        break;
        case SVC_APP_STAT_VOUT:
            AmbaMisra_TypeCast(&pVoutStatus, &pInfo);
            if (pVoutStatus->Status == SVC_APP_STAT_VOUT_READY) {
                RetVal = AmbaKAL_EventFlagSet(&DspBootTaskSrcEvent, DSP_BOOT_TASK_SRC_VOUT);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspTask_StatusCB() err, AmbaKAL_EventFlagSet DSP_BOOT_TASK_SRC_VOUT failed with 0x%x", RetVal, 0U);
                }
            }
        break;
        case SVC_APP_STAT_CAMCTRL:
            AmbaMisra_TypeCast(&pCamCtrlStatus, &pInfo);
            if ((pCamCtrlStatus->Status == SVC_APP_STAT_CAMCTRL_NONE) || (pCamCtrlStatus->Status == SVC_APP_STAT_CAMCTRL_READY)) {
                RetVal = AmbaKAL_EventFlagSet(&DspBootTaskSrcEvent, DSP_BOOT_TASK_SRC_CAMCTRL);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspTask_StatusCB() err, AmbaKAL_EventFlagSet DSP_BOOT_TASK_SRC_CAMCTRL failed with 0x%x", RetVal, 0U);
                }
            }
        break;
        case SVC_APP_STAT_DSP_BOOT:
            AmbaMisra_TypeCast(&pDspBootStatus, &pInfo);
            if (pDspBootStatus->Status == SVC_APP_STAT_DSP_BOOT_DONE) {
                RetVal = AmbaKAL_EventFlagSet(&DspBootTaskSrcEvent, DSP_BOOT_TASK_SRC_BOOT_DONE);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDspTask_StatusCB() err, AmbaKAL_EventFlagSet DSP_BOOT_TASK_SRC_BOOT_DONE failed with 0x%x", RetVal, 0U);
                }
            }
        break;
        default:
            SvcLog_NG(SVC_LOG_DSP_TASK, "Unkown StatIdx (%d)", StatIdx, 0U);
        break;
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void* DspBootTask_Entry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK, Size;
    ULONG  Base;
    AMBA_DSP_SYS_CONFIG_s   DspCfg = {0};
    AMBA_DSP_VERSION_INFO_s VerInfo = {0};
    SVC_USER_PREF_s  *pSvcUserPref;
    SVC_APP_STAT_DSP_BOOT_s DspStatus;
    SVC_APP_STAT_UCODE_s UcodeStatus;
    UINT8 SysState;
    const ULONG *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&UcodeStatus);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    SysState = (UINT8)(*pArg);

    DspBootTask_WaitSrcDone();

    {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
        UINT32 DefBinFileSize = 0U;

        SvcUcode_GetInfo(UCODE_FILE_DEFBIN, NULL, NULL, &DefBinFileSize);
        DspCfg.DefaultBinarySize = DefBinFileSize;
        SvcLog_DBG(SVC_LOG_DSP_TASK, "config default binray size 0x%x", DefBinFileSize, 0U);
#endif

        if (SVC_OK == SvcUserPref_Get(&pSvcUserPref)) {
            SvcCmdDSP_CmdLogCtrl(pSvcUserPref->ShowDspLog);
        } else {
            SvcLog_NG(SVC_LOG_DSP_TASK, "SvcUserPref_Get failed", 0U, 0U);
        }

        if (SVC_OK != AmbaWrap_memset(&DspStatus, 0, sizeof(DspStatus))) {
            SvcLog_NG(SVC_LOG_DSP_TASK, "DspBootTask_Entry() err, AmbaWrap_memset failed, DspStatus SVC_APP_STAT_DSP_BOOT_START", 0U, 0U);
        }
        DspStatus.Status = SVC_APP_STAT_DSP_BOOT_START;
        if (SVC_OK != SvcSysStat_Issue(SVC_APP_STAT_DSP_BOOT, &DspStatus)) {
            SvcLog_NG(SVC_LOG_DSP_TASK, "Svc Sys Status issue failed.", 0U, 0U);
        }

        SvcLog_DBG(SVC_LOG_DSP_TASK, "@@ Start", 0U, 0U);
        #if defined(CONFIG_ICAM_TIMING_LOG)
        SvcTime(SVC_TIME_DSP_BOOT_START, "DSP boot START");
        #endif
        SvcLog_OK(SVC_LOG_DSP_TASK, "ucodes for dsp are loaded", 0U, 0U);

        /* boot dsp */
        RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_DSP_LOG, &Base, &Size);
        if (SVC_OK == RetVal) {
            DspCfg.DebugLogDataAreaAddr = Base;
            DspCfg.DebugLogDataAreaSize = Size;

            RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_DSP_WRK, &Base, &Size);
            if (SVC_OK == RetVal) {
                DspCfg.WorkAreaAddr = Base;
                DspCfg.WorkAreaSize = Size;
                if (SysState < AMBA_DSP_SYS_STATE_NUM) {
#if defined(CONFIG_BUILD_CV) && (CONFIG_ICAM_DSP_VP_MSG_SIZE != 0)
#if 0               /* Not necessary for FlexiDAG */
                    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_VP_MSG, &Base, &Size);
                    if (SVC_OK == RetVal) {
                        SvcDSP_PackVecProcCfg(&DspCfg, Base, Size);
                    } else {
                        SvcLog_NG(SVC_LOG_DSP_TASK, "## fail to get vp msg buffer", 0U, 0U);
                    }
#endif
#endif
                    DspCfg.SysState = SysState;
                    RetVal = SvcDSP_Boot(&DspCfg, SVC_DSP_MSG_TASK_PRI, SVC_DSP_MSG_TASK_CPU_BITS);
                    if (RetVal != OK) {
                        SvcLog_NG(SVC_LOG_DSP_TASK, "SvcDSP_Boot failed %u", RetVal, 0U);
                    }

#if defined(CONFIG_FWPROG_R52SYS_ENABLE)
                    RetVal = SvcSafeStateMgr_LiveviewStart();
                    if (RetVal != OK) {
                        SvcLog_NG(SVC_LOG_DSP_TASK, "SvcSafeStateMgr_LiveviewStart return 0x%x", RetVal, 0U);
                    }
#endif
                } else {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "## unknown SysState", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_DSP_TASK, "## fail to get dsp work buffer", 0U, 0U);
            }

            if (SVC_OK == RetVal) {
                if (SVC_OK == AmbaDSP_MainGetDspVerInfo(&VerInfo)) {
                    SvcLog_OK(SVC_LOG_DSP_TASK, "==========uCode Info==========", 0U, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "version (dec): %u",   VerInfo.UCodeVer, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "version (hex): 0x%X", VerInfo.UCodeVer, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "year: %d",            VerInfo.Year, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "date: %d/%d",         VerInfo.Month, VerInfo.Day);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "api (dec): %u",       VerInfo.ApiVer, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "api (hex): 0x%X",     VerInfo.ApiVer, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "silicon: %u",         VerInfo.SiliconVer, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "init_data: 0x%X",     VerInfo.LinkBase, 0U);
                    SvcLog_OK(SVC_LOG_DSP_TASK, "=============================", 0U, 0U);
                }
            }

            if (1) {//(SVC_OK == RetVal) {
                if (SVC_OK != AmbaWrap_memset(&DspStatus, 0, sizeof(DspStatus))) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "DspBootTask_Entry() err, AmbaWrap_memset failed, DspStatus SVC_APP_STAT_DSP_BOOT_DONE", 0U, 0U);
                }
                DspStatus.Status = SVC_APP_STAT_DSP_BOOT_DONE;
                if (SVC_OK != SvcSysStat_Issue(SVC_APP_STAT_DSP_BOOT, &DspStatus)) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "Svc Sys Status issue failed", 0U, 0U);
                }
            }

#ifdef CONFIG_ICAM_UCODE_PARTIAL_LOAD
            /* we should unlock ucode encode and decode region after dsp bootup */
            if (SVC_OK == RetVal) {
                UINT32 ActualFlag;
                RetVal = AmbaKAL_EventFlagGet(&DspBootTaskSrcEvent,
                                              DSP_BOOT_TASK_SRC_UCODE_ALL,
                                              AMBA_KAL_FLAGS_ALL,
                                              AMBA_KAL_FLAGS_CLEAR_NONE,
                                              &ActualFlag,
                                              AMBA_KAL_WAIT_FOREVER);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "Stage 1 Wait DSP_BOOT_TASK_SRC_UCODE_ALL failed with %d", RetVal, 0U);
                }
                SvcUcode_LoadStage1Done();
                UcodeStatus.Status = SVC_APP_STAT_UCODE_LOAD_STAGE1_DONE;
                if (SVC_OK != SvcSysStat_Issue(SVC_APP_STAT_UCODE, &UcodeStatus)) {
                    SvcLog_NG(SVC_LOG_DSP_TASK, "Svc Sys Status issue failed", 0U, 0U);
                }
            }
#endif
        } else {
            SvcLog_NG(SVC_LOG_DSP_TASK, "## fail to get dsp log buffer", 0U, 0U);
        }
    }

    AmbaMisra_TouchUnused(&RetVal);

    return NULL;
}

static void DspBootTask_WaitSrcDone(void)
{
    UINT32            RetVal, WaitFlag = DSP_BOOT_TASK_SRC_UCODE_LIV;
    UINT32            ActualFlag = 0U, i, Src, FovNum, FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM];
    SVC_USER_PREF_s   *pSvcUserPref;

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DSP_TASK, "fail to get preference", 0U, 0U);
        WaitFlag = DSP_BOOT_TASK_SRC_UCODE_ALL;
    } else {
        if (pSvcUserPref->OperationMode == 1U) {        /* liveview */
            FovNum = 0U;
            AmbaSvcWrap_MisraMemset(FovIdxs, 0, sizeof(FovIdxs));
            if (SvcResCfg_GetFovIdxs(FovIdxs, &FovNum) == SVC_OK) {
                for (i = 0; i < FovNum; i++) {
                    Src = 255U;
                    if (SvcResCfg_GetFovSrc(FovIdxs[i], &Src) == SVC_OK) {
                        if (Src == SVC_VIN_SRC_MEM_DEC) {
                            /* for duplex mode, we must wait all ucode loaded */
                            WaitFlag = DSP_BOOT_TASK_SRC_UCODE_ALL;
                            break;
                        }
                    }
                }
            }
        } else if (pSvcUserPref->OperationMode == 2U) { /* playback */
            WaitFlag = DSP_BOOT_TASK_SRC_UCODE_ALL;
        } else {
            /* nothing */
        }
    }

    if (WaitFlag == DSP_BOOT_TASK_SRC_UCODE_ALL) {
        SVC_APP_STAT_DSP_BOOT_s DspStatus = { .Status = SVC_APP_STAT_DSP_WAIT_UCODE_DONE };
        if (SVC_OK != SvcSysStat_Issue(SVC_APP_STAT_DSP_BOOT, &DspStatus)) {
            SvcLog_NG(SVC_LOG_DSP_TASK, "Svc Sys Status issue failed", 0U, 0U);
        }
    }

    RetVal |= AmbaKAL_EventFlagGet(&DspBootTaskSrcEvent,
                                   WaitFlag,
                                   AMBA_KAL_FLAGS_ANY,
                                   AMBA_KAL_FLAGS_CLEAR_NONE,
                                   &ActualFlag,
                                   AMBA_KAL_WAIT_FOREVER);
    SvcLog_DBG(SVC_LOG_DSP_TASK, "SRC_UCODE Check", 0U, 0U);

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DSP_TASK, "Err when waiting some necessary source", 0U, 0U);
    }
}

static void DspBootTask_WaitDspDone(void)
{
    UINT32 RetVal;
    UINT32 ActualFlag = 0U;

    RetVal = AmbaKAL_EventFlagGet(&DspBootTaskSrcEvent,
                                  DSP_BOOT_TASK_SRC_BOOT_DONE,
                                  AMBA_KAL_FLAGS_ALL,
                                  AMBA_KAL_FLAGS_CLEAR_NONE,
                                  &ActualFlag,
                                  AMBA_KAL_WAIT_FOREVER);
    SvcLog_DBG(SVC_LOG_DSP_TASK, "SRC_BOOT_DONE Check", 0U, 0U);
    #if defined(CONFIG_ICAM_TIMING_LOG)
    SvcTime(SVC_TIME_DSP_BOOT_DONE, "DSP boot DONE");
    SvcTime_PrintBootTime();
    #endif

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DSP_TASK, "Err when waiting boot done flag", 0U, 0U);
    }
}
