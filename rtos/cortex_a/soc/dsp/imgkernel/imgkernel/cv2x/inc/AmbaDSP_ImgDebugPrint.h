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

#ifndef IMG_DEBUG_PRINT_H
#define IMG_DEBUG_PRINT_H

#include "AmbaDSP_ImgFilter.h"

typedef enum {
    LOG_SHOW_CMD_ID_LIST = 0U,
    LOG_VIN_SENSOR_INFO = 1U,
    LOG_FE_TONE_CURVE = 2U,
    LOG_FE_STATIC_BLC = 3U,
    LOG_FE_WB_GAIN = 4U,
    LOG_HDR_BLEND = 5U,
    LOG_VIGNETTE = 6U,
    LOG_DGAIN_SATURATION_LEVEL = 7U,
    LOG_DYNAMIC_BAD_PIXEL_CORRECTION = 8U,
    LOG_STATIC_BAD_PIXEL_CORRECTION = 9U,
    LOG_CFA_LEAKAGE_FILTER = 10U,
    LOG_ANTI_ALIASING = 11U,
    LOG_GRGB_MISMATCH = 12U,
    LOG_GLOBAL_DGAIN = 13U,
    LOG_AE_GAIN = 14U,
    LOG_BEFORE_WB_GAIN = 15U,
    LOG_CAWARP_COMPENSATION = 16U,
    LOG_CONTRAST_ENHANCE = 17U,
    LOG_CONTRAST_ENHANCE_IN_TABLE = 18U,
    LOG_CONTRAST_ENHANCE_OUT_TABLE = 19U,
    LOG_CONTRAST_ENHANCE_EXTERNAL_HDS = 20U,
    LOG_AFTER_WB_GAIN = 21U,
    LOG_CFA_NOISE_FILTER = 22U,
    LOG_DEMOSAIC = 23U,
    LOG_RGB_IR = 24U,
    LOG_PRE_CC_GAIN = 25U,
    LOG_COLOR_CORRECTION_REG = 26U,
    LOG_COLOR_CORRECTION = 27U,
    LOG_TONE_CURVE = 28U,
    LOG_RGB_TO_YUV_MATRIX = 29U,
    LOG_CHROMA_SCALE = 30U,
    LOG_CHROMA_FILTER = 31U,
    LOG_CHROMA_MEDIAN_FILTER = 32U,
    LOG_FIRST_LUMA_PROCESSING_MODE = 33U,
    LOG_ADVANCE_SPATIAL_FILTER = 34U,
    LOG_FIRST_SHARPEN_NOISE_BOTH = 35U,
    LOG_FIRST_SHARPEN_NOISE_FIR = 36U,
    LOG_FIRST_SHARPEN_NOISE_NOISE = 37U,
    LOG_FIRST_SHARPEN_NOISE_CORING = 38U,
    LOG_FIRST_SHARPEN_NOISE_CORING_INDEX_SCALE = 39U,
    LOG_FIRST_SHARPEN_NOISE_MIN_CORING_RESULT = 40U,
    LOG_FIRST_SHARPEN_NOISE_MAX_CORING_RESULT = 41U,
    LOG_FIRST_SHARPEN_NOISE_SCALE_CORING = 42U,

    LOG_WARP_COMPENSATION = 43U,
    LOG_VIDEO_MCTF = 44U,
    LOG_VIDEO_MCTF_TEMPORAL_ADJUST = 45U,
    LOG_VIDEO_MCTF_AND_FINAL_SHARPEN = 46U,

    LOG_FINAL_SHARPEN_NOISE_BOTH = 47U,
    LOG_FINAL_SHARPEN_NOISE_BOTH_TDT = 48U,
    LOG_FINAL_SHARPEN_NOISE_NOISE = 49U,
    LOG_FINAL_SHARPEN_NOISE_FIR = 50U,
    LOG_FINAL_SHARPEN_NOISE_CORING = 51U,
    LOG_FINAL_SHARPEN_NOISE_CORING_INDEX_SCALE = 52U,
    LOG_FINAL_SHARPEN_NOISE_MIN_CORING_RESULT = 53U,
    LOG_FINAL_SHARPEN_NOISE_MAX_CORING_RESULT = 54U,
    LOG_FINAL_SHARPEN_NOISE_SCALE_CORING = 55U,

    LOG_DZOOM_INFO = 56U,
    LOG_WINDOW_SIZE_INFO = 57U,
    LOG_VIN_ACTIVE_WIN = 58U,
    LOG_DUMMY_MARGIN_RANGE = 59U,
    LOG_RESAMPLER_STR = 60U,

    LOG_POST_EXECUTE = 61U,
    LOG_3A_STAT = 62U,
    LOG_CTX_INIT = 63U,
    LOG_MOTION_DETECTION = 64U,

    LOG_MAX = 65U,
    LOG_SHOW_ALL = 255U
} amba_ik_debug_id_e;

typedef struct {
    void (*vin_sensor_info)(uint32 context_id, const ik_vin_sensor_info_t *p_vin_sensor);
    void (*fe_tone_curve)(uint32 context_id, const ik_frontend_tone_curve_t *p_tone_curve);
    void (*fe_dgain_saturation_level)(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_fe_dgain_saturation_level);
    void (*fe_blc)(uint32 context_id, const ik_static_blc_level_t *p_blc, uint32 exp_idx);
    void (*fe_wb)(uint32 context_id, const ik_frontend_wb_gain_t *p_fe_wb, uint32 exp_idx);
    void (*hdr_blend)(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);
    void (*vignette_enable)(uint32 context_id, const uint32 enable);
    void (*vignette_info)(uint32 context_id, const ik_vignette_t *p_vig);
    void (*dynamic_bad_pxl_cor)(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dbc);
    void (*static_bpc_enable)(uint32 context_id, const uint32 enable);
    void (*static_bpc)(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
    void (*cfa_leakage)(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage);
    void (*anti_aliasing)(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
    void (*grgb_mismatch)(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
    void (*global_dgain)(uint32 context_id, const ik_global_dgain_t *p_global_dgain);
    void (*ae_gain)(uint32 context_id, const ik_ae_gain_t *p_ae_gain);
    void (*before_ce_wb)(uint32 context_id, const ik_wb_gain_t *p_wb_gain);
    void (*cawarp_enable)(uint32 context_id, const uint32 enable);
    void (*cawarp)(uint32 context_id, const ik_cawarp_info_t *p_cawarp);
    void (*ce)(uint32 context_id, const ik_ce_t *p_ce);
    void (*ce_input_tbl)(uint32 context_id, const ik_ce_input_table_t *p_ce_input_tbl);
    void (*ce_output_tbl)(uint32 context_id, const ik_ce_output_table_t *p_ce_output_tbl);
    void (*ce_external_hds)(uint32 context_id, const ik_ce_external_hds_t *p_ce_ext_hds);
    void (*after_ce_wb)(uint32 context_id, const ik_wb_gain_t *p_wb_gain);
    void (*cfa_noise_filter)(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
    void (*demosaic)(uint32 context_id, const ik_demosaic_t *p_demosaic);
    void (*rgb_ir)(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
    void (*pre_cc_gain)(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain);
    void (*color_correction_reg)(uint32 context_id, const ik_color_correction_reg_t *p_cc_reg);
    void (*color_correction)(uint32 context_id, const ik_color_correction_t *p_cc_3d);
    void (*tone_curve)(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
    void (*rgb_to_yuv_matrix)(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
    void (*chroma_sacle)(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
    void (*chroma_filter)(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
    void (*chroma_median_filter)(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
    void (*first_luma_proc_mode)(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_proc_mode);
    void (*adv_spat_fltr)(uint32 context_id, const ik_adv_spatial_filter_t *p_adv_spat_fltr);
    void (*first_sharpen_both)(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
    void (*first_sharpen_fir)(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
    void (*first_sharpen_noise)(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
    void (*first_sharpen_coring)(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
    void (*fstshpns_cor_idx_scl)(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
    void (*fstshpns_min_cor_rst)(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
    void (*fstshpns_max_cor_rst)(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
    void (*fstshpns_scl_cor)(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
    void (*warp_enable)(uint32 context_id, const uint32 enable);
    void (*warp_info)(uint32 context_id, const ik_warp_info_t *p_warp_info);
    void (*video_mctf)(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
    void (*video_mctf_ta)(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
    void (*video_mctf_and_fnlshp)(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
    void (*final_sharpen_both)(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
    void (*final_sharpen_fir)(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
    void (*final_sharpen_noise)(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
    void (*final_sharpen_coring)(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
    void (*fnlshpns_cor_idx_scl)(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
    void (*fnlshpns_min_cor_rst)(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);
    void (*fnlshpns_max_cor_rst)(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);
    void (*fnlshpns_scl_cor)(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
    void (*final_sharpen_both_three_d_table)(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_fstshpns_both_tdt);
    void (*dzoom_info)(uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
    void (*window_size_info)(uint32 context_id, const ik_window_size_info_t *p_window_size_info);
    void (*cfa_window_size_info)(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info);
    void (*vin_active_win)(uint32 context_id, const ik_vin_active_window_t *p_window_geo);
    void (*dummy_margin_range)(uint32 context_id, const ik_dummy_margin_range_t *p_dummy_range);
    void (*resamp_strength)(uint32 context_id, const ik_resampler_strength_t *p_window_geo);
    void (*motion_detect)(uint32 context_id, const ik_motion_detect_t *p_md);
    void (*motion_detect_pos_dep)(uint32 context_id, const ik_pos_dep33_t *p_md);
} amba_img_dsp_debug_print_func_t;

void ik_init_debug_print(void);
void ik_hook_debug_print(void);
//void ik_debug_iso_hdr_ce_config(uint32 context_id);
uint32 ik_set_debug_log_id(uint32 debug_id, uint8 enable);
const amba_img_dsp_debug_print_func_t* ik_get_debug_print_func(void);
uint32 ik_print_ik_working_buffer_info(const void* p_buffer);
//the below api can be removed in future
uint32 amba_ik_set_mctf_internal(uint32 context_id, const ik_mctf_internal_t *p_mctf_internal);
uint32 amba_ik_get_mctf_internal(uint32 context_id, ik_mctf_internal_t *p_mctf_internal);

#endif
