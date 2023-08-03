/**
 *  @file cache.c
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
 *  @details Cache Control APIs
 *
 */

#include "os_api.h"
#include "dram_sync.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cavalry.h"
#include "cvtask_errno.h"
#include "cvapi_logger_interface.h"
#include "cvapi_idspfeeder_interface.h"
#include "cvapi_cavalry.h" /* for cavalry_run_dags structure */
#include <cvsched_sysflow_format.h>   /* for sysflow_entry_t */

static uint32_t                 local_log_info_daddr;
static uint32_t                 host_logformat_addr;
static uint32_t                 host_logformat_size;

static void sync_logger_buffer(char *_base, int32_t sync_format_string,
                               cvlog_buffer_info_t *info)
{
    char *base;
    uint32_t wi_prev, wi_curr, e_size;

    base = _base + info->buffer_addr_offset;
    e_size  = info->entry_size;
    if (sync_format_string == 0) {
        wi_prev = info->prev_write_index;
        wi_curr = info->write_index;
    } else {
        wi_prev = 0;
        wi_curr = info->buffer_size_in_entries;
    }

    /* sync the tail part if there is a wrap-around */
    if (wi_prev > wi_curr) {
        int32_t pos = wi_prev * e_size;
        int32_t len = (info->buffer_size_in_entries - wi_prev) * e_size;
        ambacv_cache_invalidate(base + pos, len);
        ambacv_cache_barrier();
        wi_prev = 0;
    }

    /* sync the head part if the buffer is not empty */
    if (wi_prev != wi_curr) {
        int32_t pos = wi_prev * e_size;
        int32_t len = (wi_curr - wi_prev) * e_size;
        ambacv_cache_invalidate(base + pos, len);
        ambacv_cache_barrier();
    }

    /* sync the format string */
    if (sync_format_string != 0) {
        base = _base + (int32_t)info->binary_offset;
        ambacv_cache_invalidate(base, info->binary_size);
        ambacv_cache_barrier();
    }
}

static int32_t invalidate_logger_state(uint32_t addr, uint32_t sync)
{
    cvlogger_state_t *state = (cvlogger_state_t*)ambacv_c2v(addr);
    uint32_t i;

    ambacv_cache_invalidate(state, sizeof(*state));
    ambacv_cache_barrier();
    if (state->orcsod.perf_log.buffer_addr_offset != state->orcvp.perf_log.buffer_addr_offset) {
        sync_logger_buffer((char*)state, sync, &state->orcsod.perf_log);
    }
    if (state->orcsod.cvtask_log.buffer_addr_offset != state->orcvp.cvtask_log.buffer_addr_offset) {
        sync_logger_buffer((char*)state, sync, &state->orcsod.cvtask_log);
    }
    if (state->orcsod.sched_log.buffer_addr_offset != state->orcvp.sched_log.buffer_addr_offset) {
        sync_logger_buffer((char*)state, sync, &state->orcsod.sched_log);
    }
    sync_logger_buffer((char*)state, sync, &state->orcvp.perf_log);
    sync_logger_buffer((char*)state, sync, &state->orcvp.cvtask_log);
    sync_logger_buffer((char*)state, sync, &state->orcvp.sched_log);

    for (i = 0; i < state->num_arm; i++) {
        sync_logger_buffer((char*)state,sync, &state->arm[i].perf_log);
        sync_logger_buffer((char*)state,sync, &state->arm[i].cvtask_log);
        sync_logger_buffer((char*)state,sync, &state->arm[i].sched_log);
    }

    return 0;
}

static void clean_log_buffer(schdr_log_info_t *info)
{
    char     *base;
    uint32_t esize, wi_curr, wi_prev, *wptr;

    wptr = (uint32_t*)ambacv_c2v(info->wptr_daddr);
    base = (char*)ambacv_c2v(info->base_daddr);

    wi_prev = *wptr;
    wi_curr = (info->wridx & info->mask);
    esize = info->esize;

    if (wi_prev > wi_curr) {
        char     *ptr = base + wi_prev * esize;
        uint32_t len = (info->mask + 1 - wi_prev) * esize;
        ambacv_cache_clean(ptr, len);
        wi_prev = 0;
    }

    if (wi_prev != wi_curr) {
        char     *ptr = base + wi_prev * esize;
        uint32_t len = (wi_curr - wi_prev) * esize;
        ambacv_cache_clean(ptr, len);
    }

    *wptr = wi_curr;
    ambacv_cache_clean(wptr, sizeof(*wptr));
}

static void clean_scheduler_log(void)
{
    schdr_log_info_t *linfo;

    linfo = (schdr_log_info_t*)ambacv_c2v(local_log_info_daddr);
    clean_log_buffer(&linfo[0]);
    clean_log_buffer(&linfo[1]);
    clean_log_buffer(&linfo[2]);
}

static int32_t process_idsp_init_msg(void *payload)
{
    idspfeeder_idsp_init_msg_t *msg = (idspfeeder_idsp_init_msg_t*)payload;
    vp_frame_list_descriptor_t *dsptr;
    uint32_t i, size, fov;

    /* use msg->num_fov and msg->param_size to check if msg is valid */
    fov = msg->num_fov;
    if (fov > 2) {
        return -1;
    }
    if (fov <= 2) {
        size = 16 + fov * sizeof(vp_frame_list_descriptor_t);
        if (msg->param_size != size) {
            return -1;
        }
    }

    for (i = 0; i < fov; i++) {
        dsptr = &(msg->frame_list_descriptor[i]);
        ambacv_cache_invalidate(ambacv_c2v(dsptr->frame_list_table_addr),
                                dsptr->frame_list_table_size << 2);
        ambacv_cache_barrier();
    }

    return 0;
}

static void invalidate_run(armvis_msg_t *msg)
{
    schedmsg_cvtask_run_t *req = &msg->msg.cvtask_run;
    cvmem_membuf_t *membuf;
    uint32_t i;

    for (i = 0; i < req->cvtask_num_inputs; i++) {
        if (req->InputMemBuf_daddr[i] == 0) {
            /* skip special case for dependency-only input */
            continue;
        }

        membuf = (cvmem_membuf_t *)ambacv_c2v(req->InputMemBuf_daddr[i]);
        ambacv_cache_invalidate(membuf, sizeof(*membuf));
        ambacv_cache_barrier();
        if ((req->ignore_invalidate_flags & (1 << (i + 0))) == 0) {
            if (membuf->hdr_fixed.needs_cache_coherency != 0) {
                ambacv_cache_invalidate(
                    ambacv_c2v(membuf->hdr_variable.payload_daddr),
                    membuf->hdr_variable.payload_size);
                ambacv_cache_barrier();
            }
        }

        /* process special case for cvlog output */
        if (membuf->hdr_fixed.framework_reserved == 1) {
            invalidate_logger_state((membuf->hdr_variable.payload_daddr), 0);
        }
    }

    for (i = 0; i < req->cvtask_num_feedback; i++) {
        if (req->FeedbackMemBuf_daddr[i] == 0) {
            /* skip special case */
            continue;
        }
        membuf = (cvmem_membuf_t *)ambacv_c2v(req->FeedbackMemBuf_daddr[i]);
        ambacv_cache_invalidate(membuf, sizeof(*membuf));
        ambacv_cache_barrier();
        if ((req->ignore_invalidate_flags & (1 << (i + 28))) == 0) {
            if (membuf->hdr_fixed.needs_cache_coherency != 0) {
                ambacv_cache_invalidate(
                    ambacv_c2v(membuf->hdr_variable.payload_daddr),
                    membuf->hdr_variable.payload_size);
                ambacv_cache_barrier();
            }
        }
    }

    for (i = 0; i < req->cvtask_num_messages; i++) {
        uint32_t *payload;
        cvmem_messagebuf_t *cvmsg;

        cvmsg = (cvmem_messagebuf_t *) ambacv_c2v(req->CVTaskMessage_daddr[i]);
        ambacv_cache_invalidate(cvmsg, sizeof(*cvmsg));
        ambacv_cache_barrier();

        payload = (uint32_t*)ambacv_c2v(cvmsg->hdr_variable.message_payload_daddr);
        ambacv_cache_invalidate(payload, cvmsg->hdr_variable.message_size);
        ambacv_cache_barrier();

        /* process special case for IDSP_FEEDER_IDSP_INIT msg */
        if (payload[0] == IDSP_FEEDER_IDSP_INIT) {
            process_idsp_init_msg(payload);
        }
    }

    for (i = 0; i < req->cvtask_num_outputs; i++) {
        membuf = (cvmem_membuf_t *)
                 ambacv_c2v(req->OutputMemBuf_daddr[i]);
        ambacv_cache_invalidate(membuf, sizeof(*membuf));
        ambacv_cache_barrier();
    }
}

static void invalidate_boot_setup2(armvis_msg_t *msg)
{
    schedmsg_boot_setup2_t *req = &msg->msg.boot_setup2;

    local_log_info_daddr = req->sysflow_block_daddr;
    host_logformat_addr = req->printf_string_block_daddr;
}

static void invalidate_boot_setup3(armvis_msg_t *msg)
{
    schedmsg_boot_setup3_t *req = &msg->msg.boot_setup3;
    cvtable_entry_t  *info;
    sysflow_entry_t *entry;
    int32_t i;

    if ((req->sysflow_table_daddr != 0U) && (req->sysflow_numlines != 0U)) {
        entry = (sysflow_entry_t*)ambacv_c2v(req->sysflow_table_daddr);
        ambacv_cache_invalidate(entry, req->sysflow_numlines * sizeof(*entry));
    } else {
        entry = NULL;
    }

    if ((req->cvtable_base_daddr != 0U) && (req->cvtable_numlines != 0U)) {
        info = (cvtable_entry_t *)ambacv_c2v(req->cvtable_base_daddr);
        ambacv_cache_invalidate(info, req->cvtable_numlines * sizeof(*info));
        ambacv_cache_barrier();
        for (i = req->cvtable_numlines; i > 0; i--, info++) {
            ambacv_cache_invalidate(ambacv_c2v(info->CVTable_name_daddr), CVTABLE_NAME_MAX_LENGTH);
            ambacv_cache_invalidate(ambacv_c2v(info->CVTable_daddr), info->cvtable_size);
        }
        ambacv_cache_barrier();
    } else {
        info = NULL;
    }
}

static void invalidate_memory_report(armvis_msg_t *msg)
{
    schedmsg_cvscheduler_memory_report_t *req = &msg->msg.memory_report;
    component_build_info_t *entry;
    int32_t nsize = NAME_MAX_LENGTH;

    cvtask_mmap_remap(ambacv_c2p(req->armblock_baseaddr));

    // invalidate default message pool
    ambacv_cache_invalidate(ambacv_c2v(req->CVTaskMsgPool_daddr),
                            sizeof(cvmem_messagepool_t));

    // invalidate cvtask build info
    ambacv_cache_invalidate(ambacv_c2v(req->CVTaskInfoTable_daddr),
                            sizeof(component_build_info_t) * req->sysflow_numlines);

    // invalidate scheduler build info
    ambacv_cache_invalidate(ambacv_c2v(req->SchedInfoTable_daddr),
                            sizeof(component_build_info_t) * 16);

    ambacv_cache_barrier();

    // invalidate the component name string
    entry = (component_build_info_t *) ambacv_c2v(req->CVTaskInfoTable_daddr);
    while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
        ambacv_cache_invalidate(ambacv_c2v(entry->component_name_daddr), nsize);
        entry++;
    }

    entry = (component_build_info_t *) ambacv_c2v(req->SchedInfoTable_daddr);
    while (entry->component_type != COMPONENT_TYPE_END_OF_LIST) {
        ambacv_cache_invalidate(ambacv_c2v(entry->component_name_daddr), nsize);
        entry++;
    }

    ambacv_cache_barrier();
}

static void invalidate_shutdown(armvis_msg_t *msg)
{
    schedmsg_scheduler_shutdown_t *req = &msg->msg.shutdown;
    invalidate_logger_state(req->CVStaticLogInfo_daddr, 0);
}

static void invalidate_startup_log_update(armvis_msg_t *msg)
{
    schedmsg_cvlog_update_t *req = &msg->msg.cvlog_update;
    invalidate_logger_state(req->CVStaticLogInfo_daddr, 1);
}

static void invalidate_open_parsed(armvis_msg_t *msg)
{
    schedmsg_flexidag_open_parsed_t *req;

    req = &msg->msg.flexidag_open_parsed;

    if ((req->sysflow_daddr != 0U) && (req->sysflow_num_entries != 0U)) {
        sysflow_entry_t *entry;
        entry = (sysflow_entry_t*)ambacv_c2v(req->sysflow_daddr);
        ambacv_cache_invalidate(entry, req->sysflow_num_entries * sizeof(*entry));
        ambacv_cache_barrier();
    }

    if ((req->cvtable_daddr != 0U) && (req->cvtable_num_entries != 0U)) {
        cvtable_entry_t  *info;
        int32_t i;
        info = (cvtable_entry_t *)ambacv_c2v(req->cvtable_daddr);
        ambacv_cache_invalidate(info, req->cvtable_num_entries * sizeof(*info));
        ambacv_cache_barrier();
        for (i = req->cvtable_num_entries; i > 0; i--, info++) {
            ambacv_cache_invalidate(ambacv_c2v(info->CVTable_name_daddr), CVTABLE_NAME_MAX_LENGTH);
            ambacv_cache_invalidate(ambacv_c2v(info->CVTable_daddr), info->cvtable_size);
        }
        ambacv_cache_barrier();
    }
}

void cache_invld_schdrmsg(armvis_msg_t *msg)
{
    ambacv_cache_invalidate((char*)msg, sizeof(*msg));
    ambacv_cache_barrier();

    switch (msg->hdr.message_type) {
    case SCHEDMSG_CVTASK_RUN_REQUEST:
    case SCHEDMSG_CVTASK_MSGONLY_REQUEST:
    case SCHEDMSG_CVTASK_FINISH_REQUEST:
        invalidate_run(msg);
        break;

    case SCHEDMSG_BOOT_SETUP2:
        invalidate_boot_setup2(msg);
        break;

    case SCHEDMSG_BOOT_SETUP3:
        invalidate_boot_setup3(msg);
        break;

    case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
        invalidate_memory_report(msg);
        return;

    case SCHEDMSG_VISORC_PRINTF_UPDATE:
        clean_scheduler_log();
        break;

    case SCHEDMSG_VISORC_STARTUP_UPDATE:
        invalidate_startup_log_update(msg);
        break;

    case SCHEDMSG_SCHEDULER_SHUTDOWN:
        invalidate_shutdown(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP:
    case SCHEDMSG_BOOT_SETUP1:
    case SCHEDMSG_SET_BOOTUP_PHASE:
    case SCHEDMSG_CVTASK_REGISTER:
    case SCHEDMSG_CVTASK_QUERY_REQUEST:
    case SCHEDMSG_CVTASK_INIT_REQUEST:
    case SCHEDMSG_CONFIGURE_IDSP:
    case SCHEDMSG_CONFIGURE_VIN:
    case SCHEDMSG_CVTASK_GET_BUILDINFO:
    case SCHEDMSG_HWUNIT_RESET:
    case SCHEDMSG_CVSCHEDULER_ACTIVE:
    case SCHEDMSG_CONSOLE_ECHO_MESSAGE:
    case SCHEDMSG_SUPERDAG_ERROR_MESSAGE:
    case SCHEDMSG_SET_REWAKE_TIME:
    case SCHEDMSG_TIME_REPORT:
        // these messages have nothing to invalidate:
        break;

    // Cavalry related =--------------------------------------------------------
    case SCHEDMSG_CAVALRY_SETUP:
    case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
    case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
    case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
    case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
        // these messages have nothing to invalidate:
        break;

    case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
        invalidate_open_parsed(msg);
        break;

    // Flexidag related =-------------------------------------------------------
    case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
    case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
    case SCHEDMSG_FLEXIDAG_SETUP_REQUEST:
    case SCHEDMSG_FLEXIDAG_INIT_REPLY:
    case SCHEDMSG_FLEXIDAG_INIT_PARTITIONED:
    case SCHEDMSG_FLEXIDAG_RUN_REPLY:
    case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
    case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
    case SCHEDMSG_FLEXIDAG_CVREG_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVREG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVGETVER_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVGETVER_REPLY:
    case SCHEDMSG_FLEXIDAG_CVQUERY_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVQUERY_REPLY:
    case SCHEDMSG_FLEXIDAG_CVINIT_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVINIT_REPLY:
    case SCHEDMSG_FLEXIDAG_ERROR_MESSAGE:
        // these messages have nothing to invalidate:
        break;

    case SCHEDMSG_FLEXIDAG_CVRUN_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REQUEST:
        invalidate_run(msg);
        break;

    default:
        console_printU5(
            "[ERROR] cache_invld_schdrmsg() unknown armvis_msg_t type %d\n",
            msg->hdr.message_type,
            0U, 0U, 0U, 0U);
        break;
    }

    //schdrmsg_log_msg(msg);
}

static void clean_cvtaskmsg_send(armvis_msg_t *msg)
{
    cvmem_messagebuf_t *entry;

    entry = (cvmem_messagebuf_t *)ambacv_c2v(msg->msg.reply_cvtask_msg.CVTaskMessage_daddr);
    ambacv_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
    ambacv_cache_clean(ambacv_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
}

static void clean_cvtaskmsg_send_at_time(armvis_msg_t *msg)
{
    cvmem_messagebuf_t *entry;

    entry = (cvmem_messagebuf_t*)ambacv_c2v(msg->msg.reply_cvtask_msg_at_time.CVTaskMessage_daddr);
    ambacv_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
    ambacv_cache_clean(ambacv_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
}

static void clean_cvtaskmsg_bulk_send(armvis_msg_t *msg)
{
    schedmsg_attach_bulk_cvtask_message_t *rpl;
    cvmem_messagebuf_t *entry;
    int32_t i, total;

    rpl = &(msg->msg.reply_cvtask_bulk_msg);
    total = rpl->num_cvtask_messages;

    for (i = 0; i < total; i++) {
        uint32_t addr = rpl->CVTaskMessage_daddr[i] - sizeof(*entry);
        entry = (cvmem_messagebuf_t  *)ambacv_c2v(addr);
        ambacv_cache_clean(&entry->hdr_variable, sizeof(entry->hdr_variable));
        ambacv_cache_clean(ambacv_c2v(entry->hdr_variable.message_payload_daddr), entry->hdr_variable.message_size);
    }
}

static void clean_cvtask_query(armvis_msg_t *msg)
{
    schedmsg_cvtask_query_reply_t *rpl = &msg->msg.reply_cvtask_query;
    ambacv_cache_clean(ambacv_c2v(rpl->cvtask_memory_interface_daddr),
                       sizeof(cvtask_memory_interface_t));
}

static void clean_get_buildinfo(armvis_msg_t *msg)
{
    schedmsg_cvtask_getbuildinfo_reply_t *rpl = &msg->msg.reply_cvtask_getbuildinfo;
    int32_t maxsize;

    maxsize = rpl->versionstring_maxsize;
    ambacv_cache_clean(ambacv_c2v(rpl->daddr_versionstring_location), maxsize);
    ambacv_cache_clean(ambacv_c2v(rpl->daddr_toolstring_location), maxsize);

    if (rpl->arm_cvtask_index == ARM_CVTASK_INDEX_SCHEDULER) {
        maxsize = NAME_MAX_LENGTH;
        ambacv_cache_clean(ambacv_c2v(rpl->daddr_component_location), maxsize);
    }

}

static void clean_cvtask_run(armvis_msg_t *msg)
{
    schedmsg_cvtask_run_t *req = &msg->msg.cvtask_run;
    int32_t i;

    for (i = 0; i < req->cvtask_num_outputs; i++) {
        cvmem_membuf_t *membuf =
            (cvmem_membuf_t *) ambacv_c2v(req->OutputMemBuf_daddr[i]);
        if (!membuf->hdr_fixed.needs_cache_coherency) {
            //printk("skip output clean %d\n",
            //       membuf->hdr_fixed.sysflow_index);
            continue;
        }
        ambacv_cache_clean(ambacv_c2v(membuf->hdr_variable.payload_daddr),
                           membuf->hdr_variable.payload_size);
    }

    for (i = 0; i < req->cvtask_num_messages; i++) {
        cvmem_messagebuf_t *cvmsg = (cvmem_messagebuf_t *) ambacv_c2v(req->CVTaskMessage_daddr[i]);
        cvmsg->hdr_variable.message_processed     = 1U;
        cvmsg->hdr_variable.message_finished_time = cvtask_get_timestamp();
        ambacv_cache_clean(&cvmsg->hdr_variable, sizeof(cvmsg->hdr_variable));
    }
}

static void clean_cvtask_partial_run(armvis_msg_t *msg)
{
    cvmem_membuf_t *membuf;

    membuf = (cvmem_membuf_t *)
             ambacv_c2v(msg->msg.reply_cvtask_run_partial.output_membuf_daddr);

    if (membuf->hdr_fixed.needs_cache_coherency) {
        ambacv_cache_clean(ambacv_c2v(membuf->hdr_variable.payload_daddr),
                           membuf->hdr_variable.payload_size);
    }
}

static void clean_boot_setup1(armvis_msg_t *msg)
{
    schedmsg_boot_setup1_reply_t *rpl = &msg->msg.reply_boot_setup1;
    host_logformat_size = rpl->memsize_printf_string_block;
}

static void clean_boot_setup2(armvis_msg_t *msg)
{
    schedmsg_boot_setup2_reply_t *rpl = &msg->msg.reply_boot_setup2;
    uint32_t index, len;
    uint32_t *ptr;

    for (index = 0; index < rpl->sysflow_table_num_sets; index++) {
        len = rpl->sysflow_table_set_numlines[index] * sizeof(sysflow_entry_t);
        ptr = (uint32_t*)ambacv_c2v(rpl->sysflow_table_set_daddr[index]);
        ambacv_cache_clean(ptr, len);
    }

    for (index = 0; index < rpl->cvtable_num_sets; index++) {
        ptr = (uint32_t*)ambacv_c2v(rpl->cvtable_daddr[index]);
        // ptr[4] is the total length of the cvtable
        ambacv_cache_clean(ptr, ptr[4]);
    }


    ambacv_cache_clean(ambacv_c2v(host_logformat_addr), host_logformat_size);
}


void cache_clean_schdrmsg(armvis_msg_t *msg)
{
    switch (msg->hdr.message_type) {
    case SCHEDMSG_CVTASK_MSG_REPLY:
        clean_cvtaskmsg_send(msg);
        break;

    case SCHEDMSG_CVTASK_MSG_AT_TIME_REPLY:
        clean_cvtaskmsg_send_at_time(msg);
        break;

    case SCHEDMSG_SEND_BULK_CVTASK_MSG_TO_FRAME:
        clean_cvtaskmsg_bulk_send(msg);
        break;

    case SCHEDMSG_CVTASK_QUERY_REPLY:
        clean_cvtask_query(msg);
        break;

    case SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY:
        clean_get_buildinfo(msg);
        break;

    case SCHEDMSG_CVTASK_RUN_REPLY:
    case SCHEDMSG_CVTASK_MSGONLY_REPLY:
    case SCHEDMSG_CVTASK_FINISH_REPLY:
        clean_cvtask_run(msg);
        clean_scheduler_log();
        break;

    case SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY:
        clean_cvtask_partial_run(msg);
        clean_scheduler_log();
        break;

    case SCHEDMSG_BOOT_SETUP1_REPLY:
        clean_boot_setup1(msg);
        break;

    case SCHEDMSG_BOOT_SETUP2_REPLY:
        clean_boot_setup2(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP_REPLY:
    case SCHEDMSG_BOOT_SETUP3_REPLY:
    case SCHEDMSG_CVTASK_REGISTER_REPLY:
    case SCHEDMSG_CVTASK_INIT_REPLY:
    case SCHEDMSG_SET_DEBUG_LEVEL:
    case SCHEDMSG_CVSCHEDULER_RUN:
    case SCHEDMSG_SET_FREQUENCY:
    case SCHEDMSG_SET_FREQUENCY_AT_FRAME:
    case SCHEDMSG_SET_FREQUENCY_AT_TIME:
    case SCHEDMSG_ARM_REQUESTS_SHUTDOWN:
    case SCHEDMSG_WAKE_FEEDERS:
    case SCHEDMSG_HWUNIT_RESET_REPLY:
    case SCHEDMSG_INVALIDATE_CACHE:
    case SCHEDMSG_LICENSE_UPDATE:
        // these messages have nothing to clean
        break;

    // Cavalry related =--------------------------------------------------------
    case SCHEDMSG_CAVALRY_SETUP:
    case SCHEDMSG_CAVALRY_VP_RUN_REQUEST:
    case SCHEDMSG_CAVALRY_HL_RUN_REQUEST:
    case SCHEDMSG_CAVALRY_FEX_QUERY_REQUEST:
    case SCHEDMSG_CAVALRY_FEX_RUN_REQUEST:
    case SCHEDMSG_CAVALRY_FMA_QUERY_REQUEST:
    case SCHEDMSG_CAVALRY_FMA_RUN_REQUEST:
        // these messages have nothing to clean
        break;

    // Flexidag related =-------------------------------------------------------
    case SCHEDMSG_FLEXIDAG_CVTASK_MSG:
        clean_cvtaskmsg_send(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVTASK_MSG_AT_TIME:
        clean_cvtaskmsg_send_at_time(msg);
        break;
    case SCHEDMSG_FLEXIDAG_SEND_BULK_CVTASK_MSG_TO_FRAME:
        clean_cvtaskmsg_bulk_send(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVGETVER_REPLY:
        clean_get_buildinfo(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVQUERY_REPLY:
        clean_cvtask_query(msg);
        break;
    case SCHEDMSG_FLEXIDAG_CVRUN_REPLY:
    case SCHEDMSG_FLEXIDAG_CVRUN_MSG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVRUN_FIN_REPLY:
        clean_cvtask_run(msg);
        clean_scheduler_log();
        break;
    case SCHEDMSG_FLEXIDAG_CVRUN_PARTIAL_REPLY:
        clean_cvtask_partial_run(msg);
        clean_scheduler_log();
        break;
    case SCHEDMSG_FLEXIDAG_CREATE_REQUEST:
    case SCHEDMSG_FLEXIDAG_OPEN_REQUEST:
    case SCHEDMSG_FLEXIDAG_SETUP_REPLY:
    case SCHEDMSG_FLEXIDAG_INIT_REQUEST:
    case SCHEDMSG_FLEXIDAG_RUN_REQUEST:
    case SCHEDMSG_FLEXIDAG_CLOSE_REQUEST:
    case SCHEDMSG_FLEXIDAG_CVREG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVINIT_REPLY:
    case SCHEDMSG_FLEXIDAG_SET_DEBUG_LEVEL:
    case SCHEDMSG_FLEXIDAG_SET_FREQ:
    case SCHEDMSG_FLEXIDAG_SET_FREQ_AT_FRAME:
    case SCHEDMSG_FLEXIDAG_SET_FREQ_AT_TIME:
        // these messages have nothing to clean
        break;

    case SCHEDMSG_SEND_PRIVATE_MSG:
    case SCHEDMSG_FLEXIDAG_SEND_PRIVATE_MSG:
        // these messages have nothing to clean
        break;

    default:
        console_printU5(
            "[ERROR] cache_clean_schdrmsg() unknown armvis_msg_t type %X\n",
            msg->hdr.message_type,
            0U, 0U, 0U, 0U);
    }

    ambacv_cache_clean((char*)msg, sizeof(*msg));
    //schdrmsg_log_msg(msg);
}

