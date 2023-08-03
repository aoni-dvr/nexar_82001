/**
 *  @file schdr.h
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
 *  @details Definitions & Constants for Scheduler APIs
 *
 */

#ifndef SCHDR_H
#define SCHDR_H

#include "schdrmsg_def.h"
#include "schdr_api.h"
#include "cvtask_api.h"
#include "os_api.h"
#include "cvapi_flexidag.h"
#include "cvtask_ops.h"
#include "idsp_vis_msg.h"
#include "cvapi_metadata_interface.h"
#include "cvapi_metadata_interface_internal.h"
#include "cvapi_logger_interface.h"
#include "cvsched_sysflow_format.h"

#define SCHDR_PRIVATE_MEM_SIZE  4096U

#ifdef DISABLE_ARM_CVTASK
#define SCHDR_MAX_SCRATCHPAD_SIZE       0x100U

#define SCHDR_LOG_ENTRIES_ARMSCHDR      64U
#define SCHDR_LOG_ENTRIES_ARMCVTASK     64U
#define SCHDR_LOG_ENTRIES_ARMPERF       64U
#else
#define SCHDR_MAX_SCRATCHPAD_SIZE       0x20000U

#define SCHDR_LOG_ENTRIES_ARMSCHDR      1024U
#define SCHDR_LOG_ENTRIES_ARMCVTASK     8192U
#define SCHDR_LOG_ENTRIES_ARMPERF       8192U
#endif

#define SCHDR_LOG_ENTRIES_VISSCHDR      8192U
#define SCHDR_LOG_ENTRIES_VISCVTASK     8192U
#define SCHDR_LOG_ENTRIES_VISPERF       8192U

#define SCHDR_STATE_INACTIVE            0x0U
#define SCHDR_STATE_ACTIVE              0x1U
#define SCHDR_STATE_SAFE                0x2U

#define SCHDR_MODULE                    0x1U
#define FLEXIDAG_MODULE                 0x2U
#define CAVALRY_MODULE                  0x4U

typedef struct {
    uint32_t                cpu_id;
    void*                   scratchpad_base;
    uint32_t                scratchpad_size;
    schedmsg_cvtask_run_t  *run_req;
    uint32_t                flexidag_slot_id;
    uint32_t                reserved;
} cvtask_thpool_env_t;

typedef struct {
    thread_t           thread;
    tid_t              tid;
    cvtask_thpool_env_t *env;
} thpool_tdb_t;

typedef struct {
    uint32_t                state;
    uint32_t                module_init;
    visorc_init_params_t    visorc_init;
    thpool_tdb_t            tdb;
    cvtask_thpool_env_t     thenv;

    component_build_info_t* pBuildinfo;
    sysflow_entry_t*        pSysFlow;
    uint32_t                sysflow_numlines;
    uint64_t                sysflow_addr;
    cvtable_entry_t*        pCvtable;
    uint32_t                cvtable_numlines;
    cvmem_messagepool_t*    pMsgpool;
    schdr_log_info_t*       pLinfo;
    uint64_t                linfo_daddr;

    schdr_cb                app_callback;
    schdr_error_cb          error_callback;
    void*                   pVPCoredumpCallbackHandle;
    void*                   pVPCoredumpCallbackParam;
    schdr_error_cb          vpcoredump_callback;
} schdr_system_state_t;

typedef struct {
    uint32_t                cpus_map;
    uint32_t                flags;
    uint32_t                boot_cfg;
    uint32_t                cavalry_num_slots;
    uint32_t                flexidag_num_slots;
    int32_t                 verbosity;
    uint32_t                max_sp_size;
    uint32_t                internal_thread;
    uint32_t                autorun_interval;
    uint32_t                autorun_fex_cfg;
    uint32_t                log_entries_visschdr;
    uint32_t                log_entries_viscvtask;
    uint32_t                log_entries_visperf;
} schdr_system_cfg_t;

#ifndef DISABLE_ARM_CVTASK
extern char __cvtask_format_start;
extern char __cvtask_format_end;
#endif

extern schdr_system_cfg_t schdr_sys_cfg;
extern schdr_system_state_t schdr_sys_state;

extern psem_t *pStartupSemaphore;

void* get_next_entry(schdr_log_info_t *info);

uint32_t schdr_get_cluster_id(void);

cvlog_perf_entry_t* get_next_perf_entry(schdr_log_info_t *info);

void schdr_printf(const char *fmt, uint32_t v1, uint32_t v2,uint32_t v3, uint32_t v4, uint32_t v5);

uint32_t schdr_process_request(const armvis_msg_t *msg, uint32_t seqno);

cvtask_thpool_env_t* schdr_get_thenv(void);

uint32_t schdr_get_event_time(void);

uint32_t drv_schdr_find_msg(uint64_t addr, uint64_t *entry);

/* ARMVIS license related */
int32_t visorc_security_send_payload(void *vpPayload);

#ifdef  LINUX_BUILD
#define schdr(a, b, c, d, e)                                                    \
    (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d, (uint32_t)e

#define N0(fmt)                 schdr_printf(fmt, schdr(0,0,0,0,0))
#define N1(fmt, a)              schdr_printf(fmt, schdr(a,0,0,0,0))
#define N2(fmt, a, b)           schdr_printf(fmt, schdr(a,b,0,0,0))
#define N3(fmt, a, b, c)        schdr_printf(fmt, schdr(a,b,c,0,0))
#define N4(fmt, a, b, c, d)     schdr_printf(fmt, schdr(a,b,c,d,0))
#define N5(fmt, a, b, c, d, e)  schdr_printf(fmt, schdr(a,b,c,d,e))

#define NP(fmt, _1, _2, _3, _4, _5, MACRO, ...) MACRO

/**
 * Ouput info into logger so it can be transferred outside schdeuler framework
 *
 * This function has following limitations:
 *    1. It can only take up to 5 scalar arguments.
 *    2. 64-bit data type must be split into two 32-bit data type.
 *    3. String(%s) is not supported.
 */
#define SCHDR_PRINTF(fmt, ...) {                                                \
    static const char pf[] __attribute__((section(".cvtask_format"))) = fmt;\
    NP(fmt, ##__VA_ARGS__, N5, N4, N3, N2, N1, N0) \
        (pf, ##__VA_ARGS__); \
    }
#else
#define SCHDR_PRINTF schdr_printf
#endif

#endif //SCHDR_H

