/* CV2 DSP API header file - common
Revision: 43821
Last Changed Rev: 43790
Last Changed Date: 2022-01-25 02:15:41 -0800 (Tue, 25 Jan 2022)
*/
/**
 * @file cmd_msg_dsp.h
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
 * @brief application(product) independent part of DSP API.
 */

#ifndef CMD_MSG_DSP_H_
#define CMD_MSG_DSP_H_

#include <dsp_types.h>

#define IMG_MODE_LOW_ISO_VIDEO 95u

#define CFG_PASS_INVLAID 0xFFu
#define CFG_STEP_INVLAID 0xFFu

////////////////////////////////////////////////////////////////////////////////

typedef uint8_t pass_step_id_t;

#define IMG_PASS_VIN            0u
#define IMG_PASS_C2Y            1u
#define IMG_PASS_Y2Y            2u
#define IMG_PASS_STEP_INVALID   255u

////////////////////////////////////////////////////////////////////////////////
#define SIL_VER_INVALID         0u
#define SIL_VER_ENGINEERING     99

//#ifdef PROJECT_CV2A
#define SIL_VER_CV2FS25      20250
#define SIL_VER_CV2FS75      20750
#define SIL_VER_CV2FS85      20850
#define SIL_VER_CV22FS15     30150
#define SIL_VER_CV22FS55     30550
#define SIL_VER_CV22FS85     30850
#define SIL_VER_CV20AQ25     40250
#define SIL_VER_CV20AQ75     40750
#define SIL_VER_CV20AQ85     40850
//#endif

/* located at 0x100 */
typedef struct
{
  uint32_t ucode_version;
  uint32_t ucode_date;
  uint32_t ucode_api_version;
  uint32_t ucode_silicon_version;
  uint32_t ucode_link_base;

} ucode_info_t;

#define DSP_SYNC_COUNTER_CMD_REQ_VDSP0   19u  /* Sync Counter for VDSP0 command request */
#define DSP_SYNC_COUNTER_CMD_REQ_VDSP1   20u  /* Sync Counter for VDSP1 command request */
#define DSP_SYNC_COUNTER_IDSP_VP_MSG     127u /* Sync Counter for IDSP VP message */

#define MAX_NUM_DSP_CMD_Q 6u
#define MAX_NUM_DSP_MSG_Q 8u

typedef struct
{
  uint32_t base_daddr;   /* base daddr of Message FIFO */
  uint32_t max_num_msg;  /* maximum number of messages */
  uint32_t read_ptr;     /* read pointer */
  uint32_t write_ptr;    /* write pointer */
  uint32_t reserved[12];

} dsp_msg_q_info_t;

/* prt_period locates at chip_id_daddr+8 */
typedef struct
{
  uint8_t payload[64];      /* iv:                  : 16 bytes (128 bits) */
                            /* uid and audio ticks  : 32 bytes (256 bits) */
                            /* audio ticks          : 4  bytes  (32 bits) */
                            /* rsvd                 : 12 bytes */
} prt_period;

typedef struct
{
  uint32_t module;        // 4B
  uint32_t file_name;     // 8B
  uint32_t file_line;     // 12B
  uint32_t prn_str;       // 16B
  uint32_t prn_arg1;      // 20B
  uint32_t prn_arg2;      // 24B
  uint32_t prn_arg3;      // 28B
  uint32_t prn_arg4;      // 32B
  uint32_t prn_arg5;      // 36B

} assert_info_t;          // For system/ucode agreement :
                          // This assert_info_t Must allocate 64 bytes align

typedef struct
{
  assert_info_t info[8];   // 8 threads's assert info
} orc_assert_t;

/* located at 0x200 */
typedef struct
{
  /* Off-line predefined binary data */
  uint32_t default_binary_data_daddr;                                      /* 4B */
  uint32_t default_binary_data_dsize;                                      /* 8B */

  /* Default commands */
  uint32_t default_config_daddr;                                           /* 12B */
  uint32_t default_config_dsize;                                           /* 16B */

  /* Continguous DRAM space assgined to DSP */
  uint32_t dsp_dram_daddr;                                                 /* 20B */
  uint32_t dsp_dram_size;                                                  /* 24B */

  /* Extended IDSP memory for ucode */
  uint32_t idsp_ext_dram_daddr;                                            /* 28B */
  uint32_t idsp_ext_dram_dsize;                                            /* 32B */

  /* DSP printf area */
  uint32_t dsp_debug_daddr;                                                /* 36B */
  uint32_t dsp_debug_dsize;                                                /* 40B */

  /* DSP command */
  uint32_t cmd_data_daddr[MAX_NUM_DSP_CMD_Q];                              /* 64B */
  uint32_t cmd_data_dsize[MAX_NUM_DSP_CMD_Q];                              /* 88B */

  uint32_t cmd_req_timer                  : 4;                             /* sync counter timer in ms. ( only 1 ~ 4 ms allowed ) */
  uint32_t msg_q_full_assert              : 1;                             /* 0: no assert, 1: assert */
  uint32_t req_cmd_timeout_assert         : 1;                             /* 0: no assert, 1: assert */
  uint32_t run_with_vp                    : 1;
  uint32_t enable_vout_ln_sync_monitor    : 1;
  uint32_t is_orccode_fully_load          : 1;                             /* This bit is used under is_partial_load_en=1,
                                                                              when orccode.bin is fully loaded, system set this bit as 1 */
  uint32_t data_prot_safety_enabled       : 2;                             /* 0 : no ASIL 26262 safety mechanism is enabled.
                                                                              1 : safety mechanism is enabled.
                                                                              Other values are undefined. */
  uint32_t reserved_0                     : 21;                            /* 92B */

  /* DSP message */
  uint32_t msg_q_info_data_daddr[MAX_NUM_DSP_MSG_Q];                       /* 124B */

  /* chip_id pointer. Can point to efuse ID and much more in future. */
  uint32_t chip_id_daddr;                                                  /* 128B */

  /* The dram buffer for orc_assert_t */
  uint32_t orc_assert_daddr;                                               /* 132B */

  /*  If is_prt_opt=1,
   *  System MUST allocate a dram buffer of 64 bytes (size of prt_period).
   *  This dram buffer base is chip_id_daddr+8 !
   */
  uint32_t is_prt_opt                     : 2;
  uint32_t delta_audio_ticks              : 30;                            /* 136B */

  uint32_t att_prot_dram_daddr;                                            /* The dram space that is protected by ATT table to exclusively dedicate to the DSP */
  uint32_t att_prot_dram_dsize;                                            /* The size of the ATT protected dram space. */
  uint32_t dram_exclusion_region_num      : 8;                             /* The number of the dram segments that are excluded from DSP to use.
                                                                              Max value is 4 [i.e. valid values are 0, 1, 2, 3, and 4].
                                                                              If value = 0, then all following parameters should be filled with 0x0.*/
  uint32_t reserved_1                     : 24;                            /* 148B */
  uint32_t dram_exclusion_region_0_daddr;                                  /* The first exclusion region's starting address */
  uint32_t dram_exclusion_region_0_dsize;                                  /* The first exclusion region's size */
  uint32_t dram_exclusion_region_1_daddr;                                  /* The second exclusion region's starting address */
  uint32_t dram_exclusion_region_1_dsize;                                  /* The second exclusion region's size */
  uint32_t dram_exclusion_region_2_daddr;                                  /* The third exclusion region's starting address */
  uint32_t dram_exclusion_region_2_dsize;                                  /* The third exclusion region's size */
  uint32_t dram_exclusion_region_3_daddr;                                  /* The fourth exclusion region's starting address */
  uint32_t dram_exclusion_region_3_dsize;                                  /* The fourth exclusion region's size */
                                                                           /* 180B */
  uint32_t assertion_deselect_mask;                                        /* Indicates which module is "not" going to set cehu error bit when fatal error happens */
                                                                           /* 184B */
  uint32_t watchdog_cnt_ptr;                                               /* A pointer to a dream area of 4*(uint32_t), i.e. an array of 4 integers */
                                                                           /* whose element 0: VDSP0 (VOUT), 1: VDSP1 (VIN), 2: VDSP2 (VPROC), 3: VDSP3 (ENC). */
                                                                           /* If the watchdog counter pointer is 0x0, ucode will not accumulate ISRs to the system. */
                                                                           /* 188B */
  uint32_t enet_timer_diff_threshold;                                      /* the threashold of difference of enet_timer0 and enet_timer1 */

#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t padding[15];                                                    /* Padding up to 252B */
  uint32_t crc_init_data;                                                  /* 256B */
#else
  uint32_t padding[16];                                                    /* Padding up to 252B */
#endif
} dsp_init_data_t;


////////////////////////////////////////////////////////////////////////////////
/* same as H.264 standard (table 6-1) */
typedef uint8_t yuv_chroma_fmt_t;

#define YUV_MONO 0u /* Y only */
#define YUV_420  1u
#define YUV_422  2u
#define YUV_444  3u

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Macro Defines
////////////////////////////////////////////////////////////////////////////////

#define DSP_CMD_SIZE            128u
#define DSP_MSG_SIZE            128u

#define DSP_CMD_NUM             32u
#define DSP_MSG_NUM             32u

/* DSP command format */
typedef struct
{
  uint32_t  cmd_code;
  uint32_t  pay_load[(DSP_CMD_SIZE / 4u) - 1u];

} dsp_cmd_t;

#define DSP_CMD dsp_cmd_t
/* DSP message format */
typedef struct
{
  uint32_t msg_code;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t msg_crc;
  uint32_t pay_load[(DSP_MSG_SIZE / 4u) - 2u];
#else
    uint32_t pay_load[(DSP_MSG_SIZE / 4u) - 1u];
#endif
} dsp_msg_t;
#define DSP_MSG dsp_msg_t
/* first DSP cmd for each iteration */
typedef struct
{
  uint32_t cmd_code;    /* must be 0x000000AB */
  uint32_t cmd_seq_num;
  uint32_t num_cmds;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t cmd_block_crc;
#endif
} dsp_header_cmd_t;

/* first DSP msg for each iteration */
/* and NOT supposed to be written explicity by DSP */

/// MSG_DSP_STATUS (0x81000001u)
typedef struct
{
  uint32_t msg_code;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t msg_crc;
#endif
  uint32_t dsp_prof_id        :16;
  uint32_t fatal_error_stat   : 1; // 0 means no fatal error, 1 means fatal err has happened
  uint32_t fatal_err_th_id    : 7; // the thread that first triggered the err.
  uint32_t reserved16_0       : 8;
  uint32_t time_code;
  uint32_t prev_cmd_seq;
  uint32_t prev_num_cmds;

  /* Timeout reasons.
     Bit 0 : unused.
     Bit 1 : timer.
     Bit 2 : DMA.
     Bit 3 : HE0.
     Bit 4 : HE1.
     Bit 5 : scanner.
     Bit 6 : unused.
     Bit 7 : Send.
     Bit 8 : Receive. */
  uint32_t timeout_test_result;

} dsp_status_msg_t;

/// MSG_CMD_REQ (0x81000002u)
typedef struct
{
  uint32_t msg_code;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t msg_crc;
#endif
  uint32_t req_cmd;   /* 1 for request arm's cmd */
  uint32_t prev_cmd_seq;
  uint32_t prev_num_cmds;
  uint32_t vin_id;
  uint32_t is_time_out;

} dsp_cmd_req_msg_t;

#ifdef SUPPORT_DSP_MSG_CRC
#define ECHO_CMD_MSG_MAX_INFO_NUM ((DSP_MSG_SIZE / 8u) - 2u)
#else
#define ECHO_CMD_MSG_MAX_INFO_NUM ((DSP_MSG_SIZE / 8u) - 1u)
#endif
typedef struct
{
  uint32_t cmd_code;
  uint32_t stream_id  :8;   /* fov_id for VIN, channel_id for VPROC/ENC, 0xff for VOUT */
  uint32_t rsvd       :24;

} echo_cmd_info_t;

/// MSG_ECHO_CMD (0x81000003u)
typedef struct
{
  uint32_t msg_code;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t msg_crc;
#endif

  uint32_t num_rcvd_exec_cmd  :16;
  uint32_t module_id          :8;   /* 0: COMMON 1:VIN, 2:VPROC, 3:VOUT, 4:ENC */
  uint32_t chan_id            :8;   /* VIN_ID for VIN, VOUT_ID for VOUT, 0xff for VPROC and ENC */

  echo_cmd_info_t echo_cmd_info[ECHO_CMD_MSG_MAX_INFO_NUM];
} msg_echo_cmd_t;

////////////////////////////////////////////////////////////////////////////////

typedef uint8_t dsp_cmd_cat_t;

#define CAT_DSP_HEADER      0u   /* specifically for header of cmd/msg comm area */
#define CAT_DSP_CFG         1u
#define CAT_VPROC           2u   /* capture for video flow */
#define CAT_SPROC           3u   /* capture for still flow */
#define CAT_VIN             4u
#define CAT_VOUT            5u
#define CAT_ENC             6u
#define CAT_DEC             7u
#if 1 //def BACKWARD_COMPATIBLE_TO_IDSPDRV_30
#define CAT_IDSP            9u
#endif
#define CAT_INTERNAL        64u  /* 64u ~ 127u reserved for internal uses */
#define CAT_INVALID         127u

typedef dsp_cmd_cat_t dsp_msg_cat_t; /* Corresponging message and command share same category id */

////////////////////////////////////////////////////////////////////////////////

typedef uint16_t dsp_prof_cfg_id_t;

#define DSP_PROF_INVALID   0u
#define DSP_PROF_CAMERA    1u
#define DSP_PROF_UNITTEST0 240u /* for mm testing only */
#define DSP_PROF_UNITTEST1 241u /* for ieng testing only */
#define DSP_PROF_UNITTEST2 242u /* for ieng testing only */
#define DSP_PROF_UNITTEST3 243u /* for idspdrv testing only */
#define DSP_PROF_SAFETY    253u
#define DSP_PROF_2_IDLE    254u
#define DSP_PROF_IDLE      255u

////////////////////////////////////////////////////////////////////////////////
//
/// CMD_DSP_HAL_INF, command code = 0x01000002
typedef struct
{
  uint32_t cmd_code;

  uint32_t audi_clk_freq;
  uint32_t core_clk_freq;
  uint32_t dram_clk_freq;
  uint32_t idsp_clk_freq;

} cmd_dsp_hal_inf_t;

/// CMD_DSP_SUSPEND_PROFILE, cmd code 0x01000003
typedef struct
{
  uint32_t cmd_code;

} cmd_dsp_suspend_profile_t;

/// CMD_DSP_SET_DEBUG_LEVEL, command code 0x01000004
typedef struct
{
  uint32_t  cmd_code;
  uint32_t  module;
  uint32_t  level;

} cmd_set_debug_level_t;


/// CMD_DSP_PRINT_THREAD_DISABLE_MASK, command code 0x01000005
typedef struct
{
  uint32_t  cmd_code;
  uint32_t  orccode_mask_valid      :1;
  uint32_t  orcme_mask_valid        :1;
  uint32_t  orcmdxf_mask_valid      :1;
  uint32_t  reserved                :13;
  uint32_t  orccode_mask            :12;
  uint32_t  orcmdxf_mask            :2;
  uint32_t  orcme_mask              :2;
} cmd_print_th_disable_mask_t;

/// create bindings between ucode entities
typedef struct
{
  uint32_t bind_type   : 4;
  // refer to bind_msg_t
  // 0: bind data queue, 1: bind msg queue
  uint32_t bind_msg    : 1;
  uint32_t rsvd        : 3;
  uint32_t delay       : 8; // used in broadcast to introduce look-ahead delay

  uint32_t src_fp_type : 4; // FLOW_TAG_t
  uint32_t src_fp_id   : 4;
  uint32_t src_ch_id   : 8;
  uint32_t src_str_id  : 8;

  uint32_t dst_fp_type : 4; // FLOW_TAG_t
  uint32_t dst_fp_id   : 4;
  uint32_t dst_ch_id   : 8;
  uint32_t dst_str_id  : 8;

} bind_desc_t;

////////////////////////////////////////////////////////////////////////////////
/// to be used by bind_desc_t->bind_type

typedef uint8_t bind_type_t;

#define BIND_TYPE_BIND   1u
#define BIND_TYPE_UNBIND 2u

////////////////////////////////////////////////////////////////////////////////
/// to be used by bind_desc_t->bind_msg

typedef uint8_t bind_msg_t;

#define BIND_DATA_Q 0u
#define BIND_MSG_Q  1u

////////////////////////////////////////////////////////////////////////////////
/// to be used by bind_desc_t->src_fp_type and bind_desc_t->dst_fp_type

typedef uint8_t FLOW_TAG_t;

#define FP_BASE_VIN      1u
#define FP_BASE_VPROC_DV 2u
#define FP_BASE_SPROC    3u
#define FP_BASE_VENC     4u
#define FP_BASE_ENG0     5u
#define FP_BASE_DEC      6u
#define FP_BASE_NUM      6u
////////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_ext_mem_type_t;

#define VIN_EXT_MEM_TYPE_RAW    0u
#define VIN_EXT_MEM_TYPE_YUV    1u
#define VIN_EXT_MEM_TYPE_HDS    2u
#define VIN_EXT_MEM_TYPE_ME     3u  // ME0+ME1
#define VIN_EXT_MEM_TYPE_AUX    4u  // AUX_OUT
#define VIN_EXT_MEM_TYPE_NUM    5u

////////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_send_in_data_type_t;

#define VIN_SEND_IN_DATA_TYPE_RAW    0u
#define VIN_SEND_IN_DATA_TYPE_YUV422 1u
#define VIN_SEND_IN_DATA_TYPE_YUV420 2u
#define VIN_SEND_IN_DATA_TYPE_NUM    3u

#define VIN_SEND_IN_DATA_DEST_VPROC   0u
#define VIN_SEND_IN_DATA_DEST_ENC     1u
#define VIN_SEND_IN_DATA_DEST_NUM     2u

////////////////////////////////////////////////////////////////////////////////

/// CMD_BINDING_CFG, command code 0x01000006
#define MAX_BINDINGS_PER_CMD 12u
typedef struct
{
  uint32_t     cmd_code;
  uint32_t     num_of_bindings : 8;   // number of bindings in this cmd
  uint32_t     reserved        : 24;
  bind_desc_t  bindings[MAX_BINDINGS_PER_CMD];

} cmd_binding_cfg_t;


/// CMD_DSP_ACTIVATE_PROFILE, command code 0x01000007
typedef struct
{
  uint32_t cmd_code;
  uint16_t dsp_prof_id;
  uint16_t reserved16_0;

} cmd_dsp_activate_profile_t;

typedef struct
{
  //uint32_t  enable         : 1;
  uint32_t  memory_type    : 8; //for memory_type refer to vin_ext_mem_type_t
  uint32_t  max_daddr_slot : 8;
  uint32_t  pool_buf_num   : 8;
  uint32_t  reserved       : 8;

} ext_mem_desc_t;


#define MAX_VIN_EXT_MEM_CFG (8u)
/// CMD_DSP_VIN_FLOW_MAX_CFG, command code 0x01000009
typedef struct
{
  // W0
  uint32_t  cmd_code;

  // W1
  uint32_t  vin_id                            : 8;
  uint32_t  is_sensor_raw_out_enabled         : 1;
  uint32_t  is_contrast_enhance_out_enabled   : 1;
  uint32_t  is_yuv422_out_enabled             : 1;
  uint32_t  raw_width_is_byte_width           : 1;
  uint32_t  vin_raw_smem_win_out              : 1;
  uint32_t  ext_mem_cfg_num                   : 4;
  uint32_t  is_aux_out_enabled                : 1;
  uint32_t  rsvd                              : 14;

  // W2
  uint16_t  max_raw_cap_dbuf_num;
  uint16_t  max_raw_cap_width;
  // W3
  uint16_t  max_raw_cap_height;

  uint16_t  max_contrast_enhance_out_dbuf_num;
  // W4
  uint16_t  max_contrast_enhance_out_width;
  uint16_t  max_contrast_enhance_out_height;

  // W5
  uint16_t  max_yuv422_out_dbuf_num;
  uint16_t  max_yuv422_out_width;
  // W6
  uint16_t  max_yuv422_out_height;

  uint8_t   max_vin_outstanding_dram_xfers; // max SMEM_WIN height
  uint8_t   vin_dram_xfer_sblk;

  // W7
  uint16_t  max_fov_num;

  uint16_t  max_aux_out_dbuf_num;

  // W8 - W15
  ext_mem_desc_t ext_mem_cfg[MAX_VIN_EXT_MEM_CFG];

  // W16
  uint16_t  max_aux_out_width;
  uint16_t  max_aux_out_height;

  // W17

} cmd_dsp_vin_flow_max_cfg_t;

/////////////////////////////////////////////////////////////////////////////
#define VOUT_ID_A   0u
#define VOUT_ID_B   1u

#define VOUT_SRC_DEFAULT_IMG  0u
#define VOUT_SRC_BACKGROUND   1u
#define VOUT_SRC_VPROC        2u

#define OSD_SRC_MAPPED_IN     0u
#define OSD_SRC_DIRECT_IN_16  1u
#define OSD_SRC_DIRECT_IN_32  2u

#define CSC_DIGITAL 0u

/* 16-bit mode */
#define OSD_MODE_VYU565     0u  /* 5:6:5 (VYU) or (RGB) */
#define OSD_MODE_RGB565     0u
#define OSD_MODE_UYV565     1u  /* 5:6:5 (UYV) or (BGR) */
#define OSD_MODE_BGR565     1u
#define OSD_MODE_AYUV4444   2u  /* 4:4:4:4 (AYUV) */
#define OSD_MODE_RGBA4444   3u  /* 4:4:4:4 (RGBA) */
#define OSD_MODE_BGRA4444   4u  /* 4:4:4:4 (BGRA) */
#define OSD_MODE_ABGR4444   5u  /* 4:4:4:4 (ABGR) */
#define OSD_MODE_ARGB4444   6u  /* 4:4:4:4 (ARGB) */
#define OSD_MODE_AYUV1555   7u  /* 1:5:5:5 (AYUV) */
#define OSD_MODE_YUV1555    8u  /* 1:5:5:5 (MSB ignored YUV) */
#define OSD_MODE_RGBA5551   9u  /* 5:5:5:1 (RGBA) */
#define OSD_MODE_BGRA5551   10u /* 5:5:5:1 (BGRA) */
#define OSD_MODE_ABGR1555   11u /* 1:5:5:5 (ABGR) */
#define OSD_MODE_ARGB1555   12u /* 1:5:5:5 (ARGB) */
/* 32-bit mode */
#define OSD_MODE_AYUV8888   27u /* 8:8:8:8 (AYUV) */
#define OSD_MODE_RGBA8888   28u /* 8:8:8:8 (RGBA) */
#define OSD_MODE_BGRA8888   29u /* 8:8:8:8 (BGRA) */
#define OSD_MODE_ABGR8888   30u /* 8:8:8:8 (ABGR) */
#define OSD_MODE_ARGB8888   31u /* 8:8:8:8 (ARGB) */

/////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t reserved;

} cmd_vout_hdr_t;

// (cmd code 0x05000001)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint8_t  interlaced;
  uint8_t  frm_rate;
  uint16_t act_win_width;
  uint16_t act_win_height;
  uint8_t  back_ground_v;
  uint8_t  back_ground_u;
  uint8_t  back_ground_y;
  uint8_t  mixer_444;
  uint8_t  highlight_v;
  uint8_t  highlight_u;
  uint8_t  highlight_y;
  uint8_t  highlight_thresh;
  uint8_t  reverse_en;
  uint8_t  csc_en;
  uint8_t  mixer_420;
  uint8_t  reserved;
  uint32_t csc_parms[9];

} cmd_vout_mixer_setup_t;

// (cmd code 0x05000002)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint8_t  en;
  uint8_t  src;
  uint8_t  flip;
  uint8_t  rotate;
  uint8_t  rotate_flip_mode;
  uint8_t  interlaced;
  uint16_t win_offset_x;
  uint16_t win_offset_y;
  uint16_t win_width;
  uint16_t win_height;
  uint32_t default_img_y_addr;
  uint32_t default_img_uv_addr;
  uint16_t default_img_pitch;
  uint8_t  default_img_repeat_field;
  uint8_t  default_img_ignore_rotate_flip;
  uint8_t  default_img_ch_fmt;  // 0:420 1:422
  uint8_t  vproc_prev_setup_mode;
  uint8_t  discard_prefetch;

} cmd_vout_video_setup_t;

// (cmd code 0x05000003)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t reserved;
  uint32_t default_img_y_addr;
  uint32_t default_img_uv_addr;
  uint16_t default_img_pitch;
  uint8_t  default_img_repeat_field;
  uint8_t  reserved2;

} cmd_vout_default_img_setup_t;

// (cmd code 0x05000004)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint8_t  en;
  uint8_t  src;
  uint8_t  flip;
  uint8_t  rescaler_en;
  uint8_t  premultiplied;
  uint8_t  global_blend;
  uint16_t win_offset_x;
  uint16_t win_offset_y;
  uint16_t win_width;
  uint16_t win_height;
  uint16_t rescaler_input_width;
  uint16_t rescaler_input_height;
  uint32_t osd_buf_dram_addr;
  uint16_t osd_buf_pitch;
  uint8_t  osd_buf_repeat_field;
  uint8_t  osd_direct_mode;
  uint16_t osd_transparent_color;
  uint8_t  osd_transparent_color_en;
  uint8_t  osd_swap_bytes;
  uint32_t osd_buf_info_dram_addr;

} cmd_vout_osd_setup_t;

// (cmd code 0x05000005)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t reserved;
  uint32_t osd_buf_dram_addr;
  uint16_t osd_buf_pitch;
  uint8_t  osd_buf_repeat_field;
  uint8_t  reserved2;

} cmd_vout_osd_buf_setup_t;

// (cmd code 0x05000006)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t reserved;
  uint32_t clut_dram_addr;

} cmd_vout_osd_clut_setup_t;

// (cmd code 0x05000007)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t dual_vout_vysnc_delay_ms_x10;
  uint32_t disp_config_dram_addr;
  uint16_t vin_vout_vsync_delay_ms_x10;
  uint16_t vout_sync_with_vin;
  uint16_t vin_vout_vsync_timeout_ms;
  uint16_t reserved;

} cmd_vout_display_setup_t;

// (cmd code 0x05000008)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t reserved;
  uint32_t dve_config_dram_addr;

} cmd_vout_dve_setup_t;

// (cmd code 0x05000009)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint8_t  reset_mixer;
  uint8_t  reset_disp;

} cmd_vout_reset_t;

// (cmd code 0x0500000A)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t csc_type; // 0: digital; Only digital csc is supported in cv2a
  uint32_t csc_parms[9];

} cmd_vout_display_csc_setup_t;

// (cmd code 0x0500000B)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint16_t reserved;
  uint32_t output_mode;

} cmd_vout_digital_output_mode_setup_t;

// (cmd code 0x0500000C)
typedef struct
{
  uint32_t cmd_code;
  uint16_t vout_id;
  uint8_t  enable;
  uint8_t  setup_gamma_table;
  uint32_t gamma_dram_addr;

} cmd_vout_gamma_setup_t;

typedef struct
{
  uint32_t prn_seq_cnt;
  uint32_t thread_id;
  uint32_t format;  // format string
  uint32_t args[5]; // up to 5 arguments

} printf_t;

typedef struct
{
  uint32_t msg_code;    // message code: 0x81000003
  uint32_t num_printfs; // at least 1, up to 3
  printf_t printfs[3];  // up to 3 printf lines

} msg_dsp_echo_t;

/////////////////////////////////////////////////////////////////////////////

typedef uint32_t enc_coding_type_t;

#define ENC_UNKNOWN 0u
#define ENC_H264    1u
#define ENC_HEVC    2u
#define ENC_JPEG    3u

/////////////////////////////////////////////////////////////////////////////

typedef uint32_t enc_paff_mode_t;

#define PAFF_ALL_FRM   1u
#define PAFF_ALL_FLD   2u
#define PAFF_ALL_MBAFF 3u

/////////////////////////////////////////////////////////////////////////////

typedef uint32_t enc_gop_struct_t;

#define GOP_SIMPLE           0u
#define GOP_HIERB            1u
#define GOP_HIERP            2u
#define GOP_NONREORDERB      3u
#define GOP_FAST_SEEK_SIMPLE 4u

/////////////////////////////////////////////////////////////////////////////

// inherit from FRM_TYPE in a9
typedef uint8_t frame_type_t;

#define IDR_PICT  1u
#define I_PICT    2u
#define P_PICT    3u
#define B_PICT    4u
#define JPG_T     5u
#define FS_P_PICT 6u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_status_t;

#define STATUS_IDLE   0u
#define STATUS_ENCODE 1u

/////////////////////////////////////////////////////////////////////////////

// enc_status_msg_t is moved to the end of this file
// Stream Type definition used in command/message

typedef uint8_t stream_type_t;

#define STRM_TP_INVALID         0u
#define STRM_TP_ENC_FULL_RES    0x11u
#define STRM_TP_ENC_STILL_MJPEG 0x19u

/////////////////////////////////////////////////////////////////////////////

#define ENC_BITS_INFO_STEPSIZE  64u

#if 1
typedef struct
{
  // W0
  uint32_t stream_id                                   : 8;
  uint32_t jpeg_quality                                : 8;
  uint32_t reserved_0                                  : 16;

  // W1
  uint32_t pic_type                                    : 4;  // frame_type_t
  uint32_t pic_level                                   : 3;  // same as img_par->pic_level
  uint32_t pic_is_ref                                  : 1;  // 1=used as ref
  uint32_t pic_struct                                  : 1;  // 0=frame, 1=field
  uint32_t tile_id                                     : 2;
  uint32_t tile_num                                    : 2;
  uint32_t pts_minus_dts                               : 5;
  uint32_t top_field_first                             : 1;
  uint32_t repeat_first_field                          : 1;
  uint32_t pic_is_longterm_ref                         : 1;
  uint32_t reserved_1                                  : 11;

  // W2
  uint32_t sliceheader_offset                          : 16;
  uint32_t slice_num                                   : 8;
  uint32_t slice_idx                                   : 8;

  // W3-8
  uint32_t frame_num;
  uint32_t pts_hw;
  uint32_t encode_done_pts;
  uint32_t bits_start_addr;                                  // stream start address
  uint32_t bits_size                                   : 24; // pic stream size
  uint32_t reserved_2                                  : 8;
  uint32_t checksum;                                         // for debugging

  uint32_t bitrate_kbps;                                     // to support bitrate change on-the-fly
  uint32_t cap_seq_no;
  // W9 - 15
  /* Pad to make it total of 64 bytes */
  uint32_t reserved_3[5];
} enc_bits_info_t;
#else
typedef struct
{
  // W0
  uint32_t      channel_id   : 8;
  uint32_t      stream_type  : 8; // refer to stream_type_t
#ifdef PRODUCT_BROADCAST    // TEMPORARY UNTIL MUX UPDATED
  uint32_t      bitrate_kbps : 16;
#else //!PRODUCT_BROADCAST
  uint32_t      jpeg_quality : 8;
  uint32_t      reserved     : 8;
#endif //?PRODUCT_BROADCAST

  // W1
  uint32_t      pic_type           : 4;
  uint32_t      pic_level          : 3;    // same as img_par->pic_level
  uint32_t      pic_is_ref         : 1;    // 1=used as ref
  uint32_t      pic_struct         : 1;    // 0=frame, 1=field
  uint32_t      tile_id            : 2;
  uint32_t      tile_num           : 2;
  uint32_t      pts_minus_dts      : 5;
  uint32_t      top_field_first    : 1;
  uint32_t      repeat_first_field : 1;
  uint32_t      reserved_0         : 12;

  // W2
  uint32_t      sliceheader_offset : 16;
  uint32_t      slice_num          : 8; // total slice number per picture
  uint32_t      slice_idx          : 8; // current slice index

  // W3-W8
  uint32_t      pic_no;
  uint64_t      pts_64;
  uint32_t      bits_start_addr;    // stream start address
  uint32_t      bits_size;          // pic stream size
  int32_t       cpb_fullness;       // cpb fullness

  // W9
  uint32_t      checksum;           // for debugging

  // W10
  uint32_t      pts_cabac_done;

  // W11
  uint32_t      cap_seq_no;

#ifndef PRODUCT_BROADCAST // TEMPORARY UNTIL MUX IS UPDATED
  // W12
  uint32_t      bitrate_kbps;       // support up to 100 Mbps
                                    // to support bitrate change on-the-fly
#endif //?PRODUCT_BROADCAST

  // W13-W15: reserved
} enc_bits_info_t;
#endif

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t rc_mode_t;

#define ENC_RC_OFF        0u
#define ENC_RC_CBR        1u
#define ENC_RC_CBR_TEST   2u // reserved for internal use
#define ENC_RC_VBR        3u
#define ENC_RC_SIMPLE     4u // simple CBR
#define ENC_RC_SIMPLE_VBR 5u // simple VBR
#define ENC_RC_NUM_MODES  6u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_start_method_t;

#define ENC_START_FROM_CMD     0u // start from enc_start cmd
#define ENC_START_FROM_PICINFO 1u // start by encode_start_idc in pic_info

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_stop_method_t;

#define ENC_STOP_FROM_CMD     0u // stop by enc_stop cmd
#define ENC_STOP_FROM_PICINFO 1u // stop by encode_stop_idc in pic_info

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_smem_encode_t;

#define SMEM_ENC_OFF     0u
#define SMEM_ENC_TAR     1u
#define SMEM_ENC_TAR_REF 2u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_dram_encode_t;

// dram encode off if smem_encode>0
// dram encode w/o sync if smem_encode=0
#define DRAM_ENC_NO_SYNC   0u
#define DRAM_ENC_WITH_SYNC 1u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_slice_memd_t;

#define SLICE_MEMD_OFF 0u // disabled
#define SLICE_MEMD_NOR 1u // slice_memd (nomral)
#define SLICE_MEMD_PRI 2u // slice memd (priority, for ultra low delay)

/////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint32_t VBR_ness       : 7;
  uint32_t VBR_complexity : 9;
  uint32_t VBR_min_rate   : 7;
  uint32_t VBR_max_rate   : 9;

} vbr_config_t;

#if 0
typedef union
{
  vbr_config_t field;
  uint32_t     word;

} vbr_config_ut;
#endif

typedef struct
{
  uint32_t cmd_code;
  uint32_t channel_id : 8;
  uint32_t rsvd       : 24;

} cmd_encoder_hdr_t;

/* MD category */
#define MD_CATEGORY_MAX_NUM (8u) /* max number of MD category - HEVC only */

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t md_cat_type_t;

#define MD_CAT_TYPE_DEFAULT        0u
#define MD_CAT_TYPE_FORCED_ZMV     1u
#define MD_CAT_TYPE_FORCED_INTRA   2u
#define MD_CAT_TYPE_UNCOVERED_AREA 3u
#define MD_CAT_TYPE_BIAS_SKIP      4u
#define MD_CAT_TYPE_NUM            5u

/////////////////////////////////////////////////////////////////////////////

/// CMD_ENCODER_SETUP (0x06000001)
typedef struct
{
  uint32_t cmd_code;

  uint32_t stream_id              : 8;
  uint32_t stream_type            : 8;
  uint32_t profile_idc            : 8;   // profile
  uint32_t level_idc              : 8;   // level

  uint32_t coding_type            : 8;   // enc_coding_type_t
  uint32_t aff_mode               : 8;   // enc_paff_mode_t
  uint32_t aud_nalu_mode          : 1;
  uint32_t sps_pps_header_mode    : 1;
  uint32_t reserved_3             : 9;
  uint32_t enc_src                : 5;   // srcbuf_id

  uint32_t encode_w_sz            : 16;  // image size to encode
  uint32_t encode_h_sz            : 16;

  uint32_t encode_w_ofs           : 16;  // offset between input image and real encode image.
  uint32_t encode_h_ofs           : 16;

  uint32_t gop_structure          : 8;   // enc_gop_struct_t
  uint32_t M                      : 8;
  uint32_t N                      : 16;  // -1 as infinite gop, others are real gop_n value.

  uint32_t numRef_P               : 4;   // 0/1=1fwd 2=2fwd others=undef
  uint32_t numRef_B               : 4;   // 0/1/2=1fwd+1bwd 3=2fwd+1bwd 4=2fwd+2bwd others=undef.
  uint32_t tile_num               : 2;   // number of tiles (only for hevc)
  uint32_t beating_reduction      : 5;   // bit[0]:   0=off 1=on.
                                         // bit[4:1]: 0=default 1=weakest 9=strongest
                                         // range=0-9, others=undef
  uint32_t efm_enable             : 1;   // 0=encode from enc_src, 1=encode from memory
  uint32_t use_cabac              : 2;   // 0=cavlc 1=cabac
  uint32_t quality_level          : 14;  // [1:0]=inter_block_size, [3:2]=rdo_mode
                                         // [4]=tr8x8, [5]=constrained_intra, [6]=longterm_ref
                                         // [7]=weighted_pred(P), [8]=weighted_bipred(B), [9]=p2_ref1_full_ME
                                         // others=undef

  uint32_t  rc_mode               : 3;   // rc_mode_t
  uint32_t  initial_qp            : 6;   // initial qp used when rc_mode=0
  uint32_t  reserved_2            : 7;
  uint32_t  cpb_buf_idc           : 8;
  uint32_t  en_panic_rc           : 2;
  uint32_t  cpb_cmp_idc           : 2;   // cpb compliance idc
  uint32_t  fast_rc_idc           : 4;

  uint32_t  target_bitrate;              // target bitrate in bps (not kbps)

  uint32_t  vbr_config;                  // valid when ENC_RC_VBR

  uint32_t vbr_init_data_daddr;          // VBR initial calibration data dram address
  uint32_t bits_fifo_base;
  uint32_t bits_fifo_size;
  uint32_t bits_fifo_offset;
  uint32_t info_fifo_base;               // bits_info_xxxx is 32B aligned, so need info_fifo to tell
                                         // the real (unpadded) bitstream start/stop address.
                                         // refer to struct BIT_STREAM_HDR
  uint32_t info_fifo_size;
  uint32_t enc_frame_rate;

  uint32_t reserved_1             : 8;
  uint32_t log2_num_ltrp_per_gop  : 8;   // number of long-term reference picture in a GoP
  uint32_t hflip                  : 1;
  uint32_t vflip                  : 1;
  uint32_t rotate                 : 1;
  uint32_t fast_seek_interval     : 6;
  uint32_t custom_df_control_flag : 1;   // which way to set deblocking filter parameters(disable_df_idc/df_param1/df_param2)
                                         // 1: use external deblocking filter control params;
                                         // 0: use DSP default setting;
  uint32_t disable_df_idc         : 2;   // disable_deblocking_fliter_idc         [0,2]
  uint32_t df_param1              : 4;   // slice_alpha_c0_offset_div2 + 6        [0,12]

  uint32_t scalelist_opt;                // 0=off 1=standard's default non-flat matrix
                                         // ">=2" = user defined scale matrix dram address

  uint32_t idr_interval;
  uint32_t cpb_user_size;                // (bps, just follow the unit of average_bitrate)

  uint32_t stat_fifo_base;               // refer to struct ENCODER_STATISTIC
  uint32_t stat_fifo_limit;

  uint8_t  max_qp_i;                     // simple rc parameters
  uint8_t  min_qp_i;                     // simple rc parameters
  uint8_t  max_qp_p;                     // simple rc parameters
  uint8_t  min_qp_p;                     // simple rc parameters
  int8_t   aqp;                          // simple rc parameters
  int8_t   i_qp_reduce;                  // simple rc parameters
  int8_t   p_qp_reduce;                  // simple rc parameters
  int8_t   reserved_4;

  uint32_t num_slices_per_bits_info : 8; // specify how many slices's bitstream will be reported through one bits info
                                         // [0]     one tile of one slice per bits info;
                                         // [1,254] number of slices per bits info
                                         // [255]   put all slices of one frame into one bits info
  uint32_t slice_num                : 8; // number of slices (only for hevc for now)
                                         // each slice contains all tile columns across
                                         // full CTU rows
  uint32_t IR_cycle                 : 8; // number of intra refreshed frames
  uint32_t IR_type                  : 1; // 0=top-to-down scanning, 1=left-to-right scanning(not supported)
  uint32_t df_param2                : 4; // slice_beta_offset_div2     + 6        [0,12]
  uint32_t two_lt_ref_mode          : 1;
  uint32_t aqp_lambda               : 2; // 0 - use slice QP for lambda, 1, 2 - use CU QP for lambda

  uint32_t mvdump_daddr;
  uint32_t mvdump_fifo_limit;
  uint32_t mvdump_fifo_unit_sz;
  //uint32_t mvdump_dpitch;
  uint32_t test_binary_daddr;            // dram address to contain sample data, encoder config and
                                         // golden checksum for safety check purpose.
                                         // 0 - disable.

  uint32_t is_high_priority_stream : 1;
  uint32_t enc_start_method        : 1;
  uint32_t enc_stop_method         : 1;
  uint32_t hevc_perf_mode          : 5;
  uint32_t embed_code              : 2;
  uint32_t tier_idc                : 1;
  uint32_t recon_offset_y          : 4;   // must be multiple of 2; and >= real meta data height; DSP_ENC_CFG's extra_height also need to be set.
  uint32_t is_recon_to_vout        : 2;   // 1: recon buffer to vout b; 2: to vout a;
  uint32_t IR_no_overlap           : 1;   // disable MB overlapping between 2 neighboring frames
  uint32_t max_smvmax_scale        : 3;
  uint32_t enc_test_cmd_mode       : 2;   // 0: GOP N2M1 for IPIPIP with 1 slice (default)
                                          // 1: GOP N4M1 for IPPPIPPP with 4 slices
                                          // 2: GOP N5M1 for IPPPPIPPPP with 3 slices
  uint32_t reserved                : 9;
} cmd_encoder_setup_t;

/////////////////////////////////////////////////////////////////////////////

/// CMD_ENCODER_START (0x06000002)
typedef struct
{
  uint32_t vui_enable                              : 1;
  uint32_t aspect_ratio_info_present_flag          : 1;
  uint32_t overscan_info_present_flag              : 1;
  uint32_t overscan_appropriate_flag               : 1;
  uint32_t video_signal_type_present_flag          : 1;
  uint32_t video_full_range_flag                   : 1;
  uint32_t colour_description_present_flag         : 1;
  uint32_t chroma_loc_info_present_flag            : 1;
  uint32_t timing_info_present_flag                : 1;
  uint32_t fixed_frame_rate_flag                   : 1;
  uint32_t nal_hrd_parameters_present_flag         : 1;
  uint32_t vcl_hrd_parameters_present_flag         : 1;
  uint32_t low_delay_hrd_flag                      : 1;
  uint32_t pic_struct_present_flag                 : 1;
  uint32_t bitstream_restriction_flag              : 1;
  uint32_t motion_vectors_over_pic_boundaries_flag : 1;
  uint32_t custom_bitstream_restriction_cfg        : 1;  //the SPS bitstreamRestriction syntax will be 0 -- calculated by DSP; 1 -- set by IAV
  uint32_t reserved_vui_0                          : 7;
  // aspect_ratio_info_present_flag
  uint32_t aspect_ratio_idc                        : 8;

  uint32_t SAR_width                       : 16;
  uint32_t SAR_height                      : 16;

  // video_signal_type_present_flag
  uint32_t video_format                    : 8;
  // colour_description_present_flag
  uint32_t colour_primaries                : 8;
  uint32_t transfer_characteristics        : 8;
  uint32_t matrix_coefficients             : 8;

  // chroma_loc_info_present_flag
  uint32_t chroma_sample_loc_type_top_field       : 8;
  uint32_t chroma_sample_loc_type_bottom_field    : 8;
  uint32_t reserved_vui_3                         : 16;

  uint32_t vbr_cbp_rate;

  // bitstream_restriction_flag
  uint32_t max_bits_per_mb_denom          : 8; //for hevc it is max_bits_per_min_cu_denom
  uint32_t max_bytes_per_pic_denom        : 8;
  uint32_t log2_max_mv_length_horizontal  : 8;
  uint32_t log2_max_mv_length_vertical    : 8;

  uint32_t num_reorder_frames             : 16;
  uint32_t max_dec_frame_buffering        : 16;

  //timing_info_present_flag
  uint32_t num_units_in_tick;
  uint32_t time_scale;

} h264_vui_t; // 36B

typedef struct
{
  uint32_t enable_flag         : 1;
  uint32_t offset_left         : 7;
  uint32_t offset_right        : 8;
  uint32_t offset_top          : 8;
  uint32_t offset_bottom       : 8;

} enc_frame_crop_t;

#if 0
typedef union
{
  enc_frame_crop_t  bits;
  uint32_t          word;

} enc_frame_crop_ut;
#endif

/// CMD_ENCODER_START (0x06000002)
typedef struct
{

  uint32_t cmd_code;

  uint32_t stream_id                            : 8;
  uint32_t stream_type                          : 8;
  uint32_t gaps_in_frame_num_value_allowed_flag : 1; // value of gaps_in_frame_num_value_allowed_flag in SPS
  uint32_t force_annexb_long_start_code         : 1; // force to use 4 bytes long start code (00 00 00 01) for all NALU
  uint32_t au_type                              : 2; // 0: no AUD/SEI
                                                     // 1: send AUD -> SPS/PPS -> SEI
                                                     // 2: send SPS/PPS -> AUD -> SEI
  uint32_t reserved                             : 12;

  uint32_t encode_duration;
  uint32_t start_encode_frame_no;

  enc_frame_crop_t  frame_crop;
  h264_vui_t        h264_vui;    // h264 seq_hdr->VUI parameters

  uint32_t memd_time_out_msec;   // 0: no time out

} cmd_encoder_start_t;

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t encoder_stop_method_t;

#define STOP_IMMEDIATELY   0u
#define STOP_ON_NEXT_IP    1u
#define STOP_ON_NEXT_I     2u
#define STOP_ON_NEXT_IDR   3u
#define STOP_ON_MBSYNC     4u
#define STOP_EMERG         0xFFu

/// CMD_ENCODER_STOP (0x06000003)
typedef struct
{
  uint32_t cmd_code;
  uint32_t channel_id     : 8;
  uint32_t reserved       : 24;
  uint32_t stop_method;           // see definition in encoder_stop_method_t

} cmd_encoder_stop_t;

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t encode_param_enable_flags_t;

#define ENC_PARAM_QP_LIMIT                 0x1u
#define ENC_PARAM_INSERT_IDR               0x2u
#define ENC_PARAM_GOP                      0x4u
#define ENC_PARAM_FRAME_RATE               0x8u
#define ENC_PARAM_BITRATE_CHANGE           0x10u

/// CMD_ENCODER_REALTIME_SETUP (0x06000004)
typedef struct
{
  uint32_t cmd_code;
  uint32_t stream_id                 : 8;
  uint32_t reserved                  : 24;

  uint32_t enable_flags;  // refer to encode_param_enable_flags_t

  // simple rc parameters
  uint8_t  qp_min_on_I;
  uint8_t  qp_max_on_I;
  uint8_t  qp_min_on_P;
  uint8_t  qp_max_on_P;

  uint8_t  aqp;
  int8_t   i_qp_reduce;
  int8_t   p_qp_reduce;
  int8_t   reserved1;

  // gop change
  uint16_t gop_n_new;
  uint16_t reserved2;

  // insert idr, idr interval, frame rate muli/div factor
  uint32_t idr_interval_new       : 8;
  uint32_t force_idr              : 1; // 0: disabled, 1: Change next I/P to IDR
  uint32_t reserved3              : 7;
  uint32_t multiplication_factor  : 8;
  uint32_t division_factor        : 8;

  uint32_t enc_frame_rate;

  uint32_t target_bitrate;

} encoder_realtime_setup_t;

/////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint16_t max_enc_width;
  uint16_t max_enc_height;

  uint32_t max_smvmax       :8;
  uint32_t max_rec_fb_num   :8;
  uint32_t data_q_out_num   :1;
  uint32_t reserved         :15;
} enc_cfg_t;

////////////////////////////////////////////////////////////////////////////////

// msg between idsp and VP
#define MAX_ROI_CMD_TAG_NUM   8u
#define MAX_TOKEN_ARRAY       4u

typedef struct
{
  uint32_t fov_id;
  uint32_t fov_private_info_addr;   // this field points to a user defined private info structure.

} sideband_info_t;

typedef struct
{
  uint32_t buffer_addr;
  uint16_t buffer_pitch;
  uint16_t img_width;
  uint16_t img_height;
  uint16_t reserved_0;

} image_buffer_desc_t;

typedef struct
{
  int16_t x_offset;
  int16_t y_offset;

} image_offset_t;

typedef struct
{
  uint16_t          channel_id;
  uint8_t           is_hier_y12;
  uint8_t           reserved;
  uint32_t          cap_seq_no;
  uint32_t          batch_cmd_id;
  sideband_info_t   side_band_info[MAX_TOKEN_ARRAY];
  uint32_t          roi_cmd_tag[MAX_ROI_CMD_TAG_NUM];
  uint16_t          ln_det_hier_mask;   //if the bit position has a value of 1, then the hier output 1 is valid. bit 6 is for lane det out
  uint16_t          is_hier_out_tile_mode;
  uint32_t          vin_cap_time;
  uint32_t          proc_roi_cmd_time;
  uint32_t          c2y_done_time;
  uint32_t          warp_mctf_done_time;

} msg_vp_hier_out_lane_out_header_t;

#define HIER_ROI_NUM_CV1 13u

typedef struct
{
  msg_vp_hier_out_lane_out_header_t header;

  image_buffer_desc_t      luma_hier_outs[HIER_ROI_NUM_CV1];
  image_buffer_desc_t      chroma_hier_outs[HIER_ROI_NUM_CV1];
  image_offset_t           hier_out_offsets[HIER_ROI_NUM_CV1];

  image_buffer_desc_t      luma_lane_det_out;
  image_buffer_desc_t      chroma_lane_det_out;
  image_offset_t           lane_det_out_offset;

} msg_vp_hier_out_lane_out_t;

typedef struct
{
  uint32_t cap_seq_no;

  uint32_t chan_no    : 4;
  uint32_t tile_x_idx : 3;
  uint32_t reserved0  : 25;
} vout_timestamp_t;

////////////////////////////////////////////////////////////////////////
/// product specific header files
////////////////////////////////////////////////////////////////////////
//#include "cmd_msg_prod_dv.h"
//#include "idspdrv_cmd_msg.h"

#if 1
typedef struct
{
  uint32_t msg_code;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t msg_crc;
#endif

  uint32_t stream_id           : 8;
  uint32_t stream_type         : 8;
  uint32_t enc_status          : 2; // enc_status_t
  uint32_t checksum_err        : 1; // for Test Encoder safety check
  uint32_t reserved0           : 13;

  uint32_t total_bits_info_ctr_h264;
  uint32_t total_bits_info_ctr_hevc;
  uint32_t total_bits_info_ctr_jpeg;

  uint32_t pts_hw;
  uint32_t mvdump_curr_daddr;

  uint32_t code_checksum;           // for Test Encoder safety check
} enc_status_msg_t;
#else
typedef struct
{
  uint32_t msg_code;

  uint32_t channel_id          : 8;
  uint32_t stream_type         : 8;  // refer to stream_type_t
  uint32_t stream_report_no    : 8;
  uint32_t enc_status          : 8;  // see definition in enc_status_t


  uint32_t total_bits_info_ctr_h264;
  uint32_t total_bits_info_ctr_hevc;
  uint32_t total_bits_info_ctr_jpeg;

  uint32_t start_pts;
  uint32_t fade_io_gain;
  uint32_t encode_yuv_y_daddr;
  uint32_t encode_yuv_uv_daddr;
  uint32_t encode_yuv_dpitch;

} enc_status_msg_t;
#endif
typedef struct
{
  uint32_t msg_code;
#ifdef SUPPORT_DSP_MSG_CRC
  uint32_t msg_crc;
#endif
  uint32_t stream_id        : 8;
  uint32_t enc_status       : 2;
  uint32_t pic_type         : 3;
  uint32_t pic_is_idr       : 1;
  uint32_t is_memd_time_out : 1;
  uint32_t checksum_err     : 3; // for Test Encoder safety check
  uint32_t rsvd0            : 14;

  uint32_t rec_y_addr;
  uint32_t rec_uv_addr;
  uint32_t rec_y_pitch    : 16;
  uint32_t rsvd1          : 16;

  uint32_t input_y_addr;
  uint32_t input_uv_addr;
  uint32_t input_y_pitch  : 16;
  uint32_t rsvd2          : 16;

  uint32_t input_me_addr;
  uint32_t input_me_pitch : 16;
  uint32_t rsvd3          : 16;

  uint32_t frame_no;
  uint32_t cap_seq_no;
  uint32_t pts_hw;
  uint32_t memd_start_pts;
  uint32_t memd_done_pts;

  uint32_t memd_checksum[5];     // for Test Encoder safety check

} enc_buffer_status_msg_t;


/*
   move from hlenc_test_binary.h for system integration
   (1) This file is to specify the encoder test binary offset inside
       the dram loaded form enc_test_binary.bin.
   (2) This file is auto generated by running diag/enc/data/gen_test_binary.pl
*/

typedef struct {
  uint32_t luma0_w;
  uint32_t luma0_h;
  uint32_t luma0_p; // pitch
  uint32_t chrm0_w;
  uint32_t chrm0_h;
  uint32_t chrm0_p; // pitch
  uint32_t me0_w;
  uint32_t me0_h;
  uint32_t me0_p;   // pitch
  uint32_t luma1_w;
  uint32_t luma1_h;
  uint32_t luma1_p; // pitch
  uint32_t chrm1_w;
  uint32_t chrm1_h;
  uint32_t chrm1_p; // pitch
  uint32_t me1_w;
  uint32_t me1_h;
  uint32_t me1_p;   // pitch
  uint32_t memd_checksum[2][5]; // [frame_no][index]
  uint32_t code_checksum[2];    // [frame_no]
  uint32_t rsvd[2]; // padding to 128 bytes
  uint32_t memd_checksum_1[4][5]; // [frame_no][index]
  uint32_t code_checksum_1[4][4]; // [frame_no][slice]
  uint32_t memd_checksum_2[10][5]; // [frame_no][index]
  uint32_t code_checksum_2[10][3]; // [frame_no][slice]
  uint32_t rsvd_1[12]; // padding to 640 bytes
} test_binary_header_t;


#define ENC_TEST_HEADER_OFFSET      0u
#define ENC_TEST_CMD_OFFSET     640u
#define ENC_TEST_LUMA0_OFFSET       768u
#define ENC_TEST_CHRM0_OFFSET       111360u
#define ENC_TEST_ME0_OFFSET     166656u
#define ENC_TEST_LUMA1_OFFSET       175872u
#define ENC_TEST_CHRM1_OFFSET       286464u
#define ENC_TEST_ME1_OFFSET     341760u

#define ENC_TEST_BINARY_SIZE        350976u


#endif // CMD_MSG_DSP_H_
