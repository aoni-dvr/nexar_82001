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


// normal filters
uint32 img_ctx_safety_set_safety_info(uint32 context_id, const ik_safety_info_t *p_safety_info)
{
    amba_ik_system_print_str_5("[IK] set_safety_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_safety_info == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info)
{
    amba_ik_system_print_str_5("[IK] set_vin_sensor_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_sensor_info == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_yuv_mode(uint32 context_id, const uint32 yuv_mode)
{
    amba_ik_system_print_str_5("[IK] set_yuv_mode is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if ((context_id != 0UL) || (yuv_mode != 0xFFFFFFFFUL)) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_before_ce_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_before_ce_wb_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_after_ce_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_after_ce_wb_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain)
{
    amba_ik_system_print_str_5("[IK] set_pre_cc_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_pre_cc_gain == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] set_cfa_leakage_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_cfa_leakage_filter == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] set_anti_aliasing is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_anti_aliasing == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_dynamic_bpc(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] set_dynamic_bpc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_dynamic_bpc==NULL) {
        //MISRAC
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] set_cfa_noise_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_noise_filter == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] set_demosaic is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_demosaic == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_rgb_to_12y(uint32 context_id, const ik_rgb_to_12y_t *p_rgb_to_12y)
{
    amba_ik_system_print_str_5("[IK] set_rgb_to_12y is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_12y == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] set_luma_noise_reduction is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_luma_noise_reduce == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction)
{
    amba_ik_system_print_str_5("[IK] set_color_correction is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] set_tone_curve is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_tone_curve == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] set_rgb_to_yuv_matrix is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_yuv_matrix == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir)
{
    amba_ik_system_print_str_5("[IK] set_rgb_ir is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_ir==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale)
{
    amba_ik_system_print_str_5("[IK] set_chroma_scale is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_scale==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter)
{
    amba_ik_system_print_str_5("[IK] set_chroma_median_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_median_filter==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_first_luma_processing_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] set_first_luma_processing_mode is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_luma_process_mode==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_advance_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] set_advance_spatial_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_advance_spatial_filter==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshpns_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_both is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_both==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshpns_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_noise is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_noise==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshpns_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] first_sharpen_fir is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_fir==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshp_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshp_cor_idx_scl(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_coring_index_scale is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring_idx_scale==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshpns_min_coring_result(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_min_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_min_coring_result==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshpns_max_coring_result(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_max_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_max_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fstshpns_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] set_fstshpns_scale_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_scale_coring == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshp_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_both is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshpns_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_noise is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_noise == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshpns_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_fir is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_fir == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshp_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_coring == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshp_cor_idx_scl(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_coring_index_scale is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_coring_idx_scale == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshpns_min_coring_result(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_min_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_min_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshpns_max_coring_result(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_max_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_max_coring_result == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshpns_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] set_fnlshpns_scale_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_scale_coring == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fnlshp_both_tdt(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    amba_ik_system_print_str_5("[IK] final_sharpen_both_three_d_table is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both_three_d_table == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] set_chroma_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_w_chroma_ft(uint32 context_id, const ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] set_wide_chroma_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_wide_chroma_filter==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_w_chroma_ft_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    amba_ik_system_print_str_5("[IK] set_wide_chroma_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter_combine==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch)
{
    uint32 rval = IK_ERR_0200;
    amba_ik_system_print_str_5("[IK] set_grgb_mismatch is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        // misraC
    }
    if (p_grgb_mismatch==NULL) {
        // misraC
    }
    return rval;
}

uint32 img_ctx_safety_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf)
{
    amba_ik_system_print_str_5("[IK] set_video_mctf is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta)
{
    amba_ik_system_print_str_5("[IK] set_video_mctf_ta is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_ta==NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_mctf_and_final_sharpen(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    amba_ik_system_print_str_5("[IK] img_ctx_set_video_mctf_and_final_sharpen is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_and_final_sharpen == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info)
{
    amba_ik_system_print_str_5("[IK] set_aaa_stat_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_stat_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_aaa_pg_af_info(uint32 context_id, const ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    amba_ik_system_print_str_5("[IK] set_aaa_pg_af_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pg_af_stat_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] set_af_ex_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_af_stat_ex_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] set_af_ex_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pg_af_stat_ex_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info)
{
    amba_ik_system_print_str_5("[IK] set_window_size_info_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_window_size_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_fe_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc)
{
    amba_ik_system_print_str_5("[IK] img_ctx_ivd_set_fe_tone_curve is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRA
    }
    if (p_fe_tc == NULL) {
        //MISRA
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_resampler_str(uint32 context_id, const ik_resampler_strength_t *p_resample_str)
{
    amba_ik_system_print_str_5("[IK] set_resampler_str is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_resample_str == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    uint32 rval = IK_ERR_0200;
    amba_ik_system_print_str_5("[IK] set_histogram_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_hist_info == NULL) {
        //MISRAC
    }
    return rval;
}

uint32 img_ctx_safety_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] set_pg_histogram_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //MISRAC
    }
    if (p_hist_info == NULL) {
        //MISRAC
    }
    return IK_ERR_0200;
}


uint32 img_ctx_safety_get_safety_info(uint32 context_id, ik_safety_info_t *p_safety_info)
{
    amba_ik_system_print_str_5("[IK] get_safety_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_safety_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_safety_info, p_safety_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info)
{
    amba_ik_system_print_str_5("[IK] get_vin_sensor_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_sensor_info==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_sensor_info, p_sensor_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_yuv_mode(uint32 context_id, uint32 *p_yuv_mode)
{
    amba_ik_system_print_str_5("[IK] get_yuv_mode() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_yuv_mode != NULL) {
        //TBD
        *p_yuv_mode = 0U;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_before_ce_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_before_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_before_ce_wb_gain, p_before_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_after_ce_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_after_ce_wb_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_after_ce_wb_gain, p_after_ce_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain)
{
    amba_ik_system_print_str_5("[IK] get_pre_cc_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pre_cc_gain == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pre_cc_gain, p_pre_cc_gain, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter)
{
    amba_ik_system_print_str_5("[IK] get_cfa_leakage_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_leakage_filter==NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_leakage_filter, p_cfa_leakage_filter, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing)
{
    amba_ik_system_print_str_5("[IK] get_anti_aliasing is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_anti_aliasing == NULL) {
        //TBD
    } else {
        (void) amba_ik_system_memcpy(p_anti_aliasing, p_anti_aliasing, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_dynamic_bpc(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc)
{
    amba_ik_system_print_str_5("[IK] get_dynamic_bpc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_dynamic_bpc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_dynamic_bpc, p_dynamic_bpc, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter)
{
    amba_ik_system_print_str_5("[IK] get_cfa_noise_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_cfa_noise_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_cfa_noise_filter, p_cfa_noise_filter, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic)
{
    amba_ik_system_print_str_5("[IK] get_demosaic is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_demosaic == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_demosaic, p_demosaic, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_rgb_to_12y(uint32 context_id, ik_rgb_to_12y_t *p_rgb_to_12y)
{
    amba_ik_system_print_str_5("[IK] get_rgb_to_12y is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_12y == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_to_12y, p_rgb_to_12y, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_luma_noise_reduce)
{
    amba_ik_system_print_str_5("[IK] get_rgb_to_12y is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_luma_noise_reduce == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_luma_noise_reduce, p_luma_noise_reduce, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction)
{
    amba_ik_system_print_str_5("[IK] get_color_correction is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_color_correction == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_color_correction, p_color_correction, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve)
{
    amba_ik_system_print_str_5("[IK] get_tone_curve is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_tone_curve == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_tone_curve, p_tone_curve, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix)
{
    amba_ik_system_print_str_5("[IK] get_rgb_to_yuv_matrix is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_to_yuv_matrix == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_to_yuv_matrix, p_rgb_to_yuv_matrix, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir)
{
    amba_ik_system_print_str_5("[IK] get_rgb_ir is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_rgb_ir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_rgb_ir, p_rgb_ir, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale)
{
    amba_ik_system_print_str_5("[IK] get_chroma_scale is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_scale, p_chroma_scale, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter)
{
    amba_ik_system_print_str_5("[IK] get_chroma_median_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_median_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_median_filter, p_chroma_median_filter, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_first_luma_processing_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode)
{
    amba_ik_system_print_str_5("[IK] get_first_luma_processing_mode is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_luma_process_mode == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_luma_process_mode, p_first_luma_process_mode, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_advance_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter)
{
    amba_ik_system_print_str_5("[IK] get_advance_spatial_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_advance_spatial_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_advance_spatial_filter, p_advance_spatial_filter, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshpns_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_both is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_both, p_first_sharpen_both, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshpns_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_noise is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_noise, p_first_sharpen_noise, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshpns_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_fir is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_fir, p_first_sharpen_fir, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshp_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_first_sharpen_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_first_sharpen_coring, p_first_sharpen_coring, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshp_cor_idx_scl(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_fstshpns_coring_index_scale)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_coring_index_scale is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_coring_index_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_coring_index_scale, p_fstshpns_coring_index_scale, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshpns_min_coring_result(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_fstshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_min_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_min_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_min_coring_result, p_fstshpns_min_coring_result, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshpns_max_coring_result(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_fstshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_max_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_max_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_max_coring_result, p_fstshpns_max_coring_result, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fstshpns_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_fstshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] get_fstshpns_scale_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fstshpns_scale_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fstshpns_scale_coring, p_fstshpns_scale_coring, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshp_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_both is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_both, p_final_sharpen_both, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshpns_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_noise is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_noise == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_noise, p_final_sharpen_noise, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshpns_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_fir is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_fir == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_fir, p_final_sharpen_fir, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshp_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_coring, p_final_sharpen_coring, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshp_cor_idx_scl(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_fnlshpns_coring_index_scale)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_coring_index_scale is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_coring_index_scale == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_coring_index_scale, p_fnlshpns_coring_index_scale, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshpns_min_coring_result(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_fnlshpns_min_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_min_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_min_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_min_coring_result, p_fnlshpns_min_coring_result, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshpns_max_coring_result(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_fnlshpns_max_coring_result)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_max_coring_result is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_max_coring_result == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_max_coring_result, p_fnlshpns_max_coring_result, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshpns_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_fnlshpns_scale_coring)
{
    amba_ik_system_print_str_5("[IK] get_fnlshpns_scale_coring is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fnlshpns_scale_coring == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fnlshpns_scale_coring, p_fnlshpns_scale_coring, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fnlshp_both_tdt(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table)
{
    amba_ik_system_print_str_5("[IK] final_sharpen_both_three_d_table is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_final_sharpen_both_three_d_table == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_final_sharpen_both_three_d_table, p_final_sharpen_both_three_d_table, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] get_chroma_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_filter, p_chroma_filter, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_w_chroma_ft(uint32 context_id, ik_wide_chroma_filter_t *p_wide_chroma_filter)
{
    amba_ik_system_print_str_5("[IK] get_wide_chroma_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_wide_chroma_filter == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_wide_chroma_filter, p_wide_chroma_filter, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_w_chroma_ft_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine)
{
    amba_ik_system_print_str_5("[IK] get_wide_chroma_filter is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_chroma_filter_combine == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_chroma_filter_combine, p_chroma_filter_combine, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch)
{
    amba_ik_system_print_str_5("[IK] grgb_mismatch is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_grgb_mismatch == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_grgb_mismatch, p_grgb_mismatch, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf)
{
    amba_ik_system_print_str_5("[IK] get_video_mctf is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_video_mctf, p_video_mctf, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta)
{
    amba_ik_system_print_str_5("[IK] get_video_mctf_ta is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_ta == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_video_mctf_ta, p_video_mctf_ta, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_mctf_and_final_sharpen(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen)
{
    amba_ik_system_print_str_5("[IK] img_ctx_ivd_get_mctf_and_final_sharpen is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_video_mctf_and_final_sharpen == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_video_mctf_and_final_sharpen, p_video_mctf_and_final_sharpen, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info)
{
    amba_ik_system_print_str_5("[IK] get_aaa_stat_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_stat_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_stat_info, p_stat_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_aaa_pg_af_info(uint32 context_id, ik_aaa_pg_af_stat_info_t *p_pg_af_stat_info)
{
    amba_ik_system_print_str_5("[IK] get_aaa_pg_af_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pg_af_stat_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pg_af_stat_info, p_pg_af_stat_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] get_af_ex_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_af_stat_ex_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_af_stat_ex_info, p_af_stat_ex_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info)
{
    amba_ik_system_print_str_5("[IK] get_af_ex_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_pg_af_stat_ex_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_pg_af_stat_ex_info, p_pg_af_stat_ex_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info)
{
    amba_ik_system_print_str_5("[IK] get_window_size_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_window_size_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_window_size_info, p_window_size_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_fe_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc)
{
    amba_ik_system_print_str_5("[IK] get_fe_tone_curve is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_fe_tc == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_fe_tc, p_fe_tc, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_resampler_str(uint32 context_id, ik_resampler_strength_t *p_resample_str)
{
    amba_ik_system_print_str_5("[IK] get_resampler_str is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_resample_str == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_resample_str, p_resample_str, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] get_histogram_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hist_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hist_info, p_hist_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info)
{
    amba_ik_system_print_str_5("[IK] get_pg_histogram_info is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id != 0UL) {
        //TBD
    }
    if (p_hist_info == NULL) {
        //TBD
    } else {
        (void)amba_ik_system_memcpy(p_hist_info, p_hist_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

// calib
uint32 img_ctx_safety_set_static_bpc(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc)
{
    amba_ik_system_print_str_5("[IK][ERROR] set_static_bpc is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_static_bpc == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_static_bpc_internal(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *static_bpc_internal)
{
    amba_ik_system_print_str_5("[IK][ERROR] set_static_bpc_internal is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (static_bpc_internal == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_static_bpc_enable_info(uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK][ERROR] set_static_bpc_enable_info is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_static_bpc_enable_info(uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_static_bpc_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_vignette(uint32 context_id, const ik_vignette_t *p_vignette)
{
    amba_ik_system_print_str_5("[IK] set_vignette() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if(context_id == 0UL) {
        //TBD
    }
    if(p_vignette == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_vignette_enable_info(uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_vignette_compensation_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if(context_id == 0UL) {
        //TBD
    }
    if(enable == 0U) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_vignette_enable_info(uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_vignette_compensation_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if(context_id == 0UL) {
        //TBD
    }
    if(p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_calib_warp_info(uint32 context_id, const ik_warp_info_t *p_calib_warp_info)
{
    amba_ik_system_print_str_5("[IK] set_calib_warp_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_calib_warp_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_calib_ca_warp_info(uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info)
{
    amba_ik_system_print_str_5("[IK] set_calib_ca_warp_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_calib_ca_warp_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_dzoom_info(uint32 context_id, const ik_dzoom_info_t *p_dzoom_info)
{
    amba_ik_system_print_str_5("[IK] set_dzoom_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_dzoom_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_warp_enable_info(uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_warp_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_cawarp_enable_info(uint32 context_id, const uint32 enable)
{
    amba_ik_system_print_str_5("[IK] set_cawarp_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (enable == 0U) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_dummy_win_margin_range_info(uint32 context_id, const ik_dummy_margin_range_t *p_dmy_range)
{
    amba_ik_system_print_str_5("[IK] set_dummy_win_margin_rangee_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_dmy_range == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_flip_mode(uint32 context_id, const uint32 mode)
{
    amba_ik_system_print_str_5("[IK] set_flip_input_mode_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (mode == 0UL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_warp_buffer_info(uint32 context_id, const ik_warp_buffer_info_t *p_warp_buf_info)
{
    amba_ik_system_print_str_5("[IK] set_warp_buffer_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_warp_buf_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_stitching_info(uint32 context_id, const ik_stitch_info_t *p_stitch_info)
{
    amba_ik_system_print_str_5("[IK] set_stitching_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_stitch_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_burst_tile(uint32 context_id, const ik_burst_tile_t *p_burst_tile)
{
    amba_ik_system_print_str_5("[IK] set_burst_tile() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_burst_tile == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_static_bpc(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc)
{
    amba_ik_system_print_str_5("[IK][ERROR] get static_bpc API is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //MISRAC
    }
    if (p_static_bpc != NULL) {
        (void)amba_ik_system_memcpy(p_static_bpc, p_static_bpc, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_static_bpc_internal(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal)
{
    amba_ik_system_print_str_5("IK][ERROR] get_static_bpc_internal API is invalid in safety state", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //MISRAC
    }
    if (p_static_bpc_internal != NULL) {
        (void)amba_ik_system_memcpy(p_static_bpc_internal, p_static_bpc_internal, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_vignette(uint32 context_id, ik_vignette_t *p_vignette)
{
    amba_ik_system_print_str_5("IK][ERROR] get_vignette API is invalid in safety state", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_vignette != NULL) {
        (void)amba_ik_system_memcpy(p_vignette, p_vignette, sizeof(ik_vignette_t));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_calib_warp_info(uint32 context_id, ik_warp_info_t *p_calib_warp_info)
{
    amba_ik_system_print_str_5("[IK] get_calib_warp_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_calib_warp_info != NULL) {
        //TBD
        p_calib_warp_info->reserved = 0UL;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_calib_ca_warp_info(uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info)
{
    amba_ik_system_print_str_5("[IK] get_calib_ca_warp_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_calib_ca_warp_info != NULL) {
        //TBD
        p_calib_ca_warp_info->Reserved = 0UL;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_dzoom_info(uint32 context_id, ik_dzoom_info_t *p_dzoom_info)
{
    amba_ik_system_print_str_5("[IK] get_dzoom_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_dzoom_info != NULL) {
        //TBD
        p_dzoom_info->zoom_x = 0UL;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_warp_enable_info(uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_warp_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_cawarp_enable_info(uint32 context_id, uint32 *p_enable)
{
    amba_ik_system_print_str_5("[IK] get_cawarp_enable_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_enable != NULL) {
        //TBD
        *p_enable = 0U;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_dummy_win_margin_range_info(uint32 context_id, ik_dummy_margin_range_t *p_dmy_range)
{
    amba_ik_system_print_str_5("[IK] get_dummy_win_margin_range_info() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_dmy_range != NULL) {
        //TBD
        p_dmy_range->top = 0UL;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_vin_active_window(uint32 context_id, const ik_vin_active_window_t *p_active_win)
{
    amba_ik_system_print_str_5("[IK] vin_active_window() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_active_win != NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_vin_active_window(uint32 context_id, ik_vin_active_window_t *p_active_win)
{
    amba_ik_system_print_str_5("[IK] get_vin_active_window() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_active_win != NULL) {
        //TBD
        p_active_win->active_geo.start_x= 0UL;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_warp_internal(uint32 context_id, const ik_warp_internal_info_t *p_warp_internal)
{
    amba_ik_system_print_str_5("[IK] set warp_internal() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_warp_internal != NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_cawarp_internal(uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal)
{
    amba_ik_system_print_str_5("[IK] set cawarp_internal() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_cawarp_internal != NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_warp_internal(uint32 context_id, ik_warp_internal_info_t *p_warp_internal)
{
    amba_ik_system_print_str_5("[IK] get warp_internal() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_warp_internal != NULL) {
        //TBD
        p_warp_internal->enable = 0UL;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_cawarp_internal(uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal)
{
    amba_ik_system_print_str_5("[IK] get cawarp_internal() is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_cawarp_internal != NULL) {
        //TBD
        p_cawarp_internal->vert_warp_enable = 0U;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_flip_mode(uint32 context_id, uint32 *mode)
{
    amba_ik_system_print_str_5("[IK] get_flip_mode is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //MISRAC
    }
    if (mode != NULL) {
        (void)amba_ik_system_memcpy(mode, mode, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_warp_buffer_info(uint32 context_id, ik_warp_buffer_info_t *p_warp_buf_info)
{
    amba_ik_system_print_str_5("[IK] get_warp_buffer_info is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //MISRAC
    }
    if (p_warp_buf_info != NULL) {
        (void)amba_ik_system_memcpy(p_warp_buf_info, p_warp_buf_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_stitching_info(uint32 context_id, ik_stitch_info_t *p_stitch_info)
{
    amba_ik_system_print_str_5("[IK] get_stitching_info is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //MISRAC
    }
    if (p_stitch_info != NULL) {
        (void)amba_ik_system_memcpy(p_stitch_info, p_stitch_info, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_burst_tile(uint32 context_id, ik_burst_tile_t *p_burst_tile)
{
    amba_ik_system_print_str_5("[IK] get_burst_tile is invalid in safety state!", DC_S, DC_S, DC_S, DC_S, DC_S);
    if (context_id == 0UL) {
        //MISRAC
    }
    if (p_burst_tile != NULL) {
        (void)amba_ik_system_memcpy(p_burst_tile, p_burst_tile, sizeof(uint8));
    }
    return IK_ERR_0200;
}

// HDR
uint32 img_ctx_safety_set_exp1_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_exp1_fe_static_blc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp1_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_exp2_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_exp2_fe_static_blc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp2_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_exp1_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_exp1_fe_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp1_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_exp2_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_exp2_fe_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp2_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_raw_info)
{
    amba_ik_system_print_str_5("[IK] set_hdr_raw_offset is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_raw_info == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_ce(uint32 context_id, const ik_ce_t *p_ce)
{
    amba_ik_system_print_str_5("[IK] set_ce is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] set_ce_input_table is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_input_table == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table)
{
    amba_ik_system_print_str_5("[IK] set_ce_out_table is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_out_table == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend)
{
    amba_ik_system_print_str_5("[IK] set_hdr_blend is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_hdr_blend == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_exp0_fe_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] set_exp0_fe_static_blc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_static_blc == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_set_exp0_fe_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] set_exp0_fe_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_wb_gain == NULL) {
        //TBD
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_exp1_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_exp1_fe_static_blc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp1_frontend_static_blc != NULL) {
        //TBD
        (void)amba_ik_system_memcpy(p_exp1_frontend_static_blc, p_exp1_frontend_static_blc, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_exp2_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_exp2_fe_static_blc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp2_frontend_static_blc != NULL) {
        //TBD
        (void)amba_ik_system_memcpy(p_exp2_frontend_static_blc, p_exp2_frontend_static_blc, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_exp1_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_exp1_fe_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp1_frontend_wb_gain != NULL) {
        uint32 tmp = p_exp1_frontend_wb_gain->b_gain;
        p_exp1_frontend_wb_gain->b_gain = tmp;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_exp2_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_exp2_fe_wb_gain is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp2_frontend_wb_gain != NULL) {
        uint32 tmp = p_exp2_frontend_wb_gain->b_gain;
        p_exp2_frontend_wb_gain->b_gain = tmp;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_raw_info)
{
    amba_ik_system_print_str_5("[IK] get_hdr_raw_offset is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_raw_info != NULL) {
        uint32 tmp = p_raw_info->x_offset[0];
        p_raw_info->x_offset[0] = tmp;
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_ce(uint32 context_id, ik_ce_t *p_ce)
{
    amba_ik_system_print_str_5("[IK] get_ce is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce != NULL) {
        (void)amba_ik_system_memcpy(p_ce, p_ce, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table)
{
    amba_ik_system_print_str_5("[IK] get_ce_input_table is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_input_table != NULL) {
        (void)amba_ik_system_memcpy(p_ce_input_table, p_ce_input_table, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table)
{
    amba_ik_system_print_str_5("[IK] get_ce_out_table is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_ce_out_table != NULL) {
        (void)amba_ik_system_memcpy(p_ce_out_table, p_ce_out_table, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend)
{
    amba_ik_system_print_str_5("[IK] get_hdr_blend is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_hdr_blend != NULL) {
        (void)amba_ik_system_memcpy(p_hdr_blend, p_hdr_blend, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_exp0_fe_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc)
{
    amba_ik_system_print_str_5("[IK] get_exp0_fe_static_blc is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_static_blc != NULL) {
        (void)amba_ik_system_memcpy(p_exp0_frontend_static_blc, p_exp0_frontend_static_blc, sizeof(uint8));
    }
    return IK_ERR_0200;
}

uint32 img_ctx_safety_get_exp0_fe_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain)
{
    amba_ik_system_print_str_5("[IK] get_exp0_fe_wb_gain( is invalid in safety state!", NULL, NULL, NULL, NULL, NULL);
    if (context_id == 0UL) {
        //TBD
    }
    if (p_exp0_frontend_wb_gain != NULL) {
        (void)amba_ik_system_memcpy(p_exp0_frontend_wb_gain, p_exp0_frontend_wb_gain, sizeof(uint8));
    }
    return IK_ERR_0200;
}

