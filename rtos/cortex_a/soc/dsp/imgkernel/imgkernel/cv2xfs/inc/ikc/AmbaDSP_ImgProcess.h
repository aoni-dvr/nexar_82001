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

#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include "AmbaDSP_ImgFilter.h"
#include "idspdrv_ik_imgknl_if.h"


/* IKC API */
uint32 ikc_check_version(const uint32 ik_major_num, uint32 ikc_major_num);
uint32 ikc_get_minor_version(uint32 *p_minor_ver);

#if (defined(__unix__) && !defined(__QNX__))

typedef void* (*ikc_system_memcpy_t)(void *p_str1, const void *p_str2, size_t n);
typedef void* (*ikc_system_memset_t)(void *p_str, int32 c, size_t n);
typedef int32 (*ikc_system_memcmp_t)(const void *p_str1, const void *p_str2, size_t n);
typedef int32 (*ikc_system_print_t)(const char *p_format, ...);

typedef struct {
    ikc_system_memcpy_t system_memcpy;
    ikc_system_memset_t system_memset;
    ikc_system_memcmp_t system_memcmp;
    ikc_system_print_t system_print;
} ikc_system_api_t;

uint32 ikc_import_system_callback_func(const ikc_system_api_t *p_sys_api);
#endif

// common stuff, use in IKC init

typedef struct {
    uint32 flip_mode;
    uint32 is_yuv_mode;
    ik_vin_sensor_info_t *p_sensor_info;
} ikc_in_input_mode_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;

    // vin_stat_exp0
    uint32 cr_6_size;
    void *p_cr_6;

    // vin_stat_exp1
    uint32 cr_7_size;
    void *p_cr_7;

    // vin_stat_exp2
    uint32 cr_8_size;
    void *p_cr_8;

    // dgain_vig
    uint32 cr_9_size;
    void *p_cr_9;

    // dbp
    uint32 cr_11_size;
    void *p_cr_11;

    // sbp_grgb
    uint32 cr_12_size;
    void *p_cr_12;

    // ce
    uint32 cr_13_size;
    void *p_cr_13;

    // cfa_prescale
    uint32 cr_16_size;
    void *p_cr_16;

    // aaa_cfa
    uint32 cr_21_size;
    void *p_cr_21;

    // dgain
    uint32 cr_22_size;
    void *p_cr_22;

    // cfa_noise
    uint32 cr_23_size;
    void *p_cr_23;

    // demosaic
    uint32 cr_24_size;
    void *p_cr_24;

    // cc
    uint32 cr_26_size;
    void *p_cr_26;

    // rgb_2_yuv
    uint32 cr_29_size;
    void *p_cr_29;

    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;

    // chroma_median
    uint32 cr_31_size;
    void *p_cr_31;

    // chroma_down_smp
    uint32 cr_32_size;
    void *p_cr_32;

    // shpb
    uint32 cr_117_size;
    void *p_cr_117;

    idsp_ik_flow_info_t *p_flow;
    idsp_ik_calib_data_t *p_calib;
} ikc_out_input_mode_t;

uint32 ikc_input_mode(const ikc_in_input_mode_t *p_in_input, ikc_out_input_mode_t *p_out_input);


typedef struct {
    uint32 flip_mode;
    uint32 is_yuv_mode;
    uint32 sensor_pattern;      /* Bayer patterns RG, BG, GR, GB */
} ikc_in_input_mode_sub_t;

typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;

    // dgain_vig_b
    uint32 cr_47_size;
    void *p_cr_47;

    // dbp_b
    uint32 cr_49_size;
    void *p_cr_49;

    // sbp_grgb_b
    uint32 cr_50_size;
    void *p_cr_50;

    // ce_fe
    uint32 cr_51_size;
    void *p_cr_51;
} ikc_out_input_mode_sub_t;

uint32 ikc_input_mode_sub(const ikc_in_input_mode_sub_t *p_in_input_sub, ikc_out_input_mode_sub_t *p_out_input_sub);


typedef struct {
    uint32  is_yuv_mode;
    uint32  use_cc_for_yuv2yuv;
} ikc_in_cc_en_t;

typedef struct {
    // cc
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_cc_en_t;

uint32 ikc_color_correction_enable(const ikc_in_cc_en_t *p_in_cc_en, ikc_out_cc_en_t *p_out_cc_en); // TBD, move to input mode??

uint32 ikc_register_valid_mem_range(const uintptr mem_start, const uintptr mem_end);


// sec2 filters

// calibration

typedef struct {
    uint32 sbp_enable;
    uint32 dbp_enable;
    ik_static_bad_pxl_cor_t *p_static_bpc;
    ik_vin_sensor_geometry_t *p_vin_sensor;
} ikc_in_static_bad_pixel_t;

typedef struct {
    // sbp_grgb
    uint32 cr_12_size;
    void *p_cr_12;

    idsp_ik_flow_info_t *p_flow;
} ikc_out_static_bad_pixel_t;

uint32 ikc_static_bad_pixel(const ikc_in_static_bad_pixel_t *p_in_sbp, ikc_out_static_bad_pixel_t *p_out_sbp);


typedef struct {
    uint32 dbp_enable;
    ik_static_bad_pixel_correction_internal_t *p_sbp_internal;
} ikc_in_static_bad_pixel_internal_t;

typedef struct {
    // sbp_grgb
    uint32 cr_12_size;
    void *p_cr_12;

    idsp_ik_flow_info_t *p_flow;
} ikc_out_static_bad_pixel_internal_t;

uint32 ikc_static_bad_pixel_internal(const ikc_in_static_bad_pixel_internal_t *p_in_sbp, ikc_out_static_bad_pixel_internal_t *p_out_sbp);


typedef struct {
    uint32 vig_enable;
    uint32 flip_mode;
    ik_vignette_t *p_vignette_compensation;
    ik_vin_sensor_geometry_t *p_vin_sensor;
    ik_window_geomery_t *p_dmy_win_geo;
} ikc_in_vignette_t;

typedef struct {
    // dgain_vig
    uint32 cr_9_size;
    void *p_cr_9;

    // vig_mult
    uint32 cr_10_size;
    void *p_cr_10;

    // dbp
    uint32 cr_11_size;
    void *p_cr_11;

    idsp_ik_flow_info_t *p_flow;
} ikc_out_vignette_t;

uint32 ikc_vignette(const ikc_in_vignette_t *p_in_vig, ikc_out_vignette_t *p_out_vig);


typedef struct {
    // dgain_vig_b
    uint32 cr_47_size;
    void *p_cr_47;

    // vig_mult_b
    uint32 cr_48_size;
    void *p_cr_48;

    // dbp_b
    uint32 cr_49_size;
    void *p_cr_49;
} ikc_out_vignette_sub_t;

uint32 ikc_vignette_sub(const ikc_in_vignette_t *p_in_vig, ikc_out_vignette_sub_t *p_out_vig_sub);


typedef struct {
    uint32 is_group_cmd;
    uint32 cawarp_enable;
    uint32 flip_mode;
    ik_stitch_info_t *p_stitching_info;
    ik_window_size_info_t *p_window_size_info;
    amba_ik_calc_win_result_t *p_result_win;
    ik_cawarp_info_t *p_calib_ca_warp_info;
} ikc_in_cawarp_t;

typedef struct {
    // cfa_prescale
    uint32 cr_16_size;
    void *p_cr_16;

    // ca_tbl
    uint32 cr_17_size;
    void *p_cr_17;
    uint32 cr_18_size;
    void *p_cr_18;
    uint32 cr_19_size;
    void *p_cr_19;
    uint32 cr_20_size;
    void *p_cr_20;

    idsp_ik_calib_data_t *p_calib; // IKC should fill all the cawarp parameters except table address.
    void *p_cawarp_hor_table_addr_red; // IKC should copy the table contents to here
    uint32 h_red_size;
    void *p_cawarp_vertical_table_addr_red; // IKC should copy the table contents to here
    uint32 v_red_size;
    void *p_cawarp_hor_table_addr_blue; // IKC should copy the table contents to here
    uint32 h_blue_size;
    void *p_cawarp_vertical_table_addr_blue; // IKC should copy the table contents to here
    uint32 v_blue_size;
} ikc_out_cawarp_t;

uint32 ikc_cawarp(const ikc_in_cawarp_t *p_in_ca, ikc_out_cawarp_t *p_out_ca);


typedef struct {
    ik_stitch_info_t *p_stitching_info;
    ik_cawarp_internal_info_t *p_ca_warp_internal;
} ikc_in_cawarp_internal_t;

uint32 ikc_cawarp_internal(const ikc_in_cawarp_internal_t *p_in_ca_internal, ikc_out_cawarp_t *p_out_ca); // TODO, need to set ca enable regs by not using idspdrv_enable_resampler(sec2_header_addr, CFA_PRESCALE_CAWARP, TRUE, TRUE);


typedef struct {
    uint32 is_group_cmd;
    uint32 warp_enable;
    uint32 flip_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_win_coordintates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_info_t *p_calib_warp_info; // TBD, remap in comsvc, take remapped table as calib warp table input
    uint32 dram_efficiency;
    uint32 force_vscale_sec_n;// 2 : all v-scale done by sec2, 3 : all v-scale done by sec3, others : up by sec3, down by sec2.
} ikc_in_warp_t;

typedef struct {
    // luma_resmp
    uint32 cr_33_size;
    void *p_cr_33;

    // hwarp_map
    uint32 cr_34_size;
    void *p_cr_34;

    // chroma_resmp
    uint32 cr_35_size;
    void *p_cr_35;

    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;

    // vwarp_map
    uint32 cr_43_size;
    void *p_cr_43;

    // vwarp_me0
    uint32 cr_44_size;
    void *p_cr_44;

    idsp_ik_flow_info_t *p_flow_info; /* data flow & SBP info */
    idsp_ik_window_info_t *p_window;  /* window geometry */
    idsp_ik_phase_info_t *p_phase;    /* phase inc & init phase */
    idsp_ik_calib_data_t *p_calib; // IKC should fill all the warp parameters except table address.
    idsp_ik_stitch_data_t *p_stitch; /*stitching info*/
    void *p_warp_horizontal_table_address; // IKC should copy the table contents to here
    uint32 h_size;
    void *p_warp_vertical_table_address; // IKC should copy the table contents to here
    uint32 v_size;
} ikc_out_warp_t;

uint32 ikc_warp(const ikc_in_warp_t *p_in_warp, ikc_out_warp_t *p_out_warp);

typedef struct {
    uint32 is_group_cmd;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 dram_efficiency;
    uint32 luma_wait_lines;
    uint32 luma_dma_size;
#if defined(EARLYTEST_ENV)
    uint32 chroma_dma_size;
#else
    uint32 reserved;
#endif
} ikc_in_warp_radius_t;

typedef struct {
    // chroma_resmp
    uint32 cr_35_size;
    void *p_cr_35;

    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;

    idsp_ik_window_info_t *p_window;  /* window geometry */
    idsp_ik_calib_data_t *p_calib;
} ikc_out_warp_radius_t;

uint32 ikc_warp_radius_internal(const ikc_in_warp_radius_t *p_in_warp_radius, ikc_out_warp_radius_t *p_out_warp_radius);
uint32 ikc_warp_radius(const ikc_in_warp_radius_t *p_in_warp_radius, ikc_out_warp_radius_t *p_out_warp_radius);

typedef struct {
    uint32 flip_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_win_coordintates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_dummy_margin_range_t *p_dmy_range;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_internal_info_t *p_warp_internal;
    uint32 dram_efficiency;
    uint32 force_vscale_sec_n;// 2 : all v-scale done by sec2, 3 : all v-scale done by sec3, others : up by sec3, down by sec2.
} ikc_in_warp_internal_t;

uint32 ikc_warp_internal(const ikc_in_warp_internal_t *p_in_warp_internal, ikc_out_warp_t *p_out_warp);


// hdr_ce
typedef struct {
    uint32 exp_num;
    ik_hdr_raw_info_t *p_hdr_raw_info;
    ik_hdr_blend_t *p_hdr_blend;
} ikc_in_hdr_blend_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;

    idsp_ik_flow_info_t *p_flow;
} ikc_out_hdr_blend_t;

uint32 ikc_hdr_blend(const ikc_in_hdr_blend_t *p_in_hdr_blend, ikc_out_hdr_blend_t *p_out_hdr_blend);


typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;
} ikc_out_hdr_blend_sub_t;

uint32 ikc_hdr_blend_sub(const ikc_in_hdr_blend_t *p_in_hdr_blend, ikc_out_hdr_blend_sub_t *p_out_hdr_blend_sub);


typedef struct {
    ik_frontend_tone_curve_t *p_fe_tone_curve;
} ikc_in_hdr_tone_curve_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;

    // hdr_decomp
    uint32 cr_5_size;
    void *p_cr_5;
} ikc_out_hdr_tone_curve_t;

uint32 ikc_front_end_tone_curve(const ikc_in_hdr_tone_curve_t *p_in_hdr_tone_curve, ikc_out_hdr_tone_curve_t *p_out_hdr_tone_curve);

typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;

    // hdr_decomp_b
    uint32 cr_46_size;
    void *p_cr_46;
} ikc_out_hdr_tone_curve_sub_t;

uint32 ikc_front_end_tone_curve_sub(const ikc_in_hdr_tone_curve_t *p_in_hdr_tone_curve, ikc_out_hdr_tone_curve_sub_t *p_out_hdr_tone_curve_sub);


typedef struct {
    uint32 exp_num;
    uint32 sensor_pattern;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_ce_t *p_ce;
} ikc_in_ce_t;

typedef struct {
    // ce
    uint32 cr_13_size;
    void *p_cr_13;

    idsp_ik_flow_info_t *p_flow;
} ikc_out_ce_t;

uint32 ikc_contrast_enhancement(const ikc_in_ce_t *p_in_ce, ikc_out_ce_t *p_out_ce);


typedef struct {
    uint32 ce_enable;
    ik_ce_input_table_t *p_ce_input_table;
} ikc_in_ce_input_t;

typedef struct {
    // ce
    uint32 cr_13_size;
    void *p_cr_13;

    // ce_linear
    uint32 cr_14_size;
    void *p_cr_14;
} ikc_out_ce_input_t;

uint32 ikc_contrast_enhancement_input(const ikc_in_ce_input_t *p_in_ce_input, ikc_out_ce_input_t *p_out_ce_input);


typedef struct {
    uint32 ce_enable;
    ik_ce_output_table_t *p_ce_out_table;
} ikc_in_ce_output_t;

uint32 ikc_contrast_enhancement_output(const ikc_in_ce_output_t *p_in_ce_output, ikc_out_ce_t *p_out_ce);


typedef struct {
    // dbp_b
    uint32 cr_49_size;
    void *p_cr_49;

    // sbp_grgb_b
    uint32 cr_50_size;
    void *p_cr_50;

    // ce_fe
    uint32 cr_51_size;
    void *p_cr_51;
} ikc_out_ce_sub_t;

uint32 ikc_contrast_enhancement_sub(const ikc_in_ce_t *p_in_ce, ikc_out_ce_sub_t *p_out_ce_sub);


typedef struct {
    uint32 ce_enable;
    uint32 radius;
    uint32 epsilon;
    ik_ce_input_table_t *p_ce_input_table;
} ikc_in_ce_input_sub_t;

typedef struct {
    // ce_fe
    uint32 cr_51_size;
    void *p_cr_51;

    // ce_fe_linear
    uint32 cr_52_size;
    void *p_cr_52;
} ikc_out_ce_input_sub_t;

uint32 ikc_contrast_enhancement_input_sub(const ikc_in_ce_input_sub_t *p_in_ce_input_sub, ikc_out_ce_input_sub_t *p_out_ce_input_sub);


typedef struct {
    ik_wb_gain_t *p_before_ce_wb_gain;
} ikc_in_before_ce_gain_t;

typedef struct {
    // ce
    uint32 cr_13_size;
    void *p_cr_13;
} ikc_out_before_ce_gain_t;

uint32 ikc_before_ce_gain(const ikc_in_before_ce_gain_t *p_in_ce_gain, ikc_out_before_ce_gain_t *p_out_ce_gain); // remember to set idspdrv_set_ce_dgain_black_level_offset() to all 0


typedef struct {
    // ce_fe
    uint32 cr_51_size;
    void *p_cr_51;
} ikc_out_before_ce_gain_sub_t;

uint32 ikc_before_ce_gain_sub(const ikc_in_before_ce_gain_t *p_in_ce_gain, ikc_out_before_ce_gain_sub_t *p_out_ce_gain_sub);


typedef struct {
    uint32 exp_num;
    uint32 compress_mode;
    ik_window_geomery_t *p_dmy_win_geo;
} ikc_in_decompress_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;
} ikc_out_decompress_t;

uint32 ikc_decompression(const ikc_in_decompress_t *p_in_decompress, ikc_out_decompress_t *p_out_decompress);

typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;
} ikc_out_decompress_sub_t;

uint32 ikc_decompression_sub(const ikc_in_decompress_t *p_in_decompress, ikc_out_decompress_sub_t *p_out_decompress_sub);

typedef struct {
    uint32 exp_num;
    uint32 rgb_ir_mode;
    int32 compression_offset;
    ik_static_blc_level_t *p_exp0_frontend_static_blc;
    ik_static_blc_level_t *p_exp1_frontend_static_blc;
    ik_static_blc_level_t *p_exp2_frontend_static_blc;
} ikc_in_hdr_blc_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;

    // vin_stat_exp0
    uint32 cr_6_size;
    void *p_cr_6;

    // vin_stat_exp1
    uint32 cr_7_size;
    void *p_cr_7;

    // vin_stat_exp2
    uint32 cr_8_size;
    void *p_cr_8;
} ikc_out_hdr_blc_t;

uint32 ikc_hdr_black_level(const ikc_in_hdr_blc_t *p_in_hdr_blc, ikc_out_hdr_blc_t *p_out_hdr_blc);

typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;
} ikc_out_hdr_blc_sub_t;

uint32 ikc_hdr_black_level_sub(const ikc_in_hdr_blc_t *p_in_hdr_blc, ikc_out_hdr_blc_sub_t *p_out_hdr_blc_sub);


typedef struct {
    uint32 exp_num;
    uint32 rgb_ir_mode;
    ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain;
    ik_frontend_wb_gain_t *p_exp1_frontend_wb_gain;
    ik_frontend_wb_gain_t *p_exp2_frontend_wb_gain;
    ik_static_blc_level_t *p_exp0_frontend_static_blc;
    ik_static_blc_level_t *p_exp1_frontend_static_blc;
    ik_static_blc_level_t *p_exp2_frontend_static_blc;
} ikc_in_hdr_dgain_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;
} ikc_out_hdr_dgain_t;

uint32 ikc_hdr_dgain(const ikc_in_hdr_dgain_t *p_in_hdr_dgain, ikc_out_hdr_dgain_t *p_out_hdr_dgain);

typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;
} ikc_out_hdr_dgain_sub_t;

uint32 ikc_hdr_dgain_sub(const ikc_in_hdr_dgain_t *p_in_hdr_dgain, ikc_out_hdr_dgain_sub_t *p_out_hdr_dgain_sub);


// cfa
typedef struct {
    uint32 sbp_enable;
    ik_dynamic_bad_pixel_correction_t *p_dbp;
    uint32 rgb_ir_mode;
} ikc_in_dynamic_bad_pixel_t;

typedef struct {
    uint32 cr_11_size;
    void *p_cr_11;
    uint32 cr_12_size;
    void *p_cr_12;
} ikc_out_dynamic_bad_pixel_t;

uint32 ikc_dynamic_bad_pixel(const ikc_in_dynamic_bad_pixel_t *p_in_dbp, ikc_out_dynamic_bad_pixel_t *p_out_dbp);


typedef struct {
    ik_grgb_mismatch_t *p_grgb_mismatch;
} ikc_in_grgb_mismatch_t;

typedef struct {
    // cfa_noise
    uint32 cr_23_size;
    void *p_cr_23;
} ikc_out_grgb_mismatch_t;

uint32 ikc_grgb_mismatch(const ikc_in_grgb_mismatch_t *p_in_grgb, ikc_out_grgb_mismatch_t *p_out_grgb);


// jack, put together to solve dependency.
typedef struct {
    ik_cfa_leakage_filter_t *p_cfa_leakage_filter;
    ik_anti_aliasing_t *p_anti_aliasing;
} ikc_in_cfa_leak_anti_alias_t;

typedef struct {
    // sbp_grgb
    uint32 cr_12_size;
    void *p_cr_12;
} ikc_out_cfa_leak_anti_alias_t;

uint32 ikc_cfa_leakage_anti_aliasing(const ikc_in_cfa_leak_anti_alias_t *p_in_cfa_leak_anti_alias, ikc_out_cfa_leak_anti_alias_t *p_out_cfa_leak_anti_alias);


typedef struct {
    ik_demosaic_t *p_demosaic;
} ikc_in_demosaic_t;

typedef struct {
    // sbp_grgb
    uint32 cr_12_size;
    void *p_cr_12;

    // cfa_noise
    uint32 cr_23_size;
    void *p_cr_23;

    // demosaic
    uint32 cr_24_size;
    void *p_cr_24;
} ikc_out_demosaic_t;

uint32 ikc_demosaic_filter(const ikc_in_demosaic_t *p_in_demosaic, ikc_out_demosaic_t *p_out_demosaic);


typedef struct {
    ik_cfa_noise_filter_t *p_cfa_noise_filter;
} ikc_in_cfa_noise_t;

typedef struct {
    // cfa_noise
    uint32 cr_23_size;
    void *p_cr_23;
} ikc_out_cfa_noise_t;

uint32 ikc_cfa_noise(const ikc_in_cfa_noise_t *p_in_cfa_noise, ikc_out_cfa_noise_t *p_out_cfa_noise);


typedef struct {
    ik_wb_gain_t *p_after_ce_wb_gain;
} ikc_in_after_ce_gain_t;

typedef struct {
    // dgain
    uint32 cr_22_size;
    void *p_cr_22;
} ikc_out_after_ce_gain_t;

uint32 ikc_after_ce_gain(const ikc_in_after_ce_gain_t *p_in_after_ce_gain, ikc_out_after_ce_gain_t *p_out_after_ce_gain);


typedef struct {
    uint32 flip_mode;
    ik_rgb_ir_t *p_rgb_ir;
    ik_wb_gain_t *p_before_ce_wb_gain;
    ik_wb_gain_t *p_after_ce_wb_gain;
    ik_frontend_wb_gain_t *p_exp0_frontend_wb_gain;
} ikc_in_rgb_ir_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;

    // vin_stat_exp0
    uint32 cr_6_size;
    void *p_cr_6;

    // vin_stat_exp1
    uint32 cr_7_size;
    void *p_cr_7;

    // vin_stat_exp2
    uint32 cr_8_size;
    void *p_cr_8;

    // dgain_vig
    uint32 cr_9_size;
    void *p_cr_9;

    // dbp
    uint32 cr_11_size;
    void *p_cr_11;

    // sbp_grgb
    uint32 cr_12_size;
    void *p_cr_12;

    // aaa_cfa
    uint32 cr_21_size;
    void *p_cr_21;

    idsp_ik_flow_info_t *p_flow;
} ikc_out_rgb_ir_t;

uint32 ikc_rgb_ir(const ikc_in_rgb_ir_t *p_in_rgb_ir, ikc_out_rgb_ir_t *p_out_rgb_ir);


typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;

    // dgain_vig_b
    uint32 cr_47_size;
    void *p_cr_47;

    // dbp_b
    uint32 cr_49_size;
    void *p_cr_49;

    // sbp_grgb_b
    uint32 cr_50_size;
    void *p_cr_50;
} ikc_out_rgb_ir_sub_t;

uint32 ikc_rgb_ir_sub(const ikc_in_rgb_ir_t *p_in_rgb_ir, ikc_out_rgb_ir_sub_t *p_out_rgb_ir_sub);


typedef struct {
    uint32 sensor_readout_mode;
    uint32 cfa_cut_off_freq; /* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    uint32 cawarp_enable;
} ikc_in_cfa_prescale_t;

typedef struct {
    // cfa_prescale
    uint32 cr_16_size;
    void *p_cr_16;
} ikc_out_cfa_prescale_t;

uint32 ikc_cfa_prescale(const ikc_in_cfa_prescale_t *p_in_cfa_scale, ikc_out_cfa_prescale_t *p_out_cfa_scale);


// rgb
typedef struct {
    // cc
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_cc_reg_t;

uint32 ikc_color_correction_reg_y2y(ikc_out_cc_reg_t *p_out_cc_reg);

typedef struct {
    ik_color_correction_t *p_color_correction;
    uint32 is_ir_only;
} ikc_in_cc_t;

typedef struct {
    // cc
    uint32 cr_26_size;
    void *p_cr_26;

    // cc_3d
    uint32 cr_27_size;
    void *p_cr_27;
} ikc_out_cc_t;

uint32 ikc_color_correction(const ikc_in_cc_t *p_in_cc, ikc_out_cc_t *p_out_cc);


typedef struct {
    ik_tone_curve_t *p_tone_curve;
} ikc_in_cc_out_t;

typedef struct {
    // cc
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_cc_out_t;

uint32 ikc_color_correction_out(const ikc_in_cc_out_t *p_in_cc_out, ikc_out_cc_out_t *p_out_cc_out);

typedef struct {
    ik_pre_cc_gain_t *p_pre_cc_gain;
} ikc_in_pre_cc_gain_t;

typedef struct {
    // cc
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_pre_cc_gain_t;

uint32 ikc_pre_cc_gain(const ikc_in_pre_cc_gain_t *p_in_pre_cc_gain, ikc_out_pre_cc_gain_t *p_out_pre_cc_gain);

typedef struct {
    uint32 is_group_cmd;
    uint32 is_yuv_mode;
    ik_vin_sensor_info_t *p_sensor_info;
    ik_rgb_ir_t *p_rgb_ir;
    ik_aaa_stat_info_t *p_aaa_stat_info;
    ik_aaa_pg_af_stat_info_t  *p_aaa_pg_af_stat_info;
    ik_af_stat_ex_info_t *p_af_stat_ex_info;
    ik_pg_af_stat_ex_info_t *p_pg_af_stat_ex_info;
    ik_histogram_info_t *p_hist_info;
    ik_histogram_info_t *p_hist_info_pg;
    ik_window_geomery_t *p_logical_dmy_win_geo;//coordinate relative to dmy_win_geo.
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_stitch_info_t *p_stitching_info;
} ikc_in_aaa_t;

typedef struct {
    uint32 cr_9_size;
    void *p_cr_9;

    uint32 cr_12_size;
    void *p_cr_12;

    // aaa_cfa
    uint32 cr_21_size;
    void *p_cr_21;

    // aaa
    uint32 cr_28_size;
    void *p_cr_28;

    void *p_stitching_aaa;
} ikc_out_aaa_t;

uint32 ikc_aaa(const ikc_in_aaa_t *p_in_aaa, ikc_out_aaa_t *p_out_aaa);


typedef struct {
    ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix;
    uint32 is_ir_only;
} ikc_in_rgb2yuv_t;

typedef struct {
    // rgb_2_yuv
    uint32 cr_29_size;
    void *p_cr_29;
} ikc_out_rgb2yuv_t;

uint32 ikc_rgb2yuv(const ikc_in_rgb2yuv_t *p_in_rgb2yuv, ikc_out_rgb2yuv_t *p_out_rgb2yuv);

typedef struct {
    ik_rgb_to_12y_t *p_rgb_to_12y;
} ikc_in_rgb12y_t;

typedef struct {
    // rgb_2_12y
    uint32 cr_25_size;
    void *p_cr_25;
} ikc_out_rgb12y_t;

uint32 ikc_rgb12y(const ikc_in_rgb12y_t *p_in_rgb12y, ikc_out_rgb12y_t *p_out_rgb12y);

// yuv
typedef struct {
    uint32  is_yuv_mode;
    ik_chroma_scale_t *p_chroma_scale;
} ikc_in_chroma_scale_t;

typedef struct {
    // rgb_2_yuv
    uint32 cr_29_size;
    void *p_cr_29;
} ikc_out_chroma_scale_t;

uint32 ikc_chroma_scale(const ikc_in_chroma_scale_t *p_in_chroma_scale, ikc_out_chroma_scale_t *p_out_chroma_scale);


typedef struct {
    uint32  is_yuv_mode;
    ik_chroma_median_filter_t *p_chroma_median_filter;
} ikc_in_chroma_median_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;

    // chroma_median
    uint32 cr_31_size;
    void *p_cr_31;
} ikc_out_chroma_median_t;

uint32 ikc_chroma_median(const ikc_in_chroma_median_t *p_in_chroma_median, ikc_out_chroma_median_t *p_out_chroma_median);


typedef struct {
    uint32 is_group_cmd;
    ik_chroma_filter_t *p_chroma_filter;
    ik_wide_chroma_filter_t *p_wide_chroma_filter;
    ik_wide_chroma_filter_combine_t *p_wide_chroma_combine;
} ikc_in_chroma_noise_t;

typedef struct {
    // chroma_noise
    uint32 cr_36_size;
    void *p_cr_36;

    idsp_ik_window_info_t *p_window;  /* window geometry */
} ikc_out_chroma_noise_t;

uint32 ikc_chroma_noise(const ikc_in_chroma_noise_t *p_in_chroma_noise, ikc_out_chroma_noise_t *p_out_chroma_noise);


typedef struct {
    ik_adv_spatial_filter_t *p_advance_spatial_filter;
    uint32 first_luma_processing_mode_updated;// for set to default
} ikc_in_asf_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
} ikc_out_asf_t;

uint32 ikc_asf(const ikc_in_asf_t *p_in_asf, ikc_out_asf_t *p_out_asf);

typedef struct {
    uint32 sharpen_mode;
    ik_first_sharpen_noise_t *p_first_sharpen_noise;
    ik_first_sharpen_fir_t *p_first_sharpen_fir;
    void *p_working_buffer;
    uint32 working_buffer_size;//2700 bytes.
} ikc_in_sharpen_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
} ikc_out_sharpen_t;

uint32 ikc_sharpen(const ikc_in_sharpen_t *p_in_sharpen, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    ik_first_sharpen_both_t *p_first_sharpen_both;
} ikc_in_sharpen_both_t;

uint32 ikc_sharpen_both(const ikc_in_sharpen_both_t *p_in_sharpen_both, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    ik_first_sharpen_coring_t *p_first_sharpen_coring;
} ikc_in_sharpen_coring_t;

uint32 ikc_sharpen_coring(const ikc_in_sharpen_coring_t *p_in_sharpen_coring, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    ik_first_sharpen_coring_idx_scale_t *p_first_sharpen_coring_idx_scale;
} ikc_in_sharpen_coring_idx_scale_t;

uint32 ikc_sharpen_coring_idx_scale(const ikc_in_sharpen_coring_idx_scale_t *p_in_sharpen_coring_idx_scale, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    ik_first_sharpen_min_coring_result_t *p_first_sharpen_min_coring_result;
} ikc_in_sharpen_min_coring_t;

uint32 ikc_sharpen_min_coring(const ikc_in_sharpen_min_coring_t *p_in_sharpen_min_coring, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    ik_first_sharpen_max_coring_result_t *p_first_sharpen_max_coring_result;
} ikc_in_sharpen_max_coring_t;

uint32 ikc_sharpen_max_coring(const ikc_in_sharpen_max_coring_t *p_in_sharpen_max_coring, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    ik_first_sharpen_scale_coring_t *p_first_sharpen_scale_coring;
} ikc_in_sharpen_scale_coring_t;

uint32 ikc_sharpen_scale_coring(const ikc_in_sharpen_scale_coring_t *p_in_sharpen_scale_coring, ikc_out_sharpen_t *p_out_sharpen);

typedef struct {
    ik_luma_noise_reduction_t *p_luma_noise_reduce;
    ik_wb_gain_t *p_after_ce_wb_gain;
    ik_rgb_to_12y_t *p_rgb_to_12y;
    ik_window_dimension_t *p_cfa_win;
} ikc_in_lnl_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
    idsp_ik_flow_info_t *p_flow_info;
} ikc_out_lnl_t;

uint32 ikc_lnl(const ikc_in_lnl_t *p_in_lnl, ikc_out_lnl_t *p_out_lnl);

typedef struct {
    ik_color_correction_t *p_color_correction;
    ik_tone_curve_t *p_tone_curve;
} ikc_in_lnl_tbl_t;

uint32 ikc_lnl_tbl(const ikc_in_lnl_tbl_t *p_in_lnl_tbl, ikc_out_lnl_t *p_out_lnl);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordintates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    ik_burst_tile_t *p_burst_tile;
    uint32 force_vscale_sec_n;// 2 : all v-scale done by sec2, 3 : all v-scale done by sec3, others : up by sec3, down by sec2.
} ikc_in_main_resamp_t;

typedef struct {
    // luma_resmp
    uint32 cr_33_size;
    void *p_cr_33;

    // chroma_resmp
    uint32 cr_35_size;
    void *p_cr_35;

    // chroma_upsmp
    uint32 cr_37_size;
    void *p_cr_37;

    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;
} ikc_out_main_resamp_t;

/* please call this after ikc_warp due to chroma_dma_size */
uint32 ikc_main_resampler(const ikc_in_main_resamp_t *p_in_main_resamp, ikc_out_main_resamp_t *p_out_main_resamp);

typedef struct {
    ik_video_mctf_t *p_video_mctf;
    ik_mctf_internal_t *p_internal_video_mctf;
    ik_video_mctf_ta_t *p_video_mctf_ta;
    ik_final_sharpen_both_t *p_final_sharpen_both;
    ik_window_dimension_t *p_main;
} ikc_in_mctf_t;

typedef struct {
    // mctf
    uint32 cr_111_size;
    void *p_cr_111;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_113_size;
    void *p_cr_113;
    uint32 cr_116_size;
    void *p_cr_116;

    idsp_ik_flow_info_t *p_flow_info;
    idsp_ik_phase_info_t *p_phase;
} ikc_out_mctf_t;

// remember to set POS_DEPEND_INITIAL_PHASE_HORIZENTAL and POS_DEPEND_PHASE_INCREMENT_HORIZONTAL ... etc
uint32 ikc_mctf(const ikc_in_mctf_t *p_in_mctf, ikc_out_mctf_t *p_out_mctf);

typedef struct {
    ik_pos_dep33_t *p_video_mctf_and_final_sharpen;
} ikc_in_mctf_pos_dep33_t;

typedef struct {
    uint32 cr_114_size;
    void *p_cr_114;
    uint32 cr_118_size;
    void *p_cr_118;
} ikc_out_mctf_pos_dep33_t;

uint32 ikc_mctf_pos_dep33(const ikc_in_mctf_pos_dep33_t *p_in_mctf_pos, ikc_out_mctf_pos_dep33_t *p_out_mctf_pos);

typedef struct {
    uint32 sharpen_mode;
    uint32 sharpen_both_enable;
    ik_final_sharpen_noise_t *p_final_sharpen_noise;
    ik_final_sharpen_fir_t *p_final_sharpen_fir;
    ik_window_dimension_t *p_main;
} ikc_in_final_sharpen_t;

typedef struct {
    // shpb
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_final_sharpen_t;

uint32 ikc_fnlshp(const ikc_in_final_sharpen_t *p_in_fnl_shp, ikc_out_final_sharpen_t *p_out_fnl_shp);

typedef struct {
    ik_final_sharpen_both_t *p_final_sharpen_both;
    uint32 mctf_y_combine_strength; // 0:256
} ikc_in_final_sharpen_both_t;

uint32 ikc_fnlshp_both(const ikc_in_final_sharpen_both_t *p_in_fnl_shp_both, ikc_out_final_sharpen_t *p_out_fnl_shp);

typedef struct {
    ik_final_sharpen_coring_t *p_final_sharpen_coring;
} ikc_in_final_sharpen_coring_t;

uint32 ikc_fnlshp_coring(const ikc_in_final_sharpen_coring_t *p_in_fnl_shp_coring, ikc_out_final_sharpen_t *p_out_fnl_shp);


typedef struct {
    ik_final_sharpen_coring_idx_scale_t *p_final_sharpen_coring_idx_scale;
} ikc_in_final_sharpen_coring_idx_scale_t;

uint32 ikc_fnlshp_coring_idx_scale(const ikc_in_final_sharpen_coring_idx_scale_t *p_in_fnl_shp_coring_idx_scale, ikc_out_final_sharpen_t *p_out_fnl_shp);


typedef struct {
    ik_final_sharpen_min_coring_result_t *p_final_sharpen_min_coring_result;
} ikc_in_final_sharpen_min_coring_t;

uint32 ikc_fnlshp_min_coring(const ikc_in_final_sharpen_min_coring_t *p_in_fnl_shp_min_coring, ikc_out_final_sharpen_t *p_out_fnl_shp);


typedef struct {
    ik_final_sharpen_max_coring_result_t *p_final_sharpen_max_coring_result;
} ikc_in_final_sharpen_max_coring_t;

uint32 ikc_fnlshp_max_coring(const ikc_in_final_sharpen_max_coring_t *p_in_fnl_shp_max_coring, ikc_out_final_sharpen_t *p_out_fnl_shp);


typedef struct {
    ik_final_sharpen_scale_coring_t *p_final_sharpen_scale_coring;
} ikc_in_final_sharpen_scale_coring_t;

uint32 ikc_fnlshp_scale_coring(const ikc_in_final_sharpen_scale_coring_t *p_in_fnl_shp_scale_coring, ikc_out_final_sharpen_t *p_out_fnl_shp);

typedef struct {
    // shpb
    uint32 cr_117_size;
    void *p_cr_117;
    // shpb 3d table
    uint32 cr_119_size;
    void *p_cr_119;
} ikc_out_final_sharpen_both_3d_t;

typedef struct {
    ik_final_sharpen_both_three_d_table_t *p_final_sharpen_both_three_d_table;
} ikc_in_final_sharpen_both_3d_t;

uint32 ikc_fnlshp_both_3d(const ikc_in_final_sharpen_both_3d_t *p_in_fnl_shp_both_3d, ikc_out_final_sharpen_both_3d_t *p_out_fnl_shp_both_3d);

typedef ik_in_warp_remap_t ikc_in_warp_remap_t;
typedef ik_out_warp_remap_t ikc_out_warp_remap_t;

uint32 ikc_warp_remap(const ikc_in_warp_remap_t *p_warp_remap_in, ikc_out_warp_remap_t *p_warp_remap_out);

typedef ik_in_cawarp_remap_t ikc_in_cawarp_remap_t;
typedef ik_out_cawarp_remap_t ikc_out_cawarp_remap_t;

uint32 ikc_cawarp_remap(const ikc_in_cawarp_remap_t *p_cawarp_remap_in, ikc_out_cawarp_remap_t *p_cawarp_remap_out);

#endif
