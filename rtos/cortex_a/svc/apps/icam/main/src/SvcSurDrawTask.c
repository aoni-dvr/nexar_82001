/**
 *  @file SvcSurDrawTask.c
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
 *  @details Svc SvcSur Draw Task
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaPSD_ParkingSpaceDetect.h"
#include "AmbaOWS_ObstacleDetect.h"


#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcResCfg.h"
#include "SvcOsd.h"
#include "SvcGui.h"
#include "SvcCvFlow_Comm.h"
#include "SvcCvFlow.h"
#include "SvcCvAppDef.h"
#include "SvcSurDrawTask.h"
#include "RefFlow_RCTA.h"
#include "SvcRctaTask.h"
#include "SvcCalibAdas.h"


#define SVC_SUR_ADAS_GUI_LEVEL           (19U)
#define SVC_SUR_ADAS_FISHEYE_MODE        (0U)
#define SVC_SUR_ADAS_BIRDVIEW_MODE       (1U)
#define SVC_LOG_SUR_TASK     "SUR_ADAS_DRAW_TASK"


static SVC_GUI_CANVAS_s g_Canvas GNU_SECTION_NOZEROINIT;
static AMBA_PSD_PROC_OUTPUT_DATA_s g_SurDrawPsdOut GNU_SECTION_NOZEROINIT;
static AMBA_OWS_RESULT_s g_SurDrawOwsOut GNU_SECTION_NOZEROINIT;
static SVC_RCTA_TASK_DRAW_DATA_s g_SurDrawRctaOut GNU_SECTION_NOZEROINIT;
static UINT8 SurDisplayMode = SVC_SUR_ADAS_FISHEYE_MODE;
static DOUBLE RatioX = 0.0, RatioY = 0.0;
static UINT32 OffsetX = 0, OffsetY = 0;
static UINT8 DebugOsdEnable = 0U;
static AMBA_CAL_SIZE_s CarSize;
static DOUBLE DisplayW = 512.0, DisplayH = 704.0;
static DOUBLE CarSizeVoutW = 138.0, CarSizeVoutH = 332.0;

static UINT32 RctaDrawWarningZone(UINT32 PntNum, const AMBA_CAL_POINT_INT_2D_s *Pnt, UINT32 Color)
{
    UINT32 i;
    UINT32 RetVal = SVC_OK;
    UINT32 X1, Y1, X2, Y2;
    DOUBLE TmpDouble;

    for (i = 0U; i < (PntNum - 1U); i++) {
        TmpDouble = (DOUBLE)Pnt[i].X/RatioX;
        X1 = (UINT32)TmpDouble + OffsetX;
        TmpDouble = (DOUBLE)Pnt[i+1U].X/RatioX;
        X2 = (UINT32)TmpDouble + OffsetX;
        TmpDouble = (DOUBLE)Pnt[i].Y/RatioY;
        Y1 = (UINT32)TmpDouble + OffsetY;
        TmpDouble = (DOUBLE)Pnt[i+1U].Y/RatioY;
        Y2 = (UINT32)TmpDouble + OffsetY;
        if ((X1 < (g_Canvas.Width + g_Canvas.StartX)) && (X2 < (g_Canvas.Width  + g_Canvas.StartX)) &&
            (Y1 < (g_Canvas.Height + g_Canvas.StartY)) && (Y2 < (g_Canvas.Height + g_Canvas.StartY))) {
            RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2,3U, Color);
            if (SVC_NG == RetVal) {
                //SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error %d!", i, 0U);
            }
        }
    }

    return SVC_OK;
}

static UINT32 RctaDraw(UINT8 Dire, UINT8 Type, const AMBA_WS_RCTA_WARNING_INFO_s *Warning)
{
    UINT32 RetVal = SVC_OK;
    if ((Warning->WarningStat != AMBA_WS_RCTA_STATUS_NO_WARNING) && (Warning->WarningStat !=  AMBA_WS_RCTA_STATUS_UNACTIVE)) {
        UINT32 DrawColor = 0U;
        UINT32 X1, Y1, X2, Y2, Id, i;
        UINT32 ExclamationX;

        if (Dire == 1U){
            ExclamationX = 864U;
        } else {
            ExclamationX = 96U;
        }

        if ((Warning->WarningStat == AMBA_WS_RCTA_STATUS_LV1_LEFT) || (Warning->WarningStat == AMBA_WS_RCTA_STATUS_LV1_RIGHT)) {
            DrawColor = SUR_DRAW_YELLOW;
        } if ((Warning->WarningStat == AMBA_WS_RCTA_STATUS_LV2_LEFT) || (Warning->WarningStat == AMBA_WS_RCTA_STATUS_LV2_RIGHT)) {
            DrawColor = SUR_DRAW_RED;
        }

        /*exclamation mark*/
        RetVal = SvcOsd_DrawLine(1U, ExclamationX, 180U, ExclamationX, 220U, 10U, DrawColor);
        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
        }
        RetVal = SvcOsd_DrawLine(1U, ExclamationX, 227U, ExclamationX, 240, 10U, DrawColor);
        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
        }
        if (Type == 0U) { //3D mode don't draw crow for car point
            /*cross mark for object*/
            DOUBLE TmpDouble;
            UINT32 Mask;
            for(i = 0U; i < AMBA_SR_SCV_MAX_MODEL_VT_NUM; i++) {
                Mask = (UINT32)1U << i;
                if ((Warning->ObjData2D.VerticeStatus & Mask) > 0U) {
                    Id = i;
                    X1 = (UINT32)Warning->ObjData2D.Vertices[Id].X;
                    Y1 = (UINT32)Warning->ObjData2D.Vertices[Id].Y;
                    TmpDouble = (DOUBLE)X1 / RatioX;
                    X2 = (UINT32)TmpDouble + OffsetX;
                    TmpDouble = (DOUBLE)Y1 / RatioY;
                    Y2 = (UINT32)TmpDouble + OffsetY;
                    RetVal = SvcOsd_DrawLine(1U, X2-5U, Y2-5U, X2+5U, Y2+5U, 3U, DrawColor);
                    if (SVC_NG == RetVal) {
                        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                    }
                    RetVal = SvcOsd_DrawLine(1U, X2+5U, Y2-5U, X2-5U, Y2+5U, 3U, DrawColor);
                    if (SVC_NG == RetVal) {
                        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                    }
                }
            }
        }
    }
    return RetVal;
}

static UINT32 AdasDraw2D(void)
{
    UINT32 RetVal = SVC_OK;

    if (g_SurDrawPsdOut.SpaceNum != 0U) {
        UINT32 i, j, k;
        UINT32 X1, X2, Y1, Y2;
        DOUBLE TmpDouble;

        for (i = 0; i < g_SurDrawPsdOut.SpaceNum; i++) {
            UINT32 DrawColor;
            if (g_SurDrawPsdOut.Space[i].Status == AMBA_PSD_SPACE_STATUS_AVAILABLE) {
                DrawColor = SUR_DRAW_GREEN;
                for (j = 0; j < AMBA_PSD_SPACE_LINE_MAX; j++) {
                    if (g_SurDrawPsdOut.Space[i].Line[j].PntNum > 1U) {
                        UINT32 PntMax = g_SurDrawPsdOut.Space[i].Line[j].PntNum - 1U;
                        for (k = 0U; k < PntMax; k++) {
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[k].X/RatioX;
                            X1 = (UINT32)TmpDouble + OffsetX;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[k+1U].X/RatioX;
                            X2 = (UINT32)TmpDouble + OffsetX;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[k].Y/RatioY;
                            Y1 = (UINT32)TmpDouble + OffsetY;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[k+1U].Y/RatioY;
                            Y2 = (UINT32)TmpDouble + OffsetY;
                            RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, 7U, DrawColor);
                            if (SVC_NG == RetVal) {
                                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                            }
                        }
                    }
                }
            } else {
                if (DebugOsdEnable == 1U) {
                    DrawColor = SUR_DRAW_RED;
                    for (j = 0; j < AMBA_PSD_SPACE_LINE_MAX; j++) {
                        if (g_SurDrawPsdOut.Space[i].Line[j].PntNum > 1U) {
                            UINT32 PntMax = g_SurDrawPsdOut.Space[i].Line[j].PntNum - 1U;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[0].X/RatioX;
                            X1 = (UINT32)TmpDouble + OffsetX;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[PntMax].X/RatioX;
                            X2 = (UINT32)TmpDouble + OffsetX;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[0].Y/RatioY;
                            Y1 = (UINT32)TmpDouble + OffsetY;
                            TmpDouble = (DOUBLE)g_SurDrawPsdOut.Space[i].Line[j].Pnt2D[PntMax].Y/RatioY;
                            Y2 = (UINT32)TmpDouble + OffsetY;
                            RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, 7U, DrawColor);
                            if (SVC_NG == RetVal) {
                                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                            }
                        }
                    }
                }
            }

        }
    }

    if (g_SurDrawOwsOut.PathNum != 0U) {
        UINT8 i;//, j;
        UINT32 X1, X2, Y1, Y2;
        UINT32 DrawColor;
        UINT32 LineWidth;
        UINT32 StageBound = g_SurDrawOwsOut.StageNum / 3U;// split into three stage
        DOUBLE TmpDouble;
        for (i = 0; i < g_SurDrawOwsOut.PathNum; i++) {
            if (g_SurDrawOwsOut.ObsStage[i] != 0U) {
                if (g_SurDrawOwsOut.ObsStage[i] <= StageBound) {
                    DrawColor = SUR_DRAW_YELLOW;
                    LineWidth = 4U;
                } else if (g_SurDrawOwsOut.ObsStage[i] <= (StageBound*2U)) {
                    DrawColor = SUR_DRAW_ORANGE;
                    LineWidth = 6U;
                } else {
                    DrawColor = SUR_DRAW_RED;
                    LineWidth = 13U;
                }
                TmpDouble = (DOUBLE)g_SurDrawOwsOut.ObsStageLine[i].Pnt2D[0].X/RatioX;
                X1 = (UINT32)TmpDouble + OffsetX;
                TmpDouble = (DOUBLE)g_SurDrawOwsOut.ObsStageLine[i].Pnt2D[1].X/RatioX;
                X2 = (UINT32)TmpDouble + OffsetX;
                TmpDouble = (DOUBLE)g_SurDrawOwsOut.ObsStageLine[i].Pnt2D[0].Y/RatioY;
                Y1 = (UINT32)TmpDouble + OffsetY;
                TmpDouble = (DOUBLE)g_SurDrawOwsOut.ObsStageLine[i].Pnt2D[1].Y/RatioY;
                Y2 = (UINT32)TmpDouble + OffsetY;
                RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, LineWidth, DrawColor);
                if (SVC_NG == RetVal) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                }
            }
        }
    }
    {
        if (g_SurDrawRctaOut.WarnChan == SUR_RCTA_FRONT) {
            RetVal = RctaDraw(1U, 0U, &g_SurDrawRctaOut.WarnInfoR);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
            RetVal = RctaDraw(0U, 0U, &g_SurDrawRctaOut.WarnInfoL);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
        } else {
            RetVal = RctaDraw(0U, 0U, &g_SurDrawRctaOut.WarnInfoR);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
            RetVal = RctaDraw(1U, 0U, &g_SurDrawRctaOut.WarnInfoL);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
        }

        if (g_SurDrawRctaOut.WarningZone.WZApcPntNumR != 0U) {
            RetVal = RctaDrawWarningZone(g_SurDrawRctaOut.WarningZone.WZApcPntNumR, g_SurDrawRctaOut.WarningZone.WarnZoneApcR, SUR_DRAW_YELLOW);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
        }
        if (g_SurDrawRctaOut.WarningZone.WZApcPntNumL != 0U) {
            RetVal = RctaDrawWarningZone(g_SurDrawRctaOut.WarningZone.WZApcPntNumL, g_SurDrawRctaOut.WarningZone.WarnZoneApcL, SUR_DRAW_YELLOW);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
        }
        if (g_SurDrawRctaOut.WarningZone.WZTtcPntNumR != 0U) {
            RetVal = RctaDrawWarningZone(g_SurDrawRctaOut.WarningZone.WZTtcPntNumR, g_SurDrawRctaOut.WarningZone.WarnZoneTtcR, SUR_DRAW_RED);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
        }
        if (g_SurDrawRctaOut.WarningZone.WZTtcPntNumL != 0U) {
            RetVal = RctaDrawWarningZone(g_SurDrawRctaOut.WarningZone.WZTtcPntNumL, g_SurDrawRctaOut.WarningZone.WarnZoneTtcL, SUR_DRAW_RED);
            if (SVC_NG == RetVal) {
                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
            }
        }
    }
    return RetVal;
}

static void CovertCord3D (UINT8 Type, DOUBLE Ori, UINT32 *Out)
{
    DOUBLE CarSizeRealW = (DOUBLE) CarSize.Width;
    DOUBLE CarSizeRealH = (DOUBLE) CarSize.Height;
    DOUBLE Tmp;

    if (Type == 0U) {
        Tmp = (Ori * (CarSizeVoutW / CarSizeRealW)) + (DisplayW/2.0);
        if (Tmp > (DisplayW - 1.0)) {
            Tmp = (DisplayW - 1.0);
        } else if (Tmp < 0.0) {
            Tmp = 0.0;
        } else {
            /**do nothing*/
        }
        Tmp = Tmp / RatioX ;
        *Out = (UINT32)Tmp + g_Canvas.StartX;
    } else {
        Tmp = (DisplayH / 2.0) - (Ori * (CarSizeVoutH / CarSizeRealH));
        if (Tmp > (DisplayH - 1.0)) {
            Tmp = (DisplayH - 1.0);
        } else if (Tmp < 0.0) {
            Tmp = 0.0;
        } else {
            /**do nothing*/
        }
        Tmp = Tmp / RatioY;
        *Out = (UINT32)Tmp + g_Canvas.StartY;
    }
}

static UINT32 AdasDraw3D(void)
{
    UINT32 RetVal = SVC_OK;

    if (g_SurDrawPsdOut.SpaceNum != 0U) {
        UINT8 i, j;
        UINT32 X1, X2, Y1, Y2;
        for (i = 0; i < g_SurDrawPsdOut.SpaceNum; i++) {
            UINT32 DrawColor;
            if (g_SurDrawPsdOut.Space[i].Status == AMBA_PSD_SPACE_STATUS_AVAILABLE) {
                DrawColor = SUR_DRAW_GREEN;
                for (j = 0U; j < AMBA_PSD_SPACE_LINE_MAX; j++) {
                    if (g_SurDrawPsdOut.Space[i].Line[j].PntNum > 1U) {
                        UINT32 PntMax = g_SurDrawPsdOut.Space[i].Line[j].PntNum - 1U;
                        CovertCord3D(0U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[0].X, &X1);
                        CovertCord3D(0U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[PntMax].X, &X2);
                        CovertCord3D(1U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[0].Y, &Y1);
                        CovertCord3D(1U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[PntMax].Y, &Y2);
                        RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, 7U, DrawColor);
                        if (SVC_NG == RetVal) {
                            SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                        }
                    }
                }
            } else {
                DrawColor = SUR_DRAW_RED;
                if (DebugOsdEnable == 1U) {
                    for (j = 0U; j < AMBA_PSD_SPACE_LINE_MAX; j++) {
                        if (g_SurDrawPsdOut.Space[i].Line[j].PntNum > 1U) {
                            UINT32 PntMax = g_SurDrawPsdOut.Space[i].Line[j].PntNum - 1U;
                            CovertCord3D(0U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[0].X, &X1);
                            CovertCord3D(0U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[PntMax].X, &X2);
                            CovertCord3D(1U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[0].Y, &Y1);
                            CovertCord3D(1U, g_SurDrawPsdOut.Space[i].Line[j].Pnt3D[PntMax].Y, &Y2);
                            RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, 7U, DrawColor);
                            if (SVC_NG == RetVal) {
                                SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                            }
                        }
                    }
                }
            }
        }
    }


    if (g_SurDrawOwsOut.PathNum != 0U) {
        UINT8 i;
        UINT32 X1, X2, Y1, Y2;
        UINT32 DrawColor;
        UINT32 LineWidth;
        UINT32 StageBound = g_SurDrawOwsOut.StageNum / 3U;// split into three stage

        for (i = 0; i < g_SurDrawOwsOut.PathNum; i++) {
            if (g_SurDrawOwsOut.ObsStage[i] != 0U) {
                if (g_SurDrawOwsOut.ObsStage[i] <= StageBound) {
                    DrawColor = SUR_DRAW_YELLOW;
                    LineWidth = 4U;
                } else if (g_SurDrawOwsOut.ObsStage[i] <= (StageBound*2U)) {
                    DrawColor = SUR_DRAW_ORANGE;
                    LineWidth = 6U;
                } else {
                    DrawColor = SUR_DRAW_RED;
                    LineWidth = 13U;
                }
                CovertCord3D(0, g_SurDrawOwsOut.ObsStageLine[i].Pnt3D[0].X, &X1);
                CovertCord3D(0, g_SurDrawOwsOut.ObsStageLine[i].Pnt3D[1].X, &X2);
                CovertCord3D(1, g_SurDrawOwsOut.ObsStageLine[i].Pnt3D[0].Y, &Y1);
                CovertCord3D(1, g_SurDrawOwsOut.ObsStageLine[i].Pnt3D[1].Y, &Y2);
                RetVal = SvcOsd_DrawLine(1U, X1, Y1, X2, Y2, LineWidth, DrawColor);
                if (SVC_NG == RetVal) {
                    SvcLog_NG(SVC_LOG_SUR_TASK, "SvcOsd_DrawLine error !", 0U, 0U);
                }
            }
        }
    }

    RetVal = RctaDraw(1, 1, &g_SurDrawRctaOut.WarnInfoR);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
    }
    RetVal = RctaDraw(0, 1, &g_SurDrawRctaOut.WarnInfoL);
    if (SVC_NG == RetVal) {
        SvcLog_NG(SVC_LOG_SUR_TASK, "SvcSegDrawTask_RctaDraw error !", 0U, 0U);
    }


    return RetVal;
}

static void SvcSurTask_Draw(UINT32 VoutIdx, UINT32 Level)
{
    UINT32 RetVal = SVC_OK;

    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);
    if (SurDisplayMode == SVC_SUR_ADAS_FISHEYE_MODE) {
        RetVal = AdasDraw2D();
        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "AdasDraw2D error !", 0U, 0U);
        }
    } else {
        RetVal = AdasDraw3D();
        if (SVC_NG == RetVal) {
            SvcLog_NG(SVC_LOG_SUR_TASK, "AdasDraw3D error !", 0U, 0U);
        }
    }

}

static void SvcSurTask_Update(UINT32 VoutIdx, UINT32 Level, UINT32 *pUpdate)
{
    AmbaMisra_TouchUnused(&VoutIdx);
    AmbaMisra_TouchUnused(&Level);

    *pUpdate = 1U;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcSurDrawTask_Init
 *
 *  @Description:: Init the Segmentation osd draw module
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcSurDrawTask_Init(void)
{
    AmbaSvcWrap_MisraMemset(&g_SurDrawPsdOut, 0, sizeof(AMBA_PSD_PROC_OUTPUT_DATA_s));
    AmbaSvcWrap_MisraMemset(&g_SurDrawOwsOut, 0, sizeof(AMBA_OWS_RESULT_s));
    AmbaSvcWrap_MisraMemset(&g_SurDrawRctaOut, 0, sizeof(SVC_RCTA_TASK_DRAW_DATA_s));
    AmbaSvcWrap_MisraMemset(&g_Canvas, 0, sizeof(g_Canvas));

    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcSurDrawTask_Start
 *
 *  @Description:: Start the object detection
 *
 *  @Input      ::
 *                 None
 *  @Output     ::
 *                 None
 *  @Return     ::
 *                 SVC_OK (0): No error happens
 *                 SVC_NG (1): Error happens
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcSurDrawTask_Start(void)
{
    UINT32 FovIdx = 0U;
    UINT8 i, k;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();
    const SVC_DISP_STRM_s *pDispStrm = NULL;
    UINT32 MixerWidth, MixerHeight, OsdWidth, OsdHeight;
    UINT32 CvFlowNum = pResCfg->CvFlowNum;
    UINT32 CvFlowBits = pResCfg->CvFlowBits;
    const SVC_CV_FLOW_s *pCvFlow = pResCfg->CvFlow;
    static SVC_CALIB_ADAS_NAND_TABLE_s Cfg;
    SVC_CALIB_ADAS_NAND_TABLE_s *pCfg = &Cfg;
    UINT32 Chan,RetVal;

    Chan = SVC_CALIB_ADAS_TYPE_AVM_F;
    AmbaSvcWrap_MisraMemset(&Cfg, 0, sizeof(SVC_CALIB_ADAS_NAND_TABLE_s));
    RetVal = SvcCalib_AdasCfgGet(Chan, pCfg);
    if (RetVal == SVC_OK) {
        AmbaSvcWrap_MisraMemcpy(&CarSize, &(Cfg.AdasAvm2DCfg.Car), sizeof(AMBA_CAL_SIZE_s));
        if (Cfg.AdasAvm2DCfg.CarVoutOSD.Width != 0U) {
            CarSizeVoutW = (DOUBLE)Cfg.AdasAvm2DCfg.CarVoutOSD.Width;
        }
        if (Cfg.AdasAvm2DCfg.CarVoutOSD.Height != 0U) {
            CarSizeVoutH = (DOUBLE)Cfg.AdasAvm2DCfg.CarVoutOSD.Height;
        }
    } else {
        CarSize.Height = 4434U;
        CarSize.Width = 1810U;
    }

    for (k = 0U; k < CvFlowNum; k++) {
        if ((CvFlowBits & ((UINT32) 1U << k)) >  0U) {
            if ((pCvFlow[k].CvFlowType == SVC_CV_FLOW_REF_SEG) ||
                (pCvFlow[k].CvFlowType == SVC_CV_FLOW_AMBA_SEG)) {
                FovIdx = pCvFlow[k].InputCfg.Input[0].StrmId;
            }
        }
    }


    for (i = 0; i < pResCfg->DispNum; i++) {
        if (pResCfg->DispStrm[i].VoutID == VOUT_IDX_B) {
            pDispStrm = &pResCfg->DispStrm[i];
            (void) SvcOsd_GetOsdBufSize(pResCfg->DispStrm[i].VoutID, &OsdWidth, &OsdHeight);
            MixerWidth  = pDispStrm->StrmCfg.MaxWin.Width;
            MixerHeight = pDispStrm->StrmCfg.MaxWin.Height;

            g_Canvas.StartX = (((MixerWidth - pDispStrm->StrmCfg.Win.Width) >> 1) * OsdWidth) / MixerWidth;
            g_Canvas.StartY = (((MixerHeight - pDispStrm->StrmCfg.Win.Height) >> 1) * OsdHeight) / MixerHeight;
            g_Canvas.Width  = (pDispStrm->StrmCfg.Win.Width * OsdWidth) / MixerWidth;
            g_Canvas.Height = (pDispStrm->StrmCfg.Win.Height * OsdHeight) / MixerHeight;

            if (pDispStrm->StrmCfg.NumChan == 4U) {
                SurDisplayMode = SVC_SUR_ADAS_BIRDVIEW_MODE;
                RatioX = (DOUBLE)pDispStrm->StrmCfg.Win.Width / (DOUBLE)g_Canvas.Width;
                RatioY = (DOUBLE)pDispStrm->StrmCfg.Win.Height / (DOUBLE)g_Canvas.Height;
            } else {
                SurDisplayMode = SVC_SUR_ADAS_FISHEYE_MODE;
                RatioX = (DOUBLE)pResCfg->FovCfg[FovIdx].MainWin.Width / (DOUBLE)g_Canvas.Width;
                RatioY = (DOUBLE)pResCfg->FovCfg[FovIdx].MainWin.Height / (DOUBLE)g_Canvas.Height;
            }
            SvcGui_Register(VOUT_IDX_B, SVC_SUR_ADAS_GUI_LEVEL, "SurAdas", SvcSurTask_Draw, SvcSurTask_Update);
            OffsetX = g_Canvas.StartX;
            OffsetY = g_Canvas.StartY;

            DisplayW = (DOUBLE) pDispStrm->StrmCfg.Win.Width;
            DisplayH = (DOUBLE) pDispStrm->StrmCfg.Win.Height;

        }
    }
    return 0;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcSegDrawTask_PsdMsg
 *
 *  @Description:: Message handler
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/
UINT32 SvcSurDrawTask_DrawMsg(UINT32 MsgCode, const void *pInfo)
{
    UINT32 RetVal = SVC_OK;

    switch (MsgCode) {
    case SVC_PSD_DRAW:
        AmbaSvcWrap_MisraMemcpy(&g_SurDrawPsdOut, pInfo, sizeof(AMBA_PSD_PROC_OUTPUT_DATA_s));
        break;
    case SVC_OWS_DRAW:
        AmbaSvcWrap_MisraMemcpy(&g_SurDrawOwsOut, pInfo, sizeof(AMBA_OWS_RESULT_s));
        break;
    case SVC_RCTA_DRAW:
        AmbaSvcWrap_MisraMemcpy(&g_SurDrawRctaOut, pInfo, sizeof(SVC_RCTA_TASK_DRAW_DATA_s));
        break;
    default:
        RetVal = SVC_NG;
        break;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SvcSegDrawTask_PsdMsg
 *
 *  @Description:: Message handler
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: void
\*-----------------------------------------------------------------------------------------------*/

void SvcSurDrawTask_ClearOsd(void)
{
    AmbaSvcWrap_MisraMemset(&g_SurDrawPsdOut, 0, sizeof(AMBA_PSD_PROC_OUTPUT_DATA_s));
    AmbaSvcWrap_MisraMemset(&g_SurDrawOwsOut, 0, sizeof(AMBA_OWS_RESULT_s));
    AmbaSvcWrap_MisraMemset(&g_SurDrawRctaOut, 0, sizeof(SVC_RCTA_TASK_DRAW_DATA_s));
}

