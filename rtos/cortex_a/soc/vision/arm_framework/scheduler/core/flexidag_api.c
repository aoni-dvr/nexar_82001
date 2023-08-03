/**
 *  @file flexidag_api.c
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
 *  @details Flexidag APIs
 *
 */

#include "os_api.h"
#include "schdrmsg_def.h"
#include "schdr_api.h"
#include "dram_mmap.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cvtask_errno.h"
#include "cvapi_logger_interface.h"
#include <cvapi_flexidag.h>
#include <cvsched_flexibin_format.h>

flexidag_system_state_t flexidag_sys_state[FLEXIDAG_MAX_SLOTS] GNU_SECTION_NOZEROINIT;
psem_t                  flexidag_sys_sem[FLEXIDAG_MAX_SLOTS];

static uint32_t fd_send_msg_entry(cvmem_messagebuf_t *entry, uint32_t dst,
                                  uint32_t frame, uint32_t len, uint32_t flexidag_slot_id)
{
    uint32_t retcode = ERRCODE_NONE;
    const cvtask_thpool_env_t *env;
    armvis_msg_t schdr_msg;

    if(entry == NULL) {
        console_printU5("[ERROR] fd_send_msg_entry() : entry == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_MSG_ENTRY_INVALID;
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
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            schdr_msg.hdr.message_type  = (uint16_t) SCHEDMSG_FLEXIDAG_CVTASK_MSG;
            schdr_msg.hdr.message_id    = (uint16_t) (0x0100U + flexidag_slot_id);
            schdr_msg.hdr.message_retcode                   = ERRCODE_NONE;
            schdr_msg.msg.reply_cvtask_msg.CVTaskMessage_daddr  = (uint32_t)ambacv_v2c(entry);
            schdr_msg.msg.reply_cvtask_msg.target_frameset_id   = frame;
            retcode = schdrmsg_send_msg(&schdr_msg, (uint32_t)(sizeof(schdr_msg.hdr) + sizeof(schdr_msg.msg.reply_cvtask_msg)));
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] fd_send_msg_entry() : schdrmsg_send_msg fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL;
            }
        } else {
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        }
    }
    return retcode;
}

static cvmem_messagebuf_t *fd_find_msg_entry(cvmem_messagepool_t *pool)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t entry = 0UL;
    cvmem_messagebuf_t *pentry = NULL;
    const void *ptr = NULL;

    if(pool == NULL) {
        console_printU5("[ERROR] fd_find_msg_entry() : pool == NULL \n", 0U, 0U, 0U, 0U, 0U);
    } else {
        retcode = drv_schdr_find_msg(ambacv_v2c(pool), &entry);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] fd_find_msg_entry() : drv_schdr_find_msg fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
        } else {
            ptr =ambacv_c2v(entry);
        }
        if(ptr == NULL) {
            console_printU5("[ERROR] fd_find_msg_entry() : ptr == NULL \n", 0U, 0U, 0U, 0U, 0U);
        } else {
            typecast(&pentry,&ptr);
        }
    }
    return pentry;
}

static void fd_print_cvtable(uint32_t flexidag_slot_id)
{
    const cvtable_entry_t  *pTable = NULL;
    const cvtable_entry_t  *info;
    uint32_t  table_size = 0U;
    uint32_t  i;
    uint32_t  str_pos,tmp_pos;
    char str_line[512];
    const char *name;
    const void *ptr;

    if (schdr_sys_cfg.verbosity >= LVL_VERBOSE) {
        pTable      = flexidag_sys_state[flexidag_slot_id].pCvtable;
        table_size  = flexidag_sys_state[flexidag_slot_id].cvtable_numlines;
        if ((table_size > 0U) && (pTable != NULL)) {
            module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, "##################################################\n"
                           "    flexidag[%3d] cvtable info\n"
                           "##################################################\n",
                           flexidag_slot_id, 0U, 0U, 0U, 0U);
        } /* if ((table_size > 0) && (pTable != NULL)) */

        if(pTable != NULL) {
            for (i = 0U; i < table_size; i++) {
                info = &pTable[i];
                str_pos = snprintf_uint5(&str_line[0], (uint32_t)sizeof(str_line), "    %3d: size=%7d, buff=0x%08X",
                                         i,
                                         info->cvtable_size,
                                         (uint32_t)ambacv_c2p(info->CVTable_daddr),
                                         0U, 0U);
                if(info->CVTable_name_daddr != 0U) {
                    ptr = ambacv_c2v(info->CVTable_name_daddr);
                    if(ptr != NULL) {
                        typecast(&name,&ptr);
                        tmp_pos = snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), " => name=%s\n",
                                                name,
                                                NULL, NULL, NULL, NULL);
                        str_pos += tmp_pos;
                        (void) str_pos;
                    }
                }
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID, &str_line[0], 0U, 0U, 0U, 0U, 0U);
            }
        }
    }
} /* fd_print_cvtable() */

static void fd_print_sysflow(uint32_t flexidag_slot_id)
{
    const sysflow_entry_t *pSysFlow = NULL;
    uint32_t  i;
    uint32_t  str_pos;
    uint32_t  sysflow_numentries = 0U;
    char str_line[512];

    if (schdr_sys_cfg.verbosity >= LVL_VERBOSE) {
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            pSysFlow            = flexidag_sys_state[flexidag_slot_id].pSysFlow;
            sysflow_numentries  = flexidag_sys_state[flexidag_slot_id].sysflow_numlines;

            if ((sysflow_numentries > 0U) && (pSysFlow != NULL)) {
                module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"##################################################\n"
                               "   flexidag[%3d] sysflow table\n"
                               "##################################################\n",
                               flexidag_slot_id, 0U, 0U, 0U, 0U);
            } /* if ((sysflow_numentries > 0) && (pCVName != NULL)) */

            if(pSysFlow != NULL) {
                for (i = 0U; i < sysflow_numentries; i++) {
                    str_pos = snprintf_uint5(&str_line[0], (uint32_t)sizeof(str_line), "    %3d: uuid=%-4d ",
                                             i,
                                             pSysFlow->uuid,
                                             0U, 0U, 0U);
                    str_pos += snprintf_str5(&str_line[str_pos], (uint32_t)sizeof(str_line), "\t%s, %s, %s, %s\n",
                                             pSysFlow->instance_name,
                                             pSysFlow->algorithm_name,
                                             pSysFlow->step_name,
                                             pSysFlow->cvtask_name,
                                             NULL);
                    (void) str_pos;
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,&str_line[0], 0U, 0U, 0U, 0U, 0U);
                    pSysFlow++;
                }
                module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,"\n", NULL, NULL, NULL, NULL, NULL);
            }
        } else {
            console_printU5("[ERROR] fd_print_sysflow_name() : invalid slot id", 0U, 0U, 0U, 0U, 0U);
        }
    }

} /* fd_print_sysflow_name() */

static uint32_t fd_cvtable_find(
    IN  const char *target,
    OUT const void **ptr,
    OUT uint32_t *size,
    IN uint32_t  flexidag_slot_id)
{
    uint32_t i;
    const cvtable_entry_t  *pCVTable;
    const cvtable_entry_t  *info;
    uint32_t cvtable_numentries;
    const char *name;
    uint32_t retcode = ERR_DRV_FLEXIDAG_CVTABLE_MISMATCHED;
    const void *tmp_ptr;

    if (target == NULL) {
        console_printU5("[ERROR] fd_cvtable_find() : target == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (ptr == NULL) {
        console_printU5("[ERROR] fd_cvtable_find() : ptr == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (size == NULL) {
        console_printU5("[ERROR] fd_cvtable_find() : size == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            pCVTable            = flexidag_sys_state[flexidag_slot_id].pCvtable;
            cvtable_numentries  = flexidag_sys_state[flexidag_slot_id].cvtable_numlines;
            *ptr = NULL;
            *size = 0U;
            for (i = 0U; i < cvtable_numentries; i++) {
                info = &pCVTable[i];
                tmp_ptr = ambacv_c2v(info->CVTable_name_daddr);
                if(tmp_ptr != NULL) {
                    typecast(&name,&tmp_ptr);
                    if (thread_strcmp(target, name) == 0) {
                        if(info->CVTable_daddr != 0U) {
                            *ptr = ambacv_c2v(info->CVTable_daddr);
                        } else {
                            *ptr = NULL;
                        }
                        *size = info->cvtable_size;
                        retcode = ERRCODE_NONE;
                        break;
                    }
                }
            }
        } else {
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        }
    }

    return retcode;
}

static uint32_t fd_get_sysflow_index_list(
    IN  const char *inst_name,
    IN  const char *algo_name,
    IN  const char *step_name,
    IN  const char *task_name,
    OUT uint16_t *id_array,
    IN  uint32_t  array_size,
    OUT uint32_t *num_found,
    IN  uint32_t flexidag_slot_id)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t index;
    uint32_t i;
    const sysflow_entry_t *pSysFlow;
    uint32_t sysflow_numlines;

    if (id_array == NULL) {
        console_printU5("[ERROR] fd_get_sysflow_index_list() : id_array == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            pSysFlow           = flexidag_sys_state[flexidag_slot_id].pSysFlow;
            sysflow_numlines  = flexidag_sys_state[flexidag_slot_id].sysflow_numlines;

            index = 0U;
            for (i = 0U; i < sysflow_numlines; i++) {
                const sysflow_entry_t *curr = &pSysFlow[i];

                // check instance name
                if (inst_name != NULL) {
                    if (thread_strcmp(inst_name, curr->instance_name)!= 0) {
                        continue;
                    }
                }

                // check algorithm name
                if (algo_name != NULL) {
                    if (thread_strcmp(algo_name, curr->algorithm_name)!= 0) {
                        continue;
                    }
                }

                // check step name
                if (step_name != NULL) {
                    if (thread_strcmp(step_name, curr->step_name)!= 0) {
                        continue;
                    }
                }

                // chech cvtask name
                if (task_name != NULL) {
                    if (thread_strcmp(task_name, curr->cvtask_name)!= 0) {
                        continue;
                    }
                }

                // we found one match
                if (index < array_size) {
                    id_array[index] = (uint16_t)i;
                }

                ++index;
            }

            if (num_found != NULL) {
                *num_found = index;
            } /* if (num_found != NULL) */
        } else {
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        }
    }
    return retcode;
}

static uint32_t fd_get_sysflow_index_by_UUID(
    IN   uint32_t fd_uuid,
    OUT  uint16_t *pIndex,
    IN   uint32_t  flexidag_slot_id)
{
    uint32_t  retcode = ERRCODE_NONE;

    if (pIndex == NULL) {
        console_printU5("[ERROR] fd_get_sysflow_index_by_UUID() : pIndex == NULL", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (pIndex == NULL) */
    else { /* if (pIndex != NULL) */
        uint32_t  scan_loop;
        uint16_t  found_index;
        const sysflow_entry_t *pSysFlow;
        uint32_t sysflow_numlines;

        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {

            pSysFlow          = flexidag_sys_state[flexidag_slot_id].pSysFlow;
            sysflow_numlines  = flexidag_sys_state[flexidag_slot_id].sysflow_numlines;

            found_index = (uint16_t)SYSFLOW_INVALID_INDEX;
            scan_loop = 0U;

            while ((found_index == ((uint16_t)SYSFLOW_INVALID_INDEX)) && (scan_loop < (uint32_t)sysflow_numlines)) {
                const sysflow_entry_t *curr = &pSysFlow[scan_loop];
                if (curr->uuid == fd_uuid) {
                    found_index = (uint16_t)scan_loop;
                } /* if (curr->uuid == uuid) */
                scan_loop++;
            } /* while ((found_index == ((uint16_t)SYSFLOW_INVALID_INDEX)) && (scan_loop < (uint32_t)sysflow_numlines)) */

            if (found_index == ((uint16_t)SYSFLOW_INVALID_INDEX)) {
                retcode = ERR_DRV_FLEXIDAG_SYSFLOW_MISMATCHED;
            }
            *pIndex = found_index;
        } else {
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        }
    } /* if (pIndex != NULL) */

    return retcode;

} /* fd_get_sysflow_index_by_UUID() */

static uint32_t fd_default_message_send(const void *msg, uint32_t len,
                                        uint32_t dst, uint32_t frame, uint32_t flexidag_slot_id)
{
    cvmem_messagebuf_t *entry = NULL;
    uint32_t retcode = ERRCODE_NONE;

    if(msg == NULL) {
        console_printU5("[ERROR] fd_default_message_send() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_MSG_INVALID;
    } else {
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            if(flexidag_sys_state[flexidag_slot_id].pMsgpool == NULL) {
                retcode = ERR_DRV_FLEXIDAG_MSG_ENTRY_INVALID;
            } else if(len > flexidag_sys_state[flexidag_slot_id].pMsgpool->hdr_variable.message_size) {
                retcode = ERR_DRV_FLEXIDAG_MSG_SIZE_OUT_OF_RANGE;
                console_printU5("[ERROR] fd_default_message_send() : len (%d) > pFlexidag_msgpool[%d]->msg_size (%d)", len, flexidag_slot_id, flexidag_sys_state[flexidag_slot_id].pMsgpool->hdr_variable.message_size, 0U, 0U);
            } else {
                entry = fd_find_msg_entry(flexidag_sys_state[flexidag_slot_id].pMsgpool);
            }

            if(retcode == ERRCODE_NONE) {
                if (entry == NULL) {
                    retcode = ERR_DRV_FLEXIDAG_MSG_ENTRY_UNABLE_TO_FIND;
                } else {
                    if( thread_memcpy(ambacv_c2v(entry->hdr_variable.message_payload_daddr), msg, len) != ERRCODE_NONE) {
                        retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
                    } else {
                        retcode = fd_send_msg_entry(entry, dst, frame, len, flexidag_slot_id);
                    }
                }
            }
        } else {
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        }
    }

    return retcode;
}

static uint32_t fd_set_frequency_at_frameset_id(uint32_t num_total,
        uint32_t target_frameset_id,
        const uint16_t *index,
        const uint8_t *frequency, uint32_t flexidag_slot_id)
{
    uint32_t  retcode = ERRCODE_NONE;
    armvis_msg_t msg;
    uint32_t pos, j;
    const uint16_t *sysflow_ptr;
    const uint8_t *freq_ptr;

    if(index == NULL) {
        console_printU5("[ERROR] fd_set_frequency_at_frameset_id() : index == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (frequency == NULL) {
        console_printU5("[ERROR] fd_set_frequency_at_frameset_id() : frequency == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        sysflow_ptr = index;
        freq_ptr = frequency;

        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_SET_FREQ_AT_FRAME;
            msg.hdr.message_id    = (uint16_t)(0x0100U + flexidag_slot_id);

            msg.hdr.message_retcode = 0U;

            if (num_total == CVTASK_SET_FREQUENCY_ALL) {
                msg.msg.set_frequency_at_frame.num_entries = num_total;
                msg.msg.set_frequency_at_frame.target_frameset_id = target_frameset_id;
                msg.msg.set_frequency_at_frame.frequency[0] = freq_ptr[0];
                retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency_at_frame)));
                if(retcode != ERRCODE_NONE) {
                    console_printU5("[ERROR] fd_set_frequency_at_frameset_id() : schdrmsg_send_msg fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL;
                }
            } else {
                msg.msg.set_frequency_at_frame.target_frameset_id = target_frameset_id;
                for (pos = 0U; pos < num_total; pos += SET_FREQUENCY_MAX_ENTRIES) {
                    uint32_t entries = num_total - pos;
                    if (entries >= SET_FREQUENCY_MAX_ENTRIES) {
                        entries = SET_FREQUENCY_MAX_ENTRIES;
                    }
                    msg.msg.set_frequency_at_frame.num_entries = entries;
                    if( thread_memcpy(msg.msg.set_frequency_at_frame.sysflow_index, sysflow_ptr,
                                      entries * sizeof(uint16_t)) != ERRCODE_NONE) {
                        retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
                    } else {
                        for (j=0U; j < entries; j++) {
                            msg.msg.set_frequency_at_frame.frequency[j]=freq_ptr[j];
                        }
                        retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) + sizeof(msg.msg.set_frequency_at_frame)));
                        if(retcode != ERRCODE_NONE) {
                            console_printU5("[ERROR] fd_set_frequency_at_frameset_id() : schdrmsg_send_msg fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                            retcode = ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL;
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
        } else {
            retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
        }
    }
    return retcode;
}

static cvlog_perf_flexidag_metadata_t* get_next_perf_flexidag_meta_entry(schdr_log_info_t *info)
{
    cvlog_perf_flexidag_metadata_t *entry = NULL;
    const void *ptr = NULL;

    if(info == NULL) {
        console_printU5("[ERROR] get_next_perf_flexidag_meta_entry() : info == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        ptr = get_next_entry(info);
        if(ptr != NULL) {
            typecast(&entry,&ptr);
        }
    }
    return entry;
}

void fd_prof_unit_meta(uint32_t event, uint32_t sub_event, uint32_t flexidag_slot_id, uint16_t frameset_id)
{
    cvlog_perf_flexidag_metadata_t *e;
    schdr_log_info_t  *pMy_linfo;

    pMy_linfo = flexidag_sys_state[flexidag_slot_id].pLinfo[0];
    if (pMy_linfo != NULL) {
        e = get_next_perf_flexidag_meta_entry(&pMy_linfo[2]);
        if (e != NULL) {
            e->event                      = (uint8_t)event;
            e->event_subtype              = (uint8_t)sub_event;
            e->flexidag_slot_id           = (uint8_t)flexidag_slot_id;
            e->sysflow_index              = (uint8_t)(((schdr_get_cluster_id() << 4U) & 0xF0U) + (thread_get_cpu_id() & 0xFU));
            e->time_stamp                 = schdr_get_event_time();
            e->payload.u16data[0]         = frameset_id;
            e->payload.u16data[1]         = 0U;
            e->payload.u32data[1]         = 0U;
        } /* if (e != NULL) */
    } /* if (pMy_linfo != NULL) */

    pMy_linfo = schdr_sys_state.pLinfo;
    if (pMy_linfo != NULL) {
        e = get_next_perf_flexidag_meta_entry(&pMy_linfo[2]);
        if (e != NULL) {
            e->event                      = (uint8_t)event;
            e->event_subtype              = (uint8_t)sub_event;
            e->flexidag_slot_id           = (uint8_t)flexidag_slot_id;
            e->sysflow_index              = (uint8_t)(((schdr_get_cluster_id() << 4U) & 0xF0U) + (thread_get_cpu_id() & 0xFU));
            e->time_stamp                 = schdr_get_event_time();
            e->payload.u16data[0]         = frameset_id;
            e->payload.u16data[1]         = 0U;
            e->payload.u32data[1]         = 0U;
        } /* if (e != NULL) */
    } /* if (pMy_linfo != NULL) */
}

uint32_t flexidag_init_sysflow(uint32_t flexidag_slot_id, uint64_t base, uint32_t numlines)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void *ptr;

    if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
        ptr = ambacv_c2v(base);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_SYSFLOW_INVALID;
        } else {
            typecast(&flexidag_sys_state[flexidag_slot_id].pSysFlow,&ptr);
            retcode = ambacv_cache_invalidate(flexidag_sys_state[flexidag_slot_id].pSysFlow, (uint64_t)numlines * (uint64_t)sizeof(sysflow_entry_t));
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_init_sysflow() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
        }
        flexidag_sys_state[flexidag_slot_id].sysflow_numlines  = numlines;

        if(retcode == ERRCODE_NONE) {
            ambacv_cache_barrier();
            fd_print_sysflow(flexidag_slot_id);
        }
    } /* if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) */ else {
        retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
    }

    return retcode;
} /* cvtask_init_flexidag_sysflow_name() */

uint32_t flexidag_init_cvtable(uint32_t flexidag_slot_id, uint64_t base, uint32_t numlines, uint32_t disable_log)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint64_t size_align = 0UL;
    const void *ptr;

    if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
        ptr = ambacv_c2v(base);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_CVTABLE_INVALID;
        } else {
            typecast(&flexidag_sys_state[flexidag_slot_id].pCvtable,&ptr);
            flexidag_sys_state[flexidag_slot_id].cvtable_numlines = numlines;

            size_align = (uint64_t)numlines * (uint64_t)sizeof(cvtable_entry_t);
            size_align = ((size_align + ARM_CACHELINE_SIZE - 1U) & (~(ARM_CACHELINE_SIZE - 1U)));
            retcode = ambacv_cache_invalidate(flexidag_sys_state[flexidag_slot_id].pCvtable, size_align);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_init_cvtable() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }

            if(retcode == ERRCODE_NONE) {
                ambacv_cache_barrier();
                if (disable_log == 0U) {
                    fd_print_cvtable(flexidag_slot_id);
                } /* if (!disable_log) */
            }
        }
    } /* if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) */ else {
        retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
    }

    return retcode;
} /* flexidag_init_info */

uint32_t flexidag_init_system_msg(uint32_t flexidag_slot_id, uint64_t msgpool)
{
    uint32_t retcode = ERRCODE_NONE;
    const void *ptr;

    if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
        cvmem_messagepool_t *pMsgPool;
        ptr = ambacv_c2v(msgpool);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_MSG_POOL_INVALID;
        } else {
            typecast(&pMsgPool,&ptr);
            retcode = ambacv_cache_invalidate(pMsgPool, sizeof(cvmem_messagepool_t));
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_init_system_msg() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }

            if(retcode == ERRCODE_NONE) {
                ambacv_cache_barrier();
                flexidag_sys_state[flexidag_slot_id].pMsgpool = pMsgPool;

                if (schdr_sys_cfg.verbosity >= LVL_VERBOSE) {
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"##################################################### \n", 0U, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"                Flexidag[%3d] message pool \n", flexidag_slot_id, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"##################################################### \n", 0U, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"      write_index: %d \n",   pMsgPool->hdr_variable.wr_index, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"       queue_size: %d \n",   pMsgPool->hdr_variable.message_num, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"     message_size: %d \n",   pMsgPool->hdr_variable.message_size, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"      buffer_size: %d \n",   pMsgPool->hdr_variable.message_buffersize, 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"        pool_base: 0x%X \n", ambacv_c2p(pMsgPool->hdr_variable.messagepool_basedaddr), 0U, 0U, 0U, 0U);
                    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID," \n", 0U, 0U, 0U, 0U, 0U);
                } /* if (schdr_sys_cfg.verbosity >= LVL_VERBOSE) */
            }
        }
    } /* if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS]) */ else {
        retcode = ERR_DRV_FLEXIDAG_SLOT_OUT_OF_RANGE;
    }

    return retcode;
} /* flexidag_init_system_msg() */

static uint32_t flexidag_log_init_linfo(const schedmsg_flexidag_init_partitioned_t *pInitMsg, uint32_t  flexidag_slot_id, uint32_t  core_id)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  ret = ERRCODE_NONE;
    const cvlog_buffer_info_t    *pCVLogBuf;
    schdr_log_info_t  *pMy_linfo;
    uint64_t base_daddr = 0UL;
    const void *ptr;

    ptr = ambacv_c2v(pInitMsg->arm_linfo_daddr[core_id]);
    if(ptr == NULL) {
        retcode = ERR_DRV_FLEXIDAG_LINFO_INVALID;
    } else {
        typecast(&pMy_linfo,&ptr);
        flexidag_sys_state[flexidag_slot_id].pLinfo[core_id] = pMy_linfo;
        ret |= thread_memset(&pMy_linfo[0], 0, sizeof(schdr_log_info_t));
        ret |= thread_memset(&pMy_linfo[1], 0, sizeof(schdr_log_info_t));
        ret |= thread_memset(&pMy_linfo[2], 0, sizeof(schdr_log_info_t));
        if(ret != ERRCODE_NONE) {
            retcode = ERR_DRV_FLEXIDAG_MEMSET_FAIL;
        } else {
            base_daddr = pInitMsg->arm_cvtask_log_ctrl_daddr[core_id];
            if (base_daddr != 0U) {
                ptr = ambacv_c2v(base_daddr);
                if(ptr == NULL) {
                    retcode = ERR_DRV_FLEXIDAG_ARM_CVTASK_LOG_INVALID;
                } else {
                    typecast(&pCVLogBuf,&ptr);
                    pMy_linfo[0].base_daddr = (base_daddr + pCVLogBuf->buffer_addr_offset);
                    pMy_linfo[0].wptr_daddr = (base_daddr + 12U);
                    pMy_linfo[0].mask       = pCVLogBuf->buffer_size_in_entries - 1U;
                    pMy_linfo[0].esize      = pCVLogBuf->entry_size;
                    if(pMy_linfo[0].wptr_daddr != 0U) {
                        ptr = ambacv_c2v(pMy_linfo[0].wptr_daddr);
                        typecast(&pMy_linfo[0].wptr,&ptr);
                    } else {
                        pMy_linfo[0].wptr       = NULL;
                    }
                    if(pMy_linfo[0].base_daddr != 0U) {
                        ptr = ambacv_c2v(pMy_linfo[0].base_daddr);
                        typecast(&pMy_linfo[0].base,&ptr);
                    } else {
                        pMy_linfo[0].base       = NULL;
                    }
                    *pMy_linfo[0].wptr      = 0U;
                }
            } /* if (pInitMsg->arm_cvtask_log_ctrl_daddr[core_id] != 0) */
            base_daddr = pInitMsg->arm_sched_log_ctrl_daddr[core_id];
            if (base_daddr != 0U) {
                ptr = ambacv_c2v(base_daddr);
                if(ptr == NULL) {
                    retcode = ERR_DRV_FLEXIDAG_ARM_SCHDR_LOG_INVALID;
                } else {
                    typecast(&pCVLogBuf,&ptr);
                    pMy_linfo[1].base_daddr = (base_daddr + pCVLogBuf->buffer_addr_offset);
                    pMy_linfo[1].wptr_daddr = (base_daddr + 12U);
                    pMy_linfo[1].mask       = pCVLogBuf->buffer_size_in_entries - 1U;
                    pMy_linfo[1].esize      = pCVLogBuf->entry_size;
                    if(pMy_linfo[1].wptr_daddr != 0U) {
                        ptr = ambacv_c2v(pMy_linfo[1].wptr_daddr);
                        typecast(&pMy_linfo[1].wptr,&ptr);
                    } else {
                        pMy_linfo[1].wptr       = NULL;
                    }
                    if(pMy_linfo[1].base_daddr != 0U) {
                        ptr = ambacv_c2v(pMy_linfo[1].base_daddr);
                        typecast(&pMy_linfo[1].base,&ptr);
                    } else {
                        pMy_linfo[1].base       = NULL;
                    }
                    *pMy_linfo[1].wptr      = 0U;
                }
            } /* if (pInitMsg->arm_sched_log_ctrl_daddr[core_id] != 0) */
            base_daddr = pInitMsg->arm_perf_log_ctrl_daddr[core_id];
            if (base_daddr != 0U) {
                ptr = ambacv_c2v(base_daddr);
                if(ptr == NULL) {
                    retcode = ERR_DRV_FLEXIDAG_ARM_PERF_INVALID;
                } else {
                    typecast(&pCVLogBuf,&ptr);
                    pMy_linfo[2].base_daddr = (base_daddr + pCVLogBuf->buffer_addr_offset);
                    pMy_linfo[2].wptr_daddr = (base_daddr + 12U);
                    pMy_linfo[2].mask       = pCVLogBuf->buffer_size_in_entries - 1U;
                    pMy_linfo[2].esize      = pCVLogBuf->entry_size;
                    if(pMy_linfo[2].wptr_daddr != 0U) {
                        ptr = ambacv_c2v(pMy_linfo[2].wptr_daddr);
                        typecast(&pMy_linfo[2].wptr,&ptr);
                    } else {
                        pMy_linfo[2].wptr       = NULL;
                    }
                    if(pMy_linfo[2].base_daddr != 0U) {
                        ptr = ambacv_c2v(pMy_linfo[2].base_daddr);
                        typecast(&pMy_linfo[2].base,&ptr);
                    } else {
                        pMy_linfo[2].base       = NULL;
                    }
                    *pMy_linfo[2].wptr      = 0U;
                }
            } /* if (pInitMsg->arm_perf_log_ctrl_daddr[core_id] != 0) */
        }
    }

    return retcode;
}

uint32_t flexidag_log_init(const armvis_msg_t *pMsg)
{
    uint32_t  retcode = ERRCODE_NONE;

    (void) pMsg;
    if(pMsg == NULL) {
        console_printU5("[ERROR] flexidag_log_init() : pMsg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_VISMSG_INVALID;
    } else {
        const schedmsg_flexidag_init_partitioned_t *pInitMsg;
        uint32_t  flexidag_slot_id;
        uint32_t  core_id;

        pInitMsg          = &pMsg->msg.flexidag_init_partitioned;
        flexidag_slot_id  = ((uint32_t)pMsg->hdr.message_id - (uint32_t)0x0100U);
        core_id           = 0U; /* TODO: Make this work with multi ARM core */
        if (pInitMsg->arm_linfo_daddr[core_id] != 0U) {
            retcode = flexidag_log_init_linfo(pInitMsg, flexidag_slot_id, core_id);
        } /* if (pMsg->flexidag_init_partitioned.arm_linfo_daddr[core_id] != 0) */
        else { /* if (pMsg->flexidag_init_partitioned.arm_linfo_daddr[core_id] == 0) */
            flexidag_sys_state[flexidag_slot_id].pLinfo[core_id] = NULL;
        } /* if (pMsg->flexidag_init_partitioned.arm_linfo_daddr[core_id] == 0) */
    } /* if (is_associated_flexidag(pMsg) != 0) */

    return retcode;

} /* flexidag_log_init() */

int32_t flexidag_system_init(uint32_t version, uint32_t num_slots)
{
    uint32_t  loop;
    uint32_t retcode = ERRCODE_NONE;

    (void)version;
    if (num_slots > FLEXIDAG_MAX_SLOTS) {
        schdr_sys_state.visorc_init.flexidag_num_slots  = 0U;
        console_printU5("[ERROR] flexidag_system_init() : num_slots(%d) > FLEXIDAG_MAX_SLOTS(%d)", num_slots, FLEXIDAG_MAX_SLOTS, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FLEXIDAG_SLOT_OUT_OF_RANGE;
    } else {
        schdr_sys_state.visorc_init.hotlink_region_daddr  = 0U; /* This will be set by the driver later*/
        schdr_sys_state.visorc_init.hotlink_region_size   = num_slots * FLEXIDAG_SLOT_SIZE;
        schdr_sys_state.visorc_init.flexidag_num_slots    = num_slots;
        schdr_sys_state.visorc_init.flexidag_slot_size    = FLEXIDAG_SLOT_SIZE;
        retcode = drv_flexidag_enable(num_slots);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] flexidag_system_init() : drv_flexidag_enable fail ret (0x%x)", retcode, 0U, 0U, 0U, 0U);
        }
    }

    if(retcode == ERRCODE_NONE) {
        for (loop = 0U; loop < FLEXIDAG_MAX_SLOTS; loop++) {
            retcode = flexidag_system_reset_slot(loop);
            if(retcode != ERRCODE_NONE) {
                break;
            }
        } /* for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) */
    }

    if(retcode == ERRCODE_NONE) {
        schdr_sys_state.module_init = schdr_sys_state.module_init | FLEXIDAG_MODULE;
    }
    return (int32_t)retcode;
} /* flexidag_system_init() */

uint32_t flexidag_system_resume(uint32_t version)
{
    uint32_t  loop;
    uint32_t retcode = ERRCODE_NONE;

    (void)version;
    for (loop = 0U; loop < FLEXIDAG_MAX_SLOTS; loop++) {
        retcode = flexidag_system_reset_slot(loop);
        if(retcode != ERRCODE_NONE) {
            break;
        }
    } /* for (loop = 0; loop < FLEXIDAG_MAX_SLOTS; loop++) */

    if(retcode == ERRCODE_NONE) {
        schdr_sys_state.module_init = schdr_sys_state.module_init | FLEXIDAG_MODULE;
    }
    return retcode;

}

uint32_t flexidag_system_reset_slot(uint32_t flexidag_slot_id)
{
    uint32_t retcode = ERRCODE_NONE;
    static uint32_t flexidag_sys_sem_init[FLEXIDAG_MAX_SLOTS] = {0U};

    retcode = thread_memset(&flexidag_sys_state[flexidag_slot_id], 0, sizeof(flexidag_system_state_t));
    if(retcode == ERRCODE_NONE) {
        if(flexidag_sys_sem_init[flexidag_slot_id] == 0U) {
            retcode = thread_sem_init(&flexidag_sys_sem[flexidag_slot_id], 0, 0U);
            flexidag_sys_sem_init[flexidag_slot_id] = 1U;
        }
    }
    return retcode;
}

/*-= FlexiDag API function prototypes =-------------------------------------------------*/
uint32_t  is_associated_flexidag(const armvis_msg_t *pMsg)
{
    uint32_t retval = 0U;

    (void) pMsg;
    if(pMsg == NULL) {
        console_printU5("[ERROR] is_associated_flexidag() : pMsg == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (pMsg->hdr.message_type < SCHEDMSG_BLOCK_FLEXIDAG_START) {
            retval = 0U;
        } /* if (pMsg->hdr.message_type < SCHEDMSG_BLOCK_FLEXIDAG_START) */
        else if (pMsg->hdr.message_type > SCHEDMSG_BLOCK_FLEXIDAG_END) {
            retval = 0U;
        } /* if (pMsg->hdr.message_type > SCHEDMSG_BLOCK_FLEXIDAG_END) */
        else if ((pMsg->hdr.message_id >= (uint16_t)0x0100U) && (pMsg->hdr.message_id < (uint16_t)(0x100U + FLEXIDAG_MAX_SLOTS))) {
            uint32_t  flexidag_slot_id;

            flexidag_slot_id = ((uint32_t)pMsg->hdr.message_id - (uint32_t)0x0100U);

            if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
                retval = 0U;
            } /* if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) */
            else if (flexidag_sys_state[flexidag_slot_id].pHandle == NULL) {
                retval = 0U;
            } /* if (flexidag_sys_state[flexidag_slot_id].pHandle != NULL) */
            else { /* if ((flexidag_slot_id < FLEXIDAG_MAX_SLOTS) && (flexidag_sys_state[flexidag_slot_id].pHandle != NULL)) */
                retval = 1U;
            } /* if ((flexidag_slot_id < FLEXIDAG_MAX_SLOTS) && (vpFlexidagHandle[flexidag_slot_id] != NULL)) */
        } /* if ((pMsg->hdr.message_id >= 0x0100) && (pMsg->hdr.message_id < (0x100 + FLEXIDAG_MAX_SLOTS))) */
        else { /* if ((pMsg->hdr.message_id < 0x0100) || (pMsg->hdr.message_id >= (0x100 + FLEXIDAG_MAX_SLOTS))) */
            retval = 0U;
        } /* if ((pMsg->hdr.message_id < 0x0100) || (pMsg->hdr.message_id >= (0x100 + FLEXIDAG_MAX_SLOTS))) */
    }
    return retval;
} /* is_associated_flexidag() */

uint32_t  flexidag_find_slot_by_vphandle(void *vpHandle)
{
    uint32_t  loop = 0U;
    uint32_t  retval = FLEXIDAG_INVALID_SLOT_ID;

    (void)vpHandle;
    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_find_slot_by_vphandle() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        thread_unused(vpHandle);
        while ((loop < FLEXIDAG_MAX_SLOTS) && (retval == FLEXIDAG_INVALID_SLOT_ID)) {
            if (vpHandle == flexidag_sys_state[loop].pHandle) {
                retval = loop;
            } /* if (vpHandle == flexidag_sys_state[loop].pHandle) */
            else { /* if (vpHandle != flexidag_sys_state[loop].pHandle) */
                loop++;
            } /* if (vpHandle != flexidag_sys_state[loop].pHandle) */
        } /* while ((loop < FLEXIDAG_MAX_SLOTS) && (retval == FLEXIDAG_INVALID_SLOT_ID)) */
    }
    return retval;
} /* flexidag_find_slot_by_vphandle() */

errcode_enum_t  flexidag_create(void **vppHandle)
{
    uint32_t  slot_id;
    void     *vpHandle;
    uint32_t  retcode = ERRCODE_NONE;

    if(vppHandle == NULL) {
        console_printU5("[ERROR] flexidag_create() :  vppHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        vpHandle  = NULL;
        slot_id   = FLEXIDAG_INVALID_SLOT_ID;
        retcode    = drv_flexidag_create(&vpHandle, &slot_id);
        if(retcode != 0U) {
            console_printU5("[ERROR] flexidag_create() :  drv_flexidag_create fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        } else {
            retcode = flexidag_system_reset_slot(slot_id);
            if(retcode == ERRCODE_NONE) {
                flexidag_sys_state[slot_id].pHandle = vpHandle;
                *vppHandle  = vpHandle;
            }
        }
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_scheduler_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_create() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_FLEXIDAG_CREATE_FAIL;
    }
    return retcode;
} /* flexidag_create() */

errcode_enum_t  flexidag_set_parameter(void *vpHandle, uint32_t parameter_id, uint32_t parameter_value)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_set_parameter(vpHandle, parameter_id, parameter_value);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_set_parameter() :  drv_flexidag_set_parameter fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_parameter() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_parameter() */

errcode_enum_t  flexidag_add_tbar(void *vpHandle, flexidag_memblk_t *pblk_tbar)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_add_tbar(vpHandle, pblk_tbar);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_add_tbar() :  drv_flexidag_add_tbar fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_add_tbar() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_add_tbar() */

errcode_enum_t  flexidag_add_sfb(void *vpHandle, flexidag_memblk_t *pblk_sfb)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_add_sfb(vpHandle, pblk_sfb);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_add_sfb() :  drv_flexidag_add_sfb fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_add_sfb() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_add_sfb() */

errcode_enum_t  flexidag_open(void *vpHandle, flexidag_memblk_t *pblk_FlexiBin, flexidag_memory_requirements_t *pFlexiRequirements)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
    if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
    } else {
        fd_prof_unit_meta(EVENT_FLEXIDAG_METADATA_START, EVENT_ST_FLEXIDAG_OPEN, flexidag_slot_id, 0U);
        retcode = drv_flexidag_open(vpHandle, pblk_FlexiBin, pFlexiRequirements);
        if(retcode == ERRCODE_NONE) {
            if(thread_sem_wait(&flexidag_sys_sem[flexidag_slot_id], FLEXIDAG_WAIT_REPLY_TIMEOUT) != 0U) {
                retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_FAIL;
            }
        }
    }

    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_open() :  drv_flexidag_open fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    return retcode;
} /* flexidag_open() */

errcode_enum_t  flexidag_set_state_buffer(void *vpHandle, flexidag_memblk_t *pblk_Buffer)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_set_state_buffer(vpHandle, pblk_Buffer);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_set_state_buffer() :  drv_flexidag_set_state_buffer fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_state_buffer() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_state_buffer() */

errcode_enum_t  flexidag_set_temp_buffer(void *vpHandle, flexidag_memblk_t *pblk_Buffer)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_set_temp_buffer(vpHandle, pblk_Buffer);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_set_temp_buffer() :  drv_flexidag_set_temp_buffer fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_temp_buffer() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_temp_buffer() */

errcode_enum_t  flexidag_set_output_callback(void *vpHandle, uint32_t output_num, flexidag_cb output_callback, void *vpParameter)
{
    uint32_t  retcode = ERRCODE_NONE;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_set_output_callback() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (output_num >= FLEXIDAG_MAX_OUTPUTS) {
        retcode = ERR_DRV_FLEXIDAG_OUTPUT_NUM_OUT_OF_RANGE;
    } /* if (output_num >= FLEXIDAG_MAX_OUTPUTS) */
    else { /* if (output_num < FLEXIDAG_MAX_OUTPUTS) */
        uint32_t  flexidag_slot_id;

        flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            flexidag_sys_state[flexidag_slot_id].callback_array[output_num] = output_callback;
            flexidag_sys_state[flexidag_slot_id].pCallbackParam[output_num] = vpParameter;
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    } /* if (output_num < FLEXIDAG_MAX_OUTPUTS) */

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_output_callback() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_output_callback */

errcode_enum_t  flexidag_set_error_callback(void *vpHandle, flexidag_error_cb error_callback, void *vpParameter)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_set_error_callback() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            flexidag_sys_state[flexidag_slot_id].error_callback = error_callback;
            flexidag_sys_state[flexidag_slot_id].pErrorCallbackParam = vpParameter;
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_error_callback() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_error_callback() */

errcode_enum_t  flexidag_set_coredump_callback(void *vpHandle, uint32_t cvcore_type, flexidag_error_cb error_callback, void *vpParameter)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_set_coredump_callback() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            if (cvcore_type == (uint32_t)CVCORE_VP0) {
                flexidag_sys_state[flexidag_slot_id].vpcoredump_callback = error_callback;
                flexidag_sys_state[flexidag_slot_id].pVPCoredumpCallbackParam = vpParameter;
            } /* if (cvcore_type == CVCORE_VP0) */
            else { /* if (cvcore_type != CVCORE_VP0) */
                retcode = ERR_DRV_FLEXIDAG_VISCORE_UNAVAILABLE;
            } /* if (cvcore_type != CVCORE_VP0) */
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_coredump_callback() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
}

errcode_enum_t  flexidag_init(void *vpHandle)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
    if (flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
        retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
    } else {
        fd_prof_unit_meta(EVENT_FLEXIDAG_METADATA_START, EVENT_ST_FLEXIDAG_INIT, flexidag_slot_id, 0U);
        retcode = drv_flexidag_init(vpHandle);
        if(retcode == ERRCODE_NONE) {
            if(thread_sem_wait(&flexidag_sys_sem[flexidag_slot_id], FLEXIDAG_WAIT_REPLY_TIMEOUT) != 0U) {
                retcode = ERR_DRV_FLEXIDAG_SEM_WAIT_FAIL;
            }
        }
    }
    return retcode;
} /* flexidag_init() */

errcode_enum_t  flexidag_prep_run(void *vpHandle)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_prep_run() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            retcode = drv_flexidag_prep_run(vpHandle, NULL); /* Use pTokenId for future expansion if needed */
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_prep_run() : drv_flexidag_prep_run fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_prep_run() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_coredump_callback() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_prep_run() */

errcode_enum_t  flexidag_set_input_buffer(void *vpHandle, uint32_t input_num, flexidag_memblk_t *pblk_Buffer)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_set_input_buffer(vpHandle, input_num, pblk_Buffer);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_set_input_buffer() :  drv_flexidag_set_input_buffer fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_input_buffer() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_input_buffer() */

errcode_enum_t  flexidag_set_feedback_buffer(void *vpHandle, uint32_t feedback_num, flexidag_memblk_t *pblk_Buffer)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_set_feedback_buffer(vpHandle, feedback_num, pblk_Buffer);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_set_feedback_buffer() :  drv_flexidag_set_feedback_buffer fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_feedback_buffer() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_feedback_buffer() */

errcode_enum_t  flexidag_set_output_buffer(void *vpHandle, uint32_t output_num, flexidag_memblk_t *pblk_Buffer)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_set_output_buffer(vpHandle, output_num, pblk_Buffer);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_set_output_buffer() :  flexidag_set_output_buffer fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_output_buffer() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_output_buffer() */

errcode_enum_t  flexidag_get_sysflow_index_list(void *vpHandle,
        const char  *instance, const char *algorithm, const char *step, const char *cvtask,
        uint16_t *index_array, uint32_t array_size, uint32_t *num_found)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_list() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (index_array == NULL) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_list() :  index_array == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (num_found == NULL) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_list() :  num_found == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            retcode = fd_get_sysflow_index_list(instance, algorithm, step, cvtask, index_array, array_size, num_found, flexidag_slot_id);
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_get_sysflow_index_list() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_list() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_get_sysflow_index_list() */

errcode_enum_t  flexidag_get_sysflow_index_by_UUID(void *vpHandle, uint32_t flow_uuid, uint16_t *pIndex)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_by_UUID() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (pIndex == NULL) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_by_UUID() :  pIndex == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            retcode = fd_get_sysflow_index_by_UUID(flow_uuid, pIndex, flexidag_slot_id);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_get_sysflow_index_by_UUID() : fd_get_sysflow_index_by_UUID fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_get_sysflow_index_by_UUID() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_get_sysflow_index_by_UUID() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_get_sysflow_index_by_UUID() */

errcode_enum_t  flexidag_set_frequency(void *vpHandle, uint32_t num_entries, uint16_t *pIndexArray, uint8_t *pFreqArray)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    (void) pIndexArray;
    (void) pFreqArray;
    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_set_frequency() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (pIndexArray == NULL) {
        console_printU5("[ERROR] flexidag_set_frequency() :  pIndexArray == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (pFreqArray == NULL) {
        console_printU5("[ERROR] flexidag_set_frequency() :  pFreqArray == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        thread_unused(pIndexArray);
        thread_unused(pFreqArray);
        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            retcode = fd_set_frequency_at_frameset_id(num_entries, flexidag_sys_state[flexidag_slot_id].curr_frameset_id, pIndexArray, pFreqArray, flexidag_slot_id);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_set_frequency() : fd_set_frequency_at_frameset_id fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_set_frequency() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_frequency() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_frequency() */

errcode_enum_t  flexidag_set_debug_level(void *vpHandle, uint16_t target_index, uint16_t dbg_lvl)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_set_debug_level() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            armvis_msg_t msg;
            msg.hdr.message_type  = (uint16_t)SCHEDMSG_FLEXIDAG_SET_DEBUG_LEVEL;
            msg.hdr.message_id    = (uint16_t)(0x0100U + flexidag_slot_id);
            msg.hdr.message_retcode = 0U;
            if (target_index == SYSFLOW_INVALID_INDEX) {
                msg.msg.set_debug_level.num_entries = CVTASK_SET_DEBUG_LEVEL_ALL;
            } /* if (target_index == SYSFLOW_INVALID_INDEX) */
            else { /* if (target_index != SYSFLOW_INVALID_INDEX) */
                msg.msg.set_debug_level.num_entries = 1;
            } /* if (target_index != SYSFLOW_INVALID_INDEX) */
            msg.msg.set_debug_level.target_sysflow_index[0] = target_index;
            msg.msg.set_debug_level.target_debug_level[0]   = dbg_lvl;
            retcode = schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg)));
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_set_debug_level() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_set_debug_level() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_set_debug_level() */

errcode_enum_t  flexidag_send_message(void *vpHandle, uint16_t target_index, void *vpMessage, uint32_t length)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    (void) vpMessage;
    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_send_message() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (vpMessage == NULL) {
        console_printU5("[ERROR] flexidag_send_message() :  vpMessage == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        thread_unused(vpMessage);
        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);

        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            retcode = fd_default_message_send(vpMessage, length, (uint32_t)target_index, flexidag_sys_state[flexidag_slot_id].curr_frameset_id, flexidag_slot_id);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_send_message() : fd_default_message_send fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_send_message() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_send_message() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_send_message() */

errcode_enum_t  flexidag_send_private_msg(void *vpHandle, uint16_t target_index, const uint8_t *pMessage)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode;

    if (vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_send_private_msg() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } /* if (vpHandle == NULL) */
    else if (pMessage == NULL) {
        console_printU5("[ERROR] flexidag_send_private_msg() :  pMessage == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (pMessage == NULL) */
    else { /* if ((vpMessage != NULL) && (pMessage != NULL)) */
        retcode = ERRCODE_NONE;

        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);

        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            armvis_msg_t msg;

            msg.hdr.message_type    = (uint16_t)SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG;
            msg.hdr.message_id      = (uint16_t)(0x0100U + flexidag_slot_id);
            msg.hdr.message_retcode = ERRCODE_NONE;

            msg.msg.sched_send_private_msg.target_sysflow_index = target_index;
            msg.msg.sched_send_private_msg.reserved0            = 0U;
            msg.msg.sched_send_private_msg.reserved1            = 0U;
            msg.msg.sched_send_private_msg.reserved2            = 0U;

            if (thread_memcpy(&msg.msg.sched_send_private_msg.message, pMessage, sizeof(msg.msg.sched_send_private_msg.message)) != ERRCODE_NONE) {
                retcode = ERR_DRV_FLEXIDAG_MEMCPY_FAIL;
            } else {
                if (schdrmsg_send_msg(&msg, (uint32_t)(sizeof(msg.hdr) +  sizeof(msg.msg.reply_cvtask_run_partial))) != ERRCODE_NONE) {
                    retcode = ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL;
                }
            } /* if (retcode == ERRCODE_NONE) : thread_memcpy() */
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_send_private_msg() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    } /* if ((vpMessage != NULL) && (pMessage != NULL)) */

    return retcode;

} /* flexidag_send_private_msg() */

errcode_enum_t  flexidag_cvtable_find(void *vpHandle, const char *target, const void **ptr, uint32_t *size)
{
    uint32_t  flexidag_slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_cvtable_find() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (target == NULL) {
        console_printU5("[ERROR] flexidag_cvtable_find() :  target == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (ptr == NULL) {
        console_printU5("[ERROR] flexidag_cvtable_find() :  ptr == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else if (size == NULL) {
        console_printU5("[ERROR] flexidag_cvtable_find() :  size == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            retcode = fd_cvtable_find(target, ptr, size, flexidag_slot_id);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_cvtable_find() : fd_cvtable_find fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_cvtable_find() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_cvtable_find() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
}

errcode_enum_t  flexidag_run(void *vpHandle, flexidag_runinfo_t *pRunInfo)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_run() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (pRunInfo == NULL) {
        console_printU5("[ERROR] flexidag_run() :  pRunInfo == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            fd_prof_unit_meta(EVENT_FLEXIDAG_METADATA_START, EVENT_ST_FLEXIDAG_RUN, flexidag_slot_id,flexidag_sys_state[flexidag_slot_id].curr_frameset_id);
            retcode = drv_flexidag_run(vpHandle, pRunInfo);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_run() : drv_flexidag_run fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
            flexidag_sys_state[flexidag_slot_id].curr_frameset_id ++;
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_run() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_run() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;

} /* flexidag_run() */

errcode_enum_t  flexidag_run_noblock(void *vpHandle, uint32_t *pToken_id)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  flexidag_slot_id;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_run_noblock() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else if (pToken_id == NULL) {
        console_printU5("[ERROR] flexidag_run_noblock() :  pToken_id == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        flexidag_slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (flexidag_slot_id < FLEXIDAG_MAX_SLOTS) {
            fd_prof_unit_meta(EVENT_FLEXIDAG_METADATA_START, EVENT_ST_FLEXIDAG_RUN, flexidag_slot_id,flexidag_sys_state[flexidag_slot_id].curr_frameset_id);
            retcode = drv_flexidag_run_noblock(vpHandle, pToken_id);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_run_noblock() : drv_flexidag_run_noblock fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
            flexidag_sys_state[flexidag_slot_id].curr_frameset_id ++;
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            console_printU5("[ERROR] flexidag_run_noblock() : Invalid vpHandle\n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_run_noblock() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_run_noblock() */

errcode_enum_t  flexidag_wait_run_finish(void *vpHandle, uint32_t token_id, flexidag_runinfo_t *pRunInfo)
{
    uint32_t  retcode = ERRCODE_NONE;

    retcode = drv_flexidag_wait_run_finish(vpHandle, token_id, pRunInfo);
    if(retcode != 0U) {
        console_printU5("[ERROR] flexidag_wait_run_finish() :  drv_flexidag_wait_run_finish fail ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_wait_run_finish() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_wait_run_finish() */

errcode_enum_t  flexidag_close(void *vpHandle)
{
    uint32_t  slot_id;
    uint32_t  retcode = ERRCODE_NONE;

    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_clos() : vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_HANDLE_INVALID;
    } else {
        slot_id = flexidag_find_slot_by_vphandle(vpHandle);
        if (slot_id < FLEXIDAG_MAX_SLOTS) {
            fd_prof_unit_meta(EVENT_FLEXIDAG_METADATA_START, EVENT_ST_FLEXIDAG_CLOSE, slot_id, 0U);
            retcode = drv_flexidag_close(vpHandle);
            if(retcode != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_close() : drv_flexidag_close fail retcode = 0x%x \n",retcode, 0U, 0U, 0U, 0U);
            }
        } /* if (slot_id != FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (slot_id == FLEXIDAG_INVALID_SLOT_ID) */
            retcode = ERR_DRV_FLEXIDAG_HANDLE_UNAVAILABLE;
            console_printU5("[ERROR] flexidag_close() : Invalid vpHandle \n", 0U, 0U, 0U, 0U, 0U);
        } /* if (slot_id == FLEXIDAG_INVALID_SLOT_ID) */
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_close() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_close() */

errcode_enum_t  flexidag_show_info(void *vpHandle)
{
    return drv_flexidag_show_info(vpHandle);
}

static uint32_t flexidag_log_system_visorc_code(flexidag_system_state_t *pFlexiSysState, const schedmsg_flexidag_init_partitioned_t *pInitPart, uint32_t core_loop)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  size_align = 0U;
    const void* ptr;

    if (pInitPart->visorc_codebase[core_loop] != 0U) {
        ptr = ambacv_c2v(pInitPart->visorc_codebase[core_loop]);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_CODE_BASE_INVALID;
        } else {
            typecast(&pFlexiSysState->pVisorcBase[core_loop],&ptr);
            size_align = ((pInitPart->visorc_codesize[core_loop] + ARM_CACHELINE_SIZE - 1U) & (~(ARM_CACHELINE_SIZE - 1U)));
            retcode = ambacv_cache_invalidate(pFlexiSysState->pVisorcBase[core_loop], size_align);
            if( retcode != ERRCODE_NONE ) {
                console_printU5("[ERROR] flexidag_log_system_visorc_code() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            ambacv_cache_barrier();
        }
    } /* if (pInitPart->visorc_codebase[core_loop] != 0) */
    else { /* if (pInitPart->visorc_codebase[core_loop] == 0) */
        pFlexiSysState->pVisorcBase[core_loop] = NULL;
    } /* if (pInitPart->visorc_codebase[core_loop] == 0) */

    return retcode;
}

static uint32_t flexidag_log_system_visorc_log(flexidag_system_state_t *pFlexiSysState, const schedmsg_flexidag_init_partitioned_t *pInitPart, uint32_t core_loop)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void* ptr;

    if (pInitPart->visorc_perf_log_ctrl_daddr[core_loop] != 0U) {
        ptr = ambacv_c2v(pInitPart->visorc_perf_log_ctrl_daddr[core_loop]);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_VISORC_PERF_INVALID;
        } else {
            typecast(&pFlexiSysState->pVisPerfLogCtrl[core_loop],&ptr);
            retcode = ambacv_cache_invalidate(pFlexiSysState->pVisPerfLogCtrl[core_loop], sizeof(*pFlexiSysState->pVisPerfLogCtrl[core_loop]));
            if( retcode != ERRCODE_NONE ) {
                console_printU5("[ERROR] flexidag_log_system_visorc_log() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            ambacv_cache_barrier();
        }
    } /* if (pInitPart->visorc_perf_log_ctrl_daddr[core_loop] != 0) */

    if(retcode == ERRCODE_NONE) {
        if (pInitPart->visorc_sched_log_ctrl_daddr[core_loop] != 0U) {
            ptr = ambacv_c2v(pInitPart->visorc_sched_log_ctrl_daddr[core_loop]);
            if(ptr == NULL) {
                retcode = ERR_DRV_FLEXIDAG_VISORC_SCHDR_LOG_INVALID;
            } else {
                typecast(&pFlexiSysState->pVisSchedLogCtrl[core_loop],&ptr);
                retcode = ambacv_cache_invalidate(pFlexiSysState->pVisSchedLogCtrl[core_loop], sizeof(*pFlexiSysState->pVisSchedLogCtrl[core_loop]));
                if( retcode != ERRCODE_NONE ) {
                    console_printU5("[ERROR] flexidag_log_system_visorc_log() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
                ambacv_cache_barrier();
            }
        } /* if (pInitPart->visorc_sched_log_ctrl_daddr[core_loop] != 0) */
    }

    if(retcode == ERRCODE_NONE) {
        if (pInitPart->visorc_cvtask_log_ctrl_daddr[core_loop] != 0U) {
            ptr = ambacv_c2v(pInitPart->visorc_cvtask_log_ctrl_daddr[core_loop]);
            if(ptr == NULL) {
                retcode = ERR_DRV_FLEXIDAG_VISORC_CVTASK_LOG_INVALID;
            } else {
                typecast(&pFlexiSysState->pVisCVTaskLogCtrl[core_loop],&ptr);
                retcode = ambacv_cache_invalidate(pFlexiSysState->pVisCVTaskLogCtrl[core_loop], sizeof(*pFlexiSysState->pVisCVTaskLogCtrl[core_loop]));
                if( retcode != ERRCODE_NONE ) {
                    console_printU5("[ERROR] flexidag_log_system_visorc_log() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
                ambacv_cache_barrier();
            }
        } /* if (pInitPart->visorc_cvtask_log_ctrl_daddr[core_loop] != 0) */
    }

    return retcode;
}

static uint32_t flexidag_log_system_visorc_init(flexidag_system_state_t *pFlexiSysState, const schedmsg_flexidag_init_partitioned_t *pInitPart)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  core_loop;

    for (core_loop = 0U; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++) {
        retcode = flexidag_log_system_visorc_log(pFlexiSysState, pInitPart, core_loop);
        if(retcode == ERRCODE_NONE) {
            retcode = flexidag_log_system_visorc_code(pFlexiSysState, pInitPart, core_loop);
        }
        if(retcode != ERRCODE_NONE) {
            break;
        }
    } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_VISORC; core_loop++) */

    return retcode;
}

static uint32_t flexidag_log_system_arm_log(flexidag_system_state_t *pFlexiSysState, const schedmsg_flexidag_init_partitioned_t *pInitPart, uint32_t core_loop)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void* ptr;

    if (pInitPart->arm_perf_log_ctrl_daddr[core_loop] != 0U) {
        ptr = ambacv_c2v(pInitPart->arm_perf_log_ctrl_daddr[core_loop]);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_ARM_PERF_INVALID;
        } else {
            typecast(&pFlexiSysState->pArmPerfLogCtrl[core_loop],&ptr);
            retcode = ambacv_cache_invalidate(pFlexiSysState->pArmPerfLogCtrl[core_loop], sizeof(*pFlexiSysState->pArmPerfLogCtrl[core_loop]));
            if( retcode != ERRCODE_NONE ) {
                console_printU5("[ERROR] flexidag_log_system_arm_init() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            ambacv_cache_barrier();
        }
    } /* if (pInitPart->arm_perf_log_ctrl_daddr[core_loop] != 0) */
    if (retcode == ERRCODE_NONE) {
        if (pInitPart->arm_sched_log_ctrl_daddr[core_loop] != 0U) {
            ptr = ambacv_c2v(pInitPart->arm_sched_log_ctrl_daddr[core_loop]);
            if(ptr == NULL) {
                retcode = ERR_DRV_FLEXIDAG_ARM_SCHDR_LOG_INVALID;
            } else {
                typecast(&pFlexiSysState->pArmSchedLogCtrl[core_loop],&ptr);
                retcode = ambacv_cache_invalidate(pFlexiSysState->pArmSchedLogCtrl[core_loop], sizeof(*pFlexiSysState->pArmSchedLogCtrl[core_loop]));
                if( retcode != ERRCODE_NONE ) {
                    console_printU5("[ERROR] flexidag_log_system_arm_init() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
                ambacv_cache_barrier();
            }
        } /* if (pInitPart->arm_sched_log_ctrl_daddr[core_loop] != 0) */
    }
    if (retcode == ERRCODE_NONE) {
        if (pInitPart->arm_cvtask_log_ctrl_daddr[core_loop] != 0U) {
            ptr = ambacv_c2v(pInitPart->arm_cvtask_log_ctrl_daddr[core_loop]);
            if(ptr == NULL) {
                retcode = ERR_DRV_FLEXIDAG_ARM_CVTASK_LOG_INVALID;
            } else {
                typecast(&pFlexiSysState->pArmCVTaskLogCtrl[core_loop],&ptr);
                retcode = ambacv_cache_invalidate(pFlexiSysState->pArmCVTaskLogCtrl[core_loop], sizeof(*pFlexiSysState->pArmCVTaskLogCtrl[core_loop]));
                if( retcode != ERRCODE_NONE ) {
                    console_printU5("[ERROR] flexidag_log_system_arm_init() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
                ambacv_cache_barrier();
            }
        } /* if (pInitPart->arm_cvtask_log_ctrl_daddr[core_loop] != 0) */
    }

    return retcode;
}

static uint32_t flexidag_log_system_arm_linfo(flexidag_system_state_t *pFlexiSysState, const schedmsg_flexidag_init_partitioned_t *pInitPart, uint32_t core_loop)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void* ptr;

    if (pInitPart->arm_linfo_daddr[core_loop] != 0U) {
        ptr = ambacv_c2v(pInitPart->arm_linfo_daddr[core_loop]);
        if(ptr == NULL) {
            retcode = ERR_DRV_FLEXIDAG_LINFO_INVALID;
        } else {
            typecast(&pFlexiSysState->pLinfo[core_loop],&ptr);
        }
    } else {
        pFlexiSysState->pLinfo[core_loop] = NULL;
    }

    return retcode;
}

static uint32_t flexidag_log_system_arm_init(flexidag_system_state_t *pFlexiSysState, const schedmsg_flexidag_init_partitioned_t *pInitPart)
{
    uint32_t  retcode = ERRCODE_NONE;
    uint32_t  core_loop;

    for (core_loop = 0U; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++) {
        retcode = flexidag_log_system_arm_log(pFlexiSysState, pInitPart, core_loop);
        if (retcode == ERRCODE_NONE) {
            retcode = flexidag_log_system_arm_linfo(pFlexiSysState, pInitPart, core_loop);
        }
        if(retcode != ERRCODE_NONE) {
            break;
        }
    } /* for (core_loop = 0; core_loop < SYSTEM_MAX_NUM_CORTEX; core_loop++) */

    return retcode;
}

uint32_t flexidag_log_system_init(const armvis_msg_t *pMsg)
{
    uint32_t  retcode = ERRCODE_NONE;
    flexidag_system_state_t *pFlexiSysState;
    const schedmsg_flexidag_init_partitioned_t *pInitPart;
    uint32_t  flexidag_slot_id;
    uint32_t  size_align = 0U;
    void* ptr;

    (void) pMsg;
    if(pMsg == NULL) {
        console_printU5("[ERROR] flexidag_log_system_init() :  pMsg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_VISMSG_INVALID;
    } else {
        flexidag_slot_id  = ((uint32_t)pMsg->hdr.message_id - (uint32_t)0x0100U);
        pInitPart         = &pMsg->msg.flexidag_init_partitioned;
        pFlexiSysState    = &flexidag_sys_state[flexidag_slot_id];

        retcode = flexidag_log_system_visorc_init(pFlexiSysState, pInitPart);
        if(retcode == ERRCODE_NONE) {
            if (pInitPart->hotlink_blockbase != 0U) {
                pFlexiSysState->pHotlinkBase = ambacv_c2v(pInitPart->hotlink_blockbase);
            } else {
                pFlexiSysState->pHotlinkBase = NULL;
            }
            size_align = ((pInitPart->flexidag_blocksize + ARM_CACHELINE_SIZE - 1U) & (~(ARM_CACHELINE_SIZE - 1U)));
            ptr = ambacv_c2v(pInitPart->flexidag_blockbase);
            if(ptr == NULL) {
                retcode = ERR_DRV_FLEXIDAG_BLOCK_BASE_INVALID;
            } else {
                retcode = ambacv_cache_invalidate(ptr, size_align);
                if( retcode != ERRCODE_NONE ) {
                    console_printU5("[ERROR] flexidag_log_system_init() : ambacv_cache_invalidate fail ret = 0x%x \n", retcode, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
                ambacv_cache_barrier();
            }
            if(retcode == ERRCODE_NONE) {
                retcode = flexidag_log_system_arm_init(pFlexiSysState, pInitPart);
            }
        }
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_log_system_init() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_log_init() */

uint32_t flexidag_handle_open_parsed(uint32_t flexidag_slot_id, const schedmsg_flexidag_open_parsed_t *pOpenParsed)
{
    uint32_t  retcode = ERRCODE_NONE;
    const void* ptr;

    (void) pOpenParsed;
    if(pOpenParsed == NULL) {
        console_printU5("[ERROR] flexidag_handle_open_parsed() :  pOpenParsed == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } else {
        if(pOpenParsed->flexidag_name_daddr != 0U) {
            ptr = ambacv_c2v(pOpenParsed->flexidag_name_daddr);
            typecast(&flexidag_sys_state[flexidag_slot_id].pName,&ptr);
        } else {
            flexidag_sys_state[flexidag_slot_id].pName    = NULL;
        }
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_handle_open_parsed() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
} /* flexidag_handle_open_parsed() */

uint32_t flexidag_handle_reg(const armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    armvis_msg_t rpl = {0};

    (void) msg;
    if(msg == NULL) {
        console_printU5("[ERROR] flexidag_handle_reg() : msg == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_VISMSG_INVALID;
    } else {
        rpl.hdr.message_type    = msg->hdr.message_type | 0x8000U;
        rpl.hdr.message_id      = msg->hdr.message_id;
        rpl.hdr.message_retcode = (uint32_t)ERRCODE_NONE;
        rpl.msg.flexidag_setup_reply.flexidag_token = msg->msg.flexidag_setup_request.flexidag_token;
        rpl.msg.flexidag_setup_reply.num_cvtasks    = cvtask_get_total_class();
        retcode = schdrmsg_send_msg(&rpl, (uint32_t)(sizeof(rpl.hdr) + sizeof(rpl.msg.flexidag_setup_reply)));
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] flexidag_handle_reg() : schdrmsg_send_msg fail retcode (0x%x)", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_VISMSG_SEND_FAIL;
        }
    }

    if( (retcode != ERRCODE_NONE) && (is_drv_flexidag_err(retcode) == 0U)) {
        console_printU5("[ERROR] flexidag_handle_reg() : error ret = 0x%x", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_BASE;
    }
    return retcode;
}

/* Taken from profiler.c */
typedef struct {
    uint32_t  flexidag_slot_id;
    flexidag_system_state_t *pFlexiSysState;
    cvlog_buffer_info_t  *pLogBufInfo;
    uint32_t  log_to_console;
    uint32_t  prepend_header;
    uint32_t  full_flush;
    uint32_t  core_id;
    uint32_t  core_type;
    uint32_t  log_type;
    uint32_t  valid_run;
    uint32_t  *pRdIdx;
    uint32_t  *pWrIdx;
    uint32_t  entry_size;
    uint32_t  num_entries;
    char     *pLogRawBase;
} flexidag_dump_param_t;

typedef struct {
    char     *pOutBuf;
    uint32_t  curr_rdidx;
    uint32_t  curr_wridx;
    uint32_t  done;
    uint32_t  buffer_left;
} flexidag_dump_t;

static void flexidag_dump_log_parse1(uint32_t log_flags, flexidag_dump_param_t *param)
{
    if((log_flags & FLEXILOG_ECHO_TO_CONSOLE) != 0U) {
        param->log_to_console = 1U;
    } else {
        param->log_to_console = 0U;
    }

    if((log_flags & FLEXILOG_PREPEND_HEADER) != 0U) {
        param->prepend_header = 1U;
    } else {
        param->prepend_header = 0U;
    }

    if((log_flags & FLEXILOG_FULL_FLUSH) != 0U) {
        param->full_flush = 1U;
    } else {
        param->full_flush = 0U;
    }

    param->valid_run       = 1U;
    switch (log_flags & FLEXILOG_COREMASK) {
    case FLEXILOG_CORE0:
        param->core_id = 0U;
        break;
    case FLEXILOG_CORE1:
        param->core_id = 1U;
        break;
    case FLEXILOG_CORE2:
        param->core_id = 2U;
        break;
    case FLEXILOG_CORE3:
        param->core_id = 3U;
        break;
    default:
        param->valid_run = 0U;
        param->core_id = 0U;
        break;
    } /* switch (log_flags & FLEXILOG_COREMASK) */
}

static void flexidag_dump_log_parse2(uint32_t log_flags, flexidag_dump_param_t *param)
{
    switch (log_flags & FLEXILOG_TYPEMASK) {
    case FLEXILOG_VIS_PERF:
        param->core_type   = 0U;
        param->log_type    = CVLOG_TYPE_PERF;
        param->pRdIdx      = &param->pFlexiSysState->vis_perf_rdidx[param->core_id];
        param->pWrIdx      = &param->pFlexiSysState->pVisPerfLogCtrl[param->core_id]->write_index;
        param->pLogBufInfo = param->pFlexiSysState->pVisPerfLogCtrl[param->core_id];
        break;
    case FLEXILOG_VIS_SCHED:
        param->core_type   = 0U;
        param->log_type    = CVLOG_TYPE_SCHED;
        param->pRdIdx      = &param->pFlexiSysState->vis_sched_rdidx[param->core_id];
        param->pWrIdx      = &param->pFlexiSysState->pVisSchedLogCtrl[param->core_id]->write_index;
        param->pLogBufInfo = param->pFlexiSysState->pVisSchedLogCtrl[param->core_id];
        break;
    case FLEXILOG_VIS_CVTASK:
        param->core_type   = 0U;
        param->log_type    = CVLOG_TYPE_CVTASK;
        param->pRdIdx      = &param->pFlexiSysState->vis_cvtask_rdidx[param->core_id];
        param->pWrIdx      = &param->pFlexiSysState->pVisCVTaskLogCtrl[param->core_id]->write_index;
        param->pLogBufInfo = param->pFlexiSysState->pVisCVTaskLogCtrl[param->core_id];
        break;
    case FLEXILOG_ARM_PERF:
        param->core_type   = 1U;
        param->log_type    = CVLOG_TYPE_PERF;
        param->pRdIdx      = &param->pFlexiSysState->arm_perf_rdidx[param->core_id];
        param->pWrIdx      = &param->pFlexiSysState->pLinfo[param->core_id][2].wridx;
        param->pLogBufInfo = param->pFlexiSysState->pArmPerfLogCtrl[param->core_id];
        break;
    case FLEXILOG_ARM_SCHED:
        param->core_type   = 1U;
        param->log_type    = CVLOG_TYPE_SCHED;
        param->pRdIdx      = &param->pFlexiSysState->arm_sched_rdidx[param->core_id];
        param->pWrIdx      = &param->pFlexiSysState->pLinfo[param->core_id][1].wridx;
        param->pLogBufInfo = param->pFlexiSysState->pArmSchedLogCtrl[param->core_id];
        break;
    case FLEXILOG_ARM_CVTASK:
        param->core_type   = 1U;
        param->log_type    = CVLOG_TYPE_CVTASK;
        param->pRdIdx      = &param->pFlexiSysState->arm_cvtask_rdidx[param->core_id];
        param->pWrIdx      = &param->pFlexiSysState->pLinfo[param->core_id][0].wridx;
        param->pLogBufInfo = param->pFlexiSysState->pArmCVTaskLogCtrl[param->core_id];
        break;
    default:
        param->valid_run = 0U;
        break;
    } /* switch (log_flags & FLEXILOG_TYPEMASK) */
}

static uint32_t flexidag_dump_log_invalidate(const flexidag_dump_param_t *param, flexidag_dump_t *dump)
{
    uint32_t retcode = ERRCODE_NONE;

    (void) param;
    /*-= Invalidate relevant portions of the log buffer =---------------------------------------*/
    if (param->full_flush != 0U) {
        uint32_t  invalidate_size = 0U;
        dump->curr_rdidx  = 0U;
        dump->curr_wridx  = 0U;
        dump->done        = 0U;
        invalidate_size = ((param->num_entries * param->entry_size) + (ARM_CACHELINE_SIZE - 1U)) & (~(ARM_CACHELINE_SIZE - 1U));
        if( ambacv_cache_invalidate(&param->pLogRawBase[0], invalidate_size) != retcode) {
            console_printU5("[ERROR] flexidag_dump_log_invalidate() : ambacv_cache_invalidate fail", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
        }
        ambacv_cache_barrier();
    } /* if (full_flush != 0) */
    else { /* if (full_flush == 0) */
        dump->curr_rdidx  = (param->pRdIdx[0] % param->num_entries);
        dump->curr_wridx  = (param->pWrIdx[0] % param->num_entries);
        if(dump->curr_rdidx == dump->curr_wridx) {
            dump->done = 1U;
        } else {
            dump->done = 0U;
        }
        if (dump->done == 0U) {
            uint32_t  invalidate_size = 0U;
            uint32_t  alignment = 0U;
            uint32_t  log_entry_base = 0U;

            if (dump->curr_wridx > dump->curr_rdidx) { /* Invalidate rdidx -> wridx */
                invalidate_size = (dump->curr_wridx - dump->curr_rdidx) * param->entry_size;
            } else { /* invalidate rdidx -> end, beginning -> wridx */
                invalidate_size   = ((dump->curr_wridx * param->entry_size) + (ARM_CACHELINE_SIZE - 1U)) & (~(ARM_CACHELINE_SIZE - 1U));
                if( ambacv_cache_invalidate(&param->pLogRawBase[0], invalidate_size) != retcode) {
                    console_printU5("[ERROR] flexidag_dump_log_invalidate() : ambacv_cache_invalidate fail", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                }
                ambacv_cache_barrier();
                invalidate_size   = (param->num_entries - dump->curr_rdidx) * param->entry_size;
            }
            log_entry_base    = (dump->curr_rdidx * param->entry_size);
            alignment         = log_entry_base & (ARM_CACHELINE_SIZE - 1U);
            log_entry_base   -= alignment;
            invalidate_size   = (invalidate_size + alignment + (ARM_CACHELINE_SIZE - 1U)) & (~(ARM_CACHELINE_SIZE - 1U));
            if( ambacv_cache_invalidate(&param->pLogRawBase[log_entry_base], invalidate_size) != retcode) {
                console_printU5("[ERROR] flexidag_dump_log_invalidate() : ambacv_cache_invalidate fail", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
            }
            ambacv_cache_barrier();
        } /* if (done == 0) */
    } /* if (full_flush == 0) */

    return retcode;
}

static uint32_t flexidag_dump_log_perf(const flexidag_dump_param_t *param, flexidag_dump_t *dump)
{
    uint32_t retval = 0U;
    uint32_t ret = 0U;
    uint32_t copy_size = 0U;
    const sysflow_entry_t *pSysFlow;
    ambaprof_header_t   prof_header;
    ambaprof_section_t  prof_section[2];
    uint32_t  sysflow_numlines;
    char    prof_name_entry[AMBAPROF_MAX_NAME_LEN];
    ambaprof_flexidag_header_t  flexidag_section = {0};
    const char *dest_ptr = NULL;

    (void) param;
    if(param->entry_size != 0U) {
        if(param->prepend_header != 0U) {
            sysflow_numlines  = flexidag_sys_state[param->flexidag_slot_id].sysflow_numlines;
            pSysFlow          = flexidag_sys_state[param->flexidag_slot_id].pSysFlow;

            copy_size   = (uint32_t)sizeof(prof_header);
            copy_size  += (uint32_t)sizeof(prof_section);
            copy_size  += (NAME_MAX_LENGTH + (((uint32_t)sizeof(prof_name_entry)) * sysflow_numlines));

            if (copy_size <= (dump->buffer_left)) {
                uint32_t  sysflow_loop;
                prof_header.version       = 2U;
                prof_header.magic_num     = 0xBABEFACEU;
                prof_header.section_count = 2U;
                prof_header.padding       = 0U;
                ret = thread_memcpy(&dump->pOutBuf[0], &prof_header, sizeof(prof_header));
                dump->pOutBuf = &dump->pOutBuf[sizeof(prof_header)];

                ret |= thread_memset(prof_section, 0, sizeof(prof_section));
                dest_ptr = thread_strcpy(prof_section[0].name, "flexidag names");
                prof_section[0].size = ((uint32_t)sizeof(flexidag_section)) + (sysflow_numlines * (uint32_t)sizeof(prof_name_entry));
                prof_section[0].base = (uint32_t)(sizeof(prof_header) + sizeof(prof_section));

                dest_ptr = thread_strcpy(prof_section[1].name, "profile events");
                prof_section[1].base  = prof_section[0].base + prof_section[0].size;
                prof_section[1].size  = 0U;
                ret |= thread_memcpy(&dump->pOutBuf[0], &prof_section[0], sizeof(prof_section));
                dump->pOutBuf = &dump->pOutBuf[sizeof(prof_section)];

                ret |= thread_memcpy(&flexidag_section.flexidag_name[0], &flexidag_sys_state[param->flexidag_slot_id].pName[0], sizeof(flexidag_section.flexidag_name));
                flexidag_section.flexidag_slot_id             = param->flexidag_slot_id;
                flexidag_section.flexidag_num_sysflow_entries = sysflow_numlines;
                ret |= thread_memset(&flexidag_section.reserved_padding[0], 0, sizeof(flexidag_section.reserved_padding));
                ret |= thread_memcpy(&dump->pOutBuf[0], &flexidag_section, sizeof(flexidag_section));
                dump->pOutBuf = &dump->pOutBuf[sizeof(flexidag_section)];

                for (sysflow_loop = 0U; sysflow_loop < sysflow_numlines; sysflow_loop++) {
                    uint32_t   numwritten = 0U;

                    ret |= thread_memset(&prof_name_entry[0], 0, sizeof(prof_name_entry));
                    numwritten = snprintf_str1(&prof_name_entry[0], (uint32_t)sizeof(prof_name_entry), "%s", pSysFlow[sysflow_loop].cvtask_name);
                    numwritten += snprintf_uint1(&prof_name_entry[numwritten], (uint32_t)sizeof(prof_name_entry), "(%d)", pSysFlow[sysflow_loop].uuid);
                    (void) numwritten;
                    ret |= thread_memcpy(&dump->pOutBuf[0], &prof_name_entry[0], sizeof(prof_name_entry));
                    dump->pOutBuf = &dump->pOutBuf[sizeof(prof_name_entry)];
                } /* for (sysflow_loop = 0; sysflow_loop < sysflow_numlines; sysflow_loop++) */

                dump->buffer_left  -= copy_size;
                retval             += copy_size;
            } else {
                dump->buffer_left = 0U; /* Cannot dump out */
            }
        }

        if (param->full_flush != 0U) {
            copy_size = param->num_entries * param->entry_size;
            if (copy_size > dump->buffer_left) {
                copy_size = (dump->buffer_left / param->entry_size) * param->entry_size;
            }
            ret |= thread_memcpy(&dump->pOutBuf[0], &param->pLogRawBase[0], copy_size);
            dump->buffer_left  -= copy_size;
            retval             += copy_size;
            dump->pOutBuf      = &dump->pOutBuf[copy_size];
        } /* if (full_flush != 0) */
        else { /* if (full_flush == 0) */
            if (dump->curr_wridx > dump->curr_rdidx) { /* Dump wridx->rdidx */
                copy_size = (dump->curr_wridx - dump->curr_rdidx) * param->entry_size;
                if (copy_size > dump->buffer_left) {
                    copy_size = (dump->buffer_left / param->entry_size) * param->entry_size;
                }
                ret |= thread_memcpy(&dump->pOutBuf[0], &param->pLogRawBase[dump->curr_rdidx * param->entry_size], copy_size);
                dump->buffer_left  -= copy_size;
                retval             += copy_size;
                dump->pOutBuf      = &dump->pOutBuf[copy_size];
                dump->curr_rdidx   = (dump->curr_rdidx + (copy_size / param->entry_size)) % param->num_entries;
            } else { /* invalidate rdidx -> end, beginning -> wridx */
                copy_size = (param->num_entries - dump->curr_rdidx) * param->entry_size;
                if (copy_size > dump->buffer_left) {
                    copy_size = (dump->buffer_left / param->entry_size) * param->entry_size;
                }
                ret |= thread_memcpy(&dump->pOutBuf[0], &param->pLogRawBase[dump->curr_rdidx * param->entry_size], copy_size);
                dump->buffer_left  -= copy_size;
                retval             += copy_size;
                dump->pOutBuf      = &dump->pOutBuf[copy_size];
                dump->curr_rdidx   = (dump->curr_rdidx + (copy_size / param->entry_size)) % param->num_entries;
                if (dump->buffer_left >= param->entry_size) {
                    copy_size = dump->curr_wridx * param->entry_size;
                    if (copy_size > dump->buffer_left) {
                        copy_size = (dump->buffer_left / param->entry_size) * param->entry_size;
                    }
                    ret |= thread_memcpy(&dump->pOutBuf[0], &param->pLogRawBase[0], copy_size);
                    dump->buffer_left  -= copy_size;
                    retval             += copy_size;
                    dump->pOutBuf      = &dump->pOutBuf[copy_size];
                    dump->curr_rdidx   = (dump->curr_rdidx + (copy_size / param->entry_size)) % param->num_entries;
                } /* if (buffer_left >= entry_size) */
            }
        } /* if (full_flush == 0) */

        if(ret != 0U) {
            console_printU5("[ERROR] flexidag_dump_log_perf() :  thread_memcpy or  thread_memset fail ", 0U, 0U, 0U, 0U, 0U);
            retval = 0U;
        }
    }

    (void) dest_ptr;
    return retval;
}

static uint32_t flexidag_dump_log_string_header(const flexidag_dump_param_t *param, flexidag_dump_t *dump)
{
    uint32_t retval = 0U;
    static const char fdcorestring[2][8] = { "VISORC", "ARM" };
    static const char fdtypestring[3][8] = { "CVTASK", "SCHED", "PERF" };
    char  stringbuf[256]; /* Temporary string buffer, expand if needed */
    uint32_t   numwritten = 0U;

    (void) param;
    if (param->prepend_header != 0U) {
        numwritten += snprintf_str1(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "======================= %s", fdcorestring[param->core_type]);
        numwritten += snprintf_uint1(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "[%d]", param->core_id);
        numwritten += snprintf_str1(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "%s LOG ", fdtypestring[param->log_type]);
        numwritten += snprintf_uint1(&stringbuf[numwritten], ((uint32_t)sizeof(stringbuf) - numwritten), "total entry = %d ======================= \n", param->pLogBufInfo->buffer_size_in_entries);
        if (param->log_to_console != 0U) {
            console_printU5(&stringbuf[0], 0U, 0U, 0U, 0U, 0U);
        } /* if (log_to_console != 0) */
        if (dump->pOutBuf != NULL) {
            if (((uint32_t)numwritten) > (dump->buffer_left)) {
                dump->done = 1U;
            } /* if (((uint32_t)numwritten) > buffer_left) */
            else { /* if (((uint32_t)numwritten) <= buffer_left) */
                if( thread_memcpy(dump->pOutBuf, stringbuf, numwritten) != ERRCODE_NONE) {
                    console_printU5("[ERROR] flexidag_dump_log_string_header() :  thread_memcpy fail ", 0U, 0U, 0U, 0U, 0U);
                } else {
                    dump->pOutBuf            = &dump->pOutBuf[numwritten];
                    retval             += numwritten;
                    dump->buffer_left  -= numwritten;
                }
            } /* if (((uint32_t)numwritten) <= buffer_left) */
        } /* if (dump->pOutBuf != NULL) */
    } /* if ((done == 0) && (prepend_header != 0)) */

    return retval;
}

static uint32_t flexidag_dump_log_string_body(const flexidag_dump_param_t *param, flexidag_dump_t *dump, const char *formatbase, const cvlog_sched_entry_t *pEntry)
{
    uint32_t retval = 0U;
    const char *outstring;
    char  stringbuf[256]; /* Temporary string buffer, expand if needed */
    char  prefixbuf[32];
    uint32_t   numwritten_prefix = 0U;
    uint32_t   numwritten = 0U;

    (void) param;
    outstring   = &formatbase[(pEntry->entry_string_offset & 0x1FFFFFFFU)];
    numwritten_prefix = snprintf_uint3(&prefixbuf[0], (uint32_t)sizeof(prefixbuf), "[%05d] <TH-%d:%u> :", dump->curr_rdidx, pEntry->hdr.entry_flags.src_thread, (uint32_t)pEntry->entry_time);
    numwritten        = snprintf_uint5(&stringbuf[0], (uint32_t)sizeof(stringbuf), outstring, pEntry->entry_arg1, pEntry->entry_arg2, pEntry->entry_arg3, pEntry->entry_arg4, pEntry->entry_arg5);
    numwritten       += sprintf_str(&stringbuf[numwritten], "\n");
    if (param->log_to_console != 0U) {
        if (numwritten_prefix >= sizeof(prefixbuf)) {
            prefixbuf[sizeof(prefixbuf)-1U] = '\0';
        } /* if (numwritten_prefix >= sizeof(prefixbuf)) */
        else { /* if (numwritten_prefix < sizeof(prefixbuf)) */
            prefixbuf[numwritten_prefix] = '\0';
        } /* if (numwritten_prefix < sizeof(prefixbuf)) */
        if (numwritten >= sizeof(stringbuf)) {
            stringbuf[sizeof(stringbuf)-1U] = '\0';
        } /* if (numwritten_prefix >= sizeof(prefixbuf)) */
        else { /* if (numwritten_prefix < sizeof(prefixbuf)) */
            stringbuf[numwritten] = '\0';
        } /* if (numwritten_prefix < sizeof(prefixbuf)) */

        console_printS5("%s%s", prefixbuf, stringbuf, NULL, NULL, NULL);
    } /* if (log_to_console != 0) */

    if (dump->pOutBuf != NULL) {
        if (((uint32_t)(numwritten + numwritten_prefix)) > dump->buffer_left) {
            dump->done = 1U;
        } /* if (((uint32_t)(numwritten + numwritten_prefix)) > buffer_left) */
        else { /* if (((uint32_t)(numwritten + numwritten_prefix)) <= buffer_left) */
            if( thread_memcpy(dump->pOutBuf, prefixbuf, numwritten_prefix) != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_dump_log_string_body() :  thread_memcpy fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                dump->pOutBuf      = &dump->pOutBuf[numwritten_prefix];
                retval             += numwritten_prefix;
                dump->buffer_left  -= numwritten_prefix;
            }

            if( thread_memcpy(dump->pOutBuf, stringbuf, numwritten) != ERRCODE_NONE) {
                console_printU5("[ERROR] flexidag_dump_log_string_body() :  thread_memcpy fail ", 0U, 0U, 0U, 0U, 0U);
            } else {
                dump->pOutBuf      = &dump->pOutBuf[numwritten];
                retval             += numwritten;
                dump->buffer_left  -= numwritten;
            }
        } /* if ((numwritten + numwritten_prefix) <= buffer_left) */
    } /* if (dump->pOutBuf != NULL) */

    return retval;
}

static uint32_t flexidag_dump_log_string(const flexidag_dump_param_t *param, flexidag_dump_t *dump)
{
    uint32_t retval = 0U;

    retval += flexidag_dump_log_string_header(param, dump);
    while (dump->done == 0U) {
        const cvlog_sched_entry_t *pEntry;
        const char *log_base = &param->pLogRawBase[dump->curr_rdidx * param->entry_size];

        typecast(&pEntry, &log_base);
        if (pEntry->entry_string_offset != 0U) {
            const char *formatbase;

            if ((pEntry->entry_string_offset >> 29U) == 0U) {
                if (param->core_type == 0U) {
                    typecast(&formatbase, &param->pFlexiSysState->pVisorcBase[param->core_id]);
                } else {
#ifndef DISABLE_ARM_CVTASK
                    const char *paddr1,*paddr2;

                    paddr1 = &__cvtask_format_start;
                    paddr2 = &__cvtask_format_end;
                    if(paddr1 != paddr2) {
                        formatbase = paddr1;
                    } else {
                        formatbase = NULL;
                    }
#else
                    formatbase = NULL;
#endif
                }
            } else if ((pEntry->entry_string_offset >> 29U) == 1U) {
                typecast(&formatbase, &param->pFlexiSysState->pHotlinkBase);
            } else {
                formatbase = NULL;
            }

            if (formatbase != NULL) {
                retval += flexidag_dump_log_string_body(param, dump, formatbase, pEntry);
            } /* if (formatbase != NULL) */
        }

        // when not done, don't update rdidx since next call to this api need to copy this line buffer again
        if(dump->done == 0U) {
            dump->curr_rdidx++;
        }

        if (dump->curr_rdidx >= param->num_entries) {
            dump->curr_rdidx = 0U;
        }
        if (dump->curr_rdidx == dump->curr_wridx) {
            dump->done = 1U;
        }
    } /* while (done == 0) */

    return retval;
}

uint32_t  flexidag_dump_log(void *vpHandle, void *vpBuffer, uint32_t buffer_size, uint32_t log_flags)
{
    uint32_t  retval = 0U;
    uint32_t retcode = ERRCODE_NONE;
    flexidag_dump_param_t param;

    (void) vpBuffer;
    if(vpHandle == NULL) {
        console_printU5("[ERROR] flexidag_dump_log() :  vpHandle == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        param.flexidag_slot_id  = flexidag_find_slot_by_vphandle(vpHandle);
        if (param.flexidag_slot_id >= FLEXIDAG_MAX_SLOTS) {
            console_printU5("[ERROR] flexidag_dump_log() : Invalid vpHandle for this scheduler instance\n", 0U, 0U, 0U, 0U, 0U);
        } /* if (flexidag_slot_id == FLEXIDAG_INVALID_SLOT_ID) */
        else { /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
            param.pFlexiSysState  = &flexidag_sys_state[param.flexidag_slot_id];
            flexidag_dump_log_parse1(log_flags, &param);
            flexidag_dump_log_parse2(log_flags, &param);
            if (param.valid_run == 0U) {
                console_printU5("[ERROR] flexidag_dump_log() : unsupported log_flags state (0x%08x)\n", log_flags, 0U, 0U, 0U, 0U);
            } /* if (valid_run == 0) */
            else if ( (param.pLogBufInfo != NULL) && (param.pLogBufInfo->entry_size != 0U) && (param.pLogBufInfo->buffer_size_in_entries != 0U)) {
                char     *pLogInfoBase;
                flexidag_dump_t dump;

                if(ambacv_cache_invalidate(param.pWrIdx, sizeof(uint32_t)) != ERRCODE_NONE) { /* Invalidate pWrIdx, as it can be updated externally */
                    console_printU5("[ERROR] flexidag_dump_log() : ambacv_cache_invalidate fail", 0U, 0U, 0U, 0U, 0U);
                    retcode = ERR_DRV_FLEXIDAG_CACHE_INVALID_FAIL;
                } else {
                    param.num_entries   = param.pLogBufInfo->buffer_size_in_entries;
                    param.entry_size    = param.pLogBufInfo->entry_size;
                    typecast(&pLogInfoBase, &param.pLogBufInfo);
                    param.pLogRawBase   = &pLogInfoBase[param.pLogBufInfo->buffer_addr_offset]; /* ASSUMES ALWAYS ALIGNED TO CACHE LINE */
                    dump.buffer_left   = buffer_size;
                    typecast(&dump.pOutBuf, &vpBuffer);

                    retcode = flexidag_dump_log_invalidate(&param, &dump);
                    if ((retcode == ERRCODE_NONE) && (dump.done == 0U)) {
                        /*-= Specialized path for PERF logging dump =---------------------------------------------*/
                        if (param.log_type == (uint32_t)CVLOG_TYPE_PERF) {
                            if (param.log_to_console != 0U) {
                                console_printU5("flexidag_dump_log() : Ignoring console print request for CVLOG_TYPE_PERF\n", 0U, 0U, 0U, 0U, 0U);
                            } /* if (log_to_console != 0) */
                            if (dump.pOutBuf != NULL) {
                                retval = flexidag_dump_log_perf(&param, &dump);
                            } /* if (pOutBuf != NULL) */
                        } /* if (log_type == CVLOG_TYPE_PERF) */
                        else { /* if (log_type != CVLOG_TYPE_PERF) */
                            retval = flexidag_dump_log_string(&param, &dump);
                        } /* if (log_type != CVLOG_TYPE_PERF) */
                        if (param.full_flush == 0U) {
                            param.pRdIdx[0]   = dump.curr_rdidx;
                        } /* if (full_flush == 0)*/
                    } /* if (done == 0) */
                }
            } /* if (pLogBufInfo != NULL) */
            else { /* if ((valid_run != 0) && (pLogBufInfo == NULL)) */
                console_printU5("flexidag_dump_log() : ignoring log dump for (lf=0x%08x) - no log buffer exists\n", log_flags, 0U, 0U, 0U, 0U);
            } /* if ((valid_run != 0) && (pLogBufInfo == NULL)) */
        } /* if (flexidag_slot_id != FLEXIDAG_INVALID_SLOT_ID) */
    }
    (void) retcode;
    return retval;

} /* flexidag_dump_log() */

errcode_enum_t  flexibin_metadata_find(
    IN  void *pFlexiBin,
    IN  const char *pMetadataName,
    OUT const void **vppBuffer,
    OUT uint32_t *pSize)
{
    uint32_t  retcode = ERRCODE_NONE;

    (void)pFlexiBin;
    if (pFlexiBin == NULL) {
        console_printU5("[ERROR] flexibin_metadata_find() : pFlexiBin == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (pFlexiBin == NULL) */
    else if (pMetadataName == NULL) {
        console_printU5("[ERROR] flexibin_metadata_find() : pMetadataName == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (pMetadataName == NULL) */
    else if (vppBuffer == NULL) {
        console_printU5("[ERROR] flexibin_metadata_find() : vppBuffer == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (vppBuffer == NULL) */
    else if (pSize == NULL) {
        console_printU5("[ERROR] flexibin_metadata_find() : pSize == NULL ", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_FLEXIDAG_INPUT_PARAM_INVALID;
    } /* if (pSize == NULL) */
    else { /* if ((pFlexiBin != NULL) && (pMetadataName != NULL) && (vppBuffer != NULL) && (pSize != NULL)) */
        const uint8_t  *pFlexiBinU8;
        const flexibin_metadata_component_t *pMetaComponent;
        uint32_t  num_entries;
        uint32_t  loop;
        uint32_t  found;

        typecast(&pMetaComponent, &pFlexiBin);
        typecast(&pFlexiBinU8, &pFlexiBin);

        num_entries = pMetaComponent->hdr.size / ((uint32_t)sizeof(flexibin_component_t));
        loop        = 1U;
        found       = 0U;

        pMetaComponent++;

        while ((loop < num_entries) && (found == 0U)) {
            if (pMetaComponent->hdr.identifier == FLEXIBIN_ID_METADATA) {
                if (thread_strcmp(&pMetadataName[0], &pMetaComponent->metadata_name[0]) == 0) {
                    const void *outptr;
                    const uint8_t *srcptr;
                    outptr  = NULL;
                    found   = 1U;
                    srcptr  = &pFlexiBinU8[pMetaComponent->hdr.offset];
                    typecast(&outptr, &srcptr);
                    *vppBuffer  = outptr;
                    *pSize      = pMetaComponent->hdr.size;
                }
            }
            pMetaComponent++;
            loop++;
        } /* while ((loop < num_entries) && (found == 0)) */

        if (found == 0U) {
            retcode     = ERR_DRV_FLEXIDAG_METADATA_UNABLE_TO_FIND;
            *vppBuffer  = NULL;
            *pSize      = 0U;
        } /* if (found == 0) */

    } /* if ((pFlexiBin != NULL) && (pMetadataName != NULL) && (vppBuffer != NULL) && (pSize != NULL)) */

    return retcode;

} /* flexibin_metadata_find() */

