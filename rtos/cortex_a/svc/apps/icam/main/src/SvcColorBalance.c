/**
 *  @file SvcColorBalance.c
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
 *  @details svc color balance task
 *
 */

#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
// #include "AmbaPrint.h"
#include "AmbaShell.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"

#include "AmbaVIN_Def.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaImg_Proc.h"
#include "AmbaImg_External_CtrlFunc.h"

/* framework */
#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"

/* app shared */
#include "SvcImg.h"
#include "SvcColorBalance.h"
#include "SvcLiveview.h"
#include "SvcTaskList.h"

#include "SvcResCfg.h"
#include "SvcResCfgTask.h"

#define SVC_LOG_COLOR_BALANCE        "COLOR_BAL"

#define COLOR_BALANCE_TASK_STACK_SIZE  (0xA0000)
#define COLOR_BALANCE_MSG_QUE_DEPTH    (32U)

#define COLOR_BALANCE_BACK_VZ   2U /* switch BACK and RIGHT FOV */
#define COLOR_BALANCE_LEFT_VZ   1U
#define COLOR_BALANCE_RIGHT_VZ  0U /* switch BACK and RIGHT FOV */

#define ROI_H_NUMBER            5U
#define ROI_V_NUMBER            5U
#define COLOR_BALANCE_VOUT_ID   0U

#define COLOR_BALANCE_CH_B        0U
#define COLOR_BALANCE_CH_L        1U
#define COLOR_BALANCE_CH_R        2U
#define COLOR_BALANCE_CH_MAX_NUM  3U

typedef enum {
    COLOR_BALANCE_MSG_FEED = 0,
    COLOR_BALANCE_MSG_NUM
} COLOR_BALANCE_MESSAGE_ID;

typedef struct {
    COLOR_BALANCE_MESSAGE_ID MessageID;
    AMBA_DSP_YUV_DATA_RDY_s  *YuvData[COLOR_BALANCE_CH_MAX_NUM];
} COLOR_BALANCE_YUV_MSG_DATA_s;

typedef struct {
    UINT8                         IsInit;

    AMBA_KAL_MSG_QUEUE_t          MsgQue;
    COLOR_BALANCE_YUV_MSG_DATA_s  MsgQueBuf[COLOR_BALANCE_MSG_QUE_DEPTH];
    char                          Name[32];

    SVC_TASK_CTRL_s               TaskCtrl;
    UINT8                         TaskStack[COLOR_BALANCE_TASK_STACK_SIZE];

    UINT8                         ProcessFlag;
} COLOR_BALANCE_CTRL_s;

typedef struct {
    UINT16 StartX;
    UINT16 StartY;
    UINT16 Width;
    UINT16 Height;
} COLOR_BALANCE_ROI_s;

typedef struct {
    UINT32 AvgY;
    UINT32 AvgU;
    UINT32 AvgV;
} YUV_STAT_s;

typedef struct {
    UINT16 ViewZone;

    UINT32 RoiNum;
    AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *PrevChan;
    COLOR_BALANCE_ROI_s PreviewRoi[AMBA_DISP_ROI_MAX];
    COLOR_BALANCE_ROI_s MainRoi[AMBA_DISP_ROI_MAX];

    YUV_STAT_s YuvStat[AMBA_DISP_ROI_MAX];
    AMBA_IK_RGB_TO_YUV_MATRIX_s Y2R;
} COLOR_BALANCE_CH_INFO_s;

static COLOR_BALANCE_CTRL_s ColorBalanceCtrl GNU_SECTION_NOZEROINIT;
static AMBA_DSP_YUV_DATA_RDY_s CollectYuvData[COLOR_BALANCE_MSG_QUE_DEPTH][COLOR_BALANCE_CH_MAX_NUM] GNU_SECTION_NOZEROINIT;
static COLOR_BALANCE_CH_INFO_s ChanCtrl[COLOR_BALANCE_CH_MAX_NUM] GNU_SECTION_NOZEROINIT;

#define MAX_EXP_VAL        20U
#define MAX_MATRIX_ORDER   3U

static UINT32 IsPowerOfTwo(UINT32 val) {
    UINT32 i, s, e, tmp;
    UINT32 RetVal = 0U;
    UINT32 Bit = 1U;

    if (val < ((UINT32)1U << (MAX_EXP_VAL >> 1U))) {
        s = 0;
        e = ((UINT32)MAX_EXP_VAL >> 1U);
    } else {
        s = ((UINT32)MAX_EXP_VAL >> 1U);
        e = (UINT32)MAX_EXP_VAL;
    }

    for (i = s; i <= e; i++) {
        tmp = (Bit << i);
        if (val <= tmp) {
            RetVal = (val < tmp)? 0U : i;
            break;
        }
    }

    if (i == (MAX_EXP_VAL + 1U)) {
        RetVal = 0;
        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "is_power_of_two(): unsupported val %d which is largher than %d", val, (UINT32)1U << (MAX_EXP_VAL - 1U));
    }

    return RetVal;
}

/*For calculating Determinant of the Matrix */
static DOUBLE Determinant(DOUBLE input[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], UINT32 matrixIndex)
{
    DOUBLE Det = 0.0;

    AmbaMisra_TouchUnused(input);

    if (matrixIndex== 3U) {
        DOUBLE x,y,z;
        x = (input[1][1] * input[2][2]) - (input[2][1] * input[1][2]);
        y = (input[1][0] * input[2][2]) - (input[2][0] * input[1][2]);
        z = (input[1][0] * input[2][1]) - (input[2][0] * input[1][1]);

        Det = ((input[0][0] * x) - (input[0][1] * y)) + (input[0][2] * z);
    } else if (matrixIndex == 2U) {
        Det = (input[0][0] * input[1][1]) - (input[0][1] * input[1][0]);
    } else if (matrixIndex == 1U) {
        Det = input[0][0];
    } else {
        Det = 0.0;
    }

    return Det;
}

/*Finding transpose of matrix*/
static void Transpose(DOUBLE num[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], DOUBLE fac[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], DOUBLE *out)
{
    UINT32 i, j, idx;
    DOUBLE Temp[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], d;
    UINT32 Index = MAX_MATRIX_ORDER;

    AmbaMisra_TouchUnused(num);
    AmbaMisra_TouchUnused(fac);

    if (SVC_OK != AmbaWrap_memset(Temp, 0, sizeof(DOUBLE)*MAX_MATRIX_ORDER*MAX_MATRIX_ORDER)) {
        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaWrap_memset Temp failed", 0U, 0U);
    }

    for (i = 0;i < Index; i++) {
        for (j = 0;j < Index; j++) {
            Temp[i][j] = fac[j][i];
        }
    }
    d = Determinant(num, Index);
    for (i = 0;i < Index; i++) {
        for (j = 0;j < Index; j++) {
            idx = (i*3U) + j;
            out[idx] = Temp[i][j] / d;
        }
    }
}

static void Cofactor(DOUBLE num[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], DOUBLE *out)
{
    UINT32 Rval;
    DOUBLE Temp[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], Fac[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER];
    UINT32 p, q, m, n, i, j;
    UINT32 Index = MAX_MATRIX_ORDER;
    DOUBLE PowValue = 0.0, exponent = 0.0, d = 0.0;

    AmbaMisra_TouchUnused(num);

    for (q = 0;q < Index; q++) {
        for (p = 0;p < Index; p++) {
            m = 0;
            n = 0;
            for (i = 0;i < Index; i++) {
                for (j = 0;j < Index; j++) {
                    if ((i != q) && (j != p)) {
                        Temp[m][n] = num[i][j];
                        if (n < (Index - 2U)) {
                            n++;
                        } else {
                            n = 0U;
                            m++;
                        }
                    }
                }
            }
            exponent = ((DOUBLE)q + (DOUBLE)p);
            Rval = AmbaWrap_pow(-1.0, exponent, &PowValue);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaWrap_pow failed", 0U, 0U);
            }
            d = Determinant(Temp, Index - 1U);
            Fac[q][p] = PowValue * d;
        }
    }
    Transpose(num, Fac, out);
}

static UINT32 GetInverseMatrix(const DOUBLE *In, DOUBLE *Out)
{
    UINT32 ReturnValue = SVC_OK;
    DOUBLE Temp[MAX_MATRIX_ORDER][MAX_MATRIX_ORDER], Rval;

    UINT32 MatrixIndex = MAX_MATRIX_ORDER, idx;
    for (UINT32 i = 0;i < MatrixIndex; i++) {
        for (UINT32 j = 0;j < MatrixIndex; j++) {
            idx = (i*3U) + j;
            Temp[i][j] = In[idx];
        }
    }
    Rval = Determinant(Temp, MatrixIndex);
    if (Rval == 0.0) {
        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "Inverse of Entered Matrix is not possible", 0U, 0U);
        ReturnValue = SVC_NG;
    } else {
        Cofactor(Temp, Out);
    }

    return ReturnValue;
}

static UINT32 CalcY2RMatrix(UINT16 ViewID, AMBA_IK_RGB_TO_YUV_MATRIX_s *y2R)
{
    UINT32    ReturnValue, Rval;
    DOUBLE    InMatrix[9], OutMatrix[9];
    AMBA_IK_RGB_TO_YUV_MATRIX_s R2Y;
    AMBA_IK_MODE_CFG_s   Mode = {0};
    DOUBLE    Tmp;

    if (SVC_OK != AmbaWrap_memset(&Mode, 0, sizeof(AMBA_IK_MODE_CFG_s))) {
        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaWrap_memset Mode failed", 0U, 0U);
    }
    Mode.ContextId = ViewID;
    Rval = AmbaIK_GetRgbToYuvMatrix(&Mode, &R2Y);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaIK_GetRgbToYuvMatrix failed", 0U, 0U);
    }

    /* do matrix inverse */
    for (UINT32 i = 0; i < 9U; i++) {
        InMatrix[i] = (DOUBLE)R2Y.MatrixValues[i]/1024.0;
    }

    ReturnValue = GetInverseMatrix(InMatrix, OutMatrix);

    if (ReturnValue == SVC_OK) {
        for(UINT32 i = 0; i < 9U; i++) {
            Tmp = (OutMatrix[i]*1024.0);
#ifdef CONFIG_SOC_H22
            y2R->MatrixValues[i] = (INT16)Tmp;
#else
            y2R->MatrixValues[i] = (INT32)Tmp;
#endif
        }

        y2R->YOffset = R2Y.YOffset;
        y2R->UOffset = R2Y.UOffset;
        y2R->VOffset = R2Y.VOffset;
    }

    return ReturnValue;
}

static UINT32 YuvDatatRdy(const void *pEventData)
{
    UINT32 Rval;
#ifdef CONFIG_SOC_CV2FS
    const AMBA_DSP_PYMD_DATA_RDY_s *pYuvRdy = NULL;
#else
    const AMBA_DSP_YUV_DATA_RDY_s  *pYuvRdy = NULL;
#endif
    UINT32 Slot = 0, Bypass = 0;
    static UINT8 CurCollectYuvPtr = 0;

    if (ColorBalanceCtrl.ProcessFlag == 0U) {
        AmbaMisra_TypeCast(&(pYuvRdy), &(pEventData));

        /* Save yuv info of specific viewzone */
        switch (pYuvRdy->ViewZoneId) {
            case COLOR_BALANCE_BACK_VZ:
                Slot = COLOR_BALANCE_CH_B;
                break;
            case COLOR_BALANCE_LEFT_VZ:
                Slot = COLOR_BALANCE_CH_R;
                break;
            case COLOR_BALANCE_RIGHT_VZ:
                Slot = COLOR_BALANCE_CH_L;
                break;
            default:
                Bypass = 1;
                break;
        }

        if (Bypass == 0U) {
            UINT32 i;
#ifdef CONFIG_SOC_CV2FS
            CollectYuvData[CurCollectYuvPtr][Slot].ViewZoneId = pYuvRdy->ViewZoneId;
            CollectYuvData[CurCollectYuvPtr][Slot].CapPts = pYuvRdy->CapPts;
            CollectYuvData[CurCollectYuvPtr][Slot].YuvPts = pYuvRdy->YuvPts;
            CollectYuvData[CurCollectYuvPtr][Slot].CapSequence = pYuvRdy->CapSequence;
            if (SVC_OK != AmbaWrap_memcpy(&CollectYuvData[CurCollectYuvPtr][Slot].Buffer, pYuvRdy->YuvBuf, sizeof(AMBA_DSP_YUV_IMG_BUF_s))) {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaWrap_memcpy CollectYuvData[CurCollectYuvPtr][Slot].Buffer failed", 0U, 0U);
            }
#else
            if (SVC_OK != AmbaWrap_memcpy(&CollectYuvData[CurCollectYuvPtr][Slot], pYuvRdy, sizeof(AMBA_DSP_YUV_DATA_RDY_s))) {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaWrap_memcpy CollectYuvData[CurCollectYuvPtr][Slot] failed", 0U, 0U);
            }
#endif
            /* Check capture sequence */
            for (i = 1; i < COLOR_BALANCE_CH_MAX_NUM; i++) {
                if (CollectYuvData[CurCollectYuvPtr][i].CapSequence != CollectYuvData[CurCollectYuvPtr][i-1U].CapSequence) {
                    break;
                }
            }

            /* Capture sequence match, send message to color balance task */
            if (i == COLOR_BALANCE_CH_MAX_NUM) {
                COLOR_BALANCE_YUV_MSG_DATA_s Msg;
                Msg.MessageID = COLOR_BALANCE_MSG_FEED;
                for (i = 0; i < COLOR_BALANCE_CH_MAX_NUM; i++) {
                    Msg.YuvData[i] = &CollectYuvData[CurCollectYuvPtr][i];
                }

                Rval = AmbaKAL_MsgQueueSend(&(ColorBalanceCtrl.MsgQue), &Msg, AMBA_KAL_NO_WAIT);
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaKAL_MsgQueueSend MsgQue failed", 0U, 0U);
                }

                CurCollectYuvPtr++;

                if (CurCollectYuvPtr == COLOR_BALANCE_MSG_QUE_DEPTH) {
                    CurCollectYuvPtr = 0;
                }
            }
        }
    }

    return SVC_OK;
}

static UINT32 SetupPreviewRoi(void)
{
    UINT32 Rval = SVC_OK, i, j, Bit = 1U;
    SVC_LIV_INFO_s LivInfo;
    const AMBA_DSP_LIVEVIEW_STREAM_CFG_s    *pStrmCfg;

    /* Get liveview cfg */
    SvcLiveview_InfoGet(&LivInfo);

    /* Get AVM vout id yuv stream cfg */
    for ( i = 0; i < *LivInfo.pNumStrm; i++) {
        /* Check vout purpose stream */
        if ((LivInfo.pStrmCfg[i].Purpose & SVC_LIV_PURPOSE_VOUT) != 0U) {
            /* Check vout id */
            if (LivInfo.pStrmCfg[i].DestVout == (Bit << COLOR_BALANCE_VOUT_ID)) {
                UINT16 ViewZoneId;
                pStrmCfg = &(LivInfo.pStrmCfg[i]);
                for ( j = 0; j < pStrmCfg->NumChan; j++) {
                    ViewZoneId = pStrmCfg->pChanCfg[j].ViewZoneId;
                    switch (ViewZoneId) {
                        case COLOR_BALANCE_BACK_VZ:
                            ChanCtrl[COLOR_BALANCE_CH_B].PrevChan = &(pStrmCfg->pChanCfg[j]);
                            break;
                        case COLOR_BALANCE_LEFT_VZ:
                            ChanCtrl[COLOR_BALANCE_CH_R].PrevChan = &(pStrmCfg->pChanCfg[j]);
                            break;
                        case COLOR_BALANCE_RIGHT_VZ:
                            ChanCtrl[COLOR_BALANCE_CH_L].PrevChan = &(pStrmCfg->pChanCfg[j]);
                            break;
                        default:
                            /* Nothing to do */
                            break;
                    }
                }
            }
        }
    }

    if ((ChanCtrl[COLOR_BALANCE_CH_B].PrevChan == NULL) ||
        (ChanCtrl[COLOR_BALANCE_CH_L].PrevChan == NULL) ||
        (ChanCtrl[COLOR_BALANCE_CH_R].PrevChan == NULL)) {
        Rval = SVC_NG;
    } else {
        UINT32 RoiPointId = 0;
        UINT16 RoiW, RoiH, TileW, TileH;
        UINT16 OffsetX, OffsetY;
        UINT16 x, y;
        UINT16 StartX = 0, StartY = 0;
        const AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *Main = NULL;

        /*
            Area Mapping (on preview)

                                -----------------------------------
                                |         |    f2-c0    |         |
                                |    0    |    Back     |    1    |
                                |         |             |         |
                                -----------------------------------
        -----------------------------------             -----------------------------------
        |         |             |         |             |         |              |        |
        |   f0-c1 |    Left     |    0    |             |    0    |    Right     |  f1-c2 |
        |         |             |         |             |         |              |        |
        -----------------------------------             -----------------------------------

                        -------------------------------------
                        |     |     |     |     |     |     |
                        |  X  |  O  |  X  |  X  |  O  |  X  |
                        -------------------------------------
                        ^^^^^^^^^^^^^^^^^^B^^^^^^^^^^^^^^^^^^
         --------------------------------------------------------------------
         |              |     |     |     |     |     |     |               |
         |              |     |  O  |     |     |  O  |     |               |
         --------------------------------------------------------------------
         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^L R^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        */

        for(i = 0; i < COLOR_BALANCE_CH_MAX_NUM; i++) {

            Main = ChanCtrl[i].PrevChan;
            RoiW = ChanCtrl[0].PrevChan->Window.Width / 6U; // /2 /3
            RoiH = ChanCtrl[0].PrevChan->Window.Height;
            TileW = RoiW / ROI_H_NUMBER;
            TileH = RoiH / ROI_V_NUMBER;
            RoiPointId = 0;

            if (i == 0U) {
                /* back FOV has 2 overlapping area */
                for (j = 0U; j < 2U; j++) {
                    if (j == 0U) {
                        StartX = RoiW;
                        StartY = 0U;
                    } else {
                        StartX = (Main->Window.Width / 2U) + RoiW;
                        StartY = 0U;
                    }

                    OffsetY = 0U;
                    for (y = 0U; y < ROI_V_NUMBER; y++) {
                        OffsetX = 0U;

                        for (x = 0U; x < ROI_H_NUMBER; x++) {
                            ChanCtrl[i].PreviewRoi[RoiPointId].Width  = TileW;
                            ChanCtrl[i].PreviewRoi[RoiPointId].Height = TileH;
                            ChanCtrl[i].PreviewRoi[RoiPointId].StartX = StartX + OffsetX;
                            ChanCtrl[i].PreviewRoi[RoiPointId].StartY = StartY + OffsetY;

                            // AmbaPrint_PrintUInt5("[SetupPreviewRoi]%d (%d, %d) (%d x %d)", i,
                            //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].StartX,
                            //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].StartY,
                            //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].Width,
                            //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].Height);

                            RoiPointId++;
                            OffsetX += TileW;
                        }
                        OffsetY += TileH;
                    }
                }
            } else {
                if (i == 1U) {
                    StartX = (Main->Window.Width - RoiW) - RoiW;
                    StartY = 0U;
                } else {
                    StartX = RoiW;
                    StartY = 0U;
                }

                OffsetY = 0U;
                for (y = 0U; y < ROI_V_NUMBER; y++) {
                    OffsetX = 0U;

                    for (x = 0U; x < ROI_H_NUMBER; x++) {
                        ChanCtrl[i].PreviewRoi[RoiPointId].Width  = TileW;
                        ChanCtrl[i].PreviewRoi[RoiPointId].Height = TileH;
                        ChanCtrl[i].PreviewRoi[RoiPointId].StartX = StartX + OffsetX;
                        ChanCtrl[i].PreviewRoi[RoiPointId].StartY = StartY + OffsetY;

                        // AmbaPrint_PrintUInt5("[SetupPreviewRoi]%d (%d, %d) (%d x %d)", i,
                        //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].StartX,
                        //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].StartY,
                        //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].Width,
                        //                                                     ChanCtrl[i].PreviewRoi[RoiPointId].Height);

                        RoiPointId++;
                        OffsetX += TileW;
                    }
                    OffsetY += TileH;
                }
            }
            ChanCtrl[i].RoiNum = RoiPointId;
        }
    }

    return Rval;
}

static void Preview2Main(void)
{
    UINT32 i, j;
    UINT16 X = 0, Y = 0, W = 0, H = 0;
    UINT16 PrevW, PrevH;
    FLOAT  RatioX, RatioY, Tmp;

    for (i = 0; i < COLOR_BALANCE_CH_MAX_NUM; i++) {

        PrevW = ChanCtrl[i].PrevChan->Window.Width;
        PrevH = ChanCtrl[i].PrevChan->Window.Height;

        /* calculate x/y scale */
        RatioX = (FLOAT)ChanCtrl[i].PrevChan->ROI.Width / (FLOAT)PrevW;
        RatioY = (FLOAT)ChanCtrl[i].PrevChan->ROI.Height / (FLOAT)PrevH;

        for (j = 0; j < ChanCtrl[i].RoiNum; j++) {

            W = ChanCtrl[i].PreviewRoi[j].Width;
            H = ChanCtrl[i].PreviewRoi[j].Height;
            X = ChanCtrl[i].PreviewRoi[j].StartX;
            Y = ChanCtrl[i].PreviewRoi[j].StartY;

            Tmp = (FLOAT)X * RatioX;
            ChanCtrl[i].MainRoi[j].StartX = ChanCtrl[i].PrevChan->ROI.OffsetX + (UINT16)Tmp;
            Tmp = (FLOAT)Y * RatioY;
            ChanCtrl[i].MainRoi[j].StartY = ChanCtrl[i].PrevChan->ROI.OffsetY + (UINT16)Tmp;
            Tmp = (FLOAT)W * RatioX;
            ChanCtrl[i].MainRoi[j].Width  = (UINT16)Tmp;
            Tmp = (FLOAT)H * RatioY;
            ChanCtrl[i].MainRoi[j].Height = (UINT16)Tmp;

            // AmbaPrint_PrintUInt5("(%d) ROI#%d",i,j,0,0,0);
            // AmbaPrint_PrintUInt5("          (%d , %d)   (%d x %d)", ChanCtrl[i].MainRoi[j].StartX,
            //                                                         ChanCtrl[i].MainRoi[j].StartY,
            //                                                         ChanCtrl[i].MainRoi[j].Width,
            //                                                         ChanCtrl[i].MainRoi[j].Height,0);
        }
    }
}

static void CalcDispRoiYuvStat(const AMBA_DSP_YUV_DATA_RDY_s *yuvBufferInfo, UINT32 RoiNum, const COLOR_BALANCE_ROI_s *dispROI, YUV_STAT_s *yuvStat)
{
    UINT32 i, j, k;
    UINT8 *YAddr = NULL;
    UINT8 *UVAddr = NULL;
    UINT32 YPitch, UVPitch;
    UINT32 TotalYPixel = 0, TotalUVPixel = 0, TotalYValue = 0, TotalUValue = 0, TotalVValue = 0;
    UINT32 YOffset = 0, UVOffset = 0, UVWidth = 0, UVHeight = 0;
    UINT32 YRsft =0, UVRsft =0;
    UINT32 idx = 0;
    AmbaMisra_TypeCast(&YAddr,  &yuvBufferInfo->Buffer.BaseAddrY);
    AmbaMisra_TypeCast(&UVAddr, &yuvBufferInfo->Buffer.BaseAddrUV);

    /* CERT STR30-C */
    AmbaMisra_TouchUnused(YAddr);
    AmbaMisra_TouchUnused(UVAddr);

    YPitch = yuvBufferInfo->Buffer.Pitch;
    UVPitch = yuvBufferInfo->Buffer.Pitch;
    for (i = 0; i < RoiNum; i++) {
        /* Y **/
        TotalYPixel = (UINT32) dispROI[i].Width * (UINT32) dispROI[i].Height;
        YRsft = IsPowerOfTwo(TotalYPixel);
        if (TotalYPixel != 0U) {
            TotalYValue = 0U;
            YOffset = (dispROI[i].StartY * YPitch) + dispROI[i].StartX;
            for (j = 0; j < dispROI[i].Height; j++) {
                for (k = 0; k < dispROI[i].Width; k++) {
                    idx = YOffset + (j * YPitch) + k;
                    TotalYValue += YAddr[idx];
                }
            }
            if (YRsft != 0U) {
                yuvStat[i].AvgY = TotalYValue >> YRsft;
            } else {
                yuvStat[i].AvgY = TotalYValue / TotalYPixel;
            }
        } else {
            yuvStat[i].AvgY = 0U;
        }

        /* UV **/
        if (yuvBufferInfo->Buffer.DataFmt == AMBA_DSP_YUV420) {
            /* yuv420*/
            TotalUVPixel = TotalYPixel >> 2U;
            if (YRsft != 0U) {
                UVRsft = YRsft - 2U;
            }
            UVOffset = (((UINT32)dispROI[i].StartY >> 1U) * UVPitch) + dispROI[i].StartX;
            UVWidth  = (UINT32)dispROI[i].Width >> 1U;
            UVHeight = (UINT32)dispROI[i].Height >> 1U;
        } else {
            /* yuv422*/
            TotalUVPixel = TotalYPixel >> 1U;
            if (YRsft != 0U) {
                UVRsft = YRsft - 1U;
            }
            UVOffset = (((UINT32)dispROI[i].StartY) * UVPitch) + dispROI[i].StartX;
            UVWidth  = (UINT32)dispROI[i].Width >> 1U;
            UVHeight = (UINT32)dispROI[i].Height;

        }
        if (TotalUVPixel != 0U) {
            TotalUValue = 0U;
            TotalVValue = 0U;
            for (j = 0; j < UVHeight; j++) {
                for (k = 0; k < UVWidth; k++) {
                    idx = UVOffset + (j * UVPitch) + (k<<1);
                    TotalUValue += UVAddr[idx];

                    idx = UVOffset + (j * UVPitch) + (k<<1) + 1U;
                    TotalVValue += UVAddr[idx];
                }
            }

            if (UVRsft != 0U) {
                yuvStat[i].AvgU = TotalUValue >> UVRsft;
                yuvStat[i].AvgV = TotalVValue >> UVRsft;
            } else {
                yuvStat[i].AvgU = TotalUValue / TotalUVPixel;
                yuvStat[i].AvgV = TotalVValue / TotalUVPixel;
            }
        } else {
                yuvStat[i].AvgU = 0;
                yuvStat[i].AvgV = 0;
        }
        if ((dispROI[i].StartX % 2U) != 0U) {
            UINT32 Tmp;
            Tmp = yuvStat[i].AvgU;
            yuvStat[i].AvgU = yuvStat[i].AvgV;
            yuvStat[i].AvgV = Tmp;
        }
        // AmbaPrint_PrintUInt5("Avg %2d Y/U/V %3d %3d %3d", i, yuvStat[i].AvgY, yuvStat[i].AvgU, yuvStat[i].AvgV,0);
    }
}

static UINT32 TransferYuvStat2RgbStat(const AMBA_IK_RGB_TO_YUV_MATRIX_s *y2R, UINT32 RoiNum, const YUV_STAT_s *yuvStat, AMBA_DISP_ROI_RGB_Avg_s *rgbStat)
{
    UINT32 i;
    INT32 TempY = 0, TempU = 0, TempV = 0;
    INT32 TempS32;
    UINT32 TempR = 0, TempG = 0, TempB = 0;

    for (i = 0; i < RoiNum; i++) {
        if ((yuvStat[i].AvgY != 0U) || (yuvStat[i].AvgU != 0U) || (yuvStat[i].AvgV != 0U)) {
            TempY = (INT32)yuvStat[i].AvgY - y2R->YOffset;
            TempU = (INT32)yuvStat[i].AvgU - y2R->UOffset;
            TempV = (INT32)yuvStat[i].AvgV - y2R->VOffset;

            TempS32 = ((TempY * y2R->MatrixValues[0]) +
                       (TempU * y2R->MatrixValues[1]) +
                       (TempV * y2R->MatrixValues[2]));
            TempR   = (UINT32)TempS32 >> 10;

            TempS32 = ((TempY * y2R->MatrixValues[3]) +
                       (TempU * y2R->MatrixValues[4]) +
                       (TempV * y2R->MatrixValues[5]));
            TempG   = (UINT32)TempS32 >> 10U;

            TempS32 = ((TempY * y2R->MatrixValues[6]) +
                       (TempU * y2R->MatrixValues[7]) +
                       (TempV * y2R->MatrixValues[8]));
            TempB   = (UINT32)TempS32 >> 10U;

            if (TempR > 255U) {
                rgbStat[i].AvgR = 255U;
            } else {
                rgbStat[i].AvgR = TempR;
            }

            if (TempG > 255U) {
                rgbStat[i].AvgG = 255U;
            } else {
                rgbStat[i].AvgG = TempG;
            }

            if (TempB > 255U) {
                rgbStat[i].AvgB = 255U;
            } else {
                rgbStat[i].AvgB = TempB;
            }

        } else {
            rgbStat[i].AvgR = 0U;
            rgbStat[i].AvgG = 0U;
            rgbStat[i].AvgB = 0U;
        }
        // AmbaPrint_PrintUInt5("Avg %2d R/G/B %3d %3d %3d", i, rgbStat[i].AvgR, rgbStat[i].AvgG, rgbStat[i].AvgB, 0);
    }
    return OK;
}

static void SetupParam(void)
{
    ChanCtrl[COLOR_BALANCE_CH_B].ViewZone = COLOR_BALANCE_BACK_VZ;
    ChanCtrl[COLOR_BALANCE_CH_R].ViewZone = COLOR_BALANCE_LEFT_VZ;
    ChanCtrl[COLOR_BALANCE_CH_L].ViewZone = COLOR_BALANCE_RIGHT_VZ;

    ChanCtrl[COLOR_BALANCE_CH_B].PrevChan = NULL;
    ChanCtrl[COLOR_BALANCE_CH_L].PrevChan = NULL;
    ChanCtrl[COLOR_BALANCE_CH_R].PrevChan = NULL;
}

static void* SvcColorBalance_TaskEntry(void* EntryArg)
{
    UINT32 i, Rval = SVC_OK;
    COLOR_BALANCE_YUV_MSG_DATA_s Msg = {0};
    UINT8 LoopExit = 0U;
    AMBA_DISP_ROI_RGB_STAT_s RoiRgbStat[COLOR_BALANCE_CH_MAX_NUM] = {0};

    AmbaMisra_TouchUnused(EntryArg);

    /* Setup global parameter */
    Rval |= AmbaWrap_memset(&CollectYuvData[0], 0, sizeof(AMBA_DSP_YUV_DATA_RDY_s)*COLOR_BALANCE_CH_MAX_NUM);
    Rval |= AmbaWrap_memset(&ChanCtrl, 0, sizeof(COLOR_BALANCE_CH_INFO_s)*COLOR_BALANCE_CH_MAX_NUM);
    Rval |= AmbaWrap_memset(&RoiRgbStat, 0, sizeof(AMBA_DISP_ROI_RGB_STAT_s)*COLOR_BALANCE_CH_MAX_NUM);
    if (SVC_OK != Rval) {
        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaWrap_memset CollectYuvData[0]/ChanCtrl/RoiRgbStat failed", 0U, 0U);
    }

    /* Setup parameters */
    SetupParam();

    RoiRgbStat[COLOR_BALANCE_CH_B].Num = 75;
    RoiRgbStat[COLOR_BALANCE_CH_L].Num = 75;
    RoiRgbStat[COLOR_BALANCE_CH_R].Num = 75;

    /* Setup ROI position on preview */
    Rval = SetupPreviewRoi();

    if (Rval == SVC_OK) {
        /* Transfer ROI position from preview to main YUV buffer */
        Preview2Main();

        /* Calculate Y2R matrix */
        for( i = 0; i < COLOR_BALANCE_CH_MAX_NUM; i++) {
            Rval = CalcY2RMatrix(ChanCtrl[i].ViewZone, &ChanCtrl[i].Y2R);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "CalcY2RMatrix failed", 0U, 0U);
            }
        }
    } else {
        LoopExit = 1U;
    }

    /* Color balance task proccess */
    if (LoopExit == 0U) {
        ULONG ArgVal = 0U;
        AmbaSvcWrap_MisraMemcpy(&ArgVal, EntryArg, sizeof(ULONG));

        while ( ArgVal != 0xCafeU ) {
            /* Wait message queue */
            Rval = AmbaKAL_MsgQueueReceive(&(ColorBalanceCtrl.MsgQue), &Msg, AMBA_KAL_WAIT_FOREVER);
            if (SVC_OK != Rval) {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaKAL_MsgQueueReceive MsgQue failed", 0U, 0U);
            }

            if (Msg.MessageID == COLOR_BALANCE_MSG_FEED) {
                ColorBalanceCtrl.ProcessFlag = 1;

                for( i = 0; i < COLOR_BALANCE_CH_MAX_NUM; i++) {
                    /* Calculate YUV statistic */
                    CalcDispRoiYuvStat(Msg.YuvData[i], ChanCtrl[i].RoiNum, &ChanCtrl[i].MainRoi[0], &ChanCtrl[i].YuvStat[0]);

                    /* Transfer from YUV to RGB */
                    Rval = TransferYuvStat2RgbStat(&ChanCtrl[i].Y2R, ChanCtrl[i].RoiNum, &ChanCtrl[i].YuvStat[0], &RoiRgbStat[i].ROI[0]);
                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "TransferYuvStat2RgbStat failed", 0U, 0U);
                    }

                    // AmbaPrint_PrintUInt5("[%d]   R G B = %d , %d , %d", i, RoiRgbStat[i].ROI[0].AvgR, RoiRgbStat[i].ROI[0].AvgG, RoiRgbStat[i].ROI[0].AvgB, 0);
                }

                /* Send result to image framwork */
                SvcImg_AvmRoiRgbPut(RoiRgbStat);

                ColorBalanceCtrl.ProcessFlag = 0;
            }

            AmbaMisra_TouchUnused(&ArgVal);
        }
    }

    return NULL;
}

/**
 * Initial color balance task
 * return None
 */
void SvcColorBalance_Init(void)
{
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    /* Set eMirror mode*/
    if ((pResCfg->UserFlag & SVC_EMR_STITCH) > 0U) {
        SvcImg_AvmAlgoMode((UINT32)(1UL << 16U) | (2U));
        ColorBalanceCtrl.IsInit = 0U;
        ColorBalanceCtrl.ProcessFlag = 0U;
    }
}

/**
 * Start color balance task
 * @return 0-OK, 1-NG
 */
UINT32 SvcColorBalance_Start(void)
{
    UINT32 Rval = SVC_OK, AlgoId;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (SvcImg_AvmAlgoIdGet(&AlgoId) == SVC_OK) {
        if ((pResCfg->UserFlag & SVC_EMR_STITCH) > 0U) {
            if (ColorBalanceCtrl.IsInit == 0U) {
                /* Create msg queue */
                Rval = AmbaKAL_MsgQueueCreate(&(ColorBalanceCtrl.MsgQue), ColorBalanceCtrl.Name, (UINT32)sizeof(ColorBalanceCtrl.MsgQueBuf[0]), ColorBalanceCtrl.MsgQueBuf, (UINT32)sizeof(ColorBalanceCtrl.MsgQueBuf));

                /* Create color balance task */
                if (SVC_OK == Rval) {
                    ColorBalanceCtrl.TaskCtrl.Priority   = SVC_COLOR_BALANCE_TASK_PRI;
                    ColorBalanceCtrl.TaskCtrl.EntryFunc  = SvcColorBalance_TaskEntry;
                    ColorBalanceCtrl.TaskCtrl.EntryArg   = 0;
                    ColorBalanceCtrl.TaskCtrl.pStackBase = ColorBalanceCtrl.TaskStack;
                    ColorBalanceCtrl.TaskCtrl.StackSize  = COLOR_BALANCE_TASK_STACK_SIZE;
                    ColorBalanceCtrl.TaskCtrl.CpuBits    = SVC_COLOR_BALANCE_TASK_CPU_BITS;

                    Rval = SvcTask_Create("SvcColorBalance", &ColorBalanceCtrl.TaskCtrl);

                    if (SVC_OK != Rval) {
                        SvcLog_NG(SVC_LOG_COLOR_BALANCE, "SvcTask_Create failed", 0U, 0U);
                    } else {
                        ColorBalanceCtrl.IsInit = 1U;
                    }
                }
            }

            if (ColorBalanceCtrl.IsInit == 1U) {
#ifdef CONFIG_SOC_CV2FS
                /* Register DSP event to listen pyramid YUV data */
                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, YuvDatatRdy);
#else
                /* Register DSP event to listen main YUV data */
                Rval = AmbaDSP_EventHandlerRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, YuvDatatRdy);
#endif
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaDSP_EventHandlerRegister AMBA_DSP_EVENT_LV_YUV_DATA_RDY failed", 0U, 0U);
                }
            } else {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "ColorBalanceCtrl has already been initialized.", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }

    return Rval;
}

/**
 * Stop color balance task
 * @return 0-OK, 1-NG
 */
UINT32 SvcColorBalance_Stop(void)
{
    UINT32 Rval = SVC_OK, AlgoId;
    const SVC_RES_CFG_s *pResCfg = SvcResCfg_Get();

    if (SvcImg_AvmAlgoIdGet(&AlgoId) == SVC_OK) {
        if ((pResCfg->UserFlag & SVC_EMR_STITCH) > 0U) {
            if (ColorBalanceCtrl.IsInit == 1U) {
                Rval = SvcTask_Destroy(&ColorBalanceCtrl.TaskCtrl);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_COLOR_BALANCE, "SvcTask_Destroy failed.", 0U, 0U);
                }

                Rval = AmbaKAL_MsgQueueDelete(&ColorBalanceCtrl.MsgQue);
                if (Rval != SVC_OK) {
                    SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaKAL_MsgQueueDelete failed.", 0U, 0U);
                }

#ifdef CONFIG_SOC_CV2FS
                /* Unregister DSP event to listen pyramid YUV data */
                Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_PYRAMID_RDY, YuvDatatRdy);
#else
                /* Unregister DSP event to listen main YUV data */
                Rval = AmbaDSP_EventHandlerUnRegister(AMBA_DSP_EVENT_LV_YUV_DATA_RDY, YuvDatatRdy);
#endif
                if (SVC_OK != Rval) {
                    SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaDSP_EventHandlerUnRegister AMBA_DSP_EVENT_LV_YUV_DATA_RDY failed", 0U, 0U);
                } else {
                    UINT32 i;
                    DOUBLE AEDgain = 1.0;
                    DOUBLE BLDgainY = 1.0;
                    AMBA_MVIN_BL_RGB_GAIN_s BLDgainRGB;

                    BLDgainRGB.GainB = 1.0;
                    BLDgainRGB.GainG = 1.0;
                    BLDgainRGB.GainR = 1.0;

                    SvcImg_AvmRoiRgbPut(NULL);

                    for (i = 0U; i < 3U; i++) {
                        Rval |= AmbaImgProc_MVINSetAEDgain(i, &AEDgain, 1);
                        Rval |= AmbaImgProc_MVINSetBLDgainY(i, &BLDgainY, 1);
                        Rval |= AmbaImgProc_MVINSetBLDgainRGB(i, &BLDgainRGB);
                        if (SVC_OK != Rval) {
                            SvcLog_NG(SVC_LOG_COLOR_BALANCE, "AmbaImgProc_MVINSetAEDgain/AmbaImgProc_MVINSetBLDgainY/AmbaImgProc_MVINSetBLDgainRGB failed", 0U, 0U);
                        }
                    }
                }
                ColorBalanceCtrl.IsInit = 0U;
            } else {
                SvcLog_NG(SVC_LOG_COLOR_BALANCE, "ColorBalanceCtrl was not initialized.", 0U, 0U);
                Rval = SVC_NG;
            }
        }
    }

    return Rval;
}
