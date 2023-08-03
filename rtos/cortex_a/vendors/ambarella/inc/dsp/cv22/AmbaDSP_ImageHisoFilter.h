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

#ifndef AMBA_IK_HISO_FILTER_H
#define AMBA_IK_HISO_FILTER_H

/**
 *  @file AmbaDSP_ImgHighIsoFilter.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Definitions & Constants for Ambarella DSP Image Kernel HISO APIs
 *
 */
 
#include "AmbaDSP_ImageFilter.h"
#define AMBA_DSP_IMG_NUM_CORING_TABLE_INDEX (256UL)
//Hi
typedef AMBA_IK_ANTI_ALIASING_s AMBA_IK_HI_ANTI_ALIASING_s;
typedef AMBA_IK_CFA_LEAKAGE_FILTER_s AMBA_IK_HI_CFA_LEAKAGE_FILTER_s;
typedef AMBA_IK_DYNAMIC_BAD_PXL_COR_s AMBA_IK_HI_DYNAMIC_BPC_s;
typedef AMBA_IK_GRGB_MISMATCH_s AMBA_IK_HI_GRGB_MISMATCH_s;
typedef AMBA_IK_CHROMA_MEDIAN_FILTER_s AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s;
typedef AMBA_IK_DEMOSAIC_s AMBA_IK_HI_DEMOSAIC_s;
typedef AMBA_IK_CFA_NOISE_FILTER_s AMBA_IK_HI_CFA_NOISE_FILTER_s;

//Li2
typedef AMBA_IK_ANTI_ALIASING_s AMBA_IK_LI2_ANTI_ALIASING_s;
typedef AMBA_IK_CFA_LEAKAGE_FILTER_s AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s;
typedef AMBA_IK_DYNAMIC_BAD_PXL_COR_s AMBA_IK_LI2_DYNAMIC_BPC_s;
typedef AMBA_IK_GRGB_MISMATCH_s AMBA_IK_LI2_GRGB_MISMATCH_s;
typedef AMBA_IK_DEMOSAIC_s AMBA_IK_LI2_DEMOSAIC_s;
typedef AMBA_IK_CFA_NOISE_FILTER_s AMBA_IK_LI2_CFA_NOISE_FILTER_s;

//ASF
typedef AMBA_IK_ADV_SPAT_FLTR_s AMBA_IK_HI_ASF_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_ASF_FIR_s Fir;
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    AMBA_IK_FULL_ADAPTATION_s Adapt;
    AMBA_IK_LEVEL_METHOD_s LevelStrAdjust;
    AMBA_IK_LEVEL_METHOD_s T0T1Div;
    UINT32 MaxChangeNotT0T1Alpha;
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
} AMBA_IK_HI_ASF_A_s;

typedef AMBA_IK_HI_ASF_s AMBA_IK_LI2_ASF_s;
typedef AMBA_IK_HI_ASF_A_s AMBA_IK_HI_MED1_ASF_s;
typedef AMBA_IK_HI_ASF_A_s AMBA_IK_HI_HIGH2_ASF_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_ASF_FIR_s Fir;
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    AMBA_IK_FULL_ADAPTATION_s Adapt;
    AMBA_IK_LEVEL_s LevelStrAdjust;
    AMBA_IK_LEVEL_s T0T1Div;
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
} AMBA_IK_HI_ASF_B1_s;

typedef AMBA_IK_HI_ASF_B1_s AMBA_IK_HI_LOW_ASF_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_ASF_FIR_s Fir;
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    AMBA_IK_FULL_ADAPTATION_s Adapt;
    AMBA_IK_LEVEL_METHOD_s LevelStrAdjust;
    AMBA_IK_LEVEL_METHOD_s T0T1Div;
    UINT32 MaxChangeUp;
    UINT32 MaxChangeDown;
} AMBA_IK_HI_ASF_B2_s;

typedef AMBA_IK_HI_ASF_B2_s AMBA_IK_HI_MED2_ASF_s;
typedef AMBA_IK_HI_ASF_B2_s AMBA_IK_HI_HIGH_ASF_s;

typedef struct {
    UINT32 Enable;
    AMBA_IK_ASF_FIR_s Fir;
    UINT32 DirectionalDecideT0;
    UINT32 DirectionalDecideT1;
    UINT32 AlphaMin;
    UINT32 AlphaMax;
    UINT32 T0;
    UINT32 T1;
    AMBA_IK_LEVEL_s LevelStrAdjust;
    AMBA_IK_LEVEL_s T0T1Div;
    UINT32 MaxChangeNotT0T1Alpha;
    UINT32 MaxChange;
} AMBA_IK_CHROMA_ASF_s;

typedef AMBA_IK_CHROMA_ASF_s AMBA_IK_HI_CHROMA_ASF_s;
typedef AMBA_IK_CHROMA_ASF_s AMBA_IK_HI_LOW_CHROMA_ASF_s;

//SHP hiso high
typedef AMBA_IK_FSTSHPNS_BOTH_s AMBA_IK_HI_HIGH_SHPNS_BOTH_s;
typedef AMBA_IK_SHPNS_FIR_s AMBA_IK_HI_HIGH_SHPNS_FIR_s;
typedef AMBA_IK_CORING_s AMBA_IK_HI_HIGH_SHPNS_CORING_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s;

typedef struct {
    UINT32 MaxChangeUp; // 0:255
    UINT32 MaxChangeDown; // 0:255
    AMBA_IK_SHPNS_FIR_s SpatialFir;
    AMBA_IK_LEVEL_METHOD_s LevelStrAdjust;
    UINT32 LevelStrAdjustNotT0T1LevelBased; // 0:1
    UINT32 T0; // 0:255 t0 <= t1, t1 - t0 <= 15
    UINT32 T1; // 0:255
    UINT32 AlphaMin; // 0:16
    UINT32 AlphaMax; // 0:16
} AMBA_IK_HI_HIGH_SHPNS_NOISE_s;

//SHP hiso med
typedef AMBA_IK_FSTSHPNS_BOTH_s AMBA_IK_HI_MED_SHPNS_BOTH_s;
typedef AMBA_IK_SHPNS_FIR_s AMBA_IK_HI_MED_SHPNS_FIR_s;
typedef AMBA_IK_HI_HIGH_SHPNS_NOISE_s AMBA_IK_HI_MED_SHPNS_NOISE_s;
typedef AMBA_IK_CORING_s AMBA_IK_HI_MED_SHPNS_CORING_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_HI_MED_SHPNS_SCL_COR_s;

//SHP hiso low2
typedef AMBA_IK_SHPNS_FIR_s AMBA_IK_LI2_SHPNS_FIR_s;
typedef AMBA_IK_HI_HIGH_SHPNS_NOISE_s AMBA_IK_LI2_SHPNS_NOISE_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_LI2_SHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_LI2_SHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LEVEL_METHOD_s AMBA_IK_LI2_SHPNS_SCL_COR_s;

typedef struct {
    UINT32 Enable;
    UINT32 Mode;
    UINT32 EdgeThresh;
    UINT32 WideEdgeDetect;
    UINT32 MaxChangeUp5x5;
    UINT32 MaxChangeDown5x5;
    UINT32 MaxChangeUp; // 0:255
    UINT32 MaxChangeDown; // 0:255
} AMBA_IK_HI_SHPNS_BOTH_B_s;

typedef struct {
    UINT32 Coring[IK_NUM_CORING_TABLE_INDEX];
} AMBA_IK_HI_CORING_B_s;

typedef AMBA_IK_HI_SHPNS_BOTH_B_s AMBA_IK_LI2_SHPNS_BOTH_s;
typedef AMBA_IK_HI_CORING_B_s AMBA_IK_LI2_SHPNS_CORING_s;

//SHP hiso low high
typedef AMBA_IK_LI2_SHPNS_BOTH_s AMBA_IK_HILI_SHPNS_BOTH_s;
typedef AMBA_IK_LI2_SHPNS_FIR_s AMBA_IK_HILI_SHPNS_FIR_s;
typedef AMBA_IK_LI2_SHPNS_NOISE_s AMBA_IK_HILI_SHPNS_NOISE_s;
typedef AMBA_IK_LI2_SHPNS_CORING_s AMBA_IK_HILI_SHPNS_CORING_s;
typedef AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s;
typedef AMBA_IK_LI2_SHPNS_MIN_COR_RST_s AMBA_IK_HILI_SHPNS_MIN_COR_RST_s;
typedef AMBA_IK_LI2_SHPNS_MAX_COR_RST_s AMBA_IK_HILI_SHPNS_MAX_COR_RST_s;
typedef AMBA_IK_LI2_SHPNS_SCL_COR_s AMBA_IK_HILI_SHPNS_SCL_COR_s;

//chroma filter
typedef AMBA_IK_CHROMA_FILTER_s AMBA_IK_HI_CHROMA_FILTER_HIGH_s;

typedef struct {
    UINT32   Enable;
    UINT32   NoiseLevelCb;          /* 0-255 */
    UINT32   NoiseLevelCr;          /* 0-255 */
    UINT32  OriginalBlendStrengthCb; /* Cb 0-256  */
    UINT32  OriginalBlendStrengthCr; /* Cr 0-256  */
} AMBA_IK_HI_CHROMA_FILTER_B_s;

typedef AMBA_IK_HI_CHROMA_FILTER_B_s AMBA_IK_HI_CHROMA_FILTER_PRE_s;
typedef AMBA_IK_HI_CHROMA_FILTER_B_s AMBA_IK_HI_CHROMA_FILTER_MED_s;
typedef AMBA_IK_HI_CHROMA_FILTER_B_s AMBA_IK_HI_CHROMA_FILTER_LOW_s;
typedef AMBA_IK_HI_CHROMA_FILTER_B_s AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s;

// combine
typedef struct {
    UINT32 T0;
    UINT32 T1;
    UINT32 AlphaMax;
    UINT32 AlphaMin;
    UINT32 SignalPreserve;
    AMBA_IK_LEVEL_METHOD_s T0T1Add;
    AMBA_IK_LEVEL_METHOD_s MaxChange;
} AMBA_IK_HI_LUMA_COMBINE_s;

typedef AMBA_IK_HI_LUMA_COMBINE_s AMBA_IK_HI_LOW_ASF_COMBINE_s;

typedef struct {
    UINT32 T0Cb;
    UINT32 T0Cr;
    UINT32 T1Cb;
    UINT32 T1Cr;
    UINT32 AlphaMaxCb;
    UINT32 AlphaMaxCr;
    UINT32 AlphaMinCb;
    UINT32 AlphaMinCr;
    AMBA_IK_LEVEL_s MaxChangeLevelCb;
    AMBA_IK_LEVEL_s MaxChangeLevelCr;
    AMBA_IK_LEVEL_s T0T1AddLevelCb;
    AMBA_IK_LEVEL_s T0T1AddLevelCr;
    UINT32 SignalPreserveCb;
    UINT32 SignalPreserveCr;
} AMBA_IK_HI_CHROMA_FILTER_COMBINE_s;

typedef AMBA_IK_HI_CHROMA_FILTER_COMBINE_s AMBA_IK_HI_CHROMA_FLTR_MED_COM_s;
typedef AMBA_IK_HI_CHROMA_FILTER_COMBINE_s AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s;
typedef AMBA_IK_HI_CHROMA_FILTER_COMBINE_s AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s;

typedef struct {
    UINT32 T0Cb;
    UINT32 T0Cr;
    UINT32 T0Y;
    UINT32 T1Cb;
    UINT32 T1Cr;
    UINT32 T1Y;
    UINT32 AlphaMaxY;
    UINT32 AlphaMaxCb;
    UINT32 AlphaMaxCr;
    UINT32 AlphaMinY;
    UINT32 AlphaMinCb;
    UINT32 AlphaMinCr;
    AMBA_IK_LEVEL_METHOD_s MaxChangeLevelY;
    AMBA_IK_LEVEL_s    MaxChangeLevelCb;
    AMBA_IK_LEVEL_s    MaxChangeLevelCr;
    AMBA_IK_LEVEL_METHOD_s T0T1AddLevelY;
    AMBA_IK_LEVEL_s    T0T1AddLevelCb;
    AMBA_IK_LEVEL_s    T0T1AddLevelCr;
    UINT32 SignalPreserveCb;
    UINT32 SignalPreserveCr;
    UINT32 SignalPreserveY;
} AMBA_IK_HILI_COMBINE_s;

typedef struct {
    AMBA_IK_SHPNS_FIR_s      Fir;
    UINT32  MaxDown;
    UINT32  MaxUp;
    AMBA_IK_LEVEL_METHOD_s    ScaleLevel;
} AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s;

typedef struct {
    AMBA_IK_LEVEL_METHOD_s LumaLevel;
} AMBA_IK_HI_LUMA_BLEND_s;

typedef struct {
    UINT32 Smooth;          //0:443
    INT32 LevAdjust[16];    //-255:255
} AMBA_IK_HI_NONSMOOTH_DETECT_s;

typedef AMBA_IK_FIRST_LUMA_PROC_MODE_s AMBA_IK_HI_SELECT_s;

// Api
// Hi
UINT32 AmbaIK_SetHiAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_ANTI_ALIASING_s *pHiAntiAliasing);
UINT32 AmbaIK_GetHiAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_ANTI_ALIASING_s *pHiAntiAliasing);

UINT32 AmbaIK_SetHiCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *pHiCfaLeakageFilter);
UINT32 AmbaIK_GetHiCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *pHiCfaLeakageFilter);

UINT32 AmbaIK_SetHiDynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_DYNAMIC_BPC_s *pHiDynamicBPC);
UINT32 AmbaIK_GetHiDynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_DYNAMIC_BPC_s *pHiDynamicBPC);

UINT32 AmbaIK_SetHiGrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_GRGB_MISMATCH_s *pHiGrGbMismatch);
UINT32 AmbaIK_GetHiGrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_GRGB_MISMATCH_s *pHiGrGbMismatch);

UINT32 AmbaIK_SetHiChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *pHiChromaMedianFilter);
UINT32 AmbaIK_GetHiChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *pHiChromaMedianFilter);

UINT32 AmbaIK_SetHiCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CFA_NOISE_FILTER_s *pHiCfaNoiseFilter);
UINT32 AmbaIK_GetHiCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CFA_NOISE_FILTER_s *pHiCfaNoiseFilter);

UINT32 AmbaIK_SetHiDemosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_DEMOSAIC_s *pHiDemosaic);
UINT32 AmbaIK_GetHiDemosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_DEMOSAIC_s *pHiDemosaic);

//Li2
UINT32 AmbaIK_SetLi2AntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_ANTI_ALIASING_s *pLi2AntiAliasing);
UINT32 AmbaIK_GetLi2AntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_ANTI_ALIASING_s *pLi2AntiAliasing);

UINT32 AmbaIK_SetLi2CfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *pLi2CfaLeakageFilter);
UINT32 AmbaIK_GetLi2CfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *pLi2CfaLeakageFilter);

UINT32 AmbaIK_SetLi2DynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_DYNAMIC_BPC_s *pLi2DynamicBPC);
UINT32 AmbaIK_GetLi2DynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_DYNAMIC_BPC_s *pLi2DynamicBPC);

UINT32 AmbaIK_SetLi2GrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_GRGB_MISMATCH_s *pLi2GrGbMismatch);
UINT32 AmbaIK_GetLi2GrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_GRGB_MISMATCH_s *pLi2GrGbMismatch);

UINT32 AmbaIK_SetLi2CfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_CFA_NOISE_FILTER_s *pLi2CfaNoiseFilter);
UINT32 AmbaIK_GetLi2CfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_CFA_NOISE_FILTER_s *pLi2CfaNoiseFilter);

UINT32 AmbaIK_SetLi2Demosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_DEMOSAIC_s *pLi2Demosaic);
UINT32 AmbaIK_GetLi2Demosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_DEMOSAIC_s *pLi2Demosaic);

//ASF
UINT32 AmbaIK_SetHiAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_ASF_s *pHiAsf);
UINT32 AmbaIK_GetHiAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_ASF_s *pHiAsf);

UINT32 AmbaIK_SetLi2Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_ASF_s *pLi2Asf);
UINT32 AmbaIK_GetLi2Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_ASF_s *pLi2Asf);

UINT32 AmbaIK_SetHiLowAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LOW_ASF_s *pHiLowAsf);
UINT32 AmbaIK_GetHiLowAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LOW_ASF_s *pHiLowAsf);

UINT32 AmbaIK_SetHiMed1Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED1_ASF_s *pHiMed1Asf);
UINT32 AmbaIK_GetHiMed1Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED1_ASF_s *pHiMed1Asf);

UINT32 AmbaIK_SetHiMed2Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED2_ASF_s *pHiMed2Asf);
UINT32 AmbaIK_GetHiMed2Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED2_ASF_s *pHiMed2Asf);

UINT32 AmbaIK_SetHiHighAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_ASF_s *pHiHighAsf);
UINT32 AmbaIK_GetHiHighAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_ASF_s *pHiHighAsf);

UINT32 AmbaIK_SetHiHigh2Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH2_ASF_s *pHiHigh2Asf);
UINT32 AmbaIK_GetHiHigh2Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH2_ASF_s *pHiHigh2Asf);

UINT32 AmbaIK_SetChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_ASF_s *pChromaAsf);
UINT32 AmbaIK_GetChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_ASF_s *pChromaAsf);

UINT32 AmbaIK_SetHiChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_ASF_s *pHiChromaAsf);
UINT32 AmbaIK_GetHiChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_ASF_s *pHiChromaAsf);

UINT32 AmbaIK_SetHiLowChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LOW_CHROMA_ASF_s *pHiLowChromaAsf);
UINT32 AmbaIK_GetHiLowChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LOW_CHROMA_ASF_s *pHiLowChromaAsf);

//Sharpen
// --high--
UINT32 AmbaIK_SetHiHighShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_BOTH_s *pHiHighShpNsBoth);
UINT32 AmbaIK_GetHiHighShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_BOTH_s *pHiHighShpNsBoth);

UINT32 AmbaIK_SetHiHighShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_NOISE_s *pHiHighShpNsNoise);
UINT32 AmbaIK_GetHiHighShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_NOISE_s *pHiHighShpNsNoise);

UINT32 AmbaIK_SetHiHighShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_CORING_s *pHiHighShpNsCoring);
UINT32 AmbaIK_GetHiHighShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_CORING_s *pHiHighShpNsCoring);

UINT32 AmbaIK_SetHiHighShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_FIR_s *pHiHighShpNsFir);
UINT32 AmbaIK_GetHiHighShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_FIR_s *pHiHighShpNsFir);

UINT32 AmbaIK_SetHiHighShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *pHiHighShpNsCorIdxScl);
UINT32 AmbaIK_GetHiHighShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *pHiHighShpNsCorIdxScl);

UINT32 AmbaIK_SetHiHighShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *pHiHighShpNsMinCorRst);
UINT32 AmbaIK_GetHiHighShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *pHiHighShpNsMinCorRst);

UINT32 AmbaIK_SetHiHighShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *pHiHighShpNsMaxCorRst);
UINT32 AmbaIK_GetHiHighShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *pHiHighShpNsMaxCorRst);

UINT32 AmbaIK_SetHiHighShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *pHiHighShpNsSclCor);
UINT32 AmbaIK_GetHiHighShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *pHiHighShpNsSclCor);

// --med--
UINT32 AmbaIK_SetHiMedShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_BOTH_s *pHiMedShpNsBoth);
UINT32 AmbaIK_GetHiMedShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_BOTH_s *pHiMedShpNsBoth);

UINT32 AmbaIK_SetHiMedShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_NOISE_s *pHiMedShpNsNoise);
UINT32 AmbaIK_GetHiMedShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_NOISE_s *pHiMedShpNsNoise);

UINT32 AmbaIK_SetHiMedShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_CORING_s *pHiMedShpNsCoring);
UINT32 AmbaIK_GetHiMedShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_CORING_s *pHiMedShpNsCoring);

UINT32 AmbaIK_SetHiMedShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_FIR_s *pHiMedShpNsFir);
UINT32 AmbaIK_GetHiMedShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_FIR_s *pHiMedShpNsFir);

UINT32 AmbaIK_SetHiMedShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *pHiMedShpNsCorIdxScl);
UINT32 AmbaIK_GetHiMedShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *pHiMedShpNsCorIdxScl);

UINT32 AmbaIK_SetHiMedShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *pHiMedShpNsMinCorRst);
UINT32 AmbaIK_GetHiMedShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *pHiMedShpNsMinCorRst);

UINT32 AmbaIK_SetHiMedShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *pHiMedShpNsMaxCorRst);
UINT32 AmbaIK_GetHiMedShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *pHiMedShpNsMaxCorRst);

UINT32 AmbaIK_SetHiMedShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_SCL_COR_s *pHiMedShpNsSclCor);
UINT32 AmbaIK_GetHiMedShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_SCL_COR_s *pHiMedShpNsSclCor);

// --low2--
UINT32 AmbaIK_SetLi2ShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_BOTH_s *pLi2ShpNsBoth);
UINT32 AmbaIK_GetLi2ShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_BOTH_s *pLi2ShpNsBoth);

UINT32 AmbaIK_SetLi2ShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_NOISE_s *pLi2ShpNsNoise);
UINT32 AmbaIK_GetLi2ShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_NOISE_s *pLi2ShpNsNoise);

UINT32 AmbaIK_SetLi2ShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_CORING_s *pLi2ShpNsCoring);
UINT32 AmbaIK_GetLi2ShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_CORING_s *pLi2ShpNsCoring);

UINT32 AmbaIK_SetLi2ShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_FIR_s *pLi2ShpNsFir);
UINT32 AmbaIK_GetLi2ShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_FIR_s *pLi2ShpNsFir);

UINT32 AmbaIK_SetLi2ShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *pLi2ShpNsCorIdxScl);
UINT32 AmbaIK_GetLi2ShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *pLi2ShpNsCorIdxScl);

UINT32 AmbaIK_SetLi2ShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *pLi2ShpNsMinCorRst);
UINT32 AmbaIK_GetLi2ShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *pLi2ShpNsMinCorRst);

UINT32 AmbaIK_SetLi2ShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *pLi2ShpNsMaxCorRst);
UINT32 AmbaIK_GetLi2ShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *pLi2ShpNsMaxCorRst);

UINT32 AmbaIK_SetLi2ShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_SCL_COR_s *pLi2ShpNsSclCor);
UINT32 AmbaIK_GetLi2ShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_SCL_COR_s *pLi2ShpNsSclCor);

// --hili--
UINT32 AmbaIK_SetHiliShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_BOTH_s *pHiliShpNsBoth);
UINT32 AmbaIK_GetHiliShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_BOTH_s *pHiliShpNsBoth);

UINT32 AmbaIK_SetHiliShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_NOISE_s *pHiliShpNsNoise);
UINT32 AmbaIK_GetHiliShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_NOISE_s *pHiliShpNsNoise);

UINT32 AmbaIK_SetHiliShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_CORING_s *pHiliShpNsCoring);
UINT32 AmbaIK_GetHiliShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_CORING_s *pHiliShpNsCoring);

UINT32 AmbaIK_SetHiliShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_FIR_s *pHiliShpNsFir);
UINT32 AmbaIK_GetHiliShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_FIR_s *pHiliShpNsFir);

UINT32 AmbaIK_SetHiliShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *pHiliShpNsCorIdxScl);
UINT32 AmbaIK_GetHiliShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *pHiliShpNsCorIdxScl);

UINT32 AmbaIK_SetHiliShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *pHiliShpNsMinCorRst);
UINT32 AmbaIK_GetHiliShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *pHiliShpNsMinCorRst);

UINT32 AmbaIK_SetHiliShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *pHiliShpNsMaxCorRst);
UINT32 AmbaIK_GetHiliShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *pHiliShpNsMaxCorRst);

UINT32 AmbaIK_SetHiliShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_SCL_COR_s *pHiliShpNsSclCor);
UINT32 AmbaIK_GetHiliShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_SCL_COR_s *pHiliShpNsSclCor);

//chroma filter
UINT32 AmbaIK_SetHiChromaFilterHigh(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_HIGH_s *pHiChromaFilterHigh);
UINT32 AmbaIK_GetHiChromaFilterHigh(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_HIGH_s *pHiChromaFilterHigh);

UINT32 AmbaIK_SetHiChromaFilterPre(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_PRE_s *pHiChromaFilterPre);
UINT32 AmbaIK_GetHiChromaFilterPre(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_PRE_s *pHiChromaFilterPre);

UINT32 AmbaIK_SetHiChromaFilterMed(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_MED_s *pHiChromaFilterMed);
UINT32 AmbaIK_GetHiChromaFilterMed(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_MED_s *pHiChromaFilterMed);

UINT32 AmbaIK_SetHiChromaFilterLow(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_LOW_s *pHiChromaFilterLow);
UINT32 AmbaIK_GetHiChromaFilterLow(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_LOW_s *pHiChromaFilterLow);

UINT32 AmbaIK_SetHiChromaFilterVeryLow(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *pHiChromaFilterVeryLow);
UINT32 AmbaIK_GetHiChromaFilterVeryLow(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *pHiChromaFilterVeryLow);

//combine
UINT32 AmbaIK_SetHiLumaCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LUMA_COMBINE_s *pHiLumaCombine);
UINT32 AmbaIK_GetHiLumaCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LUMA_COMBINE_s *pHiLumaCombine);

UINT32 AmbaIK_SetHiLowASFCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LOW_ASF_COMBINE_s *pHiLowASFCombine);
UINT32 AmbaIK_GetHiLowASFCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LOW_ASF_COMBINE_s *pHiLowASFCombine);

UINT32 AmbaIK_SetHiChromaFltrMedCom(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *pHiChromaFltrMedCom);
UINT32 AmbaIK_GetHiChromaFltrMedCom(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *pHiChromaFltrMedCom);

UINT32 AmbaIK_SetHiChromaFltrLowCom(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *pHiChromaFltrLowCom);
UINT32 AmbaIK_GetHiChromaFltrLowCom(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *pHiChromaFltrLowCom);

UINT32 AmbaIK_SetHiChromaFltrVeryLowCom(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *pHiChromaFltrVeryLowCom);
UINT32 AmbaIK_GetHiChromaFltrVeryLowCom(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *pHiChromaFltrVeryLowCom);

UINT32 AmbaIK_SetHiliCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_COMBINE_s *pHiliCombine);
UINT32 AmbaIK_GetHiliCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_COMBINE_s *pHiliCombine);

UINT32 AmbaIK_SetHiMidHighFreqRecover(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *pHiMidHighFreqRecover);
UINT32 AmbaIK_GetHiMidHighFreqRecover(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *pHiMidHighFreqRecover);

UINT32 AmbaIK_SetHiLumaBlend(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LUMA_BLEND_s *pHiLumaBlend);
UINT32 AmbaIK_GetHiLumaBlend(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LUMA_BLEND_s *pHiLumaBlend);

UINT32 AmbaIK_SetHiNonSmoothDetect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_NONSMOOTH_DETECT_s *pHiNonSmoothDetect);
UINT32 AmbaIK_GetHiNonSmoothDetect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_NONSMOOTH_DETECT_s *pHiNonSmoothDetect);

UINT32 AmbaIK_SetHiSelect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_SELECT_s *pHiSelect);
UINT32 AmbaIK_GetHiSelect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_SELECT_s *pHiSelect);

#endif  /* _AMBA_IK_HISO_FILTER_H_ */
