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

#ifndef IMG_FILTERS_IF_H
#define IMG_FILTERS_IF_H

#include "AmbaDSP_ImgIkcSpec.h"
#define IK_NUM_CORING_TABLE_INDEX  (256U)
#define IK_NUM_CHROMA_GAIN_CURVE   (128U)
#define IK_NUM_EXPOSURE_CURVE      (256U)
#define IK_NUM_TONE_CURVE          (256U)
#define IK_CC_REG_SIZE             (18752U)
#define IK_CC_3D_SIZE              (17536U)
#define IK_CE_INPUT_TABLE_SIZE     (449U)
#define IK_CE_OUTPUT_TABLE_SIZE    (256U)
#define IK_CE_CORING_TABLE_SIZE    (64U)
#define IK_CE_BOOST_TABLE_SIZE     (64U)
#define IK_NUM_DECOMPAND_TABLE     (145U)
#define IK_NUM_COMPAND_TABLE       (45U)
#define IK_NUM_THREE_D_TABLE       (8192U)

#define IK_SBP_VER                 (0x20180401UL)
#define IK_VIG_VER                 (0x20180401UL)
#define IK_CA_VER                  (0x20180401UL)
#define IK_WARP_VER                (0x20180401UL)
#define IK_WARP_DISABLE            (0)
#define IK_WARP_ENABLE             (1)
#define IK_WARP_ENABLE_INTERNAL    (2)
#define IK_WARP_VIDEO_MAXHORGRID   (128UL)
#define IK_WARP_VIDEO_MAXVERGRID   (96UL)
#define IK_WARP_MAX_SLICE_NUMBER   (12UL)

#ifndef IK_FLIP_DISABLED
#define IK_FLIP_DISABLED         (0x0UL)
#endif
#ifndef IK_HORIZONTAL_FLIP
#define IK_HORIZONTAL_FLIP         (0x1UL)
#endif
#ifndef IK_VERTICAL_FLIP
#define IK_VERTICAL_FLIP           (0x2UL)
#endif
#ifndef IK_FLIP_RAW_H
#define IK_FLIP_RAW_H               (IK_HORIZONTAL_FLIP)
#endif
#ifndef IK_FLIP_RAW_V
#define IK_FLIP_RAW_V               (IK_VERTICAL_FLIP)
#endif
#ifndef IK_FLIP_RAW_H_RAW_V
#define IK_FLIP_RAW_H_RAW_V          (IK_HORIZONTAL_FLIP + IK_VERTICAL_FLIP)
#endif
#ifndef IK_FLIP_YUV_H
#define IK_FLIP_YUV_H              (IK_HORIZONTAL_FLIP<<16UL)
#endif
#ifndef IK_FLIP_YUV_V
#define IK_FLIP_YUV_V              (IK_VERTICAL_FLIP<<16UL)
#endif
#ifndef IK_FLIP_RAW_H_RAW_V
#define IK_FLIP_RAW_H_RAW_V        ((IK_HORIZONTAL_FLIP + IK_VERTICAL_FLIP)<<16UL)
#endif
#ifndef IK_FLIP_FE_H_DMA_V
#define IK_FLIP_RAW_H_YUV_V         ((IK_HORIZONTAL_FLIP) + (IK_VERTICAL_FLIP<<16UL))
#endif
#ifndef IK_FLIP_RAW_V_DMA_H
#define IK_FLIP_RAW_V_YUV_H         ((IK_VERTICAL_FLIP) + (IK_HORIZONTAL_FLIP<<16UL))
#endif

#ifndef IK_RAW_COMPRESS_OFF
#define IK_RAW_COMPRESS_OFF     0UL
#define IK_RAW_COMPRESS_6P75    1UL
#define IK_RAW_COMPRESS_7p5     2UL
#define IK_RAW_COMPRESS_7p75    3UL
#define IK_RAW_COMPRESS_8p5     4UL
#define IK_RAW_COMPRESS_8p75    5UL
#define IK_RAW_COMPRESS_9p5     6UL
#define IK_RAW_COMPRESS_9p75    7UL
#define IK_RAW_COMPRESS_10p5    8UL
#define IK_RAW_COMPACT_10B      257UL
#define IK_RAW_COMPACT_12B      258UL
#define IK_RAW_COMPACT_14B      259UL
#endif

#ifndef IK_SENSOR_MODE_NORMAL
#define IK_SENSOR_MODE_NORMAL     0UL
#define IK_SENSOR_MODE_RGBIR      1UL
#define IK_SENSOR_MODE_RCCC       2UL
#endif
typedef struct {
    uint32 step;
    uint32 mode;
    uint32 channel_id;
    uint32 tile_x;
    uint32 tile_y;
    uint32 pic_num;
} img_dsp_img_debug_mode_t;

typedef struct {
    uint32 vin_id;
    uint32 sensor_resolution;   /* Number of bits for data representation */
    uint32 sensor_pattern;      /* Bayer patterns RG, BG, GR, GB */
    uint32 sensor_mode;         /* 0: normal 1:RGB-IR 2:RCCC*/
    uint32 compression;
    int32  compression_offset;
    uint32 sensor_readout_mode;
} ik_vin_sensor_info_t;

typedef struct {
    uint32 global_dgain; // 12.12 format, default = 4096
} ik_global_dgain_t;

typedef struct {
    uint32 ae_gain; // 12.12 format, default = 4096
} ik_ae_gain_t;

typedef struct {
    uint32 gain_r;
    uint32 gain_g;
    uint32 gain_b;
} ik_wb_gain_t;

typedef struct {
    uint32 enable;
    uint32 alpha_rr;
    uint32 alpha_rb;
    uint32 alpha_br;
    uint32 alpha_bb;
    uint32 saturation_level;
} ik_cfa_leakage_filter_t;

typedef struct {
    uint32 enable;
    uint32 thresh;
    uint32 log_fractional_correct;
} ik_anti_aliasing_t;

typedef struct {
    uint32 enable;
    uint32 hot_pixel_strength;
    uint32 dark_pixel_strength;
    uint32 correction_method;
    uint32 hot_pixel_detection_strength_irmode_redblue;
    uint32 dark_pixel_detection_strength_irmode_redblue;
} ik_dynamic_bad_pixel_correction_t;

typedef struct {
    uint32 enable;
    uint32 noise_level[3];          /* R/G/B, 0-8192 */
    uint32 original_blend_str[3];    /* R/G/B, 0-256 */
    uint32 extent_regular[3];       /* R/G/B, 0-256 */
    uint32 extent_fine[3];          /* R/G/B, 0-256 */
    uint32 strength_fine[3];        /* R/G/B, 0-256 */
    uint32 selectivity_regular;     /* 0-256 */
    uint32 selectivity_fine;        /* 0-256 */
    uint32 directional_enable;
    uint32 directional_horvert_edge_strength;
    uint32 directional_horvert_strength_bias;
    uint32 directional_horvert_original_blend_strength;
} ik_cfa_noise_filter_t;

typedef struct {
    uint32 level_red;       /* 14:0 */
    uint32 level_green_even;
    uint32 level_green_odd;
    uint32 level_blue;
} ik_dgain_saturation_level_t;

typedef struct {
    uint32 enable;
    uint32 activity_thresh;
    uint32 activity_difference_thresh;
    uint32 grad_clip_thresh;
    uint32 grad_noise_thresh;
    uint32 alias_interpolation_strength;
    uint32 alias_interpolation_thresh;
} ik_demosaic_t;

typedef struct {
    uint32 reg_setting_table[IK_CC_REG_SIZE/4U];
} ik_color_correction_reg_t;

typedef struct {
    uint32 matrix_three_d_table[IK_CC_3D_SIZE/4U];
} ik_color_correction_t;

typedef struct {
    uint32 pre_cc_gain_enable;
    uint32 pre_cc_gain_r;
    uint32 pre_cc_gain_g;
    uint32 pre_cc_gain_b;
} ik_pre_cc_gain_t;

typedef struct {
    uint32 tone_curve_red[IK_NUM_TONE_CURVE];
    uint32 tone_curve_green[IK_NUM_TONE_CURVE];
    uint32 tone_curve_blue[IK_NUM_TONE_CURVE];
} ik_tone_curve_t;

typedef struct {
    int32 matrix_values[9];
    int32 y_offset;
    int32 u_offset;
    int32 v_offset;
} ik_rgb_to_yuv_matrix_t;

typedef struct {
    uint32 mode;
    int32 ircorrect_offset_b;
    int32 ircorrect_offset_gb;
    int32 ircorrect_offset_gr;
    int32 ircorrect_offset_r;
    uint32 mul_base_val;
    uint32 mul_delta_high;
    uint32 mul_high;
    uint32 mul_high_val;
    uint32 maintain_color_balance;
    uint32 ir_only;
} ik_rgb_ir_t;

typedef struct {
    uint32 enable;
    uint32 gain_curve[IK_NUM_CHROMA_GAIN_CURVE];
} ik_chroma_scale_t;

typedef struct {
    uint32 enable;
    uint32 cb_adaptive_strength;
    uint32 cr_adaptive_strength;
    uint32 cb_non_adaptive_strength;
    uint32 cr_non_adaptive_strength;
    uint32 cb_adaptive_amount;
    uint32 cr_adaptive_amount;
} ik_chroma_median_filter_t;

typedef struct {
    uint32 use_sharpen_not_asf;
} ik_first_luma_process_mode_t;

typedef struct {
    uint32 temporal_alpha0; // 0-255
    uint32 temporal_alpha1; // 0-255
    uint32 temporal_alpha2; // 0-255
    uint32 temporal_alpha3; // 0-255
    uint32 temporal_t0; // 0-63
    uint32 temporal_t1; // 0-63
    uint32 temporal_t2; // 0-63
    uint32 temporal_t3; // 0-63
} ik_video_mctf_curve_t;

typedef struct {
    uint32 low;
    uint32 low_delta;
    uint32 low_strength;
    uint32 mid_strength;
    uint32 high;
    uint32 high_delta;
    uint32 high_strength;
} ik_level_t;

typedef struct {
    uint32 low;
    uint32 low_delta;
    uint32 low_strength;
    uint32 mid_strength;
    uint32 high;
    uint32 high_delta;
    uint32 high_strength;
    uint32 method;
} ik_level_method_t;

typedef struct {
    uint32 alpha_min_up;
    uint32 alpha_max_up;
    uint32 t0_up;
    uint32 t1_up;
    uint32 alpha_min_down;
    uint32 alpha_max_down;
    uint32 t0_down;
    uint32 t1_down;
} ik_full_adaptation_t;

typedef struct {
    uint32 specify;
    uint32 per_dir_fir_iso_strengths[9];
    uint32 per_dir_fir_dir_strengths[9];
    uint32 per_dir_fir_dir_amounts[9];
    int32  coefs[9][25];
    uint32 strength_iso;
    uint32 strength_dir;
    uint32 wide_edge_detect;
} ik_asf_fir_t;

typedef struct {
    uint32 enable;
    ik_asf_fir_t fir;
    uint32 directional_decide_t0;
    uint32 directional_decide_t1;
    ik_full_adaptation_t adapt;
    ik_level_t level_str_adjust;
    ik_level_t t0_t1_div;
    uint32 max_change_not_t0_t1_alpha;
    uint32 max_change_up;
    uint32 max_change_down;
} ik_adv_spatial_filter_t;

typedef ik_level_t ik_first_sharpen_coring_idx_scale_t;
typedef ik_level_method_t ik_final_sharpen_coring_idx_scale_t;
typedef ik_level_t ik_first_sharpen_min_coring_result_t;
typedef ik_level_method_t ik_final_sharpen_min_coring_result_t;
typedef ik_level_method_t ik_final_sharpen_max_coring_result_t;
typedef ik_level_t ik_first_sharpen_scale_coring_t;
typedef ik_level_method_t ik_final_sharpen_scale_coring_t;

typedef struct {
    uint8 y_tone_offset;
    uint8 y_tone_shift;
    uint8 u_tone_offset;
    uint8 u_tone_shift;
    uint8 u_tone_bits;
    uint8 v_tone_offset;
    uint8 v_tone_shift;
    uint8 v_tone_bits;
    uint8 three_d[IK_NUM_THREE_D_TABLE];
} ik_table_indexing_t;

typedef ik_table_indexing_t ik_final_sharpen_both_three_d_table_t;

typedef struct {
    uint32 up5x5;
    uint32 down5x5;
    uint32 up;
    uint32 down;
} ik_max_change_t;

typedef struct {
    uint32 enable;
    uint32 mode;
    uint32 edge_thresh;
    uint32 wide_edge_detect;
    uint32 max_change_up5x5;
    uint32 max_change_down5x5;
} ik_first_sharpen_both_t;

typedef struct {
    uint32 enable;
    uint32 mode;
    uint32 edge_thresh;
    uint32 wide_edge_detect;
    ik_max_change_t max_change;
    uint32 narrow_nonsmooth_detect_sub[17];
    uint32 narrow_nonsmooth_detect_shift;
    ik_level_method_t method_6_lev;
    uint32 method_6_use_local;
} ik_final_sharpen_both_t;

typedef struct {
    uint32 specify;
    uint32 per_dir_fir_iso_strengths[9];
    uint32 per_dir_fir_dir_strengths[9];
    uint32 per_dir_fir_dir_amounts[9];
    int32  coefs[9][25];
    uint32 strength_iso;
    uint32 strength_dir;
} ik_sharpen_noise_sharpen_fir_t;

typedef ik_sharpen_noise_sharpen_fir_t ik_first_sharpen_fir_t;

typedef struct {
    uint32 specify;
    uint32 per_dir_fir_iso_strengths[9];
    uint32 per_dir_fir_dir_strengths[9];
    uint32 per_dir_fir_dir_amounts[9];
    int32  coefs[9][25];
    uint32 strength_iso;
    uint32 strength_dir;
    uint32 pos_fir_artifact_reduce_enable;
    uint32 pos_fir_artifact_reduce_strength;
    uint32 over_shoot_reduce_allow;
    uint32 over_shoot_reduce_strength;
    uint32 under_shoot_reduce_allow;
    uint32 under_shoot_reduce_strength;
} ik_final_sharpen_fir_t;

typedef struct {
    uint32 enable;
    uint32 max_change_method;
    uint32 max_change_table[32];
    uint32 noise_level_method;
    uint32 noise_level_table[32];
    uint32 size;
    uint32 str_a_method;
    uint32 str_a_table[32];
    uint32 str_b_method;
    uint32 str_b_table[32];
} ik_sharpen_noise_advanced_iso_t;

typedef struct {
    uint32 max_change_up; // 0:255
    uint32 max_change_down; // 0:255
    ik_sharpen_noise_sharpen_fir_t spatial_fir;
    ik_level_t level_str_adjust;
    uint32 level_str_adjust_not_t0_t1_level_based; // 0:10
    uint32 t0; // 0:255 t0 <= t1, t1 - t0 <= 15
    uint32 t1; // 0:255
    uint32 alpha_min; // 0:16
    uint32 alpha_max; // 0:16
} ik_first_sharpen_noise_t;

typedef struct {
    uint32 max_change_up; // 0:255
    uint32 max_change_down; // 0:255
    ik_sharpen_noise_sharpen_fir_t spatial_fir;
    ik_level_method_t level_str_adjust;
    uint32 level_str_adjust_not_t0_t1_level_based; // 0:1
    uint32 t0; // 0:255 t0 <= t1, t1 - t0 <= 15
    uint32 t1; // 0:255
    uint32 alpha_min; // 0:16
    uint32 alpha_max; // 0:16
    uint32 directional_decide_t0;
    uint32 directional_decide_t1;
    ik_sharpen_noise_advanced_iso_t advanced_iso;
} ik_final_sharpen_noise_t;

typedef struct {
    uint32 coring[IK_NUM_CORING_TABLE_INDEX];
    uint32 fractional_bits;
} ik_first_sharpen_coring_t;

typedef struct {
    uint32 coring[IK_NUM_CORING_TABLE_INDEX];
    uint32 fractional_bits;
} ik_final_sharpen_coring_t;

typedef struct {
    uint32 low;
    uint32 low_delta;
    uint32 low_strength;
    uint32 mid_strength;
    uint32 high;
    uint32 high_delta;
    uint32 high_strength;
} ik_first_sharpen_max_coring_result_t;

typedef struct {
    uint32 enable;
    uint32 noise_level_cb;
    uint32 noise_level_cr;          /* 0-255 */
    uint32 original_blend_strength_cb; /* Cb 0-256  */
    uint32 original_blend_strength_cr; /* Cr 0-256  */
    uint32 radius;
} ik_chroma_filter_t;

typedef struct {
    uint32 narrow_enable;
    uint32 wide_enable;
    uint32 wide_safety;
    uint32 wide_thresh;
} ik_grgb_mismatch_t;

typedef struct {
    uint32 ta_frames_combine_num1;
    uint32 ta_frames_combine_num2;
    //cv2
    ik_level_method_t y_3d_64_maxchange;
    uint32 higher_iq_lower_perf;
} ik_mctf_internal_t;

typedef struct {
    uint32 cmd_code;
    uint32 mode;
    uint32 param1;
    uint32 param2;
    uint32 param3;
    uint32 param4;
    uint32 param5;
    uint32 param6;
    uint32 param7;
    uint32 param8;
} ik_idsp_debug_internal_t;

typedef struct {
    uint32 enable; // 0,1
    uint32 y_combine_strength; // 0:256
    uint32 y_strength_3d; // 0:256
    uint32 y_spatial_weighting[64]; // 0:255
    uint32 y_temporal_max_change; // 0:255
    uint32 cb_strength_3d; // 0:256
    uint32 cb_spatial_weighting[64]; // 0:255
    uint32 cb_temporal_max_change; // 0:255
    uint32 cr_strength_3d; // 0:256
    uint32 cr_spatial_weighting[64]; // 0:255
    uint32 cr_temporal_max_change; // 0:255
    ik_video_mctf_curve_t y_curve;
    ik_video_mctf_curve_t cb_curve;
    ik_video_mctf_curve_t cr_curve;
    uint32 y_spat_smth_direct_decide_t0;
    uint32 y_spat_smth_direct_decide_t1;
    uint32 cb_spat_smth_direct_decide_t0;
    uint32 cb_spat_smth_direct_decide_t1;
    uint32 cr_spat_smth_direct_decide_t0;
    uint32 cr_spat_smth_direct_decide_t1;
    uint32 y_temporal_max_change_not_t0_t1_level_based; // 0:1
    ik_level_method_t y_temporal_either_max_change_or_t0_t1_add;
    uint32 cb_temporal_max_change_not_t0_t1_level_based; // 0:1
    ik_level_method_t cb_temporal_either_max_change_or_t0_t1_add;
    uint32 cr_temporal_max_change_not_t0_t1_level_based; // 0:1
    ik_level_method_t cr_temporal_either_max_change_or_t0_t1_add;
    //cv2
    uint32 y_narrow_nonsmooth_detect_sub[17];
    uint32 y_narrow_nonsmooth_detect_shift;
    uint32 use_level_based_ta;
    //spat_blend level
    ik_level_method_t y_spat_blend;
    ik_level_method_t cb_spat_blend;
    ik_level_method_t cr_spat_blend;
    //advanced iso
    uint32 y_advanced_iso_max_change_table[32];
    uint32 y_advanced_iso_max_change_method;
    uint32 y_advanced_iso_size;
    uint32 y_advanced_iso_noise_level;
    uint32 y_advanced_iso_enable;
    uint32 cb_advanced_iso_max_change_table[32];
    uint32 cb_advanced_iso_max_change_method;
    uint32 cb_advanced_iso_size;
    uint32 cb_advanced_iso_noise_level;
    uint32 cb_advanced_iso_enable;
    uint32 cr_advanced_iso_max_change_table[32];
    uint32 cr_advanced_iso_max_change_method;
    uint32 cr_advanced_iso_size;
    uint32 cr_advanced_iso_noise_level;
    uint32 cr_advanced_iso_enable;
    //3d max change level
    ik_level_method_t y_3d_maxchange;
    ik_level_method_t cb_3d_maxchange;
    ik_level_method_t cr_3d_maxchange;
    //overall max change level
    ik_level_method_t y_overall_max_change;
    ik_level_method_t cb_overall_max_change;
    ik_level_method_t cr_overall_max_change;
    //spat filt max smth change level
    ik_level_method_t y_spat_filt_max_smth_change;
    ik_level_method_t cb_spat_filt_max_smth_change;
    ik_level_method_t cr_spat_filt_max_smth_change;
    /*spat_smth_dir*/
    uint32 y_spat_smth_wide_edge_detect; //0:8
    uint32 y_spat_smth_edge_thresh; //0:2047
    ik_level_method_t y_spat_smth_dir;
    uint32 cb_spat_smth_wide_edge_detect;//0:8
    uint32 cb_spat_smth_edge_thresh;//0:2047
    ik_level_method_t cb_spat_smth_dir;
    uint32 cr_spat_smth_wide_edge_detect;//0:8
    uint32 cr_spat_smth_edge_thresh;//0:2047
    ik_level_method_t cr_spat_smth_dir;
    /*spat_smth_iso*/
    ik_level_method_t y_spat_smth_iso;
    ik_level_method_t cb_spat_smth_iso;
    ik_level_method_t cr_spat_smth_iso;
    //level based ta level
    ik_level_method_t y_level_based_ta;
    ik_level_method_t cb_level_based_ta;
    ik_level_method_t cr_level_based_ta;
    //temporal min target
    ik_level_method_t y_temporal_min_target;
    ik_level_method_t cb_temporal_min_target;
    ik_level_method_t cr_temporal_min_target;
    //spatial max temporal level
    uint32 y_spatial_max_change; //0:255
    ik_level_method_t y_spatial_max_temporal;
    uint32 cb_spatial_max_change;//0:255
    ik_level_method_t cb_spatial_max_temporal;
    uint32 cr_spatial_max_change;//0:255
    ik_level_method_t cr_spatial_max_temporal;
    uint32 local_lighting;
    uint32 compression_enable;
    uint32 y_temporal_ghost_prevent;
    uint32 cb_temporal_ghost_prevent;
    uint32 cr_temporal_ghost_prevent;
    uint32 y_temporal_artifact_guard;
    uint32 cb_temporal_artifact_guard;
    uint32 cr_temporal_artifact_guard;
    uint32 compression_bit_rate_luma;
    uint32 compression_bit_rate_chroma;
    uint32 compression_dither_disable;
} ik_video_mctf_t;

typedef struct {
    uint32 max; // 0:15
    uint32 min; // 0:15
    uint32 motion_response; // 0:16383
    uint32 noise_base; // 0:4095
    uint32 still_thresh; // 0:4095
} ik_video_mctf_ta_y_t;

typedef struct {
    uint32 max; // 0:15
    uint32 min; // 0:15
    uint32 motion_response; // 0:16383
    uint32 noise_base; // 0:4095
    uint32 still_thresh; // 0:4095
} ik_video_mctf_ta_cb_t;

typedef struct {
    uint32 max; // 0:15
    uint32 min; // 0:15
    uint32 motion_response; // 0:16383
    uint32 noise_base; // 0:4095
    uint32 still_thresh; // 0:4095
} ik_video_mctf_ta_cr_t;

typedef struct {
    uint32 edge_adjust_b; // 0:2047
    uint32 edge_adjust_l; // 0:2047
    uint32 edge_adjust_r; // 0:2047
    uint32 edge_adjust_t; // 0:2047
    uint32 slow_mo_sensitivity; // 0:11
    uint32 score_noise_robust; // 0:1
    uint32 lev_adjust_low; // 0:255
    uint32 lev_adjust_low_delta; // 0:7
    uint32 lev_adjust_low_strength; // 0:255
    uint32 lev_adjust_mid_strength; // 0:255
    uint32 lev_adjust_high; // 0:255
    uint32 lev_adjust_high_delta; // 0:7
    uint32 lev_adjust_high_strength; // 0:255
    ik_video_mctf_ta_y_t y_mctf_ta;
    ik_video_mctf_ta_cb_t cb_mctf_ta;
    ik_video_mctf_ta_cr_t cr_mctf_ta;
} ik_video_mctf_ta_t;

typedef struct {
    uint32 enable;
    uint32 lev_add_1_method;
    uint32 lev_add_1_table[64];
    uint32 lev_add_2_method;
    uint32 lev_add_2_table[64];
    uint32 lev_mul_1_method;
    uint32 lev_mul_1_table[64];
    uint32 lev_mul_2_method;
    uint32 lev_mul_2_table[64];
    uint32 local_lighting;
    uint32 motion_response;
    uint32 noise_base;
    uint32 slow_mo_sensitivity;
} ik_motion_detect_t;

typedef struct {
    uint8 pos_dep[33][33];
} ik_pos_dep33_t;

typedef struct {
    uint32 local_lighting_radius;
} ik_motion_detect_and_mctf_t;

typedef struct {
    uint32 factor_num;              /* subsamping factor (numerator) */
    uint32 factor_den;              /* subsamping factor (denominator) */
} ik_sensor_subsampling_t;

typedef struct {
    uint32 start_x;     // Unit in pixel. Before downsample.
    uint32 start_y;     // Unit in pixel. Before downsample.
    uint32 width;      // Unit in pixel. After downsample.
    uint32 height;     // Unit in pixel. After downsample.
    ik_sensor_subsampling_t h_sub_sample;
    ik_sensor_subsampling_t v_sub_sample;
} ik_vin_sensor_geometry_t;

typedef struct {
    uint32 version;                                 // 0x20180401UL
    uint32 num_radial_bins_coarse;                  //coarse + fine <= 128
    uint32 size_radial_bins_coarse_log;             //valid range 0~8
    uint32 num_radial_bins_fine;
    uint32 size_radial_bins_fine_log;               //valid range 0~8
    uint32 model_center_x_R;                       // 4 individual bayer components optical center x, relative to calib window.
    uint32 model_center_x_Gr;
    uint32 model_center_x_B;
    uint32 model_center_x_Gb;
    uint32 model_center_y_R;                       // 4 individual bayer components optical center x, relative to calib window.
    uint32 model_center_y_Gr;
    uint32 model_center_y_B;
    uint32 model_center_y_Gb;
    uint32 vig_gain_tbl_R[4][128];            // pointer to [4 bayer chan][4 direction][128 vertices] 0:9b mult 10:13b shift
    uint32 vig_gain_tbl_Gr[4][128];
    uint32 vig_gain_tbl_B[4][128];
    uint32 vig_gain_tbl_Gb[4][128];
} ik_calib_vignette_info_t;

typedef struct {
    uint32 calib_mode_enable;
    ik_vin_sensor_geometry_t  vin_sensor_geo;
    ik_calib_vignette_info_t  calib_vignette_info;
} ik_vignette_t;

typedef struct {
    uint32  version;            /* 0x20180401UL */
    uint8   *sbp_buffer;
} ik_calib_sbp_info_t;

typedef struct {
    ik_vin_sensor_geometry_t  vin_sensor_geo;/* Vin sensor geometry when calibrating. */
    ik_calib_sbp_info_t       calib_sbp_info;
} ik_static_bad_pxl_cor_t;

typedef struct {
    uint32 enable;
} ik_deferred_blc_level_t;

typedef struct {
    int32 black_r;
    int32 black_gr;
    int32 black_gb;
    int32 black_b;
} ik_static_blc_level_t;

typedef struct {
    uint32 cfa_cut_off_freq; /* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    uint32 main_cut_off_freq;
} ik_resampler_strength_t;

typedef struct {
    uint32 x_offset[IKC_MAX_HDR_EXPOSURE_NUM];
    uint32 y_offset[IKC_MAX_HDR_EXPOSURE_NUM];
} ik_hdr_raw_info_t;

typedef struct {
    uint32 r_gain; // 12.12 format
    uint32 g_gain; // 12.12 format
    uint32 b_gain; // 12.12 format
    uint32 shutter_ratio;
} ik_frontend_wb_gain_t;

typedef struct {
    uint32 level_red;
    uint32 level_green_even;
    uint32 level_green_odd;
    uint32 level_blue;
    uint32 pre_level_red;
    uint32 pre_level_green_even;
    uint32 pre_level_green_odd;
    uint32 pre_level_blue;
    uint32 post_level_red;
    uint32 post_level_green_even;
    uint32 post_level_green_odd;
    uint32 post_level_blue;
} ik_frontend_dgain_saturation_level_t;

typedef struct {
    uint32 enable;
    uint32 delta_t0;
    uint32 delta_t1;
    uint32 flicker_threshold;
    uint32 t0_offset;
    uint32 t1_offset;
} ik_hdr_blend_t;

typedef struct {
    uint32 enable;
    uint32 luma_avg_method;

    int32 luma_avg_weight_r;
    int32 luma_avg_weight_gr;
    int32 luma_avg_weight_gb;
    int32 luma_avg_weight_b;

    uint32 fir_enable;
    uint32 fir_coeff[3];
    uint32 coring_index_scale_shift;

    uint32 coring_index_scale_low;
    uint32 coring_index_scale_low_delta;
    uint32 coring_index_scale_low_strength;
    uint32 coring_index_scale_mid_strength;
    uint32 coring_index_scale_high;
    uint32 coring_index_scale_high_delta;
    uint32 coring_index_scale_high_strength;

    uint32 coring_gain_shift;
    uint32 coring_gain_low;
    uint32 coring_gain_low_delta;
    uint32 coring_gain_low_strength;
    uint32 coring_gain_mid_strength;
    uint32 coring_gain_high;
    uint32 coring_gain_high_delta;
    uint32 coring_gain_high_strength;

    uint32 coring_table[IK_CE_CORING_TABLE_SIZE];
    //cv2
    uint32 boost_gain_shift;
    int32 boost_table[IK_CE_BOOST_TABLE_SIZE];
    uint32 boost_table_size_exp;

    uint32 radius;
    uint32 epsilon;
} ik_ce_t;

typedef struct {
    uint32 input_enable;
    uint32 input_table[IK_CE_INPUT_TABLE_SIZE];
} ik_ce_input_table_t;

typedef struct {
    uint32 output_enable;
    uint32 output_shift;
    uint32 output_table[IK_CE_OUTPUT_TABLE_SIZE];
} ik_ce_output_table_t;

typedef struct {
    uint8 mode;
} ik_ce_external_hds_t;

typedef struct {
    uint32 awb_tile_num_col;
    uint32 awb_tile_num_row;
    uint32 awb_tile_col_start;
    uint32 awb_tile_row_start;
    uint32 awb_tile_width;
    uint32 awb_tile_height;
    uint32 awb_tile_active_width;
    uint32 awb_tile_active_height;
    uint32 awb_pix_min_value;
    uint32 awb_pix_max_value;
    uint32 ae_tile_num_col;
    uint32 ae_tile_num_row;
    uint32 ae_tile_col_start;
    uint32 ae_tile_row_start;
    uint32 ae_tile_width;
    uint32 ae_tile_height;
    uint32 ae_pix_min_value;
    uint32 ae_pix_max_value;
    uint32 af_tile_num_col;
    uint32 af_tile_num_row;
    uint32 af_tile_col_start;
    uint32 af_tile_row_start;
    uint32 af_tile_width;
    uint32 af_tile_height;
    uint32 af_tile_active_width;
    uint32 af_tile_active_height;
    uint32 six_minus_cfa_input_shift;
} ik_aaa_stat_info_t;

typedef struct {
    uint32 af_horizontal_filter1_mode;
    uint32 af_horizontal_filter1_stage1_enb;
    uint32 af_horizontal_filter1_stage2_enb;
    uint32 af_horizontal_filter1_stage3_enb;
    int32 af_horizontal_filter1_gain[7];
    uint32 af_horizontal_filter1_shift[4];
    uint32 af_horizontal_filter1_bias_off;
    uint32 af_horizontal_filter1_thresh;
    uint32 af_horizontal_filter2_mode;
    uint32 af_horizontal_filter2_stage1_enb;
    uint32 af_horizontal_filter2_stage2_enb;
    uint32 af_horizontal_filter2_stage3_enb;
    int32 af_horizontal_filter2_gain[7];
    uint32 af_horizontal_filter2_shift[4];
    uint32 af_horizontal_filter2_bias_off;
    uint32 af_horizontal_filter2_thresh;
    uint32 af_tile_fv1_horizontal_shift;
    uint32 af_tile_fv2_horizontal_shift;
} ik_af_stat_ex_info_t;

typedef struct {
    uint32 af_horizontal_filter1_mode;
    uint32 af_horizontal_filter1_stage1_enb;
    uint32 af_horizontal_filter1_stage2_enb;
    uint32 af_horizontal_filter1_stage3_enb;
    int32 af_horizontal_filter1_gain[7];
    uint32 af_horizontal_filter1_shift[4];
    uint32 af_horizontal_filter1_bias_off;
    uint32 af_horizontal_filter1_thresh;
    uint32 af_vertical_filter1_thresh;
    uint32 af_horizontal_filter2_mode;
    uint32 af_horizontal_filter2_stage1_enb;
    uint32 af_horizontal_filter2_stage2_enb;
    uint32 af_horizontal_filter2_stage3_enb;
    int32 af_horizontal_filter2_gain[7];
    uint32 af_horizontal_filter2_shift[4];
    uint32 af_horizontal_filter2_bias_off;
    uint32 af_horizontal_filter2_thresh;
    uint32 af_vertical_filter2_thresh;
    uint32 af_tile_fv1_horizontal_shift;
    uint32 af_tile_fv1_vertical_shift;
    uint32 af_tile_fv1_horizontal_weight;
    uint32 af_tile_fv1_vertical_weight;
    uint32 af_tile_fv2_horizontal_shift;
    uint32 af_tile_fv2_vertical_shift;
    uint32 af_tile_fv2_horizontal_weight;
    uint32 af_tile_fv2_vertical_weight;
} ik_pg_af_stat_ex_info_t;

typedef struct {
    // TODO:
    uint32 reserved;
} ik_aaa_float_info_t;

typedef struct {
    uint32  start_x; /* Unit in pixel*/
    uint32  start_y;
    uint32  width;
    uint32  height;
} ik_window_geomery_t;

typedef struct {
    uint32  width; /* Unit in pixel*/
    uint32  height;
} ik_window_dimension_t;

typedef struct {
    uint32 left_top_x;   /* 16.16 format*/
    uint32 left_top_y;   /* 16.16 format*/
    uint32 right_bot_x;  /* 16.16 format*/
    uint32 right_bot_y;  /* 16.16 format*/
} ik_win_coordintates_t;

typedef struct  {
    ik_window_dimension_t    *input_win_dim;
    ik_window_dimension_t    *main_win_dim;
    ik_window_dimension_t    *out_cfa_win_dim;
} ik_warp_calc_cfa_info_t;

typedef int32  (*ik_win_calc_cb_func_t)(ik_warp_calc_cfa_info_t *info);

typedef struct {
    ik_vin_sensor_geometry_t  vin_sensor;
    ik_window_dimension_t     main_dim;
    ik_window_dimension_t     prev[3];
    ik_window_dimension_t     thumbnail;
    ik_window_dimension_t     screennail;
} ik_window_size_info_t;

typedef struct {
    uint32 enable;
    ik_window_dimension_t cfa;
} ik_cfa_window_size_info_t;

typedef struct {
    int16   x;
    int16   y;
} ik_grid_point_t;

typedef struct {
    uint32                      version;        /* 0x20180401UL */
    uint32                      hor_grid_num;   /* Horizontal grid number. */
    uint32                      ver_grid_num;     /* Vertical grid number. */
    uint32                      tile_width_exp;   /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    uint32                      tile_height_exp;  /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    ik_vin_sensor_geometry_t    vin_sensor_geo;   /* Vin sensor geometry when calibrating. */
    uint32                      enb_2_stage_compensation : 1; /*1:enable, 0 disable*/
    uint32                      reserved : 31;       /* Reserved for extention. */
    ik_grid_point_t             *pwarp;         /* Warp grid vector arrey. */
} ik_warp_info_t;

typedef struct {
    int16 warp_hor[IK_WARP_VIDEO_MAXHORGRID * IK_WARP_VIDEO_MAXVERGRID];
    int16 warp_ver[IK_WARP_VIDEO_MAXHORGRID * IK_WARP_VIDEO_MAXVERGRID];
} ik_warp_video_stitch_t;

typedef struct {
    uint16      main_src_buf_id;
    uint16      main_channel_id;
    uint16      main_yuv_offset_x;
    uint16      main_yuv_offset_y;
} ik_bind_info_t;

typedef struct {
    uint32     version;            /* 0x20180401UL */
    uint32     hor_grid_num;         /* Horizontal grid number. */
    uint32     ver_grid_num;         /* Vertical grid number. */
    uint32     tile_width_exp;       /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    uint32     tile_height_exp;      /* 4:16, 5:32, 6:64, 7:128, 8:256, 9:512 */
    ik_vin_sensor_geometry_t  vin_sensor_geo;   /* Vin sensor geometry when calibrating. */
    uint32  Enb2StageCompensation : 1;
    uint32  Reserved : 31;       /* Reserved for extention. */
    ik_grid_point_t   *p_cawarp_red;   /* Warp grid vector arrey. */
    ik_grid_point_t   *p_cawarp_blue;   /* Warp grid vector arrey. */
} ik_cawarp_info_t;

typedef struct {
    ik_cawarp_info_t calib_cawarp_info;
    ik_window_size_info_t w_size_info;
} ik_cawarp_compensation_t;

typedef struct {
    uint32      enable;
    uint32      zoom_x;      /* 16.16 format*/
    uint32      zoom_y;      /* 16.16 format*/
    int32       shift_x;     /* 16.16 format, unit in pixel in VinSensorGeo domain.*/
    int32       shift_y;     /* 16.16 format, unit in pixel in VinSensorGeo domain.*/
} ik_dzoom_info_t;

typedef struct {
    uint32      enable;
    uint32      left;
    uint32      right;
    uint32      dummy_top;
    uint32      bottom;
} ik_dummy_margin_range_t;

typedef struct {
    uint32       enable;
    ik_window_geomery_t active_geo;
} ik_vin_active_window_t;

typedef struct {
    ik_warp_info_t calib_warp_info;
    ik_window_size_info_t w_size_info;
} ik_warp_compensation_t;

typedef struct {
    uint32 enable;
} ik_warp_multi_slice_info_t;

typedef struct {
    uint32 min_radius;
    uint32 max_radius;
} ik_ce_radius_range_t;

typedef struct {
    int32 internal0;
    int32 internal1;
    int32 internal2;
    int32 internal3;
} ik_bld_internal0_t;

typedef struct {
    uint32 enable;
    uint32 pixel_map_width;
    uint32 pixel_map_height;
    uint32 pixel_map_pitch;
    uint8 *p_map;
} ik_static_bad_pixel_correction_internal_t;

typedef struct {
    /* warp part */
    uint32   enable;
    uint32   grid_array_width;
    uint32   grid_array_height;
    uint32   horz_grid_spacing_exponent;
    uint32   vert_grid_spacing_exponent;
    //uint32   vert_warp_enable;
    uint32   vert_warp_grid_array_width;
    uint32   vert_warp_grid_array_height;
    uint32   vert_warp_horzgrid_spacing_exponent;
    uint32   vert_warp_vert_grid_spacing_exponent;
    int16   *pwarp_horizontal_table;
    int16   *pwarp_vertical_table;
} ik_warp_internal_info_t;

typedef struct {
    uint32  horz_warp_enable;
    uint32  vert_warp_enable;
    uint32   horz_pass_grid_array_width;
    uint32   horz_pass_grid_array_height;
    uint32   horz_pass_horz_grid_spacing_exponent;
    uint32   horz_pass_vert_grid_spacing_exponent;
    uint32   vert_pass_grid_array_width;
    uint32   vert_pass_grid_array_height;
    uint32   vert_pass_horz_grid_spacing_exponent;
    uint32   vert_pass_vert_grid_spacing_exponent;
    int16   warp_horztable_red[1536];
    int16   warp_verttable_red[1536];
    int16   warp_horztable_blue[1536];
    int16   warp_verttable_blue[1536];
} ik_cawarp_internal_info_t;

typedef struct {
    uint32 decompand_enable;
    uint32 decompand_table[IK_NUM_DECOMPAND_TABLE];
} ik_frontend_tone_curve_t;

typedef struct {
    uint32 ae_tile_mask[8];
} ik_histogram_info_t;

typedef struct {
    uint32 mode;
    uint32 sum_shift;
    uint8 pixel_weight[4];
    uint8 tone_curve[32];
} ik_aaa_pseudo_y_info_t;

// Query
typedef struct {
    ik_vin_sensor_geometry_t vin_sensor_geo;
    ik_window_geomery_t vin_active_geo;
    ik_dzoom_info_t dzoom_info;
    ik_dummy_margin_range_t dummy_margin_range;
    ik_window_dimension_t cfa_window;
    ik_window_dimension_t main_window;
    ik_window_dimension_t prev_window[3];
    uint32 dsp_video_mode;
    uint32 fps;
} ik_query_idsp_clock_info_t;

typedef struct {
    uint32 theoretical_clock;
} ik_query_idsp_clock_rst_t;

typedef struct {
    uint32                       work_buf_size;
    uint32                       out_buf_size;
} ik_query_calc_geo_buf_size_t;

typedef struct {
    ik_window_size_info_t     *p_window_size_info;  // M
    ik_vin_active_window_t    *p_active_window;     // O
    ik_dummy_margin_range_t   *p_dmy_range;         // O
    ik_dzoom_info_t           *p_dzoom_info;        // O
    uint32                    flip_mode;
    int32                     warp_enable;
    ik_warp_info_t            *p_calib_warp_info;
    int32                     use_cawarp;
    int32                     cawarp_enable;         // only take effect when use_cawarp = 1
    ik_cawarp_info_t          *p_calib_ca_warp_info;// only take effect when use_cawarp = 1
} ik_calc_geo_settings_inpar_t;

typedef struct {
    ik_calc_geo_settings_inpar_t *p_in;
    void                         *p_work_buf;
    uint32                       work_buf_size;
} ik_calc_geo_settings_t;

typedef struct {
    void                         *p_out;
    uint32                       out_buf_size;
} ik_calc_geo_result_t;

typedef struct {
    ik_window_size_info_t *p_window_size_info;// M
    ik_vin_active_window_t *p_active_window;// O
    ik_dummy_margin_range_t *p_dmy_range;// O
    ik_dzoom_info_t *p_dzoom_info;// O
    uint32 flip_mode;
    uint32 ability;// M
    int32 warp_enable;
    int32 cawarp_enable;
    ik_warp_info_t *p_calib_warp_info;// O
} ik_query_warp_inpar_t;

typedef struct {
    ik_query_warp_inpar_t in;
    void *p_work_buf;
    uint32 work_buf_size;
} ik_query_warp_t;

typedef struct {
    uint32 wait_line;
    uint32 dma_size;
    uint32 vertical_range;
    uint32 warp_vector_v_flip_num;
    uint32 dmy_broken;
    ik_dummy_margin_range_t suggested_dmy_range;
} ik_query_warp_result_t;

typedef struct {
    uint32 enable;
    int32 freq_blend_fir1[10];
    int32 freq_blend_fir2[10];
    uint32 freq_blend_fir2_manual;
    uint32 act_sub;
    uint32 act_mul;
    uint32 sat_sub;
    uint32 sat_mul;
    uint32 sat_and_act_mul;
    uint32 min_rgb;
    uint32 max_rgb;
    uint32 luma_lu[16];
    uint32 luma_diff_table[256];
} ik_fusion_t;


typedef struct {
    uint32 T0T1_add_high_cb;
    uint32 T0T1_add_high_cr;
    uint32 T0T1_add_high_delta_cb;
    uint32 T0T1_add_high_delta_cr;
    uint32 T0T1_add_high_strength_cb;
    uint32 T0T1_add_high_strength_cr;
    uint32 T0T1_add_low_cb;
    uint32 T0T1_add_low_cr;
    uint32 T0T1_add_low_delta_cb;
    uint32 T0T1_add_low_delta_cr;
    uint32 T0T1_add_low_strength_cb;
    uint32 T0T1_add_low_strength_cr;
    uint32 T0T1_add_mid_strength_cb;
    uint32 T0T1_add_mid_strength_cr;
    uint32 T0_cb;
    uint32 T0_cr;
    uint32 T1_cb;
    uint32 T1_cr;
    uint32 alpha_max_cb;
    uint32 alpha_max_cr;
    uint32 alpha_min_cb;
    uint32 alpha_min_cr;
    uint32 max_change_high_cb;
    uint32 max_change_high_cr;
    uint32 max_change_high_delta_cb;
    uint32 max_change_high_delta_cr;
    uint32 max_change_high_strength_cb;
    uint32 max_change_high_strength_cr;
    uint32 max_change_low_cb;
    uint32 max_change_low_cr;
    uint32 max_change_low_delta_cb;
    uint32 max_change_low_delta_cr;
    uint32 max_change_low_strength_cb;
    uint32 max_change_low_strength_cr;
    uint32 max_change_mid_strength_cb;
    uint32 max_change_mid_strength_cr;
} ik_wide_chroma_filter_combine_t;
typedef struct {
    /* 3a statistic */
    ik_aaa_stat_info_t aaa_stat_info;
    ik_af_stat_ex_info_t af_stat_ex_info;
    ik_pg_af_stat_ex_info_t pg_af_stat_ex_info;
    ik_aaa_float_info_t aaa_float_tile_info;

    /* calibration */
    ik_window_size_info_t window_size_info;
    ik_cfa_window_size_info_t cfa_window_size_info;
    ik_warp_info_t calib_warp_info;
    ik_cawarp_info_t calib_ca_warp_info;
    ik_static_bad_pxl_cor_t static_bpc;
    ik_vignette_t vignette_compensation;
    ik_dzoom_info_t dzoom_info;
    uint32  sbp_enable;
    uint32  warp_enable;
    uint32  ca_warp_enable;
    ik_dummy_margin_range_t dmy_range;
    int32  flip_mode;

    /* normal filters */
    ik_static_blc_level_t static_blc;
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
    ik_video_mctf_ta_t video_mctf_ta;
    ik_video_mctf_ta_y_t video_mctf_ta_y;
    ik_video_mctf_ta_cb_t video_mctf_ta_cb;
    ik_video_mctf_ta_cr_t video_mctf_ta_cr;

    /* hdr */
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

    ik_hdr_blend_t hdr_blend;
} ik_user_parameters_t;

uint32 ik_set_vin_sensor_info(uint32 context_id, const ik_vin_sensor_info_t *p_sensor_info);
uint32 ik_get_vin_sensor_info(uint32 context_id, ik_vin_sensor_info_t *p_sensor_info);

uint32 ik_set_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
uint32 ik_get_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);

uint32 ik_set_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
uint32 ik_get_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);

uint32 ik_set_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 ik_get_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);

uint32 ik_set_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 ik_get_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);

uint32 ik_set_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
uint32 ik_get_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);

uint32 ik_set_dynamic_bad_pixel_corr(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 ik_get_dynamic_bad_pixel_corr(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);

uint32 ik_set_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 ik_get_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);

uint32 ik_set_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic);
uint32 ik_get_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic);

uint32 ik_set_color_correction_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg);
uint32 ik_get_color_correction_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg);

uint32 ik_set_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction);
uint32 ik_get_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction);

uint32 ik_set_pre_cc_gain(uint32 context_id, const ik_pre_cc_gain_t *p_pre_cc_gain);
uint32 ik_get_pre_cc_gain(uint32 context_id, ik_pre_cc_gain_t *p_pre_cc_gain);

uint32 ik_set_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
uint32 ik_get_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve);

uint32 ik_set_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 ik_get_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);

uint32 ik_set_rgb_ir(uint32 context_id, const ik_rgb_ir_t *p_rgb_ir);
uint32 ik_get_rgb_ir(uint32 context_id, ik_rgb_ir_t *p_rgb_ir);

uint32 ik_set_chroma_scale(uint32 context_id, const ik_chroma_scale_t *p_chroma_scale);
uint32 ik_get_chroma_scale(uint32 context_id, ik_chroma_scale_t *p_chroma_scale);

uint32 ik_set_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
uint32 ik_get_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);

uint32 ik_set_chroma_median_filter(uint32 context_id, const ik_chroma_median_filter_t *p_chroma_median_filter);
uint32 ik_get_chroma_median_filter(uint32 context_id, ik_chroma_median_filter_t *p_chroma_median_filter);

uint32 ik_set_fst_luma_process_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 ik_get_fst_luma_process_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);

uint32 ik_set_adv_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 ik_get_adv_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);

uint32 ik_set_fst_shp_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 ik_get_fst_shp_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);

uint32 ik_set_fst_shp_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 ik_get_fst_shp_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);

uint32 ik_set_fst_shp_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 ik_get_fst_shp_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);

uint32 ik_set_fst_shp_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 ik_get_fst_shp_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);

uint32 ik_set_fst_shp_coring_idx_scale(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
uint32 ik_get_fst_shp_coring_idx_scale(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);

uint32 ik_set_fst_shp_min_coring_rslt(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
uint32 ik_get_fst_shp_min_coring_rslt(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);

uint32 ik_set_fst_shp_max_coring_rslt(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
uint32 ik_get_fst_shp_max_coring_rslt(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);

uint32 ik_set_fst_shp_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
uint32 ik_get_fst_shp_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);

uint32 ik_set_fnl_shp_both(uint32 context_id, const ik_final_sharpen_both_t *p_final_sharpen_both);
uint32 ik_get_fnl_shp_both(uint32 context_id, ik_final_sharpen_both_t *p_final_sharpen_both);

uint32 ik_set_fnl_shp_fir(uint32 context_id, const ik_final_sharpen_fir_t *p_final_sharpen_fir);
uint32 ik_get_fnl_shp_fir(uint32 context_id, ik_final_sharpen_fir_t *p_final_sharpen_fir);

uint32 ik_set_fnl_shp_noise(uint32 context_id, const ik_final_sharpen_noise_t *p_final_sharpen_noise);
uint32 ik_get_fnl_shp_noise(uint32 context_id, ik_final_sharpen_noise_t *p_final_sharpen_noise);

uint32 ik_set_fnl_shp_coring(uint32 context_id, const ik_final_sharpen_coring_t *p_final_sharpen_coring);
uint32 ik_get_fnl_shp_coring(uint32 context_id, ik_final_sharpen_coring_t *p_final_sharpen_coring);

uint32 ik_set_fnl_shp_coring_idx_scale(uint32 context_id, const ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);
uint32 ik_get_fnl_shp_coring_idx_scale(uint32 context_id, ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale);

uint32 ik_set_fnl_shp_min_coring_rslt(uint32 context_id, const ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);
uint32 ik_get_fnl_shp_min_coring_rslt(uint32 context_id, ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result);

uint32 ik_set_fnl_shp_max_coring_rslt(uint32 context_id, const ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);
uint32 ik_get_fnl_shp_max_coring_rslt(uint32 context_id, ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result);

uint32 ik_set_fnl_shp_scale_coring(uint32 context_id, const ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring);
uint32 ik_get_fnl_shp_scale_coring(uint32 context_id, ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring);

uint32 ik_set_fnl_shp_three_d_table(uint32 context_id, const ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);
uint32 ik_get_fnl_shp_three_d_table(uint32 context_id, ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table);

uint32 ik_set_video_mctf(uint32 context_id, const ik_video_mctf_t *p_video_mctf);
uint32 ik_get_video_mctf(uint32 context_id, ik_video_mctf_t *p_video_mctf);

uint32 ik_set_video_mctf_ta(uint32 context_id, const ik_video_mctf_ta_t *p_video_mctf_ta);
uint32 ik_get_video_mctf_ta(uint32 context_id, ik_video_mctf_ta_t *p_video_mctf_ta);

uint32 ik_set_video_mctf_and_fnl_shp(uint32 context_id, const ik_pos_dep33_t *p_video_mctf_and_final_sharpen);
uint32 ik_get_video_mctf_and_fnl_shp(uint32 context_id, ik_pos_dep33_t *p_video_mctf_and_final_sharpen);

uint32 ik_set_motion_detect(uint32 context_id, const ik_motion_detect_t *p_motion_detect);
uint32 ik_get_motion_detect(uint32 context_id, ik_motion_detect_t *p_motion_detect);

uint32 ik_set_motion_detect_pos_dep(uint32 context_id, const ik_pos_dep33_t *p_motion_detect_pos_dep);
uint32 ik_get_motion_detect_pos_dep(uint32 context_id, ik_pos_dep33_t *p_motion_detect_pos_dep);

uint32 ik_set_motion_detect_and_mctf(uint32 context_id, const ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);
uint32 ik_get_motion_detect_and_mctf(uint32 context_id, ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf);

uint32 ik_set_dzoom_info(uint32 context_id, const ik_dzoom_info_t *p_dzoom_info);
uint32 ik_get_dzoom_info(uint32 context_id, ik_dzoom_info_t *p_dzoom_info);

uint32 ik_set_window_size_info(uint32 context_id, const ik_window_size_info_t *p_window_size_info);
uint32 ik_get_window_size_info(uint32 context_id, ik_window_size_info_t *p_window_size_info);

uint32 ik_set_cfa_window_size_info(uint32 context_id, const ik_cfa_window_size_info_t *p_cfa_window_size_info);
uint32 ik_get_cfa_window_size_info(uint32 context_id, ik_cfa_window_size_info_t *p_cfa_window_size_info);

uint32 ik_set_dummy_margin_range(uint32 context_id, const ik_dummy_margin_range_t *p_dmy_margin_range);
uint32 ik_get_dummy_margin_range(uint32 context_id, ik_dummy_margin_range_t *p_dmy_margin_range);

uint32 ik_set_vin_active_win(uint32 context_id, const ik_vin_active_window_t *p_vin_active_win);
uint32 ik_get_vin_active_win(uint32 context_id, ik_vin_active_window_t *p_vin_active_win);

uint32 ik_set_resampler_strength(uint32 context_id, const ik_resampler_strength_t *p_resampler_strength);
uint32 ik_get_resampler_strength(uint32 context_id, ik_resampler_strength_t *p_resampler_strength);


/* Calibration filters */
uint32 ik_set_warp_enb(uint32 context_id, const uint32 enb);
uint32 ik_get_warp_enb(uint32 context_id, uint32 *p_enb);

uint32 ik_set_warp_info(uint32 context_id, const ik_warp_info_t *p_calib_warp_info);
uint32 ik_get_warp_info(uint32 context_id, ik_warp_info_t *p_calib_warp_info);

uint32 ik_set_cawarp_enb(uint32 context_id, const uint32 enb);
uint32 ik_get_cawarp_enb(uint32 context_id, uint32 *p_enb);

uint32 ik_set_cawarp_info(uint32 context_id, const ik_cawarp_info_t *p_calib_ca_warp_info);
uint32 ik_get_cawarp_info(uint32 context_id, ik_cawarp_info_t *p_calib_ca_warp_info);

uint32 ik_set_static_bad_pxl_corr_enb(uint32 context_id, const uint32 enb);
uint32 ik_get_static_bad_pxl_corr_enb(uint32 context_id, uint32 *p_enb);

uint32 ik_set_static_bad_pxl_corr(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 ik_get_static_bad_pxl_corr(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);

uint32 ik_set_vignette_enb(uint32 context_id, const uint32 enb);
uint32 ik_get_vignette_enb(uint32 context_id, uint32 *p_enb);

uint32 ik_set_vignette(uint32 context_id, const ik_vignette_t *p_vignette);
uint32 ik_get_vignette(uint32 context_id, ik_vignette_t *p_vignette);

uint32 ik_set_static_bad_pxl_corr_itnl(uint32 context_id, const ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);
uint32 ik_get_static_bad_pxl_corr_itnl(uint32 context_id, ik_static_bad_pixel_correction_internal_t *p_static_bpc_internal);

uint32 ik_set_cawarp_internal(uint32 context_id, const ik_cawarp_internal_info_t *p_cawarp_internal);
uint32 ik_get_cawarp_internal(uint32 context_id, ik_cawarp_internal_info_t *p_cawarp_internal);

uint32 ik_set_warp_internal(uint32 context_id, const ik_warp_internal_info_t *p_warp_dzoom_internal);
uint32 ik_get_warp_internal(uint32 context_id, ik_warp_internal_info_t *p_warp_dzoom_internal);

uint32 ik_set_flip_mode(uint32 context_id, const uint32 mode);
uint32 ik_get_flip_mode(uint32 context_id, uint32 *p_mode);

/* HDR filters */
uint32 ik_set_exp0_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 ik_get_exp0_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);

uint32 ik_set_exp1_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp1_frontend_static_blc);
uint32 ik_get_exp1_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp1_frontend_static_blc);

uint32 ik_set_exp2_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp2_frontend_static_blc);
uint32 ik_get_exp2_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp2_frontend_static_blc);

uint32 ik_set_exp0_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
uint32 ik_get_exp0_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);

uint32 ik_set_exp1_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);
uint32 ik_get_exp1_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain);

uint32 ik_set_exp2_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);
uint32 ik_get_exp2_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain);

uint32 ik_set_exp0_frontend_dgain_sat_lvl(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp0_frontend_dgain_sat_lvl);
uint32 ik_get_exp0_frontend_dgain_sat_lvl(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp0_frontend_dgain_sat_lvl);

uint32 ik_set_exp1_frontend_dgain_sat_lvl(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp1_frontend_dgain_sat_lvl);
uint32 ik_get_exp1_frontend_dgain_sat_lvl(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp1_frontend_dgain_sat_lvl);

uint32 ik_set_exp2_frontend_dgain_sat_lvl(uint32 context_id, const ik_frontend_dgain_saturation_level_t *p_exp2_frontend_dgain_sat_lvl);
uint32 ik_get_exp2_frontend_dgain_sat_lvl(uint32 context_id, ik_frontend_dgain_saturation_level_t *p_exp2_frontend_dgain_sat_lvl);

uint32 ik_set_ce(uint32 context_id, const ik_ce_t *p_ce);
uint32 ik_get_ce(uint32 context_id, ik_ce_t *p_ce);

uint32 ik_set_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
uint32 ik_get_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);

uint32 ik_set_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
uint32 ik_get_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);

uint32 ik_set_ce_ext_hds(uint32 context_id, const ik_ce_external_hds_t *p_ce_ext_hds);
uint32 ik_get_ce_ext_hds(uint32 context_id, ik_ce_external_hds_t *p_ce_ext_hds);

uint32 ik_set_hdr_blend(uint32 context_id, const ik_hdr_blend_t *p_hdr_blend);
uint32 ik_get_hdr_blend(uint32 context_id, ik_hdr_blend_t *p_hdr_blend);

uint32 ik_set_aaa_stat_info(uint32 context_id, const ik_aaa_stat_info_t *p_stat_info);
uint32 ik_get_aaa_stat_info(uint32 context_id, ik_aaa_stat_info_t *p_stat_info);

uint32 ik_set_af_stat_ex_info(uint32 context_id, const ik_af_stat_ex_info_t *p_af_stat_ex_info);
uint32 ik_get_af_stat_ex_info(uint32 context_id, ik_af_stat_ex_info_t *p_af_stat_ex_info);

uint32 ik_set_pg_af_stat_ex_info(uint32 context_id, const ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);
uint32 ik_get_pg_af_stat_ex_info(uint32 context_id, ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info);

uint32 ik_set_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 ik_get_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

uint32 ik_set_pg_histogram_info(uint32 context_id, const ik_histogram_info_t *p_hist_info);
uint32 ik_get_pg_histogram_info(uint32 context_id, ik_histogram_info_t *p_hist_info);

uint32 ik_set_pseudo_y_info(uint32 context_id, const ik_aaa_pseudo_y_info_t *p_pseudo_y_info);
uint32 ik_get_pseudo_y_info(uint32 context_id, ik_aaa_pseudo_y_info_t *p_pseudo_y_info);

uint32 ik_set_frontend_tone_curve(uint32 context_id, const ik_frontend_tone_curve_t *p_fe_tc);
uint32 ik_get_frontend_tone_curve(uint32 context_id, ik_frontend_tone_curve_t *p_fe_tc);

uint32 ik_set_hdr_raw_offset(uint32 context_id, const ik_hdr_raw_info_t *p_hdr_raw_info);
uint32 ik_get_hdr_raw_offset(uint32 context_id, ik_hdr_raw_info_t *p_hdr_raw_info);

uint32 ik_set_y2y_use_cc_enable_info(uint32 context_id, const uint32 enable);
uint32 ik_get_y2y_use_cc_enable_info(uint32 context_id, uint32 *enable);

#if SUPPORT_FUSION
uint32 ik_set_fusion(uint32 context_id, const ik_fusion_t *p_fusion);
uint32 ik_get_fusion(uint32 context_id, ik_fusion_t *p_fusion);
uint32 ik_set_mono_static_bad_pxl_corr(uint32 context_id, const ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 ik_get_mono_static_bad_pxl_corr(uint32 context_id, ik_static_bad_pxl_cor_t *p_static_bpc);
uint32 ik_set_mono_vignette(uint32 context_id, const ik_vignette_t *p_vignette);
uint32 ik_get_mono_vignette(uint32 context_id, ik_vignette_t *p_vignette);
uint32 ik_set_mono_before_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_before_ce_wb_gain);
uint32 ik_get_mono_before_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_before_ce_wb_gain);
uint32 ik_set_mono_after_ce_wb_gain(uint32 context_id, const ik_wb_gain_t *p_after_ce_wb_gain);
uint32 ik_get_mono_after_ce_wb_gain(uint32 context_id, ik_wb_gain_t *p_after_ce_wb_gain);
uint32 ik_set_mono_cfa_leakage_filter(uint32 context_id, const ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 ik_get_mono_cfa_leakage_filter(uint32 context_id, ik_cfa_leakage_filter_t *p_cfa_leakage_filter);
uint32 ik_set_mono_anti_aliasing(uint32 context_id, const ik_anti_aliasing_t *p_anti_aliasing);
uint32 ik_get_mono_anti_aliasing(uint32 context_id, ik_anti_aliasing_t *p_anti_aliasing);
uint32 ik_set_mono_dynamic_bad_pixel_corr(uint32 context_id, const ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 ik_get_mono_dynamic_bad_pixel_corr(uint32 context_id, ik_dynamic_bad_pixel_correction_t *p_dynamic_bpc);
uint32 ik_set_mono_grgb_mismatch(uint32 context_id, const ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 ik_get_mono_grgb_mismatch(uint32 context_id, ik_grgb_mismatch_t *p_grgb_mismatch);
uint32 ik_set_mono_cfa_noise_filter(uint32 context_id, const ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 ik_get_mono_cfa_noise_filter(uint32 context_id, ik_cfa_noise_filter_t *p_cfa_noise_filter);
uint32 ik_set_mono_demosaic(uint32 context_id, const ik_demosaic_t *p_demosaic);
uint32 ik_get_mono_demosaic(uint32 context_id, ik_demosaic_t *p_demosaic);
uint32 ik_set_mono_color_correction_reg(uint32 context_id, const ik_color_correction_reg_t *p_color_correction_reg);
uint32 ik_get_mono_color_correction_reg(uint32 context_id, ik_color_correction_reg_t *p_color_correction_reg);
uint32 ik_set_mono_color_correction(uint32 context_id, const ik_color_correction_t *p_color_correction);
uint32 ik_get_mono_color_correction(uint32 context_id, ik_color_correction_t *p_color_correction);
uint32 ik_set_mono_tone_curve(uint32 context_id, const ik_tone_curve_t *p_tone_curve);
uint32 ik_get_mono_tone_curve(uint32 context_id, ik_tone_curve_t *p_tone_curve);
uint32 ik_set_mono_rgb_to_yuv_matrix(uint32 context_id, const ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 ik_get_mono_rgb_to_yuv_matrix(uint32 context_id, ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix);
uint32 ik_set_mono_fst_luma_process_mode(uint32 context_id, const ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 ik_get_mono_fst_luma_process_mode(uint32 context_id, ik_first_luma_process_mode_t *p_first_luma_process_mode);
uint32 ik_set_mono_adv_spatial_filter(uint32 context_id, const ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 ik_get_mono_adv_spatial_filter(uint32 context_id, ik_adv_spatial_filter_t *p_advance_spatial_filter);
uint32 ik_set_mono_fst_shp_both(uint32 context_id, const ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 ik_get_mono_fst_shp_both(uint32 context_id, ik_first_sharpen_both_t *p_first_sharpen_both);
uint32 ik_set_mono_fst_shp_noise(uint32 context_id, const ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 ik_get_mono_fst_shp_noise(uint32 context_id, ik_first_sharpen_noise_t *p_first_sharpen_noise);
uint32 ik_set_mono_fst_shp_fir(uint32 context_id, const ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 ik_get_mono_fst_shp_fir(uint32 context_id, ik_first_sharpen_fir_t *p_first_sharpen_fir);
uint32 ik_set_mono_fst_shp_coring(uint32 context_id, const ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 ik_get_mono_fst_shp_coring(uint32 context_id, ik_first_sharpen_coring_t *p_first_sharpen_coring);
uint32 ik_set_mono_fst_shp_coring_idx_scale(uint32 context_id, const ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
uint32 ik_get_mono_fst_shp_coring_idx_scale(uint32 context_id, ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale);
uint32 ik_set_mono_fst_shp_min_coring_rslt(uint32 context_id, const ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
uint32 ik_get_mono_fst_shp_min_coring_rslt(uint32 context_id, ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result);
uint32 ik_set_mono_fst_shp_max_coring_rslt(uint32 context_id, const ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
uint32 ik_get_mono_fst_shp_max_coring_rslt(uint32 context_id, ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result);
uint32 ik_set_mono_fst_shp_scale_coring(uint32 context_id, const ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
uint32 ik_get_mono_fst_shp_scale_coring(uint32 context_id, ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring);
uint32 ik_set_wide_chroma_filter(uint32 context_id, const ik_chroma_filter_t *p_chroma_filter);
uint32 ik_get_wide_chroma_filter(uint32 context_id, ik_chroma_filter_t *p_chroma_filter);
uint32 ik_set_wide_chroma_filter_combine(uint32 context_id, const ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 ik_get_wide_chroma_filter_combine(uint32 context_id, ik_wide_chroma_filter_combine_t *p_chroma_filter_combine);
uint32 ik_set_mono_exp0_frontend_static_blc(uint32 context_id, const ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 ik_get_mono_exp0_frontend_static_blc(uint32 context_id, ik_static_blc_level_t *p_exp0_frontend_static_blc);
uint32 ik_set_mono_exp0_frontend_wb_gain(uint32 context_id, const ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
uint32 ik_get_mono_exp0_frontend_wb_gain(uint32 context_id, ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain);
uint32 ik_set_mono_ce(uint32 context_id, const ik_ce_t *p_ce);
uint32 ik_get_mono_ce(uint32 context_id, ik_ce_t *p_ce);
uint32 ik_set_mono_ce_input_table(uint32 context_id, const ik_ce_input_table_t *p_ce_input_table);
uint32 ik_get_mono_ce_input_table(uint32 context_id, ik_ce_input_table_t *p_ce_input_table);
uint32 ik_set_mono_ce_out_table(uint32 context_id, const ik_ce_output_table_t *p_ce_out_table);
uint32 ik_get_mono_ce_out_table(uint32 context_id, ik_ce_output_table_t *p_ce_out_table);
#endif
#endif
