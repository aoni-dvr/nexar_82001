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

#ifndef CVTASK_API_H_FILE
#define CVTASK_API_H_FILE

#include "ambint.h"
#include "cvtask_coredef.h"

#define CVTASK_API_VERSION_AMALGAM_BIT    0x00008000U
#define CVTASK_API_VERSION_AMALGAM_MASK   0xFFFF7FFFU

#ifdef AMALGAM_DIAG
#define CVTASK_API_VERSION          (0x00000006U | CVTASK_API_VERSION_AMALGAM_BIT)
#else /* !AMALGAM_DIAG */
#define CVTASK_API_VERSION          0x00000006U
#endif /* ?AMALGAM_DIAG */

#define CVTASK_API_MASK             0x0000FFFFU

#define CVCHIP_CV1                  0x01000000U
#define CVCHIP_CV2                  0x02000000U
#define CVCHIP_CV22                 0x03000000U
#define CVCHIP_CV25                 0x04000000U
#define CVCHIP_CV2A                 0x05000000U
#define CVCHIP_CV28                 0x06000000U
#define CVCHIP_CV5                  0x07000000U
#define CVCHIP_CV6                  0x08000000U
#define CVCHIP_CV22A                0x09000000U
#define CVCHIP_CV52                 0x0A000000U
#define CVCHIP_MASK                 0xFF000000U

/**
 * SYSTEM_MAX_NUM_VISORC
 *
 * Defines the maximum number of VISORC cores available for the
 * scheduler
 */
#define SYSTEM_MAX_NUM_VISORC           4U

/**
 *  SYSTEM_MAX_NUM_CORTEX
 *
 *  Defines the maximum number of ARMs available for use by the
 *  scheduler
 */
#define SYSTEM_MAX_NUM_CORTEX           4U

#define IN
#define OUT

#define CVTASK_TYPE_IDSP                    ((uint32_t)(0x00U))
#define CVTASK_TYPE_SP                      ((uint32_t)(0x08U))
#define CVTASK_TYPE_OD                      ((uint32_t)(0x10U))
#define CVTASK_TYPE_FM                      ((uint32_t)(0x20U)) // Reserve 0x20..0x27 for multiple FMA engines
#define CVTASK_TYPE_FEX                     ((uint32_t)(0x28U)) // Reserve 0x28..0x2F for multiple FEX engines
#define CVTASK_TYPE_VPANY                   ((uint32_t)(0x30U))
#define CVTASK_TYPE_VP0                     ((uint32_t)(0x31U))
#define CVTASK_TYPE_VP1                     ((uint32_t)(0x32U))
#define CVTASK_TYPE_ORCANY                  ((uint32_t)(0x40U))
#define CVTASK_TYPE_ORCVP                   ((uint32_t)(0x41U))
#define CVTASK_TYPE_ORCSOD                  ((uint32_t)(0x42U))

#define CVTASK_TYPE_ARM                     ((uint32_t)(0x80U))
#define CVTASK_TYPE_ARM_FEEDER              ((uint32_t)(0x81U))
#define CVTASK_TYPE_ARM_LOGGER              ((uint32_t)(0x82U))
#define CVTASK_TYPE_ARM_BLOCKABLE           ((uint32_t)(0x83U))
#define CVTASK_TYPE_ARM_BLOCKABLE_FEEDER    ((uint32_t)(0x84U))

#define CVTASK_TYPE_NVP                     ((uint32_t)(0xA0U)) // Reserve 0xA0..0xAF for multiple NVP engines
#define CVTASK_TYPE_GVP                     ((uint32_t)(0xB0U)) // Reserve 0xB0..0xBF for multiple GVP engines

#define CVTASK_TYPE_FEEDER                  ((uint32_t)(0xF0U))
#define CVTASK_TYPE_SINK                    ((uint32_t)(0xF1U))
#define CVTASK_TYPE_FLEXIDAG_INPUT          ((uint32_t)(0xF8U))
#define CVTASK_TYPE_FLEXIDAG_FEEDBACK       ((uint32_t)(0xF9U))
#define CVTASK_TYPE_FLEXIDAG_OUTPUT         ((uint32_t)(0xFAU))
#define CVTASK_TYPE_RADAR_RX                ((uint32_t)(0xFCU))
#define CVTASK_TYPE_IDSP_RX                 ((uint32_t)(0xFDU))
#define CVTASK_TYPE_ERROR_HANDLER           ((uint32_t)(0xFEU))
#define CVTASK_TYPE_MASK                    ((uint32_t)(0xFFU))

/* Helper flag to mark if a CVTask supports cvtask_yield */
#define CVTASK_TYPE_BIT_YIELDABLE           ((uint32_t)(0x08000000U))

/* Needed to convert a task to an autorun task */
#define CVTASK_TYPE_BIT_AUTORUN             ((uint32_t)(0x10000000U))

/* Needed for marking as critical, for shutdown path */
#define CVTASK_TYPE_BIT_CRITICAL            ((uint32_t)(0x80000000U))

/* Needed to ensure 32-bit size for entry */
#define CVTASK_MAXINT_32BIT                 ((uint32_t)(0xFFFFFFFFU))

#define cvtask_type_e uint32_t

#define CVCORE_CA0                          ((uint32_t)(0U))
#define CVCORE_CA1                          ((uint32_t)(1U))
#define CVCORE_CA2                          ((uint32_t)(2U))
#define CVCORE_CA3                          ((uint32_t)(3U))
#define CVCORE_CA4                          ((uint32_t)(4U))
#define CVCORE_CA5                          ((uint32_t)(5U))
#define CVCORE_CA6                          ((uint32_t)(6U))
#define CVCORE_CA7                          ((uint32_t)(7U))
#define CVCORE_ORCSOD                       ((uint32_t)(8U))
#define CVCORE_ORCVP                        ((uint32_t)(9U))
#define CVCORE_VP0                          ((uint32_t)(10U))
#define CVCORE_VP1                          ((uint32_t)(11U))
#define CVCORE_FMA                          ((uint32_t)(12U))
#define CVCORE_SP                           ((uint32_t)(13U))
#define CVCORE_OD                           ((uint32_t)(14U))
#define CVHW_VP0                            ((uint32_t)(15U))
#define CVHW_VP1                            ((uint32_t)(16U))
#define CVHW_FMA                            ((uint32_t)(17U))
#define CVHW_SP                             ((uint32_t)(18U))
#define CVHW_OD                             ((uint32_t)(19U))
#define CVCORE_FEX                          ((uint32_t)(20U)) /* CV2/CV2A only */
#define CVHW_FEX                            ((uint32_t)(21U)) /* CV2/CV2A only */
#define CVCORE_SAFETY                       ((uint32_t)(22U)) /* CV2A only */
#define CVCORE_SCHED                        ((uint32_t)(23U))

#define CVCORE_NVP0                         ((uint32_t)(24U)) /* CV6 */
#define CVCORE_NVP1                         ((uint32_t)(25U)) /* CV6 */
#define CVCORE_NVP2                         ((uint32_t)(26U)) /* CV6 */
#define CVCORE_NVP3                         ((uint32_t)(27U)) /* CV6 */
#define CVCORE_NVP4                         ((uint32_t)(28U)) /* CV6 */
#define CVCORE_NVP5                         ((uint32_t)(29U)) /* CV6 */
#define CVCORE_GVP0                         ((uint32_t)(30U)) /* CV6 */
#define CVCORE_GVP1                         ((uint32_t)(31U)) /* CV6 */
#define CVCORE_FEX0                         ((uint32_t)(32U)) /* CV6 */
#define CVCORE_FEX1                         ((uint32_t)(33U)) /* CV6 */
#define CVCORE_FMA0                         ((uint32_t)(34U)) /* CV6 */
#define CVCORE_FMA1                         ((uint32_t)(35U)) /* CV6 */

#define CVCORE_MAX                          ((uint32_t)(36U))

#define cvcore_type_e uint32_t

/*******************************************************************************
 *
 *                To be implemented and registered by each cvtask
 *
 ******************************************************************************/
typedef struct { /* cvtask_entry_s */
    char      cvtask_name[NAME_MAX_LENGTH];

    /**
     * type of cvtask, set to CVTASK_TYPE_ARM for arm cvtask
     */
    uint32_t  cvtask_type;

    /**
     * cvtask_api_version used on this version of cvtask
     */
    uint32_t  cvtask_api_version;

    /**
     * @iid:      ID of the target instance
     * @config:   pointer to extra private information to the target
     *   instance. The info most likely come from system-flow table.
     * @interface:the memory resource requirement by cvtask
     *
     * First function to be called by scheduler, once and only once for
     *   each @instance.
     * Implementation must be re-entrent in the sense that multiple
     *   cvtask_query could be executed concurently with different
     *   @instance.
     */
    uint32_t (*cvtask_query) (
        IN  uint32_t iid,
        IN  const uint32_t *config,
        OUT cvtask_memory_interface_t *interface);

    /**
     * @env:      provides the execution environment for the operation
     * @config:   pointer to the same data as passed to cvtask_query
     *
     * Second function to be called by scheduler, once and only once for
     *   each @instance. Given the local and shared context, the
     *   implementation should prepare itself to run the task.
     * The scheduler guarantees that all cvtask_query are called
     *   before it calls any cvtask_init.
     * Implementation must be re-entrent in the sense that multiple
     *   cvtask_init could be executed concurently with different @instance.
     **/
    uint32_t (*cvtask_init) (
        IN  const cvtask_parameter_interface_t *env,
        IN  const uint32_t *config);

    /**
     * @env:      provides the execution environment for the operation
     * @info_id:  ID of the target info to be retrieved
     * @info_data:buffer to be filled with target info
     *
     * Get information of an instance.
     **/
#define CVTASK_INFO_ID_GET_SELF_INFO    0
#define CVTASK_INFO_ID_GET_TOOL_INFO    1
    uint32_t (*cvtask_get_info) (
        IN  const cvtask_parameter_interface_t *env,
        IN  uint32_t info_id,
        OUT void *info_data);

    /**
     * @env:      provides the execution environment for the operation
     *
     * Implementation must be re-entrent in the sense that multiple
     *   cvtask_run could be executed concurently with different @instance.
     **/
    uint32_t (*cvtask_run) (
        IN  const cvtask_parameter_interface_t *env);

    /**
     * @env:      provides the execution environment for the operation
     *
     * Implementation must be re-entrent in the sense that multiple
     *   cvtask_process_messages could be executed concurently with
     *   different @instance.
     **/
    uint32_t (*cvtask_process_messages) (
        IN  const cvtask_parameter_interface_t *env);

    /**
     * @env:      provides the execution environment for the operation
     *
     * Implementation must be re-entrent in the sense that multiple
     *   cvtask_process_messages could be executed concurently with
     *   different @instance.
     **/
    uint32_t (*cvtask_finish) (
        IN  const cvtask_parameter_interface_t *env);

} cvtask_entry_t;

/*******************************************************************************
 *
 *                Utility functions provided by framework
 *
 ******************************************************************************/
/**
 * @instance:   target instance name;  set to NULL if don't care
 * @algorithm:  target algorithm name; set to NULL if don't care
 * @step:       target step name;      set to NULL if don't care
 * @cvtask:     target cvtask name;    set to NULL if don't care
 * @index_array:array to store the indices for the cvtasks found
 * @array_size: the size of @index_array
 * @num_found:  address to hold the number of indices found
 *
 * This function get the sysflow index list of cvtasks that are name-matched
 * by @instance/@algorithm/@step/@cvtask.
 *
 */
uint32_t cvtask_get_sysflow_index_list(
    IN  const char  *instance,
    IN  const char  *algorithm,
    IN  const char  *step,
    IN  const char  *cvtask,
    OUT uint32_t    *index_array,
    IN  uint32_t    array_size,
    OUT uint32_t    *num_found);

/**
 * @uuid:     The UUID to look for
 * @pIndex:   Pointer to where to store the resulting index
 *
 * This function searches for UUID (uuid) in the current sysflow table.
 */
uint32_t cvtask_get_sysflow_index_by_UUID(
    IN   uint32_t flow_uuid,
    OUT  uint32_t *pIndex);

/**
 * @sysflow_index:      the sysflow index of target cvtask
 * @instance_name:      the pointer to hold the string address of instance
 * @algorithm_name:     the pointer to hold the string address of algorithm
 * @step_name:          the pointer to hold the string address of step
 * @cvtask_name:        the pointer to hold the string address of cvtask
 * @uuid:               the pointer to hold the uuid given by sysflow tabel
 *
 * This function get instance/algorithm/step/cvtask name of a cvtask identified
 * by @sysflow_index. The pointer can be set to NULL if don't care.
 *
 * if sysflow_index is set to SYSFLOW_INVALID_INDEX, then it
 * will return the information of the current cvtask.
 */
#define SYSFLOW_INVALID_INDEX   0xFFFFU

uint32_t cvtask_get_name(
    IN  uint32_t    sysflow_index,
    OUT const char  **instance_name,
    OUT const char  **algorithm_name,
    OUT const char  **step_name,
    OUT const char  **cvtask_name,
    OUT uint32_t    *flow_uuid);

/**
 * @pool_index:         index of cvtask_memory_interface_s.custom_msgpool
 * @payload_buf:        the message buffer pointer
 * @payload_max_size:   the maximum length of message buffer
 *
 * This function gets a free message buffer from custom_msgpool[@pool_index].
 * If pool_index = DEFAUT_MESSAGE_POOL_INDEX, a system message buffer is
 * returned instead.
 *
 */
#define DEFAULT_MESSAGE_POOL_INDEX      0xFFFFFFFFU
uint32_t cvtask_message_create(
    IN  uint32_t pool_index,
    OUT void     **payload_buf,
    OUT uint32_t *payload_max_size);

/**
 * @payload_buf:        the message buffer pointer
 * @payload_len:        the length of message buffer
 * @dst_sysflow_index:  index of destination cvtask
 * @target_frameset_id: frameset_id on which the payload is
 *                      delivered can be CVTASK_MESSAGE_ASAP
 *                      also
 *
 * This function sends a message to target cvtask asynchronously. The message
 * payload @payload_buf must be acquired by cvtask_message_create.
 * The payload is delivered by pointer to destination cvtask at frame.
 * @target_frameset_id.
 *
 */
#define CVTASK_MESSAGE_ASAP             0xFFFFFFFFU
uint32_t cvtask_message_send(
    IN  const void *payload_buf,
    IN  uint32_t payload_len,
    IN  uint32_t dst_sysflow_index,
    IN  uint32_t target_frameset_id);

/**
 * @payload_buf:        the message buffer pointer
 * @payload_len:        the length of message buffer
 * @dst_sysflow_index:  index of destination cvtask
 * @target_time:        target time (audio clock) at which this
 *                      message will be delivered
 *
 * This function sends a message to target cvtask asynchonously. The message
 * payload @payload_buf must be acquired by cvtask_message_create.
 * The payload is delivered by pointer to destination cvtask at after @target_time
 *
 */
uint32_t cvtask_message_send_at_time(
    IN  const void *payload_buf,
    IN  uint32_t payload_len,
    IN  uint32_t dst_sysflow_index,
    IN  uint32_t target_time);

/**
 * @payload_buf:        the message buffer pointer
 * @payload_len:        the length of message buffer
 * @dst_sysflow_index:  index of destination cvtask
 * @target_frameset_id: frameset_id on which the payload is
 *                      delivered, can be CVTASK_MESSAGE_ASAP
 *                      also
 *
 * This function sends a message to target cvtask asynchronously. The message
 * payload @payload_buf is copied into system message queue.
 * The receiving cvtask gets the message when it is scheduled to run at frame
 * @target_frameset_id.
 *
 */
uint32_t cvtask_default_message_send(
    IN  const void *payload_buf,
    IN  uint32_t payload_len,
    IN  uint32_t dst_sysflow_index,
    IN  uint32_t target_frameset_id);

/**
 * @payload_buf:        the message buffer pointer
 * @payload_len:        the length of message buffer
 * @dst_sysflow_index:  index of destination cvtask
 * @target_time:        target time (audio clock) at which this
 *                      message will be delivered
 *
 * This function sends a message to target cvtask asynchronously. The message
 * payload @payload_buf is copied into system message queue.
 * The receiving cvtask gets the message when it is scheduled to run at time
 * @target_time.
 *
 */
uint32_t cvtask_default_message_send_at_time(
    IN  const void *payload_buf,
    IN  uint32_t payload_len,
    IN  uint32_t dst_sysflow_index,
    IN  uint32_t target_time);

/**
 * @num_messages:       number of bulk messsages
 * @payload_buf:        array of message buffer
 * @payload_len:        array of message length
 * @payload_dst:        array of message destination (sysflow indices)
 * @target_frameset_id: frameset_id on which the payload is
 *                      delivered, can be CVTASK_MESSAGE_ASAP
 *                      also
 *
 * This function sends a list of messages to target cvtask asynchronizely.
 * At @target_frameset_id, these messeages are delivered to their destinations.
 *
 */
uint32_t cvtask_message_bulk_send(
    IN  uint32_t num_messages,
    IN  void*    *payload_buf,
    IN  uint32_t *payload_len,
    IN  uint16_t *payload_dst,
    IN  uint32_t target_frameset_id);

/**
 * @target_sysflow_index: the sysflow index of target cvtask
 * @ppBuffer:             Pointer to message to send
 *
 * This function sends a message (max 224 bytes) to the target
 * cvtask.
 */
uint32_t cvtask_send_private_msg(
    IN uint32_t target_sysflow_index,
    IN const uint8_t *pMessage);

#define CVTASK_PRIVATE_MSG_MAX_SIZE           224U

/**
 * @sysflow_index:      the sysflow index of target cvtask
 * @source_info:        receives string pointer of the source-code version
 * @tool_info:          receives string pointer of the build-tool version
 *
 * This function get address of build info of cvtask identified by
 * by @sysflow_index into *@build_info.
 *
 */
uint32_t cvtask_get_buildinfo(
    IN  uint32_t    sysflow_index,
    OUT const char  **source_info,
    OUT const char  **tool_info);

/**
 * @name:  the name of target auxiliary data chunk
 * @ptr:   address of pointer to receive target data address
 * @size:  address of integer to receive target data size
 *
 * This function retrieve the buffer of auxiliary data identified by @name
 *
 */
uint32_t cvtable_find(
    IN  const char *name,
    OUT const void **ptr,
    OUT uint32_t *size);

/**
 * @state:  Set to non-zero to suppress prints related to cvtables
 *          not being loaded, or tables not found.
 */
void cvtable_suppress_notfound_print(
    IN  uint32_t state);

/**
 * @vpMetaDataBlock:    Pointer to the meta data block
 * @MetaDataBlockSize:  Size of the meta data block
 * @metadata_type:      Unique metadata-type to find
 * @vppMetaDataBuffer:  Pointer to where to return result
 * @pMetaDataSize:      Pointer to size of the metadata
 *
 * This function scans through vpMetaDataBlock, and attempts to
 * locate metadata of type (metadata_type).  If it find it, it
 * returns a non-NULL value in vppMetaDataBuffer, and a non-zero
 * size.
 */
uint32_t cvtask_metadata_find(
    IN  const void *vpMetaDataBlock,
    IN  uint32_t MetaDataBlockSize,
    IN  uint32_t metadata_type,
    OUT const void **vppMetaDataBuffer,
    OUT uint32_t *pMetaDataSize);

/**
 * This function returns the current debug level of the cvtask
 */
uint32_t cvtask_get_debug_level(void);

/**
 * @index: the index of input IO
 *
 * This function returns the buffer size of input IO indexed by @index
 */
uint32_t cvtask_get_input_size(
    IN uint32_t index);

/**
 * @index: the index of feedback IO
 *
 * This function returns the buffer size of feedback IO indexed
 * bqy @index
 */
uint32_t cvtask_get_feedback_size(
    IN uint32_t index);

/**
 * @index: the index of output IO
 *
 * This function returns the buffer size of output IO indexed by
 * @index
 */
uint32_t cvtask_get_output_size(
    IN uint32_t index);

/**
 * @num_entries:        list size
 * @sysflow_index:      list of ctask sysflow indices
 * @debug_leve:         list of debug levels
 *
 * Set debug level of a list of cvtasks.
 *
 * To set the debug level for ALL cvtasks, set
 *              @num_entries = CVTASK_SET_DEBUG_LEVEL_ALL
 * and the value of debug_level[0] will be used as the debug level.
 **/
#define CVTASK_SET_DEBUG_LEVEL_ALL       0xFFFFFFFFU
uint32_t cvtask_set_debug_level(
    IN uint32_t num_entries,
    IN const uint16_t *sysflow_index,
    IN const uint16_t *debug_level);

/**
 * @num_entries : Number of entries to modify
 * @sysflow_index_array: List of sysflow_index to modify
 * @cvtask_frequency: frequency of cvtask
 *
 * To set the frequency for all cv tasks, set
 *   @num_entries = CVTASK_SET_FREQUENCY_ALL
 * and the value of @cvtask_frequency[0] will be used as the
 * frequency.
 */
#define CVTASK_SET_FREQUENCY_ALL    0xFFFFFFFFU
#define CVTASK_FREQUENCY_ON_DEMAND  0xFFU
uint32_t cvtask_set_frequency(
    IN uint32_t num_entries,
    IN const uint16_t *sysflow_index_array,
    IN const uint8_t  *cvtask_frequency);

/**
 * @num_entries : Number of entries to modify
 * @target_frametime : Target time on which to target.
 * @sysflow_index_array: List of sysflow_index to modify
 * @cvtask_frequency: frequency of cvtask
 *
 * To set the frequency for all cv tasks, set
 *   @num_entries = CVTASK_SET_FREQUENCY_ALL
 * and the value of @cvtask_frequency[0] will be used as the
 * frequency.
 */
uint32_t cvtask_set_frequency_at_time(
    IN uint32_t num_entries,
    IN uint32_t target_frametime,
    IN const uint16_t *sysflow_index_array,
    IN const uint8_t  *cvtask_frequency);

/**
 * @num_entries : Number of entries to modify
 * @target_frameset_id : Frameset_id on which to target,
 *                     supports CVTASK_MESSAGE_ASAP as well.
 * @sysflow_index_array: List of sysflow_index to modify
 * @cvtask_frequency: frequency of cvtask
 *
 * To set the frequency for all cv tasks, set
 *   @num_entries = CVTASK_SET_FREQUENCY_ALL
 * and the value of @cvtask_frequency[0] will be used as the
 * frequency.
 */
uint32_t cvtask_set_frequency_at_frameset_id(
    IN uint32_t num_entries,
    IN uint32_t target_frameset_id,
    IN const uint16_t *sysflow_index_array,
    IN const uint8_t  *cvtask_frequency);

/**
 * @output_index : Marks an output to be early early
 */
uint32_t  cvtask_make_output_available_early(
    IN uint32_t output_index);

#ifndef ORCVIS
/**
 * @va:    the vitual pointer
 *
 * This function returns the offset from DRAM base given a virtual address
 */
uint32_t   cvtask_v2off(void *va);

#endif /* ?ORCVIS */
/**
 * @cvcore:        the cvcore that runs the cvtask
 * @sysflow_index: the sysflow index of the cvtask
 * @frame_num:     the frame number that the cvtask is processing
 *
 * This function indicates a new frame is about be processed. It should
 * be the first cvtask_report call for the the frame
 */
uint32_t cvtask_prof_new_action(
    IN uint8_t  cvcore,
    IN uint16_t sysflow_index,
    IN uint32_t frame_num);

/**
 * @cvcore:        the cvcore that runs the cvtask
 * @sysflow_index: the sysflow index of the cvtask
 * @frame_num:     the frame number that the cvtask is processing
 *
 * This function report to profiling system that the cvcore starts executing.
 */
uint32_t cvtask_prof_unit_on(
    IN uint8_t  cvcore,
    IN uint16_t sysflow_index);

/**
 * @cvcore:        the cvcore that runs the cvtask
 * @sysflow_index: the sysflow index of the cvtask
 * @frame_num:     the frame number that the cvtask is processing
 *
 * This function report to profiling system that the cvcore stops executing.
 */
uint32_t cvtask_prof_unit_off(
    IN uint8_t  cvcore,
    IN uint16_t sysflow_index);

#ifdef ORCVIS

#define cvtask_get_timestamp    get_cur_time

/**
 * This function checks if the current VISORC-based cvtask needs to yield.
 */
uint32_t  cvtask_check_yield(void);

#else /* !ORCVIS */

/**
 *
 * This function returns the current timestamp. The timestamp is read from
 * a SoC-wide clock and can be used to compare to the timestamp from the
 * other parts of SoC, for example, Vis-ORC.
 */
uint32_t cvtask_get_timestamp(void);

/*******************************************************************************
 *
 *                            cvtask entry initialization
 *
 ******************************************************************************/
/**
 * @entry:      pointer to the task entry struct
 * @version:    version number of this API. Used to cross check the version
 *              compatibility between the cvtask and the framework.
 *
 * This function registers the cvtask with the framework.
 *
 * In case that a cvtask and the framework are compiled into separate lib,
 *   we use version number to make sure they follow exactly the same API.
 */
uint32_t cvtask_register(
    IN  const cvtask_entry_t *entry,
    IN  uint32_t version);

#ifndef __KERNEL__
/* define cvtask init call prototype */
typedef uint32_t (*initcall_t)(void);
#endif /* ?__KERNEL__ */

#ifndef CONFIG_BUILD_CV_THREADX
/**
 * for function @foo, the macro cvtask_declare_create define a varible
 *                  __initcall_foo
 * The varible is set to the address of foo and linked into an exclusive
 *   section ".cvtask_create_fp".
 * At system initialization, we iterate and call each function in the
 *   .cvtask_create_fp section.
 * It is recommendated to define each foo as "static" since it is not called
 *   upon directly and multiple cvtaskes can have the same name for its own
 *   init function.
 */
#define cvtask_declare_create(foo) \
    static  initcall_t __initcall_##foo \
    __attribute__((used,section(".cvtask_create_fp"))) = foo;

/*******************************************************************************
 *
 *                            cvtask printf utility
 *
 ******************************************************************************/

/**
 * Output info into system console only. Just like printf
 * a "\n" is attached to the end of @fmt
 */
extern int32_t AmbaPrint(const char *fmt, ...);

enum {
    LVL_SILENT = 0,
    LVL_CRITICAL,
    LVL_MINIMAL,
    LVL_NORMAL,
    LVL_VERBOSE,
    LVL_DEBUG,
    NUM_DBG_LEVELS
};

extern void cvtask_printf(int32_t lvl, const char *fmt, uint32_t v0, uint32_t v1,
                          uint32_t v2, uint32_t v3, uint32_t v4);

#define cvtpf_norm(a, b, c, d, e)                                                    \
    (uint32_t)a, (uint32_t)b, (uint32_t)c, (uint32_t)d, (uint32_t)e

#define M0(lvl, fmt)                 cvtask_printf(lvl, fmt, cvtpf_norm(0,0,0,0,0))
#define M1(lvl, fmt, a)              cvtask_printf(lvl, fmt, cvtpf_norm(a,0,0,0,0))
#define M2(lvl, fmt, a, b)           cvtask_printf(lvl, fmt, cvtpf_norm(a,b,0,0,0))
#define M3(lvl, fmt, a, b, c)        cvtask_printf(lvl, fmt, cvtpf_norm(a,b,c,0,0))
#define M4(lvl, fmt, a, b, c, d)     cvtask_printf(lvl, fmt, cvtpf_norm(a,b,c,d,0))
#define M5(lvl, fmt, a, b, c, d, e)  cvtask_printf(lvl, fmt, cvtpf_norm(a,b,c,d,e))

#define MP(lvl, fmt, _1, _2, _3, _4, _5, MACRO, ...) MACRO

/**
 * Output info into logger so it can be transferred outside schdeuler framework
 *
 * This function has following limitations:
 *    1. It can only take up to 5 scalar arguments.
 *    2. 64-bit data type must be split into two 32-bit data type.
 *    3. String(%s) is not supported.
 */
#define CVTASK_PRINTF(lvl, fmt, ...) {                                          \
    static const char pf[] __attribute__((section(".cvtask_format"))) = fmt;\
    MP(lvl, fmt, ##__VA_ARGS__, M5, M4, M3, M2, M1, M0) \
        (lvl, pf, ##__VA_ARGS__); \
    }
#else
#define GNU_SECTION_CVTASK_CREATE_FP __attribute__((used,section(".cvtask_create_fp")))

enum {
    LVL_SILENT = 0,
    LVL_CRITICAL,
    LVL_MINIMAL,
    LVL_NORMAL,
    LVL_VERBOSE,
    LVL_DEBUG,
    NUM_DBG_LEVELS
};

extern void cvtask_printf(int32_t lvl, const char *fmt, uint32_t v0, uint32_t v1,
                          uint32_t v2, uint32_t v3, uint32_t v4);
#endif

#endif //!ORCVIS

#endif /* !CVTASK_API_H_FILE */
