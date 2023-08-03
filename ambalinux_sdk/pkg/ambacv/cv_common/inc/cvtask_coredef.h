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

#ifndef CVTASK_COREDEF_H_FILE
#define CVTASK_COREDEF_H_FILE


#define CVTASK_MAX_INPUTS               16U
#define CVTASK_MAX_OUTPUTS              8U
#define CVTASK_MAX_FEEDBACK             4U
#define CVTASK_MAX_MESSAGES             16U
#define CVTASK_MAX_CVTASKMSG_POOL       4U
#define NAME_MAX_LENGTH                 32U
#define CVTABLE_NAME_MAX_LENGTH         120U
#define ENGINE_NAME_MAX_LENGTH          8U
#define CVTASK_MSG_MAX_LENGTH           1024U
#define BULK_MESSAGE_MAX_NUM            16U
#define SYSFLOW_MAX_CONFIG_DATA         8U

/**
 * cvtask_io_descriptor_t
 *
 */
typedef struct { /* cvtask_io_descriptor_s */
    char        io_name[NAME_MAX_LENGTH];
    uint32_t    history_needed    : 8;
    uint32_t    may_be_cached     : 1;
    uint32_t    dependency_only   : 1;

    /* Direct attach input to a given output # (uses io_name[0]) */
    uint32_t    direct_attach     : 1;
    uint32_t    shared_with_arm   : 1;

    /* Links size to input # (matching output) */
    uint32_t    link_size_to_input: 1;

    /* CVtask will manually manage cache - framework will not manage cache */
    uint32_t    manually_manage_cache :1;

    /* Do not use this connection's activity to determine run state for downstream tasks */
    uint32_t    ignore_activity   : 1;

    uint32_t    unused_flags      : 9;
    uint32_t    framework_reserved: 8;
    uint32_t    buffer_size;
} cvtask_io_descriptor_t;


/**
 * cvtask_memory_interface_t
 *
 */
typedef struct { /* cvtask_memory_interface_s */
    uint32_t    CMEM_temporary_scratchpad_needed;
    uint32_t    DRAM_temporary_scratchpad_needed;
    uint32_t    Instance_private_storage_needed;
    uint32_t    Instance_private_uncached_storage_needed;
    uint32_t    CVTask_shared_storage_needed;
    uint32_t    num_inputs          : 8;
    uint32_t    num_feedback        : 8;
    uint32_t    num_outputs         : 8;
    uint32_t    num_custom_msgpool  : 8;

    cvtask_io_descriptor_t  input[CVTASK_MAX_INPUTS];
    cvtask_io_descriptor_t  feedback[CVTASK_MAX_FEEDBACK];
    cvtask_io_descriptor_t  output[CVTASK_MAX_OUTPUTS];

    uint32_t      custom_msgpool_message_num[CVTASK_MAX_CVTASKMSG_POOL];
    uint32_t      custom_msgpool_message_size[CVTASK_MAX_CVTASKMSG_POOL];
} cvtask_memory_interface_t;

/**
 * cvtask_parameter_interface_t
 *
 *
 */

typedef struct { /* cvtask_parameter_interface_s */
    uint16_t    cvtask_core_id;
    uint16_t    cvtask_instance;

    uint16_t    cvtask_sysflow_index;
    uint16_t    cvtask_frameset_id;                         /* frameset_id for this call */

    uint8_t     cvtask_num_inputs;
    uint8_t     cvtask_num_feedback;
    uint8_t     cvtask_num_outputs;
    uint8_t     cvtask_num_messages;

    uint32_t    CMEM_temporary_scratchpad_size;             /* ORC only */
    uint32_t    DRAM_temporary_scratchpad_size;
    uint32_t    Instance_private_storage_size;
    uint32_t    Instance_private_uncached_storage_size;     /* ORC ONLY */
    uint32_t    CVTask_shared_storage_size;
    uint32_t    frameset_basetime;
    uint32_t    align_64bit_for_arm;

    void       *vpCMEM_temporary_scratchpad;                /* ORC only */
    void       *vpDRAM_temporary_scratchpad;
    void       *vpInstance_private_storage;
    void       *vpInstance_private_uncached_storage;        /* ORC ONLY */
    void       *vpCVTask_shared_storage;
    void       *vpInputBuffer[CVTASK_MAX_INPUTS];
    void       *vpFeedbackBuffer[CVTASK_MAX_FEEDBACK];
    void       *vpOutputBuffer[CVTASK_MAX_OUTPUTS];
    void       *vpMessagePayload[CVTASK_MAX_MESSAGES];

    uint16_t    input_frameset_id[CVTASK_MAX_INPUTS];
    uint16_t    feedback_frameset_id[CVTASK_MAX_FEEDBACK];

    uint8_t     input_error_flags[CVTASK_MAX_INPUTS];
    uint8_t     feedback_error_flags[CVTASK_MAX_FEEDBACK];
    uint8_t     message_error_flags[CVTASK_MAX_MESSAGES];
} cvtask_parameter_interface_t;

#endif /* !CVTASK_COREDEF_H_FILE */
