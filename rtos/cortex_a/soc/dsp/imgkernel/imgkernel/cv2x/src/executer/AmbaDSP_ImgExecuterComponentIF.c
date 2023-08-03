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
#include "AmbaDSP_ImgArchSystemAPI.h"

static INLINE uint32 exe_clone_iso_config(const amba_ik_configs_entity_t *configs_entity, const amba_ik_attribute_t *attribute, uint32 src_config_id, uint32 dst_config_id)
{
    uint32 rval = IK_OK;
    void *ptr_dst;
    const void *ptr_src;

    if (attribute == NULL) {
        rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memcpy(&ptr_dst, &configs_entity->iso_configs_and_states[dst_config_id], sizeof(void*));
        (void)amba_ik_system_memcpy(&ptr_src, &configs_entity->iso_configs_and_states[src_config_id], sizeof(void*));
        (void)amba_ik_system_memcpy(ptr_dst, ptr_src, sizeof(amba_ik_iso_config_and_state_t));
    }
    return rval;

}

static INLINE uint32 exe_get_next_ring_buffer_index(uint32 buffer_number, uint32 current_index)
{
    uint32 rval;
    rval = (current_index+1U) % buffer_number;
    return rval;
}

static INLINE uint32 exe_get_next_iso_config_index(uint32 buffer_number, const amba_ik_active_config_state_t *active_config_state, uint32 *config_id)
{
    uint32 rval = IK_OK;
    uint32 next_iso_idx = 0;
    if (active_config_state == NULL) {
        rval = IK_ERR_0005;
    } else {
        next_iso_idx = exe_get_next_ring_buffer_index(buffer_number, active_config_state->active_iso_idx);

        if(next_iso_idx == active_config_state->locked_iso_idx) {
            rval = IK_ERR_0000; // TBD
        } else {
            *config_id = next_iso_idx;
        }
    }
    return rval;
}

static inline uint8 not_equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 != compare2) ? 1U : 0U;
}

static INLINE uint32 exe_get_next_iso_cfg_tbl_idx1(uint32 buffer_number,
        const amba_ik_filter_id_list_t *filter_update_flag,
        const amba_ik_iso_config_table_index_t *current_table_index,
        amba_ik_iso_config_table_index_t *new_table_index)
{
    uint32 rval = IK_OK;
    uint8 bool_result; // Coverity complexity hates lots of conditions in a single "If"
    if ((filter_update_flag == NULL) || (current_table_index == NULL) || (new_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        bool_result = not_equal_op_u32(filter_update_flag->video_mctf_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->internal_video_mctf_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->video_mctf_ta_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->video_mctf_ta_y_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->video_mctf_ta_cb_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->video_mctf_ta_cr_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_both_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_noise_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_fir_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_coring_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_coring_index_scale_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_min_coring_result_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_max_coring_result_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_scale_coring_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fnlshpns_both_tdt_updated, 0U);
        if  (bool_result != 0U) {
            new_table_index->ta_edge_reduce = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ta_edge_reduce);
            new_table_index->final_sharpen = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->final_sharpen);
            new_table_index->video_mctf = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->video_mctf);
#if SUPPORT_FUSION
            new_table_index->mctf_alpha = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mctf_alpha);
#endif
        }

        if (filter_update_flag->cdnr_updated!=0U) {
            new_table_index->lnl_tone_curve = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->lnl_tone_curve);
        }

        if (filter_update_flag->fnlshpns_both_tdt_updated!=0U) {
            new_table_index->level_3d_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->level_3d_table);
        }

        bool_result = not_equal_op_u32(filter_update_flag->window_size_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->cfa_window_size_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->calib_warp_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->dzoom_info_updated, 0U) + \
                      /*not_equal_op_u32(filter_update_flag->warp_enable_updated, 0U) + */ \
                      not_equal_op_u32(filter_update_flag->dmy_range_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->flip_mode_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->vin_active_win_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->warp_internal_updated, 0U);

        if (bool_result != 0U) {
            new_table_index->hor_warp = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->hor_warp);
            new_table_index->ver_warp = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ver_warp);
        }

        bool_result = not_equal_op_u32(filter_update_flag->window_size_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->cfa_window_size_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->calib_ca_warp_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->dzoom_info_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->cawarp_enable_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->dmy_range_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->flip_mode_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->vin_active_win_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->cawarp_internal_updated, 0U);
        if (bool_result != 0U) {
            new_table_index->hor_cawarp_red = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->hor_cawarp_red);
            new_table_index->hor_cawarp_blue = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->hor_cawarp_blue);
            new_table_index->ver_cawarp_red = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ver_cawarp_red);
            new_table_index->ver_cawarp_blue = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ver_cawarp_blue);
        }

        if (filter_update_flag->ce_updated!=0U) {
            new_table_index->ce_boost = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ce_boost);
            new_table_index->ce_epsilon = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ce_epsilon);
        }

        if (filter_update_flag->ce_input_table_updated!=0U) {
            new_table_index->ce_input = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ce_input);
        }

        if (filter_update_flag->ce_out_table_updated!=0U) {
            new_table_index->ce_output = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->ce_output);
        }

        if ((filter_update_flag->motion_detect_pos_dep_updated!=0U) ||
            (filter_update_flag->motion_detect_updated!=0U)) {
            new_table_index->md_mctf = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->md_mctf);
        }
#if SUPPORT_FUSION
        //        uint8 mono_vignette_compensation_updated; not support yet
        if (filter_update_flag->mono_dynamic_bpc_updated != 0U) {
            new_table_index->mono_dark_pixel_detect_thd_table = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_dark_pixel_detect_thd_table);
            new_table_index->mono_hot_pixel_detect_thd_table = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_hot_pixel_detect_thd_table);
        }

        if (filter_update_flag->mono_local_exposure_updated != 0U) {
            new_table_index->mono_local_exposure = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_local_exposure);
        }

        if (filter_update_flag->mono_color_correction_reg_updated != 0U) {
            new_table_index->mono_step1_cc_in = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_cc_in);
        }

        if (filter_update_flag->mono_color_correction_updated != 0U) {
            new_table_index->mono_step1_cc_3d = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_cc_3d);
        }

        if (filter_update_flag->mono_tone_curve_updated != 0U) {
            new_table_index->mono_step1_cc_out = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_cc_out);
        }

        bool_result = not_equal_op_u32(filter_update_flag->mono_first_luma_processing_mode_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_advance_spatial_filter_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_fstshpns_fir_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_fstshpns_both_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_fstshpns_noise_updated, 0U);
        if (bool_result != 0U) {
            new_table_index->mono_step1_shp_a_fir1 = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_shp_a_fir1);
            new_table_index->mono_step1_shp_a_fir2 = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_shp_a_fir2);
        }

        if ((filter_update_flag->mono_first_luma_processing_mode_updated!=0U) ||
            (filter_update_flag->mono_advance_spatial_filter_updated!=0U) ||
            (filter_update_flag->mono_fstshpns_coring_updated!=0U)) {
            new_table_index->mono_step1_shp_a_coring = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_shp_a_coring);
        }

        if (filter_update_flag->mono_ce_updated!=0U) {
            new_table_index->mono_ce_boost = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_ce_boost);
            new_table_index->mono_epslion = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_epslion);
        }

        if (filter_update_flag->mono_ce_input_table_updated!=0U) {
            new_table_index->mono_ce_input_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_ce_input_table);
        }

        if (filter_update_flag->mono_ce_out_table_updated!=0U) {
            new_table_index->mono_ce_output_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_ce_output_table);
        }

        if (filter_update_flag->wide_chroma_filter_combine!=0U) {
            new_table_index->mctf_C = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mctf_C);
        }
        if (filter_update_flag->fusion!=0U) {
            new_table_index->mcts_freq_blend = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mcts_freq_blend);
        }
#endif
    }
    return rval;

}

static INLINE uint32 exe_get_next_iso_config_table_indexes(uint32 buffer_number,
        const amba_ik_filter_id_list_t *filter_update_flag,
        const amba_ik_iso_config_table_index_t *current_table_index,
        amba_ik_iso_config_table_index_t *new_table_index)
{
    uint32 rval = IK_OK;
    uint8 bool_result; // Coverity complexity hates lots of conditions in a single "If"
    if ((filter_update_flag == NULL) || (current_table_index == NULL) || (new_table_index == NULL)) {
        rval = IK_ERR_0005;
    } else {
        rval |= exe_get_next_iso_cfg_tbl_idx1(buffer_number, filter_update_flag, current_table_index, new_table_index);
        if (filter_update_flag->fe_tone_curve_updated!=0U) {
            new_table_index->fe_tone_curve = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->fe_tone_curve);
        }

        if ((filter_update_flag->vignette_compensation_updated != 0U) || (filter_update_flag->vignette_compensation_internal_updated != 0U)) {
            new_table_index->vignette = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->vignette);
        }

        if (filter_update_flag->local_exposure_updated!=0U) {
            new_table_index->local_exposure = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->local_exposure);
        }

        if (filter_update_flag->chroma_scale_updated!=0U) {
            new_table_index->chroma_scale = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->chroma_scale);
        }

        if (filter_update_flag->color_correction_reg_updated!=0U) {
            new_table_index->cc_in = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->cc_in);
        }

        if (filter_update_flag->color_correction_updated!=0U) {
            new_table_index->cc_3d = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->cc_3d);
        }

        if (filter_update_flag->tone_curve_updated!=0U) {
            new_table_index->cc_out = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->cc_out);
        }

        if ((filter_update_flag->dynamic_bpc_updated!=0U)/* ||
            (filter_update_flag->deferred_blc_updated!=0U) ||
            (filter_update_flag->static_blc_updated!=0U) ||
            (filter_update_flag->after_ce_wb_gain_updated!=0U) ||
            (filter_update_flag->local_exposure_updated!=0U)*/) {
            new_table_index->hot_pixel_detect_thd_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->hot_pixel_detect_thd_table);
            new_table_index->dark_pixel_detect_thd_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->dark_pixel_detect_thd_table);
        }

        if (filter_update_flag->chroma_median_filter_updated!=0U) {
            new_table_index->chroma_median_k_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->chroma_median_k_table);
        }

        bool_result = not_equal_op_u32(filter_update_flag->first_luma_processing_mode_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->advance_spatial_filter_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fstshpns_fir_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fstshpns_both_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->fstshpns_noise_updated, 0U);
        if (bool_result != 0U) {
            new_table_index->shp_a_fir1 = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->shp_a_fir1);
            new_table_index->shp_a_fir2 = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->shp_a_fir2);
        }

        if ((filter_update_flag->first_luma_processing_mode_updated!=0U) ||
            (filter_update_flag->advance_spatial_filter_updated!=0U) ||
            (filter_update_flag->fstshpns_coring_updated!=0U)) {
            new_table_index->shp_a_coring = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->shp_a_coring);
        }

//        if ((filter_update_flag->aaa_stat_info_updated!=0U) ||
//            (filter_update_flag->af_stat_ex_info_updated!=0U) ||
//            (filter_update_flag->pg_af_stat_ex_info_updated!=0U) ||
//            (filter_update_flag->aaa_float_tile_info_updated!=0U) ||
//            (filter_update_flag->histogram_info_update!=0U) ||
//            (filter_update_flag->histogram_info_pg_update!=0U) ) {
        {
            // batch queue always update
            new_table_index->r2y_batch_queue = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->r2y_batch_queue);
        }

#if SUPPORT_FUSION
        if ((filter_update_flag->motion_detect_pos_dep_updated!=0U) ||
            (filter_update_flag->motion_detect_updated!=0U)) {
            new_table_index->md_mctf = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->md_mctf);
        }
        //        uint8 mono_vignette_compensation_updated; not support yet
        if (filter_update_flag->mono_dynamic_bpc_updated != 0U) {
            new_table_index->mono_dark_pixel_detect_thd_table = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_dark_pixel_detect_thd_table);
            new_table_index->mono_hot_pixel_detect_thd_table = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_hot_pixel_detect_thd_table);
        }

        if (filter_update_flag->mono_local_exposure_updated != 0U) {
            new_table_index->mono_local_exposure = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_local_exposure);
        }

        if (filter_update_flag->mono_color_correction_reg_updated != 0U) {
            new_table_index->mono_step1_cc_in = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_cc_in);
        }

        if (filter_update_flag->mono_color_correction_updated != 0U) {
            new_table_index->mono_step1_cc_3d = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_cc_3d);
        }

        if (filter_update_flag->mono_tone_curve_updated != 0U) {
            new_table_index->mono_step1_cc_out = (uint8) exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_cc_out);
        }

        bool_result = not_equal_op_u32(filter_update_flag->mono_first_luma_processing_mode_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_advance_spatial_filter_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_fstshpns_fir_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_fstshpns_both_updated, 0U) + \
                      not_equal_op_u32(filter_update_flag->mono_fstshpns_noise_updated, 0U);
        if(bool_result != 0U) {
            new_table_index->mono_step1_shp_a_fir1 = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_shp_a_fir1);
            new_table_index->mono_step1_shp_a_fir2 = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_shp_a_fir2);
        }

        if ((filter_update_flag->mono_first_luma_processing_mode_updated!=0U) ||
            (filter_update_flag->mono_advance_spatial_filter_updated!=0U) ||
            (filter_update_flag->mono_fstshpns_coring_updated!=0U)) {
            new_table_index->mono_step1_shp_a_coring = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_step1_shp_a_coring);
        }

        if (filter_update_flag->mono_ce_updated!=0U) {
            new_table_index->mono_ce_boost = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_ce_boost);
            new_table_index->mono_epslion = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_epslion);
        }

        if (filter_update_flag->mono_ce_input_table_updated!=0U) {
            new_table_index->mono_ce_input_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_ce_input_table);
        }

        if (filter_update_flag->mono_ce_out_table_updated!=0U) {
            new_table_index->mono_ce_output_table = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mono_ce_output_table);
        }
        if (filter_update_flag->wide_chroma_filter_combine!=0U) {
            new_table_index->mctf_C = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mctf_C);
        }

        if (filter_update_flag->fusion!=0U) {
            new_table_index->mcts_freq_blend = (uint8)exe_get_next_ring_buffer_index(buffer_number, current_table_index->mcts_freq_blend);
        }
#endif
    }
    return rval;
}

static INLINE uint32 exe_compute_mipi(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    rval |= ikc_exe_iso_cfg_share_first(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_mipi_first(filters, &iso_config->calib, cmds);
    rval |= ikc_exe_iso_cfg_color_mipi_first(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_first(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_first(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_first(filters, &iso_config->extra, 0);
    rval |= ikc_exe_iso_cfg_motion_first(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 2U);
    rval |= ikc_exe_iso_cfg_extra2_first(filters, &iso_config->extra2);
    return rval;
}

static INLINE uint32 exe_compute_first_iso_config_and_tables(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    rval |= ikc_exe_iso_cfg_share_first(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_first(filters, &iso_config->calib, cmds);
    rval |= ikc_exe_iso_cfg_color_first(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_first(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_first(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_first(filters, &iso_config->extra, 0);
    rval |= ikc_exe_iso_cfg_motion_first(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 2U);
    rval |= ikc_exe_iso_cfg_extra2_first(filters, &iso_config->extra2);
    return rval;
}

static INLINE uint32 exe_linear_cmpt_fst_iso_fusion(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    rval |= ikc_exe_iso_cfg_share_first(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_first(filters, &iso_config->calib, cmds);
    rval |= ikc_exe_iso_cfg_color_first(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_first(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_first(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_first(filters, &iso_config->extra, 0);
    rval |= ikc_exe_iso_cfg_motion_first(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 2U);
    rval |= ikc_exe_iso_cfg_extra2_first(filters, &iso_config->extra2);
#if SUPPORT_FUSION
    ik_cv2_liso_cfg_t *iso_config_cpy;
    if(iso_config != NULL) {
        (void)amba_ikc_system_memcpy(&iso_config_cpy, &iso_config, sizeof(ik_cv2_liso_cfg_t*));
    }
//    rval |= ikc_exe_iso_cfg_vin_stat_first(filters, &iso_config->vin_stat_crop, cmds);
    rval |= ikc_exe_iso_cfg_calib_mono_first(filters, &iso_config_cpy->mono.liso.calib, cmds);
    rval |= ikc_exe_iso_cfg_color_mono_first(filters, &iso_config_cpy->mono.liso.color);
    rval |= ikc_exe_iso_cfg_liso_mono_first(filters, &iso_config_cpy->mono.liso.raw2yuv, cmds);
    rval |= ikc_exe_iso_cfg_extra_mono_first(filters, &iso_config_cpy->mono.liso.extra, cmds, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_mono_first(filters, &iso_config_cpy->mono.liso.exp0, cmds);
    rval |= ikc_exe_iso_cfg_step4_mono_first(filters, &iso_config_cpy->mono.smooth);
    rval |= ikc_exe_iso_cfg_yuv_mono_first(filters, &iso_config_cpy->mono.yuv);
    rval |= ikc_exe_iso_cfg_mctf_mono_first(filters, &iso_config_cpy->mono.mctf, &iso_config_cpy->flow);
#endif
    return rval;
}

static INLINE uint32 exe_compute_hiso(const ik_cv2_hiso_cfg_t *iso_config, const amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;

    rval |= ikc_exe_iso_cfg_hiso_share(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_hiso_calib(filters, &iso_config->common, cmds);
    rval |= ikc_exe_iso_cfg_hiso_color(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_hiso_step1(filters, &iso_config->step01);
    rval |= ikc_exe_iso_cfg_hiso_step2(filters, &iso_config->step02);
    rval |= ikc_exe_iso_cfg_hiso_step3(filters, &iso_config->step03);
    rval |= ikc_exe_iso_cfg_hiso_step4(filters, &iso_config->step04);
    rval |= ikc_exe_iso_cfg_hiso_step5(filters, &iso_config->step05);
    rval |= ikc_exe_iso_cfg_hiso_step6(filters, &iso_config->step06);
    rval |= ikc_exe_iso_cfg_hiso_step7(filters, &iso_config->step07);
    rval |= ikc_exe_iso_cfg_hiso_step8(filters, &iso_config->step08);
    rval |= ikc_exe_iso_cfg_hiso_step9(filters, &iso_config->step09);
    rval |= ikc_exe_iso_cfg_hiso_step11(filters, &iso_config->step11);
    rval |= ikc_exe_iso_cfg_hiso_step12(filters, &iso_config->step12);
    rval |= ikc_exe_iso_cfg_hiso_step13(filters, &iso_config->step13);
    rval |= ikc_exe_iso_cfg_hiso_mctf(filters, &iso_config->mctf);
    rval |= ikc_exe_iso_cfg_hiso_flow(filters, &iso_config->flow);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 0, 2U);
//    uint32 ikc_exe_iso_cfg_hiso_vwarp(amba_ik_filter_t *p_filters, cv2_liso_param_vwarp_t *p_cfg_vwarp);
    return rval;
}

static INLINE uint32 exe_cmpt_upd_mipi(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    // misraC fix
    (void)amba_ik_system_memcpy(filters, filters, sizeof(uint8));
    rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);

    return rval;
}

static INLINE uint32 exe_cmpt_upd_iso(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_share_update(filters, &iso_config->share, cmds);
    }
    rval |= ikc_exe_iso_cfg_calib_update(filters, &iso_config->calib, cmds, 0);
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_color_update(filters, &iso_config->color, cmds);
        rval |= ikc_exe_iso_cfg_liso_update(filters, &iso_config->liso, cmds);
        rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);
        rval |= ikc_exe_iso_cfg_extra_update(filters, &iso_config->extra, 0);
        rval |= ikc_exe_iso_cfg_motion_update(filters, &iso_config->motion, cmds);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 0, 0U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 0, 1U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 0, 2U);
        rval |= ikc_exe_iso_cfg_extra2_update(filters, &iso_config->extra2);
    }
//    rval |= ikc_exe_iso_cfg_vwarp_update(filters, &iso_config->vwarp, cmds);

    return rval;
}

static INLINE uint32 exe_cmpt_upd_iso_fusion(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    if(iso_config != NULL) {
        //TBD
    }
    if(cmds != NULL) {
        //TBD
        uint32 tmp = cmds->idsp_cmd_number;
        cmds->idsp_cmd_number = tmp;
    }
    if(filters != NULL) {
        //TBD
        uint32 tmp = filters->input_param.ctx_buf.normalized_wb_gain.gain_g;
        filters->input_param.ctx_buf.normalized_wb_gain.gain_g = tmp;
    }
#if SUPPORT_FUSION
    ik_cv2_liso_cfg_t *iso_config_cpy;
    if(iso_config != NULL) {
        (void)amba_ikc_system_memcpy(&iso_config_cpy, &iso_config, sizeof(ik_cv2_liso_cfg_t*));
    }
    rval = ikc_exe_iso_cfg_share_update(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_update(filters, &iso_config->calib, cmds, 1);
    rval |= ikc_exe_iso_cfg_color_update(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_update(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_update(filters, &iso_config->extra, 1);
    rval |= ikc_exe_iso_cfg_motion_update(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 0, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 0, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 0, 2U);
    rval |= ikc_exe_iso_cfg_extra2_update(filters, &iso_config->extra2);
//    rval |= ikc_exe_iso_cfg_vin_stat_update(filters, &iso_config->vin_stat_crop, cmds);
    rval |= ikc_exe_iso_cfg_calib_mono_update(filters, &iso_config_cpy->mono.liso.calib, cmds);
    rval |= ikc_exe_iso_cfg_color_mono_update(filters, &iso_config_cpy->mono.liso.color, cmds);
    rval |= ikc_exe_iso_cfg_liso_mono_update(filters, &iso_config_cpy->mono.liso.raw2yuv, cmds);
    rval |= ikc_exe_iso_cfg_extra_mono_update(filters, &iso_config_cpy->mono.liso.extra);
    rval |= ikc_exe_iso_cfg_single_exp_mono_update(filters, &iso_config_cpy->mono.liso.exp0);
    rval |= ikc_exe_iso_cfg_yuv_mono_update(filters, &iso_config_cpy->mono.yuv);
    rval |= ikc_exe_iso_cfg_mctf_mono_update(filters, &iso_config_cpy->mono.mctf, &iso_config_cpy->flow);
#endif
    return rval;
}

static INLINE uint32 exe_cmpt_upd_iso_1exp(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_share_update(filters, &iso_config->share, cmds);
    }
    rval |= ikc_exe_iso_cfg_calib_update(filters, &iso_config->calib, cmds, 1);
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_color_update(filters, &iso_config->color, cmds);
        rval |= ikc_exe_iso_cfg_liso_update(filters, &iso_config->liso, cmds);
        rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);
        rval |= ikc_exe_iso_cfg_extra_update(filters, &iso_config->extra, 1);
        rval |= ikc_exe_iso_cfg_motion_update(filters, &iso_config->motion, cmds);
//        rval |= ikc_exe_iso_cfg_vwarp_update(filters, &iso_config->vwarp, cmds);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 1, 0U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 1, 1U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 1, 2U);
        rval |= ikc_exe_iso_cfg_extra2_update(filters, &iso_config->extra2);
    }
    return rval;
}

static INLINE uint32 exe_cmpt_upd_iso_2exp(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_share_update(filters, &iso_config->share, cmds);
    }
    rval |= ikc_exe_iso_cfg_calib_update(filters, &iso_config->calib, cmds, 2);
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_color_update(filters, &iso_config->color, cmds);
        rval |= ikc_exe_iso_cfg_liso_update(filters, &iso_config->liso, cmds);
        rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);
        rval |= ikc_exe_iso_cfg_extra_update(filters, &iso_config->extra, 2);
        rval |= ikc_exe_iso_cfg_motion_update(filters, &iso_config->motion, cmds);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 2, 0U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 2, 1U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 2, 2U);
        rval |= ikc_exe_iso_cfg_extra2_update(filters, &iso_config->extra2);
    }
//    rval |= ikc_exe_iso_cfg_vwarp_update(filters, &iso_config->vwarp, cmds);
    return rval;
}

static INLINE uint32 exe_cmpt_upd_iso_3exp(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval = IK_OK;
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_share_update(filters, &iso_config->share, cmds);
    }
    rval |= ikc_exe_iso_cfg_calib_update(filters, &iso_config->calib, cmds, 3);
    if (filters->input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) {
        rval |= ikc_exe_iso_cfg_color_update(filters, &iso_config->color, cmds);
        rval |= ikc_exe_iso_cfg_liso_update(filters, &iso_config->liso, cmds);
        rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);
        rval |= ikc_exe_iso_cfg_extra_update(filters, &iso_config->extra, 3);
        rval |= ikc_exe_iso_cfg_motion_update(filters, &iso_config->motion, cmds);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 3, 0U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 3, 1U);
        rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 3, 2U);
        rval |= ikc_exe_iso_cfg_extra2_update(filters, &iso_config->extra2);
    }
//    rval |= ikc_exe_iso_cfg_vwarp_update(filters, &iso_config->vwarp, cmds);
    return rval;
}

static INLINE uint32 exe_linear_cmpt_fst_iso(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters, int32 exp_num)
{
    uint32 rval;
    rval = ikc_exe_iso_cfg_share_first(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_first(filters, &iso_config->calib, cmds);
    rval |= ikc_exe_iso_cfg_color_first(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_first(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_first(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_first(filters, &iso_config->extra, exp_num);
    rval |= ikc_exe_iso_cfg_motion_first(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, exp_num, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, exp_num, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, exp_num, 2U);
    rval |= ikc_exe_iso_cfg_extra2_first(filters, &iso_config->extra2);
//    rval |= ikc_exe_iso_cfg_vwarp_first(filters, &iso_config->vwarp, cmds);

    return rval;
}

static INLINE uint32 exe_linear_cmpt_fst_iso_1exp(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval;
    rval = exe_linear_cmpt_fst_iso(iso_config, cmds, filters, 1);

    return rval;
}

static INLINE uint32 exe_linear_cmpt_fst_iso_2exp(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval;
    rval = exe_linear_cmpt_fst_iso(iso_config, cmds, filters, 2);

    return rval;
}

static INLINE uint32 exe_linear_cmpt_fst_iso_3exp(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval;
    rval = exe_linear_cmpt_fst_iso(iso_config, cmds, filters, 3);

    return rval;
}

static INLINE uint32 exe_cmpt_invalid_iso_cfg_utility(const ik_cv2_liso_cfg_t *iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval;
    if(iso_config != NULL) {
        //TBD
    }
    if(cmds != NULL) {
        //TBD
        uint32 tmp = cmds->idsp_cmd_number;
        cmds->idsp_cmd_number = tmp;
    }
    if(filters != NULL) {
        //TBD
        uint32 tmp = filters->input_param.ctx_buf.normalized_wb_gain.gain_g;
        filters->input_param.ctx_buf.normalized_wb_gain.gain_g = tmp;
    }
    rval = IK_ERR_0000;
    return rval;
}

#if PRE_NN_PROCESS
static INLINE uint32 exe_pre_nn_fst_iso(const ik_cv2_liso_cfg_t *p_iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval;
    iso_single_exposure_data_t *p_cfg_single_exp;
    ik_frontend_wb_gain_t fe_wbgain;
    ik_cv2_liso_cfg_t *iso_config;
    (void)amba_ik_system_memcpy(&iso_config, &p_iso_config, sizeof(ik_cv2_liso_cfg_t*));
    p_cfg_single_exp = &iso_config->exposure[0];
    rval = ikc_exe_iso_cfg_pre_nn_first(filters, &iso_config->pre_nn);;
    rval = ikc_exe_iso_cfg_share_first(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_first(filters, &iso_config->calib, cmds);
    rval |= ikc_exe_iso_cfg_color_first(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_first(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_first(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_first(filters, &iso_config->extra, 2);
    rval |= ikc_exe_iso_cfg_motion_first(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 2, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 2, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_first(filters, &iso_config->exposure[0], cmds, 2, 2U);
    rval |= ikc_exe_iso_cfg_extra2_first(filters, &iso_config->extra2);

    iso_config->flow.is_pre_nn_proc_enabled = 1U;

    // update for pass2 wb/blc
    {
        const ik_wb_gain_t *p_user_wb_gain = &filters->input_param.ctx_buf.pre_nn_step2_before_ce_gain;
        if ((p_user_wb_gain->gain_r == 0UL) || (p_user_wb_gain->gain_g == 0UL) || (p_user_wb_gain->gain_b == 0UL)) {
            amba_ikc_system_print_uint32_5("[IK] Warning. Gain %d, %d, %d. one or more are zeros", p_user_wb_gain->gain_r, p_user_wb_gain->gain_g, p_user_wb_gain->gain_b, DC_U, DC_U);
            rval = IK_ERR_0109;
        } else {
            iso_config->extra.before_ce_digital_gain[0] = p_user_wb_gain->gain_r;
            iso_config->extra.before_ce_digital_gain[1] = p_user_wb_gain->gain_g;
            iso_config->extra.before_ce_digital_gain[2] = p_user_wb_gain->gain_g;
            iso_config->extra.before_ce_digital_gain[3] = p_user_wb_gain->gain_b;
        }
        iso_config->extra.enable_before_ce_gain = 1U;
    }

    (void)amba_ik_system_memcpy(&fe_wbgain, &filters->input_param.ctx_buf.pre_nn_step2_wb, sizeof(ik_frontend_wb_gain_t));
    fe_wbgain.shutter_ratio = filters->input_param.exp0_frontend_wb_gain.shutter_ratio;
    p_cfg_single_exp[0].front_end_digital_gain[0] = (fe_wbgain.r_gain  *1024UL / fe_wbgain.shutter_ratio);
    p_cfg_single_exp[0].front_end_digital_gain[1] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);
    p_cfg_single_exp[0].front_end_digital_gain[2] = (fe_wbgain.b_gain  *1024UL / fe_wbgain.shutter_ratio);
    p_cfg_single_exp[0].front_end_digital_gain[3] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);
    fe_wbgain.shutter_ratio = filters->input_param.exp1_frontend_wb_gain.shutter_ratio;
    p_cfg_single_exp[1].front_end_digital_gain[0] = (fe_wbgain.r_gain  *1024UL / fe_wbgain.shutter_ratio);
    p_cfg_single_exp[1].front_end_digital_gain[1] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);
    p_cfg_single_exp[1].front_end_digital_gain[2] = (fe_wbgain.b_gain  *1024UL / fe_wbgain.shutter_ratio);
    p_cfg_single_exp[1].front_end_digital_gain[3] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);

    p_cfg_single_exp[0].front_end_input_black_level_offset[0] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][0];
    p_cfg_single_exp[0].front_end_input_black_level_offset[1] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][1];
    p_cfg_single_exp[0].front_end_input_black_level_offset[2] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][2];
    p_cfg_single_exp[0].front_end_input_black_level_offset[3] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][3];

    p_cfg_single_exp[1].front_end_input_black_level_offset[0] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][0];
    p_cfg_single_exp[1].front_end_input_black_level_offset[1] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][1];
    p_cfg_single_exp[1].front_end_input_black_level_offset[2] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][2];
    p_cfg_single_exp[1].front_end_input_black_level_offset[3] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][3];

//    p_cfg_single_exp[0].update_black_level_offset = 1U;
//    p_cfg_single_exp[1].update_black_level_offset = 1U;
//    p_cfg_single_exp[0].update_digital_gain = 1U;
//    p_cfg_single_exp[1].update_digital_gain = 1U;
    p_cfg_single_exp[0].data1 = (1U<<13U)|(1U<<12U);
    p_cfg_single_exp[1].data1 = (1U<<13U)|(1U<<12U);

    iso_config->extra.hdr_blend_delta_t0 = filters->input_param.ctx_buf.pre_nn_step2_delta_t;
    iso_config->extra.hdr_blend_t0_offset = filters->input_param.ctx_buf.pre_nn_step2_t0_offset;

    return rval;
}
static INLINE uint32 exe_pre_nn_upd_iso(const ik_cv2_liso_cfg_t *p_iso_config, amba_ik_setup_cmds_t *cmds, amba_ik_filter_t *filters)
{
    uint32 rval;
    ik_frontend_wb_gain_t fe_wbgain;
    iso_single_exposure_data_t *p_cfg_single_exp;
    ik_cv2_liso_cfg_t *iso_config;
    (void)amba_ik_system_memcpy(&iso_config, &p_iso_config, sizeof(ik_cv2_liso_cfg_t*));
    p_cfg_single_exp = &iso_config->exposure[0];
    rval = ikc_exe_iso_cfg_pre_nn_update(filters, &iso_config->pre_nn);;
    rval = ikc_exe_iso_cfg_share_update(filters, &iso_config->share, cmds);
    rval |= ikc_exe_iso_cfg_calib_update(filters, &iso_config->calib, cmds, 2);
    rval |= ikc_exe_iso_cfg_color_update(filters, &iso_config->color, cmds);
    rval |= ikc_exe_iso_cfg_liso_update(filters, &iso_config->liso, cmds);
    rval |= ikc_exe_iso_cfg_flow_update(filters, &iso_config->flow, cmds);
    rval |= ikc_exe_iso_cfg_extra_update(filters, &iso_config->extra, 2);
    rval |= ikc_exe_iso_cfg_motion_update(filters, &iso_config->motion, cmds);
    rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 2, 0U);
    rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 2, 1U);
    rval |= ikc_exe_iso_cfg_single_exp_update(filters, &iso_config->exposure[0], cmds, 2, 2U);
    rval |= ikc_exe_iso_cfg_extra2_update(filters, &iso_config->extra2);

//    iso_config->flow.is_pre_nn_proc_enabled = 1U;

    // update for pass2 wb/blc

    if((filters->update_flags.liso.exp0_fe_static_blc_updated == 1U)||\
       (filters->update_flags.liso.exp1_fe_static_blc_updated == 1U)|| \
       (filters->update_flags.liso.exp0_fe_wb_gain_updated == 1U)|| \
       (filters->update_flags.liso.exp1_fe_wb_gain_updated == 1U)|| \
       (filters->update_flags.liso.hdr_blend_updated == 1U)) {
        {
            const ik_wb_gain_t *p_user_wb_gain = &filters->input_param.ctx_buf.pre_nn_step2_before_ce_gain;
            if ((p_user_wb_gain->gain_r == 0UL) || (p_user_wb_gain->gain_g == 0UL) || (p_user_wb_gain->gain_b == 0UL)) {
                amba_ikc_system_print_uint32_5("[IK] Warning. Gain %d, %d, %d. one or more are zeros", p_user_wb_gain->gain_r, p_user_wb_gain->gain_g, p_user_wb_gain->gain_b, DC_U, DC_U);
                rval = IK_ERR_0109;
            } else {
                iso_config->extra.before_ce_digital_gain[0] = p_user_wb_gain->gain_r;
                iso_config->extra.before_ce_digital_gain[1] = p_user_wb_gain->gain_g;
                iso_config->extra.before_ce_digital_gain[2] = p_user_wb_gain->gain_g;
                iso_config->extra.before_ce_digital_gain[3] = p_user_wb_gain->gain_b;
            }
            iso_config->extra.enable_before_ce_gain = 1U;
        }

        //p_cfg_single_exp[0].update_digital_gain = 1;
        //p_cfg_single_exp[1].update_digital_gain = 1;
        //p_cfg_single_exp[0].update_black_level_offset = 1;
        //p_cfg_single_exp[1].update_black_level_offset = 1;

        p_cfg_single_exp[0].data1 = p_cfg_single_exp[0].data1|(1U<<13U)|(1U<<12U); // Rewirte update_digital_gain = 1 and update_black_level_offset = 1 by caluating the offset from the struct cv2_single_exposure_data_t
        p_cfg_single_exp[1].data1 = p_cfg_single_exp[1].data1|(1U<<13U)|(1U<<12U);

        (void)amba_ik_system_memcpy(&fe_wbgain, &filters->input_param.ctx_buf.pre_nn_step2_wb, sizeof(ik_frontend_wb_gain_t));
        fe_wbgain.shutter_ratio = filters->input_param.exp0_frontend_wb_gain.shutter_ratio;
        p_cfg_single_exp[0].front_end_digital_gain[0] = (fe_wbgain.r_gain  *1024UL / fe_wbgain.shutter_ratio);
        p_cfg_single_exp[0].front_end_digital_gain[1] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);
        p_cfg_single_exp[0].front_end_digital_gain[2] = (fe_wbgain.b_gain  *1024UL / fe_wbgain.shutter_ratio);
        p_cfg_single_exp[0].front_end_digital_gain[3] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);
        fe_wbgain.shutter_ratio = filters->input_param.exp1_frontend_wb_gain.shutter_ratio;
        p_cfg_single_exp[1].front_end_digital_gain[0] = (fe_wbgain.r_gain  *1024UL / fe_wbgain.shutter_ratio);
        p_cfg_single_exp[1].front_end_digital_gain[1] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);
        p_cfg_single_exp[1].front_end_digital_gain[2] = (fe_wbgain.b_gain  *1024UL / fe_wbgain.shutter_ratio);
        p_cfg_single_exp[1].front_end_digital_gain[3] = (fe_wbgain.g_gain  *1024UL / fe_wbgain.shutter_ratio);

        p_cfg_single_exp[0].front_end_input_black_level_offset[0] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][0];
        p_cfg_single_exp[0].front_end_input_black_level_offset[1] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][1];
        p_cfg_single_exp[0].front_end_input_black_level_offset[2] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][2];
        p_cfg_single_exp[0].front_end_input_black_level_offset[3] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[0][3];

        p_cfg_single_exp[1].front_end_input_black_level_offset[0] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][0];
        p_cfg_single_exp[1].front_end_input_black_level_offset[1] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][1];
        p_cfg_single_exp[1].front_end_input_black_level_offset[2] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][2];
        p_cfg_single_exp[1].front_end_input_black_level_offset[3] = (uint32)filters->input_param.ctx_buf.pre_nn_step2_action_add[1][3];

        iso_config->extra.hdr_blend_delta_t0 = filters->input_param.ctx_buf.pre_nn_step2_delta_t;
        iso_config->extra.hdr_blend_t0_offset = filters->input_param.ctx_buf.pre_nn_step2_t0_offset;
    }

    return rval;
}
#endif

static INLINE void get_first_iso_config_utilities_video(const ik_ability_t *ability, amba_ik_exe_first_iso_utilities_t *utilities)
{
    if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR) {
        utilities->compute_first_liso_config_and_tables = exe_compute_first_iso_config_and_tables;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_1exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_2exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_3exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR_MD) {
        utilities->compute_first_liso_config_and_tables = exe_compute_first_iso_config_and_tables;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE_MD) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_1exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_2exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_3exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_Y2Y) {
        utilities->compute_first_liso_config_and_tables = exe_compute_first_iso_config_and_tables;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_Y2Y_MD) {
        utilities->compute_first_liso_config_and_tables = exe_compute_first_iso_config_and_tables;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE_MD) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_fusion;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE) {
        utilities->compute_first_liso_config_and_tables = exe_linear_cmpt_fst_iso_fusion;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_Y2Y_MIPI) {
        utilities->compute_first_liso_config_and_tables = exe_compute_mipi;
    }
#if PRE_NN_PROCESS
    else if(ability->video_pipe == AMBA_IK_VIDEO_PRE_NN_PROCESS) {
        utilities->compute_first_liso_config_and_tables = exe_pre_nn_fst_iso;
    }
#endif
    else {
        utilities->compute_first_liso_config_and_tables = exe_cmpt_invalid_iso_cfg_utility;
        amba_ik_system_print_uint32_5("[IK] video did not support this video_pipe %d", ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
    }

}

static INLINE void exe_get_first_iso_config_utilities(const ik_ability_t *ability, amba_ik_exe_first_iso_utilities_t *utilities)
{
    if (ability->pipe == AMBA_IK_PIPE_VIDEO) {
        get_first_iso_config_utilities_video(ability, utilities);
    } else if (ability->pipe == AMBA_IK_PIPE_STILL) {
        switch (ability->still_pipe) {
        case AMBA_IK_STILL_LISO:
            utilities->compute_first_liso_config_and_tables = exe_compute_first_iso_config_and_tables;
            break;
        case AMBA_IK_STILL_HISO:
            utilities->compute_first_hiso_config_and_tables = exe_compute_hiso;
            break;
        case AMBA_IK_VIDEO_MAX:
        default:
            utilities->compute_first_liso_config_and_tables = exe_cmpt_invalid_iso_cfg_utility;
            amba_ik_system_print_uint32_5("[IK][ERROR] video did not support this still_pipe %d", ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
            break;
        }
    } else {
        amba_ik_system_print_uint32_5("[IK] did not support this pipe %d", ability->pipe, DC_U, DC_U, DC_U, DC_U);
    }
}
static INLINE void get_update_iso_config_utilities_video1(const ik_ability_t *ability, amba_ik_exe_update_iso_utilities_t *utilities)
{
    if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_1exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_2exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_3exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR_MD) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE_MD) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_1exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_2exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_3exp;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_Y2Y) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_Y2Y_MD) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE_MD) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_fusion;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_iso_fusion;
    } else if(ability->video_pipe == AMBA_IK_VIDEO_Y2Y_MIPI) {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_upd_mipi;
    }
#if PRE_NN_PROCESS
    else if(ability->video_pipe == AMBA_IK_VIDEO_PRE_NN_PROCESS) {
        utilities->compute_updated_iso_config_and_tables = exe_pre_nn_upd_iso;
    }
#endif
    else {
        utilities->compute_updated_iso_config_and_tables = exe_cmpt_invalid_iso_cfg_utility;
        amba_ik_system_print_uint32_5("[IK] video did not support this video_pipe %d", ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
    }

}

static INLINE void exe_get_update_iso_config_utilities(const ik_ability_t *ability, amba_ik_exe_update_iso_utilities_t *utilities)
{
    if (ability->pipe == AMBA_IK_PIPE_VIDEO) {
        get_update_iso_config_utilities_video1(ability, utilities);
    } else {
        amba_ik_system_print_uint32_5("[IK] did not support this pipe %d", ability->pipe, DC_U, DC_U, DC_U, DC_U);
    }

}
static INLINE uint32 exe_hook_iso_config_tables(ik_cv2_liso_cfg_t *iso_config, const amba_ik_setup_cmds_t *cmds, const amba_ik_iso_config_table_index_t *table_index, const intptr *iso_tables, uint32 eis_mode)
{
    const amba_ik_iso_config_tables_t *p_iso_cfg_table_start;
    const amba_ik_iso_config_tables_t *p_cfg_table;
    void* tmp_addr;
    const void* tmp_add2;
    const uint8 *ptr_u8;
    const uint16 *ptr_u16;
    const uint32 *ptr_u32;
    const int16 *ptr_int16;
    iso_asf_data_t *p_shp_a_asf;  //To fix coverity, asf and sharpen data share the same memory
    const iso_sharpen_data_t  *p_shp_a_shp;

    (void)amba_ik_system_memcpy(&p_iso_cfg_table_start, iso_tables, sizeof(amba_ik_iso_config_tables_t *));

    if (cmds == NULL) {
        amba_ik_system_print_str_5("[IK] cmds is NULL", NULL, NULL, NULL, NULL, NULL);
    }
    if (eis_mode != STAGE_2_FOR_EIS_EXECUTE) {// regular post-exe or eis_stage_1.
        p_cfg_table = &p_iso_cfg_table_start[table_index->fe_tone_curve];
        ptr_u8 = p_cfg_table->fe_tone_curve;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->extra.exp0_decompand_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->vignette];
        ptr_u8 = p_cfg_table->vignette;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.vignette_gain, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->local_exposure];
        ptr_u16 = p_cfg_table->local_exposure;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->color.place_holder[3], &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->chroma_scale];
        ptr_u16 = p_cfg_table->chroma_scale;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->color.chroma_scale.gain_curver_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->cc_in];
        ptr_u8 = p_cfg_table->cc_in;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->color.cc_reg, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->cc_3d];
        ptr_u8 = p_cfg_table->cc_3d;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->color.cc_3d, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->cc_out];
        ptr_u8 = p_cfg_table->cc_out;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->color.cc_out, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->dark_pixel_detect_thd_table];
        ptr_u32 = p_cfg_table->dark_pixel_detect_thd_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.dark_pixel_threshold, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->hot_pixel_detect_thd_table];
        ptr_u32 = p_cfg_table->hot_pixel_detect_thd_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.hot_pixel_threshold, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->chroma_median_k_table];
        ptr_u16 = p_cfg_table->chroma_median_k_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.chroma_median_k_table, &tmp_add2, sizeof(uint32));
#if 0 //Original Code
        p_cfg_table = &p_iso_cfg_table_start[table_index->shp_a_fir1];
        ptr_u8 = p_cfg_table->shp_a_fir1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.shp_a.asf.fir1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->shp_a_fir2];
        ptr_u8 = p_cfg_table->shp_a_fir2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.shp_a.asf.fir2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->shp_a_coring];
        ptr_u8 = p_cfg_table->shp_a_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.shp_a.asf.coring, &tmp_add2, sizeof(uint32));
#else //To fix coverity
        p_shp_a_shp = &iso_config->liso.shp_a_shp;
        (void)amba_ik_system_memcpy(&p_shp_a_asf, &p_shp_a_shp, sizeof(iso_asf_data_t*));

        p_cfg_table = &p_iso_cfg_table_start[table_index->shp_a_fir1];
        ptr_u8 = p_cfg_table->shp_a_fir1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&p_shp_a_asf->fir1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->shp_a_fir2];
        ptr_u8 = p_cfg_table->shp_a_fir2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&p_shp_a_asf->fir2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->shp_a_coring];
        ptr_u8 = p_cfg_table->shp_a_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&p_shp_a_asf->coring, &tmp_add2, sizeof(uint32));
#endif
        p_cfg_table = &p_iso_cfg_table_start[table_index->r2y_batch_queue];
        ptr_u8 = p_cfg_table->r2y_batch_queue;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->liso.batch_cmd_queue_daddr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->final_sharpen];
        ptr_u8 = p_cfg_table->final_sharpen;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->flow.final_sharpen, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->video_mctf];
        ptr_u8 = p_cfg_table->video_mctf;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->flow.video_mctf, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ta_edge_reduce];
        ptr_u8 = p_cfg_table->ta_edge_reduce;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->flow.temporal_adjust_edge_reduce, &tmp_add2, sizeof(uint32));
    }

    if (eis_mode != STAGE_1_FOR_EIS_EXECUTE) {// regular post-exe or eis_stage_2.
        p_cfg_table = &p_iso_cfg_table_start[table_index->hor_warp];
        ptr_int16 = p_cfg_table->warp_hor;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.geo_warp_control.warp_horizontal_table_address, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ver_warp];
        ptr_int16 = p_cfg_table->warp_ver;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.geo_warp_control.warp_vertical_table_address, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->hor_cawarp_red];
        ptr_int16 = p_cfg_table->ca_warp_hor_red;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.ca_warp_control.warp_horizontal_table_addr_red, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->hor_cawarp_blue];
        ptr_int16 = p_cfg_table->ca_warp_hor_blue;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.ca_warp_control.warp_horizontal_table_addr_blue, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ver_cawarp_red];
        ptr_int16 = p_cfg_table->ca_warp_ver_red;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.ca_warp_control.warp_vertical_table_addr_red, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ver_cawarp_blue];
        ptr_int16 = p_cfg_table->ca_warp_ver_blue;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->calib.ca_warp_control.warp_vertical_table_addr_blue, &tmp_add2, sizeof(uint32));
    }

    if (eis_mode != STAGE_2_FOR_EIS_EXECUTE) {// regular post-exe or eis_stage_1.
        //ce tables
        p_cfg_table = &p_iso_cfg_table_start[table_index->ce_boost];
        ptr_u8 = p_cfg_table->ce_boost_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->extra.ce_boost_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ce_coring];
        ptr_u32 = p_cfg_table->ce_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->extra.ce_coring_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ce_input];
        ptr_u32 = p_cfg_table->ce_input_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->extra.ce_input_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ce_output];
        ptr_u16 = p_cfg_table->ce_out_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->extra.ce_output_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->ce_epsilon];
        ptr_u8 = p_cfg_table->ce_epsilon_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->extra.ce_epsilon_table_addr, &tmp_add2, sizeof(uint32));

        //md tables
        p_cfg_table = &p_iso_cfg_table_start[table_index->md_smooth_coring];
        ptr_u8 = p_cfg_table->md_smooth_coring_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->motion.smooth_coring, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->md_asf_coring];
        ptr_u8 = p_cfg_table->md_asf_coring_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->motion.MoASF_coring, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->md_asf_fir1];
        ptr_u8 = p_cfg_table->md_asf_fir1_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->motion.MoASF_fir1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->md_asf_fir2];
        ptr_u8 = p_cfg_table->md_asf_fir2_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->motion.MoASF_fir2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->md_mctf];
        ptr_u8 = p_cfg_table->md_mctf_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->motion.md_mctf, &tmp_add2, sizeof(uint32));

#if SUPPORT_FUSION
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_vignette];
        ptr_u8 = p_cfg_table->mono_vignette;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.calib.vignette_gain, &tmp_add2, sizeof(uint32));

        //    uint8 mono_step1_cc_in;
        //    uint8 mono_step1_cc_3d;
        //    uint8 mono_step1_cc_out;
        //mono
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_cc_in];
        ptr_u8 = p_cfg_table->mono_step1_cc_in;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.color.cc_reg, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_cc_3d];
        ptr_u8 = p_cfg_table->mono_step1_cc_3d;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.color.cc_3d, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_cc_out];
        ptr_u8 = p_cfg_table->mono_step1_cc_out;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.color.cc_out, &tmp_add2, sizeof(uint32));

        //    uint8 mono_dark_pixel_detect_thd_table;
        //    uint8 mono_hot_pixel_detect_thd_table;
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_dark_pixel_detect_thd_table];
        ptr_u32 = p_cfg_table->mono_dark_pixel_detect_thd_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.dark_pixel_threshold, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_hot_pixel_detect_thd_table];
        ptr_u32 = p_cfg_table->mono_hot_pixel_detect_thd_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.hot_pixel_threshold, &tmp_add2, sizeof(uint32));

        //    uint8 mono_step1_shp_a_fir1;
        //    uint8 mono_step1_shp_a_fir2;
        //    uint8 mono_step1_shp_a_coring;
#if 0 //Original Code
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_shp_a_fir1];
        ptr_u8 = p_cfg_table->mono_step1_shp_a_fir1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.shp_a.asf.fir1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_shp_a_fir2];
        ptr_u8 = p_cfg_table->mono_step1_shp_a_fir2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.shp_a.asf.fir2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_shp_a_coring];
        ptr_u8 = p_cfg_table->mono_step1_shp_a_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.shp_a.asf.coring, &tmp_add2, sizeof(uint32));
#else //To fix coverity
        p_shp_a_shp = &iso_config->mono.liso.raw2yuv.shp_a_shp;
        (void)amba_ik_system_memcpy(&p_shp_a_asf, &p_shp_a_shp, sizeof(iso_asf_data_t*));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_shp_a_fir1];
        ptr_u8 = p_cfg_table->mono_step1_shp_a_fir1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&p_shp_a_asf->fir1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_shp_a_fir2];
        ptr_u8 = p_cfg_table->mono_step1_shp_a_fir2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&p_shp_a_asf->fir2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_shp_a_coring];
        ptr_u8 = p_cfg_table->mono_step1_shp_a_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&p_shp_a_asf->coring, &tmp_add2, sizeof(uint32));
#endif
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step1_batch_queue];
        ptr_u8 = p_cfg_table->mono_step1_batch_queue;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.batch_cmd_queue_daddr, &tmp_add2, sizeof(uint32));
        //    //step4
        //    uint8 mono_step4_shp_a_fir1;
        //    uint8 mono_step4_shp_a_fir2;
        //    uint8 mono_step4_shp_a_coring;
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step4_shp_a_fir1];
        ptr_u8 = p_cfg_table->mono_step4_shp_a_fir1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.smooth.fir1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step4_shp_a_fir2];
        ptr_u8 = p_cfg_table->mono_step4_shp_a_fir2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.smooth.fir2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_step4_shp_a_coring];
        ptr_u8 = p_cfg_table->mono_step4_shp_a_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.smooth.coring, &tmp_add2, sizeof(uint32));
        //    //yuv
        //    uint8 mono_cc_reg_alpha1;
        //    uint8 mono_cc_3d_alpha1;
        //    uint8 mono_cc_reg_alpha2;
        //    uint8 mono_cc_3d_alpha2;
        //    uint8 mono_cc_reg_alpha;
        //    uint8 mono_cc_3d_alpha;
        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_cc_reg_alpha1];
        ptr_u8 = p_cfg_table->mono_cc_reg_alpha1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.yuv.cc_reg_alpha1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_cc_3d_alpha1];
        ptr_u8 = p_cfg_table->mono_cc_3d_alpha1;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.yuv.cc_3d_alpha1, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_cc_reg_alpha2];
        ptr_u8 = p_cfg_table->mono_cc_reg_alpha2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.yuv.cc_reg_alpha2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_cc_3d_alpha2];
        ptr_u8 = p_cfg_table->mono_cc_3d_alpha2;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.yuv.cc_3d_alpha2, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_cc_reg_alpha];
        ptr_u8 = p_cfg_table->mono_cc_reg_alpha;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.yuv.cc_reg_alpha, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_cc_3d_alpha];
        ptr_u8 = p_cfg_table->mono_cc_3d_alpha;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.yuv.cc_3d_alpha, &tmp_add2, sizeof(uint32));

        //    //mctf
        //    uint8 mcts_fus_edge;
        //    uint8 mctf_C;
        //    uint8 mctf_alpha;
        //    uint8 mcts_alpha;
        //    uint8 mctf_freq_blend;
        //    uint8 mcts_freq_blend;

        p_cfg_table = &p_iso_cfg_table_start[table_index->mcts_fus_edge];
        ptr_u8 = p_cfg_table->mcts_fus_edge;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mcts_FusEdge, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mctf_C];
        ptr_u8 = p_cfg_table->mctf_C;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mctf_C, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mctf_alpha];
        ptr_u8 = p_cfg_table->mctf_alpha;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mctf_Alpha, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mcts_alpha];
        ptr_u8 = p_cfg_table->mcts_alpha;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mcts_Alpha, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mctf_freq_blend];
        ptr_u8 = p_cfg_table->mctf_freq_blend;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mctf_FreqBlend, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mcts_freq_blend];
        ptr_u8 = p_cfg_table->mcts_freq_blend;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mcts_FreqBlend, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mctf_passthru];
        ptr_u8 = p_cfg_table->mctf_passthru;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mctf_passthru, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mcts_passthru];
        ptr_u8 = p_cfg_table->mcts_passthru;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.mctf.mcts_passthru, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_ce_coring];
        ptr_u32 = p_cfg_table->mono_ce_coring;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.extra.ce_coring_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_ce_input_table];
        ptr_u32 = p_cfg_table->mono_ce_input_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.extra.ce_input_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_ce_output_table];
        ptr_u16 = p_cfg_table->mono_ce_out_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.extra.ce_output_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_epslion];
        ptr_u8 = p_cfg_table->mono_ce_epsilon_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.extra.ce_epsilon_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->mono_ce_boost];
        ptr_u8 = p_cfg_table->mono_ce_boost_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.extra.ce_boost_table_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->chroma_scale];
        ptr_u16 = p_cfg_table->chroma_scale;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.color.chroma_scale.gain_curver_addr, &tmp_add2, sizeof(uint32));

        p_cfg_table = &p_iso_cfg_table_start[table_index->chroma_median_k_table];
        ptr_u16 = p_cfg_table->chroma_median_k_table;
        (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
        tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
        (void)amba_ik_system_memcpy(&iso_config->mono.liso.raw2yuv.chroma_median_k_table, &tmp_add2, sizeof(uint32));
#endif
    }

    return IK_OK;
}

static INLINE uint32 exe_hook_hiso_config_tables(ik_cv2_hiso_cfg_t *iso_config, const amba_ik_hiso_config_table_index_t *table_index, const intptr *iso_tables)
{
    const amba_ik_hiso_config_tables_t *p_iso_cfg_table_start;
    const amba_ik_hiso_config_tables_t *p_cfg_table;
    void* tmp_addr;
    const void* tmp_add2;
    const uint8 *ptr_u8;
    const uint16 *ptr_u16;
    const uint32 *ptr_u32;
    const int16 *ptr_int16;
    iso_asf_data_t *p_shp_a_asf; //To fix coverity, asf and sharpen data share the same memory
    iso_sharpen_data_t  *p_shp_a_shp;

    (void)amba_ik_system_memcpy(&p_iso_cfg_table_start, iso_tables, sizeof(amba_ik_hiso_config_tables_t *));

    p_cfg_table = &p_iso_cfg_table_start[table_index->hor_warp];
    ptr_int16 = p_cfg_table->warp_hor;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.geo_warp_control.warp_horizontal_table_address, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->ver_warp];
    ptr_int16 = p_cfg_table->warp_ver;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.geo_warp_control.warp_vertical_table_address, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->hor_cawarp_red];
    ptr_int16 = p_cfg_table->ca_warp_hor_red;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.ca_warp_control.warp_horizontal_table_addr_red, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->hor_cawarp_blue];
    ptr_int16 = p_cfg_table->ca_warp_hor_blue;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.ca_warp_control.warp_horizontal_table_addr_blue, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->ver_cawarp_red];
    ptr_int16 = p_cfg_table->ca_warp_ver_red;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.ca_warp_control.warp_vertical_table_addr_red, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->ver_cawarp_blue];
    ptr_int16 = p_cfg_table->ca_warp_ver_blue;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_int16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.ca_warp_control.warp_vertical_table_addr_blue, &tmp_add2, sizeof(uint32));


    p_cfg_table = &p_iso_cfg_table_start[table_index->vignette];
    ptr_u8 = p_cfg_table->vignette;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->common.vignette_gain, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_dark_pixel_detect_thd_table];
    ptr_u32 = p_cfg_table->Step1DarkPixelDetectThdTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.raw2yuv_hi.dark_pixel_threshold, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_hot_pixel_detect_thd_table];
    ptr_u32 = p_cfg_table->Step1HotPixelDetectThdTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.raw2yuv_hi.hot_pixel_threshold, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_chroma_median_k_table];
    ptr_u16 = p_cfg_table->Step1ChromaMedianKTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.raw2yuv_hi.chroma_median_k_table, &tmp_add2, sizeof(uint32));
#if 0 //Original Code
    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_shp_a_fir1];
    ptr_u8 = p_cfg_table->Step1ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.raw2yuv_hi.shp_a.asf.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_shp_a_fir2];
    ptr_u8 = p_cfg_table->Step1ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.raw2yuv_hi.shp_a.asf.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_shp_a_coring];
    ptr_u8 = p_cfg_table->Step1ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.raw2yuv_hi.shp_a.asf.coring, &tmp_add2, sizeof(uint32));
#else //To fix coverity
    p_shp_a_shp = &iso_config->step01.raw2yuv_hi.shp_a_shp;
    (void)amba_ik_system_memcpy(&p_shp_a_asf, &p_shp_a_shp, sizeof(iso_asf_data_t*));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_shp_a_fir1];
    ptr_u8 = p_cfg_table->Step1ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_asf->fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_shp_a_fir2];
    ptr_u8 = p_cfg_table->Step1ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_asf->fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_shp_a_coring];
    ptr_u8 = p_cfg_table->Step1ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_asf->coring, &tmp_add2, sizeof(uint32));
#endif
    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_fe_tc];
    ptr_u8 = p_cfg_table->Step1FeToneCurve;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.extra_hi.exp0_decompand_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_ce_boost];
    ptr_u8 = p_cfg_table->Step1CeBoostTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.extra_hi.ce_boost_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_ce_coring];
    ptr_u32 = p_cfg_table->Step1CeCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.extra_hi.ce_coring_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_ce_input];
    ptr_u32 = p_cfg_table->Step1CeInputTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.extra_hi.ce_input_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_ce_output];
    ptr_u16 = p_cfg_table->Step1CeOutTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.extra_hi.ce_output_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step1_ce_epsilon];
    ptr_u8 = p_cfg_table->Step1CeEpsilonTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step01.extra_hi.ce_epsilon_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step2_cc_reg];
    ptr_u8 = p_cfg_table->Step2CcRegREdgeScorce;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step02.cc_reg_edge_score, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step2_cc_3d];
    ptr_u8 = p_cfg_table->Step2Cc3dEdgeScorce;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step02.cc_3d_edge_score, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->chroma_scale];
    ptr_u16 = p_cfg_table->ChromaScaleGainCurve;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->color.chroma_scale.gain_curver_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->cc_in];
    ptr_u8 = p_cfg_table->NormalCCReg;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->color.cc_reg, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->cc_3d];
    ptr_u8 = p_cfg_table->NormalCC3d;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->color.cc_3d, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->cc_out];
    ptr_u8 = p_cfg_table->NormalCCOut;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->color.cc_out, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step3_fir1];
    ptr_u8 = p_cfg_table->Step3ChromaShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step03.asf_chroma.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step3_fir2];
    ptr_u8 = p_cfg_table->Step3ChromaShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step03.asf_chroma.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step3_coring];
    ptr_u8 = p_cfg_table->Step3ChromaShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step03.asf_chroma.coring, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step4_fir1];
    ptr_u8 = p_cfg_table->Step4AsfMedFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step04.asf_med1.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step4_fir2];
    ptr_u8 = p_cfg_table->Step4AsfMedFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step04.asf_med1.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step4_coring];
    ptr_u8 = p_cfg_table->Step4AsfMedCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step04.asf_med1.coring, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step5_fir1];
    ptr_u8 = p_cfg_table->Step5MedShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step05.sharpen_med.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step5_fir2];
    ptr_u8 = p_cfg_table->Step5MedShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step05.sharpen_med.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step5_coring];
    ptr_u8 = p_cfg_table->Step5MedShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step05.sharpen_med.coring, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step9_fir1];
    ptr_u8 = p_cfg_table->Step9ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step09.shp_a_sharpen_high.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step9_fir2];
    ptr_u8 = p_cfg_table->Step9ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step09.shp_a_sharpen_high.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step9_coring];
    ptr_u8 = p_cfg_table->Step9ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step09.shp_a_sharpen_high.coring, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_hot_pxl];
    ptr_u32 = p_cfg_table->Step11HotPixelDetectThdTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.raw2yuv_li.hot_pixel_threshold, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_dark_pxl];
    ptr_u32 = p_cfg_table->Step11DarkPixelDetectThdTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.raw2yuv_li.dark_pixel_threshold, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_chroma_median_k];
    ptr_u16 = p_cfg_table->Step11ChromaMedianKTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.raw2yuv_li.chroma_median_k_table, &tmp_add2, sizeof(uint32));
#if 0 //Original Code
    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_fir1];
    ptr_u8 = p_cfg_table->Step11ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.raw2yuv_li.shp_a.asf.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_fir2];
    ptr_u8 = p_cfg_table->Step11ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.raw2yuv_li.shp_a.asf.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_coring];
    ptr_u8 = p_cfg_table->Step11ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.raw2yuv_li.shp_a.asf.coring, &tmp_add2, sizeof(uint32));
#else //To fix coverity
    p_shp_a_shp = &iso_config->step11.raw2yuv_li.shp_a_shp;
    (void)amba_ik_system_memcpy(&p_shp_a_asf, &p_shp_a_shp, sizeof(iso_asf_data_t*));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_fir1];
    ptr_u8 = p_cfg_table->Step11ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_shp->fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_fir2];
    ptr_u8 = p_cfg_table->Step11ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_shp->fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_coring];
    ptr_u8 = p_cfg_table->Step11ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_shp->coring, &tmp_add2, sizeof(uint32));
#endif
    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_fe_tc];
    ptr_u8 = p_cfg_table->Step11FeToneCurve;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.extra_li.exp0_decompand_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_ce_boost];
    ptr_u8 = p_cfg_table->Step11CeBoostTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.extra_li.ce_boost_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_ce_coring];
    ptr_u32 = p_cfg_table->Step11CeCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.extra_li.ce_coring_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_ce_input];
    ptr_u32 = p_cfg_table->Step11CeInputTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.extra_li.ce_input_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_ce_output];
    ptr_u16 = p_cfg_table->Step11CeOutTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.extra_li.ce_output_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step11_ce_epsilon];
    ptr_u8 = p_cfg_table->Step11CeEpsilonTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step11.extra_li.ce_epsilon_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_hot_pxl];
    ptr_u32 = p_cfg_table->Step12HotPixelDetectThdTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.raw2yuv_li2.hot_pixel_threshold, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_dark_pxl];
    ptr_u32 = p_cfg_table->Step12DarkPixelDetectThdTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.raw2yuv_li2.dark_pixel_threshold, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_chroma_median_k];
    ptr_u16 = p_cfg_table->Step12ChromaMedianKTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.raw2yuv_li2.chroma_median_k_table, &tmp_add2, sizeof(uint32));
#if 0 //Original Code
    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_fir1];
    ptr_u8 = p_cfg_table->Step12ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.raw2yuv_li2.shp_a.asf.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_fir2];
    ptr_u8 = p_cfg_table->Step12ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.raw2yuv_li2.shp_a.asf.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_coring];
    ptr_u8 = p_cfg_table->Step12ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.raw2yuv_li2.shp_a.asf.coring, &tmp_add2, sizeof(uint32));
#else //To fix coverity
    p_shp_a_shp = &iso_config->step12.raw2yuv_li2.shp_a_shp;
    (void)amba_ik_system_memcpy(&p_shp_a_asf, &p_shp_a_shp, sizeof(iso_asf_data_t*));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_fir1];
    ptr_u8 = p_cfg_table->Step12ShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_shp->fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_fir2];
    ptr_u8 = p_cfg_table->Step12ShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_shp->fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_coring];
    ptr_u8 = p_cfg_table->Step12ShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&p_shp_a_shp->coring, &tmp_add2, sizeof(uint32));
#endif
    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_fe_tc];
    ptr_u8 = p_cfg_table->Step12FeToneCurve;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.exp0_decompand_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_ce_boost];
    ptr_u8 = p_cfg_table->Step12CeBoostTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.ce_boost_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_ce_boost];
    ptr_u8 = p_cfg_table->Step12CeBoostTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.ce_boost_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_ce_coring];
    ptr_u32 = p_cfg_table->Step12CeCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.ce_coring_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_ce_input];
    ptr_u32 = p_cfg_table->Step12CeInputTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u32, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.ce_input_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_ce_output];
    ptr_u16 = p_cfg_table->Step12CeOutTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u16, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.ce_output_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step12_ce_epsilon];
    ptr_u8 = p_cfg_table->Step12CeEpsilonTable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step12.extra_li2.ce_epsilon_table_addr, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step13_fir1];
    ptr_u8 = p_cfg_table->Step13ChromaShpFir1Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step13.asf_chroma_li.fir1, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step13_fir2];
    ptr_u8 = p_cfg_table->Step13ChromaShpFir2Coeff;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step13.asf_chroma_li.fir2, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->step13_coring];
    ptr_u8 = p_cfg_table->Step13ChromaShpCoring;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->step13.asf_chroma_li.coring, &tmp_add2, sizeof(uint32));

    //    uint8 step01_mcts_Edge_Detect[LS_FINAL_SHARPEN_SIZE];  // step 1 SHPB;
    p_cfg_table = &p_iso_cfg_table_start[table_index->step1MctsEdgeDetect];
    ptr_u8 = p_cfg_table->step1MctsEdgeDetect;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_Edge_Detect, &tmp_add2, sizeof(uint32));

    //    uint8 step02_mcts_Edge_ASF[LS_FINAL_SHARPEN_SIZE];     // step 2 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step2MctsEdgeASF];
    ptr_u8 = p_cfg_table->step2MctsEdgeASF;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_Edge_ASF, &tmp_add2, sizeof(uint32));

    //    uint8 step03_mcts_L_High_ASF[LS_FINAL_SHARPEN_SIZE];   // step 3 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step3MctsLHighASF];
    ptr_u8 = p_cfg_table->step3MctsLHighASF;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_L_High_ASF, &tmp_add2, sizeof(uint32));

    //    uint8 step04_mcts_L_Low_ASF[LS_FINAL_SHARPEN_SIZE];    // step 4 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step4MctsLLowASF];
    ptr_u8 = p_cfg_table->step4MctsLLowASF;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_L_Low_ASF, &tmp_add2, sizeof(uint32));

    //    uint8 step05_mctf_Low_ASF_Combine[LS_VIDEO_MCTF_SIZE]; // step 5 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step5MctfLowAsfCombine];
    ptr_u8 = p_cfg_table->step5MctfLowAsfCombine;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_Low_ASF_Combine, &tmp_add2, sizeof(uint32));

    //    uint8 step05_mcts_Med2_ASF[LS_FINAL_SHARPEN_SIZE];     // step 5 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step5MctsMed2ASF];
    ptr_u8 = p_cfg_table->step5MctsMed2ASF;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_Med2_ASF, &tmp_add2, sizeof(uint32));

    //    uint8 step06_mctf_C_8[LS_VIDEO_MCTF_SIZE]; // step 6 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step6MctfC8];
    ptr_u8 = p_cfg_table->step6MctfC8;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_C_Eight, &tmp_add2, sizeof(uint32));
    //    uint8 step07_mctf_C_4[LS_VIDEO_MCTF_SIZE]; // step 7 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step7MctfC4];
    ptr_u8 = p_cfg_table->step7MctfC4;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_C_4, &tmp_add2, sizeof(uint32));
    //    uint8 step08_mctf_C_B[LS_VIDEO_MCTF_SIZE]; // step 8 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step8MctfCB];
    ptr_u8 = p_cfg_table->step8MctfCB;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_C_B, &tmp_add2, sizeof(uint32));
    //    uint8 step09_mctf_L_Mix2[LS_VIDEO_MCTF_SIZE]; // step 9 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step9MctfLMix2];
    ptr_u8 = p_cfg_table->step9MctfLMix2;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_L_Mix2, &tmp_add2, sizeof(uint32));
    //    uint8 step09_mcts_L_Mix2[LS_FINAL_SHARPEN_SIZE]; // step 9 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step9MctsLMix2];
    ptr_u8 = p_cfg_table->step9MctsLMix2;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_L_Mix2, &tmp_add2, sizeof(uint32));
    //    uint8 step10_mctf_L_Luma_Noise_Combine[LS_VIDEO_MCTF_SIZE]; // step 10 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step10MctfLLumaNoiseCombine];
    ptr_u8 = p_cfg_table->step10MctfLLumaNoiseCombine;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_L_Luma_Noise_Combine, &tmp_add2, sizeof(uint32));
    //    uint8 step11_mctf_LI_HI_Combine[LS_VIDEO_MCTF_SIZE]; // step 11 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step11MctfLIHICombine];
    ptr_u8 = p_cfg_table->step11MctfLIHICombine;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_LI_HI_Combine, &tmp_add2, sizeof(uint32));
    //    uint8 step11_mcts_LI_HI_Combine[LS_FINAL_SHARPEN_SIZE]; // step 11 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step11MctsLIHICombine];
    ptr_u8 = p_cfg_table->step11MctsLIHICombine;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_LI_HI_Combine, &tmp_add2, sizeof(uint32));
    //    uint8 step12_mctf_LI2_HI_Combine[LS_VIDEO_MCTF_SIZE]; // step 12 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step12MctfLI2HICombine];
    ptr_u8 = p_cfg_table->step12MctfLI2HICombine;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_LI2_HI_Combine, &tmp_add2, sizeof(uint32));
    //    uint8 step12_mcts_LI2_HI_Combine[LS_FINAL_SHARPEN_SIZE]; // step 12 SHPB
    p_cfg_table = &p_iso_cfg_table_start[table_index->step12MctsLI2HICombine];
    ptr_u8 = p_cfg_table->step12MctsLI2HICombine;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_LI2_HI_Combine, &tmp_add2, sizeof(uint32));
    //    uint8 step13_mctf_C_A[LS_VIDEO_MCTF_SIZE]; // step 13 MCTF
    p_cfg_table = &p_iso_cfg_table_start[table_index->step13MctfCA];
    ptr_u8 = p_cfg_table->step13MctfCA;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_C_A, &tmp_add2, sizeof(uint32));
    //    uint8 mctf_disable[LS_VIDEO_MCTF_SIZE];
    p_cfg_table = &p_iso_cfg_table_start[table_index->MctfDisable];
    ptr_u8 = p_cfg_table->MctfDisable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mctf_disable, &tmp_add2, sizeof(uint32));
    //    uint8 mcts_disable[LS_FINAL_SHARPEN_SIZE];
    p_cfg_table = &p_iso_cfg_table_start[table_index->MctsDisable];
    ptr_u8 = p_cfg_table->MctsDisable;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_disable, &tmp_add2, sizeof(uint32));

    p_cfg_table = &p_iso_cfg_table_start[table_index->MctsDisable420];
    ptr_u8 = p_cfg_table->MctsDisable420;
    (void)amba_ik_system_memcpy(&tmp_addr, &ptr_u8, sizeof(void*));
    tmp_add2 = amba_ik_system_virt_to_phys(tmp_addr);
    (void)amba_ik_system_memcpy(&iso_config->mctf.mcts_disable_420, &tmp_add2, sizeof(uint32));
    return IK_OK;
}

static uint32 img_exe_execute_invalid_config(uint32 context_id)
{
    //amba_ik_system_print("[IK] img_exe_execute_invalid_config() is invalid!");
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        amba_ik_system_print_uint32_5("[IK]  img_exe_execute_invalid_config ERROR, context_id %d, img_arch_get_context_number() %d", context_id, img_arch_get_context_number(), DC_U, DC_U, DC_U);
        rval = IK_ERR_0000; // TBD
    }
    return rval;
}


static uint32 img_exe_execute_update_iso_config(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_iso_config_and_state_t *iso_config_and_state = NULL;
    amba_ik_exe_update_iso_utilities_t utilities= {NULL};
    uint32 config_id = 0, config_number = 0;
    uint32 src_config_id;
    intptr addr = 0;
    amba_ik_filter_update_flags_t eis_mode_update_flags;
    uint32 *p_explicit_iso_runnung_number = NULL;

    rval |= img_ctx_get_context(context_id, &p_ctx);
    (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
    if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        config_number = p_ctx->organization.attribute.config_number;
        if (p_ctx->filters.input_param.eis_mode == STAGE_2_FOR_EIS_EXECUTE) {
            (void)img_ctx_get_explicit_iso_running_num(context_id, &p_explicit_iso_runnung_number);
            config_id = *p_explicit_iso_runnung_number;
        } else {
            rval |= exe_get_next_iso_config_index(config_number, &p_ctx->organization.active_config_state, &config_id);
        }
        rval |= img_ctx_get_iso_config_and_state(context_id, config_id, &addr);
        (void)amba_ik_system_memcpy(&iso_config_and_state, &addr, sizeof(void *));
        rval |= img_ctx_get_iso_config_tables(context_id, 0, &addr);
        if (rval == IK_OK) {
            (void)amba_ik_system_memset(&eis_mode_update_flags, 0x0, sizeof(amba_ik_filter_update_flags_t));
            if (p_ctx->filters.input_param.eis_mode == STAGE_1_FOR_EIS_EXECUTE) {//skip warp/ ca/window calculation, and warp/ ca ring working buffer.
                eis_mode_update_flags.liso.window_size_info_updated = p_ctx->filters.update_flags.liso.window_size_info_updated;
                p_ctx->filters.update_flags.liso.window_size_info_updated = 0U;
                eis_mode_update_flags.liso.cfa_window_size_info_updated = p_ctx->filters.update_flags.liso.cfa_window_size_info_updated;
                p_ctx->filters.update_flags.liso.cfa_window_size_info_updated = 0U;
                eis_mode_update_flags.liso.system_performance_info_updated = p_ctx->filters.update_flags.liso.system_performance_info_updated;
                p_ctx->filters.update_flags.liso.system_performance_info_updated = 0U;
                eis_mode_update_flags.liso.calib_warp_info_updated = p_ctx->filters.update_flags.liso.calib_warp_info_updated;
                p_ctx->filters.update_flags.liso.calib_warp_info_updated = 0U;
                eis_mode_update_flags.liso.calib_ca_warp_info_updated = p_ctx->filters.update_flags.liso.calib_ca_warp_info_updated;
                p_ctx->filters.update_flags.liso.calib_ca_warp_info_updated = 0U;
                eis_mode_update_flags.liso.dzoom_info_updated = p_ctx->filters.update_flags.liso.dzoom_info_updated;
                p_ctx->filters.update_flags.liso.dzoom_info_updated = 0U;
                eis_mode_update_flags.liso.warp_enable_updated = p_ctx->filters.update_flags.liso.warp_enable_updated;
                p_ctx->filters.update_flags.liso.warp_enable_updated = 0U;
                eis_mode_update_flags.liso.cawarp_enable_updated = p_ctx->filters.update_flags.liso.cawarp_enable_updated;
                p_ctx->filters.update_flags.liso.cawarp_enable_updated = 0U;
                eis_mode_update_flags.liso.dmy_range_updated = p_ctx->filters.update_flags.liso.dmy_range_updated;
                p_ctx->filters.update_flags.liso.dmy_range_updated = 0U;
                eis_mode_update_flags.liso.flip_mode_updated = p_ctx->filters.update_flags.liso.flip_mode_updated;
                p_ctx->filters.update_flags.liso.flip_mode_updated = 0U;
                eis_mode_update_flags.liso.vin_active_win_updated = p_ctx->filters.update_flags.liso.vin_active_win_updated;
                p_ctx->filters.update_flags.liso.vin_active_win_updated = 0U;
            } else if (p_ctx->filters.input_param.eis_mode == STAGE_2_FOR_EIS_EXECUTE) {// do post warp/ ca/ window calculation, and warp/ ca ring working buffer.
                (void)amba_ik_system_memcpy(&eis_mode_update_flags, &p_ctx->filters.update_flags, sizeof(amba_ik_filter_update_flags_t));
                rval |= img_ctx_reset_iso_config_filter_update_flags(context_id);
                p_ctx->filters.update_flags.liso.window_size_info_updated = eis_mode_update_flags.liso.window_size_info_updated;
                eis_mode_update_flags.liso.window_size_info_updated = 0U;
                p_ctx->filters.update_flags.liso.cfa_window_size_info_updated = eis_mode_update_flags.liso.cfa_window_size_info_updated;
                eis_mode_update_flags.liso.cfa_window_size_info_updated = 0U;
                p_ctx->filters.update_flags.liso.system_performance_info_updated = eis_mode_update_flags.liso.system_performance_info_updated;
                eis_mode_update_flags.liso.system_performance_info_updated = 0U;
                p_ctx->filters.update_flags.liso.calib_warp_info_updated = eis_mode_update_flags.liso.calib_warp_info_updated;
                eis_mode_update_flags.liso.calib_warp_info_updated = 0U;
                p_ctx->filters.update_flags.liso.calib_ca_warp_info_updated = eis_mode_update_flags.liso.calib_ca_warp_info_updated;
                eis_mode_update_flags.liso.calib_ca_warp_info_updated = 0U;
                p_ctx->filters.update_flags.liso.dzoom_info_updated = eis_mode_update_flags.liso.dzoom_info_updated;
                eis_mode_update_flags.liso.dzoom_info_updated = 0U;
                p_ctx->filters.update_flags.liso.warp_enable_updated = eis_mode_update_flags.liso.warp_enable_updated;
                eis_mode_update_flags.liso.warp_enable_updated = 0U;
                p_ctx->filters.update_flags.liso.cawarp_enable_updated = eis_mode_update_flags.liso.cawarp_enable_updated;
                eis_mode_update_flags.liso.cawarp_enable_updated = 0U;
                p_ctx->filters.update_flags.liso.dmy_range_updated = eis_mode_update_flags.liso.dmy_range_updated;
                eis_mode_update_flags.liso.dmy_range_updated = 0U;
                p_ctx->filters.update_flags.liso.flip_mode_updated = eis_mode_update_flags.liso.flip_mode_updated;
                eis_mode_update_flags.liso.flip_mode_updated = 0U;
                p_ctx->filters.update_flags.liso.vin_active_win_updated = eis_mode_update_flags.liso.vin_active_win_updated;
                eis_mode_update_flags.liso.vin_active_win_updated = 0U;

            } else {
                ;//misra.
            }
            if (p_ctx->filters.input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) { // eis_stage_2 doesn't need clone and reset cmd.
                src_config_id = p_ctx->organization.active_config_state.active_iso_idx;
                rval |= exe_clone_iso_config(&p_ctx->configs, &p_ctx->organization.attribute, src_config_id, config_id);
                (void)amba_ik_system_memset(&iso_config_and_state->cmds, 0x0, sizeof(amba_ik_setup_cmds_t));
            }
            if ((p_ctx->filters.input_param.eis_mode == STAGE_1_FOR_EIS_EXECUTE) ||
                (p_ctx->filters.input_param.eis_mode == STAGE_2_FOR_EIS_EXECUTE)) { // To get the correct table index from ctx
                iso_config_and_state->table_index.hor_warp = p_ctx->organization.active_config_state.active_iso_tbl_idx.hor_warp;
                iso_config_and_state->table_index.ver_warp = p_ctx->organization.active_config_state.active_iso_tbl_idx.ver_warp;
            }
            (void)amba_ik_system_memset(&iso_config_and_state->iso_tables_info, 0x0, sizeof(amba_ik_cfg_tbl_description_list_t));
            rval |= exe_get_next_iso_config_table_indexes(config_number, &p_ctx->filters.update_flags.liso, &p_ctx->organization.active_config_state.active_iso_tbl_idx, &iso_config_and_state->table_index);
            (void)amba_ik_system_memcpy(&p_ctx->organization.active_config_state.active_iso_tbl_idx, &iso_config_and_state->table_index, sizeof(amba_ik_iso_config_table_index_t));
            exe_get_update_iso_config_utilities(&p_ctx->organization.attribute.ability, &utilities);
            rval |= exe_hook_iso_config_tables(&iso_config_and_state->iso_config, &iso_config_and_state->cmds, &iso_config_and_state->table_index, &addr, p_ctx->filters.input_param.eis_mode);
            rval |= utilities.compute_updated_iso_config_and_tables(&iso_config_and_state->iso_config, &iso_config_and_state->cmds, &p_ctx->filters);
            if (rval == IK_OK) {
                if (p_ctx->filters.input_param.eis_mode != STAGE_2_FOR_EIS_EXECUTE) { // eis_stage_2 can not touch active_iso_idx.
                    p_ctx->organization.active_config_state.active_iso_idx = config_id;
                }
                rval |= img_ctx_reset_iso_config_filter_update_flags(context_id);
            }
            if ((p_ctx->filters.input_param.eis_mode == STAGE_1_FOR_EIS_EXECUTE) || (p_ctx->filters.input_param.eis_mode == STAGE_2_FOR_EIS_EXECUTE)) {
                // if stage = 1 : remain warp/ ca/ window related status for eis_stage_2.
                // if stage = 2 : remain rest filters status for next eis_stage_1 to update.
                (void)amba_ik_system_memcpy(&p_ctx->filters.update_flags, &eis_mode_update_flags, sizeof(amba_ik_filter_update_flags_t));
            }
            if (p_ctx->filters.input_param.eis_mode == STAGE_1_FOR_EIS_EXECUTE) {
                iso_config_and_state->iso_config.calib.is_eis_warp_ready = 0U;
            } else {// regular post-exe or eis_stage_2.
                iso_config_and_state->iso_config.calib.is_eis_warp_ready = 1U;
            }
            rval |= img_cfg_find_iso_config_tables(&iso_config_and_state->iso_config, &iso_config_and_state->iso_tables_info, p_ctx->filters.input_param.eis_mode);
            rval |= img_cfg_clean_iso_config_and_tables_cache(&iso_config_and_state->iso_config, &iso_config_and_state->iso_tables_info);

        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_config(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
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
    (void)amba_ik_system_unlock_mutex(context_id);

    return rval;

}

static uint32 INLINE sub_img_exe_update_check_still_fstshpns(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;

    if (p_update_flag->liso.fstshpns_both_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_both_updated = 0. Please check ik_set_fst_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_noise_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_noise_updated = 0. Please check ik_set_fst_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_fir_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_fir_updated = 0. Please check ik_set_fst_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_updated = 0. Please check ik_set_fst_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_coring_index_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_index_scale_updated = 0. Please check ik_set_fst_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_min_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_min_coring_result_updated = 0. Please check ik_set_fst_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_max_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_max_coring_result_updated = 0. Please check ik_set_fst_shp_max_coring_rslt() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_scale_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_scale_coring_updated = 0. Please check ik_set_fst_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    return rval;
}

static uint32 INLINE sub_img_exe_execute_filter_update_check_still(const amba_ik_filter_update_flags_t *p_update_flag, uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;
    {
        if (p_update_flag->liso.first_luma_processing_mode_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] first_luma_processing_mode_updated = 0. Please check ik_set_fst_luma_process_mode() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        } else {
            if(use_sharpen_not_asf == 0U) {
                if (p_update_flag->liso.advance_spatial_filter_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] advance_spatial_filter_updated = 0. Please check ik_set_adv_spatial_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                } else {
                    rval |= sub_img_exe_update_check_still_fstshpns(p_update_flag);
                }
            } else {
                rval |= sub_img_exe_update_check_still_fstshpns(p_update_flag);
            }
        }
    }
    return rval;
}

static uint32 INLINE sub1_img_exe_update_check_cc(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.color_correction_reg_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] color_correction_reg_updated = 0. Please check ik_set_color_correction_reg() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.color_correction_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] color_correction_updated = 0. Please check ik_set_color_correction()I have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE sub1_img_exe_update_check_chroma(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.chroma_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_scale_updated = 0. Please check ik_set_chroma_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.chroma_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_filter_updated = 0. Please check ik_set_chroma_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.chroma_median_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_median_filter_updated = 0. Please check ik_set_chroma_median_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE sub1_img_exe_execute_filter_update_check_still(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.sensor_information_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] sensor_information_updated = 0. Please check ik_set_vin_sensor_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.exp0_fe_static_blc_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] exp0_fe_static_blc_updated = 0. Please check ik_set_exp0_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.exp0_fe_wb_gain_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] exp0_fe_wb_gain_updated = 0. Please check ik_set_exp0_fe_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.dynamic_bpc_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] dynamic_bpc_updated = 0. Please check ik_set_dynamic_bad_pixel_corr() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.cfa_leakage_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] cfa_leakage_filter_updated = 0. Please check ik_set_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.anti_aliasing_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] anti_aliasing_updated = 0. Please check ik_set_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.grgb_mismatch_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] grgb_mismatch_updated = 0. Please check ik_set_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.before_ce_wb_gain_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_before_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.after_ce_wb_gain_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] after_ce_wb_gain_updated = 0. Please check ik_set_after_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.cfa_noise_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] cfa_noise_filter_updated = 0. Please check ik_set_cfa_noise_filter() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.demosaic_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] demosaic_updated = 0. Please check ik_set_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.tone_curve_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] tone_curve_updated = 0. Please check ik_set_tone_curve() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.rgb_to_yuv_matrix_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] rgb_to_yuv_matrix_updated = 0. Please check ik_set_rgb_to_yuv_matrix() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    rval |= sub1_img_exe_update_check_cc(p_update_flag);
    rval |= sub1_img_exe_update_check_chroma(p_update_flag);
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_hi(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_anti_aliasing_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_anti_aliasing_updated = 0. Please check ik_set_hi_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_cfa_leakage_filter_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_cfa_leakage_filter_updated = 0. Please check ik_set_hi_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_dynamic_bpc_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_dynamic_bpc_updated = 0. Please check ik_set_hi_dynamic_bpc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_grgb_mismatch_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_grgb_mismatch_updated = 0. Please check ik_set_hi_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_median_filter_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_median_filter_updated = 0. Please check ik_set_hi_chroma_median_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_cfa_noise_filter_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_cfa_noise_filter_updated = 0. Please check ik_set_hi_cfa_noise_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_demosaic_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_demosaic_updated = 0. Please check ik_set_hi_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_li2(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.li2_anti_aliasing_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_anti_aliasing_updated = 0. Please check ik_set_li2_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_cfa_leakage_filter_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_cfa_leakage_filter_updated = 0. Please check ik_set_li2_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_dynamic_bpc_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_dynamic_bpc_updated = 0. Please check ik_set_li2_dynamic_bpc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_grgb_mismatch_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_grgb_mismatch_updated = 0. Please check ik_set_li2_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_cfa_noise_filter_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_cfa_noise_filter_updated = 0. Please check ik_set_li2_cfa_noise_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_demosaic_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_demosaic_updated = 0. Please check ik_set_li2_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_asf(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_asf_updated = 0. Please check ik_set_hi_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_asf_updated = 0. Please check ik_set_li2_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_low_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_low_asf_updated = 0. Please check ik_set_hi_low_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med1_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med1_asf_updated = 0. Please check ik_set_hi_med1_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med2_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med2_asf_updated = 0. Please check ik_set_hi_med2_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_update_flag->liso.hi_high_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_asf_updated = 0. Please check ik_set_hi_high_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high2_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high2_asf_updated = 0. Please check ik_set_hi_high2_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.chroma_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_asf_updated = 0. Please check ik_set_chroma_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_asf_updated = 0. Please check ik_set_hi_chroma_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_low_chroma_asf_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_low_chroma_asf_updated = 0. Please check ik_set_hi_low_chroma_asf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_high_shpns(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_high_shpns_both_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_both_updated = 0. Please check ik_set_hi_high_shpns_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_noise_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_noise_updated = 0. Please check ik_set_hi_high_shpns_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_coring_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_coring_updated = 0. Please check ik_set_hi_high_shpns_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_fir_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_fir_updated = 0. Please check ik_set_hi_high_shpns_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_cor_idx_scl_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_cor_idx_scl_updated = 0. Please check ik_set_hi_high_shpns_cor_idx_scl() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_min_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_min_cor_rst_updated = 0. Please check ik_set_hi_high_shpns_min_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_max_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_max_cor_rst_updated = 0. Please check ik_set_hi_high_shpns_max_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_high_shpns_scl_cor_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_high_shpns_scl_cor_updated = 0. Please check ik_set_hi_high_shpns_scl_cor() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_med_shpns(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_med_shpns_both_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_both_updated = 0. Please check ik_set_hi_med_shpns_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med_shpns_noise_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_noise_updated = 0. Please check ik_set_hi_med_shpns_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med_shpns_coring_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_coring_updated = 0. Please check ik_set_hi_med_shpns_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med_shpns_fir_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_fir_updated = 0. Please check ik_set_hi_med_shpns_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med_shpns_cor_idx_scl_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_cor_idx_scl_updated = 0. Please check ik_set_hi_med_shpns_cor_idx_scl() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_update_flag->liso.hi_med_shpns_min_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_min_cor_rst_updated = 0. Please check ik_set_hi_med_shpns_min_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med_shpns_max_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_max_cor_rst_updated = 0. Please check ik_set_hi_med_shpns_max_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_med_shpns_scl_cor_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_med_shpns_scl_cor_updated = 0. Please check ik_set_hi_med_shpns_scl_cor() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_li2_shpns(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.li2_shpns_both_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_both_updated = 0. Please check ik_set_li2_shpns_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_noise_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_noise_updated = 0. Please check ik_set_li2_shpns_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_coring_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_coring_updated = 0. Please check ik_set_li2_shpns_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_fir_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_fir_updated = 0. Please check ik_set_li2_shpns_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_cor_idx_scl_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_cor_idx_scl_updated = 0. Please check ik_set_li2_shpns_cor_idx_scl() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_min_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_min_cor_rst_updated = 0. Please check ik_set_li2_shpns_min_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_max_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_max_cor_rst_updated = 0. Please check ik_set_li2_shpns_max_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.li2_shpns_scl_cor_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] li2_shpns_scl_cor_updated = 0. Please check ik_set_li2_shpns_scl_cor() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_hili_shpns(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hili_shpns_both_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_both_updated = 0. Please check ik_set_hili_shpns_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_shpns_noise_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_noise_updated = 0. Please check ik_set_hili_shpns_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_shpns_coring_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_coring_updated = 0. Please check ik_set_hili_shpns_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_shpns_fir_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_fir_updated = 0. Please check ik_set_hili_shpns_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_shpns_cor_idx_scl_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_cor_idx_scl_updated = 0. Please check ik_set_hili_shpns_cor_idx_scl() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_shpns_min_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_min_cor_rst_updated = 0. Please check ik_set_hili_shpns_min_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_shpns_max_cor_rst_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_max_cor_rst_updated = 0. Please check ik_set_hili_shpns_max_cor_rst() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_update_flag->liso.hili_shpns_scl_cor_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_shpns_scl_cor_updated = 0. Please check ik_set_hili_shpns_scl_cor() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_chroma(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_chroma_filter_high_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_filter_high_updated = 0. Please check ik_set_hi_chroma_filter_high() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_filter_pre_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_filter_pre_updated = 0. Please check ik_set_hi_chroma_filter_pre() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_filter_med_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_filter_med_updated = 0. Please check ik_set_hi_chroma_filter_med() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_filter_low_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_filter_low_updated = 0. Please check ik_set_hi_chroma_filter_low() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_filter_very_low_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_filter_very_low_updated = 0. Please check ik_set_hi_chroma_filter_very_low() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_combine(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_luma_combine_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_luma_combine_updated = 0. Please check ik_set_hi_luma_combine() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_low_asf_combine_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_low_asf_combine_updated = 0. Please check ik_set_hi_low_asf_combine() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_fltr_med_com_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_fltr_med_com_updated = 0. Please check ik_set_hi_chroma_fltr_med_com() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_fltr_low_com_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_fltr_low_com_updated = 0. Please check ik_set_hi_chroma_fltr_low_com() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_chroma_fltr_very_low_com_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_chroma_fltr_very_low_com_updated = 0. Please check ik_set_hi_chroma_fltr_very_low_com() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hili_combine_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hili_combine_updated = 0. Please check ik_set_hili_combine() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_update_check_hiso_other(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.hi_mid_high_freq_recover_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_mid_high_freq_recover_updated = 0. Please check ik_set_hi_mid_high_freq_recover() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_luma_blend_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_luma_blend_updated = 0. Please check ik_set_hi_luma_blend() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_nonsmooth_detect_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_nonsmooth_detect_updated = 0. Please check ik_set_hi_nonsmooth_detect() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.hi_select_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] hi_select_updated = 0. Please check ik_set_hi_select() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.ce_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] ce_updated = 0. Please check ik_set_ce() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.ce_input_table_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] ce_input_table_updated = 0. Please check ik_set_ce_input_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE img_exe_execute_still_filter_update_check(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag, uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->still_pipe;

    rval |= sub_img_exe_execute_filter_update_check_still(p_update_flag, use_sharpen_not_asf);
    rval |= sub1_img_exe_execute_filter_update_check_still(p_update_flag);
    if (p_update_flag->liso.first_luma_processing_mode_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] first_luma_processing_mode_updated = 0. Please check ik_set_first_luma_processing_mode() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.window_size_info_updated!= 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] window_size_info_updated = 0. Please check ik_set_window_size_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if (video_pipe == AMBA_IK_STILL_HISO) {
        rval |= img_exe_update_check_hiso_hi(p_update_flag);
        rval |= img_exe_update_check_hiso_li2(p_update_flag);
        rval |= img_exe_update_check_hiso_asf(p_update_flag);
        rval |= img_exe_update_check_hiso_high_shpns(p_update_flag);
        rval |= img_exe_update_check_hiso_med_shpns(p_update_flag);
        rval |= img_exe_update_check_hiso_li2_shpns(p_update_flag);
        rval |= img_exe_update_check_hiso_hili_shpns(p_update_flag);
        rval |= img_exe_update_check_hiso_chroma(p_update_flag);
        rval |= img_exe_update_check_hiso_combine(p_update_flag);
        rval |= img_exe_update_check_hiso_other(p_update_flag);
        if (p_update_flag->liso.ce_out_table_updated!= 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_out_table_updated = 0. Please check ik_set_ce_out_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}

static uint32 INLINE sub1_filter_update_check_video(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;
    if ((video_pipe != AMBA_IK_VIDEO_Y2Y)&&(video_pipe != AMBA_IK_VIDEO_Y2Y_MD)) {
        if (p_update_flag->liso.sensor_information_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] sensor_information_updated = 0. Please check ik_set_vin_sensor_info() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.exp0_fe_static_blc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp0_fe_static_blc_updated = 0. Please check ik_set_exp0_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    if ((video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD)) {
        if (p_update_flag->liso.exp1_fe_static_blc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp1_fe_static_blc_updated = 0. Please check ik_set_exp1_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.exp1_fe_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] exp1_fe_wb_gain_updated = 0. Please check ik_set_exp1_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if ((video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) ||\
            (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD)) {
            if (p_update_flag->liso.exp2_fe_static_blc_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] exp2_fe_static_blc_updated = 0. Please check ik_set_exp2_frontend_static_blc() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
            if (p_update_flag->liso.exp2_fe_wb_gain_updated != 1U) {
                rval |= IK_ERR_0100;
                amba_ik_system_print_str_5("[IK][ERROR] exp2_fe_wb_gain_updated = 0. Please check ik_set_exp2_frontend_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
            }
        }
        if (p_update_flag->liso.hdr_blend_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] hdr_blend_updated = 0. Please check ik_set_hdr_blend() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    return rval;
}

static uint32 INLINE sub2_filter_update_check_video(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;
    if ((video_pipe == AMBA_IK_VIDEO_LINEAR_CE) || \
        (video_pipe == AMBA_IK_VIDEO_LINEAR_CE_MD) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) ) {
        if (p_update_flag->liso.ce_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_updated = 0. Please check ik_set_ce() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.ce_input_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] ce_input_table_updated = 0. Please check ik_set_ce_input_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.ce_out_table_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_ce_out_table() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }
    if ((video_pipe != AMBA_IK_VIDEO_Y2Y)&&(video_pipe != AMBA_IK_VIDEO_Y2Y_MD)) {
        if (p_update_flag->liso.after_ce_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] after_ce_wb_gain_updated = 0. Please check ik_set_after_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.cfa_noise_filter_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] cfa_noise_filter_updated = 0. Please check ik_set_cfa_noise_filter() API have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.demosaic_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] demosaic_updated = 0. Please check ik_set_demosaic() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.color_correction_reg_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] color_correction_reg_updated = 0. Please check ik_set_color_correction_reg() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.color_correction_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] color_correction_updated = 0. Please check ik_set_color_correction()I have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.tone_curve_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] tone_curve_updated = 0. Please check ik_set_tone_curve() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.rgb_to_yuv_matrix_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] rgb_to_yuv_matrix_updated = 0. Please check ik_set_rgb_to_yuv_matrix() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    return rval;
}
static uint32 INLINE sub3_1_filter_update_check_video(const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    if (p_update_flag->liso.fstshpns_both_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_both_updated = 0. Please check ik_set_fst_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_noise_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_noise_updated = 0. Please check ik_set_fst_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_fir_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_fir_updated = 0. Please check ik_set_fst_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_updated = 0. Please check ik_set_fst_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_coring_index_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_coring_index_scale_updated = 0. Please check ik_set_fst_shp_coring_idx_scale() have been updated.\n",
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL);
    }
    if (p_update_flag->liso.fstshpns_min_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_min_coring_result_updated = 0. Please check ik_set_fst_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fstshpns_max_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_max_coring_result_updated = 0. Please check ik_set_fst_shp_max_coring_rslt() API have been updated.\n",
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL);
    }
    if (p_update_flag->liso.fstshpns_scale_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fstshpns_scale_coring_updated = 0. Please check ik_set_fst_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static uint32 INLINE sub3_filter_update_check_video(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag, uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;
    if ((video_pipe != AMBA_IK_VIDEO_Y2Y) && (video_pipe != AMBA_IK_VIDEO_Y2Y_MD)) {
        if (p_update_flag->liso.first_luma_processing_mode_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] first_luma_processing_mode_updated = 0. Please check ik_set_fst_luma_process_mode() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        } else {
            if (use_sharpen_not_asf == 0U) {
                if (p_update_flag->liso.advance_spatial_filter_updated != 1U) {
                    rval |= IK_ERR_0100;
                    amba_ik_system_print_str_5("[IK][ERROR] advance_spatial_filter_updated = 0. Please check ik_set_adv_spatial_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
                }
            } else {
                rval |= sub3_1_filter_update_check_video(p_update_flag);
            }
        }
    }

    return rval;
}


static uint32 INLINE sub5_filter_update_check_video(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;

    if ((video_pipe != AMBA_IK_VIDEO_Y2Y)&&(video_pipe != AMBA_IK_VIDEO_Y2Y_MD)) {
        if (p_update_flag->liso.dynamic_bpc_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] dynamic_bpc_updated = 0. Please check ik_set_dynamic_bad_pixel_corr() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.cfa_leakage_filter_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] cfa_leakage_filter_updated = 0. Please check ik_set_cfa_leakage_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.anti_aliasing_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] anti_aliasing_updated = 0. Please check ik_set_anti_aliasing() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.grgb_mismatch_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] grgb_mismatch_updated = 0. Please check ik_set_grgb_mismatch() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.before_ce_wb_gain_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] before_ce_wb_gain_updated = 0. Please check ik_set_before_ce_wb_gain() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
    }

    if (p_update_flag->liso.chroma_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_scale_updated = 0. Please check ik_set_chroma_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.chroma_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_filter_updated = 0. Please check ik_set_chroma_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.chroma_median_filter_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] chroma_median_filter_updated = 0. Please check ik_set_chroma_median_filter() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }

    if (p_update_flag->liso.video_mctf_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_updated = 0. Please check ik_set_video_mctf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.video_mctf_ta_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] video_mctf_ta_updated = 0. Please check ik_set_video_mctf_ta() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    return rval;
}

static inline uint8 equal_op_u32(uint32 compare1, uint32 compare2)
{
    return (compare1 == compare2) ? 1U : 0U;
}

static uint32 INLINE img_exe_execute_video_filter_update_check(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag,uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;
    uint32 video_pipe = p_ability->video_pipe;

    rval |= sub1_filter_update_check_video(p_ability, p_update_flag);
    rval |= sub2_filter_update_check_video(p_ability, p_update_flag);
    rval |= sub3_filter_update_check_video(p_ability, p_update_flag, use_sharpen_not_asf);
    rval |= sub5_filter_update_check_video(p_ability, p_update_flag);

    if ((video_pipe == AMBA_IK_VIDEO_LINEAR_MD) || \
        (video_pipe == AMBA_IK_VIDEO_LINEAR_CE_MD) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) || \
        (video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) || \
        (video_pipe == AMBA_IK_VIDEO_FUSION_CE_MD) ) {
        if (p_update_flag->liso.motion_detect_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] motion_detect_updated = 0. Please check ik_set_motion_detect() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.motion_detect_pos_dep_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] motion_detect_updated = 0. Please check ik_set_motion_detect_pos_dep() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }
        if (p_update_flag->liso.motion_detect_and_mctf_updated != 1U) {
            rval |= IK_ERR_0100;
            amba_ik_system_print_str_5("[IK][ERROR] motion_detect_and_mctf_updated = 0. Please check ik_set_motion_detect_and_mctf() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
        }

    }
    if (p_update_flag->liso.fnlshpns_both_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_both_updated = 0. Please check ik_set_fnl_shp_both() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_noise_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_noise_updated = 0. Please check ik_set_fnl_shp_noise() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_fir_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_fir_updated = 0. Please check ik_set_fnl_shp_fir() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_coring_updated = 0. Please check ik_set_fnl_shp_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_coring_index_scale_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_coring_index_scale_updated = 0. Please check ik_set_fnl_shp_coring_idx_scale() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_min_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_min_coring_result_updated = 0. Please check ik_set_fnl_shp_min_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_max_coring_result_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_max_coring_result_updated = 0. Please check ik_set_fnl_shp_max_coring_rslt() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    if (p_update_flag->liso.fnlshpns_scale_coring_updated != 1U) {
        rval |= IK_ERR_0100;
        amba_ik_system_print_str_5("[IK][ERROR] fnlshpns_scale_coring_updated = 0. Please check ik_set_fnl_shp_scale_coring() have been updated.\n", NULL, NULL, NULL, NULL, NULL);
    }
    /*if (p_update_flag->liso.fnlshpns_both_tdt_updated != 1U) {
        rval |= IK_ERR_1000;
        amba_ik_system_print("[IK][ERROR] fnlshpns_both_tdt_updated = 0. Please check ik_set_fnl_shp_three_d_table() have been updated.\n");
    }*/
    return rval;
}
static uint32 INLINE img_exe_execute_filter_update_check(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag,uint32 use_sharpen_not_asf);
static uint32 INLINE img_exe_execute_filter_update_check(const ik_ability_t *p_ability, const amba_ik_filter_update_flags_t *p_update_flag,uint32 use_sharpen_not_asf)
{
    uint32 rval = IK_OK;
    if(p_ability->video_pipe== AMBA_IK_VIDEO_Y2Y_MIPI) {
        //do nothing;
    } else if(p_ability->pipe == AMBA_IK_PIPE_VIDEO) {
        rval |= img_exe_execute_video_filter_update_check(p_ability, p_update_flag, use_sharpen_not_asf);
    } else if(p_ability->pipe == AMBA_IK_PIPE_STILL) {
        rval |= img_exe_execute_still_filter_update_check(p_ability, p_update_flag, use_sharpen_not_asf);
    } else {
        amba_ik_system_print_uint32_5("[IK][ERROR] Not support p_ability->pipe %d.\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
    }
    return rval;
}

static uint32 img_exe_execute_first_iso_config(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_iso_config_and_state_t *iso_config_and_state;
    amba_ik_exe_first_iso_utilities_t utilities= {NULL};
    uint32 config_id = 0;
    intptr addr;
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if (p_ctx->organization.initial_flag != 0U) {
        if (p_ctx->filters.input_param.eis_mode != REGULAR_EXECUTE) {
            rval |= IK_ERR_0000;
        }
    }
    (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
    if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval |= img_ctx_get_iso_config_and_state(context_id, config_id, &addr);
        (void)amba_ik_system_memcpy(&iso_config_and_state, &addr, sizeof(void *));
        rval |= img_ctx_get_iso_config_tables(context_id, config_id, &addr);

        if (rval == IK_OK) {
            rval |= img_exe_execute_filter_update_check(&p_ctx->organization.attribute.ability, &p_ctx->filters.update_flags, p_ctx->filters.input_param.first_luma_process_mode.use_sharpen_not_asf);
            if (rval == IK_OK) {
                p_ctx->organization.active_config_state.active_iso_idx = config_id;
                (void)amba_ik_system_memset(&iso_config_and_state->table_index, 0x0, sizeof(amba_ik_iso_config_table_index_t));
                (void)amba_ik_system_memset(&iso_config_and_state->cmds, 0x0, sizeof(amba_ik_setup_cmds_t));
                (void)amba_ik_system_memset(&iso_config_and_state->iso_tables_info, 0x0, sizeof(amba_ik_cfg_tbl_description_list_t));
                (void)amba_ik_system_memcpy(&p_ctx->organization.active_config_state.active_iso_tbl_idx, &iso_config_and_state->table_index, sizeof(amba_ik_iso_config_table_index_t));
                exe_get_first_iso_config_utilities(&p_ctx->organization.attribute.ability, &utilities);
                if (utilities.compute_first_liso_config_and_tables != NULL) {
                    rval |= exe_hook_iso_config_tables(&iso_config_and_state->iso_config, &iso_config_and_state->cmds, &iso_config_and_state->table_index, &addr, REGULAR_EXECUTE);
                    rval |= utilities.compute_first_liso_config_and_tables(&iso_config_and_state->iso_config, &iso_config_and_state->cmds, &p_ctx->filters);
                    rval |= img_ctx_reset_iso_config_filter_update_flags(context_id);
                    iso_config_and_state->iso_config.calib.is_eis_warp_ready = 1U;
                    rval |= img_cfg_find_iso_config_tables(&iso_config_and_state->iso_config, &iso_config_and_state->iso_tables_info, REGULAR_EXECUTE);
                    rval |= img_cfg_clean_iso_config_and_tables_cache(&iso_config_and_state->iso_config, &iso_config_and_state->iso_tables_info);
                } else {
                    rval = IK_ERR_0005;
                }
            } else {
                amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_execute_filter_update_check(context_id %d) fail !", context_id, DC_U, DC_U, DC_U, DC_U);
            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_config(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000; // TBD
        }

    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else if (p_ctx->filters.input_param.eis_mode != REGULAR_EXECUTE) {
            amba_ik_system_print_str_5("[IK] first frame execute can not be EIS mode!", NULL, NULL, NULL, NULL, NULL);
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }
    (void)amba_ik_system_unlock_mutex(context_id);



    p_ctx->organization.executer_method.execute_iso_config = img_exe_execute_update_iso_config;
    return rval;
}

static uint32 img_exe_execute_still_hiso_config(uint32 context_id)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    amba_ik_hiso_config_and_state_t *iso_config_and_state;
    amba_ik_exe_first_iso_utilities_t utilities= {NULL};
    uint32 config_id = 0;
    intptr addr;
    rval |= img_ctx_get_context(context_id, &p_ctx);
    (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
    if((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        rval |= img_ctx_get_iso_config_and_state(context_id, config_id, &addr);
        (void)amba_ik_system_memcpy(&iso_config_and_state, &addr, sizeof(void *));
        rval |= img_ctx_get_iso_config_tables(context_id, config_id, &addr);

        if (rval == IK_OK) {
//            rval |= img_exe_execute_filter_update_check(&p_ctx->organization.attribute.ability, &p_ctx->filters.update_flags, p_ctx->filters.input_param.first_luma_process_mode.use_sharpen_not_asf);
//            if (rval == IK_OK) {
            p_ctx->organization.active_config_state.active_iso_idx = config_id;
            (void)amba_ik_system_memset(&iso_config_and_state->table_index, 0x0, sizeof(amba_ik_hiso_config_table_index_t));
            (void)amba_ik_system_memset(&iso_config_and_state->cmds, 0x0, sizeof(amba_ik_setup_cmds_t));
            (void)amba_ik_system_memset(&iso_config_and_state->iso_tables_info, 0x0, sizeof(amba_ik_cfg_tbl_description_list_t));
            (void)amba_ik_system_memcpy(&p_ctx->organization.active_config_state.active_hiso_tbl_idx, &iso_config_and_state->table_index, sizeof(amba_ik_hiso_config_table_index_t));
            exe_get_first_iso_config_utilities(&p_ctx->organization.attribute.ability, &utilities);
            if (utilities.compute_first_hiso_config_and_tables != NULL) {
                rval |= exe_hook_hiso_config_tables(&iso_config_and_state->iso_config, &iso_config_and_state->table_index, &addr);
                rval |= utilities.compute_first_hiso_config_and_tables(&iso_config_and_state->iso_config, &iso_config_and_state->cmds, &p_ctx->filters);
                rval |= img_ctx_reset_iso_config_filter_update_flags(context_id);
                rval |= img_cfg_find_hiso_config_tables(&iso_config_and_state->iso_config, &iso_config_and_state->iso_tables_info);
                rval |= img_cfg_clean_hiso_config_and_tables_cache(&iso_config_and_state->iso_config, &iso_config_and_state->iso_tables_info);
            } else {
                rval = IK_ERR_0005;
            }
//            } else {
//                amba_ik_system_print_uint32_5("[IK][ERROR] img_exe_execute_filter_update_check(context_id %d) fail !", context_id, DC_U, DC_U, DC_U, DC_U);
//            }
        } else {
            amba_ik_system_print_uint32_5("[IK] call img_ctx_get_config(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0000; // TBD
        }

    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else if (p_ctx->filters.input_param.eis_mode != REGULAR_EXECUTE) {
            amba_ik_system_print_str_5("[IK] first frame execute can not be EIS mode!", NULL, NULL, NULL, NULL, NULL);
        } else {
            amba_ik_system_print_str_5("[IK] can't get context", NULL, NULL, NULL, NULL, NULL);
        }
    }
    (void)amba_ik_system_unlock_mutex(context_id);
    return rval;
}

uint32 img_exe_init_executer(uint32 context_id)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx = NULL;
    uint8 bool_result;

    rval = img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        if (p_ctx->organization.attribute.ability.pipe == AMBA_IK_PIPE_VIDEO) {
            bool_result = equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_LINEAR) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_LINEAR_MD) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_Y2Y) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_Y2Y_MIPI) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_Y2Y_MD) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_LINEAR_CE) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_HDR_EXPO_2) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_HDR_EXPO_3) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_LINEAR_CE_MD) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_HDR_EXPO_2_MD) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_HDR_EXPO_3_MD) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_FUSION_CE_MD) + \
                          equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_FUSION_CE);
#ifdef PRE_NN_PROCESS
            bool_result += equal_op_u32(p_ctx->organization.attribute.ability.video_pipe, AMBA_IK_VIDEO_PRE_NN_PROCESS);
#endif
            if (bool_result != 0U) {
                p_ctx->organization.executer_method.execute_iso_config = img_exe_execute_first_iso_config;
            } else {
                p_ctx->organization.executer_method.execute_iso_config = img_exe_execute_invalid_config;
                amba_ik_system_print_uint32_5("[IK] not support this video_pipe %d", p_ctx->organization.attribute.ability.video_pipe, DC_U, DC_U, DC_U, DC_U);
            }
        } else if(p_ctx->organization.attribute.ability.pipe == AMBA_IK_PIPE_STILL) {
            if ((p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_LISO)) {
                p_ctx->organization.executer_method.execute_iso_config = img_exe_execute_first_iso_config;
            } else if ((p_ctx->organization.attribute.ability.still_pipe == AMBA_IK_STILL_HISO)) {
                p_ctx->organization.executer_method.execute_iso_config = img_exe_execute_still_hiso_config;
            } else {
                p_ctx->organization.executer_method.execute_iso_config = img_exe_execute_invalid_config;
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

uint32 img_exe_calc_geo_settings(const ik_calc_geo_settings_t *p_info, const ik_calc_geo_result_t *p_result)
{
    return exe_calc_geo_settings(p_info, p_result);
}

uint32 img_exe_calc_warp_resources(const ik_query_warp_t *p_info, ik_query_warp_result_t *p_result)
{
    return exe_calc_warp_resources(p_info, p_result);
}

