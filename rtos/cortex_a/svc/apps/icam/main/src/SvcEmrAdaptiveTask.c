/**
 *  @file SvcEmrAdaptiveTask.c
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
 *  @details svc Emr Adaptive Task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"

/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* svc-shared */
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvFlow_Comm.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcPlat.h"

/* svc-icam */
#include "SvcMem.h"
#include "SvcBufMap.h"
#include "SvcBuffer.h"
#include "SvcTaskList.h"
#include "SvcTask.h"
#include "SvcEmrAdaptiveTask.h"
#include "SvcResCfgTask.h"
#include "SvcODDrawTask.h"
#include "SvcCvAppDef.h"
#include "SvcLogoDraw.h"

/* vendors */
#include "AmbaCalib_EmirrorAdaptiveIF.h"
#include "AmbaCache.h"
#include "AmbaOD_Tracker.h"

#define SVC_LOG_EMA_TASK     "EMA_TASK"

#define EMA_GUI_LEVEL                       (22U)

#define SVC_EMR_ADAPTIVE_TASK_STACK_SIZE    (0xA0000)
#define SVC_EMA_BLENDING_LINE_WIDTH         (32U)
#define SVC_EMA_BLENDING_HALF_WIDTH         (16U)
#define SVC_EMA_CHAN_NUM                    (3U)
#define SVC_EMA_BBX_BUFFER_NUM              (3U)
#define SVC_EMA_RESTRICT_WIDTH              (72U)

#define SVC_EMA_YUV_DATA_READY              (0x1U)
#define SVC_EMA_DRAW_DATA_READY             (0x1U)
#define SVC_EMA_DRAW_UPDATE                 (0x10U)

#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define SVC_EMA_COLOR_TRI                   (0x80E02040U)
#define SVC_EMA_COLOR_LEFT                  (0x8060c080U)
#define SVC_EMA_COLOR_RIGHT                 (0x80a000c0U)
#else
#define SVC_EMA_COLOR_TRI                   (79U)
#define SVC_EMA_COLOR_LEFT                  (179U)
#define SVC_EMA_COLOR_RIGHT                 (197U)
#endif

typedef struct {
    UINT32  MainWidth;
    UINT32  MainHeight;
    UINT32  DstWidth;
    UINT32  DstHeight;
} EMA_RES_WINDOW_s;

typedef struct {
    AMBA_OD_2DBBX_LIST_s            *pBbxInfo[SVC_EMA_FLOW_CHAN_NUM];
    const AMBA_CAL_ROI_s            *pRestrictArea[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s *pStitchLine[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s *pStitchLinePrev[SVC_EMA_OVERLAP_AREA_NUM];
} EMA_MSG_BBX_DATA_s;

typedef struct {
    UINT32                           PrintFlag;
    UINT32                           OsdFlag;
    UINT32                           FilterWidth;
    UINT32                           BbxRoiMargin;
    UINT32                           BbxMissTolerance;

    AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s   View;
    AMBA_CAL_EM_3IN1_VIEW_CFG_V1_s   ViewCfg;
    UINT32                           MaxStchLineOffset[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_FILTER_RULE_V2_s    FilterRule;
    AMBA_CAL_EMA_ROI_TBL_s           RoiTbl;
    AMBA_CAL_ROI_s                   RestrictArea[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s  Line[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s  OsdLine[SVC_EMA_OVERLAP_AREA_NUM];

    SVC_TASK_CTRL_s                  BbxInfoTaskCtrl;
    UINT8                            BbxInfoTaskStack[SVC_EMR_ADAPTIVE_TASK_STACK_SIZE];

    SVC_TASK_CTRL_s                  StitchLineTaskCtrl;
    UINT8                            StitchLineTaskStack[SVC_EMR_ADAPTIVE_TASK_STACK_SIZE];

    void                            *pTrackerHandler;
    AMBA_CAL_EM_CALIB_INFO_DATA_s    CalData;
    AMBA_SR_OBJ_INFO_s               SrObjInfo;
    AMBA_CAL_EM_CALC_COORD_CFG_V1_s  EmCoorCfg;

    AMBA_KAL_MUTEX_t                 BbxMtx;
    AMBA_KAL_MUTEX_t                 LineMtx;
    AMBA_KAL_MUTEX_t                 OsdMtx;
} SVC_EMA_CTRL_s;

typedef struct {
    UINT32                           CvFlowChan[SVC_EMA_FLOW_CHAN_NUM];
    AMBA_OD_2DBBX_LIST_s             BbxInfo[SVC_EMA_BBX_BUFFER_NUM][SVC_EMA_FLOW_CHAN_NUM];
    SVC_CV_DETRES_BBX_LIST_s         BbxList[SVC_EMA_BBX_BUFFER_NUM][SVC_EMA_FLOW_CHAN_NUM];
} SVC_EMA_BBX_RESULT_s;

static AMBA_KAL_EVENT_FLAG_t SvcEmaYuvFlag  GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SvcEmaBbxFlag  GNU_SECTION_NOZEROINIT;
static AMBA_KAL_EVENT_FLAG_t SvcEmaDrawFlag GNU_SECTION_NOZEROINIT;

static void   EmrAdaptiveTask_DrawStitchLineCB(UINT32 VoutIdx, UINT32 Level);
static void   EmrAdaptiveTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate);
static void   EmrAdaptiveTask_DetectionResultHandler(UINT32 CvFlowChan, void *pDetResMsg);
static void   EmrAdaptiveTask_RefODResult(UINT32 CvFlowChan, UINT32 CvType, void *pOutput);
static void   EmrAdaptiveTask_ResizeBbx(UINT32 FovIdx, AMBA_OD_2DBBX_s *pBbx);
static void   EmrAdaptiveTask_FilterBbx(AMBA_OD_2DBBX_LIST_s *pBbxList);
static UINT32 EmrAdaptiveTask_SetTracker(void);
static void   EmrAdaptiveTask_SetDistance(void);
static UINT32 EmrAdaptiveTask_YuvDataRdy(const void *pEventData);
static void   EmrAdaptive_BlendCfg(const AMBA_CAL_EMA_STITCH_LINE_INFO_s *pLine , AMBA_CAL_EMA_BLEND_CFG_s *pNewBlendCfg);

static EMA_RES_WINDOW_s EmaWindow[SVC_EMA_FLOW_CHAN_NUM] GNU_SECTION_NOZEROINIT;
static SVC_EMA_CTRL_s EmaCtrl GNU_SECTION_NOZEROINIT;
static SVC_EMA_BBX_RESULT_s EmaBbxResult GNU_SECTION_NOZEROINIT;    /* Queue is used to save the detection reuslt (BBX) */

static UINT32 EmrAdaptiveCvFlowRegisterID[SVC_EMA_FLOW_CHAN_NUM] = {0};

/**
 * Initialize parameters in emr adaptive task
 * @return None
 */
void SvcEmrAdaptiveTask_Init(void)
{
    EmaCtrl.PrintFlag        = 0U;
    EmaCtrl.OsdFlag          = 0xFU;
    EmaCtrl.FilterWidth      = 0U;
    EmaCtrl.BbxRoiMargin     = 32U;
    EmaCtrl.BbxMissTolerance = 6U;
}

/**
 * Configure emr adaptive task
 * @return 0-OK, 1-NG
 */
UINT32 SvcEmrAdaptiveTask_Config(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    static UINT32 EmaRoiTblkBufSize;
    static ULONG  EmaRoiTblWorkBuf;
    UINT8 *pEmaRoiTblWorkBuf;

    if ((pResCfg->UserFlag & SVC_EMR_STITCH) > 0U) {
        SvcLog_DBG(SVC_LOG_EMA_TASK, "SvcEmrAdaptiveTask_Config starts", 0U, 0U);

        EmaWindow[2].MainWidth  = pResCfg->FovCfg[2].MainWin.Width;
        EmaWindow[2].MainHeight = pResCfg->FovCfg[2].MainWin.Height;
        EmaWindow[2].DstWidth   = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].DstWin.Width;
        EmaWindow[2].DstHeight  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].DstWin.Height;

        EmaCtrl.View.VoutAreaBack.StartX  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].DstWin.OffsetX;
        EmaCtrl.View.VoutAreaBack.StartY  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].DstWin.OffsetY;
        EmaCtrl.View.VoutAreaBack.Width   = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].DstWin.Width;
        EmaCtrl.View.VoutAreaBack.Height  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[0].DstWin.Height;
        EmaCtrl.View.VoutAreaRight.StartX = pResCfg->DispStrm[0].StrmCfg.ChanCfg[1].DstWin.OffsetX;
        EmaCtrl.View.VoutAreaRight.StartY = pResCfg->DispStrm[0].StrmCfg.ChanCfg[1].DstWin.OffsetY;
        EmaCtrl.View.VoutAreaRight.Width  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[1].DstWin.Width;
        EmaCtrl.View.VoutAreaRight.Height = pResCfg->DispStrm[0].StrmCfg.ChanCfg[1].DstWin.Height;
        EmaCtrl.View.VoutAreaLeft.StartX  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[2].DstWin.OffsetX;
        EmaCtrl.View.VoutAreaLeft.StartY  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[2].DstWin.OffsetY;
        EmaCtrl.View.VoutAreaLeft.Width   = pResCfg->DispStrm[0].StrmCfg.ChanCfg[2].DstWin.Width;
        EmaCtrl.View.VoutAreaLeft.Height  = pResCfg->DispStrm[0].StrmCfg.ChanCfg[2].DstWin.Height;

        EmaCtrl.MaxStchLineOffset[0] = 440U;
        EmaCtrl.MaxStchLineOffset[1] = 440U;

        EmaCtrl.FilterRule.Line[0].X1    = 690U;
        EmaCtrl.FilterRule.Line[0].Y1    = 480U;
        EmaCtrl.FilterRule.Line[0].X2    = 960U;
        EmaCtrl.FilterRule.Line[0].Y2    = 210U;
        EmaCtrl.FilterRule.Line[0].VLine = 959U - 180U;
        EmaCtrl.FilterRule.Line[1].X1    = 960U;
        EmaCtrl.FilterRule.Line[1].Y1    = 210U;
        EmaCtrl.FilterRule.Line[1].X2    = 1230U;
        EmaCtrl.FilterRule.Line[1].Y2    = 480U;
        EmaCtrl.FilterRule.Line[1].VLine = 960U + 180U;

        RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_EMA_ROITBL, &EmaRoiTblWorkBuf, &EmaRoiTblkBufSize);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcBuffer_Request failed", 0U, 0U);
        }

        AmbaMisra_TypeCast(&pEmaRoiTblWorkBuf, &EmaRoiTblWorkBuf);
        EmaCtrl.RoiTbl.pAddr = pEmaRoiTblWorkBuf;

        RetVal = AmbaCal_EmaGetRoiTableSize(&EmaCtrl.View, &EmaCtrl.RoiTbl.Size);
        if (SVC_OK == RetVal) {
            RetVal = AmbaCal_EmaGenRoiTableV2(&EmaCtrl.View, &EmaCtrl.FilterRule, &EmaCtrl.RoiTbl);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaGenRoiTable failed", 0U, 0U);
            }
        }

        EmaCtrl.RestrictArea[0].StartX = 959U - SVC_EMA_RESTRICT_WIDTH;
        EmaCtrl.RestrictArea[0].StartY = 0U;
        EmaCtrl.RestrictArea[0].Width  = SVC_EMA_RESTRICT_WIDTH;
        EmaCtrl.RestrictArea[0].Height = 480U;
        EmaCtrl.RestrictArea[1].StartX = 960U;
        EmaCtrl.RestrictArea[1].StartY = 0U;
        EmaCtrl.RestrictArea[1].Width  = SVC_EMA_RESTRICT_WIDTH;
        EmaCtrl.RestrictArea[1].Height = 480U;

        EmaCtrl.Line[0].StitchX = (959U - SVC_EMA_RESTRICT_WIDTH) - SVC_EMA_BLENDING_HALF_WIDTH;
        EmaCtrl.Line[1].StitchX = (960U + SVC_EMA_RESTRICT_WIDTH) + SVC_EMA_BLENDING_HALF_WIDTH;
        EmaCtrl.Line[0].Status  = AMBA_CAL_STITCH_LINE_STATUS_MUST;
        EmaCtrl.Line[1].Status  = AMBA_CAL_STITCH_LINE_STATUS_MUST;

        EmaCtrl.OsdLine[0].StitchX = (959U - SVC_EMA_RESTRICT_WIDTH) - SVC_EMA_BLENDING_HALF_WIDTH;
        EmaCtrl.OsdLine[1].StitchX = (960U + SVC_EMA_RESTRICT_WIDTH) + SVC_EMA_BLENDING_HALF_WIDTH;

        RetVal = SvcEmrAdaptiveTask_SetDefaultBldTbl();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcEmrAdaptiveTask_SetDefaultBldTbl failed", 0U, 0U);
        }

        /* setup tracker */
        RetVal = EmrAdaptiveTask_SetTracker();
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "EmrAdaptiveTask_SetTracker failed", 0U, 0U);
        }

        /* setup distance */
        EmrAdaptiveTask_SetDistance();

        /* Draw Logo */
        SvcLogoDraw_Init(VOUT_IDX_A);
        SvcLogoDraw_Update();

        SvcLog_DBG(SVC_LOG_EMA_TASK, "SvcEmrAdaptiveTask_Config ends", 0U, 0U);
    }

    return RetVal;
}

static void* SvcEmrAdaptvie_BbxInfoTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlags = 0U;
    UINT32 Abandon[SVC_EMA_FLOW_CHAN_NUM] = {0};
    UINT32 i, bbx;

    const DOUBLE TrackerLevel = 0.5;
    AMBA_OD_TRACKER_LIST_s    TrackerList;
    AMBA_SR_WORLD_3D_OBJECT_s Object3D;
    AMBA_SR_SROBJECT_DATA_s   SROut;

    SVC_EMA_DRAW_INFO_s  EmaDrawInfo;
    AMBA_OD_2DBBX_LIST_s DrawBbxList;

    AMBA_OD_2DBBX_LIST_s            BbxInfo[SVC_EMA_FLOW_CHAN_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s Line[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s LinePrev[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_ROI_s                  RestrictArea[SVC_EMA_OVERLAP_AREA_NUM];
    EMA_MSG_BBX_DATA_s              Msg;
    const ULONG *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    RetVal |= AmbaWrap_memcpy(RestrictArea, EmaCtrl.RestrictArea, sizeof(RestrictArea));
    RetVal |= AmbaWrap_memcpy(Line, EmaCtrl.Line, sizeof(Line));
    RetVal |= AmbaWrap_memset(&DrawBbxList, 0, sizeof(AMBA_OD_2DBBX_LIST_s));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy/AmbaWrap_memset RestrictArea/Line/DrawBbxList failed", 0U, 0U);
    }

    while ((*pArg) == 0U) {
        /* Get Bbx flag to check which buf is ready */
        if (SVC_OK == AmbaKAL_EventFlagGet(&SvcEmaBbxFlag, 0xFFFFFFFFU, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER)) {
            for (i = 0U; i < SVC_EMA_BBX_BUFFER_NUM; i++) {
                if ((ActualFlags & (UINT32) ((UINT32)1U << i)) > 0U) {
                    UINT32 BufIdx = i;

                    if (SVC_OK == AmbaKAL_MutexTake(&(EmaCtrl.BbxMtx), AMBA_KAL_WAIT_FOREVER)) {
                        /* Copy EmaCtrl info */
                        if (SVC_OK != AmbaWrap_memcpy(&BbxInfo[SVC_EMA_BACK_FOV], &EmaBbxResult.BbxInfo[BufIdx][SVC_EMA_BACK_FOV], sizeof(AMBA_OD_2DBBX_LIST_s))) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy BbxInfo[SVC_EMA_BACK_FOV] failed", 0U, 0U);
                        }

                        RetVal = AmbaKAL_MutexGive(&(EmaCtrl.BbxMtx));
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexGive BbxMtx failed", 0U, 0U);
                        }
                        /* Update LinePrev */
                        if (SVC_OK != AmbaWrap_memcpy(LinePrev, Line, sizeof(Line))) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy LinePrev failed", 0U, 0U);
                        }

                        /* Filter Bbx in Center Channel */
                        EmrAdaptiveTask_FilterBbx(&BbxInfo[SVC_EMA_BACK_FOV]);

                        /* Run Tracker on Back FOV */
                        RetVal = AmbaOD_Tracker(EmaCtrl.pTrackerHandler, &BbxInfo[SVC_EMA_BACK_FOV], TrackerLevel, &TrackerList);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaOD_Tracker failed", 0U, 0U);
                        }

                        if (SVC_OK != AmbaWrap_memcpy(&DrawBbxList, &BbxInfo[SVC_EMA_BACK_FOV], sizeof(AMBA_OD_2DBBX_LIST_s))) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy DrawBbxList failed", 0U, 0U);
                        }

                        for (bbx = 0; bbx < BbxInfo[SVC_EMA_BACK_FOV].NumBbx; bbx++) {
                            /* Run Object Distance on Back FOV */
                            if (SVC_OK != AmbaWrap_memcpy(&EmaCtrl.SrObjInfo.BbxInfo, &BbxInfo[SVC_EMA_BACK_FOV].Bbx[bbx], sizeof(AMBA_OD_2DBBX_s))) {
                                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy EmaCtrl.SrObjInfo.BbxInfo failed", 0U, 0U);
                            }

                            RetVal = AmbaSR_EstObjPosV1(&EmaCtrl.SrObjInfo, &EmaCtrl.EmCoorCfg, &Object3D);
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaSR_EstObjPosV1 failed", 0U, 0U);
                            }

                            if (SVC_OK != AmbaWrap_memcpy(&SROut.SRObject[bbx].Object, &Object3D, sizeof(AMBA_SR_WORLD_3D_OBJECT_s))) {
                                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy SROut.SRObject[bbx].Object failed", 0U, 0U);
                            }

                            /* Scale and Flip Back Bbx for Ema Draw */
                            /* Use FovIdx in Ema Window */
                            DrawBbxList.Bbx[bbx].X = (UINT16) ((BbxInfo[SVC_EMA_BACK_FOV].Bbx[bbx].X * EmaWindow[2].MainWidth) / EmaWindow[2].DstWidth);
                            DrawBbxList.Bbx[bbx].Y = (UINT16) ((BbxInfo[SVC_EMA_BACK_FOV].Bbx[bbx].Y * EmaWindow[2].MainHeight) / EmaWindow[2].DstHeight);
                            DrawBbxList.Bbx[bbx].W = (UINT16) ((BbxInfo[SVC_EMA_BACK_FOV].Bbx[bbx].W * EmaWindow[2].MainWidth) / EmaWindow[2].DstWidth);
                            DrawBbxList.Bbx[bbx].H = (UINT16) ((BbxInfo[SVC_EMA_BACK_FOV].Bbx[bbx].H * EmaWindow[2].MainHeight) / EmaWindow[2].DstHeight);

                            DrawBbxList.Bbx[bbx].X = (UINT16) (((EmaWindow[2].MainWidth - 1U) - DrawBbxList.Bbx[bbx].X) - DrawBbxList.Bbx[bbx].W);
                        }
                        SROut.TotalNum = BbxInfo[SVC_EMA_BACK_FOV].NumBbx;

                        /* Ema Draw */
                        EmaDrawInfo.Chan       = EmaBbxResult.CvFlowChan[SVC_EMA_BACK_FOV];
                        EmaDrawInfo.FovIdx     = 2U;    /* Center Fov */
                        EmaDrawInfo.pBbx       = &DrawBbxList;
                        EmaDrawInfo.Class_name = EmaBbxResult.BbxList[BufIdx][SVC_EMA_BACK_FOV].class_name;
                        EmaDrawInfo.pSRData    = &SROut;
                        RetVal = SvcODDrawTask_Msg(SVC_EMA_DRAW, &EmaDrawInfo);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcODDrawTask_Msg failed", 0U, 0U);
                        }

                        /* Find Stitch Line */
                        RetVal |= AmbaWrap_memset(&BbxInfo[1], 0, sizeof(AMBA_OD_2DBBX_LIST_s));
                        RetVal |= AmbaWrap_memset(&BbxInfo[2], 0, sizeof(AMBA_OD_2DBBX_LIST_s));
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memset BbxInfo[1]/BbxInfo[2] failed", 0U, 0U);
                        }
                        Msg.pBbxInfo[SVC_EMA_BACK_FOV]  = &BbxInfo[SVC_EMA_BACK_FOV];
                        Msg.pBbxInfo[SVC_EMA_LEFT_FOV]  = &BbxInfo[SVC_EMA_LEFT_FOV];
                        Msg.pBbxInfo[SVC_EMA_RIGHT_FOV] = &BbxInfo[SVC_EMA_RIGHT_FOV];
                        Msg.pRestrictArea[0]            = &RestrictArea[0];
                        Msg.pRestrictArea[1]            = &RestrictArea[1];
                        Msg.pStitchLine[0]              = &Line[0];
                        Msg.pStitchLine[1]              = &Line[1];
                        Msg.pStitchLinePrev[0]          = &LinePrev[0];
                        Msg.pStitchLinePrev[1]          = &LinePrev[1];

                        if (SVC_OK != AmbaCal_EmaFindStiLineV2(&EmaCtrl.View, Msg.pBbxInfo, &EmaCtrl.RoiTbl, Msg.pRestrictArea, Msg.pStitchLinePrev,
                                                                EmaCtrl.MaxStchLineOffset, EmaCtrl.BbxRoiMargin, EmaCtrl.BbxMissTolerance, Msg.pStitchLine)) {
                            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaFindStiLineV2 failed", 0U, 0U);
                        }

                        {
                            /* corner case */
                            if (Line[0].StitchX < (480U + SVC_EMA_BLENDING_HALF_WIDTH)) {
                                Line[0].StitchX = 480U + SVC_EMA_BLENDING_HALF_WIDTH;
                            }
                            if (Line[0].StitchX > ((959U - SVC_EMA_RESTRICT_WIDTH) - SVC_EMA_BLENDING_HALF_WIDTH)) {
                                Line[0].StitchX = (959U - SVC_EMA_RESTRICT_WIDTH) - SVC_EMA_BLENDING_HALF_WIDTH;
                            }
                            if (Line[1].StitchX < ((960U + SVC_EMA_RESTRICT_WIDTH) + SVC_EMA_BLENDING_HALF_WIDTH)) {
                                Line[1].StitchX = (960U + SVC_EMA_RESTRICT_WIDTH) + SVC_EMA_BLENDING_HALF_WIDTH;
                            }
                            if (Line[1].StitchX > (1439U - SVC_EMA_BLENDING_HALF_WIDTH)) {
                                Line[1].StitchX = 1439U - SVC_EMA_BLENDING_HALF_WIDTH;
                            }
                        }

                        {
                            /* abandon found line if its distance between old one is shorter than margin */
                            Abandon[0] = 0U;
                            if (Line[0].StitchX > LinePrev[0].StitchX) {    // direction (old) ---> (new)
                                if (Line[0].StitchX != ((959U - SVC_EMA_RESTRICT_WIDTH) - SVC_EMA_BLENDING_HALF_WIDTH)) {   // default line
                                    if ((Line[0].StitchX - LinePrev[0].StitchX) < SVC_EMA_BLENDING_LINE_WIDTH) {
                                        Abandon[0] = 1U;
                                    }
                                }
                            } else {                                        // direction (new) <--- (old)
                                if ((LinePrev[0].StitchX - Line[0].StitchX) < SVC_EMA_BLENDING_LINE_WIDTH) {
                                    Abandon[0] = 1U;
                                }
                            }

                            Abandon[1] = 0U;
                            if (Line[1].StitchX > LinePrev[1].StitchX) {    // direction (old) ---> (new)
                                if ((Line[1].StitchX - LinePrev[1].StitchX) < SVC_EMA_BLENDING_LINE_WIDTH) {
                                    Abandon[1] = 1U;
                                }
                            } else {                                        // direction (new) <--- (old)
                                if (Line[1].StitchX != ((960U + SVC_EMA_RESTRICT_WIDTH) + SVC_EMA_BLENDING_HALF_WIDTH)) {   // default line
                                    if ((LinePrev[1].StitchX - Line[1].StitchX) < SVC_EMA_BLENDING_LINE_WIDTH) {
                                        Abandon[1] = 1U;
                                    }
                                }
                            }
                        }

                        /* Update EmaCtrl.Line */
                        if (SVC_OK == AmbaKAL_MutexTake(&(EmaCtrl.LineMtx), AMBA_KAL_WAIT_FOREVER)) {
                            if (Abandon[0] == 0U) {
                                if (SVC_OK != AmbaWrap_memcpy(&EmaCtrl.Line[0], &Line[0], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s))) {
                                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy EmaCtrl.Line[0] failed", 0U, 0U);
                                }
                            } else {
                                if (SVC_OK != AmbaWrap_memcpy(&Line[0], &LinePrev[0], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s))) {
                                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy Line[0] failed", 0U, 0U);
                                }
                            }
                            if (Abandon[1] == 0U) {
                                if (SVC_OK != AmbaWrap_memcpy(&EmaCtrl.Line[1], &Line[1], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s))) {
                                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy EmaCtrl.Line[1] failed", 0U, 0U);
                                }
                            } else {
                                if (SVC_OK != AmbaWrap_memcpy(&Line[1], &LinePrev[1], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s))) {
                                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy Line[1] failed", 0U, 0U);
                                }
                            }

                            RetVal = AmbaKAL_MutexGive(&(EmaCtrl.LineMtx));
                            if (SVC_OK != RetVal) {
                                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexGive LineMtx failed", 0U, 0U);
                            }
                        }
                    } else {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "Mutex take err. Failed to update GetLine param", 0U, 0U);
                    }

                    break;
                }
            }
        }
    }

    return NULL;
}

static void* SvcEmrAdaptive_StitchLineTaskEntry(void* EntryArg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlags = 0U;
    INT32  CmpResult = 0;

    /* Used for calculating stitch line */
    AMBA_CAL_ROI_s                  RestrictArea[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_STITCH_LINE_INFO_s Line[SVC_EMA_OVERLAP_AREA_NUM];

    /* Used for calculating blend table */
    UINT32 BufSize;
    ULONG  BufBase;
    UINT8 *pBuf;
    AMBA_CAL_EMA_BLEND_TBL_s  BlendTbl;
    AMBA_CAL_EMA_BLEND_CFG_s  NewBlendCfg[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_BLEND_CFG_s *pNewBlendCfg[SVC_EMA_OVERLAP_AREA_NUM];
    const ULONG *pArg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);

    RetVal |= AmbaWrap_memset(&BlendTbl, 0, sizeof(AMBA_CAL_EMA_BLEND_TBL_s));
    RetVal |= AmbaWrap_memset(NewBlendCfg, 0, sizeof(NewBlendCfg));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memset BlendTbl/NewBlendCfg failed", 0U, 0U);
    }

    RetVal = AmbaCal_EmaGetBlendTableSize(&EmaCtrl.View, &BlendTbl.Size);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaGetBlendTableSize failed", 0U, 0U);
    }

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_BLENDING, &BufBase, &BufSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcBuffer_Request failed", 0U, 0U);
    }

    BufBase += sizeof(AMBA_DSP_BUF_s);
    AmbaMisra_TypeCast(&pBuf, &BufBase);
    BlendTbl.pAddr = pBuf;

    pNewBlendCfg[0] = &NewBlendCfg[0];
    pNewBlendCfg[1] = &NewBlendCfg[1];

    NewBlendCfg[0].X1     = (EmaCtrl.Line[0].StitchX - EmaCtrl.View.VoutAreaBack.StartX) - SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[0].X2     = (EmaCtrl.Line[0].StitchX - EmaCtrl.View.VoutAreaBack.StartX) + SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[0].Alpha  = 128U;
    NewBlendCfg[0].Margin = SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[1].X1     = (EmaCtrl.Line[1].StitchX - EmaCtrl.View.VoutAreaBack.StartX) - SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[1].X2     = (EmaCtrl.Line[1].StitchX - EmaCtrl.View.VoutAreaBack.StartX) + SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[1].Alpha  = 128U;
    NewBlendCfg[1].Margin = SVC_EMA_BLENDING_HALF_WIDTH;

    RetVal |= AmbaWrap_memcpy(RestrictArea, EmaCtrl.RestrictArea, sizeof(RestrictArea));
    RetVal |= AmbaWrap_memcpy(Line, EmaCtrl.Line, sizeof(Line));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy RestrictArea/Line failed", 0U, 0U);
    }

    while ((*pArg) == 0U) {
        if (SVC_OK == AmbaKAL_EventFlagGet(&SvcEmaYuvFlag, SVC_EMA_YUV_DATA_READY, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_WAIT_FOREVER)) {
            if (SVC_OK == AmbaKAL_MutexTake(&(EmaCtrl.LineMtx), AMBA_KAL_WAIT_FOREVER)) {

                /* compare stitch line */
                if (SVC_OK != AmbaWrap_memcmp(&Line[0], &EmaCtrl.Line[0], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s), &CmpResult)) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcmp Line[0]/EmaCtrl.Line[0] failed", 0U, 0U);
                }
                if (CmpResult != 0) {
                    if (RetVal != AmbaWrap_memcpy(&Line[0], &EmaCtrl.Line[0], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s))) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy Line[0] failed", 0U, 0U);
                    }
                }

                if (SVC_OK != AmbaWrap_memcmp(&Line[1], &EmaCtrl.Line[1], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s), &CmpResult)) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcmp Line[1]/EmaCtrl.Line[1] failed", 0U, 0U);
                }
                if (CmpResult != 0) {
                    if (SVC_OK != AmbaWrap_memcpy(&Line[1], &EmaCtrl.Line[1], sizeof(AMBA_CAL_EMA_STITCH_LINE_INFO_s))) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy Line[1] failed", 0U, 0U);
                    }
                }

                RetVal = AmbaKAL_MutexGive(&(EmaCtrl.LineMtx));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexGive LineMtx failed", 0U, 0U);
                }
            }

            EmrAdaptive_BlendCfg(Line, NewBlendCfg);
            if (SVC_OK != AmbaCal_EmaGenBlendTable(pNewBlendCfg, &BlendTbl)) {
                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaGenBlendTable failed", 0U, 0U);
            } else {
                RetVal = SvcPlat_CacheClean(BufBase, BufSize);
                if (SVC_OK != RetVal) {
                    // SvcLog_NG(SVC_LOG_EMA_TASK, "SvcPlat_CacheClean failed", 0U, 0U);
                }

                if (SVC_OK == AmbaKAL_MutexTake(&(EmaCtrl.OsdMtx), AMBA_KAL_WAIT_FOREVER)) {
                    if (SVC_OK != AmbaWrap_memcpy(EmaCtrl.OsdLine, Line, sizeof(Line))) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy EmaCtrl.OsdLine failed", 0U, 0U);
                    }

                    RetVal = AmbaKAL_EventFlagSet(&SvcEmaDrawFlag, (SVC_EMA_DRAW_DATA_READY | SVC_EMA_DRAW_UPDATE));
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_EventFlagSet SvcEmaDrawFlag failed", 0U, 0U);
                    }

                    RetVal = AmbaKAL_MutexGive(&(EmaCtrl.OsdMtx));
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexGive OsdMtx failed", 0U, 0U);
                    }
                }
            }
        }
    }

    return NULL;
}

static void EmrAdaptive_BlendCfg(const AMBA_CAL_EMA_STITCH_LINE_INFO_s *pLine, AMBA_CAL_EMA_BLEND_CFG_s *pNewBlendCfg)
{
    pNewBlendCfg[1].X1     = ((1919U - pLine[0].StitchX) - EmaCtrl.View.VoutAreaBack.StartX) - SVC_EMA_BLENDING_HALF_WIDTH;
    pNewBlendCfg[1].X2     = ((1919U - pLine[0].StitchX) - EmaCtrl.View.VoutAreaBack.StartX) + SVC_EMA_BLENDING_HALF_WIDTH;
    pNewBlendCfg[1].Alpha  = 128U;
    pNewBlendCfg[1].Margin = SVC_EMA_BLENDING_HALF_WIDTH;

    pNewBlendCfg[0].X1     = ((1919U - pLine[1].StitchX) - EmaCtrl.View.VoutAreaBack.StartX) - SVC_EMA_BLENDING_HALF_WIDTH;
    pNewBlendCfg[0].X2     = ((1919U - pLine[1].StitchX) - EmaCtrl.View.VoutAreaBack.StartX) + SVC_EMA_BLENDING_HALF_WIDTH;
    pNewBlendCfg[0].Alpha  = 128U;
    pNewBlendCfg[0].Margin = SVC_EMA_BLENDING_HALF_WIDTH;
}

/**
 * Start emr adaptive task
 * @return 0-OK, 1-NG
 */
UINT32 SvcEmrAdaptiveTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    static char SvcEmaBbxFlagName[]   = "SvcEmaBbxFlag";
    static char SvcEmaYuvFlagName[]   = "SvcEmaYuvFlag";
    static char SvcEmaDrawFlagName[]  = "SvcEmaDrawFalg";
    static char SvcEmaBbxMutexName[]  = "SvcEmaBbxMutex";
    static char SvcEmaLineMutexName[] = "SvcEmaLineMutex";
    static char SvcEmaOsdMutexName[]  = "SvcEmaOsdMutex";

    UINT32 CvFlowNum  = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;


    if ((pResCfg->UserFlag & SVC_EMR_STITCH) > 0U) {
        SvcLog_DBG(SVC_LOG_EMA_TASK, "SvcEmrAdaptiveTask starts", 0U, 0U);

        RetVal = SvcODDrawTask_Enable(0U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcODDrawTask_Enable(0) failed", 0U, 0U);
        }

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Register(i, EmrAdaptiveTask_RefODResult, &EmrAdaptiveCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcCvFlow_Register failed", 0U, 0U);
                    }
                }
            }
        }

        RetVal = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, EmrAdaptiveTask_YuvDataRdy);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaDSP_EventHandlerRegister failed", 0U, 0U);
        }

        RetVal |= AmbaKAL_EventFlagCreate(&SvcEmaYuvFlag, SvcEmaYuvFlagName);
        RetVal |= AmbaKAL_EventFlagCreate(&SvcEmaBbxFlag, SvcEmaBbxFlagName);
        RetVal |= AmbaKAL_EventFlagCreate(&SvcEmaDrawFlag, SvcEmaDrawFlagName);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_EventFlagCreate failed", 0U, 0U);
        }

        RetVal |= AmbaKAL_MutexCreate(&EmaCtrl.BbxMtx, SvcEmaBbxMutexName);
        RetVal |= AmbaKAL_MutexCreate(&EmaCtrl.LineMtx, SvcEmaLineMutexName);
        RetVal |= AmbaKAL_MutexCreate(&EmaCtrl.OsdMtx, SvcEmaOsdMutexName);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexCreate failed", 0U, 0U);
        }

        EmaCtrl.BbxInfoTaskCtrl.Priority   = SVC_EMR_ADAPTIVE_TASK_PRI;
        EmaCtrl.BbxInfoTaskCtrl.EntryFunc  = SvcEmrAdaptvie_BbxInfoTaskEntry;
        EmaCtrl.BbxInfoTaskCtrl.EntryArg   = 0U;
        EmaCtrl.BbxInfoTaskCtrl.pStackBase = EmaCtrl.BbxInfoTaskStack;
        EmaCtrl.BbxInfoTaskCtrl.StackSize  = SVC_EMR_ADAPTIVE_TASK_STACK_SIZE;
        EmaCtrl.BbxInfoTaskCtrl.CpuBits    = SVC_EMR_ADAPTIVE_TASK_CPU_BITS;

        EmaCtrl.StitchLineTaskCtrl.Priority   = SVC_EMR_ADAPTIVE_TASK_PRI + 1U;
        EmaCtrl.StitchLineTaskCtrl.EntryFunc  = SvcEmrAdaptive_StitchLineTaskEntry;
        EmaCtrl.StitchLineTaskCtrl.EntryArg   = 0U;
        EmaCtrl.StitchLineTaskCtrl.pStackBase = EmaCtrl.StitchLineTaskStack;
        EmaCtrl.StitchLineTaskCtrl.StackSize  = SVC_EMR_ADAPTIVE_TASK_STACK_SIZE;
        EmaCtrl.StitchLineTaskCtrl.CpuBits    = SVC_EMR_ADAPTIVE_TASK_CPU_BITS;

        RetVal = SvcTask_Create("SvcEmrAdaptBbx", &EmaCtrl.BbxInfoTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcTask_Create SvcEmrAdaptBbx failed", 0U, 0U);
        }

        RetVal = SvcTask_Create("SvcEmrAdaptLine", &EmaCtrl.StitchLineTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcTask_Create SvcEmrAdaptLine failed", 0U, 0U);
        }

        SvcGui_Register(VOUT_IDX_A, EMA_GUI_LEVEL, "Ema", EmrAdaptiveTask_DrawStitchLineCB, EmrAdaptiveTask_DrawUpdate);

        SvcLog_DBG(SVC_LOG_EMA_TASK, "SvcEmrAdaptiveTask ends", 0U, 0U);
    }

    return RetVal;
}

/**
 * Stop emr adaptive task
 * @return 0-OK, 1-NG
 */
UINT32 SvcEmrAdaptiveTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum  = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    EmaCtrl.PrintFlag = 0U;

    if ((pResCfg->UserFlag & SVC_EMR_STITCH) > 0U) {

        RetVal = SvcODDrawTask_Enable(1U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcODDrawTask_Enable(1) failed", 0U, 0U);
        }

        RetVal = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, EmrAdaptiveTask_YuvDataRdy);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaDSP_EventHandlerUnRegister failed", 0U, 0U);
        }

        RetVal |= SvcTask_Destroy(&EmaCtrl.BbxInfoTaskCtrl);
        RetVal |= SvcTask_Destroy(&EmaCtrl.StitchLineTaskCtrl);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "SvcTask_Destroy failed", 0U, 0U);
        }

        RetVal |= AmbaKAL_EventFlagDelete(&SvcEmaYuvFlag);
        RetVal |= AmbaKAL_EventFlagDelete(&SvcEmaBbxFlag);
        RetVal |= AmbaKAL_EventFlagDelete(&SvcEmaDrawFlag);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_EventFlagDelete failed", 0U, 0U);
        }

        RetVal |= AmbaKAL_MutexDelete(&EmaCtrl.BbxMtx);
        RetVal |= AmbaKAL_MutexDelete(&EmaCtrl.LineMtx);
        RetVal |= AmbaKAL_MutexDelete(&EmaCtrl.OsdMtx);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexDelete failed", 0U, 0U);
        }

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_OD_FDAG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_OD_FDAG)) {
                    RetVal = SvcCvFlow_Unregister(i, EmrAdaptiveCvFlowRegisterID[i]);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcCvFlow_Unregister failed %d", RetVal, 0U);
                    }
                }
            }
        }

        SvcGui_Unregister(VOUT_IDX_A, EMA_GUI_LEVEL);
    }

    return RetVal;
}

static void EmrAdaptiveTask_DetectionResultHandler(UINT32 CvFlowChan, void *pDetResMsg)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ResMsg;
    UINT32 BufIdx;

    AmbaMisra_TypeCast(&ResMsg, pDetResMsg);

    if (ResMsg == SVC_CV_DETRES_BBX) {
        const SVC_CV_DETRES_BBX_LIST_s *pList;
        AMBA_OD_2DBBX_s Bbx;
        UINT32 i, FovIdx;

        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pList, &pDetResMsg);
        if (pList->Source < SVC_EMA_CHAN_NUM) {
            FovIdx = pList->Source;
            BufIdx = pList->FrameNum % SVC_EMA_BBX_BUFFER_NUM;

            if (SVC_OK == AmbaKAL_MutexTake(&(EmaCtrl.BbxMtx), AMBA_KAL_WAIT_FOREVER)) {
                EmaBbxResult.CvFlowChan[2U - FovIdx]                  = CvFlowChan;
                EmaBbxResult.BbxInfo[BufIdx][2U - FovIdx].CaptureTime = pList->CaptureTime;
                EmaBbxResult.BbxInfo[BufIdx][2U - FovIdx].MsgCode     = pList->MsgCode;
                EmaBbxResult.BbxInfo[BufIdx][2U - FovIdx].FrameNum    = pList->FrameNum;
                EmaBbxResult.BbxInfo[BufIdx][2U - FovIdx].NumBbx      = pList->BbxAmount;

                for (i = 0; i < pList->BbxAmount; i++) {
                    Bbx.X   = pList->Bbx[i].X;
                    Bbx.Y   = pList->Bbx[i].Y;
                    Bbx.W   = pList->Bbx[i].W;
                    Bbx.H   = pList->Bbx[i].H;
                    Bbx.Cat = pList->Bbx[i].Cat;
                    EmrAdaptiveTask_ResizeBbx(FovIdx, &Bbx);

                    if (SVC_OK != AmbaWrap_memcpy(&(EmaBbxResult.BbxInfo[BufIdx][2U - FovIdx].Bbx[i]), &Bbx, sizeof(AMBA_OD_2DBBX_s))) {
                        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy EmaBbxResult.BbxInfo[BufIdx][2U - FovIdx].Bbx[i] failed", 0U, 0U);
                    }
                }
                if (SVC_OK != AmbaWrap_memcpy(&(EmaBbxResult.BbxList[BufIdx][2U - FovIdx]), pList, sizeof(SVC_CV_DETRES_BBX_LIST_s))) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy EmaBbxResult.BbxList[BufIdx][2U - FovIdx] failed", 0U, 0U);
                }

                RetVal = AmbaKAL_EventFlagSet(&SvcEmaBbxFlag, ((UINT32) 1U << BufIdx));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_EventFlagSet SvcEmaBbxFlag failed", 0U, 0U);
                }

                RetVal = AmbaKAL_MutexGive(&(EmaCtrl.BbxMtx));
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexGive BbxMtx failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_EMA_TASK, "Mutex Get err. Failed to update bbx info", 0U, 0U);
            }
        }
    }
}

static void EmrAdaptiveTask_RefODResult(UINT32 CvFlowChan, UINT32 CvType, void *pOutput)
{
    AmbaMisra_TouchUnused(&CvType);

    EmrAdaptiveTask_DetectionResultHandler(CvFlowChan, pOutput);
}

static void EmrAdaptiveTask_ResizeBbx(UINT32 FovIdx, AMBA_OD_2DBBX_s *pBbx)
{
    pBbx->X = (UINT16) ((pBbx->X * EmaWindow[FovIdx].DstWidth) / EmaWindow[FovIdx].MainWidth);
    pBbx->Y = (UINT16) ((pBbx->Y * EmaWindow[FovIdx].DstHeight) / EmaWindow[FovIdx].MainHeight);
    pBbx->W = (UINT16) ((pBbx->W * EmaWindow[FovIdx].DstWidth) / EmaWindow[FovIdx].MainWidth);
    pBbx->H = (UINT16) ((pBbx->H * EmaWindow[FovIdx].DstHeight) / EmaWindow[FovIdx].MainHeight);

    /* hor flip */
    pBbx->X = (UINT16) (((EmaWindow[FovIdx].DstWidth - 1U) - pBbx->X) - pBbx->W);
}

static void EmrAdaptiveTask_FilterBbx(AMBA_OD_2DBBX_LIST_s *pBbxList)
{
    UINT32 NewNumBbx = 0U;
    UINT32 LeftBound, RightBound;
    UINT32 idx;

    if (EmaCtrl.FilterWidth != 0U) {
        LeftBound  = EmaCtrl.FilterWidth;
        RightBound = EmaCtrl.View.VoutAreaBack.Width - EmaCtrl.FilterWidth;

        for (idx = 0; idx < pBbxList->NumBbx; idx++) {
            /* vehicles inside target ROI */
            if ((pBbxList->Bbx[idx].Cat == 4U) || (pBbxList->Bbx[idx].Cat == 5U)) {
                if ((pBbxList->Bbx[idx].X < (UINT16) LeftBound) || ((pBbxList->Bbx[idx].X + pBbxList->Bbx[idx].W) > (UINT16) RightBound)) {
                    continue;
                }
            }
            if (SVC_OK != AmbaWrap_memcpy(&pBbxList->Bbx[NewNumBbx], &pBbxList->Bbx[idx], sizeof(AMBA_OD_2DBBX_s))) {
                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy pBbxList->Bbx[NewNumBbx] failed", 0U, 0U);
            }
            NewNumBbx++;
        }
        pBbxList->NumBbx = NewNumBbx;
    }
}

static UINT32 EmrAdaptiveTask_YuvDataRdy(const void *pEventData)
{
    UINT32 RetVal = SVC_OK;
    const  AMBA_DSP_YUV_DATA_RDY_s *pYuvDataReady = NULL;

    AmbaMisra_TypeCast(&pYuvDataReady, &pEventData);
    if (pYuvDataReady->ViewZoneId == 0x80000000UL) {
        RetVal = AmbaKAL_EventFlagSet(&SvcEmaYuvFlag, SVC_EMA_YUV_DATA_READY);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_EventFlagSet SvcEmaYuvFlag failed", 0U, 0U);
        }
    }

    return SVC_OK;
}

/**
 * Set and apply default blend table
 * @return 0-OK, 1-NG
 */
UINT32 SvcEmrAdaptiveTask_SetDefaultBldTbl(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 BufSize;
    ULONG  BufBase;
    UINT8 *pBuf;
    AMBA_CAL_EMA_BLEND_TBL_s  BlendTbl;
    AMBA_CAL_EMA_BLEND_CFG_s  NewBlendCfg[SVC_EMA_OVERLAP_AREA_NUM];
    AMBA_CAL_EMA_BLEND_CFG_s *pNewBlendCfg[SVC_EMA_OVERLAP_AREA_NUM];

    RetVal |= AmbaWrap_memset(&BlendTbl, 0, sizeof(AMBA_CAL_EMA_BLEND_TBL_s));
    RetVal |= AmbaWrap_memset(NewBlendCfg, 0, sizeof(NewBlendCfg));
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memset BlendTbl/NewBlendCfg failed", 0U, 0U);
    }

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_BLENDING, &BufBase, &BufSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcBuffer_Request failed", 0U, 0U);
    }

    RetVal = AmbaCal_EmaGetBlendTableSize(&EmaCtrl.View, &BlendTbl.Size);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaGetBlendTableSize failed", 0U, 0U);
    }

    BufBase += sizeof(AMBA_DSP_BUF_s);
    AmbaMisra_TypeCast(&pBuf, &BufBase);
    BlendTbl.pAddr = pBuf;

    NewBlendCfg[0].X1     = (EmaCtrl.Line[0].StitchX - EmaCtrl.View.VoutAreaBack.StartX) - SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[0].X2     = (EmaCtrl.Line[0].StitchX - EmaCtrl.View.VoutAreaBack.StartX) + SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[0].Alpha  = 128U;
    NewBlendCfg[0].Margin = SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[1].X1     = (EmaCtrl.Line[1].StitchX - EmaCtrl.View.VoutAreaBack.StartX) - SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[1].X2     = (EmaCtrl.Line[1].StitchX - EmaCtrl.View.VoutAreaBack.StartX) + SVC_EMA_BLENDING_HALF_WIDTH;
    NewBlendCfg[1].Alpha  = 128U;
    NewBlendCfg[1].Margin = SVC_EMA_BLENDING_HALF_WIDTH;

    pNewBlendCfg[0] = &NewBlendCfg[0];
    pNewBlendCfg[1] = &NewBlendCfg[1];

    RetVal = AmbaCal_EmaGenBlendTable(&(pNewBlendCfg[0]), &BlendTbl);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaGenBlendTable failed", 0U, 0U);
    }

    return RetVal;
}

static void EmrAdaptiveTask_DrawStitchLineCB(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal = SVC_OK;
    UINT32 ActualFlags = 0U;
    static AMBA_CAL_EMA_STITCH_LINE_INFO_s LineInfo[SVC_EMA_OVERLAP_AREA_NUM] = {
        [0] = {
            .StitchX = (959U - SVC_EMA_RESTRICT_WIDTH) - SVC_EMA_BLENDING_HALF_WIDTH,
            .Status  = AMBA_CAL_STITCH_LINE_STATUS_MUST
        },
        [1] = {
            .StitchX = (960U + SVC_EMA_RESTRICT_WIDTH) + SVC_EMA_BLENDING_HALF_WIDTH,
            .Status  = AMBA_CAL_STITCH_LINE_STATUS_MUST
        }
    };

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if (SVC_OK == AmbaKAL_MutexTake(&(EmaCtrl.OsdMtx), AMBA_KAL_WAIT_FOREVER)) {
        if (SVC_OK == AmbaKAL_EventFlagGet(&SvcEmaDrawFlag, SVC_EMA_DRAW_DATA_READY, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_NO_WAIT)) {
            if (SVC_OK != AmbaWrap_memcpy(LineInfo, EmaCtrl.OsdLine, sizeof(EmaCtrl.OsdLine))) {
                SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaWrap_memcpy LineInfo failed", 0U, 0U);
            }
        }
        RetVal = AmbaKAL_MutexGive(&(EmaCtrl.OsdMtx));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaKAL_MutexGive OsdMtx failed", 0U, 0U);
        }
    }

    /* draw stitch line */
    if ((EmaCtrl.OsdFlag & ((UINT32)1U << 1U)) > 0U) {
        RetVal |= SvcOsd_DrawSolidRect(VOUT_IDX_A, LineInfo[0].StitchX - SVC_EMA_BLENDING_HALF_WIDTH, 0U, LineInfo[0].StitchX + SVC_EMA_BLENDING_HALF_WIDTH, 480U, SVC_EMA_COLOR_LEFT);
        RetVal |= SvcOsd_DrawSolidRect(VOUT_IDX_A, LineInfo[1].StitchX - SVC_EMA_BLENDING_HALF_WIDTH, 0U, LineInfo[1].StitchX + SVC_EMA_BLENDING_HALF_WIDTH, 480U, SVC_EMA_COLOR_RIGHT);
    }

    /* draw ROI triangle and stitch line boundary */
    if ((EmaCtrl.OsdFlag & (UINT32) 1U) > 0U) {
        RetVal |= SvcOsd_DrawLine(VOUT_IDX_A, 779U, 390U, EmaCtrl.FilterRule.Line[0].X2, EmaCtrl.FilterRule.Line[0].Y2, 1U, SVC_EMA_COLOR_TRI);
        RetVal |= SvcOsd_DrawLine(VOUT_IDX_A, EmaCtrl.FilterRule.Line[1].X1, EmaCtrl.FilterRule.Line[1].Y1, 1140U, 390U, 1U, SVC_EMA_COLOR_TRI);
        RetVal |= SvcOsd_DrawSolidRect(VOUT_IDX_A, (959U - EmaCtrl.MaxStchLineOffset[0]) - 1U, 0U, (959U - EmaCtrl.MaxStchLineOffset[0]) + 1U, 479U, SVC_EMA_COLOR_TRI);
        RetVal |= SvcOsd_DrawSolidRect(VOUT_IDX_A, (960U + EmaCtrl.MaxStchLineOffset[1]) - 1U, 0U, (960U + EmaCtrl.MaxStchLineOffset[1]) + 1U, 479U, SVC_EMA_COLOR_TRI);
    }
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcOsd_DrawSolidRect/SvcOsd_DrawLine failed", 0U, 0U);
    }
}

static void EmrAdaptiveTask_DrawUpdate(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    UINT32 ActualFlags = 0U;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    *pUpdate = 0U;

    if (SVC_OK == AmbaKAL_EventFlagGet(&SvcEmaDrawFlag, SVC_EMA_DRAW_UPDATE, AMBA_KAL_FLAGS_ANY, AMBA_KAL_FLAGS_CLEAR_AUTO, &ActualFlags, AMBA_KAL_NO_WAIT)) {
        *pUpdate = 1U;
    }
}

static UINT32 EmrAdaptiveTask_SetTracker(void)
{
    UINT32 RetVal;
    static UINT32 EmaTrackerBufSize;
    static ULONG  EmaTrackerWorkBuf;
    const  void *pEmaTrackerWorkBuf;
    static AMBA_SR_NN_CAT_TO_OBJ_TYPE_LUT_s NNCat2ObjTypeLut;
    static AMBA_SR_NN_CAT_TO_OBJ_TYPE_s     Lut[12];

    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_EMA_TRACKER, &EmaTrackerWorkBuf, &EmaTrackerBufSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcBuffer_Request failed", 0U, 0U);
    }

    AmbaMisra_TypeCast(&pEmaTrackerWorkBuf, &EmaTrackerWorkBuf);

    RetVal = AmbaOD_CreateTrackerHandlerV1(SR_CAMERA_POS_BACK, SR_EMIRROR, pEmaTrackerWorkBuf, &EmaCtrl.pTrackerHandler);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaOD_CreateTrackerHandler failed", 0U, 0U);
    } else {
        Lut[0].NNCat   = 0U;
        Lut[0].ObjType = SR_OBJ_TYPE_PERSON_0;

        Lut[1].NNCat   = 1U;
        Lut[1].ObjType = SR_OBJ_TYPE_SCOOTER_0;
        Lut[2].NNCat   = 2U;
        Lut[2].ObjType = SR_OBJ_TYPE_SCOOTER_0;
        Lut[3].NNCat   = 3U;
        Lut[3].ObjType = SR_OBJ_TYPE_SCOOTER_0;

        Lut[4].NNCat   = 4U;
        Lut[4].ObjType = SR_OBJ_TYPE_VEHICLE_0;
        Lut[5].NNCat   = 5U;
        Lut[5].ObjType = SR_OBJ_TYPE_VEHICLE_0;

        NNCat2ObjTypeLut.pLut      = Lut;
        NNCat2ObjTypeLut.LutLength = 6U;
        AmbaSR_SetNNCat2ObjTypeLut(&NNCat2ObjTypeLut);
    }

    return RetVal;
}

static void EmrAdaptiveTask_SetDistance(void)
{
    UINT32 RetVal;
    UINT32 FuncRetVal = ADAS_ERR_NONE;
    UINT32 i;
    static UINT32 EmrCalibBufSize;
    static ULONG  EmrCalibWorkBuf;
    static void *pEmrCalibWorkBuf;

    static AMBA_CAL_EM_CALIB_INFO_CFG_s EmCalCfg;
    static AMBA_CAL_EM_POINT_MAP_s EmCalibPointMap;
    static AMBA_CAL_LENS_DST_REAL_EXPECT_s RealExpect;
    DOUBLE CamPosX;
    DOUBLE CamPosY;
    DOUBLE CamPosZ;
    DOUBLE OptCenterX;
    DOUBLE OptCenterY;
    DOUBLE WorldX[4];
    DOUBLE WorldY[4];
    DOUBLE WorldZ[4];
    DOUBLE RawX[4];
    DOUBLE RawY[4];
    UINT32 TblLen_L;
    static const AMBA_SR_OBJECT_SIZE_s ToyotaYarisSize = {.Length = 3465U, .Width = 1615U, .Height = 1460U};
    static const AMBA_SR_OBJECT_SIZE_s CorolllaAltisSize = {.Length = 4540U, .Width = 1760U, .Height = 1465U};
    static const AMBA_SR_OBJECT_SIZE_s RavSize = {.Length = 4605U, .Width = 1845U, .Height = 1675U};
    static const AMBA_SR_OBJECT_SIZE_s HondaCrV2018Size = {.Length = 4600U, .Width = 1855U, .Height = 1689U};
    static const AMBA_SR_OBJECT_SIZE_s HondaCrV2015Size = {.Length = 4605U, .Width = 1820U, .Height = 1685U};
    static const AMBA_SR_OBJECT_SIZE_s CmcVerycaFreezerVanSize = {.Length = 4100U, .Width = 1560U, .Height = 1895U};
    static const AMBA_SR_OBJECT_SIZE_s CmcVerycaSemiPanelVanSize = {.Length = 4105U, .Width = 1560U, .Height = 1930U};
    static const AMBA_SR_OBJECT_SIZE_s CmcVerycaPassengerVanSize = {.Length = 4105U, .Width = 1560U, .Height = 1930U};
    static const AMBA_SR_OBJECT_SIZE_s CmcVerycaPickupSize = {.Length = 4100U, .Width = 1560U, .Height = 1895U};
    static const AMBA_SR_OBJECT_SIZE_s ToyotaSientaSize = {.Length = 4100U, .Width = 1695U, .Height = 1670U};
    static const AMBA_SR_OBJECT_SIZE_s NissanKicksSize = {.Length = 4295U, .Width = 1760U, .Height = 1585U};
    static const AMBA_SR_OBJECT_SIZE_s ToyotaNewCorollaSize = {.Length = 4620U, .Width = 1775U, .Height = 1465U};
    static const AMBA_SR_OBJECT_SIZE_s HondaCivicSize = {.Length = 4525U, .Width = 1755U, .Height = 1435U};
    static const AMBA_SR_OBJECT_SIZE_s NissanNewSyiphySize = {.Length = 4615U, .Width = 1760U, .Height = 1495U};
    static const AMBA_SR_OBJECT_SIZE_s HyundaiElantraSize = {.Length = 4530U, .Width = 1775U, .Height = 1445U};
    static const AMBA_SR_OBJECT_SIZE_s ProtonPreveSize = {.Length = 4543U, .Width = 1786U, .Height = 1524U};
    static const AMBA_SR_OBJECT_SIZE_s HINOBusSize = {.Length = 12480U, .Width = 2430U, .Height = 3058U};
    static const AMBA_SR_OBJECT_SIZE_s MitsubishiJakartaFusoTruckSize = {.Length = 8410U, .Width = 2500U, .Height = 3300U};
    static const AMBA_SR_OBJECT_SIZE_s MitsubishiFuso4M50Size = {.Length = 7730U, .Width = 2070U, .Height = 2755U};
    static const AMBA_SR_OBJECT_SIZE_s ToyotaCamrySize = {.Length = 4825U, .Width = 1825U, .Height = 1470U};

    static DOUBLE RealTbl[] = {
        0.000000, 0.032474, 0.064948, 0.097421, 0.129893, 0.162366, 0.194839, 0.227313, 0.259788, 0.292264,
        0.324742, 0.357221, 0.389702, 0.422186, 0.454673, 0.487163, 0.519656, 0.552153, 0.584654, 0.617159,
        0.649670, 0.682185, 0.714706, 0.747232, 0.779765, 0.812304, 0.844850, 0.877404, 0.909965, 0.942534,
        0.975111, 1.007698, 1.040294, 1.072899, 1.105514, 1.138140, 1.170777, 1.203426, 1.236086, 1.268758,
        1.301443, 1.334141, 1.366853, 1.399579, 1.432319, 1.465074, 1.497845, 1.530632, 1.563435, 1.596256,
        1.629096, 1.661959, 1.694840, 1.727739, 1.760656, 1.793591, 1.826546, 1.859520, 1.892514, 1.925529,
        1.958565, 1.991623
    };
    static DOUBLE ExpectTbl[] = {
        0.000000, 0.032475, 0.064954, 0.097439, 0.129935, 0.162444, 0.194971, 0.227517, 0.260088, 0.292687,
        0.325316, 0.357979, 0.390681, 0.423423, 0.456211, 0.489047, 0.521935, 0.554878, 0.587881, 0.620946,
        0.654078, 0.687280, 0.720556, 0.753910, 0.787345, 0.820865, 0.854475, 0.888177, 0.921977, 0.955877,
        0.989882, 1.023996, 1.058224, 1.092569, 1.127035, 1.161627, 1.196350, 1.231207, 1.266203, 1.301343,
        1.336631, 1.372072, 1.407670, 1.443431, 1.479359, 1.515460, 1.551738, 1.588198, 1.624846, 1.661688,
        1.698727, 1.735971, 1.773425, 1.811094, 1.848984, 1.887102, 1.925453, 1.964043, 2.002879, 2.041968,
        2.081316, 2.120929
    };

    {
        for (UINT32 NNCat = 0U; NNCat < 6U; NNCat++) {
            if ((NNCat == 4U) || (NNCat == 5U)) { /* vehicle, refer to SvcCvFlow_AmbaOD.c for Cat id */
                const struct {
                    UINT32 ObjCat;
                    const AMBA_SR_OBJECT_SIZE_s *pSize;
                } CarModelInfo[20] = {
                    {NNCat, &ToyotaYarisSize},
                    {NNCat, &CorolllaAltisSize},
                    {NNCat, &RavSize},
                    {NNCat, &HondaCrV2018Size},
                    {NNCat, &HondaCrV2015Size},
                    {NNCat, &CmcVerycaFreezerVanSize},
                    {NNCat, &CmcVerycaSemiPanelVanSize},
                    {NNCat, &CmcVerycaPassengerVanSize},
                    {NNCat, &CmcVerycaPickupSize},
                    {NNCat, &ToyotaSientaSize},
                    {NNCat, &NissanKicksSize},
                    {NNCat, &ToyotaNewCorollaSize},
                    {NNCat, &HondaCivicSize},
                    {NNCat, &NissanNewSyiphySize},
                    {NNCat, &HyundaiElantraSize},
                    {NNCat, &ProtonPreveSize},
                    {NNCat, &HINOBusSize},
                    {NNCat, &MitsubishiJakartaFusoTruckSize},
                    {NNCat, &MitsubishiFuso4M50Size},
                    {NNCat, &ToyotaCamrySize},
                };

                for (i = 0U; i < 20U; i++) {
                    FuncRetVal |= AmbaSR_SetObjectSize(CarModelInfo[i].ObjCat, CarModelInfo[i].pSize);
                }
                if (ADAS_ERR_NONE != FuncRetVal) {
                    SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaSR_SetObjectSize() failed", 0U, 0U);
                }
            }
        }
    }

    CamPosX    =     0.0;
    CamPosY    = -1803.0;
    CamPosZ    =  1498.0;
    OptCenterX =   968.0;
    OptCenterY =   543.0;
    WorldX[0]  =  1600.0; WorldY[0] = -6428.0; WorldZ[0] = 1600.0;
    WorldX[1]  =  1600.0; WorldY[1] = -6428.0; WorldZ[1] =  620.0;
    WorldX[2]  = -1600.0; WorldY[2] = -6428.0; WorldZ[2] = 1600.0;
    WorldX[3]  = -1600.0; WorldY[3] = -6428.0; WorldZ[3] =  620.0;
    RawX[0]    =   453.0; RawY[0]   =   298.0;
    RawX[1]    =   468.0; RawY[1]   =   605.0;
    RawX[2]    =  1468.0; RawY[2]   =   286.0;
    RawX[3]    =  1461.0; RawY[3]   =   593.0;

    // Real Expect
    TblLen_L = 62U;
    RealExpect.Length     = TblLen_L;
    RealExpect.pExpectTbl = &ExpectTbl[0];
    RealExpect.pRealTbl   = &RealTbl[0];

    // calib point world
    EmCalibPointMap.CalibPoints[0U].WorldPos.X = WorldX[0U];
    EmCalibPointMap.CalibPoints[0U].WorldPos.Y = WorldY[0U];
    EmCalibPointMap.CalibPoints[0U].WorldPos.Z = WorldZ[0U];
    EmCalibPointMap.CalibPoints[1U].WorldPos.X = WorldX[1U];
    EmCalibPointMap.CalibPoints[1U].WorldPos.Y = WorldY[1U];
    EmCalibPointMap.CalibPoints[1U].WorldPos.Z = WorldZ[1U];
    EmCalibPointMap.CalibPoints[2U].WorldPos.X = WorldX[2U];
    EmCalibPointMap.CalibPoints[2U].WorldPos.Y = WorldY[2U];
    EmCalibPointMap.CalibPoints[2U].WorldPos.Z = WorldZ[2U];
    EmCalibPointMap.CalibPoints[3U].WorldPos.X = WorldX[3U];
    EmCalibPointMap.CalibPoints[3U].WorldPos.Y = WorldY[3U];
    EmCalibPointMap.CalibPoints[3U].WorldPos.Z = WorldZ[3U];

    // calib point raw
    EmCalibPointMap.CalibPoints[0U].RawPos.X = RawX[0U];
    EmCalibPointMap.CalibPoints[0U].RawPos.Y = RawY[0U];
    EmCalibPointMap.CalibPoints[1U].RawPos.X = RawX[1U];
    EmCalibPointMap.CalibPoints[1U].RawPos.Y = RawY[1U];
    EmCalibPointMap.CalibPoints[2U].RawPos.X = RawX[2U];
    EmCalibPointMap.CalibPoints[2U].RawPos.Y = RawY[2U];
    EmCalibPointMap.CalibPoints[3U].RawPos.X = RawX[3U];
    EmCalibPointMap.CalibPoints[3U].RawPos.Y = RawY[3U];

    // EmCalCfg
    EmCalCfg.Cam.Pos.X           = CamPosX;
    EmCalCfg.Cam.Pos.Y           = CamPosY;
    EmCalCfg.Cam.Pos.Z           = CamPosZ;
    EmCalCfg.Cam.OpticalCenter.X = OptCenterX;
    EmCalCfg.Cam.OpticalCenter.Y = OptCenterY;
    EmCalCfg.Cam.Sensor.CellSize = 0.003;
    EmCalCfg.Cam.Sensor.StartX   = 48U;
    EmCalCfg.Cam.Sensor.StartY   = 81U;
    EmCalCfg.Cam.Sensor.Width    = 1920U;
    EmCalCfg.Cam.Sensor.Height   = 1080U;
    EmCalCfg.Cam.Rotation        = AMBA_CAL_ROTATE_0;
    EmCalCfg.Cam.Lens.LensDistoType = AMBA_CAL_LD_REAL_EXPECT_TBL;
    EmCalCfg.Cam.Lens.LensDistoUnit = AMBA_CAL_LD_PIXEL;
    EmCalCfg.Cam.Lens.LensDistoSpec.pRealExpect = &RealExpect;
    EmCalCfg.Type                = AMBA_CAL_EM_3IN1;
    EmCalCfg.VoutArea.StartX     = 480U;
    EmCalCfg.VoutArea.StartY     = 0U;
    EmCalCfg.VoutArea.Width      = 960U;
    EmCalCfg.VoutArea.Height     = 480U;
    EmCalCfg.Calibinfo.p4Point   = &EmCalibPointMap;
    EmCalCfg.Calibinfo.Type      = AMBA_EM_CALIB_4_POINT;
    EmCalCfg.OptimizeLevel       = AMBA_CAL_EM_OPTIMIZE_LV_0;
    EmCalCfg.PointMapHFlipEnable = 0U;

    // Cfg
    RetVal = SvcBuffer_Request(SVC_BUFFER_FIXED, FMEM_ID_CALIB_EMIRROR, &EmrCalibWorkBuf, &EmrCalibBufSize);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "SvcBuffer_Request failed", 0U, 0U);
    }

    AmbaMisra_TypeCast(&pEmrCalibWorkBuf, &EmrCalibWorkBuf);

    RetVal = AmbaCal_EmGenCalibInfo(&EmCalCfg, pEmrCalibWorkBuf, &EmaCtrl.CalData);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmGenCalibInfo failed", 0U, 0U);
    }

    EmaCtrl.ViewCfg.ViewStartPos.X        = 19500.0;
    EmaCtrl.ViewCfg.ViewStartPos.Y        = -8000.0;
    EmaCtrl.ViewCfg.ViewStartPos.Z        =  4600.0;
    EmaCtrl.ViewCfg.ViewWidth             = 39000.0;
    EmaCtrl.ViewCfg.DistanceOfStitchPlane = 15000U;
    EmaCtrl.ViewCfg.BackViewScale         = 0.0;
    EmaCtrl.ViewCfg.BackViewVerShift      = 7.0;
    EmaCtrl.ViewCfg.LeftViewHorShift      = 0.0;
    EmaCtrl.ViewCfg.RightViewHorShift     = 0.0;
    EmaCtrl.ViewCfg.ValidVoutLowerBound   = 0U;
    EmaCtrl.ViewCfg.BlendLeft.StartX      = 0U;
    EmaCtrl.ViewCfg.BlendLeft.Angle       = 0;
    EmaCtrl.ViewCfg.BlendLeft.Width       = 32U;
    EmaCtrl.ViewCfg.BlendRight.StartX     = 959U;
    EmaCtrl.ViewCfg.BlendRight.Angle      = 0;
    EmaCtrl.ViewCfg.BlendRight.Width      = 32U;
    EmaCtrl.ViewCfg.VoutAreaBack.StartX   = 480U;
    EmaCtrl.ViewCfg.VoutAreaBack.StartY   = 0U;
    EmaCtrl.ViewCfg.VoutAreaBack.Width    = 960U;
    EmaCtrl.ViewCfg.VoutAreaBack.Height   = 480U;
    EmaCtrl.ViewCfg.VoutAreaLeft.StartX   = 0U;
    EmaCtrl.ViewCfg.VoutAreaLeft.StartY   = 0U;
    EmaCtrl.ViewCfg.VoutAreaLeft.Width    = 960U;
    EmaCtrl.ViewCfg.VoutAreaLeft.Height   = 480U;
    EmaCtrl.ViewCfg.VoutAreaRight.StartX  = 960U;
    EmaCtrl.ViewCfg.VoutAreaRight.StartY  = 0U;
    EmaCtrl.ViewCfg.VoutAreaRight.Width   = 960U;
    EmaCtrl.ViewCfg.VoutAreaRight.Height  = 480U;
    EmaCtrl.ViewCfg.MaskBack.Width        = 960U;
    EmaCtrl.ViewCfg.MaskBack.Height       = 480U;
    EmaCtrl.ViewCfg.MaskBack.pMaskTbl     = NULL;
    EmaCtrl.ViewCfg.MaskLeft.Width        = 960U;
    EmaCtrl.ViewCfg.MaskLeft.Height       = 480U;
    EmaCtrl.ViewCfg.MaskLeft.pMaskTbl     = NULL;
    EmaCtrl.ViewCfg.MaskRight.Width       = 960U;
    EmaCtrl.ViewCfg.MaskRight.Height      = 480U;
    EmaCtrl.ViewCfg.MaskRight.pMaskTbl    = NULL;
    EmaCtrl.ViewCfg.MirrorFlipCfg         = EMIR_VOUT_FLIP;

    EmaCtrl.EmCoorCfg.TransCfg.pCalibDataRaw2World = &EmaCtrl.CalData.CalibDataRaw2World;
    EmaCtrl.EmCoorCfg.TransCfg.pCurvedSurface      = NULL;
    EmaCtrl.EmCoorCfg.TransCfg.p3in1ViewCfg        = &EmaCtrl.ViewCfg;
    EmaCtrl.EmCoorCfg.Type            = EmCalCfg.Type;
    EmaCtrl.EmCoorCfg.CamPos.X        = EmCalCfg.Cam.Pos.X;
    EmaCtrl.EmCoorCfg.CamPos.Y        = EmCalCfg.Cam.Pos.Y;
    EmaCtrl.EmCoorCfg.CamPos.Z        = EmCalCfg.Cam.Pos.Z;
    EmaCtrl.EmCoorCfg.Roi.StartX      = 0U;
    EmaCtrl.EmCoorCfg.Roi.StartY      = 0U;
    EmaCtrl.EmCoorCfg.Roi.Width       = 1920U;
    EmaCtrl.EmCoorCfg.Roi.Height      = 1080U;
    EmaCtrl.EmCoorCfg.VoutArea.StartX = EmCalCfg.VoutArea.StartX ;
    EmaCtrl.EmCoorCfg.VoutArea.StartY = EmCalCfg.VoutArea.StartY;
    EmaCtrl.EmCoorCfg.VoutArea.Width  = EmCalCfg.VoutArea.Width;
    EmaCtrl.EmCoorCfg.VoutArea.Height = EmCalCfg.VoutArea.Height;
    EmaCtrl.EmCoorCfg.CarSize.Width   = 1800U;
    EmaCtrl.EmCoorCfg.CarSize.Height  = 4600U;

    EmaCtrl.SrObjInfo.Angle = 0.0;
    EmaCtrl.SrObjInfo.BbxDetROI.StartX = 480U;
    EmaCtrl.SrObjInfo.BbxDetROI.StartY = 0U;
    EmaCtrl.SrObjInfo.BbxDetROI.Width  = 960U;
    EmaCtrl.SrObjInfo.BbxDetROI.Height = 480U;
}

/**
 * Set print level of log
 * @param[in] Enb Value of print level
 * @return None
 */
void SvcEmrAdaptiveTask_SetPrint(UINT32 Enb)
{
    UINT32 RetVal;
    EmaCtrl.PrintFlag = Enb;

    if ((EmaCtrl.PrintFlag & (UINT32) 1U) > 0U) {
        RetVal = AmbaCal_EmaSetPrintLv(EMA_LOG_LVL_CRITICAL);
    } else if ((EmaCtrl.PrintFlag & ((UINT32)1U << 1U)) > 0U) {
        RetVal = AmbaCal_EmaSetPrintLv(EMA_LOG_LVL_LOG);
    } else if ((EmaCtrl.PrintFlag & ((UINT32)1U << 2U)) > 0U) {
        RetVal = AmbaCal_EmaSetPrintLv(EMA_LOG_LVL_DEBUG);
    } else {
        RetVal = AmbaCal_EmaSetPrintLv(EMA_LOG_LVL_NONE);
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_EMA_TASK, "AmbaCal_EmaSetPrintLv failed", 0U, 0U);
    }
}

/**
 * Set osd level on VOUT
 * @param[in] Enb Value of osd level
 * @return None
 */
void SvcEmrAdaptiveTask_SetOsd(UINT32 Enb)
{
    EmaCtrl.OsdFlag = Enb;
}

/**
 * Set filter width for bbxes
 * @param[in] Width Value of filter width
 * @return None
 */
void SvcEmrAdaptiveTask_SetFilterWidth(UINT32 Width)
{
    EmaCtrl.FilterWidth = Width;
}

/**
 * Set width of max stitch zone
 * @param[in] Width Value of max stitch zone
 * @return None
 */
void SvcEmrAdaptiveTask_SetMaxStitchZone(UINT32 Width)
{
    EmaCtrl.MaxStchLineOffset[0] = Width;
    EmaCtrl.MaxStchLineOffset[1] = Width;
}

/**
 * Set bbx roi margin
 * @param[in] Width Value of bbx roi margin
 * @return None
 */
void SvcEmrAdaptiveTask_SetBbxRoiMargin(UINT32 Width)
{
    EmaCtrl.BbxRoiMargin = Width;
}
