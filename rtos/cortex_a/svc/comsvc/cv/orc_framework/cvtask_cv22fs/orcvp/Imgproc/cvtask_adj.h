
#ifndef _adj__H_FILE
#define _adj__H_FILE

#include "AmbaDSP_ImageFilter.h"


#if 0
#define IK_CE_CORING_TABLE_SIZE     (64U)
#define IK_CE_BOOST_TABLE_SIZE      (64U)
#define IK_CC_3D_SIZE               (17536U)


typedef struct {
    uint32_t Enable;
    uint32_t LumaAvgMethod;

    int32_t LumaAvgWeightR;
    int32_t LumaAvgWeightGr;
    int32_t LumaAvgWeightGb;
    int32_t LumaAvgWeightB;

    uint32_t FirEnable;
    uint32_t FirCoeff[3];
    uint32_t CoringIndexScaleShift;

    uint32_t CoringIndexScaleLow;
    uint32_t CoringIndexScaleLowDelta;
    uint32_t CoringIndexScaleLowStrength;
    uint32_t CoringIndexScaleMidStrength;
    uint32_t CoringIndexScaleHigh;
    uint32_t CoringIndexScaleHighDelta;
    uint32_t CoringIndexScaleHighStrength;

    uint32_t CoringGainShift;
    uint32_t CoringGainLow;
    uint32_t CoringGainLowDelta;
    uint32_t CoringGainLowStrength;
    uint32_t CoringGainMidStrength;
    uint32_t CoringGainHigh;
    uint32_t CoringGainHighDelta;
    uint32_t CoringGainHighStrength;

    uint32_t CoringTable[IK_CE_CORING_TABLE_SIZE];
    //cv2
    uint32_t BoostGainShift;
    int32_t BoostTable[IK_CE_BOOST_TABLE_SIZE];
    uint32_t BoostTableSizeExp;

    uint32_t Radius;
    uint32_t Epsilon;
} AMBA_IK_CE_s;

typedef struct {
    uint32_t MatrixEn;
    uint32_t MatrixShiftMinus8;
    int32_t Matrix[6];
    uint32_t MatrixThreeDTable[IK_CC_3D_SIZE/4U];
} AMBA_IK_COLOR_CORRECTION_s;
#endif

/**
 * Image schdlr exposure info definitions
 */
#define AMBA_IP_VIN_COMPAND_ELEMENT_NUM    ((uint32_t)45)
#define AMBA_IP_VIN_DECOMPAND_ELEMENT_NUM  ((uint32_t)145)

/**
 * Video vin tone curve structure
 */
typedef struct {
    uint32_t DecompandTable[AMBA_IP_VIN_DECOMPAND_ELEMENT_NUM];
    uint16_t CompandTable[AMBA_IP_VIN_COMPAND_ELEMENT_NUM];
    uint8_t  Padding[2];
} AMBA_IP_VIN_TONE_CURVE_s;

typedef struct {
    uint32_t                            ModeUpdate;
    uint32_t                            WbGainUpdate;
    AMBA_IK_WB_GAIN_s                WbGain;

    // STILL_AF_INFO_s                  AfInfoStill;    /* for showing SAF focused areas on GUI after FLOCK */

    uint32_t                            RgbYuvMatrixUpdate;
    AMBA_IK_RGB_TO_YUV_MATRIX_s      RgbYuvMatrix;

    /* CFA domain filters */
    uint32_t                            BlackCorrUpdate;                   //vp_01
    AMBA_IK_STATIC_BLC_LVL_s         BlackCorr;
    uint32_t                            AntiAliasingUpdate;                //vp_02
    AMBA_IK_ANTI_ALIASING_s          AntiAliasing;
    uint32_t                            BadpixCorrUpdate;                  //vp_03
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s    BadpixCorr;

    uint32_t                            CfaFilterUpdate;                   //vp_04
    AMBA_IK_CFA_NOISE_FILTER_s       CfaFilter;

    uint32_t                            RgbIrUpdate;
    AMBA_IK_RGB_IR_s                 RgbIr;

    /* RGB domain filters */
    uint32_t                            DemosaicUpdate;                    //vp_06
    AMBA_IK_DEMOSAIC_s               Demosaic;

    uint32_t                            ColorCorrUpdate;                   //vp_07
    AMBA_IK_COLOR_CORRECTION_s       ColorCorr;

    uint32_t                            GammaUpdate;                       //vp_08
    AMBA_IK_TONE_CURVE_s             GammaTable;


    /* Y domain filters */
    uint32_t                            ChromaScaleUpdate;                 //vp_09
    AMBA_IK_CHROMA_SCALE_s           ChromaScale;
    uint32_t                            ChromaMedianUpdate;                //vp_10
    AMBA_IK_CHROMA_MEDIAN_FILTER_s   ChromaMedian;

    uint32_t                            LisoProcessSelectUpdate;
    AMBA_IK_FIRST_LUMA_PROC_MODE_s   LisoProcessSelect;

    uint32_t                            AsfUpdate;                         //vp_12
    AMBA_IK_ADV_SPAT_FLTR_s          Asf;

    uint32_t                            SharpenBothUpdate;                 //vp_13
    AMBA_IK_FSTSHPNS_BOTH_s          SharpenBoth;
    uint32_t                            SharpenNoiseUpdate;                //vp_14
    AMBA_IK_FSTSHPNS_NOISE_s         SharpenNoise;
    uint32_t                            SharpenFirUpdate;                  //vp_15
    AMBA_IK_FSTSHPNS_FIR_s           SharpenFir;
    uint32_t                            SharpenCoringUpdate;               //vp_16
    AMBA_IK_CORING_s                 SharpenCoring;
    uint32_t                            SharpenCoringIndexScaleUpdate;     //vp_17
    AMBA_IK_LEVEL_s                  SharpenCoringIndexScale;
    uint32_t                            SharpenMinCoringResultUpdate;      //vp_18
    AMBA_IK_LEVEL_s                  SharpenMinCoringResult;
    uint32_t                            SharpenScaleCoringUpdate;          //vp_19
    AMBA_IK_LEVEL_s                  SharpenScaleCoring;
    uint32_t                            SharpenMaxCoringResultUpdate;      //vp_18
    AMBA_IK_LEVEL_s                  SharpenMaxCoringResult;

    uint32_t                            SharpenBBothUpdate;
    AMBA_IK_FNLSHPNS_BOTH_s          SharpenBBoth;
    uint32_t                            SharpenBNoiseUpdate;
    AMBA_IK_FNLSHPNS_NOISE_s         SharpenBNoise;
    uint32_t                            SharpenBFirUpdate;
    AMBA_IK_FNLSHPNS_FIR_s           SharpenBFir;
    uint32_t                            SharpenBCoringUpdate;
    AMBA_IK_CORING_s                 SharpenBCoring;
    uint32_t                            SharpenBCoringIndexScaleUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBCoringIndexScale;
    uint32_t                            SharpenBMinCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBMinCoringResult;
    uint32_t                            SharpenBScaleCoringUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBScaleCoring;
    uint32_t                            SharpenBMaxCoringResultUpdate;
    AMBA_IK_LEVEL_METHOD_s           SharpenBMaxCoringResult;

    uint32_t                            ChromaFilterUpdate;                //vp_27
    AMBA_IK_CHROMA_FILTER_s          ChromaFilter;

    uint32_t                            WideChromaFilterUpdate;                //vp_27
    AMBA_IK_WIDE_CHROMA_FILTER_s     WideChromaFilter;
    uint32_t                                   WideChromaFilterCMBUpdate;
    AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s    WideChromaFilterCMB;

    uint32_t                            GbGrMismatchUpdate;                //vp_28
    AMBA_IK_GRGB_MISMATCH_s          GbGrMismatch;

    uint32_t                            ResamplerStrUpdate;
    AMBA_IK_RESAMPLER_STR_s          ResamplerStr;

    /* Warp and MCTF related filters */
    uint32_t                            MctfInfoUpdate;                    //vp_29
    AMBA_IK_VIDEO_MCTF_s             MctfInfo;
    AMBA_IK_VIDEO_MCTF_TA_s          MctfTemporalAdjust;

    uint32_t                            ColorDeptNRUpdate;
    AMBA_IK_LUMA_NOISE_REDUCTION_s   ColorDeptNR;


    uint32_t                            CEInfoUpdate;
    AMBA_IK_CE_s                     CEInfo;
    uint32_t                            CEInputTableUpdate;
    AMBA_IK_CE_INPUT_TABLE_s         CEInputTable;
    uint32_t                            CEOutputTableUpdate;
    AMBA_IK_CE_OUT_TABLE_s           CEOutputTable;
    uint32_t                            FEToneCurveUpdate;
    AMBA_IK_FE_TONE_CURVE_s          FEToneCurve;
    uint32_t                            HDSToneCurveUpdate;
    AMBA_IP_VIN_TONE_CURVE_s         HDSToneCurve;

    /* Start of Video HDR filter */
//adj control
//hdr control, set by scheduler
    uint32_t                            HdrBlendUpdate;
    AMBA_IK_HDR_BLEND_s              HdrBlend;

    uint32_t                            HdrBlackCorr0Update;
    AMBA_IK_STATIC_BLC_LVL_s         HdrBlackCorr0;
    uint32_t                            HdrBlackCorr1Update;
    AMBA_IK_STATIC_BLC_LVL_s         HdrBlackCorr1;
    uint32_t                            HdrBlackCorr2Update;
    AMBA_IK_STATIC_BLC_LVL_s         HdrBlackCorr2;

    uint32_t                            HdrWbGain0Update;
    AMBA_IK_FE_WB_GAIN_s             HdrWbGain0;
    uint32_t                            HdrWbGain1Update;
    AMBA_IK_FE_WB_GAIN_s             HdrWbGain1;
    uint32_t                            HdrWbGain2Update;
    AMBA_IK_FE_WB_GAIN_s             HdrWbGain2;

} PIPELINE_CONTROL_s;

typedef struct  {
    AMBA_IK_CE_s    CEInfo;
} ADJ_FLEXDIAG_GROUP0_s;

typedef struct  {
    AMBA_IK_CE_s                    CEInfo;
    AMBA_IK_COLOR_CORRECTION_s      CCTable;
} ADJ_FLEXDIAG_GROUP1_s;

typedef struct {
    uint32_t  PreFilterAdd;
    uint32_t  FilterAdd;
    uint32_t  NextFilterAdd;
    uint32_t  Size;
} Adj_Filter_Info_t;
#endif
