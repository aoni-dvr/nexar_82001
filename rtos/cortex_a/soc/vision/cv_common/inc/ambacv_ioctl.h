/**
 *  @file ambacv_ioctl.h
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
 *  @details Definitions & Constants for Ambacv IOCTL
 *
 */

#ifndef  AMBACV_IOCTL_H
#define  AMBACV_IOCTL_H

#include "schdrmsg_def.h"
#include "idsp_vis_msg.h"
#include "cmd_msg_vin.h"

/**
 * CVSCHEDULER_KERNEL_VERSION
 *
 * Kernel version used between the ARM sub-schedulers and kernel driver
 * CVScheduler.
 *
 * In general this version only needs to be upgraded when minor features are
 * added, or if some of the core defines below are changed.
 *
 * CVSCHEDULER_KERNEL_VERSION   5
 *   * Adds VIN queue support
 *
 * CVSCHEDULER_KERNEL_VERSION   6
 *   * Requires matching CVSCHEDULER_INTERFACE_VERSION 8
 *   * Increases SCHDRMSG_MEM_REGION_SIZE to support
 *     ARMVIS_MSG_NUM = 1024
 */
#define CVSCHEDULER_KERNEL_VERSION      6U

#define HOTLINK_MEM_REGION_VADDR_BASE   0xFC000000U
#define HOTLINK_MEM_REGION_SIZE         0x03F00000U
#define HOTLINK_VERSION                 0x1U
#define HOTLINK_RUN_FLAG_BLOCKING       0x00000001U

#define SYSINIT_OFFSET                  0x00001000U
#define SCHDRMSG_MEM_REGION_SIZE        0x00280000U
#define VISORC_MEM_REGION_SIZE          0x00600000U
#define VP_MEM_REGION_OFFSET            0x00400000U
#define VP_MEM_REGION_SIZE              0x00200000U
#define SOD_MEM_REGION_OFFSET           0x00200000U
#define SOD_MEM_REGION_SIZE             0x00200000U

#define SCHDRMSG_REGION_SIZE            0x00100000U
#define SYSCONF_QINFO_SIZE              0x00001000U

#define CAVALRY_MEM_LOG_SIZE            0x20000U
typedef struct {
    uint64_t    base;
    uint64_t    size;
} ambacv_mem_region_t;

typedef struct {
    uint64_t    ca;
    uint64_t    pa;
    uint64_t    size;
} ambacv_att_region_t;

typedef struct {
    ambacv_mem_region_t all;
    ambacv_mem_region_t sod;
    ambacv_mem_region_t vp;
    ambacv_mem_region_t tbar;
    ambacv_mem_region_t arm;
    ambacv_mem_region_t hotlink_region;
    ambacv_mem_region_t cavalry_region;
    ambacv_mem_region_t rtos_region;
} ambacv_mem_t;

typedef struct {
    ambacv_mem_region_t all;
    ambacv_mem_region_t sod;
    ambacv_mem_region_t vp;
    ambacv_mem_region_t tbar;
    ambacv_mem_region_t arm;
    ambacv_mem_region_t hotlink_region;
    ambacv_mem_region_t cavalry_region;
    ambacv_mem_region_t rtos_region;
    ambacv_mem_region_t cma_region;
    ambacv_mem_region_t cv_region;
    ambacv_mem_region_t cv_att_region;
    ambacv_mem_region_t debug_port;
} ambacv_all_mem_t;

typedef struct {
    uint64_t version;
    uint64_t index[7];
    uint64_t value[8];
} ambacv_get_info_t;

typedef struct {
    uint64_t index;
    uint64_t value;
} ambacv_get_value_t;

typedef struct {
#define SOD_SCHDR_LOG_INDEX  0U
#define SOD_CVTASK_LOG_INDEX 1U
#define VP_SCHDR_LOG_INDEX   2U
#define VP_CVTASK_LOG_INDEX  3U
    ambacv_mem_region_t schdr_log[4];
    char binary_path[256];
} ambacv_log_t;

typedef struct {
    uint64_t  buffer_vaddr;
    uint64_t  buffer_daddr;
    uint64_t  buffer_size;  /* Max buffer size: (2048MB - 1) */
    uint32_t  buffer_cacheable;
} flexidag_mem_t;

typedef struct {
    uint64_t handle;
    uint32_t slot;
    uint32_t is_blocking;
    uint32_t token_id;
    uint32_t query_ret;
} flexidag_ioctl_create_t;

typedef struct {
    uint64_t handle;
    uint32_t id;
    uint32_t value;
} flexidag_ioctl_param_t;

typedef struct {
    uint64_t handle;
    flexidag_mem_t mem_blk;
} flexidag_ioctl_memblk_t;

typedef struct {
    uint64_t handle;
    flexidag_mem_t mem_blk;
    flexidag_memory_requirements_t mem_req;
    uint32_t is_blocking;
    uint32_t token_id;
    uint32_t query_ret;
} flexidag_ioctl_open_t;

typedef struct {
    uint64_t handle;
    uint32_t is_blocking;
    uint32_t token_id;
    uint32_t query_ret;
} flexidag_ioctl_handle_t;

typedef struct {
    uint64_t handle;
    uint32_t token_id;
} flexidag_ioctl_pre_run_t;

typedef struct {
    uint64_t handle;
    uint32_t num;
    flexidag_mem_t mem_blk;
    uint64_t orig_memBlk;
} flexidag_ioctl_io_memblk_t;

typedef struct {
    uint64_t handle;
    flexidag_runinfo_t info;
    uint32_t is_blocking;
    uint32_t token_id;
    uint32_t query_ret;
} flexidag_ioctl_run_t;

typedef struct {
    uint32_t msg_code;
#ifdef CHIP_CV6               // or any chip with 40-bit support
    uint32_t msg_payload[63];
#else
    uint32_t msg_payload[31];
#endif
} ambacv_vinvis_msg_t;

typedef struct {
    uint32_t msg_size;
    idsp_vis_picinfo_t msg;
} ambacv_idspvis_msg_t;

typedef struct {
    uint32_t         cmd_size;
    dsp_cmd_t        cmd[32];
} ambacv_asynvis_msg_t;

typedef struct {
    uint64_t addr[8];
} ambacv_cfg_addr_t;

#define ECAVALRY_INVALID_SLOT_ID        300000U
#define ECAVALRY_UNABLE_TO_FIND_SLOT    300001U
#define ECAVALRY_UNABLE_TO_FIND_TOKEN   300002U

/* AMBACV_GET_VALUE ID */
#define AMBACV_ID_GET_SCHDR_TRACE                0U
#define AMBACV_ID_GET_FLEXIDAG_TRACE             1U
#define AMBACV_ID_GET_CAVALRY_TRACE              2U
#define AMBACV_ID_GET_AUTORUN_TRACE              3U
#define AMBACV_ID_GET_ORC_PERF_WI_BASE         100U

#define AMBACV_ID_GET_AUDIO_CLOCK             1000U
#define AMBACV_ID_GET_EVENT_TIME              1100U
#define AMBACV_ID_GET_CLUSTER_ID              1200U

#define AMBACV_ID_GET_ORC_CMD_WI              9900U

#define AMBACV_ID_GET_SCHDR_TOKEN             9999U
#define AMBACV_ID_WAIT_SCHDR_TOKEN_BASE      10000U


#if !defined(CONFIG_BUILD_CV_THREADX)
#define AMBACV_GET_SYSCONFIG   _IOR  ('v', 0x0U, visorc_init_params_t)
#define AMBACV_START_VISORC    _IOW  ('v', 0x1U, visorc_init_params_t)
#define AMBACV_RECV_ORCMSG     _IOWR ('v', 0x2U, ambacv_get_value_t)
#define AMBACV_SEND_ARMMSG     _IOW  ('v', 0x3U, armvis_msg_t)
#define AMBACV_FIND_CVTASKMSG  _IOWR ('v', 0x4U, ambacv_get_value_t)
#define AMBACV_SEND_ASYNCMSG   _IOW  ('v', 0x5U, ambacv_asynvis_msg_t)
#define AMBACV_GET_MEM_LAYOUT  _IOR  ('v', 0x6U, ambacv_mem_t)
#define AMBACV_UCODE_LOADED    _IOW  ('v', 0x7U, ambacv_mem_t)
#define AMBACV_STOP_VISORC     _IO   ('v', 0x8U)
#define AMBACV_SEND_VPMSG      _IOW  ('v', 0x9U, ambacv_idspvis_msg_t)
#define AMBACV_CONFIG_IDSP     _IOW  ('v', 0xaU, ambacv_cfg_addr_t)
#define AMBACV_CACHE_CLEAN     _IOW  ('v', 0xbU, ambacv_mem_region_t)
#define AMBACV_SET_BIN_PATH    _IOW  ('v', 0xcU, ambacv_log_t)
#define AMBACV_GET_LOG_INFO    _IOR  ('v', 0xdU, ambacv_log_t)
#define AMBACV_GET_VERSION     _IOR  ('v', 0xeU, uint32_t)
#define AMBACV_CACHE_INVLD     _IOW  ('v', 0xfU, ambacv_mem_region_t)
#define AMBACV_GET_ATT         _IOR  ('v', 0x10U, ambacv_att_region_t)
#define AMBACV_GET_TIME        _IOR  ('v', 0x11U, uint32_t)
#define AMBACV_WAIT_ORCMSG     _IOR  ('v', 0x12U, uint32_t)
#define AMBACV_SEND_VINMSG     _IOW  ('v', 0x13U, ambacv_vinvis_msg_t)
#define AMBACV_CONFIG_VIN      _IOW  ('v', 0x14U, ambacv_cfg_addr_t)
#define AMBACV_GET_MAL_VERSION _IOR  ('v', 0x15U, uint32_t)

/*-= Flexidag interfaces =----------------------------------------------------*/
#define AMBACV_FLEXIDAG_ENABLE              _IOW ('v', 0x40U, uint32_t)
#define AMBACV_FLEXIDAG_CREATE              _IOWR('v', 0x41U, flexidag_ioctl_create_t)
#define AMBACV_FLEXIDAG_CREATE_QUERY        _IOWR('v', 0x42U, flexidag_ioctl_create_t)
#define AMBACV_FLEXIDAG_SET_PARAMETER       _IOW ('v', 0x43U, flexidag_ioctl_param_t)
#define AMBACV_FLEXIDAG_ADD_TBAR            _IOW ('v', 0x44U, flexidag_ioctl_memblk_t)
#define AMBACV_FLEXIDAG_ADD_SFB             _IOW ('v', 0x45U, flexidag_ioctl_memblk_t)
#define AMBACV_FLEXIDAG_OPEN                _IOWR('v', 0x46U, flexidag_ioctl_open_t)
#define AMBACV_FLEXIDAG_OPEN_QUERY          _IOWR('v', 0x47U, flexidag_ioctl_open_t)
#define AMBACV_FLEXIDAG_SET_STATE_BUFFER    _IOW ('v', 0x48U, flexidag_ioctl_memblk_t)
#define AMBACV_FLEXIDAG_SET_TEMP_BUFFER     _IOW ('v', 0x49U, flexidag_ioctl_memblk_t)
#define AMBACV_FLEXIDAG_INIT                _IOWR('v', 0x4AU, flexidag_ioctl_handle_t)
#define AMBACV_FLEXIDAG_INIT_QUERY          _IOWR('v', 0x4BU, flexidag_ioctl_handle_t)
#define AMBACV_FLEXIDAG_PREP_RUN            _IOWR('v', 0x4CU, flexidag_ioctl_pre_run_t)
#define AMBACV_FLEXIDAG_SET_INPUT_BUFFER    _IOW ('v', 0x4DU, flexidag_ioctl_io_memblk_t)
#define AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER _IOW ('v', 0x4EU, flexidag_ioctl_io_memblk_t)
#define AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER   _IOW ('v', 0x4FU, flexidag_ioctl_io_memblk_t)
#define AMBACV_FLEXIDAG_RUN                 _IOWR('v', 0x50U, flexidag_ioctl_run_t)
#define AMBACV_FLEXIDAG_RUN_QUERY           _IOWR('v', 0x51U, flexidag_ioctl_run_t)
#define AMBACV_FLEXIDAG_WAIT_RUN_FINISH     _IOWR('v', 0x52U, flexidag_ioctl_run_t)
#define AMBACV_FLEXIDAG_CLOSE               _IOWR('v', 0x53U, flexidag_ioctl_handle_t)
#define AMBACV_FLEXIDAG_CLOSE_QUERY         _IOWR('v', 0x54U, flexidag_ioctl_handle_t)
#define AMBACV_FLEXIDAG_INFO                _IOW ('v', 0x55U, flexidag_ioctl_handle_t)
#define AMBACV_FLEXIDAG_HANDLE_MSG          _IOW ('v', 0x56U, armvis_msg_t)

/*-= Cavalry interface additional controls =----------------------------------*/
#define CAVALRY_ENABLE                  _IOWR ('C', 0xF0U, int)
#define CAVALRY_ASSOCIATE_BUF           _IOWR ('C', 0xF1U, int)
#define CAVALRY_SET_MEMORY_BLOCK        _IOWR ('C', 0xF2U, armvis_msg_t)
#define CAVALRY_HANDLE_VISORC_REPLY     _IOWR ('C', 0xFFU, armvis_msg_t)

/*-= TSR Scheduler =----------------------------------------------------------*/
#define AMBACV_GET_MMB_INFO    _IOWR ('v', 0xfbU, ambacv_get_info_t)
#define AMBACV_GET_VALUE       _IOWR ('v', 0xfcU, ambacv_get_value_t)
#define AMBACV_CORE_MUTEX      _IOW  ('v', 0xfdU, uint32_t)
#define AMBACV_RESUME_VISORC   _IOR  ('v', 0xfeU, visorc_init_params_t)
#define AMBACV_GET_STATE       _IOR  ('v', 0xffU, uint32_t)
#else

#define AMBACV_GET_SYSCONFIG            0x0U
#define AMBACV_START_VISORC             0x1U
#define AMBACV_RECV_ORCMSG              0x2U
#define AMBACV_SEND_ARMMSG              0x3U
#define AMBACV_FIND_CVTASKMSG           0x4U
#define AMBACV_SEND_ASYNCMSG            0x5U
#define AMBACV_GET_MEM_LAYOUT           0x6U
#define AMBACV_UCODE_LOADED             0x7U
#define AMBACV_STOP_VISORC              0x8U
#define AMBACV_SEND_VPMSG               0x9U
#define AMBACV_CONFIG_IDSP              0xAU
#define AMBACV_CACHE_CLEAN              0xBU
#define AMBACV_SET_BIN_PATH             0xCU
#define AMBACV_GET_LOG_INFO             0xDU
#define AMBACV_GET_VERSION              0xEU
#define AMBACV_CACHE_INVLD              0xFU
#define AMBACV_GET_ATT                  0x10U
#define AMBACV_GET_TIME                 0x11U
#define AMBACV_WAIT_ORCMSG              0x12U
#define AMBACV_SEND_VINMSG              0x13U
#define AMBACV_CONFIG_VIN               0x14U
#define AMBACV_GET_MAL_VERSION          0x15U

/*-= Flexidag interfaces =----------------------------------------------------*/
#define AMBACV_FLEXIDAG_ENABLE              0x40U
#define AMBACV_FLEXIDAG_CREATE              0x41U
#define AMBACV_FLEXIDAG_CREATE_QUERY        0x42U
#define AMBACV_FLEXIDAG_SET_PARAMETER       0x43U
#define AMBACV_FLEXIDAG_ADD_TBAR            0x44U
#define AMBACV_FLEXIDAG_ADD_SFB             0x45U
#define AMBACV_FLEXIDAG_OPEN                0x46U
#define AMBACV_FLEXIDAG_OPEN_QUERY          0x47U
#define AMBACV_FLEXIDAG_SET_STATE_BUFFER    0x48U
#define AMBACV_FLEXIDAG_SET_TEMP_BUFFER     0x49U
#define AMBACV_FLEXIDAG_INIT                0x4AU
#define AMBACV_FLEXIDAG_INIT_QUERY          0x4BU
#define AMBACV_FLEXIDAG_PREP_RUN            0x4CU
#define AMBACV_FLEXIDAG_SET_INPUT_BUFFER    0x4DU
#define AMBACV_FLEXIDAG_SET_FEEDBACK_BUFFER 0x4EU
#define AMBACV_FLEXIDAG_SET_OUTPUT_BUFFER   0x4FU
#define AMBACV_FLEXIDAG_RUN                 0x50U
#define AMBACV_FLEXIDAG_RUN_QUERY           0x51U
#define AMBACV_FLEXIDAG_WAIT_RUN_FINISH     0x52U
#define AMBACV_FLEXIDAG_CLOSE               0x53U
#define AMBACV_FLEXIDAG_CLOSE_QUERY         0x54U
#define AMBACV_FLEXIDAG_INFO                0x55U
#define AMBACV_FLEXIDAG_HANDLE_MSG          0x56U

/*-= Cavalry interface additional controls =----------------------------------*/
#define CAVALRY_ENABLE                  0x10F0U
#define CAVALRY_ASSOCIATE_BUF           0x10F1U
#define CAVALRY_SET_MEMORY_BLOCK        0x10F2U
#define CAVALRY_HANDLE_VISORC_REPLY     0x10FFU

/*-= TSR Scheduler =----------------------------------------------------------*/
#define AMBACV_GET_MMB_INFO             0xFBU
#define AMBACV_GET_VALUE                0xFCU
#define AMBACV_CORE_MUTEX               0xFDU
#define AMBACV_RESUME_VISORC            0xFEU
#define AMBACV_GET_STATE                0xFFU

#endif

#endif  //AMBACV_IOCTL_H
