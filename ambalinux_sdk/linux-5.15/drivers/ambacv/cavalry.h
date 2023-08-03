/*
 * Copyright (c) 2018-2019 Ambarella, Inc.
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
/* File source : "cavalry.h"                                                                      */
/* File size   : 11420 bytes                                                                      */
/* File md5sum : ff73f834e46e459fe78434dcde1274f9                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing:                                                                  */
/*  + Added in defines specific to CVFLOW                                                         */
/*  + Added in components to struct ambarella_cavalry for CVFLOW                                  */
/*  + Modified function prototypes to match into CVFLOW structure                                 */
/*  + Removed defines of commands and messages that is already in cvapi_cavalry.h                 */
/*================================================================================================*/

#ifndef __CAVALRY_H__
#define __CAVALRY_H__

#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/semaphore.h>
#include "cavalry_ioctl.h"

/*-= Added for integration with CVFLOW =----------------------------------vvv-*/
#include <cvapi_cavalry.h>      /* For some CVFLOW/CAVALRY definitions        */
#define CAV_SLOT_AVAILABLE      0
#define CAV_SLOT_USED           1

#define CAV_INVALID_SLOTID      0xFFFFFFFFU

#define MAX_CAVALRY_TOKENS      32    /* Tokens are used for FMA/FEX */
#define CAV_TOKEN_AVAILABLE     0
#define CAV_TOKEN_USED          1
#define CAV_TOKEN_WAITING       2
#define CAV_TOKEN_FINISHED      3

/*-= MAX_REPLY_SIZE =-----------------------------------------------*/
/* The maximum size of the replies are based on the following :     */
/*  schedmsg_cavalry_hotlink_run_reply_t : 28 bytes (schdrmsg_def.h)*/
/*  struct fex_query_msg  : 12 bytes                                */
/*  struct fex_run_msg    : 84 bytes                                */
/*  struct fma_query_msg  : 12 bytes                                */
/*  struct fma_run_msg    : 40 bytes                                */
/*------------------------------------------------------------------*/
#define MAX_REPLY_SIZE          88

/*-= Added for integration with CVFLOW =----------------------------------^^^-*/

#define MSG_SIZE	(128U)
#define MAX_MSG_CNT	(32)
// CMD_Q should be big enough to hold all DAGs' descriptors
#define CAVALRY_MEM_CMD_SIZE (1 << 20)
#define CAVALRY_MEM_MSG_SIZE (MSG_SIZE * MAX_MSG_CNT)
#define CAVALRY_MEM_LOG_SIZE	(128 << 10)
#define CAVALRY_MEM_CMD_FEX_SIZE (1 << 20)
#define CAVALRY_MEM_MSG_FEX_SIZE (MSG_SIZE * MAX_MSG_CNT)
#define CAVALRY_MEM_CMD_FMA_SIZE (1 << 20)
#define CAVALRY_MEM_MSG_FMA_SIZE (MSG_SIZE * MAX_MSG_CNT)

#define CAVALRY_MEM_CMD_OFFSET	(0x0 + MSG_SIZE)
#define CAVALRY_MEM_MSG_OFFSET	(CAVALRY_MEM_CMD_OFFSET + CAVALRY_MEM_CMD_SIZE)
#define CAVALRY_MEM_LOG_OFFSET	(CAVALRY_MEM_MSG_OFFSET + CAVALRY_MEM_MSG_SIZE)
#define CAVALRY_MEM_CMD_FEX_OFFSET	(CAVALRY_MEM_LOG_OFFSET + CAVALRY_MEM_LOG_SIZE)
#define CAVALRY_MEM_MSG_FEX_OFFSET	(CAVALRY_MEM_CMD_FEX_OFFSET + CAVALRY_MEM_CMD_FEX_SIZE)
#define CAVALRY_MEM_CMD_FMA_OFFSET	(CAVALRY_MEM_MSG_FEX_OFFSET + CAVALRY_MEM_MSG_FEX_SIZE)
#define CAVALRY_MEM_MSG_FMA_OFFSET	(CAVALRY_MEM_CMD_FMA_OFFSET + CAVALRY_MEM_CMD_FMA_SIZE)

#define CAVALRY_MEM_UCODE_OFFSET	(0x400000)
#define CAVALRY_MEM_HOTLINK_OFFSET	(0x500000)
#define CAVALRY_MEM_USER_OFFSET	(0x600000)

#define CAVALRY_MEM_HOTLINK_SLOT_SIZE	(0x40000)
#define CAVALRY_MEM_HOTLINK_SLOT0_OFFSET	(CAVALRY_MEM_HOTLINK_OFFSET + CAVALRY_MEM_HOTLINK_SLOT_SIZE * 0)
#define CAVALRY_MEM_HOTLINK_SLOT1_OFFSET	(CAVALRY_MEM_HOTLINK_OFFSET + CAVALRY_MEM_HOTLINK_SLOT_SIZE * 1)
#define CAVALRY_MEM_HOTLINK_SLOT2_OFFSET	(CAVALRY_MEM_HOTLINK_OFFSET + CAVALRY_MEM_HOTLINK_SLOT_SIZE * 2)
#define CAVALRY_MEM_HOTLINK_SLOT3_OFFSET	(CAVALRY_MEM_HOTLINK_OFFSET + CAVALRY_MEM_HOTLINK_SLOT_SIZE * 3)

#define CAVALRY_LOG_RING_BUF_SIZE	(1 << 20)

#define CAVALRY_SLOT_ID_OFFSET	(0x04)
#define CAVALRY_HOTLINK_FW_VER_OFFSET	(0x08)

struct cavalry_mem_info {
	unsigned long phys;
	void *virt;
	unsigned long size;
};

struct cavalry_cma_info {
	dma_addr_t phys;
	void *virt;
	size_t size;
};

struct cavalry_driver_version {
	int major;
	int minor;
	int patch;
	uint32_t mod_time;
	char description[64];
};

struct cavalry_log_status {
	wait_queue_head_t cavalry_log_wq;
	struct mutex cavalry_log_mutex;
	struct timer_list timer;
	struct cavalry_log_entry *ring_buf_start;
	struct cavalry_log_entry *ring_buf_end;
	struct cavalry_log_entry *write_ptr;
	struct cavalry_log_entry *read_ptr;
	struct cavalry_log_entry *curr_entry;
	uint32_t last_seq_num;
	atomic_t ready_to_read;
	atomic_t has_reader;
	uint32_t timer_initialized : 1;
	uint32_t reserved : 31;
};

struct cma_mmb {
	phys_addr_t phys_addr;
	void *virt_addr;
	size_t size;

	uint32_t cache_en : 1;
	uint32_t reserved_0: 31;

	uint32_t user_ref_cnt;

	struct file *f;

	struct list_head list;
};

struct ambarella_cavalry {
	struct cdev cavalry_cdev;
	struct device *dev;
	struct cavalry_mem_info cavalry_mem_info[CAVALRY_MEM_NUM];
	struct cavalry_cma_info cma_private;

	/* CMA */
	struct list_head mmb_list;
	struct list_head free_mmb_list;
	struct proc_dir_entry *cma_proc;

	struct semaphore cavalry_vp_sem;
	struct semaphore cavalry_fex_sem;
	struct semaphore cavalry_fma_sem;
	struct mutex cavalry_early_quit_mutex;
	struct mutex cavalry_buf_query_mutex;
	struct mutex cavalry_cma_mutex;
	struct spinlock cavalry_cmd_msg_vp_lock;
	struct spinlock cavalry_cmd_msg_fex_lock;
	struct spinlock cavalry_cmd_msg_fma_lock;
	wait_queue_head_t cavalry_vp_wq;
	wait_queue_head_t cavalry_fex_wq;
	wait_queue_head_t cavalry_fma_wq;
	struct cavalry_run_dags *run_dags;
	struct cavalry_log_status log_status;
	atomic_t is_vp_started;
	atomic_t is_vp_done;
	atomic_t is_early_quit_all;
	atomic_t pending_cnt;
	atomic_t is_fex_done;
	atomic_t is_fma_done;
	uint32_t slot_activated;
	uint8_t log_level;
	uint8_t is_vp_hang : 1;
	uint8_t reserved0 : 7;
	uint8_t reserved1[2];
	int vp_current_pid;

	/* CMA memory for core dump*/
	struct cma_mmb *vp_core_dump;

	/* Audio Clock */
	uint64_t audio_clk;

/*-= Added for integration with CVFLOW =----------------------------------vvv-*/
	struct cavalry_mem_info cavalry_uncached_logblock;

	struct mutex      slot_mutex;
	struct mutex      token_mutex;

	struct semaphore  slot_semaphore;
	struct semaphore  token_semaphore;

	uint32_t          num_slots_enabled;
	uint32_t          next_slot_id;
	uint8_t           slot_state[MAX_CAVALRY_SLOTS];
	uint32_t          slot_daddr[MAX_CAVALRY_SLOTS];

	uint32_t          next_token_id;
	uint32_t          cavtoken_state[MAX_CAVALRY_TOKENS];
	uint32_t          cavtoken_slotid[MAX_CAVALRY_TOKENS];
	uint32_t          cavtoken_retcode[MAX_CAVALRY_TOKENS];
	uint8_t           cavtoken_replymsg[MAX_CAVALRY_TOKENS][MAX_REPLY_SIZE];
	struct semaphore  cavtoken[MAX_CAVALRY_TOKENS];

	uint32_t          cavalry_memblock_base;
	uint32_t          cavalry_memblock_size;

	uint32_t          cavalry_trace_daddr;
	void             *vpCavalryTrace;
/*-= Added for integration with CVFLOW =----------------------------------^^^-*/
};

uint32_t cmd_to_msg(uint32_t cmd);
int cavalry_query_buf(struct ambarella_cavalry *cavalry, void __user *arg);
#if 0  /*-= Removed for integration with CVFLOW =---------------------------------------------vvv-*/
void visorc_init(struct ambarella_cavalry *cavalry);
int visorc_start(struct ambarella_cavalry *cavalry, void __user *arg);
int visorc_stop(struct ambarella_cavalry *cavalry, void __user *arg);
void visorc_shutdown(struct ambarella_cavalry *cavalry);
int cavalry_run_dags(struct ambarella_cavalry *cavalry, void __user *arg);
#endif /*-= Removed for integration with CVFLOW =---------------------------------------------^^^-*/
int cavalry_visorc_early_quit(struct ambarella_cavalry *cavalry, void __user *arg);     /* <<<- prefixed for integration with CVFLOW */
int cavalry_run_hotlink_slot(struct ambarella_cavalry *cavalry, void __user *arg);
int cavalry_set_hotlink_slot_cfg(struct ambarella_cavalry *cavalry, void __user *arg);
int cavalry_get_hotlink_slot_cfg(struct ambarella_cavalry *cavalry, void __user *arg);
int ioctl_cavalry_fex_query(struct ambarella_cavalry *pCavalry, void __user *puArg);    /* <<<- prefixed for integration with CVFLOW */
int ioctl_cavalry_fex_run(struct ambarella_cavalry *pCavalry, void __user *puArg);      /* <<<- prefixed for integration with CVFLOW */
int ioctl_cavalry_fma_query(struct ambarella_cavalry *pCavalry, void __user *puArg);    /* <<<- prefixed for integration with CVFLOW */
int ioctl_cavalry_fma_run(struct ambarella_cavalry *pCavalry, void __user *puArg);      /* <<<- prefixed for integration with CVFLOW */
int cavalry_set_log_level(struct ambarella_cavalry *cavalry, void __user *arg);

/*-= Added for integration with CVFLOW =------------------------------------------------------vvv-*/
void cavalry_system_init(struct ambarella_cavalry *cavalry);
void cavalry_system_shutdown(struct ambarella_cavalry *cavalry);
int  cavalry_visorc_start(struct ambarella_cavalry *cavalry, void __user *arg);
int  cavalry_visorc_stop(struct ambarella_cavalry *cavalry, void __user *arg);
int  ioctl_cavalry_run_dags(struct ambarella_cavalry *pCavalry, void __user *arg);
int  ioctl_cavalry_run_hotlink_slot(struct ambarella_cavalry *pCavalry, void __user *arg);
int  ioctl_cavalry_enable(struct ambarella_cavalry *cavalry, uint32_t arg);
int  ioctl_cavalry_associate_buf(struct ambarella_cavalry *cavalry, uint32_t arg);
int  ioctl_cavalry_set_memory_block(struct ambarella_cavalry *cavalry, void __user *arg);
int  ioctl_cavalry_handle_visorc_reply(struct ambarella_cavalry *cavalry, void *arg);
void krn_cavalry_system_set_trace_daddr(struct ambarella_cavalry *pCavalry, uint32_t cavalry_trace_daddr);
uint32_t krn_cavalry_system_get_trace_daddr(struct ambarella_cavalry *pCavalry);

/*-= Added for integration with CVFLOW =------------------------------------------------------^^^-*/

#endif //__CAVALRY_H__
