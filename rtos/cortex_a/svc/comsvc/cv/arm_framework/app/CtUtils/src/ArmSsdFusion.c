/**
*  @file ArmSsdFusion.c
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
*   @details The SSD fusion functions
*
*/

#include "ArmLog.h"
#include "ArmErrCode.h"
#include "ArmStdC.h"
#include "ArmSsdFusion.h"

#define ARM_LOG_SSD_FUSION  "ArmUtil_SsdFusion"

#define EXP                 (2.71828182845904)

#define MAX_PRIORBOX_NUM    (110000U)
#define MAX_TOPK_NUM        (400U)

static FLOAT G_DecodeBox[MAX_PRIORBOX_NUM * 4U] = { 0.0f };   // (xmin, ymin, xmax, ymax)
static NMS_BOX_s G_NmsBox[MAX_TOPK_NUM]    = {0};
static UINT32 G_NmsBoxNum = 0U;

#define MAX_VIS_NUM         (200U)
static VIS_BOX_s G_VisBox[MAX_VIS_NUM]         = {0};
static UINT32 G_VisBoxNum = 0U;

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MIN
 *
 *  @Description:: Return minimum value between input a and b
 *
 *  @Input      ::
 *     a:          The input a
 *     b:          The input b
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    FLOAT:       The minimum value between a and b
\*-----------------------------------------------------------------------------------------------*/
static inline FLOAT MIN(FLOAT a, FLOAT b)
{
    FLOAT tmp;

    if (a < b) {
        tmp = a;
    } else {
        tmp = b;
    }

    return tmp;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MAX
 *
 *  @Description:: Return maximum value between input a and b
 *
 *  @Input      ::
 *     a:          The input a
 *     b:          The input b
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    FLOAT:       The maximum value between a and b
\*-----------------------------------------------------------------------------------------------*/
static inline FLOAT MAX(FLOAT a, FLOAT b)
{
    FLOAT tmp;

    if (a > b) {
        tmp = a;
    } else {
        tmp = b;
    }

    return tmp;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: clean_decodebox
 *
 *  @Description:: Clean G_DecodeBox
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void clean_decodebox(void)
{
    (void) ArmStdC_memset(G_DecodeBox, 0x0, sizeof(G_DecodeBox));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: clean_nmsbox
 *
 *  @Description:: Clean G_NmsBox and reset G_NmsBoxNum
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void clean_nmsbox(void)
{
    G_NmsBoxNum = 0;
    (void) ArmStdC_memset(G_NmsBox, 0x0, sizeof(G_NmsBox));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: clean_visbox
 *
 *  @Description:: Clean G_VisBox and reset G_VisBoxNum
 *
 *  @Input      :: None
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void clean_visbox(void)
{
    G_VisBoxNum = 0;
    (void) ArmStdC_memset(G_VisBox, 0x0, sizeof(G_VisBox));
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: decode_bbox
 *
 *  @Description:: Decode bounding box by given index
 *
 *  @Input      ::
 *    PriorBoxIdx: The index of decoded box
 *    PriorBoxNum: The number of prior box
 *    pPriorBox:   Pointer to prior box
 *    pLoc:        Pointer to location
 *    ModelType:   The type of the model (caffe or tensorflow)
 *
 *  @Output     ::
 *    pDecodeBox:  Pointer to decode box
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void decode_bbox(UINT32 PriorBoxIdx, UINT32 PriorBoxNum, const FLOAT *pPriorBox, const FLOAT *pLoc, FLOAT *pDecodeBox, UINT32 ModelType)
{
    UINT32 ix4, VarOffset;
    FLOAT  Var[4];
    FLOAT  PriorCenterX, PriorCenterY, PriorWidth, PriorHeight;
    FLOAT  Loc[4];
    FLOAT  DecCenterX, DecCenterY, DecWidth, DecHeight;
    DOUBLE PowResult;

    VarOffset = PriorBoxNum * 4U;
    ix4 = PriorBoxIdx * 4U;

    if (pDecodeBox[ix4 + 2U] == 0.0f) { // if xmax is not 0, it means we've already decoded it.

        /* variance */
        Var[0] = pPriorBox[VarOffset + ix4];
        Var[1] = pPriorBox[VarOffset + ix4 + 1U];
        Var[2] = pPriorBox[VarOffset + ix4 + 2U];
        Var[3] = pPriorBox[VarOffset + ix4 + 3U];

        /* prior box */
        if (ModelType == MODEL_TYPE_CAFFE) {
            /* Caffe - the order of pPriorBox is xmin, ymin, xmax, ymax */
            PriorCenterX = (pPriorBox[ix4] + pPriorBox[ix4 + 2U]) / 2.0f;       // x_center = (xmin + xmax)/2
            PriorCenterY = (pPriorBox[ix4 + 1U] + pPriorBox[ix4 + 3U]) / 2.0f;  // y_center = (ymin + ymax)/2
            PriorWidth   = pPriorBox[ix4 + 2U] - pPriorBox[ix4];                 // w = xmax - xmin
            PriorHeight  = pPriorBox[ix4 + 3U] - pPriorBox[ix4 + 1U];            // h = ymax - ymin
        } else {
            /* Tensorflow - the order of pPriorBox is ymin, xmin, ymax, xmax */
            PriorCenterX = (pPriorBox[ix4 + 1U] + pPriorBox[ix4 + 3U]) / 2.0f;  // x_center = (xmin + xmax)/2
            PriorCenterY = (pPriorBox[ix4] + pPriorBox[ix4 + 2U]) / 2.0f;       // y_center = (ymin + ymax)/2
            PriorWidth   = pPriorBox[ix4 + 3U] - pPriorBox[ix4 + 1U];            // w = xmax - xmin
            PriorHeight  = pPriorBox[ix4 + 2U] - pPriorBox[ix4];                 // h = ymax - ymin
        }

        /* detect location */
        if (ModelType == MODEL_TYPE_CAFFE) {
            /* Caffe - the order of pLoc is x, y, w, h */
            Loc[0] = pLoc[ix4];       //x
            Loc[1] = pLoc[ix4 + 1U];  //y
            Loc[2] = pLoc[ix4 + 2U];  //w
            Loc[3] = pLoc[ix4 + 3U];  //h
        } else {
            /* Tensorflow - the order of pLoc is y, x, h, w */
            Loc[0] = pLoc[ix4 + 1U];  //x
            Loc[1] = pLoc[ix4];       //y
            Loc[2] = pLoc[ix4 + 3U];  //w
            Loc[3] = pLoc[ix4 + 2U];  //h
        }

        /* decode box */
        DecCenterX = (Loc[0] * Var[0] * PriorWidth) + PriorCenterX;
        DecCenterY = (Loc[1] * Var[1] * PriorHeight) + PriorCenterY;
        PowResult = ArmStdC_pow(EXP, (DOUBLE)Loc[2] * (DOUBLE)Var[2]);
        DecWidth   = PriorWidth * (FLOAT) PowResult;
        PowResult = ArmStdC_pow(EXP, (DOUBLE)Loc[3] * (DOUBLE)Var[3]);
        DecHeight  = PriorHeight * (FLOAT) PowResult;

        pDecodeBox[ix4]         = DecCenterX - (DecWidth  / 2.0f);  // xmin
        pDecodeBox[ix4 + 1U]    = DecCenterY - (DecHeight / 2.0f);  // ymin
        pDecodeBox[ix4 + 2U]    = DecCenterX + (DecWidth  / 2.0f);  // xmax
        pDecodeBox[ix4 + 3U]    = DecCenterY + (DecHeight / 2.0f);  // ymax

        /* The DecodeBox is between 0~1 */
        pDecodeBox[ix4]         = (pDecodeBox[ix4] < 0.0f) ? 0.0f : pDecodeBox[ix4];
        pDecodeBox[ix4 + 1U]    = (pDecodeBox[ix4 + 1U] < 0.0f) ? 0.0f : pDecodeBox[ix4 + 1U];
        pDecodeBox[ix4 + 2U]    = (pDecodeBox[ix4 + 2U] < 0.0f) ? 0.0f : pDecodeBox[ix4 + 2U];
        pDecodeBox[ix4 + 3U]    = (pDecodeBox[ix4 + 3U] < 0.0f) ? 0.0f : pDecodeBox[ix4 + 3U];

        pDecodeBox[ix4]         = (pDecodeBox[ix4] > 1.0f) ? 1.0f : pDecodeBox[ix4];
        pDecodeBox[ix4 + 1U]    = (pDecodeBox[ix4 + 1U] > 1.0f) ? 1.0f : pDecodeBox[ix4 + 1U];
        pDecodeBox[ix4 + 2U]    = (pDecodeBox[ix4 + 2U] > 1.0f) ? 1.0f : pDecodeBox[ix4 + 2U];
        pDecodeBox[ix4 + 3U]    = (pDecodeBox[ix4 + 3U] > 1.0f) ? 1.0f : pDecodeBox[ix4 + 3U];

    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: get_box_size
 *
 *  @Description:: Get box size by given Xmin, Ymin, Xmax and Ymax
 *                 size = (Xmax - Xmin) * (Ymax - Ymin)
 *
 *  @Input      ::
 *    Xmin:        The Xmin coordinate
 *    Ymin:        The Ymin coordinate
 *    Xmax:        The Xmax coordinate
 *    Ymax:        The Ymax coordinate
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    FLOAT:       (Xmax - Xmin) * (Ymax - Ymin)
\*-----------------------------------------------------------------------------------------------*/
static FLOAT get_box_size(FLOAT Xmin, FLOAT Ymin, FLOAT Xmax, FLOAT Ymax)
{
    FLOAT size = 0.0f;

    if ((Xmax < Xmin) || (Ymax < Ymin)) {
        size = 0.0f;
    } else {
        size = (Xmax - Xmin) * (Ymax - Ymin);
    }
    return size;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: jaccard_overlap
 *
 *  @Description:: Get the overlap percentage
 *
 *  @Input      ::
 *    X1min:       The X1min coordinate
 *    Y1min:       The Y1min coordinate
 *    X1max:       The X1max coordinate
 *    Y1max:       The Y1max coordinate
 *    X2min:       The X2min coordinate
 *    Y2min:       The Y2min coordinate
 *    X2max:       The X2max coordinate
 *    Y2max:       The Y2max coordinate
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    FLOAT:       InterSize / (Bbox1Size + Bbox2Size - InterSize)
\*-----------------------------------------------------------------------------------------------*/
static FLOAT jaccard_overlap(FLOAT X1min, FLOAT Y1min, FLOAT X1max, FLOAT Y1max,
                             FLOAT X2min, FLOAT Y2min, FLOAT X2max, FLOAT Y2max)
{
    FLOAT InterXmin = MAX(X1min, X2min);
    FLOAT InterYmin = MAX(Y1min, Y2min);
    FLOAT InterXmax = MIN(X1max, X2max);
    FLOAT InterYmax = MIN(Y1max, Y2max);

    FLOAT InterWidth = InterXmax - InterXmin;
    FLOAT InterHeight = InterYmax - InterYmin;
    FLOAT InterSize = InterWidth * InterHeight;

    FLOAT Bbox1Size = get_box_size(X1min, Y1min, X1max, Y1max);
    FLOAT Bbox2Size = get_box_size(X2min, Y2min, X2max, Y2max);

    return InterSize / (Bbox1Size + Bbox2Size - InterSize);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: apply_nms
 *
 *  @Description:: Apply NMS for each class
 *
 *  @Input      ::
 *    TargetClass: The target class
 *    pDecodeBox:  Pointer to decode box
 *    pPriorBox:   Pointer to prior box
 *    pLoc:        Pointer to location
 *    pConf:       Pointer to confidence
 *    ConfThrd:    The confidence threshold
 *    NmsThrd:     The NMS overlap threshold
 *    TopK:        The TopK for each class
 *    TopK:        The TopK for NMS box
 *    PriorBoxNum: The number of prior box
 *    ClassNum:    The total number of class
 *    ModelType:   The type of the model (caffe or tensorflow)
 *
 *  @Output     ::
 *    G_NmsBox:    Put the result into global variable G_NmsBox
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void apply_nms(UINT32 TargetClass, FLOAT *pDecodeBox, const FLOAT *pPriorBox, const FLOAT *pLoc, const FLOAT *pConf,
                      FLOAT ConfThrd, FLOAT NmsThrd, UINT32 TopK, UINT32 NmsTopK, UINT32 PriorBoxNum, UINT32 ClassNum, UINT32 ModelType)
{
    static SORT_BOX_s G_SortBox[MAX_TOPK_NUM] = {0};

    SORT_BOX_s SwapTmp = {0};
    UINT32 SortIdx, DecIdx, MinConfIdx;
    UINT32 i, j, Keep;
    FLOAT Overlap, MinConf;

    /* copy valid data to G_SortBox */
    SortIdx = 0U;
    (void) ArmStdC_memset(G_SortBox, 0x0, sizeof(G_SortBox));
    for (i = 0U; i < PriorBoxNum; i++) {
        if (pConf[(i * ClassNum) + TargetClass] >= ConfThrd) {

            if (SortIdx < TopK) {
                G_SortBox[SortIdx].Score = pConf[(i * ClassNum) + TargetClass];   // conf
                G_SortBox[SortIdx].DecBoxIdx = i;   // PriorBox_index
                SortIdx++;

            } else {
                // find minimum conf and replace it
                MinConf = pConf[(i * ClassNum) + TargetClass];
                MinConfIdx = TopK;
                for (j = 0; j < TopK; j ++ ) {
                    if (G_SortBox[j].Score < MinConf) {
                        MinConf = G_SortBox[j].Score;
                        MinConfIdx = j;
                    }
                }
                if (MinConfIdx != TopK) { // find
                    G_SortBox[MinConfIdx].Score = pConf[(i * ClassNum) + TargetClass];
                    G_SortBox[MinConfIdx].DecBoxIdx = i;
                }
            }
        }
    }

    /* bubble sort */
    for (i = 0U; i < SortIdx; i++) {
        for (j = 1U; j < (SortIdx - i); j++) {
            if (G_SortBox[j].Score > G_SortBox[j - 1U].Score) {
                // swap j and j-1
                (void) ArmStdC_memcpy(&SwapTmp, &G_SortBox[j], sizeof(SORT_BOX_s));
                (void) ArmStdC_memcpy(&G_SortBox[j], &G_SortBox[j - 1U], sizeof(SORT_BOX_s));
                (void) ArmStdC_memcpy(&G_SortBox[j - 1U], &SwapTmp, sizeof(SORT_BOX_s));
            }
        }
    }

    /* nms */
    for (i = 0U; i < SortIdx; i++) {
        // decode bbox before doing nms
        decode_bbox(G_SortBox[i].DecBoxIdx, PriorBoxNum, pPriorBox, pLoc, pDecodeBox, ModelType);
        Keep = 1U;
        DecIdx = G_SortBox[i].DecBoxIdx * 4U;
        for (j = 0; j < G_NmsBoxNum; j++) {
            if (G_NmsBox[j].Class == TargetClass) {
                if (Keep == 1U) {
                    Overlap = jaccard_overlap(G_NmsBox[j].Xmin, G_NmsBox[j].Ymin, G_NmsBox[j].Xmax, G_NmsBox[j].Ymax,
                                              pDecodeBox[DecIdx], pDecodeBox[DecIdx + 1U], pDecodeBox[DecIdx + 2U], pDecodeBox[DecIdx + 3U]);
                    if (Overlap <= NmsThrd) {
                        Keep = 1U;
                    } else {
                        Keep = 0U;
                    }
                } else {
                    break;
                }
            }
        }
        if (Keep == 1U) {
            if (G_NmsBoxNum < NmsTopK) {
                G_NmsBox[G_NmsBoxNum].Class = TargetClass;
                G_NmsBox[G_NmsBoxNum].Score = G_SortBox[i].Score;
                G_NmsBox[G_NmsBoxNum].Xmin = pDecodeBox[DecIdx];
                G_NmsBox[G_NmsBoxNum].Ymin = pDecodeBox[DecIdx + 1U];
                G_NmsBox[G_NmsBoxNum].Xmax = pDecodeBox[DecIdx + 2U];
                G_NmsBox[G_NmsBoxNum].Ymax = pDecodeBox[DecIdx + 3U];
                G_NmsBoxNum ++;

            } else {
                // find minimum Score and replace it
                MinConf = G_SortBox[i].Score;
                MinConfIdx = NmsTopK;
                for (j = 0; j < NmsTopK; j ++ ) {
                    if (G_NmsBox[j].Score < MinConf) {
                        MinConf = G_NmsBox[j].Score;
                        MinConfIdx = j;
                    }
                }
                if (MinConfIdx != NmsTopK) {  // find
                    G_NmsBox[MinConfIdx].Class = TargetClass;
                    G_NmsBox[MinConfIdx].Score = G_SortBox[i].Score;
                    G_NmsBox[MinConfIdx].Xmin = pDecodeBox[DecIdx];
                    G_NmsBox[MinConfIdx].Ymin = pDecodeBox[DecIdx + 1U];
                    G_NmsBox[MinConfIdx].Xmax = pDecodeBox[DecIdx + 2U];
                    G_NmsBox[MinConfIdx].Ymax = pDecodeBox[DecIdx + 3U];
                }

            }
        }
    }
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmSsdFusion_NmsCalc
 *
 *  @Description:: Do SSD NMS calculation
 *
 *  @Input      ::
 *    NmsInput:    3 pointers to prior box, location and confidence
 *    NmsCfg:      The NMS config
 *
 *  @Output     ::
 *   pNmsOutput:   The bounding box after NMS
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmSsdFusion_NmsCalc(NMS_INPUT_s NmsInput, NMS_CFG_s NmsCfg, NMS_OUTPUT_s *pNmsOutput)
{
    UINT32 RetVal = ARM_OK, i = 0;

    if (pNmsOutput == NULL) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_NmsCalc(): pNmsOutput is null", 0U, 0U);
        RetVal = ARM_NG;
    }
    if ((NmsInput.pPriorbox == NULL) || (NmsInput.pLoc == NULL) || (NmsInput.pConf == NULL)) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_NmsCalc(): input is null", 0U, 0U);
        RetVal = ARM_NG;
    }
    if (NmsCfg.PriorBoxNum > MAX_PRIORBOX_NUM) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_NmsCalc(): PriorBoxNum (%u) exceed max priorbox num (%u)", NmsCfg.PriorBoxNum, MAX_PRIORBOX_NUM);
        RetVal = ARM_NG;
    }
    if ((NmsCfg.TopK > MAX_TOPK_NUM) || (NmsCfg.NmsTopK > MAX_TOPK_NUM)) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_NmsCalc(): TopK (%d) or NmsTopK (%d) exceed max num", NmsCfg.TopK, NmsCfg.NmsTopK);
        RetVal = ARM_NG;
    }
    if ((NmsCfg.ConfThrd <= 0.0f) || (NmsCfg.NmsThrd <= 0.0f)) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_NmsCalc(): threshold <= 0", 0U, 0U);
        RetVal = ARM_NG;
    }
    if (NmsCfg.ModelType >= MAX_MODEL_TYPE_NUM) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_NmsCalc(): ModelType (%d) is invalid", NmsCfg.ModelType, 0U);
        RetVal = ARM_NG;
    }

    if ((RetVal == ARM_OK) && (pNmsOutput != NULL)) {
        /* 1. Clear Decode box and NMS box*/
        clean_decodebox();
        clean_nmsbox();

        /* 2. Find suitable box for each class (NMS) */
        for (i = 0; i < NmsCfg.ClassNum; i++) {
            if (i == 0U) { // skip back_ground class
                continue;
            }
            apply_nms(i, G_DecodeBox, NmsInput.pPriorbox, NmsInput.pLoc, NmsInput.pConf, NmsCfg.ConfThrd, NmsCfg.NmsThrd,
                      NmsCfg.TopK, NmsCfg.NmsTopK, NmsCfg.PriorBoxNum, NmsCfg.ClassNum, NmsCfg.ModelType);
        }

        pNmsOutput->pNmsBox = G_NmsBox;
        pNmsOutput->NmsBoxNum = G_NmsBoxNum;
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmSsdFusion_VisCalc
 *
 *  @Description:: Do SSD visual calculation
 *
 *  @Input      ::
 *    NmsOutput:   The bounding box after NMS
 *    VisCfg:      The visual config
 *
 *  @Output     ::
 *    pVisOutput:  The bounding box for visual
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmSsdFusion_VisCalc(NMS_OUTPUT_s NmsOutput, VIS_CFG_s VisCfg, VIS_OUTPUT_s *pVisOutput)
{
    UINT32 RetVal = ARM_OK, i = 0;
    FLOAT RoiXmin, RoiXmax, RoiYmin, RoiYmax, Tmp_fp32;
    /*
      ArmLog_DBG(ARM_LOG_SSD_FUSION, "ROI (%u x %x)", VisCfg.RoiWidth, VisCfg.RoiHeight);
      ArmLog_DBG(ARM_LOG_SSD_FUSION, "NET (%u x %x)", VisCfg.NetworkWidth, VisCfg.NetworkHeight);
      ArmLog_DBG(ARM_LOG_SSD_FUSION, "VIS (%u x %x)", VisCfg.VisWidth, VisCfg.VisHeight);
    */

    if (pVisOutput == NULL) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_VisCalc(): pVisOutput is null", 0U, 0U);
        RetVal = ARM_NG;
    }
    if (NmsOutput.pNmsBox == NULL) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_VisCalc(): NmsOutput param is null", 0U, 0U);
        RetVal = ARM_NG;
    }
    if ((VisCfg.RoiWidth == 0U) || (VisCfg.RoiHeight == 0U) || (VisCfg.NetworkWidth == 0U) || (VisCfg.NetworkHeight == 0U) ||
        (VisCfg.VisWidth == 0U) || (VisCfg.VisHeight == 0U)) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_VisCalc(): window size should not be 0", 0U, 0U);
        RetVal = ARM_NG;
    }
    if (((VisCfg.RoiStartX + VisCfg.NetworkWidth) > VisCfg.RoiWidth) || ((VisCfg.RoiStartY + VisCfg.NetworkHeight) > VisCfg.RoiHeight)) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_VisCalc(): Network window exceed ROI window", 0U, 0U);
        RetVal = ARM_NG;
    }
    if (VisCfg.VisThrd <= 0.0f) {
        ArmLog_ERR(ARM_LOG_SSD_FUSION, "## ArmSsdFusion_VisCalc(): threshold <= 0", 0U, 0U);
        RetVal = ARM_NG;
    }

    if ((RetVal == ARM_OK) && (pVisOutput != NULL)) {

        clean_visbox();
        for (i = 0; i < NmsOutput.NmsBoxNum; i ++) {
            if (NmsOutput.pNmsBox[i].Score > VisCfg.VisThrd) {
                RoiXmin = (MIN(NmsOutput.pNmsBox[i].Xmin, NmsOutput.pNmsBox[i].Xmax) * (FLOAT) VisCfg.NetworkWidth) + (FLOAT) VisCfg.RoiStartX;
                RoiXmax = (MAX(NmsOutput.pNmsBox[i].Xmin, NmsOutput.pNmsBox[i].Xmax) * (FLOAT) VisCfg.NetworkWidth) + (FLOAT) VisCfg.RoiStartX;
                RoiYmin = (MIN(NmsOutput.pNmsBox[i].Ymin, NmsOutput.pNmsBox[i].Ymax) * (FLOAT) VisCfg.NetworkHeight) + (FLOAT) VisCfg.RoiStartY;
                RoiYmax = (MAX(NmsOutput.pNmsBox[i].Ymin, NmsOutput.pNmsBox[i].Ymax) * (FLOAT) VisCfg.NetworkHeight) + (FLOAT) VisCfg.RoiStartY;

                G_VisBox[G_VisBoxNum].Class = NmsOutput.pNmsBox[i].Class;
                G_VisBox[G_VisBoxNum].Score = NmsOutput.pNmsBox[i].Score;

                /*  X = RoiXmin * VisWidth  / RoiWidth
                 *  Y = RoiYmin * VisHeight / RoiHeight
                 *  W = (RoiXmax - RoiXmin) * VisWidth  / RoiWidth
                 *  H = (RoiYmax - RoiYmin) * VisHeight / RoiHeight
                 */
                Tmp_fp32 = RoiXmin * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
                G_VisBox[G_VisBoxNum].X = (UINT32)Tmp_fp32;
                Tmp_fp32 = RoiYmin * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;
                G_VisBox[G_VisBoxNum].Y = (UINT32)Tmp_fp32;
                Tmp_fp32 = (RoiXmax - RoiXmin) * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
                G_VisBox[G_VisBoxNum].W = (UINT32)Tmp_fp32;
                Tmp_fp32 = (RoiYmax - RoiYmin) * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;;
                G_VisBox[G_VisBoxNum].H = (UINT32)Tmp_fp32;
                G_VisBoxNum ++;
            }
        }

        /* Add Net ROI */
        if (VisCfg.ShowDetRegion == 1U) {
            RoiXmin = (FLOAT) VisCfg.RoiStartX;
            RoiXmax = (FLOAT) VisCfg.NetworkWidth + (FLOAT) VisCfg.RoiStartX;
            RoiYmin = (FLOAT) VisCfg.RoiStartY;
            RoiYmax = (FLOAT) VisCfg.NetworkHeight + (FLOAT) VisCfg.RoiStartY;

            G_VisBox[G_VisBoxNum].Class = CLASS_DET_REGION;
            G_VisBox[G_VisBoxNum].Score = 1.0f;
            Tmp_fp32 = RoiXmin * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
            G_VisBox[G_VisBoxNum].X = (UINT32)Tmp_fp32;
            Tmp_fp32 = RoiYmin * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;
            G_VisBox[G_VisBoxNum].Y = (UINT32)Tmp_fp32;
            Tmp_fp32 = (RoiXmax - RoiXmin) * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
            G_VisBox[G_VisBoxNum].W = (UINT32)Tmp_fp32;
            Tmp_fp32 = (RoiYmax - RoiYmin) * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;;
            G_VisBox[G_VisBoxNum].H = (UINT32)Tmp_fp32;
            G_VisBoxNum ++;
        }
        pVisOutput->pVisBox = G_VisBox;
        pVisOutput->VisBoxNum = G_VisBoxNum;
    }

    return RetVal;
}
