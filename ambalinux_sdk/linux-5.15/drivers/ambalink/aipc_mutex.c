/*
 * arch/arm/plat-ambarella/misc/aipc_mutex.c
 *
 * Authors:
 *  Joey Li <jli@ambarella.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * Copyright (C) 2013-2015, Ambarella Inc.
 */

#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/io.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/irqdomain.h>
#include <linux/seq_file.h>
#include <asm/io.h>
#include <linux/uaccess.h>

#include <linux/aipc/ipc_mutex.h>
#include <soc/ambarella/ambalink/ambalink_cfg.h>
#include <soc/ambarella/iav_helper.h>

extern void __aipc_spin_unlock_irqrestore(void *lock, unsigned long flags);
extern void __aipc_spin_lock_irqsave(void *lock, unsigned long *flags);

#ifdef CONFIG_AMBALINK_GUEST_2
#define OWNER_IS_LOCAL(x)   ((x)->owner == AMBALINK_CORE_GUEST2)
#else
#define OWNER_IS_LOCAL(x)   ((x)->owner == AMBALINK_CORE_GUEST1)
#endif
#define OWNER_IS_EMPTY(x)   ((x)->owner == 0)
#define OWNER_IS_REMOTE(x)  (!OWNER_IS_LOCAL(x) && !OWNER_IS_EMPTY(x))

#define CORTEX_CORE
typedef struct {
	unsigned int    slock;
	unsigned char   owner;        // entity that currently owns the mutex
	unsigned char   wait_list;    // entities that are waiting for the mutex
	char            padding[10];
} amutex_share_t;

typedef struct {
	struct mutex      mutex;
	struct completion completion;
	unsigned int      count;
} amutex_local_t;

typedef struct {
	amutex_local_t local[AMBA_IPC_NUM_MUTEX];
	amutex_share_t *share;
        struct regmap *reg_ahb_scr;
} amutex_db;

static amutex_db lock_set;
static int ipc_mutex_inited = 0;

static u32 rvq_rx_irq;          /* rtos  kick svq -> linux rvq    */
static u32 sp_swi_set_offset;   /* set scratchpad sw interrupt    */
static u32 sp_swi_clear_offset; /* clear scratchpad sw interrupt  */
static u32 mtx_remote;          /* notify remote OS1 after unlock */
static u32 mtx_remote2;         /* notify remote OS2 after unlock */
//static u32 mtx_local;           /* local irq to wait for lock */

static int procfs_mutex_show(struct seq_file *m, void *v)
{
	seq_printf(m,
	           "\n"
	           "usage: echo id [op] > /proc/ambarella/mutex\n"
	           "    \"echo n +\" to lock mutex n\n"
	           "    \"echo n -\" to unlock mutex n\n"
	           "\n");

	return 0;
}

static ssize_t procfs_mutex_write(struct file *file,
                              const char __user *buffer, size_t count, loff_t *data)
{
	unsigned char str[128], op;
	int  id = 0;

	//memset(str, 0, sizeof(str));
	if (copy_from_user(str, buffer, count)) {
		printk(KERN_ERR "copy_from_user failed, aborting\n");
		return -EFAULT;
	}
	sscanf(str, "%d %c", &id, &op);

	switch (op) {
	case '+':
		printk("try to lock mutex %d, %p\n", id, current);
		aipc_mutex_lock(id);
		printk("done\n");
		break;
	case '-':
		printk("unlock mutex %d, %p\n", id, current);
		aipc_mutex_unlock(id);
		break;
	default:
		printk("unknow operations, "
		       "try \"cat /proc/ambarella/mutex\" for details\n");
	}

	return count;
}

static int procfs_mutex_open(struct inode *inode, struct file *file)
{
	return single_open(file, procfs_mutex_show, PDE_DATA(inode));
}

static const struct proc_ops proc_ipc_mutex_fops = {
	.proc_open = procfs_mutex_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_write = procfs_mutex_write,
	.proc_release = single_release,
};

static void init_procfs(void)
{
	struct proc_dir_entry *aipc_dev;

	aipc_dev = proc_create_data("mutex", S_IRUGO | S_IWUSR,
	                            get_ambarella_proc_dir(),
	                            &proc_ipc_mutex_fops, NULL);
}

static irqreturn_t ipc_mutex_isr(int irq, void *dev_id)
{
	int i;
	struct irq_data *irq_data;

	//printk("isr %d", irq);

	irq_data = irq_get_irq_data(irq);

	// clear the irq
	regmap_write(lock_set.reg_ahb_scr, sp_swi_clear_offset,
			0x1 << (irq_data->hwirq - rvq_rx_irq));

	// wake up
	for (i = 0; i < AMBA_IPC_NUM_MUTEX; i++) {
#ifdef CONFIG_AMBALINK_GUEST_2
		if (lock_set.share[i].wait_list & AMBALINK_CORE_GUEST2) {
#else
		if (lock_set.share[i].wait_list & AMBALINK_CORE_GUEST1) {
#endif
			complete_all(&lock_set.local[i].completion);
		}
	}
	return IRQ_HANDLED;
}

void aipc_mutex_lock(int id)
{
	unsigned long flags;
	amutex_share_t *share;
	amutex_local_t *local;
	u32 core_id;

	if (!ipc_mutex_inited) {
		/* FIXME: time_init will call clk_get_rate.
		 * But aipc_mutex_init is not called yet. */
		return;
	}

	if (id < 0 || id >= AMBA_IPC_NUM_MUTEX) {
		printk(KERN_ERR "%s: invalid id %d\n", __FUNCTION__, id);
		return;
	}
	share = &lock_set.share[id];
	local = &lock_set.local[id];
#ifdef CONFIG_AMBALINK_GUEST_2
	core_id = AMBALINK_CORE_GUEST2;
#else
	core_id = AMBALINK_CORE_GUEST1;
#endif

	// check repeatedly until we become the owner
	__aipc_spin_lock_irqsave((void *) &share->slock, &flags);
	{
		while (OWNER_IS_REMOTE(share)) {
			share->wait_list |= core_id;
			__aipc_spin_unlock_irqrestore((void *) &share->slock, flags);

			// wait for remote owner to finish
			/*
			 * use timeout api since the task priority lower than boss's
			 * may acquire the lock before linux.
			 * linux sleep and send rirq to rtos can let the lower
			 * priority task have chance to execute and release the lock.
			 **/
			wait_for_completion_timeout(&local->completion, 1);
			msleep(1);

			// lock and check again
			__aipc_spin_lock_irqsave((void *) &share->slock, &flags);
		}
		// set ourself as owner (note that we might be owner already)
		share->owner = core_id;
		share->wait_list &= ~core_id;
		local->count++;
	}
	__aipc_spin_unlock_irqrestore((void *) &share->slock, flags);

	// lock the local mutex
	mutex_lock(&local->mutex);
}
EXPORT_SYMBOL(aipc_mutex_lock);

void aipc_mutex_unlock(int id)
{
	unsigned long flags;
	amutex_share_t *share;
	amutex_local_t *local;

	if (!ipc_mutex_inited) {
		/* FIXME: time_init will call clk_get_rate.
		 * But aipc_mutex_init is not called yet. */
		return;
	}

	if (id < 0 || id >= AMBA_IPC_NUM_MUTEX) {
		printk(KERN_ERR "%s: invalid id %d\n", __FUNCTION__, id);
		return;
	}
	share = &lock_set.share[id];
	local = &lock_set.local[id];

	if (!OWNER_IS_LOCAL(share)) {
		// we are not the owner, there must be something wrong
		printk(KERN_ERR "aipc_mutex_unlock(%d) non-owner error\n", id);
		BUG();
		return;
	}

	// unlock local mutex
	mutex_unlock(&local->mutex);

	__aipc_spin_lock_irqsave((void *) &share->slock, &flags);
	if (--local->count == 0) {
		// We are done with the mutex,
		// now let other waiting core(s) to grab it
		share->owner = 0;
		if (share->wait_list) {
			// notify remote waiting core(s)
#ifdef CONFIG_AMBALINK_GUEST_2
			if (share->wait_list & AMBALINK_CORE_RTOS) {
				regmap_write_bits(lock_set.reg_ahb_scr, sp_swi_set_offset,
					0x1 << (mtx_remote - rvq_rx_irq),
					0x1 << (mtx_remote - rvq_rx_irq));
			} else {
				regmap_write_bits(lock_set.reg_ahb_scr, sp_swi_set_offset,
					0x1 << (mtx_remote2 - rvq_rx_irq),
					0x1 << (mtx_remote2 - rvq_rx_irq));
			}
#else
			if (share->wait_list & AMBALINK_CORE_GUEST2) {
				regmap_write_bits(lock_set.reg_ahb_scr, sp_swi_set_offset,
					0x1 << (mtx_remote2 - rvq_rx_irq),
					0x1 << (mtx_remote2 - rvq_rx_irq));
			} else {
				regmap_write_bits(lock_set.reg_ahb_scr, sp_swi_set_offset,
					0x1 << (mtx_remote - rvq_rx_irq),
					0x1 << (mtx_remote - rvq_rx_irq));
			}
#endif
			//printk("wakeup tx\n");
		}
	}
	__aipc_spin_unlock_irqrestore((void *) &share->slock, flags);
}
EXPORT_SYMBOL(aipc_mutex_unlock);

static const struct of_device_id ambarella_ipc_mutex_of_match[] __initconst = {
	{.compatible = "ambarella,ipc-mutex", },
	{},
};
MODULE_DEVICE_TABLE(of, ambarella_ipc_mutex_of_match);

static void __init aipc_mutex_of_init(struct device_node *np)
{
	int i, ret = 0;
	unsigned int irq;
	//struct irq_data *irq_data;

	/* axi software irq0 */
	ret = of_property_read_u32(np, "amb,scr-swibase", &rvq_rx_irq);
	if (ret < 0) {
		printk(KERN_ERR "Can not get amb,scr-swibase\n");
		return;
	}
	rvq_rx_irq += AXI_SPI_BASE;
	pr_debug("rvq_rx_irq %d\n", rvq_rx_irq);

	/* axi software irq4 */
	ret = of_property_read_u32(np, "amb,mtx_remote", &mtx_remote);
	if (ret < 0) {
		printk(KERN_ERR "Can not get amb,mtx_remote\n");
		return;
	}
	mtx_remote += AXI_SPI_BASE;
	pr_debug("mtx_remote %d\n", mtx_remote);

	/* axi software irq6 */
	ret = of_property_read_u32(np, "amb,mtx_remote2", &mtx_remote2);
	if (ret < 0) {
		printk(KERN_INFO "Can not get amb,mtx_remote2\n");
	} else {
		mtx_remote2 += AXI_SPI_BASE;
		pr_debug("mtx_remote2 %d\n", mtx_remote2);
	}

	/* set scratchpad sw interrupt   */
	ret = of_property_read_u32(np, "amb,scr-swiset", &sp_swi_set_offset);
	if (ret < 0) {
		printk(KERN_ERR "Can not get amb,scr-swiset\n");
		return;
	}
	pr_debug("sp_swi_set_offset 0x%x\n", sp_swi_set_offset);

	/* clear scratchpad sw interrupt */
	ret = of_property_read_u32(np, "amb,scr-swiclear", &sp_swi_clear_offset);
	if (ret < 0) {
		printk(KERN_ERR "Can not get amb,scr-swiset\n");
		return;
	}
	pr_debug("sp_swi_clear_offset 0x%x\n", sp_swi_clear_offset);

	// init shared part of aipc mutex memory
	lock_set.share = (amutex_share_t*) ambalink_phys_to_virt(ambalink_shm_layout.aipc_mutex_addr);
	if (2 == ambalink_shm_layout.ambalink_version) {
		memset(lock_set.share, 0, sizeof(amutex_share_t)*AMBA_IPC_NUM_MUTEX);
		printk(KERN_NOTICE "%s: ambalink v2\n", __func__);
	}

	printk(KERN_NOTICE "%s: aipc_mutex@0x%016lx\n", __func__, (unsigned long) lock_set.share);

	// init local part of aipc mutex memory
	for (i = 0; i < AMBA_IPC_NUM_MUTEX; i++) {
		lock_set.local[i].count = 0;
		mutex_init(&lock_set.local[i].mutex);
		init_completion(&lock_set.local[i].completion);
	}

	irq = irq_of_parse_and_map(np, 0);

	//irq_data = irq_get_irq_data(irq);
	//mtx_local = irq_data->hwirq;
	//pr_debug("mtx_local %d\n", mtx_local);

	// IRQ handling
	ret = request_irq(irq, ipc_mutex_isr,
		IRQF_SHARED | IRQF_TRIGGER_HIGH, "aipc_mutex", &lock_set);
	if (ret) {
		printk(KERN_ERR "aipc_mutex_init err %d while requesting irq %d\n",
		       ret, irq);
		ret = -EINVAL;
		goto exit;
	}

	lock_set.reg_ahb_scr = syscon_regmap_lookup_by_phandle(np, "amb,scr-regmap");
	if (IS_ERR(lock_set.reg_ahb_scr)) {
		printk(KERN_ERR "no scr regmap!\n");
		ret = PTR_ERR(lock_set.reg_ahb_scr);
		goto exit;
	}

	init_procfs();

	ipc_mutex_inited = 1;

	//printk("%s done\n", __func__);

exit:
	return;
}

static const struct of_device_id __ipc_mutex_of_table = {
	.compatible = "ambarella,ipc-mutex",
	.data = aipc_mutex_of_init	};

static int __init aipc_mutex_probe(void)
{
	int ret = 0;
	struct device_node *np;
	const struct of_device_id *match;
	of_init_fn_1 init_func;

	for_each_matching_node_and_match(np, &__ipc_mutex_of_table, &match) {
		if (!of_device_is_available(np))
			continue;

		init_func = match->data;
		init_func(np);
	}

	return ret;
}
subsys_initcall(aipc_mutex_probe);

