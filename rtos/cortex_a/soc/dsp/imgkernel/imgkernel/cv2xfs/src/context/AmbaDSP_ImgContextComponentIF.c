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

#include "AmbaDSP_ImgContextEntity.h"
#include "AmbaDSP_ImgContextComponentIF.h"
#include "AmbaDSP_ImgArchComponentIF.h"

static amba_ik_context_entity_t *context_entity[MAX_CONTEXT_NUM] = {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

static void img_ctx_hook_ivd_filter_method(amba_ik_filter_method_t *p_ivd_filter_method)
{
    /*safety information*/
    p_ivd_filter_method->set_safety_info = img_ctx_ivd_set_safety_info;
    p_ivd_filter_method->get_safety_info = img_ctx_ivd_get_safety_info;

    /*sensor information*/
    p_ivd_filter_method->set_vin_sensor_info = img_ctx_ivd_set_vin_sensor_info;
    p_ivd_filter_method->get_vin_sensor_info = img_ctx_ivd_get_vin_sensor_info;

    /* yuv mode */
    p_ivd_filter_method->set_yuv_mode = img_ctx_ivd_set_yuv_mode;
    p_ivd_filter_method->get_yuv_mode = img_ctx_ivd_get_yuv_mode;

    /*3a statistic*/
    p_ivd_filter_method->set_aaa_stat_info =  img_ctx_ivd_set_aaa_stat_info;
    p_ivd_filter_method->get_aaa_stat_info =  img_ctx_ivd_get_aaa_stat_info;

    p_ivd_filter_method->set_aaa_pg_af_info = img_ctx_ivd_set_aaa_pg_af_info;
    p_ivd_filter_method->get_aaa_pg_af_info = img_ctx_ivd_get_aaa_pg_af_info;

    p_ivd_filter_method->set_af_stat_ex_info =  img_ctx_ivd_set_af_stat_ex_info;
    p_ivd_filter_method->get_af_stat_ex_info =  img_ctx_ivd_get_af_stat_ex_info;

    p_ivd_filter_method->set_pg_af_stat_ex_info =  img_ctx_ivd_set_pg_af_stat_ex_info;
    p_ivd_filter_method->get_pg_af_stat_ex_info =  img_ctx_ivd_get_pg_af_stat_ex_info;

    /*calibration*/
    p_ivd_filter_method->set_window_size_info =  img_ctx_ivd_set_window_size_info;
    p_ivd_filter_method->get_window_size_info =  img_ctx_ivd_get_window_size_info;

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

    p_ivd_filter_method->set_pre_cc_gain =  img_ctx_ivd_set_pre_cc_gain;
    p_ivd_filter_method->get_pre_cc_gain =  img_ctx_ivd_get_pre_cc_gain;

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

    p_ivd_filter_method->set_demosaic =  img_ctx_ivd_set_demosaic;
    p_ivd_filter_method->get_demosaic =  img_ctx_ivd_get_demosaic;

    p_ivd_filter_method->set_rgb_to_12y =  img_ctx_ivd_set_rgb_to_12y;
    p_ivd_filter_method->get_rgb_to_12y =  img_ctx_ivd_get_rgb_to_12y;

    p_ivd_filter_method->set_luma_noise_reduce =  img_ctx_ivd_set_luma_noise_reduction;
    p_ivd_filter_method->get_luma_noise_reduce =  img_ctx_ivd_get_luma_noise_reduction;

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

    p_ivd_filter_method->set_wide_chroma_filter =  img_ctx_ivd_set_wide_chroma_ft;
    p_ivd_filter_method->get_wide_chroma_filter =  img_ctx_ivd_get_wide_chroma_ft;

    p_ivd_filter_method->set_wide_chroma_combine =  img_ctx_ivd_set_wide_chroma_ft_combine;
    p_ivd_filter_method->get_wide_chroma_combine =  img_ctx_ivd_get_wide_chroma_ft_combine;

    p_ivd_filter_method->set_video_mctf =  img_ctx_ivd_set_video_mctf;
    p_ivd_filter_method->get_video_mctf =  img_ctx_ivd_get_video_mctf;

    p_ivd_filter_method->set_video_mctf_ta =  img_ctx_ivd_set_mctf_ta;
    p_ivd_filter_method->get_video_mctf_ta =  img_ctx_ivd_get_mctf_ta;

    p_ivd_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_ivd_set_mctf_and_final_sharpen;
    p_ivd_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_ivd_get_mctf_and_final_sharpen;

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

    p_ivd_filter_method->set_ce =  img_ctx_ivd_set_ce;
    p_ivd_filter_method->get_ce =  img_ctx_ivd_get_ce;

    p_ivd_filter_method->set_ce_input_table =  img_ctx_ivd_set_ce_input_table;
    p_ivd_filter_method->get_ce_input_table =  img_ctx_ivd_get_ce_input_table;

    p_ivd_filter_method->set_ce_out_table =  img_ctx_ivd_set_ce_out_table;
    p_ivd_filter_method->get_ce_out_table =  img_ctx_ivd_get_ce_out_table;

    p_ivd_filter_method->set_hdr_blend =  img_ctx_ivd_set_hdr_blend;
    p_ivd_filter_method->get_hdr_blend =  img_ctx_ivd_get_hdr_blend;

    p_ivd_filter_method->set_resampler_str =  img_ctx_ivd_set_resampler_str;
    p_ivd_filter_method->get_resampler_str =  img_ctx_ivd_get_resampler_str;

    p_ivd_filter_method->set_histogram_info = img_ctx_ivd_set_histogram_info;
    p_ivd_filter_method->get_histogram_info = img_ctx_ivd_get_histogram_info;

    p_ivd_filter_method->set_pg_histogram_info = img_ctx_ivd_set_pg_histogram_info;
    p_ivd_filter_method->get_pg_histogram_info = img_ctx_ivd_get_pg_histogram_info;

    p_ivd_filter_method->set_flip_mode = img_ctx_ivd_set_flip_mode;
    p_ivd_filter_method->get_flip_mode = img_ctx_ivd_get_flip_mode;

    p_ivd_filter_method->set_warp_buffer_info = img_ctx_ivd_set_warp_buffer_info;
    p_ivd_filter_method->get_warp_buffer_info = img_ctx_ivd_get_warp_buffer_info;

    p_ivd_filter_method->set_stitching_info = img_ctx_ivd_set_stitching_info;
    p_ivd_filter_method->get_stitching_info = img_ctx_ivd_get_stitching_info;

    p_ivd_filter_method->set_burst_tile = img_ctx_ivd_set_burst_tile;
    p_ivd_filter_method->get_burst_tile = img_ctx_ivd_get_burst_tile;

}

void img_ctx_hook_safety_filter_method(amba_ik_filter_method_t *p_safety_filter_method)
{
    img_ctx_hook_ivd_filter_method(p_safety_filter_method);

    /*safety information*/
    p_safety_filter_method->set_safety_info = img_ctx_safety_set_safety_info;
    p_safety_filter_method->get_safety_info = img_ctx_safety_get_safety_info;

    /*sensor information*/
    p_safety_filter_method->set_vin_sensor_info = img_ctx_safety_set_vin_sensor_info;
    p_safety_filter_method->get_vin_sensor_info = img_ctx_safety_get_vin_sensor_info;

    /* yuv mode */
    p_safety_filter_method->set_yuv_mode = img_ctx_safety_set_yuv_mode;
    p_safety_filter_method->get_yuv_mode = img_ctx_safety_get_yuv_mode;

    /*3a statistic*/
    p_safety_filter_method->set_aaa_stat_info =  img_ctx_safety_set_aaa_stat_info;
    p_safety_filter_method->get_aaa_stat_info =  img_ctx_safety_get_aaa_stat_info;

    p_safety_filter_method->set_aaa_pg_af_info = img_ctx_safety_set_aaa_pg_af_info;
    p_safety_filter_method->get_aaa_pg_af_info = img_ctx_safety_get_aaa_pg_af_info;

    p_safety_filter_method->set_af_stat_ex_info =  img_ctx_safety_set_af_stat_ex_info;
    p_safety_filter_method->get_af_stat_ex_info =  img_ctx_safety_get_af_stat_ex_info;

    p_safety_filter_method->set_pg_af_stat_ex_info =  img_ctx_safety_set_pg_af_stat_ex_info;
    p_safety_filter_method->get_pg_af_stat_ex_info =  img_ctx_safety_get_pg_af_stat_ex_info;

    /*calibration*/
    p_safety_filter_method->set_window_size_info =  img_ctx_safety_set_window_size_info;
    p_safety_filter_method->get_window_size_info =  img_ctx_safety_get_window_size_info;

    p_safety_filter_method->set_static_bpc =  img_ctx_safety_set_static_bpc;
    p_safety_filter_method->get_static_bpc =  img_ctx_safety_get_static_bpc;
    p_safety_filter_method->set_static_bpc_internal =  img_ctx_safety_set_static_bpc_internal;
    p_safety_filter_method->get_static_bpc_internal =  img_ctx_safety_get_static_bpc_internal;
    p_safety_filter_method->set_static_bpc_enable_info = img_ctx_safety_set_static_bpc_enable_info;
    p_safety_filter_method->get_static_bpc_enable_info = img_ctx_safety_get_static_bpc_enable_info;

    p_safety_filter_method->set_vignette_compensation =  img_ctx_safety_set_vignette;
    p_safety_filter_method->get_vignette_compensation =  img_ctx_safety_get_vignette;
    p_safety_filter_method->set_vignette_enable_info = img_ctx_safety_set_vignette_enable_info;
    p_safety_filter_method->get_vignette_enable_info = img_ctx_safety_get_vignette_enable_info;

    p_safety_filter_method->set_calib_warp_info =  img_ctx_safety_set_calib_warp_info;
    p_safety_filter_method->get_calib_warp_info =  img_ctx_safety_get_calib_warp_info;

    p_safety_filter_method->set_calib_ca_warp_info =  img_ctx_safety_set_calib_ca_warp_info;
    p_safety_filter_method->get_calib_ca_warp_info =  img_ctx_safety_get_calib_ca_warp_info;

    p_safety_filter_method->set_dzoom_info =  img_ctx_safety_set_dzoom_info;
    p_safety_filter_method->get_dzoom_info =  img_ctx_safety_get_dzoom_info;

    p_safety_filter_method->set_warp_enable_info =  img_ctx_safety_set_warp_enable_info;
    p_safety_filter_method->get_warp_enable_info =  img_ctx_safety_get_warp_enable_info;

    p_safety_filter_method->set_cawarp_enable_info =  img_ctx_safety_set_cawarp_enable_info;
    p_safety_filter_method->get_cawarp_enable_info =  img_ctx_safety_get_cawarp_enable_info;

    p_safety_filter_method->set_dmy_range_info =  img_ctx_safety_set_dummy_win_margin_range_info;
    p_safety_filter_method->get_dmy_range_info =  img_ctx_safety_get_dummy_win_margin_range_info;

    p_safety_filter_method->set_warp_internal =  img_ctx_safety_set_warp_internal;
    p_safety_filter_method->get_warp_internal =  img_ctx_safety_get_warp_internal;

    p_safety_filter_method->set_ca_warp_internal =  img_ctx_safety_set_cawarp_internal;
    p_safety_filter_method->get_ca_warp_internal =  img_ctx_safety_get_cawarp_internal;

    p_safety_filter_method->set_active_win =  img_ctx_safety_set_vin_active_window;
    p_safety_filter_method->get_active_win =  img_ctx_safety_get_vin_active_window;

    /*normal filters*/
    p_safety_filter_method->set_before_ce_wb_gain =  img_ctx_safety_set_before_ce_wb_gain;
    p_safety_filter_method->get_before_ce_wb_gain =  img_ctx_safety_get_before_ce_wb_gain;

    p_safety_filter_method->set_after_ce_wb_gain =  img_ctx_safety_set_after_ce_wb_gain;
    p_safety_filter_method->get_after_ce_wb_gain =  img_ctx_safety_get_after_ce_wb_gain;

    p_safety_filter_method->set_pre_cc_gain =  img_ctx_safety_set_pre_cc_gain;
    p_safety_filter_method->get_pre_cc_gain =  img_ctx_safety_get_pre_cc_gain;

    p_safety_filter_method->set_cfa_leakage_filter =  img_ctx_safety_set_cfa_leakage_filter;
    p_safety_filter_method->get_cfa_leakage_filter =  img_ctx_safety_get_cfa_leakage_filter;

    p_safety_filter_method->set_anti_aliasing =  img_ctx_safety_set_anti_aliasing;
    p_safety_filter_method->get_anti_aliasing =  img_ctx_safety_get_anti_aliasing;

    p_safety_filter_method->set_dynamic_bpc =  img_ctx_safety_set_dynamic_bpc;
    p_safety_filter_method->get_dynamic_bpc =  img_ctx_safety_get_dynamic_bpc;

    p_safety_filter_method->set_grgb_mismatch =  img_ctx_safety_set_grgb_mismatch;
    p_safety_filter_method->get_grgb_mismatch =  img_ctx_safety_get_grgb_mismatch;

    p_safety_filter_method->set_cfa_noise_filter =  img_ctx_safety_set_cfa_noise_filter;
    p_safety_filter_method->get_cfa_noise_filter =  img_ctx_safety_get_cfa_noise_filter;

    p_safety_filter_method->set_demosaic =  img_ctx_safety_set_demosaic;
    p_safety_filter_method->get_demosaic =  img_ctx_safety_get_demosaic;

    p_safety_filter_method->set_rgb_to_12y =  img_ctx_safety_set_rgb_to_12y;
    p_safety_filter_method->get_rgb_to_12y =  img_ctx_safety_get_rgb_to_12y;

    p_safety_filter_method->set_luma_noise_reduce =  img_ctx_safety_set_luma_noise_reduction;
    p_safety_filter_method->get_luma_noise_reduce =  img_ctx_safety_get_luma_noise_reduction;

    p_safety_filter_method->set_color_correction =  img_ctx_safety_set_color_correction;
    p_safety_filter_method->get_color_correction =  img_ctx_safety_get_color_correction;

    p_safety_filter_method->set_tone_curve =  img_ctx_safety_set_tone_curve;
    p_safety_filter_method->get_tone_curve =  img_ctx_safety_get_tone_curve;

    p_safety_filter_method->set_rgb_to_yuv_matrix =  img_ctx_safety_set_rgb_to_yuv_matrix;
    p_safety_filter_method->get_rgb_to_yuv_matrix =  img_ctx_safety_get_rgb_to_yuv_matrix;

    p_safety_filter_method->set_rgb_ir = img_ctx_safety_set_rgb_ir;
    p_safety_filter_method->get_rgb_ir = img_ctx_safety_get_rgb_ir;

    p_safety_filter_method->set_chroma_scale =  img_ctx_safety_set_chroma_scale;
    p_safety_filter_method->get_chroma_scale =  img_ctx_safety_get_chroma_scale;

    p_safety_filter_method->set_chroma_median_filter =  img_ctx_safety_set_chroma_median_filter;
    p_safety_filter_method->get_chroma_median_filter =  img_ctx_safety_get_chroma_median_filter;

    p_safety_filter_method->set_first_luma_processing_mode =  img_ctx_safety_set_first_luma_processing_mode;
    p_safety_filter_method->get_first_luma_processing_mode =  img_ctx_safety_get_first_luma_processing_mode;

    p_safety_filter_method->set_advance_spatial_filter =  img_ctx_safety_set_advance_spatial_filter;
    p_safety_filter_method->get_advance_spatial_filter =  img_ctx_safety_get_advance_spatial_filter;

    p_safety_filter_method->set_fstshpns_both =  img_ctx_safety_set_fstshpns_both;
    p_safety_filter_method->get_fstshpns_both =  img_ctx_safety_get_fstshpns_both;

    p_safety_filter_method->set_fstshpns_noise =  img_ctx_safety_set_fstshpns_noise;
    p_safety_filter_method->get_fstshpns_noise =  img_ctx_safety_get_fstshpns_noise;

    p_safety_filter_method->set_fstshpns_fir =  img_ctx_safety_set_fstshpns_fir;
    p_safety_filter_method->get_fstshpns_fir =  img_ctx_safety_get_fstshpns_fir;

    p_safety_filter_method->set_fstshpns_coring =  img_ctx_safety_set_fstshp_coring;
    p_safety_filter_method->get_fstshpns_coring =  img_ctx_safety_get_fstshp_coring;

    p_safety_filter_method->set_fstshpns_coring_index_scale =  img_ctx_safety_set_fstshp_cor_idx_scl;
    p_safety_filter_method->get_fstshpns_coring_index_scale =  img_ctx_safety_get_fstshp_cor_idx_scl;

    p_safety_filter_method->set_fstshpns_min_coring_result =  img_ctx_safety_set_fstshpns_min_coring_result;
    p_safety_filter_method->get_fstshpns_min_coring_result =  img_ctx_safety_get_fstshpns_min_coring_result;

    p_safety_filter_method->set_fstshpns_max_coring_result =  img_ctx_safety_set_fstshpns_max_coring_result;
    p_safety_filter_method->get_fstshpns_max_coring_result =  img_ctx_safety_get_fstshpns_max_coring_result;

    p_safety_filter_method->set_fstshpns_scale_coring =  img_ctx_safety_set_fstshpns_scale_coring;
    p_safety_filter_method->get_fstshpns_scale_coring =  img_ctx_safety_get_fstshpns_scale_coring;

    p_safety_filter_method->set_fnlshpns_both =  img_ctx_safety_set_fnlshp_both;
    p_safety_filter_method->get_fnlshpns_both =  img_ctx_safety_get_fnlshp_both;

    p_safety_filter_method->set_fnlshpns_noise =  img_ctx_safety_set_fnlshpns_noise;
    p_safety_filter_method->get_fnlshpns_noise =  img_ctx_safety_get_fnlshpns_noise;

    p_safety_filter_method->set_fnlshpns_fir =  img_ctx_safety_set_fnlshpns_fir;
    p_safety_filter_method->get_fnlshpns_fir =  img_ctx_safety_get_fnlshpns_fir;

    p_safety_filter_method->set_fnlshpns_coring =  img_ctx_safety_set_fnlshp_coring;
    p_safety_filter_method->get_fnlshpns_coring =  img_ctx_safety_get_fnlshp_coring;

    p_safety_filter_method->set_fnlshpns_coring_index_scale =  img_ctx_safety_set_fnlshp_cor_idx_scl;
    p_safety_filter_method->get_fnlshpns_coring_index_scale =  img_ctx_safety_get_fnlshp_cor_idx_scl;

    p_safety_filter_method->set_fnlshpns_min_coring_result =  img_ctx_safety_set_fnlshpns_min_coring_result;
    p_safety_filter_method->get_fnlshpns_min_coring_result =  img_ctx_safety_get_fnlshpns_min_coring_result;

    p_safety_filter_method->set_fnlshpns_max_coring_result =  img_ctx_safety_set_fnlshpns_max_coring_result;
    p_safety_filter_method->get_fnlshpns_max_coring_result =  img_ctx_safety_get_fnlshpns_max_coring_result;

    p_safety_filter_method->set_fnlshpns_scale_coring =  img_ctx_safety_set_fnlshpns_scale_coring;
    p_safety_filter_method->get_fnlshpns_scale_coring =  img_ctx_safety_get_fnlshpns_scale_coring;

    p_safety_filter_method->set_fnlshpns_both_tdt =  img_ctx_safety_set_fnlshp_both_tdt;
    p_safety_filter_method->get_fnlshpns_both_tdt =  img_ctx_safety_get_fnlshp_both_tdt;

    p_safety_filter_method->set_chroma_filter =  img_ctx_safety_set_chroma_filter;
    p_safety_filter_method->get_chroma_filter =  img_ctx_safety_get_chroma_filter;

    p_safety_filter_method->set_wide_chroma_filter =  img_ctx_safety_set_w_chroma_ft;
    p_safety_filter_method->get_wide_chroma_filter =  img_ctx_safety_get_w_chroma_ft;

    p_safety_filter_method->set_wide_chroma_combine =  img_ctx_safety_set_w_chroma_ft_combine;
    p_safety_filter_method->get_wide_chroma_combine =  img_ctx_safety_get_w_chroma_ft_combine;

    p_safety_filter_method->set_video_mctf =  img_ctx_safety_set_video_mctf;
    p_safety_filter_method->get_video_mctf =  img_ctx_safety_get_video_mctf;

    p_safety_filter_method->set_video_mctf_ta =  img_ctx_safety_set_mctf_ta;
    p_safety_filter_method->get_video_mctf_ta =  img_ctx_safety_get_mctf_ta;

    p_safety_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_safety_set_mctf_and_final_sharpen;
    p_safety_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_safety_get_mctf_and_final_sharpen;

    p_safety_filter_method->set_front_end_tone_curve = img_ctx_safety_set_fe_tone_curve;
    p_safety_filter_method->get_front_end_tone_curve = img_ctx_safety_get_fe_tone_curve;

    /*hdr*/
    p_safety_filter_method->set_hdr_raw_offset = img_ctx_safety_set_hdr_raw_offset;
    p_safety_filter_method->get_hdr_raw_offset = img_ctx_safety_get_hdr_raw_offset;

    p_safety_filter_method->set_exp0_fe_static_blc =  img_ctx_safety_set_exp0_fe_static_blc;
    p_safety_filter_method->get_exp0_fe_static_blc =  img_ctx_safety_get_exp0_fe_static_blc;

    p_safety_filter_method->set_exp1_fe_static_blc =  img_ctx_safety_set_exp1_fe_static_blc;
    p_safety_filter_method->get_exp1_fe_static_blc =  img_ctx_safety_get_exp1_fe_static_blc;

    p_safety_filter_method->set_exp2_fe_static_blc =  img_ctx_safety_set_exp2_fe_static_blc;
    p_safety_filter_method->get_exp2_fe_static_blc =  img_ctx_safety_get_exp2_fe_static_blc;

    p_safety_filter_method->set_exp0_fe_wb_gain =  img_ctx_safety_set_exp0_fe_wb_gain;
    p_safety_filter_method->get_exp0_fe_wb_gain =  img_ctx_safety_get_exp0_fe_wb_gain;

    p_safety_filter_method->set_exp1_fe_wb_gain =  img_ctx_safety_set_exp1_fe_wb_gain;
    p_safety_filter_method->get_exp1_fe_wb_gain =  img_ctx_safety_get_exp1_fe_wb_gain;

    p_safety_filter_method->set_exp2_fe_wb_gain =  img_ctx_safety_set_exp2_fe_wb_gain;
    p_safety_filter_method->get_exp2_fe_wb_gain =  img_ctx_safety_get_exp2_fe_wb_gain;

    p_safety_filter_method->set_ce =  img_ctx_safety_set_ce;
    p_safety_filter_method->get_ce =  img_ctx_safety_get_ce;

    p_safety_filter_method->set_ce_input_table =  img_ctx_safety_set_ce_input_table;
    p_safety_filter_method->get_ce_input_table =  img_ctx_safety_get_ce_input_table;

    p_safety_filter_method->set_ce_out_table =  img_ctx_safety_set_ce_out_table;
    p_safety_filter_method->get_ce_out_table =  img_ctx_safety_get_ce_out_table;

    p_safety_filter_method->set_hdr_blend =  img_ctx_safety_set_hdr_blend;
    p_safety_filter_method->get_hdr_blend =  img_ctx_safety_get_hdr_blend;

    p_safety_filter_method->set_resampler_str =  img_ctx_safety_set_resampler_str;
    p_safety_filter_method->get_resampler_str =  img_ctx_safety_get_resampler_str;

    p_safety_filter_method->set_histogram_info = img_ctx_safety_set_histogram_info;
    p_safety_filter_method->get_histogram_info = img_ctx_safety_get_histogram_info;

    p_safety_filter_method->set_pg_histogram_info = img_ctx_safety_set_pg_histogram_info;
    p_safety_filter_method->get_pg_histogram_info = img_ctx_safety_get_pg_histogram_info;

    p_safety_filter_method->set_flip_mode = img_ctx_safety_set_flip_mode;
    p_safety_filter_method->get_flip_mode = img_ctx_safety_get_flip_mode;

    p_safety_filter_method->set_warp_buffer_info = img_ctx_safety_set_warp_buffer_info;
    p_safety_filter_method->get_warp_buffer_info = img_ctx_safety_get_warp_buffer_info;

    p_safety_filter_method->set_stitching_info = img_ctx_safety_set_stitching_info;
    p_safety_filter_method->get_stitching_info = img_ctx_safety_get_stitching_info;

    p_safety_filter_method->set_burst_tile = img_ctx_safety_set_burst_tile;
    p_safety_filter_method->get_burst_tile = img_ctx_safety_get_burst_tile;

}

static void img_ctx_hook_filter_method(uint8 video_pipe, amba_ik_filter_method_t *p_filter_method)
{
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    (void)img_arch_get_ik_working_buffer(&p_ik_buffer_info);

    img_ctx_hook_ivd_filter_method(p_filter_method);

    if(p_ik_buffer_info->safety_enable == 1u) {
        /*safety information*/
        p_filter_method->set_safety_info = img_ctx_set_safety_info;
        p_filter_method->get_safety_info = img_ctx_get_safety_info;
    }

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        /*sensor information*/
        p_filter_method->set_vin_sensor_info = img_ctx_set_vin_sensor_info;
        p_filter_method->get_vin_sensor_info = img_ctx_get_vin_sensor_info;

        /*3a statistic*/
        p_filter_method->set_af_stat_ex_info =  img_ctx_set_af_stat_ex_info;
        p_filter_method->get_af_stat_ex_info =  img_ctx_get_af_stat_ex_info;

        p_filter_method->set_histogram_info =  img_ctx_set_histogram_info;
        p_filter_method->get_histogram_info =  img_ctx_get_histogram_info;
    }

    p_filter_method->set_resampler_str =  img_ctx_set_resampler_str;
    p_filter_method->get_resampler_str =  img_ctx_get_resampler_str;

    p_filter_method->set_aaa_stat_info =  img_ctx_set_aaa_stat_info;
    p_filter_method->get_aaa_stat_info =  img_ctx_get_aaa_stat_info;

    p_filter_method->set_aaa_pg_af_info = img_ctx_set_aaa_pg_af_info;
    p_filter_method->get_aaa_pg_af_info = img_ctx_get_aaa_pg_af_info;

    p_filter_method->set_pg_af_stat_ex_info =  img_ctx_set_pg_af_stat_ex_info;
    p_filter_method->get_pg_af_stat_ex_info =  img_ctx_get_pg_af_stat_ex_info;

    p_filter_method->set_pg_histogram_info =  img_ctx_set_pg_histogram_info;
    p_filter_method->get_pg_histogram_info =  img_ctx_get_pg_histogram_info;

    /*calibration*/
    p_filter_method->set_window_size_info =  img_ctx_set_window_size_info;
    p_filter_method->get_window_size_info =  img_ctx_get_window_size_info;

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

    p_filter_method->set_warp_internal =  img_ctx_set_warp_internal;
    p_filter_method->get_warp_internal =  img_ctx_get_warp_internal;

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

        p_filter_method->set_ca_warp_internal =  img_ctx_set_cawarp_internal;
        p_filter_method->get_ca_warp_internal =  img_ctx_get_cawarp_internal;
    }

    /*normal filters*/
    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        p_filter_method->set_before_ce_wb_gain =  img_ctx_set_before_ce_wb_gain;
        p_filter_method->get_before_ce_wb_gain =  img_ctx_get_before_ce_wb_gain;

        p_filter_method->set_after_ce_wb_gain =  img_ctx_set_after_ce_wb_gain;
        p_filter_method->get_after_ce_wb_gain =  img_ctx_get_after_ce_wb_gain;

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

        p_filter_method->set_demosaic =  img_ctx_set_demosaic;
        p_filter_method->get_demosaic =  img_ctx_get_demosaic;

        p_filter_method->set_rgb_to_12y =  img_ctx_set_rgb_to_12y;
        p_filter_method->get_rgb_to_12y =  img_ctx_get_rgb_to_12y;
    }

    p_filter_method->set_luma_noise_reduce =  img_ctx_set_luma_noise_reduction;
    p_filter_method->get_luma_noise_reduce =  img_ctx_get_luma_noise_reduction;

    p_filter_method->set_flip_mode = img_ctx_set_flip_mode;
    p_filter_method->get_flip_mode = img_ctx_get_flip_mode;

    p_filter_method->set_warp_buffer_info = img_ctx_set_warp_buffer_info;
    p_filter_method->get_warp_buffer_info = img_ctx_get_warp_buffer_info;

    p_filter_method->set_stitching_info = img_ctx_set_stitching_info;
    p_filter_method->get_stitching_info = img_ctx_get_stitching_info;

    p_filter_method->set_burst_tile = img_ctx_set_burst_tile;
    p_filter_method->get_burst_tile = img_ctx_get_burst_tile;

    if (video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y) {
        p_filter_method->set_rgb_ir = img_ctx_set_rgb_ir;
        p_filter_method->get_rgb_ir = img_ctx_get_rgb_ir;
    }

    p_filter_method->set_pre_cc_gain =  img_ctx_set_pre_cc_gain;
    p_filter_method->get_pre_cc_gain =  img_ctx_get_pre_cc_gain;

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

    p_filter_method->set_wide_chroma_filter =  img_ctx_set_wide_chroma_filter;
    p_filter_method->get_wide_chroma_filter =  img_ctx_get_wide_chroma_filter;

    p_filter_method->set_wide_chroma_combine =  img_ctx_set_wide_chroma_filter_combine;
    p_filter_method->get_wide_chroma_combine =  img_ctx_get_wide_chroma_filter_combine;

    p_filter_method->set_video_mctf =  img_ctx_set_video_mctf;
    p_filter_method->get_video_mctf =  img_ctx_get_video_mctf;

    p_filter_method->set_video_mctf_ta =  img_ctx_set_video_mctf_ta;
    p_filter_method->get_video_mctf_ta =  img_ctx_get_video_mctf_ta;

    p_filter_method->set_video_mctf_and_final_sharpen =  img_ctx_set_video_mctf_and_final_sharpen;
    p_filter_method->get_video_mctf_and_final_sharpen =  img_ctx_get_video_mctf_and_final_sharpen;

    if ((video_pipe == (uint8)AMBA_IK_VIDEO_Y2Y)) {
        /* yuv mode */
        p_filter_method->set_yuv_mode = img_ctx_set_yuv_mode;
        p_filter_method->get_yuv_mode = img_ctx_get_yuv_mode;
    }

    if ((video_pipe != (uint8)AMBA_IK_VIDEO_Y2Y)) {
        p_filter_method->set_front_end_tone_curve = img_ctx_set_fe_tone_curve;
        p_filter_method->get_front_end_tone_curve = img_ctx_get_fe_tone_curve;

        p_filter_method->set_exp0_fe_static_blc =  img_ctx_set_exp0_fe_static_blc;
        p_filter_method->get_exp0_fe_static_blc =  img_ctx_get_exp0_fe_static_blc;

        p_filter_method->set_exp0_fe_wb_gain =  img_ctx_set_exp0_fe_wb_gain;
        p_filter_method->get_exp0_fe_wb_gain =  img_ctx_get_exp0_fe_wb_gain;
    }

    /*CE*/
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_LINEAR_CE) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3)) {
        p_filter_method->set_ce =  img_ctx_set_ce;
        p_filter_method->get_ce =  img_ctx_get_ce;

        p_filter_method->set_ce_input_table =  img_ctx_set_ce_input_table;
        p_filter_method->get_ce_input_table =  img_ctx_get_ce_input_table;

        p_filter_method->set_ce_out_table =  img_ctx_set_ce_out_table;
        p_filter_method->get_ce_out_table =  img_ctx_get_ce_out_table;
    }

    /*hdr*/
    if ((video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_2) || (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3)) {
        p_filter_method->set_hdr_raw_offset = img_ctx_set_hdr_raw_offset;
        p_filter_method->get_hdr_raw_offset = img_ctx_get_hdr_raw_offset;

        p_filter_method->set_exp1_fe_static_blc =  img_ctx_set_exp1_fe_static_blc;
        p_filter_method->get_exp1_fe_static_blc =  img_ctx_get_exp1_fe_static_blc;

        p_filter_method->set_exp1_fe_wb_gain =  img_ctx_set_exp1_fe_wb_gain;
        p_filter_method->get_exp1_fe_wb_gain =  img_ctx_get_exp1_fe_wb_gain;

        p_filter_method->set_hdr_blend =  img_ctx_set_hdr_blend;
        p_filter_method->get_hdr_blend =  img_ctx_get_hdr_blend;
    }

    if (video_pipe == (uint8)AMBA_IK_VIDEO_HDR_EXPO_3) {
        p_filter_method->set_exp2_fe_static_blc =  img_ctx_set_exp2_fe_static_blc;
        p_filter_method->get_exp2_fe_static_blc =  img_ctx_get_exp2_fe_static_blc;

        p_filter_method->set_exp2_fe_wb_gain =  img_ctx_set_exp2_fe_wb_gain;
        p_filter_method->get_exp2_fe_wb_gain =  img_ctx_get_exp2_fe_wb_gain;
    }
}

static INLINE void ctx_get_filter_method(uint8 video_pipe, amba_ik_filter_method_t* p_filter_method)
{
    img_ctx_hook_filter_method(video_pipe, p_filter_method);
}

static INLINE uint32 ctx_config_filter_baseline_default(uint8 video_pipe, amba_ik_context_entity_t *p_ctx)
{
    uint32 rval = IK_OK;

    if (video_pipe == 0U) {
        // misraC fix
    }

    p_ctx->filters.update_flags.iso.is_1st_frame = 1;
    p_ctx->filters.input_param.safety_info.update_freq = 1;
    p_ctx->filters.update_flags.iso.aaa_stat_info_updated = 1;

    p_ctx->filters.input_param.aaa_stat_info.awb_tile_num_col = 64UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_num_row = 64UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_col_start = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_row_start = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_width = 64UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_height = 64UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_active_width = 64UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_tile_active_height = 64UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_pix_min_value = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.awb_pix_max_value = 16383UL;

    p_ctx->filters.input_param.aaa_stat_info.ae_tile_num_col = 24UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_tile_num_row = 16UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_tile_col_start = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_tile_row_start = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_tile_width = 170UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_tile_height = 256UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_pix_min_value = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.ae_pix_max_value = 16383UL;

    p_ctx->filters.input_param.aaa_stat_info.af_tile_num_col = 24UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_num_row = 16UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_col_start = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_row_start = 0UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_width = 170UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_height = 256UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_active_width = 170UL;
    p_ctx->filters.input_param.aaa_stat_info.af_tile_active_height = 256UL;

    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_num_col = 32UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_num_row = 16UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_col_start = 0UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_row_start = 0UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_width = 128UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_height = 256UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_active_width = 128UL;
    p_ctx->filters.input_param.aaa_pg_stat_info.af_tile_active_height = 256UL;

    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_stage1_enb = 1UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_stage2_enb = 1UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_stage3_enb = 1UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_stage1_enb = 1UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_stage2_enb = 1UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_stage3_enb = 1UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_thresh = 50UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_tile_fv1_horizontal_shift = 8UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_tile_fv2_horizontal_shift = 8UL;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[0] = 188;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[1] = 467;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[2] = -235;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[3] = 375;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[4] = -184;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[5] = 276;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_gain[6] = -206;

    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_shift[0] = 7;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_shift[1] = 2;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_shift[2] = 2;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter1_shift[3] = 0;

    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[0] = 188;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[1] = 467;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[2] = -235;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[3] = 375;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[4] = -184;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[5] = 276;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_gain[6] = -206;

    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_shift[0] = 7;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_shift[1] = 2;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_shift[2] = 2;
    p_ctx->filters.input_param.af_stat_ex_info.af_horizontal_filter2_shift[3] = 0;

    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_stage1_enb = 1UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_stage2_enb = 1UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_stage3_enb = 1UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_stage1_enb = 1UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_stage2_enb = 1UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_stage3_enb = 1UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_vertical_filter1_thresh = 100UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_tile_fv1_horizontal_shift = 7UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_tile_fv1_horizontal_weight = 255UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_vertical_filter2_thresh = 100UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_tile_fv2_horizontal_shift = 7UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_tile_fv2_horizontal_weight = 255UL;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[0] = 188;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[1] = 467;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[2] = -235;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[3] = 375;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[4] = -184;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[5] = 276;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_gain[6] = -206;

    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_shift[0] = 7;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_shift[1] = 2;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_shift[2] = 2;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter1_shift[3] = 0;

    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[0] = 188;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[1] = 467;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[2] = -235;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[3] = 375;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[4] = -184;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[5] = 276;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_gain[6] = -206;

    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_shift[0] = 7;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_shift[1] = 2;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_shift[2] = 2;
    p_ctx->filters.input_param.pg_af_stat_ex_info.af_horizontal_filter2_shift[3] = 0;

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

    p_ctx->filters.input_param.hist_info.ae_tile_mask[0] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[1] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[2] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[3] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[4] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[5] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[6] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[7] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[8] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[9] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[10] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[11] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[12] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[13] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[14] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info.ae_tile_mask[15] = 0xFFFFFF;

    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[0] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[1] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[2] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[3] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[4] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[5] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[6] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[7] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[8] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[9] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[10] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[11] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[12] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[13] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[14] = 0xFFFFFF;
    p_ctx->filters.input_param.hist_info_pg.ae_tile_mask[15] = 0xFFFFFF;

    p_ctx->filters.input_param.warp_buf_info.dram_efficiency = 0u;
#if defined(EARLYTEST_ENV)
    p_ctx->filters.input_param.warp_buf_info.luma_wait_lines = 8u;
    p_ctx->filters.input_param.warp_buf_info.luma_dma_size = 32u;
#else
    p_ctx->filters.input_param.warp_buf_info.luma_wait_lines = 8u;
    p_ctx->filters.input_param.warp_buf_info.luma_dma_size = 24u;
#endif

    if (video_pipe != (uint8) AMBA_IK_VIDEO_Y2Y) {
        p_ctx->filters.update_flags.iso.histogram_info_update = 1;
    }
    p_ctx->filters.update_flags.iso.histogram_info_pg_update = 1;
    p_ctx->filters.input_param.use_cc_for_yuv2yuv = 0;
    p_ctx->filters.input_param.yuv_mode = 1;

    p_ctx->filters.input_param.stitching_info.enable = 0;
    p_ctx->filters.input_param.stitching_info.tile_num_x = 1;
    p_ctx->filters.input_param.stitching_info.tile_num_y = 1;

    p_ctx->filters.input_param.burst_tile.enable = 0;

    //for y2y lnl default rgb ratio.
    p_ctx->filters.input_param.after_ce_wb_gain.gain_r = 4096UL;
    p_ctx->filters.input_param.after_ce_wb_gain.gain_g = 4096UL;
    p_ctx->filters.input_param.after_ce_wb_gain.gain_b = 4096UL;
    //for y2y default rgb_to_yuv matrix.
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

    p_ctx->filters.input_param.rgb_to_12y.enable = 1;
    p_ctx->filters.input_param.rgb_to_12y.matrix_values[0] = 218;
    p_ctx->filters.input_param.rgb_to_12y.matrix_values[1] = 732;
    p_ctx->filters.input_param.rgb_to_12y.matrix_values[2] = 74;
    p_ctx->filters.input_param.rgb_to_12y.y_offset = 0;
    //for y2y lnl default disable.
    p_ctx->filters.input_param.luma_noise_reduce.enable = 0;
    p_ctx->filters.input_param.luma_noise_reduce.sensor_wb_b = 0;
    p_ctx->filters.input_param.luma_noise_reduce.sensor_wb_g = 0;
    p_ctx->filters.input_param.luma_noise_reduce.sensor_wb_r = 0;
    p_ctx->filters.input_param.luma_noise_reduce.strength0 = 100UL;
    p_ctx->filters.input_param.luma_noise_reduce.strength1 = 128UL;
    p_ctx->filters.input_param.luma_noise_reduce.strength2 = 0;
    p_ctx->filters.input_param.luma_noise_reduce.strength2_max_change = 0;

    return rval;
}

uint32 img_ctx_query_context_memory_size(size_t *p_mem_size)
{
    uint32 rval = IK_OK;

    if (p_mem_size == NULL) {
        rval = IK_ERR_0005;
    } else {
        *p_mem_size = sizeof(amba_ik_context_entity_t) + 512u; // extra 512 bytes for alignment
    }

    return rval;
}

uint32 img_ctx_prepare_context_memory(uint32 context_id, uintptr mem_addr, size_t mem_size)
{
    uint32 rval = IK_OK;
    uint32 context_number;
    ik_buffer_info_t *p_ik_buffer_info = NULL;

    context_number = img_arch_get_context_number();
    if ((mem_addr == 0U) || (mem_size == (size_t)0) || (context_id > context_number)) {
        amba_ik_system_print_int32_5("[IK]  img_ctx_prepare_context_memory , mem_addr:%d", (int32)mem_addr, DC_I, DC_I, DC_I, DC_I);
        amba_ik_system_print_uint32_5("mem_size:%d, context_id:%d", mem_size, context_id, DC_U, DC_U, DC_U);
        rval = IK_ERR_0005;
    } else {
        (void)amba_ik_system_memcpy(&context_entity[context_id], &mem_addr, sizeof(amba_ik_context_entity_t *));

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
        // Jack, I suppose this condition would never happen.
        // because we already check context_id is inside valid scope by (context_id > img_arch_get_context_number())
        // all of the context_entity[context_id] inside the scope of p_ik_working_buffer_info->number_of_context should be not NULL
        // vice versa, all of the context_entity[context_id] NOT inside the scope of p_ik_working_buffer_info->number_of_context should be NULL
#if 0
        if(context_entity[context_id] == NULL) {
            amba_ik_system_print_uint32_5("[IK]  img_ctx_get_context ERROR, p_ctx %d is NULL", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0003;
        } else
#endif
        {
            *p_ctx = context_entity[context_id] ;
        }
    }
    return rval;
}

uint32 img_ctx_reset_update_flags(uint32 context_id)
{
    uint32 rval = IK_OK;

    if (context_id > img_arch_get_context_number()) {
        rval = IK_ERR_0003;
    } else {
        (void)amba_ik_system_memset(&context_entity[context_id]->filters.update_flags, 0x0, sizeof(amba_ik_filter_update_flags_t));
    }
    return rval;
}

#if SUPPORT_CR_MEMORY_FENCE
static INLINE uint32 img_ctx_init_memory_fence(amba_ik_context_entity_t *p_ctx)
{
    uint32 rval = IK_OK;
    uint32 fence = 0x58585858UL;

    p_ctx->filters.input_param.ctx_fence_0 = fence;
    p_ctx->filters.input_param.ctx_fence_1 = fence;
    p_ctx->filters.input_param.ctx_fence_2 = fence;
    p_ctx->filters.input_param.ctx_fence_3 = fence;
    p_ctx->filters.input_param.ctx_fence_4 = fence;
    p_ctx->filters.input_param.ctx_fence_5 = fence;
    p_ctx->filters.input_param.ctx_fence_6 = fence;
    p_ctx->filters.input_param.ctx_fence_7 = fence;
    p_ctx->filters.input_param.ctx_fence_8 = fence;
    p_ctx->filters.input_param.ctx_fence_9 = fence;
    p_ctx->filters.input_param.ctx_fence_10 = fence;
    p_ctx->filters.input_param.ctx_fence_11 = fence;
    p_ctx->filters.input_param.ctx_fence_12 = fence;
    p_ctx->filters.input_param.ctx_fence_13 = fence;
    p_ctx->filters.input_param.ctx_fence_14 = fence;
    p_ctx->filters.input_param.ctx_fence_15 = fence;
    p_ctx->filters.input_param.ctx_fence_16 = fence;
    p_ctx->filters.input_param.ctx_fence_17 = fence;
    p_ctx->filters.input_param.ctx_fence_18 = fence;
    p_ctx->filters.input_param.ctx_fence_19 = fence;

    return rval;
}
#endif

uint32 img_ctx_init_context(uint32 context_id, const ik_ability_t *p_ability)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;
    OS_MUTEX_TYPE *p_mutex;
    ik_buffer_info_t *p_ik_buffer_info = NULL;
    char mutex_name[32] = "mutex_IK";

    rval = img_arch_get_ik_working_buffer(&p_ik_buffer_info);
    if (p_ability == NULL) {
        amba_ik_system_print_str_5("[IK][ERROR] img_ctx_init_context() detect NULL pointer", NULL, NULL, NULL, NULL, NULL);
        rval |= IK_ERR_0005;
    }
    if(rval == IK_OK) {
        if((p_ik_buffer_info->init_flag == 1u) && (p_ik_buffer_info->p_bin_data_dram_addr != NULL)) {
            rval = img_ctx_get_context(context_id, &p_ctx);
            if(rval == IK_OK) {
                p_ctx->organization.p_bin_data_dram_addr = p_ik_buffer_info->p_bin_data_dram_addr;
                if(p_ctx->organization.initial_flag == 0U) {
                    amba_ik_get_mutex(context_id, &p_mutex, 0u);
                    (void) amba_ik_system_memset(p_mutex, 0x0, sizeof(OS_MUTEX_TYPE));
                    rval |= (uint32)amba_ik_system_create_mutex(p_mutex, mutex_name);
                    if (rval != IK_OK) {
                        amba_ik_system_print_uint32_5("[IK] amba_ik_system_create_mutex fail ctx:%d ", context_id, DC_U, DC_U, DC_U, DC_U);
                    }
                }

                if ((rval == IK_OK) && (p_ability->pipe == AMBA_IK_PIPE_VIDEO)) {
                    if(p_ability->video_pipe < AMBA_IK_VIDEO_MAX) {
                        (void)amba_ik_system_lock_mutex(context_id, IK_WAIT_FOREVER, 0u);
                        (void)amba_ik_system_memcpy(&p_ctx->organization.attribute.ability, p_ability, sizeof(ik_ability_t));
                        ctx_get_filter_method((uint8)p_ability->video_pipe, &p_ctx->organization.filter_methods);
                        if (p_ik_buffer_info->init_mode == AMBA_IK_ARCH_HARD_INIT) {
                            (void)amba_ik_system_memset(&p_ctx->organization.active_cr_state, 0x0, sizeof(amba_ik_active_cr_state_t));
                            (void)amba_ik_system_memset(&p_ctx->filters, 0x0, sizeof(amba_ik_filter_t));
                            (void)amba_ik_system_memset(&p_ctx->filters.update_flags.cr, 0xff, sizeof(amba_ik_cr_id_list_t)); // should be full update at first time
                            (void)amba_ik_system_memset(p_ctx->fence, 0x0, sizeof(amba_ik_cr_fence_info_t)*68u);

                            // handle max_running_number
                            // claim the max running number, in case we get wrong ring_idx when cr_running_number overflow
                            p_ctx->organization.active_cr_state.max_running_number = (uint16)(65535U - (65535U % p_ctx->organization.attribute.cr_ring_number));
                        }
                        p_ctx->filters.input_param.ability = p_ability->video_pipe;
                        p_ctx->organization.initial_flag = 1;
#if SUPPORT_CR_MEMORY_FENCE
                        rval |= img_ctx_init_memory_fence(p_ctx);
#endif
                        rval |= ctx_config_filter_baseline_default((uint8)p_ability->video_pipe, p_ctx);
                        (void)amba_ik_system_unlock_mutex(context_id, 0u);
                    } else {
                        amba_ik_system_print_uint32_5("[IK] Error, ability %d is not support now\n", p_ability->video_pipe, DC_U, DC_U, DC_U, DC_U);
                        rval = IK_ERR_0008;
                    }
                } else if ((rval == IK_OK) && (p_ability->pipe == AMBA_IK_PIPE_STILL)) {
                    amba_ik_system_print_uint32_5("[IK] Error, pipe %d is not support now\n", p_ability->pipe, DC_U, DC_U, DC_U, DC_U);
                    rval = IK_ERR_0008;
                } else {
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
        if(rval != IK_OK) {
            amba_ik_system_print_uint32_5("[IK] context_id %d, ability addr: is invalid, get ctx fail !", context_id, DC_U, DC_U, DC_U, DC_U);
        } else {
            amba_ik_system_print_uint32_5("[IK] context_id = %d is not initial", context_id, DC_U, DC_U, DC_U, DC_U);
            rval = IK_ERR_0002;
        }
    }
    return rval;
}

uint32 img_ctx_get_flow_control(uint32 context_id, uint32 flow_idx, uintptr *p_flow_ctrl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    if (p_flow_ctrl == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            if (p_ctx->organization.attribute.cr_ring_number > flow_idx) {
                *p_flow_ctrl = p_ctx->flow.flow_ctrl[flow_idx];
            } else {
                amba_ik_system_print_uint32_5("[IK] cr_ring_number: %d <= flow_idx: %d", p_ctx->organization.attribute.cr_ring_number, flow_idx, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD: Never happen
            }
        }
    }
    return rval;
}

uint32 img_ctx_get_flow_tbl(uint32 context_id, uint32 flow_idx, uintptr *p_flow_tbl)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    if (p_flow_tbl == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            if (p_ctx->organization.attribute.cr_ring_number > flow_idx) {
                *p_flow_tbl = p_ctx->flow.flow_tbl[flow_idx];
            } else {
                amba_ik_system_print_uint32_5("[IK] cr_ring_number: %d <= flow_idx: %d", p_ctx->organization.attribute.cr_ring_number, flow_idx, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD: Never happen
            }
        }
    }
    return rval;
}

uint32 img_ctx_get_flow_tbl_list(uint32 context_id, uint32 flow_idx, uintptr *p_flow_tbl_list)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    if (p_flow_tbl_list == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            if (p_ctx->organization.attribute.cr_ring_number > flow_idx) {
                *p_flow_tbl_list = p_ctx->flow.flow_tbl_list[flow_idx];
            } else {
                amba_ik_system_print_uint32_5("[IK] cr_ring_number: %d <= flow_idx: %d", p_ctx->organization.attribute.cr_ring_number, flow_idx, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD: Never happen
            }
        }
    }
    return rval;
}

uint32 img_ctx_get_crc_data(uint32 context_id, uint32 flow_idx, uintptr *p_crc_data)
{
    uint32 rval;
    amba_ik_context_entity_t *p_ctx;

    if (p_crc_data == NULL) {
        rval = IK_ERR_0005;
    } else {
        rval = img_ctx_get_context(context_id, &p_ctx);
        if (rval == IK_OK) {
            if (p_ctx->organization.attribute.cr_ring_number > flow_idx) {
                *p_crc_data = p_ctx->flow.crc_data[flow_idx];
            } else {
                amba_ik_system_print_uint32_5("[IK] cr_ring_number: %d <= flow_idx: %d", p_ctx->organization.attribute.cr_ring_number, flow_idx, DC_U, DC_U, DC_U);
                rval = IK_ERR_0000; // TBD: Never happen
            }
        }
    }
    return rval;
}

