/*
*  @file AmbaDSP_ImageUtility.c
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
* MERCHANTABILITY, AND FITNESS FOR A PARTICUAR PURPOSE ARE DISCLAIMED.
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
#include "AmbaDSP_ImgConfigEntity.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImgDebugPrint.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaDSP_IksUtility.h"
#include "AmbaDSP_ImgAdvancedFilter.h"

#ifdef SUPPORT_IK_SAFETY
/**
* Calculates the memory usage based on user's requirement
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pTotalCtxSize, Returned IK required working buffer memory size
* @param [in]  SafetyEnable, enable IK safety check
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryArchMemorySize(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, const SIZE_t *pTotalCtxSize, UINT32 SafetyEnable)
{
    UINT32 rval = IK_OK;
    ik_context_setting_t context_setting= {0};
    size_t *p_total_size;
    size_t ctx_memory = 0, flow_ctrl_memory = 0, flow_tbl_memory = 0, flow_tbl_list_memory = 0;
    size_t crc_data_memory = 0;
    UINT32 i;
    size_t iks_ctx_size = 0;
    size_t iks_cr_size = 0;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<MAX_CONTEXT_NUM; i++) {
        context_setting.cr_ring_setting[i].cr_ring_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.cr_ring_setting[i].p_ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t *));
    }

    (void)amba_ik_system_memcpy(&p_total_size, &pTotalCtxSize, sizeof(void *));

    rval = ik_query_arch_memory_size(&context_setting, &ctx_memory, &flow_ctrl_memory, &flow_tbl_memory, &flow_tbl_list_memory, &crc_data_memory);
    *p_total_size = ctx_memory + flow_ctrl_memory + flow_tbl_memory + flow_tbl_list_memory + crc_data_memory;

    if(rval == IK_OK) {
        if(SafetyEnable == 1u) {
            rval |= iks_query_arch_memory_size(&context_setting, &iks_ctx_size, &iks_cr_size);
            *p_total_size += (iks_ctx_size + iks_cr_size);
        }
    }

    return rval;
}

/**
* Specify the memory space for IK.
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pMemAddr, Memory buffer address for Image Kernel working buffer
* @param [in]  MemSize, Memory buffer size for Image Kernel Working buffer
* @param [in]  SafetyEnable, enable IK safety check
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitArch(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, void *pMemAddr, SIZE_t MemSize, UINT32 SafetyEnable)
{
    UINT32 rval = IK_OK;
    ik_context_setting_t context_setting;
    size_t mem_size = MemSize;
    INT32 i;
    iks_check_info_t info;
    size_t total_size = 0, ctx_memory = 0, flow_ctrl_memory = 0, flow_tbl_memory = 0, flow_tbl_list_memory = 0;
    size_t crc_data_memory = 0;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<(INT32)MAX_CONTEXT_NUM; i++) {
        context_setting.cr_ring_setting[i].cr_ring_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.cr_ring_setting[i].p_ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t *));
    }

    rval |= ik_init_arch(&context_setting, pMemAddr, mem_size, AMBA_IK_ARCH_HARD_INIT);
    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if (rval == IK_OK) {
        if(p_ik_buffer_info->init_flag == 1u) {
            p_ik_buffer_info->safety_enable = SafetyEnable;
        }

        if(SafetyEnable == 1u) {
            info.ik_major_num = IK_VERSION_MAJOR;
            info.iks_major_num = IKS_VERSION_MAJOR;

            rval |= ik_query_arch_memory_size(&context_setting, &ctx_memory, &flow_ctrl_memory, &flow_tbl_memory, &flow_tbl_list_memory, &crc_data_memory);
            total_size = ctx_memory + flow_ctrl_memory + flow_tbl_memory + flow_tbl_list_memory + crc_data_memory;
            total_size = (total_size + 63) & (~63);

            rval |= iks_init_arch(&context_setting, pMemAddr + total_size, mem_size - total_size, AMBA_IK_ARCH_HARD_INIT, &info, SafetyEnable);
        }
    }

    return rval;
}

/**
* Specify the def binary address for IK.
* @param [in]  pBinDataAddr, Memory buffer address for def binary
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitDefBinary(void *pBinDataAddr)
{
    UINT32 rval = IK_OK;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    rval |= ik_init_default_binary(pBinDataAddr);
    if(rval == IK_OK) {
        rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
        if (rval == IK_OK) {
            if((p_ik_buffer_info->init_flag == 1u) && (p_ik_buffer_info->safety_enable == 1u)) {
                iks_init_default_binary(pBinDataAddr);
            }
        }
    }

    return rval;
}

/**
* Initialize ability of the specified context
* @param [in]  pMode, Set ContextId
* @param [in]  pAbility, Set ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitContext(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility)
{
    UINT32 rval = IK_OK;
    const ik_ability_t *p_ability;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    (void)amba_ik_system_memcpy(&p_ability, &pAbility, sizeof(void *));

    rval |= ik_init_context(pMode->ContextId, p_ability);
    if(rval == IK_OK) {
        rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
        if (rval == IK_OK) {
            if(p_ik_buffer_info->safety_enable == 1u) {
                rval |= iks_init_context(pMode->ContextId, p_ability);
            }
        }
    }

    return rval;
}

/**
* To trigger image kernel to compose user assigned image quality parameters of context ID to configuration
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExecuteContainer, Returned composed config stored in container
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_ExecuteConfig(const AMBA_IK_MODE_CFG_s *pMode,const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    UINT32 rval = 0u;
    ik_execute_container_t *p_execute_container;
    ik_execute_container_t iks_execute_container;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    (void)amba_ik_system_memcpy(&p_execute_container, &pExecuteContainer, sizeof(void *));

    rval |= ik_execute(pMode->ContextId, p_execute_container);
    if(rval == IK_OK) {
        rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
        if (rval == IK_OK) {
            if(p_ik_buffer_info->safety_enable == 1u) {
                rval |= iks_execute(pMode->ContextId, &iks_execute_container);
            }
        }
    }

    return rval;
}

/**
* Set IK safety parameters
* @param [in]  pMode, Set ContextId
* @param [in]  pSafetyInfo, Set safety parameters of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_SetSafetyInfo(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_SAFETY_INFO_s *pSafetyInfo)
{
    UINT32 rval = 0u;
    const iks_safety_info_t *p_safety_info;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if(rval == IK_OK) {
        if(p_ik_buffer_info->safety_enable == 1u) {
            (void)amba_ik_system_memcpy(&p_safety_info, &pSafetyInfo, sizeof(void *));
            rval |= iks_set_safety_info((UINT32)pMode->ContextId, p_safety_info);
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] img_arch_get_ik_working_buffer fail, %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
    }

    return rval;
}

#else
/**
* Calculates the memory usage based on user's requirement
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pTotalCtxSize, Returned IK required working buffer memory size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryArchMemorySize(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, const SIZE_t *pTotalCtxSize)
{
    uint32 rval = IK_OK;
    ik_context_setting_t context_setting= {0};
    size_t *p_total_size;
    size_t ctx_memory = 0;
    size_t flow_ctrl_memory = 0;
    size_t flow_tbl_memory = 0;
    size_t flow_tbl_list_memory = 0;
    size_t crc_data_memory = 0;
    UINT32 i;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<MAX_CONTEXT_NUM; i++) {
        context_setting.cr_ring_setting[i].cr_ring_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.cr_ring_setting[i].p_ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t *));
    }

    (void)amba_ik_system_memcpy(&p_total_size, &pTotalCtxSize, sizeof(void *));

    rval = ik_query_arch_memory_size(&context_setting, &ctx_memory, &flow_ctrl_memory, &flow_tbl_memory, &flow_tbl_list_memory, &crc_data_memory);
    *p_total_size = ctx_memory + flow_ctrl_memory + flow_tbl_memory + flow_tbl_list_memory + crc_data_memory;

    return rval;
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
    size_t mem_size = MemSize;
    INT32 i;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<(INT32)MAX_CONTEXT_NUM; i++) {
        context_setting.cr_ring_setting[i].cr_ring_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.cr_ring_setting[i].p_ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t *));
    }

    return ik_init_arch(&context_setting, pMemAddr, mem_size, AMBA_IK_ARCH_HARD_INIT);
}

/**
* Specify the memory space for IK.
* @param [in]  pSettingOfEachContext, Context setting which user wants to assign in Image Kernel
* @param [in]  pMemAddr, Memory buffer address for Image Kernel working buffer
* @param [in]  MemSize, Memory buffer size for Image Kernel Working buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitArchSoft(const AMBA_IK_CONTEXT_SETTING_s *pSettingOfEachContext, void *pMemAddr, SIZE_t MemSize)
{
    ik_context_setting_t context_setting;
    size_t mem_size = MemSize;
    INT32 i;

    context_setting.context_number = pSettingOfEachContext->ContextNumber;
    for (i=0; i<(INT32)MAX_CONTEXT_NUM; i++) {
        context_setting.cr_ring_setting[i].cr_ring_number = pSettingOfEachContext->ConfigSetting[i].ConfigNumber;
        (void)amba_ik_system_memcpy(&context_setting.cr_ring_setting[i].p_ability, &pSettingOfEachContext->ConfigSetting[i].pAbility, sizeof(ik_ability_t *));
    }

    return ik_init_arch(&context_setting, pMemAddr, mem_size, AMBA_IK_ARCH_SOFT_INIT);
}

/**
* Specify the def binary address for IK.
* @param [in]  pBinDataAddr, Memory buffer address for def binary
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitDefBinary(void *pBinDataAddr)
{
    return ik_init_default_binary(pBinDataAddr);
}

/**
* Initialize ability of the specified context
* @param [in]  pMode, Set ContextId
* @param [in]  pAbility, Set ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_InitContext(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility)
{
    const ik_ability_t *p_ability;

    (void)amba_ik_system_memcpy(&p_ability, &pAbility, sizeof(void *));

    return ik_init_context(pMode->ContextId, p_ability);
}

/**
* To trigger image kernel to compose user assigned image quality parameters of context ID to configuration
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExecuteContainer, Returned composed config stored in container
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_ExecuteConfig(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    ik_execute_container_t *p_execute_container;

    (void)amba_ik_system_memcpy(&p_execute_container, &pExecuteContainer, sizeof(void *));

    return ik_execute(pMode->ContextId, p_execute_container);
}

/**
* To trigger image kernel to compose user assigned image quality parameters of context ID to configuration
* @param [in]  pMode, Set ContextId
* @param [in,out]  pExecuteContainer, Returned composed config stored in container
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_ExecuteConfigNoWarp(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    ik_execute_container_t *p_execute_container;

    (void)amba_ik_system_memcpy(&p_execute_container, &pExecuteContainer, sizeof(void *));

    return ik_execute_eis_1st(pMode->ContextId, p_execute_container);
}

/**
* To trigger image kernel to compose user assigned image quality parameters of context ID to configuration
* @param [in]  pMode, Set ContextId; ExplicitIkId, set ik id
* @param [in,out]  pExecuteContainer, Returned composed config stored in container
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_ExecuteConfigUpdateWarp(const AMBA_IK_MODE_CFG_s *pMode, UINT32 ExplicitIkId, const AMBA_IK_EXECUTE_CONTAINER_s *pExecuteContainer)
{
    ik_execute_container_t *p_execute_container;

    (void)amba_ik_system_memcpy(&p_execute_container, &pExecuteContainer, sizeof(void *));

    return ik_execute_eis_2nd(pMode->ContextId, ExplicitIkId, p_execute_container);
}
#endif

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
        if(p_ik_buffer_info->init_flag == 1U) {
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
        if(p_ik_buffer_info->init_flag == 1U) {
            ik_hook_debug_check();
        } else {
            Rval = IK_ERR_0001;
        }
    }
    return Rval;
}

/**
* Get the ability of context ID
* @param [in]  pMode, Set ContextId
* @param [in,out]  pAbility, Get ability of this context.
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_GetContextAbility(const AMBA_IK_MODE_CFG_s *pMode, const AMBA_IK_ABILITY_s *pAbility)
{
    ik_ability_t *p_ability;

    (void)amba_ik_system_memcpy(&p_ability, &pAbility, sizeof(void *));

    return ik_get_context_ability(pMode->ContextId, p_ability);
}

/**
* Calculates the memory usage based for debug config dump
* @param [in,out]  pSize, Returned required memory size for debug config dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryDebugConfigSize(const AMBA_IK_MODE_CFG_s *pMode, const SIZE_t *pSize)
{
    size_t *p_size;
    (void)amba_ik_system_memcpy(&p_size, &pSize, sizeof(void *));
    return ik_query_debug_config_size(pMode->ContextId, p_size);
}

/**
* Dump debug config of specified config
* @param [in]  pMode, Set ContextId
* @param [in]  PreviousCount, Previous count of composed config
* @param [in,out]  pDumpConfigBuffer, Memory buffer pointer prepared by USER to store debug config dump
* @param [in]  Size, the size for debug config dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_DumpDebugConfig(const AMBA_IK_MODE_CFG_s *pMode, UINT32 PreviousCount, const void *pDumpConfigBuffer, SIZE_t Size)
{
    UINT32 Rval = IK_OK;

    Rval = ik_dump_debug_config(pMode->ContextId, PreviousCount, pDumpConfigBuffer, Size);

    return Rval;
}

UINT32 AmbaIK_QueryHisoDebugConfigSize(const SIZE_t *pSize)
{
    size_t *p_size;
    (void)amba_ik_system_memcpy(&p_size, &pSize, sizeof(void *));
    return ik_query_hiso_debug_config_size(p_size);
}

UINT32 AmbaIK_DumpHisoDebugConfig(const AMBA_IK_MODE_CFG_s *pMode, UINT32 PreviousCount, const void *pDumpConfigBuffer, SIZE_t Size)
{
    UINT32 Rval = IK_OK;

    Rval = ik_dump_hiso_debug_config(pMode->ContextId, PreviousCount, pDumpConfigBuffer, Size);

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
    const ik_query_idsp_clock_info_t *p_query_idsp_clock_info;
    ik_query_idsp_clock_rst_t *p_query_idsp_clock_rst;
    if ((pQueryIdspClockInfo != NULL) && (pQueryIdspClockRst != NULL)) {
        (void) amba_ik_system_memcpy(&p_query_idsp_clock_info, &pQueryIdspClockInfo, sizeof(ik_query_idsp_clock_info_t*));
        (void) amba_ik_system_memcpy(&p_query_idsp_clock_rst, &pQueryIdspClockRst, sizeof(ik_query_idsp_clock_rst_t*));
        Rval = ik_query_idsp_clock(p_query_idsp_clock_info, p_query_idsp_clock_rst);
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  Query the calculate geometry settings required buffer sizes
* @param [in,out]  pBufSizeInfo, returned required buffer size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryCalcGeoBufSize(const AMBA_IK_QUERY_CALC_GEO_BUF_SIZE_s *pBufSizeInfo)
{
    UINT32 Rval;
    ik_query_calc_geo_buf_size_t *p_buf_size_info;
    if (pBufSizeInfo != NULL) {
        (void) amba_ik_system_memcpy(&p_buf_size_info, &pBufSizeInfo, sizeof(ik_query_calc_geo_buf_size_t*));
        Rval = ik_query_calc_geo_buf_size(p_buf_size_info);
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  Calculate the geometry settings
* @param [in]  pAbility, Set ability of this context.
* @param [in,out]  pInfo, Calc input information and working buffer and working buffer size
* @param [in,out]  pGeoRst, output buffer and output buffer size
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_CalcGeoSettings(const AMBA_IK_ABILITY_s *pAbility, const AMBA_IK_CALC_GEO_SETTINGS_s *pInfo, const AMBA_IK_CALC_GEO_RESULT_s *pGeoRst)
{
    UINT32 Rval;
    const ik_calc_geo_settings_t *p_info;
    const ik_calc_geo_result_t *p_result;
    const ik_ability_t *p_ability;
    if ((pInfo != NULL) && (pGeoRst != NULL) && (pAbility != NULL)) {
        (void) amba_ik_system_memcpy(&p_info, &pInfo, sizeof(ik_calc_geo_settings_t *));
        (void) amba_ik_system_memcpy(&p_result, &pGeoRst, sizeof(ik_calc_geo_result_t *));
        (void) amba_ik_system_memcpy(&p_ability, &pAbility, sizeof(ik_ability_t *));
        Rval = ik_calc_geo_settings(p_ability, p_info, p_result);
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  Query the warp resources
* @param [in,out]  pQueryParams, Query warp working resources
* @param [in,out]  pQueryResult, Query warp result resources
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_CalcWarpResources(const AMBA_IK_QUERY_WARP_s *pQueryParams, AMBA_IK_QUERY_WARP_RESULT_s *pQueryResult)
{
    UINT32 Rval = IK_OK;
    const ik_query_warp_t *p_usr_cfg;
    ik_query_warp_result_t QueryWarpResult;

    (void)amba_ik_system_memcpy(&p_usr_cfg, &pQueryParams, sizeof(void *));
    (void)amba_ik_system_memset(&QueryWarpResult, 0x0, sizeof(ik_query_warp_result_t));

    (void)amba_ik_system_memcpy(pQueryResult, pQueryResult, sizeof(UINT8));//misra fix.
    /*Rval = ik_calc_warp_resources(p_usr_cfg, &QueryWarpResult);

    pQueryResult->WaitLine = QueryWarpResult.wait_line;
    pQueryResult->DmaSize = QueryWarpResult.dma_size;
    pQueryResult->VerticalWarpVectorFlipNum = QueryWarpResult.warp_vector_v_flip_num;
    pQueryResult->DmyBroken = QueryWarpResult.dmy_broken;
    (void)amba_ik_system_memcpy(&pQueryResult->SuggestedDmyRange, &QueryWarpResult.suggested_dmy_range, sizeof(AMBA_IK_DUMMY_MARGIN_RANGE_s));*/

    return Rval;
}

/**
*  Warp table pre-process
* @param [in]  pWarpPreProcIn, Calc input information and working buffer
* @param [in,out]  pWarpPreProcOut, output buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_PreProcWarpTbl(const AMBA_IK_IN_WARP_PRE_PROC_s *pWarpPreProcIn, AMBA_IK_OUT_WARP_PRE_PROC_s *pWarpPreProcOut)
{
    UINT32 Rval;
    ik_in_warp_remap_t warp_remap_in;
    ik_out_warp_remap_t *p_warp_remap_out;
    ik_warp_info_t input_info;
    warp_remap_in.p_input_info = &input_info;
    if ((pWarpPreProcIn != NULL) && (pWarpPreProcOut != NULL)) {
        if(pWarpPreProcIn->pInputInfo != NULL) {
            (void)amba_ik_system_memcpy(pWarpPreProcOut, pWarpPreProcOut, sizeof(UINT8));//misra fix.
            //keep FOV optimize under lower case layer.
            warp_remap_in.p_working_buffer = pWarpPreProcIn->pWorkingBuffer;
            warp_remap_in.working_buffer_size = pWarpPreProcIn->WorkingBufferSize;
            (void) amba_ik_system_memcpy(warp_remap_in.p_input_info, pWarpPreProcIn->pInputInfo, sizeof(ik_warp_info_t));
            warp_remap_in.p_input_info->vin_sensor_geo.width = (warp_remap_in.p_input_info->hor_grid_num-1U)<<warp_remap_in.p_input_info->tile_width_exp;
            warp_remap_in.p_input_info->vin_sensor_geo.height = (warp_remap_in.p_input_info->ver_grid_num-1U)<<warp_remap_in.p_input_info->tile_height_exp;
            (void) amba_ik_system_memcpy(&warp_remap_in.vin_sensor_geo, &warp_remap_in.p_input_info->vin_sensor_geo, sizeof(ik_vin_sensor_geometry_t));

            (void) amba_ik_system_memcpy(&p_warp_remap_out, &pWarpPreProcOut, sizeof(ik_out_warp_remap_t *));
            Rval = ik_warp_remap(&warp_remap_in, p_warp_remap_out);
            p_warp_remap_out->p_result_info->vin_sensor_geo.width = pWarpPreProcIn->pInputInfo->VinSensorGeo.Width;
            p_warp_remap_out->p_result_info->vin_sensor_geo.height = pWarpPreProcIn->pInputInfo->VinSensorGeo.Height;
            p_warp_remap_out->p_result_info->enb_2_stage_compensation = 0;//from SVC user request.
        } else {
            Rval = IK_ERR_0005;
        }
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  VR Warp table pre-process
* @param [in]  pWarpPreProcIn, Calc input information and working buffer
* @param [in,out]  pWarpPreProcOut, pWarpPreProcOut2nd, output buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_PreProcWarpTblVR(const AMBA_IK_IN_WARP_PRE_PROC_s *pWarpPreProcIn, AMBA_IK_OUT_WARP_PRE_PROC_s *pWarpPreProcOut, AMBA_IK_OUT_WARP_PRE_PROC_s *pWarpPreProcOut2nd)
{
    UINT32 Rval;
    ik_in_warp_remap_t warp_remap_in;
    ik_out_warp_remap_t *p_warp_remap_out;
    ik_out_warp_remap_t *p_warp_remap_out_2nd;
    ik_warp_info_t input_info;
    warp_remap_in.p_input_info = &input_info;
    if ((pWarpPreProcIn != NULL) && (pWarpPreProcOut != NULL) && (pWarpPreProcOut2nd != NULL)) {
        if(pWarpPreProcIn->pInputInfo != NULL) {
            (void)amba_ik_system_memcpy(pWarpPreProcOut, pWarpPreProcOut, sizeof(UINT8));//misra fix.
            (void)amba_ik_system_memcpy(pWarpPreProcOut2nd, pWarpPreProcOut2nd, sizeof(UINT8));//misra fix.
            //HVH VR actually doesn't have FOV optimize function.
            warp_remap_in.p_working_buffer = pWarpPreProcIn->pWorkingBuffer;
            warp_remap_in.working_buffer_size = pWarpPreProcIn->WorkingBufferSize;
            (void) amba_ik_system_memcpy(warp_remap_in.p_input_info, pWarpPreProcIn->pInputInfo, sizeof(ik_warp_info_t));
            warp_remap_in.p_input_info->vin_sensor_geo.width = (warp_remap_in.p_input_info->hor_grid_num-1U)<<warp_remap_in.p_input_info->tile_width_exp;
            warp_remap_in.p_input_info->vin_sensor_geo.height = (warp_remap_in.p_input_info->ver_grid_num-1U)<<warp_remap_in.p_input_info->tile_height_exp;
            (void) amba_ik_system_memcpy(&warp_remap_in.vin_sensor_geo, &warp_remap_in.p_input_info->vin_sensor_geo, sizeof(ik_vin_sensor_geometry_t));

            (void) amba_ik_system_memcpy(&p_warp_remap_out, &pWarpPreProcOut, sizeof(ik_out_warp_remap_t *));
            (void) amba_ik_system_memcpy(&p_warp_remap_out_2nd, &pWarpPreProcOut2nd, sizeof(ik_out_warp_remap_t *));
            Rval = ik_warp_remap_hvh_vr(&warp_remap_in, p_warp_remap_out, p_warp_remap_out_2nd);
            p_warp_remap_out->p_result_info->vin_sensor_geo.width = pWarpPreProcIn->pInputInfo->VinSensorGeo.Width;
            p_warp_remap_out->p_result_info->vin_sensor_geo.height = pWarpPreProcIn->pInputInfo->VinSensorGeo.Height;
            p_warp_remap_out->p_result_info->enb_2_stage_compensation = 0;//from SVC user request.
            p_warp_remap_out_2nd->p_result_info->vin_sensor_geo.width = pWarpPreProcIn->pInputInfo->VinSensorGeo.Width;
            p_warp_remap_out_2nd->p_result_info->vin_sensor_geo.height = pWarpPreProcIn->pInputInfo->VinSensorGeo.Height;
            p_warp_remap_out_2nd->p_result_info->enb_2_stage_compensation = 0;//from SVC user request.
        } else {
            Rval = IK_ERR_0005;
        }
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
*  CAWarp table pre-process
* @param [in]  pCaPreProcIn, Calc input information and working buffer
* @param [in,out]  pCaPreProcOut, output buffer
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_PreProcCAWarpTbl(const AMBA_IK_IN_CAWARP_PRE_PROC_s *pCaPreProcIn, AMBA_IK_OUT_CAWARP_PRE_PROC_s *pCaPreProcOut)
{
    UINT32 Rval;
    const ik_in_cawarp_remap_t *p_cawarp_remap_in;
    ik_out_cawarp_remap_t *p_cawarp_remap_out;
    if ((pCaPreProcIn != NULL) && (pCaPreProcOut != NULL)) {
        (void)amba_ik_system_memcpy(pCaPreProcOut, pCaPreProcOut, sizeof(UINT8));//misra fix.
        (void) amba_ik_system_memcpy(&p_cawarp_remap_in, &pCaPreProcIn, sizeof(ik_in_cawarp_remap_t *));
        (void) amba_ik_system_memcpy(&p_cawarp_remap_out, &pCaPreProcOut, sizeof(ik_out_cawarp_remap_t *));
        Rval = ik_cawarp_remap(p_cawarp_remap_in, p_cawarp_remap_out);
        p_cawarp_remap_out->p_result_info->enb_2_stage_compensation= 0;//from SVC user request.
    } else {
        Rval = IK_ERR_0005;
    }
    return Rval;
}

/**
* Query specified frame information with IkId
* @param [in]  pMode, Set ContextId
* @param [in]  IkId, IK config ring ID
* @param [in,out]  pFrameInfo, FrameInfo buffer pointer prepared by USER to store FrameInfo dump
* @return OK - success, NG - fail
*/
UINT32 AmbaIK_QueryFrameInfo(const AMBA_IK_MODE_CFG_s *pMode, const UINT32 IkId, AMBA_IK_QUERY_FRAME_INFO_s *pFrameInfo)
{
    UINT32 Rval = IK_OK;
    ik_query_frame_info_t *p_ik_frame_info;
    if (pFrameInfo != NULL) {
        (void)amba_ik_system_memcpy(pFrameInfo, pFrameInfo, sizeof(UINT8));//misra fix.
        (void) amba_ik_system_memcpy(&p_ik_frame_info, &pFrameInfo, sizeof(ik_query_frame_info_t *));
        Rval = ik_query_frame_info(pMode->ContextId, IkId, p_ik_frame_info);
    } else {
        Rval = IK_ERR_0005;
    }

    return Rval;
}

