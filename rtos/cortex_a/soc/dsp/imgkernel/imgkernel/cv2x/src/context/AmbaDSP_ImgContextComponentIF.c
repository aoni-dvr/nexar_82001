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

#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgArchComponentIF.h"
#include "AmbaDSP_ImgArchSystemAPI.h"

static amba_ik_context_entity_t *context_entity[MAX_CONTEXT_NUM] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static void img_ctx_hook_ivd_filter_method(amba_ik_filter_method_t *p_ivd_filter_method)
{
    /*sensor information*/
    p_ivd_filter_method->set_vin_sensor_info = img_ctx_ivd_set_vin_sensor_info;
    p_ivd_filter_method->get_vin_sensor_info = img_ctx_ivd_get_vin_sensor_info;

    /*3a statistic*/
    p_ivd_filter_method->set_aaa_stat_info =  img_ctx_ivd_set_aaa_stat_info;
    p_ivd_filter_method->get_aaa_stat_info =  img_ctx_ivd_get_aaa_stat_info;

    p_ivd_filter_method->set_af_stat_ex_info =  img_ctx_ivd_set_af_stat_ex_info;
    p_ivd_filter_method->get_af_stat_ex_info =  img_ctx_ivd_get_af_stat_ex_info;

    p_ivd_filter_method->set_pg_af_stat_ex_info =  img_ctx_ivd_set_pg_af_stat_ex_info;
    p_ivd_filter_method->get_pg_af_stat_ex_info =  img_ctx_ivd_get_pg_af_stat_ex_info;

    p_ivd_filter_method->set_aaa_stat_float_tile_info =  img_ctx_ivd_set_aaa_stat_float_tile_info;
    p_ivd_filter_method->get_aaa_stat_float_tile_info =  img_ctx_ivd_get_aaa_stat_float_tile_info;

    /*calibration*/
    p_ivd_filter_method->set_window_size_info =  img_ctx_ivd_set_window_size_info;
    p_ivd_filter_method->get_window_size_info =  img_ctx_ivd_get_window_size_info;

    p_ivd_filter_method->set_cfa_window_size_info =  img_ctx_ivd_set_cfa_window_size_info;
    p_ivd_filter_method->get_cfa_window_size_info =  img_ctx_ivd_get_cfa_window_size_info;

    p_ivd_filter_method->set_static_bpc =  img_ctx_ivd_set_static_bpc;
    p_ivd_filter_method->get_static_bpc =  img_ctx_ivd_get_static_bpc;
    p_ivd_filter_method->set_static_bpc_internal =  img_ctx_ivd_set_static_bpc_internal;
    p_ivd_filter_method->get_static_bpc_internal =  img_ctx_ivd_get_static_bpc_internal;
    p_ivd_filter_method->set_static_bpc_enable_info = img_ctx_ivd_set_static_bpc_enable_info;
    p_ivd_filter_method->get_static_bpc_enable_info = img_ctx_ivd_get_static_bpc_enable_info;

    p_ivd_filter_method->set_vignette_compensation =  img_ctx_ivd_set_vignette;
    p_ivd_filter_method->get_vignette_compensation =  img_ctx_ivd_get_vignette;
    p_ivd_filter_method->set_vignette_enable_info = img_ctx_ivd_set_vignette_enable_info;
    p_ivd_filter_method->get_vignette_enable_info = img_ctx_ivd_get_vignette_enable_info;

    p_ivd_filter_method->set_calib_warp_info =  img_ctx_ivd_set_calib_warp_info;
    p_ivd_filter_method->get_calib_warp_info =  img_ctx_ivd_get_calib_warp_info;

    p_ivd_filter_method->set_calib_ca_warp_info =  img_ctx_ivd_set_calib_ca_warp_info;
    p_ivd_filter_method->get_calib_ca_warp_info =  img_ctx_ivd_get_calib_ca_warp_info;

    p_ivd_filter_method->set_dzoom_info =  img_ctx_ivd_set_dzoom_info;
    p_ivd_filter_method->get_dzoom_info =  img_ctx_ivd_get_dzoom_info;

    p_ivd_filter_method->set_warp_enable_info =  img_ctx_ivd_set_warp_enable_info;
    p_ivd_filter_method->get_warp_enable_info =  img_ctx_ivd_get_warp_enable_info;

    p_ivd_filter_method->set_cawarp_enable_info =  img_ctx_ivd_set_cawarp_enable_info;
    p_ivd_filter_method->get_cawarp_enable_info =  img_ctx_ivd_get_cawarp_enable_info;

    p_ivd_filter_method->set_dmy_range_info =  img_ctx_ivd_set_dummy_win_margin_range_info;
    p_ivd_filter_method->get_dmy_range_info =  img_ctx_ivd_get_dummy_win_margin_range_info;

    p_ivd_filter_method->set_warp_internal =  img_ctx_ivd_set_warp_internal;
    p_ivd_filter_method->get_warp_internal =  img_ctx_ivd_get_warp_internal;

    p_ivd_filter_method->set_ca_warp_internal =  img_ctx_ivd_set_cawarp_internal;
    p_ivd_filter_method->get_ca_warp_internal =  img_ctx_ivd_get_cawarp_internal;

    p_ivd_filter_method->set_active_win =  img_ctx_ivd_set_vin_active_window;
    p_ivd_filter_method->get_active_win =  img_ctx_ivd_get_vin_active_window;

    /*normal filters*/
    p_ivd_filter_method->set_before_ce_wb_gain =  img_ctx_ivd_set_before_ce_wb_gain;
    p_ivd_filter_method->get_before_ce_wb_gain =  img_ctx_ivd_get_before_ce_wb_gain;

    p_ivd_filter_method->set_after_ce_wb_gain =  img_ctx_ivd_set_after_ce_wb_gain;
    p_ivd_filter_method->get_after_ce_wb_gain =  img_ctx_ivd_get_after_ce_wb_gain;

    p_ivd_filter_method->set_ae_gain =  img_ctx_ivd_set_ae_gain;
    p_ivd_filter_method->get_ae_gain =  img_ctx_ivd_get_ae_gain;

    p_ivd_filter_method->set_global_dgain =  img_ctx_ivd_set_global_dgain;
    p_ivd_filter_method->get_global_dgain =  img_ctx_ivd_get_global_dgain;

    p_ivd_filter_method->set_cfa_leakage_filter =  img_ctx_ivd_set_cfa_leakage_filter;
    p_ivd_filter_method->get_cfa_leakage_filter =  img_ctx_ivd_get_cfa_leakage_filter;

    p_ivd_filter_method->set_anti_aliasing =  img_ctx_ivd_set_anti_aliasing;
    p_ivd_filter_method->get_anti_aliasing =  img_ctx_ivd_get_anti_aliasing;

    p_ivd_filter_method->set_dynamic_bpc =  img_ctx_ivd_set_dynamic_bpc;
    p_ivd_filter_method->get_dynamic_bpc =  img_ctx_ivd_get_dynamic_bpc;

    p_ivd_filter_method->set_grgb_mismatch =  img_ctx_ivd_set_grgb_mismatch;
    p_ivd_filter_method->get_grgb_mismatch =  img_ctx_ivd_get_grgb_mismatch;

    p_ivd_filter_method->set_cfa_noise_filter =  img_ctx_ivd_set_cfa_noise_filter;
    p_ivd_filter_method->get_cfa_noise_filter =  img_ctx_ivd_get_cfa_noise_filter;

    p_ivd_filter_method->set_dgain_sat_lvl =  img_ctx_ivd_set_dgain_sat_lvl;
    p_ivd_filter_method->get_dgain_sat_lvl =  img_ctx_ivd_get_dgain_sat_lvl;

    p_ivd_filter_method->set_demosaic =  img_ctx_ivd_set_demosaic;
    p_ivd_filter_method->get_demosaic =  img_ctx_ivd_get_demosaic;

    p_ivd_filter_method->set_pre_cc_gain = img_ctx_ivd_set_pre_cc_gain;
    p_ivd_filter_method->get_pre_cc_gain = img_ctx_ivd_get_pre_cc_gain;

    p_ivd_filter_method->set_color_correction_reg =  img_ctx_ivd_set_cc_reg;
    p_ivd_filter_method->get_color_correction_reg =  img_ctx_ivd_get_cc_reg;

    p_ivd_filter_method->set_color_correction =  img_ctx_ivd_set_color_correction;
    p_ivd_filter_method->get_color_correction =  img_ctx_ivd_get_color_correction;

    p_ivd_filter_method->set_tone_curve =  img_ctx_ivd_set_tone_curve;
    p_ivd_filter_method->get_tone_curve =  img_ctx_ivd_get_tone_curve;

    p_ivd_filter_method->set_rgb_to_yuv_matrix =  img_ctx_ivd_set_rgb_to_yuv_matrix;
    p_ivd_filter_method->get_rgb_to_yuv_matrix =  img_ctx_ivd_get_rgb_to_yuv_matrix;

    p_ivd_filter_method->set_rgb_ir = img_ctx_ivd_set_rgb_ir;
    p_ivd_filter_method->get_rgb_ir = img_ctx_ivd_get_rgb_ir;

    p_ivd_filter_method->set_chroma_scale =  img_ctx_ivd_set_chroma_scale;
    p_ivd_filter_method->get_chroma_scale =  img_ctx_ivd_get_chroma_scale;

    p_ivd_filter_method->set_chroma_median_filter =  img_ctx_ivd_set_chroma_median_filter;
    p_ivd_filter_method->get_chroma_median_filter =  img_ctx_ivd_get_chroma_median_filter;

    p_ivd_filter_method->set_first_luma_processing_mode =  img_ctx_ivd_set_first_luma_processing_mode;
    p_ivd_filter_method->get_first_luma_processing_mode =  img_ctx_ivd_get_first_luma_processing_mode;

    p_ivd_filter_method->set_advance_spatial_filter =  img_ctx_ivd_set_advance_spatial_filter;
    p_ivd_filter_method->get_advance_spatial_filter =  img_ctx_ivd_get_advance_spatial_filter;

    p_ivd_filter_method->set_fstshpns_both =  img_ctx_ivd_set_fstshpns_both;
    p_ivd_filter_method->get_fstshpns_both =  img_ctx_ivd_get_fstshpns_both;

    p_ivd_filter_method->set_fstshpns_noise =  img_ctx_ivd_set_fstshpns_noise;
    p_ivd_filter_method->get_fstshpns_noise =  img_ctx_ivd_get_fstshpns_noise;

    p_ivd_filter_method->set_fstshpns_fir =  img_ctx_ivd_set_fstshpns_fir;
    p_ivd_filter_method->get_fstshpns_fir =  img_ctx_ivd_get_fstshpns_fir;

    p_ivd_filter_method->set_fstshpns_coring =  img_ctx_ivd_set_fstshpns_coring;
    p_ivd_filter_method->get_fstshpns_coring =  img_ctx_ivd_get_fstshpns_coring;

    p_ivd_filter_method->set_fstshpns_coring_index_scale =  img_ctx_ivd_set_fstshpns_cor_idx_scl;
    p_ivd_filter_method->get_fstshpns_coring_index_scale =  img_ctx_ivd_get_fstshpns_cor_idx_scl;

    p_ivd_filter_method->set_fstshpns_min_coring_result =  img_ctx_ivd_set_fstshpns_min_coring_result;
    p_ivd_filter_method->get_fstshpns_min_coring_result =  img_ctx_ivd_get_fstshpns_min_coring_result;

    p_ivd_filter_method->set_fstshpns_max_coring_result =  img_ctx_ivd_set_fstshpns_max_coring_result;
    p_ivd_filter_method->get_fstshpns_max_coring_result =  img_ctx_ivd_get_fstshpns_max_coring_result;

    p_ivd_filter_method->set_fstshpns_scale_coring =  img_ctx_ivd_set_fstshpns_scale_coring;
    p_ivd_filter_method->get_fstshpns_scale_coring =  img_ctx_ivd_get_fstshpns_scale_coring;

    p_ivd_filter_method->set_fnlshpns_both =  img_ctx_ivd_set_fnlshpns_both;
    p_ivd_filter_method->get_fnlshpns_both =  img_ctx_ivd_get_fnlshpns_both;

    p_ivd_filter_method->set_fnlshpns_noise =  img_ctx_ivd_set_fnlshpns_noise;
    p_ivd_filter_method->get_fnlshpns_noise =  img_ctx_ivd_get_fnlshpns_noise;

    p_ivd_filter_method->set_fnlshpns_fir =  img_ctx_ivd_set_fnlshpns_fir;
    p_ivd_filter_method->get_fnlshpns_fir =  img_ctx_ivd_get_fnlshpns_fir;

    p_ivd_filter_method->set_fnlshpns_coring =  img_ctx_ivd_set_fnlshpns_coring;
    p_ivd_filter_method->get_fnlshpns_coring =  img_ctx_ivd_get_fnlshpns_coring;

    p_ivd_filter_method->set_fnlshpns_coring_index_scale =  img_ctx_ivd_set_fnlshpns_cor_idx_scl;
    p_ivd_filter_method->get_fnlshpns_coring_index_scale =  img_ctx_ivd_get_fnlshpns_cor_idx_scl;

    p_ivd_filter_method->set_fnlshpns_min_coring_result =  img_ctx_ivd_set_fnlshpns_min_coring_result;
    p_ivd_filter_method->get_fnlshpns_min_coring_result =  img_ctx_ivd_get_fnlshpns_min_coring_result;

    p_ivd_filter_method->set_fnlshpns_max_coring_result =  img_ctx_ivd_set_fnlshpns_max_coring_result;
    p_ivd_filter_method->get_fnlshpns_max_coring_result =  img_ctx_ivd_get_fnlshpns_max_coring_result;

    p_ivd_filter_method->set_fnlshpns_scale_coring =  img_ctx_ivd_set_fnlshpns_scale_coring;
    p_ivd_filter_method->get_fnlshpns_scale_coring =  img_ctx_ivd_get_fnlshpns_scale_coring;

    p_ivd_filter_method->set_fnlshpns_both_tdt =  img_ctx_ivd_set_fnlshpns_both_tdt;
    p_ivd_filter_method->get_fnlshpns_both_tdt =  img_ctx_ivd_get_fnlshpns_both_tdt;

    p_ivd_filter_method->set_chroma_filter =  img_ctx_ivd_set_chroma_filter;
    p_ivd_filter_method->get_chroma_filter =  img_ctx_ivd_get_chroma_filter;

    p_ivd_filter_method->set_video_mctf =  img_ctx_ivd_set_video_mctf;
    p_ivd_filter_method->get_video_mctf =  img_ctx_ivd_get_video_mctf;

    p_ivd_filter_method->set_video_mctf_ta =  img_ctx_ivd_set_mctf_ta;
    p_ivd_filter_method->get_video_mctf_ta =  img_ctx_ivd_get_mctf_ta;

    p_ivd_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_ivd_set_mctf_and_final_sharpen;
    p_ivd_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_ivd_get_mctf_and_final_sharpen;

    p_ivd_filter_method->set_motion_detect = img_ctx_ivd_set_motion_detect;
    p_ivd_filter_method->get_motion_detect = img_ctx_ivd_get_motion_detect;

    p_ivd_filter_method->set_motion_detect_pos_dep = img_ctx_ivd_set_motion_detect_pos_dep;
    p_ivd_filter_method->get_motion_detect_pos_dep = img_ctx_ivd_get_motion_detect_pos_dep;

    p_ivd_filter_method->set_motion_detect_and_mctf = img_ctx_ivd_set_motion_detect_and_mctf;
    p_ivd_filter_method->get_motion_detect_and_mctf = img_ctx_ivd_get_motion_detect_and_mctf;

    p_ivd_filter_method->set_mctf_internal = img_ctx_ivd_set_mctf_internal;
    p_ivd_filter_method->get_mctf_internal = img_ctx_ivd_get_mctf_internal;

    p_ivd_filter_method->set_front_end_tone_curve = img_ctx_ivd_set_fe_tone_curve;
    p_ivd_filter_method->get_front_end_tone_curve = img_ctx_ivd_get_fe_tone_curve;

    /*hdr*/
    p_ivd_filter_method->set_hdr_raw_offset = img_ctx_ivd_set_hdr_raw_offset;
    p_ivd_filter_method->get_hdr_raw_offset = img_ctx_ivd_get_hdr_raw_offset;

    p_ivd_filter_method->set_exp0_fe_static_blc =  img_ctx_ivd_set_exp0_fe_static_blc;
    p_ivd_filter_method->get_exp0_fe_static_blc =  img_ctx_ivd_get_exp0_fe_static_blc;

    p_ivd_filter_method->set_exp1_fe_static_blc =  img_ctx_ivd_set_exp1_fe_static_blc;
    p_ivd_filter_method->get_exp1_fe_static_blc =  img_ctx_ivd_get_exp1_fe_static_blc;

    p_ivd_filter_method->set_exp2_fe_static_blc =  img_ctx_ivd_set_exp2_fe_static_blc;
    p_ivd_filter_method->get_exp2_fe_static_blc =  img_ctx_ivd_get_exp2_fe_static_blc;

    p_ivd_filter_method->set_exp0_fe_wb_gain =  img_ctx_ivd_set_exp0_fe_wb_gain;
    p_ivd_filter_method->get_exp0_fe_wb_gain =  img_ctx_ivd_get_exp0_fe_wb_gain;

    p_ivd_filter_method->set_exp1_fe_wb_gain =  img_ctx_ivd_set_exp1_fe_wb_gain;
    p_ivd_filter_method->get_exp1_fe_wb_gain =  img_ctx_ivd_get_exp1_fe_wb_gain;

    p_ivd_filter_method->set_exp2_fe_wb_gain =  img_ctx_ivd_set_exp2_fe_wb_gain;
    p_ivd_filter_method->get_exp2_fe_wb_gain =  img_ctx_ivd_get_exp2_fe_wb_gain;

    p_ivd_filter_method->set_exp0_fe_dgain_sat_lvl =  img_ctx_ivd_set_exp0_fe_dgain_sat_lvl;
    p_ivd_filter_method->get_exp0_fe_dgain_sat_lvl =  img_ctx_ivd_get_exp0_fe_dgain_sat_lvl;

    p_ivd_filter_method->set_exp1_fe_dgain_sat_lvl =  img_ctx_ivd_set_exp1_fe_dgain_sat_lvl;
    p_ivd_filter_method->get_exp1_fe_dgain_sat_lvl =  img_ctx_ivd_get_exp1_fe_dgain_sat_lvl;

    p_ivd_filter_method->set_exp2_fe_dgain_sat_lvl =  img_ctx_ivd_set_exp2_fe_dgain_sat_lvl;
    p_ivd_filter_method->get_exp2_fe_dgain_sat_lvl =  img_ctx_ivd_get_exp2_fe_dgain_sat_lvl;

    p_ivd_filter_method->set_ce =  img_ctx_ivd_set_ce;
    p_ivd_filter_method->get_ce =  img_ctx_ivd_get_ce;

    p_ivd_filter_method->set_ce_input_table =  img_ctx_ivd_set_ce_input_table;
    p_ivd_filter_method->get_ce_input_table =  img_ctx_ivd_get_ce_input_table;

    p_ivd_filter_method->set_ce_out_table =  img_ctx_ivd_set_ce_out_table;
    p_ivd_filter_method->get_ce_out_table =  img_ctx_ivd_get_ce_out_table;

    p_ivd_filter_method->set_ce_ext_hds = img_ctx_ivd_set_ce_ext_hds;
    p_ivd_filter_method->get_ce_ext_hds = img_ctx_ivd_get_ce_ext_hds;

    p_ivd_filter_method->set_hdr_blend =  img_ctx_ivd_set_hdr_blend;
    p_ivd_filter_method->get_hdr_blend =  img_ctx_ivd_get_hdr_blend;

    p_ivd_filter_method->set_resampler_str =  img_ctx_ivd_set_resampler_str;
    p_ivd_filter_method->get_resampler_str =  img_ctx_ivd_get_resampler_str;

    p_ivd_filter_method->set_histogram_info = img_ctx_ivd_set_histogram_info;
    p_ivd_filter_method->get_histogram_info = img_ctx_ivd_get_histogram_info;

    p_ivd_filter_method->set_pg_histogram_info = img_ctx_ivd_set_pg_histogram_info;
    p_ivd_filter_method->get_pg_histogram_info = img_ctx_ivd_get_pg_histogram_info;

    p_ivd_filter_method->set_pseudo_y_info = img_ctx_ivd_set_pseudo_y_info;
    p_ivd_filter_method->get_pseudo_y_info = img_ctx_ivd_get_pseudo_y_info;

    p_ivd_filter_method->set_flip_mode = img_ctx_ivd_set_flip_mode;
    p_ivd_filter_method->get_flip_mode = img_ctx_ivd_get_flip_mode;

    p_ivd_filter_method->set_y2y_use_cc_enable_info =  img_ctx_ivd_set_y2y_use_cc_enable_info;
    p_ivd_filter_method->get_y2y_use_cc_enable_info =  img_ctx_ivd_get_y2y_use_cc_enable_info;

    // hiso
    p_ivd_filter_method->set_hi_anti_aliasing = img_ctx_ivd_set_hi_anti_aliasing;
    p_ivd_filter_method->get_hi_anti_aliasing = img_ctx_ivd_get_hi_anti_aliasing;

    p_ivd_filter_method->set_hi_cfa_leakage_filter = img_ctx_ivd_set_hi_cfa_leakage_filter;
    p_ivd_filter_method->get_hi_cfa_leakage_filter = img_ctx_ivd_get_hi_cfa_leakage_filter;

    p_ivd_filter_method->set_hi_dynamic_bpc = img_ctx_ivd_set_hi_dynamic_bpc;
    p_ivd_filter_method->get_hi_dynamic_bpc = img_ctx_ivd_get_hi_dynamic_bpc;

    p_ivd_filter_method->set_hi_grgb_mismatch = img_ctx_ivd_set_hi_grgb_mismatch;
    p_ivd_filter_method->get_hi_grgb_mismatch = img_ctx_ivd_get_hi_grgb_mismatch;

    p_ivd_filter_method->set_hi_chroma_median_filter = img_ctx_ivd_set_hi_chroma_median_filter;
    p_ivd_filter_method->get_hi_chroma_median_filter = img_ctx_ivd_get_hi_chroma_median_filter;

    p_ivd_filter_method->set_hi_cfa_noise_filter = img_ctx_ivd_set_hi_cfa_noise_filter;
    p_ivd_filter_method->get_hi_cfa_noise_filter = img_ctx_ivd_get_hi_cfa_noise_filter;

    p_ivd_filter_method->set_hi_demosaic = img_ctx_ivd_set_hi_demosaic;
    p_ivd_filter_method->get_hi_demosaic = img_ctx_ivd_get_hi_demosaic;

    //hiso low2
    p_ivd_filter_method->set_li2_anti_aliasing = img_ctx_ivd_set_li2_anti_aliasing;
    p_ivd_filter_method->get_li2_anti_aliasing = img_ctx_ivd_get_li2_anti_aliasing;

    p_ivd_filter_method->set_li2_cfa_leakage_filter = img_ctx_ivd_set_li2_cfa_leakage_filter;
    p_ivd_filter_method->get_li2_cfa_leakage_filter = img_ctx_ivd_get_li2_cfa_leakage_filter;

    p_ivd_filter_method->set_li2_dynamic_bpc = img_ctx_ivd_set_li2_dynamic_bpc;
    p_ivd_filter_method->get_li2_dynamic_bpc = img_ctx_ivd_get_li2_dynamic_bpc;

    p_ivd_filter_method->set_li2_grgb_mismatch = img_ctx_ivd_set_li2_grgb_mismatch;
    p_ivd_filter_method->get_li2_grgb_mismatch = img_ctx_ivd_get_li2_grgb_mismatch;

    p_ivd_filter_method->set_li2_cfa_noise_filter = img_ctx_ivd_set_li2_cfa_noise_filter;
    p_ivd_filter_method->get_li2_cfa_noise_filter = img_ctx_ivd_get_li2_cfa_noise_filter;

    p_ivd_filter_method->set_li2_demosaic = img_ctx_ivd_set_li2_demosaic;
    p_ivd_filter_method->get_li2_demosaic = img_ctx_ivd_get_li2_demosaic;

    //asf
    p_ivd_filter_method->set_hi_asf = img_ctx_ivd_set_hi_asf;
    p_ivd_filter_method->get_hi_asf = img_ctx_ivd_get_hi_asf;

    p_ivd_filter_method->set_li2_asf = img_ctx_ivd_set_li2_asf;
    p_ivd_filter_method->get_li2_asf = img_ctx_ivd_get_li2_asf;

    p_ivd_filter_method->set_hi_low_asf = img_ctx_ivd_set_hi_low_asf;
    p_ivd_filter_method->get_hi_low_asf = img_ctx_ivd_get_hi_low_asf;

    p_ivd_filter_method->set_hi_med1_asf = img_ctx_ivd_set_hi_med1_asf;
    p_ivd_filter_method->get_hi_med1_asf = img_ctx_ivd_get_hi_med1_asf;

    p_ivd_filter_method->set_hi_med2_asf = img_ctx_ivd_set_hi_med2_asf;
    p_ivd_filter_method->get_hi_med2_asf = img_ctx_ivd_get_hi_med2_asf;

    p_ivd_filter_method->set_hi_high_asf = img_ctx_ivd_set_hi_high_asf;
    p_ivd_filter_method->get_hi_high_asf = img_ctx_ivd_get_hi_high_asf;

    p_ivd_filter_method->set_hi_high2_asf = img_ctx_ivd_set_hi_high2_asf;
    p_ivd_filter_method->get_hi_high2_asf = img_ctx_ivd_get_hi_high2_asf;

    p_ivd_filter_method->set_chroma_asf = img_ctx_ivd_set_chroma_asf;
    p_ivd_filter_method->get_chroma_asf = img_ctx_ivd_get_chroma_asf;

    p_ivd_filter_method->set_hi_chroma_asf = img_ctx_ivd_set_hi_chroma_asf;
    p_ivd_filter_method->get_hi_chroma_asf = img_ctx_ivd_get_hi_chroma_asf;

    p_ivd_filter_method->set_hi_low_chroma_asf = img_ctx_ivd_set_hi_low_chroma_asf;
    p_ivd_filter_method->get_hi_low_chroma_asf = img_ctx_ivd_get_hi_low_chroma_asf;

    // sharpen
    // --high--
    p_ivd_filter_method->set_hi_high_shpns_both = img_ctx_ivd_set_hi_high_shpns_both;
    p_ivd_filter_method->get_hi_high_shpns_both = img_ctx_ivd_get_hi_high_shpns_both;

    p_ivd_filter_method->set_hi_high_shpns_noise = img_ctx_ivd_set_hi_high_shpns_noise;
    p_ivd_filter_method->get_hi_high_shpns_noise = img_ctx_ivd_get_hi_high_shpns_noise;

    p_ivd_filter_method->set_hi_high_shpns_coring = img_ctx_ivd_set_hi_high_shpns_coring;
    p_ivd_filter_method->get_hi_high_shpns_coring = img_ctx_ivd_get_hi_high_shpns_coring;

    p_ivd_filter_method->set_hi_high_shpns_fir = img_ctx_ivd_set_hi_high_shpns_fir;
    p_ivd_filter_method->get_hi_high_shpns_fir = img_ctx_ivd_get_hi_high_shpns_fir;

    p_ivd_filter_method->set_hi_high_shpns_cor_idx_scl = img_ctx_ivd_set_hi_high_shp_cor_idx_scl;
    p_ivd_filter_method->get_hi_high_shpns_cor_idx_scl = img_ctx_ivd_get_hi_high_shp_cor_idx_scl;

    p_ivd_filter_method->set_hi_high_shpns_min_cor_rst = img_ctx_ivd_set_hi_high_shp_min_cor_rst;
    p_ivd_filter_method->get_hi_high_shpns_min_cor_rst = img_ctx_ivd_get_hi_high_shp_min_cor_rst;

    p_ivd_filter_method->set_hi_high_shpns_max_cor_rst = img_ctx_ivd_set_hi_high_shp_max_cor_rst;
    p_ivd_filter_method->get_hi_high_shpns_max_cor_rst = img_ctx_ivd_get_hi_high_shp_max_cor_rst;

    p_ivd_filter_method->set_hi_high_shpns_scl_cor = img_ctx_ivd_set_hi_high_shpns_scl_cor;
    p_ivd_filter_method->get_hi_high_shpns_scl_cor = img_ctx_ivd_get_hi_high_shpns_scl_cor;

    // --med--
    p_ivd_filter_method->set_hi_med_shpns_both = img_ctx_ivd_set_hi_med_shpns_both;
    p_ivd_filter_method->get_hi_med_shpns_both = img_ctx_ivd_get_hi_med_shpns_both;

    p_ivd_filter_method->set_hi_med_shpns_noise = img_ctx_ivd_set_hi_med_shpns_noise;
    p_ivd_filter_method->get_hi_med_shpns_noise = img_ctx_ivd_get_hi_med_shpns_noise;

    p_ivd_filter_method->set_hi_med_shpns_coring = img_ctx_ivd_set_hi_med_shpns_coring;
    p_ivd_filter_method->get_hi_med_shpns_coring = img_ctx_ivd_get_hi_med_shpns_coring;

    p_ivd_filter_method->set_hi_med_shpns_fir = img_ctx_ivd_set_hi_med_shpns_fir;
    p_ivd_filter_method->get_hi_med_shpns_fir = img_ctx_ivd_get_hi_med_shpns_fir;

    p_ivd_filter_method->set_hi_med_shpns_cor_idx_scl = img_ctx_ivd_set_hi_med_shp_cor_idx_scl;
    p_ivd_filter_method->get_hi_med_shpns_cor_idx_scl = img_ctx_ivd_get_hi_med_shp_cor_idx_scl;

    p_ivd_filter_method->set_hi_med_shpns_min_cor_rst = img_ctx_ivd_set_hi_med_shpns_min_cor_rst;
    p_ivd_filter_method->get_hi_med_shpns_min_cor_rst = img_ctx_ivd_get_hi_med_shpns_min_cor_rst;

    p_ivd_filter_method->set_hi_med_shpns_max_cor_rst = img_ctx_ivd_set_hi_med_shpns_max_cor_rst;
    p_ivd_filter_method->get_hi_med_shpns_max_cor_rst = img_ctx_ivd_get_hi_med_shpns_max_cor_rst;

    p_ivd_filter_method->set_hi_med_shpns_scl_cor = img_ctx_ivd_set_hi_med_shpns_scl_cor;
    p_ivd_filter_method->get_hi_med_shpns_scl_cor = img_ctx_ivd_get_hi_med_shpns_scl_cor;

    // --low2--
    p_ivd_filter_method->set_li2_shpns_both = img_ctx_ivd_set_li2_shpns_both;
    p_ivd_filter_method->get_li2_shpns_both = img_ctx_ivd_get_li2_shpns_both;

    p_ivd_filter_method->set_li2_shpns_noise = img_ctx_ivd_set_li2_shpns_noise;
    p_ivd_filter_method->get_li2_shpns_noise = img_ctx_ivd_get_li2_shpns_noise;

    p_ivd_filter_method->set_li2_shpns_coring = img_ctx_ivd_set_li2_shpns_coring;
    p_ivd_filter_method->get_li2_shpns_coring = img_ctx_ivd_get_li2_shpns_coring;

    p_ivd_filter_method->set_li2_shpns_fir = img_ctx_ivd_set_li2_shpns_fir;
    p_ivd_filter_method->get_li2_shpns_fir = img_ctx_ivd_get_li2_shpns_fir;

    p_ivd_filter_method->set_li2_shpns_cor_idx_scl = img_ctx_ivd_set_li2_shpns_cor_idx_scl;
    p_ivd_filter_method->get_li2_shpns_cor_idx_scl = img_ctx_ivd_get_li2_shpns_cor_idx_scl;

    p_ivd_filter_method->set_li2_shpns_min_cor_rst = img_ctx_ivd_set_li2_shpns_min_cor_rst;
    p_ivd_filter_method->get_li2_shpns_min_cor_rst = img_ctx_ivd_get_li2_shpns_min_cor_rst;

    p_ivd_filter_method->set_li2_shpns_max_cor_rst = img_ctx_ivd_set_li2_shpns_max_cor_rst;
    p_ivd_filter_method->get_li2_shpns_max_cor_rst = img_ctx_ivd_get_li2_shpns_max_cor_rst;

    p_ivd_filter_method->set_li2_shpns_scl_cor = img_ctx_ivd_set_li2_shpns_scl_cor;
    p_ivd_filter_method->get_li2_shpns_scl_cor = img_ctx_ivd_get_li2_shpns_scl_cor;

    // --hili--
    p_ivd_filter_method->set_hili_shpns_both = img_ctx_ivd_set_hili_shpns_both;
    p_ivd_filter_method->get_hili_shpns_both = img_ctx_ivd_get_hili_shpns_both;

    p_ivd_filter_method->set_hili_shpns_noise = img_ctx_ivd_set_hili_shpns_noise;
    p_ivd_filter_method->get_hili_shpns_noise = img_ctx_ivd_get_hili_shpns_noise;

    p_ivd_filter_method->set_hili_shpns_coring = img_ctx_ivd_set_hili_shpns_coring;
    p_ivd_filter_method->get_hili_shpns_coring = img_ctx_ivd_get_hili_shpns_coring;

    p_ivd_filter_method->set_hili_shpns_fir = img_ctx_ivd_set_hili_shpns_fir;
    p_ivd_filter_method->get_hili_shpns_fir = img_ctx_ivd_get_hili_shpns_fir;

    p_ivd_filter_method->set_hili_shpns_cor_idx_scl = img_ctx_ivd_set_hili_shpns_cor_idx_scl;
    p_ivd_filter_method->get_hili_shpns_cor_idx_scl = img_ctx_ivd_get_hili_shpns_cor_idx_scl;

    p_ivd_filter_method->set_hili_shpns_min_cor_rst = img_ctx_ivd_set_hili_shpns_min_cor_rst;
    p_ivd_filter_method->get_hili_shpns_min_cor_rst = img_ctx_ivd_get_hili_shpns_min_cor_rst;

    p_ivd_filter_method->set_hili_shpns_max_cor_rst = img_ctx_ivd_set_hili_shpns_max_cor_rst;
    p_ivd_filter_method->get_hili_shpns_max_cor_rst = img_ctx_ivd_get_hili_shpns_max_cor_rst;

    p_ivd_filter_method->set_hili_shpns_scl_cor = img_ctx_ivd_set_hili_shpns_scl_cor;
    p_ivd_filter_method->get_hili_shpns_scl_cor = img_ctx_ivd_get_hili_shpns_scl_cor;

    //chroma filter
    p_ivd_filter_method->set_hi_chroma_filter_high = img_ctx_ivd_set_hi_chrm_fltr_high;
    p_ivd_filter_method->get_hi_chroma_filter_high = img_ctx_ivd_get_hi_chrm_fltr_high;

    p_ivd_filter_method->set_hi_chroma_filter_pre = img_ctx_ivd_set_hi_chrm_fltr_pre;
    p_ivd_filter_method->get_hi_chroma_filter_pre = img_ctx_ivd_get_hi_chrm_fltr_pre;

    p_ivd_filter_method->set_hi_chroma_filter_med = img_ctx_ivd_set_hi_chrm_fltr_med;
    p_ivd_filter_method->get_hi_chroma_filter_med = img_ctx_ivd_get_hi_chrm_fltr_med;

    p_ivd_filter_method->set_hi_chroma_filter_low = img_ctx_ivd_set_hi_chrm_filter_low;
    p_ivd_filter_method->get_hi_chroma_filter_low = img_ctx_ivd_get_hi_chrm_filter_low;

    p_ivd_filter_method->set_hi_chroma_filter_very_low = img_ctx_ivd_set_hi_chrm_fltr_very_low;
    p_ivd_filter_method->get_hi_chroma_filter_very_low = img_ctx_ivd_get_hi_chrm_fltr_very_low;

    //combine
    p_ivd_filter_method->set_hi_luma_combine = img_ctx_ivd_set_hi_luma_combine;
    p_ivd_filter_method->get_hi_luma_combine = img_ctx_ivd_get_hi_luma_combine;

    p_ivd_filter_method->set_hi_low_asf_combine = img_ctx_ivd_set_hi_low_asf_combine;
    p_ivd_filter_method->get_hi_low_asf_combine = img_ctx_ivd_get_hi_low_asf_combine;

    p_ivd_filter_method->set_hi_chroma_fltr_med_com = img_ctx_ivd_set_hi_chroma_fltr_med_com;
    p_ivd_filter_method->get_hi_chroma_fltr_med_com = img_ctx_ivd_get_hi_chroma_fltr_med_com;

    p_ivd_filter_method->set_hi_chroma_fltr_low_com = img_ctx_ivd_set_hi_chrm_fltr_low_com;
    p_ivd_filter_method->get_hi_chroma_fltr_low_com = img_ctx_ivd_get_hi_chrm_fltr_low_com;

    p_ivd_filter_method->set_hi_chroma_fltr_very_low_com = img_ctx_ivd_set_hi_chm_fltr_very_low_com;
    p_ivd_filter_method->get_hi_chroma_fltr_very_low_com = img_ctx_ivd_get_hi_chm_fltr_very_low_com;

    p_ivd_filter_method->set_hili_combine = img_ctx_ivd_set_hili_combine;
    p_ivd_filter_method->get_hili_combine = img_ctx_ivd_get_hili_combine;

    p_ivd_filter_method->set_hi_mid_high_freq_recover = img_ctx_ivd_set_hi_mid_high_freq_recover;
    p_ivd_filter_method->get_hi_mid_high_freq_recover = img_ctx_ivd_get_hi_mid_high_freq_recover;

    p_ivd_filter_method->set_hi_mid_high_freq_recover = img_ctx_ivd_set_hi_mid_high_freq_recover;
    p_ivd_filter_method->get_hi_mid_high_freq_recover = img_ctx_ivd_get_hi_mid_high_freq_recover;

    p_ivd_filter_method->set_hi_luma_blend = img_ctx_ivd_set_hi_luma_blend;
    p_ivd_filter_method->get_hi_luma_blend = img_ctx_ivd_get_hi_luma_blend;

    p_ivd_filter_method->set_hi_nonsmooth_detect = img_ctx_ivd_set_hi_nonsmooth_detect;
    p_ivd_filter_method->get_hi_nonsmooth_detect = img_ctx_ivd_get_hi_nonsmooth_detect;

    p_ivd_filter_method->set_hi_select = img_ctx_ivd_set_hi_select;
    p_ivd_filter_method->get_hi_select = img_ctx_ivd_get_hi_select;

#if SUPPORT_FUSION
    p_ivd_filter_method->set_fusion =  img_ctx_ivd_set_fusion;
    p_ivd_filter_method->get_fusion =  img_ctx_ivd_get_fusion;

    p_ivd_filter_method->set_mono_static_bpc =  img_ctx_ivd_set_static_bpc;
    p_ivd_filter_method->get_mono_static_bpc =  img_ctx_ivd_get_static_bpc;

    p_ivd_filter_method->set_mono_vignette_compensation =  img_ctx_ivd_set_vignette;
    p_ivd_filter_method->get_mono_vignette_compensation =  img_ctx_ivd_get_vignette;

    p_ivd_filter_method->set_mono_static_bpc_enable_info =  img_ctx_ivd_set_static_bpc_enable_info;
    p_ivd_filter_method->get_mono_static_bpc_enable_info =  img_ctx_ivd_get_static_bpc_enable_info;

    p_ivd_filter_method->set_mono_vignette_enable_info =  img_ctx_ivd_set_vignette_enable_info;
    p_ivd_filter_method->get_mono_vignette_enable_info =  img_ctx_ivd_get_vignette_enable_info;

    p_ivd_filter_method->set_mono_before_ce_wb_gain =  img_ctx_ivd_set_mono_before_ce_wb_gain;
    p_ivd_filter_method->get_mono_before_ce_wb_gain =  img_ctx_ivd_get_mono_before_ce_wb_gain;

    p_ivd_filter_method->set_mono_after_ce_wb_gain =  img_ctx_ivd_set_mono_after_ce_wb_gain;
    p_ivd_filter_method->get_mono_after_ce_wb_gain =  img_ctx_ivd_get_mono_after_ce_wb_gain;

    p_ivd_filter_method->set_mono_cfa_leakage_filter =  img_ctx_ivd_set_mono_cfa_leakage_filter;
    p_ivd_filter_method->get_mono_cfa_leakage_filter =  img_ctx_ivd_get_mono_cfa_leakage_filter;

    p_ivd_filter_method->set_mono_anti_aliasing =  img_ctx_ivd_set_mono_anti_aliasing;
    p_ivd_filter_method->get_mono_anti_aliasing =  img_ctx_ivd_get_mono_anti_aliasing;

    p_ivd_filter_method->set_mono_dynamic_bpc =  img_ctx_ivd_set_mono_dynamic_bpc;
    p_ivd_filter_method->get_mono_dynamic_bpc =  img_ctx_ivd_get_mono_dynamic_bpc;

    p_ivd_filter_method->set_mono_grgb_mismatch =  img_ctx_ivd_set_mono_grgb_mismatch;
    p_ivd_filter_method->get_mono_grgb_mismatch =  img_ctx_ivd_get_mono_grgb_mismatch;

    p_ivd_filter_method->set_mono_cfa_noise_filter =  img_ctx_ivd_set_mono_cfa_noise_filter;
    p_ivd_filter_method->get_mono_cfa_noise_filter =  img_ctx_ivd_get_mono_cfa_noise_filter;

    p_ivd_filter_method->set_mono_demosaic =  img_ctx_ivd_set_mono_demosaic;
    p_ivd_filter_method->get_mono_demosaic =  img_ctx_ivd_get_mono_demosaic;

    p_ivd_filter_method->set_mono_color_correction_reg =  img_ctx_ivd_set_mono_cc_reg;
    p_ivd_filter_method->get_mono_color_correction_reg =  img_ctx_ivd_get_mono_cc_reg;

    p_ivd_filter_method->set_mono_color_correction =  img_ctx_ivd_set_mono_color_correction;
    p_ivd_filter_method->get_mono_color_correction =  img_ctx_ivd_get_mono_color_correction;

    p_ivd_filter_method->set_mono_tone_curve =  img_ctx_ivd_set_mono_tone_curve;
    p_ivd_filter_method->get_mono_tone_curve =  img_ctx_ivd_get_mono_tone_curve;

    p_ivd_filter_method->set_mono_rgb_to_yuv_matrix =  img_ctx_ivd_set_mono_rgb_to_yuv_matrix;
    p_ivd_filter_method->get_mono_rgb_to_yuv_matrix =  img_ctx_ivd_get_mono_rgb_to_yuv_matrix;

    p_ivd_filter_method->set_mono_first_luma_processing_mode =  img_ctx_ivd_set_mono_first_luma_processing_mode;
    p_ivd_filter_method->get_mono_first_luma_processing_mode =  img_ctx_ivd_get_mono_first_luma_processing_mode;

    p_ivd_filter_method->set_mono_advance_spatial_filter =  img_ctx_ivd_set_mono_advance_spatial_filter;
    p_ivd_filter_method->get_mono_advance_spatial_filter =  img_ctx_ivd_get_mono_advance_spatial_filter;

    p_ivd_filter_method->set_mono_fstshpns_both =  img_ctx_ivd_set_mono_fstshpns_both;
    p_ivd_filter_method->get_mono_fstshpns_both =  img_ctx_ivd_get_mono_fstshpns_both;

    p_ivd_filter_method->set_mono_fstshpns_noise =  img_ctx_ivd_set_mono_fstshpns_noise;
    p_ivd_filter_method->get_mono_fstshpns_noise =  img_ctx_ivd_get_mono_fstshpns_noise;

    p_ivd_filter_method->set_mono_fstshpns_fir =  img_ctx_ivd_set_mono_fstshpns_fir;
    p_ivd_filter_method->get_mono_fstshpns_fir =  img_ctx_ivd_get_mono_fstshpns_fir;

    p_ivd_filter_method->set_mono_fstshpns_coring =  img_ctx_ivd_set_mono_fstshpns_coring;
    p_ivd_filter_method->get_mono_fstshpns_coring =  img_ctx_ivd_get_mono_fstshpns_coring;

    p_ivd_filter_method->set_mono_fstshpns_coring_index_scale =  img_ctx_ivd_set_mono_fstshpns_cor_idx_scl;
    p_ivd_filter_method->get_mono_fstshpns_coring_index_scale =  img_ctx_ivd_get_mono_fstshpns_cor_idx_scl;

    p_ivd_filter_method->set_mono_fstshpns_min_coring_result =  img_ctx_ivd_set_mono_fstshpns_min_coring_result;
    p_ivd_filter_method->get_mono_fstshpns_min_coring_result =  img_ctx_ivd_get_mono_fstshpns_min_coring_result;

    p_ivd_filter_method->set_mono_fstshpns_max_coring_result =  img_ctx_ivd_set_mono_fstshpns_max_coring_result;
    p_ivd_filter_method->get_mono_fstshpns_max_coring_result =  img_ctx_ivd_get_mono_fstshpns_max_coring_result;

    p_ivd_filter_method->set_mono_fstshpns_scale_coring =  img_ctx_ivd_set_mono_fstshpns_scale_coring;
    p_ivd_filter_method->get_mono_fstshpns_scale_coring =  img_ctx_ivd_get_mono_fstshpns_scale_coring;

    p_ivd_filter_method->set_wide_chroma_filter =  img_ctx_ivd_set_wide_chroma_filter;
    p_ivd_filter_method->get_wide_chroma_filter =  img_ctx_ivd_get_wide_chroma_filter;

    p_ivd_filter_method->set_wide_chroma_combine =  img_ctx_ivd_set_wide_chroma_filter_combine;
    p_ivd_filter_method->get_wide_chroma_combine =  img_ctx_ivd_get_wide_chroma_filter_combine;

    p_ivd_filter_method->set_mono_exp0_fe_static_blc =  img_ctx_ivd_set_mono_exp0_fe_static_blc;
    p_ivd_filter_method->get_mono_exp0_fe_static_blc =  img_ctx_ivd_get_mono_exp0_fe_static_blc;

    p_ivd_filter_method->set_mono_exp0_fe_wb_gain =  img_ctx_ivd_set_mono_exp0_fe_wb_gain;
    p_ivd_filter_method->get_mono_exp0_fe_wb_gain =  img_ctx_ivd_get_mono_exp0_fe_wb_gain;

    p_ivd_filter_method->set_mono_ce =  img_ctx_ivd_set_mono_ce;
    p_ivd_filter_method->get_mono_ce =  img_ctx_ivd_get_mono_ce;

    p_ivd_filter_method->set_mono_ce_input_table =  img_ctx_ivd_set_mono_ce_input_table;
    p_ivd_filter_method->get_mono_ce_input_table =  img_ctx_ivd_get_mono_ce_input_table;

    p_ivd_filter_method->set_mono_ce_out_table =  img_ctx_ivd_set_mono_ce_out_table;
    p_ivd_filter_method->get_mono_ce_out_table =  img_ctx_ivd_get_mono_ce_out_table;
#endif
}

static void img_ctx_hook_mipi(amba_ik_filter_method_t *p_filter_method)
{
    img_ctx_hook_ivd_filter_method(p_filter_method);

    /*sensor information*/
    p_filter_method->set_window_size_info =  img_ctx_set_window_size_info;
    p_filter_method->get_window_size_info =  img_ctx_get_window_size_info;

    p_filter_method->set_cfa_window_size_info =  img_ctx_set_cfa_window_size_info;
    p_filter_method->get_cfa_window_size_info =  img_ctx_get_cfa_window_size_info;

    p_filter_method->set_vin_sensor_info = img_ctx_set_vin_sensor_info;
    p_filter_method->get_vin_sensor_info = img_ctx_get_vin_sensor_info;

    p_filter_method->set_fnlshpns_both =  img_ctx_set_fnlshpns_both;
    p_filter_method->get_fnlshpns_both =  img_ctx_get_fnlshpns_both;

    p_filter_method->set_fnlshpns_noise =  img_ctx_set_fnlshpns_noise;
    p_filter_method->get_fnlshpns_noise =  img_ctx_get_fnlshpns_noise;

    p_filter_method->set_fnlshpns_fir =  img_ctx_set_fnlshpns_fir;
    p_filter_method->get_fnlshpns_fir =  img_ctx_get_fnlshpns_fir;

    p_filter_method->set_fnlshpns_coring =  img_ctx_set_fnlshpns_coring;
    p_filter_method->get_fnlshpns_coring =  img_ctx_get_fnlshpns_coring;

    p_filter_method->set_fnlshpns_coring_index_scale =  img_ctx_set_fnlshpns_cor_idx_scl;
    p_filter_method->get_fnlshpns_coring_index_scale =  img_ctx_get_fnlshpns_cor_idx_scl;

    p_filter_method->set_fnlshpns_min_coring_result =  img_ctx_set_fnlshpns_min_coring_result;
    p_filter_method->get_fnlshpns_min_coring_result =  img_ctx_get_fnlshpns_min_coring_result;

    p_filter_method->set_fnlshpns_max_coring_result =  img_ctx_set_fnlshpns_max_coring_result;
    p_filter_method->get_fnlshpns_max_coring_result =  img_ctx_get_fnlshpns_max_coring_result;

    p_filter_method->set_fnlshpns_scale_coring =  img_ctx_set_fnlshpns_scale_coring;
    p_filter_method->get_fnlshpns_scale_coring =  img_ctx_get_fnlshpns_scale_coring;

    p_filter_method->set_fnlshpns_both_tdt =  img_ctx_set_fnlshpns_both_tdt;
    p_filter_method->get_fnlshpns_both_tdt =  img_ctx_get_fnlshpns_both_tdt;

    p_filter_method->set_video_mctf =  img_ctx_set_video_mctf;
    p_filter_method->get_video_mctf =  img_ctx_get_video_mctf;

    p_filter_method->set_video_mctf_ta =  img_ctx_set_video_mctf_ta;
    p_filter_method->get_video_mctf_ta =  img_ctx_get_video_mctf_ta;

    p_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_set_video_mctf_and_final_sharpen;
    p_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_get_video_mctf_and_final_sharpen;
}

static void img_ctx_hook_filter_method_part2(uint8 video_pipe, amba_ik_filter_method_t *p_filter_method)  // To reduce complexity
{
    /*CE*/
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_LINEAR_CE) || (video_pipe == (uint8)AMBA_IK_VIDEO_LINEAR_CE_MD) ||
        (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3) ||
        (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2_MD) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3_MD)||
        (video_pipe == (uint8)AMBA_IK_VIDEO_FUSION_CE_MD) || (video_pipe == (uint8)AMBA_IK_VIDEO_FUSION_CE)) {
        p_filter_method->set_ce =  img_ctx_set_ce;
        p_filter_method->get_ce =  img_ctx_get_ce;

        p_filter_method->set_ce_input_table =  img_ctx_set_ce_input_table;
        p_filter_method->get_ce_input_table =  img_ctx_get_ce_input_table;

        p_filter_method->set_ce_out_table =  img_ctx_set_ce_out_table;
        p_filter_method->get_ce_out_table =  img_ctx_get_ce_out_table;
    }

    p_filter_method->set_ce_ext_hds = img_ctx_set_ce_ext_hds;
    p_filter_method->get_ce_ext_hds = img_ctx_get_ce_ext_hds;

    /*hdr*/
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2_MD)
        ||(video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3_MD)
#if PRE_NN_PROCESS
        || (video_pipe == (uint8)AMBA_IKC_VIDEO_PRE_NN_PROCESS)
#endif
       ) {
        p_filter_method->set_hdr_raw_offset = img_ctx_set_hdr_raw_offset;
        p_filter_method->get_hdr_raw_offset = img_ctx_get_hdr_raw_offset;

        p_filter_method->set_exp1_fe_static_blc =  img_ctx_set_exp1_fe_static_blc;
        p_filter_method->get_exp1_fe_static_blc =  img_ctx_get_exp1_fe_static_blc;

        p_filter_method->set_exp1_fe_wb_gain =  img_ctx_set_exp1_fe_wb_gain;
        p_filter_method->get_exp1_fe_wb_gain =  img_ctx_get_exp1_fe_wb_gain;

        p_filter_method->set_exp1_fe_dgain_sat_lvl =  img_ctx_set_exp1_fe_dgain_sat_lvl;
        p_filter_method->get_exp1_fe_dgain_sat_lvl =  img_ctx_get_exp1_fe_dgain_sat_lvl;

        p_filter_method->set_hdr_blend =  img_ctx_set_hdr_blend;
        p_filter_method->get_hdr_blend =  img_ctx_get_hdr_blend;
    }
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3_MD)) {
        p_filter_method->set_exp2_fe_static_blc =  img_ctx_set_exp2_fe_static_blc;
        p_filter_method->get_exp2_fe_static_blc =  img_ctx_get_exp2_fe_static_blc;

        p_filter_method->set_exp2_fe_wb_gain =  img_ctx_set_exp2_fe_wb_gain;
        p_filter_method->get_exp2_fe_wb_gain =  img_ctx_get_exp2_fe_wb_gain;

        p_filter_method->set_exp2_fe_dgain_sat_lvl =  img_ctx_set_exp2_fe_dgain_sat_lvl;
        p_filter_method->get_exp2_fe_dgain_sat_lvl =  img_ctx_get_exp2_fe_dgain_sat_lvl;

    }
#if PRE_NN_PROCESS
    if (video_pipe == (uint8)AMBA_IK_VIDEO_PRE_NN_PROCESS) {
        p_filter_method->set_hdr_raw_offset = img_ctx_set_hdr_raw_offset;
        p_filter_method->get_hdr_raw_offset = img_ctx_get_hdr_raw_offset;

        p_filter_method->set_exp1_fe_static_blc =  img_ctx_set_exp1_fe_static_blc;
        p_filter_method->get_exp1_fe_static_blc =  img_ctx_get_exp1_fe_static_blc;

        p_filter_method->set_exp1_fe_wb_gain =  img_ctx_set_exp1_fe_wb_gain;
        p_filter_method->get_exp1_fe_wb_gain =  img_ctx_get_exp1_fe_wb_gain;

        p_filter_method->set_exp1_fe_dgain_sat_lvl =  img_ctx_set_exp1_fe_dgain_sat_lvl;
        p_filter_method->get_exp1_fe_dgain_sat_lvl =  img_ctx_get_exp1_fe_dgain_sat_lvl;

        p_filter_method->set_hdr_blend =  img_ctx_set_hdr_blend;
        p_filter_method->get_hdr_blend =  img_ctx_get_hdr_blend;
    }
#endif

#if SUPPORT_FUSION
    p_filter_method->set_fusion =  img_ctx_set_fusion;
    p_filter_method->get_fusion =  img_ctx_get_fusion;

    p_filter_method->set_mono_static_bpc =  img_ctx_set_static_bpc;
    p_filter_method->get_mono_static_bpc =  img_ctx_get_static_bpc;

    p_filter_method->set_mono_vignette_compensation =  img_ctx_set_vignette;
    p_filter_method->get_mono_vignette_compensation =  img_ctx_get_vignette;

    p_filter_method->set_mono_static_bpc_enable_info =  img_ctx_set_static_bpc_enable_info;
    p_filter_method->get_mono_static_bpc_enable_info =  img_ctx_get_static_bpc_enable_info;

    p_filter_method->set_mono_vignette_enable_info =  img_ctx_set_vignette_enable_info;
    p_filter_method->get_mono_vignette_enable_info =  img_ctx_get_vignette_enable_info;

    p_filter_method->set_mono_before_ce_wb_gain =  img_ctx_set_mono_before_ce_wb_gain;
    p_filter_method->get_mono_before_ce_wb_gain =  img_ctx_get_mono_before_ce_wb_gain;

    p_filter_method->set_mono_after_ce_wb_gain =  img_ctx_set_mono_after_ce_wb_gain;
    p_filter_method->get_mono_after_ce_wb_gain =  img_ctx_get_mono_after_ce_wb_gain;

    p_filter_method->set_mono_cfa_leakage_filter =  img_ctx_set_mono_cfa_leakage_filter;
    p_filter_method->get_mono_cfa_leakage_filter =  img_ctx_get_mono_cfa_leakage_filter;

    p_filter_method->set_mono_anti_aliasing =  img_ctx_set_mono_anti_aliasing;
    p_filter_method->get_mono_anti_aliasing =  img_ctx_get_mono_anti_aliasing;

    p_filter_method->set_mono_dynamic_bpc =  img_ctx_set_mono_dynamic_bpc;
    p_filter_method->get_mono_dynamic_bpc =  img_ctx_get_mono_dynamic_bpc;

    p_filter_method->set_mono_grgb_mismatch =  img_ctx_set_mono_grgb_mismatch;
    p_filter_method->get_mono_grgb_mismatch =  img_ctx_get_mono_grgb_mismatch;

    p_filter_method->set_mono_cfa_noise_filter =  img_ctx_set_mono_cfa_noise_filter;
    p_filter_method->get_mono_cfa_noise_filter =  img_ctx_get_mono_cfa_noise_filter;

    p_filter_method->set_mono_demosaic =  img_ctx_set_mono_demosaic;
    p_filter_method->get_mono_demosaic =  img_ctx_get_mono_demosaic;

    p_filter_method->set_mono_color_correction_reg =  img_ctx_set_mono_color_correction_reg;
    p_filter_method->get_mono_color_correction_reg =  img_ctx_get_mono_color_correction_reg;

    p_filter_method->set_mono_color_correction =  img_ctx_set_mono_color_correction;
    p_filter_method->get_mono_color_correction =  img_ctx_get_mono_color_correction;

    p_filter_method->set_mono_tone_curve =  img_ctx_set_mono_tone_curve;
    p_filter_method->get_mono_tone_curve =  img_ctx_get_mono_tone_curve;

    p_filter_method->set_mono_rgb_to_yuv_matrix =  img_ctx_set_mono_rgb_to_yuv_matrix;
    p_filter_method->get_mono_rgb_to_yuv_matrix =  img_ctx_get_mono_rgb_to_yuv_matrix;

    p_filter_method->set_mono_first_luma_processing_mode =  img_ctx_set_mono_first_luma_processing_mode;
    p_filter_method->get_mono_first_luma_processing_mode =  img_ctx_get_mono_first_luma_processing_mode;

    p_filter_method->set_mono_advance_spatial_filter =  img_ctx_set_mono_advance_spatial_filter;
    p_filter_method->get_mono_advance_spatial_filter =  img_ctx_get_mono_advance_spatial_filter;

    p_filter_method->set_mono_fstshpns_both =  img_ctx_set_mono_fstshpns_both;
    p_filter_method->get_mono_fstshpns_both =  img_ctx_get_mono_fstshpns_both;

    p_filter_method->set_mono_fstshpns_noise =  img_ctx_set_mono_fstshpns_noise;
    p_filter_method->get_mono_fstshpns_noise =  img_ctx_get_mono_fstshpns_noise;

    p_filter_method->set_mono_fstshpns_fir =  img_ctx_set_mono_fstshpns_fir;
    p_filter_method->get_mono_fstshpns_fir =  img_ctx_get_mono_fstshpns_fir;

    p_filter_method->set_mono_fstshpns_coring =  img_ctx_set_mono_fstshpns_coring;
    p_filter_method->get_mono_fstshpns_coring =  img_ctx_get_mono_fstshpns_coring;

    p_filter_method->set_mono_fstshpns_coring_index_scale =  img_ctx_set_mono_fstshpns_cor_idx_scl;
    p_filter_method->get_mono_fstshpns_coring_index_scale =  img_ctx_get_mono_fstshpns_cor_idx_scl;

    p_filter_method->set_mono_fstshpns_min_coring_result =  img_ctx_set_mono_fstshpns_min_coring_result;
    p_filter_method->get_mono_fstshpns_min_coring_result =  img_ctx_get_mono_fstshpns_min_coring_result;

    p_filter_method->set_mono_fstshpns_max_coring_result =  img_ctx_set_mono_fstshpns_max_coring_result;
    p_filter_method->get_mono_fstshpns_max_coring_result =  img_ctx_get_mono_fstshpns_max_coring_result;

    p_filter_method->set_mono_fstshpns_scale_coring =  img_ctx_set_mono_fstshpns_scale_coring;
    p_filter_method->get_mono_fstshpns_scale_coring =  img_ctx_get_mono_fstshpns_scale_coring;

    p_filter_method->set_wide_chroma_filter =  img_ctx_set_wide_chroma_filter;
    p_filter_method->get_wide_chroma_filter =  img_ctx_get_wide_chroma_filter;

    p_filter_method->set_wide_chroma_combine =  img_ctx_set_wide_chroma_filter_combine;
    p_filter_method->get_wide_chroma_combine =  img_ctx_get_wide_chroma_filter_combine;

    p_filter_method->set_mono_exp0_fe_static_blc =  img_ctx_set_mono_exp0_fe_static_blc;
    p_filter_method->get_mono_exp0_fe_static_blc =  img_ctx_get_mono_exp0_fe_static_blc;

    p_filter_method->set_mono_exp0_fe_wb_gain =  img_ctx_set_mono_exp0_fe_wb_gain;
    p_filter_method->get_mono_exp0_fe_wb_gain =  img_ctx_get_mono_exp0_fe_wb_gain;

    p_filter_method->set_mono_ce =  img_ctx_set_mono_ce;
    p_filter_method->get_mono_ce =  img_ctx_get_mono_ce;

    p_filter_method->set_mono_ce_input_table =  img_ctx_set_mono_ce_input_table;
    p_filter_method->get_mono_ce_input_table =  img_ctx_get_mono_ce_input_table;

    p_filter_method->set_mono_ce_out_table =  img_ctx_set_mono_ce_out_table;
    p_filter_method->get_mono_ce_out_table =  img_ctx_get_mono_ce_out_table;
#endif
}
static void img_ctx_hook_filter_method(uint8 video_pipe, amba_ik_filter_method_t *p_filter_method)
{

    img_ctx_hook_ivd_filter_method(p_filter_method);
    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        /*sensor information*/
        p_filter_method->set_vin_sensor_info = img_ctx_set_vin_sensor_info;
        p_filter_method->get_vin_sensor_info = img_ctx_get_vin_sensor_info;

        /*3a statistic*/
        p_filter_method->set_af_stat_ex_info =  img_ctx_set_af_stat_ex_info;
        p_filter_method->get_af_stat_ex_info =  img_ctx_get_af_stat_ex_info;

        p_filter_method->set_aaa_stat_float_tile_info =  img_ctx_set_aaa_stat_float_tile_info;
        p_filter_method->get_aaa_stat_float_tile_info =  img_ctx_get_aaa_stat_float_tile_info;

        p_filter_method->set_histogram_info =  img_ctx_set_histogram_info;
        p_filter_method->get_histogram_info =  img_ctx_get_histogram_info;

        p_filter_method->set_pseudo_y_info = img_ctx_set_pseudo_y_info;
        p_filter_method->get_pseudo_y_info = img_ctx_get_pseudo_y_info;
    }
    p_filter_method->set_resampler_str =  img_ctx_set_resampler_str;
    p_filter_method->get_resampler_str =  img_ctx_get_resampler_str;

    p_filter_method->set_aaa_stat_info =  img_ctx_set_aaa_stat_info;
    p_filter_method->get_aaa_stat_info =  img_ctx_get_aaa_stat_info;

    p_filter_method->set_pg_af_stat_ex_info =  img_ctx_set_pg_af_stat_ex_info;
    p_filter_method->get_pg_af_stat_ex_info =  img_ctx_get_pg_af_stat_ex_info;

    p_filter_method->set_pg_histogram_info =  img_ctx_set_pg_histogram_info;
    p_filter_method->get_pg_histogram_info =  img_ctx_get_pg_histogram_info;

    /*calibration*/
    p_filter_method->set_window_size_info =  img_ctx_set_window_size_info;
    p_filter_method->get_window_size_info =  img_ctx_get_window_size_info;

    p_filter_method->set_cfa_window_size_info =  img_ctx_set_cfa_window_size_info;
    p_filter_method->get_cfa_window_size_info =  img_ctx_get_cfa_window_size_info;

    p_filter_method->set_warp_enable_info =  img_ctx_set_warp_enable_info;
    p_filter_method->get_warp_enable_info =  img_ctx_get_warp_enable_info;

    p_filter_method->set_calib_warp_info =  img_ctx_set_calib_warp_info;
    p_filter_method->get_calib_warp_info =  img_ctx_get_calib_warp_info;

    p_filter_method->set_dzoom_info =  img_ctx_set_dzoom_info;
    p_filter_method->get_dzoom_info =  img_ctx_get_dzoom_info;

    p_filter_method->set_active_win =  img_ctx_set_vin_active_window;
    p_filter_method->get_active_win =  img_ctx_get_vin_active_window;

    p_filter_method->set_dmy_range_info =  img_ctx_set_dummy_win_margin_range_info;
    p_filter_method->get_dmy_range_info =  img_ctx_get_dummy_win_margin_range_info;

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        p_filter_method->set_static_bpc =  img_ctx_set_static_bpc;
        p_filter_method->get_static_bpc =  img_ctx_get_static_bpc;
        p_filter_method->set_static_bpc_internal =  img_ctx_set_static_bpc_internal;
        p_filter_method->get_static_bpc_internal =  img_ctx_get_static_bpc_internal;
        p_filter_method->set_static_bpc_enable_info = img_ctx_set_static_bpc_enable_info;
        p_filter_method->get_static_bpc_enable_info = img_ctx_get_static_bpc_enable_info;

        p_filter_method->set_vignette_compensation =  img_ctx_set_vignette;
        p_filter_method->get_vignette_compensation =  img_ctx_get_vignette;
        p_filter_method->set_vignette_enable_info = img_ctx_set_vignette_enable_info;
        p_filter_method->get_vignette_enable_info = img_ctx_get_vignette_enable_info;

        p_filter_method->set_cawarp_enable_info =  img_ctx_set_cawarp_enable_info;
        p_filter_method->get_cawarp_enable_info =  img_ctx_get_cawarp_enable_info;
        p_filter_method->set_calib_ca_warp_info =  img_ctx_set_calib_ca_warp_info;
        p_filter_method->get_calib_ca_warp_info =  img_ctx_get_calib_ca_warp_info;

        p_filter_method->set_warp_internal =  img_ctx_set_warp_internal;
        p_filter_method->get_warp_internal =  img_ctx_get_warp_internal;

        p_filter_method->set_ca_warp_internal =  img_ctx_set_cawarp_internal;
        p_filter_method->get_ca_warp_internal =  img_ctx_get_cawarp_internal;
    }

    /*normal filters*/
    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        p_filter_method->set_before_ce_wb_gain =  img_ctx_set_before_ce_wb_gain;
        p_filter_method->get_before_ce_wb_gain =  img_ctx_get_before_ce_wb_gain;

        p_filter_method->set_after_ce_wb_gain =  img_ctx_set_after_ce_wb_gain;
        p_filter_method->get_after_ce_wb_gain =  img_ctx_get_after_ce_wb_gain;

        p_filter_method->set_ae_gain =  img_ctx_set_ae_gain;
        p_filter_method->get_ae_gain =  img_ctx_get_ae_gain;

        p_filter_method->set_global_dgain =  img_ctx_set_global_dgain;
        p_filter_method->get_global_dgain =  img_ctx_get_global_dgain;

        p_filter_method->set_cfa_leakage_filter =  img_ctx_set_cfa_leakage_filter;
        p_filter_method->get_cfa_leakage_filter =  img_ctx_get_cfa_leakage_filter;

        p_filter_method->set_anti_aliasing =  img_ctx_set_anti_aliasing;
        p_filter_method->get_anti_aliasing =  img_ctx_get_anti_aliasing;

        p_filter_method->set_dynamic_bpc =  img_ctx_set_dynamic_bpc;
        p_filter_method->get_dynamic_bpc =  img_ctx_get_dynamic_bpc;

        p_filter_method->set_grgb_mismatch =  img_ctx_set_grgb_mismatch;
        p_filter_method->get_grgb_mismatch =  img_ctx_get_grgb_mismatch;

        p_filter_method->set_cfa_noise_filter =  img_ctx_set_cfa_noise_filter;
        p_filter_method->get_cfa_noise_filter =  img_ctx_get_cfa_noise_filter;

        p_filter_method->set_dgain_sat_lvl =  img_ctx_set_dgain_sat_lvl;
        p_filter_method->get_dgain_sat_lvl =  img_ctx_get_dgain_sat_lvl;

        p_filter_method->set_demosaic =  img_ctx_set_demosaic;
        p_filter_method->get_demosaic =  img_ctx_get_demosaic;
    }

    p_filter_method->set_flip_mode = img_ctx_set_flip_mode;
    p_filter_method->get_flip_mode = img_ctx_get_flip_mode;

    p_filter_method->set_pre_cc_gain = img_ctx_set_pre_cc_gain;
    p_filter_method->get_pre_cc_gain = img_ctx_get_pre_cc_gain;

    p_filter_method->set_color_correction_reg =  img_ctx_set_color_correction_reg;
    p_filter_method->get_color_correction_reg =  img_ctx_get_color_correction_reg;

    p_filter_method->set_color_correction =  img_ctx_set_color_correction;
    p_filter_method->get_color_correction =  img_ctx_get_color_correction;

    p_filter_method->set_tone_curve =  img_ctx_set_tone_curve;
    p_filter_method->get_tone_curve =  img_ctx_get_tone_curve;

    if (video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y) {
        p_filter_method->set_rgb_ir = img_ctx_set_rgb_ir;
        p_filter_method->get_rgb_ir = img_ctx_get_rgb_ir;
    }

    p_filter_method->set_rgb_to_yuv_matrix =  img_ctx_set_rgb_to_yuv_matrix;
    p_filter_method->get_rgb_to_yuv_matrix =  img_ctx_get_rgb_to_yuv_matrix;

    p_filter_method->set_chroma_scale =  img_ctx_set_chroma_scale;
    p_filter_method->get_chroma_scale =  img_ctx_get_chroma_scale;

    p_filter_method->set_chroma_median_filter =  img_ctx_set_chroma_median_filter;
    p_filter_method->get_chroma_median_filter =  img_ctx_get_chroma_median_filter;

    p_filter_method->set_first_luma_processing_mode =  img_ctx_set_first_luma_processing_mode;
    p_filter_method->get_first_luma_processing_mode =  img_ctx_get_first_luma_processing_mode;

    p_filter_method->set_advance_spatial_filter =  img_ctx_set_advance_spatial_filter;
    p_filter_method->get_advance_spatial_filter =  img_ctx_get_advance_spatial_filter;

    p_filter_method->set_fstshpns_both =  img_ctx_set_fstshpns_both;
    p_filter_method->get_fstshpns_both =  img_ctx_get_fstshpns_both;

    p_filter_method->set_fstshpns_noise =  img_ctx_set_fstshpns_noise;
    p_filter_method->get_fstshpns_noise =  img_ctx_get_fstshpns_noise;

    p_filter_method->set_fstshpns_fir =  img_ctx_set_fstshpns_fir;
    p_filter_method->get_fstshpns_fir =  img_ctx_get_fstshpns_fir;

    p_filter_method->set_fstshpns_coring =  img_ctx_set_fstshpns_coring;
    p_filter_method->get_fstshpns_coring =  img_ctx_get_fstshpns_coring;

    p_filter_method->set_fstshpns_coring_index_scale =  img_ctx_set_fstshpns_cor_idx_scl;
    p_filter_method->get_fstshpns_coring_index_scale =  img_ctx_get_fstshpns_cor_idx_scl;

    p_filter_method->set_fstshpns_min_coring_result =  img_ctx_set_fstshpns_min_coring_result;
    p_filter_method->get_fstshpns_min_coring_result =  img_ctx_get_fstshpns_min_coring_result;

    p_filter_method->set_fstshpns_max_coring_result =  img_ctx_set_fstshpns_max_coring_result;
    p_filter_method->get_fstshpns_max_coring_result =  img_ctx_get_fstshpns_max_coring_result;

    p_filter_method->set_fstshpns_scale_coring =  img_ctx_set_fstshpns_scale_coring;
    p_filter_method->get_fstshpns_scale_coring =  img_ctx_get_fstshpns_scale_coring;

    p_filter_method->set_fnlshpns_both =  img_ctx_set_fnlshpns_both;
    p_filter_method->get_fnlshpns_both =  img_ctx_get_fnlshpns_both;

    p_filter_method->set_fnlshpns_noise =  img_ctx_set_fnlshpns_noise;
    p_filter_method->get_fnlshpns_noise =  img_ctx_get_fnlshpns_noise;

    p_filter_method->set_fnlshpns_fir =  img_ctx_set_fnlshpns_fir;
    p_filter_method->get_fnlshpns_fir =  img_ctx_get_fnlshpns_fir;

    p_filter_method->set_fnlshpns_coring =  img_ctx_set_fnlshpns_coring;
    p_filter_method->get_fnlshpns_coring =  img_ctx_get_fnlshpns_coring;

    p_filter_method->set_fnlshpns_coring_index_scale =  img_ctx_set_fnlshpns_cor_idx_scl;
    p_filter_method->get_fnlshpns_coring_index_scale =  img_ctx_get_fnlshpns_cor_idx_scl;

    p_filter_method->set_fnlshpns_min_coring_result =  img_ctx_set_fnlshpns_min_coring_result;
    p_filter_method->get_fnlshpns_min_coring_result =  img_ctx_get_fnlshpns_min_coring_result;

    p_filter_method->set_fnlshpns_max_coring_result =  img_ctx_set_fnlshpns_max_coring_result;
    p_filter_method->get_fnlshpns_max_coring_result =  img_ctx_get_fnlshpns_max_coring_result;

    p_filter_method->set_fnlshpns_scale_coring =  img_ctx_set_fnlshpns_scale_coring;
    p_filter_method->get_fnlshpns_scale_coring =  img_ctx_get_fnlshpns_scale_coring;

    p_filter_method->set_fnlshpns_both_tdt =  img_ctx_set_fnlshpns_both_tdt;
    p_filter_method->get_fnlshpns_both_tdt =  img_ctx_get_fnlshpns_both_tdt;

    p_filter_method->set_chroma_filter =  img_ctx_set_chroma_filter;
    p_filter_method->get_chroma_filter =  img_ctx_get_chroma_filter;

    p_filter_method->set_video_mctf =  img_ctx_set_video_mctf;
    p_filter_method->get_video_mctf =  img_ctx_get_video_mctf;

    p_filter_method->set_video_mctf_ta =  img_ctx_set_video_mctf_ta;
    p_filter_method->get_video_mctf_ta =  img_ctx_get_video_mctf_ta;

    p_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_set_video_mctf_and_final_sharpen;
    p_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_get_video_mctf_and_final_sharpen;

    p_filter_method->set_mctf_internal = img_ctx_set_video_mctf_internal;
    p_filter_method->get_mctf_internal = img_ctx_get_video_mctf_internal;
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_Y2Y)) {
        p_filter_method->set_y2y_use_cc_enable_info =  img_ctx_set_y2y_use_cc_enable_info;
        p_filter_method->get_y2y_use_cc_enable_info =  img_ctx_get_y2y_use_cc_enable_info;
    }

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        p_filter_method->set_front_end_tone_curve = img_ctx_set_fe_tone_curve;
        p_filter_method->get_front_end_tone_curve = img_ctx_get_fe_tone_curve;

        p_filter_method->set_exp0_fe_static_blc =  img_ctx_set_exp0_fe_static_blc;
        p_filter_method->get_exp0_fe_static_blc =  img_ctx_get_exp0_fe_static_blc;

        p_filter_method->set_exp0_fe_wb_gain =  img_ctx_set_exp0_fe_wb_gain;
        p_filter_method->get_exp0_fe_wb_gain =  img_ctx_get_exp0_fe_wb_gain;

        p_filter_method->set_exp0_fe_dgain_sat_lvl =  img_ctx_set_exp0_fe_dgain_sat_lvl;
        p_filter_method->get_exp0_fe_dgain_sat_lvl =  img_ctx_get_exp0_fe_dgain_sat_lvl;
    }

    /*md*/
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_LINEAR_MD) || (video_pipe == (uint8)AMBA_IK_VIDEO_LINEAR_CE_MD) ||
        (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2_MD) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3_MD) ||
        (video_pipe == (uint8)AMBA_IK_VIDEO_Y2Y)|| (video_pipe == (uint8)AMBA_IK_VIDEO_FUSION_CE_MD)) {
        p_filter_method->set_motion_detect = img_ctx_set_motion_detect;
        p_filter_method->get_motion_detect = img_ctx_get_motion_detect;
        p_filter_method->set_motion_detect_pos_dep = img_ctx_set_motion_detect_pos_dep;
        p_filter_method->get_motion_detect_pos_dep = img_ctx_get_motion_detect_pos_dep;
        p_filter_method->set_motion_detect_and_mctf = img_ctx_set_motion_detect_and_mctf;
        p_filter_method->get_motion_detect_and_mctf = img_ctx_get_motion_detect_and_mctf;
    }
    img_ctx_hook_filter_method_part2(video_pipe, p_filter_method); //To reduce coverity complexity
}

static void img_ctx_hook_filter_method_hiso(amba_ik_filter_method_t *p_filter_method)
{

    img_ctx_hook_ivd_filter_method(p_filter_method);

    /*sensor information*/
    p_filter_method->set_vin_sensor_info = img_ctx_set_vin_sensor_info;
    p_filter_method->get_vin_sensor_info = img_ctx_get_vin_sensor_info;

    /*calibration*/
    p_filter_method->set_window_size_info =  img_ctx_set_window_size_info;
    p_filter_method->get_window_size_info =  img_ctx_get_window_size_info;

    p_filter_method->set_warp_enable_info =  img_ctx_set_warp_enable_info;
    p_filter_method->get_warp_enable_info =  img_ctx_get_warp_enable_info;

    p_filter_method->set_cfa_window_size_info =  img_ctx_set_cfa_window_size_info;
    p_filter_method->get_cfa_window_size_info =  img_ctx_get_cfa_window_size_info;

    p_filter_method->set_calib_warp_info =  img_ctx_set_calib_warp_info;
    p_filter_method->get_calib_warp_info =  img_ctx_get_calib_warp_info;

    p_filter_method->set_active_win =  img_ctx_set_vin_active_window;
    p_filter_method->get_active_win =  img_ctx_get_vin_active_window;

    {
        p_filter_method->set_static_bpc =  img_ctx_set_static_bpc;
        p_filter_method->get_static_bpc =  img_ctx_get_static_bpc;
        p_filter_method->set_static_bpc_internal =  img_ctx_set_static_bpc_internal;
        p_filter_method->get_static_bpc_internal =  img_ctx_get_static_bpc_internal;
        p_filter_method->set_static_bpc_enable_info = img_ctx_set_static_bpc_enable_info;
        p_filter_method->get_static_bpc_enable_info = img_ctx_get_static_bpc_enable_info;

        p_filter_method->set_vignette_compensation =  img_ctx_set_vignette;
        p_filter_method->get_vignette_compensation =  img_ctx_get_vignette;
        p_filter_method->set_vignette_enable_info = img_ctx_set_vignette_enable_info;
        p_filter_method->get_vignette_enable_info = img_ctx_get_vignette_enable_info;

        p_filter_method->set_cawarp_enable_info =  img_ctx_set_cawarp_enable_info;
        p_filter_method->get_cawarp_enable_info =  img_ctx_get_cawarp_enable_info;
        p_filter_method->set_calib_ca_warp_info =  img_ctx_set_calib_ca_warp_info;
        p_filter_method->get_calib_ca_warp_info =  img_ctx_get_calib_ca_warp_info;

        p_filter_method->set_dmy_range_info =  img_ctx_set_dummy_win_margin_range_info;
        p_filter_method->get_dmy_range_info =  img_ctx_get_dummy_win_margin_range_info;

        p_filter_method->set_warp_internal =  img_ctx_set_warp_internal;
        p_filter_method->get_warp_internal =  img_ctx_get_warp_internal;

        p_filter_method->set_ca_warp_internal =  img_ctx_set_cawarp_internal;
        p_filter_method->get_ca_warp_internal =  img_ctx_get_cawarp_internal;

        p_filter_method->set_dzoom_info =  img_ctx_set_dzoom_info;
        p_filter_method->get_dzoom_info =  img_ctx_get_dzoom_info;
    }

    /*normal filters*/
    {
        p_filter_method->set_before_ce_wb_gain =  img_ctx_set_before_ce_wb_gain;
        p_filter_method->get_before_ce_wb_gain =  img_ctx_get_before_ce_wb_gain;

        p_filter_method->set_after_ce_wb_gain =  img_ctx_set_after_ce_wb_gain;
        p_filter_method->get_after_ce_wb_gain =  img_ctx_get_after_ce_wb_gain;

        p_filter_method->set_ae_gain =  img_ctx_set_ae_gain;
        p_filter_method->get_ae_gain =  img_ctx_get_ae_gain;

        p_filter_method->set_global_dgain =  img_ctx_set_global_dgain;
        p_filter_method->get_global_dgain =  img_ctx_get_global_dgain;

        p_filter_method->set_cfa_leakage_filter =  img_ctx_set_cfa_leakage_filter;
        p_filter_method->get_cfa_leakage_filter =  img_ctx_get_cfa_leakage_filter;

        p_filter_method->set_anti_aliasing =  img_ctx_set_anti_aliasing;
        p_filter_method->get_anti_aliasing =  img_ctx_get_anti_aliasing;

        p_filter_method->set_dynamic_bpc =  img_ctx_set_dynamic_bpc;
        p_filter_method->get_dynamic_bpc =  img_ctx_get_dynamic_bpc;

        p_filter_method->set_grgb_mismatch =  img_ctx_set_grgb_mismatch;
        p_filter_method->get_grgb_mismatch =  img_ctx_get_grgb_mismatch;

        p_filter_method->set_cfa_noise_filter =  img_ctx_set_cfa_noise_filter;
        p_filter_method->get_cfa_noise_filter =  img_ctx_get_cfa_noise_filter;

        p_filter_method->set_dgain_sat_lvl =  img_ctx_set_dgain_sat_lvl;
        p_filter_method->get_dgain_sat_lvl =  img_ctx_get_dgain_sat_lvl;

        p_filter_method->set_demosaic =  img_ctx_set_demosaic;
        p_filter_method->get_demosaic =  img_ctx_get_demosaic;
    }

    p_filter_method->set_flip_mode = img_ctx_set_flip_mode;
    p_filter_method->get_flip_mode = img_ctx_get_flip_mode;

    p_filter_method->set_pre_cc_gain = img_ctx_set_pre_cc_gain;
    p_filter_method->get_pre_cc_gain = img_ctx_get_pre_cc_gain;

    p_filter_method->set_color_correction_reg =  img_ctx_set_color_correction_reg;
    p_filter_method->get_color_correction_reg =  img_ctx_get_color_correction_reg;

    p_filter_method->set_color_correction =  img_ctx_set_color_correction;
    p_filter_method->get_color_correction =  img_ctx_get_color_correction;

    p_filter_method->set_tone_curve =  img_ctx_set_tone_curve;
    p_filter_method->get_tone_curve =  img_ctx_get_tone_curve;

    p_filter_method->set_rgb_to_yuv_matrix =  img_ctx_set_rgb_to_yuv_matrix;
    p_filter_method->get_rgb_to_yuv_matrix =  img_ctx_get_rgb_to_yuv_matrix;

    p_filter_method->set_chroma_scale =  img_ctx_set_chroma_scale;
    p_filter_method->get_chroma_scale =  img_ctx_get_chroma_scale;

    p_filter_method->set_chroma_median_filter =  img_ctx_set_chroma_median_filter;
    p_filter_method->get_chroma_median_filter =  img_ctx_get_chroma_median_filter;

    p_filter_method->set_first_luma_processing_mode =  img_ctx_set_first_luma_processing_mode;
    p_filter_method->get_first_luma_processing_mode =  img_ctx_get_first_luma_processing_mode;

    p_filter_method->set_advance_spatial_filter =  img_ctx_set_advance_spatial_filter;
    p_filter_method->get_advance_spatial_filter =  img_ctx_get_advance_spatial_filter;

    p_filter_method->set_fstshpns_both =  img_ctx_set_fstshpns_both;
    p_filter_method->get_fstshpns_both =  img_ctx_get_fstshpns_both;

    p_filter_method->set_fstshpns_noise =  img_ctx_set_fstshpns_noise;
    p_filter_method->get_fstshpns_noise =  img_ctx_get_fstshpns_noise;

    p_filter_method->set_fstshpns_fir =  img_ctx_set_fstshpns_fir;
    p_filter_method->get_fstshpns_fir =  img_ctx_get_fstshpns_fir;

    p_filter_method->set_fstshpns_coring =  img_ctx_set_fstshpns_coring;
    p_filter_method->get_fstshpns_coring =  img_ctx_get_fstshpns_coring;

    p_filter_method->set_fstshpns_coring_index_scale =  img_ctx_set_fstshpns_cor_idx_scl;
    p_filter_method->get_fstshpns_coring_index_scale =  img_ctx_get_fstshpns_cor_idx_scl;

    p_filter_method->set_fstshpns_min_coring_result =  img_ctx_set_fstshpns_min_coring_result;
    p_filter_method->get_fstshpns_min_coring_result =  img_ctx_get_fstshpns_min_coring_result;

    p_filter_method->set_fstshpns_max_coring_result =  img_ctx_set_fstshpns_max_coring_result;
    p_filter_method->get_fstshpns_max_coring_result =  img_ctx_get_fstshpns_max_coring_result;

    p_filter_method->set_fstshpns_scale_coring =  img_ctx_set_fstshpns_scale_coring;
    p_filter_method->get_fstshpns_scale_coring =  img_ctx_get_fstshpns_scale_coring;

    p_filter_method->set_fnlshpns_both =  img_ctx_set_fnlshpns_both;
    p_filter_method->get_fnlshpns_both =  img_ctx_get_fnlshpns_both;

    p_filter_method->set_fnlshpns_noise =  img_ctx_set_fnlshpns_noise;
    p_filter_method->get_fnlshpns_noise =  img_ctx_get_fnlshpns_noise;

    p_filter_method->set_fnlshpns_fir =  img_ctx_set_fnlshpns_fir;
    p_filter_method->get_fnlshpns_fir =  img_ctx_get_fnlshpns_fir;

    p_filter_method->set_fnlshpns_coring =  img_ctx_set_fnlshpns_coring;
    p_filter_method->get_fnlshpns_coring =  img_ctx_get_fnlshpns_coring;

    p_filter_method->set_fnlshpns_coring_index_scale =  img_ctx_set_fnlshpns_cor_idx_scl;
    p_filter_method->get_fnlshpns_coring_index_scale =  img_ctx_get_fnlshpns_cor_idx_scl;

    p_filter_method->set_fnlshpns_min_coring_result =  img_ctx_set_fnlshpns_min_coring_result;
    p_filter_method->get_fnlshpns_min_coring_result =  img_ctx_get_fnlshpns_min_coring_result;

    p_filter_method->set_fnlshpns_max_coring_result =  img_ctx_set_fnlshpns_max_coring_result;
    p_filter_method->get_fnlshpns_max_coring_result =  img_ctx_get_fnlshpns_max_coring_result;

    p_filter_method->set_fnlshpns_scale_coring =  img_ctx_set_fnlshpns_scale_coring;
    p_filter_method->get_fnlshpns_scale_coring =  img_ctx_get_fnlshpns_scale_coring;

    p_filter_method->set_fnlshpns_both_tdt =  img_ctx_set_fnlshpns_both_tdt;
    p_filter_method->get_fnlshpns_both_tdt =  img_ctx_get_fnlshpns_both_tdt;

    p_filter_method->set_chroma_filter =  img_ctx_set_chroma_filter;
    p_filter_method->get_chroma_filter =  img_ctx_get_chroma_filter;

    p_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_set_video_mctf_and_final_sharpen;
    p_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_get_video_mctf_and_final_sharpen;

    p_filter_method->set_mctf_internal = img_ctx_set_video_mctf_internal;
    p_filter_method->get_mctf_internal = img_ctx_get_video_mctf_internal;

    {
        p_filter_method->set_y2y_use_cc_enable_info =  img_ctx_set_y2y_use_cc_enable_info;
        p_filter_method->get_y2y_use_cc_enable_info =  img_ctx_get_y2y_use_cc_enable_info;
    }
    {
        p_filter_method->set_front_end_tone_curve = img_ctx_set_fe_tone_curve;
        p_filter_method->get_front_end_tone_curve = img_ctx_get_fe_tone_curve;

        p_filter_method->set_exp0_fe_static_blc =  img_ctx_set_exp0_fe_static_blc;
        p_filter_method->get_exp0_fe_static_blc =  img_ctx_get_exp0_fe_static_blc;

        p_filter_method->set_exp0_fe_wb_gain =  img_ctx_set_exp0_fe_wb_gain;
        p_filter_method->get_exp0_fe_wb_gain =  img_ctx_get_exp0_fe_wb_gain;

        p_filter_method->set_exp0_fe_dgain_sat_lvl =  img_ctx_set_exp0_fe_dgain_sat_lvl;
        p_filter_method->get_exp0_fe_dgain_sat_lvl =  img_ctx_get_exp0_fe_dgain_sat_lvl;
    }
    {
        p_filter_method->set_ce =  img_ctx_set_ce;
        p_filter_method->get_ce =  img_ctx_get_ce;

        p_filter_method->set_ce_input_table =  img_ctx_set_ce_input_table;
        p_filter_method->get_ce_input_table =  img_ctx_get_ce_input_table;

        p_filter_method->set_ce_out_table =  img_ctx_set_ce_out_table;
        p_filter_method->get_ce_out_table =  img_ctx_get_ce_out_table;
    }
    {
        // hiso
        p_filter_method->set_hi_anti_aliasing = img_ctx_set_hi_anti_aliasing;
        p_filter_method->get_hi_anti_aliasing = img_ctx_get_hi_anti_aliasing;

        p_filter_method->set_hi_cfa_leakage_filter = img_ctx_set_hi_cfa_leakage_filter;
        p_filter_method->get_hi_cfa_leakage_filter = img_ctx_get_hi_cfa_leakage_filter;

        p_filter_method->set_hi_dynamic_bpc = img_ctx_set_hi_dynamic_bpc;
        p_filter_method->get_hi_dynamic_bpc = img_ctx_get_hi_dynamic_bpc;

        p_filter_method->set_hi_grgb_mismatch = img_ctx_set_hi_grgb_mismatch;
        p_filter_method->get_hi_grgb_mismatch = img_ctx_get_hi_grgb_mismatch;

        p_filter_method->set_hi_chroma_median_filter = img_ctx_set_hi_chroma_median_filter;
        p_filter_method->get_hi_chroma_median_filter = img_ctx_get_hi_chroma_median_filter;

        p_filter_method->set_hi_cfa_noise_filter = img_ctx_set_hi_cfa_noise_filter;
        p_filter_method->get_hi_cfa_noise_filter = img_ctx_get_hi_cfa_noise_filter;

        p_filter_method->set_hi_demosaic = img_ctx_set_hi_demosaic;
        p_filter_method->get_hi_demosaic = img_ctx_get_hi_demosaic;

        //hiso low2
        p_filter_method->set_li2_anti_aliasing = img_ctx_set_li2_anti_aliasing;
        p_filter_method->get_li2_anti_aliasing = img_ctx_get_li2_anti_aliasing;

        p_filter_method->set_li2_cfa_leakage_filter = img_ctx_set_li2_cfa_leakage_filter;
        p_filter_method->get_li2_cfa_leakage_filter = img_ctx_get_li2_cfa_leakage_filter;

        p_filter_method->set_li2_dynamic_bpc = img_ctx_set_li2_dynamic_bpc;
        p_filter_method->get_li2_dynamic_bpc = img_ctx_get_li2_dynamic_bpc;

        p_filter_method->set_li2_grgb_mismatch = img_ctx_set_li2_grgb_mismatch;
        p_filter_method->get_li2_grgb_mismatch = img_ctx_get_li2_grgb_mismatch;

        p_filter_method->set_li2_cfa_noise_filter = img_ctx_set_li2_cfa_noise_filter;
        p_filter_method->get_li2_cfa_noise_filter = img_ctx_get_li2_cfa_noise_filter;

        p_filter_method->set_li2_demosaic = img_ctx_set_li2_demosaic;
        p_filter_method->get_li2_demosaic = img_ctx_get_li2_demosaic;

        //asf
        p_filter_method->set_hi_asf = img_ctx_set_hi_asf;
        p_filter_method->get_hi_asf = img_ctx_get_hi_asf;

        p_filter_method->set_li2_asf = img_ctx_set_li2_asf;
        p_filter_method->get_li2_asf = img_ctx_get_li2_asf;

        p_filter_method->set_hi_low_asf = img_ctx_set_hi_low_asf;
        p_filter_method->get_hi_low_asf = img_ctx_get_hi_low_asf;

        p_filter_method->set_hi_med1_asf = img_ctx_set_hi_med1_asf;
        p_filter_method->get_hi_med1_asf = img_ctx_get_hi_med1_asf;

        p_filter_method->set_hi_med2_asf = img_ctx_set_hi_med2_asf;
        p_filter_method->get_hi_med2_asf = img_ctx_get_hi_med2_asf;

        p_filter_method->set_hi_high_asf = img_ctx_set_hi_high_asf;
        p_filter_method->get_hi_high_asf = img_ctx_get_hi_high_asf;

        p_filter_method->set_hi_high2_asf = img_ctx_set_hi_high2_asf;
        p_filter_method->get_hi_high2_asf = img_ctx_get_hi_high2_asf;

        p_filter_method->set_chroma_asf = img_ctx_set_chroma_asf;
        p_filter_method->get_chroma_asf = img_ctx_get_chroma_asf;

        p_filter_method->set_hi_chroma_asf = img_ctx_set_hi_chroma_asf;
        p_filter_method->get_hi_chroma_asf = img_ctx_get_hi_chroma_asf;

        p_filter_method->set_hi_low_chroma_asf = img_ctx_set_hi_low_chroma_asf;
        p_filter_method->get_hi_low_chroma_asf = img_ctx_get_hi_low_chroma_asf;

        // sharpen
        // --high--
        p_filter_method->set_hi_high_shpns_both = img_ctx_set_hi_high_shpns_both;
        p_filter_method->get_hi_high_shpns_both = img_ctx_get_hi_high_shpns_both;

        p_filter_method->set_hi_high_shpns_noise = img_ctx_set_hi_high_shpns_noise;
        p_filter_method->get_hi_high_shpns_noise = img_ctx_get_hi_high_shpns_noise;

        p_filter_method->set_hi_high_shpns_coring = img_ctx_set_hi_high_shpns_coring;
        p_filter_method->get_hi_high_shpns_coring = img_ctx_get_hi_high_shpns_coring;

        p_filter_method->set_hi_high_shpns_fir = img_ctx_set_hi_high_shpns_fir;
        p_filter_method->get_hi_high_shpns_fir = img_ctx_get_hi_high_shpns_fir;

        p_filter_method->set_hi_high_shpns_cor_idx_scl = img_ctx_set_hi_high_shpns_cor_idx_scl;
        p_filter_method->get_hi_high_shpns_cor_idx_scl = img_ctx_get_hi_high_shpns_cor_idx_scl;

        p_filter_method->set_hi_high_shpns_min_cor_rst = img_ctx_set_hi_high_shpns_min_cor_rst;
        p_filter_method->get_hi_high_shpns_min_cor_rst = img_ctx_get_hi_high_shpns_min_cor_rst;

        p_filter_method->set_hi_high_shpns_max_cor_rst = img_ctx_set_hi_high_shpns_max_cor_rst;
        p_filter_method->get_hi_high_shpns_max_cor_rst = img_ctx_get_hi_high_shpns_max_cor_rst;

        p_filter_method->set_hi_high_shpns_scl_cor = img_ctx_set_hi_high_shpns_scl_cor;
        p_filter_method->get_hi_high_shpns_scl_cor = img_ctx_get_hi_high_shpns_scl_cor;

        // --med--
        p_filter_method->set_hi_med_shpns_both = img_ctx_set_hi_med_shpns_both;
        p_filter_method->get_hi_med_shpns_both = img_ctx_get_hi_med_shpns_both;

        p_filter_method->set_hi_med_shpns_noise = img_ctx_set_hi_med_shpns_noise;
        p_filter_method->get_hi_med_shpns_noise = img_ctx_get_hi_med_shpns_noise;

        p_filter_method->set_hi_med_shpns_coring = img_ctx_set_hi_med_shpns_coring;
        p_filter_method->get_hi_med_shpns_coring = img_ctx_get_hi_med_shpns_coring;

        p_filter_method->set_hi_med_shpns_fir = img_ctx_set_hi_med_shpns_fir;
        p_filter_method->get_hi_med_shpns_fir = img_ctx_get_hi_med_shpns_fir;

        p_filter_method->set_hi_med_shpns_cor_idx_scl = img_ctx_set_hi_med_shpns_cor_idx_scl;
        p_filter_method->get_hi_med_shpns_cor_idx_scl = img_ctx_get_hi_med_shpns_cor_idx_scl;

        p_filter_method->set_hi_med_shpns_min_cor_rst = img_ctx_set_hi_med_shpns_min_cor_rst;
        p_filter_method->get_hi_med_shpns_min_cor_rst = img_ctx_get_hi_med_shpns_min_cor_rst;

        p_filter_method->set_hi_med_shpns_max_cor_rst = img_ctx_set_hi_med_shpns_max_cor_rst;
        p_filter_method->get_hi_med_shpns_max_cor_rst = img_ctx_get_hi_med_shpns_max_cor_rst;

        p_filter_method->set_hi_med_shpns_scl_cor = img_ctx_set_hi_med_shpns_scl_cor;
        p_filter_method->get_hi_med_shpns_scl_cor = img_ctx_get_hi_med_shpns_scl_cor;

        // --low2--
        p_filter_method->set_li2_shpns_both = img_ctx_set_li2_shpns_both;
        p_filter_method->get_li2_shpns_both = img_ctx_get_li2_shpns_both;

        p_filter_method->set_li2_shpns_noise = img_ctx_set_li2_shpns_noise;
        p_filter_method->get_li2_shpns_noise = img_ctx_get_li2_shpns_noise;

        p_filter_method->set_li2_shpns_coring = img_ctx_set_li2_shpns_coring;
        p_filter_method->get_li2_shpns_coring = img_ctx_get_li2_shpns_coring;

        p_filter_method->set_li2_shpns_fir = img_ctx_set_li2_shpns_fir;
        p_filter_method->get_li2_shpns_fir = img_ctx_get_li2_shpns_fir;

        p_filter_method->set_li2_shpns_cor_idx_scl = img_ctx_set_li2_shpns_cor_idx_scl;
        p_filter_method->get_li2_shpns_cor_idx_scl = img_ctx_get_li2_shpns_cor_idx_scl;

        p_filter_method->set_li2_shpns_min_cor_rst = img_ctx_set_li2_shpns_min_cor_rst;
        p_filter_method->get_li2_shpns_min_cor_rst = img_ctx_get_li2_shpns_min_cor_rst;

        p_filter_method->set_li2_shpns_max_cor_rst = img_ctx_set_li2_shpns_max_cor_rst;
        p_filter_method->get_li2_shpns_max_cor_rst = img_ctx_get_li2_shpns_max_cor_rst;

        p_filter_method->set_li2_shpns_scl_cor = img_ctx_set_li2_shpns_scl_cor;
        p_filter_method->get_li2_shpns_scl_cor = img_ctx_get_li2_shpns_scl_cor;

        // --hili--
        p_filter_method->set_hili_shpns_both = img_ctx_set_hili_shpns_both;
        p_filter_method->get_hili_shpns_both = img_ctx_get_hili_shpns_both;

        p_filter_method->set_hili_shpns_noise = img_ctx_set_hili_shpns_noise;
        p_filter_method->get_hili_shpns_noise = img_ctx_get_hili_shpns_noise;

        p_filter_method->set_hili_shpns_coring = img_ctx_set_hili_shpns_coring;
        p_filter_method->get_hili_shpns_coring = img_ctx_get_hili_shpns_coring;

        p_filter_method->set_hili_shpns_fir = img_ctx_set_hili_shpns_fir;
        p_filter_method->get_hili_shpns_fir = img_ctx_get_hili_shpns_fir;

        p_filter_method->set_hili_shpns_cor_idx_scl = img_ctx_set_hili_shpns_cor_idx_scl;
        p_filter_method->get_hili_shpns_cor_idx_scl = img_ctx_get_hili_shpns_cor_idx_scl;

        p_filter_method->set_hili_shpns_min_cor_rst = img_ctx_set_hili_shpns_min_cor_rst;
        p_filter_method->get_hili_shpns_min_cor_rst = img_ctx_get_hili_shpns_min_cor_rst;

        p_filter_method->set_hili_shpns_max_cor_rst = img_ctx_set_hili_shpns_max_cor_rst;
        p_filter_method->get_hili_shpns_max_cor_rst = img_ctx_get_hili_shpns_max_cor_rst;

        p_filter_method->set_hili_shpns_scl_cor = img_ctx_set_hili_shpns_scl_cor;
        p_filter_method->get_hili_shpns_scl_cor = img_ctx_get_hili_shpns_scl_cor;

        //chroma filter
        p_filter_method->set_hi_chroma_filter_high = img_ctx_set_hi_chroma_filter_high;
        p_filter_method->get_hi_chroma_filter_high = img_ctx_get_hi_chroma_filter_high;

        p_filter_method->set_hi_chroma_filter_pre = img_ctx_set_hi_chroma_filter_pre;
        p_filter_method->get_hi_chroma_filter_pre = img_ctx_get_hi_chroma_filter_pre;

        p_filter_method->set_hi_chroma_filter_med = img_ctx_set_hi_chroma_filter_med;
        p_filter_method->get_hi_chroma_filter_med = img_ctx_get_hi_chroma_filter_med;

        p_filter_method->set_hi_chroma_filter_low = img_ctx_set_hi_chroma_filter_low;
        p_filter_method->get_hi_chroma_filter_low = img_ctx_get_hi_chroma_filter_low;

        p_filter_method->set_hi_chroma_filter_very_low = img_ctx_set_hi_chroma_filter_very_low;
        p_filter_method->get_hi_chroma_filter_very_low = img_ctx_get_hi_chroma_filter_very_low;

        //combine
        p_filter_method->set_hi_luma_combine = img_ctx_set_hi_luma_combine;
        p_filter_method->get_hi_luma_combine = img_ctx_get_hi_luma_combine;

        p_filter_method->set_hi_low_asf_combine = img_ctx_set_hi_low_asf_combine;
        p_filter_method->get_hi_low_asf_combine = img_ctx_get_hi_low_asf_combine;

        p_filter_method->set_hi_chroma_fltr_med_com = img_ctx_set_hi_chroma_fltr_med_com;
        p_filter_method->get_hi_chroma_fltr_med_com = img_ctx_get_hi_chroma_fltr_med_com;

        p_filter_method->set_hi_chroma_fltr_low_com = img_ctx_set_hi_chroma_fltr_low_com;
        p_filter_method->get_hi_chroma_fltr_low_com = img_ctx_get_hi_chroma_fltr_low_com;

        p_filter_method->set_hi_chroma_fltr_very_low_com = img_ctx_set_hi_chroma_fltr_very_low_com;
        p_filter_method->get_hi_chroma_fltr_very_low_com = img_ctx_get_hi_chroma_fltr_very_low_com;

        p_filter_method->set_hili_combine = img_ctx_set_hili_combine;
        p_filter_method->get_hili_combine = img_ctx_get_hili_combine;

        p_filter_method->set_hi_mid_high_freq_recover = img_ctx_set_hi_mid_high_freq_recover;
        p_filter_method->get_hi_mid_high_freq_recover = img_ctx_get_hi_mid_high_freq_recover;

        p_filter_method->set_hi_mid_high_freq_recover = img_ctx_set_hi_mid_high_freq_recover;
        p_filter_method->get_hi_mid_high_freq_recover = img_ctx_get_hi_mid_high_freq_recover;

        p_filter_method->set_hi_luma_blend = img_ctx_set_hi_luma_blend;
        p_filter_method->get_hi_luma_blend = img_ctx_get_hi_luma_blend;

        p_filter_method->set_hi_nonsmooth_detect = img_ctx_set_hi_nonsmooth_detect;
        p_filter_method->get_hi_nonsmooth_detect = img_ctx_get_hi_nonsmooth_detect;

        p_filter_method->set_hi_select = img_ctx_set_hi_select;
        p_filter_method->get_hi_select = img_ctx_get_hi_select;
    }
}

static INLINE uint32 ctx_config_video_filter_invalid(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        rval = IK_ERR_0003;
    }

    if (p_ctx == NULL) {
        rval = IK_ERR_0005;
    }

    if (p_default_value == NULL) {
        //rval = IK_ERR_0005;
    }

    return rval;
}

static INLINE uint32 ctx_config_linear_filter(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval = IK_OK;

    //3a statistic
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_info(context_id, &p_default_value->aaa_stat_info);
    rval |= p_ctx->organization.filter_methods.set_af_stat_ex_info(context_id, &p_default_value->af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_pg_af_stat_ex_info(context_id, &p_default_value->pg_af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_float_tile_info(context_id, &p_default_value->aaa_float_tile_info);

    //calibration
    rval |= p_ctx->organization.filter_methods.set_window_size_info(context_id, &p_default_value->window_size_info);
    rval |= p_ctx->organization.filter_methods.set_cfa_window_size_info(context_id, &p_default_value->cfa_window_size_info);
    rval |= p_ctx->organization.filter_methods.set_calib_warp_info(context_id, &p_default_value->calib_warp_info);
    rval |= p_ctx->organization.filter_methods.set_calib_ca_warp_info(context_id, &p_default_value->calib_ca_warp_info);
    rval |= p_ctx->organization.filter_methods.set_static_bpc_enable_info(context_id, p_default_value->sbp_enable);
    if(p_default_value->sbp_enable == 1U) {
        rval |= p_ctx->organization.filter_methods.set_static_bpc(context_id, &p_default_value->static_bpc);
    }
    rval |= p_ctx->organization.filter_methods.set_vignette_compensation(context_id, &p_default_value->vignette_compensation);
    rval |= p_ctx->organization.filter_methods.set_dzoom_info(context_id, &p_default_value->dzoom_info);
    rval |= p_ctx->organization.filter_methods.set_warp_enable_info(context_id, p_default_value->warp_enable);
    rval |= p_ctx->organization.filter_methods.set_cawarp_enable_info(context_id, p_default_value->ca_warp_enable);
    rval |= p_ctx->organization.filter_methods.set_dmy_range_info(context_id, &p_default_value->dmy_range);

    //normal filters
    rval |= p_ctx->organization.filter_methods.set_before_ce_wb_gain(context_id, &p_default_value->before_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_after_ce_wb_gain(context_id, &p_default_value->after_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_ae_gain(context_id, &p_default_value->ae_gain);
    rval |= p_ctx->organization.filter_methods.set_global_dgain(context_id, &p_default_value->global_dgain);
    rval |= p_ctx->organization.filter_methods.set_cfa_leakage_filter(context_id, &p_default_value->cfa_leakage_filter);
    rval |= p_ctx->organization.filter_methods.set_anti_aliasing(context_id, &p_default_value->anti_aliasing);
    rval |= p_ctx->organization.filter_methods.set_dynamic_bpc(context_id, &p_default_value->dynamic_bpc);
    rval |= p_ctx->organization.filter_methods.set_grgb_mismatch(context_id, &p_default_value->grgb_mismatch);
    rval |= p_ctx->organization.filter_methods.set_cfa_noise_filter(context_id, &p_default_value->cfa_noise_filter);
    rval |= p_ctx->organization.filter_methods.set_demosaic(context_id, &p_default_value->demosaic);
    rval |= p_ctx->organization.filter_methods.set_color_correction_reg(context_id, &p_default_value->color_correction_reg);
    rval |= p_ctx->organization.filter_methods.set_color_correction(context_id, &p_default_value->color_correction);
    rval |= p_ctx->organization.filter_methods.set_tone_curve(context_id, &p_default_value->tone_curve);
    rval |= p_ctx->organization.filter_methods.set_rgb_to_yuv_matrix(context_id, &p_default_value->rgb_to_yuv_matrix);
    rval |= p_ctx->organization.filter_methods.set_rgb_ir(context_id, &p_default_value->rgb_ir);
    rval |= p_ctx->organization.filter_methods.set_chroma_scale(context_id, &p_default_value->chroma_scale);
    rval |= p_ctx->organization.filter_methods.set_chroma_median_filter(context_id, &p_default_value->chroma_median_filter);
    rval |= p_ctx->organization.filter_methods.set_first_luma_processing_mode(context_id, &p_default_value->first_luma_process_mode);
    if (p_default_value->first_luma_process_mode.use_sharpen_not_asf == 0U) {
        rval |= p_ctx->organization.filter_methods.set_advance_spatial_filter(context_id, &p_default_value->advance_spatial_filter);
    } else {
        rval |= p_ctx->organization.filter_methods.set_fstshpns_both(context_id, &p_default_value->first_sharpen_both);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_noise(context_id, &p_default_value->first_sharpen_noise);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_fir(context_id, &p_default_value->first_sharpen_fir);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring(context_id, &p_default_value->first_sharpen_coring);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring_index_scale(context_id, &p_default_value->first_sharpen_coring_idx_scale);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_min_coring_result(context_id, &p_default_value->first_sharpen_min_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_max_coring_result(context_id, &p_default_value->first_sharpen_max_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_scale_coring(context_id, &p_default_value->first_sharpen_scale_coring);
    }
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both(context_id, &p_default_value->final_sharpen_both);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_noise(context_id, &p_default_value->final_sharpen_noise);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_fir(context_id, &p_default_value->final_sharpen_fir);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring(context_id, &p_default_value->final_sharpen_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring_index_scale(context_id, &p_default_value->final_sharpen_coring_idx_scale);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_min_coring_result(context_id, &p_default_value->final_sharpen_min_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_max_coring_result(context_id, &p_default_value->final_sharpen_max_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_scale_coring(context_id, &p_default_value->final_sharpen_scale_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both_tdt(context_id, &p_default_value->final_sharpen_both_three_d_table);
    rval |= p_ctx->organization.filter_methods.set_chroma_filter(context_id, &p_default_value->chroma_filter);
    rval |= p_ctx->organization.filter_methods.set_video_mctf(context_id, &p_default_value->video_mctf);
    rval |= p_ctx->organization.filter_methods.set_video_mctf_ta(context_id, &p_default_value->video_mctf_ta);

    return rval;
}


static INLINE uint32 ctx_config_linear_ce_filter(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval = IK_OK;

    //3a statistic
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_info(context_id, &p_default_value->aaa_stat_info);
    rval |= p_ctx->organization.filter_methods.set_af_stat_ex_info(context_id, &p_default_value->af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_pg_af_stat_ex_info(context_id, &p_default_value->pg_af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_float_tile_info(context_id, &p_default_value->aaa_float_tile_info);

    //calibration
    rval |= p_ctx->organization.filter_methods.set_window_size_info(context_id, &p_default_value->window_size_info);
    rval |= p_ctx->organization.filter_methods.set_cfa_window_size_info(context_id, &p_default_value->cfa_window_size_info);
    rval |= p_ctx->organization.filter_methods.set_calib_warp_info(context_id, &p_default_value->calib_warp_info);
    rval |= p_ctx->organization.filter_methods.set_calib_ca_warp_info(context_id, &p_default_value->calib_ca_warp_info);
    rval |= p_ctx->organization.filter_methods.set_static_bpc_enable_info(context_id, p_default_value->sbp_enable);
    if(p_default_value->sbp_enable == 1U) {
        rval |= p_ctx->organization.filter_methods.set_static_bpc(context_id, &p_default_value->static_bpc);
    }
    rval |= p_ctx->organization.filter_methods.set_vignette_compensation(context_id, &p_default_value->vignette_compensation);
    rval |= p_ctx->organization.filter_methods.set_dzoom_info(context_id, &p_default_value->dzoom_info);
    rval |= p_ctx->organization.filter_methods.set_warp_enable_info(context_id, p_default_value->warp_enable);
    rval |= p_ctx->organization.filter_methods.set_cawarp_enable_info(context_id, p_default_value->ca_warp_enable);
    rval |= p_ctx->organization.filter_methods.set_dmy_range_info(context_id, &p_default_value->dmy_range);
    //normal filters
    rval |= p_ctx->organization.filter_methods.set_before_ce_wb_gain(context_id, &p_default_value->before_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_after_ce_wb_gain(context_id, &p_default_value->after_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_ae_gain(context_id, &p_default_value->ae_gain);
    rval |= p_ctx->organization.filter_methods.set_global_dgain(context_id, &p_default_value->global_dgain);
    rval |= p_ctx->organization.filter_methods.set_cfa_leakage_filter(context_id, &p_default_value->cfa_leakage_filter);
    rval |= p_ctx->organization.filter_methods.set_anti_aliasing(context_id, &p_default_value->anti_aliasing);
    rval |= p_ctx->organization.filter_methods.set_dynamic_bpc(context_id, &p_default_value->dynamic_bpc);
    rval |= p_ctx->organization.filter_methods.set_grgb_mismatch(context_id, &p_default_value->grgb_mismatch);
    rval |= p_ctx->organization.filter_methods.set_cfa_noise_filter(context_id, &p_default_value->cfa_noise_filter);
    rval |= p_ctx->organization.filter_methods.set_demosaic(context_id, &p_default_value->demosaic);
    rval |= p_ctx->organization.filter_methods.set_color_correction_reg(context_id, &p_default_value->color_correction_reg);
    rval |= p_ctx->organization.filter_methods.set_color_correction(context_id, &p_default_value->color_correction);
    rval |= p_ctx->organization.filter_methods.set_tone_curve(context_id, &p_default_value->tone_curve);
    rval |= p_ctx->organization.filter_methods.set_rgb_to_yuv_matrix(context_id, &p_default_value->rgb_to_yuv_matrix);
    rval |= p_ctx->organization.filter_methods.set_rgb_ir(context_id, &p_default_value->rgb_ir);
    rval |= p_ctx->organization.filter_methods.set_chroma_scale(context_id, &p_default_value->chroma_scale);
    rval |= p_ctx->organization.filter_methods.set_chroma_median_filter(context_id, &p_default_value->chroma_median_filter);
    rval |= p_ctx->organization.filter_methods.set_first_luma_processing_mode(context_id, &p_default_value->first_luma_process_mode);
    if (p_default_value->first_luma_process_mode.use_sharpen_not_asf == 0U) {
        rval |= p_ctx->organization.filter_methods.set_advance_spatial_filter(context_id, &p_default_value->advance_spatial_filter);
    } else {
        rval |= p_ctx->organization.filter_methods.set_fstshpns_both(context_id, &p_default_value->first_sharpen_both);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_noise(context_id, &p_default_value->first_sharpen_noise);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_fir(context_id, &p_default_value->first_sharpen_fir);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring(context_id, &p_default_value->first_sharpen_coring);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring_index_scale(context_id, &p_default_value->first_sharpen_coring_idx_scale);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_min_coring_result(context_id, &p_default_value->first_sharpen_min_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_max_coring_result(context_id, &p_default_value->first_sharpen_max_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_scale_coring(context_id, &p_default_value->first_sharpen_scale_coring);
    }
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both(context_id, &p_default_value->final_sharpen_both);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_noise(context_id, &p_default_value->final_sharpen_noise);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_fir(context_id, &p_default_value->final_sharpen_fir);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring(context_id, &p_default_value->final_sharpen_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring_index_scale(context_id, &p_default_value->final_sharpen_coring_idx_scale);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_min_coring_result(context_id, &p_default_value->final_sharpen_min_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_max_coring_result(context_id, &p_default_value->final_sharpen_max_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_scale_coring(context_id, &p_default_value->final_sharpen_scale_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both_tdt(context_id, &p_default_value->final_sharpen_both_three_d_table);
    rval |= p_ctx->organization.filter_methods.set_chroma_filter(context_id, &p_default_value->chroma_filter);
    rval |= p_ctx->organization.filter_methods.set_video_mctf(context_id, &p_default_value->video_mctf);
    rval |= p_ctx->organization.filter_methods.set_video_mctf_ta(context_id, &p_default_value->video_mctf_ta);

    //hdr
    rval |= p_ctx->organization.filter_methods.set_exp0_fe_static_blc(context_id, &p_default_value->exp0_frontend_static_blc);
    rval |= p_ctx->organization.filter_methods.set_exp0_fe_wb_gain(context_id, &p_default_value->exp0_frontend_wb_gain);
    //rval |= p_ctx->organization.filter_methods.set_exp0_fe_dgain_sat_lvl(context_id, &p_default_value->exp0_frontend_dgain_saturation_level); //Dgain saturation level should not be set, should be calculated by calling set_sat_level()
    rval |= p_ctx->organization.filter_methods.set_ce(context_id, &p_default_value->ce);
    rval |= p_ctx->organization.filter_methods.set_ce_input_table(context_id, &p_default_value->ce_input_table);
    rval |= p_ctx->organization.filter_methods.set_ce_out_table(context_id, &p_default_value->ce_out_table);

    return rval;
}


#if PRE_NN_PROCESS
static INLINE uint32 ctx_config_pre_nn_filter(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    return IK_OK;
}
#endif

static INLINE uint32 ctx_config_hdr_2x_filter(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval = IK_OK;

    //3a statistic
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_info(context_id, &p_default_value->aaa_stat_info);
    rval |= p_ctx->organization.filter_methods.set_af_stat_ex_info(context_id, &p_default_value->af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_pg_af_stat_ex_info(context_id, &p_default_value->pg_af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_float_tile_info(context_id, &p_default_value->aaa_float_tile_info);

    //calibration
    rval |= p_ctx->organization.filter_methods.set_window_size_info(context_id, &p_default_value->window_size_info);
    rval |= p_ctx->organization.filter_methods.set_cfa_window_size_info(context_id, &p_default_value->cfa_window_size_info);
    rval |= p_ctx->organization.filter_methods.set_calib_warp_info(context_id, &p_default_value->calib_warp_info);
    rval |= p_ctx->organization.filter_methods.set_calib_ca_warp_info(context_id, &p_default_value->calib_ca_warp_info);
    rval |= p_ctx->organization.filter_methods.set_static_bpc_enable_info(context_id, p_default_value->sbp_enable);
    if(p_default_value->sbp_enable == 1U) {
        rval |= p_ctx->organization.filter_methods.set_static_bpc(context_id, &p_default_value->static_bpc);
    }
    rval |= p_ctx->organization.filter_methods.set_vignette_compensation(context_id, &p_default_value->vignette_compensation);
    rval |= p_ctx->organization.filter_methods.set_dzoom_info(context_id, &p_default_value->dzoom_info);
    rval |= p_ctx->organization.filter_methods.set_warp_enable_info(context_id, p_default_value->warp_enable);
    rval |= p_ctx->organization.filter_methods.set_cawarp_enable_info(context_id, p_default_value->ca_warp_enable);
    rval |= p_ctx->organization.filter_methods.set_dmy_range_info(context_id, &p_default_value->dmy_range);

    //normal filters
    rval |= p_ctx->organization.filter_methods.set_before_ce_wb_gain(context_id, &p_default_value->before_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_after_ce_wb_gain(context_id, &p_default_value->after_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_ae_gain(context_id, &p_default_value->ae_gain);
    rval |= p_ctx->organization.filter_methods.set_global_dgain(context_id, &p_default_value->global_dgain);
    rval |= p_ctx->organization.filter_methods.set_cfa_leakage_filter(context_id, &p_default_value->cfa_leakage_filter);
    rval |= p_ctx->organization.filter_methods.set_anti_aliasing(context_id, &p_default_value->anti_aliasing);
    rval |= p_ctx->organization.filter_methods.set_dynamic_bpc(context_id, &p_default_value->dynamic_bpc);
    rval |= p_ctx->organization.filter_methods.set_grgb_mismatch(context_id, &p_default_value->grgb_mismatch);
    rval |= p_ctx->organization.filter_methods.set_cfa_noise_filter(context_id, &p_default_value->cfa_noise_filter);
    rval |= p_ctx->organization.filter_methods.set_demosaic(context_id, &p_default_value->demosaic);
    rval |= p_ctx->organization.filter_methods.set_color_correction_reg(context_id, &p_default_value->color_correction_reg);
    rval |= p_ctx->organization.filter_methods.set_color_correction(context_id, &p_default_value->color_correction);
    rval |= p_ctx->organization.filter_methods.set_tone_curve(context_id, &p_default_value->tone_curve);
    rval |= p_ctx->organization.filter_methods.set_rgb_to_yuv_matrix(context_id, &p_default_value->rgb_to_yuv_matrix);
    rval |= p_ctx->organization.filter_methods.set_chroma_scale(context_id, &p_default_value->chroma_scale);
    rval |= p_ctx->organization.filter_methods.set_chroma_median_filter(context_id, &p_default_value->chroma_median_filter);
    rval |= p_ctx->organization.filter_methods.set_first_luma_processing_mode(context_id, &p_default_value->first_luma_process_mode);
    if (p_default_value->first_luma_process_mode.use_sharpen_not_asf == 0U) {
        rval |= p_ctx->organization.filter_methods.set_advance_spatial_filter(context_id, &p_default_value->advance_spatial_filter);
    } else {
        rval |= p_ctx->organization.filter_methods.set_fstshpns_both(context_id, &p_default_value->first_sharpen_both);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_noise(context_id, &p_default_value->first_sharpen_noise);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_fir(context_id, &p_default_value->first_sharpen_fir);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring(context_id, &p_default_value->first_sharpen_coring);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring_index_scale(context_id, &p_default_value->first_sharpen_coring_idx_scale);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_min_coring_result(context_id, &p_default_value->first_sharpen_min_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_max_coring_result(context_id, &p_default_value->first_sharpen_max_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_scale_coring(context_id, &p_default_value->first_sharpen_scale_coring);
    }
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both(context_id, &p_default_value->final_sharpen_both);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_noise(context_id, &p_default_value->final_sharpen_noise);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_fir(context_id, &p_default_value->final_sharpen_fir);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring(context_id, &p_default_value->final_sharpen_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring_index_scale(context_id, &p_default_value->final_sharpen_coring_idx_scale);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_min_coring_result(context_id, &p_default_value->final_sharpen_min_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_max_coring_result(context_id, &p_default_value->final_sharpen_max_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_scale_coring(context_id, &p_default_value->final_sharpen_scale_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both_tdt(context_id, &p_default_value->final_sharpen_both_three_d_table);

    rval |= p_ctx->organization.filter_methods.set_chroma_filter(context_id, &p_default_value->chroma_filter);
    rval |= p_ctx->organization.filter_methods.set_video_mctf(context_id, &p_default_value->video_mctf);
    rval |= p_ctx->organization.filter_methods.set_video_mctf_ta(context_id, &p_default_value->video_mctf_ta);

    //hdr
    rval |= p_ctx->organization.filter_methods.set_hdr_raw_offset(context_id, &p_default_value->hdr_raw_info);
    rval |= p_ctx->organization.filter_methods.set_exp0_fe_static_blc(context_id, &p_default_value->exp0_frontend_static_blc);
    rval |= p_ctx->organization.filter_methods.set_exp1_fe_static_blc(context_id, &p_default_value->exp1_frontend_static_blc);
    rval |= p_ctx->organization.filter_methods.set_exp0_fe_wb_gain(context_id, &p_default_value->exp0_frontend_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_exp1_fe_wb_gain(context_id, &p_default_value->exp1_frontend_wb_gain);
    //rval |= p_ctx->organization.filter_methods.set_exp0_fe_dgain_sat_lvl(context_id, &p_default_value->exp0_frontend_dgain_saturation_level); //Dgain saturation level should not be set, should be calculated by calling set_sat_level()
    //rval |= p_ctx->organization.filter_methods.set_exp1_fe_dgain_sat_lvl(context_id, &p_default_value->exp1_frontend_dgain_saturation_level);
    rval |= p_ctx->organization.filter_methods.set_ce(context_id, &p_default_value->ce);
    rval |= p_ctx->organization.filter_methods.set_ce_input_table(context_id, &p_default_value->ce_input_table);
    rval |= p_ctx->organization.filter_methods.set_ce_out_table(context_id, &p_default_value->ce_out_table);
    rval |= p_ctx->organization.filter_methods.set_hdr_blend(context_id, &p_default_value->hdr_blend);

    return rval;
}




static INLINE uint32 ctx_config_hdr_3x_filter(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval = IK_OK;

    //3a statistic
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_info(context_id, &p_default_value->aaa_stat_info);
    rval |= p_ctx->organization.filter_methods.set_af_stat_ex_info(context_id, &p_default_value->af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_pg_af_stat_ex_info(context_id, &p_default_value->pg_af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_float_tile_info(context_id, &p_default_value->aaa_float_tile_info);

    //calibration
    rval |= p_ctx->organization.filter_methods.set_window_size_info(context_id, &p_default_value->window_size_info);
    rval |= p_ctx->organization.filter_methods.set_cfa_window_size_info(context_id, &p_default_value->cfa_window_size_info);
    rval |= p_ctx->organization.filter_methods.set_calib_warp_info(context_id, &p_default_value->calib_warp_info);
    rval |= p_ctx->organization.filter_methods.set_calib_ca_warp_info(context_id, &p_default_value->calib_ca_warp_info);
    rval |= p_ctx->organization.filter_methods.set_static_bpc_enable_info(context_id, p_default_value->sbp_enable);
    if(p_default_value->sbp_enable == 1U) {
        rval |= p_ctx->organization.filter_methods.set_static_bpc(context_id, &p_default_value->static_bpc);
    }
    rval |= p_ctx->organization.filter_methods.set_vignette_compensation(context_id, &p_default_value->vignette_compensation);
    rval |= p_ctx->organization.filter_methods.set_dzoom_info(context_id, &p_default_value->dzoom_info);
    rval |= p_ctx->organization.filter_methods.set_warp_enable_info(context_id, p_default_value->warp_enable);
    rval |= p_ctx->organization.filter_methods.set_cawarp_enable_info(context_id, p_default_value->ca_warp_enable);
    rval |= p_ctx->organization.filter_methods.set_dmy_range_info(context_id, &p_default_value->dmy_range);

    //normal filters
    rval |= p_ctx->organization.filter_methods.set_before_ce_wb_gain(context_id, &p_default_value->before_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_after_ce_wb_gain(context_id, &p_default_value->after_ce_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_ae_gain(context_id, &p_default_value->ae_gain);
    rval |= p_ctx->organization.filter_methods.set_global_dgain(context_id, &p_default_value->global_dgain);
    rval |= p_ctx->organization.filter_methods.set_cfa_leakage_filter(context_id, &p_default_value->cfa_leakage_filter);
    rval |= p_ctx->organization.filter_methods.set_anti_aliasing(context_id, &p_default_value->anti_aliasing);
    rval |= p_ctx->organization.filter_methods.set_dynamic_bpc(context_id, &p_default_value->dynamic_bpc);
    rval |= p_ctx->organization.filter_methods.set_grgb_mismatch(context_id, &p_default_value->grgb_mismatch);
    rval |= p_ctx->organization.filter_methods.set_cfa_noise_filter(context_id, &p_default_value->cfa_noise_filter);
    rval |= p_ctx->organization.filter_methods.set_demosaic(context_id, &p_default_value->demosaic);
    rval |= p_ctx->organization.filter_methods.set_color_correction_reg(context_id, &p_default_value->color_correction_reg);
    rval |= p_ctx->organization.filter_methods.set_color_correction(context_id, &p_default_value->color_correction);
    rval |= p_ctx->organization.filter_methods.set_tone_curve(context_id, &p_default_value->tone_curve);
    rval |= p_ctx->organization.filter_methods.set_rgb_to_yuv_matrix(context_id, &p_default_value->rgb_to_yuv_matrix);
    rval |= p_ctx->organization.filter_methods.set_chroma_scale(context_id, &p_default_value->chroma_scale);
    rval |= p_ctx->organization.filter_methods.set_chroma_median_filter(context_id, &p_default_value->chroma_median_filter);
    rval |= p_ctx->organization.filter_methods.set_first_luma_processing_mode(context_id, &p_default_value->first_luma_process_mode);
    if (p_default_value->first_luma_process_mode.use_sharpen_not_asf == 0U) {
        rval |= p_ctx->organization.filter_methods.set_advance_spatial_filter(context_id, &p_default_value->advance_spatial_filter);
    } else {
        rval |= p_ctx->organization.filter_methods.set_fstshpns_both(context_id, &p_default_value->first_sharpen_both);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_noise(context_id, &p_default_value->first_sharpen_noise);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_fir(context_id, &p_default_value->first_sharpen_fir);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring(context_id, &p_default_value->first_sharpen_coring);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring_index_scale(context_id, &p_default_value->first_sharpen_coring_idx_scale);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_min_coring_result(context_id, &p_default_value->first_sharpen_min_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_max_coring_result(context_id, &p_default_value->first_sharpen_max_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_scale_coring(context_id, &p_default_value->first_sharpen_scale_coring);
    }
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both(context_id, &p_default_value->final_sharpen_both);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_noise(context_id, &p_default_value->final_sharpen_noise);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_fir(context_id, &p_default_value->final_sharpen_fir);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring(context_id, &p_default_value->final_sharpen_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring_index_scale(context_id, &p_default_value->final_sharpen_coring_idx_scale);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_min_coring_result(context_id, &p_default_value->final_sharpen_min_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_max_coring_result(context_id, &p_default_value->final_sharpen_max_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_scale_coring(context_id, &p_default_value->final_sharpen_scale_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both_tdt(context_id, &p_default_value->final_sharpen_both_three_d_table);
    rval |= p_ctx->organization.filter_methods.set_chroma_filter(context_id, &p_default_value->chroma_filter);
    rval |= p_ctx->organization.filter_methods.set_video_mctf(context_id, &p_default_value->video_mctf);
    rval |= p_ctx->organization.filter_methods.set_video_mctf_ta(context_id, &p_default_value->video_mctf_ta);

    //hdr
    rval |= p_ctx->organization.filter_methods.set_hdr_raw_offset(context_id, &p_default_value->hdr_raw_info);
    rval |= p_ctx->organization.filter_methods.set_exp0_fe_static_blc(context_id, &p_default_value->exp0_frontend_static_blc);
    rval |= p_ctx->organization.filter_methods.set_exp1_fe_static_blc(context_id, &p_default_value->exp1_frontend_static_blc);
    rval |= p_ctx->organization.filter_methods.set_exp2_fe_static_blc(context_id, &p_default_value->exp2_frontend_static_blc);
    rval |= p_ctx->organization.filter_methods.set_exp0_fe_wb_gain(context_id, &p_default_value->exp0_frontend_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_exp1_fe_wb_gain(context_id, &p_default_value->exp1_frontend_wb_gain);
    rval |= p_ctx->organization.filter_methods.set_exp2_fe_wb_gain(context_id, &p_default_value->exp2_frontend_wb_gain);
    //rval |= p_ctx->organization.filter_methods.set_exp0_fe_dgain_sat_lvl(context_id, &p_default_value->exp0_frontend_dgain_saturation_level); //Dgain saturation level should not be set, should be calculated by calling set_sat_level()
    //rval |= p_ctx->organization.filter_methods.set_exp1_fe_dgain_sat_lvl(context_id, &p_default_value->exp1_frontend_dgain_saturation_level);
    //rval |= p_ctx->organization.filter_methods.set_exp2_fe_dgain_sat_lvl(context_id, &p_default_value->exp2_frontend_dgain_saturation_level);
    rval |= p_ctx->organization.filter_methods.set_ce(context_id, &p_default_value->ce);
    rval |= p_ctx->organization.filter_methods.set_ce_input_table(context_id, &p_default_value->ce_input_table);
    rval |= p_ctx->organization.filter_methods.set_ce_out_table(context_id, &p_default_value->ce_out_table);
    rval |= p_ctx->organization.filter_methods.set_hdr_blend(context_id, &p_default_value->hdr_blend);

    return rval;
}

static INLINE uint32 ctx_config_y2y_filter(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval = IK_OK;
#if 0
    //3a statistic
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_info(context_id, &p_default_value->aaa_stat_info);
    rval |= p_ctx->organization.filter_methods.set_pg_af_stat_ex_info(context_id, &p_default_value->pg_af_stat_ex_info);
    rval |= p_ctx->organization.filter_methods.set_aaa_stat_float_tile_info(context_id, &p_default_value->aaa_float_tile_info);

    //calibration
    rval |= p_ctx->organization.filter_methods.set_window_size_info(context_id, &p_default_value->window_size_info);
    rval |= p_ctx->organization.filter_methods.set_calib_warp_info(context_id, &p_default_value->calib_warp_info);
    rval |= p_ctx->organization.filter_methods.set_dzoom_info(context_id, &p_default_value->dzoom_info);
    rval |= p_ctx->organization.filter_methods.set_warp_enable_info(context_id, p_default_value->warp_enable);
    rval |= p_ctx->organization.filter_methods.set_dmy_range_info(context_id, &p_default_value->dmy_range);

    //normal filters
    rval |= p_ctx->organization.filter_methods.set_color_correction_reg(context_id, &p_default_value->color_correction_reg);
    rval |= p_ctx->organization.filter_methods.set_color_correction(context_id, &p_default_value->color_correction);
    rval |= p_ctx->organization.filter_methods.set_tone_curve(context_id, &p_default_value->tone_curve);
    rval |= p_ctx->organization.filter_methods.set_rgb_to_yuv_matrix(context_id, &p_default_value->rgb_to_yuv_matrix);
    rval |= p_ctx->organization.filter_methods.set_chroma_scale(context_id, &p_default_value->chroma_scale);
    rval |= p_ctx->organization.filter_methods.set_chroma_median_filter(context_id, &p_default_value->chroma_median_filter);
#endif
    rval |= p_ctx->organization.filter_methods.set_first_luma_processing_mode(context_id, &p_default_value->first_luma_process_mode);
    if (p_default_value->first_luma_process_mode.use_sharpen_not_asf == 0U) {
        rval |= p_ctx->organization.filter_methods.set_advance_spatial_filter(context_id, &p_default_value->advance_spatial_filter);
    } else {
        rval |= p_ctx->organization.filter_methods.set_fstshpns_both(context_id, &p_default_value->first_sharpen_both);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_noise(context_id, &p_default_value->first_sharpen_noise);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_fir(context_id, &p_default_value->first_sharpen_fir);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring(context_id, &p_default_value->first_sharpen_coring);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_coring_index_scale(context_id, &p_default_value->first_sharpen_coring_idx_scale);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_min_coring_result(context_id, &p_default_value->first_sharpen_min_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_max_coring_result(context_id, &p_default_value->first_sharpen_max_coring_result);
        rval |= p_ctx->organization.filter_methods.set_fstshpns_scale_coring(context_id, &p_default_value->first_sharpen_scale_coring);
    }
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both(context_id, &p_default_value->final_sharpen_both);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_noise(context_id, &p_default_value->final_sharpen_noise);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_fir(context_id, &p_default_value->final_sharpen_fir);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring(context_id, &p_default_value->final_sharpen_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_coring_index_scale(context_id, &p_default_value->final_sharpen_coring_idx_scale);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_min_coring_result(context_id, &p_default_value->final_sharpen_min_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_max_coring_result(context_id, &p_default_value->final_sharpen_max_coring_result);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_scale_coring(context_id, &p_default_value->final_sharpen_scale_coring);
    rval |= p_ctx->organization.filter_methods.set_fnlshpns_both_tdt(context_id, &p_default_value->final_sharpen_both_three_d_table);
    rval |= p_ctx->organization.filter_methods.set_chroma_filter(context_id, &p_default_value->chroma_filter);
    rval |= p_ctx->organization.filter_methods.set_video_mctf(context_id, &p_default_value->video_mctf);
    rval |= p_ctx->organization.filter_methods.set_video_mctf_ta(context_id, &p_default_value->video_mctf_ta);
    return rval;
}

static INLINE void ctx_get_filter_method(uint8 video_pipe, amba_ik_filter_method_t* p_filter_method)
{
    if (video_pipe < (uint8)AMBA_IK_VIDEO_MAX) {
        if(video_pipe == (uint8)AMBA_IK_VIDEO_Y2Y_MIPI) {
            img_ctx_hook_mipi(p_filter_method);
        } else {
            img_ctx_hook_filter_method(video_pipe, p_filter_method);
        }
    } else if (video_pipe == (uint8)AMBA_IK_STILL_LISO) {
        img_ctx_hook_filter_method((uint8)AMBA_IK_VIDEO_LINEAR_CE, p_filter_method);
    } else if (video_pipe == (uint8)AMBA_IK_STILL_HISO) {
        img_ctx_hook_filter_method_hiso(p_filter_method);
    } else {
        img_ctx_hook_ivd_filter_method(p_filter_method);
        amba_ik_system_print_uint32_5("[IK] Error, Current pipe %d is not support ", video_pipe, DC_U, DC_U, DC_U, DC_U);
    }
}
static INLINE uint32 ctx_config_filter_baseline_default(uint32 context_id, const ik_ability_t *p_ability, amba_ik_context_entity_t *p_ctx)
{
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        rval = IK_ERR_0003;
    }

    if (p_ctx == NULL) {
        rval = IK_ERR_0005;
    } else {
        p_ctx->filters.update_flags.liso.aaa_stat_info_updated = 1;

        p_ctx->filters.input_param.aaa_stat_info.awb_tile_num_col = 32UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_num_row = 32UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_col_start = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_row_start = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_width = 128UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_height = 128UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_active_width = 128UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_tile_active_height = 128UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_pix_min_value = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.awb_pix_max_value = 16383UL;

        p_ctx->filters.input_param.aaa_stat_info.ae_tile_num_col = 12UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_tile_num_row = 8UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_tile_col_start = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_tile_row_start = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_tile_width = 341UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_tile_height = 512UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_pix_min_value = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.ae_pix_max_value = 16383UL;

        p_ctx->filters.input_param.aaa_stat_info.af_tile_num_col = 12UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_num_row = 8UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_col_start = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_row_start = 0UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_width = 256UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_height = 256UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_active_width = 256UL;
        p_ctx->filters.input_param.aaa_stat_info.af_tile_active_height = 256UL;

        p_ctx->filters.input_param.ae_gain.ae_gain = 4096UL;
        p_ctx->filters.input_param.global_dgain.global_dgain = 4096UL;

        p_ctx->filters.input_param.exp0_frontend_wb_gain.r_gain = 4096UL;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.g_gain = 4096UL;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.b_gain = 4096UL;
        p_ctx->filters.input_param.exp0_frontend_wb_gain.shutter_ratio = 1UL;

        p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_den = 1UL;
        p_ctx->filters.input_param.window_size_info.vin_sensor.h_sub_sample.factor_num = 1UL;
        p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_den = 1UL;
        p_ctx->filters.input_param.window_size_info.vin_sensor.v_sub_sample.factor_num = 1UL;
        p_ctx->filters.input_param.calib_warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1UL;
        p_ctx->filters.input_param.calib_warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1UL;
        p_ctx->filters.input_param.calib_warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1UL;
        p_ctx->filters.input_param.calib_warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1UL;
        p_ctx->filters.input_param.calib_ca_warp_info.vin_sensor_geo.h_sub_sample.factor_den = 1UL;
        p_ctx->filters.input_param.calib_ca_warp_info.vin_sensor_geo.h_sub_sample.factor_num = 1UL;
        p_ctx->filters.input_param.calib_ca_warp_info.vin_sensor_geo.v_sub_sample.factor_den = 1UL;
        p_ctx->filters.input_param.calib_ca_warp_info.vin_sensor_geo.v_sub_sample.factor_num = 1UL;

        p_ctx->filters.input_param.dgain_sauration_level.level_blue = 16383UL;
        p_ctx->filters.input_param.dgain_sauration_level.level_green_even= 16383UL;
        p_ctx->filters.input_param.dgain_sauration_level.level_green_odd= 16383UL;
        p_ctx->filters.input_param.dgain_sauration_level.level_red= 16383UL;

        p_ctx->filters.input_param.hist_info.ae_tile_mask[0] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[1] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[2] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[3] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[4] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[5] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[6] = 0xFFFF;
        p_ctx->filters.input_param.hist_info.ae_tile_mask[7] = 0xFFFF;

        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[0] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[1] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[2] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[3] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[4] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[5] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[6] = 0xFFFF;
        p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[7] = 0xFFFF;

        if ((p_ability->video_pipe != (uint8) AMBA_IK_VIDEO_Y2Y) && (p_ability->video_pipe != (uint8) AMBA_IK_VIDEO_Y2Y_MD)) {
            p_ctx->filters.update_flags.liso.histogram_info_update = 1;
        }
        p_ctx->filters.update_flags.liso.histogram_info_pg_update = 1;
        p_ctx->filters.input_param.use_cc_for_yuv2yuv = 1;

        p_ctx->filters.input_param.ce_use_external_hds_mode = 0;

        if ((p_ability->video_pipe == (uint8) AMBA_IK_VIDEO_Y2Y) || (p_ability->video_pipe == (uint8) AMBA_IK_VIDEO_Y2Y_MD)) {
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[0] = 1024;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[1] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[2] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[3] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[4] = 1024;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[5] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[6] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[7] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[8] = 1024;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.y_offset = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.u_offset = 128;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.v_offset = 128;
        }
        if(p_ability->video_pipe == (uint8) AMBA_IK_VIDEO_Y2Y_MIPI) {
            static const uint32 tone_curve_blue[] = {
                0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
                64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
                128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189,
                193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253,
                257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317,
                321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381,
                385, 389, 393, 397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445,
                449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509,
                514, 518, 522, 526, 530, 534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574,
                578, 582, 586, 590, 594, 598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638,
                642, 646, 650, 654, 658, 662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702,
                706, 710, 714, 718, 722, 726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766,
                770, 774, 778, 782, 786, 790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830,
                834, 838, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895,
                899, 903, 907, 911, 915, 919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959,
                963, 967, 971, 975, 979, 983, 987, 991, 995, 999, 1003, 1007, 1011, 1015, 1019, 1023
            };
            static const uint32 tone_curve_green[] = {
                0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
                64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
                128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189,
                193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253,
                257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317,
                321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381,
                385, 389, 393, 397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445,
                449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509,
                514, 518, 522, 526, 530, 534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574,
                578, 582, 586, 590, 594, 598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638,
                642, 646, 650, 654, 658, 662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702,
                706, 710, 714, 718, 722, 726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766,
                770, 774, 778, 782, 786, 790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830,
                834, 838, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895,
                899, 903, 907, 911, 915, 919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959,
                963, 967, 971, 975, 979, 983, 987, 991, 995, 999, 1003, 1007, 1011, 1015, 1019, 1023
            };
            static const uint32 tone_curve_red[] = {
                0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
                64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124,
                128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 173, 177, 181, 185, 189,
                193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 237, 241, 245, 249, 253,
                257, 261, 265, 269, 273, 277, 281, 285, 289, 293, 297, 301, 305, 309, 313, 317,
                321, 325, 329, 333, 337, 341, 345, 349, 353, 357, 361, 365, 369, 373, 377, 381,
                385, 389, 393, 397, 401, 405, 409, 413, 417, 421, 425, 429, 433, 437, 441, 445,
                449, 453, 457, 461, 465, 469, 473, 477, 481, 485, 489, 493, 497, 501, 505, 509,
                514, 518, 522, 526, 530, 534, 538, 542, 546, 550, 554, 558, 562, 566, 570, 574,
                578, 582, 586, 590, 594, 598, 602, 606, 610, 614, 618, 622, 626, 630, 634, 638,
                642, 646, 650, 654, 658, 662, 666, 670, 674, 678, 682, 686, 690, 694, 698, 702,
                706, 710, 714, 718, 722, 726, 730, 734, 738, 742, 746, 750, 754, 758, 762, 766,
                770, 774, 778, 782, 786, 790, 794, 798, 802, 806, 810, 814, 818, 822, 826, 830,
                834, 838, 842, 846, 850, 855, 859, 863, 867, 871, 875, 879, 883, 887, 891, 895,
                899, 903, 907, 911, 915, 919, 923, 927, 931, 935, 939, 943, 947, 951, 955, 959,
                963, 967, 971, 975, 979, 983, 987, 991, 995, 999, 1003, 1007, 1011, 1015, 1019, 1023
            };

            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.tone_curve.tone_curve_blue[0], &tone_curve_blue[0], sizeof(uint32)*IK_NUM_TONE_CURVE);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.tone_curve.tone_curve_green[0], &tone_curve_green[0], sizeof(uint32)*IK_NUM_TONE_CURVE);
            (void)amba_ik_system_memcpy(&p_ctx->filters.input_param.tone_curve.tone_curve_red[0], &tone_curve_red[0], sizeof(uint32)*IK_NUM_TONE_CURVE);

            p_ctx->filters.input_param.exp0_frontend_wb_gain.b_gain = 4096UL;
            p_ctx->filters.input_param.exp0_frontend_wb_gain.g_gain = 4096UL;
            p_ctx->filters.input_param.exp0_frontend_wb_gain.r_gain = 4096UL;
            p_ctx->filters.input_param.exp0_frontend_wb_gain.shutter_ratio = 1UL;

            p_ctx->filters.input_param.global_dgain.global_dgain = 4096UL;
            p_ctx->filters.input_param.before_ce_wb_gain.gain_b = 64UL;
            p_ctx->filters.input_param.before_ce_wb_gain.gain_g = 64UL;
            p_ctx->filters.input_param.before_ce_wb_gain.gain_r = 64UL;
            p_ctx->filters.input_param.after_ce_wb_gain.gain_b = 131072UL;
            p_ctx->filters.input_param.after_ce_wb_gain.gain_g = 131072UL;
            p_ctx->filters.input_param.after_ce_wb_gain.gain_r = 131072UL;

            p_ctx->filters.input_param.chroma_scale.enable = 0UL;
            p_ctx->filters.input_param.ce.enable = 0UL;
            p_ctx->filters.input_param.deferred_blc.enable = 0UL;
            p_ctx->filters.input_param.dgain_sauration_level.level_blue = 16383UL;
            p_ctx->filters.input_param.dgain_sauration_level.level_green_even = 16383UL;
            p_ctx->filters.input_param.dgain_sauration_level.level_green_odd = 16383UL;
            p_ctx->filters.input_param.dgain_sauration_level.level_red = 16383UL;
            p_ctx->filters.input_param.dzoom_info.enable = 0UL;
            p_ctx->filters.input_param.hdr_blend.enable = 0UL;
            //p_ctx->filters.input_param.hdr_input.compand_enable
            p_ctx->filters.input_param.fe_tone_curve.decompand_enable = 0UL;

            p_ctx->filters.input_param.advance_spatial_filter.enable = 0UL;
            p_ctx->filters.input_param.anti_aliasing.enable = 0UL;
            p_ctx->filters.input_param.dynamic_bpc.enable = 0UL;
            p_ctx->filters.input_param.cfa_leakage_filter.enable = 0UL;
            p_ctx->filters.input_param.cfa_noise_filter.enable = 0UL;
            p_ctx->filters.input_param.chroma_filter.enable = 0UL;
            p_ctx->filters.input_param.chroma_filter.radius = 32UL;
            p_ctx->filters.input_param.chroma_median_filter.enable = 0UL;
            p_ctx->filters.input_param.demosaic.enable = 0UL;
            p_ctx->filters.input_param.grgb_mismatch.narrow_enable = 0UL;
            p_ctx->filters.input_param.grgb_mismatch.wide_enable = 0UL;
            p_ctx->filters.input_param.first_luma_process_mode.use_sharpen_not_asf = 0UL;
            p_ctx->filters.input_param.first_sharpen_both.enable = 0UL;//li_sharpen_noise_filter_both

            p_ctx->filters.input_param.motion_detect.enable = 0UL;
            p_ctx->filters.input_param.video_mctf.enable = 0UL;
            p_ctx->filters.input_param.final_sharpen_both.enable = 0UL;

            p_ctx->filters.input_param.rgb_ir.mode = 0UL;

            p_ctx->filters.input_param.window_size_info.main_dim.height = 1080UL;
            p_ctx->filters.input_param.window_size_info.main_dim.width = 1920UL;
            p_ctx->filters.input_param.sensor_info.sensor_mode = 0UL;//system.raw_bayer 0
            p_ctx->filters.input_param.window_size_info.vin_sensor.height = 1080UL;//system.raw_height 1080
            //system.raw_pitch 3840
            p_ctx->filters.input_param.sensor_info.sensor_resolution = 14UL;//system.raw_resolution 14
            p_ctx->filters.input_param.window_size_info.vin_sensor.width = 1920UL; //system.raw_width 1920
            //system.tuning_mode IMG_MODE_VIDEO
            //system.raw_path null

            p_ctx->filters.input_param.sbp_enable = 0UL;
            p_ctx->filters.input_param.cawarp_enable = 0;
            p_ctx->filters.input_param.vig_enable = 0UL;
            p_ctx->filters.input_param.warp_enable = 0;

            p_ctx->filters.input_param.exp0_frontend_static_blc.black_b = 0;
            p_ctx->filters.input_param.exp0_frontend_static_blc.black_gb = 0;
            p_ctx->filters.input_param.exp0_frontend_static_blc.black_gr = 0;
            p_ctx->filters.input_param.exp0_frontend_static_blc.black_r = 0;

            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[0] = 2048;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[1] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[2] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[3] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[4] = 2048;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[5] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[6] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[7] = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.matrix_values[8] = 2048;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.u_offset = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.v_offset = 0;
            p_ctx->filters.input_param.rgb_to_yuv_matrix.y_offset = 0;

            p_ctx->filters.input_param.anti_aliasing.thresh = 0UL;
            p_ctx->filters.input_param.anti_aliasing.log_fractional_correct = 0UL;

            p_ctx->filters.input_param.grgb_mismatch.wide_safety = 0UL;
            p_ctx->filters.input_param.grgb_mismatch.wide_thresh = 0UL;
            p_ctx->filters.input_param.motion_detect_and_mctf.local_lighting_radius = 68UL;

        }
    }


    return rval;
}

static INLINE uint32 ctx_config_filter_default(uint32 context_id, const ik_ability_t *p_ability, amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value)
{
    uint32 rval;
    uint32 (*ctx_config_filter[AMBA_IK_VIDEO_MAX])(uint32 context_id, const amba_ik_context_entity_t *p_ctx, const ik_user_parameters_t *p_default_value);

    ctx_config_filter[0] = ctx_config_linear_filter;
    ctx_config_filter[1] = ctx_config_linear_ce_filter;
    ctx_config_filter[2] = ctx_config_hdr_2x_filter;
    ctx_config_filter[3] = ctx_config_hdr_3x_filter;
    ctx_config_filter[4] = ctx_config_linear_filter;
    ctx_config_filter[5] = ctx_config_linear_ce_filter;
    ctx_config_filter[6] = ctx_config_hdr_2x_filter;
    ctx_config_filter[7] = ctx_config_hdr_3x_filter;
    ctx_config_filter[8] = ctx_config_y2y_filter;
    ctx_config_filter[9] = ctx_config_linear_filter;
    ctx_config_filter[10] = ctx_config_linear_filter;
    ctx_config_filter[11] = ctx_config_linear_filter;
#if PRE_NN_PROCESS
    ctx_config_filter[12] = ctx_config_pre_nn_filter;
#endif

    rval = ctx_config_filter_baseline_default(context_id, p_ability, p_ctx);
    if((p_ability->video_pipe == (uint8)AMBA_IK_VIDEO_Y2Y_MIPI)) {
        //rval |= ctx_config_video_filter_invalid(context_id, p_ctx, p_default_value);
        //amba_ik_system_print_uint32_5("[IK] Pipe mipi does not support default_value", DC_U, DC_U, DC_U, DC_U, DC_U);
    } else if((rval == IK_OK) && (p_ability->video_pipe < (uint8)AMBA_IK_VIDEO_MAX) && ( p_default_value != NULL)) {
        rval |= ctx_config_filter[p_ability->video_pipe](context_id, p_ctx, p_default_value);
    } else {
        rval |= ctx_config_video_filter_invalid(context_id, p_ctx, p_default_value);
        //amba_ik_system_print_uint32_5("[IK]  default values are not initial in current pipe %d", video_pipe, DC_U, DC_U, DC_U, DC_U);
    }

    return rval;
}

uint32 img_ctx_query_context_memory_size(const ik_ability_t *ability, size_t *p_mem_size)
{
    uint32 rval = IK_OK;
    uint32 total_size;

    if (p_mem_size == NULL) {
        rval = IK_ERR_0005;
    } else {
        if(ability->pipe == AMBA_IK_PIPE_VIDEO) {
            *p_mem_size = sizeof(amba_ik_context_entity_t);
        } else {
            total_size = sizeof(amba_ik_context_entity_t);
            if(ability->still_pipe == AMBA_IK_STILL_HISO) {
                total_size += sizeof(amba_ik_hiso_input_parameters_t);
            }
            *p_mem_size = total_size;
        }
    }

    return rval;
}

uint32 img_ctx_prepare_context_memory(const ik_ability_t *ability, uint32 context_id, uintptr mem_addr, size_t mem_size)
{
    uint32 rval = IK_OK;
    uint32 context_number;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    context_number = img_arch_get_context_number();
    if ((ability == NULL) || (mem_addr == 0U) || (mem_size == (size_t)0) || (context_id > context_number)) {
        amba_ik_system_print_int32_5("[IK]  img_ctx_prepare_context_memory , mem_addr:%d", (int32)mem_addr, DC_I, DC_I, DC_I, DC_I);
        amba_ik_system_print_uint32_5("mem_size:%d, context_id:%d", mem_size, context_id, DC_U, DC_U, DC_U);
        rval = IK_ERR_0005;
    } else {
        //context_entity[context_id] = (amba_ik_context_entity_t *) mem_addr;
        (void)amba_ik_system_memcpy(&context_entity[context_id], &mem_addr, sizeof(amba_ik_context_entity_t *));
        if(ability->pipe == AMBA_IK_PIPE_STILL) {
            uintptr addr;
            addr = mem_addr + sizeof(amba_ik_context_entity_t);
            (void)amba_ik_system_memcpy(&context_entity[context_id]->filters.p_hiso_input_param, &addr, sizeof(amba_ik_hiso_input_parameters_t *));
        }
        rval = img_arch_get_ik_working_buffer(&p_ik_buffer_info);// Also record in ik buffer
        if(rval == IK_OK) {
            p_ik_buffer_info->context_entity[context_id] = mem_addr;
        }
    }

    return rval;
}

uint32 img_ctx_get_context(uint32 context_id, amba_ik_context_entity_t **p_ctx)
{
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        amba_ik_system_print_uint32_5("[IK]  img_ctx_get_context ERROR, context_id %d, img_arch_get_context_number() %d", context_id, img_arch_get_context_number(), DC_U, DC_U, DC_U);
        rval = IK_ERR_0003;
    } else {
        if(context_entity[context_id] == NULL) {
            amba_ik_system_print_uint32_5("[IK]  img_ctx_get_context ERROR, p_ctx %d is NULL", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0003;
        } else {
            *p_ctx = context_entity[context_id] ;
        }
    }
    return rval;
}

uint32 img_ctx_reset_iso_config_filter_update_flags(uint32 context_id)
{
    // TODO:
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        rval = IK_ERR_0003;
    } else {
        /* Common filter */
        (void)amba_ik_system_memset(&context_entity[context_id]->filters.update_flags.liso, 0x0, sizeof(amba_ik_filter_id_list_t));
        (void)amba_ik_system_memset(&context_entity[context_id]->filters.update_flags.hiso, 0x0, sizeof(amba_ik_filter_hiso_id_list_t));
        //context_entity[context_id]->filters.update_flags.liso.static_blc_updated = 0;
        //context_entity[context_id]->filters.update_flags.liso.exp0_fe_wb_gain_updated = 0;
        //context_entity[context_id]->filters.update_flags.liso.after_ce_wb_gain_updated = 0;
    }
    return rval;
}

static uint32 img_ctx_init_context_video(uint32 context_id, amba_ik_context_entity_t *p_ctx, const ik_ability_t *p_ability, const ik_user_parameters_t *p_default_value, uint32 init_mode)
{
    uint32 rval = IK_OK;

    if(p_ability->video_pipe < AMBA_IK_VIDEO_MAX) {
        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
        (void)amba_ik_system_memcpy(&p_ctx->organization.attribute.ability, p_ability, sizeof(ik_ability_t));
        ctx_get_filter_method((uint8)p_ability->video_pipe, &p_ctx->organization.filter_methods);
        if (init_mode == AMBA_IK_ARCH_HARD_INIT) {
            (void) amba_ik_system_memset(&p_ctx->organization.active_config_state, 0x0, sizeof(amba_ik_active_config_state_t));
            (void) amba_ik_system_memset(&p_ctx->filters.input_param, 0x0, sizeof(amba_ik_input_parameters_t));
            (void) amba_ik_system_memset(&p_ctx->filters.update_flags, 0x0, sizeof(amba_ik_filter_update_flags_t));
            // handle max_running_number
            // claim the max running number, in case we get wrong ring_idx when iso_running_number overflow
            p_ctx->organization.active_config_state.max_running_number = (uint16)(65535U- (65535U % p_ctx->organization.attribute.config_number));
        } else {
            (void)amba_ik_system_memset(&p_ctx->organization.active_config_state, 0x0, sizeof(amba_ik_active_config_state_t));
            p_ctx->filters.input_param.eis_mode = REGULAR_EXECUTE;
        }
        if ((p_ability->video_pipe == AMBA_IK_VIDEO_Y2Y) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_Y2Y_MD)) {
            p_ctx->filters.input_param.func_mode = (uint8)amba_ik_func_mode_y2y;
        }
        if ((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_Y2Y_MD)) {
            p_ctx->filters.input_param.func_mode |= (uint8)amba_ik_func_mode_md;
        }
        if ((p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_LINEAR_CE_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_2_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_HDR_EXPO_3_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE_MD) ||
            (p_ability->video_pipe == AMBA_IK_VIDEO_FUSION_CE)) {
            p_ctx->filters.input_param.func_mode |= (uint8)amba_ik_func_mode_ce;
        }
#if PRE_NN_PROCESS
        if ((p_ability->video_pipe == AMBA_IK_VIDEO_PRE_NN_PROCESS)) {
            p_ctx->filters.input_param.func_mode |= (uint8)amba_ik_func_mode_md;
        }
#endif
        p_ctx->organization.initial_flag = 1;
        rval |= ctx_config_filter_default(context_id, p_ability, p_ctx, p_default_value);
        p_ctx->organization.active_config_state.locked_iso_idx = 0xFFFFFFFFU;
        (void)amba_ik_system_unlock_mutex(context_id);
    } else {
        amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
        rval = IK_ERR_0008;
    }

    return rval;
}

static uint32 img_ctx_init_context_still(uint32 context_id, amba_ik_context_entity_t *p_ctx, const ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;
    (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER);
    (void)amba_ik_system_memcpy(&p_ctx->organization.attribute.ability, p_ability, sizeof(ik_ability_t));
    ctx_get_filter_method((uint8)p_ability->still_pipe, &p_ctx->organization.filter_methods);
    (void) amba_ik_system_memset(&p_ctx->filters.input_param, 0x0, sizeof(amba_ik_input_parameters_t));
    (void) amba_ik_system_memset(&p_ctx->filters.update_flags, 0x0, sizeof(amba_ik_filter_update_flags_t));
    if (p_ability->still_pipe == AMBA_IK_STILL_HISO) {
        if (p_ctx->filters.p_hiso_input_param == NULL) {
            amba_ik_system_print_str_5("[IK] p_hiso_input_param detect NULL pointer !", NULL, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0005;
        } else {
            (void) amba_ik_system_memset(p_ctx->filters.p_hiso_input_param, 0x0, sizeof(amba_ik_hiso_input_parameters_t));
        }
    }

    if (rval == IK_OK) {
        p_ctx->organization.initial_flag = 1;
        rval |= ctx_config_filter_default(context_id, p_ability, p_ctx, NULL);
        p_ctx->organization.active_config_state.locked_iso_idx = 0xFFFFFFFFU;
    }
    (void)amba_ik_system_unlock_mutex(context_id);
    return rval;
}

uint32 img_ctx_init_context(uint32 context_id, const ik_ability_t *p_ability, const ik_user_parameters_t *p_default_value, uint32 init_mode)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx;
    OS_MUTEX_TYPE  *p_mutex;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    char mutex_name[32] = "mutex_IK";

    if (p_ability == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] img_ctx_init_context() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    } else {
        amba_ik_system_print_uint32_5("[IK] img_ctx_init_context, ability->pipe:%d, ability->video_pipe:%d, ability->still_pipe:%d", (uint32)p_ability->pipe, p_ability->video_pipe, p_ability->still_pipe, DC_U, DC_U);
    }
    rval |= img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if(rval == IK_OK) {
        if(p_ik_buffer_info->init_flag == 1U) {
            rval = img_ctx_get_context(context_id, &p_ctx);
            if(rval == IK_OK) {

                if(p_ctx->organization.initial_flag == 0U) {
                    amba_ik_get_mutex(context_id, &p_mutex);
                    (void) amba_ik_system_memset(p_mutex, 0x0, sizeof(OS_MUTEX_TYPE ));
                    rval |= (uint32)amba_ik_system_create_mutex(p_mutex, mutex_name);
                    if (rval != IK_OK) {
                        amba_ik_system_print_uint32_5("[IK] amba_ik_system_create_mutex fail ctx:%d ", context_id, DC_U, DC_U, DC_U, DC_U);
                    }
                }

                if ((rval == IK_OK) && (p_ability->pipe == AMBA_IK_PIPE_VIDEO)) {
                    rval |= img_ctx_init_context_video(context_id, p_ctx, p_ability, p_default_value, init_mode);
                } else if ((rval == IK_OK) && (p_ability->pipe == AMBA_IK_PIPE_STILL)) {
                    rval |= img_ctx_init_context_still(context_id, p_ctx, p_ability);
                } else {
                    amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0004;
                }

            } else {
                amba_ik_system_print_uint32_5("[IK] IK get ctx %d fail", context_id, DC_U, DC_U, DC_U, DC_U);
            }

        } else {
            amba_ik_system_print_str_5("[IK] IK is not initial", NULL, NULL, NULL, NULL, NULL);
            rval = IK_ERR_0001;
        }
    }

    return rval;
}

uint32 img_ctx_get_context_ability(uint32 context_id, ik_ability_t *p_ability)
{
    uint32 rval = IK_OK;
    amba_ik_context_entity_t *p_ctx = NULL;
    if(p_ability == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] img_ctx_get_context_ability() detect NULL pointer !", NULL, NULL, NULL, NULL, NULL);
        rval = IK_ERR_0005;
    }
    rval |= img_ctx_get_context(context_id, &p_ctx);
    if ((rval == IK_OK) && (p_ctx->organization.initial_flag != 0U)) {
        (void)amba_ik_system_memcpy(p_ability, &p_ctx->organization.attribute.ability, sizeof(ik_ability_t));
    } else {
        if(p_ctx->organization.initial_flag == 0U) {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        } else {
            amba_ik_system_print_uint32_5("[IK] context_id %d, ability addr: is invalid", context_id, DC_U, DC_U, DC_U, DC_U);
        }
    }
    return rval;
}

uint32 img_ctx_get_iso_config_and_state(uint32 context_id, uint32 config_id, intptr *p_iso_config_and_state)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    if (p_iso_config_and_state == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            if (p_ctx->organization.attribute.config_number > config_id) {
                *p_iso_config_and_state = p_ctx->configs.iso_configs_and_states[config_id];
            } else {
                amba_ik_system_print_uint32_5("[IK] config_number: %d <= config_id: %d", p_ctx->organization.attribute.config_number, config_id, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD: Never happen
            }
        }
    }
    return rval;
}

uint32 img_ctx_get_iso_config_tables(uint32 context_id, uint32 config_id, intptr *p_iso_config_table)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    if (p_iso_config_table == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            if (p_ctx->organization.attribute.config_number > config_id) {
                *p_iso_config_table = p_ctx->configs.iso_tables[config_id];
            } else {
                amba_ik_system_print_uint32_5("[IK] config_number: %d <= config_id: %d", p_ctx->organization.attribute.config_number, config_id, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD: Never happen
            }
        }
    }
    return rval;
}

static INLINE uint32 ctx_generate_config_tag_context_id(uint32 context_id)
{
    return ((uint32)context_id & (uint32)0xFU) << 26;
}

INLINE uint32 ctx_parse_context_id(uint32 ik_id)
{
    return ((uint32)ik_id & (uint32)0x3C000000U) >> 26;
}

static INLINE uint32 ctx_generate_config_tag_pipe_id(uint8 pipe_id)
{
    return ((uint32)((pipe_id == AMBA_IK_PIPE_VIDEO) ? ((uint32)AMBA_IK_PIPE_VIDEO) : ((uint32)AMBA_IK_PIPE_STILL)) & (uint32)0x3U) << 24;
}

INLINE uint32 ctx_parse_pipe(uint32 ik_id)
{
    return ((uint32)ik_id & (uint32)0x3000000U) >> 24;
}

static INLINE uint32 ctx_generate_config_tag_ability_id(uint32 ability_id)
{
    return ((uint32)ability_id & (uint32)0x3FU) << 18;
}

INLINE uint32 ctx_parse_ability(uint32 ik_id)
{
    return ((uint32)ik_id & (uint32)0xFC0000U) >> 18;
}

static INLINE uint32 ctx_generate_config_tag_config_id_running_number(uint16 iso_running_number)
{
    return ((uint32)iso_running_number) & (uint32)0xFFFFU;
}

INLINE uint32 ctx_parse_running_number(uint32 ik_id)
{
    return (ik_id & 0x0000FFFFUL);
}

uint32 img_ctx_generate_config_tag(uint32 context_id)
{
    uint32 rval;
    uint32 config_tag = 0;
    amba_ik_context_entity_t *p_ctx;
    rval = img_ctx_get_context(context_id, &p_ctx);
    if (rval == IK_OK) {
        config_tag |= ctx_generate_config_tag_context_id(context_id);
        config_tag |= ctx_generate_config_tag_pipe_id(p_ctx->organization.attribute.ability.pipe);
        if( p_ctx->organization.attribute.ability.pipe == AMBA_IK_PIPE_VIDEO) {
            config_tag |= ctx_generate_config_tag_ability_id(p_ctx->organization.attribute.ability.video_pipe);
        } else {
            config_tag |= ctx_generate_config_tag_ability_id(p_ctx->organization.attribute.ability.still_pipe);
        }
        config_tag |= ctx_generate_config_tag_config_id_running_number(p_ctx->organization.active_config_state.iso_running_number);
    } else {
        amba_ik_system_print_uint32_5("[IK] call img_ctx_get_context(%d) fail", context_id, DC_U, DC_U, DC_U, DC_U);
        config_tag = CONFIG_TAG_UNKNOWN;
    }
    return config_tag;
}
#if 0
uint32 img_ctx_lock_iso_config(uint32 config_tag)
{
    uint32 rval=IK_OK;
    if (config_tag == CONFIG_TAG_UNKNOWN) {
        // misraC
    }

    // TODO:
    return rval;
}
#endif
uint32 img_ctx_get_explicit_running_num(uint32 context_id, uint32 **p_explicit_running_number)
{
    uint32 rval=IK_OK;
    static uint32 explicit_running_number[MAX_CONTEXT_NUM] = {0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL};
    static uint32 n_count = 0;
    uint32 i= 0 ;
    if(n_count == 0U) {
        for(i=0; i<MAX_CONTEXT_NUM; i++) {
            explicit_running_number[i] = 0xFFFFFFFFUL;
        }
        n_count++;
    }
    if (context_id > img_arch_get_context_number()) {
        amba_ik_system_print_uint32_5("[IK]  img_ctx_get_explicit_running_num ERROR, context_id %d, img_arch_get_context_number() %d", context_id, img_arch_get_context_number(), DC_U, DC_U, DC_U);
        rval = IK_ERR_0003;
    } else {
        *p_explicit_running_number = &explicit_running_number[context_id];
    }
    return rval;
}

uint32 img_ctx_get_explicit_iso_running_num(uint32 context_id, uint32 **p_explicit_iso_running_number)
{
    uint32 rval=IK_OK;
    static uint32 explicit_iso_running_number[MAX_CONTEXT_NUM] = {0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL,0xFFFFFFFFUL};
    static uint32 n_count = 0;
    uint32 i= 0 ;
    if(n_count == 0U) {
        for(i=0; i<MAX_CONTEXT_NUM; i++) {
            explicit_iso_running_number[i] = 0xFFFFFFFFUL;
        }
        n_count++;
    }
    if (context_id > img_arch_get_context_number()) {
        amba_ik_system_print_uint32_5("[IK]  img_ctx_get_explicit_iso_running_num ERROR, context_id %d, img_arch_get_context_number() %d", context_id, img_arch_get_context_number(), DC_U, DC_U, DC_U);
        rval = IK_ERR_0003;
    } else {
        *p_explicit_iso_running_number = &explicit_iso_running_number[context_id];
    }
    return rval;
}

