/*
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

#ifndef AMBA_IK_IMAGE_FILTER_INTERNAL_H
#define AMBA_IK_IMAGE_FILTER_INTERNAL_H

#include "AmbaDSP_ImageFilter.h"

typedef struct {
    // Warp part
    UINT32   Enable;
    UINT32   GridArrayWidth;
    UINT32   GridArrayHeight;
    UINT32   HorzGridSpacingExponent;
    UINT32   VertGridSpacingExponent;
    UINT32   VertWarpGridArrayWidth;
    UINT32   VertWarpGridArrayHeight;
    UINT32   VertWarpHorzGridSpacingExponent;
    UINT32   VertWarpVertGridSpacingExponent;
    INT16   *pWarpHorizontalTable;
    INT16   *pWarpVerticalTable;
} AMBA_IK_WARP_INTERNAL_INFO_s;

typedef struct {
    UINT32  HorzWarpEnable;
    UINT32  VertWarpEnable;
    UINT32   HorzPassGridArrayWidth;
    UINT32   HorzPassGridArrayHeight;
    UINT32   HorzPassHorzGridSpacingExponent;
    UINT32   HorzPassVertGridSpacingExponent;
    UINT32   VertPassGridArrayWidth;
    UINT32   VertPassGridArrayHeight;
    UINT32   VertPassHorzGridSpacingExponent;
    UINT32   VertPassVertGridSpacingExponent;
    INT16   WarpHorzTableRed[1536];
    INT16   WarpVertTableRed[1536];
    INT16   WarpHorzTableBlue[1536];
    INT16   WarpVertTableBlue[1536];
} AMBA_IK_CAWARP_INTERNAL_INFO_s;

typedef struct {
    UINT32 Enable;
} AMBA_IK_DEFERRED_BLC_s;

typedef struct {
    UINT32 LevelRed;
    UINT32 LevelGreenEven;
    UINT32 LevelGreenOdd;
    UINT32 LevelBlue;
    UINT32 PreLevelRed; // 0:16383
    UINT32 PreLevelGreenEven; // 0:16383
    UINT32 PreLevelGreenOdd; // 0:16383
    UINT32 PreLevelBule; // 0:16383
    UINT32 PostLevelRed; // 0:16383
    UINT32 PostLevelGreenEven; // 0:16383
    UINT32 PostLevelGreenOdd; // 0:16383
    UINT32 PostLevelBule; // 0:16383
} AMBA_IK_DGAIN_SAT_LVL_INTERNAL_s;
/*
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
    UINT32 MotionResponse;
    UINT32 NoiseBase;
    UINT32 SlowMoSensitivity;
} AMBA_IK_MOTION_DETECT_s;
*/

typedef struct {
    /* normal filters */
    AMBA_IK_VIN_SENSOR_INFO_s VinSensorInfo;
    AMBA_IK_WB_GAIN_s BeforeCeWbGain;
    AMBA_IK_WB_GAIN_s AfterCeWbGain;
    AMBA_IK_CFA_LEAKAGE_FILTER_s CfaLeakageFilter;
    AMBA_IK_GRGB_MISMATCH_s GrgbMismatch;
    AMBA_IK_ANTI_ALIASING_s AntiAliasing;
    AMBA_IK_DYNAMIC_BAD_PXL_COR_s DynamicBadPixelCorrection;
    AMBA_IK_CFA_NOISE_FILTER_s CfaNoiseFilter;
    AMBA_IK_DEMOSAIC_s Demosaic;
    AMBA_IK_COLOR_CORRECTION_REG_s ColorCorrectionReg;
    AMBA_IK_COLOR_CORRECTION_s ColorCorrection;
    AMBA_IK_RGB_TO_YUV_MATRIX_s RgbToYuvMatrix;
    AMBA_IK_RGB_IR_s RgbIr;
    AMBA_IK_CHROMA_SCALE_s ChromaScale;
    AMBA_IK_CHROMA_FILTER_s ChromaFilter;
    AMBA_IK_CHROMA_MEDIAN_FILTER_s ChromaMedianFilter;
    AMBA_IK_FIRST_LUMA_PROC_MODE_s FirstLumaProcessingMode;
    AMBA_IK_ADV_SPAT_FLTR_s AdvanceSpatialFilter;
    AMBA_IK_FSTSHPNS_BOTH_s FirstSharpenNoiseBoth;
    AMBA_IK_FSTSHPNS_NOISE_s FirstSharpenNoiseNoise;
    AMBA_IK_FSTSHPNS_FIR_s FirstSharpenNoiseSharpenFir;
    AMBA_IK_FSTSHPNS_CORING_s FirstSharpenNoiseSharpenCoring;
    AMBA_IK_FSTSHPNS_COR_IDX_SCL_s FirstSharpenNoiseSharpenCoringIndexScale;
    AMBA_IK_FSTSHPNS_MIN_COR_RST_s FirstSharpenNoiseSharpenMinCoringResult;
    AMBA_IK_FSTSHPNS_MAX_COR_RST_s FirstSharpenNoiseSharpenMaxCoringResult;
    AMBA_IK_FSTSHPNS_SCL_COR_s FirstSharpenNoiseSharpenScaleCoring;
    AMBA_IK_FNLSHPNS_BOTH_s FinalSharpenNoiseBoth;
    AMBA_IK_FNLSHPNS_NOISE_s FinalSharpenNoiseNoise;
    AMBA_IK_FNLSHPNS_FIR_s FinalSharpenNoiseSharpenFir;
    AMBA_IK_FNLSHPNS_CORING_s FinalSharpenNoiseSharpenCoring;
    AMBA_IK_FNLSHPNS_COR_IDX_SCL_s FinalSharpenNoiseSharpenCoringIndexScale;
    AMBA_IK_FNLSHPNS_MIN_COR_RST_s FinalSharpenNoiseSharpenMinCoringResult;
    AMBA_IK_FNLSHPNS_MAX_COR_RST_s FinalSharpenNoiseSharpenMaxCoringResult;
    AMBA_IK_FNLSHPNS_SCL_COR_s FinalSharpenNoiseSharpenScaleCoring;
    AMBA_IK_FNLSHPNS_BOTH_TDT_s FinalSharpenNoiseBothThreeDTable;
    AMBA_IK_VIDEO_MCTF_s VideoMctf;
    AMBA_IK_VIDEO_MCTF_TA_s VideoMctfTemporalAdjust;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s VideoMctfTemporalAdjustY;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s VideoMctfTemporalAdjustCb;
    AMBA_IK_VIDEO_MCTF_TA_Y_CB_CR_s VideoMctfTemporalAdjustCr;
    AMBA_IK_POS_DSP33_t VideoMctfAndFinalSarpen;
    AMBA_IK_MOTION_DETECT_s MotionDetect;
    AMBA_IK_MOTION_DETECT_AND_MCTF_s MotionDetectAndMctf;

    /* calibration */
    AMBA_IK_DZOOM_INFO_s DzoomInfo;
    AMBA_IK_WINDOW_SIZE_INFO_s WindowSizeInfo;
    //INT32 DummyMarginEnb;
    AMBA_IK_DUMMY_MARGIN_RANGE_s DummyMarginRange;
    AMBA_IK_WINDOW_GEOMETRY_s WindowGeometry;
    INT32 WarpEnb;
    INT32 CawarpEnb;
    INT32 StaticBpcEnb;
    INT32 VignetteEnb;
    AMBA_IK_WARP_INFO_s WarpInfo;
    AMBA_IK_CAWARP_INFO_s CawarpInfo;
    AMBA_IK_STATIC_BAD_PXL_COR_s StaticBpcInfo;
    AMBA_IK_VIGNETTE_s VignetteInfo;

    // Resampler
    AMBA_IK_RESAMPLER_STR_s ResamplerStr;

    /* front end */
    AMBA_IK_FE_TONE_CURVE_s FeToneCurve;
    AMBA_IK_STATIC_BLC_LVL_s FeStaticBlc[3];
    AMBA_IK_WB_GAIN_s FeWbGain[3];
    AMBA_IK_CE_s ContrastEnhance;
    AMBA_IK_CE_INPUT_TABLE_s ContrastEnhanceInTable;
    AMBA_IK_CE_OUT_TABLE_s ContrastEnhanceOutTable;

    /* HDR structure */
    AMBA_IK_HDR_BLEND_s HdrBlend;

    /* 3a statistic */
    AMBA_IK_AAA_STAT_INFO_s AaaStatInfo;
    AMBA_IK_AF_STAT_EX_INFO_s AfStatExInfo;
    AMBA_IK_PG_AF_STAT_EX_INFO_s PgAfStatExInfo;
} AMBA_IK_USER_PARAMETERS_s;


UINT32 AmbaIK_SetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal);
UINT32 AmbaIK_GetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal);
UINT32 AmbaIK_SetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal);
UINT32 AmbaIK_GetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal);
UINT32 AmbaIK_SetStaticBpcHLMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 StaticBpcHLMode);
UINT32 AmbaIK_GetStaticBpcHLMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pStaticBpcHLMode);
UINT32 AmbaIK_SetDeferredBlcLvl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DEFERRED_BLC_s *pDeferredBlcLvl);
UINT32 AmbaIK_GetDeferredBlcLvl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DEFERRED_BLC_s *pDeferredBlcLvl);
/**
* Set the motion detect setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDeferredBlcLvl, Set motion detect setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
//UINT32 AmbaIK_SetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_MOTION_DETECT_s *pMotionDetect);
/**
* Get the motion detect setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDeferredBlcLvl, Get motion detect setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
//UINT32 AmbaIK_GetMotionDetect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_MOTION_DETECT_s *pMotionDetect);
/**
* Set the motion detect position dependency setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDeferredBlcLvl, Set motion detect position dependency setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
//UINT32 AmbaIK_SetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pMotionDetectPosDep);
/**
* Get the motion detect position dependency setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDeferredBlcLvl, Get motion detect position dependency setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
//UINT32 AmbaIK_GetMotionDetectPosDep(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pMotionDetectPosDep);
/**
* Transfer CFA statistic from hw
* @param [in]  In, 3a statistic data
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32  AmbaIK_TransferCFAAaaStatData(const void *In);
/**
* Transfer PG statistic from hw
* @param [in]  In, 3a statistic data
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32  AmbaIK_TransferPGAaaStatData(const void *In);
/**
* Transfer CFA histogram statistic from hw
* @param [in]  In, 3a statistic data
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32  AmbaIK_TransCFAHistStatData(const void *In);


/* Debug filters */
/**
* Set diagnal case id setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  DiagCaseId, Set diagnal case id setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_SetDiagCaseId(const AMBA_IK_MODE_CFG_s *pMode, const UINT8 DiagCaseId);
/**
* Dump debug data
* @param [in]  pMode, Set ContextId
* @param [in]  DumpFolder, Set dump path
* @param [in]  DumpLevel, Set dump level
* @param [in]  pBuffer, Set dump bufer
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
//UINT32 AmbaIK_DebugDump(const AMBA_IK_MODE_CFG_s *pMode, char *DumpFolder, INT32 DumpLevel, UINT8  *pBuffer);
/**
* Set warp debug level
* @param [in]  WarpDebugLevel, Set debug level
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_SetWarpDebugLevel(UINT32 WarpDebugLevel);
/**
* Set the fronted dgain statuation level of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeDGainSatLvl, Set fronted dgain statuation level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_SetFeDGainSatLvl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DGAIN_SAT_LVL_INTERNAL_s *pFeDGainSatLvl, UINT32 ExpIdx);
/**
* Get the fronted dgain statuation level of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeWbGain, Get fronted dgain statuation level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_GetFeDGainSatLvl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DGAIN_SAT_LVL_INTERNAL_s *pFeDGainSatLvl, UINT32 ExpIdx);
#endif
