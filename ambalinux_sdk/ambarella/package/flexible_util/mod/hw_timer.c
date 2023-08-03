/*
 * hw_timer.c
 *
 * History:
 *	2016/12/07 - [Jingyang Qiu] created file
 *
 * Copyright (C) 2016  Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

// #include <config.h>

#include <linux/version.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/vmalloc.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/seq_file.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/i2c.h>
#include <linux/firmware.h>
#include <linux/string.h>
#include <linux/fb.h>
#include <linux/spi/spi.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/version.h>
#include <asm/dma.h>
#include <linux/semaphore.h>

#include "inc/iav_utils.h"


#define AMBARELLA_HWTIMER_NAME				"ambarella_hwtimer"
#define AMBARELLA_HWTIMER_OUTFREQ			"ambarella_hwtimer_outfreq"

#define HWTIMER_APB_BASE					(0x20E4000000)
#define HWTIMER_TM_OFFSET					(0x5000)
#define HWTIMER_TM_BASE						(HWTIMER_APB_BASE + HWTIMER_TM_OFFSET)
#define HWTIMER_TM_REG(x)					(HWTIMER_TM_BASE + (x))
#define HWTIMER_TM_CTL_OFFSET				(0x30)
#define HWTIMER_TIMER4_BASE					HWTIMER_TM_REG(HWTIMER_TM_CTL_OFFSET)

#define HWTIMER_TIMER_CTR_REG				(0x00)
#define HWTIMER_TIMER4_STATUS_OFFSET		(0x04)
#define HWTIMER_TIMER4_RELOAD_OFFSET		(0x08)
#define HWTIMER_TIMER4_MATCH1_OFFSET		(0x0C)
#define HWTIMER_TIMER4_MATCH2_OFFSET		(0x10)

#define HWTIMER_TIMER4_REG_MAP_SIZE			0x14

#define AMBARELLA_HWTIMER_CTR_EN			0x00001000
#define AMBARELLA_HWTIMER_CTR_OF			0x00004000
#define AMBARELLA_HWTIMER_CTR_CSL			0x00002000
#define AMBARELLA_HWTIMER_CTR_MASK			0x0000000F


#define AMBARELLA_HWTIMER_IRQ				3//TIMER4_IRQ

// read audio clock every 20 miliseconds
#define HWTIMER_OVERFLOW_MILLI_SECONDS		20
#define HWTIMER_DEFAULT_OUTPUT_FREQ			90000
#define AUDIO_CHECK_MILLI_SECONDS_THRES		25

#define CODE_OFFSET							0x140000
#define CODE_BASE							(DBGBUS_BASE + CODE_OFFSET)
#define GLOBAL_TIMER_OFFSET					0x0044
#define GLOBAL_TIMER_REG					(CODE_BASE + GLOBAL_TIMER_OFFSET)


static void __iomem *hwtimer_reg_base	= NULL;
static void __iomem *hwtimer_audio_base	= NULL;

static u32 hwtimer_enable_flag			= 0;
static u32 hwtimer_init_overflow_value 	= 0;
static u64 hwtimer_init_output_value 	= 0;
static u32 hwtimer_input_freq			= 0;
static u32 hwtimer_curr_outfreq			= 0;
static u32 hwtimer_output_gcd			= 0;

static u64 hwtimer_init_delta_seconds 	= 10;

static u32 audio_isr_tick 				= 0;
static u64 audio_overflow_count 		= 0;
static u64 audio_init_value 			= 0;
static u32 audio_input_freq				= 0;
static u32 audio_input_gcd				= 0;
static u32 audio_check_thres_value		= 0;

static spinlock_t timer_isr_lock;

static int fastboot = 0;
module_param(fastboot, int, 0644);
MODULE_PARM_DESC(fastboot, "Set hw timer init value according to fastboot audio");

static int hwtimer_reset(void);

static inline unsigned long ambarella_hwtimer_input_freq(void)
{
	struct clk *clk = clk_get(NULL, "gclk_apb");
	unsigned long value = clk_get_rate(clk);
	clk_put(clk);

	return value;
}

static inline unsigned long ambarella_audio_input_freq(void)
{
	struct clk *clk = clk_get(NULL, "gclk_audio");
	unsigned long value = clk_get_rate(clk);
	clk_put(clk);

	return value;
}

static inline void ambarella_hwtimer_disable(void)
{
	u32 val = readl_relaxed(hwtimer_reg_base + HWTIMER_TIMER_CTR_REG);
	val &= (~AMBARELLA_HWTIMER_CTR_EN);
	writel_relaxed(val & (~AMBARELLA_HWTIMER_CTR_EN), hwtimer_reg_base + HWTIMER_TIMER_CTR_REG);

	return ;
}

static inline void ambarella_hwtimer_enable(void)
{
	u32 val = readl_relaxed(hwtimer_reg_base + HWTIMER_TIMER_CTR_REG);
	val  |= AMBARELLA_HWTIMER_CTR_EN;
	writel_relaxed(val, hwtimer_reg_base + HWTIMER_TIMER_CTR_REG);

	return ;
}

static inline void ambarella_hwtimer_misc(void)
{
	u32 val = readl_relaxed(hwtimer_reg_base + HWTIMER_TIMER_CTR_REG);
	val |= AMBARELLA_HWTIMER_CTR_OF;
	val &= (~AMBARELLA_HWTIMER_CTR_CSL);
	writel_relaxed(val, hwtimer_reg_base + HWTIMER_TIMER_CTR_REG);

	return ;
}

static inline void ambarella_hwtimer_config(void)
{
	if (!hwtimer_init_overflow_value) {
		hwtimer_init_overflow_value = hwtimer_input_freq;
	}
	writel_relaxed(hwtimer_init_overflow_value,
		hwtimer_reg_base + HWTIMER_TIMER4_STATUS_OFFSET);
	writel_relaxed(hwtimer_init_overflow_value,
		hwtimer_reg_base + HWTIMER_TIMER4_RELOAD_OFFSET);
	writel_relaxed(0x0, hwtimer_reg_base + HWTIMER_TIMER4_MATCH1_OFFSET);
	writel_relaxed(0x0, hwtimer_reg_base + HWTIMER_TIMER4_MATCH2_OFFSET);

	ambarella_hwtimer_misc();

	return ;
}

static inline void ambarella_hwtimer_init(void)
{
	ambarella_hwtimer_disable();
	ambarella_hwtimer_config();
	ambarella_hwtimer_enable();
	hwtimer_enable_flag = 1;

	return ;
}

u32 get_hwtimer_audio_tick(void)
{
	return readl_relaxed(hwtimer_audio_base);
}
EXPORT_SYMBOL(get_hwtimer_audio_tick);

static inline int get_gcd(int a, int b)
{
	int rval = 0;

	if ((a == 0) || (b == 0)) {
		printk("wrong input for gcd \n");
		rval = 1;
	} else {
		while ((a != 0) && (b != 0)) {
			if (a > b) {
				a = a % b;
			} else {
				b = b % a;
			}
		}
		rval = (a == 0) ? b : a;
	}

	return rval;
}

// is_pts: 0 for current tick; 1 for previous tick of pts
static int calc_output_ticks(u64 *out_tick, int is_pts, u32 audio_pts)
{
	u64 total_ticks = 0, overflow_ticks = 0, audio_ticks = 0;
	u64 pts_diff = 0;
	u64 audio_init = 0, hwtimer_init = 0;
	u32 curr_reg_value = 0, isr_tick = 0;
	u32 input_gcd = 0, output_gcd = 0;
	unsigned long flags = 0;
	int negative = 0, rval = 0;

	spin_lock_irqsave(&timer_isr_lock, flags);
	if (!hwtimer_enable_flag) {
		*out_tick = 0;
		spin_unlock_irqrestore(&timer_isr_lock, flags);
		return 0;
	}
	audio_init = audio_init_value;
	hwtimer_init = hwtimer_init_output_value;
	isr_tick = audio_isr_tick;
	curr_reg_value = get_hwtimer_audio_tick();
	overflow_ticks = (u64)audio_overflow_count << 32;
	if (likely(curr_reg_value >= isr_tick)) {
		audio_ticks = curr_reg_value;
	} else {
		audio_ticks = (u64)curr_reg_value + ((u64)1 << 32);
	}
	input_gcd = audio_input_gcd;
	output_gcd = hwtimer_output_gcd;
	spin_unlock_irqrestore(&timer_isr_lock, flags);
	total_ticks = overflow_ticks + audio_ticks;

	// get the ticks since recent hw timer write
	if (likely(total_ticks >= audio_init)) {
		total_ticks -= audio_init;
		negative = 0;
	} else {
		total_ticks = total_ticks + ((u64)1 << 32) - audio_init;
		negative = 0;
	}
	// generate actual pts tick from input audio tick
	if (is_pts) {
		if (audio_pts > curr_reg_value) {
			pts_diff = ((u64)1 << 32) - audio_pts + curr_reg_value;
		} else {
			pts_diff = curr_reg_value - audio_pts;
		}
		if (likely(total_ticks >= pts_diff)){
			total_ticks -= pts_diff;
		} else {
			total_ticks = pts_diff - total_ticks;
			negative = 1;
		}
	}
	// change frequency from audio clock to current output frequency
	total_ticks = total_ticks * output_gcd + (input_gcd >> 1);
	if (likely(input_gcd)) {
		do_div(total_ticks, input_gcd);
	} else {
		printk("HWTimer: calculate output ticks failed! Can not divide zero!\n");
		*out_tick = 0;
		rval = -EINVAL;
		goto CALC_OUTPUT_TICKS_EXIT;
	}
	// generate output ticks
	if (likely(!negative)) {
		*out_tick = hwtimer_init + total_ticks;
	} else {
		*out_tick = hwtimer_init - total_ticks;
	}

CALC_OUTPUT_TICKS_EXIT:
	return rval;
}

static ssize_t hwtimer_write_proc(struct file *file, const char __user *buffer,
		size_t count, loff_t *data)
{
	ssize_t rval = 0;
	u64 value = 0;
	char buf[50] = {0};
	unsigned long flags = 0;

	if (count >= 50 || count <= 1) {
		printk("HWTimer: %s: count %d out of size!\n", __func__, (u32)count);
		rval = -ENOSPC;
		goto HWTIMER_WRITE_PROC_EXIT;
	}

	if (copy_from_user(buf, buffer, count)) {
		printk("HWTimer: %s: copy_from_user fail!\n", __func__);
		rval = -EFAULT;
		goto HWTIMER_WRITE_PROC_EXIT;
	}
	buf[count] = '\0';
	value = simple_strtoull(buf, NULL, 10);

	spin_lock_irqsave(&timer_isr_lock, flags);
	hwtimer_init_output_value = value;
	hwtimer_reset();
	spin_unlock_irqrestore(&timer_isr_lock, flags);
	rval = count;

HWTIMER_WRITE_PROC_EXIT:
	return rval;
}

int get_hwtimer_output_ticks(u64 *out_tick)
{
	return calc_output_ticks(out_tick, 0, 0);
}
EXPORT_SYMBOL(get_hwtimer_output_ticks);

int get_hwtimer_output_pts(u64 *out_tick, u32 audio_tick)
{
	return calc_output_ticks(out_tick, 1, audio_tick);
}
EXPORT_SYMBOL(get_hwtimer_output_pts);

static ssize_t hwtimer_read_proc(struct file *file, char __user *buf,
	size_t size, loff_t *ppos)
{
	u64 final_ticks = 0;
	char str_ticks[32] = {0};
	int rval = 0;

	if (size < 32) {
		printk("HWTimer: hw timer read buf size %zd should be >= 32!\n", size);
		rval = -EINVAL;
		goto HWTIMER_READ_PROC_EXIT;
	}

	rval = calc_output_ticks(&final_ticks, 0, 0);
	if (rval) {
		goto HWTIMER_READ_PROC_EXIT;
	}

	rval = snprintf(str_ticks, sizeof(str_ticks), "%lld\n", final_ticks);
	if(copy_to_user(buf, str_ticks, rval)) {
		rval = -EFAULT;
	}

HWTIMER_READ_PROC_EXIT:
	return rval;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops hwtimer_proc_fops = {
	.proc_read = hwtimer_read_proc,
	.proc_write = hwtimer_write_proc,
};
#else
static const struct file_operations hwtimer_proc_fops = {
	.owner		= THIS_MODULE,
	.read = hwtimer_read_proc,
	.write = hwtimer_write_proc,
};
#endif

static int hwtimer_create_proc(void)
{
	struct proc_dir_entry *hwtimer_entry = NULL;
	int err_code = 0;

	hwtimer_entry = proc_create(AMBARELLA_HWTIMER_NAME,	S_IRUGO | S_IWUGO,
		get_ambarella_proc_dir(), &hwtimer_proc_fops);

	if (!hwtimer_entry) {
		err_code = -EINVAL;
	}

	return err_code;
}

int get_hwtimer_output_freq(u32 *out_freq)
{
	unsigned long flags = 0;
	int rval = 0;

	spin_lock_irqsave(&timer_isr_lock, flags);
	if (hwtimer_enable_flag) {
		*out_freq = hwtimer_curr_outfreq;
	} else {
		*out_freq = 0;
	}
	spin_unlock_irqrestore(&timer_isr_lock, flags);

	return rval;
}
EXPORT_SYMBOL(get_hwtimer_output_freq);

static ssize_t hwtimer_outfreq_read_proc(struct file *file, char __user *buf,
	size_t size, loff_t *ppos)
{
	char str_freq[16] = {0};
	u32 out_freq = 0;
	unsigned long flags = 0;
	int rval = 0;

	if (size < 16) {
		printk("HWTimer: out freq read buf size %zd should be >= 16!\n", size);
		rval = -EINVAL;
		goto HWTIMER_OUTFREQ_READ_PROC_EXIT;
	}

	spin_lock_irqsave(&timer_isr_lock, flags);
	if (hwtimer_enable_flag) {
		out_freq = hwtimer_curr_outfreq;
	} else {
		out_freq = 0;
	}
	spin_unlock_irqrestore(&timer_isr_lock, flags);

	rval = snprintf(str_freq, sizeof(str_freq), "%u\n", out_freq);
	if(copy_to_user(buf, str_freq, rval)) {
		rval = -EFAULT;
	}

HWTIMER_OUTFREQ_READ_PROC_EXIT:
	return rval;
}

static ssize_t hwtimer_outfreq_write_proc(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	u32 gcd = 0, input = 0, output = 0, out_freq = 0;
	char buf[50] = {0};
	unsigned long flags = 0;
	ssize_t rval = 0;

	if (count >= 50 || count <= 1) {
		printk("HWTimer: %s: count %d out of size!\n", __func__, (u32)count);
		rval = -ENOSPC;
		goto HWTIMER_OUTFREQ_WRITE_PROC_EXIT;
	}

	if (copy_from_user(buf, buffer, count)) {
		printk("HWTimer: %s: copy_from_user fail!\n", __func__);
		rval = -EFAULT;
		goto HWTIMER_OUTFREQ_WRITE_PROC_EXIT;
	}
	buf[count] = '\0';
	out_freq = (u32)simple_strtoull(buf, NULL, 10);
	gcd = get_gcd(audio_input_freq, out_freq);
	input = audio_input_freq / gcd;
	output = out_freq / gcd;

	spin_lock_irqsave(&timer_isr_lock, flags);
	hwtimer_curr_outfreq = out_freq;
	audio_input_gcd = input;
	hwtimer_output_gcd = output;
	spin_unlock_irqrestore(&timer_isr_lock, flags);

	rval = count;

HWTIMER_OUTFREQ_WRITE_PROC_EXIT:
	return rval;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops hwtimer_freq_proc_fops =
{
	.proc_read 	= hwtimer_outfreq_read_proc,
	.proc_write 	= hwtimer_outfreq_write_proc,
};
#else
static const struct file_operations hwtimer_freq_proc_fops =
{
	.owner	= THIS_MODULE,
	.read 	= hwtimer_outfreq_read_proc,
	.write 	= hwtimer_outfreq_write_proc,
};
#endif

static int hwtimer_create_outfreq_proc(void)
{
	struct proc_dir_entry *outfreq_entry = NULL;
	int err_code = 0;

	outfreq_entry = proc_create(AMBARELLA_HWTIMER_OUTFREQ, S_IRUGO | S_IWUGO,
		get_ambarella_proc_dir(), &hwtimer_freq_proc_fops);

	if (!outfreq_entry) {
		printk("HWTimer: %s: create outfreq proc failed!\n", __func__);
		err_code = -EINVAL;
	}

	return err_code;
}

static irqreturn_t hwtimer_isr(int irq, void *dev_id)
{
	u32 curr_tick = 0;

	spin_lock(&timer_isr_lock);
	curr_tick = get_hwtimer_audio_tick();
	if (curr_tick < audio_isr_tick) {
		audio_overflow_count++;
	}
	audio_isr_tick = curr_tick;
	spin_unlock(&timer_isr_lock);

	return IRQ_HANDLED;
}

static void hwtimer_remove_proc(void)
{
	remove_proc_entry(AMBARELLA_HWTIMER_NAME, get_ambarella_proc_dir());
}

static void hwtimer_remove_outfreq_proc(void)
{
	remove_proc_entry(AMBARELLA_HWTIMER_OUTFREQ, get_ambarella_proc_dir());
}

static u64 hwtimer_get_fastboot_ticks(void)
{
	u32 gcd = 0, input = 0, output = 0;
	u32 input_freq = ambarella_hwtimer_input_freq();
	u32 output_freq = hwtimer_curr_outfreq;
	u64 total_ticks = readl_relaxed(hwtimer_reg_base + HWTIMER_TIMER4_STATUS_OFFSET);

	gcd = get_gcd(input_freq, output_freq);
	input = input_freq / gcd;
	output = output_freq / gcd;

	total_ticks = ((u64)1 << 32) - total_ticks;
	total_ticks = total_ticks * output + (input >> 1);
	if (likely(input)) {
		do_div(total_ticks, input);
	} else {
		printk("HWTimer: calculate output ticks failed! Can not divide zero!\n");
	}

	return total_ticks;
}

#ifdef CONFIG_PM
static int hwtimer_pause(void)
{
	u64 suspend_ticks = 0;

	get_hwtimer_output_ticks(&suspend_ticks);
	hwtimer_init_output_value = suspend_ticks;
	audio_isr_tick = 0;

	ambarella_hwtimer_disable();
	hwtimer_enable_flag = 0;

	return 0;
}

int hwtimer_suspend(void)
{
	hwtimer_pause();
	return 0;
}

EXPORT_SYMBOL(hwtimer_suspend);
int hwtimer_resume(void)
{
	hwtimer_reset();

	return 0;
}
EXPORT_SYMBOL(hwtimer_resume);

int hwtimer_freeze(void)
{
	return hwtimer_pause();
}
EXPORT_SYMBOL(hwtimer_freeze);

int hwtimer_thaw(int fastboot_audio)
{
	if (fastboot_audio) {
		// fast boot audio is enabled
		hwtimer_init_output_value = hwtimer_init_delta_seconds *
			hwtimer_curr_outfreq + hwtimer_get_fastboot_ticks();
		audio_isr_tick = 0;
	}

	return hwtimer_reset();
}
EXPORT_SYMBOL(hwtimer_thaw);
#endif

int hwtimer_reset(void)
{
	u64 ticks = 0;

	hwtimer_enable_flag = 0;
	ambarella_hwtimer_init();
	// reset audio tick values
	audio_overflow_count = 0;
	audio_init_value = get_hwtimer_audio_tick();
	if (unlikely(audio_init_value < audio_check_thres_value)) {
		ticks = ((u64)1 << 32) - audio_check_thres_value;
		/* Clear audio isr tick for overflow detection, when hw timer reset
		 * right after overflow happens
		 */
		if (audio_isr_tick > ticks) {
			audio_isr_tick = 0;
		}
	}

	return 0;
}

static struct platform_device *hwtimer_device = NULL;

struct hwtimer_irq_t {
	int irqno;
	irqreturn_t (*handler)(int irqno, void *dev_id);
	const char *desc;
};

struct hwtimer_irq_t hwtimer_irq = {AMBARELLA_HWTIMER_IRQ, hwtimer_isr, "hwtimer"};

static int init_hwtimer_irq(struct platform_device *pdev)
{
	int err_code = 0;
	int irq = -1;

	irq = of_irq_get_byname(pdev->dev.of_node, hwtimer_irq.desc);
	if (irq < 0)
		irq = hwtimer_irq.irqno;

	err_code = request_irq(irq, hwtimer_irq.handler,
		IRQF_TRIGGER_RISING, hwtimer_irq.desc, NULL);

	return err_code;
}

static int hwtimer_drv_probe(struct platform_device *pdev)
{
	int gcd = 0, err_code = 0;

	err_code = hwtimer_create_proc();
	if (err_code) {
		printk("HWTimer: create proc file for hw timer failed!\n");
		goto HWTIMER_DRV_PROBE_ERROR0;
	}

	err_code = hwtimer_create_outfreq_proc();
	if (err_code) {
		printk("HWTimer: create outfreq proc file for hw timer failed!\n");
		goto HWTIMER_DRV_PROBE_ERROR1;
	}

	err_code = init_hwtimer_irq(pdev);
	if (err_code) {
		printk("HWTimer: request irq for hw timer failed!\n");
		goto HWTIMER_DRV_PROBE_ERROR2;
	}

	hwtimer_reg_base = ioremap(HWTIMER_TIMER4_BASE, HWTIMER_TIMER4_REG_MAP_SIZE);
	if (hwtimer_reg_base == NULL) {
		err_code = -ENOMEM;
		goto HWTIMER_DRV_PROBE_ERROR3;
	}
	hwtimer_audio_base = ioremap(GLOBAL_TIMER_REG, 0x4);
	if (hwtimer_audio_base == NULL) {
		err_code = -ENOMEM;
		goto HWTIMER_DRV_PROBE_ERROR3;
	}

	hwtimer_input_freq = ambarella_hwtimer_input_freq();
	hwtimer_init_overflow_value = hwtimer_input_freq / 1000 *
		HWTIMER_OVERFLOW_MILLI_SECONDS;
	hwtimer_curr_outfreq = HWTIMER_DEFAULT_OUTPUT_FREQ;
	hwtimer_enable_flag = 0;
	audio_input_freq = ambarella_audio_input_freq();
	audio_check_thres_value = audio_input_freq / 1000 *
		AUDIO_CHECK_MILLI_SECONDS_THRES;
	gcd = get_gcd(audio_input_freq, hwtimer_curr_outfreq);
	audio_input_gcd = audio_input_freq / gcd;
	hwtimer_output_gcd = hwtimer_curr_outfreq / gcd;
	audio_isr_tick = 0;

	if (!fastboot) {
		hwtimer_init_output_value = 0;
	} else {
		// To support fastboot audio case
		hwtimer_init_output_value = hwtimer_init_delta_seconds *
			hwtimer_curr_outfreq + hwtimer_get_fastboot_ticks();
	}

	hwtimer_reset();
	spin_lock_init(&timer_isr_lock);

	goto HWTIMER_DRV_PROBE_EXIT;


HWTIMER_DRV_PROBE_ERROR3:
	if (err_code < 0) {
		if (hwtimer_reg_base) {
			iounmap((void __iomem *)hwtimer_reg_base);
			hwtimer_reg_base = NULL;
		}
		if (hwtimer_audio_base) {
			iounmap((void __iomem *)hwtimer_audio_base);
			hwtimer_audio_base = NULL;
		}
	}

HWTIMER_DRV_PROBE_ERROR2:
	free_irq(AMBARELLA_HWTIMER_IRQ, NULL);

HWTIMER_DRV_PROBE_ERROR1:
	hwtimer_remove_outfreq_proc();

HWTIMER_DRV_PROBE_ERROR0:
	hwtimer_remove_proc();

HWTIMER_DRV_PROBE_EXIT:
	return err_code;
}

static int hwtimer_drv_remove(struct platform_device *pdev)
{
	if (hwtimer_enable_flag) {
		ambarella_hwtimer_disable();
		hwtimer_enable_flag = 0;
	}
	iounmap((void __iomem *)hwtimer_reg_base);
	iounmap((void __iomem *)hwtimer_audio_base);
	free_irq(AMBARELLA_HWTIMER_IRQ, NULL);
	hwtimer_remove_outfreq_proc();
	hwtimer_remove_proc();

	return 0;
}

static const struct of_device_id hwtimer_dt_ids[] = {
	{ .compatible = "ambarella,hwtimer", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, hwtimer_dt_ids);

static struct platform_driver hwtimer_driver = {
	.probe = hwtimer_drv_probe,
	.remove = hwtimer_drv_remove,
	.driver = {
		.name = "hwtimer",
		.of_match_table = hwtimer_dt_ids,
	}
};

static int __init hwtimer_init(void)
{
	const char *dev_dts_path = "/iav/hwtimer";
	struct device_node *node = NULL;
	int rval = 0;

	node = of_find_node_by_path(dev_dts_path);
	if (node) {
		hwtimer_device = of_find_device_by_node(node);
		/* If the device is created, skip. */
		if (hwtimer_device == NULL) {
			/* If not, create once. */
			hwtimer_device = of_platform_device_create(node, NULL, NULL);
			if (hwtimer_device == NULL) {
				iav_error("Failed to create device.\n");
				rval = -EFAULT;
				goto HWTIMER_INIT_EXIT;
			}
		}
	} else {
		iav_error("Failed to find dts node: \"%s\".\n", dev_dts_path);
		rval = -EFAULT;
		goto HWTIMER_INIT_EXIT;
	}

	platform_driver_register(&hwtimer_driver);

HWTIMER_INIT_EXIT:
	return rval;
}

static void __exit hwtimer_exit(void)
{
	platform_driver_unregister(&hwtimer_driver);
}

module_init(hwtimer_init);
module_exit(hwtimer_exit);

MODULE_DESCRIPTION("Ambarella V5 hardware timer driver");
MODULE_AUTHOR("Zhaoyang Chen <zychen@ambarella.com>");
MODULE_LICENSE("GPL v2");

