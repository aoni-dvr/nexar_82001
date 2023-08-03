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


#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgDebugUtility.h"
//#include "AmbaDSP_ImgConfigDebugPrint.h"
#include "AmbaDSP_ImgConfigComponentIF.h"

static uint32 s_execute_config(uint32 context_id, ik_execute_container_t *p_execute_container, uint32 eis_mode, uint32 explicit_eis_ik_id);
static uint32 s_execute_config_still(uint32 context_id, ik_execute_container_t *p_execute_container);

uint32 ik_execute_config(uint32 context_id, ik_execute_container_t *p_execute_container)
{
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 rval = IK_OK;
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        if(p_ctx->organization.attribute.ability.pipe == AMBA_IK_PIPE_VIDEO) {
            rval |= s_execute_config(context_id, p_execute_container, REGULAR_EXECUTE, 0UL);
        } else {
            rval |= s_execute_config_still(context_id, p_execute_container);
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] Error, can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

uint32 ik_execute_config_eis_1st(uint32 context_id, ik_execute_container_t *p_execute_container)
{
    return s_execute_config(context_id, p_execute_container, STAGE_1_FOR_EIS_EXECUTE, 0UL);
}

uint32 ik_execute_config_eis_2nd(uint32 context_id, uint32 explicit_eis_ik_id, ik_execute_container_t *p_execute_container)
{
    return s_execute_config(context_id, p_execute_container, STAGE_2_FOR_EIS_EXECUTE, explicit_eis_ik_id);
}
static uint32 s_execute_config_error_check(uint32 context_id, \
        const ik_execute_container_t *p_execute_container, \
        uint32 eis_mode, \
        uint32 explicit_eis_ik_id,\
        const amba_ik_context_entity_t *p_ctx,
        uint32 next_eis_running_number,
        uint32 eis_running_number,
        const uint32 *p_explicit_runnung_number,
        uint32 is_reasonable_delay,
        uint32 rval)
{
    uint32 ret_value = 0U;
    ret_value = rval;
    if(p_execute_container == NULL) {
        amba_ik_system_print_str_5("[IK] Error, NULL pointer in %s", __func__, DC_S, DC_S, DC_S, DC_S);
        ret_value |= IK_ERR_0005;
    } else if(p_ctx->organization.initial_flag == 0U) {
        amba_ik_system_print_uint32_5("[IK] Error, context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
        ret_value |= IK_ERR_0002;
    } else if((eis_mode == STAGE_2_FOR_EIS_EXECUTE) && ((ret_value & IK_ERR_0008) != 0UL)) {
        if (p_ctx->filters.input_param.eis_mode != STAGE_1_FOR_EIS_EXECUTE) {
            amba_ik_system_print_uint32_5("[IK] AmbaIK_ExecuteConfig_EIS_2nd() pass ik_id [%d], missing call AmbaIK_ExecuteConfig_EIS_1st",
                                          explicit_eis_ik_id, DC_U, DC_U, DC_U, DC_U);
        } else if ((next_eis_running_number != eis_running_number) && (*p_explicit_runnung_number != 0xFFFFFFFFUL)) {
            amba_ik_system_print_uint32_5("[IK] Error, AmbaIK_ExecuteConfig_EIS_2nd() pass discontinuous ik_id [%d], previous AmbaIK_ExecuteConfig_EIS_2nd runnung number was [%d]",
                                          explicit_eis_ik_id, *p_explicit_runnung_number, DC_U, DC_U, DC_U);
        } else if (is_reasonable_delay==0UL) {
            amba_ik_system_print_uint32_5("[IK] Error, AmbaIK_ExecuteConfig_EIS_2nd() pass ik_id [%d] too late, current runnung number is [%d]",
                                          explicit_eis_ik_id, ctx_parse_running_number(p_ctx->organization.active_config_state.active_iso_idx), DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_uint32_5("[IK] Error, AmbaIK_ExecuteConfig_EIS_2nd() pass invalid ik_id [%d], current ctx_id [%d], pipe [%d], ability [%d]",
                                          explicit_eis_ik_id, context_id, p_ctx->organization.attribute.ability.pipe, p_ctx->organization.attribute.ability.video_pipe, DC_U);
        }
    } else {//((eis_mode == REGULAR_EXECUTE) && ((rval & IK_ERR_0008) != 0UL))
        amba_ik_system_print_uint32_5("[IK] Error, Invalid AmbaIK_ExecuteConfig() after AmbaIK_ExecuteConfig_EIS_1st() activated", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    return ret_value;
}

static uint32 s_execute_config_eis_check(const amba_ik_context_entity_t *p_ctx,
        uint32 explicit_eis_ik_id,\
        uint32 next_eis_running_number,\
        uint32 eis_running_number,\
        uint32 *p_explicit_runnung_number,\
        uint32 eis_ctx_id,\
        uint32 context_id, \
        uint32 eis_pipe, \
        uint32 eis_ability, \
        uint32 is_reasonable_delay,\
        uint32 *p_explicit_iso_runnung_number,\
        uint32 diff_id,\
        uint32 *p_iso_config_id)
{
    uint32 rval = IK_OK;
    if (p_ctx->filters.input_param.eis_mode != STAGE_1_FOR_EIS_EXECUTE) {//missing eis stage1.
        amba_ik_system_print_uint32_5("[IK] AmbaIK_ExecuteConfig_EIS_2nd() pass ik_id [%d], without calling AmbaIK_ExecuteConfig_EIS_1st", explicit_eis_ik_id, DC_U, DC_U, DC_U, DC_U);
        //rval |= IK_ERR_0008; Let APP decides the timing
    }
    if ((next_eis_running_number != eis_running_number) && (*p_explicit_runnung_number != 0xFFFFFFFFUL)) {// check continuity.
        amba_ik_system_print_uint32_5("#### error is here:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else if ((eis_ctx_id != context_id) || (eis_pipe != (uint32)p_ctx->organization.attribute.ability.pipe) || (eis_ability != p_ctx->organization.attribute.ability.video_pipe)) {
        // check ctx_id, pipe, ability consistency.
        amba_ik_system_print_uint32_5("#### error is here:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else if (is_reasonable_delay == 0UL) {// eis delay > MAX_EIS_DELAY_FRAMES.
        amba_ik_system_print_uint32_5("#### error is here:%d", __LINE__, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else {// set lastest stage2 ik_id running number.
        *p_explicit_runnung_number = eis_running_number;
        *p_explicit_iso_runnung_number = (p_ctx->organization.active_config_state.active_iso_idx + p_ctx->organization.attribute.config_number - diff_id)%p_ctx->organization.attribute.config_number;
        *p_iso_config_id = *p_explicit_iso_runnung_number;
    }
    return rval;
}

static uint32 s_execute_config(uint32 context_id, ik_execute_container_t *p_execute_container, uint32 eis_mode, uint32 explicit_eis_ik_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_config_container_controller_t ct_ctrler;
    uint32 iso_config_id = 0, iso_cfg_tag = 0;
    amba_ik_iso_config_and_state_t *iso_config_and_state;
    intptr temp_address;
    uint32  ik_debug_level;
    const void *p_iso_cfg_misra;
    uint32 eis_running_number=0UL, next_eis_running_number=0UL, *p_explicit_runnung_number=NULL, *p_explicit_iso_runnung_number=NULL, eis_legal_delay_idx, eis_delay;
    uint32 eis_ctx_id, eis_pipe, eis_ability, is_reasonable_delay=0UL, diff_id;
    ct_ctrler.pack_container = NULL;
    ct_ctrler.handler.container = NULL;
    (void)ik_get_debug_level(&ik_debug_level);
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        if ((p_ctx->organization.initial_flag != 0U) && (eis_mode == REGULAR_EXECUTE)) { // once switch to eis_execute, no coming back way to regular.
            if (p_ctx->filters.input_param.eis_mode != REGULAR_EXECUTE) {
                rval |= IK_ERR_0008;
            }
        }

        if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U) && (eis_mode == STAGE_2_FOR_EIS_EXECUTE)) {
            eis_ctx_id = ctx_parse_context_id(explicit_eis_ik_id);
            eis_pipe = ctx_parse_pipe(explicit_eis_ik_id);
            eis_ability = ctx_parse_ability(explicit_eis_ik_id);
            eis_running_number = ctx_parse_running_number(explicit_eis_ik_id);
            rval |= img_ctx_get_explicit_running_num(context_id, &p_explicit_runnung_number);
            //next_eis_running_number = (*p_explicit_runnung_number + 1UL) & 0x0000FFFFUL;
            if(*p_explicit_runnung_number == p_ctx->organization.active_config_state.max_running_number) {
                next_eis_running_number = 1UL;
            } else {
                next_eis_running_number = *p_explicit_runnung_number + 1UL;
            }
            rval |= img_ctx_get_explicit_iso_running_num(context_id, &p_explicit_iso_runnung_number);
            eis_legal_delay_idx = ctx_parse_running_number((uint32)p_ctx->organization.active_config_state.iso_running_number);
            is_reasonable_delay = 0UL;
            diff_id = 0UL;
            for (eis_delay=0U; eis_delay<=MAX_EIS_DELAY_FRAMES; eis_delay++) {
                if (eis_legal_delay_idx==eis_running_number) {
                    is_reasonable_delay = 1UL;
                    break;
                }
                diff_id++;
                //eis_legal_delay_idx = (eis_legal_delay_idx + 65536UL - 1UL) & 0x0000FFFFUL;
                if(eis_legal_delay_idx == 1UL) {
                    eis_legal_delay_idx = p_ctx->organization.active_config_state.max_running_number;
                } else {
                    eis_legal_delay_idx = eis_legal_delay_idx - 1UL;
                }
            }
            rval |= s_execute_config_eis_check(p_ctx, explicit_eis_ik_id, next_eis_running_number, eis_running_number, p_explicit_runnung_number, eis_ctx_id, context_id, eis_pipe, eis_ability, is_reasonable_delay, p_explicit_iso_runnung_number, diff_id, &iso_config_id);
        }

        if ((p_execute_container != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {

            p_ctx->filters.input_param.eis_mode = eis_mode;// 0 : normal execute, 1 : before EIS warp execute, 2 : with EIS warp execute.
            rval |= p_ctx->organization.executer_method.execute_iso_config(context_id);

            if (eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
                if(p_ctx->organization.active_config_state.iso_running_number == p_ctx->organization.active_config_state.max_running_number) {
                    p_ctx->organization.active_config_state.iso_running_number = 1U;
                } else {
                    p_ctx->organization.active_config_state.iso_running_number ++; //ucode 0 is used for some case. start from 1
                }
                iso_config_id = p_ctx->organization.active_config_state.active_iso_idx;
                rval |= img_exe_construct_config_container_controller(context_id, iso_config_id, &ct_ctrler);
                rval |= img_ctx_get_iso_config_and_state(context_id, iso_config_id, &temp_address);
                (void)amba_ik_system_memcpy(&iso_config_and_state, &temp_address, sizeof(intptr));
                iso_cfg_tag = img_ctx_generate_config_tag(context_id);
                if(ct_ctrler.pack_container != NULL) {
                    rval |= ct_ctrler.pack_container(&ct_ctrler.handler, iso_cfg_tag);
                }
                if (ct_ctrler.handler.container != NULL) {
                    p_execute_container->ik_id = ct_ctrler.handler.container->ik_id;
                    {
                        ik_cv2_liso_cfg_t *p_iso_cfg = &iso_config_and_state->iso_config;
                        p_iso_cfg->share.iso_config_tag = ct_ctrler.handler.container->ik_id;
                        (void)amba_ik_system_memcpy(&p_iso_cfg_misra, &p_iso_cfg, sizeof(void *));
                        amba_ik_system_clean_cache(p_iso_cfg_misra, sizeof(ik_cv2_liso_cfg_t));
                        (void)amba_ik_system_memcpy(&p_execute_container->p_iso_cfg, &p_iso_cfg, sizeof(ik_cv2_liso_cfg_t *));
                    }
                }
            } else {//eis_mode == STAGE_2_FOR_EIS_EXECUTE
                p_execute_container->ik_id = explicit_eis_ik_id;
                rval |= img_ctx_get_iso_config_and_state(context_id, iso_config_id, &temp_address);
                (void)amba_ik_system_memcpy(&iso_config_and_state, &temp_address, sizeof(intptr));
                {
                    const ik_cv2_liso_cfg_t *p_iso_cfg = &iso_config_and_state->iso_config;
                    (void)amba_ik_system_memcpy(&p_execute_container->p_iso_cfg, &p_iso_cfg, sizeof(ik_cv2_liso_cfg_t *));
                }
            }
            rval |= img_cfg_update_config_flags(&iso_config_and_state->iso_config, &p_execute_container->update);
        } else {
            rval |= s_execute_config_error_check(context_id, p_execute_container, eis_mode, explicit_eis_ik_id, \
                                                 p_ctx, next_eis_running_number, eis_running_number, p_explicit_runnung_number, is_reasonable_delay, rval);
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] Error, can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

static uint32 s_execute_config_still(uint32 context_id, ik_execute_container_t *p_execute_container)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_config_container_controller_t ct_ctrler;
    uint32 iso_config_id = 0, iso_cfg_tag = 0;
    amba_ik_iso_config_and_state_t *iso_config_and_state = NULL;
    intptr temp_address;
    uint32  ik_debug_level;
    const void *p_iso_cfg_misra;
    amba_ik_hiso_config_and_state_t *hiso_config_and_state = NULL;
    ct_ctrler.pack_container = NULL;
    ct_ctrler.handler.container = NULL;
    (void)ik_get_debug_level(&ik_debug_level);
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        if ((p_execute_container != NULL) && (p_ctx->organization.initial_flag != 0U)) {
            rval |= p_ctx->organization.executer_method.execute_iso_config(context_id);
            if(p_ctx->organization.active_config_state.iso_running_number == p_ctx->organization.active_config_state.max_running_number) {
                p_ctx->organization.active_config_state.iso_running_number = 1U;
            } else {
                p_ctx->organization.active_config_state.iso_running_number ++; //ucode 0 is used for some case. start from 1
            }
            iso_config_id = p_ctx->organization.active_config_state.active_iso_idx;
            rval |= img_exe_construct_config_container_controller(context_id, iso_config_id, &ct_ctrler);
            rval |= img_ctx_get_iso_config_and_state(context_id, iso_config_id, &temp_address);
            if(p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_HISO) {
                (void)amba_ik_system_memcpy(&hiso_config_and_state, &temp_address, sizeof(intptr));
            } else {
                (void)amba_ik_system_memcpy(&iso_config_and_state, &temp_address, sizeof(intptr));
            }
            iso_cfg_tag = img_ctx_generate_config_tag(context_id);
            if(ct_ctrler.pack_container != NULL) {
                rval |= ct_ctrler.pack_container(&ct_ctrler.handler, iso_cfg_tag);
            }
            if (ct_ctrler.handler.container != NULL) {
                p_execute_container->ik_id = ct_ctrler.handler.container->ik_id;
                if ((p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_HISO) && (hiso_config_and_state != NULL)) {
                    ik_cv2_hiso_cfg_t *p_iso_cfg = &hiso_config_and_state->iso_config;
                    p_iso_cfg->share.iso_config_tag = ct_ctrler.handler.container->ik_id;
                    (void)amba_ik_system_memcpy(&p_iso_cfg_misra, &p_iso_cfg, sizeof(void *));
                    amba_ik_system_clean_cache(p_iso_cfg_misra, sizeof(ik_cv2_hiso_cfg_t));
                    (void)amba_ik_system_memcpy(&p_execute_container->p_iso_cfg, &p_iso_cfg, sizeof(ik_cv2_hiso_cfg_t *));
                } else {
                    if (iso_config_and_state != NULL) {
                        ik_cv2_liso_cfg_t *p_iso_cfg = &iso_config_and_state->iso_config;
                        p_iso_cfg->share.iso_config_tag = ct_ctrler.handler.container->ik_id;
                        (void)amba_ik_system_memcpy(&p_iso_cfg_misra, &p_iso_cfg, sizeof(void *));
                        amba_ik_system_clean_cache(p_iso_cfg_misra, sizeof(ik_cv2_liso_cfg_t));
                        (void)amba_ik_system_memcpy(&p_execute_container->p_iso_cfg, &p_iso_cfg, sizeof(ik_cv2_liso_cfg_t *));
                        rval |= img_cfg_update_config_flags(&iso_config_and_state->iso_config, &p_execute_container->update);
                    }
                }
            }
        } else {
            if(p_execute_container == NULL) {
                amba_ik_system_print_str_5("[IK] Error, NULL pointer in %s", __func__, DC_S, DC_S, DC_S, DC_S);
                rval |= IK_ERR_0005;
            } else if(p_ctx->organization.initial_flag == 0U) {
                amba_ik_system_print_uint32_5("[IK] Error, context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            } else {
                amba_ik_system_print_uint32_5("[IK] Error, s_execute_config_still() Fail", DC_U, DC_U, DC_U, DC_U, DC_U);
            }
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] Error, can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

#if 0
uint32 ik_lock_config(uint32 config_tag)
{
    // TODO:
    if(config_tag == 0U) {
        //
    }
    return IK_OK;
}
#endif
uint32 ik_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info)
{
    return img_exe_query_calc_geo_buf_size(p_buf_size_info);
}

uint32 ik_calc_geo_settings(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    return img_exe_calc_geo_settings(p_info, p_result);
}

uint32 ik_calc_warp_resources(const ik_query_warp_t *p_info, ik_query_warp_result_t *p_result)
{
    return img_exe_calc_warp_resources(p_info, p_result);
}

