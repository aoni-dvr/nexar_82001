/*
 * Copyright (c) 2018-2018 Ambarella, Inc.
 * 2018/06/01 - [Zhikan Yang] created file
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

/*================================================================================================*/
/* Directly imported from ambarella/private/cavalry_drv git repository                            */
/*                                                                                                */
/* File source : "cavalry_visorc.c"                                                               */
/* File size   : 40875 bytes                                                                      */
/* File md5sum : d88571a81a15ce6fff6b470825330f64                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing:                                                                  */
/*  + Clipped large components of the file out, as they are moved into ambacv_dev.c, or           */
/*    cavalry_ioctl.c for integration into CVFLOW.                                                */
/*================================================================================================*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/of_irq.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#if 0  /*-= Removed for integration with CVFLOW =-----------------------------*/
       /* Special note : This is removed, as "config.h" does not exist in     */
       /* AmbaLink based builds.  All definitions that require the use of the */
       /* defines contained within config.h are not active in this file, as   */
       /* as they are moved into the ambacv_dev or cavalry_ioctl files.       */
       /*-----------------------------------------------------------------vvv-*/
#include "config.h"
#else
#include "ambacv_kal.h"   // For common "orc_send_sync_count"
#define VISORC_EARLY_QUIT_SYNC	(126)
#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/
#include "cavalry.h"
#include "cavalry_mem.h"
#include "cavalry_log.h"
#include "cavalry_print.h"
#include "cavalry_core_dump.h"

#define TIMEOUT_MS_LONG (10000) // 10s
#define TIMEOUT_MS_MIDDLE (5000) // 5s
#define TIMEOUT_MS_SHORT (2000) // 2s

#if 0  /*-= Removed for integration with CVFLOW =-------------------------vvv-*/
#define VISORC_BASE	(0xed000000)
#define VISORC_SIZE	(0x01000000)

#define VISORC_VP_SYNC	(127)
#define VISORC_EARLY_QUIT_SYNC	(126)
#define VISORC_FEX_SYNC	(125)
#define VISORC_FMA_SYNC	(124)

/* for VP-ORC */
#define VP_TRESET	(0x010000)
#define VP_CACHE	(0x010004)
#define VP_RESET_PC	(0x010008)
#define VP_STATUS	(0x010030)

/* for SMEM */
#define SYNC_CTR_BASE (0x05F000)

/* for RCT */
#define CLUSTER_SRESET	(0x080228)

/* for L2-cache */
#define REMAP_R0_START	(0x0a0040)
#define REMAP_R0_SIZE		(0x0a0044)
#define REMAP_R0_RBASE	(0x0a0048)
#define REMAP_R1_START	(0x0a0050)
#define REMAP_R1_SIZE		(0x0a0054)
#define REMAP_R1_RBASE	(0x0a0058)
#define REMAP_R2_START	(0x0a0060)
#define REMAP_R2_SIZE		(0x0a0064)
#define REMAP_R2_RBASE	(0x0a0068)
#define REMAP_R3_START	(0x0a0070)
#define REMAP_R3_SIZE		(0x0a0074)
#define REMAP_R3_RBASE	(0x0a0078)

#define visorc_reg(x)	(visorc_io_base + (x))

static void __iomem *visorc_io_base = NULL;

static void orc_send_sync_count(int id, int val)
{
	writel(val, visorc_reg(SYNC_CTR_BASE) + 4 * id);
}

static void visorc_kick_vp(void)
{
	orc_send_sync_count(VISORC_VP_SYNC, 0x1400);
}

static void visorc_kick_fex(void)
{
	orc_send_sync_count(VISORC_FEX_SYNC, 0x1400);
}

static void visorc_kick_fma(void)
{
	orc_send_sync_count(VISORC_FMA_SYNC, 0x1400);
}

static void visorc_fill_init_data(struct ambarella_cavalry *cavalry)
{
	struct cavaly_init_data *init_data = (struct cavaly_init_data *)cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].virt;

	init_data->base_daddr = cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys;

	init_data->cmd_q_daddr = CAVALRY_MEM_CMD_OFFSET;
	init_data->cmd_q_size = CAVALRY_MEM_CMD_SIZE;
	init_data->msg_q_daddr = CAVALRY_MEM_MSG_OFFSET;
	init_data->msg_q_size = CAVALRY_MEM_MSG_SIZE;

	init_data->log_q_daddr = CAVALRY_MEM_LOG_OFFSET;
	init_data->log_q_size = CAVALRY_MEM_LOG_SIZE;

	init_data->cmd_q_fex_daddr = CAVALRY_MEM_CMD_FEX_OFFSET;
	init_data->cmd_q_fex_size = CAVALRY_MEM_CMD_FEX_SIZE;
	init_data->msg_q_fex_daddr = CAVALRY_MEM_MSG_FEX_OFFSET;
	init_data->msg_q_fex_size = CAVALRY_MEM_MSG_FEX_SIZE;

	init_data->cmd_q_fma_daddr = CAVALRY_MEM_CMD_FMA_OFFSET;
	init_data->cmd_q_fma_size = CAVALRY_MEM_CMD_FMA_SIZE;
	init_data->msg_q_fma_daddr = CAVALRY_MEM_MSG_FMA_OFFSET;
	init_data->msg_q_fma_size = CAVALRY_MEM_MSG_FMA_SIZE;

	init_data->hotlink_slot0_offset = CAVALRY_MEM_HOTLINK_SLOT0_OFFSET;
	init_data->hotlink_slot1_offset = CAVALRY_MEM_HOTLINK_SLOT1_OFFSET;
	init_data->hotlink_slot2_offset = CAVALRY_MEM_HOTLINK_SLOT2_OFFSET;
	init_data->hotlink_slot3_offset = CAVALRY_MEM_HOTLINK_SLOT3_OFFSET;
	init_data->log_level = cavalry->log_level;
}

void visorc_init(struct ambarella_cavalry *cavalry)
{
	if (visorc_io_base == NULL) {
		visorc_io_base = ioremap(VISORC_BASE, VISORC_SIZE);
		if (visorc_io_base == NULL) {
			pr_err("ioremap VISORC registers failed\n");
		}
	}

	init_waitqueue_head(&cavalry->cavalry_vp_wq);
	init_waitqueue_head(&cavalry->cavalry_fex_wq);
	init_waitqueue_head(&cavalry->cavalry_fma_wq);
	sema_init(&cavalry->cavalry_vp_sem, 1);
	sema_init(&cavalry->cavalry_fex_sem, 1);
	sema_init(&cavalry->cavalry_fma_sem, 1);
	mutex_init(&cavalry->cavalry_early_quit_mutex);
	spin_lock_init(&cavalry->cavalry_cmd_msg_vp_lock);
	spin_lock_init(&cavalry->cavalry_cmd_msg_fex_lock);
	spin_lock_init(&cavalry->cavalry_cmd_msg_fma_lock);
	visorc_fill_init_data(cavalry);
}

void visorc_shutdown(struct ambarella_cavalry *cavalry)
{
	struct stop_cmd *stop_cmd = NULL;
	int ret = 0;

	if (atomic_read(&cavalry->is_vp_started) == 1 &&
		(!cavalry->is_vp_hang)) {
		down(&cavalry->cavalry_vp_sem);

		spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);
		stop_cmd = (struct stop_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt;
		stop_cmd->cmd_code = STOP_CMD;
		visorc_kick_vp();
		spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);

		ret = wait_event_timeout(cavalry->cavalry_vp_wq,
			atomic_read(&cavalry->is_vp_started) == 0, msecs_to_jiffies(TIMEOUT_MS_SHORT));
		if (ret == 0) {
			prt_err("VP Stop timeout!\n");
		} else if (ret < 0) {
			prt_err("VP Stop abort!\n");
		} else {
			prt_info("Cavalry Ucode is stopped.\n");
		}

		up(&cavalry->cavalry_vp_sem);
	}

	if (visorc_io_base != NULL) {
		iounmap(visorc_io_base);
		visorc_io_base = NULL;
	}
}
#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/

int cavalry_visorc_start(struct ambarella_cavalry *cavalry, void __user *arg)
{
	cavalry_msg_t *msg = NULL;
#if 0  /*-= Removed for integration with CVFLOW =-------------------------vvv-*/
	phys_addr_t offset = 0;
	uint32_t i = 0;
	int rval = 0, ret = 0;
#else
	int rval = 0;
#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/
	down(&cavalry->cavalry_vp_sem);
	if (cavalry->is_vp_hang) {
		up(&cavalry->cavalry_vp_sem);
		prt_err("VP hangs, Can't accept new request.\n");
		return -EFAULT;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (atomic_read(&cavalry->is_vp_started) == 1) {
		spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		up(&cavalry->cavalry_vp_sem);
		prt_trace("Cavalry Ucode has already started.\n");
		return 0;
	}

	atomic_set(&cavalry->is_vp_started, 0);
	msg = (cavalry_msg_t *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].virt;
	msg->msg_code = 0;

#if 0  /*-= Removed for integration with CVFLOW =-------------------------vvv-*/
	/* soft-reset all clusters*/
	/* have to skip bit[9] since it resets audio tick counter as well */
	writel(0x00000000, visorc_reg(CLUSTER_SRESET));
#if defined (CONFIG_ARCH_CV2)
	writel(0x00000780, visorc_reg(CLUSTER_SRESET));
#elif defined (CONFIG_ARCH_CV22)
	writel(0x00000700, visorc_reg(CLUSTER_SRESET));
#elif defined (CONFIG_ARCH_CV25)
	writel(0x00000700, visorc_reg(CLUSTER_SRESET));
#else
	#error "unsupported CHIP ID, please check sdk config"
#endif
	writel(0x00000000, visorc_reg(CLUSTER_SRESET));

	/* setup remap registers */
	writel(0x00000000, visorc_reg(REMAP_R0_START));
	writel(CAVALRY_MEM_USER_OFFSET, visorc_reg(REMAP_R0_SIZE));
	writel(cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys | 0x1, visorc_reg(REMAP_R0_RBASE));
	writel(0x00000000, visorc_reg(REMAP_R1_SIZE));
	writel(0x00000000, visorc_reg(REMAP_R2_SIZE));
	writel(0x00000000, visorc_reg(REMAP_R3_SIZE));

	/* setup visorc reset vector */
	offset = cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].phys -
		cavalry->cavalry_mem_info[CAVALRY_MEM_ALL].phys;
	writel((uint32_t)offset, visorc_reg(VP_RESET_PC));

	/* invalidate both I/D cache */
	writel(0x00000003, visorc_reg(VP_CACHE));
	//writel(0x00000000, visorc_reg(0x0a008C));

	/* reset orcvp */
#if defined (CONFIG_ARCH_CV2)
	writel(0x00001F00, visorc_reg(VP_TRESET));
#elif defined (CONFIG_ARCH_CV22)
	writel(0x00000F00, visorc_reg(VP_TRESET));
#elif defined (CONFIG_ARCH_CV25)
	writel(0x00000F00, visorc_reg(VP_TRESET));
#endif
	/* reset all sync counters */
	for (i = 112; i < 0x80; i++) {
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 112; i < 0x80; i++) {
		orc_send_sync_count(i, 0x0000);
	}
	for (i = 112; i < 0x80; i++) {
		orc_send_sync_count(i, 0x1020);
	}
	for (i = 112; i < 0x80; i++) {
		orc_send_sync_count(i, 0x0000);
	}

	/* release orc-sod/vp */
#if defined (CONFIG_ARCH_CV2)
	writel(0x0000001F, visorc_reg(VP_TRESET));
#elif defined (CONFIG_ARCH_CV22)
	writel(0x0000000F, visorc_reg(VP_TRESET));
#elif defined (CONFIG_ARCH_CV25)
	writel(0x0000000F, visorc_reg(VP_TRESET));
#endif

#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/
	spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
#if 0  /*-= Removed for integration with CVFLOW =-------------------------vvv */
	ret = wait_event_timeout(cavalry->cavalry_vp_wq,
		atomic_read(&cavalry->is_vp_started) == 1, msecs_to_jiffies(TIMEOUT_MS_SHORT));
	if (ret == 0) {
		prt_err("VP start timeout!\n");
		rval = -EFAULT;
	} else if (ret < 0) {
		prt_err("VP start abort!\n");
		rval = -EINTR;
	} else {
		prt_info("Cavalry Ucode is started.\n");
	}
#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/

	up(&cavalry->cavalry_vp_sem);

	return rval;
}

int cavalry_visorc_stop(struct ambarella_cavalry *cavalry, void __user *arg)
{
#if 0  /*-= Removed for integration with CVFLOW =-------------------------vvv-*/
	struct stop_cmd *stop_cmd = NULL;
	int rval = 0, ret = 0;
#else
	int rval = 0;
#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/

	down(&cavalry->cavalry_vp_sem);

	if (cavalry->is_vp_hang) {
		up(&cavalry->cavalry_vp_sem);
		prt_err("VP hangs, Can't accept new request.\n");
		return -EFAULT;
	}

#if 0  /*-= Removed for integration with CVFLOW =-------------------------vvv-*/
	spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);
	stop_cmd = (struct stop_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt;
	stop_cmd->cmd_code = STOP_CMD;
	visorc_kick_vp();
	spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);

	ret = wait_event_timeout(cavalry->cavalry_vp_wq,
		atomic_read(&cavalry->is_vp_started) == 0, msecs_to_jiffies(TIMEOUT_MS_SHORT));
	if (ret == 0) {
		prt_err("VP Stop timeout!\n");
		rval = -EFAULT;
	} else if (ret < 0) {
		prt_err("VP Stop abort!\n");
		rval = -EINTR;
	} else {
		prt_info("Cavalry Ucode is stopped.\n");
	}
#endif /*-= Removed for integration with CVFLOW =-------------------------^^^-*/

	cavalry_log_reset(cavalry);

	up(&cavalry->cavalry_vp_sem);

	return rval;
}

inline uint32_t cmd_to_msg(uint32_t cmd)
{
	return cmd | 0x80000000;
}

#if 0  /*-= Moved to cavalry_ioctl.c for integration with CVFLOW =--------vvv-*/

static int cavalry_check_dags(struct cavalry_run_dags *run_dags)
{
	int i = 0, j = 0;
	struct cavalry_dag_desc *dag_desc = NULL;

	if (run_dags->dag_cnt == 0 || run_dags->dag_cnt > MAX_DAG_CNT) {
		prt_err("Invalid dag_cnt %u\n", run_dags->dag_cnt);
		return -1;
	}

	for (i = 0; i < run_dags->dag_cnt; i++) {
		dag_desc = &run_dags->dag_desc[i];

		if (!dag_desc->dvi_img_size) {
			prt_err("dvi img size can't be zero.\n");
			return -1;
		}
		if (!dag_desc->dvi_img_vaddr) {
			prt_err("dvi img vaddr can't be zero.\n");
			return -1;
		}
		if (!dag_desc->dvi_dag_vaddr) {
			prt_err("dvi dag vaddr can't be zero.\n");
			return -1;
		}

		if (dag_desc->port_cnt > MAX_PORT_CNT) {
			prt_err("DAG [%u] port cnt [%u] exceed Max [%u].\n", i, dag_desc->port_cnt,
				MAX_PORT_CNT);
			return -1;
		}

		if (dag_desc->poke_cnt > MAX_POKE_CNT) {
			prt_err("DAG [%u] poke cnt [%u] exceed Max [%u].\n", i, dag_desc->poke_cnt,
				MAX_POKE_CNT);
			return -1;
		}

		for (j = 0; j < dag_desc->port_cnt; j++) {
			if (dag_desc->port_desc[j].port_dram_size == 0) {
				prt_err("DRAM size of Port [%d] in Dag [%d] can't be zero.\n", j, i);
				return -1;
			}
		}
	}

	return 0;
}

int cavalry_run_dags(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_run_dags *run_dags = cavalry->run_dags;
	struct dag_run_cmd *dag_run_cmd = NULL;
	struct dag_run_msg *dag_run_msg = NULL;
	uint32_t dag_cnt = 0, i = 0, offset = 0;
	uint32_t timeout_ms = 0;
	int rval = 0, ret = 0;
	uint8_t run_done = 0;
	uint8_t dag_loop_enabled = 0;
	uint32_t vp_status = 0;

	atomic_inc(&cavalry->pending_cnt);

	down(&cavalry->cavalry_vp_sem);

	if (cavalry->is_vp_hang) {
		atomic_dec(&cavalry->pending_cnt);
		up(&cavalry->cavalry_vp_sem);
		prt_err("VP hangs, Can't accept new request.\n");
		return -EFAULT;
	}

	if (copy_from_user(&dag_cnt, arg + offsetof(struct cavalry_run_dags, dag_cnt), sizeof(dag_cnt))) {
		atomic_dec(&cavalry->pending_cnt);
		up(&cavalry->cavalry_vp_sem);
		prt_err("copy_from_user dag_cnt err.\n");
		return -EFAULT;
	}

	if (dag_cnt == 0 || dag_cnt > MAX_DAG_CNT) {
		atomic_dec(&cavalry->pending_cnt);
		up(&cavalry->cavalry_vp_sem);
		prt_err("Invalid dag_cnt %u.\n", dag_cnt);
		return -EINVAL;
	}

	offset = offsetof(struct cavalry_run_dags, dag_desc);
	run_dags->dag_cnt = dag_cnt;
	for (i = 0; i < dag_cnt; i++) {
		if (copy_from_user(&run_dags->dag_desc[i], arg + offset,
			offsetof(struct cavalry_dag_desc, port_desc))) {
			ret = -1;
			break;
		}
		if (copy_from_user(run_dags->dag_desc[i].port_desc,
			arg + offset + offsetof(struct cavalry_dag_desc, port_desc),
			run_dags->dag_desc[i].port_cnt * sizeof(struct cavalry_port_desc))) {
			ret = -1;
			break;
		}
		if (copy_from_user(run_dags->dag_desc[i].poke_desc,
			arg + offset + offsetof(struct cavalry_dag_desc, poke_desc),
			run_dags->dag_desc[i].poke_cnt * sizeof(struct cavalry_poke_desc))) {
			ret = -1;
			break;
		}
		offset += sizeof(struct cavalry_dag_desc);
	}
	if (ret < 0) {
		atomic_dec(&cavalry->pending_cnt);
		up(&cavalry->cavalry_vp_sem);
		prt_err("copy_from_user run_dags err.\n");
		return -EFAULT;
	}

	if (cavalry_check_dags(run_dags) < 0) {
		atomic_dec(&cavalry->pending_cnt);
		up(&cavalry->cavalry_vp_sem);
		return -EINVAL;
	}

	for (i = 0; i < dag_cnt; i++) {
		if (run_dags->dag_desc[i].dag_loop_cnt > 1) {
			dag_loop_enabled = 1;
			prt_debug("Find dag_loop_cnt: %u\n", run_dags->dag_desc[i].dag_loop_cnt);
			break;
		}
	}

	spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		atomic_dec(&cavalry->pending_cnt);
		spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		up(&cavalry->cavalry_vp_sem);
		if (!atomic_read(&cavalry->pending_cnt)) {
			atomic_set(&cavalry->is_early_quit_all, 0);
		}
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	if (atomic_read(&cavalry->is_early_quit_all)) {
		atomic_dec(&cavalry->pending_cnt);
		spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		up(&cavalry->cavalry_vp_sem);
		if (!atomic_read(&cavalry->pending_cnt)) {
			atomic_set(&cavalry->is_early_quit_all, 0);
		}
		// Early quit pending run request with finish_dags = 0
		memset(arg, 0, offsetof(struct cavalry_run_dags, dag_cnt));
		return 0;
	}

	do {
		atomic_set(&cavalry->is_vp_done, 0);
		cavalry->vp_current_pid = current->pid;
		// Fill CMD in phys
		dag_run_cmd = (struct dag_run_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt;
		dag_run_msg = (struct dag_run_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].virt;
		dag_run_cmd->cmd_code = DAG_RUN_CMD;
		dag_run_cmd->dag_cnt = dag_cnt;
		for (i = 0; i < dag_cnt; i++) {
			memcpy(&dag_run_cmd->dag_desc[i], &run_dags->dag_desc[i],
				offsetof(struct cavalry_dag_desc, port_desc));
			memcpy(dag_run_cmd->dag_desc[i].port_desc, run_dags->dag_desc[i].port_desc,
				run_dags->dag_desc[i].port_cnt * sizeof(struct cavalry_port_desc));
			memcpy(dag_run_cmd->dag_desc[i].poke_desc, run_dags->dag_desc[i].poke_desc,
				run_dags->dag_desc[i].poke_cnt * sizeof(struct cavalry_poke_desc));
		}
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (!rval) {
		visorc_kick_vp();

		if (dag_loop_enabled) {
			timeout_ms = TIMEOUT_MS_LONG;
		} else {
			timeout_ms = TIMEOUT_MS_MIDDLE;
		}
		ret = wait_event_timeout(cavalry->cavalry_vp_wq,
			atomic_read(&cavalry->is_vp_done) == 1, msecs_to_jiffies(timeout_ms));
		if (ret == 0) {
			prt_err("Wait VP event timeout.\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait VP event abort.\n");
			rval = -EINTR;
		} else {
			rval = 0;
		}

		// Check if VP hangs ot not.
		vp_status = readl(visorc_reg(VP_STATUS));
		cavalry->is_vp_hang = (vp_status >> 4) & 0x01;
		if (cavalry->is_vp_hang) {
			cavalry_vp_core_dump(cavalry, dag_run_msg->finish_dags,
				dag_run_msg->dag_loop_index);
			prt_err("VP Seems Hang.\n");
		}

		if (!rval) {
			spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);
			if (dag_run_msg->msg_code == cmd_to_msg(DAG_RUN_CMD)) {
				switch (dag_run_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				case MSG_RVAL_INVALID_DAGCNT:
					prt_err("Invalid dag cnt!\n");
					rval = -EFAULT;
					break;
				case MSG_RVAL_INVALID_DAGDESC:
					prt_err("Invalid dag descriptor!\n");
					rval = -EFAULT;
					break;
				case MSG_RVAL_VP_RESERVED_VMEM_TRASHED:
					prt_err("VP Reserved VMEM Trashed!\n");
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		}
		if (run_done) {
			if (copy_to_user(arg, &dag_run_msg->rval,
				offsetof(struct cavalry_run_dags, dag_cnt))) {
				rval = -EFAULT;
			}

			for (i = 0; i < dag_cnt; i++) {
				if (copy_to_user(arg + offsetof(struct cavalry_run_dags, dag_desc) +
					offsetof(struct cavalry_dag_desc, reserved1[0]) +
					sizeof(struct cavalry_dag_desc) * i, &dag_run_cmd->dag_desc[i].reserved1[0],
					sizeof(uint32_t))) {
					rval = -EFAULT;
					break;
				}
			}
		}
	}

	atomic_dec(&cavalry->pending_cnt);
	if (!atomic_read(&cavalry->pending_cnt)) {
		atomic_set(&cavalry->is_early_quit_all, 0);
	}

	up(&cavalry->cavalry_vp_sem);

	return rval;
}
#endif /*-= Moved to cavalry_ioctl.c for integration with CVFLOW =--------^^^-*/

int cavalry_visorc_early_quit(struct ambarella_cavalry *cavalry, void __user *arg)
{
	int rval = 0;
	struct cavalry_early_quit early_quit;

	mutex_lock(&cavalry->cavalry_early_quit_mutex);

	if (copy_from_user(&early_quit, arg, sizeof(early_quit))) {
		mutex_unlock(&cavalry->cavalry_early_quit_mutex);
		return -EFAULT;
	}

#if 1  /*-= Changed for integration with CVFLOW =-------------------------vvv-*/
	if (atomic_read(&cavalry->pending_cnt)) {
		if (early_quit.early_quit_all) {
			atomic_set(&cavalry->is_early_quit_all, 1);
			orc_send_sync_count(VISORC_EARLY_QUIT_SYNC, 0x2400); /* Early quit all signal */
		}
		else
		{
			orc_send_sync_count(VISORC_EARLY_QUIT_SYNC, 0x1400); /* Early quit one signal */
		}
	}
#else
	if (atomic_read(&cavalry->pending_cnt)) {
		if (early_quit.early_quit_all) {
			atomic_set(&cavalry->is_early_quit_all, 1);
		}
		orc_send_sync_count(VISORC_EARLY_QUIT_SYNC, 0x1400);
	}
#endif /*-= Changed for integration with CVFLOW =-------------------------^^^-*/

	mutex_unlock(&cavalry->cavalry_early_quit_mutex);

	return rval;
}

static int cavalry_check_hotlink_slot(struct ambarella_cavalry *cavalry,
	uint32_t slot_bit_map)
{
	struct version_info_s *ver = NULL;
	uint32_t *slot_id_virt = NULL;
	uint32_t *hotlink_ver = NULL;
	uint8_t mem_idx = 0, slot_id = 0;

	if (slot_bit_map > ((1 << HOTLINK_SLOT_NUM) - 1)) {
		prt_err("Slot bit map [0x%x] out of range [0x%x].\n",
			slot_bit_map, ((1 << HOTLINK_SLOT_NUM) - 1));
		return -1;
	}

	ver = (struct version_info_s *)(cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt +
		VERSION_INFO_OFFSET);
	for (slot_id = 0; slot_id < HOTLINK_SLOT_NUM; slot_id++) {
		if ((slot_bit_map >> slot_id) & (0x1)) {
			mem_idx = CAVALRY_MEM_HOTLINK_SLOT0 + slot_id;
			slot_id_virt = (uint32_t *)(cavalry->cavalry_mem_info[mem_idx].virt +
				CAVALRY_SLOT_ID_OFFSET);
			if (*slot_id_virt != slot_id) {
				prt_err("Slot ID [%u : %u] mismatch!\n", *slot_id_virt, slot_id);
				return -1;
			}

			hotlink_ver = (uint32_t *)(cavalry->cavalry_mem_info[mem_idx].virt +
				CAVALRY_HOTLINK_FW_VER_OFFSET);
			if (*hotlink_ver != ver->hotlink_fw_version) {
				prt_err("Hotlink Version [%u : %u] mismatch!\n",
					*hotlink_ver, ver->hotlink_fw_version);
				return -1;
			}
		}
	}

	return 0;
}

#if 0  /*-= Moved to cavalry_ioctl.c for integration with CVFLOW =--------vvv-*/
int cavalry_run_hotlink_slot(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_run_hotlink_slot run_hotlink_slot;
	struct hotlink_run_cmd *hotlink_run_cmd = NULL;
	struct hotlink_run_msg *hotlink_run_msg = NULL;
	int rval = 0, ret = 0, run_done = 0;

	down(&cavalry->cavalry_vp_sem);

	if (cavalry->is_vp_hang) {
		up(&cavalry->cavalry_vp_sem);
		prt_err("VP hangs, Can't accept new request.\n");
		return -EFAULT;
	}

	if (copy_from_user(&run_hotlink_slot, arg, sizeof(run_hotlink_slot))) {
		up(&cavalry->cavalry_vp_sem);
		return -EFAULT;
	}

	if (((cavalry->slot_activated) & (1 << run_hotlink_slot.slot_id)) == 0) {
		prt_err("Slot [%u] is not activated yet.\n", run_hotlink_slot.slot_id);
		return -EINVAL;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		up(&cavalry->cavalry_vp_sem);
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	do {
		atomic_set(&cavalry->is_vp_done, 0);

		// Fill CMD
		hotlink_run_cmd = (struct hotlink_run_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt;
		hotlink_run_cmd->cmd_code = HOTLINK_RUN_CMD;
		hotlink_run_cmd->slot_id = run_hotlink_slot.slot_id;
		hotlink_run_cmd->arg_daddr = run_hotlink_slot.arg_daddr;
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (!rval) {
		visorc_kick_vp();

		ret = wait_event_timeout(cavalry->cavalry_vp_wq,
			atomic_read(&cavalry->is_vp_done) == 1, msecs_to_jiffies(TIMEOUT_MS_MIDDLE));

		if (ret == 0) {
			prt_err("Wait VP event timeout\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait VP event abort\n");
			rval = -EINTR;
		} else {
			hotlink_run_msg = (struct hotlink_run_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].virt;
			spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);
			if (hotlink_run_msg->msg_code == cmd_to_msg(HOTLINK_RUN_CMD)) {
				switch (hotlink_run_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				case MSG_RVAL_INVALID_SLOT_ID:
					prt_err("Invalid Slot ID!\n");
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		}
		if (run_done) {
			run_hotlink_slot.slot_rval = hotlink_run_msg->slot_rval;
			run_hotlink_slot.start_tick = hotlink_run_msg->start_tick;
			run_hotlink_slot.end_tick = hotlink_run_msg->end_tick;
			if (copy_to_user(arg, &run_hotlink_slot, sizeof(run_hotlink_slot))) {
				rval = -EFAULT;
			}
		}
	}

	up(&cavalry->cavalry_vp_sem);

	return rval;
}
#endif /*-= Moved to cavalry_ioctl.c for integration with CVFLOW =--------^^^-*/

int cavalry_set_hotlink_slot_cfg(struct ambarella_cavalry *cavalry, void __user *arg)
{
	int rval = 0;
	uint32_t slot_activated = 0;

	down(&cavalry->cavalry_vp_sem);

	if (copy_from_user(&slot_activated, arg, sizeof(slot_activated))) {
		up(&cavalry->cavalry_vp_sem);
		return -EFAULT;
	}

	if (cavalry_check_hotlink_slot(cavalry, slot_activated) < 0) {
		up(&cavalry->cavalry_vp_sem);
		return -EINVAL;
	}

	cavalry->slot_activated = slot_activated;

#if 1  /*-= Added to support I-Cache drop in VISORC =---------------------vvv-*/
	{
		armvis_msg_t msgbase;
		msgbase.hdr.message_type    = SCHEDMSG_INVALIDATE_CACHE;
        msgbase.hdr.message_id      = 0;
        msgbase.hdr.message_retcode = ERRCODE_NONE;
		schdrmsg_send_armmsg((unsigned long)&msgbase, 1);
	}
#endif /*-= Added to support I-Cache drop in VISORC =---------------------^^^-*/

	up(&cavalry->cavalry_vp_sem);

	return rval;
}

int cavalry_get_hotlink_slot_cfg(struct ambarella_cavalry *cavalry, void __user *arg)
{
	int rval = 0;

	down(&cavalry->cavalry_vp_sem);

	rval = copy_to_user(arg, &cavalry->slot_activated, sizeof(cavalry->slot_activated));

	up(&cavalry->cavalry_vp_sem);

	return rval;
}

#if 0  /*-= Moved to cavalry_ioctl.c for integration with CVFLOW =--------vvv-*/
int cavalry_fex_query(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_fex_query fex_query;
	struct fex_query_cmd *fex_query_cmd = NULL;
	struct fex_query_msg *fex_query_msg = NULL;
	int rval = 0, ret = 0, run_done = 0;
	struct version_info_s *ver;

	down(&cavalry->cavalry_fex_sem);

	ver = (struct version_info_s *)(cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt +
		VERSION_INFO_OFFSET);
	if (ver->chip != CHIP_TYPE_CV2) {
		prt_err("FEX is only available on CV2!\n");
		up(&cavalry->cavalry_fex_sem);
		return -EINVAL;
	}

	if (copy_from_user(&fex_query, arg, sizeof(struct cavalry_fex_query))) {
		up(&cavalry->cavalry_fex_sem);
		return -EFAULT;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_fex_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		spin_unlock(&cavalry->cavalry_cmd_msg_fex_lock);
		up(&cavalry->cavalry_fex_sem);
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	do {
		atomic_set(&cavalry->is_fex_done, 0);

		// Fill CMD
		fex_query_cmd = (struct fex_query_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FEX].virt;
		fex_query_cmd->cmd_code = FEX_QUERY_CMD;
		fex_query_cmd->img_width = fex_query.img_width;
		fex_query_cmd->img_height = fex_query.img_height;
		fex_query_cmd->img_pitch = fex_query.img_pitch;
		fex_query_cmd->harris_en = fex_query.harris_en;
		fex_query_cmd->stereo_en = fex_query.stereo_en;
		fex_query_cmd->dump_harris_score = fex_query.dump_harris_score;
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_fex_lock);

	if (!rval) {
		visorc_kick_fex();

		ret = wait_event_timeout(cavalry->cavalry_fex_wq,
			atomic_read(&cavalry->is_fex_done) == 1, msecs_to_jiffies(TIMEOUT_MS_MIDDLE));

		if (ret == 0) {
			prt_err("Wait FEX event timeout\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait FEX event abort\n");
			rval = -EINTR;
		} else {
			fex_query_msg = (struct fex_query_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FEX].virt;
			spin_lock(&cavalry->cavalry_cmd_msg_fex_lock);
			if (fex_query_msg->msg_code == cmd_to_msg(FEX_QUERY_CMD)) {
				switch (fex_query_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_fex_lock);
		}
		if (run_done) {
			fex_query.dram_required = fex_query_msg->dram_required;
			if (copy_to_user(arg, &fex_query, sizeof(fex_query))) {
				rval = -EFAULT;
			}
		}
	}

	up(&cavalry->cavalry_fex_sem);

	return rval;
}

static int cavalry_check_fex_run(struct ambarella_cavalry *cavalry,
	struct cavalry_fex_run *fex_run)
{
	if (fex_run->stereo_en && (!fex_run->luma_daddr[0] ||
		!fex_run->luma_daddr[1])) {
		prt_err("luma_daddr[0] 0x%x and luma_daddr[1] 0x%x can't be NULL when " \
			"stereo is enabled.\n", fex_run->luma_daddr[0], fex_run->luma_daddr[1]);
		return -1;
	}

	if ((fex_run->harris_en & 0x01) && (!fex_run->luma_daddr[0])) {
		prt_err("luma_daddr[0] can't be NULL when Harris for this input is enabled.\n");
		return -1;
	}

	if (((fex_run->harris_en >> 1) & 0x01) && (!fex_run->luma_daddr[1])) {
		prt_err("luma_daddr[1] can't be NULL when Harris for this input is enabled.\n");
		return -1;
	}

	if (!fex_run->img_width) {
		prt_err("Image Width can't be Zero.\n");
		return -1;
	}

	if (!fex_run->img_height) {
		prt_err("Image Height can't be Zero.\n");
		return -1;
	}

	if (fex_run->img_pitch < fex_run->img_width) {
		prt_err("Image Pitch [%u] can't be smaller than Width [%u].\n", fex_run->img_pitch,
			fex_run->img_width);
		return -1;
	}

	if ((fex_run->harris_en || fex_run->stereo_en) &&
		(!fex_run->output_daddr || !fex_run->output_size)) {
		prt_err("Output Daddr and Size can't be Zero when Harris or Stereo is enabled.\n");
		return -1;
	}

	if (fex_run->stereo_profile >= STEREO_PROFILE_NUM) {
		prt_err("Stereo Profile should be less than %u.\n", STEREO_PROFILE_NUM);
		return -1;
	}

	if (fex_run->fex_cfg.cfg_mask & FEX_CFG_MASK_NMS_WINDOW_FOR_HARRIS) {
		if (fex_run->fex_cfg.nms_window_for_harris < FEX_MIN_NMS_WINDOW ||
			fex_run->fex_cfg.nms_window_for_harris > FEX_MAX_NMS_WINDOW) {
				prt_err("Valid NMS window should be [%u-%u].\n", FEX_MIN_NMS_WINDOW,
					FEX_MAX_NMS_WINDOW);
				return -1;
		}
	}

	return 0;
}

int cavalry_fex_run(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_fex_run fex_run;
	struct fex_run_cmd *fex_run_cmd = NULL;
	struct fex_run_msg *fex_run_msg = NULL;
	int rval = 0, ret = 0, run_done = 0;
	struct version_info_s *ver;

	down(&cavalry->cavalry_fex_sem);

	ver = (struct version_info_s *)(cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt +
		VERSION_INFO_OFFSET);
	if (ver->chip != CHIP_TYPE_CV2) {
		prt_err("FEX is only available on CV2!\n");
		up(&cavalry->cavalry_fex_sem);
		return -EINVAL;
	}

	if (copy_from_user(&fex_run, arg, sizeof(struct cavalry_fex_run))) {
		up(&cavalry->cavalry_fex_sem);
		return -EFAULT;
	}

	if (cavalry_check_fex_run(cavalry, &fex_run) < 0) {
		up(&cavalry->cavalry_fex_sem);
		return -EINVAL;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_fex_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		spin_unlock(&cavalry->cavalry_cmd_msg_fex_lock);
		up(&cavalry->cavalry_fex_sem);
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	do {
		atomic_set(&cavalry->is_fex_done, 0);

		// Fill CMD
		fex_run_cmd = (struct fex_run_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FEX].virt;
		fex_run_cmd->cmd_code = FEX_RUN_CMD;
		fex_run_cmd->img_width = fex_run.img_width;
		fex_run_cmd->img_height = fex_run.img_height;
		fex_run_cmd->img_pitch = fex_run.img_pitch;
		fex_run_cmd->harris_en = fex_run.harris_en;
		fex_run_cmd->stereo_en = fex_run.stereo_en;
		fex_run_cmd->stereo_profile = fex_run.stereo_profile;
		fex_run_cmd->dump_harris_score = fex_run.dump_harris_score;
		fex_run_cmd->output_daddr = fex_run.output_daddr;
		fex_run_cmd->output_size = fex_run.output_size;
		fex_run_cmd->luma_daddr[0] = fex_run.luma_daddr[0];
		fex_run_cmd->luma_daddr[1] = fex_run.luma_daddr[1];
		fex_run_cmd->fex_cfg = fex_run.fex_cfg;
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_fex_lock);

	if (!rval) {
		visorc_kick_fex();

		ret = wait_event_timeout(cavalry->cavalry_fex_wq,
			atomic_read(&cavalry->is_fex_done) == 1, msecs_to_jiffies(TIMEOUT_MS_MIDDLE));

		if (ret == 0) {
			prt_err("Wait FEX event timeout\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait FEX event abort\n");
			rval = -EINTR;
		} else {
			fex_run_msg = (struct fex_run_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FEX].virt;
			spin_lock(&cavalry->cavalry_cmd_msg_fex_lock);
			if (fex_run_msg->msg_code == cmd_to_msg(FEX_RUN_CMD)) {
				switch (fex_run_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				case MSG_RVAL_FEX_INSUFFICIENT_DRAM:
					prt_err("DRAM size 0x%x is insufficient for FEX!\n", fex_run_cmd->output_size);
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_fex_lock);
		}
		if (run_done) {
			fex_run.rval = fex_run_msg->rval;
			fex_run.disparity_daddr = fex_run_msg->disparity_daddr;
			fex_run.disparity_size = fex_run_msg->disparity_size;
			fex_run.invalid_disparities = fex_run_msg->invalid_disparities;
			fex_run.disparity_width = fex_run_msg->disparity_width;
			fex_run.disparity_height = fex_run_msg->disparity_height;
			fex_run.disparity_pitch = fex_run_msg->disparity_pitch;
			fex_run.harris_count_daddr[0] = fex_run_msg->harris_count_daddr[0];
			fex_run.harris_count_daddr[1] = fex_run_msg->harris_count_daddr[1];
			fex_run.harris_count_size[0] = fex_run_msg->harris_count_size[0];
			fex_run.harris_count_size[1] = fex_run_msg->harris_count_size[1];
			fex_run.harris_point_daddr[0] = fex_run_msg->harris_point_daddr[0];
			fex_run.harris_point_daddr[1] = fex_run_msg->harris_point_daddr[1];
			fex_run.harris_point_size[0] = fex_run_msg->harris_point_size[0];
			fex_run.harris_point_size[1] = fex_run_msg->harris_point_size[1];
			fex_run.brief_descriptor_daddr[0] = fex_run_msg->brief_descriptor_daddr[0];
			fex_run.brief_descriptor_daddr[1] = fex_run_msg->brief_descriptor_daddr[1];
			fex_run.brief_descriptor_size[0] = fex_run_msg->brief_descriptor_size[0];
			fex_run.brief_descriptor_size[1] = fex_run_msg->brief_descriptor_size[1];
			fex_run.start_tick = fex_run_msg->start_tick;
			fex_run.end_tick = fex_run_msg->end_tick;

			if (copy_to_user(arg, &fex_run, sizeof(fex_run))) {
				rval = -EFAULT;
			}
		}
	}

	up(&cavalry->cavalry_fex_sem);

	return rval;
}

int cavalry_fma_query(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_fma_query fma_query;
	struct fma_query_cmd *fma_query_cmd = NULL;
	struct fma_query_msg *fma_query_msg = NULL;
	int rval = 0, ret = 0, run_done = 0;
	struct version_info_s *ver;

	down(&cavalry->cavalry_fma_sem);

	ver = (struct version_info_s *)(cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt +
		VERSION_INFO_OFFSET);
	if (ver->chip != CHIP_TYPE_CV2) {
		prt_err("FMA is only available on CV2!\n");
		up(&cavalry->cavalry_fma_sem);
		return -EINVAL;
	}

	if (copy_from_user(&fma_query, arg, sizeof(struct cavalry_fma_query))) {
		up(&cavalry->cavalry_fma_sem);
		return -EFAULT;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_fma_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		spin_unlock(&cavalry->cavalry_cmd_msg_fma_lock);
		up(&cavalry->cavalry_fma_sem);
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	do {
		atomic_set(&cavalry->is_fma_done, 0);

		// Fill CMD
		fma_query_cmd = (struct fma_query_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FMA].virt;
		fma_query_cmd->cmd_code = FMA_QUERY_CMD;
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_fma_lock);

	if (!rval) {
		visorc_kick_fma();

		ret = wait_event_timeout(cavalry->cavalry_fma_wq,
			atomic_read(&cavalry->is_fma_done) == 1, msecs_to_jiffies(TIMEOUT_MS_MIDDLE));

		if (ret == 0) {
			prt_err("Wait FMA event timeout\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait FMA event abort\n");
			rval = -EINTR;
		} else {
			fma_query_msg = (struct fma_query_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FMA].virt;
			spin_lock(&cavalry->cavalry_cmd_msg_fma_lock);
			if (fma_query_msg->msg_code == cmd_to_msg(FMA_QUERY_CMD)) {
				switch (fma_query_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_fma_lock);
		}
		if (run_done) {
			fma_query.dram_required = fma_query_msg->dram_required;
			if (copy_to_user(arg, &fma_query, sizeof(fma_query))) {
				rval = -EFAULT;
			}
		}
	}

	up(&cavalry->cavalry_fma_sem);

	return rval;
}

static int cavalry_check_fma_run(struct ambarella_cavalry *cavalry,
	struct cavalry_fma_run *fma_run)
{
	if (!fma_run->output_daddr) {
		prt_err("output_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->output_size) {
		prt_err("output_size can't be Zero.\n");
		return -1;
	}

	if (!fma_run->target_coord_daddr) {
		prt_err("target_coord_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->target_descriptor_daddr) {
		prt_err("target_descriptor_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->reference_coord_daddr) {
		prt_err("reference_coord_daddr can't be Zero.\n");
		return -1;
	}

	if (!fma_run->reference_descriptor_daddr) {
		prt_err("reference_descriptor_daddr can't be Zero.\n");
		return -1;
	}

	if ((fma_run->mode != CAVALRY_FMA_MODE_TEMPORAL) &&
		(fma_run->mode != CAVALRY_FMA_MODE_STEREO)) {
		prt_err("Incorrect FMA mode %u.\n", fma_run->mode);
		return -1;
	}

	return 0;
}

int cavalry_fma_run(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_fma_run fma_run;
	struct fma_run_cmd *fma_run_cmd = NULL;
	struct fma_run_msg *fma_run_msg = NULL;
	int rval = 0, ret = 0, run_done = 0;
	struct version_info_s *ver;

	down(&cavalry->cavalry_fma_sem);

	ver = (struct version_info_s *)(cavalry->cavalry_mem_info[CAVALRY_MEM_UCODE].virt +
		VERSION_INFO_OFFSET);
	if (ver->chip != CHIP_TYPE_CV2) {
		prt_err("FMA is only available on CV2!\n");
		up(&cavalry->cavalry_fma_sem);
		return -EINVAL;
	}

	if (copy_from_user(&fma_run, arg, sizeof(struct cavalry_fma_run))) {
		up(&cavalry->cavalry_fma_sem);
		return -EFAULT;
	}

	if (cavalry_check_fma_run(cavalry, &fma_run) < 0) {
		up(&cavalry->cavalry_fma_sem);
		return -EINVAL;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_fma_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		spin_unlock(&cavalry->cavalry_cmd_msg_fma_lock);
		up(&cavalry->cavalry_fma_sem);
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	do {
		atomic_set(&cavalry->is_fma_done, 0);

		// Fill CMD
		fma_run_cmd = (struct fma_run_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD_FMA].virt;
		fma_run_cmd->cmd_code = FMA_RUN_CMD;
		fma_run_cmd->output_daddr = fma_run.output_daddr;
		fma_run_cmd->output_size = fma_run.output_size;
		fma_run_cmd->target_coord_daddr = fma_run.target_coord_daddr;
		fma_run_cmd->target_descriptor_daddr = fma_run.target_descriptor_daddr;
		fma_run_cmd->reference_coord_daddr = fma_run.reference_coord_daddr;
		fma_run_cmd->reference_descriptor_daddr = fma_run.reference_descriptor_daddr;
		fma_run_cmd->mode = fma_run.mode;
		fma_run_cmd->temporal_cfg = fma_run.temporal_cfg;
		fma_run_cmd->stereo_cfg = fma_run.stereo_cfg;
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_fma_lock);

	if (!rval) {
		visorc_kick_fma();

		ret = wait_event_timeout(cavalry->cavalry_fma_wq,
			atomic_read(&cavalry->is_fma_done) == 1, msecs_to_jiffies(TIMEOUT_MS_MIDDLE));

		if (ret == 0) {
			prt_err("Wait FMA event timeout\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait FMA event abort\n");
			rval = -EINTR;
		} else {
			fma_run_msg = (struct fma_run_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG_FMA].virt;
			spin_lock(&cavalry->cavalry_cmd_msg_fma_lock);
			if (fma_run_msg->msg_code == cmd_to_msg(FMA_RUN_CMD)) {
				switch (fma_run_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				case MSG_RVAL_FMA_INSUFFICIENT_DRAM:
					prt_err("DRAM size 0x%x is insufficient for FMA!\n", fma_run_cmd->output_size);
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_fma_lock);
		}
		if (run_done) {
			fma_run.rval = fma_run_msg->rval;
			fma_run.result_score_daddr = fma_run_msg->result_score_daddr;
			fma_run.result_score_size = fma_run_msg->result_score_size;
			fma_run.result_index_daddr = fma_run_msg->result_index_daddr;
			fma_run.result_index_size = fma_run_msg->result_index_size;
			fma_run.temporal_coord_daddr = fma_run_msg->temporal_coord_daddr;
			fma_run.temporal_coord_size = fma_run_msg->temporal_coord_size;
			fma_run.start_tick = fma_run_msg->start_tick;
			fma_run.end_tick = fma_run_msg->end_tick;

			if (copy_to_user(arg, &fma_run, sizeof(fma_run))) {
				rval = -EFAULT;
			}
		}
	}

	up(&cavalry->cavalry_fma_sem);

	return rval;
}

static int cavalry_check_log_level(struct ambarella_cavalry *cavalry,
	struct cavalry_set_log_level *set_log_level)
{
	if ((set_log_level->log_level < CAVALRY_LOG_LEVEL_FIRST) ||
		(set_log_level->log_level > CAVALRY_LOG_LEVEL_LAST)) {
		prt_err("Log level [%d] should be within [%u-%u].\n",
			set_log_level->log_level, CAVALRY_LOG_LEVEL_FIRST, CAVALRY_LOG_LEVEL_LAST);
		return -1;
	}

	return 0;
}

int cavalry_set_log_level(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_set_log_level set_log_level;
	struct set_log_level_cmd *set_log_level_cmd = NULL;
	struct set_log_level_msg *set_log_level_msg = NULL;
	int rval = 0, ret = 0, run_done = 0;

	down(&cavalry->cavalry_vp_sem);
	if (cavalry->is_vp_hang) {
		up(&cavalry->cavalry_vp_sem);
		prt_err("VP hangs, Can't accept new request.\n");
		return -EFAULT;
	}

	if (copy_from_user(&set_log_level, arg, sizeof(struct cavalry_set_log_level))) {
		up(&cavalry->cavalry_vp_sem);
		return -EFAULT;
	}

	if (cavalry_check_log_level(cavalry, &set_log_level) < 0) {
		up(&cavalry->cavalry_vp_sem);
		return -EINVAL;
	}

	spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (!atomic_read(&cavalry->is_vp_started)) {
		spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		up(&cavalry->cavalry_vp_sem);
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	do {
		atomic_set(&cavalry->is_vp_done, 0);

		// Fill CMD
		set_log_level_cmd = (struct set_log_level_cmd *)cavalry->cavalry_mem_info[CAVALRY_MEM_CMD].virt;
		set_log_level_cmd->cmd_code = SET_LOG_LEVEL_CMD;
		set_log_level_cmd->log_level = set_log_level.log_level;
	} while (0);

	spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);

	if (!rval) {
		visorc_kick_vp();

		ret = wait_event_timeout(cavalry->cavalry_vp_wq,
			atomic_read(&cavalry->is_vp_done) == 1, msecs_to_jiffies(TIMEOUT_MS_MIDDLE));

		if (ret == 0) {
			prt_err("Wait VP event timeout\n");
			rval = -EFAULT;
		} else if (ret < 0) {
			prt_err("Wait VP event abort\n");
			rval = -EINTR;
		} else {
			set_log_level_msg = (struct set_log_level_msg *)cavalry->cavalry_mem_info[CAVALRY_MEM_MSG].virt;
			spin_lock(&cavalry->cavalry_cmd_msg_vp_lock);
			if (set_log_level_msg->msg_code == cmd_to_msg(SET_LOG_LEVEL_CMD)) {
				switch (set_log_level_msg->rval) {
				case MSG_RVAL_INVALID_CMD:
					prt_err("Unsupported CMD!\n");
					rval = -EFAULT;
					break;
				default:
					run_done = 1;
					rval = 0;
					break;
				}
			}
			spin_unlock(&cavalry->cavalry_cmd_msg_vp_lock);
		}
		if (run_done) {
			set_log_level.rval = set_log_level_msg->rval;

			if (copy_to_user(arg, &set_log_level, sizeof(set_log_level))) {
				rval = -EFAULT;
			}
		}
	}

	up(&cavalry->cavalry_vp_sem);

	return rval;
}

#endif /*-= Moved to cavalry_ioctl.c for integration with CVFLOW =--------^^^-*/
