/*
 * Copyright (C) 2017-2029, Ambarella, Inc.
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

#if defined(CONFIG_ARCH_AMBA_CAMERA)
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <soc/ambarella/misc.h>

typedef struct {
    uint32_t  ClientRequestStatis[32];    /* Statistics for each client's number of requests */
    uint32_t  ClientBurstStatis[32];      /* Statistics for each client's number of bursts */
    uint32_t  ClientMaskWriteStatis[32];  /* Statistics for each client's number of masked write bursts */
} AMBA_DRAMC_STATIS_s;

extern int ambarella_scm_dramc_enable_statis_ctrl(void);
extern int ambarella_scm_dramc_disable_statis_ctrl(void);
extern AMBA_DRAMC_STATIS_s* ambarella_scm_dramc_get_statis_info(void);

static struct workqueue_struct *dramc_stats_wq;
static int dramc_stats_enable = 0;

static int ambarella_dramc_stats_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "usage:\n");
	seq_printf(m, "\techo [0(disable)/1(enable)] > /proc/ambarella/dramc_stats\n");

	if(dramc_stats_enable) {
	    seq_printf(m, "DRAMC stats is now enabled.\n");
        ambarella_scm_dramc_get_statis_info();
	    seq_printf(m, "DRAMC stats has been collected.\n");
	    seq_printf(m, "DRAMC stats is now disabled.\n");
        dramc_stats_enable = 0;
    }
    else {
	    seq_printf(m, "DRAMC stats is now disabled.\n");
    }

	return 0;
}

static int ambarella_dramc_stats_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ambarella_dramc_stats_proc_show, PDE_DATA(inode));
}

static ssize_t ambarella_dramc_stats_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *ppos)
{
	int val, ret;

	ret = kstrtoint_from_user(buffer, count, 0, &val);
	if (ret)
		return ret;

	if(val == 0) {
        ambarella_scm_dramc_disable_statis_ctrl();
        dramc_stats_enable = 0;
	} else if(val == 1){
        ambarella_scm_dramc_enable_statis_ctrl();
        dramc_stats_enable = 1;
	} else {
		pr_err("Invalid argument!\n");
		return -EINVAL;
	}

	return count;
}

static const struct proc_ops proc_dramc_stats_fops = {
	.proc_open = ambarella_dramc_stats_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_write = ambarella_dramc_stats_proc_write,
	.proc_release = single_release,
};

static int __init ambarella_dramc_stats_init(void)
{
	proc_create_data("dramc_stats", S_IRUGO|S_IWUSR,
			ambarella_proc_dir(), &proc_dramc_stats_fops, NULL);

	dramc_stats_wq = alloc_workqueue("dramc_stats_wq", WQ_UNBOUND | WQ_MEM_RECLAIM, 0);

	if (!dramc_stats_wq)
		return -ENOMEM;

#if (DRAMC_STATS_ENABLE_AT_INIT == 1)
    ambarella_scm_dramc_enable_statis_ctrl();
    dramc_stats_enable = 1;
#endif

	return 0;
}

static void __exit ambarella_dramc_stats_exit(void)
{
	destroy_workqueue(dramc_stats_wq);
}

module_init(ambarella_dramc_stats_init);
module_exit(ambarella_dramc_stats_exit);
#endif

