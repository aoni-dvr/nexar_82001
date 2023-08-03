/*
 * Copyright (c) 2018-2018 Ambarella, Inc.
 * 2018/06/01 - [Zhikan Yang] created file
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
#ifndef SDK_CAVALRY_IOCTL_H
#define SDK_CAVALRY_IOCTL_H

#if !defined(CONFIG_BUILD_CV_THREADX)

#ifndef CAVALRY_IOCTL_H
#define CAVALRY_IOCTL_H
#if defined(__QNXNTO__)
#include <sys/ioctl.h>
#else
#include <linux/ioctl.h>
#endif
#include <cavalry/config.h>

#if defined (BUILD_AMBARELLA_AMBACV_DRV) && defined (BUILD_AMBARELLA_CAVALRY_DRV)
#error "Can not enable ambacv and cavalry at the same time"
#elif defined (BUILD_AMBARELLA_AMBACV_DRV)
#define CAVALRY_DEV_NODE        "/dev/ambacv"
#else
#define CAVALRY_DEV_NODE        "/dev/cavalry"
#endif

#define MAX_PORT_CNT    (128U)
#define MAX_POKE_CNT    (64U)
#define MAX_DAG_CNT         (256U)

#define VERSION_INFO_OFFSET     (0x40U)

#define CAVALRY_SLOT_ID_OFFSET  (0x04)

#define CAVALRY_HARRIS_H_BLOCKS (8U)
#define CAVALRY_HARRIS_V_BLOCKS (8U)
#define CAVALRY_HARRIS_MAX_POINTS_PER_BLOCK     (32U)

#define FEX_MIN_NMS_WINDOW (1)
#define FEX_MAX_NMS_WINDOW (8)

#define DEFAULT_AUDIO_CLK_HZ (12288000)

typedef enum {
    CHIP_TYPE_CV22 = 0,
    CHIP_TYPE_CV2 = 1,
    CHIP_TYPE_CV25 = 2,
} chip_type_t;

typedef enum {
    CAVALRY_LOG_LEVEL_SILENT = 0,
    CAVALRY_LOG_LEVEL_MINIMAL,
    CAVALRY_LOG_LEVEL_NORMAL,
    CAVALRY_LOG_LEVEL_VERBOSE,
    CAVALRY_LOG_LEVEL_DEBUG,
    CAVALRY_LOG_LEVEL_NUM,
    CAVALRY_LOG_LEVEL_FIRST = CAVALRY_LOG_LEVEL_SILENT,
    CAVALRY_LOG_LEVEL_LAST = CAVALRY_LOG_LEVEL_DEBUG,
} cavalry_log_level_t;

struct cavalry_set_log_level {
    cavalry_log_level_t log_level;
    uint32_t rval;
};

struct version_info_s {
    uint32_t chip;
    uint32_t ucode_version;
    uint32_t build_date;
    uint32_t git_hash;
    uint32_t hotlink_fw_version;
};

typedef enum {
    CAVALRY_MEM_ALL = 0x00,
    CAVALRY_MEM_CMD = 0x01,
    CAVALRY_MEM_MSG = 0x02,
    CAVALRY_MEM_LOG = 0x03,
    CAVALRY_MEM_CMD_FEX = 0x04,
    CAVALRY_MEM_MSG_FEX = 0x05,
    CAVALRY_MEM_CMD_FMA = 0x06,
    CAVALRY_MEM_MSG_FMA = 0x07,
    CAVALRY_MEM_UCODE = 0x08,
    CAVALRY_MEM_HOTLINK_SLOT0 = 0x09,
    CAVALRY_MEM_HOTLINK_SLOT1 = 0x0A,
    CAVALRY_MEM_HOTLINK_SLOT2 = 0x0B,
    CAVALRY_MEM_HOTLINK_SLOT3 = 0x0C,
    CAVALRY_MEM_USER = 0x0D,
    CAVALRY_MEM_NUM = 0x0E,
    CAVALRY_MEM_FIRST = CAVALRY_MEM_ALL,
    CAVALRY_MEM_LAST = CAVALRY_MEM_USER,
} CAVALRY_MEM;

struct cavalry_log_entry {
    uint32_t        seq_num;
    uint8_t thread_id;
    uint8_t slot_id;
    uint8_t reserved[2];
    uint32_t        format_offset;
    uint32_t        arg1;
    uint32_t        arg2;
    uint32_t        arg3;
    uint32_t        arg4;
    uint32_t        arg5;
};

struct cavalry_querybuf {
    CAVALRY_MEM buf;
    unsigned long length;
    unsigned long offset;
};

struct cavalry_port_desc {
    uint32_t port_dram_addr;
    uint32_t port_boffset_in_dag;
    uint32_t port_bsize;
    uint32_t port_daddr_increment;
};

struct cavalry_poke_desc {
    uint32_t poke_val;
    uint32_t poke_vaddr;
    uint32_t poke_bsize;
};

struct cavalry_dag_desc {
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
    struct cavalry_port_desc port_desc[MAX_PORT_CNT];
    struct cavalry_poke_desc poke_desc[MAX_POKE_CNT];
};

typedef enum {
    MSG_RVAL_NONE,
    MSG_RVAL_INVALID_CMD,
    MSG_RVAL_INVALID_DAGCNT,
    MSG_RVAL_INVALID_DAGDESC,
    MSG_RVAL_INVALID_SLOT_ID,
    MSG_RVAL_FEX_INSUFFICIENT_DRAM,
    MSG_RVAL_FMA_INSUFFICIENT_DRAM,
    MSG_RVAL_VP_RESERVED_VMEM_TRASHED,
} cavalry_msg_rval_t;

typedef enum {
    HOTLINK_SLOT_0 = 0,
    HOTLINK_SLOT_1 = 1,
    HOTLINK_SLOT_2 = 2,
    HOTLINK_SLOT_3 = 3,
    HOTLINK_SLOT_NUM = 4,
    HOTLINK_SLOT_FIRST = HOTLINK_SLOT_0,
    HOTLINK_SLOT_LAST = HOTLINK_SLOT_3,
    HOTLINK_SLOT_CAVALRY_FRAMEWORK = 0xFF,
} hotlink_slot_t;

typedef enum {
    CAVALRY_FMA_MODE_TEMPORAL = 0,
    CAVALRY_FMA_MODE_STEREO,
    CAVALRY_FMA_MODE_NUM,
    CAVALRY_FMA_MODE_FIRST = CAVALRY_FMA_MODE_TEMPORAL,
    CAVALRY_FMA_MODE_LAST = CAVALRY_FMA_MODE_STEREO,
} cavalry_fma_mode_t;

struct cavalry_run_dags {
    uint32_t rval;
    uint32_t start_tick;
    uint32_t end_tick;
    uint32_t finish_dags;
    uint32_t dag_cnt;
    struct cavalry_dag_desc dag_desc[MAX_DAG_CNT];
};

struct cavalry_early_quit {
    uint32_t early_quit_all : 1;
    uint32_t reserved : 31;
};

struct cavalry_mem {
    uint32_t cache_en : 1; /* 0: nocached; 1: cached */
    uint32_t reserved : 31;

    unsigned long length; /* as input: the mem part size to be allocated
                                as return: the real size of the allocated mem part */
    unsigned long offset; /* the physical address of the allocated mem part */
};

struct cavalry_cache_mem {
    uint32_t clean : 1;  /* do after arm write: cache -> dram */
    uint32_t invalid : 1; /* do before arm read: dram -> cache */
    uint32_t reserved : 30;

    unsigned long length;

    unsigned long offset; /* the physical address of the allocated mem part */
};

struct cavalry_usage_mem {
    unsigned long used_length;
    unsigned long free_length;
};

struct cavalry_run_hotlink_slot {
    uint8_t slot_id;
    uint8_t reserved0[3];
    uint32_t arg_daddr;
    uint32_t slot_rval;
    uint32_t start_tick;
    uint32_t end_tick;
};

struct cavalry_fex_query {
    /* Input */
    uint16_t img_width;
    uint16_t img_height;
    uint16_t img_pitch;
    uint16_t harris_en : 2;
    uint16_t stereo_en : 1;
    uint16_t dump_harris_score : 1;
    uint16_t reserved1 : 12;
    /* Output */
    uint32_t dram_required;
};

typedef enum {
    FEX_CFG_MASK_NMS_THRESHOLD = (1 << 0),
    FEX_CFG_MASK_NMS_WINDOW_FOR_HARRIS = (1 << 1),
    FEX_CFG_MASK_ALL = FEX_CFG_MASK_NMS_THRESHOLD | FEX_CFG_MASK_NMS_WINDOW_FOR_HARRIS,
} fex_cfg_mask_t;

struct fex_user_cfg {
    uint32_t cfg_mask;
    uint16_t nms_threshold;
    uint16_t nms_window_for_harris : 4;
    uint16_t reserved2 : 12;
};

typedef enum {
    STEREO_PROFILE_DEFAULT = 0,
    STEREO_PROFILE_1,
    STEREO_PROFILE_NUM,
} stereo_profile_t;

struct cavalry_fex_run {
    /* Input */
    uint16_t img_width;
    uint16_t img_height;
    uint16_t img_pitch;
    uint16_t harris_en : 2;
    uint16_t stereo_en : 1;
    uint16_t stereo_profile : 5;
    uint16_t dump_harris_score : 1;
    uint16_t reserved1 : 7;
    struct fex_user_cfg fex_cfg;
    uint32_t output_daddr;
    uint32_t output_size;
    uint32_t luma_daddr[2];

    /* Output */
    uint32_t rval;
    uint32_t disparity_daddr;
    uint32_t disparity_size;
    uint32_t invalid_disparities;
    uint16_t disparity_width;
    uint16_t disparity_height;
    uint16_t disparity_pitch;
    uint16_t reserved2;
    uint32_t harris_count_daddr[2];
    uint32_t harris_count_size[2];
    uint32_t harris_point_daddr[2];
    uint32_t harris_point_size[2];
    uint32_t brief_descriptor_daddr[2];
    uint32_t brief_descriptor_size[2];
    uint32_t start_tick;
    uint32_t end_tick;
};

struct cavalry_fma_query {
    /* Output */
    uint32_t dram_required;
};

typedef enum {
    FMA_CFG_MASK_MIN_THRESHOLD = (1 << 0),
    FMA_CFG_MASK_RATIO_THRESHOLD = (1 << 1),
    FMA_CFG_MASK_WIN_WIDTH = (1 << 2),
    FMA_CFG_MASK_WIN_HEIGHT = (1 << 3),
    FMA_CFG_MASK_X_THRESHOLD_R = (1 << 4),
    FMA_CFG_MASK_X_THRESHOLD_L = (1 << 5),
    FMA_CFG_MASK_Y_THRESHOLD_U = (1 << 6),
    FMA_CFG_MASK_Y_THRESHOLD_D = (1 << 7),
    FMA_CFG_MASK_ALL = FMA_CFG_MASK_MIN_THRESHOLD | FMA_CFG_MASK_RATIO_THRESHOLD |
                       FMA_CFG_MASK_WIN_WIDTH | FMA_CFG_MASK_WIN_HEIGHT |
                       FMA_CFG_MASK_X_THRESHOLD_R | FMA_CFG_MASK_X_THRESHOLD_L |
                       FMA_CFG_MASK_Y_THRESHOLD_U | FMA_CFG_MASK_Y_THRESHOLD_D,
} fma_cfg_mask_t;

struct fma_user_cfg {
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
};

struct cavalry_fma_run {
    /* Input */
    uint32_t output_daddr;
    uint32_t output_size;
    uint32_t target_coord_daddr;
    uint32_t target_descriptor_daddr;
    uint32_t reference_coord_daddr;
    uint32_t reference_descriptor_daddr;
    uint32_t mode : 1;      // 0 for temporal, 1 for stereo
    uint32_t reserved1 : 31;
    struct fma_user_cfg stereo_cfg;
    struct fma_user_cfg temporal_cfg;

    /* Output */
    uint32_t rval;
    uint32_t result_score_daddr;
    uint32_t result_score_size;
    uint32_t result_index_daddr;
    uint32_t result_index_size;
    uint32_t temporal_coord_daddr;
    uint32_t temporal_coord_size;
    uint32_t start_tick;
    uint32_t end_tick;
};

struct cavalry_stats_get {
    uint32_t is_vp_idle : 1;
    uint32_t reserved : 31;
    int vp_current_pid;
};

#define CAVALRY_QUERY_BUF               _IOWR ('C', 0x0, struct cavalry_querybuf *)
#define CAVALRY_START_VP                _IOWR ('C', 0x1, void *)
#define CAVALRY_STOP_VP                 _IOWR ('C', 0x2, void *)
#define CAVALRY_RUN_DAGS                _IOWR ('C', 0x3, struct cavalry_run_dags *)
#define CAVALRY_START_LOG               _IOWR ('C', 0x4, void *)
#define CAVALRY_STOP_LOG                _IOWR ('C', 0x5, void *)
#define CAVALRY_EARLY_QUIT      _IOWR ('C', 0x6, struct cavalry_early_quit *)

/* cv user memory ioctl */
#define CAVALRY_ALLOC_MEM               _IOWR ('C', 0x7, struct cavalry_mem *)
#define CAVALRY_FREE_MEM                _IOWR ('C', 0x8, struct cavalry_mem *)
#define CAVALRY_SYNC_CACHE_MEM  _IOWR ('C', 0x9, struct cavalry_cache_mem *)
#define CAVALRY_GET_USAGE_MEM   _IOWR ('C', 0xA, struct cavalry_usage_mem *)

#define CAVALRY_RUN_HOTLINK_SLOT        _IOWR ('C', 0xB, struct cavalry_run_hotlink_slot *)
#define CAVALRY_SET_HOTLINK_SLOT_CFG    _IOWR ('C', 0xC, uint32_t *)
#define CAVALRY_GET_HOTLINK_SLOT_CFG    _IOWR ('C', 0xD, uint32_t *)

#define CAVALRY_FEX_QUERY       _IOWR ('C', 0xE, struct cavalry_fex_query *)
#define CAVALRY_FEX_RUN _IOWR ('C', 0xF, struct cavalry_fex_run *)

#define CAVALRY_FMA_QUERY       _IOWR ('C', 0x10, struct cavalry_fma_query *)
#define CAVALRY_FMA_RUN _IOWR ('C', 0x11, struct cavalry_fma_run *)

#define CAVALRY_SET_LOG_LEVEL   _IOWR ('C', 0x12, struct cavalry_set_log_level *)

#define CAVALRY_GET_STATS       _IOWR ('C', 0x13, struct cavalry_stats_get *)

/* misc ioctl */
#define CAVALRY_GET_AUDIO_CLK   _IOR ('C', 0x80, uint64_t *)
#endif //__CAVALRY_IOCTL_H__

#else
#define CAVALRY_QUERY_BUF                   0x100U
#define CAVALRY_START_VP                    0x101U
#define CAVALRY_STOP_VP                     0x102U
#define CAVALRY_RUN_DAGS                    0x103U
#define CAVALRY_START_LOG                   0x104U
#define CAVALRY_STOP_LOG                    0x105U
#define CAVALRY_EARLY_QUIT                  0x106U

/* cv user memory ioctl */
#define CAVALRY_ALLOC_MEM                   0x107U
#define CAVALRY_FREE_MEM                    0x108U
#define CAVALRY_SYNC_CACHE_MEM              0x109U
#define CAVALRY_GET_USAGE_MEM               0x10AU

#define CAVALRY_RUN_HOTLINK_SLOT            0x10BU
#define CAVALRY_SET_HOTLINK_SLOT_CFG        0x10CU
#define CAVALRY_GET_HOTLINK_SLOT_CFG        0x10DU

#define CAVALRY_FEX_QUERY                   0x10EU
#define CAVALRY_FEX_RUN                     0x10FU

#define CAVALRY_FMA_QUERY                   0x1010U
#define CAVALRY_FMA_RUN                     0x1011U

#define CAVALRY_SET_LOG_LEVEL               0x1012U

#define CAVALRY_GET_STATS                   0x1013U

/* misc ioctl */
#define CAVALRY_GET_AUDIO_CLK               0x1080U
#endif

#endif //CAVALRY_IOCTL_H
