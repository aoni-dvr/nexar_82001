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
#include "AmbaKAL.h"
#include "AmbaDSP_ImageUtility.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImageFilterInternal.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImgDebugPrint.h"
#include "AmbaDSP_ImgArchComponentIF.h"

UINT32 AmbaIK_GetFilterDefaultValues(const AMBA_IK_USER_PARAMETERS_s *pDefaultValues);


/**
* Calculates the memory usage based on user's requirement
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pTotalCtxSize, Returned IK required working buffer memory size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryArchMemorySize(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, const SIZE_t *pTotalCtxSize)
{
    ik_context_setting_t context_setting= {0};
    size_t *p_total_size;
    INT32 i;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<(INT32)MAX_CONTEXT_NUM; i++) {
        context_setting.config_setting[i].config_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.config_setting[i].ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t*));
        context_setting.config_setting[i].default_setting = NULL;
    }

    //(void)amba_ik_system_memcpy(&p_context_setting, &pSettingOfEachContext, sizeof(void *));
    (void)amba_ik_system_memcpy(&p_total_size, &pTotalCtxSize, sizeof(void *));

    return ik_query_arch_memory_size(&context_setting, p_total_size);
}


/**
* Specify the memory space for IK.
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pMemAddr, Memory buffer address for Image Kernel working buffer
* @param [in]  MemSize, Memory buffer size for Image Kernel Working buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitArch(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, void *pMemAddr, SIZE_t MemSize)
{
    ik_context_setting_t context_setting;
    size_t ik_mem_size = MemSize;
    INT32 i;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<(INT32)MAX_CONTEXT_NUM; i++) {
        context_setting.config_setting[i].config_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.config_setting[i].ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t*));
        context_setting.config_setting[i].default_setting = NULL;
    }

    //(void)amba_ik_system_memcpy(&p_context_setting, &pSettingOfEachContext, sizeof(void *));
    return ik_init_arch(&context_setting, pMemAddr, ik_mem_size, AMBA_IK_ARCH_HARD_INIT);
}

/**
* Initialize the image kernel print debug log function.
* @param [in]  void
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitPrintFunc(void)
{
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    UINT32 Rval;
    Rval = img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if (Rval == IK_OK) {
        if(p_ik_buffer_info->init_flag == 1UL) {
            ik_init_debug_print();
            ik_hook_debug_print();
        } else {
            Rval = IK_ERR_0001;
        }
    }
    return Rval;
}

/**
* Initialize the image kernel filter parameter check function
* @param [in]  void
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitCheckParamFunc(void)
{
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    UINT32 Rval;
    Rval = img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if (Rval == IK_OK) {
        if(p_ik_buffer_info->init_flag == 1UL) {
            ik_hook_debug_check();
        } else {
            Rval = IK_ERR_0001;
        }
    }
    return Rval;
}

/**
* Get default value of every filter of the specified context
* @param [in,out]  pDefaultValues, Get default value of every filter
* The api is internal and we don't need to verify it.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetFilterDefaultValues(const AMBA_IK_USER_PARAMETERS_s *pDefaultValues)
{
    ik_user_parameters_t *p_user_parameters;

    (void)amba_ik_system_memcpy(&p_user_parameters, &pDefaultValues, sizeof(void *));

    return ik_get_filter_default_values(p_user_parameters);
}


/**
* Initialize ability of the specified context
* @param [in]  pMode, Set ContextId
* @param [in]  pAbility, Set ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitContext(const AMBA_IK_MODE_CFG_s *pMode,const AMBA_IK_ABILITY_s *pAbility)
{
    const ik_user_parameters_t *p_default_values = NULL;
    const ik_ability_t *p_ik_ability;

    (void)amba_ik_system_memcpy(&p_ik_ability, &pAbility, sizeof(void *));

    return ik_init_context(pMode->ContextId, p_ik_ability, p_default_values);
}


/**
* Get the ability of context ID
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAbility, Get ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetContextAbility(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility)
{
    ik_ability_t *p_ik_ability;

    (void)amba_ik_system_memcpy(&p_ik_ability, &pAbility, sizeof(void *));

    return ik_get_context_ability(pMode->ContextId, p_ik_ability);
}


/**
* To trigger image kernel to compose user assigned image quality parameters of context ID to configuration
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExecuteContainer, Returned composed config stored in container
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_ExecuteConfig(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    ik_execute_container_t execute_container;
    UINT32 rval  = IK_OK;
    AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer_copy;

    (void)amba_ik_system_memcpy(&pExecuteContainer_copy, &pExecuteContainer, sizeof(void *));

    rval = ik_execute_config(pMode->ContextId, &execute_container);
    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(pExecuteContainer_copy, &execute_container, sizeof(AMBA_IK_EXECUTE_CONTAINER_s));
    }
    return rval;
}

UINT32 AmbaIK_ExecuteConfigNoWarp(const AMBA_IK_MODE_CFG_s *pMode,const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    ik_execute_container_t execute_container;
    UINT32 rval  = IK_OK;
    AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer_copy;

    (void)amba_ik_system_memcpy(&pExecuteContainer_copy, &pExecuteContainer, sizeof(void *));

    rval = ik_execute_config_eis_1st(pMode->ContextId, &execute_container);
    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(pExecuteContainer_copy, &execute_container, sizeof(AMBA_IK_EXECUTE_CONTAINER_s));
    }
    return rval;
}

UINT32 AmbaIK_ExecuteConfigUpdateWarp(const AMBA_IK_MODE_CFG_s *pMode, UINT32 ExplicitIkId, const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    ik_execute_container_t execute_container;
    UINT32 rval  = IK_OK;
    AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer_copy;

    (void)amba_ik_system_memcpy(&pExecuteContainer_copy, &pExecuteContainer, sizeof(void *));

    rval = ik_execute_config_eis_2nd(pMode->ContextId, ExplicitIkId, &execute_container);
    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(pExecuteContainer_copy, &execute_container, sizeof(AMBA_IK_EXECUTE_CONTAINER_s));
    }
    return rval;
}

/**
* Calculates the memory usage based for debug config dump
* @param [in,out]  pSize, Returned required memory size for debug config dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryDebugConfigSize(const SIZE_t *pSize)
{
    size_t *p_ik_size;
    (void)amba_ik_system_memcpy(&p_ik_size, &pSize, sizeof(void *));
    return ik_query_debug_config_size(p_ik_size);
}


/**
* Dump debug config of specified config
* @param [in]  pMode, Set ContextId
* @param [in]  PreviousCount, Previous count of composed config
* @param [in,out]  pDumpConfigBuffer, Memory buffer pointer prepared by USER to store debug config dump
* @param [in]  Size, the size for debug config dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_DumpDebugConfig(const AMBA_IK_MODE_CFG_s *pMode, UINT32 PreviousCount,const void *pDumpConfigBuffer, SIZE_t Size)
{
    UINT32 Rval;
    Rval = ik_dump_debug_config(pMode->ContextId, PreviousCount, pDumpConfigBuffer, Size);
    return Rval;
}


/**
*  Get a theoretical clock value under single channel case
* @param [in]  pQueryIdspClockInfo, Query input information
* @param [in,out]  pQueryIdspClockRst, Queried output result
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryIdspClock(const AMBA_IK_QUERY_IDSP_CLOCK_INFO_s *pQueryIdspClockInfo, const AMBA_IK_QUERY_IDSP_CLOCK_RST_s *pQueryIdspClockRst)
{
    UINT32 Rval;
    const ik_query_idsp_clock_info_t *p_ik_query_idsp_clock_info;
    ik_query_idsp_clock_rst_t *p_ik_query_idsp_clock_rst;
    if ((pQueryIdspClockInfo != NULL) && (pQueryIdspClockRst != NULL)) {
        (void) amba_ik_system_memcpy(&p_ik_query_idsp_clock_info, &pQueryIdspClockInfo, sizeof(ik_query_idsp_clock_info_t*));
        (void) amba_ik_system_memcpy(&p_ik_query_idsp_clock_rst, &pQueryIdspClockRst, sizeof(ik_query_idsp_clock_rst_t*));
        Rval = ik_query_idsp_clock(p_ik_query_idsp_clock_info, p_ik_query_idsp_clock_rst);
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  Query the calculate geometry settings required buffer sizes
* @param [out]  pBufSizeInfo, returned required buffer size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryCalcGeoBufSize(const AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s *pBufSizeInfo)
{
    UINT32 Rval;
    ik_query_calc_geo_buf_size_t *p_ik_buf_size_info;
    if (pBufSizeInfo != NULL) {
        (void) amba_ik_system_memcpy(&p_ik_buf_size_info, &pBufSizeInfo, sizeof(ik_query_calc_geo_buf_size_t*));
        Rval = ik_query_calc_geo_buf_size(p_ik_buf_size_info);
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  Calculate the geometry settings
* @param [in,out]  pInfo, Calc input information and working buffer and output buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_CalcGeoSettings(const AMBA_IK_CALC_GEO_SETTINGS_s *pInfo, const AMBA_IK_CALC_GEO_RESULT_s *pGeoRst)
{
    UINT32 Rval;
    const ik_calc_geo_settings_t *p_ik_info;
    const ik_calc_geo_result_t *p_result;
    if ((pInfo != NULL) && (pGeoRst != NULL)) {
        (void) amba_ik_system_memcpy(&p_ik_info, &pInfo, sizeof(ik_calc_geo_settings_t*));
        (void) amba_ik_system_memcpy(&p_result, &pGeoRst, sizeof(ik_calc_geo_result_t*));
        Rval = ik_calc_geo_settings(p_ik_info, p_result);
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

UINT32 AmbaIK_CalcWarpResources(const AMBA_IK_QUERY_WARP_s *pQueryParams, AMBA_IK_QUERY_WARP_RESULT_s *pQueryResult)
{
    UINT32 Rval;
    const ik_query_warp_t *p_usr_cfg;
    ik_query_warp_result_t QueryWarpResult;

    (void)amba_ik_system_memcpy(&p_usr_cfg, &pQueryParams, sizeof(void *));
    (void)amba_ik_system_memset(&QueryWarpResult, 0x0, sizeof(ik_query_warp_result_t));

    Rval = ik_calc_warp_resources(p_usr_cfg, &QueryWarpResult);

    pQueryResult->WaitLine = QueryWarpResult.wait_line;
    pQueryResult->DmaSize = QueryWarpResult.dma_size;
    pQueryResult->VerticalWarpVectorFlipNum = QueryWarpResult.warp_vector_v_flip_num;
    pQueryResult->DmyBroken = QueryWarpResult.dmy_broken;
    (void)amba_ik_system_memcpy(&pQueryResult->SuggestedDmyRange, &QueryWarpResult.suggested_dmy_range, sizeof(AMBA_IK_DUMMY_MARGIN_RANGE_s));

    return Rval;
}

