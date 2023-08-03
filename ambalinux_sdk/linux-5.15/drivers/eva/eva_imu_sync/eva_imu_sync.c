// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * IMU Sync driver for Ambarella SoCs
 *
 * History:
 *	2022/11/22 - [Jason Chang] created file
 *
 * Copyright (C) 2012-2048, Ambarella, Inc.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/slab.h>
#include <linux/syscore_ops.h>
#include <linux/regmap.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/completion.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include "linux/eva/eva_audio_tick.h"
#include "linux/eva/eva_imu_sync.h"


static int imu_sync_len = eva_imu_sync_recoder_len_min;
module_param(imu_sync_len, int, 0);


#define imu_sync_todo 1

#define DTS_IMU_SYNC_DRV_NODE_NAME_NODE  "/imu_sync/imu_sync_node@0"
#define DTS_IMU_SYNC_DRV_NODE_NAME_COMP  "ambarella,imu_sync"

#define IMU_SYNC_DEV_NAME "imu_sync"
#define IMU_SYNC_DEV_NUM 0

#define __AMBA_CORTEX_A78_DEBUG_PORT_BASE_ADDR                (0xffed000000UL)
#define __AMBA_DBG_PORT_CORC0_BASE_ADDR                       (__AMBA_CORTEX_A78_DEBUG_PORT_BASE_ADDR + 0x140000UL)

#define amba_orc_reg_addr __AMBA_DBG_PORT_CORC0_BASE_ADDR
#define amba_orc_reg_addr_ofs_curtime 0x44UL

#define imu_sync_gpio 32


// 1536/125 = 12.288(AmbaRTSL_PllGetAudioClk()?)
// 2^10 = 1024, 1024/12.288 = 8.3
#define temp_clk_1000 12288
#define temp_clk_base 10
#define temp_clk_src  ((1 << temp_clk_base) * 1000) / temp_clk_1000 // 2^10 / 12.288		
#define imu_sync_to_tickus(tick) (tick >> temp_clk_base) * temp_clk_src

#define print_info 0
#define sim_tick 0

static struct imu_sync_st {
	int major;
	int minor;
	dev_t dev_num;
	struct cdev *cdev;
	struct class *class;
	struct device *device;
}*my_imu_sync_st;

typedef struct {
/* ctrl */
	int query_rdy;
	int user_start;
	spinlock_t spin_lock_latest; 	// for build code warning
	spinlock_t spin_lock_oldest; 	// for build code warning
	spinlock_t spin_lock_info; 		// for build code warning & racing issue
	struct mutex mutex;
	struct tasklet_struct tasklet;
	wait_queue_head_t wait;	
	int irq_toggle;

/* param */
	volatile void __iomem* io_addr_orc_curtime;
	int irqno;
	uint32_t  len;		 // number of cap_seq_no & tick
	uint32_t  idx;	     // idx of cap_seq_no & tick
	uint32_t cap_seq_no; // valid count : len
	uint64_t tick; 		 // valid count : len
	uint32_t cap_seq_no_cur;
	uint32_t irq_cnt; 
} eva_imu_sync_info_int_s;

static eva_imu_sync_info_int_s imu_sync_info = {0};
static eva_audio_tick_query_lastest_s audio_tick_latest; //to query from audio tick kernel driver.
static eva_imu_sync_info_int_s info = {0}; // for build code warning & racing issue
static eva_imu_sync_cur_s in_current = {0}; //from userspace
static eva_imu_sync_cur_s imu_sync_current = {0};
static uint32_t imu_sync_irq = 0;

static uint64_t get_imu_sync(void)
{
	uint32_t tick_u32_curr;
	uint64_t tick_u64_curr;
	static uint32_t tick_u32_prev_static = 0;
	static uint64_t tick_msb_cnt_static = 0;

	tick_u32_curr = readl(imu_sync_info.io_addr_orc_curtime);
	if (tick_u32_prev_static > tick_u32_curr) {
		tick_msb_cnt_static++;
	} 
	tick_u64_curr = (tick_msb_cnt_static << 32) | tick_u32_curr;
	
	tick_u32_prev_static = tick_u32_curr;

	return tick_u64_curr;
}

static int imu_sync_upd_info(void)
{
    int Rval;

	imu_sync_current.imu_sync_tick = get_imu_sync();
    imu_sync_current.imu_sync_tick_us = imu_sync_to_tickus(imu_sync_current.imu_sync_tick);
    audio_tick_latest.len = 1; //set len to 1
    Rval = audio_tick_query_lastest(&audio_tick_latest);
    if (Rval != 0) {
        return Rval;
    } else {
        imu_sync_current.audio_tick = audio_tick_latest.tick[0];
        imu_sync_current.audio_tick_us = audio_tick_latest.tick_us[0];
        imu_sync_current.audio_tick_CapSeqNo = audio_tick_latest.cap_seq_no[0];
    }

	return Rval;
}

int imu_sync_cat_info(void)
{
#if print_info
	printk(KERN_ERR "Curren CapSeqNo %d audio tick 0x%llx imu sync tick 0x%llx \n"
		, imu_sync_current.audio_tick_CapSeqNo
		, imu_sync_current.audio_tick
		, imu_sync_current.imu_sync_tick);
#endif	
	return 0;
}

static void imu_sync_tasklet_handler(unsigned long data)
{
	static int print_cnt = 0;

#if print_info
    printk(KERN_ERR "IMU Sync Tasklet Handler here");
	imu_sync_cat_info();
#endif	
	print_cnt++;
}

static int imu_sync_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int imu_sync_release(struct inode *inode, struct file *filp) 
{
	return 0;
}

int imu_sync_get_current_ioctl(eva_imu_sync_info_int_s* info, unsigned long arg)
{
    int i, start, idx, len;
    const eva_imu_sync_cur_s __user *from = (eva_imu_sync_cur_s*)arg;

    spin_lock(&imu_sync_info.spin_lock_latest);
    /* copy from user*/
    if (copy_from_user(&in_current, from, sizeof(eva_imu_sync_cur_s)) != 0) {
        printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_from_user");
        spin_unlock(&imu_sync_info.spin_lock_latest);
        return -1;
    }

    in_current.audio_tick_CapSeqNo =  imu_sync_current.audio_tick_CapSeqNo;
    in_current.audio_tick          =  imu_sync_current.audio_tick;
    in_current.audio_tick_us       =  imu_sync_current.audio_tick_us;
    in_current.imu_sync_tick       =  imu_sync_current.imu_sync_tick;
    in_current.imu_sync_tick_us    =  imu_sync_current.imu_sync_tick_us;

    /* copy to user*/
    if (copy_to_user((void __user *)from, &in_current, sizeof(eva_imu_sync_cur_s)) != 0) {
        printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
        spin_unlock(&imu_sync_info.spin_lock_latest);
        return -1;
    }
    spin_unlock(&imu_sync_info.spin_lock_latest);
    return 0;
}

static long imu_sync_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int rt = 0;

	spin_lock(&imu_sync_info.spin_lock_info);
	memcpy(&info, &imu_sync_info, sizeof(eva_imu_sync_info_int_s));
	spin_unlock(&imu_sync_info.spin_lock_info);

	switch(cmd) {
    case EVA_IMUSYNC_IOCTL_GET_CURRENT:   {
            rt = imu_sync_get_current_ioctl(&info, arg);
        }
        break;
	default:
		printk(KERN_ERR "[%s] not support cmd 0x%x\r\n", __func__, cmd);
	break;
	}
	
//end:
	return rt;
}

static __poll_t imu_sync_poll(struct file *file, struct poll_table_struct *pollt)
{
	__poll_t events = 0;

	poll_wait(file, &imu_sync_info.wait, pollt);

	if (imu_sync_info.irq_toggle == 1) {
		imu_sync_info.irq_toggle = 0;
		events |= POLLIN;
	}

	return events;
}

static irqreturn_t imu_sync_irq_handler(int irqno, void *dev)
{
    int Rval;
	Rval = imu_sync_upd_info();
    if (Rval == 0) {
        wake_up(&imu_sync_info.wait);
        tasklet_schedule(&imu_sync_info.tasklet);
    }
    //error handle
    return IRQ_HANDLED;
}


struct file_operations fops = {
	.open			= imu_sync_open,
	.unlocked_ioctl	= imu_sync_ioctl,
	.release 		= imu_sync_release,
	.poll			= imu_sync_poll,
};
	
int imu_sync_init_ioctl(void) 
{
	int ret;

	my_imu_sync_st = kzalloc(sizeof(struct imu_sync_st), GFP_KERNEL);
//	my_imu_sync_st->privte = kzalloc(sizeof(struct imu_sync_info_st), GFP_KERNEL);
 	my_imu_sync_st->dev_num = IMU_SYNC_DEV_NUM;
	my_imu_sync_st->cdev = cdev_alloc();
	printk(KERN_INFO "[%s] Inside init module\n", __func__);
	ret = alloc_chrdev_region(&my_imu_sync_st->dev_num , my_imu_sync_st->dev_num, 1, IMU_SYNC_DEV_NAME);
	if (ret < 0) {
		printk(KERN_ERR "Major number allocation is failed\n");
		return ret;
	}
    my_imu_sync_st->major = MAJOR(my_imu_sync_st->dev_num);
	my_imu_sync_st->minor = MINOR(my_imu_sync_st->dev_num);
	cdev_init(my_imu_sync_st->cdev, &fops);
	my_imu_sync_st->cdev->owner = THIS_MODULE;

	printk(KERN_INFO "[%s] The major number for device is %d\n", __func__, my_imu_sync_st->major);
	ret = cdev_add(my_imu_sync_st->cdev, my_imu_sync_st->dev_num, 1);
	if(ret < 0)	{
		printk(KERN_ERR KERN_INFO "Unable to allocate cdev");
		return ret;
	}

	my_imu_sync_st->class = class_create(THIS_MODULE, IMU_SYNC_DEV_NAME);
	if(IS_ERR(my_imu_sync_st->class))	{
		printk(KERN_ERR KERN_INFO "class_create fail");
		return ret;
	}

	my_imu_sync_st->device = device_create(my_imu_sync_st->class, NULL, my_imu_sync_st->dev_num, NULL, IMU_SYNC_DEV_NAME);
	if(IS_ERR(my_imu_sync_st->device))	{
		printk(KERN_ERR KERN_INFO "device_create fail");
		return ret;
	}

	return 0;
}

int imu_sync_uninit_ioctl(void) 
{
	if (my_imu_sync_st) {
		kfree(my_imu_sync_st);	
	}
	return 0;
}

static int imu_sync_init_info(void)
{
	imu_sync_info.query_rdy = 0;
	imu_sync_info.user_start = 0;
	spin_lock_init(&imu_sync_info.spin_lock_latest);
	spin_lock_init(&imu_sync_info.spin_lock_oldest);
	spin_lock_init(&imu_sync_info.spin_lock_info);
	mutex_init(&imu_sync_info.mutex);
	tasklet_init(&imu_sync_info.tasklet, imu_sync_tasklet_handler, 0);
	imu_sync_info.irq_toggle = 0;	
	imu_sync_info.io_addr_orc_curtime = (void __iomem *)ioremap(amba_orc_reg_addr + amba_orc_reg_addr_ofs_curtime, 4);
	imu_sync_info.idx = 0;
	imu_sync_info.len = imu_sync_len;
    imu_sync_info.cap_seq_no = 0;
    imu_sync_info.tick = 0;
	imu_sync_info.cap_seq_no_cur = 0;
	imu_sync_info.irq_cnt = 0;
	
	init_waitqueue_head(&imu_sync_info.wait);

#if sim_tick
	long i;

	for (i = 0; i < imu_sync_info.len; i++) {
		imu_sync_info.cap_seq_no[i] 	= i;
		imu_sync_info.tick[i] 		= (i * 33333 * temp_clk_1000) / 1000;
	}
#endif

	return 0;
}

static int imu_sync_uninit_info(void)
{	
	iounmap(imu_sync_info.io_addr_orc_curtime);	
	return 0;
}

#define USE_OF_TABLE 0
#if USE_OF_TABLE
static void imu_sync_of_init(struct device_node *np)
{
    (void)np;
    // DO NOTHING
}

static const struct of_device_id __imu_sync_of_table = {
    .compatible = DTS_IMU_SYNC_DRV_NODE_NAME_COMP,
    .data = imu_sync_of_init,
};
#endif


static int __init eva_imu_sync_init(void)
{
	uint32_t irqno, rt = 0;
	struct device_node *dev_node = NULL;

	imu_sync_init_info();  	

#if USE_OF_TABLE
    struct device_node *np = NULL;
	of_init_fn_1 init_func;
	const struct of_device_id *match;

	for_each_matching_node_and_match(np, &__imu_sync_of_table, &match) {
		if (!of_device_is_available(np)) {
			continue;
		}

		init_func = match->data;
		init_func(np);
		dev_node = np;		
	}
#else	

	if ((dev_node = of_find_compatible_node(NULL, NULL, DTS_IMU_SYNC_DRV_NODE_NAME_COMP)) == 0) {
		printk(KERN_ERR "[%s] of_find_compatible_node dev_node\r\n", __func__);
	}
#endif

	imu_sync_init_ioctl();

	//imu_sync_info.irqno = irqno = irq_of_parse_and_map(dev_node, 0);
    imu_sync_irq = imu_sync_info.irqno = irqno = gpio_to_irq(imu_sync_gpio);
    printk(KERN_ERR KERN_INFO "imu_sync_irq %d gpio %d", irqno, imu_sync_gpio);
	rt = request_irq(irqno, imu_sync_irq_handler, IRQF_TRIGGER_RISING, IMU_SYNC_DEV_NAME, NULL);	

    if (rt) {
        printk(KERN_ERR KERN_INFO "request_irq fail, rt %d irq %d", (int)rt, (int)irqno);
		return -1;
    }

	printk(KERN_ERR KERN_INFO "imu_sync_len %d", imu_sync_len);

    return 0;
}


static void __exit eva_imu_sync_exit(void)
{	
	free_irq(imu_sync_irq, NULL);
    tasklet_kill(&imu_sync_info.tasklet);

	imu_sync_uninit_info();
	imu_sync_uninit_ioctl();
}


module_init(eva_imu_sync_init);
module_exit(eva_imu_sync_exit);

MODULE_AUTHOR("Jason Chang <chchang@ambarella.com>");
MODULE_DESCRIPTION("eva imu sync driver");
MODULE_LICENSE("GPL v2");



