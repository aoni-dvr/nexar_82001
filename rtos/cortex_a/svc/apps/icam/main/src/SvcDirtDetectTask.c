/**
 *  @file SvcDirtDetectTask.c
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
 *  @details svc Dirt Detection task
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcCmd.h"

/* svc-shared */
#include "SvcDirtDetect.h"
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcSysStat.h"
#include "SvcResCfg.h"
#include "SvcButtonCtrl.h"

/* svc-icam */
#include "SvcAppStat.h"
#include "SvcTaskList.h"
#include "SvcDirtDetectTask.h"
#include "SvcUtil.h"
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcBuffer.h"

#define DIRT_DETECT_TASK_OSD_CANVAS_LEVEL   (4U)
#define DIRT_DETECT_TASK_YUV_HIER   (4U)
//#define DIRT_DETECT_TASK_MASK_THD   (20U)
//#define DIRT_DETECT_TASK_BLOCK_THD  (50U)
//#define DIRT_DETECT_TASK_SGFL       (1U)

//#define DIRT_DETECT_TASK_ACC_NUM    (3U)
//#define DIRT_DETECT_TASK_COVERGE    (10U)

#if defined(CONFIG_ICAM_PROJECT_EMIRROR)
#define DIRT_DETECT_TASK_OSD_CHAN_DEFAULT   (VOUT_IDX_A)
#define DIRT_DETECT_TASK_INTERVAL   (8U)
#else
#define DIRT_DETECT_TASK_OSD_CHAN_DEFAULT   (VOUT_IDX_B)
#define DIRT_DETECT_TASK_INTERVAL   (3U)
#endif

#define DIRT_DETECT_TASK_BUF_WIDTH_MAX  (1920U)
#define DIRT_DETECT_TASK_BUF_HEIGHT_MAX (1080U)

#define SVC_LOG_DIRT_DETECT_TASK    "DIRT_DETECT_TASK"

#define SVC_DIRT_DETECT_CTRL_TASK_START (0x1U)
#define SVC_DIRT_DETECT_CTRL_TASK_STOP  (0x2U)

#define SVC_DIRT_DETECT_OSD_CTRL_INIT   (0x1U)
#define SVC_DIRT_DETECT_OSD_CTRL_UPDATE (0x2U)

/* The dirt detect output osd buffer */
static UINT8 DirtDetectPixelBaseBuf[DIRT_DETECT_TASK_BUF_WIDTH_MAX * DIRT_DETECT_TASK_BUF_HEIGHT_MAX] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8 FullFovOsdBuf[DIRT_DETECT_TASK_BUF_WIDTH_MAX * DIRT_DETECT_TASK_BUF_HEIGHT_MAX] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8 CropFovOsdBuf[DIRT_DETECT_TASK_BUF_WIDTH_MAX * DIRT_DETECT_TASK_BUF_HEIGHT_MAX] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
static UINT8 CropFovOsdBuf_LR_Revert[DIRT_DETECT_TASK_BUF_WIDTH_MAX * DIRT_DETECT_TASK_BUF_HEIGHT_MAX] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;

static void SvcDirtDetectGui_Init(void);
static void SvcDirtDetectGui_DeInit(void);

static void* DirtDetectTask_Entry(void* EntryArg);
static void DirtDetectTask_DrawScene(void);
static void DirtDetectTask_DrawOpMode(void);
static void DirtDetectTask_Draw(UINT32 VoutIdx, UINT32 Level);
static void DirtDetectTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void DirtDetectTask_Done(void);
static void DirtDetectTask_CmdInstall(void);
static void DirtDetectTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);
static void DirtDetectTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc);

static void DirtDetectTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo);

static UINT32 DirtDetectTask_AdcBtnEntry(void);

static AMBA_KAL_EVENT_FLAG_t SvcDirtDetectCtrlFlag GNU_SECTION_NOZEROINIT;

/* The dirt detct configuration */
static SVC_DIRT_DETECT_CONFIG_s DirtDetectTaskConfig GNU_SECTION_NOZEROINIT;
static UINT32 DirtDetectOsdCtrl = 0U;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetectTask_Init
 *
 *  @Description:: Init the Dirt detection module
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetectTask_Init(void)
{
#define DIRT_DETECT_CTRL_TASK_STACK_SIZE    (0x1000U)
    UINT32 RetVal;
    static SVC_TASK_CTRL_s DirtDetectCtrlTaskCtrl;
    static UINT8 DirtDetectCtrlTaskStack[DIRT_DETECT_CTRL_TASK_STACK_SIZE];
    static char  DirtDetectCtrlFlagName[] = "DirtDetectCtrlFlag";

    AmbaSvcWrap_MisraMemset(DirtDetectPixelBaseBuf, 0, sizeof(DirtDetectPixelBaseBuf));
    AmbaSvcWrap_MisraMemset(FullFovOsdBuf, 0, sizeof(FullFovOsdBuf));
    AmbaSvcWrap_MisraMemset(CropFovOsdBuf, 0, sizeof(CropFovOsdBuf));
    AmbaSvcWrap_MisraMemset(CropFovOsdBuf_LR_Revert, 0, sizeof(CropFovOsdBuf_LR_Revert));
    AmbaSvcWrap_MisraMemset(&DirtDetectTaskConfig, 0, sizeof(DirtDetectTaskConfig));

    RetVal = SvcDirtDetect_Init();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetectTask_Init err", 0U, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_EventFlagCreate(&SvcDirtDetectCtrlFlag, DirtDetectCtrlFlagName);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "DirtDetect Ctrl Flag create failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        /* trigger task to init vin source */
        DirtDetectCtrlTaskCtrl.Priority   = SVC_DIRT_DETECT_TASK_PRI;
        DirtDetectCtrlTaskCtrl.EntryFunc  = DirtDetectTask_Entry;
        DirtDetectCtrlTaskCtrl.pStackBase = DirtDetectCtrlTaskStack;
        DirtDetectCtrlTaskCtrl.StackSize  = DIRT_DETECT_CTRL_TASK_STACK_SIZE;
        DirtDetectCtrlTaskCtrl.CpuBits    = SVC_DIRT_DETECT_TASK_CPU_BITS;

        RetVal = SvcTask_Create("SvcDirtDetectCtrlTask", &DirtDetectCtrlTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "DirtDetect Ctrl Task create failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        static SVC_BUTTON_CTRL_ID     DirtDetectButtonCtrl_ID = 0U;
        static SVC_BUTTON_CTRL_LIST_s DirtDetectButtonCtrl[1] = {
            { "button_adc_1", DirtDetectTask_AdcBtnEntry },
        };

        RetVal = SvcButtonCtrl_Register(SVC_BUTTON_TYPE_ADC, DirtDetectButtonCtrl, 1, &DirtDetectButtonCtrl_ID);
        if (SVC_OK == RetVal) {
            RetVal = SvcButtonCtrl_Request(&DirtDetectButtonCtrl_ID);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcButtonCtrl_Request failed", DirtDetectButtonCtrl_ID, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcButtonCtrl_Register failed", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SVC_SYS_STAT_ID StatusID;
        RetVal = SvcSysStat_Register(SVC_APP_STAT_MENU, DirtDetectTask_MenuStatusCallback, &StatusID);
    }

    DirtDetectTask_CmdInstall();

    {
        UINT32 ItnBufBase = 0U, ItnBufSize = 0U;
        if (SVC_OK ==  SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_DIRT_DETECT_WRK, &ItnBufBase, &ItnBufSize)) {
            SvcLog_DBG(__func__,"DirtDetect Size %d",ItnBufSize,0U);
        } else {
            SvcLog_NG(__func__, "Line %d", __LINE__, 0U);
        }
        RetVal = SvcDirtDetect_InitAddress(ItnBufBase);
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    }

    return RetVal;
}

static void SvcDirtDetectGui_Init(void) {
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    const SVC_CHAN_CFG_s  *pChanCfg = NULL;
    UINT32 i, j;

    for (i = 0U; i < pResCfg->DispNum; i++) {
        if (pResCfg->DispStrm[i].VoutID == DIRT_DETECT_TASK_OSD_CHAN_DEFAULT) {
            pDispStrm = &(pResCfg->DispStrm[i]);
            for (j = 0U; j < pDispStrm->StrmCfg.NumChan; j++) {
                if (pDispStrm->StrmCfg.ChanCfg[j].FovId == DirtDetectTaskConfig.FovIdx) {
                    pChanCfg = &(pDispStrm->StrmCfg.ChanCfg[j]);
                }
            }
        }
    }

    if ((pDispStrm == NULL) || (pChanCfg == NULL)) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "Cannot find the related window setting in ResCfg. DirtDetect will not start", 0U, 0U);
        RetVal = SVC_NG;
    }

    AmbaSvcWrap_MisraMemset(FullFovOsdBuf, 0, sizeof(FullFovOsdBuf));
    AmbaSvcWrap_MisraMemset(CropFovOsdBuf, 0, sizeof(CropFovOsdBuf));
    AmbaSvcWrap_MisraMemset(CropFovOsdBuf_LR_Revert, 0, sizeof(CropFovOsdBuf_LR_Revert));

    if ((DirtDetectOsdCtrl & SVC_DIRT_DETECT_OSD_CTRL_INIT) == 0U) {
        SVC_OSD_WINDOW_s VoutChanWin = {
            .OffsetX = pChanCfg->DstWin.OffsetX,
            .OffsetY = pChanCfg->DstWin.OffsetY,
            .Width   = pChanCfg->DstWin.Width,
            .Height  = pChanCfg->DstWin.Height,
        };  /* Show result only on it's Fov VOUT area */
        SVC_OSD_WINDOW_s CanvasWin = {0};
        SVC_OSD_WINDOW_s FovWin = {
            .OffsetX = 0U,
            .OffsetY = 0U,
            .Width   = pResCfg->FovCfg[DirtDetectTaskConfig.FovIdx].MainWin.Width,
            .Height  = pResCfg->FovCfg[DirtDetectTaskConfig.FovIdx].MainWin.Height,
        };  /* Use full view for detection */
        SVC_OSD_WINDOW_s OsdWin = {0};

        RetVal = SvcOsd_TransferLivWinToOsdWin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, &VoutChanWin, &CanvasWin);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "Transfer from VoutChanWin to CanvasWin failed", 0U, 0U);
        }

        RetVal = SvcOsd_TransferLivWinToOsdWin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, &FovWin, &OsdWin);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "Transfer from FovWin to OsdWin failed", 0U, 0U);
        }

        DirtDetectTaskConfig.AlgoInterval    = DIRT_DETECT_TASK_INTERVAL;
        DirtDetectTaskConfig.PyramidHier     = DIRT_DETECT_TASK_YUV_HIER;
        DirtDetectTaskConfig.OsdOutputWidth  = OsdWin.Width;
        DirtDetectTaskConfig.OsdOutputHeight = OsdWin.Height;
        DirtDetectTaskConfig.pOutputBuf      = DirtDetectPixelBaseBuf;
        DirtDetectTaskConfig.pOsdOutputBuf   = FullFovOsdBuf;
        DirtDetectTaskConfig.pDetectDoneFunc = DirtDetectTask_Done;
        DirtDetectTaskConfig.OsdOutputBlockX = 16U;
        DirtDetectTaskConfig.OsdOutputBlockY = 10U;
    }

    if((DirtDetectOsdCtrl & SVC_DIRT_DETECT_OSD_CTRL_INIT) == 0U) {
        SvcGui_Register(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, DIRT_DETECT_TASK_OSD_CANVAS_LEVEL, "Dirt", DirtDetectTask_Draw, DirtDetectTask_DrawUPdate);
        DirtDetectOsdCtrl |= SVC_DIRT_DETECT_OSD_CTRL_INIT;
    }
}

static void SvcDirtDetectGui_DeInit(void) {
    SvcGui_Unregister(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, DIRT_DETECT_TASK_OSD_CANVAS_LEVEL);
    DirtDetectOsdCtrl &= ~(SVC_DIRT_DETECT_OSD_CTRL_INIT);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_Start
 *
 *  @Description:: Start the Dirt detection module
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetectTask_Start(void)
{
    UINT32 RetVal;

    SvcDirtDetectGui_Init();

    RetVal = SvcDirtDetect_Config(&DirtDetectTaskConfig);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetectTask_Start err. Config failed %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = SvcDirtDetect_Start(SVC_DIRT_DETECT_TASK_PRI, SVC_DIRT_DETECT_TASK_CPU_BITS);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetectTask_Start err", 0U, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SVC_APP_STAT_DIRT_DETECT_s DirtDetectStat = { .Status = SVC_APP_STAT_DIRT_DETECT_ENABLE };
        RetVal = SvcSysStat_Issue(SVC_APP_STAT_DIRT_DETECT, &DirtDetectStat);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "App Status issue failed", 0U, 0U);
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcDirtDetect_Stop
 *
 *  @Description:: Stop the Dirt detection module
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcDirtDetectTask_Stop(void)
{
    UINT32 RetVal;

    SVC_APP_STAT_DIRT_DETECT_s DirtDetectStat = { .Status = SVC_APP_STAT_DIRT_DETECT_DISABLE };
    RetVal = SvcSysStat_Issue(SVC_APP_STAT_DIRT_DETECT, &DirtDetectStat);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "App Status issue failed", 0U, 0U);
    }

    RetVal = SvcDirtDetect_Stop();
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetectTask_Stop err", 0U, 0U);
    }

    return RetVal;
}

static void* DirtDetectTask_Entry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlags = 0U;
    const ULONG  *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    while ((*pArg) == 0U) {
        if (RetVal != AmbaKAL_EventFlagGet(&SvcDirtDetectCtrlFlag, (SVC_DIRT_DETECT_CTRL_TASK_START | SVC_DIRT_DETECT_CTRL_TASK_STOP), AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER)) {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetectCtrl Flag Get Failed %d", RetVal, 0U);
        }

        if (ActualFlags == SVC_DIRT_DETECT_CTRL_TASK_START) {
            RetVal = SvcDirtDetectTask_Start();
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
        } else if (ActualFlags == SVC_DIRT_DETECT_CTRL_TASK_STOP) {
            RetVal = SvcDirtDetectTask_Stop();
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
        } else {
            SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetectCtrl Flag unkown", 0U, 0U);
        }
    }

    return NULL;
}

static void DirtDetectTask_DrawScene(void)
{
    #define CountNumber 100U//Uint is 50ms

    UINT32 Scene;
    static UINT32 Count = 0U;
    UINT32 RetVal;

    SvcDirtDetect_GetScene(&Scene);
    if (Scene == SVC_DIRT_DETECT_NIGHT_SCENE) {
        RetVal = SvcOsd_DrawString(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, 130U, 30U, 4U, 249U, "Night Scene!!");
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }

        if(Count == CountNumber) {
            Count --;
        } else {
            Count = CountNumber;
        }
    } else {
        if(Count == 0U) {
            RetVal = SvcOsd_DrawString(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, 130U, 30U, 4U, 249U, "");
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
        } else {
            RetVal = SvcOsd_DrawString(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, 30U, 30U, 4U, 249U, "Night Scene!!");
            if (Rval != SVC_OK) {
                SvcLog_NG(__func__, "%d", __LINE__, 0U);
            }
            Count --;
        }
    }
}

static void DirtDetectTask_DrawOpMode(void)
{
    UINT32 OpMode;
    UINT32 RetVal;

    SvcDirtDetect_GetOpMode(&OpMode);
    if (OpMode == SVC_DIRT_DETECT_REPEAT_MODE) {
        (void) SvcOsd_DrawString(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, 130U, 130U, 4U, 249U, "");
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    } else {
        (void) SvcOsd_DrawString(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, 130U, 130U, 4U, 249U, "");
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    }
}

static void DirtDetectTask_Draw(UINT32 VoutIdx, UINT32 Level)
{
    SVC_OSD_WINDOW_s FovWin = {0};
    SVC_OSD_WINDOW_s OsdWin = {0};
    SVC_OSD_WINDOW_s CanvasWin = {0};
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    const SVC_CHAN_CFG_s  *pChanCfg = NULL;
    UINT32 i, j;
    UINT32 RetVal;

    for (i = 0U; i < pResCfg->DispNum; i++) {
        if (pResCfg->DispStrm[i].VoutID == DIRT_DETECT_TASK_OSD_CHAN_DEFAULT) {
            pDispStrm = &(pResCfg->DispStrm[i]);
            for (j = 0U; j < pDispStrm->StrmCfg.NumChan; j++) {
                if (pDispStrm->StrmCfg.ChanCfg[j].FovId == DirtDetectTaskConfig.FovIdx) {
                    pChanCfg = &(pDispStrm->StrmCfg.ChanCfg[j]);
                }
            }
        }
    }

    if ((pDispStrm != NULL) && (pChanCfg != NULL)) {
        FovWin.OffsetX = pChanCfg->SrcWin.OffsetX;
        FovWin.OffsetY = pChanCfg->SrcWin.OffsetY;
        FovWin.Width   = pChanCfg->SrcWin.Width;
        FovWin.Height  = pChanCfg->SrcWin.Height;

        Rval = SvcOsd_TransferLivWinToOsdWin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, &FovWin, &OsdWin);
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }

        FovWin.OffsetX = pChanCfg->DstWin.OffsetX;
        FovWin.OffsetY = pChanCfg->DstWin.OffsetY;
        FovWin.Width   = pChanCfg->DstWin.Width;
        FovWin.Height  = pChanCfg->DstWin.Height;

        Rval = SvcOsd_TransferLivWinToOsdWin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, &FovWin, &CanvasWin);
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    }

    if (pDispStrm->VideoRotateFlip == AMBA_DSP_ROTATE_0_HORZ_FLIP) {
        UINT32 x, y;
        for(y=0; y < OsdWin.Height; y++){
            for(x=0; x < OsdWin.Width; x++){
                CropFovOsdBuf_LR_Revert[(OsdWin.Width - x) + (y * OsdWin.Width)] = CropFovOsdBuf[x + (y * OsdWin.Width)];
            }
        }
        Rval = SvcOsd_DrawBin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, CanvasWin.OffsetX, CanvasWin.OffsetY, OsdWin.Width, OsdWin.Height, CropFovOsdBuf_LR_Revert);
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    } else {
        Rval = SvcOsd_DrawBin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, CanvasWin.OffsetX, CanvasWin.OffsetY, OsdWin.Width, OsdWin.Height, CropFovOsdBuf);
        if (Rval != SVC_OK) {
            SvcLog_NG(__func__, "%d", __LINE__, 0U);
        }
    }

    DirtDetectTask_DrawScene();
    DirtDetectTask_DrawOpMode();

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    AmbaMisra_TouchUnused(CropFovOsdBuf_LR_Revert);
}

static void DirtDetectTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((DirtDetectOsdCtrl & SVC_DIRT_DETECT_OSD_CTRL_UPDATE) > 0U) {
        DirtDetectOsdCtrl &= ~(SVC_DIRT_DETECT_OSD_CTRL_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void DirtDetectTask_Done(void)
{
    INT32 ShiftVal = 0;
    SVC_OSD_WINDOW_s FovWin = {0};
    SVC_OSD_WINDOW_s OsdWin = {0};
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    const SVC_CHAN_CFG_s  *pChanCfg = NULL;
    UINT32 i, j;
    UINT32 RetVal;

    for (i = 0U; i < pResCfg->DispNum; i++) {
        if (pResCfg->DispStrm[i].VoutID == DIRT_DETECT_TASK_OSD_CHAN_DEFAULT) {
            pDispStrm = &(pResCfg->DispStrm[i]);
            for (j = 0U; j < pDispStrm->StrmCfg.NumChan; j++) {
                if (pDispStrm->StrmCfg.ChanCfg[j].FovId == DirtDetectTaskConfig.FovIdx) {
                    pChanCfg = &(pDispStrm->StrmCfg.ChanCfg[j]);
                }
            }
        }
    }

    if ((pDispStrm != NULL) && (pChanCfg != NULL)) {
        /* Copy the desired ROI */
        FovWin.OffsetX = pChanCfg->SrcWin.OffsetX;
        FovWin.OffsetY = pChanCfg->SrcWin.OffsetY;
        FovWin.Width   = pChanCfg->SrcWin.Width;
        FovWin.Height  = pChanCfg->SrcWin.Height;

        if (SVC_OK == SvcOsd_TransferLivWinToOsdWin(DIRT_DETECT_TASK_OSD_CHAN_DEFAULT, &FovWin, &OsdWin)) {
            ShiftVal = ((INT32) OsdWin.OffsetY) * ((INT32) OsdWin.Width);
        }
    }

    RetVal = AmbaWrap_memcpy(CropFovOsdBuf, &(FullFovOsdBuf[ShiftVal]), ((UINT32) OsdWin.Width) * ((UINT32) OsdWin.Height));
    if (Rval != SVC_OK) {
        SvcLog_NG(__func__, "%d", __LINE__, 0U);
    }
    {
        UINT32 Status;
        SvcDirtDetect_GetStatus(&Status);
        if (Status == SVC_DIRT_DETECT_STATUS_FORCE_STOP) {
            if (SVC_OK != AmbaKAL_EventFlagSet(&SvcDirtDetectCtrlFlag, SVC_DIRT_DETECT_CTRL_TASK_STOP)) {
                SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "SvcDirtDetect Flag set failed", 0U, 0U);
            }
        }
    }

    DirtDetectOsdCtrl |= SVC_DIRT_DETECT_OSD_CTRL_UPDATE;
}

static void DirtDetectTask_MenuStatusCallback(UINT32 StatIdx, void *pInfo)
{
    UINT32 RetVal;
    UINT32 OpMode;
    UINT32 RetVal;

    SVC_APP_STAT_MENU_s *pStatus = NULL;

    AmbaMisra_TouchUnused(&StatIdx);    /* Misra-c fixed */
    AmbaMisra_TouchUnused(pInfo);       /* Misra-c fixed */
    AmbaMisra_TouchUnused(pStatus);     /* Misra-c fixed */

    AmbaMisra_TypeCast(&pStatus, &pInfo);

    if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_EMR) {
        if (pStatus->FuncIdx == SVC_APP_STAT_MENU_EMR_DIRT_DETECT) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                /* Check status and start/stop */
                UINT32 Status;
                RetVal = SvcDirtDetect_GetStatus(&Status);
                if (Status == SVC_DIRT_DETECT_STATUS_IDLE) {
                    RetVal = SvcDirtDetectTask_Start();
                    if (Rval != SVC_OK) {
                        SvcLog_NG(__func__, "%d", __LINE__, 0U);
                    }
                } else {
                    RetVal = SvcDirtDetectTask_Stop();
                    if (Rval != SVC_OK) {
                        SvcLog_NG(__func__, "%d", __LINE__, 0U);
                    }
                }
            }
        }
    } else if (pStatus->Type == SVC_APP_STAT_MENU_TYPE_ADAS_DVR) {
        if (pStatus->FuncIdx == SVC_APP_STAT_MENU_ADAS_DIRT_DETECT) {
            if (pStatus->Operation == SVC_APP_STAT_MENU_OP_ENTER) {
                /* Check status and start/stop */
                UINT32 Status;
                SvcDirtDetect_GetStatus(&Status);
                if (Status == SVC_DIRT_DETECT_STATUS_IDLE) {
                    RetVal = SvcDirtDetectTask_Start();
                    if (Rval != SVC_OK) {
                        SvcLog_NG(__func__, "%d", __LINE__, 0U);
                    }
                } else {
                    RetVal = SvcDirtDetectTask_Stop();
                    if (Rval != SVC_OK) {
                        SvcLog_NG(__func__, "%d", __LINE__, 0U);
                    }
                }
            }
        }
    } else {
        /* Do nothing */
    }

    if (pStatus->Operation == SVC_APP_STAT_MENU_OP_PLUS) {
        SvcDirtDetect_GetOpMode(&OpMode);
        if(OpMode == SVC_DIRT_DETECT_ONE_TIME) {
            SvcLog_NG("SVC_DIRT_DETECT_REPEAT_MODE", "", 0U, 0U);
            SvcDirtDetect_SetOpMode(SVC_DIRT_DETECT_REPEAT_MODE);
        } else {
            SvcLog_NG("SVC_DIRT_DETECT_ONE_TIME", "", 0U, 0U);
            SvcDirtDetect_SetOpMode(SVC_DIRT_DETECT_ONE_TIME);
        }
    }

    if (pStatus->Operation == SVC_APP_STAT_MENU_OP_MINUS) {
        AmbaSvcWrap_MisraMemset(CropFovOsdBuf, 0, sizeof(CropFovOsdBuf));
        SvcDirtDetectGui_DeInit();
    }

    AmbaMisra_TouchUnused(&RetVal);
}

static UINT32 DirtDetectTask_AdcBtnEntry(void)
{
    UINT32 RetVal;

    DirtDetectTaskConfig.FovIdx = 2U;
    Rval = SvcDirtDetectTask_Start();
    if (Rval != SVC_OK) {
        SvcLog_NG(__func__, "%d", __LINE__, 0U);
    }

    return SVC_OK;
}

static void DirtDetectTask_CmdInstall(void)
{
    AMBA_SHELL_COMMAND_s  DirtDetectTaskCmd;

    UINT32  RetVal;

    DirtDetectTaskCmd.pName    = "svc_dirtdetect_task";
    DirtDetectTaskCmd.MainFunc = DirtDetectTask_CmdEntry;
    DirtDetectTaskCmd.pNext    = NULL;

    RetVal = SvcCmd_CommandRegister(&DirtDetectTaskCmd);
    if (SHELL_ERR_SUCCESS != RetVal) {
        SvcLog_NG(SVC_LOG_DIRT_DETECT_TASK, "## fail to install svc dirt detect command", 0U, 0U);
    }
}

static void DirtDetectTask_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;
    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("hier", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Hier;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &Hier);
                if (SVC_OK == RetVal) {
                    DirtDetectTaskConfig.PyramidHier = Hier;
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("interval", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Interval;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &Interval);
                if (SVC_OK == RetVal) {
                    DirtDetectTaskConfig.AlgoInterval = Interval;
                }
            } else {
                RetVal = SVC_NG;
            }
        // } else if (0 == SvcWrap_strcmp("debug", pArgVector[1U])){
        //     RetVal = SvcDirtDetect_Debug(&pArgVector[2U]);
        } else if (0 == SvcWrap_strcmp("target", pArgVector[1U])) {
            if (2U < ArgCount) {
                UINT32 Fov;
                RetVal = SvcWrap_strtoul(pArgVector[2U], &Fov);
                if (SVC_OK == RetVal) {
                    DirtDetectTaskConfig.FovIdx = Fov;
                }
            } else {
                RetVal = SVC_NG;
            }
        } else if (0 == SvcWrap_strcmp("start", pArgVector[1U])) {
            RetVal = SvcDirtDetectTask_Start();
        } else if (0 == SvcWrap_strcmp("stop", pArgVector[1U])) {
            RetVal = SvcDirtDetectTask_Stop();
        } else {
            RetVal = SVC_NG;
        }
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        DirtDetectTask_CmdUsage(PrintFunc);
    }
}

static void DirtDetectTask_CmdUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("svc_dirtdetect_task commands:\n");
    PrintFunc("                    hier [value]     : Set dirt detection algo hier to be used\n");
    PrintFunc("                    interval [value] : Set dirt detection algo interval (in frames)\n");
    PrintFunc("                    tareget [value]  : Set target fov to be used\n");
    PrintFunc("                    start            : Start dirt detection algo\n");
    PrintFunc("                    stop             : Stop dirt detection algo\n");
}
