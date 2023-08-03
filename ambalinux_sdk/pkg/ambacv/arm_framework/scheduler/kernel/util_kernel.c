// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

#include "os_kernel.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "schdrmsg_def.h"
#include "cache_kernel.h"

static void krn_req_boot_setup1(const armvis_msg_t *msg)
{
    const schedmsg_boot_setup1_t *r = &msg->msg.boot_setup1;

    krn_module_printU5("-->SCHEDMSG_BOOT_SETUP1: version=(0x%X 0x%X)\n",
                       r->scheduler_version, r->cvtask_version, 0U, 0U, 0U);
}

static void krn_rpl_boot_setup1(const armvis_msg_t *msg)
{
    const schedmsg_boot_setup1_reply_t *r = &msg->msg.reply_boot_setup1;

    krn_module_printU5("<--SCHEDMSG_BOOT_SETUP1_REPLY: version=(0x%X 0x%X), #cvtask=%d\n",
                       r->scheduler_version, r->cvtask_version, r->cvtask_total_num, 0U, 0U);
    krn_module_printU5("                               %X %X %X",
                       r->memsize_arm_sysflow_block, r->memsize_arm_cvtable_block, r->memsize_printf_string_block, 0U, 0U);
    krn_module_printU5("%X %X %X\n",
                       r->memsize_arm_cvtask_debug_buffer, r->memsize_arm_sched_debug_buffer, r->memsize_arm_perf_debug_buffer, 0U, 0U);
}

static void krn_req_boot_setup2(const armvis_msg_t *msg)
{
    const schedmsg_boot_setup2_t *r = &msg->msg.boot_setup2;

    krn_module_printU5("-->SCHEDMSG_BOOT_SETUP2:\n", 0U, 0U, 0U, 0U, 0U);
    krn_module_printU5("        sysflow_addr = 0x%llX\n", krn_c2p(r->sysflow_block_daddr), 0U, 0U, 0U, 0U);
    krn_module_printU5("        cvtable_addr = 0x%llX\n", krn_c2p(r->cvtable_block_daddr), 0U, 0U, 0U, 0U);
    krn_module_printU5("         format_addr = 0x%llX\n", krn_c2p(r->printf_string_block_daddr), 0U, 0U, 0U, 0U);
    krn_module_printU5("       log_info_addr = 0x%llX\n", krn_c2p(r->arm_printf_buffer_info_daddr), 0U, 0U, 0U, 0U);
    krn_module_printU5("         cvtask_addr = 0x%llX\n", krn_c2p(r->arm_cvtask_debug_buffer_daddr), 0U, 0U, 0U, 0U);
    krn_module_printU5("          sched_addr = 0x%llX\n", krn_c2p(r->arm_sched_debug_buffer_daddr), 0U, 0U, 0U, 0U);
    krn_module_printU5("           perf_addr = 0x%llX\n", krn_c2p(r->arm_perf_debug_buffer_daddr), 0U, 0U, 0U, 0U);
}

static void krn_rpl_boot_setup2(const armvis_msg_t *msg)
{
    const schedmsg_boot_setup2_reply_t *r = &msg->msg.reply_boot_setup2;
    uint64_t addr1, addr2;

    addr1 = krn_c2p(r->sysflow_table_set_daddr[0]);
    addr2 = krn_c2p(r->cvtable_daddr[0]);
    krn_module_printU5("<--SCHEDMSG_BOOT_SETUP2_REPLY: sysflow=(%d,@0x%llx), cvtable=(%d,@0x%llx)\n",
                       r->sysflow_table_set_numlines[0], addr1,
                       r->cvtable_num_sets, addr2, 0U);
}

static void krn_req_boot_setup3(const armvis_msg_t *msg)
{
    const schedmsg_boot_setup3_t *r = &msg->msg.boot_setup3;
    uint64_t addr1,addr2;

    addr1 = krn_c2p(r->sysflow_table_daddr);
    addr2 = krn_c2p(r->cvtable_base_daddr);
    krn_module_printU5("-->SCHEDMSG_BOOT_SETUP3: sysflow=0x%llx cvtable=0x%llx\n",
                       addr1, addr2, 0U, 0U, 0U);
}

static void krn_rpl_boot_setup3(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("<--SCHEDMSG_BOOT_SETUP3_REPLY:\n", 0U, 0U, 0U, 0U, 0U);
}

static void krn_req_cvtask_reg(const armvis_msg_t *msg)
{
    krn_module_printU5("-->SCHEDMSG_CVTASK_REGISTER:   %d\n", msg->msg.cvtask_register_msg.arm_cvtask_index, 0U, 0U, 0U, 0U);
}

static void krn_rpl_cvtask_reg(const armvis_msg_t *msg)
{
    krn_module_printU5("<--SCHEDMSG_CVTASK_REGISTER_REPLY:  (%d)", msg->msg.reply_cvtask_register.arm_cvtask_index, 0U, 0U, 0U, 0U);
    krn_module_printS5(" : %s\n", msg->msg.reply_cvtask_register.cvtask_name, NULL, NULL, NULL, NULL);
}

static void krn_req_cvtask_query(const armvis_msg_t *msg)
{
    const schedmsg_cvtask_query_t *r = &msg->msg.cvtask_query;
    krn_module_printU5("-->SCHEDMSG_CVTASK_QUERY_REQUEST: (%d %d %d) interface @0x%llx\n",
                       r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance, krn_c2p(r->cvtask_memory_interface_daddr), 0U);
}

static void krn_rpl_cvtask_query(const armvis_msg_t *msg)
{
    const schedmsg_cvtask_query_reply_t *r = &msg->msg.reply_cvtask_query;
    krn_module_printU5("<--SCHEDMSG_CVTASK_QUERY_REPLY: (%d %d %d) interface @0x%llx\n",
                       r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance, krn_c2p(r->cvtask_memory_interface_daddr), 0U);
}

static void krn_req_cvtask_init(const armvis_msg_t *msg)
{
    const schedmsg_cvtask_init_t *r = &msg->msg.cvtask_init;
    krn_module_printU5("-->SCHEDMSG_CVTASK_INIT_REQUEST:  (%d %d %d) (%d, @0x%llx) \n",
                       r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance,
                       r->Instance_private_storage_size, krn_c2p(r->Instance_private_storage_daddr));
}

static void krn_rpl_cvtask_init(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("<--SCHEDMSG_CVTASK_INIT_REPLY:\n", 0U, 0U, 0U, 0U, 0U);
}

static void krn_req_cvtask_finish(const armvis_msg_t *msg)
{
    const schedmsg_cvtask_run_t *r = &msg->msg.cvtask_run;
    krn_module_printU5("-->SCHEDMSG_CVTASK_FINISH_REQUEST:   (%d %d %d) (%d,0x%llX) \n",
                       r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance,
                       r->Instance_private_storage_size, krn_c2p(r->Instance_private_storage_daddr));
}

static void krn_rpl_cvtask_finish(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("<--SCHEDMSG_CVTASK_FINISH_REPLY:", 0U, 0U, 0U, 0U, 0U);
}

static void krn_req_cvtask_run(const armvis_msg_t *msg)
{
    const schedmsg_cvtask_run_t *r = &msg->msg.cvtask_run;
    krn_module_printU5("-->SCHEDMSG_CVTASK_RUN_REQUEST:   (%d %d %d) (%d,0x%llX) \n",
                       r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance,
                       r->Instance_private_storage_size, krn_c2p(r->Instance_private_storage_daddr));
}

static void krn_rpl_cvtask_run(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("<--SCHEDMSG_CVTASK_RUN_REPLY:\n", 0U, 0U, 0U, 0U, 0U);
}

static void krn_rpl_cvtask_partial_run(const armvis_msg_t *msg)
{
    krn_module_printU5("<--SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY: ind[%d] daddr[0x%x]\n",
                       msg->msg.reply_cvtask_run_partial.output_index, msg->msg.reply_cvtask_run_partial.output_membuf_daddr, 0U, 0U, 0U);
}

static void krn_req_set_phase(const armvis_msg_t *msg)
{
    krn_module_printU5("-->SCHEDMSG_SET_BOOTUP_PHASE:    %d\n",
                       msg->msg.set_bootup_phase.bootup_phase, 0U, 0U, 0U, 0U);
}

static void krn_req_init_setup(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("-->SCHEDMSG_INITIAL_SETUP\n", 0U, 0U, 0U, 0U, 0U);
}

static void krn_rpl_init_setup(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("<--SCHEDMSG_INITIAL_SETUP_REPLY\n", 0U, 0U, 0U, 0U, 0U);
}

static void krn_req_report_memory(const armvis_msg_t *msg)
{
    const schedmsg_cvscheduler_memory_report_t *r = &msg->msg.memory_report;
    uint64_t addr1, addr2;

    addr1 = krn_c2p(r->orc_cached_baseaddr);
    addr2 = krn_c2p(r->orc_uncached_baseaddr);
    krn_module_printU5("-->SCHDRMSG_REPORT_MEM:   0x%llx+0x%x, 0x%llx+0x%x",
                       addr1, r->orc_cached_size,
                       addr2, r->orc_uncached_size, 0U);
    addr1 = krn_c2p(r->armblock_baseaddr);
    addr2 = krn_c2p(r->CVTaskMsgPool_daddr);
    krn_module_printU5(", 0x%llx+0x%llx, 0x%llx\n",
                       addr1, r->armblock_size, addr2, 0U, 0U);
}

static void krn_rpl_cvtaskmsg_send(const armvis_msg_t *msg)
{
    krn_module_printU5("<--SCHEDMSG_CVTASK_MSG_REPLY: target frameset_id=%d\n", msg->msg.reply_cvtask_msg.target_frameset_id, 0U, 0U, 0U, 0U);
}

static void krn_rpl_start_engine(const armvis_msg_t *msg)
{
    (void) msg;
    krn_module_printU5("<--SCHEDMSG_CVSCHEDULER_RUN\n", 0U, 0U, 0U, 0U, 0U);
}

static void krn_req_cvtask_get_buildinfo(const armvis_msg_t *msg)
{
    krn_module_printU5("-->SCHEDMSG_CVTASK_GET_BUILDINFO: %d\n",
                       msg->msg.cvtask_getbuildinfo.sysflow_index, 0U, 0U, 0U, 0U);
}

static void krn_rpl_cvtask_get_buildinfo(const armvis_msg_t *msg)
{
    krn_module_printU5("<--SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY: %d\n",
                       msg->msg.reply_cvtask_getbuildinfo.sysflow_index, 0U, 0U, 0U, 0U);
}

static void krn_req_softreset(const armvis_msg_t *msg)
{
    const schedmsg_softreset_t *r = &msg->msg.hw_reset;
    krn_module_printU5("-->SCHEDMSG_HWUNIT_RESET: %d\n", r->hardware_unit, 0U, 0U, 0U, 0U);
}

static void krn_rpl_softreset(const armvis_msg_t *msg)
{
    const schedmsg_softreset_t *r = &msg->msg.reply_hw_reset;
    krn_module_printU5("<--SCHEDMSG_HWUNIT_RESET_REPLY: %d\n", r->hardware_unit, 0U, 0U, 0U, 0U);
}

static void krn_req_log_update(const armvis_msg_t *msg)
{
    const schedmsg_cvlog_update_t *r = &msg->msg.cvlog_update;
    krn_module_printU5("-->SCHEDMSG_VISORC_PRINTF_UPDATE 0x%llX\n", krn_c2p(r->CVStaticLogInfo_daddr), 0U, 0U, 0U, 0U);
}

static void krn_req_startup_log_update(const armvis_msg_t *msg)
{
    const schedmsg_cvlog_update_t *r = &msg->msg.cvlog_update;
    krn_module_printU5("-->SCHEDMSG_VISORC_STARTUP_UPDATE 0x%llX\n", krn_c2p(r->CVStaticLogInfo_daddr), 0U, 0U, 0U, 0U);
}

static void krn_req_shutdown(const armvis_msg_t *msg)
{
    const schedmsg_scheduler_shutdown_t *r = &msg->msg.shutdown;
    krn_module_printU5("-->SCHEDMSG_SCHEDULER_SHUTDOWN 0x%llX 0x%X\n",
                       krn_c2p(r->CVStaticLogInfo_daddr), r->shutdown_retcode, 0U, 0U, 0U);
}

static void krn_log_unknown(const armvis_msg_t *msg)
{
    krn_module_printU5("---SCHDRMSG_UNKNOWN:      0x%X\n", msg->hdr.message_type, 0U, 0U, 0U, 0U);
}

static void krn_schdrmsg_log_boot1(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_BOOT_SETUP1:
        krn_req_boot_setup1(msg);
        break;
    case SCHEDMSG_BOOT_SETUP1_REPLY:
        krn_rpl_boot_setup1(msg);
        break;

    case SCHEDMSG_BOOT_SETUP2:
        krn_req_boot_setup2(msg);
        break;
    case SCHEDMSG_BOOT_SETUP2_REPLY:
        krn_rpl_boot_setup2(msg);
        break;

    case SCHEDMSG_BOOT_SETUP3:
        krn_req_boot_setup3(msg);
        break;
    case SCHEDMSG_BOOT_SETUP3_REPLY:
        krn_rpl_boot_setup3(msg);
        break;

    case SCHEDMSG_SET_BOOTUP_PHASE:
        krn_req_set_phase(msg);
        break;

    case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
        krn_req_report_memory(msg);
        break;
    case SCHEDMSG_CVSCHEDULER_RUN:
        krn_rpl_start_engine(msg);
        break;

    case SCHEDMSG_CVTASK_MSG_REPLY:
        krn_rpl_cvtaskmsg_send(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP:
        krn_req_init_setup(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP_REPLY:
        krn_rpl_init_setup(msg);
        break;

    case SCHEDMSG_HWUNIT_RESET:
        krn_req_softreset(msg);
        break;

    case SCHEDMSG_HWUNIT_RESET_REPLY:
        krn_rpl_softreset(msg);
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void krn_schdrmsg_log_boot2(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_VISORC_PRINTF_UPDATE:
        krn_req_log_update(msg);
        break;

    case SCHEDMSG_VISORC_STARTUP_UPDATE:
        krn_req_startup_log_update(msg);
        break;

    case SCHEDMSG_SCHEDULER_SHUTDOWN:
        krn_req_shutdown(msg);
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void krn_schdrmsg_log_cvtask1(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_CVTASK_REGISTER:
        krn_req_cvtask_reg(msg);
        break;
    case SCHEDMSG_CVTASK_REGISTER_REPLY:
        krn_rpl_cvtask_reg(msg);
        break;

    case SCHEDMSG_CVTASK_QUERY_REQUEST:
        krn_req_cvtask_query(msg);
        break;
    case SCHEDMSG_CVTASK_QUERY_REPLY:
        krn_rpl_cvtask_query(msg);
        break;

    case SCHEDMSG_CVTASK_INIT_REQUEST:
        krn_req_cvtask_init(msg);
        break;
    case SCHEDMSG_CVTASK_INIT_REPLY:
        krn_rpl_cvtask_init(msg);
        break;

    case SCHEDMSG_CVTASK_FINISH_REQUEST:
        krn_req_cvtask_finish(msg);
        break;
    case SCHEDMSG_CVTASK_FINISH_REPLY:
        krn_rpl_cvtask_finish(msg);
        break;

    case SCHEDMSG_CVTASK_RUN_REQUEST:
    case SCHEDMSG_CVTASK_MSGONLY_REQUEST:
        krn_req_cvtask_run(msg);
        break;
    case SCHEDMSG_CVTASK_RUN_REPLY:
    case SCHEDMSG_CVTASK_MSGONLY_REPLY:
        krn_rpl_cvtask_run(msg);
        break;
    case SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY:
        krn_rpl_cvtask_partial_run(msg);
        break;
    default:
        *hit = 0U;
        break;
    }
}

static void krn_schdrmsg_log_cvtask2(const armvis_msg_t *msg, uint32_t type, uint32_t *hit)
{
    *hit = 1U;
    switch (type) {
    case SCHEDMSG_CVTASK_GET_BUILDINFO:
        krn_req_cvtask_get_buildinfo(msg);
        break;

    case SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY:
        krn_rpl_cvtask_get_buildinfo(msg);
        break;
    default:
        *hit = 0U;
        break;
    }
}

void krn_schdrmsg_log_msg(const armvis_msg_t *msg)
{
    uint32_t type, hit;

    if(msg == NULL) {
        krn_printU5("[ERROR] krn_schdrmsg_log_msg() : msg == NULL", 0U, 0U, 0U, 0U, 0U);
    } else {
        type = (uint32_t)msg->hdr.message_type;
        krn_schdrmsg_log_boot1(msg, type, &hit);
        if(hit == 0U) {
            krn_schdrmsg_log_boot2(msg, type, &hit);
            if(hit == 0U) {
                krn_schdrmsg_log_cvtask1(msg, type, &hit);
                if(hit == 0U) {
                    krn_schdrmsg_log_cvtask2(msg, type, &hit);
                    if(hit == 0U) {
                        krn_log_unknown(msg);
                    }
                }
            }
        }
    }
}

