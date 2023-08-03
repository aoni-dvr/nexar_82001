/**
 *  @file cvapi_ambacv_flexidag.h
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
 *  @details Definitions & Constants for Ambacv flexidag APIs
 *
 */

#ifndef CVAPI_AMBACV_FlEXIDAG_H
#define CVAPI_AMBACV_FlEXIDAG_H

/**
 * @defgroup API_VisionDriver vision driver api
 * @brief vision driver api for scheduler and flexidag
 */

/**
 * @addtogroup API_VisionDriver api for vision cvflow control
 * @{
 */

#include "ambint.h"
#include "cvapi_flexidag.h"
#if defined(CONFIG_BUILD_SSP_KAL) && !defined(DISABLE_KAL)
#include "AmbaKAL.h"
#include "AmbaErrorCode.h"
#include "AmbaMMU.h"
#else
#include <pthread.h>
#endif

#if defined(CONFIG_BUILD_SSP_KAL) && !defined(DISABLE_KAL)
typedef AMBA_KAL_MUTEX_t        mutex_t;
#else
#define SSP_ERR_BASE        0x01000000U /* 0x01000000 to 0x01ffffff: Reserved for SSP */
#define SCHDR_ERR_BASE      (SSP_ERR_BASE + 0x00400000U)
#define CVTASK_ERR_BASE     (SSP_ERR_BASE + 0x00410000U)
#define CVAPP_ERR_BASE      (SSP_ERR_BASE + 0x00420000U)
#define CVACC_ERR_BASE      (SSP_ERR_BASE + 0x00430000U)

typedef pthread_mutex_t mutex_t;
#endif

/* Private macro */
#define SCHDR_PRINT_MODULE_ID           ((UINT16)(SCHDR_ERR_BASE >> 16U))
#define CVTASK_PRINT_MODULE_ID          ((UINT16)(CVTASK_ERR_BASE >> 16U))
#define CVAPP_PRINT_MODULE_ID           ((UINT16)(CVAPP_ERR_BASE >> 16U))

#define IN
#define OUT

/* Public macro */
#define CV_ERR_0000 ((uint32_t)(CVAPP_ERR_BASE | 0UL))        /**< Interface error */
#define CV_ERR_0001 ((uint32_t)(CVAPP_ERR_BASE | 1UL))        /**< Scheduler error */
#define CV_ERR_0002 ((uint32_t)(CVAPP_ERR_BASE | 2UL))        /**< Flexidag error */
#define CV_ERR_0003 ((uint32_t)(CVAPP_ERR_BASE | 3UL))        /**< Invalid error(not expect error category) */

#define FLEXIDAG_API_TIME_BUF               16U             /**< API time buffer array size */
#define FLEXIDAG_LABEL_SIZE                 32U             /**< Label array size */

#define FLEXIDAG_SCHDR_OFF                  0U              /**< Scheduler state off */
#define FLEXIDAG_SCHDR_ON                   1U              /**< Scheduler state on */
#define FLEXIDAG_SCHDR_SAFE                 2U              /**< Scheduler state safe */

#define FLEXIDAG_SCHDR_DRV_OFF              0U              /**< Scheduler drv state off */
#define FLEXIDAG_SCHDR_DRV_ON               1U              /**< Scheduler drv state on */
#define FLEXIDAG_SCHDR_DRV_SAFE             2U              /**< Scheduler drv state safe */

#define FLEXIDAG_PARAMSET_LOG_MSG   0U                      /**< Paramset for AMBA_CV_FLEXIDAG_LOG_MSG_s */
#define FLEXIDAG_PARAMSET_TIMEOUT   1U                      /**< Paramset for AMBA_CV_FLEXIDAG_TIMEOUT_s */

#define FLEXIDAG_PERF_START         0U                      /**< event start of AmbaCV_FlexidagPerf */
#define FLEXIDAG_PERF_STOP          1U                      /**< event stop of AmbaCV_FlexidagPerf */
#define FLEXIDAG_PERF_STOP_CONTI    2U                      /**< event stop without changing stop time */

#define FLEXIDAG_SCHDR_VP_BIN       0U
#define FLEXIDAG_SCHDR_SOD_BIN      1U

#define VP_OUTPUT_ALIGNMENT         0U
#define FLEXIDAG_MEM_ALIGNMENT      1U

typedef uint32_t (*flexidag_schdr_load_cb)(const char *file_name, void* ptr, uint32_t size, uint32_t *real_size);
typedef int32_t (*flexidag_schdr_fill_cb)(char *buff, int32_t size, void* token);

/**
 * @brief flexidag time structure.
 */
typedef struct {
    uint32_t                        token_id;               /*!< Token ID which is from the non-blocking run.. */
    uint32_t                        at_time;                   /*!< Timestamp. */
} AMBA_CV_FLEXIDAG_TIME_s;

/**
 * @brief flexidag log message structure.
 */
typedef struct {
    uint32_t                        flexidag_msg_entry;     /*!< Msg entries supported in FlexiDAG for ARM APP sends msg to CVTask and the size of entry is 1024 Bytes. It can set to 0 if there is no requirements in this FlexiDAG. (Default is 8 entries). */
    uint32_t                        cvtask_msg_entry;       /*!< Msg entries supported in FlexiDAG for internal CVTask to send msg to CVTask and the size of entry is 1024 Bytes. It can set to 0 if there is no requirement in this FlexiDAG. (Default is 4 entries). */
    uint32_t                        arm_cpu_map;            /*!< This is the Arm scheduler bit map. For single Arm scheduler, set to 0x1. \n
                                                                 For multiple Arm schedulers, RTOS would be 0x1, Linux could be 0x2.   \n
                                                                 The total Arm log entry required for main scheduler is arm_scheduler_number * arm_log_entry.*/
    uint32_t                        arm_log_entry;          /*!< Arm side log entry. If there's no Arm CVTask, it could be 0. (Default = 0 for cvtask, sched, and perf log) */
    uint32_t                        orc_log_entry;          /*!< ORC side log entry. (Default 512 entry for cvtask, and sched, perf log is 0) */
} AMBA_CV_FLEXIDAG_LOG_MSG_s;

/**
 * @brief flexidag debug config structure.
 */
typedef struct {
    uint16_t                        flow_id;                /*!< The flow ID of the system flow table. */
    uint16_t                        log_level;              /*!< Log level of flexidag. */
} AMBA_CV_FLEXIDAG_DEBUG_CFG_s;

/**
 * @brief flexidag timeout structure.
 */
typedef struct {
    uint32_t                        run_timeout;            /*!< The FlexiDAG API execute timeout. */
} AMBA_CV_FLEXIDAG_TIMEOUT_s;


/**
 * @brief flexidag init structure.
 */
typedef struct {
    flexidag_memblk_t               state_buf;              /*!< The internal state buffer of FlexiDAG \n
                                                                 (state_buf is the buffer required for internal used in FlexiDAG, including internal in-out buffer between CVTask, \n
                                                                 data structure, buffer queue, message buffer, log buffer and instance private data in orc CVTask). */
    flexidag_memblk_t               temp_buf;               /*!< The internal temp buffer of FlexiDAG \n
                                                                 (temp_buf buffer required for DRAM_temporary_scratchpad_needed in CVTask which is only used in current run \n
                                                                 and can be shared with other CVTasks after the current run is complete). */
} AMBA_CV_FLEXIDAG_INIT_s;

/**
 * @brief flexidag io structure.
 */
typedef struct {
    uint32_t                        num_of_buf;             /*!< Number of Buffer. */
    flexidag_memblk_t               buf[FLEXIDAG_MAX_INPUTS];  /*!< Buffer information. */
} AMBA_CV_FLEXIDAG_IO_s;

/**
 * @brief flexidag message structure.
 */
typedef struct {
    uint16_t                        flow_id;                /*!< The flow ID of the system flow table. */
    void                            *vpMessage;             /*!< The pointer of the message. */
    uint32_t                        length;                 /*!< The length of the message. */
} AMBA_CV_FLEXIDAG_MSG_s;

/**
 * @brief flexidag name structure.
 */
typedef struct {
    char                            *instance;              /*!< Instance name in the system flow table. */
    char                            *algorithm;             /*!< Algorithm name in the system flow table. */
    char                            *step;                  /*!< Step name in the system flow table. */
    char                            *cvtask;                /*!< CVTask name in the system flow table. */
} AMBA_CV_FLEXIDAG_NAME_s;

/**
 * @brief flexidag handle structure.
 */
typedef struct {
    void                            *fd_handle;             /*!< The pointer of FlexiDAG handle */
    uint8_t                         set_log_msg;            /*!< If set to 1, log_msg value will be set. */
    AMBA_CV_FLEXIDAG_LOG_MSG_s      log_msg;                /*!< The value of the log message which is set by AmbaCV_FlexidagSetParamSet */
    uint32_t                        perf_start;             /*!< Timestamp of performance measure start */
    AMBA_CV_FLEXIDAG_TIME_s         api_run_start[FLEXIDAG_API_TIME_BUF];   /*!< Structure array of non-blocking runs when API measure starts */
    uint32_t                        api_run_index;          /*!< Current index of api_run_start array */
    mutex_t                         api_run_mutex;          /*!< Mutex to protect api_run_start array */
    flexidag_memory_requirements_t  mem_req;                /*!< The memory requirement for internal and output of FlexiDAG. */
} AMBA_CV_FLEXIDAG_HANDLE_s;

#define FLEXIDAG_ENABLE_BOOT_CFG    65536                   /*!< enable bit of boot_cfg of AMBA_CV_STANDALONE_SCHDR_CFG_s */
#define LOG_ENTRIES_ENABLE_CFG      0xC1000000U             /*!< enable config magic of log_entries of AMBA_CV_STANDALONE_SCHDR_CFG_s */
#define LOG_ENTRIES_ENABLE_BIT      0xFF000000U             /*!< enable bit of log_entries of AMBA_CV_STANDALONE_SCHDR_CFG_s */

/**
 * @brief flexidag standalone start config structure.
 */
typedef struct {
    int32_t                         flexidag_slot_num;      /*!< Maximum number of FlexiDAGs in the system */
    int32_t                         cavalry_slot_num;       /*!< Maximum number of cavalry in the system (scheduler supports both the cavalry and the FlexiDAG feature, one can keep cavalry_slot_num = 0)*/
    int32_t                         cpu_map;                /*!< CPU bits map for running the thread pool for CVTasks (Example: In the case of 0xD, it works with 3 cores, Core0, Core2, and Core3) */
    int32_t                         log_level;              /*!< Log level of scheduler(LVL_SILENT = 0,        \n
                                                                                        LVL_CRITICAL = 1,     \n
                                                                                        LVL_MINIMAL = 2,      \n
                                                                                        LVL_NORMAL = 3,       \n
                                                                                        LVL_VERBOSE = 4,      \n
                                                                                        LVL_DEBUG = 5)*/
    int32_t                         flag;                   /*!< Flag for scheduler( SCHDR_DISABLE_SS_LOG            0x0004U  disable startup/shutdown log   \n
                                                                                     SCHDR_FASTBOOT                  0x1000U  Fastboot state */
    int32_t                         boot_cfg;               /*!< Default: 0 \n
                                                                 Two schedulers for two OSes:    \n
                                                                 A OS:  2 + FLEXIDAG_ENABLE_BOOT_CFG \n
                                                                 B OS : 0    \n
                                                            */
    int32_t                         autorun_interval;       /*!< Autorun cvtask interval in ms(Optional for ASIL CHIP), defaults to 90ms if zero or greater than 10000. */
    int32_t                         autorun_fex_cfg;        /*!< Autorun fex cvtask config(Optional for ASIL CHIP), disable if set 1 . */
    uint32_t                        log_entries_visschdr;   /*!< Log entries visschdr size(bit[31:24]: enable bit, bit[23:0]: size bit) */
    uint32_t                        log_entries_viscvtask;  /*!< Log entries viscvtask size(bit[31:24]: enable bit, bit[23:0]: size bit) */
    uint32_t                        log_entries_visperf;    /*!< Log entries visperf size(bit[31:24]: enable bit, bit[23:0]: size bit) */
} AMBA_CV_STANDALONE_SCHDR_CFG_s;

/**
 * @brief flexidag scheduler start config structure.
 */
typedef struct {
    int32_t                         cpu_map;                /*!< CPU bits map for running thread pool for CVTasks (Example: In the case of 0xD, it works with 3 cores, namely Core0, Core2, and Core3) */
    int32_t                         log_level;              /*!< Log level of scheduler(LVL_SILENT = 0,     \n
                                                                                        LVL_CRITICAL = 1,  \n
                                                                                        LVL_MINIMAL = 2,  \n
                                                                                        LVL_NORMAL = 3,   \n
                                                                                        LVL_VERBOSE = 4,  \n
                                                                                        LVL_DEBUG = 5)*/
} AMBA_CV_FLEXIDAG_SCHDR_CFG_s;

/**
 * @brief flexidag run info structure.
 */
typedef struct {
    uint32_t                        overall_retcode;        /*!< Retcode from full run (return error code which is reported by the ORC scheduler) */
    uint32_t                        start_time;             /*!< Event run start time in us. (start time for FlexiDAG beginning run in ORC side) */
    uint32_t                        end_time;               /*!< Event run end time in us. (end time for FlexiDAG all output generated in ORC side) */
    uint32_t                        api_start_time;         /*!< API run start time in us. (start time measured in API start) */
    uint32_t                        api_end_time;           /*!< API run end time in us. (end time measured at API end and it means (end_time - start_time) as executing time of ORC  \n
                                                                 and (api_end_time - api_start_time) as executing time of ARM + ORC) */
    uint32_t                        output_not_generated;   /*!< Bitmap to mark if an output isn't generated.   \n
                                                                 (If the frequency in AmbaCV_FlexidagSetFrequency is 2, output_not_generated is set to 1 every other run) */
    uint32_t                        output_donetime[FLEXIDAG_MAX_OUTPUTS];  /*!< Value in us. The time when output is complete.     \n
                                                                 (Each FlexiDAG can have more than one output and output_donetime is the end time for this output) */
} AMBA_CV_FLEXIDAG_RUN_INFO_s;

/**
 * @brief flexidag perf structure.
 */
typedef struct {
    char                            label[FLEXIDAG_LABEL_SIZE]; /*!< Label of FlexiDAG */
    uint32_t                        runs;                   /*!< The number of runs in this performance */
    uint32_t                        avg_time;               /*!< Average time of the VP run in one frame (us) without the yield time*/
    uint32_t                        min_time;               /*!< Minimal time of the VP run in one frame (us) without the yield time */
    uint32_t                        max_time;               /*!< Maximal time of the VP run in one frame (us) without the yield time */
    uint32_t                        avg_frame_time;         /*!< Average time in one frame (us) with yield time */
    uint32_t                        min_frame_time;         /*!< Minimal time in one frame (us) with yield time  */
    uint32_t                        max_frame_time;         /*!< Maximal time in one frame (us) with yield time  */
    uint32_t                        total_run_time;         /*!< Total VP run time in this performance (us)*/
    uint32_t                        total_time;             /*!< Total time in this performance measure.(us) */
} AMBA_CV_FLEXIDAG_PERF_s;

/**
 *  @brief      Loads visorc.bin to bring up scheduler.
 *
 *  @param      path : [IN] Directory of visorc.bin
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrLoad(
    IN    const char *path);

/**
 *  @brief      Loads visorc.bin by user callback to bring up scheduler.
 *
 *  @param      file_name : [IN] File of visorc.bin
 *
 *  @param      bin_type : [IN] vp or sod bin
 *
 *  @param      load_cb : [IN] load call back function
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrLoadExt(
    IN const char *file_name,
    IN uint32_t bin_type,
    IN flexidag_schdr_load_cb load_cb);

/**
 *  @brief      Check visorc from memory.
 *
 *  @param      bin_type : [IN] vp or sod bin
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrCheckVisorc(
    IN    uint32_t bin_type);

/**
 *  @brief      Loads cvtable.bin by user callback to bring up scheduler.
 *
 *  @param      file_name : [IN] File of cvtable.bin
 *
 *  @param      fill_cb : [IN] fill call back function
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrAddCvtable(
    IN const char *file_name,
    IN flexidag_schdr_fill_cb fill_cb);

/**
 *  @brief      Get scheduler drv state.\n
 *              APP get scheduler driver state.  \n
 *              If state = FLEXIDAG_SCHDR_DRV_OFF, App should call AmbaCV_StandAloneSchdrStart or AmbaCV_FlexidagSchdrStart\n
 *              If state = FLEXIDAG_SCHDR_DRV_ON, App can start to call flexidag API \n
 *              If state = FLEXIDAG_SCHDR_DRV_SAFE, App would not allow to call flexidag API \n
 *
 *  @param      status : [IN] Scheduler state   \n
 *                              FLEXIDAG_SCHDR_DRV_OFF : 0x0    \n
 *                              FLEXIDAG_SCHDR_DRV_ON :  0x1
 *                              FLEXIDAG_SCHDR_DRV_SAFE :  0x2
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrDrvState(
    OUT   uint32_t *status);

/**
 *  @brief      Get ORC scheduler state. For a process to initialize the scheduler,   \n
 *              APP calls AmbaCV_StandAloneSchdrStart to initialize scheduler if state is FLEXIDAG_SCHDR_OFF   \n
 *              and calls AmbaCV_FlexidagSchdrStart if the state is FLEXIDAG_SCHDR_ON.
 *
 *  @param      status : [IN] Scheduler state   \n
 *                              FLEXIDAG_SCHDR_OFF : 0x0    \n
 *                              FLEXIDAG_SCHDR_ON  : 0x1
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrState(
    OUT   uint32_t *status);


/**
 *  @brief      Initialize scheduler for FlexiDAG which is used for the first process.    \n
 *              In RTOS, this is the API that is used to start a FlexiDAG scheduler since the whole system is a single process.
 *
 *  @param      cfg : [IN] Config of scheduler initializes
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_StandAloneSchdrStart(
    IN    const AMBA_CV_STANDALONE_SCHDR_CFG_s *cfg);

/**
 *  @brief      Initialize the scheduler for the FlexiDAG which used for other process.   \n
 *              In RTOS, this API is not used since the whole system is a single process.
 *
 *  @param      cfg : [IN] Config of scheduler initializes
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSchdrStart(
    IN    const AMBA_CV_FLEXIDAG_SCHDR_CFG_s *cfg);

/**
 *  @brief      Stop arm scheduler driver only (VISORC still active) and scheduler driver state change to inactive.
 *
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSchdrStop(void);

/**
 *  @brief      Shutdown scheduler and shutdown flush log generator.
 *
 *  @param      Reserved : [IN] Not used for now
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrShutdown(
    IN    uint32_t Reserved);

/**
 *  @brief      Dump scheduler messages from VISORC.
 *
 *  @param      type : [IN] FLEXIDAG_SCHDR_VP_BIN or FLEXIDAG_SCHDR_SOD_BIN
 *  @param      log_file : [IN] file name to dump out.If not, set NULL
 *  @param      num_lines : [IN] num line of message to dump
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrDumpLog(uint32_t type, const char *log_file, uint32_t num_lines);

/**
 *  @brief      Wait and process messages from VISORC.
 *              The function will handle cache coherence for all incoming messages and process some messages in Kernel layer due to
 *              the multiple process support of FlexiDAG operation in multiple OSes.
 *              The wait will timeout if there is no message coming and user need to call this API again to wait again.
 *
 *  @param      msg_num : [OUT] num of messages ready to process
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrWaitMsg(uint32_t *msg_num);

/**
 *  @brief      Process message messages from VISORC.
 *              If no message can process, API return fail.
 *              If this API return success, user need to check if message_retcode success or fail.
 *              If message_retcode is failed, user can get failed message ID and corresponding FlexiDAG handle if
 *              the message belongs to a FlexiDAG.
 *
 *  @param      pphandle : [OUT] if phandle is NULL, it is scheduler message, not for a particular FlexiDAG.
 *  @param      message_type : [OUT] type of message
 *  @param      message_retcode : [OUT] return error code
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrProcessMsg(
    OUT AMBA_CV_FLEXIDAG_HANDLE_s **pphandle,
    OUT uint32_t *message_type,
    OUT uint32_t *message_retcode);

/**
 *  @brief      Get HW info.
 *
 *  @param      id : [IN] The ID of the HW info.  \n
 *                                   VP_OUTPUT_ALIGNMENT      0U
 *                                   FLEXIDAG_MEM_ALIGNMENT   1U
 *  @param      info : [IN] The pointer of the param_set structure.
 *  @param      info_size : [IN] The size of the param_set structure.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_SchdrGetHwInfo(
    IN uint32_t id,
    IN void *info,
    IN uint32_t info_size);

/**
 *  @brief      Set FlexiDAG using the input tbar and return the handle.
 *
 *  @param      flexidag_tbar : [IN] The FlexiDAG buffer copies from files containing CVTasks Tbar.
 *  @param      handle : [OUT] The handle of the FlexiDAG object
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetTbar(
    IN    flexidag_memblk_t *flexidag_tbar,
    OUT   AMBA_CV_FLEXIDAG_HANDLE_s *handle);

/**
 *  @brief      Set FlexiDAG using the input sfb and return the handle.
 *
 *  @param      flexidag_sfb : [IN] The FlexiDAG buffer copies from files containing CVTasks sfb.
 *  @param      handle : [OUT] The handle of the FlexiDAG object
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetSFB(
    IN    flexidag_memblk_t *flexidag_sfb,
    OUT   AMBA_CV_FLEXIDAG_HANDLE_s *handle);

/**
 *  @brief      Open the FlexiDAG using the input FlexiBin and return the handle.
 *
 *  @param      flexibin_buf : [IN] The FlexiBin buffer copies from files containing any CVTasks that runs on the VISORC.
 *  @param      handle : [OUT] The handle of the FlexiDAG object
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagOpen(
    IN    flexidag_memblk_t *flexibin_buf,
    OUT   AMBA_CV_FLEXIDAG_HANDLE_s *handle);

/**
 *  @brief      Initializes FlexiDAG and assigns internal buffer.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      init : [IN] The initial buffer required by FlexiDAG.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagInit(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    AMBA_CV_FLEXIDAG_INIT_s *init);

/**
 *  @brief      With the provided input and output buffer, run FlexiDAG in blocking mode and return run information.  \n
 *              Application should do cache handling on input and output buffer if necessary.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      in : [IN] The input buffer required by FlexiDAG run.
 *  @param      out : [IN] The output buffer required by FlexiDAG run, the output result is filled in the buffer after this API is complete..
 *  @param      run_info : [OUT] Information of this run.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagRun(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    AMBA_CV_FLEXIDAG_IO_s *in,
    IN    AMBA_CV_FLEXIDAG_IO_s *out,
    OUT   AMBA_CV_FLEXIDAG_RUN_INFO_s *run_info);

/**
 *  @brief      With the provided input and output buffer, run FlexiDAG in blocking mode and return run information.  \n
 *              Application should do cache handling on input and output buffer if necessary.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      cb : [IN] The output callback function.
 *  @param      cb_param : [IN] Input parameter of callback function.
 *  @param      in : [IN] The input buffer required by FlexiDAG run.
 *  @param      out : [IN] The output buffer required by FlexiDAG run; the output result is filled in the buffer after callback woken up.
 *  @param      token_id : [OUT] Used to get run information.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagRunNonBlock(
    IN    AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    flexidag_cb cb,
    IN    void *cb_param,
    IN    AMBA_CV_FLEXIDAG_IO_s *in,
    IN    AMBA_CV_FLEXIDAG_IO_s *out,
    OUT   uint32_t *token_id);

/**
 *  @brief      This is used with non-blocking run API. When this API is called, it returns full run information by token ID until all outputs are complete.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      token_id : [IN] The input buffer required by FlexiDAG run.
 *  @param      pRunInfo : [OUT] The output buffer required by FlexiDAG run.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagWaitRunFinish(
    IN    AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    uint32_t token_id,
    OUT   AMBA_CV_FLEXIDAG_RUN_INFO_s *pRunInfo);

/**
 *  @brief      Dump log of the FlexiDAG. The type of the log is controlled by flags, and the log can be the running log of the FlexiDAG or performance log. \n
 *              The flags are like FLEXILOG_XXX defined in cvapi_flexidag.h..
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      path : [IN] The file path of log, if path == NULL, output to console.
 *  @param      flags : [IN] Output message type of log.    \n
 *                                  FLEXILOG_VIS_PERF             0x00000000U \n
 *                                  FLEXILOG_VIS_SCHED           0x00010000U \n
 *                                  FLEXILOG_VIS_CVTASK         0x00020000U
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagDumpLog(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    const char *path,
    IN    uint32_t flags);

/**
 *  @brief      Get the system flow table index of the CVTask corresponding to the UUID.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      flow_uuid : [IN] UUID in the system flow table.
 *  @param      flow_id : [OUT] Flow ID of the system flow table.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagGetFlowIdByUUID(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    uint32_t flow_uuid,
    OUT   uint16_t *flow_id
);

/**
 *  @brief      Get system flow table index of the CVTask corresponding to the name. There could be multiple CVTasks matching the name.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      name : [IN] Name in system flow table.
 *  @param      num_of_array : [IN] Number of array of flow ID array. (The upper bond of num_of array is set by FLEXIDAG_MAX_SFB_ENTRIES).
 *  @param      flow_id_array : [OUT] Flow ID array.
 *  @param      num_found : [OUT] Number of Flow ID existing in flow_id_array.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagGetFlowIdByName(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    AMBA_CV_FLEXIDAG_NAME_s name,
    IN    uint32_t num_of_array,
    OUT   uint16_t *flow_id_array,
    OUT   uint32_t *num_found
);

/**
 *  @brief      Send message to CVTask for the opened FlexiDAG.   \n
 *              Flow ID is ID for certain CVTasks and can get by name or UUID in *.csv which is needed when compiling FlexiDAG bin.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      msg : [IN] Message to send to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSendMsg(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    const AMBA_CV_FLEXIDAG_MSG_s    *msg);

/**
 *  @brief      Send private message to CVTask.   \n
 *              Flow ID is ID for certain CVTasks and can get by name or UUID in *.csv which is needed when compiling FlexiDAG bin.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      msg : [IN] Message to send to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSendPrivateMsg(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    const AMBA_CV_FLEXIDAG_MSG_s    *msg);

/**
 *  @brief      Set a set of parameters of FlexiDAG. The parameters are named as FLEXIDAG_PARAMSET_XXX and listed in cvapi_ambacv_flexidag.h.    \n
 *              It is recommend to call AmbaCV_FlexidagSetParamSet before AmbaCV_FlexidagInit.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      id : [IN] The ID of the parameter set.  \n
 *                                   FLEXIDAG_PARAMSET_LOG_MSG   0U
 *  @param      param_set : [IN] The pointer of the param_set structure.
 *  @param      param_size : [IN] The size of the param_set structure.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetParamSet(
    IN    AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    uint32_t id,
    IN    const void *param_set,
    IN    uint32_t param_size);

/**
 *  @brief      Set debug level to CVTask for the opened FlexiDAG.   \n
 *              Flow ID is ID for certain CVTasks and can get by name or UUID in *.csv which is needed when compiling FlexiDAG bin.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      debug : [IN] Debug level to set to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetDebugLevel(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    const AMBA_CV_FLEXIDAG_DEBUG_CFG_s *debug);

/**
 *  @brief      Find cvtable data in *.tbar which is needed when compiling the FlexiDAG bin. \n
 *              Example: AmbaCV_FlexidagFindCvtable(handle, "FEEDER_IONAME_000", &cvtable_data, &cvtable_size);    \n
 *              The result of cvtable_data is "BISENET_MNV2_INPUT_0" xxxx.mnft.in "FEEDER_IONAME_000"   string    "BISENET_MNV2_INPUT_0";.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      name : [IN] Name of cvtable.
 *  @param      data : [OUT] Data of cvtable.
 *  @param      size : [OUT] Size of data.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagFindCvtable(
    IN    const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    const char *name,
    OUT   const void **data,
    OUT   uint32_t *size);

/**
 *  @brief      Call back to handle the error from the ORC scheduler.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      cb : [IN] Call back.
 *  @param      cb_param : [OUT] Parameter of call back.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagSetErrorCb(
    IN const AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN flexidag_error_cb cb,
    IN void* cb_param);

/**
 *  @brief      This API is used to get the performance measure or the performance profiling. The profiling result is for the period between start and stop event. \n
 *              Get the execution time information in us for the CV hardware specified by cvcore.  \n
 *              The cvcore is cvcore_type_e defined in cvtask_api.h. For example, CVCORE_VP is used to profile VP usage for the FlexiDAG.  \n
 *              The event specifies the start or stop of the profile the FlexiDAG.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      event : [IN] Start or stop of performance measure.  \n
 *                                  FLEXIDAG_PERF_START         0U  \n
 *                                  FLEXIDAG_PERF_STOP          1U
 *  @param      cvcore : [IN] Cvcore to performance measure.
 *  @param      flow_id_perf_array : [OUT] The structure array of performance measure result of flow ID.
 *  @param      num_of_array : [IN] Array size of structure.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagPerf(
    IN    AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    IN    uint32_t event,
    IN    uint32_t cvcore,
    OUT   AMBA_CV_FLEXIDAG_PERF_s *flow_id_perf_array,
    IN    uint32_t num_of_array);

/**
 *  @brief      Performance measure for all FlexiDAGs in one process.    \n
 *              Get the execution time information in us for the CV hardware specified by cvcore.  \n
 *              The cvcore is cvcore_type_e defined in cvtask_api.h. For example, CVCORE_VP is used to profile VP usage for the FlexiDAG.  \n
 *              The event specified the start or stop to profile the FlexiDAG. \n
 *
 *  @param      event : [IN] Start or stop of performance measure.  \n
 *                                  FLEXIDAG_PERF_START         0U          \n
 *                                  FLEXIDAG_PERF_STOP          1U
 *  @param      cvcore : [IN] Cvcore to performance measure.
 *  @param      flexidag_perf_array : [OUT] the structure array of performance measure result of FlexiDAG.
 *  @param      num_of_array : [IN] Array size of structure.
 *  @param      num_of_flexidag : [OUT] Number of FlexiDAG profiled.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagPerfAll(
    IN    uint32_t event,
    IN    uint32_t cvcore,
    OUT   AMBA_CV_FLEXIDAG_PERF_s *flexidag_perf_array,
    IN    uint32_t num_of_array,
    OUT   uint32_t *num_of_flexidag);

/**
 *  @brief      Close the flexiDAG and free the resources.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagClose(
    IN    AMBA_CV_FLEXIDAG_HANDLE_s *handle);

/**
 *  @brief      Get internal error number when API return CV_ERR_0003.
 *
 *  @param      handle : [IN] The handle of the FlexiDAG object.
 *  @param      retcode : [OUT] Internal error code.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagGetInternalError(
    IN  AMBA_CV_FLEXIDAG_HANDLE_s *handle,
    OUT uint32_t *retcode);

/**
 *  @brief      Get AmbaCV API handle from internal fd_handle.
 *
 *  @param      fd_handle : [IN] The fd_handle in AMBA_CV_FLEXIDAG_HANDLE_s.
 *  @param      handle : [OUT] The handle of the FlexiDAG object.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_FlexidagFdHandleToHandle(
    IN  void *fd_handle,
    OUT AMBA_CV_FLEXIDAG_HANDLE_s **handle);

/**
 *  @brief      Find metadata from FlexiDAG bin through metadata name.
 *
 *  @param      pFlexiBin : [IN] FlexiDAG bin.
 *  @param      pMetadataName : [IN] Name of metadata.
 *  @param      vppBuffer : [OUT] address of metadata in FlexiDAG bin.
 *  @param      pSize : [OUT] size of metadata in FlexiDAG bin.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */

uint32_t AmbaCV_FlexidagFindMetadata(
    IN void *pFlexiBin,
    IN const char *pMetadataName,
    OUT const void **vppBuffer,
    OUT uint32_t *pSize);

/**
 *  @brief      Initialize the AmbaCV IPC service.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_IpcInit(
    IN  void);
//======================== utiltity ==========================//
/**
 *  @brief      Get alignment size of file.
 *
 *  @param      path : [IN] The path of file.
 *  @param      size_align : [OUT] The alignment size of file.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityFileSize(
    IN    const char *path,
    OUT   uint32_t *size_align);

/**
 *  @brief      This function loads the file to the buffer.
 *
 *  @param      path : [IN] The path of file.
 *  @param      flexibin_buf : [IN] The buffer which the file loads to.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityFileLoad(
    IN    const char *path,
    IN    const flexidag_memblk_t *flexibin_buf);

/**
 *  @brief      The function allocates CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemAlloc(
    IN    uint32_t req_size,
    IN    uint32_t is_cached,
    OUT   flexidag_memblk_t *req_buf);

/**
 *  @brief      The function remap CMA buffer from the Linux kernel. The CMA buffer  is continuous in physical memory and aligned to 4K byte.
 *
 *  @param      req_addr : [IN] The physical addr of the request.
 *  @param      req_size : [IN] The size of the request.
 *  @param      is_cached : [IN] The cache type of the request.
 *  @param      req_buf : [OUT] The buffer of the request.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemRemap(
    IN    ulong req_addr,
    IN    uint32_t req_size,
    IN    uint32_t is_cached,
    OUT   flexidag_memblk_t *req_buf);

/**
 *  @brief      This function unmap CMA buffer.
 *
 *  @param      buf : [IN] The buffer to unmap.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemUnmap(
    IN    flexidag_memblk_t *buf);

/**
 *  @brief      This function frees CMA buffer.
 *
 *  @param      buf : [IN] The buffer to free.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemFree(
    IN    flexidag_memblk_t *buf);

/**
 *  @brief      Arm cache used to clean the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache clean operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemClean(
    IN    flexidag_memblk_t *buf);

/**
 *  @brief      Arm cache invalidates the CMA buffer.
 *
 *  @param      buf : [IN] The buffer used to run cache invalidates operation.
 *
 *  @return     0            on success.    \n
 *              CV_ERR_0000  on Interface error   \n
 *              CV_ERR_0001  on Scheduler error   \n
 *              CV_ERR_0002  on Flexidag error \n
 *              CV_ERR_0003  on Invalid error(not expect error category) \n
 */
uint32_t AmbaCV_UtilityCmaMemInvalid(
    IN    flexidag_memblk_t *buf);
/**
 * @}
 */
#endif // CVAPI_AMBACV_FlEXIDAG_H
