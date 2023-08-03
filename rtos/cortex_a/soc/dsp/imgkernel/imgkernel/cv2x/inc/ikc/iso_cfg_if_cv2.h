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

#ifndef ISO_CFG_IF_CV2_H
#define ISO_CFG_IF_CV2_H

#include "cmd_msg_cv2.h"
#include "cmd_msg_dsp.h"
#include "cmd_msg_prod_dv.h"
#include "ik_data_type.h"

typedef struct {
    uint32_t iso_config_tag;

    uint8_t  iso_mode; // 0: LISO, 1: HISO
    uint8_t  adv_mode; // LISO 0: 1-pass, 1: 3-pass ALISO, 2: 2-pass BLISO, 3: 2-pass+TA BLISO, 4: CLISO
    uint8_t  diag_case_id;
    uint8_t  channel_id;

    uint32_t sequence_id;

    // padding to 16 bytes
    uint32_t reserved;
} iso_meta_header_t;

typedef struct {
    uint8_t data[8];
    uint32_t warp_horizontal_table_address;
    uint32_t warp_vertical_table_address;
    uint8_t data2[44];
    uint32_t vertical_slice_proc_cfg_addr;
    uint8_t data1[36];
} iso_warp_cmd_data_t;

typedef struct {
    uint8_t data[16];
    uint32_t warp_horizontal_table_addr_red;
    uint32_t warp_horizontal_table_addr_blue;
    uint32_t warp_vertical_table_addr_red;
    uint32_t warp_vertical_table_addr_blue;
} iso_cawarp_cmd_data_t;

typedef struct {
    uint32_t reserved                           :19;
    uint32_t is_eis_warp_ready                  :1;
    uint32_t reserved2                          :12;
    uint32_t vignette_gain;
    iso_cawarp_cmd_data_t ca_warp_control;
    iso_warp_cmd_data_t geo_warp_control; // for horz warp, vert warp and DZ
    uint8_t data1[32];
} iso_calib_data_t;

typedef struct {
    uint8_t data[24];
    uint32_t gain_curver_addr;
} iso_chroma_scale_t;

typedef struct {
//    uint8_t data[168];
    uint8_t data[60];
    uint32_t place_holder[5];
    iso_chroma_scale_t chroma_scale;
    uint32_t cc_reg;
    uint32_t cc_3d;
    uint32_t cc_out;
    uint8_t data2[48];
} iso_color_data_t;

typedef struct {
    uint32_t fir1;
    uint32_t fir2;
    uint32_t coring;
    uint8_t data[24];
} iso_asf_data_t;

typedef struct {
    uint32_t fir1;
    uint32_t fir2;
    uint32_t coring;
    uint8_t data[52];
} iso_sharpen_data_t;

typedef struct {
    // 168
    uint32_t dark_pixel_threshold;
    uint32_t hot_pixel_threshold;
    uint32_t chroma_median_k_table;
    uint8_t data[60];
    //shp
#if 0 //Original Code
    union {
        iso_asf_data_t asf;
        iso_sharpen_data_t shp;
    } shp_a;
#else //To fix coverity
    iso_sharpen_data_t  shp_a_shp;
#endif
    uint8_t data1[24];
    uint32_t batch_cmd_queue_daddr;
    uint8_t data2[4];
} iso_raw2yuv_data_t;

typedef struct {
//    uint8_t data[40];
    uint8_t data[12];
    uint32_t video_mctf;
    uint32_t final_sharpen;
    uint32_t temporal_adjust_edge_reduce;
    uint32_t reserved2;
    uint32_t ta_edge_reduce_update   : 1;
    uint32_t mctf_10bit_luma_ref          : 1;
    uint32_t mctf_compress_dither_enable  : 1;
    uint32_t mctf_compress_enable         : 1;
    uint32_t mctf_compress_bitrate_luma   : 6;
    uint32_t mctf_compress_bitrate_chroma : 6;
    uint32_t use_motion_detect            : 1; // runtime update not supported
    uint32_t negative_image               : 1;
    uint32_t disable_video_mctf_local_lighting : 1;
    uint32_t is_fusion_flow               : 1;
    uint32_t is_post_shp_enabled          : 1;
    uint32_t is_pre_nn_proc_enabled       : 1;
    uint32_t report_true_vin_stats        : 1;
    uint32_t is_thermal_freq_combine      : 1;
    uint32_t mctf_compress_log_vb_size_minus4 : 5;
    uint32_t reserved                     : 3;
    uint8_t data1[8];
} iso_flow_data_t;

typedef struct {
//    uint8_t data[128];
    uint8_t data2[4];
    uint32_t exp0_decompand_table_addr;
    uint32_t hdr_blend_t0_offset;
    uint32_t hdr_blend_t1_offset;
    uint16_t hdr_blend_flicker_threshold;
    uint8_t  hdr_blend_delta_t0;
    uint8_t  hdr_blend_delta_t1;
    uint8_t data[4];
    uint32_t reserved :28; //contrast_enhance.fir_coeff[3]
    uint32_t enable_before_ce_gain :1;
    uint32_t update_before_ce_gain :1;
    uint32_t enable_decompand :1;
    uint32_t update_hdr_blend :1;
    uint32_t ce_input_table_addr; // contrast_enhance.input_table
    uint32_t ce_coring_table_addr; //contrast_enhance.coring_table
    uint32_t ce_boost_table_addr; // contrast_enhance.boost_table
    uint32_t ce_output_table_addr; //contrast_enhance.output_table_path
    uint32_t ce_epsilon_table_addr; //contrast_enhance.epsilon
    uint8_t data3[16];
    uint32_t before_ce_digital_gain[4]; // 12-bit mulitplier & 5-bit shift in 12.12 format
    uint8_t data1[48];
} iso_extra_data_t;

typedef struct {
    //[32];
    uint8_t data[4];
    uint32_t smooth_coring;     // canned
    uint8_t data1[4];
    // Pass 2-4 Motion Detect
    uint32_t md_mctf;       // canned
    uint32_t MoASF_fir1;    // canned
    uint32_t MoASF_fir2;    // canned
    uint32_t MoASF_coring;  // canned
    uint8_t data2[4];
} iso_motion_detect_data_t;

typedef struct {
    uint32_t data1:16;
    uint32_t raw_start_offset_y:16;
    uint32_t front_end_input_black_level_offset[4]; //static_black_level.r/gr/gb/b_black in s+14-bit format
    uint32_t front_end_digital_gain[4]; // 12-bit mulitplier & 5-bit shift in 12.12 format
    uint32_t front_end_dgain_output_saturation_level[4]; // 20 bits default to 2^20 -1
} iso_single_exposure_data_t;

#if INTERNAL_EARLY_TEST_ENV
typedef struct {
    uint8_t data[12];;
} iso_vin_stat_crop_data_t;
#endif

typedef struct {
    uint32_t cc_reg_alpha1; // step 3
    uint32_t cc_3d_alpha1;
    uint32_t cc_reg_alpha2; // step 5
    uint32_t cc_3d_alpha2;
    uint32_t cc_reg_alpha;  // step 7
    uint32_t cc_3d_alpha;
    uint8_t data[8];
} iso_mono_yuv_t;

typedef struct {
    uint32_t mcts_FusEdge;   // step 3 SHPB
    uint32_t mctf_C;         // step 6 MCTF
    uint32_t mctf_Alpha;     // step 7 MCTF
    uint32_t mcts_Alpha;     // step 7 SHPB
    uint32_t mctf_FreqBlend; // step 8 MCTF
    uint32_t mcts_FreqBlend; // step 8 SHPB
    uint32_t mctf_passthru;
    uint32_t mcts_passthru;
} iso_mono_mctf_t;

typedef struct {
    iso_calib_data_t calib; // 172 bytes
    iso_color_data_t color; // 168 bytes
    iso_raw2yuv_data_t raw2yuv; // 168 bytes
    iso_extra_data_t extra; // 128 bytes
    iso_single_exposure_data_t exp0;
} iso_mono_raw2yuv_t;

typedef struct {
    iso_mono_raw2yuv_t liso;       // step 1 mono raw2yuv
    iso_sharpen_data_t smooth; // step 4 canned sharpening
    iso_mono_yuv_t yuv;            // CC and chroma noise filter
    iso_mono_mctf_t mctf;          // MCTF and SHPB
} iso_mono_param_t;

typedef struct {
    uint8_t data[76];
} iso_nn_param_t;

typedef struct {
    uint32_t pre_cc_gain_enable     :1;
    uint32_t update_pre_cc_gain     :1;
    uint32_t reserved               :30;

    uint32_t pre_cc_gain_r;
    uint32_t pre_cc_gain_g;
    uint32_t pre_cc_gain_b;
} iso_extra2_data_t;

typedef struct {
    iso_meta_header_t share; // 16 bytes
    iso_calib_data_t calib; // 172 bytes
    iso_color_data_t color; // 168 bytes
    iso_raw2yuv_data_t liso; // 168 bytes
    iso_flow_data_t flow; // 40 bytes
    iso_extra_data_t extra; // 128 bytes
    iso_motion_detect_data_t motion; // 32 bytes
    iso_single_exposure_data_t exposure[3]; // 52*3 = 156 bytes
#if INTERNAL_EARLY_TEST_ENV
    iso_vin_stat_crop_data_t vin_stat_crop; // 12 bytes
    union {
        iso_mono_param_t mono; // 816 bytes
        iso_sharpen_data_t post_shp;
        iso_nn_param_t pre_nn; // 68 bytes
    };
#endif
    iso_extra2_data_t extra2; // 16 bytes
} cv2_low_iso_cfg_t; // 896 bytes

typedef struct {
    iso_raw2yuv_data_t raw2yuv_hi;
    iso_extra_data_t extra_hi;
} iso_still_hiso_param01_t;

typedef struct {
    uint32_t cc_reg_edge_score;
    uint32_t cc_3d_edge_score;
} iso_still_hiso_param02_t;

typedef struct {
    iso_asf_data_t asf_chroma;
} iso_still_hiso_param03_t;

typedef struct {
    iso_asf_data_t asf_med1;
    uint8_t data[8];
} iso_still_hiso_param04_t;

typedef struct {
    iso_sharpen_data_t sharpen_med;
} iso_still_hiso_param05_t;

typedef struct {
    uint8_t data[8];
} iso_still_hiso_param06_t;

typedef struct {
    uint8_t data[8];
} iso_still_hiso_param07_t;

typedef struct {
    uint8_t data[8];
} iso_still_hiso_param08_t;

typedef struct {
    uint32_t is_step9_sharpen_high : 1;
    uint32_t reserved              : 31;
#if 0 //Original Code
    union {
        iso_asf_data_t asf_high2;
        iso_sharpen_data_t sharpen_high;
    } shp_a;
#else //To fix coverity
    iso_sharpen_data_t shp_a_sharpen_high;
#endif
} iso_still_hiso_param09_t;

typedef struct {
    iso_raw2yuv_data_t raw2yuv_li;
    iso_extra_data_t extra_li;
} iso_still_hiso_param11_t;

typedef struct {
    iso_raw2yuv_data_t raw2yuv_li2;
    iso_extra_data_t extra_li2;
} iso_still_hiso_param12_t;

typedef struct {
    iso_asf_data_t asf_chroma_li;
} iso_still_hiso_param13_t;

typedef struct {
    uint8_t data[32];
} iso_still_hiso_param_flow_t;

typedef struct {
    uint32_t mcts_Edge_Detect;  // step 1 SHPB
    uint32_t mcts_Edge_ASF;     // step 2 SHPB
    uint32_t mcts_L_High_ASF;   // step 3 SHPB
    uint32_t mcts_L_Low_ASF;    // step 4 SHPB
    uint32_t mctf_Low_ASF_Combine; // step 5 MCTF
    uint32_t mcts_Med2_ASF;     // step 5 SHPB
    uint32_t mctf_C_Eight; // step 6 MCTF
    uint32_t mctf_C_4; // step 7 MCTF
    uint32_t mctf_C_B; // step 8 MCTF
    uint32_t mctf_L_Mix2; // step 9 MCTF
    uint32_t mcts_L_Mix2; // step 9 SHPB
    uint32_t mctf_L_Luma_Noise_Combine; // step 10 MCTF
    uint32_t mctf_LI_HI_Combine; // step 11 MCTF
    uint32_t mcts_LI_HI_Combine; // step 11 SHPB
    uint32_t mctf_LI2_HI_Combine; // step 12 MCTF
    uint32_t mcts_LI2_HI_Combine; // step 12 SHPB
    uint32_t mctf_C_A; // step 13 MCTF
    uint32_t mctf_disable;
    uint32_t mcts_disable; // step 6,7,8,13: 422
    uint32_t mcts_disable_420; // step 10
} iso_still_hiso_param_mctf_t;

typedef struct {
    uint8_t data[72];
} iso_liso_param_vwarp_t;

typedef struct {
    iso_meta_header_t share;
    iso_calib_data_t common;
    iso_color_data_t color;
    iso_still_hiso_param01_t step01;
    iso_still_hiso_param02_t step02;
    iso_still_hiso_param03_t step03;
    iso_still_hiso_param04_t step04;
    iso_still_hiso_param05_t step05;
    iso_still_hiso_param06_t step06;
    iso_still_hiso_param07_t step07;
    iso_still_hiso_param08_t step08;
    iso_still_hiso_param09_t step09;
    iso_still_hiso_param11_t step11;
    iso_still_hiso_param12_t step12;
    iso_still_hiso_param13_t step13;
    iso_still_hiso_param_flow_t flow;
    iso_still_hiso_param_mctf_t mctf;
    iso_liso_param_vwarp_t vwarp;
    iso_single_exposure_data_t exposure[3];
} still_high_iso_cfg_t;

#endif //ISO_CFG_IF_CV2_H
