/**
 *  @file RfcnProc.c
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details The RFCN process algorithm
 *
 */

/**
 * CLASSES = ('__background__',
 *            'aeroplane', 'bicycle', 'bird', 'boat',
 *            'bottle', 'bus', 'car', 'cat', 'chair',
 *            'cow', 'diningtable', 'dog', 'horse',
 *            'motorbike', 'person', 'pottedplant',
 *            'sheep', 'sofa', 'train', 'tvmonitor')
 */

#include "CvCommFlexi.h"
#include "RfcnProc.h"

#define INPUT_SIZE_224_299
#define EXP                       (2.71828182845904)

#define ARM_LOG_RFCN             "RfcnProc"
#define ENABLE_DEBUG_LOG          (1U)
#define PART_1_INDEX              (0U)
#define PART_2_INDEX              (1U)

/* RPN */
#ifdef INPUT_SIZE_224_299
#define INPUT_BOX_NUM             (2394U)     // Input bbox number for RPN
#define INPUT_NODE_HEIGHT         (224U)      // Height of input node
#define INPUT_NODE_WIDTH          (299U)      // Width of input node
#define RPN_MIN_SIZE              (16U)       // Proposal height and width both need to be greater than RPN_MIN_SIZE (at original image scale)
#define PRE_NMS_TOP_NUM           (100U)      // Input bbox number of nms
#define POST_NMS_TOP_NUM          (20U)       // Output bbox number of nms
#else
#define INPUT_BOX_NUM             (17100U)    // Input bbox number for RPN
#define INPUT_NODE_HEIGHT         (600U)      // Height of input node
#define INPUT_NODE_WIDTH          (800U)      // Width of input node
#define RPN_MIN_SIZE              (16U)       // Proposal height and width both need to be greater than RPN_MIN_SIZE (at original image scale)
#define PRE_NMS_TOP_NUM           (6000U)     // Input bbox number of nms
#define POST_NMS_TOP_NUM          (300U)      // Output bbox number of nms
#endif

/* ROI pooling */
#define FEATURE_SCALE             (16U)       // Feature scale between input node and feature
#ifdef INPUT_SIZE_224_299
#define FEATURE_HEIGHT            (14U)       // Feature height
#define FEATURE_WIDTH             (19U)       // Feature width
#else
#define FEATURE_HEIGHT            (38U)       // Feature height
#define FEATURE_WIDTH             (50U)       // Feature width
#endif
#define FEATURE_DEPTH             (512U)      // Feature depth
#define POOLING_HEIGHT            (7U)        // ROI Pooling height
#define POOLING_WIDTH             (7U)        // ROI Pooling width

/* Classifier */
#define CLASS_NUM                 (21U)       // Class number
#define CLASS_TOPK                (10U)       // Final topK of each class

/* Internal configs */
#ifdef INPUT_SIZE_224_299
#define RFCN_ANCHOR              "./anchors_224_299.bin"
#else
#define RFCN_ANCHOR              "./anchors.bin"
#endif
#define RFCN_DEPTH               (4U)
#define RFCN_DEPTH_PLUS_1        (RFCN_DEPTH + 1U)
#define RFCN_RPN_BOX_PRED_SIZE   (FEATURE_HEIGHT * FEATURE_WIDTH * 9U * 4U)
#define RFCN_RPN_CLS_PROB_SIZE   (FEATURE_HEIGHT * FEATURE_WIDTH * 9U * 1U)
#define RFCN_BOX_PRED_SIZE        (8U)
#define RFCN_CLS_SCORE_MAP_SIZE   (FEATURE_HEIGHT * FEATURE_WIDTH * CLASS_NUM * POOLING_HEIGHT * POOLING_WIDTH)
#define RFCN_LOC_SCORE_MAP_SIZE   (FEATURE_HEIGHT * FEATURE_WIDTH * RFCN_BOX_PRED_SIZE * POOLING_HEIGHT * POOLING_WIDTH)
#define RFCN_CLS_PROB_SIZE        (CLASS_NUM)
#define RFCN_PSROI_POOL_CLS_SIZE  (1U * CLASS_NUM * POOLING_HEIGHT * POOLING_WIDTH)
#define RFCN_PSROI_POOL_LOC_SIZE  (1U * RFCN_BOX_PRED_SIZE * POOLING_HEIGHT * POOLING_WIDTH)
#define USR_RPN_NMS_THRESH        (0.7f)
#define USR_CLS_NMS_THRESH        (0.3f)
#define USR_CLS_CONF_THRESH       (0.5f)

typedef struct {
    void              *pUserData;
    UINT32            Count;
    UINT32            Idx;
    flexidag_memblk_t PSRoiPoolClsBuf;         // (1, 21, 7, 7)
    flexidag_memblk_t PSRoiPoolLocBuf;         // (1, 8, 7, 7)
} FRAME_BUF_s;

typedef struct {
    void        *pUserData;
    UINT32      Wp;                       // Write index
    UINT32      AvblNum;                  // Available buffer number
    UINT32      MaxAvblNum;               // The maximum available buffer number
    FRAME_BUF_s Buf[RFCN_DEPTH_PLUS_1];
    ArmMutex_t  Mutex;
} FRAME_CTRL_s;

typedef struct {
    UINT32             AnchorNum;
    flexidag_memblk_t  AnchorBuf;
    FRAME_CTRL_s       FrameCtrl;
    RFCN_PROC_PARAM_s Param;
} RFCN_PROC_CTRL_s;

typedef struct {
    FLOAT  Score;
    FLOAT  Xmin;
    FLOAT  Ymin;
    FLOAT  Xmax;
    FLOAT  Ymax;
    UINT8  Keep;
    UINT32 InitIdx;
} RPN_BOX_s;

typedef struct {
    FLOAT ImgScaleRatio;                  // Scale between height of input node and Min(source image height, source image width)
    FLOAT NmsThrehold;                    // Threshold of NMS
} RPN_CFG_s;

typedef struct {
    FLOAT *pAnchor;
    FLOAT *pDeltaBox;
    FLOAT *pScore;
} RPN_INPUT_s;

typedef struct {
    RPN_BOX_s Box[INPUT_BOX_NUM];
    UINT32    BoxIdx[POST_NMS_TOP_NUM];
    UINT32    ValidNum;
} RPN_OUTPUT_s;

typedef struct {
    UINT32 FeatureScale;
    UINT32 FeatureDepth;
    UINT32 FeatureHeight;
    UINT32 FeatureWidth;
    UINT32 PoolingHeight;
    UINT32 PoolingWidth;
    UINT32 PoolingDepth;
} POOL_CFG_s;

typedef struct {
    RPN_BOX_s Box;
    FLOAT     *pFeature;
} POOL_INPUT_s;

typedef struct {
    FLOAT *pFeature;
} POOL_OUTPUT_s;

typedef struct {
    UINT32 ValidNum;
    UINT32 ImgHeight;
    UINT32 ImgWidth;
} FUSION_CFG_s;

typedef struct {
    FLOAT *pPredBox;
    FLOAT *pScore;
    FLOAT *pRoi;
} FUSION_INPUT_s;

typedef struct {
    RPN_BOX_s ClsBox[CLASS_NUM][POST_NMS_TOP_NUM];
    UINT32    ClsBoxIdx[CLASS_NUM][CLASS_TOPK];
} FUSION_OUTPUT_s;

typedef struct {
    ArmMutex_t   Mutex;
    RPN_OUTPUT_s RpnOutput;
    UINT32       Count;
    FLOAT        BoxPred[POST_NMS_TOP_NUM][RFCN_BOX_PRED_SIZE];
    FLOAT        ClsProb[POST_NMS_TOP_NUM][RFCN_CLS_PROB_SIZE];
} RFCN_INTERNAL_s;

static RFCN_INTERNAL_s g_RfcnInternal = {0};

/*---------------------------------------------------------------------------*\
 * Static functions
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: Align8
 *
 *  @Description:: Align 8
 *
 *  @Input      ::
 *     InputNum:   Input number
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      Align8 number
\*---------------------------------------------------------------------------*/
static inline UINT32 Align8(UINT32 InputNum)
{
    UINT32 Rval = 0;

    if ((InputNum % 8U) != 0U) {
        Rval = ((InputNum / 8U) + 1U) * 8U;
    } else {
        Rval = InputNum;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: MaxFloat
 *
 *  @Description:: Get max value
 *
 *  @Input      ::
 *     InputX:     Input number x
 *     InputY:     Input number y
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      Max number
\*---------------------------------------------------------------------------*/
static inline FLOAT MaxFloat(FLOAT InputX, FLOAT InputY)
{
    FLOAT Rval = 0.0f;

    if (InputX > InputY) {
        Rval = InputX;
    } else {
        Rval = InputY;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: MaxUint32
 *
 *  @Description:: Get max value
 *
 *  @Input      ::
 *     InputX:     Input number x
 *     InputY:     Input number y
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      Max number
\*---------------------------------------------------------------------------*/
static inline UINT32 MaxUint32(UINT32 InputX, UINT32 InputY)
{
    UINT32 Rval = 0U;

    if (InputX > InputY) {
        Rval = InputX;
    } else {
        Rval = InputY;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: MinUint32
 *
 *  @Description:: Get max value
 *
 *  @Input      ::
 *     InputX:     Input number x
 *     InputY:     Input number y
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      Min number
\*---------------------------------------------------------------------------*/
static inline UINT32 MinUint32(UINT32 InputX, UINT32 InputY)
{
    UINT32 Rval = 0U;

    if (InputX < InputY) {
        Rval = InputX;
    } else {
        Rval = InputY;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: MinFloat
 *
 *  @Description:: Get max value
 *
 *  @Input      ::
 *     InputX:     Input number x
 *     InputY:     Input number y
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      Min number
\*---------------------------------------------------------------------------*/
static inline FLOAT MinFloat(FLOAT InputX, FLOAT InputY)
{
    FLOAT Rval = 0.0f;

    if (InputX < InputY) {
        Rval = InputX;
    } else {
        Rval = InputY;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: GetAvailableBufIdx
 *
 *  @Description:: Get available buffer index
 *
 *  @Input      ::
 *     pBufCtrl:   Pointer to buffer control
 *
 *  @Output     ::
 *     pIndex:     Pointer to buffer index
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 GetAvailableBufIdx(FRAME_CTRL_s *pBufCtrl, UINT32 *pIndex)
{
    UINT32 Rval = ARM_OK;
    UINT32 WaitTime = 20;  // 20ms

    /* 1. Sanity check */
    if (pBufCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## pBufCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }
    if (pIndex == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## pIndex is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Get available index */
    if ((pBufCtrl != NULL) && (pIndex != NULL)) {
        do {
            (void) ArmMutex_Take(&pBufCtrl->Mutex);

            if (pBufCtrl->AvblNum > 0U) {
                pBufCtrl->AvblNum--;

                *pIndex = pBufCtrl->Wp;
                pBufCtrl->Wp ++;
                pBufCtrl->Wp %= pBufCtrl->MaxAvblNum;

                (void) ArmMutex_Give(&pBufCtrl->Mutex);
                break;
            } else {
                (void) ArmMutex_Give(&pBufCtrl->Mutex);
                ArmLog_WARN(ARM_LOG_RFCN, "Warning!! Cannot get index, wait %ums", WaitTime, 0U);
                (void) ArmTask_Sleep(WaitTime);
            }
        } while (Rval == ARM_OK);
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FreeBufIdx
 *
 *  @Description:: Free buffer index
 *
 *  @Input      ::
 *     pBufCtrl:   Pointer to buffer control
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FreeBufIdx(FRAME_CTRL_s *pBufCtrl)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check */
    if (pBufCtrl == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## pBufCtrl is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Free index */
    if ((Rval == ARM_OK) && (pBufCtrl != NULL)) {
        (void) ArmMutex_Take(&pBufCtrl->Mutex);

        if (pBufCtrl->AvblNum >= pBufCtrl->MaxAvblNum) {
            ArmLog_DBG(ARM_LOG_RFCN, "## can't free index due to incorrect AvblNum %u", pBufCtrl->AvblNum, 0U);
            Rval = ARM_NG;
        } else {
            pBufCtrl->AvblNum++;
        }

        (void) ArmMutex_Give(&pBufCtrl->Mutex);
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetAlgoCtrl
 *
 *  @Description:: Get the AlgoCtrl struct pointer from pHdlr
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    RFCN_PROC_CTRL_s*: The pointer to RFCN_PROC_CTRL_s
\*-----------------------------------------------------------------------------------------------*/
static RFCN_PROC_CTRL_s *GetAlgoCtrl(const SVC_CV_ALGO_HANDLE_s* pHdlr)
{
    RFCN_PROC_CTRL_s *pAlgoCtrl = NULL;

    if (pHdlr != NULL) {
        (void) ArmStdC_memcpy(&pAlgoCtrl, &(pHdlr->pAlgoCtrl), sizeof(void *));
    }

    return pAlgoCtrl;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: SendCallback
 *
 *  @Description:: Send callback to top level
 *
 *  @Input      ::
 *    Event:       The callback event
 *    pHdlr:       The SvcCvAlgo handler
 *    pOutData:    The callback output data
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void SendCallback(UINT32 Event, const SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_OUTPUT_s *pOutData)
{
    if ((pHdlr != NULL) && (pOutData != NULL)) {
        for (UINT32 Idx = 0U; Idx < MAX_CALLBACK_NUM; Idx++) {
            const SVC_CV_ALGO_CALLBACK_f *pCvAlgoCB = &(pHdlr->Callback[Idx]);
            if ((*pCvAlgoCB) == NULL) {
                continue;
            } else {
                (void) (*pCvAlgoCB)(Event, pOutData);
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: GetBoxSize
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
static FLOAT GetBoxSize(FLOAT Xmin, FLOAT Ymin, FLOAT Xmax, FLOAT Ymax)
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
 *  @RoutineName:: JaccardOverlap
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
static FLOAT JaccardOverlap(FLOAT X1min, FLOAT Y1min, FLOAT X1max, FLOAT Y1max,
                            FLOAT X2min, FLOAT Y2min, FLOAT X2max, FLOAT Y2max)
{
    FLOAT InterXmin = MaxFloat(X1min, X2min);
    FLOAT InterYmin = MaxFloat(Y1min, Y2min);
    FLOAT InterXmax = MinFloat(X1max, X2max);
    FLOAT InterYmax = MinFloat(Y1max, Y2max);

    FLOAT InterWidth = InterXmax - InterXmin;
    FLOAT InterHeight = InterYmax - InterYmin;
    FLOAT InterSize = InterWidth * InterHeight;

    FLOAT Bbox1Size = GetBoxSize(X1min, Y1min, X1max, Y1max);
    FLOAT Bbox2Size = GetBoxSize(X2min, Y2min, X2max, Y2max);

    return InterSize / (Bbox1Size + Bbox2Size - InterSize);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: FilterBox
 *
 *  @Description:: Filter Box
 *
 *  @Input      ::
 *    BoxNum:       Number of boxes
 *    Size:         Target size
 *
 *  @Output     ::
 *    pBox:         Pointer to boxes
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 FilterBox(UINT32 BoxNum, FLOAT Size, RPN_BOX_s *pBox)
{
    UINT32 i;

    if ((BoxNum > 0U) && (pBox != NULL)) {
        for (i = 0U ; i < BoxNum; i++) {
            if (MinFloat(pBox[i].Xmax - pBox[i].Xmin + 1.0f, pBox[i].Ymax - pBox[i].Ymin + 1.0f) < Size) {
                pBox[i].Keep = 0U;
            } else {
                pBox[i].Keep = 1U;
            }
            if (pBox[i].Keep == 0U) {
                pBox[i].Score = 0.0f;
            }
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ClipBox
 *
 *  @Description:: Clip Box
 *
 *  @Input      ::
 *    BoxNum:       Number of boxes
 *    Width:        Target width
 *    Height:       Target height
 *
 *  @Output     ::
 *    pBox:         Pointer to boxes
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ClipBox(UINT32 BoxNum, UINT32 Width, UINT32 Height, RPN_BOX_s *pBox)
{
    UINT32 i;

    if ((BoxNum > 0U) && (pBox != NULL)) {
        for (i = 0U ; i < BoxNum; i++) {
            if (pBox[i].Xmin > 0.0f) {
                pBox[i].Xmin = pBox[i].Xmin;
            } else {
                pBox[i].Xmin = 0.0f;
            }
            if (pBox[i].Ymin > 0.0f) {
                pBox[i].Ymin = pBox[i].Ymin;
            } else {
                pBox[i].Ymin = 0.0f;
            }
            if (pBox[i].Xmax < (FLOAT) Width) {
                pBox[i].Xmax = pBox[i].Xmax;
            } else {
                pBox[i].Xmax = (FLOAT) Width - 1.0f;
            }
            if (pBox[i].Ymax < (FLOAT) Height) {
                pBox[i].Ymax = pBox[i].Ymax;
            } else {
                pBox[i].Ymax = (FLOAT) Height - 1.0f;
            }
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: BoxRestoration
 *
 *  @Description:: Restorate Box
 *
 *  @Input      ::
 *    BoxNum:       Number of boxes
 *    DeltaNum:     Number of deltas
 *    pBox:         Pointer to boxes
 *    pDelta:       Pointer to deltas
 *    pScore:       Pointer to scores
 *    AlignMode_8:  Flag indicate if data is 8 byte alignment
 *
 *  @Output     ::
 *    pPredictBox:  Pointer to predict boxes
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 BoxRestoration(UINT32 BoxNum, UINT32 DeltaNum, const FLOAT *pBox, const FLOAT *pDelta, const FLOAT *pScore, UINT8 AlignMode_8, RPN_BOX_s *pPredictBox)
{
    UINT32 i;
    FLOAT Width, Height;
    FLOAT CenterX, CenterY;
    FLOAT DeltaX, DeltaY, DeltaW, DeltaH;
    FLOAT PredictCenterX, PredictCenterY;
    FLOAT PredictWidth, PredictHeight;

    if ((BoxNum > 0U) && (DeltaNum > 0U) && (pBox != NULL) && (pDelta != NULL) && (pPredictBox != NULL)) {
        for (i = 0U ; i < DeltaNum; i++) {
            if (BoxNum == DeltaNum) {
                Width = pBox[(i * 4U) + 2U] - pBox[i * 4U] + 1.0f;
                Height = pBox[(i * 4U) + 3U] - pBox[(i * 4U) + 1U] + 1.0f;
                CenterX = pBox[i * 4U] + (0.5f * Width);
                CenterY = pBox[(i * 4U) + 1U] + (0.5f * Height);
            } else {
                Width = pBox[2U] - pBox[0U] + 1.0f;
                Height = pBox[3U] - pBox[1U] + 1.0f;
                CenterX = pBox[0U] + (0.5f * Width);
                CenterY = pBox[1U] + (0.5f * Height);
            }

            if (AlignMode_8 != 0U) {
                DeltaX = pDelta[i * 8U];
                DeltaY = pDelta[(i * 8U) + 1U];
                DeltaW = pDelta[(i * 8U) + 2U];
                DeltaH = pDelta[(i * 8U) + 3U];
            } else {
                DeltaX = pDelta[i * 4U];
                DeltaY = pDelta[(i * 4U) + 1U];
                DeltaW = pDelta[(i * 4U) + 2U];
                DeltaH = pDelta[(i * 4U) + 3U];
            }

            PredictCenterX = (DeltaX * Width) + CenterX;
            PredictCenterY = (DeltaY * Height) + CenterY;
            PredictWidth = (FLOAT) ArmStdC_pow((DOUBLE) EXP, (DOUBLE) DeltaW) * Width;
            PredictHeight = (FLOAT) ArmStdC_pow((DOUBLE) EXP, (DOUBLE) DeltaH) * Height;

            pPredictBox[i].Xmin = PredictCenterX - (0.5f * PredictWidth);
            pPredictBox[i].Ymin = PredictCenterY - (0.5f * PredictHeight);
            pPredictBox[i].Xmax = PredictCenterX + (0.5f * PredictWidth);
            pPredictBox[i].Ymax = PredictCenterY + (0.5f * PredictHeight);

            if (pScore != NULL) {
                if (AlignMode_8 != 0U) {
                    pPredictBox[i].Score = pScore[i * 8U];
                } else {
                    pPredictBox[i].Score = pScore[i];
                }
            }

            pPredictBox[i].Keep = 1U;
            pPredictBox[i].InitIdx = i;
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: BoxRestorationAgnostic
 *
 *  @Description:: Restorate Box
 *
 *  @Input      ::
 *    BoxNum:       Number of boxes
 *    DeltaNum:     Number of deltas
 *    pBox:         Pointer to boxes
 *    pDelta:       Pointer to deltas
 *    pScore:       Pointer to scores
 *    AlignMode_8:  Flag indicate if data is 8 byte alignment
 *
 *  @Output     ::
 *    pPredictBox:  Pointer to predict boxes
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 BoxRestorationAgnostic(UINT32 BoxNum, UINT32 DeltaNum, const FLOAT *pBox, const FLOAT *pDelta, const FLOAT *pScore, UINT8 AlignMode_8, RPN_BOX_s *pPredictBox)
{
    UINT32 i;
    FLOAT Width, Height;
    FLOAT CenterX, CenterY;
    FLOAT DeltaX, DeltaY, DeltaW, DeltaH;
    FLOAT PredictCenterX, PredictCenterY;
    FLOAT PredictWidth, PredictHeight;
    FLOAT Xmin, Ymin, Xmax, Ymax;

    if ((BoxNum > 0U) && (DeltaNum > 0U) && (pBox != NULL) && (pDelta != NULL) && (pPredictBox != NULL)) {
        // calculate roi
        Width = pBox[2U] - pBox[0U] + 1.0f;
        Height = pBox[3U] - pBox[1U] + 1.0f;
        CenterX = pBox[0U] + (0.5f * Width);
        CenterY = pBox[1U] + (0.5f * Height);


        // calculate delta of fg
        DeltaX = pDelta[4U];
        DeltaY = pDelta[5U];
        DeltaW = pDelta[6U];
        DeltaH = pDelta[7U];

        PredictCenterX = (DeltaX * Width) + CenterX;
        PredictCenterY = (DeltaY * Height) + CenterY;
        PredictWidth = (FLOAT) ArmStdC_pow((DOUBLE) EXP, (DOUBLE) DeltaW) * Width;
        PredictHeight = (FLOAT) ArmStdC_pow((DOUBLE) EXP, (DOUBLE) DeltaH) * Height;

        Xmin = PredictCenterX - (0.5f * PredictWidth);
        Ymin = PredictCenterY - (0.5f * PredictHeight);
        Xmax = PredictCenterX + (0.5f * PredictWidth);
        Ymax = PredictCenterY + (0.5f * PredictHeight);

        // assign fg
        for (i = 0U; i < DeltaNum; i++) {
            // box location
            pPredictBox[i].Xmin = Xmin;
            pPredictBox[i].Ymin = Ymin;
            pPredictBox[i].Xmax = Xmax;
            pPredictBox[i].Ymax = Ymax;

            if (pScore != NULL) {
                if (AlignMode_8 != 0U) {
                    pPredictBox[i].Score = pScore[i * 8U];
                } else {
                    pPredictBox[i].Score = pScore[i];
                }
            }

            pPredictBox[i].Keep = 1U;
            pPredictBox[i].InitIdx = i;
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: BoxNMS
 *
 *  @Description:: Box NMS
 *
 *  @Input      ::
 *    BoxNum:      Number of boxes
 *    pBox:        Pointer to boxes
 *    TopK:        Number of top K boxes
 *    Threshold:   Threshold
 *    ValidNum:    Valid number
 *
 *  @Output     ::
 *    OutputIdx:   Output indexes of boxes (-1 means no boxes)
 *    ValidNum:    Valid box number
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 BoxNMS(UINT32 BoxNum, RPN_BOX_s *pBox, UINT32 TopK, FLOAT Threshold, UINT32 *OutputIdx, UINT32 *ValidNum)
{
    UINT32 i, j, KeepIdx;
    FLOAT Overlap;

    KeepIdx = 0U;

    if ((BoxNum > 0U) && (TopK > 0U) && (Threshold > 0.0f) && (pBox != NULL) && (OutputIdx != NULL) && (ValidNum != NULL)) {
        for (i = 0U; i < BoxNum; i++) {
            if (KeepIdx == TopK) {
                break;
            }

            if (i < TopK) {
                OutputIdx[i] = 0xFFFFFFFFU;
            }

            if (pBox[i].Keep == 0U) {
                continue;
            }

            OutputIdx[KeepIdx] = i;
            KeepIdx++;

            for (j = i + 1U; j < BoxNum; j++) {
                Overlap = JaccardOverlap(pBox[i].Xmin, pBox[i].Ymin, pBox[i].Xmax, pBox[i].Ymax,
                                         pBox[j].Xmin, pBox[j].Ymin, pBox[j].Xmax, pBox[j].Ymax);
                if ((pBox[j].Keep != 0U) && (Overlap >= Threshold)) {
                    pBox[j].Keep = 0U;
                }
            }
        }

        *ValidNum = KeepIdx;

        /* Set 0xFFFFFFFF to invalid box indexes when BoxNum < TopK */
        if (BoxNum < TopK) {
            for (i = KeepIdx; i < MaxUint32(BoxNum, TopK); i++) {
                OutputIdx[i] = 0xFFFFFFFFU;
            }
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: MergeSort
 *
 *  @Description:: MergeSort
 *
 *  @Input      ::
 *    pInData:     Pointer to input data array
 *    Left:        Left point
 *    Right:       Right point
 *
 *  @Output     :: None
 *
 *    UINT32       ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MergeSort(RPN_BOX_s *pInData, UINT32 BoxNum)
{
    UINT32 s, i, j, k, l1, h1, l2, h2;
    RPN_BOX_s Temp[INPUT_BOX_NUM];

    for (s = 1U; s < BoxNum; s *= 2U) {
        l1 = 0U;
        k = 0U;

        while ((l1 + s) < BoxNum) {
            h1 = l1 + s - 1U;
            l2 = h1 + 1U;
            h2 = l2 + s - 1U;

            /* Check if h2 exceeds the limit */
            if (h2 >= BoxNum) {
                h2 = BoxNum - 1U;
            }

            /* Merge the two pairs with lower limits l1 and l2 */
            i = l1;
            j = l2;

            while ((i <= h1) && (j <= h2)) {
                if (pInData[i].Score >= pInData[j].Score) {
                    (void) ArmStdC_memcpy(&Temp[k], &pInData[i], sizeof(RPN_BOX_s));
                    k++;
                    i++;
                } else {
                    (void) ArmStdC_memcpy(&Temp[k], &pInData[j], sizeof(RPN_BOX_s));
                    k++;
                    j++;
                }
            }

            while(i <= h1) {
                (void) ArmStdC_memcpy(&Temp[k], &pInData[i], sizeof(RPN_BOX_s));
                k++;
                i++;
            }

            while(j <= h2) {
                (void) ArmStdC_memcpy(&Temp[k], &pInData[j], sizeof(RPN_BOX_s));
                k++;
                j++;
            }

            /* Merge completed, take the next two pairs for merging */
            l1 = h2 + 1U;
        }

        /* If there is any pair left */
        for (i = l1; k < BoxNum; i++) {
            (void) ArmStdC_memcpy(&Temp[k], &pInData[i], sizeof(RPN_BOX_s));
            k++;
        }

        for(i = 0U; i < BoxNum; i++) {
            (void) ArmStdC_memcpy(&pInData[i], &Temp[i], sizeof(RPN_BOX_s));
        }
    }

    return ARM_OK;
}

/*----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Sort-TopKBox
 *
 *  @Description:: Sort top K boxes
 *
 *  @Input      ::
 *    BoxNum:       Number of boxes
 *    pInputBox:    Pointer to input boxes
 *    TopK:         Number of top K boxes
 *
 *  @Output     ::  None
 *
 *  @Return     ::
 *    UINT32        ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 SortTopKBox(UINT32 BoxNum, RPN_BOX_s *pInputBox, UINT32 TopK)
{
    if ((BoxNum > 0U) && (TopK > 0U) && (pInputBox != NULL)) {
        /* Merge sort */
        (void) MergeSort(pInputBox, BoxNum);
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: RegionProposal
 *
 *  @Description:: Region proposal
 *
 *  @Input      ::
 *    Config:      RPN config
 *    pInput:      Pointer to RPN input
 *
 *  @Output     ::
 *    pOutput:     Pointer to RPN output
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RegionProposal(RPN_CFG_s Config, const RPN_INPUT_s *pInput, RPN_OUTPUT_s *pOutput)
{
    if ((pInput != NULL) && (pOutput != NULL)) {
        /* Box restoration */
        (void) BoxRestoration(INPUT_BOX_NUM, INPUT_BOX_NUM, pInput->pAnchor, pInput->pDeltaBox, pInput->pScore, 1U,  pOutput->Box);

        /* Clip boxes */
        (void) ClipBox(INPUT_BOX_NUM, INPUT_NODE_WIDTH, INPUT_NODE_HEIGHT, pOutput->Box);

        /* Filter boxes */
        (void) FilterBox((UINT32) INPUT_BOX_NUM, (Config.ImgScaleRatio * (FLOAT) RPN_MIN_SIZE), pOutput->Box);

        /* Sort top k boxes */
        (void) SortTopKBox(INPUT_BOX_NUM, pOutput->Box, PRE_NMS_TOP_NUM);

        /* Box NMS */
        (void) BoxNMS(PRE_NMS_TOP_NUM, pOutput->Box, POST_NMS_TOP_NUM, Config.NmsThrehold, pOutput->BoxIdx, &pOutput->ValidNum);
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PSRoiPooling
 *
 *  @Description:: Position-sensitive ROI pooling
 *
 *  @Input      ::
 *    Config:      Pooling config
 *    pInput:      Pointer to pooling input
 *
 *  @Output     ::
 *    pOutput:     Pointer to pooling ouput
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 PSRoiPooling(POOL_CFG_s Config, const POOL_INPUT_s *pInput, const POOL_OUTPUT_s *pOutput)
{
    UINT32 pc, pw, ph, PoolIdx;
    UINT32 BinArea;
    UINT32 h, w, StartH, StartW, EndH, EndW; // for calculate pooled value
    FLOAT OutSum;

    FLOAT Xmin = pInput->Box.Xmin;
    FLOAT Ymin = pInput->Box.Ymin;
    FLOAT Xmax = pInput->Box.Xmax;
    FLOAT Ymax = pInput->Box.Ymax;
    DOUBLE SpatialScale = 1.0f / (DOUBLE) Config.FeatureScale;
    UINT32 PoolHeight = Config.PoolingHeight;
    UINT32 PoolWidth = Config.PoolingWidth;
    UINT32 PoolDepth = Config.PoolingDepth;
    UINT32 FeatureHeight = Config.FeatureHeight;
    UINT32 FeatureWidth = Config.FeatureWidth;
    UINT32 FeatureDepth = Config.FeatureDepth;
    UINT32 RoiStartHeight;
    UINT32 RoiStartWidth;
    UINT32 RoiEndHeight;
    UINT32 RoiEndWidth;
    UINT32 RoiHeight;
    UINT32 RoiWidth;
    DOUBLE Temp1, Temp2;
    DOUBLE BinSizeHeight;
    DOUBLE BinSizeWidth;

    Temp1 = ArmStdC_floor(Ymin * SpatialScale);
    Temp2 = ArmStdC_floor(Xmin * SpatialScale);
    RoiStartHeight = (UINT32) Temp1;
    RoiStartWidth = (UINT32) Temp2;

    Temp1 = ArmStdC_floor(Ymax * SpatialScale);
    Temp2 = ArmStdC_floor(Xmax * SpatialScale);
    RoiEndHeight = (UINT32) Temp1;
    RoiEndWidth = (UINT32) Temp2;

    RoiHeight = (UINT32) (MaxUint32(RoiEndHeight - RoiStartHeight + 1U, 1U));
    RoiWidth = (UINT32) (MaxUint32(RoiEndWidth - RoiStartWidth + 1U, 1U));

    BinSizeHeight = (DOUBLE) RoiHeight / (DOUBLE) PoolHeight;
    BinSizeWidth = (DOUBLE) RoiWidth / (DOUBLE) PoolWidth;

    if ((pInput != NULL) && (pOutput != NULL)) {
        for (pc = 0; pc < PoolDepth ; pc++) {
            UINT32 PoolPlaneOffset = pc * PoolHeight * Align8(PoolWidth);
            for (UINT32 ph = 0; ph < PoolHeight; ph++) {
                UINT32 PoolRowOffset = PoolPlaneOffset + ph * Align8(PoolWidth);
                for (UINT32 pw = 0; pw < PoolWidth; pw++) {
                    PoolIdx = PoolRowOffset + pw;
                    DOUBLE H, W;

                    H = ArmStdC_floor((DOUBLE) ph * BinSizeHeight);
                    W = ArmStdC_floor((DOUBLE) pw * BinSizeWidth);
                    StartH = (UINT32) H;
                    StartW = (UINT32) W;

                    H = ArmStdC_ceil(((DOUBLE) ph + 1.0f) * BinSizeHeight);
                    W = ArmStdC_ceil(((DOUBLE) pw + 1.0f) * BinSizeWidth);
                    EndH = (UINT32) H;
                    EndW = (UINT32) W;

                    StartH = (UINT32) (MinUint32(MaxUint32(StartH + RoiStartHeight, 0U), FeatureHeight));
                    EndH = (UINT32) (MinUint32(MaxUint32(EndH + RoiStartHeight, 0U), FeatureHeight));
                    StartW = (UINT32) (MinUint32(MaxUint32(StartW + RoiStartWidth, 0U), FeatureWidth));
                    EndW = (UINT32) (MinUint32(MaxUint32(EndW + RoiStartWidth, 0U), FeatureWidth));

                    UINT32 is_empty = (EndH <= StartH) || (EndW <= StartW);
                    UINT32 c = (pc * PoolHeight + ph) * PoolWidth + pw; // channel of input feature map

                    OutSum = 0.0f;
                    UINT32 FeaturePlaneOffset = c * FeatureHeight * Align8(FeatureWidth);
                    for (h = StartH; h < EndH; h++) {
                        UINT32 FeatureRowOffset = h * Align8(FeatureWidth);
                        for (w = StartW; w < EndW; w++) {
                            UINT32 FeatureIdx = FeatureRowOffset + w;
                            OutSum += pInput->pFeature[FeaturePlaneOffset + FeatureIdx];
                        }
                    }

                    BinArea = (EndH - StartH) * (EndW - StartW);
                    pOutput->pFeature[PoolIdx] = (is_empty == 1U) ? 0. : OutSum / BinArea;
                }
            }
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ClassFusion of agnostic detection
 *
 *  @Description:: Classification fusion
 *
 *  @Input      ::
 *    Config:      Fusion config
 *    pInput:      Pointer to fusion input
 *
 *  @Output     ::
 *    pOutput:     Pointer to fusion ouput
 *
 *  @Return     ::
 *    UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 ClassFusionAgnostic(FUSION_CFG_s Config, const FUSION_INPUT_s *pInput, FUSION_OUTPUT_s *pOutput)
{
    UINT32 n, c;
    RPN_BOX_s TmpPreBox[CLASS_NUM];
    FLOAT TmpBox[4U];
    const FLOAT *pInBox;
    const FLOAT *pInDelta;
    const FLOAT *pInScore;
    RPN_BOX_s *pOutBox;
    UINT32 *pOutBoxIdx;
    UINT32 BoxNum = Config.ValidNum;
    UINT32 ValidNum;

    UINT32 i;
    (void) ArmStdC_memset(&TmpPreBox, 0x0, (sizeof(RPN_BOX_s) * CLASS_NUM));

    if ((pInput != NULL) && (pOutput != NULL)) {
        pInBox = pInput->pRoi;
        pInDelta = pInput->pPredBox;
        pInScore = pInput->pScore;
        pOutBox = &pOutput->ClsBox[0][0];
        pOutBoxIdx = &pOutput->ClsBoxIdx[0][0];

        for (n = 0U; n < BoxNum; n++) {
            TmpBox[0U] = pInBox[n * 4U];
            TmpBox[1U] = pInBox[(n * 4U) + 1U];
            TmpBox[2U] = pInBox[(n * 4U) + 2U];
            TmpBox[3U] = pInBox[(n * 4U) + 3U];

            /* Box restoration */
            (void) BoxRestorationAgnostic(1U, CLASS_NUM, &TmpBox[0], &pInDelta[n * (8U)], &pInScore[n * CLASS_NUM], 0U, &TmpPreBox[0]);

            /* Clip boxes */
            (void) ClipBox(CLASS_NUM, Config.ImgWidth, Config.ImgHeight, &TmpPreBox[0]);

            for (c = 0U; c < CLASS_NUM; c++) {
                (void) ArmStdC_memcpy(&pOutBox[(c * POST_NMS_TOP_NUM) + n], &TmpPreBox[c], sizeof(RPN_BOX_s));
            }
        }

        for (c = 0U; c < CLASS_NUM; c++) {
            /* Sort top k boxes */
            (void) SortTopKBox(BoxNum, (RPN_BOX_s*) &pOutBox[c * POST_NMS_TOP_NUM], BoxNum);

            /* Box NMS */
            (void) BoxNMS(BoxNum, &pOutBox[c * POST_NMS_TOP_NUM], CLASS_TOPK, USR_CLS_NMS_THRESH, &pOutBoxIdx[c * CLASS_TOPK], &ValidNum);
        }
    }

    return ARM_OK;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProcInit
 *
 *  @Description:: RFCN process initialazation
 *
 *  @Input      ::
 *     pCtrl:      The control pointer
 *     MemPoolId:  The memory pool ID
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RfcnProcInit(RFCN_PROC_CTRL_s* pCtrl, UINT32 MemPoolId)
{
    UINT32 Rval = ARM_NG;
    UINT32 FileSize = 0U, LoadSize = 0U;

    if (pCtrl != NULL) {
        ArmLog_STR(ARM_LOG_RFCN, "Anchor: %s", RFCN_ANCHOR, NULL);

        /* 1. Allocate memory for anchor and load */
        Rval = ArmFIO_GetSize(RFCN_ANCHOR, &FileSize);
        if (Rval == ARM_OK) {
            Rval = ArmMemPool_Allocate(MemPoolId, FileSize, &(pCtrl->AnchorBuf));
        }

        if (Rval == ARM_OK) {
            Rval = ArmFIO_Load(pCtrl->AnchorBuf.pBuffer, FileSize, RFCN_ANCHOR, &LoadSize);
            if (LoadSize != FileSize) {
                ArmLog_ERR(ARM_LOG_RFCN, "## Load anchor fail (LoadSize(%u) != FileSize(%u))", LoadSize, FileSize);
                Rval = ARM_NG;
            }
        }

        /* 2. Calculate AnchorNum */
        if (Rval == ARM_OK) {
            pCtrl->AnchorNum = FileSize >> 4;
        }
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProcVisCalc
 *
 *  @Description:: Do visual calculation for OSD
 *
 *  @Input      ::
 *     pRFCNOutput: Pointer to CVALGO_RFCN_OUTPUT_s struct
 *     pParam:       Pointer to RFCN_PROC_PARAM_s struct
 *
 *  @Output     ::
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
static UINT32 RfcnProcVisCalc(CVALGO_RFCN_OUTPUT_s* pRfcnOutput, const RFCN_PROC_PARAM_s* pParam)
{
    UINT32 Rval = ARM_OK;
    UINT32 i;
    FLOAT Temp;

    if (pRfcnOutput == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## RfcnProcVisCalc(): pRfcnOutput is null", 0U, 0U);
        Rval = ARM_NG;
    }

    if (pParam == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## RfcnProcVisCalc(): pParam is null", 0U, 0U);
        Rval = ARM_NG;
    }

    if ((pParam->RoiWidth == 0U) || (pParam->RoiHeight == 0U) || (pParam->NetworkWidth == 0U) || (pParam->NetworkHeight == 0U) ||
        (pParam->VisWidth == 0U) || (pParam->VisHeight == 0U)) {
        ArmLog_ERR(ARM_LOG_RFCN, "## RfcnProcVisCalc(): window size should not be 0", 0U, 0U);
        Rval = ARM_NG;
    }

    if (Rval == ARM_OK) {
        for (i = 0; i < pRfcnOutput->BoxNum; i ++) {
            pRfcnOutput->Box[i].Xmin = pRfcnOutput->Box[i].Xmin + pParam->RoiStartX;
            pRfcnOutput->Box[i].Xmax = pRfcnOutput->Box[i].Xmax + pParam->RoiStartX;
            pRfcnOutput->Box[i].Ymin = pRfcnOutput->Box[i].Ymin + pParam->RoiStartY;
            pRfcnOutput->Box[i].Ymax = pRfcnOutput->Box[i].Ymax + pParam->RoiStartY;

            /*  X = RoiX * VisWidth  / RoiWidth */
            /*  Y = RoiY * VisHeight / RoiHeight */
            Temp = (FLOAT) pRfcnOutput->Box[i].Xmin * (FLOAT) pParam->VisWidth / (FLOAT) pParam->RoiWidth;
            pRfcnOutput->Box[i].Xmin = (UINT32) Temp;
            Temp = (FLOAT) pRfcnOutput->Box[i].Xmax * (FLOAT) pParam->VisWidth / (FLOAT) pParam->RoiWidth;
            pRfcnOutput->Box[i].Xmax = (UINT32) Temp;
            Temp = (FLOAT) pRfcnOutput->Box[i].Ymin * (FLOAT) pParam->VisHeight / (FLOAT) pParam->RoiHeight;
            pRfcnOutput->Box[i].Ymin = (UINT32) Temp;
            Temp = (FLOAT) pRfcnOutput->Box[i].Ymax * (FLOAT) pParam->VisHeight / (FLOAT) pParam->RoiHeight;
            pRfcnOutput->Box[i].Ymax = (UINT32) Temp;
        }

        /* Add ROI rectangle */
        pRfcnOutput->BoxNum++;
        pRfcnOutput->Box[i].Class = 255U;
        pRfcnOutput->Box[i].Xmin = pParam->RoiStartX;
        pRfcnOutput->Box[i].Ymin = pParam->RoiStartY;
        pRfcnOutput->Box[i].Xmax = pParam->RoiStartX + pParam->NetworkWidth;
        pRfcnOutput->Box[i].Ymax = pParam->RoiStartY + pParam->NetworkHeight;

        Temp = (FLOAT) pRfcnOutput->Box[i].Xmin * (FLOAT) pParam->VisWidth / (FLOAT) pParam->RoiWidth;
        pRfcnOutput->Box[i].Xmin = (UINT32) Temp;
        Temp = (FLOAT) pRfcnOutput->Box[i].Xmax * (FLOAT) pParam->VisWidth / (FLOAT) pParam->RoiWidth;
        pRfcnOutput->Box[i].Xmax = (UINT32) Temp;
        Temp = (FLOAT) pRfcnOutput->Box[i].Ymin * (FLOAT) pParam->VisHeight / (FLOAT) pParam->RoiHeight;
        pRfcnOutput->Box[i].Ymin = (UINT32) Temp;
        Temp = (FLOAT) pRfcnOutput->Box[i].Ymax * (FLOAT) pParam->VisHeight / (FLOAT) pParam->RoiHeight;
        pRfcnOutput->Box[i].Ymax = (UINT32) Temp;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Part1Callback
 *
 *  @Description:: Callback for part-1
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void Part1Callback(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    SVC_CV_ALGO_HANDLE_s *pHdlr = NULL;
    const RFCN_PROC_CTRL_s *pRfcnCtrl = NULL;
    FRAME_CTRL_s *pFrameCtrl = NULL;
    RFCN_INTERNAL_s *pRfcnInternal = &g_RfcnInternal;
    RPN_OUTPUT_s *pRpnOutput = &pRfcnInternal->RpnOutput;

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pUserData == NULL) {   // We use pUserData to pass FRAME_CTRL_s
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pUserData (FRAME_CTRL_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pInternal == NULL) {   // We use pInternal to pass final output buffer
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pInternal (final output buffer) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Assign the following pointers
          pHdlr         : CvAlgo handler
          pFrameCtrl    : The control of current frame
          pRFCNCtrl    : RFCN algo control
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));
        (void) ArmStdC_memcpy(&pFrameCtrl, &(pEvnetData->pUserData), sizeof(void *));
        pRfcnCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Sanity check for output size */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pRfcnCtrl != NULL)) {

        /* RPN_BOX_PRED */
        if (pEvnetData->pOut->buf[0].buffer_size < (RFCN_RPN_BOX_PRED_SIZE * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid RPN_BOX_PRED size (%d), please check if RPN_BOX_PRED is fp32 and at 1st output",
                       pEvnetData->pOut->buf[0].buffer_size, 0U);
            Rval = ARM_NG;
        }
        /* RPN_CLS_PROB */
        if (pEvnetData->pOut->buf[1].buffer_size < (RFCN_RPN_CLS_PROB_SIZE * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid RPN_CLS_PROB size (%d), please check if RPN_CLS_PROB is fp32 and at 2nd output",
                       pEvnetData->pOut->buf[1].buffer_size, 0U);
            Rval = ARM_NG;
        }

        /* RFCN_CLS */
        if (pEvnetData->pOut->buf[2].buffer_size < (RFCN_CLS_SCORE_MAP_SIZE * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid RFCN_CLS_SCORE_MAP_SIZE size (%d), please check if RFCN_CLS_SCORE_MAP_SIZE is fp32 and at 3rd output",
                       pEvnetData->pOut->buf[2].buffer_size, 0U);
            Rval = ARM_NG;
        }

        /* RFCN_BBOX */
        if (pEvnetData->pOut->buf[3].buffer_size < (RFCN_LOC_SCORE_MAP_SIZE * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid RFCN_LOC_SCORE_MAP_SIZE size (%d), please check if RFCN_LOC_SCORE_MAP_SIZE is fp32 and at 4nd output",
                       pEvnetData->pOut->buf[3].buffer_size, 0U);
            Rval = ARM_NG;
        }

        if (pRfcnCtrl->AnchorNum == 0U) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid AnchorNum %d", pRfcnCtrl->AnchorNum, 0U);
            Rval = ARM_NG;
        }
    }

    /* 4. Region proposal */
    if ((Rval == ARM_OK) && (pEvnetData != NULL) && (pRfcnCtrl != NULL)) {
        RPN_CFG_s RpnConfig = {
            .ImgScaleRatio = (FLOAT) INPUT_NODE_HEIGHT / (FLOAT) MinUint32(pRfcnCtrl->Param.ImgHeight, pRfcnCtrl->Param.ImgWidth),
            .NmsThrehold = USR_RPN_NMS_THRESH,
        };
        RPN_INPUT_s RpnInput;

        (void) ArmStdC_memcpy(&(RpnInput.pAnchor), &(pRfcnCtrl->AnchorBuf.pBuffer), sizeof(void *));
        (void) ArmStdC_memcpy(&(RpnInput.pDeltaBox), &(pEvnetData->pOut->buf[0].pBuffer), sizeof(void *));
        (void) ArmStdC_memcpy(&(RpnInput.pScore), &(pEvnetData->pOut->buf[1].pBuffer), sizeof(void *));

        Rval = RegionProposal(RpnConfig, &RpnInput, pRpnOutput);
    }

    /* 5. Clear Valid flag and free input */
    if ((Rval == ARM_OK) && (pHdlr != NULL)) {
        if (pRpnOutput->ValidNum == 0U) {
            {
                /* Free input here due to no bbox is detected */
                SVC_CV_ALGO_OUTPUT_s CvAlgoOut;
                CvAlgoOut.pOutput    = NULL;
                CvAlgoOut.pUserData  = pFrameCtrl->pUserData;
                CvAlgoOut.pExtOutput = NULL;

                SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
            }
        }
    }

    /* 6. Call part-2 */
    if ((Rval == ARM_OK) && (pRpnOutput->ValidNum != 0U) && (pHdlr != NULL) && (pRfcnCtrl != NULL) && (pEvnetData != NULL)) {
        UINT32 FrameCount = 0;

        /* Config buffer size */
        pFrameCtrl->AvblNum = RFCN_DEPTH_PLUS_1;
        pFrameCtrl->MaxAvblNum = RFCN_DEPTH_PLUS_1;

        for (UINT32 i = 0U; i < POST_NMS_TOP_NUM; i ++) {
            UINT32 BufIdx = 0;

            if (pRpnOutput->BoxIdx[i] != 0xFFFFFFFFU) {
                /* ROI pooling with valid boxes */
                {
                    POOL_CFG_s PoolConfigCls = {
                        .FeatureScale = FEATURE_SCALE,
                        .FeatureDepth = 1029U,
                        .FeatureHeight = FEATURE_HEIGHT,
                        .FeatureWidth = FEATURE_WIDTH,
                        .PoolingHeight = POOLING_HEIGHT,
                        .PoolingWidth = POOLING_WIDTH,
                        .PoolingDepth = CLASS_NUM,
                    };

                    POOL_CFG_s PoolConfigLoc = {
                        .FeatureScale = FEATURE_SCALE,
                        .FeatureDepth = 392U,
                        .FeatureHeight = FEATURE_HEIGHT,
                        .FeatureWidth = FEATURE_WIDTH,
                        .PoolingHeight = POOLING_HEIGHT,
                        .PoolingWidth = POOLING_WIDTH,
                        .PoolingDepth = RFCN_BOX_PRED_SIZE,
                    };

                    POOL_INPUT_s PoolInputCls, PoolInputLoc;
                    POOL_OUTPUT_s PoolOutputCls, PoolOutputLoc;

                    PoolInputCls.Box = pRpnOutput->Box[pRpnOutput->BoxIdx[i]];
                    PoolInputLoc.Box = pRpnOutput->Box[pRpnOutput->BoxIdx[i]];

                    (void) ArmStdC_memcpy(&(PoolInputCls.pFeature), &(pEvnetData->pOut->buf[2].pBuffer), sizeof(void *));
                    (void) ArmStdC_memcpy(&(PoolInputLoc.pFeature), &(pEvnetData->pOut->buf[3].pBuffer), sizeof(void *));

                    /*  Get available buffer and fill data */
                    (void) GetAvailableBufIdx(pFrameCtrl, &BufIdx);
                    pFrameCtrl->Buf[BufIdx].Idx = BufIdx;
                    pFrameCtrl->Buf[BufIdx].Count = FrameCount;
                    pFrameCtrl->Buf[BufIdx].pUserData = pFrameCtrl->pUserData;
                    (void) ArmStdC_memcpy(&(PoolOutputCls.pFeature), &(pFrameCtrl->Buf[BufIdx].PSRoiPoolClsBuf.pBuffer), sizeof(void *));  // Pointer to the internal ROI buffer
                    (void) ArmStdC_memcpy(&(PoolOutputLoc.pFeature), &(pFrameCtrl->Buf[BufIdx].PSRoiPoolLocBuf.pBuffer), sizeof(void *));  // Pointer to the internal ROI buffer
                    (void) PSRoiPooling(PoolConfigCls, &PoolInputCls, &PoolOutputCls);
                    (void) PSRoiPooling(PoolConfigLoc, &PoolInputLoc, &PoolOutputLoc);
                    FrameCount++;

                    /* Clean cache */
                    if(pFrameCtrl->Buf[BufIdx].PSRoiPoolClsBuf.buffer_cacheable != 0U) {
                        (void) ArmMemPool_CacheClean(&pFrameCtrl->Buf[BufIdx].PSRoiPoolClsBuf);
                    }

                    if(pFrameCtrl->Buf[BufIdx].PSRoiPoolLocBuf.buffer_cacheable != 0U) {
                        (void) ArmMemPool_CacheClean(&pFrameCtrl->Buf[BufIdx].PSRoiPoolLocBuf);
                    }
                }

                /* Call CvCommFlexi_Feed() to run part-2 */
                {
                    memio_source_recv_multi_raw_t CCFRawData = {0};

                    CCFRawData.io[0].addr = pFrameCtrl->Buf[BufIdx].PSRoiPoolLocBuf.buffer_daddr;
                    CCFRawData.io[0].size = RFCN_BOX_PRED_SIZE * POOLING_HEIGHT * Align8(POOLING_WIDTH) * sizeof(FLOAT);
                    CCFRawData.io[0].pitch = 0;


                    CCFRawData.io[1].addr  = pFrameCtrl->Buf[BufIdx].PSRoiPoolClsBuf.buffer_daddr;
                    CCFRawData.io[1].size  = CLASS_NUM * POOLING_HEIGHT * Align8(POOLING_WIDTH) * sizeof(FLOAT);
                    CCFRawData.io[1].pitch = 0;

                    CCFRawData.num_io = 2U;

                    {
                        CCF_FEED_CFG_s FeedCfg;

                        FeedCfg.pIn         = NULL;
                        FeedCfg.pRaw        = &CCFRawData;
                        FeedCfg.pPic        = NULL;
                        FeedCfg.pOut        = NULL;
                        FeedCfg.pUserData   = &pFrameCtrl->Buf[BufIdx];
                        FeedCfg.pCvAlgoHdlr = pHdlr;
                        FeedCfg.pInternal   = pEvnetData->pInternal;

                        Rval = CvCommFlexi_Feed(&(pHdlr->FDs[PART_2_INDEX]), &FeedCfg);
                        if (Rval != ARM_OK) {
                            ArmLog_ERR(ARM_LOG_RFCN, "## fail to feed to part-2 (%u of %u)", i, POST_NMS_TOP_NUM);
                        }
                    }
                }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: Part2Callback
 *
 *  @Description:: Callback for part-2
 *
 *  @Input      ::
 *    pEventData:  Pointer to data (CCF_OUTPUT_s)
 *
 *  @Output     :: None
 *
 *  @Return     :: None
 *
\*-----------------------------------------------------------------------------------------------*/
static void Part2Callback(const CCF_OUTPUT_s *pEvnetData)
{
    UINT32 Rval = ARM_OK;
    const SVC_CV_ALGO_HANDLE_s *pHdlr = NULL;
    const FRAME_BUF_s *pFrameBuf = NULL;
    CVALGO_RFCN_OUTPUT_s *pRfcnOutput = NULL;
    RFCN_PROC_CTRL_s *pRfcnCtrl = NULL;
    RFCN_INTERNAL_s *pRfcnInternal = &g_RfcnInternal;
    const RPN_OUTPUT_s *pRpnOutput = &pRfcnInternal->RpnOutput;

    /* 1. Sanity check for parameters */
    if (pEvnetData == NULL) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pEvnetData is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pEvnetData->pCvAlgoHdlr == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pCvAlgoHdlr is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pUserData == NULL) {   // We use pUserData to pass FRAME_BUF_s
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pUserData (FRAME_BUF_s) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pEvnetData->pInternal == NULL) {   // We use pInternal to pass final output buffer
            ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (pInternal (final output buffer) is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            if (pEvnetData->pOut->num_of_buf != 2U) {
                ArmLog_ERR(ARM_LOG_RFCN, "## Callback() fail (part-2 should have 2 output (num_of_buf = %u)", pEvnetData->pOut->num_of_buf, 0U);
                Rval = ARM_NG;
            }
        }
    }

    /* 2. Assign the following pointers
          pHdlr         : CvAlgo handler
          pFrameBuf     : The control of current frame buffer
          pRfcnOutput  : The buffer to put final output result
          pRdcnCtrl    : RFCN algo control
     */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        (void) ArmStdC_memcpy(&pHdlr, &(pEvnetData->pCvAlgoHdlr), sizeof(void *));
        (void) ArmStdC_memcpy(&pFrameBuf, &(pEvnetData->pUserData), sizeof(void *));
        (void) ArmStdC_memcpy(&pRfcnOutput, &(pEvnetData->pInternal->buf[0].pBuffer), sizeof(void *));
        pRfcnCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Sanity check for output size */
    if ((Rval == ARM_OK) && (pEvnetData != NULL)) {
        if (pEvnetData->pOut->buf[0].buffer_size < (RFCN_BOX_PRED_SIZE * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid box pred output size (%d), please check if dataformat is fp32",
                       pEvnetData->pOut->buf[0].buffer_size, 0U);
            Rval = ARM_NG;
        }
        if (pEvnetData->pOut->buf[1].buffer_size < (RFCN_CLS_PROB_SIZE * sizeof(FLOAT))) {
            ArmLog_ERR(ARM_LOG_RFCN, "## invalid cls prob output size (%d), please check if dataformat is fp32",
                       pEvnetData->pOut->buf[1].buffer_size, 0U);
            Rval = ARM_NG;
        }
    }

    /* 4. Fusion */
    if ((Rval == ARM_OK) && (pRfcnCtrl != NULL) && (pEvnetData != NULL) && (pFrameBuf != NULL)) {
        UINT32 i;
        const FLOAT *pTemp;

        /* Update data */
        (void) ArmMutex_Take(&(pRfcnInternal->Mutex));  // Take mutex

        (void) ArmStdC_memcpy(&pTemp, &(pEvnetData->pOut->buf[0].pBuffer), sizeof(void *));
        for (i = 0U; i < RFCN_BOX_PRED_SIZE; i++) {
            pRfcnInternal->BoxPred[pFrameBuf->Count][i]= pTemp[i];
        }

        (void) ArmStdC_memcpy(&pTemp, &(pEvnetData->pOut->buf[1].pBuffer), sizeof(void *));
        for (i = 0U; i < RFCN_CLS_PROB_SIZE; i++) {
            pRfcnInternal->ClsProb[pFrameBuf->Count][i]= pTemp[i];
        }

        pRfcnInternal->Count++;

#if ENABLE_DEBUG_LOG
        ArmLog_DBG(ARM_LOG_RFCN, "[Frame: %d/%d]", pRfcnInternal->Count, pRpnOutput->ValidNum);
#endif

        (void) ArmMutex_Give(&(pRfcnInternal->Mutex));  // Give mutex

        /* Do fusion when getting the last box */
        if (pRfcnInternal->Count == pRpnOutput->ValidNum) {

            FLOAT RoiBox[POST_NMS_TOP_NUM * 4U];
            FUSION_CFG_s FusionConfig;
            FUSION_INPUT_s FusionInput;
            FUSION_OUTPUT_s FusionOutput;
            UINT32 c, k, TopIdx;

            (void) ArmStdC_memset(&FusionConfig, 0x0, sizeof(FUSION_CFG_s));
            (void) ArmStdC_memset(&FusionInput, 0x0, sizeof(FUSION_INPUT_s));
            (void) ArmStdC_memset(&FusionOutput, 0x0, sizeof(FUSION_OUTPUT_s));

            for (i = 0U; i < pRpnOutput->ValidNum; i++) {
                /* Value / ImgScaleRatio */
                RoiBox[i * 4U] = pRpnOutput->Box[pRpnOutput->BoxIdx[i]].Xmin / ((FLOAT) INPUT_NODE_HEIGHT / (FLOAT) MinUint32(pRfcnCtrl->Param.ImgHeight, pRfcnCtrl->Param.ImgWidth));
                RoiBox[(i * 4U) + 1U] = pRpnOutput->Box[pRpnOutput->BoxIdx[i]].Ymin / ((FLOAT) INPUT_NODE_HEIGHT / (FLOAT) MinUint32(pRfcnCtrl->Param.ImgHeight, pRfcnCtrl->Param.ImgWidth));
                RoiBox[(i * 4U) + 2U] = pRpnOutput->Box[pRpnOutput->BoxIdx[i]].Xmax / ((FLOAT) INPUT_NODE_HEIGHT / (FLOAT) MinUint32(pRfcnCtrl->Param.ImgHeight, pRfcnCtrl->Param.ImgWidth));
                RoiBox[(i * 4U) + 3U] = pRpnOutput->Box[pRpnOutput->BoxIdx[i]].Ymax / ((FLOAT) INPUT_NODE_HEIGHT / (FLOAT) MinUint32(pRfcnCtrl->Param.ImgHeight, pRfcnCtrl->Param.ImgWidth));
            }

            FusionConfig.ValidNum = pRpnOutput->ValidNum;
            FusionConfig.ImgHeight = pRfcnCtrl->Param.ImgHeight;
            FusionConfig.ImgWidth = pRfcnCtrl->Param.ImgWidth;

            FusionInput.pPredBox = &pRfcnInternal->BoxPred[0][0];
            FusionInput.pScore = &pRfcnInternal->ClsProb[0][0];
            FusionInput.pRoi = &RoiBox[0];

            (void) ClassFusionAgnostic(FusionConfig, &FusionInput, &FusionOutput);

            /* Show and Save result */
            pRfcnOutput->BoxNum = 0;

            for (c = 0U; c < CLASS_NUM; c++) {
                for (k = 0U; k < CLASS_TOPK; k++) {
                    TopIdx = FusionOutput.ClsBoxIdx[c][k];

                    if ((TopIdx != 0xFFFFFFFFU) && (FusionOutput.ClsBox[c][TopIdx].Score > USR_CLS_CONF_THRESH) && (c != 0U)) {
                        pRfcnOutput->Box[pRfcnOutput->BoxNum].Class = c;
                        pRfcnOutput->Box[pRfcnOutput->BoxNum].Score = FusionOutput.ClsBox[c][TopIdx].Score;
                        pRfcnOutput->Box[pRfcnOutput->BoxNum].Xmin = (UINT32) FusionOutput.ClsBox[c][TopIdx].Xmin;
                        pRfcnOutput->Box[pRfcnOutput->BoxNum].Ymin = (UINT32) FusionOutput.ClsBox[c][TopIdx].Ymin;
                        pRfcnOutput->Box[pRfcnOutput->BoxNum].Xmax = (UINT32) FusionOutput.ClsBox[c][TopIdx].Xmax;
                        pRfcnOutput->Box[pRfcnOutput->BoxNum].Ymax = (UINT32) FusionOutput.ClsBox[c][TopIdx].Ymax;
                        pRfcnOutput->BoxNum++;
                    }
                }
            }

#if ENABLE_DEBUG_LOG
            for (i = 0U; i < pRfcnOutput->BoxNum; i++) {
                ArmLog_DBG(ARM_LOG_RFCN, "Class: %d", pRfcnOutput->Box[i].Class, 0U);
                ArmLog_DBG(ARM_LOG_RFCN, "Xmin: %d, Xmax: %d", pRfcnOutput->Box[i].Xmin, pRfcnOutput->Box[i].Xmax);
                ArmLog_DBG(ARM_LOG_RFCN, "Ymin: %d, Ymax: %d", pRfcnOutput->Box[i].Ymin, pRfcnOutput->Box[i].Ymax);
                ArmLog_DBG(ARM_LOG_RFCN, "Score: %f", pRfcnOutput->Box[i].Score, 0U);
            }
#endif

            /* Refine output for OSD */
            if (pRfcnCtrl->Param.OsdEnable != 0U) {
                (void) RfcnProcVisCalc(pRfcnOutput, &pRfcnCtrl->Param);
            }

            /* Send CALLBACK_EVENT_OUTPUT */
            if (pHdlr != NULL) {
                SVC_CV_ALGO_OUTPUT_s CvAlgoOut;

                CvAlgoOut.pOutput    = pEvnetData->pInternal;
                CvAlgoOut.pUserData  = pFrameBuf->pUserData;
                CvAlgoOut.pExtOutput = NULL;

                SendCallback(CALLBACK_EVENT_FREE_INPUT, pHdlr, &CvAlgoOut);
                SendCallback(CALLBACK_EVENT_OUTPUT, pHdlr, &CvAlgoOut);
            }

            /* Release frame buffer */
            (void) FreeBufIdx(&pRfcnCtrl->FrameCtrl);
            pRfcnInternal->Count = 0;  // Reset frame count
        } else {
            /* Release frame buffer */
            (void) FreeBufIdx(&pRfcnCtrl->FrameCtrl);
        }
    }
}

/*---------------------------------------------------------------------------*\
 * RFCNProc APIs
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProc_Query
 *
 *  @Description:: Query buffer requirement
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The query config
 *
 *  @Output     ::
 *     pCfg:       The buffer requirement
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RfcnProc_Query(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_QUERY_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Query() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Query() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoObj == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Query() fail (pAlgoObj is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Save pAlgoObj and return required size */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pHdlr->pAlgoObj     = pCfg->pAlgoObj;
        pCfg->TotalReqBufSz = 256 * 1024 * 1024;  // 256 MB
    }

    /* 3. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProc_Create
 *
 *  @Description:: Create flexidag
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The create config
 *
 *  @Output     ::
 *     pCfg:       The output buffer size
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RfcnProc_Create(SVC_CV_ALGO_HANDLE_s* pHdlr, SVC_CV_ALGO_CREATE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    CCF_OUTPUT_INFO_s OutInfo = {0};
    CCF_CREATE_CFG_s  CreateCfg;
    CCF_REGCB_CFG_s   RegCbCfg;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Create() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_PREOPEN) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Create() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pAlgoBuf == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Create() fail (pAlgoBuf is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->NumFD != 2U) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Create() fail (invalid NumFD %u)", pCfg->NumFD, 0U);
            Rval = ARM_NG;
        } else {
            for (UINT32 i = 0U; i < pCfg->NumFD; i ++) {
                if (pCfg->pBin[i] == NULL) {
                    ArmLog_ERR(ARM_LOG_RFCN, "## Create() fail (pBin[%u] is null)", i, 0U);
                    Rval = ARM_NG;
                }
            }
        }
    }

    /* 2. Create memory pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Create(pCfg->pAlgoBuf, &pHdlr->MemPoolId);
    }

    /* 3-1. Allocate buffer for RFCN_PROC_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        flexidag_memblk_t CtrlBuf;
        Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, sizeof(RFCN_PROC_CTRL_s), &CtrlBuf);
        if (Rval == ARM_OK) {
            pHdlr->pAlgoCtrl = CtrlBuf.pBuffer;
            (void) ArmStdC_memset(pHdlr->pAlgoCtrl, 0x0, sizeof(RFCN_PROC_CTRL_s));
        }
    }

    /* 3-2. Allocate buffer for ROI pooling buffer */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RFCN_PROC_CTRL_s *pRfcnCtrl = GetAlgoCtrl(pHdlr);

        if (pRfcnCtrl != NULL) {
            for (UINT32 i = 0U; i < RFCN_DEPTH_PLUS_1; i++) {
                if (Rval == ARM_OK) {
                    Rval = ArmMemPool_Allocate(pHdlr->MemPoolId, RFCN_PSROI_POOL_CLS_SIZE * sizeof(FLOAT), &(pRfcnCtrl->FrameCtrl.Buf[i].PSRoiPoolClsBuf));
                    Rval |= ArmMemPool_Allocate(pHdlr->MemPoolId, RFCN_PSROI_POOL_LOC_SIZE * sizeof(FLOAT), &(pRfcnCtrl->FrameCtrl.Buf[i].PSRoiPoolLocBuf));
                }
            }
        } else {
            ArmLog_ERR(ARM_LOG_RFCN, "## pRfcnCtrl is null", 0U, 0U);
            Rval = ARM_NG;
        }
    }

    /* 3-3. Create mutex */
    if ((Rval == ARM_OK) && (pHdlr != NULL)) {
        char MutexPooling[20] = "mutex_for_pooling";
        char MutexFusion[20] = "mutex_for_fusion";
        RFCN_PROC_CTRL_s *pRfcnCtrl = GetAlgoCtrl(pHdlr);
        Rval = ArmMutex_Create(&pRfcnCtrl->FrameCtrl.Mutex, MutexPooling);
        Rval |= ArmMutex_Create(&g_RfcnInternal.Mutex, MutexFusion);
    }

    /* 4. RFCNProc Init */
    if ((Rval == ARM_OK) && (pHdlr != NULL)) {
        RFCN_PROC_CTRL_s *pRfcnCtrl;
        pRfcnCtrl = GetAlgoCtrl(pHdlr);
        Rval = RfcnProcInit(pRfcnCtrl, pHdlr->MemPoolId);
    }

    /* 5-1. Create flexidag[0] - part-1 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[PART_1_INDEX];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = 0U;
        CreateCfg.OutBufDepth = 1U;             // Automatical allocate output buffer

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE * 2U;  // Need bigger size
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "Part-1");
        Rval = CvCommFlexi_Create(&(pHdlr->FDs[PART_1_INDEX]), &CreateCfg, &OutInfo);
    }

    /* 5-2. Create flexidag[1] - part-2 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CreateCfg.pBin        = pCfg->pBin[PART_2_INDEX];
        CreateCfg.MemPoolId   = pHdlr->MemPoolId;
        CreateCfg.InBufDepth  = RFCN_DEPTH;    // Automatical allocate input buffer
        CreateCfg.OutBufDepth = RFCN_DEPTH;    // Automatical allocate output buffer

        CreateCfg.RunTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.RunTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.RunTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        CreateCfg.CBTskCtrl.StackSz  = DEFAULT_TSK_STACK_SIZE;
        CreateCfg.CBTskCtrl.Priority = DEFAULT_TSK_PRIORITY;
        CreateCfg.CBTskCtrl.CoreSel  = DEFAULT_TSK_CORE_SEL;

        ArmStdC_strcpy(CreateCfg.FDName, MAX_CCF_NAME_LEN, "Part-2");
        Rval = CvCommFlexi_Create(&(pHdlr->FDs[PART_2_INDEX]), &CreateCfg, &OutInfo);
    }

    /* 6-1. Register Callback for part-1 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0U;  // register
        RegCbCfg.Callback = Part1Callback;
        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[PART_1_INDEX]), &RegCbCfg);
    }

    /* 6-2. Register Callback for part-2 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        RegCbCfg.Mode = 0U;  // register
        RegCbCfg.Callback = Part2Callback;
        Rval = CvCommFlexi_RegCallback(&(pHdlr->FDs[PART_2_INDEX]), &RegCbCfg);
    }

    /* 7. Fill output size and change state (output struct = CVALGO_RFCN_OUTPUT_s) */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pCfg->OutputNum = 1U;
        pCfg->OutputSz[0] = sizeof(CVALGO_RFCN_OUTPUT_s);
        pHdlr->State    = CVALGO_STATE_INITED;
        pHdlr->pAlgoBuf = pCfg->pAlgoBuf;
    }

    /* 8. Print the Max mempool usage */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        UINT32 UsedSize = 0U;
        Rval = ArmMemPool_GetUsage(pHdlr->MemPoolId, &UsedSize);
        if (Rval == ARM_OK) {
            ArmLog_DBG(ARM_LOG_RFCN, "Total used memory - %u", UsedSize, 0U);
        }
    }

    /* 9. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProc_Delete
 *
 *  @Description:: Delete all flexidag
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The delete config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RfcnProc_Delete(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_DELETE_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Delete() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Delete() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2-1. Close flexidag[0] - part-1 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[PART_1_INDEX]));
    }

    /* 2-2. Close flexidag[1] - part-2 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = CvCommFlexi_Delete(&(pHdlr->FDs[PART_2_INDEX]));
    }

    /* 3. Delete buffer pool */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        Rval = ArmMemPool_Delete(pHdlr->MemPoolId);
    }

    /* 4. Set State to CVALGO_STATE_PREOPEN */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pHdlr->State = CVALGO_STATE_PREOPEN;
    }

    /* 5. Delete mutex */
    if ((Rval == ARM_OK) && (pHdlr != NULL)) {
        RFCN_PROC_CTRL_s *pRfcnCtrl = GetAlgoCtrl(pHdlr);
        Rval = ArmMutex_Delete(&pRfcnCtrl->FrameCtrl.Mutex);
        Rval |= ArmMutex_Delete(&g_RfcnInternal.Mutex);
    }

    /* 6. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProc_Feed
 *
 *  @Description:: Feed data
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The feed config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RfcnProc_Feed(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_FEED_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    RFCN_PROC_CTRL_s *pRfcnProcCtrl = NULL;
    FRAME_CTRL_s  *pFrameCtrl = NULL;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Feed() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Feed() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pIn == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Feed() fail (pIn is null)", 0U, 0U);
            Rval = ARM_NG;
        } else if (pCfg->pOut == NULL) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Feed() fail (pOut is null)", 0U, 0U);
            Rval = ARM_NG;
        } else {
            Rval = ARM_OK;
        }
    }

    /* 2. Assign RFCN_PROC_CTRL_s and FRAME_CTRL_s */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pRfcnProcCtrl = GetAlgoCtrl(pHdlr);
        if (pRfcnProcCtrl != NULL) {
            pFrameCtrl = &pRfcnProcCtrl->FrameCtrl;
        }
    }

    /* 3. Get available FrameCtrl and fill it */
    if ((Rval == ARM_OK) && (pRfcnProcCtrl != NULL) && (pFrameCtrl != NULL) && (pCfg != NULL)) {
        pFrameCtrl->pUserData = pCfg->pUserData;
    }

    /* 4. Feed data to flexidag[0] - part-1 */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        CCF_FEED_CFG_s FeedCfg;

        FeedCfg.pIn         = pCfg->pIn;
        FeedCfg.pRaw        = NULL;
        FeedCfg.pPic        = NULL;
        FeedCfg.pOut        = NULL;
        FeedCfg.pUserData   = pFrameCtrl;
        FeedCfg.pCvAlgoHdlr = pHdlr;
        FeedCfg.pInternal   = pCfg->pOut;

        Rval = CvCommFlexi_Feed(&(pHdlr->FDs[PART_1_INDEX]), &FeedCfg);
    }

    /* 5. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: RfcnProc_Control
 *
 *  @Description:: Send control to flexidag
 *
 *  @Input      ::
 *     pHdlr:      The SvcCvAlgo handler
 *     pCfg:       The control config
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      CVALGO_OK(0)/CVALGO_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 RfcnProc_Control(SVC_CV_ALGO_HANDLE_s* pHdlr, const SVC_CV_ALGO_CTRL_CFG_s *pCfg)
{
    UINT32 Rval = ARM_OK;
    RFCN_PROC_CTRL_s *pRfcnProcCtrl = NULL;

    /* 1. Sanity check for parameters */
    if ((pHdlr == NULL) || (pCfg == NULL)) {
        ArmLog_ERR(ARM_LOG_RFCN, "## Control() fail (param is null)", 0U, 0U);
        Rval = ARM_NG;
    } else {
        if (pHdlr->State != CVALGO_STATE_INITED) {
            ArmLog_ERR(ARM_LOG_RFCN, "## Control() fail (invalid State %u)", pHdlr->State, 0U);
            Rval = ARM_NG;
        }
    }

    /* 2. Assign AlgoCtrl */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL)) {
        pRfcnProcCtrl = GetAlgoCtrl(pHdlr);
    }

    /* 3. Send message to flexidag */
    if ((Rval == ARM_OK) && (pHdlr != NULL) && (pCfg != NULL) && (pRfcnProcCtrl != NULL)) {
        switch (pCfg->CtrlType) {
        /* Public control */
        case CTRL_TYPE_DMSG: {
            (void) CvCommFlexi_DumpLog(&(pHdlr->FDs[PART_1_INDEX]));
            (void) CvCommFlexi_DumpLog(&(pHdlr->FDs[PART_2_INDEX]));
        }
        break;
        /* Private control */
        case CTRL_TYPE_RFCN_PROC_PARAM: {
            (void) ArmStdC_memcpy(&pRfcnProcCtrl->Param, pCfg->pCtrlParam, sizeof(RFCN_PROC_PARAM_s));
        }
        break;
        default:
            ArmLog_ERR(ARM_LOG_RFCN, "## Control() fail (unknown control type %u)", pCfg->CtrlType, 0U);
            Rval = ARM_NG;
            break;
        }

        pHdlr->State = CVALGO_STATE_INITED;  // To fix misraC
    }

    /* 3. Covert the output value to CVALGO_OK or CVALGO_NG */
    if (Rval == ARM_OK) {
        Rval = CVALGO_OK;
    } else {
        Rval = CVALGO_NG;
    }

    return Rval;
}

SVC_CV_ALGO_OBJ_s CvAlgoObj_RfcnProc = {
    .Name          = "RfcnProc",
    .Query         = RfcnProc_Query,
    .Create        = RfcnProc_Create,
    .Delete        = RfcnProc_Delete,
    .Feed          = RfcnProc_Feed,
    .Control       = RfcnProc_Control,
};
