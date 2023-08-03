
#ifndef _imgproc__H_FILE
#define _imgproc__H_FILE

#define MAX_NUM_TILES   4096
#define MAX_WHITE_REGION_X  20

// @ZZ: local variables with 0..19 indices, used in part (c) and (d)
typedef struct {
    uint64_t RGainSum[MAX_WHITE_REGION_X];
    uint64_t BGainSum[MAX_WHITE_REGION_X];
    uint32_t WeightSum[MAX_WHITE_REGION_X];
    uint32_t WhiteNum[MAX_WHITE_REGION_X];
} local_cd_t;

/**
 * @ZZ: This is the main data structure we put into stack space in _run().
 * maximum size is 128KB minus other local variables
 */
typedef struct {
    uint16_t AwbTV_RGain[MAX_NUM_TILES];
    uint16_t AwbTV_BGain[MAX_NUM_TILES];
    uint8_t  Awb_Y[MAX_NUM_TILES];
    uint8_t  White_Tbl[2][121][121];
    uint8_t  Pading0[2];
    // Gains for INDOOR/OUTDOOR/HILIGHT
    uint32_t GainR[3];
    uint32_t GainB[3];
    // final gain result after interpolation
    uint32_t GainR_Res;
    uint32_t GainB_Res;
    // comparison result
    uint32_t Cmp_Result;

    uint32_t temp0[128];
    uint32_t temp1[128];
    uint32_t temp2[128];
    uint32_t temp[2][4392];
} awb_sim_scb_t;


/**
 * @ZZ: This is the structure we put into CMEM space. It is persists between
 *   all cvtask functions.
 * Maximum size = 4KB
 */
#define LUMA_WEIGHT_DIM 64
#define CMEM_CACHE_LINE 512

enum { INDOOR=0, OUTDOOR, HILIGHT, BLENDING_INDOOR_OUTDOOR, BLENDING_OUTDOOR_HILIGHT, USE_DEFAULT };
typedef struct {
    uint8_t xmin, xmax, ymin, ymax;
} white_region_geo_t;
typedef struct {
    /*** 64 +4 bytes ***/
    uint32_t frame_index;
    uint32_t num_of_tiles;
    uint32_t RGain_To_Cmp;
    uint32_t BGain_To_Cmp;
    uint32_t RGain_Default;
    uint32_t BGain_Default;
    uint32_t ROI_Weight_da;   // DRAM base address of ROI_Weight table
    uint32_t CFA_AWB_da;      // DRAM base address of packed CFA_AWB stats
    uint16_t AwbRgbShift;
    uint16_t AwbTileWidth;
    uint16_t AwbTileHeight;
    uint16_t mode_w_blending;  // deduce from the following EV values
    uint16_t  ev_current;
    uint16_t  ev_double;
    uint16_t  ev_outdoor;
    uint16_t  ev_hilight;
    white_region_geo_t box[3];  // 3 is for INDOOR/OUTDOOR/HILIGHT
//---------------------------------------------------------
    uint32_t RGRatio[2];
    uint32_t BGRatio[2];
    uint32_t CurGainR;
    uint32_t CurGainG;
    uint32_t CurGainB;
    int16_t  Speed;
    uint8_t  Padding1[2];
    uint32_t cal_set_gainR[2];
    int32_t  cal_deltaR[2];
    uint32_t cal_set_gainB[2];
    int32_t  cal_deltaB[2];
    uint32_t dbg_prt;
    uint32_t CfaAWBRatio;
//---------------------------------------------
    uint32_t rsvd_1[3];
    /*** 64 bytes ***/
    uint8_t  luma_weight[LUMA_WEIGHT_DIM];
    uint8_t  table_weight[MAX_WHITE_REGION_X];

    /*** AE ***/
    float     PreShutterTime;
    float     PreAgcGain;
    int32_t   PreDgain;
    float     CurShutterTime;
    float     CurAgcGain;
    int32_t   CurDgain;
    uint16_t  CurLumaStat;
    int16_t   CurLimitStatus;
    uint16_t  CurTarget;
    int8_t    AeResult;
    uint8_t   Padding3;
    /////////////////////////////////
    uint8_t  cache[2][CMEM_CACHE_LINE];
    uint8_t  cache_pre[CMEM_CACHE_LINE];
    uint8_t  cache_cal[CMEM_CACHE_LINE];
} awb_sim_ccb_t;



typedef struct {
    int32_t Result;
} AMBA_AE_FlEXIDAG_RESULT_t;

typedef struct {
    int32_t Result;
} AMBA_AWB_FlEXIDAG_RESULT_t;

typedef struct {
    int32_t Result;
} AMBA_ADJ_FlEXIDAG_RESULT_t;

typedef struct {
    AMBA_AE_FlEXIDAG_RESULT_t AeResult;
    AMBA_AWB_FlEXIDAG_RESULT_t AwbResult;
    AMBA_ADJ_FlEXIDAG_RESULT_t AdjResult;
} AMBA_IMGPROC_FlEXIDAG_RESULT_t;


typedef struct  {
    int32_t    min_gr;
    int32_t    max_gr;
    int32_t    min_gb;
    int32_t    max_gb;
    uint8_t    patch_no[32][32];
} AWB_PATCH_INFO_s;//for external use

typedef struct  {
    int32_t    min_gr;
    int32_t    max_gr;
    int32_t    min_gb;
    int32_t    max_gb;
    uint8_t    patch_no[14641];
    uint8_t    padding0[3];
} AWB_PATCH_INFO_FULL_s;//for external use

typedef struct {
    uint32_t  Ae;
    uint32_t  Awb;
    uint32_t  Adj;
} AMBA_AAA_ASIL_OP_INFO_t;

typedef struct {
    uint32_t  FrameIndex;
    uint16_t  SumRGB[12288];
    uint8_t   ROIWeight[4096];
    uint8_t   LumaWeight[64];
    uint8_t   TableWeight[20];
    AMBA_IK_WB_GAIN_s GoldenCalibration[2];
    AMBA_IK_WB_GAIN_s CurrentCalibration [2];
    uint32_t  HighLightEvIndex; // Specifies the lowest Luma value (LV) of a high light environment.
    uint32_t  OutdoorEvIndex;   // Specifies the lowest LV of an outdoor environment.
    uint32_t  EvIndex;
    uint32_t  DoubleInc;
    // uint8_t  padding0;
    AMBA_IK_WB_GAIN_s FineRatio;
    uint16_t  AwbRgbShift;
    uint16_t  AwbTileWidth;
    uint16_t  AwbTileHeight;
    uint8_t  Padding0[2];
    // AWB_CONTROL_s AwbCtrl;
    uint32_t NumOfTiles;
    uint32_t RGainToCmp;
    uint32_t BGainToCmp;
    uint32_t RGainDefault;
    uint32_t BGainDefault;
    uint32_t RGRatio[2];
    uint32_t BGRatio[2];
    AMBA_IK_WB_GAIN_s CurGain;
    int16_t Speed;
    uint8_t  Padding1[2];
    uint32_t Cal_Set_GainR[2];
    int32_t  Cal_DeltaR[2];
    uint32_t Cal_Set_GainB[2];
    int32_t  Cal_DeltaB[2];
    AWB_PATCH_INFO_FULL_s IndoorWRFULL;
    AWB_PATCH_INFO_FULL_s OutdoorWRFULL;
    AWB_PATCH_INFO_FULL_s HighlightWRFULL;
    uint8_t  DbgPrt;
    uint8_t  Padding2[3];
    uint32_t CfaAWBRatio;
//AE
    float     CurShutterTime;
    float     CurAgcGain;
    int32_t   CurDgain;
    uint16_t  CurLumaStat;
    int16_t   CurLimitStatus;
    uint16_t  CurTarget;
    uint8_t   Padding3[2];
    float     PreShutterTime;
    float     PreAgcGain;
    int32_t   PreDgain;
//ADJ
    PIPELINE_CONTROL_s *pPipe0;
    PIPELINE_CONTROL_s *pPipe1;
    PIPELINE_CONTROL_s *pPipe2;
    AMBA_AAA_ASIL_OP_INFO_t OP;
} AMBA_AAA_Flexidag_INFO_t;


#endif
