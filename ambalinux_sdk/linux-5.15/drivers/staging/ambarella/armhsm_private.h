/*
 * ambarella/armhsm_private.h
 *
 * This file contains structures and utility functions which
 * can only be included by armhsm.c.
 *
 * Copyright (C) 2022, Ambarella Inc.
 */

#ifndef AMBARELLA_ARMHSM_PRIVATE_H
#define AMBARELLA_ARMHSM_PRIVATE_H

#include <linux/dma-mapping.h>

struct comm_inst_info {
	void *hsm2dom_virt;
	dma_addr_t hsm2dom_phys;
	size_t hsm2dom_size;

	void *dom2hsm_virt;
	dma_addr_t dom2hsm_phys;
	size_t dom2hsm_size;
};
/*
 * ARM HSM driver object
 */
struct armhsm {
	unsigned int domain;
	struct device *dev;
	struct device_node *node;
	struct semaphore hsm_sem;
	struct mutex comm_lock;
	struct work_struct hsm_work;
	struct workqueue_struct *hsm_workqueue;
	struct work_struct nvmclnt_work;
	struct workqueue_struct *nvmclnt_workqueue;
	struct hsmnvm hsmnvm;
	struct hsm_comm *comm;
	struct hsm_comm *pro_comm;
	uint32_t armhsm_irq0;
	uint32_t armhsm_irq1;
	uint32_t clnthsm_irq;

	uint32_t comm_inst;
	struct comm_inst_info comm_info[HSM_NUM_DRAM_COMM + 1];
	int ticket_waitcnt;
	struct regmap *ca78ae_scr_reg;
	struct __logbuf {
		char *buf;
		uint32_t *wp;
		uint32_t *rp;
		uint32_t size;
	} logbuf;
	dma_addr_t logbuf_phys_addr;
};

struct armhsm_dma {
	void *arg;
	void __user *user_ptr;
	dma_addr_t arg_dma;
	uint32_t size;
	struct arg_member {
		void *ptr;
		void __user *user_ptr;
		dma_addr_t dma;
		uint32_t size;
	} member[16];
};

#define HSM_LOGBUF_SIZE 32768

//#define DEBUG_HSM_TICKET

#define ARMHSM_TICKET_NUM	4
#if !defined(CONFIG_AMALGAM)
#define ARMHSM_WAIT_TIMEOUT     1000    // 1 second
#else
#define ARMHSM_WAIT_TIMEOUT     5000    // 5 seconds
#endif

struct hsm_ticket {
	uint32_t state;
#define ARMHSM_TICKET_UNUSED	0
#define ARMHSM_TICKET_USED	1

	uint32_t data;
	struct semaphore sem;
	struct hsm_reply reply;
};

extern uint32_t seqno;

static inline uint32_t get_hsmcall_seqno(void)
{
	return seqno;
}

#endif
