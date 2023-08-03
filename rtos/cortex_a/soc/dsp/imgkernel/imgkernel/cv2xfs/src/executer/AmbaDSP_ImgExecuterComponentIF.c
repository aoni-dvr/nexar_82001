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

#include "AmbaDSP_ImgExecuterComponentIF_static.c"

uint32 img_exe_inject_safety_error(uint32 context_id, const uint32 selection)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    uintptr addr = 0;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        rval |= img_ctx_get_flow_tbl_list(context_id, p_ctx->organization.active_cr_state.active_flow_idx, &addr);
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
        rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);

        if (rval == IK_OK) {
            switch (selection) {
#if SUPPORT_CR_MEMORY_FENCE
            case AMBA_IK_SAFETY_ERROR_MEM_FENCE:
                rval = img_exe_inject_safety_error_mem_fence(p_ctx, p_flow_tbl_list);
                break;
#endif
            case AMBA_IK_SAFETY_ERROR_RING_BUF:
                p_ctx->filters.input_param.ring_buffer_error_injection = 1U;
                break;
            case AMBA_IK_SAFETY_ERROR_CRC_MISMATCH:
                // send incorrect crc by corrupt cr buffer.
                // choose the cr buffer which do not have cr memory fence, so it won't trigger AMBA_IK_SAFETY_ERROR_MEM_FENCE
                // only corrupt ik cr buffer to make crc inconsistency.
                if(p_ik_buffer_info->safety_crc_enable == 1u) {
                    p_ctx->filters.input_param.crc_mismatch_error_injection = 1U;
                }
                break;
            /*
            case corrupt default binary:
                we can't have this test because this api should be used after ctx_init (dut to mutex), but default bin are set before/within ctx_init.
                break;
            */
            case AMBA_IK_FORCE_SAFETY_ERROR:
                p_ik_buffer_info->safety_state = IK_SAFETY_STATE_ERROR;
                break;
            case AMBA_IK_SAFETY_SYS_API_ERROR:
                amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, 30u);
                (void)amba_ik_system_memcpy(p_flow_tbl_list->p_CR_buf_4, p_flow_tbl_list->p_CR_buf_4, 0u);
                (void)amba_ik_system_memset(NULL, 0x0, 0u);
                break;
            default:
                rval = IK_ERR_0008;
                break;
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_tbl_list(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 img_exe_init_executer(uint32 context_id)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        if (p_ctx->organization.initial_flag != 0U) {
            if (p_ctx->organization.attribute.ability.pipe == AMBA_IK_PIPE_VIDEO) {
                if ((equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_LINEAR)+\
                     equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_Y2Y)+ \
                     equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_LINEAR_CE)+
                     equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_HDR_EXPO_2)+
                     equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_HDR_EXPO_3)) != 0u) {
                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_first_cr;
                } else {
                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_invalid_cr;
                    amba_ik_system_print_uint32_5("[IK] not support this video_pipe %d", p_ctx->organization.attribute.ability.video_pipe, DC_U, DC_U, DC_U, DC_U);
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] not support this pipe %d", p_ctx->organization.attribute.ability.pipe, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

uint32 img_exe_init_safety_executer(uint32 context_id)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        p_ctx->organization.executer_method.execute_cr = img_exe_execute_safety_cr;
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

uint32 img_exe_query_calc_geo_buf_size(ik_query_calc_geo_buf_size_t *p_buf_size_info)
{
    return exe_query_calc_geo_buf_size(p_buf_size_info);
}

uint32 img_exe_calc_geo_settings(const ik_ability_t *p_ability, const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    return exe_calc_geo_settings(p_ability, p_info, p_result);
}

uint32 img_exe_warp_remap(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out)
{
    return exe_warp_remap(p_warp_remap_in, p_warp_remap_out);
}

uint32 img_exe_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out)
{
    return exe_cawarp_remap(p_cawarp_remap_in, p_cawarp_remap_out);
}

uint32 img_exe_query_frame_info(uint32 context_id, const uint32 ik_id, ik_query_frame_info_t *p_frame_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    uintptr addr;
    uint32 running_num;
    uint32 active_id;
    uint32 current_running_num;
    uint32 max_running_num;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        if (p_frame_info == NULL) {
            amba_ik_system_print_uint32_5("[IK][ERROR] p_frame_info is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0005;
        } else {
            max_running_num = p_ctx->organization.active_cr_state.max_running_number;
            current_running_num = p_ctx->organization.active_cr_state.cr_running_number;
            running_num = ik_id - ((ik_id>>16UL)<<16UL);
            active_id = (running_num-1UL) % p_ctx->organization.attribute.cr_ring_number;
            rval |= exe_query_frame_info_check_run_num((uint32)current_running_num, (uint32)max_running_num, (uint32)running_num, (uint32)p_ctx->organization.attribute.cr_ring_number);
            if (rval == IK_OK) {
                rval |= img_ctx_get_flow_tbl_list(context_id, (uint32)active_id, &addr);
                (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

                if (p_flow_tbl_list->p_frame_info != NULL) {
                    (void)amba_ik_system_memcpy(p_frame_info, p_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t));
                } else {
                    amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_query_frame_info() p_flow_tbl_list is NULL \n", DC_U, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0000; // TBD
                }
            }
        }
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] can't get ctx_id %d",context_id, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

