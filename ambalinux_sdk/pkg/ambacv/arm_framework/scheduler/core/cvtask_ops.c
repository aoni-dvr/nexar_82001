/**
 *  @file cvtask_ops.c
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
 *  @details Cvtask OPS APIs
 *
 */

#include "os_api.h"
#include "dram_mmap.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "cvtask_errno.h"
#include "cvapi_logger_interface.h"

#ifndef DISABLE_ARM_CVTASK
#define MAX_CLASS_NUM          510U
#define MAX_LOGGER_NUM         32U

typedef struct {
    uint32_t total_class;
    uint32_t reserved;
    const cvtask_entry_t *entry[MAX_CLASS_NUM];
} cvtask_database_t;

typedef struct {
    uint32_t total_instance;
    char*   private_storage[MAX_LOGGER_NUM];
    const cvtask_entry_t *entry[MAX_LOGGER_NUM];
} logger_database_t;

static cvtask_database_t   ddb GNU_SECTION_NOZEROINIT;
static logger_database_t   ldb GNU_SECTION_NOZEROINIT;
static uint32_t    cvcore_base = (uint32_t)CVCORE_CA0;
// Provide a default dram scratchpad size for flexidag

uint32_t is_blockable_cvtask(uint32_t cvtask_id)
{
    uint32_t retval;
    if (ddb.entry[cvtask_id] == NULL) {
        retval = 0U;
    } /* if (ddb.entry[cvtask_id] == NULL) */
    else if ((ddb.entry[cvtask_id]->cvtask_type == (uint32_t)CVTASK_TYPE_ARM_BLOCKABLE) ||
             (ddb.entry[cvtask_id]->cvtask_type == (uint32_t)CVTASK_TYPE_ARM_BLOCKABLE_FEEDER)) {
        retval = 1U;
    } else {
        retval = 0U;
    }
    return retval;
}

static uint32_t cvtask_handle_reg(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_register_t *req;
    schedmsg_cvtask_register_reply_t *rpl;
    const cvtask_entry_t *node;
    const char *dest_ptr = NULL;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_reg() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        req = &msg->msg.cvtask_register_msg;
        rpl = &msg->msg.reply_cvtask_register;
        node = ddb.entry[req->arm_cvtask_index];

        if(thread_memset(rpl->cvtask_name, 0, NAME_MAX_LENGTH) != ERRCODE_NONE) {
            retcode = CVTASK_ERR_GENERAL;
        } else {
            if (req->arm_cvtask_index >= (uint32_t)ddb.total_class) {
                console_printU5("[ERROR] cvtask_handle_reg() : index %d out of range",
                                req->arm_cvtask_index, 0U, 0U, 0U, 0U);
                msg->hdr.message_retcode  = 0xFFFF;
                retcode = CVTASK_ERR_INDEX_TOO_LARGE;
            } else {
                dest_ptr = thread_strcpy(rpl->cvtask_name, node->cvtask_name);
                rpl->arm_cvtask_type = node->cvtask_type;

                // MASTER is not aware of CVTASK_TYPE_ARM_LOGGER, CVTASK_TYPE_ARM_BLOCKABLE, CVTASK_TYPE_ARM_BLOCKABLE_FEEDER
                if (rpl->arm_cvtask_type == (uint32_t)CVTASK_TYPE_ARM_LOGGER) {
                    rpl->arm_cvtask_type = (uint32_t)CVTASK_TYPE_ARM;
                }

                if (rpl->arm_cvtask_type == (uint32_t)CVTASK_TYPE_ARM_BLOCKABLE) {
                    rpl->arm_cvtask_type = (uint32_t)CVTASK_TYPE_ARM;
                }

                if (rpl->arm_cvtask_type == (uint32_t)CVTASK_TYPE_ARM_BLOCKABLE_FEEDER) {
                    rpl->arm_cvtask_type = (uint32_t)CVTASK_TYPE_ARM_FEEDER;
                }

                if (node->cvtask_finish != NULL) {
                    rpl->has_finish_method = 1U;
                } else {
                    rpl->has_finish_method = 0U;
                }
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(*rpl);
    }
    (void) dest_ptr;
    return retcode;
}

static uint32_t cvtask_handle_query(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_query_t *req;
    const cvtask_entry_t *node;
    cvtask_memory_interface_t *interface;
    cvtask_thpool_env_t *env;
    schedmsg_cvtask_run_t run_req;
    const void* ptr;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_query() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            req = &msg->msg.cvtask_query;
            node = ddb.entry[req->arm_cvtask_index];

            run_req.sysflow_index       = req->sysflow_index;
            run_req.cvtask_debug_level  = req->cvtask_debug_level;
            env->run_req = &run_req;
            ptr = ambacv_c2v(req->cvtask_memory_interface_daddr);
            if(ptr == NULL) {
                retcode = CVTASK_ERR_NULL_POINTER;
            } else {
                typecast(&interface,&ptr);
                msg->hdr.message_retcode  = node->cvtask_query((uint32_t)req->cvtask_instance,
                                            req->config_data, interface);
                if (schdr_sys_cfg.max_sp_size < interface->DRAM_temporary_scratchpad_needed) {
                    if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) {
                        // FlexiDAG
                        console_printU5("[ERROR] cvtask_handle_query() : pre-allocated DRAM scratchpad is not enough, allocated = 0x%x, needed = 0x%x\n",
                                        schdr_sys_cfg.max_sp_size, interface->DRAM_temporary_scratchpad_needed, 0U, 0U, 0U);
                        msg->hdr.message_retcode  = (uint32_t)ERR_ALLOCFAIL_ARM_DRAM_SCRATCHPAD;
                        retcode = CVTASK_ERR_GENERAL;
                    } else {
                        // SuperDAG: calculate max scratchpad size
                        schdr_sys_cfg.max_sp_size = interface->DRAM_temporary_scratchpad_needed;
                    }
                }
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(schedmsg_cvtask_query_reply_t);
    }
    return retcode;
}

static uint32_t cvtask_handle_init(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_init_t *req;
    const cvtask_entry_t *node;
    cvtask_parameter_interface_t interface;
    cvtask_thpool_env_t *env;
    schedmsg_cvtask_run_t run_req;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_init() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            req = &msg->msg.cvtask_init;
            node = ddb.entry[req->arm_cvtask_index];

            run_req.sysflow_index                   = req->sysflow_index;
            run_req.cvtask_debug_level              = req->cvtask_debug_level;
            run_req.cvtask_num_custom_message_pool  = req->cvtask_num_custom_message_pool;
            if(thread_memcpy(run_req.CVTaskCustomMsgPool_daddr, req->CVTaskCustomMsgPool_daddr, sizeof(req->CVTaskCustomMsgPool_daddr)) != ERRCODE_NONE) {
                retcode = CVTASK_ERR_GENERAL;
            }
            run_req.cvtask_frameset_id    = 0U;
            env->run_req = &run_req;

            if(retcode == ERRCODE_NONE) {
                if(thread_memset(&interface, 0, sizeof(interface)) != ERRCODE_NONE) {
                    retcode = CVTASK_ERR_GENERAL;
                } else {
                    interface.cvtask_core_id = (uint16_t)(cvcore_base);
                    interface.cvtask_instance = req->cvtask_instance;
                    interface.cvtask_sysflow_index = req->sysflow_index;
                    interface.Instance_private_storage_size = req->Instance_private_storage_size;
                    if(req->Instance_private_storage_daddr != 0U) {
                        interface.vpInstance_private_storage = ambacv_c2v(req->Instance_private_storage_daddr);
                    } else {
                        interface.vpInstance_private_storage = NULL;
                    }
                    interface.CVTask_shared_storage_size = 0;
                    interface.vpCVTask_shared_storage = NULL;
                    interface.DRAM_temporary_scratchpad_size = env->scratchpad_size;
                    interface.vpDRAM_temporary_scratchpad = env->scratchpad_base;

                    // invoke registered cvtask_init function
                    msg->hdr.message_retcode  = node->cvtask_init(&interface, req->config_data);

                    // if this is a logger task, insert it to logger database
                    if (node->cvtask_type == (uint32_t)CVTASK_TYPE_ARM_LOGGER) {
                        uint32_t index = ldb.total_instance++;
                        if (index >= MAX_LOGGER_NUM) {
                            console_printU5("[ERROR] cvtask_handle_init() : Too many Logger instance!", 0U, 0U, 0U, 0U, 0U);
                            retcode = CVTASK_ERR_INDEX_TOO_LARGE;
                        } else {
                            typecast(&ldb.private_storage[index],&interface.vpInstance_private_storage);
                            ldb.entry[index] = node;
                        }
                    }
                }
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(schedmsg_cvtask_init_reply_t);
    }
    return retcode;
}

static uint32_t cvtask_handle_run_prepare_membuf(cvtask_parameter_interface_t *pinterface, const schedmsg_cvtask_run_t *req)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t i;
    const void* ptr;
    const cvmem_membuf_t *membuf;

    (void) req;
    for (i = 0U; i < pinterface->cvtask_num_inputs; i++) {
        if (req->InputMemBuf_daddr[i] == 0U) {
            /* special case for dependency-only input */
            pinterface->vpInputBuffer[i] = NULL;
            pinterface->input_frameset_id[i] = req->cvtask_frameset_id;
            continue;
        }

        ptr = ambacv_c2v(req->InputMemBuf_daddr[i]);
        if(ptr == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
            console_printU5("[ERROR] cvtask_handle_run_prepare_membuf() : input (%d) membuf == NULL!", i, 0U, 0U, 0U, 0U);
            break;
        } else {
            typecast(&membuf,&ptr);
            pinterface->input_frameset_id[i] =
                membuf->hdr_variable.membuf_frameset_id;
            if(membuf->hdr_variable.payload_daddr != 0U) {
                pinterface->vpInputBuffer[i] = ambacv_c2v(membuf->hdr_variable.payload_daddr);
            } else {
                pinterface->vpInputBuffer[i] = NULL;
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        for (i = 0U; i < pinterface->cvtask_num_feedback; i++) {
            if (req->FeedbackMemBuf_daddr[i] == 0U) {
                /* special case for frame zero */
                pinterface->vpFeedbackBuffer[i] = NULL;
                continue;
            }

            ptr = ambacv_c2v(req->FeedbackMemBuf_daddr[i]);
            if(ptr == NULL) {
                retcode = CVTASK_ERR_NULL_POINTER;
                console_printU5("[ERROR] cvtask_handle_run_prepare_membuf() : feedback (%d) membuf == NULL!", i, 0U, 0U, 0U, 0U);
                break;
            } else {
                typecast(&membuf,&ptr);
                pinterface->feedback_frameset_id[i] =
                    membuf->hdr_variable.membuf_frameset_id;
                if(membuf->hdr_variable.payload_daddr != 0U) {
                    pinterface->vpFeedbackBuffer[i] = ambacv_c2v(membuf->hdr_variable.payload_daddr);
                } else {
                    pinterface->vpFeedbackBuffer[i] = NULL;
                }
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        for (i = 0U; i < pinterface->cvtask_num_messages; i++) {
            const cvmem_messagebuf_t *cvmsg;

            ptr = ambacv_c2v(req->CVTaskMessage_daddr[i]);
            if(ptr == NULL) {
                retcode = CVTASK_ERR_NULL_POINTER;
                console_printU5("[ERROR] cvtask_handle_run_prepare_membuf() : message (%d) cvmsg == NULL!", i, 0U, 0U, 0U, 0U);
                break;
            } else {
                typecast(&cvmsg,&ptr);
                if(cvmsg->hdr_variable.message_payload_daddr != 0U) {
                    pinterface->vpMessagePayload[i] = ambacv_c2v(cvmsg->hdr_variable.message_payload_daddr);
                } else {
                    pinterface->vpMessagePayload[i] = NULL;
                }
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        for (i = 0U; i < pinterface->cvtask_num_outputs; i++) {
            ptr = ambacv_c2v(req->OutputMemBuf_daddr[i]);
            if(ptr == NULL) {
                retcode = CVTASK_ERR_NULL_POINTER;
                console_printU5("[ERROR] cvtask_handle_run_prepare_membuf() : output (%d) membuf == NULL!", i, 0U, 0U, 0U, 0U);
                break;
            } else {
                typecast(&membuf,&ptr);
                if(membuf->hdr_variable.payload_daddr != 0U) {
                    pinterface->vpOutputBuffer[i] = ambacv_c2v(membuf->hdr_variable.payload_daddr);
                } else {
                    pinterface->vpOutputBuffer[i] = NULL;
                }
            }
        }
    }

    return retcode;
}

static uint32_t cvtask_handle_run_prepare_interface(cvtask_parameter_interface_t *pinterface, const schedmsg_cvtask_run_t *req, const cvtask_thpool_env_t *env)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) env;
    if(thread_memset(pinterface, 0, sizeof(cvtask_parameter_interface_t)) != ERRCODE_NONE) {
        retcode = CVTASK_ERR_GENERAL;
    } else {
        pinterface->cvtask_core_id = (uint16_t)(cvcore_base);
        pinterface->cvtask_instance = req->cvtask_instance;
        pinterface->cvtask_sysflow_index = req->sysflow_index;
        pinterface->cvtask_frameset_id = req->cvtask_frameset_id;
        pinterface->cvtask_num_inputs = req->cvtask_num_inputs;
        pinterface->cvtask_num_outputs = req->cvtask_num_outputs;
        pinterface->cvtask_num_feedback = req->cvtask_num_feedback;
        pinterface->cvtask_num_messages = req->cvtask_num_messages;
        pinterface->Instance_private_storage_size = req->Instance_private_storage_size;
        if(req->Instance_private_storage_daddr != 0U) {
            pinterface->vpInstance_private_storage = ambacv_c2v(req->Instance_private_storage_daddr);
        } else {
            pinterface->vpInstance_private_storage = NULL;
        }
        pinterface->CVTask_shared_storage_size = 0;
        pinterface->vpCVTask_shared_storage = NULL;
        pinterface->frameset_basetime = req->frameset_basetime;
        pinterface->DRAM_temporary_scratchpad_size = env->scratchpad_size;
        pinterface->vpDRAM_temporary_scratchpad = env->scratchpad_base;

        retcode = cvtask_handle_run_prepare_membuf(pinterface, req);
    }
    return retcode;
}

static uint32_t cvtask_handle_run(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    schedmsg_cvtask_run_t *req;
    schedmsg_cvtask_run_reply_t *rpl;
    const cvtask_entry_t *node;
    cvtask_parameter_interface_t interface;
    uint32_t i, cvtask_uuid;
    cvtask_thpool_env_t *env;
    const void* ptr;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_run() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            req = &msg->msg.cvtask_run;
            rpl = &msg->msg.reply_cvtask_run;
            node = ddb.entry[req->arm_cvtask_index];

            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                cvtask_uuid = schdr_sys_state.pSysFlow[req->sysflow_index].uuid;
            } else {
                cvtask_uuid = flexidag_sys_state[env->flexidag_slot_id].pSysFlow[req->sysflow_index].uuid;
            }
            SCHDR_PRINTF(".cvtask_run start, uuid=%d, frame=%d, cpu=%d",
                         cvtask_uuid, req->cvtask_frameset_id, env->cpu_id, 0U, 0U);
            env->run_req = req;
            retcode = cvtask_handle_run_prepare_interface(&interface, req, env);

            if(retcode == ERRCODE_NONE) {
                env->reserved = 0U;
                env->reserved |= (env->cpu_id & 0xFU);
                if (is_blockable_cvtask(req->arm_cvtask_index) != 0U) {
                    env->reserved |= 0x80U;
                }
                retcode = cvtask_prof_new_action((uint8_t)(cvcore_base), req->sysflow_index,
                                                 req->cvtask_frameset_id);
            }
            if(retcode == ERRCODE_NONE) {
                retcode = cvtask_prof_unit_on((uint8_t)(cvcore_base), req->sysflow_index);
                msg->hdr.message_retcode  = node->cvtask_run(&interface);
            }
            if(retcode == ERRCODE_NONE) {
                retcode = cvtask_prof_unit_off((uint8_t)(cvcore_base), req->sysflow_index);
            }

            if(retcode == ERRCODE_NONE) {
                for (i = 0U; i < interface.cvtask_num_messages; i++) {
                    cvmem_messagebuf_t *cvmsg;

                    ptr = ambacv_c2v(req->CVTaskMessage_daddr[i]);
                    if(ptr == NULL) {
                        retcode = CVTASK_ERR_NULL_POINTER;
                        console_printU5("[ERROR] cvtask_handle_run() : message (%d) cvmsg == NULL!", i, 0U, 0U, 0U, 0U);
                        break;
                    } else {
                        typecast(&cvmsg,&ptr);
                        cvmsg->hdr_variable.message_processed = 1U;
                    }
                }
                SCHDR_PRINTF(".cvtask_run done,  uuid=%d, frame=%d, cpu=%d",
                             cvtask_uuid, req->cvtask_frameset_id, env->cpu_id, 0U, 0U);
                rpl->scheduler_tasknode_id = req->scheduler_tasknode_id;
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(*rpl);
    }
    return retcode;
}

static uint32_t cvtask_handle_process_msg(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    schedmsg_cvtask_run_t *req;
    schedmsg_cvtask_run_reply_t *rpl;
    const cvtask_entry_t *node;
    cvtask_parameter_interface_t interface;
    uint32_t i;
    const void* ptr;
    cvtask_thpool_env_t *env;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_process_msg() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            req = &msg->msg.cvtask_run;
            rpl = &msg->msg.reply_cvtask_run;
            node = ddb.entry[req->arm_cvtask_index];

            /* check if cvtask implments "process_message" */
            if (node->cvtask_process_messages == NULL) {
                console_printS5(
                    "[ERROR] cvtask_handle_process_msg() : [%s] doesn't implement process_messages, exit!\n",
                    node->cvtask_name,
                    NULL, NULL, NULL, NULL);
                retcode = CVTASK_ERR_NULL_POINTER;
            } else {
                env->run_req = req;

                if(thread_memset(&interface, 0, sizeof(interface)) != ERRCODE_NONE ) {
                    retcode = CVTASK_ERR_GENERAL;
                } else {
                    interface.cvtask_core_id = (uint16_t)(cvcore_base);
                    interface.cvtask_instance = req->cvtask_instance;
                    interface.cvtask_sysflow_index = req->sysflow_index;
                    interface.cvtask_frameset_id = req->cvtask_frameset_id;
                    interface.cvtask_num_inputs = req->cvtask_num_inputs;
                    interface.cvtask_num_outputs = req->cvtask_num_outputs;
                    interface.cvtask_num_feedback = req->cvtask_num_feedback;
                    interface.cvtask_num_messages = req->cvtask_num_messages;
                    interface.Instance_private_storage_size = req->Instance_private_storage_size;
                    if(req->Instance_private_storage_daddr != 0U) {
                        interface.vpInstance_private_storage = ambacv_c2v(req->Instance_private_storage_daddr);
                    } else {
                        interface.vpInstance_private_storage = NULL;
                    }
                    interface.CVTask_shared_storage_size = 0;
                    interface.vpCVTask_shared_storage = NULL;
                    interface.frameset_basetime = req->frameset_basetime;
                    interface.DRAM_temporary_scratchpad_size = env->scratchpad_size;
                    interface.vpDRAM_temporary_scratchpad = env->scratchpad_base;

                    for (i = 0U; i < interface.cvtask_num_messages; i++) {
                        const cvmem_messagebuf_t *cvmsg;

                        ptr = ambacv_c2v(req->CVTaskMessage_daddr[i]);
                        if(ptr == NULL) {
                            retcode = CVTASK_ERR_NULL_POINTER;
                            console_printU5("[ERROR] cvtask_handle_process_msg message() : (%d) cvmsg == NULL!", i, 0U, 0U, 0U, 0U);
                            break;
                        } else {
                            typecast(&cvmsg,&ptr);
                            if(cvmsg->hdr_variable.message_payload_daddr != 0U) {
                                interface.vpMessagePayload[i] = ambacv_c2v(cvmsg->hdr_variable.message_payload_daddr);
                            } else {
                                interface.vpMessagePayload[i] = NULL;
                            }
                        }
                    }

                    if(retcode == ERRCODE_NONE) {
                        msg->hdr.message_retcode    = node->cvtask_process_messages(&interface);
                        rpl->scheduler_tasknode_id  = req->scheduler_tasknode_id;
                    }
                }
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(*rpl);
    }
    return retcode;
}

static uint32_t cvtask_handle_get_buildinfo(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    const schedmsg_cvtask_getbuildinfo_t *req;
    const cvtask_entry_t *node;
    const char *self_info, *tool_info;
    char un_name[32] = "un-versioned";
    char *info;
    const void* ptr;
    const char *dest_ptr = NULL;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_get_buildinfo() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        req = &msg->msg.cvtask_getbuildinfo;
        // for regular cvtask
        node = ddb.entry[req->arm_cvtask_index];
        self_info = un_name;
        tool_info = un_name;

        /* check if cvtask implements "process_message" */
        if (node->cvtask_get_info == NULL) {
            console_printS5(
                "[INFO] cvtask_handle_get_buildinfo() : get_info not implemented by %s.\n",
                node->cvtask_name,
                NULL, NULL, NULL, NULL);
        } else {
            retcode = node->cvtask_get_info(
                          NULL,
                          CVTASK_INFO_ID_GET_SELF_INFO,
                          &self_info);
            if(retcode == ERRCODE_NONE) {
                retcode = node->cvtask_get_info(
                              NULL,
                              CVTASK_INFO_ID_GET_TOOL_INFO,
                              &tool_info);
                if (retcode != ERRCODE_NONE) {
                    retcode = ERRCODE_NONE; /* Temporarily suppress this error - some legacy ARM CVTasks don't report a tool version*/
                }
            }
        }

        if(retcode == ERRCODE_NONE) {
            ptr = ambacv_c2v(req->daddr_versionstring_location);
            if(ptr == NULL) {
                retcode = CVTASK_ERR_NULL_POINTER;
                console_printU5("[ERROR] cvtask_handle_get_buildinfo() : selfinfo_daddr == NULL!", 0U, 0U, 0U, 0U, 0U);
            } else {
                typecast(&info,&ptr);
                dest_ptr = thread_strncpy(info, self_info, req->versionstring_maxsize);
            }
        }

        if(retcode == ERRCODE_NONE) {
            ptr = ambacv_c2v(req->daddr_toolstring_location);
            if(ptr == NULL) {
                retcode = CVTASK_ERR_NULL_POINTER;
                console_printU5("[ERROR] cvtask_handle_get_buildinfo() : toolinfo_daddr == NULL!", 0U, 0U, 0U, 0U, 0U);
            } else {
                typecast(&info,&ptr);
                dest_ptr = thread_strncpy(info, tool_info, req->versionstring_maxsize);
                msg->hdr.message_retcode  = CVTASK_ERR_OK;
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(schedmsg_cvtask_getbuildinfo_reply_t);
    }
    (void) dest_ptr;
    return retcode;
}

static uint32_t cvtask_handle_finish(armvis_msg_t *msg, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;
    schedmsg_cvtask_run_t *req;
    schedmsg_cvtask_run_reply_t *rpl;
    const cvtask_entry_t *node;
    cvtask_parameter_interface_t interface;
    uint32_t cvtask_uuid;
    cvtask_thpool_env_t *env;

    if((msg == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] cvtask_handle_finish() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            req = &msg->msg.cvtask_run;
            rpl = &msg->msg.reply_cvtask_run;
            node = ddb.entry[req->arm_cvtask_index];

            cvtask_uuid = schdr_sys_state.pSysFlow[req->sysflow_index].uuid;
            SCHDR_PRINTF(".cvtask_finish start, uuid=%d, frame=%d, cpu=%d",
                         cvtask_uuid, req->cvtask_frameset_id, env->cpu_id, 0U, 0U);
            env->run_req = req;

            if(thread_memset(&interface, 0, sizeof(interface)) != ERRCODE_NONE) {
                retcode = CVTASK_ERR_GENERAL;
            } else {
                interface.cvtask_core_id = (uint16_t)(cvcore_base);
                interface.cvtask_instance = req->cvtask_instance;
                interface.cvtask_sysflow_index = req->sysflow_index;
                interface.cvtask_frameset_id = req->cvtask_frameset_id;
                interface.cvtask_num_inputs = req->cvtask_num_inputs;
                interface.cvtask_num_outputs = req->cvtask_num_outputs;
                interface.cvtask_num_feedback = req->cvtask_num_feedback;
                interface.cvtask_num_messages = req->cvtask_num_messages;
                interface.Instance_private_storage_size = req->Instance_private_storage_size;
                if (req->Instance_private_storage_daddr != 0U) {
                    interface.vpInstance_private_storage = ambacv_c2v(req->Instance_private_storage_daddr);
                } else {
                    interface.vpInstance_private_storage = NULL;
                }
                interface.CVTask_shared_storage_size = 0;
                interface.vpCVTask_shared_storage = NULL;
                interface.frameset_basetime = req->frameset_basetime;
                interface.DRAM_temporary_scratchpad_size = env->scratchpad_size;
                interface.vpDRAM_temporary_scratchpad = env->scratchpad_base;

                /* No input, feedback, message, or output buffers are being used */

                retcode = cvtask_prof_new_action((uint8_t)(cvcore_base), req->sysflow_index,
                                                 req->cvtask_frameset_id);
                if(retcode == ERRCODE_NONE) {
                    retcode = cvtask_prof_unit_on((uint8_t)(cvcore_base), req->sysflow_index);
                    msg->hdr.message_retcode  = node->cvtask_finish(&interface);
                }
                if(retcode == ERRCODE_NONE) {
                    retcode = cvtask_prof_unit_off((uint8_t)(cvcore_base), req->sysflow_index);
                }

                SCHDR_PRINTF(".cvtask_finish done,  uuid=%d, frame=%d, cpu=%d",
                             cvtask_uuid, req->cvtask_frameset_id, env->cpu_id, 0U, 0U);
                rpl->scheduler_tasknode_id = req->scheduler_tasknode_id;
            }
        }
    }

    if(retcode == ERRCODE_NONE) {
        *ret_size = (uint32_t)sizeof(*rpl);
    }
    return retcode;
}

static uint32_t flexidag_execute_request(armvis_msg_t *msg,cvtask_thpool_env_t *env, uint32_t slot_id, uint32_t *ret_size)
{
    uint32_t retcode = ERRCODE_NONE;

    if((msg == NULL) || (env == NULL) || (ret_size == NULL)) {
        console_printU5("[ERROR] flexidag_execute_request() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
    } else if (slot_id == FLEXIDAG_INVALID_SLOT_ID) {
        console_printU5("[ERROR] : flexidag_execute_request() : Invalid message_id (0x%04x) for flexidag command 0x%x", msg->hdr.message_id, msg->hdr.message_type, 0U, 0U, 0U);
    } else {

        switch (msg->hdr.message_type) {
        case SCHEDMSG_FLEXIDAG_CVREG_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_reg(msg, ret_size);
            break;

        case SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_query(msg, ret_size);
            break;

        case SCHEDMSG_FLEXIDAG_CVINIT_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_init(msg, ret_size);
            break;

        case SCHEDMSG_FLEXIDAG_CVRUN_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_run(msg, ret_size);
            break;

        case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_process_msg(msg, ret_size);
            break;

        case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_finish(msg, ret_size);
            break;

        case SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST:
            env->flexidag_slot_id = slot_id;
            retcode = cvtask_handle_get_buildinfo(msg, ret_size);
            break;

        // Cavalry related =--------------------------------------------------------
        case 0x460:  /* TODO: temporarily suppress this message until the proper */
        case 0x461:  /* messages are imported and synced with the AMALGAM implementation */
        case 0x468:
        case 0x469:
            // these messages have nothing to invalidate:
            break;

        default:
            console_printU5("[ERROR] flexidag_execute_request() : unknown armvis_msg_t type %d",
                            msg->hdr.message_type, 0U, 0U, 0U, 0U);
            break;
        }
    }

    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] flexidag_execute_request() :  fail type 0x%x ", msg->hdr.message_type, 0U, 0U, 0U, 0U);
    }

    return 0U;
}

static void cvtask_execute_request_cvtask(armvis_msg_t *msg, uint32_t *psize, uint32_t *hit)
{
    uint32_t retcode = ERRCODE_NONE;

    *hit = 1U;
    switch (msg->hdr.message_type) {
    case SCHEDMSG_CVTASK_REGISTER:
        retcode = cvtask_handle_reg(msg, psize);
        break;
    case SCHEDMSG_CVTASK_QUERY_REQUEST:
        retcode = cvtask_handle_query(msg, psize);
        break;
    case SCHEDMSG_CVTASK_INIT_REQUEST:
        retcode = cvtask_handle_init(msg, psize);
        break;
    case SCHEDMSG_CVTASK_RUN_REQUEST:
        retcode = cvtask_handle_run(msg, psize);
        break;
    case SCHEDMSG_CVTASK_MSGONLY_REQUEST:
        retcode = cvtask_handle_process_msg(msg, psize);
        break;
    case SCHEDMSG_CVTASK_FINISH_REQUEST:
        retcode = cvtask_handle_finish(msg, psize);
        break;
    case SCHEDMSG_CVTASK_GET_BUILDINFO:
        retcode = cvtask_handle_get_buildinfo(msg, psize);
        break;
    default:
        *hit = 0U;
        break;
    }

    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] cvtask_execute_request_cvtask() :  fail type 0x%x ", msg->hdr.message_type, 0U, 0U, 0U, 0U);
    }
}

static void cvtask_execute_request_flexidag(armvis_msg_t *msg, cvtask_thpool_env_t *env,uint32_t slot_id, uint32_t *psize, uint32_t *hit)
{
    *hit = 1U;
    switch (msg->hdr.message_type) {
    case SCHEDMSG_FLEXIDAG_CVREG_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVINIT_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST:
        if (flexidag_execute_request(msg, env, slot_id, psize) != ERRCODE_NONE) {
            console_printU5("[ERROR] cvtask_execute_request_flexidag() : flexidag_execute_request fail type 0x%x ", msg->hdr.message_type, 0U, 0U, 0U, 0U);
        }
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void cvtask_execute_request_cavalry(const armvis_msg_t *msg, const uint32_t *psize, uint32_t *hit)
{
    *hit = 1U;
    (void) psize;
    (void) msg;
    switch (msg->hdr.message_type) {
    // Cavalry related =--------------------------------------------------------
    case 0x460:  /* TODO: temporarily suppress this message until the proper */
    case 0x461:  /* messages are imported and synced with the AMALGAM implementation */
    case 0x468:
    case 0x469:
        // these messages have nothing to invalidate:
        break;
    default:
        *hit = 0U;
        break;
    }
}

uint32_t cvtask_execute_request(void *arg)
{
    armvis_msg_t *msg = NULL;
    uint32_t rpl_size = 0U;
    cvtask_thpool_env_t *env;
    uint32_t  flexidag_cvfn_slot_id, hit;
    uint32_t retcode = ERRCODE_NONE;

    (void) arg;
    if(arg == NULL) {
        console_printU5("[ERROR] cvtask_execute_request() : arg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        typecast(&msg,&arg);
        env = schdr_get_thenv();
        if(env == NULL) {
            console_printU5("[ERROR] cvtask_execute_request() : env == NULL", 0U, 0U, 0U, 0U, 0U);
        } else {
            env->flexidag_slot_id = FLEXIDAG_INVALID_SLOT_ID;
            if ((msg->hdr.message_id >= (uint16_t)0x0100U) && (msg->hdr.message_id < (uint16_t)(0x0100U + FLEXIDAG_MAX_SLOTS))) {
                flexidag_cvfn_slot_id = (uint32_t)msg->hdr.message_id - 0x0100U;
            } /* if ((msg->hdr.message_id >= 0x0100) && (msg->hdr.message_id < (0x0100 + FLEXIDAG_MAX_SLOTS))) */
            else { /* if ((msg->hdr.message_id < 0x0100) || (msg->hdr.message_id >= (0x0100 + FLEXIDAG_MAX_SLOTS))) */
                flexidag_cvfn_slot_id = FLEXIDAG_INVALID_SLOT_ID;
            } /* if ((msg->hdr.message_id < 0x0100) || (msg->hdr.message_id >= (0x0100 + FLEXIDAG_MAX_SLOTS))) */

            cvtask_execute_request_cvtask(msg, &rpl_size, &hit);
            if(hit == 0U) {
                cvtask_execute_request_flexidag(msg, env, flexidag_cvfn_slot_id, &rpl_size, &hit);
                if(hit == 0U) {
                    cvtask_execute_request_cavalry(msg, &rpl_size, &hit);
                    if(hit == 0U) {
                        console_printU5("[ERROR] cvtask_execute_request() : unknown armvis_msg_t type %d",
                                        msg->hdr.message_type, 0U, 0U, 0U, 0U);
                    }
                }
            }
            if(rpl_size > 0U) {
                if (schdrmsg_send_msg(msg, rpl_size) != ERRCODE_NONE) {
                    console_printU5("[ERROR] cvtask_execute_request() : schdrmsg_send_msg fail type 0x%x ", msg->hdr.message_type, 0U, 0U, 0U, 0U);
                }
            }
            env->flexidag_slot_id = FLEXIDAG_INVALID_SLOT_ID;
        }
    }
    return retcode;
}

uint32_t cvtask_get_scratchpad_size(void)
{
    return schdr_sys_cfg.max_sp_size;
}

/**
 * register a cvtask with @name and @ops
 * This is called within the scheduler task, so no mutex is needed.
 */
uint32_t cvtask_register(const cvtask_entry_t *entry, uint32_t version)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index = ddb.total_class;
    uint32_t str_pos;
    char str_line[512];

    if(entry == NULL) {
        console_printU5("[ERROR] cvtask_register() : entry == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
    } else {
        if (index >= MAX_CLASS_NUM) {
            console_printS5(
                "[ERROR] cvtask_register() : Too much cvtask registered\n",
                NULL, NULL, NULL, NULL, NULL);
            retcode = ERR_DRV_SCHDR_ARM_CVTASK_OUT_OF_RANGE;
        } else {
            if (version != CVTASK_API_VERSION) {
                str_pos = snprintf_str5(&str_line[0], (uint32_t)sizeof(str_line), "[ERROR] cvtask_register() : [%s] version mismatch: ",
                                        entry->cvtask_name,
                                        NULL, NULL, NULL, NULL);
                str_pos += snprintf_uint5(&str_line[str_pos], (uint32_t)sizeof(str_line), "\texpect 0x%X, got 0x%X",
                                          CVTASK_API_VERSION, version,
                                          0U, 0U, 0U);
                (void) str_pos;
                console_printU5(&str_line[0], 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_CVTASK_VERSION_MISMATCHED;
            } else {
                module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                               "register cvtask [%s]\n",
                               entry->cvtask_name,
                               NULL, NULL, NULL, NULL);

                ddb.entry[index] = entry;
                ddb.total_class++;
            }
        }
    }
    return retcode;
}

uint32_t cvtask_init_modules(void)
{
    uint32_t retcode = ERRCODE_NONE;
    extern initcall_t __cvtask_init_start[128];
    extern initcall_t __cvtask_init_end;
    initcall_t fn;
    uint32_t loop = 1U,counter = 0U;
    static uint32_t cvtask_ops_init = 0U;

    /* return if cvtask module is init'ed */
    if(cvtask_ops_init == 0U) {
        retcode = thread_memset(&ddb, 0, sizeof(ddb));
        if(retcode == ERRCODE_NONE) {
            retcode = thread_memset(&ldb, 0, sizeof(ldb));
        }

        if(retcode == ERRCODE_NONE) {
            /* init each cvtask */
            while(loop == 1U) {
                fn = __cvtask_init_start[counter];
                if(counter < 128U) {
                    if((&__cvtask_init_start[counter] !=  &__cvtask_init_end)) {
                        if((fn)() != ERRCODE_NONE) {
                            loop = 0U;
                            console_printU5("[ERROR] cvtask_init_modules() : fail retcode (0x%x) ", retcode, 0U, 0U, 0U, 0U);
                            retcode = ERR_DRV_SCHDR_CVTASK_INIT_MOD_FAIL;
                        } else {
                            counter++;
                        }
                    } else {
                        loop = 0U;
                    }
                } else {
                    loop = 0U;
                    console_printU5("[ERROR] cvtask_init_modules() : counter > 128 ", counter, 0U, 0U, 0U, 0U);
                }
            }
            if(retcode == ERRCODE_NONE) {
                cvtask_ops_init = 1U;
            }
        }
    }

    return retcode;
}

uint32_t cvtask_get_total_class(void)
{
    return ddb.total_class;
}

uint32_t cvtask_log_flush(uint64_t state_addr)
{
    uint32_t retcode = ERRCODE_NONE;
    cvtask_parameter_interface_t interface;
    uint32_t i;

    retcode = thread_memset(&interface, 0, sizeof(interface));
    if(retcode == ERRCODE_NONE) {
        interface.cvtask_num_inputs = 1U;
        interface.cvtask_frameset_id = 0xFFFFU;
        if(state_addr != 0U) {
            interface.vpInputBuffer[0] = ambacv_c2v(state_addr);
        } else {
            interface.vpInputBuffer[0] = NULL;
        }
        for (i = 0U; i < ldb.total_instance; i++) {
            interface.vpInstance_private_storage = ldb.private_storage[i];
            if(ldb.entry[i]->cvtask_run(&interface) != ERRCODE_NONE) {
                break;
            }
        }
    }

    return retcode;
}

uint32_t cvtask_set_cluster_id(uint32_t cid)
{
    uint32_t retcode = ERRCODE_NONE;

    if(cid <= CVCORE_CA7) {
        cvcore_base = cid;
    } else {
        console_printU5(
            "[ERROR] cvtask_set_cluster_id() : Invalid cluster ID %d\n",
            cid,
            0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_CLUSTER_UNKNOW;
    }

    return retcode;
}
#endif
