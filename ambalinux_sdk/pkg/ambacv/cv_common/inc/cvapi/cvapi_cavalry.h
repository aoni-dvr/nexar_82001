/*
 * Copyright (c) 2017-2019 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVAPI_CAVALRY_H_FILE
#define CVAPI_CAVALRY_H_FILE

/*================================================================================================*/
/* Directly imported from cavalry_ucode.h                                                         */
/*                                                                                                */
/* File source : "cavalry_ucode.h" from Cavalry implementation provided by zkyang on 2019-09-10   */
/* File size   : 8382 bytes                                                                       */
/* File md5sum : a4c815c0e6cf7b3984be6c3d7aee8ebc                                                 */
/*                                                                                                */
/* Changes     : Some of these definitions overlap with the cavalry_ioctl.h, these have to be     */
/*               removed to allow this to compile without getting redefinition errors.  Since we  */
/*               have control of this file, but not cavalry_ioctl.h, if both files are included,  */
/*               then cavalry_ioctl.h has to come before cvapi_cavalry.h                          */
/*                                                                                                */
/* Additionally: These now use "typedef struct" to fix some specific MISRA-2012 warnings/errors.  */
/*               * Dir 4.8  : "struct" must be obscured if not referenced.                        */
/*               * Rule 2.4 : Project cannot contain unused tag definitions.                      */
/*================================================================================================*/

#define MAX_PORT_CNT	(128U)
#define MAX_POKE_CNT	(64U)
#define MAX_DAG_CNT	    (256U)
#define MSG_SIZE	    (128U)
#define VERSION_INFO_OFFSET	(0x40U)
#define CAVALRY_HARRIS_H_BLOCKS	(8U)
#define CAVALRY_HARRIS_V_BLOCKS	(8U)
#define CAVALRY_HARRIS_MAX_POINTS_PER_BLOCK	(32U)

#ifndef __CAVALRY_IOCTL_H__ /* This is already defined in cavalry_ioctl.h =---------vvv-*/
typedef enum {
	MSG_RVAL_NONE,
	MSG_RVAL_INVALID_CMD,
	MSG_RVAL_INVALID_DAGCNT,
	MSG_RVAL_INVALID_DAGDESC,
	MSG_RVAL_INVALID_SLOT_ID,
	MSG_RVAL_FEX_INSUFFICIENT_DRAM,
	MSG_RVAL_FMA_INSUFFICIENT_DRAM,
	MSG_RVAL_VP_RESERVED_VMEM_TRASHED
} msg_rval_t;

typedef enum {
	CHIP_TYPE_CV22 = 0U,
	CHIP_TYPE_CV2 = 1U,
	CHIP_TYPE_CV25 = 2U
} chip_type_t;

typedef enum {
	HOTLINK_SLOT_0 = 0U,
	HOTLINK_SLOT_1 = 1U,
	HOTLINK_SLOT_2 = 2U,
	HOTLINK_SLOT_3 = 3U,
	HOTLINK_SLOT_NUM = 4U,
	HOTLINK_SLOT_FIRST = HOTLINK_SLOT_0,
	HOTLINK_SLOT_LAST = HOTLINK_SLOT_3,
	HOTLINK_SLOT_CAVALRY_FRAMEWORK = 0xFFU
} hotlink_slot_t;

typedef enum {
	CAVALRY_LOG_LEVEL_SILENT = 0U,
	CAVALRY_LOG_LEVEL_MINIMAL,
	CAVALRY_LOG_LEVEL_NORMAL,
	CAVALRY_LOG_LEVEL_VERBOSE,
	CAVALRY_LOG_LEVEL_DEBUG,
	CAVALRY_LOG_LEVEL_NUM,
} cavalry_log_level_t;
#define CAVALRY_LOG_LEVEL_FIRST       CAVALRY_LOG_LEVEL_SILENT
#define CAVALRY_LOG_LEVEL_LAST        CAVALRY_LOG_LEVEL_DEBUG

typedef enum {
	CAVALRY_FMA_MODE_TEMPORAL = 0U,
	CAVALRY_FMA_MODE_STEREO,
	CAVALRY_FMA_MODE_NUM,
} cavalry_fma_mode_t;
#define CAVALRY_FMA_MODE_FIRST        CAVALRY_FMA_MODE_TEMPORAL
#define CAVALRY_FMA_MODE_LAST         CAVALRY_FMA_MODE_STEREO

typedef struct {
	uint32_t chip;
	uint32_t ucode_version;
	uint32_t build_date;
	uint32_t git_hash;
	uint32_t hotlink_fw_version;
} version_info_t;
#endif /* ?__CAVALRY_IOCTL_H__ =----------------------------------------------------^^^-*/

typedef struct {
	uint32_t port_dram_addr;
	uint32_t port_boffset_in_dag;
	uint32_t port_bsize;
	uint32_t port_daddr_increment;
} port_desc_t;

typedef struct {
	uint32_t poke_val;
	uint32_t poke_vaddr;
	uint32_t poke_bsize;
} poke_desc_t;

typedef struct {
	uint32_t dvi_mode : 1;
	uint32_t use_ping_pong_vmem : 1;
	uint32_t reserved : 14;
	uint32_t dag_loop_cnt : 16;
	uint32_t dvi_dram_addr;
	uint32_t dvi_img_vaddr;
	uint32_t dvi_img_size;
	uint32_t dvi_dag_vaddr;

	uint32_t reserved1[4];

	uint32_t port_cnt;
	uint32_t poke_cnt;
	port_desc_t port_desc[MAX_PORT_CNT];
	poke_desc_t poke_desc[MAX_POKE_CNT];
} dag_desc_t;

typedef struct {
	uint32_t cmd_code;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 1U];
} cavalry_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 1U];
} cavalry_msg_t;

#define DAG_RUN_CMD	(0x00000001U)
typedef struct {
	uint32_t cmd_code;
	uint32_t dag_cnt;
	dag_desc_t dag_desc[MAX_DAG_CNT];
} dag_run_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t start_tick;
	uint32_t end_tick;
	uint32_t finish_dags;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 5U];
} dag_run_msg_t;

#define STOP_CMD	(0x00000002U)
typedef struct {
	uint32_t cmd_code;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 1U];
} stop_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 1U];
} stop_msg_t;

#define HOTLINK_RUN_CMD	(0x00000003U)
typedef struct {
	uint32_t cmd_code;
	uint8_t slot_id;
	uint8_t reserved0[3];
	uint32_t arg_daddr;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 3U];
} hotlink_run_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t slot_rval;
	uint32_t start_tick;
	uint32_t end_tick;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 5U];
} hotlink_run_msg_t;

#define FEX_QUERY_CMD	(0x00000004U)
typedef struct {
	uint32_t cmd_code;
	uint32_t img_width          :16;
	uint32_t img_height         :16;
	uint32_t img_pitch          :16;
	uint32_t harris_en          : 2;
	uint32_t stereo_en          : 1;
	uint32_t dump_harris_score  : 1;
	uint32_t reserved1          :12;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 3U];
} fex_query_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t dram_required;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 3U];
} fex_query_msg_t;

#define FEX_RUN_CMD	(0x00000005U)
#ifndef __CAVALRY_IOCTL_H__ /* This is already defined in cavalry_ioctl.h =---------vvv-*/
typedef enum {
	FEX_CFG_MASK_NMS_THRESHOLD = (1U << 0U),
	FEX_CFG_MASK_NMS_WINDOW_FOR_HARRIS = (1U << 1U),
	FEX_CFG_MASK_ALL = ((uint32_t)FEX_CFG_MASK_NMS_THRESHOLD | (uint32_t)FEX_CFG_MASK_NMS_WINDOW_FOR_HARRIS)
} fex_cfg_mask_t;

typedef enum {
	STEREO_PROFILE_DEFAULT = 0,
	STEREO_PROFILE_1,
	STEREO_PROFILE_NUM
} stereo_profile_t;
#endif /* ?__CAVALRY_IOCTL_H__ =----------------------------------------------------^^^-*/

typedef struct {
	uint32_t cfg_mask;
	uint32_t nms_threshold          :16;
	uint32_t nms_window_for_harris  : 4;
	uint32_t reserved2              :12;
} fex_user_cfg_t;

typedef struct {
	uint32_t cmd_code;
	uint32_t img_width          :16;
	uint32_t img_height         :16;
	uint32_t img_pitch          :16;
	uint32_t harris_en          : 2;
	uint32_t stereo_en          : 1;
	uint32_t stereo_profile     : 5;
	uint32_t dump_harris_score  : 1;
	uint32_t reserved1          : 7;
	fex_user_cfg_t fex_cfg;
	uint32_t output_daddr;
	uint32_t output_size;
	uint32_t luma_daddr[2];
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - (7U + (sizeof(fex_user_cfg_t) / sizeof(uint32_t)))];
} fex_run_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t disparity_daddr;
	uint32_t disparity_size;
	uint32_t invalid_disparities;
	uint16_t disparity_width;
	uint16_t disparity_height;
	uint16_t disparity_pitch;
	uint16_t reserved1;
	uint32_t harris_count_daddr[2];
	uint32_t harris_count_size[2];
	uint32_t harris_point_daddr[2];
	uint32_t harris_point_size[2];
	uint32_t brief_descriptor_daddr[2];
	uint32_t brief_descriptor_size[2];
	uint32_t start_tick;
	uint32_t end_tick;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 21U];
} fex_run_msg_t;

#define FMA_QUERY_CMD	(0x00000006U)
typedef struct {
	uint32_t cmd_code;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 1U];
} fma_query_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t dram_required;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 3U];
} fma_query_msg_t;

#define FMA_RUN_CMD	(0x00000007U)
#ifndef __CAVALRY_IOCTL_H__ /* This is already defined in cavalry_ioctl.h =---------vvv-*/
typedef enum {
	FMA_CFG_MASK_MIN_THRESHOLD = (1U << 0U),
	FMA_CFG_MASK_RATIO_THRESHOLD = (1U << 1U),
	FMA_CFG_MASK_WIN_WIDTH = (1U << 2U),
	FMA_CFG_MASK_WIN_HEIGHT = (1U << 3U),
	FMA_CFG_MASK_X_THRESHOLD_R = (1U << 4U),
	FMA_CFG_MASK_X_THRESHOLD_L = (1U << 5U),
	FMA_CFG_MASK_Y_THRESHOLD_U = (1U << 6U),
	FMA_CFG_MASK_Y_THRESHOLD_D = (1U << 7U),
	FMA_CFG_MASK_ALL = ((uint32_t)FMA_CFG_MASK_MIN_THRESHOLD | (uint32_t)FMA_CFG_MASK_RATIO_THRESHOLD |
		(uint32_t)FMA_CFG_MASK_WIN_WIDTH | (uint32_t)FMA_CFG_MASK_WIN_HEIGHT |
		(uint32_t)FMA_CFG_MASK_X_THRESHOLD_R | (uint32_t)FMA_CFG_MASK_X_THRESHOLD_L |
		(uint32_t)FMA_CFG_MASK_Y_THRESHOLD_U | (uint32_t)FMA_CFG_MASK_Y_THRESHOLD_D)
} fma_cfg_mask_t;

#endif /* ?__CAVALRY_IOCTL_H__ =----------------------------------------------------^^^-*/

typedef struct {
	uint32_t cfg_mask;
	uint32_t min_threshold;

	uint16_t ratio_threshold;
	uint16_t reserved;

	uint16_t win_width;
	uint16_t win_height;

	uint16_t x_threshold_r;
	uint16_t x_threshold_l;
	uint16_t y_threshold_u;
	uint16_t y_threshold_d;
} fma_user_cfg_t;

typedef struct {
	uint32_t cmd_code;

	uint32_t output_daddr;
	uint32_t output_size;

	uint32_t target_coord_daddr;
	uint32_t target_descriptor_daddr;

	uint32_t reference_coord_daddr;
	uint32_t reference_descriptor_daddr;

	uint32_t mode : 1;	// 0 for temporal, 1 for stereo
	uint32_t reserved1 : 31;

	fma_user_cfg_t stereo_cfg;
	fma_user_cfg_t temporal_cfg;

	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - (8U + ((2U * sizeof(fma_user_cfg_t)) / sizeof(uint32_t)))];
} fma_run_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t result_score_daddr;
	uint32_t result_score_size;

	uint32_t result_index_daddr;
	uint32_t result_index_size;

	uint32_t temporal_coord_daddr;
	uint32_t temporal_coord_size;

	uint32_t start_tick;
	uint32_t end_tick;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 10U];
} fma_run_msg_t;

#define SET_LOG_LEVEL_CMD	(0x00000008U)
typedef struct {
	uint32_t cmd_code;
	uint32_t log_level;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 2U];
} set_log_level_cmd_t;

typedef struct {
	uint32_t msg_code;
	uint32_t rval;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 2U];
} set_log_level_msg_t;

typedef struct {
	uint32_t base_daddr;
	uint32_t cmd_q_daddr;
	uint32_t cmd_q_size;
	uint32_t msg_q_daddr;
	uint32_t msg_q_size;
	uint32_t log_q_daddr;
	uint32_t log_q_size;
	uint32_t hotlink_slot0_offset;
	uint32_t hotlink_slot1_offset;
	uint32_t hotlink_slot2_offset;
	uint32_t hotlink_slot3_offset;
	uint8_t log_level;
	uint8_t reserved0[3];
	uint32_t cmd_q_fex_daddr;
	uint32_t cmd_q_fex_size;
	uint32_t msg_q_fex_daddr;
	uint32_t msg_q_fex_size;
	uint32_t cmd_q_fma_daddr;
	uint32_t cmd_q_fma_size;
	uint32_t msg_q_fma_daddr;
	uint32_t msg_q_fma_size;
	uint32_t reserved[(MSG_SIZE / sizeof(uint32_t)) - 20U];
} cavalry_init_data_t;

#ifndef __CAVALRY_IOCTL_H__ /* This is already defined in cavalry_ioctl.h =---------vvv-*/
typedef struct {
	uint32_t	seq_num;
	uint8_t	thread_id;
	uint8_t slot_id;
	uint8_t	reserved[2];
	uint32_t	format_offset;
	uint32_t	arg1;
	uint32_t	arg2;
	uint32_t	arg3;
	uint32_t	arg4;
	uint32_t	arg5;
} cavalry_log_entry_t;

#endif /* ?__CAVALRY_IOCTL_H__ =----------------------------------------------------^^^-*/
/*================================================================================================*/
/* CVFLOW specific : start                                                                        */
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*-= ARM-side app implementation =------------------------------------------------------*/
typedef struct {
	uint32_t rval;
	uint32_t start_tick;
	uint32_t end_tick;
	uint32_t finish_dags;
	uint32_t dag_cnt;
	dag_desc_t dag_desc[MAX_DAG_CNT];
} cavalry_run_dags_t;

/*------------------------------------------------------------------------------------------------*/
/*-= Cavalry system related =---------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
#define MAX_CAVALRY_SLOTS               (16U)
#define CAVALRY_SLOT_SIZE               (((sizeof(cavalry_run_dags_t) + 32U) + 255U) & 0xFFFFFF00U)
#define CAVALRY_VERSION                 0x1U

/* profiler header type for Cavalry (ambaprof_header_t.version >= 2) : */
typedef struct
{
  uint32_t    cavalry_pid;
  uint32_t    reserved[7];
  uint8_t     process_name[256U - (8U * sizeof(uint32_t))];
} ambaprof_cavalry_header_t;

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/* CVFLOW specific : end                                                                          */
/*================================================================================================*/

#endif /* ?CVAPI_CAVALRY_H_FILE */

