/**
 *  @file util.c
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
 *  @details Scheduler utility APIs
 *
 */

#include "os_api.h"
#include "schdrmsg_def.h"
#include "schdr.h"
#include "schdr_internal.h"
#include "msg.h"
#include "flexidag.h"
#include "flexidag_internal.h"
#include "cavalry.h"

#define UNUSED(x)       (void)(x)

static void req_boot_setup1(armvis_msg_t *msg)
{
    schedmsg_boot_setup1_t *r = &msg->msg.boot_setup1;

    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_BOOT_SETUP1: version=(0x%X 0x%X)\n",
                   r->scheduler_version,
                   r->cvtask_version,
                   0U, 0U, 0U);
}

static void rpl_boot_setup1(armvis_msg_t *msg)
{
    schedmsg_boot_setup1_reply_t *r = &msg->msg.reply_boot_setup1;

    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "<--SCHEDMSG_BOOT_SETUP1_REPLY: version=(0x%X 0x%X), #cvtask=%d\n",
                   r->scheduler_version,
                   r->cvtask_version,
                   r->cvtask_total_num,
                   0U, 0U);
}

static void req_boot_setup2(armvis_msg_t *msg)
{
    schedmsg_boot_setup2_t *r = &msg->msg.boot_setup2;

    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"-->SCHEDMSG_BOOT_SETUP2:\n", 0U, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"        sysflow_addr = 0x%08X\n",
                   r->sysflow_block_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"        cvtable_addr = 0x%08X\n",
                   r->cvtable_block_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"         format_addr = 0x%08X\n",
                   r->printf_string_block_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"       log_info_addr = 0x%08X\n",
                   r->arm_printf_buffer_info_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"         cvtask_addr = 0x%08X\n",
                   r->arm_cvtask_debug_buffer_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"          sched_addr = 0x%08X\n",
                   r->arm_sched_debug_buffer_daddr,0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"           perf_addr = 0x%08X\n",
                   r->arm_perf_debug_buffer_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"       vp_quque_addr = 0x%08X\n",
                   r->arm_idsp_picinfo_queue_daddr, 0U, 0U, 0U, 0U);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"    async_queue_addr = 0x%08X\n",
                   r->arm_idsp_async_queue_daddr, 0U, 0U, 0U, 0U);
}

static void rpl_boot_setup2(armvis_msg_t *msg)
{
    schedmsg_boot_setup2_reply_t *r = &msg->msg.reply_boot_setup2;

    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "<--SCHEDMSG_BOOT_SETUP2_REPLY: sysflow=(%d,%X), cvtable=(%d,%X)\n",
                   r->sysflow_table_set_numlines[0],
                   r->sysflow_table_set_daddr[0],
                   r->cvtable_num_sets,
                   r->cvtable_daddr[0],
                   0U);
}

static void req_boot_setup3(armvis_msg_t *msg)
{
    schedmsg_boot_setup3_t *r = &msg->msg.boot_setup3;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_BOOT_SETUP3: sysflow=%X cvtable=%X\n",
                   r->sysflow_name_daddr,
                   r->cvtable_base_daddr,
                   0U, 0U, 0U);
}

static void rpl_boot_setup3(armvis_msg_t *msg)
{
    UNUSED(msg);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"<--SCHEDMSG_BOOT_SETUP3_REPLY: 0x%X\n", 0U, 0U, 0U, 0U, 0U);
}

static void req_init_setup(armvis_msg_t *msg)
{
    schedmsg_initial_setup_t *r = &msg->msg.initial_setup;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_INITIAL_SETUP: version=(0x%X 0x%X)",
                   r->orc_major_version,
                   r->orc_minor_version,
                   0U, 0U, 0U);
}

static void rpl_init_setup(armvis_msg_t *msg)
{
    schedmsg_initial_setup_reply_t *r = &msg->msg.reply_initial_setup;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "<--SCHEDMSG_INITIAL_SETUP_REPLY: version=(0x%X 0x%X), #cvtask=%d\n",
                   r->arm_major_version,
                   r->arm_minor_version,
                   r->arm_num_cvtask,
                   0U, 0U);
}

static void req_cvtask_reg(armvis_msg_t *msg)
{
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CVTASK_REGISTER:   %d\n",
                   msg->msg.cvtask_register_msg.arm_cvtask_index,
                   0U, 0U, 0U, 0U);
}

static void rpl_cvtask_reg(armvis_msg_t *msg)
{
    module_printS5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "<--SCHEDMSG_CVTASK_REGISTER_REPLY:   %s\n",
                   msg->msg.reply_cvtask_register.cvtask_name,
                   NULL, NULL, NULL, NULL);
}

static void req_cvtask_query(armvis_msg_t *msg)
{
    schedmsg_cvtask_query_t *r = &msg->msg.cvtask_query;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CVTASK_QUERY_REQUEST: (%d %d %d) interface=0x%X\n",
                   r->arm_cvtask_index,
                   r->sysflow_index,
                   r->cvtask_instance,
                   r->cvtask_memory_interface_daddr,
                   0U);
}

static void rpl_cvtask_query(armvis_msg_t *msg)
{
    schedmsg_cvtask_query_reply_t *r = &msg->msg.reply_cvtask_query;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "<--SCHEDMSG_CVTASK_QUERY_REPLY: (%d %d %d) interface=0x%X\n",
                   r->arm_cvtask_index,
                   r->sysflow_index,
                   r->cvtask_instance,
                   r->cvtask_memory_interface_daddr,
                   0U);
}

static void req_cvtask_init(armvis_msg_t *msg)
{
    schedmsg_cvtask_init_t *r = &msg->msg.cvtask_init;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CVTASK_INIT_REQUEST:  (%d %d %d)\n",
                   r->arm_cvtask_index,
                   r->sysflow_index,
                   r->cvtask_instance,
                   0U,0U);
}

static void rpl_cvtask_init(armvis_msg_t *msg)
{
    UNUSED(msg);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"<--SCHEDMSG_CVTASK_INIT_REPLY:\n", 0U, 0U, 0U, 0U, 0U);
}

static void req_cvtask_run(armvis_msg_t *msg)
{
    schedmsg_cvtask_run_t *r = &msg->msg.cvtask_run;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CVTASK_RUN_REQUEST:   (%d %d %d)\n",
                   r->arm_cvtask_index,
                   r->sysflow_index,
                   r->cvtask_instance,
                   0, 0);
}

static void rpl_cvtask_run(armvis_msg_t *msg)
{
    UNUSED(msg);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"<--SCHEDMSG_CVTASK_RUN_REPLY:\n", 0U, 0U, 0U, 0U, 0U);
}

static void req_set_phase(armvis_msg_t *msg)
{
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_SET_BOOTUP_PHASE:    %d\n",
                   msg->msg.set_bootup_phase.bootup_phase,
                   0, 0, 0, 0);
}

static void req_report_memory(armvis_msg_t *msg)
{
    schedmsg_cvscheduler_memory_report_t *r = &msg->msg.memory_report;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CVSCHEDULER_REPORT_MEMORY: 0x%X+0x%X\n",
                   r->armblock_baseaddr, r->armblock_size, 0U, 0U, 0U);
}

static void req_configure_idsp(armvis_msg_t *msg)
{
    schedmsg_configure_idsp_t *r = &msg->msg.idsp_config;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CONFIGURE_IDSP:  id=%d, num=%d, size=%d\n",
                   r->channel_id, r->num_buffers, r->picbuffer_size, 0U, 0U);
}

static void req_configure_vin(armvis_msg_t *msg)
{
    schedmsg_configure_vin_t *r = &msg->msg.vin_config;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CONFIGURE_VIN:  id=%d, num=%d, size=%d [%d x %d]\n",
                   r->vin_id, r->num_buffers, r->picbuffer_size, r->line_width, r->frame_height);
}

static void rpl_cvtaskmsg_send(armvis_msg_t *msg)
{
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "<--SCHEDMSG_CVTASK_MSG_REPLY: target frame=%d\n",
                   msg->msg.reply_cvtask_msg.target_frameset_id, 0U, 0U, 0U, 0U);
}

static void rpl_start_engine(armvis_msg_t *msg)
{
    UNUSED(msg);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"<--SCHEDMSG_CVSCHEDULER_RUN:\n", 0U, 0U, 0U, 0U, 0U);
}

static void req_log_update(armvis_msg_t *msg)
{
    UNUSED(msg);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"-->SCHEDMSG_VISORC_PRINTF_UPDATE:\n", 0U, 0U, 0U, 0U, 0U);
}

static void req_shutdown(armvis_msg_t *msg)
{
    UNUSED(msg);
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"-->SCHEDMSG_SCHEDULER_SHUTDOWN:\n", 0U, 0U, 0U, 0U, 0U);
}

static void req_buildinfo(armvis_msg_t *msg)
{
    schedmsg_cvtask_getbuildinfo_t *r = &msg->msg.cvtask_getbuildinfo;
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHEDMSG_CVTASK_GET_BUILDINFO: cvtask=%d sysflow=%d, maxsize=%d\n",
                   r->arm_cvtask_index,
                   r->sysflow_index,
                   r->versionstring_maxsize,
                   0, 0);
}

static void log_unknown(armvis_msg_t *msg)
{
    module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,
                   "-->SCHDRMSG_UNKNOWN:      0x%X\n", msg->hdr.message_type, 0U, 0U, 0U, 0U);
}

void schdrmsg_log_msg(armvis_msg_t *msg)
{
    switch (msg->hdr.message_type) {
    case SCHEDMSG_BOOT_SETUP1:
        req_boot_setup1(msg);
        break;
    case SCHEDMSG_BOOT_SETUP1_REPLY:
        rpl_boot_setup1(msg);
        break;

    case SCHEDMSG_BOOT_SETUP2:
        req_boot_setup2(msg);
        break;
    case SCHEDMSG_BOOT_SETUP2_REPLY:
        rpl_boot_setup2(msg);
        break;

    case SCHEDMSG_BOOT_SETUP3:
        req_boot_setup3(msg);
        break;
    case SCHEDMSG_BOOT_SETUP3_REPLY:
        rpl_boot_setup3(msg);
        break;

    case SCHEDMSG_CVTASK_REGISTER:
        req_cvtask_reg(msg);
        break;
    case SCHEDMSG_CVTASK_REGISTER_REPLY:
        rpl_cvtask_reg(msg);
        break;

    case SCHEDMSG_CVTASK_QUERY_REQUEST:
        req_cvtask_query(msg);
        break;
    case SCHEDMSG_CVTASK_QUERY_REPLY:
        rpl_cvtask_query(msg);
        break;

    case SCHEDMSG_CVTASK_INIT_REQUEST:
        req_cvtask_init(msg);
        break;
    case SCHEDMSG_CVTASK_INIT_REPLY:
        rpl_cvtask_init(msg);
        break;

    case SCHEDMSG_CVTASK_RUN_REQUEST:
        req_cvtask_run(msg);
        break;
    case SCHEDMSG_CVTASK_RUN_REPLY:
        rpl_cvtask_run(msg);
        break;

    case SCHEDMSG_SET_BOOTUP_PHASE:
        req_set_phase(msg);
        break;

    case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
        req_report_memory(msg);
        break;

    case SCHEDMSG_CONFIGURE_IDSP:
        req_configure_idsp(msg);
        break;

    case SCHEDMSG_CONFIGURE_VIN:
        req_configure_vin(msg);
        break;

    case SCHEDMSG_CVSCHEDULER_RUN:
        rpl_start_engine(msg);
        break;

    case SCHEDMSG_CVTASK_MSG_REPLY:
    case SCHEDMSG_FLEXIDAG_CVTASK_MSG:
        rpl_cvtaskmsg_send(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP:
        req_init_setup(msg);
        break;

    case SCHEDMSG_INITIAL_SETUP_REPLY:
        rpl_init_setup(msg);
        break;

    case SCHEDMSG_VISORC_PRINTF_UPDATE:
        req_log_update(msg);
        break;

    case SCHEDMSG_SCHEDULER_SHUTDOWN:
        req_shutdown(msg);
        break;

    case SCHEDMSG_CVTASK_GET_BUILDINFO:
        req_buildinfo(msg);
        break;

    default:
        log_unknown(msg);
    }
}
