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
/* File source : "cavalry_log.c"                                                                  */
/* File size   : 8108 bytes                                                                       */
/* File md5sum : 96f152d2199ee7e48890aa0a09da0a91                                                 */
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
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/uaccess.h>
#include "cavalry.h"
#include "cavalry_mem.h"
#include "cavalry_log.h"
#include "cavalry_print.h"

int cavalry_log_init(struct ambarella_cavalry *cavalry)
{
	uint8_t *ring_buf = NULL;
	struct cavalry_log_status *log_status = &cavalry->log_status;

	ring_buf = kzalloc(CAVALRY_LOG_RING_BUF_SIZE, GFP_KERNEL);

	if (!ring_buf) {
		prt_err("Can not allocate mem for Log ring buffer!\n");
		return -1;
	}

	mutex_init(&log_status->cavalry_log_mutex);
	init_waitqueue_head(&cavalry->log_status.cavalry_log_wq);

	log_status->ring_buf_start = (struct cavalry_log_entry *)ring_buf;
	log_status->ring_buf_end = (struct cavalry_log_entry *)(ring_buf + CAVALRY_LOG_RING_BUF_SIZE);
	log_status->write_ptr = log_status->ring_buf_start;
	log_status->read_ptr = log_status->ring_buf_start;
	atomic_set(&log_status->ready_to_read, 0);
	log_status->last_seq_num = 0;
	log_status->curr_entry =
		(struct cavalry_log_entry *)cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].virt;
	memset((void *)log_status->curr_entry, 0, cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size);

	return 0;
}

void cavalry_log_exit(struct ambarella_cavalry *cavalry)
{
	struct cavalry_log_status *log_status = &cavalry->log_status;

	mutex_lock(&cavalry->log_status.cavalry_log_mutex);

	if (log_status->timer_initialized) {
		del_timer_sync(&log_status->timer);
		log_status->timer_initialized = 0;
	}
	if (log_status->ring_buf_start) {
		kfree(log_status->ring_buf_start);
		log_status->ring_buf_start = NULL;
	}

	mutex_unlock(&cavalry->log_status.cavalry_log_mutex);
}

ssize_t cavalry_log_read(struct file *filp, char __user *buffer, size_t count, loff_t *offp)
{
	struct ambarella_cavalry *cavalry = (struct ambarella_cavalry *)filp->private_data;
	struct cavalry_log_status *log_status = &cavalry->log_status;
	int rval = 0;
	ssize_t size_copy = 0;
	ssize_t size_return = 0;

	atomic_set(&log_status->has_reader, 1);

	wait_event_interruptible(log_status->cavalry_log_wq,
		(atomic_read(&log_status->ready_to_read) == 1));

	mutex_lock(&log_status->cavalry_log_mutex);
	if (log_status->read_ptr <= log_status->write_ptr) {
		size_copy = (void *)log_status->write_ptr - (void *)log_status->read_ptr;
		rval = copy_to_user(buffer, log_status->read_ptr, size_copy);
		size_return = size_copy - rval;
	} else {
		// two segments
		size_copy = (void *)log_status->ring_buf_end - (void *)log_status->read_ptr;
		rval = copy_to_user(buffer, log_status->read_ptr, size_copy);
		size_return = size_copy - rval;

		size_copy = (void *)log_status->write_ptr - (void *)log_status->ring_buf_start;
		rval = copy_to_user(buffer + size_return, log_status->ring_buf_start, size_copy);
		size_return += (size_copy - rval);
	}

	log_status->read_ptr = log_status->write_ptr;
	atomic_set(&log_status->ready_to_read, 0);
	mutex_unlock(&log_status->cavalry_log_mutex);
	atomic_set(&log_status->has_reader, 0);

	return size_return;
}
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
static void cavalry_log_timer_callback(unsigned long data)
{
	struct ambarella_cavalry *cavalry = (struct ambarella_cavalry *)data;
	struct cavalry_log_status *log_status = &cavalry->log_status;
	void *cavalry_log_start = cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].virt;
	void *cavalry_log_end = cavalry_log_start +
		cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size;
	uint32_t entry_cnt = 0;

	//mutex_lock(&log_status->cavalry_log_mutex);

	while (log_status->curr_entry->seq_num > log_status->last_seq_num) {
		log_status->last_seq_num = log_status->curr_entry->seq_num;
		memcpy(log_status->write_ptr, log_status->curr_entry, sizeof(struct cavalry_log_entry));
		entry_cnt++;
		log_status->write_ptr++;
		if (log_status->write_ptr == log_status->ring_buf_end) {
			log_status->write_ptr = log_status->ring_buf_start;
		}

		log_status->curr_entry++;
		if (log_status->curr_entry == cavalry_log_end) {
			log_status->curr_entry = cavalry_log_start;
		}
	}

	if (log_status->last_seq_num == 0xFFFFFFFF) {
		log_status->last_seq_num = 0;
	}

	if (entry_cnt > 0) {
		atomic_set(&log_status->ready_to_read, 1);
		wake_up_interruptible_all(&log_status->cavalry_log_wq);
	}
	//mutex_unlock(&log_status->cavalry_log_mutex);

	//Auto Restart
	if (log_status->timer_initialized) {
		mod_timer(&cavalry->log_status.timer, jiffies + CAVALRY_LOG_TIMER_PERIOD);
	}
}
#else
static void cavalry_log_timer_callback(struct timer_list *t)
{
    struct cavalry_log_status *log_status = from_timer(log_status, t, timer);
    struct ambarella_cavalry *cavalry = container_of(log_status, struct ambarella_cavalry, log_status);
	void *cavalry_log_start = cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].virt;
	void *cavalry_log_end = cavalry_log_start +
		cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size;
	uint32_t entry_cnt = 0;

	//mutex_lock(&log_status->cavalry_log_mutex);

	while (log_status->curr_entry->seq_num > log_status->last_seq_num) {
		log_status->last_seq_num = log_status->curr_entry->seq_num;
		memcpy(log_status->write_ptr, log_status->curr_entry, sizeof(struct cavalry_log_entry));
		entry_cnt++;
		log_status->write_ptr++;
		if (log_status->write_ptr == log_status->ring_buf_end) {
			log_status->write_ptr = log_status->ring_buf_start;
		}

		log_status->curr_entry++;
		if (log_status->curr_entry == cavalry_log_end) {
			log_status->curr_entry = cavalry_log_start;
		}
	}

	if (log_status->last_seq_num == 0xFFFFFFFF) {
		log_status->last_seq_num = 0;
	}

	if (entry_cnt > 0) {
		atomic_set(&log_status->ready_to_read, 1);
		wake_up_interruptible_all(&log_status->cavalry_log_wq);
	}
	//mutex_unlock(&log_status->cavalry_log_mutex);

	//Auto Restart
	if (log_status->timer_initialized) {
		mod_timer(&cavalry->log_status.timer, jiffies + CAVALRY_LOG_TIMER_PERIOD);
	}
}
#endif

int cavalry_log_start(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_log_status *log_status = &cavalry->log_status;

	mutex_lock(&cavalry->log_status.cavalry_log_mutex);

	if (!log_status->timer_initialized) {
		log_status->read_ptr = log_status->write_ptr;
		log_status->timer.expires = jiffies + CAVALRY_LOG_TIMER_PERIOD;
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
		log_status->timer.data = (unsigned long)cavalry;
		log_status->timer.function = &cavalry_log_timer_callback;
		init_timer(&log_status->timer);
#else
		timer_setup(&log_status->timer, cavalry_log_timer_callback, 0);
#endif
		log_status->timer_initialized = 1;
		add_timer(&log_status->timer);
	}

	mutex_unlock(&cavalry->log_status.cavalry_log_mutex);
	return 0;
}

int cavalry_log_stop(struct ambarella_cavalry *cavalry, void __user *arg)
{
	struct cavalry_log_status *log_status = &cavalry->log_status;

	mutex_lock(&cavalry->log_status.cavalry_log_mutex);

	if (log_status->timer_initialized) {
		del_timer_sync(&log_status->timer);
		log_status->timer_initialized = 0;
	}

	mutex_unlock(&cavalry->log_status.cavalry_log_mutex);
	return 0;
}

int cavalry_log_reset(struct ambarella_cavalry *cavalry)
{
	struct cavalry_log_status *log_status = &cavalry->log_status;

	mutex_lock(&cavalry->log_status.cavalry_log_mutex);

	if (log_status->timer_initialized) {
		del_timer_sync(&log_status->timer);
		log_status->timer_initialized = 0;
	}

	mutex_unlock(&cavalry->log_status.cavalry_log_mutex);

	// make sure last several logs are read out.
	cavalry_log_timer_callback((unsigned long)cavalry);

	while (1) {
		if (atomic_read(&log_status->ready_to_read) == 0 ||
			atomic_read(&log_status->has_reader) == 0) {
			log_status->write_ptr = log_status->ring_buf_start;
			log_status->read_ptr = log_status->ring_buf_start;
			log_status->last_seq_num = 0;
			log_status->curr_entry =
				(struct cavalry_log_entry *)cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].virt;
			memset((void *)log_status->curr_entry, 0, cavalry->cavalry_mem_info[CAVALRY_MEM_LOG].size);
			break;
		}
	}

	cavalry_log_start(cavalry, NULL);

	return 0;
}

