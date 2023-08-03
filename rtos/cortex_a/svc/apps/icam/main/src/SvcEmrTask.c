/**
 *  @file SvcEmrTask.c
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
 *  @details svc emr task
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaDMA_Def.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaSYS.h"

/* ssp */
#include "AmbaDSP.h"
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_Liveview.h"

/* framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcPref.h"
#include "SvcCmd.h"

/* app-shared */
#include "SvcSysStat.h"
#include "SvcButtonCtrl.h"
#include "SvcResCfg.h"
#include "SvcViewCtrl.h"
#include "SvcGui.h"
#include "SvcGuiCheckBox.h"
#include "SvcGuiTextView.h"
#include "SvcOsd.h"

/* app-icam */
#include "SvcAppStat.h"
#include "SvcUserPref.h"

#if defined(CONFIG_BUILD_AMBA_ADAS)
#include "SvcCan.h"
#endif

#include ".svc_autogen"
#include "SvcEmrTask.h"

#define SVC_LOG_EMR_TASK "EMR_TASK"
#define SVC_EMR_TASK_ANIM_TASK_STACK_SIZE (0x1000)
#define SVC_EMR_TASK_CAN_BUS_TASK_STACK_SIZE (0x1000)

#define SVC_EMR_TASK_ZOOM_STEP  (20U)

#if defined(CONFIG_BUILD_AMBA_ADAS)
#define SVC_EMR_TASK_ICON_WIDTH     (80U)
#define SVC_EMR_TASK_ICON_HEIGHT    (80U)
#define SVC_EMR_TASK_ICON_SIZE      (SVC_EMR_TASK_ICON_WIDTH * SVC_EMR_TASK_ICON_HEIGHT)

#define SVC_EMR_TASK_2_0_FORMAT_ID      (16U)
#define SVC_EMR_TASK_2_0_PREV_FORMAT_ID (23U)
#endif

#define SVC_EMR_TASK_FOV_GUI_CHECKBOX_W (270U)
#define SVC_EMR_TASK_FOV_GUI_CHECKBOX_H (60U)
#define SVC_EMR_TASK_FOV_GUI_TEXTBOX1_H (25U)
#define SVC_EMR_TASK_FOV_GUI_TEXTBOX2_H (25U)
#define SVC_EMR_TASK_FOV_GUI_W          (SVC_EMR_TASK_FOV_GUI_CHECKBOX_W)
#define SVC_EMR_TASK_FOV_GUI_H          (SVC_EMR_TASK_FOV_GUI_CHECKBOX_H + SVC_EMR_TASK_FOV_GUI_TEXTBOX1_H + SVC_EMR_TASK_FOV_GUI_TEXTBOX2_H)

#define SVC_EMR_TASK_GUI_FOV_SHOW       (0x01U)
#define SVC_EMR_TASK_GUI_EMR2_0_SHOW    (0x02U)
#define SVC_EMR_TASK_GUI_FOV_UPDATE     (0x10U)
#define SVC_EMR_TASK_GUI_EMR2_0_UPDATE  (0x20U)

typedef struct /*SVC_EMR_TASK_ANIM_TASK_s*/ {
    char            TaskName[32];
    AMBA_KAL_TASK_t TaskCtrl;
    UINT32          Priority;
    UINT32          EntryArg;
    UINT32          StackSize;
    UINT8           Stack[SVC_EMR_TASK_ANIM_TASK_STACK_SIZE];
    UINT8           TaskCreated;
} SVC_EMR_TASK_ANIM_TASK_s;

typedef struct /* SVC_EMR_TASK_ANIM_INFO_s */ {
    SVC_EMR_TASK_WINDOW_s SrcWin;
    SVC_EMR_TASK_WINDOW_s DstWin;
    UINT8                 Status;
} SVC_EMR_TASK_ANIM_INFO_s;

typedef struct /* SVC_EMR_TASK_CAN_BUS_TASK_s */ {
    char            TaskName[32];
    AMBA_KAL_TASK_t TaskCtrl;
    UINT32          Priority;
    UINT32          EntryArg;
    UINT32          StackSize;
    UINT8           Stack[SVC_EMR_TASK_CAN_BUS_TASK_STACK_SIZE];
    UINT8           TaskCreated;
} SVC_EMR_TASK_CAN_BUS_TASK_s;

#if defined(CONFIG_BUILD_AMBA_ADAS)
typedef struct /* SVC_EMR_TASK_CAN_BUS_DATA_s */ {
    UINT32 GearStatus;
    UINT32 DirLightStatus;
} SVC_EMR_TASK_CAN_BUS_DATA_s;

typedef struct /* SVC_EMR_TASK_CAN_BUS_INFO_s */ {
#define SVC_EMR_TASK_CAN_BUS_DATA_QUEUE_NUM (5U)
    SVC_EMR_TASK_CAN_BUS_DATA_s Queue[SVC_EMR_TASK_CAN_BUS_DATA_QUEUE_NUM];
    AMBA_KAL_MSG_QUEUE_t        QueueId;
    char                        QueueName[32];
} SVC_EMR_TASK_CAN_BUS_INFO_s;
#endif

/* Task for zoom animation */
static void* EmrTaskAnimTaskEntry(void* EntryArg);

/* Callback of Menu status */
static void EmrTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);

/* Task for listening can-bus raw data */
static void EmrTask_CanBusDataListener(UINT32 EntryArg);
static void* EmrTask_CanBusEntry(void* EntryArg);

static void EmrTask_OsdDrawVout0(UINT32 VoutIdx, UINT32 Level);
#if defined(AMBA_DSP_MAX_VOUT_NUM) && (AMBA_DSP_MAX_VOUT_NUM > 1U)
static void EmrTask_OsdDrawVout1(UINT32 VoutIdx, UINT32 Level);
#endif
static void EmrTask_OsdDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);

static void EmrTask_OsdShowFovEntry(UINT32 VoutIdx, UINT32 Level);

/* Button Ctrl callback */
static UINT32 EmrTask_Minus(void);
static UINT32 EmrTask_Plus(void);
static UINT32 EmrTask_Move(void);

static void EmrTask_CmdInstall(void);
static void EmrTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void EmrTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

/* Parameter for controlling EmrTask */
static UINT32 EmrTaskTargetFov = 0U;
static UINT32 EmrTaskTargetVout = 0U;
static INT32  EmrTaskUnit = 2;

/* Parameter for button control */
static SVC_BUTTON_CTRL_ID EmrTaskButtonCtrl_ID = 0U;

/* Parameter for controlling OSD showing target fov */
static UINT32 EmrTaskGuiCtrl = 0U;
static UINT32 EmrTaskOsdShowTargetFovOnVout = 0U;
static SVC_GUI_CANVAS_s EmrTaskTargetFovCanvas;
static SVC_GUI_CHECKBOX_s EmrTaskTargetFovCheckBox = {
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

static SVC_EMR_TASK_ANIM_TASK_s EmrTaskAnimTask[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static SVC_EMR_TASK_ANIM_INFO_s EmrTaskZoomAnimInfo[AMBA_DSP_MAX_VIEWZONE_NUM] GNU_SECTION_NOZEROINIT;
static UINT8 EmrTaskFunc = 0U;

#if defined(CONFIG_BUILD_AMBA_ADAS)
static SVC_EMR_TASK_CAN_BUS_INFO_s EmrTaskCanBusMsg;
static SVC_EMR_TASK_CAN_BUS_DATA_s EmrTaskCanBusData = {0};

static UINT8 EmrTaskGearRLIcon[SVC_EMR_TASK_ICON_SIZE] GNU_SECTION_NOZEROINIT;  /* Gear R for left mirror */
static UINT8 EmrTaskGearRRIcon[SVC_EMR_TASK_ICON_SIZE] GNU_SECTION_NOZEROINIT;  /* Gear R for right mirror */
static UINT8 EmrTaskDirLightLIcon[SVC_EMR_TASK_ICON_SIZE] GNU_SECTION_NOZEROINIT;   /* Direction light for left mirror */
static UINT8 EmrTaskDirLightRIcon[SVC_EMR_TASK_ICON_SIZE] GNU_SECTION_NOZEROINIT;   /* Direction light for right mirror */
static UINT8 EmrTaskEnableEmr_2_0 = 0U;   /* For demo emr 2.0 */
#endif

static void Svc_PrintNg(const char *pFormat, UINT32 Line, UINT32 RVAL)
{
    if(RVAL != SVC_OK) {
        SvcLog_NG(pFormat, "Line %d RVAL %d", Line, RVAL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Init
 *
 *  @Description:: Init the Emr task, using view ctrl module to operate emirror
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Init(void)
{
    UINT32 RetVal;
    UINT32 i;

    static AMBA_KAL_TIMER_t EmrTaskCanBusDataListenerTimer;
    static SVC_EMR_TASK_CAN_BUS_TASK_s EmrTaskCanBusTask GNU_SECTION_NOZEROINIT;

    /* Callback for ButtonControl */
    static SVC_BUTTON_CTRL_LIST_s EmrTaskButtonCtrl[3] = {
        { "button_1", EmrTask_Minus },
        { "button_2", EmrTask_Plus  },
        { "button_3", EmrTask_Move  },
    };

    SvcLog_OK(SVC_LOG_EMR_TASK, "@@ SvcEmrTask_Init() start", 0U, 0U);

    EmrTask_CmdInstall();

    {
        SVC_SYS_STAT_ID StatusID;
        RetVal = SvcSysStat_Register(SVC_APP_STAT_MENU, EmrTask_MenuStatusCallback, &StatusID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Init() failed. Menu status register fail", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcButtonCtrl_Register(SVC_BUTTON_TYPE_GPIO, EmrTaskButtonCtrl, 3U, &EmrTaskButtonCtrl_ID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Init() failed. Button register fail", 0U, 0U);
        }
    }

#if defined(CONFIG_BUILD_AMBA_ADAS)
    if (SVC_OK == RetVal) {
        RetVal |= SvcOsd_LoadBmp(EmrTaskGearRLIcon, sizeof(UINT8) * SVC_EMR_TASK_ICON_SIZE, "icon_gear_r_left.bmp");
        RetVal |= SvcOsd_LoadBmp(EmrTaskGearRRIcon, sizeof(UINT8) * SVC_EMR_TASK_ICON_SIZE, "icon_gear_r_right.bmp");
        RetVal |= SvcOsd_LoadBmp(EmrTaskDirLightLIcon, sizeof(UINT8) * SVC_EMR_TASK_ICON_SIZE, "icon_directional_light_left.bmp");
        RetVal |= SvcOsd_LoadBmp(EmrTaskDirLightRIcon, sizeof(UINT8) * SVC_EMR_TASK_ICON_SIZE, "icon_directional_light_right.bmp");
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Init() failed. Load icon bmp fail", 0U, 0U);
        }
        /* It is ok if there's no bmp loaded */
        RetVal = SVC_OK;
    }

    if (SVC_OK == RetVal) {
        const char EmrTaskCanBusQueueName[32U] = "EmrTaskCanBusQueue";
        RetVal = AmbaWrap_memcpy(EmrTaskCanBusMsg.QueueName, EmrTaskCanBusQueueName, sizeof(char) * 32U);
        RetVal = AmbaKAL_MsgQueueCreate(&EmrTaskCanBusMsg.QueueId,
                                        EmrTaskCanBusMsg.QueueName,
                                        sizeof(SVC_EMR_TASK_CAN_BUS_DATA_s),
                                        EmrTaskCanBusMsg.Queue,
                                        SVC_EMR_TASK_CAN_BUS_DATA_QUEUE_NUM * sizeof(SVC_EMR_TASK_CAN_BUS_DATA_s));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Init() err. Msg Queue Create fail %d", RetVal, 0U);
        }
    }
#endif

    if (SVC_OK == RetVal) {
        const char EmrTaskCanBusTaskName[32U] = "EmrTaskCanBusTask";
        RetVal = AmbaWrap_memcpy(EmrTaskCanBusTask.TaskName, EmrTaskCanBusTaskName, sizeof(char) * 32U);
        EmrTaskCanBusTask.Priority = 151U;
        EmrTaskCanBusTask.EntryArg = 0U;
        EmrTaskCanBusTask.StackSize = SVC_EMR_TASK_CAN_BUS_TASK_STACK_SIZE;

        RetVal = AmbaKAL_TaskCreate(&EmrTaskCanBusTask.TaskCtrl,
                                     EmrTaskCanBusTask.TaskName,
                                     EmrTaskCanBusTask.Priority,
                                     EmrTask_CanBusEntry,
                                     &(EmrTaskCanBusTask.EntryArg),
                                     EmrTaskCanBusTask.Stack,
                                     EmrTaskCanBusTask.StackSize,
                                     AMBA_KAL_AUTO_START);

        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Init() err. CanBus task create fail %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        char CanBusTimerName[32] = "EmrTaskCanBusDataListenerTimer";
        RetVal = AmbaKAL_TimerCreate(&EmrTaskCanBusDataListenerTimer, CanBusTimerName, EmrTask_CanBusDataListener, 0U, 100U, 100U, AMBA_KAL_AUTO_START);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Init() err. CanBusTimer create failed with %d", RetVal, 0U);
        }
    }

    for (i = 0; i < AMBA_DSP_MAX_VIEWZONE_NUM; i++) {
        EmrTaskAnimTask[i].TaskCreated = 0U;
        EmrTaskZoomAnimInfo[i].Status = 0U;
    }

    SvcLog_OK(SVC_LOG_EMR_TASK, "@@ SvcEmrTask_Init() done", 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Config
 *
 *  @Description:: Configure the Emr task, i.e., configure the view control based emirror configuration
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Config(void)
{
    UINT32 RetVal;
    UINT32 i, j, Rlen;
    UINT32 VinIDs[AMBA_DSP_MAX_VIN_NUM] = {0U}, VinID = 0U, VinNum = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0U}, FovIdx = 0U, FovNum = 0U;
    SVC_VIEW_CTRL_WARP_INFO_s MirrorInfo[AMBA_DSP_MAX_VIEWZONE_NUM];

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    SVC_USER_PREF_s *pSvcUserPref;

    static char EmrTaskFovName[AMBA_DSP_MAX_VIEWZONE_NUM][12U];

    RetVal = AmbaWrap_memset(MirrorInfo, 0, sizeof(MirrorInfo));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "MirrofInfo reset failed", 0U, 0U);
    }

    RetVal = SvcViewCtrl_Init();
    if (SVC_OK == RetVal) {
        /* Do nothing */
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcViewCtrl_Init() err. SvcViewCtrl_Init() failed with %d", RetVal, 0U);
    }

    (void)SvcWrap_strcpy(EmrTaskFovName[0], 12U, "FOV0(REAR)");
    (void)SvcWrap_strcpy(EmrTaskFovName[1], 12U, "FOV1(LEFT)");
    (void)SvcWrap_strcpy(EmrTaskFovName[2], 12U, "FOV2(RIGHT)");

    //Initial the rest of the EmrTaskFovName, e.g.,FOV3, FOV4, FOV5, ....
    for(i = 3U; i< AMBA_DSP_MAX_VIEWZONE_NUM;i++) {
        /* Double value to String */
        SVC_WRAP_SNPRINT "FOV%d"
                         SVC_SNPRN_ARG_S EmrTaskFovName[i]
                         SVC_SNPRN_ARG_UINT32 i SVC_SNPRN_ARG_POST
                         SVC_SNPRN_ARG_BSIZE 12U
                         SVC_SNPRN_ARG_RLEN &Rlen
                         SVC_SNPRN_ARG_E
    }

    SvcLog_OK(SVC_LOG_EMR_TASK, "@@ SvcEmrTask_Config() start", 0U, 0U);

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Config() err. SvcUserPref_Get() failed %d", RetVal, 0U);
    }

#if defined(CONFIG_BUILD_AMBA_ADAS)
    if (SVC_OK == RetVal) {
        SVC_APP_STAT_EMR_VER_s Version;
        if ((pSvcUserPref->FormatId == SVC_EMR_TASK_2_0_FORMAT_ID) || (pSvcUserPref->FormatId == SVC_EMR_TASK_2_0_PREV_FORMAT_ID)) {
            Version.Status = SVC_APP_STAT_EMR_VER_2_0;
            EmrTaskEnableEmr_2_0 = 1U;
        } else {
            Version.Status = SVC_APP_STAT_EMR_VER_1_0;
            EmrTaskEnableEmr_2_0 = 0U;
        }
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_EMR_VER, &Version);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcSysStat_Issue failed", 0U, 0U);
        }
    }
#endif

    if (SVC_OK == RetVal) {
        RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
        if (SVC_OK == RetVal) {
            for (i = 0U; i < VinNum; i++) {
                VinID = VinIDs[i];
                RetVal = SvcResCfg_GetFovIdxsInVinID(VinID, FovIdxs, &FovNum);
                if (SVC_OK == RetVal) {
                    for (j = 0U; j < FovNum; j++) {
                        FovIdx = FovIdxs[j];
                        MirrorInfo[FovIdx].Position.ShiftX = pSvcUserPref->EmrInfo[FovIdx].ShiftX;
                        MirrorInfo[FovIdx].Position.ShiftY = pSvcUserPref->EmrInfo[FovIdx].ShiftY;
                        MirrorInfo[FovIdx].Position.ZoomX = pSvcUserPref->EmrInfo[FovIdx].ZoomX;
                        MirrorInfo[FovIdx].Position.ZoomY = pSvcUserPref->EmrInfo[FovIdx].ZoomY;
                        MirrorInfo[FovIdx].Rotation.Theta = pSvcUserPref->EmrInfo[FovIdx].Theta;
                    }
                } else {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Config() err. SvcResCfg_GetFovIdxsInVinID() failed %d", RetVal, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Config() err. SvcResCfg_GetVinIDs() failed %d", RetVal, 0U);
        }
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Config() err. SvcResCfg_GetVinIDs() failed %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcViewCtrl_Config(MirrorInfo);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Config() err. SvcViewCtrl_Config() failed %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        UINT32 VoutIdx;

        for (i = 0U; i < pCfg->DispNum; i++) {
            UINT32 OsdWidth, OsdHeight;
            VoutIdx = pCfg->DispStrm[i].VoutID;
            RetVal = SvcOsd_GetOsdBufSize(VoutIdx, &OsdWidth, &OsdHeight);
            if (SVC_OK == RetVal) {
                if (VoutIdx == 0U) {
                    SvcGui_Register(VoutIdx, 27U, "Emr2_0", EmrTask_OsdDrawVout0, EmrTask_OsdDrawUpdate);
                    EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_SHOW;
                } else if (VoutIdx == 1U) {
#if defined(AMBA_DSP_MAX_VOUT_NUM) && (AMBA_DSP_MAX_VOUT_NUM > 1U)
                    SvcGui_Register(VoutIdx, 27U, "Emr2_0", EmrTask_OsdDrawVout1, EmrTask_OsdDrawUpdate);
#endif
                    EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_SHOW;
                } else {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "Unkown VOUT for registered", 0U, 0U);
                }
            }
        }
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);
        if (SVC_OK == RetVal) {
            for (i = 0; i < FovNum; i++) {
                FovIdx = FovIdxs[i];
                EmrTaskTargetFovCheckBox.pList[i] = EmrTaskFovName[FovIdx];
            }
            EmrTaskTargetFovCheckBox.ListNum = FovNum;
        }
    }

    SvcLog_OK(SVC_LOG_EMR_TASK, "@@ SvcEmrTask_Config() done", 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Stop
 *
 *  @Description:: Stop the Emr task. Release some uncessary resource
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 VoutIdx, i;
    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    SvcLog_OK(SVC_LOG_EMR_TASK, "@@ SvcEmrTask_Stop() start", 0U, 0U);

    RetVal = SvcViewCtrl_DeInit();
    if (SVC_OK == RetVal) {
        /* Do nothing */
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcViewCtrl_DeInit() err. SvcViewCtrl_DeInit() failed with %d", RetVal, 0U);
    }

    for (i = 0U; i < pCfg->DispNum; i++) {
        VoutIdx = pCfg->DispStrm[i].VoutID;
        if (VoutIdx == 0U) {
            SvcGui_Unregister(VoutIdx, 27U);
        } else if (VoutIdx == 1U) {
            SvcGui_Unregister(VoutIdx, 27U);
        } else {
            SvcLog_NG(SVC_LOG_EMR_TASK, "Unkown VOUT for registered", 0U, 0U);
        }
    }

    SvcLog_OK(SVC_LOG_EMR_TASK, "@@ SvcEmrTask_Stop() done", 0U, 0U);

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_SetTarget
 *
 *  @Description:: Set the emr target fov
 *
 *  @Input      ::
 *                 FovIdx: The target fov index
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_SetTarget(UINT32 FovIdx)
{
    UINT32 RetVal;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0U}, FovNum;
    UINT32 i;

    RetVal = SvcResCfg_GetFovIdxs(FovIdxs, &FovNum);

    if (SVC_OK == RetVal) {
        for (i = 0; i < FovNum; i++) {
            /* Can find the FovIdx in SvcResCfg */
            if (FovIdx == FovIdxs[i]) {
                SvcLog_OK(SVC_LOG_EMR_TASK, "SvcEmrTask_SetTarget() succeed. Now target Fov is %d", FovIdx, 0U);
                EmrTaskTargetFov = FovIdx;
                break;
            }
        }

        if (i == FovNum) {
            EmrTaskTargetFov = FovIdxs[0];
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SetTarget() err. Cannot find the FovIdx(%d) in the system. Force target as %d", FovIdx, FovIdxs[0]);
            RetVal = SVC_NG;
        }
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SetTarget() err. SvcResCfg cannot get FovIdxs", 0U, 0U);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_SetUnit
 *
 *  @Description:: Set the operation unit
 *
 *  @Input      ::
 *                 Unit: The unit of operation
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_SetUnit(INT32 Unit)
{
    UINT32 RetVal;

    if (Unit < 0) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SetUnit() err. Unit should not smaller than 0", 0U, 0U);
        RetVal = SVC_NG;
    } else {
        EmrTaskUnit = Unit;
        SvcLog_OK(SVC_LOG_EMR_TASK, "SvcEmrTask_SetUnit() succeed. Now move unit is %d", (UINT32) Unit, 0U);
        RetVal = SVC_OK;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Pan
 *
 *  @Description:: Emr operation pan, move fov in horizontal direction
 *
 *  @Input      ::
 *                 Value: The operation value
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Pan(INT32 Value)
{
    UINT32 RetVal;
    UINT32 VinID;
    SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    RetVal = SvcResCfg_GetVinIDOfFovIdx(EmrTaskTargetFov, &VinID);
    if (SVC_OK == RetVal) {
        MirrorCmdMsg.Value  = (INT32) (EmrTaskUnit * Value);
        MirrorCmdMsg.VinID  = (UINT8) VinID;
        MirrorCmdMsg.FovIdx = (UINT8) EmrTaskTargetFov;
        MirrorCmdMsg.VoutID = (UINT8) EmrTaskTargetVout;
        if (pResCfg->FovCfg[EmrTaskTargetFov].PipeCfg.ViewCtrlOperation == SVC_RES_VIEW_CTRL_OP_DZOOM) {
            MirrorCmdMsg.Cmd = SVC_VIEW_CTRL_WARP_CMD_PAN;
        } else {
            MirrorCmdMsg.Cmd = SVC_VIEW_CTRL_PREV_CMD_PAN;
        }

        SvcViewCtrl_Exec(MirrorCmdMsg);
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Pan() err. SvcResCfg_GetVinIDOfFovIdx() failed %d", RetVal, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Tilt
 *
 *  @Description:: Emr operation tile, move fov in vertical direction
 *
 *  @Input      ::
 *                 Value: The operation value
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Tilt(INT32 Value)
{
    UINT32 RetVal;
    UINT32 VinID;
    SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    RetVal = SvcResCfg_GetVinIDOfFovIdx(EmrTaskTargetFov, &VinID);
    if (SVC_OK == RetVal) {
        MirrorCmdMsg.Value  = (INT32) (EmrTaskUnit * Value);
        MirrorCmdMsg.VinID  = (UINT8) VinID;
        MirrorCmdMsg.FovIdx = (UINT8) EmrTaskTargetFov;
        MirrorCmdMsg.VoutID = (UINT8) EmrTaskTargetVout;
        if (pResCfg->FovCfg[EmrTaskTargetFov].PipeCfg.ViewCtrlOperation == SVC_RES_VIEW_CTRL_OP_DZOOM) {
            MirrorCmdMsg.Cmd    = SVC_VIEW_CTRL_WARP_CMD_TILT;
        } else {
            MirrorCmdMsg.Cmd    = SVC_VIEW_CTRL_PREV_CMD_TILT;
        }

        SvcViewCtrl_Exec(MirrorCmdMsg);
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Tilt() err. SvcResCfg_GetVinIDOfFovIdx() failed %d", RetVal, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Rotate
 *
 *  @Description:: Emr operation rotation, rotate fov.
 *
 *  @Input      ::
 *                 Value: The operation value
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Rotate(INT32 Value)
{
    UINT32 RetVal;
    UINT32 VinID;
    SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

    RetVal = SvcResCfg_GetVinIDOfFovIdx(EmrTaskTargetFov, &VinID);
    if (SVC_OK == RetVal) {
        MirrorCmdMsg.Value  = (INT32) (EmrTaskUnit * Value);
        MirrorCmdMsg.VinID  = (UINT8) VinID;
        MirrorCmdMsg.FovIdx = (UINT8) EmrTaskTargetFov;
        MirrorCmdMsg.VoutID = (UINT8) EmrTaskTargetVout;
        MirrorCmdMsg.Cmd    = SVC_VIEW_CTRL_WARP_CMD_ROTATE;

        SvcViewCtrl_Exec(MirrorCmdMsg);
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_Rotate() err. SvcResCfg_GetVinIDOfFovIdx() failed %d", RetVal, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_Zoom
 *
 *  @Description:: Emr operation zoom, zoom fov.
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_Zoom(INT32 Value)
{
    UINT32 RetVal;
    UINT32 VinID;
    SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    RetVal = SvcResCfg_GetVinIDOfFovIdx(EmrTaskTargetFov, &VinID);
    if (SVC_OK == RetVal) {
        MirrorCmdMsg.Value  = (INT32) (EmrTaskUnit * Value);
        MirrorCmdMsg.VinID  = (UINT8) VinID;
        MirrorCmdMsg.FovIdx = (UINT8) EmrTaskTargetFov;
        MirrorCmdMsg.VoutID = (UINT8) EmrTaskTargetVout;
        if (pResCfg->FovCfg[EmrTaskTargetFov].PipeCfg.ViewCtrlOperation == SVC_RES_VIEW_CTRL_OP_DZOOM) {
            MirrorCmdMsg.Cmd = SVC_VIEW_CTRL_WARP_CMD_ZOOM;
        } else {
            MirrorCmdMsg.Cmd = SVC_VIEW_CTRL_PREV_CMD_ZOOM;
        }

        SvcViewCtrl_Exec(MirrorCmdMsg);
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_ZoomIn() err. SvcResCfg_GetVinIDOfFovIdx() failed %d", RetVal, 0U);
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_ZoomAnim
 *
 *  @Description:: Emr operation zoom, auto zoom fov with animation.
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_ZoomAnim(SVC_EMR_TASK_WINDOW_s *pSrcWin, SVC_EMR_TASK_WINDOW_s *pDstWin)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(pSrcWin);
    AmbaMisra_TouchUnused(pDstWin);

    RetVal |= AmbaWrap_memcpy(&EmrTaskZoomAnimInfo[EmrTaskTargetFov].SrcWin, pSrcWin, sizeof(SVC_EMR_TASK_WINDOW_s));
    RetVal |= AmbaWrap_memcpy(&EmrTaskZoomAnimInfo[EmrTaskTargetFov].DstWin, pDstWin, sizeof(SVC_EMR_TASK_WINDOW_s));

    if ((SVC_OK == RetVal) && (EmrTaskAnimTask[EmrTaskTargetFov].TaskCreated == 1U)) {
        RetVal = AmbaKAL_TaskTerminate(&EmrTaskAnimTask[EmrTaskTargetFov].TaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "ZoomAnim task terminate err", 0U, 0U);
        }

        RetVal = AmbaKAL_TaskDelete(&EmrTaskAnimTask[EmrTaskTargetFov].TaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "ZoomAnim task delete err", 0U, 0U);
        }
    }

    EmrTaskAnimTask[EmrTaskTargetFov].Priority = 150U;
    EmrTaskAnimTask[EmrTaskTargetFov].EntryArg = EmrTaskTargetFov | (EmrTaskTargetVout << 16U);
    EmrTaskAnimTask[EmrTaskTargetFov].StackSize = SVC_EMR_TASK_ANIM_TASK_STACK_SIZE;

    RetVal = AmbaKAL_TaskCreate(&EmrTaskAnimTask[EmrTaskTargetFov].TaskCtrl,
                                    EmrTaskAnimTask[EmrTaskTargetFov].TaskName,
                                    EmrTaskAnimTask[EmrTaskTargetFov].Priority,
                                    EmrTaskAnimTaskEntry,
                                    &(EmrTaskAnimTask[EmrTaskTargetFov].EntryArg),
                                    EmrTaskAnimTask[EmrTaskTargetFov].Stack,
                                    EmrTaskAnimTask[EmrTaskTargetFov].StackSize,
                                    AMBA_KAL_AUTO_START);

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "ZoomAnim task create err", 0U, 0U);
    }

    EmrTaskAnimTask[EmrTaskTargetFov].TaskCreated = 1U;

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_SetTargetVout
 *
 *  @Description:: Set the emr target VoutID
 *
 *  @Input      ::
 *                 VoutID: The target VoutID
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_SetTargetVout(UINT32 VoutID)
{
    UINT32 RetVal;

    if (VoutID < AMBA_DSP_MAX_VOUT_NUM) {
        EmrTaskTargetVout = VoutID;
        SvcLog_OK(SVC_LOG_EMR_TASK, "SvcEmrTask_SetTargetVout() succeed. Now target VOUT is %d", VoutID, 0U);
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SetTargetVout() err. VoutID (%d) exceeds (%d)", VoutID, AMBA_DSP_MAX_VOUT_NUM);
        RetVal = SVC_NG;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_SetOperation
 *
 *  @Description:: Set the emr operation
 *
 *  @Input      ::
 *                 Operation: 0 is dzoom-cropping, 1 is prev-cropping
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_SetOperation(UINT32 Operation)
{
    UINT32 RetVal = SVC_OK;
    SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (Operation < 2U) {
        pResCfg->FovCfg[EmrTaskTargetFov].PipeCfg.ViewCtrlOperation = Operation;

        if (Operation == 0U) {
            SvcLog_OK(SVC_LOG_EMR_TASK, "Current operation is dzoom-cropping for Fov %d", EmrTaskTargetFov, 0U);
        } else {
            SvcLog_OK(SVC_LOG_EMR_TASK, "Current operation is prev-cropping for Fov %d", EmrTaskTargetFov, 0U);
        }
        RetVal = SVC_OK;
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "Invalid operation, 0 for dzoom-cropping, 1 for prev-cropping", 0U, 0U);
        RetVal = SVC_NG;
    }


    return RetVal;
}

static void* EmrTaskAnimTaskEntry(void* EntryArg)
{
    const UINT32  *pArg;
    UINT32 RetVal;
    UINT32 FovIdx;
    UINT32 VinID;
    UINT32 VoutID;
    SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};
    DOUBLE AnimWindow[SVC_EMR_TASK_ZOOM_STEP][4];
    UINT32 AnimStep = 0U;

    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    FovIdx = (*pArg) & 0x0000FFFFU;          /* [15:0]  for FovIdx */
    VoutID = ((*pArg) & 0xFFFF0000U) >> 16U; /* [31:16] for VoutID */

    EmrTaskZoomAnimInfo[FovIdx].Status = 1U;    /* Processing animation */

    RetVal = AmbaWrap_memset(AnimWindow, 0, sizeof(AnimWindow));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "AnimWindow set failed", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        DOUBLE  DiffX = {0.0}, DiffY = {0.0}, DiffWidth = {0.0}, DiffHeight = {0.0};

        DiffX = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].DstWin.OffsetX - (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetX;
        DiffY = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].DstWin.OffsetY - (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetY;
        DiffWidth  = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].DstWin.Width - (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Width;
        DiffHeight = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].DstWin.Height - (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Height;
        for (AnimStep = 0; AnimStep < SVC_EMR_TASK_ZOOM_STEP; AnimStep++) {
            DOUBLE AnimStepInDouble = (DOUBLE) AnimStep + (DOUBLE) 1U;
            DOUBLE ZoomStepInDouble = (DOUBLE) SVC_EMR_TASK_ZOOM_STEP;
            AnimWindow[AnimStep][0] = ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetX + (DiffX * AnimStepInDouble / ZoomStepInDouble)); /* X_offset */
            AnimWindow[AnimStep][1] = ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetY + (DiffY * AnimStepInDouble / ZoomStepInDouble)); /* Y_offset */
            AnimWindow[AnimStep][2] = ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Width  + (DiffWidth * AnimStepInDouble / ZoomStepInDouble)); /* Width */
            AnimWindow[AnimStep][3] = ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Height + (DiffHeight * AnimStepInDouble / ZoomStepInDouble)); /* Height */
        }
    }

    RetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID);
    if (SVC_OK == RetVal) {
        for (AnimStep = 0; AnimStep < SVC_EMR_TASK_ZOOM_STEP; AnimStep++) {
            MirrorCmdMsg.Value       = (INT32) (0);
            MirrorCmdMsg.Reserved[0] = (INT32) AnimWindow[AnimStep][0]; /* X_offset */
            MirrorCmdMsg.Reserved[1] = (INT32) AnimWindow[AnimStep][1]; /* Y_offset */
            MirrorCmdMsg.Reserved[2] = (INT32) AnimWindow[AnimStep][2]; /* Width */
            MirrorCmdMsg.Reserved[3] = (INT32) AnimWindow[AnimStep][3]; /* Height */
            MirrorCmdMsg.VinID       = (UINT8) VinID;
            MirrorCmdMsg.FovIdx      = (UINT8) FovIdx;
            MirrorCmdMsg.VoutID      = (UINT8) VoutID;
            if (pResCfg->FovCfg[FovIdx].PipeCfg.ViewCtrlOperation == SVC_RES_VIEW_CTRL_OP_DZOOM) {
                MirrorCmdMsg.Cmd     = SVC_VIEW_CTRL_WARP_CMD_SET_WIN;
            } else {
                MirrorCmdMsg.Cmd     = SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN;
            }

            SvcViewCtrl_Exec(MirrorCmdMsg);

            RetVal = AmbaKAL_TaskSleep(17);
            Svc_PrintNg(__func__, __LINE__, RetVal);
        }
    } else {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_ZoomOut() err. SvcResCfg_GetVinIDOfFovIdx() failed %d", RetVal, 0U);
    }

    EmrTaskZoomAnimInfo[FovIdx].Status = 0U;    /* Animation done */

    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcEmrTask_SaveSetting
 *
 *  @Description:: Save the emr setting
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcEmrTask_SaveSetting(void)
{
    UINT32 RetVal;
    UINT32 i, j;
    UINT32 VinIDs[AMBA_DSP_MAX_VIEWZONE_NUM]  = {0U}, VinID = 0U, VinNum = 0U;
    UINT32 FovIdxs[AMBA_DSP_MAX_VIEWZONE_NUM] = {0U}, FovIdx = 0U, FovNum = 0U;
    SVC_VIEW_CTRL_WARP_INFO_s MirrorInfo;

    SVC_USER_PREF_s *pSvcUserPref;

    SvcLog_OK(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() start", 0U, 0U);

    RetVal = SvcUserPref_Get(&pSvcUserPref);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() failed. SvcUserPref_Get() err %d", RetVal, 0U);
    } else {
        RetVal = SvcResCfg_GetVinIDs(VinIDs, &VinNum);
        if (SVC_OK == RetVal) {
            for (i = 0; i < VinNum; i++) {
                VinID = VinIDs[i];
                RetVal = SvcResCfg_GetFovIdxsInVinID(VinID, FovIdxs, &FovNum);
                if (SVC_OK == RetVal) {
                    for (j = 0; j < FovNum; j++) {
                        FovIdx = FovIdxs[j];
                        RetVal = SvcViewCtrl_GetViewCtrlInfo(FovIdx, &MirrorInfo);
                        if (RetVal == SVC_OK) {
                            pSvcUserPref->EmrInfo[FovIdx].ShiftX = MirrorInfo.Position.ShiftX;
                            pSvcUserPref->EmrInfo[FovIdx].ShiftY = MirrorInfo.Position.ShiftY;
                            pSvcUserPref->EmrInfo[FovIdx].ZoomX = MirrorInfo.Position.ZoomX;
                            pSvcUserPref->EmrInfo[FovIdx].ZoomY = MirrorInfo.Position.ZoomY;
                            pSvcUserPref->EmrInfo[FovIdx].Theta = MirrorInfo.Rotation.Theta;
                        } else {
                            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() failed. SvcViewCtrl_GetEmrInfo() failed %d", RetVal, 0U);
                            break;
                        }
                    }
                } else {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() err. SvcResCfg_GetFovIdxsInVinID() failed %d", RetVal, 0U);
                    break;
                }
            }
            if (SVC_OK == RetVal) {
                UINT32  PrefBufSize;
                ULONG   PrefBufAddr;

                SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() failed. SvcPref_Save() err with %d", RetVal, 0U);
                }
            }
        } else {
            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() err. SvcResCfg_GetVinIDs() failed %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() done", 0U, 0U);
    } else {
        SvcLog_OK(SVC_LOG_EMR_TASK, "SvcEmrTask_SaveSetting() failed", 0U, 0U);
    }

    return RetVal;
}

static void EmrTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;
    SVC_APP_STAT_MENU_s *pStatus = NULL;

    AmbaMisra_TouchUnused(pInfo);   /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus); /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if ((StatIdx == SVC_APP_STAT_MENU) && (pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR)) {
        if ((pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_PAN)  ||
            (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_TILT) ||
            (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_ROTATE)) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                if ((EmrTaskGuiCtrl & SVC_EMR_TASK_GUI_FOV_SHOW) == 0U) {
                    UINT32 OsdWidth, OsdHeight;

                    EmrTaskFunc = pStatus->FuncIdx;
                    /* To show GUI for choosing Fov */
                    EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_FOV_SHOW;
                    EmrTaskOsdShowTargetFovOnVout = pStatus->Vout;

                    /* To reset CheckBox information */
                    EmrTaskTargetFovCheckBox.CurIdx = EmrTaskTargetFov;
                    EmrTaskTargetFovCheckBox.CheckBits = ((UINT32) 1U << EmrTaskTargetFov);

                    RetVal = SvcOsd_GetOsdBufSize(EmrTaskOsdShowTargetFovOnVout, &OsdWidth, &OsdHeight);
                    if (SVC_OK == RetVal) {
                        EmrTaskTargetFovCanvas.StartX = (OsdWidth - (SVC_EMR_TASK_FOV_GUI_W)) >> (UINT32) 1U;
                        EmrTaskTargetFovCanvas.StartY = (OsdHeight - (SVC_EMR_TASK_FOV_GUI_H)) >> (UINT32) 1U;
                        EmrTaskTargetFovCanvas.Width  = SVC_EMR_TASK_FOV_GUI_W;
                        EmrTaskTargetFovCanvas.Height = SVC_EMR_TASK_FOV_GUI_H;
                    }

                    SvcGui_Register(pStatus->Vout, 17U, "EmrMenu", EmrTask_OsdShowFovEntry, EmrTask_OsdDrawUpdate);
                    EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_FOV_UPDATE;
                    RetVal = SvcButtonCtrl_Request(&EmrTaskButtonCtrl_ID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "EmrTask_MenuStatusCallback err. Button request failed", 0U, 0U);
                    }
                } else {
                    EmrTaskGuiCtrl &= ~(SVC_EMR_TASK_GUI_FOV_SHOW);
                    SvcGui_Unregister(pStatus->Vout, 17U);
                    RetVal = SvcButtonCtrl_Release(&EmrTaskButtonCtrl_ID);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "EmrTask_MenuStatusCallback err. Button release failed", 0U, 0U);
                    }
                }
            }
        } else if (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_FORMAT) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                RetVal = SvcEmrTask_SaveSetting();
            }
        } else if (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_EMR2_0) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
#if defined(CONFIG_BUILD_AMBA_ADAS)
                if (EmrTaskEnableEmr_2_0 == 0U) {
                    UINT32          PrefBufSize;
                    ULONG           PrefBufAddr;
                    SVC_USER_PREF_s *pSvcUserPref;

                    /* Reboot and enable EMR 2.0 */
                    RetVal = SvcUserPref_Get(&pSvcUserPref);
                    if (SVC_OK == RetVal) {
                        pSvcUserPref->Emr_1_0 = pSvcUserPref->FormatId;
                        pSvcUserPref->FormatId = SVC_EMR_TASK_2_0_PREV_FORMAT_ID;   /* EMR 2.0 Mode */
                        pSvcUserPref->Emr_2_0 = pSvcUserPref->FormatId;
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                        RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
                        if (SVC_OK == RetVal) {
                            if (SVC_OK != AmbaSYS_Reboot()) {
                                SvcLog_NG(SVC_LOG_EMR_TASK, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcUserPref_Save() failed", 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcUserPref_Get() failed", 0U, 0U);
                    }
                } else {
                    /* Reboot and back to EMR 1.0 */
                    UINT32          PrefBufSize;
                    ULONG           PrefBufAddr;
                    SVC_USER_PREF_s *pSvcUserPref;

                    RetVal = SvcUserPref_Get(&pSvcUserPref);
                    if (SVC_OK == RetVal) {
                        pSvcUserPref->FormatId = pSvcUserPref->Emr_1_0;   /* EMR 1.0 Mode */
                        SvcPref_GetPrefBuf(&PrefBufAddr, &PrefBufSize);

                        RetVal = SvcPref_Save(PrefBufAddr, PrefBufSize);
                        if (SVC_OK == RetVal) {
                            if (SVC_OK != AmbaSYS_Reboot()) {
                                SvcLog_NG(SVC_LOG_EMR_TASK, "AmbaSYS_Reboot() failed!!", 0U, 0U);
                            }
                        } else {
                            SvcLog_NG(SVC_LOG_EMR_TASK, "SvcUserPref_Save() failed", 0U, 0U);
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "SvcUserPref_Get() failed", 0U, 0U);
                    }
                }
#endif
            }
        } else {
            /* Do nothing */
        }
    }

    AmbaMisra_TouchUnused(&RetVal); /* Misra-c fixed */
}

static void EmrTask_CanBusDataListener(UINT32 EntryArg)
{
#if defined(CONFIG_BUILD_AMBA_ADAS)
    AMBA_SR_CANBUS_RAW_DATA_s CanBusRawData = {0};
    SVC_EMR_TASK_CAN_BUS_DATA_s CanBusData = {0};

    UINT32 RetVal = SVC_OK;

    SvcCan_GetRawData(&CanBusRawData);

    CanBusData.GearStatus = CanBusRawData.CANBusGearStatus;
    CanBusData.DirLightStatus = CanBusRawData.CANBusTurnLightStatus;

    RetVal = AmbaKAL_MsgQueueSend(&EmrTaskCanBusMsg.QueueId, &CanBusData, AMBA_KAL_NO_WAIT);
    Svc_PrintNg(__func__, __LINE__, RetVal);
#endif

    AmbaMisra_TouchUnused(&EntryArg);
}

static void* EmrTask_CanBusEntry(void* EntryArg)
{
#if defined(CONFIG_BUILD_AMBA_ADAS)
    const UINT32  *pArg;
    UINT32 RetVal = SVC_OK;

    SVC_EMR_TASK_CAN_BUS_DATA_s CanBusData = {0U};

    UINT32 GearStatusPrev = 0U;
    UINT32 DirLightStatusPrev = 0U;

    SVC_VIEW_CTRL_WINDOW_s WindowInfo = {0U};

    SVC_EMR_TASK_WINDOW_s CanBusZoomWindow[AMBA_DSP_MAX_VIEWZONE_NUM][3U];  /* 0 is for restore, 1 is for DirLight L/R, 2 is for Gear R */

    SVC_EMR_TASK_WINDOW_s SrcWindow, DstWindow;
    UINT32 OriginTargetFov = 0U;
    UINT32 OriginTargetVout = 0U;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while ((*pArg) == 0U) {
        RetVal = AmbaKAL_MsgQueueReceive(&EmrTaskCanBusMsg.QueueId, &CanBusData, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMR_TASK, "CanBusEntry Msg Receive err %d.", RetVal, 0U);
        }

        if ((EmrTaskZoomAnimInfo[1U].Status != 0U) || (EmrTaskZoomAnimInfo[2U].Status != 0U)) {
            continue;
        }

        /* Only receive when EMR 2.0 */
        if (EmrTaskEnableEmr_2_0 == 0U) {
            continue;
        }

        EmrTaskCanBusData.GearStatus = CanBusData.GearStatus;
        EmrTaskCanBusData.DirLightStatus = CanBusData.DirLightStatus;

        OriginTargetFov = EmrTaskTargetFov;
        OriginTargetVout = EmrTaskTargetVout;

        if (CanBusData.GearStatus == AMBA_SR_CANBUS_GEAR_R) {
            if (GearStatusPrev != AMBA_SR_CANBUS_GEAR_R) {
                /* From other gear P, N, D to R */
                /* Zoom-out */
                /* Left FOV1 */
                if (CanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_L) {
                    /* If there's TURN_LIGHT_L, then change view from stage 1 -> 2 */
                    RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[1U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                    Svc_PrintNg(__func__, __LINE__, RetVal);
                } else {
                    /* If there's NO TURN_LIGHT_L, then change view from stage 0 -> 2 */
                    RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[1U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                    Svc_PrintNg(__func__, __LINE__, RetVal);
                }
                RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[1U][2U], sizeof(SVC_EMR_TASK_WINDOW_s));
                Svc_PrintNg(__func__, __LINE__, RetVal);
                RetVal |= SvcEmrTask_SetTarget(1U);
                RetVal |= SvcEmrTask_SetTargetVout(1U);
                RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);
                if (SVC_OK == RetVal) {
                    /* Right FOV2 */
                    if (CanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_R) {
                        /* If there's TURN_LIGHT_R, then change view from stage 1 -> 2 */
                        RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[2U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                    } else {
                        /* If there's NO TURN_LIGHT_R, then change view from stage 0 -> 2 */
                        RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[2U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                    }
                }
                RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[2U][2U], sizeof(SVC_EMR_TASK_WINDOW_s));
                Svc_PrintNg(__func__, __LINE__, RetVal);
                RetVal |= SvcEmrTask_SetTarget(2U);
                RetVal |= SvcEmrTask_SetTargetVout(1U);
                RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_UPDATE;
            } else {
                /* Gear is R and not change */
                /* Do nothing */
            }
        } else {
            if (GearStatusPrev == AMBA_SR_CANBUS_GEAR_R) {
                /* From gear R to other (P, N, D) */
                /* Zoom-in */
                /* Left FOV1 */
                RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[1U][2U], sizeof(SVC_EMR_TASK_WINDOW_s));
                Svc_PrintNg(__func__, __LINE__, RetVal);
                if (CanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_L) {
                    /* If there's TURN_LIGHT_L, then change view from stage 2 -> 1 */
                    RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[1U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                    Svc_PrintNg(__func__, __LINE__, RetVal);
                } else {
                    /* If there's NO TURN_LIGHT_L, then change view from stage 2 -> 0 */
                    RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[1U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                    Svc_PrintNg(__func__, __LINE__, RetVal);
                }
                RetVal |= SvcEmrTask_SetTarget(1U);
                RetVal |= SvcEmrTask_SetTargetVout(1U);
                RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                /* Right FOV2 */
                RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[2U][2U], sizeof(SVC_EMR_TASK_WINDOW_s));
                Svc_PrintNg(__func__, __LINE__, RetVal);
                if (CanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_R) {
                    /* If there's TURN_LIGHT_R, then change view from stage 2 -> 1 */
                    RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[2U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                    Svc_PrintNg(__func__, __LINE__, RetVal);
                } else {
                    /* If there's NO TURN_LIGHT_R, then change view from stage 2 -> 0 */
                    RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[2U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                    Svc_PrintNg(__func__, __LINE__, RetVal);
                }
                RetVal |= SvcEmrTask_SetTarget(2U);
                RetVal |= SvcEmrTask_SetTargetVout(1U);
                RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_UPDATE;
            } else {
                /* Gear may not change or other case */
                /* Zoom-in */
                if (CanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_L) {
                    /* Left FOV1 */
                    if (DirLightStatusPrev != AMBA_SR_CANBUS_TURN_LIGHT_L) {
                        RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[1U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[1U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal |= SvcEmrTask_SetTarget(1U);
                        RetVal |= SvcEmrTask_SetTargetVout(1U);
                        RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                        EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_UPDATE;
                    }
                } else if (CanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_R) {
                    /* Right FOV2 */
                    if (DirLightStatusPrev != AMBA_SR_CANBUS_TURN_LIGHT_R) {
                        RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[2U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[2U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal |= SvcEmrTask_SetTarget(2U);
                        RetVal |= SvcEmrTask_SetTargetVout(1U);
                        RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                        EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_UPDATE;
                    }
                } else {
                    if (DirLightStatusPrev == AMBA_SR_CANBUS_TURN_LIGHT_L) {
                        RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[1U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[1U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal |= SvcEmrTask_SetTarget(1U);
                        RetVal |= SvcEmrTask_SetTargetVout(1U);
                        RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                        EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_UPDATE;
                    } else if (DirLightStatusPrev == AMBA_SR_CANBUS_TURN_LIGHT_R) {
                        RetVal = AmbaWrap_memcpy(&SrcWindow, &CanBusZoomWindow[2U][1U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal = AmbaWrap_memcpy(&DstWindow, &CanBusZoomWindow[2U][0U], sizeof(SVC_EMR_TASK_WINDOW_s));
                        Svc_PrintNg(__func__, __LINE__, RetVal);
                        RetVal |= SvcEmrTask_SetTarget(2U);
                        RetVal |= SvcEmrTask_SetTargetVout(1U);
                        RetVal |= SvcEmrTask_ZoomAnim(&SrcWindow, &DstWindow);

                        EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_EMR2_0_UPDATE;
                    } else {
                        if (pCfg->FovCfg[1U].PipeCfg.ViewCtrlOperation == SVC_RES_VIEW_CTRL_OP_DZOOM) {
                            /* Because user may change view by pan/tilt or other operation, update ZoomWindow everytime when receive can bus data */
                            RetVal = SvcViewCtrl_GetViewWinInfo(1U, &WindowInfo);
                            Svc_PrintNg(__func__, __LINE__, RetVal);
                            CanBusZoomWindow[1U][0U].OffsetX = WindowInfo.OffsetX;
                            CanBusZoomWindow[1U][0U].OffsetY = WindowInfo.OffsetY;
                            CanBusZoomWindow[1U][0U].Width   = WindowInfo.Width;
                            CanBusZoomWindow[1U][0U].Height  = WindowInfo.Height;

                            CanBusZoomWindow[1U][2U].OffsetX = WindowInfo.OffsetX;  /* Fix left-top point */
                            CanBusZoomWindow[1U][2U].OffsetY = WindowInfo.OffsetY;  /* Fix left-top point */
                            CanBusZoomWindow[1U][2U].Width   = pCfg->FovCfg[1U].RawWin.Width - WindowInfo.OffsetX;
                            CanBusZoomWindow[1U][2U].Height  = pCfg->FovCfg[1U].RawWin.Height - WindowInfo.OffsetY;

                            CanBusZoomWindow[1U][1U].OffsetX = WindowInfo.OffsetX;  /* Fix left-top point */
                            CanBusZoomWindow[1U][1U].OffsetY = WindowInfo.OffsetY;  /* Fix left-top point */
                            CanBusZoomWindow[1U][1U].Width   = (CanBusZoomWindow[1U][2U].Width) * 9U / 10U;     /* 0.9 Width of Gear-R view for Left */
                            CanBusZoomWindow[1U][1U].Height  = (CanBusZoomWindow[1U][2U].Height) * 9U / 10U;    /* 0.9 Height of Gear-R view for Left */

                            RetVal = SvcViewCtrl_GetViewWinInfo(2U, &WindowInfo);
                            Svc_PrintNg(__func__, __LINE__, RetVal);
                            CanBusZoomWindow[2U][0U].OffsetX = WindowInfo.OffsetX;
                            CanBusZoomWindow[2U][0U].OffsetY = WindowInfo.OffsetY;
                            CanBusZoomWindow[2U][0U].Width   = WindowInfo.Width;
                            CanBusZoomWindow[2U][0U].Height  = WindowInfo.Height;

                            CanBusZoomWindow[2U][2U].OffsetX = 0U;                  /* Fix right-top point */
                            CanBusZoomWindow[2U][2U].OffsetY = WindowInfo.OffsetY;  /* Fix right-top point */
                            CanBusZoomWindow[2U][2U].Width   = WindowInfo.Width + WindowInfo.OffsetX;
                            CanBusZoomWindow[2U][2U].Height  = pCfg->FovCfg[2U].RawWin.Height - WindowInfo.OffsetY;

                            CanBusZoomWindow[2U][1U].OffsetX = CanBusZoomWindow[2U][2U].Width * 1U / 10U;
                            CanBusZoomWindow[2U][1U].OffsetY = WindowInfo.OffsetY;
                            CanBusZoomWindow[2U][1U].Width   = (CanBusZoomWindow[2U][2U].Width) * 9U / 10U;     /* 0.9 Width of Gear-R view for Left */
                            CanBusZoomWindow[2U][1U].Height  = (CanBusZoomWindow[2U][2U].Height) * 9U / 10U;    /* 0.9 Height of Gear-R view for Left */
                        } else {
                            RetVal = SvcViewCtrl_GetPrevWinInfo(1U /* VoutID */, 1U /* FovIdx */, &WindowInfo);
                            Svc_PrintNg(__func__, __LINE__, RetVal);
                            CanBusZoomWindow[1U][0U].OffsetX = WindowInfo.OffsetX;
                            CanBusZoomWindow[1U][0U].OffsetY = WindowInfo.OffsetY;
                            CanBusZoomWindow[1U][0U].Width   = WindowInfo.Width;
                            CanBusZoomWindow[1U][0U].Height  = WindowInfo.Height;

                            CanBusZoomWindow[1U][2U].OffsetX = WindowInfo.OffsetX;  /* Fix left-top point */
                            CanBusZoomWindow[1U][2U].OffsetY = WindowInfo.OffsetY;  /* Fix left-top point */
                            CanBusZoomWindow[1U][2U].Width   = pCfg->FovCfg[1U].MainWin.Width - WindowInfo.OffsetX;
                            CanBusZoomWindow[1U][2U].Height  = pCfg->FovCfg[1U].MainWin.Height - WindowInfo.OffsetY;

                            CanBusZoomWindow[1U][1U].OffsetX = WindowInfo.OffsetX;  /* Fix left-top point */
                            CanBusZoomWindow[1U][1U].OffsetY = WindowInfo.OffsetY;  /* Fix left-top point */
                            CanBusZoomWindow[1U][1U].Width   = (CanBusZoomWindow[1U][2U].Width) * 9U / 10U;     /* 0.9 Width of Gear-R view for Left */
                            CanBusZoomWindow[1U][1U].Height  = (CanBusZoomWindow[1U][2U].Height) * 9U / 10U;    /* 0.9 Height of Gear-R view for Left */

                            RetVal = SvcViewCtrl_GetPrevWinInfo(1U /* VoutID */, 2U /* FovIdx */, &WindowInfo);
                            Svc_PrintNg(__func__, __LINE__, RetVal);
                            CanBusZoomWindow[2U][0U].OffsetX = WindowInfo.OffsetX;
                            CanBusZoomWindow[2U][0U].OffsetY = WindowInfo.OffsetY;
                            CanBusZoomWindow[2U][0U].Width   = WindowInfo.Width;
                            CanBusZoomWindow[2U][0U].Height  = WindowInfo.Height;

                            CanBusZoomWindow[2U][2U].OffsetX = 0U;                  /* Fix right-top point */
                            CanBusZoomWindow[2U][2U].OffsetY = WindowInfo.OffsetY;  /* Fix right-top point */
                            CanBusZoomWindow[2U][2U].Width   = WindowInfo.Width + WindowInfo.OffsetX;
                            CanBusZoomWindow[2U][2U].Height  = pCfg->FovCfg[2U].MainWin.Height - WindowInfo.OffsetY;

                            CanBusZoomWindow[2U][1U].OffsetX = CanBusZoomWindow[2U][2U].Width * 1U / 10U;
                            CanBusZoomWindow[2U][1U].OffsetY = WindowInfo.OffsetY;
                            CanBusZoomWindow[2U][1U].Width   = (CanBusZoomWindow[2U][2U].Width) * 9U / 10U;     /* 0.9 Width of Gear-R view for Left */
                            CanBusZoomWindow[2U][1U].Height  = (CanBusZoomWindow[2U][2U].Height) * 9U / 10U;    /* 0.9 Height of Gear-R view for Left */
                        }
                    }
                }
            }
        }
        GearStatusPrev = CanBusData.GearStatus;
        DirLightStatusPrev = CanBusData.DirLightStatus;
        /* Restore the Target Fov */
        if (OriginTargetFov != EmrTaskTargetFov) {
            RetVal = SvcEmrTask_SetTarget(OriginTargetFov);
            Svc_PrintNg(__func__, __LINE__, RetVal);
        }
        if (OriginTargetVout != EmrTaskTargetVout) {
            RetVal = SvcEmrTask_SetTargetVout(OriginTargetVout);
            Svc_PrintNg(__func__, __LINE__, RetVal);
        }
    }
#else
    AmbaMisra_TouchUnused(EntryArg);
#endif

    return NULL;
}

static void EmrTask_OsdDrawVout0(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;
    UINT32 i;
    UINT32 OsdWidth, OsdHeight;
    UINT32 OriginAreaRect = 0U;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = &pCfg->DispStrm[0U];

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    AmbaMisra_TouchUnused(&OriginAreaRect);
    RetVal = SvcOsd_GetOsdBufSize(0U, &OsdWidth, &OsdHeight);

    for  (i = 0; i < pDispStrm->StrmCfg.NumChan; i++) {
        UINT32 FovIdx = pDispStrm->StrmCfg.ChanCfg[i].FovId;

        if (OriginAreaRect != 0U) {
            DOUBLE FactorYuvX = (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Width / (DOUBLE) pDispStrm->StrmCfg.MaxWin.Width;
            DOUBLE FactorYuvY = (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Height / (DOUBLE) pDispStrm->StrmCfg.MaxWin.Height;
            DOUBLE FactorOsdX = (DOUBLE) OsdWidth / (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Width;
            DOUBLE FactorOsdY = (DOUBLE) OsdHeight / (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Height;
            DOUBLE CornerX[4];   /* 0: Left-top, 1: Right-top, 2: Left-bot, 3: Right-bot */
            DOUBLE CornerY[4];   /* 0: Left-top, 1: Right-top, 2: Left-bot, 3: Right-bot */

            CornerX[0] = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetX * FactorYuvX * FactorOsdX;
            CornerY[0] = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetY * FactorYuvY * FactorOsdY;
            CornerX[1] = CornerX[0] + ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Width * FactorYuvX * FactorOsdX);
            CornerY[1] = CornerY[0];
            CornerX[2] = CornerX[0];
            CornerY[2] = CornerY[0] + ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Height * FactorYuvY * FactorOsdY);
            CornerX[3] = CornerX[1];
            CornerY[3] = CornerY[2];

            if (pDispStrm->VideoRotateFlip != AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                /* Draw Left-top corner */
                RetVal |= SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[0], (UINT32) CornerY[0], (UINT32) CornerX[0] + 40U , (UINT32) CornerY[0] + 5U, 232U);
                RetVal |= SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[0], (UINT32) CornerY[0], (UINT32) CornerX[0] + 5U , (UINT32) CornerY[0] + 40U, 232U);

                /* Draw Right-top corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[1] - 40U, (UINT32) CornerY[1], (UINT32) CornerX[1] , (UINT32) CornerY[1] + 5U, 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[1] - 5U, (UINT32) CornerY[1], (UINT32) CornerX[1] , (UINT32) CornerY[1] + 40U, 232U);

                /* Draw Left-bot corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[2], (UINT32) CornerY[2] - 5U, (UINT32) CornerX[2] + 40U , (UINT32) CornerY[2], 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[2], (UINT32) CornerY[2] - 40U, (UINT32) CornerX[2] + 5U , (UINT32) CornerY[2], 232U);

                /* Draw Right-bot corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[3] - 40U, (UINT32) CornerY[3] - 5U, (UINT32) CornerX[3] , (UINT32) CornerY[3], 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[3] - 5U, (UINT32) CornerY[3] - 40U, (UINT32) CornerX[3] , (UINT32) CornerY[3], 232U);
            } else {
                UINT32 j;
                /* New coordinate, 1: Left-top, 0: Right-top, 3: Left-bot, 2: Right-bot */
                for (j = 0U; j < 4U; j++) {
                    CornerX[j] = (CornerX[j] * (-1.0)) + (DOUBLE) OsdWidth;
                    // CornerY[j] = CornerY[j];
                }

                /* Draw Left-top corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[1], (UINT32) CornerY[1], (UINT32) CornerX[1] + 40U , (UINT32) CornerY[1] + 5U, 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[1], (UINT32) CornerY[1], (UINT32) CornerX[1] + 5U , (UINT32) CornerY[1] + 40U, 232U);

                /* Draw Right-top corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[0] - 40U, (UINT32) CornerY[0], (UINT32) CornerX[0] , (UINT32) CornerY[0] + 5U, 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[0] - 5U, (UINT32) CornerY[0], (UINT32) CornerX[0] , (UINT32) CornerY[0] + 40U, 232U);

                /* Draw Left-bot corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[3], (UINT32) CornerY[3] - 5U, (UINT32) CornerX[3] + 40U , (UINT32) CornerY[3], 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[3], (UINT32) CornerY[3] - 40U, (UINT32) CornerX[3] + 5U , (UINT32) CornerY[3], 232U);

                /* Draw Right-bot corner */
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[2] - 40U, (UINT32) CornerY[2] - 5U, (UINT32) CornerX[2] , (UINT32) CornerY[2], 232U);
                RetVal = SvcOsd_DrawSolidRect(0U, (UINT32) CornerX[2] - 5U, (UINT32) CornerY[2] - 40U, (UINT32) CornerX[2] , (UINT32) CornerY[2], 232U);
            }

            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_EMR_TASK, "SvcOsd Draw on VOUT_0 err", 0U, 0U);
            }
        }
    }
}

#if defined(AMBA_DSP_MAX_VOUT_NUM) && (AMBA_DSP_MAX_VOUT_NUM > 1U)
static void EmrTask_OsdDrawVout1(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal;
    UINT32 i;
    UINT32 OsdWidth, OsdHeight;
    UINT32 OriginAreaRect = 0U;

    const SVC_RES_CFG_s *pCfg = SvcResCfg_Get();

    const SVC_DISP_STRM_s *pDispStrm = &pCfg->DispStrm[1U];

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    AmbaMisra_TouchUnused(&OriginAreaRect);
    RetVal = SvcOsd_GetOsdBufSize(1U, &OsdWidth, &OsdHeight);

#if defined(CONFIG_BUILD_AMBA_ADAS)
    if (EmrTaskCanBusData.GearStatus == AMBA_SR_CANBUS_GEAR_R) {
        RetVal = SvcOsd_DrawBmp(1U, 0U, OsdHeight - SVC_EMR_TASK_ICON_HEIGHT, SVC_EMR_TASK_ICON_WIDTH, OsdHeight, EmrTaskGearRLIcon);
        RetVal = SvcOsd_DrawBmp(1U, OsdWidth - SVC_EMR_TASK_ICON_WIDTH, OsdHeight - SVC_EMR_TASK_ICON_HEIGHT, OsdWidth, OsdHeight, EmrTaskGearRRIcon);
    } else {
        if (EmrTaskCanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_L) {
            RetVal = SvcOsd_DrawBmp(1U, 0U, OsdHeight - SVC_EMR_TASK_ICON_HEIGHT, SVC_EMR_TASK_ICON_WIDTH, OsdHeight, EmrTaskDirLightLIcon);
        } else if (EmrTaskCanBusData.DirLightStatus == AMBA_SR_CANBUS_TURN_LIGHT_R) {
            RetVal = SvcOsd_DrawBmp(1U, OsdWidth - SVC_EMR_TASK_ICON_WIDTH, OsdHeight - SVC_EMR_TASK_ICON_HEIGHT, OsdWidth, OsdHeight, EmrTaskDirLightRIcon);
        } else {
            /* Do nothing */
        }
    }
#endif

    for  (i = 0; i < pDispStrm->StrmCfg.NumChan; i++) {
        UINT32 FovIdx = pDispStrm->StrmCfg.ChanCfg[i].FovId;

        if (OriginAreaRect != 0U) {
            DOUBLE FactorYuvX = (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Width / (DOUBLE) pDispStrm->StrmCfg.MaxWin.Width;
            DOUBLE FactorYuvY = (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Height / (DOUBLE) pDispStrm->StrmCfg.MaxWin.Height;
            DOUBLE FactorOsdX = (DOUBLE) OsdWidth / (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Width;
            DOUBLE FactorOsdY = (DOUBLE) OsdHeight / (DOUBLE) pDispStrm->StrmCfg.ChanCfg[FovIdx].DstWin.Height;
            DOUBLE CornerX[4];   /* 0: Left-top, 1: Right-top, 2: Left-bot, 3: Right-bot */
            DOUBLE CornerY[4];   /* 0: Left-top, 1: Right-top, 2: Left-bot, 3: Right-bot */

            CornerX[0] = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetX * FactorYuvX * FactorOsdX;
            CornerY[0] = (DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.OffsetY * FactorYuvY * FactorOsdY;
            CornerX[1] = CornerX[0] + ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Width * FactorYuvX * FactorOsdX);
            CornerY[1] = CornerY[0];
            CornerX[2] = CornerX[0];
            CornerY[2] = CornerY[0] + ((DOUBLE) EmrTaskZoomAnimInfo[FovIdx].SrcWin.Height * FactorYuvY * FactorOsdY);
            CornerX[3] = CornerX[1];
            CornerY[3] = CornerY[2];

            if (pDispStrm->VideoRotateFlip != AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                /* Draw Left-top corner */
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[0], (UINT32) CornerY[0], (UINT32) CornerX[0] + 40U , (UINT32) CornerY[0] + 5U, 232U);
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[0], (UINT32) CornerY[0], (UINT32) CornerX[0] + 5U , (UINT32) CornerY[0] + 40U, 232U);

                /* Draw Right-top corner */
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[1] - 40U, (UINT32) CornerY[1], (UINT32) CornerX[1] , (UINT32) CornerY[1] + 5U, 232U);
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[1] - 5U, (UINT32) CornerY[1], (UINT32) CornerX[1] , (UINT32) CornerY[1] + 40U, 232U);

                /* Draw Left-bot corner */
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[2], (UINT32) CornerY[2] - 5U, (UINT32) CornerX[2] + 40U , (UINT32) CornerY[2], 232U);
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[2], (UINT32) CornerY[2] - 40U, (UINT32) CornerX[2] + 5U , (UINT32) CornerY[2], 232U);

                /* Draw Right-bot corner */
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[3] - 40U, (UINT32) CornerY[3] - 5U, (UINT32) CornerX[3] , (UINT32) CornerY[3], 232U);
                RetVal |= SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[3] - 5U, (UINT32) CornerY[3] - 40U, (UINT32) CornerX[3] , (UINT32) CornerY[3], 232U);
            } else {
                UINT32 j;
                /* New coordinate, 1: Left-top, 0: Right-top, 3: Left-bot, 2: Right-bot */
                for (j = 0U; j < 4U; j++) {
                    CornerX[j] = (CornerX[j] * (-1.0)) + (DOUBLE) OsdWidth;
                    // CornerY[j] = CornerY[j];
                }

                /* Draw Left-top corner */
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[1], (UINT32) CornerY[1], (UINT32) CornerX[1] + 40U , (UINT32) CornerY[1] + 5U, 232U);
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[1], (UINT32) CornerY[1], (UINT32) CornerX[1] + 5U , (UINT32) CornerY[1] + 40U, 232U);

                /* Draw Right-top corner */
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[0] - 40U, (UINT32) CornerY[0], (UINT32) CornerX[0] , (UINT32) CornerY[0] + 5U, 232U);
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[0] - 5U, (UINT32) CornerY[0], (UINT32) CornerX[0] , (UINT32) CornerY[0] + 40U, 232U);

                /* Draw Left-bot corner */
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[3], (UINT32) CornerY[3] - 5U, (UINT32) CornerX[3] + 40U , (UINT32) CornerY[3], 232U);
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[3], (UINT32) CornerY[3] - 40U, (UINT32) CornerX[3] + 5U , (UINT32) CornerY[3], 232U);

                /* Draw Right-bot corner */
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[2] - 40U, (UINT32) CornerY[2] - 5U, (UINT32) CornerX[2] , (UINT32) CornerY[2], 232U);
                RetVal = SvcOsd_DrawSolidRect(1U, (UINT32) CornerX[2] - 5U, (UINT32) CornerY[2] - 40U, (UINT32) CornerX[2] , (UINT32) CornerY[2], 232U);
            }

            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_EMR_TASK, "SvcOsd Draw on VOUT_0 err", 0U, 0U);
            }
        }
    }
}
#endif

static void EmrTask_OsdShowFovEntry(UINT32 VoutIdx, UINT32 Level)
{
    SVC_GUI_CANVAS_s CheckBoxCanvas = { .StartX = EmrTaskTargetFovCanvas.StartX, .StartY = EmrTaskTargetFovCanvas.StartY, .Width = SVC_EMR_TASK_FOV_GUI_W, .Height = SVC_EMR_TASK_FOV_GUI_CHECKBOX_H };
    SVC_GUI_CANVAS_s TextViewCanvas = { .StartX = EmrTaskTargetFovCanvas.StartX, .StartY = EmrTaskTargetFovCanvas.StartY + SVC_EMR_TASK_FOV_GUI_CHECKBOX_H, .Width = SVC_EMR_TASK_FOV_GUI_W, .Height = SVC_EMR_TASK_FOV_GUI_TEXTBOX1_H };
    SVC_GUI_TEXTVIEW_s TextView = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "1/2: Operation. 3: Move 4: Leave",
    };
    SVC_GUI_CANVAS_s TextViewCanvas1 = { .StartX = EmrTaskTargetFovCanvas.StartX, .StartY = EmrTaskTargetFovCanvas.StartY + SVC_EMR_TASK_FOV_GUI_CHECKBOX_H + SVC_EMR_TASK_FOV_GUI_TEXTBOX1_H, .Width = SVC_EMR_TASK_FOV_GUI_W, .Height = SVC_EMR_TASK_FOV_GUI_TEXTBOX2_H };
    SVC_GUI_TEXTVIEW_s TextView1 = {
        .BackgroundColor = 0x80202040U,
        .FontColor = 0xffffffffU,
        .FontSize = 1U,
        .LineGap = 1U,
        .pText = "-/+: Operation. !: Move @: Leave",
    };

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (((EmrTaskGuiCtrl & SVC_EMR_TASK_GUI_FOV_SHOW) > 0U) && (EmrTaskOsdShowTargetFovOnVout == VoutIdx)) {
        SvcGuiCheckBox_Draw(EmrTaskOsdShowTargetFovOnVout, &CheckBoxCanvas,  &EmrTaskTargetFovCheckBox);
        SvcGuiTextView_Draw(EmrTaskOsdShowTargetFovOnVout, &TextViewCanvas,  &TextView);
        SvcGuiTextView_Draw(EmrTaskOsdShowTargetFovOnVout, &TextViewCanvas1, &TextView1);
    }
}

static void EmrTask_OsdDrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    *pUpdate = 0U;
    if (((EmrTaskGuiCtrl & SVC_EMR_TASK_GUI_FOV_UPDATE) > 0U) && (EmrTaskOsdShowTargetFovOnVout == VoutIdx)) {
        EmrTaskGuiCtrl &= ~(SVC_EMR_TASK_GUI_FOV_UPDATE);
        *pUpdate = 1U;
    }
    if (((EmrTaskGuiCtrl & SVC_EMR_TASK_GUI_EMR2_0_UPDATE) > 0U) &&  (EmrTaskOsdShowTargetFovOnVout == VoutIdx)) {
        EmrTaskGuiCtrl &= ~(SVC_EMR_TASK_GUI_EMR2_0_UPDATE);
        *pUpdate = 1U;
    }
}

static UINT32 EmrTask_Minus(void)
{
    UINT32 RetVal = SVC_OK;

    switch (EmrTaskFunc) {
        case SVC_APP_STAT_MENU_EMR_PAN:
            RetVal = SvcEmrTask_Pan(1);
        break;
        case SVC_APP_STAT_MENU_EMR_TILT:
            RetVal = SvcEmrTask_Tilt(-1);
        break;
        case SVC_APP_STAT_MENU_EMR_ROTATE:
            RetVal = SvcEmrTask_Rotate(-1);
        break;
        default:
            /* Other func not support */
        break;
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "EmrTask_Minus err. EmrTaskFunc failed", 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 EmrTask_Plus(void)
{
    UINT32 RetVal = SVC_OK;

    switch (EmrTaskFunc) {
        case SVC_APP_STAT_MENU_EMR_PAN:
            RetVal = SvcEmrTask_Pan(-1);
        break;
        case SVC_APP_STAT_MENU_EMR_TILT:
            RetVal = SvcEmrTask_Tilt(1);
        break;
        case SVC_APP_STAT_MENU_EMR_ROTATE:
            RetVal = SvcEmrTask_Rotate(1);
        break;
        default:
            /* Other func not support */
        break;
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "EmrTask_Minus err. EmrTaskFunc failed", 0U, 0U);
    }

    return SVC_OK;
}

static UINT32 EmrTask_Move(void)
{
    UINT32 RetVal = SVC_OK;

    EmrTaskTargetFovCheckBox.CurIdx = (EmrTaskTargetFovCheckBox.CurIdx + 1U) % EmrTaskTargetFovCheckBox.ListNum;
    EmrTaskTargetFovCheckBox.CheckBits = (UINT32) 1U << EmrTaskTargetFovCheckBox.CurIdx;
    EmrTaskTargetFov = EmrTaskTargetFovCheckBox.CurIdx;

    EmrTaskGuiCtrl |= SVC_EMR_TASK_GUI_FOV_UPDATE;

    switch (EmrTaskTargetFov) {
        case 1U:
        case 2U:
            #if defined(CONFIG_BSP_CV28DK_8LAYER_A_V100) || defined(CONFIG_BSP_CV28DK_OPTION_B_V100)
            RetVal = SvcEmrTask_SetTargetVout(ICAMAQ_TV_VOUT);  /* IN EMR, Fov1 and Fov1 in VOUT-B */
            #else
            RetVal = SvcEmrTask_SetTargetVout(1U);  /* IN EMR, Fov1 and Fov1 in VOUT-B */
            #endif
        break;
        default:
            RetVal = SvcEmrTask_SetTargetVout(0U);  /* Restore to VOUT-A for other case */
        break;
    }

    return RetVal;
}


static void EmrTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  SvcEmrCmd;

    UINT32  RetVal;

    SvcEmrCmd.pName    = "svc_emr_task";
    SvcEmrCmd.MainFunc = EmrTask_CmdEntry;
    SvcEmrCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&SvcEmrCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_EMR_TASK, "## fail to install svc emr command", 0U, 0U);
    }
}

static void EmrTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("target", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 FovIdx;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &FovIdx);
                if (SVC_OK == RetVal) {
                    RetVal = SvcEmrTask_SetTarget(FovIdx);
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("unit", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Unit;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &Unit);
                if (SVC_OK == RetVal) {
                    RetVal = SvcEmrTask_SetUnit((INT32) Unit);
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("pan", pArgVector[1U])) {
            if (2U < ArgCount) {
                INT32 Value;
                RetVal = SvcWrap_strtol(pArgVector[2U], &Value);
                if (SVC_OK == RetVal) {
                    if (Value == 0) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "Value cannot be 0", 0U, 0U);
                        RetVal = SVC_NG;
                    }
                    if (SVC_NG != RetVal) {
                        RetVal = SvcEmrTask_Pan(Value);
                    }
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("tilt", pArgVector[1U])) {
            if (2U < ArgCount) {
                INT32 Value;
                RetVal = SvcWrap_strtol(pArgVector[2U], &Value);
                if (SVC_OK == RetVal) {
                    if (Value == 0) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "Value cannot be 0", 0U, 0U);
                        RetVal = SVC_NG;
                    }
                    if (SVC_NG != RetVal) {
                        RetVal = SvcEmrTask_Tilt(Value);
                    }
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("rotate", pArgVector[1U])) {
            if (2U < ArgCount) {
                INT32 Value;
                RetVal = SvcWrap_strtol(pArgVector[2U], &Value);
                if (SVC_OK == RetVal) {
                    if (Value == 0) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "Value cannot be 0", 0U, 0U);
                        RetVal = SVC_NG;
                    }
                    if (SVC_NG != RetVal) {
                        RetVal = SvcEmrTask_Rotate(Value);
                    }
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("zoom", pArgVector[1U])) {
            if (2U < ArgCount) {
                INT32 Value;
                RetVal = SvcWrap_strtol(pArgVector[2U], &Value);
                if (SVC_OK == RetVal) {
                    if (Value == 0) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "Value cannot be 0", 0U, 0U);
                        RetVal = SVC_NG;
                    }
                    if (SVC_NG != RetVal) {
                        RetVal = SvcEmrTask_Zoom(Value);
                    }
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("zoom_anim", pArgVector[1U])) {
            if (9U < ArgCount) {
                UINT32 OffsetX = 0U, OffsetY = 0U, Width = 0U, Height = 0U;
                SVC_EMR_TASK_WINDOW_s SrcWin, DstWin;
                RetVal = SVC_OK;

                RetVal |= SvcWrap_strtoul(pArgVector[2U], &OffsetX);
                RetVal |= SvcWrap_strtoul(pArgVector[3U], &OffsetY);
                RetVal |= SvcWrap_strtoul(pArgVector[4U], &Width);
                RetVal |= SvcWrap_strtoul(pArgVector[5U], &Height);

                SrcWin.OffsetX = (UINT16) OffsetX;
                SrcWin.OffsetY = (UINT16) OffsetY;
                SrcWin.Width   = (UINT16) Width;
                SrcWin.Height  = (UINT16) Height;

                RetVal |= SvcWrap_strtoul(pArgVector[6U], &OffsetX);
                RetVal |= SvcWrap_strtoul(pArgVector[7U], &OffsetY);
                RetVal |= SvcWrap_strtoul(pArgVector[8U], &Width);
                RetVal |= SvcWrap_strtoul(pArgVector[9U], &Height);

                DstWin.OffsetX = (UINT16) OffsetX;
                DstWin.OffsetY = (UINT16) OffsetY;
                DstWin.Width   = (UINT16) Width;
                DstWin.Height  = (UINT16) Height;

                if (SVC_OK == RetVal) {
                    RetVal = SvcEmrTask_ZoomAnim(&SrcWin, &DstWin);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMR_TASK, "zoom anim err", 0U, 0U);
                    }
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("save", pArgVector[1U])) {
            RetVal = SvcEmrTask_SaveSetting();
        } else if (0 == SvcWrap_strcmp("set_win", pArgVector[1U])) {
            if (5U < ArgCount) {
                UINT32 VinID, FovIdx;
                UINT32 X_offset, Y_offset, Width, Height;
                SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

                FovIdx = EmrTaskTargetFov;

                RetVal = SVC_OK;
                RetVal |= SvcWrap_strtoul(pArgVector[2U], &X_offset);
                RetVal |= SvcWrap_strtoul(pArgVector[3U], &Y_offset);
                RetVal |= SvcWrap_strtoul(pArgVector[4U], &Width);
                RetVal |= SvcWrap_strtoul(pArgVector[5U], &Height);

                if (SVC_OK == RetVal) {
                    RetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID);

                    if (SVC_OK == RetVal) {
                        MirrorCmdMsg.Value       = (INT32) (0);
                        MirrorCmdMsg.Reserved[0] = (INT32) X_offset;
                        MirrorCmdMsg.Reserved[1] = (INT32) Y_offset;
                        MirrorCmdMsg.Reserved[2] = (INT32) Width;
                        MirrorCmdMsg.Reserved[3] = (INT32) Height;
                        MirrorCmdMsg.VinID       = (UINT8) VinID;
                        MirrorCmdMsg.FovIdx      = (UINT8) FovIdx;
                        MirrorCmdMsg.Cmd         = SVC_VIEW_CTRL_WARP_CMD_SET_WIN;

                        SvcViewCtrl_Exec(MirrorCmdMsg);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "error argument", 0U, 0U);
                    RetVal = SVC_NG;
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("target_vout", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 VoutID;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &VoutID);
                if (SVC_OK == RetVal) {
                    RetVal = SvcEmrTask_SetTargetVout(VoutID);
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("prev_set_srcwin", pArgVector[1U])) {
            if (5U < ArgCount) {
                UINT32 VinID, VoutID, FovIdx;
                UINT32 X_offset, Y_offset, Width, Height;
                SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

                FovIdx = EmrTaskTargetFov;
                VoutID = EmrTaskTargetVout;

                RetVal = SVC_OK;
                RetVal |= SvcWrap_strtoul(pArgVector[2U], &X_offset);
                RetVal |= SvcWrap_strtoul(pArgVector[3U], &Y_offset);
                RetVal |= SvcWrap_strtoul(pArgVector[4U], &Width);
                RetVal |= SvcWrap_strtoul(pArgVector[5U], &Height);

                if (SVC_OK == RetVal) {
                    RetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID);

                    if (SVC_OK == RetVal) {
                        MirrorCmdMsg.Value       = (INT32) (0);
                        MirrorCmdMsg.Reserved[0] = (INT32) X_offset;
                        MirrorCmdMsg.Reserved[1] = (INT32) Y_offset;
                        MirrorCmdMsg.Reserved[2] = (INT32) Width;
                        MirrorCmdMsg.Reserved[3] = (INT32) Height;
                        MirrorCmdMsg.VinID       = (UINT8) VinID;
                        MirrorCmdMsg.VoutID      = (UINT8) VoutID;
                        MirrorCmdMsg.FovIdx      = (UINT8) FovIdx;
                        MirrorCmdMsg.Cmd         = SVC_VIEW_CTRL_PREV_CMD_SET_SRCWIN;

                        SvcViewCtrl_Exec(MirrorCmdMsg);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "error argument", 0U, 0U);
                    RetVal = SVC_NG;
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("prev_set_dstwin", pArgVector[1U])) {
            if (5U < ArgCount) {
                UINT32 VinID, VoutID, FovIdx;
                UINT32 X_offset, Y_offset, Width, Height;
                SVC_VIEW_CTRL_CMD_MSG_s MirrorCmdMsg = {0};

                FovIdx = EmrTaskTargetFov;
                VoutID = EmrTaskTargetVout;

                RetVal = SVC_OK;
                RetVal |= SvcWrap_strtoul(pArgVector[2U], &X_offset);
                RetVal |= SvcWrap_strtoul(pArgVector[3U], &Y_offset);
                RetVal |= SvcWrap_strtoul(pArgVector[4U], &Width);
                RetVal |= SvcWrap_strtoul(pArgVector[5U], &Height);

                if (SVC_OK == RetVal) {
                    RetVal = SvcResCfg_GetVinIDOfFovIdx(FovIdx, &VinID);

                    if (SVC_OK == RetVal) {
                        MirrorCmdMsg.Value       = (INT32) (0);
                        MirrorCmdMsg.Reserved[0] = (INT32) X_offset;
                        MirrorCmdMsg.Reserved[1] = (INT32) Y_offset;
                        MirrorCmdMsg.Reserved[2] = (INT32) Width;
                        MirrorCmdMsg.Reserved[3] = (INT32) Height;
                        MirrorCmdMsg.VinID       = (UINT8) VinID;
                        MirrorCmdMsg.VoutID      = (UINT8) VoutID;
                        MirrorCmdMsg.FovIdx      = (UINT8) FovIdx;
                        MirrorCmdMsg.Cmd         = SVC_VIEW_CTRL_PREV_CMD_SET_DSTWIN;

                        SvcViewCtrl_Exec(MirrorCmdMsg);
                    }
                } else {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "error argument", 0U, 0U);
                    RetVal = SVC_NG;
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("operation", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Operation = 0U;

                RetVal = SVC_OK;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &Operation);

                RetVal = SvcEmrTask_SetOperation(Operation);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EMR_TASK, "Set operation failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_EMR_TASK, "error argument", 0U, 0U);
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("view_ctrl_dbg", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Level = 0U;

                RetVal = SVC_OK;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &Level);

                SvcViewCtrl_DebugEnable(Level);
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
        EmrTask_CmdUsage(PrintFunc);
    }
}

static void EmrTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_emr_task commands:\n");
    PrintFunc(" =======================\n");
    PrintFunc("             target    : Set emirror operation target fov idx\n");
    PrintFunc("              [FovIdx] - Target fov idx\n");
    PrintFunc("             unit      : Set emirror operation unit\n");
    PrintFunc("              [value]  - Target unit\n");
    PrintFunc("             pan       : Pan the image. Offset will be [value] * unit pixel \n");
    PrintFunc("              [value]  - Target value\n");
    PrintFunc("             tilt      : Tilt the image. Offset will be [value] * unit pixel \n");
    PrintFunc("              [value]  - Target value\n");
    PrintFunc("             rotate    : Rotate the image. Offset will be [value] * unit degree\n");
    PrintFunc("              [value]  - Target value\n");
    PrintFunc("             zoom      : Zoom image.\n");
    PrintFunc("              [value]  - 100 means increase unit*0.1x, [value] -100 means decrease unit*(-0.1)x\n");
    PrintFunc("             zoom_anim : Zoom animation to see full view\\cropping view\n");
    PrintFunc("              [src_x]  - Source offset_x\n");
    PrintFunc("              [src_y]  - Source offset_y\n");
    PrintFunc("              [src_w]  - Source fov width\n");
    PrintFunc("              [src_h]  - Source fov height\n");
    PrintFunc("              [dst_x]  - Target offset_x\n");
    PrintFunc("              [dst_y]  - Target offset_y\n");
    PrintFunc("              [dst_w]  - Target fov width\n");
    PrintFunc("              [dst_h]  - Target fov height\n");
    PrintFunc("             set_win   : Set Fov active window.\n");
    PrintFunc("              [x]      - Target offset_x\n");
    PrintFunc("              [y]      - Target offset_y\n");
    PrintFunc("              [width]  - Target fov width\n");
    PrintFunc("              [height] - Target fov height\n");
    PrintFunc(" =======================\n");
    PrintFunc("             target_vout     : Set mirror operation target VoutID\n");
    PrintFunc("              [VoutID]       - Target VoutID\n");
    PrintFunc("             prev_set_srcwin : Preview cropping src windonw.\n");
    PrintFunc("              [x]            - Target offset_x\n");
    PrintFunc("              [y]            - Target offset_y\n");
    PrintFunc("              [width]        - Target fov width\n");
    PrintFunc("              [height]       - Target fov height\n");
    PrintFunc("             prev_set_dstwin : Preview cropping dst windonw.\n");
    PrintFunc("              [x]            - Target offset_x\n");
    PrintFunc("              [y]            - Target offset_y\n");
    PrintFunc("              [width]        - Target fov width\n");
    PrintFunc("              [height]       - Target fov height\n");
    PrintFunc("             save      : Save the EMR setting\n");
    PrintFunc("             operation       : Determine mirror operation with warp or preview\n");
    PrintFunc("              [op]           - 0 - Warp, 1 - Preview\n");
    PrintFunc(" =======================\n");
    PrintFunc("             view_ctrl_dbg   : Enable viewctrl debug \n");
    PrintFunc("              [lvl]          - 0 - disable, others, debug level\n");
}
