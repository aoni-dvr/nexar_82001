/**
 *  @file cvtask_api.c
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
 *  @details Cvtask APIs
 *
 */

#include "os_api.h"
#include "dram_mmap.h"
#include "schdrmsg_def.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "cvtask_errno.h"

#ifndef DISABLE_ARM_CVTASK
static cvlog_cvtask_entry_t* get_next_cvtask_entry(schdr_log_info_t *info)
{
    cvlog_cvtask_entry_t *entry = NULL;
    const void *ptr = NULL;

    if(info == NULL) {
        console_printU5("[ERROR] get_next_cvtask_entry() : info == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = get_next_entry(info);
        if(ptr != NULL) {
            typecast(&entry,&ptr);
        }
    }
    return entry;
}

static cvlog_perf_flexidag_entry_t* get_next_perf_flexidag_entry(schdr_log_info_t *info)
{
    cvlog_perf_flexidag_entry_t *entry = NULL;
    const void *ptr = NULL;

    if(info == NULL) {
        console_printU5("[ERROR] get_next_perf_flexidag_entry() : info == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = get_next_entry(info);
        if(ptr != NULL) {
            typecast(&entry,&ptr);
        }
    }
    return entry;
}

static cvmem_messagebuf_t *find_msg_entry(cvmem_messagepool_t *pool)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t entry = 0UL;
    cvmem_messagebuf_t *pentry = NULL;
    const void *ptr = NULL;

    if(pool == NULL) {
        console_printU5("[ERROR] find_msg_entry() : pool == NULL \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = drv_schdr_find_msg(ambacv_v2c(pool), &entry);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] find_msg_entry() : drv_schdr_find_msg fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
        } else {
            ptr =ambacv_c2v(entry);
        }
        if(ptr == NULL) {
            console_printU5("[ERROR] find_msg_entry() : ptr == NULL \n", 0U, 0U, 0U, 0U, 0U);
        } else {
            typecast(&pentry,&ptr);
        }
    }
    return pentry;
}

static uint32_t send_msg_entry(cvmem_messagebuf_t *entry, uint32_t dst,
                               uint32_t frame, uint32_t len, uint32_t flexidag_slot_id)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvtask_thpool_env_t *env;
    armvis_msg_t schdr_msg;

    if(entry == NULL) {
        console_printU5("[ERROR] send_msg_entry() : entry == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if (env != NULL) {
            if (env->run_req == NULL) {
                entry->hdr_variable.src_sysflow_index = 0xFFFFU;
            } else {
                entry->hdr_variable.src_sysflow_index = env->run_req->sysflow_index;
            }
        } else {
            entry->hdr_variable.src_sysflow_index = 0xFFFFU;
        }
        entry->hdr_variable.dst_sysflow_index = (uint16_t)dst;
        entry->hdr_variable.message_size      = len;
        entry->hdr_variable.message_send_time = cvtask_get_timestamp();
        if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            schdr_msg.hdr.message_type  = (uint16_t) SCHEDMSG_CVTASK_MSG_REPLY;
            schdr_msg.hdr.message_id    = (uint16_t) 0U;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            schdr_msg.hdr.message_type  = (uint16_t) SCHEDMSG_FLEXIDAG_CVTASK_MSG;
            schdr_msg.hdr.message_id    = (uint16_t) (0x0100U + flexidag_slot_id);
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        schdr_msg.hdr.message_retcode                   = ERRCODE_NONE;
        schdr_msg.msg.reply_cvtask_msg.CVTaskMessage_daddr  = (uint32_t)ambacv_v2c(entry);
        schdr_msg.msg.reply_cvtask_msg.target_frameset_id   = frame;
        retcode = schdrmsg_send_msg(&schdr_msg, (uint32_t)(sizeof(schdr_msg.hdr) + sizeof(schdr_msg.msg.reply_cvtask_msg)));
        if(retcode != ERRCODE_NONE) {
            retcode = CVTASK_ERR_GENERAL;
        }
    }
    return retcode;
}


static uint32_t send_msg_entry_at_time(cvmem_messagebuf_t *entry, uint32_t dst,
                                       uint32_t at_time, uint32_t len, uint32_t flexidag_slot_id)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvtask_thpool_env_t *env;
    armvis_msg_t schdr_msg;

    if(entry == NULL) {
        console_printU5("[ERROR] send_msg_entry_at_time() : entry == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if (env != NULL) {
            if (env->run_req == NULL) {
                entry->hdr_variable.src_sysflow_index = 0xFFFFU;
            } else {
                entry->hdr_variable.src_sysflow_index = env->run_req->sysflow_index;
            }
        } else {
            entry->hdr_variable.src_sysflow_index = 0xFFFFU;
        }
        entry->hdr_variable.dst_sysflow_index = (uint16_t)dst;
        entry->hdr_variable.message_size      = len;
        entry->hdr_variable.message_send_time = cvtask_get_timestamp();

        if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            schdr_msg.hdr.message_type  = (uint16_t)SCHEDMSG_CVTASK_MSG_AT_TIME_REPLY;
            schdr_msg.hdr.message_id    = (uint16_t)0U;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            schdr_msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_CVTASK_MSG_AT_TIME;
            schdr_msg.hdr.message_id    = (uint16_t)(0x0100U + flexidag_slot_id);
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        schdr_msg.hdr.message_retcode = ERRCODE_NONE;
        schdr_msg.msg.reply_cvtask_msg_at_time.CVTaskMessage_daddr  = (uint32_t)ambacv_v2c(entry);
        schdr_msg.msg.reply_cvtask_msg_at_time.target_frametime     = at_time;
        retcode = schdrmsg_send_msg(&schdr_msg, (uint32_t)(sizeof(schdr_msg.hdr) + sizeof(schdr_msg.msg.reply_cvtask_msg_at_time)));
        if(retcode != ERRCODE_NONE) {
            retcode = CVTASK_ERR_GENERAL;
        }
    }
    return retcode;
}

uint32_t cvtask_get_sysflow_index_list(
    IN  const char *instance,
    IN  const char *algorithm,
    IN  const char *step,
    IN  const char *cvtask,
    OUT uint32_t *index_array,
    IN  uint32_t  array_size,
    OUT uint32_t *num_found)
{
    uint32_t index;
    uint32_t i;
    const sysflow_entry_t *pSysFlow;
    uint32_t sysflow_numlines;
    const cvtask_thpool_env_t *env;
    uint32_t  retcode = ERRCODE_NONE;

    if ((index_array == NULL) && (array_size != 0U)) {
        console_printU5("[ERROR] cvtask_get_sysflow_index_list() : invalid arg", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                pSysFlow          = schdr_sys_state.pSysFlow;
                sysflow_numlines  = schdr_sys_state.sysflow_numlines;
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                pSysFlow          = flexidag_sys_state[env->flexidag_slot_id].pSysFlow;
                sysflow_numlines  = flexidag_sys_state[env->flexidag_slot_id].sysflow_numlines;
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

            index = 0U;
            for (i = 0U; i < sysflow_numlines; i++) {
                const sysflow_entry_t *curr = &pSysFlow[i];

                // check instance name
                if (instance != NULL) {
                    if (thread_strcmp(instance, curr->instance_name)!= 0) {
                        continue;
                    }
                }

                // check algorithm name
                if (algorithm != NULL) {
                    if (thread_strcmp(algorithm, curr->algorithm_name)!= 0) {
                        continue;
                    }
                }

                // check step name
                if (step != NULL) {
                    if (thread_strcmp(step, curr->step_name)!= 0) {
                        continue;
                    }
                }

                // chech cvtask name
                if (cvtask != NULL) {
                    if (thread_strcmp(cvtask, curr->cvtask_name)!= 0) {
                        continue;
                    }
                }

                // we found one match
                if (index < array_size) {
                    index_array[index] = i;
                }

                ++index;
            }
            if (num_found != NULL) {
                *num_found = index;
            } /* if (num_found != NULL) */
        }
    }
    return retcode;
}

uint32_t cvtask_get_sysflow_index_by_UUID(
    IN   uint32_t flow_uuid,
    OUT  uint32_t *pIndex)
{
    uint32_t  retcode = ERRCODE_NONE;

    if (pIndex == NULL) {
        console_printU5("[ERROR] cvtask_get_sysflow_index_by_UUID() : pIndex == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } /* if (pIndex == NULL) */
    else { /* if (pIndex != NULL) */
        uint32_t  scan_loop;
        uint32_t  found_index;
        const sysflow_entry_t *pSysFlow;
        uint32_t sysflow_numlines;
        const cvtask_thpool_env_t *env;

        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                pSysFlow          = schdr_sys_state.pSysFlow;
                sysflow_numlines  = schdr_sys_state.sysflow_numlines;
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                pSysFlow          = flexidag_sys_state[env->flexidag_slot_id].pSysFlow;
                sysflow_numlines  = flexidag_sys_state[env->flexidag_slot_id].sysflow_numlines;
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

            found_index = (uint32_t)SYSFLOW_INVALID_INDEX;
            scan_loop = 0U;

            while ((found_index == ((uint32_t)SYSFLOW_INVALID_INDEX)) && (scan_loop < (uint32_t)sysflow_numlines)) {
                const sysflow_entry_t *curr = &pSysFlow[scan_loop];
                if (curr->uuid == flow_uuid) {
                    found_index = scan_loop;
                } /* if (curr->uuid == uuid) */
                scan_loop++;
            } /* while ((found_index == ((uint32_t)SYSFLOW_INVALID_INDEX)) && (scan_loop < (uint32_t)sysflow_numlines)) */

            if (found_index == ((uint32_t)SYSFLOW_INVALID_INDEX)) {
                retcode = CVTASK_ERR_NOT_FOUND;
            }
            *pIndex = found_index;
        }
    } /* if (pIndex != NULL) */

    return retcode;

} /* cvtask_get_sysflow_index_by_UUID() */

uint32_t cvtask_get_name(
    IN uint32_t     sysflow_index,
    OUT const char  **instance_name,
    OUT const char  **algorithm_name,
    OUT const char  **step_name,
    OUT const char  **cvtask_name,
    OUT uint32_t    *flow_uuid)
{
    const sysflow_entry_t *pSysFlow;
    const sysflow_entry_t *entry;
    const cvtask_thpool_env_t *env;
    uint32_t sysflow_numlines;
    uint32_t retcode = ERRCODE_NONE;
    uint32_t index = sysflow_index;

    env = schdr_get_thenv();
    if(env == NULL) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            pSysFlow          = schdr_sys_state.pSysFlow;
            sysflow_numlines  = schdr_sys_state.sysflow_numlines;
        } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            pSysFlow          = flexidag_sys_state[env->flexidag_slot_id].pSysFlow;
            sysflow_numlines  = flexidag_sys_state[env->flexidag_slot_id].sysflow_numlines;
        } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

        if (index == (uint32_t)SYSFLOW_INVALID_INDEX) { /* Specialized case */
            index = (uint32_t)env->run_req->sysflow_index;
        } /* if (sysflow_index == SYSFLOW_INVALID_INDEX) */

        if (index >= (uint32_t) sysflow_numlines) {
            retcode = CVTASK_ERR_INDEX_TOO_LARGE;
        } else {

            entry = &pSysFlow[index];
            if (instance_name != NULL) {
                *instance_name = entry->instance_name;
            }
            if (algorithm_name != NULL) {
                *algorithm_name = entry->algorithm_name;
            }
            if (step_name != NULL) {
                *step_name = entry->step_name;
            }
            if (cvtask_name != NULL) {
                *cvtask_name = entry->cvtask_name;
            }
            if (flow_uuid != NULL) {
                *flow_uuid = entry->uuid;
            }
        }
    }

    return retcode;
}

uint32_t cvtask_message_create(uint32_t pool_index, void **payload_buf, uint32_t *payload_max_size)
{
    const cvmem_messagebuf_t   *entry;
    cvmem_messagepool_t  *tskpool = NULL;
    const cvtask_thpool_env_t *env;
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    if((payload_buf == NULL) || (payload_max_size == NULL)) {
        console_printU5("[ERROR] cvtask_message_create() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if (env == NULL) {
            console_printU5("[ERROR] cvtask_message_create() : Called out of CVTask context; env is NULL\n", 0U, 0U, 0U, 0U, 0U);
            retcode = CVTASK_ERR_NULL_POINTER;
        } /* if (env == NULL) */ else { /* if (env != NULL) */
            if (pool_index == DEFAULT_MESSAGE_POOL_INDEX) {
                if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                    tskpool = schdr_sys_state.pMsgpool;
                } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */ else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                    tskpool = flexidag_sys_state[env->flexidag_slot_id].pMsgpool;
                } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            } else { /* if (idx != DEFAULT_MESSAGE_POOL_INDEX) */
                if (pool_index >= env->run_req->cvtask_num_custom_message_pool) {
                    console_printU5("[ERROR] cvtask_message_create() : mesg pool index(%d) too big", pool_index, 0U, 0U, 0U, 0U);
                    retcode = CVTASK_ERR_INDEX_TOO_LARGE;
                }

                if(retcode == ERRCODE_NONE) {
                    ptr = ambacv_c2v(env->run_req->CVTaskCustomMsgPool_daddr[pool_index]);
                    if (ptr == NULL) {
                        console_printU5("[ERROR] cvtask_message_create() : tskpool is incorrect for FD[%3d] / pool_id=%d\n", env->flexidag_slot_id, pool_index, 0U, 0U, 0U);
                        retcode = CVTASK_ERR_INDEX_TOO_LARGE; /* TODO: Find better error */
                    } /* if (tskpool == NULL) */else {
                        typecast(&tskpool,&ptr);
                    }
                }
            } /* if (idx != DEFAULT_MESSAGE_POOL_INDEX) */

            if(retcode == ERRCODE_NONE) {
                entry = find_msg_entry(tskpool);
                if (entry == NULL) {
                    console_printU5("[ERROR] cvtask_message_create() : msgpool is full", 0U, 0U, 0U, 0U, 0U);
                    retcode = CVTASK_ERR_NOT_FOUND;
                } /* if (entry == NULL) */ else { /* if (entry != NULL) */
                    *payload_buf = ambacv_c2v(entry->hdr_variable.message_payload_daddr);
                    *payload_max_size = tskpool->hdr_variable.message_size;
                } /* if (entry != NULL) */
            }
        } /* if (env != NULL) */
    }
    return retcode;

} /* cvtask_message_create() */

uint32_t cvtask_message_send(const void *payload_buf, uint32_t payload_len,
                             uint32_t dst_sysflow_index, uint32_t target_frameset_id)
{
    uint32_t retcode = ERRCODE_NONE;
    cvmem_messagebuf_t *entry = NULL;
    const cvtask_thpool_env_t *env;

    if(payload_buf == NULL) {
        console_printU5("[ERROR] cvtask_message_send() : payload_buf == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        typecast(&entry,&payload_buf);
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            --entry;
            retcode = send_msg_entry(entry, dst_sysflow_index, target_frameset_id, payload_len, env->flexidag_slot_id);
        }
    }
    return retcode;
}

uint32_t cvtask_message_send_at_time(const void *payload_buf, uint32_t payload_len,
                                     uint32_t dst_sysflow_index, uint32_t target_time)
{
    uint32_t retcode = ERRCODE_NONE;
    cvmem_messagebuf_t *entry = NULL;
    const cvtask_thpool_env_t *env;

    if(payload_buf == NULL) {
        console_printU5("[ERROR] cvtask_message_send_at_time() : payload_buf == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        typecast(&entry,&payload_buf);
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            --entry;
            retcode = send_msg_entry_at_time(entry, dst_sysflow_index, target_time, payload_len, env->flexidag_slot_id);
        }
    }
    return retcode;
}

uint32_t cvtask_default_message_send(const void *payload_buf, uint32_t payload_len,
                                     uint32_t dst_sysflow_index, uint32_t target_frameset_id)
{
    cvmem_messagebuf_t *entry = NULL;
    const cvtask_thpool_env_t *env;
    uint32_t retcode = ERRCODE_NONE;

    if(payload_buf == NULL) {
        console_printU5("[ERROR] cvtask_default_message_send() : payload_buf == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                if(payload_len > schdr_sys_state.pMsgpool->hdr_variable.message_size) {
                    retcode = CVTASK_ERR_INDEX_TOO_LARGE;
                    console_printU5("[ERROR] cvtask_default_message_send() : len (%d) > msgpool->msg_size (%d)", payload_len, schdr_sys_state.pMsgpool->hdr_variable.message_size, 0U, 0U, 0U);
                } else {
                    entry = find_msg_entry(schdr_sys_state.pMsgpool);
                }
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                if(payload_len > flexidag_sys_state[env->flexidag_slot_id].pMsgpool->hdr_variable.message_size) {
                    retcode = CVTASK_ERR_INDEX_TOO_LARGE;
                    console_printU5("[ERROR] cvtask_default_message_send() : len (%d) > pFlexidag_msgpool[%d]->msg_size (%d)", payload_len, env->flexidag_slot_id, flexidag_sys_state[env->flexidag_slot_id].pMsgpool->hdr_variable.message_size, 0U, 0U);
                } else {
                    entry = find_msg_entry(flexidag_sys_state[env->flexidag_slot_id].pMsgpool);
                }
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

            if(retcode == ERRCODE_NONE) {
                if (entry == NULL) {
                    retcode = CVTASK_ERR_NOT_FOUND;
                } else {
                    if(thread_memcpy(ambacv_c2v(entry->hdr_variable.message_payload_daddr), payload_buf, payload_len) != ERRCODE_NONE) {
                        retcode = CVTASK_ERR_GENERAL;
                    } else {
                        retcode = send_msg_entry(entry, dst_sysflow_index, target_frameset_id, payload_len, env->flexidag_slot_id);
                    }
                }
            }
        }
    }

    return retcode;
}

uint32_t cvtask_default_message_send_at_time(const void *payload_buf, uint32_t payload_len,
        uint32_t dst_sysflow_index, uint32_t target_time)
{
    cvmem_messagebuf_t *entry = NULL;
    const cvtask_thpool_env_t *env;
    uint32_t retcode = ERRCODE_NONE;

    env = schdr_get_thenv();
    if(env == NULL) {
        retcode = CVTASK_ERR_NULL_POINTER;
    }

    if(retcode == ERRCODE_NONE) {
        if(payload_buf == NULL) {
            console_printU5("[ERROR] cvtask_default_message_send_at_time() : payload_buf == NULL ", 0U, 0U, 0U, 0U, 0U);
            retcode = CVTASK_ERR_NULL_POINTER;
        } else if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            if(payload_len > schdr_sys_state.pMsgpool->hdr_variable.message_size) {
                retcode = CVTASK_ERR_INDEX_TOO_LARGE;
                console_printU5("[ERROR] cvtask_default_message_send_at_time() : len (%d) > msgpool->msg_size (%d)", payload_len, schdr_sys_state.pMsgpool->hdr_variable.message_size, 0U, 0U, 0U);
            } else {
                entry = find_msg_entry(schdr_sys_state.pMsgpool);
            }
        } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            if(payload_len > flexidag_sys_state[env->flexidag_slot_id].pMsgpool->hdr_variable.message_size) {
                retcode = CVTASK_ERR_INDEX_TOO_LARGE;
                console_printU5("[ERROR] cvtask_default_message_send_at_time() : len (%d) > pFlexidag_msgpool[%d]->msg_size (%d)", payload_len, env->flexidag_slot_id, flexidag_sys_state[env->flexidag_slot_id].pMsgpool->hdr_variable.message_size, 0U, 0U);
            } else {
                entry = find_msg_entry(flexidag_sys_state[env->flexidag_slot_id].pMsgpool);
            }
        } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

        if(retcode == ERRCODE_NONE) {
            if (entry == NULL) {
                retcode = CVTASK_ERR_NOT_FOUND;
            } else {
                if(thread_memcpy(ambacv_c2v(entry->hdr_variable.message_payload_daddr), payload_buf, payload_len) != ERRCODE_NONE) {
                    retcode = CVTASK_ERR_GENERAL;
                } else {
                    retcode = send_msg_entry_at_time(entry, dst_sysflow_index, target_time, payload_len, env->flexidag_slot_id);
                }
            }
        }
    }
    return retcode;
}

uint32_t cvtask_message_bulk_send(
    uint32_t num_messages,
    void*    *payload_buf,
    uint32_t *payload_len,
    uint16_t *payload_dst,
    uint32_t target_frameset_id)
{
    armvis_msg_t schdr_msg;
    schedmsg_attach_bulk_cvtask_message_t *rpl;
    cvmem_messagebuf_t *entry;
    uint32_t timestamp = cvtask_get_timestamp();
    uint32_t index;
    const cvtask_thpool_env_t *env;
    uint32_t  retcode = ERRCODE_NONE;

    (void) payload_dst;
    (void) payload_buf;
    (void) payload_len;
    if((payload_buf == NULL) || (payload_len == NULL) || (payload_dst == NULL)) {
        console_printU5("[ERROR] cvtask_message_bulk_send() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        thread_unused(payload_dst);
        thread_unused(payload_len);
        thread_unused(payload_buf);
        /* check if num_messages is in the correct range */
        if (num_messages >= BULK_MESSAGE_MAX_NUM) {
            console_printU5("[ERROR] cvtask_message_bulk_send() : too many messages in bulk_send", 0U, 0U, 0U, 0U, 0U);
            retcode = CVTASK_ERR_NULL_POINTER;
        }

        if(retcode == ERRCODE_NONE) {
            rpl = &schdr_msg.msg.reply_cvtask_bulk_msg;
            for (index = 0U; index < num_messages; index++) {
                /* check if each payload_buf is created correctly */
                char *vir;
                const void *ptr = payload_buf[index];

                typecast(&vir,&ptr);
                if(vir == NULL) {
                    console_printU5("[ERROR] cvtask_message_bulk_send() : wrong payload buffer", 0U, 0U, 0U, 0U, 0U);
                    retcode = CVTASK_ERR_NULL_POINTER;
                } else {
                    uint64_t phy = ambacv_v2c(vir);

                    rpl->CVTaskMessage_daddr[index]  = (uint32_t)phy;
                    rpl->message_size[index]         = payload_len[index];
                    rpl->target_sysflow_index[index] = payload_dst[index];

                    /* set up header info for each entry */
                    ptr = ambacv_c2v(phy - sizeof(cvmem_messagebuf_t));
                    if(ptr == NULL) {
                        retcode = CVTASK_ERR_NULL_POINTER;
                    } else {
                        typecast(&entry,&ptr);
                        entry->hdr_variable.dst_sysflow_index = payload_dst[index];
                        entry->hdr_variable.message_size      = payload_len[index];
                        entry->hdr_variable.message_send_time = timestamp;
                    }

                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }

            if(retcode == ERRCODE_NONE) {
                env = schdr_get_thenv();
                if(env == NULL) {
                    retcode = CVTASK_ERR_NULL_POINTER;
                } else {
                    if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                        schdr_msg.hdr.message_type  = (uint16_t) SCHEDMSG_SEND_BULK_CVTASK_MSG_TO_FRAME;
                        schdr_msg.hdr.message_id    = (uint16_t) 0U;
                    } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
                    else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                        schdr_msg.hdr.message_type  = (uint16_t) SCHEDMSG_FLEXIDAG_SEND_BULK_CVTASK_MSG_TO_FRAME;
                        schdr_msg.hdr.message_id    = (uint16_t) (0x0100U + env->flexidag_slot_id);
                    } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                    schdr_msg.hdr.message_retcode = ERRCODE_NONE;

                    rpl->num_cvtask_messages = num_messages;
                    rpl->target_frameset_id = target_frameset_id;
                    retcode = schdrmsg_send_msg(&schdr_msg, (uint32_t)sizeof(*rpl));
                    if(retcode != ERRCODE_NONE) {
                        retcode = CVTASK_ERR_GENERAL;
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t cvtask_get_buildinfo(uint32_t sysflow_index,
                              const char **source_info, const char **tool_info)
{
    const component_build_info_t *entry;
    uint32_t retcode = ERRCODE_NONE;

    if((source_info == NULL) || (tool_info == NULL)) {
        console_printU5("[ERROR] cvtask_get_buildinfo() : invalid arg", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else if (sysflow_index >= (uint32_t)schdr_sys_state.sysflow_numlines) {
        retcode = CVTASK_ERR_INDEX_TOO_LARGE;
    } else {
        entry = &schdr_sys_state.pBuildinfo[sysflow_index];
        *source_info = entry->component_version;
        *tool_info = entry->tool_version;
    }
    return retcode;
}

uint32_t cvtable_find(
    IN  const char *name,
    OUT const void **ptr,
    OUT uint32_t *size)
{
    uint32_t i;
    const cvtable_entry_t  *pCVTable;
    const cvtable_entry_t  *info;
    uint32_t cvtable_numentries;
    const char *info_name;
    uint32_t retcode = CVTASK_ERR_NOT_FOUND;
    const cvtask_thpool_env_t *env;
    const void *tmp_ptr;

    if((name == NULL) || (ptr == NULL)) {
        console_printU5("[ERROR] cvtable_find() : invalid arg", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                pCVTable            = schdr_sys_state.pCvtable;
                cvtable_numentries  = schdr_sys_state.cvtable_numlines;
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                pCVTable            = flexidag_sys_state[env->flexidag_slot_id].pCvtable;
                cvtable_numentries  = flexidag_sys_state[env->flexidag_slot_id].cvtable_numlines;
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

            *ptr = NULL;
            if (size != NULL) {
                *size = 0U;
            }
            for (i = 0U; i < cvtable_numentries; i++) {
                info = &pCVTable[i];
                tmp_ptr = ambacv_c2v(info->CVTable_name_daddr);
                if(tmp_ptr != NULL) {
                    typecast(&info_name,&tmp_ptr);
                    if (thread_strcmp(name, info_name) == 0) {
                        if(info->CVTable_daddr != 0U) {
                            *ptr = ambacv_c2v(info->CVTable_daddr);
                        } else {
                            *ptr = NULL;
                        }
                        if (size != NULL) {
                            *size = info->cvtable_size;
                        }
                        retcode = ERRCODE_NONE;
                        break;
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t  cvtask_metadata_find(
    const void *vpMetaDataBlock,
    uint32_t   MetaDataBlockSize,
    uint32_t   metadata_type,
    const void **vppMetaDataBuffer,
    uint32_t   *pMetaDataSize)
{
    uint32_t retcode = ERRCODE_NONE;
    const char *baseaddr = NULL, *newbaseaddr = NULL;
    const metadata_block_header_t *pMetaHdr;
    uint32_t pos = 0U;
    uint32_t leave = 0U;
    const void *ptr;

    if (vpMetaDataBlock == NULL) {
        console_printU5("[ERROR] cvtask_metadata_find() : NULL vpMetaDataBlock", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else if (vppMetaDataBuffer == NULL) {
        console_printU5("[ERROR] cvtask_metadata_find() : NULL vppMetaDataBuffer", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else if (pMetaDataSize == NULL) {
        console_printU5("[ERROR] cvtask_metadata_find() : NULL pMetaDataSize", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        *vppMetaDataBuffer = NULL;
        *pMetaDataSize = 0U;

        typecast(&baseaddr,&vpMetaDataBlock);
        while (pos < MetaDataBlockSize) {
            newbaseaddr = &baseaddr[pos];
            typecast(&pMetaHdr,&newbaseaddr);
            if (pMetaHdr->metadata_type == metadata_type) {
                ptr = &baseaddr[pos + sizeof(metadata_block_header_t)];
                *vppMetaDataBuffer = ptr;
                *pMetaDataSize     = pMetaHdr->metadata_size;
                leave = 1U;
            }

            if (pMetaHdr->metadata_type == METADATA_TYPE_END) {
                leave = 1U;
            }

            if(leave == 1U) {
                break;
            }
            pos += pMetaHdr->metadata_block_size;
        }
    }

    return retcode;
}

uint32_t cvtask_get_debug_level(void)
{
    const cvtask_thpool_env_t *env;
    uint32_t ret = 0U;

    env = schdr_get_thenv();
    if (env == NULL) {
        console_printU5("[ERROR] cvtask_get_debug_level() : wrong context!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else {
        ret = env->run_req->cvtask_debug_level;
    }

    return ret;
}

uint32_t cvtask_get_input_size(uint32_t index)
{
    const cvtask_thpool_env_t *env;
    const cvmem_membuf_t *membuf;
    uint32_t ret = 0U;
    const void *ptr;

    env = schdr_get_thenv();
    if (env == NULL) {
        console_printU5("[ERROR] cvtask_get_input_size() : wrong context!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else if (index >= env->run_req->cvtask_num_inputs) {
        console_printU5("[ERROR] cvtask_get_input_size() : index out of range!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else {
        ptr = ambacv_c2v(env->run_req->InputMemBuf_daddr[index]);
        if(ptr != NULL) {
            typecast(&membuf,&ptr);
            ret = membuf->hdr_fixed.payload_size;
        } else {
            console_printU5("[ERROR] cvtask_get_input_size() : membuf == NULL !", 0U, 0U, 0U, 0U, 0U);
            ret = 0U;
        }
    }

    return ret;
}

uint32_t cvtask_get_feedback_size(uint32_t index)
{
    const cvtask_thpool_env_t *env;
    const cvmem_membuf_t *membuf;
    uint32_t ret = 0U;
    const void *ptr;

    env = schdr_get_thenv();
    if (env == NULL) {
        console_printU5("[ERROR] cvtask_get_feedback_size() : wrong context!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else if (index >= env->run_req->cvtask_num_feedback) {
        console_printU5("[ERROR] cvtask_get_feedback_size() : index out of range!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else {
        ptr = ambacv_c2v(env->run_req->FeedbackMemBuf_daddr[index]);
        if(ptr != NULL) {
            typecast(&membuf,&ptr);
            ret = membuf->hdr_fixed.payload_size;
        } else {
            console_printU5("[ERROR] cvtask_get_feedback_size() : membuf == NULL !", 0U, 0U, 0U, 0U, 0U);
            ret = 0U;
        }
    }

    return ret;
}

uint32_t cvtask_get_output_size(uint32_t index)
{
    const cvtask_thpool_env_t *env;
    const cvmem_membuf_t *membuf;
    uint32_t ret = 0U;
    const void *ptr;

    env = schdr_get_thenv();
    if (env == NULL) {
        console_printU5("[ERROR] cvtask_get_output_size() : wrong context!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else if (index >= env->run_req->cvtask_num_outputs) {
        console_printU5("[ERROR] cvtask_get_output_size() : index out of range!", 0U, 0U, 0U, 0U, 0U);
        ret = 0U;
    } else {
        ptr = ambacv_c2v(env->run_req->OutputMemBuf_daddr[index]);
        if(ptr != NULL) {
            typecast(&membuf,&ptr);
            ret = membuf->hdr_fixed.payload_size;
        } else {
            console_printU5("[ERROR] cvtask_get_output_size() : membuf == NULL !", 0U, 0U, 0U, 0U, 0U);
            ret = 0U;
        }
    }

    return ret;
}

uint32_t cvtask_set_debug_level(uint32_t num_entries,
                                const uint16_t *sysflow_index,
                                const uint16_t *debug_level)
{
    uint32_t  retcode = ERRCODE_NONE;
    armvis_msg_t msg;
    uint32_t pos;
    const cvtask_thpool_env_t *env;
    const uint16_t *sysflow_ptr;
    const uint16_t *level_ptr;

    if((sysflow_index == NULL) || (debug_level == NULL)) {
        console_printU5("[ERROR] cvtask_set_debug_level() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        sysflow_ptr = sysflow_index;
        level_ptr = debug_level;

        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_SET_DEBUG_LEVEL;
                msg.hdr.message_id    = (uint16_t)0U;
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_SET_DEBUG_LEVEL;
                msg.hdr.message_id    = (uint16_t)(0x0100U + env->flexidag_slot_id);
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            msg.hdr.message_retcode = 0U;

            if (num_entries == CVTASK_SET_DEBUG_LEVEL_ALL) {
                msg.msg.set_debug_level.num_entries           = num_entries;
                msg.msg.set_debug_level.target_debug_level[0] = level_ptr[0];
                retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_debug_level)));
                if(retcode != ERRCODE_NONE) {
                    retcode = CVTASK_ERR_GENERAL;
                }
            } else {
                for (pos = 0U; pos < num_entries; pos += SET_DEBUG_LEVEL_MAX_ENTRIES) {
                    uint32_t entries = num_entries - pos;
                    if (entries >= SET_DEBUG_LEVEL_MAX_ENTRIES) {
                        entries = SET_DEBUG_LEVEL_MAX_ENTRIES;
                    }
                    msg.msg.set_debug_level.num_entries = entries;
                    if( thread_memcpy(msg.msg.set_debug_level.target_sysflow_index, sysflow_ptr,
                                      entries * sizeof(uint16_t)) != ERRCODE_NONE) {
                        retcode = CVTASK_ERR_GENERAL;
                    } else {
                        if(thread_memcpy(msg.msg.set_debug_level.target_debug_level, level_ptr,
                                         entries * sizeof(uint16_t)) != ERRCODE_NONE) {
                            retcode = CVTASK_ERR_GENERAL;
                        } else {
                            retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_debug_level)));
                            if(retcode != ERRCODE_NONE) {
                                retcode = CVTASK_ERR_GENERAL;
                            } else {
                                sysflow_ptr = &sysflow_ptr[entries];
                                level_ptr = &level_ptr[entries];
                            }
                        }
                    }
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t cvtask_set_frequency(uint32_t num_entries,
                              const uint16_t *sysflow_index_array,
                              const uint8_t *cvtask_frequency)
{
    uint32_t  retcode = ERRCODE_NONE;
    armvis_msg_t msg;
    uint32_t pos, j;
    const cvtask_thpool_env_t *env;
    const uint16_t *sysflow_ptr;
    const uint8_t *freq_ptr;


    if((sysflow_index_array == NULL) || (cvtask_frequency == NULL)) {
        console_printU5("[ERROR] cvtask_set_frequency() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        sysflow_ptr = sysflow_index_array;
        freq_ptr = cvtask_frequency;

        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_SET_FREQUENCY;
                msg.hdr.message_id    = (uint16_t)0U;
            } else {
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_SET_FREQ;
                msg.hdr.message_id    = (uint16_t)(0x0100U + env->flexidag_slot_id);
            }
            msg.hdr.message_retcode = 0U;

            if (num_entries == CVTASK_SET_FREQUENCY_ALL) {
                msg.msg.set_frequency.num_entries = num_entries;
                msg.msg.set_frequency.frequency[0] = freq_ptr[0];
                retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency)));
                if(retcode != ERRCODE_NONE) {
                    retcode = CVTASK_ERR_GENERAL;
                }
            } else {
                for (pos = 0U; pos < num_entries; pos += SET_FREQUENCY_MAX_ENTRIES) {
                    uint32_t entries = num_entries - pos;
                    if (entries >= SET_FREQUENCY_MAX_ENTRIES) {
                        entries = SET_FREQUENCY_MAX_ENTRIES;
                    }
                    msg.msg.set_frequency.num_entries = entries;
                    if(thread_memcpy(msg.msg.set_frequency.sysflow_index, sysflow_ptr,
                                     entries * sizeof(uint16_t)) != ERRCODE_NONE) {
                        retcode = CVTASK_ERR_GENERAL;
                    } else {
                        for (j=0U; j < entries; j++) {
                            msg.msg.set_frequency.frequency[j]=freq_ptr[j];
                        }
                        retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency)));
                        if(retcode != ERRCODE_NONE) {
                            retcode = CVTASK_ERR_GENERAL;
                        } else {
                            sysflow_ptr = &sysflow_ptr[entries];
                            freq_ptr = &freq_ptr[entries];
                        }
                    }
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t cvtask_set_frequency_at_time(uint32_t num_entries,
                                      uint32_t target_frametime,
                                      const uint16_t *sysflow_index_array,
                                      const uint8_t *cvtask_frequency)
{
    uint32_t  retcode = ERRCODE_NONE;
    armvis_msg_t msg;
    uint32_t pos, j;
    const cvtask_thpool_env_t *env;
    const uint16_t *sysflow_ptr;
    const uint8_t *freq_ptr;

    if((sysflow_index_array == NULL) || (cvtask_frequency == NULL)) {
        console_printU5("[ERROR] cvtask_set_frequency_at_time() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        sysflow_ptr = sysflow_index_array;
        freq_ptr = cvtask_frequency;

        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_SET_FREQUENCY_AT_TIME;
                msg.hdr.message_id    = (uint16_t)0U;
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id ! FLEXIDAG_INVALID_SLOT_ID) */
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_SET_FREQ_AT_TIME;
                msg.hdr.message_id    = (uint16_t)(0x0100U + env->flexidag_slot_id);
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            msg.hdr.message_retcode = 0U;

            if (num_entries == CVTASK_SET_FREQUENCY_ALL) {
                msg.msg.set_frequency_at_time.num_entries       = num_entries;
                msg.msg.set_frequency_at_time.target_frametime  = target_frametime;
                msg.msg.set_frequency_at_time.frequency[0]      = freq_ptr[0];
                retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency_at_time)));
                if(retcode != ERRCODE_NONE) {
                    retcode = CVTASK_ERR_GENERAL;
                }
            } else {
                msg.msg.set_frequency_at_time.target_frametime  = target_frametime;
                for (pos = 0U; pos < num_entries; pos += SET_FREQUENCY_MAX_ENTRIES) {
                    uint32_t entries = num_entries - pos;
                    if (entries >= SET_FREQUENCY_MAX_ENTRIES) {
                        entries = SET_FREQUENCY_MAX_ENTRIES;
                    }
                    msg.msg.set_frequency_at_time.num_entries = entries;
                    if(thread_memcpy(msg.msg.set_frequency_at_time.sysflow_index, sysflow_ptr,
                                     entries * sizeof(uint16_t)) != ERRCODE_NONE) {
                        retcode = CVTASK_ERR_GENERAL;
                    } else {
                        for (j=0U; j < entries; j++) {
                            msg.msg.set_frequency_at_time.frequency[j]=freq_ptr[j];
                        }
                        retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency_at_time)));
                        if(retcode != ERRCODE_NONE) {
                            retcode = CVTASK_ERR_GENERAL;
                        } else {
                            sysflow_ptr = &sysflow_ptr[entries];
                            freq_ptr = &freq_ptr[entries];
                        }
                    }
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t cvtask_set_frequency_at_frameset_id(uint32_t num_entries,
        uint32_t target_frameset_id,
        const uint16_t *sysflow_index_array,
        const uint8_t *cvtask_frequency)
{
    uint32_t  retcode = ERRCODE_NONE;
    armvis_msg_t msg;
    uint32_t pos, j;
    const cvtask_thpool_env_t *env;
    const uint16_t *sysflow_ptr;
    const uint8_t *freq_ptr;

    if((sysflow_index_array == NULL) || (cvtask_frequency == NULL)) {
        console_printU5("[ERROR] cvtask_set_frequency_at_frameset_id() : invalid arg ", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        sysflow_ptr = sysflow_index_array;
        freq_ptr = cvtask_frequency;

        env = schdr_get_thenv();
        if(env == NULL) {
            retcode = CVTASK_ERR_NULL_POINTER;
        } else {
            if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_SET_FREQUENCY_AT_FRAME;
                msg.hdr.message_id    = (uint16_t)0U;
            } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_SET_FREQ_AT_FRAME;
                msg.hdr.message_id    = (uint16_t)(0x0100U + env->flexidag_slot_id);
            } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            msg.hdr.message_retcode = 0U;

            if (num_entries == CVTASK_SET_FREQUENCY_ALL) {
                msg.msg.set_frequency_at_frame.num_entries = num_entries;
                msg.msg.set_frequency_at_frame.target_frameset_id = target_frameset_id;
                msg.msg.set_frequency_at_frame.frequency[0] = freq_ptr[0];
                retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency_at_frame)));
                if(retcode != ERRCODE_NONE) {
                    retcode = CVTASK_ERR_GENERAL;
                }
            } else {
                msg.msg.set_frequency_at_frame.target_frameset_id = target_frameset_id;
                for (pos = 0U; pos < num_entries; pos += SET_FREQUENCY_MAX_ENTRIES) {
                    uint32_t entries = num_entries - pos;
                    if (entries >= SET_FREQUENCY_MAX_ENTRIES) {
                        entries = SET_FREQUENCY_MAX_ENTRIES;
                    }
                    msg.msg.set_frequency_at_frame.num_entries = entries;
                    if(thread_memcpy(msg.msg.set_frequency_at_frame.sysflow_index, sysflow_ptr,
                                     entries * sizeof(uint16_t)) != ERRCODE_NONE) {
                        retcode = CVTASK_ERR_GENERAL;
                    } else {
                        for (j=0U; j < entries; j++) {
                            msg.msg.set_frequency_at_frame.frequency[j]=freq_ptr[j];
                        }
                        retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency_at_frame)));
                        if(retcode != ERRCODE_NONE) {
                            retcode = CVTASK_ERR_GENERAL;
                        } else {
                            sysflow_ptr = &sysflow_ptr[entries];
                            freq_ptr = &freq_ptr[entries];
                        }
                    }
                    if(retcode != ERRCODE_NONE) {
                        break;
                    }
                }
            }
        }
    }
    return retcode;
}

uint32_t cvtask_make_output_available_early(uint32_t output_index)
{
    uint32_t retcode = ERRCODE_NONE;
    armvis_msg_t msg;
    const schedmsg_cvtask_run_t *req;
    const cvtask_thpool_env_t *env;

    env = schdr_get_thenv();
    if ( (env == NULL) || (env->run_req == NULL)) {
        console_printU5("[ERROR] cvtask_make_output_available_early() : is called in wrong context!", 0U, 0U, 0U, 0U, 0U);
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        req = env->run_req;
        if (output_index >= req->cvtask_num_outputs) {
            retcode = CVTASK_ERR_INDEX_TOO_LARGE;
            console_printU5("[ERROR] cvtask_make_output_available_early() : Called with out of bounds output_index (%d, cvtask_num_outputs=%d)\n",
                            output_index, req->cvtask_num_outputs, 0U, 0U, 0U);
        } /* if (output_index >= req->cvtask_num_outputs) */
        else { /* if (output_index < req->cvtask_num_outputs) */
            msg.hdr.message_type    = (uint16_t)SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY;
            msg.hdr.message_id      = (uint16_t)0U;
            msg.hdr.message_retcode = ERRCODE_NONE;

            msg.msg.reply_cvtask_run_partial.scheduler_tasknode_id  = env->run_req->scheduler_tasknode_id;
            msg.msg.reply_cvtask_run_partial.output_index           = output_index;
            msg.msg.reply_cvtask_run_partial.output_membuf_daddr    = req->OutputMemBuf_daddr[output_index];

            SCHDR_PRINTF(".cvtask_run early-out,  uuid=%d, frame=%d, output=%d",
                         schdr_sys_state.pSysFlow[req->sysflow_index].uuid,
                         req->cvtask_frameset_id, output_index, 0U, 0U);
            retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) +  sizeof(msg.msg.reply_cvtask_run_partial)));
            if(retcode != ERRCODE_NONE) {
                retcode = CVTASK_ERR_GENERAL;
            }
        } /* if (output_index < req->cvtask_num_outputs) */
    }
    return retcode;
}

uint32_t cvtask_send_private_msg(uint32_t target_sysflow_index, const uint8_t *pMessage)
{
    const cvtask_thpool_env_t *env;
    uint32_t retcode;

    env = schdr_get_thenv();
    if(env == NULL) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } /* if(env == NULL) */
    else { /* if(env != NULL) */
        armvis_msg_t msg;

        if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            msg.hdr.message_type    = (uint16_t)SCHEDMSG_SEND_PRIVATE_MSG;
            msg.hdr.message_id      = (uint16_t)0U;
        } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            msg.hdr.message_type    = (uint16_t)SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG;
            msg.hdr.message_id      = (uint16_t)(0x0100U + env->flexidag_slot_id);
        } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */

        msg.hdr.message_retcode = ERRCODE_NONE;

        msg.msg.sched_send_private_msg.target_sysflow_index = target_sysflow_index;
        msg.msg.sched_send_private_msg.reserved0            = 0U;
        msg.msg.sched_send_private_msg.reserved1            = 0U;
        msg.msg.sched_send_private_msg.reserved2            = 0U;

        retcode = thread_memcpy(&msg.msg.sched_send_private_msg.message, pMessage, sizeof(msg.msg.sched_send_private_msg.message));
        if (retcode == ERRCODE_NONE) {
            retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) +  sizeof(msg.msg.reply_cvtask_run_partial)));
            if(retcode != ERRCODE_NONE) {
                retcode = CVTASK_ERR_GENERAL;
            }
        } /* if (retcode == ERRCODE_NONE) : thread_memcpy() */
        else {
            retcode = CVTASK_ERR_GENERAL;
        }
    } /* if (env != NULL) */

    return retcode;
} /* cvtask_send_private_msg() */

uint32_t cvtask_prof_new_action(uint8_t  cvcore, uint16_t sysflow_index, uint32_t frame_num)
{
    uint32_t  retcode = ERRCODE_NONE;
    schdr_log_info_t  *pMy_linfo;
    const cvtask_thpool_env_t *env;
    uint32_t  base_timestamp;

    env = schdr_get_thenv();
    if(env == NULL) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        base_timestamp = schdr_get_event_time();
        if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            cvlog_perf_entry_t *e;
            pMy_linfo = schdr_sys_state.pLinfo;
            if (pMy_linfo != NULL) {
                e = get_next_perf_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event                        = EVENT_NEW_FRAME;
                    e->cvcore                       = cvcore;
                    e->sysflow_index                = sysflow_index;
                    e->time_stamp                   = base_timestamp;
                    e->perfmsg.new_frame.frame_num  = frame_num;
                    e->perfmsg.new_frame.reserved   = env->reserved;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */
        } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            cvlog_perf_flexidag_entry_t *e;
            pMy_linfo = flexidag_sys_state[env->flexidag_slot_id].pLinfo[0];

            if (pMy_linfo != NULL) {
                e = get_next_perf_flexidag_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event                      = EVENT_FLEXIDAG_NEW_FRAME;
                    e->cvcore                     = cvcore;
                    e->flexidag_slot_id           = (uint8_t)env->flexidag_slot_id;
                    e->sysflow_index              = (uint8_t)sysflow_index;
                    e->time_stamp                 = base_timestamp;
                    e->fdmsg.new_frame.frame_num  = frame_num;
                    e->fdmsg.new_frame.reserved   = env->reserved;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */

            /* also echo to master scheduler perf log */
            pMy_linfo = schdr_sys_state.pLinfo;
            if (pMy_linfo != NULL) {
                e = get_next_perf_flexidag_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event                      = EVENT_FLEXIDAG_NEW_FRAME;
                    e->cvcore                     = cvcore;
                    e->flexidag_slot_id           = (uint8_t)env->flexidag_slot_id;
                    e->sysflow_index              = (uint8_t)sysflow_index;
                    e->time_stamp                 = base_timestamp;
                    e->fdmsg.new_frame.frame_num  = frame_num;
                    e->fdmsg.new_frame.reserved   = env->reserved;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */
        } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    }
    return retcode;
} /* cvtask_prof_new_action() */

uint32_t cvtask_prof_unit_on(uint8_t cvcore, uint16_t sysflow_index)
{
    uint32_t  retcode = ERRCODE_NONE;
    schdr_log_info_t  *pMy_linfo;
    const cvtask_thpool_env_t *env;
    uint32_t  base_timestamp;

    env = schdr_get_thenv();
    if(env == NULL) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        base_timestamp = schdr_get_event_time();
        if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            cvlog_perf_entry_t *e;
            pMy_linfo = schdr_sys_state.pLinfo;
            if (pMy_linfo != NULL) {
                e = get_next_perf_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event              = EVENT_CVCORE_ON;
                    e->cvcore             = cvcore;
                    e->sysflow_index      = sysflow_index;
                    e->time_stamp         = base_timestamp;
                    e->perfmsg.u32data[0] = 0U;
                    e->perfmsg.u32data[1] = 0U;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */
        } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            cvlog_perf_flexidag_entry_t *e;
            pMy_linfo = flexidag_sys_state[env->flexidag_slot_id].pLinfo[0];
            if (pMy_linfo != NULL) {
                e = get_next_perf_flexidag_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event            = EVENT_FLEXIDAG_CVCORE_ON;
                    e->cvcore           = cvcore;
                    e->flexidag_slot_id = (uint8_t) env->flexidag_slot_id;
                    e->sysflow_index    = (uint8_t)sysflow_index;
                    e->time_stamp       = base_timestamp;
                    e->fdmsg.u32data[0] = 0U;
                    e->fdmsg.u32data[1] = 0U;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */

            /* also echo to master scheduler perf log */
            pMy_linfo = schdr_sys_state.pLinfo;
            if (pMy_linfo != NULL) {
                e = get_next_perf_flexidag_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event            = EVENT_FLEXIDAG_CVCORE_ON;
                    e->cvcore           = cvcore;
                    e->flexidag_slot_id = (uint8_t) env->flexidag_slot_id;
                    e->sysflow_index    = (uint8_t)sysflow_index;
                    e->time_stamp       = base_timestamp;
                    e->fdmsg.u32data[0] = 0U;
                    e->fdmsg.u32data[1] = 0U;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */
        } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    }
    return retcode;
} /* cvtask_prof_unit_on() */

uint32_t cvtask_prof_unit_off(uint8_t cvcore, uint16_t sysflow_index)
{
    uint32_t  retcode = ERRCODE_NONE;
    schdr_log_info_t  *pMy_linfo;
    const cvtask_thpool_env_t *env;
    uint32_t  base_timestamp;

    env = schdr_get_thenv();
    if(env == NULL) {
        retcode = CVTASK_ERR_NULL_POINTER;
    } else {
        base_timestamp = schdr_get_event_time();
        if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
            cvlog_perf_entry_t *e;
            pMy_linfo = schdr_sys_state.pLinfo;
            if (pMy_linfo != NULL) {
                e = get_next_perf_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event              = EVENT_CVCORE_OFF;
                    e->cvcore             = cvcore;
                    e->sysflow_index      = sysflow_index;
                    e->time_stamp         = base_timestamp;
                    e->perfmsg.u32data[0] = 0U;
                    e->perfmsg.u32data[1] = 0U;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */
        } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            cvlog_perf_flexidag_entry_t *e;
            pMy_linfo = flexidag_sys_state[env->flexidag_slot_id].pLinfo[0];
            if (pMy_linfo != NULL) {
                e = get_next_perf_flexidag_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event            = EVENT_FLEXIDAG_CVCORE_OFF;
                    e->cvcore           = cvcore;
                    e->flexidag_slot_id = (uint8_t)env->flexidag_slot_id;
                    e->sysflow_index    = (uint8_t)sysflow_index;
                    e->time_stamp       = base_timestamp;
                    e->fdmsg.u32data[0] = 0U;
                    e->fdmsg.u32data[1] = 0U;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */

            /* also echo to master scheduler perf log */
            pMy_linfo = schdr_sys_state.pLinfo;
            if (pMy_linfo != NULL) {
                e = get_next_perf_flexidag_entry(&pMy_linfo[2]);
                if (e != NULL) {
                    e->event            = EVENT_FLEXIDAG_CVCORE_OFF;
                    e->cvcore           = cvcore;
                    e->flexidag_slot_id = (uint8_t)env->flexidag_slot_id;
                    e->sysflow_index    = (uint8_t)sysflow_index;
                    e->time_stamp       = base_timestamp;
                    e->fdmsg.u32data[0] = 0U;
                    e->fdmsg.u32data[1] = 0U;
                } /* if (e != NULL) */
            } /* if (pMy_linfo != NULL) */
        } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    }
    return retcode;
} /* cvtask_prof_unit_off() */

void cvtask_printf(int32_t lvl, const char *fmt, uint32_t v0, uint32_t v1,
                   uint32_t v2, uint32_t v3, uint32_t v4)
{
    uint8_t debug_level;
    const cvtask_thpool_env_t *env;
    const schedmsg_cvtask_run_t *req;
    schdr_log_info_t  *pMy_linfo;
    uint32_t ret = 0U;
#ifndef DISABLE_ARM_CVTASK
    uint64_t addr1 = 0UL,addr2 = 0UL;
    const char *paddr1,*paddr2;
    cvlog_cvtask_entry_t *e;
#endif

    if(fmt == NULL) {
        console_printU5("[ERROR] cvtask_printf() : fmt == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        {

            env = schdr_get_thenv();
            if ((env == NULL) || (env->run_req == NULL)) {
                console_printS5(
                    "[ERROR] cvtask_printf() : Calling CVTASK_PRINTF out of context\n",
                    NULL, NULL, NULL, NULL, NULL);
                ret = 1U;
            }

            if(ret == 0U) {
                req = env->run_req;

                /* output to console */
                debug_level = req->cvtask_debug_level & 0xFU;
                if ((uint8_t)lvl <= debug_level) {
                    module_printU5(AMBA_CVTASK_PRINT_MODULE_ID, fmt, v0, v1, v2, v3, v4);
                    module_printU5(AMBA_CVTASK_PRINT_MODULE_ID, "\n", 0U, 0U, 0U, 0U, 0U);
                }

                if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) {
                    pMy_linfo = schdr_sys_state.pLinfo;
                } /* if (env->flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
                else { /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
                    pMy_linfo = flexidag_sys_state[env->flexidag_slot_id].pLinfo[0];
                } /* if (env->flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
#ifndef DISABLE_ARM_CVTASK
                /* output to logger buffer */
                debug_level = req->cvtask_debug_level >> 4;
                if ((uint8_t)lvl <= debug_level) {
                    paddr1 = &__cvtask_format_start;
                    paddr2 = &__cvtask_format_end;
                    if(paddr1 != paddr2) {
                        e = get_next_cvtask_entry(&pMy_linfo[0]);
                        if (e != NULL) {
                            e->hdr.entry_flags.index = (uint16_t)req->arm_cvtask_index;
                            e->hdr.entry_flags.dbg_level = debug_level;
                            e->hdr.entry_flags.src_thread = (uint8_t)env->cpu_id;

                            e->entry_time = cvtask_get_timestamp();
                            typecast(&addr1,&fmt);
                            typecast(&addr2,&paddr1);
                            e->entry_string_offset = (uint32_t)(addr1 - addr2);
                            e->entry_arg1 = v0;
                            e->entry_arg2 = v1;
                            e->entry_arg3 = v2;
                            e->entry_arg4 = v3;
                            e->entry_arg5 = v4;
                        }
                    }
                }
#endif
            }
        }
    }
}

#endif
