#include <linux/printk.h>
#include "ambacv_kal.h"
#include "schdrmsg_def.h"

static void req_boot_setup1(armvis_msg_t *msg)
{
  schedmsg_boot_setup1_t *r = &msg->msg.boot_setup1;

  pr_info("-->SCHEDMSG_BOOT_SETUP1: version=(0x%X 0x%X)\n",
         r->scheduler_version, r->cvtask_version);
}

static void rpl_boot_setup1(armvis_msg_t *msg)
{
  schedmsg_boot_setup1_reply_t *r = &msg->msg.reply_boot_setup1;

  pr_info("<--SCHEDMSG_BOOT_SETUP1_REPLY: version=(0x%X 0x%X), #cvtask=%d\n",
         r->scheduler_version, r->cvtask_version, r->cvtask_total_num);
  pr_info("                               %X %X %X %X %X %X\n",
         r->memsize_arm_sysflow_block, r->memsize_arm_cvtable_block, r->memsize_printf_string_block,
         r->memsize_arm_cvtask_debug_buffer, r->memsize_arm_sched_debug_buffer, r->memsize_arm_perf_debug_buffer);
}

static void req_boot_setup2(armvis_msg_t *msg)
{
  schedmsg_boot_setup2_t *r = &msg->msg.boot_setup2;

  pr_info("-->SCHEDMSG_BOOT_SETUP2:\n");
  pr_info("        sysflow_addr = 0x%llX\n", ambacv_c2p(r->sysflow_block_daddr));
  pr_info("        cvtable_addr = 0x%llX\n", ambacv_c2p(r->cvtable_block_daddr));
  pr_info("         format_addr = 0x%llX\n", ambacv_c2p(r->printf_string_block_daddr));
  pr_info("       log_info_addr = 0x%llX\n", ambacv_c2p(r->arm_printf_buffer_info_daddr));
  pr_info("         cvtask_addr = 0x%llX\n", ambacv_c2p(r->arm_cvtask_debug_buffer_daddr));
  pr_info("          sched_addr = 0x%llX\n", ambacv_c2p(r->arm_sched_debug_buffer_daddr));
  pr_info("           perf_addr = 0x%llX\n", ambacv_c2p(r->arm_perf_debug_buffer_daddr));
}

static void rpl_boot_setup2(armvis_msg_t *msg)
{
  schedmsg_boot_setup2_reply_t *r = &msg->msg.reply_boot_setup2;

  pr_info("<--SCHEDMSG_BOOT_SETUP2_REPLY: sysflow=(%d,@0x%llx), cvtable=(%d,@0x%llx)\n",
         r->sysflow_table_set_numlines[0], ambacv_c2p(r->sysflow_table_set_daddr[0]),
         r->cvtable_num_sets, ambacv_c2p(r->cvtable_daddr[0]));
}

static void req_boot_setup3(armvis_msg_t *msg)
{
  schedmsg_boot_setup3_t *r = &msg->msg.boot_setup3;
  pr_info("-->SCHEDMSG_BOOT_SETUP3: sysflow=0x%llx cvtable=0x%llx\n",
         ambacv_c2p(r->sysflow_name_daddr), ambacv_c2p(r->cvtable_base_daddr));
}

static void rpl_boot_setup3(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_BOOT_SETUP3_REPLY:\n");
}

static void req_cvtask_reg(armvis_msg_t *msg)
{
  pr_info("-->SCHEDMSG_CVTASK_REGISTER:   %d\n", msg->msg.cvtask_register_msg.arm_cvtask_index);
}

static void rpl_cvtask_reg(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_REGISTER_REPLY:  (%d) : %s\n", msg->msg.reply_cvtask_register.arm_cvtask_index, msg->msg.reply_cvtask_register.cvtask_name);
}

static void req_cvtask_query(armvis_msg_t *msg)
{
  schedmsg_cvtask_query_t *r = &msg->msg.cvtask_query;
  pr_info("-->SCHEDMSG_CVTASK_QUERY_REQUEST: (%d %d %d) interface @0x%llx\n",
         r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance, ambacv_c2p(r->cvtask_memory_interface_daddr));
}

static void rpl_cvtask_query(armvis_msg_t *msg)
{
  schedmsg_cvtask_query_reply_t *r = &msg->msg.reply_cvtask_query;
  pr_info("<--SCHEDMSG_CVTASK_QUERY_REPLY: (%d %d %d) interface @0x%llx\n",
         r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance, ambacv_c2p(r->cvtask_memory_interface_daddr));
}

static void req_cvtask_init(armvis_msg_t *msg)
{
  schedmsg_cvtask_init_t *r = &msg->msg.cvtask_init;
  pr_info("-->SCHEDMSG_CVTASK_INIT_REQUEST:  (%d %d %d) (%d, @0x%llx)\n",
         r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance,
         r->Instance_private_storage_size, ambacv_c2p(r->Instance_private_storage_daddr));
}

static void rpl_cvtask_init(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_INIT_REPLY:\n");
}

static void req_cvtask_finish(armvis_msg_t *msg)
{
  schedmsg_cvtask_run_t *r = &msg->msg.cvtask_run;
  pr_info("-->SCHEDMSG_CVTASK_FINISH_REQUEST:   (%d %d %d) (%d,0x%llX)\n",
         r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance,
         r->Instance_private_storage_size, ambacv_c2p(r->Instance_private_storage_daddr));
}

static void rpl_cvtask_finish(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_FINISH_REPLY:");
}

static void req_cvtask_run(armvis_msg_t *msg)
{
  schedmsg_cvtask_run_t *r = &msg->msg.cvtask_run;
  pr_info("-->SCHEDMSG_CVTASK_RUN_REQUEST:   (%d %d %d) (%d,0x%llX)\n",
         r->arm_cvtask_index, r->sysflow_index, r->cvtask_instance,
         r->Instance_private_storage_size, ambacv_c2p(r->Instance_private_storage_daddr));
}

static void rpl_cvtask_run(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_RUN_REPLY:\n");
}

static void rpl_cvtask_partial_run(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY: ind[%d] daddr[0x%x]\n",
         msg->msg.reply_cvtask_run_partial.output_index, msg->msg.reply_cvtask_run_partial.output_membuf_daddr);
}

static void req_set_phase(armvis_msg_t *msg)
{
  pr_info("-->SCHEDMSG_SET_BOOTUP_PHASE:    %d\n",
         msg->msg.set_bootup_phase.bootup_phase);
}

static void req_init_setup(armvis_msg_t *msg)
{
  pr_info("-->SCHEDMSG_INITIAL_SETUP\n");
}

static void rpl_init_setup(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_INITIAL_SETUP_REPLY\n");
}

static void req_report_memory(armvis_msg_t *msg)
{
  schedmsg_cvscheduler_memory_report_t *r = &msg->msg.memory_report;
  pr_info("-->SCHDRMSG_REPORT_MEM:   0x%llx+0x%x, 0x%llx+0x%x, 0x%llx+0x%x, 0x%llx\n",
         ambacv_c2p(r->orc_cached_baseaddr), r->orc_cached_size,
         ambacv_c2p(r->orc_uncached_baseaddr), r->orc_uncached_size,
         ambacv_c2p(r->armblock_baseaddr), r->armblock_size, ambacv_c2p(r->CVTaskMsgPool_daddr));
}

static void rpl_cvtaskmsg_send(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_MSG_REPLY: target frameset_id=%d\n", msg->msg.reply_cvtask_msg.target_frameset_id);
}

static void rpl_start_engine(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVSCHEDULER_RUN\n");
}

static void req_cvtask_get_buildinfo(armvis_msg_t *msg)
{
  pr_info("-->SCHEDMSG_CVTASK_GET_BUILDINFO: %d\n",
         msg->msg.cvtask_getbuildinfo.sysflow_index);
}

static void rpl_cvtask_get_buildinfo(armvis_msg_t *msg)
{
  pr_info("<--SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY: %d\n",
         msg->msg.reply_cvtask_getbuildinfo.sysflow_index);
}

static void req_softreset(armvis_msg_t *msg)
{
  schedmsg_softreset_t *r = &msg->msg.hw_reset;
  pr_info("-->SCHEDMSG_HWUNIT_RESET: %d\n", r->hardware_unit);
}

static void rpl_softreset(armvis_msg_t *msg)
{
  schedmsg_softreset_t *r = &msg->msg.reply_hw_reset;
  pr_info("<--SCHEDMSG_HWUNIT_RESET_REPLY: %d\n", r->hardware_unit);
}

static void req_log_update(armvis_msg_t *msg)
{
  schedmsg_cvlog_update_t *r = &msg->msg.cvlog_update;
  pr_info("-->SCHEDMSG_VISORC_PRINTF_UPDATE 0x%llX\n", ambacv_c2p(r->CVStaticLogInfo_daddr));
}

static void req_startup_log_update(armvis_msg_t *msg)
{
  schedmsg_cvlog_update_t *r = &msg->msg.cvlog_update;
  pr_info("-->SCHEDMSG_VISORC_STARTUP_UPDATE 0x%llX\n", ambacv_c2p(r->CVStaticLogInfo_daddr));
}

static void req_shutdown(armvis_msg_t *msg)
{
  schedmsg_scheduler_shutdown_t *r = &msg->msg.shutdown;
  pr_info("-->SCHEDMSG_SCHEDULER_SHUTDOWN 0x%llX 0x%X\n",
         ambacv_c2p(r->CVStaticLogInfo_daddr), r->shutdown_retcode);
}

static void log_unknown(armvis_msg_t *msg)
{
	pr_info("---SCHDRMSG_UNKNOWN:      0x%X\n", msg->hdr.message_type);
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

	case SCHEDMSG_CVTASK_FINISH_REQUEST:
		req_cvtask_finish(msg);
		break;
	case SCHEDMSG_CVTASK_FINISH_REPLY:
		rpl_cvtask_finish(msg);
		break;

	case SCHEDMSG_CVTASK_RUN_REQUEST:
	case SCHEDMSG_CVTASK_MSGONLY_REQUEST:
		req_cvtask_run(msg);
		break;
	case SCHEDMSG_CVTASK_RUN_REPLY:
	case SCHEDMSG_CVTASK_MSGONLY_REPLY:
		rpl_cvtask_run(msg);
		break;
	case SCHEDMSG_CVTASK_RUN_PARTIAL_REPLY:
		rpl_cvtask_partial_run(msg);
		break;

	case SCHEDMSG_CVTASK_GET_BUILDINFO:
		req_cvtask_get_buildinfo(msg);
		break;

	case SCHEDMSG_CVTASK_GET_BUILDINFO_REPLY:
		rpl_cvtask_get_buildinfo(msg);
		break;

	case SCHEDMSG_SET_BOOTUP_PHASE:
		req_set_phase(msg);
		break;

	case SCHEDMSG_CVSCHEDULER_REPORT_MEMORY:
		req_report_memory(msg);
		break;

	case SCHEDMSG_CVSCHEDULER_RUN:
		rpl_start_engine(msg);
		break;

	case SCHEDMSG_CVTASK_MSG_REPLY:
		rpl_cvtaskmsg_send(msg);
		break;

	case SCHEDMSG_INITIAL_SETUP:
		req_init_setup(msg);
		break;

	case SCHEDMSG_INITIAL_SETUP_REPLY:
		rpl_init_setup(msg);
		break;

	case SCHEDMSG_HWUNIT_RESET:
		req_softreset(msg);
		break;

	case SCHEDMSG_HWUNIT_RESET_REPLY:
		rpl_softreset(msg);
		break;

	case SCHEDMSG_VISORC_PRINTF_UPDATE:
		req_log_update(msg);
		break;

	case SCHEDMSG_VISORC_STARTUP_UPDATE:
		req_startup_log_update(msg);
		break;

	case SCHEDMSG_SCHEDULER_SHUTDOWN:
		req_shutdown(msg);
		break;

	default:
		log_unknown(msg);
	} /* switch (msg->hdr.message_type) */
}
