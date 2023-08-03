/*
*  @file AmbaDSP_ImageFilter.h
*
* Copyright (c) 2020 Ambarella International LP
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
*/

#ifndef AMBA_IK_IMAGE_FILTER_H
#define AMBA_IK_IMAGE_FILTER_H



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

#ifndef IK_SBP_VER
#define IK_SBP_VER                  (0x20180401UL)
#endif
#ifndef IK_VIG_VER
#define IK_VIG_VER                  (0x20180401UL)
#endif
#ifndef IK_CA_VER
#define IK_CA_VER                   (0x20180401UL)
#endif
#ifndef IK_WARP_VER
#define IK_WARP_VER                 (0x20180401UL)
#endif
#ifndef IK_WARP_DISABLE
#define IK_WARP_DISABLE             (0)
#endif
#ifndef IK_WARP_ENABLE
#define IK_WARP_ENABLE              (1)
#endif
#ifndef IK_WARP_ENABLE_INTERNAL
#define IK_WARP_ENABLE_INTERNAL     (2)
#endif
#ifndef IK_WARP_VIDEO_MAXHORGRID
#define IK_WARP_VIDEO_MAXHORGRID    (128UL)
#endif
#ifndef IK_WARP_VIDEO_MAXVERGRID
#define IK_WARP_VIDEO_MAXVERGRID    (96UL)
#endif
#ifndef IK_WARP_MAX_SLICE_NUMBER
#define IK_WARP_MAX_SLICE_NUMBER    (12UL)
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
#define IK_FLIP_YUV_H               (IK_HORIZONTAL_FLIP<<16UL)
#endif
#ifndef IK_FLIP_RAW_V_YUV_H
#define IK_FLIP_RAW_V_YUV_H         ((IK_VERTICAL_FLIP) + (IK_HORIZONTAL_FLIP<<16UL))
#endif

#ifndef IK_RAW_COMPRESS_OFF
#define IK_RAW_COMPRESS_OFF     0UL
#define IK_RAW_COMPRESS_6P75    1UL
#define IK_RAW_COMPRESS_7p5     2UL
#define IK_RAW_COMPRESS_7p75    3UL
#define IK_RAW_COMPRESS_8p5     4UL
#define IK_RAW_COMPRESS_8p75    5UL
#define IK_RAW_COMPRESS_9p5     6UL
#define IK_RAW_COMPRESS_9p75    7UL
#define IK_RAW_COMPRESS_10p5    8UL
#define IK_RAW_COMPACT_8B       256UL
#define IK_RAW_COMPACT_10B     257UL
#define IK_RAW_COMPACT_12B     258UL
#define IK_RAW_COMPACT_14B     259UL
#endif

#define AMBA_IK_YUV420         (0x0U)
#define AMBA_IK_YUV422         (0x1U)


typedef struct {
    uint32_t ContextId;
} AMBA_IK_MODE_CFG_s;

typedef struct {
    uint32_t VinId;
    uint32_t SensorResolution;    /* Number of bits for data representation */
    uint32_t SensorPattern;       /* Bayer patterns RG, BG, GR, GB */
    uint32_t SensorMode;          /*0: normal 1:RGB-IR 2:RCCC*/
    uint32_t Compression;
    int32_t CompressionOffset;
    uint32_t SensorReadoutMode;
} AMBA_IK_VIN_SENSOR_INFO_s;

typedef struct {
    uint32_t GainR;
    uint32_t GainG;
    uint32_t GainB;
} AMBA_IK_WB_GAIN_s;

typedef struct {
    uint32_t Enable;
    uint32_t AlphaRr;
    uint32_t AlphaRb;
    uint32_t AlphaBr;
    uint32_t AlphaBb;
    uint32_t SaturationLevel;
} AMBA_IK_CFA_LEAKAGE_FILTER_s;

typedef struct {
    uint32_t NarrowEnable;
    uint32_t WideEnable;
    uint32_t WideSafety;
    uint32_t WideThresh;
} AMBA_IK_GRGB_MISMATCH_s;

typedef struct {
    uint32_t Enable;
    uint32_t Thresh;
    uint32_t LogFractionalCorrect;
} AMBA_IK_ANTI_ALIASING_s;

typedef struct {
    uint32_t Enable;
    uint32_t HotPixelStrength;
    uint32_t DarkPixelStrength;
    uint32_t CorrectionMethod;
    uint32_t HotPixelStrengthIRModeRedBlue;
    uint32_t DarkPixelStrengthIRModeRedBlue;
} AMBA_IK_DYNAMIC_BAD_PXL_COR_s;

typedef struct {
    uint32_t Enable;
    uint32_t ExtentFine[3];          /* R/G/B, 0-256 */
    uint32_t ExtentRegular[3];       /* R/G/B, 0-256 */
    uint32_t OriginalBlendStr[3];    /* R/G/B, 0-256 */
    uint32_t SelectivityFine;        /* 0-256 */
    uint32_t SelectivityRegular;     /* 0-256 */
    uint32_t StrengthFine[3];        /* R/G/B, 0-256 */
    uint32_t DirLevMax[3];
    uint32_t DirLevMin[3];
    uint32_t DirLevMul[3];
    int32_t  DirLevOffset[3];
    uint32_t DirLevShift[3];
    uint32_t LevMul[3];
    int32_t  LevOffset[3];
    uint32_t LevShift[3];
    uint32_t MaxNoiseLevel[3];
    uint32_t MinNoiseLevel[3];
    uint32_t ApplyToColorDiffRed;
    uint32_t ApplyToColorDiffBlue;
    uint32_t OptForRccbRed;
    uint32_t OptForRccbBlue;
    uint32_t DirectionalEnable;
    uint32_t DirectionalHorvertEdgeStrength;
    uint32_t DirectionalHorvertStrengthBias;
    uint32_t DirectionalHorvertOriginalBlendStrength;

} AMBA_IK_CFA_NOISE_FILTER_s;

typedef struct {
    uint32_t Enable;
    uint32_t ActivityThresh;
    uint32_t ActivityDifferenceThresh;
    uint32_t GradClipThresh;
    uint32_t GradNoiseThresh;
    uint32_t AliasInterpolationStrength;
    uint32_t AliasInterpolationThresh;
} AMBA_IK_DEMOSAIC_s;

typedef struct {
    uint32_t MatrixEn;
    uint32_t MatrixShiftMinus8;
    int32_t Matrix[6];
    uint32_t MatrixThreeDTable[IK_CC_3D_SIZE/4U];
} AMBA_IK_COLOR_CORRECTION_s;

typedef struct {
    uint32_t ToneCurveRed[IK_NUM_TONE_CURVE];
    uint32_t ToneCurveGreen[IK_NUM_TONE_CURVE];
    uint32_t ToneCurveBlue[IK_NUM_TONE_CURVE];
} AMBA_IK_TONE_CURVE_s;

typedef struct {
    int32_t MatrixValues[9];
    int32_t YOffset;
    int32_t UOffset;
    int32_t VOffset;
} AMBA_IK_RGB_TO_YUV_MATRIX_s;

typedef struct {
    uint32_t Enable;
    int32_t MatrixValues[3];
    int32_t YOffset;
} AMBA_IK_RGB_TO_12Y_s;

typedef struct {
    uint32_t Mode;
    int32_t IrcorrectOffsetB;
    int32_t IrcorrectOffsetGb;
    int32_t IrcorrectOffsetGr;
    int32_t IrcorrectOffsetR;
    uint32_t MulBaseVal;
    uint32_t MulDeltaHigh;
    uint32_t MulHigh;
    uint32_t MulHighVal;
    uint32_t IrOnly;
} AMBA_IK_RGB_IR_s;

typedef struct {
    uint32_t Enable;
    uint32_t GainCurve[IK_NUM_CHROMA_GAIN_CURVE];
} AMBA_IK_CHROMA_SCALE_s;

typedef struct {
    uint32_t Enable;
    uint32_t NoiseLevelCb;
    uint32_t NoiseLevelCr;
    uint32_t OriginalBlendStrengthCb;
    uint32_t OriginalBlendStrengthCr;
    uint32_t Radius;
} AMBA_IK_CHROMA_FILTER_s;

typedef struct {
    uint32_t Enable;
    uint32_t NoiseLevelCb;
    uint32_t NoiseLevelCr;
} AMBA_IK_WIDE_CHROMA_FILTER_s;

typedef struct {
    uint32_t T0Cb;
    uint32_t T0Cr;
    uint32_t T1Cb;
    uint32_t T1Cr;
    uint32_t AlphaMaxCb;
    uint32_t AlphaMaxCr;
    uint32_t AlphaMinCb;
    uint32_t AlphaMinCr;
    uint32_t MaxChangeCb;
    uint32_t MaxChangeCr;
} AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s;

typedef struct {
    uint32_t Enable;
    uint32_t CbAdaptiveStrength;
    uint32_t CrAdaptiveStrength;
    uint32_t CbNonAdaptiveStrength;
    uint32_t CrNonAdaptiveStrength;
    uint32_t CbAdaptiveAmount;
    uint32_t CrAdaptiveAmount;
} AMBA_IK_CHROMA_MEDIAN_FILTER_s;

typedef struct {
    uint32_t UseSharpenNotAsf;
} AMBA_IK_FIRST_LUMA_PROC_MODE_s;

typedef struct {
    uint32_t Specify;
    uint32_t PerDirFirIsoStrengths[9];
    uint32_t PerDirFirDirStrengths[9];
    uint32_t PerDirFirDirAmounts[9];
    int32_t Coefs[9][25];
    uint32_t StrengthIso;
    uint32_t StrengthDir;
    uint32_t WideEdgeDetect;
} AMBA_IK_ASF_FIR_s;

typedef struct {
    uint32_t Enable;
    uint32_t SensorWbB;
    uint32_t SensorWbG;
    uint32_t SensorWbR;
    uint32_t Strength0;
    uint32_t Strength1;
    uint32_t Strength2;
    uint32_t Strength2MaxChange;
} AMBA_IK_LUMA_NOISE_REDUCTION_s;

typedef struct {
    uint32_t AlphaMinUp;
    uint32_t AlphaMaxUp;
    uint32_t T0Up;
    uint32_t T1Up;
    uint32_t AlphaMinDown;
    uint32_t AlphaMaxDown;
    uint32_t T0Down;
    uint32_t T1Down;
} AMBA_IK_FULL_ADAPTATION_s;

typedef struct {
    uint32_t TemporalAlpha0; // 0-255
    uint32_t TemporalAlpha1; // 0-255
    uint32_t TemporalAlpha2; // 0-255
    uint32_t TemporalAlpha3; // 0-255
    uint32_t TemporalT0; // 0-63
    uint32_t TemporalT1; // 0-63
    uint32_t TemporalT2; // 0-63
    uint32_t TemporalT3; // 0-63
} AMBA_IK_VIDEO_MCTF_CURVE_s;

typedef struct {
    uint32_t Low;
    uint32_t LowDelta;
    uint32_t LowStrength;
    uint32_t MidStrength;
    uint32_t High;
    uint32_t HighDelta;
    uint32_t HighStrength;
} AMBA_IK_LEVEL_s;

typedef struct {
    uint32_t Low;
    uint32_t LowDelta;
    uint32_t LowStrength;
    uint32_t MidStrength;
    uint32_t High;
    uint32_t HighDelta;
    uint32_t HighStrength;
    uint32_t Method;
} AMBA_IK_LEVEL_METHOD_s;

typedef struct {
    uint32_t Enable;
    AMBA_IK_ASF_FIR_s Fir;
    uint32_t DirectionalDecideT0;
    uint32_t DirectionalDecideT1;
    AMBA_IK_FULL_ADAPTATION_s Adapt;
    AMBA_IK_LEVEL_s LevelStrAdjust;
    AMBA_IK_LEVEL_s T0T1Div;
    uint32_t MaxChangeNotT0T1Alpha;
    uint32_t MaxChangeUp;
    uint32_t MaxChangeDown;
} AMBA_IK_ADV_SPAT_FLTR_s;

typedef struct {
    uint32_t Up5x5;
    uint32_t Down5x5;
    uint32_t Up;
    uint32_t Down;
} AMBA_IK_MAX_CHANGE_s;

typedef struct {
    uint32_t Enable;
    uint32_t Mode;
    uint32_t EdgeThresh;
    uint32_t WideEdgeDetect;
    uint32_t MaxChangeUp5x5;
    uint32_t MaxChangeDown5x5;
} AMBA_IK_FSTSHPNS_BOTH_s;

typedef struct {
    uint32_t Enable;
    uint32_t Mode;
    uint32_t EdgeThresh;
    uint32_t WideEdgeDetect;
    AMBA_IK_MAX_CHANGE_s MaxChange;
    uint32_t NarrowNonsmoothDetectSub[17];
    uint32_t NarrowNonsmoothDetectShift;
} AMBA_IK_FNLSHPNS_BOTH_s;

typedef struct {
    uint32_t Specify;
    uint32_t PerDirFirIsoStrengths[9];
    uint32_t PerDirFirDirStrengths[9];
    uint32_t PerDirFirDirAmounts[9];
    int32_t Coefs[9][25];
    uint32_t StrengthIso;
    uint32_t StrengthDir;
} AMBA_IK_SHPNS_FIR_s;

#define AMBA_IK_FSTSHPNS_FIR_s AMBA_IK_SHPNS_FIR_s

typedef struct {
    uint32_t Specify;
    uint32_t PerDirFirIsoStrengths[9];
    uint32_t PerDirFirDirStrengths[9];
    uint32_t PerDirFirDirAmounts[9];
    int32_t Coefs[9][25];
    uint32_t StrengthIso;
    uint32_t StrengthDir;
    uint32_t PosFirArtifactReduceEnable;
    uint32_t PosFirArtifactReduceStrength;
    uint32_t OverShootReduceAllow;
    uint32_t OverShootReduceStrength;
    uint32_t UnderShootReduceAllow;
    uint32_t UnderShootReduceStrength;
} AMBA_IK_FNLSHPNS_FIR_s;

typedef struct {
    uint32_t Enable;
    uint32_t MaxChangeMethod;
    uint32_t MaxChangeTable[32];
    uint32_t NoiseLevelMethod;
    uint32_t NoiseLevelTable[32];
    uint32_t Size;
    uint32_t StrAMethod;
    uint32_t StrATable[32];
    uint32_t StrBMethod;
    uint32_t StrBTable[32];
} AMBA_IK_FNLSHPNS_ADV_ISO_s;

typedef struct {
    uint32_t MaxChangeUp;
    uint32_t MaxChangeDown;
    AMBA_IK_FSTSHPNS_FIR_s FirstShpFir;
    AMBA_IK_LEVEL_s LevelStrAdjust;
    uint32_t LevelStrAdjustNotT0T1LevelBased;
    uint32_t T0;
    uint32_t T1;
    uint32_t AlphaMin;
    uint32_t AlphaMax;
} AMBA_IK_FSTSHPNS_NOISE_s;

typedef struct {
    uint32_t MaxChangeUp;
    uint32_t MaxChangeDown;
    AMBA_IK_SHPNS_FIR_s FinalShpFir;
    AMBA_IK_LEVEL_METHOD_s LevelStrAdjust;
    uint32_t LevelStrAdjustNotT0T1LevelBased;
    uint32_t T0;
    uint32_t T1;
    uint32_t AlphaMin;
    uint32_t AlphaMax;
    uint32_t DirectionalDecideT0;
    uint32_t DirectionalDecideT1;
    AMBA_IK_FNLSHPNS_ADV_ISO_s AdvancedIso;
} AMBA_IK_FNLSHPNS_NOISE_s;

typedef struct {
    uint32_t   Coring[IK_NUM_CORING_TABLE_INDEX];
    uint32_t   FractionalBits;
} AMBA_IK_CORING_s;

typedef AMBA_IK_CORING_s AMBA_IK_FSTSHPNS_CORING_s;
typedef AMBA_IK_CORING_s AMBA_IK_FNLSHPNS_CORING_s;

typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_COR_IDX_SCL_s;

typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_MIN_COR_RST_s;

typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_MAX_COR_RST_s;

typedef AMBA_IK_LEVEL_s AMBA_IK_FSTSHPNS_SCL_COR_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_FNLSHPNS_SCL_COR_s;

// typedef struct {
//     UINT8 YToneOffset;
//     UINT8 YToneShift;
//     UINT8 UToneOffset;
//     UINT8 UToneShift;
//     UINT8 UToneBits;
//     UINT8 VToneOffset;
//     UINT8 VToneShift;
//     UINT8 VToneBits;
//     UINT8 ThreeD[IK_NUM_THREE_D_TABLE];
// } AMBA_IK_TABLE_INDEXING_s;

// typedef AMBA_IK_TABLE_INDEXING_s AMBA_IK_FNLSHPNS_BOTH_TDT_s;

typedef struct {
    uint32_t MaxChangeTable[32];
    uint32_t MaxChangeMethod;
    uint32_t Size;
    uint32_t NoiseLevel;
    uint32_t Enable;
} AMBA_IK_ADV_ISO_s;

typedef struct {
    uint32_t Enable; // 0,1
    uint32_t YNarrowNonsmoothDetectShift; // 0:5
    uint32_t YNarrowNonsmoothDetectSub[17]; // 0:255
    uint32_t UseLevelBasedTa;  // 0:1
    uint32_t CompressionEnable;  // 0:1
    //3d max change level
    AMBA_IK_LEVEL_METHOD_s Y3dMaxchange;
    AMBA_IK_LEVEL_METHOD_s Cb3dMaxchange;
    AMBA_IK_LEVEL_METHOD_s Cr3dMaxchange;
    //advanced iso
    uint32_t YAdvancedIsoEnable;
    uint32_t YAdvancedIsoMaxChangeMethod;
    uint32_t YAdvancedIsoMaxChangeTable[32];
    uint32_t YAdvancedIsoNoiseLevel;
    uint32_t YAdvancedIsoSize;
    uint32_t CbAdvancedIsoEnable;
    uint32_t CbAdvancedIsoMaxChangeMethod;
    uint32_t CbAdvancedIsoMaxChangeTable[32];
    uint32_t CbAdvancedIsoNoiseLevel;
    uint32_t CbAdvancedIsoSize;
    uint32_t CrAdvancedIsoEnable;
    uint32_t CrAdvancedIsoMaxChangeMethod;
    uint32_t CrAdvancedIsoMaxChangeTable[32];
    uint32_t CrAdvancedIsoNoiseLevel;
    uint32_t CrAdvancedIsoSize;
    //level based ta
    AMBA_IK_LEVEL_METHOD_s YLevelBasedTa;
    AMBA_IK_LEVEL_METHOD_s CbLevelBasedTa;
    AMBA_IK_LEVEL_METHOD_s CrLevelBasedTa;
    //overall max change level
    AMBA_IK_LEVEL_METHOD_s YOverallMaxChange;
    AMBA_IK_LEVEL_METHOD_s CbOverallMaxChange;
    AMBA_IK_LEVEL_METHOD_s CrOverallMaxChange;
    //spat_blend level
    AMBA_IK_LEVEL_METHOD_s YSpatBlend;
    AMBA_IK_LEVEL_METHOD_s CbSpatBlend;
    AMBA_IK_LEVEL_METHOD_s CrSpatBlend;
    //spat filt max smth change level
    AMBA_IK_LEVEL_METHOD_s YSpatFiltMaxSmthChange;
    AMBA_IK_LEVEL_METHOD_s CbSpatFiltMaxSmthChange;
    AMBA_IK_LEVEL_METHOD_s CrSpatFiltMaxSmthChange;
    /*spat_smth_dir*/
    uint32_t YSpatSmthWideEdgeDetect; //0:8
    uint32_t YSpatSmthEdgeThresh; //0:2047
    AMBA_IK_LEVEL_METHOD_s YSpatSmthDir;
    uint32_t CbSpatSmthWideEdgeDetect;//0:8
    uint32_t CbSpatSmthEdgeThresh;//0:2047
    AMBA_IK_LEVEL_METHOD_s CbSpatSmthDir;
    uint32_t CrSpatSmthWideEdgeDetect;//0:8
    uint32_t CrSpatSmthEdgeThresh;//0:2047
    AMBA_IK_LEVEL_METHOD_s CrSpatSmthDir;
    /*spat_smth_iso*/
    AMBA_IK_LEVEL_METHOD_s YSpatSmthIso;
    AMBA_IK_LEVEL_METHOD_s CbSpatSmthIso;
    AMBA_IK_LEVEL_METHOD_s CrSpatSmthIso;
    uint32_t YSpatSmthDirectDecideT0;
    uint32_t YSpatSmthDirectDecideT1;
    uint32_t CbSpatSmthDirectDecideT0;
    uint32_t CbSpatSmthDirectDecideT1;
    uint32_t CrSpatSmthDirectDecideT0;
    uint32_t CrSpatSmthDirectDecideT1;
    //spatial max temporal level
    uint32_t YSpatialMaxChange; //0:255
    AMBA_IK_LEVEL_METHOD_s YSpatialMaxTemporal;
    uint32_t CbSpatialMaxChange;//0:255
    AMBA_IK_LEVEL_METHOD_s CbSpatialMaxTemporal;
    uint32_t CrSpatialMaxChange;//0:255
    AMBA_IK_LEVEL_METHOD_s CrSpatialMaxTemporal;
    AMBA_IK_VIDEO_MCTF_CURVE_s YCurve;
    uint32_t YTemporalMaxChange; // 0:255
    AMBA_IK_LEVEL_METHOD_s YTemporalMinTarget;
    AMBA_IK_VIDEO_MCTF_CURVE_s CbCurve;
    uint32_t CbTemporalMaxChange; // 0:255
    AMBA_IK_LEVEL_METHOD_s CbTemporalMinTarget;
    AMBA_IK_VIDEO_MCTF_CURVE_s CrCurve;
    uint32_t CrTemporalMaxChange; // 0:255
    AMBA_IK_LEVEL_METHOD_s CrTemporalMinTarget;
    uint32_t YTemporalGhostPrevent;
    uint32_t CbTemporalGhostPrevent;
    uint32_t CrTemporalGhostPrevent;
    uint32_t CompressionBitRateLuma;
    uint32_t CompressionBitRateChroma;
    uint32_t CompressionDitherDisable;
    uint32_t YTemporalArtifactGuard;
    uint32_t CbTemporalArtifactGuard;
    uint32_t CrTemporalArtifactGuard;
    uint32_t YCombineStrength; // 0:256
    uint32_t YStrength3d; // 0:256
    uint32_t YSpatialWeighting[64]; // 0:255
    uint32_t CbStrength3d; // 0:256
    uint32_t CbSpatialWeighting[64]; // 0:255
    uint32_t CrStrength3d; // 0:256
    uint32_t CrSpatialWeighting[64]; // 0:255

    // video_mctf_level
    uint32_t YTemporalMaxChangeNotT0T1LevelBased; // 0:1
    AMBA_IK_LEVEL_METHOD_s YTemporalEitherMaxChangeOrT0T1Add;
    uint32_t CbTemporalMaxChangeNotT0T1LevelBased; // 0:1
    AMBA_IK_LEVEL_METHOD_s CbTemporalEitherMaxChangeOrT0T1Add;
    uint32_t CrTemporalMaxChangeNotT0T1LevelBased; // 0:1
    AMBA_IK_LEVEL_METHOD_s CrTemporalEitherMaxChangeOrT0T1Add;
} AMBA_IK_VIDEO_MCTF_s;

typedef struct {
    uint32_t Max;
    uint32_t Min;
    uint32_t MotionResponse;
    uint32_t NoiseBase;
    uint32_t StillThresh;
} AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s;

typedef struct {
    uint32_t SlowMoSensitivity;
    uint32_t ScoreNoiseRobust;
    uint32_t LevAdjustLow;
    uint32_t LevAdjustLowDelta;
    uint32_t LevAdjustLowStrength;
    uint32_t LevAdjustMidStrength;
    uint32_t LevAdjustHigh;
    uint32_t LevAdjustHighDelta;
    uint32_t LevAdjustHighStrength;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s YMctfTa;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s CbMctfTa;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s CrMctfTa;
} AMBA_IK_VIDEO_MCTF_TA_s;

// typedef struct {
//     UINT8 PosDep[33][33];
// } AMBA_IK_POS_DSP33_t;

typedef struct {
    uint32_t Enable;
    uint32_t ZoomX;
    uint32_t ZoomY;
    int32_t ShiftX;
    int32_t ShiftY;
} AMBA_IK_DZOOM_INFO_s;

typedef struct {
    uint32_t StartX;
    uint32_t StartY;
    uint32_t Width;
    uint32_t Height;
} AMBA_IK_WINDOW_GEOMETRY_s;

typedef struct {
    uint32_t FactorNum;              /* subsamping factor (numerator) */
    uint32_t FactorDen;              /* subsamping factor (denominator) */
} AMBA_IK_SENSOR_SUBSAMPLING_s;

typedef struct {
    uint32_t StartX;     // Unit in pixel. Before downsample.
    uint32_t StartY;     // Unit in pixel. Before downsample.
    uint32_t Width;      // Unit in pixel. After downsample.
    uint32_t Height;     // Unit in pixel. After downsample.
    AMBA_IK_SENSOR_SUBSAMPLING_s HSubSample;
    AMBA_IK_SENSOR_SUBSAMPLING_s VSubSample;
} AMBA_IK_VIN_SENSOR_GEOMETRY_s;

typedef struct {
    uint32_t Width; /* Unit in pixel*/
    uint32_t Height;
} AMBA_IK_WINDOW_DIMENSION_s;

typedef struct {
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensor;
    AMBA_IK_WINDOW_DIMENSION_s Main;
    AMBA_IK_WINDOW_DIMENSION_s Prev[3];
    AMBA_IK_WINDOW_DIMENSION_s Thumbnail;
    AMBA_IK_WINDOW_DIMENSION_s Screennail;
} AMBA_IK_WINDOW_SIZE_INFO_s;

typedef struct {
    uint32_t Enable;
    uint32_t Left;
    uint32_t Right;
    uint32_t Top;
    uint32_t Bottom;
} AMBA_IK_DUMMY_MARGIN_RANGE_s;

typedef struct {
    uint32_t Enable;
    AMBA_IK_WINDOW_GEOMETRY_s  ActiveGeo;
} AMBA_IK_VIN_ACTIVE_WINDOW_s;

// typedef struct {
//     INT16 X;
//     INT16 Y;
// } AMBA_IK_GRID_POINT_s;

// typedef struct {
//     uint32_t Version;
//     uint32_t HorGridNum;          /* Horizontal grid number. Range to 256 as maximum. */
//     uint32_t VerGridNum;          /* Vertical grid number. Range to 192 as maximum. */
//     uint32_t TileWidthExp;        /* 3:8, 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
//     uint32_t TileHeightExp;       /* 3:8, 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
//     AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;   /* Vin sensor geometry when calibrating. */
//     uint32_t Enb_2StageCompensation : 1; /*1:enable, 0 disable*/
//     uint32_t Reserved : 31;       /* Reserved for extention. */
//     AMBA_IK_GRID_POINT_s *pWarp;         /* Warp grid vector array. */
// } AMBA_IK_WARP_INFO_s;

typedef struct  {
    uint32_t Enable;   //0, 1
    uint32_t TileNum_x;           // 1~4
    uint32_t TileNum_y;           // 1~4
} AMBA_IK_STITCH_INFO_s;

// typedef struct {
//     uint32_t Version;
//     uint32_t HorGridNum;          /* Horizontal grid number. Range to 64 as maximum. */
//     uint32_t VerGridNum;          /* Vertical grid number. Range to 96 as maximum. */
//     uint32_t TileWidthExp;        /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
//     uint32_t TileHeightExp;       /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
//     AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;   /* Vin sensor geometry when calibrating. */
//     uint32_t Enb2StageCompensation : 1;
//     uint32_t Reserved : 31;       /* Reserved for extention. */
//     AMBA_IK_GRID_POINT_s *pCawarpRed;    /* Cawarp grid vector array. */
//     AMBA_IK_GRID_POINT_s *pCawarpBlue;   /* Cawarp grid vector array. */
// } AMBA_IK_CAWARP_INFO_s;

// typedef struct {
//     uint32_t Version;
//     UINT8 *pSbpBuffer;
// } AMBA_IK_CALIB_SBP_INFO_s;

// typedef struct {
//     AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
//     AMBA_IK_CALIB_SBP_INFO_s CalibSbpInfo;
// } AMBA_IK_STATIC_BAD_PXL_COR_s;

typedef struct {
    uint32_t Version;
    uint32_t NumRadialBinsCoarse;                  //coarse + fine <= 128
    uint32_t SizeRadialBinsCoarseLog;             //valid range 0~8
    uint32_t NumRadialBinsFine;
    uint32_t SizeRadialBinsFineLog;               //valid range 0~8
    uint32_t ModelCenterXR;                       // 4 individual bayer components optical center x, relative to calib window.
    uint32_t ModelCenterXGr;
    uint32_t ModelCenterXB;
    uint32_t ModelCenterXGb;
    uint32_t ModelCenterYR;                       // 4 individual bayer components optical center x, relative to calib window.
    uint32_t ModelCenterYGr;
    uint32_t ModelCenterYB;
    uint32_t ModelCenterYGb;
    uint32_t VigGainTblR[4][128];            // pointer to [4 bayer chan][4 direction][128 vertices] 0:9b mult 10:13b shift
    uint32_t VigGainTblGr[4][128];
    uint32_t VigGainTblB[4][128];
    uint32_t VigGainTblGb[4][128];
} AMBA_IK_CALIB_VIGNETTE_INFO_s;

typedef struct {
    uint32_t CalibModeEnable;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s  VinSensorGeo;
    AMBA_IK_CALIB_VIGNETTE_INFO_s  CalibVignetteInfo;
} AMBA_IK_VIGNETTE_s;

typedef struct {
    uint32_t DecompandEnable;
    uint32_t DecompandTable[IK_NUM_DECOMPAND_TABLE];
} AMBA_IK_FE_TONE_CURVE_s;

typedef struct {
    uint32_t Rgain; // 12.12 format
    uint32_t Ggain; // 12.12 format
    uint32_t Bgain; // 12.12 format
    uint32_t IrGain;
    uint32_t ShutterRatio;
} AMBA_IK_FE_WB_GAIN_s;

typedef struct {
    int32_t BlackR;
    int32_t BlackGr;
    int32_t BlackGb;
    int32_t BlackB;
    int32_t BlackG;
    int32_t BlackIr;
} AMBA_IK_STATIC_BLC_LVL_s;

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
    uint32_t InputEnable;
    uint32_t InputTable[IK_CE_INPUT_TABLE_SIZE];
} AMBA_IK_CE_INPUT_TABLE_s;

typedef struct {
    uint32_t OutputEnable;
    uint32_t OutputShift;
    uint32_t OutputTable[IK_CE_OUTPUT_TABLE_SIZE];
} AMBA_IK_CE_OUT_TABLE_s;

typedef struct {
    uint32_t Enable;
    uint32_t DeltaT0[4];
    uint32_t DeltaT1[4];
    uint32_t FlickerThreshold;
    uint32_t T0Offset[4];
    uint32_t T1Offset[4];
} AMBA_IK_HDR_BLEND_s;

typedef struct {
    uint32_t  CfaCutoffFreq; /* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    uint32_t  MainCutoffFreq;
} AMBA_IK_RESAMPLER_STR_s;

// 3A statistic structure
typedef struct {
    uint32_t  AeTileNumCol;
    uint32_t  AeTileNumRow;
    uint32_t  AeTileColStart;
    uint32_t  AeTileRowStart;
    uint32_t  AeTileWidth;
    uint32_t  AeTileHeight;
    uint32_t  AePixMinValue;
    uint32_t  AePixMaxValue;
} AMBA_IK_AE_STAT_INFO_s;

typedef struct {
    uint32_t  AwbTileNumCol;
    uint32_t  AwbTileNumRow;
    uint32_t  AwbTileColStart;
    uint32_t  AwbTileRowStart;
    uint32_t  AwbTileWidth;
    uint32_t  AwbTileHeight;
    uint32_t  AwbTileActiveWidth;
    uint32_t  AwbTileActiveHeight;
    uint32_t  AwbPixMinValue;
    uint32_t  AwbPixMaxValue;
} AMBA_IK_AWB_STAT_INFO_s;

typedef struct {
    uint32_t  AfTileNumCol;
    uint32_t  AfTileNumRow;
    uint32_t  AfTileColStart;
    uint32_t  AfTileRowStart;
    uint32_t  AfTileWidth;
    uint32_t  AfTileHeight;
    uint32_t  AfTileActiveWidth;
    uint32_t  AfTileActiveHeight;
} AMBA_IK_AF_STAT_INFO_s;

typedef struct {
    uint32_t AfHorizontalFilter1Mode;
    uint32_t AfHorizontalFilter1Stage1Enb;
    uint32_t AfHorizontalFilter1Stage2Enb;
    uint32_t AfHorizontalFilter1Stage3Enb;
    int32_t AfHorizontalFilter1Gain[7];
    uint32_t AfHorizontalFilter1Shift[4];
    uint32_t AfHorizontalFilter1BiasOff;
    uint32_t AfHorizontalFilter1Thresh;

    uint32_t AfHorizontalFilter2Mode;
    uint32_t AfHorizontalFilter2Stage1Enb;
    uint32_t AfHorizontalFilter2Stage2Enb;
    uint32_t AfHorizontalFilter2Stage3Enb;
    int32_t AfHorizontalFilter2Gain[7];
    uint32_t AfHorizontalFilter2Shift[4];
    uint32_t AfHorizontalFilter2BiasOff;
    uint32_t AfHorizontalFilter2Thresh;

    uint32_t AfTileFv1HorizontalShift;
    uint32_t AfTileFv2HorizontalShift;
} AMBA_IK_AF_STAT_EX_INFO_s;

typedef struct {
    uint32_t AfHorizontalFilter1Mode;
    uint32_t AfHorizontalFilter1Stage1Enb;
    uint32_t AfHorizontalFilter1Stage2Enb;
    uint32_t AfHorizontalFilter1Stage3Enb;
    int32_t AfHorizontalFilter1Gain[7];
    uint32_t AfHorizontalFilter1Shift[4];
    uint32_t AfHorizontalFilter1BiasOff;
    uint32_t AfHorizontalFilter1Thresh;
    uint32_t AfVerticalFilter1Thresh;

    uint32_t AfHorizontalFilter2Mode;
    uint32_t AfHorizontalFilter2Stage1Enb;
    uint32_t AfHorizontalFilter2Stage2Enb;
    uint32_t AfHorizontalFilter2Stage3Enb;
    int32_t AfHorizontalFilter2Gain[7];
    uint32_t AfHorizontalFilter2Shift[4];
    uint32_t AfHorizontalFilter2BiasOff;
    uint32_t AfHorizontalFilter2Thresh;
    uint32_t AfVerticalFilter2Thresh;

    uint32_t AfTileFv1HorizontalShift;
    uint32_t AfTileFv1VerticalShift;
    uint32_t AfTileFv1HorizontalWeight;
    uint32_t AfTileFv1VerticalWeight;
    uint32_t AfTileFv2HorizontalShift;
    uint32_t AfTileFv2VerticalShift;
    uint32_t AfTileFv2HorizontalWeight;
    uint32_t AfTileFv2VerticalWeight;
} AMBA_IK_PG_AF_STAT_EX_INFO_s;

typedef struct {
    uint32_t  AwbTileNumCol;
    uint32_t  AwbTileNumRow;
    uint32_t  AwbTileColStart;
    uint32_t  AwbTileRowStart;
    uint32_t  AwbTileWidth;
    uint32_t  AwbTileHeight;
    uint32_t  AwbTileActiveWidth;
    uint32_t  AwbTileActiveHeight;
    uint32_t  AwbPixMinValue;
    uint32_t  AwbPixMaxValue;
    uint32_t  AeTileNumCol;
    uint32_t  AeTileNumRow;
    uint32_t  AeTileColStart;
    uint32_t  AeTileRowStart;
    uint32_t  AeTileWidth;
    uint32_t  AeTileHeight;
    uint32_t  AePixMinValue;
    uint32_t  AePixMaxValue;
    uint32_t  AfTileNumCol;
    uint32_t  AfTileNumRow;
    uint32_t  AfTileColStart;
    uint32_t  AfTileRowStart;
    uint32_t  AfTileWidth;
    uint32_t  AfTileHeight;
    uint32_t  AfTileActiveWidth;
    uint32_t  AfTileActiveHeight;
    uint32_t  SixMinusCfaInputShift;
} AMBA_IK_AAA_STAT_INFO_s;

typedef struct {
    uint32_t  AeTileMask[16];
} AMBA_IK_HISTOGRAM_INFO_s;

typedef struct {
    uint32_t  FlipH;
    uint32_t  FlipV;
} AMBA_IK_FLIP_INFO_s;

typedef struct {
    uint32_t XOffset[3];
    uint32_t YOffset[3];
} AMBA_IK_HDR_RAW_INFO_s;

// TBR
// typedef struct {
//     uint32_t   Enb;
//     AMBA_IK_VIN_SENSOR_GEOMETRY_s  CurrentVinSensorGeo;
//     AMBA_IK_CALIB_SBP_INFO_s       CalibSbpInfo;
// } AMBA_IK_SBP_CORRECTION_s;

typedef struct {
    uint32_t   Enb;
    uint32_t   GainShift;
    uint32_t   VigStrengthEffectMode;
    uint32_t   Control;
    uint32_t  ChromaRatio;
    uint32_t  VigStrength;
    AMBA_IK_VIN_SENSOR_GEOMETRY_s  CurrentVinSensorGeo;
    AMBA_IK_CALIB_VIGNETTE_INFO_s  CalibVignetteInfo;
} AMBA_IK_VIGNETTE_CALC_INFO_s;

typedef struct {
    uint32_t DramEfficiency;
    uint32_t LumaWaitLines;
    uint32_t LumaDmaSize;
    uint32_t Reserved;
} AMBA_IK_WARP_BUFFER_INFO_s;

// Query
typedef struct {
    AMBA_IK_VIN_SENSOR_GEOMETRY_s VinSensorGeo;
    AMBA_IK_WINDOW_GEOMETRY_s VinActiveGeo;
    AMBA_IK_DZOOM_INFO_s DzoomInfo;
    AMBA_IK_DUMMY_MARGIN_RANGE_s DummyMarginRange;
    AMBA_IK_WINDOW_DIMENSION_s CfaWindow;
    AMBA_IK_WINDOW_DIMENSION_s MainWindow;
    AMBA_IK_WINDOW_DIMENSION_s PrevWindow[3];
    uint32_t DspVideoMode;
    uint32_t Fps;
} AMBA_IK_QUERY_IDSP_CLOCK_INFO_s;

typedef struct {
    uint32_t TheoreticalClock;
} AMBA_IK_QUERY_IDSP_CLOCK_RST_s;

typedef struct {
    uint32_t                          WorkBufSize;
    uint32_t                          OutBufSize;
} AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s;

// typedef struct {
//     uint32_t                          FlipMode;
//     uint32_t                          YuvMode;
//     AMBA_IK_VIN_SENSOR_INFO_s       *pSensorInfo;
//     AMBA_IK_RGB_IR_s                *pRgbIr;
//     AMBA_IK_AAA_STAT_INFO_s         *pAaaStatInfo;
//     AMBA_IK_AF_STAT_INFO_s          *pAaaPgAfStatInfo;
//     AMBA_IK_AF_STAT_EX_INFO_s       *pAfStatExInfo;
//     AMBA_IK_PG_AF_STAT_EX_INFO_s    *pPgAfStatExInfo;
//     AMBA_IK_HISTOGRAM_INFO_s        *pHistInfo;
//     AMBA_IK_HISTOGRAM_INFO_s        *pHistInfoPg;
//     int32_t                           WarpEnable;
//     uint32_t                          ChromaFilterRadius;
//     AMBA_IK_WINDOW_SIZE_INFO_s      *pWindowSizeInfo;   // M
//     AMBA_IK_VIN_ACTIVE_WINDOW_s     *pVinActiveWindow;  // O
//     AMBA_IK_DUMMY_MARGIN_RANGE_s    *pDmyRange;         // O
//     AMBA_IK_DZOOM_INFO_s            *pDzoomInfo;        // O
//     AMBA_IK_WARP_INFO_s             *pWarpInfo;
//     int32_t                           UseCawarp;
//     int32_t                           CawarpEnable;       // only take effect when use_cawarp = 1
//     AMBA_IK_CAWARP_INFO_s          *pCalibCaWarpInfo;   // only take effect when use_cawarp = 1
//     AMBA_IK_WARP_BUFFER_INFO_s      *pWarpBufInfo;
// } AMBA_IK_CALC_GEO_SETTINGS_IN_s;

// typedef struct {
//     AMBA_IK_CALC_GEO_SETTINGS_IN_s  *pIn;
//     void                            *pWorkBuf;
//     uint32_t                          WorkBufSize;
// } AMBA_IK_CALC_GEO_SETTINGS_s;

// typedef struct {
//     void                            *pOut;
//     uint32_t                          OutBufSize;
// } AMBA_IK_CALC_GEO_RESULT_s;

// typedef struct {
//     AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo;// Mandatory
//     AMBA_IK_VIN_ACTIVE_WINDOW_s *pActiveWindow;// Optional
//     AMBA_IK_DUMMY_MARGIN_RANGE_s *pDmyRange;// Optional
//     AMBA_IK_DZOOM_INFO_s *pDzoomInfo;// Optional
//     uint32_t FlipMode;// Mandatory
//     uint32_t Ability;// Mandatory
//     uint32_t WarpEnb;// Mandatory
//     uint32_t CawarpEnb;// Mandatory
//     AMBA_IK_WARP_INFO_s *pWarpInfo;// Optional
// } AMBA_IK_QUERY_WARP_INPAR_s;

// typedef struct {
//     AMBA_IK_QUERY_WARP_INPAR_s In;
//     void *pWorkBuf;//885988 bytes
//     uint32_t WorkBufSize;
// } AMBA_IK_QUERY_WARP_s;

// typedef struct {
//     uint32_t WaitLine;
//     uint32_t DmaSize;
//     uint32_t VerticalWarpVectorFlipNum;
//     uint32_t DmyBroken;
//     AMBA_IK_DUMMY_MARGIN_RANGE_s SuggestedDmyRange;
// } AMBA_IK_QUERY_WARP_RESULT_s;

// typedef struct {
//     AMBA_IK_WARP_INFO_s *pInputInfo;
//     void *pWorkingBuffer;
//     uint32_t WorkingBufferSize;
// } AMBA_IK_IN_WARP_PRE_PROC_s;

// typedef struct {
//     AMBA_IK_WARP_INFO_s *pResultInfo;
// } AMBA_IK_OUT_WARP_PRE_PROC_s;

// typedef struct {
//     AMBA_IK_CAWARP_INFO_s *pInpuIinfo;
//     void *pWorkingBuffer;
//     uint32_t WorkingBufferSize;
// } AMBA_IK_IN_CAWARP_PRE_PROC_s;

// typedef struct {
//     AMBA_IK_CAWARP_INFO_s *pResultInfo;
// } AMBA_IK_OUT_CAWARP_PRE_PROC_s;

// typedef struct {
//     AMBA_IK_HDR_RAW_INFO_s HdrRawInfo;
//     uint32_t Reserved[26];
// } AMBA_IK_QUERY_FRAME_INFO_s; // make it 128 alignment
#if 0
/**
* Set the Vin Sensor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Vin sensor information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVinSensorInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIN_SENSOR_INFO_s *pVinSensorInfo);

/**
* Get the Vin Sensor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVinSensorInfo, Get Vin sensor information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVinSensorInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIN_SENSOR_INFO_s *pVinSensorInfo);

/**
* Set the YUV Mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  YuvMode, Set yuv mode information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetYuvMode(const AMBA_IK_MODE_CFG_s *pMode, const uint32_t YuvMode);

/**
* Get the YUV Mode Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pYuvMode, Get yuv mode information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetYuvMode(const AMBA_IK_MODE_CFG_s *pMode, uint32_t *pYuvMode);

/**
* Set the before CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pBeforeCeWbGain, Set before CE WB gain information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetBeforeCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WB_GAIN_s *pBeforeCeWbGain);

/**
* Get the before CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pBeforeCeWbGain, Get before CE WB gain information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetBeforeCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WB_GAIN_s *pBeforeCeWbGain);

/**
* Set the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfterCeWbGain, Set after CE WB gain information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAfterCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WB_GAIN_s *pAfterCeWbGain);

/**
* Get the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfterCeWbGain, Get after CE WB gain information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAfterCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WB_GAIN_s *pAfterCeWbGain);

/**
* Set the CFA leakage filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaLeakageFilter, Set CFA leakage filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_LEAKAGE_FILTER_s *pCfaLeakageFilter);

/**
* Get the CFA leakage filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCfaLeakageFilter, Get CFA leakage filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_LEAKAGE_FILTER_s *pCfaLeakageFilter);

/**
* Set the Grgb Mismatch of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pGrgbMismatch, Set Grgb Mismatch filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetGrgbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_GRGB_MISMATCH_s *pGrgbMismatch);

/**
* Get the Grgb Mismatch of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pGrgbMismatch, Get Grgb Mismatch filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetGrgbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_GRGB_MISMATCH_s *pGrgbMismatch);

/**
* Set the Anti-Aliasing filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAntiAliasing, Set Anti-Aliasing filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ANTI_ALIASING_s *pAntiAliasing);

/**
* Get the Anti-Aliasing filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAntiAliasing, Get Anti-Aliasing filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ANTI_ALIASING_s *pAntiAliasing);

/**
* Set the dynamic bad pixel filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDynamicBadPxlCor, Set dynamic bad pixel filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetDynamicBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DYNAMIC_BAD_PXL_COR_s *pDynamicBadPxlCor);

/**
* Get the dynamic bad pixel filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDynamicBadPxlCor, Get dynamic bad pixel filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetDynamicBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DYNAMIC_BAD_PXL_COR_s *pDynamicBadPxlCor);

/**
* Set the CFA noise filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaNoiseFilter, Set CFA noise filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_NOISE_FILTER_s *pCfaNoiseFilter);

/**
* Get the CFA noise filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCfaNoiseFilter, Get CFA noise filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_NOISE_FILTER_s *pCfaNoiseFilter);

/**
* Set the demosaic filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDemosaic, Set demosaic filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetDemosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DEMOSAIC_s *pDemosaic);

/**
* Get the demosaic filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDemosaic, Get demosaic filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetDemosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DEMOSAIC_s *pDemosaic);

/**
* Set the rgb_to_12y filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbTo12Y, Set rgb_to_12y filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetRgbTo12Y(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_TO_12Y_s *pRgbTo12Y);

/**
* Get the rgb_to_12y filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pRgbTo12Y, Get rgb_to_12y filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetRgbTo12Y(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_TO_12Y_s *pRgbTo12Y);

/**
* Set the luma_noise_reduction filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pLumaNoiseReduction, Set luma_noise_reduction filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LUMA_NOISE_REDUCTION_s *pLumaNoiseReduction);

/**
* Get the luma_noise_reduction filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLumaNoiseReduction, Get luma_noise_reduction filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LUMA_NOISE_REDUCTION_s *pLumaNoiseReduction);

/**
* Set the color correction of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pColorCorrection, Set color correction
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetColorCorrection(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_COLOR_CORRECTION_s *pColorCorrection);

/**
* Get the color correction of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pColorCorrection, Get color correction
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetColorCorrection(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_COLOR_CORRECTION_s *pColorCorrection);

/**
* Set the tone curve filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pToneCurve, Set tone curve filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetToneCurve(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_TONE_CURVE_s *pToneCurve);

/**
* Get the tone curve filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pToneCurve, Get tone curve filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetToneCurve(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_TONE_CURVE_s *pToneCurve);

/**
* Set the RGB to YUV matrix of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbToYuvMatrix, Set RGB to YUV matrix
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetRgbToYuvMatrix(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_TO_YUV_MATRIX_s *pRgbToYuvMatrix);

/**
* Get the RGB to YUV matrix of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pRgbToYuvMatrix, Get RGB to YUV matrix
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetRgbToYuvMatrix(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_TO_YUV_MATRIX_s *pRgbToYuvMatrix);

/**
* Set the RgbIr setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbIr, Set RgbIr filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetRgbIr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_IR_s *pRgbIr);

/**
* Get the RgbIr setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pRgbIr, Get RgbIr filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetRgbIr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_IR_s *pRgbIr);

/**
* Set the chroma scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaScale, Set chroma scale
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetChromaScale(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_SCALE_s *pChromaScale);

/**
* Get the chroma scale of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaScale, Get chroma scale
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetChromaScale(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_SCALE_s *pChromaScale);

/**
* Set the chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaFilter, Set chroma filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_FILTER_s *pChromaFilter);

/**
* Get the chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaFilter, Get chroma filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_FILTER_s *pChromaFilter);

/**
* Set the wide chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWideChromaFilter, Set wide chroma filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetWideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WIDE_CHROMA_FILTER_s *pWideChromaFilter);

/**
* Get the wide chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWideChromaFilter, Get wide chroma filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetWideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WIDE_CHROMA_FILTER_s *pWideChromaFilter);

/**
* Set the wide chroma filter combine of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWideChromaFilterCombine, Set wide chroma filter combine
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetWideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s *pWideChromaFilterCombine);

/**
* Get the wide chroma filter combine of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWideChromaFilterCombine, Get wide chroma filter combine
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetWideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s *pWideChromaFilterCombine);

/**
* Set the chroma median filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaMedianFilter, Set chroma median filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_MEDIAN_FILTER_s *pChromaMedianFilter);

/**
* Get the chroma median filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaMedianFilter, Get chroma median filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_MEDIAN_FILTER_s *pChromaMedianFilter);

/**
* Set the first luma processing mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFirstLumaProcMode, Set first luma processing mode
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFirstLumaProcMode(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FIRST_LUMA_PROC_MODE_s *pFirstLumaProcMode);

/**
* Get the first luma processing mode of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFirstLumaProcMode, Get first luma processing mode
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFirstLumaProcMode(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FIRST_LUMA_PROC_MODE_s *pFirstLumaProcMode);

/**
* Set the advance spatial filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAdvSpatFltr, Set advance spatial filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAdvSpatFltr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ADV_SPAT_FLTR_s *pAdvSpatFltr);
/**
* Get the advance spatial filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAdvSpatFltr, Get advance spatial filter
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAdvSpatFltr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ADV_SPAT_FLTR_s *pAdvSpatFltr);

/**
* Set the first sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsBoth, Set first sharpen noise both
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_BOTH_s *pFstShpNsBoth);

/**
* Get the first sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsBoth, Get first sharpen noise both
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_BOTH_s *pFstShpNsBoth);

/**
* Set the first sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsFir, Set first sharpen noise fir
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_FIR_s *pFstShpNsFir);

/**
* Get the first sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsFir, Get first sharpen noise fir
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_FIR_s *pFstShpNsFir);

/**
* Set the first sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsNoise, Set first sharpen noise noise
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_NOISE_s *pFstShpNsNoise);

/**
* Get the first sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsNoise, Get first sharpen noise noise
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_NOISE_s *pFstShpNsNoise);

/**
* Set the first sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsCoring, Set first sharpen noise coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_CORING_s *pFstShpNsCoring);

/**
* Get the first sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsCoring, Get first sharpen noise coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_CORING_s *pFstShpNsCoring);

/**
* Set the first sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsCorIdxScl, Set first sharpen noise coring index scale
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *pFstShpNsCorIdxScl);

/**
* Get the first sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsCorIdxScl, Set first sharpen noise coring index scale
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *pFstShpNsCorIdxScl);

/**
* Set the first sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsMinCorRst, Set first sharpen noise min coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_MIN_COR_RST_s *pFstShpNsMinCorRst);

/**
* Get the first sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsMinCorRst, Get first sharpen noise min coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_MIN_COR_RST_s *pFstShpNsMinCorRst);

/**
* Set the first sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsMaxCorRst, Set first sharpen noise max coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_MAX_COR_RST_s *pFstShpNsMaxCorRst);

/**
* Get the first sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsMaxCorRst, Get first sharpen noise max coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_MAX_COR_RST_s *pFstShpNsMaxCorRst);

/**
* Set the first sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsSclCor, Set first sharpen noise scale coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFstShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_SCL_COR_s *pFstShpNsSclCor);

/**
* Get the first sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsSclCor, Get first sharpen noise scale coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFstShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_SCL_COR_s *pFstShpNsSclCor);

/**
* Set the final sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsBoth, Set final sharpen noise both
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_BOTH_s *pFnlShpNsBoth);

/**
* Get the final sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsBoth, Get final sharpen noise both
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_BOTH_s *pFnlShpNsBoth);

/**
* Set the final sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsFir, Set final sharpen noise fir
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_FIR_s *pFnlShpNsFir);

/**
* Get the final sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsFir, Get final sharpen noise fir
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_FIR_s *pFnlShpNsFir);

/**
* Set the final sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsNoise, Set final sharpen noise noise
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_NOISE_s *pFnlShpNsNoise);

/**
* Get the final sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsNoise, Get final sharpen noise noise
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_NOISE_s *pFnlShpNsNoise);

/**
* Set the final sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsCoring, Set final sharpen noise coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_CORING_s *pFnlShpNsCoring);

/**
* Get the final sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsCoring, Get final sharpen noise coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_CORING_s *pFnlShpNsCoring);

/**
* Set the final sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsCorIdxScl, Set final sharpen noise coring index scale
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *pFnlShpNsCorIdxScl);

/**
* Get the final sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsCorIdxScl, Set final sharpen noise coring index scale
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *pFnlShpNsCorIdxScl);

/**
* Set the final sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsMinCorRst, Set final sharpen noise min coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_MIN_COR_RST_s *pFnlShpNsMinCorRst);

/**
* Get the final sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsMinCorRst, Get final sharpen noise min coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_MIN_COR_RST_s *pFnlShpNsMinCorRst);

/**
* Set the final sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsMaxCorRst, Set final sharpen noise max coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_MAX_COR_RST_s *pFnlShpNsMaxCorRst);

/**
* Get the final sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsMaxCorRst, Get final sharpen noise max coring result
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_MAX_COR_RST_s *pFnlShpNsMaxCorRst);

/**
* Set the final sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsSclCor, Set final sharpen noise scale coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_SCL_COR_s *pFnlShpNsSclCor);

/**
* Get the final sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsSclCor, Get final sharpen noise scale coring
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_SCL_COR_s *pFnlShpNsSclCor);

/**
* Set the final sharpen noise both 3d table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsBothTdt, Set final sharpen noise both 3d table
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFnlShpNsBothTdt(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_BOTH_TDT_s *pFnlShpNsBothTdt);

/**
* Get the final sharpen noise both 3d table of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsBothTdt, Get final sharpen noise both 3d table
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFnlShpNsBothTdt(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_BOTH_TDT_s *pFnlShpNsBothTdt);

/**
* Set the video mctf of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctf, Set video mctf
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVideoMctf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIDEO_MCTF_s *pVideoMctf);

/**
* Get the video mctf of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVideoMctf, Get video mctf
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVideoMctf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIDEO_MCTF_s *pVideoMctf);

/**
* Set the video mctf temporal adjust of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctfTa, Set video mctf temporal adjust
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVideoMctfTa(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIDEO_MCTF_TA_s *pVideoMctfTa);

/**
* Get the video mctf temporal adjust of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVideoMctfTa, Get video mctf temporal adjust
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVideoMctfTa(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIDEO_MCTF_TA_s *pVideoMctfTa);

/**
* Set the video mctf and final sharpen of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctfAndFnlshp, Set video mctf and final sharpen
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVideoMctfAndFnlshp(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pVideoMctfAndFnlshp);

/**
* Get the video mctf and final sharpen of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVideoMctfAndFnlshp, Get video mctf and final sharpen
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVideoMctfAndFnlshp(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pVideoMctfAndFnlshp);

/**
* Set the dzoom information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDzoomInfo, Set dzoom information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetDzoomInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DZOOM_INFO_s *pDzoomInfo);

/**
* Get the dzoom information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDzoomInfo, Get dzoom information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetDzoomInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DZOOM_INFO_s *pDzoomInfo);

/**
* Set the window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWindowSizeInfo, Set window size information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo);

/**
* Get the window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWindowSizeInfo, Get window size information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo);

/**
* Set the vin window information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinActiveWin, Set vin window information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVinActiveWin(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIN_ACTIVE_WINDOW_s *pVinActiveWin);

/**
* Get the vin window information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVinActiveWin, Get vin window information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVinActiveWin(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIN_ACTIVE_WINDOW_s *pVinActiveWin);

/**
* Set the dummy margin range of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDummyMarginRange, Set dummy margin range
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetDummyMarginRange(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyMarginRange);

/**
* Get the dummy margin range of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDummyMarginRange, Get dummy margin range
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetDummyMarginRange(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyMarginRange);

/**
* Set the resamp strength of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pResamplerStr, Set resamp strength
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetResampStrength(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RESAMPLER_STR_s *pResamplerStr);

/**
* Get the resamp strength of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pResamplerStr, Get resamp strength
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetResampStrength(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RESAMPLER_STR_s *pResamplerStr);

/**
* Set the front tone curve of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeToneCurve, Set front tone curve
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFeToneCurve(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FE_TONE_CURVE_s *pFeToneCurve);

/**
* Get the front tone curve of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeToneCurve, Get front tone curve
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFeToneCurve(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FE_TONE_CURVE_s *pFeToneCurve);

/**
* Set the front static black level of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeStaticBlc, Set front static black level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFeStaticBlc(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STATIC_BLC_LVL_s *pFeStaticBlc, uint32_t ExpIdx);

/**
* Get the front static black level of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeStaticBlc, Get front static black level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFeStaticBlc(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STATIC_BLC_LVL_s *pFeStaticBlc, uint32_t ExpIdx);

/**
* Set the front wb gain of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeWbGain, Set front wb gain
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FE_WB_GAIN_s *pFeWbGain, const uint32_t ExpIdx);

/**
* Get the front wb gain of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeWbGain, Get front wb gain
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FE_WB_GAIN_s *pFeWbGain, uint32_t ExpIdx);

/* Calibration filters */
/**
* Set the warp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  WarpEnb, Set warp enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetWarpEnb(const AMBA_IK_MODE_CFG_s *pMode, const uint32_t WarpEnb);

/**
* Get the warp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWarpEnb, Get warp enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetWarpEnb(const AMBA_IK_MODE_CFG_s *pMode, uint32_t *pWarpEnb);

/**
* Set the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo, Set warp information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetWarpInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INFO_s *pWarpInfo);

/**
* Get the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo, Get warp information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetWarpInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INFO_s *pWarpInfo);

/**
* Set the cawarp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  CawarpEnb, Set cawarp enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCawarpEnb(const AMBA_IK_MODE_CFG_s *pMode, const uint32_t CawarpEnb);

/**
* Get the cawarp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCawarpEnb, Get cawarp enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCawarpEnb(const AMBA_IK_MODE_CFG_s *pMode, uint32_t *pCawarpEnb);

/**
* Set the cawarp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCawarpInfo, Set cawarp information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCawarpInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INFO_s *pCawarpInfo);

/**
* Get the cawarp information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCawarpInfo, Get cawarp information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCawarpInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INFO_s *pCawarpInfo);

/**
* Set the static bad pixel correction enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  StaticBpcEnb, Set static bad pixel correction enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetStaticBadPxlCorEnb(const AMBA_IK_MODE_CFG_s *pMode, const uint32_t StaticBpcEnb);

/**
* Get the static bad pixel correction enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pStaticBpcEnb, Get static bad pixel correction enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetStaticBadPxlCorEnb(const AMBA_IK_MODE_CFG_s *pMode, uint32_t *pStaticBpcEnb);

/**
* Set the static bad pixel correction information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStaticBadPxlCor, Set static bad pixel correction information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetStaticBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STATIC_BAD_PXL_COR_s *pStaticBadPxlCor);

/**
* Get the static bad pixel correction information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pStaticBadPxlCor, Get static bad pixel correction information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetStaticBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STATIC_BAD_PXL_COR_s *pStaticBadPxlCor);

/**
* Set the vignette enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  VignetteEnb, Set vignette enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVignetteEnb(const AMBA_IK_MODE_CFG_s *pMode, const uint32_t VignetteEnb);

/**
* Get the vignette enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVignetteEnb, Get vignette enable
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVignetteEnb(const AMBA_IK_MODE_CFG_s *pMode, uint32_t *pVignetteEnb);

/**
* Set the vignette information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVignette, Set vignette information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetVignette(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIGNETTE_s *pVignette);

/**
* Get the vignette information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVignette, Get vignette information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetVignette(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIGNETTE_s *pVignette);

/*CE filters*/
/**
* Set the contrast enhancement of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCe, Set contrast enhancement
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCe(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_s *pCe);

/**
* Get the contrast enhancement of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCe, Get contrast enhancement
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCe(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_s *pCe);

/**
* Set the contrast enhancement input table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCeInputTable, Set contrast enhancement  input table
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCeInputTable(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_INPUT_TABLE_s *pCeInputTable);

/**
* Get the contrast enhancement input table of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCeInputTable, Get contrast enhancement  input table
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCeInputTable(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_INPUT_TABLE_s *pCeInputTable);

/**
* Set the contrast enhancement output table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCeOutTable, Set contrast enhancement  output table
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetCeOutputTable(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_OUT_TABLE_s *pCeOutTable);

/**
* Get the contrast enhancement output table of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCeOutTable, Get contrast enhancement  output table
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetCeOutputTable(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_OUT_TABLE_s *pCeOutTable);

/* HDR filters */

/**
* Set the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrBlend, Set HDR blend
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetHdrBlend(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_BLEND_s *pHdrBlend);

/**
* Get the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHdrBlend, Get HDR blend
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetHdrBlend(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_BLEND_s *pHdrBlend);

/**
* Set the HDR raw offset of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrRawInfo, Set HDR raw offset
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetHdrRawOffset(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo);

/**
* Get the HDR raw offset of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHdrRawInfo, Get HDR raw offset
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetHdrRawOffset(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo);

/* 3a statistic*/

/**
* Set the AE statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAeStatInfo, Set  AE statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAeStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AE_STAT_INFO_s *pAeStatInfo);

/**
* Get the AE statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAeStatInfo, Get  AE statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAeStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AE_STAT_INFO_s *pAeStatInfo);

/**
* Set the Awb statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAwbStatInfo, Set Awb statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAwbStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AWB_STAT_INFO_s *pAwbStatInfo);

/**
* Get the Awb statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAwbStatInfo, Get Awb statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAwbStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AWB_STAT_INFO_s *pAwbStatInfo);

/**
* Set the CFA Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatInfo, Set Af statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);

/**
* Get the CFA Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfStatInfo, Get Af statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);

/**
* Set the PG Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatInfo, Set Af statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetPgAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);

/**
* Get the PG Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfStatInfo, Get Af statistic information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetPgAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_INFO_s *pAfStatInfo);

/**
* Set the cfa Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatExInfo, Set cfa Af statistic extended information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_EX_INFO_s *pAfStatExInfo);

/**
* Get the cfa Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatExInfo, Get cfa Af statistic extended information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_EX_INFO_s *pAfStatExInfo);

/**
* Set the pg Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pPgAfStatExInfo, Set pg Af statistic extended information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetPgAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_PG_AF_STAT_EX_INFO_s *pPgAfStatExInfo);

/**
* Get the pg Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pPgAfStatExInfo, Get pg Af statistic extended information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetPgAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_PG_AF_STAT_EX_INFO_s *pPgAfStatExInfo);

/**
* Set the cfa histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Set cfa histogram information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);

/**
* Get the cfa histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Get cfa histogram information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);

/**
* Set the pg histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Set pg histogram information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetPgHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);

/**
* Set the pg histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHistInfo, Set pg histogram information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetPgHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HISTOGRAM_INFO_s *pHistInfo);

/**
* Set the flip mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  FlipMode, Set Flip Mode
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetFlipMode(const AMBA_IK_MODE_CFG_s *pMode, const uint32_t FlipMode);

/**
* Get the flip mode of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFlipMode, Get Flip Mode
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetFlipMode(const AMBA_IK_MODE_CFG_s *pMode, uint32_t *pFlipMode);

/**
* Set the warp buffer info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpBufInfo, Set warp buffer information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetWarpBufferInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_BUFFER_INFO_s *pWarpBufInfo);

/**
* Get the warp buffer info of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWarpBufInfo, Get warp buffer information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetWarpBufferInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_BUFFER_INFO_s *pWarpBufInfo);

/**
* Set the stitching info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStitchInfo, Set stitching information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_SetStitchingInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STITCH_INFO_s *pStitchInfo);

/**
* Get the stitching info of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pStitchInfo, Get stitching information
* @return OK - success, NG - fail
*/
uint32_t AmbaIK_GetStitchingInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STITCH_INFO_s *pStitchInfo);


/*******************   Unsupportted IK APIs below :   *******************/
typedef struct {
    uint32_t Mode;
    uint32_t SumShift;
    UINT8 PixelWeight[4];
    UINT8 ToneCurve[32];
} AMBA_IK_AAA_PSEUDO_INFO_s;

uint32_t AmbaIK_SetPseudoYInfo(const AMBA_IK_MODE_CFG_s *pMode, const  AMBA_IK_AAA_PSEUDO_INFO_s *pPseudoYInfo);
uint32_t AmbaIK_GetPseudoYInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AAA_PSEUDO_INFO_s *pPseudoYInfo);
#endif
#endif
