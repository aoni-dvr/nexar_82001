/* CV2L DSP API header file - idsp
Revision: 289982
Last Changed Rev: 288459
Last Changed Date: 2020-03-11 20:37:25 -0700 (Wed, 11 Mar 2020)
ISO_CFG_REV:  1
*/
/**
 * @file CMD_MSG_CV2_H_
 *
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef CMD_MSG_CV2_H_
#define CMD_MSG_CV2_H_

#include <dsp_types.h>

#define ISO_CFG_REV

/************************************************************
 * IDSP commands (Category 9)
 */

typedef unsigned char       u8; /**< UNSIGNED 8-bit data type */
typedef unsigned short     u16; /**< UNSIGNED 16-bit data type */
typedef unsigned int       u32; /**< UNSIGNED 32-bit data type */
typedef signed char         s8; /**< SIGNED 8-bit data type */
typedef signed short       s16; /**< SIGNED 16-bit data type */
typedef signed int         s32; /**< SIGNED 32-bit data type */

/**
 * Ser Vin Capture windows (ARCH_VER >= 3) (0x09001001)
 */
typedef struct
{
  uint16_t s_ctrl_reg;
  uint16_t s_inp_cfg_reg;
  uint16_t reserved;
  uint16_t s_v_width_reg;
  uint16_t s_h_width_reg;
  uint16_t s_h_offset_top_reg;
  uint16_t s_h_offset_bot_reg;
  uint16_t s_v_reg;
  uint16_t s_h_reg;
  uint16_t s_min_v_reg;
  uint16_t s_min_h_reg;
  uint16_t s_trigger_0_start_reg;
  uint16_t s_trigger_0_end_reg;
  uint16_t s_trigger_1_start_reg;
  uint16_t s_trigger_1_end_reg;
  uint16_t s_vout_start_0_reg;
  uint16_t s_vout_start_1_reg;
  uint16_t s_cap_start_v_reg;
  uint16_t s_cap_start_h_reg;
  uint16_t s_cap_end_v_reg;
  uint16_t s_cap_end_h_reg;
  uint16_t s_blank_leng_h_reg;
  uint32_t vsync_timeout;
  uint32_t hsync_timeout;

} vin_common_t;

typedef struct
{
  uint32_t cmd_code;
  uint16_t s_ctrl_reg;
  uint16_t s_inp_cfg_reg;
  uint16_t s_bit_precision;
  uint16_t s_v_width_reg;
  uint16_t s_h_width_reg;
  uint16_t s_h_offset_top_reg;
  uint16_t s_h_offset_bot_reg;
  uint16_t s_v_reg;
  uint16_t s_h_reg;
  uint16_t s_min_v_reg;
  uint16_t s_min_h_reg;
  uint16_t s_trigger_0_start_reg;
  uint16_t s_trigger_0_end_reg;
  uint16_t s_trigger_1_start_reg;
  uint16_t s_trigger_1_end_reg;
  uint16_t s_vout_start_0_reg;
  uint16_t s_vout_start_1_reg;
  uint16_t s_cap_start_v_reg;
  uint16_t s_cap_start_h_reg;
  uint16_t s_cap_end_v_reg;
  uint16_t s_cap_end_h_reg;
  uint16_t s_blank_leng_h_reg;
  uint32_t vsync_timeout;
  uint32_t hsync_timeout;

} vin_cap_win_t;

typedef vin_cap_win_t VIN_SETUP_CMD;

/**
 * Sensor Input setup (0x09001002)
 */
typedef struct
{
  uint32_t cmd_code;
  uint8_t  sensor_id;
  uint8_t  field_format;
  uint8_t  sensor_resolution;
  uint8_t  sensor_pattern;
  uint8_t  first_line_field_0;
  uint8_t  first_line_field_1;
  uint8_t  first_line_field_2;
  uint8_t  first_line_field_3;
  uint8_t  first_line_field_4;
  uint8_t  first_line_field_5;
  uint8_t  first_line_field_6;
  uint8_t  first_line_field_7;
  uint32_t sensor_readout_mode;

} sensor_input_setup_t;

/**
 * Set VIN Configuration (0x09001008)
 */
typedef struct
{
  uint32_t cmd_code;
  uint16_t vin_width;
  uint16_t vin_height;
  uint32_t vin_config_dram_addr;
  uint16_t config_data_size;
  uint8_t  sensor_resolution;
  uint8_t  sensor_bayer_pattern;
  uint32_t skip_vin_config_dram_addr;
  uint16_t skip_vin_config_data_size;
  uint16_t reserved;

} set_vin_config_t;

typedef struct
{
  uint32_t cmd_code;
  uint8_t  decomp_en;
  uint8_t  comp_en;
  uint8_t  cfa_4x_en;
  uint8_t  reserved;
  uint32_t cfa_4x_coeff_shift;
  uint32_t cfa_4x_hout_width;
  uint32_t decomp_lookup_table_daddr;
  uint32_t comp_lookup_table_daddr;
  uint32_t cfa_4x_coeff_table_daddr;

} set_hdsp_config_t;

/**
 * Set Master clock Configuration (0x0009100E)
 */
typedef struct
{
  u32 cmd_code;
  u8  main_or_pip;          /* 0: main; 1: pip */
  u8  reserved[3];
  u16 master_sync_reg_word0;
  u16 master_sync_reg_word1;
  u16 master_sync_reg_word2;
  u16 master_sync_reg_word3;
  u16 master_sync_reg_word4;
  u16 master_sync_reg_word5;
  u16 master_sync_reg_word6;
  u16 master_sync_reg_word7;

} set_vin_master_t;

/**
 * Set Global Vin Configuration (0x0009100F)
 */
typedef struct
{
  u32 cmd_code;
  u8  main_or_pip;          /* 0: main; 1: pip; 2: pip2; 3: pip3; 4: pip4; 5: pip5 */
  u8  reserved[3];
  u32 global_cfg_reg_word0; /* word0 of the register values */

} set_vin_global_config_t;

/**
 * AAA statistics setup (0x09004001)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t on                   : 8;
  uint32_t auto_shift           : 8;
  uint32_t data_fifo_no_reset   : 1;
  uint32_t data_fifo2_no_reset  : 1;
  uint32_t six_minus_cfa_input_shift : 3;
  uint32_t reserved : 11;
  uint32_t data_fifo_base;
  uint32_t data_fifo_limit;
  uint32_t data_fifo2_base;
  uint32_t data_fifo2_limit;
  uint16_t awb_tile_num_col;
  uint16_t awb_tile_num_row;
  uint16_t awb_tile_col_start;
  uint16_t awb_tile_row_start;
  uint16_t awb_tile_width;
  uint16_t awb_tile_height;
  uint16_t awb_tile_active_width;
  uint16_t awb_tile_active_height;
  uint16_t awb_pix_min_value;
  uint16_t awb_pix_max_value;
  uint16_t ae_tile_num_col;
  uint16_t ae_tile_num_row;
  uint16_t ae_tile_col_start;
  uint16_t ae_tile_row_start;
  uint16_t ae_tile_width;
  uint16_t ae_tile_height;
  uint16_t ae_pix_min_value;
  uint16_t ae_pix_max_value;
  uint16_t af_tile_num_col;
  uint16_t af_tile_num_row;
  uint16_t af_tile_col_start;
  uint16_t af_tile_row_start;
  uint16_t af_tile_width;
  uint16_t af_tile_height;
  uint16_t af_tile_active_width;
  uint16_t af_tile_active_height;

} aaa_statistics_setup_t;

/**
 * AAA pseudo Y setup (0x09004002)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t mode;
  uint32_t sum_shift;
  uint8_t  pixel_weight[4];
  uint8_t  tone_curve[32];

} aaa_pseudo_y_t;

#define AAA_FILTER_SELECT_BOTH 0
#define AAA_FILTER_SELECT_CFA  1
#define AAA_FILTER_SELECT_YUV  2

/**
 * AAA histogram setup (0x09004003)
 */
typedef struct
{
  uint32_t cmd_code;
  uint16_t mode;
  uint16_t histogram_select;
  uint16_t ae_tile_mask[8];

} aaa_histogram_t;

/**
 * AAA statistics setup 1 (0x09004004)
 */

typedef struct
{
  uint32_t cmd_code;
  uint32_t af_horizontal_filter1_mode       : 4;
  uint32_t af_filter1_select                : 4;
  uint32_t af_horizontal_filter1_stage1_enb : 8;
  uint32_t af_horizontal_filter1_stage2_enb : 8;
  uint32_t af_horizontal_filter1_stage3_enb : 8;
  uint16_t af_horizontal_filter1_gain[7];
  uint16_t af_horizontal_filter1_shift[4];
  uint16_t af_horizontal_filter1_bias_off;
  uint16_t af_horizontal_filter1_thresh;
  uint16_t af_vertical_filter1_thresh;
  uint16_t af_tile_fv1_horizontal_shift;
  uint16_t af_tile_fv1_vertical_shift;
  uint16_t af_tile_fv1_horizontal_weight;
  uint16_t af_tile_fv1_vertical_weight;

} aaa_statistics_setup1_t;

/**
 * AAA statistics setup 2 (0x09004005)
 */
 typedef struct
{
  uint32_t cmd_code;
  uint32_t af_horizontal_filter2_mode       : 4;
  uint32_t af_filter2_select                : 4;
  uint32_t af_horizontal_filter2_stage1_enb : 8;
  uint32_t af_horizontal_filter2_stage2_enb : 8;
  uint32_t af_horizontal_filter2_stage3_enb : 8;
  uint16_t af_horizontal_filter2_gain[7];
  uint16_t af_horizontal_filter2_shift[4];
  uint16_t af_horizontal_filter2_bias_off;
  uint16_t af_horizontal_filter2_thresh;
  uint16_t af_vertical_filter2_thresh;
  uint16_t af_tile_fv2_horizontal_shift;
  uint16_t af_tile_fv2_vertical_shift;
  uint16_t af_tile_fv2_horizontal_weight;
  uint16_t af_tile_fv2_vertical_weight;

} aaa_statistics_setup2_t;

/**
 * AAA statistics setup 3 (0x09004006)
 */
typedef struct
{
  uint32_t cmd_code;
  uint16_t awb_tile_rgb_shift;
  uint16_t awb_tile_y_shift;
  uint16_t awb_tile_min_max_shift;
  uint16_t ae_tile_y_shift;
  uint16_t ae_tile_linear_y_shift;
  uint16_t af_tile_cfa_y_shift;
  uint16_t af_tile_y_shift;

} aaa_statistics_setup3_t;

/*
 * AAA early WB gain (0x09004007)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t red_multiplier;
  uint32_t green_multiplier_even;
  uint32_t green_multiplier_odd;
  uint32_t blue_multiplier;
  uint8_t  enable_ae_wb_gain;
  uint8_t  enable_af_wb_gain;
  uint8_t  enable_histogram_wb_gain;
  uint8_t  reserved;
  uint32_t red_wb_multiplier;
  uint32_t green_wb_multiplier_even;
  uint32_t green_wb_multiplier_odd;
  uint32_t blue_wb_multiplier;

} aaa_early_wb_gain_t;

/**
 * VIN_STAT(main or HDR) (0x09004010)
 */
typedef struct
{
  uint32_t cmd_code;

  uint8_t  reserved_0[4];

  uint8_t  vin_stats_main_on;
  uint8_t  vin_stats_hdr_on;
  uint8_t  total_exposures;
  uint8_t  total_slice_in_x;

  uint32_t main_data_fifo_base;
  uint32_t main_data_fifo_limit;
  uint32_t hdr_data_fifo_base;
  uint32_t hdr_data_fifo_limit;

} vin_stats_setup_t;

/* VIN_STAT header */
typedef struct
{
  uint32_t vin_stats_type   : 8; /* 0: main; 1: hdr; 2: hdr2 */
  uint32_t chan_index       : 4;
  uint32_t is_rgbir         : 1;
  uint32_t reserved_0       : 3;
  uint32_t total_exposures  : 8;
  uint32_t blend_index      : 8; /* exposure no. */

  uint8_t  total_slice_in_x;
  uint8_t  slice_index_x;
  uint8_t  total_slice_in_y;
  uint8_t  slice_index_y;

  uint16_t vin_stats_slice_left;
  uint16_t vin_stats_slice_width;

  uint16_t vin_stats_slice_top;
  uint16_t vin_stats_slice_height;

  /* uint32_t reserved[28]; ** pad to 128B */

} vin_stats_tile_config_t;

/*
 * AAA tile configuration (internal use only)
 */
typedef struct
{
  /* AWB */
  uint16_t awb_tile_col_start;
  uint16_t awb_tile_row_start;
  uint16_t awb_tile_width;
  uint16_t awb_tile_height;
  uint16_t awb_tile_active_width;
  uint16_t awb_tile_active_height;
  uint16_t awb_rgb_shift;
  uint16_t awb_y_shift;
  uint16_t awb_min_max_shift;
  /* AE */
  uint16_t ae_tile_col_start;
  uint16_t ae_tile_row_start;
  uint16_t ae_tile_width;
  uint16_t ae_tile_height;
  uint16_t ae_y_shift;
  uint16_t ae_linear_y_shift;
  uint16_t ae_min_max_shift;
  /* AF */
  uint16_t af_tile_col_start;
  uint16_t af_tile_row_start;
  uint16_t af_tile_width;
  uint16_t af_tile_height;
  uint16_t af_tile_active_width;
  uint16_t af_tile_active_height;
  uint16_t af_y_shift;
  uint16_t af_cfa_y_shift;

  /* AWB tiles */
  uint8_t  awb_tile_num_col;
  uint8_t  awb_tile_num_row;

  /* AE tiles */
  uint8_t  ae_tile_num_col;
  uint8_t  ae_tile_num_row;

  /* AF tiles */
  uint8_t  af_tile_num_col;
  uint8_t  af_tile_num_row;

  /* Strip info */
  uint8_t  total_slices_x;
  uint8_t  total_slices_y;

  uint8_t  slice_index_x;
  uint8_t  slice_index_y;
  uint16_t slice_width;

  uint16_t slice_height;
  uint16_t slice_start_x;

  uint32_t slice_start_y : 16;
  uint32_t chan_index    : 4;
  uint32_t reserved      : 12;

  uint32_t raw_pic_seq_num;  /* sequence number of raw picture used in producing this block of AAA results */
  uint32_t iso_config_tag;

} aaa_tile_config_t;

/**
 * Lossy compression (0x09005003)
 */
typedef struct
{
  uint32_t cmd_code;
  uint8_t  enable;
  uint8_t  mantissa;
  uint8_t  block_size;
  uint8_t  reserved;

} lossy_compression_t;

/*
 * Set Warp Control
 */
typedef struct
{
#define WARP_CONTROL_DISABLE    0
#define WARP_CONTROL_ENABLE     1
  uint32_t cmd_code;
  uint32_t warp_control;
  uint32_t warp_horizontal_table_address;
  uint32_t warp_vertical_table_address;
  uint32_t actual_left_top_x;
  uint32_t actual_left_top_y;
  uint32_t actual_right_bot_x;
  uint32_t actual_right_bot_y;
  uint32_t zoom_x;
  uint32_t zoom_y;
  uint16_t main_out_x;
  uint16_t main_out_y;
  uint16_t max_hwarp_drag;
  uint16_t reserved_0;

  uint32_t grid_array_width           : 12;
  uint32_t horz_grid_spacing_exponent : 4;
  uint32_t grid_array_height          : 12;
  uint32_t vert_grid_spacing_exponent : 4;

  uint32_t vert_warp_grid_array_width           : 12;
  uint32_t vert_warp_horz_grid_spacing_exponent : 4;
  uint32_t vert_warp_grid_array_height          : 12;
  uint32_t vert_warp_vert_grid_spacing_exponent : 4;

  uint32_t vert_warp_enable    : 8;
  uint32_t force_v4tap_disable : 8;
  uint32_t srcbuf_id           : 5; /* used if multi_chan_style == 1 */
  uint32_t resolution_type     : 2; /* 0: main, 1: me1 */
  uint32_t reserved_1          : 1;
  uint32_t chan_id             : 4;
  uint32_t reserved_2          : 4;
  uint32_t vertical_slice_proc_cfg_addr;

  /*
    This one is used for ARM to calcuate the
    dummy window for Ucode, these fields should be
    zero for turbo command in case of EIS. could be
    non-zero valid value only when this warp command is send
    in non-turbo command way.
  */

  uint16_t dummy_window_x_left;
  uint16_t dummy_window_y_top;
  uint16_t dummy_window_width;
  uint16_t dummy_window_height;

  /*
    This field is used for ARM to calculate the
    cfa prescaler zoom factor which will affect
    the warping table value. this should also be zeor
    during the turbo command sending.Only valid on the
    non-turbo command time.
  */
  uint16_t cfa_output_width;
  uint16_t cfa_output_height;
  uint32_t extra_sec2_vert_out_vid_mode;

#ifdef ISO_CFG_REV
  uint16_t sec2_output_height;
  uint16_t sec2_vscale_phase_inc;
  uint32_t sec3_vscale_init_phase;
  uint16_t sec3_vscale_phase_inc;
  uint16_t reserved_3;
  uint32_t reserved_4;
  uint32_t reserved_5;
#endif
} set_warp_control_t;

/*
 *  Set Chromatic Aberration Warp Control (0x09005009)
 *   */
typedef struct  {
  uint32_t cmd_code;
  uint16_t horz_warp_enable;
  uint16_t vert_warp_enable;

  uint8_t  horz_pass_grid_array_width;
  uint8_t  horz_pass_grid_array_height;
  uint8_t  horz_pass_horz_grid_spacing_exponent;
  uint8_t  horz_pass_vert_grid_spacing_exponent;
  uint8_t  vert_pass_grid_array_width;
  uint8_t  vert_pass_grid_array_height;
  uint8_t  vert_pass_horz_grid_spacing_exponent;
  uint8_t  vert_pass_vert_grid_spacing_exponent;

  uint32_t warp_horizontal_table_addr_red;
  uint32_t warp_horizontal_table_addr_blue;
  uint32_t warp_vertical_table_addr_red;
  uint32_t warp_vertical_table_addr_blue;

} set_chromatic_aberration_warp_control_t;

/*
 *  Propcess IDSP Command Batch (0x09006001)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t group_index;
  uint32_t idsp_cmd_buf_addr;
  uint32_t cmd_buf_size;

} process_idsp_batch_cmd_t;

/*
* PROCESS_ISO_CONFIG_CMD (0x09006002)
*/
typedef struct
{
  uint32_t iso_config_common_update : 1;
  uint32_t iso_config_color_update  : 1;
  uint32_t iso_config_mctf_update   : 1;
  uint32_t iso_config_step1_update  : 1;
  uint32_t iso_config_aaa_update    : 1; /* AAA setup update */
  uint32_t iso_config_warp_update   : 1;
  uint32_t reserved                 : 26;

} VCAP_ISO_CONFIG_UPDATE;

typedef struct
{
  uint32_t               cmd_code;
  uint32_t               iso_cfg_daddr;
  VCAP_ISO_CONFIG_UPDATE flag;
  uint32_t               sequence_id;

} process_idsp_iso_config_cmd_t;

/*
 * Send IDSP debug command (0x09006003)
 */
typedef struct
{
  uint32_t cmd_code;
  uint32_t mode;
  uint32_t param1;
  uint32_t param2;
  uint32_t param3;
  uint32_t param4;
  uint32_t param5;
  uint32_t param6;
  uint32_t param7;
  uint32_t param8;

} amb_dsp_debug_3_t;

typedef amb_dsp_debug_3_t send_idsp_debug_cmd_t;


/************************************************************/

//#if defined(DSP_SMALL_INT)
//#undef int8_t
//#endif
/************************************************************/

#endif /* CMD_MSG_CV2_H_ */
