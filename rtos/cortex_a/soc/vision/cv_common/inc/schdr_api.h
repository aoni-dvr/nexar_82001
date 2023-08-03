/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
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

#ifndef SCHDR_API_H_FILE
#define SCHDR_API_H_FILE

#define MAX_CVTABLE_COUNT       16U
#define MAX_SYSFLOW_COUNT       16U

#define MAX_THPOOL_SIZE           4
#define MAX_THPOOL_EXTRA_LIMITED  32

#include "ambint.h"
#include "cvtask_api.h"
#include <cvtask_errno.h>   /* for errcode_enum_t       */
#include <cvapi_flexidag.h> /* for FLEXIDAG_MAX_SLOTS   */

/*
 * @cpus_map:  specifies the physical cores on which to run the cvtask
 * @flags:     flags to control various behavior of the scheduler
 * @boot_cfg:  parameter to control ORC boot
 *             [0]   --> for non-orc-booting sub-scheduler
 *             [1~4] --> total number of sub-schedulers for the framework
 *
 * Initialize the scheduler (SuperDAG mode)
 */
#define SCHDR_AGGREGATE_SYSFLOW         0x0001U  // dump aggregated sysflow table
#define SCHDR_DISABLE_CVTABLE_LOG       0x0002U  // disable info log for cvtable
#define SCHDR_DISABLE_SS_LOG            0x0004U  // disable startup/shutdown log
#define SCHDR_ENABLE_VP_RESET           0x0100U  // enable VP-reset
#define SCHDR_FASTBOOT                  0x1000U  // Fastboot state
#define SCHDR_IS_SECONDARY_SCHEDULER    0x8000U  // Secondary scheduler (for TSR scheduler)
#define SCHDR_NO_INTERNAL_THREAD        0x10000U // No interal thread in scheduler

#define SCHDR_ARM_ORC_RESET             0x0000U  //ARM ORC reset
#define SCHDR_ARM_RESET                 0x0001U  //ARMreset

int32_t schdr_init(int32_t cpus_map, int32_t flags, int32_t boot_cfg);

uint32_t schdr_get_ambacv_state(uint32_t *pState);

uint32_t schdr_get_state(uint32_t *pState);

/*
 * Start sub-scheduler (SuperDAG mode) on hosting OS
 */
int32_t schdr_start(void);

/*
 * Shutdown entire schduler framework
 */
int32_t schdr_shutdown(int32_t reserved);

/*
 * Callback function to fill a binary data into a buffer provided by scheduler
 *
 * @buff:       address of buffer
 * @size:       maximum size of the buffer
 * @token:      token that identifies binary resouce
 *
 * If @buff==NULL, return the data size only.
 * If @buff!=NULL, fill the data into @buff and return the data size
 */
typedef int32_t (*schdr_fill_cb)(char *buff, int32_t size, void* token);

/*
 * add a cvtable to scheduler
 * @token:      cvtable identifier
 * @cb:         callback to fill the cvtable data
 */
int32_t schdr_add_cvtable(void *token, schdr_fill_cb cb);

/*
 * add a system-flow table to scheduler
 * @token:      sysflow identifier
 * @cb:         callback to fill the sysflow data
 */
int32_t schdr_add_sysflow(void *token, schdr_fill_cb cb);

/*
 * insert a new entry to cvtable
 * @name:       entry name
 * @data:       entry data pointer
 * @length:     entry data lengh
 */
int32_t schdr_insert_cvtable_entry(const char *name, const void *data, int32_t length);

/*
 * App can optionally register a callback function to get the status
 * of the scheduler
 */
#define SCHDR_CB_START_REGISTRATION     0x01U
#define SCHDR_CB_START_QUERY            0x02U
#define SCHDR_CB_START_INIT             0x03U
#define SCHDR_CB_START_RUN              0x04U
#define SCHDR_CB_START_SHUTDOWN         0x05U

typedef int32_t (*schdr_cb)(int32_t type, void *arg);

int32_t schdr_set_callback(schdr_cb cb);

/*
 * Set the name of shutdown log file
 */
int32_t schdr_set_shutdown_log_name(const char *name);

/*
 * Set the name of shutdown log prefix
 */
void schdr_set_log_prefix(const char *prefix);

/*
 * Set the DRAM simulation mode
 * For MODE_SHM, the arg is the name for the shared memory
 */
#define SCHDR_DRAM_MODE_MALLOC             0U
#define SCHDR_DRAM_MODE_MMAP               1U
#define SCHDR_DRAM_MODE_SHM                2U

int32_t schdr_set_dram_mode(int32_t mode, const void *arg);

uint32_t schdr_get_audio_tick(uint32_t *freq);

uint32_t schdr_get_perf_last_wridx_daddr(uint32_t visorc, uint32_t *w_index);

uint32_t schdr_get_arm_perf_last_wridx_daddr(uint32_t *w_index);

void schdr_insert_customer_event(uint32_t event_id, uint32_t sub_index, uint32_t data0, uint32_t data1);

uint32_t schdrmsg_send_vinmsg(const void *msg);

/*
 * Amalgam mode only: socket/key for pairing server, total dram size
 *
 */
int32_t amalgam_config(char *inet, int32_t port, uint64_t key,
                       uint32_t dram_size);

int32_t schdrcfg_parse_amalgam_opts(int32_t argc, char *argv[]);  /* Only needed for AMALGAM mode */
int32_t schdrcfg_set_flexidag_num_slots(int32_t num_slots);   /* Default is 0 */
int32_t schdrcfg_set_cavalry_num_slots(int32_t num_slots);    /* Default is 0 */
int32_t schdrcfg_set_cpu_map(int32_t cpu_map);                /* Default is 0xD */
int32_t schdrcfg_set_verbosity(int32_t verbosity);
int32_t schdrcfg_set_flags(int32_t flags);
int32_t schdrcfg_set_boot_cfg(int32_t cfg);
int32_t schdrcfg_set_flexidag_arm_scratchpad_size(uint32_t size); /* FlexiDAG only */
int32_t schdrcfg_set_autorun_interval(int32_t interval);
int32_t schdrcfg_set_autorun_fex_cfg(int32_t cfg);
int32_t schdrcfg_set_log_entries_visschdr(uint32_t size);
int32_t schdrcfg_set_log_entries_viscvtask(uint32_t size);
int32_t schdrcfg_set_log_entries_visperf(uint32_t size);

/*
 * Start subscheduler (Standalone mode) on the hosting OS
 */
int32_t schdr_standalone_start(void);

/*
 * Request scheduler to flush any pending VISORC printfs.
 * flush_type == 0 : PERF log only
 * flush_type != 0 : All VISORC logs
 */
int32_t schdr_flush_visorc_printf(uint32_t flush_type);

/*
 * Attach/Remove flexidag scheduler support (Standalone mode) on the hosting OS
 */
int32_t flexidag_schdr_start(void);
int32_t flexidag_schdr_stop(void);

errcode_enum_t  schdr_get_cma_region_info(uint64_t *pBase, uint64_t *pSize);  /* Returns location and size of CMA region, if available */

int32_t cavalry_init(uint32_t version, uint32_t num_slots);

/*------------------------------------------------------------------------------------------------*/
/*-= Flexidag system related =--------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int32_t flexidag_system_init(uint32_t version, uint32_t num_slots);

/*------------------------------------------------------------------------------------------------*/
/*-= Miscellaneous ARM support functions =--------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------*/
int32_t schdr_wake_feeders(int32_t wake_all_feeders, uint32_t target_uuid);
int32_t schdr_request_shutdown(int32_t reserved);

/*
 * Get the RTOS memory region that is allocated for the system.  If
 * no region is allocated, or there is an error, this will return a
 * non-zero value.
 */
int32_t schdr_get_rtos_region(uint64_t *base, uint64_t *size, uint32_t *is_cached);
int32_t schdr_get_cvschdr_region_info(uint64_t *base, uint64_t *size);
uint32_t schdr_get_mmb_cache(uint64_t base, uint32_t *is_cached);

uint32_t schdr_wait_msg(uint32_t *msg_num);
uint32_t schdr_process_msg(void **vppHandle, uint32_t *type, uint32_t *message_retcode);
uint32_t schdr_process_safety_monitor(void *arg);

/*=- Scheduler trace information -================================================================*/
/* The scheduler trace information */
/*================================================================================================*/
#define CVTRACE_MAX_ENTRIES       16U

typedef struct {
    char      cvcore_name[12];                    /* +  0 : Name (string)                 */
    uint8_t   visorc_core_id;                     /* + 12 : VISORC core_id                */
    uint8_t   cvcore_type;                        /* + 13 : VISORC core_type              */
    uint8_t   reserved[2];                        /* + 14 : Reserved for padding          */
    uint32_t  cvload_report_time;                 /* + 16 : Time report generated         */
    uint32_t  cvload_500ms;                       /* + 20 : cycles used over past 500ms   */
    uint32_t  cvload_2500ms;                      /* + 24 : cycles used over past 2500ms  */
    uint32_t  cvload_10000ms;                     /* + 28 : cycles used over past 10000ms */
} cvthread_trace_t;

typedef struct {
    uint32_t  reserved[128];      /* Temporary 512b structure placeholder until developed */

    /*-= CVLoad trace information =-------------------------------------------------------*/
    cvthread_trace_t  cvthread_loadtrace[CVTRACE_MAX_ENTRIES];  /* +512 : CVThread load   */

} scheduler_trace_t;

scheduler_trace_t *schdr_get_pSchedulerTrace(void);

/*-= Unified with cvscheduler_interface.h */

/**
 * sysflow_name_entry_t
 *
 * Simplified system flow table - contains names, hashes, and a few flags shadowed from the
 * main system flow table.
 */

typedef struct {
    char        instance_name[NAME_MAX_LENGTH];
    char        algorithm_name[NAME_MAX_LENGTH];
    char        step_name[NAME_MAX_LENGTH];
    char        cvtask_name[NAME_MAX_LENGTH];

    uint32_t    instance_hash;
    uint32_t    algorithm_hash;

    uint32_t    step_hash;
    uint32_t    cvtask_name_hash;

    uint32_t    entry_hash;
    uint32_t    uuid;

    uint32_t    cvtask_index                  :16;
    uint32_t    cvtask_instance               :8;
    uint32_t    is_instance_hash_nonunique    :1;
    uint32_t    is_algorithm_hash_nonunique   :1;
    uint32_t    is_step_hash_nonunique        :1;
    uint32_t    is_cvtask_name_hash_nonunique :1;
    uint32_t    is_entry_hash_nonunique       :1;
    uint32_t    is_sod_task                   :1;
    uint32_t    is_vp_task                    :1;
    uint32_t    is_arm_task                   :1;
    uint32_t    flags_unused1;
} sysflow_name_entry_t;

typedef struct {
    uint32_t  last_wake_time;
    uint32_t  slot_state;
    errcode_enum_t  last_errorcode;
    uint32_t  pending_frameset_id;
    uint32_t  finished_frameset_id;
    uint32_t  sysflow_name_table_daddr;
    uint32_t  sysflow_name_table_numlines;
    uint32_t  flexidag_name_daddr;
    uint32_t  fdarm_printf_string_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  fdarm_printf_string_size[SYSTEM_MAX_NUM_CORTEX];

    uint32_t  fdvis_printbuf_cvtask_daddr[SYSTEM_MAX_NUM_VISORC];
    uint32_t  fdvis_printbuf_cvtask_size[SYSTEM_MAX_NUM_VISORC];
    uint32_t  fdarm_printbuf_cvtask_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  fdarm_printbuf_cvtask_size[SYSTEM_MAX_NUM_CORTEX];

    uint32_t  fdvis_printbuf_sched_daddr[SYSTEM_MAX_NUM_VISORC];    // Note : Last entry is this array is now repurposed; see below
    uint32_t  fdvis_printbuf_sched_size[SYSTEM_MAX_NUM_VISORC];     // -/
    uint32_t  fdarm_printbuf_sched_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  fdarm_printbuf_sched_size[SYSTEM_MAX_NUM_CORTEX];

    uint32_t  fdvis_printbuf_perf_daddr[SYSTEM_MAX_NUM_VISORC];
    uint32_t  fdvis_printbuf_perf_size[SYSTEM_MAX_NUM_VISORC];
    uint32_t  fdarm_printbuf_perf_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  fdarm_printbuf_perf_size[SYSTEM_MAX_NUM_CORTEX];
} flexidag_slot_trace_t;

#define fd_buildinfo_table_daddr      fdvis_printbuf_sched_daddr[SYSTEM_MAX_NUM_VISORC - 1U]
#define fd_buildinfo_KEEP_AT_ZERO     fdvis_printbuf_sched_size[SYSTEM_MAX_NUM_VISORC - 1U]

typedef struct {
    uint32_t  last_wake_time;
    uint32_t  system_state;
    uint32_t  num_slots_enabled;
    uint32_t  num_slots_used;
    uint32_t  sysvis_scheduler_code_base;
    uint32_t  sysvis_scheduler_code_size;
    uint32_t  sysvis_hotlink_code_base;
    uint32_t  sysvis_hotlink_code_size;
    uint32_t  sysarm_printf_string_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  sysarm_printf_string_size[SYSTEM_MAX_NUM_CORTEX];

    uint32_t  sysvis_printbuf_cvtask_daddr[SYSTEM_MAX_NUM_VISORC];
    uint32_t  sysvis_printbuf_cvtask_size[SYSTEM_MAX_NUM_VISORC];
    uint32_t  sysarm_printbuf_cvtask_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  sysarm_printbuf_cvtask_size[SYSTEM_MAX_NUM_CORTEX];

    uint32_t  sysvis_printbuf_sched_daddr[SYSTEM_MAX_NUM_VISORC];   // Note : Last entry is this array is now repurposed; see below
    uint32_t  sysvis_printbuf_sched_size[SYSTEM_MAX_NUM_VISORC];    // -/
    uint32_t  sysarm_printbuf_sched_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  sysarm_printbuf_sched_size[SYSTEM_MAX_NUM_CORTEX];

    uint32_t  sysvis_printbuf_perf_daddr[SYSTEM_MAX_NUM_VISORC];
    uint32_t  sysvis_printbuf_perf_size[SYSTEM_MAX_NUM_VISORC];
    uint32_t  sysarm_printbuf_perf_daddr[SYSTEM_MAX_NUM_CORTEX];
    uint32_t  sysarm_printbuf_perf_size[SYSTEM_MAX_NUM_CORTEX];

    flexidag_slot_trace_t slottrace[FLEXIDAG_MAX_SLOTS];
} flexidag_trace_t;

#define sys_buildinfo_table_daddr     sysvis_printbuf_sched_daddr[SYSTEM_MAX_NUM_VISORC - 1U]
#define sys_buildinfo_KEEP_AT_ZERO    sysvis_printbuf_sched_size[SYSTEM_MAX_NUM_VISORC - 1U]

flexidag_trace_t *schdr_get_pFlexidagTrace(void);

#define CAVALRY_TRACE_MAX_PROCESS   32U
#define CAVALRY_INVALID_PID         0xFFFFFFFFU

typedef struct {
    uint32_t  cavalry_pid;
    uint32_t  frame_no;
    uint32_t  last_access_tick;
    uint32_t  reserved[5];
    uint8_t   process_name[224]; /* Keep structure size aligned to 256b */
} cavalry_process_trace_t;

typedef struct {
    cavalry_process_trace_t   cavalry_info[CAVALRY_TRACE_MAX_PROCESS];
} cavalry_trace_t;

cavalry_trace_t *schdr_get_pCavalryTrace(void);

typedef struct {
    uint16_t  autorun_cvtask_num;             /* Number of autorun CVTasks active in the system */
    uint16_t  autorun_cvtask_id;              /* Autorun CVTaskID */
    uint16_t  autorun_period_ms;              /* Period of autorun task in ms */
    uint16_t  autorun_timeout_ms;             /* Timeout of autorun task in ms */
    uint32_t  autorun_num_pass;               /* Number of iterations that passed (capped at 0xFFFFFFFFU) */
    uint32_t  autorun_num_fail;               /* Number of iterations that failed (capped at 0xFFFFFFFFU) */
    uint32_t  autorun_last_pass_tick;         /* Audio tick of last passing run */
    uint32_t  autorun_last_fail_tick;         /* Audio tick of last failing run */
    uint32_t  autorun_last_fail_retcode;      /* Return code from last failing run */
    uint32_t  structure_64b_alignment;
    char      autorun_cvtask_name[32];        /* Note: effective name length is 31 - last byte is always 0x00 */
} autorun_trace_t;

autorun_trace_t *schdr_get_pAutoRunTrace(void);

/* These callback structures are the same as flexidag - this is just to keep it
   common, but the names unique.  Eventually they will be merged into one. */
typedef struct {
    uint32_t  flexidag_slot_id;
    uint16_t  sysflow_index;
    uint16_t  cvtask_frameset_id;
    uint32_t  cvtask_uuid;
    uint32_t  retcode;
    char     *pFormatString;
    uint32_t  msg_time;
    uint32_t  arg0;
    uint32_t  arg1;
    uint32_t  arg2;
    uint32_t  arg3;
    uint32_t  arg4;
} superdag_error_struct_t;

typedef void (*schdr_error_cb)(void *vpHandle, superdag_error_struct_t *pErrorStruct, void *vpParameter);
errcode_enum_t  schdr_set_error_callback(void *vpHandle, schdr_error_cb error_callback, void *vpParameter);
errcode_enum_t  schdr_set_coredump_callback(void *vpHandle, uint32_t cvcore_type, schdr_error_cb error_callback, void *vpParameter);

#endif /* !SCHDR_API_H_FILE */

