/*
*  @file AmbaDSP_ImgDebugUtility.c
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

#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgDebugUtility.h"
#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgUtility.h"

uint32 ik_query_debug_config_size(uint32 context_id, size_t *p_size)
{
    uint32 rval;
    rval = img_cfg_query_debug_flow_data_size(context_id, p_size);
    return rval;
}

uint32 ik_dump_debug_config(uint32 context_id, uint32 previous_count, const void *p_user_buffer, size_t size)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    int32 flow_id;
    const idsp_flow_ctrl_t *p_flow = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tbl_list = NULL;
    uintptr addr = 0UL;
    size_t amalgam_size;
    uint32 pipe=0;
    uint32 video_pipe=0;
    uint32 still_pipe=0;

    rval = img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        pipe = p_ctx->organization.attribute.ability.pipe;
        video_pipe = p_ctx->organization.attribute.ability.video_pipe;
        still_pipe = p_ctx->organization.attribute.ability.still_pipe;

        (void)img_cfg_query_debug_flow_data_size(context_id, &amalgam_size);
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
            if(previous_count > (p_ctx->organization.attribute.cr_ring_number - 1u)) {
                amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > (cr_ring_number-1) = %d", (p_ctx->organization.attribute.cr_ring_number - 1u), DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD
            }
            if (rval == IK_OK) { // jack, TBD, do we need a mutex here?? what if active_flow_idx are under execution?
                flow_id = (int32)p_ctx->organization.active_cr_state.active_flow_idx - (int32)previous_count;
                if ((p_ctx->organization.active_cr_state.cr_running_number < p_ctx->organization.attribute.cr_ring_number) && (flow_id < 0)) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > cr_running_number = %d", p_ctx->organization.active_cr_state.cr_running_number, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0008;
                } else { //check cr_running_number > cr_ring_number
                    if (flow_id < 0) {
                        flow_id = flow_id + (int32)p_ctx->organization.attribute.cr_ring_number;
                    }
                    rval |= img_ctx_get_flow_control(context_id, (uint32)flow_id, &addr);
                    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
                    rval |= img_ctx_get_flow_tbl_list(context_id, (uint32)flow_id, &addr);

                    if ((p_flow != NULL) && (addr != 0UL)) {
                        if(pipe == AMBA_IK_VIDEO_PIPE) {
                            if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH)||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB)||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                                (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                                (video_pipe == AMBA_IK_STILL_PIPE_LISO)) {
                                (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
                                rval |= img_cfg_amalgam_data_update(p_flow, p_flow_tbl_list, p_user_buffer);
                                rval |= img_cfg_amalgam_data_ctx_update(context_id, p_user_buffer);
                            } else if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                                       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                                (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl_list, &addr, sizeof(void *));
                                rval |= img_cfg_motion_fusion_amalgam_data_update(p_flow, p_motion_fusion_flow_tbl_list, p_user_buffer);
                                rval |= img_cfg_motion_fusion_amalgam_data_ctx_update(context_id, p_user_buffer);
                                //rval |= img_cfg_amalgam_data_ctx_update(context_id, p_user_buffer);
                            } else {
                                // misraC
                            }
                        } else if(pipe == AMBA_IK_STILL_PIPE) {
                            if (still_pipe == AMBA_IK_STILL_PIPE_LISO) {
                                (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));
                                rval |= img_cfg_amalgam_data_update(p_flow, p_flow_tbl_list, p_user_buffer);
                                rval |= img_cfg_amalgam_data_ctx_update(context_id, p_user_buffer);
                            }
                        } else {
                            // misraC
                        }
                    } else {
                        amba_ik_system_print_uint32_5("[IK][ERROR] ik_dump_debug_config() p_flow is NULL \n", DC_U, DC_U, DC_U, DC_U, DC_U);
                    }
                }
            }
        }
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] can't get ctx_id %d",context_id, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

uint32 ik_query_hiso_debug_config_size(size_t *p_size)
{
    uint32 rval;
    rval = img_cfg_hiso_query_debug_flow_data_size(p_size);
    return rval;
}

uint32 ik_dump_hiso_debug_config(uint32 context_id, uint32 previous_count,const void *p_user_buffer, size_t size)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    int32 flow_id;
    const idsp_flow_ctrl_t *p_flow = NULL;
    const amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list = NULL;
    uintptr addr = 0UL;
    size_t amalgam_size;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        (void)img_cfg_hiso_query_debug_flow_data_size(&amalgam_size);
        if ((p_user_buffer == NULL)||(size < (uint32)amalgam_size)) {
            if(p_user_buffer == NULL) {
                amba_ik_system_print_uint32_5("[IK][ERROR] p_user_buffer is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0005;
            }
            if(size < (uint32)amalgam_size) {
                amba_ik_system_print_uint32_5("[IK][ERROR] size < sizeof(amalgam_hiso_t)", DC_U, DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0007;
            }
        } else {
            if(previous_count > (p_ctx->organization.attribute.cr_ring_number - 1u)) {
                amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > (cr_ring_number-1) = %d", (p_ctx->organization.attribute.cr_ring_number - 1u), DC_U, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD
            }
            if (rval == IK_OK) { // jack, TBD, do we need a mutex here?? what if active_flow_idx are under execution?
                flow_id = (int32)p_ctx->organization.active_cr_state.active_flow_idx - (int32)previous_count;
                if ((p_ctx->organization.active_cr_state.cr_running_number < p_ctx->organization.attribute.cr_ring_number) && (flow_id < 0)) {
                    amba_ik_system_print_uint32_5("[IK][ERROR] previous_count > cr_running_number = %d", p_ctx->organization.active_cr_state.cr_running_number, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0008;
                } else { //check cr_running_number > cr_ring_number
                    if (flow_id < 0) {
                        flow_id = flow_id + (int32)p_ctx->organization.attribute.cr_ring_number;
                    }
                    rval |= img_ctx_get_flow_control(context_id, (uint32)flow_id, &addr);
                    (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));
                    rval |= img_ctx_get_flow_tbl_list(context_id, (uint32)flow_id, &addr);
                    (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

                    if ((p_flow != NULL) && (p_flow_tbl_list != NULL)) {
                        rval |= img_cfg_hiso_amalgam_data_update(p_flow, p_flow_tbl_list, p_user_buffer);
                    } else {
                        amba_ik_system_print_uint32_5("[IK][ERROR] ik_dump_debug_config() p_flow is NULL \n", DC_U, DC_U, DC_U, DC_U, DC_U);
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

    if ((p_query_idsp_clock_info->vin_sensor_geo.width == 0U) || (p_query_idsp_clock_info->vin_sensor_geo.height == 0U)) {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_query_idsp_clock() incorrect vin_sensor_geo width %d height %d\n", \
                                      p_query_idsp_clock_info->vin_sensor_geo.width, p_query_idsp_clock_info->vin_sensor_geo.height, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    if ((p_query_idsp_clock_info->main_window.width == 0U) || (p_query_idsp_clock_info->main_window.height == 0U)) {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_query_idsp_clock() incorrect vin_sensor_geo width %d height %d\n", \
                                      p_query_idsp_clock_info->main_window.width, p_query_idsp_clock_info->main_window.height, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }
    if (p_query_idsp_clock_info->fps == 0U) {
        amba_ik_system_print_uint32_5("[IK][ERROR] ik_query_idsp_clock() incorrect fps %d \n", \
                                      p_query_idsp_clock_info->fps, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0008;
    }

    vin_pixels = p_query_idsp_clock_info->vin_sensor_geo.width*p_query_idsp_clock_info->vin_sensor_geo.height;
    main_pixels = p_query_idsp_clock_info->main_window.width*p_query_idsp_clock_info->main_window.height;
    max_pixels = (vin_pixels > main_pixels)?vin_pixels:main_pixels;
    p_query_idsp_clock_rst->theoretical_clock = (uint32)(((uint64)max_pixels * p_query_idsp_clock_info->fps * 1130U) >> 10);

    return rval;
}

void ik_hook_debug_check(void)
{
    ik_ctx_hook_debug_check();
}

