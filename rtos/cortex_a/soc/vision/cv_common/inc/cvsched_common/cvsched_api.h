/**
 *  @file cvsched_api.h
 *
 *  Copyright (c) 2020-2022 Ambarella International LP
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
 *  @details Definitions & Constants for the interface between the /n
 *           CVScheduler and Vision Driver (non-ASIL version)
 *
 */

#ifndef CVSCHED_API_H_FILE
#define CVSCHED_API_H_FILE

/*-= CVScheduler API file =-----------------------------------------------------------------------*/
#ifdef ORCVIS
#include <dsp_types.h>
#endif

/*================================================================================================*/
/* Core definitions for the CVScheduler Interface                                                 */
/*================================================================================================*/

/*! @def CVSCHEDULER_INTERFACE_VERSION
 *
 *  @brief Interface version used between the ARM sub-schedulers and VISORC master
 *  CVScheduler.
 *
 *  In general this version only needs to be upgraded when major features are
 *  added, or if some of the core defines below are changed.
 *
 *  CVSCHEDULER_INTERFACE_VERSION 8: (requires CVSCHEDULER_KERNEL_VERSION 6)
 *  > Increases number of messages on VISORC <-> ARM interface to 1024
 *
 *  CVSCHEDULER_INTERFACE_VERSION 7:
 *  > Adds explicit support for seqno/fnv1a_csum
 *
 *  CVSCHEDULER_INTERFACE_VERSION 6:
 *  > Adds fields required for MISRA/ASIL compliance
 *  > Expands flexidag_memblk_t structure to contain daddr and caddr
 *  > Removes support for CVTask shared storage on ARM CVTasks
 */
#define CVSCHEDULER_INTERFACE_VERSION       8U

/*! @def ARM_CACHELINE_SIZE
 *
 *  @brief Defines the cacheline size for the ARM
 *
 *  Defines the minimum number of bytes in a cache-line for the ARM.  This
 *  is needed, as several structures rely on keeping the cache lines separate
 *  when communicating between the VISORC and ARM to ensure that cache lines
 *  don't overlap.  Note that changing this value also requires a change
 *  of the CVSCHEDULER_INTERFACE_VERSION, as it affects the messaging
 *  interface.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 8U    : ARM_CACHELINE_SIZE = 64U
 */
#define ARM_CACHELINE_SIZE                  64U

/*! @def ARMVIS_MESSAGE_NUM
 *
 *  @brief Defines the number of messages in each CVScheduler <-> Vision Driver queue
 *
 *  Note that changing this value requires a change of the
 *  CVSCHEDULER_INTERFACE_VERSION as it affects the messaging interface.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 7U    : ARMVIS_MESSAGE_NUM = 256U
 *  CVSCHEDULER_INTERFACE_VERSION >= 8U    : ARMVIS_MESSAGE_NUM = 1024U
 */
#define ARMVIS_MESSAGE_NUM                  1024U

/*! @def ARMVIS_MESSAGE_SIZE
 *
 *  @brief Defines the size of the messages in each CVSCheduler <-> Vision Driver queue
 *
 *  Note that changing this value requires a change of the
 *  CVSCHEDULER_INTERFACE_VERSION as it affects the messaging interface.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 8U    : ARMVIS_MESSAGE_SIZE = 256U
 */
#define ARMVIS_MESSAGE_SIZE                 256U

/*! @def SUPERDAG_MAX_CVTABLE_PER_ARM
 *
 *  @brief Defines the maximum number of CVTables available per ARM in SuperDAG mode
 *
 *  Note that changing this value requires a change of the
 *  CVSCHEDULER_INTERFACE_VERSION, as it affects some of the messages.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 8U    : SUPERDAG_MAX_CVTABLE_PER_ARM = 16U
 */
#define SUPERDAG_MAX_CVTABLE_PER_ARM        16U

/*! @def SUPERDAG_MAX_SYSFLOW_PER_ARM
 *
 *  @brief Defines the maximum number of Sysflow Tables per ARM in SuperDAG mode
 *
 *  Note that changing this value requires a change of the
 *  CVSCHEDULER_INTERFACE_VERSION, as it affects some of the messages.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 8U    : SUPERDAG_MAX_SYSFLOW_PER_ARM = 4U
 */
#define SUPERDAG_MAX_SYSFLOW_PER_ARM        4U

/*! @def MAX_IDSP_BUFFERS
 *
 *  @brief Defines the maximum number message buffers from the IDSP to VISORC in
 *         SuperDAG mode.
 *
 *  Note that changing this value requires a change of the
 *  CVSCHEDULER_INTERFACE_VERSION, as it affects some of the messages.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 8U    : MAX_IDSP_BUFFERS = 32U
 */
#define MAX_IDSP_BUFFERS                    32U

/*! @def MAX_RADAR_BUFFERS
 *
 *  @brief Defines the maximum number message buffers from the RADAR to VISORC in
 *         SuperDAG mode.
 *
 *  Note that changing this value requires a change of the
 *  CVSCHEDULER_INTERFACE_VERSION, as it affects some of the messages.
 *
 *  CVSCHEDULER_INTERFACE_VERSION <= 8U    : MAX_RADAR_BUFFERS = 32U
 */
#define MAX_RADAR_BUFFERS                   32U

/*! @def MAX_AUTORUN_CVTASKS
 *
 *  @brief Defines the maximum number of AutoRUN CVTasks available in the system
 *
 * Changing this value does not require a change to the CVSCHEDULER_INTERFACE_VERSION,
 * as it is an internal value to the scheduler.
 */
#define MAX_AUTORUN_CVTASKS                 16U

/*! @def AUTORUN_DEFAULT_INTERVAL
 *
 *  @brief Defines the default size of an AutoRUN interval (in ms)
 */
#define AUTORUN_DEFAULT_INTERVAL    90U

/*! @def CVSCHED_MAX_PIPELINE_DEPTH
 *
 *  @brief Defines the maximum pipeline depth supported by the CVScheduler, in framesets
 */
#define CVSCHED_MAX_PIPELINE_DEPTH   8U

/*================================================================================================*/
/* CVTASK API header include                                                                      */
/*================================================================================================*/
#include <cvtask_api.h>

/**
 * Some of the defines contained in cvtask_api.h will affect the
 * CVSCHEDULER_INTERFACE_VERSION, as they are used in the interface's
 * messages.  Changing any of these will require an update to the
 * CVSCHEDULER_INTERFACE_VERSION.
 *
 * CVSCHEDULER_INTERFACE_VERSION <= 8U      :
 *  + SYSTEM_MAX_NUM_VISORC           4U
 *  + SYSTEM_MAX_NUM_CORTEX           4U
 */

/*================================================================================================*/
/* CVTask interface header include                                                                */
/*================================================================================================*/
#include <cvtask_coredef.h>

/**
 * Some of the defines contained in cvtask_coredef.h will affect the
 * CVSCHEDULER_INTERFACE_VERSION, as they are used in the interface's
 * messages.  Changing any of these will require an update to the
 * CVSCHEDULER_INTERFACE_VERSION.
 *
 * CVSCHEDULER_INTERFACE_VERSION <= 8U      :
 *  + SYSFLOW_MAX_CONFIG_DATA         8U
 *  + NAME_MAX_LENGTH                 32U
 *  + CVTASK_MAX_INPUTS               16U
 *  + CVTASK_MAX_OUTPUTS              8U
 *  + CVTASK_MAX_FEEDBACK             4U
 *  + CVTASK_MAX_MESSAGES             16U
 *  + CVTASK_MAX_CVTASKMSG_POOL       4U
 */

/*================================================================================================*/
/* Cavalry API header include                                                                     */
/*================================================================================================*/
#include <cvapi_cavalry.h>

/**
 * The message size definition (MSG_SIZE) in cvapi_cavalry.h will affect
 * the CVSCHEDULER_INTERFACE_VERSION, as it affects the size of
 * the messages contained in the communication interface.
 *
 * CVSCHEDULER_INTERFACE_VERSION <= 8U    : MSG_SIZE = 128U
 */

/*================================================================================================*/
/* FlexiDAG API header include                                                                    */
/*================================================================================================*/
#include <cvapi_flexidag.h>

/**
 * Several of the defines contained in cvapi_flexidag.h will affect the
 * CVSCHEDULER_INTERFACE_VERSION, as they are used in the interface's
 * messages.  Changing any of these will require an update to the interface's
 * version.
 *
 * CVSCHEDULER_INTERFACE_VERSION <= 8U      :
 *  + FLEXIDAG_MAX_INPUTS           16U
 *  + FLEXIDAG_MAX_OUTPUTS          8U
 *  + FLEXIDAG_MAX_FEEDBACKS        4U
 *  + FLEXIDAG_MAX_SFB              4U
 *  + FLEXIDAG_MAX_TBAR             8U
 */

/*================================================================================================*/
/* Safety-based timeouts (valid for CV2A only)                                                    */
/*================================================================================================*/

/*! @def CVSCHED_WAKE_PERIOD_MS
 *
 *  @brief Defines the period at which the CVSched thread must wake.
 *
 *  This is enabled on all safety-enabled systems.  The period for this is defined by
 *  a Software System Requirement.
 */
#define CVSCHED_WAKE_PERIOD_MS      33U

/*! @def CVFRAMEWORK_WAKE_PERIOD_MS
 *
 *  @brief Defines the period at which the CVFramework threads must wake.
 *
 *  This is enabled on all safety-enabled systems.  The period for this is defined by
 *  a Software System Requirement.
 */
#define CVFRAMEWORK_WAKE_PERIOD_MS  33U

/*! @def CVSAFETY_WAKE_PERIOD_MS
 *
 *  @brief Defines the period at which the CVSafety thread must wake.
 *
 *  This is enabled on all safety-enabled systems.  The period for this is defined by
 *  a Software System Requirement.
 */
#define CVSAFETY_WAKE_PERIOD_MS     15U

/*! @def CVSAFETY_HEARTBEAT_INTERVAL
 *
 *  @brief Defines the period at which the CVSafety heartbeat is sent.
 *
 *  This is enabled on all safety-enabled systems.  The period for this is defined by
 *  a Software System Requirement.  The CVSafety heartbeat message is defined later
 *  in this file (see visorc_safety_heartbeat_t)
 */
#define CVSAFETY_HEARTBEAT_INTERVAL 75U

#define CORTEX_INVALID_CVTASK_INDEX     (0xFFFFFFFFU)   /*!< Value to use for an invalid arm cvtask_index */

/*==========================================================================================================*/
/*==========================================================================================================*/
/* ARM <-> VISORC shared structures                                                                         */
/*==========================================================================================================*/
/*==========================================================================================================*/

/*======================================================================================*/
/*=- Component build info support -=====================================================*/
/*======================================================================================*/

/*! @def COMPONENT_NAME_STRING_MAX_LENGTH
 *
 *  @brief Defines the maximum length of a component name string.  This value
 *         represents the actual length of the string, and therefore does not
 *         does not include the trailing 0x00.  This value should always be one byte
 *         under a multiple-of-4 for efficient allocation and usage.
 */
#define COMPONENT_NAME_STRING_MAX_LENGTH    31U

/*! @def COMPONENT_NAME_STRING_ALLOC_SIZE
 *
 *  @brief Defines the actual allocation size used for the component name string.
 *         This define forces a round-up to a multiple-of-4 for alignment purposes.
 */
#define COMPONENT_NAME_STRING_ALLOC_SIZE    ((COMPONENT_NAME_STRING_MAX_LENGTH + 3U) & 0xFFFFFFFCU)

/*! @def VERSION_INFO_STRING_MAX_LENGTH
 *
 *  @brief Defines the maximum length of a version info / tool info string.  This value
 *         represents the actual length of the string, and therefore does not
 *         does not include the trailing 0x00.  This value should always be one byte
 *         under a multiple-of-4 for efficient allocation and usage.
 */
#define VERSION_INFO_STRING_MAX_LENGTH      255U

/*! @def VERSION_INFO_STRING_ALLOC_SIZE
 *
 *  @brief Defines the actual allocation size used for the version/tool info string.
 *         This define forces a round-up to a multiple-of-4 for alignment purposes.
*/
#define VERSION_INFO_STRING_ALLOC_SIZE      ((VERSION_INFO_STRING_MAX_LENGTH + 3U) & 0xFFFFFFFCU)

/*! @struct component_build_info_t
 *
 *  @brief Shared common structure to hold the component information.
 *
 *  This structure shall be placed in a memory location that is known by both the
 *  VISORC and ARM-A53.
 */
typedef struct {
    uint32_t  component_type;                 /*!< Component type (see COMPONENT_TYPE_* defines below */
    uint32_t  component_name_daddr;           /*!< Physical DRAM Address of component name (not part of this table) */
    uint32_t  reserved[14];                   /*!< Alignment to 64 bytes                                        */
    char      component_version[VERSION_INFO_STRING_ALLOC_SIZE];  /*!< Component version string                 */
    char      tool_version[VERSION_INFO_STRING_ALLOC_SIZE];       /*!< Component tool version string            */
} component_build_info_t;

#define COMPONENT_TYPE_END_OF_LIST  0x00000000U   /*!< Marker for end of the component list               */
#define COMPONENT_TYPE_CV1_ORCSOD   0x00000001U   /*!< Component is a CV1-ORCSOD processor                */
#define COMPONENT_TYPE_CV1_ORCVP    0x00000002U   /*!< Component is a CV1-ORCVP processor                 */
#define COMPONENT_TYPE_CV2_ORCVP    0x00000003U   /*!< Component is a CV2-ORCVP processor                 */
#define COMPONENT_TYPE_CV22_ORCVP   0x00000004U   /*!< Component is a CV22-ORCVP processor                */
#define COMPONENT_TYPE_CV25_ORCVP   0x00000005U   /*!< Component is a CV25-ORCVP processor                */
#define COMPONENT_TYPE_CV2A_ORCVP   0x00000006U   /*!< Component is a CV2A-ORCVP processor                */
#define COMPONENT_TYPE_CV28_ORCVP   0x00000007U   /*!< Component is a CV28-ORCVP processor                */
#define COMPONENT_TYPE_CV5_ORCVP    0x00000008U   /*!< Component is a CV5-ORCVP processor                 */
#define COMPONENT_TYPE_CV6_VISORC   0x00000009U   /*!< Component is a CV6-VISORC processor                */
#define COMPONENT_TYPE_CV52_ORCVP   0x0000000AU   /*!< Component is a CV52-ORCVP processor                */
#define COMPONENT_TYPE_CV22A_ORCVP  0x0000000BU   /*!< Component is a CV22A-ORCVP processor               */
#define COMPONENT_TYPE_FLEXIDAG_IO  0x10000000U   /*!< Component is an internal FlexiDAG I/O component    */
#define COMPONENT_TYPE_ORC_CVTASK   0x20000000U   /*!< Component is a VISORC CVTask                       */
#define COMPONENT_TYPE_ARM_CVTASK   0x20000001U   /*!< Component is an ARM CVTask                         */
#define COMPONENT_TYPE_ARM_SCHDR    0x40000000U   /*!< Component is the ARM Scheduler                     */
#define COMPONENT_TYPE_BYPASS       0xFFFFFFFFU   /*!< Component is a blank entry (skippable)             */

#ifdef ASIL_COMPLIANCE
/*! @struct asil_component_info_t
 *
 *  @brief Shared common structure to hold the component information. (ASIL)
 *
 *  This structure shall be placed in a memory location that is known by both the
 *  VISORC and ARM-A53.  This contains the name instead of having as a secondary
 *  string located in a different buffer.
 */
typedef struct {
    uint32_t  component_type;                 /*!< Component type (see COMPONENT_TYPE_* defines below */
    uint32_t  legacy_set_to_zero;             /*!< Legacy - set to zero */
    uint32_t  reserved[6];                    /*!< Alignment to 64 bytes                                        */
    char      component_name[COMPONENT_NAME_STRING_ALLOC_SIZE];   /*!< Component name (integrated into table)   */

    char      component_version[VERSION_INFO_STRING_ALLOC_SIZE];  /*!< Component version string                 */
    char      tool_version[VERSION_INFO_STRING_ALLOC_SIZE];       /*!< Component tool version string            */
} asil_component_info_t;
#endif /* ?ASIL_COMPLIANCE */

/*======================================================================================*/
/*=- CVTable (hashed) support -=========================================================*/
/*======================================================================================*/

/*! @struct cvtable_entry_t
 *
 *  @brief CVTable (TBAR) table of contents for hash-based searching.
 *
 *  This structure shall be placed in a memory location that is known by both the
 *  VISORC and ARM-A53.
 */
typedef struct {
    uint32_t  cvtable_hash;                     /*!< Hash entry for a CVTable entry */
    uint32_t  cvtable_size              :31;    /*!< Size of CVTable entry          */
    uint32_t  is_cvtable_hash_nonunique : 1;    /*!< Marked if the hash is not unique, name match may not occur */
    uint32_t  CVTable_daddr;                    /*!< Physical dram address of the CVTable entry */
    uint32_t  CVTable_name_daddr;               /*!< Physical dram address of teh CVTable's name */
} cvtable_entry_t;

/*======================================================================================*/
/*=- Debug buffer support -=============================================================*/
/*======================================================================================*/

/*! @struct debug_buffer_info_t
 *
 *  @brief Structure to pass debug buffer information between the VISORC to ARM-A53
 */
typedef struct {
    uint32_t  last_wridx_daddr;       /*!< Physical DRAM address of where the last write index is actually stored   */
    uint32_t  base_daddr;             /*!< Physical DRAM address of the debug buffer                                */
    uint32_t  entry_size;             /*!< Size of debug buffer entry (either 16 for perf, or 32 for cvtask/sched)  */
    uint32_t  size_in_entries;        /*!< Size of debug buffer, in entries                                         */
    uint8_t   arm_cacheline_pad[ARM_CACHELINE_SIZE - 32U];  /*!< Alignment for structure                            */
    char      core_name[8];           /*!< Name of core associated with debug buffer                                */
    char      log_type[8];            /*!< Name of log type (PERF, CVTASK, SCHED)                                   */
} debug_buffer_info_t;

/*! @struct cvprintf_buffer_info_t
 *
 *  @brief Structure containing all debug buffer descriptors
 */
typedef struct {
    debug_buffer_info_t   sod_cvtask_debugbuf;        /*!< LEGACY, will be removed in CVAPIv7 */
    debug_buffer_info_t   vp_cvtask_debugbuf;         /*!< LEGACY, will be removed in CVAPIv7 */
    debug_buffer_info_t   sod_sched_debugbuf;         /*!< LEGACY, will be removed in CVAPIv7 */
    debug_buffer_info_t   vp_sched_debugbuf;          /*!< LEGACY, will be removed in CVAPIv7 */
    debug_buffer_info_t   sod_perf_debugbuf;          /*!< LEGACY, will be removed in CVAPIv7 */
    debug_buffer_info_t   vp_perf_debugbuf;           /*!< LEGACY, will be removed in CVAPIv7 */
    debug_buffer_info_t   arm_cvtask_debugbuf[SYSTEM_MAX_NUM_CORTEX];     /*!< ARM/CVTask Debug buffer descriptors */
    debug_buffer_info_t   arm_sched_debugbuf[SYSTEM_MAX_NUM_CORTEX];      /*!< ARM/Sched Debug buffer descriptors */
    debug_buffer_info_t   arm_perf_debugbuf[SYSTEM_MAX_NUM_CORTEX];       /*!< ARM/Perf Debug buffer descriptors */
    debug_buffer_info_t   visorc_cvtask_debugbuf[SYSTEM_MAX_NUM_VISORC];  /*!< VISORC/CVTask Debug buffer descriptors */
    debug_buffer_info_t   visorc_sched_debugbuf[SYSTEM_MAX_NUM_VISORC];   /*!< VISORC/Sched Debug buffer descriptors */
    debug_buffer_info_t   visorc_perf_debugbuf[SYSTEM_MAX_NUM_VISORC];    /*!< VISORC/Perf Debug buffer descriptors */
} cvprintf_buffer_info_t;

/*================================================================================================*/
/* ARM <-> VISORC setup                                                                           */
/*                                                                                                */
/* This structure is written to by the ARM prior to booting up the VISORC scheduler, and contains */
/* as much setup information as needed to start up the basic scheduler.  This structure should    */
/* be treated as READ-ONLY by the VISORC.                                                         */
/*================================================================================================*/

/*! @struct visorc_init_params_t
 *
 *  @brief Initialization parameters for VISORC.
 *
 *  This structure shall be placed in a memory location that is known by both the
 *  VISORC and ARM-A53.
 */
typedef struct {
    uint32_t  sysinit_dram_block_baseaddr;                        /*!< [+   0] : DRAM block for scheduler - base */
    uint32_t  sysinit_dram_block_size;                            /*!< [+   4] : DRAM block for scheduler - size */

    uint32_t  coprocessor_cfg;                                    /*!< [+   8] : [Reserved] : Kernel internal */

    uint32_t  MsgQueue_orc_to_arm_daddr[SYSTEM_MAX_NUM_CORTEX];   /*!< [+  12 -  27] : Communication queues (VISORC->ARM) */
    uint32_t  MsgQueue_arm_to_orc_daddr[SYSTEM_MAX_NUM_CORTEX];   /*!< [+  28 -  43] : Communication queues (ARM->VISORC) */

    uint32_t  debug_buffer_sod_cvtask_size;                       /*!< [+  44] : Debug buffer size (SOD/CVTASK) */
    uint32_t  debug_buffer_vp_cvtask_size;                        /*!< [+  48] : Debug buffer size (VP/CVTASK) */
    uint32_t  debug_buffer_sod_sched_size;                        /*!< [+  52] : Debug buffer size (SOD/SCHED) */
    uint32_t  debug_buffer_vp_sched_size;                         /*!< [+  56] : Debug buffer size (VP/SCHED) */
    uint32_t  debug_buffer_sod_perf_size;                         /*!< [+  60] : Debug buffer size (SOD/PERF) */
    uint32_t  debug_buffer_vp_perf_size;                          /*!< [+  64] : Debug buffer size (VP/PERF) */

    uint32_t  reserved1a[11];                                     /*!< [+  68 ..  111] : Alignment for structure */
    uint32_t  chip_nvp_ticks_per_ms;                              /*!< [+ 112] : [CV6 only] : NVP core ticks per millisecond    */
    uint32_t  chip_gvp_ticks_per_ms;                              /*!< [+ 116] : [CV6 only] : GVP core ticks per millisecond    */
    uint32_t  chip_fex_ticks_per_ms;                              /*!< [+ 120] : [CV6 only] : FEX core ticks per millisecond    */
    uint32_t  chip_fma_ticks_per_ms;                              /*!< [+ 124] : [CV6 only] : FMA core ticks per millisecond    */

    uint32_t  cvchip_and_cvkversion;                              /*!< [+ 128] : CVCHIP_ID (upper 16-bits) | CVSCHEDULER_KERNEL_VERSION (lower 16-bits) */
    uint32_t  cvsched_and_cvtask_api_ver;                         /*!< [+ 132] : CVSCHEDULER_INTERFACE_VERSION (upper 16-bits) | CVTASK_API_VERSION (lower 16-bits) */
    uint32_t  cv2a_error_config;                                  /*!< [+ 136] : Error class configuration bits - if set: \n */
                                                                  /*!<           b7: (ignored - always treat S1 as critical)  \n*/
                                                                  /*!<           b6: (ignored - always treat S2 as critical)  \n*/
                                                                  /*!<           b5: Treat S3 class errors as critical        \n*/
                                                                  /*!<           b4: Treat S4 class errors as critical        \n*/
                                                                  /*!<           b3: Treat C1 class errors as critical        \n*/
                                                                  /*!<           b2: Treat C2 class errors as critical        \n*/
                                                                  /*!<           b1: (ignored - always treat C3 as critical)  \n*/
                                                                  /*!<           b0: (ignored - always treat C4 as critical)    */

    uint32_t  cv2a_cehu_config;                                   /*!< [+ 140] : CEHU configuration bits - if set :           \n*/
                                                                  /*!<           b7: Trigger CEHU on S1 class error           \n*/
                                                                  /*!<           b6: Trigger CEHU on S2 class error           \n*/
                                                                  /*!<           b5: Trigger CEHU on S3 class error           \n*/
                                                                  /*!<           b4: Trigger CEHU on S4 class error           \n*/
                                                                  /*!<           b3: Trigger CEHU on C1 class error           \n*/
                                                                  /*!<           b2: Trigger CEHU on C2 class error           \n*/
                                                                  /*!<           b1: Trigger CEHU on C3 class error           \n*/
                                                                  /*!<           b0: Trigger CEHU on C4 class error             */

    uint32_t  reserved1b;                                         /*!< [+ 144] : Reserved for expansion                         */
    uint32_t  reserved1c;                                         /*!< [+ 148] : Reserved for expansion                         */

    uint32_t  autorun_fextest_config;                             /*!< [+ 152] : Special command word for FEX configuration */
    uint32_t  autorun_interval_in_ms;                             /*!< [+ 156] : Autorun interval time in ms.  Defaults to 90ms if zero, or greater than 10000ms */
    uint32_t  chip_visorc_ticks_per_ms;                           /*!< [+ 160] : Number of VISORC ticks per millisecond (defaults to 792000 (792MHz) if zero) */
    uint32_t  chip_audio_ticks_per_ms;                            /*!< [+ 164] : Number of audio clock ticks per millsecond (defaults to 12288 (12.288MHz) if zero) */
    uint32_t  arm_linfo_base_daddr;                               /*!< [+ 168] : Needed for TSR scheduler - global LINFO for ARM*/
    uint32_t  arm_base_timestamp;                                 /*!< [+ 172] : Needed for CV2, due to FEX reset also resets VISORC timer */

    uint32_t  region_cache_flags;                                 /*!< [+ 176] : Region cache flags (used by ARM)             \n*/
                                                                  /*!<           b2: Cache enabled for RTOS                   \n*/
                                                                  /*!<           b1: Cache enabled for VISORC                 \n*/
                                                                  /*!<           b0: Cache  enabled for SuperDAG                */
#define CACHE_FLAG_SUPERDAG     1U  /*!< Used by visorc_init_params_t.region_cache_flags */
#define CACHE_FLAG_VISORC       2U  /*!< Used by visorc_init_params_t.region_cache_flags */
#define CACHE_FLAG_RTOS         4U  /*!< Used by visorc_init_params_t.region_cache_flags */

    uint32_t  cvtask_region_base_daddr;                           /*!< [+ 180] : CVTask region base daddr */
    uint32_t  kernel_region_base_daddr;                           /*!< [+ 184] : Kernel region base daddr */
    uint32_t  kernel_region_size;                                 /*!< [+ 188] : Kernel region size*/

    uint32_t  hotlink_region_daddr;                               /*!< [+ 192 ] : Hotlink region base address */
    uint32_t  hotlink_region_size;                                /*!< [+ 196 ] : Hotlink region size */
    uint32_t  cavalry_base_daddr;                                 /*!< [+ 200 ] : Cavalry base address */
    uint32_t  cavalry_num_slots;                                  /*!< [+ 204 ] : Number of Cavalry slots enabled */
    uint32_t  flexidag_num_slots;                                 /*!< [+ 208 ] : Number of Flexidag slots enabled */
    uint32_t  flexidag_slot_size;                                 /*!< [+ 212 ] : Size of a flexidag slot */
    uint32_t  cma_region_daddr;                                   /*!< [+ 216 ] : CMA region base daddr */
    uint32_t  cma_region_size;                                    /*!< [+ 220 ] : CMA region size */
    uint32_t  rtos_region_daddr;                                  /*!< [+ 224 ] : RTOS region base daddr */
    uint32_t  rtos_region_size;                                   /*!< [+ 228 ] : RTOS region size */
    uint32_t  reserved2[2];                                       /*!< [+ 232 ..  239] : Alignment for structure */

    /*-= Specialized flags =--------------------------------------------------*/
    uint32_t  disable_startup_dram_clear;                         /*!< [+ 240 ] : Set to "0xdefeed00" to disable dram clear at startup */
    uint32_t  rtos_spin_lock;                                     /*!< [+ 244 ] : [Reserved] RTOS uses this as a spinlock address */
    uint32_t  seamless_cvtask_errors;                             /*!< [+ 248 ] : Set to "0xc0debeef" to prevent cvtask errors from stopping operation */
    uint32_t  disable_cache;                                      /*!< [+ 252 ] : Set to "0x4d15able" to disable cache */

    /*-= Second 256-byte block =----------------------------------------------*/
    uint32_t  init_params_fnv1a_hash;                             /*!< [+ 256 ] : FNV1a-based hash of the preceeding 256 bytes of visorc_init_params_t */

} visorc_init_params_t;

/*==========================================================================================================*/
/*==========================================================================================================*/
/* ARM <-> VISORC messaging system                                                                          */
/*==========================================================================================================*/
/*==========================================================================================================*/

/*================================================================================================*/
/* ARM <-> VISORC message queue definition                                                        */
/*================================================================================================*/

/*! @struct armvis_message_queue_t
 *
 *  @brief Message queue control block, used between CVScheduler and Vision Driver
 *
 *  This structure shall be placed in a memory location that has full access control
 *  for both the VISORC and ARM-A53.  Each instance represents the control block for
 *  a single uni-directional queue that connects either the VISORC to the ARM-A53,
 *  or the ARM-A53 to the VISORC.
 */
typedef struct {
    uint32_t    rd_idx;                     /*!< Read index                                           */
    uint32_t    rd_msg_array_daddr;         /*!< Duplicate of msg_array_daddr for single line access  */
    uint32_t    rd_queue_id;                /*!< Duplicate of queue_id for single line access         */
    uint8_t     rx_cacheline_pad[ARM_CACHELINE_SIZE - 12U]; /*!< Alignment to ensure ARM cacheline boundaries */

    uint32_t    wr_idx;                     /*!< Write index                                          */
    uint32_t    wr_msg_array_daddr;         /*!< Duplicate of msg_array_daddr for single line access  */
    uint32_t    wr_queue_id;                /*!< Duplicate of queue_id for single line access         */
    uint8_t     wr_cacheline_pad[ARM_CACHELINE_SIZE - 12U]; /*!< Alignment to ensure ARM cacheline boundaries */

    uint32_t    msg_size;                   /*!< Message size                                         */
    uint32_t    max_count;                  /*!< Maximum number of messages                           */
    uint32_t    queue_id;                   /*!< Queue identification value.  Possible states are   \n*/
                                            /*!<  0x00 - 0x7F : VISORC -> ARM                       \n*/
                                            /*!<  0x80 - 0xFF : ARM -> VISORC                         */
    uint32_t    align_to_32bytes[5];        /*!< Alignment to ensure 32-byte alignment                */
    uint32_t    msg_array_daddr;            /*!< address of message queue                             */
    uint8_t     cfg_cacheline_pad[ARM_CACHELINE_SIZE - 36U];  /*!< Alignment to ensure ARM cacheline boundaries */

    uint8_t     reserved_cacheline_pad[ARM_CACHELINE_SIZE];   /*!< Alignment to ensure structure is 4 ARM cachelines (2 VISORC cachelines) */
} armvis_message_queue_t;

/*==========================================================================================================*/
/* Scheduler messages                                                                                       */
/*==========================================================================================================*/

/*================================================================================================*/
/* Miscellaneous defines                                                                          */
/*================================================================================================*/

/*================================================================================================*/
/* ARM <-> VISORC Message Header                                                                  */
/*================================================================================================*/

/*! @def MSGTYPEBIT_ARM_TO_VISORC
 *
 *  @brief Defines the bitmask for an ARM->VISORC message, for hdr.message_type */
#define MSGTYPEBIT_ARM_TO_VISORC                ((uint16_t)(0x8000U))

/*! @struct armvis_msg_hdr_t
 *
 *  @brief Header used for communication between CVScheduler and Vision Driver.
 *
 *  This header contains a type, slot identification, return code, and sequence number.
 *  For safety-enabled systems, a FNV-1a based checksum is also available.
 */
typedef struct {
    uint16_t      message_type;         /*!< Message type; If MSB is set, the message direction is ARM->VISORC        */
    uint16_t      message_id;           /*!< Message slot target.  Possible values are                              \n*/
                                        /*!< 0x00 : SuperDAG slot                                                   \n*/
                                        /*!< 0x100-0x1FF range : FlexiDAG slot specific                               */
    uint32_t      message_retcode;      /*!< Return code associated with message                                      */
    uint32_t      message_seqno;        /*!< Sequence number of the message                                           */
    uint32_t      message_checksum;     /*!< [SAFETY] : FNV-1a based checksum of the message.  Note that when the     */
                                        /*!<            checksum is calculated, this field is skipped.                */
} armvis_msg_hdr_t;

/*================================================================================================*/
/* Scheduler primary bootup sequence messages                                                     */
/*================================================================================================*/
/*! @struct schedmsg_boot_setup1_t
 *
 *  @brief Boot sequence, setup-1 message
 *
 *  This message is sent from the VISORC to each attached ARM-A53.  Once this message
 *  is sent, a reply (see schedmsg_boot_setup1_reply_t) must be received from each
 *  attached ARM-A53.
 */
#define SCHEDMSG_BOOT_SETUP1                    ((uint16_t)(0x0020U)) /*!< Message type for schedmsg_boot_setup1_t */
typedef struct {
    uint32_t  scheduler_version;        /*!< VISORC Scheduler version (CVSCHEDULER_INTERFACE_VERSION) */
    uint32_t  cvtask_version;           /*!< VISORC CVTask interface version (CVTASK_API_VERSION)     */

    uint32_t  scheduler_trace_daddr;    /*!< Physical address of the CVScheduler trace table          */
    uint32_t  flexidag_trace_daddr;     /*!< Physical address of the FlexiDAG trace table             */
    uint32_t  cavalry_trace_daddr;      /*!< Physical address of the Cavalry trace table              */
    uint32_t  autorun_trace_daddr;      /*!< Physical address of the AutoRUN trace table              */

} schedmsg_boot_setup1_t;

/*! @struct schedmsg_boot_setup1_reply_t
 *
 *  @brief Boot sequence, setup-1 reply message
 *
 *  This message is sent back from every ARM-A53 attached to the CVSCheduler.  This
 *  message conveys information related to the versions, cvtask information, and
 *  buffers that need to be allocated for use by the ARM subscheduler.
 */
#define SCHEDMSG_BOOT_SETUP1_REPLY              ((uint16_t)(0x0020U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_boot_setup1_reply_t */
typedef struct {
    uint32_t  scheduler_version;                /*!< ARM-A53 Scheduler version (CVSCHEDULER_INTERFACE_VERSION)      */
    uint32_t  cvtask_version;                   /*!< ARM-A53 CVTask interface version (CVTASK_API_VERISON)          */
    uint32_t  cvtask_total_num;                 /*!< Number of CVTasks loaded into this ARM-A53's subscheduler      */

    uint32_t  memsize_arm_sysflow_block;        /*!< Size of ARM-accessible memory block required to load SuperDAG sysflow tables into */
    uint32_t  memsize_arm_cvtable_block;        /*!< Size of ARM-accessible memory block required to load AutoRUN and SuperDAG TBAR files into */
    uint32_t  memsize_printf_string_block;      /*!< Size of ARM-accessible memory block required to shadow printf strings into */
    uint32_t  memsize_arm_cvtask_debug_buffer;  /*!< Size of debug buffer (in bytes) for the ARM/CVTask Debug Buffer  */
    uint32_t  memsize_arm_sched_debug_buffer;   /*!< Size of debug buffer (in bytes) for the ARM/Sched Debug Buffer   */
    uint32_t  memsize_arm_perf_debug_buffer;    /*!< Size of debug buffer (in bytes) for the ARM/Perf Debug Buffer    */
} schedmsg_boot_setup1_reply_t;

/*! @struct schedmsg_boot_setup2_t
 *
 * @brief Boot sequence, setup-2 message
 *
 * This message conveys the locations of the buffers requested by boot-setup-1-reply
 * for each ARM-A53 attached to the CVScheduler.  This also reports the location of
 * the IDSP communication queues.  These queues are only used for SuperDAG mode.
 */
#define SCHEDMSG_BOOT_SETUP2                    ((uint16_t)(0x0021U)) /*!< Message type for schedmsg_boot_setup2_t */
typedef struct {
    uint32_t  sysflow_block_daddr;                  /*!< Physical DRAM address of the block (size memsize_arm_sysflow_block), from boot_setup1_reply_t */
    uint32_t  cvtable_block_daddr;                  /*!< Physical DRAM address of the block (size memsize_arm_cvtable_block), from boot_setup1_reply_t */
    uint32_t  printf_string_block_daddr;            /*!< Physical DRAM address of the block (size memsize_printf_string_block) from boot_setup1_reply_t */
    uint32_t  arm_printf_buffer_info_daddr;         /*!< Physical DRAM address of the cvprintf_buffer_info_t associated with this ARM-A53 */
    uint32_t  arm_cvtask_debug_buffer_daddr;        /*!< Physical DRAM address of the ARM/CVTask Debug Buffer requested from boot_setup1_reply_t. \n*/
                                                    /*!< If 0, then no buffer was allocated */
    uint32_t  arm_cvtask_debug_buffer_wridx_daddr;  /*!< Physical DRAM address of the write-index associated with the ARM/CVTask Debug Buffer.    \n*/
                                                    /*!< If 0, then no buffer was allocated */
    uint32_t  arm_sched_debug_buffer_daddr;         /*!< Physical DRAM address of the ARM/Sched Debug Buffer requested from boot_setup1_reply_t.  \n*/
                                                    /*!< If 0, then no buffer was allocated */
    uint32_t  arm_sched_debug_buffer_wridx_daddr;   /*!< Physical DRAM address of the write-index associated with the ARM/Sched Debug Buffer.     \n*/
                                                    /*!< If 0, then no buffer was allocated */
    uint32_t  arm_perf_debug_buffer_daddr;          /*!< Physical DRAM address of the ARM/Perf Debug Buffer requested from boot_setup1_reply_t.   \n*/
                                                    /*!< If 0, then no buffer was allocated */
    uint32_t  arm_perf_debug_buffer_wridx_daddr;    /*!< Physical DRAM address of the write-index associated with the ARM/Perf Debug Buffer.     \n*/
                                                    /*!< If 0, then no buffer was allocated */
    uint32_t  arm_idsp_picinfo_queue_daddr;         /*!< Physical DRAM address of the IDSP->VISORC picinfo queue */
    uint32_t  arm_idsp_async_queue_daddr;           /*!< Physical DRAM address of the IDSP->VISORC async queue  */
    uint32_t  arm_extra_picinfo_queue_daddr;        /*!< Physical DRAM address of the ARM-A53->VISORC picinfo queue*/
    uint32_t  arm_vin0_queue_daddr;                 /*!< Physical DRAM address of the VIN0->VISORC queue; added for RADAR data */
    uint32_t  arm_vin1_queue_daddr;                 /*!< Physical DRAM address of the VIN1->VISORC queue; added for RADAR data */
    uint32_t  arm_armvin_queue_daddr;               /*!< Physical DRAM address of the ARMVIN->VISORC queue; added for RADAR data */
    uint32_t  arm_idsp1_picinfo_queue_daddr;        /*!< Physical DRAM address of the IDSP1->VISORC picinfo queue */
} schedmsg_boot_setup2_t;

/*! @struct schedmsg_boot_setup2_reply_t
 *
 * @brief Boot sequence, setup-2 reply message
 *
 * This message conveys the locations of the sysflow tables and cvtables loaded by
 * the Vision Driver into the areas provided by schedmsg_boot_setup2_t.
 */
#define SCHEDMSG_BOOT_SETUP2_REPLY              ((uint16_t)(0x0021U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_boot_setup2_reply_t */
typedef struct {
    uint32_t  sysflow_table_num_sets;                                     /*!< Number of sysflow tables loaded*/
    uint32_t  sysflow_table_set_numlines[SUPERDAG_MAX_SYSFLOW_PER_ARM];   /*!< Number of lines in each associated sysflow table (0 if none)*/
    uint32_t  sysflow_table_set_daddr[SUPERDAG_MAX_SYSFLOW_PER_ARM];      /*!< (sysflow_entry_t *) Physical DRAM address of the sysflow table loaded by the Vision Driver. */

    uint32_t  cvtable_num_sets;                                           /*!< Number of cvtables loaded */
    uint32_t  cvtable_daddr[SUPERDAG_MAX_CVTABLE_PER_ARM];                /*!< Physical DRAM address of each CVTable. */
} schedmsg_boot_setup2_reply_t;

/*! @struct schedmsg_boot_setup3_t
 *
 * @brief Boot sequence, setup-3 message
 *
 * This message conveys the combined and processed cvtable and sysflow table.
 */
#define SCHEDMSG_BOOT_SETUP3                    ((uint16_t)(0x0022U))   /*!< Mesage type for schedmsg_boot_setup3_t */
typedef struct {
    uint32_t  sysflow_numlines;       /*!< Number of total lines associated with the combined sysflow tables.\n*/
                                      /*!< If 0, the no SuperDAG sysflow table is used.*/
    uint32_t  sysflow_name_daddr;     /*!< (sysflow_name_entry_t *) Physical DRAM address of the sysflow_name_table \n*/
                                      /*!< If 0, the no SuperDAG sysflow table is used.*/
    uint32_t  cvtable_numlines;       /*!< Number of total entries in the master cvtable toc. \n*/
                                      /*!< If 0, no CVTables are loaded by SuperDAG or AutoRUN */
    uint32_t  cvtable_base_daddr;     /*!< (cvtable_entry_t *) Physical DRAM address of the master cvtable toc \n*/
                                      /*!< If 0, no CVTables are loaded by SuperDAG or AutoRUN */
    uint32_t  sysflow_table_daddr;    /*!< (sysflow_entry_t *) Physical DRAM address of the combined and distilled sysflow table \n*/
                                      /*!< If 0, the no SuperDAG sysflow table is used.*/
} schedmsg_boot_setup3_t;

/*! @struct schedmsg_boot_setup3_reply_t
 *
 * @brief Boot sequence, setup-3 reply message
 *
 * This message is a simple acknowledge that the ARM-A53 has completed booting.
 */
#define SCHEDMSG_BOOT_SETUP3_REPLY              ((uint16_t)(0x0022U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_boot_setup3_reply_t */
typedef struct {
    uint32_t  dummy_ack;              /*!< Dummy content, for structure */
} schedmsg_boot_setup3_reply_t;

/*! @struct schedmsg_time_report_t
 *
 * @brief Reports time to CVScheduler to insert as a perf event
 *
 * This message is used to report time from the ARM to the CVScheduler
 */
#define SCHEDMSG_TIME_REPORT                    ((uint16_t)(0x00A5U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_time_report_t */
typedef struct {
    uint32_t  linux_epoch_time;       /*!< Epoch time (seconds since 1/1/1970) */
    uint32_t  linux_mstime;           /*!< Number of milliseconds at capture time */
    uint32_t  linux_event_time;       /*!< Linux event time (CV6 - audclk/64; others - audclk) */
} schedmsg_time_report_t;

/*================================================================================================*/
/* SuperDAG related bootup sequence messages                                                      */
/*================================================================================================*/

/** ==============================================================================================
 *  ==============================================================================================
 * Bootup phase 0 - Discovery and Initial setup
 * : messages 0x0000 - 0x00FF : Messages to ARM
 * : messages 0x8000 - 0x80FF : Replies to ORC
 * ===============================================================================================
 * ============================================================================================ */

/*! @struct schedmsg_initial_setup_t
 *
 *  @brief Initial setup message, this is broadcast to all attached CVSubSchedulers
 */
#define SCHEDMSG_INITIAL_SETUP                  ((uint16_t)(0x0000U)) /*!< Message type for schedmsg_initial_setup_t */
typedef struct {
    uint32_t    orc_major_version;            /*!< CVSCHEDULER_INTERFACE_VERSION reported to ARM */
    uint32_t    orc_minor_version;            /*!< CVTASK_API_VERSION reported to ARM */
    uint32_t    CVPrintfBufferInfo_daddr;     /*!< (cvprintf_buffer_info_t *) Physical DRAM address of the cvprintf_buffer_info_t associated with this ARM-A53 */

    uint32_t    sysflow_numlines;             /*!< Number of sysflow lines (redundant from boot_setup3) */
    uint32_t    sysflow_name_table_daddr;     /*!< (sysflow_name_entry_t *) Physical DRAM address of the sysflow_name_table (redundant from boot_setup3) */

    uint32_t    cvtable_master_num_entries;   /*!< Number of entries in master cvtable (redundant from boot_setup3) */
    uint32_t    cvtable_master_daddr;         /*!< (cvtable_entry_t *) Physical DRAM address of the master cvtable (redundant from boot_setup3) */

} schedmsg_initial_setup_t;

/*! @struct schedmsg_initial_setup_reply_t
 *
 *  @brief Reply from attached CVSubScheduler
 */
#define SCHEDMSG_INITIAL_SETUP_REPLY            ((uint16_t)(0x0000U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_initial_setup_reply_t */
typedef struct {
    uint32_t      arm_major_version;          /*!< CVSCHEDULER_INTERFACE_VERSION reported by ARM */
    uint32_t      arm_minor_version;          /*!< CVTASK_API_VERSION reported by ARM */
    uint32_t      arm_num_cvtask;             /*!< Number of CVTasks compiled into ARM */
} schedmsg_initial_setup_reply_t;

/*! @struct schedmsg_set_bootup_phase_t
 *
 *  @brief sets boot phase for the CVSubScheduler
 *
 *  This is primarily used to enable callbacks for the CVSubScheduler
 */
#define SCHEDMSG_SET_BOOTUP_PHASE               ((uint16_t)(0x0001U)) /*!< Message type for schedmsg_set_bootup_phase_t */
typedef struct {
    uint32_t      bootup_phase;   /*!< Bootup phase, values are \n*/
                                  /*!< DISCOVERY_PHASE (0)\n*/
                                  /*!< REGISTRATION_PHASE (1)\n*/
                                  /*!< QUERY_PHASE (2)\n*/
                                  /*!< INITIALIZATION_PHASE (3)\n*/
                                  /*!< ACTIVE_PHASE (4)\n*/
                                  /*!< SHUTDOWN_PHASE (5)*/
#define DISCOVERY_PHASE                 0U  /*!< Discovery Phase for schedmsg_set_bootup_phase_t::bootup_phase */
#define REGISTRATION_PHASE              1U  /*!< Registration Phase for schedmsg_set_bootup_phase_t::bootup_phase */
#define QUERY_PHASE                     2U  /*!< Query Phase for schedmsg_set_bootup_phase_t::bootup_phase */
#define INITIALIZATION_PHASE            3U  /*!< Initialization Phase for schedmsg_set_bootup_phase_t::bootup_phase */
#define ACTIVE_PHASE                    4U  /*!< Active Phase for schedmsg_set_bootup_phase_t::bootup_phase */
#define SHUTDOWN_PHASE                  5U  /*!< Shutdown Phase for schedmsg_set_bootup_phase_t::bootup_phase */
} schedmsg_set_bootup_phase_t;

/** ==============================================================================================
 *  ==============================================================================================
 * Bootup phase 1 - Registration phase
 * : messages 0x0100 - 0x01FF : Messages to ARM
 * : messages 0x8100 - 0x81FF : Replies to ORC
 * ===============================================================================================
 * ============================================================================================ */


/** ==============================================================================================
 *  ==============================================================================================
 * Bootup phase 2 - CVTask Query phase
 * : messages 0x0200 - 0x02FF : Messages to ARM
 * : messages 0x8200 - 0x82FF : Replies to ORC
 * ===============================================================================================
 * ============================================================================================ */

/*! @struct schedmsg_cvscheduler_memory_report_t
 *
 *  @brief Reports memory locations to the CVSubScheduler during startup.
 */
#define SCHEDMSG_CVSCHEDULER_REPORT_MEMORY      ((uint16_t)(0x0201U)) /*!< Message type for schedmsg_cvscheduler_memory_report_t */
typedef struct {
    uint32_t    sysflow_numlines;             /*!< Number of sysflow lines (redundant from boot_setup3) */
    uint32_t    sysflow_name_table_daddr;     /*!< (sysflow_name_entry_t *) Physical DRAM address of the sysflow_name_table (redundant from boot_setup3) */

    uint32_t    cvtable_master_num_entries;   /*!< Number of entries in master cvtable (redundant from boot_setup3) */
    uint32_t    cvtable_master_daddr;         /*!< (cvtable_entry_t *) Physical DRAM address of the master cvtable (redundant from boot_setup3) */

    uint32_t    orc_cached_baseaddr;          /*!< base DRAM address of the orc-cached region */
    uint32_t    orc_cached_size;              /*!< size of orc-cached region */

    uint32_t    orc_uncached_baseaddr;        /*!< base DRAM address of the orc-uncached region */
    uint32_t    orc_uncached_size;            /*!< size of orc-uncached region */

    uint32_t    armblock_baseaddr;            /*!< base DRAM address of the ARM block region */
    uint32_t    armblock_size;                /*!< size of ARM block region */

    uint32_t    CVTaskMsgPool_daddr;          /*!< (cvmem_messagepool_t *) Physical DRAM address of CVTask Message Pool allocated for CVSubScheduler */

    uint32_t    CVTaskInfoTable_daddr;        /*!< (component_build_info_t *) Physical DRAM address for CVTask buildinfo */
    uint32_t    SchedInfoTable_daddr;         /*!< (component_build_info_t *) Physical DRAM address for CVScheduler component buildinfo */

} schedmsg_cvscheduler_memory_report_t;

/*! @struct schedmsg_configure_idsp_t
 *
 *  @brief Used to configure an IDSP channel.
 *
 *  As the VISORC has no direct connection to the IDSP, this has to be echoed
 *  through the Vision Driver.
 */
#define SCHEDMSG_CONFIGURE_IDSP                 ((uint16_t)(0x0202U)) /*!< Message type for schedmsg_configure_idsp_t */
typedef struct {
    uint32_t    channel_id;         /*!< Channel ID to configure */
    uint32_t    num_buffers;        /*!< Number of buffers associated with this channel_id */
    uint32_t    picbuffer_size;     /*!< Size of picbuffers */
    uint32_t    picbuffer_daddr[MAX_IDSP_BUFFERS];  /*!< (char *) Physical DRAM addresses of the picbuffer */
} schedmsg_configure_idsp_t;

/*! @struct schedmsg_configure_vin_t
 *
 *  @brief Used to configure an VIN channel.
 *
 *  As the VISORC has no direct connection to the IDSP, this has to be echoed
 *  through the Vision Driver.
 */
#define SCHEDMSG_CONFIGURE_VIN                  ((uint16_t)(0x0203U)) /*!< Message type for schedmsg_configure_vin_t */
typedef struct {
    uint32_t    vin_id;             /*!< Channel ID to configure */
    uint32_t    line_width;         /*!< Expected width of a given line */
    uint32_t    line_pitch;         /*!< Expected pitch of a given line */
    uint32_t    frame_height;       /*!< Expected height of a given capture */
    uint32_t    frame_size;         /*!< Expected size of a given capture */
    uint32_t    num_buffers;        /*!< Number of buffers associated with this channel_id */
    uint32_t    picbuffer_size;     /*!< Size of picbuffers */
    uint32_t    picbuffer_daddr[MAX_RADAR_BUFFERS];  /*!< (char *) Physical DRAM addresses of the picbuffer */
} schedmsg_configure_vin_t;

/** ==============================================================================================
 *  ==============================================================================================
 * Bootup phase 3 - Initialization phase
 * : messages 0x0300 - 0x03FF : Messages to ARM
 * : messages 0x8300 - 0x83FF : Replies to ORC
 * ===============================================================================================
 * ============================================================================================ */

/** ==============================================================================================
 *  ==============================================================================================
 * Bootup phase 4 - Run phase
 * : messages 0x0400 - 0x04FF : Messages to ARM
 * : messages 0x8400 - 0x84FF : Replies to ORC
 * ===============================================================================================
 * ============================================================================================ */

#define SCHEDMSG_CVSCHEDULER_ACTIVE             ((uint16_t)(0x04F0U)) /*!< Message to Vision Driver to denote that it is active */
#define SCHEDMSG_CVSCHEDULER_RUN                ((uint16_t)(0x04FFU | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message from Vision Driver to enable CVScheduler */

/*! @struct schedmsg_wake_feeders_t
 *
 *  @brief Message to from ARM to force feeder wakes.
 *
 *  This can either be a general, or targeted wake.
 */
#define SCHEDMSG_WAKE_FEEDERS                   ((uint16_t)(0x04B0U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_wake_feeders_t */
typedef struct {
    uint32_t  targeted_wake;      /*!< If zero, all feeders are woken, otherwise only target_uuid is woken */
    uint32_t  target_uuid;        /*!< Target CVTask's UUID to wake, assuming targeted_wake is non-zero */
} schedmsg_wake_feeders_t;

/*! @struct schedmsg_cvlog_update_t
 *
 *  @brief Message to ARM to flush the printf buffers out to file.
 *
 *  There are two different versions - a general update, and one after startup
 *  completes.  The reason for this is to allow the Vision Driver to know which
 *  file to dump data to.
 */
#define SCHEDMSG_VISORC_PRINTF_UPDATE           ((uint16_t)(0x04E0U)) /*!< Message type for schedmsg_cvlog_update_t */
#define SCHEDMSG_VISORC_STARTUP_UPDATE          ((uint16_t)(0x04E1U)) /*!< Message type for schedmsg_cvlog_update_t */
typedef struct {
    uint32_t    CVStaticLogInfo_daddr;  /*!< (cvlogger_state_t *) Physical DRAM address of logger state, used for flushing debug buffers */
} schedmsg_cvlog_update_t;


/*================================================================================================*/
/* CVTask related messages, generic (used in SuperDAG and FlexiDAG modes)                         */
/*================================================================================================*/

/*! @struct schedmsg_cvtask_register_t
 *
 *  @brief Request to CVSubScheduler to register an ARM CVTask
 */
#define SCHEDMSG_CVTASK_REGISTER                ((uint16_t)(0x0100U)) /*!< Message type for schedmsg_cvtask_register_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVREG_REQUEST         ((uint16_t)(0x1800U)) /*!< Message type for schedmsg_cvtask_register_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */
} schedmsg_cvtask_register_t;

/*! @struct schedmsg_cvtask_register_reply_t
 *
 *  @brief Reply from CVSubScheduler after registering an ARM CVTask
 */
#define SCHEDMSG_CVTASK_REGISTER_REPLY          ((uint16_t)(0x0100U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_register_reply_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVREG_REPLY           ((uint16_t)(0x1800U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_register_reply_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */
    char        cvtask_name[NAME_MAX_LENGTH];   /*!< CVTask Name */
    uint32_t    arm_cvtask_type;                /*!< CVTask Type - must be CVTASK_TYPE_ARM_(*) */
    uint32_t    has_finish_method;              /*!< If non zero, this CVTask has a finisher associated with it */
} schedmsg_cvtask_register_reply_t;

/*! @struct schedmsg_cvtask_query_t
 *
 *  @brief Request to CVSubScheduler to query an ARM CVTask.
 */
#define SCHEDMSG_CVTASK_QUERY_REQUEST           ((uint16_t)(0x0200U)) /*!< Message type for schedmsg_cvtask_query_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST       ((uint16_t)(0x1820U)) /*!< Message type for schedmsg_cvtask_query_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */

    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    cvtask_instance;                /*!< CVTask instance */

    uint32_t    cvtask_memory_interface_daddr;  /*!< (cvtask_memory_interface_t *) Location to store results */

    uint32_t    config_data[SYSFLOW_MAX_CONFIG_DATA]; /*!< Array of configuration data to provide to query */

    uint8_t     cvtask_debug_level;             /*!< Debug level for call */
    uint8_t     unused_alignment_8byte[7];      /*!< Structure element alignment */

} schedmsg_cvtask_query_t;

/*! @struct schedmsg_cvtask_query_reply_t
 *
 *  @brief Reply from CVSubScheduler after querying an ARM CVTask
 */
#define SCHEDMSG_CVTASK_QUERY_REPLY             ((uint16_t)(0x0200U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_query_reply_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVQUERY_REPLY         ((uint16_t)(0x1820U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_query_reply_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */

    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    cvtask_instance;                /*!< CVTask instance */

    uint32_t    cvtask_memory_interface_daddr;  /*!< (cvtask_memory_interface_t *) shadowed from query request*/
} schedmsg_cvtask_query_reply_t;

/*! @struct schedmsg_cvtask_getbuildinfo_t
 *
 *  @brief Request to CVSubScheduler to call an arm-cvtask_getinfo for build/tool info
 */
#define SCHEDMSG_CVTASK_GET_BUILDINFO           ((uint16_t)(0x0280U)) /*!< Message type for schedmsg_cvtask_getbuildinfo_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST      ((uint16_t)(0x1810U)) /*!< Message type for schedmsg_cvtask_getbuildinfo_t in FlexiDAG mode */
#define ARM_CVTASK_INDEX_SCHEDULER      (0xFFFFFFFFU) /*!< Special index for schedmsg_cvtask_getbuildinfo_t; see schedmsg_cvtask_getbuildinfo_t::arm_cvtask_index */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index.  If this is set to ARM_CVTASK_INDEX_SCHEDULER, the CVSubScheduler information will be returned */
    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    versionstring_maxsize;          /*!< Maximum length of version and tool strings */
    uint32_t    daddr_versionstring_location;   /*!< (char *) Physical DRAM address to copy version string to */
    uint32_t    daddr_toolstring_location;      /*!< (char *) Physical DRAM address to copy tool string to */
    uint32_t    daddr_component_location;       /*!< (char *) Physical DRAM address to copy component string to */
} schedmsg_cvtask_getbuildinfo_t;

/*! @struct schedmsg_cvtask_getbuildinfo_reply_t
 *
 *  @brief Reply from CVSubScheduler after calling arm-cvtask_getinfo for build/tool info
 *
 *  Note that the values should be a shadow copy from what was sent in
 *  schedmsg_cvtask_getbuildinfo_t
 */
#define SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY     ((uint16_t)(0x0280U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_getbuildinfo_reply_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVGETVER_REPLY        ((uint16_t)(0x1810U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_getbuildinfo_reply_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index.  If this is set to ARM_CVTASK_INDEX_SCHEDULER, the CVSubScheduler information will be returned */
    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    versionstring_maxsize;          /*!< Maximum length of version and tool strings */
    uint32_t    daddr_versionstring_location;   /*!< (char *) Physical DRAM address to copy version string to */
    uint32_t    daddr_toolstring_location;      /*!< (char *) Physical DRAM address to copy tool string to */
    uint32_t    daddr_component_location;       /*!< (char *) Physical DRAM address to copy component string to */
} schedmsg_cvtask_getbuildinfo_reply_t;

/*! @struct schedmsg_cvtask_init_t
 *
 *  @brief Request to CVSubScheduler to call an arm-cvtask_init.
 */
#define SCHEDMSG_CVTASK_INIT_REQUEST            ((uint16_t)(0x0300U)) /*!< Message type for schedmsg_cvtask_init_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVINIT_REQUEST        ((uint16_t)(0x1830U)) /*!< Message type for schedmsg_cvtask_init_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */

    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    cvtask_instance;                /*!< CVTask instance */

    uint8_t     cvtask_debug_level;             /*!< Debug level for job */
    uint8_t     cvtask_num_custom_message_pool; /*!< Number of custom message pools */
    uint8_t     unused_alignment_8byte[6];      /*!< Alignment for structure */

    uint32_t    config_data[SYSFLOW_MAX_CONFIG_DATA];   /*!< Array of config data values*/

    uint32_t    Instance_private_storage_size;    /*!< Size of instance private storage for CVTask */
    uint32_t    Instance_private_storage_daddr;   /*!< (void *)Physical DRAM address of instance private storage for CVTask */

    uint32_t    CVTaskCustomMsgPool_daddr[CVTASK_MAX_CVTASKMSG_POOL]; /*!< (cvmem_messagepool_t *) Array of Physical addresses for CVTask Message Pools */

} schedmsg_cvtask_init_t;

/*! @struct schedmsg_cvtask_init_reply_t
 *
 *  @brief Reply from CVSubScheduler upon completion of an arm-cvtask_init.
 *
 *  Return code from init is stored in hdr.message_retcode
 */
#define SCHEDMSG_CVTASK_INIT_REPLY              ((uint16_t)(0x0300U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_init_reply_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVINIT_REPLY          ((uint16_t)(0x1830U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_init_reply_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */

    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    cvtask_instance;                /*!< CVTask instance */

} schedmsg_cvtask_init_reply_t;

/*! @struct schedmsg_cvtask_run_t
 *
 *  @brief Command to CVSubScheduler to run an ARM CVTask
 *
 *  This has multiple versions (RUN, MSGONLY, FINISH), as this structure is shared
 *  across multiple run modes.
 */
#define SCHEDMSG_CVTASK_RUN_REQUEST             ((uint16_t)(0x0400U))   /*!< Message type for schedmsg_cvtask_run_t in SuperDAG mode */
#define SCHEDMSG_CVTASK_MSGONLY_REQUEST         ((uint16_t)(0x0401U))   /*!< Message type for schedmsg_cvtask_run_t in SuperDAG mode */
#define SCHEDMSG_CVTASK_FINISH_REQUEST          ((uint16_t)(0x0404U))   /*!< Message type for schedmsg_cvtask_run_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_REQUEST         ((uint16_t)(0x1840U))   /*!< Message type for schedmsg_cvtask_run_t in FlexiDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST     ((uint16_t)(0x1850U))   /*!< Message type for schedmsg_cvtask_run_t in FlexiDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST     ((uint16_t)(0x1860U))   /*!< Message type for schedmsg_cvtask_run_t in FlexiDAG mode */
typedef struct {
    uint32_t    arm_cvtask_index;               /*!< ARM CVTask Index */

    uint16_t    sysflow_index;                  /*!< Sysflow index */
    uint16_t    cvtask_instance;                /*!< CVTask instance */

    uint32_t    scheduler_tasknode_id;          /*!< CVTaskNode identifier, must be passed ot run_reply / run_partial_reply */

    uint16_t    cvtask_frameset_id;             /*!< Frameset_id of job */
    uint8_t     cvtask_debug_level;             /*!< Debug level for job */
    uint8_t     cvtask_num_custom_message_pool; /*!< Number of custom message pools */

    uint32_t    task_deadline;                  /*!< Deadline of CVTask (priority)*/
    uint32_t    task_deadline_fail;             /*!< Failure deadline of CVTask  */

    uint8_t     cvtask_num_inputs;              /*!< Number of inputs for CVTask */
    uint8_t     cvtask_num_feedback;            /*!< Number of feedbacks for CVTask */
    uint8_t     cvtask_num_outputs;             /*!< Number of outputs for CVTask */
    uint8_t     cvtask_num_messages;            /*!< Number of messages for CVTask */

    uint32_t    Instance_private_storage_size;    /*!< Size of instance private storage for CVTask */
    uint32_t    Instance_private_storage_daddr;   /*!< (void *)Physical DRAM address of instance private storage for CVTask */

    uint32_t    InputMemBuf_daddr[CVTASK_MAX_INPUTS];       /*!< (cvmem_membuf_t *) Array of Physical DRAM addresses for CVTask inputs */
    uint32_t    FeedbackMemBuf_daddr[CVTASK_MAX_FEEDBACK];  /*!< (cvmem_membuf_t *) Array of Physical DRAM addresses for CVTask feedbacks */
    uint32_t    OutputMemBuf_daddr[CVTASK_MAX_OUTPUTS];     /*!< (cvmem_membuf_t *) Array of Physical DRAM addresses for CVTask outputs */
    uint32_t    CVTaskMessage_daddr[CVTASK_MAX_MESSAGES];   /*!< (cvmem_messagebuf_t *) Array of Physical DRAM addresses for CVTask messages */
    uint32_t    CVTaskCustomMsgPool_daddr[CVTASK_MAX_CVTASKMSG_POOL]; /*!< (cvmem_messagepool_t *) Array of Physical addresses for CVTask Message Pools */

    uint32_t    frameset_basetime;                /*!< Frameset basetime */

    uint32_t    ignore_invalidate_flags;          /*!< Ignore invalidation for incoming CVTask data.  Bitmasked as \n */
                                                  /*!< bit  0.. bit 15 : ignore invalidations on inputs [0..15] \n */
                                                  /*!< bit 28.. bit 31 : ignore invalidations on feedbacks[0..3] */
} schedmsg_cvtask_run_t;

/*! @struct schedmsg_cvtask_run_reply_t
 *
 *  @brief Marks an ARM CVTask as complete
 */
#define SCHEDMSG_CVTASK_RUN_REPLY               ((uint16_t)(0x0400U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_reply_t in SuperDAG mode */
#define SCHEDMSG_CVTASK_MSGONLY_REPLY           ((uint16_t)(0x0401U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_reply_t in SuperDAG mode */
#define SCHEDMSG_CVTASK_FINISH_REPLY            ((uint16_t)(0x0404U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_reply_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_REPLY           ((uint16_t)(0x1840U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_reply_t in FlexiDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_MSG_REPLY       ((uint16_t)(0x1850U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_reply_t in FlexiDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_FIN_REPLY       ((uint16_t)(0x1860U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_reply_t in FlexiDAG mode */
typedef struct {
    uint32_t    scheduler_tasknode_id;      /*!< CVTaskNode identifier */
} schedmsg_cvtask_run_reply_t;

/*! @struct schedmsg_cvtask_run_partial_reply_t 
 *
 *  @brief Marks an output from an ARM CVTask as completed early
 *
 *  This partial completion is used to allow downstream tasks that rely on an output
 *  from a CVTask that completes early.
 */
#define SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY       ((uint16_t)(0x0410U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_partial_reply_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVRUN_PARTIAL_REPLY   ((uint16_t)(0x1841U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cvtask_run_partial_reply_t in FlexiDAG mode */
typedef struct {
    uint32_t    scheduler_tasknode_id;      /*!< CVTaskNode identifier */
    uint32_t    output_index;               /*!< output index to mark as complete */
    uint32_t    output_membuf_daddr;        /*!< DRAM address of output buffer */
} schedmsg_cvtask_run_partial_reply_t;

/*================================================================================================*/
/* CVTask support related messages, generic (used in SuperDAG and FlexiDAG modes)                 */
/*================================================================================================*/

/*! @struct schedmsg_attach_cvtask_message_t
 *
 *  @brief Attaches a CVTask message to a target frameset_id.
 *
 *  Target and source sysflow indexes are carried within the cvmem_messagebuf_t
 */
#define SCHEDMSG_CVTASK_MSG_REPLY               ((uint16_t)(0x0301U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_cvtask_message_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVTASK_MSG            ((uint16_t)(0x1900U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_cvtask_message_t in FlexiDAG mode */
typedef struct {
    uint32_t    CVTaskMessage_daddr;      /*!< (cvmem_messagebuf_t *)Physical DRAM address of the message to attach */
    uint32_t    target_frameset_id;       /*!< Target frameset_id to attach to.  Valid values are \n*/
                                          /*!< 0x0000-0xFFFF : frameset_id                        \n*/
                                          /*!< 0xFFFFFFFF    : ASAP (next frameset_id for target) */
} schedmsg_attach_cvtask_message_t;

/*! @struct schedmsg_attach_time_cvtask_message_t
 *
 *  @brief Attaches a CVTask message at a target time (in audio ticks).
 *
 *  Target and source sysflow indexes are carried within the cvmem_messagebuf_t
 */
#define SCHEDMSG_CVTASK_MSG_AT_TIME_REPLY       ((uint16_t)(0x0302U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_time_cvtask_message_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_CVTASK_MSG_AT_TIME    ((uint16_t)(0x1901U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_time_cvtask_message_t in FlexiDAG mode */
typedef struct {
    uint32_t    CVTaskMessage_daddr;      /*!< (cvmem_messagebuf_t *)Physical DRAM address of the message to attach */
    uint32_t    target_frametime;         /*!< Target time to attach message at */
} schedmsg_attach_time_cvtask_message_t;

/*! @struct schedmsg_attach_bulk_cvtask_message_t
 *
 *  @brief Sends multiple CVTask Messages from a source task to multiple targets
 *
 *  There are two versions - bulk (targets a frame), and tokenized (based on an IDSP
 *  token)
 */
#define SCHEDMSG_SEND_BULK_CVTASK_MSG_TO_FRAME        ((uint16_t)(0x0402U | MSGTYPEBIT_ARM_TO_VISORC))          /*!< Message type for schedmsg_attach_bulk_cvtask_message_t in SuperDAG mode */
#define SCHEDMSG_SEND_TOKENIZED_CVTASK_MSG_TO_FRAME   ((uint16_t)(0x0403U | MSGTYPEBIT_ARM_TO_VISORC))          /*!< Message type for schedmsg_attach_bulk_cvtask_message_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_SEND_BULK_CVTASK_MSG_TO_FRAME       ((uint16_t)(0x1902U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_bulk_cvtask_message_t in FlexiDAG mode */
#define SCHEDMSG_FLEXIDAG_SEND_TOKENIZED_CVTASK_MSG_TO_FRAME  ((uint16_t)(0x1903U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_bulk_cvtask_message_t in FlexiDAG mode */
typedef struct {
    uint32_t    num_cvtask_messages;      /*!< number of messages (must be <= BULK_MESSAGE_MAX_NUM, see cvtask_coredef.h) */
    uint32_t    target_frameset_id;       /*!< Either target frameset_id, or IDSP token based on message_type */
    uint32_t    alignment_64bit;          /*!< Unnecessary alignment; to be removed in CVAPIv7 */

    uint32_t    CVTaskMessage_daddr[BULK_MESSAGE_MAX_NUM];  /*!< (cvmem_messagebuf_t *)Array of Physical DRAM addresses of messages */
    uint32_t    message_size[BULK_MESSAGE_MAX_NUM];         /*!< Array of message sizes */
    uint16_t    target_sysflow_index[BULK_MESSAGE_MAX_NUM]; /*!< Array of message targets */
} schedmsg_attach_bulk_cvtask_message_t;

/*! @struct schedmsg_attach_private_msg_t
 *
 *  @brief Attaches a private 224-byte message to the next run of a target CVTask
 *
 *  This message is a one-time use message, and is erased upon consumption
 */
#define SCHEDMSG_SEND_PRIVATE_MSG               ((uint16_t)(0x040FU | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_private_msg_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG      ((uint16_t)(0x190FU | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_attach_private_msg_t in FlexiDAG mode */
typedef struct {
    uint32_t  target_sysflow_index;     /*!< Target sysflow index for message */
    uint32_t  reserved0;                /*!< Reserved for alignment/expansion */
    uint32_t  reserved1;                /*!< Reserved for alignment/expansion */
    uint32_t  reserved2;                /*!< Reserved for alignment/expansion */
    uint8_t   message[224];             /*!< Message payload */
} schedmsg_attach_private_msg_t;

/*! @struct schedmsg_set_debug_level_t
 *
 *  @brief Used to set the debug level of multiple SFB entries
 */
#define SCHEDMSG_SET_DEBUG_LEVEL                ((uint16_t)(0x04C0U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_debug_level_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_SET_DEBUG_LEVEL       ((uint16_t)(0x1910U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_debug_level_t in FlexiDAG mode */
#define SET_DEBUG_LEVEL_MAX_ENTRIES             56U             /*!< Maximum number of levels settable by a single message */
typedef struct {
    uint32_t  num_entries;              /*!< Number of entries to set.  Valid values are \n*/
                                        /*!< 0 .. SET_DEBUG_LEVEL_MAX_ENTRIES : Sets that many entries  \n*/
                                        /*! CVTASK_SET_DEBUG_LEVEL_ALL (cvtask_api.h) : Sets all entries to target_debug_level[0] */
    uint16_t  target_sysflow_index[SET_DEBUG_LEVEL_MAX_ENTRIES];    /*!< Array of target sysflow indexes. */
    uint16_t  target_debug_level[SET_DEBUG_LEVEL_MAX_ENTRIES];      /*!< Array of debug levels            */
} schedmsg_set_debug_level_t;

/*! @struct schedmsg_set_frequency_t
 *
 *  @brief Used to set multiple SFB Entry's frequencies immediately
 */
#define SCHEDMSG_SET_FREQUENCY                  ((uint16_t)(0x04C1U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_frequency_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_SET_FREQ              ((uint16_t)(0x1920U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_frequency_t in FlexiDAG mode */
#define SET_FREQUENCY_MAX_ENTRIES               56U     /*!< Maximum number of frequencies settable by a single set_freq type message */
typedef struct {
    uint32_t  num_entries;              /*!< Number of entries to set.  Valid values are \n*/
                                        /*!< 0 .. SET_FREQUENCY_MAX_ENTRIES : Sets that many entries  \n*/
                                        /*!< CVTASK_SET_FREQUENCY_ALL (cvtask_api.h) : Sets all entries to frequency[0] */
    uint16_t  sysflow_index[SET_FREQUENCY_MAX_ENTRIES]; /*!< Array of target sysflow indexes */
    uint16_t  frequency[SET_FREQUENCY_MAX_ENTRIES];     /*!< Array of frequencies.  0xFF is on-demand */
} schedmsg_set_frequency_t;

/*! @struct schedmsg_set_frequency_at_frame_t
 *
 *  @brief Used to set multiple SFB Entry's frequencies at a target frameset_id
 */
#define SCHEDMSG_SET_FREQUENCY_AT_FRAME         ((uint16_t)(0x04C2U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_frequency_at_frame_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_SET_FREQ_AT_FRAME     ((uint16_t)(0x1921U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_frequency_at_frame_t in FlexiDAG mode */
typedef struct {
    uint32_t  num_entries;              /*!< Number of entries to set.  Valid values are \n*/
                                        /*!< 0 .. SET_FREQUENCY_MAX_ENTRIES : Sets that many entries  \n*/
                                        /*!< CVTASK_SET_FREQUENCY_ALL (cvtask_api.h) : Sets all entries to frequency[0] */
    uint32_t  target_frameset_id;       /*!< Target frameset_id to change frequency at.  Valid values are \n*/
                                        /*!< 0x0000-0xFFFF : frameset_id                        \n*/
                                        /*!< 0xFFFFFFFF    : ASAP (next frameset_id for target) */
    uint16_t  sysflow_index[SET_FREQUENCY_MAX_ENTRIES]; /*!< Array of target sysflow indexes */
    uint16_t  frequency[SET_FREQUENCY_MAX_ENTRIES];     /*!< Array of frequencies.  0xFF is on-demand */
} schedmsg_set_frequency_at_frame_t;

/*! @struct schedmsg_set_frequency_at_time_t
 *
 *  @brief Used to set multiple SFB Entry's frequencies at a target time
 */
#define SCHEDMSG_SET_FREQUENCY_AT_TIME          ((uint16_t)(0x04C3U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_frequency_at_time_t in SuperDAG mode */
#define SCHEDMSG_FLEXIDAG_SET_FREQ_AT_TIME      ((uint16_t)(0x1922U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_set_frequency_at_time_t in FlexiDAG mode */
typedef struct {
    uint32_t  num_entries;              /*!< Number of entries to set.  Valid values are \n*/
                                        /*!< 0 .. SET_FREQUENCY_MAX_ENTRIES : Sets that many entries  \n*/
                                        /*!< CVTASK_SET_FREQUENCY_ALL (cvtask_api.h) : Sets all entries to frequency[0] */
    uint32_t  target_frametime;         /*!< Target time (in audio ticks) to change frequency at */
    uint16_t  sysflow_index[SET_FREQUENCY_MAX_ENTRIES]; /*!< Array of target sysflow indexes */
    uint16_t  frequency[SET_FREQUENCY_MAX_ENTRIES];     /*!< Array of frequencies.  0xFF is on-demand */
} schedmsg_set_frequency_at_time_t;

/*================================================================================================*/
/* Cavalry system :                                                                               */
/*================================================================================================*/

/*! @struct schedmsg_cavalry_setup_t
 *
 *  @brief : Setup command for Cavalry internal CVTask
 *
 *  All Cavalry-related calls are optional, and are only supported if the Cavalry
 *  internal CVTasks are not compiled in.  This function will still accept the setup
 *  request if received.
 */
#define SCHEDMSG_CAVALRY_SETUP                  ((uint16_t)(0x0470U | MSGTYPEBIT_ARM_TO_VISORC))    /*!< Message type for schedmsg_cavalry_setup_t */
typedef struct {
    uint32_t    all_base_daddr;         /*!< CAV : Defines the physical DRAM address base of memory */
    uint32_t    all_size;               /*!< CAV : Defines the size of DRAM associated with Cavalry */
    uint32_t    cmd_base_daddr;         /*!< CAV : Defines the base of the Cavalry Command Block */
    uint32_t    cmd_size;               /*!< CAV : Defines the size of the Cavalry Command Block */
    uint32_t    msg_base_daddr;         /*!< CAV : Defines the base of the Cavalry Message Block */
    uint32_t    msg_size;               /*!< CAV : Defines the size of the Cavalry Message Block */
    uint32_t    log_base_daddr;         /*!< CAV : Defines the base of the Cavalry Log Block */
    uint32_t    log_size;               /*!< CAV : Defines the size of the Cavalry Log Block */
    uint32_t    ucode_base_daddr;       /*!< CAV : Defines the base of the Cavalry Code Block */
    uint32_t    ucode_size;             /*!< CAV : Defines the size of the Cavalry Code Block */
    uint32_t    user_base_daddr;        /*!< CAV : Defines the base of the Cavalry User Block */
    uint32_t    user_size;              /*!< CAV : Defines the base of the Cavalry User Block */
    uint32_t    hotlink_slot0_offset;   /*!< CAV : Defines the offset of the Cavalry Hotlink Slot 0 */
    uint32_t    hotlink_slot1_offset;   /*!< CAV : Defines the offset of the Cavalry Hotlink Slot 1 */
    uint32_t    hotlink_slot2_offset;   /*!< CAV : Defines the offset of the Cavalry Hotlink Slot 2 */
    uint32_t    hotlink_slot3_offset;   /*!< CAV : Defines the offset of the Cavalry Hotlink Slot 3 */
    uint8_t     log_level;              /*!< CAV : Defines the debug level for the Cavalry log */
    uint8_t     reserved0[3];           /*!< CAV : Reserved for uint32_t alignment */
} schedmsg_cavalry_setup_t;

/*================================================================================================*/
/* Cavalry/VP                                                                                     */
/*================================================================================================*/

/*! @struct schedmsg_cavalry_vp_run_request_t
 *
 *  @brief Cavalry/VP request
 *
 *  As this is a blocking call in the kernel, a function token will be used as a way
 *  to identify the semaphore/block that is associated with this call.
 */
#define SCHEDMSG_CAVALRY_VP_RUN_REQUEST         ((uint16_t)(0x047CU | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cavalry_vp_run_request_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token to return when completed */
    uint32_t    priority;                     /*!< Priority of request */
    uint32_t    cavalry_pid;                  /*!< Process-ID of the caller (for perf logging) */
    uint32_t    frame_no;                     /*!< Frame number associated with call (for perf logging) */
    uint32_t    unused2;                      /*!< Reserved for expansion */
    uint32_t    unused3;                      /*!< Reserved for expansion */
    uint32_t    parameter_daddr;              /*!< (struct dag_run_cmd*) Physical DRAM address of run command (from cvapi_cavalry.h) */
    uint32_t    local_printf_buffer_daddr;    /*!< [DEPRECATED] : superseded by contents of schedmsg_cavalry_setup_t */
    uint32_t    local_printf_buffer_size;     /*!< [DEPRECATED] : superseded by contents of schedmsg_cavalry_setup_t */
} schedmsg_cavalry_vp_run_request_t;

/*! @struct schedmsg_cavalry_vp_run_reply_t
 *
 *  @brief Cavalry/VP run reply
 *
 *  This returns the result of the vp run that was completed.  If Cavalry/VP is not
 *  installed, this will return ERR_CAVALRY_VP_UNAVAILABLE in function_retcode.
 */
#define SCHEDMSG_CAVALRY_VP_RUN_REPLY           ((uint16_t)(0x047CU)) /*!< Message type for schedmsg_cavalry_vp_run_reply_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token associated with this run request (from SCHEDMSG_CAVALRY_VP_RUN_REQUEST)*/
    uint32_t    function_retcode;             /*!< Return code of call */
    uint32_t    parameter_daddr;              /*!< (struct dag_run_cmd*) Physical DRAM address of run command (from cvapi_cavalry.h) */
} schedmsg_cavalry_vp_run_reply_t;

/*! @struct schedmsg_cavalry_hl_run_request_t
 *
 *  @brief Cavalry/Hotlink run request
 *
 *  As this is a blocking call in the kernel, a function token will be used as a way
 *  to identify the semaphore/block that is associated with this call.
 *  An error will be returned in the SCHEDMSG_CAVALRY_HL_RUN_REPLY if the Cavalry/VP
 *  cvtask is not installed.
 */
#define SCHEDMSG_CAVALRY_HL_RUN_REQUEST         ((uint16_t)(0x047DU | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_cavalry_hl_run_request_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token to return when completed */
    uint32_t    priority;                     /*!< Priority of request */
    uint32_t    cavalry_pid;                  /*!< Process-ID of the caller (for perf logging) */
    uint32_t    frame_no;                     /*!< Frame number associated with call (for perf logging) */
    uint32_t    slot_number;                  /*!< Hotlink slot to use (0..3) */
    uint32_t    unused3;                      /*!< Reserved for expansion */
    uint32_t    parameter_daddr;              /*!< Parameters to pass into the hotlink call */
    uint32_t    local_printf_buffer_daddr;    /*!< [DEPRECATED] : superseded by contents of schedmsg_cavalry_setup_t */
    uint32_t    local_printf_buffer_size;     /*!< [DEPRECATED] : superseded by contents of schedmsg_cavalry_setup_t */
} schedmsg_cavalry_hl_run_request_t;

/*! @struct schedmsg_cavalry_hl_run_reply_t
 *
 *  @brief Cavalry/Hotlink run request
 *
 *  This returns the result of the hotlink run that was completed.  If Cavalry/VP is
 *  not installed, this will return ERR_CAVALRY_VP_UNAVAILABLE in function_retcode.
 */
#define SCHEDMSG_CAVALRY_HL_RUN_REPLY           ((uint16_t)(0x047DU)) /*!< Message type for schedmsg_cavalry_hl_run_reply_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token associated with this hotlink request (from SCHEDMSG_CAVALRY_HL_RUN_REQUEST) */
    uint32_t    function_retcode;             /*!< Return code of call */
    uint32_t    parameter_daddr;              /*!< Parameters to pass into the hotlink call */
    uint32_t    slot_retcode;                 /*!< Return code from hotlink call */
    uint32_t    assigned_time;                /*!< Time (in audio ticks) when function was assigned to scheduler */
    uint32_t    started_time;                 /*!< Time (in audio ticks) when function was started by CVFramework */
    uint32_t    finished_time;                /*!< Time (in audio ticks) when function was completed by CVFramework */
} schedmsg_cavalry_hl_run_reply_t;

/*================================================================================================*/
/* Cavalry/FEX                                                                                    */
/*================================================================================================*/

/*! @struct schedmsg_cavalry_fex_query_request_t
 *
 *  @brief Cavalry/FEX query request
 *
 *  As this is a blocking call in the kernel, a function token will be used as a way
 *  to identify the semaphore/block that is associated with this call.
 */
#define SCHEDMSG_CAVALRY_FEX_QUERY_REQUEST      ((uint16_t)(0x0460U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cavalry_fex_query_request_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token to return when completed */
    fex_query_cmd_t cmd;                      /*!< FEX query command to use */
    uint32_t    cavalry_pid;                  /*!< Process-ID of the caller (for perf logging) */
    uint32_t    frame_no;                     /*!< Frame number associated with call (for perf logging) */
} schedmsg_cavalry_fex_query_request_t;

/*! @struct schedmsg_cavalry_fex_query_reply_t
 *
 *  @brief Cavalry/FEX query reply
 *
 *  This returns the result of the FEX/Query that was completed.  If Cavalry/FEX is
 *  not installed, this will return ERR_CAVALRY_FEX_UNAVAILABLE in hdr.message_retcode
 */
#define SCHEDMSG_CAVALRY_FEX_QUERY_REPLY        ((uint16_t)(0x0460U))   /*!< Message type for schedmsg_cavalry_fex_query_reply_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token associated with this fex/query request (from SCHEDMSG_CAVALRY_FEX_QUERY_REQUEST) */
    fex_query_msg_t msg;                      /*!< FEX query command to used; contains return values from call */
} schedmsg_cavalry_fex_query_reply_t;

/*! @struct schedmsg_cavalry_fex_run_request_t
 *
 *  @brief Cavalry/FEX run request
 *
 *  As this is a blocking call in the kernel, a function token will be used as a way
 *  to identify the semaphore/block that is associated with this call.
 */
#define SCHEDMSG_CAVALRY_FEX_RUN_REQUEST        ((uint16_t)(0x0461U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cavalry_fex_run_request_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token to return when completed */
    fex_run_cmd_t cmd;                        /*!< FEX run command to use */
    uint32_t    cavalry_pid;                  /*!< Process-ID of the caller (for perf logging) */
    uint32_t    frame_no;                     /*!< Frame number associated with call (for perf logging) */
} schedmsg_cavalry_fex_run_request_t;

/*! @struct schedmsg_cavalry_fex_run_reply_t
 *
 *  @brief Cavalry/FEX run reply
 *
 *  This returns the result of the FEX/Run that was completed.  If Cavalry/FEX is
 *  not installed, this will return ERR_CAVALRY_FEX_UNAVAILABLE in hdr.message_retcode.
 */
#define SCHEDMSG_CAVALRY_FEX_RUN_REPLY          ((uint16_t)(0x0461U))   /*!< Message type for schedmsg_cavalry_fex_run_reply_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token associated with this fex/run request (from SCHEDMSG_CAVALRY_FEX_RUN_REQUEST) */
    fex_run_msg_t msg;                        /*!< FEX run command used; may be modified by call */
} schedmsg_cavalry_fex_run_reply_t;

/*================================================================================================*/
/* Cavalry/FMA                                                                                    */
/*================================================================================================*/

/*! @struct schedmsg_cavalry_fma_query_request_t
 *
 *  @brief Cavalry/FMA query request
 *
 *  As this is a blocking call in the kernel, a function token will be used as a way
 *  to identify the semaphore/block that is associated with this call.
 */
#define SCHEDMSG_CAVALRY_FMA_QUERY_REQUEST      ((uint16_t)(0x0468U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_cavalry_fma_query_request_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token to return when completed */
    fma_query_cmd_t cmd;                      /*!< FMA query command to use */
    uint32_t    cavalry_pid;                  /*!< Process-ID of the caller (for perf logging) */
    uint32_t    frame_no;                     /*!< Frame number associated with call (for perf logging) */
} schedmsg_cavalry_fma_query_request_t;

/*! @struct schedmsg_cavalry_fma_query_reply_t
 *
 *  @brief Cavalry/FMA query reply
 *
 *  This returns the result of the FMA/Query that was completed.  If Cavalry/FMA is
 *  not installed, this will return ERR_CAVALRY_FMA_UNAVAILABLE in hdr.message_retcode
 */
#define SCHEDMSG_CAVALRY_FMA_QUERY_REPLY        ((uint16_t)(0x0468U)) /*!< Message type for schedmsg_cavalry_fma_query_reply_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token associated with this FMA/query request (from SCHEDMSG_CAVALRY_FMA_QUERY_REQUEST) */
    fma_query_msg_t msg;                      /*!< FMA query command used; may be modified by call */
} schedmsg_cavalry_fma_query_reply_t;

/*! @struct schedmsg_cavalry_fma_run_request_t
 *
 *  @brief Cavalry/FMA run request
 *
 *  As this is a blocking call in the kernel, a function token will be used as a way
 *  to identify the semaphore/block that is associated with this call.
 */

#define SCHEDMSG_CAVALRY_FMA_RUN_REQUEST        ((uint16_t)(0x0469U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_cavalry_fma_run_request_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token to return when completed */
    fma_run_cmd_t cmd;                        /*!< FMA run command to use */
    uint32_t    cavalry_pid;                  /*!< Process-ID of the caller (for perf logging) */
    uint32_t    frame_no;                     /*!< Frame number associated with call (for perf logging) */
} schedmsg_cavalry_fma_run_request_t;

/*! @struct schedmsg_cavalry_fma_run_reply_t
 *
 *  @brief Cavalry/FMA run reply
 *
 *  This returns the result of the FMA/Run that was completed.  If Cavalry/FMA is
 *  not installed, this will return ERR_CAVALRY_FMA_UNAVAILABLE in hdr.message_retcode
 */
#define SCHEDMSG_CAVALRY_FMA_RUN_REPLY          ((uint16_t)(0x0469U)) /*!< Message type for schedmsg_cavalry_fma_run_reply_t */
typedef struct {
    uint32_t    requested_function_token;     /*!< Function token associated with this FMA/run request (from SCHEDMSG_CAVALRY_FMA_RUN_REQUEST) */
    fma_run_msg_t msg;                        /*!< FMA run command used; may be modified by call */
} schedmsg_cavalry_fma_run_reply_t;

/*================================================================================================*/
/* FlexiDAG system :                                                                              */
/* This uses the "message_id" field in the VISARM communication header as a way to convey the     */
/*   flexidag_slot_id.  The value will be "0x0100 + flexidag_slot_id" for FlexiDAG messages.      */
/*================================================================================================*/

/*! @struct schedmsg_flexidag_create_request_t
 *
 *  @brief Message to request the creation of a FlexiDAG slot.
 *
 *  This has a corresponding response (schedmsg_flexidag_create_reply_t) when the
 *  create has been processed.
 */
#define SCHEDMSG_FLEXIDAG_CREATE_REQUEST        ((uint16_t)(0x1000U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_flexidag_create_request_t */
typedef struct {
    uint32_t  flexidag_token;           /*!< Token to return to the Vision Driver when the create operation completes */
} schedmsg_flexidag_create_request_t;

/*! @struct schedmsg_flexidag_create_reply_t
 *
 *  @brief Message response once the creation of a FlexiDAG slot completes.
 *
 *  This has is the response generated by the CVScheduler once the creation has
 *  completed.  This also uses the hdr.message_retcode to return any error codes
 *  that may have been generated by the create request.
*/
#define SCHEDMSG_FLEXIDAG_CREATE_REPLY          ((uint16_t)(0x01000U)) /*!< Message type for schedmsg_flexidag_create_reply_t */
typedef struct {
    uint32_t  flexidag_token;           /*!< Token returned from create request */
    uint32_t  flexidag_slot_id;         /*!< Slot ID that the FlexiDAG slot is create */
    uint32_t  master_trace_daddr;       /*!< (flexidag_trace_t *) Physical DRAM address for the FlexiDAG system's trace area */
    uint32_t  slot_trace_daddr;         /*!< (flexidag_slot_trace_t *) Physical DRAM address for the created slot's trace area */
} schedmsg_flexidag_create_reply_t;

/*! @struct schedmsg_flexidag_open_request_t
 *
 *  @brief Open request for a FlexiDAG slot.
 *
 *  This message is sent to FlexiDAG slot, with information about the associated
 *  FlexiBIN, and various configuration values, as defined by flexidag_set_parameter.
 */
#define SCHEDMSG_FLEXIDAG_OPEN_REQUEST          ((uint16_t)(0x1100U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_flexidag_open_request_t */
typedef struct {
    uint32_t  flexidag_token;           /*!< Token to return to the Vision Driver when the open operation completes */

    uint32_t  flexidag_binary_daddr;    /*!< (flexibin file) Physical DRAM address for the flexibin file associated with the slot.*/
    uint32_t  reserved1[4];             /*!< Alignment reserved */

    uint32_t  arm_cvmsg_num;            /*!< Number of CVTask Messages reserved for the ARM CVTasks */
    uint32_t  flexidag_cvmsg_num;       /*!< Number of CVTask Messages reserved for the VISORC CVTasks */
    uint32_t  arm_cvtask_string_size;   /*!< CVTask string size required for ARM print bufs */
    uint32_t  reserved2[5];             /*!< Alignment reserved */

    uint32_t  visorc_perf_log_entry_num[SYSTEM_MAX_NUM_VISORC];   /*!< Number of VISORC/Perf Debug Logs. Only index [0] is used. */
    uint32_t  visorc_sched_log_entry_num[SYSTEM_MAX_NUM_VISORC];  /*!< Number of VISORC/Sched Debug Logs.  Only index [0] is used. */
    uint32_t  visorc_cvtask_log_entry_num[SYSTEM_MAX_NUM_VISORC]; /*!< Number of VISORC/CVTask Debug Logs.  Only index [0] is used. */
    uint32_t  arm_perf_log_entry_num[SYSTEM_MAX_NUM_CORTEX];      /*!< Number of ARM/Perf Debug Logs.  Only index [0] is used. */
    uint32_t  arm_sched_log_entry_num[SYSTEM_MAX_NUM_CORTEX];     /*!< Number of ARM/Sched Debug Logs.  Only index [0] is used. */
    uint32_t  arm_cvtask_log_entry_num[SYSTEM_MAX_NUM_CORTEX];    /*!< Number of ARM/CVTask Debug Logs.  Only index [0] is used. */

    uint32_t  additional_tbar_daddr[FLEXIDAG_MAX_TBAR];           /*!< Additional TBAR files to associate with the slot. */
    uint32_t  additional_sfb_daddr[FLEXIDAG_MAX_SFB];             /*!< Additional SFB files to associate with the slot. */
    uint16_t  additional_sfb_numlines[FLEXIDAG_MAX_SFB];          /*!< Number of lines per SFB file */

} schedmsg_flexidag_open_request_t;

/*! @struct schedmsg_flexidag_open_reply_t
 *
 *  @brief Open reply for a FlexiDAG slot
 *
 *  This is sent once the open has fully parsed.  It returns the amount of memory
 *  required for state, temp, and output buffers.
 */
#define SCHEDMSG_FLEXIDAG_OPEN_REPLY            ((uint16_t)(0x1100U)) /*!< Message type for schedmsg_flexidag_open_reply_t */
typedef struct {
    uint32_t  flexidag_token;                             /*!< Token from SCHEDMSG_FLEXIDAG_OPEN_REQUEST */
    uint32_t  flexidag_num_outputs;                       /*!< Number of outputs used by this FlexiDAG slot */
    uint32_t  flexidag_memsize_temp;                      /*!< Size of temp buffer (in bytes) */
    uint32_t  flexidag_memsize_state;                     /*!< Size of state buffer (in bytes) */
    uint32_t  flexidag_output_size[FLEXIDAG_MAX_OUTPUTS]; /*!< Size of each output buffer (in bytes) */
} schedmsg_flexidag_open_reply_t;

/*! @struct schedmsg_flexidag_setup_request_t
 *
 *  @brief Setup request sent from VISORC to ARM to request information
 */
#define SCHEDMSG_FLEXIDAG_SETUP_REQUEST         ((uint16_t)(0x1101U)) /*!< Message type for schedmsg_flexidag_setup_request_t */
typedef struct {
    uint32_t  flexidag_token;             /*!< Token from previous SCHEDMSG_FLEXIDAG_OPEN_REQUEST */
} schedmsg_flexidag_setup_request_t;

/*! @struct schedmsg_flexidag_setup_reply_t
 *
 *  @brief Setup response sent from ARM to VISORC
 */
#define SCHEDMSG_FLEXIDAG_SETUP_REPLY           ((uint16_t)(0x1101U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_flexidag_setup_reply_t */
typedef struct {
    uint32_t  flexidag_token;             /*!< Token from previous SCHEDMSG_FLEXIDAG_OPEN_REQUEST */
    uint32_t  num_cvtasks;                /*!< Number of ARM CVTasks loaded into application */
} schedmsg_flexidag_setup_reply_t;

/*! @struct schedmsg_flexidag_open_parsed_t
 *
 *  @brief Message from VISORC to ARM to pass information parsed from the FlexiBIN file.
 */
#define SCHEDMSG_FLEXIDAG_OPEN_PARSED           ((uint16_t)(0x1002U)) /*!< Message type for schedmsg_flexidag_open_parsed_t */
/* [TODO] Should be 0x1102, fix in CVAPI-v7 */
typedef struct {
    uint32_t  sysflow_num_entries;      /*!< Number of entries in for this FlexiDAG slot's sysflow table */
    uint32_t  sysflow_daddr;            /*!< (sysflow_entry_t *) Physical DRAM address of the combined and distilled sysflow table for this FlexiDAG slot */
    uint32_t  sysflow_name_daddr;       /*!< (sysflow_name_entry_t *) Physical DRAM address of the sysflow name table for this FlexiDAG slot */

    uint32_t  cvtable_num_entries;      /*!< Number of total entries in the master cvtable toc for this FlexiDAG slot. \n*/
                                        /*!< If 0, no CVTables are available for this FlexiDAG slot */
    uint32_t  cvtable_daddr;            /*!< (cvtable_entry_t *) Physical DRAM address of the master cvtable toc for this FlexiDAG slot. \n*/
                                        /*!< If 0, no CVTables are available for this FlexiDAG slot */

    uint32_t  flexidag_name_daddr;      /*!< (char *) Physical DRAM address of this FlexiDAG's name, read from the FlexiBIN file. */
    uint32_t  max_outstanding_runs;     /*!< Maximum outstanding runs able to be absorbed by this FlexiDAG slot. */

} schedmsg_flexidag_open_parsed_t;

/*! @struct schedmsg_flexidag_init_request_t
 *
 *  @brief Starts the init phase for a FlexiDAG slot
 *
 *  The init phase requires that the state buffer and temp buffer be allocated.
 *  This also sends in the callback tokens for each output
 */
#define SCHEDMSG_FLEXIDAG_INIT_REQUEST          ((uint16_t)(0x1200U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_flexidag_init_request_t */
typedef struct {
    uint32_t  flexidag_token;                 /*!< Token associated with this flexidag_init command */
    uint32_t  flexidag_state_buffer_daddr;    /*!< (void *)Physical DRAM address for the FlexiDAG's state buffer.  This value must remain constant \n*/
                                              /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_temp_buffer_daddr;     /*!< (void *)Physical DRAM address for the FlexiDAG's temp buffer.  This value may change between runs */
                                              /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_callback_id[FLEXIDAG_MAX_OUTPUTS]; /*!< Array of Callback token IDs for each output */
} schedmsg_flexidag_init_request_t;

/*! @struct schedmsg_flexidag_init_reply_t
 *
 *  @brief Command for flexidag_init complete.
 *
 *  This uses the hdr.message_retcode field to relay any errors that occur during the
 *  flexidag_init request.
 */
#define SCHEDMSG_FLEXIDAG_INIT_REPLY            ((uint16_t)(0x1200U)) /*!< Message type for schedmsg_flexidag_init_reply_t */
typedef struct {
    uint32_t  flexidag_token;                 /*!< Token from previous SCHEDMSG_FLEXIDAG_INIT_REQUEST */
} schedmsg_flexidag_init_reply_t;

/*! @struct schedmsg_flexidag_init_partitioned_t
 *
 *  @brief Returns relevant pointers that are partitioned from the state buffer.
 *
 *  Additional side note - for all "daddr" in this structure, if it is set to zero,
 *  No buffer is allocated for that particular field.
 */
#define SCHEDMSG_FLEXIDAG_INIT_PARTITIONED      ((uint16_t)(0x1201U)) /*!< Message type for schedmsg_flexidag_init_partitioned_t */
typedef struct {
    uint32_t  visorc_perf_log_ctrl_daddr[SYSTEM_MAX_NUM_VISORC];    /*!< Array of Physical DRAM address for each VISORC/Perf Debug Buffer for this FlexiDAG slot */
    uint32_t  visorc_sched_log_ctrl_daddr[SYSTEM_MAX_NUM_VISORC];   /*!< Array of Physical DRAM address for each VISORC/Sched Debug Buffer for this FlexiDAG slot */
    uint32_t  visorc_cvtask_log_ctrl_daddr[SYSTEM_MAX_NUM_VISORC];  /*!< Array of Physical DRAM address for each VISORC/CVTask Debug Buffer for this FlexiDAG slot */

    uint32_t  arm_cvmsg_ctrl_daddr[SYSTEM_MAX_NUM_CORTEX];          /*!< Array of Physical DRAM address for each ARM CVMessage control structure */

    uint32_t  arm_linfo_daddr[SYSTEM_MAX_NUM_CORTEX];               /*!< Array of Physical DRAM address for each ARM's loginfo control structure */
    uint32_t  arm_perf_log_ctrl_daddr[SYSTEM_MAX_NUM_CORTEX];       /*!< Array of Physical DRAM address for each ARM/Perf Debug Buffer for this FlexiDAG slot */
    uint32_t  arm_sched_log_ctrl_daddr[SYSTEM_MAX_NUM_CORTEX];      /*!< Array of Physical DRAM address for each ARM/Sched Debug Buffer for this FlexiDAG slot */
    uint32_t  arm_cvtask_log_ctrl_daddr[SYSTEM_MAX_NUM_CORTEX];     /*!< Array of Physical DRAM address for each ARM/CVTask Debug Buffer for this FlexiDAG slot */

    uint32_t  visorc_codebase[SYSTEM_MAX_NUM_VISORC];               /*!< Array of Physical DRAM address for the VISORC codebase.  Only index [0] is used. */
    uint32_t  visorc_codesize[SYSTEM_MAX_NUM_VISORC];               /*!< Array of sizes of each VISORC codebase.  Only index[0] is used. */
    uint32_t  hotlink_blockbase;                                    /*!< Unused field (legacy) */
    uint32_t  hotlink_blocksize;                                    /*!< Unused field (legacy) */
    uint32_t  flexidag_blockbase;                                   /*!< Address of the FlexiDAG slot's code blockbase. */
    uint32_t  flexidag_blocksize;                                   /*!< Size of the FlexiDAG slot's code */
} schedmsg_flexidag_init_partitioned_t;

/*! @struct schedmsg_flexidag_run_request_t
 *
 *  @brief Sends a run request for a FlexiDAG slot.
 *
 *  This message is sent when a FlexiDAG run is needed.  If a FlexiDAG run is made on a
 *  a safety-enabled system, then a pre-run (schedmsg_flexidag_prerun_request_t)
 *  message must be sent prior to this message.
 */
#define SCHEDMSG_FLEXIDAG_RUN_REQUEST           ((uint16_t)(0x1300U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_flexidag_run_request_t */
typedef struct {
    uint32_t  flexidag_token;               /*!< FlexiDAG token associated with the run request */
    uint32_t  flexidag_basetime;            /*!< Basetime for the FlexiDAG run */
    uint32_t  flexidag_state_buffer_daddr;  /*!< (void *)Physical DRAM address for the FlexiDAG's state buffer.  This value must remain constant \n*/
                                            /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_temp_buffer_daddr;   /*!< (void *)Physical DRAM address for the FlexiDAG's temp buffer.  This value may change between runs */
                                            /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_input_buffer_daddr[FLEXIDAG_MAX_INPUTS];       /*!< (void *)Array of physical DRAM addresses for the input buffers \n*/
                                                                      /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_feedback_buffer_daddr[FLEXIDAG_MAX_FEEDBACKS]; /*!< (void *)Array of physical DRAM address for the feedback buffers \n*/
                                                                      /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_output_buffer_daddr[FLEXIDAG_MAX_OUTPUTS];     /*!< (void *)Array of physical DRAM address for the output buffers \n*/
                                                                      /*!< If 0, then no buffer was allocated */
    uint32_t  flexidag_input_buffer_size[FLEXIDAG_MAX_INPUTS];        /*!< Array of input buffer sizes */
    uint32_t  flexidag_feedback_buffer_size[FLEXIDAG_MAX_FEEDBACKS];  /*!< Array of feedback buffer sizes */
    uint32_t  flexidag_output_buffer_size[FLEXIDAG_MAX_OUTPUTS];      /*!< Array of output buffer sizes */
} schedmsg_flexidag_run_request_t;

/*! @struct schedmsg_flexidag_run_reply_t
 *
 *  @brief Reply for a completed FlexiDAG run.
 *
 *  This message is sent when a FlexiDAG has completed.  If any issue occurs during the
 *  run, the error code will be stored in the hdr.message_retcode field.
 */
#define SCHEDMSG_FLEXIDAG_RUN_REPLY             ((uint16_t)(0x1300U)) /*!< Message type for schedmsg_flexidag_run_reply_t */
typedef struct {
    uint32_t  flexidag_token;               /*!< FlexiDAG token associated with the run request */
    uint32_t  start_time;                   /*!< Time (in audio ticks) that this run was sent to the CVScheduler */
    uint32_t  end_time;                     /*!< Time (in audio ticks) that this run was completed by the CVScheduler*/
    uint32_t  output_not_generated;         /*!< Output not generated bitmask - this is used to denote whether an output was not generated. */
} schedmsg_flexidag_run_reply_t;

/*! @struct schedmsg_flexidag_output_reply_t
 *
 *  @brief Output for a particular FlexiDAG run command has completed
 *
 *  This message is used to denote when an output of a FlexiDAG is available.  This
 *  can also trigger an output callback by the FlexiDAG application, if configured
 *  to do so.
 */
#define SCHEDMSG_FLEXIDAG_OUTPUT_DONE           ((uint16_t)(0x1301U)) /*!< Message type for schedmsg_flexidag_output_reply_t */
typedef struct {
    uint32_t  flexidag_token;               /*!< FlexiDAG token associated with the run request */
    uint32_t  flexidag_output_num;          /*!< Output number that has completed */
    uint32_t  flexidag_callback_token;      /*!< Callback token associated with output, set during flexidag_init stage */
    uint32_t  flexidag_output_daddr;        /*!< (void *) Physical DRAM address of the output buffer */
    uint32_t  flexidag_output_size;         /*!< Size of the output buffer */
    uint32_t  flexidag_output_donetime;     /*!< Time (in audio ticks) when output buffer was completed */
    void     *vpOutputBlk;                  /*!< Special : This value is translated by the Vision Driver Kernel when processed */
} schedmsg_flexidag_output_reply_t;

/*! @struct schedmsg_flexidag_prerun_request_t
 *
 *  @brief FlexiDAG prerun request.  This is used for safety-enabled systems
 *
 *  This particular command is used to transmit the additional FNV-1a based checksums
 *  for any buffer going into a FlexiDAG.  Any buffer coming out of a FlexiDAG needs
 *  to have the membuf_creation_header_t placed at the beginning of the buffer.  This
 *  also uses the hdr.message_retcode field to relay any issues related to processing
 *  this message.
 */
#define SCHEDMSG_FLEXIDAG_PRERUN_REQUEST        ((uint16_t)(0x13F0U | MSGTYPEBIT_ARM_TO_VISORC))  /*!< Message type for schedmsg_flexidag_prerun_request_t */
typedef struct {
    uint32_t  flexidag_token;                                         /*!< FlexiDAG token associated with the run request.  */
    uint32_t  flexidag_input_buffer_csum[FLEXIDAG_MAX_INPUTS];        /*!< FNV-1a of the first 128 bytes of each input buffer */
    uint32_t  flexidag_feedback_buffer_csum[FLEXIDAG_MAX_FEEDBACKS];  /*!< FNV-1a of the first 128 bytes of each feedback buffer */
} schedmsg_flexidag_prerun_request_t;

/*! @struct schedmsg_flexidag_close_request_t
 *
 *  @brief Requests that a FlexiDAG slot be closed.
 *
 *  This request will close once all outstanding CVJobs for a slot are completed.  Once
 *  a close request has been made, no further run commands can be accepted by the slot.
 */
#define SCHEDMSG_FLEXIDAG_CLOSE_REQUEST         ((uint16_t)(0x1400U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_flexidag_close_request_t */
typedef struct {
    uint32_t  flexidag_token;         /*!< Token to return to the Vision Driver when the close operation completes */
} schedmsg_flexidag_close_request_t;

/*! @struct schedmsg_flexidag_close_reply_t
 *
 *  @brief Response sent when a FlexiDAG slot has closed (command SCHEDMSG_FLEXIDAG_CLOSE_REQUEST)
 *
 *  Note that this also makes use of the hdr.message_retcode to return any errors
 *  associated with a close request.
 */
#define SCHEDMSG_FLEXIDAG_CLOSE_REPLY           ((uint16_t)(0x1400U)) /*!< Message type for schedmsg_flexidag_close_reply_t */
typedef struct {
    uint32_t  flexidag_token;         /*!< Token associated with close request, from SCHEDMSG_FLEXIDAG_CLOSE_REQUEST */
} schedmsg_flexidag_close_reply_t;

/*! @struct schedmsg_flexidag_error_request_t
 *
 *  @brief Sends an error message to the ARM-A53, targeting a FlexiDAG application
 *
 *  This will also invoke the FlexiDAG's error handler, if available.  The default
 *  action is to echo to the console.
 */
#define SCHEDMSG_FLEXIDAG_ERROR_MESSAGE         ((uint16_t)(0x1F00U)) /*!< Message type for schedmsg_flexidag_error_request_t */
typedef struct {
    uint32_t  flexidag_slot_id;       /*!< Slot ID associated with this message */
    uint16_t  sysflow_index;          /*!< Sysflow index associated with this message */
    uint16_t  cvtask_frameset_id;     /*!< frameset_id associated with this message */
    uint32_t  cvtask_uuid;            /*!< cvtask_uuid associated with this message */
    uint32_t  retcode;                /*!< retcode associated with this message   */
    uint32_t  format_daddr;           /*!< DRAM address of the error message      */
    uint32_t  reserved_armptr_hi;     /*!< (Reserved) : used by the kernel when converting the string*/
    uint32_t  msg_time;               /*!< Time message was sent                  */
    uint32_t  arg0;                   /*!< ARG0 for message's format string */
    uint32_t  arg1;                   /*!< ARG1 for message's format string */
    uint32_t  arg2;                   /*!< ARG2 for message's format string */
    uint32_t  arg3;                   /*!< ARG3 for message's format string */
    uint32_t  arg4;                   /*!< ARG4 for message's format string */
    uint32_t  format_daddr_raw;       /*!< (CV6) : raw daddr for format string */
    uint32_t  cvcore_id;              /*!< (CV6) : core_id of message */
} schedmsg_flexidag_error_request_t;

/** ==============================================================================================
 * Flexidag CV Interface messages
 * + Uses message_id as an adjusted flexidag_slot_id (0x0100 | flexidag_slot_id)
 * ============================================================================================ */

#define SCHEDMSG_BLOCK_FLEXIDAG_START                         ((uint16_t)(0x1000U)) /*!< Message type block reserved for FlexiDAG messages */
#define SCHEDMSG_BLOCK_FLEXIDAG_END                           ((uint16_t)(0x1FFFU)) /*!< Message type block reserved for FlexiDAG messages */

/*================================================================================================*/
/* Scheduler specific messages, generic (used in SuperDAG and FlexiDAG modes)                     */
/*================================================================================================*/

/*! @struct schedmsg_console_echo_message_t
 *
 *  @brief Sends a message to be echoed on the console
 *
 *  This particular handler is only used during the startup phase, as some of
 *  the required resources may not be available if the bootup fails.  Note that
 *  this copies the physical string into the message.
 */
#define SCHEDMSG_CONSOLE_ECHO_MESSAGE                         ((uint16_t)(0x04EEU)) /*!< Message type for schedmsg_console_echo_message_t */
typedef struct {
    uint32_t  flexidag_slot_id;       /*!< Slot ID associated with this message */
    uint32_t  msg_retcode;            /*!< retcode associated with this message   */
    uint32_t  msg_time;               /*!< Time message was sent                  */
    uint32_t  arg0;                   /*!< ARG0 for message's format string */
    uint32_t  arg1;                   /*!< ARG1 for message's format string */
    uint32_t  arg2;                   /*!< ARG2 for message's format string */
    uint32_t  arg3;                   /*!< ARG3 for message's format string */
    uint32_t  arg4;                   /*!< ARG4 for message's format string */
    char      fmtstring[204];         /*!< Format string of the message */
    uint32_t  zeropad;                /*!< Enforced zero-pad at the end */
} schedmsg_console_echo_message_t;

/*! @struct schedmsg_superdag_error_request_t
 *
 *  @brief Sends an error message to the ARM-A53.
 *
 *  The default handler in the Vision Driver also echoes this to the console.
 */
#define SCHEDMSG_SUPERDAG_ERROR_MESSAGE       ((uint16_t)(0x04EFU))   /*!< Message type for schedmsg_superdag_error_request_t */
typedef struct {
    uint32_t  flexidag_slot_id;       /*!< Slot ID associated with this message */
    uint16_t  sysflow_index;          /*!< Sysflow index associated with this message */
    uint16_t  cvtask_frameset_id;     /*!< frameset_id associated with this message */
    uint32_t  cvtask_uuid;            /*!< cvtask_uuid associated with this message */
    uint32_t  retcode;                /*!< retcode associated with this message   */
    uint32_t  format_daddr;           /*!< DRAM address of the error message      */
    uint32_t  reserved_armptr_hi;     /*!< (Reserved) : used by the kernel when converting the string*/
    uint32_t  msg_time;               /*!< Time message was sent                  */
    uint32_t  arg0;                   /*!< ARG0 for message's format string */
    uint32_t  arg1;                   /*!< ARG1 for message's format string */
    uint32_t  arg2;                   /*!< ARG2 for message's format string */
    uint32_t  arg3;                   /*!< ARG3 for message's format string */
    uint32_t  arg4;                   /*!< ARG4 for message's format string */
} schedmsg_superdag_error_request_t;

/*! @struct schedmsg_scheduler_shutdown_t
 *
 *  @brief Message sent by CVScheduler to inform ARM that it has shut down.
 */
#define SCHEDMSG_SCHEDULER_SHUTDOWN             ((uint16_t)(0x0501U))   /*!< Message type for schedmsg_scheduler_shutdown_t */
typedef struct {
    uint32_t    CVStaticLogInfo_daddr;      /*!< (cvlogger_state_t) Physical DRAM address of the logger control buffers */
    uint32_t    shutdown_retcode;           /*!< Shutdown reason */
} schedmsg_scheduler_shutdown_t;

/*! @struct schedmsg_arm_shutdown_req_t
 *
 *  @brief Message to request the shutdown of the CVScheduler by the ARM-A53 */
#define SCHEDMSG_ARM_REQUESTS_SHUTDOWN          ((uint16_t)(0x0502U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_arm_shutdown_req_t */
typedef struct {
    uint32_t  dummyval;       /*!< Dummy value for message structure */
} schedmsg_arm_shutdown_req_t;

/** ==============================================================================================
 * Specialized messages
 * ============================================================================================ */

/*! @struct schedmsg_softreset_t
 *
 *  @brief This is used to request a soft-reset of a particular CVCore.
 *
 *  The message is sent from the VISORC->ARM-R53, and a reply is sent back.
 *  The CVScheduler keeps track of the outstanding reply, and won't schedule any
 *  CVJobs on a CVCore that has a pending reset request.
 */
#define SCHEDMSG_HWUNIT_RESET                   ((uint16_t)(0x7000U)) /*!< Message type for schedmsg_softreset_t */
#define SCHEDMSG_HWUNIT_RESET_REPLY             ((uint16_t)(0x7000U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_softreset_t */
typedef struct {
    uint32_t  hardware_unit;      /*!< CVCore to reset */
} schedmsg_softreset_t;

/*! @struct schedmsg_invalidate_cache_t
 *
 *  @brief Forces the VISORC to invalidate it's cache.
 *
 *  This is only used for Cavalry/Hotlink slot loading
 */
#define SCHEDMSG_INVALIDATE_CACHE               ((uint16_t)(0x7001U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_invalidate_cache_t */
typedef struct {
    uint32_t  token_id;           /*!< Reserved for later */
    uint32_t  block_vaddr_start;  /*!< Reserved, may be virtual address */
    uint32_t  block_size;         /*!< Reserved, size of block to invalidate */
} schedmsg_invalidate_cache_t;

/*! @struct schedmsg_flush_visprintf_t
 *
 *  @brief Forces a flush of the system printf buffers.
 *
 *  This is needed to ensure that any messages buffered internally for performance are
 *  flushed to DRAM.  This is usually used when transitioning out of bootup, and at
 *  shutdown.  Once a request is made, the reply is sent with the same token_id, as a
 *  confirmation to unblock the ARM-A53.
 */
#define SCHEDMSG_FLUSH_VISPRINTF_REPLY          ((uint16_t)(0x7002U)) /*!< Message type for schedmsg_flush_visprintf_t */
#define SCHEDMSG_FLUSH_VISPRINTF_REQ            ((uint16_t)(0x7002U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_flush_visprintf_t */
typedef struct {
    uint32_t  token_id;           /*!< Used for identifying semaphore for blocking token */
    uint32_t  flush_type;         /*!< Flush type.  Valid values are  \n*/
                                  /*!< 0: PERF only                   \n*/
                                  /*!< !=0: All printf buffers          */
} schedmsg_flush_visprintf_t;

/*! @struct schedmsg_license_update_t
 *
 *  @brief Updates licensing information for the CVTask security system
 */
#define SCHEDMSG_LICENSE_UPDATE                 ((uint16_t)(0x7100U | MSGTYPEBIT_ARM_TO_VISORC)) /*!< Message type for schedmsg_license_update_t */
typedef struct {
    uint8_t   payload[64];          /*!< License payload */
} schedmsg_license_update_t;

/*! @struct schedmsg_set_rewake_time_t
 *
 *  @brief Requests a forced rewake of the CVSched thread by the Vision Driver
 *
 *  This is only used on systems that are not safety-enabled, as those systems do not
 *  have the wake-timeout-cycle support.
 */
#define SCHEDMSG_SET_REWAKE_TIME                ((uint16_t)(0x7F00U)) /*!< Message type for schedmsg_set_rewake_time_t */
typedef struct {
    uint32_t  rewake_time_delta;    /*!< Rewake time delta.  Possible values are \n*/
                                    /*!< 0: Disables any pending rewake request \n*/
                                    /*!< 1-0xFFFFFFFF - rewake time audio ticks */
    uint32_t  sent_time;            /*!< Time message was sent, needed to compensate for time differentials in the CV2/CV22 system */
} schedmsg_set_rewake_time_t;

/*! @struct schedmsg_autorun_status_t
 *
 *  @brief Updates autorun status after the completion of an interval
 */
#define SCHEDMSG_AUTORUN_STATUS_UPDATE          ((uint16_t)(0x7F80U)) /*!< Message type for schedmsg_autorun_status_t */
typedef struct {
    uint32_t  autorun_num_active;                     /*!< Number active autorun CVTasks in the system */
    uint32_t  autorun_trace_daddr;                    /*!< Trace address (format is autorun_trace_t - see schdr_api.h for definition) */
    uint8_t   autorun_numpassed[MAX_AUTORUN_CVTASKS]; /*!< Number passed since last report */
    uint8_t   autorun_numfailed[MAX_AUTORUN_CVTASKS]; /*!< Number failed since last report */
    uint32_t  last_passtime[MAX_AUTORUN_CVTASKS];     /*!< Last time an autorun has passed, in audio ticks */
    uint32_t  last_failtime[MAX_AUTORUN_CVTASKS];     /*!< Last time an autorun has failed, in audio ticks */
    uint32_t  last_failcode[MAX_AUTORUN_CVTASKS];     /*!< Last failure return code */
} schedmsg_autorun_status_t;

/*================================================================================================*/
/* ARM <-> VISORC Message                                                                         */
/*================================================================================================*/

/*! @struct armvis_msg_t
 *
 *  @brief  General structure of communication message between ARM-A53 and VISORC
 *
 *  Note that this structure must always be of size ARMVIS_MESSAGE_SIZE.
 */
typedef struct {
    armvis_msg_hdr_t  hdr;                                                      /*!< header of message */
    union {
        schedmsg_boot_setup1_t                  boot_setup1;                    /*!< Message type SCHEDMSG_BOOT_SETUP1, id 0x00 */
        schedmsg_boot_setup2_t                  boot_setup2;                    /*!< Message type SCHEDMSG_BOOT_SETUP2, id 0x00 */
        schedmsg_boot_setup3_t                  boot_setup3;                    /*!< Message type SCHEDMSG_BOOT_SETUP3, id 0x00 */
        schedmsg_initial_setup_t                initial_setup;                  /*!< Message type SCHEDMSG_INITIAL_SETUP, id 0x00 */
        schedmsg_set_bootup_phase_t             set_bootup_phase;               /*!< Message type SCHEDMSG_SET_BOOTUP_PHASE, id 0x00 */
        schedmsg_cvtask_register_t              cvtask_register_msg;            /*!< Message type SCHEDMSG_CVTASK_REGISTER, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVREG_REQUEST, id 0x100-0x1FF */
        schedmsg_cvtask_query_t                 cvtask_query;                   /*!< Message type SCHEDMSG_CVTASK_QUERY_REQUEST, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST, id 0x100-0x1FF */
        schedmsg_cvtask_getbuildinfo_t          cvtask_getbuildinfo;            /*!< Message type SCHEDMSG_CVTASK_GET_BUILDINFO, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST, id 0x100-0x1FF */
        schedmsg_cvscheduler_memory_report_t    memory_report;                  /*!< Message type SCHEDMSG_CVSCHEDULER_REPORT_MEMORY, id 0x00 */
        schedmsg_configure_idsp_t               idsp_config;                    /*!< Message type SCHEDMSG_CONFIGURE_IDSP, id 0x00 */
        schedmsg_configure_vin_t                vin_config;                     /*!< Message type SCHEDMSG_CONFIGURE_IDSP, id 0x00 */
        schedmsg_cvtask_init_t                  cvtask_init;                    /*!< Message type SCHEDMSG_CVTASK_INIT_REQUEST, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVINIT_REQUEST, id 0x100-0x1FF */
        schedmsg_cvtask_run_t                   cvtask_run;                     /*!< Message type SCHEDMSG_CVTASK_RUN_REQUEST, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_CVTASK_MSGONLY_REQUEST, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_CVTASK_FINISH_REQUEST, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_REQUEST, id 0x100-0x1FF \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST, id 0x100-0x1FF \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST, id 0x100-0x1FF */
        schedmsg_cvlog_update_t                 cvlog_update;                   /*!< Message type SCHEDMSG_VISORC_PRINTF_UPDATE, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_VISORC_STARTUP_UPDATE, id 0x00 */
        schedmsg_scheduler_shutdown_t           shutdown;                       /*!< Message type SCHEDMSG_SCHEDULER_SHUTDOWN, id 0x00 */
        schedmsg_softreset_t                    hw_reset;                       /*!< Message type SCHEDMSG_HWUNIT_RESET, id 0x00 */
        schedmsg_flush_visprintf_t              flush_visprintf_reply;          /*!< Message type SCHEDMSG_FLUSH_VISPRINTF_REQ, id 0x00 */

        schedmsg_boot_setup1_reply_t            reply_boot_setup1;              /*!< Message type SCHEDMSG_BOOT_SETUP1_REPLY, id 0x00 */
        schedmsg_boot_setup2_reply_t            reply_boot_setup2;              /*!< Message type SCHEDMSG_BOOT_SETUP2_REPLY, id 0x00 */
        schedmsg_boot_setup3_reply_t            reply_boot_setup3;              /*!< Message type SCHEDMSG_BOOT_SETUP3_REPLY, id 0x00 */
        schedmsg_initial_setup_reply_t          reply_initial_setup;            /*!< Message type SCHEDMSG_INITIAL_SETUP_REPLY, id 0x00 */
        schedmsg_cvtask_register_reply_t        reply_cvtask_register;          /*!< Message type SCHEDMSG_CVTASK_REGISTER_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVREG_REPLY, id 0x100-0x1FF */
        schedmsg_cvtask_query_reply_t           reply_cvtask_query;             /*!< Message type SCHEDMSG_CVTASK_QUERY_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVQUERY_REPLY, id 0x100-0x1FF */
        schedmsg_cvtask_getbuildinfo_reply_t    reply_cvtask_getbuildinfo;      /*!< Message type SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVGETVER_REPLY, id 0x100-0x1FF */
        schedmsg_cvtask_init_reply_t            reply_cvtask_init;              /*!< Message type SCHEDMSG_CVTASK_INIT_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVINIT_REPLY, id 0x100-0x1FF */
        schedmsg_cvtask_run_reply_t             reply_cvtask_run;               /*!< Message type SCHEDMSG_CVTASK_RUN_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_CVTASK_MSGONLY_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_CVTASK_FINISH_REPLY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_REPLY, id 0x100-0x1FF \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_MSG_REPLY, id 0x100-0x1FF \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_FIN_REPLY, id 0x100-0x1FF */
        schedmsg_cvtask_run_partial_reply_t     reply_cvtask_run_partial;       /*!< Message type SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY, id 0x00  \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVRUN_PARTIAL_REPLY, id 0x100-0x1FF */
        schedmsg_attach_cvtask_message_t        reply_cvtask_msg;               /*!< Message type SCHEDMSG_CVTASK_MSG_REPLY, id 0x00  \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVTASK_MSG, id 0x100-0x1FF */
        schedmsg_attach_time_cvtask_message_t   reply_cvtask_msg_at_time;       /*!< Message type SCHEDMSG_CVTASK_MSG_AT_TIME_REPLY, id 0x00  \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_CVTASK_MSG_AT_TIME, id 0x100-0x1FF */
        schedmsg_attach_bulk_cvtask_message_t   reply_cvtask_bulk_msg;          /*!< Message type SCHEDMSG_SEND_BULK_CVTASK_MSG_TO_FRAME, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SEND_BULK_CVTASK_MSG_TO_FRAME, id 0x100-0x1FF */
        schedmsg_attach_bulk_cvtask_message_t   reply_cvtask_tokenized_msg;     /*!< Message type SCHEDMSG_SEND_TOKENIZED_CVTASK_MSG_TO_FRAME, id 0x00  \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SEND_TOKENIZED_CVTASK_MSG_TO_FRAME, id 0x100-0x1FF */
        schedmsg_attach_private_msg_t           sched_send_private_msg;         /*!< Message type SCHEDMSG_SEND_PRIVATE_MSG, id 0x00  \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG, id 0x100-0x1FF) */
        schedmsg_set_debug_level_t              set_debug_level;                /*!< Message type SCHEDMSG_SET_DEBUG_LEVEL, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SET_DEBUG_LEVEL, id 0x100-0x1FF) */
        schedmsg_set_frequency_t                set_frequency;                  /*!< Message type SCHEDMSG_SET_FREQUENCY, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SET_FREQ, id 0x100-0x1FF */
        schedmsg_set_frequency_at_frame_t       set_frequency_at_frame;         /*!< Message type SCHEDMSG_SET_FREQUENCY_AT_FRAME, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SET_FREQ_AT_FRAME, id 0x100-0x1FF */
        schedmsg_set_frequency_at_time_t        set_frequency_at_time;          /*!< Message type SCHEDMSG_SET_FREQUENCY_AT_TIME, id 0x00 \n*/
                                                                                /*!< Message type SCHEDMSG_FLEXIDAG_SET_FREQ_AT_TIME, id 0x100-0x1FF */
        schedmsg_wake_feeders_t                 wake_feeders;                   /*!< Message type SCHEDMSG_WAKE_FEEDERS, id 0x00 */
        schedmsg_arm_shutdown_req_t             arm_shutdown_req;               /*!< Message type SCHEDMSG_ARM_REQUESTS_SHUTDOWN, id 0x00 */
        schedmsg_softreset_t                    reply_hw_reset;                 /*!< Message type SCHEDMSG_HWUNIT_RESET_REPLY, id 0x00 */
        schedmsg_flush_visprintf_t              flush_visprintf_request;        /*!< Message type SCHEDMSG_FLUSH_VISPRINTF_REPLY, id 0x00 */
        schedmsg_invalidate_cache_t             invalidate_cache_req;           /*!< Message type SCHEDMSG_INVALIDATE_CACHE, id 0x00; CAVALRY-HOTLINK*/
        schedmsg_license_update_t               license_update_req;             /*!< Message type SCHEDMSG_LICENSE_UPDATE, id 0x00 */
        schedmsg_set_rewake_time_t              set_rewake_time;                /*!< Message type SCHEDMSG_SET_REWAKE_TIME, id 0x00 */
        schedmsg_autorun_status_t               autorun_status;                 /*!< Message type SCHEDMSG_AUTORUN_STATUS_UPDATE, id 0x00 */
        schedmsg_console_echo_message_t         console_message;                /*!< Message type SCHEDMSG_CONSOLE_ECHO_MESSAGE, id 0x00 */
        schedmsg_superdag_error_request_t       superdag_error_message;         /*!< Message type SCHEDMSG_SUPERDAG_ERROR_MESSAGE, id 0x00 */
        schedmsg_time_report_t                  time_report;                    /*!< MEssage type SCHEDMSG_TIME_REPORT, id 0x00 */

        /*-= Cavalry interface messages =---------------------------------------------------*/
        schedmsg_cavalry_setup_t                cavalry_setup;                  /*!< Message type SCHEDMSG_CAVALRY_SETUP, id 0x00 */

        schedmsg_cavalry_vp_run_request_t       cavalry_vp_run_request;         /*!< Message type SCHEDMSG_CAVALRY_VP_RUN_REQUEST, id 0x00 */
        schedmsg_cavalry_vp_run_reply_t         cavalry_vp_run_reply;           /*!< Message type SCHEDMSG_CAVALRY_VP_RUN_REPLY, id 0x00 */

        schedmsg_cavalry_hl_run_request_t       cavalry_hotlink_run_request;    /*!< Message type SCHEDMSG_CAVALRY_HL_RUN_REQUEST, id 0x00 */
        schedmsg_cavalry_hl_run_reply_t         cavalry_hotlink_run_reply;      /*!< Message type SCHEDMSG_CAVALRY_HL_RUN_REPLY, id 0x00 */

        schedmsg_cavalry_fex_query_request_t    cavalry_fex_query_request;      /*!< Message type SCHEDMSG_CAVALRY_FEX_QUERY_REQUEST, id 0x00 */
        schedmsg_cavalry_fex_query_reply_t      cavalry_fex_query_reply;        /*!< Message type SCHEDMSG_CAVALRY_FEX_QUERY_REPLY, id 0x00 */

        schedmsg_cavalry_fex_run_request_t      cavalry_fex_run_request;        /*!< Message type SCHEDMSG_CAVALRY_FEX_RUN_REQUEST, id 0x00 */
        schedmsg_cavalry_fex_run_reply_t        cavalry_fex_run_reply;          /*!< Message type SCHEDMSG_CAVALRY_FEX_RUN_REPLY, id 0x00 */

        schedmsg_cavalry_fma_query_request_t    cavalry_fma_query_request;      /*!< Message type SCHEDMSG_CAVALRY_FMA_QUERY_REQUEST, id 0x00 */
        schedmsg_cavalry_fma_query_reply_t      cavalry_fma_query_reply;        /*!< Message type SCHEDMSG_CAVALRY_FMA_QUERY_REPLY, id 0x00 */

        schedmsg_cavalry_fma_run_request_t      cavalry_fma_run_request;        /*!< Message type SCHEDMSG_CAVALRY_FMA_RUN_REQUEST, id 0x00 */
        schedmsg_cavalry_fma_run_reply_t        cavalry_fma_run_reply;          /*!< Message type SCHEDMSG_CAVALRY_FMA_RUN_REPLY, id 0x00 */

        /*-= Flexidag interface messages =--------------------------------------------------*/
        schedmsg_flexidag_create_request_t      flexidag_create_request;        /*!< Message type SCHEDMSG_FLEXIDAG_CREATE_REQUEST, ID 0x00 */
        schedmsg_flexidag_create_reply_t        flexidag_create_reply;          /*!< Message type SCHEDMSG_FLEXIDAG_CREATE_REPLY, ID 0x00 */
        schedmsg_flexidag_open_request_t        flexidag_open_request;          /*!< Message type SCHEDMSG_FLEXIDAG_OPEN_REQUEST, ID 0x100-0x1FF */
        schedmsg_flexidag_open_parsed_t         flexidag_open_parsed;           /*!< Message type SCHEDMSG_FLEXIDAG_OPEN_PARSED, ID 0x100-0x1FF */
        schedmsg_flexidag_open_reply_t          flexidag_open_reply;            /*!< Message type SCHEDMSG_FLEXIDAG_OPEN_REPLY, ID 0x100-0x1FF */
        schedmsg_flexidag_setup_request_t       flexidag_setup_request;         /*!< Message type SCHEDMSG_FLEXIDAG_SETUP_REQUEST, ID 0x100-0x1FF */
        schedmsg_flexidag_setup_reply_t         flexidag_setup_reply;           /*!< Message type SCHEDMSG_FLEXIDAG_SETUP_REPLY, ID 0x100-0x1FF */
        schedmsg_flexidag_close_request_t       flexidag_close_request;         /*!< Message type SCHEDMSG_FLEXIDAG_CLOSE_REQUEST, ID 0x100-0x1FF */
        schedmsg_flexidag_close_reply_t         flexidag_close_reply;           /*!< Message type SCHEDMSG_FLEXIDAG_CLOSE_REPLY, ID 0x100-0x1FF */
        schedmsg_flexidag_init_request_t        flexidag_init_request;          /*!< Message type SCHEDMSG_FLEXIDAG_INIT_REQUEST, ID 0x100-0x1FF */
        schedmsg_flexidag_init_partitioned_t    flexidag_init_partitioned;      /*!< Message type SCHEDMSG_FLEXIDAG_INIT_PARTITIONED, ID 0x100-0x1FF */
        schedmsg_flexidag_init_reply_t          flexidag_init_reply;            /*!< Message type SCHEDMSG_FLEXIDAG_INIT_REPLY, ID 0x100-0x1FF */
        schedmsg_flexidag_run_request_t         flexidag_run_request;           /*!< Message type SCHEDMSG_FLEXIDAG_RUN_REQUEST, ID 0x100-0x1FF */
        schedmsg_flexidag_prerun_request_t      flexidag_prerun_request;        /*!< Message type SCHEDMSG_FLEXIDAG_PRERUN_REQUEST, ID 0x100-0x1FF */
        schedmsg_flexidag_run_reply_t           flexidag_run_reply;             /*!< Message type SCHEDMSG_FLEXIDAG_RUN_REPLY, ID 0x100-0x1FF */
        schedmsg_flexidag_output_reply_t        flexidag_output_reply;          /*!< Message type SCHEDMSG_FLEXIDAG_OUTPUT_DONE, ID 0x100-0x1FF */
        schedmsg_flexidag_error_request_t       flexidag_error_message;         /*!< Message type SCHEDMSG_FLEXIDAG_ERROR_MESSAGE, ID 0x100-0x1FF */
        uint32_t    align_payload[ ((ARMVIS_MESSAGE_SIZE - sizeof(armvis_msg_hdr_t)) + 3U) >> 2U];
    } msg;  /*!< message union */
} armvis_msg_t;


/*==========================================================================================================*/
/*==========================================================================================================*/
/* CVScheduler memory buffer structures                                                                     */
/*==========================================================================================================*/
/*==========================================================================================================*/

/*================================================================================================*/
/*=- Memory Guardbanding Structures -=============================================================*/
/*================================================================================================*/

/**
 *  For safety, many of the memory buffers that are allocated have a guardband
 *  associated with them.  These guardbands are designed to ensure that the buffers do
 *  not write outside of areas that are allocated for them.
 *
 *  Each visorc memory buffer has an integrated guardband feature (both pre and post)
 *  that is used as a quick way to ensure that buffers that are used by an application
 *  do not overwrite before or after their specified space.  The memory buffer's header
 *  also contains some additional information about the memory buffer.  The format is
 *
 * +----------------------------------------------------------+
 * | Pre guardband (0xFEEDBABE)                         (32B) |
 * +----------------------------------------------------------+
 * | Fixed memory buffer header information             (32B) |
 * +----------------------------------------------------------+
 * | Variable memory buffer header information          (32B) |
 * +----------------------------------------------------------+
 * | Post guardband (0x4ADADBOD)                        (32B) |
 * +----------------------------------------------------------+
 * |                                                          |
 * = Payload                                                  =
 * |                                                          |
 * +----------------------------------------------------------+
 */

/*! @def GUARDBAND_PREFIX_NUM
 *
 *  @brief Defines the number of guardband entries in the prefix
 */
#define GUARDBAND_PREFIX_NUM      (7U)            /*!< Number of prefix elements    */

/*! @def GUARDBAND_PREFIX_VALUE
 *
 *  @brief Defines the pattern used in the guardband.  This value is explicitly not
 *         equal to other values used in the system for clarity (0xDEADBEEF for
 *         stack checking, 0x4ADADB0B for the guardband postfix)
 */
#define GUARDBAND_PREFIX_VALUE    (0xFEEDBABEU)   /*!< Pattern for prefix elements  */

/*! @struct cvmem_guardband_prefix_t
 *
 *  @brief Generic guardband prefix
 */
typedef struct {
    uint32_t    guardband_prefix[GUARDBAND_PREFIX_NUM]; /*!< Prefix array                               */
    uint32_t    block_payload_size_in_chunks  :24;      /*!< Size of block payload, in 128 byte chunks  */
    uint32_t    block_header_size_in_chunks   :2;       /*!< Size of block header, in 128 byte chunks   */
    uint32_t    block_type                    :6;       /*!< Type of block (informative)                */
} cvmem_guardband_prefix_t;

/*! @def GUARDBAND_POSTFIX_NUM
 *
 *  @brief Defines the number of guardband entries in the prefix
 */
#define GUARDBAND_POSTFIX_NUM     (7U)            /*!< Number of postfix elements   */

/*! @def GUARDBAND_POSTFIX_VALUE
 *
 *  @brief Defines the pattern used in the guardband.  This value is explicitly not
 *         equal to other values used in the system for clarity (0xDEADBEEF for
 *         stack checking, 0xFEEDBABE for the guardband prefix)
 */
#define GUARDBAND_POSTFIX_VALUE   (0x4ADADB0DU)   /*!< Pattern for postfix elements */

/*! @struct cvmem_guardband_postfix_t
 *
 *  @brief Generic guardband postfix
 */
typedef struct {
    uint32_t    block_payload_size_in_chunks  :24;        /*!< Size of block payload, in 128 byte chunks  */
    uint32_t    block_header_size_in_chunks   :2;         /*!< Size of block header, in 128 byte chunks   */
    uint32_t    block_type                    :6;         /*!< Type of block (informative)                */
    uint32_t    guardband_postfix[GUARDBAND_POSTFIX_NUM]; /*!< Postfix array                              */
} cvmem_guardband_postfix_t;

/*================================================================================================*/
/*=- Generic Memory Block Structure -=============================================================*/
/*================================================================================================*/

/*! @struct cvmem_membuf_fixed_hdr_t
 *
 *  @brief Fixed content portion for the CVTask buffer guardband
 *
 *  The contents of this structure are set once when the buffer is created, and
 *  shall not be modified during runtime.
 */
typedef struct {
    uint32_t  membuf_sysflow_index    :16;        /*!< Sysflow index that uses this membuf as output  */
    uint32_t  unused_alignment        : 7;        /*!<                                                */
    uint32_t  needs_cache_coherency   : 1;        /*!< Marks if buffer needs cache operations on ARM  */
    uint32_t  framework_reserved      : 8;        /*!< Copied from the CVTask IO state                */
    uint16_t  membuf_cvtask_index;                /*!< CVTask Index for this membuf                   */
    uint16_t  membuf_cvtask_instance;             /*!< CVTask Instance for this membuf                */
    uint16_t  membuf_index;                       /*!< Membuf index in this memory buffer set         */
    uint16_t  membufset_num_membuf;               /*!< Number of membuf in memory buffer set          */
    uint32_t  payload_daddr;                      /*!< Payload dram address                           */
    uint32_t  membuf_size;                        /*!< Aligned payload size + header                  */
    uint32_t  payload_size;                       /*!< Actual payload size used                       */
    uint32_t  membuf_baseaddr;                    /*!< Base address of the memory buffer set          */
    uint32_t  pMemBufSet_daddr;                   /*!< Pointer to the owning memory buffer set        */
} cvmem_membuf_fixed_hdr_t;

/*! @struct cvmem_membuf_variable_hdr_t
 *
 *  @brief Variable content portion for the CVTask buffer guardband
 *
 *  The contents of this structure will be updated by the CVScheduler when a cvtask
 *  buffer is created, assigned to an output for writing, released by output once
 *  writing has completed, assigned as an input to a CVTask, and released once the
 *  CVTask has completed.
 *
 *  Note that several variables internal to this are shadowed from the fixed portion.
 */
typedef struct {
    uint32_t  membuf_frameset_id      :16;        /*!< Frameset_id (lsw)                              */
    uint32_t  buf_error_flags         : 8;        /*!< Buffer error flags                             */
    uint32_t  framework_reserved      : 8;        /*!< Copied from the CVTask IO state                */
    uint32_t  membuf_create_time;                 /*!< Time buffer is sent to a cvtask for writing    */
    uint32_t  membuf_write_time;                  /*!< Time buffer is written to by the cvtask        */
    uint32_t  membuf_last_read_time;              /*!< Last time buffer is used as an input           */
    uint32_t  payload_daddr;                      /*!< Payload dram address (shadowed from fixed_hdr) */
    uint32_t  membuf_size;                        /*!< Aligned payload size + header                  */
    uint32_t  payload_size;                       /*!< Payload size (shadowed from fixed header)      */
    uint32_t  structure_alignment;                /*!< Align to 32-bytes                              */
} cvmem_membuf_variable_hdr_t;

/*! @struct cvmem_membuf_t
 *
 *  @brief Guardband structure used for CVTask Buffers.
 *
 *  All memory blocks allocated by the CVScheduler utilizes guardbands between each
 *  memory block.  This guardband description is specific to the CVTask Buffers.
 */
typedef struct {
    cvmem_guardband_prefix_t    prefix;           /*!< substruct: guardband prefix                            */
    cvmem_membuf_fixed_hdr_t    hdr_fixed;        /*!< substruct: fixed content portion of the guardband      */
    cvmem_membuf_variable_hdr_t hdr_variable;     /*!< substruct: variable content portion of the guardband   */
    cvmem_guardband_postfix_t   postfix;          /*!< substruct: guardband postfix                           */
} cvmem_membuf_t;

/*================================================================================================*/
/*=- Generic CV Message Pool Structure -==========================================================*/
/*================================================================================================*/

/*! @struct cvmem_messagepool_variable_t
 *
 *  @brief Variable content portion for the cvtask message pool guardband
 *
 *  The contents of this structure will be updated by the CVScheduler when a cvtask
 *  message is created.  This or finished.  Note that a message can only be created
 *  in a particular block if "message_in_use" and "message_processed" are both false
 *  or both true.
 */
typedef struct {
    uint32_t    wr_index;                 /*!< write index for the next entry in the pool         */
    uint32_t    unused_alignment0;        /*!< Unused, kept to keep size of structure to 32 bytes */
    uint32_t    unused_alignment1;        /*!< Unused, kept to keep size of structure to 32 bytes */
    uint32_t    unused_alignment2;        /*!< Unused, kept to keep size of structure to 32 bytes */
    uint32_t    message_num;              /*!< Number of messages in this pool                    */
    uint32_t    message_size;             /*!< Size of messages in this pool                      */
    uint32_t    message_buffersize;       /*!< Buffer size (message buffer size + guardband size) */
    uint32_t    messagepool_basedaddr;    /*!< Base address of the message pool buffer            */
} cvmem_messagepool_variable_t;

/*! @struct cvmem_messagepool_t
 *
 *  @brief Guardband structure used for CVTask Message Pools
 *
 *  All memory blocks allocated by the CVScheduler utilizes guardbands between each
 *  memory block.  This guardband description is specific to the CVTask Message Pools
 */
typedef struct {
    cvmem_guardband_prefix_t      prefix;                 /*!< substruct: guardband prefix                            */
    char                          owning_cvtask_name[32]; /*!< Owning CVTask Name.  This has a fixed length           */
    cvmem_messagepool_variable_t  hdr_variable;           /*!< substruct: variable content portion of the guardband   */
    cvmem_guardband_postfix_t     postfix;                /*!< substruct: guardband postfix                           */
} cvmem_messagepool_t;

/*================================================================================================*/
/*=- Generic CV Message Block Structure -=========================================================*/
/*================================================================================================*/

/*! @struct cvmem_messagebuf_fixed_hdr_t
 *
 *  @brief Fixed content portion for the cvtask message guardband
 *
 *  The contents of this structure are set once during message pool creation, and
 *  should not be modified during runtime.
 */
typedef struct {
    uint32_t  CVTaskMsgPool_daddr;        /*!< Physical DRAM address of the owning CVTaskMsgPool  */
    uint32_t  message_payload_daddr;      /*!< Physical DRAM address of the message payload       */
    uint32_t  buffer_size;                /*!< Buffer size (message buffer size + guardband size) */
    uint32_t  message_pool_index;         /*!< Index into message pool for this message           */
    uint32_t  unused_alignment0;          /*!< Unused, kept to keep size of structure to 32 bytes */
    uint32_t  unused_alignment1;          /*!< Unused, kept to keep size of structure to 32 bytes */
    uint32_t  unused_alignment2;          /*!< Unused, kept to keep size of structure to 32 bytes */
    uint32_t  unused_alignment3;          /*!< Unused, kept to keep size of structure to 32 bytes */
} cvmem_messagebuf_fixed_hdr_t;

/*! @struct cvmem_messagebuf_variable_hdr_t
 *
 *  @brief Variable content portion for the cvtask message guardband
 *
 *  The contents of this structure will be updated by the CVScheduler when a cvtask
 *  message is created, sent, or finished.  Note that a message can only be created
 *  in a particular block if "message_in_use" and "message_processed" are both false
 *  or both true.
 */
typedef struct {
    uint32_t    message_send_time;        /*!< Time message was sent                              */
    uint32_t    message_in_use;           /*!< Marker if the message is in use.                   */
    uint32_t    message_finished_time;    /*!< Time message was finished                          */
    uint32_t    msg_error_flags   : 8;    /*!< Error flags associated with message (unused)       */
    uint32_t    message_processed :24;    /*!< Marker if the message was processed                */
    uint16_t    src_sysflow_index;        /*!< Sysflow index of the source cvtask                 */
    uint16_t    dst_sysflow_index;        /*!< Sysflow index of the destination cvtask            */
    uint32_t    message_payload_daddr;    /*!< Physical DRAM address of the message payload       */
    uint32_t    message_size;             /*!< Message size                                       */
    uint32_t  buffer_size;                /*!< Buffer size (message buffer size + guardband size) */
} cvmem_messagebuf_variable_hdr_t;

/*! @struct cvmem_messagebuf_t
 *
 *  @brief Guardband structure used for CVTask messages.
 *
 *  All memory blocks allocated by the CVScheduler utilizes guardbands between each
 *  memory block.  This guardband description is specific to the CVTask Messages.
 */
typedef struct {
    cvmem_guardband_prefix_t        prefix;       /*!< substruct: guardband prefix                            */
    cvmem_messagebuf_fixed_hdr_t    hdr_fixed;    /*!< substruct: fixed content portion of the guardband      */
    cvmem_messagebuf_variable_hdr_t hdr_variable; /*!< substruct: variable content portion of the guardband   */
    cvmem_guardband_postfix_t       postfix;      /*!< substruct: guardband postfix                           */
} cvmem_messagebuf_t;

/*================================================================================================*/
/*=- FlexiDAG membuf header -=====================================================================*/
/*================================================================================================*/

/*! @struct membuf_creation_header_t
 *
 *  @brief Out-of-band header used to verify headers in safety-enabled systems
 *
 *  All communication to the VISORC (ASIL-B part) from the ARM-A53 (QM part) is done
 *  through a communication queue that is protected by an additional safety layer.
 *  However, this does not protect against a failure that generated the message.
 *  This structure is placed at the beginning of any fresh data buffer used by the
 *  Vision Driver out-of-band (i.e. outside of the message) as a secondary method
 *  of verification.
 */
typedef struct {
    uint32_t buffer_daddr;    /*!< Equals current address, must be a multiple of 128b   */
    uint32_t buffer_size;     /*!< Size of this buffer, must be a multiple of 128b      */
    uint32_t curr_time;       /*!< Time buffer was created, in audio ticks              */
    uint32_t fnv1a_checksum;  /*!< FNV1A-based checksum of previous 12 bytes            */
} membuf_creation_header_t;

/*================================================================================================*/
/* VISORC <-> ARM-R52 shared interface (used by Safety Thread)                                    */
/*================================================================================================*/

/*! @struct visorc_safety_heartbeat_t
 *
 *  @brief Heartbeat message shared between the VISORC and ARM-R52 for safety-enabled
 *         systems
 *
 *  This structure shall be placed in a memory location that is known by both the
 *  VISORC and ARM-R52.  This shall be updated at a rate that ensures compliance with
 *  the maximum reporting interval specified in the technical safety concept (TSC).
 */
typedef struct {
    uint32_t hb_sequence_no;      /*!< Sequence number of heartbeat, must be monotonically increasing */
    uint32_t hb_time;             /*!< Time of heartbeat message, in audio ticks */
    uint32_t schdr_state;         /*!< 0: down, 1: up, 0x8000.0000-0xFFFF.FFFF : shut down, return code */
    uint32_t fnv1a_checksum;      /*!< FNV1a-based checksum of previous 12 bytes */
} visorc_safety_heartbeat_t;

#ifdef ASIL_COMPLIANCE

/*! @struct sysflow_hashed_entry_t
 *
 *  @brief  Hashed entry for the sysflow table.  This is designed to work in parallel with the\n
 *          sysflow_entry_t table.
 *
 *  Note that matching hashes do not guarantee a matching entry.  The names from sysflow_entry_t
 *  must also be verified to ensure a match.
 */
typedef struct {
    uint32_t  instance_hash;              /*!< FNV-1A hash of instance_name from sysflow_entry_t */
    uint32_t  algorithm_hash;             /*!< FNV-1A hash of algorithm_name from sysflow_entry_t */
    uint32_t  step_hash;                  /*!< FNV-1A hash of step_name from sysflow_entry_t */
    uint32_t  cvtask_name_hash;           /*!< FNV-1A hash of cvtask_name from sysflow_entry_t */
    uint32_t  entry_hash;                 /*!< FNV-1A hash of the four strings combined */
    uint32_t  uuid;                       /*!< UUID from sysflow_entry_t  */
    uint32_t  sfhe_alignment_reserved0;   /*!< Alignment/Reserved for expansion, keeps structure at 32bytes */
    uint32_t  sfhe_alignment_reserved1;   /*!< Alignment/Reserved for expansion, keeps structure at 32bytes */
} sysflow_hashed_entry_t;

/*! @struct cvtable_hashed_entry_t
 *
 *  @brief  Hashed entry for the tbar table.
 *
 *  Note that a matching hash does not guarantee a matching entry.  The cvtable_name must
 *  also be verified to ensure a match.  Note that due to the varying table types, the
 *  maximum length of the cvtable_name is 120 chars (v2).
 */
typedef struct {
    uint32_t  cvtable_hash;               /*!< FNV-1A hash of the full cvtable_name (120 chars max) */
    uint32_t  cvtable_size;               /*!< size of cvtable entry */
    uint32_t  cvtable_daddr;              /*!< Physical daddr for cvtable entry */
    uint32_t  cvtable_reserved;           /*!< Reserved address, reserved for 40-bit expansion support */
    char      cvtable_name[112];          /*!< First 111 characters of cvtable_name, will be zero terminated */
} cvtable_hashed_entry_t;

/*-= CVScheduler Guardbands =---------------------------------------------------------------------+
| CVScheduler provides a "guardband" structure around memory blocks as a way to detect buffer     |
| overruns.  The guardbands provide two small strips of memory containing known patterns, and     |
| a metadata structure between these strips.                                                      |
+------------------------------------------------------------------------------------------------*/

/*! @struct core_guardband_raw_metadata_t
 *
 *  @brief  Raw format metadata structure for core_guardband
 *
 *  The "raw" metadata type provides a generalized access format to the guardband.
 *  This is primarily used when calculating the fnv-1a.  The metadata area is split
 *  into the fixed (7 uint32_t + fnv1a hash) and variable (8 uint32_t) areas.
 *
 *  This structure must be kept at 64 bytes.
 */
typedef struct  {
    uint32_t  gb_fixdata[7];          /*!< Fixed data (28 bytes)    */
    uint32_t  gb_fixdata_fnv1a;       /*!< FNV-1a hash              */
    uint32_t  gb_vardata[8];          /*!< Variable data (32 bytes) */
} core_guardband_raw_metadata_t;

/*! @struct core_guardband_base_metadata_t
 *
 *  @brief  Base format metadata structure for core_guardband
 *
 *  This structure provides a template from which all other formatted guardband
 *  metadata structures must follow.  The first four entries contain information
 *  that should be considered common across all other formatted guardband types.
 *
 *  Note that this structure uses "_dxaddr" which is an addressing format designed
 *  to be used with a 40-bit addressing system. This address shall always be the
 *  upper 32-bits of a 40-bit sized address, as the payload and guardbands must
 *  always be aligned on a 256 byte boundary.
 *
 *  Variables marked with "_daddr" can be converted directly into VISORC accessible
 *  addresses.
 *
 *  This structure must be kept at 64 bytes.
 */
typedef struct {
    uint32_t  gb_flags_and_type;      /*!< gb_fixdata[0] : flags TTxxGGUU : \n                          */
                                      /*!<            TT : Payload type \n                              */
                                      /*!<            GG : [40BIT] : GID of segment \n                  */
                                      /*!<            UU : [40BIT] : Upper 8-bits of segment offset     */
    uint32_t  gb_payload_dxaddr;      /*!< gb_fixdata[1] : dxaddr of payload.                           */
    uint32_t  gb_payload_segoffset;   /*!< gb_fixdata[2] : [40BIT] : lower 32-bits of segment offset    */
    uint32_t  gb_payload_size;        /*!< gb_fixdata[3] : size of payload                              */
    uint32_t  gb_metadata4;           /*!< gb_fixdata[4] : (used by other metadata formats)             */
    uint32_t  gb_metadata5;           /*!< gb_fixdata[5] : (used by other metadata formats)             */
    uint32_t  gb_metadata6;           /*!< gb_fixdata[6] : (used by other metadata formats)             */
    uint32_t  gb_fixdata_fnv1a;       /*!< --- fnv1a --- : Hash of gb_fixdata[]                         */
    uint32_t  gb_vardata[8];          /*!< gb_vardata[.] : (used by other metadata formats)             */
} core_guardband_base_metadata_t;

#define CVSCHED_GBTYPE_MASK           0xFF000000U     /*!< (gb_flags_and_type) : Mask to strip out GBTYPE */
#define CVSCHED_GBTYPE_MEMBLK         0x5A000000U     /*!< (gb_flags_and_type) : Memblk type              */
#define CVSCHED_GBTYPE_MSGBLK         0x27000000U     /*!< (gb_flags_and_type) : Msgblk type              */
#define CVSCHED_GBTYPE_MSGPOOL        0xB3000000U     /*!< (gb_flags_and_type) : Msgpool type             */
#define CVSCHED_GBTYPE_STACKBLK       0xCD000000U     /*!< (gb_flags_and_type) : Stack type               */

/*! @struct core_guardband_memblk_metadata_t
 *
 *  @brief  Memory Block format metadata structure for core_guardband
 *
 *  This structure preceeds memory blocks allocated by the CVScheduler.  In
 *  the case where the block is part of a MemBlkSet, then the gb_memblkset_*
 *  entries will be populated with non-zero values.
 *
 *  Note that this structure uses "_dxaddr" which is an addressing format designed
 *  to be used with a 40-bit addressing system. This address shall always be the
 *  upper 32-bits of a 40-bit sized address, as the payload and guardbands must
 *  always be aligned on a 256 byte boundary.
 *
 *  Variables marked with "_daddr" can be converted directly into VISORC accessible
 *  addresses.
 *
 *  This structure must be kept at 64 bytes.
 */

typedef struct
{
    uint32_t  gb_flags_and_type;      /*!< gb_fixdata[0] : flags TTxxGGUU : \n                          */
                                      /*!<            TT : Payload type \n                              */
                                      /*!<            GG : [40BIT] : GID of segment \n                  */
                                      /*!<            UU : [40BIT] : Upper 8-bits of segment offset     */
    uint32_t  gb_payload_dxaddr;      /*!< gb_fixdata[1] : dxaddr of payload.                           */
    uint32_t  gb_payload_segoffset;   /*!< gb_fixdata[2] : [40BIT] : lower 32-bits of segment offset    */
    uint32_t  gb_payload_size;        /*!< gb_fixdata[3] : size of payload                              */
    uint32_t  gb_memblkset_daddr;     /*!< gb_fixdata[4] : daddr of owning memblkset (0, if none)       */
    uint32_t  gb_memblkset_index;     /*!< gb_fixdata[5] : flags: XXXXCCCC : \n                         */
                                      /*!<          XXXX : Maximum number of memblk in memblkset \n     */
                                      /*!<          CCCC : Current index of memblk in memblkset         */
    uint32_t  gb_block_ctrl_flags;    /*!< gb_fixdata[6] : b0 : Needs_cache_coherency \n                */
                                      /*!<               : b1 : framework_reserved_1 (logger)           */
    uint32_t  gb_fixdata_fnv1a;       /*!< --- fnv1a --- : Hash of gb_fixdata[]                         */
    uint32_t  gbvd_frameset_id;       /*!< gb_vardata[0] : Frameset_id of memblk                        */
    uint32_t  gb_vardata[7];          /*!< gb_vardata[.] : (unused)                                     */
} core_guardband_memblk_metadata_t;

#define MEMBLK_FLAG_NEEDS_CACHE_COHERENCY   0x00000001U
#define MEMBLK_FLAG_LOGGER_OUTPUT           0x00000002U

#define memblk_guardband_t          core_guardband_t    /*!< Define to differentiate between guardband types */

/*! @struct core_guardband_msgblk_metadata_t
 *
 *  @brief  Message Block format metadata structure for core_guardband
 *
 *  This structure preceeds message blocks allocated by the CVScheduler.
 *
 *  Note that this structure uses "_dxaddr" which is an addressing format designed
 *  to be used with a 40-bit addressing system. This address shall always be the
 *  upper 32-bits of a 40-bit sized address, as the payload and guardbands must
 *  always be aligned on a 256 byte boundary.
 *
 *  Variables marked with "_daddr" can be converted directly into VISORC accessible
 *  addresses.
 *
 *  This structure must be kept at 64 bytes.
 */
typedef struct
{
    uint32_t  gb_flags_and_type;      /*!< gb_fixdata[0] : flags TTxxGGUU : \n                          */
                                      /*!<            TT : Payload type \n                              */
                                      /*!<            GG : [40BIT] : GID of segment \n                  */
                                      /*!<            UU : [40BIT] : Upper 8-bits of segment offset     */
    uint32_t  gb_payload_dxaddr;      /*!< gb_fixdata[1] : dxaddr of payload.                           */
    uint32_t  gb_payload_segoffset;   /*!< gb_fixdata[2] : [40BIT] : lower 32-bits of segment offset    */
    uint32_t  gb_payload_size;        /*!< gb_fixdata[3] : size of payload                              */
    uint32_t  gb_msgpool_dxaddr;      /*!< gb_fixdata[4] : dxaddr of owning msgpool                     */
    uint32_t  gb_msgpool_vis_daddr;   /*!< gb_fixdata[5] : daddr of CVScheduler visible control block   */
    uint32_t  gb_msg_index;           /*!< gb_fixdata[6] : flags: XXXXCCCC : \n                         */
                                      /*!<          XXXX : Maximum number of messages in pool \n        */
                                      /*!<          CCCC : Current index of this message in pool        */
    uint32_t  gb_fixdata_fnv1a;       /*!< --- fnv1a --- : Hash of gb_fixdata[]                         */
    uint32_t  gbvd_usage_flag;        /*!< gb_vardata[0] : non-zero : currently in use                  */
    uint32_t  gbvd_src_sfbindex;      /*!< gb_vardata[1] : Message source's SFB index                   */
    uint32_t  gbvd_dst_sfbindex;      /*!< gb_vardata[2] : Message destination's SFB index              */
    uint32_t  gb_vardata[5];          /*!< gb_vardata[3..7] : (unused)                                  */
} core_guardband_msgblk_metadata_t;

#define msgblk_guardband_t          core_guardband_t    /*!< Define to differentiate between guardband types */

/*! @struct core_guardband_msgpool_metadata_t
 *
 *  @brief  Message pool control metadata structure for core_guardband
 *
 *  This structure is used to define a message pool for use by the CVScheduler.
 *  This is always located in a DRAM address visible to the ARM, but also has an
 *  associated internal CVScheduler control (also referenced by msgblk as well)
 *
 *  Note that this structure uses "_dxaddr" which is an addressing format designed
 *  to be used with a 40-bit addressing system. This address shall always be the
 *  upper 32-bits of a 40-bit sized address, as the payload and guardbands must
 *  always be aligned on a 256 byte boundary.
 *
 *  Variables marked with "_daddr" can be converted directly into VISORC accessible
 *  addresses.
 *
 *  This structure must be kept at 64 bytes.
 */
typedef struct
{
    uint32_t  gb_flags_and_type;      /*!< gb_fixdata[0] : flags TTxxGGUU : \n                          */
                                      /*!<            TT : Payload type \n                              */
                                      /*!<            GG : [40BIT] : GID of segment \n                  */
                                      /*!<            UU : [40BIT] : Upper 8-bits of segment offset     */
    uint32_t  gb_payload_dxaddr;      /*!< gb_fixdata[1] : dxaddr of message poolbase                   */
    uint32_t  gb_payload_segoffset;   /*!< gb_fixdata[2] : [40BIT] : lower 32-bits of segment offset    */
    uint32_t  gb_payload_size;        /*!< gb_fixdata[3] : size of payload                              */
    uint32_t  gb_msgpool_vis_daddr;   /*!< gb_fixdata[4] : daddr of CVScheduler visible control block   */
    uint32_t  gb_msgpool_msgsize;     /*!< gb_fixdata[5] : size of each message payload in pool         */
    uint32_t  gb_msgpool_msgnum;      /*!< gb_fixdata[6] : number of messages in pool                   */
    uint32_t  gb_fixdata_fnv1a;       /*!< --- fnv1a --- : Hash of gb_fixdata[]                         */
    uint32_t  gbvd_payload_dxaddr;    /*!< gb_vardata[0] : Used by ARM : Shadow copy for ARM cache line */
    uint32_t  gbvd_msgpool_bufsize;   /*!< gb_vardata[1] : Used by ARM : step between message buffers   */
    uint32_t  gbvd_msgpool_msgnum;    /*!< gb_vardata[2] : Used by ARM : Shadow copy for ARM cache line */
    uint32_t  gbvd_msgpool_wridx;     /*!< gb_vardata[3] : Used by ARM : next write index               */
    uint32_t  gb_vardata[4];          /*!< gb_vardata[4..7] : Unused                                    */
} core_guardband_msgpool_metadata_t;

#define msgpool_guardband_t         core_guardband_t    /*!< Define to differentiate between guardband types */

/*! @struct core_guardband_stack_metadata_t
 *
 *  @brief  CVTask sandboxed stack metadata structure for core_guardband
 *
 *  This structure preceeds isolated cvtask stack blocks allocated by the CVScheduler
 *
 *  Note that this structure uses "_dxaddr" which is an addressing format designed
 *  to be used with a 40-bit addressing system. This address shall always be the
 *  upper 32-bits of a 40-bit sized address, as the payload and guardbands must
 *  always be aligned on a 256 byte boundary.
 *
 *  Variables marked with "_daddr" can be converted directly into VISORC accessible
 *  addresses.
 *
 *  This structure must be kept at 64 bytes.
 */
typedef struct
{
    uint32_t  gb_flags_and_type;      /*!< gb_fixdata[0] : flags TTxxGGUU : \n                          */
                                      /*!<            TT : Payload type \n                              */
                                      /*!<            GG : [40BIT] : GID of segment \n                  */
                                      /*!<            UU : [40BIT] : Upper 8-bits of segment offset     */
    uint32_t  gb_payload_dxaddr;      /*!< gb_fixdata[1] : dxaddr of payload.                           */
    uint32_t  gb_payload_segoffset;   /*!< gb_fixdata[2] : [40BIT] : lower 32-bits of segment offset    */
    uint32_t  gb_payload_size;        /*!< gb_fixdata[3] : size of payload                              */
    uint32_t  gb_metadata4;           /*!< gb_fixdata[4] : (unused)                                     */
    uint32_t  gb_metadata5;           /*!< gb_fixdata[5] : (unused)                                     */
    uint32_t  gb_metadata6;           /*!< gb_fixdata[6] : (unused)                                     */
    uint32_t  gb_fixdata_fnv1a;       /*!< --- fnv1a --- : Hash of gb_fixdata[]                         */
    uint32_t  gb_vardata[8];          /*!< gb_vardata[.] : Unused                                       */
} core_guardband_stack_metadata_t;

#define stack_guardband_t           core_guardband_t    /*!< Define to differentiate between guardband types */

/*! @struct core_guardband_t
 *
 *  @brief  Generalized structure for guardbanding.
 *
 *  The CVScheduler utilizes the concept of guardbands between memory buffers that
 *  are allocated by the CVScheduler.  These guardbands contain two symmetrical, large
 *  strips containing a predefined patterns, and a metadata block placed between these
 *  two strips.  This metadata blocks is further subdivided into two portions - a fixed
 *  and variable area.  The fixed area contains information that shall not be changed
 *  once the associated data block is set up, and is protected with a FNV1A hash.
 *
 *  This structure must be kept at 256 bytes.
 */
#define   CVSCH_GUARDBAND_STRIP_SIZE    (24U)           /*!< Size of each strip (must be symmetrical) */
#define   CVSCH_GUARDBAND_PREFIX_VAL    (0xFEEDBABEU)   /*!< Pattern for prefix guarband strip        */
#define   CVSCH_GUARDBAND_POSTFIX_VAL   (0x4ADADB0DU)   /*!< Pattern for postfix guardband strip      */

typedef struct {

    uint32_t  gb_prefix[CVSCH_GUARDBAND_STRIP_SIZE];    /*!< Prefix strip (filled with CVSCH_GUARDBAND_PREFIX_VAL) */

    union {                                             /*!< Union used for metadata blocks.  These are differentiated by \n */
                                                        /*!< the gb_flags_and_type variable in each metadata block type. */
        core_guardband_raw_metadata_t     raw;          /*!< raw guardband metadata type */
        core_guardband_base_metadata_t    base;         /*!< base guardband metadata type */
        core_guardband_memblk_metadata_t  memblk;       /*!< memory block guardband metadata type */
        core_guardband_msgblk_metadata_t  msgblk;       /*!< message block guardband metadata type */
        core_guardband_msgpool_metadata_t msgpool;      /*!< message pool guardband metadata type */
        core_guardband_stack_metadata_t   stackblk;     /*!< cvtask-stack guardband metadata type */
    } mtdt;

    uint32_t  gb_postfix[CVSCH_GUARDBAND_STRIP_SIZE];   /*!< Postfixed strip (filled with CVSCH_GUARDBAND_POSTFIX_VAL) */

} core_guardband_t;

#endif /* ?ASIL_COMPLIANCE */

#endif /* ?CVSCHED_API_H_FILE */

