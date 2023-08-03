/**
 *  @file SvcSegDrawTask.c
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
 *  @details svc segmentation draw task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "ambint.h"
/* ssp */
#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDef.h"
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "AmbaSR_Lane.h"
#include "AmbaAP_LKA.h"
#include "AmbaCalib_AVMIF.h"
#include "SvcCalibAdas.h"
#endif

/* svc-framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow_Rebel_RefOD.h"
#include "SvcCvFlow_Rebel_RefSeg.h"

/* svc-shared */
#include "SvcGui.h"
#include "SvcOsd.h"
#include "SvcResCfg.h"
#include "SvcCvFlow.h"
#include "SvcCvImgUtil.h"
#include "SvcCvFlow_Comm.h"
#include "SvcTiming.h"
#include "SvcSysStat.h"

/* svc-icam */
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
#include "SvcLdwsTask.h"
#endif
#include "SvcCvAppDef.h"
#include "SvcSegDrawTask.h"
#include "SvcCvCamCtrl.h"
#include "SvcAdasNotify.h"
#include "SvcAppStat.h"

#define SVC_LOG_SEG_TASK     "SEG_DRAW_TASK"

typedef struct {
    INT16 x;
    INT16 y;
}ST_LANE_point_s;

#define SVC_CV_FLOW_CHAN_MAX            (16U)
#define SEGDRAW_ROI_W_VOUTA             (960U)
#define SEGDRAW_ROI_H_VOUTA             (540U)
#define SEGDRAW_ROI_W_VOUTB             (960U)
#define SEGDRAW_ROI_H_VOUTB             (540U)
#define SEGDRAW_ROI_W_MAX               (1808U)
#define SEGDRAW_ROI_H_MAX               (720U)
#define SEGDRAW_PING_PONG_IDX           (2U)

#define SEGDRAW_GUI_SHOW                (0x01U)
#define SEGDRAW_GUI_UPDATE              (0x02U)
#define SEGDRAW_GUI_LANE_ST_SHOW        (0x04U)
#define SEGDRAW_DISCARD_RATIO_VERTICAL  (0.0) /*Setting for data discard. Will reduce drawing area if not 0*/

#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define SEGDRAW_PIXEL_SIZE_SFT          (2U)   /* 2: 32 bit */
#define SEGDRAW_BUF_SIZE                ((SEGDRAW_ROI_W_MAX * SEGDRAW_ROI_H_MAX) << SEGDRAW_PIXEL_SIZE_SFT)
#else
#define SEGDRAW_PIXEL_SIZE_SFT          (0U)   /* 0: 8 bit */
#define SEGDRAW_BUF_SIZE                (SEGDRAW_ROI_W_MAX * SEGDRAW_ROI_H_MAX)
#endif
#define SEGDRAW_RESCALE_TO_INTER_BUF    (0U)   // Rescale to intermediate buffer before copying to OSD buffer

#define MAX_VOUT_CHAN_NUM            (2U)
#define REBEL_LANE_MAX_POINT_NUMBER  (20U) //Each lane max points

/* The Cv Flow registered ID */
static UINT32 CvFlowRegisterID[SVC_CV_FLOW_CHAN_MAX] = {0};
static UINT32 HaveSegData = 0U;
static SVC_GUI_CANVAS_s g_Canvas GNU_SECTION_NOZEROINIT;
static SVC_SEG_DRAW_OSD_ROI_s g_SegDrawROI GNU_SECTION_NOZEROINIT;
static UINT8  TargetVout = VOUT_IDX_B;
static UINT32 TargetCvChannel = SVC_CV_FLOW_CHAN_MAX + 1U;

static UINT8 *g_pPixelMapTable, *g_pLineMapTable GNU_SECTION_NOZEROINIT;
static UINT32 g_PixelMapTableLength, g_LineMapTableLength GNU_SECTION_NOZEROINIT;
static UINT8 SegDraw[SEGDRAW_BUF_SIZE] GNU_SECTION_NOZEROINIT;
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static SVC_LDWS_OUT_s g_LDout;
/* LKA needed */
static AMBA_CAL_EM_CALC_COORD_CFG_s *g_pLKACalibData = NULL;
#endif

static SVC_CV_DETRES_SEG_REBEL_s g_LanelistRebel[SEGDRAW_PING_PONG_IDX];
static UINT8 *pOsdUpdateSldBuf GNU_SECTION_NOZEROINIT;
static UINT32 g_MaskWidth = 0, g_MaskHeight = 0;
static UINT32 g_SegDrawPinPongIdx = 0U;

static void SvcSegDraw_StatusCallback(UINT32 StatIdx, void *pInfo);


#define SQRT2                         ((FLOAT) 1.414213562)
#define SQRT2xSQRT2                   ((FLOAT) 2)
#define SQRT2xSQRT2xSQRT2             ((FLOAT) 2.828427124)
#define SQRT2xSQRT2xSQRT2xSQRT2       ((FLOAT) 4)
#define SQRT2xSQRT2xSQRT2xSQRT2xSQRT2 ((FLOAT) 5.656854248)
static void SvcSegDrawTask_ROI_Rebel(UINT32 VoutChan, UINT32 FovIdx, const SVC_CV_ROI_s *pRoi, const SVC_CV_ROI_s *pMaskInfo)
{

    FLOAT Tmp = 0.0f;
    UINT32 i;
    AMBA_DSP_WINDOW_DIMENSION_s MainWin, HierWin;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                FLOAT Width, Height;
                DOUBLE RoiMaskRatioWidth = 1.0, RoiMaskRatioHeight = 1.0;

                MainWin.Width  = pResCfg->FovCfg[FovIdx].MainWin.Width;
                MainWin.Height = pResCfg->FovCfg[FovIdx].MainWin.Height;
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]MainWin Width = %d, Height = %d", MainWin.Width, MainWin.Height);
                Width  = (FLOAT)(MainWin.Width);
                Height = (FLOAT)(MainWin.Height);
                switch(pRoi->Index) {
                case 0:
                    //No action
                    break;
                case 1:
                    Width  /= SQRT2;
                    Height /= SQRT2;
                    break;
                case 2:
                    Width  /= SQRT2xSQRT2;
                    Height /= SQRT2xSQRT2;
                    break;
                case 3:
                    Width  /= SQRT2xSQRT2xSQRT2;
                    Height /= SQRT2xSQRT2xSQRT2;
                    break;
                case 4:
                    Width  /= SQRT2xSQRT2xSQRT2xSQRT2;
                    Height /= SQRT2xSQRT2xSQRT2xSQRT2;
                    break;
                case 5:
                    Width  /= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                    Height /= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                    break;
                default:
                    SvcLog_NG(SVC_LOG_SEG_TASK, "SvcSegDrawTask_ROI_Rebel() Something wrong, PyramidScale = %d", pRoi->Index, 0U);
                    break;

                }
                HierWin.Width = (UINT16)GetAlignedValU32((UINT32)Width, 8U);
                HierWin.Height = (UINT16)GetAlignedValU32((UINT32)Height, 2U);

                g_SegDrawROI.Chan = VoutChan;
                Tmp = (FLOAT)(((FLOAT)g_Canvas.Width)/((FLOAT)HierWin.Width))*((FLOAT)pRoi->Width);
                if ((UINT32)Tmp > g_SegDrawROI.Width) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "g_SegDrawROI.Width(%d) smaller than evaluate value (%d)", g_SegDrawROI.Width, (UINT32)Tmp);
                } else {
                    g_SegDrawROI.Width = (UINT32)Tmp;
                }

                Tmp = (FLOAT)(((FLOAT)g_Canvas.Height)/((FLOAT)HierWin.Height))*((FLOAT)pRoi->Height);
                if ((UINT32)Tmp > g_SegDrawROI.Height) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "g_SegDrawROI.Height(%d) smaller than evaluate value (%d)", g_SegDrawROI.Height, (UINT32)Tmp);
                } else {
                    g_SegDrawROI.Height = (UINT32)Tmp;
                }

                Tmp = (FLOAT)(((FLOAT)g_Canvas.Width)/((FLOAT)HierWin.Width))*((FLOAT)pRoi->StartX);
                g_SegDrawROI.Xstart = ((UINT32)Tmp)+g_Canvas.StartX;
                Tmp = (FLOAT)(((FLOAT)g_Canvas.Height)/((FLOAT)HierWin.Height))*((FLOAT)pRoi->StartY);
                g_SegDrawROI.Ystart = ((UINT32)Tmp)+g_Canvas.StartY;

                RoiMaskRatioWidth  = ((DOUBLE)pRoi->Width)/((DOUBLE)pMaskInfo->Width);
                RoiMaskRatioHeight = ((DOUBLE)pRoi->Height)/((DOUBLE)pMaskInfo->Height);

                // Setup PixelMapTable info
                SvcCvImgUtil_GetBitMapScaleTable((((DOUBLE)g_SegDrawROI.Width)/((DOUBLE)pRoi->Width))*RoiMaskRatioWidth,
                                                 &g_pPixelMapTable, &g_PixelMapTableLength);
                // Setup LineMapTable info
                SvcCvImgUtil_GetBitMapScaleTable((((DOUBLE)g_SegDrawROI.Height)/((DOUBLE)pRoi->Height))*RoiMaskRatioHeight,
                                                 &g_pLineMapTable, &g_LineMapTableLength);

                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]pMaskInfo->Width = %d", pMaskInfo->Width, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]pMaskInfo->Height = %d", pMaskInfo->Height, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw][PyramidScale = L_%d]", pRoi->Index, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]pRoi->StartX = %d, StartY = %d", pRoi->StartX, pRoi->StartY);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]pRoi->Width = %d, Height = %d", pRoi->Width, pRoi->Height);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]HierWin Width = %d, Height = %d", HierWin.Width, HierWin.Height);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]SvcSegDrawTask_ROI_Rebel() g_SegDrawROI Xstart = %d, Ystart = %d", g_SegDrawROI.Xstart, g_SegDrawROI.Ystart);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]SvcSegDrawTask_ROI_Rebel() g_SegDrawROI Width = %d, Height = %d", g_SegDrawROI.Width, g_SegDrawROI.Height);
                break;
            }
        }
    }
}

static void SvcSegDrawTask_ROI(UINT32 VoutChan, UINT32 FovIdx)
{
    FLOAT Tmp = 0.0f;
    UINT32 i, RetVal;
    SVC_CV_ROI_INFO_s  RoiInfo = {0};
    SVC_CV_MASK_INFO_s MaskInfo = {0};
    AMBA_DSP_WINDOW_DIMENSION_s MainWin, HierWin;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                FLOAT Width, Height;
                DOUBLE RoiMaskRatioWidth = 1.0, RoiMaskRatioHeight = 1.0;

                MainWin.Width  = pResCfg->FovCfg[FovIdx].MainWin.Width;
                MainWin.Height = pResCfg->FovCfg[FovIdx].MainWin.Height;
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]MainWin Width = %d, Height = %d", MainWin.Width, MainWin.Height);
                RetVal = SvcCvFlow_Control(i, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo);
                Width  = (FLOAT)(MainWin.Width);
                Height = (FLOAT)(MainWin.Height);
                switch(RoiInfo.Roi[0].Index) {
                case 0:
                    //No action
                    break;
                case 1:
                    Width  /= SQRT2;
                    Height /= SQRT2;
                    break;
                case 2:
                    Width  /= SQRT2xSQRT2;
                    Height /= SQRT2xSQRT2;
                    break;
                case 3:
                    Width  /= SQRT2xSQRT2xSQRT2;
                    Height /= SQRT2xSQRT2xSQRT2;
                    break;
                case 4:
                    Width  /= SQRT2xSQRT2xSQRT2xSQRT2;
                    Height /= SQRT2xSQRT2xSQRT2xSQRT2;
                    break;
                case 5:
                    Width  /= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                    Height /= SQRT2xSQRT2xSQRT2xSQRT2xSQRT2;
                    break;
                default:
                    SvcLog_NG(SVC_LOG_SEG_TASK, "SvcSegDrawTask_ROI() Something wrong, PyramidScale = %d", RoiInfo.Roi[0].Index, 0U);
                    break;

                }
                HierWin.Width = (UINT16)GetAlignedValU32((UINT32)Width, 8U);
                HierWin.Height = (UINT16)GetAlignedValU32((UINT32)Height, 2U);

                g_SegDrawROI.Chan = VoutChan;
                Tmp = (FLOAT)(((FLOAT)g_Canvas.Width)/((FLOAT)HierWin.Width))*((FLOAT)RoiInfo.Roi[0].Width);
                if ((UINT32)Tmp > g_SegDrawROI.Width) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "g_SegDrawROI.Width(%d) smaller than evaluate value (%d)", g_SegDrawROI.Width, (UINT32)Tmp);
                } else {
                    g_SegDrawROI.Width = (UINT32)Tmp;
                }

                Tmp = (FLOAT)(((FLOAT)g_Canvas.Height)/((FLOAT)HierWin.Height))*((FLOAT)RoiInfo.Roi[0].Height);
                if ((UINT32)Tmp > g_SegDrawROI.Height) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "g_SegDrawROI.Height(%d) smaller than evaluate value (%d)", g_SegDrawROI.Height, (UINT32)Tmp);
                } else {
                    g_SegDrawROI.Height = (UINT32)Tmp;
                }

                Tmp = (FLOAT)(((FLOAT)g_Canvas.Width)/((FLOAT)HierWin.Width))*((FLOAT)RoiInfo.Roi[0].StartX);
                g_SegDrawROI.Xstart = ((UINT32)Tmp)+g_Canvas.StartX;
                Tmp = (FLOAT)(((FLOAT)g_Canvas.Height)/((FLOAT)HierWin.Height))*((FLOAT)RoiInfo.Roi[0].StartY);
                g_SegDrawROI.Ystart = ((UINT32)Tmp)+g_Canvas.StartY;
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "SvcCvFlow_Control GET_ROI_INFO err %d", RetVal, 0U);
                }

                RetVal = SvcCvFlow_Control(i, SVC_CV_CTRL_GET_MASK_INFO, &MaskInfo);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "SvcCvFlow_Control GET_MASK_INFO err %d", RetVal, 0U);
                } else {
                    RoiMaskRatioWidth  = ((DOUBLE)RoiInfo.Roi[0].Width)/((DOUBLE)MaskInfo.Roi[0].Width);
                    RoiMaskRatioHeight = ((DOUBLE)RoiInfo.Roi[0].Height)/((DOUBLE)MaskInfo.Roi[0].Height);
                }

                /* Setup PixelMapTable info */
                SvcCvImgUtil_GetBitMapScaleTable((((DOUBLE)g_SegDrawROI.Width)/((DOUBLE)RoiInfo.Roi[0].Width))*RoiMaskRatioWidth,
                                                 &g_pPixelMapTable, &g_PixelMapTableLength);
                /* Setup LineMapTable info */
                SvcCvImgUtil_GetBitMapScaleTable((((DOUBLE)g_SegDrawROI.Height)/((DOUBLE)RoiInfo.Roi[0].Height))*RoiMaskRatioHeight,
                                                 &g_pLineMapTable, &g_LineMapTableLength);

                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]MaskInfo NumCat = %d", MaskInfo.Roi[0].NumCat, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]MaskInfo Width = %d", MaskInfo.Roi[0].Width, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]MaskInfo Height = %d", MaskInfo.Roi[0].Height, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw][PyramidScale = L_%d]", RoiInfo.Roi[0].Index, 0U);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]RoiInfo.Roi[0] StartX = %d, StartY = %d", RoiInfo.Roi[0].StartX, RoiInfo.Roi[0].StartY);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]RoiInfo.Roi[0] Width = %d, Height = %d", RoiInfo.Roi[0].Width, RoiInfo.Roi[0].Height);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]HierWin Width = %d, Height = %d", HierWin.Width, HierWin.Height);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]SvcSegDrawTask_ROI() g_SegDrawROI Xstart = %d, Ystart = %d", g_SegDrawROI.Xstart, g_SegDrawROI.Ystart);
                SvcLog_OK(SVC_LOG_SEG_TASK, "[SegDraw]SvcSegDrawTask_ROI() g_SegDrawROI Width = %d, Height = %d", g_SegDrawROI.Width, g_SegDrawROI.Height);
                break;
            }
        }
    }
}

static void SvcSegGui_Draw(UINT32 VoutIdx, UINT32 Level)
{
    #define LANE_WIDTH                    (2U)
    #define OSD_DRAW_LINE_MAX_LINE_POINTS (100U)//Amba Draw Line Max
    #define REBEL_SEG_COLOR_IDX_OFFSET      (40U)
    #define OPEN_SEG_COLOR_IDX_OFFSET       (48U)
    static UINT32 OsdBufWidth, OsdBufHeight;
      static UINT32 OsdXend, OsdYend, Tmp;
    static SVC_SEG_DRAW_OSD_ROI_s SegDrawROI;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((HaveSegData & SEGDRAW_GUI_LANE_ST_SHOW) > 0U) {
        UINT32 SegDrawIdx = g_SegDrawPinPongIdx;
        if (SegDrawIdx == 0U){
            SegDrawIdx = (SEGDRAW_PING_PONG_IDX-1U);
        } else {
            SegDrawIdx--;
        }

        {
            if (g_LanelistRebel[SegDrawIdx].SegResult.length != 0U){//length ==0 means not bitmap usage ('length' filled from Linux APP)
                static UINT8 IsROI_Init = 0U;

                if (0U == IsROI_Init) {
                    SVC_CV_ROI_s Roi, MaskInfo;

                    // Initialize SegDraw ROI Width and Height
                    if (TargetVout == VOUT_IDX_A) {
                        g_SegDrawROI.Width = SEGDRAW_ROI_W_VOUTA;
                        g_SegDrawROI.Height = SEGDRAW_ROI_H_VOUTA;
                    } else {
                        g_SegDrawROI.Width = SEGDRAW_ROI_W_VOUTB;
                        g_SegDrawROI.Height = SEGDRAW_ROI_H_VOUTB;
                    }

                    AmbaSvcWrap_MisraMemset(&Roi, 0, sizeof(SVC_CV_ROI_s));
                    AmbaSvcWrap_MisraMemset(&MaskInfo, 0, sizeof(SVC_CV_ROI_s));
                    Roi.Index  = g_LanelistRebel[SegDrawIdx].SegResult.pyramid_id;
                    Roi.StartX = g_LanelistRebel[SegDrawIdx].SegResult.mask_start_col;
                    Roi.StartY = g_LanelistRebel[SegDrawIdx].SegResult.mask_start_row;
                    Roi.Width  = g_LanelistRebel[SegDrawIdx].SegResult.mask_width;
                    Roi.Height = g_LanelistRebel[SegDrawIdx].SegResult.mask_height;

                    MaskInfo.Index  = g_LanelistRebel[SegDrawIdx].SegResult.pyramid_id;
                    MaskInfo.Width  = g_LanelistRebel[SegDrawIdx].SegResult.buff_width;
                    MaskInfo.Height = g_LanelistRebel[SegDrawIdx].SegResult.buff_height;

                    SvcSegDrawTask_ROI_Rebel(TargetVout, 0U, &Roi, &MaskInfo);
                    IsROI_Init = 1U;
                }

                {
                    UINT32 RetVal;
                    SVC_BIT_MAP_RESCALE_REPT_s Cfg;
                    UINT8 *pPixelMap, *pLineMap;
                    UINT32 PixelMapSize, LineMapSize;
                    UINT32 *pClut = NULL, NumColor = 0U;

                    AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_BIT_MAP_RESCALE_REPT_s));
                    Cfg.pSrc = g_LanelistRebel[SegDrawIdx].SegResult.buff;

                    pPixelMap = g_pPixelMapTable;
                    pLineMap  = g_pLineMapTable;
                    PixelMapSize = g_PixelMapTableLength;
                    LineMapSize  = g_LineMapTableLength;

                    Cfg.SrcWidth = g_LanelistRebel[SegDrawIdx].SegResult.buff_width;
                    Cfg.SrcHeight = g_LanelistRebel[SegDrawIdx].SegResult.buff_height;
                    Cfg.SrcPitch = g_LanelistRebel[SegDrawIdx].SegResult.buff_width;
                    Cfg.SrcStartY = 0U;

                    Cfg.pHorizantalMap = pPixelMap;
                    Cfg.HorizantalMapLen = PixelMapSize;
                    Cfg.pVerticalMap = pLineMap;
                    Cfg.VerticalMapLen = LineMapSize;

                    Cfg.pDst = SegDraw;
                    Cfg.DstWidth = g_SegDrawROI.Width;
                    Cfg.DstHeight = g_SegDrawROI.Height;
                    Cfg.DstPitch = g_SegDrawROI.Width << SEGDRAW_PIXEL_SIZE_SFT;

                    RetVal = SvcOsd_GetOsdCLUT(TargetVout, &pClut, &NumColor);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(__func__, "line %u SvcOsd_GetOsdCLUT return %u", __LINE__, RetVal);
                    }

                    if (NumColor > REBEL_SEG_COLOR_IDX_OFFSET) {
                        Cfg.pClut = &pClut[REBEL_SEG_COLOR_IDX_OFFSET];
                        Cfg.NumColor = NumColor - REBEL_SEG_COLOR_IDX_OFFSET;
                    } else {
                        Cfg.pClut = pClut;
                        Cfg.NumColor = NumColor;
                    }

                    RetVal = SvcOsd_GetOsdBufSize(TargetVout, &OsdBufWidth, &OsdBufHeight);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
                    }

                    SegDrawROI.Xstart = g_SegDrawROI.Xstart;
                    OsdXend = SegDrawROI.Xstart + g_SegDrawROI.Width;

                    SegDrawROI.Ystart = g_SegDrawROI.Ystart;
                    Tmp = SegDrawROI.Ystart + g_SegDrawROI.Height;
                    AmbaMisra_TouchUnused(&Tmp);

                    // Draw and rescale the original map to OSD buffer directly
                    {
                        SVC_OSD_SW_RESCALE_CFG_s OsdRsCfg;
                        OsdRsCfg.pSrc             = Cfg.pSrc;
                        OsdRsCfg.SrcWidth         = Cfg.SrcWidth;
                        OsdRsCfg.SrcHeight        = Cfg.SrcHeight;
                        OsdRsCfg.SrcPitch         = Cfg.SrcPitch;
                        OsdRsCfg.pDst             = Cfg.pDst;
                        OsdRsCfg.DstWidth         = Cfg.DstWidth;
                        OsdRsCfg.DstHeight        = Cfg.DstHeight;
                        OsdRsCfg.DstPitch         = Cfg.DstPitch;
                        OsdRsCfg.HorizantalMapLen = Cfg.HorizantalMapLen;
                        OsdRsCfg.pHorizantalMap   = Cfg.pHorizantalMap;
                        OsdRsCfg.VerticalMapLen   = Cfg.VerticalMapLen;
                        OsdRsCfg.pVerticalMap     = Cfg.pVerticalMap;
                        OsdRsCfg.ColorOffset      = REBEL_SEG_COLOR_IDX_OFFSET;
                        OsdRsCfg.pClut            = Cfg.pClut;
                        OsdRsCfg.NumColor         = Cfg.NumColor;
                        OsdRsCfg.SrcStartY        = Cfg.SrcStartY;
                        RetVal = SvcOsd_DrawBin_Rescale(TargetVout, SegDrawROI.Xstart, SegDrawROI.Ystart, OsdXend, OsdYend, &OsdRsCfg);
                    }
                    if (SVC_NG == RetVal) {
                        SvcLog_NG(SVC_LOG_SEG_TASK, "[Rebel]g_SegDrawROI.Xstart, g_SegDrawROI.Ystart = %d, %d", g_SegDrawROI.Xstart, g_SegDrawROI.Ystart);
                        SvcLog_NG(SVC_LOG_SEG_TASK, "[Rebel]OsdXend, OsdYend = %d, %d", OsdXend, OsdYend);
                        SvcLog_NG(SVC_LOG_SEG_TASK, "[Rebel]SvcOsd_DrawBin DrawBin error !", 0U, 0U);
                    } else {
                        //Do nothing
                    }
                }
            }



            {
                UINT32 LaneIdx = 0U;
                UINT32 EachLanePointMax = REBEL_LANE_MAX_POINT_NUMBER;
                if (g_LanelistRebel[SegDrawIdx].TotalLaneNum > 0U) {
                    if (g_LanelistRebel[SegDrawIdx].TotalLaneNum > 5U) {
                        EachLanePointMax = OSD_DRAW_LINE_MAX_LINE_POINTS / g_LanelistRebel[SegDrawIdx].TotalLaneNum;
                    } else {
                        EachLanePointMax = REBEL_LANE_MAX_POINT_NUMBER; //Each lane max is 20 points
                    }
                }
                if (g_LanelistRebel[SegDrawIdx].TotalLaneNum > 0U) {
                    ST_LANE_point_s ST_Point[REBEL_LANE_MAX_POINT_NUMBER];
                    for (LaneIdx = 0U; LaneIdx < g_LanelistRebel[SegDrawIdx].TotalLaneNum; LaneIdx++) {
                        if (g_LanelistRebel[SegDrawIdx].SampledPointNum[LaneIdx] > 2U) {
                            UINT32 RetVal;
                            UINT32 SkipRatio = 1U;
                            UINT32 NuberOfPoint = 0;
                            if (g_LanelistRebel[SegDrawIdx].SampledPointNum[LaneIdx] > EachLanePointMax) {
                                SkipRatio = (g_LanelistRebel[SegDrawIdx].SampledPointNum[LaneIdx] / EachLanePointMax) + 1U;
                            }

                            for (UINT32 Idx = 0U; (Idx + SkipRatio) < (g_LanelistRebel[SegDrawIdx].SampledPointNum[LaneIdx] - 1U); Idx += SkipRatio) {
                                UINT32 X = (((g_LanelistRebel[SegDrawIdx].point[LaneIdx][Idx].x))/4U);
                                UINT32 Y = (((g_LanelistRebel[SegDrawIdx].point[LaneIdx][Idx].y)/4U)+30U);

                                RetVal = SvcOsd_GetOsdBufSize(TargetVout, &OsdBufWidth, &OsdBufHeight);
                                if (SVC_OK != RetVal) {
                                    SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
                                }
                                if ((X < (OsdBufWidth - LANE_WIDTH)) && (Y < (OsdBufHeight - LANE_WIDTH))) {
                                    ST_Point[NuberOfPoint].x = (INT16)X;
                                    ST_Point[NuberOfPoint].y = (INT16)Y;
                                    NuberOfPoint++;
                                }
                            }
//                            SvcLog_NG(__func__, "[%d] NuberOfPoint = %d", LaneIdx, NuberOfPoint);
                            RetVal = SvcOsd_DrawMultiplePointsLine(TargetVout, ST_Point, NuberOfPoint, LANE_WIDTH, 0xFF39EBEBU);
                            AmbaMisra_TouchUnused(&RetVal);
                        }
                    }
                }
            }
        }
    } else if ((HaveSegData & SEGDRAW_GUI_SHOW) > 0U) {
        UINT32 RetVal;
        SVC_BIT_MAP_RESCALE_REPT_s Cfg;
        UINT8 *pPixelMap, *pLineMap;
        UINT32 PixelMapSize, LineMapSize;
        UINT32 *pClut = NULL, NumColor = 0U;
        FLOAT SrcStartY;

        AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_BIT_MAP_RESCALE_REPT_s));
        Cfg.pSrc = pOsdUpdateSldBuf;

        pPixelMap = g_pPixelMapTable;
        pLineMap  = g_pLineMapTable;
        PixelMapSize = g_PixelMapTableLength;
        LineMapSize  = g_LineMapTableLength;

        Cfg.SrcWidth = g_MaskWidth;
        Cfg.SrcHeight = g_MaskHeight;
        Cfg.SrcPitch = g_MaskWidth;
        SrcStartY = (FLOAT)Cfg.SrcHeight * (FLOAT)SEGDRAW_DISCARD_RATIO_VERTICAL;
        Cfg.SrcStartY = (UINT32)SrcStartY;

        Cfg.pHorizantalMap = pPixelMap;
        Cfg.HorizantalMapLen = PixelMapSize;
        Cfg.pVerticalMap = pLineMap;
        Cfg.VerticalMapLen = LineMapSize;

        Cfg.pDst = SegDraw;
        Cfg.DstWidth = g_SegDrawROI.Width;
        Cfg.DstHeight = g_SegDrawROI.Height;
        Cfg.DstPitch = g_SegDrawROI.Width << SEGDRAW_PIXEL_SIZE_SFT;

#ifdef CONFIG_ICAM_32BITS_OSD_USED
        RetVal = SvcOsd_GetOsdCLUT(TargetVout, &pClut, &NumColor);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u SvcOsd_GetOsdCLUT return %u", __LINE__, RetVal);
        }

        if (NumColor > OPEN_SEG_COLOR_IDX_OFFSET) {
            Cfg.pClut = &pClut[OPEN_SEG_COLOR_IDX_OFFSET];
            Cfg.NumColor = NumColor - OPEN_SEG_COLOR_IDX_OFFSET;
        } else {
            Cfg.pClut = pClut;
            Cfg.NumColor = NumColor;
        }
#if SEGDRAW_RESCALE_TO_INTER_BUF
        RetVal = SvcCvImgUtil_RescaleBitMap_Idx2Color(&Cfg);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u SvcCvImgUtil_RescaleBitMap return %u", __LINE__, RetVal);
        }
#endif

#else
        AmbaMisra_TouchUnused(pClut);
        AmbaMisra_TouchUnused(&NumColor);
        Cfg.ColorOffset = SEGDRAW_COLOR_IDX_OFFSET;
#if SEGDRAW_RESCALE_TO_INTER_BUF
        RetVal = SvcCvImgUtil_RescaleBitMap_Rept(&Cfg);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u SvcCvImgUtil_RescaleBitMap return %u", __LINE__, RetVal);
        }
#endif
#endif

        RetVal = SvcOsd_GetOsdBufSize(TargetVout, &OsdBufWidth, &OsdBufHeight);
        if (SVC_OK != RetVal) {
            SvcLog_NG(__func__, "line %u SvcOsd_GetOsdBufSize return %u", __LINE__, RetVal);
        }

        SegDrawROI.Xstart = g_SegDrawROI.Xstart;
        OsdXend = SegDrawROI.Xstart + g_SegDrawROI.Width;

        SegDrawROI.Ystart = g_SegDrawROI.Ystart;
        Tmp = SegDrawROI.Ystart + g_SegDrawROI.Height;
        //Need to avoid to cropped menu icon when drawing segment => (OsdBufHeight-60U)
        OsdYend = (Tmp > (OsdBufHeight-60U)) ? (OsdBufHeight-60U) : Tmp;

#if SEGDRAW_RESCALE_TO_INTER_BUF
        /* Draw the rescaled map from inter buffer to OSD buffer */
        RetVal = SvcOsd_DrawBin(TargetVout, SegDrawROI.Xstart, SegDrawROI.Ystart, OsdXend, OsdYend, SegDraw);
#else
        /* Draw and rescale the original map to OSD buffer directly */
        {
            SVC_OSD_SW_RESCALE_CFG_s OsdRsCfg;
            OsdRsCfg.pSrc             = Cfg.pSrc;
            OsdRsCfg.SrcWidth         = Cfg.SrcWidth;
            OsdRsCfg.SrcHeight        = Cfg.SrcHeight;
            OsdRsCfg.SrcPitch         = Cfg.SrcPitch;
            OsdRsCfg.pDst             = Cfg.pDst;
            OsdRsCfg.DstWidth         = Cfg.DstWidth;
            OsdRsCfg.DstHeight        = Cfg.DstHeight;
            OsdRsCfg.DstPitch         = Cfg.DstPitch;
            OsdRsCfg.HorizantalMapLen = Cfg.HorizantalMapLen;
            OsdRsCfg.pHorizantalMap   = Cfg.pHorizantalMap;
            OsdRsCfg.VerticalMapLen   = Cfg.VerticalMapLen;
            OsdRsCfg.pVerticalMap     = Cfg.pVerticalMap;
            OsdRsCfg.ColorOffset      = Cfg.ColorOffset;
            OsdRsCfg.pClut            = Cfg.pClut;
            OsdRsCfg.NumColor         = Cfg.NumColor;
            OsdRsCfg.SrcStartY        = Cfg.SrcStartY;
            RetVal = SvcOsd_DrawBin_Rescale(TargetVout, SegDrawROI.Xstart, SegDrawROI.Ystart, OsdXend, OsdYend, &OsdRsCfg);
        }
#endif
        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_SEG_TASK, "g_SegDrawROI.Xstart, g_SegDrawROI.Ystart = %d, %d", g_SegDrawROI.Xstart, g_SegDrawROI.Ystart);
            SvcLog_NG(SVC_LOG_SEG_TASK, "OsdXend, OsdYend = %d, %d", OsdXend, OsdYend);
            SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawBin DrawBin error !", 0U, 0U);
        } else {
            //Do nothing
        }
    } else {
        //Do nothing
    }







    AmbaMisra_TouchUnused(&g_pPixelMapTable);
    AmbaMisra_TouchUnused(&g_PixelMapTableLength);
    AmbaMisra_TouchUnused(&g_LineMapTableLength);
    AmbaMisra_TouchUnused(&g_pLineMapTable);
    AmbaMisra_TouchUnused(SegDraw);

}

static void SvcSegGui_Update(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    if ((HaveSegData & SEGDRAW_GUI_UPDATE) > 0U) {
        HaveSegData &= ~(SEGDRAW_GUI_UPDATE);
        *pUpdate = 1U;
    } else {
        *pUpdate = 0U;
    }
}

static void SvcSegDraw_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    const SVC_CV_DETRES_SEG_s* pSegResult;
    UINT32 MsgCode;

    #if defined(CONFIG_ICAM_TIMING_LOG)
    { /* for SVC timing measurement */
        static UINT8 Done = 0U;
        if (0U == Done) {
            SvcTime(SVC_TIME_CV_FLEXIDAG_1ST_RESULT, "CV FlexiDag 1st result");
            SvcTime_PrintBootTimeCv();
            Done = 1U;
        }
    }
    #endif

    (void) pDetResMsg;
    if(Chan == TargetCvChannel){
        AmbaMisra_TouchUnused(&Chan);
        AmbaMisra_TouchUnused(&CvType);
        AmbaMisra_TypeCast32(&MsgCode, pDetResMsg);
        if (SVC_CV_DETRES_SEG_REBEL == MsgCode) {
             const SVC_CV_DETRES_SEG_REBEL_s *pLanelist;
             AmbaMisra_TypeCast(&pLanelist, &pDetResMsg);
             AmbaSvcWrap_MisraMemcpy(&g_LanelistRebel[g_SegDrawPinPongIdx], pLanelist, sizeof(SVC_CV_DETRES_SEG_REBEL_s));
             g_SegDrawPinPongIdx++;
             if (g_SegDrawPinPongIdx == SEGDRAW_PING_PONG_IDX) {
                 g_SegDrawPinPongIdx = 0U;
             }
             HaveSegData |= (SEGDRAW_GUI_LANE_ST_SHOW | SEGDRAW_GUI_UPDATE);
         } else if (SVC_CV_DETRES_SEG == MsgCode) {
             AmbaMisra_TypeCast(&pSegResult, &pDetResMsg);
             AmbaMisra_TypeCast(&pOsdUpdateSldBuf, &pSegResult->pBuf);
             g_MaskWidth = pSegResult->Width;
             g_MaskHeight = pSegResult->Height;
             HaveSegData |= (SEGDRAW_GUI_SHOW | SEGDRAW_GUI_UPDATE);
         } else {
             //Do nothing
         }
    }
}

/**
 *  Init the Segmentation osd draw module
 *  @return error code
 */
UINT32 SvcSegDrawTask_Init(void)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 i, j, k, Found = 0U, FovIdx = 0U, RetVal;
    UINT32 VoutPri[MAX_VOUT_CHAN_NUM];
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    const SVC_CHAN_CFG_s  *pChanCfg = NULL;
    UINT32 Value, MixerWidth, MixerHeight, OsdWidth, OsdHeight;

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 Rval;
    static AMBA_CAL_EM_CALC_COORD_CFG_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    AmbaSvcWrap_MisraMemset(&g_LDout, 0, sizeof(SVC_LDWS_OUT_s));
#endif
    AmbaSvcWrap_MisraMemset(&g_Canvas, 0, sizeof(g_Canvas));
    AmbaSvcWrap_MisraMemset(&g_SegDrawROI, 0, sizeof(g_SegDrawROI));
    AmbaSvcWrap_MisraMemset(&g_LanelistRebel, 0, sizeof(SVC_CV_DETRES_SEG_REBEL_s) * SEGDRAW_PING_PONG_IDX);

    /* Get Display config */
    VoutPri[0] = VOUT_IDX_B;
    VoutPri[1] = VOUT_IDX_A;
    for (k = 0; k < MAX_VOUT_CHAN_NUM; k++) {
        /* Search corresponding VOUT */
        for (i = 0; i < pResCfg->DispNum; i++) {
            if (pResCfg->DispStrm[i].VoutID == VoutPri[k]) {
                /* Search corresponding FOV */
                for (j = 0; j < pResCfg->DispStrm[i].StrmCfg.NumChan; j++) {
                    if (FovIdx == pResCfg->DispStrm[i].StrmCfg.ChanCfg[j].FovId) {
                        pDispStrm = &pResCfg->DispStrm[i];
                        pChanCfg  = &pResCfg->DispStrm[i].StrmCfg.ChanCfg[j];
                        Found = 1U;

                        /* Initialize SegDraw ROI Width and Height */
                        if (TargetVout == VOUT_IDX_A) {
                            g_SegDrawROI.Width = SEGDRAW_ROI_W_VOUTA;
                            g_SegDrawROI.Height = SEGDRAW_ROI_H_VOUTA;
                        } else {
                            g_SegDrawROI.Width = SEGDRAW_ROI_W_VOUTB;
                            g_SegDrawROI.Height = SEGDRAW_ROI_H_VOUTB;
                        }

                        /* Calculate OSD region with resepect to OSD buffer */
                        /* Suppose max OSD window is the same as Mixer window */
                        RetVal = SvcOsd_GetOsdBufSize(pResCfg->DispStrm[i].VoutID, &OsdWidth, &OsdHeight);
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_GetOsdBufSize err %d", RetVal, 0U);
                        }
                        MixerWidth  = pDispStrm->StrmCfg.MaxWin.Width;
                        MixerHeight = pDispStrm->StrmCfg.MaxWin.Height;

                        if (pDispStrm->VideoRotateFlip == (UINT8) AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                            Value = (UINT32) pDispStrm->StrmCfg.Win.Width - ((UINT32) pChanCfg->DstWin.OffsetX + (UINT32) pChanCfg->DstWin.Width);
                        } else {
                            Value = pChanCfg->DstWin.OffsetX;
                        }
                        g_Canvas.StartX = ((Value + ((MixerWidth - pDispStrm->StrmCfg.Win.Width) >> 1)) * OsdWidth) / MixerWidth;
                        g_Canvas.StartY = ((pChanCfg->DstWin.OffsetY + ((MixerHeight - pDispStrm->StrmCfg.Win.Height) >> 1)) * OsdHeight) / MixerHeight;
                        g_Canvas.Width  = (pChanCfg->DstWin.Width * OsdWidth) / MixerWidth;
                        g_Canvas.Height = (pChanCfg->DstWin.Height * OsdHeight) / MixerHeight;
                        break;
                    }
                }
            }
            if (1U == Found) {
                break;
            }
        }
        if (1U == Found) {
            break;
        }
    }
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                FovIdx = pCvFlow[i].InputCfg.Input[0].StrmId;
            }
        }
    }

    /* Get calibration data which LKA needed */
    Cfg.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
    Rval = SvcCalib_AdasCfgGet(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);
    if(Rval == SVC_OK) {
        SvcLog_OK("SvcSegDrawTask_Init", "[LKA] Get ADAS CAM CALIB from NAND !", 0U, 0U);
        g_pLKACalibData = &Cfg;
    } else {
        SvcLog_OK("SvcSegDrawTask_Init", "[LKA] No CALIB from NAND !", 0U, 0U);
    }

#endif

    /* Register necessary CallBack function */
    {
        SVC_SYS_STAT_ID StatusID;
        RetVal = SvcSysStat_Register(SVC_APP_STAT_DISP, SvcSegDraw_StatusCallback, &StatusID);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SEG_TASK, "SvcSegDrawTask_Init err. Register DISP status failed", 0U, 0U);
        }
    }


    return SVC_OK;
}

void SvcSegDrawTask_FoVResInit(void)
{

    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 i, j, k, s, Found = 0U, FovIdx = 0U;
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    UINT32 VoutPri[MAX_VOUT_CHAN_NUM];
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    const SVC_CHAN_CFG_s  *pChanCfg = NULL;
    UINT32 Value, MixerWidth, MixerHeight, OsdWidth, OsdHeight;
    UINT32 DispNum = 0U;
    const SVC_DISP_STRM_s *pDisp = NULL;
    TargetCvChannel = SVC_CV_FLOW_CHAN_MAX + 1U;
    HaveSegData &= ~(SEGDRAW_GUI_SHOW);
    HaveSegData |= SEGDRAW_GUI_UPDATE;

    if(pResCfg->DispAltNum > 0U){
        pDisp = pResCfg->DispAlt[pResCfg->DispAltIdx].DispStrm;
        DispNum = pResCfg->DispAlt[pResCfg->DispAltIdx].DispNum;
    } else{
        pDisp = pResCfg->DispStrm;
        DispNum = pResCfg->DispNum;
    }

    /* Get Display config */
    VoutPri[0] = VOUT_IDX_B;
    VoutPri[1] = VOUT_IDX_A;
    for (k = 0; k < MAX_VOUT_CHAN_NUM; k++) {
        /* Search corresponding VOUT */
        for (i = 0; i < DispNum; i++) {
            if (pDisp[i].VoutID == VoutPri[k]) {
                /* Search corresponding FOV */
                for (j = 0; j < pDisp[i].StrmCfg.NumChan; j++) {

                    for (s = 0U; s < CvFlowNum; s++) {
                        if ((CvFlowBits & ((UINT32) 1U << s)) > 0U) {
                            if ((pCvFlow[s].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                                (pCvFlow[s].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                                FovIdx = pCvFlow[s].InputCfg.Input[0].StrmId;
                                if (FovIdx == pDisp[i].StrmCfg.ChanCfg[j].FovId){
                                    Found = 1U;
                                    TargetCvChannel = s;
                                    break;
                                }
                            }
                        }
                    }

                    if (Found == 1U) {
                        pDispStrm = &pDisp[i];
                        pChanCfg  = &pDisp[i].StrmCfg.ChanCfg[j];
                        //Found = 1U;

                        TargetVout = (UINT8)pDisp[i].VoutID;

                        if (TargetVout == VOUT_IDX_A) {
                            g_SegDrawROI.Width = SEGDRAW_ROI_W_VOUTA;
                            g_SegDrawROI.Height = SEGDRAW_ROI_H_VOUTA;
                        } else {
                            g_SegDrawROI.Width = SEGDRAW_ROI_W_VOUTB;
                            g_SegDrawROI.Height = SEGDRAW_ROI_H_VOUTB;
                        }

                        /* Calculate OSD region with resepect to OSD buffer */
                        /* Suppose max OSD window is the same as Mixer window */
                        (void) SvcOsd_GetOsdBufSize(TargetVout, &OsdWidth, &OsdHeight);
                        MixerWidth  = pDispStrm->StrmCfg.MaxWin.Width;
                        MixerHeight = pDispStrm->StrmCfg.MaxWin.Height;

                        if (pDispStrm->VideoRotateFlip == (UINT8) AMBA_DSP_ROTATE_0_HORZ_FLIP) {
                            Value = (UINT32) pDispStrm->StrmCfg.Win.Width - ((UINT32) pChanCfg->DstWin.OffsetX + (UINT32) pChanCfg->DstWin.Width);
                        } else {
                            Value = pChanCfg->DstWin.OffsetX;
                        }
                        g_Canvas.StartX = ((Value + ((MixerWidth - pDispStrm->StrmCfg.Win.Width) >> 1)) * OsdWidth) / MixerWidth;
                        g_Canvas.StartY = ((pChanCfg->DstWin.OffsetY + ((MixerHeight - pDispStrm->StrmCfg.Win.Height) >> 1)) * OsdHeight) / MixerHeight;
                        g_Canvas.Width  = (pChanCfg->DstWin.Width * OsdWidth) / MixerWidth;
                        g_Canvas.Height = (pChanCfg->DstWin.Height * OsdHeight) / MixerHeight;

                        /* Seg ROI */
                        SvcSegDrawTask_ROI(TargetVout, FovIdx);
                        AmbaPrint_PrintUInt5("SvcSegDrawTask_FoVResInit() g_Canvas StartX %u StartY %u Width %u Height %u",
                                            g_Canvas.StartX,
                                            g_Canvas.StartY,
                                            g_Canvas.Width,
                                            g_Canvas.Height, 0U);
                        AmbaPrint_PrintUInt5("SvcSegDrawTask_FoVResInit() Found FOV%d DispStrm[%u].StrmCfg.ChanCfg[%u]", FovIdx, i, j, 0U, 0U);
                        break;

                    }
                }
            }
            if (1U == Found) {
                break;
            }
        }
        if (1U == Found) {
            break;
        }
    }
}

/**
 *  Start Segmentation osd draw module
 *  @return error code
 */
UINT32 SvcSegDrawTask_Start(void)
{
    UINT32 RetVal = SVC_OK;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 i;
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;


    AmbaSvcWrap_MisraMemset(SegDraw, 0, sizeof(SegDraw));


    SvcSegDrawTask_FoVResInit();

    /* Canvas register */
    SvcGui_Register(TargetVout, SVC_NOTIFY_GUI_DRAW_LANE, "SegDraw", SvcSegGui_Draw, SvcSegGui_Update);

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG) || (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG)) {
                RetVal |= SvcCvFlow_Register(i, SvcSegDraw_DetectionResultCallback, &CvFlowRegisterID[i]);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "SvcSegDrawTask_Init err. SvcCvFlow_Register failed %d", RetVal, 0U);
                }
            }
        }
    }
    HaveSegData &= ~(SEGDRAW_GUI_SHOW | SEGDRAW_GUI_UPDATE);

    return RetVal;
}

/**
 *  Stop Segmentation osd draw module
 *  @return error code
 */
UINT32 SvcSegDrawTask_Stop(void)
{
    UINT32 RetVal = SVC_OK;
    UINT32 i;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;

    for (i = 0U; i < CvFlowNum; i++) {
        if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
            if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG) || (pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG)) {
                RetVal |= SvcCvFlow_Unregister(i, CvFlowRegisterID[i]);
                if (SVC_OK != RetVal) {
                    SvcLog_NG(SVC_LOG_SEG_TASK, "SvcSegDrawTask_Stop err. SvcCvFlow_Register failed %d", RetVal, 0U);
                }
            }
        }
    }

    /* Un-register Gui canvas*/
    SvcGui_Unregister(TargetVout, SVC_NOTIFY_GUI_DRAW_LANE);

    HaveSegData &= ~(SEGDRAW_GUI_SHOW);

    return RetVal;
}

/**
 *  Get Canvas ROI info
 *  @return Canvas info
 */
SVC_GUI_CANVAS_s* SvcSegDrawTask_GetCanvasROIinfo(void)
{
    return (&g_Canvas);
}

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
/**
 *  ADAS module, LDWS message draw
 *  @param[in] MsgCode message code to point out which LDWS msg draw
 *  @param[out] pInfo LDWS draw info
 *  @return error code
 */
UINT32 SvcSegDrawTask_LdMsg(UINT32 MsgCode, const void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    switch (MsgCode) {
    case SVC_LW_DRAW:
        AmbaSvcWrap_MisraMemcpy(&g_LDout, pInfo, sizeof(SVC_LDWS_OUT_s));
        break;
    default:
        RetVal = SVC_NG;
        break;
    }

    return RetVal;
}
#endif

static void SvcSegDraw_StatusCallback(UINT32 StatIdx, void *pInfo)
{
    SVC_APP_STAT_DISP_s *pDispStatus = NULL;

    AmbaMisra_TouchUnused(pInfo);
    AmbaMisra_TouchUnused(pDispStatus);

    if (StatIdx == SVC_APP_STAT_DISP){
        AmbaMisra_TypeCast(&pDispStatus, &pInfo);
        if(pDispStatus->Status == SVC_APP_STAT_DISP_CHG){
            SvcSegDrawTask_FoVResInit();
            SvcLog_DBG(SVC_LOG_SEG_TASK, "Display status changed. SegDraw setting should be reset", 0U, 0U);
        }else{
            /* Do nothing now*/
        }
    }
}
