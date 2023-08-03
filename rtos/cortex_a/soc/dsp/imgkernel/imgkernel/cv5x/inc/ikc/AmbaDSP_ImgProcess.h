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

#include "idspdrv_imgknl_if.h"
#include "AmbaDSP_ImgFilter.h"
#include "AmbaDSP_ImgHisoFilter.h"


uint32 ikc_check_version(uint32 ik_version_major);
uint32 ikc_get_version(uint32 *p_major_ver, uint32 *p_minor_ver);

typedef struct {
    uint32 step;
} ikc_in_def_t;


typedef struct {
    uint32 flip_mode;
    uint32 is_yuv_mode;
    ik_vin_sensor_info_t *p_sensor_info;
} ikc_in_input_mode_t;

typedef struct {
    uint32 step;
} ikc_in_input_mode_y2y_t;

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

    idsp_flow_info_t *p_flow;
    idsp_calib_data_t *p_calib;
} ikc_out_input_mode_t;

typedef struct {
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
    //uint32 cr_117_size;
    //void *p_cr_117;

    idsp_flow_info_t *p_flow;
    idsp_calib_data_t *p_calib;
} ikc_out_input_mode_y2y_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;

    // chroma_down_smp
    uint32 cr_32_size;
    void *p_cr_32;

} ikc_out_step4a_def_value_t;


uint32 ikc_input_mode(const ikc_in_input_mode_t *p_in_input, ikc_out_input_mode_t *p_out_input);

//uint32 ikc_input_mode_y2y(const ikc_in_input_mode_t *p_in_input, ikc_out_input_mode_y2y_t *p_out_input);
uint32 ikc_input_mode_y2y_step2(const ikc_in_input_mode_t *p_in_input, ikc_out_input_mode_y2y_t *p_out_input);
uint32 ikc_input_mode_y2y_step3(const ikc_in_input_mode_y2y_t *p_in_input, ikc_out_input_mode_y2y_t *p_out_input);
uint32 ikc_input_mode_y2y_step4(const ikc_in_input_mode_y2y_t *p_in_input, ikc_out_input_mode_y2y_t *p_out_input);
uint32 ikc_hiso_sec2_hardcode(const ikc_in_def_t *p_in_hiso_def, ikc_out_step4a_def_value_t *p_out_step4a_def);

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

uint32 ikc_register_valid_mem_range(const void *p_mem_start, const void *p_mem_end);


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

    idsp_flow_info_t *p_flow;
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

    idsp_flow_info_t *p_flow;
} ikc_out_static_bad_pixel_internal_t;

uint32 ikc_static_bad_pixel_internal(const ikc_in_static_bad_pixel_internal_t *p_in_sbp, ikc_out_static_bad_pixel_internal_t *p_out_sbp);


typedef struct {
    uint32 vig_enable;
    uint32 flip_mode;
    ik_vignette_t *p_vignette_compensation;
    ik_vin_sensor_geometry_t *p_vin_sensor;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_vin_active_window_t *p_active_window;
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

    idsp_flow_info_t *p_flow;
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
    uint32 is_hiso;
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

    idsp_calib_data_t *p_calib; // IKC should fill all the cawarp parameters except table address.
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
    uint32 is_hiso;
    uint32 warp_enable;
    uint32 flip_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_info_t *p_calib_warp_info;
    uint32 dram_efficiency;
    uint32 luma_wait_lines;
    uint32 luma_dma_size;
    uint32 is_still_422;
} ikc_in_hwarp_t;

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

    idsp_flow_info_t *p_flow_info; /* data flow & SBP info */
    idsp_window_info_t *p_window;  /* window geometry */
    idsp_phase_info_t *p_phase;    /* phase inc & init phase */
    idsp_calib_data_t *p_calib; // IKC should fill all the warp parameters except table address.
    idsp_stitch_data_t *p_stitch; /*stitching info*/
    void *p_warp_horizontal_table_address; // IKC should copy the table contents to here
    uint32 h_size;
    void *p_warp_vertical_table_address; // IKC should copy the table contents to here
    uint32 v_size;
} ikc_out_hwarp_t;

uint32 ikc_hwarp(const ikc_in_hwarp_t *p_in_hwarp, ikc_out_hwarp_t *p_out_hwarp);

typedef struct {
    uint32 is_hvh_enabled;
    uint32 is_hhb_enabled;
    uint32 is_group_cmd;
    uint32 warp_enable;
    uint32 flip_mode;
    uint32 chroma_radius;
    ik_window_size_info_t *p_window_size_info;
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_info_t *p_calib_warp_info;
} ikc_in_2nd_hwarp_t;

typedef struct {
    // hwarp config
    uint32 cr_100_size;
    void *p_cr_100;

    // hwarp map
    uint32 cr_101_size;
    void *p_cr_101;

    idsp_flow_info_t *p_flow_info; /* data flow & SBP info */
    idsp_phase_info_t *p_phase;    /* phase inc & init phase */
    idsp_calib_data_t *p_calib; // IKC should fill all the warp parameters except table address.
    void *p_warp_horizontal_b_table_address; // IKC should copy the table contents to here
    uint32 h_size;
} ikc_out_2nd_hwarp_t;

uint32 ikc_secondary_hwarp(const ikc_in_2nd_hwarp_t *p_in_2nd_hwarp, const ikc_out_2nd_hwarp_t *p_out_2nd_hwarp);

//uint32 ikc_secondary_hwarp_core(const ikc_in_2nd_hwarp_t *p_in_2nd_hwarp, ikc_out_2nd_hwarp_t *p_out_2nd_hwarp);


typedef struct {
    uint32 is_hvh_enabled;
    uint32 is_hhb_enabled;
    uint32 is_multi_pass; // 0:optA_r2y, 1:optC_r2y, 2:optC_me1
    uint32 is_group_cmd;
    uint32 warp_enable;
    uint32 flip_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_info_t *p_calib_warp_info;
    uint32 dram_efficiency;
    uint32 luma_wait_lines;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_vwarp_t;

typedef struct {
    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;

    // vwarp_map
    uint32 cr_43_size;
    void *p_cr_43;

    // vwarp_me0
    uint32 cr_44_size;
    void *p_cr_44;

    idsp_window_info_t *p_window;  /* window geometry */
    idsp_calib_data_t *p_calib; // IKC should fill all the warp parameters except table address.
    void *p_warp_vertical_table_address; // IKC should copy the table contents to here
    uint32 v_size;
} ikc_out_vwarp_t;

uint32 ikc_vwarp(const ikc_in_vwarp_t *p_in_vwarp, const ikc_out_vwarp_t *p_out_vwarp);


typedef struct {
    uint32 flip_mode;
    ik_window_size_info_t *p_window_size_info;
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_internal_info_t *p_warp_internal;
    uint32 dram_efficiency;
    uint32 luma_wait_lines;
    uint32 luma_dma_size;
} ikc_in_warp_internal_t;

uint32 ikc_hwarp_internal(const ikc_in_warp_internal_t *p_in_warp_internal, ikc_out_hwarp_t *p_out_hwarp);
uint32 ikc_vwarp_internal(const ikc_in_warp_internal_t *p_in_warp_internal, ikc_out_vwarp_t *p_out_vwarp);

typedef struct {
    ik_window_size_info_t *p_window_size_info;
    ik_stitch_info_t *p_stitching_info;
    ik_warp_2nd_internal_info_t *p_warp_2nd_internal;
} ikc_in_2nd_hwarp_internal_t;

uint32 ikc_secondary_hwarp_internal(const ikc_in_2nd_hwarp_internal_t *p_in_2nd_hwarp_internal, ikc_out_2nd_hwarp_t *p_out_2nd_hwarp);

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

    idsp_flow_info_t *p_flow;
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

    idsp_flow_info_t *p_flow;
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
    uint32 is_ir_only;
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
    ik_window_geomery_t *p_dmy_win_geo;
    ik_vin_sensor_geometry_t *p_vin_sensor;
    int32 horizontal_shift_exp1;//unit in pixels.
    int32 vertical_shift_exp1;
    int32 horizontal_shift_exp2;
    int32 vertical_shift_exp2;
} ikc_in_hdr_eis_t;

typedef struct {
    // hdr_blend
    uint32 cr_4_size;
    void *p_cr_4;

    // flow idsp_extra_window_info_t
    void *p_extra_window_info;
    idsp_window_info_t *p_window;  /* window geometry */
} ikc_out_hdr_eis_t;

uint32 ikc_hdr_eis(const ikc_in_hdr_eis_t *p_in_hdr_eis, ikc_out_hdr_eis_t *p_out_hdr_eis);

typedef struct {
    // hdr_blend_b
    uint32 cr_45_size;
    void *p_cr_45;
} ikc_out_hdr_eis_sub_t;

uint32 ikc_hdr_eis_sub(const ikc_in_hdr_eis_t *p_in_hdr_eis, ikc_out_hdr_eis_sub_t *p_out_hdr_eis_sub);

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
    ik_stored_ir_t *p_stored_ir;
} ikc_in_stored_ir_t;

typedef struct {
    uint32 cr_12_size;
    void *p_cr_12;
} ikc_out_stored_ir_t;

uint32 ikc_stored_ir(const ikc_in_stored_ir_t *p_in_stored_ir, ikc_out_stored_ir_t *p_out_stored_ir);

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

    idsp_flow_info_t *p_flow;
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
    idsp_flow_info_t *p_flow;
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
    uint32 is_ir_only;
    ik_color_correction_t *p_color_correction;
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
    uint32 is_first_frame;
    ik_pre_cc_gain_t *p_pre_cc_gain;
} ikc_in_cc_premul_t;

uint32 ikc_color_correction_premul(const ikc_in_cc_premul_t *p_in_cc_premul, ikc_out_cc_out_t *p_out_cc_premul);


typedef struct {
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
    uint32 is_ir_only;
    ik_rgb_to_yuv_matrix_t *p_rgb_to_yuv_matrix;
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
    uint32 is_wide_chroma_invalid;
    ik_chroma_filter_t *p_chroma_filter;
    ik_wide_chroma_filter_t *p_wide_chroma_filter;
    ik_wide_chroma_filter_combine_t *p_wide_chroma_combine;
} ikc_in_chroma_noise_t;

typedef struct {
    // chroma_noise
    uint32 cr_36_size;
    void *p_cr_36;

    idsp_window_info_t *p_window;  /* window geometry */
} ikc_out_chroma_noise_t;

uint32 ikc_chroma_noise(const ikc_in_chroma_noise_t *p_in_chroma_noise, ikc_out_chroma_noise_t *p_out_chroma_noise);

typedef struct {
    uint32 is_first_frame;
} ikc_in_chroma_noise_default_val_t;
typedef struct {
    // set chroma_noise default value
    uint32 cr_36_size;
    void *p_cr_36;
} ikc_out_chroma_noise_default_val_t;

uint32 ikc_chroma_noise_default(const ikc_in_chroma_noise_default_val_t *p_in_chroma_noise, ikc_out_chroma_noise_default_val_t *p_out_chroma_noise);


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
uint32 ikc_hiso_asf(const ikc_in_asf_t *p_in_asf, ikc_out_asf_t *p_out_asf);

typedef struct {
    ik_hi_asf_a_t *p_advance_spatial_filter;
    uint32 first_luma_processing_mode_updated;// for set to default
} ikc_in_asf_a_t;

uint32 ikc_hiso_asf_a(const ikc_in_asf_a_t *p_in_asf, ikc_out_asf_t *p_out_asf);

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
    //SHP
    ik_hi_high_shpns_both_t        *hi_high_shpns_both;
    ik_hi_high_shpns_noise_t       *hi_high_shpns_noise;
    ik_hi_high_shpns_coring_t      *hi_high_shpns_coring;
    ik_hi_high_shpns_fir_t         *hi_high_shpns_fir;
    ik_hi_high_shpns_cor_idx_scl_t *hi_high_shpns_cor_idx_scl;
    ik_hi_high_shpns_min_cor_rst_t *hi_high_shpns_min_cor_rst;
    ik_hi_high_shpns_max_cor_rst_t *hi_high_shpns_max_cor_rst;
    ik_hi_high_shpns_scl_cor_t     *hi_high_shpns_scl_cor;
} ikc_in_step9_shpa_noise_t;

uint32 ikc_hiso_step9_shpa_nosie_filter(const ikc_in_step9_shpa_noise_t *p_in_step9_shpa_noise, ikc_out_sharpen_t *p_out_sharpen);


typedef struct {
    uint32 sharpen_mode;
    ik_hi_med_shpns_noise_t *p_first_sharpen_noise;
    ik_first_sharpen_fir_t  *p_first_sharpen_fir;
    void *p_working_buffer;
    uint32 working_buffer_size;//2700 bytes.
} ikc_in_hiso_sharpen_t;

uint32 ikc_hiso_sharpen(const ikc_in_hiso_sharpen_t *p_in_sharpen, ikc_out_sharpen_t *p_out_sharpen);

typedef struct {
    ik_first_sharpen_both_t *p_first_sharpen_both;
} ikc_in_sharpen_both_t;

uint32 ikc_sharpen_both_disable(ikc_out_sharpen_t *p_out_sharpen);
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
    // luma_shp
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_sec18_sharpen_t;

typedef struct {
    ik_hi_nonsmooth_detect_t *p_in_hi_nonsmooth_detect;
} ikc_in_step2_asf_mcts;
uint32 ikc_sec18_step2_asf_mcts(const ikc_in_step2_asf_mcts *p_in_step2_asf_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_hi_high_asf_t *p_in_hi_high_asf;
} ikc_in_step3_asf_mcts;
uint32 ikc_sec18_step3_asf_mcts(const ikc_in_step3_asf_mcts *p_in_step3_asf_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_hi_low_asf_t *p_in_hi_low_asf;
} ikc_in_step4_asf_mcts;
uint32 ikc_sec18_step4_asf_mcts(const ikc_in_step4_asf_mcts *p_in_step4_asf_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_hi_med2_asf_t *p_in_hi_med2_asf;
} ikc_in_step5_asf_mcts;
uint32 ikc_sec18_step5_asf_mcts(const ikc_in_step5_asf_mcts *p_in_step5_asf_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);


typedef struct {
    ik_li2_shpns_both_t   li2_shpns_both;
    ik_li2_shpns_noise_t  li2_shpns_noise;
    ik_li2_shpns_coring_t li2_shpns_coring;
    ik_li2_shpns_fir_t    li2_shpns_fir;
    ik_li2_shpns_cor_idx_scl_t li2_shpns_cor_idx_scl;
    ik_li2_shpns_min_cor_rst_t li2_shpns_min_cor_rst;
    ik_li2_shpns_max_cor_rst_t li2_shpns_max_cor_rst;
    ik_li2_shpns_scl_cor_t     li2_shpns_scl_cor;
} li2_shp_t;

typedef struct {
    ik_li2_shpns_both_t        *p_li2_shpns_both;
    ik_li2_shpns_noise_t       *p_li2_shpns_noise;
    ik_li2_shpns_coring_t      *p_li2_shpns_coring;
    ik_li2_shpns_fir_t         *p_li2_shpns_fir;
    ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl;
    ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst;
    ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst;
    ik_li2_shpns_scl_cor_t     *p_li2_shpns_scl_cor;
} ikc_in_sec18_step12_shp_mcts_t;


typedef struct {
    ik_hili_shpns_both_t hili_shpns_both;
    ik_hili_shpns_noise_t hili_shpns_noise;
    ik_hili_shpns_coring_t hili_shpns_coring;
    ik_hili_shpns_fir_t hili_shpns_fir;
    ik_hili_shpns_cor_idx_scl_t hili_shpns_cor_idx_scl;
    ik_hili_shpns_min_cor_rst_t hili_shpns_min_cor_rst;
    ik_hili_shpns_max_cor_rst_t hili_shpns_max_cor_rst;
    ik_hili_shpns_scl_cor_t hili_shpns_scl_cor;
} low_high_shp_t;

typedef struct {
    ik_hili_shpns_both_t          *p_hili_shpns_both;
    ik_hili_shpns_noise_t         *p_hili_shpns_noise;
    ik_hili_shpns_coring_t        *p_hili_shpns_coring;
    ik_hili_shpns_fir_t           *p_hili_shpns_fir;
    ik_hili_shpns_cor_idx_scl_t   *p_hili_shpns_cor_idx_scl;
    ik_hili_shpns_min_cor_rst_t   *p_hili_shpns_min_cor_rst;
    ik_hili_shpns_max_cor_rst_t   *p_hili_shpns_max_cor_rst;
    ik_hili_shpns_scl_cor_t       *p_hili_shpns_scl_cor;
    ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover;
} ikc_in_sec18_step11_shp_mcts_t;

uint32 ikc_sec18_step11_shp_mcts(const ikc_in_sec18_step11_shp_mcts_t *p_in_sec18_step11_shp_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);
uint32 ikc_sec18_step12_shp_mcts(const ikc_in_sec18_step12_shp_mcts_t *p_in_sec18_step12_shp_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_both_t *p_li2_shpns_both;
} ikc_in_sec18_shp_both_t;
uint32 ikc_sec18_sharpen_both(const ikc_in_sec18_shp_both_t *p_in_sharpen_both, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_noise_t *p_li2_shpns_noise;
    ik_li2_shpns_both_t  *p_li2_shpns_both;
} ikc_in_sec18_shp_noise_t;
uint32 ikc_sec18_sharpen_noise(const ikc_in_sec18_shp_noise_t *p_in_sharpen_noise, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_coring_t *p_li2_shpns_coring;
} ikc_in_sec18_shp_coring_t;
uint32 ikc_sec18_sharpen_coring(const ikc_in_sec18_shp_coring_t *p_in_sharpen_coring, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_both_t *p_li2_shpns_both;
    ik_li2_shpns_noise_t *p_li2_shpns_noise;
    ik_li2_shpns_fir_t *p_li2_shpns_fir;
} hi_low2_shp;

typedef struct {
    ik_li2_shpns_fir_t *p_li2_shpns_fir;
    ik_li2_shpns_both_t *p_li2_shpns_both;
    ik_li2_shpns_noise_t *p_li2_shpns_noise;
} ikc_in_sec18_shp_fir_t;
uint32 ikc_sec18_sharpen_fir(const ikc_in_sec18_shp_fir_t *p_in_sharpen_fir, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_fir_t            *p_hili_shpns_fir;
    ik_li2_shpns_both_t           *p_hili_shpns_both;
    ik_li2_shpns_noise_t          *p_hili_shpns_noise;
    ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover;
} ikc_in_sec18_freq_recover_mcts_t;

typedef struct {
    ik_hili_shpns_both_t  *p_hili_shpns_both;
    ik_hili_shpns_noise_t *p_hili_shpns_noise;
    ik_hili_shpns_fir_t   *p_hili_shpns_fir;
    ik_hi_mid_high_freq_recover_t *p_hi_mid_high_freq_recover;
} hi_step11_low2_shp;
uint32 ikc_sec18_freq_recover_mcts(const ikc_in_sec18_freq_recover_mcts_t *p_in_freq_recover_mcts, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_cor_idx_scl_t *p_li2_shpns_cor_idx_scl;
} ikc_in_sec18_shp_coring_idx_scale_t;
uint32 ikc_sec18_sharpen_coring_idx_scale(const ikc_in_sec18_shp_coring_idx_scale_t *p_in_sharpen_coring_idx_scale, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_min_cor_rst_t *p_li2_shpns_min_cor_rst;
} ikc_in_sec18_shp_min_coring_t;
uint32 ikc_sec18_sharpen_min_coring(const ikc_in_sec18_shp_min_coring_t *p_in_sharpen_min_coring, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_max_cor_rst_t *p_li2_shpns_max_cor_rst;
} ikc_in_sec18_shp_max_coring_t;
uint32 ikc_sec18_sharpen_max_coring(const ikc_in_sec18_shp_max_coring_t *p_in_sharpen_max_coring, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_li2_shpns_scl_cor_t *p_li2_shpns_scl_cor;
} ikc_in_sec18_shp_scale_coring_t;
uint32 ikc_sec18_sharpen_scale_coring(const ikc_in_sec18_shp_scale_coring_t *p_in_sharpen_scale_coring, ikc_out_sec18_sharpen_t *p_out_sharpen);

typedef struct {
    ik_luma_noise_reduction_t *p_luma_noise_reduce;
    ik_wb_gain_t *p_after_ce_wb_gain;
    ik_rgb_to_12y_t *p_rgb_to_12y;
    ik_window_dimension_t *p_cfa_win;
    uint32 use_sharpen_not_asf;
    ik_adv_spatial_filter_t *p_advance_spatial_filter;
    ik_first_sharpen_both_t *p_first_sharpen_both;
} ikc_in_lnl_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
    idsp_flow_info_t *p_flow_info;
} ikc_out_lnl_t;

uint32 ikc_lnl(const ikc_in_lnl_t *p_in_lnl, ikc_out_lnl_t *p_out_lnl);

typedef struct {
    ik_color_correction_t *p_color_correction;
    ik_tone_curve_t *p_tone_curve;
} ikc_in_lnl_tbl_t;

uint32 ikc_lnl_tbl(const ikc_in_lnl_tbl_t *p_in_lnl_tbl, ikc_out_lnl_t *p_out_lnl);

typedef struct {
    uint32 ext_raw_out_mode;
} ikc_in_cfa_lossy_compression_t;

typedef struct {
    // cfa_lossy_compr
    uint32 cr_15_size;
    void *p_cr_15;
    idsp_flow_info_t *p_flow_info;
} ikc_out_cfa_lossy_compression_t;

uint32 ikc_cfa_lossy_compression(const ikc_in_cfa_lossy_compression_t *p_in_cfa_lossy_compr, ikc_out_cfa_lossy_compression_t *p_out_cfa_lossy_compr);

#if 0
typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
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

uint32 ikc_main_resampler(const ikc_in_main_resamp_t *p_in_main_resamp, ikc_out_main_resamp_t *p_out_main_resamp);
#endif
typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_main_resampler_t;

typedef struct {
    uint32 step;
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_window_dimension_t *p_main;
    uint32 chroma_radius;
} ikc_in_sec2_hiso_resampler_t;

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
} ikc_out_sec2_main_resampler_t;

uint32 ikc_sec2_main_resampler(const ikc_in_sec2_main_resampler_t *p_in_sec2_main_resampler, ikc_out_sec2_main_resampler_t *p_out_sec2_main_resampler);
uint32 ikc_sec2_hiso_resampler(const ikc_in_sec2_hiso_resampler_t *p_in_sec2_hiso_resampler, ikc_out_sec2_main_resampler_t *p_out_sec2_main_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_optc_main_resampler_t;

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
} ikc_out_sec2_optc_main_resampler_t;

uint32 ikc_sec2_optc_main_resampler(const ikc_in_sec2_optc_main_resampler_t *p_in_sec2_optc_main_resampler, ikc_out_sec2_optc_main_resampler_t *p_out_sec2_optc_main_resampler);

typedef struct {
    uint32 is_hhb_enabled;
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_window_dimension_t *p_main;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 luma_dma_size;
} ikc_in_sec2_motion_me1_resampler_t;

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
} ikc_out_sec2_motion_me1_resampler_t;

uint32 ikc_sec2_motion_me1_resampler(const ikc_in_sec2_motion_me1_resampler_t *p_in_sec2_motion_me1_resampler, ikc_out_sec2_motion_me1_resampler_t *p_out_sec2_motion_me1_resampler);

typedef struct {
    uint32 is_hhb_enabled;
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_window_dimension_t *p_main;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 luma_dma_size;
} ikc_in_sec2_motion_abc_resampler_t;

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
} ikc_out_sec2_motion_abc_resampler_t;

uint32 ikc_sec2_motion_abc_resampler(const ikc_in_sec2_motion_abc_resampler_t *p_in_sec2_motion_abc_resampler, ikc_out_sec2_motion_abc_resampler_t *p_out_sec2_motion_abc_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_mono1_8_resampler_t;

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
} ikc_out_sec2_mono1_8_resampler_t;

uint32 ikc_sec2_mono1_8_resampler(const ikc_in_sec2_mono1_8_resampler_t *p_in_sec2_mono1_8_resampler, ikc_out_sec2_mono1_8_resampler_t *p_out_sec2_mono1_8_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_mono3_resampler_t;

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
} ikc_out_sec2_mono3_resampler_t;

uint32 ikc_sec2_mono3_resampler(const ikc_in_sec2_mono3_resampler_t *p_in_sec2_mono3_resampler, ikc_out_sec2_mono3_resampler_t *p_out_sec2_mono3_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_mono4_resampler_t;

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
} ikc_out_sec2_mono4_resampler_t;

uint32 ikc_sec2_mono4_resampler(const ikc_in_sec2_mono4_resampler_t *p_in_sec2_mono4_resampler, ikc_out_sec2_mono4_resampler_t *p_out_sec2_mono4_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_mono5_resampler_t;

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
} ikc_out_sec2_mono5_resampler_t;

uint32 ikc_sec2_mono5_resampler(const ikc_in_sec2_mono5_resampler_t *p_in_sec2_mono5_resampler, ikc_out_sec2_mono5_resampler_t *p_out_sec2_mono5_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_mono6_resampler_t;

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
} ikc_out_sec2_mono6_resampler_t;

uint32 ikc_sec2_mono6_resampler(const ikc_in_sec2_mono6_resampler_t *p_in_sec2_mono6_resampler, ikc_out_sec2_mono6_resampler_t *p_out_sec2_mono6_resampler);

typedef struct {
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
    uint32 luma_dma_size;
    ik_burst_tile_t *p_burst_tile;
} ikc_in_sec2_mono7_resampler_t;

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
} ikc_out_sec2_mono7_resampler_t;

uint32 ikc_sec2_mono7_resampler(const ikc_in_sec2_mono7_resampler_t *p_in_sec2_mono7_resampler, ikc_out_sec2_mono7_resampler_t *p_out_sec2_mono7_resampler);

typedef struct {
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
} ikc_in_sec3_vertical_resampler_t;

typedef struct {
    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;
} ikc_out_sec3_vertical_resampler_t;

uint32 ikc_sec3_vertical_resampler(const ikc_in_sec3_vertical_resampler_t *p_in_sec3_vertical_resampler, ikc_out_sec3_vertical_resampler_t *p_out_sec3_vertical_resampler);
uint32 ikc_sec3_vertical_resampler_still(const ikc_in_sec3_vertical_resampler_t *p_in_sec3_vertical_resampler, ikc_out_sec3_vertical_resampler_t *p_out_sec3_vertical_resampler);

typedef struct {
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
} ikc_in_sec3_optc_vertical_resampler_t;

typedef struct {
    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;
} ikc_out_sec3_optc_vertical_resampler_t;

uint32 ikc_sec3_optc_vertical_resampler(const ikc_in_sec3_optc_vertical_resampler_t *p_in_sec3_optc_vertical_resampler, ikc_out_sec3_optc_vertical_resampler_t *p_out_sec3_optc_vertical_resampler);

typedef struct {
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 is_y2y_420input;
    uint32 flip_mode;
} ikc_in_sec3_motion_me1_vertical_resampler_t;

typedef struct {
    // vwarp
    uint32 cr_42_size;
    void *p_cr_42;
} ikc_out_sec3_motion_me1_vertical_resampler_t;

uint32 ikc_sec3_motion_me1_vertical_resampler(const ikc_in_sec3_motion_me1_vertical_resampler_t *p_in_sec3_motion_me1_vertical_resampler, ikc_out_sec3_motion_me1_vertical_resampler_t *p_out_sec3_motion_me1_vertical_resampler);

typedef struct {
    uint32 is_hhb_enabled;
    uint32 main_cut_off_freq;/* 0:M, 1:M-2, 2:M-4, 3:M-6, 4:M-8, 5:M-10, 6:M-12, 7:M-14 */
    ik_win_coordinates_t *p_act_win_crop;
    ik_window_geomery_t *p_dmy_win_geo;
    ik_window_dimension_t *p_cfa_win_dim;
    ik_window_dimension_t *p_main;
    ik_dummy_margin_range_t *p_dmy_range;
    uint32 chroma_radius;
    uint32 flip_mode;
    uint32 luma_dma_size;
} ikc_in_sec11_main_resampler_t;

typedef struct {
    uint32 cr_100_size;
    void *p_cr_100;
} ikc_out_sec11_main_resampler_t;

uint32 ikc_sec11_main_resampler(const ikc_in_sec11_main_resampler_t *p_in_sec11_main_resampler, ikc_out_sec11_main_resampler_t *p_out_sec11_main_resampler);

typedef struct {
    uint32 is_hhb_enabled;
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

    idsp_flow_info_t *p_flow_info;
    idsp_phase_info_t *p_phase;
} ikc_out_mctf_t;

typedef struct {
    ik_video_mctf_t *p_video_mctf;
} ikc_in_mctf_compression_t;

typedef struct {
    // mctf
    uint32 cr_111_size;
    void *p_cr_111;
    uint32 cr_116_size;
    void *p_cr_116;

    idsp_flow_info_t *p_flow_info;
} ikc_out_mctf_compression_t;

typedef struct {
    uint32 cr_112_size;
    void *p_cr_112;

    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_sec18_mctf_shpb_t;


typedef struct {
    ik_hi_luma_blend_t *p_hi_luma_blend;
} ikc_in_hili2_luma_blend_mctf_t;

typedef struct {
    uint32 is_yuv_422;
    uint32 step;
} ikc_in_mctf_pass_through_t;

typedef struct {
    uint32 step;
} ikc_in_sec18_chroma_format_t;

// remember to set POS_DEPEND_INITIAL_PHASE_HORIZENTAL and POS_DEPEND_PHASE_INCREMENT_HORIZONTAL ... etc
uint32 ikc_mctf(const ikc_in_mctf_t *p_in_mctf, ikc_out_mctf_t *p_out_mctf);
uint32 ikc_mctf_compression(const ikc_in_mctf_compression_t *p_in_mctf_compression, ikc_out_mctf_compression_t *p_out_mctf_compression);
uint32 ikc_hiso_mctf_pass_through(const ikc_in_mctf_pass_through_t *p_in_mctf_pass_through, ikc_out_sec18_mctf_shpb_t *p_out_mctf);
uint32 ikc_hili2_luma_blend_mctf(const ikc_in_hili2_luma_blend_mctf_t *p_in_mctf, ikc_out_sec18_mctf_shpb_t *p_out_mctf);
uint32 ikc_hiso_sec18_chroma_format(const ikc_in_sec18_chroma_format_t *p_in_mctf_pass_through, ikc_out_sec18_mctf_shpb_t *p_out_mctf);
uint32 ikc_hiso_mctf_ta_constraints(ikc_out_sec18_mctf_shpb_t *p_out_mctf);
uint32 ikc_hiso_set_regs_that_mctf_rtl_needs(const ikc_in_def_t *p_in_input, ikc_out_sec18_mctf_shpb_t *p_out_mctf);

typedef struct {
    uint32 is_yuv_422;
    uint32 step;
} ikc_in_shpb_pass_through_t;
uint32 ikc_hiso_shpb_pass_through(const ikc_in_shpb_pass_through_t *p_in_shpb_pass_through, ikc_out_sec18_mctf_shpb_t *p_out_shpb);
uint32 ikc_hiso_shpa_pass_through(ikc_out_asf_t *p_out_asf);

typedef struct {
    uint32 step;
    ik_window_dimension_t      *p_main;
    ik_hi_nonsmooth_detect_t   *hi_nonsmooth_detect;
    ik_hi_chroma_filter_high_t *hi_chroma_filter_high;
    ik_chroma_filter_t         *chroma_filter;
} ikc_in_hiso_frame_size_t;

typedef struct {
    uint32 step;
    ik_window_dimension_t      *p_main;
    ik_hi_nonsmooth_detect_t   *hi_nonsmooth_detect;
    ik_hi_chroma_filter_high_t *hi_chroma_filter_high;
    ik_chroma_filter_t         *chroma_filter;
} ikc_in_hiso_resampler_size_t;

typedef struct {
    ik_window_dimension_t output;
    idsp_flow_ctrl_t     *p_flow;
} ikc_out_hiso_resampler_size_t;

uint32 ikc_hiso_shpa_frame_size(const ikc_in_hiso_frame_size_t *p_in_hiso_shpa_frame_size, ikc_out_asf_t *p_out_shpa);
uint32 ikc_hiso_sec18_frame_size_div16_m1(const ikc_in_hiso_frame_size_t *p_in_frame_size_div16_m1, ikc_out_sec18_mctf_shpb_t *p_out_frame_size_div16_m1);
uint32 ikc_hiso_resampler_size(const ikc_in_hiso_resampler_size_t *p_in_hiso_resampler, ikc_out_hiso_resampler_size_t *p_out_hiso_resampler_size);

typedef struct {
    uint32 step;
    ik_hi_luma_combine_t *p_hi_luma_filter_combine;
} ikc_in_luma_combine_2_mctf_t;

typedef struct {
    ik_hi_chroma_filter_combine_t *p_hili_combine_cbcr;
} ikc_in_luma_combine_2_mctf_cbcr_t;

uint32 ikc_luma_combine_2_mctf(const ikc_in_luma_combine_2_mctf_t *p_in_mctf, ikc_out_mctf_t *p_out_mctf);
uint32 ikc_luma_combine_2_mctf_cbcr(const ikc_in_luma_combine_2_mctf_cbcr_t *p_in_mctf, ikc_out_mctf_t *p_out_mctf);

typedef struct {
    ik_hi_chroma_filter_combine_t *p_hi_chroma_filter_combine;
} ikc_in_chroma_combine_2_mctf_t;
uint32 ikc_chroma_combine_2_mctf(const ikc_in_chroma_combine_2_mctf_t *p_in_mctf, ikc_out_mctf_t *p_out_mctf);

typedef struct {
    ik_hili_combine_t *p_hili_combine;
} ikc_in_hili_combine_cbcr_mctf_t;
uint32 ikc_hili_combine_cbcr(const ikc_in_hili_combine_cbcr_mctf_t *p_in_mctf, ikc_out_mctf_t *p_out_mctf);

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
    uint32 is_hhb_enabled;
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
uint32 ikc_warp_remap_hvh_vr(const ikc_in_warp_remap_t *p_warp_remap_in, ikc_out_warp_remap_t *p_warp_remap_out, ikc_out_warp_remap_t *p_warp_remap_out_2nd);

typedef ik_in_cawarp_remap_t ikc_in_cawarp_remap_t;
typedef ik_out_cawarp_remap_t ikc_out_cawarp_remap_t;

uint32 ikc_cawarp_remap(const ikc_in_cawarp_remap_t *p_cawarp_remap_in, ikc_out_cawarp_remap_t *p_cawarp_remap_out);


///////////////////////////////////////////////// hiso

typedef struct {
    uint32 exp_num;
    ik_hdr_raw_info_t *p_hdr_raw_info;
    ik_window_dimension_t      *p_main;
    ik_hi_nonsmooth_detect_t   *hi_nonsmooth_detect;
    ik_hi_chroma_filter_high_t *hi_chroma_filter_high;
} ikc_in_temp_hack_hiso_data_t;

typedef struct {
    idsp_flow_ctrl_t *p_flow;
} ikc_out_temp_hack_hiso_data_t;

uint32 ikc_temp_hack_hiso_data(const ikc_in_temp_hack_hiso_data_t *p_in_hiso_hack, const ikc_out_temp_hack_hiso_data_t *p_out_hiso_hack);
void ikc_temp_hack_hiso_vwarp(void *p_addr);


//////////////////// step1

//////////////////// step2
typedef struct {
    ik_hi_nonsmooth_detect_t *p_hi_nonsmooth_detect;
} ikc_in_cc_edge_score_t;

typedef struct {
    // cc_3d
    uint32 cr_27_size;
    void *p_cr_27;
} ikc_out_cc_edge_score_t;

uint32 ikc_color_correction_edge_score(const ikc_in_cc_edge_score_t *p_in_cc_edge_score, ikc_out_cc_edge_score_t *p_out_cc_edge_score);


//////////////////// step3

//////////////////// step4

//////////////////// step4a

//////////////////// step5

//////////////////// step6

//////////////////// step7

//////////////////// step8

//////////////////// step9

//////////////////// step10

//////////////////// step11

//////////////////// step12

//////////////////// step13

// MotionABC

//////////////////// step1
typedef struct {
} ikc_in_r2y_default_tbl_t;

typedef struct {
    uint32 cr_112_size;
    void *p_cr_112;
} ikc_out_r2y_default_tbl_t;

uint32 ikc_r2y_default_tbl(const ikc_in_r2y_default_tbl_t *p_in_r2y_default_tbl, const ikc_out_r2y_default_tbl_t *p_out_r2y_default_tbl);

typedef struct {
    ik_motion_detect_t *p_motion_detect;
    ik_video_mctf_t *p_video_mctf;
} ikc_in_optc_r2y_t;

typedef struct {
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_optc_r2y_t;

uint32 ikc_optc_r2y(const ikc_in_optc_r2y_t *p_in_optc_r2y, ikc_out_optc_r2y_t *p_out_optc_r2y);

//////////////////// step2
typedef struct {
} ikc_in_motion_me1_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_29_size;
    void *p_cr_29;
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_31_size;
    void *p_cr_31;
    uint32 cr_32_size;
    void *p_cr_32;
    uint32 cr_36_size;
    void *p_cr_36;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_motion_me1_default_tbl_t;

uint32 ikc_motion_me1_default_tbl(const ikc_in_motion_me1_default_tbl_t *p_in_motion_me1_default_tbl, const ikc_out_motion_me1_default_tbl_t *p_out_motion_me1_default_tbl);

typedef struct {
    uint32 is_hhb_enabled;
    ik_window_dimension_t *p_main;
} ikc_in_motion_me1_input_size_t;

typedef struct {
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_motion_me1_input_size_t;

uint32 ikc_motion_me1_input_size(const ikc_in_motion_me1_input_size_t *p_in_motion_me1_input_size, ikc_out_motion_me1_input_size_t *p_out_motion_me1_input_size);

//////////////////// step3
typedef struct {
    ik_motion_detect_t *p_motion_detect;
} ikc_in_motion_detect_a_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
} ikc_out_motion_detect_a_t;

uint32 ikc_motion_detect_a(const ikc_in_motion_detect_a_t *p_in_motion_detect_a, ikc_out_motion_detect_a_t *p_out_motion_detect_a);

typedef struct {
    ik_motion_detect_t *p_motion_detect;
    ik_pos_dep33_t *p_pos_dep33;
    ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf;
    ik_window_dimension_t *p_main;
} ikc_in_motion_detect_a_mctf_t;

typedef struct {
    // luma_shp
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_114_size;
    void *p_cr_114;
    idsp_step_info_t *p_step_info;
} ikc_out_motion_detect_a_mctf_t;

uint32 ikc_motion_detect_a_mctf(const ikc_in_motion_detect_a_mctf_t *p_in_motion_detect_a_mctf, ikc_out_motion_detect_a_mctf_t *p_out_motion_detect_a_mctf);

typedef struct {
    uint32 is_hhb_enabled;
    ik_window_dimension_t *p_main;
    uint32 motion_detect_smooth;
} ikc_in_motion_a_input_size_t;

typedef struct {
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_motion_a_input_size_t;

uint32 ikc_motion_a_input_size(const ikc_in_motion_a_input_size_t *p_in_motion_a_input_size, ikc_out_motion_a_input_size_t *p_out_motion_a_input_size);

//////////////////// step4
typedef struct {
    ik_motion_detect_t *p_motion_detect;
} ikc_in_motion_detect_b_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
} ikc_out_motion_detect_b_t;

uint32 ikc_motion_detect_b(const ikc_in_motion_detect_b_t *p_in_motion_detect_b, ikc_out_motion_detect_b_t *p_out_motion_detect_b);

typedef struct {
    ik_motion_detect_t *p_motion_detect;
    ik_pos_dep33_t *p_pos_dep33;
    ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf;
    ik_window_dimension_t *p_main;
} ikc_in_motion_detect_b_mctf_t;

typedef struct {
    // luma_shp
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_114_size;
    void *p_cr_114;
} ikc_out_motion_detect_b_mctf_t;

uint32 ikc_motion_detect_b_mctf(const ikc_in_motion_detect_b_mctf_t *p_in_motion_detect_b_mctf, ikc_out_motion_detect_b_mctf_t *p_out_motion_detect_b_mctf);

typedef struct {
    uint32 is_hhb_enabled;
    ik_window_dimension_t *p_main;
    uint32 motion_detect_smooth;
} ikc_in_motion_b_input_size_t;

typedef struct {
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_motion_b_input_size_t;

uint32 ikc_motion_b_input_size(const ikc_in_motion_b_input_size_t *p_in_motion_b_input_size, ikc_out_motion_b_input_size_t *p_out_motion_b_input_size);

//////////////////// step5
typedef struct {
    ik_motion_detect_t *p_motion_detect;
} ikc_in_motion_detect_c_t;

typedef struct {
    // luma_shp
    uint32 cr_30_size;
    void *p_cr_30;
} ikc_out_motion_detect_c_t;

uint32 ikc_motion_detect_c(const ikc_in_motion_detect_c_t *p_in_motion_detect_c, ikc_out_motion_detect_c_t *p_out_motion_detect_c);

typedef struct {
    ik_motion_detect_t *p_motion_detect;
    ik_pos_dep33_t *p_pos_dep33;
    ik_motion_detect_and_mctf_t *p_motion_detect_and_mctf;
    ik_window_dimension_t *p_main;
} ikc_in_motion_detect_c_mctf_t;

typedef struct {
    // luma_shp
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_114_size;
    void *p_cr_114;
} ikc_out_motion_detect_c_mctf_t;

uint32 ikc_motion_detect_c_mctf(const ikc_in_motion_detect_c_mctf_t *p_in_motion_detect_c_mctf, ikc_out_motion_detect_c_mctf_t *p_out_motion_detect_c_mctf);

typedef struct {
    uint32 is_hhb_enabled;
    ik_window_dimension_t *p_main;
    uint32 motion_detect_smooth;
} ikc_in_motion_c_input_size_t;

typedef struct {
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_motion_c_input_size_t;

uint32 ikc_motion_c_input_size(const ikc_in_motion_c_input_size_t *p_in_motion_c_input_size, ikc_out_motion_c_input_size_t *p_out_motion_c_input_size);

//////////////////// mono1_8
typedef struct {
    ik_fusion_t *p_fusion;
} ikc_in_mono1_8_mctf_t;

typedef struct {
    uint32 cr_117_size;
    void *p_cr_117;
} ikc_out_mono1_8_mctf_t;

uint32 ikc_mono1_8_mctf(const ikc_in_mono1_8_mctf_t *p_in_mono1_8_mctf, ikc_out_mono1_8_mctf_t *p_out_mono1_8_mctf);


//////////////////// mono3
typedef struct {
    ik_fusion_t *p_fusion;
} ikc_in_mono3_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_27_size;
    void *p_cr_27;
} ikc_out_mono3_t;

uint32 ikc_mono3(const ikc_in_mono3_t *p_in_mono3, ikc_out_mono3_t *p_out_mono3);

//////////////////// mono5
typedef struct {
    ik_fusion_t *p_fusion;
} ikc_in_mono5_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_27_size;
    void *p_cr_27;
} ikc_out_mono5_t;

uint32 ikc_mono5(const ikc_in_mono5_t *p_in_mono5, ikc_out_mono5_t *p_out_mono5);

//////////////////// mono7
typedef struct {
    ik_fusion_t *p_fusion;
} ikc_in_mono7_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_27_size;
    void *p_cr_27;
} ikc_out_mono7_t;

uint32 ikc_mono7(const ikc_in_mono7_t *p_in_mono7, ikc_out_mono7_t *p_out_mono7);

typedef struct {
} ikc_in_motion_a_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_29_size;
    void *p_cr_29;
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_31_size;
    void *p_cr_31;
    uint32 cr_32_size;
    void *p_cr_32;
    uint32 cr_36_size;
    void *p_cr_36;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_113_size;
    void *p_cr_113;
    uint32 cr_115_size;
    void *p_cr_115;
    uint32 cr_117_size;
    void *p_cr_117;
    uint32 cr_119_size;
    void *p_cr_119;
} ikc_out_motion_a_default_tbl_t;

uint32 ikc_motion_a_default_tbl(const ikc_in_motion_a_default_tbl_t *p_in_motion_a_default_tbl, const ikc_out_motion_a_default_tbl_t *p_out_motion_a_default_tbl);

typedef struct {
} ikc_in_motion_b_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_29_size;
    void *p_cr_29;
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_31_size;
    void *p_cr_31;
    uint32 cr_32_size;
    void *p_cr_32;
    uint32 cr_36_size;
    void *p_cr_36;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_113_size;
    void *p_cr_113;
    uint32 cr_115_size;
    void *p_cr_115;
    uint32 cr_117_size;
    void *p_cr_117;
    uint32 cr_119_size;
    void *p_cr_119;
} ikc_out_motion_b_default_tbl_t;

uint32 ikc_motion_b_default_tbl(const ikc_in_motion_b_default_tbl_t *p_in_motion_b_default_tbl, const ikc_out_motion_b_default_tbl_t *p_out_motion_b_default_tbl);

typedef struct {
} ikc_in_motion_c_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
    uint32 cr_29_size;
    void *p_cr_29;
    uint32 cr_30_size;
    void *p_cr_30;
    uint32 cr_31_size;
    void *p_cr_31;
    uint32 cr_32_size;
    void *p_cr_32;
    uint32 cr_36_size;
    void *p_cr_36;
    uint32 cr_112_size;
    void *p_cr_112;
    uint32 cr_113_size;
    void *p_cr_113;
    uint32 cr_115_size;
    void *p_cr_115;
    uint32 cr_117_size;
    void *p_cr_117;
    uint32 cr_119_size;
    void *p_cr_119;
} ikc_out_motion_c_default_tbl_t;

uint32 ikc_motion_c_default_tbl(const ikc_in_motion_c_default_tbl_t *p_in_motion_c_default_tbl, const ikc_out_motion_c_default_tbl_t *p_out_motion_c_default_tbl);

typedef struct {
} ikc_in_mono1_8_default_tbl_t;

typedef struct {
    uint32 cr_112_size;
    void *p_cr_112;
} ikc_out_mono1_8_default_tbl_t;

uint32 ikc_mono1_8_default_tbl(const ikc_in_mono1_8_default_tbl_t *p_in_mono1_8_default_tbl, const ikc_out_mono1_8_default_tbl_t *p_out_mono1_8_default_tbl);

typedef struct {
} ikc_in_mono3_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_mono3_default_tbl_t;

uint32 ikc_mono3_default_tbl(const ikc_in_mono3_default_tbl_t *p_in_mono3_default_tbl, const ikc_out_mono3_default_tbl_t *p_out_mono3_default_tbl);

typedef struct {
} ikc_in_mono4_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_mono4_default_tbl_t;

uint32 ikc_mono4_default_tbl(const ikc_in_mono4_default_tbl_t *p_in_mono4_default_tbl, const ikc_out_mono4_default_tbl_t *p_out_mono4_default_tbl);

typedef struct {
} ikc_in_mono5_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_mono5_default_tbl_t;

uint32 ikc_mono5_default_tbl(const ikc_in_mono5_default_tbl_t *p_in_mono5_default_tbl, const ikc_out_mono5_default_tbl_t *p_out_mono5_default_tbl);

typedef struct {
} ikc_in_mono6_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_mono6_default_tbl_t;

uint32 ikc_mono6_default_tbl(const ikc_in_mono6_default_tbl_t *p_in_mono6_default_tbl, const ikc_out_mono6_default_tbl_t *p_out_mono6_default_tbl);

typedef struct {
} ikc_in_mono7_default_tbl_t;

typedef struct {
    uint32 cr_26_size;
    void *p_cr_26;
} ikc_out_mono7_default_tbl_t;

uint32 ikc_mono7_default_tbl(const ikc_in_mono7_default_tbl_t *p_in_mono7_default_tbl, const ikc_out_mono7_default_tbl_t *p_out_mono7_default_tbl);


#endif

