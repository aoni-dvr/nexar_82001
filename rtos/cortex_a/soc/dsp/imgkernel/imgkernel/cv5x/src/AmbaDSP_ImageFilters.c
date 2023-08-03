/*
*  @file AmbaDSP_ImageFilters.c
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

#include "AmbaTypes.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgAdvancedFilter.h"
#include "AmbaDSP_ImageFilter.h"
#include "AmbaDSP_ImageFilterInternal.h"
#include "AmbaDSP_Image3aStatistics.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaWrap.h"
#include "AmbaDSP_IksFilter.h"

/**
* Set the Vin Sensor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Vin sensor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVinSensorInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIN_SENSOR_INFO_s *pVinSensorInfo)
{
    UINT32 rval = 0u;
    const ik_vin_sensor_info_t *p_ik_vin_sensor_info;

    (void)amba_ik_system_memcpy(&p_ik_vin_sensor_info, &pVinSensorInfo, sizeof(void *));
    rval |= ik_set_vin_sensor_info((UINT32)pMode->ContextId, p_ik_vin_sensor_info);

    return rval;
}

/**
* Get the Vin Sensor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVinSensorInfo, Get Vin sensor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVinSensorInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIN_SENSOR_INFO_s *pVinSensorInfo)
{
    ik_vin_sensor_info_t *p_ik_vin_sensor_info;

    (void)amba_ik_system_memcpy(pVinSensorInfo, pVinSensorInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_vin_sensor_info, &pVinSensorInfo, sizeof(void *));
    return ik_get_vin_sensor_info((UINT32)pMode->ContextId, p_ik_vin_sensor_info);
}

/**
* Set the YUV Mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  YuvMode, Set yuv mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetYuvMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 YuvMode)
{
    UINT32 rval = 0u;

    rval |= ik_set_yuv_mode((UINT32)pMode->ContextId, YuvMode);

    return rval;
}

/**
* Get the YUV Mode Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pYuvMode, Get yuv mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetYuvMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pYuvMode)
{
    return ik_get_yuv_mode((UINT32)pMode->ContextId, pYuvMode);
}

/**
* Set the before CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pBeforeCeWbGain, Set before CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetBeforeCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WB_GAIN_s *pBeforeCeWbGain)
{
    UINT32 rval = 0u;
    const ik_wb_gain_t *p_ik_before_ce_wb_gain;

    (void)amba_ik_system_memcpy(&p_ik_before_ce_wb_gain, &pBeforeCeWbGain, sizeof(void *));
    rval |= ik_set_before_ce_wb_gain((UINT32)pMode->ContextId, p_ik_before_ce_wb_gain);

    return rval;
}

/**
* Get the before CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pBeforeCeWbGain, Get before CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetBeforeCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WB_GAIN_s *pBeforeCeWbGain)
{
    ik_wb_gain_t *p_ik_before_ce_wb_gain;

    (void)amba_ik_system_memcpy(pBeforeCeWbGain, pBeforeCeWbGain, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_before_ce_wb_gain, &pBeforeCeWbGain, sizeof(void *));
    return ik_get_before_ce_wb_gain((UINT32)pMode->ContextId, p_ik_before_ce_wb_gain);
}

/**
* Set the pre cc gain of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pPreCcGain, Set pre cc gain
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPreCcGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_PRE_CC_GAIN_s *pPreCcGain)
{
    UINT32 rval = 0u;
    const ik_pre_cc_gain_t *pre_cc_gain;

    (void)amba_ik_system_memcpy(&pre_cc_gain, &pPreCcGain, sizeof(void *));
    rval |= ik_set_pre_cc_gain((UINT32)pMode->ContextId, pre_cc_gain);

    return rval;
}

/**
* Get the pre cc gain of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pPreCcGain, Get pre cc gain
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPreCcGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_PRE_CC_GAIN_s *pPreCcGain)
{
    ik_pre_cc_gain_t *pre_ik_cc_gain;

    (void)amba_ik_system_memcpy(pPreCcGain, pPreCcGain, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&pre_ik_cc_gain, &pPreCcGain, sizeof(void *));
    return ik_get_pre_cc_gain((UINT32)pMode->ContextId, pre_ik_cc_gain);
}

/**
* Set the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfterCeWbGain, Set after CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAfterCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WB_GAIN_s *pAfterCeWbGain)
{
    UINT32 rval = 0u;
    const ik_wb_gain_t *p_ik_after_ce_wb_gain;

    (void)amba_ik_system_memcpy(&p_ik_after_ce_wb_gain, &pAfterCeWbGain, sizeof(void *));
    rval |= ik_set_after_ce_wb_gain((UINT32)pMode->ContextId, p_ik_after_ce_wb_gain);

    return rval;
}

/**
* Get the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfterCeWbGain, Get after CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAfterCeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WB_GAIN_s *pAfterCeWbGain)
{
    ik_wb_gain_t *p_ik_after_ce_wb_gain;

    (void)amba_ik_system_memcpy(pAfterCeWbGain, pAfterCeWbGain, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_after_ce_wb_gain, &pAfterCeWbGain, sizeof(void *));
    return ik_get_after_ce_wb_gain((UINT32)pMode->ContextId, p_ik_after_ce_wb_gain);
}


/**
* Set the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfterCeWbGain, Set after CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStoreIr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STORED_IR_s *pStoreIr)
{
    UINT32 rval = 0u;
    const ik_stored_ir_t *p_stored_ir;

    (void)amba_ik_system_memcpy(&p_stored_ir, &pStoreIr, sizeof(void *));
    rval |= ik_set_stored_ir((UINT32)pMode->ContextId, p_stored_ir);

    return rval;
}

/**
* Get the after CE WB Gain's setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfterCeWbGain, Get after CE WB gain information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStoreIr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STORED_IR_s *pStoreIr)
{
    ik_stored_ir_t *p_ik_stored_ir;

    (void)amba_ik_system_memcpy(pStoreIr, pStoreIr, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_stored_ir, &pStoreIr, sizeof(void *));
    return ik_get_stored_ir((UINT32)pMode->ContextId, p_ik_stored_ir);
}


/**
* Set the CFA leakage filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaLeakageFilter, Set CFA leakage filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_LEAKAGE_FILTER_s *pCfaLeakageFilter)
{
    UINT32 rval = 0u;
    const ik_cfa_leakage_filter_t *p_ik_cfa_leakage_filter;

    (void)amba_ik_system_memcpy(&p_ik_cfa_leakage_filter, &pCfaLeakageFilter, sizeof(void *));
    rval |= ik_set_cfa_leakage_filter((UINT32)pMode->ContextId, p_ik_cfa_leakage_filter);

    return rval;
}

/**
* Get the CFA leakage filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCfaLeakageFilter, Get CFA leakage filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_LEAKAGE_FILTER_s *pCfaLeakageFilter)
{
    ik_cfa_leakage_filter_t *p_ik_cfa_leakage_filter;

    (void)amba_ik_system_memcpy(pCfaLeakageFilter, pCfaLeakageFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_cfa_leakage_filter, &pCfaLeakageFilter, sizeof(void *));
    return ik_get_cfa_leakage_filter((UINT32)pMode->ContextId, p_ik_cfa_leakage_filter);
}

/**
* Set the Grgb Mismatch of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pGrgbMismatch, Set Grgb Mismatch filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetGrgbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_GRGB_MISMATCH_s *pGrgbMismatch)
{
    UINT32 rval = 0u;
    const ik_grgb_mismatch_t *p_ik_grgb_mismatch;

    (void)amba_ik_system_memcpy(&p_ik_grgb_mismatch, &pGrgbMismatch, sizeof(void *));
    rval |= ik_set_grgb_mismatch((UINT32)pMode->ContextId, p_ik_grgb_mismatch);

    return rval;
}

/**
* Get the Grgb Mismatch of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pGrgbMismatch, Get Grgb Mismatch filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetGrgbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_GRGB_MISMATCH_s *pGrgbMismatch)
{
    ik_grgb_mismatch_t *p_ik_grgb_mismatch;

    (void)amba_ik_system_memcpy(pGrgbMismatch, pGrgbMismatch, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_grgb_mismatch, &pGrgbMismatch, sizeof(void *));
    return ik_get_grgb_mismatch((UINT32)pMode->ContextId, p_ik_grgb_mismatch);
}

/**
* Set the Anti-Aliasing filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAntiAliasing, Set Anti-Aliasing filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ANTI_ALIASING_s *pAntiAliasing)
{
    UINT32 rval = 0u;
    const ik_anti_aliasing_t *p_ik_anti_aliasing;

    (void)amba_ik_system_memcpy(&p_ik_anti_aliasing, &pAntiAliasing, sizeof(void *));
    rval |= ik_set_anti_aliasing((UINT32)pMode->ContextId, p_ik_anti_aliasing);

    return rval;
}

/**
* Get the Anti-Aliasing filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAntiAliasing, Get Anti-Aliasing filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ANTI_ALIASING_s *pAntiAliasing)
{
    ik_anti_aliasing_t *p_ik_anti_aliasing;

    (void)amba_ik_system_memcpy(pAntiAliasing, pAntiAliasing, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_anti_aliasing, &pAntiAliasing, sizeof(void *));
    return ik_get_anti_aliasing((UINT32)pMode->ContextId, p_ik_anti_aliasing);
}

/**
* Set the dynamic bad pixel filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDynamicBadPxlCor, Set dynamic bad pixel filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDynamicBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DYNAMIC_BAD_PXL_COR_s *pDynamicBadPxlCor)
{
    UINT32 rval = 0u;
    const ik_dynamic_bad_pixel_correction_t *p_ik_dynamic_bad_pxl_cor;

    (void)amba_ik_system_memcpy(&p_ik_dynamic_bad_pxl_cor, &pDynamicBadPxlCor, sizeof(void *));
    rval |= ik_set_dynamic_bad_pixel_corr((UINT32)pMode->ContextId, p_ik_dynamic_bad_pxl_cor);

    return rval;
}

/**
* Get the dynamic bad pixel filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDynamicBadPxlCor, Get dynamic bad pixel filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDynamicBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DYNAMIC_BAD_PXL_COR_s *pDynamicBadPxlCor)
{
    ik_dynamic_bad_pixel_correction_t *p_ik_dynamic_bad_pxl_cor;

    (void)amba_ik_system_memcpy(pDynamicBadPxlCor, pDynamicBadPxlCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_dynamic_bad_pxl_cor, &pDynamicBadPxlCor, sizeof(void *));
    return ik_get_dynamic_bad_pixel_corr((UINT32)pMode->ContextId, p_ik_dynamic_bad_pxl_cor);
}

/**
* Set the CFA noise filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaNoiseFilter, Set CFA noise filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_NOISE_FILTER_s *pCfaNoiseFilter)
{
    UINT32 rval = 0u;
    const ik_cfa_noise_filter_t *p_ik_cfa_noise_filter;

    (void)amba_ik_system_memcpy(&p_ik_cfa_noise_filter, &pCfaNoiseFilter, sizeof(void *));
    rval |= ik_set_cfa_noise_filter((UINT32)pMode->ContextId, p_ik_cfa_noise_filter);

    return rval;
}

/**
* Get the CFA noise filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCfaNoiseFilter, Get CFA noise filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_NOISE_FILTER_s *pCfaNoiseFilter)
{
    ik_cfa_noise_filter_t *p_ik_cfa_noise_filter;

    (void)amba_ik_system_memcpy(pCfaNoiseFilter, pCfaNoiseFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_cfa_noise_filter, &pCfaNoiseFilter, sizeof(void *));
    return ik_get_cfa_noise_filter((UINT32)pMode->ContextId, p_ik_cfa_noise_filter);
}

/**
* Set the demosaic filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDemosaic, Set demosaic filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDemosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DEMOSAIC_s *pDemosaic)
{
    UINT32 rval = 0u;
    const ik_demosaic_t *p_ik_demosaic;

    (void)amba_ik_system_memcpy(&p_ik_demosaic, &pDemosaic, sizeof(void *));
    rval |= ik_set_demosaic((UINT32)pMode->ContextId, p_ik_demosaic);

    return rval;
}

/**
* Get the demosaic filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDemosaic, Get demosaic filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDemosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DEMOSAIC_s *pDemosaic)
{
    ik_demosaic_t *p_ik_demosaic;

    (void)amba_ik_system_memcpy(pDemosaic, pDemosaic, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_demosaic, &pDemosaic, sizeof(void *));
    return ik_get_demosaic((UINT32)pMode->ContextId, p_ik_demosaic);
}

/**
* Set the rgb_to_12y filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbTo12Y, Set rgb_to_12y filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetRgbTo12Y(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_TO_12Y_s *pRgbTo12Y)
{
    UINT32 rval = 0u;
    const ik_rgb_to_12y_t *p_ik_rgb_to_12y;

    (void)amba_ik_system_memcpy(&p_ik_rgb_to_12y, &pRgbTo12Y, sizeof(void *));
    rval |= ik_set_rgb_to_12y((UINT32)pMode->ContextId, p_ik_rgb_to_12y);

    return rval;
}

/**
* Get the rgb_to_12y filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pRgbTo12Y, Get rgb_to_12y filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetRgbTo12Y(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_TO_12Y_s *pRgbTo12Y)
{
    ik_rgb_to_12y_t *p_ik_rgb_to_12y;

    (void)amba_ik_system_memcpy(pRgbTo12Y, pRgbTo12Y, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_rgb_to_12y, &pRgbTo12Y, sizeof(void *));
    return ik_get_rgb_to_12y((UINT32)pMode->ContextId, p_ik_rgb_to_12y);
}

/**
* Set the luma_noise_reduction filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pLumaNoiseReduction, Set luma_noise_reduction filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LUMA_NOISE_REDUCTION_s *pLumaNoiseReduction)
{
    UINT32 rval = 0u;
    const ik_luma_noise_reduction_t *p_ik_luma_noise_reduction;

    (void)amba_ik_system_memcpy(&p_ik_luma_noise_reduction, &pLumaNoiseReduction, sizeof(void *));
    rval |= ik_set_luma_noise_reduction((UINT32)pMode->ContextId, p_ik_luma_noise_reduction);

    return rval;
}

/**
* Get the luma_noise_reduction filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLumaNoiseReduction, Get luma_noise_reduction filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LUMA_NOISE_REDUCTION_s *pLumaNoiseReduction)
{
    ik_luma_noise_reduction_t *p_ik_luma_noise_reduction;

    (void)amba_ik_system_memcpy(pLumaNoiseReduction, pLumaNoiseReduction, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_luma_noise_reduction, &pLumaNoiseReduction, sizeof(void *));
    return ik_get_luma_noise_reduction((UINT32)pMode->ContextId, p_ik_luma_noise_reduction);
}

/**
* Set the color correction of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pColorCorrection, Set color correction
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetColorCorrection(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_COLOR_CORRECTION_s *pColorCorrection)
{
    UINT32 rval = 0u;
    const ik_color_correction_t *p_ik_color_correction;

    (void)amba_ik_system_memcpy(&p_ik_color_correction, &pColorCorrection, sizeof(void *));
    rval |= ik_set_color_correction((UINT32)pMode->ContextId, p_ik_color_correction);

    return rval;
}

/**
* Get the color correction of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pColorCorrection, Get color correction
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetColorCorrection(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_COLOR_CORRECTION_s *pColorCorrection)
{
    ik_color_correction_t *p_ik_color_correction;

    (void)amba_ik_system_memcpy(pColorCorrection, pColorCorrection, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_color_correction, &pColorCorrection, sizeof(void *));
    return ik_get_color_correction((UINT32)pMode->ContextId, p_ik_color_correction);
}

/**
* Set the tone curve filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pToneCurve, Set tone curve filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetToneCurve(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_TONE_CURVE_s *pToneCurve)
{
    UINT32 rval = 0u;
    const ik_tone_curve_t *p_ik_tone_curve;

    (void)amba_ik_system_memcpy(&p_ik_tone_curve, &pToneCurve, sizeof(void *));
    rval |= ik_set_tone_curve((UINT32)pMode->ContextId, p_ik_tone_curve);

    return rval;
}

/**
* Get the tone curve filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pToneCurve, Get tone curve filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetToneCurve(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_TONE_CURVE_s *pToneCurve)
{
    ik_tone_curve_t *p_ik_tone_curve;

    (void)amba_ik_system_memcpy(pToneCurve, pToneCurve, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_tone_curve, &pToneCurve, sizeof(void *));
    return ik_get_tone_curve((UINT32)pMode->ContextId, p_ik_tone_curve);
}

/**
* Set the RGB to YUV matrix of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbToYuvMatrix, Set RGB to YUV matrix
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetRgbToYuvMatrix(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_TO_YUV_MATRIX_s *pRgbToYuvMatrix)
{
    UINT32 rval = 0u;
    const ik_rgb_to_yuv_matrix_t *p_ik_rgb_to_yuv_matrix;

    (void)amba_ik_system_memcpy(&p_ik_rgb_to_yuv_matrix, &pRgbToYuvMatrix, sizeof(void *));
    rval |= ik_set_rgb_to_yuv_matrix((UINT32)pMode->ContextId, p_ik_rgb_to_yuv_matrix);

    return rval;
}

/**
* Get the RGB to YUV matrix of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pRgbToYuvMatrix, Get RGB to YUV matrix
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetRgbToYuvMatrix(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_TO_YUV_MATRIX_s *pRgbToYuvMatrix)
{
    ik_rgb_to_yuv_matrix_t *p_ik_rgb_to_yuv_matrix;

    (void)amba_ik_system_memcpy(pRgbToYuvMatrix, pRgbToYuvMatrix, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_rgb_to_yuv_matrix, &pRgbToYuvMatrix, sizeof(void *));
    return ik_get_rgb_to_yuv_matrix((UINT32)pMode->ContextId, p_ik_rgb_to_yuv_matrix);
}

/**
* Set the RgbIr setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pRgbIr, Set RgbIr filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetRgbIr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RGB_IR_s *pRgbIr)
{
    UINT32 rval = 0u;
    const ik_rgb_ir_t *p_ik_rgb_ir;

    (void)amba_ik_system_memcpy(&p_ik_rgb_ir, &pRgbIr, sizeof(void *));
    rval |= ik_set_rgb_ir((UINT32)pMode->ContextId, p_ik_rgb_ir);

    return rval;
}

/**
* Get the RgbIr setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pRgbIr, Get RgbIr filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetRgbIr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RGB_IR_s *pRgbIr)
{
    ik_rgb_ir_t *p_ik_rgb_ir;

    (void)amba_ik_system_memcpy(pRgbIr, pRgbIr, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_rgb_ir, &pRgbIr, sizeof(void *));
    return ik_get_rgb_ir((UINT32)pMode->ContextId, p_ik_rgb_ir);
}

/**
* Set the chroma scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaScale, Set chroma scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaScale(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_SCALE_s *pChromaScale)
{
    UINT32 rval = 0u;
    const ik_chroma_scale_t *p_ik_chroma_scale;

    (void)amba_ik_system_memcpy(&p_ik_chroma_scale, &pChromaScale, sizeof(void *));
    rval |= ik_set_chroma_scale((UINT32)pMode->ContextId, p_ik_chroma_scale);

    return rval;
}

/**
* Get the chroma scale of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaScale, Get chroma scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaScale(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_SCALE_s *pChromaScale)
{
    ik_chroma_scale_t *p_ik_chroma_scale;

    (void)amba_ik_system_memcpy(pChromaScale, pChromaScale, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_chroma_scale, &pChromaScale, sizeof(void *));
    return ik_get_chroma_scale((UINT32)pMode->ContextId, p_ik_chroma_scale);
}

/**
* Set the chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaFilter, Set chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_FILTER_s *pChromaFilter)
{
    UINT32 rval = 0u;
    const ik_chroma_filter_t *p_ik_chroma_filter;

    (void)amba_ik_system_memcpy(&p_ik_chroma_filter, &pChromaFilter, sizeof(void *));
    rval |= ik_set_chroma_filter((UINT32)pMode->ContextId, p_ik_chroma_filter);

    return rval;
}

/**
* Get the chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaFilter, Get chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_FILTER_s *pChromaFilter)
{
    ik_chroma_filter_t *p_ik_chroma_filter;

    (void)amba_ik_system_memcpy(pChromaFilter, pChromaFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_chroma_filter, &pChromaFilter, sizeof(void *));
    return ik_get_chroma_filter((UINT32)pMode->ContextId, p_ik_chroma_filter);
}

/**
* Set the wide chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWideChromaFilter, Set wide chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WIDE_CHROMA_FILTER_s *pWideChromaFilter)
{
    UINT32 rval = 0u;
    const ik_wide_chroma_filter_t *p_ik_wide_chroma_filter;

    (void)amba_ik_system_memcpy(&p_ik_wide_chroma_filter, &pWideChromaFilter, sizeof(void *));
    rval |= ik_set_wide_chroma_filter((UINT32)pMode->ContextId, p_ik_wide_chroma_filter);

    return rval;
}

/**
* Get the wide chroma filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWideChromaFilter, Get wide chroma filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WIDE_CHROMA_FILTER_s *pWideChromaFilter)
{
    ik_wide_chroma_filter_t *p_ik_wide_chroma_filter;

    (void)amba_ik_system_memcpy(pWideChromaFilter, pWideChromaFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_wide_chroma_filter, &pWideChromaFilter, sizeof(void *));
    return ik_get_wide_chroma_filter((UINT32)pMode->ContextId, p_ik_wide_chroma_filter);
}

/**
* Set the wide chroma filter combine of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWideChromaFilterCombine, Set wide chroma filter combine
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s *pWideChromaFilterCombine)
{
    UINT32 rval = 0u;
    const ik_wide_chroma_filter_combine_t *p_ik_wide_chroma_filter_combine;

    (void)amba_ik_system_memcpy(&p_ik_wide_chroma_filter_combine, &pWideChromaFilterCombine, sizeof(void *));
    rval |= ik_set_wide_chroma_filter_combine((UINT32)pMode->ContextId, p_ik_wide_chroma_filter_combine);

    return rval;
}

/**
* Get the wide chroma filter combine of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWideChromaFilterCombine, Get wide chroma filter combine
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WIDE_CHROMA_FILTER_COMBINE_s *pWideChromaFilterCombine)
{
    ik_wide_chroma_filter_combine_t *p_ik_wide_chroma_filter_combine;

    (void)amba_ik_system_memcpy(pWideChromaFilterCombine, pWideChromaFilterCombine, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_wide_chroma_filter_combine, &pWideChromaFilterCombine, sizeof(void *));
    return ik_get_wide_chroma_filter_combine((UINT32)pMode->ContextId, p_ik_wide_chroma_filter_combine);
}

/**
* Set the chroma median filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pChromaMedianFilter, Set chroma median filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_MEDIAN_FILTER_s *pChromaMedianFilter)
{
    UINT32 rval = 0u;
    const ik_chroma_median_filter_t *p_ik_chroma_median_filter;

    (void)amba_ik_system_memcpy(&p_ik_chroma_median_filter, &pChromaMedianFilter, sizeof(void *));
    rval |= ik_set_chroma_median_filter((UINT32)pMode->ContextId, p_ik_chroma_median_filter);

    return rval;
}

/**
* Get the chroma median filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaMedianFilter, Get chroma median filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_MEDIAN_FILTER_s *pChromaMedianFilter)
{
    ik_chroma_median_filter_t *p_ik_chroma_median_filter;

    (void)amba_ik_system_memcpy(pChromaMedianFilter, pChromaMedianFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_chroma_median_filter, &pChromaMedianFilter, sizeof(void *));
    return ik_get_chroma_median_filter((UINT32)pMode->ContextId, p_ik_chroma_median_filter);
}

/**
* Set the first luma processing mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFirstLumaProcMode, Set first luma processing mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFirstLumaProcMode(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FIRST_LUMA_PROC_MODE_s *pFirstLumaProcMode)
{
    UINT32 rval = 0u;
    const ik_first_luma_process_mode_t *p_ik_first_luma_proc_mode;

    (void)amba_ik_system_memcpy(&p_ik_first_luma_proc_mode, &pFirstLumaProcMode, sizeof(void *));
    rval |= ik_set_fst_luma_process_mode((UINT32)pMode->ContextId, p_ik_first_luma_proc_mode);

    return rval;
}

/**
* Get the first luma processing mode of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFirstLumaProcMode, Get first luma processing mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFirstLumaProcMode(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FIRST_LUMA_PROC_MODE_s *pFirstLumaProcMode)
{
    ik_first_luma_process_mode_t *p_ik_first_luma_proc_mode;

    (void)amba_ik_system_memcpy(pFirstLumaProcMode, pFirstLumaProcMode, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_luma_proc_mode, &pFirstLumaProcMode, sizeof(void *));
    return ik_get_fst_luma_process_mode((UINT32)pMode->ContextId, p_ik_first_luma_proc_mode);
}

/**
* Set the advance spatial filter of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAdvSpatFltr, Set advance spatial filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAdvSpatFltr(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ADV_SPAT_FLTR_s *pAdvSpatFltr)
{
    UINT32 rval = 0u;
    const ik_adv_spatial_filter_t*p_ik_asf_spat_fltr;

    (void)amba_ik_system_memcpy(&p_ik_asf_spat_fltr, &pAdvSpatFltr, sizeof(void *));
    rval |= ik_set_adv_spatial_filter((UINT32)pMode->ContextId, p_ik_asf_spat_fltr);

    return rval;
}

/**
* Get the advance spatial filter of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAdvSpatFltr, Get advance spatial filter
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAdvSpatFltr(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_ADV_SPAT_FLTR_s *pAdvSpatFltr)
{
    ik_adv_spatial_filter_t*p_ik_asf_spat_fltr;

    (void)amba_ik_system_memcpy(pAdvSpatFltr, pAdvSpatFltr, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_asf_spat_fltr, &pAdvSpatFltr, sizeof(void *));
    return ik_get_adv_spatial_filter((UINT32)pMode->ContextId, p_ik_asf_spat_fltr);
}

/**
* Set the first sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsBoth, Set first sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_BOTH_s *pFstShpNsBoth)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_both_t *p_ik_first_sharpen_both;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_both, &pFstShpNsBoth, sizeof(void *));
    rval |= ik_set_fst_shp_both((UINT32)pMode->ContextId, p_ik_first_sharpen_both);

    return rval;
}

/**
* Get the first sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsBoth, Get first sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_BOTH_s *pFstShpNsBoth)
{
    ik_first_sharpen_both_t *p_ik_first_sharpen_both;

    (void)amba_ik_system_memcpy(pFstShpNsBoth, pFstShpNsBoth, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_both, &pFstShpNsBoth, sizeof(void *));
    return ik_get_fst_shp_both((UINT32)pMode->ContextId, p_ik_first_sharpen_both);
}

/**
* Set the first sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsFir, Set first sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_FIR_s *pFstShpNsFir)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_fir_t *p_ik_first_sharpen_fir;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_fir, &pFstShpNsFir, sizeof(void *));
    rval |= ik_set_fst_shp_fir((UINT32)pMode->ContextId, p_ik_first_sharpen_fir);

    return rval;
}

/**
* Get the first sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsFir, Get first sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_FIR_s *pFstShpNsFir)
{
    ik_first_sharpen_fir_t *p_ik_first_sharpen_fir;

    (void)amba_ik_system_memcpy(pFstShpNsFir, pFstShpNsFir, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_fir, &pFstShpNsFir, sizeof(void *));
    return ik_get_fst_shp_fir((UINT32)pMode->ContextId, p_ik_first_sharpen_fir);
}

/**
* Set the first sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsNoise, Set first sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_NOISE_s *pFstShpNsNoise)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_noise_t *p_ik_first_sharpen_noise;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_noise, &pFstShpNsNoise, sizeof(void *));
    rval |= ik_set_fst_shp_noise((UINT32)pMode->ContextId, p_ik_first_sharpen_noise);

    return rval;
}

/**
* Get the first sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsNoise, Get first sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_NOISE_s *pFstShpNsNoise)
{
    ik_first_sharpen_noise_t *p_ik_first_sharpen_noise;

    (void)amba_ik_system_memcpy(pFstShpNsNoise, pFstShpNsNoise, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_noise, &pFstShpNsNoise, sizeof(void *));
    return ik_get_fst_shp_noise((UINT32)pMode->ContextId, p_ik_first_sharpen_noise);
}

/**
* Set the first sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsCoring, Set first sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_CORING_s *pFstShpNsCoring)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_coring_t *p_ik_first_sharpen_coring;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_coring, &pFstShpNsCoring, sizeof(void *));
    rval |= ik_set_fst_shp_coring((UINT32)pMode->ContextId, p_ik_first_sharpen_coring);

    return rval;
}

/**
* Get the first sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsCoring, Get first sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_CORING_s *pFstShpNsCoring)
{
    ik_first_sharpen_coring_t *p_ik_first_sharpen_coring;

    (void)amba_ik_system_memcpy(pFstShpNsCoring, pFstShpNsCoring, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_coring, &pFstShpNsCoring, sizeof(void *));
    return ik_get_fst_shp_coring((UINT32)pMode->ContextId, p_ik_first_sharpen_coring);
}

/**
* Set the first sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsCorIdxScl, Set first sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *pFstShpNsCorIdxScl)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_coring_idx_scale_t *p_ik_first_sharpen_coring_idx_scale;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_coring_idx_scale, &pFstShpNsCorIdxScl, sizeof(void *));
    rval |= ik_set_fst_shp_coring_idx_scale((UINT32)pMode->ContextId, p_ik_first_sharpen_coring_idx_scale);

    return rval;
}

/**
* Get the first sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsCorIdxScl, Set first sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_COR_IDX_SCL_s *pFstShpNsCorIdxScl)
{
    ik_first_sharpen_coring_idx_scale_t *p_ik_first_sharpen_coring_idx_scale;

    (void)amba_ik_system_memcpy(pFstShpNsCorIdxScl, pFstShpNsCorIdxScl, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_coring_idx_scale, &pFstShpNsCorIdxScl, sizeof(void *));
    return ik_get_fst_shp_coring_idx_scale((UINT32)pMode->ContextId, p_ik_first_sharpen_coring_idx_scale);
}

/**
* Set the first sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsMinCorRst, Set first sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_MIN_COR_RST_s *pFstShpNsMinCorRst)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_min_coring_result_t *p_ik_first_sharpen_min_coring_result;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_min_coring_result, &pFstShpNsMinCorRst, sizeof(void *));
    rval |= ik_set_fst_shp_min_coring_rslt((UINT32)pMode->ContextId, p_ik_first_sharpen_min_coring_result);

    return rval;
}

/**
* Get the first sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsMinCorRst, Get first sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_MIN_COR_RST_s *pFstShpNsMinCorRst)
{
    ik_first_sharpen_min_coring_result_t *p_ik_first_sharpen_min_coring_result;

    (void)amba_ik_system_memcpy(pFstShpNsMinCorRst, pFstShpNsMinCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_min_coring_result, &pFstShpNsMinCorRst, sizeof(void *));
    return ik_get_fst_shp_min_coring_rslt((UINT32)pMode->ContextId, p_ik_first_sharpen_min_coring_result);
}

/**
* Set the first sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsMaxCorRst, Set first sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_MAX_COR_RST_s *pFstShpNsMaxCorRst)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_max_coring_result_t *p_ik_first_sharpen_max_coring_result;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_max_coring_result, &pFstShpNsMaxCorRst, sizeof(void *));
    rval |= ik_set_fst_shp_max_coring_rslt((UINT32)pMode->ContextId, p_ik_first_sharpen_max_coring_result);

    return rval;
}

/**
* Get the first sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsMaxCorRst, Get first sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_MAX_COR_RST_s *pFstShpNsMaxCorRst)
{
    ik_first_sharpen_max_coring_result_t *p_ik_first_sharpen_max_coring_result;

    (void)amba_ik_system_memcpy(pFstShpNsMaxCorRst, pFstShpNsMaxCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_max_coring_result, &pFstShpNsMaxCorRst, sizeof(void *));
    return ik_get_fst_shp_max_coring_rslt((UINT32)pMode->ContextId, p_ik_first_sharpen_max_coring_result);
}

/**
* Set the first sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFstShpNsSclCor, Set first sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFstShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FSTSHPNS_SCL_COR_s *pFstShpNsSclCor)
{
    UINT32 rval = 0u;
    const ik_first_sharpen_scale_coring_t *p_ik_first_sharpen_scale_coring;

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_scale_coring, &pFstShpNsSclCor, sizeof(void *));
    rval |= ik_set_fst_shp_scale_coring((UINT32)pMode->ContextId, p_ik_first_sharpen_scale_coring);

    return rval;
}

/**
* Get the first sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFstShpNsSclCor, Get first sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFstShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FSTSHPNS_SCL_COR_s *pFstShpNsSclCor)
{
    ik_first_sharpen_scale_coring_t *p_ik_first_sharpen_scale_coring;

    (void)amba_ik_system_memcpy(pFstShpNsSclCor, pFstShpNsSclCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_first_sharpen_scale_coring, &pFstShpNsSclCor, sizeof(void *));
    return ik_get_fst_shp_scale_coring((UINT32)pMode->ContextId, p_ik_first_sharpen_scale_coring);
}

/**
* Set the final sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsBoth, Set final sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_BOTH_s *pFnlShpNsBoth)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_both_t *p_ik_final_sharpen_both;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_both, &pFnlShpNsBoth, sizeof(void *));
    rval |= ik_set_fnl_shp_both((UINT32)pMode->ContextId, p_ik_final_sharpen_both);

    return rval;
}

/**
* Get the final sharpen noise both of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsBoth, Get final sharpen noise both
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_BOTH_s *pFnlShpNsBoth)
{
    ik_final_sharpen_both_t *p_ik_final_sharpen_both;

    (void)amba_ik_system_memcpy(pFnlShpNsBoth, pFnlShpNsBoth, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_both, &pFnlShpNsBoth, sizeof(void *));
    return ik_get_fnl_shp_both((UINT32)pMode->ContextId, p_ik_final_sharpen_both);
}

/**
* Set the final sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsFir, Set final sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_FIR_s *pFnlShpNsFir)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_fir_t *p_ik_final_sharpen_fir;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_fir, &pFnlShpNsFir, sizeof(void *));
    rval |= ik_set_fnl_shp_fir((UINT32)pMode->ContextId, p_ik_final_sharpen_fir);

    return rval;
}

/**
* Get the final sharpen noise fir of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsFir, Get final sharpen noise fir
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_FIR_s *pFnlShpNsFir)
{
    ik_final_sharpen_fir_t *p_ik_final_sharpen_fir;

    (void)amba_ik_system_memcpy(pFnlShpNsFir, pFnlShpNsFir, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_fir, &pFnlShpNsFir, sizeof(void *));
    return ik_get_fnl_shp_fir((UINT32)pMode->ContextId, p_ik_final_sharpen_fir);
}

/**
* Set the final sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsNoise, Set final sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_NOISE_s *pFnlShpNsNoise)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_noise_t *p_ik_final_sharpen_noise;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_noise, &pFnlShpNsNoise, sizeof(void *));
    rval |= ik_set_fnl_shp_noise((UINT32)pMode->ContextId, p_ik_final_sharpen_noise);

    return rval;
}

/**
* Get the final sharpen noise noise of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsNoise, Get final sharpen noise noise
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_NOISE_s *pFnlShpNsNoise)
{
    ik_final_sharpen_noise_t *p_ik_final_sharpen_noise;

    (void)amba_ik_system_memcpy(pFnlShpNsNoise, pFnlShpNsNoise, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_noise, &pFnlShpNsNoise, sizeof(void *));
    return ik_get_fnl_shp_noise((UINT32)pMode->ContextId, p_ik_final_sharpen_noise);
}

/**
* Set the final sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsCoring, Set final sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_CORING_s *pFnlShpNsCoring)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_coring_t *p_ik_final_sharpen_coring;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_coring, &pFnlShpNsCoring, sizeof(void *));
    rval |= ik_set_fnl_shp_coring((UINT32)pMode->ContextId, p_ik_final_sharpen_coring);

    return rval;
}

/**
* Get the final sharpen noise coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsCoring, Get final sharpen noise coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_CORING_s *pFnlShpNsCoring)
{
    ik_final_sharpen_coring_t *p_ik_final_sharpen_coring;

    (void)amba_ik_system_memcpy(pFnlShpNsCoring, pFnlShpNsCoring, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_coring, &pFnlShpNsCoring, sizeof(void *));
    return ik_get_fnl_shp_coring((UINT32)pMode->ContextId, p_ik_final_sharpen_coring);
}

/**
* Set the final sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsCorIdxScl, Set final sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *pFnlShpNsCorIdxScl)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_coring_idx_scale_t *p_ik_final_sharpen_coring_idx_scale;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_coring_idx_scale, &pFnlShpNsCorIdxScl, sizeof(void *));
    rval |= ik_set_fnl_shp_coring_idx_scale((UINT32)pMode->ContextId, p_ik_final_sharpen_coring_idx_scale);

    return rval;
}

/**
* Get the final sharpen noise coring index scale of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsCorIdxScl, Set final sharpen noise coring index scale
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_COR_IDX_SCL_s *pFnlShpNsCorIdxScl)
{
    ik_final_sharpen_coring_idx_scale_t *p_ik_final_sharpen_coring_idx_scale;

    (void)amba_ik_system_memcpy(pFnlShpNsCorIdxScl, pFnlShpNsCorIdxScl, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_coring_idx_scale, &pFnlShpNsCorIdxScl, sizeof(void *));
    return ik_get_fnl_shp_coring_idx_scale((UINT32)pMode->ContextId, p_ik_final_sharpen_coring_idx_scale);
}

/**
* Set the final sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsMinCorRst, Set final sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_MIN_COR_RST_s *pFnlShpNsMinCorRst)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_min_coring_result_t *p_ik_final_sharpen_min_coring_result;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_min_coring_result, &pFnlShpNsMinCorRst, sizeof(void *));
    rval |= ik_set_fnl_shp_min_coring_rslt((UINT32)pMode->ContextId, p_ik_final_sharpen_min_coring_result);

    return rval;
}

/**
* Get the final sharpen noise min coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsMinCorRst, Get final sharpen noise min coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_MIN_COR_RST_s *pFnlShpNsMinCorRst)
{
    ik_final_sharpen_min_coring_result_t *p_ik_final_sharpen_min_coring_result;

    (void)amba_ik_system_memcpy(pFnlShpNsMinCorRst, pFnlShpNsMinCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_min_coring_result, &pFnlShpNsMinCorRst, sizeof(void *));
    return ik_get_fnl_shp_min_coring_rslt((UINT32)pMode->ContextId, p_ik_final_sharpen_min_coring_result);
}

/**
* Set the final sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsMaxCorRst, Set final sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_MAX_COR_RST_s *pFnlShpNsMaxCorRst)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_max_coring_result_t *p_ik_final_sharpen_max_coring_result;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_max_coring_result, &pFnlShpNsMaxCorRst, sizeof(void *));
    rval |= ik_set_fnl_shp_max_coring_rslt((UINT32)pMode->ContextId, p_ik_final_sharpen_max_coring_result);

    return rval;
}

/**
* Get the final sharpen noise max coring result of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsMaxCorRst, Get final sharpen noise max coring result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_MAX_COR_RST_s *pFnlShpNsMaxCorRst)
{
    ik_final_sharpen_max_coring_result_t *p_ik_final_sharpen_max_coring_result;

    (void)amba_ik_system_memcpy(pFnlShpNsMaxCorRst, pFnlShpNsMaxCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_max_coring_result, &pFnlShpNsMaxCorRst, sizeof(void *));
    return ik_get_fnl_shp_max_coring_rslt((UINT32)pMode->ContextId, p_ik_final_sharpen_max_coring_result);
}

/**
* Set the final sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsSclCor, Set final sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_SCL_COR_s *pFnlShpNsSclCor)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_scale_coring_t *p_ik_final_sharpen_scale_coring;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_scale_coring, &pFnlShpNsSclCor, sizeof(void *));
    rval |= ik_set_fnl_shp_scale_coring((UINT32)pMode->ContextId, p_ik_final_sharpen_scale_coring);

    return rval;
}

/**
* Get the final sharpen noise scale coring of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsSclCor, Get final sharpen noise scale coring
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_SCL_COR_s *pFnlShpNsSclCor)
{
    ik_final_sharpen_scale_coring_t *p_ik_final_sharpen_scale_coring;

    (void)amba_ik_system_memcpy(pFnlShpNsSclCor, pFnlShpNsSclCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_scale_coring, &pFnlShpNsSclCor, sizeof(void *));
    return ik_get_fnl_shp_scale_coring((UINT32)pMode->ContextId, p_ik_final_sharpen_scale_coring);
}

/**
* Set the final sharpen noise both 3d table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFnlShpNsBothTdt, Set final sharpen noise both 3d table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFnlShpNsBothTdt(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FNLSHPNS_BOTH_TDT_s *pFnlShpNsBothTdt)
{
    UINT32 rval = 0u;
    const ik_final_sharpen_both_three_d_table_t *p_ik_final_sharpen_both_three_d_table;

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_both_three_d_table, &pFnlShpNsBothTdt, sizeof(void *));
    rval |= ik_set_fnl_shp_three_d_table((UINT32)pMode->ContextId, p_ik_final_sharpen_both_three_d_table);

    return rval;
}

/**
* Get the final sharpen noise both 3d table of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFnlShpNsBothTdt, Get final sharpen noise both 3d table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFnlShpNsBothTdt(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FNLSHPNS_BOTH_TDT_s *pFnlShpNsBothTdt)
{
    ik_final_sharpen_both_three_d_table_t *p_ik_final_sharpen_both_three_d_table;

    (void)amba_ik_system_memcpy(pFnlShpNsBothTdt, pFnlShpNsBothTdt, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_final_sharpen_both_three_d_table, &pFnlShpNsBothTdt, sizeof(void *));
    return ik_get_fnl_shp_three_d_table((UINT32)pMode->ContextId, p_ik_final_sharpen_both_three_d_table);
}

/**
* Set the video mctf of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctf, Set video mctf
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVideoMctf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIDEO_MCTF_s *pVideoMctf)
{
    UINT32 rval = 0u;
    const ik_video_mctf_t *p_ik_video_mctf;

    (void)amba_ik_system_memcpy(&p_ik_video_mctf, &pVideoMctf, sizeof(void *));
    rval |= ik_set_video_mctf((UINT32)pMode->ContextId, p_ik_video_mctf);

    return rval;
}

/**
* Get the video mctf of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVideoMctf, Get video mctf
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVideoMctf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIDEO_MCTF_s *pVideoMctf)
{
    ik_video_mctf_t *p_ik_video_mctf;

    (void)amba_ik_system_memcpy(pVideoMctf, pVideoMctf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_video_mctf, &pVideoMctf, sizeof(void *));
    return ik_get_video_mctf((UINT32)pMode->ContextId, p_ik_video_mctf);
}

/**
* Set the video mctf temporal adjust of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctfTa, Set video mctf temporal adjust
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVideoMctfTa(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIDEO_MCTF_TA_s *pVideoMctfTa)
{
    UINT32 rval = 0u;
    const ik_video_mctf_ta_t *p_ik_video_mctf_ta;

    (void)amba_ik_system_memcpy(&p_ik_video_mctf_ta, &pVideoMctfTa, sizeof(void *));
    rval |= ik_set_video_mctf_ta((UINT32)pMode->ContextId, p_ik_video_mctf_ta);

    return rval;
}

/**
* Get the video mctf temporal adjust of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVideoMctfTa, Get video mctf temporal adjust
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVideoMctfTa(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIDEO_MCTF_TA_s *pVideoMctfTa)
{
    ik_video_mctf_ta_t *p_ik_video_mctf_ta;

    (void)amba_ik_system_memcpy(pVideoMctfTa, pVideoMctfTa, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_video_mctf_ta, &pVideoMctfTa, sizeof(void *));
    return ik_get_video_mctf_ta((UINT32)pMode->ContextId, p_ik_video_mctf_ta);
}

/**
* Set the video mctf and final sharpen of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVideoMctfAndFnlshp, Set video mctf and final sharpen
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVideoMctfAndFnlshp(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_POS_DSP33_t *pVideoMctfAndFnlshp)
{
    UINT32 rval = 0u;
    const ik_pos_dep33_t *p_ik_video_mctf_and_final_sharpen;

    (void)amba_ik_system_memcpy(&p_ik_video_mctf_and_final_sharpen, &pVideoMctfAndFnlshp, sizeof(void *));
    rval |= ik_set_video_mctf_and_fnl_shp((UINT32)pMode->ContextId, p_ik_video_mctf_and_final_sharpen);

    return rval;
}

/**
* Get the video mctf and final sharpen of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVideoMctfAndFnlshp, Get video mctf and final sharpen
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVideoMctfAndFnlshp(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_POS_DSP33_t *pVideoMctfAndFnlshp)
{
    ik_pos_dep33_t *p_ik_video_mctf_and_final_sharpen;

    (void)amba_ik_system_memcpy(pVideoMctfAndFnlshp, pVideoMctfAndFnlshp, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_video_mctf_and_final_sharpen, &pVideoMctfAndFnlshp, sizeof(void *));
    return ik_get_video_mctf_and_fnl_shp((UINT32)pMode->ContextId, p_ik_video_mctf_and_final_sharpen);
}

/**
* Set the window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWindowSizeInfo, Set window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo)
{
    UINT32 rval = 0u;
    const ik_window_size_info_t *p_ik_window_size_info;

    (void)amba_ik_system_memcpy(&p_ik_window_size_info, &pWindowSizeInfo, sizeof(void *));
    rval |= ik_set_window_size_info((UINT32)pMode->ContextId, p_ik_window_size_info);

    return rval;
}

/**
* Get the window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWindowSizeInfo, Get window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WINDOW_SIZE_INFO_s *pWindowSizeInfo)
{
    ik_window_size_info_t *p_ik_window_size_info;

    (void)amba_ik_system_memcpy(pWindowSizeInfo, pWindowSizeInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_window_size_info, &pWindowSizeInfo, sizeof(void *));
    return ik_get_window_size_info((UINT32)pMode->ContextId, p_ik_window_size_info);
}

/**
* Set the cfa window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCfaWindowSizeInfo, Set cfa window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCfaWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CFA_WINDOW_SIZE_INFO_s *pCfaWindowSizeInfo)
{
    UINT32 rval = 0u;
    const ik_cfa_window_size_info_t *p_ik_cfa_window_size_info;

    (void)amba_ik_system_memcpy(&p_ik_cfa_window_size_info, &pCfaWindowSizeInfo, sizeof(void *));
    rval |= ik_set_cfa_window_size_info((UINT32)pMode->ContextId, p_ik_cfa_window_size_info);

    return rval;
}

/**
* Get the cfa window size information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCfaWindowSizeInfo, Get cfa window size information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCfaWindowSizeInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CFA_WINDOW_SIZE_INFO_s *pCfaWindowSizeInfo)
{
    ik_cfa_window_size_info_t *p_ik_cfa_window_size_info;

    (void)amba_ik_system_memcpy(pCfaWindowSizeInfo, pCfaWindowSizeInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_cfa_window_size_info, &pCfaWindowSizeInfo, sizeof(void *));
    return ik_get_cfa_window_size_info((UINT32)pMode->ContextId, p_ik_cfa_window_size_info);
}

/**
* Set the dzoom information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDzoomInfo, Set dzoom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDzoomInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DZOOM_INFO_s *pDzoomInfo)
{
    UINT32 rval = 0u;
    const ik_dzoom_info_t *p_ik_dzoom_info;

    (void)amba_ik_system_memcpy(&p_ik_dzoom_info, &pDzoomInfo, sizeof(void *));
    rval |= ik_set_dzoom_info((UINT32)pMode->ContextId, p_ik_dzoom_info);

    return rval;
}

/**
* Get the dzoom information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDzoomInfo, Get dzoom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDzoomInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DZOOM_INFO_s *pDzoomInfo)
{
    ik_dzoom_info_t *p_ik_dzoom_info;

    (void)amba_ik_system_memcpy(pDzoomInfo, pDzoomInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_dzoom_info, &pDzoomInfo, sizeof(void *));
    return ik_get_dzoom_info((UINT32)pMode->ContextId, p_ik_dzoom_info);
}

/**
* Set the Hdr Eis Shift Compensation information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrSftInfo, Set Hdr Eis Shift Compensation
* @param [in]  ExpIdx, Set Exposure Index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrShiftInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_EIS_INFO_s *pHdrSftInfo, UINT32 ExpIdx)
{
    UINT32 rval = 0u;
    const ik_hdr_eis_info_t *p_ik_hdr_sft_info;

    (void)amba_ik_system_memcpy(&p_ik_hdr_sft_info, &pHdrSftInfo, sizeof(void *));
    if (ExpIdx == 1U) {
        rval |= ik_set_hdr_shift_info_exp1((UINT32)pMode->ContextId, p_ik_hdr_sft_info);
    } else if (ExpIdx == 2U) {
        rval |= ik_set_hdr_shift_info_exp2((UINT32)pMode->ContextId, p_ik_hdr_sft_info);
    } else {
        rval |= (UINT32)-1;
    }

    return rval;
}

/**
* Get the Hdr Eis Shift Compensation information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHdrSftInfo, Get Hdr Eis Shift Compensation
* @param [in]  ExpIdx, Set Exposure Index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrShiftInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_EIS_INFO_s *pHdrSftInfo, UINT32 ExpIdx)
{
    UINT32 rval = 0u;
    ik_hdr_eis_info_t *p_ik_hdr_sft_info;

    (void)amba_ik_system_memcpy(pHdrSftInfo, pHdrSftInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hdr_sft_info, &pHdrSftInfo, sizeof(void *));
    if (ExpIdx == 1U) {
        rval |= ik_get_hdr_shift_info_exp1((UINT32)pMode->ContextId, p_ik_hdr_sft_info);
    } else if (ExpIdx == 2U) {
        rval |= ik_get_hdr_shift_info_exp2((UINT32)pMode->ContextId, p_ik_hdr_sft_info);
    } else {
        rval |= (UINT32)-1;
    }

    return rval;
}

/**
* Set the dummy margin range of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pDummyMarginRange, Set dummy margin range
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetDummyMarginRange(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyMarginRange)
{
    UINT32 rval = 0u;
    const ik_dummy_margin_range_t *p_ik_dummy_margin_range;

    (void)amba_ik_system_memcpy(&p_ik_dummy_margin_range, &pDummyMarginRange, sizeof(void *));
    rval |= ik_set_dummy_margin_range((UINT32)pMode->ContextId, p_ik_dummy_margin_range);

    return rval;
}

/**
* Get the dummy margin range of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pDummyMarginRange, Get dummy margin range
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetDummyMarginRange(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_DUMMY_MARGIN_RANGE_s *pDummyMarginRange)
{
    ik_dummy_margin_range_t *p_ik_dummy_margin_range;

    (void)amba_ik_system_memcpy(pDummyMarginRange, pDummyMarginRange, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_dummy_margin_range, &pDummyMarginRange, sizeof(void *));
    return ik_get_dummy_margin_range((UINT32)pMode->ContextId, p_ik_dummy_margin_range);
}

/**
* Set the vin window information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinActiveWin, Set vin window information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVinActiveWin(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIN_ACTIVE_WINDOW_s *pVinActiveWin)
{
    UINT32 rval = 0u;
    const ik_vin_active_window_t *p_ik_active_window;

    (void)amba_ik_system_memcpy(&p_ik_active_window, &pVinActiveWin, sizeof(void *));
    rval |= ik_set_vin_active_win((UINT32)pMode->ContextId, p_ik_active_window);

    return rval;
}

/**
* Get the vin window information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVinActiveWin, Get vin window information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVinActiveWin(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIN_ACTIVE_WINDOW_s *pVinActiveWin)
{
    ik_vin_active_window_t *p_ik_active_window;

    (void)amba_ik_system_memcpy(pVinActiveWin, pVinActiveWin, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_active_window, &pVinActiveWin, sizeof(void *));
    return ik_get_vin_active_win((UINT32)pMode->ContextId, p_ik_active_window);
}

// Calib apis
/**
* Set the warp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  WarpEnb, Set warp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarpEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 WarpEnb)
{
    UINT32 rval = 0u;

    rval |= ik_set_warp_enb((UINT32)pMode->ContextId, WarpEnb);

    return rval;
}

/**
* Get the warp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWarpEnb, Get warp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarpEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pWarpEnb)
{
    return ik_get_warp_enb((UINT32)pMode->ContextId, pWarpEnb);
}

/**
* Set the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo, Set warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarpInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INFO_s *pWarpInfo)
{
    UINT32 rval = 0u;
    const ik_warp_info_t *p_ik_warp_info;

    (void)amba_ik_system_memcpy(&p_ik_warp_info, &pWarpInfo, sizeof(void *));
    rval |= ik_set_warp_info((UINT32)pMode->ContextId, p_ik_warp_info);

    return rval;
}

/**
* Get the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo, Get warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarpInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INFO_s *pWarpInfo)
{
    ik_warp_info_t *p_ik_warp_info;

    (void)amba_ik_system_memcpy(pWarpInfo, pWarpInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_warp_info, &pWarpInfo, sizeof(void *));
    return ik_get_warp_info((UINT32)pMode->ContextId, p_ik_warp_info);
}

// Calib apis
/**
* Set the warp2nd enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  Warp2ndEnb, Set warp2nd enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarp2ndEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 Warp2ndEnb)
{
    UINT32 rval = 0u;

    rval |= ik_set_warp_2nd_enb((UINT32)pMode->ContextId, Warp2ndEnb);

    return rval;
}

/**
* Get the warp2nd enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWarp2ndEnb, Get warp2nd enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarp2ndEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pWarp2ndEnb)
{
    return ik_get_warp_2nd_enb((UINT32)pMode->ContextId, pWarp2ndEnb);
}

/**
* Set the warp2nd information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo2nd, Set warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarp2ndInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INFO_s *pWarp2ndInfo)
{
    UINT32 rval = 0u;
    const ik_warp_info_t *p_ik_warp_info;

    (void)amba_ik_system_memcpy(&p_ik_warp_info, &pWarp2ndInfo, sizeof(void *));
    rval |= ik_set_2nd_warp_info((UINT32)pMode->ContextId, p_ik_warp_info);

    return rval;
}

/**
* Get the warp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInfo2nd, Get warp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarp2ndInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INFO_s *pWarp2ndInfo)
{
    ik_warp_info_t *p_ik_warp_info;

    (void)amba_ik_system_memcpy(pWarp2ndInfo, pWarp2ndInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_warp_info, &pWarp2ndInfo, sizeof(void *));
    return ik_get_2nd_warp_info((UINT32)pMode->ContextId, p_ik_warp_info);
}

/**
* Set the cawarp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  CawarpEnb, Set cawarp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCawarpEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 CawarpEnb)
{
    UINT32 rval = 0u;

    rval |= ik_set_cawarp_enb((UINT32)pMode->ContextId, CawarpEnb);

    return rval;
}

/**
* Get the cawarp enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCawarpEnb, Get cawarp enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCawarpEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pCawarpEnb)
{
    return ik_get_cawarp_enb((UINT32)pMode->ContextId, pCawarpEnb);
}

/**
* Set the cawarp information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCawarpInfo, Set cawarp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCawarpInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INFO_s *pCawarpInfo)
{
    UINT32 rval = 0u;
    const ik_cawarp_info_t *p_ik_cawarp_info;

    (void)amba_ik_system_memcpy(&p_ik_cawarp_info, &pCawarpInfo, sizeof(void *));
    rval |= ik_set_cawarp_info((UINT32)pMode->ContextId, p_ik_cawarp_info);

    return rval;
}

/**
* Get the cawarp information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCawarpInfo, Get cawarp information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCawarpInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INFO_s *pCawarpInfo)
{
    ik_cawarp_info_t *p_ik_cawarp_info;

    (void)amba_ik_system_memcpy(pCawarpInfo, pCawarpInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_cawarp_info, &pCawarpInfo, sizeof(void *));
    return ik_get_cawarp_info((UINT32)pMode->ContextId, p_ik_cawarp_info);
}

/**
* Set the static bad pixel correction enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  StaticBpcEnb, Set static bad pixel correction enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStaticBadPxlCorEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 StaticBpcEnb)
{
    UINT32 rval = 0u;

    rval |= ik_set_static_bad_pxl_corr_enb((UINT32)pMode->ContextId, StaticBpcEnb);

    return rval;
}

/**
* Get the static bad pixel correction enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pStaticBpcEnb, Get static bad pixel correction enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStaticBadPxlCorEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pStaticBpcEnb)
{
    return ik_get_static_bad_pxl_corr_enb((UINT32)pMode->ContextId, pStaticBpcEnb);
}

/**
* Set the static bad pixel correction information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStaticBadPxlCor, Set static bad pixel correction information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStaticBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STATIC_BAD_PXL_COR_s *pStaticBadPxlCor)
{
    UINT32 rval = 0u;
    const ik_static_bad_pxl_cor_t *p_ik_static_bad_pxl_cor;

    (void)amba_ik_system_memcpy(&p_ik_static_bad_pxl_cor, &pStaticBadPxlCor, sizeof(void *));
    rval |= ik_set_static_bad_pxl_corr((UINT32)pMode->ContextId, p_ik_static_bad_pxl_cor);

    return rval;
}

/**
* Get the static bad pixel correction information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pStaticBadPxlCor, Get static bad pixel correction information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStaticBadPxlCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STATIC_BAD_PXL_COR_s *pStaticBadPxlCor)
{
    ik_static_bad_pxl_cor_t *p_ik_static_bad_pxl_cor;

    (void)amba_ik_system_memcpy(pStaticBadPxlCor, pStaticBadPxlCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_static_bad_pxl_cor, &pStaticBadPxlCor, sizeof(void *));
    return ik_get_static_bad_pxl_corr((UINT32)pMode->ContextId, p_ik_static_bad_pxl_cor);
}

/**
* Set the vignette enable of each context
* @param [in]  pMode, Set ContextId
* @param [in]  VignetteEnb, Set vignette enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVignetteEnb(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 VignetteEnb)
{
    UINT32 rval = 0u;

    rval |= ik_set_vignette_enb((UINT32)pMode->ContextId, VignetteEnb);

    return rval;
}

/**
* Get the vignette enable of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVignetteEnb, Get vignette enable
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVignetteEnb(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pVignetteEnb)
{
    return ik_get_vignette_enb((UINT32)pMode->ContextId, pVignetteEnb);
}

/**
* Set the vignette information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVignette, Set vignette information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetVignette(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_VIGNETTE_s *pVignette)
{
    UINT32 rval = 0u;
    const ik_vignette_t *p_ik_vignette;

    (void)amba_ik_system_memcpy(&p_ik_vignette, &pVignette, sizeof(void *));
    rval |= ik_set_vignette((UINT32)pMode->ContextId, p_ik_vignette);

    return rval;
}

/**
* Get the vignette information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pVignette, Get vignette information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetVignette(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_VIGNETTE_s *pVignette)
{
    ik_vignette_t *p_ik_vignette;

    (void)amba_ik_system_memcpy(pVignette, pVignette, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_vignette, &pVignette, sizeof(void *));
    return ik_get_vignette((UINT32)pMode->ContextId, p_ik_vignette);
}

/**
* Set the resamp strength of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pResamplerStr, Set resamp strength
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetResampStrength(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_RESAMPLER_STR_s *pResamplerStr)
{
    UINT32 rval = 0u;
    const ik_resampler_strength_t *p_ik_resample_str;

    (void)amba_ik_system_memcpy(&p_ik_resample_str, &pResamplerStr, sizeof(void *));
    rval |= ik_set_resampler_strength((UINT32)pMode->ContextId, p_ik_resample_str);

    return rval;
}

/**
* Get the resamp strength of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pResamplerStr, Get resamp strength
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetResampStrength(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_RESAMPLER_STR_s *pResamplerStr)
{
    ik_resampler_strength_t *p_ik_resample_str;

    (void)amba_ik_system_memcpy(pResamplerStr, pResamplerStr, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_resample_str, &pResamplerStr, sizeof(void *));
    return ik_get_resampler_strength((UINT32)pMode->ContextId, p_ik_resample_str);
}

/**
* Set the front tone curve of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeToneCurve, Set front tone curve
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFeToneCurve(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FE_TONE_CURVE_s *pFeToneCurve)
{
    UINT32 rval = 0u;
    const ik_frontend_tone_curve_t *p_ik_fe_tone_curve;

    (void)amba_ik_system_memcpy(&p_ik_fe_tone_curve, &pFeToneCurve, sizeof(void *));
    rval |= ik_set_frontend_tone_curve((UINT32)pMode->ContextId, p_ik_fe_tone_curve);

    return rval;
}

/**
* Get the front tone curve of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeToneCurve, Get front tone curve
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFeToneCurve(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FE_TONE_CURVE_s *pFeToneCurve)
{
    ik_frontend_tone_curve_t *p_ik_fe_tone_curve;

    (void)amba_ik_system_memcpy(pFeToneCurve, pFeToneCurve, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_fe_tone_curve, &pFeToneCurve, sizeof(void *));
    return ik_get_frontend_tone_curve((UINT32)pMode->ContextId, p_ik_fe_tone_curve);
}

/**
* Set the front static black level of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeStaticBlc, Set front static black level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFeStaticBlc(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STATIC_BLC_LVL_s *pFeStaticBlc, UINT32 ExpIdx)
{
    UINT32 rval = IK_OK;
    const ik_static_blc_level_t *p_ik_fe_static_blc;

    (void)amba_ik_system_memcpy(&p_ik_fe_static_blc, &pFeStaticBlc, sizeof(void *));
    if (ExpIdx == 0U) {
        rval |= ik_set_exp0_frontend_static_blc((UINT32)pMode->ContextId, p_ik_fe_static_blc);
    } else if (ExpIdx == 1U) {
        rval |= ik_set_exp1_frontend_static_blc((UINT32)pMode->ContextId, p_ik_fe_static_blc);
    } else if (ExpIdx == 2U) {
        rval |= ik_set_exp2_frontend_static_blc((UINT32)pMode->ContextId, p_ik_fe_static_blc);
    } else {
        rval |= (UINT32)-1;
    }

    return rval;
}

/**
* Get the front static black level of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeStaticBlc, Get front static black level
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFeStaticBlc(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STATIC_BLC_LVL_s *pFeStaticBlc, UINT32 ExpIdx)
{
    UINT32 rval = IK_OK;
    ik_static_blc_level_t *p_ik_fe_static_blc;

    (void)amba_ik_system_memcpy(pFeStaticBlc, pFeStaticBlc, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_fe_static_blc, &pFeStaticBlc, sizeof(void *));

    if (ExpIdx == 0U) {
        rval = ik_get_exp0_frontend_static_blc((UINT32)pMode->ContextId, p_ik_fe_static_blc);
    } else if (ExpIdx == 1U) {
        rval = ik_get_exp1_frontend_static_blc((UINT32)pMode->ContextId, p_ik_fe_static_blc);
    } else if (ExpIdx == 2U) {
        rval = ik_get_exp2_frontend_static_blc((UINT32)pMode->ContextId, p_ik_fe_static_blc);
    } else {
        rval = (UINT32)-1;
    }

    return rval;
}

/**
* Set the front wb gain of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pFeWbGain, Set front wb gain
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFeWbGain(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_FE_WB_GAIN_s *pFeWbGain, const UINT32 ExpIdx)
{
    UINT32 rval = IK_OK;
    const ik_frontend_wb_gain_t *p_ik_fe_wb_gain;

    (void)amba_ik_system_memcpy(&p_ik_fe_wb_gain, &pFeWbGain, sizeof(void *));
    if (ExpIdx == 0U) {
        rval |= ik_set_exp0_frontend_wb_gain((UINT32)pMode->ContextId, p_ik_fe_wb_gain);
    } else if (ExpIdx == 1U) {
        rval |= ik_set_exp1_frontend_wb_gain((UINT32)pMode->ContextId, p_ik_fe_wb_gain);
    } else if (ExpIdx == 2U) {
        rval |= ik_set_exp2_frontend_wb_gain((UINT32)pMode->ContextId, p_ik_fe_wb_gain);
    } else {
        rval |= (UINT32)-1;
    }

    return rval;
}

/**
* Get the front wb gain of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFeWbGain, Get front wb gain
* @param [in]  ExpIdx, Set exposure index
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFeWbGain(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_FE_WB_GAIN_s *pFeWbGain, UINT32 ExpIdx)
{
    UINT32 rval = IK_OK;
    ik_frontend_wb_gain_t *p_ik_fe_wb_gain;

    (void)amba_ik_system_memcpy(pFeWbGain, pFeWbGain, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_fe_wb_gain, &pFeWbGain, sizeof(void *));
    if (ExpIdx == 0U) {
        rval = ik_get_exp0_frontend_wb_gain((UINT32)pMode->ContextId, p_ik_fe_wb_gain);
    } else if (ExpIdx == 1U) {
        rval = ik_get_exp1_frontend_wb_gain((UINT32)pMode->ContextId, p_ik_fe_wb_gain);
    } else if (ExpIdx == 2U) {
        rval = ik_get_exp2_frontend_wb_gain((UINT32)pMode->ContextId, p_ik_fe_wb_gain);
    } else {
        rval =(UINT32)-1;
    }

    return rval;
}

/**
* Set the contrast enhancement of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCe, Set contrast enhancement
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCe(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_s *pCe)
{
    UINT32 rval = 0u;
    const ik_ce_t *p_ik_ce;

    (void)amba_ik_system_memcpy(&p_ik_ce, &pCe, sizeof(void *));
    rval |= ik_set_ce((UINT32)pMode->ContextId, p_ik_ce);

    return rval;
}

/**
* Get the contrast enhancement of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCe, Get contrast enhancement
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCe(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_s *pCe)
{
    ik_ce_t *p_ik_ce;

    (void)amba_ik_system_memcpy(pCe, pCe, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_ce, &pCe, sizeof(void *));
    return ik_get_ce((UINT32)pMode->ContextId, p_ik_ce);
}

/**
* Set the contrast enhancement input table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCeInputTable, Set contrast enhancement  input table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCeInputTable(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_INPUT_TABLE_s *pCeInputTable)
{
    UINT32 rval = 0u;
    const ik_ce_input_table_t *p_ik_ce_input_table;

    (void)amba_ik_system_memcpy(&p_ik_ce_input_table, &pCeInputTable, sizeof(void *));
    rval |= ik_set_ce_input_table((UINT32)pMode->ContextId, p_ik_ce_input_table);

    return rval;
}

/**
* Get the contrast enhancement input table of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCeInputTable, Get contrast enhancement  input table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCeInputTable(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_INPUT_TABLE_s *pCeInputTable)
{
    ik_ce_input_table_t *p_ik_ce_input_table;

    (void)amba_ik_system_memcpy(pCeInputTable, pCeInputTable, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_ce_input_table, &pCeInputTable, sizeof(void *));
    return ik_get_ce_input_table((UINT32)pMode->ContextId, p_ik_ce_input_table);
}

/**
* Set the contrast enhancement output table of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCeOutTable, Set contrast enhancement  output table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetCeOutputTable(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CE_OUT_TABLE_s *pCeOutTable)
{
    UINT32 rval = 0u;
    const ik_ce_output_table_t *p_ik_ce_out_table;

    (void)amba_ik_system_memcpy(&p_ik_ce_out_table, &pCeOutTable, sizeof(void *));
    rval |= ik_set_ce_out_table((UINT32)pMode->ContextId, p_ik_ce_out_table);

    return rval;
}

/**
* Get the contrast enhancement output table of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCeOutTable, Get contrast enhancement  output table
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetCeOutputTable(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CE_OUT_TABLE_s *pCeOutTable)
{
    ik_ce_output_table_t *p_ik_ce_out_table;

    (void)amba_ik_system_memcpy(pCeOutTable, pCeOutTable, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_ce_out_table, &pCeOutTable, sizeof(void *));
    return ik_get_ce_out_table((UINT32)pMode->ContextId, p_ik_ce_out_table);
}

/**
* Set the HDR exposure number of each context
* @param [in]  pMode, Set ContextId
* @param [in]  ExpNum, Set HDR exposure number
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPipeMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 PipeMode)
{
    UINT32 rval = 0u;

    rval |= ik_set_hdr_exp_num((UINT32)pMode->ContextId, PipeMode);

    return rval;
}

/**
* Get the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pExpNum, Get HDR exposure number
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPipeMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pPipeMode)
{
    return ik_get_hdr_exp_num((UINT32)pMode->ContextId, pPipeMode);
}

/**
* Set the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrBlend, Set HDR blend
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrBlend(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_BLEND_s *pHdrBlend)
{
    UINT32 rval = 0u;
    const ik_hdr_blend_t *p_ik_hdr_blend;

    (void)amba_ik_system_memcpy(&p_ik_hdr_blend, &pHdrBlend, sizeof(void *));
    rval |= ik_set_hdr_blend((UINT32)pMode->ContextId, p_ik_hdr_blend);

    return rval;
}

/**
* Get the HDR blend of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHdrBlend, Get HDR blend
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrBlend(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_BLEND_s *pHdrBlend)
{
    ik_hdr_blend_t *p_ik_hdr_blend;

    (void)amba_ik_system_memcpy(pHdrBlend, pHdrBlend, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hdr_blend, &pHdrBlend, sizeof(void *));
    return ik_get_hdr_blend((UINT32)pMode->ContextId, p_ik_hdr_blend);
}

/**
* Set the HDR raw offset of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHdrRawInfo, Set HDR raw offset
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHdrRawOffset(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo)
{
    UINT32 rval = 0u;
    const ik_hdr_raw_info_t *p_ik_hdr_raw_info;

    (void)amba_ik_system_memcpy(&p_ik_hdr_raw_info, &pHdrRawInfo, sizeof(void *));
    rval |= ik_set_hdr_raw_offset((UINT32)pMode->ContextId, p_ik_hdr_raw_info);

    return rval;
}

/**
* Get the HDR raw offset of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHdrRawInfo, Get HDR raw offset
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHdrRawOffset(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HDR_RAW_INFO_s *pHdrRawInfo)
{
    ik_hdr_raw_info_t *p_ik_hdr_raw_info;

    (void)amba_ik_system_memcpy(pHdrRawInfo, pHdrRawInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hdr_raw_info, &pHdrRawInfo, sizeof(void *));
    return ik_get_hdr_raw_offset((UINT32)pMode->ContextId, p_ik_hdr_raw_info);
}

// 3A statistic
/**
* Set the AE statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAeStatInfo, Set  AE statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAeStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AE_STAT_INFO_s *pAeStatInfo)
{
    UINT32 rval = IK_OK;
    ik_aaa_stat_info_t amba_ik_aaa_stat_info;

    rval |= ik_get_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    amba_ik_aaa_stat_info.ae_tile_num_col = pAeStatInfo->AeTileNumCol;
    amba_ik_aaa_stat_info.ae_tile_num_row = pAeStatInfo->AeTileNumRow;
    amba_ik_aaa_stat_info.ae_tile_col_start = pAeStatInfo->AeTileColStart;
    amba_ik_aaa_stat_info.ae_tile_row_start = pAeStatInfo->AeTileRowStart;
    amba_ik_aaa_stat_info.ae_tile_width = pAeStatInfo->AeTileWidth;
    amba_ik_aaa_stat_info.ae_tile_height = pAeStatInfo->AeTileHeight;
    amba_ik_aaa_stat_info.ae_pix_min_value = pAeStatInfo->AePixMinValue;
    amba_ik_aaa_stat_info.ae_pix_max_value = pAeStatInfo->AePixMaxValue;

    rval |= ik_set_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    return rval;
}

/**
* Get the AE statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAeStatInfo, Get  AE statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAeStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AE_STAT_INFO_s *pAeStatInfo)
{
    UINT32 rval = IK_OK;
    ik_aaa_stat_info_t amba_ik_aaa_stat_info;

    rval |= ik_get_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    pAeStatInfo->AeTileNumCol = amba_ik_aaa_stat_info.ae_tile_num_col;
    pAeStatInfo->AeTileNumRow = amba_ik_aaa_stat_info.ae_tile_num_row;
    pAeStatInfo->AeTileColStart = amba_ik_aaa_stat_info.ae_tile_col_start;
    pAeStatInfo->AeTileRowStart = amba_ik_aaa_stat_info.ae_tile_row_start;
    pAeStatInfo->AeTileWidth = amba_ik_aaa_stat_info.ae_tile_width;
    pAeStatInfo->AeTileHeight = amba_ik_aaa_stat_info.ae_tile_height;
    pAeStatInfo->AePixMinValue = amba_ik_aaa_stat_info.ae_pix_min_value;
    pAeStatInfo->AePixMaxValue = amba_ik_aaa_stat_info.ae_pix_max_value;

    return rval;
}

/**
* Set the Awb statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAwbStatInfo, Set Awb statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAwbStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AWB_STAT_INFO_s *pAwbStatInfo)
{
    UINT32 rval = IK_OK;
    ik_aaa_stat_info_t amba_ik_aaa_stat_info;

    rval |= ik_get_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    amba_ik_aaa_stat_info.awb_tile_num_col = pAwbStatInfo->AwbTileNumCol;
    amba_ik_aaa_stat_info.awb_tile_num_row = pAwbStatInfo->AwbTileNumRow;
    amba_ik_aaa_stat_info.awb_tile_col_start = pAwbStatInfo->AwbTileColStart;
    amba_ik_aaa_stat_info.awb_tile_row_start = pAwbStatInfo->AwbTileRowStart;
    amba_ik_aaa_stat_info.awb_tile_width = pAwbStatInfo->AwbTileWidth;
    amba_ik_aaa_stat_info.awb_tile_height = pAwbStatInfo->AwbTileHeight;
    amba_ik_aaa_stat_info.awb_tile_active_width = pAwbStatInfo->AwbTileActiveWidth;
    amba_ik_aaa_stat_info.awb_tile_active_height = pAwbStatInfo->AwbTileActiveHeight;
    amba_ik_aaa_stat_info.awb_pix_min_value = pAwbStatInfo->AwbPixMinValue;
    amba_ik_aaa_stat_info.awb_pix_max_value = pAwbStatInfo->AwbPixMaxValue;

    rval |= ik_set_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    return rval;
}

/**
* Get the Awb statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAwbStatInfo, Get Awb statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAwbStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AWB_STAT_INFO_s *pAwbStatInfo)
{
    UINT32 rval = IK_OK;
    ik_aaa_stat_info_t amba_ik_aaa_stat_info;

    rval |= ik_get_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    pAwbStatInfo->AwbTileNumCol = amba_ik_aaa_stat_info.awb_tile_num_col;
    pAwbStatInfo->AwbTileNumRow = amba_ik_aaa_stat_info.awb_tile_num_row;
    pAwbStatInfo->AwbTileColStart = amba_ik_aaa_stat_info.awb_tile_col_start;
    pAwbStatInfo->AwbTileRowStart = amba_ik_aaa_stat_info.awb_tile_row_start;
    pAwbStatInfo->AwbTileWidth = amba_ik_aaa_stat_info.awb_tile_width;
    pAwbStatInfo->AwbTileHeight = amba_ik_aaa_stat_info.awb_tile_height;
    pAwbStatInfo->AwbTileActiveWidth = amba_ik_aaa_stat_info.awb_tile_active_width;
    pAwbStatInfo->AwbTileActiveHeight = amba_ik_aaa_stat_info.awb_tile_active_height;
    pAwbStatInfo->AwbPixMinValue = amba_ik_aaa_stat_info.awb_pix_min_value;
    pAwbStatInfo->AwbPixMaxValue = amba_ik_aaa_stat_info.awb_pix_max_value;

    return rval;

}

/**
* Set the CFA Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatInfo, Set Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_INFO_s *pAfStatInfo)
{
    UINT32 rval = IK_OK;
    ik_aaa_stat_info_t amba_ik_aaa_stat_info;

    rval |= ik_get_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    amba_ik_aaa_stat_info.af_tile_num_col = pAfStatInfo->AfTileNumCol;
    amba_ik_aaa_stat_info.af_tile_num_row = pAfStatInfo->AfTileNumRow;
    amba_ik_aaa_stat_info.af_tile_col_start = pAfStatInfo->AfTileColStart;
    amba_ik_aaa_stat_info.af_tile_row_start = pAfStatInfo->AfTileRowStart;
    amba_ik_aaa_stat_info.af_tile_width = pAfStatInfo->AfTileWidth;
    amba_ik_aaa_stat_info.af_tile_height = pAfStatInfo->AfTileHeight;
    amba_ik_aaa_stat_info.af_tile_active_width = pAfStatInfo->AfTileActiveWidth;
    amba_ik_aaa_stat_info.af_tile_active_height = pAfStatInfo->AfTileActiveHeight;

    rval |= ik_set_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    return rval;
}

/**
* Get the CFA Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfStatInfo, Get Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_INFO_s *pAfStatInfo)
{
    UINT32 rval = IK_OK;
    ik_aaa_stat_info_t amba_ik_aaa_stat_info;

    rval |= ik_get_aaa_stat_info((UINT32)pMode->ContextId, &amba_ik_aaa_stat_info);

    pAfStatInfo->AfTileNumCol = amba_ik_aaa_stat_info.af_tile_num_col;
    pAfStatInfo->AfTileNumRow = amba_ik_aaa_stat_info.af_tile_num_row;
    pAfStatInfo->AfTileColStart = amba_ik_aaa_stat_info.af_tile_col_start;
    pAfStatInfo->AfTileRowStart = amba_ik_aaa_stat_info.af_tile_row_start;
    pAfStatInfo->AfTileWidth = amba_ik_aaa_stat_info.af_tile_width;
    pAfStatInfo->AfTileHeight = amba_ik_aaa_stat_info.af_tile_height;
    pAfStatInfo->AfTileActiveWidth = amba_ik_aaa_stat_info.af_tile_active_width;
    pAfStatInfo->AfTileActiveHeight = amba_ik_aaa_stat_info.af_tile_active_height;

    return rval;
}

/**
* Set the PG Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatInfo, Set Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPgAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_INFO_s *pAfStatInfo)
{
    UINT32 rval = 0u;
    const ik_aaa_pg_af_stat_info_t *p_ik_pg_af_stat_info;

    (void)amba_ik_system_memcpy(&p_ik_pg_af_stat_info, &pAfStatInfo, sizeof(void *));
    rval |= ik_set_aaa_pg_af_stat_info((UINT32)pMode->ContextId, p_ik_pg_af_stat_info);

    return rval;
}

/**
* Get the PG Af statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfStatInfo, Get Af statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPgAfStatInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_INFO_s *pAfStatInfo)
{
    ik_aaa_pg_af_stat_info_t *p_ik_pg_af_stat_info;

    (void)amba_ik_system_memcpy(pAfStatInfo, pAfStatInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_pg_af_stat_info, &pAfStatInfo, sizeof(void *));
    return ik_get_aaa_pg_af_stat_info((UINT32)pMode->ContextId, p_ik_pg_af_stat_info);
}

/**
* Set the AAA statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pAfStatExInfo, Set AAA statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_AF_STAT_EX_INFO_s *pAfStatExInfo)
{
    UINT32 rval = 0u;
    const ik_af_stat_ex_info_t *p_ik_af_stat_ex_info;

    (void)amba_ik_system_memcpy(&p_ik_af_stat_ex_info, &pAfStatExInfo, sizeof(void *));
    rval |= ik_set_af_stat_ex_info((UINT32)pMode->ContextId, p_ik_af_stat_ex_info);

    return rval;
}

/**
* Get the AAA statistic information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAfStatExInfo, Get AAA statistic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_AF_STAT_EX_INFO_s *pAfStatExInfo)
{
    ik_af_stat_ex_info_t *p_ik_af_stat_ex_info;

    (void)amba_ik_system_memcpy(pAfStatExInfo, pAfStatExInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_af_stat_ex_info, &pAfStatExInfo, sizeof(void *));
    return ik_get_af_stat_ex_info((UINT32)pMode->ContextId, p_ik_af_stat_ex_info);
}

/**
* Set the cfa Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pPgAfStatExInfo, Set cfa Af statistic extended information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPgAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_PG_AF_STAT_EX_INFO_s *pPgAfStatExInfo)
{
    UINT32 rval = 0u;
    const ik_pg_af_stat_ex_info_t *p_ik_pg_af_stat_ex_info;

    (void)amba_ik_system_memcpy(&p_ik_pg_af_stat_ex_info, &pPgAfStatExInfo, sizeof(void *));
    rval |= ik_set_pg_af_stat_ex_info((UINT32)pMode->ContextId, p_ik_pg_af_stat_ex_info);

    return rval;
}

/**
* Get the cfa Af statistic extended information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pPgAfStatExInfo, Get cfa Af statistic extended information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPgAfStatExInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_PG_AF_STAT_EX_INFO_s *pPgAfStatExInfo)
{
    ik_pg_af_stat_ex_info_t *p_ik_pg_af_stat_ex_info;

    (void)amba_ik_system_memcpy(pPgAfStatExInfo, pPgAfStatExInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_pg_af_stat_ex_info, &pPgAfStatExInfo, sizeof(void *));
    return ik_get_pg_af_stat_ex_info((UINT32)pMode->ContextId, p_ik_pg_af_stat_ex_info);
}

/**
* Set the cfa histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Set cfa histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_HISTOGRAM_INFO_s *pHistInfo)
{
    UINT32 rval = 0u;
    const ik_histogram_info_t *p_ik_hist_info;

    (void)amba_ik_system_memcpy(&p_ik_hist_info, &pHistInfo, sizeof(void *));
    rval |= ik_set_histogram_info((UINT32)pMode->ContextId, p_ik_hist_info);

    return rval;
}

/**
* Get the cfa histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Get cfa histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HISTOGRAM_INFO_s *pHistInfo)
{
    ik_histogram_info_t *p_ik_hist_info;

    (void)amba_ik_system_memcpy(pHistInfo, pHistInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hist_info, &pHistInfo, sizeof(void *));
    return ik_get_histogram_info((UINT32)pMode->ContextId, p_ik_hist_info);
}

/**
* Set the pg histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pHistInfo, Set pg histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetPgHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode,const  AMBA_IK_HISTOGRAM_INFO_s *pHistInfo)
{
    UINT32 rval = 0u;
    const ik_histogram_info_t *p_ik_hist_info;

    (void)amba_ik_system_memcpy(&p_ik_hist_info, &pHistInfo, sizeof(void *));
    rval |= ik_set_pg_histogram_info((UINT32)pMode->ContextId, p_ik_hist_info);

    return rval;
}

/**
* Set the pg histogram information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHistInfo, Set pg histogram information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetPgHistogramInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HISTOGRAM_INFO_s *pHistInfo)
{
    ik_histogram_info_t *p_ik_hist_info;

    (void)amba_ik_system_memcpy(pHistInfo, pHistInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hist_info, &pHistInfo, sizeof(void *));
    return ik_get_pg_histogram_info((UINT32)pMode->ContextId, p_ik_hist_info);
}

/**
* Set the flip mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  FlipMode, Set Flip Mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetFlipMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 FlipMode)
{
    UINT32 rval = 0u;

    rval |= ik_set_flip_mode((UINT32)pMode->ContextId, FlipMode);

    return rval;
}

/**
* Get the flip mode of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pFlipMode, Get Flip Mode
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFlipMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pFlipMode)
{
    return ik_get_flip_mode((UINT32)pMode->ContextId, pFlipMode);
}

/**
* Set the warp buffer info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpBufInfo, Set warp buffer information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetWarpBufferInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_BUFFER_INFO_s *pWarpBufInfo)
{
    UINT32 rval = 0u;
    const ik_warp_buffer_info_t *p_ik_warp_buf_info;

    (void)amba_ik_system_memcpy(&p_ik_warp_buf_info, &pWarpBufInfo, sizeof(void *));
    rval |= ik_set_warp_buffer_info((UINT32)pMode->ContextId, p_ik_warp_buf_info);

    return rval;
}

/**
* Get the warp buffer info of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWarpBufInfo, Get warp buffer information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetWarpBufferInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_BUFFER_INFO_s *pWarpBufInfo)
{
    ik_warp_buffer_info_t *p_ik_warp_buf_info;

    (void)amba_ik_system_memcpy(pWarpBufInfo, pWarpBufInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_warp_buf_info, &pWarpBufInfo, sizeof(void *));
    return ik_get_warp_buffer_info((UINT32)pMode->ContextId, p_ik_warp_buf_info);
}

/**
* Set the overlap x of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pOverlapX, Set overlap x
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetOverlapX(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_OVERLAP_X_s *pOverlapX)
{
    UINT32 rval = 0u;
    const ik_overlap_x_t *p_ik_overlap_x;

    (void)amba_ik_system_memcpy(&p_ik_overlap_x, &pOverlapX, sizeof(void *));
    rval |= ik_set_overlap_x((UINT32)pMode->ContextId, p_ik_overlap_x);

    return rval;
}

/**
* Get the overlap x of each context
* @param [in]  pMode, Set ContextId
* @param [inout]  pOverlapX, Get overlap x
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetOverlapX(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_OVERLAP_X_s *pOverlapX)
{
    ik_overlap_x_t *p_ik_overlap_x;

    (void)amba_ik_system_memcpy(pOverlapX, pOverlapX, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_overlap_x, &pOverlapX, sizeof(void *));
    return ik_get_overlap_x((UINT32)pMode->ContextId, p_ik_overlap_x);
}

/**
* Set the stitching info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pStitchInfo, Set stitching information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetStitchingInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_STITCH_INFO_s *pStitchInfo)
{
    UINT32 rval = 0u;
    const ik_stitch_info_t *p_ik_stitch_info;

    (void)amba_ik_system_memcpy(&p_ik_stitch_info, &pStitchInfo, sizeof(void *));
    rval |= ik_set_stitching_info((UINT32)pMode->ContextId, p_ik_stitch_info);

    return rval;
}

/**
* Get the stitching info of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pStitchInfo, Get stitching information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetStitchingInfo(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_STITCH_INFO_s *pStitchInfo)
{
    ik_stitch_info_t *p_ik_stitch_info;

    (void)amba_ik_system_memcpy(pStitchInfo, pStitchInfo, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_stitch_info, &pStitchInfo, sizeof(void *));
    return ik_get_stitching_info((UINT32)pMode->ContextId, p_ik_stitch_info);
}

/**
* Set the burst info of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pBurstTile, Set burst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetBurstTile(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_BURST_TILE_s *pBurstTile)
{
    UINT32 rval = 0u;
    const ik_burst_tile_t *p_ik_burst_tile;

    (void)amba_ik_system_memcpy(&p_ik_burst_tile, &pBurstTile, sizeof(void *));
    rval |= ik_set_burst_tile((UINT32)pMode->ContextId, p_ik_burst_tile);

    return rval;
}
/**
* Get the burst info of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pBurstTile, Get burst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetBurstTile(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_BURST_TILE_s *pBurstTile)
{
    ik_burst_tile_t *p_ik_burst_tile;

    (void)amba_ik_system_memcpy(pBurstTile, pBurstTile, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_burst_tile, &pBurstTile, sizeof(void *));
    return ik_get_burst_tile((UINT32)pMode->ContextId, p_ik_burst_tile);
}

// Internal apis
/**
* Set the warp internal setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pWarpInternal, Set warp internal setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_SetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal)
{
    UINT32 rval = 0u;
    const ik_warp_internal_info_t *p_ik_warp_internal_info;

    (void)amba_ik_system_memcpy(&p_ik_warp_internal_info, &pWarpInternal, sizeof(void *));
    rval |= ik_set_warp_internal((UINT32)pMode->ContextId, p_ik_warp_internal_info);

    return rval;
}

/**
* Get the warp internal setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pWarpInternal, Get warp internal setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_GetWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_WARP_INTERNAL_INFO_s *pWarpInternal)
{
    ik_warp_internal_info_t *p_ik_warp_internal_info;

    (void)amba_ik_system_memcpy(pWarpInternal, pWarpInternal, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_warp_internal_info, &pWarpInternal, sizeof(void *));
    return ik_get_warp_internal((UINT32)pMode->ContextId, p_ik_warp_internal_info);
}

/**
* Set the cawarp internal setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pCawarpInternal, Set cawarp internal setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_SetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal)
{
    UINT32 rval = 0u;
    const ik_cawarp_internal_info_t *p_ik_cawarp_internal_info;

    (void)amba_ik_system_memcpy(&p_ik_cawarp_internal_info, &pCawarpInternal, sizeof(void *));
    rval |= ik_set_cawarp_internal((UINT32)pMode->ContextId, p_ik_cawarp_internal_info);

    return rval;
}

/**
* Get the cawarp internal setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pCawarpInternal, Get cawarp internal setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_GetCaWarpInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CAWARP_INTERNAL_INFO_s *pCawarpInternal)
{
    ik_cawarp_internal_info_t *p_ik_cawarp_internal_info;

    (void)amba_ik_system_memcpy(pCawarpInternal, pCawarpInternal, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_cawarp_internal_info, &pCawarpInternal, sizeof(void *));
    return ik_get_cawarp_internal((UINT32)pMode->ContextId, p_ik_cawarp_internal_info);
}

/**
* Set the sbp internal setting of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pSbpInternal, Set sbp internal setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_SetStaticBpcInternal(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_SBP_INTERNAL_INFO_s *pSbpInternal)
{
    UINT32 rval = 0u;
    const ik_static_bad_pixel_correction_internal_t *p_ik_static_bpc_internal_info;

    (void)amba_ik_system_memcpy(&p_ik_static_bpc_internal_info, &pSbpInternal, sizeof(void *));
    rval |= ik_set_static_bad_pxl_corr_itnl((UINT32)pMode->ContextId, p_ik_static_bpc_internal_info);

    return rval;
}

/**
* Get the sbp internal setting of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pSbpInternal, Get sbp internal setting
* @return OK - success, NG - fail
* The api is internal and we don't need to verify it.
*/
UINT32 AmbaIK_GetStaticBpcInternal(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_SBP_INTERNAL_INFO_s *pSbpInternal)
{
    ik_static_bad_pixel_correction_internal_t *p_ik_static_bpc_internal_info;

    (void)amba_ik_system_memcpy(pSbpInternal, pSbpInternal, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_static_bpc_internal_info, &pSbpInternal, sizeof(void *));
    return ik_get_static_bad_pxl_corr_itnl((UINT32)pMode->ContextId, p_ik_static_bpc_internal_info);
}

/**
* Set the external raw out mode of each context
* @param [in]  pMode, Set ContextId
* @param [in]  ExtRawOutMode, Set external raw out mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetExtRawOutMode(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 ExtRawOutMode)
{
    UINT32 rval = 0u;

    rval |= ik_set_ext_raw_out_mode((UINT32)pMode->ContextId, ExtRawOutMode);

    return rval;
}

/**
* Get the external raw out mode Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExtRawOutMode, Get external raw out mode information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetExtRawOutMode(const AMBA_IK_MODE_CFG_s *pMode, UINT32 *pExtRawOutMode)
{
    return ik_get_ext_raw_out_mode((UINT32)pMode->ContextId, pExtRawOutMode);
}

