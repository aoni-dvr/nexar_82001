/*
*  @file AmbaDSP_ImgExecuter.c
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

#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgUtility.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImgAdvancedFilter.h"

#define IK_DBG_PROFILE_EXE 0

static INLINE uint32 exe_get_current_ring_buffer_index(uint32 buffer_number, uint32 current_index)
{
    uint32 rval;
    rval = (current_index - 1U) % buffer_number;
    return rval;
}

static uint32 uint_execute_config_eis_check(const amba_ik_context_entity_t *p_ctx,
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
        uint32 *p_ik_cfg_id)
{
    uint32 rval = IK_OK;
    if ((next_eis_running_number != eis_running_number) && (*p_explicit_runnung_number != 0xFFFFFFFFU)) {// check continuity.
        amba_ik_system_print_uint32_5("[IK] EIS check id doesn't follow continuity (eis_ik_id:%d)", explicit_eis_ik_id, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else if ((eis_ctx_id != context_id) || (eis_pipe != (uint32)p_ctx->organization.attribute.ability.pipe) || (eis_ability != p_ctx->organization.attribute.ability.video_pipe)) {
        // check ctx_id, pipe, ability consistency.
        amba_ik_system_print_uint32_5("[IK] EIS check fail on ctx:%d, pipe%d and ability:%d", context_id, p_ctx->organization.attribute.ability.pipe, p_ctx->organization.attribute.ability.video_pipe, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else if (is_reasonable_delay == 0U) {// eis delay > eis_max_delay_framess.
        amba_ik_system_print_uint32_5("[IK] EIS check fail on delay overflow", DC_U, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    } else {// set lastest stage2 ik_id running number.
        *p_explicit_runnung_number = eis_running_number;
        *p_explicit_iso_runnung_number = (p_ctx->organization.active_cr_state.active_flow_idx + p_ctx->organization.attribute.cr_ring_number - diff_id)%p_ctx->organization.attribute.cr_ring_number;
        *p_ik_cfg_id = *p_explicit_iso_runnung_number;
    }
    return rval;
}

static uint32 unit_execute(uint32 context_id, ik_execute_container_t *p_execute_container, uint32 eis_mode, uint32 explicit_eis_ik_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 flow_idx = 0;
    uintptr addr = 0;
    uint32 ik_cfg_id;
    uint32 eis_running_number=0U, next_eis_running_number=0U, *p_explicit_runnung_number=NULL, *p_explicit_iso_runnung_number=NULL, eis_legal_delay_idx, eis_delay;
    uint32 eis_ctx_id, eis_pipe, eis_ability, is_reasonable_delay=0U, diff_id, eis_max_delay_frames=0U;
    uint32 *p_misra_u32;

#if IK_DBG_PROFILE_EXE
    uint32 start_tick = 0;
    uint32 end_tick = 0;
    uint32 start_sys_time = 0;
    uint32 end_sys_time = 0;

    extern UINT32 AmbaKAL_GetSysTickCount(UINT32 * pSysTickCount);
    extern UINT32 AmbaRTSL_GetOrcTimer(void);

    (void)AmbaKAL_GetSysTickCount(&start_sys_time);
    start_tick = AmbaRTSL_GetOrcTimer();
#endif
    if ((eis_mode==0U) || (explicit_eis_ik_id==0U)) {
        // misraC
    }

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        (void)amba_ik_system_lock_mutex(&p_ctx->organization.mutex, IK_WAIT_FOREVER);

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
            if(*p_explicit_runnung_number == p_ctx->organization.active_cr_state.max_running_number) {
                next_eis_running_number = 1U;
            } else {
                next_eis_running_number = *p_explicit_runnung_number + 1U;
            }
            rval |= img_ctx_get_explicit_iso_running_num(context_id, &p_explicit_iso_runnung_number);
            eis_legal_delay_idx = ctx_parse_running_number((uint32)p_ctx->organization.active_cr_state.cr_running_number);
            is_reasonable_delay = 0U;
            eis_max_delay_frames = p_ctx->organization.attribute.cr_ring_number - 2U; /*1 for ucode, 1 for stage1*/
            diff_id = 0U;
            for (eis_delay=0U; eis_delay<=eis_max_delay_frames; eis_delay++) {
                if (eis_legal_delay_idx==eis_running_number) {
                    is_reasonable_delay = 1U;
                    break;
                }
                diff_id++;
                //eis_legal_delay_idx = (eis_legal_delay_idx + 65536UL - 1UL) & 0x0000FFFFUL;
                if(eis_legal_delay_idx == 1UL) {
                    eis_legal_delay_idx = p_ctx->organization.active_cr_state.max_running_number;
                } else {
                    eis_legal_delay_idx = eis_legal_delay_idx - 1U;
                }
            }
            rval |= uint_execute_config_eis_check(p_ctx, explicit_eis_ik_id, next_eis_running_number, eis_running_number, p_explicit_runnung_number, eis_ctx_id, context_id, eis_pipe, eis_ability, is_reasonable_delay, p_explicit_iso_runnung_number, diff_id, &ik_cfg_id);
        }

        if ((p_execute_container != NULL) && (rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
            if (eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
                if(p_ctx->organization.active_cr_state.cr_running_number == p_ctx->organization.active_cr_state.max_running_number) {
                    p_ctx->organization.active_cr_state.cr_running_number = 1u;
                } else {
                    p_ctx->organization.active_cr_state.cr_running_number++; //ucode 0 is used for some case. start from 1
                }
                p_ctx->filters.input_param.eis_mode = eis_mode;// 0 : normal execute, 1 : before EIS warp execute, 2 : with EIS warp execute.
                rval |= p_ctx->organization.executer_method.execute_cr(context_id);
                flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;

                rval |= img_ctx_get_flow_control(context_id, flow_idx, &addr);
                (void)amba_ik_system_memcpy(&p_execute_container->p_flow_ctrl, &addr, sizeof(idsp_flow_ctrl_t *));
                (void)amba_ik_system_memcpy(&p_misra_u32, &addr, sizeof(uint32*));
                ik_cfg_id = p_misra_u32[0];
                p_execute_container->ik_id = ik_cfg_id;
                p_ctx->organization.active_cr_state.ik_id = ik_cfg_id;
                //record the running number at the end of unit_execute to prevent the unexpected termination
                p_ctx->organization.active_cr_state.cr_running_number_is_finished = p_ctx->organization.active_cr_state.cr_running_number;
            } else {
                p_ctx->filters.input_param.eis_mode = eis_mode;// 0 : normal execute, 1 : before EIS warp execute, 2 : with EIS warp execute.
                rval |= p_ctx->organization.executer_method.execute_cr(context_id);
                eis_running_number = ctx_parse_running_number(explicit_eis_ik_id);

                flow_idx = exe_get_current_ring_buffer_index(p_ctx->organization.attribute.cr_ring_number, eis_running_number);
                rval |= img_ctx_get_flow_control(context_id, flow_idx, &addr);
                (void)amba_ik_system_memcpy(&p_execute_container->p_flow_ctrl, &addr, sizeof(idsp_flow_ctrl_t *));

                p_execute_container->ik_id = explicit_eis_ik_id;
            }
        } else {
            if(p_execute_container == NULL) {
                amba_ik_system_print_str_5("[IK] NULL pointer in %s", __func__, DC_S, DC_S, DC_S, DC_S);
                rval |= IK_ERR_0005;
            } else {
                amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
                rval |= IK_ERR_0002;
            }
        }

        (void)amba_ik_system_unlock_mutex(&p_ctx->organization.mutex);
    } else {
        amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
    }

#if IK_DBG_PROFILE_EXE
    end_tick = AmbaRTSL_GetOrcTimer();
    (void)AmbaKAL_GetSysTickCount(&end_sys_time);
    amba_ik_system_print_uint32_5("[IK][Profile ik_execute()] flow_idx %d, sys_time = %d ms, tick = %d", flow_idx, (end_sys_time - start_sys_time), (end_tick - start_tick), DC_U, DC_U);
#endif

    return rval;
}

uint32 ik_execute(uint32 context_id, ik_execute_container_t *p_execute_container)
{
    return unit_execute(context_id, p_execute_container, REGULAR_EXECUTE, 0U);
}

uint32 ik_execute_eis_1st(uint32 context_id, ik_execute_container_t *p_execute_container)
{
    return unit_execute(context_id, p_execute_container, STAGE_1_FOR_EIS_EXECUTE, 0U);
}

uint32 ik_execute_eis_2nd(uint32 context_id, uint32 explicit_eis_ik_id, ik_execute_container_t *p_execute_container)
{
    return unit_execute(context_id, p_execute_container, STAGE_2_FOR_EIS_EXECUTE, explicit_eis_ik_id);
}

uint32 ik_pre_execute_check(uint32 context_id, const ik_pre_execute_check_in_t *p_pre_execute_check_in, ik_pre_execute_check_out_t *p_pre_execute_check_out)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 config_current, config_in_use, config_next, config_ring;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        p_pre_execute_check_out->ik_id_current = p_ctx->organization.active_cr_state.ik_id;
        config_current = p_pre_execute_check_out->ik_id_current & 0x0000FFFFU;
        config_in_use = p_pre_execute_check_in->ik_id_in_use & 0x0000FFFFU;
        if (config_in_use > config_current) {
            config_next = config_current + 1U + (uint32)p_ctx->organization.active_cr_state.max_running_number;
            config_ring = config_in_use + p_ctx->organization.attribute.cr_ring_number - 1U;
        } else {
            config_next = config_current + 1U;
            config_ring = config_in_use + p_ctx->organization.attribute.cr_ring_number - 1U;
        }
        p_pre_execute_check_out->remain_buffer_num = config_ring - config_next + 1U;
        if (p_pre_execute_check_out->remain_buffer_num > p_ctx->organization.attribute.cr_ring_number) {
            amba_ik_system_print_uint32_5("[IK] remain_buffer_num:%ld > cr_ring_number:%d", p_pre_execute_check_out->remain_buffer_num, p_ctx->organization.attribute.cr_ring_number, DC_U, DC_U, DC_U);
            amba_ik_system_print_uint32_5("[IK] config_current:%ld > config_in_use:%d", config_current, config_in_use, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0000;
        } else if (p_pre_execute_check_out->remain_buffer_num == 0U) {
            amba_ik_system_print_uint32_5("[IK] remain_buffer_num:%d == 0", p_pre_execute_check_out->remain_buffer_num, DC_U, DC_U, DC_U, DC_U);
            rval |= IK_ERR_0000;
        } else {
            // misra
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] can't get context", DC_U, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0000;
    }

    return rval;
}

uint32 ik_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info)
{
    return img_exe_query_calc_geo_buf_size(p_buf_size_info);
}

uint32 ik_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    return img_exe_calc_geo_settings(p_ability, p_info, p_result);
}

uint32 ik_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out)
{
    return img_exe_warp_remap(p_warp_remap_in, p_warp_remap_out);
}

uint32 ik_warp_remap_hvh_vr(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out_1st, ik_out_warp_remap_t *p_warp_remap_out_2nd)
{
    return img_exe_warp_remap_hvh_vr(p_warp_remap_in, p_warp_remap_out_1st, p_warp_remap_out_2nd);
}

uint32 ik_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out)
{
    return img_exe_cawarp_remap(p_cawarp_remap_in, p_cawarp_remap_out);
}

uint32 ik_query_frame_info(uint32 context_id, const uint32 ik_id, ik_query_frame_info_t *p_frame_info)
{
    return img_exe_query_frame_info(context_id, ik_id, p_frame_info);
}

