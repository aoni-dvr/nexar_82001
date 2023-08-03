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

#ifndef IMG_UTILITY_H
#define IMG_UTILITY_H

//#include "ik_data_type.h"
//#include "AmbaDSP_ImgOSAL.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgArchSystemAPI.h"

typedef struct {
    uintptr iso_config_addr;
    uint32 iso_config_tag;
    // update flags
} executed_iso_config_result_t;

typedef struct {
    uintptr hdr_ce_config_addr;
    uint32 hdr_ce_config_tag;
    // update flags
} executed_hdr_ce_config_result_t;

typedef struct {
    uint32 context_number;
    struct {
        uint32 config_number;
        ik_ability_t *ability;
        ik_user_parameters_t *default_setting;
    } config_setting[MAX_CONTEXT_NUM];
} ik_context_setting_t;

uint32 ik_query_arch_memory_size(const ik_context_setting_t *p_setting_of_each_context, size_t *p_total_ctx_size);
uint32 ik_init_arch(const ik_context_setting_t *p_setting_of_each_context, void* p_mem_addr, size_t mem_size, uint32 ik_init_mode);
uint32 ik_get_filter_default_values(ik_user_parameters_t *p_default_values);
uint32 ik_init_context(uint32 context_id, const ik_ability_t *p_ability, const ik_user_parameters_t *p_default_values);
uint32 ik_get_context_ability(uint32 context_id, ik_ability_t *p_ability);
uint32 ik_execute_config(uint32 context_id, ik_execute_container_t *p_execute_container);
uint32 ik_execute_config_eis_1st(uint32 context_id, ik_execute_container_t *p_execute_container);
uint32 ik_execute_config_eis_2nd(uint32 context_id, uint32 explicit_eis_ik_id, ik_execute_container_t *p_execute_container);
//uint32 ik_lock_config(uint32 config_tag);
uint32 ik_query_debug_config_size(size_t *p_size);
uint32 ik_dump_debug_config(uint32 context_id, uint32 previous_count,const void *p_user_buffer, size_t size);
uint32 ik_query_idsp_clock(const ik_query_idsp_clock_info_t *p_query_idsp_clock_info, ik_query_idsp_clock_rst_t *p_query_idsp_clock_rst);

uint32 ik_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info);
uint32 ik_calc_geo_settings(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result);
uint32 ik_calc_warp_resources(const ik_query_warp_t *p_info, ik_query_warp_result_t *p_result);

#endif
