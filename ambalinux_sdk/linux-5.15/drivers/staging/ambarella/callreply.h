/*
 * ambarella/callreply.h
 *
 * ARM HSM command call and reply handler
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#ifndef AMBARELLA_CALLREPLY_H
#define AMBARELLA_CALLREPLY_H

#include <hsm/api/struct.h>
#include "armhsm_private.h"

// #define DEBUG

struct mem_pool {
	uint32_t total_size;
	uint32_t used_size;
	void *base;
	void *cur;
};

/*
 * This struct is used to store ptrs in hsm_call.
 */
struct hsm_ptr_store {
	uint32_t id;
	hsm_ptr_t arg;
	hsm_ptr_t ptr[MAX_POINTERS];
};

int hsm_cre_mempool(struct mem_pool *pool, uint32_t pool_size);

void hsm_del_mempool(struct mem_pool *pool);

void cleanup_hsm_mem(struct mem_pool *pool, struct hsm_ptr_store *ptr_store);

int handle_call_incoming(struct hsm_call *call,
			 struct hsm_ptr_store *ptr_store,
			 struct mem_pool *pool);

int handle_reply_outgoing(struct hsm_call *call,
			  struct hsm_reply *reply,
			  struct hsm_ptr_store *ptr_store,
			  struct mem_pool *pool);


int package_call_incoming(struct armhsm *armhsm, struct hsm_call *call,
			  struct armhsm_dma *dma);
int package_reply_outgoing(struct armhsm *armhsm, struct hsm_call *call,
			  struct hsm_reply *reply, struct armhsm_dma *dma);

void finalize_call(struct hsm_call *call, struct hsm_ticket *ticket);

int kernel_call_incoming(struct hsm_call *call);

int kernel_reply_outgoing(struct hsm_call *call, struct hsm_reply *reply);

int nvmclnt_call_incoming(struct hsm_call *call);

int nvmclnt_reply_outgoing(struct hsm_call *call, struct hsm_reply *reply);

#endif

