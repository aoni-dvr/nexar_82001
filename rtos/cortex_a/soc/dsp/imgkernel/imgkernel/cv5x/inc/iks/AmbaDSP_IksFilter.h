#ifndef IKS_FILTERS_IF_H
#define IKS_FILTERS_IF_H

#include "ik_data_type.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgArchSpec.h"


typedef struct {
    uint32 update_freq;
} iks_safety_info_t;

typedef struct {
    uint32 normalized_r;
    uint32 normalized_b;
} iks_normalized_gain_t;

uint32 iks_set_safety_info(uint32 context_id, const iks_safety_info_t *p_safety_info);
uint32 iks_get_safety_info(uint32 context_id, iks_safety_info_t *p_safety_info);

uint32 iks_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
uint32 iks_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info);

uint32 iks_set_yuv_mode(uint32 context_id, const uint32 yuv_mode);
uint32 iks_get_yuv_mode(uint32 context_id, uint32 *p_yuv_mode);

uint32 iks_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
uint32 iks_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);

uint32 iks_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
uint32 iks_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);

uint32 iks_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 iks_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);

uint32 iks_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 iks_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);

uint32 iks_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
uint32 iks_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);

uint32 iks_set_dynamic_bad_pixel_corr(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 iks_get_dynamic_bad_pixel_corr(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);

uint32 iks_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 iks_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);

uint32 iks_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic);
uint32 iks_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic);

uint32 iks_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y);
uint32 iks_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y);

uint32 iks_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce);
uint32 iks_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce);

uint32 iks_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction);
uint32 iks_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction);

uint32 iks_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
uint32 iks_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve);

uint32 iks_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 iks_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);

uint32 iks_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
uint32 iks_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);

uint32 iks_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
uint32 iks_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale);

uint32 iks_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
uint32 iks_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);

uint32 iks_set_wide_chroma_filter(uint32 context_id, const ik_wide_chroma_filter_t *p_chroma_filter);
uint32 iks_get_wide_chroma_filter(uint32 context_id, ik_wide_chroma_filter_t *p_chroma_filter);

uint32 iks_set_wide_chroma_filter_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 iks_get_wide_chroma_filter_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);

uint32 iks_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
uint32 iks_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter);

uint32 iks_set_fst_luma_process_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 iks_get_fst_luma_process_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);

uint32 iks_set_adv_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 iks_get_adv_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);

uint32 iks_set_fst_shp_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 iks_get_fst_shp_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);

uint32 iks_set_fst_shp_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 iks_get_fst_shp_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);

uint32 iks_set_fst_shp_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 iks_get_fst_shp_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);

uint32 iks_set_fst_shp_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 iks_get_fst_shp_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);

uint32 iks_set_fst_shp_coring_idx_scale(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
uint32 iks_get_fst_shp_coring_idx_scale(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);

uint32 iks_set_fst_shp_min_coring_rslt(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
uint32 iks_get_fst_shp_min_coring_rslt(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);

uint32 iks_set_fst_shp_max_coring_rslt(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
uint32 iks_get_fst_shp_max_coring_rslt(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);

uint32 iks_set_fst_shp_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
uint32 iks_get_fst_shp_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);

uint32 iks_set_fnl_shp_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
uint32 iks_get_fnl_shp_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both);

uint32 iks_set_fnl_shp_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
uint32 iks_get_fnl_shp_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir);

uint32 iks_set_fnl_shp_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
uint32 iks_get_fnl_shp_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise);

uint32 iks_set_fnl_shp_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
uint32 iks_get_fnl_shp_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring);

uint32 iks_set_fnl_shp_coring_idx_scale(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
uint32 iks_get_fnl_shp_coring_idx_scale(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);

uint32 iks_set_fnl_shp_min_coring_rslt(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);
uint32 iks_get_fnl_shp_min_coring_rslt(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);

uint32 iks_set_fnl_shp_max_coring_rslt(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);
uint32 iks_get_fnl_shp_max_coring_rslt(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);

uint32 iks_set_fnl_shp_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring);
uint32 iks_get_fnl_shp_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring);

uint32 iks_set_fnl_shp_three_d_table(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
uint32 iks_get_fnl_shp_three_d_table(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);

uint32 iks_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
uint32 iks_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf);

uint32 iks_set_video_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
uint32 iks_get_video_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta);

uint32 iks_set_video_mctf_and_fnl_shp(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
uint32 iks_get_video_mctf_and_fnl_shp(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen);

uint32 iks_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
uint32 iks_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);

uint32 iks_set_dzoom_info(uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
uint32 iks_get_dzoom_info(uint32 context_id, ik_dzoom_info_t *p_dzoom_info);

uint32 iks_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info);
uint32 iks_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info);

uint32 iks_set_dummy_margin_range(uint32 context_id, const ik_dummy_margin_range_t *p_dmy_margin_range);
uint32 iks_get_dummy_margin_range(uint32 context_id, ik_dummy_margin_range_t *p_dmy_margin_range);

uint32 iks_set_vin_active_win(uint32 context_id, const ik_vin_active_window_t *p_vin_active_win);
uint32 iks_get_vin_active_win(uint32 context_id, ik_vin_active_window_t *p_vin_active_win);

uint32 iks_set_resampler_strength(uint32 context_id, const ik_resampler_strength_t *p_resampler_strength);
uint32 iks_get_resampler_strength(uint32 context_id, ik_resampler_strength_t *p_resampler_strength);


/* Calibration filters */
uint32 iks_set_warp_enb(uint32 context_id, const uint32 enb);
uint32 iks_get_warp_enb(uint32 context_id, uint32 *p_enb);

uint32 iks_set_warp_info(uint32 context_id, const ik_warp_info_t *p_calib_warp_info);
uint32 iks_get_warp_info(uint32 context_id, ik_warp_info_t *p_calib_warp_info);

uint32 iks_set_cawarp_enb(uint32 context_id, const uint32 enb);
uint32 iks_get_cawarp_enb(uint32 context_id, uint32 *p_enb);

uint32 iks_set_cawarp_info(uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info);
uint32 iks_get_cawarp_info(uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info);

uint32 iks_set_static_bad_pxl_corr_enb(uint32 context_id, const uint32 enb);
uint32 iks_get_static_bad_pxl_corr_enb(uint32 context_id, uint32 *p_enb);

uint32 iks_set_static_bad_pxl_corr(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 iks_get_static_bad_pxl_corr(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);

uint32 iks_set_vignette_enb(uint32 context_id, const uint32 enb);
uint32 iks_get_vignette_enb(uint32 context_id, uint32 *p_enb);

uint32 iks_set_vignette(uint32 context_id, const ik_vignette_t *p_vignette);
uint32 iks_get_vignette(uint32 context_id, ik_vignette_t *p_vignette);

uint32 iks_set_static_bad_pxl_corr_itnl(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
uint32 iks_get_static_bad_pxl_corr_itnl(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);

uint32 iks_set_cawarp_internal(uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal);
uint32 iks_get_cawarp_internal(uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal);

uint32 iks_set_warp_internal(uint32 context_id, const ik_warp_internal_info_t *p_warp_dzoom_internal);
uint32 iks_get_warp_internal(uint32 context_id, ik_warp_internal_info_t *p_warp_dzoom_internal);

uint32 iks_set_flip_mode(uint32 context_id, const uint32 mode);
uint32 iks_get_flip_mode(uint32 context_id, uint32 *p_mode);

uint32 iks_set_warp_buffer_info(uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info);
uint32 iks_get_warp_buffer_info(uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info);

uint32 iks_set_stitching_info(uint32 context_id, const ik_stitch_info_t *p_stitch_info);
uint32 iks_get_stitching_info(uint32 context_id, ik_stitch_info_t *p_stitch_info);

/* HDR filters */
uint32 iks_set_exp0_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 iks_get_exp0_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);

uint32 iks_set_exp1_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc);
uint32 iks_get_exp1_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc);

uint32 iks_set_exp2_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc);
uint32 iks_get_exp2_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc);

uint32 iks_set_exp0_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
uint32 iks_get_exp0_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);

uint32 iks_set_exp1_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
uint32 iks_get_exp1_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);

uint32 iks_set_exp2_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
uint32 iks_get_exp2_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);

uint32 iks_set_ce(uint32 context_id, const ik_ce_t *p_ce);
uint32 iks_get_ce(uint32 context_id, ik_ce_t *p_ce);

uint32 iks_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
uint32 iks_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);

uint32 iks_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
uint32 iks_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);

uint32 iks_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);
uint32 iks_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend);

uint32 iks_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info);
uint32 iks_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info);

uint32 iks_set_aaa_pg_af_stat_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);
uint32 iks_get_aaa_pg_af_stat_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info);

uint32 iks_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info);
uint32 iks_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info);

uint32 iks_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
uint32 iks_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);

uint32 iks_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 iks_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

uint32 iks_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 iks_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

uint32 iks_set_frontend_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc);
uint32 iks_get_frontend_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc);

uint32 iks_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_hdr_raw_info);
uint32 iks_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_hdr_raw_info);

uint32 iks_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable);
uint32 iks_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *enable);

#endif
