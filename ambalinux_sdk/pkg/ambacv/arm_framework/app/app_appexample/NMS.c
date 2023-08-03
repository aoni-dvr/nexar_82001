#include <stdio.h>
#include <math.h>
#include "cvtask_api.h"
#include "cvtask_ossrv.h"
#include "rtos/AmbaRTOSWrapper.h"
#include "NMS.h"
#include "preference.h"


// #define DBG_LOG     AmbaPrint_PrintUInt5
// #define CRI_LOG     AmbaPrint_PrintUInt5

#define EXP                 (2.71828182845904)



#define MAX_PRIORBOX_NUM    (256000U)

#define MAX_TOPK_NUM        (400U)
#define MODEL_TYPE_CAFFE    (0)

#define NUM_TOPK            (250U)
#define NUM_NMSTOPK         (200U)
#define CONFTHRD            (0.4f)
#define NMSTHRD             (0.45f)

typedef struct {
    UINT32 raw_w;
    UINT32 raw_h;

    UINT32 image_pyramid_index;
    UINT32 roi_w;
    UINT32 roi_h;

    UINT32 roi_start_col;
    UINT32 roi_start_row;

    UINT32 net_in_w;
    UINT32 net_in_h;
} win_ctx_t;

typedef struct {
    FLOAT x1;
    FLOAT y1;
    FLOAT x2;
    FLOAT y2;

    UINT32    cls;
    FLOAT     conf;
    UINT32    prev_idx;
    UINT32    next_idx;
} nms_dex_bbx_t;

typedef struct {
    FLOAT* pPriorbox;
    FLOAT* pLoc;
    FLOAT* pConf;
} NMS_INPUT_s;

typedef struct {
    UINT32 PriorBoxNum;
    UINT32 ClassNum;
    UINT32 TopK;
    UINT32 NmsTopK;
    FLOAT  ConfThrd;
    FLOAT  NmsThrd;
    UINT32 ModelType;
} NMS_CFG_s;

typedef struct {
    UINT32 Class;
    FLOAT  Score;
    FLOAT  Xmin;
    FLOAT  Ymin;
    FLOAT  Xmax;
    FLOAT  Ymax;
} NMS_BOX_s;

typedef struct {
    UINT32    NmsBoxNum;
    NMS_BOX_s *pNmsBox;
} NMS_OUTPUT_s;

typedef struct {
    UINT32 DecBoxIdx;
    FLOAT  Score;
} SORT_BOX_s;

typedef struct {
    UINT32 RoiWidth;
    UINT32 RoiHeight;

    UINT32 RoiStartX;
    UINT32 RoiStartY;

    UINT32 NetorkWidth;
    UINT32 NetorkHeight;

    UINT32 VisWidth;   // The output window domain (It could be osd or raw)
    UINT32 VisHeight;

    UINT8  ShowDetRegion;
    FLOAT  VisThrd;
} VIS_CFG_s;

typedef struct {
    FLOAT *g_prior_box;
    UINT32 total_bbox;
    UINT32 pb_len;

    FLOAT G_DecodeBox[MAX_PRIORBOX_NUM * 4U]; // = { 0.0f };   // (xmin, ymin, xmax, ymax)
    NMS_BOX_s G_NmsBox[MAX_TOPK_NUM];
    UINT32 G_NmsBoxNum;
    SORT_BOX_s G_SortBox[MAX_TOPK_NUM];
    amba_od_candidate_t *ODCandidates;
    UINT32 ODBoxNum;
} NMS_hndlr_s;

static UINT32 AmbaWrap_pow(DOUBLE base, DOUBLE exponent, void *pV)
{
    UINT32 err = 0;
    DOUBLE v;

    if (pV == NULL) {
        err = 1;
    } else {
        v = pow(base, exponent);
        memcpy(pV, &v, sizeof(v));
        err = 0;
    }

    return err;
}

static UINT32 get_file_length(const char *filename, UINT32 *pb_len)
{
    UINT32 ret, len;
    FILE *ifp;

    ifp = fopen(filename, "rb");
    if (ifp == NULL) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
    } else {
        ret = fseek(ifp, 0LL, SEEK_END);
        if (ret != 0U){
            AmbaPrint_PrintUInt5("fseek fail ", 0U, 0U, 0U, 0U, 0U);
        } else {
            len = ftell(ifp);
            *pb_len = len;
            fclose(ifp);
        }
    }
    return ret;
}

static void load_binary(const char *filename, FLOAT *dst, UINT32 byte_size)
{
    UINT32 ret;
    FILE *ifp;

    ifp = fopen(filename, "rb");
    if (ifp == NULL) {
        AmbaPrint_PrintStr5("Can't load file %s!", filename, NULL, NULL, NULL, NULL);
    } else {
        ret = fread(dst, 1, byte_size, ifp);
        if (ret != byte_size){
            AmbaPrint_PrintUInt5("fread fail: %d", ret, 0U, 0U, 0U, 0U);
        } else {
            AmbaPrint_PrintUInt5("Read %d bytes from prior_box bin", ret, 0, 0, 0, 0);
            fclose(ifp);
        }
    }
}

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
            /* Caffe - the order of pLoc is xmin, ymin, xmax, ymax */
            Loc[0] = pLoc[ix4];       //xmin
            Loc[1] = pLoc[ix4 + 1U];  //ymin
            Loc[2] = pLoc[ix4 + 2U];  //xmax
            Loc[3] = pLoc[ix4 + 3U];  //ymax
        } else {
            /* Tensorflow - the order of pLoc is ymin, xmin, ymax, xmax */
            Loc[0] = pLoc[ix4 + 1U];  //xmin
            Loc[1] = pLoc[ix4];       //ymin
            Loc[2] = pLoc[ix4 + 3U];  //xmax
            Loc[3] = pLoc[ix4 + 2U];  //ymax
        }

        /* decode box */
        DecCenterX = (Loc[0] * Var[0] * PriorWidth) + PriorCenterX;
        DecCenterY = (Loc[1] * Var[1] * PriorHeight) + PriorCenterY;
        AmbaWrap_pow(EXP, (DOUBLE)Loc[2] * (DOUBLE)Var[2], &PowResult);
        DecWidth   = PriorWidth * (FLOAT) PowResult;
        AmbaWrap_pow(EXP, (DOUBLE)Loc[3] * (DOUBLE)Var[3], &PowResult);
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

static void apply_nms(UINT32 TargetClass, NMS_hndlr_s *NMS_hndlr, FLOAT *pDecodeBox, const FLOAT *pPriorBox, const FLOAT *pLoc, const FLOAT *pConf,
  FLOAT ConfThrd, FLOAT NmsThrd, UINT32 TopK, UINT32 NmsTopK, UINT32 PriorBoxNum, UINT32 ClassNum, UINT32 ModelType)
{
    SORT_BOX_s SwapTmp = {0};
    UINT32 SortIdx, DecIdx, MinConfIdx;
    UINT32 i, j, Keep;
    FLOAT Overlap = 1.0f, MinConf;

    /* copy valid data to G_SortBox */
    SortIdx = 0U;
    (void) AmbaWrap_memset(NMS_hndlr->G_SortBox, 0x0, sizeof(SORT_BOX_s)*MAX_TOPK_NUM);
    for (i = 0U; i < PriorBoxNum; i++) {
        if (pConf[(i * ClassNum) + TargetClass] >= ConfThrd) {
            if (SortIdx < TopK) {
                NMS_hndlr->G_SortBox[SortIdx].Score = pConf[(i * ClassNum) + TargetClass];   // conf
                NMS_hndlr->G_SortBox[SortIdx].DecBoxIdx = i;   // PriorBox_index
                SortIdx++;
            } else {
                // find minimum conf and replace it
                MinConf = pConf[(i * ClassNum) + TargetClass];
                MinConfIdx = TopK;
                for (j = 0; j < TopK; j ++ ) {
                    if (NMS_hndlr->G_SortBox[j].Score < MinConf) {
                        MinConf = NMS_hndlr->G_SortBox[j].Score;
                        MinConfIdx = j;
                    }
                }
                if (MinConfIdx != TopK) { // find
                    NMS_hndlr->G_SortBox[MinConfIdx].Score = pConf[(i * ClassNum) + TargetClass];
                    NMS_hndlr->G_SortBox[MinConfIdx].DecBoxIdx = i;
                }
            }
        }
    }

    /* bubble sort */
    for (i = 0U; i < SortIdx; i++) {
        for (j = 1U; j < (SortIdx - i); j++) {
            if (NMS_hndlr->G_SortBox[j].Score > NMS_hndlr->G_SortBox[j - 1U].Score) {
                // swap j and j-1
                (void) AmbaWrap_memcpy(&SwapTmp, &NMS_hndlr->G_SortBox[j], sizeof(SORT_BOX_s));
                (void) AmbaWrap_memcpy(&NMS_hndlr->G_SortBox[j], &NMS_hndlr->G_SortBox[j - 1U], sizeof(SORT_BOX_s));
                (void) AmbaWrap_memcpy(&NMS_hndlr->G_SortBox[j - 1U], &SwapTmp, sizeof(SORT_BOX_s));
            }
        }
    }

    /* nms */
    for (i = 0U; i < SortIdx; i++) {
        // decode bbox before doing nms
        decode_bbox(NMS_hndlr->G_SortBox[i].DecBoxIdx, PriorBoxNum, pPriorBox, pLoc, pDecodeBox, ModelType);
        Keep = 1U;
        DecIdx = NMS_hndlr->G_SortBox[i].DecBoxIdx * 4U;
        for (j = 0; j < NMS_hndlr->G_NmsBoxNum; j++) {
            if (NMS_hndlr->G_NmsBox[j].Class == TargetClass) {
                if (Keep == 1U) {
                    Overlap = jaccard_overlap(NMS_hndlr->G_NmsBox[j].Xmin, NMS_hndlr->G_NmsBox[j].Ymin, NMS_hndlr->G_NmsBox[j].Xmax, NMS_hndlr->G_NmsBox[j].Ymax,
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
            if (NMS_hndlr->G_NmsBoxNum < NmsTopK) {
                NMS_hndlr->G_NmsBox[NMS_hndlr->G_NmsBoxNum].Class = TargetClass;
                NMS_hndlr->G_NmsBox[NMS_hndlr->G_NmsBoxNum].Score = NMS_hndlr->G_SortBox[i].Score;
                NMS_hndlr->G_NmsBox[NMS_hndlr->G_NmsBoxNum].Xmin = pDecodeBox[DecIdx];
                NMS_hndlr->G_NmsBox[NMS_hndlr->G_NmsBoxNum].Ymin = pDecodeBox[DecIdx + 1U];
                NMS_hndlr->G_NmsBox[NMS_hndlr->G_NmsBoxNum].Xmax = pDecodeBox[DecIdx + 2U];
                NMS_hndlr->G_NmsBox[NMS_hndlr->G_NmsBoxNum].Ymax = pDecodeBox[DecIdx + 3U];
                NMS_hndlr->G_NmsBoxNum ++;
            } else {
                // find minimum Score and replace it
                MinConf = NMS_hndlr->G_SortBox[i].Score;
                MinConfIdx = NmsTopK;
                for (j = 0; j < NmsTopK; j ++ ) {
                    if (NMS_hndlr->G_NmsBox[j].Score < MinConf) {
                        MinConf = NMS_hndlr->G_NmsBox[j].Score;
                        MinConfIdx = j;
                    }
                }
                if (MinConfIdx != NmsTopK) {  // find
                    NMS_hndlr->G_NmsBox[MinConfIdx].Class = TargetClass;
                    NMS_hndlr->G_NmsBox[MinConfIdx].Score = NMS_hndlr->G_SortBox[i].Score;
                    NMS_hndlr->G_NmsBox[MinConfIdx].Xmin = pDecodeBox[DecIdx];
                    NMS_hndlr->G_NmsBox[MinConfIdx].Ymin = pDecodeBox[DecIdx + 1U];
                    NMS_hndlr->G_NmsBox[MinConfIdx].Xmax = pDecodeBox[DecIdx + 2U];
                    NMS_hndlr->G_NmsBox[MinConfIdx].Ymax = pDecodeBox[DecIdx + 3U];
                }
            }
        }
    }
}

static void clean_decodebox(FLOAT *DecodeBox)
{
    (void) AmbaWrap_memset(DecodeBox, 0x0, sizeof(FLOAT) * MAX_PRIORBOX_NUM * 4U);
}

static void clean_nmsbox(NMS_hndlr_s *NMS_hndlr)
{
    NMS_hndlr->G_NmsBoxNum = 0;
    (void) AmbaWrap_memset(NMS_hndlr->G_NmsBox, 0x0, sizeof(NMS_BOX_s) * MAX_TOPK_NUM);
}

static void clean_visbox(NMS_hndlr_s *NMS_hndlr)
{
    NMS_hndlr->ODBoxNum = 0;
    (void) AmbaWrap_memset(NMS_hndlr->ODCandidates, 0x0, sizeof(amba_od_candidate_t)*MAX_VIS_NUM);
}

static NMS_OUTPUT_s ArmSsdFusion_NmsCalc(NMS_hndlr_s *NMS_hndlr, NMS_INPUT_s NmsInput, NMS_CFG_s NmsCfg)
{
    UINT32 RetVal = 0U, i = 0U;
    NMS_OUTPUT_s NmsOutput = {0};

    if ((NmsInput.pPriorbox == NULL) || (NmsInput.pLoc == NULL) || (NmsInput.pConf == NULL)) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_NmsCalc(): input is null", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_NmsCalc(): input is null", 0U, 0U);
        RetVal = 1;
    }
    if (NmsCfg.PriorBoxNum > MAX_PRIORBOX_NUM) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_NmsCalc(): PriorBoxNum(%u) exceed max priorbox num(%u)", NmsCfg.PriorBoxNum, MAX_PRIORBOX_NUM, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_NmsCalc(): PriorBoxNum (%u) exceed max priorbox num (%u)", NmsCfg.PriorBoxNum, MAX_PRIORBOX_NUM);
        RetVal = 1;
    }
    if ((NmsCfg.TopK > MAX_TOPK_NUM) || (NmsCfg.NmsTopK > MAX_TOPK_NUM)) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_NmsCalc(): TopK(%u) or NmsTopK(%u) exceed max num(%u,%u)", NmsCfg.TopK, NmsCfg.NmsTopK, MAX_TOPK_NUM, MAX_TOPK_NUM, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_NmsCalc(): TopK (%d) or NmsTopK (%d) exceed max num", NmsCfg.TopK, NmsCfg.NmsTopK);
        RetVal = 1;
    }
    if ((NmsCfg.ConfThrd <= 0.0f) || (NmsCfg.NmsThrd <= 0.0f)) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_NmsCalc(): threshold <= 0", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_NmsCalc(): threshold <= 0", 0U, 0U);
        RetVal = 1;
    }
    if (NmsCfg.ModelType >= 2) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_NmsCalc(): ModelType is invalid", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_NmsCalc(): ModelType (%d) is invalid", NmsCfg.ModelType, 0U);
        RetVal = 1;
    }
    if (0U == RetVal) {
        /* 1. Clear Decode box and NMS box*/
        clean_decodebox(NMS_hndlr->G_DecodeBox);
        clean_nmsbox(NMS_hndlr);

        /* 2. Find suitable box for each class (NMS) */
        for (i = 0U; i < NmsCfg.ClassNum; i++) {
            if (i == 0U) { // skip back_ground class
                continue;
            }
            apply_nms(i, NMS_hndlr, NMS_hndlr->G_DecodeBox, NmsInput.pPriorbox, NmsInput.pLoc, NmsInput.pConf, NmsCfg.ConfThrd, NmsCfg.NmsThrd,
                        NmsCfg.TopK, NmsCfg.NmsTopK, NmsCfg.PriorBoxNum, NmsCfg.ClassNum, NmsCfg.ModelType);
        }

        NmsOutput.pNmsBox = NMS_hndlr->G_NmsBox;
        NmsOutput.NmsBoxNum = NMS_hndlr->G_NmsBoxNum;
    }

    return NmsOutput;
}

static UINT32 ArmSsdFusion_VisCalc(NMS_hndlr_s *NMS_hndlr, NMS_OUTPUT_s NmsOutput, VIS_CFG_s VisCfg)
{
    UINT32 RetVal = 0, i = 0;
    FLOAT RoiXmin, RoiXmax, RoiYmin, RoiYmax, Tmp_fp32;
    amba_od_candidate_t *ODCandidates;

    /*
    ArmLog_DBG(ARM_LOG_SSD, "ROI (%u x %x)", VisCfg.RoiWidth, VisCfg.RoiHeight);
    ArmLog_DBG(ARM_LOG_SSD, "NET (%u x %x)", VisCfg.NetorkWidth, VisCfg.NetorkHeight);
    ArmLog_DBG(ARM_LOG_SSD, "VIS (%u x %x)", VisCfg.VisWidth, VisCfg.VisHeight);
    */
    if (NmsOutput.pNmsBox == NULL) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_VisCalc(): NmsOutput param is null", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_VisCalc(): NmsOutput param is null", 0U, 0U);
        RetVal = 1;
    }
    if ((VisCfg.RoiWidth == 0U) || (VisCfg.RoiHeight == 0U) || (VisCfg.NetorkWidth == 0U) || (VisCfg.NetorkHeight == 0U) ||
        (VisCfg.VisWidth == 0U) || (VisCfg.VisHeight == 0U)) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_VisCalc(): window size should not be 0", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_VisCalc(): window size should not be 0", 0U, 0U);
        RetVal = 1;
    }
    if (((VisCfg.RoiStartX + VisCfg.NetorkWidth) > VisCfg.RoiWidth) || ((VisCfg.RoiStartY + VisCfg.NetorkHeight) > VisCfg.RoiHeight)) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_VisCalc(): Network window exceed ROI window", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_VisCalc(): Network window exceed ROI window", 0U, 0U);
        RetVal = 1;
    }
    if (VisCfg.VisThrd <= 0.0f) {
        AmbaPrint_PrintUInt5("## ArmSsdFusion_VisCalc(): threshold <= 0", 0U, 0U, 0U, 0U, 0U);
        //ArmLog_ERR(ARM_LOG_SSD, "## ArmSsdFusion_VisCalc(): threshold <= 0", 0U, 0U);
        RetVal = 1;
    }

    if (RetVal == 0) {
        clean_visbox(NMS_hndlr);
        ODCandidates = NMS_hndlr->ODCandidates;

        for (i = 0; i < NmsOutput.NmsBoxNum; i ++) {
            if (NmsOutput.pNmsBox[i].Score > VisCfg.VisThrd) {
                RoiXmin = (MIN(NmsOutput.pNmsBox[i].Xmin, NmsOutput.pNmsBox[i].Xmax) * (FLOAT) VisCfg.NetorkWidth) + (FLOAT) VisCfg.RoiStartX;
                RoiXmax = (MAX(NmsOutput.pNmsBox[i].Xmin, NmsOutput.pNmsBox[i].Xmax) * (FLOAT) VisCfg.NetorkWidth) + (FLOAT) VisCfg.RoiStartX;
                RoiYmin = (MIN(NmsOutput.pNmsBox[i].Ymin, NmsOutput.pNmsBox[i].Ymax) * (FLOAT) VisCfg.NetorkHeight) + (FLOAT) VisCfg.RoiStartY;
                RoiYmax = (MAX(NmsOutput.pNmsBox[i].Ymin, NmsOutput.pNmsBox[i].Ymax) * (FLOAT) VisCfg.NetorkHeight) + (FLOAT) VisCfg.RoiStartY;

                ODCandidates[NMS_hndlr->ODBoxNum].cls = NmsOutput.pNmsBox[i].Class;
                ODCandidates[NMS_hndlr->ODBoxNum].score = 255 * NmsOutput.pNmsBox[i].Score; //map score into 0~255 for amba_od_candidate_t

                /*  X = RoiXmin * VisWidth  / RoiWidth
                *  Y = RoiYmin * VisHeight / RoiHeight
                *  W = (RoiXmax - RoiXmin) * VisWidth  / RoiWidth
                *  H = (RoiYmax - RoiYmin) * VisHeight / RoiHeight
                */
                Tmp_fp32 = RoiXmin * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
                ODCandidates[NMS_hndlr->ODBoxNum].bb_start_col = (UINT32)Tmp_fp32;
                Tmp_fp32 = RoiYmin * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;
                ODCandidates[NMS_hndlr->ODBoxNum].bb_start_row = (UINT32)Tmp_fp32;
                Tmp_fp32 = (RoiXmax - RoiXmin) * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
                ODCandidates[NMS_hndlr->ODBoxNum].bb_width_m1 = (UINT32)Tmp_fp32 - 1;
                Tmp_fp32 = (RoiYmax - RoiYmin) * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;;
                ODCandidates[NMS_hndlr->ODBoxNum].bb_height_m1 = (UINT32)Tmp_fp32 - 1;
                NMS_hndlr->ODBoxNum ++;
            }
        }

        /* Add Net ROI */
        if (VisCfg.ShowDetRegion == 1U) {
            RoiXmin = (FLOAT) VisCfg.RoiStartX;
            RoiXmax = (FLOAT) VisCfg.NetorkWidth + (FLOAT) VisCfg.RoiStartX;
            RoiYmin = (FLOAT) VisCfg.RoiStartY;
            RoiYmax = (FLOAT) VisCfg.NetorkHeight + (FLOAT) VisCfg.RoiStartY;

            ODCandidates[NMS_hndlr->ODBoxNum].cls = 255;
            ODCandidates[NMS_hndlr->ODBoxNum].score = 255; //1.0f;
            Tmp_fp32 = RoiXmin * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
            ODCandidates[NMS_hndlr->ODBoxNum].bb_start_col = (UINT32)Tmp_fp32;
            Tmp_fp32 = RoiYmin * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;
            ODCandidates[NMS_hndlr->ODBoxNum].bb_start_row = (UINT32)Tmp_fp32;
            Tmp_fp32 = (RoiXmax - RoiXmin) * (FLOAT) VisCfg.VisWidth / (FLOAT) VisCfg.RoiWidth;
            ODCandidates[NMS_hndlr->ODBoxNum].bb_width_m1 = (UINT32)Tmp_fp32 - 1;
            Tmp_fp32 = (RoiYmax - RoiYmin) * (FLOAT) VisCfg.VisHeight / (FLOAT) VisCfg.RoiHeight;;
            ODCandidates[NMS_hndlr->ODBoxNum].bb_height_m1 = (UINT32)Tmp_fp32 - 1;
            NMS_hndlr->ODBoxNum ++;
        }
    }

    return 0;
}

uint32_t NMS_Process(uint32_t ch, void *hndlr, float *pLoc, float *pConf, amba_od_candidate_t *pODOutput, uint32_t *BoxAmount)
{
    uint32_t Rval = 0U;
    NMS_INPUT_s NmsInput;
    NMS_CFG_s NmsCfg = {0};
    NMS_OUTPUT_s NmsOutput = {0};
    VIS_CFG_s VisCfg = {0};
    NMS_hndlr_s *NMS_hndlr;

    NMS_hndlr = (NMS_hndlr_s *)hndlr;
    if (NMS_hndlr == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid hndlr!!",0,0,0,0,0);
        return 1;
    }
    if (NMS_hndlr->g_prior_box == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid prior_box!!",0,0,0,0,0);
        return 1;
    }
    if (pODOutput == NULL) {
        AmbaPrint_PrintUInt5("NMS_Process: invalid pODOutput!!",0,0,0,0,0);
        return 1;
    }
    NMS_hndlr->ODCandidates = pODOutput;

    NmsInput.pPriorbox  = NMS_hndlr->g_prior_box;
    NmsInput.pLoc       = pLoc; //OutBuf.buf[0].pBuffer;
    NmsInput.pConf      = pConf;//OutBuf.buf[1].pBuffer;

    /*{
        UINT32 Addr;
        AmbaMisra_TypeCast32(&Addr, &OutBuf.buf[0].pBuffer);
        AmbaPrint_PrintUInt5("OpenOD_GetResult: OutBuf.buf[0].pBuffer 0x%08X", Addr, 0U, 0U, 0U, 0U);
        AmbaMisra_TypeCast32(&Addr, &OutBuf.buf[1].pBuffer);
        AmbaPrint_PrintUInt5("OpenOD_GetResult: OutBuf.buf[1].pBuffer 0x%08X", Addr, 0U, 0U, 0U, 0U);
    }*/

    NmsCfg.PriorBoxNum  = NMS_hndlr->total_bbox;
    NmsCfg.ClassNum     = NUM_CLS;
    NmsCfg.TopK         = NUM_TOPK;
    NmsCfg.NmsTopK      = NUM_NMSTOPK;
    NmsCfg.ConfThrd     = CONFTHRD;
    NmsCfg.NmsThrd      = NMSTHRD;
    NmsCfg.ModelType    = MODEL_TYPE_CAFFE;
    NmsOutput = ArmSsdFusion_NmsCalc(NMS_hndlr, NmsInput, NmsCfg);

    AmbaPrint_PrintUInt5("NMS_Process: NmsBoxNum %d", NmsOutput.NmsBoxNum, 0U, 0U, 0U, 0U);

    /* Do Visual calculation */
    {
        amba_roi_config_t RoiCfg = {0};
        WINDOW_INFO_s InputInfo = {0};
        OSD_INFO_s OsdInfo = {0};
        Preference_GetInputInfo(ch, &InputInfo);
        Preference_GetDAGRoiCfg(ch, &RoiCfg);
        Preference_GetOSDInfo(ch, &OsdInfo);

        VisCfg.VisThrd        = 0.4f;
        VisCfg.NetorkWidth    = RoiCfg.roi_width;
        VisCfg.NetorkHeight   = RoiCfg.roi_height;
        VisCfg.RoiWidth       = InputInfo.Width; //input width
        VisCfg.RoiHeight      = InputInfo.Height; //input height
        if (RoiCfg.roi_start_col == 9999) {
            VisCfg.RoiStartX      = (VisCfg.RoiWidth - VisCfg.NetorkWidth)/2; //center
        } else {
            VisCfg.RoiStartX      = RoiCfg.roi_start_col;
        }
        if (RoiCfg.roi_start_row == 9999) {
            VisCfg.RoiStartY      = (VisCfg.RoiHeight - VisCfg.NetorkHeight)/2; //center
        } else {
            VisCfg.RoiStartY      = RoiCfg.roi_start_row;
        }
        VisCfg.VisWidth       = OsdInfo.OsdWin.Width;   // OSD size
        VisCfg.VisHeight      = OsdInfo.OsdWin.Height;
        VisCfg.ShowDetRegion  = 1;
        Rval = ArmSsdFusion_VisCalc(NMS_hndlr, NmsOutput, VisCfg); //result will be set into NMS_hndlr->ODCandidates
    }

    AmbaPrint_PrintUInt5("NMS_Process: VisBoxNum %d", NMS_hndlr->ODBoxNum, 0U, 0U, 0U, 0U);

    *BoxAmount = NMS_hndlr->ODBoxNum;

    return Rval;
}

uint32_t NMS_LoadPriorBox(char *filename, void **hndlr)
{
    uint32_t ret = 0U;
    NMS_hndlr_s *NMS_hndlr;

    if (hndlr == NULL) {
        AmbaPrint_PrintUInt5("invalid hndlr!!\n",0,0,0,0,0);
        return 1;
    }

    NMS_hndlr = (NMS_hndlr_s *)malloc(sizeof(NMS_hndlr_s));
    *hndlr = NMS_hndlr;
    if (NMS_hndlr == NULL) {
        AmbaPrint_PrintUInt5("fail to creat NMS_hndlr!!\n",0,0,0,0,0);
        ret = 1U;
    } else {
        memset(NMS_hndlr, 0, sizeof(NMS_hndlr_s));
    }

    if (ret == 0U) {
        ret = get_file_length(filename, &(NMS_hndlr->pb_len));
        if (ret == 0U) {
            //Use malloc to create priorbox buffer for Linux. This has to change for RTOS case.
            NMS_hndlr->g_prior_box = (float *)malloc(NMS_hndlr->pb_len);
            if (NMS_hndlr->g_prior_box == NULL) {
                AmbaPrint_PrintUInt5("fail to creat g_prior_box!!\n",0,0,0,0,0);
                ret = 2;
            } else {
                load_binary(filename, NMS_hndlr->g_prior_box, NMS_hndlr->pb_len);
                NMS_hndlr->total_bbox = NMS_hndlr->pb_len >> 5U;
            }
        }
    }

    return ret;
}

uint32_t NMS_Release(void *hndlr)
{
    NMS_hndlr_s *NMS_hndlr;

    NMS_hndlr = (NMS_hndlr_s *)hndlr;
    if (NMS_hndlr != NULL) {
        if (NMS_hndlr->g_prior_box != NULL) {
            free(NMS_hndlr->g_prior_box);
        }
        free(NMS_hndlr);
    }

    return 0;
}

