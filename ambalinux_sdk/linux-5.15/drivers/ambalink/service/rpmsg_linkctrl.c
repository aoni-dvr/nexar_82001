/*
 *
 * Copyright (C) 2012-2016, Ambarella, Inc.
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

#include <linux/ioport.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rpmsg.h>
#include <linux/err.h>
#include <linux/kthread.h>
#include <linux/remoteproc.h>
#include <linux/suspend.h>
#include <linux/types.h>
#include <trace/events/power.h>
#include <asm/suspend.h>

#include <soc/ambarella/ambalink/ambalink_cfg.h>
#include <soc/ambarella/iav_helper.h>

/*----------------------------------------------------------------------------*/
#if 0 // debug
    #define dbg_trace(str, args...)     printk("@@@ %s[#%d]: " str, __func__, __LINE__, ## args)
#else
    #define dbg_trace(str, args...)
#endif


typedef u32 UINT32;
typedef u64 UINT64;

typedef enum _AMBA_RPDEV_LINK_CTRL_CMD_e_ {
	LINK_CTRL_CMD_SUSPEND_PREPARE = 0,
	LINK_CTRL_CMD_SUSPEND_DONE,
	LINK_CTRL_CMD_SUSPEND_EXIT,
	LINK_CTRL_CMD_SUSPEND_ACK,
	LINK_CTRL_CMD_SUSPEND,
	LINK_CTRL_CMD_GPIO_LINUX_ONLY_LIST,
	LINK_CTRL_CMD_GET_MEM_INFO,
	LINK_CTRL_CMD_SET_RTOS_MEM,
	LINK_CTRL_CMD_SET_WIFI_CONF,
	LINK_CTRL_CMD_REBOOT
} AMBA_RPDEV_LINK_CTRL_CMD_e;

/**
 * LinkCtrlSuspendLinux related data structure.
 */
typedef enum _AMBA_RPDEV_LINK_CTRL_SUSPEND_TARGET_e_ {
	LINK_CTRL_CMD_SUSPEND_TO_DISK = 0,
	LINK_CTRL_CMD_SUSPEND_TO_DRAM
} AMBA_RPDEV_LINK_CTRL_SUSPEND_TARGET_e;

typedef struct _AMBA_RPDEV_LINK_CTRL_CMD_s_ {
	UINT32  Cmd;
	UINT64  Param1;
	UINT64  Param2;
} AMBA_RPDEV_LINK_CTRL_CMD_s;

typedef struct _LINK_CTRL_MEMINFO_CMD_s_ {
	UINT32	Cmd;
        UINT64	RtosStart;
        UINT64	RtosEnd;
        UINT64	RtosSystemStart;
        UINT64	RtosSystemEnd;
        UINT64	CachedHeapStart;
        UINT64	CachedHeapEnd;
        UINT64	NonCachedHeapStart;
        UINT64	NonCachedHeapEnd;
} LINK_CTRL_MEMINFO_CMD_s;

typedef struct _LINK_CTRL_WIFI_CONF_s_ {
	UINT32	Cmd;
        char 	Buf[512];
} LINK_CTRL_WIFI_CONF_s;

typedef enum _AMBA_RPDEV_LINK_CTRL_MEMTYPE_e_ {
	LINK_CTRL_MEMTYPE_HEAP = 0,
	LINK_CTRL_MEMTYPE_DSP
} AMBA_RPDEV_LINK_CTRL_MEMTYPE_e;

struct _AMBA_RPDEV_LINK_CTRL_MEMINFO_s_ {
	UINT64 base_addr;
	UINT64 phys_addr;
	UINT32 size;
	UINT32 padding;
	UINT32 padding1;
	UINT32 padding2;
	UINT32 padding3;
	UINT32 padding4;
} __attribute__((aligned(32), packed));
typedef struct _AMBA_RPDEV_LINK_CTRL_MEMINFO_s_ AMBA_RPDEV_LINK_CTRL_MEMINFO_t;

extern int suspend_status;

DECLARE_COMPLETION(linkctrl_comp);
struct rpmsg_device *rpdev_linkctrl;
int hibernation_start = 0;
static AMBA_RPDEV_LINK_CTRL_MEMINFO_t rpdev_meminfo;

static struct resource amba_rtosmem = {
       .name   = "RTOS memory region",
       .start  = 0x0,
       .end    = 0x0,
       .flags  = IORESOURCE_MEM | IORESOURCE_BUSY,
};

/*
 * Standard memory resources
 */
static struct resource amba_heapmem[] = {
	{
		.name = "RTOS System",
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM | IORESOURCE_EXCLUSIVE | IORESOURCE_BUSY
	},
	{
		.name = "Cached heap",
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM
	},
	{
		.name = "Non-Cached heap",
		.start = 0,
		.end = 0,
		.flags = IORESOURCE_MEM
	},
};

/*----------------------------------------------------------------------------*/
static int rpmsg_linkctrl_ack(void *data)
{
	complete(&linkctrl_comp);

	return 0;
}

/*----------------------------------------------------------------------------*/
static int rpmsg_linkctrl_suspend(void *data)
{
	extern int amba_state_store(void *suspend_to);
	AMBA_RPDEV_LINK_CTRL_CMD_s *ctrl_cmd = (AMBA_RPDEV_LINK_CTRL_CMD_s *) data;
	struct task_struct *task;

	task = kthread_run(amba_state_store, (void *)(uintptr_t) ctrl_cmd->Param1,
	                   "linkctrl_suspend");
	if (IS_ERR(task))
		return PTR_ERR(task);

	return 0;
}

/*----------------------------------------------------------------------------*/
static u32 ambalink_gpio_linux_only_mask[8];
static void ambarella_gpio_create_linux_only_mask(u32 gpio)
{
	ambalink_gpio_linux_only_mask[gpio >> 5] |= 1 << (gpio % 32);
}

static int rpmsg_linkctrl_gpio_linux_only_list(void *data)
{
	AMBA_RPDEV_LINK_CTRL_CMD_s *ctrl_cmd = (AMBA_RPDEV_LINK_CTRL_CMD_s *) data;
	u8 *p;
	int ret;
	u32 gpio;
	char *virt;

	virt = (char *) ambalink_phys_to_virt(ctrl_cmd->Param1);
	if(virt == NULL)
		return 0;

    ambcache_inv_range((void *) virt, strlen((char*) virt));

    if (*virt != 0x0) {
		while((p = (u8 *) strsep((char **) &virt, ", "))) {
			ret = kstrtouint(p, 0, &gpio);
			if (ret < 0) {
				continue;
			}

			ambarella_gpio_create_linux_only_mask(gpio);
		}
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
static int rpmsg_linkctrl_set_rtos_mem(void *data)
{
	int rval = 0;
	LINK_CTRL_MEMINFO_CMD_s *meminfo_cmd = (LINK_CTRL_MEMINFO_CMD_s *) data;

	amba_rtosmem.start = (resource_size_t) meminfo_cmd->RtosStart;
	amba_rtosmem.end = (resource_size_t) meminfo_cmd->RtosEnd;

	rval = request_resource(&iomem_resource, &amba_rtosmem);
	if (rval < 0 && rval != -EBUSY) {
		printk("Ambarella RTOS memory resource %pR cannot be added\n", &amba_rtosmem);
	}

	amba_heapmem[0].start	= (resource_size_t) meminfo_cmd->RtosSystemStart;
	amba_heapmem[0].end	= (resource_size_t) meminfo_cmd->RtosSystemEnd - 1;
	amba_heapmem[1].start	= (resource_size_t) meminfo_cmd->CachedHeapStart;
	amba_heapmem[1].end	= (resource_size_t) meminfo_cmd->CachedHeapEnd - 1;
	amba_heapmem[2].start	= (resource_size_t) meminfo_cmd->NonCachedHeapStart;
	amba_heapmem[2].end	= (resource_size_t) meminfo_cmd->NonCachedHeapEnd - 1;

	rval = request_resource(&amba_rtosmem, &amba_heapmem[0]);
	if (rval < 0 && rval != -EBUSY) {
		printk("Ambarella RTOS system memory resource %pR cannot be added\n",
			&amba_heapmem[0]);
	}

	rval = request_resource(&amba_rtosmem, &amba_heapmem[1]);
	if (rval < 0 && rval != -EBUSY) {
		printk("Ambarella RTOS cached heap memory resource %pR cannot be added\n",
			&amba_heapmem[1]);
	}

	rval = request_resource(&amba_rtosmem, &amba_heapmem[2]);
	if (rval < 0 && rval != -EBUSY) {
		printk("Ambarella RTOS non-cached heap memory resource %pR cannot be added\n",
			&amba_heapmem[2]);
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
static LINK_CTRL_WIFI_CONF_s LinkCtrlWiFiCmd;
static int wifi_conf_proc_read(struct seq_file *m, void *v)
{
	seq_printf(m, "%s", LinkCtrlWiFiCmd.Buf);
	return 0;
}
static int wifi_conf_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, wifi_conf_proc_read, NULL);
}
static const struct proc_ops proc_fops_wifi_conf = {
	.proc_open = wifi_conf_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
static int rpmsg_linkctrl_set_wifi_conf(void *data)
{
	static int wifi_settled = 0;
	LINK_CTRL_WIFI_CONF_s *ctrl_cmd = (LINK_CTRL_WIFI_CONF_s *) data;

	memcpy(&LinkCtrlWiFiCmd, ctrl_cmd, sizeof(ctrl_cmd->Cmd) + strlen(ctrl_cmd->Buf) + 1);
	if (0 == wifi_settled) {
		wifi_settled = 1;
		proc_create("wifi.conf", S_IRUGO | S_IWUSR, get_ambarella_proc_dir(),
			&proc_fops_wifi_conf);
	}

	printk("set /proc/ambarella/wifi.conf\n");

	dbg_trace("%s", LinkCtrlWiFiCmd.Buf);

	return 0;
}

/*----------------------------------------------------------------------------*/
int rpmsg_linkctrl_cmd_get_mem_info(u8 type, void **base, void **phys, u32 *size)
{
	AMBA_RPDEV_LINK_CTRL_CMD_s ctrl_cmd;
	u64 phy_addr;

	dbg_trace("type=%d\n", type);

	phy_addr = (u64) ambalink_virt_to_phys((uintptr_t)&rpdev_meminfo);

	memset(&ctrl_cmd, 0x0, sizeof(ctrl_cmd));
	ctrl_cmd.Cmd = LINK_CTRL_CMD_GET_MEM_INFO;
	ctrl_cmd.Param1 = (u64) type;
	ctrl_cmd.Param2 = phy_addr;

	if (rpdev_linkctrl) {
		rpmsg_send(rpdev_linkctrl->ept, &ctrl_cmd, sizeof(ctrl_cmd));
	} else {
		dbg_trace("NULL rpdev_linkctrl\n");
	}

	wait_for_completion(&linkctrl_comp);

	ambcache_inv_range((void *) ambalink_phys_to_virt(phy_addr), 32);

	*base = (void *)(uintptr_t) rpdev_meminfo.base_addr;
	*phys = (void *)(uintptr_t) rpdev_meminfo.phys_addr;
	*size = rpdev_meminfo.size;

	dbg_trace("type= %u, base= x%llx, phys= x%llx, size= x%x\n",
	    type, rpdev_meminfo.base_addr, rpdev_meminfo.phys_addr, rpdev_meminfo.size);

	return 0;
}
EXPORT_SYMBOL(rpmsg_linkctrl_cmd_get_mem_info);

#ifdef CONFIG_HIBERNATION
extern void *ambalink_image_info;
static unsigned long ambalink_image_info_paddr;
#endif

int rpmsg_linkctrl_cmd_suspend_prepare(int flag)
{
#ifdef CONFIG_HIBERNATION
	AMBA_RPDEV_LINK_CTRL_CMD_s ctrl_cmd;

	//dbg_trace("0x%016lx\n", info);

	memset(&ctrl_cmd, 0x0, sizeof(ctrl_cmd));
	ctrl_cmd.Cmd = LINK_CTRL_CMD_SUSPEND_PREPARE;
	ctrl_cmd.Param1 = ambalink_virt_to_phys((uintptr_t)&ambalink_image_info_paddr);

	if (rpdev_linkctrl) {
		rpmsg_send(rpdev_linkctrl->ept, &ctrl_cmd, sizeof(ctrl_cmd));
	} else {
		dbg_trace("NULL rpdev_linkctrl\n");
	}

	wait_for_completion(&linkctrl_comp);

	ambalink_image_info = (void *) ambalink_phys_to_virt(ambalink_image_info_paddr);

	hibernation_start = 1;
#endif
	return 0;
}
EXPORT_SYMBOL(rpmsg_linkctrl_cmd_suspend_prepare);

#if !defined(CONFIG_AMBALINK_LINKCTRL_MODULE)
int rpmsg_linkctrl_cmd_suspend_done(int flag)
{
	AMBA_RPDEV_LINK_CTRL_CMD_s ctrl_cmd;

	dbg_trace("\n");

	memset(&ctrl_cmd, 0x0, sizeof(ctrl_cmd));
	ctrl_cmd.Cmd = LINK_CTRL_CMD_SUSPEND_DONE;

	if (flag == PM_SUSPEND_MEM)
		ctrl_cmd.Param1 = LINK_CTRL_CMD_SUSPEND_TO_DRAM;
	else
		ctrl_cmd.Param1 = LINK_CTRL_CMD_SUSPEND_TO_DISK;

	ctrl_cmd.Param2 = (u64) ambalink_virt_to_phys((uintptr_t)cpu_resume);

	if (rpdev_linkctrl) {
		rpmsg_send(rpdev_linkctrl->ept, &ctrl_cmd, sizeof(ctrl_cmd));
	} else {
		dbg_trace("NULL rpdev_linkctrl\n");
	}

	return 0;
}
EXPORT_SYMBOL(rpmsg_linkctrl_cmd_suspend_done);
#endif

int rpmsg_linkctrl_cmd_suspend_exit(int flag)
{
	AMBA_RPDEV_LINK_CTRL_CMD_s ctrl_cmd;

	memset(&ctrl_cmd, 0x0, sizeof(ctrl_cmd));
	ctrl_cmd.Cmd = LINK_CTRL_CMD_SUSPEND_EXIT;

	if (suspend_status == 0)
		ctrl_cmd.Param1 = flag;
	else
		ctrl_cmd.Param1 = (UINT64) suspend_status;

	wait_for_completion(&linkctrl_comp);

	if (rpdev_linkctrl) {
		rpmsg_send(rpdev_linkctrl->ept, &ctrl_cmd, sizeof(ctrl_cmd));
	} else {
		dbg_trace("NULL rpdev_linkctrl\n");
	}

	hibernation_start = 0;

	return 0;
}
EXPORT_SYMBOL(rpmsg_linkctrl_cmd_suspend_exit);

int rpmsg_linkctrl_cmd_reboot(int flag)
{
	AMBA_RPDEV_LINK_CTRL_CMD_s ctrl_cmd;

	dbg_trace("\n");

	memset(&ctrl_cmd, 0x0, sizeof(ctrl_cmd));
	ctrl_cmd.Cmd = LINK_CTRL_CMD_REBOOT;
	ctrl_cmd.Param1 = flag;

	if (rpdev_linkctrl) {
		rpmsg_send(rpdev_linkctrl->ept, &ctrl_cmd, sizeof(ctrl_cmd));
	} else {
		dbg_trace("NULL rpdev_linkctrl\n");
	}

	return 0;
}
EXPORT_SYMBOL(rpmsg_linkctrl_cmd_reboot);

/*----------------------------------------------------------------------------*/
typedef int (*PROC_FUNC)(void *data);
static PROC_FUNC linkctrl_proc_list[] = {
	rpmsg_linkctrl_ack,
	rpmsg_linkctrl_suspend,
	rpmsg_linkctrl_gpio_linux_only_list,
	rpmsg_linkctrl_set_rtos_mem,
	rpmsg_linkctrl_set_wifi_conf,
};

static int rpmsg_linkctrl_cb(struct rpmsg_device *rpdev, void *data, int len,
                              void *priv, u32 src)
{
	int rval = 0;
	AMBA_RPDEV_LINK_CTRL_CMD_s *msg = (AMBA_RPDEV_LINK_CTRL_CMD_s *) data;

	dbg_trace("recv: cmd= %d, param1= x%llx, param2= x%llx\n",
        msg->Cmd, msg->Param1, msg->Param2);

	switch (msg->Cmd) {
	case LINK_CTRL_CMD_SUSPEND_ACK:
		rval = linkctrl_proc_list[0](data);
		break;
	case LINK_CTRL_CMD_SUSPEND:
		rval = linkctrl_proc_list[1](data);
		break;
	case LINK_CTRL_CMD_GPIO_LINUX_ONLY_LIST:
		rval = linkctrl_proc_list[2](data);
		break;
	case LINK_CTRL_CMD_SET_RTOS_MEM:
		rval = linkctrl_proc_list[3](data);
		break;

	case LINK_CTRL_CMD_SET_WIFI_CONF:
		rval = linkctrl_proc_list[4](data);
		break;
	default:
		break;
	}

	return rval;
}

/*
 * This synchronization is implemented by mutually excluding regular CPU
 * hotplug and Suspend/Hibernate call paths by hooking onto the Suspend/
 * Hibernate notifications.
 */
static int rpmsg_linkctrl_pm_callback(struct notifier_block *nb,
			unsigned long action, void *ptr)
{
	switch (action) {
	case PM_SUSPEND_PREPARE:
		rpmsg_linkctrl_cmd_suspend_prepare(LINK_CTRL_CMD_SUSPEND_TO_DRAM);
		break;
	case PM_HIBERNATION_PREPARE:
		rpmsg_linkctrl_cmd_suspend_prepare(LINK_CTRL_CMD_SUSPEND_TO_DISK);
		break;
	case PM_POST_SUSPEND:
		rpmsg_linkctrl_cmd_suspend_exit(LINK_CTRL_CMD_SUSPEND_TO_DRAM);
		break;
	case PM_POST_HIBERNATION:
		rpmsg_linkctrl_cmd_suspend_exit(LINK_CTRL_CMD_SUSPEND_TO_DISK);
		break;
	case PM_POST_RESTORE:
		rpmsg_linkctrl_cmd_suspend_exit(LINK_CTRL_CMD_SUSPEND_TO_DRAM);
		break;
	default:
		return NOTIFY_DONE;
	}

	return NOTIFY_OK;
}

static struct notifier_block fn_rpmsg_linkctrl_pm_callback_nb = {
			.notifier_call = rpmsg_linkctrl_pm_callback,
			.priority = 0 };

static int rpmsg_linkctrl_probe(struct rpmsg_device *rpdev)
{
	int ret = 0;
	struct rpmsg_channel_info chinfo;

	rpdev_linkctrl = rpdev;

	strncpy(chinfo.name, rpdev->id.name, sizeof(chinfo.name));
	chinfo.src = RPMSG_ADDR_ANY;
	chinfo.dst = rpdev->dst;

	rpmsg_send(rpdev->ept, &chinfo, sizeof(chinfo));

	register_pm_notifier(&fn_rpmsg_linkctrl_pm_callback_nb);

	return ret;
}

static void rpmsg_linkctrl_remove(struct rpmsg_device *rpdev)
{
}

static struct rpmsg_device_id rpmsg_linkctrl_id_table[] = {
	{ .name = "link_ctrl", },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_linkctrl_id_table);

static struct rpmsg_driver rpmsg_linkctrl_driver = {
	.drv.name   = KBUILD_MODNAME,
	.drv.owner  = THIS_MODULE,
	.id_table   = rpmsg_linkctrl_id_table,
	.probe      = rpmsg_linkctrl_probe,
	.callback   = rpmsg_linkctrl_cb,
	.remove     = rpmsg_linkctrl_remove,
};

static int __init rpmsg_linkctrl_init(void)
{
	return register_rpmsg_driver(&rpmsg_linkctrl_driver);
}

static void __exit rpmsg_linkctrl_fini(void)
{
	int rval;

	rval = release_resource(&amba_heapmem[2]);
	if (rval)
		pr_err("release_resource(&amba_heapmem[2]) failed...\n");
	rval = release_resource(&amba_heapmem[1]);
	if (rval)
		pr_err("release_resource(&amba_heapmem[1]) failed...\n");
	rval = release_resource(&amba_heapmem[0]);
	if (rval)
		pr_err("release_resource(&amba_heapmem[0]) failed...\n");
	rval = release_resource(&amba_rtosmem);
	if (rval)
		pr_err("release_resource(&amba_rtosmem) failed...\n");

	unregister_pm_notifier(&fn_rpmsg_linkctrl_pm_callback_nb);

	unregister_rpmsg_driver(&rpmsg_linkctrl_driver);
}

module_init(rpmsg_linkctrl_init);
module_exit(rpmsg_linkctrl_fini);

MODULE_DESCRIPTION("RPMSG AmbaRpdev_LinkCtrl Server");
MODULE_LICENSE("GPL");
