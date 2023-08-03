/* CV5 DSP API header file
Commit: 866f87a68201ebaf124b7b0b3c0c277a4bc392e9
Last Changed Commit: 406d04d8eb6dbd3c1eb73ac065f2a8ccc55ab069
Last Changed Date: 2023-01-02 05:37:19 -0800
*/
/**
 * @file cmd_msg_dsp.h
 *
 *  Copyright (c) 2021 Ambarella International LP
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

#define PROJECT_CV5

#define IMG_MODE_LOW_ISO_VIDEO 95u
#define IMG_MODE_HIGH_ISO_STILL 99u

#define CFG_PASS_INVLAID 0xFFu
#define CFG_STEP_INVLAID 0xFFu

////////////////////////////////////////////////////////////////////////////////

typedef uint8_t pass_step_id_t;

#define IMG_PASS_VIN            0u
#define IMG_PASS_C2Y            1u
#define IMG_PASS_Y2Y            2u
#define IMG_PASS_OSD_BLEND      3u
#define IMG_PASS_MD_BASE        4u
#define IMG_PASS_ME1_SMTH       IMG_PASS_MD_BASE
#define IMG_PASS_MD_A           5u
#define IMG_PASS_MD_B           6u
#define IMG_PASS_MD_C           7u
#define IMG_PASS_FISHI          8u
#define IMG_PASS_HI_BASE        16u
#define IMG_PASS_STEP_INVALID   255u

#define VPROC_MAIN_STREAM_IDX 0u /* mcts/mctf */
#define VPROC_PIP_STREAM_IDX  1u /* preview A */
#define VPROC_3RD_STREAM_IDX  2u /* preview B */
#define VPROC_4TH_STREAM_IDX  3u /* preview C */
#define VPROC_5TH_STREAM_IDX  4u /* preview D */
#define VPROC_6TH_STREAM_IDX  5u /* mixer scaler */
#define VPROC_7TH_STREAM_IDX  6u /* hier */
#define VPROC_STREAM_NUMS     7u

////////////////////////////////////////////////////////////////////////////////
#define SIL_VER_INVALID         0u
#define SIL_VER_ENGINEERING     99u

#ifdef PROJECT_CV5
// FIXME define SIL_VER_XXXX???

#define SIL_VER_CV5AX55     10550
#define SIL_VER_CV5AX55R    11550
#define SIL_VER_CV5AX85     10850
#define SIL_VER_CV5A55      20550
#define SIL_VER_CV5A85      20850
#define SIL_VER_CV52AX55    30550
#define SIL_VER_CV52AX75    30750
#define SIL_VER_CV52AX85    30850
#define SIL_VER_CV52A55     40550
#define SIL_VER_CV52A75     40750
#define SIL_VER_CV52A85     40850
#define SIL_VER_CV5U55      50550
#define SIL_VER_CV5U85      50850
#define SIL_VER_CV52U75     60750
#define SIL_VER_CV52U85     60850
#endif

/* located at 0x100 */
typedef struct
{
  uint32_t ucode_version;
  uint32_t ucode_date;
  uint32_t ucode_api_version;
  uint32_t ucode_silicon_version;
  uint32_t ucode_link_base;
} ucode_info_t;

typedef struct
{
  uint32_t ucode_version_w0;
  uint32_t ucode_version_w1;
  uint32_t ucode_version_w2;
  uint32_t ucode_version_w3;
  uint32_t ucode_version_w4;
} ucode_ver_t;

typedef struct
{
  uint32_t ucode_api_version_w0;
  uint32_t ucode_api_version_w1;
  uint32_t ucode_api_version_w2;
  uint32_t ucode_api_version_w3;
  uint32_t ucode_api_version_w4;
} ucode_api_ver_t;

typedef struct
{
  uint32_t common_version_w0;
  uint32_t common_version_w1;
  uint32_t common_version_w2;
  uint32_t common_version_w3;
  uint32_t common_version_w4;
} common_ver_t;

typedef struct
{
  uint32_t idspdrv_version_w0;
  uint32_t idspdrv_version_w1;
  uint32_t idspdrv_version_w2;
  uint32_t idspdrv_version_w3;
  uint32_t idspdrv_version_w4;
} idspdrv_ver_t;

typedef struct
{
  uint32_t orcmdxf_version_w0;
  uint32_t orcmdxf_version_w1;
  uint32_t orcmdxf_version_w2;
  uint32_t orcmdxf_version_w3;
  uint32_t orcmdxf_version_w4;
} orcmdxf_ver_t;

typedef struct
{
  uint32_t orcme_version_w0;
  uint32_t orcme_version_w1;
  uint32_t orcme_version_w2;
  uint32_t orcme_version_w3;
  uint32_t orcme_version_w4;
} orcme_ver_t;

typedef struct
{
  uint32_t commit_time_w0;
  uint32_t commit_time_w1;
} ucode_commit_time_t;

typedef struct
{
  ucode_info_t    ucode_info;
  ucode_ver_t     ucode_ver;
  ucode_api_ver_t ucode_api_ver;
  common_ver_t    common_ver;
  idspdrv_ver_t   idspdrv_ver;
  orcmdxf_ver_t   orcmdxf_ver;
  orcme_ver_t     orcme_ver;
  ucode_commit_time_t commit_time;
} ucode_info_full_t;

#define PIC_STATS_DUMP_MAX_NUM (8)
#define PIC_STATS_DUMP_SHIFT   (13)
#define PIC_STATS_DUMP_SIZE    (1 << (PIC_STATS_DUMP_SHIFT)) /* bytes */

typedef struct
{
/* xf_stat */
    uint16_t xfMbCatHisto[16];
    uint32_t rsvd[32]; /* (64 + 64)/4 */
/* Nom QP Table */
    uint8_t qp[16];
}pic_stats_avc_t;

#define DSP_SYNC_COUNTER_CMD_REQ_VDSP0   19u  /* Sync Counter for VDSP0 command request */
#define DSP_SYNC_COUNTER_CMD_REQ_VDSP1   20u  /* Sync Counter for VDSP1 command request */
#define DSP_SYNC_COUNTER_CMD_REQ_ASYNC   21u  /* Sync Counter for Async command queue (CV5: VDSP2, CV6: OrcVin1 VDSP0) */
#ifndef PROJECT_CV5
#define DSP_SYNC_COUNTER_CMD_REQ_VIN_1   22u  /* Sync Counter for OrcVin1 command queue (OrcVin1 VDSP1) */
#endif
#define DSP_SYNC_COUNTER_IDSP_VP_MSG     255u /* Sync Counter for IDSP VP message */

/* DSP API types */
#define DSP_API_CMD_MSG_MAIN    0u /* for flows running on orcvin only, VOUT frame-based timing */
#define DSP_API_CMD_MSG_VIN     1u /* for flows running on orcvin only, VIN frame-based timing */
#define DSP_API_CMD_MSG_ASYNC   2u /* for async flow running on orcvin */
#ifndef PROJECT_CV5
#define DSP_API_CMD_VIN_1       3u /* for flows running on orcvin1 only, VIN frame-based timing */
#endif
#define DSP_API_MSG_VPROC       3u /* for vproc flow running on orcidsp1 only */
#define DSP_API_MSG_IDSP_VP     4u /* for vproc flow running on orcidsp1 only */
#define DSP_API_MSG_CODEC       5u /* for flows running on orccodec only, enc/dec */
#ifndef PROJECT_CV5
#define DSP_API_MSG_VIN_1       6u /* for flows running on orcvin1 only, VIN frame-based timing */
#define DSP_API_MSG_VIN_VP      7u /* for flows running on orcvin/orcvin1 only, VIN frame-based timing */
#endif

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

} assert_info_t;

// For system/ucode agreement:
// This assert_info_dram_t must be 128-byte aligned
typedef struct
{
  assert_info_t assert_info;
  uint32_t      reserved[23];
} assert_info_dram_t;


/****************************************************************************
 * ORC_TH_GLOBAL_ID for COMMON_SMEM_TH index and assertion
 *
 * CV5 ORC_TOTAL_THREADS=16 :
 * ORC_TH_GLOBAL_ID = 0,  1,  2,  3,  for orccode
 * ORC_TH_GLOBAL_ID = 4,  5,  6,  7,  for orcvin
 * ORC_TH_GLOBAL_ID = 8,  9,  10, 11, for orcidsp0
 * ORC_TH_GLOBAL_ID = 12, 13, 14, 15, for orcidsp1
 *
 * CV6 ORC_TOTAL_THREADS=20:
 * ORC_TH_GLOBAL_ID = 0,  1,  2,  3,  for orccode
 * ORC_TH_GLOBAL_ID = 4,  5,  6,  7,  for orcvin(0)
 * ORC_TH_GLOBAL_ID = 8,  9,  10, 11, for orcvin1
 * ORC_TH_GLOBAL_ID = 12, 13, 14, 15, for orcidsp0
 * ORC_TH_GLOBAL_ID = 16, 17, 18, 19, for orcidsp1
 ***************************************************************************/
#ifdef PROJECT_CV6
#define ORC_TOTAL_THREADS 20u
#else /* CV5 */
#define ORC_TOTAL_THREADS 16u
#endif
typedef struct
{
  assert_info_dram_t info[ORC_TOTAL_THREADS];
} orc_assert_t;

/* located at DSP_INIT_DATA_BASE(0x200) defined in ld_defs.h */
#define DSP_INIT_DATA_SIZE 256u
typedef struct
{
  /* Off-line predefined binary data */
  uint32_t default_binary_data_daddr;                 /* 4B */
  uint32_t default_binary_data_dsize;                 /* 8B */

  /* Default commands */
  uint32_t default_config_daddr;                      /* 12B */
  uint32_t default_config_dsize;                      /* 16B */

  /* Continguous DRAM space assgined to DSP */
  uint32_t dsp_dram_daddr;                            /* 20B */
  uint32_t dsp_dram_size;                             /* 24B */

  /* Extended IDSP memory for ucode */
  uint32_t idsp_ext_dram_daddr;                       /* 28B */
  uint32_t idsp_ext_dram_dsize;                       /* 32B */

  /* DSP printf area */
  uint32_t dsp_debug_daddr;                           /* 36B */
  uint32_t dsp_debug_dsize;                           /* 40B */

  /* DSP command */
  uint32_t cmd_data_daddr[MAX_NUM_DSP_CMD_Q];         /* 64B */
  uint32_t cmd_data_dsize[MAX_NUM_DSP_CMD_Q];         /* 88B */

  uint32_t cmd_req_timer               :4;            /* sync counter timer in ms. ( only 1 ~ 4 ms allowed ) */
  uint32_t msg_q_full_assert           :1;            /* 0: no assert, 1: assert */
  uint32_t req_cmd_timeout_assert      :1;            /* 0: no assert, 1: assert */
  uint32_t run_with_vp                 :1;
  uint32_t enable_vout_ln_sync_monitor :1;
  uint32_t is_orccode_fully_load       :1;            /* This bit is used under is_partial_load_en=1,
                                                         when orccode.bin is fully loaded, system set this bit as 1 */
  uint32_t data_prot_safety_enabled    :2;            /* 0 : no ASIL 26262 safety mechanism is enabled.
                                                         1 : safety mechanism is enabled.
                                                         Other values are undefined. */
  uint32_t orc_printf_ver              :2;            /* orc_printf version number */
  uint32_t use_debug_chip_id           :1;
  uint32_t reserved_0                  :18;           /* 92B */

  /* DSP message */
  uint32_t msg_q_info_data_daddr[MAX_NUM_DSP_MSG_Q];  /* 124B */

  /* chip_id pointer */
  uint32_t chip_id_daddr;                             /* 128B */

  /* The dram buffer for orc_assert_t */
  uint32_t orc_assert_daddr;                          /* 132B */

  /*  If is_prt_opt=1,
   *  System MUST allocate a dram buffer of 64 bytes (size of prt_period).
   *  This dram buffer base is chip_id_daddr+8 !
   */
  uint32_t is_prt_opt        :2;
  uint32_t delta_audio_ticks :30;                     /* 136B */

  uint32_t att_prot_dram_daddr;                       /* The dram space that is protected by ATT table to exclusively dedicate to the DSP */
  uint32_t att_prot_dram_dsize;                       /* The size of the ATT protected dram space. */
  uint32_t dram_exclusion_region_num :8;              /* The number of the dram segments that are excluded from DSP to use.
                                                         Max value is 4 [i.e. valid values are 0, 1, 2, 3, and 4].
                                                         If value = 0, then all following parameters should be filled with 0x0.*/
  uint32_t reserved_1                :24;             /* 148B */

  uint32_t dram_exclusion_region_0_daddr;             /* The first exclusion region's starting address */
  uint32_t dram_exclusion_region_0_dsize;             /* The first exclusion region's size */
  uint32_t dram_exclusion_region_1_daddr;
  uint32_t dram_exclusion_region_1_dsize;
  uint32_t dram_exclusion_region_2_daddr;
  uint32_t dram_exclusion_region_2_dsize;
  uint32_t dram_exclusion_region_3_daddr;
  uint32_t dram_exclusion_region_3_dsize;             /* 180B */

  /* DSP message write pointer, started from CV6 */
  uint32_t msg_q_wr_daddr[MAX_NUM_DSP_MSG_Q];         /* 212B */

  uint32_t padding[10];                               /* Padding up to 252B */
  uint32_t crc_init_data;                             /* 256B */

} dsp_init_data_t;


/******************************************************************************
 * Macro Defines
 *****************************************************************************/
/* same as H.264 standard (table 6-1) */
typedef uint8_t yuv_chroma_fmt_t;

#define YUV_MONO 0u /* Y only */
#define YUV_420  1u
#define YUV_422  2u
#define YUV_444  3u

#define IDSP_SBUF_MCBL_NUM 3u /* for the sblk=1,2,3 accordingly */

#ifdef PROJECT_CV6
#define MAX_REAL_VIN_NUM         24u
#define MAX_REAL_VIN_BIT_MASK    0xffffffu
#else /* default is CV5 */
#define MAX_REAL_VIN_NUM         14u
#define MAX_REAL_VIN_BIT_MASK    0x3fffu
#endif

#define VIRTUAL_VIN_ID_0    (MAX_REAL_VIN_NUM)

#ifdef PROJECT_CV6
#define MAX_VIRTUAL_VIN_NUM 8u
#define MAX_VIN_TEMPORAL_DEMUX_NUM 9u /* 1u + MAX_VIRTUAL_VIN_NUM */
#else
#define MAX_VIRTUAL_VIN_NUM 12u
#define MAX_VIN_TEMPORAL_DEMUX_NUM 13u /* 1u + MAX_VIRTUAL_VIN_NUM */
#endif

#define MAX_VIN_PATTERN_DEMUX_NUM 2u


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
#define DSP_PROF_UNITTEST2 242u /* for vproc testing only */
#define DSP_PROF_UNITTEST3 243u /* for idspdrv testing only */
#define DSP_PROF_UNITTEST4 244u /* for vin testing only */
#define DSP_PROF_SAFETY    253u
#define DSP_PROF_2_IDLE    254u
#define DSP_PROF_IDLE      255u

#ifndef PROJECT_CV5
#define DSP_CMD_SIZE  256u
#define DSP_MSG_SIZE  256u
#else
#define DSP_CMD_SIZE  128u
#define DSP_MSG_SIZE  128u
#endif

#define DSP_CMD_NUM   32u
#define DSP_MSG_NUM   32u

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
  uint32_t msg_crc;
  uint32_t pay_load[(DSP_MSG_SIZE / 4u) - 2u];

} dsp_msg_t;
#define DSP_MSG dsp_msg_t
/* first DSP cmd for each iteration */
typedef struct
{
  uint32_t cmd_code;    /* must be 0x000000AB */
  uint32_t cmd_seq_num;
  uint32_t num_cmds;
  uint32_t cmd_block_crc;
  uint8_t  pipeline_id; /* don't care in CV5; start from CV6 */
  uint8_t  reserved_8b;
  uint16_t reserved_16b;

} dsp_header_cmd_t;

/******************************************************************************
 * Command Code
 *****************************************************************************/
typedef uint32_t dsp_cmd_code_t;

/* CAT_DSP_HEADER(0),  this is a special one, must be the first command */
#define CMD_DSP_HEADER                          0x000000ABu
#define CMD_DSP_ENDER                           0x000000ACu

/* CAT_DSP_CFG(1) */
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
#define CMD_DSP_VOUT_FLOW_MAX_CFG               0x0100000Eu
#ifndef PROJECT_CV5
#define CMD_DSP_VIN_1_FLOW_MAX_CFG              0x0100000Fu
#endif

#define CMD_DSP_UNITTEST0                       0x01000010u
#define CMD_DSP_UNITTEST1                       0x01000011u
#define CMD_DSP_UNITTEST2                       0x01000012u
#define CMD_DSP_VIN_UNITTEST                    0x01000013u
#define CMD_DSP_UNITTEST3                       0x01000014u
#define CMD_DSP_VIN_UNITTEST2                   0x01000015u
#define CMD_DSP_VOUT_UNITTEST                   0x01000016u
#define CMD_DSP_UNITTEST4                       0x01000017u
#define CMD_DSP_COVERAGE_TEST                   0x01000018u
#define CMD_DSP_UNITTEST_CONFIG                 0x01000019u

#define CMD_DSP_VPROC_FLOW_MAX_CFG_EXT          0x01000100u

/* CAT_VPROC(2) */
#define CMD_VPROC_CONFIG                        0x02000001u
#define CMD_VPROC_SETUP                         0x02000002u
#define CMD_VPROC_IK_CONFIG                     0x02000003u
#define CMD_VPROC_IMG_PRMD_SETUP                0x02000004u
#define CMD_VPROC_LN_DET_SETUP                  0x02000005u
#define CMD_VPROC_PREV_SETUP                    0x02000006u
#define CMD_VPROC_SET_EXT_MEM                   0x02000007u
#define CMD_VPROC_GRP_CMD                       0x02000008u
#define CMD_VPROC_STOP                          0x02000009u
#define CMD_VPROC_OSD_BLEND                     0x0200000Au
#define CMD_VPROC_PIN_OUT_DECIMATION            0x0200000Bu
#define CMD_VPROC_ECHO                          0x0200000Cu
#define CMD_VPROC_WARP_GROUP_UPDATE             0x0200000Du
#define CMD_VPROC_OSD_INSERT                    0x0200000Eu
#define CMD_VPROC_SET_PRIVACY_MASK              0x0200000Fu
#define CMD_VPROC_SET_STREAMS_DELAY             0x02000010u
#define CMD_VPROC_FISHI_WARP_REGION_SETUP       0x02000011u
#define CMD_VPROC_FISHI_WARP_PREV_SETUP         0x02000012u
#define CMD_VPROC_BLUR_PM_PP                    0x02000013u

/* VPROC common commands */
#define CMD_VPROC_MULTI_STREAM_PP               0x02000064u
#define CMD_VPROC_SET_EFFECT_BUF_IMG_SZ         0x02000065u
#define CMD_VPROC_MULTI_CHAN_PROC_ORDER         0x02000066u
#define CMD_VPROC_SET_VPROC_GRPING              0x02000067u
#define CMD_VPROC_SET_TESTFRAME                 0x02000068u

/* CAT_VIN(4) */
#define CMD_VIN_START                           0x04000001u
#define CMD_VIN_IDLE                            0x04000002u
//\#define CMD_VIN_SET_RAW_FRM_CAP_CNT             0x04000003u
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

/* for amalgam diag */
/*\#define CMD_VIN_VCAP_DRAM_IN_SETUP              0x04000010u*/

#define CMD_VIN_VOUT_LOCK_SETUP                 0x04000011u
#define CMD_VIN_ATTACH_METADATA                 0x04000012u
#define CMD_VIN_TEMPORAL_DEMUX_SETUP            0x04000013u
#define CMD_VIN_PATTERN_DEMUX_SETUP             0x04000014u

/* CAT_VOUT(5) */
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

/* CAT_ENC (6) */
#define CMD_ENCODER_SETUP                       0x06000001u
#define CMD_ENCODER_START                       0x06000002u
#define CMD_ENCODER_STOP                        0x06000003u
#define CMD_ENCODER_REALTIME_SETUP              0x06000004u
#define CMD_ENCODER_JPEG_SETUP                  0x06000005u
#define CMD_ENCODER_FLUSH_INPUT                 0x06000006u
#define CMD_ENCODER_VDSP_SYNC_CMD               0x06000007u

/* CAT_DEC (7) */
#define CMD_DECODER_SETUP                       0x07000001u
#define CMD_DECODER_START                       0x07000002u
#define CMD_DECODER_STOP                        0x07000003u
#define CMD_DECODER_BITSFIFO_UPDATE             0x07000004u
#define CMD_DECODER_SPEED                       0x07000005u
#define CMD_DECODER_TRICKPLAY                   0x07000006u
#define CMD_DECODER_STILLDEC                    0x07000007u
#define CMD_DECODER_BATCH                       0x07000008u

/******************************************************************************
 * Message Code
 *****************************************************************************/
typedef uint32_t dsp_msg_code_t;

#define MSG_DSP_STATUS                          0x81000001u
#define MSG_CMD_REQ                             0x81000002u
#define MSG_ECHO_CMD                            0x81000003u

/* CAT_VPROC (2) */
#define MSG_VPROC_STATUS                        0x82000001u
#define MSG_VPROC_AAA_STATUS                    0x82000002u
#define MSG_VPROC_COMP_OUT_STATUS               0x82000003u
#define MSG_VPROC_EFFECT_DATA_STATUS            0x82000004u

/* CAT_VIN(4) */
#define MSG_VIN_SENSOR_RAW_CAPTURE_STATUS       0x84000001u

/* CAT_VOUT (5) */
#define MSG_VOUT_STATUS                         0x85000001u

/* CAT_ENC(6) */
#define MSG_ENC_STATUS                          0x86000001u
#define MSG_ENC_BUFFER_STATUS                   0x86000002u
#define MSG_ENC_VDSP_INFO                       0x86000003u

/* CAT_DEC (7) */
#define MSG_DECODER_STATUS                      0x87000001u
#define MSG_DECODER_STILLDEC                    0x87000002u
#define MSG_DECODER_PICINFO                     0x87000003u

/******************************************************************************
 * DSP cmds
 *****************************************************************************/
/******************************************************************************
 * CAT_DSP_CFG(1)
 *****************************************************************************/
/// CMD_DSP_CONFIG, command code 0x01000001
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

  /********************************************************************
   * the num of entries in the mcb list reserved for the idsp
   * ld str.
   * [0: sblk = 0, 1]
   * [1: sblk = 2]
   * [2: sblk = 3]
   *******************************************************************/
  uint8_t  tot_idsp_ld_str_num[IDSP_SBUF_MCBL_NUM];

                                // W14
  uint32_t vin_bit_mask;
                                // W15
  uint32_t num_of_virtual_vins_mem_input   :4;       /* memory input used */
  uint32_t num_of_virtual_vins_tmp_demux   :4;       /* temporal demux used */
  uint32_t max_fov_num_per_vin             :8;
  uint32_t vproc_data_in_size              :16;      /* the pic q size of a vproc channel. 0: the pic q size default = 16 */
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
  uint32_t vout_double_cmd_en     : 1;
  uint32_t is_testframe_on        : 1;

                                // W19
  uint8_t  num_of_dec_channel;
  uint8_t  dec_codec_support;   // dec_codec_sup_t, set for 1st channel, jpeg decode need to set at 1st channel that didn't support multiple instance
  uint8_t  vdec_capture_ena;
  uint8_t  page_size_k_log2; /* CV6 only supports page size of 4096 */

                               //  W20
  uint32_t max_eng0_width      : 16;
  uint32_t is_sproc_hi_enabled : 1;
  uint32_t delayline_ena       : 1; // delayline after y2y, before OSD8 enable flag
  uint32_t xcode_sep_coding    : 1;
  uint32_t max_data_out_num_per_vproc_strm : 5;
  uint32_t num_of_virtual_vins_pat_demux   :4;
  uint32_t xcode_sep_xform     : 1;
  uint32_t idsp_has_double_banks : 1;
  uint32_t vin_0_use_frm_sync_timeout      : 1; // 1: use frm sync time out for vin of cv5 or vinorc0 of cv6, 0: use polling
  uint32_t is_blur_pm_enabled : 1;

                               //  W21
  uint32_t idsp_hw_timeout     : 8;
  uint32_t idsp_win_in_str_num : 6;
  uint32_t idsp_job_q_size     : 16;
  uint32_t reserved_1          : 2;
                                //  W22
  uint32_t dec_codec_support_multi_chan; //set decode codec support for 2nd~8th,[0:3] 2nd, [4:7] 3rd ...
                               //  W23
#ifndef PROJECT_CV5
  uint32_t vin_1_bit_mask;
                               //  W24
  uint32_t num_of_virtual_vins_1_mem_input : 4; /* memory input used */
  uint32_t num_of_virtual_vins_1_tmp_demux : 4; /* temporal demux used */
  uint32_t max_fov_num_per_vin_1           : 8;
  uint32_t num_of_virtual_vins_1_pat_demux : 4;
  uint32_t vin_1_use_frm_sync_timeout      : 1; // 1: use frm sync time out for vinorc1 of cv6, 0: use polling
  uint32_t reserved_2                      : 11;
                               //  W25
#endif

  uint32_t vin_0_frm_sync_timeout_ticks; //frm sync time out in ticks for vin of cv5 or vinorc0 of cv6
#ifndef PROJECT_CV5
  uint32_t vin_1_frm_sync_timeout_ticks; //frm sync time out in ticks for vinorc1 of cv6
#endif

  uint32_t idsp_hwarp_win_in_str_num : 3;
  uint32_t reserved_3 : 29;
} cmd_dsp_config_t;

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
  uint32_t  orcme_mask_valid        :1; /* unused orcme0_mask_valid */
  uint32_t  orcmdxf_mask_valid      :1; /* unused orcmdxf0_mask_valid */
  uint32_t  orcme1_mask_valid       :1; /* unused */
  uint32_t  orcmdxf1_mask_valid     :1; /* unused */
  uint32_t  orcvin_mask_valid       :1;
  uint32_t  orcidsp0_mask_valid     :1;
  uint32_t  orcidsp1_mask_valid     :1;
  uint32_t  orcvin1_mask_valid      :1;
  uint32_t  reserved                :7;
  uint32_t  orccode_mask            :12;
  uint32_t  orcmdxf_mask            :2; /* unused orcmdxf0_mask */
  uint32_t  orcme_mask              :2; /* unused orcme0_mask */

  uint16_t  orcmdxf1_mask;  /* unused */
  uint16_t  orcme1_mask;    /* unused */
  uint16_t  orcvin_mask;
  uint16_t  orcidsp0_mask;
  uint16_t  orcidsp1_mask;
  uint16_t  orcvin1_mask;

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
  uint32_t src_str_id  : 4;
  uint32_t src_fp_id   : 8;
  uint32_t src_ch_id   : 8;

  uint32_t dst_fp_type : 4; // FLOW_TAG_t
  uint32_t dst_str_id  : 4;
  uint32_t dst_fp_id   : 8;
  uint32_t dst_ch_id   : 8;

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

#define FP_BASE_VIN      1u
#define FP_BASE_VPROC_DV 2u
#define FP_BASE_VOUT     3u
#define FP_BASE_VENC     4u
#define FP_BASE_ENG0     5u
#define FP_BASE_DEC      6u
#define FP_BASE_ENG1     7u
#ifdef PROJECT_CV5
#define FP_BASE_NUM      7u
#else
#define FP_BASE_VIN_1    8u
#define FP_BASE_NUM      8u
#endif
////////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_ext_mem_type_t;

#define VIN_EXT_MEM_TYPE_RAW    0u
#define VIN_EXT_MEM_TYPE_YUV    1u
#define VIN_EXT_MEM_TYPE_CE     2u
#ifdef PROJECT_CV5
#define VIN_EXT_MEM_TYPE_HDS    VIN_EXT_MEM_TYPE_CE // 2u
#endif
#define VIN_EXT_MEM_TYPE_ME     3u  // ME0+ME1
#define VIN_EXT_MEM_TYPE_AUX    4u  // AUX_OUT
#ifndef PROJECT_CV5
#define VIN_EXT_MEM_TYPE_HDS    5u
#else /* default is CV5 */
#define VIN_EXT_MEM_TYPE_PREV   5u  // PREV_OUT
#endif
#define VIN_EXT_MEM_TYPE_NUM    6u

////////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_send_in_data_type_t;

#define VIN_SEND_IN_DATA_TYPE_RAW    0u
#define VIN_SEND_IN_DATA_TYPE_YUV422 1u
#define VIN_SEND_IN_DATA_TYPE_YUV420 2u
#define VIN_SEND_IN_DATA_TYPE_NUM    3u

#define VIN_SEND_IN_DATA_DEST_VPROC     0u
#define VIN_SEND_IN_DATA_DEST_ENC       1u
#define VIN_SEND_IN_DATA_DEST_VPROC_OSD 2u
#define VIN_SEND_IN_DATA_DEST_NUM       3u

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

  uint32_t is_ce_enabled              : 1;
  uint32_t is_prev_ln_enabled         : 1;
  uint32_t is_prev_a_enabled          : 1;
  uint32_t is_prev_b_enabled          : 1;
  uint32_t is_prev_c_enabled          : 1;
  uint32_t is_prev_hier_enabled       : 1;
  uint32_t is_compressed_out_enabled  : 1;
  uint32_t is_c2y_burst_tiling_out    : 1;

  uint32_t is_hier_burst_tiling_out   : 7;
  uint32_t num_exp_max                : 2; /* max number of exposure: 1, 2, 3 */
  uint32_t is_dzoom_enabled           : 1;
  uint32_t num_of_vproc_groups        : 3; /* up to MAX_VPROC_GROUPS */
  uint32_t is_mctf_cmpr_en            : 1;
  uint32_t is_c2y_dram_sync_to_warp   : 1;
  uint32_t is_c2y_smem_sync_to_warp   : 1;

  // word2
  uint32_t raw_comp_blk_sz_wst   : 8; // block size of raw compression in the worst-case channel
  uint32_t raw_comp_mantissa_wst : 8; // mantissa of raw compression in the worst-case channel

  uint32_t prev_com0_ch_fmt      : 2;  // see definition in yuv_chroma_fmt_t
  uint32_t prev_com1_ch_fmt      : 2;  // see definition in yuv_chroma_fmt_t
  uint32_t is_y12_out_enabled    : 1; /* enable main_y12_out and hier_a_luma12[0-6]_out */
  uint32_t is_sbp_enabled        : 1; /* enable bad_pixel_in stream */
  uint32_t max_c2y_testframes    : 5; /*\ <= VPROC_MAX_TESTFRAME_CASES(16) */
  uint32_t max_y2y_testframes    : 5; /*\ <= VPROC_MAX_TESTFRAME_CASES(16) */


  // word3
  uint32_t max_ch_c2y_tile_num : 8; // total c2y tile number in all channels
  uint32_t max_ch_warp_tile_num: 8; // total warp tile number in all channels
  uint32_t max_c2y_tile_x_num  : 4; // c2y tile number in the worst-case (smem) channel
  uint32_t max_warp_tile_x_num : 4; // warp tile number in the worst-case channel
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

  uint8_t  prev_com0_fb_num;
  uint8_t  prev_com1_fb_num;
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

  uint32_t warp_wait_lines_max      :16;
  uint32_t idsp_hw_timeout          : 8; /* milliseconds, 0: disable */
  uint32_t is_c2y_y12_out_enabled   : 1; /* enable main_y12_out only */
  uint32_t is_mcts_disabled         : 1; /* disable mcts */
  uint32_t is_c2y_warp_smem_shared  : 1; /* share c2y warp smem */
  uint32_t sproc_hi_min_tile_x_num  : 2;
  /****************************************************************************
   * bit mask of using which idsp unit to process frame data:
   * 00: idsp0(default)
   * 01: idsp0
   * 10: idsp1
   * 11: idsp0 and 1 in round-robin mode
   *
   * Note: the idsp_unit_mask must be same as or subset of the idsp_unit_mask
   * in cmd_dsp_vproc_flow_max_cfg_t
   ***************************************************************************/
  uint32_t idsp_unit_mask           : 2;
  uint32_t is_idsp_mixer_enabled    : 1;

  uint16_t sproc_hi_W_max;
  uint16_t sproc_hi_H_max;

  uint16_t idsp_mixer_w_max;
  uint16_t pm_byte_width_max;

  uint32_t is_hwarp_enabled         : 1; /* sec11 */
  uint32_t max_idsp_mixer_area_num  : 8; /* total mixer area number in all channels */
  uint32_t is_prev_d_enabled        : 1;
  uint32_t calc_stitch_data         : 1; /* 1: ucode calculates idsp_stitch_data */
  uint32_t is_c2y_me_enabled        : 1;
  uint32_t is_md_enabled            : 1; /* 1: LISO video with motion detection (Option-C) */
  uint32_t is_scaler_mode_enabled : 1; /* scaler_mode use mixer scaler */

  /***************************************************************************
   * vproc I/O grpups' dram_dram_pri default pri value(0/1) = DRAM_DMA_PRI_1
   **************************************************************************/
  uint32_t c2y_dram_dma_pri  : 3;
  uint32_t warp_dram_dma_pri : 3;
  uint32_t mctf_dram_dma_pri : 3;
  uint32_t prev_dram_dma_pri : 3;
  uint32_t c2y_in_dram_dma_pri : 3; //FIXME--flpan: to be removed if c2y_in is smem_win only???

  uint32_t is_ir_out_enabled   : 1;
  uint32_t uv_nf_radius_max    : 2; /* 0 - 128 (bwd compatibility), 1 - 64, 2 - 32 */

  // postp common buf and max sizes
  uint16_t postp_prev_b_fb_num;
  uint16_t postp_prev_b_me01_fb_num;
  uint16_t postp_prev_c_fb_num;
  uint16_t postp_prev_c_me01_fb_num;

  uint16_t postp_prev_b_w_max;
  uint16_t postp_prev_b_h_max;
  uint16_t postp_prev_c_w_max;
  uint16_t postp_prev_c_h_max;

  /***************************************************************************
   * dram_dma_pri0-7 groups max_xfer minus 1, default m1 value(0/1)= 2 xfer
   **************************************************************************/
  uint32_t dram_dma_pri0_max_xfer_m1 : 2;
  uint32_t dram_dma_pri1_max_xfer_m1 : 2;
  uint32_t dram_dma_pri2_max_xfer_m1 : 2;
  uint32_t dram_dma_pri3_max_xfer_m1 : 2;
  uint32_t dram_dma_pri4_max_xfer_m1 : 2;
  uint32_t dram_dma_pri5_max_xfer_m1 : 2;
  uint32_t dram_dma_pri6_max_xfer_m1 : 2;
  uint32_t dram_dma_pri7_max_xfer_m1 : 2;
  uint32_t is_prev_a_crop_enabled    : 1;

  /* disabled preview-to-hier connections */
  uint32_t hier_src_disable_mask     : 4;
  uint32_t is_vwarp_disabled         : 1;

  /* Minimal CE horizontal downsample rate log2, started from CV6 */
  uint32_t ce_hor_ds_log2_min        : 2;
  uint32_t do_double_cmds            : 1;
  uint32_t sproc_hi_dram_opt         : 1;
  uint32_t is_mcts_burst_tiling_out  : 1;
  uint32_t is_fishi_enabled          : 1;
  uint32_t max_fishi_regions_m1      : 4;

  /* REACH MAXIMUM CMD SIZE 128B */
#ifndef PROJECT_CV5
  u64_addr_upper_t msg_ext_data_base_addr_upper;
#endif
} cmd_dsp_vproc_flow_max_cfg_t;

/// CMD_DSP_VIN_FLOW_MAX_CFG,   command code 0x01000009
/// CMD_DSP_VIN_1_FLOW_MAX_CFG, command code 0x0100000F
typedef struct
{
  //uint32_t  enable         : 1;
  uint32_t  memory_type    : 8; //for memory_type refer to vin_ext_mem_type_t
  uint32_t  max_daddr_slot : 8;
  uint32_t  pool_buf_num   : 8;
  uint32_t  reserved       : 8;

} ext_mem_desc_t;

#define MAX_VIN_EXT_MEM_CFG (8u)
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
  uint32_t  vin_raw_smem_win_out              : 1; // for both raw and yuv422 output case
  uint32_t  ext_mem_cfg_num                   : 4;
  uint32_t  is_aux_out_enabled                : 1;
#ifndef PROJECT_CV5
  uint32_t  is_hds_out_enabled                : 1;
#else
  uint32_t  is_prev_out_enabled               : 1;
#endif
  uint32_t  rsvd                              : 13;

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

  // W8
  uint16_t  max_aux_out_width;
  uint16_t  max_aux_out_height;

#ifndef PROJECT_CV5
  // W9
  uint16_t  max_hds_out_dbuf_num;

  uint16_t  max_hds_out_width;

  // W10
  uint16_t  max_hds_out_height;
  uint16_t  rsvd_1;
#else // CV5
  // W9
  uint16_t  max_prev_out_dbuf_num;

  uint16_t  max_prev_out_width;

  // W10
  uint16_t  max_prev_out_height;
  uint16_t  rsvd_1;
#endif

  // W11-W18
  ext_mem_desc_t ext_mem_cfg[MAX_VIN_EXT_MEM_CFG];

  // W19
  uint32_t max_repeat_cnt;

} cmd_dsp_vin_flow_max_cfg_t;

/// CMD_DSP_ENC_FLOW_MAX_CFG, command code 0x0100000A
#define ENC_MAX_STREAMS                16u

///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint32_t cmd_code;

  uint32_t reconfig_enc_type      :1; // not used
  uint32_t separate_ref_smem      :1; // not used
  uint32_t smem_encode            :2; // not used
  uint32_t reset_info_fifo_offset :1; // reset offset when all streams are in idle state
  uint32_t slice_memd             :1; // not used
  uint32_t ceng_used              :2; // 0 or 1: coding engine#0(HE0) used
                                      // 2: coding engine#1(HE1) used
                                      // 3: both coding engines(HE0/HE1) used
  uint32_t dram_xfer_512B         :1; // 0: use the default sblk for dram xfer width 256B
                                      // 1: use the large sblk for dram xfer width 512B (for hevc only)
  uint32_t enc_dram_dma_pri       :3; // 0: defualt (DRAM_DMA_PRI_0)
                                      // 1 - 6: DRAM_DMA_PRI_1 - DRAM_DMA_PRI_6
                                      // 7: DRAM_DMA_PRI_0 (note: no DRAM_DMA_PRI_7 which reserved for VIN)
  uint32_t disable_chk_scoreboard :1; // 0: check scoreboard in hlenc (the optimal 2 memd cores scheduling)
                                      // 1: disable check scoreboard in hlenc (the optimal hlenc flow)
  uint32_t dram_xfer_512B_AVC     :1; // 0: use the default sblk for dram xfer width 256B
                                      // 1: use the large sblk for dram xfer width 512B (for avc only)
  uint32_t reserved               :18;

  uint16_t  eng0_msg_queue_size[2]; // user defined msg queue size. 0=default 32.
                                    // FIXME--CV5: hardcode ENG0_MSG_QUEUE_ENTRIES(32) in flow_base.h
                                    //             for create_eng0_flow_base(), need to move to cmd_dsp_config_t

  uint16_t reserved0;               // alignment for backward-compatible

  uint32_t enc_cfg_daddr;
  uint32_t tile_pjpeg_dram_size;    // per-tile pjpeg dram size (0: default 16MB)

} cmd_dsp_enc_flow_max_cfg_t;


/// CMD_DSP_SET_PROFILE, command code = 0x0100000B
/* carry cmd_dsp_config_t cmd, processed by dsp_proc_restart_profile-->dsp_boot_config */

/// CMD_DSP_DEC_FLOW_MAX_CFG, command code 0x0100000C
#define DEC_MAX_CHANNELS      8u

typedef uint8_t dec_codec_sup_t;
#define DEC_CODEC_SUP_DEFAULT 0u
#define DEC_CODEC_SUP_AVC     1u
#define DEC_CODEC_SUP_HEVC    2u
#define DEC_CODEC_SUP_JPEG    4u
#define DEC_CODEC_SUP_ALL     7u

/// CMD_DSP_VOUT_FLOW_MAX_CFG, command code 0x0100000E
typedef struct
{
  uint32_t cmd_code;

  uint16_t vouta_max_pic_ncols;
  uint16_t vouta_max_osd_ncols;
  uint16_t voutb_max_pic_ncols;
  uint16_t voutb_max_osd_ncols;

  uint8_t vouta_max_pic_nrows;
  uint8_t vouta_max_osd_nrows;
  uint8_t voutb_max_pic_nrows;
  uint8_t voutb_max_osd_nrows;

  uint16_t vout_mixer_ncols;
  uint8_t  vout_mixer_nrows;
  uint8_t  rsvd1;

  uint8_t vouta_max_virt_chans_minus_1;  /* vouta max virtual chans - 1 */
  uint8_t voutb_max_virt_chans_minus_1;  /* voutb max virtual chans - 1 */

} cmd_dsp_vout_flow_max_cfg_t;

typedef struct
{
  uint16_t frm_width;
  uint16_t frm_height;
  uint32_t smvmax         :8;
  uint32_t gmvy           :8;   /* doesn't use currently */
  uint32_t core_used          :2;   /* 0 or 1: core#0 used(MDXF) */
                                    /* 2: core#1 used */
                                    /* 3: both cores used for dual-core-temporal (encode-pair pipeline) */
  uint32_t ref_num        :2;
  uint32_t coding_type        :1;   /* 0:h264 ;1:hevc */
  uint32_t dual_core_mode     :2;   /* 0: for dual-core-temporal */
                                    /* 1: for dual-core-stripe (for hevc only) */
  uint32_t reserved           :9;
} dec_cfg_t;

typedef struct
{
  uint32_t cmd_code;

  uint32_t coding_engine_usage :2;  /* 0/1 means using coding engine 0 */
                                    /* 2 means using codeing engine 1 */
                                    /* 3 means using both coding engine */
  uint32_t max_frm_num_of_dpb  :8;
  uint32_t reserved            :22;
  uint32_t max_bit_rate_h264;
  uint32_t max_bit_rate_hevc;
  uint32_t dec_cfg_daddr;           /* each content is dec_cfg_t structure */
  uint32_t dec_cfg_num;
  uint32_t max_resolution[DEC_MAX_CHANNELS];//for frame allocation,[15:0] width,[31:16] height
#ifndef PROJECT_CV5
 u64_addr_upper_t dec_cfg_daddr_upper;
#endif
} cmd_dsp_dec_flow_max_cfg_t;

/// CMD_DSP_UNITTEST0, command code 0x01000010
typedef struct
{
  uint32_t  cmd_code;

  uint8_t   test_mpart;
  uint8_t   test_appfile;
  uint8_t   test_orcmsg; /* 1: intra orc, 2: cross orcs */
  uint8_t   test_fbp;

  uint8_t   test_cbp;
  uint8_t   test_dbp;
  uint8_t   test_bdt;
  uint8_t   test_dma;

  uint8_t   test_mbuf;
  uint8_t   test_12bit;
  uint8_t   test_mfbp;
  uint8_t   test_stitch;

  uint8_t   test_assert_report;
  uint8_t   test_aes;
  uint8_t   test_orcasm;
  uint8_t   test_math;

} cmd_dsp_unittest0_t;

/// CMD_DSP_UNITTEST1(ieng), command code 0x01000011
typedef struct
{

  uint32_t cmd_code;                  // W0

  // W1
  uint32_t is_ce_en                     : 1;
  uint32_t is_main_en                   : 1;
  uint32_t is_warp_en                   : 1;
  uint32_t is_mctf_en                   : 1;
  uint32_t is_prev_a_en                 : 1;
  uint32_t is_prev_b_en                 : 1;
  uint32_t is_prev_c_en                 : 1;
  uint32_t is_prev_d_en                 : 1;
  uint32_t is_hier_en                   : 1;
  uint32_t is_osd_en                    : 1;
  uint32_t is_idsp_mixer_en             : 1;
  uint32_t is_hwarp_en                  : 1;
  uint32_t is_cfa_en                    : 1;
  uint32_t is_hdsp_en                   : 1;
  uint32_t rsvd1                        : 13;
  uint32_t osd_test_case                : 5;

  // W2
  uint32_t is_yuv_in                    : 1;
  uint32_t is_hdr_2exp                  : 1;
  uint32_t is_hdr_3exp                  : 1;
  uint32_t is_hier_octv                 : 1;
  uint32_t is_sbuf_mode                 : 1;
  uint32_t is_do_double_cmds            : 1;
  uint32_t is_double_smem_in            : 1;
  uint32_t is_keep_prev_config          : 1;
  uint32_t is_ik_flow                   : 1;
  uint32_t is_shpb_en                   : 1;
  uint32_t is_mctf_ref_decmpr_en        : 1;
  uint32_t is_mctf_ref_cmpr_en          : 1;
  uint32_t is_hier_y12_en               : 1;
  uint32_t is_ir_en                     : 1;
  uint32_t is_ieng_smem_in              : 1;
  uint32_t is_crop_shpb_by_init_phase   : 1;
  uint32_t is_warp_pm_en                : 1;
  uint32_t osd_in_enable                : 2;
  uint32_t rsvd2                        : 13;

  uint16_t poly_y_out_w;              // W3
  uint16_t poly_y_out_h;

  uint16_t prev_a_luma_out_w;         // W4
  uint16_t prev_a_luma_out_h;

  uint16_t prev_b_luma_out_w;         // W5
  uint16_t prev_b_luma_out_h;

  uint16_t prev_c_luma_out_w;         // W6
  uint16_t prev_c_luma_out_h;

  uint16_t osd_0_left;                // W7
  uint16_t osd_0_width;

  uint16_t osd_0_top;                 // W8
  uint16_t osd_0_height;

  uint16_t active_width;              // W9
  uint16_t active_height;

  uint16_t main_yuv_luma_out_w;       // W10
  uint16_t main_yuv_luma_out_h;

  uint16_t prev_d_luma_out_w;         // W11
  uint16_t prev_d_luma_out_h;

  uint16_t mask_in_w;                 // W12
  uint16_t mask_in_h;

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

/// CMD_DSP_VOUT_UNITTEST, command code 0x01000016
typedef struct
{
  uint32_t  cmd_code;
  uint32_t  vout_test_case_num;
} cmd_dsp_vout_unittest_t;

/// CMD_DSP_UNITTEST4(idspdrv), command code 0x01000017
typedef struct
{
  uint32_t cmd_code;                  // W0

  // W1
  uint32_t is_ce_en                     : 1;
  uint32_t is_main_en                   : 1;
  uint32_t is_warp_en                   : 1;
  uint32_t is_mctf_en                   : 1;
  uint32_t is_prev_a_en                 : 1;
  uint32_t is_prev_b_en                 : 1;
  uint32_t is_prev_c_en                 : 1;
  uint32_t is_prev_d_en                 : 1;
  uint32_t is_hier_en                   : 1;
  uint32_t is_osd_en                    : 1;
  uint32_t rsvd1                        : 22;

  // W2
  uint32_t is_yuv_in                    : 1;
  uint32_t is_hdr_2exp                  : 1;
  uint32_t is_hdr_3exp                  : 1;
  uint32_t is_hier_octv                 : 1;
  uint32_t is_sbuf_mode                 : 1;
  uint32_t is_do_double_cmds            : 1;
  uint32_t is_double_smem_in            : 1;
  uint32_t is_keep_prev_config          : 1;
  uint32_t is_ik_flow                   : 1;
  uint32_t rsvd2                        : 23;

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

  uint8_t  rsvd0;                  // W30
  uint8_t  deint_vin;
  uint8_t  bypass_c2y;
  uint8_t  test_keep_cfg;

  uint16_t prev_e_luma_w;             // W31
  uint16_t prev_e_luma_h;

} cmd_dsp_unittest4_t;

/* CMD_DSP_COVERAGE_TEST  0x01000018u*/
typedef struct
{
  uint32_t  cmd_code;

  uint32_t  ctest_module:16;
  uint32_t  ctest_case  :16;

} cmd_dsp_coverage_test_t;

/// CMD_DSP_VPROC_FLOW_MAX_CFG_EXT, command code 0x01000100
typedef struct
{
  uint32_t cmd_code;

  uint32_t cfa_aaa_fifo_num_per_ch :8;
  uint32_t rgb_aaa_fifo_num_per_ch :8;
  uint32_t vin_stats_fifo_num_per_ch :8;
  uint32_t idsp_mixer_as_prev_scaler : 1 ; /* 1 - use resampler in idsp mixer as a standalone preview scaler */
  uint32_t chan_max_cfg_num_minus_1 : 6;
  uint32_t rsvd0 :1;

  uint16_t mixer_prev_scaler_w_max ; /* max resamp out w when idsp_mixer_as_prev_scaler = 1 */
  uint16_t mixer_prev_scaler_h_max ; /* max resamp out h when idsp_mixer_as_prev_scaler = 1 */

  uint16_t scaler_mode_src_w_max;
  uint16_t scaler_mode_src_h_max;

  // postp common buf and max sizes
  uint32_t postp_prev_d_fb_num :8;
  uint32_t postp_prev_d_me01_fb_num :8;
  uint32_t postp_prev_hier_fb_num :8;
  uint32_t postp_prev_hier_me01_fb_num :8;

  uint32_t postp_prev_mixer_fb_num :8;
  uint32_t staging_buf_cfg_num : 4;
  uint32_t max_blur_regions_m1 :8;
  uint32_t copy_out_desc_fifo_num : 8;
  uint32_t rsvd24 :4;

  uint16_t postp_prev_d_w_max;
  uint16_t postp_prev_d_h_max;

  uint16_t postp_prev_mixer_w_max;
  uint16_t postp_prev_mixer_h_max;

  uint16_t postp_prev_hier_w_max;
  uint16_t postp_prev_hier_h_max;

  uint32_t chan_max_cfg_daddr; /* cmd_dsp_vproc_chan_max_cfg_t per channel */

  uint32_t staging_buf_cfg_daddr;
#ifndef PROJECT_CV5
  u64_addr_upper_t chan_max_cfg_daddr_upper;
  u64_addr_upper_t staging_buf_cfg_daddr_upper;
  u64_addr_upper_t copy_out_desc_fifo_base_upper;
#endif

  uint32_t copy_out_desc_fifo_base;
} cmd_dsp_vproc_flow_max_cfg_ext_t;
/******************************************************************************
 * CAT_VPROC(2)
 *****************************************************************************/
typedef struct
{
  uint32_t cmd_code;
  uint32_t channel_id : 6;
  uint32_t rsvd       : 26;
} cmd_vproc_hdr_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vproc_prev_mode_t;

#define DRAM_PREV_MODE     0u
#define Y2Y_SMEM_PREV_MODE 1u // Warp/MCTF->SMEM(SYNC_TYPE_TILE->LINE)->PREV->DRAM
#define VOUT_TH_PREV_MODE  2u // DRAM-->PREV->SMEM->VOUT, PREV sunning in VOUT thread

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vproc_input_format_t;

#define VPROC_INPUT_RAW_RGB 0u
#define VPROC_INPUT_YUV_422 1u
#define VPROC_INPUT_YUV_420 2u

///////////////////////////////////////////////////////////////////////////////

#define VPROC_LN_DEC_FB_DISABLE   0XFFu
#define VPROC_HIER_FB_NUM_DISABLE 0XFFu
#define VPROC_FB_NUM_DISABLE      0XFFu

#define VPROC_MAX_CHAN_NUM 20u
#define VPROC_MAX_CHAN_PER_GRP 16u /* up to 16 channels per vproc group */
#define VPROC_MAX_GRP_NUM   4u
#define VEFFECT_MAX_GRP_NUM 4u

///////////////////////////////////////////////////////////////////////////////
/// CMD_VPROC_CONFIG (0x02000001)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id             : 6;
  uint32_t input_format           : 3; /* refer to vproc_input_format_t */
  uint32_t prev_mode              : 2; /* refer to vproc_prev_mode_t */
  uint32_t grp_id                 : 3;
  uint32_t is_li_enabled          : 1;
  uint32_t is_warp_enabled        : 1;
  uint32_t is_y12_out_enabled     : 1; /* enable main_y12_out and hier_a_luma12[0-6]_out */
  uint32_t is_c2y_y12_out_enabled : 1; /* enable main_y12_out only */
  uint32_t is_osd_blend_enabled   : 1;
  uint32_t is_hwarp_enabled       : 1; /* sec11 */
  uint32_t is_mctf_cmpr_en        : 1;
  uint32_t rsvd0                  : 11;

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
  uint16_t prev_hier_w_max; /* Hierarchical resampler, sec17*/
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
  uint8_t c2y_int_y12_fb_num;

  uint16_t extra_sec2_vert_out_max; /* max extra section 2 vertical output for dzoom */
  uint8_t  prev_b_me_fb_num;
  uint8_t  prev_c_me_fb_num;

  uint32_t c2y_tile_num_x_max        : 4;
  uint32_t c2y_tile_num_y_max        : 4;
  uint32_t warp_tile_num_x_max       : 4;
  uint32_t warp_tile_num_y_max       : 4;
  uint32_t effect_copy_job_num_max   : 3; /* max number of effect copy jobs */
  uint32_t effect_blend_job_num_max  : 3; /* max number of effect blend jobs */
  uint32_t osd_blend_area_num_max_m1 : 5;
  uint32_t uv_nf_radius_max          : 2; /* 0 - 128 (bwd compatibility), 1 - 64, 2 - 32 */
  uint32_t ce_hor_ds_log2_min        : 2; /* Minimal CE horizontal downsample rate log2, started from CV6 */
  uint32_t rsvd2                     : 1;

  uint8_t  hier_y12_fb_num; /* interpret as hier_me_fb_num when hier_as_prev_scaler = 1 */
  uint8_t  prev_d_me_fb_num;
  uint16_t osd_blend_w_max;

  uint16_t warp_tile_overlap_x_max;
  uint16_t warp_tile_overlap_y_max;

  uint8_t  vouta_virtual_chan_id;
  uint8_t  voutb_virtual_chan_id;
  uint8_t  mixer_scaler_fb_num ;

  uint16_t mixer_scaler_w_max ;
  uint16_t mixer_scaler_h_max ;

  uint16_t scaler_mode_src_w_max;
  uint16_t scaler_mode_src_h_max;
} cmd_vproc_cfg_t;

///////////////////////////////////////////////////////////////////////////////
// vproc preview format

typedef uint8_t vproc_prev_format_t;

#define VPROC_PREV_FORMAT_PROGRESSIVE 0u
#define VPROC_PREV_FORMAT_INTERLACE   1u

#define IDSP0_UNIT_IDX 0u
#define IDSP1_UNIT_IDX 1u
#define IDSP_UNIT_NUM 2u
#define IDSP0_UNIT_MASK 1u
#define IDSP1_UNIT_MASK 2u
#define IDSP0_IDSP1_UNITS_MASK 3u

#define TILES_IDSP_UNIT0_ALL_IDX 0x00000000u      /* all tiles to unit0 */
#define TILES_IDSP_UNIT1_ALL_IDX 0xffffffffu      /* all tiles to unit1 */
#define TILES_IDSP_UNIT_EVEN0_ODD1_IDX 0xaaaaaaaau /* even tiles to unit0, odd tiles to unit1 */
#define TILES_IDSP_UNIT_EVEN1_ODD0_IDX 0x55555555u /* even tiles to unit1, odd tiles to unit0 */

///////////////////////////////////////////////////////////////////////////////

#define VIDEO_LISO  0u
#define STILL_LISO  1u
#define STILL_HISO  2u
#define SCALER_MODE 3u
#define FISHI_MODE  4u

///////////////////////////////////////////////////////////////////////////////

/* Hierarchical resampler input source */
#define HIER_SRC_MCTS     0u
#define HIER_SRC_PREV_A   1u
#define HIER_SRC_PREV_B   2u
#define HIER_SRC_PREV_C   3u
#define HIER_SRC_PREV_D   4u
#define HIER_SRC_DRAM     5u
#define HIER_SRC_RESERVED 6u
#define HIER_SRC_NOT_USED 7u

#define HIER_BIT_POS_PREV_A 0u
#define HIER_BIT_POS_PREV_B 1u
#define HIER_BIT_POS_PREV_C 2u
#define HIER_BIT_POS_PREV_D 3u

/// CMD_VPROC_SETUP (0x02000002)
typedef struct
{
  uint32_t cmd_code;

  // Word1
  uint32_t channel_id        : 6;
  uint32_t delayline_ena    : 1; // delayline after y2y, before OSD8 enable flag
  uint32_t rsvd0             : 25;

  // Word2
  uint32_t input_format      : 3; // refer to vproc_input_format_t
  uint32_t is_raw_compressed : 1;
  uint32_t is_tile_mode      : 1;
  uint32_t is_li_enabled     : 1;
  uint32_t is_warp_enabled   : 1;
  uint32_t is_mctf_enabled   : 1;

  uint32_t is_ce_enabled             : 1;
  uint32_t is_prev_ln_enabled        : 1; // prev_d
  uint32_t is_prev_a_enabled         : 1; // PIP
  uint32_t is_prev_b_enabled         : 1; // TV
  uint32_t is_prev_c_enabled         : 1; // LCD
  uint32_t is_prev_hier_enabled      : 1; // Hier
  uint32_t is_compressed_out_enabled : 1; // raw compression out
  uint32_t is_c2y_burst_tiling_out   : 1;

  uint32_t is_hier_burst_tiling_out  : 7;
  uint32_t prev_mode                 : 2;  // refer to vproc_input_format_t
  uint32_t is_y12_out_enabled        : 1; /* enable main_y12_out and hier_a_luma12[0-6]_out */
  uint32_t is_sbp_enabled            : 1; /* enable bad_pixel_in stream */
  uint32_t is_c2y_y12_out_enabled    : 1; /* enable main_y12_out only */
  uint32_t is_mcts_disabled          : 1; /* disable mcts */
  uint32_t proc_mode                 : 2; /* 0: VIDEO_LISO, 2: STILL_HISO 3: SCALER_MODE, 4: FISHI_MODE */
  uint32_t is_osd_blend_enabled      : 1; /* OSD blending */

  // Word3
  uint32_t is_aaa_enabled                : 1; // AAA cfg
  uint32_t is_c2y_dram_out_enabled       : 1; /* 1: luma_main_out and chroma_main_out to dram */
  uint32_t is_fast_y2y_en                : 1; /* 1: bypass idsp section2 and no dram copy from pic info */
  uint32_t is_hier_poly_sqrt2            : 1;
  uint32_t is_hdr_blend_dram_out_enabled : 1; // 1: sec2 external_raw_out to dram for debug
  uint32_t is_warp_dram_out_enabled      : 1; // 1: warp_luma_out and warp_chroma_out to dram
  uint32_t is_mcts_dram_out_enabled      : 1; // 1: mcts out to dram
  uint32_t keep_prev_cr_en               : 1;

  uint32_t ln_det_src                    : 8;  // hier filter idx serves as an input to the ln detect filter

  uint32_t aaa_cfa_mux_sel               : 2; /** 0: prescale
                                               *  1: badpixel
                                               *  2: vignette
                                               */
  uint32_t is_dzoom_enabled              : 1;
  uint32_t is_mctf_cmpr_en               : 1;
  uint32_t is_pic_info_cache_enabled     : 1;

  /**************************************************************************
   *  a = is_frame_alternate
   *  b = cap_seq_no & 1
   *  x = initial aaa_cfa_mux_sel
   *  y = output aaa_cfa_mux_sel
   *
   *  y = 0: IDSP_CMD_2_AAA_CFA_MUX_SELECT_PRESCALE
   *  y = 1: IDSP_CMD_2_AAA_CFA_MUX_SELECT_BADPIXEL
   *  y = 2: IDSP_CMD_2_AAA_CFA_MUX_SELECT_VIGNETTE
   *
   *  y = 1 if a == 1, b == 1, x == 2
   *  y = 2 if a == 1, b == 1, x != 2
   *  y = x otherwise
   *************************************************************************/
  uint32_t is_frame_alternate            : 1;
  uint32_t is_hwarp_enabled              : 1;
  uint32_t is_prev_d_enabled             : 1;
  uint32_t prev_d_src       :2; // 0: dram, 1: smem, 2: vout thread preview,
                                // 0xf: No_PREVIEW(turning off idsp preveiw filter)
  uint32_t prev_d_dst       :4; // refer to enum VPROC_PREV_DST_MASK
                                // preveiw destination:
                                // 0: default arrangement, preview a -> PIN,
                                //                         preview b -> TV,
                                //                         preview c -> LCD
                                // 1: VOUT0
                                // 2: VOUT1
                                // 3:
                                // 4: PIN
  uint32_t prev_d_ch_fmt    :2; // yuv_chroma_fmt_t

  // Word4
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

  uint32_t prev_b_format    :2; // refer to enum vproc_prev_format_t
                                // 0:progressive, 1:interlace
  uint32_t prev_b_src       :2; // 0: dram, 1: smem, 2: vout thread preview,
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

  // Word5
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
  uint32_t mctf_mode                :2; /* 0: filtering, 2: blending */

  uint32_t is_ldly_enabled          :1; /* 1: horizontal slice low delay pipeline */
  uint32_t is_c2y_dram_sync_to_warp :1;
  uint32_t num_of_exp               :2; /* number of exposure: 1, 2, 3 */
  uint32_t is_c2y_smem_sync_to_warp :1;
  uint32_t calc_stitch_data         :1;  /* 1: ucode calculates idsp_stitch_data */
  uint32_t is_c2y_me_enabled        :1;
  uint32_t is_md_enabled            :1; /* 1: LISO video with motion detection (Option-C) */

  // Word6
  uint32_t c2y_tile_num_x  : 4;
  uint32_t c2y_tile_num_y  : 4;
  uint32_t warp_tile_num_x : 4;
  uint32_t warp_tile_num_y : 4;
  uint32_t ext_mem_cfg_num : 5;
  /****************************************************************************
   * bit mask of using which idsp unit to process frame data:
   * 00: idsp0(default)
   * 01: idsp0
   * 10: idsp1
   * 11: idsp0 and 1 in round-robin mode
   ***************************************************************************/
  uint32_t idsp_unit_mask : 2;

  /****************************************************************************
   * Hierarchical resampler input source:
   * 0: mcts output smem
   * 1: preview A output smem
   * 2: preview B output smem
   * 3: preview C output smem
   * 4: preview D output smem
   * 5: dram
   * 0xff: not used
   ***************************************************************************/
  uint32_t hier_src                 : 3;
  uint32_t prev_a_presend_to_vout   : 1;
  uint32_t prev_b_presend_to_vout   : 1;
  uint32_t prev_c_presend_to_vout   : 1;
  uint32_t is_scaler_mode_enabled : 1; /* scaler_mode using mixer scaler */
  uint32_t proc_hds_as_main         : 1;
  uint32_t is_tile_dcrop_disabled   : 1; /* disable dma crop for tile mode */

  uint32_t pts_delta;

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

  uint16_t raw_tile_align_x; /* default value 128 */
  uint16_t raw_tile_overlap_x;

  uint32_t ext_mem_cfg_addr;

  uint16_t privacy_mask_width;
  uint16_t privacy_mask_height;

  uint32_t ext_buf_mask;

  uint32_t is_ir_out_enabled           : 1;
  uint32_t is_prev_a_crop_for_mcts     : 1; /* 1: store mcts with prev a crop out. is_mcts_dram_out_enabled must be 0 */
  uint32_t is_mctf_dram_out_dis        : 1;
  uint32_t is_vwarp_disabled           : 1;
  uint32_t is_y2y_rotate_flip_en       : 1; /* 0: v-flip and h-flip at the input of C2Y and Y2Y, respectively. No rotation */
                                            /* 1: rotation & flip at the input of Y2Y */
                                            /* Rot/flip flags from CMD_VIN_SET_FRM_LVL_FLIP_ROT_CONTROL/VIN_SEND_INPUT_DATA */
  uint32_t is_hier_poly_arb            : 1; /* 1: arbitrary scaling enabled for hier poly resamp */
  uint32_t prev_sync_to_enc            : 2; /* sync to encoder core#0 */
  uint32_t calc_stitch_overlap         : 1; /* 1: ucode decides raw_tile_align_x, raw_tile_overlap_x, hier_poly_tile_overlap_x */
  uint32_t slice_ext_data              : 1; /* 1: report ext data at slice boundary */
  uint32_t idsp_mixer_as_prev_scaler   : 1; /* 1 - use resampler in idsp mixer as a standalone preview scaler */
  uint32_t hier_as_prev_scaler         : 1; /* 1 - use hi-resampler as a standalone preview scaler */
  uint32_t mixer_scaler_ch_fmt         : 2;
  uint32_t do_double_cmds              : 1;
  uint32_t tile_idsp_unit_order        : 1; /* 0: even tiles->idsp0 odd tiles->idsp1, 1: even tiles->idsp1 odd tiles->idsp0 */
  uint32_t is_mcts_burst_tiling_out    : 1;
  uint32_t sync_c2y_y2y_jobs           : 1; /* 1: run c2y and y2y jobs on HW at the same time */
  uint32_t is_main_dst_vproc           : 1; // when main dst = VPROC_PREV_DST_PIN, the pin is binded to 0: enc, 1: vproc
  uint32_t proc_mode_msb               : 2;
  uint32_t prev_sync_to_enc_core1      : 2; /* sync to encoder core#1 */
  uint32_t ir_size                     : 1; /* 0: full size; 1: quarter size */
  uint32_t ext_mem_imginf_num          : 4;
  uint32_t rsvd2                       : 4;

  uint32_t frm_delay_time_ticks;

  uint16_t mixer_scaler_w ; /* resampler size in idsp mixer */
  uint16_t mixer_scaler_h ;

#ifndef PROJECT_CV5
 u64_addr_upper_t ext_mem_cfg_addr_upper;
#endif
} cmd_vproc_setup_t;

/// CMD_VPROC_IK_CONFIG (0x02000003)
typedef struct
{
  uint32_t cmd_code;

  // Word1
  uint32_t channel_id      : 6;
  uint32_t use_flow_warp_cr: 1; /* use CA/Geowarp_cr in idsp_flow data until receiving warp cmd */
  uint32_t rsvd0           : 25;

  // Word2
  uint32_t ik_cfg_id; /* ik_cfg_id_t*/

  // Word3
  uint32_t idsp_flow_addr; /* dram address of this idsp_flow_ctrl_t instance */

#ifndef PROJECT_CV5
 u64_addr_upper_t idsp_flow_addr_upper;
#endif
} cmd_vproc_ik_config_t;

/// CMD_VPROC_IMG_PRMD_SETUP  (0x02000004)
typedef struct
{
  int16_t  roi_start_row;
  int16_t  roi_start_col;
  uint16_t roi_height;
  uint16_t roi_width;
} scale_info_t;

#define SCALE_INFO_NUM_MAX 7u

typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id               : 6;
  uint32_t enable_bit_mask          : 7; // bit positon indicates which scaler is on, i.e. bit 0 -> scale_out_0, bit 1->scale_out_1, etc
  uint32_t is_hier_poly_sqrt2       : 1;

  // is 1 then, the roi cmd is first entered into a holding area, or grouped with all other roi cmds,
  // and then issued to the master fov's cmd q [indicated by grp_fov_cmd_id]. this will make sure
  // all roi commands with a stereo pair would be executed at the same time.
  // is 0, then the roi cmd is dispatched directly to fov cmd q indicated by channel_id
  uint32_t is_grp_cmd               : 1;
  uint32_t grp_fov_cmd_id           : 4;
  uint32_t is_hier_burst_tiling_out : 7;
  uint32_t is_hier_y12              : 1;
  uint32_t rsvd0                    : 5;

  scale_info_t scale_info[SCALE_INFO_NUM_MAX];
  uint32_t roi_tag;
  uint32_t deci_rate;

  uint32_t hier_poly_w :16;
  uint32_t hier_poly_h :16;
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

#define VPROC_PREV_A       0u
#define VPROC_PREV_B       1u
#define VPROC_PREV_C       2u
#define VPROC_PREV_D       3u
#define VPROC_PREV_MIXER   4u
#define VPROC_PREV_HIER    5u
#define VPROC_PREV_SEC_NUM 6u


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
#define VPROC_PREV_DST_VOUTS  (VPROC_PREV_DST_VOUT0 | VPROC_PREV_DST_VOUT1)
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
  uint32_t rsvd0          : 4;

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
#define EXT_MEM_TYPE_PREV_D       4u
#define EXT_MEM_TYPE_LN_DEC       EXT_MEM_TYPE_PREV_D
#define EXT_MEM_TYPE_HIER_0       5u
#define EXT_MEM_TYPE_COMP_RAW     6u
#define EXT_MEM_TYPE_MAIN_ME      7u
#define EXT_MEM_TYPE_PIP_ME       8u
#define EXT_MEM_TYPE_HIER_Y12     9u
#define EXT_MEM_TYPE_C2Y_Y12      10u
#define EXT_MEM_TYPE_HIER_ME      EXT_MEM_TYPE_HIER_Y12
#define EXT_MEM_TYPE_PREV_D_ME    11u
#define EXT_MEM_TYPE_PREV_B_ME    12u
#define EXT_MEM_TYPE_PREV_C_ME    13u
#define EXT_MEM_TYPE_IR           14u
#define EXT_MEM_TYPE_MIXER_SCALER 15u
#define EXT_MEM_TYPE_NUM          16u

///////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint32_t    cmd_code;

  uint32_t    vin_id_or_chan_id   : 8;  // CMD_VIN_SET_EXT_MEM:   VIN ID
                                        // CMD_VPROC_SET_EXT_MEM: VPROC channel ID
  uint32_t    allocation_mode     : 2;  // 0: new, 1: append
  uint32_t    overflow_cntl       : 2;  // 0: rotate, 1: stall, 2: disable dram
  uint32_t    chroma_format       : 2;  // yuv_chroma_fmt_t,
                                        // YUV_422/YUV_420 for yuv memory_type,
                                        // YUV_MONO otherwase.
  uint32_t    reserved            : 18;

  uint8_t     memory_type;     // CMD_VIN_SET_EXT_MEM: see vin_ext_mem_type_t.
                               // 0 for raw buf; 1: YUV; 2: HDS 3: ME 4: Aux 5: Prev
                               // CMD_VPROC_SET_EXT_MEM: see ext_mem_type_t.
  uint8_t     allocation_type; // 0: distinct addr, 1: start addr
  uint16_t    num_frm_buf;     // number of raw frame buffers

  uint16_t    buf_pitch;             // buffer pitch
  uint16_t    buf_width;             // buffer width
  uint16_t    buf_height;            // buffer height
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

  uint32_t    buf_addr;  // allocation_type=1: memory segment starting address
                         // allocation_type=0: memory with distinct FB addresses

#ifndef PROJECT_CV5
  u64_addr_upper_t buf_addr_upper;
#endif
} cmd_set_ext_mem_t;

/// CMD_VPROC_SET_EXT_MEM (0x02000007)
typedef cmd_set_ext_mem_t cmd_vproc_set_ext_mem_t;

// CMD_VPROC_GRP_CMD - 0x02000008
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id       : 8;  // channel id
  uint32_t is_sent_from_sys : 4;
  uint32_t is_vout_cmds     : 4;  // the entire group cmd contains only vout cmds.
  uint32_t vout_id          : 2;
  uint32_t is_encode_cmds   : 1;  // the entire group cmd contains only encoder cmds.
  uint32_t encode_id        : 5;
  uint32_t reserved_1       : 8;

  uint32_t grp_cmd_buf_id;
  uint32_t grp_cmd_buf_addr;
  uint32_t num_cmds_in_buf;
  uint32_t enc_grp_cmd_buf_id;
  uint32_t enc_grp_cmd_buf_addr;
  uint32_t enc_num_cmds_in_buf;

#ifndef PROJECT_CV5
  u64_addr_upper_t grp_cmd_buf_addr_upper;
  u64_addr_upper_t enc_grp_cmd_buf_addr_upper;
#endif
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

#ifndef PROJECT_CV5
  u64_addr_upper_t osd_addr_y_upper;
  u64_addr_upper_t osd_addr_uv_upper;
  u64_addr_upper_t alpha_addr_y_upper;
  u64_addr_upper_t alpha_addr_uv_upper;
#endif
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


/// CMD_VPROC_WARP_GROUP_UPDATE (0x0200000Du)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id        : 6;
  uint32_t rsvd0             : 26;

  uint32_t group_update_info_daddr; /* idsp_group_update_info_t defined in idspdrv_imgknl_if.h */

#ifndef PROJECT_CV5
  u64_addr_upper_t group_update_info_daddr_upper;
#endif
} cmd_vproc_warp_group_update_t;

typedef struct
{
  int16_t x ; /* must be even for chroma pairing (& 0xFFFE) */
  int16_t y ; /* must be a multiple of 4 to ensure even height for 420 chroma (& 0xFFFC) */
  uint16_t w ; /* must be a multiple of 4 for SMEM alignment */
  uint16_t h ; /* must be a multiple of 4 to ensure even height for 420 chroma (& 0xFFFC) */
} osd_insert_rect_t;

typedef struct
{
  uint32_t osd_clut_daddr ;   /* DRAM address of CLUT */
  uint32_t osd_daddr ;
  osd_insert_rect_t osd_size ;
  uint16_t osd_dpitch ;

#ifndef PROJECT_CV5
  u64_addr_upper_t osd_clut_daddr_upper;
  u64_addr_upper_t osd_daddr_upper;
#endif
} osd_insert_buf_t;

/// CMD_VPROC_OSD_INSERT  (0x0200000Eu)
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
  uint32_t force_update_flag :1 ;
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
  uint32_t osd_region_batch_daddr ;

#ifndef PROJECT_CV5
  u64_addr_upper_t osd_region_batch_daddr_upper;
#endif
} cmd_vproc_osd_insert_t;

/// CMD_VPROC_SET_PRIVACY_MASK (0x0200000Fu)
typedef struct
{
  uint32_t cmd_code;
  uint32_t channel_id;
  uint32_t enabled_flags_dram_addr;
  uint32_t enabled_flags_dram_pitch;

  uint32_t Y          :8;
  uint32_t U          :8;
  uint32_t V          :8;
  uint32_t reserved   :8;

  uint16_t raw_width;
  uint16_t raw_height;

#ifndef PROJECT_CV5
  u64_addr_upper_t enabled_flags_dram_addr_upper;
#endif
} cmd_vproc_set_privacy_mask_t;


/// CMD_VPROC_SET_STREAMS_DELAY(0x02000010u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id: 6;
  uint32_t rsvd0     : 26;

  uint32_t delay_in_ms[VPROC_STREAM_NUMS];

} cmd_vproc_set_streams_delay_t;

typedef struct
{
  uint16_t horz_start;
  uint16_t vert_start;
  uint16_t width;
  uint16_t height;
} rect_info_t;

typedef struct
{
  uint32_t zx ; /* 16.16 */
  uint32_t zy ; /* 16.16 */
  rect_info_t in;
  rect_info_t out;
  uint32_t rsvd [1] ;
} vproc_fishi_resamp_t ;

typedef struct
{
  uint32_t id : 5;
  uint32_t rotate : 1;
  uint32_t hflip : 1;
  uint32_t vflip : 1;
  uint32_t is_vh : 1; /* HV - 0 , VH - 1*/
  uint32_t reserved : 23;

  rect_info_t intermediate; /* for HV only */
  vproc_fishi_resamp_t main_resamp;

  uint32_t calib_data[20]; /* idsp_calib_data_t - 80 bytes */
  uint32_t reserved1[2]; /* align to 128 bytes */
} vproc_fishi_region_ext_t;

typedef struct
{
  uint32_t id : 5;
  uint32_t prev_a_updated : 1;
  uint32_t prev_b_updated : 1;
  uint32_t prev_c_updated : 1;
  uint32_t prev_d_updated : 1;
  uint32_t rsvd0 : 23;

  vproc_fishi_resamp_t prev_a;
  vproc_fishi_resamp_t prev_b;
  vproc_fishi_resamp_t prev_c;
  vproc_fishi_resamp_t prev_d;

  uint32_t rsvd1[3]; /* align to 128 bytes */
} vproc_fishi_region_prev_t;

/// CMD_VPROC_FISHI_WARP_REGION_SETUP(0x02000011u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id: 6;
  uint32_t region_num_m1 : 5; /* 5-bits to make it future proof */
  uint32_t element_size_in_4bytes : 8; /* set to ROUND_UP4(sizeof(vproc_fishi_region_t)) >> 2 */
  uint32_t rsvd1 : 13;

  uint32_t daddr; /* packed with vproc_fishi_region_t */
#ifndef PROJECT_CV5
  u64_addr_upper_t daddr_upper;
#endif

  uint32_t order_mask : 16; /* each bit pos corresponds to region_id. Bit is set to 0/1 for hv/vh. */
  uint32_t update_mask : 16; /* each bit pos corresponds to region_id. Bit is set to 1 if the region is updated. */
  uint32_t disable_mask : 16; /* each bit pos corresponds to region_id. Bit is set to 1 if the region is disabled/skipped */
  uint32_t rsvd2 : 16 ;
} cmd_vproc_fishi_warp_region_setup_t;

/// CMD_VPROC_FISHI_WARP_PREV_SETUP(0x02000012u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id : 6;
  uint32_t region_num_m1 : 5; /* 5-bits to make it future proof */
  uint32_t element_size_in_4bytes : 8; /* set to ROUND_UP4(sizeof(vproc_fishi_region_prev_t)) >> 2 */
  uint32_t rsvd0 : 13;

  uint32_t daddr; /* packed with vproc_fishi_region_prev_t */
#ifndef PROJECT_CV5
  u64_addr_upper_t daddr_upper;
#endif

  uint32_t update_mask : 16; /* each bit pos corresponds to region_id. Bit is set to 1 if the region is updated. */
  uint32_t rsvd1 : 16;
} cmd_vproc_fishi_warp_prev_setup_t;

typedef struct
{
  uint16_t x_offset;
  uint16_t y_offset;

  uint16_t width;
  uint16_t height;

  uint32_t alpha_luma_dram_pitch :16;
  uint32_t blur_src :3;
  uint32_t blur_strength :2;
  uint32_t rsvd1 :11;

  uint32_t alpha_luma_dram_addr;
#ifndef PROJECT_CV5
  u64_addr_upper_t alpha_luma_dram_addr_upper;
#endif
} vproc_blur_pp_cfg_t;

/* CMD_VPROC_BLUR_PM_PP 0x02000013u */
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id :6;
  uint32_t str_id :8;
  uint32_t is_blur_pm_enabled :1;
  uint32_t num_of_blur_ops_m1 :6;
  /* 0: only update blur regions (assume same FOV and blur src)
   * 1: update blur regions and copy cfgs (required if FOV or blur src changed)
   */
  uint32_t update_src_img :1;
  uint32_t rsvd :10;

  /* dram base address of vproc_blur_pp_cfg_t block */
  uint32_t  blur_cfg_address;
#ifndef PROJECT_CV5
  u64_addr_upper_t blur_cfg_address_upper;
#endif
} cmd_vproc_blur_pm_pp_t;

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

  uint32_t  is_embed_marker            : 1;
  uint32_t  rsvd0                      : 21;
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
  uint32_t  is_rotate_1st_inp : 4; /* 0:no rotation, 1:90, 2:270, 3:180, 4: hfllp, 5: vflip, 6: rotate+hflip, 7: rotate+vflip */
  uint32_t  is_rotate_2nd_inp : 4; /* 0:no rotation, 1:90, 2:270, 3:180, 4: hfllp, 5: vflip, 6: rotate+hflip, 7: rotate+vflip */
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

  uint32_t  chroma_format     : 2; // 0: 420, 1: 422
  uint32_t  scaling_input_sel : 4; // 0: none, 1: first, 2: second, 3: alpha
  uint32_t  alpha_format      : 2; // 0: 8bit, 1: 1bit
  uint32_t  rsvd1             : 24;

  uint16_t  scaling_input_width;  // scaling input width
  uint16_t  scaling_input_height; // scaling input height

#ifndef PROJECT_CV5
  u64_addr_upper_t alpha_luma_dram_addr_upper;
  u64_addr_upper_t alpha_chroma_dram_addr_upper;
#endif
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

#ifndef PROJECT_CV5
  u64_addr_upper_t post_r2y_copy_cfg_address_upper;
  u64_addr_upper_t y2y_blending_cfg_address_upper;
#endif
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
  uint32_t hl_job_scheduler: 2; /* 0: auto; 1:enable; 2:disable */
  uint32_t final_output_presend : 1;
  /* 0: default - veffect use VPROC_SETUP.idsp_unit_mask
   * 1: idsp0
   * 2: idsp1
   */
  uint32_t idsp_unit_mask : 2;
  uint32_t rsvd         : 15;

  /* dram base address of num_of_inputs vproc_pp_stream_cntl_t block */
  uint32_t input_pp_cfg_addr_array;

#ifndef PROJECT_CV5
  u64_addr_upper_t input_pp_cfg_addr_array_upper;
#endif
} cmd_vproc_multi_stream_pp_t;

/// CMD_VPROC_SET_EFFECT_BUF_IMG_SZ (0x02000065u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t chan_id       : 6;
  uint32_t effect_grp_id : 2;
  uint32_t str_id        : 8;
  uint32_t output_dst    : 8;
  uint32_t rsvd1         : 8;

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
  uint32_t grp_id              : 3;
  uint32_t free_run_grp        : 1;
  uint32_t is_runtime_change   : 1; /* using the VPROC common commands queue */
  uint32_t rsvd1               : 7;

  uint32_t cap_seq_no;     // cap_seq_no from where onward the proc_order is followed

  uint32_t num_of_vproc_channel;
#define VPROC_CH_DISABLE_FLAG_MASK 0x80u /* MSB: vproc channel processing disable flag */
#define VPROC_CH_ID_MASK           0x3Fu /* 6 bits channel_id */
  uint8_t  proc_order[VPROC_MAX_CHAN_PER_GRP]; /* U8: disable flag(MSB) + chan_id */
  uint32_t proc_order_addr;
#ifndef PROJECT_CV5
  u64_addr_upper_t proc_order_addr_upper;
#endif
} cmd_vproc_multi_chan_proc_order_t;

/// CMD_VPROC_SET_VPROC_GRPING              0x02000067u
typedef struct
{
  uint32_t cmd_code;

  uint32_t num_of_vproc_groups  : 6; /* up to MAX_VPROC_GROUPS */
  uint32_t rsvd0                : 2;
  uint32_t max_vprocs_per_group : 8; /* indicate array size per group */
  uint32_t free_run_grp         : 16; /* bitmask per group (up to MAX_VPROC_GROUPS) */

  uint32_t num_vprocs_per_group_addr;
  uint32_t vprocs_in_a_group_addr;

#ifndef PROJECT_CV5
  u64_addr_upper_t num_vprocs_per_group_addr_upper;
  u64_addr_upper_t vprocs_in_a_group_addr_upper;
#endif
} cmd_vproc_set_vproc_grping;

#define VPROC_MAX_TESTFRAME_CASES 4u
#define VPROC_Y2Y_TESTFRAME_MAX_INPUT_NUM 32u
#define VPROC_Y2Y_TESTFRAME_MAX_OUTPUT_NUM 26u

#define VPROC_C2Y_TESTFRAME_TYPE 0u
#define VPROC_Y2Y_TESTFRAME_TYPE 1u

/// CMD_VPROC_SET_TESTFRAME              0x02000068u
typedef struct
{
  uint32_t cmd_code;

  uint32_t type            : 2; /* VPROC_C2Y/Y2Y_TESTFRAME_TYPE */
  uint32_t num_of_cases    : 5; /* number of tesframe cases */
  uint32_t is_sending_jobs : 1; /* 1 for sending jobs, 0 for resource allocation */
  uint32_t rsvd            : 24;

  /* dram address of testframe_cfg data, MUST BE 128 aligned */
  uint32_t tf_cfg_daddr[VPROC_MAX_TESTFRAME_CASES];

  /* the order of issueing test cases */
  uint8_t  tc_order[VPROC_MAX_TESTFRAME_CASES];

  /* dram address of testframe input table */
  uint32_t tf_in_tbl_addr;
  uint32_t tf_out_tbl_addr;

#ifndef PROJECT_CV5
  //FIXME--flpan_CV6_AST TBD
  u64_addr_upper_t tf_cfg_daddr_upper[VPROC_MAX_TESTFRAME_CASES];
  u64_addr_upper_t tf_in_tbl_addr_upper;
  u64_addr_upper_t tf_out_tbl_addr_upper;
#endif
} cmd_vproc_set_testframe_t;


/******************************************************************************
 * CAT_VIN(4)
 *****************************************************************************/
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

#define MAX_VIN_CAP_SLICE_NUM     8u

typedef struct
{
  uint16_t chan_id;
  uint16_t x_offset;
  uint16_t y_offset;
  uint16_t width;
  uint16_t height;
#ifndef PROJECT_CV5  // let the system do the calculation
  // 3 idsp outputs, raw above and hds/ce below
  uint16_t hds_x_offset;
  uint16_t hds_y_offset;
  uint16_t hds_width;
  uint16_t hds_height;
  uint16_t ce_x_offset;
  uint16_t ce_y_offset;
  uint16_t ce_width;
  uint16_t ce_height;
#endif
  uint8_t  xpitch;              // pitch multipiler. set 1 for normal fov. set 2 for two interlaced fov.
  uint8_t  rsvd0;

  uint32_t hdr_intlac_mode      : 1; // 0: [(EXP,FOV)] = [(0,0);(0,1);(1,0);(1,1)]. 1: [(0,0);(1,0);(0,1);(1,1)]
  uint32_t is_fov_active        : 1;
  uint32_t is_last_fov          : 1;
#ifndef PROJECT_CV5
  uint32_t fov_raw_strm         :3; // indicate the raw fov strm to vproc: 0:idsp rwo out 1:idsp hds out
  uint32_t fov_ce_strm          :3; // indicate the ce fov strm to vproc:  1:idsp hds out 2: idsp ce out
  uint32_t hdec_factor_minus_1  :3; // between raw_strm and ce_strm defined above
  uint32_t non_low_delay_fov    :1; // no presend, and send by frame even for sliced fov
  uint32_t rsvd1                :19;
#else
  uint32_t is_prev_fov          : 1; // fov is for vin prev out or vin raw out
  uint32_t non_low_delay_fov    : 1; // no presend, and send by frame even for sliced fov
  uint32_t rsvd1                : 27;
#endif

  batch_cmd_set_info_t  batch_cmd_set_info;
  uint16_t slice_term_cap_line[MAX_VIN_CAP_SLICE_NUM];

  uint32_t rsvd2[7]; // rsv some extra space
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
  uint32_t rpt_frm_cntrl  :4;           // see defines above for valid values
  uint32_t rsvd           :28;
} sys2dsp_vin_cntrl;

typedef struct
{
  // W0
  uint32_t cmd_code;

  // W1
  uint32_t vin_id                   : 8;
  uint32_t cmd_msg_decimation_rate  : 8;
  uint32_t is_compression_en        : 1;  //used by CV5 only, deprecated for CV6
  uint32_t send_input_data_type     : 2;  // 0: raw, 1: yuv422, 2:yuv420
  uint32_t is_reset_frm_sync        : 1;
  uint32_t is_auto_frm_drop_en      : 1; /* no halt if vin fb is run out */
  uint32_t is_check_timestamp       : 1;
  uint32_t cap_slice_num            : 4;
  uint32_t is_vin_cap_presend       : 1;
  uint32_t no_reset_fp_shared_res   : 1;  /* on/off the resetting of resources (fbp) */
                                          /* that are shared with other FPs */
                                          /* =1, for example, when is_pic_info_cache_enabled=1 */
  uint32_t vin_ce_out_reset         : 1;  /* =1 will turn off ce out */
  uint32_t vin_aux_out_en           : 1;
#ifndef PROJECT_CV5
  uint32_t vin_hds_out_en           : 1;
  uint32_t share_hds_from_other_vin : 1; /* if set, need ext mem to be sync'ed up
                                          with the vin actually capturing the shared hds data */
#else
  uint32_t vin_prev_out_en          : 1;
  uint32_t vin_prev_share_ce_out    : 1;
#endif

  // W2
  uint8_t  fov_num;
  uint8_t  skip_frm_cnt;
  uint8_t  output_dest;
  uint8_t  input_source;                // 0: sensor in, 1: ext data (raw/yuv422/yuv420) in, 2: vin yuv422 input

  // W3
  uint16_t vin_cap_width;
  uint16_t vin_cap_height;

  // W4
  uint8_t  blk_sz;    //used by CV5 only, deprecated for CV6
  uint8_t  mantissa;  //used by CV5 only, deprecated for CV6
  uint16_t flush_raw_status_to_arm : 1;
  uint16_t rsvd1 : 15;

  // W5
  uint32_t fov_cfg_tbl_daddr;           // for input parameters

  // W6, W7, W8
  batch_cmd_set_info_t batch_cmd_set_info;

  // W9
  uint32_t vin_poll_intvl;

  // W10
  uint32_t capture_time_out_msec;      // 0: no time out

  // W11
  uint16_t vin_aux_cap_width;
  uint16_t vin_aux_cap_height;

  // W12
#ifndef PROJECT_CV5
  uint16_t vin_hds_cap_width;
  uint16_t vin_hds_cap_height;
#else
  uint16_t vin_prev_cap_width;
  uint16_t vin_prev_cap_height;
#endif

  // W13
#ifdef PROJECT_CV6
  uint32_t  vin_hds_ce_cap_bot_height :8; // For CV6, hds failed to output some lines at the bottom
  uint32_t  rsvd2                     :24;
#else
  uint32_t  rsvd2;
#endif

  // W14
  uint32_t default_raw_image_address;   // default luma image for yuv422 case

  // W15
  uint32_t default_ce_image_address;    // default chroma image for yuv422 case

  // W16
  uint16_t default_raw_image_pitch;   // default luma image pitch for yuv422 case
  uint16_t default_ce_image_pitch;    // default chroma image pitch for yuv422 case

  // W17
  uint32_t capture_compl_cntl_msg_addr;

  // W18
  uint32_t compl_cntl_msg_update_time_msec;

#ifndef PROJECT_CV5
  // W19
  uint32_t default_hds_image_address;

  // W20
  uint16_t default_hds_image_pitch;
  uint16_t rsvd3;

  // W21
  u64_addr_upper_t fov_cfg_tbl_daddr_upper;

  // W22
  u64_addr_upper_t default_raw_image_address_upper;

  // W23
  u64_addr_upper_t default_ce_image_address_upper;

  // W24
  u64_addr_upper_t capture_compl_cntl_msg_addr_upper;

  // W25
  u64_addr_upper_t default_hds_image_address_upper;
#endif

  // W26
  uint32_t short_frame_drop_thresh_ticks; // in the units of the audio clock ticks

  // W27
  uint32_t boot_time_out_msec;      // 0: equal to capture_time_out_msec

} cmd_vin_start_t;

/// CMD_VIN_IDLE (0x04000002)
typedef struct
{
  uint32_t  cmd_code;

  uint32_t  vin_id  : 8;
  uint32_t  rsvd0   : 24;
} cmd_vin_idle_t;

#if 0
/// CMD_VIN_SET_RAW_FRM_CAP_CNT (0x04000003)
typedef struct
{
  uint32_t  cmd_code;

  uint32_t  vin_id  : 8;
  uint32_t  rsvd0   : 24;

  uint32_t  cap_seq_no;
} cmd_vin_set_raw_frm_cap_cnt_t;
#endif

/// CMD_VIN_SET_EXT_MEM (0x04000004)
typedef cmd_set_ext_mem_t cmd_vin_set_ext_mem_t;

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

#ifndef PROJECT_CV5
  uint32_t input_data_type        : 8;   // 0: raw, 1: yuv422
  uint32_t rsvd1                  : 24;
#else
  uint32_t raw_compression_type   : 8;
  uint32_t input_data_type        : 8;   // 0: raw, 1: yuv422
  uint32_t rsvd1                  : 16;
#endif

  uint32_t stop_condition;              // frames to capture, 0xffffffff means no limit

  uint8_t  raw_cap_done_sw_ctrl;        // 0: to IDLE, 1: to internal buffer
  uint8_t  raw_buffer_proc_ctrl;        // 0: no raw2yuv, 1: pass to raw2yuv
  uint8_t  raw_cap_sync_event;
  uint8_t  rsvd2;
} cmd_vin_initiate_raw_cap_to_ext_buf_t;

///////////////////////////////////////////////////////////////////////////////
// Event mask bits
#define BIT_POS_EXT_MAIN         0u
#define BIT_POS_EXT_PREV_A       1u
#define BIT_POS_EXT_PREV_B       2u
#define BIT_POS_EXT_PREV_C       3u
#define BIT_POS_EXT_LN_DEC       4u
#define BIT_POS_EXT_HIER_0       5u
#define BIT_POS_EXT_COMP_RAW     6u
#define BIT_POS_EXT_MAIN_ME      7u
#define BIT_POS_EXT_PREV_A_ME    8u
#define BIT_POS_EXT_PREV_B_ME    9u
#define BIT_POS_EXT_PREV_C_ME    10u
#define BIT_POS_EXT_MCTS         11u
#define BIT_POS_EXT_MCTF         12u
#define BIT_POS_EXT_HIER_Y12     13u
#define BIT_POS_EXT_HIER_ME      BIT_POS_EXT_HIER_Y12
#define BIT_POS_EXT_C2Y_Y12      14u
#define BIT_POS_EXT_PREV_D_ME    15u
#define BIT_POS_EXT_IR           20u
#define BIT_POS_EXT_MIXER_SCALER 21u

#define ext_hier_me             ext_hier_y12

typedef struct
{
  uint32_t ext_main           : 1;
  uint32_t ext_prev_a         : 1;
  uint32_t ext_prev_b         : 1;
  uint32_t ext_prev_c         : 1;

  uint32_t ext_ln_dec         : 1;
  uint32_t ext_hier_0         : 1;
  uint32_t ext_comp_raw       : 1;
  uint32_t ext_main_me        : 1;

  uint32_t ext_prev_a_me      : 1;
  uint32_t ext_prev_b_me      : 1;
  uint32_t ext_prev_c_me      : 1;
  uint32_t ext_mcts           : 1;

  uint32_t ext_mctf           : 1;
  uint32_t ext_hier_y12       : 1;
  uint32_t ext_c2y_y12        : 1;
  uint32_t ext_prev_d_me      : 1;

  uint32_t vproc_rpt          : 1;
  uint32_t vproc_skip         : 1;
  uint32_t enc_start          : 1;
  uint32_t enc_stop           : 1;
  uint32_t ext_ir             : 1;
  uint32_t ext_mixer_scaler   : 1;
  uint32_t skip_cap_dram_out  : 1; // skip dram out for vin capture and skip send frame to vproc
  uint32_t skip_cap_dram_out_with_vproc_rpt  : 1; // skip dram out for vin capture and send repeat frame to vproc
  uint32_t rsvd1              : 8;

} event_mask_t;

///////////////////////////////////////////////////////////////////////////////

/// CMD_VIN_ATTACH_EVENT_TO_RAW (0x04000007)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id : 8;
  //uint32_t is_indep_fov_control : 1;
  uint32_t rsvd0  : 24;

  event_mask_t event_mask;

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
#if 0
typedef uint8_t vin_ce_out_format_t;

#define VIN_CE_OUT_FORMAT_DEINTERLEAVE 0u
#define VIN_CE_OUT_FORMAT_INTERLEAVE   1u
#endif
///////////////////////////////////////////////////////////////////////////////

/// CMD_VIN_CE_SETUP (0x04000009)
typedef struct
{
   uint32_t cmd_code;

   uint32_t vin_id                  : 8;
   uint32_t share_ce_from_other_vin : 1; /* if set, need ext mem to be sync'ed up
                                          with the vin actually capturing the shared ce data */
   uint32_t rsvd                    : 23;

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
   uint32_t multi_vin_hdr               : 1;
   uint32_t rsvd                        : 15;

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
#ifndef PROJECT_CV5  // let the system do the calculation
  // 3 idsp outputs, raw above and hds/ce below
  uint16_t hds_img_x_ofset;
  uint16_t hds_img_y_ofset;
  uint16_t hds_img_width;
  uint16_t hds_img_height;
  uint16_t ce_img_x_ofset;
  uint16_t ce_img_y_ofset;
  uint16_t ce_img_width;
  uint16_t ce_img_height;
#endif
  uint8_t  xpitch;              // pitch multipiler. set 1 for normal fov. set 2 for two interlaced fov.
  uint8_t  rsvd0;

  uint32_t hdr_intlac_mode    : 1; // 0: [(EXP,FOV)] = [(0,0);(0,1);(1,0);(1,1)]. 1: [(0,0);(1,0);(0,1);(1,1)]
  uint32_t is_fov_active      : 1;
  uint32_t is_last_fov        : 1;
#ifndef PROJECT_CV5
  uint32_t fov_raw_strm         :3; // indicate the raw fov strm to vproc: 0:idsp rwo out 1:idsp hds out
  uint32_t fov_ce_strm          :3; // indicate the ce fov strm to vproc:  1:idsp hds out 2: idsp ce out
  uint32_t hdec_factor_minus_1  :3; // between raw_strm and ce_strm defined above
  uint32_t non_low_delay_fov    :1; // no presend, and send by frame even for sliced fov
  uint32_t rsvd1                :19;
#else
  uint32_t is_prev_fov        : 1;
  uint32_t non_low_delay_fov  : 1; // no presend, and send by frame even for sliced fov
  uint32_t rsvd1              : 27;
#endif
} fov_layout_t;

/// CMD_VIN_SET_FOV_LAYOUT (0x0400000c)

typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id                 : 8;
  uint32_t rsvd0                  : 24;

  uint32_t num_of_fovs;
  uint32_t fov_lay_out_ptr;
#ifndef PROJECT_CV5
  u64_addr_upper_t fov_lay_out_ptr_upper;
#endif
} cmd_vin_set_fov_layout_t;

///CMD_VIN_SET_FRM_VPROC_DELAY (0x0400000d)
typedef struct
{
  uint32_t cmd_code;
  uint32_t vin_id                 : 8;
  uint32_t rsvd0                  : 24;

  uint32_t delay_time_msec;
} cmd_vin_set_frm_vproc_delay_t;

#define VIN_MAX_FOV_NUM FLOW_MAX_DATA_OUT_PINS //max num of fovs per vin

/// CMD_VIN_ATTACH_PROC_CFG_TO_CAP_FRM (0x0400000e)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id   : 8;
  uint32_t fov_num  : 8;
  uint32_t rsvd0    : 16;

  uint32_t fov_batch_cmd_set_addr; /* vin_fov_batch_cmd_set_t */
#ifndef PROJECT_CV5
  u64_addr_upper_t fov_batch_cmd_set_addr_upper;
#endif
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

#ifndef PROJECT_CV5
  u64_addr_upper_t per_fov_sideband_info_addr_upper[MAX_SIDEBAND_INFO_PER_CMD];
#endif
} cmd_vin_attach_sideband_info_to_cap_frm_t;

///////////////////////////////////////////////////////////////////////////////

typedef uint8_t vin_simu_vproc_dram_in_fmt_t;

#define RAW_IMG_DATA_TO_VPROC    0u
#define YUV420_IMG_DATA_TO_VPROC 1u

///////////////////////////////////////////////////////////////////////////////
#if 0
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
#endif

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

#ifndef PROJECT_CV5
  u64_addr_upper_t metadata_addr_upper;
#endif

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

typedef struct
{
  uint32_t tar_id :8;
  uint32_t rsvd0  :24;
  uint32_t bitmask;
  uint32_t pattern;
} pattern_demux_cfg_t;

/// CMD_VIN_PATTERN_DEMUX_SETUP  (0x04000014u)
typedef struct
{
  uint32_t cmd_code;

  uint32_t vin_id               :8;
  uint32_t pattern_demux_num_m1 :4; /* num = pattern_demux_num_m1+1 */
  uint32_t en_pattern_demux     :1;
  uint32_t pattern_demux_mode   :1; /* 0 - uncompr raw out, 1 - aux out */
  uint32_t rsvd0                :18;

  /* dram address of pattern_demux_num_m1+1 pattern_demux_cfg_t blocks */
  uint32_t pattern_demux_cfgs_daddr;
#ifndef PROJECT_CV5
  u64_addr_upper_t pattern_demux_cfgs_daddr_upper;
#endif
} cmd_vin_pattern_demux_setup_t;

#define MAX_SIDEBAND_INFO_PER_CMD (8u)


/******************************************************************************
 * CAT_VOUT(5)
 *****************************************************************************/
/////////////////////////////////////////////////////////////////////////////
#define VOUT_ID_A   0u
#define VOUT_ID_B   1u
#define VOUT_ID_INVALID   0xFFu

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
  uint16_t vout_id         :8;
  uint16_t virtual_chan_id :8;
  uint16_t reserved;

} cmd_vout_hdr_t;

// (cmd code 0x05000001)
typedef struct
{
  uint32_t cmd_code;                  /* W0 */
  uint16_t vout_id         :8;
  uint16_t virtual_chan_id :8;        /* W1 */
  uint8_t  interlaced;
  uint8_t  frm_rate;
  uint16_t act_win_width;             /* W2 */
  uint16_t act_win_height;
  uint8_t  back_ground_v;
  uint8_t  back_ground_u;             /* W3 */
  uint8_t  back_ground_y;
  uint8_t  mixer_444;
  uint8_t  highlight_v;
  uint8_t  highlight_u;               /* W4 */
  uint8_t  highlight_y;
  uint8_t  highlight_thresh;
  uint8_t  reverse_en;
  uint8_t  csc_en;                    /* W5 */
  uint8_t  mixer_420;
  uint8_t  reserved;                  /* W6 */
  uint32_t csc_parms[9];              /* W15 */
  uint32_t mixer_dram_out_y_addr;     /* W16 */
  uint32_t mixer_dram_out_uv_addr;    /* W17 */
  uint16_t mixer_dram_out_pitch;
  uint8_t  mixer_dram_out;            /* mixer output to dram */
  uint8_t  reserved2;                 /* W18 */

#ifndef PROJECT_CV5
  u64_addr_upper_t mixer_dram_out_y_addr_upper;
  u64_addr_upper_t mixer_dram_out_uv_addr_upper;
#endif
} cmd_vout_mixer_setup_t;

// (cmd code 0x05000002)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
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
  uint8_t  is_video_lowdelay;
  uint8_t  reserved;
  uint8_t  reserved1;

#ifndef PROJECT_CV5
  u64_addr_upper_t default_img_y_addr_upper;
  u64_addr_upper_t default_img_uv_addr_upper;
#endif
} cmd_vout_video_setup_t;

// (cmd code 0x05000003)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint16_t reserved;
  uint32_t default_img_y_addr;
  uint32_t default_img_uv_addr;
  uint16_t default_img_pitch;
  uint8_t  default_img_repeat_field;
  uint8_t  reserved2;

#ifndef PROJECT_CV5
  u64_addr_upper_t default_img_y_addr_upper;
  u64_addr_upper_t default_img_uv_addr_upper;
#endif
} cmd_vout_default_img_setup_t;

// (cmd code 0x05000004)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
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

#ifndef PROJECT_CV5
  u64_addr_upper_t osd_buf_dram_addr_upper;
  u64_addr_upper_t osd_buf_info_dram_addr_upper;
#endif
} cmd_vout_osd_setup_t;

// (cmd code 0x05000005)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint16_t reserved;
  uint32_t osd_buf_dram_addr;
  uint16_t osd_buf_pitch;
  uint8_t  osd_buf_repeat_field;

#ifndef PROJECT_CV5
  u64_addr_upper_t osd_buf_dram_addr_upper;
#endif
} cmd_vout_osd_buf_setup_t;

// (cmd code 0x05000006)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint16_t reserved;
  uint32_t clut_dram_addr;

#ifndef PROJECT_CV5
  u64_addr_upper_t clut_dram_addr_upper;
#endif
} cmd_vout_osd_clut_setup_t;

// (cmd code 0x05000007)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint16_t dual_vout_vysnc_delay_ms_x10;
  uint32_t disp_config_dram_addr;
  uint16_t vin_vout_vsync_delay_ms_x10;
  uint16_t vout_sync_with_vin;

  uint32_t vin_vout_vsync_timeout_ms: 16;
  uint32_t is_use_disp_c    : 1;      /* always 0 (use Display A) if vout_id==0 */
                                      /* 0: use Display B if vout_id==1 */
                                      /* 1: use Display C if vout_id==1 */
  uint32_t reverse_polarity : 1;
  uint32_t reserved         :14;

#ifndef PROJECT_CV5
  u64_addr_upper_t disp_config_dram_addr_upper;
#endif
} cmd_vout_display_setup_t;

// (cmd code 0x05000008)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint16_t reserved;
  uint32_t dve_config_dram_addr;

#ifndef PROJECT_CV5
  u64_addr_upper_t dve_config_dram_addr_upper;
#endif
} cmd_vout_dve_setup_t;

// (cmd code 0x05000009)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint8_t  reset_mixer;
  uint8_t  reset_disp;
  uint8_t  reset_top;
} cmd_vout_reset_t;

// (cmd code 0x0500000A)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint16_t csc_type; // 0: digital; Only digital csc is supported in cv2a
  uint32_t csc_parms[9];

} cmd_vout_display_csc_setup_t;

// (cmd code 0x0500000B)
typedef struct
{
  uint32_t cmd_code;
  uint32_t vout_id         :8;
  uint32_t virtual_chan_id :8;
  uint32_t reserved        :16;
  uint32_t output_mode;

} cmd_vout_digital_output_mode_setup_t;

// (cmd code 0x0500000C)
typedef struct
{
  uint32_t cmd_code;
  uint8_t  vout_id;
  uint8_t  virtual_chan_id;
  uint8_t  enable;
  uint8_t  setup_gamma_table;
  uint32_t gamma_dram_addr;

#ifndef PROJECT_CV5
  u64_addr_upper_t gamma_dram_addr_upper;
#endif
} cmd_vout_gamma_setup_t;

/******************************************************************************
 * CAT_ENC(6)
 *****************************************************************************/
typedef uint8_t enc_coding_type_t;

#define ENC_UNKNOWN 0u
#define ENC_H264    1u
#define ENC_HEVC    2u
#define ENC_JPEG    3u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_paff_mode_t;

#define PAFF_ALL_FRM   1u
#define PAFF_ALL_FLD   2u
#define PAFF_ALL_MBAFF 3u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t enc_gop_struct_t;

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
#define STATUS_FIFO_UPDATE 2u

/////////////////////////////////////////////////////////////////////////////

// enc_status_msg_t is moved to the end of this file
// Stream Type definition used in command/message

typedef uint8_t stream_type_t;

#define STRM_TP_INVALID         0u
#define STRM_TP_ENC_FULL_RES    0x11u
#define STRM_TP_ENC_STILL_MJPEG 0x19u

/////////////////////////////////////////////////////////////////////////////

#define ENC_BITS_INFO_STEPSIZE  64u

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
  uint32_t tile_id                                     : 4;
  uint32_t tile_num                                    : 4;
  uint32_t pts_minus_dts                               : 5;
  uint32_t top_field_first                             : 1;
  uint32_t repeat_first_field                          : 1;
  uint32_t pic_is_longterm_ref                         : 1;
  uint32_t force_frame_skip                            : 1;
  uint32_t reserved_1                                  : 6;

  // W2
  uint32_t sliceheader_offset                          : 16;
  uint32_t slice_num                                   : 8;
  uint32_t slice_idx                                   : 8;

  // W3-8
  uint32_t frame_num;
  uint32_t pts_hw;
  uint32_t encode_done_pts;
  uint32_t bits_start_addr;                                  // stream start address
  uint32_t bits_size;                                        // pic stream size
  uint32_t checksum;                                         // for debugging

  // W9-10
  uint32_t bitrate_kbps;                                     // to support bitrate change on-the-fly
  uint32_t cap_seq_no;

  // W11
  uint32_t session_id;

#ifndef PROJECT_CV5
  // W12
  u64_addr_upper_t bits_start_addr_upper;

  // W13-15
  /* Pad to make it total of 64 bytes */
  uint32_t reserved_3[3];
#else
  // W12-15
  /* Pad to make it total of 64 bytes */
  uint32_t reserved_3[4];
#endif

} enc_bits_info_t;

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

typedef union
{
  vbr_config_t field;
  uint32_t     word;

} vbr_config_ut;

typedef struct
{
  uint32_t cmd_code;
  uint32_t channel_id : 8;
  uint32_t force_update_flag : 1;
  uint32_t rsvd       : 23;
  /* make target_pts last 4 bytes of 128 byte DSP_CMD_SIZE
   * Remember if we add any cmds that have vdsp sync we need to follow this template*/
  uint32_t reserved[29];
  uint32_t target_pts ;
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
#define MD_CAT_TYPE_FORCED_INTER16 5u
#define MD_CAT_TYPE_FORCED_INTER32 6u
#define MD_CAT_TYPE_NUM            7u

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t hevc_perf_mode_t;

#define PERF_DEFAULT          0
#define PERF_4KP30_FASTEST_P1 1
#define PERF_4KP30_FAST_P2_ZMV 2
#define PERF_FAST_800         10  // 800 cycles/CTB

/////////////////////////////////////////////////////////////////////////////

typedef uint8_t trigger_frame_mode_t;

#define TRIGGER_FRAME_DROP   0u
#define TRIGGER_FRAME_ENC    1u

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
  uint32_t core_used              : 2;   // 0 or 1: core#0 used
                                         // 2: core#1 used
                                         // 3: both cores used
  uint32_t dual_core_mode         : 2;   // 0: for dual-core-temporal
                                         // 1: for dual-core-stripe (for hevc only)
  uint32_t ceng_used              : 2;   // 0 or 1: coding engine#0(HE0) used
                                         // 2: coding engine#1(HE1) used
  uint32_t enc_vdsp_info_enable   : 1;
  uint32_t reserved_3             : 2;
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
  uint32_t tile_num               : 3;   // number of tiles (only for hevc)
  uint32_t beating_reduction      : 5;   // bit[0]:   0=off 1=on.
                                         // bit[4:1]: 0=default 1=weakest 9=strongest
                                         // range=0-9, others=undef
  uint32_t efm_enable             : 1;   // 0=encode from enc_src, 1=encode from memory
  uint32_t use_cabac              : 1;   // 0=cavlc 1=cabac
  uint32_t quality_level          : 14;  // [1:0]=inter_block_size, [3:2]=rdo_mode
                                         // [4]=tr8x8, [5]=constrained_intra, [6]=longterm_ref
                                         // [7]=weighted_pred(P), [8]=weighted_bipred(B), [9]=p2_ref1_full_ME
                                         // others=undef

  uint32_t  rc_mode               : 3;   // rc_mode_t
  uint32_t  initial_qp            : 6;   // initial qp used when rc_mode=0
  uint32_t  rc_overlap_memd       : 1;
  uint32_t  IR_max_qp             : 6;   // for IR_tuning_mode, set max QP [1-51]
                                         // 0=off: no max QP limit
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
  uint32_t IR_tuning_mode         : 1;   // enable special tunings for intra refresh (only for ENC_RC_CBR(1))
  uint32_t IR_max_qp_spread       : 4;   // for IR_tuning_mode, set max QP spread [0-15]
                                         // 4 cat max QP=[IR_max_qp+qp_spread, IR_max_qp, IR_max_qp-qp_spread, IR_max_qp-2*qp_spread]
  uint32_t disable_sao            : 1;   // for HEVC only
  uint32_t custom_df_control_flag : 1;   // which way to set deblocking filter parameters(disable_df_idc/df_param1/df_param2)
                                         // 1: use external deblocking filter control params;
                                         // 0: use DSP default setting (disable_df_idc=0/df_param1=6/df_param2=6)
  uint32_t disable_df_idc         : 2;   // HEVC: slice_deblocking_filter_disabled_flag (0 or 1)
                                         //       0=default; 1=disable loop filter (deblock + SAO)
                                         // AVC : disable_deblocking_fliter_idc         [0,2]
                                         //       0=default, 1=disable, 2=disable df across slice boundary
  uint32_t df_param1              : 4;   // HEVC: slice_tc_offset_div2       + 6        [0,12]
                                         // AVC : slice_alpha_c0_offset_div2 + 6        [0,12]

  uint32_t scalelist_opt;                // 0=off 1=standard's default non-flat matrix
                                         // ">=2" = user defined scale matrix dram address

  uint32_t idr_interval;
  uint32_t cpb_user_size;                // (bps, just follow the unit of average_bitrate)

  uint32_t stat_fifo_base;               // refer to struct ENCODER_STATISTIC (no use)
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
  uint32_t df_param2                : 4; // HEVC: slice_beta_offset_div2     + 6        [0,12]
                                         // AVC : slice_beta_offset_div2     + 6        [0,12]
  uint32_t two_lt_ref_mode          : 1;
  uint32_t aqp_lambda               : 2; // 0 - use slice QP for lambda, 1, 2 - use CU QP for lambda

  uint32_t mvdump_daddr;
  uint32_t mvdump_fifo_limit;
  uint32_t mvdump_fifo_unit_sz;
  uint32_t test_binary_daddr;            // dram address to contain sample data, encoder config and
                                         // golden checksum for safety check purpose.
                                         // 0 - disable.

  uint32_t is_high_priority_stream : 1;
  uint32_t enc_start_method        : 1;
  uint32_t enc_stop_method         : 1;
  uint32_t hevc_perf_mode          : 5;   // hevc_perf_mode_t
  uint32_t embed_code              : 2;   // embed cap_seq_no/enc_id into the 1st MB/CTB
                                          // bit[0]: embed cap_seq_no
                                          // bit[1]: embed enc_id
  uint32_t tier_idc                : 1;
  uint32_t recon_offset_y          : 4;   // must be multiple of 2; and >= real meta data height; DSP_ENC_CFG's extra_height also need to be set.
  uint32_t is_recon_to_vout        : 2;   // 1: recon buffer to vout b; 2: to vout a;
  uint32_t IR_no_overlap           : 1;   // disable MB overlapping between 2 neighboring frames
  uint32_t max_smvmax_scale        : 3;   // 0: default; ">=1": user defined (max_smvmax = max_smvmax_scale*8)
  uint32_t per_slice_vdsp_interrupt : 1;  // 0: default; 1: enable one vdsp interrupt per slice
  uint32_t enc_pic_q_low_delay     : 1;   // 0: default; 1: encode immediately when picture comes in
  uint32_t is_mono                 : 1;   // 0: default, 1: enable mono
  uint32_t max_vdsp_dummy_latency  : 3;   // [0, 5] per stream enc dummy latency.
  uint32_t is_auto_frm_drop_en     : 1;
  uint32_t reserved                : 4;

  uint32_t reserved_5;                    // 128B (CV5 CMD_MSG_SIZE = 128B)

#ifndef PROJECT_CV5  // CV6 CMD_MSG_SIZE = 256B
  u64_addr_upper_t vbr_init_data_daddr_upper;
  u64_addr_upper_t bits_fifo_base_upper;
  u64_addr_upper_t info_fifo_base_upper;
  u64_addr_upper_t scalelist_opt_upper;
  u64_addr_upper_t test_binary_daddr_upper;
#endif

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

typedef union
{
  enc_frame_crop_t  bits;
  uint32_t          word;

} enc_frame_crop_ut;

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
                                                     // 3: IDR: AUD -> SPS -> BPSEI & PTSEI, P: AUD -> SPS & PPS -> PTSEI
                                                     // 4: extended by au_type_msb, no AUD, SPS/PPS -> SEI
  uint32_t au_type_msb                          : 2; // msb for the extension of au type, type is defined as ((au_type_msb << 2) | au_type)
  uint32_t append_user_data_sei                 : 1; // for user_data sei placeholder
  uint32_t reserved                             : 9;

  uint32_t encode_duration;
  uint32_t start_encode_frame_no;

  enc_frame_crop_t  frame_crop;
  h264_vui_t        h264_vui;    // h264 seq_hdr->VUI parameters

  uint32_t memd_time_out_msec;   // 0: no time out

  uint32_t session_id;
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

#define NUM_PIC_TYPES 3u

typedef uint8_t encode_param_enable_flags_t;

#define ENC_PARAM_QP_LIMIT                 0x1u
#define ENC_PARAM_INSERT_IDR               0x2u
#define ENC_PARAM_GOP                      0x4u
#define ENC_PARAM_FRAME_RATE               0x8u
#define ENC_PARAM_BITRATE_CHANGE           0x10u
#define ENC_PARAM_TRIGGER_FRAME            0x20u
#define ENC_PARAM_QUANT_MATRIX             0x40u
#define ENC_PARAM_OFFSET                   0x80u
#define ENC_PARAM_MONO_ENABLE              0x100u
#define ENC_PARAM_MDSWCAT                  0x200u
#define ENC_PARAM_FAST_SEEK_INTERVAL       0x400u
#define ENC_PARAM_MVDUMP                   0x800u
#define ENC_PARAM_FORCE_FRAME_PSKIP        0x1000u

/// CMD_ENCODER_REALTIME_SETUP (0x06000004)
typedef struct
{
  uint32_t cmd_code;
  uint32_t stream_id                 : 8;
  uint32_t force_update_flag         : 1;
  uint32_t fast_seek_interval        : 6;
  uint32_t force_pskip_num_plus1     : 8;
  uint32_t force_pskip_is_inverted   : 1;
  uint32_t qlevel                    : 8;

  uint32_t enable_flags;  // refer to encode_param_enable_flags_t

  // simple rc parameters
  uint8_t  qp_min_on_I;
  uint8_t  qp_max_on_I;
  uint8_t  qp_min_on_P;
  uint8_t  qp_max_on_P;

  uint8_t  aqp;
  int8_t   i_qp_reduce;
  int8_t   p_qp_reduce;
  int8_t   b_qp_reduce;

  // gop change
  uint32_t gop_n_new              : 16;

  // frame trigger
  uint32_t frame_trigger_num      : 8;
  uint32_t frame_trigger_mode     : 1;
  uint32_t reserved2              : 7;

  // insert idr, idr interval, frame rate muli/div factor
  uint32_t idr_interval_new       : 8;
  uint32_t force_idr              : 1; // 0: disabled, 1: Change next I/P to IDR
  uint32_t is_mono                : 1; // 0: disabled, 1: enable mono
  uint32_t mvdump_enable          : 1;
  uint32_t intra_bias_override_usr1 : 1 ; /* if 0  intra_bias[0] is added, 1  intra_bias[0] is directly applied */
  uint32_t direct_bias_override_usr1 : 1 ; /* if 0  direct_bias[0] is added, 1  direct_bias[0] is directly applied */
  uint32_t intra_bias_override_usr2 : 1 ; /* if 0  intra_bias[1] is added, 1  intra_bias[1] is directly applied */
  uint32_t direct_bias_override_usr2 : 1 ; /* if 0  direct_bias[1] is added, 1  direct_bias[1] is directly applied */
  uint32_t reserved3              : 1;

  uint32_t multiplication_factor  : 8;
  uint32_t division_factor        : 8;

  uint32_t enc_frame_rate;

  uint32_t target_bitrate;

  uint32_t dqt_daddr;

  uint32_t encode_w_ofs : 16;  // offset between input image and real encode image.
  uint32_t encode_h_ofs : 16;

  uint8_t  qp_min_on_B;
  uint8_t  qp_max_on_B;
  uint8_t  qp_min_on_C;
  uint8_t  qp_max_on_C;
  uint8_t  qp_min_on_D;
  uint8_t  qp_max_on_D;
  uint8_t  qp_min_on_Q;
  uint8_t  qp_max_on_Q;
  int8_t   c_qp_reduce;
  int8_t   q_qp_reduce;
  uint8_t  skip_flags;
  uint8_t  skip_flags_mode : 1; // 0: handle skip_flags through drop frame; 1: through repeat frame;
  uint8_t  reserved5      : 7;

  uint32_t set_I_size; /* max size in bits for I frame, set 0 to disable */

  // mdSwCat
  uint32_t mdSwCat_daddr[NUM_PIC_TYPES];             /* the MB/CTB Map for per MB/CTB Encoding Control (0: I, 1: P, 2: B) */
  uint32_t mdSwCat_daddr_P2;                         /* mdSwCat_daddr for 2-ref P (FIXME--CV5: P2 is not ready) */
                                                     // AVC:  MB level Encoding Control (4 bytes per MB)
                                                     //       usr_class[1:0]: 4 classes (0: default, 1/2: for intra/direct_bias, 3: reserve for IR)
                                                     //       deltaQP [14:8]: valid values are -51 to 51
                                                     // HEVC: CTB level Encoding Control (16 bytes per CTB: HevcMvsrcPad)
                                                     //       maxi_category_cu32/cu16_0/1/2/3: use default value 15
                                                     //       md_category_cu32/cu16_0/1/2/3: 8 md_categories (0-6: for md_category_lookup, 7 reserve for IR)
                                                     //       qp_adjust_cu32/cu16_0/1/2/3: valid values are -51 to 51

  uint16_t intra_bias[2];                            /* AVC only: can only change usr class 1 and 2 biases: the higher the values bias against intra mode */
  uint16_t direct_bias[2];                           /* AVC only: can only change usr class 1 and 2 biases: the higher the values bias against direct mode */

  uint8_t  md_category_lookup[MD_CATEGORY_MAX_NUM];  /* HEVC only: md_cat_type_t for each md_category */

  uint32_t reserved4[8];  /* padd to match cmd_encoder_hdr_t template */
  uint32_t target_pts;    // 128B (CV5 CMD_MSG_SIZE = 128B)

#ifndef PROJECT_CV5  // CV6 CMD_MSG_SIZE = 256B
  u64_addr_upper_t dqt_daddr_upper;
  u64_addr_upper_t mdSwCat_daddr_upper[NUM_PIC_TYPES];
  u64_addr_upper_t mdSwCat_daddr_P2_upper;
#endif

} encoder_realtime_setup_t;

/////////////////////////////////////////////////////////////////////////////

#define ENC_MAX_TYPES_ALL  0  // for HEVC and H264
#define ENC_MAX_TYPES_H264 1  // for H264 only
#define ENC_MAX_TYPES_JPEG 2  // for JPEG only
typedef struct
{
  uint16_t max_enc_width;
  uint16_t max_enc_height;

  uint32_t max_enc_types    :2;  // resource allocation for enc types
                                 // 0: for HEVC and H264
                                 // 1: for H264 only
  uint32_t max_ref_num      :2;  // 0=default (1 ref). max=2 refs
  uint32_t core_used        :2;  // 0 or 1: core#0 used
                                 // 2: core#1 used
                                 // 3: both cores used
  uint32_t dual_core_mode   :2;  // 0: for dual-core-temporal
                                 // 1: for dual-core-stripe (for hevc only)
  uint32_t max_rec_fb_num   :8;
  uint32_t max_smvmax_h264  :8;  // H264's max_smvmax
  uint32_t max_smvmax_hevc  :8;  // HEVC's max_smvmax
} enc_cfg_t;

/////////////////////////////////////////////////////////////////////////////

/// CMD_ENCODER_JPEG_SETUP (0x06000005)
typedef struct
{

  uint32_t cmd_code;

  uint32_t stream_id      : 8;
  uint32_t stream_type    : 8;
  uint32_t core_used      : 2;
  uint32_t ceng_used      : 2;
  uint32_t dual_core_mode : 1;
  uint32_t reserved       : 11;

  uint32_t coding_type      : 8;  // enc_coding_type_t
  uint32_t enc_start_method : 1;  // enc_start_method_t
                                  // 0=start from enc_start cmd
                                  // 1=start by encode_start_idc/encode_stop_idc
                                  // in picinfo
  uint32_t enc_stop_method  : 1;
  uint32_t chroma_format    : 2;  // 0: 400 (mono), 1: 420, 2: 422
  uint32_t is_mjpeg         : 1;
  uint32_t hflip            : 1;
  uint32_t vflip            : 1;
  uint32_t rotate           : 1;
  uint32_t slice_num        : 8;
  uint32_t enc_vdsp_info_enable : 1;
  uint32_t reserved_1       : 7;

  uint32_t restart_interval;      // 0=handle in ucode, >0: user specified

  uint16_t encode_w;              // 0=from vproc, >0=from cmd
  uint16_t encode_h;

  uint32_t bits_fifo_base;
  uint32_t bits_fifo_size;
  uint32_t info_fifo_base;
  uint32_t info_fifo_size;

  uint32_t dqt_daddr;             // quant table address
  uint32_t enc_frame_rate;

  uint32_t encode_w_ofs : 16;     // the offset between input image and real encode image.
  uint32_t encode_h_ofs : 16;

  uint32_t target_bpp     : 16;   // target bits per pixel (4.12 fixed point)
  uint32_t enc_src        : 5;    // srcbuf_id
  uint32_t efm_enable     : 1;    // 0=encode from enc_src, 1=encode from memory
  uint32_t reserved_2     : 10;

  // reencode related
  uint32_t initial_qlevel : 8;
  uint32_t tolerance      : 8;    // % offset from target_bpp (in 0.8 fixed point)
  uint32_t max_reenc_loops: 8;    // max reencode loop
                                  // 0/1  : encode once
                                  // 2-254: encode up to 2-254 times
                                  // 255  : encode once and propagate qlevel to next
  uint32_t rct_sample_num : 8;    // rct=rate curve table
                                  // number of sample points in rct
  uint32_t rct_daddr;

  uint32_t bits_fifo_offset;

#ifndef PROJECT_CV5
  u64_addr_upper_t bits_fifo_base_upper;
  u64_addr_upper_t info_fifo_base_upper;
  u64_addr_upper_t dqt_daddr_upper;
#endif

} cmd_encoder_jpeg_setup_t;

/////////////////////////////////////////////////////////////////////////////

/// CMD_ENCODER_FLUSH_INPUT (0x06000006)
typedef struct
{
  uint32_t cmd_code;

  uint32_t channel_id : 8;
  uint32_t reserved   : 24;
} cmd_encoder_flush_input_t;

/// CMD_ENCODER_VDSP_SYNC_CMD (0x06000007)
typedef struct
{
  uint32_t cmd_code;
  uint32_t stream_id              :8;
  uint32_t force_update_flag      :1;
  uint32_t reserved               :23;
  uint64_t pic_frame_no;
  uint32_t cmd_daddr;

#ifndef PROJECT_CV5
  u64_addr_upper_t cmd_daddr_upper;
#endif
} VDSP_ENC_SYNC_CMD;
/******************************************************************************
 * CAT_DEC(7)
 *****************************************************************************/
typedef uint8_t dec_codec_type_t;
#define DEC_CODEC_TYPE_AVC     0u
#define DEC_CODEC_TYPE_HEVC    1u
#define DEC_CODEC_TYPE_JPEG    2u

typedef struct
{
  uint32_t cmd_code;

  uint8_t  decoder_id;
  uint8_t  codec_type; // 0: avc ;1: hevc;2 :jpeg
  uint8_t  reserved[2];
} cmd_dec_hdr_t;

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
  uint16_t lu_img_offset_x;
  uint16_t lu_img_offset_y;
  uint16_t ch_img_offset_x;
  uint16_t ch_img_offset_y;
#ifndef PROJECT_CV5
  u64_addr_upper_t lu_buf_base_upper;
  u64_addr_upper_t ch_buf_base_upper;
#endif
} dec_fb_desc_t;

// (cmd code 0x07000001)
typedef uint8_t dec_setup_type_t;
/* sync with dec_hl.h DEC_SETUP_TYPES first */
#define DEC_SETUP_TYPE_DEFAULT    0u // fully compliant on CV5 - Optimized for ambarella cv2a streams
#define DEC_SETUP_TYPE_NO_FMO     1u // DEC_SETUP_TYPE_LENIENT not sure on CV5 or not
#define DEC_SETUP_TYPE_I_ONLY     2u
#define DEC_SETUP_AMBA_TYPE       4u
#define DEC_SETUP_AU_BOUND_TYPE   8u
#define DEC_SETUP_LD_TYPE         16u
#define DEC_SETUP_TYPE_IP_ONLY    64u
#define DEC_SETUP_TYPE_CODING     128u

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

  uint8_t  cabac_2_recon_delay;
  uint8_t  err_handling_mode; // dec_err_handling_mode_t
  uint8_t  max_frm_num_of_dpb;
  uint8_t  enable_pic_info;

#ifndef PROJECT_CV5
  u64_addr_upper_t bits_fifo_base_upper;
  u64_addr_upper_t bits_fifo_limit_upper;
#endif
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

  uint32_t mdxf_core_usage :2;        /* 0/1: core 0 is assigned to decode(default) */
                                      /* 2: core 1 is assigned to decode */
                                      /* 3: dual cores are assigned to decode */
  uint32_t coding_engine_usage :2;    /* 0: using coding engine 0 */
                                      /* 1: using coding engine 1 */
  uint32_t dual_core_mode : 2;        /* 0: for dual-core-temporal */
                                      /* 1: for dual-core-stripe (for hevc only) */
  uint32_t reserved : 26;

  batch_cmd_set_info_t  batch_cmd_set_info;
#ifndef PROJECT_CV5
  u64_addr_upper_t bits_fifo_start_upper;
  u64_addr_upper_t bits_fifo_end_upper;
#endif
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
#ifndef PROJECT_CV5
  u64_addr_upper_t bits_fifo_start_upper;
  u64_addr_upper_t bits_fifo_end_upper;
#endif
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

typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t mode; // dec_trickplay_mode_t
} cmd_decoder_trickplay_t;

// (cmd code 0x07000007)
typedef struct
{
  cmd_dec_hdr_t hdr;

  uint32_t bstrm_start;
  uint32_t bstrm_size;

  uint32_t yuv_buf_base;
  uint32_t yuv_buf_size;

  uint32_t stripe_ena;  /* used for hevc stripe */
#ifndef PROJECT_CV5
  u64_addr_upper_t bstrm_start_upper;
  u64_addr_upper_t yuv_buf_base_upper;
#endif
} cmd_decoder_stilldec_t;

// (cmd code 0x07000008)
typedef struct
{
  cmd_dec_hdr_t hdr;

  batch_cmd_set_info_t  batch_cmd_set_info;

} cmd_decoder_batch_t;

/******************************************************************************
 * DSP msgs
 *****************************************************************************/
/* first DSP msg for each iteration */
/* and NOT supposed to be written explicity by DSP */

/// MSG_DSP_STATUS (0x81000001u)
typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;
  uint32_t dsp_prof_id        :16;
  uint32_t fatal_error_stat   : 1; // 0 means no fatal error, 1 means fatal err has happened

  /*
   * ORC_ID_TH_ID = 0,  1,  2,  3,  for orccode
   * ORC_ID_TH_ID = 4,  5,  6,  7,  for orcvin
   * ORC_ID_TH_ID = 8,  9,  10, 11, for orcidsp0
   * ORC_ID_TH_ID = 12, 13, 14, 15, for orcidsp1
   */
  uint32_t fatal_err_th_id    : 7; // the thread that first triggered the err.
  uint32_t reserved16_0       : 8;
  uint32_t time_code;
  uint32_t prev_cmd_seq;
  uint32_t prev_num_cmds;

  uint32_t orcvin_status   :8;
  uint32_t orccode_status  :8;
  uint32_t orcidsp0_status :8;
  uint32_t orcidsp1_status :8;

#ifdef PROJECT_CV6
  uint32_t orcvin1_status  :8;
  uint32_t reserved_1      :24;
#endif

  uint32_t dram_used;
} dsp_status_msg_t;

/// MSG_CMD_REQ (0x81000002u)
typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;
  uint32_t req_cmd;   /* 1 for request arm's cmd */
  uint32_t prev_cmd_seq;
  uint32_t prev_num_cmds;
  uint32_t vin_id;
  uint32_t is_time_out;

} dsp_cmd_req_msg_t;

#define ECHO_CMD_MSG_MAX_INFO_NUM ((DSP_MSG_SIZE / 8u) - 2u)
typedef struct
{
  uint32_t cmd_code;
  uint32_t stream_id  :8;   /* fov_id for VIN, channel_id for VPROC/ENC, decoder_id for DEC, 0xff for VOUT */
  uint32_t rsvd       :24;

} echo_cmd_info_t;

/// MSG_ECHO_CMD (0x81000003u)
typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;

  uint32_t num_rcvd_exec_cmd  :16;
  uint32_t module_id          :8;   /* 0: COMMON 1:VIN, 2:VPROC, 3:VOUT, 4:ENC, 5:DEC */
  uint32_t chan_id            :8;   /* VIN_ID for VIN, VOUT_ID for VOUT, 0xff for VPROC, ENC, and DEC */

  echo_cmd_info_t echo_cmd_info[ECHO_CMD_MSG_MAX_INFO_NUM];
} msg_echo_cmd_t;

#if 0
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
#endif

////////////////////////////////////////////////////////////////////////////////
// VPROC working modes
typedef uint8_t vproc_working_mode_t;

#define VPROC_TIMER_MODE  10u
#define VPROC_ACTIVE_MODE 20u
#define VPROC_PAUSE_MODE  30u

///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/// MSG_VPROC_STATUS (0x82000001)
/* C2Y step idsp sections' index */
#define  VPROC_C2Y_SEC2_IDX    0u
#define  VPROC_C2Y_SEC4_IDX    1u
#define  VPROC_C2Y_SEC_IDX_NUM 2u

/* Y2Y step idsp sections' index */
#define VPROC_Y2Y_SEC3_IDX    0u
#define VPROC_Y2Y_SEC5_IDX    1u
#define VPROC_Y2Y_SEC6_IDX    2u
#define VPROC_Y2Y_SEC7_IDX    3u
#define VPROC_Y2Y_SEC9_IDX    4u
#define VPROC_Y2Y_SEC11_IDX   5u
#define VPROC_Y2Y_SEC17_IDX   6u
#define VPROC_Y2Y_SEC18_IDX   7u
#define VPROC_Y2Y_SEC10_IDX   8u
#define VPROC_Y2Y_SEC_IDX_NUM 9u

/* OSD blend step idsp sections' index */
#define VPROC_OSD_BLEND_SEC10_IDX   0u
#define VPROC_OSD_BLEND_SEC_IDX_NUM 1u

/* MD step idsp sections' index */
#define VPROC_MD_PPL_SEC2_IDX 0u
#define VPROC_MD_PPL_SEC3_IDX 1u
#define VPROC_MD_PPL_SEC11_IDX VPROC_MD_PPL_SEC3_IDX
#define VPROC_MD_PPL_SEC18_IDX VPROC_MD_PPL_SEC3_IDX
#define VPROC_MD_PPL_SEC5_IDX 2u
#define VPROC_MD_PPL_SEC6_IDX 3u
#define VPROC_MD_PPL_SEC_IDX_NUM 4u

/* FISHI idsp sections' index */
#define VPROC_FISHI_PPL_SEC2_IDX 0u
#define VPROC_FISHI_PPL_SEC3_IDX 1u
#define VPROC_FISHI_PPL_SEC5_IDX 2u
#define VPROC_FISHI_PPL_SEC6_IDX 3u
#define VPROC_FISHI_PPL_SEC7_IDX 4u
#define VPROC_FISHI_PPL_SEC9_IDX 5u
#define VPROC_FISHI_PPL_SEC_IDX_NUM 6u

/* Index of extended status message dram addresses reported back to system */
#define VPROC_EXT_DATA_C2Y_Y2Y_IDX   0u
#define VPROC_EXT_DATA_HIER_LANE_IDX 1u

#define VPROC_SECT_CFG_HDR_NUMS 8u

#define VPROC_ERR_CODE_NO_ERROR 0x0u
#define VPROC_ERR_SYNC_CMD_OSD_UPDATE_FAIL 0x1u

typedef struct
{
  uint32_t msg_code;               /* Word 0 */
  uint32_t msg_crc;                /* Word 1 */

  uint8_t  channel_id;
  uint8_t  status;                 // refer to vproc_working_mode_t
                                   // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
                                   // 30: VPROC_PAUSE_MODE
  uint8_t  pass_step_id;           // IMG_PASS_XXX
  uint8_t  iso_mode;               // IMG_MODE_XXXXXX

  uint32_t cap_seq_no;            /* Word 3 */
  uint32_t batch_cmd_id;          /* Word 4 */

  uint32_t ext_buf_idc;           /* Word 5 */

  uint32_t is_last_tile    :1;
  uint32_t metadata_status :3;
  uint32_t group_id        :3;
  uint32_t vin_id          :4;
  uint32_t is_c2y_y12_ena  :1;
  uint32_t vin_id_msb      :4;
  uint32_t vin_pipeline_id :1;
  uint32_t tf_y2y_tc_cnt   :5;
  uint32_t is_tf_y2y_done  :1;
  uint32_t cmd_sync_err_code :3;
  uint32_t reserved_0      :6;    /* Word 6 */

  uint8_t  stitch_tile_num_x; // total number slices of X direction
  uint8_t  stitch_tile_num_y; // total number slices of Y direction
  uint8_t  tile_x_idx;        // current tile X direction index
  uint8_t  tile_y_idx;        // current tile Y direction index

  uint32_t sect_mask;              /* Word 8: indicate which section included */
  uint32_t sect_cfg_hdr_daddr[VPROC_SECT_CFG_HDR_NUMS];  /* Word 16: report cached sect_config_header to system with above C2Y/Y2Y section IDX */

  uint32_t vproc_done_hw_pts;    // This is Audio PTS, which is audio clock when DSP finishes procssing a frame
  uint32_t vin_cap_done_pts;
  uint32_t job_done_pts;           /* Word 19  */

  uint32_t ext_data_daddr[2];      /* Word 21: VPROC_EXT_DATA_XXXX_IDX  */
  uint32_t grp_cmd_buf_id;         /* Word 22 */

  uint32_t vproc_seq_no;                               /* Word 23 */
  uint32_t grp_cmd_buf_addr[2];                        /* Word 25: vout grp cmds */
  uint32_t tf_acc_hw_duration; // accumulated testframe hw duration
  uint32_t tf_job_done_duration; // latest job done duration since last issue job time stamp
  uint32_t job_start_pts;
  uint32_t reserved[3] ;

#ifndef PROJECT_CV5
  uint32_t reserved_1[1] ;
  u64_addr_upper_t sect_cfg_hdr_daddr_upper[VPROC_SECT_CFG_HDR_NUMS];
  u64_addr_upper_t ext_data_daddr_upper[2];
  u64_addr_upper_t grp_cmd_buf_addr_upper[2];
#endif
} msg_vproc_status_t;

/// MSG_VPROC_AAA_STATUS (0x82000002)
typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;

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

  uint32_t vin_cap_done_pts;

  uint32_t is_last_tile    :1;
  uint32_t aaa_cfa_mux_sel :2;
  uint32_t reserved_0      :29;

  uint32_t vproc_seq_no;

#ifndef PROJECT_CV5
  u64_addr_upper_t tile_params_addr_upper;
  u64_addr_upper_t raw_buf_addr_upper;
  u64_addr_upper_t cfa_aaa_stat_buf_addr_upper;
  u64_addr_upper_t rgb_aaa_stat_buf_addr_upper;
  u64_addr_upper_t vin_stat_buf_addr_upper;
  u64_addr_upper_t hdr_vin_stat_buf_addr_upper;
  u64_addr_upper_t hdr2_vin_stat_buf_addr_upper;
#endif

  uint32_t ik_cfg_id;
} msg_vproc_aaa_status_t;


/////////////////////////////////////////////////////////////////////////////
// VPROC_XXX_EXT_DATA structures for VPROC step/passes
typedef struct
{
  uint32_t buffer_addr;
#ifndef PROJECT_CV5
  u64_addr_upper_t buffer_addr_upper;
#endif
  uint16_t buffer_pitch;
  uint16_t img_width;
  uint16_t img_height;
  uint16_t reserved_0;

} image_buffer_desc_t;

// IMG_PASS_C2Y
typedef struct
{
  image_buffer_desc_t c2y_luma_out;
  image_buffer_desc_t c2y_chroma_out;
  image_buffer_desc_t compressed_raw_out;
  image_buffer_desc_t c2y_y12_out;
  image_buffer_desc_t c2y_me1_out;
  image_buffer_desc_t c2y_me0_out;
  image_buffer_desc_t c2y_ir_out;
} vproc_li_c2y_ext_data_t;


//  IMG_PASS_Y2Y: LC_Warp+MCTF+PrevABC
//  IMG_PASS_FISHI: LC_Warp+PrevABCD
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
  image_buffer_desc_t sec11_luma_out;
  image_buffer_desc_t sec11_chroma_out;
  image_buffer_desc_t sec11_me1_out;
  image_buffer_desc_t sec11_me0_out;
  image_buffer_desc_t mctf_luma_out;
  image_buffer_desc_t mctf_chroma_out;
  image_buffer_desc_t mcts_luma_out;
  image_buffer_desc_t mcts_chroma_out;
  image_buffer_desc_t sec9_luma_out;
  image_buffer_desc_t sec9_chroma_out;
  image_buffer_desc_t sec9_me1_out;
  image_buffer_desc_t sec9_me0_out;
  image_buffer_desc_t sec10_luma_out;
  image_buffer_desc_t sec10_chroma_out;
  image_buffer_desc_t sec17_luma_out;
  image_buffer_desc_t sec17_chroma_out;
  image_buffer_desc_t sec17_me1_out;
  image_buffer_desc_t sec17_me0_out;
} vproc_li_warp_mctf_prev_ext_data_t;

// OSD blend
typedef struct
{
  image_buffer_desc_t osd_blend_luma_out;
  image_buffer_desc_t osd_blend_chroma_out;
} vproc_osd_blend_ext_data_t;

// IMG_PASS_MD_BASE
typedef struct
{
  image_buffer_desc_t sec5_luma_out[2];
} vproc_md_ext_data_t;

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

/// MSG_VPROC_COMP_OUT_STATUS (0x82000003)
typedef struct {
  uint32_t msg_code;
  uint32_t msg_crc;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
  uint8_t  compression_out_enable;
  uint8_t  reserved_0;

  uint32_t cap_seq_no;
  uint32_t batch_cmd_id;

  image_buffer_desc_t compressed_raw_out;
} msg_vproc_comp_out_status_t;

typedef struct {
  uint32_t channel_id : 8;
  uint32_t strm_id : 8;
  uint32_t is_str_last_copy : 1;
  uint32_t is_last_copy_job : 1;
  uint32_t rsvd0 : 14;

  uint32_t cap_seq_no;
  uint32_t hw_pts;

  uint32_t lu_base;
  uint32_t chroma_base;

  uint32_t height;
  uint32_t width;
  uint32_t pitch;

  uint32_t input_start_x;
  uint32_t input_start_y;

  uint32_t rsvd1[6];
#ifdef PROJECT_CV6
  u64_addr_upper_t lu_base_upper;
  u64_addr_upper_t chroma_base_upper;
#endif
} vproc_copy_out_desc_t;

/// MSG_VPROC_EFFECT_DATA_STATUS (0x82000004)
typedef struct {
  uint32_t msg_code;
  uint32_t msg_crc;

  uint8_t  channel_id;
  uint8_t  status;                 // 10: VPROC_TIMER_MODE
                                   // 20: VPROC_ACTIVE_MODE
  uint8_t  stream_id;              // vproc_stream_idx_t
  uint8_t  group_id;

  uint32_t cap_seq_no;                  /* Word 3 */

  image_buffer_desc_t effect_luma;      /* Word 6 */
  image_buffer_desc_t effect_chroma;    /* Word 9 */

  /* me0/1 info of VPROC_MAIN_STREAM_IDX and VPROC_PIP_STREAM_IDX */
  image_buffer_desc_t effect_me0;       /* Word 12 */
  image_buffer_desc_t effect_me1;       /* Word 15 */

  uint32_t is_ext      : 1;
  uint32_t is_me01_ext : 1;
  uint32_t rsvd1       :30;             /* Word 16 */

  uint32_t vproc_seq_no;                /* Word 17 */
  uint32_t source_fov_bitmask;          /* Word 18 */

  uint32_t grp_cmd_buf_addr[2];     /* Word 19-20: vout grp cmds */
  uint32_t reserved;    /* Word 21: */

  uint32_t vin_cap_done_pts;
  uint32_t job_start_pts;
  uint32_t job_done_pts;

#ifndef PROJECT_CV5
  u64_addr_upper_t grp_cmd_buf_addr_upper[2];
  u64_addr_upper_t enc_grp_cmd_buf_addr_upper;
  u64_addr_upper_t copy_out_desc_addr_upper;
#endif

  uint32_t copy_out_desc_addr;
} msg_vproc_effect_data_status_t;


///////////////////////////////////////////////////////////////////////////////
typedef uint8_t vin_raw_capture_status_t;

#define TIMER_MODE   10u
#define CAPTURE_MODE 20u

///////////////////////////////////////////////////////////////////////////////

#define RAW_COMP_CFG_DATA_SIZE 32u
/// MSG_VIN_SENSOR_RAW_CAPTURE_STATUS (0x84000001)
typedef struct
{
  // W0
  uint32_t    msg_code;
  // W1
  uint32_t    msg_crc;

  // W2
  uint32_t    vin_id              : 8;
  uint32_t    raw_capture_status  : 8; // refer to vin_raw_capture_status_t
  uint32_t    is_capture_time_out : 1;
  uint32_t    rpt_frm_cntrl       : 4;
  uint32_t    is_external_raw_buf_from_system : 1;
  uint32_t    is_external_ce_buf_from_system  : 1;
  uint32_t    raw_cap_buf_ch_fmt  : 4; // for send to enc case, 0:YUV_MONO, 1:YUV_420, 2:YUV_422
  uint32_t    is_ext_aux_out_buf_from_system  : 1;
  uint32_t    is_skip_vproc       : 1;
  uint32_t    vin_pipeline_id     : 1;
  uint32_t    is_fb_empty         : 1;
  uint32_t    is_output_q_full    : 1;

  // W3
  uint32_t    raw_cap_cnt;

  // W4
  uint32_t    raw_cap_buf_addr;

  // W5
  uint16_t    raw_cap_buf_pitch;
  uint16_t    raw_cap_buf_width;

  // W6
  uint16_t    raw_cap_buf_height;
  uint16_t    rsvd1;

  // W7
  uint32_t    raw_cap_buf_fid;

  // W8
  uint32_t    pts;

  // W9
  uint32_t    pts_diff;

  // W10 - w12
  batch_cmd_set_info_t batch_cmd_set_info;

  // W13
  event_mask_t event_mask;

  // W14
  uint32_t    config_addr;

  // W15
  uint32_t    ce_cap_buf_fid;

  // W16
  uint32_t    ce_cap_buf_addr;

  // W17
  uint16_t    ce_cap_buf_pitch;
  uint16_t    ce_cap_buf_width;

  // W18
  uint16_t    ce_cap_buf_height;
  uint16_t    rsvd2;

  // W19
  uint32_t    frm_vproc_dalay_time_ticks;

  // W20
  uint32_t    raw_repeat_default_buf_addr;  // luma part for yuv422 case

  // W21
  uint32_t    ce_repeat_default_buf_addr;   // chroma part for yuv422 case

  // W22
  uint8_t  raw_cap_buf_num_tot; // yuv buf if yuv422 capture case
  uint8_t  raw_cap_buf_num_free; // yuv buf if yuv422 capture case
  uint8_t  ce_cap_buf_num_tot;
  uint8_t  ce_cap_buf_num_free;

  // W23
  uint32_t  current_cap_slice_idx       :4;  // slice capture
  uint32_t  cap_slice_num               :4;  // slice capture
  uint32_t  temporal_demux_tar_vin_id   :8;
  uint32_t  pattern_demux_tar_num       :4;
  uint32_t  rsvd3                       :12;

  // W24
  uint32_t  aux_out_cap_buf_fid;

  // W25
  uint32_t  aux_out_cap_buf_addr;

  // W26
  uint16_t  aux_out_cap_buf_pitch;
  uint16_t  aux_out_cap_buf_width;

  // W27
  uint16_t  aux_out_cap_buf_height;
  uint8_t   aux_out_cap_buf_num_tot;
  uint8_t   aux_out_cap_buf_num_free;

#ifndef PROJECT_CV5
  // W29
  uint32_t  hds_out_cap_buf_fid;

  // W30
  uint32_t  hds_out_cap_buf_addr;

  // W31
  uint16_t  hds_out_cap_buf_pitch;
  uint16_t  hds_out_cap_buf_width;

  // W32
  uint16_t  hds_out_cap_buf_height;
  uint8_t   hds_out_cap_buf_num_tot;
  uint8_t   hds_out_cap_buf_num_free;

  // W33
  uint32_t  hds_repeat_default_buf_addr;

  // W34
  u64_addr_upper_t  raw_cap_buf_addr_upper;

  // W35
  u64_addr_upper_t  config_addr_upper;

  // W36
  u64_addr_upper_t  ce_cap_buf_addr_upper;

  // W37
  u64_addr_upper_t  aux_out_cap_buf_addr_upper;

  // W38
  u64_addr_upper_t  hds_out_cap_buf_addr_upper;

  // W39
  u64_addr_upper_t  raw_repeat_default_buf_addr_upper;

  // W40
  u64_addr_upper_t  ce_repeat_default_buf_addr_upper;

  // W41
  u64_addr_upper_t  hds_repeat_default_buf_addr_upper;
#else
  // W28
  uint32_t  prev_out_cap_buf_fid;

  // W29
  uint32_t  prev_out_cap_buf_addr;

  // W30
  uint16_t  prev_out_cap_buf_pitch;
  uint16_t  prev_out_cap_buf_width;

  // W31
  uint16_t  prev_out_cap_buf_height;
  uint8_t   prev_out_cap_buf_num_tot;
  uint8_t   prev_out_cap_buf_num_free;
#endif
} msg_vin_sensor_raw_capture_status_t;

///////////////////////////////////////////////////////////////////////////////
// MSG_VOUT_STATUS (0x85000001)
typedef struct
{
  uint32_t    msg_code;
  uint32_t    msg_crc;

  uint32_t    vout_id              : 8;  // 0 = vout A, 1 = vout B
  uint32_t    vout_status          : 8;
  uint32_t    is_vin_vout_locked   : 1;
  uint32_t    vout_in_safe_state   : 1;
  uint32_t    num_susp_thread      : 4;
  uint32_t    is_vout_timed_out    : 2; // bit 0: start time timed out, bit 1: end time timed out
  uint32_t    frm_sync_timeout_cnt : 8;

  uint32_t    vout_disp_fid;
  uint32_t    vout_disp_cmd_buf_id;
  uint32_t    vout_disp_luma_addr;
  uint32_t    vout_disp_chroma_addr;
  uint32_t    vout_disp_start_time;
  uint32_t    vout_disp_done_time;
  uint32_t    vout_buffer_pitch;
  uint32_t    vout_img_width;
  uint32_t    vout_img_height;
  uint32_t    tot_num_of_frame_dropped;
  uint32_t    tot_num_of_frame_repeated;
  uint32_t    vproc_fov_id;
  uint32_t    vin_cap_seq;
  uint32_t    vin_cap_pts;
  uint32_t    mixer_config_dump_daddr;
  uint32_t    display_config_dump_daddr;
  uint32_t    osd_res_config_dump_daddr;
  uint32_t    vout_luma_crc;
  uint32_t    vout_chroma_crc;

  uint32_t    is_use_disp_c : 1;
  uint32_t    reserved : 31;

  uint32_t    vproc_frames_ready_hw_pts;

#ifndef PROJECT_CV5
  u64_addr_upper_t vout_disp_luma_addr_upper;
  u64_addr_upper_t vout_disp_chroma_addr_upper;
  u64_addr_upper_t mixer_config_dump_daddr_upper;
  u64_addr_upper_t display_config_dump_daddr_upper;
  u64_addr_upper_t osd_res_config_dump_daddr_upper;
#endif
} msg_vout_status_t;

////////////////////////////////////////////////////////////////////////////////
typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;

  uint32_t stream_id           : 8;
  uint32_t stream_type         : 8;
  uint32_t enc_status          : 2; // enc_status_t
  uint32_t checksum_err        : 1; // for Test Encoder safety check
  uint32_t stat_fifo_wr_idx    : 3;
  uint32_t reserved0           : 10;

  uint32_t total_bits_info_ctr_h264;
  uint32_t total_bits_info_ctr_hevc;
  uint32_t total_bits_info_ctr_jpeg;

  uint32_t pts_hw;
  uint32_t reserved1;

  uint32_t code_checksum;           // for Test Encoder safety check
} enc_status_msg_t;

typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;
  uint32_t stream_id        : 8;
  uint32_t enc_type         : 2;
  uint32_t enc_status       : 2;
  uint32_t pic_type         : 3;
  uint32_t pic_is_idr       : 1;
  uint32_t is_memd_time_out : 1;
  uint32_t checksum_err     : 3; // for Test Encoder safety check
  uint32_t core_type        : 3; // core_type_t
  uint32_t stripe_id        : 2;
  uint32_t rsvd0            : 7;

  uint32_t rec_y_addr;
  uint32_t rec_uv_addr;
  uint32_t rec_y_pitch    : 16;
  uint32_t mvdump_pitch : 16;

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
  uint32_t memd_done_pts;

  uint32_t memd_checksum[5];     // for Test Encoder safety check

#ifndef PROJECT_CV5
  u64_addr_upper_t rec_y_addr_upper;
  u64_addr_upper_t rec_uv_addr_upper;
  u64_addr_upper_t input_y_addr_upper;
  u64_addr_upper_t input_uv_addr_upper;
  u64_addr_upper_t input_me_addr_upper;
  u64_addr_upper_t mvdump_addr_upper;
#endif
  uint32_t mvdump_addr;
} enc_buffer_status_msg_t;

#if 0
typedef enum {
  VDSP_ERR_CODE_NO_ERROR = 0x0,
  VDSP_ERR_VDSP_SYNC_CMD_UPDATE_FAIL = 0x1,
} vdsp_err_code_t;
#else
#define VDSP_ERR_CODE_NO_ERROR 0x0u
#define VDSP_ERR_VDSP_SYNC_CMD_UPDATE_FAIL 0x1u
#define VDSP_ERR_DSP_SYNC_CMD_UPDATE_FAIL 0x2u
#endif

typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;

  uint32_t stream_id           :8;
  uint32_t is_drop             :1;
  uint32_t pic_type            :4;    // frame_type_t
  uint32_t is_long_term_flag   :1;
  uint32_t vdsp_err_code       :4;
  uint32_t is_passive_drop     :1;
  uint32_t reserved            :13;

  uint32_t frame_buf_base_y;
  uint32_t frame_buf_base_uv;
  uint32_t hw_pts;

  uint32_t me0_buf_base;
  uint32_t me1_buf_base;

  uint16_t me0_buf_pitch;
  uint16_t me1_buf_pitch;
  uint16_t frame_buf_pitch;
  uint16_t reserved1;

  uint64_t pic_frame_no;

#ifndef PROJECT_CV5
  u64_addr_upper_t frame_buf_base_y_upper;
  u64_addr_upper_t frame_buf_base_uv_upper;
  u64_addr_upper_t me0_buf_base_upper;
  u64_addr_upper_t me1_buf_base_upper;
#endif

  uint32_t cap_seq_no;
}  enc_vdsp_info_msg_t;

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
  uint32_t code_checksum[2];    // [frame_no][index]
  uint32_t rsvd[2]; // padding to 128 bytes
} test_binary_header_t;


#define ENC_TEST_HEADER_OFFSET      0u
#define ENC_TEST_CMD_OFFSET     128u
#define ENC_TEST_LUMA0_OFFSET       256u
#define ENC_TEST_CHRM0_OFFSET       110848u
#define ENC_TEST_ME0_OFFSET     166144u
#define ENC_TEST_LUMA1_OFFSET       175360u
#define ENC_TEST_CHRM1_OFFSET       285952u
#define ENC_TEST_ME1_OFFSET     341248u

#define ENC_TEST_BINARY_SIZE        350464u

/////////////////////////////////////////////////////////////////////////////
typedef struct
{
  uint32_t msg_code;
  uint32_t msg_crc;
  uint32_t time_code;
  uint32_t latest_clock_counter;

  uint8_t  decoder_id;
  uint8_t  codec_type; // 0: avc;1:hevc;2:jpeg
  uint8_t  reserved[2];
} msg_dec_hdr_t;

// (msg code 0x87000001)
typedef uint8_t dec_op_mode_t;
#define DEC_OPM_INVALID          0u
#define DEC_OPM_IDLE             1u
#define DEC_OPM_RUN              2u
#define DEC_OPM_VDEC_IDLE        3u
#define DEC_OPM_RUN_2_IDLE       4u
#define DEC_OPM_RUN_2_VDEC_IDLE  5u

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
#ifndef PROJECT_CV5
  u64_addr_upper_t bits_fifo_next_rptr_upper;
#endif
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


////////////////////////////////////////////////////////////////////////////////
// msg between idsp and VP
#define MAX_ROI_CMD_TAG_NUM   8u
#define MAX_TOKEN_ARRAY       4u

typedef struct
{
  uint32_t fov_id;
  uint32_t fov_private_info_addr;   // this field points to a user defined private info structure.

#ifndef PROJECT_CV5
  u64_addr_upper_t fov_private_info_addr_upper;
#endif
} sideband_info_t;

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


#endif // CMD_MSG_DSP_H_
