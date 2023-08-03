#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include "ambacv_kal.h"
#include "flexidag_ioctl.h"
#include <cvsched_api.h>

MODULE_AUTHOR("Joey Li");
MODULE_LICENSE("GPL");

#define MSG_INDEX_MASK          (ARMVIS_MESSAGE_NUM - 1)

static struct task_struct *rx_kthread;
static struct semaphore    rx_lock;
static struct mutex        tx_lock;
static struct mutex        find_lock;
static wait_queue_head_t   msg_wq;
static int                 is_booting_os;
static uint32_t            visorc_rewake_time = 0U;  /* zero is disabled, non-zero means next wake point for the VISORC in audio ticks */

typedef struct {
	unsigned int*           from_orc_r;
	unsigned int*           from_orc_w;
	armvis_msg_t*           from_orc_q;
	unsigned int            from_orc_wi;
	uint32_t                from_orc_w_seqno;

	unsigned int*           from_arm_r;
	unsigned int*           from_arm_w;
	armvis_msg_t*           from_arm_q;
	uint32_t                from_arm_w_seqno;

	uint64_t                reqQ_addr;
	uint64_t                rplQ_addr;
} q_config_t;

static q_config_t  qconfig;
uint32_t audio_clock_ms = 12288U;
uint32_t vision_clock_ms = 792000U;

extern struct ambarella_cavalry *pCavalrySupport;
#include "cavalry.h"

extern void scheduler_system_set_trace_daddr(uint64_t daddr);
extern void scheduler_autorun_set_trace_daddr(uint64_t trace_daddr);
extern uint32_t debug_level;
extern void ambacv_release_token(uint32_t token_id, errcode_enum_t function_retcode);

static struct mutex event_time_lock;
static uint32_t  event_time_ctrl = 0U;

static void process_kernel_schdrmsg(armvis_msg_t *msg)
{
	switch (msg->hdr.message_type) {
	case SCHEDMSG_HWUNIT_RESET:
		visorc_softreset(msg->msg.hw_reset.hardware_unit);
		schdrmsg_send_armmsg((unsigned long)msg, 1);
		break;

	case SCHEDMSG_BOOT_SETUP1:
		scheduler_system_set_trace_daddr(msg->msg.boot_setup1.scheduler_trace_daddr);
		scheduler_autorun_set_trace_daddr(msg->msg.boot_setup1.autorun_trace_daddr);
		krn_flexidag_system_set_trace_daddr(msg->msg.boot_setup1.flexidag_trace_daddr);
		krn_cavalry_system_set_trace_daddr(pCavalrySupport, msg->msg.boot_setup1.cavalry_trace_daddr);
		break;

	case SCHEDMSG_CAVALRY_VP_RUN_REPLY:
	case SCHEDMSG_CAVALRY_HL_RUN_REPLY:
	case SCHEDMSG_CAVALRY_FEX_QUERY_REPLY:
	case SCHEDMSG_CAVALRY_FEX_RUN_REPLY:
	case SCHEDMSG_CAVALRY_FMA_QUERY_REPLY:
	case SCHEDMSG_CAVALRY_FMA_RUN_REPLY:
		ioctl_cavalry_handle_visorc_reply(pCavalrySupport, msg);
		break;

	case SCHEDMSG_FLEXIDAG_CREATE_REPLY:
	case SCHEDMSG_FLEXIDAG_OPEN_REPLY:
	case SCHEDMSG_FLEXIDAG_OPEN_PARSED:
	case SCHEDMSG_FLEXIDAG_INIT_REPLY:
	case SCHEDMSG_FLEXIDAG_OUTPUT_DONE:
	case SCHEDMSG_FLEXIDAG_RUN_REPLY:
	case SCHEDMSG_FLEXIDAG_CLOSE_REPLY:
		krn_flexidag_ioctl_handle_message(msg);
		break;

	case SCHEDMSG_SET_REWAKE_TIME:
		if (msg->msg.set_rewake_time.rewake_time_delta == 0U)
		{
			visorc_rewake_time = 0U;
		} /* if (msg->set_rewake_time.rewake_time_delta == 0U) */
		else /* if (msg->set_rewake_time.rewake_time_delta != 0U) */
		{
			uint32_t curr_time = visorc_get_curtime();
			visorc_rewake_time = ((curr_time + msg->msg.set_rewake_time.rewake_time_delta) - (curr_time - msg->msg.set_rewake_time.sent_time)) | 1U;
		} /* if (msg->set_rewake_time.rewake_time_delta != 0U) */
		break;

	case SCHEDMSG_FLUSH_VISPRINTF_REPLY:
		ambacv_release_token(msg->msg.flush_visprintf_reply.token_id, msg->hdr.message_retcode);
		break;

	} /* switch (msg->hdr.message_type) */
}

static uint32_t schdrmsg_update_checksum(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;
    uint32_t hash = FNV1A_32_INIT_HASH;

#ifdef ASIL_COMPLIANCE
    if(msg->hdr.message_checksum != 0U) {
        hash = schdrmsg_calc_hash(&msg->hdr, 12, hash);
        hash = schdrmsg_calc_hash(&msg->msg, sizeof(msg->msg), hash);
        msg->hdr.message_checksum = hash;
        ambacv_cache_clean((char*)&msg->hdr, sizeof(msg->hdr));
        ambacv_cache_invalidate((char*)&msg->hdr, sizeof(msg->hdr));
    }
#endif /* ?ASIL_COMPLIANCE */

    return retcode;
}

static uint32_t schdrmsg_check(armvis_msg_t *msg)
{
    uint32_t retcode = ERRCODE_NONE;

    if(msg->hdr.message_seqno != qconfig.from_orc_w_seqno) {
        retcode = ERR_DRV_SCHDR_VISMSG_SEQNO_MISMATCHED;
    }
#ifdef ASIL_COMPLIANCE
    else
    {
        uint32_t hash = FNV1A_32_INIT_HASH;

        if(msg->hdr.message_checksum != 0U) {
            hash = schdrmsg_calc_hash(&msg->hdr, 12, hash);
            hash = schdrmsg_calc_hash(&msg->msg, sizeof(msg->msg), hash);
            if(hash != msg->hdr.message_checksum) {
                retcode = ERR_DRV_SCHDR_VISMSG_CHECKSUM_MISMATCHED;
            }
        }
    }
#endif /* ?ASIL_COMPLIANCE */

    qconfig.from_orc_w_seqno++;
    return retcode;
}

static void invalidate_orc_schdrmsg(void)
{
	uint32_t retcode = ERRCODE_NONE;
	armvis_msg_t *msg;
	unsigned index, wi;

	ambacv_cache_invalidate((char*)qconfig.from_orc_w, sizeof(int));
	ambacv_cache_barrier();
	wi = *qconfig.from_orc_w;
	index = qconfig.from_orc_wi;

	while (index != wi) {
		// get one armvis_msg_t from the queue
		msg = &(qconfig.from_orc_q[index & MSG_INDEX_MASK]);
		cache_invld_schdrmsg(msg, 0);
		retcode = schdrmsg_check(msg);
		if(retcode != ERRCODE_NONE) {
			pr_err("[ERROR] invalidate_orc_schdrmsg() : schdrmsg_check fail ret 0x%x \n", retcode);
		} else {
			cache_invld_schdrmsg(msg, 1);
			process_kernel_schdrmsg(msg);
			if(msg->hdr.message_type == SCHEDMSG_FLEXIDAG_OUTPUT_DONE) {
				schdrmsg_update_checksum(msg);
			}
		}
		index++;
	}
	qconfig.from_orc_wi = wi;

	// wake-up waiting AMBACV_RECV_ORCMSG ioctl
	wake_up_interruptible(&msg_wq);
}

void schdrmsg_set_event_time(void)
{
#if defined (CHIP_CV6)
	uint32_t base_event_time;
	uint32_t  audclk_time;
	uint32_t  extclk_time;

	base_event_time = visorc_get_curtime();
	mutex_lock(&event_time_lock);

	base_event_time = base_event_time >>6U;
	audclk_time = (base_event_time << 6U);
	extclk_time = (base_event_time >> 26U);
	event_time_ctrl = extclk_time | ((audclk_time >> 25U) & 0x40U);

	mutex_unlock(&event_time_lock);
#endif
}

uint32_t schdrmsg_get_event_time(void)
{
	uint32_t  retval = 0U;
#if defined (CHIP_CV6)
	uint32_t  curtime;
	uint32_t  ctrlval;
	uint32_t  flag;

	mutex_lock(&event_time_lock);

	curtime = visorc_get_curtime();
	ctrlval = event_time_ctrl;
	flag    = ((curtime >> 31U) | (ctrlval & 0x40U));

	if (flag == 0x40U) {
		ctrlval = (ctrlval + 1U) & 0x3F;
		event_time_ctrl = ctrlval;
	} else if (flag == 1U) {
		ctrlval |= 0x40;
		event_time_ctrl = ctrlval;
	} else { /* if ((flag == 0U) || (flag == 0x41U)) */
		; /* Do nothing, MISRA 15.7 */
	} /* if ((flag == 0U) || (flag == 0x41U)) */
	retval  = curtime >> 6;
	retval |= (ctrlval & 0x3FU) << 26;

	mutex_unlock(&event_time_lock);
#else
	retval = visorc_get_curtime();
#endif

	return retval;
}

static void schdrmsg_report_time(void)
{
  uint64_t now;
  uint32_t sec,msec;
  armvis_msg_t msgbase = {0};
  schedmsg_time_report_t *pMsg;
  uint32_t event_time;

  event_time = schdrmsg_get_event_time();
  now = ktime_to_ms(ktime_get_real());
  if(now != 0U) {
    sec = (uint32_t)(now/1000U);
    msec = (uint32_t)(now%1000U);
  }  else {
    sec = 0U;
    msec = 0U;
  }

  msgbase.hdr.message_type  = (uint16_t)SCHEDMSG_TIME_REPORT;
  pMsg = &msgbase.msg.time_report;
  pMsg->linux_epoch_time = sec;
  pMsg->linux_mstime = msec;
  pMsg->linux_event_time = event_time;
  if (schdrmsg_send_armmsg(&msgbase, 1U) != 0U) {
    printk("[ERROR] schdrmsg_report_time() : schdrmsg_send_armmsg fail \n", 0U, 0U, 0U, 0U, 0U);
  }
}

static void schdrmsg_rewake_report_time(void)
{
  static uint32_t report_period;
  static uint32_t last_report_time = 0U;
  uint32_t current_time;
  uint32_t  atick_delta;

  if(ambacv_state == 1U) {
    current_time = visorc_get_curtime();
    if(last_report_time == 0U) {
      report_period = (100U * audio_clock_ms);
      last_report_time = current_time;
    } else {
      atick_delta = (uint32_t)((last_report_time + report_period) - current_time);
      if (atick_delta >= 0x80000000U) { /* Expired */
        schdrmsg_report_time();
        last_report_time = current_time;
      } /* if (atick_delta >= 0x80000000) */
      else if (atick_delta >= (100U * audio_clock_ms)) { /* Too far away */
        schdrmsg_report_time();
        last_report_time = current_time;
      } /* if (atick_delta >= (100U * audio_clock_ms)) */
      else { /* if (atick_delta < (100U * audio_clock_ms)) : Under default 100ms tick*/
        /* Do nothing */
      } /* if (atick_delta < (100U * audio_clock_ms)) */
    }
  }
}

static int schdrmsg_rx_entry(void *arg)
{
	int ret;
	uint32_t num_ms_wait;

	for (;;) {
		if (visorc_rewake_time != 0U)
		{
			uint32_t  ctime;
			uint32_t  atick_delta;

			ctime = visorc_get_curtime(); /* Adjustment*/
			atick_delta = (uint32_t)(visorc_rewake_time - ctime);

			if (atick_delta >= 0x80000000U) /* Expired */
			{
				num_ms_wait         = 100U;
				visorc_rewake_time  = 0;
			} /* if (atick_delta >= 0x80000000) */
			else if (atick_delta >= (100U * audio_clock_ms)) /* Too far away */
			{
				num_ms_wait = 100;
			} /* if (atick_delta >= (100U * audio_clock_ms)) */
			else /* if (atick_delta < (100U * audio_clock_ms)) : Under default 100ms tick*/
			{
				num_ms_wait = (atick_delta + (audio_clock_ms -1)) / audio_clock_ms;
				if (num_ms_wait < 2) /* Clip to 2ms minimum wait */
				{
					num_ms_wait = 2;
				} /* if (num_ms_wait < 2) */
			} /* if (atick_delta < (100U * audio_clock_ms)) */
		} /* if (autorewake_time != 0U) */
		else /* if (autorewake_time == 0U) */
		{
			num_ms_wait = 100;
		} /* if (autorewake_time == 0U) */

		ret = down_timeout(&rx_lock, msecs_to_jiffies(num_ms_wait));

		if (ret != 0) {
			if (kthread_should_stop()) {
				pr_info("ambacv: exit schdrmsg_rx thread\n");
				do_exit(0);
			} else if(is_booting_os) {
				/* kick ORC after timeout */
				visorc_kick();
			}
		}
		/* invalidate all incoming messages */
		invalidate_orc_schdrmsg();
		schdrmsg_rewake_report_time();
	}

	return 0;
}

irqreturn_t ambacv_isr(int irq, void *devid)
{
	up(&rx_lock);
	return IRQ_HANDLED;
}

extern int enable_cma;
extern int enable_arm_cache;
extern int enable_rtos_cache;
extern int enable_orc_cache;

int schdrmsg_get_sysconfig(unsigned long arg)
{
	visorc_init_params_t local;
	armvis_message_queue_t *q;

	schdrmsg_shutdown();

	/* reset queue config */
	qconfig.from_orc_wi = 0;
	qconfig.from_orc_w_seqno = 0U;
	qconfig.from_arm_w_seqno = 0U;
	q = (armvis_message_queue_t*)ambacv_c2v(qconfig.reqQ_addr);
	q->rd_idx = q->wr_idx = 0;
	ambacv_cache_clean(q, sizeof(*q));
	q = (armvis_message_queue_t*)ambacv_c2v(qconfig.rplQ_addr);
	q->rd_idx = q->wr_idx = 0;
	ambacv_cache_clean(q, sizeof(*q));

	/* return info back to user */
	/* first 6MB is reserved for orc binaries */
	/*  last 1MB is reserved for arm-orc messag queue */
	memset(&local, 0, sizeof(local));
	local.sysinit_dram_block_baseaddr = ambacv_global_mem.all.base + VISORC_MEM_REGION_SIZE;
	local.sysinit_dram_block_size     = ambacv_global_mem.all.size - VISORC_MEM_REGION_SIZE - SCHDRMSG_MEM_REGION_SIZE;

	/* use copressor_cfg to pass size of shdrmsg region to user */
	local.coprocessor_cfg = SCHDRMSG_MEM_REGION_SIZE;

	local.MsgQueue_orc_to_arm_daddr[scheduler_id] = qconfig.reqQ_addr;
	local.MsgQueue_arm_to_orc_daddr[scheduler_id] = qconfig.rplQ_addr;
	local.kernel_region_base_daddr  = local.sysinit_dram_block_baseaddr + local.sysinit_dram_block_size;
	local.kernel_region_size        = SCHDRMSG_MEM_REGION_SIZE;
	if (enable_cma == 0)
	{
		local.cma_region_daddr  = 0;
		local.cma_region_size   = 0;
	}
	else
	{
		local.cma_region_daddr  = (pCavalrySupport->cavalry_memblock_base + pCavalrySupport->cma_private.size);
		local.cma_region_size   = (pCavalrySupport->cavalry_memblock_size - pCavalrySupport->cma_private.size);
	}
	local.rtos_region_daddr = ambacv_global_mem.rtos_region.base;
	local.rtos_region_size  = ambacv_global_mem.rtos_region.size;
	local.region_cache_flags = 0;
	local.region_cache_flags |= (enable_arm_cache != 0) ? CACHE_FLAG_SUPERDAG : 0;
	local.region_cache_flags |= (enable_orc_cache != 0) ? CACHE_FLAG_VISORC : 0;
	local.region_cache_flags |= (enable_rtos_cache != 0) ? CACHE_FLAG_RTOS : 0;

	return copy_to_user((void*)arg, &local, sizeof(local));
}

int schdrmsg_get_clock(visorc_init_params_t *sysconfig)
{
	struct clk *gclk = NULL;
	uint32_t clock_ms;

	gclk = clk_get_sys(NULL, "gclk_audio");
	if(gclk != NULL) {
		audio_clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_audio_ticks_per_ms = audio_clock_ms;
		pr_info("ambacv : audio clock %d \n", audio_clock_ms*1000);
	} else {
		pr_info("ambacv : audio clock not found \n");
	}

#if defined(CHIP_CV6)
	gclk = NULL;
	gclk = clk_get_sys(NULL, "gclk_hsm");
	if(gclk != NULL) {
		vision_clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_visorc_ticks_per_ms = vision_clock_ms;
		pr_info("ambacv : vision clock %d \n", vision_clock_ms*1000);
	} else {
		pr_info("ambacv : vision clock not found \n");
	}

	gclk = NULL;
	gclk = clk_get_sys(NULL, "gclk_nvp");
	if(gclk != NULL) {
		clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_nvp_ticks_per_ms = clock_ms;
		pr_info("ambacv : nvp clock %d \n", clock_ms*1000);
	} else {
		pr_info("ambacv : nvp clock not found \n");
	}

	gclk = NULL;
	gclk = clk_get_sys(NULL, "gclk_gvp");
	if(gclk != NULL) {
		clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_gvp_ticks_per_ms = clock_ms;
		pr_info("ambacv : gvp clock %d \n", clock_ms*1000);
	} else {
		pr_info("ambacv : gvp clock not found \n");
	}

	gclk = NULL;
	gclk = clk_get_sys(NULL, "gclk_fex");
	if(gclk != NULL) {
		clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_fex_ticks_per_ms = clock_ms;
		pr_info("ambacv : fex clock %d \n", clock_ms*1000);
	} else {
		pr_info("ambacv : fex clock not found \n");
	}

	gclk = NULL;
	gclk = clk_get_sys(NULL, "gclk_fma");
	if(gclk != NULL) {
		clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_fma_ticks_per_ms = clock_ms;
		pr_info("ambacv : fma clock %d \n", clock_ms*1000);
	} else {
		pr_info("ambacv : fma clock not found \n");
	}
#else
	gclk = NULL;
	gclk = clk_get_sys(NULL, "gclk_vision");
	if(gclk != NULL) {
		vision_clock_ms = (clk_get_rate(gclk) + 500U)/1000U; /* rounding */
		sysconfig->chip_visorc_ticks_per_ms = vision_clock_ms;
		pr_info("ambacv : vision clock %d \n", vision_clock_ms*1000);
	} else {
		pr_info("ambacv : vision clock not found \n");
	}
#endif

	return 0;
}

int schdrmsg_start_visorc(unsigned long arg)
{
	visorc_init_params_t *sysconfig, local;
	volatile uint32_t *slock;
	int i, ret;
	struct clk *gclk = NULL;

	sysconfig = ambacv_get_sysinit_addr();
	slock = (volatile uint32_t *) &sysconfig->rtos_spin_lock;

	ret = copy_from_user((void*)&local, (void*)arg, sizeof(local));
	if (ret != 0)
		return ret;
	is_booting_os = local.coprocessor_cfg & 0xFF;

	if (is_booting_os) {
		memset(sysconfig, 0, sizeof(*sysconfig));
		sysconfig->sysinit_dram_block_baseaddr  = local.sysinit_dram_block_baseaddr;
		sysconfig->sysinit_dram_block_size      = local.sysinit_dram_block_size;
		sysconfig->coprocessor_cfg              = local.coprocessor_cfg & 0xFF;
		sysconfig->debug_buffer_sod_cvtask_size = local.debug_buffer_sod_cvtask_size;
		sysconfig->debug_buffer_vp_cvtask_size  = local.debug_buffer_vp_cvtask_size;
		sysconfig->debug_buffer_sod_sched_size  = local.debug_buffer_sod_sched_size;
		sysconfig->debug_buffer_vp_sched_size   = local.debug_buffer_vp_sched_size;
		sysconfig->debug_buffer_sod_perf_size   = local.debug_buffer_sod_perf_size;
		sysconfig->debug_buffer_vp_perf_size    = local.debug_buffer_vp_perf_size;
		sysconfig->hotlink_region_daddr         = local.hotlink_region_daddr;
		sysconfig->hotlink_region_size          = local.hotlink_region_size;
		sysconfig->cavalry_base_daddr           = local.cavalry_base_daddr;
		sysconfig->cavalry_num_slots            = local.cavalry_num_slots;
		sysconfig->flexidag_num_slots           = local.flexidag_num_slots;
		sysconfig->flexidag_slot_size           = local.flexidag_slot_size;
		sysconfig->cma_region_daddr             = local.cma_region_daddr;
		sysconfig->cma_region_size              = local.cma_region_size;
		sysconfig->rtos_region_daddr            = local.rtos_region_daddr;
		sysconfig->rtos_region_size             = local.rtos_region_size;
		sysconfig->region_cache_flags           = local.region_cache_flags;
		sysconfig->kernel_region_base_daddr     = local.kernel_region_base_daddr;
		sysconfig->kernel_region_size           = local.kernel_region_size;

		ambacv_global_mem.hotlink_region.base   = local.hotlink_region_daddr;
		ambacv_global_mem.hotlink_region.size   = local.hotlink_region_size;
		ambacv_global_mem.rtos_region.base      = local.rtos_region_daddr;
		ambacv_global_mem.rtos_region.size      = local.rtos_region_size;

		sysconfig->disable_startup_dram_clear   = local.disable_startup_dram_clear;
		sysconfig->disable_cache                = (enable_orc_cache) ? 0 : 0x4D15AB1E;
		sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id]  = qconfig.reqQ_addr;
		sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id]  = qconfig.rplQ_addr;

		// Propagate autorun values
		sysconfig->autorun_interval_in_ms       = local.autorun_interval_in_ms;
		sysconfig->autorun_fextest_config       = local.autorun_fextest_config;
#if defined(CHIP_CV1)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV1 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV2)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV2 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV22)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV22 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV25)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV25 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV28)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV28 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV2A) || defined(CHIP_CV2FS)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV2A | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV22A) || defined(CHIP_CV22FS)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV22A | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV5)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV5 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV52)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV52 | CVSCHEDULER_KERNEL_VERSION);
#elif defined(CHIP_CV6)
		sysconfig->cvchip_and_cvkversion        = (CVCHIP_CV6 | CVSCHEDULER_KERNEL_VERSION);
#else
#error "unsupported CHIP ID, please check your PROJECT setting"
#endif
		sysconfig->cvsched_and_cvtask_api_ver   = (((uint32_t)CVSCHEDULER_INTERFACE_VERSION <<16) | CVTASK_API_VERSION);

		schdrmsg_get_clock(sysconfig);

		/* release spin-lock so other sub-schedulers can continue */
		*slock = 0xBABEFACE;
		(void) ambacv_cache_clean(sysconfig, sizeof(*sysconfig));

		/* wait for all reqQ/rplQ becoming ready */
		pr_info("Waiting for %d sub-schedulers\n",
		       sysconfig->coprocessor_cfg);
		while (true) {
			volatile uint32_t *ptr0, *ptr1;

			(void) ambacv_cache_invalidate(&sysconfig->MsgQueue_orc_to_arm_daddr[0], SYSTEM_MAX_NUM_CORTEX * sizeof(sysconfig->MsgQueue_orc_to_arm_daddr[0]));
			(void) ambacv_cache_invalidate(&sysconfig->MsgQueue_arm_to_orc_daddr[0], SYSTEM_MAX_NUM_CORTEX * sizeof(sysconfig->MsgQueue_orc_to_arm_daddr[0]));
			for (i = ret = 0; i < SYSTEM_MAX_NUM_CORTEX; i++) {
				ptr0 = (volatile uint32_t*) &sysconfig->MsgQueue_orc_to_arm_daddr[i];
				ptr1 = (volatile uint32_t*) &sysconfig->MsgQueue_arm_to_orc_daddr[i];
				if (*ptr0 != 0 && *ptr1 != 0) {
					ret++;
				}
			}
			if (ret == sysconfig->coprocessor_cfg)
				break;
			msleep(10);
		}

		/* reset spin lock as soon as we can */
		*slock = 0;
		(void) ambacv_cache_clean(&sysconfig->rtos_spin_lock, sizeof(sysconfig->rtos_spin_lock));

		/* Reset internal flexidag kernel state */
		krn_flexidag_system_reset(0);

		if (enable_arm_cache != 0)
		{
			pr_info("Explicitly clearing VISORC area from cache [0x%llx - 0x%llx]\n", ambacv_c2p(sysconfig->sysinit_dram_block_baseaddr), ambacv_c2p(sysconfig->sysinit_dram_block_baseaddr) + sysconfig->sysinit_dram_block_size - 1);
			ambacv_cache_invalidate(ambacv_c2v(sysconfig->sysinit_dram_block_baseaddr), sysconfig->sysinit_dram_block_size);
		} /* if (enable_arm_cache != 0) */

		/* trigger ORC to start */
		if(sysconfig->flexidag_num_slots != local.flexidag_num_slots) {
			pr_err("[ERROR] flexidag slot is not the same, sys slot (%d), local slot (%d)\n", sysconfig->flexidag_num_slots, local.flexidag_num_slots);
			return -1;
		}
		if(sysconfig->cavalry_num_slots != local.cavalry_num_slots) {
			pr_err("[ERROR] cavalry slot is not the same, sys slot (%d), local slot (%d)\n", sysconfig->cavalry_num_slots, local.cavalry_num_slots);
			return -1;
		}
		pr_info("start visorc\n");
		sysconfig->init_params_fnv1a_hash = schdrmsg_calc_hash(sysconfig, sizeof(visorc_init_params_t) - 4U, FNV1A_32_INIT_HASH);
		ambacv_cache_clean(&sysconfig->init_params_fnv1a_hash, sizeof(sysconfig->init_params_fnv1a_hash));
		visorc_start(local.coprocessor_cfg & 0x100, sysconfig);
		visorc_kick();

	} else {
		(void) ambacv_cache_invalidate(sysconfig, sizeof(*sysconfig));
		/* wait for spin-lock from booting sub-scheduler */
		pr_info("waiting for spin lock\n");
		while (*slock != 0xBABEFACE) {
			msleep(10);
			(void) ambacv_cache_invalidate(&sysconfig->rtos_spin_lock, sizeof(sysconfig->rtos_spin_lock));
		}
		pr_info("setup message queue\n");
		sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id] = qconfig.reqQ_addr;
		sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id] = qconfig.rplQ_addr;
		(void) ambacv_cache_clean(&sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id], sizeof(sysconfig->MsgQueue_orc_to_arm_daddr[scheduler_id]));
		(void) ambacv_cache_clean(&sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id], sizeof(sysconfig->MsgQueue_arm_to_orc_daddr[scheduler_id]));
	}
	schdrmsg_set_event_time();

	/* start armvis_msg_t recevier thread */
	rx_kthread = kthread_run(schdrmsg_rx_entry, NULL, "schdrmsg_rx");
	return 0;
}

int schdrmsg_resume_visorc(unsigned long arg)
{
	int ret;
	visorc_init_params_t *sysconfig;

	sysconfig = ambacv_get_sysinit_addr();

	if (debug_level > 0)
	{
		pr_info("[CVFLOW] : Restoring kernel sysconfig (%p -> %p)\n", sysconfig, (void *)arg);
	} /* if (debug_level > 0) */

	ret = copy_to_user((void*)arg, sysconfig, sizeof(*sysconfig));

	if (ret != 0) return ret;

	return 0;
}

int  schdrmsg_stop_visorc(void)
{
	visorc_stop();
	schdrmsg_shutdown();
	return 0;
}

uint32_t schdrmsg_calc_hash(void* buf, uint32_t len, uint32_t hval)
{
    uint8_t *ptr = NULL;
    uint32_t count = 0U;
    uint32_t new_hval = 0U;

    (void) ptr;
    (void) buf;
    if((buf != NULL) && (len != 0U)) {
        ptr = (uint8_t *)buf;
        new_hval = hval;
        while (count < len) {
            new_hval = new_hval ^(uint32_t)ptr[count];
            new_hval = new_hval + (new_hval<<1) + (new_hval<<4) + (new_hval<<7) + (new_hval<<8) + (new_hval<<24);
            count++;
        }
    }
    return new_hval;
}

int  schdrmsg_recv_orcmsg(unsigned long arg)
{
	int32_t ret = 0;
	uint32_t ri;
	ambacv_get_value_t karg;

	ret = copy_from_user(&karg, (void*)arg, sizeof(ambacv_get_value_t));
	ri = karg.index;
	/* update read index of request queue */
	*qconfig.from_orc_r = ri;
	ambacv_cache_clean(qconfig.from_orc_r, sizeof(*qconfig.from_orc_r));

	/* wait until we have new request message */
	wait_event_interruptible_timeout(msg_wq, ri < qconfig.from_orc_wi, 100);
	karg.value = qconfig.from_orc_wi;
	ret = copy_to_user((void*)arg, &karg, sizeof(ambacv_get_value_t));
	return ret;
}

uint32_t  schdrmsg_get_orcarm_wi(void)
{
  return qconfig.from_orc_wi;
} /* schdrmsg_get_orcarm_wi() */

int schdrmsg_send_armmsg(unsigned long arg, int is_from_kernel_space)
{
	uint32_t wi, ri;
	armvis_msg_t *dst;
	uint32_t hash = FNV1A_32_INIT_HASH;

	mutex_lock(&tx_lock);
	ambacv_cache_invalidate((char*)qconfig.from_arm_r, sizeof(*qconfig.from_arm_r));
	ambacv_cache_invalidate((char*)qconfig.from_arm_w, sizeof(*qconfig.from_arm_w));
	ambacv_cache_barrier();

	ri = *qconfig.from_arm_r;
	wi = *qconfig.from_arm_w;

	while (wi - ri >= ARMVIS_MESSAGE_NUM) {
		pr_info("arm->orc message full %d %d\n", ri, wi);
		msleep(10);
		ambacv_cache_invalidate((char*)qconfig.from_arm_r, sizeof(*qconfig.from_arm_r));
		ambacv_cache_barrier();
		ri = *qconfig.from_arm_r;
	}

	dst = &(qconfig.from_arm_q[wi & MSG_INDEX_MASK]);
	if (is_from_kernel_space == 0)
	{
		if (copy_from_user(dst, (void *)arg, sizeof(*dst)) != 0)
		{
			pr_err("ERROR enqueue ARM schdrmsg!!\n");
			return -1;
		}
	} /* if (is_from_kernel_space == 0) */
	else /* if (is_from_kernel_space != 0) */
	{
		memcpy(dst, (void *)arg, sizeof(*dst));
	} /* if (is_from_kernel_space != 0) */

	dst->hdr.message_type |= 0x8000; /* Make sure bit is set */
	/* seqno */
	dst->hdr.message_seqno = qconfig.from_arm_w_seqno;
	qconfig.from_arm_w_seqno++;
#ifdef ASIL_COMPLIANCE
	/* checksum */
	hash = schdrmsg_calc_hash(&dst->hdr, 12, hash);
	hash = schdrmsg_calc_hash(&dst->msg, sizeof(dst->msg), hash);
	dst->hdr.message_checksum = hash;
#endif /* ?ASIL_COMPLIANCE */
	//schdrmsg_log_msg(dst);
	cache_clean_schdrmsg(dst);

	*qconfig.from_arm_w = ++wi;
	ambacv_cache_clean(qconfig.from_arm_w, sizeof(*qconfig.from_arm_w));
	mutex_unlock(&tx_lock);

	// trigger ORC to receive message
	visorc_armidsp_queue_kick();

	return 0;
}

uint32_t schdrmsg_find_entry(unsigned long arg)
{
	cvmem_messagepool_t  *pool;
	cvmem_messagebuf_t   *entry, *found;
	char *base;
	int pos, top, end, step;
	ambacv_get_value_t karg;
	int ret = 0U;

	ret = copy_from_user(&karg, (void*)arg, sizeof(ambacv_get_value_t));
	pool = ambacv_c2v(karg.index);
	ambacv_cache_invalidate(pool, sizeof(*pool));
	ambacv_cache_barrier();
	found = NULL;

	if (pool->hdr_variable.message_num != 0)
	{
		pos  = end = pool->hdr_variable.wr_index;
		top  = pool->hdr_variable.message_num;
		step = pool->hdr_variable.message_buffersize;
		base = (char*)ambacv_c2v(pool->hdr_variable.messagepool_basedaddr);

		mutex_lock(&find_lock);
		do {
			entry = (cvmem_messagebuf_t *)(base + pos * step);
			ambacv_cache_invalidate(&entry->hdr_variable, sizeof(entry->hdr_variable));
			ambacv_cache_barrier();

			if (++pos == top)
				pos = 0;

			if (entry->hdr_variable.message_in_use == entry->hdr_variable.message_processed) {
				//pr_info("find entry at index %d %d %d", top,end,pos);
				pool->hdr_variable.wr_index           = pos;
				entry->hdr_variable.message_in_use    = 1;
				entry->hdr_variable.message_processed = 0;
				found = entry;
				break;
			}
		} while (pos != end);
		mutex_unlock(&find_lock);
	}

	if(found != NULL) {
		karg.value = ambacv_v2c(found);
		ret = copy_to_user((void*)arg, &karg, sizeof(ambacv_get_value_t));
	} else {
		ret = -1;
	}
	return ret;
}

void schdrmsg_init(void)
{
	armvis_message_queue_t *conf;
	uint64_t conf_base = 0UL, buff_base = 0UL, top = 0UL;

	sema_init(&rx_lock, 0);
	mutex_init(&tx_lock);
	mutex_init(&find_lock);
	mutex_init(&event_time_lock);
	init_waitqueue_head(&msg_wq);

	// setup local queue config according to scheduler_id
	top  = ambacv_global_mem.all.base + ambacv_global_mem.all.size;
	conf_base  = top - SYSCONF_QINFO_SIZE;
	conf_base += 2 * scheduler_id * sizeof(*conf);
	buff_base  = top - SCHDRMSG_MEM_REGION_SIZE;
	buff_base += 2 * scheduler_id * (ARMVIS_MESSAGE_NUM * ARMVIS_MESSAGE_SIZE);

	qconfig.reqQ_addr = conf_base;
	conf = (armvis_message_queue_t*) ambacv_c2v(qconfig.reqQ_addr);
	memset(conf, 0, sizeof(*conf));
	conf->msg_array_daddr = buff_base;
	conf->max_count = ARMVIS_MESSAGE_NUM;
	conf->queue_id = scheduler_id;
	conf->rd_msg_array_daddr  = conf->msg_array_daddr;  /* Shadow copies  */
	conf->wr_msg_array_daddr  = conf->msg_array_daddr;  /*  |             */
	conf->rd_queue_id         = conf->queue_id;         /*  |             */
	conf->wr_queue_id         = conf->queue_id;         /* -/             */

	qconfig.from_orc_r = &(conf->rd_idx);
	qconfig.from_orc_w = &(conf->wr_idx);
	qconfig.from_orc_q = ambacv_c2v(conf->msg_array_daddr);

	qconfig.rplQ_addr = conf_base + sizeof(*conf);
	conf = (armvis_message_queue_t*) ambacv_c2v(qconfig.rplQ_addr);
	memset(conf, 0, sizeof(*conf));
	conf->msg_array_daddr = buff_base + (ARMVIS_MESSAGE_NUM * ARMVIS_MESSAGE_SIZE);
	conf->max_count = ARMVIS_MESSAGE_NUM;
	conf->queue_id = scheduler_id;
	conf->rd_msg_array_daddr  = conf->msg_array_daddr;  /* Shadow copies  */
	conf->wr_msg_array_daddr  = conf->msg_array_daddr;  /*  |             */
	conf->rd_queue_id         = conf->queue_id;         /*  |             */
	conf->wr_queue_id         = conf->queue_id;         /* -/             */

	qconfig.from_arm_r = &(conf->rd_idx);
	qconfig.from_arm_w = &(conf->wr_idx);
	qconfig.from_arm_q = ambacv_c2v(conf->msg_array_daddr);
}

void schdrmsg_shutdown(void)
{
	if (rx_kthread != NULL) {
		up(&rx_lock);
		kthread_stop(rx_kthread);
		rx_kthread = NULL;
	}
	krn_flexidag_shutdown();
	ambacv_global_mem.arm.base = ambacv_global_mem.arm.size = 0;
	pr_info("ambacv: schdr cleanup finished\n");
}
