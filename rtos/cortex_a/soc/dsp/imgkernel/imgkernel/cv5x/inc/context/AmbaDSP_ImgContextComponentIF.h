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

#ifndef CONTEXT_COMPONENT_IF_H
#define CONTEXT_COMPONENT_IF_H

#include "AmbaDSP_ImgContextEntity.h"
#include "ik_data_type.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgAdvancedFilter.h"
#include "AmbaDSP_ImgHisoFilter.h"


uint32 img_ctx_query_context_memory_size(size_t *p_mem_size);
uint32 img_ctx_prepare_context_memory(uint32 context_id, uintptr mem_addr, size_t mem_size);
uint32 img_ctx_get_context(uint32 context_id, amba_ik_context_entity_t **p_ctx);
uint32 img_ctx_reset_update_flags(uint32 context_id);
uint32 img_ctx_init_context(uint32 context_id, const ik_ability_t *p_ability);
uint32 img_ctx_get_context_ability(uint32 context_id, ik_ability_t *p_ability);
uint32 img_ctx_get_flow_control(uint32 context_id, uint32 flow_idx, uintptr *p_flow_ctrl);
uint32 img_ctx_get_flow_tbl(uint32 context_id, uint32 flow_idx, uintptr *p_flow_tbl);
uint32 img_ctx_get_flow_tbl_list(uint32 context_id, uint32 flow_idx, uintptr *p_flow_tbl_list);
uint32 img_ctx_get_crc_data(uint32 context_id, uint32 flow_idx, uintptr *p_crc_data);

uint32 img_ctx_get_explicit_running_num(uint32 context_id, uint32 **p_explicit_running_number);
uint32 img_ctx_get_explicit_iso_running_num(uint32 context_id, uint32 **p_explicit_iso_running_number);
uint32 ctx_parse_context_id(uint32 ik_id);
uint32 ctx_parse_pipe(uint32 ik_id);
uint32 ctx_parse_ability(uint32 ik_id);
uint32 ctx_parse_running_number(uint32 ik_id);

//filters
uint32 img_ctx_ivd_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
uint32 img_ctx_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
uint32 img_ctx_ivd_set_yuv_mode(const uint32 context_id, const uint32 yuv_mode);
uint32 img_ctx_set_yuv_mode(const uint32 context_id, const uint32 yuv_mode);
uint32 img_ctx_ivd_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
uint32 img_ctx_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
uint32 img_ctx_ivd_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain);
uint32 img_ctx_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain);
uint32 img_ctx_ivd_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
uint32 img_ctx_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
uint32 img_ctx_ivd_set_stored_ir(uint32 context_id, const ik_stored_ir_t *p_stored_ir);
uint32 img_ctx_set_stored_ir(uint32 context_id, const ik_stored_ir_t *p_stored_ir);
uint32 img_ctx_ivd_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 img_ctx_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 img_ctx_ivd_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
uint32 img_ctx_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
uint32 img_ctx_ivd_set_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 img_ctx_set_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 img_ctx_ivd_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 img_ctx_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 img_ctx_ivd_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic);
uint32 img_ctx_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic);
uint32 img_ctx_ivd_set_ext_raw_out_mode(uint32 context_id, const uint32 ext_raw_out_mode);
uint32 img_ctx_set_ext_raw_out_mode(uint32 context_id, const uint32 ext_raw_out_mode);
uint32 img_ctx_ivd_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y);
uint32 img_ctx_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y);
uint32 img_ctx_ivd_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce);
uint32 img_ctx_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce);
uint32 img_ctx_ivd_set_hi_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_hi_luma_noise_reduce);
uint32 img_ctx_set_hi_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_hi_luma_noise_reduce);
uint32 img_ctx_ivd_set_li2_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_li2_luma_noise_reduce);
uint32 img_ctx_set_li2_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_li2_luma_noise_reduce);
uint32 img_ctx_ivd_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction);
uint32 img_ctx_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction);
uint32 img_ctx_ivd_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
uint32 img_ctx_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
uint32 img_ctx_ivd_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 img_ctx_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 img_ctx_ivd_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
uint32 img_ctx_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
uint32 img_ctx_ivd_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
uint32 img_ctx_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
uint32 img_ctx_ivd_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
uint32 img_ctx_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
uint32 img_ctx_ivd_set_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 img_ctx_set_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 img_ctx_ivd_set_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 img_ctx_set_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 img_ctx_ivd_set_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 img_ctx_set_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 img_ctx_ivd_set_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 img_ctx_set_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 img_ctx_ivd_set_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 img_ctx_set_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 img_ctx_ivd_set_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 img_ctx_set_fstshpns_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 img_ctx_ivd_set_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
uint32 img_ctx_set_fstshpns_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
uint32 img_ctx_ivd_set_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result);
uint32 img_ctx_set_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result);
uint32 img_ctx_ivd_set_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result);
uint32 img_ctx_set_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result);
uint32 img_ctx_ivd_set_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring);
uint32 img_ctx_set_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring);
uint32 img_ctx_ivd_set_fnlshpns_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
uint32 img_ctx_set_fnlshpns_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
uint32 img_ctx_ivd_set_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
uint32 img_ctx_set_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
uint32 img_ctx_ivd_set_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
uint32 img_ctx_set_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
uint32 img_ctx_ivd_set_fnlshpns_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
uint32 img_ctx_set_fnlshpns_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
uint32 img_ctx_ivd_set_fnlshpns_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
uint32 img_ctx_set_fnlshpns_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
uint32 img_ctx_ivd_set_fnlshpns_min_coring_result(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result);
uint32 img_ctx_set_fnlshpns_min_coring_result(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result);
uint32 img_ctx_ivd_set_fnlshpns_max_coring_result(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result);
uint32 img_ctx_set_fnlshpns_max_coring_result(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result);
uint32 img_ctx_ivd_set_fnlshpns_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring);
uint32 img_ctx_set_fnlshpns_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring);
uint32 img_ctx_ivd_set_fnlshpns_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
uint32 img_ctx_set_fnlshpns_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
uint32 img_ctx_ivd_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
uint32 img_ctx_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
uint32 img_ctx_ivd_set_wide_chroma_ft(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter);
uint32 img_ctx_set_wide_chroma_filter(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter);
uint32 img_ctx_ivd_set_wide_chroma_ft_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 img_ctx_set_wide_chroma_filter_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 img_ctx_ivd_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 img_ctx_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 img_ctx_ivd_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
uint32 img_ctx_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
uint32 img_ctx_ivd_set_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
uint32 img_ctx_set_video_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
uint32 img_ctx_ivd_set_mctf_and_final_sharpen(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
uint32 img_ctx_set_video_mctf_and_final_sharpen(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
uint32 img_ctx_ivd_set_static_bpc(const uint32 context_id,const  ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 img_ctx_ivd_set_static_bpc_internal(const uint32 context_id, const ik_static_bad_pixel_correction_internal_t *static_bpc_internal);
uint32 img_ctx_set_static_bpc(const uint32 context_id,const  ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 img_ctx_set_static_bpc_internal(const uint32 context_id, const ik_static_bad_pixel_correction_internal_t *static_bpc_internal);
uint32 img_ctx_ivd_set_static_bpc_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_get_static_bpc_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_set_static_bpc_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_get_static_bpc_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_ivd_set_vignette(const uint32 context_id, const ik_vignette_t *p_vignette);
uint32 img_ctx_set_vignette(const uint32 context_id, const ik_vignette_t *p_vignette);
uint32 img_ctx_ivd_set_vignette_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_get_vignette_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_set_vignette_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_get_vignette_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_ivd_set_vignette_internal(const uint32 context_id, const ik_vignette_t *p_vignette);
uint32 img_ctx_ivd_set_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info);
uint32 img_ctx_set_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info);
uint32 img_ctx_ivd_set_2nd_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info_2nd);
uint32 img_ctx_set_2nd_calib_warp_info(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info_2nd);

uint32 img_ctx_set_warp_bind_info(const uint32 context_id, const ik_bind_info_t *p_warp_bind_info);

uint32 img_ctx_ivd_set_calib_ca_warp_info(const uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info);
uint32 img_ctx_set_calib_ca_warp_info(const uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info);
uint32 img_ctx_ivd_set_dzoom_info(const uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
uint32 img_ctx_set_dzoom_info(const uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
uint32 img_ctx_ivd_set_hdr_shift_info_exp1(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 img_ctx_set_hdr_shift_info_exp1(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 img_ctx_ivd_set_hdr_shift_info_exp2(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 img_ctx_set_hdr_shift_info_exp2(const uint32 context_id, const ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 img_ctx_ivd_set_warp_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_set_warp_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_set_warp_2nd_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_set_warp_2nd_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_set_cawarp_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_set_cawarp_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_set_dummy_win_margin_range_info(const uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range);
uint32 img_ctx_set_dummy_win_margin_range_info(const uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range);
uint32 img_ctx_ivd_set_dmy_margin_enable_info(const uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_set_flip_mode(const uint32 context_id, const uint32 mode);
uint32 img_ctx_set_flip_mode(const uint32 context_id, const uint32 mode);
uint32 img_ctx_ivd_set_warp_buffer_info(const uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info);
uint32 img_ctx_set_warp_buffer_info(const uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info);
uint32 img_ctx_ivd_set_overlap_x(const uint32 context_id, const ik_overlap_x_t *p_overlap_x);
uint32 img_ctx_set_overlap_x(const uint32 context_id, const ik_overlap_x_t *p_overlap_x);
uint32 img_ctx_ivd_set_stitching_info(const uint32 context_id, const ik_stitch_info_t *p_stitch_info);
uint32 img_ctx_set_stitching_info(const uint32 context_id, const ik_stitch_info_t *p_stitch_info);
uint32 img_ctx_ivd_set_burst_tile(const uint32 context_id, const ik_burst_tile_t *p_burst_tile);
uint32 img_ctx_set_burst_tile(const uint32 context_id, const ik_burst_tile_t *p_burst_tile);

uint32 img_ctx_ivd_set_exp1_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc);
uint32 img_ctx_set_exp1_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc);
uint32 img_ctx_ivd_set_exp2_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc);
uint32 img_ctx_set_exp2_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc);
uint32 img_ctx_ivd_set_exp1_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
uint32 img_ctx_set_exp1_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
uint32 img_ctx_ivd_set_exp2_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
uint32 img_ctx_set_exp2_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
uint32 img_ctx_ivd_set_static_blc(uint32 context_id, const ik_static_blc_level_t *p_static_blc);
uint32 img_ctx_set_static_blc(uint32 context_id, const ik_static_blc_level_t *p_static_blc);

uint32 img_ctx_ivd_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info);
uint32 img_ctx_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info);

uint32 img_ctx_ivd_set_ce(uint32 context_id, const ik_ce_t *p_ce);
uint32 img_ctx_set_ce(uint32 context_id, const ik_ce_t *p_ce);
uint32 img_ctx_ivd_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
uint32 img_ctx_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
uint32 img_ctx_ivd_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
uint32 img_ctx_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
uint32 img_ctx_ivd_set_hdr_exp_num(const uint32 context_id, const uint32 exp_num);
uint32 img_ctx_set_hdr_exp_num(const uint32 context_id, const uint32 exp_num);
uint32 img_ctx_ivd_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);
uint32 img_ctx_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);

uint32 img_ctx_ivd_set_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 img_ctx_set_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 img_ctx_ivd_set_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
uint32 img_ctx_set_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);

uint32 img_ctx_ivd_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info);
uint32 img_ctx_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info);
uint32 img_ctx_ivd_set_aaa_pg_af_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
uint32 img_ctx_set_aaa_pg_af_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
uint32 img_ctx_ivd_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info);
uint32 img_ctx_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info);
uint32 img_ctx_ivd_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
uint32 img_ctx_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);

uint32 img_ctx_ivd_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info);
uint32 img_ctx_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info);

uint32 img_ctx_ivd_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info);
uint32 img_ctx_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info);

uint32 img_ctx_ivd_set_resampler_str(uint32 context_id, const ik_resampler_strength_t *p_resample_str);
uint32 img_ctx_set_resampler_str(uint32 context_id, const ik_resampler_strength_t *p_resample_str);

uint32 img_ctx_ivd_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info);
uint32 img_ctx_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info);
uint32 img_ctx_ivd_get_yuv_mode(const uint32 context_id, uint32 *p_yuv_mode);
uint32 img_ctx_get_yuv_mode(const uint32 context_id, uint32 *p_yuv_mode);
uint32 img_ctx_ivd_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);
uint32 img_ctx_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);
uint32 img_ctx_ivd_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain);
uint32 img_ctx_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain);
uint32 img_ctx_ivd_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);
uint32 img_ctx_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);
uint32 img_ctx_ivd_get_stored_ir(uint32 context_id, ik_stored_ir_t *p_stored_ir);
uint32 img_ctx_get_stored_ir(uint32 context_id, ik_stored_ir_t *p_stored_ir);
uint32 img_ctx_ivd_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 img_ctx_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 img_ctx_ivd_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);
uint32 img_ctx_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);
uint32 img_ctx_ivd_get_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 img_ctx_get_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 img_ctx_ivd_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 img_ctx_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 img_ctx_ivd_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic);
uint32 img_ctx_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic);
uint32 img_ctx_ivd_get_ext_raw_out_mode(uint32 context_id, uint32 *p_ext_raw_out_mode);
uint32 img_ctx_get_ext_raw_out_mode(uint32 context_id, uint32 *p_ext_raw_out_mode);
uint32 img_ctx_ivd_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y);
uint32 img_ctx_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y);
uint32 img_ctx_ivd_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce);
uint32 img_ctx_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce);
uint32 img_ctx_ivd_get_hi_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_hi_luma_noise_reduce);
uint32 img_ctx_get_hi_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_hi_luma_noise_reduce);
uint32 img_ctx_ivd_get_li2_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_li2_luma_noise_reduce);
uint32 img_ctx_get_li2_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_li2_luma_noise_reduce);
uint32 img_ctx_ivd_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction);
uint32 img_ctx_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction);
uint32 img_ctx_ivd_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve);
uint32 img_ctx_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve);
uint32 img_ctx_ivd_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 img_ctx_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 img_ctx_ivd_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);
uint32 img_ctx_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);
uint32 img_ctx_ivd_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale);
uint32 img_ctx_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale);
uint32 img_ctx_ivd_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter);
uint32 img_ctx_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter);
uint32 img_ctx_ivd_get_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 img_ctx_get_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 img_ctx_ivd_get_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 img_ctx_get_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 img_ctx_ivd_get_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 img_ctx_get_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 img_ctx_ivd_get_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 img_ctx_get_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 img_ctx_ivd_get_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 img_ctx_get_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 img_ctx_ivd_get_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 img_ctx_get_fstshpns_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 img_ctx_ivd_get_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale);
uint32 img_ctx_get_fstshpns_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale);
uint32 img_ctx_ivd_get_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result);
uint32 img_ctx_get_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result);
uint32 img_ctx_ivd_get_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result);
uint32 img_ctx_get_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result);
uint32 img_ctx_ivd_get_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring);
uint32 img_ctx_get_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring);
uint32 img_ctx_ivd_get_fnlshpns_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both);
uint32 img_ctx_get_fnlshpns_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both);
uint32 img_ctx_ivd_get_fnlshpns_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise);
uint32 img_ctx_get_fnlshpns_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise);
uint32 img_ctx_ivd_get_fnlshpns_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir);
uint32 img_ctx_get_fnlshpns_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir);
uint32 img_ctx_ivd_get_fnlshpns_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring);
uint32 img_ctx_get_fnlshpns_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring);
uint32 img_ctx_ivd_get_fnlshpns_cor_idx_scl(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_fnlshpns_coring_index_scale);
uint32 img_ctx_get_fnlshpns_cor_idx_scl(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_fnlshpns_coring_index_scale);
uint32 img_ctx_ivd_get_fnlshpns_min_coring_result(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result);
uint32 img_ctx_get_fnlshpns_min_coring_result(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result);
uint32 img_ctx_ivd_get_fnlshpns_max_coring_result(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result);
uint32 img_ctx_get_fnlshpns_max_coring_result(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result);
uint32 img_ctx_ivd_get_fnlshpns_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring);
uint32 img_ctx_get_fnlshpns_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring);
uint32 img_ctx_ivd_get_fnlshpns_both_tdt(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
uint32 img_ctx_get_fnlshpns_both_tdt(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
uint32 img_ctx_ivd_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);
uint32 img_ctx_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);
uint32 img_ctx_ivd_get_wide_chroma_ft(uint32 context_id, ik_wide_chroma_filter_t *p_wide_chroma_filter);
uint32 img_ctx_get_wide_chroma_filter(uint32 context_id, ik_wide_chroma_filter_t *p_wide_chroma_filter);
uint32 img_ctx_ivd_get_wide_chroma_ft_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 img_ctx_get_wide_chroma_filter_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 img_ctx_ivd_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 img_ctx_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 img_ctx_ivd_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf);
uint32 img_ctx_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf);
uint32 img_ctx_ivd_get_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta);
uint32 img_ctx_get_video_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta);
uint32 img_ctx_ivd_get_mctf_and_final_sharpen(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
uint32 img_ctx_get_video_mctf_and_final_sharpen(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
uint32 img_ctx_ivd_get_static_bpc(const uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 img_ctx_ivd_get_static_bpc_internal(const uint32 context_id,ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
uint32 img_ctx_get_static_bpc(const uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 img_ctx_get_static_bpc_internal(const uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
uint32 img_ctx_ivd_get_vignette(const uint32 context_id, ik_vignette_t *p_vignette);
uint32 img_ctx_get_vignette(const uint32 context_id, ik_vignette_t *p_vignette);
uint32 img_ctx_ivd_set_motion_detect(uint32 context_id, const ik_motion_detect_t *p_motion_detect);
uint32 img_ctx_ivd_get_motion_detect(uint32 context_id, ik_motion_detect_t *p_motion_detect);
uint32 img_ctx_set_motion_detect(uint32 context_id, const ik_motion_detect_t *p_motion_detect);
uint32 img_ctx_get_motion_detect(uint32 context_id, ik_motion_detect_t *p_motion_detect);
uint32 img_ctx_ivd_set_motion_detect_pos_dep(uint32 context_id, const ik_pos_dep33_t *p_md_pos_dep);
uint32 img_ctx_set_motion_detect_pos_dep(uint32 context_id, const ik_pos_dep33_t *p_md_pos_dep);
uint32 img_ctx_ivd_get_motion_detect_pos_dep(uint32 context_id, ik_pos_dep33_t *p_pos_dep);
uint32 img_ctx_get_motion_detect_pos_dep(uint32 context_id, ik_pos_dep33_t *p_pos_dep);
uint32 img_ctx_ivd_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
uint32 img_ctx_ivd_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
uint32 img_ctx_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
uint32 img_ctx_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);

uint32 img_ctx_ivd_get_exp1_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc);
uint32 img_ctx_get_exp1_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc);
uint32 img_ctx_ivd_get_exp2_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc);
uint32 img_ctx_get_exp2_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc);
uint32 img_ctx_ivd_get_exp1_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
uint32 img_ctx_get_exp1_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
uint32 img_ctx_ivd_get_exp2_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
uint32 img_ctx_get_exp2_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);

uint32 img_ctx_ivd_get_static_blc(uint32 context_id, ik_static_blc_level_t *p_static_blc);
uint32 img_ctx_get_static_blc(uint32 context_id, ik_static_blc_level_t *p_static_blc);
uint32 img_ctx_ivd_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_raw_info);
uint32 img_ctx_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_raw_info);

uint32 img_ctx_ivd_get_ce(uint32 context_id, ik_ce_t *p_ce);
uint32 img_ctx_get_ce(uint32 context_id, ik_ce_t *p_ce);

uint32 img_ctx_ivd_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);
uint32 img_ctx_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);
uint32 img_ctx_ivd_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);
uint32 img_ctx_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);
uint32 img_ctx_ivd_get_hdr_exp_num(const uint32 context_id, uint32 *p_hdr_exp_num);
uint32 img_ctx_get_hdr_exp_num(const uint32 context_id, uint32 *p_hdr_exp_num);
uint32 img_ctx_ivd_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend);
uint32 img_ctx_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend);

uint32 img_ctx_ivd_get_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 img_ctx_get_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 img_ctx_ivd_get_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
uint32 img_ctx_get_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);

uint32 img_ctx_ivd_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info);
uint32 img_ctx_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info);
uint32 img_ctx_ivd_get_aaa_pg_af_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
uint32 img_ctx_get_aaa_pg_af_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
uint32 img_ctx_ivd_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info);
uint32 img_ctx_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info);
uint32 img_ctx_ivd_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
uint32 img_ctx_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);

uint32 img_ctx_ivd_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 img_ctx_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 img_ctx_ivd_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);
uint32 img_ctx_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

uint32 img_ctx_ivd_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 img_ctx_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 img_ctx_ivd_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);
uint32 img_ctx_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

uint32 img_ctx_ivd_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info);
uint32 img_ctx_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info);

uint32 img_ctx_ivd_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info);
uint32 img_ctx_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info);

uint32 img_ctx_ivd_get_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info);
uint32 img_ctx_get_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info);
uint32 img_ctx_ivd_get_2nd_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info_2nd);
uint32 img_ctx_get_2nd_calib_warp_info(const uint32 context_id, ik_warp_info_t *p_calib_warp_info_2nd);

uint32 img_ctx_get_warp_bind_info(const uint32 context_id, ik_bind_info_t *p_warp_bind_info);

uint32 img_ctx_ivd_get_calib_ca_warp_info(const uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info);
uint32 img_ctx_get_calib_ca_warp_info(const uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info);

uint32 img_ctx_ivd_get_dzoom_info(const uint32 context_id, ik_dzoom_info_t *p_dzoom_info);
uint32 img_ctx_get_dzoom_info(const uint32 context_id, ik_dzoom_info_t *p_dzoom_info);

uint32 img_ctx_ivd_get_hdr_shift_info_exp1(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 img_ctx_get_hdr_shift_info_exp1(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info);

uint32 img_ctx_ivd_get_hdr_shift_info_exp2(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info);
uint32 img_ctx_get_hdr_shift_info_exp2(const uint32 context_id, ik_hdr_eis_info_t *p_hdr_sft_info);

uint32 img_ctx_ivd_get_warp_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_get_warp_enable_info(const uint32 context_id, uint32 *p_enable);

uint32 img_ctx_ivd_get_warp_2nd_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_get_warp_2nd_enable_info(const uint32 context_id, uint32 *p_enable);

uint32 img_ctx_ivd_get_cawarp_enable_info(const uint32 context_id, uint32 *p_enable);
uint32 img_ctx_get_cawarp_enable_info(const uint32 context_id, uint32 *p_enable);

uint32 img_ctx_ivd_get_dummy_win_margin_range_info(const uint32 context_id, ik_dummy_margin_range_t *p_dmy_range);
uint32 img_ctx_get_dummy_win_margin_range_info(const uint32 context_id, ik_dummy_margin_range_t *p_dmy_range);

uint32 img_ctx_ivd_set_vin_active_window(const uint32 context_id, const ik_vin_active_window_t *p_active_win);
uint32 img_ctx_set_vin_active_window(const uint32 context_id, const ik_vin_active_window_t *p_active_win);

uint32 img_ctx_ivd_get_vin_active_window(const uint32 context_id, ik_vin_active_window_t *p_active_win);
uint32 img_ctx_get_vin_active_window(const uint32 context_id, ik_vin_active_window_t *p_active_win);

uint32 img_ctx_ivd_set_warp_internal(const uint32 context_id, const ik_warp_internal_info_t *p_warp_internal);
uint32 img_ctx_set_warp_internal(const uint32 context_id, const ik_warp_internal_info_t *p_warp_internal);

uint32 img_ctx_ivd_set_warp_2nd_internal(const uint32 context_id, const ik_warp_2nd_internal_info_t *p_warp_2nd_internal);
uint32 img_ctx_set_warp_2nd_internal(const uint32 context_id, const ik_warp_2nd_internal_info_t *p_warp_2nd_internal);

uint32 img_ctx_ivd_set_cawarp_internal(const uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal);
uint32 img_ctx_set_cawarp_internal(const uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal);

uint32 img_ctx_ivd_get_warp_internal(const uint32 context_id, ik_warp_internal_info_t *p_warp_internal);
uint32 img_ctx_get_warp_internal(const uint32 context_id, ik_warp_internal_info_t *p_warp_internal);

uint32 img_ctx_ivd_get_warp_2nd_internal(const uint32 context_id, ik_warp_2nd_internal_info_t *p_warp_2nd_internal);
uint32 img_ctx_get_warp_2nd_internal(const uint32 context_id, ik_warp_2nd_internal_info_t *p_warp_2nd_internal);

uint32 img_ctx_ivd_get_cawarp_internal(const uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal);
uint32 img_ctx_get_cawarp_internal(const uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal);

uint32 img_ctx_ivd_set_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_video_mctf_internal);
uint32 img_ctx_set_video_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_video_mctf_internal);

uint32 img_ctx_ivd_get_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal);
uint32 img_ctx_get_video_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal);
uint32 img_ctx_get_win_result(const uint32 context_id, amba_ik_calc_win_result_t *p_win_result);

uint32 img_ctx_ivd_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc);
uint32 img_ctx_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc);
uint32 img_ctx_ivd_get_fe_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc);
uint32 img_ctx_get_fe_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc);
uint32 img_ctx_ivd_get_resampler_str(uint32 context_id, ik_resampler_strength_t *p_resample_str);
uint32 img_ctx_get_resampler_str(uint32 context_id, ik_resampler_strength_t *p_resample_str);

uint32 img_ctx_ivd_get_flip_mode(const uint32 context_id, uint32 *mode);
uint32 img_ctx_get_flip_mode(const uint32 context_id, uint32 *mode);
uint32 img_ctx_ivd_get_warp_buffer_info(const uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info);
uint32 img_ctx_get_warp_buffer_info(const uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info);
uint32 img_ctx_ivd_get_overlap_x(const uint32 context_id, ik_overlap_x_t *p_overlap_x);
uint32 img_ctx_get_overlap_x(const uint32 context_id, ik_overlap_x_t *p_overlap_x);
uint32 img_ctx_ivd_get_stitching_info(const uint32 context_id, ik_stitch_info_t *p_stitch_info);
uint32 img_ctx_get_stitching_info(const uint32 context_id, ik_stitch_info_t *p_stitch_info);
uint32 img_ctx_ivd_get_burst_tile(const uint32 context_id, ik_burst_tile_t *p_burst_tile);
uint32 img_ctx_get_burst_tile(const uint32 context_id, ik_burst_tile_t *p_burst_tile);

uint32 img_ctx_ivd_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable);
uint32 img_ctx_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable);
uint32 img_ctx_ivd_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *p_enable);
uint32 img_ctx_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *p_enable);

//hiso
uint32 img_ctx_ivd_set_hi_anti_aliasing(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing);
uint32 img_ctx_set_hi_anti_aliasing(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing);
uint32 img_ctx_ivd_get_hi_anti_aliasing(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing);
uint32 img_ctx_get_hi_anti_aliasing(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing);

uint32 img_ctx_ivd_set_hi_cfa_leakage_filter(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);
uint32 img_ctx_set_hi_cfa_leakage_filter(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);
uint32 img_ctx_ivd_get_hi_cfa_leakage_filter(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);
uint32 img_ctx_get_hi_cfa_leakage_filter(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);

uint32 img_ctx_ivd_set_hi_dynamic_bpc(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);
uint32 img_ctx_set_hi_dynamic_bpc(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);
uint32 img_ctx_ivd_get_hi_dynamic_bpc(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);
uint32 img_ctx_get_hi_dynamic_bpc(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);

uint32 img_ctx_ivd_set_hi_grgb_mismatch(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);
uint32 img_ctx_set_hi_grgb_mismatch(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);
uint32 img_ctx_ivd_get_hi_grgb_mismatch(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);
uint32 img_ctx_get_hi_grgb_mismatch(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);

uint32 img_ctx_ivd_set_hi_chroma_median_filter(uint32 context_id, const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);
uint32 img_ctx_set_hi_chroma_median_filter(uint32 context_id, const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);
uint32 img_ctx_ivd_get_hi_chroma_median_filter(uint32 context_id, ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);
uint32 img_ctx_get_hi_chroma_median_filter(uint32 context_id, ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);

uint32 img_ctx_ivd_set_hi_cfa_noise_filter(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);
uint32 img_ctx_set_hi_cfa_noise_filter(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);
uint32 img_ctx_ivd_get_hi_cfa_noise_filter(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);
uint32 img_ctx_get_hi_cfa_noise_filter(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);

uint32 img_ctx_ivd_set_hi_demosaic(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic);
uint32 img_ctx_set_hi_demosaic(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic);
uint32 img_ctx_ivd_get_hi_demosaic(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic);
uint32 img_ctx_get_hi_demosaic(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic);

uint32 img_ctx_ivd_set_li2_anti_aliasing(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing);
uint32 img_ctx_set_li2_anti_aliasing(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing);
uint32 img_ctx_ivd_get_li2_anti_aliasing(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing);
uint32 img_ctx_get_li2_anti_aliasing(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing);

uint32 img_ctx_ivd_set_li2_cfa_leakage_filter(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);
uint32 img_ctx_set_li2_cfa_leakage_filter(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);
uint32 img_ctx_ivd_get_li2_cfa_leakage_filter(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);
uint32 img_ctx_get_li2_cfa_leakage_filter(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);

uint32 img_ctx_ivd_set_li2_dynamic_bpc(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);
uint32 img_ctx_set_li2_dynamic_bpc(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);
uint32 img_ctx_ivd_get_li2_dynamic_bpc(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);
uint32 img_ctx_get_li2_dynamic_bpc(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);

uint32 img_ctx_ivd_set_li2_grgb_mismatch(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);
uint32 img_ctx_set_li2_grgb_mismatch(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);
uint32 img_ctx_ivd_get_li2_grgb_mismatch(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);
uint32 img_ctx_get_li2_grgb_mismatch(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);

uint32 img_ctx_ivd_set_li2_cfa_noise_filter(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);
uint32 img_ctx_set_li2_cfa_noise_filter(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);
uint32 img_ctx_ivd_get_li2_cfa_noise_filter(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);
uint32 img_ctx_get_li2_cfa_noise_filter(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);

uint32 img_ctx_ivd_set_li2_demosaic(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic);
uint32 img_ctx_set_li2_demosaic(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic);
uint32 img_ctx_ivd_get_li2_demosaic(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic);
uint32 img_ctx_get_li2_demosaic(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic);

uint32 img_ctx_ivd_set_hi_asf(uint32 context_id, const ik_hi_asf_t *p_hi_asf);
uint32 img_ctx_set_hi_asf(uint32 context_id, const ik_hi_asf_t *p_hi_asf);
uint32 img_ctx_ivd_get_hi_asf(uint32 context_id, ik_hi_asf_t *p_hi_asf);
uint32 img_ctx_get_hi_asf(uint32 context_id, ik_hi_asf_t *p_hi_asf);

uint32 img_ctx_ivd_set_li2_asf(uint32 context_id, const ik_li2_asf_t *p_li2_asf);
uint32 img_ctx_set_li2_asf(uint32 context_id, const ik_li2_asf_t *p_li2_asf);
uint32 img_ctx_ivd_get_li2_asf(uint32 context_id, ik_li2_asf_t *p_li2_asf);
uint32 img_ctx_get_li2_asf(uint32 context_id, ik_li2_asf_t *p_li2_asf);

uint32 img_ctx_ivd_set_hi_low_asf(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf);
uint32 img_ctx_set_hi_low_asf(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf);
uint32 img_ctx_ivd_get_hi_low_asf(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf);
uint32 img_ctx_get_hi_low_asf(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf);

uint32 img_ctx_ivd_set_hi_med1_asf(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf);
uint32 img_ctx_set_hi_med1_asf(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf);
uint32 img_ctx_ivd_get_hi_med1_asf(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf);
uint32 img_ctx_get_hi_med1_asf(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf);

uint32 img_ctx_ivd_set_hi_med2_asf(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf);
uint32 img_ctx_set_hi_med2_asf(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf);
uint32 img_ctx_ivd_get_hi_med2_asf(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf);
uint32 img_ctx_get_hi_med2_asf(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf);

uint32 img_ctx_ivd_set_hi_high_asf(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf);
uint32 img_ctx_set_hi_high_asf(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf);
uint32 img_ctx_ivd_get_hi_high_asf(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf);
uint32 img_ctx_get_hi_high_asf(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf);

uint32 img_ctx_ivd_set_hi_high2_asf(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf);
uint32 img_ctx_set_hi_high2_asf(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf);
uint32 img_ctx_ivd_get_hi_high2_asf(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf);
uint32 img_ctx_get_hi_high2_asf(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf);

uint32 img_ctx_ivd_set_li_chroma_asf(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf);
uint32 img_ctx_set_li_chroma_asf(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf);
uint32 img_ctx_ivd_get_li_chroma_asf(uint32 context_id, ik_chroma_asf_t *p_chroma_asf);
uint32 img_ctx_get_li_chroma_asf(uint32 context_id, ik_chroma_asf_t *p_chroma_asf);

uint32 img_ctx_ivd_set_hi_chroma_asf(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf);
uint32 img_ctx_set_hi_chroma_asf(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf);
uint32 img_ctx_ivd_get_hi_chroma_asf(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf);
uint32 img_ctx_get_hi_chroma_asf(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf);

uint32 img_ctx_ivd_set_hi_low_chroma_asf(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);
uint32 img_ctx_set_hi_low_chroma_asf(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);
uint32 img_ctx_ivd_get_hi_low_chroma_asf(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);
uint32 img_ctx_get_hi_low_chroma_asf(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);

uint32 img_ctx_ivd_set_hi_high_shpns_both(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both);
uint32 img_ctx_set_hi_high_shpns_both(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both);
uint32 img_ctx_ivd_get_hi_high_shpns_both(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both);
uint32 img_ctx_get_hi_high_shpns_both(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both);

uint32 img_ctx_ivd_set_hi_high_shpns_noise(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);
uint32 img_ctx_set_hi_high_shpns_noise(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);
uint32 img_ctx_ivd_get_hi_high_shpns_noise(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);
uint32 img_ctx_get_hi_high_shpns_noise(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);

uint32 img_ctx_ivd_set_hi_high_shpns_coring(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);
uint32 img_ctx_set_hi_high_shpns_coring(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);
uint32 img_ctx_ivd_get_hi_high_shpns_coring(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);
uint32 img_ctx_get_hi_high_shpns_coring(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);

uint32 img_ctx_ivd_set_hi_high_shpns_fir(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);
uint32 img_ctx_set_hi_high_shpns_fir(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);
uint32 img_ctx_ivd_get_hi_high_shpns_fir(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);
uint32 img_ctx_get_hi_high_shpns_fir(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);

uint32 img_ctx_ivd_set_hi_high_shp_cor_idx_scl(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);
uint32 img_ctx_set_hi_high_shpns_cor_idx_scl(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);
uint32 img_ctx_ivd_get_hi_high_shp_cor_idx_scl(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);
uint32 img_ctx_get_hi_high_shpns_cor_idx_scl(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);

uint32 img_ctx_ivd_set_hi_high_shp_min_cor_rst(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);
uint32 img_ctx_set_hi_high_shpns_min_cor_rst(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);
uint32 img_ctx_ivd_get_hi_high_shp_min_cor_rst(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);
uint32 img_ctx_get_hi_high_shpns_min_cor_rst(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);

uint32 img_ctx_ivd_set_hi_high_shp_max_cor_rst(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);
uint32 img_ctx_set_hi_high_shpns_max_cor_rst(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);
uint32 img_ctx_ivd_get_hi_high_shp_max_cor_rst(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);
uint32 img_ctx_get_hi_high_shpns_max_cor_rst(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);

uint32 img_ctx_ivd_set_hi_high_shpns_scl_cor(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);
uint32 img_ctx_set_hi_high_shpns_scl_cor(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);
uint32 img_ctx_ivd_get_hi_high_shpns_scl_cor(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);
uint32 img_ctx_get_hi_high_shpns_scl_cor(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);

uint32 img_ctx_ivd_set_hi_med_shpns_both(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both);
uint32 img_ctx_set_hi_med_shpns_both(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both);
uint32 img_ctx_ivd_get_hi_med_shpns_both(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both);
uint32 img_ctx_get_hi_med_shpns_both(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both);

uint32 img_ctx_ivd_set_hi_med_shpns_noise(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);
uint32 img_ctx_set_hi_med_shpns_noise(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);
uint32 img_ctx_ivd_get_hi_med_shpns_noise(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);
uint32 img_ctx_get_hi_med_shpns_noise(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);

uint32 img_ctx_ivd_set_hi_med_shpns_coring(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);
uint32 img_ctx_set_hi_med_shpns_coring(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);
uint32 img_ctx_ivd_get_hi_med_shpns_coring(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);
uint32 img_ctx_get_hi_med_shpns_coring(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);

uint32 img_ctx_ivd_set_hi_med_shpns_fir(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);
uint32 img_ctx_set_hi_med_shpns_fir(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);
uint32 img_ctx_ivd_get_hi_med_shpns_fir(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);
uint32 img_ctx_get_hi_med_shpns_fir(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);

uint32 img_ctx_ivd_set_hi_med_shp_cor_idx_scl(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);
uint32 img_ctx_set_hi_med_shpns_cor_idx_scl(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);
uint32 img_ctx_ivd_get_hi_med_shp_cor_idx_scl(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);
uint32 img_ctx_get_hi_med_shpns_cor_idx_scl(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);

uint32 img_ctx_ivd_set_hi_med_shpns_min_cor_rst(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);
uint32 img_ctx_set_hi_med_shpns_min_cor_rst(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);
uint32 img_ctx_ivd_get_hi_med_shpns_min_cor_rst(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);
uint32 img_ctx_get_hi_med_shpns_min_cor_rst(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);

uint32 img_ctx_ivd_set_hi_med_shpns_max_cor_rst(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);
uint32 img_ctx_set_hi_med_shpns_max_cor_rst(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);
uint32 img_ctx_ivd_get_hi_med_shpns_max_cor_rst(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);
uint32 img_ctx_get_hi_med_shpns_max_cor_rst(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);

uint32 img_ctx_ivd_set_hi_med_shpns_scl_cor(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);
uint32 img_ctx_set_hi_med_shpns_scl_cor(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);
uint32 img_ctx_ivd_get_hi_med_shpns_scl_cor(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);
uint32 img_ctx_get_hi_med_shpns_scl_cor(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);

uint32 img_ctx_ivd_set_li2_shpns_both(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both);
uint32 img_ctx_set_li2_shpns_both(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both);
uint32 img_ctx_ivd_get_li2_shpns_both(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both);
uint32 img_ctx_get_li2_shpns_both(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both);

uint32 img_ctx_ivd_set_li2_shpns_noise(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise);
uint32 img_ctx_set_li2_shpns_noise(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise);
uint32 img_ctx_ivd_get_li2_shpns_noise(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise);
uint32 img_ctx_get_li2_shpns_noise(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise);

uint32 img_ctx_ivd_set_li2_shpns_coring(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring);
uint32 img_ctx_set_li2_shpns_coring(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring);
uint32 img_ctx_ivd_get_li2_shpns_coring(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring);
uint32 img_ctx_get_li2_shpns_coring(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring);

uint32 img_ctx_ivd_set_li2_shpns_fir(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir);
uint32 img_ctx_set_li2_shpns_fir(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir);
uint32 img_ctx_ivd_get_li2_shpns_fir(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir);
uint32 img_ctx_get_li2_shpns_fir(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir);

uint32 img_ctx_ivd_set_li2_shpns_cor_idx_scl(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);
uint32 img_ctx_set_li2_shpns_cor_idx_scl(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);
uint32 img_ctx_ivd_get_li2_shpns_cor_idx_scl(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);
uint32 img_ctx_get_li2_shpns_cor_idx_scl(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);

uint32 img_ctx_ivd_set_li2_shpns_min_cor_rst(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);
uint32 img_ctx_set_li2_shpns_min_cor_rst(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);
uint32 img_ctx_ivd_get_li2_shpns_min_cor_rst(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);
uint32 img_ctx_get_li2_shpns_min_cor_rst(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);

uint32 img_ctx_ivd_set_li2_shpns_max_cor_rst(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);
uint32 img_ctx_set_li2_shpns_max_cor_rst(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);
uint32 img_ctx_ivd_get_li2_shpns_max_cor_rst(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);
uint32 img_ctx_get_li2_shpns_max_cor_rst(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);

uint32 img_ctx_ivd_set_li2_shpns_scl_cor(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);
uint32 img_ctx_set_li2_shpns_scl_cor(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);
uint32 img_ctx_ivd_get_li2_shpns_scl_cor(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);
uint32 img_ctx_get_li2_shpns_scl_cor(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);

uint32 img_ctx_ivd_set_hili_shpns_both(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both);
uint32 img_ctx_set_hili_shpns_both(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both);
uint32 img_ctx_ivd_get_hili_shpns_both(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both);
uint32 img_ctx_get_hili_shpns_both(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both);

uint32 img_ctx_ivd_set_hili_shpns_noise(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise);
uint32 img_ctx_set_hili_shpns_noise(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise);
uint32 img_ctx_ivd_get_hili_shpns_noise(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise);
uint32 img_ctx_get_hili_shpns_noise(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise);

uint32 img_ctx_ivd_set_hili_shpns_coring(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring);
uint32 img_ctx_set_hili_shpns_coring(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring);
uint32 img_ctx_ivd_get_hili_shpns_coring(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring);
uint32 img_ctx_get_hili_shpns_coring(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring);

uint32 img_ctx_ivd_set_hili_shpns_fir(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir);
uint32 img_ctx_set_hili_shpns_fir(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir);
uint32 img_ctx_ivd_get_hili_shpns_fir(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir);
uint32 img_ctx_get_hili_shpns_fir(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir);

uint32 img_ctx_ivd_set_hili_shpns_cor_idx_scl(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);
uint32 img_ctx_set_hili_shpns_cor_idx_scl(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);
uint32 img_ctx_ivd_get_hili_shpns_cor_idx_scl(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);
uint32 img_ctx_get_hili_shpns_cor_idx_scl(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);

uint32 img_ctx_ivd_set_hili_shpns_min_cor_rst(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);
uint32 img_ctx_set_hili_shpns_min_cor_rst(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);
uint32 img_ctx_ivd_get_hili_shpns_min_cor_rst(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);
uint32 img_ctx_get_hili_shpns_min_cor_rst(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);

uint32 img_ctx_ivd_set_hili_shpns_max_cor_rst(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);
uint32 img_ctx_set_hili_shpns_max_cor_rst(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);
uint32 img_ctx_ivd_get_hili_shpns_max_cor_rst(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);
uint32 img_ctx_get_hili_shpns_max_cor_rst(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);

uint32 img_ctx_ivd_set_hili_shpns_scl_cor(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);
uint32 img_ctx_set_hili_shpns_scl_cor(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);
uint32 img_ctx_ivd_get_hili_shpns_scl_cor(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);
uint32 img_ctx_get_hili_shpns_scl_cor(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);

uint32 img_ctx_ivd_set_hi_chrm_fltr_high(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);
uint32 img_ctx_set_hi_chroma_filter_high(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);
uint32 img_ctx_ivd_get_hi_chrm_fltr_high(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);
uint32 img_ctx_get_hi_chroma_filter_high(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);

uint32 img_ctx_ivd_set_hi_chrm_fltr_pre(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);
uint32 img_ctx_set_hi_chroma_filter_pre(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);
uint32 img_ctx_ivd_get_hi_chrm_fltr_pre(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);
uint32 img_ctx_get_hi_chroma_filter_pre(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);

uint32 img_ctx_ivd_set_hi_chrm_fltr_med(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);
uint32 img_ctx_set_hi_chroma_filter_med(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);
uint32 img_ctx_ivd_get_hi_chrm_fltr_med(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);
uint32 img_ctx_get_hi_chroma_filter_med(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);

uint32 img_ctx_ivd_set_hi_chrm_filter_low(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);
uint32 img_ctx_set_hi_chroma_filter_low(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);
uint32 img_ctx_ivd_get_hi_chrm_filter_low(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);
uint32 img_ctx_get_hi_chroma_filter_low(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);

uint32 img_ctx_ivd_set_hi_chrm_fltr_very_low(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);
uint32 img_ctx_set_hi_chroma_filter_very_low(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);
uint32 img_ctx_ivd_get_hi_chrm_fltr_very_low(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);
uint32 img_ctx_get_hi_chroma_filter_very_low(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);

uint32 img_ctx_ivd_set_hi_luma_combine(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine);
uint32 img_ctx_set_hi_luma_combine(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine);
uint32 img_ctx_ivd_get_hi_luma_combine(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine);
uint32 img_ctx_get_hi_luma_combine(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine);

uint32 img_ctx_ivd_set_hi_low_asf_combine(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine);
uint32 img_ctx_set_hi_low_asf_combine(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine);
uint32 img_ctx_ivd_get_hi_low_asf_combine(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine);
uint32 img_ctx_get_hi_low_asf_combine(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine);

uint32 img_ctx_ivd_set_hi_chroma_fltr_med_com(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);
uint32 img_ctx_set_hi_chroma_fltr_med_com(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);
uint32 img_ctx_ivd_get_hi_chroma_fltr_med_com(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);
uint32 img_ctx_get_hi_chroma_fltr_med_com(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);

uint32 img_ctx_ivd_set_hi_chrm_fltr_low_com(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);
uint32 img_ctx_set_hi_chroma_fltr_low_com(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);
uint32 img_ctx_ivd_get_hi_chrm_fltr_low_com(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);
uint32 img_ctx_get_hi_chroma_fltr_low_com(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);

uint32 img_ctx_ivd_set_hi_chm_fltr_very_low_com(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);
uint32 img_ctx_set_hi_chroma_fltr_very_low_com(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);
uint32 img_ctx_ivd_get_hi_chm_fltr_very_low_com(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);
uint32 img_ctx_get_hi_chroma_fltr_very_low_com(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);

uint32 img_ctx_ivd_set_hili_combine(uint32 context_id, const ik_hili_combine_t *p_hili_combine);
uint32 img_ctx_set_hili_combine(uint32 context_id, const ik_hili_combine_t *p_hili_combine);
uint32 img_ctx_ivd_get_hili_combine(uint32 context_id, ik_hili_combine_t *p_hili_combine);
uint32 img_ctx_get_hili_combine(uint32 context_id, ik_hili_combine_t *p_hili_combine);

uint32 img_ctx_ivd_set_hi_mid_high_freq_recover(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);
uint32 img_ctx_set_hi_mid_high_freq_recover(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);
uint32 img_ctx_ivd_get_hi_mid_high_freq_recover(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);
uint32 img_ctx_get_hi_mid_high_freq_recover(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);

uint32 img_ctx_ivd_set_hi_luma_blend(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend);
uint32 img_ctx_set_hi_luma_blend(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend);
uint32 img_ctx_ivd_get_hi_luma_blend(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend);
uint32 img_ctx_get_hi_luma_blend(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend);

uint32 img_ctx_ivd_set_hi_nonsmooth_detect(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);
uint32 img_ctx_set_hi_nonsmooth_detect(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);
uint32 img_ctx_ivd_get_hi_nonsmooth_detect(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);
uint32 img_ctx_get_hi_nonsmooth_detect(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);

uint32 img_ctx_ivd_set_hi_select(uint32 context_id, const ik_hi_select_t *p_hi_select);
uint32 img_ctx_set_hi_select(uint32 context_id, const ik_hi_select_t *p_hi_select);
uint32 img_ctx_ivd_get_hi_select(uint32 context_id, ik_hi_select_t *p_hi_select);
uint32 img_ctx_get_hi_select(uint32 context_id, ik_hi_select_t *p_hi_select);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter(uint32 context_id, const ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);
uint32 img_ctx_set_hi_wide_chroma_filter(uint32 context_id, const ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter(uint32 context_id, ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);
uint32 img_ctx_get_hi_wide_chroma_filter(uint32 context_id, ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_combine(uint32 context_id, const ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);
uint32 img_ctx_set_hi_wide_chroma_filter_combine(uint32 context_id, const ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_combine(uint32 context_id, ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);
uint32 img_ctx_get_hi_wide_chroma_filter_combine(uint32 context_id, ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);

uint32 img_ctx_ivd_set_li2_wide_chroma_filter(uint32 context_id, const ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);
uint32 img_ctx_set_li2_wide_chroma_filter(uint32 context_id, const ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);
uint32 img_ctx_ivd_get_li2_wide_chroma_filter(uint32 context_id, ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);
uint32 img_ctx_get_li2_wide_chroma_filter(uint32 context_id, ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);

uint32 img_ctx_ivd_set_li2_wide_chroma_filter_combine(uint32 context_id, const ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);
uint32 img_ctx_set_li2_wide_chroma_filter_combine(uint32 context_id, const ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);
uint32 img_ctx_ivd_get_li2_wide_chroma_filter_combine(uint32 context_id, ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);
uint32 img_ctx_get_li2_wide_chroma_filter_combine(uint32 context_id, ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_high(uint32 context_id, const ik_hi_wide_chroma_filter_high_t *p_hi_wide_chroma_filter_high);
uint32 img_ctx_set_hi_wide_chroma_filter_high(uint32 context_id, const ik_hi_wide_chroma_filter_high_t *p_hi_wide_chroma_filter_high);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_high(uint32 context_id, ik_hi_wide_chroma_filter_high_t *p_hi_wide_chroma_filter_high);
uint32 img_ctx_get_hi_wide_chroma_filter_high(uint32 context_id, ik_hi_wide_chroma_filter_high_t *p_hi_wide_chroma_filter_high);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_combine_high(uint32 context_id, const ik_hi_wide_chroma_filter_combine_high_t *p_hi_wide_chroma_filter_combine_high);
uint32 img_ctx_set_hi_wide_chroma_filter_combine_high(uint32 context_id, const ik_hi_wide_chroma_filter_combine_high_t *p_hi_wide_chroma_filter_combine_high);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_combine_high(uint32 context_id, ik_hi_wide_chroma_filter_combine_high_t *p_hi_wide_chroma_filter_combine_high);
uint32 img_ctx_get_hi_wide_chroma_filter_combine_high(uint32 context_id, ik_hi_wide_chroma_filter_combine_high_t *p_hi_wide_chroma_filter_combine_high);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_med(uint32 context_id, const ik_hi_wide_chroma_filter_med_t *p_hi_wide_chroma_filter_med);
uint32 img_ctx_set_hi_wide_chroma_filter_med(uint32 context_id, const ik_hi_wide_chroma_filter_med_t *p_hi_wide_chroma_filter_med);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_med(uint32 context_id, ik_hi_wide_chroma_filter_med_t *p_hi_wide_chroma_filter_med);
uint32 img_ctx_get_hi_wide_chroma_filter_med(uint32 context_id, ik_hi_wide_chroma_filter_med_t *p_hi_wide_chroma_filter_med);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_combine_med(uint32 context_id, const ik_hi_wide_chroma_filter_combine_med_t *p_hi_wide_chroma_filter_combine_med);
uint32 img_ctx_set_hi_wide_chroma_filter_combine_med(uint32 context_id, const ik_hi_wide_chroma_filter_combine_med_t *p_hi_wide_chroma_filter_combine_med);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_combine_med(uint32 context_id, ik_hi_wide_chroma_filter_combine_med_t *p_hi_wide_chroma_filter_combine_med);
uint32 img_ctx_get_hi_wide_chroma_filter_combine_med(uint32 context_id, ik_hi_wide_chroma_filter_combine_med_t *p_hi_wide_chroma_filter_combine_med);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_low(uint32 context_id, const ik_hi_wide_chroma_filter_low_t *p_hi_wide_chroma_filter_low);
uint32 img_ctx_set_hi_wide_chroma_filter_low(uint32 context_id, const ik_hi_wide_chroma_filter_low_t *p_hi_wide_chroma_filter_low);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_low(uint32 context_id, ik_hi_wide_chroma_filter_low_t *p_hi_wide_chroma_filter_low);
uint32 img_ctx_get_hi_wide_chroma_filter_low(uint32 context_id, ik_hi_wide_chroma_filter_low_t *p_hi_wide_chroma_filter_low);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_combine_low(uint32 context_id, const ik_hi_wide_chroma_filter_combine_low_t *p_hi_wide_chroma_filter_combine_low);
uint32 img_ctx_set_hi_wide_chroma_filter_combine_low(uint32 context_id, const ik_hi_wide_chroma_filter_combine_low_t *p_hi_wide_chroma_filter_combine_low);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_combine_low(uint32 context_id, ik_hi_wide_chroma_filter_combine_low_t *p_hi_wide_chroma_filter_combine_low);
uint32 img_ctx_get_hi_wide_chroma_filter_combine_low(uint32 context_id, ik_hi_wide_chroma_filter_combine_low_t *p_hi_wide_chroma_filter_combine_low);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_very_low(uint32 context_id, const ik_hi_wide_chroma_filter_very_low_t *p_hi_wide_chroma_filter_very_low);
uint32 img_ctx_set_hi_wide_chroma_filter_very_low(uint32 context_id, const ik_hi_wide_chroma_filter_very_low_t *p_hi_wide_chroma_filter_very_low);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_very_low(uint32 context_id, ik_hi_wide_chroma_filter_very_low_t *p_hi_wide_chroma_filter_very_low);
uint32 img_ctx_get_hi_wide_chroma_filter_very_low(uint32 context_id, ik_hi_wide_chroma_filter_very_low_t *p_hi_wide_chroma_filter_very_low);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_combine_very_low(uint32 context_id, const ik_hi_wide_chroma_filter_combine_very_low_t *p_hi_wide_chroma_filter_combine_very_low);
uint32 img_ctx_set_hi_wide_chroma_filter_combine_very_low(uint32 context_id, const ik_hi_wide_chroma_filter_combine_very_low_t *p_hi_wide_chroma_filter_combine_very_low);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_combine_very_low(uint32 context_id, ik_hi_wide_chroma_filter_combine_very_low_t *p_hi_wide_chroma_filter_combine_very_low);
uint32 img_ctx_get_hi_wide_chroma_filter_combine_very_low(uint32 context_id, ik_hi_wide_chroma_filter_combine_very_low_t *p_hi_wide_chroma_filter_combine_very_low);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_pre(uint32 context_id, const ik_hi_wide_chroma_filter_pre_t *p_hi_wide_chroma_filter_pre);
uint32 img_ctx_set_hi_wide_chroma_filter_pre(uint32 context_id, const ik_hi_wide_chroma_filter_pre_t *p_hi_wide_chroma_filter_pre);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_pre(uint32 context_id, ik_hi_wide_chroma_filter_pre_t *p_hi_wide_chroma_filter_pre);
uint32 img_ctx_get_hi_wide_chroma_filter_pre(uint32 context_id, ik_hi_wide_chroma_filter_pre_t *p_hi_wide_chroma_filter_pre);

uint32 img_ctx_ivd_set_hi_wide_chroma_filter_pre_combine(uint32 context_id, const ik_hi_wide_chroma_filter_pre_combine_t *p_hi_wide_chroma_filter_pre_combine);
uint32 img_ctx_set_hi_wide_chroma_filter_pre_combine(uint32 context_id, const ik_hi_wide_chroma_filter_pre_combine_t *p_hi_wide_chroma_filter_pre_combine);
uint32 img_ctx_ivd_get_hi_wide_chroma_filter_pre_combine(uint32 context_id, ik_hi_wide_chroma_filter_pre_combine_t *p_hi_wide_chroma_filter_pre_combine);
uint32 img_ctx_get_hi_wide_chroma_filter_pre_combine(uint32 context_id, ik_hi_wide_chroma_filter_pre_combine_t *p_hi_wide_chroma_filter_pre_combine);

//mono
uint32 img_ctx_ivd_set_mono_dynamic_bad_pixel_corr(uint32 context_id, const ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr);
uint32 img_ctx_set_mono_dynamic_bad_pixel_corr(uint32 context_id, const ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr);
uint32 img_ctx_ivd_get_mono_dynamic_bad_pixel_corr(uint32 context_id, ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr);
uint32 img_ctx_get_mono_dynamic_bad_pixel_corr(uint32 context_id, ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr);

uint32 img_ctx_ivd_set_mono_cfa_leakage_filter(uint32 context_id, const ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter);
uint32 img_ctx_set_mono_cfa_leakage_filter(uint32 context_id, const ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter);
uint32 img_ctx_ivd_get_mono_cfa_leakage_filter(uint32 context_id, ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter);
uint32 img_ctx_get_mono_cfa_leakage_filter(uint32 context_id, ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter);

uint32 img_ctx_ivd_set_mono_cfa_noise_filter(uint32 context_id, const ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter);
uint32 img_ctx_set_mono_cfa_noise_filter(uint32 context_id, const ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter);
uint32 img_ctx_ivd_get_mono_cfa_noise_filter(uint32 context_id, ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter);
uint32 img_ctx_get_mono_cfa_noise_filter(uint32 context_id, ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter);

uint32 img_ctx_ivd_set_mono_anti_aliasing(uint32 context_id, const ik_mono_anti_aliasing_t *p_mono_anti_aliasing);
uint32 img_ctx_set_mono_anti_aliasing(uint32 context_id, const ik_mono_anti_aliasing_t *p_mono_anti_aliasing);
uint32 img_ctx_ivd_get_mono_anti_aliasing(uint32 context_id, ik_mono_anti_aliasing_t *p_mono_anti_aliasing);
uint32 img_ctx_get_mono_anti_aliasing(uint32 context_id, ik_mono_anti_aliasing_t *p_mono_anti_aliasing);

uint32 img_ctx_ivd_set_mono_before_ce_wb_gain(uint32 context_id, const ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain);
uint32 img_ctx_set_mono_before_ce_wb_gain(uint32 context_id, const ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain);
uint32 img_ctx_ivd_get_mono_before_ce_wb_gain(uint32 context_id, ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain);
uint32 img_ctx_get_mono_before_ce_wb_gain(uint32 context_id, ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain);

uint32 img_ctx_ivd_set_mono_after_ce_wb_gain(uint32 context_id, const ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain);
uint32 img_ctx_set_mono_after_ce_wb_gain(uint32 context_id, const ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain);
uint32 img_ctx_ivd_get_mono_after_ce_wb_gain(uint32 context_id, ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain);
uint32 img_ctx_get_mono_after_ce_wb_gain(uint32 context_id, ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain);

uint32 img_ctx_ivd_set_mono_color_correction(uint32 context_id, const ik_mono_color_correction_t *p_mono_color_correction);
uint32 img_ctx_set_mono_color_correction(uint32 context_id, const ik_mono_color_correction_t *p_mono_color_correction);
uint32 img_ctx_ivd_get_mono_color_correction(uint32 context_id, ik_mono_color_correction_t *p_mono_color_correction);
uint32 img_ctx_get_mono_color_correction(uint32 context_id, ik_mono_color_correction_t *p_mono_color_correction);

uint32 img_ctx_ivd_set_mono_tone_curve(uint32 context_id, const ik_mono_tone_curve_t *p_mono_tone_curve);
uint32 img_ctx_set_mono_tone_curve(uint32 context_id, const ik_mono_tone_curve_t *p_mono_tone_curve);
uint32 img_ctx_ivd_get_mono_tone_curve(uint32 context_id, ik_mono_tone_curve_t *p_mono_tone_curve);
uint32 img_ctx_get_mono_tone_curve(uint32 context_id, ik_mono_tone_curve_t *p_mono_tone_curve);

uint32 img_ctx_ivd_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix);
uint32 img_ctx_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix);
uint32 img_ctx_ivd_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix);
uint32 img_ctx_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix);

uint32 img_ctx_ivd_set_mono_demosaic(uint32 context_id, const ik_mono_demosaic_t *p_mono_demosaic);
uint32 img_ctx_set_mono_demosaic(uint32 context_id, const ik_mono_demosaic_t *p_mono_demosaic);
uint32 img_ctx_ivd_get_mono_demosaic(uint32 context_id, ik_mono_demosaic_t *p_mono_demosaic);
uint32 img_ctx_get_mono_demosaic(uint32 context_id, ik_mono_demosaic_t *p_mono_demosaic);

uint32 img_ctx_ivd_set_mono_grgb_mismatch(uint32 context_id, const ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch);
uint32 img_ctx_set_mono_grgb_mismatch(uint32 context_id, const ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch);
uint32 img_ctx_ivd_get_mono_grgb_mismatch(uint32 context_id, ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch);
uint32 img_ctx_get_mono_grgb_mismatch(uint32 context_id, ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch);

uint32 img_ctx_ivd_set_mono_fst_luma_process_mode(uint32 context_id, const ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode);
uint32 img_ctx_set_mono_fst_luma_process_mode(uint32 context_id, const ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode);
uint32 img_ctx_ivd_get_mono_fst_luma_process_mode(uint32 context_id, ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode);
uint32 img_ctx_get_mono_fst_luma_process_mode(uint32 context_id, ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode);

uint32 img_ctx_ivd_set_mono_adv_spatial_filter(uint32 context_id, const ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter);
uint32 img_ctx_set_mono_adv_spatial_filter(uint32 context_id, const ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter);
uint32 img_ctx_ivd_get_mono_adv_spatial_filter(uint32 context_id, ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter);
uint32 img_ctx_get_mono_adv_spatial_filter(uint32 context_id, ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter);

uint32 img_ctx_ivd_set_mono_fst_shp_both(uint32 context_id, const ik_mono_fst_shp_both_t *p_mono_fst_shp_both);
uint32 img_ctx_set_mono_fst_shp_both(uint32 context_id, const ik_mono_fst_shp_both_t *p_mono_fst_shp_both);
uint32 img_ctx_ivd_get_mono_fst_shp_both(uint32 context_id, ik_mono_fst_shp_both_t *p_mono_fst_shp_both);
uint32 img_ctx_get_mono_fst_shp_both(uint32 context_id, ik_mono_fst_shp_both_t *p_mono_fst_shp_both);

uint32 img_ctx_ivd_set_mono_fst_shp_noise(uint32 context_id, const ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise);
uint32 img_ctx_set_mono_fst_shp_noise(uint32 context_id, const ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise);
uint32 img_ctx_ivd_get_mono_fst_shp_noise(uint32 context_id, ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise);
uint32 img_ctx_get_mono_fst_shp_noise(uint32 context_id, ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise);

uint32 img_ctx_ivd_set_mono_fst_shp_fir(uint32 context_id, const ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir);
uint32 img_ctx_set_mono_fst_shp_fir(uint32 context_id, const ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir);
uint32 img_ctx_ivd_get_mono_fst_shp_fir(uint32 context_id, ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir);
uint32 img_ctx_get_mono_fst_shp_fir(uint32 context_id, ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir);

uint32 img_ctx_ivd_set_mono_fst_shp_coring(uint32 context_id, const ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring);
uint32 img_ctx_set_mono_fst_shp_coring(uint32 context_id, const ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring);
uint32 img_ctx_ivd_get_mono_fst_shp_coring(uint32 context_id, ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring);
uint32 img_ctx_get_mono_fst_shp_coring(uint32 context_id, ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring);

uint32 img_ctx_ivd_set_mono_fst_shp_coring_idx_scale(uint32 context_id, const ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale);
uint32 img_ctx_set_mono_fst_shp_coring_idx_scale(uint32 context_id, const ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale);
uint32 img_ctx_ivd_get_mono_fst_shp_coring_idx_scale(uint32 context_id, ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale);
uint32 img_ctx_get_mono_fst_shp_coring_idx_scale(uint32 context_id, ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale);

uint32 img_ctx_ivd_set_mono_fst_shp_min_coring_rslt(uint32 context_id, const ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt);
uint32 img_ctx_set_mono_fst_shp_min_coring_rslt(uint32 context_id, const ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt);
uint32 img_ctx_ivd_get_mono_fst_shp_min_coring_rslt(uint32 context_id, ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt);
uint32 img_ctx_get_mono_fst_shp_min_coring_rslt(uint32 context_id, ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt);

uint32 img_ctx_ivd_set_mono_fst_shp_max_coring_rslt(uint32 context_id, const ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt);
uint32 img_ctx_set_mono_fst_shp_max_coring_rslt(uint32 context_id, const ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt);
uint32 img_ctx_ivd_get_mono_fst_shp_max_coring_rslt(uint32 context_id, ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt);
uint32 img_ctx_get_mono_fst_shp_max_coring_rslt(uint32 context_id, ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt);

uint32 img_ctx_ivd_set_mono_fst_shp_scale_coring(uint32 context_id, const ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring);
uint32 img_ctx_set_mono_fst_shp_scale_coring(uint32 context_id, const ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring);
uint32 img_ctx_ivd_get_mono_fst_shp_scale_coring(uint32 context_id, ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring);
uint32 img_ctx_get_mono_fst_shp_scale_coring(uint32 context_id, ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring);

uint32 img_ctx_ivd_set_mono_exp0_fe_wb_gain(uint32 context_id, const ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain);
uint32 img_ctx_set_mono_exp0_fe_wb_gain(uint32 context_id, const ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain);
uint32 img_ctx_ivd_get_mono_exp0_fe_wb_gain(uint32 context_id, ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain);
uint32 img_ctx_get_mono_exp0_fe_wb_gain(uint32 context_id, ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain);

uint32 img_ctx_ivd_set_mono_exp0_fe_static_blc_level(uint32 context_id, const ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level);
uint32 img_ctx_set_mono_exp0_fe_static_blc_level(uint32 context_id, const ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level);
uint32 img_ctx_ivd_get_mono_exp0_fe_static_blc_level(uint32 context_id, ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level);
uint32 img_ctx_get_mono_exp0_fe_static_blc_level(uint32 context_id, ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level);

uint32 img_ctx_ivd_set_mono_ce(uint32 context_id, const ik_mono_ce_t *p_mono_ce);
uint32 img_ctx_set_mono_ce(uint32 context_id, const ik_mono_ce_t *p_mono_ce);
uint32 img_ctx_ivd_get_mono_ce(uint32 context_id, ik_mono_ce_t *p_mono_ce);
uint32 img_ctx_get_mono_ce(uint32 context_id, ik_mono_ce_t *p_mono_ce);

uint32 img_ctx_ivd_set_mono_ce_input_table(uint32 context_id, const ik_mono_ce_input_table_t *p_mono_ce_input_table);
uint32 img_ctx_set_mono_ce_input_table(uint32 context_id, const ik_mono_ce_input_table_t *p_mono_ce_input_table);
uint32 img_ctx_ivd_get_mono_ce_input_table(uint32 context_id, ik_mono_ce_input_table_t *p_mono_ce_input_table);
uint32 img_ctx_get_mono_ce_input_table(uint32 context_id, ik_mono_ce_input_table_t *p_mono_ce_input_table);

uint32 img_ctx_ivd_set_mono_ce_output_table(uint32 context_id, const ik_mono_ce_output_table_t *p_mono_ce_output_table);
uint32 img_ctx_set_mono_ce_output_table(uint32 context_id, const ik_mono_ce_output_table_t *p_mono_ce_output_table);
uint32 img_ctx_ivd_get_mono_ce_output_table(uint32 context_id, ik_mono_ce_output_table_t *p_mono_ce_output_table);
uint32 img_ctx_get_mono_ce_output_table(uint32 context_id, ik_mono_ce_output_table_t *p_mono_ce_output_table);

uint32 img_ctx_ivd_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion);
uint32 img_ctx_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion);
uint32 img_ctx_ivd_get_fusion(uint32 context_id, ik_fusion_t *p_fusion);
uint32 img_ctx_get_fusion(uint32 context_id, ik_fusion_t *p_fusion);

#endif
