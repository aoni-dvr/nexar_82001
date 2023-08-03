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

#include "AmbaTypes.h"
//#include "AmbaWrap.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgHisoFilter.h"
#include "AmbaDSP_ImageHisoFilter.h"
//#define amba_ik_system_print (...)//AmbaPrint  //TBD

//#define amba_ik_system_memcpy AmbaWrap_memcpy//memcpy   //TBD

/**
* Set the HiAntiAliasing Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiAntiAliasing information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_ANTI_ALIASING_s *pHiAntiAliasing)
{
    const ik_hi_anti_aliasing_t *p_ik_hi_anti_aliasing;

    (void)amba_ik_system_memcpy(&p_ik_hi_anti_aliasing, &pHiAntiAliasing, sizeof(void *));
    return ik_set_hi_anti_aliasing((UINT32)pMode->ContextId, p_ik_hi_anti_aliasing);
}

/**
* Get the HiAntiAliasing Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiAntiAliasing, Get HiAntiAliasing information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiAntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_ANTI_ALIASING_s *pHiAntiAliasing)
{
    ik_hi_anti_aliasing_t *p_ik_hi_anti_aliasing;

    (void)amba_ik_system_memcpy(pHiAntiAliasing, pHiAntiAliasing, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_anti_aliasing, &pHiAntiAliasing, sizeof(void *));
    return ik_get_hi_anti_aliasing((UINT32)pMode->ContextId, p_ik_hi_anti_aliasing);
}

/**
* Set the HiCfaLeakageFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiCfaLeakageFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *pHiCfaLeakageFilter)
{
    const ik_hi_cfa_leakage_filter_t *p_ik_hi_cfa_leakage_filter;

    (void)amba_ik_system_memcpy(&p_ik_hi_cfa_leakage_filter, &pHiCfaLeakageFilter, sizeof(void *));
    return ik_set_hi_cfa_leakage_filter((UINT32)pMode->ContextId, p_ik_hi_cfa_leakage_filter);
}

/**
* Get the HiCfaLeakageFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiCfaLeakageFilter, Get HiCfaLeakageFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiCfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CFA_LEAKAGE_FILTER_s *pHiCfaLeakageFilter)
{
    ik_hi_cfa_leakage_filter_t *p_ik_hi_cfa_leakage_filter;

    (void)amba_ik_system_memcpy(pHiCfaLeakageFilter, pHiCfaLeakageFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_cfa_leakage_filter, &pHiCfaLeakageFilter, sizeof(void *));
    return ik_get_hi_cfa_leakage_filter((UINT32)pMode->ContextId, p_ik_hi_cfa_leakage_filter);
}

/**
* Set the HiDynamicBPC Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiDynamicBPC information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiDynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_DYNAMIC_BPC_s *pHiDynamicBPC)
{
    const ik_hi_dynamic_bpc_t *p_ik_hi_dynamic_bpc;

    (void)amba_ik_system_memcpy(&p_ik_hi_dynamic_bpc, &pHiDynamicBPC, sizeof(void *));
    return ik_set_hi_dynamic_bpc((UINT32)pMode->ContextId, p_ik_hi_dynamic_bpc);
}

/**
* Get the HiDynamicBPC Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiDynamicBPC, Get HiDynamicBPC information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiDynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_DYNAMIC_BPC_s *pHiDynamicBPC)
{
    ik_hi_dynamic_bpc_t *p_ik_hi_dynamic_bpc;

    (void)amba_ik_system_memcpy(pHiDynamicBPC, pHiDynamicBPC, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_dynamic_bpc, &pHiDynamicBPC, sizeof(void *));
    return ik_get_hi_dynamic_bpc((UINT32)pMode->ContextId, p_ik_hi_dynamic_bpc);
}

/**
* Set the HiChromaMedianFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaMedianFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiGrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_GRGB_MISMATCH_s *pHiGrGbMismatch)
{
    const ik_hi_grgb_mismatch_t *p_ik_hi_grgb_mismatch;

    (void)amba_ik_system_memcpy(&p_ik_hi_grgb_mismatch, &pHiGrGbMismatch, sizeof(void *));
    return ik_set_hi_grgb_mismatch((UINT32)pMode->ContextId, p_ik_hi_grgb_mismatch);
}

/**
* Get the HiGrGbMismatch Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiGrGbMismatch, Get HiGrGbMismatch information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiGrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_GRGB_MISMATCH_s *pHiGrGbMismatch)
{
    ik_hi_grgb_mismatch_t *p_ik_hi_grgb_mismatch;

    (void)amba_ik_system_memcpy(pHiGrGbMismatch, pHiGrGbMismatch, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_grgb_mismatch, &pHiGrGbMismatch, sizeof(void *));
    return ik_get_hi_grgb_mismatch((UINT32)pMode->ContextId, p_ik_hi_grgb_mismatch);
}

/**
* Set the HiChromaMedianFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaMedianFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *pHiChromaMedianFilter)
{
    const ik_hi_chroma_median_filter_t *p_ik_hi_chroma_median_filter;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_median_filter, &pHiChromaMedianFilter, sizeof(void *));
    return ik_set_hi_chroma_median_filter((UINT32)pMode->ContextId, p_ik_hi_chroma_median_filter);
}

/**
* Get the HiChromaMedianFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaMedianFilter, Get HiChromaMedianFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaMedianFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_MEDIAN_FILTER_s *pHiChromaMedianFilter)
{
    ik_hi_chroma_median_filter_t *p_ik_hi_chroma_median_filter;

    (void)amba_ik_system_memcpy(pHiChromaMedianFilter, pHiChromaMedianFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_median_filter, &pHiChromaMedianFilter, sizeof(void *));
    return ik_get_hi_chroma_median_filter((UINT32)pMode->ContextId, p_ik_hi_chroma_median_filter);
}

/**
* Set the HiCfaNoiseFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiCfaNoiseFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CFA_NOISE_FILTER_s *pHiCfaNoiseFilter)
{
    const ik_hi_cfa_noise_filter_t *p_ik_hi_cfa_noise_filter;

    (void)amba_ik_system_memcpy(&p_ik_hi_cfa_noise_filter, &pHiCfaNoiseFilter, sizeof(void *));
    return ik_set_hi_cfa_noise_filter((UINT32)pMode->ContextId, p_ik_hi_cfa_noise_filter);
}

/**
* Get the HiCfaNoiseFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiCfaNoiseFilter, Get HiCfaNoiseFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiCfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CFA_NOISE_FILTER_s *pHiCfaNoiseFilter)
{
    ik_hi_cfa_noise_filter_t *p_ik_hi_cfa_noise_filter;

    (void)amba_ik_system_memcpy(pHiCfaNoiseFilter, pHiCfaNoiseFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_cfa_noise_filter, &pHiCfaNoiseFilter, sizeof(void *));
    return ik_get_hi_cfa_noise_filter((UINT32)pMode->ContextId, p_ik_hi_cfa_noise_filter);
}

/**
* Set the HiDemosaic Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiDemosaic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiDemosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_DEMOSAIC_s *pHiDemosaic)
{
    const ik_hi_demosaic_t *p_ik_hi_demosaic;

    (void)amba_ik_system_memcpy(&p_ik_hi_demosaic, &pHiDemosaic, sizeof(void *));
    return ik_set_hi_demosaic((UINT32)pMode->ContextId, p_ik_hi_demosaic);
}

/**
* Get the HiDemosaic Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiDemosaic, Get HiDemosaic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiDemosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_DEMOSAIC_s *pHiDemosaic)
{
    ik_hi_demosaic_t *p_ik_hi_demosaic;

    (void)amba_ik_system_memcpy(pHiDemosaic, pHiDemosaic, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_demosaic, &pHiDemosaic, sizeof(void *));
    return ik_get_hi_demosaic((UINT32)pMode->ContextId, p_ik_hi_demosaic);
}

/**
* Set the Li2AntiAliasing Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2AntiAliasing information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2AntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_ANTI_ALIASING_s *pLi2AntiAliasing)
{
    const ik_li2_anti_aliasing_t *p_ik_li2_anti_aliasing;

    (void)amba_ik_system_memcpy(&p_ik_li2_anti_aliasing, &pLi2AntiAliasing, sizeof(void *));
    return ik_set_li2_anti_aliasing((UINT32)pMode->ContextId, p_ik_li2_anti_aliasing);
}

/**
* Get the Li2AntiAliasing Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2AntiAliasing, Get Li2AntiAliasing information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2AntiAliasing(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_ANTI_ALIASING_s *pLi2AntiAliasing)
{
    ik_li2_anti_aliasing_t *p_ik_li2_anti_aliasing;

    (void)amba_ik_system_memcpy(pLi2AntiAliasing, pLi2AntiAliasing, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_anti_aliasing, &pLi2AntiAliasing, sizeof(void *));
    return ik_get_li2_anti_aliasing((UINT32)pMode->ContextId, p_ik_li2_anti_aliasing);
}

/**
* Set the Li2CfaLeakageFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2CfaLeakageFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2CfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *pLi2CfaLeakageFilter)
{
    const ik_li2_cfa_leakage_filter_t *p_ik_li2_cfa_leakage_filter;

    (void)amba_ik_system_memcpy(&p_ik_li2_cfa_leakage_filter, &pLi2CfaLeakageFilter, sizeof(void *));
    return ik_set_li2_cfa_leakage_filter((UINT32)pMode->ContextId, p_ik_li2_cfa_leakage_filter);
}

/**
* Get the Li2CfaLeakageFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2CfaLeakageFilter, Get Li2CfaLeakageFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2CfaLeakageFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_CFA_LEAKAGE_FILTER_s *pLi2CfaLeakageFilter)
{
    ik_li2_cfa_leakage_filter_t *p_ik_li2_cfa_leakage_filter;

    (void)amba_ik_system_memcpy(pLi2CfaLeakageFilter, pLi2CfaLeakageFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_cfa_leakage_filter, &pLi2CfaLeakageFilter, sizeof(void *));
    return ik_get_li2_cfa_leakage_filter((UINT32)pMode->ContextId, p_ik_li2_cfa_leakage_filter);
}

/**
* Set the Li2DynamicBPC Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2DynamicBPC information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2DynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_DYNAMIC_BPC_s *pLi2DynamicBPC)
{
    const ik_li2_dynamic_bpc_t *p_ik_li2_dynamic_bpc;

    (void)amba_ik_system_memcpy(&p_ik_li2_dynamic_bpc, &pLi2DynamicBPC, sizeof(void *));
    return ik_set_li2_dynamic_bpc((UINT32)pMode->ContextId, p_ik_li2_dynamic_bpc);
}

/**
* Get the Li2DynamicBPC Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2DynamicBPC, Get Li2DynamicBPC information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2DynamicBPC(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_DYNAMIC_BPC_s *pLi2DynamicBPC)
{
    ik_li2_dynamic_bpc_t *p_ik_li2_dynamic_bpc;

    (void)amba_ik_system_memcpy(pLi2DynamicBPC, pLi2DynamicBPC, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_dynamic_bpc, &pLi2DynamicBPC, sizeof(void *));
    return ik_get_li2_dynamic_bpc((UINT32)pMode->ContextId, p_ik_li2_dynamic_bpc);
}

/**
* Set the Li2GrGbMismatch Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2GrGbMismatch information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2GrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_GRGB_MISMATCH_s *pLi2GrGbMismatch)
{
    const ik_li2_grgb_mismatch_t *p_ik_li2_grgb_mismatch;

    (void)amba_ik_system_memcpy(&p_ik_li2_grgb_mismatch, &pLi2GrGbMismatch, sizeof(void *));
    return ik_set_li2_grgb_mismatch((UINT32)pMode->ContextId, p_ik_li2_grgb_mismatch);
}

/**
* Get the Li2GrGbMismatch Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2GrGbMismatch, Get Li2GrGbMismatch information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2GrGbMismatch(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_GRGB_MISMATCH_s *pLi2GrGbMismatch)
{
    ik_li2_grgb_mismatch_t *p_ik_li2_grgb_mismatch;

    (void)amba_ik_system_memcpy(pLi2GrGbMismatch, pLi2GrGbMismatch, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_grgb_mismatch, &pLi2GrGbMismatch, sizeof(void *));
    return ik_get_li2_grgb_mismatch((UINT32)pMode->ContextId, p_ik_li2_grgb_mismatch);
}

/**
* Set the Li2CfaNoiseFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2CfaNoiseFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2CfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_CFA_NOISE_FILTER_s *pLi2CfaNoiseFilter)
{
    const ik_li2_cfa_noise_filter_t *p_ik_li2_cfa_noise_filter;

    (void)amba_ik_system_memcpy(&p_ik_li2_cfa_noise_filter, &pLi2CfaNoiseFilter, sizeof(void *));
    return ik_set_li2_cfa_noise_filter((UINT32)pMode->ContextId, p_ik_li2_cfa_noise_filter);
}

/**
* Get the Li2CfaNoiseFilter Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2CfaNoiseFilter, Get Li2CfaNoiseFilter information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2CfaNoiseFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_CFA_NOISE_FILTER_s *pLi2CfaNoiseFilter)
{
    ik_li2_cfa_noise_filter_t *p_ik_li2_cfa_noise_filter;

    (void)amba_ik_system_memcpy(pLi2CfaNoiseFilter, pLi2CfaNoiseFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_cfa_noise_filter, &pLi2CfaNoiseFilter, sizeof(void *));
    return ik_get_li2_cfa_noise_filter((UINT32)pMode->ContextId, p_ik_li2_cfa_noise_filter);
}

/**
* Set the Li2Demosaic Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2Demosaic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2Demosaic(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_DEMOSAIC_s *pLi2Demosaic)
{
    const ik_li2_demosaic_t *p_ik_li2_demosaic;

    (void)amba_ik_system_memcpy(&p_ik_li2_demosaic, &pLi2Demosaic, sizeof(void *));
    return ik_set_li2_demosaic((UINT32)pMode->ContextId, p_ik_li2_demosaic);
}

/**
* Get the Li2Demosaic Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2Demosaic, Get Li2Demosaic information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2Demosaic(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_DEMOSAIC_s *pLi2Demosaic)
{
    ik_li2_demosaic_t *p_ik_li2_demosaic;

    (void)amba_ik_system_memcpy(pLi2Demosaic, pLi2Demosaic, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_demosaic, &pLi2Demosaic, sizeof(void *));
    return ik_get_li2_demosaic((UINT32)pMode->ContextId, p_ik_li2_demosaic);
}

/**
* Set the HiAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_ASF_s *pHiAsf)
{
    const ik_hi_asf_t *p_ik_hi_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_asf, &pHiAsf, sizeof(void *));
    return ik_set_hi_asf((UINT32)pMode->ContextId, p_ik_hi_asf);
}

/**
* Get the HiAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiAsf, Get HiAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_ASF_s *pHiAsf)
{
    ik_hi_asf_t *p_ik_hi_asf;

    (void)amba_ik_system_memcpy(pHiAsf, pHiAsf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_asf, &pHiAsf, sizeof(void *));
    return ik_get_hi_asf((UINT32)pMode->ContextId, p_ik_hi_asf);
}

/**
* Set the Li2Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_ASF_s *pLi2Asf)
{
    const ik_li2_asf_t *p_ik_li2_asf;

    (void)amba_ik_system_memcpy(&p_ik_li2_asf, &pLi2Asf, sizeof(void *));
    return ik_set_li2_asf((UINT32)pMode->ContextId, p_ik_li2_asf);
}

/**
* Get the Li2Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2Asf, Get Li2Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_ASF_s *pLi2Asf)
{
    ik_li2_asf_t *p_ik_li2_asf;

    (void)amba_ik_system_memcpy(pLi2Asf, pLi2Asf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_asf, &pLi2Asf, sizeof(void *));
    return ik_get_li2_asf((UINT32)pMode->ContextId, p_ik_li2_asf);
}

/**
* Set the HiLowAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiLowAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiLowAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LOW_ASF_s *pHiLowAsf)
{
    const ik_hi_low_asf_t *p_ik_hi_low_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_low_asf, &pHiLowAsf, sizeof(void *));
    return ik_set_hi_low_asf((UINT32)pMode->ContextId, p_ik_hi_low_asf);
}

/**
* Get the HiLowAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiLowAsf, Get HiLowAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiLowAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LOW_ASF_s *pHiLowAsf)
{
    ik_hi_low_asf_t *p_ik_hi_low_asf;

    (void)amba_ik_system_memcpy(pHiLowAsf, pHiLowAsf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_low_asf, &pHiLowAsf, sizeof(void *));
    return ik_get_hi_low_asf((UINT32)pMode->ContextId, p_ik_hi_low_asf);
}

/**
* Set the HiMed1Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMed1Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMed1Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED1_ASF_s *pHiMed1Asf)
{
    const ik_hi_med1_asf_t *p_ik_hi_med1_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_med1_asf, &pHiMed1Asf, sizeof(void *));
    return ik_set_hi_med1_asf((UINT32)pMode->ContextId, p_ik_hi_med1_asf);
}

/**
* Get the HiMed1Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMed1Asf, Get HiMed1Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMed1Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED1_ASF_s *pHiMed1Asf)
{
    ik_hi_med1_asf_t *p_ik_hi_med1_asf;

    (void)amba_ik_system_memcpy(pHiMed1Asf, pHiMed1Asf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med1_asf, &pHiMed1Asf, sizeof(void *));
    return ik_get_hi_med1_asf((UINT32)pMode->ContextId, p_ik_hi_med1_asf);
}

/**
* Set the HiMed2Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMed2Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMed2Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED2_ASF_s *pHiMed2Asf)
{
    const ik_hi_med2_asf_t *p_ik_hi_med2_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_med2_asf, &pHiMed2Asf, sizeof(void *));
    return ik_set_hi_med2_asf((UINT32)pMode->ContextId, p_ik_hi_med2_asf);
}

/**
* Get the HiMed2Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMed2Asf, Get HiMed2Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMed2Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED2_ASF_s *pHiMed2Asf)
{
    ik_hi_med2_asf_t *p_ik_hi_med2_asf;

    (void)amba_ik_system_memcpy(pHiMed2Asf, pHiMed2Asf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med2_asf, &pHiMed2Asf, sizeof(void *));
    return ik_get_hi_med2_asf((UINT32)pMode->ContextId, p_ik_hi_med2_asf);
}

/**
* Set the HiHighAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_ASF_s *pHiHighAsf)
{
    const ik_hi_high_asf_t *p_ik_hi_high_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_asf, &pHiHighAsf, sizeof(void *));
    return ik_set_hi_high_asf((UINT32)pMode->ContextId, p_ik_hi_high_asf);
}

/**
* Get the HiHighAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighAsf, Get HiHighAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_ASF_s *pHiHighAsf)
{
    ik_hi_high_asf_t *p_ik_hi_high_asf;

    (void)amba_ik_system_memcpy(pHiHighAsf, pHiHighAsf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_asf, &pHiHighAsf, sizeof(void *));
    return ik_get_hi_high_asf((UINT32)pMode->ContextId, p_ik_hi_high_asf);
}

/**
* Set the HiHigh2Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHigh2Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHigh2Asf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH2_ASF_s *pHiHigh2Asf)
{
    const ik_hi_high2_asf_t *p_ik_hi_high2_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_high2_asf, &pHiHigh2Asf, sizeof(void *));
    return ik_set_hi_high2_asf((UINT32)pMode->ContextId, p_ik_hi_high2_asf);
}

/**
* Get the HiHigh2Asf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHigh2Asf, Get HiHigh2Asf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHigh2Asf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH2_ASF_s *pHiHigh2Asf)
{
    ik_hi_high2_asf_t *p_ik_hi_high2_asf;

    (void)amba_ik_system_memcpy(pHiHigh2Asf, pHiHigh2Asf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high2_asf, &pHiHigh2Asf, sizeof(void *));
    return ik_get_hi_high2_asf((UINT32)pMode->ContextId, p_ik_hi_high2_asf);
}

/**
* Set the ChromaAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set ChromaAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_CHROMA_ASF_s *pChromaAsf)
{
    const ik_chroma_asf_t *p_ik_chroma_asf;

    (void)amba_ik_system_memcpy(&p_ik_chroma_asf, &pChromaAsf, sizeof(void *));
    return ik_set_li_chroma_asf((UINT32)pMode->ContextId, p_ik_chroma_asf);
}

/**
* Get the ChromaAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pChromaAsf, Get ChromaAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_CHROMA_ASF_s *pChromaAsf)
{
    ik_chroma_asf_t *p_ik_chroma_asf;

    (void)amba_ik_system_memcpy(pChromaAsf, pChromaAsf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_chroma_asf, &pChromaAsf, sizeof(void *));
    return ik_get_li_chroma_asf((UINT32)pMode->ContextId, p_ik_chroma_asf);
}

/**
* Set the HiChromaAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_ASF_s *pHiChromaAsf)
{
    const ik_hi_chroma_asf_t *p_ik_hi_chroma_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_asf, &pHiChromaAsf, sizeof(void *));
    return ik_set_hi_chroma_asf((UINT32)pMode->ContextId, p_ik_hi_chroma_asf);
}

/**
* Get the HiChromaAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaAsf, Get HiChromaAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_ASF_s *pHiChromaAsf)
{
    ik_hi_chroma_asf_t *p_ik_hi_chroma_asf;

    (void)amba_ik_system_memcpy(pHiChromaAsf, pHiChromaAsf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_asf, &pHiChromaAsf, sizeof(void *));
    return ik_get_hi_chroma_asf((UINT32)pMode->ContextId, p_ik_hi_chroma_asf);
}

/**
* Set the HiLowChromaAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiLowChromaAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiLowChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LOW_CHROMA_ASF_s *pHiLowChromaAsf)
{
    const ik_hi_low_chroma_asf_t *p_ik_hi_low_chroma_asf;

    (void)amba_ik_system_memcpy(&p_ik_hi_low_chroma_asf, &pHiLowChromaAsf, sizeof(void *));
    return ik_set_hi_low_chroma_asf((UINT32)pMode->ContextId, p_ik_hi_low_chroma_asf);
}

/**
* Get the HiLowChromaAsf Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiLowChromaAsf, Get HiLowChromaAsf information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiLowChromaAsf(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LOW_CHROMA_ASF_s *pHiLowChromaAsf)
{
    ik_hi_low_chroma_asf_t *p_ik_hi_low_chroma_asf;

    (void)amba_ik_system_memcpy(pHiLowChromaAsf, pHiLowChromaAsf, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_low_chroma_asf, &pHiLowChromaAsf, sizeof(void *));
    return ik_get_hi_low_chroma_asf((UINT32)pMode->ContextId, p_ik_hi_low_chroma_asf);
}

/**
* Set the HiHighShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_BOTH_s *pHiHighShpNsBoth)
{
    const ik_hi_high_shpns_both_t *p_ik_hi_high_shpns_both;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_both, &pHiHighShpNsBoth, sizeof(void *));
    return ik_set_hi_high_shpns_both((UINT32)pMode->ContextId, p_ik_hi_high_shpns_both);
}

/**
* Get the HiHighShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsBoth, Get HiHighShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_BOTH_s *pHiHighShpNsBoth)
{
    ik_hi_high_shpns_both_t *p_ik_hi_high_shpns_both;

    (void)amba_ik_system_memcpy(pHiHighShpNsBoth, pHiHighShpNsBoth, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_both, &pHiHighShpNsBoth, sizeof(void *));
    return ik_get_hi_high_shpns_both((UINT32)pMode->ContextId, p_ik_hi_high_shpns_both);
}

/**
* Set the HiHighShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_NOISE_s *pHiHighShpNsNoise)
{
    const ik_hi_high_shpns_noise_t *p_ik_hi_high_shpns_noise;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_noise, &pHiHighShpNsNoise, sizeof(void *));
    return ik_set_hi_high_shpns_noise((UINT32)pMode->ContextId, p_ik_hi_high_shpns_noise);
}

/**
* Get the HiHighShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsNoise, Get HiHighShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_NOISE_s *pHiHighShpNsNoise)
{
    ik_hi_high_shpns_noise_t *p_ik_hi_high_shpns_noise;

    (void)amba_ik_system_memcpy(pHiHighShpNsNoise, pHiHighShpNsNoise, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_noise, &pHiHighShpNsNoise, sizeof(void *));
    return ik_get_hi_high_shpns_noise((UINT32)pMode->ContextId, p_ik_hi_high_shpns_noise);
}

/**
* Set the HiHighShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_CORING_s *pHiHighShpNsCoring)
{
    const ik_hi_high_shpns_coring_t *p_ik_hi_high_shpns_coring;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_coring, &pHiHighShpNsCoring, sizeof(void *));
    return ik_set_hi_high_shpns_coring((UINT32)pMode->ContextId, p_ik_hi_high_shpns_coring);
}

/**
* Get the HiHighShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsCoring, Get HiHighShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_CORING_s *pHiHighShpNsCoring)
{
    ik_hi_high_shpns_coring_t *p_ik_hi_high_shpns_coring;

    (void)amba_ik_system_memcpy(pHiHighShpNsCoring, pHiHighShpNsCoring, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_coring, &pHiHighShpNsCoring, sizeof(void *));
    return ik_get_hi_high_shpns_coring((UINT32)pMode->ContextId, p_ik_hi_high_shpns_coring);
}

/**
* Set the HiHighShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_FIR_s *pHiHighShpNsFir)
{
    const ik_hi_high_shpns_fir_t *p_ik_hi_high_shpns_fir;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_fir, &pHiHighShpNsFir, sizeof(void *));
    return ik_set_hi_high_shpns_fir((UINT32)pMode->ContextId, p_ik_hi_high_shpns_fir);
}

/**
* Get the HiHighShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsFir, Get HiHighShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_FIR_s *pHiHighShpNsFir)
{
    ik_hi_high_shpns_fir_t *p_ik_hi_high_shpns_fir;

    (void)amba_ik_system_memcpy(pHiHighShpNsFir, pHiHighShpNsFir, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_fir, &pHiHighShpNsFir, sizeof(void *));
    return ik_get_hi_high_shpns_fir((UINT32)pMode->ContextId, p_ik_hi_high_shpns_fir);
}

/**
* Set the HiHighShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *pHiHighShpNsCorIdxScl)
{
    const ik_hi_high_shpns_cor_idx_scl_t *p_ik_hi_high_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_cor_idx_scl, &pHiHighShpNsCorIdxScl, sizeof(void *));
    return ik_set_hi_high_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_hi_high_shpns_cor_idx_scl);
}

/**
* Get the HiHighShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsCorIdxScl, Get HiHighShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_COR_IDX_SCL_s *pHiHighShpNsCorIdxScl)
{
    ik_hi_high_shpns_cor_idx_scl_t *p_ik_hi_high_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(pHiHighShpNsCorIdxScl, pHiHighShpNsCorIdxScl, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_cor_idx_scl, &pHiHighShpNsCorIdxScl, sizeof(void *));
    return ik_get_hi_high_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_hi_high_shpns_cor_idx_scl);
}

/**
* Set the HiHighShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *pHiHighShpNsMinCorRst)
{
    const ik_hi_high_shpns_min_cor_rst_t *p_ik_hi_high_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_min_cor_rst, &pHiHighShpNsMinCorRst, sizeof(void *));
    return ik_set_hi_high_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_hi_high_shpns_min_cor_rst);
}

/**
* Get the HiHighShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsMinCorRst, Get HiHighShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_MIN_COR_RST_s *pHiHighShpNsMinCorRst)
{
    ik_hi_high_shpns_min_cor_rst_t *p_ik_hi_high_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(pHiHighShpNsMinCorRst, pHiHighShpNsMinCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_min_cor_rst, &pHiHighShpNsMinCorRst, sizeof(void *));
    return ik_get_hi_high_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_hi_high_shpns_min_cor_rst);
}

/**
* Set the HiHighShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *pHiHighShpNsMaxCorRst)
{
    const ik_hi_high_shpns_max_cor_rst_t *p_ik_hi_high_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_max_cor_rst, &pHiHighShpNsMaxCorRst, sizeof(void *));
    return ik_set_hi_high_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_hi_high_shpns_max_cor_rst);
}

/**
* Get the HiHighShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsMaxCorRst, Get HiHighShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_MAX_COR_RST_s *pHiHighShpNsMaxCorRst)
{
    ik_hi_high_shpns_max_cor_rst_t *p_ik_hi_high_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(pHiHighShpNsMaxCorRst, pHiHighShpNsMaxCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_max_cor_rst, &pHiHighShpNsMaxCorRst, sizeof(void *));
    return ik_get_hi_high_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_hi_high_shpns_max_cor_rst);
}

/**
* Set the HiHighShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiHighShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiHighShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *pHiHighShpNsSclCor)
{
    const ik_hi_high_shpns_scl_cor_t *p_ik_hi_high_shpns_scl_cor;

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_scl_cor, &pHiHighShpNsSclCor, sizeof(void *));
    return ik_set_hi_high_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_hi_high_shpns_scl_cor);
}

/**
* Get the HiHighShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiHighShpNsSclCor, Get HiHighShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiHighShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_HIGH_SHPNS_SCL_COR_s *pHiHighShpNsSclCor)
{
    ik_hi_high_shpns_scl_cor_t *p_ik_hi_high_shpns_scl_cor;

    (void)amba_ik_system_memcpy(pHiHighShpNsSclCor, pHiHighShpNsSclCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_high_shpns_scl_cor, &pHiHighShpNsSclCor, sizeof(void *));
    return ik_get_hi_high_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_hi_high_shpns_scl_cor);
}

/**
* Set the HiMedShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_BOTH_s *pHiMedShpNsBoth)
{
    const ik_hi_med_shpns_both_t *p_ik_hi_med_shpns_both;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_both, &pHiMedShpNsBoth, sizeof(void *));
    return ik_set_hi_med_shpns_both((UINT32)pMode->ContextId, p_ik_hi_med_shpns_both);
}

/**
* Get the HiMedShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsBoth, Get HiMedShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_BOTH_s *pHiMedShpNsBoth)
{
    ik_hi_med_shpns_both_t *p_ik_hi_med_shpns_both;

    (void)amba_ik_system_memcpy(pHiMedShpNsBoth, pHiMedShpNsBoth, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_both, &pHiMedShpNsBoth, sizeof(void *));
    return ik_get_hi_med_shpns_both((UINT32)pMode->ContextId, p_ik_hi_med_shpns_both);
}

/**
* Set the HiMedShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_NOISE_s *pHiMedShpNsNoise)
{
    const ik_hi_med_shpns_noise_t *p_ik_hi_med_shpns_noise;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_noise, &pHiMedShpNsNoise, sizeof(void *));
    return ik_set_hi_med_shpns_noise((UINT32)pMode->ContextId, p_ik_hi_med_shpns_noise);
}

/**
* Get the HiMedShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsNoise, Get HiMedShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_NOISE_s *pHiMedShpNsNoise)
{
    ik_hi_med_shpns_noise_t *p_ik_hi_med_shpns_noise;

    (void)amba_ik_system_memcpy(pHiMedShpNsNoise, pHiMedShpNsNoise, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_noise, &pHiMedShpNsNoise, sizeof(void *));
    return ik_get_hi_med_shpns_noise((UINT32)pMode->ContextId, p_ik_hi_med_shpns_noise);
}

/**
* Set the HiMedShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_CORING_s *pHiMedShpNsCoring)
{
    const ik_hi_med_shpns_coring_t *p_ik_hi_med_shpns_coring;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_coring, &pHiMedShpNsCoring, sizeof(void *));
    return ik_set_hi_med_shpns_coring((UINT32)pMode->ContextId, p_ik_hi_med_shpns_coring);
}

/**
* Get the HiMedShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsCoring, Get HiMedShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_CORING_s *pHiMedShpNsCoring)
{
    ik_hi_med_shpns_coring_t *p_ik_hi_med_shpns_coring;

    (void)amba_ik_system_memcpy(pHiMedShpNsCoring, pHiMedShpNsCoring, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_coring, &pHiMedShpNsCoring, sizeof(void *));
    return ik_get_hi_med_shpns_coring((UINT32)pMode->ContextId, p_ik_hi_med_shpns_coring);
}

/**
* Set the HiMedShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_FIR_s *pHiMedShpNsFir)
{
    const ik_hi_med_shpns_fir_t *p_ik_hi_med_shpns_fir;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_fir, &pHiMedShpNsFir, sizeof(void *));
    return ik_set_hi_med_shpns_fir((UINT32)pMode->ContextId, p_ik_hi_med_shpns_fir);
}

/**
* Get the HiMedShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsFir, Get HiMedShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_FIR_s *pHiMedShpNsFir)
{
    ik_hi_med_shpns_fir_t *p_ik_hi_med_shpns_fir;

    (void)amba_ik_system_memcpy(pHiMedShpNsFir, pHiMedShpNsFir, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_fir, &pHiMedShpNsFir, sizeof(void *));
    return ik_get_hi_med_shpns_fir((UINT32)pMode->ContextId, p_ik_hi_med_shpns_fir);
}

/**
* Set the HiMedShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *pHiMedShpNsCorIdxScl)
{
    const ik_hi_med_shpns_cor_idx_scl_t *p_ik_hi_med_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_cor_idx_scl, &pHiMedShpNsCorIdxScl, sizeof(void *));
    return ik_set_hi_med_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_hi_med_shpns_cor_idx_scl);
}

/**
* Get the HiMedShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsCorIdxScl, Get HiMedShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_COR_IDX_SCL_s *pHiMedShpNsCorIdxScl)
{
    ik_hi_med_shpns_cor_idx_scl_t *p_ik_hi_med_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(pHiMedShpNsCorIdxScl, pHiMedShpNsCorIdxScl, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_cor_idx_scl, &pHiMedShpNsCorIdxScl, sizeof(void *));
    return ik_get_hi_med_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_hi_med_shpns_cor_idx_scl);
}

/**
* Set the HiMedShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *pHiMedShpNsMinCorRst)
{
    const ik_hi_med_shpns_min_cor_rst_t *p_ik_hi_med_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_min_cor_rst, &pHiMedShpNsMinCorRst, sizeof(void *));
    return ik_set_hi_med_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_hi_med_shpns_min_cor_rst);
}

/**
* Get the HiMedShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsMinCorRst, Get HiMedShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_MIN_COR_RST_s *pHiMedShpNsMinCorRst)
{
    ik_hi_med_shpns_min_cor_rst_t *p_ik_hi_med_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(pHiMedShpNsMinCorRst, pHiMedShpNsMinCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_min_cor_rst, &pHiMedShpNsMinCorRst, sizeof(void *));
    return ik_get_hi_med_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_hi_med_shpns_min_cor_rst);
}

/**
* Set the HiMedShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *pHiMedShpNsMaxCorRst)
{
    const ik_hi_med_shpns_max_cor_rst_t *p_ik_hi_med_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_max_cor_rst, &pHiMedShpNsMaxCorRst, sizeof(void *));
    return ik_set_hi_med_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_hi_med_shpns_max_cor_rst);
}

/**
* Get the HiMedShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsMaxCorRst, Get HiMedShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_MAX_COR_RST_s *pHiMedShpNsMaxCorRst)
{
    ik_hi_med_shpns_max_cor_rst_t *p_ik_hi_med_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(pHiMedShpNsMaxCorRst, pHiMedShpNsMaxCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_max_cor_rst, &pHiMedShpNsMaxCorRst, sizeof(void *));
    return ik_get_hi_med_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_hi_med_shpns_max_cor_rst);
}

/**
* Set the HiMedShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMedShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MED_SHPNS_SCL_COR_s *pHiMedShpNsSclCor)
{
    const ik_hi_med_shpns_scl_cor_t *p_ik_hi_med_shpns_scl_cor;

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_scl_cor, &pHiMedShpNsSclCor, sizeof(void *));
    return ik_set_hi_med_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_hi_med_shpns_scl_cor);
}

/**
* Get the HiMedShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMedShpNsSclCor, Get HiMedShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMedShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MED_SHPNS_SCL_COR_s *pHiMedShpNsSclCor)
{
    ik_hi_med_shpns_scl_cor_t *p_ik_hi_med_shpns_scl_cor;

    (void)amba_ik_system_memcpy(pHiMedShpNsSclCor, pHiMedShpNsSclCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_med_shpns_scl_cor, &pHiMedShpNsSclCor, sizeof(void *));
    return ik_get_hi_med_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_hi_med_shpns_scl_cor);
}

/**
* Set the Li2ShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_BOTH_s *pLi2ShpNsBoth)
{
    const ik_li2_shpns_both_t *p_ik_li2_shpns_both;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_both, &pLi2ShpNsBoth, sizeof(void *));
    return ik_set_li2_shpns_both((UINT32)pMode->ContextId, p_ik_li2_shpns_both);
}

/**
* Get the Li2ShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsBoth, Get Li2ShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_BOTH_s *pLi2ShpNsBoth)
{
    ik_li2_shpns_both_t *p_ik_li2_shpns_both;

    (void)amba_ik_system_memcpy(pLi2ShpNsBoth, pLi2ShpNsBoth, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_both, &pLi2ShpNsBoth, sizeof(void *));
    return ik_get_li2_shpns_both((UINT32)pMode->ContextId, p_ik_li2_shpns_both);
}

/**
* Set the Li2ShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_NOISE_s *pLi2ShpNsNoise)
{
    const ik_li2_shpns_noise_t *p_ik_li2_shpns_noise;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_noise, &pLi2ShpNsNoise, sizeof(void *));
    return ik_set_li2_shpns_noise((UINT32)pMode->ContextId, p_ik_li2_shpns_noise);
}

/**
* Get the Li2ShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsNoise, Get Li2ShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_NOISE_s *pLi2ShpNsNoise)
{
    ik_li2_shpns_noise_t *p_ik_li2_shpns_noise;

    (void)amba_ik_system_memcpy(pLi2ShpNsNoise, pLi2ShpNsNoise, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_noise, &pLi2ShpNsNoise, sizeof(void *));
    return ik_get_li2_shpns_noise((UINT32)pMode->ContextId, p_ik_li2_shpns_noise);
}

/**
* Set the Li2ShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_CORING_s *pLi2ShpNsCoring)
{
    const ik_li2_shpns_coring_t *p_ik_li2_shpns_coring;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_coring, &pLi2ShpNsCoring, sizeof(void *));
    return ik_set_li2_shpns_coring((UINT32)pMode->ContextId, p_ik_li2_shpns_coring);
}

/**
* Get the Li2ShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsCoring, Get Li2ShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_CORING_s *pLi2ShpNsCoring)
{
    ik_li2_shpns_coring_t *p_ik_li2_shpns_coring;

    (void)amba_ik_system_memcpy(pLi2ShpNsCoring, pLi2ShpNsCoring, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_coring, &pLi2ShpNsCoring, sizeof(void *));
    return ik_get_li2_shpns_coring((UINT32)pMode->ContextId, p_ik_li2_shpns_coring);
}

/**
* Set the Li2ShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_FIR_s *pLi2ShpNsFir)
{
    const ik_li2_shpns_fir_t *p_ik_li2_shpns_fir;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_fir, &pLi2ShpNsFir, sizeof(void *));
    return ik_set_li2_shpns_fir((UINT32)pMode->ContextId, p_ik_li2_shpns_fir);
}

/**
* Get the Li2ShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsFir, Get Li2ShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_FIR_s *pLi2ShpNsFir)
{
    ik_li2_shpns_fir_t *p_ik_li2_shpns_fir;

    (void)amba_ik_system_memcpy(pLi2ShpNsFir, pLi2ShpNsFir, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_fir, &pLi2ShpNsFir, sizeof(void *));
    return ik_get_li2_shpns_fir((UINT32)pMode->ContextId, p_ik_li2_shpns_fir);
}

/**
* Set the Li2ShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *pLi2ShpNsCorIdxScl)
{
    const ik_li2_shpns_cor_idx_scl_t *p_ik_li2_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_cor_idx_scl, &pLi2ShpNsCorIdxScl, sizeof(void *));
    return ik_set_li2_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_li2_shpns_cor_idx_scl);
}

/**
* Get the Li2ShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsCorIdxScl, Get Li2ShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_COR_IDX_SCL_s *pLi2ShpNsCorIdxScl)
{
    ik_li2_shpns_cor_idx_scl_t *p_ik_li2_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(pLi2ShpNsCorIdxScl, pLi2ShpNsCorIdxScl, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_cor_idx_scl, &pLi2ShpNsCorIdxScl, sizeof(void *));
    return ik_get_li2_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_li2_shpns_cor_idx_scl);
}

/**
* Set the Li2ShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *pLi2ShpNsMinCorRst)
{
    const ik_li2_shpns_min_cor_rst_t *p_ik_li2_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_min_cor_rst, &pLi2ShpNsMinCorRst, sizeof(void *));
    return ik_set_li2_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_li2_shpns_min_cor_rst);
}

/**
* Get the Li2ShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsMinCorRst, Get Li2ShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_MIN_COR_RST_s *pLi2ShpNsMinCorRst)
{
    ik_li2_shpns_min_cor_rst_t *p_ik_li2_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(pLi2ShpNsMinCorRst, pLi2ShpNsMinCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_min_cor_rst, &pLi2ShpNsMinCorRst, sizeof(void *));
    return ik_get_li2_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_li2_shpns_min_cor_rst);
}

/**
* Set the Li2ShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *pLi2ShpNsMaxCorRst)
{
    const ik_li2_shpns_max_cor_rst_t *p_ik_li2_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_max_cor_rst, &pLi2ShpNsMaxCorRst, sizeof(void *));
    return ik_set_li2_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_li2_shpns_max_cor_rst);
}

/**
* Get the Li2ShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsMaxCorRst, Get Li2ShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_MAX_COR_RST_s *pLi2ShpNsMaxCorRst)
{
    ik_li2_shpns_max_cor_rst_t *p_ik_li2_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(pLi2ShpNsMaxCorRst, pLi2ShpNsMaxCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_max_cor_rst, &pLi2ShpNsMaxCorRst, sizeof(void *));
    return ik_get_li2_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_li2_shpns_max_cor_rst);
}

/**
* Set the Li2ShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set Li2ShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetLi2ShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_SHPNS_SCL_COR_s *pLi2ShpNsSclCor)
{
    const ik_li2_shpns_scl_cor_t *p_ik_li2_shpns_scl_cor;

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_scl_cor, &pLi2ShpNsSclCor, sizeof(void *));
    return ik_set_li2_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_li2_shpns_scl_cor);
}

/**
* Get the Li2ShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pLi2ShpNsSclCor, Get Li2ShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetLi2ShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_SHPNS_SCL_COR_s *pLi2ShpNsSclCor)
{
    ik_li2_shpns_scl_cor_t *p_ik_li2_shpns_scl_cor;

    (void)amba_ik_system_memcpy(pLi2ShpNsSclCor, pLi2ShpNsSclCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_shpns_scl_cor, &pLi2ShpNsSclCor, sizeof(void *));
    return ik_get_li2_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_li2_shpns_scl_cor);
}

/**
* Set the HiliShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMedShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_BOTH_s *pHiliShpNsBoth)
{
    const ik_hili_shpns_both_t *p_ik_hili_shpns_both;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_both, &pHiliShpNsBoth, sizeof(void *));
    return ik_set_hili_shpns_both((UINT32)pMode->ContextId, p_ik_hili_shpns_both);
}

/**
* Get the HiliShpNsBoth Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsBoth, Get HiliShpNsBoth information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsBoth(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_BOTH_s *pHiliShpNsBoth)
{
    ik_hili_shpns_both_t *p_ik_hili_shpns_both;

    (void)amba_ik_system_memcpy(pHiliShpNsBoth, pHiliShpNsBoth, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_both, &pHiliShpNsBoth, sizeof(void *));
    return ik_get_hili_shpns_both((UINT32)pMode->ContextId, p_ik_hili_shpns_both);
}

/**
* Set the HiliShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_NOISE_s *pHiliShpNsNoise)
{
    const ik_hili_shpns_noise_t *p_ik_hili_shpns_noise;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_noise, &pHiliShpNsNoise, sizeof(void *));
    return ik_set_hili_shpns_noise((UINT32)pMode->ContextId, p_ik_hili_shpns_noise);
}

/**
* Get the HiliShpNsNoise Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsNoise, Get HiliShpNsNoise information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsNoise(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_NOISE_s *pHiliShpNsNoise)
{
    ik_hili_shpns_noise_t *p_ik_hili_shpns_noise;

    (void)amba_ik_system_memcpy(pHiliShpNsNoise, pHiliShpNsNoise, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_noise, &pHiliShpNsNoise, sizeof(void *));
    return ik_get_hili_shpns_noise((UINT32)pMode->ContextId, p_ik_hili_shpns_noise);
}

/**
* Set the HiliShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_CORING_s *pHiliShpNsCoring)
{
    const ik_hili_shpns_coring_t *p_ik_hili_shpns_coring;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_coring, &pHiliShpNsCoring, sizeof(void *));
    return ik_set_hili_shpns_coring((UINT32)pMode->ContextId, p_ik_hili_shpns_coring);
}

/**
* Get the HiliShpNsCoring Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsCoring, Get HiliShpNsCoring information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsCoring(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_CORING_s *pHiliShpNsCoring)
{
    ik_hili_shpns_coring_t *p_ik_hili_shpns_coring;

    (void)amba_ik_system_memcpy(pHiliShpNsCoring, pHiliShpNsCoring, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_coring, &pHiliShpNsCoring, sizeof(void *));
    return ik_get_hili_shpns_coring((UINT32)pMode->ContextId, p_ik_hili_shpns_coring);
}

/**
* Set the HiliShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_FIR_s *pHiliShpNsFir)
{
    const ik_hili_shpns_fir_t *p_ik_hili_shpns_fir;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_fir, &pHiliShpNsFir, sizeof(void *));
    return ik_set_hili_shpns_fir((UINT32)pMode->ContextId, p_ik_hili_shpns_fir);
}

/**
* Get the HiliShpNsFir Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsFir, Get HiliShpNsFir information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsFir(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_FIR_s *pHiliShpNsFir)
{
    ik_hili_shpns_fir_t *p_ik_hili_shpns_fir;

    (void)amba_ik_system_memcpy(pHiliShpNsFir, pHiliShpNsFir, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_fir, &pHiliShpNsFir, sizeof(void *));
    return ik_get_hili_shpns_fir((UINT32)pMode->ContextId, p_ik_hili_shpns_fir);
}

/**
* Set the HiliShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *pHiliShpNsCorIdxScl)
{
    const ik_hili_shpns_cor_idx_scl_t *p_ik_hili_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_cor_idx_scl, &pHiliShpNsCorIdxScl, sizeof(void *));
    return ik_set_hili_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_hili_shpns_cor_idx_scl);
}

/**
* Get the HiliShpNsCorIdxScl Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsCorIdxScl, Get HiliShpNsCorIdxScl information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsCorIdxScl(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_COR_IDX_SCL_s *pHiliShpNsCorIdxScl)
{
    ik_hili_shpns_cor_idx_scl_t *p_ik_hili_shpns_cor_idx_scl;

    (void)amba_ik_system_memcpy(pHiliShpNsCorIdxScl, pHiliShpNsCorIdxScl, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_cor_idx_scl, &pHiliShpNsCorIdxScl, sizeof(void *));
    return ik_get_hili_shpns_cor_idx_scl((UINT32)pMode->ContextId, p_ik_hili_shpns_cor_idx_scl);
}

/**
* Set the HiliShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *pHiliShpNsMinCorRst)
{
    const ik_hili_shpns_min_cor_rst_t *p_ik_hili_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_min_cor_rst, &pHiliShpNsMinCorRst, sizeof(void *));
    return ik_set_hili_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_hili_shpns_min_cor_rst);
}

/**
* Get the HiliShpNsMinCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsMinCorRst, Get HiliShpNsMinCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsMinCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_MIN_COR_RST_s *pHiliShpNsMinCorRst)
{
    ik_hili_shpns_min_cor_rst_t *p_ik_hili_shpns_min_cor_rst;

    (void)amba_ik_system_memcpy(pHiliShpNsMinCorRst, pHiliShpNsMinCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_min_cor_rst, &pHiliShpNsMinCorRst, sizeof(void *));
    return ik_get_hili_shpns_min_cor_rst((UINT32)pMode->ContextId, p_ik_hili_shpns_min_cor_rst);
}

/**
* Set the HiliShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *pHiliShpNsMaxCorRst)
{
    const ik_hili_shpns_max_cor_rst_t *p_ik_hili_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_max_cor_rst, &pHiliShpNsMaxCorRst, sizeof(void *));
    return ik_set_hili_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_hili_shpns_max_cor_rst);
}

/**
* Get the HiliShpNsMaxCorRst Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsMaxCorRst, Get HiliShpNsMaxCorRst information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsMaxCorRst(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_MAX_COR_RST_s *pHiliShpNsMaxCorRst)
{
    ik_hili_shpns_max_cor_rst_t *p_ik_hili_shpns_max_cor_rst;

    (void)amba_ik_system_memcpy(pHiliShpNsMaxCorRst, pHiliShpNsMaxCorRst, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_max_cor_rst, &pHiliShpNsMaxCorRst, sizeof(void *));
    return ik_get_hili_shpns_max_cor_rst((UINT32)pMode->ContextId, p_ik_hili_shpns_max_cor_rst);
}

/**
* Set the HiliShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_SHPNS_SCL_COR_s *pHiliShpNsSclCor)
{
    const ik_hili_shpns_scl_cor_t *p_ik_hili_shpns_scl_cor;

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_scl_cor, &pHiliShpNsSclCor, sizeof(void *));
    return ik_set_hili_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_hili_shpns_scl_cor);
}

/**
* Get the HiliShpNsSclCor Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliShpNsSclCor, Get HiliShpNsSclCor information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliShpNsSclCor(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_SHPNS_SCL_COR_s *pHiliShpNsSclCor)
{
    ik_hili_shpns_scl_cor_t *p_ik_hili_shpns_scl_cor;

    (void)amba_ik_system_memcpy(pHiliShpNsSclCor, pHiliShpNsSclCor, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_shpns_scl_cor, &pHiliShpNsSclCor, sizeof(void *));
    return ik_get_hili_shpns_scl_cor((UINT32)pMode->ContextId, p_ik_hili_shpns_scl_cor);
}

/**
* Set the HiChromaFilterHigh Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFilterHigh information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFilterHigh(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_HIGH_s *pHiChromaFilterHigh)
{
    const ik_hi_chroma_filter_high_t *p_ik_hi_chroma_filter_high;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_high, &pHiChromaFilterHigh, sizeof(void *));
    return ik_set_hi_chroma_filter_high((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_high);
}

/**
* Get the HiChromaFilterHigh Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFilterHigh, Get HiChromaFilterHigh information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFilterHigh(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_HIGH_s *pHiChromaFilterHigh)
{
    ik_hi_chroma_filter_high_t *p_ik_hi_chroma_filter_high;

    (void)amba_ik_system_memcpy(pHiChromaFilterHigh, pHiChromaFilterHigh, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_high, &pHiChromaFilterHigh, sizeof(void *));
    return ik_get_hi_chroma_filter_high((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_high);
}

/**
* Set the HiChromaFilterPre Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFilterPre information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFilterPre(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_PRE_s *pHiChromaFilterPre)
{
    const ik_hi_chroma_filter_pre_t *p_ik_hi_chroma_filter_pre;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_pre, &pHiChromaFilterPre, sizeof(void *));
    return ik_set_hi_chroma_filter_pre((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_pre);
}

/**
* Get the HiChromaFilterPre Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFilterPre, Get HiChromaFilterPre information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFilterPre(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_PRE_s *pHiChromaFilterPre)
{
    ik_hi_chroma_filter_pre_t *p_ik_hi_chroma_filter_pre;

    (void)amba_ik_system_memcpy(pHiChromaFilterPre, pHiChromaFilterPre, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_pre, &pHiChromaFilterPre, sizeof(void *));
    return ik_get_hi_chroma_filter_pre((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_pre);
}

/**
* Set the HiChromaFilterMed Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFilterMed information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFilterMed(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_MED_s *pHiChromaFilterMed)
{
    const ik_hi_chroma_filter_med_t *p_ik_hi_chroma_filter_med;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_med, &pHiChromaFilterMed, sizeof(void *));
    return ik_set_hi_chroma_filter_med((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_med);
}

/**
* Get the HiChromaFilterMed Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFilterMed, Get HiChromaFilterMed information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFilterMed(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_MED_s *pHiChromaFilterMed)
{
    ik_hi_chroma_filter_med_t *p_ik_hi_chroma_filter_med;

    (void)amba_ik_system_memcpy(pHiChromaFilterMed, pHiChromaFilterMed, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_med, &pHiChromaFilterMed, sizeof(void *));
    return ik_get_hi_chroma_filter_med((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_med);
}

/**
* Set the HiChromaFilterLow Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFilterLow information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFilterLow(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_LOW_s *pHiChromaFilterLow)
{
    const ik_hi_chroma_filter_low_t *p_ik_hi_chroma_filter_low;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_low, &pHiChromaFilterLow, sizeof(void *));
    return ik_set_hi_chroma_filter_low((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_low);
}

/**
* Get the HiChromaFilterLow Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFilterLow, Get HiChromaFilterLow information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFilterLow(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_LOW_s *pHiChromaFilterLow)
{
    ik_hi_chroma_filter_low_t *p_ik_hi_chroma_filter_low;

    (void)amba_ik_system_memcpy(pHiChromaFilterLow, pHiChromaFilterLow, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_low, &pHiChromaFilterLow, sizeof(void *));
    return ik_get_hi_chroma_filter_low((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_low);
}

/**
* Set the HiChromaFilterVeryLow Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFilterVeryLow information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFilterVeryLow(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *pHiChromaFilterVeryLow)
{
    const ik_hi_chroma_filter_very_low_t *p_ik_hi_chroma_filter_very_low;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_very_low, &pHiChromaFilterVeryLow, sizeof(void *));
    return ik_set_hi_chroma_filter_very_low((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_very_low);
}

/**
* Get the HiChromaFilterVeryLow Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFilterVeryLow, Get HiChromaFilterVeryLow information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFilterVeryLow(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FILTER_VERY_LOW_s *pHiChromaFilterVeryLow)
{
    ik_hi_chroma_filter_very_low_t *p_ik_hi_chroma_filter_very_low;

    (void)amba_ik_system_memcpy(pHiChromaFilterVeryLow, pHiChromaFilterVeryLow, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_filter_very_low, &pHiChromaFilterVeryLow, sizeof(void *));
    return ik_get_hi_chroma_filter_very_low((UINT32)pMode->ContextId, p_ik_hi_chroma_filter_very_low);
}

/**
* Set the HiLumaCombine Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiLumaCombine information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiLumaCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LUMA_COMBINE_s *pHiLumaCombine)
{
    const ik_hi_luma_combine_t *p_ik_hi_luma_combine;

    (void)amba_ik_system_memcpy(&p_ik_hi_luma_combine, &pHiLumaCombine, sizeof(void *));
    return ik_set_hi_luma_combine((UINT32)pMode->ContextId, p_ik_hi_luma_combine);
}

/**
* Get the HiLumaCombine Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiLumaCombine, Get HiLumaCombine information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiLumaCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LUMA_COMBINE_s *pHiLumaCombine)
{
    ik_hi_luma_combine_t *p_ik_hi_luma_combine;

    (void)amba_ik_system_memcpy(pHiLumaCombine, pHiLumaCombine, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_luma_combine, &pHiLumaCombine, sizeof(void *));
    return ik_get_hi_luma_combine((UINT32)pMode->ContextId, p_ik_hi_luma_combine);
}

/**
* Set the HiLowASFCombine Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiLowASFCombine information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiLowASFCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LOW_ASF_COMBINE_s *pHiLowASFCombine)
{
    const ik_hi_low_asf_combine_t *p_ik_hi_low_asf_combine;

    (void)amba_ik_system_memcpy(&p_ik_hi_low_asf_combine, &pHiLowASFCombine, sizeof(void *));
    return ik_set_hi_low_asf_combine((UINT32)pMode->ContextId, p_ik_hi_low_asf_combine);
}

/**
* Get the HiLowASFCombine Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiLowASFCombine, Get HiLowASFCombine information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiLowASFCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LOW_ASF_COMBINE_s *pHiLowASFCombine)
{
    ik_hi_low_asf_combine_t *p_ik_hi_low_asf_combine;

    (void)amba_ik_system_memcpy(pHiLowASFCombine, pHiLowASFCombine, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_low_asf_combine, &pHiLowASFCombine, sizeof(void *));
    return ik_get_hi_low_asf_combine((UINT32)pMode->ContextId, p_ik_hi_low_asf_combine);
}

/**
* Set the HiChromaFltrMedCom Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFltrMedCom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFltrMedCom(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *pHiChromaFltrMedCom)
{
    const ik_hi_chroma_fltr_med_com_t *p_ik_hi_chroma_fltr_med_com;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_fltr_med_com, &pHiChromaFltrMedCom, sizeof(void *));
    return ik_set_hi_chroma_fltr_med_com((UINT32)pMode->ContextId, p_ik_hi_chroma_fltr_med_com);
}

/**
* Get the HiChromaFltrMedCom Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFltrMedCom, Get HiChromaFltrMedCom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFltrMedCom(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FLTR_MED_COM_s *pHiChromaFltrMedCom)
{
    ik_hi_chroma_fltr_med_com_t *p_ik_hi_chroma_fltr_med_com;

    (void)amba_ik_system_memcpy(pHiChromaFltrMedCom, pHiChromaFltrMedCom, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_fltr_med_com, &pHiChromaFltrMedCom, sizeof(void *));
    return ik_get_hi_chroma_fltr_med_com((UINT32)pMode->ContextId, p_ik_hi_chroma_fltr_med_com);
}

/**
* Set the HiChromaFltrLowCom Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFltrLowCom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFltrLowCom(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *pHiChromaFltrLowCom)
{
    const ik_hi_chroma_fltr_low_com_t *p_ik_hi_chroma_fltr_low_com;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_fltr_low_com, &pHiChromaFltrLowCom, sizeof(void *));
    return ik_set_hi_chroma_fltr_low_com((UINT32)pMode->ContextId, p_ik_hi_chroma_fltr_low_com);
}

/**
* Get the HiChromaFltrLowCom Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFltrLowCom, Get HiChromaFltrLowCom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFltrLowCom(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FLTR_LOW_COM_s *pHiChromaFltrLowCom)
{
    ik_hi_chroma_fltr_low_com_t *p_ik_hi_chroma_fltr_low_com;

    (void)amba_ik_system_memcpy(pHiChromaFltrLowCom, pHiChromaFltrLowCom, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_fltr_low_com, &pHiChromaFltrLowCom, sizeof(void *));
    return ik_get_hi_chroma_fltr_low_com((UINT32)pMode->ContextId, p_ik_hi_chroma_fltr_low_com);
}

/**
* Set the HiChromaFltrVeryLowCom Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiChromaFltrVeryLowCom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiChromaFltrVeryLowCom(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *pHiChromaFltrVeryLowCom)
{
    const ik_hi_chroma_fltr_very_low_com_t *p_ik_hi_chroma_fltr_very_low_com;

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_fltr_very_low_com, &pHiChromaFltrVeryLowCom, sizeof(void *));
    return ik_set_hi_chroma_fltr_very_low_com((UINT32)pMode->ContextId, p_ik_hi_chroma_fltr_very_low_com);
}

/**
* Get the HiChromaFltrVeryLowCom Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiChromaFltrVeryLowCom, Get HiChromaFltrVeryLowCom information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiChromaFltrVeryLowCom(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_CHROMA_FLTR_VERY_LOW_COM_s *pHiChromaFltrVeryLowCom)
{
    ik_hi_chroma_fltr_very_low_com_t *p_ik_hi_chroma_fltr_very_low_com;

    (void)amba_ik_system_memcpy(pHiChromaFltrVeryLowCom, pHiChromaFltrVeryLowCom, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_chroma_fltr_very_low_com, &pHiChromaFltrVeryLowCom, sizeof(void *));
    return ik_get_hi_chroma_fltr_very_low_com((UINT32)pMode->ContextId, p_ik_hi_chroma_fltr_very_low_com);
}

/**
* Set the HiliCombine Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiliCombine information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiliCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HILI_COMBINE_s *pHiliCombine)
{
    const ik_hili_combine_t *p_ik_hili_combine;

    (void)amba_ik_system_memcpy(&p_ik_hili_combine, &pHiliCombine, sizeof(void *));
    return ik_set_hili_combine((UINT32)pMode->ContextId, p_ik_hili_combine);
}

/**
* Get the HiliCombine Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiliCombine, Get HiliCombine information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiliCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HILI_COMBINE_s *pHiliCombine)
{
    ik_hili_combine_t *p_ik_hili_combine;

    (void)amba_ik_system_memcpy(pHiliCombine, pHiliCombine, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hili_combine, &pHiliCombine, sizeof(void *));
    return ik_get_hili_combine((UINT32)pMode->ContextId, p_ik_hili_combine);
}

/**
* Set the HiMidHighFreqRecover Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiMidHighFreqRecover information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiMidHighFreqRecover(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *pHiMidHighFreqRecover)
{
    const ik_hi_mid_high_freq_recover_t *p_ik_hi_mid_high_freq_recover;

    (void)amba_ik_system_memcpy(&p_ik_hi_mid_high_freq_recover, &pHiMidHighFreqRecover, sizeof(void *));
    return ik_set_hi_mid_high_freq_recover((UINT32)pMode->ContextId, p_ik_hi_mid_high_freq_recover);
}

/**
* Get the HiMidHighFreqRecover Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiMidHighFreqRecover, Get HiMidHighFreqRecover information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiMidHighFreqRecover(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_MID_HIGH_FREQ_RECOVER_s *pHiMidHighFreqRecover)
{
    ik_hi_mid_high_freq_recover_t *p_ik_hi_mid_high_freq_recover;

    (void)amba_ik_system_memcpy(pHiMidHighFreqRecover, pHiMidHighFreqRecover, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_mid_high_freq_recover, &pHiMidHighFreqRecover, sizeof(void *));
    return ik_get_hi_mid_high_freq_recover((UINT32)pMode->ContextId, p_ik_hi_mid_high_freq_recover);
}

/**
* Set the HiLumaBlend Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiLumaBlend information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiLumaBlend(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_LUMA_BLEND_s *pHiLumaBlend)
{
    const ik_hi_luma_blend_t *p_ik_hi_luma_blend;

    (void)amba_ik_system_memcpy(&p_ik_hi_luma_blend, &pHiLumaBlend, sizeof(void *));
    return ik_set_hi_luma_blend((UINT32)pMode->ContextId, p_ik_hi_luma_blend);
}

/**
* Get the HiLumaBlend Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiLumaBlend, Get HiLumaBlend information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiLumaBlend(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_LUMA_BLEND_s *pHiLumaBlend)
{
    ik_hi_luma_blend_t *p_ik_hi_luma_blend;

    (void)amba_ik_system_memcpy(pHiLumaBlend, pHiLumaBlend, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_luma_blend, &pHiLumaBlend, sizeof(void *));
    return ik_get_hi_luma_blend((UINT32)pMode->ContextId, p_ik_hi_luma_blend);
}

/**
* Set the HiNonSmoothDetect Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiNonSmoothDetect information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiNonSmoothDetect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_NONSMOOTH_DETECT_s *pHiNonSmoothDetect)
{
    const ik_hi_nonsmooth_detect_t *p_ik_hi_nonsmooth_detect;

    (void)amba_ik_system_memcpy(&p_ik_hi_nonsmooth_detect, &pHiNonSmoothDetect, sizeof(void *));
    return ik_set_hi_nonsmooth_detect((UINT32)pMode->ContextId, p_ik_hi_nonsmooth_detect);
}

/**
* Get the HiNonSmoothDetect Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiNonSmoothDetect, Get HiNonSmoothDetect information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiNonSmoothDetect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_NONSMOOTH_DETECT_s *pHiNonSmoothDetect)
{
    ik_hi_nonsmooth_detect_t *p_ik_hi_nonsmooth_detect;

    (void)amba_ik_system_memcpy(pHiNonSmoothDetect, pHiNonSmoothDetect, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_nonsmooth_detect, &pHiNonSmoothDetect, sizeof(void *));
    return ik_get_hi_nonsmooth_detect((UINT32)pMode->ContextId, p_ik_hi_nonsmooth_detect);
}

/**
* Set the HiSelect Information of each context
* @param [in]  pMode, Set ContextId
* @param [in]  pVinSensorInfo, Set HiSelect information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetHiSelect(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_HI_SELECT_s *pHiSelect)
{
    const ik_hi_select_t *p_ik_hi_select;

    (void)amba_ik_system_memcpy(&p_ik_hi_select, &pHiSelect, sizeof(void *));
    return ik_set_hi_select((UINT32)pMode->ContextId, p_ik_hi_select);
}

/**
* Get the HiSelect Information of each context
* @param [in]  pMode, Set ContextId
* @param [in,out]  pHiSelect, Get HiSelect information
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetHiSelect(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_HI_SELECT_s *pHiSelect)
{
    ik_hi_select_t *p_ik_hi_select;

    (void)amba_ik_system_memcpy(pHiSelect, pHiSelect, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_select, &pHiSelect, sizeof(void *));
    return ik_get_hi_select((UINT32)pMode->ContextId, p_ik_hi_select);
}


UINT32 AmbaIK_SetHiLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LUMA_NOISE_REDUCTION_s *pHiLumaNoiseReduction)
{
    UINT32 rval = 0u;
    const ik_luma_noise_reduction_t *p_ik_hi_luma_noise_reduction;

    (void)amba_ik_system_memcpy(&p_ik_hi_luma_noise_reduction, &pHiLumaNoiseReduction, sizeof(void *));
    rval |= ik_set_hi_luma_noise_reduction((UINT32)pMode->ContextId, p_ik_hi_luma_noise_reduction);

    return rval;
}

UINT32 AmbaIK_GetHiLumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LUMA_NOISE_REDUCTION_s *pHiLumaNoiseReduction)
{
    ik_luma_noise_reduction_t *p_ik_hi_luma_noise_reduction;

    (void)amba_ik_system_memcpy(pHiLumaNoiseReduction, pHiLumaNoiseReduction, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_hi_luma_noise_reduction, &pHiLumaNoiseReduction, sizeof(void *));
    return ik_get_hi_luma_noise_reduction((UINT32)pMode->ContextId, p_ik_hi_luma_noise_reduction);
}

UINT32 AmbaIK_SetLi2LumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LUMA_NOISE_REDUCTION_s *pLi2LumaNoiseReduction)
{
    UINT32 rval = 0u;
    const ik_luma_noise_reduction_t *p_ik_li2_luma_noise_reduction;

    (void)amba_ik_system_memcpy(&p_ik_li2_luma_noise_reduction, &pLi2LumaNoiseReduction, sizeof(void *));
    rval |= ik_set_li2_luma_noise_reduction((UINT32)pMode->ContextId, p_ik_li2_luma_noise_reduction);

    return rval;
}

UINT32 AmbaIK_GetLi2LumaNoiseReduction(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LUMA_NOISE_REDUCTION_s *pLi2LumaNoiseReduction)
{
    ik_luma_noise_reduction_t *p_ik_li2_luma_noise_reduction;

    (void)amba_ik_system_memcpy(pLi2LumaNoiseReduction, pLi2LumaNoiseReduction, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_luma_noise_reduction, &pLi2LumaNoiseReduction, sizeof(void *));
    return ik_get_li2_luma_noise_reduction((UINT32)pMode->ContextId, p_ik_li2_luma_noise_reduction);
}

UINT32 AmbaIK_SetLi2WideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_WIDE_CHROMA_FILTER_s *pLi2WideChromaFilter)
{
    const ik_li2_wide_chroma_filter_t *p_ik_li2_wide_chroma_filter;

    (void)amba_ik_system_memcpy(&p_ik_li2_wide_chroma_filter, &pLi2WideChromaFilter, sizeof(void *));
    return ik_set_li2_wide_chroma_filter((UINT32)pMode->ContextId, p_ik_li2_wide_chroma_filter);
}

UINT32 AmbaIK_GetLi2WideChromaFilter(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_WIDE_CHROMA_FILTER_s       *pLi2WideChromaFilter)
{
    ik_li2_wide_chroma_filter_t *p_ik_li2_wide_chroma_filter;

    (void)amba_ik_system_memcpy(pLi2WideChromaFilter, pLi2WideChromaFilter, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_wide_chroma_filter, &pLi2WideChromaFilter, sizeof(void *));
    return ik_get_li2_wide_chroma_filter((UINT32)pMode->ContextId, p_ik_li2_wide_chroma_filter);
}

UINT32 AmbaIK_SetLi2WideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_LI2_WIDE_CHROMA_FILTER_COMBINE_s *pLi2WideChromaFilterCombine)
{
    const ik_li2_wide_chroma_filter_combine_t *p_ik_li2_wide_chroma_filter_combine;

    (void)amba_ik_system_memcpy(&p_ik_li2_wide_chroma_filter_combine, &pLi2WideChromaFilterCombine, sizeof(void *));
    return ik_set_li2_wide_chroma_filter_combine((UINT32)pMode->ContextId, p_ik_li2_wide_chroma_filter_combine);
}

UINT32 AmbaIK_GetLi2WideChromaFilterCombine(const AMBA_IK_MODE_CFG_s *pMode, AMBA_IK_LI2_WIDE_CHROMA_FILTER_COMBINE_s       *pLi2WideChromaFilterCombine)
{
    ik_li2_wide_chroma_filter_combine_t *p_ik_li2_wide_chroma_filter_combine;

    (void)amba_ik_system_memcpy(pLi2WideChromaFilterCombine, pLi2WideChromaFilterCombine, sizeof(UINT8));//misraC

    (void)amba_ik_system_memcpy(&p_ik_li2_wide_chroma_filter_combine, &pLi2WideChromaFilterCombine, sizeof(void *));
    return ik_get_li2_wide_chroma_filter_combine((UINT32)pMode->ContextId, p_ik_li2_wide_chroma_filter_combine);
}


