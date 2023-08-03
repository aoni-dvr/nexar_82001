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

#ifndef CONTEXT_ENTITY_H
#define CONTEXT_ENTITY_H

#include "AmbaDSP_ImgIkcSpec.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgControlState.h" //For iso_config_table_index_t, hdr_ce_config_table_index_t
#include "AmbaDSP_ImgHisoFilter.h"

#define CTX_PAD_ALIGN_SIZE (32UL)

#define CONFIG_TAG_UNKNOWN (0xFFFFFFFFU)

#define WARP_VIDEO_REMAP_MAXHORGRID  256UL
#define WARP_VIDEO_REMAP_MAXVERGRID  192UL
#define CAWARP_VIDEO_REMAP_MAXHORGRID 64UL
#define CAWARP_VIDEO_REMAP_MAXVERGRID 96UL
#define CALIB_VIGNETTE_DOWN_SMP_TABLE_NUM 4UL
#define CALIB_VIGNETTE_BUF_SIZE 1089UL
#define DSP_IMG_CORE_VIGNETTE_GAIN_MAP_SIZE   6370UL//65x49xsizeof(u16)

typedef struct {
    ikc_ability_t ability;
    uint32 config_number;
} amba_ik_attribute_t;

//#########################################################//need be moved

typedef struct  {
    int16   def_black1[3];
    int16   def_black2[3];
    int16   def_black3[3];
    int16   pre_status;
} amba_ik_deferred_amount_t;

typedef struct {
    uint32 prev_frame_brightness;
} amba_ik_video_mctf_working_buf_t;

typedef struct {
    ik_ce_radius_range_t   ce_radius_range;
    uint8 Factor[3];
} amba_ik_ce_working_buf_t;

typedef struct {
    uint8 set_interpolate;
    int16 grad_add_ir;
    int16 grad_add_red_on_blue;
    uint8 grad_mul_ir;
    uint8 grad_mul_red_on_blue;
    uint8 grad_shift_ir;
    uint8 grad_shift_red_on_blue;
} amba_ik_rgb_ir_internal_t;

typedef struct  {
    uint64 sec2_hscale_phase_inc;
    uint64 sec2_vscale_phase_inc;
    uint64 sec3_vscale_init_phase;
    uint64 sec3_vscale_phase_inc;
    ik_window_dimension_t   sec2_output;
} amba_ik_warp_phase_t;

typedef struct  {
    uint32                  result_cnt; // Will ++ when any of param is changed.
    ik_win_coordintates_t   act_win_crop;
    ik_window_geomery_t     dmy_win_geo;
    ik_window_dimension_t   cfa_win_dim;
    amba_ik_warp_phase_t    warp_phase;
} amba_ik_calc_win_result_t;

typedef struct {

    amba_ik_deferred_amount_t deferred_amount;
    ik_wb_gain_t normalized_wb_gain;
//    amba_ik_video_mctf_working_buf_t working_buf_video_mctf;
//    amba_ik_ce_working_buf_t          working_buf_ce;
    amba_ik_rgb_ir_internal_t rgb_ir_internal;
    amba_ik_calc_win_result_t      result_win;
    int32  warp_internal_mode_flag;
    int32  cawarp_internal_mode_flag;
    int32  dzoom_internal_mode_flag;

    ik_grid_point_t     remap_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t     remap_search_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t     ca_red_remap_buffer[CAWARP_VIDEO_REMAP_MAXHORGRID * CAWARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t     ca_blue_remap_buffer[CAWARP_VIDEO_REMAP_MAXHORGRID * CAWARP_VIDEO_REMAP_MAXVERGRID];
    int32               remap_row[WARP_VIDEO_REMAP_MAXVERGRID];
    int32               ca_remap_row[CAWARP_VIDEO_REMAP_MAXVERGRID];
    uint32              remap_motion_scan_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    ik_grid_point_t     warp_add_buffer[WARP_VIDEO_REMAP_MAXHORGRID * WARP_VIDEO_REMAP_MAXVERGRID];
    uint8 last_aaa_statistics_setup[76];
    uint8 last_aaa_statistics_setup1_af_ex[44];
    uint8 last_aaa_statistics_setup2_af_ex[44];
    uint8 last_aaa_statistics_setup1_pg_af_ex[44];
    uint8 last_aaa_statistics_setup2_pg_af_ex[44];
    uint8 last_aaa_histogram[24];
    uint8 last_aaa_histogram_pg[24];
    uint8 last_aaa_pseudo_y[48];
    int32 first_compression_offset;
#if PRE_NN_PROCESS
    int32 pre_nn_step2_delta_t;
    int32 pre_nn_step2_t0_offset;
    double64 pre_nn_step2_action_add[2][4];
    ik_wb_gain_t pre_nn_step2_before_ce_gain;
    ik_frontend_wb_gain_t pre_nn_step2_wb;
    int32 pre_nn_b0;
    int32 pre_nn_b1;
#endif
} img_ctx_internal_buffer_t;


//#########################################################


typedef struct {
    //sensor information
    uint32 (*set_vin_sensor_info)(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
    uint32 (*get_vin_sensor_info)(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info);

    //3a statistic
    uint32 (*set_aaa_stat_info)(uint32 context_id, const ik_aaa_stat_info_t *p_aaa_stat_info);
    uint32 (*get_aaa_stat_info)(uint32 context_id, ik_aaa_stat_info_t *p_aaa_stat_info);

    uint32 (*set_af_stat_ex_info)(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info);
    uint32 (*get_af_stat_ex_info)(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info);

    uint32 (*set_pg_af_stat_ex_info)(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
    uint32 (*get_pg_af_stat_ex_info)(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);

    uint32 (*set_aaa_stat_float_tile_info)(uint32 context_id, const ik_aaa_float_info_t *p_aaa_stat_float_info);
    uint32 (*get_aaa_stat_float_tile_info)(uint32 context_id, ik_aaa_float_info_t *p_aaa_stat_float_info);

    //calibration
    uint32 (*set_window_size_info)(uint32 context_id, const ik_window_size_info_t *p_window_size_info);
    uint32 (*get_window_size_info)(uint32 context_id, ik_window_size_info_t *p_window_size_info);
    uint32 (*set_cfa_window_size_info)(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info);
    uint32 (*get_cfa_window_size_info)(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info);

    uint32 (*set_static_bpc)(const uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
    uint32 (*get_static_bpc)(const uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);

    uint32 (*set_static_bpc_internal)(const uint32 context_id, const ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
    uint32 (*get_static_bpc_internal)(const uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);

    uint32 (*set_vignette_compensation)(const uint32 context_id, const ik_vignette_t *p_vignette);
    uint32 (*get_vignette_compensation)(const uint32 context_id, ik_vignette_t *p_vignette);

    uint32 (*set_static_bpc_enable_info)(const uint32 context_id, const uint32 enable);
    uint32 (*get_static_bpc_enable_info)(const uint32 context_id, uint32 *enable);

    uint32 (*set_vignette_enable_info)(const uint32 context_id, const uint32 enable);
    uint32 (*get_vignette_enable_info)(const uint32 context_id, uint32 *enable);

    uint32 (*set_calib_warp_info)(const uint32 context_id, const ik_warp_info_t *p_calib_warp_info);
    uint32 (*get_calib_warp_info)(const uint32 context_id, ik_warp_info_t *p_calib_warp_info);

    uint32 (*set_calib_ca_warp_info)(const uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info);
    uint32 (*get_calib_ca_warp_info)(const uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info);

    uint32 (*set_dzoom_info)(const uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
    uint32 (*get_dzoom_info)(const uint32 context_id, ik_dzoom_info_t *p_dzoom_info);

    uint32 (*set_warp_enable_info)(const uint32 context_id, const uint32 enable);
    uint32 (*get_warp_enable_info)(const uint32 context_id, uint32 *enable);

    uint32 (*set_cawarp_enable_info)(const uint32 context_id, const uint32 enable);
    uint32 (*get_cawarp_enable_info)(const uint32 context_id, uint32 *enable);

    uint32 (*set_dmy_range_info)(const uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range);
    uint32 (*get_dmy_range_info)(const uint32 context_id, ik_dummy_margin_range_t *p_dmy_range);

    uint32 (*set_active_win)(const uint32 context_id, const ik_vin_active_window_t *p_active_win);
    uint32 (*get_active_win)(const uint32 context_id, ik_vin_active_window_t *p_active_win);

    //normal filters

    uint32 (*set_static_blc)(uint32 context_id, const ik_static_blc_level_t *p_static_blc);
    uint32 (*get_static_blc)(uint32 context_id, ik_static_blc_level_t *p_static_blc);

    uint32 (*set_before_ce_wb_gain)(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
    uint32 (*get_before_ce_wb_gain)(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);

    uint32 (*set_after_ce_wb_gain)(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
    uint32 (*get_after_ce_wb_gain)(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);

    uint32 (*set_ae_gain)(uint32 context_id, const ik_ae_gain_t *p_ae_gain);
    uint32 (*get_ae_gain)(uint32 context_id, ik_ae_gain_t *p_ae_gain);

    uint32 (*set_global_dgain)(uint32 context_id, const ik_global_dgain_t *p_global_dgain);
    uint32 (*get_global_dgain)(uint32 context_id, ik_global_dgain_t *p_global_dgain);

    uint32 (*set_cfa_leakage_filter)(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
    uint32 (*get_cfa_leakage_filter)(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);

    uint32 (*set_anti_aliasing)(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
    uint32 (*get_anti_aliasing)(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);

    uint32 (*set_dynamic_bpc)(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
    uint32 (*get_dynamic_bpc)(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);

    uint32 (*set_grgb_mismatch)(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
    uint32 (*get_grgb_mismatch)(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);

    uint32 (*set_cfa_noise_filter)(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
    uint32 (*get_cfa_noise_filter)(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);

    uint32 (*set_dgain_sat_lvl)(uint32 context_id, const ik_dgain_saturation_level_t *p_dgain_saturation_level);
    uint32 (*get_dgain_sat_lvl)(uint32 context_id, ik_dgain_saturation_level_t *p_dgain_saturation_level);

    uint32 (*set_demosaic)(uint32 context_id, const ik_demosaic_t *p_demosaic);
    uint32 (*get_demosaic)(uint32 context_id, ik_demosaic_t *p_demosaic);

    uint32 (*set_color_correction_reg)(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg);
    uint32 (*get_color_correction_reg)(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg);

    uint32 (*set_color_correction)(uint32 context_id, const ik_color_correction_t *p_color_correction);
    uint32 (*get_color_correction)(uint32 context_id, ik_color_correction_t *p_color_correction);

    uint32 (*set_pre_cc_gain)(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain);
    uint32 (*get_pre_cc_gain)(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain);

    uint32 (*set_tone_curve)(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
    uint32 (*get_tone_curve)(uint32 context_id, ik_tone_curve_t *p_tone_curve);

    uint32 (*set_rgb_to_yuv_matrix)(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
    uint32 (*get_rgb_to_yuv_matrix)(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);

    uint32 (*set_rgb_ir)(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
    uint32 (*get_rgb_ir)(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);

    uint32 (*set_chroma_scale)(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
    uint32 (*get_chroma_scale)(uint32 context_id, ik_chroma_scale_t *p_chroma_scale);

    uint32 (*set_chroma_median_filter)(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
    uint32 (*get_chroma_median_filter)(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter);

    uint32 (*set_first_luma_processing_mode)(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
    uint32 (*get_first_luma_processing_mode)(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);

    uint32 (*set_advance_spatial_filter)(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
    uint32 (*get_advance_spatial_filter)(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);

    uint32 (*set_fstshpns_both)(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
    uint32 (*get_fstshpns_both)(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);

    uint32 (*set_fstshpns_noise)(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
    uint32 (*get_fstshpns_noise)(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);

    uint32 (*set_fstshpns_fir)(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
    uint32 (*get_fstshpns_fir)(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);

    uint32 (*set_fstshpns_coring)(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
    uint32 (*get_fstshpns_coring)(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);

    uint32 (*set_fstshpns_coring_index_scale)(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
    uint32 (*get_fstshpns_coring_index_scale)(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);

    uint32 (*set_fstshpns_min_coring_result)(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
    uint32 (*get_fstshpns_min_coring_result)(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);

    uint32 (*set_fstshpns_max_coring_result)(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
    uint32 (*get_fstshpns_max_coring_result)(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);

    uint32 (*set_fstshpns_scale_coring)(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
    uint32 (*get_fstshpns_scale_coring)(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);

    uint32 (*set_fnlshpns_both)(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
    uint32 (*get_fnlshpns_both)(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both);

    uint32 (*set_fnlshpns_noise)(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
    uint32 (*get_fnlshpns_noise)(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise);

    uint32 (*set_fnlshpns_fir)(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
    uint32 (*get_fnlshpns_fir)(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir);

    uint32 (*set_fnlshpns_coring)(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
    uint32 (*get_fnlshpns_coring)(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring);

    uint32 (*set_fnlshpns_coring_index_scale)(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
    uint32 (*get_fnlshpns_coring_index_scale)(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);

    uint32 (*set_fnlshpns_min_coring_result)(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);
    uint32 (*get_fnlshpns_min_coring_result)(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);

    uint32 (*set_fnlshpns_max_coring_result)(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);
    uint32 (*get_fnlshpns_max_coring_result)(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);

    uint32 (*set_fnlshpns_scale_coring)(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring);
    uint32 (*get_fnlshpns_scale_coring)(uint32 context_id, ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring);

    uint32 (*set_fnlshpns_both_tdt)(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
    uint32 (*get_fnlshpns_both_tdt)(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);

    uint32 (*set_chroma_filter)(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
    uint32 (*get_chroma_filter)(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);

    uint32 (*set_video_mctf)(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
    uint32 (*get_video_mctf)(uint32 context_id, ik_video_mctf_t *p_video_mctf);

    uint32 (*set_video_mctf_ta)(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
    uint32 (*get_video_mctf_ta)(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta);

    uint32 (*set_video_mctf_and_final_sharpen)(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
    uint32 (*get_video_mctf_and_final_sharpen)(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen);

    uint32 (*set_motion_detect)(uint32 context_id, const ik_motion_detect_t *p_motion_detect);
    uint32 (*get_motion_detect)(uint32 context_id, ik_motion_detect_t *p_motion_detect);

    uint32 (*set_motion_detect_pos_dep)(uint32 context_id, const ik_pos_dep33_t *p_motion_detect);
    uint32 (*get_motion_detect_pos_dep)(uint32 context_id, ik_pos_dep33_t *p_motion_detect);

    uint32 (*set_motion_detect_and_mctf)(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
    uint32 (*get_motion_detect_and_mctf)(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);

    //hdr
    uint32 (*set_hdr_raw_offset)(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info);
    uint32 (*get_hdr_raw_offset)(uint32 context_id, ik_hdr_raw_info_t *p_raw_info);

    uint32 (*set_exp0_fe_static_blc)(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
    uint32 (*get_exp0_fe_static_blc)(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);

    uint32 (*set_exp1_fe_static_blc)(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc);
    uint32 (*get_exp1_fe_static_blc)(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc);

    uint32 (*set_exp2_fe_static_blc)(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc);
    uint32 (*get_exp2_fe_static_blc)(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc);

    uint32 (*set_exp0_fe_wb_gain)(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
    uint32 (*get_exp0_fe_wb_gain)(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);

    uint32 (*set_exp1_fe_wb_gain)(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
    uint32 (*get_exp1_fe_wb_gain)(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);

    uint32 (*set_exp2_fe_wb_gain)(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
    uint32 (*get_exp2_fe_wb_gain)(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);

    uint32 (*set_exp0_fe_dgain_sat_lvl)(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp0_frontend_dgain_sat_lvl);
    uint32 (*get_exp0_fe_dgain_sat_lvl)(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp0_frontend_dgain_sat_lvl);

    uint32 (*set_exp1_fe_dgain_sat_lvl)(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp1_frontend_dgain_sat_lvl);
    uint32 (*get_exp1_fe_dgain_sat_lvl)(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp1_frontend_dgain_sat_lvl);

    uint32 (*set_exp2_fe_dgain_sat_lvl)(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp2_frontend_dgain_sat_lvl);
    uint32 (*get_exp2_fe_dgain_sat_lvl)(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp2_frontend_dgain_sat_lvl);

    uint32 (*set_ce)(uint32 context_id, const ik_ce_t *p_ce);
    uint32 (*get_ce)(uint32 context_id, ik_ce_t *p_ce);

    uint32 (*set_ce_input_table)(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
    uint32 (*get_ce_input_table)(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);

    uint32 (*set_ce_out_table)(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
    uint32 (*get_ce_out_table)(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);

    uint32 (*set_ce_ext_hds)(uint32 context_id, const ik_ce_external_hds_t *p_ce_ext_hds);
    uint32 (*get_ce_ext_hds)(uint32 context_id, ik_ce_external_hds_t *p_ce_ext_hds);

    //cv2
    uint32 (*set_hdr_blend)(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);
    uint32 (*get_hdr_blend)(uint32 context_id, ik_hdr_blend_t *p_hdr_blend);

    uint32 (*set_front_end_tone_curve)(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_ce);
    uint32 (*get_front_end_tone_curve)(uint32 context_id, ik_frontend_tone_curve_t *p_fe_ce);

    uint32 (*set_resampler_str)(uint32 context_id, const ik_resampler_strength_t *p_resampler_strength);
    uint32 (*get_resampler_str)(uint32 context_id, ik_resampler_strength_t *p_resampler_strength);

    uint32 (*set_histogram_info)(uint32 context_id, const ik_histogram_info_t *p_histogram_info);
    uint32 (*get_histogram_info)(uint32 context_id, ik_histogram_info_t *p_histogram_info);

    uint32 (*set_pg_histogram_info)(uint32 context_id, const ik_histogram_info_t *p_histogram_info);
    uint32 (*get_pg_histogram_info)(uint32 context_id, ik_histogram_info_t *p_histogram_info);

    uint32 (*set_pseudo_y_info)(uint32 context_id, const ik_aaa_pseudo_y_info_t *p_histogram_info);
    uint32 (*get_pseudo_y_info)(uint32 context_id, ik_aaa_pseudo_y_info_t *p_histogram_info);

    uint32 (*set_flip_mode)(const uint32 context_id, const uint32 mode);
    uint32 (*get_flip_mode)(const uint32 context_id, uint32 *mode);

    uint32 (*set_hi_anti_aliasing)(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing);
    uint32 (*get_hi_anti_aliasing)(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing);

    uint32 (*set_hi_cfa_leakage_filter)(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);
    uint32 (*get_hi_cfa_leakage_filter)(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);

    uint32 (*set_hi_dynamic_bpc)(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);
    uint32 (*get_hi_dynamic_bpc)(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);

    uint32 (*set_hi_grgb_mismatch)(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);
    uint32 (*get_hi_grgb_mismatch)(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);

    uint32 (*set_hi_chroma_median_filter)(uint32 context_id, const ik_hi_chroma_median_filter_t *pHisoChroma_median);
    uint32 (*get_hi_chroma_median_filter)(uint32 context_id, ik_hi_chroma_median_filter_t *pHisoChroma_median);

    uint32 (*set_hi_cfa_noise_filter)(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);
    uint32 (*get_hi_cfa_noise_filter)(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);

    uint32 (*set_hi_demosaic)(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic);
    uint32 (*get_hi_demosaic)(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic);

    //hiso low2
    uint32 (*set_li2_anti_aliasing)(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing);
    uint32 (*get_li2_anti_aliasing)(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing);

    uint32 (*set_li2_cfa_leakage_filter)(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);
    uint32 (*get_li2_cfa_leakage_filter)(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);

    uint32 (*set_li2_dynamic_bpc)(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);
    uint32 (*get_li2_dynamic_bpc)(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);

    uint32 (*set_li2_grgb_mismatch)(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);
    uint32 (*get_li2_grgb_mismatch)(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);

    uint32 (*set_li2_cfa_noise_filter)(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);
    uint32 (*get_li2_cfa_noise_filter)(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);

    uint32 (*set_li2_demosaic)(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic);
    uint32 (*get_li2_demosaic)(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic);

    //asf
    uint32 (*set_hi_asf)(uint32 context_id, const ik_hi_asf_t *p_hi_asf);
    uint32 (*get_hi_asf)(uint32 context_id, ik_hi_asf_t *p_hi_asf);

    uint32 (*set_li2_asf)(uint32 context_id, const ik_li2_asf_t *p_li2_asf);
    uint32 (*get_li2_asf)(uint32 context_id, ik_li2_asf_t *p_li2_asf);

    uint32 (*set_hi_low_asf)(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf);
    uint32 (*get_hi_low_asf)(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf);

    uint32 (*set_hi_med1_asf)(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf);
    uint32 (*get_hi_med1_asf)(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf);

    uint32 (*set_hi_med2_asf)(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf);
    uint32 (*get_hi_med2_asf)(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf);

    uint32 (*set_hi_high_asf)(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf);
    uint32 (*get_hi_high_asf)(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf);

    uint32 (*set_hi_high2_asf)(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf);
    uint32 (*get_hi_high2_asf)(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf);

    uint32 (*set_chroma_asf)(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf);
    uint32 (*get_chroma_asf)(uint32 context_id, ik_chroma_asf_t *p_chroma_asf);

    uint32 (*set_hi_chroma_asf)(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf);
    uint32 (*get_hi_chroma_asf)(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf);

    uint32 (*set_hi_low_chroma_asf)(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);
    uint32 (*get_hi_low_chroma_asf)(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);

    // sharpen
    // --high--
    uint32 (*set_hi_high_shpns_both)(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both);
    uint32 (*get_hi_high_shpns_both)(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both);

    uint32 (*set_hi_high_shpns_noise)(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);
    uint32 (*get_hi_high_shpns_noise)(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);

    uint32 (*set_hi_high_shpns_coring)(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);
    uint32 (*get_hi_high_shpns_coring)(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);

    uint32 (*set_hi_high_shpns_fir)(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);
    uint32 (*get_hi_high_shpns_fir)(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);

    uint32 (*set_hi_high_shpns_cor_idx_scl)(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);
    uint32 (*get_hi_high_shpns_cor_idx_scl)(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);

    uint32 (*set_hi_high_shpns_min_cor_rst)(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);
    uint32 (*get_hi_high_shpns_min_cor_rst)(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);

    uint32 (*set_hi_high_shpns_max_cor_rst)(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);
    uint32 (*get_hi_high_shpns_max_cor_rst)(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);

    uint32 (*set_hi_high_shpns_scl_cor)(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);
    uint32 (*get_hi_high_shpns_scl_cor)(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);

    // --med--
    uint32 (*set_hi_med_shpns_both)(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both);
    uint32 (*get_hi_med_shpns_both)(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both);

    uint32 (*set_hi_med_shpns_noise)(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);
    uint32 (*get_hi_med_shpns_noise)(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);

    uint32 (*set_hi_med_shpns_coring)(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);
    uint32 (*get_hi_med_shpns_coring)(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);

    uint32 (*set_hi_med_shpns_fir)(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);
    uint32 (*get_hi_med_shpns_fir)(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);

    uint32 (*set_hi_med_shpns_cor_idx_scl)(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);
    uint32 (*get_hi_med_shpns_cor_idx_scl)(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);

    uint32 (*set_hi_med_shpns_min_cor_rst)(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);
    uint32 (*get_hi_med_shpns_min_cor_rst)(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);

    uint32 (*set_hi_med_shpns_max_cor_rst)(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);
    uint32 (*get_hi_med_shpns_max_cor_rst)(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);

    uint32 (*set_hi_med_shpns_scl_cor)(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);
    uint32 (*get_hi_med_shpns_scl_cor)(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);

    // --low2--
    uint32 (*set_li2_shpns_both)(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both);
    uint32 (*get_li2_shpns_both)(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both);

    uint32 (*set_li2_shpns_noise)(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise);
    uint32 (*get_li2_shpns_noise)(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise);

    uint32 (*set_li2_shpns_coring)(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring);
    uint32 (*get_li2_shpns_coring)(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring);

    uint32 (*set_li2_shpns_fir)(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir);
    uint32 (*get_li2_shpns_fir)(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir);

    uint32 (*set_li2_shpns_cor_idx_scl)(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);
    uint32 (*get_li2_shpns_cor_idx_scl)(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);

    uint32 (*set_li2_shpns_min_cor_rst)(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);
    uint32 (*get_li2_shpns_min_cor_rst)(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);

    uint32 (*set_li2_shpns_max_cor_rst)(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);
    uint32 (*get_li2_shpns_max_cor_rst)(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);

    uint32 (*set_li2_shpns_scl_cor)(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);
    uint32 (*get_li2_shpns_scl_cor)(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);

    // --hili--
    uint32 (*set_hili_shpns_both)(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both);
    uint32 (*get_hili_shpns_both)(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both);

    uint32 (*set_hili_shpns_noise)(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise);
    uint32 (*get_hili_shpns_noise)(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise);

    uint32 (*set_hili_shpns_coring)(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring);
    uint32 (*get_hili_shpns_coring)(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring);

    uint32 (*set_hili_shpns_fir)(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir);
    uint32 (*get_hili_shpns_fir)(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir);

    uint32 (*set_hili_shpns_cor_idx_scl)(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);
    uint32 (*get_hili_shpns_cor_idx_scl)(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);

    uint32 (*set_hili_shpns_min_cor_rst)(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);
    uint32 (*get_hili_shpns_min_cor_rst)(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);

    uint32 (*set_hili_shpns_max_cor_rst)(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);
    uint32 (*get_hili_shpns_max_cor_rst)(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);

    uint32 (*set_hili_shpns_scl_cor)(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);
    uint32 (*get_hili_shpns_scl_cor)(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);

    //chroma filter
    uint32 (*set_hi_chroma_filter_high)(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);
    uint32 (*get_hi_chroma_filter_high)(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);

    uint32 (*set_hi_chroma_filter_pre)(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);
    uint32 (*get_hi_chroma_filter_pre)(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);

    uint32 (*set_hi_chroma_filter_med)(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);
    uint32 (*get_hi_chroma_filter_med)(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);

    uint32 (*set_hi_chroma_filter_low)(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);
    uint32 (*get_hi_chroma_filter_low)(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);

    uint32 (*set_hi_chroma_filter_very_low)(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);
    uint32 (*get_hi_chroma_filter_very_low)(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);

    //combine
    uint32 (*set_hi_luma_combine)(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine);
    uint32 (*get_hi_luma_combine)(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine);

    uint32 (*set_hi_low_asf_combine)(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine);
    uint32 (*get_hi_low_asf_combine)(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine);

    uint32 (*set_hi_chroma_fltr_med_com)(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);
    uint32 (*get_hi_chroma_fltr_med_com)(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);

    uint32 (*set_hi_chroma_fltr_low_com)(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);
    uint32 (*get_hi_chroma_fltr_low_com)(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);

    uint32 (*set_hi_chroma_fltr_very_low_com)(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);
    uint32 (*get_hi_chroma_fltr_very_low_com)(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);

    uint32 (*set_hili_combine)(uint32 context_id, const ik_hili_combine_t *p_hili_combine);
    uint32 (*get_hili_combine)(uint32 context_id, ik_hili_combine_t *p_hili_combine);

    uint32 (*set_hi_mid_high_freq_recover)(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);
    uint32 (*get_hi_mid_high_freq_recover)(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);

    uint32 (*set_hi_luma_blend)(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend);
    uint32 (*get_hi_luma_blend)(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend);

    uint32 (*set_hi_nonsmooth_detect)(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);
    uint32 (*get_hi_nonsmooth_detect)(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);

    uint32 (*set_hi_select)(uint32 context_id, const ik_hi_select_t *p_hi_select);
    uint32 (*get_hi_select)(uint32 context_id, ik_hi_select_t *p_hi_select);

#if INTERNAL_EARLY_TEST_ENV
    uint32 (*set_fusion)(uint32 context_id, const ik_fusion_t *p_fusion);
    uint32 (*get_fusion)(uint32 context_id, ik_fusion_t *p_fusion);

    uint32 (*set_mono_static_bpc)(const uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
    uint32 (*get_mono_static_bpc)(const uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);

    uint32 (*set_mono_vignette_compensation)(const uint32 context_id, const ik_vignette_t *p_vignette);
    uint32 (*get_mono_vignette_compensation)(const uint32 context_id, ik_vignette_t *p_vignette);

    uint32 (*set_mono_static_bpc_enable_info)(const uint32 context_id, const uint32 enable);
    uint32 (*get_mono_static_bpc_enable_info)(const uint32 context_id, uint32 *enable);

    uint32 (*set_mono_vignette_enable_info)(const uint32 context_id, const uint32 enable);
    uint32 (*get_mono_vignette_enable_info)(const uint32 context_id, uint32 *enable);

    uint32 (*set_mono_before_ce_wb_gain)(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
    uint32 (*get_mono_before_ce_wb_gain)(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);

    uint32 (*set_mono_after_ce_wb_gain)(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
    uint32 (*get_mono_after_ce_wb_gain)(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);

    uint32 (*set_mono_cfa_leakage_filter)(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
    uint32 (*get_mono_cfa_leakage_filter)(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);

    uint32 (*set_mono_anti_aliasing)(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
    uint32 (*get_mono_anti_aliasing)(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);

    uint32 (*set_mono_dynamic_bpc)(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
    uint32 (*get_mono_dynamic_bpc)(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);

    uint32 (*set_mono_grgb_mismatch)(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
    uint32 (*get_mono_grgb_mismatch)(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);

    uint32 (*set_mono_cfa_noise_filter)(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
    uint32 (*get_mono_cfa_noise_filter)(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);

    uint32 (*set_mono_demosaic)(uint32 context_id, const ik_demosaic_t *p_demosaic);
    uint32 (*get_mono_demosaic)(uint32 context_id, ik_demosaic_t *p_demosaic);

    uint32 (*set_mono_color_correction_reg)(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg);
    uint32 (*get_mono_color_correction_reg)(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg);

    uint32 (*set_mono_color_correction)(uint32 context_id, const ik_color_correction_t *p_color_correction);
    uint32 (*get_mono_color_correction)(uint32 context_id, ik_color_correction_t *p_color_correction);

    uint32 (*set_mono_tone_curve)(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
    uint32 (*get_mono_tone_curve)(uint32 context_id, ik_tone_curve_t *p_tone_curve);

    uint32 (*set_mono_rgb_to_yuv_matrix)(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
    uint32 (*get_mono_rgb_to_yuv_matrix)(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);

    uint32 (*set_mono_rgb_ir)(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
    uint32 (*get_mono_rgb_ir)(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);

    uint32 (*set_mono_first_luma_processing_mode)(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
    uint32 (*get_mono_first_luma_processing_mode)(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);

    uint32 (*set_mono_advance_spatial_filter)(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
    uint32 (*get_mono_advance_spatial_filter)(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);

    uint32 (*set_mono_fstshpns_both)(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
    uint32 (*get_mono_fstshpns_both)(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);

    uint32 (*set_mono_fstshpns_noise)(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
    uint32 (*get_mono_fstshpns_noise)(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);

    uint32 (*set_mono_fstshpns_fir)(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
    uint32 (*get_mono_fstshpns_fir)(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);

    uint32 (*set_mono_fstshpns_coring)(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
    uint32 (*get_mono_fstshpns_coring)(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);

    uint32 (*set_mono_fstshpns_coring_index_scale)(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
    uint32 (*get_mono_fstshpns_coring_index_scale)(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);

    uint32 (*set_mono_fstshpns_min_coring_result)(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
    uint32 (*get_mono_fstshpns_min_coring_result)(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);

    uint32 (*set_mono_fstshpns_max_coring_result)(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
    uint32 (*get_mono_fstshpns_max_coring_result)(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);

    uint32 (*set_mono_fstshpns_scale_coring)(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
    uint32 (*get_mono_fstshpns_scale_coring)(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);

    uint32 (*set_wide_chroma_filter)(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
    uint32 (*get_wide_chroma_filter)(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);

    uint32 (*set_wide_chroma_combine)(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_wide_chroma_combine);
    uint32 (*get_wide_chroma_combine)(uint32 context_id, ik_wide_chroma_filter_combine_t *p_wide_chroma_combine);

    uint32 (*set_mono_exp0_fe_static_blc)(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
    uint32 (*get_mono_exp0_fe_static_blc)(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);

    uint32 (*set_mono_exp0_fe_wb_gain)(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
    uint32 (*get_mono_exp0_fe_wb_gain)(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);

    uint32 (*set_mono_ce)(uint32 context_id, const ik_ce_t *p_ce);
    uint32 (*get_mono_ce)(uint32 context_id, ik_ce_t *p_ce);

    uint32 (*set_mono_ce_input_table)(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
    uint32 (*get_mono_ce_input_table)(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);

    uint32 (*set_mono_ce_out_table)(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
    uint32 (*get_mono_ce_out_table)(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);
#endif

    /* internal API */
    uint32 (*set_warp_internal)(const uint32 context_id, const ik_warp_internal_info_t *p_warp_internal);
    uint32 (*get_warp_internal)(const uint32 context_id, ik_warp_internal_info_t *p_warp_internal);

    uint32 (*set_ca_warp_internal)(const uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal);
    uint32 (*get_ca_warp_internal)(const uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal);

    uint32 (*set_mctf_internal)(uint32 context_id, const ik_mctf_internal_t *p_mctf_internal);
    uint32 (*get_mctf_internal)(uint32 context_id, ik_mctf_internal_t *p_mctf_internal);

    uint32 (*set_y2y_use_cc_enable_info)(uint32 context_id, const uint32 enable);
    uint32 (*get_y2y_use_cc_enable_info)(uint32 context_id, uint32 *enable);
} amba_ik_filter_method_t;

typedef struct {
    uint32 (*execute_iso_config)(uint32 context_id);
//    uint32 (*execute_hdr_ce_config)(uint32 context_id);
} amba_ik_executer_method_t;

typedef struct {
    uint32 active_iso_idx;
    uint32 locked_iso_idx;
    amba_ik_iso_config_table_index_t active_iso_tbl_idx;
    amba_ik_hiso_config_table_index_t active_hiso_tbl_idx;
    /* min_iso_running_number < 255 && min_iso_running_number < (config number * N)*/
    uint16 iso_running_number;
    uint16 max_running_number;
} amba_ik_active_config_state_t;

typedef struct {
    uint32 initial_flag;
    IKC_OS_MUTEX_TYPE obsolete_mutex;
    amba_ik_attribute_t attribute;
    amba_ik_filter_method_t filter_methods;
    amba_ik_executer_method_t executer_method;
    amba_ik_active_config_state_t active_config_state;
} amba_ik_context_organization_t;

typedef struct {
    //yuv input
    uint8 use_y2y_enable_info_updated;

    //sensor information
    uint8 sensor_information_updated;

    //3a statistic
    uint8 aaa_stat_info_updated;
    uint8 af_stat_ex_info_updated;
    uint8 pg_af_stat_ex_info_updated;
    uint8 aaa_float_tile_info_updated;
    uint8 histogram_info_update;
    uint8 histogram_info_pg_update;
    uint8 aaa_pseudo_y_update;

    //calibration
    uint8 window_size_info_updated;
    uint8 cfa_window_size_info_updated;
    uint8 system_performance_info_updated;
    uint8 static_bpc_updated;
    uint8 vignette_compensation_updated;
    uint8 vignette_compensation_internal_updated;
    uint8 calib_warp_info_updated;
    uint8 warp_bind_info_updated;
    uint8 calib_ca_warp_info_updated;
    uint8 dzoom_info_updated;
    uint8 warp_enable_updated;
    uint8 cawarp_enable_updated;
    uint8 dmy_range_updated;
    uint8 flip_mode_updated;
    uint8 vin_active_win_updated;
    uint8 warp_internal_updated;
    uint8 cawarp_internal_updated;
    uint8 static_bpc_internal_updated;

    //normal filters
    uint8 static_blc_updated;
    uint8 deferred_blc_updated;
    uint8 before_ce_wb_gain_updated;
    uint8 after_ce_wb_gain_updated;
    uint8 ae_gain_updated;
    uint8 global_dgain_updated;
    uint8 cfa_leakage_filter_updated;
    uint8 anti_aliasing_updated;
    uint8 dynamic_bpc_updated;
    uint8 grgb_mismatch_updated;
    uint8 cfa_noise_filter_updated;
    uint8 dgain_sat_lvl_updated;
    uint8 local_exposure_updated;
    uint8 demosaic_updated;
    uint8 pre_cc_gain_updated;
    uint8 color_correction_reg_updated;
    uint8 color_correction_updated;
    uint8 tone_curve_updated;
    uint8 rgb_to_yuv_matrix_updated;
    uint8 rgb_ir_updated;
    uint8 cdnr_updated;
    uint8 chroma_scale_updated;
    uint8 chroma_median_filter_updated;
    uint8 first_luma_processing_mode_updated;
    uint8 advance_spatial_filter_updated;
    uint8 fstshpns_both_updated;
    uint8 fstshpns_noise_updated;
    uint8 fstshpns_fir_updated;
    uint8 fstshpns_coring_updated;
    uint8 fstshpns_coring_index_scale_updated;
    uint8 fstshpns_min_coring_result_updated;
    uint8 fstshpns_max_coring_result_updated;
    uint8 fstshpns_scale_coring_updated;
    uint8 fstshpns_both_tdt_updated;
    uint8 fnlshpns_both_updated;
    uint8 fnlshpns_noise_updated;
    uint8 fnlshpns_fir_updated;
    uint8 fnlshpns_coring_updated;
    uint8 fnlshpns_coring_index_scale_updated;
    uint8 fnlshpns_min_coring_result_updated;
    uint8 fnlshpns_max_coring_result_updated;
    uint8 fnlshpns_scale_coring_updated;
    uint8 fnlshpns_both_tdt_updated;
    uint8 chroma_filter_updated;
    uint8 video_mctf_updated;
    uint8 internal_video_mctf_updated;
    uint8 video_mctf_ta_updated;
    uint8 video_mctf_ta_y_updated;
    uint8 video_mctf_ta_cb_updated;
    uint8 video_mctf_ta_cr_updated;
    uint8 video_mctf_and_final_sharpen_updated;
    uint8 motion_detect_updated;
    uint8 motion_detect_pos_dep_updated;
    uint8 motion_detect_and_mctf_updated;
    uint8 internal_idsp_debug_updated;

    //hdr
    uint8 hdr_raw_info_updated;
    uint8 exp0_fe_static_blc_updated;
    uint8 exp1_fe_static_blc_updated;
    uint8 exp2_fe_static_blc_updated;

    uint8 exp0_fe_wb_gain_updated;
    uint8 exp1_fe_wb_gain_updated;
    uint8 exp2_fe_wb_gain_updated;

    uint8 exp0_dgain_sat_lvl_updated;
    uint8 exp1_dgain_sat_lvl_updated;
    uint8 exp2_dgain_sat_lvl_updated;

    uint8 ce_updated;
    uint8 ce_input_table_updated;
    uint8 ce_out_table_updated;
    uint8 ce_use_ext_hds_mode_updated;
    uint8 hdr_blend_updated;

    uint8 fe_tone_curve_updated;
    uint8 resample_str_update;

    // hiso
    uint8 hi_anti_aliasing_updated;
    uint8 hi_cfa_leakage_filter_updated;
    uint8 hi_dynamic_bpc_updated;
    uint8 hi_grgb_mismatch_updated;
    uint8 hi_chroma_median_filter_updated;
    uint8 hi_cfa_noise_filter_updated;
    uint8 hi_demosaic_updated;

    //hiso low2
    uint8 li2_anti_aliasing_updated;
    uint8 li2_cfa_leakage_filter_updated;
    uint8 li2_dynamic_bpc_updated;
    uint8 li2_grgb_mismatch_updated;
    uint8 li2_cfa_noise_filter_updated;
    uint8 li2_demosaic_updated;

    //asf
    uint8 hi_asf_updated;
    uint8 li2_asf_updated;
    uint8 hi_low_asf_updated;
    uint8 hi_med1_asf_updated;
    uint8 hi_med2_asf_updated;
    uint8 hi_high_asf_updated;
    uint8 hi_high2_asf_updated;
    uint8 chroma_asf_updated;
    uint8 hi_chroma_asf_updated;
    uint8 hi_low_chroma_asf_updated;

    // sharpen
    // --high--
    uint8 hi_high_shpns_both_updated;
    uint8 hi_high_shpns_noise_updated;
    uint8 hi_high_shpns_coring_updated;
    uint8 hi_high_shpns_fir_updated;
    uint8 hi_high_shpns_cor_idx_scl_updated;
    uint8 hi_high_shpns_min_cor_rst_updated;
    uint8 hi_high_shpns_max_cor_rst_updated;
    uint8 hi_high_shpns_scl_cor_updated;

    // --med--
    uint8 hi_med_shpns_both_updated;
    uint8 hi_med_shpns_noise_updated;
    uint8 hi_med_shpns_coring_updated;
    uint8 hi_med_shpns_fir_updated;
    uint8 hi_med_shpns_cor_idx_scl_updated;
    uint8 hi_med_shpns_min_cor_rst_updated;
    uint8 hi_med_shpns_max_cor_rst_updated;
    uint8 hi_med_shpns_scl_cor_updated;

    // --low2--
    uint8 li2_shpns_both_updated;
    uint8 li2_shpns_noise_updated;
    uint8 li2_shpns_coring_updated;
    uint8 li2_shpns_fir_updated;
    uint8 li2_shpns_cor_idx_scl_updated;
    uint8 li2_shpns_min_cor_rst_updated;
    uint8 li2_shpns_max_cor_rst_updated;
    uint8 li2_shpns_scl_cor_updated;

    // --hili--
    uint8 hili_shpns_both_updated;
    uint8 hili_shpns_noise_updated;
    uint8 hili_shpns_coring_updated;
    uint8 hili_shpns_fir_updated;
    uint8 hili_shpns_cor_idx_scl_updated;
    uint8 hili_shpns_min_cor_rst_updated;
    uint8 hili_shpns_max_cor_rst_updated;
    uint8 hili_shpns_scl_cor_updated;

    //chroma filter
    uint8 hi_chroma_filter_high_updated;
    uint8 hi_chroma_filter_pre_updated;
    uint8 hi_chroma_filter_med_updated;
    uint8 hi_chroma_filter_low_updated;
    uint8 hi_chroma_filter_very_low_updated;

    //combine
    uint8 hi_luma_combine_updated;
    uint8 hi_low_asf_combine_updated;
    uint8 hi_chroma_fltr_med_com_updated;
    uint8 hi_chroma_fltr_low_com_updated;
    uint8 hi_chroma_fltr_very_low_com_updated;
    uint8 hili_combine_updated;
    uint8 hi_mid_high_freq_recover_updated;
    uint8 hi_luma_blend_updated;
    uint8 hi_nonsmooth_detect_updated;
    uint8 hi_select_updated;

#if INTERNAL_EARLY_TEST_ENV
    //mono
    uint8 fusion;
    uint8 mono_static_bpc_updated;
    uint8 mono_vignette_compensation_updated;
    uint8 mono_static_blc_updated;
    uint8 mono_before_ce_wb_gain_updated;
    uint8 mono_after_ce_wb_gain_updated;
    uint8 mono_cfa_leakage_filter_updated;
    uint8 mono_anti_aliasing_updated;
    uint8 mono_dynamic_bpc_updated;
    uint8 mono_grgb_mismatch_updated;
    uint8 mono_cfa_noise_filter_updated;
    uint8 mono_local_exposure_updated;
    uint8 mono_demosaic_updated;
    uint8 mono_color_correction_reg_updated;
    uint8 mono_color_correction_updated;
    uint8 mono_tone_curve_updated;
    uint8 mono_rgb_to_yuv_matrix_updated;
    uint8 mono_first_luma_processing_mode_updated;
    uint8 mono_advance_spatial_filter_updated;
    uint8 mono_fstshpns_both_updated;
    uint8 mono_fstshpns_noise_updated;
    uint8 mono_fstshpns_fir_updated;
    uint8 mono_fstshpns_coring_updated;
    uint8 mono_fstshpns_coring_index_scale_updated;
    uint8 mono_fstshpns_min_coring_result_updated;
    uint8 mono_fstshpns_max_coring_result_updated;
    uint8 mono_fstshpns_scale_coring_updated;

    uint8 mono_exp0_fe_static_blc_updated;
    uint8 mono_exp0_fe_wb_gain_updated;

    uint8 mono_ce_updated;
    uint8 mono_ce_input_table_updated;
    uint8 mono_ce_out_table_updated;
    uint8 wide_chroma_filter;
    uint8 wide_chroma_filter_combine;
#endif

    // TODO:
} amba_ik_filter_id_list_t;

typedef struct {
    //sensor information
    uint8 sensor_information_updated;
    //TBD
} amba_ik_filter_hiso_id_list_t;

typedef struct {
    amba_ik_filter_id_list_t liso;
    amba_ik_filter_hiso_id_list_t hiso;
} amba_ik_filter_update_flags_t;

typedef struct {
    //debug utility
    uint8 diag_case_id;
    img_dsp_img_debug_mode_t debug_utility;
    uint32 sbp_highlight_mode;

    //sensor information
    ik_vin_sensor_info_t sensor_info;

    //3a statistic
    ik_aaa_stat_info_t aaa_stat_info;
    ik_af_stat_ex_info_t af_stat_ex_info;
    ik_pg_af_stat_ex_info_t pg_af_stat_ex_info;
    ik_aaa_float_info_t aaa_float_tile_info;

    //calibration
    ik_window_size_info_t window_size_info;
    ik_cfa_window_size_info_t cfa_window_size_info;
    ik_cawarp_compensation_t cawarp_compensation;
    uint32 sbp_enable;
    ik_static_bad_pxl_cor_t static_bpc;
    uint32 vig_enable;
    ik_vignette_t vignette_compensation;
    ik_warp_info_t calib_warp_info;
    ik_bind_info_t warp_bind_info;
    ik_warp_video_stitch_t warp_stitch_vertical;
    ik_cawarp_info_t calib_ca_warp_info;
    ik_dzoom_info_t dzoom_info;
    int32  warp_enable;
    int32  cawarp_enable;
    ik_dummy_margin_range_t dmy_range;
    ik_vin_active_window_t active_window;
    uint32  flip_mode;
    ik_warp_internal_info_t warp_internal;
    ik_cawarp_internal_info_t ca_warp_internal;
    ik_static_bad_pixel_correction_internal_t sbp_internal;

    //normal filters
    ik_deferred_blc_level_t deferred_blc;
    ik_wb_gain_t before_ce_wb_gain;
    ik_wb_gain_t after_ce_wb_gain;
    ik_ae_gain_t ae_gain;
    ik_global_dgain_t global_dgain;
    ik_cfa_leakage_filter_t cfa_leakage_filter;
    ik_anti_aliasing_t anti_aliasing;
    ik_dynamic_bad_pixel_correction_t dynamic_bpc;
    ik_grgb_mismatch_t grgb_mismatch;
    ik_cfa_noise_filter_t cfa_noise_filter;
    ik_dgain_saturation_level_t dgain_sauration_level;
    ik_demosaic_t demosaic;
    ik_pre_cc_gain_t pre_cc_gain;
    ik_color_correction_reg_t color_correction_reg;
    ik_color_correction_t color_correction;
    ik_tone_curve_t tone_curve;
    ik_rgb_to_yuv_matrix_t rgb_to_yuv_matrix;
    ik_rgb_ir_t rgb_ir;
    ik_chroma_scale_t chroma_scale;
    ik_chroma_median_filter_t chroma_median_filter;
    ik_first_luma_process_mode_t first_luma_process_mode;
    ik_adv_spatial_filter_t advance_spatial_filter;
    ik_first_sharpen_both_t first_sharpen_both;
    ik_first_sharpen_noise_t first_sharpen_noise;
    ik_first_sharpen_fir_t first_sharpen_fir;
    ik_first_sharpen_coring_t first_sharpen_coring;

    ik_first_sharpen_coring_idx_scale_t first_sharpen_coring_idx_scale;
    ik_first_sharpen_min_coring_result_t first_sharpen_min_coring_result;
    ik_first_sharpen_max_coring_result_t first_sharpen_max_coring_result;
    ik_first_sharpen_scale_coring_t first_sharpen_scale_coring;
    ik_final_sharpen_both_t final_sharpen_both;
    ik_final_sharpen_noise_t final_sharpen_noise;
    ik_final_sharpen_fir_t final_sharpen_fir;
    ik_final_sharpen_coring_t final_sharpen_coring;
    ik_final_sharpen_coring_idx_scale_t final_sharpen_coring_idx_scale;
    ik_final_sharpen_min_coring_result_t final_sharpen_min_coring_result;
    ik_final_sharpen_max_coring_result_t final_sharpen_max_coring_result;
    ik_final_sharpen_scale_coring_t final_sharpen_scale_coring;
    ik_final_sharpen_both_three_d_table_t final_sharpen_both_three_d_table;
    ik_chroma_filter_t chroma_filter;
    ik_video_mctf_t video_mctf;
    ik_mctf_internal_t internal_video_mctf;
    ik_idsp_debug_internal_t  internal_idsp_debug;
    ik_video_mctf_ta_t video_mctf_ta;
    ik_pos_dep33_t video_mctf_and_final_sharpen;
    ik_motion_detect_t motion_detect;
    ik_pos_dep33_t motion_detect_pos_dep;
    ik_motion_detect_and_mctf_t motion_detect_and_mctf;

    //hdr
    ik_hdr_raw_info_t hdr_raw_info;
    ik_static_blc_level_t exp0_frontend_static_blc;
    ik_static_blc_level_t exp1_frontend_static_blc;
    ik_static_blc_level_t exp2_frontend_static_blc;

    ik_frontend_wb_gain_t exp0_frontend_wb_gain;
    ik_frontend_wb_gain_t exp1_frontend_wb_gain;
    ik_frontend_wb_gain_t exp2_frontend_wb_gain;

    ik_frontend_dgain_saturation_level_t exp0_frontend_dgain_saturation_level;
    ik_frontend_dgain_saturation_level_t exp1_frontend_dgain_saturation_level;
    ik_frontend_dgain_saturation_level_t exp2_frontend_dgain_saturation_level;

    ik_ce_t ce;
    ik_ce_input_table_t ce_input_table;
    ik_ce_output_table_t ce_out_table;
    uint8 ce_use_external_hds_mode;

    //CV2
    ik_hdr_blend_t hdr_blend;
    ik_frontend_tone_curve_t fe_tone_curve;
    ik_resampler_strength_t resample_str;
    ik_histogram_info_t hist_info;
    ik_histogram_info_t hist_info_pg;
    ik_aaa_pseudo_y_info_t pseudo_y;

    uint8 func_mode;
    uint8 three_d_table_enable;
    /* internal parameter or buffer, working buffer of  all filter*/
    img_ctx_internal_buffer_t ctx_buf;
    uint32 use_cc;
    uint32 use_cc_reg;
    uint32 use_tone_curve;
    uint32 use_cc_for_yuv2yuv;
    uint32 eis_mode;
    uint32 user_frontend_dgain_saturation_level;   // As a flag to determine frontend_dgain_saturation_level  if 1  using the use's value else calling set_sat_level() to calculate

    //mono
#if INTERNAL_EARLY_TEST_ENV
    ik_fusion_t fusion;
    ik_static_bad_pxl_cor_t mono_static_bpc;
    ik_vignette_t mono_vignette_compensation;

    ik_wb_gain_t mono_before_ce_wb_gain;
    ik_wb_gain_t mono_after_ce_wb_gain;
    ik_cfa_leakage_filter_t mono_cfa_leakage_filter;
    ik_anti_aliasing_t mono_anti_aliasing;
    ik_dynamic_bad_pixel_correction_t mono_dynamic_bpc;
    ik_grgb_mismatch_t mono_grgb_mismatch;
    ik_cfa_noise_filter_t mono_cfa_noise_filter;
    ik_demosaic_t mono_demosaic;
    ik_color_correction_reg_t mono_color_correction_reg;
    ik_color_correction_t mono_color_correction;
    ik_tone_curve_t mono_tone_curve;
    ik_rgb_to_yuv_matrix_t mono_rgb_to_yuv_matrix;
    ik_first_luma_process_mode_t mono_first_luma_process_mode;
    ik_adv_spatial_filter_t mono_advance_spatial_filter;
    ik_first_sharpen_both_t mono_first_sharpen_both;
    ik_first_sharpen_noise_t mono_first_sharpen_noise;
    ik_first_sharpen_fir_t mono_first_sharpen_fir;
    ik_first_sharpen_coring_t mono_first_sharpen_coring;
    ik_first_sharpen_coring_idx_scale_t mono_first_sharpen_coring_idx_scale;
    ik_first_sharpen_min_coring_result_t mono_first_sharpen_min_coring_result;
    ik_first_sharpen_max_coring_result_t mono_first_sharpen_max_coring_result;
    ik_first_sharpen_scale_coring_t mono_first_sharpen_scale_coring;

    ik_chroma_filter_t wide_chroma_filter;
    ik_wide_chroma_filter_combine_t wide_chroma_combine;

    ik_static_blc_level_t mono_exp0_frontend_static_blc;
    ik_frontend_wb_gain_t mono_exp0_frontend_wb_gain;

    ik_ce_t mono_ce;
    ik_ce_input_table_t mono_ce_input_table;
    ik_ce_output_table_t mono_ce_out_table;

    uint32 mono_sbp_enable;
    uint32 mono_vig_enable;
#endif

} amba_ik_input_parameters_t;

typedef struct {
    //hi
    ik_hi_anti_aliasing_t hi_anti_aliasing;
    ik_hi_cfa_leakage_filter_t hi_cfa_leakage_filter;
    ik_hi_dynamic_bpc_t hi_dynamic_bpc;
    ik_hi_grgb_mismatch_t hi_grgb_mismatch;
    ik_hi_chroma_median_filter_t hi_chroma_median_filter;
    ik_hi_cfa_noise_filter_t hi_cfa_noise_filter;
    ik_hi_demosaic_t hi_demosaic;
    //li2
    ik_li2_anti_aliasing_t li2_anti_aliasing;
    ik_li2_cfa_leakage_filter_t li2_cfa_leakage_filter;
    ik_li2_dynamic_bpc_t li2_dynamic_bpc;
    ik_li2_grgb_mismatch_t li2_grgb_mismatch;
    ik_li2_cfa_noise_filter_t li2_cfa_noise_filter;
    ik_li2_demosaic_t li2_demosaic;
    //ASF
    ik_hi_asf_t hi_asf;
    ik_li2_asf_t li2_asf;
    ik_hi_low_asf_t hi_low_asf;
    ik_hi_med1_asf_t hi_med1_asf;
    ik_hi_med2_asf_t hi_med2_asf;
    ik_hi_high_asf_t hi_high_asf;
    ik_hi_high2_asf_t hi_high2_asf;
    ik_hi_chroma_asf_t hi_chroma_asf;
    //SHP
    ik_hi_high_shpns_both_t hi_high_shpns_both;
    ik_hi_high_shpns_noise_t hi_high_shpns_noise;
    ik_hi_high_shpns_coring_t hi_high_shpns_coring;
    ik_hi_high_shpns_fir_t hi_high_shpns_fir;
    ik_hi_high_shpns_cor_idx_scl_t hi_high_shpns_cor_idx_scl;
    ik_hi_high_shpns_min_cor_rst_t hi_high_shpns_min_cor_rst;
    ik_hi_high_shpns_max_cor_rst_t hi_high_shpns_max_cor_rst;
    ik_hi_high_shpns_scl_cor_t hi_high_shpns_scl_cor;
    //Med
    ik_hi_med_shpns_both_t hi_med_shpns_both;
    ik_hi_med_shpns_noise_t hi_med_shpns_noise;
    ik_hi_med_shpns_coring_t hi_med_shpns_coring;
    ik_hi_med_shpns_fir_t hi_med_shpns_fir;
    ik_hi_med_shpns_cor_idx_scl_t hi_med_shpns_cor_idx_scl;
    ik_hi_med_shpns_min_cor_rst_t hi_med_shpns_min_cor_rst;
    ik_hi_med_shpns_max_cor_rst_t hi_med_shpns_max_cor_rst;
    ik_hi_med_shpns_scl_cor_t hi_med_shpns_scl_cor;
    //li2_shp
    ik_li2_shpns_both_t li2_shpns_both;
    ik_li2_shpns_noise_t li2_shpns_noise;
    ik_li2_shpns_coring_t li2_shpns_coring;
    ik_li2_shpns_fir_t li2_shpns_fir;
    ik_li2_shpns_cor_idx_scl_t li2_shpns_cor_idx_scl;
    ik_li2_shpns_min_cor_rst_t li2_shpns_min_cor_rst;
    ik_li2_shpns_max_cor_rst_t li2_shpns_max_cor_rst;
    ik_li2_shpns_scl_cor_t li2_shpns_scl_cor;
    //hili
    ik_hili_shpns_both_t hili_shpns_both;
    ik_hili_shpns_noise_t hili_shpns_noise;
    ik_hili_shpns_coring_t hili_shpns_coring;
    ik_hili_shpns_fir_t hili_shpns_fir;
    ik_hili_shpns_cor_idx_scl_t hili_shpns_cor_idx_scl;
    ik_hili_shpns_min_cor_rst_t hili_shpns_min_cor_rst;
    ik_hili_shpns_max_cor_rst_t hili_shpns_max_cor_rst;
    ik_hili_shpns_scl_cor_t hili_shpns_scl_cor;
    //chroma_filter
    ik_hi_chroma_filter_high_t hi_chroma_filter_high;
    ik_hi_chroma_filter_pre_t hi_chroma_filter_pre;
    ik_hi_chroma_filter_med_t hi_chroma_filter_med;
    ik_hi_chroma_filter_low_t hi_chroma_filter_low;
    ik_hi_chroma_filter_very_low_t hi_chroma_filter_very_low;
    //combine
    ik_hi_luma_combine_t hi_luma_combine;
    ik_hi_low_asf_combine_t hi_low_asf_combine;
    ik_hi_chroma_fltr_med_com_t hi_chroma_fltr_med_com;
    ik_hi_chroma_fltr_low_com_t hi_chroma_fltr_low_com;
    ik_hi_chroma_fltr_very_low_com_t hi_chroma_fltr_very_low_com;
    ik_hili_combine_t hili_combine;
    ik_hi_mid_high_freq_recover_t hi_mid_high_freq_recover;
    ik_hi_luma_blend_t hi_luma_blend;
    ik_hi_nonsmooth_detect_t hi_nonsmooth_detect;
    ik_hi_select_t hi_select;
    //li
    ik_chroma_asf_t chroma_asf;
} amba_ik_hiso_input_parameters_t;

typedef struct {
    // filter_id_list_t used_filters;
    amba_ik_filter_update_flags_t update_flags;
    amba_ik_input_parameters_t input_param;
    amba_ik_hiso_input_parameters_t *p_hiso_input_param;
} amba_ik_filter_t;

typedef struct {
    /*11.1 Conversions shall not be performed between a pointer to a function and any type other than an integral type.*/
    intptr iso_configs_and_states[MAX_CONFIG_NUM];
    intptr iso_tables[MAX_CONFIG_NUM];
//    intptr hdr_ce_config_and_state[MAX_CONFIG_NUM];
//    intptr hdr_ce_tables[MAX_CONFIG_NUM];
    intptr configs_container[MAX_CONFIG_NUM];
} amba_ik_configs_entity_t;

typedef struct {
    amba_ik_context_organization_t organization;
    uint8 reserved_context_organization_t[(CTX_PAD_ALIGN_SIZE)-((sizeof(amba_ik_context_organization_t))%(CTX_PAD_ALIGN_SIZE))];
    amba_ik_filter_t filters;
    uint8 reserved_filter_t[(CTX_PAD_ALIGN_SIZE)-((sizeof(amba_ik_filter_t))%(CTX_PAD_ALIGN_SIZE))];
    amba_ik_configs_entity_t configs;
    uint8 reserved_configs_entity_t[(CTX_PAD_ALIGN_SIZE)-((sizeof(amba_ik_configs_entity_t))%(CTX_PAD_ALIGN_SIZE))];
} amba_ik_context_entity_t;
#endif
