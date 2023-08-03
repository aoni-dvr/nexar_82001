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

#ifndef CVAPI_FLEXIDAG_H_FILE
#define CVAPI_FLEXIDAG_H_FILE

#include <cvtask_api.h>
#include <cvtask_errno.h>

typedef unsigned long       ulong;

/*-= FlexiDag resource configuration limitations =--------------------------------------*/
#define FLEXIDAG_SLOT_SIZE            (256U * 1024U)
#define FLEXIDAG_MAX_SLOTS            120U
#define FLEXIDAG_MAX_INPUTS           16U
#define FLEXIDAG_MAX_OUTPUTS          8U
#define FLEXIDAG_MAX_FEEDBACKS        4U
#define FLEXIDAG_MAX_SFB              4U
#define FLEXIDAG_MAX_TBAR             8U
#define FLEXIDAG_VERSION              1U

/*
 * FLEXIDAG_NAME_MAX_LENGTH (fixed at 32)
 *
 * This defines the maximum length of the name associated with the flexidag,
 * contained in the FlexiBIN file.  This is limited to 31 characters, as the
 * string trailing zero is required.  This also is fixed to 32 characters to
 * ensure that the flexidag name can be contained within the flexidag_header.
 */
#define FLEXIDAG_NAME_MAX_LENGTH      32U

/*
 * FLEXIDAG_FRAME_QUEUE_SIZE (default 8)
 *
 * This defines the maximum number of frames that can be pending and active
 * (combined) within a FlexiDAG environment.  Lowering this value will reduce
 * memory requirements (scheduler internal), but at the cost of being unable
 * to queue up FlexiDAG run commands, or target messages within the FlexiDAG
 * at a future time.
 *
 * Due to the way that FlexiDAG's input/output buffers are used, this needs
 * to be kept at or below "MAX_NUM_MEMBUF_PER_SET" (currently 59).
 */
#define FLEXIDAG_FRAME_QUEUE_SIZE     8U

/*
 * FLEXIDAG_MAX_SFB_ENTRIES (default 64)
 *
 * This defines the maximium number of entries in the system flow table that
 * can be associated with a given FlexiDAG. Note that "$#%" based overrides
 * do not count towards this limit.  The last entry of this array must also
 * be effectively zeroed out, ensuring a scannable end to the table.  This
 * effectively reduces the number of usable entries by one (default 63).
 * This value must be defined to ensure that the scheduler can preallocate
 * enough memory in a cache-visible region during startup for the FlexiDAG's
 * states.  Lowering this value will reduce the memory  requirements, but at
 * the cost of limiting the number of usable entries in a FlexiDAG.
 *
 * The default value of 64 requires 117kb / Flexidag - broken down as
 *  > 384b for ORC-CACHE visible sysflow_entry_t table
 *  > 384b for ARM visible sysflow_entry_t table
 *  > 192b for ORC only visible sysflow_override_entry_t table
 *  > 160b for ORC-CACHE visible sysflow_name_entry_t table
 *  > 160b for ARM visible sysflow_name_entry_t table
 *  > 576b for system_build_info_t table (8 extra for cores)
 *  + 256b for component names
 */
#define FLEXIDAG_MAX_SFB_ENTRIES      64U

/*
 * FLEXIDAG_MAX_CVTASK_ENTRIES (default 64)
 *
 * This defines the maximium number of cvtasks that can be associated with
 * a given FlexiDAG.  The reason this value can be different from
 * FLEXIDAG_MAX_SFB_ENTRIES, is it is entirely possible to have a FlexiBin
 * that contains many cvtasks, and no sysflow tables.  In this case, the
 * application could create the SFB using a combination of CVTasks within
 * the FlexiBin, and use flexidag_add_sfb() to define the system flow table
 * prior to opening the FleexiDAG.
 *
 * Lowering this value will reduce the memory requirements, but at the
 * cost of limiting the number of cvtasks that can be registered in a
 * FlexiDAG.
 *
 * The default value of 64 requires 8kb / Flexidag
 */
#define FLEXIDAG_MAX_CVTASK_ENTRIES   64U

/*
 * FLEXIDAG_MAX_MEMBUF_SETS (default 128)
 *
 * This defines the maximium number of membuf sets available for a given
 * FlexiDAG.  At maximum it should be (SFB_ENTRIES * OUTPUTS), or 1024,
 * but in general, not every SFB entry will use all of it's outputs, and not
 * all SFBs will be used, hence why this is limited down to a lower value.
 * Lowering this value will reduce the memory requirements, but at the cost
 * of limiting the number of internal buffers available to the FlexiDAG.
 *
 * The default value of 128 requires 32kb / Flexidag
 */
#define FLEXIDAG_MAX_MEMBUF_SETS      128U

/*
 * FLEXIDAG_MAX_TBAR_ENTRIES (default 512)
 *
 * This defines the maximum number of TBAR entries, split across all TBARs
 * (max FLEXIDAG_MAX_TBAR) loaded in the FlexiDAG's binary.  This hard limit
 * is placed to ensure that the scheduler's internals can preallocate enough
 * memory in fixed locations visible to the ARM and ORC during startup.  Also
 * the last entry of the CVTable list must always be zeroed out to ensure a
 * searchable list, which effectively reduces the number of usable entries by
 * one (default of 511)  Lowering this value will reduce memory requirements,
 * but at the cost of limiting the number of CVTables available to the
 * FlexiDAG.
 *
 * The default value of 512 uses 16kb / FlexiDAG
 */
#define FLEXIDAG_MAX_TBAR_ENTRIES     512U

/*
 * FLEXIDAG_TASKNODE_POOLSIZE (default 128)
 *
 * This defines the maximum number of cvtasknodes associated with each FlexiDAG.
 * This memory must be allocated from the VISORC-cached area, so this must be
 * done before the VISORC scheduler fully starts.
 *
 * The default value of 128 uses 192kb / FlexiDAG
 */
#define FLEXIDAG_TASKNODE_POOLSIZE    128U

/*-= Structures associated with FlexiDAGs =---------------------------------------------*/
typedef struct
{
  uint32_t  flexidag_num_outputs;
  uint32_t  flexidag_state_buffer_size;
  uint32_t  flexidag_temp_buffer_size;
  uint32_t  reserved_block1[13];

  uint32_t  flexidag_output_buffer_size[FLEXIDAG_MAX_OUTPUTS];
  uint32_t  reserved_block2[16U - FLEXIDAG_MAX_OUTPUTS];
} flexidag_memory_requirements_t;

typedef struct
{
  uint32_t  version;              /* Version 1                */
  uint32_t  size;                 /* Version 1 : size = 64b   */
  uint32_t  header_reserved0;     /* (reserved for expansion) */
  uint32_t  header_reserved1;     /* (reserved for expansion) */
  /*--------------------------*/
  uint32_t  overall_retcode;      /* Retcode from full run    */
  uint32_t  start_time;           /* Supported from version 1 */
  uint32_t  end_time;             /* Supported from version 1 */
  uint32_t  output_not_generated; /* Bitmap to mark if an output isn't generated */
  uint32_t  output_donetime[FLEXIDAG_MAX_OUTPUTS];
  /*--------------------------*/
} flexidag_runinfo_t;

/*-= All flexidag memory buffers need to be passed in using this structure =------------*/
typedef struct
{
  char     *pBuffer;                  /* ARM (Virtual) address of buffer */
  ulong     buffer_daddr;             /* Physical address of buffer in DRAM */
  uint32_t  buffer_cacheable  :1;
  uint32_t  buffer_size       :31;    /* Max buffer size: (2048MB - 1) */
  ulong     buffer_caddr;             /* Client DRAM address of buffer; should equal buffer_daddr if ATT is disabled */
  uint32_t  reserved_expansion;       /* Reserved for future expansion, aligns structure to 32 bytes in size */
} flexidag_memblk_t;

typedef void (*flexidag_cb)(void *vpHandle, uint32_t flexidag_output_num, flexidag_memblk_t *pblk_Output, void *vpParameter);

typedef struct
{
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
} flexidag_error_struct_t;

typedef void (*flexidag_error_cb)(void *vpHandle, flexidag_error_struct_t *pErrorStruct, void *vpParameter);

/*-= Interface parameters, used with flexidag_set_parameter() =-------------------------*/

/**
 * FDPARAM_INTERFACE_MSGNUM_[ID/DEFAULT]
 *
 * Defines the number of CVTask messages available for ARM-side FlexiDAG
 * components to send either into the FlexiDAG (via flexidag_send_message)
 * or intra-CVTask.  This does not include custom message pools.  Each
 * message requires 1152 bytes (default of 256 uses 288kb) from the state
 * buffer.  Lowering the amount can save memory, but will cause problems
 * if the FlexiDAG requires a large number of CVTask messages to be sent
 * by the ARM.
 */
#define FDPARAM_INTERFACE_MSGNUM_ID             0U
#define FDPARAM_INTERFACE_MSGNUM_DEFAULT        256U

/**
 * FDPARAM_INTERNAL_MSGNUM_[ID/DEFAULT]
 *
 * Defines the number of CVTask messages available for VISORC-side FlexiDAG
 * components to send intra-CVTask.  This does not include custom message
 * pools.  Each message requires 1152 bytes (default of 256 messages uses
 * 288kb) from the state buffer. Lowering the amount can save memory, but
 * will cause problems the FlexiDAG requires a large number of CVTask
 * messages to be sent by VISORC-based CVTasks.
 */
#define FDPARAM_INTERNAL_MSGNUM_ID              32U
#define FDPARAM_INTERNAL_MSGNUM_DEFAULT         256U

/**
 * FDPARAM_TOKEN_TIMEOUT_[ID/DEFAULT] (default 10000)
 *
 * Defines the amount of time, in milliseconds to wait for a response for
 * any given FlexiDAG API command.  If the timeout expires, there will be
 * an error returned from the FlexiDAG API call.
 */
#define FDPARAM_TOKEN_TIMEOUT_ID                256U
#define FDPARAM_TOKEN_TIMEOUT_DEFAULT           10000U

/**
 * FDPARAM_LOG_ENTRIES_ARM[#]_[ID/DEFAULT] (default 8192)
 *
 * Defines the number of log entries associated with an ARM Core's CVTasks.
 * Each entry is 36 bytes in size, and is allocated within the state buffer.
 * Lowering this amount can reduce memory usage (default of 8192 uses 288kb
 * of memory), but will limit the usefulness of the logs. 
 * 
 * If an ARM core is not utilized with a given FlexiDAG, then no memory is
 * used for that log.
 */
#define FDPARAM_LOG_ENTRIES_ARM_DEFAULT         8192U
#define FDPARAM_LOG_ENTRIES_ARM0_ID             1024U
#define FDPARAM_LOG_ENTRIES_ARM1_ID             1025U
#define FDPARAM_LOG_ENTRIES_ARM2_ID             1026U
#define FDPARAM_LOG_ENTRIES_ARM3_ID             1027U

/**
 * FDPARAM_LOG_ENTRIES_ARMSCH[#]_[ID/DEFAULT] (default 8192)
 *
 * Defines the number of log entries associated with an ARM Core's scheduler.
 * Each entry is 36 bytes in size, and is allocated within the state buffer.
 * Lowering this amount can reduce memory usage (default of 8192 uses 288kb
 * of memory), but will limit the usefulness of the logs.
 * 
 * If an ARM core is not utilized with a given FlexiDAG, then no memory is
 * used for that log.
 */
#define FDPARAM_LOG_ENTRIES_ARMSCH_DEFAULT      8192U
#define FDPARAM_LOG_ENTRIES_ARMSCH0_ID          1040U
#define FDPARAM_LOG_ENTRIES_ARMSCH1_ID          1041U
#define FDPARAM_LOG_ENTRIES_ARMSCH2_ID          1042U
#define FDPARAM_LOG_ENTRIES_ARMSCH3_ID          1043U

/**
 * FDPARAM_LOG_ENTRIES_ARMPERF[#]_[ID/DEFAULT] (default 8192)
 *
 * Defines the number of perf entries associated with an ARM Core's scheduler.
 * Each entry is 16 bytes in size, and is allocated within the state buffer.
 * Lowering this amount can reduce memory usage (default of 8192 uses 128kb
 * of memory), but will limit the usefulness of the logs.
 * 
 * If an ARM core is not utilized with a given FlexiDAG, then no memory is
 * used for that log.
 */
#define FDPARAM_LOG_ENTRIES_ARMPERF_DEFAULT     8192U
#define FDPARAM_LOG_ENTRIES_ARMPERF0_ID         1056U
#define FDPARAM_LOG_ENTRIES_ARMPERF1_ID         1057U
#define FDPARAM_LOG_ENTRIES_ARMPERF2_ID         1058U
#define FDPARAM_LOG_ENTRIES_ARMPERF3_ID         1059U

/**
 * FDPARAM_LOG_ENTRIES_VISORC[#]_[ID/DEFAULT] (default 8192)
 *
 * Defines the number of log entries associated with a given VISORC core's
 * CVTasks.  Each entry is 36 bytes in size, and is allocated within the
 * state buffer.  Lowering this amount can reduce memory usage (default of
 * 8192 uses 288kb of memory), but will limit the usefulness of the logs.
 *
 * By default all CV chips have VISORC core #0.  If a VISORC core is not
 * available on the given chip, then no memory is actually used for that log.
 */
#define FDPARAM_LOG_ENTRIES_VISORC_DEFAULT      8192U
#define FDPARAM_LOG_ENTRIES_VISORC0_ID          1280U
#define FDPARAM_LOG_ENTRIES_VISORC1_ID          1281U
#define FDPARAM_LOG_ENTRIES_VISORC2_ID          1282U
#define FDPARAM_LOG_ENTRIES_VISORC3_ID          1283U

/**
 * FDPARAM_LOG_ENTRIES_VISSCH[#]_[ID/DEFAULT] (default 8192)
 *
 * Defines the number of log entries associated with a given VISORC's
 * scheduler.  Each entry is 36 bytes in size, and is allocated within the
 * state buffer.  Lowering this amount can reduce memory usage (default of
 * 8192 uses 288kb of memory), but will limit the usefulness of the logs.
 * 
 * By default all CV chips have VISORC core #0.  If a VISORC core is not
 * available on the given chip, then no memory is actually used for that log.
 */
#define FDPARAM_LOG_ENTRIES_VISSCH_DEFAULT      8192U
#define FDPARAM_LOG_ENTRIES_VISSCH0_ID          1296U
#define FDPARAM_LOG_ENTRIES_VISSCH1_ID          1297U
#define FDPARAM_LOG_ENTRIES_VISSCH2_ID          1298U
#define FDPARAM_LOG_ENTRIES_VISSCH3_ID          1299U

/**
 * FDPARAM_LOG_ENTRIES_VISPERF[#]_[ID/DEFAULT] (default 8192)
 *
 * Defines the number of perf entries associated with a given VISORC's
 * scheduler.  Each entry is 16 bytes in size, and is allocated within the
 * state buffer.  Lowering this amount can reduce memory usage (default of
 * 8192 uses 128kb of memory), but will limit the usefulness of the logs.
 * 
 * By default all CV chips have VISORC core #0.  If a VISORC core is not
 * available on the given chip, then no memory is actually used for that log.
 */
#define FDPARAM_LOG_ENTRIES_VISPERF_DEFAULT     8192U
#define FDPARAM_LOG_ENTRIES_VISPERF0_ID         1312U
#define FDPARAM_LOG_ENTRIES_VISPERF1_ID         1313U
#define FDPARAM_LOG_ENTRIES_VISPERF2_ID         1314U
#define FDPARAM_LOG_ENTRIES_VISPERF3_ID         1315U

#define FLEXIDAG_INVALID_SLOT_ID                0xFFFFFFFFU

/*-= FlexiDag API function prototypes : These are for ARM only =------------------------*/
#ifndef ORCVIS

errcode_enum_t  flexidag_create(void **vppHandle);

errcode_enum_t  flexidag_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value);

errcode_enum_t  flexidag_add_sfb(void *vpHandle, flexidag_memblk_t *pblk_sfb);

errcode_enum_t  flexidag_add_tbar(void *vpHandle, flexidag_memblk_t *pblk_tbar);

errcode_enum_t  flexidag_open(void *vpHandle, flexidag_memblk_t *pblk_FlexiBin, flexidag_memory_requirements_t *pFlexiRequirements);

errcode_enum_t  flexidag_set_state_buffer(void *vpHandle, flexidag_memblk_t *pblk_Buffer);

errcode_enum_t  flexidag_set_temp_buffer(void *vpHandle, flexidag_memblk_t *pblk_Buffer);

errcode_enum_t  flexidag_set_output_callback(void *vpHandle, uint32_t output_num, flexidag_cb output_callback, void *vpParameter);

errcode_enum_t  flexidag_set_error_callback(void *vpHandle, flexidag_error_cb error_callback, void *vpParameter);

errcode_enum_t  flexidag_set_coredump_callback(void *vpHandle, uint32_t cvcore_type, flexidag_error_cb error_callback, void *vpParameter);

errcode_enum_t  flexidag_init(void *vpHandle);

errcode_enum_t  flexidag_prep_run(void *vpHandle);

errcode_enum_t  flexidag_set_input_buffer(void *vpHandle, uint32_t input_num, flexidag_memblk_t *pblk_Buffer);

errcode_enum_t  flexidag_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, flexidag_memblk_t *pblk_Buffer);

errcode_enum_t  flexidag_set_output_buffer(void *vpHandle, uint32_t output_num, flexidag_memblk_t *pblk_Buffer);

errcode_enum_t  flexidag_get_sysflow_index_by_UUID(void *vpHandle, uint32_t flow_uuid, uint16_t *pIndex);

errcode_enum_t  flexidag_get_sysflow_index_list(void *vpHandle,
	const char  *instance, const char  *algorithm, const char  *step, const char  *cvtask,
	uint16_t    *index_array, uint32_t array_size,
	uint32_t    *num_found);

errcode_enum_t  flexidag_cvtable_find(void *vpHandle, const char *target, const void **ptr, uint32_t *size);

errcode_enum_t  flexidag_set_frequency(void *vpHandle, uint32_t num_entries, uint16_t *pIndexArray, uint8_t *pFreqArray);

errcode_enum_t  flexidag_set_debug_level(void *vpHandle, uint16_t target_index, uint16_t dbg_lvl);

errcode_enum_t  flexidag_send_message(void *vpHandle, uint16_t target_index, void *vpMessage, uint32_t length);

errcode_enum_t  flexidag_send_private_msg(void *vpHandle, uint16_t target_index, const uint8_t *pMessage);

errcode_enum_t  flexidag_run(void *vpHandle, flexidag_runinfo_t *pRunInfo);  /* pRunInfo may be NULL if not needed */

errcode_enum_t  flexidag_run_noblock(void *vpHandle, uint32_t *pToken_id);  /* pToken_id may be NULL if not needed */

errcode_enum_t  flexidag_wait_run_finish(void *vpHandle, uint32_t token_id, flexidag_runinfo_t *pRunInfo); /* pRunInfo may be NULL if not needed */

uint32_t  flexidag_dump_log(void *vpHandle, void *vpBuffer, uint32_t buffer_size, uint32_t log_flags); /* pBuffer may be NULL if not needed, returns bytes written */

uint32_t  flexidag_find_slot_by_vphandle(void *vpHandle);

#define FLEXILOG_ECHO_TO_CONSOLE    0x00000001U
#define FLEXILOG_PREPEND_HEADER     0x00000002U

#define FLEXILOG_COREMASK           0x00F00000U
#define FLEXILOG_CORE0              0x00000000U
#define FLEXILOG_CORE1              0x00100000U
#define FLEXILOG_CORE2              0x00200000U
#define FLEXILOG_CORE3              0x00300000U

#define FLEXILOG_TYPEMASK           0x000F0000U
#define FLEXILOG_VIS_PERF           0x00000000U
#define FLEXILOG_VIS_SCHED          0x00010000U
#define FLEXILOG_VIS_CVTASK         0x00020000U
#define FLEXILOG_ARM_PERF           0x00080000U
#define FLEXILOG_ARM_SCHED          0x00090000U
#define FLEXILOG_ARM_CVTASK         0x000a0000U

#define FLEXILOG_FULL_FLUSH         0x80000000U /* Dumps full log */

errcode_enum_t  flexidag_close(void *vpHandle);

errcode_enum_t  flexidag_show_info(void *vpHandle);

errcode_enum_t  flexibin_metadata_find(void *pFlexiBin, const char *pMetadataName, const void **vppBuffer, uint32_t *pSize);

#endif /*!ORCVIS*/

/*-= Profiler file specific structures (taken from profiler.c) =--------------*/
#define AMBAPROF_MAX_NAME_LEN   128U

typedef struct
{
  char       name[24];
  uint32_t   size;
  uint32_t   base;
} ambaprof_section_t;

typedef struct
{
  uint32_t   version;
  uint32_t   magic_num;
  uint32_t   section_count;
  uint32_t   padding;
} ambaprof_header_t;

/* For flexidag (ambaprof_header_t.version >= 2) : */
typedef struct
{
  uint32_t    flexidag_slot_id;
  uint32_t    flexidag_num_sysflow_entries;
  uint32_t    reserved_padding[6];
  uint8_t     flexidag_name[FLEXIDAG_NAME_MAX_LENGTH];
} ambaprof_flexidag_header_t;

#endif /* ?CVAPI_FLEXIDAG_H_FILE */

