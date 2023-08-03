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

#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_IkcDebugUtility.h"

uint32 ik_set_debug_level(uint32 debug_level)
{
    (void)ikc_set_debug_level(debug_level);
    return IK_OK;
}

uint32 ik_get_debug_level(uint32 *debug_level)
{
    (void)ikc_get_debug_level(debug_level);
    return IK_OK;
}

uint32 ik_set_warp_debug_level(uint32 warp_debug_level)
{
    (void)ikc_set_warp_debug_level(warp_debug_level);
    return IK_OK;
}

uint32 ik_get_warp_debug_level(uint32 *warp_debug_level)
{
    (void)ikc_get_warp_debug_level(warp_debug_level);
    return IK_OK;
}

uint32 ik_set_diag_case_id(uint32 context_id, uint8 diag_case_id)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        img_ctx_set_diag_case_id(context_id, diag_case_id);
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_set_static_bpc_highlight(uint32 context_id, uint32 high_light_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        p_ctx->filters.input_param.sbp_highlight_mode = high_light_mode;
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_get_static_bpc_highlight(uint32 context_id, uint32 *p_high_light_mode)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((p_high_light_mode != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        *p_high_light_mode = p_ctx->filters.input_param.sbp_highlight_mode;
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        if(p_high_light_mode == NULL) {
            amba_ik_system_print_str_5("[IK] NULL pointer in %s", __func__, DC_S, DC_S, DC_S, DC_S);
        } else if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 ik_query_debug_config_size(size_t *p_size)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    uint32 i;
    size_t max_size = 0;
    size_t tmp_size;
    rval = img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    for(i = 0; i < p_ik_buffer_info->number_of_context; i++) {
        rval = img_ctx_get_context(i, &p_ctx);
        rval = img_cfg_query_debug_config_size(&p_ctx->organization.attribute.ability, &tmp_size);
        if (tmp_size > max_size) {
            max_size = tmp_size;
        }
    }
    *p_size = max_size;
    return rval;
}

static INLINE uint32 img_cfg_amalgam_data_ctx_update(const void *p_user_buffer, const amba_ik_context_entity_t *p_ctx)
{
    uint32 rval = 0UL;
    ik_amalgam_liso_t *p_early_test_amalgam_liso;
    uint64 addr_start =  0, addr_ctx_info = 0, offset = 0;
    const ik_ctx_info_t *p_ctx_info;
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso, &p_user_buffer, sizeof(ik_amalgam_liso_t*));

    p_early_test_amalgam_liso->ctx_info.flip_mode = p_ctx->filters.input_param.flip_mode;
    p_early_test_amalgam_liso->ctx_info.ctx_start = 0x09875987UL;

    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_statistics_setup, &p_ctx->filters.input_param.ctx_buf.last_aaa_statistics_setup[0], sizeof(aaa_statistics_setup_t));
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_statistics_setup1_af_ex, &p_ctx->filters.input_param.ctx_buf.last_aaa_statistics_setup1_af_ex[0], sizeof(aaa_statistics_setup1_t));
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_statistics_setup2_af_ex, &p_ctx->filters.input_param.ctx_buf.last_aaa_statistics_setup2_af_ex[0], sizeof(aaa_statistics_setup2_t));
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_statistics_setup1_pg_af_ex, &p_ctx->filters.input_param.ctx_buf.last_aaa_statistics_setup1_pg_af_ex[0], sizeof(aaa_statistics_setup1_t));
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_statistics_setup2_pg_af_ex, &p_ctx->filters.input_param.ctx_buf.last_aaa_statistics_setup2_pg_af_ex[0], sizeof(aaa_statistics_setup2_t));
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_histogram, &p_ctx->filters.input_param.ctx_buf.last_aaa_histogram[0], sizeof(aaa_histogram_t));
    (void)amba_ik_system_memcpy(&p_early_test_amalgam_liso->ctx_info.last_aaa_histogram_pg, &p_ctx->filters.input_param.ctx_buf.last_aaa_histogram_pg[0], sizeof(aaa_histogram_t));
    p_early_test_amalgam_liso->ctx_info.first_compression_offset = p_ctx->filters.input_param.ctx_buf.first_compression_offset;
    p_early_test_amalgam_liso->ctx_info.ik_version_major = IK_VERSION_MAJOR;
    p_early_test_amalgam_liso->ctx_info.ik_version_minor = IK_VERSION_MINOR;

    (void)amba_ik_system_memcpy(&addr_start, &p_early_test_amalgam_liso, sizeof(void*));
    p_ctx_info = &p_early_test_amalgam_liso->ctx_info;
    (void)amba_ik_system_memcpy(&addr_ctx_info, &p_ctx_info, sizeof(void*));
    offset = addr_ctx_info - addr_start;
    p_early_test_amalgam_liso->ctx_info_offset = (uint32)offset;

    return rval;
}

uint32 ik_dump_debug_config(uint32 context_id, uint32 previous_count, const void *p_user_buffer, size_t size)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    const amba_ik_hiso_config_and_state_t *hiso_config_and_state;
    const amba_ik_iso_config_and_state_t *iso_config_and_state;
    int32 cfg_id;
    size_t amalgam_size;
    intptr misra_intptr_addr;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        (void)img_cfg_query_debug_config_size(&p_ctx->organization.attribute.ability, &amalgam_size);
        if ((p_user_buffer == NULL)||(size < (uint32)amalgam_size)) {
            if(p_user_buffer == NULL) {
                amba_ik_system_print_uint32_5("[IK][ERROR] p_user_buffer is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0005;
            }
            if(size < (uint32)amalgam_size) {
                amba_ik_system_print_uint32_5("[IK][ERROR] size < sizeof(amalgam_liso_t)", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0007;
            }
        } else {
            if(previous_count > (p_ctx->organization.attribute.config_number-1U)) {
                amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > (config_number-1) = %d", (p_ctx->organization.attribute.config_number-1U), DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD
            }
            if (rval == IK_OK) {
                cfg_id = (int32)p_ctx->organization.active_config_state.active_iso_idx - (int32)previous_count;
                if ((p_ctx->organization.active_config_state.iso_running_number < p_ctx->organization.attribute.config_number) && (cfg_id < 0)) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > iso_running_number = %d", p_ctx->organization.active_config_state.iso_running_number, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0008;
                } else { //check iso_running_number > config_number
                    if (cfg_id < 0) {
                        cfg_id = cfg_id + (int32)p_ctx->organization.attribute.config_number;
                    }
                    //iso_config_and_state = (amba_ik_iso_config_and_state_t *) p_ctx->configs.iso_configs_and_states[cfg_id];
                    misra_intptr_addr = p_ctx->configs.iso_configs_and_states[cfg_id];
                    if(p_ctx->organization.attribute.ability.pipe == AMBA_IK_PIPE_VIDEO) {
                        (void) amba_ik_system_memcpy(&iso_config_and_state, &misra_intptr_addr, sizeof(amba_ik_iso_config_and_state_t *));
                        if (iso_config_and_state != NULL) {
                            rval |= img_cfg_amalgam_data_update(p_user_buffer, &iso_config_and_state->iso_config);
                            rval |= img_cfg_amalgam_data_ctx_update(p_user_buffer, p_ctx);
                        } else {
                            amba_ik_system_print_uint32_5("[IK][ERROR] ik_dump_debug_config() iso_config is NULL \n", DC_U, DC_U, DC_U, DC_U, DC_U);
                        }
                    } else {
                        if(p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_HISO) {
                            (void) amba_ik_system_memcpy(&hiso_config_and_state, &misra_intptr_addr, sizeof(amba_ik_hiso_config_and_state_t *));
                            if (hiso_config_and_state != NULL) {
                                rval |= img_cfg_amalgam_data_hiso_update(p_user_buffer, &hiso_config_and_state->iso_config);
                            }
                        } else {
                            (void) amba_ik_system_memcpy(&iso_config_and_state, &misra_intptr_addr, sizeof(amba_ik_iso_config_and_state_t *));
                            if (iso_config_and_state != NULL) {
                                rval |= img_cfg_amalgam_data_update(p_user_buffer, &iso_config_and_state->iso_config);
                                rval |= img_cfg_amalgam_data_ctx_update(p_user_buffer, p_ctx);
                            } else {
                                amba_ik_system_print_uint32_5("[IK][ERROR] ik_dump_debug_config() iso_config is NULL \n", DC_U, DC_U, DC_U, DC_U, DC_U);
                            }
                        }
                    }
                }
            }
        }
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] can't get ctx_id %d",context_id, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

uint32 ik_query_idsp_clock(const ik_query_idsp_clock_info_t *p_query_idsp_clock_info, ik_query_idsp_clock_rst_t *p_query_idsp_clock_rst)
{
    uint32 rval = IK_OK;
    uint32 vin_pixels, main_pixels, max_pixels;

    if ((p_query_idsp_clock_info->vin_sensor_geo.width == 0UL) || (p_query_idsp_clock_info->vin_sensor_geo.height == 0UL)) {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_query_idsp_clock() incorrect vin_sensor_geo width %d height %d\n", \
                                      p_query_idsp_clock_info->vin_sensor_geo.width, p_query_idsp_clock_info->vin_sensor_geo.height, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    if ((p_query_idsp_clock_info->main_window.width == 0UL) || (p_query_idsp_clock_info->main_window.height == 0UL)) {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_query_idsp_clock() incorrect vin_sensor_geo width %d height %d\n", \
                                      p_query_idsp_clock_info->main_window.width, p_query_idsp_clock_info->main_window.height, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    if (p_query_idsp_clock_info->fps == 0UL) {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_query_idsp_clock() incorrect fps %d \n", \
                                      p_query_idsp_clock_info->fps, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }

    vin_pixels = p_query_idsp_clock_info->vin_sensor_geo.width*p_query_idsp_clock_info->vin_sensor_geo.height;
    main_pixels = p_query_idsp_clock_info->main_window.width*p_query_idsp_clock_info->main_window.height;
    max_pixels = (vin_pixels > main_pixels)?vin_pixels:main_pixels;
    p_query_idsp_clock_rst->theoretical_clock = (uint32)(((uint64)max_pixels * p_query_idsp_clock_info->fps * 1130UL) >> 10);

    return rval;
}

void ik_hook_debug_check(void )
{
    ik_ctx_hook_debug_check();
}

