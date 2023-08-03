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

#ifndef CONTEXT_DEBUG_UTILITY_H
#define CONTEXT_DEBUG_UTILITY_H

#include "ik_data_type.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgAdvancedFilter.h"
#include "AmbaDSP_ImgHisoFilter.h"


typedef struct {
    uint32 (*vin_sensor_info)(const ik_vin_sensor_info_t *p_vin_sensor);
    uint32 (*fe_tone_curve)(const ik_frontend_tone_curve_t *p_tone_curve);
    uint32 (*fe_blc)(const ik_static_blc_level_t *p_blc, uint32 exp_idx);
    uint32 (*fe_wb)(const ik_frontend_wb_gain_t *p_fe_wb, uint32 exp_idx);
    uint32 (*fe_dgain_sat)(const ik_frontend_dgain_saturation_level_t *p_fe_wb, uint32 exp_idx);
    uint32 (*hdr_blend)(const ik_hdr_blend_t *p_hdr_blend);
    uint32 (*vignette_info)(const ik_vignette_t *p_vig);
    uint32 (*dynamic_bad_pxl_cor)(const ik_dynamic_bad_pixel_correction_t *p_dbc);
    uint32 (*static_bpc)(const ik_static_bad_pxl_cor_t *p_static_bpc);
    uint32 (*static_bpc_internal)(const ik_static_bad_pixel_correction_internal_t *static_bpc);
    uint32 (*cfa_leakage)(const ik_cfa_leakage_filter_t *p_cfa_leakage);
    uint32 (*anti_aliasing)(const ik_anti_aliasing_t *p_anti_aliasing);
    uint32 (*grgb_mismatch)(const ik_grgb_mismatch_t *p_grgb_mismatch);
    uint32 (*before_ce_wb)(const ik_wb_gain_t *p_wb_gain);
    uint32 (*cawarp)(const ik_cawarp_info_t *p_cawarp);
    uint32 (*cawarp_internal)(const ik_cawarp_internal_info_t *cawarp_internal);
    uint32 (*ce)(const ik_ce_t *p_ce);
    uint32 (*ce_input_tbl)(const ik_ce_input_table_t *p_ce_input_tbl);
    uint32 (*ce_output_tbl)(const ik_ce_output_table_t *p_ce_output_tbl);
    uint32 (*after_ce_wb)(const ik_wb_gain_t *p_wb_gain);
    uint32 (*stored_ir)(const ik_stored_ir_t *p_stored_ir);
    uint32 (*cfa_noise_filter)(const ik_cfa_noise_filter_t *p_cfa_noise_filter);
    uint32 (*demosaic)(const ik_demosaic_t *p_demosaic);
    uint32 (*ext_raw_out_mode)(const uint32 ext_raw_out_mode);
    uint32 (*rgb_to_12y)(const ik_rgb_to_12y_t *p_rgb_to_12y);
    uint32 (*luma_noise_reduce)(const ik_luma_noise_reduction_t *p_luma_noise_reduce);
    uint32 (*rgb_ir)(const ik_rgb_ir_t *p_rgb_ir);
    uint32 (*color_correction)(const ik_color_correction_t *p_cc_3d);
    uint32 (*pre_cc_gain)(const ik_pre_cc_gain_t *pre_cc_gain);
    uint32 (*tone_curve)(const ik_tone_curve_t *p_tone_curve);
    uint32 (*rgb_to_yuv_matrix)(const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
    uint32 (*chroma_sacle)(const ik_chroma_scale_t *p_chroma_scale);
    uint32 (*chroma_filter)(const ik_chroma_filter_t *p_chroma_filter);
    uint32 (*chroma_median_filter)(const ik_chroma_median_filter_t *p_chroma_median_filter);
    uint32 (*first_luma_proc_mode)(const ik_first_luma_process_mode_t *p_first_luma_proc_mode);
    uint32 (*adv_spat_fltr)(const ik_adv_spatial_filter_t *p_adv_spat_fltr);
    uint32 (*first_sharpen_both)(const ik_first_sharpen_both_t *p_first_sharpen_both);
    uint32 (*first_sharpen_fir)(const ik_first_sharpen_fir_t *p_first_sharpen_fir);
    uint32 (*first_sharpen_noise)(const ik_first_sharpen_noise_t *p_first_sharpen_noise);
    uint32 (*first_sharpen_coring)(const ik_first_sharpen_coring_t *p_first_sharpen_coring);
    uint32 (*fstshpns_cor_idx_scl)(const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
    uint32 (*fstshpns_min_cor_rst)(const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
    uint32 (*fstshpns_max_cor_rst)(const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
    uint32 (*fstshpns_scl_cor)(const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
    uint32 (*warp_enable)(const uint32 enable);
    uint32 (*warp_info)(const ik_warp_info_t *p_warp_info);
    uint32 (*warp_info_2nd)(const ik_warp_info_t *p_warp_info);
    uint32 (*warp_info_internal)(const ik_warp_internal_info_t *warp_internal);
    uint32 (*warp_info_2nd_internal)(const ik_warp_2nd_internal_info_t *warp_2nd_internal);
    uint32 (*warp_bind_info)(const ik_bind_info_t *warp_bind_info);
    uint32 (*video_mctf)(const ik_video_mctf_t *p_video_mctf);
    uint32 (*video_mctf_internal)(const ik_mctf_internal_t *video_mctf_internal);
    uint32 (*video_mctf_ta)(const ik_video_mctf_ta_t *p_video_mctf_ta);
    uint32 (*video_mctf_and_fnlshp)(const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
    uint32 (*final_sharpen_both)(const ik_final_sharpen_both_t *p_final_sharpen_both);
    uint32 (*final_sharpen_fir)(const ik_final_sharpen_fir_t *p_final_sharpen_fir);
    uint32 (*final_sharpen_noise)(const ik_final_sharpen_noise_t *p_final_sharpen_noise);
    uint32 (*final_sharpen_coring)(const ik_final_sharpen_coring_t *p_final_sharpen_coring);
    uint32 (*fnlshpns_cor_idx_scl)(const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
    uint32 (*fnlshpns_min_cor_rst)(const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);
    uint32 (*fnlshpns_max_cor_rst)(const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);
    uint32 (*fnlshpns_scl_cor)(const ik_final_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
    uint32 (*final_sharpen_both_three_d_table)(const ik_final_sharpen_both_three_d_table_t *p_fstshpns_both_tdt);
    uint32 (*dzoom_info)(const ik_dzoom_info_t *p_dzoom_info);
    uint32 (*hdr_shift)(const ik_hdr_eis_info_t *hdr_sft);
    uint32 (*window_size_info)(const ik_window_size_info_t *p_window_size_info, uint32 ability);
    uint32 (*cfa_window_size_info)(const ik_cfa_window_size_info_t *p_cfa_window_size_info);
    uint32 (*vin_active_win)(const ik_vin_active_window_t *p_window_geo, uint32 ability);
    uint32 (*dummy_margin_range)(const ik_dummy_margin_range_t *p_dummy_range, uint32 stitch_enb);
    uint32 (*resamp_strength)(const ik_resampler_strength_t *p_window_geo);
    uint32 (*motion_detect)(const ik_motion_detect_t *p_md);
    uint32 (*motion_detect_pos_dep)(const ik_pos_dep33_t *p_md);
    uint32 (*motion_detect_and_mctf)(const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
    uint32 (*histogram_info)(const ik_histogram_info_t *p_hist_info);
    uint32 (*flip_mode)(uint32 mode);
    uint32 (*aaa_stat)(const ik_aaa_stat_info_t *p_stat_info);
    uint32 (*aaa_pg_af_stat)(const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
    uint32 (*af_stat_ex)(const ik_af_stat_ex_info_t *p_af_stat_ex_info);
    uint32 (*pg_af_stat_ex)(const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
    uint32 (*wide_chroma_filter)(const ik_wide_chroma_filter_t *p_wide_chroma_filter);
    uint32 (*wide_chroma_filter_combine)(const ik_wide_chroma_filter_combine_t *p_wide_chroma_filter_combine);
    uint32 (*warp_buffer_info)(const ik_warp_buffer_info_t *p_warp_buf_info);
    uint32 (*overlap_x)(const ik_overlap_x_t *p_overlap_x);
    uint32 (*stitching_info)(const ik_stitch_info_t *p_stitch_info);
    uint32 (*burst_tile)(const ik_burst_tile_t *p_burst_tile);

    // hiso
    uint32 (*hi_anti_aliasing)(const ik_hi_anti_aliasing_t *p_hi_anti_aliasing);
    uint32 (*hi_cfa_leakage_filter)(const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);
    uint32 (*hi_dynamic_bpc)(const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);
    uint32 (*hi_grgb_mismatch)(const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);
    uint32 (*hi_chroma_median_filter)(const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);
    uint32 (*hi_cfa_noise_filter)(const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);
    uint32 (*hi_demosaic)(const ik_hi_demosaic_t *p_hi_demosaic);
    //li2
    uint32 (*li2_anti_aliasing)(const ik_li2_anti_aliasing_t *p_li2_anti_aliasing);
    uint32 (*li2_cfa_leakage_filter)(const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);
    uint32 (*li2_dynamic_bpc)(const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);
    uint32 (*li2_grgb_mismatch)(const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);
    uint32 (*li2_cfa_noise_filter)(const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);
    uint32 (*li2_demosaic)(const ik_li2_demosaic_t *p_li2_demosaic);
    //ASF
    uint32 (*hi_asf)(const ik_hi_asf_t *p_hi_asf);
    uint32 (*li2_asf)(const ik_li2_asf_t *p_li2_asf);
    uint32 (*hi_low_asf)(const ik_hi_low_asf_t *p_hi_low_asf);
    uint32 (*hi_med1_asf)(const ik_hi_med1_asf_t *p_hi_med1_asf);
    uint32 (*hi_med2_asf)(const ik_hi_med2_asf_t *p_hi_med2_asf);
    uint32 (*hi_high_asf)(const ik_hi_high_asf_t *p_hi_high_asf);
    uint32 (*hi_high2_asf)(const ik_hi_high2_asf_t *p_hi_high2_asf);
    uint32 (*chroma_asf)(const ik_chroma_asf_t *p_chroma_asf);
    uint32 (*hi_chroma_asf)(const ik_hi_chroma_asf_t *p_hi_chroma_asf);
    uint32 (*hi_low_chroma_asf)(const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);
    //SHP
    uint32 (*hi_high_shpns_both)(const ik_hi_high_shpns_both_t *p_hi_high_shpns_both);
    uint32 (*hi_high_shpns_noise)(const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);
    uint32 (*hi_high_shpns_coring)(const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);
    uint32 (*hi_high_shpns_fir)(const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);
    uint32 (*hi_high_shpns_cor_idx_scl)(const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);
    uint32 (*hi_high_shpns_min_cor_rst)(const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);
    uint32 (*hi_high_shpns_max_cor_rst)(const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);
    uint32 (*hi_high_shpns_scl_cor)(const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);
    //Med
    uint32 (*hi_med_shpns_both)(const ik_hi_med_shpns_both_t *p_hi_med_shpns_both);
    uint32 (*hi_med_shpns_noise)(const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);
    uint32 (*hi_med_shpns_coring)(const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);
    uint32 (*hi_med_shpns_fir)(const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);
    uint32 (*hi_med_shpns_cor_idx_scl)(const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);
    uint32 (*hi_med_shpns_min_cor_rst)(const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);
    uint32 (*hi_med_shpns_max_cor_rst)(const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);
    uint32 (*hi_med_shpns_scl_cor)(const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);
    //li2_shp
    uint32 (*li2_shpns_both)(const ik_li2_shpns_both_t *p_li2_shpns_both);
    uint32 (*li2_shpns_noise)(const ik_li2_shpns_noise_t *p_li2_shpns_noise);
    uint32 (*li2_shpns_coring)(const ik_li2_shpns_coring_t *p_li2_shpns_coring);
    uint32 (*li2_shpns_fir)(const ik_li2_shpns_fir_t *p_li2_shpns_fir);
    uint32 (*li2_shpns_cor_idx_scl)(const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);
    uint32 (*li2_shpns_min_cor_rst)(const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);
    uint32 (*li2_shpns_max_cor_rst)(const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);
    uint32 (*li2_shpns_scl_cor)(const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);
    //hili
    uint32 (*hili_shpns_both)(const ik_hili_shpns_both_t *p_hili_shpns_both);
    uint32 (*hili_shpns_noise)(const ik_hili_shpns_noise_t *p_hili_shpns_noise);
    uint32 (*hili_shpns_coring)(const ik_hili_shpns_coring_t *p_hili_shpns_coring);
    uint32 (*hili_shpns_fir)(const ik_hili_shpns_fir_t *p_hili_shpns_fir);
    uint32 (*hili_shpns_cor_idx_scl)(const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);
    uint32 (*hili_shpns_min_cor_rst)(const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);
    uint32 (*hili_shpns_max_cor_rst)(const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);
    uint32 (*hili_shpns_scl_cor)(const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);
    //chroma_filter
    uint32 (*hi_chroma_filter_high)(const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);
    uint32 (*hi_chroma_filter_pre)(const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);
    uint32 (*hi_chroma_filter_med)(const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);
    uint32 (*hi_chroma_filter_low)(const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);
    uint32 (*hi_chroma_filter_very_low)(const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);
    //combine
    uint32 (*hi_luma_combine)(const ik_hi_luma_combine_t *p_hi_luma_combine);
    uint32 (*hi_low_asf_combine)(const ik_hi_low_asf_combine_t *p_hi_low_asf_combine);
    uint32 (*hi_chroma_fltr_med_com)(const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);
    uint32 (*hi_chroma_fltr_low_com)(const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);
    uint32 (*hi_chroma_fltr_very_low_com)(const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);
    uint32 (*hili_combine)(const ik_hili_combine_t *p_hili_combine);
    uint32 (*hi_mid_high_freq_recover)(const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);
    uint32 (*hi_luma_blend)(const ik_hi_luma_blend_t *p_hi_luma_blend);
    uint32 (*hi_nonsmooth_detect)(const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);
    uint32 (*hi_select)(const ik_hi_select_t *p_hi_select);
    uint32 (*hi_wide_chroma_filter)(const ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);
    uint32 (*hi_wide_chroma_filter_combine)(const ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);
    uint32 (*li2_wide_chroma_filter)(const ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);
    uint32 (*li2_wide_chroma_filter_combine)(const ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);

    //mono
    uint32 (*mono_dynamic_bad_pixel_corr)(const ik_mono_dynamic_bad_pixel_corr_t *p_mono_dynamic_bad_pixel_corr);
    uint32 (*mono_cfa_leakage_filter)(const ik_mono_cfa_leakage_filter_t *p_mono_cfa_leakage_filter);
    uint32 (*mono_cfa_noise_filter)(const ik_mono_cfa_noise_filter_t *p_mono_cfa_noise_filter);
    uint32 (*mono_anti_aliasing)(const ik_mono_anti_aliasing_t *p_mono_anti_aliasing);
    uint32 (*mono_before_ce_wb_gain)(const ik_mono_before_ce_wb_gain_t *p_mono_before_ce_wb_gain);
    uint32 (*mono_after_ce_wb_gain)(const ik_mono_after_ce_wb_gain_t *p_mono_after_ce_wb_gain);
    uint32 (*mono_color_correction)(const ik_mono_color_correction_t *p_mono_color_correction);
    uint32 (*mono_tone_curve)(const ik_mono_tone_curve_t *p_mono_tone_curve);
    uint32 (*mono_rgb_to_yuv_matrix)(const ik_mono_rgb_to_yuv_matrix_t *p_mono_rgb_to_yuv_matrix);
    uint32 (*mono_demosaic)(const ik_mono_demosaic_t *p_mono_demosaic);
    uint32 (*mono_grgb_mismatch)(const ik_mono_grgb_mismatch_t *p_mono_grgb_mismatch);
    uint32 (*mono_fst_luma_process_mode)(const ik_mono_fst_luma_process_mode_t *p_mono_fst_luma_process_mode);
    uint32 (*mono_adv_spatial_filter)(const ik_mono_adv_spatial_filter_t *p_mono_adv_spatial_filter);
    uint32 (*mono_fst_shp_both)(const ik_mono_fst_shp_both_t *p_mono_fst_shp_both);
    uint32 (*mono_fst_shp_noise)(const ik_mono_fst_shp_noise_t *p_mono_fst_shp_noise);
    uint32 (*mono_fst_shp_fir)(const ik_mono_fst_shp_fir_t *p_mono_fst_shp_fir);
    uint32 (*mono_fst_shp_coring)(const ik_mono_fst_shp_coring_t *p_mono_fst_shp_coring);
    uint32 (*mono_fst_shp_coring_idx_scale)(const ik_mono_fst_shp_coring_idx_scale_t *p_mono_fst_shp_coring_idx_scale);
    uint32 (*mono_fst_shp_min_coring_rslt)(const ik_mono_fst_shp_min_coring_rslt_t *p_mono_fst_shp_min_coring_rslt);
    uint32 (*mono_fst_shp_max_coring_rslt)(const ik_mono_fst_shp_max_coring_rslt_t *p_mono_fst_shp_max_coring_rslt);
    uint32 (*mono_fst_shp_scale_coring)(const ik_mono_fst_shp_scale_coring_t *p_mono_fst_shp_scale_coring);
    uint32 (*mono_exp0_fe_wb_gain)(const ik_mono_exp0_fe_wb_gain_t *p_mono_exp0_fe_wb_gain);
    uint32 (*mono_exp0_fe_static_blc_level)(const ik_mono_exp0_fe_static_blc_level_t *p_mono_exp0_fe_static_blc_level);
    uint32 (*mono_ce)(const ik_mono_ce_t *p_mono_ce);
    uint32 (*mono_ce_input_table)(const ik_mono_ce_input_table_t *p_mono_ce_input_table);
    uint32 (*mono_ce_output_table)(const ik_mono_ce_output_table_t *p_mono_ce_output_table);
    uint32 (*fusion)(const ik_fusion_t *p_fusion);
} ik_debug_check_func_t;

void ik_ctx_hook_debug_check(void);
const ik_debug_check_func_t* ik_ctx_get_debug_check_func(void);

#endif
