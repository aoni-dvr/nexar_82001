/*
 * Copyright (c) 2019-2019 Ambarella, Inc.
 * 2019/09/16 - [Zhikan Yang] created file
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
/* File source : "cavalry_core_dump.c"                                                            */
/* File size   : 5969 bytes                                                                       */
/* File md5sum : cd9b01831b62c95994e36a72f3a286c8                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing: None                                                             */
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
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include "cavalry.h"
#include "cavalry_mem.h"
#include "cavalry_core_dump.h"
#include "cavalry_print.h"
#include "cavalry_cma.h"

#define ROUND_UP_32(x)	round_up(x, 32)

int cavalry_vp_core_dump(struct ambarella_cavalry *cavalry,
	uint32_t finish_dags, uint16_t dag_loop_index)
{
	struct cma_mmb *vp_core_dump = NULL;
	struct cavalry_dag_desc *hang_dag = &cavalry->run_dags->dag_desc[finish_dags];
	struct cavalry_dag_desc *hang_dag_dump = NULL;
	struct cma_mmb *mmb = NULL, *_mmb = NULL;
	struct cavalry_port_desc *port_desc = NULL;
	uint32_t port_daddr = 0, port_size = 0;
	size_t core_dump_size = 0;
	uint32_t i = 0;
	void *curr_addr = NULL;
	void *temp = NULL;

	mutex_lock(&cavalry->cavalry_cma_mutex);

	core_dump_size = ROUND_UP_32(sizeof(struct cavalry_dag_desc)) +
		ROUND_UP_32(hang_dag->dvi_img_size);

	for (i = 0; i < hang_dag->port_cnt; i++) {
		core_dump_size += ROUND_UP_32(hang_dag->port_desc[i].port_dram_size);
	}

	core_dump_size = PAGE_ALIGN(core_dump_size);

	cavalry->vp_core_dump = cma_alloc(cavalry, core_dump_size, 0);
	vp_core_dump = cavalry->vp_core_dump;
	curr_addr = vp_core_dump->virt_addr;

	memcpy(curr_addr, (void *)hang_dag, sizeof(struct cavalry_dag_desc));
	curr_addr += ROUND_UP_32(sizeof(struct cavalry_dag_desc));

	if (!list_empty(&cavalry->mmb_list)) {
		// DVI
		list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
			if ((hang_dag->dvi_dram_addr >= mmb->phys_addr) &&
				(hang_dag->dvi_dram_addr + hang_dag->dvi_img_size <= mmb->phys_addr + mmb->size)) {
				temp = hang_dag->dvi_dram_addr - mmb->phys_addr + mmb->virt_addr;
				ambcache_inv_range(temp, hang_dag->dvi_img_size);
				memcpy(curr_addr, temp, hang_dag->dvi_img_size);
				curr_addr += ROUND_UP_32(hang_dag->dvi_img_size);
				break;
			}
		}

		// HMBs
		for (i = 0; i < hang_dag->port_cnt; i++) {
			port_desc = &hang_dag->port_desc[i];
			port_daddr = port_desc->port_dram_addr + port_desc->port_daddr_increment *
				dag_loop_index;
			port_size = hang_dag->port_desc[i].port_dram_size;
			list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
				if ((port_daddr >= mmb->phys_addr) &&
					(port_daddr + port_size <= mmb->phys_addr + mmb->size)) {
					temp = port_daddr - mmb->phys_addr + mmb->virt_addr;
					ambcache_inv_range(temp, port_size);
					memcpy(curr_addr, temp, port_size);
					curr_addr += ROUND_UP_32(port_size);
					break;
				}
			}
		}
	}

	// Update all DRAM address into offset
	hang_dag_dump = (struct cavalry_dag_desc *)vp_core_dump->virt_addr;
	hang_dag_dump->dvi_dram_addr = ROUND_UP_32(sizeof(struct cavalry_dag_desc));
	hang_dag_dump->port_desc[0].port_dram_addr = hang_dag_dump->dvi_dram_addr +
		ROUND_UP_32(hang_dag->dvi_img_size);
	for (i = 1; i < hang_dag->port_cnt; i++) {
		hang_dag_dump->port_desc[i].port_dram_addr = hang_dag_dump->port_desc[i - 1].port_dram_addr +
			ROUND_UP_32(hang_dag_dump->port_desc[i - 1].port_dram_size);
	}

	ambcache_clean_range(vp_core_dump->virt_addr, vp_core_dump->size);
	vp_core_dump->cache_en = 0;
	vp_core_dump->user_ref_cnt = 0;

	prt_err("VP core dump phys addr 0x%08llx, size 0x%08lx.\n",
		vp_core_dump->phys_addr, vp_core_dump->size);

	mutex_unlock(&cavalry->cavalry_cma_mutex);

	return 0;
}

int cavalry_query_vp_core_dump(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_vp_core_dump core_dump;
	int rval = 0;

	if (!atomic_read(&cavalry->is_vp_started)) {
		prt_err("Cavalry is not started yet.\n");
		return -EPERM;
	}

	core_dump.is_vp_hang = cavalry->is_vp_hang;
	if (cavalry->vp_core_dump) {
		core_dump.core_dump_daddr = cavalry->vp_core_dump->phys_addr;
		core_dump.core_dump_size = cavalry->vp_core_dump->size;
	} else {
		core_dump.core_dump_daddr = 0;
		core_dump.core_dump_size = 0;
	}
	if (copy_to_user(arg, &core_dump, sizeof(core_dump))) {
		rval = -EFAULT;
	}

	return rval;
}

int cavalry_core_dump_init(struct ambarella_cavalry *cavalry)
{
	// Do nothing now.
	return 0;
}

int cavalry_core_dump_exit(struct ambarella_cavalry *cavalry)
{
	// Do nothing now.
	return 0;
}

