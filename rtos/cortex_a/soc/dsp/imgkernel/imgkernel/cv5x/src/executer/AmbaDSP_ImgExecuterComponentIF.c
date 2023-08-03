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

#include "AmbaDSP_ImgConfigComponentIF.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgExecuterComponentIF.h"
#include "AmbaDSP_ImgArchComponentIF.h"
//#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_ImgExecuterUtilities.h"
#include "AmbaDSP_ImgExecuterCalib.h"
#include "AmbaDSP_ImgContextDebugUtility.h"
#include "AmbaDSP_ImgProcess.h"


//uint8 u_earlytest_mode = 0U;
//u_earlytest_mode
// 0: Both exe_process_hiso_cr and Hack HISO
// 1: Hack HISO only
// 2. exe_process_hiso_cr only

//for misra casting.
static inline uint32 physical2u32(const void *p)
{
    uint32 out;
    (void)amba_ik_system_memcpy(&out, &p, sizeof(uint32));
    return out;
}

static INLINE uint32 exe_clone_flow_control_and_tbl_list(uint32 context_id, idsp_flow_ctrl_t *p_dst_flow, void *p_dst_flow_tbl_list, uint32 src_flow_idx)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    const idsp_flow_ctrl_t *p_src_flow = NULL;
    const void *p_src_flow_tbl_list = NULL;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 video_pipe;

    // get ability
    rval |= img_ctx_get_context(context_id, &p_ctx);
    video_pipe = p_ctx->organization.attribute.ability.video_pipe;

    rval |= img_ctx_get_flow_control(context_id, src_flow_idx, &addr);
    (void)amba_ik_system_memcpy(&p_src_flow, &addr, sizeof(void *));

    rval |= img_ctx_get_flow_tbl_list(context_id, src_flow_idx, &addr);
    (void)amba_ik_system_memcpy(&p_src_flow_tbl_list, &addr, sizeof(void *));

    if (rval == IK_OK) {
        (void)amba_ik_system_memcpy(p_dst_flow, p_src_flow, sizeof(idsp_flow_ctrl_t));
        if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
            (void)amba_ik_system_memcpy(p_dst_flow_tbl_list, p_src_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t));
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
            (void)amba_ik_system_memcpy(p_dst_flow_tbl_list, p_src_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t));
        } else {
            // misra
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] call exe_clone_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0000;
    }

    return rval;
}

static INLINE uint32 exe_get_next_ring_buffer_index(uint32 buffer_number, uint32 current_index)
{
    uint32 rval;
    rval = (current_index+1U) % buffer_number;
    return rval;
}

static INLINE uint32 exe_get_next_flow_control_index(uint32 buffer_number, const amba_ik_active_cr_state_t *active_cr_state, uint32 *p_flow_idx)
{
    uint32 rval = IK_OK;
    if (active_cr_state == NULL) {
        rval = IK_ERR_0005;
    } else {
        *p_flow_idx = exe_get_next_ring_buffer_index(buffer_number, active_cr_state->active_flow_idx);
    }
    return rval;
}

static uint32 exe_r2y_get_next_cr_index(uint32 buffer_number,
                                        amba_ik_filter_update_flags_t *p_update_flag,
                                        amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;

    uint32 window_calculate_updated = 0U;

    if ((p_update_flag == NULL) || (p_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        if ((p_update_flag->iso.window_size_info_updated != 0U) ||
            (p_update_flag->iso.cfa_window_size_info_updated != 0U) ||
            (p_update_flag->iso.dmy_range_updated != 0U) ||
            (p_update_flag->iso.dzoom_info_updated != 0U) ||
            (p_update_flag->iso.vin_active_win_updated != 0U) ||
            (p_update_flag->iso.stitching_info_updated != 0U) ||
            (p_update_flag->iso.overlap_x_updated != 0U) ||
            (p_update_flag->iso.flip_mode_updated != 0U) ||
            (p_update_flag->iso.warp_enable_updated != 0U) ||
            (p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            window_calculate_updated = 1U;
        }

        if (window_calculate_updated != 0U) {
            if(p_update_flag->cr.cawarp == 0U) {
                p_update_flag->cr.cawarp = 1;
                p_table_index->cawarp = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->cawarp);
            }
            if(p_update_flag->cr.warp == 0U) {
                p_update_flag->cr.warp = 1;
                p_table_index->warp = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp);
            }
            if(p_update_flag->cr.warp_2nd == 0U) {
                p_update_flag->cr.warp_2nd = 1;
                p_table_index->warp_2nd = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp_2nd);
            }
            if(p_update_flag->cr.aaa_data == 0U) {
                p_update_flag->cr.aaa_data = 1;
                p_table_index->aaa_data = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->aaa_data);
            }
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_9_update == 0U) {
                p_update_flag->cr.r2y.cr_9_update = 1;
                p_table_index->r2y.cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_9_index);
            }
            if(p_update_flag->cr.r2y.cr_10_update == 0U) {
                p_update_flag->cr.r2y.cr_10_update = 1;
                p_table_index->r2y.cr_10_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_10_index);
            }
            if(p_update_flag->cr.r2y.cr_11_update == 0U) {
                p_update_flag->cr.r2y.cr_11_update = 1;
                p_table_index->r2y.cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_11_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_13_update == 0U) {
                p_update_flag->cr.r2y.cr_13_update = 1;
                p_table_index->r2y.cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_13_index);
            }
            if(p_update_flag->cr.r2y.cr_16_update == 0U) {
                p_update_flag->cr.r2y.cr_16_update = 1;
                p_table_index->r2y.cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_16_index);
            }
            if(p_update_flag->cr.r2y.cr_17_update == 0U) {
                p_update_flag->cr.r2y.cr_17_update = 1;
                p_table_index->r2y.cr_17_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_17_index);
            }
            if(p_update_flag->cr.r2y.cr_21_update == 0U) {
                p_update_flag->cr.r2y.cr_21_update = 1;
                p_table_index->r2y.cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_21_index);
            }
            if(p_update_flag->cr.r2y.cr_28_update == 0U) {
                p_update_flag->cr.r2y.cr_28_update = 1;
                p_table_index->r2y.cr_28_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_28_index);
            }
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
            if(p_update_flag->cr.r2y.cr_33_update == 0U) {
                p_update_flag->cr.r2y.cr_33_update = 1;
                p_table_index->r2y.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_33_index);
            }
            if(p_update_flag->cr.r2y.cr_34_update == 0U) {
                p_update_flag->cr.r2y.cr_34_update = 1;
                p_table_index->r2y.cr_34_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_34_index);
            }
            if(p_update_flag->cr.r2y.cr_35_update == 0U) {
                p_update_flag->cr.r2y.cr_35_update = 1;
                p_table_index->r2y.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_35_index);
            }
            if(p_update_flag->cr.r2y.cr_36_update == 0U) {
                p_update_flag->cr.r2y.cr_36_update = 1;
                p_table_index->r2y.cr_36_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_36_index);
            }
            if(p_update_flag->cr.r2y.cr_37_update == 0U) {
                p_update_flag->cr.r2y.cr_37_update = 1;
                p_table_index->r2y.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_37_index);
            }
            if(p_update_flag->cr.r2y.cr_42_update == 0U) {
                p_update_flag->cr.r2y.cr_42_update = 1;
                p_table_index->r2y.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_42_index);
            }
            if(p_update_flag->cr.r2y.cr_43_update == 0U) {
                p_update_flag->cr.r2y.cr_43_update = 1;
                p_table_index->r2y.cr_43_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_43_index);
            }
            if(p_update_flag->cr.r2y.cr_44_update == 0U) {
                p_update_flag->cr.r2y.cr_44_update = 1;
                p_table_index->r2y.cr_44_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_44_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
            if(p_update_flag->cr.r2y.cr_47_update == 0U) {
                p_update_flag->cr.r2y.cr_47_update = 1;
                p_table_index->r2y.cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_47_index);
            }
            if(p_update_flag->cr.r2y.cr_48_update == 0U) {
                p_update_flag->cr.r2y.cr_48_update = 1;
                p_table_index->r2y.cr_48_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_48_index);
            }
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
            if(p_update_flag->cr.r2y.cr_51_update == 0U) {
                p_update_flag->cr.r2y.cr_51_update = 1;
                p_table_index->r2y.cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_51_index);
            }
            if(p_update_flag->cr.r2y.cr_100_update == 0U) {
                p_update_flag->cr.r2y.cr_100_update = 1;
                p_table_index->r2y.cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_100_index);
            }
            if(p_update_flag->cr.r2y.cr_101_update == 0U) {
                p_update_flag->cr.r2y.cr_101_update = 1;
                p_table_index->r2y.cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_101_index);
            }
        }
        if (p_update_flag->iso.use_y2y_enable_info_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_26_update == 0U) {
                p_update_flag->cr.r2y.cr_26_update = 1;
                p_table_index->r2y.cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_26_index);
            }
            if(p_update_flag->cr.r2y.cr_27_update == 0U) {
                p_update_flag->cr.r2y.cr_27_update = 1;
                p_table_index->r2y.cr_27_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_27_index);
            }
        }

        if (p_update_flag->iso.hdr_eis_shift_info_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
            if(p_update_flag->cr.extra_window == 0U) {
                p_update_flag->cr.extra_window = 1;
                p_table_index->extra_window = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->extra_window);
            }
        }

        if (p_update_flag->iso.sensor_information_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_13_update == 0U) {
                p_update_flag->cr.r2y.cr_13_update = 1;
                p_table_index->r2y.cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_13_index);
            }
            if(p_update_flag->cr.r2y.cr_51_update == 0U) {
                p_update_flag->cr.r2y.cr_51_update = 1;
                p_table_index->r2y.cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_51_index);
            }
            if(p_update_flag->cr.r2y.cr_24_update == 0U) {
                p_update_flag->cr.r2y.cr_24_update = 1;
                p_table_index->r2y.cr_24_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_24_index);
            }
            if(p_update_flag->cr.r2y.cr_23_update == 0U) {
                p_update_flag->cr.r2y.cr_23_update = 1;
                p_table_index->r2y.cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_23_index);
            }
            if(p_update_flag->cr.r2y.cr_22_update == 0U) {
                p_update_flag->cr.r2y.cr_22_update = 1;
                p_table_index->r2y.cr_22_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_22_index);
            }
            if(p_update_flag->cr.r2y.cr_21_update == 0U) {
                p_update_flag->cr.r2y.cr_21_update = 1;
                p_table_index->r2y.cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_21_index);
            }
            if(p_update_flag->cr.r2y.cr_16_update == 0U) {
                p_update_flag->cr.r2y.cr_16_update = 1;
                p_table_index->r2y.cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_16_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_11_update == 0U) {
                p_update_flag->cr.r2y.cr_11_update = 1;
                p_table_index->r2y.cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_11_index);
            }
            if(p_update_flag->cr.r2y.cr_9_update == 0U) {
                p_update_flag->cr.r2y.cr_9_update = 1;
                p_table_index->r2y.cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_9_index);
            }
            if(p_update_flag->cr.r2y.cr_6_update == 0U) {
                p_update_flag->cr.r2y.cr_6_update = 1;
                p_table_index->r2y.cr_6_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_6_index);
            }
            if(p_update_flag->cr.r2y.cr_7_update == 0U) {
                p_update_flag->cr.r2y.cr_7_update = 1;
                p_table_index->r2y.cr_7_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_7_index);
            }
            if(p_update_flag->cr.r2y.cr_8_update == 0U) {
                p_update_flag->cr.r2y.cr_8_update = 1;
                p_table_index->r2y.cr_8_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_8_index);
            }
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }

            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
            if(p_update_flag->cr.r2y.cr_47_update == 0U) {
                p_update_flag->cr.r2y.cr_47_update = 1;
                p_table_index->r2y.cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_47_index);
            }
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
        }

        if ((p_update_flag->iso.aaa_stat_info_updated != 0U)||(p_update_flag->iso.af_stat_ex_info_updated != 0U)||
            (p_update_flag->iso.pg_af_stat_ex_info_updated != 0U)||(p_update_flag->iso.histogram_info_update != 0U)||
            (p_update_flag->iso.histogram_info_pg_update != 0U)) {
            if(p_update_flag->cr.aaa_data == 0U) {
                p_update_flag->cr.aaa_data = 1;
                p_table_index->aaa_data = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->aaa_data);
            }
            if(p_update_flag->cr.r2y.cr_9_update == 0U) {
                p_update_flag->cr.r2y.cr_9_update = 1;
                p_table_index->r2y.cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_9_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_21_update == 0U) {
                p_update_flag->cr.r2y.cr_21_update = 1;
                p_table_index->r2y.cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_21_index);
            }
            if(p_update_flag->cr.r2y.cr_28_update == 0U) {
                p_update_flag->cr.r2y.cr_28_update = 1;
                p_table_index->r2y.cr_28_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_28_index);
            }
        }

        if ((p_update_flag->iso.static_bpc_updated != 0U) || (p_update_flag->iso.static_bpc_internal_updated != 0U)) {
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
        }

        if ((p_update_flag->iso.vignette_compensation_updated != 0U) || (p_update_flag->iso.vignette_compensation_internal_updated != 0U)) {
            if(p_update_flag->cr.r2y.cr_9_update == 0U) {
                p_update_flag->cr.r2y.cr_9_update = 1;
                p_table_index->r2y.cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_9_index);
            }
            if(p_update_flag->cr.r2y.cr_10_update == 0U) {
                p_update_flag->cr.r2y.cr_10_update = 1;
                p_table_index->r2y.cr_10_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_10_index);
            }
            if(p_update_flag->cr.r2y.cr_11_update == 0U) {
                p_update_flag->cr.r2y.cr_11_update = 1;
                p_table_index->r2y.cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_11_index);
            }
            if(p_update_flag->cr.r2y.cr_47_update == 0U) {
                p_update_flag->cr.r2y.cr_47_update = 1;
                p_table_index->r2y.cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_47_index);
            }
            if(p_update_flag->cr.r2y.cr_48_update == 0U) {
                p_update_flag->cr.r2y.cr_48_update = 1;
                p_table_index->r2y.cr_48_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_48_index);
            }
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
        }

        if ((p_update_flag->iso.calib_warp_info_updated!=0U)||(p_update_flag->iso.warp_internal_updated!=0U)||(p_update_flag->iso.chroma_filter_updated!=0U)) {
            if(p_update_flag->cr.warp == 0U) {
                p_update_flag->cr.warp = 1;
                p_table_index->warp = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp);
            }
            if(p_update_flag->cr.r2y.cr_33_update == 0U) {
                p_update_flag->cr.r2y.cr_33_update = 1;
                p_table_index->r2y.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_33_index);
            }
            if(p_update_flag->cr.r2y.cr_34_update == 0U) {
                p_update_flag->cr.r2y.cr_34_update = 1;
                p_table_index->r2y.cr_34_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_34_index);
            }
            if(p_update_flag->cr.r2y.cr_35_update == 0U) {
                p_update_flag->cr.r2y.cr_35_update = 1;
                p_table_index->r2y.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_35_index);
            }
            if(p_update_flag->cr.r2y.cr_42_update == 0U) {
                p_update_flag->cr.r2y.cr_42_update = 1;
                p_table_index->r2y.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_42_index);
            }
            if(p_update_flag->cr.r2y.cr_43_update == 0U) {
                p_update_flag->cr.r2y.cr_43_update = 1;
                p_table_index->r2y.cr_43_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_43_index);
            }
            if(p_update_flag->cr.r2y.cr_44_update == 0U) {
                p_update_flag->cr.r2y.cr_44_update = 1;
                p_table_index->r2y.cr_44_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_44_index);
            }
        }

        if ((p_update_flag->iso.calib_warp_2nd_info_updated!=0U)||(p_update_flag->iso.warp_internal_2nd_updated!=0U)||(p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            if(p_update_flag->cr.warp_2nd == 0U) {
                p_update_flag->cr.warp_2nd = 1;
                p_table_index->warp_2nd = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp_2nd);
            }
            if(p_update_flag->cr.r2y.cr_100_update == 0U) {
                p_update_flag->cr.r2y.cr_100_update = 1;
                p_table_index->r2y.cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_100_index);
            }
            if(p_update_flag->cr.r2y.cr_101_update == 0U) {
                p_update_flag->cr.r2y.cr_101_update = 1;
                p_table_index->r2y.cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_101_index);
            }
        }

        if ((p_update_flag->iso.calib_ca_warp_info_updated!=0U)||(p_update_flag->iso.cawarp_enable_updated!=0U)||(p_update_flag->iso.cawarp_internal_updated!=0U)) {
            if(p_update_flag->cr.cawarp == 0U) {
                p_update_flag->cr.cawarp = 1;
                p_table_index->cawarp = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->cawarp);
            }
            if(p_update_flag->cr.r2y.cr_16_update == 0U) {
                p_update_flag->cr.r2y.cr_16_update = 1;
                p_table_index->r2y.cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_16_index);
            }
            if(p_update_flag->cr.r2y.cr_17_update == 0U) {
                p_update_flag->cr.r2y.cr_17_update = 1;
                p_table_index->r2y.cr_17_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_17_index);
            }
        }

        if (p_update_flag->iso.before_ce_wb_gain_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_13_update == 0U) {
                p_update_flag->cr.r2y.cr_13_update = 1;
                p_table_index->r2y.cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_13_index);
            }
            if(p_update_flag->cr.r2y.cr_51_update == 0U) {
                p_update_flag->cr.r2y.cr_51_update = 1;
                p_table_index->r2y.cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_51_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
        }

        if (p_update_flag->iso.after_ce_wb_gain_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
            if(p_update_flag->cr.r2y.cr_22_update == 0U) {
                p_update_flag->cr.r2y.cr_22_update = 1;
                p_table_index->r2y.cr_22_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_22_index);
            }
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
        }

        if (p_update_flag->iso.cfa_leakage_filter_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
            if(p_update_flag->cr.r2y.cr_23_update == 0U) {
                p_update_flag->cr.r2y.cr_23_update = 1;
                p_table_index->r2y.cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_23_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
        }

        if (p_update_flag->iso.anti_aliasing_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_23_update == 0U) {
                p_update_flag->cr.r2y.cr_23_update = 1;
                p_table_index->r2y.cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_23_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
        }

        if (p_update_flag->iso.dynamic_bpc_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_11_update == 0U) {
                p_update_flag->cr.r2y.cr_11_update = 1;
                p_table_index->r2y.cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_11_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
        }

        if (p_update_flag->iso.grgb_mismatch_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_23_update == 0U) {
                p_update_flag->cr.r2y.cr_23_update = 1;
                p_table_index->r2y.cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_23_index);
            }
        }

        if (p_update_flag->iso.cfa_noise_filter_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_23_update == 0U) {
                p_update_flag->cr.r2y.cr_23_update = 1;
                p_table_index->r2y.cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_23_index);
            }
            if(p_update_flag->cr.r2y.cr_11_update == 0U) {
                p_update_flag->cr.r2y.cr_11_update = 1;
                p_table_index->r2y.cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_11_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
        }

        if (p_update_flag->iso.dgain_sat_lvl_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_22_update == 0U) {
                p_update_flag->cr.r2y.cr_22_update = 1;
                p_table_index->r2y.cr_22_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_22_index);
            }
        }

        if (p_update_flag->iso.demosaic_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_23_update == 0U) {
                p_update_flag->cr.r2y.cr_23_update = 1;
                p_table_index->r2y.cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_23_index);
            }
            if(p_update_flag->cr.r2y.cr_24_update == 0U) {
                p_update_flag->cr.r2y.cr_24_update = 1;
                p_table_index->r2y.cr_24_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_24_index);
            }
        }

        if (p_update_flag->iso.color_correction_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_26_update == 0U) {
                p_update_flag->cr.r2y.cr_26_update = 1;
                p_table_index->r2y.cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_26_index);
            }
            if(p_update_flag->cr.r2y.cr_27_update == 0U) {
                p_update_flag->cr.r2y.cr_27_update = 1;
                p_table_index->r2y.cr_27_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_27_index);
            }
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
        }

        if (p_update_flag->iso.pre_cc_gain_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_26_update == 0U) {
                p_update_flag->cr.r2y.cr_26_update = 1;
                p_table_index->r2y.cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_26_index);
            }
        }

        if (p_update_flag->iso.tone_curve_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_26_update == 0U) {
                p_update_flag->cr.r2y.cr_26_update = 1;
                p_table_index->r2y.cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_26_index);
            }
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
        }

        if (p_update_flag->iso.rgb_to_12y_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_25_update == 0U) {
                p_update_flag->cr.r2y.cr_25_update = 1;
                p_table_index->r2y.cr_25_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_25_index);
            }
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
        }

        if (p_update_flag->iso.rgb_to_yuv_matrix_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_29_update == 0U) {
                p_update_flag->cr.r2y.cr_29_update = 1;
                p_table_index->r2y.cr_29_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_29_index);
            }
        }

        if (p_update_flag->iso.rgb_ir_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_6_update == 0U) {
                p_update_flag->cr.r2y.cr_6_update = 1;
                p_table_index->r2y.cr_6_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_6_index);
            }
            if(p_update_flag->cr.r2y.cr_7_update == 0U) {
                p_update_flag->cr.r2y.cr_7_update = 1;
                p_table_index->r2y.cr_7_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_7_index);
            }
            if(p_update_flag->cr.r2y.cr_8_update == 0U) {
                p_update_flag->cr.r2y.cr_8_update = 1;
                p_table_index->r2y.cr_8_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_8_index);
            }
            if(p_update_flag->cr.r2y.cr_9_update == 0U) {
                p_update_flag->cr.r2y.cr_9_update = 1;
                p_table_index->r2y.cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_9_index);
            }
            if(p_update_flag->cr.r2y.cr_11_update == 0U) {
                p_update_flag->cr.r2y.cr_11_update = 1;
                p_table_index->r2y.cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_11_index);
            }
            if(p_update_flag->cr.r2y.cr_12_update == 0U) {
                p_update_flag->cr.r2y.cr_12_update = 1;
                p_table_index->r2y.cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_12_index);
            }
            if(p_update_flag->cr.r2y.cr_21_update == 0U) {
                p_update_flag->cr.r2y.cr_21_update = 1;
                p_table_index->r2y.cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_21_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
            if(p_update_flag->cr.r2y.cr_47_update == 0U) {
                p_update_flag->cr.r2y.cr_47_update = 1;
                p_table_index->r2y.cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_47_index);
            }
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
        }

        if (p_update_flag->iso.chroma_scale_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_29_update == 0U) {
                p_update_flag->cr.r2y.cr_29_update = 1;
                p_table_index->r2y.cr_29_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_29_index);
            }
        }

        if (p_update_flag->iso.chroma_median_filter_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
            if(p_update_flag->cr.r2y.cr_31_update == 0U) {
                p_update_flag->cr.r2y.cr_31_update = 1;
                p_table_index->r2y.cr_31_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_31_index);
            }
        }

        if ((p_update_flag->iso.first_luma_processing_mode_updated != 0U) || (p_update_flag->iso.advance_spatial_filter_updated != 0U) ||
            (p_update_flag->iso.fstshpns_fir_updated != 0U) || (p_update_flag->iso.fstshpns_both_updated != 0U) ||
            (p_update_flag->iso.fstshpns_noise_updated != 0U) || (p_update_flag->iso.fstshpns_coring_updated != 0U) ||
            (p_update_flag->iso.fstshpns_coring_index_scale_updated != 0U) || (p_update_flag->iso.fstshpns_min_coring_result_updated != 0U) ||
            (p_update_flag->iso.fstshpns_max_coring_result_updated != 0U) || (p_update_flag->iso.fstshpns_scale_coring_updated != 0U)) {
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
        }

        if ((p_update_flag->iso.video_mctf_updated!=0U) || (p_update_flag->iso.internal_video_mctf_updated!=0U) ||
            (p_update_flag->iso.video_mctf_ta_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_both_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_noise_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_fir_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_coring_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_coring_index_scale_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_min_coring_result_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_max_coring_result_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_scale_coring_updated!=0U) ||
            (p_update_flag->iso.fnlshpns_both_tdt_updated!=0U) ||
            (p_update_flag->iso.video_mctf_and_final_sharpen_updated!=0U)) {
            // TBD...
            if(p_update_flag->cr.r2y.cr_111_update == 0U) {
                p_update_flag->cr.r2y.cr_111_update = 1;
                p_table_index->r2y.cr_111_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_111_index);
            }
            if(p_update_flag->cr.r2y.cr_112_update == 0U) {
                p_update_flag->cr.r2y.cr_112_update = 1;
                p_table_index->r2y.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_112_index);
            }
            if(p_update_flag->cr.r2y.cr_113_update == 0U) {
                p_update_flag->cr.r2y.cr_113_update = 1;
                p_table_index->r2y.cr_113_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_113_index);
            }
            if(p_update_flag->cr.r2y.cr_114_update == 0U) {
                p_update_flag->cr.r2y.cr_114_update = 1;
                p_table_index->r2y.cr_114_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_114_index);
            }
            if(p_update_flag->cr.r2y.cr_115_update == 0U) {
                p_update_flag->cr.r2y.cr_115_update = 1;
                p_table_index->r2y.cr_115_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_115_index);
            }
            if(p_update_flag->cr.r2y.cr_116_update == 0U) {
                p_update_flag->cr.r2y.cr_116_update = 1;
                p_table_index->r2y.cr_116_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_116_index);
            }
            if(p_update_flag->cr.r2y.cr_117_update == 0U) {
                p_update_flag->cr.r2y.cr_117_update = 1;
                p_table_index->r2y.cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_117_index);
            }
            if(p_update_flag->cr.r2y.cr_118_update == 0U) {
                p_update_flag->cr.r2y.cr_118_update = 1;
                p_table_index->r2y.cr_118_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_118_index);
            }
            if(p_update_flag->cr.r2y.cr_119_update == 0U) {
                p_update_flag->cr.r2y.cr_119_update = 1;
                p_table_index->r2y.cr_119_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_119_index);
            }
            if(p_update_flag->cr.r2y.cr_120_update == 0U) {
                p_update_flag->cr.r2y.cr_120_update = 1;
                p_table_index->r2y.cr_120_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_120_index);
            }
            if(p_update_flag->cr.r2y.cr_121_update == 0U) {
                p_update_flag->cr.r2y.cr_121_update = 1;
                p_table_index->r2y.cr_121_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_121_index);
            }
        }

        if (p_update_flag->iso.window_size_info_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_111_update == 0U) {
                p_update_flag->cr.r2y.cr_111_update = 1;
                p_table_index->r2y.cr_111_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_111_index);
            }
            if(p_update_flag->cr.r2y.cr_112_update == 0U) {
                p_update_flag->cr.r2y.cr_112_update = 1;
                p_table_index->r2y.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_112_index);
            }
            if(p_update_flag->cr.r2y.cr_113_update == 0U) {
                p_update_flag->cr.r2y.cr_113_update = 1;
                p_table_index->r2y.cr_113_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_113_index);
            }
            if(p_update_flag->cr.r2y.cr_116_update == 0U) {
                p_update_flag->cr.r2y.cr_116_update = 1;
                p_table_index->r2y.cr_116_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_116_index);
            }
            if(p_update_flag->cr.r2y.cr_117_update == 0U) {
                p_update_flag->cr.r2y.cr_117_update = 1;
                p_table_index->r2y.cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_117_index);
            }
        }

        if ((p_update_flag->iso.chroma_filter_updated != 0U) || (p_update_flag->iso.wide_chroma_filter != 0U) || (p_update_flag->iso.wide_chroma_filter_combine != 0U)) {
            if(p_update_flag->cr.warp_2nd == 0U) {
                p_update_flag->cr.warp_2nd = 1;
                p_table_index->warp_2nd = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp_2nd);
            }
            if(p_update_flag->cr.r2y.cr_36_update == 0U) {
                p_update_flag->cr.r2y.cr_36_update = 1;
                p_table_index->r2y.cr_36_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_36_index);
            }
            if(p_update_flag->cr.r2y.cr_37_update == 0U) {
                p_update_flag->cr.r2y.cr_37_update = 1;
                p_table_index->r2y.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_37_index);
            }
            if(p_update_flag->cr.r2y.cr_35_update == 0U) {
                p_update_flag->cr.r2y.cr_35_update = 1;
                p_table_index->r2y.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_35_index);
            }
            if(p_update_flag->cr.r2y.cr_42_update == 0U) {
                p_update_flag->cr.r2y.cr_42_update = 1;
                p_table_index->r2y.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_42_index);
            }
            if(p_update_flag->cr.r2y.cr_33_update == 0U) {
                p_update_flag->cr.r2y.cr_33_update = 1;
                p_table_index->r2y.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_33_index);
            }
            if(p_update_flag->cr.r2y.cr_100_update == 0U) {
                p_update_flag->cr.r2y.cr_100_update = 1;
                p_table_index->r2y.cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_100_index);
            }
            if(p_update_flag->cr.r2y.cr_101_update == 0U) {
                p_update_flag->cr.r2y.cr_101_update = 1;
                p_table_index->r2y.cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_101_index);
            }
        }

        if (p_update_flag->iso.hdr_raw_info_updated != 0U) {
            // no CR, update flow_info...
        }

        if ((p_update_flag->iso.exp0_fe_static_blc_updated != 0U) || (p_update_flag->iso.exp1_fe_static_blc_updated != 0U) || (p_update_flag->iso.exp2_fe_static_blc_updated != 0U)) {
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
            if(p_update_flag->cr.r2y.cr_6_update == 0U) {
                p_update_flag->cr.r2y.cr_6_update = 1;
                p_table_index->r2y.cr_6_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_6_index);
            }
            if(p_update_flag->cr.r2y.cr_7_update == 0U) {
                p_update_flag->cr.r2y.cr_7_update = 1;
                p_table_index->r2y.cr_7_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_7_index);
            }
            if(p_update_flag->cr.r2y.cr_8_update == 0U) {
                p_update_flag->cr.r2y.cr_8_update = 1;
                p_table_index->r2y.cr_8_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_8_index);
            }
        }

        if ((p_update_flag->iso.exp0_fe_wb_gain_updated != 0U) || (p_update_flag->iso.exp1_fe_wb_gain_updated != 0U) || (p_update_flag->iso.exp2_fe_wb_gain_updated != 0U)) {
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
        }

        if ((p_update_flag->iso.ce_updated != 0U) || (p_update_flag->iso.ce_input_table_updated != 0U) || (p_update_flag->iso.ce_out_table_updated != 0U)) {
            if(p_update_flag->cr.r2y.cr_13_update == 0U) {
                p_update_flag->cr.r2y.cr_13_update = 1;
                p_table_index->r2y.cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_13_index);
            }
            if(p_update_flag->cr.r2y.cr_14_update == 0U) {
                p_update_flag->cr.r2y.cr_14_update = 1;
                p_table_index->r2y.cr_14_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_14_index);
            }
        }

        if (p_update_flag->iso.ce_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_49_update == 0U) {
                p_update_flag->cr.r2y.cr_49_update = 1;
                p_table_index->r2y.cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_49_index);
            }
            if(p_update_flag->cr.r2y.cr_50_update == 0U) {
                p_update_flag->cr.r2y.cr_50_update = 1;
                p_table_index->r2y.cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_50_index);
            }
            if(p_update_flag->cr.r2y.cr_51_update == 0U) {
                p_update_flag->cr.r2y.cr_51_update = 1;
                p_table_index->r2y.cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_51_index);
            }
        }

        if (p_update_flag->iso.ce_input_table_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_51_update == 0U) {
                p_update_flag->cr.r2y.cr_51_update = 1;
                p_table_index->r2y.cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_51_index);
            }
            if(p_update_flag->cr.r2y.cr_52_update == 0U) {
                p_update_flag->cr.r2y.cr_52_update = 1;
                p_table_index->r2y.cr_52_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_52_index);
            }
        }

        if (p_update_flag->iso.hdr_blend_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
        }

        if (p_update_flag->iso.fe_tone_curve_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_4_update == 0U) {
                p_update_flag->cr.r2y.cr_4_update = 1;
                p_table_index->r2y.cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_4_index);
            }
            if(p_update_flag->cr.r2y.cr_5_update == 0U) {
                p_update_flag->cr.r2y.cr_5_update = 1;
                p_table_index->r2y.cr_5_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_5_index);
            }
            if(p_update_flag->cr.r2y.cr_45_update == 0U) {
                p_update_flag->cr.r2y.cr_45_update = 1;
                p_table_index->r2y.cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_45_index);
            }
            if(p_update_flag->cr.r2y.cr_46_update == 0U) {
                p_update_flag->cr.r2y.cr_46_update = 1;
                p_table_index->r2y.cr_46_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_46_index);
            }
        }

        if (p_update_flag->iso.resample_str_update != 0U) {
            //TBD...
            if(p_update_flag->cr.r2y.cr_16_update == 0U) {
                p_update_flag->cr.r2y.cr_16_update = 1;
                p_table_index->r2y.cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_16_index);
            }
            if(p_update_flag->cr.r2y.cr_33_update == 0U) {
                p_update_flag->cr.r2y.cr_33_update = 1;
                p_table_index->r2y.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_33_index);
            }
            if(p_update_flag->cr.r2y.cr_35_update == 0U) {
                p_update_flag->cr.r2y.cr_35_update = 1;
                p_table_index->r2y.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_35_index);
            }
            if(p_update_flag->cr.r2y.cr_37_update == 0U) {
                p_update_flag->cr.r2y.cr_37_update = 1;
                p_table_index->r2y.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_37_index);
            }
            if(p_update_flag->cr.r2y.cr_42_update == 0U) {
                p_update_flag->cr.r2y.cr_42_update = 1;
                p_table_index->r2y.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_42_index);
            }
            if(p_update_flag->cr.r2y.cr_100_update == 0U) {
                p_update_flag->cr.r2y.cr_100_update = 1;
                p_table_index->r2y.cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_100_index);
            }
            if(p_update_flag->cr.r2y.cr_101_update == 0U) {
                p_update_flag->cr.r2y.cr_101_update = 1;
                p_table_index->r2y.cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_101_index);
            }
        }
        // handle for frame info
        if (p_update_flag->iso.hdr_raw_info_updated != 0U) {
            if(p_update_flag->cr.frame_info == 0U) {
                p_update_flag->cr.frame_info = 1;
                p_table_index->frame_info = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->frame_info);
            }
        }
        if (p_update_flag->iso.lnl_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_30_update == 0U) {
                p_update_flag->cr.r2y.cr_30_update = 1;
                p_table_index->r2y.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_30_index);
            }
        }
        if (p_update_flag->iso.ext_raw_out_mode_updated != 0U) {
            if(p_update_flag->cr.r2y.cr_15_update == 0U) {
                p_update_flag->cr.r2y.cr_15_update = 1;
                p_table_index->r2y.cr_15_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_15_index);
            }
        }
    }

    return rval;
}

static uint32 exe_motion_me1_get_next_cr_index(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if ((p_update_flag == NULL) || (p_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        if ((p_update_flag->iso.window_size_info_updated != 0U) ||
            (p_update_flag->iso.dmy_range_updated != 0U) ||
            (p_update_flag->iso.dzoom_info_updated != 0U) ||
            (p_update_flag->iso.vin_active_win_updated != 0U) ||
            (p_update_flag->iso.stitching_info_updated != 0U) ||
            (p_update_flag->iso.overlap_x_updated != 0U) ||
            (p_update_flag->iso.flip_mode_updated != 0U) ||
            (p_update_flag->iso.warp_enable_updated != 0U) ||
            (p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            window_calculate_updated = 1U;
        }

        if (window_calculate_updated == 1U) {
            if(p_update_flag->cr.warp == 0U) {
                p_update_flag->cr.warp = 1;
                p_table_index->warp = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp);
            }
            if(p_update_flag->cr.motion_me1.cr_30_update == 0U) {
                p_update_flag->cr.motion_me1.cr_30_update = 1;
                p_table_index->motion_me1.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_30_index);
            }
            if(p_update_flag->cr.motion_me1.cr_33_update == 0U) {
                p_update_flag->cr.motion_me1.cr_33_update = 1;
                p_table_index->motion_me1.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_33_index);
            }
            if(p_update_flag->cr.motion_me1.cr_35_update == 0U) {
                p_update_flag->cr.motion_me1.cr_35_update = 1;
                p_table_index->motion_me1.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_35_index);
            }
            if(p_update_flag->cr.motion_me1.cr_37_update == 0U) {
                p_update_flag->cr.motion_me1.cr_37_update = 1;
                p_table_index->motion_me1.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_37_index);
            }
            if(p_update_flag->cr.motion_me1.cr_42_update == 0U) {
                p_update_flag->cr.motion_me1.cr_42_update = 1;
                p_table_index->motion_me1.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_42_index);
            }
            if(p_update_flag->cr.motion_me1.cr_43_update == 0U) {
                p_update_flag->cr.motion_me1.cr_43_update = 1;
                p_table_index->motion_me1.cr_43_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->r2y.cr_43_index);
            }
            if(p_update_flag->cr.motion_me1.cr_44_update == 0U) {
                p_update_flag->cr.motion_me1.cr_44_update = 1;
                p_table_index->motion_me1.cr_44_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_44_index);
            }
            if(p_update_flag->cr.motion_me1.cr_112_update == 0U) {
                p_update_flag->cr.motion_me1.cr_112_update = 1;
                p_table_index->motion_me1.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_112_index);
            }
            if(p_update_flag->cr.motion_me1.cr_117_update == 0U) {
                p_update_flag->cr.motion_me1.cr_117_update = 1;
                p_table_index->motion_me1.cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_117_index);
            }
        }
        if ((p_update_flag->iso.calib_warp_info_updated!=0U)||(p_update_flag->iso.warp_internal_updated!=0U)||(p_update_flag->iso.chroma_filter_updated!=0U)) {
            if(p_update_flag->cr.warp == 0U) {
                p_update_flag->cr.warp = 1;
                p_table_index->warp = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->warp);
            }
            if(p_update_flag->cr.motion_me1.cr_42_update == 0U) {
                p_update_flag->cr.motion_me1.cr_42_update = 1;
                p_table_index->motion_me1.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_42_index);
            }
            if(p_update_flag->cr.motion_me1.cr_43_update == 0U) {
                p_update_flag->cr.motion_me1.cr_43_update = 1;
                p_table_index->motion_me1.cr_43_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_43_index);
            }
            if(p_update_flag->cr.motion_me1.cr_44_update == 0U) {
                p_update_flag->cr.motion_me1.cr_44_update = 1;
                p_table_index->motion_me1.cr_44_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_44_index);
            }
        }
        if ((p_update_flag->iso.chroma_filter_updated != 0U) || (p_update_flag->iso.wide_chroma_filter != 0U) || (p_update_flag->iso.wide_chroma_filter_combine != 0U)) {
            if(p_update_flag->cr.motion_me1.cr_33_update == 0U) {
                p_update_flag->cr.motion_me1.cr_33_update = 1;
                p_table_index->motion_me1.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_33_index);
            }
            if(p_update_flag->cr.motion_me1.cr_35_update == 0U) {
                p_update_flag->cr.motion_me1.cr_35_update = 1;
                p_table_index->motion_me1.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_35_index);
            }
            if(p_update_flag->cr.motion_me1.cr_37_update == 0U) {
                p_update_flag->cr.motion_me1.cr_37_update = 1;
                p_table_index->motion_me1.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_37_index);
            }
            if(p_update_flag->cr.motion_me1.cr_42_update == 0U) {
                p_update_flag->cr.motion_me1.cr_42_update = 1;
                p_table_index->motion_me1.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_42_index);
            }
        }
        if (p_update_flag->iso.resample_str_update != 0U) {
            if(p_update_flag->cr.motion_me1.cr_33_update == 0U) {
                p_update_flag->cr.motion_me1.cr_33_update = 1;
                p_table_index->motion_me1.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_33_index);
            }
            if(p_update_flag->cr.motion_me1.cr_35_update == 0U) {
                p_update_flag->cr.motion_me1.cr_35_update = 1;
                p_table_index->motion_me1.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_35_index);
            }
            if(p_update_flag->cr.motion_me1.cr_37_update == 0U) {
                p_update_flag->cr.motion_me1.cr_37_update = 1;
                p_table_index->motion_me1.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_37_index);
            }
            if(p_update_flag->cr.motion_me1.cr_42_update == 0U) {
                p_update_flag->cr.motion_me1.cr_42_update = 1;
                p_table_index->motion_me1.cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_me1.cr_42_index);
            }
        }
    }

    return rval;
}

static uint32 exe_motion_a_get_next_cr_index(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if ((p_update_flag == NULL) || (p_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        if ((p_update_flag->iso.window_size_info_updated != 0U) ||
            (p_update_flag->iso.dmy_range_updated != 0U) ||
            (p_update_flag->iso.dzoom_info_updated != 0U) ||
            (p_update_flag->iso.vin_active_win_updated != 0U) ||
            (p_update_flag->iso.stitching_info_updated != 0U) ||
            (p_update_flag->iso.overlap_x_updated != 0U) ||
            (p_update_flag->iso.flip_mode_updated != 0U) ||
            (p_update_flag->iso.warp_enable_updated != 0U) ||
            (p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            window_calculate_updated = 1U;
        }

        if ((p_update_flag->iso.motion_detect_updated != 0U) ||
            (p_update_flag->iso.motion_detect_pos_dep_updated != 0U)) {
            if(p_update_flag->cr.motion_a.cr_30_update == 0U) {
                p_update_flag->cr.motion_a.cr_30_update = 1;
                p_table_index->motion_a.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_30_index);
            }
            if(p_update_flag->cr.motion_a.cr_112_update == 0U) {
                p_update_flag->cr.motion_a.cr_112_update = 1;
                p_table_index->motion_a.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_112_index);
            }
            if(p_update_flag->cr.motion_a.cr_114_update == 0U) {
                p_update_flag->cr.motion_a.cr_114_update = 1;
                p_table_index->motion_a.cr_114_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_114_index);
            }
        }
        if (p_update_flag->iso.chroma_filter_updated != 0U) {
            if(p_update_flag->cr.motion_a.cr_33_update == 0U) {
                p_update_flag->cr.motion_a.cr_33_update = 1;
                p_table_index->motion_a.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_33_index);
            }
            if(p_update_flag->cr.motion_a.cr_35_update == 0U) {
                p_update_flag->cr.motion_a.cr_35_update = 1;
                p_table_index->motion_a.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_35_index);
            }
            if(p_update_flag->cr.motion_a.cr_37_update == 0U) {
                p_update_flag->cr.motion_a.cr_37_update = 1;
                p_table_index->motion_a.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_37_index);
            }
        }
        if (p_update_flag->iso.resample_str_update != 0U) {
            if(p_update_flag->cr.motion_a.cr_33_update == 0U) {
                p_update_flag->cr.motion_a.cr_33_update = 1;
                p_table_index->motion_a.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_33_index);
            }
            if(p_update_flag->cr.motion_a.cr_35_update == 0U) {
                p_update_flag->cr.motion_a.cr_35_update = 1;
                p_table_index->motion_a.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_35_index);
            }
            if(p_update_flag->cr.motion_a.cr_37_update == 0U) {
                p_update_flag->cr.motion_a.cr_37_update = 1;
                p_table_index->motion_a.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_37_index);
            }
        }
        if (window_calculate_updated == 1U) {
            if(p_update_flag->cr.motion_a.cr_30_update == 0U) {
                p_update_flag->cr.motion_a.cr_30_update = 1;
                p_table_index->motion_a.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_30_index);
            }
            if(p_update_flag->cr.motion_a.cr_33_update == 0U) {
                p_update_flag->cr.motion_a.cr_33_update = 1;
                p_table_index->motion_a.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_33_index);
            }
            if(p_update_flag->cr.motion_a.cr_35_update == 0U) {
                p_update_flag->cr.motion_a.cr_35_update = 1;
                p_table_index->motion_a.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_35_index);
            }
            if(p_update_flag->cr.motion_a.cr_37_update == 0U) {
                p_update_flag->cr.motion_a.cr_37_update = 1;
                p_table_index->motion_a.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_37_index);
            }
            if(p_update_flag->cr.motion_a.cr_112_update == 0U) {
                p_update_flag->cr.motion_a.cr_112_update = 1;
                p_table_index->motion_a.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_112_index);
            }
            if(p_update_flag->cr.motion_a.cr_117_update == 0U) {
                p_update_flag->cr.motion_a.cr_117_update = 1;
                p_table_index->motion_a.cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_a.cr_117_index);
            }
        }
    }

    return rval;
}

static uint32 exe_motion_b_get_next_cr_index(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if ((p_update_flag == NULL) || (p_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        if ((p_update_flag->iso.window_size_info_updated != 0U) ||
            (p_update_flag->iso.dmy_range_updated != 0U) ||
            (p_update_flag->iso.dzoom_info_updated != 0U) ||
            (p_update_flag->iso.vin_active_win_updated != 0U) ||
            (p_update_flag->iso.stitching_info_updated != 0U) ||
            (p_update_flag->iso.overlap_x_updated != 0U) ||
            (p_update_flag->iso.flip_mode_updated != 0U) ||
            (p_update_flag->iso.warp_enable_updated != 0U) ||
            (p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            window_calculate_updated = 1U;
        }

        if ((p_update_flag->iso.motion_detect_updated != 0U) ||
            (p_update_flag->iso.motion_detect_pos_dep_updated != 0U)) {
            if(p_update_flag->cr.motion_b.cr_30_update == 0U) {
                p_update_flag->cr.motion_b.cr_30_update = 1;
                p_table_index->motion_b.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_30_index);
            }
            if(p_update_flag->cr.motion_b.cr_112_update == 0U) {
                p_update_flag->cr.motion_b.cr_112_update = 1;
                p_table_index->motion_b.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_112_index);
            }
            if(p_update_flag->cr.motion_b.cr_114_update == 0U) {
                p_update_flag->cr.motion_b.cr_114_update = 1;
                p_table_index->motion_b.cr_114_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_114_index);
            }
        }
        if (p_update_flag->iso.chroma_filter_updated != 0U) {
            if(p_update_flag->cr.motion_b.cr_33_update == 0U) {
                p_update_flag->cr.motion_b.cr_33_update = 1;
                p_table_index->motion_b.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_33_index);
            }
            if(p_update_flag->cr.motion_b.cr_35_update == 0U) {
                p_update_flag->cr.motion_b.cr_35_update = 1;
                p_table_index->motion_b.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_35_index);
            }
            if(p_update_flag->cr.motion_b.cr_37_update == 0U) {
                p_update_flag->cr.motion_b.cr_37_update = 1;
                p_table_index->motion_b.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_37_index);
            }
        }
        if (p_update_flag->iso.resample_str_update != 0U) {
            if(p_update_flag->cr.motion_b.cr_33_update == 0U) {
                p_update_flag->cr.motion_b.cr_33_update = 1;
                p_table_index->motion_b.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_33_index);
            }
            if(p_update_flag->cr.motion_b.cr_35_update == 0U) {
                p_update_flag->cr.motion_b.cr_35_update = 1;
                p_table_index->motion_b.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_35_index);
            }
            if(p_update_flag->cr.motion_b.cr_37_update == 0U) {
                p_update_flag->cr.motion_b.cr_37_update = 1;
                p_table_index->motion_b.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_37_index);
            }
        }
        if (window_calculate_updated == 1U) {
            if(p_update_flag->cr.motion_b.cr_30_update == 0U) {
                p_update_flag->cr.motion_b.cr_30_update = 1;
                p_table_index->motion_b.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_30_index);
            }
            if(p_update_flag->cr.motion_b.cr_33_update == 0U) {
                p_update_flag->cr.motion_b.cr_33_update = 1;
                p_table_index->motion_b.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_33_index);
            }
            if(p_update_flag->cr.motion_b.cr_35_update == 0U) {
                p_update_flag->cr.motion_b.cr_35_update = 1;
                p_table_index->motion_b.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_35_index);
            }
            if(p_update_flag->cr.motion_b.cr_37_update == 0U) {
                p_update_flag->cr.motion_b.cr_37_update = 1;
                p_table_index->motion_b.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_37_index);
            }
            if(p_update_flag->cr.motion_b.cr_112_update == 0U) {
                p_update_flag->cr.motion_b.cr_112_update = 1;
                p_table_index->motion_b.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_112_index);
            }
            if(p_update_flag->cr.motion_b.cr_117_update == 0U) {
                p_update_flag->cr.motion_b.cr_117_update = 1;
                p_table_index->motion_b.cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_b.cr_117_index);
            }
        }
    }
    return rval;
}

static uint32 exe_motion_c_get_next_cr_index(uint32 buffer_number,
        amba_ik_filter_update_flags_t *p_update_flag,
        amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    uint32 window_calculate_updated = 0U;

    if ((p_update_flag == NULL) || (p_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        if ((p_update_flag->iso.window_size_info_updated != 0U) ||
            (p_update_flag->iso.dmy_range_updated != 0U) ||
            (p_update_flag->iso.dzoom_info_updated != 0U) ||
            (p_update_flag->iso.vin_active_win_updated != 0U) ||
            (p_update_flag->iso.stitching_info_updated != 0U) ||
            (p_update_flag->iso.overlap_x_updated != 0U) ||
            (p_update_flag->iso.flip_mode_updated != 0U) ||
            (p_update_flag->iso.warp_enable_updated != 0U) ||
            (p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            window_calculate_updated = 1U;
        }

        if ((p_update_flag->iso.motion_detect_updated != 0U) ||
            (p_update_flag->iso.motion_detect_pos_dep_updated != 0U)) {
            if(p_update_flag->cr.motion_c.cr_30_update == 0U) {
                p_update_flag->cr.motion_c.cr_30_update = 1;
                p_table_index->motion_c.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_30_index);
            }
            if(p_update_flag->cr.motion_c.cr_112_update == 0U) {
                p_update_flag->cr.motion_c.cr_112_update = 1;
                p_table_index->motion_c.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_112_index);
            }
            if(p_update_flag->cr.motion_c.cr_114_update == 0U) {
                p_update_flag->cr.motion_c.cr_114_update = 1;
                p_table_index->motion_c.cr_114_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_114_index);
            }
        }
        if (p_update_flag->iso.chroma_filter_updated != 0U) {
            if(p_update_flag->cr.motion_c.cr_33_update == 0U) {
                p_update_flag->cr.motion_c.cr_33_update = 1;
                p_table_index->motion_c.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_33_index);
            }
            if(p_update_flag->cr.motion_c.cr_35_update == 0U) {
                p_update_flag->cr.motion_c.cr_35_update = 1;
                p_table_index->motion_c.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_35_index);
            }
            if(p_update_flag->cr.motion_c.cr_37_update == 0U) {
                p_update_flag->cr.motion_c.cr_37_update = 1;
                p_table_index->motion_c.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_37_index);
            }
        }
        if (p_update_flag->iso.resample_str_update != 0U) {
            if(p_update_flag->cr.motion_c.cr_33_update == 0U) {
                p_update_flag->cr.motion_c.cr_33_update = 1;
                p_table_index->motion_c.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_33_index);
            }
            if(p_update_flag->cr.motion_c.cr_35_update == 0U) {
                p_update_flag->cr.motion_c.cr_35_update = 1;
                p_table_index->motion_c.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_35_index);
            }
            if(p_update_flag->cr.motion_c.cr_37_update == 0U) {
                p_update_flag->cr.motion_c.cr_37_update = 1;
                p_table_index->motion_c.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_37_index);
            }
        }
        if (window_calculate_updated == 1U) {
            if(p_update_flag->cr.motion_c.cr_30_update == 0U) {
                p_update_flag->cr.motion_c.cr_30_update = 1;
                p_table_index->motion_c.cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_30_index);
            }
            if(p_update_flag->cr.motion_c.cr_33_update == 0U) {
                p_update_flag->cr.motion_c.cr_33_update = 1;
                p_table_index->motion_c.cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_33_index);
            }
            if(p_update_flag->cr.motion_c.cr_35_update == 0U) {
                p_update_flag->cr.motion_c.cr_35_update = 1;
                p_table_index->motion_c.cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_35_index);
            }
            if(p_update_flag->cr.motion_c.cr_37_update == 0U) {
                p_update_flag->cr.motion_c.cr_37_update = 1;
                p_table_index->motion_c.cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_37_index);
            }
            if(p_update_flag->cr.motion_c.cr_112_update == 0U) {
                p_update_flag->cr.motion_c.cr_112_update = 1;
                p_table_index->motion_c.cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_112_index);
            }
            if(p_update_flag->cr.motion_c.cr_117_update == 0U) {
                p_update_flag->cr.motion_c.cr_117_update = 1;
                p_table_index->motion_c.cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->motion_c.cr_117_index);
            }
        }
    }

    return rval;
}
#if SUPPORT_FUSION
static uint32 exe_mono1_8_get_next_cr_index(uint32 buffer_number,
                                        amba_ik_filter_update_flags_t *p_update_flag,
                                        amba_ik_cr_index_t *p_table_index,
                                        uint32 pass)
{
    uint32 rval = IK_OK;

    uint32 window_calculate_updated = 0U;
    amba_ik_r2y_cr_id_list_t *p_r2y_cr_id;
    amba_ik_r2y_cr_index_t *p_r2y_cr_index;

    if (pass == 1u) {
        p_r2y_cr_id = &p_update_flag->cr.mono1;
        p_r2y_cr_index = &p_table_index->mono1;
    } else if (pass == 8u) {
        p_r2y_cr_id = &p_update_flag->cr.mono8;
        p_r2y_cr_index = &p_table_index->mono8;
    } else {
        p_r2y_cr_id = NULL;
        p_r2y_cr_index = NULL;
    }

    if ((p_update_flag == NULL) || (p_table_index == NULL) || (p_r2y_cr_id == NULL)) {
        rval = IK_ERR_0005;
    } else {
        if ((p_update_flag->iso.window_size_info_updated != 0U) ||
            (p_update_flag->iso.cfa_window_size_info_updated != 0U) ||
            (p_update_flag->iso.dmy_range_updated != 0U) ||
            (p_update_flag->iso.dzoom_info_updated != 0U) ||
            (p_update_flag->iso.vin_active_win_updated != 0U) ||
            (p_update_flag->iso.stitching_info_updated != 0U) ||
            (p_update_flag->iso.overlap_x_updated != 0U) ||
            (p_update_flag->iso.flip_mode_updated != 0U) ||
            (p_update_flag->iso.warp_enable_updated != 0U) ||
            (p_update_flag->iso.warp_enable_2nd_updated != 0U)) {
            window_calculate_updated = 1U;
        }

        if (window_calculate_updated != 0U) {
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }
            if(p_r2y_cr_id->cr_9_update == 0U) {
                p_r2y_cr_id->cr_9_update = 1;
                p_r2y_cr_index->cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_9_index);
            }
            if(p_r2y_cr_id->cr_10_update == 0U) {
                p_r2y_cr_id->cr_10_update = 1;
                p_r2y_cr_index->cr_10_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_10_index);
            }
            if(p_r2y_cr_id->cr_11_update == 0U) {
                p_r2y_cr_id->cr_11_update = 1;
                p_r2y_cr_index->cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_11_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_13_update == 0U) {
                p_r2y_cr_id->cr_13_update = 1;
                p_r2y_cr_index->cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_13_index);
            }
            if(p_r2y_cr_id->cr_16_update == 0U) {
                p_r2y_cr_id->cr_16_update = 1;
                p_r2y_cr_index->cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_16_index);
            }
            if(p_r2y_cr_id->cr_17_update == 0U) {
                p_r2y_cr_id->cr_17_update = 1;
                p_r2y_cr_index->cr_17_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_17_index);
            }
            if(p_r2y_cr_id->cr_21_update == 0U) {
                p_r2y_cr_id->cr_21_update = 1;
                p_r2y_cr_index->cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_21_index);
            }
            if(p_r2y_cr_id->cr_28_update == 0U) {
                p_r2y_cr_id->cr_28_update = 1;
                p_r2y_cr_index->cr_28_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_28_index);
            }
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
            if(p_r2y_cr_id->cr_33_update == 0U) {
                p_r2y_cr_id->cr_33_update = 1;
                p_r2y_cr_index->cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_33_index);
            }
            if(p_r2y_cr_id->cr_34_update == 0U) {
                p_r2y_cr_id->cr_34_update = 1;
                p_r2y_cr_index->cr_34_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_34_index);
            }
            if(p_r2y_cr_id->cr_35_update == 0U) {
                p_r2y_cr_id->cr_35_update = 1;
                p_r2y_cr_index->cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_35_index);
            }
            if(p_r2y_cr_id->cr_36_update == 0U) {
                p_r2y_cr_id->cr_36_update = 1;
                p_r2y_cr_index->cr_36_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_36_index);
            }
            if(p_r2y_cr_id->cr_37_update == 0U) {
                p_r2y_cr_id->cr_37_update = 1;
                p_r2y_cr_index->cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_37_index);
            }
            if(p_r2y_cr_id->cr_42_update == 0U) {
                p_r2y_cr_id->cr_42_update = 1;
                p_r2y_cr_index->cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_42_index);
            }
            if(p_r2y_cr_id->cr_43_update == 0U) {
                p_r2y_cr_id->cr_43_update = 1;
                p_r2y_cr_index->cr_43_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_43_index);
            }
            if(p_r2y_cr_id->cr_44_update == 0U) {
                p_r2y_cr_id->cr_44_update = 1;
                p_r2y_cr_index->cr_44_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_44_index);
            }
            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
            if(p_r2y_cr_id->cr_47_update == 0U) {
                p_r2y_cr_id->cr_47_update = 1;
                p_r2y_cr_index->cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_47_index);
            }
            if(p_r2y_cr_id->cr_48_update == 0U) {
                p_r2y_cr_id->cr_48_update = 1;
                p_r2y_cr_index->cr_48_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_48_index);
            }
            if(p_r2y_cr_id->cr_49_update == 0U) {
                p_r2y_cr_id->cr_49_update = 1;
                p_r2y_cr_index->cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_49_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
            if(p_r2y_cr_id->cr_51_update == 0U) {
                p_r2y_cr_id->cr_51_update = 1;
                p_r2y_cr_index->cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_51_index);
            }
            if(p_r2y_cr_id->cr_100_update == 0U) {
                p_r2y_cr_id->cr_100_update = 1;
                p_r2y_cr_index->cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_100_index);
            }
            if(p_r2y_cr_id->cr_101_update == 0U) {
                p_r2y_cr_id->cr_101_update = 1;
                p_r2y_cr_index->cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_101_index);
            }
        }
        if (p_update_flag->iso.use_y2y_enable_info_updated != 0U) {
            if(p_r2y_cr_id->cr_26_update == 0U) {
                p_r2y_cr_id->cr_26_update = 1;
                p_r2y_cr_index->cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_26_index);
            }
            if(p_r2y_cr_id->cr_27_update == 0U) {
                p_r2y_cr_id->cr_27_update = 1;
                p_r2y_cr_index->cr_27_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_27_index);
            }
        }

        if (p_update_flag->iso.hdr_eis_shift_info_updated != 0U) {
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }
            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
            if(p_update_flag->cr.extra_window == 0U) {
                p_update_flag->cr.extra_window = 1;
                p_table_index->extra_window = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->extra_window);
            }
        }

        if (p_update_flag->iso.sensor_information_updated != 0U) {
            if(p_r2y_cr_id->cr_13_update == 0U) {
                p_r2y_cr_id->cr_13_update = 1;
                p_r2y_cr_index->cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_13_index);
            }
            if(p_r2y_cr_id->cr_51_update == 0U) {
                p_r2y_cr_id->cr_51_update = 1;
                p_r2y_cr_index->cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_51_index);
            }
            if(p_r2y_cr_id->cr_24_update == 0U) {
                p_r2y_cr_id->cr_24_update = 1;
                p_r2y_cr_index->cr_24_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_24_index);
            }
            if(p_r2y_cr_id->cr_23_update == 0U) {
                p_r2y_cr_id->cr_23_update = 1;
                p_r2y_cr_index->cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_23_index);
            }
            if(p_r2y_cr_id->cr_22_update == 0U) {
                p_r2y_cr_id->cr_22_update = 1;
                p_r2y_cr_index->cr_22_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_22_index);
            }
            if(p_r2y_cr_id->cr_21_update == 0U) {
                p_r2y_cr_id->cr_21_update = 1;
                p_r2y_cr_index->cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_21_index);
            }
            if(p_r2y_cr_id->cr_16_update == 0U) {
                p_r2y_cr_id->cr_16_update = 1;
                p_r2y_cr_index->cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_16_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_11_update == 0U) {
                p_r2y_cr_id->cr_11_update = 1;
                p_r2y_cr_index->cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_11_index);
            }
            if(p_r2y_cr_id->cr_9_update == 0U) {
                p_r2y_cr_id->cr_9_update = 1;
                p_r2y_cr_index->cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_9_index);
            }
            if(p_r2y_cr_id->cr_6_update == 0U) {
                p_r2y_cr_id->cr_6_update = 1;
                p_r2y_cr_index->cr_6_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_6_index);
            }
            if(p_r2y_cr_id->cr_7_update == 0U) {
                p_r2y_cr_id->cr_7_update = 1;
                p_r2y_cr_index->cr_7_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_7_index);
            }
            if(p_r2y_cr_id->cr_8_update == 0U) {
                p_r2y_cr_id->cr_8_update = 1;
                p_r2y_cr_index->cr_8_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_8_index);
            }
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }

            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
            if(p_r2y_cr_id->cr_47_update == 0U) {
                p_r2y_cr_id->cr_47_update = 1;
                p_r2y_cr_index->cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_47_index);
            }
            if(p_r2y_cr_id->cr_49_update == 0U) {
                p_r2y_cr_id->cr_49_update = 1;
                p_r2y_cr_index->cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_49_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
        }

        if (p_update_flag->iso.mono_before_ce_wb_gain_updated != 0U) {
            if(p_r2y_cr_id->cr_13_update == 0U) {
                p_r2y_cr_id->cr_13_update = 1;
                p_r2y_cr_index->cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_13_index);
            }
            if(p_r2y_cr_id->cr_51_update == 0U) {
                p_r2y_cr_id->cr_51_update = 1;
                p_r2y_cr_index->cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_51_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
        }

        if (p_update_flag->iso.mono_after_ce_wb_gain_updated != 0U) {
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
            if(p_r2y_cr_id->cr_22_update == 0U) {
                p_r2y_cr_id->cr_22_update = 1;
                p_r2y_cr_index->cr_22_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_22_index);
            }
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
        }

        if (p_update_flag->iso.mono_cfa_leakage_filter_updated != 0U) {
            if(p_r2y_cr_id->cr_49_update == 0U) {
                p_r2y_cr_id->cr_49_update = 1;
                p_r2y_cr_index->cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_49_index);
            }
            if(p_r2y_cr_id->cr_23_update == 0U) {
                p_r2y_cr_id->cr_23_update = 1;
                p_r2y_cr_index->cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_23_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
        }

        if (p_update_flag->iso.mono_anti_aliasing_updated != 0U) {
            if(p_r2y_cr_id->cr_23_update == 0U) {
                p_r2y_cr_id->cr_23_update = 1;
                p_r2y_cr_index->cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_23_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
        }

        if (p_update_flag->iso.mono_dynamic_bad_pixel_corr_updated != 0U) {
            if(p_r2y_cr_id->cr_11_update == 0U) {
                p_r2y_cr_id->cr_11_update = 1;
                p_r2y_cr_index->cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_11_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
        }

        if (p_update_flag->iso.mono_grgb_mismatch_updated != 0U) {
            if(p_r2y_cr_id->cr_23_update == 0U) {
                p_r2y_cr_id->cr_23_update = 1;
                p_r2y_cr_index->cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_23_index);
            }
        }

        if (p_update_flag->iso.mono_cfa_noise_filter_updated != 0U) {
            if(p_r2y_cr_id->cr_23_update == 0U) {
                p_r2y_cr_id->cr_23_update = 1;
                p_r2y_cr_index->cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_23_index);
            }
            if(p_r2y_cr_id->cr_11_update == 0U) {
                p_r2y_cr_id->cr_11_update = 1;
                p_r2y_cr_index->cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_11_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_49_update == 0U) {
                p_r2y_cr_id->cr_49_update = 1;
                p_r2y_cr_index->cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_49_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
        }

        if (p_update_flag->iso.mono_demosaic_updated != 0U) {
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_23_update == 0U) {
                p_r2y_cr_id->cr_23_update = 1;
                p_r2y_cr_index->cr_23_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_23_index);
            }
            if(p_r2y_cr_id->cr_24_update == 0U) {
                p_r2y_cr_id->cr_24_update = 1;
                p_r2y_cr_index->cr_24_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_24_index);
            }
        }

        if (p_update_flag->iso.mono_color_correction_updated != 0U) {
            if(p_r2y_cr_id->cr_26_update == 0U) {
                p_r2y_cr_id->cr_26_update = 1;
                p_r2y_cr_index->cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_26_index);
            }
            if(p_r2y_cr_id->cr_27_update == 0U) {
                p_r2y_cr_id->cr_27_update = 1;
                p_r2y_cr_index->cr_27_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_27_index);
            }
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
        }

        if (p_update_flag->iso.pre_cc_gain_updated != 0U) {
            if(p_r2y_cr_id->cr_26_update == 0U) {
                p_r2y_cr_id->cr_26_update = 1;
                p_r2y_cr_index->cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_26_index);
            }
        }

        if (p_update_flag->iso.mono_tone_curve_updated != 0U) {
            if(p_r2y_cr_id->cr_26_update == 0U) {
                p_r2y_cr_id->cr_26_update = 1;
                p_r2y_cr_index->cr_26_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_26_index);
            }
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
        }

        if (p_update_flag->iso.rgb_to_12y_updated != 0U) {
            if(p_r2y_cr_id->cr_25_update == 0U) {
                p_r2y_cr_id->cr_25_update = 1;
                p_r2y_cr_index->cr_25_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_25_index);
            }
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
        }

        if (p_update_flag->iso.mono_rgb_to_yuv_matrix_updated != 0U) {
            if(p_r2y_cr_id->cr_29_update == 0U) {
                p_r2y_cr_id->cr_29_update = 1;
                p_r2y_cr_index->cr_29_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_29_index);
            }
        }

        if (p_update_flag->iso.rgb_ir_updated != 0U) {
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }
            if(p_r2y_cr_id->cr_6_update == 0U) {
                p_r2y_cr_id->cr_6_update = 1;
                p_r2y_cr_index->cr_6_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_6_index);
            }
            if(p_r2y_cr_id->cr_7_update == 0U) {
                p_r2y_cr_id->cr_7_update = 1;
                p_r2y_cr_index->cr_7_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_7_index);
            }
            if(p_r2y_cr_id->cr_8_update == 0U) {
                p_r2y_cr_id->cr_8_update = 1;
                p_r2y_cr_index->cr_8_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_8_index);
            }
            if(p_r2y_cr_id->cr_9_update == 0U) {
                p_r2y_cr_id->cr_9_update = 1;
                p_r2y_cr_index->cr_9_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_9_index);
            }
            if(p_r2y_cr_id->cr_11_update == 0U) {
                p_r2y_cr_id->cr_11_update = 1;
                p_r2y_cr_index->cr_11_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_11_index);
            }
            if(p_r2y_cr_id->cr_12_update == 0U) {
                p_r2y_cr_id->cr_12_update = 1;
                p_r2y_cr_index->cr_12_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_12_index);
            }
            if(p_r2y_cr_id->cr_21_update == 0U) {
                p_r2y_cr_id->cr_21_update = 1;
                p_r2y_cr_index->cr_21_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_21_index);
            }
            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
            if(p_r2y_cr_id->cr_47_update == 0U) {
                p_r2y_cr_id->cr_47_update = 1;
                p_r2y_cr_index->cr_47_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_47_index);
            }
            if(p_r2y_cr_id->cr_49_update == 0U) {
                p_r2y_cr_id->cr_49_update = 1;
                p_r2y_cr_index->cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_49_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
        }

        if (p_update_flag->iso.chroma_scale_updated != 0U) {
            if(p_r2y_cr_id->cr_29_update == 0U) {
                p_r2y_cr_id->cr_29_update = 1;
                p_r2y_cr_index->cr_29_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_29_index);
            }
        }

        if (p_update_flag->iso.chroma_median_filter_updated != 0U) {
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
            if(p_r2y_cr_id->cr_31_update == 0U) {
                p_r2y_cr_id->cr_31_update = 1;
                p_r2y_cr_index->cr_31_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_31_index);
            }
        }

        if ((p_update_flag->iso.mono_fst_luma_process_mode_updated != 0U) || (p_update_flag->iso.mono_adv_spatial_filter_updated != 0U) ||
            (p_update_flag->iso.mono_fst_shp_fir_updated != 0U) || (p_update_flag->iso.mono_fst_shp_both_updated != 0U) ||
            (p_update_flag->iso.mono_fst_shp_noise_updated != 0U) || (p_update_flag->iso.mono_fst_shp_coring_updated != 0U) ||
            (p_update_flag->iso.mono_fst_shp_coring_idx_scale_updated != 0U) || (p_update_flag->iso.mono_fst_shp_min_coring_rslt_updated != 0U) ||
            (p_update_flag->iso.mono_fst_shp_max_coring_rslt_updated != 0U) || (p_update_flag->iso.mono_fst_shp_scale_coring_updated != 0U)) {
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
        }

        if (p_update_flag->iso.window_size_info_updated != 0U) {
            if(p_r2y_cr_id->cr_111_update == 0U) {
                p_r2y_cr_id->cr_111_update = 1;
                p_r2y_cr_index->cr_111_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_111_index);
            }
            if(p_r2y_cr_id->cr_112_update == 0U) {
                p_r2y_cr_id->cr_112_update = 1;
                p_r2y_cr_index->cr_112_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_112_index);
            }
            if(p_r2y_cr_id->cr_113_update == 0U) {
                p_r2y_cr_id->cr_113_update = 1;
                p_r2y_cr_index->cr_113_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_113_index);
            }
            if(p_r2y_cr_id->cr_116_update == 0U) {
                p_r2y_cr_id->cr_116_update = 1;
                p_r2y_cr_index->cr_116_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_116_index);
            }
            if(p_r2y_cr_id->cr_117_update == 0U) {
                p_r2y_cr_id->cr_117_update = 1;
                p_r2y_cr_index->cr_117_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_117_index);
            }
        }

        if ((p_update_flag->iso.chroma_filter_updated != 0U) || (p_update_flag->iso.wide_chroma_filter != 0U) || (p_update_flag->iso.wide_chroma_filter_combine != 0U)) {
            if(p_r2y_cr_id->cr_36_update == 0U) {
                p_r2y_cr_id->cr_36_update = 1;
                p_r2y_cr_index->cr_36_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_36_index);
            }
            if(p_r2y_cr_id->cr_37_update == 0U) {
                p_r2y_cr_id->cr_37_update = 1;
                p_r2y_cr_index->cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_37_index);
            }
            if(p_r2y_cr_id->cr_35_update == 0U) {
                p_r2y_cr_id->cr_35_update = 1;
                p_r2y_cr_index->cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_35_index);
            }
            if(p_r2y_cr_id->cr_42_update == 0U) {
                p_r2y_cr_id->cr_42_update = 1;
                p_r2y_cr_index->cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_42_index);
            }
            if(p_r2y_cr_id->cr_33_update == 0U) {
                p_r2y_cr_id->cr_33_update = 1;
                p_r2y_cr_index->cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_33_index);
            }
            if(p_r2y_cr_id->cr_100_update == 0U) {
                p_r2y_cr_id->cr_100_update = 1;
                p_r2y_cr_index->cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_100_index);
            }
            if(p_r2y_cr_id->cr_101_update == 0U) {
                p_r2y_cr_id->cr_101_update = 1;
                p_r2y_cr_index->cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_101_index);
            }
        }

        if (p_update_flag->iso.mono_exp0_fe_static_blc_level_updated != 0U) {
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }
            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
            if(p_r2y_cr_id->cr_6_update == 0U) {
                p_r2y_cr_id->cr_6_update = 1;
                p_r2y_cr_index->cr_6_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_6_index);
            }
            if(p_r2y_cr_id->cr_7_update == 0U) {
                p_r2y_cr_id->cr_7_update = 1;
                p_r2y_cr_index->cr_7_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_7_index);
            }
            if(p_r2y_cr_id->cr_8_update == 0U) {
                p_r2y_cr_id->cr_8_update = 1;
                p_r2y_cr_index->cr_8_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_8_index);
            }
        }

        if (p_update_flag->iso.mono_exp0_fe_wb_gain_updated != 0U) {
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }
            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
        }

        if ((p_update_flag->iso.mono_ce_updated != 0U) || (p_update_flag->iso.mono_ce_input_table_updated != 0U) || (p_update_flag->iso.mono_ce_output_table_updated != 0U)) {
            if(p_r2y_cr_id->cr_13_update == 0U) {
                p_r2y_cr_id->cr_13_update = 1;
                p_r2y_cr_index->cr_13_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_13_index);
            }
            if(p_r2y_cr_id->cr_14_update == 0U) {
                p_r2y_cr_id->cr_14_update = 1;
                p_r2y_cr_index->cr_14_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_14_index);
            }
        }

        if (p_update_flag->iso.mono_ce_updated != 0U) {
            if(p_r2y_cr_id->cr_49_update == 0U) {
                p_r2y_cr_id->cr_49_update = 1;
                p_r2y_cr_index->cr_49_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_49_index);
            }
            if(p_r2y_cr_id->cr_50_update == 0U) {
                p_r2y_cr_id->cr_50_update = 1;
                p_r2y_cr_index->cr_50_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_50_index);
            }
            if(p_r2y_cr_id->cr_51_update == 0U) {
                p_r2y_cr_id->cr_51_update = 1;
                p_r2y_cr_index->cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_51_index);
            }
        }

        if (p_update_flag->iso.mono_ce_input_table_updated != 0U) {
            if(p_r2y_cr_id->cr_51_update == 0U) {
                p_r2y_cr_id->cr_51_update = 1;
                p_r2y_cr_index->cr_51_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_51_index);
            }
            if(p_r2y_cr_id->cr_52_update == 0U) {
                p_r2y_cr_id->cr_52_update = 1;
                p_r2y_cr_index->cr_52_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_52_index);
            }
        }

        if (p_update_flag->iso.fe_tone_curve_updated != 0U) {
            if(p_r2y_cr_id->cr_4_update == 0U) {
                p_r2y_cr_id->cr_4_update = 1;
                p_r2y_cr_index->cr_4_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_4_index);
            }
            if(p_r2y_cr_id->cr_5_update == 0U) {
                p_r2y_cr_id->cr_5_update = 1;
                p_r2y_cr_index->cr_5_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_5_index);
            }
            if(p_r2y_cr_id->cr_45_update == 0U) {
                p_r2y_cr_id->cr_45_update = 1;
                p_r2y_cr_index->cr_45_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_45_index);
            }
            if(p_r2y_cr_id->cr_46_update == 0U) {
                p_r2y_cr_id->cr_46_update = 1;
                p_r2y_cr_index->cr_46_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_46_index);
            }
        }

        if (p_update_flag->iso.resample_str_update != 0U) {
            //TBD...
            if(p_r2y_cr_id->cr_16_update == 0U) {
                p_r2y_cr_id->cr_16_update = 1;
                p_r2y_cr_index->cr_16_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_16_index);
            }
            if(p_r2y_cr_id->cr_33_update == 0U) {
                p_r2y_cr_id->cr_33_update = 1;
                p_r2y_cr_index->cr_33_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_33_index);
            }
            if(p_r2y_cr_id->cr_35_update == 0U) {
                p_r2y_cr_id->cr_35_update = 1;
                p_r2y_cr_index->cr_35_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_35_index);
            }
            if(p_r2y_cr_id->cr_37_update == 0U) {
                p_r2y_cr_id->cr_37_update = 1;
                p_r2y_cr_index->cr_37_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_37_index);
            }
            if(p_r2y_cr_id->cr_42_update == 0U) {
                p_r2y_cr_id->cr_42_update = 1;
                p_r2y_cr_index->cr_42_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_42_index);
            }
            if(p_r2y_cr_id->cr_100_update == 0U) {
                p_r2y_cr_id->cr_100_update = 1;
                p_r2y_cr_index->cr_100_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_100_index);
            }
            if(p_r2y_cr_id->cr_101_update == 0U) {
                p_r2y_cr_id->cr_101_update = 1;
                p_r2y_cr_index->cr_101_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_101_index);
            }
        }
        // handle for frame info
        if (p_update_flag->iso.hdr_raw_info_updated != 0U) {
            if(p_update_flag->cr.frame_info == 0U) {
                p_update_flag->cr.frame_info = 1;
                p_table_index->frame_info = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_table_index->frame_info);
            }
        }
        if (p_update_flag->iso.lnl_updated != 0U) {
            if(p_r2y_cr_id->cr_30_update == 0U) {
                p_r2y_cr_id->cr_30_update = 1;
                p_r2y_cr_index->cr_30_index = (uint8)exe_get_next_ring_buffer_index(buffer_number, p_r2y_cr_index->cr_30_index);
            }
        }
    }

    return rval;
}
#endif
static uint32 exe_get_next_cr_index(uint32 buffer_number,
                                    amba_ik_filter_update_flags_t *p_update_flag,
                                    amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;

    // step0
    rval |= exe_r2y_get_next_cr_index(buffer_number, p_update_flag, p_table_index);
    // step1
    rval |= exe_motion_me1_get_next_cr_index(buffer_number, p_update_flag, p_table_index);
    // step2
    rval |= exe_motion_a_get_next_cr_index(buffer_number, p_update_flag, p_table_index);
    // step3
    rval |= exe_motion_b_get_next_cr_index(buffer_number, p_update_flag, p_table_index);
    // step4
    rval |= exe_motion_c_get_next_cr_index(buffer_number, p_update_flag, p_table_index);
#if SUPPORT_FUSION
    // step5
    rval |= exe_mono1_8_get_next_cr_index(buffer_number, p_update_flag, p_table_index, 1u);
#endif
    return rval;
}

static void exe_r2y_check_next_cr_index(uint32 video_pipe, amba_ik_cr_id_list_t *p_update_flag, amba_ik_cr_index_t *p_table_index)
{
    if((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
       (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        p_update_flag->r2y.cr_45_update = 0;
        p_update_flag->r2y.cr_46_update = 0;
        p_update_flag->r2y.cr_47_update = 0;
        p_update_flag->r2y.cr_48_update = 0;
        p_update_flag->r2y.cr_49_update = 0;
        p_update_flag->r2y.cr_50_update = 0;
        p_update_flag->r2y.cr_51_update = 0;
        p_update_flag->r2y.cr_52_update = 0;

        p_table_index->r2y.cr_45_index = 0;
        p_table_index->r2y.cr_46_index = 0;
        p_table_index->r2y.cr_47_index = 0;
        p_table_index->r2y.cr_48_index = 0;
        p_table_index->r2y.cr_49_index = 0;
        p_table_index->r2y.cr_50_index = 0;
        p_table_index->r2y.cr_51_index = 0;
        p_table_index->r2y.cr_52_index = 0;
    }
}

static uint32 exe_check_next_cr_index(uint32 context_id, amba_ik_cr_id_list_t *p_update_flag, amba_ik_cr_index_t *p_table_index)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        if(p_ctx->organization.attribute.ability.pipe == AMBA_IK_VIDEO_PIPE) {
            // step0
            exe_r2y_check_next_cr_index(p_ctx->organization.attribute.ability.video_pipe, p_update_flag, p_table_index);
            // step1/2/3/4
        }
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0000;
    }

    return rval;
}

static uint32 exe_get_next_r2y_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_r2y_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 42:
        p_tmp_u8 = p_flow_tables->CR_buf_42;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 43:
        p_tmp_u8 = p_flow_tables->CR_buf_43;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 44:
        p_tmp_u8 = p_flow_tables->CR_buf_44;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 45:
        p_tmp_u8 = p_flow_tables->CR_buf_45;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 46:
        p_tmp_u8 = p_flow_tables->CR_buf_46;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 47:
        p_tmp_u8 = p_flow_tables->CR_buf_47;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 48:
        p_tmp_u8 = p_flow_tables->CR_buf_48;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 49:
        p_tmp_u8 = p_flow_tables->CR_buf_49;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 50:
        p_tmp_u8 = p_flow_tables->CR_buf_50;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 51:
        p_tmp_u8 = p_flow_tables->CR_buf_51;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 52:
        p_tmp_u8 = p_flow_tables->CR_buf_52;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;

    case 100:
        p_tmp_u8 = p_flow_tables->CR_buf_100;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 101:
        p_tmp_u8 = p_flow_tables->CR_buf_101;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 102:
        p_tmp_u8 = p_flow_tables->CR_buf_102;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 103:
        p_tmp_u8 = p_flow_tables->CR_buf_103;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;

    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_motion_me1_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_motion_me1_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 42:
        p_tmp_u8 = p_flow_tables->CR_buf_42;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 43:
        p_tmp_u8 = p_flow_tables->CR_buf_43;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 44:
        p_tmp_u8 = p_flow_tables->CR_buf_44;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_motion_a_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_motion_a_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_motion_b_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_motion_b_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_motion_c_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_motion_c_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}
#if SUPPORT_FUSION
static uint32 exe_get_next_mono1_8_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_r2y_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 42:
        p_tmp_u8 = p_flow_tables->CR_buf_42;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 43:
        p_tmp_u8 = p_flow_tables->CR_buf_43;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 44:
        p_tmp_u8 = p_flow_tables->CR_buf_44;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 45:
        p_tmp_u8 = p_flow_tables->CR_buf_45;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 46:
        p_tmp_u8 = p_flow_tables->CR_buf_46;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 47:
        p_tmp_u8 = p_flow_tables->CR_buf_47;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 48:
        p_tmp_u8 = p_flow_tables->CR_buf_48;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 49:
        p_tmp_u8 = p_flow_tables->CR_buf_49;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 50:
        p_tmp_u8 = p_flow_tables->CR_buf_50;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 51:
        p_tmp_u8 = p_flow_tables->CR_buf_51;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 52:
        p_tmp_u8 = p_flow_tables->CR_buf_52;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;

    case 100:
        p_tmp_u8 = p_flow_tables->CR_buf_100;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 101:
        p_tmp_u8 = p_flow_tables->CR_buf_101;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 102:
        p_tmp_u8 = p_flow_tables->CR_buf_102;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 103:
        p_tmp_u8 = p_flow_tables->CR_buf_103;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;

    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_mono3_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_mono3_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_mono4_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_mono4_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_mono5_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_mono5_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_mono6_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_mono6_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}

static uint32 exe_get_next_mono7_flow_tbl(uint32 cr_id, uint32 index, void **p_addr, const amba_ik_mono7_flow_tables_t *p_flow_tables)
{
    uint32 rval = IK_OK;
    const uint8 *p_tmp_u8;
    void* tmp_ptr;

    if (index==0U) {
        // fix compile error
    } else {
        // default
    }

    switch (cr_id) {
    case 4:
        p_tmp_u8 = p_flow_tables->CR_buf_4;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 5:
        p_tmp_u8 = p_flow_tables->CR_buf_5;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 6:
        p_tmp_u8 = p_flow_tables->CR_buf_6;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 7:
        p_tmp_u8 = p_flow_tables->CR_buf_7;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 8:
        p_tmp_u8 = p_flow_tables->CR_buf_8;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 9:
        p_tmp_u8 = p_flow_tables->CR_buf_9;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 10:
        p_tmp_u8 = p_flow_tables->CR_buf_10;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 11:
        p_tmp_u8 = p_flow_tables->CR_buf_11;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 12:
        p_tmp_u8 = p_flow_tables->CR_buf_12;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 13:
        p_tmp_u8 = p_flow_tables->CR_buf_13;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 14:
        p_tmp_u8 = p_flow_tables->CR_buf_14;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 15:
        p_tmp_u8 = p_flow_tables->CR_buf_15;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 16:
        p_tmp_u8 = p_flow_tables->CR_buf_16;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 17:
        p_tmp_u8 = p_flow_tables->CR_buf_17;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 18:
        p_tmp_u8 = p_flow_tables->CR_buf_18;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 19:
        p_tmp_u8 = p_flow_tables->CR_buf_19;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 20:
        p_tmp_u8 = p_flow_tables->CR_buf_20;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 21:
        p_tmp_u8 = p_flow_tables->CR_buf_21;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 22:
        p_tmp_u8 = p_flow_tables->CR_buf_22;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 23:
        p_tmp_u8 = p_flow_tables->CR_buf_23;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 24:
        p_tmp_u8 = p_flow_tables->CR_buf_24;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 25:
        p_tmp_u8 = p_flow_tables->CR_buf_25;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 26:
        p_tmp_u8 = p_flow_tables->CR_buf_26;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 27:
        p_tmp_u8 = p_flow_tables->CR_buf_27;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 28:
        p_tmp_u8 = p_flow_tables->CR_buf_28;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 29:
        p_tmp_u8 = p_flow_tables->CR_buf_29;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 30:
        p_tmp_u8 = p_flow_tables->CR_buf_30;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 31:
        p_tmp_u8 = p_flow_tables->CR_buf_31;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 32:
        p_tmp_u8 = p_flow_tables->CR_buf_32;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 33:
        p_tmp_u8 = p_flow_tables->CR_buf_33;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 34:
        p_tmp_u8 = p_flow_tables->CR_buf_34;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 35:
        p_tmp_u8 = p_flow_tables->CR_buf_35;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 36:
        p_tmp_u8 = p_flow_tables->CR_buf_36;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 37:
        p_tmp_u8 = p_flow_tables->CR_buf_37;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 38:
        p_tmp_u8 = p_flow_tables->CR_buf_38;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 39:
        p_tmp_u8 = p_flow_tables->CR_buf_39;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 40:
        p_tmp_u8 = p_flow_tables->CR_buf_40;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 41:
        p_tmp_u8 = p_flow_tables->CR_buf_41;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 111:
        p_tmp_u8 = p_flow_tables->CR_buf_111;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 112:
        p_tmp_u8 = p_flow_tables->CR_buf_112;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 113:
        p_tmp_u8 = p_flow_tables->CR_buf_113;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 114:
        p_tmp_u8 = p_flow_tables->CR_buf_114;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 115:
        p_tmp_u8 = p_flow_tables->CR_buf_115;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 116:
        p_tmp_u8 = p_flow_tables->CR_buf_116;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 117:
        p_tmp_u8 = p_flow_tables->CR_buf_117;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 118:
        p_tmp_u8 = p_flow_tables->CR_buf_118;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 119:
        p_tmp_u8 = p_flow_tables->CR_buf_119;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 120:
        p_tmp_u8 = p_flow_tables->CR_buf_120;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    case 121:
        p_tmp_u8 = p_flow_tables->CR_buf_121;
        (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_u8, sizeof(void*));
        *p_addr = tmp_ptr;
        break;
    default:
        // TBD
        amba_ik_system_print_uint32_5("[IK] Non-access cr_id = %d", cr_id, DC_U, DC_U, DC_U, DC_U);
        break;
    }

    return rval;
}
#endif
static uint32 exe_get_next_flow_tbl(uint32 context_id, uint32 step_id, uint32 cr_id, uint32 index, void **p_addr)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    const amba_ik_flow_tables_t *p_flow_tbl = NULL;
    const amba_ik_motion_fusion_flow_tables_t *p_motion_fusion_flow_tbl = NULL;
    const int16 *p_tmp_s16;
    void* tmp_ptr;
    const idsp_aaa_data_t *p_tmp_aaa;
    const idsp_extra_window_info_t *p_tmp_extra_window = NULL;
    const ik_query_frame_info_t *p_tmp_query_frame_info;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 video_pipe;
    const amba_ik_r2y_flow_tables_t *p_r2y_flow_tables = NULL;
    const int16 *p_ca_warp_hor_red = NULL;
    const int16 *p_ca_warp_hor_blue = NULL;
    const int16 *p_ca_warp_ver_red = NULL;
    const int16 *p_ca_warp_ver_blue = NULL;
    const int16 *p_warp_hor = NULL;
    const int16 *p_warp_ver = NULL;
    const int16 *p_warp_hor_b = NULL;
    const idsp_aaa_data_t *p_aaa = NULL;
    const idsp_extra_window_info_t *p_extra_window = NULL;
    const ik_query_frame_info_t *p_query_frame_info = NULL;

    // get ability
    rval |= img_ctx_get_context(context_id, &p_ctx);
    video_pipe = p_ctx->organization.attribute.ability.video_pipe;

    rval |= img_ctx_get_flow_tbl(context_id, index, &addr);
    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
        (void)amba_ik_system_memcpy(&p_flow_tbl, &addr, sizeof(amba_ik_flow_tables_t *));
        p_r2y_flow_tables = &p_flow_tbl->r2y;
        p_ca_warp_hor_red = p_flow_tbl->ca_warp_hor_red;
        p_ca_warp_hor_blue = p_flow_tbl->ca_warp_hor_blue;
        p_ca_warp_ver_red = p_flow_tbl->ca_warp_ver_red;
        p_ca_warp_ver_blue = p_flow_tbl->ca_warp_ver_blue;
        p_warp_hor = p_flow_tbl->warp_hor;
        p_warp_ver = p_flow_tbl->warp_ver;
        p_warp_hor_b = p_flow_tbl->warp_hor_b;
        p_aaa = p_flow_tbl->aaa[0];
        p_extra_window = &p_flow_tbl->extra_window_info;
        p_query_frame_info = &p_flow_tbl->frame_info;
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
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl, &addr, sizeof(amba_ik_motion_fusion_flow_tables_t *));
        p_r2y_flow_tables = &p_motion_fusion_flow_tbl->r2y;
        p_ca_warp_hor_red = p_motion_fusion_flow_tbl->ca_warp_hor_red;
        p_ca_warp_hor_blue = p_motion_fusion_flow_tbl->ca_warp_hor_blue;
        p_ca_warp_ver_red = p_motion_fusion_flow_tbl->ca_warp_ver_red;
        p_ca_warp_ver_blue = p_motion_fusion_flow_tbl->ca_warp_ver_blue;
        p_warp_hor = p_motion_fusion_flow_tbl->warp_hor;
        p_warp_ver = p_motion_fusion_flow_tbl->warp_ver;
        p_warp_hor_b = p_motion_fusion_flow_tbl->warp_hor_b;
        p_aaa = p_motion_fusion_flow_tbl->aaa[0];
        p_extra_window = &p_motion_fusion_flow_tbl->extra_window_info;
        p_query_frame_info = &p_motion_fusion_flow_tbl->frame_info;
    } else {
        // misra
    }

    if ((cr_id==122U) || (cr_id==123U) || (cr_id==124U) || (cr_id==125U) || (cr_id==126U) || (cr_id==127U) ||
        (cr_id==128U) || (cr_id==129U) || (cr_id==130U) || (cr_id==131U)) {
        switch (cr_id) {
        case 122:
            p_tmp_s16 = p_ca_warp_hor_red;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 123:
            p_tmp_s16 = p_ca_warp_hor_blue;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 124:
            p_tmp_s16 = p_ca_warp_ver_red;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 125:
            p_tmp_s16 = p_ca_warp_ver_blue;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 126:
            p_tmp_s16 = p_warp_hor;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 127:
            p_tmp_s16 = p_warp_ver;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 128:
            p_tmp_aaa = p_aaa;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_aaa, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 129:
            p_tmp_s16 = p_warp_hor_b;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_s16, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 130:
            p_tmp_extra_window = p_extra_window;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_extra_window, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        case 131:
        default: // misraC
            p_tmp_query_frame_info = p_query_frame_info;
            (void)amba_ik_system_memcpy(&tmp_ptr, &p_tmp_query_frame_info, sizeof(void*));
            *p_addr = tmp_ptr;
            break;
        }
    } else if (step_id==1U) {
        (void)exe_get_next_r2y_flow_tbl(cr_id, index, p_addr, p_r2y_flow_tables);
    } else if (step_id==2U) {
        (void)exe_get_next_motion_me1_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->motion_me1);
    } else if (step_id==3U) {
        (void)exe_get_next_motion_a_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->motion_a);
    } else if (step_id==4U) {
        (void)exe_get_next_motion_b_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->motion_b);
    } else if (step_id==5U) {
        (void)exe_get_next_motion_c_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->motion_c);
    } 
#if SUPPORT_FUSION
    else if (step_id==6U) {
        (void)exe_get_next_mono1_8_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono1);
    } else if (step_id==7U) {
        (void)exe_get_next_mono3_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono3);
    } else if (step_id==8U) {
        (void)exe_get_next_mono4_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono4);
    } else if (step_id==9U) {
        (void)exe_get_next_mono5_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono5);
    } else if (step_id==10U) {
        (void)exe_get_next_mono6_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono6);
    } else if (step_id==11U) {
        (void)exe_get_next_mono7_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono7);
    } else if (step_id==12U) {
        (void)exe_get_next_mono1_8_flow_tbl(cr_id, index, p_addr, &p_motion_fusion_flow_tbl->mono8);
    } 
#endif
    else {
        // misraC
    }
    return rval;
}

static uint32 exe_r2y_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_r2y_cr_index_t *p_table_index, const amba_ik_r2y_cr_id_list_t *p_cr_upd_flag, amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 1, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 1, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 1, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_42_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 42, p_table_index->cr_42_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
        p_flow_tbl_list->p_CR_buf_42 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_43_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 43, p_table_index->cr_43_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
        p_flow_tbl_list->p_CR_buf_43 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_44_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 44, p_table_index->cr_44_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
        p_flow_tbl_list->p_CR_buf_44 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_45_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 45, p_table_index->cr_45_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
        p_flow_tbl_list->p_CR_buf_45 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_46_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 46, p_table_index->cr_46_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
        p_flow_tbl_list->p_CR_buf_46 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_47_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 47, p_table_index->cr_47_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
        p_flow_tbl_list->p_CR_buf_47 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_48_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 48, p_table_index->cr_48_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
        p_flow_tbl_list->p_CR_buf_48 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_49_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 49, p_table_index->cr_49_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
        p_flow_tbl_list->p_CR_buf_49 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_50_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 50, p_table_index->cr_50_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
        p_flow_tbl_list->p_CR_buf_50 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_51_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 51, p_table_index->cr_51_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
        p_flow_tbl_list->p_CR_buf_51 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_52_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 52, p_table_index->cr_52_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
        p_flow_tbl_list->p_CR_buf_52 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_100_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 100, p_table_index->cr_100_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_100, CR_SIZE_100);
        p_flow_tbl_list->p_CR_buf_100 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_101_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 101, p_table_index->cr_101_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_101, CR_SIZE_101);
        p_flow_tbl_list->p_CR_buf_101 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_102_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 102, p_table_index->cr_102_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_102, CR_SIZE_102);
        p_flow_tbl_list->p_CR_buf_102 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_103_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 103, p_table_index->cr_103_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_103, CR_SIZE_103);
        p_flow_tbl_list->p_CR_buf_103 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}

static uint32 exe_motion_me1_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_motion_me1_cr_index_t *p_table_index, const amba_ik_motion_me1_cr_id_list_t *p_cr_upd_flag, amba_ik_motion_me1_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 2, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 2, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 2, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 2, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 2, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 2, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 2, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 2, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 2, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_42_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 42, p_table_index->cr_42_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
        p_flow_tbl_list->p_CR_buf_42 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_43_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 43, p_table_index->cr_43_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
        p_flow_tbl_list->p_CR_buf_43 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_44_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 2, 44, p_table_index->cr_44_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
        p_flow_tbl_list->p_CR_buf_44 = p_dst_addr;
    }

    return rval;
}

static uint32 exe_motion_a_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_motion_a_cr_index_t *p_table_index, const amba_ik_motion_a_cr_id_list_t *p_cr_upd_flag, amba_ik_motion_a_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}

static uint32 exe_motion_b_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_motion_b_cr_index_t *p_table_index, const amba_ik_motion_b_cr_id_list_t *p_cr_upd_flag, amba_ik_motion_b_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 4, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 4, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 4, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 4, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 4, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 4, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 4, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 4, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 4, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 4, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}

static uint32 exe_motion_c_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_motion_c_cr_index_t *p_table_index, const amba_ik_motion_c_cr_id_list_t *p_cr_upd_flag, amba_ik_motion_c_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 5, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 5, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 5, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 5, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 5, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 5, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 5, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 5, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 5, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 5, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}
#if SUPPORT_FUSION
static uint32 exe_mono1_8_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_r2y_cr_index_t *p_table_index, const amba_ik_r2y_cr_id_list_t *p_cr_upd_flag, amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 1, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 1, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 1, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_42_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 42, p_table_index->cr_42_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
        p_flow_tbl_list->p_CR_buf_42 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_43_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 43, p_table_index->cr_43_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
        p_flow_tbl_list->p_CR_buf_43 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_44_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 44, p_table_index->cr_44_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
        p_flow_tbl_list->p_CR_buf_44 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_45_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 45, p_table_index->cr_45_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
        p_flow_tbl_list->p_CR_buf_45 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_46_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 46, p_table_index->cr_46_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
        p_flow_tbl_list->p_CR_buf_46 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_47_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 47, p_table_index->cr_47_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
        p_flow_tbl_list->p_CR_buf_47 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_48_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 48, p_table_index->cr_48_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
        p_flow_tbl_list->p_CR_buf_48 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_49_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 49, p_table_index->cr_49_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
        p_flow_tbl_list->p_CR_buf_49 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_50_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 50, p_table_index->cr_50_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
        p_flow_tbl_list->p_CR_buf_50 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_51_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 51, p_table_index->cr_51_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
        p_flow_tbl_list->p_CR_buf_51 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_52_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 52, p_table_index->cr_52_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
        p_flow_tbl_list->p_CR_buf_52 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_100_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 100, p_table_index->cr_100_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_100, CR_SIZE_100);
        p_flow_tbl_list->p_CR_buf_100 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_101_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 101, p_table_index->cr_101_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_101, CR_SIZE_101);
        p_flow_tbl_list->p_CR_buf_101 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_102_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 102, p_table_index->cr_102_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_102, CR_SIZE_102);
        p_flow_tbl_list->p_CR_buf_102 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_103_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 103, p_table_index->cr_103_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_103, CR_SIZE_103);
        p_flow_tbl_list->p_CR_buf_103 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 1, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 1, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}

static uint32 exe_mono3_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_mono3_cr_index_t *p_table_index, const amba_ik_mono3_cr_id_list_t *p_cr_upd_flag, amba_ik_mono3_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}


static uint32 exe_mono4_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_mono4_cr_index_t *p_table_index, const amba_ik_mono4_cr_id_list_t *p_cr_upd_flag, amba_ik_mono4_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    return rval;
}


static uint32 exe_mono5_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_mono5_cr_index_t *p_table_index, const amba_ik_mono5_cr_id_list_t *p_cr_upd_flag, amba_ik_mono5_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    return rval;
}


static uint32 exe_mono6_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_mono6_cr_index_t *p_table_index, const amba_ik_mono6_cr_id_list_t *p_cr_upd_flag, amba_ik_mono6_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}


static uint32 exe_mono7_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_mono7_cr_index_t *p_table_index, const amba_ik_mono7_cr_id_list_t *p_cr_upd_flag, amba_ik_mono7_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    void *p_dst_addr;

    if (flow_idx==0U) {
        // fix compile error
    } else {
        // default
    }

    if(p_cr_upd_flag->cr_4_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 4, p_table_index->cr_4_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
        p_flow_tbl_list->p_CR_buf_4 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_5_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 5, p_table_index->cr_5_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
        p_flow_tbl_list->p_CR_buf_5 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_6_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 6, p_table_index->cr_6_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
        p_flow_tbl_list->p_CR_buf_6 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_7_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 7, p_table_index->cr_7_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
        p_flow_tbl_list->p_CR_buf_7 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_8_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 8, p_table_index->cr_8_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
        p_flow_tbl_list->p_CR_buf_8 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_9_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 9, p_table_index->cr_9_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
        p_flow_tbl_list->p_CR_buf_9 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_10_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 10, p_table_index->cr_10_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
        p_flow_tbl_list->p_CR_buf_10 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_11_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 11, p_table_index->cr_11_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
        p_flow_tbl_list->p_CR_buf_11 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_12_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 12, p_table_index->cr_12_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
        p_flow_tbl_list->p_CR_buf_12 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_13_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 13, p_table_index->cr_13_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
        p_flow_tbl_list->p_CR_buf_13 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_14_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 14, p_table_index->cr_14_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
        p_flow_tbl_list->p_CR_buf_14 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_15_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 15, p_table_index->cr_15_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
        p_flow_tbl_list->p_CR_buf_15 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_16_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 16, p_table_index->cr_16_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
        p_flow_tbl_list->p_CR_buf_16 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_17_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 17, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
        p_flow_tbl_list->p_CR_buf_17 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 18, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
        p_flow_tbl_list->p_CR_buf_18 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 19, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
        p_flow_tbl_list->p_CR_buf_19 = p_dst_addr;

        (void)exe_get_next_flow_tbl(context_id, 3, 20, p_table_index->cr_17_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
        p_flow_tbl_list->p_CR_buf_20 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_21_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 21, p_table_index->cr_21_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
        p_flow_tbl_list->p_CR_buf_21 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_22_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 22, p_table_index->cr_22_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
        p_flow_tbl_list->p_CR_buf_22 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_23_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 23, p_table_index->cr_23_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
        p_flow_tbl_list->p_CR_buf_23 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_24_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 24, p_table_index->cr_24_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
        p_flow_tbl_list->p_CR_buf_24 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_25_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 25, p_table_index->cr_25_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
        p_flow_tbl_list->p_CR_buf_25 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_26_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 26, p_table_index->cr_26_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
        p_flow_tbl_list->p_CR_buf_26 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_27_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 27, p_table_index->cr_27_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
        p_flow_tbl_list->p_CR_buf_27 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_28_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 28, p_table_index->cr_28_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
        p_flow_tbl_list->p_CR_buf_28 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_29_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 29, p_table_index->cr_29_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
        p_flow_tbl_list->p_CR_buf_29 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_30_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 30, p_table_index->cr_30_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
        p_flow_tbl_list->p_CR_buf_30 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_31_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 31, p_table_index->cr_31_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
        p_flow_tbl_list->p_CR_buf_31 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_32_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 32, p_table_index->cr_32_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
        p_flow_tbl_list->p_CR_buf_32 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_33_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 33, p_table_index->cr_33_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
        p_flow_tbl_list->p_CR_buf_33 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_34_update != 0U) { // TBD, I think we don't need copy table, replace it by re-calculation
        (void)exe_get_next_flow_tbl(context_id, 3, 34, p_table_index->cr_34_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
        p_flow_tbl_list->p_CR_buf_34 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_35_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 35, p_table_index->cr_35_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
        p_flow_tbl_list->p_CR_buf_35 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_36_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 36, p_table_index->cr_36_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
        p_flow_tbl_list->p_CR_buf_36 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_37_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 37, p_table_index->cr_37_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
        p_flow_tbl_list->p_CR_buf_37 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_38_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 38, p_table_index->cr_38_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
        p_flow_tbl_list->p_CR_buf_38 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_39_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 39, p_table_index->cr_39_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
        p_flow_tbl_list->p_CR_buf_39 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_40_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 40, p_table_index->cr_40_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
        p_flow_tbl_list->p_CR_buf_40 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_41_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 41, p_table_index->cr_41_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
        p_flow_tbl_list->p_CR_buf_41 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_111_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 111, p_table_index->cr_111_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
        p_flow_tbl_list->p_CR_buf_111 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_112_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 112, p_table_index->cr_112_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
        p_flow_tbl_list->p_CR_buf_112 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_113_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 113, p_table_index->cr_113_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
        p_flow_tbl_list->p_CR_buf_113 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_114_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 114, p_table_index->cr_114_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
        p_flow_tbl_list->p_CR_buf_114 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_115_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 115, p_table_index->cr_115_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
        p_flow_tbl_list->p_CR_buf_115 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_116_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 116, p_table_index->cr_116_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
        p_flow_tbl_list->p_CR_buf_116 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_117_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 117, p_table_index->cr_117_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
        p_flow_tbl_list->p_CR_buf_117 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_118_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 118, p_table_index->cr_118_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
        p_flow_tbl_list->p_CR_buf_118 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_119_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 119, p_table_index->cr_119_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
        p_flow_tbl_list->p_CR_buf_119 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_120_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 120, p_table_index->cr_120_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
        p_flow_tbl_list->p_CR_buf_120 = p_dst_addr;
    }
    if(p_cr_upd_flag->cr_121_update != 0U) {
        (void)exe_get_next_flow_tbl(context_id, 3, 121, p_table_index->cr_121_index, &p_dst_addr);
        (void)amba_ik_system_memcpy(p_dst_addr, p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
        p_flow_tbl_list->p_CR_buf_121 = p_dst_addr;
    }

    return rval;
}
#endif
static uint32 exe_update_flow_tbl_list(uint32 context_id, uint32 flow_idx, const amba_ik_cr_index_t *p_table_index, const amba_ik_cr_id_list_t *p_cr_upd_flag)
{
    uint32 rval = IK_OK;
    uintptr addr = 0;
    uintptr addr_list = 0;
    void *p_dst_addr;
    amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    amba_ik_motion_fusion_flow_tables_list_t *p_motion_fusion_flow_tbl_list = NULL;
    amba_ik_r2y_flow_tables_list_t *p_r2y_flow_tables_list = NULL;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 video_pipe;

    // get ability
    rval |= img_ctx_get_context(context_id, &p_ctx);
    video_pipe = p_ctx->organization.attribute.ability.video_pipe;

    rval |= img_ctx_get_flow_tbl(context_id, flow_idx, &addr);
    rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr_list);
    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
        (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr_list, sizeof(amba_ik_flow_tables_list_t *));
        p_r2y_flow_tables_list = &p_flow_tbl_list->r2y;

        if(p_cr_upd_flag->cawarp != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 122U, p_table_index->cawarp, &p_dst_addr);
            p_flow_tbl_list->p_ca_warp_hor_red = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 123U, p_table_index->cawarp, &p_dst_addr);
            p_flow_tbl_list->p_ca_warp_hor_blue = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 124U, p_table_index->cawarp, &p_dst_addr);
            p_flow_tbl_list->p_ca_warp_ver_red = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 125U, p_table_index->cawarp, &p_dst_addr);
            p_flow_tbl_list->p_ca_warp_ver_blue = p_dst_addr;
        }
        if(p_cr_upd_flag->warp != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 126U, p_table_index->warp, &p_dst_addr);
            p_flow_tbl_list->p_warp_hor = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 127U, p_table_index->warp, &p_dst_addr);
            p_flow_tbl_list->p_warp_ver = p_dst_addr;
        }
        if(p_cr_upd_flag->warp_2nd != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 129U, p_table_index->warp_2nd, &p_dst_addr);
            p_flow_tbl_list->p_warp_hor_b = p_dst_addr;
        }
        if(p_cr_upd_flag->aaa_data != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 128U, p_table_index->aaa_data, &p_dst_addr);
            p_flow_tbl_list->p_aaa = p_dst_addr;
        }
        if(p_cr_upd_flag->extra_window != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 130U, p_table_index->extra_window, &p_dst_addr);
            p_flow_tbl_list->p_extra_window = p_dst_addr;
        }
        if(p_cr_upd_flag->frame_info != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 131U, p_table_index->frame_info, &p_dst_addr);
            p_flow_tbl_list->p_frame_info = p_dst_addr;
        }
        // step1
        rval |= exe_r2y_update_flow_tbl_list(context_id, flow_idx, &p_table_index->r2y, &p_cr_upd_flag->r2y, p_r2y_flow_tables_list);
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
        (void)amba_ik_system_memcpy(&p_motion_fusion_flow_tbl_list, &addr_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t *));
        p_r2y_flow_tables_list = &p_motion_fusion_flow_tbl_list->r2y;

        if(p_cr_upd_flag->cawarp != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 122U, p_table_index->cawarp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_ca_warp_hor_red = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 123U, p_table_index->cawarp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_ca_warp_hor_blue = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 124U, p_table_index->cawarp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_ca_warp_ver_red = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 125U, p_table_index->cawarp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_ca_warp_ver_blue = p_dst_addr;
        }
        if(p_cr_upd_flag->warp != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 126U, p_table_index->warp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_warp_hor = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 127U, p_table_index->warp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_warp_ver = p_dst_addr;

            (void)exe_get_next_flow_tbl(context_id, 1, 129U, p_table_index->warp, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_warp_hor_b = p_dst_addr;
        }
        if(p_cr_upd_flag->aaa_data != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 128U, p_table_index->aaa_data, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_aaa = p_dst_addr;
        }
        if(p_cr_upd_flag->extra_window != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 130U, p_table_index->extra_window, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_extra_window = p_dst_addr;
        }
        if(p_cr_upd_flag->aaa_data != 0U) {
            (void)exe_get_next_flow_tbl(context_id, 1, 131U, p_table_index->frame_info, &p_dst_addr);
            p_motion_fusion_flow_tbl_list->p_frame_info = p_dst_addr;
        }
        // step0
        rval |= exe_r2y_update_flow_tbl_list(context_id, flow_idx, &p_table_index->r2y, &p_cr_upd_flag->r2y, p_r2y_flow_tables_list);
        // step1
        rval |= exe_motion_me1_update_flow_tbl_list(context_id, flow_idx, &p_table_index->motion_me1, &p_cr_upd_flag->motion_me1, &p_motion_fusion_flow_tbl_list->motion_me1);
        // step2
        rval |= exe_motion_a_update_flow_tbl_list(context_id, flow_idx, &p_table_index->motion_a, &p_cr_upd_flag->motion_a, &p_motion_fusion_flow_tbl_list->motion_a);
        // step3
        rval |= exe_motion_b_update_flow_tbl_list(context_id, flow_idx, &p_table_index->motion_b, &p_cr_upd_flag->motion_b, &p_motion_fusion_flow_tbl_list->motion_b);
        // step4
        rval |= exe_motion_c_update_flow_tbl_list(context_id, flow_idx, &p_table_index->motion_c, &p_cr_upd_flag->motion_c, &p_motion_fusion_flow_tbl_list->motion_c);
#if SUPPORT_FUSION
        // step5
        rval |= exe_mono1_8_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono1, &p_cr_upd_flag->mono1, &p_motion_fusion_flow_tbl_list->mono1);
        // step6
        rval |= exe_mono3_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono3, &p_cr_upd_flag->mono3, &p_motion_fusion_flow_tbl_list->mono3);
        // step6
        rval |= exe_mono4_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono4, &p_cr_upd_flag->mono4, &p_motion_fusion_flow_tbl_list->mono4);
        // step6
        rval |= exe_mono5_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono5, &p_cr_upd_flag->mono5, &p_motion_fusion_flow_tbl_list->mono5);
        // step6
        rval |= exe_mono6_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono6, &p_cr_upd_flag->mono6, &p_motion_fusion_flow_tbl_list->mono6);
        // step6
        rval |= exe_mono7_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono7, &p_cr_upd_flag->mono7, &p_motion_fusion_flow_tbl_list->mono7);
        // step11
        rval |= exe_mono1_8_update_flow_tbl_list(context_id, flow_idx, &p_table_index->mono8, &p_cr_upd_flag->mono8, &p_motion_fusion_flow_tbl_list->mono8);
#endif
    } else {
        // misra
    }
    return rval;
}

static INLINE uint32 exe_compute_linear_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    /******************** Calibration ********************/
    rval |= exe_process_calib_cr(p_flow, p_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 0);

    rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_1x_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 1);

    rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 1);

    rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_1x_still_liso_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_list, p_filters);

    /******************** CFA ********************/
    if(p_filters->input_param.num_of_exposures == 0U) {

        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 0);

    } else if(p_filters->input_param.num_of_exposures == 1U) {

        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 1);

        rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 1);

    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] Still LISO did not support this pipe %d\n", p_filters->input_param.num_of_exposures, DC_U, DC_U, DC_U, DC_U);
        rval |= IK_ERR_0000;
    }

    rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);

    rval |= exe_process_mctf_mcts_still_liso_cr(p_flow, p_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_2x_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 2);

    rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 2);

    rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_hdr_3x_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    rval |= exe_process_calib_cr(p_flow, p_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 3);

    rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 3);

    rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_linear_md_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    {// step0
        rval |= exe_process_optc_r2y_default_cr(p_flow, p_list, p_filters);
        /******************** Calibration ********************/
        rval |= exe_process_calib_cr(p_flow, p_list, p_filters);
        /******************** CFA ********************/
        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 0);
        rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);
        /******************** RGB ********************/
        rval |= exe_process_rgb_cr(p_list, p_filters);
        /******************** YUV ********************/
        rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);
        /******************** MD ********************/
        rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);
        // rewrite cr
        rval |= exe_process_optc_r2y_cr(p_flow, p_list, p_filters);
    }
    {// step1
        rval |= exe_process_optc_motion_me1_cr(p_flow, p_list, p_filters);
    }
    {// step2
        rval |= exe_process_optc_motion_a_cr(p_flow, p_list, p_filters);
    }
    {// step3
        rval |= exe_process_optc_motion_b_cr(p_flow, p_list, p_filters);
    }
    {// step4
        rval |= exe_process_optc_motion_c_cr(p_flow, p_list, p_filters);
    }

    return rval;
}

static INLINE uint32 exe_compute_hdr_1x_md_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    {// step0
        rval |= exe_process_optc_r2y_default_cr(p_flow, p_list, p_filters);
        /******************** Calibration ********************/
        rval |= exe_process_calib_cr(p_flow, p_list, p_filters);
        /******************** CFA ********************/
        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 1);
        rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 1);
        rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);
        /******************** RGB ********************/
        rval |= exe_process_rgb_cr(p_list, p_filters);
        /******************** YUV ********************/
        rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);
        /******************** MD ********************/
        rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);
        // rewrite cr
        rval |= exe_process_optc_r2y_cr(p_flow, p_list, p_filters);
    }
    {// step1
        rval |= exe_process_optc_motion_me1_cr(p_flow, p_list, p_filters);
    }
    {// step2
        rval |= exe_process_optc_motion_a_cr(p_flow, p_list, p_filters);
    }
    {// step3
        rval |= exe_process_optc_motion_b_cr(p_flow, p_list, p_filters);
    }
    {// step4
        rval |= exe_process_optc_motion_c_cr(p_flow, p_list, p_filters);
    }

    return rval;
}

static INLINE uint32 exe_compute_hdr_2x_md_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    {// step0
        rval |= exe_process_optc_r2y_default_cr(p_flow, p_list, p_filters);
        /******************** Calibration ********************/
        rval |= exe_process_calib_cr(p_flow, p_list, p_filters);
        /******************** CFA ********************/
        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 2);
        rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 2);
        rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);
        /******************** RGB ********************/
        rval |= exe_process_rgb_cr(p_list, p_filters);
        /******************** YUV ********************/
        rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);
        /******************** MD ********************/
        rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);
        // rewrite cr
        rval |= exe_process_optc_r2y_cr(p_flow, p_list, p_filters);
    }
    {// step1
        rval |= exe_process_optc_motion_me1_cr(p_flow, p_list, p_filters);
    }
    {// step2
        rval |= exe_process_optc_motion_a_cr(p_flow, p_list, p_filters);
    }
    {// step3
        rval |= exe_process_optc_motion_b_cr(p_flow, p_list, p_filters);
    }
    {// step4
        rval |= exe_process_optc_motion_c_cr(p_flow, p_list, p_filters);
    }

    return rval;
}

static INLINE uint32 exe_compute_hdr_3x_md_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    {// step0
        rval |= exe_process_optc_r2y_default_cr(p_flow, p_list, p_filters);
        /******************** Calibration ********************/
        rval |= exe_process_calib_cr(p_flow, p_list, p_filters);
        /******************** CFA ********************/
        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 3);
        rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 3);
        rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);
        /******************** RGB ********************/
        rval |= exe_process_rgb_cr(p_list, p_filters);
        /******************** YUV ********************/
        rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);
        /******************** MD ********************/
        rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);
        // rewrite cr
        rval |= exe_process_optc_r2y_cr(p_flow, p_list, p_filters);
    }
    {// step1
        rval |= exe_process_optc_motion_me1_cr(p_flow, p_list, p_filters);
    }
    {// step2
        rval |= exe_process_optc_motion_a_cr(p_flow, p_list, p_filters);
    }
    {// step3
        rval |= exe_process_optc_motion_b_cr(p_flow, p_list, p_filters);
    }
    {// step4
        rval |= exe_process_optc_motion_c_cr(p_flow, p_list, p_filters);
    }

    return rval;
}
#if SUPPORT_FUSION
static INLINE uint32 exe_compute_fusion(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    {// step0
        rval |= exe_process_optc_r2y_default_cr(p_flow, p_list, p_filters);
        /******************** Calibration ********************/
        rval |= exe_process_calib_cr(p_flow, p_list, p_filters);
        /******************** CFA ********************/
        rval |= exe_process_hdr_ce_cr(p_flow, p_list, p_filters, 1);
        rval |= exe_process_hdr_ce_sub_cr(p_list, p_filters, 1);
        rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);
        /******************** RGB ********************/
        rval |= exe_process_rgb_cr(p_list, p_filters);
        /******************** YUV ********************/
        rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);
        /******************** MD ********************/
        rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);
        // rewrite cr
        rval |= exe_process_optc_r2y_cr(p_flow, p_list, p_filters);
    }
    {// step1
        rval |= exe_process_optc_motion_me1_cr(p_flow, p_list, p_filters);
    }
    {// step2
        rval |= exe_process_optc_motion_a_cr(p_flow, p_list, p_filters);
    }
    {// step3
        rval |= exe_process_optc_motion_b_cr(p_flow, p_list, p_filters);
    }
    {// step4
        rval |= exe_process_optc_motion_c_cr(p_flow, p_list, p_filters);
    }
    {// step5
        rval |= exe_process_mono1_8_cr(p_flow, p_list, p_filters, 1u);
    }
    {// step6
        rval |= exe_process_mono3_cr(p_flow, p_list, p_filters);
    }
    {// step7
        rval |= exe_process_mono4_cr(p_flow, p_list, p_filters);
    }
    {// step8
        rval |= exe_process_mono5_cr(p_flow, p_list, p_filters);
    }
    {// step9
        rval |= exe_process_mono6_cr(p_flow, p_list, p_filters);
    }
    {// step10
        rval |= exe_process_mono7_cr(p_flow, p_list, p_filters);
    }
    {// step11
        rval |= exe_process_mono1_8_cr(p_flow, p_list, p_filters, 8u);
    }

    return rval;
}
#endif
static INLINE uint32 exe_compute_y2y_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;

    /******************** Calibration ********************/
    rval |= exe_process_calib_cr(p_flow, p_list, p_filters);

    /******************** CFA ********************/
    rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);

    /******************** RGB ********************/
    rval |= exe_process_rgb_cr(p_list, p_filters);

    /******************** YUV ********************/
    rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);

    /******************** MD ********************/
    rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);

    return rval;
}

static INLINE uint32 exe_compute_y2y_md_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    {// step0
        rval |= exe_process_optc_r2y_default_cr(p_flow, p_list, p_filters);
        /******************** Calibration ********************/
        rval |= exe_process_calib_cr(p_flow, p_list, p_filters);
        /******************** CFA ********************/
        rval |= exe_process_cfa_cr(p_flow, p_list, p_filters);
        /******************** RGB ********************/
        rval |= exe_process_rgb_cr(p_list, p_filters);
        /******************** YUV ********************/
        rval |= exe_process_yuv_cr(p_flow, p_list, p_filters);
        /******************** MD ********************/
        rval |= exe_process_mctf_mcts_cr(p_flow, p_list, p_filters);
        // rewrite cr
        rval |= exe_process_optc_r2y_cr(p_flow, p_list, p_filters);
    }
    {// step1
        rval |= exe_process_optc_motion_me1_cr(p_flow, p_list, p_filters);
    }
    {// step2
        rval |= exe_process_optc_motion_a_cr(p_flow, p_list, p_filters);
    }
    {// step3
        rval |= exe_process_optc_motion_b_cr(p_flow, p_list, p_filters);
    }
    {// step4
        rval |= exe_process_optc_motion_c_cr(p_flow, p_list, p_filters);
    }

    return rval;
}

#if 0
// temporarily copy canned binary cr to cr buffer.
// todo, remove them.
static void exe_hack_load_step1_cfg_binary(amba_ik_hiso_step1_cr_list_t *p_cr, uint8 *buffer, uint32 num_of_exp)
{
    uint32 offset = 0u;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_4, buffer+offset, MODE_7_SEC_2_CFG_SZ);

    // sec4
    offset += MODE_7_SEC_2_CFG_SZ;
    if(num_of_exp != 0u) {
        (void)amba_ik_system_memcpy(p_cr->sec4.p_CR_buf_45, buffer+offset, MAX_SEC_4_CFG_SZ);
    }

    // sec18
    offset += MAX_SEC_4_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step2_cfg_binary(amba_ik_hiso_step2_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 104320;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_27, buffer+offset+MODE_8_SEC_2_CR_OFFSET_27, CR_SIZE_27);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step3_cfg_binary(amba_ik_hiso_step3_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 168320;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step4_cfg_binary(amba_ik_hiso_step4_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 232320;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step4a_cfg_binary(amba_ik_hiso_step4a_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 296320;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);
}

static void exe_hack_load_step5_cfg_binary(amba_ik_hiso_step5_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 346624;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step6_cfg_binary(amba_ik_hiso_step6_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 410624;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
}

static void exe_hack_load_step7_cfg_binary(amba_ik_hiso_step7_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 474624;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
}

static void exe_hack_load_step8_cfg_binary(amba_ik_hiso_step8_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 538624;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
}

static void exe_hack_load_step9_cfg_binary(amba_ik_hiso_step9_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 602624;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);
}

static void exe_hack_load_step10_cfg_binary(amba_ik_hiso_step10_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 652928;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

    // sec18
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step11_cfg_binary(amba_ik_hiso_step11_cr_list_t *p_cr, uint8 *buffer, uint32 num_of_exp)
{
    uint32 offset = 716928;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_4, buffer+offset, MODE_7_SEC_2_CFG_SZ);

    // sec4
    offset += MODE_7_SEC_2_CFG_SZ;
    if(num_of_exp != 0u) {
        (void)amba_ik_system_memcpy(p_cr->sec4.p_CR_buf_45, buffer+offset, MAX_SEC_4_CFG_SZ);
    }

    // sec18
    offset += MAX_SEC_4_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step12_cfg_binary(amba_ik_hiso_step12_cr_list_t *p_cr, uint8 *buffer, uint32 num_of_exp)
{
    uint32 offset = 821248;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_4, buffer+offset, MODE_7_SEC_2_CFG_SZ);

    // sec4
    offset += MODE_7_SEC_2_CFG_SZ;
    if(num_of_exp != 0u) {
        (void)amba_ik_system_memcpy(p_cr->sec4.p_CR_buf_45, buffer+offset, MAX_SEC_4_CFG_SZ);
    }

    // sec18
    offset += MAX_SEC_4_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_118, buffer+offset+MODE_4_SEC_18_CR_OFFSET_118, CR_SIZE_118);
}

static void exe_hack_load_step13_cfg_binary(amba_ik_hiso_step13_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 925568;

    // sec2
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_26, buffer+offset+MODE_8_SEC_2_CR_OFFSET_26, CR_SIZE_26);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_29, buffer+offset+MODE_8_SEC_2_CR_OFFSET_29, CR_SIZE_29);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_30, buffer+offset+MODE_8_SEC_2_CR_OFFSET_30, CR_SIZE_30);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_31, buffer+offset+MODE_8_SEC_2_CR_OFFSET_31, CR_SIZE_31);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_32, buffer+offset+MODE_8_SEC_2_CR_OFFSET_32, CR_SIZE_32);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_33, buffer+offset+MODE_8_SEC_2_CR_OFFSET_33, CR_SIZE_33);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_35, buffer+offset+MODE_8_SEC_2_CR_OFFSET_35, CR_SIZE_35);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_36, buffer+offset+MODE_8_SEC_2_CR_OFFSET_36, CR_SIZE_36);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_37, buffer+offset+MODE_8_SEC_2_CR_OFFSET_37, CR_SIZE_37);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_38, buffer+offset+MODE_8_SEC_2_CR_OFFSET_38, CR_SIZE_38);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_39, buffer+offset+MODE_8_SEC_2_CR_OFFSET_39, CR_SIZE_39);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_40, buffer+offset+MODE_8_SEC_2_CR_OFFSET_40, CR_SIZE_40);
    (void)amba_ik_system_memcpy(p_cr->sec2.p_CR_buf_41, buffer+offset+MODE_8_SEC_2_CR_OFFSET_41, CR_SIZE_41);

#ifdef _HISO_SEC3_IN_STEP13
    // sec3
    offset += MODE_8_SEC_2_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec3.p_CR_buf_42, buffer+offset, MAX_SEC_3_CFG_SZ);
    ikc_temp_hack_hiso_vwarp(p_cr->sec3.p_CR_buf_42);
#endif

    // sec18
    offset += MAX_SEC_3_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_114, buffer+offset+MODE_4_SEC_18_CR_OFFSET_114, CR_SIZE_114);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
}

static void exe_hack_load_step14_cfg_binary(amba_ik_hiso_step14_cr_list_t *p_cr, uint8 *buffer)
{
    uint32 offset = 989568;

    // sec3
    (void)amba_ik_system_memcpy(p_cr->sec3.p_CR_buf_42, buffer+offset, MAX_SEC_3_CFG_SZ);
    ikc_temp_hack_hiso_vwarp(p_cr->sec3.p_CR_buf_42);

    // sec18
    offset += MAX_SEC_3_CFG_SZ;
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_112, buffer+offset+MODE_4_SEC_18_CR_OFFSET_112, CR_SIZE_112);
    (void)amba_ik_system_memcpy(p_cr->sec18.p_CR_buf_117, buffer+offset+MODE_4_SEC_18_CR_OFFSET_117, CR_SIZE_117);
}
#endif

static INLINE uint32 exe_compute_hiso_cr(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_OK;
    amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list = NULL;

    (void)amba_ik_system_memcpy(&p_flow_tbl_list, &p_list, sizeof(void *));

    rval |= exe_process_temp_hack_hiso_data(p_flow, p_filters);

#if 0
    // temporarily copy canned binary cr to cr buffer.
    // todo, implement the hiso algo.
    if((u_earlytest_mode == 0U) || (u_earlytest_mode == 1U)) {
#include "stdio.h"
        FILE *pFileCR;
        uint8 *buffer = NULL;
        uint32 CrSize, rsize;

        //read default binary
        pFileCR = fopen("../../hiso_step_crs_reduced.bin", "rb" );
        if (pFileCR == NULL) {
            fputs("File error pFileCR, load HISO CR failed.\n", stderr);
            return 1;
        }

        fseek(pFileCR, 0, SEEK_END);
        CrSize = ftell(pFileCR);
        rewind(pFileCR);

        // alloc buf
        buffer = (uint8 *)malloc(CrSize);
        if(buffer == NULL) {
            printf("[ERROR] malloc(%d) fail!\n", CrSize);
            return -1;
        }

        printf("Jack, binary size : %d\n", CrSize);
        printf("Jack, Step 1 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step1_sec2_cr_t));
        printf("Jack, Sec 4 : %ld\n", sizeof(amba_ik_sec4_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step1_sec18_cr_t));

        printf("Jack, Step 2 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step2_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step2_sec18_cr_t));

        printf("Jack, Step 3 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step3_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step3_sec18_cr_t));

        printf("Jack, Step 4 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step4_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step4_sec18_cr_t));

        printf("Jack, Step 4a :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step4a_sec2_cr_t));

        printf("Jack, Step 5 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step5_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step5_sec18_cr_t));

        printf("Jack, Step 6 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step6_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step6_sec18_cr_t));

        printf("Jack, Step 7 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step7_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step7_sec18_cr_t));

        printf("Jack, Step 8 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step8_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step8_sec18_cr_t));

        printf("Jack, Step 9 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step9_sec2_cr_t));

        printf("Jack, Step 10 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step10_sec2_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step10_sec18_cr_t));

        printf("Jack, Step 11 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step11_sec2_cr_t));
        printf("Jack, Sec 4 : %ld\n", sizeof(amba_ik_sec4_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step11_sec18_cr_t));

        printf("Jack, Step 12 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step12_sec2_cr_t));
        printf("Jack, Sec 4 : %ld\n", sizeof(amba_ik_sec4_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step12_sec18_cr_t));

        printf("Jack, Step 13 :\n");
        printf("Jack, Sec 2 : %ld\n", sizeof(amba_ik_step13_sec2_cr_t));
#ifdef _HISO_SEC3_IN_STEP13
        printf("Jack, Sec 3 : %ld\n", sizeof(amba_ik_sec3_cr_t));
#endif
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step13_sec18_cr_t));

#ifndef _HISO_SEC3_IN_STEP13
        printf("Jack, Step 14 :\n");
        printf("Jack, Sec 3 : %ld\n", sizeof(amba_ik_sec3_cr_t));
        printf("Jack, Sec 18 : %ld\n", sizeof(amba_ik_step13_sec18_cr_t));
#endif

        rsize = fread(buffer, 1, CrSize, pFileCR);
        if (rsize != CrSize) {
            fputs("Reading error pSec2Cfg\n", stderr);
            return 3;
        }

        exe_hack_load_step1_cfg_binary(&p_flow_tbl_list->step1, buffer, p_filters->input_param.num_of_exposures);
        exe_hack_load_step2_cfg_binary(&p_flow_tbl_list->step2, buffer);
        exe_hack_load_step3_cfg_binary(&p_flow_tbl_list->step3, buffer);
        exe_hack_load_step4_cfg_binary(&p_flow_tbl_list->step4, buffer);
        exe_hack_load_step4a_cfg_binary(&p_flow_tbl_list->step4a, buffer);
        exe_hack_load_step5_cfg_binary(&p_flow_tbl_list->step5, buffer);
        exe_hack_load_step6_cfg_binary(&p_flow_tbl_list->step6, buffer);
        exe_hack_load_step7_cfg_binary(&p_flow_tbl_list->step7, buffer);
        exe_hack_load_step8_cfg_binary(&p_flow_tbl_list->step8, buffer);
        exe_hack_load_step9_cfg_binary(&p_flow_tbl_list->step9, buffer);
        exe_hack_load_step10_cfg_binary(&p_flow_tbl_list->step10, buffer);
        exe_hack_load_step11_cfg_binary(&p_flow_tbl_list->step11, buffer, p_filters->input_param.num_of_exposures);
        exe_hack_load_step12_cfg_binary(&p_flow_tbl_list->step12, buffer, p_filters->input_param.num_of_exposures);
        exe_hack_load_step13_cfg_binary(&p_flow_tbl_list->step13, buffer);
#ifndef _HISO_SEC3_IN_STEP13
        exe_hack_load_step14_cfg_binary(&p_flow_tbl_list->step14, buffer);
#endif

        fclose(pFileCR);
        free(buffer);
    }
#endif

//    if((u_earlytest_mode == 0U) || (u_earlytest_mode == 2U)) {
        rval |= exe_process_hiso_cr(p_flow, p_flow_tbl_list, p_filters);
//    }
    return rval;
}

static INLINE uint32 exe_cmpt_invalid_cr_utility(idsp_flow_ctrl_t *p_flow, void *p_list, amba_ik_filter_t *p_filters)
{
    uint32 rval = IK_ERR_0000;

    if (p_flow != NULL) {
        (void)amba_ik_system_memcpy(p_flow,p_flow,sizeof(uint8));//misra fix.
    }
    if (p_list != NULL) {
        (void)amba_ik_system_memcpy(p_list, p_list, sizeof(uint8));//misra fix.
    }
    if (p_filters != NULL) {
        (void)amba_ik_system_memcpy(p_filters,p_filters,sizeof(uint8));//misra fix.
    }

    return rval;
}

static INLINE void exe_get_cr_utilities(const ik_ability_t *p_ability, amba_ik_exe_cr_utilities_t *p_utilities)
{
    if (p_ability->pipe == AMBA_IK_VIDEO_PIPE) {
        switch (p_ability->video_pipe) {
        case AMBA_IK_VIDEO_PIPE_LINEAR:
            p_utilities->compute_cr = exe_compute_linear_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_CE:
            p_utilities->compute_cr = exe_compute_hdr_1x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2:
            p_utilities->compute_cr = exe_compute_hdr_2x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3:
            p_utilities->compute_cr = exe_compute_hdr_3x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_MD:
            p_utilities->compute_cr = exe_compute_linear_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD:
            p_utilities->compute_cr = exe_compute_hdr_1x_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD:
            p_utilities->compute_cr = exe_compute_hdr_2x_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD:
            p_utilities->compute_cr = exe_compute_hdr_3x_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_FUSION:
#if SUPPORT_FUSION
            p_utilities->compute_cr = exe_compute_fusion;
#endif
            break;
        case AMBA_IK_VIDEO_PIPE_Y2Y:
            p_utilities->compute_cr = exe_compute_y2y_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_HVH:
            p_utilities->compute_cr = exe_compute_linear_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH:
            p_utilities->compute_cr = exe_compute_hdr_1x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH:
            p_utilities->compute_cr = exe_compute_hdr_2x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH:
            p_utilities->compute_cr = exe_compute_hdr_3x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_Y2Y_HVH:
            p_utilities->compute_cr = exe_compute_y2y_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_HHB:
            p_utilities->compute_cr = exe_compute_linear_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB:
            p_utilities->compute_cr = exe_compute_hdr_1x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB:
            p_utilities->compute_cr = exe_compute_hdr_2x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB:
            p_utilities->compute_cr = exe_compute_hdr_3x_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB:
            p_utilities->compute_cr = exe_compute_linear_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB:
            p_utilities->compute_cr = exe_compute_hdr_1x_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB:
            p_utilities->compute_cr = exe_compute_hdr_2x_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB:
            p_utilities->compute_cr = exe_compute_hdr_3x_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_Y2Y_HHB:
            p_utilities->compute_cr = exe_compute_y2y_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_Y2Y_MD:
            p_utilities->compute_cr = exe_compute_y2y_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB:
            p_utilities->compute_cr = exe_compute_y2y_md_cr;
            break;
        case AMBA_IK_VIDEO_PIPE_MAX:
        default:
            p_utilities->compute_cr = exe_cmpt_invalid_cr_utility;
            amba_ik_system_print_uint32_5("[IK][ERROR] video did not support this video_pipe %d", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
            break;
        }
    } else if (p_ability->pipe == AMBA_IK_STILL_PIPE) {
        switch (p_ability->still_pipe) {
        case AMBA_IK_STILL_PIPE_HISO:
            //case AMBA_IK_STILL_HISO_CE:
            p_utilities->compute_cr = exe_compute_hiso_cr;
            break;
        case AMBA_IK_STILL_PIPE_LISO:
            p_utilities->compute_cr = exe_compute_hdr_1x_still_liso_cr;
            break;
        case AMBA_IK_STILL_PIPE_MAX:
        default:
            p_utilities->compute_cr = exe_cmpt_invalid_cr_utility;
            amba_ik_system_print_uint32_5("[IK][ERROR] still did not support this still_pipe %d", p_ability->still_pipe, DC_U, DC_U, DC_U, DC_U);
            break;
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] did not support this pipe %d", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
    }
}

static uint32 exe_set_flow_header(uint32 context_id, idsp_flow_ctrl_t *p_flow)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        p_flow->header.ik_cfg_id.running_number = p_ctx->organization.active_cr_state.cr_running_number;
        (void)amba_ik_system_memcpy(&p_flow->header.idsp_flow_addr, &p_flow, sizeof(uint32));
    }

    return rval;
}

static uint32 exe_r2y_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->r2y.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_42_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
            p_step_crs->sec3_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_42)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_43_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
            p_step_crs->sec3_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_43)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_44_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
            p_step_crs->sec3_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_44)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_45_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
            p_step_crs->sec4_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_45)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_46_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
            p_step_crs->sec4_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_46)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_47_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
            p_step_crs->sec4_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_47)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_48_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
            p_step_crs->sec4_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_48)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_49_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
            p_step_crs->sec4_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_49)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_50_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
            p_step_crs->sec4_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_50)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_51_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
            p_step_crs->sec4_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_51)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_52_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
            p_step_crs->sec4_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_52)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_100_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_100, CR_SIZE_100);
            p_step_crs->sec11_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_100)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_101_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_101, CR_SIZE_101);
            p_step_crs->sec11_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_101)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_102_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_102, CR_SIZE_102);
            p_step_crs->sec11_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_102)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_103_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_103, CR_SIZE_103);
            p_step_crs->sec11_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_103)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->r2y.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_motion_me1_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_motion_me1_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_me1.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_42_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
            p_step_crs->sec3_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_42)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_43_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
            p_step_crs->sec3_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_43)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_44_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
            p_step_crs->sec3_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_44)) >> 7);
        }
        if(p_cr_upd_flag->motion_me1.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_me1.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_motion_a_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_motion_a_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_a.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_motion_b_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_motion_b_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_b.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_b.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_motion_c_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_motion_c_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_c.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_c.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}
#if SUPPORT_FUSION
static uint32 exe_mono1_8_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_r2y_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag, uint32 pass) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;
    const amba_ik_r2y_cr_id_list_t *p_r2y_cr_id_list;

    if (pass == 1u) {
        p_r2y_cr_id_list = &p_cr_upd_flag->mono1;
    } else if (pass == 8u) {
        p_r2y_cr_id_list = &p_cr_upd_flag->mono8;
    } else {
        rval = IK_ERR_0005;
    }

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    }
    if (rval == IK_OK) {
        if(p_r2y_cr_id_list->cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_42_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_42, CR_SIZE_42);
            p_step_crs->sec3_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_42)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_43_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_43, CR_SIZE_43);
            p_step_crs->sec3_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_43)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_44_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_44, CR_SIZE_44);
            p_step_crs->sec3_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_44)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_45_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
            p_step_crs->sec4_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_45)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_46_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
            p_step_crs->sec4_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_46)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_47_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
            p_step_crs->sec4_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_47)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_48_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
            p_step_crs->sec4_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_48)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_49_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
            p_step_crs->sec4_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_49)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_50_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
            p_step_crs->sec4_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_50)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_51_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
            p_step_crs->sec4_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_51)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_52_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
            p_step_crs->sec4_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_52)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_100_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_100, CR_SIZE_100);
            p_step_crs->sec11_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_100)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_101_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_101, CR_SIZE_101);
            p_step_crs->sec11_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_101)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_102_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_102, CR_SIZE_102);
            p_step_crs->sec11_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_102)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_103_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_103, CR_SIZE_103);
            p_step_crs->sec11_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_103)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_r2y_cr_id_list->cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_mono3_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_mono3_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_a.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_mono4_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_mono4_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_a.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_mono5_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_mono5_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_a.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_mono6_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_mono6_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_a.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}

static uint32 exe_mono7_clean_flow_ctrl_cache(idsp_step_crs_t *p_step_crs, const amba_ik_mono7_flow_tables_list_t *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;

    if ((p_step_crs == NULL) || (p_flow_tbl_list == NULL) || (p_cr_upd_flag == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        if(p_cr_upd_flag->motion_a.cr_4_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
            p_step_crs->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_5_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
            p_step_crs->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_6_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
            p_step_crs->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_7_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
            p_step_crs->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_8_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
            p_step_crs->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_9_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
            p_step_crs->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_10_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
            p_step_crs->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_11_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
            p_step_crs->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_12_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
            p_step_crs->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_13_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
            p_step_crs->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_14_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
            p_step_crs->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_15_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
            p_step_crs->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_16_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
            p_step_crs->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_17_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
            p_step_crs->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
            p_step_crs->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
            p_step_crs->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
            p_step_crs->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_21_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
            p_step_crs->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_22_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
            p_step_crs->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_23_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
            p_step_crs->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_24_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
            p_step_crs->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_25_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
            p_step_crs->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_26_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
            p_step_crs->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_27_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
            p_step_crs->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_28_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
            p_step_crs->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_29_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
            p_step_crs->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_30_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
            p_step_crs->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_31_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
            p_step_crs->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_32_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
            p_step_crs->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_33_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
            p_step_crs->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_34_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
            p_step_crs->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_35_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
            p_step_crs->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_36_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
            p_step_crs->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_37_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
            p_step_crs->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_38_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
            p_step_crs->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_39_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
            p_step_crs->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_40_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
            p_step_crs->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_41_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
            p_step_crs->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_111_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_111, CR_SIZE_111);
            p_step_crs->sec18_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_111)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_112_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_112, CR_SIZE_112);
            p_step_crs->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_112)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_113_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_113, CR_SIZE_113);
            p_step_crs->sec18_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_113)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_114_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_114, CR_SIZE_114);
            p_step_crs->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_114)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_115_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_115, CR_SIZE_115);
            p_step_crs->sec18_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_115)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_116_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_116, CR_SIZE_116);
            p_step_crs->sec18_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_116)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_117_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_117, CR_SIZE_117);
            p_step_crs->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_117)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_118_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_118, CR_SIZE_118);
            p_step_crs->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_118)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_119_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_119, CR_SIZE_119);
            p_step_crs->sec18_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_119)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_120_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_120, CR_SIZE_120);
            p_step_crs->sec18_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_120)) >> 7);
        }

        if(p_cr_upd_flag->motion_a.cr_121_update != 0u) {
            amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_121, CR_SIZE_121);
            p_step_crs->sec18_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_121)) >> 7);
        }
    }

    return rval;
}
#endif
static uint32 exe_clean_flow_ctrl_cache(uint32 context_id, idsp_flow_ctrl_t *p_flow, const void *p_flow_tbl_list, const amba_ik_cr_id_list_t *p_cr_upd_flag) // TODO, consider multiple steps...
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint32 video_pipe;
    uint32 size = 0;

    // get ability
    rval |= img_ctx_get_context(context_id, &p_ctx);
    video_pipe = p_ctx->organization.attribute.ability.video_pipe;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB)) {
        amba_ik_flow_tables_list_t *p_flow_tables_list;
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t*));

        if(p_cr_upd_flag->cawarp != 0u) {
            size = (uint32)sizeof(int16) * (CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_hor_red, size);
            p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_hor_red));

            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_hor_blue, size);
            p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_hor_blue));

            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_ver_red, size);
            p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_ver_red));

            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_ver_blue, size);
            p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_ver_blue));
        }

        if(p_cr_upd_flag->warp != 0u) {
            size = (uint32)sizeof(int16) * (WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
            amba_ik_system_clean_cache(p_flow_tables_list->p_warp_hor, size);
            p_flow->calib.warp_horizontal_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_warp_hor));

            amba_ik_system_clean_cache(p_flow_tables_list->p_warp_ver, size);
            p_flow->calib.warp_vertical_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_warp_ver));
        }

        if(p_cr_upd_flag->warp_2nd != 0u) {
            size = (uint32)sizeof(int16) * (WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
            amba_ik_system_clean_cache(p_flow_tables_list->p_warp_hor_b, size);
            p_flow->calib.warp_horizontal_b_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_warp_hor_b));
        }

        if(p_cr_upd_flag->aaa_data != 0u) {
            amba_ik_system_clean_cache(p_flow_tables_list->p_aaa, (uint32)sizeof(idsp_aaa_data_t)*(uint32)MAX_TILE_NUM_X*(uint32)MAX_TILE_NUM_Y);
            p_flow->stitch.aaa_info_daddr = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_aaa));
        }

        if(p_cr_upd_flag->extra_window != 0u) {
            amba_ik_system_clean_cache(p_flow_tables_list->p_extra_window, (uint32)sizeof(idsp_extra_window_info_t));
            p_flow->window.extra_window_info_addr = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_extra_window));
        }

        if(p_cr_upd_flag->frame_info != 0u) {
            amba_ik_system_clean_cache(p_flow_tables_list->p_frame_info, (uint32)sizeof(ik_query_frame_info_t));
        }

        rval |= exe_r2y_clean_flow_ctrl_cache(&p_flow->step_crs[0], &p_flow_tables_list->r2y, p_cr_upd_flag);

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
        amba_ik_motion_fusion_flow_tables_list_t *p_flow_tables_list;
        (void)amba_ik_system_memcpy(&p_flow_tables_list, &p_flow_tbl_list, sizeof(amba_ik_motion_fusion_flow_tables_list_t*));

        if(p_cr_upd_flag->cawarp != 0u) {
            size = (uint32)sizeof(int16) * (CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_hor_red, size);
            p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_hor_red));

            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_hor_blue, size);
            p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_hor_blue));

            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_ver_red, size);
            p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_ver_red));

            amba_ik_system_clean_cache(p_flow_tables_list->p_ca_warp_ver_blue, size);
            p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_ca_warp_ver_blue));
        }

        if(p_cr_upd_flag->warp != 0u) {
            size = (uint32)sizeof(int16) * (WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
            amba_ik_system_clean_cache(p_flow_tables_list->p_warp_hor, size);
            p_flow->calib.warp_horizontal_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_warp_hor));

            amba_ik_system_clean_cache(p_flow_tables_list->p_warp_ver, size);
            p_flow->calib.warp_vertical_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_warp_ver));

            amba_ik_system_clean_cache(p_flow_tables_list->p_warp_hor_b, size);
            p_flow->calib.warp_horizontal_b_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_warp_hor_b));
        }

        if(p_cr_upd_flag->aaa_data != 0u) {
            amba_ik_system_clean_cache(p_flow_tables_list->p_aaa, (uint32)sizeof(idsp_aaa_data_t)*(uint32)MAX_TILE_NUM_X*(uint32)MAX_TILE_NUM_Y);
            p_flow->stitch.aaa_info_daddr = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_aaa));
        }

        if(p_cr_upd_flag->extra_window != 0u) {
            amba_ik_system_clean_cache(p_flow_tables_list->p_extra_window, (uint32)sizeof(idsp_extra_window_info_t));
            p_flow->window.extra_window_info_addr = physical2u32(amba_ik_system_virt_to_phys(p_flow_tables_list->p_extra_window));
        }

        if(p_cr_upd_flag->frame_info != 0u) {
            amba_ik_system_clean_cache(p_flow_tables_list->p_frame_info, (uint32)sizeof(ik_query_frame_info_t));
        }

        // step0
        rval |= exe_r2y_clean_flow_ctrl_cache(&p_flow->step_crs[0], &p_flow_tables_list->r2y, p_cr_upd_flag);
        // step1
        rval |= exe_motion_me1_clean_flow_ctrl_cache(&p_flow->step_crs[1], &p_flow_tables_list->motion_me1, p_cr_upd_flag);
        // step2
        rval |= exe_motion_a_clean_flow_ctrl_cache(&p_flow->step_crs[2], &p_flow_tables_list->motion_a, p_cr_upd_flag);
        // step3
        rval |= exe_motion_b_clean_flow_ctrl_cache(&p_flow->step_crs[3], &p_flow_tables_list->motion_b, p_cr_upd_flag);
        // step4
        rval |= exe_motion_c_clean_flow_ctrl_cache(&p_flow->step_crs[4], &p_flow_tables_list->motion_c, p_cr_upd_flag);
        if (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) {
#if SUPPORT_FUSION
            rval |= exe_mono1_8_clean_flow_ctrl_cache(&p_flow->step_crs[5], &p_flow_tables_list->mono1, p_cr_upd_flag, 1u);
            rval |= exe_mono3_clean_flow_ctrl_cache(&p_flow->step_crs[6], &p_flow_tables_list->mono3, p_cr_upd_flag);
            rval |= exe_mono4_clean_flow_ctrl_cache(&p_flow->step_crs[7], &p_flow_tables_list->mono4, p_cr_upd_flag);
            rval |= exe_mono5_clean_flow_ctrl_cache(&p_flow->step_crs[8], &p_flow_tables_list->mono5, p_cr_upd_flag);
            rval |= exe_mono6_clean_flow_ctrl_cache(&p_flow->step_crs[9], &p_flow_tables_list->mono6, p_cr_upd_flag);
            rval |= exe_mono7_clean_flow_ctrl_cache(&p_flow->step_crs[10], &p_flow_tables_list->mono7, p_cr_upd_flag);
            rval |= exe_mono1_8_clean_flow_ctrl_cache(&p_flow->step_crs[11], &p_flow_tables_list->mono8, p_cr_upd_flag, 1u);
#endif
        }
    } else {
        // misraC
    }

    p_flow->header.idsp_flow_addr = physical2u32(amba_ik_system_virt_to_phys(p_flow));
    amba_ik_system_clean_cache(p_flow, (uint32)sizeof(idsp_flow_ctrl_t));

    return rval;
}

static uint32 exe_set_frame_info(uint32 context_id, amba_ik_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    ik_query_frame_info_t *p_frame_info = NULL;

    // get ability
    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        (void)amba_ik_system_memcpy(&p_frame_info, &p_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t *));
        // hdr raw offset
        (void)amba_ik_system_memcpy(&p_frame_info->hdr_raw_info, &p_ctx->filters.input_param.hdr_raw_info, sizeof(ik_hdr_raw_info_t));
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
    }

static uint32 exe_set_hiso_frame_info(uint32 context_id, amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    ik_query_frame_info_t *p_frame_info = NULL;

    // get ability
    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        (void)amba_ik_system_memcpy(&p_frame_info, &p_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t*));
        // hdr raw offset
        (void)amba_ik_system_memcpy(&p_frame_info->hdr_raw_info, &p_ctx->filters.input_param.hdr_raw_info, sizeof(ik_hdr_raw_info_t));
    } else {
        amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

uint32 img_exe_execute_invalid_cr(uint32 context_id)
{
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        amba_ik_system_print_uint32_5("[IK]  img_exe_execute_invalid_cr ERROR, context_id %d, img_arch_get_context_number() %d", context_id, img_arch_get_context_number(), DC_U, DC_U, DC_U);
        rval = IK_ERR_0000; // TBD
    }
    return rval;
}

uint32 img_exe_execute_update_cr(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uintptr addr = 0;
    idsp_flow_ctrl_t *p_flow = NULL;
    void *p_flow_tbl_list = NULL;
    amba_ik_exe_cr_utilities_t utilities = {NULL};
    uint32 flow_idx = 0, cr_ring_num = 0;
    uint32 src_flow_idx;
    uint32 *p_explicit_iso_runnung_number = NULL;
    amba_ik_flow_tables_list_t *p_flow_tbl_list_ptr = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        if(p_ctx->organization.initial_flag != 0U) {
            cr_ring_num = p_ctx->organization.attribute.cr_ring_number;
            if (p_ctx->filters.input_param.eis_mode == STAGE_2_FOR_EIS_EXECUTE) {
                (void)img_ctx_get_explicit_iso_running_num(context_id, &p_explicit_iso_runnung_number);
                flow_idx = *p_explicit_iso_runnung_number;
            } else {
                rval |= exe_get_next_flow_control_index(cr_ring_num, &p_ctx->organization.active_cr_state, &flow_idx);
            }
            rval |= img_ctx_get_flow_control(context_id, flow_idx, &addr);
            (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));

            rval |= img_ctx_get_flow_tbl_list(context_id, flow_idx, &addr);
            (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

            if (rval == IK_OK) {
                if (p_ctx->filters.input_param.eis_mode == STAGE_1_FOR_EIS_EXECUTE) {//skip warp/ ca/window calculation, and warp/ ca ring working buffer.
                    //eis_mode_update_flags.iso.window_size_info_updated = p_ctx->filters.update_flags.iso.window_size_info_updated;
                    p_ctx->filters.update_flags.iso.window_size_info_updated = 0U;
                    //eis_mode_update_flags.iso.cfa_window_size_info_updated = p_ctx->filters.update_flags.iso.cfa_window_size_info_updated;
                    p_ctx->filters.update_flags.iso.cfa_window_size_info_updated = 0U;
                    //eis_mode_update_flags.iso.calib_warp_info_updated = p_ctx->filters.update_flags.iso.calib_warp_info_updated;
                    p_ctx->filters.update_flags.iso.calib_warp_info_updated = 0U;
                    //eis_mode_update_flags.iso.calib_ca_warp_info_updated = p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated;
                    p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated = 0U;
                    //eis_mode_update_flags.iso.dzoom_info_updated = p_ctx->filters.update_flags.iso.dzoom_info_updated;
                    p_ctx->filters.update_flags.iso.dzoom_info_updated = 0U;
                    //eis_mode_update_flags.iso.warp_enable_updated = p_ctx->filters.update_flags.iso.warp_enable_updated;
                    p_ctx->filters.update_flags.iso.warp_enable_updated = 0U;
                    //eis_mode_update_flags.iso.cawarp_enable_updated = p_ctx->filters.update_flags.iso.cawarp_enable_updated;
                    p_ctx->filters.update_flags.iso.cawarp_enable_updated = 0U;
                    //eis_mode_update_flags.iso.dmy_range_updated = p_ctx->filters.update_flags.iso.dmy_range_updated;
                    p_ctx->filters.update_flags.iso.dmy_range_updated = 0U;
                    //eis_mode_update_flags.iso.flip_mode_updated = p_ctx->filters.update_flags.iso.flip_mode_updated;
                    p_ctx->filters.update_flags.iso.flip_mode_updated = 0U;
                    //eis_mode_update_flags.iso.vin_active_win_updated = p_ctx->filters.update_flags.iso.vin_active_win_updated;
                    p_ctx->filters.update_flags.iso.vin_active_win_updated = 0U;
                    //eis_mode_update_flags.iso.stitching_info_updated = p_ctx->filters.update_flags.iso.stitching_info_updated;
                    p_ctx->filters.update_flags.iso.stitching_info_updated = 0U;
                } else if (p_ctx->filters.input_param.eis_mode == STAGE_2_FOR_EIS_EXECUTE) {// do post warp/ ca/ window calculation, and warp/ ca ring working buffer.
                    {
                        amba_ik_filter_update_flags_t eis_mode_update_flags;
                        (void)amba_ik_system_memset(&eis_mode_update_flags, 0x0, sizeof(amba_ik_filter_update_flags_t));
                        (void)amba_ik_system_memcpy(&eis_mode_update_flags, &p_ctx->filters.update_flags, sizeof(amba_ik_filter_update_flags_t));
                        rval |= img_ctx_reset_update_flags(context_id);
                        p_ctx->filters.update_flags.iso.window_size_info_updated = eis_mode_update_flags.iso.window_size_info_updated;
                        //eis_mode_update_flags.iso.window_size_info_updated = 0U;
                        p_ctx->filters.update_flags.iso.cfa_window_size_info_updated = eis_mode_update_flags.iso.cfa_window_size_info_updated;
                        //eis_mode_update_flags.iso.cfa_window_size_info_updated = 0U;
                        p_ctx->filters.update_flags.iso.calib_warp_info_updated = eis_mode_update_flags.iso.calib_warp_info_updated;
                        //eis_mode_update_flags.iso.calib_warp_info_updated = 0U;
                        p_ctx->filters.update_flags.iso.calib_ca_warp_info_updated = eis_mode_update_flags.iso.calib_ca_warp_info_updated;
                        //eis_mode_update_flags.iso.calib_ca_warp_info_updated = 0U;
                        p_ctx->filters.update_flags.iso.dzoom_info_updated = eis_mode_update_flags.iso.dzoom_info_updated;
                        //eis_mode_update_flags.iso.dzoom_info_updated = 0U;
                        p_ctx->filters.update_flags.iso.warp_enable_updated = eis_mode_update_flags.iso.warp_enable_updated;
                        //eis_mode_update_flags.iso.warp_enable_updated = 0U;
                        p_ctx->filters.update_flags.iso.cawarp_enable_updated = eis_mode_update_flags.iso.cawarp_enable_updated;
                        //eis_mode_update_flags.iso.cawarp_enable_updated = 0U;
                        p_ctx->filters.update_flags.iso.dmy_range_updated = eis_mode_update_flags.iso.dmy_range_updated;
                        //eis_mode_update_flags.iso.dmy_range_updated = 0U;
                        p_ctx->filters.update_flags.iso.flip_mode_updated = eis_mode_update_flags.iso.flip_mode_updated;
                        //eis_mode_update_flags.iso.flip_mode_updated = 0U;
                        p_ctx->filters.update_flags.iso.vin_active_win_updated = eis_mode_update_flags.iso.vin_active_win_updated;
                        //eis_mode_update_flags.iso.vin_active_win_updated = 0U;
                        p_ctx->filters.update_flags.iso.stitching_info_updated = eis_mode_update_flags.iso.stitching_info_updated;
                        //eis_mode_update_flags.iso.stitching_info_updated = 0U;
                    }

                } else {
                    ;//misra.
                }
                if (p_ctx->filters.input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) { // eis_stage_2 doesn't need clone and reset cmd.
                    src_flow_idx = p_ctx->organization.active_cr_state.active_flow_idx;
                    rval |= exe_clone_flow_control_and_tbl_list(context_id, p_flow, p_flow_tbl_list, src_flow_idx);
                }
                rval |= exe_get_next_cr_index(cr_ring_num, &p_ctx->filters.update_flags, &p_ctx->organization.active_cr_state.cr_index);
                rval |= exe_check_next_cr_index(context_id, &p_ctx->filters.update_flags.cr, &p_ctx->organization.active_cr_state.cr_index);
                rval |= exe_update_flow_tbl_list(context_id, flow_idx, &p_ctx->organization.active_cr_state.cr_index, &p_ctx->filters.update_flags.cr);
                if (p_ctx->filters.input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) { // eis_stage_2 can not touch active_iso_idx.
                    p_ctx->organization.active_cr_state.active_flow_idx = flow_idx;
                }
                exe_get_cr_utilities(&p_ctx->organization.attribute.ability, &utilities);
                if (utilities.compute_cr != NULL) {
                    rval |= utilities.compute_cr(p_flow, (void *)p_flow_tbl_list, &p_ctx->filters);

                    // prepare idsp_flow_ctrl_t
                    if (p_ctx->filters.input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) { // eis_stage_2 can not touch flow_header.
                        rval |= exe_set_flow_header(context_id, p_flow);
                    }
                    rval |= exe_clean_flow_ctrl_cache(context_id, p_flow, p_flow_tbl_list, &p_ctx->filters.update_flags.cr);
                    rval |= img_ctx_reset_update_flags(context_id);
                    (void)amba_ik_system_memcpy(&p_flow_tbl_list_ptr, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t *));
                    rval |= exe_set_frame_info(context_id, p_flow_tbl_list_ptr);
                } else {
                    amba_ik_system_print_str_5("[IK] get NULL compute_cr", NULL, NULL, NULL, NULL, NULL);
                    rval = IK_ERR_0005;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
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

static uint32 INLINE exe_execute_r2y_filter_update_check(uint32 video_pipe, const amba_ik_filter_update_flags_t *p_update_flag, uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        if (p_update_flag->iso.sensor_information_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] sensor_information_updated = 0. Please check ik_set_vin_sensor_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.exp0_fe_static_blc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp0_fe_static_blc_updated = 0. Please check ik_set_exp0_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    if ((video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) || \
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) || \
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) || \
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) || \
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD)) {
        if (p_update_flag->iso.exp1_fe_static_blc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp1_fe_static_blc_updated = 0. Please check ik_set_exp1_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.exp1_fe_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp1_fe_wb_gain_updated = 0. Please check ik_set_exp1_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if ((video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
            (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB)) {
            if (p_update_flag->iso.exp2_fe_static_blc_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] exp2_fe_static_blc_updated = 0. Please check ik_set_exp2_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.exp2_fe_wb_gain_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] exp2_fe_wb_gain_updated = 0. Please check ik_set_exp2_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
        if (p_update_flag->iso.hdr_blend_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] hdr_blend_updated = 0. Please check ik_set_hdr_blend() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        if (p_update_flag->iso.dynamic_bpc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] dynamic_bpc_updated = 0. Please check ik_set_dynamic_bad_pixel_corr() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.cfa_leakage_filter_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] cfa_leakage_filter_updated = 0. Please check ik_set_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.anti_aliasing_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] anti_aliasing_updated = 0. Please check ik_set_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.grgb_mismatch_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] grgb_mismatch_updated = 0. Please check ik_set_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.before_ce_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_before_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB)) {
        if (p_update_flag->iso.ce_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_updated = 0. Please check ik_set_ce() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.ce_input_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_input_table_updated = 0. Please check ik_set_ce_input_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.ce_out_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_ce_out_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    if ((video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        if (p_update_flag->iso.after_ce_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] after_ce_wb_gain_updated = 0. Please check ik_set_after_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.cfa_noise_filter_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] cfa_noise_filter_updated = 0. Please check ik_set_cfa_noise_filter() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.demosaic_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] demosaic_updated = 0. Please check ik_set_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.color_correction_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] color_correction_updated = 0. Please check ik_set_color_correction()I have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.tone_curve_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] tone_curve_updated = 0. Please check ik_set_tone_curve() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.rgb_to_yuv_matrix_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] rgb_to_yuv_matrix_updated = 0. Please check ik_set_rgb_to_yuv_matrix() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    if (p_update_flag->iso.chroma_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_scale_updated = 0. Please check ik_set_chroma_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.chroma_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_filter_updated = 0. Please check ik_set_chroma_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.chroma_median_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_median_filter_updated = 0. Please check ik_set_chroma_median_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HVH) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_HHB) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD) &&
        (video_pipe != (uint8)AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        if (p_update_flag->iso.first_luma_processing_mode_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] first_luma_processing_mode_updated = 0. Please check ik_set_fst_luma_process_mode() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        } else {
            if(use_sharpen_not_asf == 0U) {
                if (p_update_flag->iso.advance_spatial_filter_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] advance_spatial_filter_updated = 0. Please check ik_set_adv_spatial_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
            } else {
                if (p_update_flag->iso.fstshpns_both_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_both_updated = 0. Please check ik_set_fst_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_noise_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_noise_updated = 0. Please check ik_set_fst_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_fir_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_fir_updated = 0. Please check ik_set_fst_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_coring_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_updated = 0. Please check ik_set_fst_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_coring_index_scale_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_index_scale_updated = 0. Please check ik_set_fst_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_min_coring_result_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_min_coring_result_updated = 0. Please check ik_set_fst_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_max_coring_result_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_max_coring_result_updated = 0. Please check ik_set_fst_shp_max_coring_rslt() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
                if (p_update_flag->iso.fstshpns_scale_coring_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] fstshpns_scale_coring_updated = 0. Please check ik_set_fst_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    if (p_update_flag->iso.video_mctf_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_updated = 0. Please check ik_set_video_mctf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.video_mctf_ta_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_ta_updated = 0. Please check ik_set_video_mctf_ta() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_update_flag->iso.fnlshpns_both_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_both_updated = 0. Please check ik_set_fnl_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_noise_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_noise_updated = 0. Please check ik_set_fnl_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_fir_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_fir_updated = 0. Please check ik_set_fnl_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_coring_updated = 0. Please check ik_set_fnl_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_coring_index_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_coring_index_scale_updated = 0. Please check ik_set_fnl_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_min_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_min_coring_result_updated = 0. Please check ik_set_fnl_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_max_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_max_coring_result_updated = 0. Please check ik_set_fnl_shp_max_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->iso.fnlshpns_scale_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_scale_coring_updated = 0. Please check ik_set_fnl_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 INLINE exe_execute_motion_detect_update_check(uint32 video_pipe, const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;

    if ((video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
        (video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
        if (p_update_flag->iso.motion_detect_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] motion_detect_updated = 0. Please check ik_set_motion_detect() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
#if SUPPORT_FUSION
static uint32 INLINE exe_execute_fusion_update_check(uint32 video_pipe, const amba_ik_filter_update_flags_t *p_update_flag, uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;

    if (video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) {
        if (p_update_flag->iso.mono_dynamic_bad_pixel_corr_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_dynamic_bad_pixel_corr_updated = 0. Please check ik_set_mono_dynamic_bad_pixel_corr() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_cfa_leakage_filter_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_cfa_leakage_filter_updated = 0. Please check ik_set_mono_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_cfa_noise_filter_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_cfa_noise_filter_updated = 0. Please check ik_set_mono_cfa_noise_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_anti_aliasing_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_anti_aliasing_updated = 0. Please check ik_set_mono_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_before_ce_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_before_ce_wb_gain_updated = 0. Please check ik_set_mono_before_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_after_ce_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_after_ce_wb_gain_updated = 0. Please check ik_set_mono_after_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_color_correction_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_color_correction_updated = 0. Please check ik_set_mono_color_correction() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_tone_curve_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_tone_curve_updated = 0. Please check ik_set_mono_tone_curve() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_rgb_to_yuv_matrix_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_rgb_to_yuv_matrix_updated = 0. Please check ik_set_mono_rgb_to_yuv_matrix() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_demosaic_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_demosaic_updated = 0. Please check ik_set_mono_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_grgb_mismatch_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_grgb_mismatch_updated = 0. Please check ik_set_mono_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_fst_luma_process_mode_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_fst_luma_process_mode_updated = 0. Please check ik_set_mono_fst_luma_process_mode() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (use_sharpen_not_asf == 0u) {
            if (p_update_flag->iso.mono_adv_spatial_filter_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_adv_spatial_filter_updated = 0. Please check ik_set_mono_adv_spatial_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        } else if (use_sharpen_not_asf == 1u) {
            if (p_update_flag->iso.mono_fst_shp_both_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_both_updated = 0. Please check ik_set_mono_fst_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_noise_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_noise_updated = 0. Please check ik_set_mono_fst_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_fir_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_fir_updated = 0. Please check ik_set_mono_fst_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_coring_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_coring_updated = 0. Please check ik_set_mono_fst_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_coring_idx_scale_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_coring_idx_scale_updated = 0. Please check ik_set_mono_fst_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_min_coring_rslt_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_min_coring_rslt_updated = 0. Please check ik_set_mono_fst_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_max_coring_rslt_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_max_coring_rslt_updated = 0. Please check ik_set_mono_fst_shp_max_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->iso.mono_fst_shp_scale_coring_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] mono_fst_shp_scale_coring_updated = 0. Please check ik_set_mono_fst_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        } else {
            // misraC
        }
        if (p_update_flag->iso.mono_exp0_fe_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_exp0_fe_wb_gain_updated = 0. Please check ik_set_mono_exp0_fe_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_exp0_fe_static_blc_level_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_exp0_fe_static_blc_level_updated = 0. Please check ik_set_mono_exp0_fe_static_blc_level() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_ce_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_ce_updated = 0. Please check ik_set_mono_ce() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_ce_input_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_ce_input_table_updated = 0. Please check ik_set_mono_ce_input_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.mono_ce_output_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] mono_ce_output_table_updated = 0. Please check ik_set_mono_ce_output_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->iso.fusion_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] fusion_updated = 0. Please check ik_set_fusion() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    } else {
        // misraC
    }

    return rval;
}
#endif
static uint32 INLINE exe_execute_filter_update_check(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag, const amba_ik_input_parameters_t *p_input_para)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;

    // step0
    rval |= exe_execute_r2y_filter_update_check(video_pipe, p_update_flag, p_input_para->first_luma_process_mode.use_sharpen_not_asf);
    // step1/2/3/4
    rval |= exe_execute_motion_detect_update_check(video_pipe, p_update_flag);
#if SUPPORT_FUSION
    // step5/6/7/8/9/10/11
    rval |= exe_execute_fusion_update_check(video_pipe, p_update_flag, p_input_para->mono_fst_luma_process_mode.use_sharpen_not_asf);
#endif
    return rval;
}

uint32 img_exe_execute_first_cr(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uintptr addr = 0;
    idsp_flow_ctrl_t *p_flow = NULL;
    void *p_flow_tbl_list = NULL;
    amba_ik_exe_cr_utilities_t utilities = {NULL};
    amba_ik_flow_tables_list_t *p_flow_tbl_list_ptr = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        if(p_ctx->organization.initial_flag != 0U) {
            rval |= img_ctx_get_flow_control(context_id, 0, &addr);
            (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));

            rval |= img_ctx_get_flow_tbl_list(context_id, 0, &addr);
            (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

            if (rval == IK_OK) {
                rval |= exe_execute_filter_update_check(&p_ctx->organization.attribute.ability, &p_ctx->filters.update_flags, &p_ctx->filters.input_param);
                if (rval == IK_OK) {
                    p_ctx->organization.active_cr_state.active_flow_idx = 0;
                    rval |= exe_check_next_cr_index(context_id, &p_ctx->filters.update_flags.cr, &p_ctx->organization.active_cr_state.cr_index);
                    exe_get_cr_utilities(&p_ctx->organization.attribute.ability, &utilities);
                    if (utilities.compute_cr != NULL) {
                        rval |= utilities.compute_cr(p_flow, p_flow_tbl_list, &p_ctx->filters);

                        // prepare idsp_flow_ctrl_t
                        rval |= exe_set_flow_header(context_id, p_flow);
                        rval |= exe_clean_flow_ctrl_cache(context_id, p_flow, p_flow_tbl_list, &p_ctx->filters.update_flags.cr);
                        rval |= img_ctx_reset_update_flags(context_id);
                        (void)amba_ik_system_memcpy(&p_flow_tbl_list_ptr, &p_flow_tbl_list, sizeof(amba_ik_flow_tables_list_t *));
                        rval |= exe_set_frame_info(context_id, p_flow_tbl_list_ptr);
                        p_ctx->organization.executer_method.execute_cr = img_exe_execute_update_cr;
                    } else {
                        amba_ik_system_print_str_5("[IK] get NULL compute_cr", NULL, NULL, NULL, NULL, NULL);
                        rval = IK_ERR_0005;
                    }
                } else {
                    amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_execute_filter_update_check(context_id %d) fail !", context_id, DC_U, DC_U, DC_U, DC_U);
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
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

uint32 img_exe_init_executer(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        if (p_ctx->organization.attribute.ability.pipe == AMBA_IK_VIDEO_PIPE) {
            if ((p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR)||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_FUSION) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HVH) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HVH) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HVH) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HVH) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HVH) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_MD_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_LINEAR_CE_MD_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_2_MD_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_HDR_EXPO_3_MD_HHB) ||
                (p_ctx->organization.attribute.ability.video_pipe == AMBA_IK_VIDEO_PIPE_Y2Y_MD_HHB)) {
                if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) {
                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_first_cr;
                } else if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_SOFT_INIT) {
                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_update_cr;
                } else {
                    // misra
                }

            }  else {
                p_ctx->organization.executer_method.execute_cr = img_exe_execute_invalid_cr;
                amba_ik_system_print_uint32_5("[IK] not support this video_pipe %d", p_ctx->organization.attribute.ability.video_pipe, DC_U, DC_U, DC_U, DC_U);
            }
        } else if (p_ctx->organization.attribute.ability.pipe == AMBA_IK_STILL_PIPE) {
            if ((p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_PIPE_HISO)/* || (p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_HISO_CE)*/) {
                p_ctx->organization.executer_method.execute_cr = img_exe_execute_hiso_cr;
            } else if ((p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_PIPE_LISO)) {
                if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) {
                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_first_cr;
                } else if(p_ik_buffer_info->init_mode == AMBA_IK_ARCH_SOFT_INIT) {
                    p_ctx->organization.executer_method.execute_cr = img_exe_execute_update_cr;
                } else {
                    // misra
                }
            } else {
                p_ctx->organization.executer_method.execute_cr = img_exe_execute_invalid_cr;
                amba_ik_system_print_uint32_5("[IK] not support this still_pipe %d", p_ctx->organization.attribute.ability.still_pipe, DC_U, DC_U, DC_U, DC_U);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] not support this pipe %d", p_ctx->organization.attribute.ability.pipe, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000; // TBD
        }
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
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

uint32 img_exe_warp_remap_hvh_vr(const ik_in_warp_remap_t *p_warp_remap_in, ik_out_warp_remap_t *p_warp_remap_out_1st, ik_out_warp_remap_t *p_warp_remap_out_2nd)
{
    return exe_warp_remap_hvh_vr(p_warp_remap_in, p_warp_remap_out_1st, p_warp_remap_out_2nd);
}

uint32 img_exe_cawarp_remap(const ik_in_cawarp_remap_t *p_cawarp_remap_in, ik_out_cawarp_remap_t *p_cawarp_remap_out)
{
    return exe_cawarp_remap(p_cawarp_remap_in, p_cawarp_remap_out);
}

/****************     clean cache     ****************/
// common use api
static void exe_clean_hiso_r2y_sec2_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_step1_sec2_cr_list_t *p_flow_tbl_list)
{
    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_4, CR_SIZE_4);
    p_step_cr->sec2_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_4)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_5, CR_SIZE_5);
    p_step_cr->sec2_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_5)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_6, CR_SIZE_6);
    p_step_cr->sec2_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_6)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_7, CR_SIZE_7);
    p_step_cr->sec2_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_7)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_8, CR_SIZE_8);
    p_step_cr->sec2_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_8)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_9, CR_SIZE_9);
    p_step_cr->sec2_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_9)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_10, CR_SIZE_10);
    p_step_cr->sec2_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_10)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_11, CR_SIZE_11);
    p_step_cr->sec2_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_11)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_12, CR_SIZE_12);
    p_step_cr->sec2_crs[8].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_12)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_13, CR_SIZE_13);
    p_step_cr->sec2_crs[9].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_13)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_14, CR_SIZE_14);
    p_step_cr->sec2_crs[10].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_14)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_15, CR_SIZE_15);
    p_step_cr->sec2_crs[11].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_15)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_16, CR_SIZE_16);
    p_step_cr->sec2_crs[12].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_16)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_17, CR_SIZE_17);
    p_step_cr->sec2_crs[13].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_17)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_18, CR_SIZE_18);
    p_step_cr->sec2_crs[14].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_18)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_19, CR_SIZE_19);
    p_step_cr->sec2_crs[15].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_19)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_20, CR_SIZE_20);
    p_step_cr->sec2_crs[16].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_20)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_21, CR_SIZE_21);
    p_step_cr->sec2_crs[17].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_21)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_22, CR_SIZE_22);
    p_step_cr->sec2_crs[18].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_22)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_23, CR_SIZE_23);
    p_step_cr->sec2_crs[19].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_23)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_24, CR_SIZE_24);
    p_step_cr->sec2_crs[20].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_24)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_25, CR_SIZE_25);
    p_step_cr->sec2_crs[21].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_25)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_27)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_28, CR_SIZE_28);
    p_step_cr->sec2_crs[24].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_28)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_34, CR_SIZE_34);
    p_step_cr->sec2_crs[30].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_34)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
}

static void exe_clean_hiso_r2y_sec4_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_sec4_cr_list_t *p_flow_tbl_list)
{
    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_45, CR_SIZE_45);
    p_step_cr->sec4_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_45)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_46, CR_SIZE_46);
    p_step_cr->sec4_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_46)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_47, CR_SIZE_47);
    p_step_cr->sec4_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_47)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_48, CR_SIZE_48);
    p_step_cr->sec4_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_48)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_49, CR_SIZE_49);
    p_step_cr->sec4_crs[4].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_49)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_50, CR_SIZE_50);
    p_step_cr->sec4_crs[5].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_50)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_51, CR_SIZE_51);
    p_step_cr->sec4_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_51)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_52, CR_SIZE_52);
    p_step_cr->sec4_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_52)) >> 7);
}

static void exe_clean_hiso_y2y_sec2_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_step3_sec2_cr_list_t *p_flow_tbl_list)
{
    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_26)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_29)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_30)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_31)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_32)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_33)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_35)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_36)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_37)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_38)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_39)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_40)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_CR_buf_41)) >> 7);
}

// every steps api
static void exe_clean_hiso_step1_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step1_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_r2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);
    exe_clean_hiso_r2y_sec4_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec4);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_118)) >> 7);
}

static void exe_clean_hiso_step2_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step2_cr_list_t *p_flow_tbl_list)
{
    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_26, CR_SIZE_26);
    p_step_cr->sec2_crs[22].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_26)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_27, CR_SIZE_27);
    p_step_cr->sec2_crs[23].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_27)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_29, CR_SIZE_29);
    p_step_cr->sec2_crs[25].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_29)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_30, CR_SIZE_30);
    p_step_cr->sec2_crs[26].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_30)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_31, CR_SIZE_31);
    p_step_cr->sec2_crs[27].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_31)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_32, CR_SIZE_32);
    p_step_cr->sec2_crs[28].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_32)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_33, CR_SIZE_33);
    p_step_cr->sec2_crs[29].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_33)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_35, CR_SIZE_35);
    p_step_cr->sec2_crs[31].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_35)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_36, CR_SIZE_36);
    p_step_cr->sec2_crs[33].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_36)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_37, CR_SIZE_37);
    p_step_cr->sec2_crs[34].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_37)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_38, CR_SIZE_38);
    p_step_cr->sec2_crs[35].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_38)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_39, CR_SIZE_39);
    p_step_cr->sec2_crs[36].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_39)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_40, CR_SIZE_40);
    p_step_cr->sec2_crs[32].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_40)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec2.p_CR_buf_41, CR_SIZE_41);
    p_step_cr->sec2_crs[37].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec2.p_CR_buf_41)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_118)) >> 7);
}

static void exe_clean_hiso_step3_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step3_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_y2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_118)) >> 7);
}

static void exe_clean_hiso_step4a_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step4a_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_y2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);
}

static void exe_clean_hiso_step5_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step5_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_y2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_114)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_118)) >> 7);
}

static void exe_clean_hiso_step6_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step6_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_y2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_114)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);
}

static void exe_clean_hiso_step11_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step11_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_r2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);
    exe_clean_hiso_r2y_sec4_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec4);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_114)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_118, CR_SIZE_118);
    p_step_cr->sec18_crs[7].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_118)) >> 7);
}

static void exe_clean_hiso_step13_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step13_cr_list_t *p_flow_tbl_list)
{
    exe_clean_hiso_y2y_sec2_flow_ctrl_cache(p_step_cr, &p_flow_tbl_list->sec2);

#ifdef _HISO_SEC3_IN_STEP13
    amba_ik_system_clean_cache(p_flow_tbl_list->sec3.p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec3.p_CR_buf_42)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec3.p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec3.p_CR_buf_43)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec3.p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec3.p_CR_buf_44)) >> 7);
#endif

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_114, CR_SIZE_114);
    p_step_cr->sec18_crs[3].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_114)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);
}

static void exe_clean_hiso_step14_flow_ctrl_cache(idsp_step_crs_t *p_step_cr, const amba_ik_hiso_step14_cr_list_t *p_flow_tbl_list)
{
    amba_ik_system_clean_cache(p_flow_tbl_list->sec3.p_CR_buf_42, CR_SIZE_42);
    p_step_cr->sec3_crs[0].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec3.p_CR_buf_42)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec3.p_CR_buf_43, CR_SIZE_43);
    p_step_cr->sec3_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec3.p_CR_buf_43)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec3.p_CR_buf_44, CR_SIZE_44);
    p_step_cr->sec3_crs[2].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec3.p_CR_buf_44)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_112, CR_SIZE_112);
    p_step_cr->sec18_crs[1].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_112)) >> 7);

    amba_ik_system_clean_cache(p_flow_tbl_list->sec18.p_CR_buf_117, CR_SIZE_117);
    p_step_cr->sec18_crs[6].config_region_addr = (physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->sec18.p_CR_buf_117)) >> 7);
}

static uint32 exe_clean_hiso_flow_ctrl_cache(idsp_flow_ctrl_t *p_flow, const amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list)
{
    uint32 rval = IK_OK;
    uint32 size;

    if ((p_flow == NULL) || (p_flow_tbl_list == NULL)) {
        amba_ik_system_print_str_5("[IK][ERROR] img_cfg_clean_flow_ctrl_cache() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        exe_clean_hiso_step1_flow_ctrl_cache(&p_flow->step_crs[0], &p_flow_tbl_list->step1);
        exe_clean_hiso_step2_flow_ctrl_cache(&p_flow->step_crs[1], &p_flow_tbl_list->step2);
        exe_clean_hiso_step3_flow_ctrl_cache(&p_flow->step_crs[2], &p_flow_tbl_list->step3);
        exe_clean_hiso_step3_flow_ctrl_cache(&p_flow->step_crs[3], &p_flow_tbl_list->step4);
        exe_clean_hiso_step4a_flow_ctrl_cache(&p_flow->step_crs[4], &p_flow_tbl_list->step4a);
        exe_clean_hiso_step5_flow_ctrl_cache(&p_flow->step_crs[5], &p_flow_tbl_list->step5);
        exe_clean_hiso_step6_flow_ctrl_cache(&p_flow->step_crs[6], &p_flow_tbl_list->step6);
        exe_clean_hiso_step6_flow_ctrl_cache(&p_flow->step_crs[7], &p_flow_tbl_list->step7);
        exe_clean_hiso_step6_flow_ctrl_cache(&p_flow->step_crs[8], &p_flow_tbl_list->step8);
        exe_clean_hiso_step4a_flow_ctrl_cache(&p_flow->step_crs[9], &p_flow_tbl_list->step9);
        exe_clean_hiso_step5_flow_ctrl_cache(&p_flow->step_crs[10], &p_flow_tbl_list->step10);
        exe_clean_hiso_step11_flow_ctrl_cache(&p_flow->step_crs[11], &p_flow_tbl_list->step11);
        exe_clean_hiso_step11_flow_ctrl_cache(&p_flow->step_crs[12], &p_flow_tbl_list->step12);
        exe_clean_hiso_step13_flow_ctrl_cache(&p_flow->step_crs[13], &p_flow_tbl_list->step13);
#ifndef _HISO_SEC3_IN_STEP13
        exe_clean_hiso_step14_flow_ctrl_cache(&p_flow->step_crs[14], &p_flow_tbl_list->step14);
#endif

        size = (uint32)sizeof(int16) * (CAWARP_VIDEO_MAXHORGRID * CAWARP_VIDEO_MAXVERGRID);
        amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_hor_red, size);
        p_flow->calib.cawarp_horizontal_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_hor_red));

        amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_hor_blue, size);
        p_flow->calib.cawarp_horizontal_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_hor_blue));

        amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_ver_red, size);
        p_flow->calib.cawarp_vertical_table_addr_red = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_ver_red));

        amba_ik_system_clean_cache(p_flow_tbl_list->p_ca_warp_ver_blue, size);
        p_flow->calib.cawarp_vertical_table_addr_blue = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_ca_warp_ver_blue));

        size = (uint32)sizeof(int16) * (WARP_VIDEO_MAXHORGRID * WARP_VIDEO_MAXVERGRID * 2U);
        amba_ik_system_clean_cache(p_flow_tbl_list->p_warp_hor, size);
        p_flow->calib.warp_horizontal_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_warp_hor));

        amba_ik_system_clean_cache(p_flow_tbl_list->p_warp_ver, size);
        p_flow->calib.warp_vertical_table_address = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_warp_ver));

        amba_ik_system_clean_cache(p_flow_tbl_list->p_aaa, (uint32)sizeof(idsp_aaa_data_t)*(uint32)MAX_TILE_NUM_X*(uint32)MAX_TILE_NUM_Y);
        p_flow->stitch.aaa_info_daddr = physical2u32(amba_ik_system_virt_to_phys(p_flow_tbl_list->p_aaa));

        p_flow->header.idsp_flow_addr = physical2u32(amba_ik_system_virt_to_phys(p_flow));
        amba_ik_system_clean_cache(p_flow, (uint32)sizeof(idsp_flow_ctrl_t));

        amba_ik_system_clean_cache(p_flow_tbl_list->p_frame_info, (uint32)sizeof(ik_query_frame_info_t));
    }

    return rval;
}

uint32 img_exe_execute_hiso_cr(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    uintptr addr = 0;
    idsp_flow_ctrl_t *p_flow = NULL;
    amba_ik_hiso_flow_tables_list_t *p_flow_tbl_list = NULL;
    amba_ik_exe_cr_utilities_t utilities = {NULL};

    rval |= img_ctx_get_context(context_id, &p_ctx);

    if(rval == IK_OK) {
        if(p_ctx->organization.initial_flag != 0U) {
            rval |= img_ctx_get_flow_control(context_id, 0, &addr);
            (void)amba_ik_system_memcpy(&p_flow, &addr, sizeof(void *));

            rval |= img_ctx_get_flow_tbl_list(context_id, 0, &addr);
            (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

            if (rval == IK_OK) {
                p_ctx->organization.active_cr_state.active_flow_idx = 0;
                exe_get_cr_utilities(&p_ctx->organization.attribute.ability, &utilities);
                if (utilities.compute_cr != NULL) {
                    rval |= utilities.compute_cr(p_flow, (void *)p_flow_tbl_list, &p_ctx->filters);

                    // prepare idsp_flow_ctrl_t
                    rval |= exe_set_flow_header(context_id, p_flow);
                    rval |= exe_clean_hiso_flow_ctrl_cache(p_flow, p_flow_tbl_list);
                    rval |= img_ctx_reset_update_flags(context_id);
                    rval |= exe_set_hiso_frame_info(context_id, p_flow_tbl_list);
                } else {
                    amba_ik_system_print_str_5("[IK] get NULL compute_cr", NULL, NULL, NULL, NULL, NULL);
                    rval = IK_ERR_0005;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK] call img_ctx_get_flow_control(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
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

uint32 img_exe_query_frame_info(uint32 context_id, const uint32 ik_id, ik_query_frame_info_t *p_frame_info)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    const amba_ik_flow_tables_list_t *p_flow_tbl_list = NULL;
    const amba_ik_hiso_flow_tables_list_t *p_hiso_flow_tbl_list = NULL;
    uintptr addr = 0UL;
    uint32 running_num;
    uint32 active_id;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if(rval == IK_OK) {
        if (p_frame_info == NULL) {
            amba_ik_system_print_uint32_5("[IK][ERROR] p_frame_info is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0005;
        } else {
            running_num = ik_id - ((ik_id>>16U)<<16U);
            active_id = (running_num-1U) % p_ctx->organization.attribute.cr_ring_number;
            if(active_id > (p_ctx->organization.attribute.cr_ring_number - 1u)) {
                amba_ik_system_print_uint32_5("[IK][ERROR] active_id %d > (cr_ring_number-1) = %d", active_id, (p_ctx->organization.attribute.cr_ring_number - 1u), DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD
            }
            if (rval == IK_OK) {
                if (p_ctx->organization.attribute.ability.pipe == AMBA_IK_VIDEO_PIPE) {
                    rval |= img_ctx_get_flow_tbl_list(context_id, (uint32)active_id, &addr);
                    (void)amba_ik_system_memcpy(&p_flow_tbl_list, &addr, sizeof(void *));

                    if (p_flow_tbl_list->p_frame_info != NULL) {
                        (void)amba_ik_system_memcpy(p_frame_info, p_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t));
                    } else {
                        amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_query_frame_info() p_flow_tbl_list is NULL", DC_U, DC_U, DC_U, DC_U, DC_U);
                    }
                } else if (p_ctx->organization.attribute.ability.pipe == AMBA_IK_STILL_PIPE) {
                    if (p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_PIPE_HISO) {
                        rval |= img_ctx_get_flow_tbl_list(context_id, (uint32)active_id, &addr);
                        (void)amba_ik_system_memcpy(&p_hiso_flow_tbl_list, &addr, sizeof(void *));

                        if (p_hiso_flow_tbl_list->p_frame_info != NULL) {
                            (void)amba_ik_system_memcpy(p_frame_info, p_hiso_flow_tbl_list->p_frame_info, sizeof(ik_query_frame_info_t));
                        } else {
                            amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_query_frame_info() p_hiso_flow_tbl_list is NULL \n", DC_U, DC_U, DC_U, DC_U, DC_U);
                        }
                    } else {
                        //MisraC
                    }
                } else {
                    // misra
                }
            }
        }
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] can't get ctx_id %d",context_id, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

