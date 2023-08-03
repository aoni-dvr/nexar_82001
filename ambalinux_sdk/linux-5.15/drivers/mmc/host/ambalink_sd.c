/*
 * drivers/mmc/host/ambalink_sd.c
 *
 * Copyright (C) 2004-2099, Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/semaphore.h>
#include <linux/of.h>
#include <linux/mmc/host.h>
#include <linux/interrupt.h>
#include <linux/aipc/ipc_mutex.h>
#include <linux/aipc/rpmsg_sd.h>


#define NUM_AMBALINK_SD (3)

#if defined(CONFIG_AMBALINK_SD)
extern struct rpdev_sdinfo G_rpdev_sdinfo[NUM_AMBALINK_SD];
#endif

struct mmc_host *G_mmc[NUM_AMBALINK_SD];
EXPORT_SYMBOL(G_mmc);

static struct semaphore G_system_event_sem[NUM_AMBALINK_SD];
static unsigned int G_irq[NUM_AMBALINK_SD];

/* ==========================================================================*/

void ambarella_sd_init_bus_lock(struct mmc_host *mmc, unsigned int irq)
{
	u32 host_id;

	host_id = (!strcmp(mmc->parent->of_node->name, "sdmmc0")) ? 0 :
		  (!strcmp(mmc->parent->of_node->name, "sdmmc1")) ? 1 : 2;

	G_irq[host_id] = irq;
}
EXPORT_SYMBOL_GPL(ambarella_sd_init_bus_lock);


#if defined(CONFIG_AMBALINK_SD)
/**
 * Service initialization.
 */
int ambarella_sd_rpmsg_sdinfo_init(struct mmc_host *mmc)
{
	u32 host_id;

	host_id = (!strcmp(mmc->parent->of_node->name, "sdmmc0")) ? 0 :
		  (!strcmp(mmc->parent->of_node->name, "sdmmc1")) ? 1 : 2;

	G_rpdev_sdinfo[host_id].host_id = host_id;
	rpmsg_sdinfo_get((void *) &G_rpdev_sdinfo[host_id]);

	G_mmc[host_id] = mmc;

	ambsd_prt("sd[%d] mmc index %d\n", host_id, mmc->index);
	ambsd_prt("sd[%d] is_init   = %d\n", host_id, G_rpdev_sdinfo[host_id].is_init);
	ambsd_prt("sd[%d] is_sdmem  = %d\n", host_id, G_rpdev_sdinfo[host_id].is_sdmem);
	ambsd_prt("sd[%d] is_mmc    = %d\n", host_id, G_rpdev_sdinfo[host_id].is_mmc);
	ambsd_prt("sd[%d] is_sdio   = %d\n", host_id, G_rpdev_sdinfo[host_id].is_sdio);
	ambsd_prt("sd[%d] bus_width = %d\n", host_id, G_rpdev_sdinfo[host_id].bus_width);
	ambsd_prt("sd[%d] clk       = %d\n", host_id, G_rpdev_sdinfo[host_id].clk);

	return 0;
}
EXPORT_SYMBOL_GPL(ambarella_sd_rpmsg_sdinfo_init);

/**
 * Enable to use the rpmsg sdinfo.
 */
void ambarella_sd_rpmsg_sdinfo_en(struct mmc_host *mmc, u8 enable)
{
	u32 host_id;

	host_id = (!strcmp(mmc->parent->of_node->name, "sdmmc0")) ? 0 :
		  (!strcmp(mmc->parent->of_node->name, "sdmmc1")) ? 1 : 2;

	if (enable)
		G_rpdev_sdinfo[host_id].from_rpmsg = enable;
	else
		memset(&G_rpdev_sdinfo[host_id], 0x0, sizeof(struct rpdev_sdinfo));
}
EXPORT_SYMBOL_GPL(ambarella_sd_rpmsg_sdinfo_en);
#endif // defined(CONFIG_AMBALINK_SD)

void ambarella_sd_request_bus(struct mmc_host *mmc)
{
	u32 host_id;
	unsigned int irq;

	host_id = (!strcmp(mmc->parent->of_node->name, "sdmmc0")) ? 0 :
		  (!strcmp(mmc->parent->of_node->name, "sdmmc1")) ? 1 : 2;

	down(&G_system_event_sem[host_id]);

	irq = G_irq[host_id];

	/* Skip locking SD1 (SDIO for WiFi) and SD2. */
	/* Because they are not shared between dual OS. */
	if (!strcmp(mmc->parent->of_node->name, "sdmmc0")) {
		aipc_mutex_lock(AMBA_IPC_MUTEX_SD0);

		disable_irq(irq);
		enable_irq(irq);
	} else if (!strcmp(mmc->parent->of_node->name, "sdmmc1")) {
		/*
		aipc_mutex_lock(AMBA_IPC_MUTEX_SD1);

		disable_irq(irq);
		enable_irq(irq);
		*/
	} else if (!strcmp(mmc->parent->of_node->name, "sdmmc2")) {
		/*
		aipc_mutex_lock(AMBA_IPC_MUTEX_SD2);

		disable_irq(irq);
		enable_irq(irq);
		*/
	} else {
		pr_err("%s: unknown SD host(%s)!!", __func__, mmc->parent->of_node->name);
	}
}
EXPORT_SYMBOL_GPL(ambarella_sd_request_bus);

void ambarella_sd_release_bus(struct mmc_host *mmc)
{
	u32 host_id;

	host_id = (!strcmp(mmc->parent->of_node->name, "sdmmc0")) ? 0 :
		  (!strcmp(mmc->parent->of_node->name, "sdmmc1")) ? 1 : 2;

	/* Skip unlocking SD1 (SDIO for WiFi) and SD2. */
	/* Because they are not shared between dual OS. */
	if (!strcmp(mmc->parent->of_node->name, "sdmmc0")) {
		aipc_mutex_unlock(AMBA_IPC_MUTEX_SD0);
	} else if (!strcmp(mmc->parent->of_node->name, "sdmmc1")) {
		//aipc_mutex_unlock(AMBA_IPC_MUTEX_SD1);
	} else if (!strcmp(mmc->parent->of_node->name, "sdmmc2")) {
		//aipc_mutex_unlock(AMBA_IPC_MUTEX_SD2);
	} else {
		pr_err("%s: unknown SD host(%s)!!", __func__, mmc->parent->of_node->name);
	}

	up(&G_system_event_sem[host_id]);
}
EXPORT_SYMBOL_GPL(ambarella_sd_release_bus);


static int __init ambalink_sd_init(void)
{
	int rval = 0;
	int i;

	for (i = 0; i < NUM_AMBALINK_SD; i++) {
		sema_init(&G_system_event_sem[i], 1);
	}

	return rval;
}
arch_initcall(ambalink_sd_init); // Init early

static void __exit ambalink_sd_exit(void)
{
}
module_exit(ambalink_sd_exit);


MODULE_DESCRIPTION("Lock/RPMSG for Ambarella Secure Digital Host Controller Interface driver");
MODULE_LICENSE("GPL v2");

