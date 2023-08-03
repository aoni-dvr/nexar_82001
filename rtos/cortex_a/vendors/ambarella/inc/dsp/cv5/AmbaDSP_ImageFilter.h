#ifndef AMBA_IK_IMAGE_FILTER_H
#define AMBA_IK_IMAGE_FILTER_H

#include "AmbaTypes.h"

#ifndef IK_NUM_CORING_TABLE_INDEX
    #define IK_NUM_CORING_TABLE_INDEX   (256U)
#endif
#ifndef IK_NUM_CHROMA_GAIN_CURVE
    #define IK_NUM_CHROMA_GAIN_CURVE    (128U)
#endif
#ifndef IK_NUM_EXPOSURE_CURVE
    #define IK_NUM_EXPOSURE_CURVE       (256U)
#endif
#ifndef IK_NUM_TONE_CURVE
    #define IK_NUM_TONE_CURVE           (256U)
#endif
#ifndef IK_CC_3D_SIZE
    #define IK_CC_3D_SIZE               (17536U)
#endif
#ifndef IK_CE_INPUT_TABLE_SIZE
    #define IK_CE_INPUT_TABLE_SIZE      (449U)
#endif
#ifndef IK_CE_OUTPUT_TABLE_SIZE
    #define IK_CE_OUTPUT_TABLE_SIZE     (256U)
#endif
#ifndef IK_CE_CORING_TABLE_SIZE
    #define IK_CE_CORING_TABLE_SIZE     (64U)
#endif
#ifndef IK_CE_BOOST_TABLE_SIZE
    #define IK_CE_BOOST_TABLE_SIZE      (64U)
#endif
#ifndef IK_NUM_DECOMPAND_TABLE
    #define IK_NUM_DECOMPAND_TABLE      (145U)
#endif
#ifndef IK_NUM_COMPAND_TABLE
    #define IK_NUM_COMPAND_TABLE        (45U)
#endif
#ifndef IK_NUM_THREE_D_TABLE
    #define IK_NUM_THREE_D_TABLE        (8192U)
#endif
#ifndef IK_MAX_HDR_EXPOSURE_NUM
    #define IK_MAX_HDR_EXPOSURE_NUM     (3U)
#endif

#ifndef IK_SBP_VER
    #define IK_SBP_VER                  (0x20180401U)
#endif
#ifndef IK_VIG_VER
    #define IK_VIG_VER                  (0x20180401U)
#endif
#ifndef IK_CA_VER
    #define IK_CA_VER                   (0x20180401U)
#endif
#ifndef IK_WARP_VER
    #define IK_WARP_VER                 (0x20180401U)
#endif
#ifndef IK_WARP_DISABLE
    #define IK_WARP_DISABLE             (0)
#endif
#ifndef IK_WARP_ENABLE
    #define IK_WARP_ENABLE              (1)
#endif
#ifndef IK_WARP_VIDEO_MAXHORGRID
    #define IK_WARP_VIDEO_MAXHORGRID    (128U)
#endif
#ifndef IK_WARP_VIDEO_MAXVERGRID
    #define IK_WARP_VIDEO_MAXVERGRID    (96U)
#endif
#ifndef IK_WARP_MAX_SLICE_NUMBER
    #define IK_WARP_MAX_SLICE_NUMBER    (12U)
#endif

#ifndef IK_FLIP_DISABLED
    #define IK_FLIP_DISABLED            (0x0UL)
#endif
#ifndef IK_HORIZONTAL_FLIP
    #define IK_HORIZONTAL_FLIP          (0x1UL)
#endif
#ifndef IK_VERTICAL_FLIP
    #define IK_VERTICAL_FLIP            (0x2UL)
#endif
#ifndef IK_FLIP_RAW_V
    #define IK_FLIP_RAW_V               (IK_VERTICAL_FLIP)
#endif
#ifndef IK_FLIP_YUV_H
    #define IK_FLIP_YUV_H               (IK_HORIZONTAL_FLIP<<16U)
#endif
#ifndef IK_FLIP_RAW_V_YUV_H
    #define IK_FLIP_RAW_V_YUV_H         ((IK_VERTICAL_FLIP) + (IK_HORIZONTAL_FLIP<<16U))
#endif

#ifndef IK_RAW_COMPRESS_OFF
    #define IK_RAW_COMPRESS_OFF     0U
    #define IK_RAW_COMPRESS_6P75    1U
    #define IK_RAW_COMPRESS_7p5     2U
    #define IK_RAW_COMPRESS_7p75    3U
    #define IK_RAW_COMPRESS_8p5     4U
    #define IK_RAW_COMPRESS_8p75    5U
    #define IK_RAW_COMPRESS_9p5     6U
    #define IK_RAW_COMPRESS_9p75    7U
    #define IK_RAW_COMPRESS_10p5    8U
    #define IK_RAW_COMPACT_8B       256U
    #define IK_RAW_COMPACT_10B      257U
    #define IK_RAW_COMPACT_12B      258U
    #define IK_RAW_COMPACT_14B      259U
#endif

#define AMBA_IK_YUV420         (0x0U)
#define AMBA_IK_YUV422         (0x1U)


typedef struct {
    UINT32 ContextId;
}AMBA_IK_MODE_CFG_s;

typedef struct {
    UINT32 VinId;
    UINT32 SensorResolution;    /* Number of bits for data representation */
    UINT32 SensorPattern;       /* Bayer patterns RG, BG, GR, GB */
    UINT32 SensorMode;          /*0: normal 1:RGB-IR 2:RCCC*/
    UINT32 Compression;
    INT32 CompressionOffset;
    UINT32 SensorReadoutMode;
} AMBA_IK_VIN_SENSOR_INFO_s;

typedef struct {
    UINT32 GainR;
    UINT32 GainG;
    UINT32 GainB;
} AMBA_IK_WB_GAIN_s;

typedef struct {
    UINT32 IrShift;
    UINT32 IrToneCurve[IK_NUM_TONE_CURVE];
} AMBA_IK_STORED_IR_s;

typedef struct {
    UINT32 Enable;
    INT32 AlphaRr;
    INT32 AlphaRb;
    INT32 AlphaBr;
    INT32 AlphaBb;
    UINT32 SaturationLevel;
} AMBA_IK_CFA_LEAKAGE_FILTER_s;

typedef struct {
    UINT32 Enable;
    UINT32 Thresh;
    UINT32 LogFractionalCorrect;
} AMBA_IK_ANTI_ALIASING_s;

typedef struct {
    UINT32 Enable;
    UINT32 HotPixelStrength;
    UINT32 DarkPixelStrength;
    UINT32 CorrectionMethod;
    UINT32 HotPixelStrengthIRModeRedBlue;
    UINT32 DarkPixelStrengthIRModeRedBlue;
} AMBA_IK_DYNAMIC_BAD_PXL_COR_s;

typedef struct {
    UINT32 Enable;
    UINT32 MinNoiseLevel[3];
    UINT32 MaxNoiseLevel[3];
    UINT32 OriginalBlendStr[3];    /* R/G/B, 0-256 */
    UINT32 ExtentRegular[3];       /* R/G/B, 0-256 */
    UINT32 ExtentFine[3];          /* R/G/B, 0-256 */
    UINT32 StrengthFine[3];        /* R/G/B, 0-256 */
    UINT32 SelectivityRegular;     /* 0-256 */
    UINT32 SelectivityFine;        /* 0-256 */
    UINT32 DirectionalEnable;
    UINT32 DirectionalHorvertEdgeStrength;
    UINT32 DirectionalHorvertStrengthBias;
    UINT32 DirectionalHorvertOriginalBlendStrength;
    UINT32 DirLevMax[3];
    UINT32 DirLevMin[3];
    UINT32 DirLevMul[3];
    INT32  DirLevOffset[3];
    UINT32 DirLevShift[3];
    UINT32 LevMul[3];
    INT32  LevOffset[3];
    UINT32 LevShift[3];
    UINT32 ApplyToColorDiffRed;
    UINT32 ApplyToColorDiffBlue;
    UINT32 OptForRccbRed;
    UINT32 OptForRccbBlue;
} AMBA_IK_CFA_NOISE_FILTER_s;

typedef struct {
    UINT32 Enable;
    UINT32 ActivityThresh;
    UINT32 ActivityDifferenceThresh;
    UINT32 GradClipThresh;
    UINT32 GradNoiseThresh;
    UINT32 AliasInterpolationStrength;
    UINT32 AliasInterpolationThresh;
} AMBA_IK_DEMOSAIC_s;

typedef struct {
    UINT32 MatrixThreeDTable[IK_CC_3D_SIZE/4U];
    UINT32 MatrixEn;
    UINT32 MatrixShiftMinus8;
    INT32 Matrix[6];
} AMBA_IK_COLOR_CORRECTION_s;

typedef struct {
    UINT32 ToneCurveRed[IK_NUM_TONE_CURVE];
    UINT32 ToneCurveGreen[IK_NUM_TONE_CURVE];
    UINT32 ToneCurveBlue[IK_NUM_TONE_CURVE];
} AMBA_IK_TONE_CURVE_s;

typedef struct {
    INT32 MatrixValues[9];
    INT32 YOffset;
    INT32 UOffset;
    INT32 VOffset;
} AMBA_IK_RGB_TO_YUV_MATRIX_s;

typedef struct {
    UINT32 Enable;
    INT32 MatrixValues[3];
    INT32 YOffset;
} AMBA_IK_RGB_TO_12Y_s;

typedef struct {
    UINT32 Mode;
    INT32 IrcorrectOffsetB;
    INT32 IrcorrectOffsetGb;
    INT32 IrcorrectOffsetGr;
    INT32 IrcorrectOffsetR;
    UINT32 MulBaseVal;
    UINT32 MulDeltaHigh;
    UINT32 MulHigh;
    UINT32 MulHighVal;
    UINT32 IrOnly;
} AMBA_IK_RGB_IR_s;

typedef struct {
    UINT32 Enable;
    UINT32 GainCurve[IK_NUM_CHROMA_GAIN_CURVE];
} AMBA_IK_CHROMA_SCALE_s;

typedef struct {
    UINT32 Enable;
    UINT32 CbAdaptiveStrength;
    UINT32 CrAdaptiveStrength;
    UINT32 CbNonAdaptiveStrength;
    UINT32 CrNonAdaptiveStrength;
    UINT32 CbAdaptiveAmount;
    UINT32 CrAdaptiveAmount;
} AMBA_IK_CHROMA_MEDIAN_FILTER_s;

typedef struct {
    UINT32 UseSharpenNotAsf;
} AMBA_IK_FIRST_LUMA_PROC_MODE_s;

typedef struct {
    UINT32 TemporalAlpha0; // 0-255
    UINT32 TemporalAlpha1; // 0-255
    UINT32 TemporalAlpha2; // 0-255
    UINT32 TemporalAlpha3; // 0-255
    UINT32 TemporalT0; // 0-63
    UINT32 TemporalT1; // 0-63
    UINT32 TemporalT2; // 0-63
    UINT32 TemporalT3; // 0-63
} AMBA_IK_VIDEO_MCTF_CURVE_s;

typedef struct {
    UINT32 Low;
    UINT32 LowDelta;
    UINT32 LowStrength;
    UINT32 MidStrength;
    UINT32 High;
    UINT32 HighDelta;
    UINT32 HighStrength;
} AMBA_IK_LEVEL_s;

typedef struct {
    UINT32 Low;
    UINT32 LowDelta;
    UINT32 LowStrength;
    UINT32 MidStrength;
    UINT32 High;
    UINT32 HighDelta;
    UINT32 HighStrength;
    UINT32 Method;
} AMBA_IK_LEVEL_METHOD_s;

typedef struct {
    UINT32 AlphaMinUp;
    UINT32 AlphaMaxUp;
    UINT32 T0Up;
    UINT32 T1Up;
    UINT32 AlphaMinDown;
    UINT32 AlphaMaxDown;
    UINT32 T0Down;
    UINT32 T1Down;
} AMBA_IK_FULL_ADAPTATION_s;

typedef struct {
    UINT32 Specify;
    UINT32 PerDirFirIsoStrengths[9];
    UINT32 PerDirFirDirStrengths[9];
    UINT32 PerDirFirDirAmounts[9];
    INT32 Coefs[9][25];
    UINT32 StrengthIso;
    UINT32 StrengthDir;
    UINT32 WideEdgeDetect;
} AMBA_IK_ASF_FIR_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_ASF_FIR_s Fir;
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    AMBA_IK_FULL_ADAPTATION_s Adapt;
    AMBA_IK_LEVEL_s LevelStrAdjust;
    AMBA_IK_LEVEL_s T0T1Div;
    UINT32 MaxChangeNotT0T1Alpha;
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
} AMBA_IK_ADV_SPAT_FLTR_s;

typedef struct {
    UINT32 Enable;
    UINT32 SensorWbB;
    UINT32 SensorWbG;
    UINT32 SensorWbR;
    UINT32 Strength0;
    UINT32 Strength1;
    UINT32 Strength2;
    UINT32 Strength2MaxChange;
} AMBA_IK_LUMA_NOISE_REDUCTION_s;

typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_SCL_COR_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_SCL_COR_s;

typedef struct {
    UINT8 YToneOffset;
    UINT8 YToneShift;
    UINT8 UToneOffset;
    UINT8 UToneShift;
    UINT8 UToneBits;
    UINT8 VToneOffset;
    UINT8 VToneShift;
    UINT8 VToneBits;
    UINT8 ThreeD[IK_NUM_THREE_D_TABLE];
} AMBA_IK_TABLE_INDEXING_s;

typedef AMBA_IK_TABLE_INDEXING_s AMBA_IK_FNLSHPNS_BOTH_TDT_s;

typedef struct {
    UINT32 Up5x5;
    UINT32 Down5x5;
    UINT32 Up;
    UINT32 Down;
} AMBA_IK_MAX_CHANGE_s;

typedef struct {
    UINT32 Enable;
    UINT32 Mode;
    UINT32 EdgeThresh;
    UINT32 WideEdgeDetect;
    UINT32 MaxChangeUp5x5;
    UINT32 MaxChangeDown5x5;
} AMBA_IK_FSTSHPNS_BOTH_s;

typedef struct {
    UINT32 Enable;
    UINT32 Mode;
    UINT32 EdgeThresh;
    UINT32 WideEdgeDetect;
    AMBA_IK_MAX_CHANGE_s MaxChange;
    UINT32 NarrowNonsmoothDetectSub[17];
    UINT32 NarrowNonsmoothDetectShift;
} AMBA_IK_FNLSHPNS_BOTH_s;

typedef struct {
    UINT32 Specify;
    UINT32 PerDirFirIsoStrengths[9];
    UINT32 PerDirFirDirStrengths[9];
    UINT32 PerDirFirDirAmounts[9];
    INT32 Coefs[9][25];
    UINT32 StrengthIso;
    UINT32 StrengthDir;
} AMBA_IK_SHPNS_FIR_s;

typedef AMBA_IK_SHPNS_FIR_s AMBA_IK_FSTSHPNS_FIR_s;

typedef struct {
    UINT32 Specify;
    UINT32 PerDirFirIsoStrengths[9];
    UINT32 PerDirFirDirStrengths[9];
    UINT32 PerDirFirDirAmounts[9];
    INT32 Coefs[9][25];
    UINT32 StrengthIso;
    UINT32 StrengthDir;
    UINT32 PosFirArtifactReduceEnable;
    UINT32 PosFirArtifactReduceStrength;
    UINT32 OverShootReduceAllow;
    UINT32 OverShootReduceStrength;
    UINT32 UnderShootReduceAllow;
    UINT32 UnderShootReduceStrength;
} AMBA_IK_FNLSHPNS_FIR_s;

typedef struct {
    UINT32 Enable;
    UINT32 MaxChangeMethod;
    UINT32 MaxChangeTable[32];
    UINT32 NoiseLevelMethod;
    UINT32 NoiseLevelTable[32];
    UINT32 Size;
    UINT32 StrAMethod;
    UINT32 StrATable[32];
    UINT32 StrBMethod;
    UINT32 StrBTable[32];
} AMBA_IK_SHPNS_ADV_ISO_s;

typedef struct {
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
    AMBA_IK_FSTSHPNS_FIR_s FirstShpFir;
    AMBA_IK_LEVEL_s LevelStrAdjust;
    UINT32 LevelStrAdjustNotT0T1LevelBased;
    UINT32 T0;
    UINT32 T1;
    UINT32 AlphaMin;
    UINT32 AlphaMax;
} AMBA_IK_FSTSHPNS_NOISE_s;

typedef struct {
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
    AMBA_IK_SHPNS_FIR_s FinalShpFir;
    AMBA_IK_LEVEL_METHOD_s LevelStrAdjust;
    UINT32 LevelStrAdjustNotT0T1LevelBased;
    UINT32 T0;
    UINT32 T1;
    UINT32 AlphaMin;
    UINT32 AlphaMax;
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    AMBA_IK_SHPNS_ADV_ISO_s AdvancedIso;
} AMBA_IK_FNLSHPNS_NOISE_s;

typedef struct {
    UINT32   Coring[IK_NUM_CORING_TABLE_INDEX];
    UINT32   FractionalBits;
} AMBA_IK_CORING_s;

typedef AMBA_IK_CORING_s AMBA_IK_FSTSHPNS_CORING_s;
typedef AMBA_IK_CORING_s AMBA_IK_FNLSHPNS_CORING_s;

typedef struct {
    UINT32 Enable;
    UINT32 NoiseLevelCb;
    UINT32 NoiseLevelCr;
    UINT32 OriginalBlendStrengthCb;
    UINT32 OriginalBlendStrengthCr;
    UINT32 Radius;
} AMBA_IK_CHROMA_FILTER_s;

typedef struct {
    UINT32 Enable;
    UINT32 NoiseLevelCb;
    UINT32 NoiseLevelCr;
} AMBA_IK_WIDE_CHROMA_FILTER_s;

typedef struct {
    UINT32 T0Cb;
    UINT32 T0Cr;
    UINT32 T1Cb;
    UINT32 T1Cr;
    UINT32 AlphaMaxCb;
    UINT32 AlphaMaxCr;
    UINT32 AlphaMinCb;
    UINT32 AlphaMinCr;
    UINT32 MaxChangeCb;
    UINT32 MaxChangeCr;
} AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s;

typedef struct {
    UINT32 NarrowEnable;
    UINT32 WideEnable;
    UINT32 WideSafety;
    UINT32 WideThresh;
} AMBA_IK_GRGB_MISMATCH_s;

typedef struct {
    UINT32 MaxChangeTable[32];
    UINT32 MaxChangeMethod;
    UINT32 Size;
    UINT32 NoiseLevel;
    UINT32 Enable;
} AMBA_IK_ADV_ISO_s;

typedef struct {
    UINT32 Enable; // 0,1
    UINT32 YCombineStrength; // 0:256
    UINT32 YStrength3d; // 0:256
    UINT32 YSpatialWeighting[64]; // 0:255
    UINT32 YTemporalMaxChange; // 0:255
    UINT32 CbStrength3d; // 0:256
    UINT32 CbSpatialWeighting[64]; // 0:255
    UINT32 CbTemporalMaxChange; // 0:255
    UINT32 CrStrength3d; // 0:256
    UINT32 CrSpatialWeighting[64]; // 0:255
    UINT32 CrTemporalMaxChange; // 0:255
    AMBA_IK_VIDEO_MCTF_CURVE_s YCurve;
    AMBA_IK_VIDEO_MCTF_CURVE_s CbCurve;
    AMBA_IK_VIDEO_MCTF_CURVE_s CrCurve;
    UINT32 YSpatSmthDirectDecideT0;
    UINT32 YSpatSmthDirectDecideT1;
    UINT32 CbSpatSmthDirectDecideT0;
    UINT32 CbSpatSmthDirectDecideT1;
    UINT32 CrSpatSmthDirectDecideT0;
    UINT32 CrSpatSmthDirectDecideT1;
    UINT32 YTemporalMaxChangeNotT0T1LevelBased; // 0:1
    AMBA_IK_LEVEL_METHOD_s YTemporalEitherMaxChangeOrT0T1Add;
    UINT32 CbTemporalMaxChangeNotT0T1LevelBased; // 0:1
    AMBA_IK_LEVEL_METHOD_s CbTemporalEitherMaxChangeOrT0T1Add;
    UINT32 CrTemporalMaxChangeNotT0T1LevelBased; // 0:1
    AMBA_IK_LEVEL_METHOD_s CrTemporalEitherMaxChangeOrT0T1Add;
    UINT32 YNarrowNonsmoothDetectSub[17]; // 0:255
    UINT32 YNarrowNonsmoothDetectShift; // 0:5
    UINT32 UseLevelBasedTa;  // 0:1
    //spat_blend level
    AMBA_IK_LEVEL_METHOD_s YSpatBlend;
    AMBA_IK_LEVEL_METHOD_s CbSpatBlend;
    AMBA_IK_LEVEL_METHOD_s CrSpatBlend;
    //advanced iso
    AMBA_IK_ADV_ISO_s YAdvancedIso;
    AMBA_IK_ADV_ISO_s CbAdvancedIso;
    AMBA_IK_ADV_ISO_s CrAdvancedIso;
    //3d max change level
    AMBA_IK_LEVEL_METHOD_s Y3dMaxchange;
    AMBA_IK_LEVEL_METHOD_s Cb3dMaxchange;
    AMBA_IK_LEVEL_METHOD_s Cr3dMaxchange;
    //overall max change level
    AMBA_IK_LEVEL_METHOD_s YOverallMaxChange;
    AMBA_IK_LEVEL_METHOD_s CbOverallMaxChange;
    AMBA_IK_LEVEL_METHOD_s CrOverallMaxChange;
    //spat filt max smth change level
    AMBA_IK_LEVEL_METHOD_s YSpatFiltMaxSmthChange;
    AMBA_IK_LEVEL_METHOD_s CbSpatFiltMaxSmthChange;
    AMBA_IK_LEVEL_METHOD_s CrSpatFiltMaxSmthChange;
    /*spat_smth_dir*/
    UINT32 YSpatSmthWideEdgeDetect; //0:8
    UINT32 YSpatSmthEdgeThresh; //0:2047
    AMBA_IK_LEVEL_METHOD_s YSpatSmthDir;
    UINT32 CbSpatSmthWideEdgeDetect;//0:8
    UINT32 CbSpatSmthEdgeThresh;//0:2047
    AMBA_IK_LEVEL_METHOD_s CbSpatSmthDir;
    UINT32 CrSpatSmthWideEdgeDetect;//0:8
    UINT32 CrSpatSmthEdgeThresh;//0:2047
    AMBA_IK_LEVEL_METHOD_s CrSpatSmthDir;
    /*spat_smth_iso*/
    AMBA_IK_LEVEL_METHOD_s YSpatSmthIso;
    AMBA_IK_LEVEL_METHOD_s CbSpatSmthIso;
    AMBA_IK_LEVEL_METHOD_s CrSpatSmthIso;
    //level based ta level
    AMBA_IK_LEVEL_METHOD_s YLevelBasedTa;
    AMBA_IK_LEVEL_METHOD_s CbLevelBasedTa;
    AMBA_IK_LEVEL_METHOD_s CrLevelBasedTa;
    //temporal min target
    AMBA_IK_LEVEL_METHOD_s YTemporalMinTarget;
    AMBA_IK_LEVEL_METHOD_s CbTemporalMinTarget;
    AMBA_IK_LEVEL_METHOD_s CrTemporalMinTarget;
    //spatial max temporal level
    UINT32 YSpatialMaxChange; //0:255
    AMBA_IK_LEVEL_METHOD_s YSpatialMaxTemporal;
    UINT32 CbSpatialMaxChange;//0:255
    AMBA_IK_LEVEL_METHOD_s CbSpatialMaxTemporal;
    UINT32 CrSpatialMaxChange;//0:255
    AMBA_IK_LEVEL_METHOD_s CrSpatialMaxTemporal;
    UINT32 LocalLighting;
    UINT32 CompressionEnable;  // 0:1
    UINT32 YTemporalGhostPrevent;
    UINT32 CbTemporalGhostPrevent;
    UINT32 CrTemporalGhostPrevent;
    UINT32 YTemporalArtifactGuard;
    UINT32 CbTemporalArtifactGuard;
    UINT32 CrTemporalArtifactGuard;
    UINT32 CompressionBitRateLuma;
    UINT32 CompressionBitRateChroma;
    UINT32 CompressionDitherDisable;
} AMBA_IK_VIDEO_MCTF_s;


typedef struct {
    UINT32 Max;
    UINT32 Min;
    UINT32 MotionResponse;
    UINT32 NoiseBase;
    UINT32 StillThresh;
} AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s;

typedef struct {
    UINT32 EdgeAdjustB;
    UINT32 EdgeAdjustL;
    UINT32 EdgeAdjustR;
    UINT32 EdgeAdjustT;
    UINT32 SlowMoSensitivity;
    UINT32 ScoreNoiseRobust;
    UINT32 LevAdjustLow;
    UINT32 LevAdjustLowDelta;
    UINT32 LevAdjustLowStrength;
    UINT32 LevAdjustMidStrength;
    UINT32 LevAdjustHigh;
    UINT32 LevAdjustHighDelta;
    UINT32 LevAdjustHighStrength;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s YMctfTa;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s CbMctfTa;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s CrMctfTa;
} AMBA_IK_VIDEO_MCTF_TA_s;

typedef struct {
    UINT32 Enable;
    UINT32 LevAdd1Method;
    UINT32 LevAdd1Table[64];
    UINT32 LevAdd2Method;
    UINT32 LevAdd2Table[64];
    UINT32 LevMul1Method;
    UINT32 LevMul1Table[64];
    UINT32 LevMul2Method;
    UINT32 LevMul2Table[64];
    UINT32 LocalLighting;
    UINT32 MotionInfoDelayed;
    UINT32 MotionResponse;
    UINT32 NoiseBase;
    UINT32 SlowMoSensitivity;
} AMBA_IK_MOTION_DETECT_s;

typedef struct {
    UINT8 PosDep[33][33];
} AMBA_IK_POS_DSP33_t;

typedef struct {
    UINT32 LocalLightingRadius;
} AMBA_IK_MOTION_DETECT_AND_MCTF_s;

typedef struct {
    UINT32 FactorNum;              /* subsamping factor (numerator) */
    UINT32 FactorDen;              /* subsamping factor (denominator) */
} AMBA_IK_SENSOR_SUBSAMPLING_s;

typedef struct {
    UINT32 StartX;     // Unit in pixel. Before downsample.
    UINT32 StartY;     // Unit in pixel. Before downsample.
    UINT32 Width;      // Unit in pixel. After downsample.
    UINT32 Height;     // Unit in pixel. After downsample.
    AMBA_IK_SENSOR_SUBSAMPLING_s HSubSample;
    AMBA_IK_SENSOR_SUBSAMPLING_s VSubSample;
} AMBA_IK_VIN_SENSOR_GEOMETRY_s;

typedef struct {
    UINT32 Version;
    UINT32 NumRadialBinsCoarse;                  //coarse + fine <= 128
    UINT32 SizeRadialBinsCoarseLog;             //valid range 0~8
    UINT32 NumRadialBinsFine;
    UINT32 SizeRadialBinsFineLog;               //valid range 0~8
    UINT32 ModelCenterXR;                       // 4 individual bayer components optical center x, relative to calib window.
    UINT32 ModelCenterXGr;
    UINT32 ModelCenterXB;
    UINT32 ModelCenterXGb;
    UINT32 ModelCenterYR;                       // 4 individual bayer components optical center x, relative to calib window.
    UINT32 ModelCenterYGr;
    UINT32 ModelCenterYB;
    UINT32 ModelCenterYGb;
    UINT32 VigGainTblR[4][128];            // pointer to [4 bayer chan][4 direction][128 vertices] 0:9b mult 10:13b shift
    UINT32 VigGainTblGr[4][128];
    UINT32 VigGainTblB[4][128];
    UINT32 VigGainTblGb[4][128];
} AMBA_IK_CALIB_VIGNETTE_INFO_s;

typedef struct {
    UINT32 CalibModeEnable;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;
    AMBA_IK_CALIB_VIGNETTE_INFO_s  CalibVignetteInfo;
} AMBA_IK_VIGNETTE_s;

typedef struct {
    UINT32 Version;
    UINT8 *pSbpBuffer;
} AMBA_IK_CALIB_SBP_INFO_s;

typedef struct {
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_IK_CALIB_SBP_INFO_s CalibSbpInfo;
} AMBA_IK_STATIC_BAD_PXL_COR_s;

typedef struct {
    INT32 BlackR;
    INT32 BlackGr;
    INT32 BlackGb;
    INT32 BlackB;
    INT32 BlackG;
    INT32 BlackIr;
} AMBA_IK_STATIC_BLC_LVL_s;

typedef struct {
    UINT32  CfaCutoffFreq; /* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    UINT32  MainCutoffFreq;
} AMBA_IK_RESAMPLER_STR_s;

typedef struct {
    UINT32 XOffset[IK_MAX_HDR_EXPOSURE_NUM];
    UINT32 YOffset[IK_MAX_HDR_EXPOSURE_NUM];
} AMBA_IK_HDR_RAW_INFO_s;

typedef struct {
    UINT32 Rgain; // 12.12 format
    UINT32 Ggain; // 12.12 format
    UINT32 Bgain; // 12.12 format
    UINT32 IrGain;
    UINT32 ShutterRatio;
} AMBA_IK_FE_WB_GAIN_s;

typedef struct {
    UINT32 Enable;
    UINT32 DeltaT0[4];
    UINT32 DeltaT1[4];
    UINT32 FlickerThreshold;
    UINT32 T0Offset[4];
    UINT32 T1Offset[4];
    UINT32 A0[4];
    UINT32 A1[4];
    UINT32 B0[4];
    UINT32 B1[4];
} AMBA_IK_HDR_BLEND_s;

typedef struct {
    UINT32 Enable;
    UINT32 LumaAvgMethod;

    INT32 LumaAvgWeightR;
    INT32 LumaAvgWeightGr;
    INT32 LumaAvgWeightGb;
    INT32 LumaAvgWeightB;

    UINT32 FirEnable;
    UINT32 FirCoeff[3];
    UINT32 CoringIndexScaleShift;

    UINT32 CoringIndexScaleLow;
    UINT32 CoringIndexScaleLowDelta;
    UINT32 CoringIndexScaleLowStrength;
    UINT32 CoringIndexScaleMidStrength;
    UINT32 CoringIndexScaleHigh;
    UINT32 CoringIndexScaleHighDelta;
    UINT32 CoringIndexScaleHighStrength;

    UINT32 CoringGainShift;
    UINT32 CoringGainLow;
    UINT32 CoringGainLowDelta;
    UINT32 CoringGainLowStrength;
    UINT32 CoringGainMidStrength;
    UINT32 CoringGainHigh;
    UINT32 CoringGainHighDelta;
    UINT32 CoringGainHighStrength;

    UINT32 CoringTable[IK_CE_CORING_TABLE_SIZE];
    //cv2
    UINT32 BoostGainShift;
    INT32 BoostTable[IK_CE_BOOST_TABLE_SIZE];
    UINT32 BoostTableSizeExp;

    UINT32 Radius;
    UINT32 Epsilon;
} AMBA_IK_CE_s;

typedef struct {
    UINT32 InputEnable;
    UINT32 InputTable[IK_CE_INPUT_TABLE_SIZE];
} AMBA_IK_CE_INPUT_TABLE_s;

typedef struct {
    UINT32 OutputEnable;
    UINT32 OutputShift;
    UINT32 OutputTable[IK_CE_OUTPUT_TABLE_SIZE];
} AMBA_IK_CE_OUT_TABLE_s;

typedef struct {
    UINT32  AeTileNumCol;
    UINT32  AeTileNumRow;
    UINT32  AeTileColStart;
    UINT32  AeTileRowStart;
    UINT32  AeTileWidth;
    UINT32  AeTileHeight;
    UINT32  AePixMinValue;
    UINT32  AePixMaxValue;
} AMBA_IK_AE_STAT_INFO_s;

typedef struct {
    UINT32  AwbTileNumCol;
    UINT32  AwbTileNumRow;
    UINT32  AwbTileColStart;
    UINT32  AwbTileRowStart;
    UINT32  AwbTileWidth;
    UINT32  AwbTileHeight;
    UINT32  AwbTileActiveWidth;
    UINT32  AwbTileActiveHeight;
    UINT32  AwbPixMinValue;
    UINT32  AwbPixMaxValue;
} AMBA_IK_AWB_STAT_INFO_s;

typedef struct {
    UINT32  AfTileNumCol;
    UINT32  AfTileNumRow;
    UINT32  AfTileColStart;
    UINT32  AfTileRowStart;
    UINT32  AfTileWidth;
    UINT32  AfTileHeight;
    UINT32  AfTileActiveWidth;
    UINT32  AfTileActiveHeight;
} AMBA_IK_AF_STAT_INFO_s;

typedef struct {
    UINT32  AwbTileNumCol;
    UINT32  AwbTileNumRow;
    UINT32  AwbTileColStart;
    UINT32  AwbTileRowStart;
    UINT32  AwbTileWidth;
    UINT32  AwbTileHeight;
    UINT32  AwbTileActiveWidth;
    UINT32  AwbTileActiveHeight;
    UINT32  AwbPixMinValue;
    UINT32  AwbPixMaxValue;
    UINT32  AeTileNumCol;
    UINT32  AeTileNumRow;
    UINT32  AeTileColStart;
    UINT32  AeTileRowStart;
    UINT32  AeTileWidth;
    UINT32  AeTileHeight;
    UINT32  AePixMinValue;
    UINT32  AePixMaxValue;
    UINT32  AfTileNumCol;
    UINT32  AfTileNumRow;
    UINT32  AfTileColStart;
    UINT32  AfTileRowStart;
    UINT32  AfTileWidth;
    UINT32  AfTileHeight;
    UINT32  AfTileActiveWidth;
    UINT32  AfTileActiveHeight;
    UINT32  SixMinusCfaInputShift;
} AMBA_IK_AAA_STAT_INFO_s;

typedef struct {
    UINT32 AfHorizontalFilter1Mode;
    UINT32 AfHorizontalFilter1Stage1Enb;
    UINT32 AfHorizontalFilter1Stage2Enb;
    UINT32 AfHorizontalFilter1Stage3Enb;
    INT32 AfHorizontalFilter1Gain[7];
    UINT32 AfHorizontalFilter1Shift[4];
    UINT32 AfHorizontalFilter1BiasOff;
    UINT32 AfHorizontalFilter1Thresh;
    UINT32 AfHorizontalFilter2Mode;
    UINT32 AfHorizontalFilter2Stage1Enb;
    UINT32 AfHorizontalFilter2Stage2Enb;
    UINT32 AfHorizontalFilter2Stage3Enb;
    INT32 AfHorizontalFilter2Gain[7];
    UINT32 AfHorizontalFilter2Shift[4];
    UINT32 AfHorizontalFilter2BiasOff;
    UINT32 AfHorizontalFilter2Thresh;
    UINT32 AfTileFv1HorizontalShift;
    UINT32 AfTileFv2HorizontalShift;
} AMBA_IK_AF_STAT_EX_INFO_s;

typedef struct {
    UINT32 AfHorizontalFilter1Mode;
    UINT32 AfHorizontalFilter1Stage1Enb;
    UINT32 AfHorizontalFilter1Stage2Enb;
    UINT32 AfHorizontalFilter1Stage3Enb;
    INT32 AfHorizontalFilter1Gain[7];
    UINT32 AfHorizontalFilter1Shift[4];
    UINT32 AfHorizontalFilter1BiasOff;
    UINT32 AfHorizontalFilter1Thresh;
    UINT32 AfVerticalFilter1Thresh;
    UINT32 AfHorizontalFilter2Mode;
    UINT32 AfHorizontalFilter2Stage1Enb;
    UINT32 AfHorizontalFilter2Stage2Enb;
    UINT32 AfHorizontalFilter2Stage3Enb;
    INT32 AfHorizontalFilter2Gain[7];
    UINT32 AfHorizontalFilter2Shift[4];
    UINT32 AfHorizontalFilter2BiasOff;
    UINT32 AfHorizontalFilter2Thresh;
    UINT32 AfVerticalFilter2Thresh;
    UINT32 AfTileFv1HorizontalShift;
    UINT32 AfTileFv1VerticalShift;
    UINT32 AfTileFv1HorizontalWeight;
    UINT32 AfTileFv1VerticalWeight;
    UINT32 AfTileFv2HorizontalShift;
    UINT32 AfTileFv2VerticalShift;
    UINT32 AfTileFv2HorizontalWeight;
    UINT32 AfTileFv2VerticalWeight;
} AMBA_IK_PG_AF_STAT_EX_INFO_s;

typedef struct {
    UINT32 Width; /* Unit in pixel*/
    UINT32 Height;
} AMBA_IK_WINDOW_DIMENSION_s;

typedef struct {
    UINT32 StartX;
    UINT32 StartY;
    UINT32 Width;
    UINT32 Height;
} AMBA_IK_WINDOW_GEOMETRY_s;

typedef struct {
    UINT32 LeftTopX;   /* 16.16 format*/
    UINT32 LeftTopY;   /* 16.16 format*/
    UINT32 RightBotX;  /* 16.16 format*/
    UINT32 RightBotY;  /* 16.16 format*/
} AMBA_IK_WIN_COORDINATES_s;

typedef struct {
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensor;
    AMBA_IK_WINDOW_DIMENSION_s Main;
    AMBA_IK_WINDOW_DIMENSION_s Prev[3];
    AMBA_IK_WINDOW_DIMENSION_s Thumbnail;
    AMBA_IK_WINDOW_DIMENSION_s Screennail;
} AMBA_IK_WINDOW_SIZE_INFO_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_WINDOW_DIMENSION_s Cfa;
} AMBA_IK_CFA_WINDOW_SIZE_INFO_s;

typedef struct {
    INT16 X;
    INT16 Y;
} AMBA_IK_GRID_POINT_s;

typedef struct {
    UINT32 Version;
    UINT32 HorGridNum;          /* Horizontal grid number. Range to 256 as maximum. */
    UINT32 VerGridNum;          /* Vertical grid number. Range to 192 as maximum. */
    UINT32 TileWidthExp;        /* 3:8, 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    UINT32 TileHeightExp;       /* 3:8, 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;   /* Vin sensor geometry when calibrating. */
    UINT32 Enb_2StageCompensation : 1; /*1:enable, 0 disable*/
    UINT32 Reserved : 31;       /* Reserved for extention. */
    AMBA_IK_GRID_POINT_s *pWarp;         /* Warp grid vector array. */
} AMBA_IK_WARP_INFO_s;

typedef struct  {
  UINT32 Enable;   //0, 1
  UINT32 TileNum_x;           // 1~4
  UINT32 TileNum_y;           // 1~4
} AMBA_IK_STITCH_INFO_s;

typedef struct {
    UINT32   Enable;
} AMBA_IK_BURST_TILE_s;

typedef struct {
    UINT32 Version;
    UINT32 HorGridNum;          /* Horizontal grid number. Range to 64 as maximum. */
    UINT32 VerGridNum;          /* Vertical grid number. Range to 96 as maximum. */
    UINT32 TileWidthExp;        /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    UINT32 TileHeightExp;       /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;   /* Vin sensor geometry when calibrating. */
    UINT32 Enb2StageCompensation : 1;
    UINT32 Reserved : 31;       /* Reserved for extention. */
    AMBA_IK_GRID_POINT_s *pCawarpRed;    /* Cawarp grid vector array. */
    AMBA_IK_GRID_POINT_s *pCawarpBlue;   /* Cawarp grid vector array. */
} AMBA_IK_CAWARP_INFO_s;

typedef struct {
    UINT32 Enable;
    UINT32 ZoomX;
    UINT32 ZoomY;
    INT32 ShiftX;
    INT32 ShiftY;
} AMBA_IK_DZOOM_INFO_s;

typedef struct {
    INT32 ShiftX;
    INT32 ShiftY;
} AMBA_IK_HDR_EIS_INFO_s;

typedef struct {
    UINT32 Enable;
    UINT32 Left;
    UINT32 Right;
    UINT32 Top;
    UINT32 Bottom;
} AMBA_IK_DUMMY_MARGIN_RANGE_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_WINDOW_GEOMETRY_s  ActiveGeo; 
} AMBA_IK_VIN_ACTIVE_WINDOW_s;

typedef struct {
    UINT32 DecompandEnable;
    UINT32 DecompandTable[IK_NUM_DECOMPAND_TABLE];
} AMBA_IK_FE_TONE_CURVE_s;

typedef struct {
    UINT32  AeTileMask[16];
} AMBA_IK_HISTOGRAM_INFO_s;

typedef struct {
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_IK_WINDOW_GEOMETRY_s VinActiveGeo;
    AMBA_IK_DZOOM_INFO_s DzoomInfo;
    AMBA_IK_DUMMY_MARGIN_RANGE_s DummyMarginRange;
    AMBA_IK_WINDOW_DIMENSION_s CfaWindow;
    AMBA_IK_WINDOW_DIMENSION_s MainWindow;
    AMBA_IK_WINDOW_DIMENSION_s PrevWindow[3];
    UINT32 DspVideoMode;
    UINT32 Fps;
} AMBA_IK_QUERY_IDSP_CLOCK_INFO_s;

typedef struct {
    UINT32 TheoreticalClock;
} AMBA_IK_QUERY_IDSP_CLOCK_RST_s;

typedef struct {
    UINT32 DramEfficiency;
    UINT32 LumaDmaSize;
    UINT32 LumaWaitLines;
} AMBA_IK_WARP_BUFFER_INFO_s;

typedef struct {
    UINT32 OverlapX;
} AMBA_IK_OVERLAP_X_s;

typedef struct {
    UINT32                          WorkBufSize;
    UINT32                          OutBufSize;
} AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s;

typedef struct {
    UINT32                          FlipMode;
    UINT32                          YuvMode;
    AMBA_IK_VIN_SENSOR_INFO_s       *pSensorInfo;
    AMBA_IK_RGB_IR_s                *pRgbIr;
    AMBA_IK_AAA_STAT_INFO_s         *pAaaStatInfo;
    AMBA_IK_AF_STAT_INFO_s          *pAaaPgAfStatInfo;
    AMBA_IK_AF_STAT_EX_INFO_s       *pAfStatExInfo;
    AMBA_IK_PG_AF_STAT_EX_INFO_s    *pPgAfStatExInfo;
    AMBA_IK_HISTOGRAM_INFO_s        *pHistInfo;
    AMBA_IK_HISTOGRAM_INFO_s        *pHistInfoPg;
    INT32                           WarpEnable;
    INT32                           Warp2ndEnable;
    UINT32                          ChromaFilterRadius;
    AMBA_IK_WINDOW_SIZE_INFO_s      *pWindowSizeInfo;   // M
    AMBA_IK_VIN_ACTIVE_WINDOW_s     *pVinActiveWindow;  // O
    AMBA_IK_DUMMY_MARGIN_RANGE_s    *pDmyRange;         // O
    AMBA_IK_DZOOM_INFO_s            *pDzoomInfo;        // O
    AMBA_IK_STITCH_INFO_s           *pStitchInfo;       // O
    AMBA_IK_WARP_INFO_s             *pWarpInfo;
    AMBA_IK_WARP_INFO_s             *pWarpInfo2nd;
    INT32                           UseCawarp;
    INT32                           CawarpEnable;       // only take effect when use_cawarp = 1
    AMBA_IK_CAWARP_INFO_s          *pCalibCaWarpInfo;   // only take effect when use_cawarp = 1
} AMBA_IK_CALC_GEO_SETTINGS_IN_s;

typedef struct {
    AMBA_IK_CALC_GEO_SETTINGS_IN_s  *pIn;
    void                            *pWorkBuf;
    UINT32                          WorkBufSize;
} AMBA_IK_CALC_GEO_SETTINGS_s;

typedef struct {
    void                            *pOut;
    UINT32                          OutBufSize;
} AMBA_IK_CALC_GEO_RESULT_s;

typedef struct {
    AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo;// Mandatory
    AMBA_IK_VIN_ACTIVE_WINDOW_s *pActiveWindow;// Optional
    AMBA_IK_DUMMY_MARGIN_RANGE_s *pDmyRange;// Optional
    AMBA_IK_DZOOM_INFO_s *pDzoomInfo;// Optional
    UINT32 FlipMode;// Mandatory
    UINT32 Ability;// Mandatory
    UINT32 WarpEnb;// Mandatory
    UINT32 CawarpEnb;// Mandatory
    AMBA_IK_WARP_INFO_s *pWarpInfo;// Optional
} AMBA_IK_QUERY_WARP_INPAR_s;

typedef struct {
    AMBA_IK_QUERY_WARP_INPAR_s In;
    void *pWorkBuf;//885988 bytes
    UINT32 WorkBufSize;
} AMBA_IK_QUERY_WARP_s;

typedef struct {
    UINT32 WaitLine;
    UINT32 DmaSize;
    UINT32 VerticalWarpVectorFlipNum;
    UINT32 WarpVectorVFlipNum;
    UINT32 DmyBroken;
    AMBA_IK_DUMMY_MARGIN_RANGE_s SuggestedDmyRange;
} AMBA_IK_QUERY_WARP_RESULT_s;

typedef struct {
    AMBA_IK_WARP_INFO_s *pInputInfo;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;//FOV optimize, must within input_info.vin_sensor_geo.
    void *pWorkingBuffer;
    UINT32 WorkingBufferSize;
} AMBA_IK_IN_WARP_PRE_PROC_s;

typedef struct {
    AMBA_IK_WARP_INFO_s *pResultInfo;
} AMBA_IK_OUT_WARP_PRE_PROC_s;

typedef struct {
    AMBA_IK_CAWARP_INFO_s *pInpuIinfo;
    void *pWorkingBuffer;
    UINT32 WorkingBufferSize;
} AMBA_IK_IN_CAWARP_PRE_PROC_s;

typedef struct {
    AMBA_IK_CAWARP_INFO_s *pResultInfo;
} AMBA_IK_OUT_CAWARP_PRE_PROC_s;

typedef struct {
    UINT32   Enb;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s  CurrentVinSensorGeo;
    AMBA_IK_CALIB_SBP_INFO_s       CalibSbpInfo;
} AMBA_IK_SBP_CORRECTION_s;

typedef struct {
    AMBA_IK_HDR_RAW_INFO_s HdrRawInfo;
    UINT32 Reserved[26];
} AMBA_IK_QUERY_FRAME_INFO_s; // make it 128 alignment

typedef struct {
    UINT32 PreCcGainEnable;
    UINT32 PreCcGainR;
    UINT32 PreCcGainG;
    UINT32 PreCcGainB;
} AMBA_IK_PRE_CC_GAIN_s;

/**
* Set the Vin Sensor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Vin sensor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVinSensorInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIN_SENSOR_INFO_s *pVinSensorInfo);
/**
* Get the Vin Sensor Information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVinSensorInfo, Get Vin sensor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVinSensorInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIN_SENSOR_INFO_s *pVinSensorInfo);

/**
* Set the YUV Mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  YuvMode, Set yuv mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetYuvMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 YuvMode);
/**
* Get the YUV Mode Information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pYuvMode, Get yuv mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetYuvMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pYuvMode);

/**
* Set the before CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pBeforeCeWbGain, Set before CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetBeforeCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WB_GAIN_s *pBeforeCeWbGain);
/**
* Get the before CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pBeforeCeWbGain, Get before CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetBeforeCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WB_GAIN_s *pBeforeCeWbGain);


/**
* Set the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfterCeWbGain, Set after CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAfterCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WB_GAIN_s *pAfterCeWbGain);
/**
* Get the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAfterCeWbGain, Get after CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAfterCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WB_GAIN_s *pAfterCeWbGain);


/**
* Set the StoreIr filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStoreIr, Set StoreIr filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStoreIr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STORED_IR_s *pStoreIr);
/**
* Get the StoreIr filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pStoreIr, Get StoreIr filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStoreIr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STORED_IR_s *pStoreIr);


/**
* Set the CFA leakage filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaLeakageFilter, Set CFA leakage filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_LEAKAGE_FILTER_s *pCfaLeakageFilter);
/**
* Get the CFA leakage filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCfaLeakageFilter, Get CFA leakage filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_LEAKAGE_FILTER_s *pCfaLeakageFilter);


/**
* Set the Grgb Mismatch of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pGrgbMismatch, Set Grgb Mismatch filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetGrgbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_GRGB_MISMATCH_s *pGrgbMismatch);
/**
* Get the Grgb Mismatch of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pGrgbMismatch, Get Grgb Mismatch filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetGrgbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_GRGB_MISMATCH_s *pGrgbMismatch);


/**
* Set the Anti-Aliasing filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAntiAliasing, Set Anti-Aliasing filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ANTI_ALIASING_s *pAntiAliasing);
/**
* Get the Anti-Aliasing filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAntiAliasing, Get Anti-Aliasing filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ANTI_ALIASING_s *pAntiAliasing);


/**
* Set the dynamic bad pixel filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDynamicBadPxlCor, Set dynamic bad pixel filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDynamicBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DYNAMIC_BAD_PXL_COR_s *pDynamicBadPxlCor);
/**
* Get the dynamic bad pixel filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pDynamicBadPxlCor, Get dynamic bad pixel filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDynamicBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DYNAMIC_BAD_PXL_COR_s *pDynamicBadPxlCor);


/**
* Set the CFA noise filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaNoiseFilter, Set CFA noise filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_NOISE_FILTER_s *pCfaNoiseFilter);
/**
* Get the CFA noise filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCfaNoiseFilter, Get CFA noise filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_NOISE_FILTER_s *pCfaNoiseFilter);


/**
* Set the demosaic filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDemosaic, Set demosaic filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDemosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DEMOSAIC_s *pDemosaic);
/**
* Get the demosaic filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pDemosaic, Get demosaic filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDemosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DEMOSAIC_s *pDemosaic);

/**
* Set the rgb_to_12y filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbTo12Y, Set rgb_to_12y filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetRgbTo12Y(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_TO_12Y_s *pRgbTo12Y);

/**
* Get the rgb_to_12y filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pRgbTo12Y, Get rgb_to_12y filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetRgbTo12Y(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_TO_12Y_s *pRgbTo12Y);

/**
* Set the luma_noise_reduction filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pLumaNoiseReduction, Set luma_noise_reduction filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LUMA_NOISE_REDUCTION_s *pLumaNoiseReduction);

/**
* Get the luma_noise_reduction filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pLumaNoiseReduction, Get luma_noise_reduction filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LUMA_NOISE_REDUCTION_s *pLumaNoiseReduction);

/**
* Set the color correction of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pColorCorrection, Set color correction
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetColorCorrection(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_COLOR_CORRECTION_s *pColorCorrection);
/**
* Get the color correction of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pColorCorrection, Get color correction
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetColorCorrection(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_COLOR_CORRECTION_s *pColorCorrection);


/**
* Set the tone curve filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pToneCurve, Set tone curve filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetToneCurve(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_TONE_CURVE_s *pToneCurve);
/**
* Get the tone curve filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pToneCurve, Get tone curve filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetToneCurve(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_TONE_CURVE_s *pToneCurve);


/**
* Set the RGB to YUV matrix of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbToYuvMatrix, Set RGB to YUV matrix
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetRgbToYuvMatrix(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_TO_YUV_MATRIX_s *pRgbToYuvMatrix);
/**
* Get the RGB to YUV matrix of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pRgbToYuvMatrix, Get RGB to YUV matrix
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetRgbToYuvMatrix(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_TO_YUV_MATRIX_s *pRgbToYuvMatrix);


/**
* Set the RgbIr setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbIr, Set RgbIr filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetRgbIr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_IR_s *pRgbIr);
/**
* Get the RgbIr setting of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pRgbIr, Get RgbIr filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetRgbIr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_IR_s *pRgbIr);


/**
* Set the chroma scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaScale, Set chroma scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaScale(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_SCALE_s *pChromaScale);
/**
* Get the chroma scale of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pChromaScale, Get chroma scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaScale(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_SCALE_s *pChromaScale);

/**
* Set the chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaFilter, Set chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_FILTER_s *pChromaFilter);
/**
* Get the chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pChromaFilter, Get chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_FILTER_s *pChromaFilter);

/**
* Set the wide chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWideChromaFilter, Set wide chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WIDE_CHROMA_FILTER_s *pWideChromaFilter);

/**
* Get the wide chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pWideChromaFilter, Get wide chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WIDE_CHROMA_FILTER_s *pWideChromaFilter);

/**
* Set the wide chroma filter combine of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWideChromaFilterCombine, Set wide chroma filter combine
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s *pWideChromaFilterCombine);

/**
* Get the wide chroma filter combine of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pWideChromaFilterCombine, Get wide chroma filter combine
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s *pWideChromaFilterCombine);

/**
* Set the chroma median filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaMedianFilter, Set chroma median filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_MEDIAN_FILTER_s *pChromaMedianFilter);
/**
* Get the chroma median filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pChromaMedianFilter, Get chroma median filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_MEDIAN_FILTER_s *pChromaMedianFilter);


/**
* Set the first luma processing mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFirstLumaProcMode, Set first luma processing mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFirstLumaProcMode(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FIRST_LUMA_PROC_MODE_s *pFirstLumaProcMode);
/**
* Get the first luma processing mode of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFirstLumaProcMode, Get first luma processing mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFirstLumaProcMode(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FIRST_LUMA_PROC_MODE_s *pFirstLumaProcMode);

/**
* Set the advance spatial filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAdvSpatFltr, Set advance spatial filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAdvSpatFltr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ADV_SPAT_FLTR_s *pAdvSpatFltr);
/**
* Get the advance spatial filter of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAdvSpatFltr, Get advance spatial filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAdvSpatFltr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ADV_SPAT_FLTR_s *pAdvSpatFltr);


/**
* Set the first sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsBoth, Set first sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_BOTH_s *pFstShpNsBoth);
/**
* Get the first sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsBoth, Get first sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_BOTH_s *pFstShpNsBoth);


/**
* Set the first sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsFir, Set first sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_FIR_s *pFstShpNsFir);
/**
* Get the first sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsFir, Get first sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_FIR_s *pFstShpNsFir);


/**
* Set the first sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsNoise, Set first sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_NOISE_s *pFstShpNsNoise);
/**
* Get the first sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsNoise, Get first sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_NOISE_s *pFstShpNsNoise);


/**
* Set the first sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsCoring, Set first sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_CORING_s *pFstShpNsCoring);
/**
* Get the first sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsCoring, Get first sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_CORING_s *pFstShpNsCoring);


/**
* Set the first sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsCorIdxScl, Set first sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *pFstShpNsCorIdxScl);
/**
* Get the first sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsCorIdxScl, Set first sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *pFstShpNsCorIdxScl);


/**
* Set the first sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsMinCorRst, Set first sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_MIN_COR_RST_s *pFstShpNsMinCorRst);
/**
* Get the first sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsMinCorRst, Get first sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_MIN_COR_RST_s *pFstShpNsMinCorRst);


/**
* Set the first sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsMaxCorRst, Set first sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_MAX_COR_RST_s *pFstShpNsMaxCorRst);
/**
* Get the first sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsMaxCorRst, Get first sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_MAX_COR_RST_s *pFstShpNsMaxCorRst);


/**
* Set the first sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsSclCor, Set first sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_SCL_COR_s *pFstShpNsSclCor);
/**
* Get the first sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFstShpNsSclCor, Get first sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_SCL_COR_s *pFstShpNsSclCor);


/**
* Set the final sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsBoth, Set final sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_BOTH_s *pFnlShpNsBoth);
/**
* Get the final sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsBoth, Get final sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_BOTH_s *pFnlShpNsBoth);


/**
* Set the final sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsFir, Set final sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_FIR_s *pFnlShpNsFir);
/**
* Get the final sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsFir, Get final sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_FIR_s *pFnlShpNsFir);


/**
* Set the final sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsNoise, Set final sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_NOISE_s *pFnlShpNsNoise);
/**
* Get the final sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsNoise, Get final sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_NOISE_s *pFnlShpNsNoise);


/**
* Set the final sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsCoring, Set final sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_CORING_s *pFnlShpNsCoring);
/**
* Get the final sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsCoring, Get final sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_CORING_s *pFnlShpNsCoring);


/**
* Set the final sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsCorIdxScl, Set final sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *pFnlShpNsCorIdxScl);
/**
* Get the final sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsCorIdxScl, Set final sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *pFnlShpNsCorIdxScl);


/**
* Set the final sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsMinCorRst, Set final sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_MIN_COR_RST_s *pFnlShpNsMinCorRst);
/**
* Get the final sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsMinCorRst, Get final sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_MIN_COR_RST_s *pFnlShpNsMinCorRst);


/**
* Set the final sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsMaxCorRst, Set final sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_MAX_COR_RST_s *pFnlShpNsMaxCorRst);
/**
* Get the final sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsMaxCorRst, Get final sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_MAX_COR_RST_s *pFnlShpNsMaxCorRst);


/**
* Set the final sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsSclCor, Set final sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_SCL_COR_s *pFnlShpNsSclCor);
/**
* Get the final sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsSclCor, Get final sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_SCL_COR_s *pFnlShpNsSclCor);


/**
* Set the final sharpen noise both 3d table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsBothTdt, Set final sharpen noise both 3d table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsBothTdt(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_BOTH_TDT_s *pFnlShpNsBothTdt);
/**
* Get the final sharpen noise both 3d table of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFnlShpNsBothTdt, Get final sharpen noise both 3d table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsBothTdt(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_BOTH_TDT_s *pFnlShpNsBothTdt);


/**
* Set the video mctf of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctf, Set video mctf
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVideoMctf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIDEO_MCTF_s *pVideoMctf);
/**
* Get the video mctf of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVideoMctf, Get video mctf
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVideoMctf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIDEO_MCTF_s *pVideoMctf);


/**
* Set the video mctf temporal adjust of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctfTa, Set video mctf temporal adjust
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVideoMctfTa(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIDEO_MCTF_TA_s *pVideoMctfTa);
/**
* Get the video mctf temporal adjust of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVideoMctfTa, Get video mctf temporal adjust
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVideoMctfTa(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIDEO_MCTF_TA_s *pVideoMctfTa);


/**
* Set the video mctf and final sharpen of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctfAndFnlshp, Set video mctf and final sharpen
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVideoMctfAndFnlshp(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pVideoMctfAndFnlshp);
/**
* Get the video mctf and final sharpen of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVideoMctfAndFnlshp, Get video mctf and final sharpen
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVideoMctfAndFnlshp(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pVideoMctfAndFnlshp);

/**
* Set the motion detect of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pMotionDetect, Set motion detect
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_s*pMotionDetect);
/**
* Get the motion detect of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pMotionDetect, Get motion detect
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_s *pMotionDetect);

/**
* Set the motion detect pos33 of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pMotionDetectPosDep, Set motion detect pos33
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pMotionDetectPosDep);
/**
* Get the motion detect pos33 of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pMotionDetectPosDep, Get motion detect pos33
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pMotionDetectPosDep);

/**
* Set the motion detect and mctf of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pMotionDetectAndMCTF, Set motion detect and mctf
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetMotionDetectAndMCTF(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_AND_MCTF_s *pMotionDetectAndMCTF);
/**
* Get the motion detect and mctf of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pMotionDetectAndMCTF, Get motion detect and mctf
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetMotionDetectAndMCTF(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_AND_MCTF_s *pMotionDetectAndMCTF);


/**
* Set the dzoom information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDzoomInfo, Set dzoom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDzoomInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DZOOM_INFO_s *pDzoomInfo);
/**
* Get the dzoom information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pDzoomInfo, Get dzoom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDzoomInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DZOOM_INFO_s *pDzoomInfo);

/**
* Set the Hdr Eis Shift Compensation information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrSftInfo, Set Hdr Eis Shift Compensation
* @param [in]  ExpIdx, Set Exposure Index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrShiftInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_EIS_INFO_s *pHdrSftInfo, UINT32 ExpIdx);
/**
* Get the Hdr Eis Shift Compensation information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHdrSftInfo, Get Hdr Eis Shift Compensation
* @param [in]  ExpIdx, Set Exposure Index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrShiftInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_EIS_INFO_s *pHdrSftInfo, UINT32 ExpIdx);

/**
* Set the window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWindowSizeInfo, Set window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo);
/**
* Get the window size information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pWindowSizeInfo, Get window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo);

/**
* Set the cfa window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaWindowSizeInfo, Set cfa window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCfaWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_WINDOW_SIZE_INFO_s *pCfaWindowSizeInfo);
/**
* Get the cfa window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCfaWindowSizeInfo, Get cfa window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCfaWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_WINDOW_SIZE_INFO_s *pCfaWindowSizeInfo);

/**
* Set the dummy margin range of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDummyMarginRange, Set dummy margin range
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDummyMarginRange(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyMarginRange);
/**
* Get the dummy margin range of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pDummyMarginRange, Get dummy margin range
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDummyMarginRange(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyMarginRange);

/**
* Set the vin window information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinActiveWin, Set vin window information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVinActiveWin(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIN_ACTIVE_WINDOW_s *pVinActiveWin);
/**
* Get the vin window information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVinActiveWin, Get vin window information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVinActiveWin(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIN_ACTIVE_WINDOW_s *pVinActiveWin);

/**
* Set the resamp strength of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pResamplerStr, Set resamp strength
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetResampStrength(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RESAMPLER_STR_s *pResamplerStr);
/**
* Get the resamp strength of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pResamplerStr, Get resamp strength
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetResampStrength(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RESAMPLER_STR_s *pResamplerStr);

/* Calibration filters */
/**
* Set the warp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  WarpEnb, Set warp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarpEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 WarpEnb);
/**
* Get the warp enable of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pWarpEnb, Get warp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarpEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pWarpEnb);


/**
* Set the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo, Set warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarpInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INFO_s *pWarpInfo);
/**
* Get the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo, Get warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarpInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INFO_s *pWarpInfo);

/* Calibration filters */
/**
* Set the warp2nd enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  Warp2ndEnb, Set warp2nd enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarp2ndEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 Warp2ndEnb);
/**
* Get the warp2nd enable of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pWarp2ndEnb, Get warp2nd enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarp2ndEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pWarp2ndEnb);

/**
* Set the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarp2ndInfo, Set warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarp2ndInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INFO_s *pWarp2ndInfo);
/**
* Get the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarp2ndInfo, Get warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarp2ndInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INFO_s *pWarp2ndInfo);

/**
* Set the cawarp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  CawarpEnb, Set cawarp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCawarpEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 CawarpEnb);
/**
* Get the cawarp enable of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCawarpEnb, Get cawarp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCawarpEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pCawarpEnb);


/**
* Set the cawarp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCawarpInfo, Set cawarp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCawarpInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INFO_s *pCawarpInfo);
/**
* Get the cawarp information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCawarpInfo, Get cawarp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCawarpInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INFO_s *pCawarpInfo);


/**
* Set the static bad pixel correction enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  StaticBpcEnb, Set static bad pixel correction enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStaticBadPxlCorEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 StaticBpcEnb);
/**
* Get the static bad pixel correction enable of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pStaticBpcEnb, Get static bad pixel correction enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStaticBadPxlCorEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pStaticBpcEnb);


/**
* Set the static bad pixel correction information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStaticBadPxlCor, Set static bad pixel correction information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStaticBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STATIC_BAD_PXL_COR_s *pStaticBadPxlCor);
/**
* Get the static bad pixel correction information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pStaticBadPxlCor, Get static bad pixel correction information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStaticBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STATIC_BAD_PXL_COR_s *pStaticBadPxlCor);

/**
* Set the vignette enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  VignetteEnb, Set vignette enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVignetteEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 VignetteEnb);
/**
* Get the vignette enable of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVignetteEnb, Get vignette enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVignetteEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pVignetteEnb);

/**
* Set the vignette information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVignette, Set vignette information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVignette(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIGNETTE_s *pVignette);
/**
* Get the vignette information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pVignette, Get vignette information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVignette(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIGNETTE_s *pVignette);

/**
* Set the flip mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  FlipMode, Set Flip Mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFlipMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 FlipMode);
/**
* Get the flip mode of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFlipMode, Get Flip Mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFlipMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pFlipMode);

/**
* Set the warp buffer info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpBufInfo, Set warp buffer information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarpBufferInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_BUFFER_INFO_s *pWarpBufInfo);
/**
* Get the warp buffer info of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pWarpBufInfo, Get warp buffer information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarpBufferInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_BUFFER_INFO_s *pWarpBufInfo);

/**
* Set the overlap x of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pOverlapX, Set overlap x
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetOverlapX(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_OVERLAP_X_s *pOverlapX);
/**
* Get the overlap x of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pOverlapX, Get overlap x
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetOverlapX(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_OVERLAP_X_s *pOverlapX);

/**
* Set the stitching info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStitchInfo, Set stitching information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStitchingInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STITCH_INFO_s *pStitchInfo);
/**
* Get the stitching info of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pStitchInfo, Get stitching information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStitchingInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STITCH_INFO_s *pStitchInfo);

/**
* Set the BurstTile info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pBurstTile, Set BurstTile
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetBurstTile(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_BURST_TILE_s *pBurstTile);
/**
* Get the BurstTile info of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pBurstTile, Get BurstTile
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetBurstTile(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_BURST_TILE_s *pBurstTile);

/**
* Set the front static black level of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeStaticBlc, Set front static black level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFeStaticBlc(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STATIC_BLC_LVL_s *pFeStaticBlc, UINT32 ExpIdx);
/**
* Get the front static black level of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFeStaticBlc, Get front static black level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFeStaticBlc(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STATIC_BLC_LVL_s *pFeStaticBlc, UINT32 ExpIdx);


/**
* Set the front wb gain of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeWbGain, Set front wb gain
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FE_WB_GAIN_s *pFeWbGain, const UINT32 ExpIdx);
/**
* Get the front wb gain of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFeWbGain, Get front wb gain
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FE_WB_GAIN_s *pFeWbGain, UINT32 ExpIdx);

/*CE filters*/
/**
* Set the contrast enhancement of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCe, Set contrast enhancement
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCe(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_s *pCe);
/**
* Get the contrast enhancement of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCe, Get contrast enhancement
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCe(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_s *pCe);


/**
* Set the contrast enhancement input table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCeInputTable, Set contrast enhancement  input table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCeInputTable(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_INPUT_TABLE_s *pCeInputTable);
/**
* Get the contrast enhancement input table of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCeInputTable, Get contrast enhancement  input table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCeInputTable(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_INPUT_TABLE_s *pCeInputTable);


/**
* Set the contrast enhancement output table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCeOutputTable, Set contrast enhancement  output table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCeOutputTable(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_OUT_TABLE_s *pCeOutTable);
/**
* Get the contrast enhancement output table of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pCeOutputTable, Get contrast enhancement  output table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCeOutputTable(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_OUT_TABLE_s *pCeOutTable);

/* HDR filters */

/**
* Set the HDR exposure number of each context
* @param [in]  pMode, Set ContextId
* @param [in]  ExpNum, Set HDR exposure number
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrExpNum(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 ExpNum);
/**
* Get the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pExpNum, Get HDR exposure number
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrExpNum(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pExpNum);


UINT32 AmbaIK_SetPipeMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 PipeMode);
UINT32 AmbaIK_GetPipeMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pPipeMode);

/**
* Set the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrBlend, Set HDR blend
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrBlend(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_BLEND_s *pHdrBlend);
/**
* Get the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pHdrBlend, Get HDR blend
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrBlend(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_BLEND_s *pHdrBlend);

/* 3a statistic*/

/**
* Set the AE statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAeStatInfo, Set  AE statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAeStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AE_STAT_INFO_s *pAeStatInfo);
/**
* Get the AE statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAeStatInfo, Get  AE statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAeStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AE_STAT_INFO_s *pAeStatInfo);


/**
* Set the Awb statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAwbStatInfo, Set Awb statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAwbStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AWB_STAT_INFO_s *pAwbStatInfo);
/**
* Get the Awb statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAwbStatInfo, Get Awb statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAwbStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AWB_STAT_INFO_s *pAwbStatInfo);


/**
* Set the CFA Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatInfo, Set Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);
/**
* Get the CFA Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAfStatInfo, Get Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);

/**
* Set the PG Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatInfo, Set Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPgAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);
/** 
* Get the PG Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pAfStatInfo, Get Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPgAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);

/**
* Set the cfa Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatExInfo, Set cfa Af statistic extended information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_EX_INFO_s *pAfStatExInfo);
/**
* Get the cfa Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatExInfo, Get cfa Af statistic extended information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_EX_INFO_s *pAfStatExInfo);


/**
* Set the pg Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pPgAfStatExInfo, Set pg Af statistic extended information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPgAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_PG_AF_STAT_EX_INFO_s *pPgAfStatExInfo);
/**
* Get the pg Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pPgAfStatExInfo, Get pg Af statistic extended information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPgAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_PG_AF_STAT_EX_INFO_s *pPgAfStatExInfo);


/**
* Set the cfa histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Set cfa histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);
/**
* Get the cfa histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Get cfa histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);

/**
* Set the pg histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Set pg histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPgHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);
/**
* Set the pg histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pHistInfo, Set pg histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPgHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);

/**
* Set the front tone curve of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeToneCurve, Set front tone curve
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFeToneCurve(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FE_TONE_CURVE_s *pFeToneCurve);
/**
* Get the front tone curve of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pFeToneCurve, Get front tone curve
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFeToneCurve(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FE_TONE_CURVE_s *pFeToneCurve);

/**
* Set the HDR raw offset of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrRawInfo, Set HDR raw offset
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrRawOffset(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo);
/**
* Get the HDR raw offset of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pHdrRawInfo, Get HDR raw offset
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrRawOffset(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo);

/**
* Set the y2y use cc enable info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pY2yUseCcEnableInfo, Set y2y use cc enable info
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetY2yUseCcEnableInfo(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 Y2yUseCcEnableInfo);
/**
* Get the y2y use cc enable info of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pY2yUseCcEnableInfo, Get y2y use cc enable info
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetY2yUseCcEnableInfo(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pY2yUseCcEnableInfo);

/**
* Set the pre cc gain of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pPreCcGain, Set pre cc gain
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPreCcGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_PRE_CC_GAIN_s *pPreCcGain);
/**
* Get the pre cc gain of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pPreCcGain, Get pre cc gain
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPreCcGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_PRE_CC_GAIN_s *pPreCcGain);

/**
* Set the external raw out mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  ExtRawOutMode, Set external raw out mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetExtRawOutMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 ExtRawOutMode);

/**
* Get the external raw out mode Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExtRawOutMode, Get external raw out mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetExtRawOutMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pExtRawOutMode);

#endif
