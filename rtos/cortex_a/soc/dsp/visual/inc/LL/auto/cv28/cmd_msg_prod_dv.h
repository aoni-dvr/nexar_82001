/* CV2L DSP API header file - product specific
Revision: 289982
Last Changed Rev: 289981
Last Changed Date: 2022-12-07 01:32:12 -0800 (Wed, 07 Dec 2022)
*/
/*
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
/// cmd_msg header file for PRODUCT_DV
#ifndef CMD_MSG_PROD_DV_H_
#define CMD_MSG_PROD_DV_H_

#include <dsp_types.h>


///////////////////////////////////////////////////////////////////////////////
// Command Code
///////////////////////////////////////////////////////////////////////////////

typedef uint32_t dsp_cmd_code_t;

// CAT_DSP_HEADER(0),  this is a special one, must be the first command
#define CMD_DSP_HEADER                          0x000000ABu
#define CMD_DSP_ENDER                           0x000000ACu

// CAT_DSP_CFG(1)
#define CMD_DSP_CONFIG                          0x01000001u
#define CMD_DSP_HAL_INF                         0x01000002u
#define CMD_DSP_SUSPEND_PROFILE                 0x01000003u
#define CMD_DSP_SET_DEBUG_LEVEL                 0x01000004u
#define CMD_DSP_PRINT_THREAD_DISABLE_MASK       0x01000005u
#define CMD_DSP_BINDING_CFG                     0x01000006u
#define CMD_DSP_ACTIVATE_PROFILE                0x01000007u
#define CMD_DSP_VPROC_FLOW_MAX_CFG              0x01000008u
#define CMD_DSP_VIN_FLOW_MAX_CFG                0x01000009u
#define CMD_DSP_ENC_FLOW_MAX_CFG                0x0100000Au
#define CMD_DSP_SET_PROFILE                     0x0100000Bu
#define CMD_DSP_DEC_FLOW_MAX_CFG                0x0100000Cu
#define CMD_DSP_NOP                             0x0100000Du

#define CMD_DSP_UNITTEST0                       0x01000010u
#define CMD_DSP_UNITTEST1                       0x01000011u
#define CMD_DSP_UNITTEST2                       0x01000012u
#define CMD_DSP_VIN_UNITTEST                    0x01000013u
#define CMD_DSP_UNITTEST3                       0x01000014u
#define CMD_DSP_VIN_UNITTEST2                   0x01000015u
#define CMD_DSP_VOUTB_UNITTEST                  0x01000016u

// CAT_VPROC(2)
#define CMD_VPROC_CONFIG                        0x02000001u
#define CMD_VPROC_SETUP                         0x02000002u
#define CMD_VPROC_MCTF_CONFIG                   0x02000003u
#define CMD_VPROC_IMG_PRMD_SETUP                0x02000004u
#define CMD_VPROC_LN_DET_SETUP                  0x02000005u
#define CMD_VPROC_PREV_SETUP                    0x02000006u
#define CMD_VPROC_SET_EXT_MEM                   0x02000007u
#define CMD_VPROC_GRP_CMD                       0x02000008u
#define CMD_VPROC_STOP                          0x02000009u
#define CMD_VPROC_OSD_BLEND                     0x0200000Au
#define CMD_VPROC_PIN_OUT_DECIMATION            0x0200000Bu
#define CMD_VPROC_ECHO                          0x0200000Cu
#define CMD_VPROC_OSD_INSERT                    0x0200000Du
#define CMD_VPROC_SET_STREAMS_DELAY             0x0200000Eu

// VPROC common commands
#define CMD_VPROC_MULTI_STREAM_PP               0x02000064u
#define CMD_VPROC_SET_EFFECT_BUF_IMG_SZ         0x02000065u
#define CMD_VPROC_MULTI_CHAN_PROC_ORDER         0x02000066u
#define CMD_VPROC_SET_VPROC_GRPING              0x02000067u

// CAT_VIN(4)
#define CMD_VIN_START                           0x04000001u
#define CMD_VIN_IDLE                            0x04000002u
#define CMD_VIN_SET_RAW_FRM_CAP_CNT             0x04000003u
#define CMD_VIN_SET_EXT_MEM                     0x04000004u
#define CMD_VIN_SEND_INPUT_DATA                 0x04000005u
#define CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF     0x04000006u
#define CMD_VIN_ATTACH_EVENT_TO_RAW             0x04000007u
#define CMD_VIN_CMD_MSG_DECIMATION_RATE         0x04000008u
#define CMD_VIN_CE_SETUP                        0x04000009u
#define CMD_VIN_HDR_SETUP                       0x0400000Au
#define CMD_VIN_SET_FRM_LVL_FLIP_ROT_CONTROL    0x0400000Bu
#define CMD_VIN_SET_FOV_LAYOUT                  0x0400000Cu
#define CMD_VIN_SET_FRM_VPROC_DELAY             0x0400000Du
#define CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM      0x0400000Eu
#define CMD_VIN_ATTACH_SIDEBAND_INFO_TO_CAP_FRM 0x0400000Fu

// for amalgam diag
#define CMD_VIN_VCAP_DRAM_IN_SETUP              0x04000010u

#define CMD_VIN_VOUT_LOCK_SETUP                 0x04000011u
#define CMD_VIN_ATTACH_METADATA                 0x04000012u
#define CMD_VIN_TEMPORAL_DEMUX_SETUP            0x04000013u

// CAT_VOUT(5)
#define CMD_VOUT_MIXER_SETUP                    0x05000001u
#define CMD_VOUT_VIDEO_SETUP                    0x05000002u
#define CMD_VOUT_DEFAULT_IMG_SETUP              0x05000003u
#define CMD_VOUT_OSD_SETUP                      0x05000004u
#define CMD_VOUT_OSD_BUFFER_SETUP               0x05000005u
#define CMD_VOUT_OSD_CLUT_SETUP                 0x05000006u
#define CMD_VOUT_DISPLAY_SETUP                  0x05000007u
#define CMD_VOUT_DVE_SETUP                      0x05000008u
#define CMD_VOUT_RESET                          0x05000009u
#define CMD_VOUT_DISPLAY_CSC_SETUP              0x0500000Au
#define CMD_VOUT_DIGITAL_OUTPUT_MODE_SETUP      0x0500000Bu
#define CMD_VOUT_GAMMA_SETUP                    0x0500000Cu

// CAT_ENC (6)
#define CMD_ENCODER_SETUP                       0x06000001u
#define CMD_ENCODER_START                       0x06000002u
#define CMD_ENCODER_STOP                        0x06000003u
#define CMD_H264ENC_JPEG_SETUP                  0x06000004u
#define CMD_H264ENC_REALTIME_ENCODE_SETUP       0x06000005u
#define CMD_H264ENC_ENC_FRM_DRAM_HANDSHAKE      0x06000006u
#define CMD_H264ENC_OSD_INSERT                  0x06000007u
#define CMD_H264ENC_VDSP_SYNC_CMD               0x06000008u
#define CMD_ENCODER_UPDATE_FRMRATE_CHANGE       0x06000009u

// CAT_DEC (7)
#define CMD_DECODER_SETUP                       0x07000001u
#define CMD_DECODER_START                       0x07000002u
#define CMD_DECODER_STOP                        0x07000003u
#define CMD_DECODER_BITSFIFO_UPDATE             0x07000004u
#define CMD_DECODER_SPEED                       0x07000005u
#define CMD_DECODER_TRICKPLAY                   0x07000006u
#define CMD_DECODER_STILLDEC                    0x07000007u
#define CMD_DECODER_BATCH                       0x07000008u

// CAT_DPROC (8)
#define CMD_DECPROC_SETUP                       0x08000001u
#define CMD_DECPROC_YUV_TO_YUV                  0x08000002u
#define CMD_DECPROC_YUV_DISPLAY                 0x08000003u
#define CMD_DECPROC_YUV_PAINT                   0x08000004u
#define CMD_DECPROC_YUV_BLEND                   0x08000005u

#define CMD_DECPROC_VRSCL_LAYOUT                0x08000011u
#define CMD_DECPROC_VRSCL_START                 0x08000012u
#define CMD_DECPROC_VRSCL_STOP                  0x08000013u
#define CMD_DECPROC_VRSCL_VID_WIN               0x08000014u
#define CMD_DECPROC_VRSCL_SEND_EXT_PIC          0x08000015u

#define CMD_DECPROC_RESEVERED_FOR_DEC           0x080000F1u

// CAT_IDSP(9)

// -- 0x1001 ~ 0x1FFF   Sensor
// -- 0x2001 ~ 0x2FFF   Color
// -- 0x3001 ~ 0x3FFF   Noiseu
// -- 0x4001 ~ 0x4FFF   3A Statistics
// -- 0x5001 ~ 0x5FFF   Miscellaneous
// -- 0x6001 ~ 0x6FFF   Debug

#define SET_VIN_CAPTURE_WIN                     0x09001001u
#define SET_VIN_CONFIG                          0x09001002u
#define SET_PIP_CAPTURE_WIN                     0x09001003u
#define SET_PIP_CONFIG                          0x09001004u
#define SET_PIP2_CAPTURE_WIN                    0x09001005u
#define SET_PIP2_CONFIG                         0x09001006u
#define SET_PIP3_CAPTURE_WIN                    0x09001007u
#define SET_PIP3_CONFIG                         0x09001008u
#define SET_PIP4_CAPTURE_WIN                    0x09001009u
#define SET_PIP4_CONFIG                         0x0900100Au
#define SET_PIP5_CAPTURE_WIN                    0x0900100Bu
#define SET_PIP5_CONFIG                         0x0900100Cu
#define SET_VIN_SUBPIPELINE                     0x0900100Du
#define SET_PIP_SUBPIPELINE                     0x0900100Eu
#define SET_PIP2_SUBPIPELINE                    0x0900100Fu
#define SET_PIP3_SUBPIPELINE                    0x09001010u
#define SET_PIP4_SUBPIPELINE                    0x09001011u
#define SET_PIP5_SUBPIPELINE                    0x09001012u
#define SET_VIN_MASTER_CLK                      0x09001013u
#define SET_VIN_GLOBAL_CLK                      0x09001014u

#define AAA_STATISTICS_SETUP                    0x09004001u
#define AAA_PSEUDO_Y_SETUP                      0x09004002u
#define AAA_HISTORGRAM_SETUP                    0x09004003u
#define AAA_STATISTICS_SETUP1                   0x09004004u
#define AAA_STATISTICS_SETUP2                   0x09004005u
#define AAA_STATISTICS_SETUP3                   0x09004006u
#define AAA_EARLY_WB_GAIN                       0x09004007u

#define SET_VIN_COMPRESSION                     0x09005001u
#define SET_PIP_COMPRESSION                     0x09005002u
#define SET_PIP2_COMPRESSION                    0x09005003u
#define SET_PIP3_COMPRESSION                    0x09005004u
#define SET_PIP4_COMPRESSION                    0x09005005u
#define SET_PIP5_COMPRESSION                    0x09005006u
#define SET_VIN_HDR_COMPRESSION                 0x09005007u
#define SET_PIP_HDR_COMPRESSION                 0x09005008u
#define SET_PIP2_HDR_COMPRESSION                0x09005009u
#define SET_PIP3_HDR_COMPRESSION                0x0900500Au
#define SET_PIP4_HDR_COMPRESSION                0x0900500Bu
#define SET_PIP5_HDR_COMPRESSION                0x0900500Cu
#define SET_WARP_CONTROL                        0x0900500Du   
#define SET_CHROMATIC_ABERRATION_WARP_CONTROL   0x0900500Eu

#define PROCESS_IDSP_BATCH_CMD                  0x09006001u
#define PROCESS_ISO_CONFIG_CMD                  0x09006002u
#define SEND_IDSP_DEBUG_CMD                     0x09006003u

// CAT_POSTPROC (10)
#define CMD_POSTPROC                            0x0A000001u                

///////////////////////////////////////////////////////////////////////////////
// Message Code
///////////////////////////////////////////////////////////////////////////////

typedef uint32_t dsp_msg_code_t;

#define MSG_DSP_STATUS                          0x81000001u
#define MSG_CMD_REQ                             0x81000002u
#define MSG_ECHO                                0x81000003u

// CAT_VPROC (2)
#define MSG_VPROC_STATUS                        0x82000001u
#define MSG_VPROC_AAA_STATUS                    0x82000002u
#define MSG_VPROC_EXT_STATUS                    0x82000003u
#define MSG_VPROC_EXT_DATA_STATUS               0x82000004u
#define MSG_VPROC_COMP_OUT_STATUS               0x82000005u
#define MSG_VPROC_EFFECT_DATA_STATUS            0x82000006u
#define MSG_VPROC_DELAYLINE_OSD_STATUS          0x82000007u

// CAT_VIN(4)
#define MSG_VIN_SENSOR_RAW_CAPTURE_STATUS       0x84000001u

// CAT_VOUT (5)
#define MSG_VOUT_STATUS                         0x85000001u

// CAT_ENC(6)
#define MSG_ENC_STATUS                          0x86000001u
#define MSG_ENC_BUFFER_STATUS                   0x86000002u
#define MSG_ENC_INPUT_BUFFER_STATUS             0x86000003u

// CAT_DEC (7)
#define MSG_DECODER_STATUS                      0x87000001u
#define MSG_DECODER_STILLDEC                    0x87000002u
#define MSG_DECODER_PICINFO                     0x87000003u

// CAT_DPROC (8)
#define MSG_DECPROC_STATUS                      0x88000001u
#define MSG_DECPROC_YUV_TO_YUV                  0x88000002u
#define MSG_DECPROC_YUV_DISPLAY                 0x88000003u
#define MSG_DECPROC_YUV_PAINT                   0x88000004u
#define MSG_DECPROC_YUV_BLEND                   0x88000005u

// CAT_POSTPROC (10)
#define MSG_POSTPROC_STATUS                     0x8A000001u

/// CMD_DSP_CONFIG, command code = 0x01000001
/// CMD_DSP_SET_PROFILE, command code = 0x0100000B

#define TOT_IDSP_LD_STR_MCB_LIST_NUM 3u // for the sblk=1,2,3 accordingly

#ifdef PROJECT_CV2S
#define MAX_REAL_VIN_NUM  2u
#define VIRTUAL_VIN_ID_0  2u
#elif defined(PROJECT_CV2E) ||  defined(PROJECT_CV2L) || defined(PROJECT_H2E)
#define MAX_REAL_VIN_NUM  3u
#define VIRTUAL_VIN_ID_0  3u
#else
#define MAX_REAL_VIN_NUM  6u
#define VIRTUAL_VIN_ID_0  6u 
#endif

#define MAX_VIRTUAL_VIN_NUM 7u
#define MAX_VIN_TEMPORAL_DEMUX_NUM 8u // 1u + MAX_VIRTUAL_VIN_NUM

typedef struct
{
  uint32_t cmd_code;
                                // W1
  uint16_t dsp_prof_id;
  uint16_t max_daik_par_num;
                                // W2
  uint32_t orc_poll_intv;
                                // W3
  uint16_t max_dram_par_num;
  uint16_t max_smem_par_num;
                                // W4
  uint16_t max_sub_par_num;
  uint16_t max_sup_par_num;
                                // W5
  uint16_t max_fbp_num;
  uint16_t max_fb_num;
                                // W6
  uint16_t max_dbp_num;
  uint16_t max_cbuf_num;
                                // W7
  uint16_t max_bdt_num;
  uint16_t max_bd_num;
                                // W8
  uint16_t max_imginf_num;
  uint16_t max_ext_fb_num;
                                // W9
  uint16_t max_mcbl_num;
  uint16_t max_mcb_num;
                                // W10
  uint32_t mbuf_par_size;
  uint32_t mbuf_size;
                                // W12
  uint16_t max_orccode_msg_qs;
  uint16_t max_orc_all_msg_qs;
                                // W13
  uint8_t  num_of_vproc_channel;

  // the num of entries in the mcb list reserved for the idsp
  // ld str.
  // [0: sblk = 0, 1]
  // [1: sblk = 2]
  // [2: sblk = 3]
  uint8_t  tot_idsp_ld_str_num[TOT_IDSP_LD_STR_MCB_LIST_NUM];

                                // W14
  uint8_t  vin_bit_mask;
  uint8_t  vin0_output_pins;
  uint8_t  vin1_output_pins;
  uint8_t  vin2_output_pins;
                                // W15
  uint8_t  vin3_output_pins;
  uint8_t  vin4_output_pins;
  uint8_t  vin5_output_pins;
  uint8_t  page_size_k_log2;
                                // W16
  uint16_t max_mfbp_num;
  uint16_t max_mfb_num;
  uint16_t max_ext_mfb_num;
  uint16_t max_mimginf_num;
                                // W18
  uint32_t num_of_enc_channel     : 8;
  uint32_t vout_bit_mask          : 8;

  uint32_t use_fbp_smem_cache     : 1;
  uint32_t use_fbp_dram_cache     : 1;
  uint32_t use_fb_smem_cache      : 1;
  uint32_t use_fb_dram_cache      : 1;
  uint32_t use_imginf_smem_cache  : 1;
  uint32_t use_imginf_dram_cache  : 1;
  uint32_t use_mfbp_smem_cache    : 1;
  uint32_t use_mfbp_dram_cache    : 1;

  uint32_t use_mfb_smem_cache     : 1;
  uint32_t use_mfb_dram_cache     : 1;
  uint32_t use_mimginf_smem_cache : 1;
  uint32_t use_mimginf_dram_cache : 1;
  uint32_t is_effect_on           : 1;
  uint32_t is_partial_load_en     : 1;
  uint32_t dpri3_max_outstanding_xfers_m1 :2;

                                // W19
  uint8_t  num_of_dec_channel;
  uint8_t  dec_codec_support;   // dec_codec_sup_t
  uint8_t  vdec_capture_ena;    
  uint8_t  max_fov_num_per_vin;
                                // W20
  uint8_t  num_of_virtual_vins;
  uint8_t  virtual_vin_output_pins[MAX_VIRTUAL_VIN_NUM];
                               //  W22
  uint16_t vouta_max_pic_ncols;
  uint16_t vouta_max_osd_ncols;
  uint16_t voutb_max_pic_ncols;
  uint16_t voutb_max_osd_ncols;
                               //  W24
  uint8_t vouta_max_pic_nrows;
  uint8_t vouta_max_osd_nrows;
  uint8_t voutb_max_pic_nrows;
  uint8_t voutb_max_osd_nrows;
                               //  W25
  uint16_t vout_mixer_ncols;
  uint8_t vout_mixer_nrows;
  uint8_t vproc_system_dram_size; // size in Megabyte
                               //  W26
  uint32_t max_eng0_width          :16;
  uint32_t vproc_ext_mem_init_mode : 1;
  uint32_t is_sproc_hi_enabled     : 1;
  uint32_t delayline_ena           : 1;
  uint32_t max_dbuf_num_per_vin    : 8;
  uint32_t reserved_2              : 5;
                               //  W27
} cmd_dsp_config_t;

/// CMD_DSP_VPROC_FLOW_MAX_CFG, command code 0x01000008
typedef struct
{
  uint32_t cmd_code;

  // word1
  uint32_t input_format               : 3; // refer to vproc_input_format_t
  uint32_t is_raw_compressed          : 1;
  uint32_t is_tile_mode               : 1;
  uint32_t is_li_enabled              : 1;
  uint32_t is_warp_enabled            : 1;
  uint32_t is_mctf_enabled            : 1;

  uint32_t is_hdr_enabled             : 1;
  uint32_t is_prev_ln_enabled         : 1;
  uint32_t is_prev_a_enabled          : 1;
  uint32_t is_prev_b_enabled          : 1;
  uint32_t is_prev_c_enabled          : 1;
  uint32_t is_prev_hier_enabled       : 1;
  uint32_t is_compressed_out_enabled  : 1;
  uint32_t is_c2y_burst_tiling_out    : 1;

  uint32_t is_hier_burst_tiling_out   : 6;
  uint32_t is_mctf_ref_win_mode       : 1;
  uint32_t num_exp_max                : 2;
  uint32_t is_dzoom_enabled           : 1;
  uint32_t num_of_vproc_groups        : 6; /* up to MAX_VPROC_GROUPS */

  // word2
  uint32_t raw_comp_blk_sz_wst   : 8; // block size of raw compression in the worst-case channel
  uint32_t raw_comp_mantissa_wst : 8; // mantissa of raw compression in the worst-case channel
  uint32_t prev_com0_ch_fmt      : 2;  // see definition in yuv_chroma_fmt_t
  uint32_t prev_com1_ch_fmt      : 2;  // see definition in yuv_chroma_fmt_t
  uint32_t is_mipi_yuyv_enabled  : 1;
  uint32_t max_ch_effect_copy_num: 8; // total effect copy stream in all channels
  uint32_t prev_a_roi_tile_en    : 1;
  uint32_t prev_b_roi_tile_en    : 1;
  uint32_t prev_c_roi_tile_en    : 1;
  //uint32_t reserved2             : 3;


  // word3
  uint32_t max_ch_c2y_tile_num : 8; // total c2y tile number in all channels
  uint32_t max_ch_tile_num     : 8; // total warp tile number in all channels
  uint32_t min_tile_num        : 4; // c2y tile number in the worst-case (smem) channel
  uint32_t max_tile_num        : 4; // warp tile number in the worst-case channel
  uint32_t max_c2y_tile_y_num  : 4;
  uint32_t max_warp_tile_y_num : 4;

  //
  uint16_t raw_width_max;
  uint16_t raw_height_max;

  // must have C2Y/Y2Y parameters
  uint16_t W0_max;
  uint16_t H0_max;

  //////////////////////////////////////

  // WARP luma in maximum size
  uint16_t W_pre_warp_luma_max;
  uint16_t H_pre_warp_luma_max;

  // MAIN out(WARP out, to encoder/preview) maximum sizes
  uint16_t W_main_max;
  uint16_t H_main_max;

  // preview max sizes...
  uint16_t prev_ln_w_max; // prev_d
  uint16_t prev_ln_h_max;
  uint16_t prev_hier_w_max;
  uint16_t prev_hier_h_max;

  uint16_t prev_a_w_max; // PIP
  uint16_t prev_a_h_max;
  uint16_t prev_b_w_max; // TV
  uint16_t prev_b_h_max;
  uint16_t prev_c_w_max; // LCD
  uint16_t prev_c_h_max;

  // parameters for allocating DRAM_Direct mode SMEM buffers
  // warp_a DRAM-direct input maximum heights, default is 64/32
  uint16_t warp_a_y_in_blk_h_max;
  uint16_t warp_a_uv_in_blk_h_max;

  uint16_t warp_tile_overlap_x_max;
  uint16_t hier_poly_tile_overlap_x_max;

  uint8_t  prev_com0_fb_num;  // for prev_c(str_3) usage when veffect; for vout0 usage when non-veffect
  uint8_t  prev_com1_fb_num;  // for prev_b(str_2) usage when veffect; for vout1 usage when non-veffect
  uint16_t msg_ext_data_max_num;
  uint32_t msg_ext_data_base_addr;

  // postp common buf
  uint16_t postp_main_fb_num;
  uint16_t postp_main_me01_fb_num;
  uint16_t postp_pip_fb_num;
  uint16_t postp_pip_me01_fb_num;

  // common preview buf for vout display
  uint16_t prev_com0_w_max;
  uint16_t prev_com0_h_max;
  uint16_t prev_com1_w_max;
  uint16_t prev_com1_h_max;

  // veffect main and pip final output maximum sizes
  uint16_t postp_main_w_max;
  uint16_t postp_main_h_max;
  uint16_t postp_pip_w_max;
  uint16_t postp_pip_h_max;

  uint32_t is_c2y_line_sync_to_warp : 1;
  uint32_t is_c2y_smem_sync_to_warp : 1;
  uint32_t vwarp_chroma_scale_max   : 2;
  uint32_t is_yuyv2y_burst_tiling_out : 1;
  uint32_t init_prev_com0_fb        : 1;
  uint32_t init_prev_com1_fb        : 1;
  uint32_t reserved                 :25;

  uint16_t warp_wait_lines_max;
  uint16_t reserved1;

  uint32_t reserved2            :11;
  // these 3 fields are not in used now, put at the end
  uint32_t is_osd_mixer_enabled : 1;
  uint32_t osd_mixer_w_max      :12;
  uint32_t max_ch_osd_area_num  : 8;

  uint16_t sproc_hi_W_max;
  uint16_t sproc_hi_H_max;
} cmd_dsp_vproc_flow_max_cfg_t;

/// CMD_DSP_ENC_FLOW_MAX_CFG, command code 0x0100000A
#define ENC_MAX_CHANNELS                16u

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_max_resol_t;

#define ENC_MAX_RESOL_4096         0u
#define ENC_MAX_RESOL_3840         1u
#define ENC_MAX_RESOL_1920         2u
#define ENC_MAX_RESOL_1280         3u
#define ENC_MAX_RESOL_JPEG         4u // minimal resource allocation for JPEG
#define ENC_MAX_RESOL_4000x3008    5u // special resol for google jump (4000x3008)
#define ENC_MAX_RESOL_USER_DEFINED 6u
#define ENC_MAX_RESOL_NUM          7u

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_max_type_t;

#define ENC_MAX_TYPES_OFF  0u // use fixed smem size defined in enc_constant.h
#define ENC_MAX_TYPES_ALL  1u // use MAX(SMEM_HEVC, SMEM_H264, SMEM_JPEG)
#define ENC_MAX_TYPES_H264 2u
#define ENC_MAX_TYPES_HEVC 3u
#define ENC_MAX_TYPES_NUM  4u

///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint32_t cmd_code;

  uint32_t reconfig_enc_type                           : 1; // 1=runtime encode type change w/o reset profile. 0=off.
  uint32_t max_enc_types                               : 3; // resource allocation for enc types
                                                            // see definition in enc_max_type_t.
  uint32_t separate_ref_smem                           : 1; // 0=share ref, 1=separate
  uint32_t smem_encode                                 : 2; // see definition in enc_smem_encode_t
                                                            // 2=target+ref smem encode. others=undef.
  uint32_t reset_info_fifo_offset_when_all_stream_idle : 1;
  uint32_t is_10bit                                    : 1; // 0=8bit 1=10bit
  uint32_t slice_memd                                  : 1; // slice_memd support                                     : 0=off 1=on
  uint32_t cout_dwidth_mul                             : 5;
  uint32_t reserved                                    : 17;

  uint8_t  enc_max_resol[ENC_MAX_CHANNELS]; // see definition in enc_max_resol_t.
                                            // 0=default
  uint8_t  max_ref_num[ENC_MAX_CHANNELS];   // max ref number for each channel
                                            // don't care if max_enc_types=0
                                            // 0=default (1 ref). max=4 refs. >4=undef.
  uint8_t  max_smvmax[ENC_MAX_CHANNELS];    // max smvmax: 0=default (56).
                                            // requirement: multiple of 8.
                                            // don't care if max_enc_types=0
  uint8_t  is_mctf_share_ref[ENC_MAX_CHANNELS];
                                            // to specify this channel needs to support
                                            // separate me01 ref smem (when smem_encode=2)
                                            // separate full ref smem (when slice_memd=1)
  uint8_t  max_rec_fb_num[ENC_MAX_CHANNELS];

  uint8_t  eng0_msg_queue_size[2]; // user defined msg queue size. 0=default 32.
  uint16_t eng0_reserved;          // alignment

  uint32_t pjpeg_dram_size;
  uint32_t avc_pjpeg_dram_size;
  uint32_t enc_cfg_daddr;          // DRAM address for the DSP_ENC_CFG arrays
} cmd_dsp_enc_flow_max_cfg_t;

/// CMD_DSP_DEC_FLOW_MAX_CFG, command code 0x0100000C
#define DEC_MAX_CHANNELS      16u

typedef uint8_t dec_codec_sup_t;
#define DEC_CODEC_SUP_DEFAULT 0u
#define DEC_CODEC_SUP_AVC     1u
#define DEC_CODEC_SUP_HEVC    2u
#define DEC_CODEC_SUP_JPEG    4u
#define DEC_CODEC_SUP_ALL     7u

typedef struct
{
  uint32_t max_bit_rate;
  uint32_t max_n_to_m_ratio;
  uint32_t max_frm_num_of_dpb;
  uint16_t max_frm_width;
  uint16_t max_frm_height;
  uint32_t ref_smem_size;

  uint32_t reserved[3];
} dec_flow_max_cfg_t;

#define DPROC_MAX_OUT_STREAMS 3u
#define DPROC_MAX_YUV_OUT_NUM 3u
typedef struct 
{
  uint8_t  yuv_ena;
  uint8_t  me1_ena;
  uint16_t fbuf_num;

  uint16_t fbuf_width;
  uint16_t fbuf_height;
} dproc_vrscl_out_strm_t;

typedef struct
{
  dproc_vrscl_out_strm_t  out_strm[DPROC_MAX_OUT_STREAMS];

  uint32_t reserved[2];
} dproc_flow_max_cfg_t;

typedef struct
{
  uint8_t  alloc_vout0_fbuf;
  uint8_t  reserved_0[3];

  uint16_t vout0_fbuf_width;
  uint16_t vout0_fbuf_height;
  uint32_t vout0_fbuf_num;

  uint32_t reserved_1[5];
} postp_flow_max_cfg_t;

typedef struct
{
  uint32_t cmd_code;

  dec_flow_max_cfg_t    dec_cfg;
  dproc_flow_max_cfg_t  dproc_cfg;
  postp_flow_max_cfg_t  postp_cfg;

} cmd_dsp_dec_flow_max_cfg_t;

/// CMD_DSP_UNITTEST0, command code 0x01000010
typedef struct
{
  uint32_t  cmd_code;

  uint8_t   test_mpart;
  uint8_t   test_appfile;
  uint8_t   test_orcmsg;
  uint8_t   test_fbp;

  uint8_t   test_cbp;
  uint8_t   test_dbp;
  uint8_t   test_bdt;
  uint8_t   test_dma;

  uint8_t   test_mbuf;
  uint8_t   test_10bit;
  uint8_t   test_mfbp;
  uint8_t   test_stitch;

  uint8_t   test_assert_report;
  uint8_t   test_aes;


} cmd_dsp_unittest0_t;

/// CMD_DSP_UNITTEST1(ieng), command code 0x01000011
typedef struct
{

  uint32_t cmd_code;                  // W0
  uint32_t test_id;                   // W1
  uint32_t test_code;                 // W2
  uint32_t aik_cfg_daddr;             // W3

  uint16_t prev_h_luma_in_w;          // W4
  uint16_t prev_h_luma_in_h;

  uint16_t prev_h_luma_out_w;         // W5
  uint8_t  prev_a_en;
  uint8_t  prev_c_en;


  uint8_t  prev_h_in_samp_mode;       // W6
  uint8_t  prev_b_out_samp_mode;
  uint8_t  prev_c_out_samp_mode;
  uint8_t  osd_mixer_enable;

  //
  uint8_t  prev_a_in_samp_mode;       // W7
  uint8_t  prev_a_out_samp_mode;
  uint8_t  osd_0_enable;
  uint8_t  osd_1_enable;

  //
  uint16_t prev_a_luma_in_w;          // W8
  uint16_t prev_a_luma_in_h;

  uint16_t prev_a_luma_out_w;         // W9
  uint16_t prev_a_luma_out_h;

  uint16_t prev_b_luma_out_w;         // W10
  uint16_t prev_b_luma_out_h;

  uint16_t prev_c_luma_out_w;         // W11
  uint16_t prev_c_luma_out_h;

  //
  uint16_t osd_0_left;                // W12
  uint16_t osd_0_width;

  uint16_t osd_0_top;                 // W13
  uint16_t osd_0_height;

  uint16_t osd_1_left;                // W14
  uint16_t osd_1_width;

  uint16_t osd_1_top;                 // W15
  uint16_t osd_1_height;

  uint16_t active_width;              // W16
  uint16_t active_height;

  uint16_t vid_left;                  // W17
  uint16_t vid_width;

  uint16_t vid_top;                   // W18
  uint16_t vid_height;

  uint8_t  extra_2x_0_hor_enable;     // W19
  uint8_t  extra_2x_0_ver_enable;
  uint8_t  extra_2x_1_hor_enable;
  uint8_t  extra_2x_1_ver_enable;

  uint8_t mctf_pos_enable;            // W20
  uint8_t mctf_pos_in_width_m1;
  uint8_t mctf_pos_in_height_m1;
  uint8_t aaa_cfa_enable;

  uint8_t aaa_pg_enable;              // W21
  uint8_t prev_in_tiled_fmt;
  uint8_t prev_a_user_customized_filter_coefficients_enable;
  uint8_t prev_a_adjust_coefficients;

  uint16_t main_raw_decomp_width;     // W22
  uint8_t  main_yuv_in_samp_mode;
  uint8_t  main_yuv_out_samp_mode;

  uint16_t main_yuv_luma_in_w;        // W23
  uint16_t main_yuv_luma_in_h;

  uint16_t main_yuv_luma_out_w;       // W24
  uint16_t main_yuv_luma_out_h;

  uint8_t bypass_vin;                 // W25
  uint8_t me0_enable;
  uint8_t me_enable;
  uint8_t osd_rescaler_in_select;

  uint16_t osd_rescaler_w;            // W26
  uint16_t osd_rescaler_h;

  uint8_t  osd_rescaler_out_osd_mode; // W27
  uint8_t  osd_rescaler_in_osd_mode;
  uint8_t  prev_d_in_samp_mode;
  uint8_t  prev_d_out_samp_mode;

  uint16_t prev_d_luma_in_w;          // W28
  uint16_t prev_d_luma_in_h;

  uint16_t prev_d_luma_out_w;         // W29
  uint16_t prev_d_luma_out_h;

  uint8_t  is_10bit;                  // W30
  uint8_t  deint_vin;
  uint8_t  bypass_c2y;
  uint8_t  test_keep_cfg;

  uint16_t prev_e_luma_w;             // W31
  uint16_t prev_e_luma_h;

} cmd_dsp_unittest1_t;

/// CMD_DSP_UNITTEST2(gme), command code 0x01000012
typedef struct
{
  uint32_t  cmd_code; // W0
  uint32_t  test_id;  // W1
  uint16_t  width;
  uint16_t  height;
  uint16_t  superblock_width;
  uint16_t  superblock_height;
  uint8_t   hpel_search;
  uint8_t   hpel_exclude_boundary_fp;
  uint8_t   big_search;
  uint8_t   search_range;
  uint8_t   luma_thresh;
  uint8_t   nframes;
  uint8_t   strscr_enable;
  uint8_t   strscr_size;
  uint8_t   strscr_shift;
  uint8_t   activity_mode;
  uint8_t   slice_num;
  uint8_t   reserved;
  char      sequence[128];
} cmd_dsp_unittest2_t;

/// CMD_DSP_VIN_UNITTEST, command code 0x01000013
typedef struct
{
  uint32_t  cmd_code;
  uint16_t  vin_cap_width;
  uint16_t  vin_cap_height;
  uint8_t   sensor_type;
} cmd_dsp_vin_unittest_t;

/// CMD_DSP_UNITTEST3(idspdrv), command code 0x01000014
typedef struct
{
  uint32_t  cmd_code;
  uint16_t  test_id;

  uint16_t  active_width;
  uint16_t  active_height;
  uint16_t  ce_out_width;

  uint16_t  main_yuv_luma_out_w;
  uint16_t  main_yuv_luma_out_h;
  uint16_t  ext_raw_out_w;
  uint16_t  ext_raw_out_h;

  uint8_t  in_samp_mode;
  uint8_t  out_samp_mode;

  uint16_t prev_a_luma_out_w;
  uint16_t prev_a_luma_out_h;
  uint16_t prev_b_luma_out_w;
  uint16_t prev_b_luma_out_h;

  uint16_t crop_win_left;
  uint16_t crop_win_top;
  uint16_t crop_win_width;
  uint16_t crop_win_height;

  uint16_t padding;

} cmd_dsp_unittest3_t;

/// CMD_DSP_VIN_UNITTEST2, command code 0x01000015
typedef struct
{
  uint32_t  cmd_code;
  uint32_t  test_id;
  uint16_t  cap_width[5];
  uint16_t  cap_height[5];
  uint32_t  test_case;
} cmd_dsp_vin_unittest2_t;

/// CMD_DSP_VOUTB_UNITTEST, command code 0x01000016
typedef struct
{
  uint32_t  cmd_code;
  uint32_t  is_voutb_test_prof_chg  : 1;
  uint32_t  is_test_def_cfg_chg     : 1;
  uint32_t  is_test_mode_switch     : 1;
  uint32_t  reserved0               : 29;
} cmd_dsp_voutb_unittest_t;

///////////////////////////////////////////////////////////////////////////////
typedef struct
{
  uint32_t cmd_code;
  uint32_t channel_id : 6;
  uint32_t rsvd       : 26;
} cmd_vproc_hdr_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vproc_prev_mode_t;

#define DRAM_PREV_MODE           0u
#define WARP_MCTF_SMEM_PREV_MODE 1u // Warp/MCTF->SMEM(SYNC_TYPE_TILE->LINE)->PREV->DRAM
#define VOUT_TH_PREV_MODE        2u // DRAM-->PREV->SMEM->VOUT, PREV sunning in VOUT thread

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vproc_input_format_t;

#define VPROC_INPUT_RAW_RGB 0u
#define VPROC_INPUT_YUV_422 1u
#define VPROC_INPUT_YUV_420 2u
#define VPROC_INPUT_MIPI_YUYV   3u
#define VPROC_INPUT_MIPI_RAW    4u

///////////////////////////////////////////////////////////////////////////////

#define VPROC_LN_DEC_FB_DISABLE   0XFFu
#define VPROC_HIER_FB_NUM_DISABLE 0XFFu
#define VPROC_FB_NUM_DISABLE      0XFFu

#define VPROC_MAX_CHAN_NUM 20u
#define VPROC_MAX_CHAN_PER_GRP 16u /* up to 16 channels per vproc group */
#define VPROC_MAX_GRP_NUM   4u
#define VEFFECT_MAX_GRP_NUM 4u

#define VPROC_MAIN_STREAM_IDX 0u
#define VPROC_PIP_STREAM_IDX  1u
#define VPROC_3RD_STREAM_IDX  2u
#define VPROC_4TH_STREAM_IDX  3u
#define VPROC_STREAM_NUMS     4u

/// CMD_VPROC_CONFIG (0x02000001)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id             : 6;
  uint32_t input_format           : 3; /* refer to vproc_input_format_t */
  uint32_t prev_mode              : 2; /* refer to vproc_prev_mode_t */
  uint32_t num_of_exp             : 2; /* number of exposure: 1, 2, 3 */
  uint32_t grp_id                 : 3; 
  uint32_t is_li_enabled          : 1;
  uint32_t is_hdr_enabled         : 1;
  uint32_t is_warp_enabled        : 1;
  uint32_t prev_a_roi_tile_en     : 1;
  uint32_t prev_b_roi_tile_en     : 1;
  uint32_t prev_c_roi_tile_en     : 1;
  uint32_t is_osd_mixer_enabled   : 1;
  uint32_t is_still_hi_enabled    : 1;
  uint32_t rsvd0                  : 8;

  // raw max sizes
  uint16_t raw_width_max;
  uint16_t raw_height_max;

  // post cfa raw size
  uint16_t W0_max;
  uint16_t H0_max;

  // WARP luma in maximum size
  uint16_t W_pre_warp_luma_max;
  uint16_t H_pre_warp_luma_max;

  // MAIN out(WARP out, to encoder/preview) maximum sizes
  uint16_t W_main_max;
  uint16_t H_main_max;

  // preview max sizes...
  uint16_t prev_ln_w_max;   /* prev_d */
  uint16_t prev_ln_h_max;
  uint16_t prev_hier_w_max; /* Hierarchical resampler, sec15*/
  uint16_t prev_hier_h_max;

  uint16_t prev_a_w_max; /* PIP */
  uint16_t prev_a_h_max;
  uint16_t prev_b_w_max; /* TV */
  uint16_t prev_b_h_max;
  uint16_t prev_c_w_max; /* LCD */
  uint16_t prev_c_h_max;

  // parameters for allocating DRAM_Direct/SMEM_WIN mode SMEM buffers
  uint16_t warp_a_y_in_blk_h_max;
  uint16_t warp_a_uv_in_blk_h_max;

  /* main stream's main and me01 FB numbers, 0: use default value */
  uint8_t main_fb_num;
  uint8_t main_me01_fb_num;

  uint8_t prev_a_fb_num;
  uint8_t prev_a_me_fb_num; 

  uint8_t prev_b_fb_num;
  uint8_t prev_c_fb_num;

  uint8_t ln_dec_fb_num; /* 0: default, VPROC_LN_DEC_FB_DISABLE(0xff): disable */
  uint8_t hier_fb_num;   /* 0: default, VPROC_HIER_FB_NUM_DISABLE(0xff): disable */

  uint8_t c2y_int_fb_num;   /* C2Y pass internal FBP size */

  uint8_t raw_comp_blk_sz_wst;   /* block size of raw compression in worst case */
  uint8_t raw_comp_mantissa_wst; /* mantissa of raw compression in worst case */

  uint8_t rsvd1;

  uint16_t extra_sec2_vert_out_max; /* max extra section 2 vertical output for dzoom */
  uint8_t  prev_b_me_fb_num;
  uint8_t  prev_c_me_fb_num;

  uint32_t c2y_tile_num_x_max     : 4;
  uint32_t c2y_tile_num_y_max     : 2;
  uint32_t warp_tile_num_x_max    : 4;
  uint32_t warp_tile_num_y_max    : 2;
  uint32_t c2y_big_tile_num_y_max : 4;
  uint32_t warp_big_tile_num_y_max: 4;
  uint32_t osd_blend_area_num_max_m1   : 5;  
  uint32_t osd_blend_stream_num_max_m1 : 3;  
  uint32_t effect_copy_job_num_max     : 3; /* max number of effect copy jobs */
  uint32_t rsvd2                       : 1;

  uint32_t effect_blend_job_num_max    : 3; /* max number of effect blend jobs */
  uint32_t rsvd3                       :29;
} cmd_vproc_cfg_t;

///////////////////////////////////////////////////////////////////////////////
// vproc preview format

typedef uint8_t vproc_prev_format_t;

#define VPROC_PREV_FORMAT_PROGRESSIVE 0u
#define VPROC_PREV_FORMAT_INTERLACE   1u

///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint8_t  tile_idx_x;
  uint8_t  tile_idx_y;
  uint16_t tile_start_x;
  uint16_t tile_start_y;
  uint16_t tile_width;
  uint16_t tile_height;
} slice_layout_t;

typedef uint8_t vproc_proc_mode_t;

#define VIDEO_LISO 0u
#define STILL_LISO 1u
#define STILL_HISO 2u  // not ready

/// CMD_VPROC_SETUP (0x02000002)
typedef struct
{
  uint32_t cmd_code;

  // Word1
  uint32_t channel_id        : 6;
  uint32_t rsvd0             : 26;

  // Word2
  uint32_t input_format      : 3; // refer to vproc_input_format_t
  uint32_t is_raw_compressed : 1;
  uint32_t is_tile_mode      : 1;
  uint32_t is_li_enabled     : 1;
  uint32_t is_warp_enabled   : 1;
  uint32_t is_mctf_enabled   : 1;

  uint32_t is_hdr_enabled            : 1;
  uint32_t is_prev_ln_enabled        : 1; // prev_d
  uint32_t is_prev_a_enabled         : 1; // PIP
  uint32_t is_prev_b_enabled         : 1; // TV
  uint32_t is_prev_c_enabled         : 1; // LCD
  uint32_t is_prev_hier_enabled      : 1;  // Hier                                
  uint32_t is_compressed_out_enabled : 1; // mctf compression out
  uint32_t is_c2y_burst_tiling_out   : 1;

  uint32_t is_hier_burst_tiling_out  : 13;
  uint32_t is_mctf_ref_win_mode      : 1;
  uint32_t prev_mode                 : 2;  // refer to vproc_input_format_t

  // Word3
  uint32_t is_aaa_enabled     : 1; // AAA cfg
  uint32_t cfg_aaa_by_ae_y    : 1; // configure AAA by AE Y tile from AAA setup command for tile mode
  uint32_t is_fast_y2y_en     : 1; /* 1: bypass idsp section2 and no dram copy from pic info */
  uint32_t is_hier_poly_sqrt2 : 1;
  uint32_t is_hdr_blend_dram_out_enabled : 1; // 1: sec2 external_raw_out to dram for debug
  uint32_t is_warp_dram_out_enabled      : 1; // 1: warp_luma_out and warp_chroma_out to dram
  uint32_t is_mcts_dram_out_enabled      : 1; // 1: mcts out to dram
  uint32_t is_c2y_dram_out_enabled       : 1;
 
  uint32_t c2y_tile_num_x  : 4;
  uint32_t c2y_tile_num_y  : 2;
  uint32_t warp_tile_num_x : 4;
  uint32_t warp_tile_num_y : 2;
  uint32_t ln_det_src      : 8;  // hier filter idx serves as an input to the ln detect filter
  uint32_t aaa_cfa_mux_sel : 1; 
  uint32_t is_dzoom_enabled: 1;
  uint32_t is_mipi_yuyv_enabled : 1;  /* mipi yuyv: use yuyv2y flow with sec6/7 to extract yuv */
  uint32_t is_pic_info_cache_enabled: 1;

  /////////////////////////////////////////////////////////////////////////////
  // preview A/B/C setting...
  uint32_t prev_a_format    :2; // refer to enum vproc_prev_format_t
                                // 0:progressive, 1:interlace
  uint32_t prev_a_src       :2; // 0: dram, 1: smem, 2: vout thread preview,
                                // 0xf: No_PREVIEW(turning off idsp preveiw filter)
  uint32_t prev_a_dst       :4; // refer to enum VPROC_PREV_DST_MASK
                                // preveiw destination:
                                // 0: default arrangement, preview a -> PIN, 
                                //                         preview b -> TV, 
                                //                         preview c -> LCD
                                // 1: VOUT0
                                // 2: VOUT1
                                // 3:
                                // 4: PIN
  uint32_t prev_a_frame_rate:8; // refer to enum VPROC_PREV_FRAME_RATE
                                // 0: 29.97, 1: 59.94, 30, 60, 24, 15

  uint32_t prev_b_sync_to_enc :1;
  uint32_t prev_b_rsvd      :3;
                                // 0xf: No_PREVIEW(turning off idsp preveiw filter)
  uint32_t prev_b_dst       :4; // refer to enum VPROC_PREV_DST_MASK
                                // preveiw destination:
                                // 0: default arrangement, preview a -> PIN, 
                                //                         preview b -> TV, 
                                //                         preview c -> LCD
                                // 1: VOUT0
                                // 2: VOUT1
                                // 4: PIN
                                // 8: DUMMY (no dram out)

  uint32_t prev_b_frame_rate:8; // refer to enum VPROC_PREV_FRAME_RATE
                                // 0: 29.97, 1: 59.94, 30, 60, 24, 15
  
  //
  uint32_t prev_c_format    :2; // refer to enum vproc_prev_format_t
                                // 0:progressive, 1:interlace
  uint32_t prev_c_src       :2; // 0: dram, 1: smem, 2: vout thread preview,
                                // 0xf: No_PREVIEW(turning off idsp preveiw filter)
  uint32_t prev_c_dst       :4; // refer to enum VPROC_PREV_DST_MASK
                                // preveiw destination:
                                // 0: default arrangement, preview a -> PIN, 
                                //                         preview b -> TV, 
                                //                         preview c -> LCD
                                // 1: VOUT0
                                // 2: VOUT1
                                // 4: PIN
                                // 8: DUMMY (no dram out)

  uint32_t prev_c_frame_rate:8; // refer to enum VPROC_PREV_FRAME_RATE
                                // 0: 29.97, 1: 59.94, 30, 60, 24, 15

  uint32_t prev_a_ch_fmt            :2; // yuv_chroma_fmt_t
  uint32_t prev_b_ch_fmt            :2; // yuv_chroma_fmt_t
  uint32_t prev_c_ch_fmt            :2; // yuv_chroma_fmt_t
  uint32_t mctf_mode                :2;
  uint32_t mctf_chan                :2;
  uint32_t mcts_chan                :2;
  uint32_t is_h_slice_low_delay     :1;
  uint32_t is_c2y_line_sync_to_warp :1;
  uint32_t is_c2y_smem_sync_to_warp :1;
  uint32_t is_osd_mixer_enabled     :1;

  uint32_t frm_delay_time_ticks;

  // capture main out size, to encoder/previews
  uint16_t W_main;
  uint16_t H_main;

  // preview sizes...
  uint16_t prev_a_w;
  uint16_t prev_a_h;
  uint16_t prev_b_w;
  uint16_t prev_b_h; 
  uint16_t prev_c_w;
  uint16_t prev_c_h;

  uint16_t prev_ln_w;
  uint16_t prev_ln_h;

  uint16_t hier_poly_w;
  uint16_t hier_poly_h;

  uint16_t warp_tile_overlap_x;
  uint16_t hier_poly_tile_overlap_x;

  uint16_t warp_tile_overlap_y;
  uint16_t warp_wait_lines;

  uint32_t ext_buf_mask        : 16;
  uint32_t c2y_big_tile_num_y  : 4;
  uint32_t warp_big_tile_num_y : 4;
  uint32_t vwarp_chroma_scale_max : 2;
  uint32_t ext_mem_cfg_num     : 5;
  uint32_t raw_tile_alignment_mode : 1;

  /**
   * C2Y output slice layout.
   * slice_layout_ptr points to a 1D array of slice_layout_t.
   * unit index = tile_idx_x + (c2y_tile_num_x * tile_idx_y).
   * unit size = slice_layout_usize
   * unit number = c2y_tile_num_x * c2y_tile_num_y.
   * if c2y_big_tile_num_y > 0, c2y_tile_num_y = c2y_big_tile_num_y.
   */
  uint32_t slice_layout_ptr;
  uint32_t slice_layout_usize; /* ALIGN_64(sizeof(slice_layout_t)) */

  uint32_t ext_mem_cfg_addr;

  uint32_t raw_tile_overlap_x  : 8;
  uint32_t is_3A_stat_only     : 1;  // 3A data output only [i.e. only c2y stage, no c2y output data though]
  uint32_t proc_mode           : 4;  // refer to enum vproc_proc_mode_t
  uint32_t num_of_exp          : 2;  // number of exposure: 1, 2, 3
                                     // num_of_exp is also controlled by cmd_vproc_config
                                     // for backward compatible, using 0 to keep the original setting
  uint32_t raw_tile_overlap_x_ext : 3;
  uint32_t is_yuyv2y_burst_tiling_out : 1;
  uint32_t mctf_cmpr           : 2;
  uint32_t proc_hds_as_main    : 1;
  uint32_t raw_tile_overlap_mode : 1;
  uint32_t prev_a_presend_to_vout :1;
  uint32_t prev_b_presend_to_vout :1;
  uint32_t prev_c_presend_to_vout :1;
  uint32_t c2y_delay_prefetch  : 1;
  uint32_t main_pin_out_type   : 2;
  uint32_t vp_msg_seq_no_mode  : 1;
  uint32_t delayline_ena       : 1;
  uint32_t rsvd4               : 1;
} cmd_vproc_setup_t;

#if 0
typedef struct
{
  uint32_t mctf_config_update         : 1;
  uint32_t frame_active_update        : 1;
  uint32_t motion_vectors_update      : 1;
  uint32_t histogram_0_update         : 1;
  uint32_t histogram_1_update         : 1;
  uint32_t histogram_2_update         : 1;
  uint32_t histogram_3_update         : 1;
  uint32_t curve_b_update             : 1;
  uint32_t curve_c_update             : 1;
  uint32_t curve_d_update             : 1;
  uint32_t tone_adj_config_update     : 1;
  uint32_t tone_hist_tbl_update       : 1;
  uint32_t mctf_3d_lvl_l_update       : 1;
  uint32_t mctf_3d_lvl_u_update       : 1;
  uint32_t curve_sb2_update           : 1;
  uint32_t ref_luma_remap_update      : 1;
  uint32_t mcts_config_update         : 1;
  uint32_t shpb_config_update         : 1;
  uint32_t shpc_config_update         : 1;
  uint32_t shpb_lpf_coef_1_update     : 1;
  uint32_t shpb_lpf_coef_2_update     : 1;
  uint32_t shpc_lpf_coef_1_update     : 1;
  uint32_t shpc_lpf_coef_2_update     : 1;
  uint32_t shpb_alpha_update          : 1;
  uint32_t shpc_alpha_update          : 1;
  uint32_t shpb_coring1_tbl_update    : 1;
  uint32_t shpc_coring1_tbl_update    : 1;
  uint32_t shpb_coring2_tbl_update    : 1;
  uint32_t shrp_3d_lvl_l_update       : 1;
  uint32_t shrp_3d_lvl_u_update       : 1;
  uint32_t cmpress_config_1_update    : 1;
  uint32_t cmpress_config_2_update    : 1;
} vproc_mctf_cfg_update_t;
#endif

/// CMD_VPROC_MCTF_CONFIG (0x02000003)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id                 : 6;
  uint32_t noise_filter_strength      : 2;
  uint32_t mctf_chan                  : 2;
  uint32_t mcts_chan                  : 2;
  uint32_t cmpr_en                    : 1; // doesn't support currently
  uint32_t mctf_mode                  : 2;
  uint32_t bitrate_y                  : 6; // doesn't support currently
  uint32_t bitrate_uv                 : 6; // doesn't support currently
  uint32_t bypass_mctf                : 1; // 1 to enable mctf low power. (for mode 5/6 use), doesn't support on A9A
  uint32_t reserved                   : 4;
  uint32_t mctf_cfg_0_dbase;
  uint32_t mctf_cfg_1_dbase;
  uint32_t mctf_cfg_ta_dbase;
  uint32_t cmpr_cfg_dbase;                 // current doesn't support compression

  // config update flags
  //vproc_mctf_cfg_update_t  cmds;
  uint32_t loadcfg_type;

} cmd_vproc_mctf_config_t;

/// CMD_VPROC_IMG_PRMD_SETUP  (0x02000004)
typedef struct
{
  int16_t  roi_start_row;
  int16_t  roi_start_col;
  uint16_t roi_height;
  uint16_t roi_width;
} scale_info_t;

typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id            : 6;
  uint32_t enable_bit_mask       : 6; // bit positon indicates which scaler is on, i.e. bit 0 -> scale_out_0, bit 1->scale_out_1, etc
  uint32_t is_hier_poly_sqrt2    : 1;

  // is 1 then, the roi cmd is first entered into a holding area, or grouped with all other roi cmds,
  // and then issued to the master fov's cmd q [indicated by grp_fov_cmd_id]. this will make sure
  // all roi commands with a stereo pair would be executed at the same time.
  // is 0, then the roi cmd is dispatched directly to fov cmd q indicated by channel_id
  uint32_t is_grp_cmd            : 1;
  uint32_t grp_fov_cmd_id        : 4;
  uint32_t hier_burst_tiling_out : 6;
  uint32_t rsvd0                 : 8;
  scale_info_t scale_info[6];
  uint32_t roi_tag;
  uint32_t deci_rate;
} cmd_vproc_img_pyramid_setup_t;

/// CMD_VPROC_LN_DET_SETUP        (0x02000005)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id      : 6;
  uint32_t pyr_filter_idx  : 4; // which pyramid output serves as input to the lane detection input.
  uint32_t is_grp_cmd      : 1; // same idea as those used in 0x02000004. often roi cmds may need to pair with lane detect change cmd since lane det input is one of the roi's output.
  uint32_t grp_fov_cmd_id  : 4;
  uint32_t rsvd0           : 17;
  uint16_t ld_roi_start_row;
  uint16_t ld_roi_start_col;
  uint16_t ld_roi_height;
  uint16_t ld_roi_width;
} cmd_vproc_lane_det_setup_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t SPC_OUT_SEL_t;

#define SPC_COMPR_RAW_OUTPUT_SEL_SBPIXEL  0u // 0=Static BadPixel
#define SPC_COMPR_RAW_OUTPUT_SEL_PRESCALE 1u // 1=Prescale-2D
#define SPC_COMPR_RAW_OUTPUT_SEL_CFA_NF   2u // 2=CFA Noise Filter
#define SPC_COMPR_RAW_OUTPUT_SEL_CFA_TF   3u // 3=CFA Temporal Filter
#define SPC_YUV420_OUTPUT                 4u

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vproc_prev_id_t;

#define VPROC_PREV_A 0u
#define VPROC_PREV_B 1u
#define VPROC_PREV_C 2u
#define VPROC_PREV_D 3u


///////////////////////////////////////////////////////////////////////////////

typedef uint8_t VPROC_PREV_FRAME_RATE;

#define VPROC_PREV_FRAME_RATE_15    15u // 15 fps
#define VPROC_PREV_FRAME_RATE_24    24u // 24 fps
#define VPROC_PREV_FRAME_RATE_29_9  0u  // 29.9 fps
#define VPROC_PREV_FRAME_RATE_30    30u // 30 fps
#define VPROC_PREV_FRAME_RATE_59_94 1u  // 59.94 fps
#define VPROC_PREV_FRAME_RATE_60    60u // 60 fps

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t VPROC_PREV_DST_MASK;

#define VPROC_PREV_DST_NULL   0x0u
#define VPROC_PREV_DST_VOUT0  0x1u
#define VPROC_PREV_DST_VOUT1  0x2u
#define VPROC_PREV_DST_PIN    0x4u
#define VPROC_PREV_DST_DUMMY  0x8u // No DRAM out

///////////////////////////////////////////////////////////////////////////////

/// CMD_VPROC_PREV_SETUP (0x02000006)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id     : 6;
  uint32_t prev_id        : 3; // use vproc_prev_id_t
  uint32_t prev_format    : 2; // 0: progressive, 1: interlace
  uint32_t prev_src       : 4; // 0: dram, 1: smem, 2: vout thread preview,
                               // 0xf: No_PREVIEW(turning off idsp preveiw filter)
  uint32_t prev_dst       : 4; // use VPROC_PREV_DST_MASK
  uint32_t prev_frame_rate: 8; // refer to VPROC_PREV_FRAME_RATE
                               // 0: 29.97, 1: 59.94, 30, 60, 24, 15
  uint32_t prev_freeze_ena: 1; // preveiw A freeze support
  uint32_t prev_roi_tile_en:1; // preview roi enable for tile mode
  uint32_t rsvd0          : 3;

  uint16_t prev_w;
  uint16_t prev_h;

  uint16_t prev_src_w;        // pan scan support
  uint16_t prev_src_h;        // new fields for preview source window parameters of preview A and B
  uint16_t prev_src_x_offset; // x/y offset is relative to the upper left corner of the Main window.
  uint16_t prev_src_y_offset;

  uint16_t prev_out_width; 
  uint16_t prev_out_height;
} cmd_vproc_prev_setup_t;

///////////////////////////////////////////////////////////////////////////////
// external memory types

typedef uint8_t ext_mem_type_t;

#define EXT_MEM_TYPE_MAIN         0u
#define EXT_MEM_TYPE_PREV_A       1u
#define EXT_MEM_TYPE_PIP          EXT_MEM_TYPE_PREV_A
#define EXT_MEM_TYPE_PREV_B       2u
#define EXT_MEM_TYPE_PREV_C       3u
#define EXT_MEM_TYPE_LN_DEC       4u
#define EXT_MEM_TYPE_HIER_0       5u
#define EXT_MEM_TYPE_COMP_RAW     6u
#define EXT_MEM_TYPE_MAIN_ME      7u
#define EXT_MEM_TYPE_PIP_ME       8u
#define EXT_MEM_TYPE_NUM          9u
#define EXT_MEM_TYPE_PREV_A_ME    (EXT_MEM_TYPE_NUM+0u)
#define EXT_MEM_TYPE_PREV_B_ME    (EXT_MEM_TYPE_NUM+1u)
#define EXT_MEM_TYPE_PREV_C_ME    (EXT_MEM_TYPE_NUM+2u)
#define EXT_MEM_TYPE_MCTS         (EXT_MEM_TYPE_NUM+3u)
#define EXT_MEM_TYPE_MCTF         (EXT_MEM_TYPE_NUM+4u)

///////////////////////////////////////////////////////////////////////////////

/// CMD_VPROC_SET_EXT_MEM (0x02000007)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id       : 8;  // channel id
  uint32_t reserved_0       : 16;
  uint32_t reserved_1       : 8;

  uint8_t  memory_type;           // ext_mem_type_t
  uint8_t  allocation_type;       // 0: distinct addr, 1: start addr
  uint16_t num_frm_buf;           // number of raw frame buffers

  uint16_t buf_pitch;             // buffer pitch
  uint16_t buf_width;             // buffer width
  uint16_t buf_height;            // buffer height

  uint32_t allocation_mode  : 2;  // 0: new, 1: append
  uint32_t overflow_cntl    : 2;  // 0: rotate, 1: stall
  uint32_t chroma_format    : 2;  // yuv_chroma_fmt_t
  uint32_t reserved_2       : 16;
  uint32_t reserved_3       : 8;
  uint32_t reserved_4       : 2;

  uint16_t luma_img_ofs_x;        // luma image offset x
  uint16_t luma_img_ofs_y;        // luma image offset y
  uint16_t chroma_img_ofs_x;      // chroma image offset x
  uint16_t chroma_img_ofs_y;      // chroma image offsest y
  uint16_t img_width;             // image width
  uint16_t img_height;            // image height

  uint32_t buf_addr;              // memory segment starting address

  uint16_t    aux_pitch;
  uint16_t    aux_width;
  uint16_t    aux_height;
  uint16_t    aux_img_ofs_x;         // aux image offset x
  uint16_t    aux_img_ofs_y;         // aux image offset y
  uint16_t    aux_img_width;         // aux image width
  uint16_t    aux_img_height;        // aux image height
  uint16_t    reserved_5;
  
} cmd_vproc_set_ext_mem_t;

// CMD_VPROC_GRP_CMD - 0x02000008
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id       : 8;  // channel id
  uint32_t is_sent_from_sys : 4;
  uint32_t is_vout_cmds     : 4;  // the entire group cmd contains only vout cmds.
  uint32_t vout_id          : 2;
  uint32_t reserved_0       : 6;
  uint32_t reserved_1       : 8;

  uint32_t grp_cmd_buf_id;
  uint32_t grp_cmd_buf_addr;
  uint32_t num_cmds_in_buf;
} cmd_vproc_fov_grp_cmd_t;


/// CMD_VPROC_STOP (0x02000009)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id : 6;
  uint32_t rsvd0      : 26;

  uint32_t stop_cap_seq_no;
  uint8_t  reset_option;
  uint8_t  rsvd1;
  uint8_t  rsvd2;
  uint8_t  rsvd3;

} cmd_vproc_stop_t;

/// CMD_VPROC_OSD_BLEND (0x0200000Au)
typedef struct
{
  uint32_t  cmd_code;

  uint32_t  channel_id  : 6;  // channel id
  uint32_t  reserved_0  : 2;

  uint32_t  stream_id   : 8;  // 0: main output pin, 1: pin1, 2: pin2, 3: pin3
  uint32_t  reserved_1  : 16;

  uint32_t  cap_seq_no;       // osd blending starting sequence #
  uint8_t   enable;           // 0: disable, 1: enable, 2: config only
  uint8_t   blending_area_id; // unique id from 0 to 31
  uint16_t  reserved_2;

  uint32_t  osd_addr_y;       // osd dram luma address
  uint32_t  osd_addr_uv;      // osd dram chroma address
  uint32_t  alpha_addr_y;     // alpha luma dram address
  uint32_t  alpha_addr_uv;    // alpha chroma dram address
  uint16_t  osd_width;        // blend osd (alpha) width
  uint16_t  osd_pitch;        // osd matrix width or dram pitch
  uint16_t  osd_height;       // osd (alpha) matrix height
  uint16_t  osd_start_x;      // starting horizontal position
  uint16_t  osd_start_y;      // starting vertical position
  uint16_t  reserved_3;
} cmd_vproc_osd_blend_t;

/// CMD_VPROC_PIN_OUT_DECIMATION (0x0200000Bu)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id :6;
  uint32_t rsvd0      :2;
  uint32_t stream_id  :8;
  uint32_t rsvd1      :16;

  uint32_t cap_seq_no;                // to control decimation rate happen at raw cap frame sequence #.
  uint32_t output_repeat_ratio;       // N: bit 31~16, M: bit 15~0; default 0x10001;

} cmd_vproc_pin_out_deci_t;

/// CMD_VPROC_ECHO (0x0200000Cu)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id :6;
  uint32_t rsvd0      :2;
  uint32_t rsvd1      :24;

  uint32_t grp_cmd_buf_id;

} cmd_vproc_echo_t;


typedef struct
{
  uint32_t user_id;
  uint32_t pin_code;
} osd_sync_data_t;

typedef struct
{
  int16_t x; /* must be even for chroma pairing (& 0xFFFE) */
  int16_t y; /* must be a multiple of 4 to ensure even height for 420 chroma (& 0xFFFC) */
  uint16_t w; /* must be a multiple of 4 for SMEM alignment */
  uint16_t h; /* must be a multiple of 4 to ensure even height for 420 chroma (& 0xFFFC) */
} osd_insert_rect_t;

typedef struct
{
  osd_sync_data_t osd_sync;
  uint32_t osd_clut_daddr;   /* DRAM address of CLUT */
  uint32_t osd_daddr;
  osd_insert_rect_t osd_size;
  uint16_t osd_dpitch;
  uint16_t rsvd0;
} osd_insert_buf_t;

/// CMD_VPROC_OSD_INSERT  (0x0200000Du)
#define VPROC_OSD_DISABLED     0u
#define VPROC_OSD_MAPPED_IN    1u
#define VPROC_OSD_16BIT_DIR_IN 2u
#define VPROC_OSD_32BIT_DIR_IN 3u
typedef struct
{
  uint32_t cmd_code;

  uint32_t chan_id                :6;
  uint32_t stream_id              :8;
  /**************************************************************************
   * 0: Disabled
   * 1: Mapped In Enabled
   * 2: 16-bit Direct In Enabled
   * 3: 32-bit Direct In Enabled
   *************************************************************************/
  uint32_t osd_enable             :2;
  /*************************************************************************
   * The OSD data format in 16-bit mode. 0 - 5:6:5 (VYU) or (RGB)
   * 1 - 5:6:5 (UYV) or (BGR)
   * 2 - 4:4:4:4 (AYUV)
   * 3 - 4:4:4:4 (RGBA)
   * 4 - 4:4:4:4 (BGRA)
   * 5 - 4:4:4:4 (ABGR)
   * 6 - 4:4:4:4 (ARGB)
   * 7 - 1:5:5:5 (AYUV)
   * 8 - 1:5:5:5 (MSB ignored, YUV)
   * 9 - 5:5:5:1 (RGBA)
   * 10 - 5:5:5:1 (BGRA)
   * 11 - 1:5:5:5 (ABGR)
   * 12 - 1:5:5:5 (ARGB)
   * 
   * 13 - 26: reserved
   * 
   * The OSD data format in 32-bit mode.
   * 27: 8:8:8:8 (AYUV)
   * 28: 8:8:8:8 (RGBA)
   * 29: 8:8:8:8 (BGRA)
   * 30: 8:8:8:8 (ABGR)
   * 31: 8:8:8:8 (ARGB)
   *
   * Other values reserved must be set in conjunction with OSD In
   * Enable field, ignored in OSD Mapped mode (8-bit)
   *************************************************************************/
  uint32_t osd_mode               :5;
  uint32_t osd_insert_always      :1; /* if 1, we insert OSD into buffer even when it's
                                         not encoded by the encoder */
  uint32_t osd_num_regions_minus1 :8; /* FIXME--flpan: use actual num_regions instead of _minus1???*/
  /**************************************************************************
   * If the OSD command is late(ie: it has a target_pts for a frame that 
   * has already been processed by the OSD stage after the frame's latency), 
   * the force_update_flag parameter will indicate how to handle it:
   *  
   * force_update_flag = 0, the OSD cmd is discarded
   * force_update_flag = 1, the OSD cmd is applied to catch up.
   * It also becomes the most recent OSD that vproc would hold on to
   *************************************************************************/
  uint32_t force_update_flag :1;
  /**************************************************************************
   * sync_timestamp = 0, OSD cmd is applied immediately regardless of current
   *   vin_cap_pts. It becomes most recent OSD that vproc would hold on to
   * sync_timestamp = 1, OSD cmd is applied based on pts and force_update_flag
   *   if cmd is late
   *************************************************************************/
  uint32_t sync_timestamp :1;

  uint32_t target_pts; /* cmd's target frame vin_cap_pts */

  /**************************************************************************
   * osd_region_batch_daddr is a dram address where 
   * the 1st osd_insert_buf_t starts
   *************************************************************************/
  uint32_t osd_region_batch_daddr;
} cmd_vproc_osd_insert_t; 

/// CMD_VPROC_SET_STREAMS_DELAY(0x0200000Eu)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id: 6;
  uint32_t rsvd0     : 26;

  uint32_t delay_in_ms[VPROC_STREAM_NUMS];

} cmd_vproc_set_streams_delay_t;

/// VPROC common commands
/// CMD_VPROC_MULTI_STREAM_PP (0x02000064)
// veffect stream configuration flags
#define VEFFECT_ROTATE_NONE       0u
#define VEFFECT_ROTATE_90_DEGREE  1u  // rotate=1
#define VEFFECT_ROTATE_270_DEGREE 2u  // rotate=1, hflip=1, vflip=1
#define VEFFECT_ROTATE_180_DEGREE 3u  // hflip=1,  vflip=1
#define VEFFECT_HFLIP             4u  // hflip=1
#define VEFFECT_VFLIP             5u  // vflip=1
#define VEFFECT_ROTATE_90_HFLIP   6u  // rotate=1, hflip=1
#define VEFFECT_ROTATE_90_VFLIP   7u  // rotate=1, vflip=1

typedef struct {
  uint32_t  is_alloc_out_buf           : 1; /* TRUE or FALSE flags */
  uint32_t  is_last_blending_channel   : 1;
  uint32_t  is_last_passthrough_channel: 1;
  uint32_t  is_last_copy_channel       : 1;

  uint32_t  num_of_post_r2y_copy_ops   : 3;
  uint32_t  num_of_y2y_blending_ops    : 3;

  uint32_t  rsvd0                      : 22;
} vproc_stream_cfg_t;


/**************************************************************************** 
 * use MODE_BLEND mctf as veffect alpha blending engine, no sync_cnt
 * msg->command_w0.cmd.sreg_win_in = MCTF_TARG_WIN_IN|MCTF_REF_WIN_IN
 * sec_cfg->sreg_win_out = MCTF_Y_WIN_OUT
 *
 * input streams:
 * first_input_buf_id region-->SMEM_WIN-->mctf_tar:
 * MCTF_DRAM_mctf_tar_y_INDEX
 * MCTF_DRAM_mctf_tar_uv_INDEX
 *
 * second_input_buf_id region -->SMEM_WIN-->mctf_ref:
 * MCTF_DRAM_mctf_ref_y_INDEX
 * MCTF_DRAM_mctf_ref_uv_INDEX
 *
 * alpha data-->SMEM_SBUF-->mctf_blend_alpha:
 * MCTF_DRAM_mctf_blend_alpha_y_INDEX
 * MCTF_DRAM_mctf_blend_alpha_uv_INDEX
 *
 * output streams:
 * mctf_out-->SMEM_WIN-->output_buf_id region:
 * MCTF_DRAM_mctf_out_y_INDEX
 * MCTF_DRAM_mctf_out_y_INDEX
 * 
 * blending_width/height is final blending out width/height
 * 
 * mctf_out = (alpha/255)*mctf_tar + (1- (alpha/255))*mctf_ref
 * 
 * alpha:0x00  out= mctf_ref
 * alpha:0x80  out= blend of mctf_tar and mctf_ref
 * alpha:0xFF  out= mctf_tar
 ***************************************************************************/
typedef struct
{
  uint32_t  output_buf_id     : 16;
  uint32_t  is_alloc_out_buf  : 1; /* 1: alloc buf, 0: use pre-alloc buf */
  uint32_t  is_rotate_1st_inp : 4; /* 0:no rotation, 1:90, 2:270, 3:180 4: hfllp, 5: vflip, 6: rotate+vflip, 6: rotate+hflip */
  uint32_t  is_rotate_2nd_inp : 4; /* 0:no rotation, 1:90, 2:270, 3:180 4: hfllp, 5: vflip, 6: rotate+vflip, 6: rotate+hflip */
  uint32_t  rsvd0 : 7;

  /* from Y2Y IDSP out or DRAM */
  uint16_t  first_input_buf_id;
  uint16_t  first_inp_x_ofs;
  uint16_t  first_inp_y_ofs;

  /* from DRAM only */
  uint16_t  second_input_buf_id;
  uint16_t  second_inp_x_ofs;
  uint16_t  second_inp_y_ofs;

  /* blending region dimension */
  uint16_t  blending_width;
  uint16_t  blending_height;

  uint16_t  output_x_ofs;
  uint16_t  output_y_ofs;

  /* alpha cfg1: constant blending fact */
  uint32_t  is_alpha_stream_value : 8;
  uint32_t  alpha_luma_value      : 8;
  uint32_t  alpha_u_value         : 8;
  uint32_t  alpha_v_value         : 8;

  /* alpha cfg2: alpha blending fact dma streams */
  uint16_t  alpha_luma_dram_pitch;
  uint16_t  alpha_chroma_dram_pitch;

  uint32_t  alpha_luma_dram_addr;
  uint32_t  alpha_chroma_dram_addr;
} vproc_y2y_blending_cfg_t;

typedef struct
{
  uint32_t output_buf_id    : 16;
  uint32_t is_alloc_out_buf : 1; /* 1: alloc buf, 0: use pre-alloc buf */
  uint32_t is_rotate        : 4;
  uint32_t rsvd             : 11;

  /* input start point reference to physical buffer */
  uint32_t input_start_x;
  uint32_t input_start_y;

  /* output start point reference to physical buffer */
  uint32_t output_start_x;
  uint32_t output_start_y;

  /* copy region size */
  uint32_t copy_width;
  uint32_t copy_height;
} vproc_post_r2y_copy_cfg_t;

typedef struct
{
  uint8_t   input_channel_num;
  uint8_t   input_stream_num;

  /* channel/stream veffect output buffer index */
  uint16_t  output_buf_id;
  
  vproc_stream_cfg_t stream_cfg;

  /* output start point reference to physical buffer */
  uint16_t  output_x_ofs;
  uint16_t  output_y_ofs;

  /* output size */
  uint16_t  output_width;
  uint16_t  output_height;

  /* dram base address of vproc_post_r2y_copy_cfg_t block */
  uint32_t  post_r2y_copy_cfg_address;

  /* dram base address of vproc_y2y_blending_cfg_t block */
  uint32_t  y2y_blending_cfg_address;

} vproc_pp_stream_cntl_t;

typedef struct
{
  uint32_t cmd_code;

  uint32_t chan_id            : 6;
  uint32_t effect_grp_id      : 2;
  uint32_t str_id             : 8;
  uint32_t final_output_buf_id: 16; /* buf_index of veffect final output */

  uint32_t output_dst   : 8; /* bitmask, 1:vout0; 2:vout1; 4:PIN */
  uint32_t num_of_inputs: 4; /* one vproc_pp_stream_cntl_t per stream */
  uint32_t final_output_presend : 1; 
  uint32_t rsvd2        : 19;

  /* dram base address of num_of_inputs vproc_pp_stream_cntl_t block */
  uint32_t input_pp_cfg_addr_array; 
} cmd_vproc_multi_stream_pp_t;

/// CMD_VPROC_SET_EFFECT_BUF_IMG_SZ (0x02000065u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t chan_id       : 6;
  uint32_t effect_grp_id : 2;
  uint32_t str_id        : 8;
  uint32_t rsvd1         : 16;

  uint32_t img_width  : 16;
  uint32_t img_height : 16;

  uint32_t img_x_ofs  : 16;
  uint32_t img_y_ofs  : 16;

  uint16_t ch_fmt;
} cmd_vproc_set_effect_buf_img_sz_t;

/// CMD_VPROC_MULTI_CHAN_PROC_ORDER (0x02000066u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t chan_id             : 6;
  uint32_t rsvd0               : 2;
  uint32_t str_id              : 8;
  uint32_t proc_order_check_en : 1;
  uint32_t grp_id              : 6;
  uint32_t free_run_grp        : 1;
  uint32_t is_runtime_change   : 1; /* using the VPROC common commands queue */
  uint32_t rsvd1               : 7;

  uint32_t cap_seq_no;     /* cap_seq_no from where onward the proc_order is followed */

  uint32_t num_of_vproc_channel; /* up to VPROC_MAX_CHAN_PER_GRP */

#define VPROC_CH_DISABLE_FLAG_MASK 0x80u /* MSB: vproc channel processing disable flag */
#define VPROC_CH_ID_MASK           0x3Fu /* 6 bits channel_id */
  uint8_t  proc_order[VPROC_MAX_CHAN_PER_GRP]; /* U8: disable flag(MSB) + chan_id */
  uint32_t proc_order_addr;
} cmd_vproc_multi_chan_proc_order_t;

/// CMD_VPROC_SET_VPROC_GRPING              0x02000067u
typedef struct
{
  uint32_t cmd_code;

  uint32_t num_of_vproc_groups  : 6; /* up to MAX_VPROC_GROUPS */
  uint32_t rsvd0                : 2;
  uint32_t max_vprocs_per_group : 8; /* indicate array size per group */
  uint32_t free_run_grp         :16; /* bitmask per group (up to MAX_VPROC_GROUPS) */

  uint32_t num_vprocs_per_group_addr;
  uint32_t vprocs_in_a_group_addr;
} cmd_vproc_set_vproc_grping;

///////////////////////////////////////////////////////////////////////////////
// VPROC working modes
typedef uint8_t vproc_working_mode_t;

#define VPROC_TIMER_MODE  10u
#define VPROC_ACTIVE_MODE 20u
#define VPROC_PAUSE_MODE  30u

///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/// MSG_VPROC_STATUS (0x82000001)
typedef struct
{
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // refer to vproc_working_mode_t
                                   // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
                                   // 30: VPROC_PAUSE_MODE
  uint8_t  pass_step_id;
  uint8_t  iso_mode;               // IMG_MODE_XXXXXX (refer to #define's under aik_img_mode_t)

  uint32_t cap_seq_no;
  uint32_t batch_cmd_id;

  uint32_t ext_buf_idc;

  uint32_t is_last_tile         : 1;
  uint32_t metadata_status      : 3;
  uint32_t group_id             : 2;
  uint32_t vin_id               : 4;
  uint32_t vp_msg_error         : 2; // bit[0]: vp msg queue full, bit[1]: vp msg sync cnt full
  uint32_t reserved_0           :20;

  uint8_t  stitch_tile_num_x; // total number slices of X direction
  uint8_t  stitch_tile_num_y; // total number slices of Y direction
  uint8_t  tile_x_idx;        // current tile X direction index
  uint8_t  tile_y_idx;        // current tile Y direction index

  uint32_t sect_mask;            // indicate which section included
  uint32_t idsp_cfg_hdr_addr[8]; // [IDSP_JOB_SEC_NUM]

  uint32_t vproc_done_hw_pts;    // This is Audio PTS, which is audio clock when DSP finishes procssing a frame
  uint32_t vin_cap_done_pts;
  uint32_t job_done_pts;

  uint32_t ext_data_daddr[2];
  uint32_t grp_cmd_buf_id;
  uint32_t job_start_pts;

  uint32_t vproc_seq_no;
} msg_vproc_status_t;

/// MSG_VPROC_AAA_STATUS (0x82000002)
typedef struct
{
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
  uint8_t  raw_compression_mode;
  uint8_t  iso_mode;               // IMG_MODE_XXXXXX

  uint32_t cap_seq_no;
  uint32_t batch_cmd_id;

  uint32_t tile_params_addr;

  uint32_t raw_buf_addr;
  uint16_t raw_buf_pitch;
  uint16_t raw_buf_width;
  uint16_t raw_buf_height;

  uint8_t  stitch_tile_num_x; // total number slices of X direction
  uint8_t  stitch_tile_num_y; // total number slices of Y direction

  uint8_t  tile_x_idx;        // current tile X direction index
  uint8_t  tile_y_idx;        // current tile Y direction index

  uint16_t tile_start_x;
  uint16_t tile_start_y;

  uint16_t tile_w;
  uint16_t tile_h;

  uint32_t cfa_aaa_stat_buf_addr;
  uint32_t cfa_aaa_stat_buf_length;

  uint32_t rgb_aaa_stat_buf_addr;
  uint32_t rgb_aaa_stat_buf_length;

  uint32_t vin_stat_buf_addr;       // Vin Stat Main
  uint32_t vin_stat_buf_length;

  uint32_t hdr_vin_stat_buf_addr;   // Vin Stat HDR
  uint32_t hdr_vin_stat_buf_length;

  uint32_t hdr2_vin_stat_buf_addr;  // Vin Stat HDR2
  uint32_t hdr2_vin_stat_buf_length;

  uint32_t aaa_hl_pts;              // 3A high-level pts
  uint32_t aaa_hw_duration;         // 3A HW duration
} msg_vproc_aaa_status_t;

/// MSG_VPROC_EXT_STATUS (0x82000003)
typedef struct
{
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
  uint8_t  pass_step_id;           // refer to pass_step_id_t
  uint8_t  is_last_tile;

  uint32_t cap_seq_no;
  uint32_t batch_cmd_id;

  uint32_t sect_mask;            // indicate which section included
  uint32_t idsp_cfg_hdr_addr[8]; // [IDSP_JOB_SEC_NUM]
  uint32_t vproc_done_hw_pts;    // This is Audio PTS, which is audio clock when DSP finishes procssing a frame

  uint32_t vin_cap_done_pts;
  uint32_t job_done_pts;
  uint32_t grp_cmd_buf_id;

} msg_vproc_ext_status_t;



/////////////////////////////////////////////////////////////////////////////
// VPROC_XXX_EXT_DATA structures for VPROC step/passes
// IMG_PASS_C2Y
typedef struct
{
  uint32_t sec2_cfg_addr;
  uint32_t sec4_cfg_addr;

  uint32_t sec2_cfg_len;
  uint32_t sec4_cfg_len;

  image_buffer_desc_t c2y_luma_out;
  image_buffer_desc_t c2y_chroma_out;
  image_buffer_desc_t compressed_raw_out;
} vproc_li_c2y_ext_data_t;

// IMG_PASS_YUYV2Y
typedef struct
{
  uint32_t sec6_cfg_addr;
  uint32_t sec7_cfg_addr;

  uint32_t sec6_cfg_len;
  uint32_t sec7_cfg_len;

  image_buffer_desc_t yuyv2y_luma_out;
  image_buffer_desc_t yuyv2y_chroma_out;

  uint32_t sec2_cfg_addr;
  uint32_t sec2_cfg_len;
  image_buffer_desc_t sec2_luma_out;
} vproc_yuyv2y_ext_data_t;

//  IMG_STEP_WARP_MCTF: LC_Warp+MCTF(M2)
typedef struct
{
  image_buffer_desc_t sec3_luma_out;
  image_buffer_desc_t sec3_chroma_out;
  image_buffer_desc_t sec3_me1_out;
  image_buffer_desc_t sec3_me0_out;
  image_buffer_desc_t sec5_luma_out;
  image_buffer_desc_t sec5_chroma_out;
  image_buffer_desc_t sec5_me1_out;
  image_buffer_desc_t sec5_me0_out;
  image_buffer_desc_t sec6_luma_out;
  image_buffer_desc_t sec6_chroma_out;
  image_buffer_desc_t sec6_me1_out;
  image_buffer_desc_t sec6_me0_out;
  image_buffer_desc_t sec7_luma_out;
  image_buffer_desc_t sec7_chroma_out;
  image_buffer_desc_t sec7_me1_out;
  image_buffer_desc_t sec7_me0_out;
  image_buffer_desc_t mctf_luma_out;
  image_buffer_desc_t mctf_chroma_out;
  image_buffer_desc_t mcts_luma_out;
  image_buffer_desc_t mcts_chroma_out;
} vproc_li_warp_mctf_prev_ext_data_t;

typedef struct
{
  image_buffer_desc_t sec2_luma_out;
  image_buffer_desc_t sec2_chroma_out;
  image_buffer_desc_t sec5_luma_out;
  image_buffer_desc_t sec5_chroma_out;
  image_buffer_desc_t sec6_luma_out;
  image_buffer_desc_t sec6_chroma_out;
  image_buffer_desc_t sec7_luma_out;
  image_buffer_desc_t sec7_chroma_out;
  image_buffer_desc_t sec9_luma_out;
  image_buffer_desc_t sec9_chroma_out;
  image_buffer_desc_t mctf_luma_out;
  image_buffer_desc_t mctf_chroma_out;
  image_buffer_desc_t mcts_luma_out;
  image_buffer_desc_t mcts_chroma_out;
} sproc_hi_ext_data_t;

typedef struct
{
  image_buffer_desc_t sec8_luma_out;
  image_buffer_desc_t sec8_chroma_out;
} vproc_osd_blend_ext_data_t;

/// MSG_VPROC_EXT_DATA_STATUS (0x82000004)
typedef struct {
  vproc_li_c2y_ext_data_t           li_c2y;
} vproc_ext_data_t;


/// MSG_VPROC_EXT_DATA_STATUS (0x82000004)
typedef struct
{
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
                                   // 30: VPROC_PAUSE_MODE
  uint8_t  pass_step_id;           // preblend, hier_land_det outputs (refer to pass_step_id_t)
  uint8_t  reserved_0;

  uint32_t cap_seq_no;
  uint32_t batch_cmd_id;

  uint8_t  stitch_tile_num_x; // total number slices of X direction
  uint8_t  stitch_tile_num_y; // total number slices of Y direction

  uint8_t  tile_x_idx;        // current tile X direction index
  uint8_t  tile_y_idx;        // current tile Y direction index

  uint16_t tile_start_x;
  uint16_t tile_start_y;

  uint16_t tile_w;
  uint16_t tile_h;

  // preblend, hierachical, land detect, etc. intermediate output Y/UV/ME buffer descriptors.
  vproc_ext_data_t ext_data;

} msg_vproc_ext_data_status_t;


/// MSG_VPROC_COMP_OUT_STATUS (0x82000005)
typedef struct {
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
  uint8_t  compression_out_enable;
  uint8_t  reserved_0;

  uint32_t cap_seq_no;
  uint32_t batch_cmd_id;

  image_buffer_desc_t compressed_raw_out;
} msg_vproc_comp_out_status_t;

/// MSG_VPROC_EFFECT_DATA_STATUS (0x82000006)
typedef struct {
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
  uint8_t  stream_id;              // vproc_stream_idx_t
  uint8_t  group_id;
  
  uint32_t cap_seq_no;
   
  image_buffer_desc_t effect_luma;
  image_buffer_desc_t effect_chroma;

  /* me0/1 info of VPROC_MAIN_STREAM_IDX and VPROC_PIP_STREAM_IDX */
  image_buffer_desc_t effect_me0;
  image_buffer_desc_t effect_me1;

  /* audio ticks right before DSP sends VCAP interrupt */
  uint32_t vcap_audio_clk_counter;

  uint32_t is_ext      : 1; // for sanity check
  uint32_t is_me01_ext : 1; // for sanity check
  uint32_t rsvd1       :30;

  uint32_t vproc_seq_no;
  uint32_t source_fov_bitmask;

} msg_vproc_effect_data_status_t;

/// MSG_VPROC_DELAYLINE_OSD_STATUS (0x82000007)
typedef struct {
  uint32_t msg_code;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE, 20: VPROC_ACTIVE_MODE
  uint8_t  stream_id;
  uint8_t  rsvd0;
  
  uint32_t osd_sync_error : 1;
  uint32_t rsvd1          :31;

  uint32_t cap_seq_no;
  uint32_t vin_cap_done_pts;
  uint32_t y2y_done_pts;           // y2y yuv done pts
  uint32_t job_start_pts;          // osd job start pts
  uint32_t job_done_pts;           // osd job done pts
  uint32_t vproc_done_pts;         // vproc osd done pts

  uint32_t user_id;                //read from user_id in osd_insert_buf_t 
} msg_vproc_delayline_osd_status_t;


/////////////////////////////////////////////////////////////////////////////
typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id     : 8;
  uint32_t rsvd       : 24;
} cmd_vin_hdr_t;


// CMD_VIN_START (0x04000001)

typedef struct
{
  uint32_t addr;
  uint32_t id;
  uint32_t size;
} batch_cmd_set_info_t;

#define MAX_VIN_CAP_SLICE_NUM     10u

typedef struct
{
  uint16_t chan_id;
  uint16_t x_offset;
  uint16_t y_offset;
  uint16_t width;
  uint16_t height;
  uint8_t  xpitch;              // pitch multipiler. set 1 for normal fov. set 2 for two interlaced fov.
  uint8_t  rsvd0;

  uint32_t hdr_intlac_mode  : 1; // 0: [(EXP,FOV)] = [(0,0);(0,1);(1,0);(1,1)]. 1: [(0,0);(1,0);(0,1);(1,1)]
  uint32_t is_fov_active    : 1;
  uint32_t is_last_fov      : 1;
  uint32_t rsvd1            : 29;

  batch_cmd_set_info_t  batch_cmd_set_info;
  uint16_t slice_term_cap_line[MAX_VIN_CAP_SLICE_NUM];

  uint32_t rsvd2[4]; // rsv some extra space
} vin_fov_cfg_t;

typedef struct
{
  uint16_t vin_id;
  uint16_t chan_id;
  batch_cmd_set_info_t batch_cmd_set_info;
} vin_fov_batch_cmd_set_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_start_output_dest_t;

// write out captured vin data to external buffer frame. The captured frame will not pass on to any raw2yuv or yuv2yuv processing unit.
#define VIN_START_OUTPUT_DEST_EXT 1u
// write out captured vin data to internal buffer frame. The captured data will pass on to the video processing pipeline for further processing to preview.
#define VIN_START_OUTPUT_DEST_INT 2u

///////////////////////////////////////////////////////////////////////////////
#define VIN_RPT_FRM_KEEP     0u
#define VIN_RPT_FRM_DROP     1u
#define VIN_RPT_FRM_REPEAT   2u
#define VIN_RPT_FRM_USE_DEF  3u  // use default

// capture_compl_cntl_msg box definition
typedef struct
{
  // system must fill the rpt_frm_cntrl in time (frm_done_poll_msec) 
  // to tell ucode to keep/drop/repeat/use_default for the current capture.
  // ucode ack this choice back to system by VCAP_SENSOR_RAW_CAPTURE_STATUS_MSG.
  uint32_t rpt_frm_cntrl;           // see defines above for valid values

} sys2dsp_vin_cntrl;

typedef struct
{
  // W0
  uint32_t cmd_code;

  // W1
  uint32_t vin_id                   : 8;
  uint32_t cmd_msg_decimation_rate  : 8;
  uint32_t vin_smem_to_vproc        : 1;
  uint32_t vin_line_sync_to_vproc   : 1; // obsolete
  uint32_t is_compression_en        : 1;
  uint32_t send_input_data_type     : 2;  // 0: raw, 1: yuv422, 2:yuv420
  uint32_t is_reset_frm_sync        : 1;
  uint32_t is_auto_frm_drop_en      : 1; /* no halt if vin fb is run out */
  uint32_t is_send_msg_to_main_msg_q: 1;
  uint32_t is_check_timestamp       : 1;
  uint32_t cap_slice_num            : 4;
  uint32_t is_slice_cfg_present     : 1;
  uint32_t is_vin_cap_presend       : 1;
  uint32_t no_reset_fp_shared_res   : 1;  /* on/off the resetting of resources (fbp) */ 
                                          /* that are shared with other FPs */
                                          /* =1, for example, when is_pic_info_cache_enabled=1 */ 

  // W2
  uint8_t  fov_num;
  uint8_t  skip_frm_cnt;
  uint8_t  output_dest;
  uint8_t  input_source;                // 0: sensor in, 1: ext data (raw/yuv422/yuv420) in, 2: vin yuv422 input

  // W3
  uint16_t vin_cap_width;
  uint16_t vin_cap_height;

  // W4
  uint32_t  blk_sz            : 8;
  uint32_t  mantissa          : 8;
  uint32_t vin_ce_out_reset   : 1;
  uint32_t rsvd1              :15;

  // W5
  uint32_t fov_cfg_tbl_daddr;           // for input parameters

  // W6, W7, W8
  batch_cmd_set_info_t batch_cmd_set_info;

  // W9
  uint32_t vin_poll_intvl;

  // W10
  uint32_t allow_raw_cap_skip;

  // W11
  uint32_t default_raw_image_address;   // default luma image for yuv422 case

  // W12
  uint32_t default_ce_image_address;    // default chroma image for yuv422 case
  
  // W13
  uint16_t default_raw_image_pitch;   // default luma image pitch for yuv422 case
  uint16_t default_ce_image_pitch;    // default chroma image pitch for yuv422 case
    
  // W14 
  uint32_t capture_compl_cntl_msg_addr;
  
  // W15 
  uint32_t capture_time_out_msec;      // 0: no time out

  // W16 
  uint32_t compl_cntl_msg_update_time_msec; 

  // W17 
  uint32_t short_frame_drop_thresh_ticks; // in the units of the audio clock ticks

} cmd_vin_start_t;

/// CMD_VIN_IDLE (0x04000002)
typedef struct
{
  uint32_t  cmd_code;

  uint32_t  vin_id  : 8;
  uint32_t  rsvd0   : 24;
} cmd_vin_idle_t;

/// CMD_VIN_SET_RAW_FRM_CAP_CNT (0x04000003)
typedef struct
{
  uint32_t  cmd_code;

  uint32_t  vin_id  : 8;
  uint32_t  rsvd0   : 24;

  uint32_t  cap_seq_no;
} cmd_vin_set_raw_frm_cap_cnt_t;

/// CMD_VIN_SET_EXT_MEM (0x04000004)
typedef struct
{
  uint32_t    cmd_code;

  uint32_t    vin_id          : 8;
  uint32_t    allocation_mode : 2;
  uint32_t    overflow_cntl   : 2;
  uint32_t    chroma_format   : 2; // yuv_chroma_fmt_t
  uint32_t    vin_yuv_enc_fbp_disable : 1;
  uint32_t    reserved        : 17;

  uint8_t     memory_type;           // see vin_ext_mem_type_t. 0 for raw buf; 1: YUV422; 2: HDS 3: ME
  uint8_t     allocation_type;
  uint16_t    num_frm_buf;

  uint16_t    buf_pitch;
  uint16_t    buf_width;
  uint16_t    buf_height;
  uint16_t    luma_img_ofs_x;        // luma image offset x
  uint16_t    luma_img_ofs_y;        // luma image offset y
  uint16_t    chroma_img_ofs_x;      // chroma image offset x
  uint16_t    chroma_img_ofs_y;      // chroma image offset y
  uint16_t    img_width;             // image width
  uint16_t    img_height;            // image height

  uint16_t    aux_pitch;
  uint16_t    aux_width;
  uint16_t    aux_height;
  uint16_t    aux_img_ofs_x;         // aux image offset x
  uint16_t    aux_img_ofs_y;         // aux image offset y
  uint16_t    aux_img_width;         // aux image width
  uint16_t    aux_img_height;        // aux image height

  uint32_t    buf_addr;
} cmd_vin_set_ext_mem_t;

/// CMD_VIN_SEND_INPUT_DATA (0x04000005)
typedef struct
{
  uint32_t    cmd_code;

  uint32_t vin_id                          : 8;
  uint32_t is_compression_en               : 1;
  uint32_t blk_sz                          : 3;
  uint32_t mantissa                        : 4;
  uint32_t chan_id                         : 8;
  uint32_t send_data_dest                  : 4; // 0:VIN_SEND_IN_DATA_DEST_VPROC, 1:VIN_SEND_IN_DATA_DEST_ENC
  uint32_t encode_start_idc                : 1;  // for h264 encoding
  uint32_t encode_stop_idc                 : 1;  // for h264 encoding
  uint32_t reserved                        : 2;                                

  uint32_t raw_frm_cap_cnt;
  uint32_t input_data_type;
  uint32_t ext_fb_idx;
  uint32_t ext_ce_fb_idx; // me buf idx for direct to enc h264 case

  batch_cmd_set_info_t batch_cmd_set_info;

  uint32_t vproc_hflip_control             : 1;
  uint32_t vproc_vflip_control             : 1;
  uint32_t vproc_rotation_control          : 1;
  uint32_t vout_hflip_control              : 1;
  uint32_t vout_vflip_control              : 1;
  uint32_t vout_rotation_control           : 1;
  uint32_t reserved_1                      : 26;

  uint32_t hw_pts;  // for h264 encoding
} cmd_vin_send_input_data_t;

#define VIN_CAP_TO_EXT_BUF_NO_LIMIT 0xFFFFFFFFu

#define VIN_CAP_TO_EXT_IN_TYPE_RAW      0x0u // raw input date type
#define VIN_CAP_TO_EXT_IN_TYPE_YUV422   0x1u // yuv422 input date type

/// CMD_VIN_INITIATE_RAW_CAP_TO_EXT_BUF (0x04000006)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id                 : 8;
  uint32_t rsvd0                  : 24;

  uint32_t raw_compression_type   : 8;
  uint32_t input_data_type        : 8;   // 0: raw, 1: yuv422
  uint32_t rsvd1                  : 16;

  uint32_t stop_condition;              // frames to capture, 0xffffffff means no limit

  uint8_t  raw_cap_done_sw_ctrl;        // 0: to IDLE, 1: to internal buffer
  uint8_t  raw_buffer_proc_ctrl;        // 0: no raw2yuv, 1: pass to raw2yuv
  uint8_t  raw_cap_sync_event;
  uint8_t  rsvd2;
} cmd_vin_initiate_raw_cap_to_ext_buf_t;

///////////////////////////////////////////////////////////////////////////////
// Event mask bits
typedef uint8_t event_mask_bit_pos_t;
/*
#define EXT_MEM_TYPE_MAIN   0u
#define EXT_MEM_TYPE_LN_DEC 1u
#define EXT_MEM_TYPE_PREV   2u
#define EXT_MEM_TYPE_HIER_0 3u
#define EXT_MEM_TYPE_NUM    4u
*/
#define BIT_POS_EXT_MAIN        0u
#define BIT_POS_EXT_PREV_A      1u
#define BIT_POS_EXT_PREV_B      2u
#define BIT_POS_EXT_PREV_C      3u
#define BIT_POS_EXT_LN_DEC      4u
#define BIT_POS_EXT_HIER_0      5u
#define BIT_POS_EXT_COMP_RAW    6u
#define BIT_POS_VPROC_RPT       7u
#define BIT_POS_VPROC_SKIP      8u
#define BIT_POS_ENC_START       9u
#define BIT_POS_ENC_STOP        10u
#define BIT_POS_EXT_MAIN_ME     11u
#define BIT_POS_EXT_PREV_A_ME   12u
#define BIT_POS_EXT_PREV_B_ME   13u
#define BIT_POS_EXT_PREV_C_ME   14u
#define BIT_POS_EXT_MCTS        15u
#define BIT_POS_EXT_MCTF        16u


///////////////////////////////////////////////////////////////////////////////

/// CMD_VIN_ATTACH_EVENT_TO_RAW (0x04000007)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id : 8;
  uint32_t ch_id  : 8;
  uint32_t is_indep_fov_control : 1;
  uint32_t rsvd0  : 15;

  uint32_t event_mask;

  // vin repeat send M pictures; decimate by N
  uint32_t repeat_cnt;
  uint32_t time_duration_bet_rpts;
  uint32_t fov_mask_valid;
} cmd_vin_attach_event_to_raw_t;


/// CMD_VIN_CMD_MSG_DECIMATION_RATE (0x04000008)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id : 8;
  uint32_t rsvd0  : 24;

  uint8_t decimation_rate;

} cmd_vin_cmd_msg_dec_rate_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_ce_out_format_t;

#define VIN_CE_OUT_FORMAT_DEINTERLEAVE 0u
#define VIN_CE_OUT_FORMAT_INTERLEAVE   1u

///////////////////////////////////////////////////////////////////////////////

/// CMD_VIN_CE_SETUP (0x04000009)
typedef struct
{
   uint32_t cmd_code;
   uint32_t vin_id                  : 8;
   uint32_t number_ce_exp           : 2;   // numbers of ce outputs : should matched with numbers of exposures.
   uint32_t primary_out_exp         : 3;   // 3exps out enable: bit0->main, bit1->hdr, bit2->hdr2
   uint32_t secondary_out_exp       : 3;   // 3exps out enable: bit0->main, bit1->hdr, bit2->hdr2
   uint32_t ce_out_format           : 1;     // 0: de-interlace 1: interlaced
                                            // may always set to be 1,
   uint32_t rsvd                    : 15;

   uint16_t ce_width;       // ce_width  = (source_w*2/ds_factor+3)>>2<<2
   uint16_t ce_height;      // ce_height = source_h
} cmd_vin_ce_setup_t;

//#define VIN_HDR_MAX_EXP_NUM 4

///////////////////////////////////////////////////////////////////////////////
// Definition of the index of exp_vert_offset_table
typedef uint8_t vin_hdr_exp_vert_offset_idx_t;

#define VIN_HDR_OFFSET_TABLE_MAIN_IN 0u
#define VIN_HDR_OFFSET_TABLE_HDR_IN  1u
#define VIN_HDR_OFFSET_TABLE_HDR2_IN 2u
#define VIN_HDR_MAX_EXP_NUM          3u

///////////////////////////////////////////////////////////////////////////////

/// CMD_VIN_HDR_SETUP (0x0400000a)
typedef struct
{
   uint32_t cmd_code;

   uint32_t vin_id                      : 8;
   uint32_t num_exp_minus_1             : 8;  // numbers of exposures - 1
   uint32_t num_hdr_blends              : 8;  // 0: blend off, 1: exp0+exp1, 2: exp0+exp1+exp2
   uint32_t rsvd                        : 8;


   uint32_t aggregate_raw_frame_width;
   uint32_t aggregate_raw_frame_height;


   uint32_t source_width                : 16; // individual raw width
   uint32_t source_height               : 16; // individual raw height

   uint32_t bayer_pattern               : 3;  // for sec10, 15's idspdrv_bayer_pattern()
   uint32_t rsvd2                       : 29;

   uint16_t exp_vert_offset_table[VIN_HDR_MAX_EXP_NUM]; // see Metin's mail notes below

   /* from Metin's mail notes ***********************************

    2-exposure HDR:
    sensor_raw_main_in: Exposure 0 (long)
    sensor_raw_hdr_in: Exposure 1 (short)

    3-exposure HDR:
    sensor_raw_main_in: Exposure 1 (middle)
    sensor_raw_hdr_in: Exposure 2 (short)
    sensor_raw_hdr2_in: Exposure 0 (long)

    *************************************************************

    exp_vert_offset_table[0] : the raw offset of which exposure should go to sensor_raw_main_in
    exp_vert_offset_table[1] : the raw offset of which exposure should go to sensor_raw_hdr_in
    exp_vert_offset_table[2] : the raw offset of which exposure should go to sensor_raw_hdr2_in

    ***************************************************************/
} cmd_vin_hdr_setup_t;

/// CMD_VIN_SET_FRM_LVL_FLIP_ROT_CONTROL  (0x0400000b)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id                   : 8;
  uint32_t fov_id                   : 8;

  // the sizes of variables need to be
  // the exponent of 2 bytes
  uint32_t rsvd0                    : 16;

  uint32_t vproc_hflip_control      : 2;
  uint32_t vproc_vflip_control      : 2;
  uint32_t vproc_rotation_control   : 4;
  uint32_t vout_hflip_control       : 2;
  uint32_t vout_vflip_control       : 2;
  uint32_t vout_rotation_control    : 4;
  uint32_t rsvd1                    : 16;
} cmd_vin_set_frm_lvl_flip_rot_control_t;

typedef struct
{
  uint16_t chan_id;             // get layout from imginfo of vin_fov_fbp_id[chan_id]
  uint16_t img_x_ofset;
  uint16_t img_y_ofset;
  uint16_t img_width;
  uint16_t img_height;
  uint8_t  xpitch;              // pitch multipiler. set 1 for normal fov. set 2 for two interlaced fov.
  uint8_t  rsvd0;

  uint32_t hdr_intlac_mode  : 1; // 0: [(EXP,FOV)] = [(0,0);(0,1);(1,0);(1,1)]. 1: [(0,0);(1,0);(0,1);(1,1)]
  uint32_t is_fov_active    : 1;
  uint32_t is_last_fov      : 1;
  uint32_t rsvd1            : 29;

} fov_layout_t;

/// CMD_VIN_SET_FOV_LAYOUT (0x0400000c)

typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id                 : 8;
  uint32_t rsvd0                  : 24;

  uint32_t num_of_fovs;
  uint32_t fov_lay_out_ptr;
} cmd_vin_set_fov_layout_t;

///CMD_VIN_SET_FRM_VPROC_DELAY (0x0400000d)
typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id                 : 8;
  uint32_t rsvd0                  : 24;

  uint32_t delay_time_msec;
} cmd_vin_set_frm_vproc_delay_t;

#define VIN_MAX_FOV_NUM (16u) //max num of fovs per vin

/// CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM (0x0400000e)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id   : 8;
  uint32_t fov_num  : 8;
  uint32_t rsvd0    : 16;

  uint32_t fov_batch_cmd_set_addr; /* vin_fov_batch_cmd_set_t */

} cmd_vin_attach_proc_cfg_to_cap_frm_t;

#define MAX_SIDEBAND_INFO_PER_CMD (8u)
/// CMD_VIN_ATTACH_SIDEBAND_INFO_TO_CAP_FRM (0x0400000f)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id            : 8;
  uint32_t sideband_info_num : 8;  // total number of sideband_info in command
  uint32_t rsvd0             : 16;

  uint32_t per_fov_sideband_info_addr[MAX_SIDEBAND_INFO_PER_CMD];       // each array element is a pointer to a structure as defined by sideband_info_t
} cmd_vin_attach_sideband_info_to_cap_frm_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_simu_vproc_dram_in_fmt_t;

#define RAW_IMG_DATA_TO_VPROC    0u
#define YUV420_IMG_DATA_TO_VPROC 1u

///////////////////////////////////////////////////////////////////////////////

/// CMD_VIN_VCAP_DRAM_IN_SETUP (0x04000010)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id                          : 8;
  uint32_t channel_id                      : 8;
  uint32_t cap_iso_mode                    : 8;
  uint32_t vcap_is_frm                     : 1;
  uint32_t is_raw_compressed               : 1;
  uint32_t vin_simu_vproc_dram_in_fmt      : 2; // refer to vin_simu_vproc_dram_in_fmt_t
                                               // 0: raw image data send to VPROC 1: yuv420 image data send to VPROC
  uint32_t rsvd0                           : 4;

  uint16_t vcap_width;
  uint16_t vcap_height;
  uint16_t vcap_hdec_width;
  uint32_t ik_cfg_addr;
  uint32_t ik_cfg_id;
} cmd_vin_vcap_dram_in_setup_t;


/// CMD_VIN_VOUT_LOCK_SETUP (0x04000011u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id             : 8;
  uint32_t en_lock            : 8;
  uint32_t reset_lock_sync    : 8;
  uint32_t rsvd0              : 8;

} cmd_vin_vout_lock_setup_t;

typedef struct 
{
    uint32_t fov_id : 8;
    uint32_t rsvd0  :24;
    uint32_t frame_no;
    uint32_t capture_pts_val;
} lyft_metadata_header_t;

#define METADATA_NULL            0u
#define METADATA_NOT_READY       1u
#define METADATA_HEADER_READY    2u

/// CMD_VIN_ATTACH_METADATA   (0x04000012u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id                 : 8;
  uint32_t fov_id                 : 8;
  uint32_t is_metadata_attach     : 1;
  uint32_t rsvd0                  : 15;

  uint32_t metadata_addr;   // metadata_addr layout
                            /* ***************************************************************************
                             * sizeof(uint32_t)              : metadata size in bytes, system fill this 
                             *                                 area once the real metadata content ready;
                             *                                 otherwise fill with 0
                             * sizeof(lyft_metadata_header_t): ucode fill this area if
                             *                                 the metadata size ready
                             * the real metadata content
                             * ***************************************************************************/
  uint32_t rsvd1;
} cmd_vin_attach_metadata_t;

/// CMD_VIN_TEMPORAL_DEMUX_SETUP  (0x04000013u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t  vin_id                : 8;
  uint32_t  temporal_demux_num    : 8;
  uint32_t  en_temporal_demux     : 1;
  uint32_t  rsvd0                 : 15;

  uint8_t   temporal_demux_tar[MAX_VIN_TEMPORAL_DEMUX_NUM];

  uint32_t rsvd1;
} cmd_vin_temporal_demux_setup_t;

#define MAX_SIDEBAND_INFO_PER_CMD (8u)

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_raw_capture_status_t;

#define TIMER_MODE   10u
#define CAPTURE_MODE 20u

///////////////////////////////////////////////////////////////////////////////

#define RAW_COMP_CFG_DATA_SIZE 32u
/// MSG_VIN_SENSOR_RAW_CAPTURE_STATUS (0x84000001)
typedef struct
{
  uint32_t    msg_code;

  uint32_t    vin_id              : 8;
  uint32_t    raw_capture_status  : 8; // refer to vin_raw_capture_status_t
  uint32_t    is_capture_time_out : 1;
  uint32_t    rpt_frm_cntrl       : 4;
  uint32_t    is_external_raw_buf_from_system : 2;
  uint32_t    is_external_ce_buf_from_system  : 2;
  uint32_t    raw_cap_buf_ch_fmt  : 4; // for send to enc case, 0:YUV_MONO, 1:YUV_420, 2:YUV_422
  uint32_t    rsvd0               : 3;

  uint32_t    raw_cap_cnt;
  uint32_t    raw_cap_buf_addr;
  uint16_t    raw_cap_buf_pitch;
  uint16_t    raw_cap_buf_width;
  uint16_t    raw_cap_buf_height;
  uint16_t    rsvd1;

  uint32_t    raw_cap_buf_fid;
  uint32_t    pts;
  uint32_t    pts_diff;

  batch_cmd_set_info_t batch_cmd_set_info;

  uint32_t    event_mask;
  uint32_t    config_addr;
  uint32_t    ce_cap_buf_fid;
  uint32_t    ce_cap_buf_addr;
  uint16_t    ce_cap_buf_pitch;
  uint16_t    ce_cap_buf_width;
  uint16_t    ce_cap_buf_height;
  uint16_t    rsvd2;

  uint32_t    frm_vproc_dalay_time_ticks;

  uint32_t    raw_repeat_default_buf_addr;  // luma part for yuv422 case
  uint32_t    ce_repeat_default_buf_addr;   // chroma part for yuv422 case

  uint8_t  raw_cap_buf_num_tot; // yuv buf if yuv422 capture case
  uint8_t  raw_cap_buf_num_free; // yuv buf if yuv422 capture case 
  uint8_t  ce_cap_buf_num_tot;
  uint8_t  ce_cap_buf_num_free;

  uint32_t  current_cap_slice_idx       :4;  // slice capture
  uint32_t  cap_slice_num               :4;  // slice capture
  uint32_t  temporal_demux_tar_vin_id   :8;
  uint32_t  rsvd3                       :16;

} msg_vin_sensor_raw_capture_status_t;

// MSG_VOUT_STATUS (0x85000001)
typedef struct
{
  uint32_t    msg_code;

  uint32_t    vout_id              : 8;  // 0 = vout A, 1 = vout B
  uint32_t    vout_status          : 8;
  uint32_t    is_vin_vout_locked   : 1;
  uint32_t    is_in_timeout_mode   : 1;
  uint32_t    polarity             : 1;
  uint32_t    vout_in_safe_state   : 1;
  uint32_t    reserved_0           : 12;

  uint32_t    vout_disp_fid;
  uint32_t    vout_disp_cmd_buf_id;
  uint32_t    vout_disp_luma_addr;
  uint32_t    vout_disp_chroma_addr;
  uint32_t    vout_disp_start_time;
  uint32_t    vout_disp_done_time;
  uint32_t    vout_buffer_pitch;
  uint32_t    vout_img_width;
  uint32_t    vout_img_height;
} msg_vout_status_t;

///////////////////////////////////////////////////////////
// Decoder structures
///////////////////////////////////////////////////////////
/*
 * Decoder GOP Header
 * start_code                :32                  
 * nal_hdr                   : 8  (16 bits for hevc)
 * version_main              : 8                   
 * version_sub               : 8                   
 * gop_skip_first_I          : 1                   
 * gop_skip_last_I           : 1
 * num_units_in_tick_high    :16
 * reserved_one              : 1
 * num_units_in_tick_low     :16
 * reserved_one              : 1
 * time_scale_high           :16
 * reserved_one              : 1
 * time_scale_low            :16
 * reserved_one              : 1
 * ------- version_sub=1 -------
 * gop_first_I_pts_b31_16    :16
 * reserved_one              : 1
 * gop_first_I_pts_b15_0     :16
 * reserved_one              : 1
 * gop_n                     : 8
 * gop_m                     : 4
 * reserved                  : 4
 * ------- version_sub=2 -------
 * gop_first_I_pts_b31_16    :16
 * reserved_one              : 1
 * gop_first_I_pts_b15_0     :16
 * reserved_one              : 1
 * gop_first_I_pts_b63_48    :16
 * reserved_one              : 1
 * gop_first_I_pts_b47_32    :16
 * reserved_one              : 1
 * gop_n                     : 8
 * gop_m                     : 4
 * reserved                  : 2
 */

typedef uint8_t dec_codec_type_t;
#define DEC_CODEC_TYPE_AVC     0u
#define DEC_CODEC_TYPE_HEVC    1u
#define DEC_CODEC_TYPE_JPEG    2u

typedef struct
{
  uint32_t cmd_code;

  uint8_t  decoder_id;
  uint8_t  codec_type; // 0: avc; 1: hevc; 2: jpeg(still decode only)
  uint8_t  reserved[2];
} cmd_dec_hdr_t;

typedef struct
{
  uint32_t msg_code;
  uint32_t time_code;
  uint32_t latest_clock_counter;

  uint8_t  decoder_id;
  uint8_t  codec_type; // 0: avc; 1: hevc; 2: jpeg(still decode only)
  uint8_t  reserved[2];
} msg_dec_hdr_t;

typedef struct
{
  uint8_t  valid;
  uint8_t  ch_fmt;
  uint16_t buf_pitch;
  uint16_t buf_width;
  uint16_t buf_height;
  uint32_t lu_buf_base;
  uint32_t ch_buf_base;

  uint16_t img_width;
  uint16_t img_height;
  uint16_t img_offset_x;
  uint16_t img_offset_y;
} dec_fb_desc_t;

typedef struct
{
  uint8_t  valid;
  uint8_t  ch_fmt;
  uint8_t  rotate;
  uint8_t  flip;
  uint16_t img_width;
  uint16_t img_height;
  uint16_t img_offset_x;
  uint16_t img_offset_y;
  uint32_t quant_table_base;
  uint32_t bstrm_base;
  uint32_t bstrm_size_max;
} dec_jpg_desc_t;

// (cmd code 0x07000001)
typedef uint8_t dec_setup_type_t;
#define DEC_SETUP_TYPE_DEFAULT    0u
#define DEC_SETUP_TYPE_AU_BOUND   1u
#define DEC_SETUP_TYPE_I_ONLY     2u
#define DEC_SETUP_TYPE_IP_ONLY    4u
#define DEC_SETUP_TYPE_MULTI_CHN  8u
#define DEC_SETUP_TYPE_LOW_DELAY  16u

typedef uint8_t dec_err_handling_mode_t;
#define DEC_ERR_HANDLING_NONE     0u      // Assert on any error
#define DEC_ERR_HANDLING_HALT     1u      // Report error and halt scheduler

typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t dec_setup_type; // dec_setup_type_t

  uint32_t bits_fifo_base;
  uint32_t bits_fifo_limit;

  uint32_t rbuf_smem_size;
  uint32_t fbuf_dram_size;
  uint32_t pjpeg_buf_size;

  uint8_t  use_dproc_vrscl;
  uint8_t  cabac_2_recon_delay;
  uint8_t  err_handling_mode; // dec_err_handling_mode_t
  uint8_t  max_frm_num_of_dpb;

  uint8_t  enable_pic_info;
  uint8_t  is_regression;
  uint8_t  is_bringup;
  uint8_t  reserved;

  uint16_t ext_buf_mask; // pass to vproc in duplex mode
} cmd_decoder_setup_t;

// (cmd code 0x07000002)
typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t bits_fifo_start;
  uint32_t bits_fifo_end;

  uint32_t num_pics;
  uint32_t num_frame_decode;

  uint64_t first_frame_display;
} cmd_decoder_start_t;

// (cmd code 0x07000003)
typedef uint8_t dec_stop_mode_t;
#define DEC_STOP_MODE_VOUT_STOP  0u
#define DEC_STOP_MODE_LAST_PIC   1u

typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t stop_mode; // dec_stop_mode_t
} cmd_decoder_stop_t;

// (cmd code 0x07000004)
typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t bits_fifo_start;
  uint32_t bits_fifo_end;
  uint32_t num_pics;
} cmd_decoder_bitsfifo_update_t;

// (cmd code 0x07000005)
typedef uint8_t dec_speed_scan_mode_t;
#define DEC_SCAN_IPB_ALL    0u
#define DEC_SCAN_REF_ONLY   1u
#define DEC_SCAN_I_ONLY     2u

typedef uint8_t dec_speed_direction_t;
#define DEC_DIR_FWD         0u
#define DEC_DIR_BWD         1u

typedef struct
{
  cmd_dec_hdr_t hdr;

  uint16_t speed;
  uint8_t  scan_mode; // dec_speed_scan_mode_t
  uint8_t  direction; // dec_speed_direction_t
  uint32_t out_strm_time_scale;
  uint32_t out_strm_num_units_in_tick;
} cmd_decoder_speed_t;

// (cmd code 0x07000006)
typedef uint8_t dec_trickplay_mode_t;
#define DEC_TRICK_PAUSE     0u
#define DEC_TRICK_RESUME    1u
#define DEC_TRICK_STEP      2u
#define DEC_TRICK_FREEZE    3u
#define DEC_TRICK_UNFREEZE  4u

typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t mode; // dec_trickplay_mode_t
} cmd_decoder_trickplay_t;

// (cmd code 0x07000007)
typedef struct
{
  cmd_dec_hdr_t hdr;

  uint8_t  user_data;
  uint8_t  feedback;
  uint8_t  is_regression;
  uint8_t  reserved;

  uint32_t bstrm_start;
  uint32_t bstrm_size;

  uint32_t yuv_buf_base;
  uint32_t yuv_buf_size;
} cmd_decoder_stilldec_t;


// (cmd code 0x07000008)
typedef struct
{
  cmd_dec_hdr_t hdr;

  batch_cmd_set_info_t  batch_cmd_set_info;

} cmd_decoder_batch_t;


// (msg code 0x87000001)
typedef uint8_t dec_op_mode_t;
#define DEC_OPM_INVALID          0u
#define DEC_OPM_IDLE             1u
#define DEC_OPM_RUN              2u
#define DEC_OPM_VDEC_IDLE        3u
#define DEC_OPM_RUN_2_IDLE       4u
#define DEC_OPM_RUN_2_VDEC_IDLE  5u
#define DEC_OPM_FLUSHING         6u
#define DEC_OPM_FREEZE           7u
#define DEC_OPM_RUN_2_FREEZE     8u

typedef uint8_t dec_error_level_t;
#define DEC_ERR_LVL_NONE         0u
#define DEC_ERR_LVL_WARNING      1u
#define DEC_ERR_LVL_RECOVERABLE  2u
#define DEC_ERR_LVL_FATAL        3u

typedef struct
{
  uint32_t err_type   :16;
  uint32_t err_level  :8;  // dec_error_level_t (decoder shall be halted and stopped with DEC_ERR_LVL_FATAL)
  uint32_t codec_type :8;  // dec_codec_type_t
} dec_err_code_t;

typedef struct
{
  msg_dec_hdr_t hdr;

  uint32_t decode_state; // dec_op_mode_t
  uint32_t bits_fifo_next_rptr;
  uint32_t decoded_pic_number;
  uint32_t reconed_pic_number;
  uint32_t error_status; // dec_err_code_t

  uint16_t frm_pitch;
  uint16_t frm_width;
  uint16_t frm_height;
  uint8_t  chroma_format_idc;
  uint8_t  is_eostrm;

  uint32_t batch_cmd_id;
} msg_decoder_status_t;

// (msg code 0x87000002)
typedef struct
{
  msg_dec_hdr_t hdr;

  dec_fb_desc_t yuv_pic; // if errors, yuv_pic.valid = 0
} msg_decoder_stilldec_t;

// (msg code 0x87000003)
typedef struct
{
  msg_dec_hdr_t hdr;

  uint32_t pic_no; // display order
  
  uint32_t pts_low;
  uint32_t pts_high;

  dec_fb_desc_t yuv_pic;

} msg_decoder_picinfo_t;

/************************************************************
 * DPROC command and messages (Category 8)
 */

// (cmd code 0x08000001)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  vrscl_paint_fbp;
  uint8_t  vrscl_bg_y;
  uint8_t  vrscl_bg_u;
  uint8_t  vrscl_bg_v;

} cmd_dproc_setup_t;

// (cmd code 0x08000002)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  user_data;
  uint8_t  feedback;
  uint8_t  rotate;
  uint8_t  flip;

  int8_t   luma_gain;
  uint8_t  reserved[3];

  dec_fb_desc_t src_pic;
  dec_fb_desc_t dst_pic[DPROC_MAX_YUV_OUT_NUM];

} cmd_dproc_yuv_to_yuv_t;

// (cmd code 0x08000003)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  vout_id;
  uint8_t  vid_win_update;
  uint8_t  vid_win_rotate;
  uint8_t  vid_flip;

  uint16_t vid_win_width;
  uint16_t vid_win_height;
  uint16_t vid_win_offset_x;
  uint16_t vid_win_offset_y;

  dec_fb_desc_t yuv_pic;

} cmd_dproc_yuv_display_t;

// (cmd code 0x08000004)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  user_data;
  uint8_t  feedback;
  uint8_t  background_y;
  uint8_t  background_u;
  uint8_t  background_v;
  uint8_t  reserved[3];

  dec_fb_desc_t yuv_pic;

} cmd_dproc_yuv_paint_t;

// (cmd code 0x08000011)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  channel_id;
  uint8_t  stream_id;
  uint8_t  tar_win_rotate;
  uint8_t  tar_win_flip;

  uint16_t tar_win_offset_x;
  uint16_t tar_win_offset_y;
  uint16_t tar_win_width;
  uint16_t tar_win_height;

  uint8_t  inp_win_update;
  uint8_t  reserved[3];
  uint16_t inp_win_offset_x;
  uint16_t inp_win_offset_y;
  uint16_t inp_win_width;
  uint16_t inp_win_height;

} cmd_dproc_vrscl_layout_t;

// (cmd code 0x08000012)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  channel_id;
  uint8_t  stream_id;
  uint8_t  direction; // dec_direction_t
  uint8_t  resrved_0;

  uint64_t pts64; // 0: start immediatly

  // framet rate info (0: no frame rate control)
  uint32_t num_units_in_tick;
  uint32_t time_scale;

} cmd_dproc_vrscl_start_t;

// (cmd code 0x08000013)
#define DPROC_STOP_MODE_NORMAL 0u
#define DPROC_STOP_MODE_EOSTRM 1u
#define DPROC_STOP_MODE_PAUSE  2u

typedef struct
{
  uint32_t cmd_code;

  uint8_t  channel_id;
  uint8_t  stream_id;
  uint8_t  stop_mode;
  uint8_t  resrved_0;

  uint64_t pts64; // 0: stop immediatly

} cmd_dproc_vrscl_stop_t;

// (cmd code 0x08000014)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  stream_id;
  uint8_t  interlaced;
  uint8_t  resrved_0;
  uint8_t  resrved_1;

  uint16_t vid_win_offset_x;
  uint16_t vid_win_offset_y;
  uint16_t vid_win_width;
  uint16_t vid_win_height;

} cmd_dproc_vrscl_vid_win_t;

// (cmd code 0x08000015)
typedef struct
{
  uint32_t cmd_code;

  uint8_t  channel_id;
  uint8_t  resrved_0;
  uint8_t  resrved_1;
  uint8_t  resrved_2;

  uint32_t pts_high;
  uint32_t pts_low;

  dec_fb_desc_t ext_pic;

} cmd_dproc_vrscl_send_ext_pic_t;

// (msg code 0x88000001)
typedef struct
{
  uint32_t msg_code;
  uint32_t timecode;
  uint32_t latest_clock_counter;

  // TBD
} msg_dproc_status_t;

// (msg code 0x88000002)
typedef struct
{
  uint32_t msg_code;
  uint32_t timecode;
  uint32_t latest_clock_counter;

  dec_fb_desc_t yuv_pic[DPROC_MAX_YUV_OUT_NUM];

} msg_dproc_yuv_to_yuv_t;

// (msg code 0x88000004)
typedef struct
{
  uint32_t msg_code;
  uint32_t timecode;
  uint32_t latest_clock_counter;

  dec_fb_desc_t yuv_pic;

} msg_dproc_yuv_paint_t;

/************************************************************
 * Postprocess commands (Category 10)
 */
// cmd code 0x0A000001
typedef uint8_t ppvout_win_upd_t;
#define  PPVOUT_WIN_UPD_NONE   0u
#define  PPVOUT_WIN_UPD_NEXT   1u
#define  PPVOUT_WIN_UPD_CURR   2u

typedef struct 
{
  uint32_t cmd_code;

  uint8_t  decode_cat_id; // dsp_cmd_cat_t
  uint8_t  decode_id;
  uint8_t  voutA_enable;
  uint8_t  voutB_enable;

  uint16_t input_center_x;
  uint16_t input_center_y;

  // window update defined in ppvout_win_upd_t
  uint8_t  voutA_target_win_update;
  uint8_t  voutB_target_win_update;
  uint8_t  vout0_win_update;
  uint8_t  vout1_win_update;

  // target window
  uint16_t voutA_target_win_offset_x;
  uint16_t voutA_target_win_offset_y;
  uint16_t voutA_target_win_width;
  uint16_t voutA_target_win_height;

  uint32_t voutA_zoom_factor_x;
  uint32_t voutA_zoom_factor_y;

  uint16_t voutB_target_win_offset_x;
  uint16_t voutB_target_win_offset_y;
  uint16_t voutB_target_win_width;
  uint16_t voutB_target_win_height;

  uint32_t voutB_zoom_factor_x;
  uint32_t voutB_zoom_factor_y;

  // video window
  uint8_t  vout0_flip;
  uint8_t  vout0_rotate;
  uint16_t vout0_win_offset_x;
  uint16_t vout0_win_offset_y;
  uint16_t vout0_win_width;
  uint16_t vout0_win_height;

  uint8_t  vout1_flip;
  uint8_t  vout1_rotate;
  uint16_t vout1_win_offset_x;
  uint16_t vout1_win_offset_y;
  uint16_t vout1_win_width;
  uint16_t vout1_win_height;

} cmd_postproc_t;

/**
 * postproc status msg (0x8a000001)
 */
typedef struct 
{
  uint32_t msg_code;
  uint32_t timecode;
  uint32_t latest_clock_counter;

  uint8_t  ppvout_id;
  uint8_t  ppvout_en;
  uint8_t  ppvout_exist;
  uint8_t  is_end_of_strm;

  uint32_t ppvout_state;

  uint32_t last_pts_high;
  uint32_t last_pts_low;

  uint32_t last_lu_base;
  uint32_t last_ch_base;

  uint32_t ch_fmt; // yuv_chroma_fmt_t

} msg_postp_status_t;


#endif //!CMD_MSG_PROD_DV_H_
