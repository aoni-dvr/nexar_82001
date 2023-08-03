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

#ifndef AMBA_IK_IMG_HISO_FILTER_H
#define AMBA_IK_IMG_HISO_FILTER_H

#include "AmbaDSP_ImgFilter.h"

//hiso
typedef ik_anti_aliasing_t ik_hi_anti_aliasing_t;
typedef ik_cfa_leakage_filter_t ik_hi_cfa_leakage_filter_t;
typedef ik_dynamic_bad_pixel_correction_t ik_hi_dynamic_bpc_t;
typedef ik_grgb_mismatch_t ik_hi_grgb_mismatch_t;
typedef ik_chroma_median_filter_t ik_hi_chroma_median_filter_t;
typedef ik_demosaic_t ik_hi_demosaic_t;
typedef ik_cfa_noise_filter_t ik_hi_cfa_noise_filter_t;

//hiso low2
typedef ik_anti_aliasing_t ik_li2_anti_aliasing_t;
typedef ik_cfa_leakage_filter_t ik_li2_cfa_leakage_filter_t;
typedef ik_dynamic_bad_pixel_correction_t ik_li2_dynamic_bpc_t;
typedef ik_grgb_mismatch_t ik_li2_grgb_mismatch_t;
typedef ik_demosaic_t ik_li2_demosaic_t;
typedef ik_cfa_noise_filter_t ik_li2_cfa_noise_filter_t;

//asf
typedef ik_adv_spatial_filter_t ik_hi_asf_t;

typedef struct {
    uint32 enable;
    ik_asf_fir_t fir;
    uint32 directional_decide_t0;
    uint32 directional_decide_t1;
    ik_full_adaptation_t adapt;
    ik_level_method_t level_str_adjust;
    ik_level_method_t t0_t1_div;
    uint32 max_change_not_t0_t1_alpha;
    uint32 max_change_up;
    uint32 max_change_down;
} ik_hi_asf_a_t;

typedef ik_hi_asf_t ik_li2_asf_t;
typedef ik_hi_asf_a_t ik_hi_med1_asf_t;
typedef ik_hi_asf_a_t ik_hi_high2_asf_t;

typedef struct {
    uint32 enable;
    ik_asf_fir_t fir;
    uint32 directional_decide_t0;
    uint32 directional_decide_t1;
    ik_full_adaptation_t adapt;
    ik_level_t level_str_adjust;
    ik_level_t t0_t1_div;
    uint32 max_change_up;
    uint32 max_change_down;
} ik_hi_asf_b1_t;

typedef ik_hi_asf_b1_t ik_hi_low_asf_t;

typedef struct {
    uint32 enable;
    ik_asf_fir_t fir;
    uint32 directional_decide_t0;
    uint32 directional_decide_t1;
    ik_full_adaptation_t adapt;
    ik_level_method_t level_str_adjust;
    ik_level_method_t t0_t1_div;
    uint32 max_change_up;
    uint32 max_change_down;
} ik_hi_asf_b2_t;

typedef ik_hi_asf_b2_t ik_hi_med2_asf_t;
typedef ik_hi_asf_b2_t ik_hi_high_asf_t;

typedef struct {
    uint32 enable;
    ik_asf_fir_t fir;
    uint32 directional_decide_t0;
    uint32 directional_decide_t1;
    uint32 alpha_min;
    uint32 alpha_max;
    uint32 t0;
    uint32 t1;
    ik_level_t level_str_adjust;
    ik_level_t t0_t1_div;
    uint32 max_change_not_t0_t1_alpha;
    uint32 max_change;
} ik_chroma_asf_t;

typedef ik_chroma_asf_t ik_hi_chroma_asf_t;
typedef ik_chroma_asf_t ik_hi_low_chroma_asf_t;

//shp hiso high
typedef ik_first_sharpen_both_t ik_hi_high_shpns_both_t;
typedef ik_first_sharpen_fir_t ik_hi_high_shpns_fir_t;
typedef ik_first_sharpen_coring_t ik_hi_high_shpns_coring_t;
typedef ik_level_method_t ik_hi_high_shpns_cor_idx_scl_t;
typedef ik_level_method_t ik_hi_high_shpns_min_cor_rst_t;
typedef ik_level_method_t ik_hi_high_shpns_max_cor_rst_t;
typedef ik_level_method_t ik_hi_high_shpns_scl_cor_t;

typedef struct {
    uint32 max_change_up; // 0:255
    uint32 max_change_down; // 0:255
    ik_sharpen_noise_sharpen_fir_t spatial_fir;
    ik_level_method_t level_str_adjust;
    uint32 level_str_adjust_not_t0_t1_level_based; // 0:10
    uint32 t0; // 0:255 t0 <= t1, t1 - t0 <= 15
    uint32 t1; // 0:255
    uint32 alpha_min; // 0:16
    uint32 alpha_max; // 0:16
} ik_hi_high_shpns_noise_t;

//shp hiso med
typedef ik_first_sharpen_both_t ik_hi_med_shpns_both_t;
typedef ik_first_sharpen_fir_t ik_hi_med_shpns_fir_t;
typedef ik_hi_high_shpns_noise_t ik_hi_med_shpns_noise_t;
typedef ik_first_sharpen_coring_t ik_hi_med_shpns_coring_t;
typedef ik_level_method_t ik_hi_med_shpns_cor_idx_scl_t;
typedef ik_level_method_t ik_hi_med_shpns_min_cor_rst_t;
typedef ik_level_method_t ik_hi_med_shpns_max_cor_rst_t;
typedef ik_level_method_t ik_hi_med_shpns_scl_cor_t;

//SHP hiso low2
typedef ik_first_sharpen_fir_t ik_li2_shpns_fir_t;
typedef ik_hi_high_shpns_noise_t ik_li2_shpns_noise_t;
typedef ik_level_method_t ik_li2_shpns_cor_idx_scl_t;
typedef ik_level_method_t ik_li2_shpns_min_cor_rst_t;
typedef ik_level_method_t ik_li2_shpns_max_cor_rst_t;
typedef ik_level_method_t ik_li2_shpns_scl_cor_t;

typedef struct {
    uint32 enable;
    uint32 mode;
    uint32 edge_thresh;
    uint32 wide_edge_detect;
    uint32 max_change_up5x5;
    uint32 max_change_down5x5;
    uint32 max_change_up; // 0:255
    uint32 max_change_down; // 0:255
} ik_hi_shpns_both_b_t;

typedef struct {
    uint32 coring[IK_NUM_CORING_TABLE_INDEX];
} ik_hi_shpns_coring_b_t;

typedef ik_hi_shpns_both_b_t ik_li2_shpns_both_t;
typedef ik_hi_shpns_coring_b_t ik_li2_shpns_coring_t;

//shp hiso low high
typedef ik_li2_shpns_both_t ik_hili_shpns_both_t;
typedef ik_li2_shpns_fir_t ik_hili_shpns_fir_t;
typedef ik_li2_shpns_noise_t ik_hili_shpns_noise_t;
typedef ik_li2_shpns_coring_t ik_hili_shpns_coring_t;
typedef ik_li2_shpns_cor_idx_scl_t ik_hili_shpns_cor_idx_scl_t;
typedef ik_li2_shpns_min_cor_rst_t ik_hili_shpns_min_cor_rst_t;
typedef ik_li2_shpns_max_cor_rst_t ik_hili_shpns_max_cor_rst_t;
typedef ik_li2_shpns_scl_cor_t ik_hili_shpns_scl_cor_t;

//chroma filter
typedef ik_chroma_filter_t ik_hi_chroma_filter_high_t;

typedef struct {
    uint32 enable;
    uint32 noise_level_cb;          /* 0-255 */
    uint32 noise_level_cr;          /* 0-255 */
    uint32 original_blend_strength_cb; /* Cb 0-256  */
    uint32 original_blend_strength_cr; /* Cr 0-256  */
} ik_hi_chroma_filter_b_t;

typedef ik_hi_chroma_filter_b_t ik_hi_chroma_filter_pre_t;
typedef ik_hi_chroma_filter_b_t ik_hi_chroma_filter_med_t;
typedef ik_hi_chroma_filter_b_t ik_hi_chroma_filter_low_t;
typedef ik_hi_chroma_filter_b_t ik_hi_chroma_filter_very_low_t;

// combine
typedef struct {
    uint32 t0;
    uint32 t1;
    uint32 alpha_max;
    uint32 alpha_min;
    uint32 signal_preserve;
    ik_level_method_t t0_t1_add;
    ik_level_method_t max_change;
    ik_level_method_t min_tar;
    ik_level_t        min_tar_cb;
    ik_level_t        min_tar_cr;
} ik_hi_luma_combine_t;

typedef ik_hi_luma_combine_t ik_hi_low_asf_combine_t;

typedef struct {
    uint32 t0_cb;
    uint32 t0_cr;
    uint32 t1_cb;
    uint32 t1_cr;
    uint32 alpha_max_cb;
    uint32 alpha_max_cr;
    uint32 alpha_min_cb;
    uint32 alpha_min_cr;
    ik_level_t max_change_cb;
    ik_level_t max_change_cr;
    ik_level_t to_t1_add_cb;
    ik_level_t to_t1_add_cr;
    uint32 signal_preserve_cb;
    uint32 signal_preserve_cr;
} ik_hi_chroma_filter_combine_t;

typedef ik_hi_chroma_filter_combine_t ik_hi_chroma_fltr_med_com_t;
typedef ik_hi_chroma_filter_combine_t ik_hi_chroma_fltr_low_com_t;
typedef ik_hi_chroma_filter_combine_t ik_hi_chroma_fltr_very_low_com_t;

typedef struct {
    uint32 t0_cb;
    uint32 t0_cr;
    uint32 t0_y;
    uint32 t1_cb;
    uint32 t1_cr;
    uint32 t1_y;
    uint32 alpha_max_y;
    uint32 alpha_max_cb;
    uint32 alpha_max_cr;
    uint32 alpha_min_y;
    uint32 alpha_min_cb;
    uint32 alpha_min_cr;
    ik_level_method_t max_change_y;
    ik_level_t max_change_cb;
    ik_level_t max_change_cr;
    ik_level_method_t to_t1_add_y;
    ik_level_t to_t1_add_cb;
    ik_level_t to_t1_add_cr;
    ik_level_method_t min_tar_y;
    ik_level_t min_tar_cb;
    ik_level_t min_tar_cr;
    uint32 signal_preserve_cb;
    uint32 signal_preserve_cr;
    uint32 signal_preserve_y;
} ik_hili_combine_t;

typedef struct {
    ik_sharpen_noise_sharpen_fir_t fir;
    uint32 max_down;
    uint32 max_up;
    ik_level_method_t scale_level;
} ik_hi_mid_high_freq_recover_t;

typedef struct {
    ik_level_method_t luma_level;
} ik_hi_luma_blend_t;

typedef struct {
    uint32 smooth;
    int32 lev_adjust[16];
} ik_hi_nonsmooth_detect_t;

typedef ik_first_luma_process_mode_t ik_hi_select_t;

typedef ik_wide_chroma_filter_t ik_hi_wide_chroma_filter_t;
typedef ik_wide_chroma_filter_combine_t ik_hi_wide_chroma_filter_combine_t;
typedef ik_wide_chroma_filter_t ik_li2_wide_chroma_filter_t;
typedef ik_wide_chroma_filter_combine_t ik_li2_wide_chroma_filter_combine_t;
typedef ik_wide_chroma_filter_t ik_hi_wide_chroma_filter_high_t;
typedef ik_wide_chroma_filter_combine_t ik_hi_wide_chroma_filter_combine_high_t;
typedef ik_wide_chroma_filter_t ik_hi_wide_chroma_filter_med_t;
typedef ik_wide_chroma_filter_combine_t ik_hi_wide_chroma_filter_combine_med_t;
typedef ik_wide_chroma_filter_t ik_hi_wide_chroma_filter_low_t;
typedef ik_wide_chroma_filter_combine_t ik_hi_wide_chroma_filter_combine_low_t;
typedef ik_wide_chroma_filter_t ik_hi_wide_chroma_filter_very_low_t;
typedef ik_wide_chroma_filter_combine_t ik_hi_wide_chroma_filter_combine_very_low_t;
typedef ik_wide_chroma_filter_t ik_hi_wide_chroma_filter_pre_t;
typedef ik_wide_chroma_filter_combine_t ik_hi_wide_chroma_filter_pre_combine_t;

// Api
// hiso
uint32 ik_set_hi_anti_aliasing(uint32 context_id, const ik_hi_anti_aliasing_t *p_hi_anti_aliasing);
uint32 ik_get_hi_anti_aliasing(uint32 context_id, ik_hi_anti_aliasing_t *p_hi_anti_aliasing);

uint32 ik_set_hi_cfa_leakage_filter(uint32 context_id, const ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);
uint32 ik_get_hi_cfa_leakage_filter(uint32 context_id, ik_hi_cfa_leakage_filter_t *p_hi_cfa_leakage_filter);

uint32 ik_set_hi_dynamic_bpc(uint32 context_id, const ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);
uint32 ik_get_hi_dynamic_bpc(uint32 context_id, ik_hi_dynamic_bpc_t *p_hi_dynamic_bpc);

uint32 ik_set_hi_grgb_mismatch(uint32 context_id, const ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);
uint32 ik_get_hi_grgb_mismatch(uint32 context_id, ik_hi_grgb_mismatch_t *p_hi_grgb_mismatch);

uint32 ik_set_hi_chroma_median_filter(uint32 context_id, const ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);
uint32 ik_get_hi_chroma_median_filter(uint32 context_id, ik_hi_chroma_median_filter_t *p_hi_chroma_median_filter);

uint32 ik_set_hi_cfa_noise_filter(uint32 context_id, const ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);
uint32 ik_get_hi_cfa_noise_filter(uint32 context_id, ik_hi_cfa_noise_filter_t *p_hi_cfa_noise_filter);

uint32 ik_set_hi_demosaic(uint32 context_id, const ik_hi_demosaic_t *p_hi_demosaic);
uint32 ik_get_hi_demosaic(uint32 context_id, ik_hi_demosaic_t *p_hi_demosaic);

//hiso low2
uint32 ik_set_li2_anti_aliasing(uint32 context_id, const ik_li2_anti_aliasing_t *p_li2_anti_aliasing);
uint32 ik_get_li2_anti_aliasing(uint32 context_id, ik_li2_anti_aliasing_t *p_li2_anti_aliasing);

uint32 ik_set_li2_cfa_leakage_filter(uint32 context_id, const ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);
uint32 ik_get_li2_cfa_leakage_filter(uint32 context_id, ik_li2_cfa_leakage_filter_t *p_li2_cfa_leakage_filter);

uint32 ik_set_li2_dynamic_bpc(uint32 context_id, const ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);
uint32 ik_get_li2_dynamic_bpc(uint32 context_id, ik_li2_dynamic_bpc_t *p_li2_dynamic_bpc);

uint32 ik_set_li2_grgb_mismatch(uint32 context_id, const ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);
uint32 ik_get_li2_grgb_mismatch(uint32 context_id, ik_li2_grgb_mismatch_t *p_li2_grgb_mismatch);

uint32 ik_set_li2_cfa_noise_filter(uint32 context_id, const ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);
uint32 ik_get_li2_cfa_noise_filter(uint32 context_id, ik_li2_cfa_noise_filter_t *p_li2_cfa_noise_filter);

uint32 ik_set_li2_demosaic(uint32 context_id, const ik_li2_demosaic_t *p_li2_demosaic);
uint32 ik_get_li2_demosaic(uint32 context_id, ik_li2_demosaic_t *p_li2_demosaic);

//asf
uint32 ik_set_hi_asf(uint32 context_id, const ik_hi_asf_t *p_hi_asf);
uint32 ik_get_hi_asf(uint32 context_id, ik_hi_asf_t *p_hi_asf);

uint32 ik_set_li2_asf(uint32 context_id, const ik_li2_asf_t *p_li2_asf);
uint32 ik_get_li2_asf(uint32 context_id, ik_li2_asf_t *p_li2_asf);

uint32 ik_set_hi_low_asf(uint32 context_id, const ik_hi_low_asf_t *p_hi_low_asf);
uint32 ik_get_hi_low_asf(uint32 context_id, ik_hi_low_asf_t *p_hi_low_asf);

uint32 ik_set_hi_med1_asf(uint32 context_id, const ik_hi_med1_asf_t *p_hi_med1_asf);
uint32 ik_get_hi_med1_asf(uint32 context_id, ik_hi_med1_asf_t *p_hi_med1_asf);

uint32 ik_set_hi_med2_asf(uint32 context_id, const ik_hi_med2_asf_t *p_hi_med2_asf);
uint32 ik_get_hi_med2_asf(uint32 context_id, ik_hi_med2_asf_t *p_hi_med2_asf);

uint32 ik_set_hi_high_asf(uint32 context_id, const ik_hi_high_asf_t *p_hi_high_asf);
uint32 ik_get_hi_high_asf(uint32 context_id, ik_hi_high_asf_t *p_hi_high_asf);

uint32 ik_set_hi_high2_asf(uint32 context_id, const ik_hi_high2_asf_t *p_hi_high2_asf);
uint32 ik_get_hi_high2_asf(uint32 context_id, ik_hi_high2_asf_t *p_hi_high2_asf);

uint32 ik_set_li_chroma_asf(uint32 context_id, const ik_chroma_asf_t *p_chroma_asf);
uint32 ik_get_li_chroma_asf(uint32 context_id, ik_chroma_asf_t *p_chroma_asf);

uint32 ik_set_hi_chroma_asf(uint32 context_id, const ik_hi_chroma_asf_t *p_hi_chroma_asf);
uint32 ik_get_hi_chroma_asf(uint32 context_id, ik_hi_chroma_asf_t *p_hi_chroma_asf);

uint32 ik_set_hi_low_chroma_asf(uint32 context_id, const ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);
uint32 ik_get_hi_low_chroma_asf(uint32 context_id, ik_hi_low_chroma_asf_t *p_hi_low_chroma_asf);

// sharpen
// --high--
uint32 ik_set_hi_high_shpns_both(uint32 context_id, const ik_hi_high_shpns_both_t *p_hi_high_shpns_both);
uint32 ik_get_hi_high_shpns_both(uint32 context_id, ik_hi_high_shpns_both_t *p_hi_high_shpns_both);

uint32 ik_set_hi_high_shpns_noise(uint32 context_id, const ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);
uint32 ik_get_hi_high_shpns_noise(uint32 context_id, ik_hi_high_shpns_noise_t *p_hi_high_shpns_noise);

uint32 ik_set_hi_high_shpns_coring(uint32 context_id, const ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);
uint32 ik_get_hi_high_shpns_coring(uint32 context_id, ik_hi_high_shpns_coring_t *p_hi_high_shpns_coring);

uint32 ik_set_hi_high_shpns_fir(uint32 context_id, const ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);
uint32 ik_get_hi_high_shpns_fir(uint32 context_id, ik_hi_high_shpns_fir_t *p_hi_high_shpns_fir);

uint32 ik_set_hi_high_shpns_cor_idx_scl(uint32 context_id, const ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);
uint32 ik_get_hi_high_shpns_cor_idx_scl(uint32 context_id, ik_hi_high_shpns_cor_idx_scl_t *p_hi_high_shpns_cor_idx_scl);

uint32 ik_set_hi_high_shpns_min_cor_rst(uint32 context_id, const ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);
uint32 ik_get_hi_high_shpns_min_cor_rst(uint32 context_id, ik_hi_high_shpns_min_cor_rst_t *p_hi_high_shpns_min_cor_rst);

uint32 ik_set_hi_high_shpns_max_cor_rst(uint32 context_id, const ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);
uint32 ik_get_hi_high_shpns_max_cor_rst(uint32 context_id, ik_hi_high_shpns_max_cor_rst_t *p_hi_high_shpns_max_cor_rst);

uint32 ik_set_hi_high_shpns_scl_cor(uint32 context_id, const ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);
uint32 ik_get_hi_high_shpns_scl_cor(uint32 context_id, ik_hi_high_shpns_scl_cor_t *p_hi_high_shpns_scl_cor);

// --med--
uint32 ik_set_hi_med_shpns_both(uint32 context_id, const ik_hi_med_shpns_both_t *p_hi_med_shpns_both);
uint32 ik_get_hi_med_shpns_both(uint32 context_id, ik_hi_med_shpns_both_t *p_hi_med_shpns_both);

uint32 ik_set_hi_med_shpns_noise(uint32 context_id, const ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);
uint32 ik_get_hi_med_shpns_noise(uint32 context_id, ik_hi_med_shpns_noise_t *p_hi_med_shpns_noise);

uint32 ik_set_hi_med_shpns_coring(uint32 context_id, const ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);
uint32 ik_get_hi_med_shpns_coring(uint32 context_id, ik_hi_med_shpns_coring_t *p_hi_med_shpns_coring);

uint32 ik_set_hi_med_shpns_fir(uint32 context_id, const ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);
uint32 ik_get_hi_med_shpns_fir(uint32 context_id, ik_hi_med_shpns_fir_t *p_hi_med_shpns_fir);

uint32 ik_set_hi_med_shpns_cor_idx_scl(uint32 context_id, const ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);
uint32 ik_get_hi_med_shpns_cor_idx_scl(uint32 context_id, ik_hi_med_shpns_cor_idx_scl_t *p_hi_med_shpns_cor_idx_scl);

uint32 ik_set_hi_med_shpns_min_cor_rst(uint32 context_id, const ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);
uint32 ik_get_hi_med_shpns_min_cor_rst(uint32 context_id, ik_hi_med_shpns_min_cor_rst_t *p_hi_med_shpns_min_cor_rst);

uint32 ik_set_hi_med_shpns_max_cor_rst(uint32 context_id, const ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);
uint32 ik_get_hi_med_shpns_max_cor_rst(uint32 context_id, ik_hi_med_shpns_max_cor_rst_t *p_hi_med_shpns_max_cor_rst);

uint32 ik_set_hi_med_shpns_scl_cor(uint32 context_id, const ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);
uint32 ik_get_hi_med_shpns_scl_cor(uint32 context_id, ik_hi_med_shpns_scl_cor_t *p_hi_med_shpns_scl_cor);

// --low2--
uint32 ik_set_li2_shpns_both(uint32 context_id, const ik_li2_shpns_both_t *p_li2_shpns_both);
uint32 ik_get_li2_shpns_both(uint32 context_id, ik_li2_shpns_both_t *p_li2_shpns_both);

uint32 ik_set_li2_shpns_noise(uint32 context_id, const ik_li2_shpns_noise_t *p_li2_shpns_noise);
uint32 ik_get_li2_shpns_noise(uint32 context_id, ik_li2_shpns_noise_t *p_li2_shpns_noise);

uint32 ik_set_li2_shpns_coring(uint32 context_id, const ik_li2_shpns_coring_t *p_li2_shpns_coring);
uint32 ik_get_li2_shpns_coring(uint32 context_id, ik_li2_shpns_coring_t *p_li2_shpns_coring);

uint32 ik_set_li2_shpns_fir(uint32 context_id, const ik_li2_shpns_fir_t *p_li2_shpns_fir);
uint32 ik_get_li2_shpns_fir(uint32 context_id, ik_li2_shpns_fir_t *p_li2_shpns_fir);

uint32 ik_set_li2_shpns_cor_idx_scl(uint32 context_id, const ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);
uint32 ik_get_li2_shpns_cor_idx_scl(uint32 context_id, ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl);

uint32 ik_set_li2_shpns_min_cor_rst(uint32 context_id, const ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);
uint32 ik_get_li2_shpns_min_cor_rst(uint32 context_id, ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst);

uint32 ik_set_li2_shpns_max_cor_rst(uint32 context_id, const ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);
uint32 ik_get_li2_shpns_max_cor_rst(uint32 context_id, ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst);

uint32 ik_set_li2_shpns_scl_cor(uint32 context_id, const ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);
uint32 ik_get_li2_shpns_scl_cor(uint32 context_id, ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor);

// --hili--
uint32 ik_set_hili_shpns_both(uint32 context_id, const ik_hili_shpns_both_t *p_hili_shpns_both);
uint32 ik_get_hili_shpns_both(uint32 context_id, ik_hili_shpns_both_t *p_hili_shpns_both);

uint32 ik_set_hili_shpns_noise(uint32 context_id, const ik_hili_shpns_noise_t *p_hili_shpns_noise);
uint32 ik_get_hili_shpns_noise(uint32 context_id, ik_hili_shpns_noise_t *p_hili_shpns_noise);

uint32 ik_set_hili_shpns_coring(uint32 context_id, const ik_hili_shpns_coring_t *p_hili_shpns_coring);
uint32 ik_get_hili_shpns_coring(uint32 context_id, ik_hili_shpns_coring_t *p_hili_shpns_coring);

uint32 ik_set_hili_shpns_fir(uint32 context_id, const ik_hili_shpns_fir_t *p_hili_shpns_fir);
uint32 ik_get_hili_shpns_fir(uint32 context_id, ik_hili_shpns_fir_t *p_hili_shpns_fir);

uint32 ik_set_hili_shpns_cor_idx_scl(uint32 context_id, const ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);
uint32 ik_get_hili_shpns_cor_idx_scl(uint32 context_id, ik_hili_shpns_cor_idx_scl_t *p_hili_shpns_cor_idx_scl);

uint32 ik_set_hili_shpns_min_cor_rst(uint32 context_id, const ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);
uint32 ik_get_hili_shpns_min_cor_rst(uint32 context_id, ik_hili_shpns_min_cor_rst_t *p_hili_shpns_min_cor_rst);

uint32 ik_set_hili_shpns_max_cor_rst(uint32 context_id, const ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);
uint32 ik_get_hili_shpns_max_cor_rst(uint32 context_id, ik_hili_shpns_max_cor_rst_t *p_hili_shpns_max_cor_rst);

uint32 ik_set_hili_shpns_scl_cor(uint32 context_id, const ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);
uint32 ik_get_hili_shpns_scl_cor(uint32 context_id, ik_hili_shpns_scl_cor_t *p_hili_shpns_scl_cor);

//chroma filter
uint32 ik_set_hi_chroma_filter_high(uint32 context_id, const ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);
uint32 ik_get_hi_chroma_filter_high(uint32 context_id, ik_hi_chroma_filter_high_t *p_hi_chroma_filter_high);

uint32 ik_set_hi_chroma_filter_pre(uint32 context_id, const ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);
uint32 ik_get_hi_chroma_filter_pre(uint32 context_id, ik_hi_chroma_filter_pre_t *p_hi_chroma_filter_pre);

uint32 ik_set_hi_chroma_filter_med(uint32 context_id, const ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);
uint32 ik_get_hi_chroma_filter_med(uint32 context_id, ik_hi_chroma_filter_med_t *p_hi_chroma_filter_med);

uint32 ik_set_hi_chroma_filter_low(uint32 context_id, const ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);
uint32 ik_get_hi_chroma_filter_low(uint32 context_id, ik_hi_chroma_filter_low_t *p_hi_chroma_filter_low);

uint32 ik_set_hi_chroma_filter_very_low(uint32 context_id, const ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);
uint32 ik_get_hi_chroma_filter_very_low(uint32 context_id, ik_hi_chroma_filter_very_low_t *p_hi_chroma_filter_very_low);

//combine
uint32 ik_set_hi_luma_combine(uint32 context_id, const ik_hi_luma_combine_t *p_hi_luma_combine);
uint32 ik_get_hi_luma_combine(uint32 context_id, ik_hi_luma_combine_t *p_hi_luma_combine);

uint32 ik_set_hi_low_asf_combine(uint32 context_id, const ik_hi_low_asf_combine_t *p_hi_low_asf_combine);
uint32 ik_get_hi_low_asf_combine(uint32 context_id, ik_hi_low_asf_combine_t *p_hi_low_asf_combine);

uint32 ik_set_hi_chroma_fltr_med_com(uint32 context_id, const ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);
uint32 ik_get_hi_chroma_fltr_med_com(uint32 context_id, ik_hi_chroma_fltr_med_com_t *p_hi_chroma_fltr_med_com);

uint32 ik_set_hi_chroma_fltr_low_com(uint32 context_id, const ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);
uint32 ik_get_hi_chroma_fltr_low_com(uint32 context_id, ik_hi_chroma_fltr_low_com_t *p_hi_chroma_fltr_low_com);

uint32 ik_set_hi_chroma_fltr_very_low_com(uint32 context_id, const ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);
uint32 ik_get_hi_chroma_fltr_very_low_com(uint32 context_id, ik_hi_chroma_fltr_very_low_com_t *p_hi_chroma_fltr_very_low_com);

uint32 ik_set_hili_combine(uint32 context_id, const ik_hili_combine_t *p_hili_combine);
uint32 ik_get_hili_combine(uint32 context_id, ik_hili_combine_t *p_hili_combine);

uint32 ik_set_hi_mid_high_freq_recover(uint32 context_id, const ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);
uint32 ik_get_hi_mid_high_freq_recover(uint32 context_id, ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover);

uint32 ik_set_hi_luma_blend(uint32 context_id, const ik_hi_luma_blend_t *p_hi_luma_blend);
uint32 ik_get_hi_luma_blend(uint32 context_id, ik_hi_luma_blend_t *p_hi_luma_blend);

uint32 ik_set_hi_nonsmooth_detect(uint32 context_id, const ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);
uint32 ik_get_hi_nonsmooth_detect(uint32 context_id, ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect);

uint32 ik_set_hi_select(uint32 context_id, const ik_hi_select_t *p_hi_select);
uint32 ik_get_hi_select(uint32 context_id, ik_hi_select_t *p_hi_select);

uint32 ik_set_hi_wide_chroma_filter(uint32 context_id, const ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);
uint32 ik_get_hi_wide_chroma_filter(uint32 context_id, ik_hi_wide_chroma_filter_t *p_hi_wide_chroma_filter);

uint32 ik_set_hi_wide_chroma_filter_combine(uint32 context_id, const ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);
uint32 ik_get_hi_wide_chroma_filter_combine(uint32 context_id, ik_hi_wide_chroma_filter_combine_t *p_hi_wide_chroma_filter_combine);

uint32 ik_set_li2_wide_chroma_filter(uint32 context_id, const ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);
uint32 ik_get_li2_wide_chroma_filter(uint32 context_id, ik_li2_wide_chroma_filter_t *p_li2_wide_chroma_filter);

uint32 ik_set_li2_wide_chroma_filter_combine(uint32 context_id, const ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);
uint32 ik_get_li2_wide_chroma_filter_combine(uint32 context_id, ik_li2_wide_chroma_filter_combine_t *p_li2_wide_chroma_filter_combine);

uint32 ik_set_hi_wide_chroma_filter_high(uint32 context_id, const ik_hi_wide_chroma_filter_high_t *p_hi_wide_chroma_filter_high);
uint32 ik_get_hi_wide_chroma_filter_high(uint32 context_id, ik_hi_wide_chroma_filter_high_t *p_hi_wide_chroma_filter_high);

uint32 ik_set_hi_wide_chroma_filter_combine_high(uint32 context_id, const ik_hi_wide_chroma_filter_combine_high_t *p_hi_wide_chroma_filter_combine_high);
uint32 ik_get_hi_wide_chroma_filter_combine_high(uint32 context_id, ik_hi_wide_chroma_filter_combine_high_t *p_hi_wide_chroma_filter_combine_high);

uint32 ik_set_hi_wide_chroma_filter_med(uint32 context_id, const ik_hi_wide_chroma_filter_med_t *p_hi_wide_chroma_filter_med);
uint32 ik_get_hi_wide_chroma_filter_med(uint32 context_id, ik_hi_wide_chroma_filter_med_t *p_hi_wide_chroma_filter_med);

uint32 ik_set_hi_wide_chroma_filter_combine_med(uint32 context_id, const ik_hi_wide_chroma_filter_combine_med_t *p_hi_wide_chroma_filter_combine_med);
uint32 ik_get_hi_wide_chroma_filter_combine_med(uint32 context_id, ik_hi_wide_chroma_filter_combine_med_t *p_hi_wide_chroma_filter_combine_med);

uint32 ik_set_hi_wide_chroma_filter_low(uint32 context_id, const ik_hi_wide_chroma_filter_low_t *p_hi_wide_chroma_filter_low);
uint32 ik_get_hi_wide_chroma_filter_low(uint32 context_id, ik_hi_wide_chroma_filter_low_t *p_hi_wide_chroma_filter_low);

uint32 ik_set_hi_wide_chroma_filter_combine_low(uint32 context_id, const ik_hi_wide_chroma_filter_combine_low_t *p_hi_wide_chroma_filter_combine_low);
uint32 ik_get_hi_wide_chroma_filter_combine_low(uint32 context_id, ik_hi_wide_chroma_filter_combine_low_t *p_hi_wide_chroma_filter_combine_low);

uint32 ik_set_hi_wide_chroma_filter_very_low(uint32 context_id, const ik_hi_wide_chroma_filter_very_low_t *p_hi_wide_chroma_filter_very_low);
uint32 ik_get_hi_wide_chroma_filter_very_low(uint32 context_id, ik_hi_wide_chroma_filter_very_low_t *p_hi_wide_chroma_filter_very_low);

uint32 ik_set_hi_wide_chroma_filter_combine_very_low(uint32 context_id, const ik_hi_wide_chroma_filter_combine_very_low_t *p_hi_wide_chroma_filter_combine_very_low);
uint32 ik_get_hi_wide_chroma_filter_combine_very_low(uint32 context_id, ik_hi_wide_chroma_filter_combine_very_low_t *p_hi_wide_chroma_filter_combine_very_low);

uint32 ik_set_hi_wide_chroma_filter_pre(uint32 context_id, const ik_hi_wide_chroma_filter_pre_t *p_hi_wide_chroma_filter_pre);
uint32 ik_get_hi_wide_chroma_filter_pre(uint32 context_id, ik_hi_wide_chroma_filter_pre_t *p_hi_wide_chroma_filter_pre);

uint32 ik_set_hi_wide_chroma_filter_pre_combine(uint32 context_id, const ik_hi_wide_chroma_filter_pre_combine_t *p_hi_wide_chroma_filter_pre_combine);
uint32 ik_get_hi_wide_chroma_filter_pre_combine(uint32 context_id, ik_hi_wide_chroma_filter_pre_combine_t *p_hi_wide_chroma_filter_pre_combine);


uint32 ik_set_hi_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_hi_luma_noise_reduce);
uint32 ik_get_hi_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_hi_luma_noise_reduce);

uint32 ik_set_li2_luma_noise_reduction(uint32 context_id, const ik_luma_noise_reduction_t *p_li2_luma_noise_reduce);
uint32 ik_get_li2_luma_noise_reduction(uint32 context_id, ik_luma_noise_reduction_t *p_li2_luma_noise_reduce);

#endif
