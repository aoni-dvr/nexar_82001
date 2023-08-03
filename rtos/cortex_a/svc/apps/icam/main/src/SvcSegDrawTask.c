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
#include "AmbaWS_FCWS.h"
#include "SvcAdasNotify.h"
#include "SvcAppStat.h"
#include "SvcMem.h"
#include "SvcBuffer.h"
#include "SvcBufMap.h"

#if defined(CONFIG_ICAM_CV_LOGGER)
#include "AmbaVfs.h"
#include "SvcCvLogger.h"
#include "SvcCvLoggerTask.h"
#include "RefFlow_Common.h"
#include "cvapi_flexidag_ref_util.h"

#endif

#define SVC_LOG_SEG_TASK     "SEG_DRAW_TASK"

#define SVC_CV_FLOW_CHAN_MAX            (16U)
#define SEGDRAW_ROI_W_VOUTA             (1808U)
#define SEGDRAW_ROI_H_VOUTA             (723U)
#define SEGDRAW_ROI_W_VOUTB             (1808U)
#define SEGDRAW_ROI_H_VOUTB             (723U)
#define SEGDRAW_ROI_W_MAX               (1808U)
#define SEGDRAW_ROI_H_MAX               (720U)

#define SEGDRAW_GUI_SHOW                (0x01U)
#define SEGDRAW_GUI_UPDATE              (0x02U)

#ifdef CONFIG_ICAM_32BITS_OSD_USED
#define SEGDRAW_PIXEL_SIZE_SFT          (2U)   /* 2: 32 bit */
#define SEGDRAW_BUF_SIZE                ((SEGDRAW_ROI_W_MAX * SEGDRAW_ROI_H_MAX) << SEGDRAW_PIXEL_SIZE_SFT)
#define SEGDRAW_DISCARD_RATIO_VERTICAL  (0.0) /*Setting for data discard. Will reduce drawing area if not 0*/
#else
#define SEGDRAW_PIXEL_SIZE_SFT          (0U)   /* 0: 8 bit */
#define SEGDRAW_BUF_SIZE                (SEGDRAW_ROI_W_MAX * SEGDRAW_ROI_H_MAX)
#define SEGDRAW_DISCARD_RATIO_VERTICAL  (0.0) /*Setting for data discard. Will reduce drawing area if not 0*/
#endif
#define SEGDRAW_RESCALE_TO_INTER_BUF    (0U)   /* Rescale to intermediate buffer before copying to OSD buffer */

#define MAX_VOUT_CHAN_NUM      (2U)

/* The Cv Flow registered ID */
static UINT32 CvFlowRegisterID[SVC_CV_FLOW_CHAN_MAX] = {0};
static UINT32 HaveSegData = 0U;
static UINT8 *pOsdUpdateSldBuf GNU_SECTION_NOZEROINIT;
static UINT32 MaskWidth = 0, MaskHeight = 0;
static AMBA_DSP_WINDOW_DIMENSION_s HierWin = {0};
static SVC_GUI_CANVAS_s g_Canvas GNU_SECTION_NOZEROINIT;
static UINT32 g_RatioX, g_RatioY GNU_SECTION_NOZEROINIT;
static SVC_SEG_DRAW_OSD_ROI_s g_SegDrawROI GNU_SECTION_NOZEROINIT;
static UINT8  TargetVout = VOUT_IDX_B;
static UINT32 TargetCvChannel = SVC_CV_FLOW_CHAN_MAX + 1U;

static UINT8 *g_pPixelMapTable, *g_pLineMapTable GNU_SECTION_NOZEROINIT;
static UINT32 g_PixelMapTableLength, g_LineMapTableLength GNU_SECTION_NOZEROINIT;
static UINT8 SegDraw[SEGDRAW_BUF_SIZE] GNU_SECTION_NOZEROINIT;
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
static SVC_LDWS_OUT_s g_LDout;
/* LKA needed */
static SVC_CALIB_ADAS_INFO_GET_s *g_pLKACalibData = NULL;
#endif

#if defined(CONFIG_ICAM_CV_LOGGER)
static void SegDrawTask_DataLoggerInit(void);
static ULONG g_SegBufBase = 0U;
static UINT32 g_SegBufferSize = 0U;
static UINT32 g_SegSaveNum = 1U;
#endif

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
void DrawLKASteeringInfo(const AMBA_AP_LKA_RESULT_s* pLkaRes);
#endif
static void SvcSegDraw_StatusCallback(UINT32 StatIdx, void *pInfo);

static void SvcSegDrawTask_ROI(UINT32 VoutChan, UINT32 FovIdx)
{
#define SQRT2                         ((FLOAT) 1.414213562)
#define SQRT2xSQRT2                   ((FLOAT) 2)
#define SQRT2xSQRT2xSQRT2             ((FLOAT) 2.828427124)
#define SQRT2xSQRT2xSQRT2xSQRT2       ((FLOAT) 4)
#define SQRT2xSQRT2xSQRT2xSQRT2xSQRT2 ((FLOAT) 5.656854248)

    FLOAT Tmp = 0.0f;
    UINT32 i, RetVal;
    SVC_CV_ROI_INFO_s  RoiInfo = {0};
    SVC_CV_MASK_INFO_s MaskInfo = {0};
    AMBA_DSP_WINDOW_DIMENSION_s MainWin;
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

#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
void DrawLKASteeringInfo(const AMBA_AP_LKA_RESULT_s* pLkaRes)
{
    const AMBA_AP_LKA_STEERING_INFO_s* pSteerInfo = &pLkaRes->SteeringInfo[AMBA_SR_LANE_EGO];
    UINT32 Flag = 0U, RetVal;
    UINT32 LKA_Radius = SVC_LW_LKA_RADIUS;

    if ((pSteerInfo->Status == AMBA_AP_LKA_STATUS_NEUTRAL_STEERING) ||
        (pSteerInfo->Status == AMBA_AP_LKA_STATUS_UNDER_STEERING)) {
        Flag = 1U;
    }

    if (Flag == 1U) {
        UINT32 centre[2];
        UINT32 OsdBufWidth, OsdBufHeight;

        RetVal = SvcOsd_GetOsdBufSize(TargetVout, &OsdBufWidth, &OsdBufHeight);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_GetOsdBufSize err %d", RetVal, 0U);
        }
        /** Draw steering wheel circle */
        centre[0] = (OsdBufWidth/2U);
        centre[1] = OsdBufHeight - (LKA_MINUS_ICON_H + 10U + LKA_Radius);

        RetVal = SvcOsd_DrawCircle(TargetVout, centre[0], centre[1], LKA_Radius, 5U, 0xAAffff00U);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawCircle err %d", RetVal, 0U);
        }

        { // Draw T
            DOUBLE angle;
            UINT32 pos[3][2]; /**< right, left and bottom point */
            DOUBLE cosVal;
            DOUBLE sinVal;
            UINT32 loop;
            DOUBLE WheelAngle = ((pSteerInfo->SteeringWheelAngle)/360.0) + 0.5;
            DOUBLE TmpV, TmpCentre;
            INT64 DoubleToINT64;

            RetVal = AmbaWrap_abs((INT32)(WheelAngle), &loop);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_abs err %d", RetVal, 0U);
            }
            /** right point */
            angle = (pSteerInfo->SteeringWheelAngle) * 3.14159265359 / 180.0;
            RetVal = AmbaWrap_cos(angle, &cosVal);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_cos err %d", RetVal, 0U);
            }
            RetVal = AmbaWrap_sin(angle, &sinVal);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_sin err %d", RetVal, 0U);
            }

            TmpV = (DOUBLE)LKA_Radius * cosVal;
            TmpCentre = (DOUBLE)centre[0];
            TmpCentre = TmpCentre + TmpV;
            DoubleToINT64 = (INT64)TmpCentre;
            AmbaMisra_TypeCast32(&(pos[0][0]), &DoubleToINT64);

            TmpV = (DOUBLE)LKA_Radius * sinVal;
            TmpCentre = (DOUBLE)centre[1];
            TmpCentre = TmpCentre - TmpV;
            DoubleToINT64 = (INT64)TmpCentre;
            AmbaMisra_TypeCast32(&(pos[0][1]), &DoubleToINT64);
//            pos[0][0] = (UINT32)(centre[0] + (UINT32)(((DOUBLE)LKA_Radius*cosVal) + 0.5));
//            pos[0][1] = (UINT32)(centre[1] - (UINT32)(((DOUBLE)LKA_Radius*sinVal) + 0.5));

            /** left point */
            angle = (pSteerInfo->SteeringWheelAngle + 180.0) * 3.14159265359 / 180.0;
            RetVal = AmbaWrap_cos(angle, &cosVal);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_cos err %d", RetVal, 0U);
            }
            RetVal = AmbaWrap_sin(angle, &sinVal);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_sin err %d", RetVal, 0U);
            }

            TmpV = (DOUBLE)LKA_Radius * cosVal;
            TmpCentre = (DOUBLE)centre[0];
            TmpCentre = TmpCentre + TmpV;
            DoubleToINT64 = (INT64)TmpCentre;
            AmbaMisra_TypeCast32(&(pos[1][0]), &DoubleToINT64);

            TmpV = (DOUBLE)LKA_Radius * sinVal;
            TmpCentre = (DOUBLE)centre[1];
            TmpCentre = TmpCentre - TmpV;
            DoubleToINT64 = (INT64)TmpCentre;
            AmbaMisra_TypeCast32(&(pos[1][1]), &DoubleToINT64);
//            pos[1][0] = (UINT32)((DOUBLE)centre[0] + (DOUBLE)(((DOUBLE)LKA_Radius*cosVal) + 0.5));
//            pos[1][1] = (UINT32)((DOUBLE)centre[1] - (DOUBLE)(((DOUBLE)LKA_Radius*sinVal) + 0.5));

            /** bottom point */
            angle = (pSteerInfo->SteeringWheelAngle + 270.0) * 3.14159265359 / 180.0;
            RetVal = AmbaWrap_cos(angle, &cosVal);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_cos err %d", RetVal, 0U);
            }
            RetVal = AmbaWrap_sin(angle, &sinVal);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_sin err %d", RetVal, 0U);
            }

            TmpV = (DOUBLE)LKA_Radius * cosVal;
            TmpCentre = (DOUBLE)centre[0];
            TmpCentre = TmpCentre + TmpV;
            DoubleToINT64 = (INT64)TmpCentre;
            AmbaMisra_TypeCast32(&(pos[2][0]), &DoubleToINT64);

            TmpV = (DOUBLE)LKA_Radius * sinVal;
            TmpCentre = (DOUBLE)centre[1];
            TmpCentre = TmpCentre - TmpV;
            DoubleToINT64 = (INT64)TmpCentre;
            AmbaMisra_TypeCast32(&(pos[2][1]), &DoubleToINT64);
//            pos[2][0] = (INT32)((DOUBLE)centre[0] + (DOUBLE)LKA_Radius*cosVal + 0.5);
//            pos[2][1] = (INT32)((DOUBLE)centre[1] - (DOUBLE)LKA_Radius*sinVal + 0.5);

            /* Draw T line */
            RetVal = SvcOsd_DrawLine(TargetVout, pos[0][0], pos[0][1], pos[1][0], pos[1][1], 5U, 0xAAffff00U);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawLine err %d", RetVal, 0U);
            }
            RetVal = SvcOsd_DrawLine(TargetVout, pos[2][0], pos[2][1], centre[0], centre[1], 5U, 0xAAffff00U);
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawLine err %d", RetVal, 0U);
            }
        }
    }
}

static void SvcSeg_PolyLine(const INT32 *xList, const INT32 *yList, UINT32 listNum)
{
    UINT32 RetVal = 0U;
    if (listNum >= 2U){
        for (UINT32 i = 0U; i < (listNum-1U); i++){
            UINT32 X1 = ((UINT32)(xList[i])/g_RatioX) + g_Canvas.StartX;
            UINT32 Y1 = ((UINT32)(yList[i])/g_RatioY) + g_Canvas.StartY;
            UINT32 X2 = ((UINT32)(xList[i+1U])/g_RatioX) + g_Canvas.StartX;
            UINT32 Y2 = ((UINT32)(yList[i+1U])/g_RatioY) + g_Canvas.StartY;
            RetVal = SvcOsd_DrawLine(TargetVout, X1, Y1, X2, Y2, 5U, 0x800000ffU);//Draw blue line
            if (SVC_OK != RetVal) {
                SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawLine err %d", RetVal, 0U);
            }
        }
    } else {
        AmbaPrint_PrintStr5("Two points at least.", NULL, NULL, NULL, NULL, NULL);
    }
}

static UINT32 SvcSeg_CheckBoundary(INT32 X, INT32 Y, INT32 RoiX, INT32 RoiY, INT32 RoiW, INT32 RoiH)
{
    UINT32 Rval = 0U;
    if ((X < RoiX) || (X > (RoiX+RoiW-1)) || (Y < RoiY) || (Y > (RoiY+RoiH-1))) {
        Rval = 1U;
    }
    return Rval;
}

static UINT32 SvcSeg_TransferWorld2SrcV1(AMBA_SR_LINE_PNT_3D_s LineWorld, const AMBA_CAL_EM_CALC_COORD_CFG_V1_s* pCalInfo, AMBA_SR_LINE_PNT_2D_s *LineSrc)
{
    /**
    *  Width, Height: Source image's width and height
    */
    UINT32 Rval;
    AMBA_CAL_POINT_DB_3D_s pnt3D;
    AMBA_CAL_POINT_DB_2D_s pnt2D;

    pnt3D.X = LineWorld.X;
    pnt3D.Y = LineWorld.Y;
    pnt3D.Z = LineWorld.Z;

    Rval = AmbaCal_EmConvPtWorldToImgV1(pCalInfo, &pnt3D, &pnt2D);

    if (Rval == CAL_OK) {
        LineSrc->X = (INT32)pnt2D.X;
        LineSrc->Y = (INT32)pnt2D.Y;
        Rval = SvcSeg_CheckBoundary(LineSrc->X, LineSrc->Y, (INT32)pCalInfo->Roi.StartX, (INT32)pCalInfo->Roi.StartY, (INT32)pCalInfo->Roi.Width, (INT32)pCalInfo->Roi.Height);
    }
    return Rval;
}


static UINT32 SvcSeg_TransferWorld2Src(AMBA_SR_LINE_PNT_3D_s LineWorld, const AMBA_CAL_EM_CALC_COORD_CFG_s* pCalInfo, AMBA_SR_LINE_PNT_2D_s *LineSrc)
{
    /**
    *  Width, Height: Source image's width and height
    */
    UINT32 Rval;
    AMBA_CAL_POINT_DB_3D_s pnt3D;
    AMBA_CAL_POINT_DB_2D_s pnt2D;

    pnt3D.X = LineWorld.X;
    pnt3D.Y = LineWorld.Y;
    pnt3D.Z = LineWorld.Z;

    Rval = AmbaCal_EmConvPtWorldToImgPlane(pCalInfo, &pnt3D, &pnt2D);

    if (Rval == CAL_OK) {
        LineSrc->X = (INT32)pnt2D.X;
        LineSrc->Y = (INT32)pnt2D.Y;
        Rval = SvcSeg_CheckBoundary(LineSrc->X, LineSrc->Y, (INT32)pCalInfo->Roi.StartX, (INT32)pCalInfo->Roi.StartY, (INT32)pCalInfo->Roi.Width, (INT32)pCalInfo->Roi.Height);
    }

    return Rval;
}

static void DrawLKAPredictivePath(const AMBA_AP_LKA_RESULT_s* pLkaRes)
{
    UINT32 Rval = SVC_OK;
    const AMBA_AP_LKA_STEERING_INFO_s* pSteerInfo = &pLkaRes->SteeringInfo[AMBA_SR_LANE_EGO];
    UINT32 Flag = 0U;

    if ((pSteerInfo->Status == AMBA_AP_LKA_STATUS_NEUTRAL_STEERING) ||
        (pSteerInfo->Status == AMBA_AP_LKA_STATUS_UNDER_STEERING)) {
        Flag = 1U;
    }

    if (Flag == 1U) {
        AMBA_SR_LINE_PNT_2D_s pnt2d;
        INT32 xlist[AMBA_SR_MAX_PNT_NUM];  /**< x list For SvcSeg_PolyLine */
        INT32 ylist[AMBA_SR_MAX_PNT_NUM];  /**< y list For SvcSeg_PolyLine */
        UINT32 drawPntNum = 0U;
        UINT32 /*MaxPntNum = 5U,*/ DrawPntNum;


        if (pSteerInfo->PredictivePath.PntNum > 1U) {

            DrawPntNum = (pSteerInfo->PredictivePath.PntNum);
            for (UINT32 p = 0U; p < (DrawPntNum - 1U); p++) {
                if (SVC_CALIB_ADAS_VERSION_V1 == SvcCalib_AdasCfgGetCalVer(SVC_CALIB_ADAS_TYPE_FRONT)) {
                    Rval = SvcSeg_TransferWorld2SrcV1(pSteerInfo->PredictivePath.PntData[p], &g_pLKACalibData->AdasCfgV1, &pnt2d);
                } else {
                    Rval = SvcSeg_TransferWorld2Src(pSteerInfo->PredictivePath.PntData[p], &g_pLKACalibData->AdasCfg, &pnt2d);
                }
                if (Rval == 0U) {
                    xlist[drawPntNum] = pnt2d.X;
                    ylist[drawPntNum] = pnt2d.Y;
                    drawPntNum ++;
                }
            }
            if (drawPntNum >= 1U) {
                SvcSeg_PolyLine(xlist, ylist, drawPntNum);
            }
        }
    }
}

static void DrawLKACanbusInfo(const AMBA_AP_LKA_CONFIG_s* pLkaCfg, const AMBA_SR_CANBUS_TRANSFER_DATA_s* pCanbusTrData) {

    DOUBLE XWA = pCanbusTrData->TransferWheelAngle;
    UINT32 centre[2];
    UINT32 LKA_Radius = SVC_LW_LKA_RADIUS;
    DOUBLE angle;
    UINT32 pos[3][2];
    DOUBLE cosVal;
    DOUBLE sinVal;
    UINT32 OsdBufWidth, OsdBufHeight;
    DOUBLE GearRatio;
    DOUBLE SteerAngle;
    INT32  dir;
    DOUBLE TmpV, TmpCentre;
    INT64 DoubleToINT64;
    UINT32 RetVal = 0U;


    if (pCanbusTrData->WheelDir == 0U) {
        dir = 1;
    } else {
        dir = -1;
    }

    GearRatio = pLkaCfg->SteeringSpec.SteeringGearRatio[(UINT32)pCanbusTrData->TransferSpeed];
    SteerAngle = GearRatio * XWA * (DOUBLE)dir;

    /** Draw steering wheel circle */
    RetVal = SvcOsd_GetOsdBufSize(TargetVout, &OsdBufWidth, &OsdBufHeight);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_GetOsdBufSize err %d", RetVal, 0U);
    }
    centre[0] = (OsdBufWidth/2U);
    centre[1] = (OsdBufHeight*5U/6U);

    RetVal = SvcOsd_DrawCircle(TargetVout, centre[0], centre[1], LKA_Radius, 5U, 0x80ffff00U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawCircle err %d", RetVal, 0U);
    }

    /** right point */
    angle = SteerAngle;
    RetVal = AmbaWrap_cos(angle, &cosVal);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_cos err %d", RetVal, 0U);
    }
    RetVal = AmbaWrap_sin(angle, &sinVal);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_sin err %d", RetVal, 0U);
    }

    TmpV = (DOUBLE)LKA_Radius * cosVal;
    TmpCentre = (DOUBLE)centre[0];
    TmpCentre = TmpCentre + TmpV;
    DoubleToINT64 = (INT64)TmpCentre;
    AmbaMisra_TypeCast32(&(pos[0][0]), &DoubleToINT64);

    TmpV = (DOUBLE)LKA_Radius * sinVal;
    TmpCentre = (DOUBLE)centre[1];
    TmpCentre = TmpCentre - TmpV;
    DoubleToINT64 = (INT64)TmpCentre;
    AmbaMisra_TypeCast32(&(pos[0][1]), &DoubleToINT64);
//            pos[0][0] = (UINT32)(centre[0] + (UINT32)(((DOUBLE)LKA_Radius*cosVal) + 0.5));
//            pos[0][1] = (UINT32)(centre[1] - (UINT32)(((DOUBLE)LKA_Radius*sinVal) + 0.5));


    /** left point */
    angle = (SteerAngle + 3.14159265359);
    RetVal = AmbaWrap_cos(angle, &cosVal);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_cos err %d", RetVal, 0U);
    }
    RetVal = AmbaWrap_sin(angle, &sinVal);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_sin err %d", RetVal, 0U);
    }

    TmpV = (DOUBLE)LKA_Radius * cosVal;
    TmpCentre = (DOUBLE)centre[0];
    TmpCentre = TmpCentre + TmpV;
    DoubleToINT64 = (INT64)TmpCentre;
    AmbaMisra_TypeCast32(&(pos[1][0]), &DoubleToINT64);

    TmpV = (DOUBLE)LKA_Radius * sinVal;
    TmpCentre = (DOUBLE)centre[1];
    TmpCentre = TmpCentre - TmpV;
    DoubleToINT64 = (INT64)TmpCentre;
    AmbaMisra_TypeCast32(&(pos[1][1]), &DoubleToINT64);
//            pos[1][0] = (UINT32)((DOUBLE)centre[0] + (DOUBLE)(((DOUBLE)LKA_Radius*cosVal) + 0.5));
//            pos[1][1] = (UINT32)((DOUBLE)centre[1] - (DOUBLE)(((DOUBLE)LKA_Radius*sinVal) + 0.5));


    /** bottom point */
    angle = SteerAngle + (3.14159265359 * 1.5);
    RetVal = AmbaWrap_cos(angle, &cosVal);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_cos err %d", RetVal, 0U);
    }
    RetVal = AmbaWrap_sin(angle, &sinVal);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_sin err %d", RetVal, 0U);
    }


    TmpV = (DOUBLE)LKA_Radius * cosVal;
    TmpCentre = (DOUBLE)centre[0];
    TmpCentre = TmpCentre + TmpV;
    DoubleToINT64 = (INT64)TmpCentre;
    AmbaMisra_TypeCast32(&(pos[2][0]), &DoubleToINT64);

    TmpV = (DOUBLE)LKA_Radius * sinVal;
    TmpCentre = (DOUBLE)centre[1];
    TmpCentre = TmpCentre - TmpV;
    DoubleToINT64 = (INT64)TmpCentre;
    AmbaMisra_TypeCast32(&(pos[2][1]), &DoubleToINT64);
//            pos[2][0] = (INT32)((DOUBLE)centre[0] + (DOUBLE)LKA_Radius*cosVal + 0.5);
//            pos[2][1] = (INT32)((DOUBLE)centre[1] - (DOUBLE)LKA_Radius*sinVal + 0.5);


    /* Draw T line */
    RetVal = SvcOsd_DrawLine(TargetVout, pos[0][0], pos[0][1], pos[1][0], pos[1][1], 5U, 0x80ffff00U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawLine err %d", RetVal, 0U);
    }
    RetVal = SvcOsd_DrawLine(TargetVout, pos[2][0], pos[2][1], centre[0], centre[1], 5U, 0x80ffff00U);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawLine err %d", RetVal, 0U);
    }
}
#endif

static void SvcSegGui_Draw(UINT32 VoutIdx, UINT32 Level)
{
    #define SEGDRAW_COLOR_IDX_OFFSET      (48U)
    static UINT32 OsdBufWidth, OsdBufHeight, OsdXend, OsdYend, Tmp;
    static SVC_SEG_DRAW_OSD_ROI_s SegDrawROI;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    /*
       Main    : 1920U*1080U            OSD buffer = 960U*540U
       Seg ROI : 1280U*512U             Seg ROI on OSD buf size = 900U*360U
       Seg 1 step => 1360U*764U                           (Offset 28U*128U)
              (Offset 40U*180U)


           .-> offset = 40*180               .-> offset = 28*128
           .                                 .
          _._____1360______                 _._____960_______
         | ._____________  |               | ._____________  |
         | |    1280     | |               | |     900     | |
      764| |          512| |   ======>  540| |          360| |
         | |_____________| |               | |_____________| |
         |_________________|               |_________________|
    */
    /*
     * Scaling:
     * 1. Original ROI = 1280U*512U
     *   Main YUV = 1920U*1080U => Seg 1 step => 1360U*764U (LDWS draw will offset 40U*180U)
     * 2. Seg output size = 320U*128U
     * 3. OSD buffer = 960U*540U
     *    => So Seg on OSD buf size = 900U*360U (Offset 28U*128U)
     * 4. Assume (320U*128U) for 3X scaling to fit OSD buffer BUT:
     *    a. Every 16 line will skip 1 line on X direction
     *    b. Every 16 line will skip 1 line on Y direction
     *       => So: (320U*128U) -> 3x -> (960U*384U) -> After skip -> 900U*360U
     */
    if ((HaveSegData & SEGDRAW_GUI_SHOW) > 0U) {
        UINT32 RetVal;

#if 0 /* Old method, process/evaluate every single pixel. */
        UINT32 i, j, Idx;

        for (i = 0; i< 960U; i++) {//320U*3U = 960U
            Idx = 0;
            if (0U == (i % 16U)) {//Every 16 line will skip 1 line
                continue;
            } else {
                for (j = 0; j< 384U; j++) {//128U*3U = 384U
                    if (0U == (j % 16U)) {//Every  16 line will skip 1 line
                        continue;
                    } else {
                        SegDraw[((Idx*900U) + i) - (i/16U)] = pOsdUpdateSldBuf[((j/3U)*320U) +(i/3U)];
                        /* For OSD lookup table, 40 ~ 43 are reserve for segmentation draw */
                        SegDraw[((Idx*900U) + i) - (i/16U)] += 40U;
                        Idx++;
                    }
                }
            }
        }

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

        RetVal = SvcOsd_DrawBin(TargetVout, SegDrawROI.Xstart, SegDrawROI.Ystart, OsdXend, OsdYend, SegDraw);
        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_SEG_TASK, "g_SegDrawROI.Xstart, g_SegDrawROI.Ystart = %d, %d", g_SegDrawROI.Xstart, g_SegDrawROI.Ystart);
            SvcLog_NG(SVC_LOG_SEG_TASK, "OsdXend, OsdYend = %d, %d", OsdXend, OsdYend);
            SvcLog_NG(SVC_LOG_SEG_TASK, "SvcOsd_DrawBin DrawBin error !", 0U, 0U);
        } else {
            //Do nothing
        }

#else   /* New method, SvcCvImgUtil_RescaleBitMap_Rept() */
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

        Cfg.SrcWidth = MaskWidth;
        Cfg.SrcHeight = MaskHeight;
        Cfg.SrcPitch = MaskWidth;
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

        if (NumColor > SEGDRAW_COLOR_IDX_OFFSET) {
            Cfg.pClut = &pClut[SEGDRAW_COLOR_IDX_OFFSET];
            Cfg.NumColor = NumColor - SEGDRAW_COLOR_IDX_OFFSET;
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
#endif /* End of New method */

        /* Draw LDWS detect lane */
#if defined(CONFIG_ICAM_PROJECT_ADAS_DVR) && defined(CONFIG_BUILD_AMBA_ADAS)
        if (1U == g_LDout.LKAInfoIsAssign) {
            AMBA_AP_LKA_RESULT_s LKAInfo;

            AmbaSvcWrap_MisraMemcpy(&LKAInfo, &g_LDout.LKAInfo, sizeof(AMBA_AP_LKA_RESULT_s));
            DrawLKAPredictivePath(&LKAInfo);
//            DrawLKASteeringInfo(&LKAInfo);
        }
        if (1U == g_LDout.LKADebugFlag) {
            AMBA_AP_LKA_CONFIG_s LkaCfg;
            AMBA_SR_CANBUS_TRANSFER_DATA_s CanbusTrData;

            AmbaSvcWrap_MisraMemcpy(&LkaCfg, &g_LDout.LKAConfigInfo, sizeof(AMBA_AP_LKA_CONFIG_s));
            AmbaSvcWrap_MisraMemcpy(&CanbusTrData, &g_LDout.CanbusTrData, sizeof(AMBA_SR_CANBUS_TRANSFER_DATA_s));
            DrawLKACanbusInfo(&LkaCfg, &CanbusTrData);
        }
        if (1U == g_LDout.LaneInfoIsAssign) {
            AMBA_SR_LANE_MEMBER_s LDOut;
            UINT32 MaxPntNum = 5U;
            UINT32 DrawPntNum;
            UINT32 LaneInfoIdx;

            AmbaSvcWrap_MisraMemcpy(&LDOut, &(g_LDout.LaneInfo.LaneMember[AMBA_SR_LANE_EGO]), sizeof(AMBA_SR_LANE_MEMBER_s));

            LaneInfoIdx = AMBA_SR_LANE_LINE_LEFT0;
            if (1U < LDOut.Line2D[LaneInfoIdx].PntNum) {
                DrawPntNum = ((LDOut.Line2D[LaneInfoIdx].PntNum > MaxPntNum) ? (MaxPntNum) : (LDOut.Line2D[LaneInfoIdx].PntNum));
                for (UINT32 p = 0U; p < (DrawPntNum - 1U); p++) {
                    UINT32 pIdx0 = (LDOut.Line2D[LaneInfoIdx].PntNum - 1U) * (p) / (DrawPntNum - 1U);
                    UINT32 pIdx1 = (LDOut.Line2D[LaneInfoIdx].PntNum - 1U) * (p+1U) / (DrawPntNum - 1U);

                    UINT32 X1 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx0].X)/g_RatioX) + g_Canvas.StartX;
                    UINT32 Y1 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx0].Y)/g_RatioY) + g_Canvas.StartY;
                    UINT32 X2 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx1].X)/g_RatioX) + g_Canvas.StartX;
                    UINT32 Y2 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx1].Y)/g_RatioY) + g_Canvas.StartY;
                    RetVal = SvcOsd_DrawLine(TargetVout, X1, Y1, X2, Y2, 5U, 0x80c00040U);
                    if (SVC_NG == RetVal) {
                        SvcLog_NG(SVC_LOG_SEG_TASK, "[Lane][Left] SvcOsd_DrawLine error !", 0U, 0U);
                    }
                }
            }

            LaneInfoIdx = AMBA_SR_LANE_LINE_RIGHT0;
            if (1U < LDOut.Line2D[LaneInfoIdx].PntNum) {
                DrawPntNum = (LDOut.Line2D[LaneInfoIdx].PntNum > MaxPntNum)? (MaxPntNum) : (LDOut.Line2D[LaneInfoIdx].PntNum);
                for (UINT32 p = 0U; p < (DrawPntNum - 1U); p++) {
                    UINT32 pIdx0 = (LDOut.Line2D[LaneInfoIdx].PntNum - 1U) * (p) / (DrawPntNum - 1U);
                    UINT32 pIdx1 = (LDOut.Line2D[LaneInfoIdx].PntNum - 1U) * (p+1U) / (DrawPntNum - 1U);

                    UINT32 X1 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx0].X)/g_RatioX) + g_Canvas.StartX;
                    UINT32 Y1 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx0].Y)/g_RatioY) + g_Canvas.StartY;
                    UINT32 X2 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx1].X)/g_RatioX) + g_Canvas.StartX;
                    UINT32 Y2 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx1].Y)/g_RatioY) + g_Canvas.StartY;
                    RetVal = SvcOsd_DrawLine(TargetVout, X1, Y1, X2, Y2, 5U, 0x80c00040U);
                    if (SVC_NG == RetVal) {
                        SvcLog_NG(SVC_LOG_SEG_TASK, "[Lane][Right] SvcOsd_DrawLine error !", 0U, 0U);
                    }
                }
            }


            if (1U == g_LDout.LKADebugFlag) {
                LaneInfoIdx = AMBA_SR_LANE_LINE_CENTER;
                if (1U < LDOut.Line2D[LaneInfoIdx].PntNum) {
                    DrawPntNum = (LDOut.Line2D[LaneInfoIdx].PntNum > MaxPntNum)? (MaxPntNum) : (LDOut.Line2D[LaneInfoIdx].PntNum);
                    for (UINT32 p = 0U; p < (DrawPntNum - 1U); p++) {
                        UINT32 pIdx0 = (LDOut.Line2D[LaneInfoIdx].PntNum - 1U) * (p) / (DrawPntNum - 1U);
                        UINT32 pIdx1 = (LDOut.Line2D[LaneInfoIdx].PntNum - 1U) * (p+1U) / (DrawPntNum - 1U);

                        UINT32 X1 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx0].X)/g_RatioX) + g_Canvas.StartX;
                        UINT32 Y1 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx0].Y)/g_RatioY) + g_Canvas.StartY;
                        UINT32 X2 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx1].X)/g_RatioX) + g_Canvas.StartX;
                        UINT32 Y2 = ((UINT32)(LDOut.Line2D[LaneInfoIdx].PntData[pIdx1].Y)/g_RatioY) + g_Canvas.StartY;
                        RetVal = SvcOsd_DrawLine(TargetVout, X1, Y1, X2, Y2, 5U, 0x8000ff00U);
                        if (SVC_NG == RetVal) {
                            SvcLog_NG(SVC_LOG_SEG_TASK, "[Lane][Center] SvcOsd_DrawLine error !", 0U, 0U);
                        }
                    }
                }
            }

        }
#endif
    }
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
#if defined(CONFIG_ICAM_CV_LOGGER)
static void SegDrawTask_DataLoggerInit(void){
    UINT32 RetVal;
    UINT32 Addr;
    RetVal = SvcBuffer_Request(SVC_BUFFER_SHARED, SMEM_PF0_ID_CV_LOGGER_SEG, &g_SegBufBase, &g_SegBufferSize);

    AmbaMisra_TypeCast(&Addr, &g_SegBufBase);
    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_SEG_TASK, "SvcBuffer_Request ok size Base 0x%x, Size %d", Addr, g_SegBufferSize);
    } else {
        SvcLog_NG(SVC_LOG_SEG_TASK, "fail to SvcBuffer_Request", 0U, 0U);
    }
}

#define AMBASEG_OUT_BUF_NUM  (4U)

static void SegDrawTask_DataLoggerHandler(UINT32 CvFlowChan, void *pOutput)
{
    UINT32 RetVal, i;
    const UINT32 *pMsgCode;
    UINT32 RegSize, PayloadSize;
    SVC_CV_LOGGER_WRITE_BUFFER_s WrBuf = {.Size = 0};
    RF_LOG_HEADER_V0_s *pHeader;
    typedef struct { void **pDPtr; } VOID_PTR_s;
    VOID_PTR_s VoidPtr, VoidPtr2;

    SVC_CV_ROI_INFO_s  RoiInfo = {0};
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    const UINT8 *pSegBuf;
    UINT8 *pSegOutBuf;
    const SVC_CV_DETRES_SEG_s *pSegResult;
    SVC_CV_LOGGER_SEG_s *pSegHeader;
    UINT32 size = 0U;
    UINT32 EncodeFail = 0U;
    UINT32 BufferSize = 0U;
    const AMBA_CV_FLEXIDAG_IO_s *pOutputIO;
    UINT8 *pSegOut;
    UINT32 DataFmt[AMBASEG_OUT_BUF_NUM];
    DataFmt[0] = RF_COM_DFMT_SEG;
    DataFmt[1] = RF_COM_DFMT_SEG_COLOR;
    DataFmt[2] = RF_COM_DFMT_SEG_TYPE;
    DataFmt[3] = RF_COM_DFMT_SEG_INSTANCE;

    if (1U == SvcCvLoggerTask_IsEnable(CvFlowChan)) {
        AmbaMisra_TypeCast(&pMsgCode, &pOutput);
        AmbaMisra_TypeCast(&pSegResult, &pOutput);
        AmbaMisra_TypeCast(&pOutputIO, &pSegResult->pCvOut);

        for (i = 0U; i < CvFlowNum; i++) {
            if ((CvFlowBits & ((UINT32) 1U << i)) > 0U) {
                if ((pCvFlow[i].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                    (pCvFlow[i].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                    RetVal = SvcCvFlow_Control(i, SVC_CV_CTRL_GET_ROI_INFO, &RoiInfo);
                    if (SVC_OK != RetVal) {
                        SvcLog_NG(SVC_LOG_SEG_TASK, "SvcCvFlow_Control GET_ROI_INFO err %d", RetVal, 0U);
                    }
                }
            }
        }
        if (*pMsgCode == SVC_CV_DETRES_SEG) {
            for (i = 0U; i < g_SegSaveNum; i++) {
                AmbaMisra_TypeCast(&pSegBuf, &pOutputIO->buf[i].pBuffer);
                AmbaMisra_TypeCast(&pSegOut, &g_SegBufBase);
                EncodeFail = SvcCvLogger_RunLengthEncode(pSegBuf, pSegResult->Width, pSegResult->Height, pSegResult->Pitch, pSegOut, 655360U, &size);
                if (EncodeFail == 0U) {
                    BufferSize = size;
                    if (BufferSize > g_SegBufferSize) {
                        SvcLog_NG(SVC_LOG_SEG_TASK, "EncodeSize %d is bigger than g_SegBufferSize %d", BufferSize, g_SegBufferSize);
                    }
                } else {
                    BufferSize = pSegResult->Pitch * pSegResult->Height;
                }

                PayloadSize = (UINT32)sizeof(RF_LOG_SEG_BUF_HEADER_s) + (BufferSize * (UINT32)sizeof(UINT8));
                RegSize = (UINT32)SVC_CV_LOGGER_HEADER_SIZE + PayloadSize;
                WrBuf.Size = 0U;
                RetVal = SvcCvLoggerTask_TrunkBufferGet(CvFlowChan, RegSize, SVC_CV_LOGGER_FLAG_LOG_TRUNK, &WrBuf);
                if ((RetVal == SVC_OK) && (WrBuf.Size >= RegSize)) {
                    AmbaMisra_TypeCast(&pHeader, &WrBuf.Addr);
                    /* pLogOut = &pHeader->pDataAddr */
                    VoidPtr.pDPtr = &pHeader->pDataAddr;
                    AmbaMisra_TypeCast(&pSegHeader, &VoidPtr);

                    pHeader->Magic = RF_DATA_FMT_HDR_MGC;
                    pHeader->HdrVer   = RF_DATA_FMT_HDR_VER;
                    pHeader->DataFmt  = DataFmt[i];
                    pHeader->Count    = 0U;
                    pHeader->Channel  = 0U;
                    pHeader->CapPTS   = pSegResult->CaptureTime;
                    pHeader->DataSize = PayloadSize;

                    AmbaMisra_TouchUnused(&pSegHeader);
                    pSegHeader->SegHeader.Width = pSegResult->Width;
                    pSegHeader->SegHeader.Height = pSegResult->Height;
                    pSegHeader->SegHeader.Pitch = pSegResult->Width;
                    pSegHeader->SegHeader.TimeStamp = pSegResult->CaptureTime;
                    pSegHeader->SegHeader.FrameNum = pSegResult->FrameNum;

                    //AmbaPrint_PrintUInt5("Width %d Height %d Pitch %d Timestamp %d FrameNum %d", pCvLoggerSeg->SegHeader.Width, pCvLoggerSeg->SegHeader.Height, pCvLoggerSeg->SegHeader.Pitch, pCvLoggerSeg->SegHeader.TimeStamp, pCvLoggerSeg->SegHeader.FrameNum );
                    pSegHeader->SegHeader.CropInfo.RoiW = RoiInfo.Roi[0].Width;
                    pSegHeader->SegHeader.CropInfo.RoiH = RoiInfo.Roi[0].Height;
                    pSegHeader->SegHeader.CropInfo.RoiX = RoiInfo.Roi[0].StartX;
                    pSegHeader->SegHeader.CropInfo.RoiY = RoiInfo.Roi[0].StartY;
                    pSegHeader->SegHeader.CropInfo.SrcW = HierWin.Width;
                    pSegHeader->SegHeader.CropInfo.SrcH = HierWin.Height;
                    pSegHeader->SegHeader.BufferSize = BufferSize;

                    VoidPtr2.pDPtr = &pSegHeader->pBitmap;
                    AmbaMisra_TypeCast(&pSegOutBuf, &VoidPtr2);

                    if (EncodeFail == 0U) {
                        pSegHeader->SegHeader.EncodeMode = 1U;
                        RetVal = AmbaWrap_memcpy(pSegOutBuf, pSegOut, BufferSize * sizeof(UINT8));
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_memcpy err", 0U, 0U);
                        }
                    } else {
                         pSegHeader->SegHeader.EncodeMode = 0U;
                        RetVal = AmbaWrap_memcpy(pSegOutBuf, pSegBuf, BufferSize * sizeof(UINT8));
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_SEG_TASK, "AmbaWrap_memcpy err", 0U, 0U);
                        }
                    }

                    RetVal = SvcCvLoggerTask_TrunkBufferUpdate(CvFlowChan, &WrBuf, RegSize);
                    if (SVC_OK != RetVal) {
                        SvcLog_DBG(SVC_LOG_SEG_TASK, "TrunkBufferUpdate(%d) failed(0x%x)", CvFlowChan, RetVal);
                    }
                } else {
                    SvcLog_DBG(SVC_LOG_SEG_TASK, "CvFlowChan(%d) WriteBufferGet failed(0x%x)", CvFlowChan, RetVal);
                }
            }
        }
    }

    AmbaMisra_TouchUnused(pOutput);
}

/**
 *  Set Segmentation seg buffer number
 *  @UINT32 : save number
 */

void SvcSegDrawTask_SetSaveBufferNum(UINT32 Number) {
    g_SegSaveNum = Number;
}

#endif

static void SvcSegDraw_DetectionResultCallback(UINT32 Chan, UINT32 CvType, void *pDetResMsg)
{
    const SVC_CV_DETRES_SEG_s* pSegResult;

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

    if(Chan == TargetCvChannel){
        AmbaMisra_TouchUnused(&Chan);
        AmbaMisra_TouchUnused(&CvType);
        (void) pDetResMsg;
        AmbaMisra_TypeCast(&pSegResult, &pDetResMsg);
        AmbaMisra_TypeCast(&pOsdUpdateSldBuf, &pSegResult->pBuf);
        MaskWidth = pSegResult->Width;
        MaskHeight = pSegResult->Height;
        HaveSegData |= (SEGDRAW_GUI_SHOW | SEGDRAW_GUI_UPDATE);

#if defined(CONFIG_ICAM_CV_LOGGER)
        SegDrawTask_DataLoggerHandler(Chan, pDetResMsg);
#endif
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
    static SVC_CALIB_ADAS_INFO_GET_s Cfg;
    static AMBA_CAL_EM_CAM_CALIB_DATA_s CalibDataRaw2World;
    static AMBA_CAL_EM_CURVED_SURFACE_CFG_s CurvedSurface;
    AmbaSvcWrap_MisraMemset(&g_LDout, 0, sizeof(SVC_LDWS_OUT_s));
#endif
    AmbaSvcWrap_MisraMemset(&g_Canvas, 0, sizeof(g_Canvas));
    AmbaSvcWrap_MisraMemset(&g_SegDrawROI, 0, sizeof(g_SegDrawROI));
    g_RatioX = 0U;
    g_RatioY = 0U;

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
    g_RatioX = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Width) /(g_Canvas.Width);
    g_RatioY = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Height)/(g_Canvas.Height);

    /* Get calibration data which LKA needed */
    Cfg.AdasCfgV1.TransCfg.pCalibDataRaw2World = &CalibDataRaw2World;
    Cfg.AdasCfgV1.TransCfg.pCurvedSurface = &CurvedSurface;
    Rval = SvcCalib_AdasCfgGetV1(SVC_CALIB_ADAS_TYPE_FRONT, &Cfg);
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
#if defined(CONFIG_ICAM_CV_LOGGER)
    SegDrawTask_DataLoggerInit();
#endif

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

    g_RatioX = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Width) /(g_Canvas.Width);
    g_RatioY = ((UINT32)pResCfg->FovCfg[FovIdx].MainWin.Height)/(g_Canvas.Height);

    AmbaMisra_TouchUnused(&HierWin);
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
