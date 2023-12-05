/**
 *  @file SvcRecTask.c
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
 *  @details svc record task file
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaGDMA.h"

/* ssp */
#include "AmbaDSP.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"

/* framework */
#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcWrap.h"
#include "SvcUtil.h"
#include "SvcTask.h"
#include "SvcMem.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "SvcCmd.h"

/* app-shared */
#include "SvcSysStat.h"
#include "SvcBuffer.h"
#include "SvcRecMain.h"
#include "AmbaAudioBSBuf.h"
#include "AmbaAEncFlow.h"
#include "AmbaADecFlow.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcGuiCheckBox.h"
#include "SvcGuiTextView.h"
#include "SvcButtonCtrl.h"
#include "SvcVinSrc.h"
#if defined(CONFIG_ICAM_PLAYBACK_USED)
#include "SvcPbkPictDisp.h"
#endif
#include "SvcTiming.h"

#include "AmbaVIN_Priv.h"
#include "SvcImg.h"

/* app-icam */
#include "SvcAppStat.h"
#include "SvcRecInfoPack.h"
#include "SvcBufMap.h"

#include "SvcRecTask.h"
#include "SvcCmdRec.h"

#include "SvcUserPref.h"
#include "SvcStill.h"
#include "SvcStillCap.h"
#include "SvcStillTask.h"
#include "SvcTaskList.h"

#ifdef CONFIG_ICAM_YUVFILE_FEED
#include "SvcLvFeedFileYuvTask.h"
#endif
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
#include <app_base.h>
#endif

#define SVC_LOG_REC_TASK  "REC_TASK"

#define RECORD_GUI_LEVEL_H      (18U)
#define RECORD_GUI_LEVEL_M      (19U)
#define RECORD_GUI_LEVEL_S      (20U)

#define FONT_SIZE               (5U)
#if defined(CONFIG_ICAM_32BITS_OSD_USED)
#define FONT_COLOR              (0xffffffffU)
#define BACKGRD_COLOR           (0x80202040U)
#else
#define FONT_COLOR              (255U)
#define BACKGRD_COLOR           (73U)
#endif

#define FONT_WIDTH              (FONT_SIZE * (SVC_OSD_CHAR_WIDTH + SVC_OSD_CHAR_GAP))
#define FONT_HEIGHT             (FONT_SIZE * SVC_OSD_CHAR_HEIGHT)

#define REC_TASK_STRM_GUI_CHECKBOX_W (260U)
#define REC_TASK_STRM_GUI_CHECKBOX_H (60U)
#define REC_TASK_STRM_GUI_TEXTBOX1_H (25U)
#define REC_TASK_STRM_GUI_TEXTBOX2_H (25U)
#define REC_TASK_STRM_GUI_W          (REC_TASK_STRM_GUI_CHECKBOX_W)
#define REC_TASK_STRM_GUI_H          (REC_TASK_STRM_GUI_CHECKBOX_H + REC_TASK_STRM_GUI_TEXTBOX1_H + REC_TASK_STRM_GUI_TEXTBOX2_H)

#define REC_TASK_OSD_STRM_SHOW       (1U)
#define REC_TASK_OSD_STRM_UPDATE     (2U)

typedef struct {
    UINT32 Seconds;
    UINT32 Minutes;
    UINT32 Hours;
    UINT32 RecTaskExpiredTimes;
    UINT32 Update;
} ICAM_TIME_INFO_s;

static UINT32 RecTaskStreamBits = 0U;
static UINT32 RecTaskStartBits  = 0U;
static UINT32 RecMVStreamBits  = 0U;

static AMBA_KAL_TIMER_t RecTaskGuiTimer GNU_SECTION_NOZEROINIT;
static ICAM_TIME_INFO_s Time GNU_SECTION_NOZEROINIT;

#ifdef CONFIG_FPD_MAXIM_ZS095BH3001A8H3_BII
static UINT32 VoutOfRecGui = VOUT_IDX_A;
static UINT32 RecTaskGuiStartX = (1920U - (10U * FONT_WIDTH)) / 2U;
static UINT32 RecTaskGuiStartY = 0U;
#else
static UINT32 VoutOfRecGui = VOUT_IDX_B;
static UINT32 RecTaskGuiStartX = (960U - (10U * FONT_WIDTH)) / 2U;
static UINT32 RecTaskGuiStartY = 0U;
#endif

#if defined(CONFIG_ICAM_STLCAP_USED)
static UINT8 RecTaskVidThm = 0U;
#endif

/* Parameter for button control */
static SVC_BUTTON_CTRL_ID RecTaskButtonCtrl_ID = 0U;

/* Parameter for controlling OSD showing target strm */
static UINT32 RecTaskOsdShowTargetStrm = 0U;
static UINT32 RecTaskOsdShowTargetStrmOnVout = 0U;
static SVC_GUI_CANVAS_s RecTaskTargetStrmCanvas;

#ifndef CONFIG_ICAM_32BITS_OSD_USED
static SVC_GUI_CHECKBOX_s RecTaskTargetStrmCheckBox = {
    .CheckBarColor    = 23U,
    .SelectedBoxColor = 232U,
    .BackgroundColor  = 73U,
    .FontColor        = 255U,
    .FontSize         = 2U,
    .RowGap           = 1U,
    .RowNum           = 3U,
    .ListNum          = 0U,
    .pList            = { NULL },
    .CurIdx           = 0U,
    .CheckBits        = 0U,
};

#else
static SVC_GUI_CHECKBOX_s RecTaskTargetStrmCheckBox = {
    .CheckBarColor    = 0x80e04000U,
    .SelectedBoxColor = 0x8000a0c0U,
    .BackgroundColor  = 0x80202040U,
    .FontColor        = 0xffffffffU,
    .FontSize         = 2U,
    .RowGap           = 1U,
    .RowNum           = 3U,
    .ListNum          = 0U,
    .pList            = { NULL },
    .CurIdx           = 0U,
    .CheckBits        = 0U,
};
#endif

static AMBA_KAL_EVENT_FLAG_t RecTaskEvent;
#define REC_TASK_UCODE_ALL          (0x01U)
#define REC_TASK_DSP_BOOT_DONE      (0x02U)
#define REC_TASK_UCODE_STAGE1_DONE  (0x04U)
#define REC_TASK_STG_C_DONE         (0x08U)
#define REC_TASK_STG_D_DONE         (0x10U)
#define REC_TASK_STG_I_DONE         (0x20U)


static void RecTask_GuiUpdate(UINT32 EntryArg);
static void RecTask_GuiDraw_Hours(UINT32 VoutIdx, UINT32 Level);
static void RecTask_GuiDraw_Mins(UINT32 VoutIdx, UINT32 Level);
static void RecTask_GuiDraw_Secs(UINT32 VoutIdx, UINT32 Level);
static void RecTask_GuiDraw_Update(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static void RecTask_OsdShowStrmEntry(UINT32 VoutIdx, UINT32 Level);
static void RecTask_OsdShowStrmUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);


/* Button Ctrl callback */
static UINT32 RecTask_Minus(void);
static UINT32 RecTask_Plus(void);
static UINT32 RecTask_Move(void);

static void RecTask_CmdInstall(void);
static void RecTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void RecTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

/* Callback of menu status */
static void RecTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);

static void RecMonNotify(UINT32 NotifyCode, UINT32 StrmBits)
{

    AmbaMisra_TouchUnused(&StrmBits);

    if (NotifyCode == SVC_REC_MON_NCODE_QUEUE_EXCEP) {
        SvcLog_DBG(SVC_LOG_REC_TASK, "\n", 0U, 0U);
        SvcLog_DBG(SVC_LOG_REC_TASK, "#### storage or source queues are abnormal ...", 0U, 0U);
        SvcLog_DBG(SVC_LOG_REC_TASK, "####   stop record automatically ... 0x%X", StrmBits, 0U);
        SvcLog_DBG(SVC_LOG_REC_TASK, "\n", 0U, 0U);

        SvcRecMain_Stop(StrmBits, 1U);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
        if (dqa_test_script.product_line_mode == 0) {
            Pmic_SoftReset();
        }
#endif
    }
}

/**
* Init record module. Prepare the resource needed to start record.
* @return ErrorCode
*/
UINT32 SvcRecTask_Init(void)
{
    UINT32 RetVal;

    /* Callback for ButtonControl */
    static SVC_BUTTON_CTRL_LIST_s RecTaskButtonCtrl[3] = {
        { "button_1", RecTask_Minus },
        { "button_2", RecTask_Plus  },
        { "button_3", RecTask_Move  },
    };

    {
        /* Create time */
        static char RecTaskGuiTimerName[16] = "RecTaskGuiTimer";

        /* Timer is used to showing and update GUI */
        RetVal = AmbaKAL_TimerCreate(&RecTaskGuiTimer,
                                     RecTaskGuiTimerName,
                                     RecTask_GuiUpdate,
                                     0U,
                                     1000U,
                                     1000U,
                                     AMBA_KAL_DONT_START);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Init() failed. Timer create failed with %d", RetVal, 0U);
        }
    }

    SvcRecMain_Init();
    /* install rec command */
    SvcCmdRec_Install();

    if (SVC_OK == RetVal) {
        SVC_SYS_STAT_ID StatusID;
        RetVal = SvcSysStat_Register(SVC_APP_STAT_MENU, RecTask_MenuStatusCallback, &StatusID);
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcButtonCtrl_Register(SVC_BUTTON_TYPE_GPIO, RecTaskButtonCtrl, 3U, &RecTaskButtonCtrl_ID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Init() failed. Button Register failed", 0U, 0U);
        }
    }

    RecTask_CmdInstall();

    return RetVal;
}


static UINT32 WaitRecResource(void)
{
    UINT32    Err, RetVal = SVC_OK;
    UINT32    WaitFlag, ActualFlag = 0U;

    AmbaMisra_TouchUnused(&RetVal);

    /* wait dsp boot done */
    if (RetVal == SVC_OK) {
        #ifdef CONFIG_ICAM_UCODE_PARTIAL_LOAD
        WaitFlag = REC_TASK_UCODE_STAGE1_DONE;
        #else
        WaitFlag = REC_TASK_DSP_BOOT_DONE | REC_TASK_UCODE_ALL;
        #endif

        SvcLog_OK(SVC_LOG_REC_TASK, "start to wait for DSP and Ucode", 0U, 0U);
        Err = AmbaKAL_EventFlagGet(&RecTaskEvent,
                                    WaitFlag,
                                    AMBA_KAL_FLAGS_ALL,
                                    AMBA_KAL_FLAGS_CLEAR_NONE,
                                    &ActualFlag,
                                    AMBA_KAL_WAIT_FOREVER);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_REC_TASK, "Wait REC_TASK_UCODE_ALL and REC_TASK_DSP_BOOT_DONE (%u) failed with %d", WaitFlag, Err);
            RetVal = SVC_NG;
        }

        if (RetVal == SVC_OK) {
            SvcLog_OK(SVC_LOG_REC_TASK, "DSP and Ucode done", 0U, 0U);
        }
    }

#ifdef CONFIG_BUILD_IMGFRW_AAA
    /* wait image framework done */
    if (RetVal == SVC_OK) {
        UINT32                i, j, VinId = 0U, SensorIdx = 0U, FovId;
        UINT32                RecFov[AMBA_DSP_MAX_VIEWZONE_NUM] = {0};
        const SVC_RES_CFG_s   *pCfg = SvcResCfg_Get();

        SvcLog_OK(SVC_LOG_REC_TASK, "start to wait for image framework", 0U, 0U);

        for (i = 0U; i < pCfg->RecNum; i++) {
            if (pCfg->RecStrm[i].RecSetting.BootToRec > 0U) {
                for (j = 0U; j < pCfg->RecStrm[i].StrmCfg.NumChan; j++) {
                    FovId = pCfg->RecStrm[i].StrmCfg.ChanCfg[j].FovId;
                    if (FovId < (UINT32)AMBA_DSP_MAX_VIEWZONE_NUM) {
                        RecFov[FovId] = 1U;
                    }
                }
            }
        }

        for (i = 0U; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
            if (RecFov[i] > 0U) {
                Err = SvcResCfg_GetSensorIdxOfFovIdx(i, &VinId, &SensorIdx);
                if (Err != SVC_OK) {
                    SvcLog_NG(SVC_LOG_REC_TASK, "SvcResCfg_GetSensorIdxOfFovIdx failed %d", Err, 0U);
                    RetVal = SVC_NG;
                }

                if (RetVal == SVC_OK) {
                    Err = SvcImg_AeStableWait(VinId, SensorIdx, (UINT32)AMBA_KAL_WAIT_FOREVER);
                    if (Err != SVC_OK) {
                        SvcLog_NG(SVC_LOG_REC_TASK, "SvcImg_AeStableWait failed %d", Err, 0U);
                        RetVal = SVC_NG;
                    }
                }
            }
        }

        if (RetVal == SVC_OK) {
            SvcLog_OK(SVC_LOG_REC_TASK, "image framework done", 0U, 0U);
        }
    }
#endif

    /* wait storage and dcf ready */
    /* if user want to start recorders ASAP, please set BootToRecTimeOut to 0 in preference */
    if (RetVal == SVC_OK) {
        UINT32                i, NullWrite = 0U, TimeOut = 0U;
        SVC_REC_MAIN_INFO_s   RecMainInfo;
        SVC_USER_PREF_s       *pSvcUserPref;
        const SVC_RES_CFG_s   *pCfg = SvcResCfg_Get();

        AmbaMisra_TouchUnused(&NullWrite);

        WaitFlag = 0U;

        Err = AmbaWrap_memset(&RecMainInfo, 0, sizeof(RecMainInfo));
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_REC_TASK, "AmbaWrap_memset with err %u", Err, 0U);
        }

        Err = SvcUserPref_Get(&pSvcUserPref);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_REC_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
        } else {
            TimeOut = pSvcUserPref->BootToRecTimeOut;
        }

        SvcRecMain_Control(SVC_RCM_PMT_NLWR_GET, 0, NULL, &NullWrite);
        SvcRecMain_InfoGet(&RecMainInfo);

        if (NullWrite == 0U) {
            for (i = 0U; i < pCfg->RecNum; i++) {
                if (pCfg->RecStrm[i].RecSetting.BootToRec > 0U) {
                    switch ((UINT8)RecMainInfo.pFwkStrmCfg[i].Drive) {
                    case (UINT8)('c'):
                    case (UINT8)('C'):
                        WaitFlag |= REC_TASK_STG_C_DONE;
                        break;
                    case (UINT8)('d'):
                    case (UINT8)('D'):
                        WaitFlag |= REC_TASK_STG_D_DONE;
                        break;
                    case (UINT8)('i'):
                    case (UINT8)('I'):
                        WaitFlag |= REC_TASK_STG_I_DONE;
                        break;
                    default:
                        /* nothing */
                        break;
                    }
                }
            }
        }

        if ((WaitFlag > 0U) && (TimeOut > 0U)) {
            SvcLog_OK(SVC_LOG_REC_TASK, "start to wait for drive initialization %u", WaitFlag, 0U);
            Err = AmbaKAL_EventFlagGet(&RecTaskEvent,
                                        WaitFlag,
                                        AMBA_KAL_FLAGS_ALL,
                                        AMBA_KAL_FLAGS_CLEAR_NONE,
                                        &ActualFlag,
                                        TimeOut);
            if (Err != SVC_OK) {
                SvcLog_DBG(SVC_LOG_REC_TASK, "cannot wait for drive done event %u, start recorders anyway", WaitFlag, 0U);
            } else {
                SvcLog_OK(SVC_LOG_REC_TASK, "drive initialization %u done", WaitFlag, 0U);
            }
        }
    }

    return RetVal;
}

static void* SvcRecTask_AutoRecordEntry(void* EntryArg)
{
    UINT32    RetVal = SVC_OK, Err;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TouchUnused(&RetVal);

    /* wait resource done */
    if (RetVal == SVC_OK) {
        Err = WaitRecResource();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_REC_TASK, "WaitRecResource failed with %u", Err, 0U);
            RetVal = SVC_NG;
        }
    }

    if (RetVal == SVC_OK) {
        UINT32                 i, Bits = 1U;
        const SVC_RES_CFG_s    *pCfg = SvcResCfg_Get();

        /* start MV record streams if they are enabled */
        SvcRecTask_StartMV();


        /* start recorder */
        for (i = 0U; i < pCfg->RecNum; i++) {
            if (pCfg->RecStrm[i].RecSetting.BootToRec > 0U) {
                SvcRecMain_Start((Bits << i), 1U);
            }
        }

    }

    #if defined(CONFIG_ICAM_TIMING_LOG)
    if (RetVal == SVC_OK) {
        SvcTime(SVC_TIME_REC_START, NULL);
        SvcTime_PrintBootTime();
    }
    #endif

    return NULL;
}

UINT32 SvcRecTask_AutoStart(void)
{
#define RECSTACK 0x3000U
    static char            RecTaskEventName[32] = "RecTaskEvent";
    static SVC_TASK_CTRL_s RecTaskCtrl GNU_SECTION_NOZEROINIT;
    static UINT8           RecTaskStack[RECSTACK] GNU_SECTION_NOZEROINIT;
    UINT32                 RetVal, CtrlID;

    RetVal = AmbaKAL_EventFlagCreate(&RecTaskEvent, RecTaskEventName);

    if (RetVal != SVC_OK) {
        SvcLog_NG(SVC_LOG_REC_TASK, "Create event flag failed %d", RetVal, 0U);
    }

    RetVal |= SvcSysStat_Register(SVC_APP_STAT_UCODE,    RecTask_MenuStatusCallback, &CtrlID);
    RetVal |= SvcSysStat_Register(SVC_APP_STAT_DSP_BOOT, RecTask_MenuStatusCallback, &CtrlID);
    RetVal |= SvcSysStat_Register(SVC_APP_STAT_STG,      RecTask_MenuStatusCallback, &CtrlID);


    if (RetVal != SVC_OK){
        SvcLog_NG(SVC_LOG_REC_TASK, "Ucode and dsp boot stat register failed", 0U, 0U);
    } else {
        RecTaskCtrl.Priority   = SVC_REC_AUTO_REC_TASK_PRI;
        RecTaskCtrl.EntryFunc  = SvcRecTask_AutoRecordEntry;
        RecTaskCtrl.pStackBase = RecTaskStack;
        RecTaskCtrl.StackSize  = RECSTACK;
        RecTaskCtrl.CpuBits    = SVC_REC_AUTO_REC_TASK_CPU_BITS;
        RecTaskCtrl.EntryArg   = (UINT32) 0U;
    }

    RetVal = SvcTask_Create("SvcAutoRec", &RecTaskCtrl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_REC_TASK, "SvcAutoRec create failed", 0U, 0U);
    }

    return RetVal;
}

/**
* Configure the record task. Including loading record framework.
* @return ErrorCode
*/
UINT32 SvcRecTask_Config(void)
{
    UINT32               RetVal = SVC_OK, i;
    static UINT32        RecFwkLoad = 0U;
    SVC_ENC_INFO_s       EncInfo;
    SVC_REC_MAIN_INFO_s  RecMainInfo;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    static const char RecTaskStrmName[4U][12U] = { "STREAM_0",  "STREAM_1",  "STREAM_2", "STREAM_3" };

    AmbaMisra_TouchUnused(&RetVal);

    RecTaskStreamBits = pCfg->RecBits;
    RecMVStreamBits   = 0U;
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
    {
        UINT32  Bit;

        for (i = 0U; i < pCfg->RecNum; i++) {
            Bit = (UINT32)(0x01U) << i;
            if (0U < pCfg->RecStrm[i].RecSetting.MVInfoFlag) {
                RecTaskStreamBits = ClearBits(RecTaskStreamBits, Bit);
                RecMVStreamBits = SetBits(RecMVStreamBits, Bit);
            }
        }
    }
#endif

    /* First time we don't need to fwk unload */
    if (RecFwkLoad == 1U) {
        SvcRecMain_FwkUnload();
#if defined(CONFIG_ICAM_STLCAP_USED)
        if (RecTaskVidThm == 1U) {
            RetVal = SvcStillTask_Create(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "SvcStillTask_Delete failed!!", 0U, 0U);
            }
            RecTaskVidThm = 0U;
        }
#endif
    }

    if (SVC_OK == RetVal) {
        /* load video encoder parameters */
        SvcEnc_InfoGet(&EncInfo);
        SvcInfoPack_EncConfig(&EncInfo);

        /* load record framework */
        SvcRecMain_InfoGet(&RecMainInfo);
        SvcInfoPack_RecConfig(&RecMainInfo);
        SvcRecMain_FwkLoad(RecMonNotify, SvcRecTask_CapThmCB);
    }

#if defined(CONFIG_ICAM_STLCAP_USED)
    if ((RetVal == SVC_OK) && (pCfg->StillCfg.EnableStill == 1U)) {
        UINT32           Src = 0U;
        SVC_USER_PREF_s  *pSvcUserPref;

        if (SVC_OK != SvcUserPref_Get(&pSvcUserPref)) {
            SvcLog_NG(SVC_LOG_REC_TASK, "SvcUserPref_Get() failed!!", 0U, 0U);
        } else {
            RecTaskVidThm = pSvcUserPref->EnableVidThm;
        }

        /* Disable still task if it's duplex mode */
        for (i = 0U; i < pCfg->FovNum; i++) {
            RetVal = SvcResCfg_GetFovSrc(i, &Src);
            if (SVC_OK == RetVal) {
                if (Src == SVC_VIN_SRC_MEM_DEC) {
                    RecTaskVidThm = 0U;
                    break;
                }
            }
        }

        if ((RetVal == SVC_OK) && (RecTaskVidThm == 1U)) {
            RetVal = SvcStillTask_Create(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);
        }
    }

#endif



    if (SVC_OK == RetVal) {
        RecTaskTargetStrmCheckBox.CheckBits = pCfg->RecBits;

        for (i = 0; i < pCfg->RecNum; i++) {
            RecTaskTargetStrmCheckBox.pList[i] = RecTaskStrmName[i];
        }
        RecTaskTargetStrmCheckBox.ListNum = pCfg->RecNum;
    }

    if (RecFwkLoad == 0U) {
        RecFwkLoad = 1U;
    }

    return RetVal;
}

/**
* DeConfig the record task. Including loading record framework.
* @return ErrorCode
*/
UINT32 SvcRecTask_DeConfig(void)
{
    UINT32 RetVal = SVC_OK;

    SvcRecMain_FwkUnload();

#if defined(CONFIG_ICAM_STLCAP_USED)
    {
        UINT32  Err;

        if (RecTaskVidThm == 1U) {
            Err = SvcStillTask_Delete(SVC_STILL_CAP | SVC_STILL_PROC | SVC_STILL_ENC | SVC_STILL_MUX);
            if (SVC_OK != Err) {
                SvcLog_NG(SVC_LOG_REC_TASK, "SvcStillTask_Delete failed %u", Err, 0U);
                RetVal = SVC_NG;
            } else {
                RecTaskVidThm = 0U;
            }
        }
    }
#endif

    return RetVal;
}

/**
* Configure which stream is going to record
* @param [in]  Confiugre the desired recording stream in bits
* @return ErrorCode
*/
UINT32 SvcRecTask_SetStreamBits(UINT32 StreamBits)
{
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    UINT32              ValidStreamBits = pCfg->RecBits;

    UINT32 RetVal;

    if ((ValidStreamBits & StreamBits) != StreamBits) {
        SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_SetStreamBits() failed. Input should be subset of ValidStreamBits (0x%x).", ValidStreamBits, 0U);
        RetVal = SVC_NG;
    } else {
        RecTaskStreamBits = StreamBits;
        RetVal = SVC_OK;
    }

    return RetVal;
}

/**
* Start record.
* @return ErrorCode
*/
UINT32 SvcRecTask_Start(void)
{
    UINT32               RetVal = SVC_OK, i, j, VinSrc = 0U, IsDuplex = 0U;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    SvcLog_OK(SVC_LOG_REC_TASK, "SvcRecTask_Start() ", 0U, 0U);

    /* User must stop previous recording stream before start next one */
    if ((RecTaskStreamBits != 0U) && ((RecTaskStartBits & RecTaskStreamBits) == 0U)) {
        /* register gui */
        if (RecTaskStartBits == 0U) {
            /* Hours */
            SvcGui_Register(VoutOfRecGui, RECORD_GUI_LEVEL_H, "Rec_H", RecTask_GuiDraw_Hours, RecTask_GuiDraw_Update);

            /* Mins */
            SvcGui_Register(VoutOfRecGui, RECORD_GUI_LEVEL_M, "Rec_M", RecTask_GuiDraw_Mins, RecTask_GuiDraw_Update);

            /* Secs */
            SvcGui_Register(VoutOfRecGui, RECORD_GUI_LEVEL_S, "Rec_S", RecTask_GuiDraw_Secs, RecTask_GuiDraw_Update);

            RetVal = AmbaWrap_memset(&Time, 0, sizeof(ICAM_TIME_INFO_s));
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "AmbaWrap_memset with err %d", RetVal, 0U);
            }
        }

        /* if it's xcode, start playback */
        for (i = 0U; i < pCfg->RecNum; i++) {
            if ((RecTaskStreamBits & (1UL << i)) > 0UL) {
                for (j = 0U; j < pCfg->RecStrm[i].StrmCfg.NumChan; j++) {
                    RetVal = SvcResCfg_GetFovSrc(pCfg->RecStrm[i].StrmCfg.ChanCfg[j].FovId, &VinSrc);
                    if ((RetVal == SVC_OK) && (VinSrc == SVC_VIN_SRC_MEM_DEC)) {
                        IsDuplex = 1U;
                        break;
                    }
                }
            }
        }

        if (IsDuplex == 1U) {
#ifdef CONFIG_ICAM_PLAYBACK_USED
            SVC_VIDPBK_INFO_s    VidInfo = {0};
            UINT32               InputVal = 0U;

            PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);
            if (VidInfo.VideoStart == 0U) {
                PictDispVideoHandler(SVC_VIDPBK_PMT_START, &InputVal);
            }
#endif
        }

#ifdef CONFIG_ICAM_RAWENC_USED
        {
            extern void SvcRawEncTask_StrmCapCreate(UINT32 RecBits, UINT8 *pBuf, UINT32 BufSize);
            SvcRawEncTask_StrmCapCreate(RecTaskStreamBits, NULL, 0U);
        }
#endif

        /* record start */
        SvcRecMain_Start(RecTaskStreamBits, 0U);

        SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0U, NULL, &RecTaskStartBits);
        if (RecTaskStartBits != 0U) {
            /* start timer to show gui */
            RetVal = AmbaKAL_TimerStart(&RecTaskGuiTimer);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Start() failed, timer cannot be started with err %d", RetVal, 0U);
            }
        } else {
            RetVal = SVC_NG;
            /* unregister gui */
            SvcGui_Unregister(VoutOfRecGui, RECORD_GUI_LEVEL_S);
            SvcGui_Unregister(VoutOfRecGui, RECORD_GUI_LEVEL_M);
            SvcGui_Unregister(VoutOfRecGui, RECORD_GUI_LEVEL_H);
        }
    } else {
        SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Start() failed, Stream (0x%x) is recording now, Input stream (0x%x)", RecTaskStartBits, RecTaskStreamBits);
        RetVal = SVC_NG;
    }

    SvcLog_OK(SVC_LOG_REC_TASK, "SvcRecTask_Start() done", 0U, 0U);
    return RetVal;
}

/**
* Stop record.
* @param [in]  IsEmgStop
* @return ErrorCode
*/
UINT32 SvcRecTask_Stop(UINT32 IsEmgStop)
{
    UINT32               RetVal = SVC_OK, IsDuplex = 0U, VinSrc = 0U, i, j;
    const SVC_RES_CFG_s  *pCfg = SvcResCfg_Get();

    SvcLog_OK(SVC_LOG_REC_TASK, "SvcRecTask_Stop()", 0U, 0U);

    if ((RecTaskStreamBits == 0U) || ((RecTaskStartBits & RecTaskStreamBits) != RecTaskStreamBits)) {
        SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Stop() failed, Stream (0x%x) is recording now, Input stream (0x%x)", RecTaskStartBits, RecTaskStreamBits);
        RetVal = SVC_NG;
    } else {

        /* if it's xcode, stop playback */
        for (i = 0U; i < pCfg->RecNum; i++) {
            if ((RecTaskStreamBits & (1UL << i)) > 0UL) {
                for (j = 0U; j < pCfg->RecStrm[i].StrmCfg.NumChan; j++) {
                    RetVal = SvcResCfg_GetFovSrc(pCfg->RecStrm[i].StrmCfg.ChanCfg[j].FovId, &VinSrc);
                    if ((RetVal == SVC_OK) && (VinSrc == SVC_VIN_SRC_MEM_DEC)) {
                        IsDuplex = 1U;
                        break;
                    }
                }
            }
        }

        if (IsDuplex == 1U) {
#ifdef CONFIG_ICAM_PLAYBACK_USED
            SVC_VIDPBK_INFO_s    VidInfo = {0};
            UINT32               InputVal = SVC_VIDPBK_STOP_STAY;

            PictDispVideoHandler(SVC_VIDPBK_PMT_GETINFO, &VidInfo);
            if (VidInfo.VideoStart == 1U) {
                PictDispVideoHandler(SVC_VIDPBK_PMT_STOP, &InputVal);
            }
#endif
        }

        SvcRecMain_Stop(RecTaskStreamBits, IsEmgStop);
        SvcRecMain_Control(SVC_RCM_GET_ENC_STATUS, 0U, NULL, &RecTaskStartBits);
        if (RecTaskStartBits == 0U) {
            RetVal = AmbaKAL_TimerStop(&RecTaskGuiTimer);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Stop() failed, timer cannot be stopped with err %d", RetVal, 0U);
            }

            /* unregister gui */
            SvcGui_Unregister(VoutOfRecGui, RECORD_GUI_LEVEL_S);
            SvcGui_Unregister(VoutOfRecGui, RECORD_GUI_LEVEL_M);
            SvcGui_Unregister(VoutOfRecGui, RECORD_GUI_LEVEL_H);
        }
    }
    SvcLog_OK(SVC_LOG_REC_TASK, "SvcRecTask_Stop() done", 0U, 0U);

#ifdef CONFIG_ICAM_RAWENC_USED
    {
        extern void SvcRawEncTask_StrmCapDelete(void);
        SvcRawEncTask_StrmCapDelete();
    }
#endif

    return RetVal;
}

/**
* Get record status (with StreamBits)
* @param [out]  pStreamBits: Current recording streaming in bits
*/
void SvcRecTask_GetStatus(UINT32 *pStreamBits)
{
    *pStreamBits = RecTaskStartBits;
}

/**
* Stop MV streams.
* @param [in]  StreamBits
*/
void SvcRecTask_CapThmCB(UINT32 StreamBits)
{
#if defined(CONFIG_ICAM_STLCAP_USED)
    UINT32                i, FovID, Src = 0U, Bits, ValidBits = StreamBits, TimelapseBits = StreamBits;
    SVC_CAP_CTRL_s    Ctrl;
    const SVC_RES_CFG_s   *pResCfg = SvcResCfg_Get();
    const SVC_REC_STRM_s  *pRec;

    if ((pResCfg->StillCfg.EnableStill == 1U) && (RecTaskVidThm == 1U)) {
        /* check if duplex/transcode stream */
        for (i = 0U; i < AMBA_DSP_MAX_STREAM_NUM; i++) {
            pRec = &(pResCfg->RecStrm[i]);

            Bits = (UINT32)(0x01U) << i;
            if (0U < CheckBits(ValidBits, Bits)) {
                /* Is decoder input ? */
                FovID = pRec->StrmCfg.ChanCfg[0].FovId;
                if (SvcResCfg_GetFovSrc(FovID, &Src) == SVC_OK) {
                    if (Src == SVC_VIN_SRC_MEM_DEC) {
                        ValidBits = ClearBits(ValidBits, Bits);
                        SvcLog_DBG(SVC_LOG_REC_TASK, "# no thumbnail for dec input(%u)", i, 0U);
                    }
                }
#if defined(CONFIG_ICAM_ENCMV_SUPPORTED)
                /* Is MV stream ? */
                if (0U < pRec->RecSetting.MVInfoFlag) {
                    ValidBits = ClearBits(ValidBits, Bits);
                    SvcLog_DBG(SVC_LOG_REC_TASK, "# no thumbnail for MV steam(%u)", i, 0U);
                }
#endif
                /* Is timelapse stream ? */
                if (0U == pRec->RecSetting.TimeLapse) {
                    TimelapseBits = ClearBits(TimelapseBits, Bits);
                }
            }
        }

        if (0U < ValidBits) {
            AmbaSvcWrap_MisraMemset(&Ctrl, 0, sizeof(Ctrl));
            Ctrl.Cfg.CapNum       = 1;
            Ctrl.Cfg.StrmMsk      = (UINT16)ValidBits << pResCfg->DispNum;
            Ctrl.Cfg.PicType      = SVC_STL_TYPE_THM;
            Ctrl.Cfg.SyncEncStart = 1;
            Ctrl.Cfg.CapOnly      = 0;
            Ctrl.Cfg.TimelapseMsk = (UINT16)TimelapseBits << pResCfg->DispNum;
            if (SvcStillTask_Capture(&Ctrl) != SVC_OK) {
                SvcLog_NG(SVC_LOG_REC_TASK, "SvcStillTask_Capture() failed", 0U, 0U);
            }
        }
    }
#else
    AmbaMisra_TouchUnused(&StreamBits);
#endif
}

/**
* Start MV streams.
*/
void SvcRecTask_StartMV(void)
{
    if (0U < RecMVStreamBits) {
        SvcRecMain_Start(RecMVStreamBits, 0U);
        SvcLog_DBG(SVC_LOG_REC_TASK, "## start MV record streams(0x%X)", RecMVStreamBits, 0U);
    }
}

/**
* Stop MV streams.
*/
void SvcRecTask_StopMV(void)
{
    if (0U < RecMVStreamBits) {
        SvcRecMain_Stop(RecMVStreamBits, 0U);
        SvcLog_DBG(SVC_LOG_REC_TASK, "## stop MV record streams(0x%X)", RecMVStreamBits, 0U);
    }
}

static void RecTask_GuiUpdate(UINT32 EntryArg)
{
    UINT32 Seconds, Minutes, Hours;
    AmbaMisra_TouchUnused(&EntryArg);

    Time.RecTaskExpiredTimes += 1U;

    Seconds = Time.RecTaskExpiredTimes % 60U;
    Minutes = (Time.RecTaskExpiredTimes / 60U) % 60U;
    Hours   = Time.RecTaskExpiredTimes / 3600U;

    if (Time.Seconds != Seconds) {
        Time.Seconds = Seconds;
    }

    if (Time.Minutes != Minutes) {
        Time.Minutes = Minutes;
    }

    if (Time.Hours != Hours) {
        Time.Hours = Hours;
    }

    Time.Update = 1U;
}

static void RecTask_GuiDraw_Update(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (Time.Update == 1U) {
        Time.Update = 0U;
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void RecTask_GuiDraw_Hours(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;
    char   Digits[4U];

    UINT32 Hours100Digit  = (Time.Hours / 100U) % 10U;
    UINT32 Hours10Digit   = (Time.Hours / 10U) % 10U;
    UINT32 HoursDigit     = (Time.Hours % 10U);

    UINT32 OffsetX        = RecTaskGuiStartX;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (RecTaskStartBits == 0U) {
        /* Draw nothing */
    } else {
        /* Background color */
        RetVal = SvcOsd_DrawSolidRect(VoutOfRecGui, OffsetX, RecTaskGuiStartY, OffsetX + (FONT_WIDTH * 4U), RecTaskGuiStartY + FONT_HEIGHT + FONT_SIZE, BACKGRD_COLOR);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawSolidRect err %d", RetVal, 0U);
        }

        /* Red rectangle */
        RetVal = SvcOsd_DrawSolidRect(VoutOfRecGui, OffsetX + (FONT_WIDTH / 3U), RecTaskGuiStartY + (FONT_HEIGHT / 3U), OffsetX + ((FONT_WIDTH * 2U) / 3U), RecTaskGuiStartY + ((FONT_HEIGHT * 2U) / 3U), 249U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawSolidRect err %d", RetVal, 0U);
        }
        OffsetX += FONT_WIDTH;

        /* draw (HHH)*/
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[0]), 2U, Hours100Digit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[1]), 2U, Hours10Digit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[2]), 2U, HoursDigit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        Digits[3U] = '\0';

        RetVal = SvcOsd_DrawString(VoutOfRecGui, OffsetX, RecTaskGuiStartY, FONT_SIZE, FONT_COLOR, Digits);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawString err %d", RetVal, 0U);
        }

    }
}

static void RecTask_GuiDraw_Mins(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;
    char   Digits[4U];

    UINT32 Minutes10Digit = Time.Minutes / 10U;
    UINT32 MinutesDigit   = Time.Minutes % 10U;

    UINT32 OffsetX        = RecTaskGuiStartX + (FONT_WIDTH * 4U);

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (RecTaskStartBits == 0U) {
        /* Draw nothing */
    } else {
        /* Background color */
        RetVal = SvcOsd_DrawSolidRect(VoutOfRecGui, OffsetX, RecTaskGuiStartY, OffsetX + (FONT_WIDTH * 3U), RecTaskGuiStartY + FONT_HEIGHT + FONT_SIZE, BACKGRD_COLOR);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawSolidRect err %d", RetVal, 0U);
        }

        /* draw (:MM)*/
        Digits[0U] = ':';
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[1]), 2U, Minutes10Digit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[2]), 2U, MinutesDigit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        Digits[3U] = '\0';

        RetVal = SvcOsd_DrawString(VoutOfRecGui, OffsetX, RecTaskGuiStartY, FONT_SIZE, FONT_COLOR, Digits);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawString err %d", RetVal, 0U);
        }
    }
}

static void RecTask_GuiDraw_Secs(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;
    char   Digits[4U];

    UINT32 Seconds10Digit = Time.Seconds / 10U;
    UINT32 SecondsDigit   = Time.Seconds % 10U;

    UINT32 OffsetX        = RecTaskGuiStartX + (FONT_WIDTH * 3U) + (FONT_WIDTH * 4U);

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (RecTaskStartBits == 0U) {
        /* Draw nothing */
    } else {
        /* Background color */
        RetVal = SvcOsd_DrawSolidRect(VoutOfRecGui, OffsetX, RecTaskGuiStartY, OffsetX + (FONT_WIDTH * 3U), RecTaskGuiStartY + FONT_HEIGHT + FONT_SIZE, BACKGRD_COLOR);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawSolidRect err %d", RetVal, 0U);
        }

        /* draw (:SS)*/
        Digits[0U] = ':';
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[1]), 2U, Seconds10Digit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        RetVal = AmbaUtility_UInt32ToStr(&(Digits[2]), 2U, SecondsDigit, 10U);
        if (1U != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. AmbaUtility_UInt32ToStr err %d", RetVal, 0U);
        }
        Digits[3U] = '\0';

        RetVal = SvcOsd_DrawString(VoutOfRecGui, OffsetX, RecTaskGuiStartY, FONT_SIZE, FONT_COLOR, Digits);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_GuiDraw() failed. SvcOsd_DrawString err %d", RetVal, 0U);
        }

    }
}

static void RecTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;
    SVC_APP_STAT_MENU_s     *pStatus = NULL;
    SVC_APP_STAT_UCODE_s    *pUcodeStatus = NULL;
    SVC_APP_STAT_DSP_BOOT_s *pDspBootStatus = NULL;
    SVC_APP_STAT_STG_s      *pStgStatus = NULL;

    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus); /* Misra-c fixed */

    AmbaMisra_TouchUnused(pUcodeStatus);
    AmbaMisra_TouchUnused(pDspBootStatus);
    AmbaMisra_TouchUnused(pStgStatus);

    /* Verify this is record event */
    if (StatIdx == SVC_APP_STAT_MENU) {
        AmbaMisra_TypeCast(&pStatus, &pInfo);
        if (((pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) && (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_RECORD)) ||
            ((pStatus->Type == SVC_APP_STAT_MENU_TYPE_ADAS_DVR) && (pStatus->FuncIdx == SVC_APP_STAT_MENU_ADAS_RECORD)) ||
            ((pStatus->Type == SVC_APP_STAT_MENU_TYPE_DMS) && (pStatus->FuncIdx == SVC_APP_STAT_MENU_DMS_RECORD))) {

            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                if ((RecTaskOsdShowTargetStrm & REC_TASK_OSD_STRM_SHOW) == 0U) {
                    UINT32 OsdWidth, OsdHeight;

                    /* To show GUI for choosing Fov */
                    RecTaskOsdShowTargetStrm |= REC_TASK_OSD_STRM_SHOW;
                    RecTaskOsdShowTargetStrm |= REC_TASK_OSD_STRM_UPDATE;
                    RecTaskOsdShowTargetStrmOnVout = pStatus->Vout;

                    RetVal = SvcOsd_GetOsdBufSize(RecTaskOsdShowTargetStrmOnVout, &OsdWidth, &OsdHeight);
                    if (SVC_OK == RetVal) {
                        RecTaskTargetStrmCanvas.StartX = (OsdWidth - (REC_TASK_STRM_GUI_W)) >> (UINT32) 1U;
                        RecTaskTargetStrmCanvas.StartY = (OsdHeight - (REC_TASK_STRM_GUI_H)) >> (UINT32) 1U;
                        RecTaskTargetStrmCanvas.Width  = REC_TASK_STRM_GUI_W;
                        RecTaskTargetStrmCanvas.Height = REC_TASK_STRM_GUI_H;
                    }

                    SvcGui_Register(pStatus->Vout, 17U, "RecMenu", RecTask_OsdShowStrmEntry, RecTask_OsdShowStrmUpdate);
                    RetVal = SvcButtonCtrl_Request(&RecTaskButtonCtrl_ID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback err. Button request failed", 0U, 0U);
                    }
                } else {
                    RecTaskOsdShowTargetStrm &= ~(REC_TASK_OSD_STRM_SHOW);
                    SvcGui_Unregister(pStatus->Vout, 17U);
                    RetVal = SvcButtonCtrl_Release(&RecTaskButtonCtrl_ID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback err. Button release failed", 0U, 0U);
                    }
                }
            }
        }
    } else if (StatIdx == SVC_APP_STAT_UCODE){
        AmbaMisra_TypeCast(&pUcodeStatus, &pInfo);
        if(pUcodeStatus->Status == SVC_APP_STAT_UCODE_ALL_DONE){
            RetVal = AmbaKAL_EventFlagSet(&RecTaskEvent, REC_TASK_UCODE_ALL);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_UCODE_ALL failed with 0x%x", RetVal, 0U);
            }
        } else if (pUcodeStatus->Status == SVC_APP_STAT_UCODE_LOAD_STAGE1_DONE){
            RetVal = AmbaKAL_EventFlagSet(&RecTaskEvent, REC_TASK_UCODE_STAGE1_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_UCODE_ALL failed with 0x%x", RetVal, 0U);
            }
        } else {
            /**/
        }
    } else if (StatIdx == SVC_APP_STAT_DSP_BOOT){
        AmbaMisra_TypeCast(&pDspBootStatus, &pInfo);
        if (pDspBootStatus->Status == SVC_APP_STAT_DSP_BOOT_DONE){
            RetVal = AmbaKAL_EventFlagSet(&RecTaskEvent, REC_TASK_DSP_BOOT_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_DSP_BOOT_DONE failed with 0x%x", RetVal, 0U);
            }
        }
    }
#if !defined(CONFIG_APP_FLOW_CARDV_AONI)
	else if (StatIdx == SVC_APP_STAT_STG) {
        AmbaMisra_TypeCast(&pStgStatus, &pInfo);
        if ((pStgStatus->Status & SVC_APP_STAT_STG_C_READY) > 0U) {
            RetVal = AmbaKAL_EventFlagSet(&RecTaskEvent, REC_TASK_STG_C_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_STG_C_DONE failed with 0x%x", RetVal, 0U);
            }
        }
        if ((pStgStatus->Status & SVC_APP_STAT_STG_D_READY) > 0U) {
            RetVal = AmbaKAL_EventFlagSet(&RecTaskEvent, REC_TASK_STG_D_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_STG_D_DONE failed with 0x%x", RetVal, 0U);
            }
        }
        if ((pStgStatus->Status & SVC_APP_STAT_STG_I_READY) > 0U) {
            RetVal = AmbaKAL_EventFlagSet(&RecTaskEvent, REC_TASK_STG_I_DONE);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_REC_TASK, "RecTask_MenuStatusCallback() err, AmbaKAL_EventFlagSet REC_TASK_STG_I_DONE failed with 0x%x", RetVal, 0U);
            }
        }
    }
#endif
	else {
        /* Do nothing */
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static void RecTask_OsdShowStrmEntry(UINT32 VoutIdx, UINT32 Level)
{
    SVC_GUI_CANVAS_s CheckBoxCanvas = { .StartX = RecTaskTargetStrmCanvas.StartX, .StartY = RecTaskTargetStrmCanvas.StartY, .Width = REC_TASK_STRM_GUI_W, .Height = REC_TASK_STRM_GUI_CHECKBOX_H };
    SVC_GUI_CANVAS_s TextViewCanvas = { .StartX = RecTaskTargetStrmCanvas.StartX, .StartY = RecTaskTargetStrmCanvas.StartY + REC_TASK_STRM_GUI_CHECKBOX_H, .Width = REC_TASK_STRM_GUI_W, .Height = REC_TASK_STRM_GUI_TEXTBOX1_H };
    SVC_GUI_TEXTVIEW_s TextView = {
#ifdef CONFIG_ICAM_32BITS_OSD_USED
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
#else
        .BackgroundColor = 73U,
        .FontColor = 255U,
#endif
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "1: Pick 2: REC 3: Move 4: Leave",
    };
    SVC_GUI_CANVAS_s TextViewCanvas1 = { .StartX = RecTaskTargetStrmCanvas.StartX, .StartY = RecTaskTargetStrmCanvas.StartY + REC_TASK_STRM_GUI_CHECKBOX_H + REC_TASK_STRM_GUI_TEXTBOX1_H, .Width = REC_TASK_STRM_GUI_W, .Height = REC_TASK_STRM_GUI_TEXTBOX2_H };
    SVC_GUI_TEXTVIEW_s TextView1 = {
#ifdef CONFIG_ICAM_32BITS_OSD_USED
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
#else
        .BackgroundColor = 73U,
        .FontColor = 255U,
#endif
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "-: Pick +: REC !: Move @: Leave",
    };

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);


    if ((RecTaskOsdShowTargetStrm & REC_TASK_OSD_STRM_SHOW) > 0U) {
        SvcGuiCheckBox_Draw(RecTaskOsdShowTargetStrmOnVout, &CheckBoxCanvas,  &RecTaskTargetStrmCheckBox);
        SvcGuiTextView_Draw(RecTaskOsdShowTargetStrmOnVout, &TextViewCanvas,  &TextView);
        SvcGuiTextView_Draw(RecTaskOsdShowTargetStrmOnVout, &TextViewCanvas1, &TextView1);
    }
}

static void RecTask_OsdShowStrmUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((RecTaskOsdShowTargetStrm & REC_TASK_OSD_STRM_UPDATE) > 0U) {
        *pUpdate = 1U;
        RecTaskOsdShowTargetStrm &= ~(REC_TASK_OSD_STRM_UPDATE);
    } else {
        *pUpdate = 0U;
    }

}

static void RecTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcRecCmd;

    UINT32  RetVal;

    SvcRecCmd.pName    = "svc_rec_task";
    SvcRecCmd.MainFunc = RecTask_CmdEntry;
    SvcRecCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcRecCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_REC_TASK, "## fail to install svc emr command", 0U, 0U);
    }
}


static UINT32 RecTask_Minus(void)
{
    if ((RecTaskTargetStrmCheckBox.CheckBits & ((UINT32) 1U << RecTaskTargetStrmCheckBox.CurIdx)) > 0U) {
        RecTaskTargetStrmCheckBox.CheckBits &= ~((UINT32) 1U << RecTaskTargetStrmCheckBox.CurIdx);
    } else {
        RecTaskTargetStrmCheckBox.CheckBits |= ((UINT32) 1U << RecTaskTargetStrmCheckBox.CurIdx);
    }

    RecTaskStreamBits = RecTaskTargetStrmCheckBox.CheckBits;

    return SVC_OK;
}

static UINT32 RecTask_Plus(void)
{
    UINT32 RecStatus, Err;

    SvcRecTask_GetStatus(&RecStatus);
    if ((RecStatus & RecTaskStreamBits) == RecTaskStreamBits) {
        Err = SvcRecTask_Stop(0U);
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Stop failed %u", Err, 0U);
        }
    } else {
        Err = SvcRecTask_Start();
        if (Err != SVC_OK) {
            SvcLog_NG(SVC_LOG_REC_TASK, "SvcRecTask_Start failed %u", Err, 0U);
        }
    }

    return SVC_OK;
}

static UINT32 RecTask_Move(void)
{
    RecTaskTargetStrmCheckBox.CurIdx = (RecTaskTargetStrmCheckBox.CurIdx + 1U) % RecTaskTargetStrmCheckBox.ListNum;
    RecTaskOsdShowTargetStrm |= REC_TASK_OSD_STRM_UPDATE;

    return SVC_OK;
}

static void RecTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            RetVal = SvcRecTask_Start();
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            RetVal = SvcRecTask_Stop(0U);
        } else if (0 == SvcWrap_strcmp("streambits", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 StreamBits;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &StreamBits);
                if (SVC_OK == RetVal) {
                    RetVal = SvcRecTask_SetStreamBits(StreamBits);
                }
            } else {
                RetVal = SVC_NG;
            }
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        RecTask_CmdUsage(PrintFunc);
    }
}

static void RecTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_rec_task commands:\n");
    PrintFunc("             start : Start recording\n");
    PrintFunc("             stop  : Stop recording\n");
    PrintFunc("             streambits [value] : set the streambits for record start/stop");
}
