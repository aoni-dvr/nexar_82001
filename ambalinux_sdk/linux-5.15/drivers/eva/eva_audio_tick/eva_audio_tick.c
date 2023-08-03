// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Pinctrl driver for Ambarella SoCs
 *
 * History:
 *	2013/12/18 - [Cao Rongrong] created file
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


static int audio_tick_len = eva_audio_tick_recoder_len_dft;
module_param(audio_tick_len, int, 0);


#define audio_tick_todo 1

#define DTS_AUDIO_TICK_DRV_NODE_NAME_NODE  "/audio_tick/audio_tick_node@0"
#define DTS_AUDIO_TICK_DRV_NODE_NAME_COMP  "ambarella,audio_tick"

#define AUDIO_TICK_DEV_NAME "audio_tick"
#define AUDIO_TICK_DEV_NUM 0

#define __AMBA_CORTEX_A78_DEBUG_PORT_BASE_ADDR                (0xffed000000UL)
#define __AMBA_DBG_PORT_CORC0_BASE_ADDR                       (__AMBA_CORTEX_A78_DEBUG_PORT_BASE_ADDR + 0x140000UL)

#define amba_orc_reg_addr __AMBA_DBG_PORT_CORC0_BASE_ADDR
#define amba_orc_reg_addr_ofs_curtime 0x44UL

#define audio_tick_irq 281


// 1536/125 = 12.288(AmbaRTSL_PllGetAudioClk()?)
// 2^10 = 1024, 1024/12.288 = 8.3
#define audio_clk_1000 12288
#define audio_clk_base 10
#define audio_clk_src  ((1 << audio_clk_base) * 1000) / audio_clk_1000 // 2^10 / 12.288		
#define audio_tick_to_tickus(tick) (tick >> audio_clk_base) * audio_clk_src

#define print_info 0
#define sim_tick 0

static struct audio_tick_st {
	int major;
	int minor;
	dev_t dev_num;
	struct cdev *cdev;
	struct class *class;
	struct device *device;
}*my_audio_tick_st;

typedef struct {
/* ctrl */
	int query_rdy;
	int user_start;
	spinlock_t spin_lock_lastest; 	// for build code warning
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
	uint32_t cap_seq_no[eva_audio_tick_recoder_len_max]; // valid count : len
	uint64_t tick[eva_audio_tick_recoder_len_max]; 		 // valid count : len
	uint32_t cap_seq_no_cur;
	uint32_t irq_cnt; 
} eva_audio_tick_info_int_s;

static eva_audio_tick_info_int_s audio_tick_info = {0};
static eva_audio_tick_query_lastest_s in_lastest; // for build code warning
static eva_audio_tick_query_oldest_s in_oldest; // for build code warning
static eva_audio_tick_info_int_s info = {0}; // for build code warning & racing issue
static eva_audio_tick_info_int_s info_lastest = {0}; // for build code warning & racing issue
static eva_audio_tick_info_int_s info_oldest = {0}; // for build code warning & racing issue


static uint64_t get_audio_tick(void)
{
	uint32_t tick_u32_curr;
	uint64_t tick_u64_curr;
	static uint32_t tick_u32_prev_static = 0;
	static uint64_t tick_msb_cnt_static = 0;

	tick_u32_curr = readl(audio_tick_info.io_addr_orc_curtime);
	if (tick_u32_prev_static > tick_u32_curr) {
		tick_msb_cnt_static++;
	} 
	tick_u64_curr = (tick_msb_cnt_static << 32) | tick_u32_curr;
	
	tick_u32_prev_static = tick_u32_curr;

	return tick_u64_curr;
}

static int audio_tick_upd_info(void)
{
//	spin_lock(&audio_tick_info.spin_lock_info);

	//if (audio_tick_info.user_start) {
		audio_tick_info.cap_seq_no[audio_tick_info.idx] = audio_tick_info.cap_seq_no_cur;
		audio_tick_info.tick[audio_tick_info.idx] = get_audio_tick();
		audio_tick_info.idx = (audio_tick_info.idx + 1) % audio_tick_info.len;	
		audio_tick_info.cap_seq_no_cur++;
	//}

//	spin_unlock(&audio_tick_info.spin_lock_info);

	return 0;
}

int audio_tick_cat_info(void)
{
#if print_info
	int i;

	for (i = 0; i < audio_tick_info.len; i++) {
		printk(KERN_ERR "idx %-3d cap_seq_no %d  tick 0x%llx diff %ld \n"
			, i
			, audio_tick_info.cap_seq_no[i]
			, audio_tick_info.tick[i]
			, (i == 0) ? audio_tick_info.tick[i] - audio_tick_info.tick[audio_tick_info.len - 1] : audio_tick_info.tick[i] - audio_tick_info.tick[i - 1]);
	}
#endif	
	return 0;
}




static void audio_tick_tasklet_handler(unsigned long data)
{
	static int print_cnt = 0;

//	audio_tick_upd_info();
	
#if print_info
	if ((print_cnt % (30*60)) == 0) {
		audio_tick_cat_info();
	}
#endif	
	print_cnt++;
	

}

static int audio_tick_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int audio_tick_release(struct inode *inode, struct file *filp) 
{
	return 0;
}

int audio_tick_query_with_capseqno_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	int i, start, idx, len;
	eva_audio_tick_query_with_capseqno_s in;
	const eva_audio_tick_query_with_capseqno_s __user *from = (eva_audio_tick_query_with_capseqno_s*)arg;

	/* copy from user*/
	if (copy_from_user(&in, from, sizeof(eva_audio_tick_query_with_capseqno_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_from_user");
		return -1;
	}

	/* set value */
	start = (info->idx == 0) ? info->len : info->idx;
	len = info->len;
	for (i = 0; i < info->len; i++) {
		idx = (start + len + i);
		if (in.cap_seq_no_user >= info->cap_seq_no[idx % len]) {
			in.cap_seq_no_real	= info->cap_seq_no[idx % len];							
			in.tick_real		= info->tick[idx % len];
		}
	}
	in.tick_us_real 	= audio_tick_to_tickus(in.tick_real);					
	
	/* copy to user*/
	if (copy_to_user((void __user *)from, &in, sizeof(eva_audio_tick_query_with_capseqno_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		return -1;
	}	
	return 0;
}

int audio_tick_query_with_tick_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	int i, start, idx, len;
	eva_audio_tick_query_with_tick_s in;
	const eva_audio_tick_query_with_tick_s __user *from = (eva_audio_tick_query_with_tick_s*)arg;

	/* copy from user*/
	if (copy_from_user(&in, from, sizeof(eva_audio_tick_query_with_tick_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_from_user");
		return -1;
	}

	/* set value */
	start = (info->idx == 0) ? info->len : info->idx;
	len = info->len;
	for (i = 0; i < info->len; i++) {
		idx = (start + len + i);
//		if (in.tick_user  >= info->tick[idx % len]) {
        if (in.tick_user  >= (info->tick[idx % len] & 0xffffffffU)) {
			in.cap_seq_no_real	= info->cap_seq_no[idx % len];							
			in.tick_real		= info->tick[idx % len];
		}
	}
	in.tick_us_real 	= audio_tick_to_tickus(in.tick_real);				

	/* copy to user*/
	if (copy_to_user((void __user *)from, &in, sizeof(eva_audio_tick_query_with_tick_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		return -1;
	}	
	return 0;
}

int audio_tick_query_lastest_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	int i, start, idx, len;
	const eva_audio_tick_query_lastest_s __user *from = (eva_audio_tick_query_lastest_s*)arg;

	spin_lock(&audio_tick_info.spin_lock_lastest);
	/* copy from user*/
	if (copy_from_user(&in_lastest, from, sizeof(eva_audio_tick_query_lastest_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_from_user");
		spin_unlock(&audio_tick_info.spin_lock_lastest);
		return -1;
	}

	/* set value */
	if (in_lastest.len > info->len) {
		printk(KERN_ERR "[%s] input len %d > %d ovfl\r\n", __func__, in_lastest.len, info->len);
		spin_unlock(&audio_tick_info.spin_lock_lastest);
		return -1;
	}			
	start = (info->idx == 0) ? info->len : info->idx;
	len = info->len;
	for (i = 0; i < in_lastest.len; i++) {
		idx = (start + len - i - 1);
		in_lastest.cap_seq_no[i] 	= info->cap_seq_no[idx % len];								
		in_lastest.tick[i] 			= info->tick[idx % len];
		in_lastest.tick_us[i] 		= audio_tick_to_tickus(in_lastest.tick[i]);		
	}

	/* copy to user*/
	if (copy_to_user((void __user *)from, &in_lastest, sizeof(eva_audio_tick_query_lastest_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		spin_unlock(&audio_tick_info.spin_lock_lastest);
		return -1;
	}	
	spin_unlock(&audio_tick_info.spin_lock_lastest);
	return 0;
}


int audio_tick_query_lastest(eva_audio_tick_query_lastest_s *in)
{
	int i, start, idx, len;

    memcpy(&info_lastest, &audio_tick_info, sizeof(eva_audio_tick_info_int_s));

	/* set value */
	if (in->len > info_lastest.len) {
		printk(KERN_ERR "[%s] input len %d > %d ovfl\r\n", __func__, in->len, info_lastest.len);
		return -1;
	}			
	start = (info_lastest.idx == 0) ? info_lastest.len : info_lastest.idx;
	len = info_lastest.len;
	for (i = 0; i < in->len; i++) {
		idx = (start + len - i - 1);
		in->cap_seq_no[i]   = info_lastest.cap_seq_no[idx % len];								
		in->tick[i]         = info_lastest.tick[idx % len];
		in->tick_us[i]      = audio_tick_to_tickus(in->tick[i]);		
	}
    
	return 0;
}

int audio_tick_query_oldest_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	int i, start, idx, len;
	const eva_audio_tick_query_oldest_s __user *from = (eva_audio_tick_query_oldest_s*)arg;

	spin_lock(&audio_tick_info.spin_lock_oldest);
	/* copy from user*/
	if (copy_from_user(&in_oldest, from, sizeof(eva_audio_tick_query_oldest_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_from_user");
		spin_unlock(&audio_tick_info.spin_lock_oldest);
		return -1;
	}

	/* set value */
	if (in_oldest.len > info->len) {
		printk(KERN_ERR "[%s] input len %d > %d ovfl\r\n", __func__, in_oldest.len, info->len);
		spin_unlock(&audio_tick_info.spin_lock_oldest);
		return -1;
	}			
	start = (info->idx == 0) ? info->len : info->idx;
	len = info->len;
	for (i = 0; i < in_oldest.len; i++) {
		idx = (start + len + i);
		in_oldest.cap_seq_no[i] 	= info->cap_seq_no[idx % len];								
		in_oldest.tick[i] 			= info->tick[idx % len];
		in_oldest.tick_us[i] 		= audio_tick_to_tickus(in_oldest.tick[i]);		
	}			

	/* copy to user*/
	if (copy_to_user((void __user *)from, &in_oldest, sizeof(eva_audio_tick_query_lastest_s)) != 0) {
		printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		spin_unlock(&audio_tick_info.spin_lock_oldest);
		return -1;
	}
	spin_unlock(&audio_tick_info.spin_lock_oldest);
	return 0;
}

int audio_tick_query_oldest(eva_audio_tick_query_oldest_s *in)
{
	int i, start, idx, len;

    memcpy(&info_oldest, &audio_tick_info, sizeof(eva_audio_tick_info_int_s));

	/* set value */
	if (in->len > info_oldest.len) {
		printk(KERN_ERR "[%s] input len %d > %d ovfl\r\n", __func__, in->len, info_oldest.len);
		return -1;
	}			
	start = (info_oldest.idx == 0) ? info_oldest.len : info_oldest.idx;
	len = info_oldest.len;
	for (i = 0; i < in->len; i++) {
		idx = (start + len + i);
		in->cap_seq_no[i] 	= info_oldest.cap_seq_no[idx % len];								
		in->tick[i] 		= info_oldest.tick[idx % len];
		in->tick_us[i] 		= audio_tick_to_tickus(in->tick[i]);		
	}			
	return 0;
}

int audio_tick_get_cur_tick_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	eva_audio_tick_cur_s out;
	const eva_audio_tick_cur_s __user *from = (eva_audio_tick_cur_s*)arg;

	/* get value */
	out.tick = get_audio_tick();
	out.tick_us = out.tick >> audio_clk_base;
	out.tick_us = out.tick_us * audio_clk_src;			
	
	/* copy to user*/			
	if (copy_to_user((void __user *)from, &out, sizeof(eva_audio_tick_cur_s)) != 0) {
        printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		return -1;
	}
    return 0;
}

int audio_tick_get_cur_tick(eva_audio_tick_cur_s *out)
{
	/* get value */
	out->tick = get_audio_tick();
	out->tick_us = out->tick >> audio_clk_base;
	out->tick_us = out->tick_us * audio_clk_src;			

    return 0;
}

int audio_tick_get_cur_capseqno_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	uint32_t out;
	const uint32_t __user *from = (uint32_t*)arg;

	/* get value */
	spin_lock(&audio_tick_info.spin_lock_info);
	out = audio_tick_info.irq_cnt;
	spin_unlock(&audio_tick_info.spin_lock_info);
	
	/* copy to user*/			
	if (copy_to_user((void __user *)from, &out, sizeof(uint32_t)) != 0) {
        printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		return -1;
	}
    return 0;
}


int audio_tick_get_cur_capseqno(uint32_t *out)
{
	/* get value */
	spin_lock(&audio_tick_info.spin_lock_info);
	*out = audio_tick_info.irq_cnt;
	spin_unlock(&audio_tick_info.spin_lock_info);

    return 0;
}


int audio_tick_get_query_ready_ioctl(eva_audio_tick_info_int_s* info, unsigned long arg)
{
	int out;
	const int __user *from = (int*)arg;

	/* get value */
#if audio_tick_todo
	audio_tick_info.query_rdy = 1;
#else		
	// todo
#endif
	out = audio_tick_info.query_rdy;		
	
	/* copy to user*/			
	if (copy_to_user((void __user *)from, &out, sizeof(int)) != 0) {
        printk(KERN_ERR "[%s] %s error\r\n", __func__, "copy_to_user");
		return -1;
	}
    return 0;
}

static long audio_tick_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int rt = 0;

	spin_lock(&audio_tick_info.spin_lock_info);
	memcpy(&info, &audio_tick_info, sizeof(eva_audio_tick_info_int_s));
	spin_unlock(&audio_tick_info.spin_lock_info);

	switch(cmd) {
	case EVA_AUDIOTICK_IOCTL_QUERY_WITH_CAPSEQNO: 	{
		rt = audio_tick_query_with_capseqno_ioctl(&info, arg);		
	}	
	break;
    
	case EVA_AUDIOTICK_IOCTL_QUERY_WITH_TICK: {
		rt = audio_tick_query_with_tick_ioctl(&info, arg);		
	}		
	break;

	case EVA_AUDIOTICK_IOCTL_QUERY_LASTEST: {
		rt = audio_tick_query_lastest_ioctl(&info, arg);				
	}		
	break;

	case EVA_AUDIOTICK_IOCTL_QUERY_OLDEST: {
		rt = audio_tick_query_oldest_ioctl(&info, arg);				
	}		
	break;
	
	case EVA_AUDIOTICK_IOCTL_SET_TRIG: {
		eva_audio_tick_trig_type_e in;
		const eva_audio_tick_trig_type_e __user *from = (eva_audio_tick_trig_type_e*)arg;

		/* copy from user*/
		if (copy_from_user(&in, from, sizeof(eva_audio_tick_trig_type_e)) != 0) {
			printk(KERN_ERR "[%s] %s %s error\r\n", __func__, "EVA_AUDIOTICK_IOCTL_SET_TRIG", "copy_from_user");
			rt = -1;
		}
		
		if (in == EVA_AUDIOTICK_TRIG_START) {
//			audio_tick_info.idx = 0;
//			memset(&audio_tick_info.cap_seq_no[0], 0, sizeof(audio_tick_info.cap_seq_no));
//			memset(&audio_tick_info.tick[0], 0, sizeof(audio_tick_info.tick));
			audio_tick_info.user_start = 1;			
		} else if (in == EVA_AUDIOTICK_TRIG_STOP) {
			audio_tick_info.user_start = 0;
		} else {
			printk(KERN_ERR "[%s] %s not support %d\r\n", __func__, "EVA_AUDIOTICK_IOCTL_SET_TRIG", in);
			rt = -1;
		}
	}
	break;
	
	case EVA_AUDIOTICK_IOCTL_GET_TRIG: {
		int out;
		const int __user *from = (int*)arg;

		/* get value */
		out = audio_tick_info.user_start;		
		
		/* copy to user*/			
		if (copy_to_user((void __user *)from, &out, sizeof(int)) != 0) {
			printk(KERN_ERR "[%s] %s %s error\r\n", __func__, "EVA_AUDIOTICK_IOCTL_GET_TRIG", "copy_to_user");
			rt = -1;
		}
	}
	break;

	case EVA_AUDIOTICK_IOCTL_SET_REC_LEN: {
#if 1
        printk(KERN_ERR "[%s] %s pls set in module param. (current : %d)\r\n", __func__, "EVA_AUDIOTICK_IOCTL_SET_REC_LEN", audio_tick_info.len);
#else
		uint32_t in;
		const uint32_t __user *from = (uint32_t*)arg;
		if (audio_tick_info.user_start) {
			printk(KERN_ERR "[%s] %s not support when %s set to %d\r\n", __func__, "EVA_AUDIOTICK_IOCTL_SET_REC_LEN", "GET_TRIG", audio_tick_info.user_start);
			rt = -1;
			goto end;
		}

		/* copy from user*/
		if (copy_from_user(&in, from, sizeof(uint32_t)) != 0) {
			printk(KERN_ERR "[%s] %s %s error\r\n", __func__, "EVA_AUDIOTICK_IOCTL_SET_REC_LEN", "copy_from_user");
			rt = -1;
		}
		if ((in > eva_audio_tick_recoder_len_max) || (in < eva_audio_tick_recoder_len_min)) {
			printk(KERN_ERR "[%s] %s in %d ovfl (max %d, min %d)\r\n", __func__, "EVA_AUDIOTICK_IOCTL_SET_REC_LEN", in, eva_audio_tick_recoder_len_max, 1);
			rt = -1;
		} else {
			audio_tick_info.len = in;
		}
#endif        
	}
	break;
	
	case EVA_AUDIOTICK_IOCTL_GET_REC_LEN: {
		int out;
		const int __user *from = (int*)arg;

		/* get value */
		out = audio_tick_info.len;		
		
		/* copy to user*/			
		if (copy_to_user((void __user *)from, &out, sizeof(int)) != 0) {
			printk(KERN_ERR "[%s] %s %s error\r\n", __func__, "EVA_AUDIOTICK_IOCTL_GET_REC_LEN", "copy_to_user");
			rt = -1;
		}
	}
	break;
	
	case EVA_AUDIOTICK_IOCTL_GET_CUR_TICK: {
        rt = audio_tick_get_cur_tick_ioctl(&info, arg);
	}
	break;

	case EVA_AUDIOTICK_IOCTL_GET_CUR_CAPSEQNO: {
		rt = audio_tick_get_cur_capseqno_ioctl(&info, arg);
	}
	break;
	
	case EVA_AUDIOTICK_IOCTL_GET_QUERY_RDY: {
		rt = audio_tick_get_query_ready_ioctl(&info, arg);
	}
	break;
	
	default:
		printk(KERN_ERR "[%s] not support cmd 0x%x\r\n", __func__, cmd);
	break;
	}
	
	return rt;
}

static __poll_t audio_tick_poll(struct file *file, struct poll_table_struct *pollt)
{
	__poll_t events = 0;

	poll_wait(file, &audio_tick_info.wait, pollt);

	if (audio_tick_info.irq_toggle == 1) {
		audio_tick_info.irq_toggle = 0;
		events |= POLLIN;
	}

	return events;
}

static irqreturn_t audio_tick_irq_handler(int irqno, void *dev)
{
	audio_tick_upd_info();
	audio_tick_info.irq_cnt++;
	if (irqno == audio_tick_info.irqno) {
		audio_tick_info.irq_toggle = 1;
		wake_up(&audio_tick_info.wait);
	}
	tasklet_schedule(&audio_tick_info.tasklet);

    return IRQ_HANDLED;
}


struct file_operations fops = {
	.open			= audio_tick_open,
	.unlocked_ioctl	= audio_tick_ioctl,
	.release 		= audio_tick_release,
	.poll			= audio_tick_poll,
};
	
int audio_tick_init_ioctl(void) 
{
	int ret;

	my_audio_tick_st = kzalloc(sizeof(struct audio_tick_st), GFP_KERNEL);
//	my_audio_tick_st->privte = kzalloc(sizeof(struct audio_tick_info_st), GFP_KERNEL);
 	my_audio_tick_st->dev_num = AUDIO_TICK_DEV_NUM;
	my_audio_tick_st->cdev = cdev_alloc();
	printk(KERN_INFO "[%s] Inside init module\n", __func__);
	ret = alloc_chrdev_region(&my_audio_tick_st->dev_num , my_audio_tick_st->dev_num, 1, AUDIO_TICK_DEV_NAME);
	if (ret < 0) {
		printk(KERN_ERR "Major number allocation is failed\n");
		return ret;
	}
    my_audio_tick_st->major = MAJOR(my_audio_tick_st->dev_num);
	my_audio_tick_st->minor = MINOR(my_audio_tick_st->dev_num);
	cdev_init(my_audio_tick_st->cdev, &fops);
	my_audio_tick_st->cdev->owner = THIS_MODULE;

	printk(KERN_INFO "[%s] The major number for device is %d\n", __func__, my_audio_tick_st->major);
	ret = cdev_add(my_audio_tick_st->cdev, my_audio_tick_st->dev_num, 1);
	if(ret < 0)	{
		printk(KERN_ERR KERN_INFO "Unable to allocate cdev");
		return ret;
	}

	my_audio_tick_st->class = class_create(THIS_MODULE, AUDIO_TICK_DEV_NAME);
	if(IS_ERR(my_audio_tick_st->class))	{
		printk(KERN_ERR KERN_INFO "class_create fail");
		return ret;
	}

	my_audio_tick_st->device = device_create(my_audio_tick_st->class, NULL, my_audio_tick_st->dev_num, NULL, AUDIO_TICK_DEV_NAME);
	if(IS_ERR(my_audio_tick_st->device))	{
		printk(KERN_ERR KERN_INFO "device_create fail");
		return ret;
	}

	return 0;
}

int audio_tick_uninit_ioctl(void) 
{
	if (my_audio_tick_st) {
		kfree(my_audio_tick_st);	
	}
	return 0;
}

static int audio_tick_init_info(void)
{
	audio_tick_info.query_rdy = 0;
	audio_tick_info.user_start = 0;
	spin_lock_init(&audio_tick_info.spin_lock_lastest);
	spin_lock_init(&audio_tick_info.spin_lock_oldest);
	spin_lock_init(&audio_tick_info.spin_lock_info);
	mutex_init(&audio_tick_info.mutex);
	tasklet_init(&audio_tick_info.tasklet, audio_tick_tasklet_handler, 0);
	audio_tick_info.irq_toggle = 0;	
	audio_tick_info.io_addr_orc_curtime = (void __iomem *)ioremap(amba_orc_reg_addr + amba_orc_reg_addr_ofs_curtime, 4);
	audio_tick_info.idx = 0;
	audio_tick_info.len = audio_tick_len;
	memset(&audio_tick_info.cap_seq_no[0], 0, sizeof(audio_tick_info.cap_seq_no));
	memset(&audio_tick_info.tick[0], 0, sizeof(audio_tick_info.tick));
	audio_tick_info.cap_seq_no_cur = 0;
	audio_tick_info.irq_cnt = 0;
	
	init_waitqueue_head(&audio_tick_info.wait);

#if sim_tick
	long i;

	for (i = 0; i < audio_tick_info.len; i++) {
		audio_tick_info.cap_seq_no[i] 	= i;
		audio_tick_info.tick[i] 		= (i * 33333 * audio_clk_1000) / 1000;
	}
#endif

	return 0;
}

static int audio_tick_uninit_info(void)
{	
	iounmap(audio_tick_info.io_addr_orc_curtime);	
	return 0;
}

#define USE_OF_TABLE 0
#if USE_OF_TABLE
static void audio_tick_of_init(struct device_node *np)
{
    (void)np;
    // DO NOTHING
}

static const struct of_device_id __audio_tick_of_table = {
    .compatible = DTS_AUDIO_TICK_DRV_NODE_NAME_COMP,
    .data = audio_tick_of_init,
};
#endif


static int __init eva_audio_tick_init(void)
{
	uint32_t irqno, rt = 0;
	struct device_node *dev_node = NULL;

	audio_tick_init_info();  	

#if USE_OF_TABLE
    struct device_node *np = NULL;
	of_init_fn_1 init_func;
	const struct of_device_id *match;

	for_each_matching_node_and_match(np, &__audio_tick_of_table, &match) {
		if (!of_device_is_available(np)) {
			continue;
		}

		init_func = match->data;
		init_func(np);
		dev_node = np;		
	}
#else	

	if ((dev_node = of_find_compatible_node(NULL, NULL, DTS_AUDIO_TICK_DRV_NODE_NAME_COMP)) == 0) {
		printk(KERN_ERR "[%s] of_find_compatible_node dev_node\r\n", __func__);
	}
#endif

	audio_tick_init_ioctl();

	audio_tick_info.irqno = irqno = irq_of_parse_and_map(dev_node, 0);
	rt = request_irq(irqno, audio_tick_irq_handler, IRQF_TRIGGER_RISING, AUDIO_TICK_DEV_NAME, NULL);	

    if (rt) {
        printk(KERN_ERR KERN_INFO "request_irq fail, rt %d irq %d", (int)rt, (int)irqno);
		return -1;
    }

	printk(KERN_ERR KERN_INFO "audio_tick_len %d", audio_tick_len);

    return 0;
}


static void __exit eva_audio_tick_exit(void)
{	
	free_irq(audio_tick_irq, NULL);
    tasklet_kill(&audio_tick_info.tasklet);

	audio_tick_uninit_info();
	audio_tick_uninit_ioctl();
}

module_init(eva_audio_tick_init);
module_exit(eva_audio_tick_exit);


//EXPORT_SYMBOL(audio_tick_query_with_capseqno);
//EXPORT_SYMBOL(audio_tick_query_with_tick);
EXPORT_SYMBOL(audio_tick_query_lastest);
EXPORT_SYMBOL(audio_tick_query_oldest);
EXPORT_SYMBOL(audio_tick_get_cur_tick);
EXPORT_SYMBOL(audio_tick_get_cur_capseqno);
//EXPORT_SYMBOL(audio_tick_get_query_ready);


MODULE_AUTHOR("Silvia Wu <hjwu@ambarella.com>");
MODULE_DESCRIPTION("eva audio tick driver");
MODULE_LICENSE("GPL v2");



