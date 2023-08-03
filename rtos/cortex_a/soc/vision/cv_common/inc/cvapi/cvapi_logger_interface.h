/*
* Copyright (c) 2017-2017 Ambarella, Inc.
*
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CVAPI_LOGGER_INTERFACE_H_FILE
#define CVAPI_LOGGER_INTERFACE_H_FILE

#include "cvtask_api.h"
#include <cvapi_common.h>

#if !defined(ORCVIS) && !defined(CONFIG_BUILD_CV_THREADX)
#define rptr32_t int32_t
#endif /* !ORCVIS */

#define CVLOG_TYPE_CVTASK       0U
#define CVLOG_TYPE_SCHED        1U
#define CVLOG_TYPE_PERF         2U
#define CVLOG_TYPE_MAX          3U

/*
 * Defines the general information for a cvlog buffer
 * This struct is usually passed as an  input to downstream cvtask
 */
typedef struct {
    uint16_t  entry_type;             // cvtask OR sched OR perf
    uint16_t  entry_size;             // size of each entry in the buffer
    rptr32_t  buffer_addr_offset;     // buffer starting address, offset from the current buffer's base
    uint32_t  buffer_size_in_entries; // number of entries in the buffer
    uint32_t  write_index;            // write index of the next entry to be filled, as an index into the printf buffer
    uint32_t  prev_write_index;       // last write index of the next entry to be filled, as an index into the printf buffer
    rptr32_t  binary_offset;          // pointer of string section for printf format, offset from the current buffer's base
    uint32_t  binary_size;            // size of string section
    rptr32_t  hotlink_offset;         // Pointer of hotlink section for printf format, offset from the current buffer's base;
    uint32_t  hotlink_size;           // Size of string section (hotlink)
    uint32_t  reserved_alignment[7];  // Align to 64b/structure
} cvlog_buffer_info_t;

typedef struct {
    cvlog_buffer_info_t cvtask_log;
    cvlog_buffer_info_t perf_log;
    cvlog_buffer_info_t sched_log;
} cvlog_buffer_set_t;

typedef struct {
    cvlog_buffer_set_t  orcsod;     /* Legacy : This is a direct copy of visorc[0] */
    cvlog_buffer_set_t  orcvp;      /* Legacy : This is a direct copy of visorc[1] */
    uint32_t            num_arm;
    cvlog_buffer_set_t  arm[SYSTEM_MAX_NUM_CORTEX];
    uint32_t            num_visorc;
    cvlog_buffer_set_t  visorc[SYSTEM_MAX_NUM_VISORC];
} cvlogger_state_t;

/*
 * Defines the data structure of a perf log entry
 */
#define EVENT_NEW_FRAME                 ((uint8_t)0x00U)  /* sub-struct : cvlog_perf_new_frame        */
#define EVENT_CVCORE_ON                 ((uint8_t)0x01U)  /* sub-struct : none                        */
#define EVENT_CVCORE_OFF                ((uint8_t)0x02U)  /* sub-struct : cvlog_perf_cvcore_off       */
#define EVENT_CYCLES                    ((uint8_t)0x03U)  /* sub-struct : cvlog_perf_cycles           */
#define EVENT_CVCORE_YIELD              ((uint8_t)0x04U)  /* sub-struct : none                        */
#define EVENT_RESUME_FRAME              ((uint8_t)0x05U)  /* sub-struct : cvlog_perf_new_frame        */
#define EVENT_CVTASK_READY              ((uint8_t)0x06U)  /* sub-struct : cvlog_perf_new_frame        */
/*                                          */
#define EVENT_TIME_AT_CYCLE             ((uint8_t)0xCEU)  /* sub-struct : (for sysflow_index == 0xFFFF), associate time with event.time_stamp       */
/*              u32data[0] : Linux Epoch time (treat as uint32_t to avoid rollover)       */
/*              u32data[1] : Millseconds of time (0..1000) ; associate with event_time()  */
/*                                                                                        */
#define EVENT_CHIP_INFO                 ((uint8_t)0xCFU)  /* sub-struct : (for sysflow_index == 0xFFFF)                                             */
/*              u32data[0] : CVCHIP (mask upper 16-bits)                                  */
/*                         : ticks_per_ms (mask lower 16-bits)                            */
/*                                                                                        */
/*-----------------------------------------------= uses cvlog_perf_flexidag_metadata_t =----*/
#define EVENT_FLEXIDAG_METADATA_START   ((uint8_t)0xEEU)  /* sub-struct : none                        */
#define EVENT_FLEXIDAG_METADATA_CONT    ((uint8_t)0xEFU)  /* sub-struct : none                        */

/*-----------------------------------------------= uses cvlog_perf_flexidag_entry_t =-------*/
#define EVENT_FLEXIDAG_NEW_FRAME        ((uint8_t)0xF0U)  /* sub-struct : cvlog_perf_new_frame        */
#define EVENT_FLEXIDAG_CVCORE_ON        ((uint8_t)0xF1U)  /* sub-struct : none                        */
#define EVENT_FLEXIDAG_CVCORE_OFF       ((uint8_t)0xF2U)  /* sub-struct : none                        */
#define EVENT_FLEXIDAG_CVCORE_CYCLES    ((uint8_t)0xF3U)  /* sub-struct : cvlog_perf_cycles           */
#define EVENT_FLEXIDAG_CVCORE_YIELD     ((uint8_t)0xF4U)  /* sub-struct : none                        */
#define EVENT_FLEXIDAG_RESUME_FRAME     ((uint8_t)0xF5U)  /* sub-struct : cvlog_perf_new_frame        */
#define EVENT_FLEXIDAG_CVTASK_READY     ((uint8_t)0xF6U)  /* sub-struct : cvlog_perf_new_frame        */

#define EVENT_MAX                       ((uint8_t)0xFFU)  /* 8-bit field                              */

struct cvlog_perf_new_frame {
    uint32_t  frame_num;
    uint32_t  reserved;
};

struct cvlog_perf_cycles {
    uint32_t  instructions;
    uint32_t  cpu_clocks;
};

struct cvlog_perf_cvcore_off {
    uint32_t  reserved;
    uint32_t  cvtask_retcode;
};

struct cvlog_perf_new_cavalry_frame {
    uint32_t  frame_num;
    uint32_t  pid;
};

struct cvlog_perf_new_autorun_frame {
    uint32_t  frame_num;
    uint32_t  autorun_id;
};

#define PERF_SYSFLOW_INDEX_AUTORUN              ((uint16_t)0xFFA0U)
#define PERF_SYSFLOW_INDEX_CAVALRY_VP           ((uint16_t)0xFFC0U)
#define PERF_SYSFLOW_INDEX_CAVALRY_FEX          ((uint16_t)0xFFC1U)
#define PERF_SYSFLOW_INDEX_CAVALRY_FMA          ((uint16_t)0xFFC2U)
#define PERF_SYSFLOW_INDEX_HOTLINK_BASE         ((uint16_t)0xFFE0U)
#define PERF_SYSFLOW_INDEX_CAVHL_SLOT0          ((uint16_t)0xFFE0U)
#define PERF_SYSFLOW_INDEX_CAVHL_SLOT1          ((uint16_t)0xFFE1U)
#define PERF_SYSFLOW_INDEX_CAVHL_SLOT2          ((uint16_t)0xFFE2U)
#define PERF_SYSFLOW_INDEX_CAVHL_SLOT3          ((uint16_t)0xFFE3U)

typedef struct {
    uint8_t   event;
    uint8_t   cvcore;
    uint16_t  sysflow_index;
    uint32_t  time_stamp;
    union {
        struct cvlog_perf_new_frame new_frame;
        struct cvlog_perf_new_cavalry_frame cavalry_new_frame;
        struct cvlog_perf_new_autorun_frame autorun_new_frame;
        struct cvlog_perf_cvcore_off  cvcore_off;
        struct cvlog_perf_cycles      cycles;
        uint32_t  u32data[2];
    } perfmsg;
} cvlog_perf_entry_t;

typedef struct {
    uint32_t  event             : 8;
    uint32_t  event_subtype     : 8;
    uint32_t  sysflow_index     : 8;
    uint32_t  flexidag_slot_id  : 8;
    uint32_t  time_stamp;
    union {
        uint32_t  u32data[2];
        uint16_t  u16data[4];
        uint8_t   u8data[8];
        char      chardata[8];
    } payload;
} cvlog_perf_flexidag_metadata_t;

/*-= Flexidag metadata event types =----------------------------------------------------*/
/* For "cvlog_perf_flexidag_metadata_t", unless specifically listed :                   */
/* .event             = EVENT_FLEXIDAG_METADATA_START                                   */
/* .subtype           = (one of the below defines).                                     */
/* .sysflow_index     = sysflow_index that the metadata refers to.                      */
/* .flexidag_slot_id  = flexidag_slot_id that the metadata belongs to.                  */
/* .timestamp         = time metadata was appended to perf log                          */
/*                                                                                      */
/* For "cvlog_perf_flexidag_metadata_cont_t"                                            */
/* .event             = EVENT_FLEXIDAG_METADATA_CONT                                    */
/*--------------------------------------------------------------------------------------*/
#define EVENT_ST_FLEXIDAG_CREATE      ((uint8_t)0x00U)  /* metadata : .u32data[0] = (reserved)                    */
/*            .u32data[1] = flexidag_create retcode       */
#define EVENT_ST_FLEXIDAG_OPEN        ((uint8_t)0x01U)  /* metadata : .u32data[0] = (reserved)                    */
/*            .u32data[1] = flexidag_open retcode         */
#define EVENT_ST_FLEXIDAG_INIT        ((uint8_t)0x02U)  /* metadata : .u32data[0] = (reserved)                    */
/*            .u32data[1] = flexidag_init retcode         */
#define EVENT_ST_FLEXIDAG_RUN         ((uint8_t)0x20U)  /* metadata : .u16data[0] = frameset_id                   */
/*            .u16data[1] = (reserved)                    */
/*            .u32data[1] = basetime of frameset (0 is use event time) */
#define EVENT_ST_FLEXIDAG_RUN_DONE    ((uint8_t)0x21U)  /* metadata : .u16data[0] = frameset_id                   */
/*            .u16data[1] = (reserved)                    */
/*            .u32data[1] = flexidag_run finished retcode */
#define EVENT_ST_FLEXIDAG_ENTRYNAME   ((uint8_t)0x3FU)  /* metadata : .u32data[0] = cvtask_name_strlen            */
/*            .u32data[1] = cvtask_UUID                   */
/*            + one or more EVENT_FLEXIDAG_METADATA_CONT  */
/*              .u8data[] : cvtask_name                   */
#define EVENT_ST_FLEXIDAG_CLOSE       ((uint8_t)0x40U)  /* metadata : .sysflow_index = sysflow_num_entries        */
/*            .u32data[0] = flexidag_name_strlen          */
/*            .u32data[1] = flexidag_close retcode        */
/*            + one or more EVENT_FLEXIDAG_METADATA_CONT  */
/*              .u8data[] : flexidag_name                 */

typedef struct {
    uint32_t  event             : 8;
    uint32_t  cvcore            : 8;
    uint32_t  sysflow_index     : 8;
    uint32_t  flexidag_slot_id  : 8;
    uint32_t  time_stamp;
    union {
        struct cvlog_perf_new_frame new_frame;
        struct cvlog_perf_cycles    cycles;
        uint32_t  u32data[2];
    } fdmsg;
} cvlog_perf_flexidag_entry_t;

/*
 * define the data structure of a cvtask/scheduler log entry
 */
typedef struct {
    uint32_t  index       :16;
    uint32_t  unused      : 4;
    uint32_t  dbg_level   : 4;
    uint32_t  src_thread  : 4;
    uint32_t  src_id      : 4;
} cvlog_printf_flags_t;

typedef struct {
    union {
        cvlog_printf_flags_t  entry_flags;
        uint32_t              entry_flags_dword;
    } hdr;
    uint32_t  entry_time;
    uint32_t  entry_string_offset;
    uint32_t  entry_arg1;
    uint32_t  entry_arg2;
    uint32_t  entry_arg3;
    uint32_t  entry_arg4;
    uint32_t  entry_arg5;
} cvlog_sched_entry_t, cvlog_cvtask_entry_t;

/**
 * The logger's primary output will be of type cvlogger_state_t,
 * and can be referenced using the io_name
 * IONAME_CVLOGGER_STATE.
 */
#define IONAME_CVLOGGER_STATE   "SYSTEM_LOGINFO"

#define TASKNAME_LOGGER_CVTASK  "LOGGER_CVTASK"

#endif /* !CVAPI_LOGGER_INTERFACE_H_FILE */
