/*
 * ambarella/msgq.c
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <soc/ambarella/iav_helper.h>
#include "comm.h"
#include "msgq.h"
#include "amalgam.h"

#define HSM_SHMEM_BASE		0xFFF20000UL
#define PHYS_TO_VIRT(x)		((x) > HSM_SHMEM_BASE ? shmem_comm_virt + (x) - HSM_SHMEM_BASE : phys_to_virt(x))

extern void __iomem *shmem_comm_virt;
extern phys_addr_t shmem_comm_phys;

void memory_sync_read(void *addr, size_t n)
{
	ambcache_clean_range(addr, n);
	ambcache_inv_range(addr, n);
}

void memory_sync_write(void *addr, size_t n)
{
	ambcache_clean_range(addr, n);
}

/*
 * Initialization of msgq.
 */
int msgq_init(struct msgq *msgq, uint64_t addr,
	      size_t msg_size, size_t nmsg, const char *name)
{
	int rval = 0;
	size_t n = sizeof(msgq->name);

	if (msgq == NULL) {
		rval = -1;
		goto done;
	}

	msgq->base_addr = addr;
	msgq->msg_size = msg_size;
	msgq->num_msgs = nmsg;
	msgq->wp = 0;
	msgq->rp = 0;
	strncpy(msgq->name, name, n);
	msgq->name[n - 1] = '\0';

done:

	return rval;
}

/*
 * Clean of msgq.
 */
int msgq_cleanup(struct msgq *msgq)
{
	int rval = 0;

	if (msgq == NULL) {
		rval = -1;
		goto done;
	}

	memset(msgq, 0x0, sizeof(struct msgq));

done:

	return rval;
}

/*
 * Push data to msgq from msgs array and wp is update.
 */
size_t msgq_push(struct msgq *msgq, void *const msgs, size_t nmsg)
{
	void *src, *dst;
	size_t n1, n2, actual_nmsg;

	if (msgq == NULL) {
		actual_nmsg = 0;
		goto done;
	}

	memory_sync_read(msgq, sizeof(*msgq));

	if (msgq_get_freespace(msgq) < nmsg)
		actual_nmsg = msgq_get_freespace(msgq);
	else
		actual_nmsg = nmsg;

	src = msgs;
	if (!comm_use_dram()) {
		dst = (void *) ((uint64_t) PHYS_TO_VIRT(msgq->base_addr) +
				(msgq->wp * msgq->msg_size));
	} else {
#if ARMHSM_USE_DMA
		dst = (void *) ((uint64_t) comm_dma_to_virt(msgq->base_addr, 1) +
				(msgq->wp * msgq->msg_size));
#else
		dst = (void *) ((uint64_t) phys_to_virt(msgq->base_addr) +
				(msgq->wp * msgq->msg_size));
#endif
	}

	if ((msgq->wp + actual_nmsg) <= msgq->num_msgs) {
		memcpy(dst, src, actual_nmsg * msgq->msg_size);
		memory_sync_write(dst, actual_nmsg * msgq->msg_size);
	} else {
		n1 = msgq->num_msgs - msgq->wp;
		memcpy(dst, src, n1 * msgq->msg_size);
		memory_sync_write(dst, n1 * msgq->msg_size);

		n2 = actual_nmsg - n1;
		src = (void *) ((char *) msgs + (n1 * msgq->msg_size));
		if (!comm_use_dram()) {
			dst = (void *) ((uint64_t) PHYS_TO_VIRT(msgq->base_addr));
		} else {
#if ARMHSM_USE_DMA
			dst = (void *) ((uint64_t) comm_dma_to_virt(msgq->base_addr, 1));
#else
			dst = (void *) ((uint64_t) phys_to_virt(msgq->base_addr));
#endif
		}
		memcpy(dst, src, n2 * msgq->msg_size);
		memory_sync_write(dst, n2 * msgq->msg_size);
	}

	if (msgq_update_wp(msgq, actual_nmsg) < 0) {
		actual_nmsg = 0;
		goto done;
	}

	/*
	 * Update wp only and leave rp no change, as it could be changed on
	 * remote processor. On real chip, if msgq is in cache area, memory
	 * address of wp or rp has to be cache line alignment in order to do
	 * cache handling properly. If whole msgq struct is update, rp will
	 * update to an old value and cause same msg pop out twice at remote
	 * processor.
	 */
	memory_sync_write(&msgq->wp, sizeof(msgq->wp));

done:

	return actual_nmsg;
}

/*
 * Pop data from msgq to msgs array and rp is update.
 */
size_t msgq_pop(struct msgq *msgq, void *msgs, size_t nmsg)
{
	void *src, *dst;
	size_t n1, n2, actual_nmsg;

	if (msgq == NULL) {
		actual_nmsg = 0;
		goto done;
	}

	memory_sync_read(msgq, sizeof(*msgq));

	if (msgq_get_datalen(msgq) < nmsg)
		actual_nmsg = msgq_get_datalen(msgq);
	else
		actual_nmsg = nmsg;

	if (actual_nmsg == 0) {
		goto done;
	}

	if (!comm_use_dram()) {
		src = (void *) ((uint64_t) PHYS_TO_VIRT(msgq->base_addr) +
				(msgq->rp * msgq->msg_size));
	} else {
#if ARMHSM_USE_DMA
		src = (void *) ((uint64_t) comm_dma_to_virt(msgq->base_addr, 0) +
				(msgq->rp * msgq->msg_size));
#else
		src = (void *) ((uint64_t) phys_to_virt(msgq->base_addr) +
				(msgq->rp * msgq->msg_size));
#endif
	}
	dst = msgs;
	if ((msgq->rp + actual_nmsg) <= msgq->num_msgs) {
		memory_sync_read(src, actual_nmsg * msgq->msg_size);
		memcpy(dst, src, actual_nmsg * msgq->msg_size);
	} else {
		n1 = msgq->num_msgs - msgq->rp;
		memory_sync_read(src, n1 * msgq->msg_size);
		memcpy(dst, src, n1 * msgq->msg_size);

		n2 = actual_nmsg - n1;
		if (!comm_use_dram()) {
			src = (void *) ((uint64_t) PHYS_TO_VIRT(msgq->base_addr));
		} else {
#if ARMHSM_USE_DMA
			src = (void *) ((uint64_t) comm_dma_to_virt(msgq->base_addr, 0));
#else
			src = (void *) ((uint64_t) phys_to_virt(msgq->base_addr));
#endif
		}
		dst = (void *) ((char *) msgs + (n1 * msgq->msg_size));
		memory_sync_read(src, n1 * msgq->msg_size);
		memcpy(dst, src, n2 * msgq->msg_size);
	}

	if (msgq_update_rp(msgq, actual_nmsg) < 0) {
		actual_nmsg = 0;
		goto done;
	}

	/*
	 * Update rp only and leave wp no change, as it could be changed on
	 * remote processor.
	 */
	memory_sync_write(&msgq->rp, sizeof(msgq->rp));

done:

	return actual_nmsg;
}

void *msgq_get_readaddr(struct msgq *msgq)
{
	void *ptr = NULL;

	if (msgq == NULL) {
		goto done;
	}

	if (!comm_use_dram()) {
		ptr = (void *) (PHYS_TO_VIRT(msgq->base_addr) +
				(msgq->rp * msgq->msg_size));
	} else {
#if ARMHSM_USE_DMA
		ptr = (void *) (comm_dma_to_virt(msgq->base_addr, 0) +
				(msgq->rp * msgq->msg_size));
#else
		ptr = (void *) (phys_to_virt(msgq->base_addr) +
				(msgq->rp * msgq->msg_size));
#endif
	}

done:

	return ptr;
}

void *msgq_get_writeaddr(struct msgq *msgq)
{
	void *ptr = NULL;

	if (msgq == NULL) {
		goto done;
	}

	if (!comm_use_dram()) {
		ptr = (void *) (PHYS_TO_VIRT(msgq->base_addr) +
				(msgq->wp * msgq->msg_size));
	} else {
#if ARMHSM_USE_DMA
		ptr = (void *) (comm_dma_to_virt(msgq->base_addr, 1) +
				(msgq->wp * msgq->msg_size));
#else
		ptr = (void *) (phys_to_virt(msgq->base_addr) +
				(msgq->wp * msgq->msg_size));
#endif
	}

done:

	return ptr;
}

int msgq_update_rp(struct msgq *msgq, size_t nmsg)
{
	int rval = 0;

	if ((msgq == NULL) || (nmsg > msgq->num_msgs)) {
		rval = -1;
		goto done;
	}

	msgq->rp += nmsg;
	if (msgq->rp >= msgq->num_msgs) {
		msgq->rp -= msgq->num_msgs;
	}

done:

	return rval;
}

int msgq_update_wp(struct msgq *msgq, size_t nmsg)
{
	int rval = 0;

	if ((msgq == NULL) || (nmsg > msgq->num_msgs)) {
		rval = -1;
		goto done;
	}

	msgq->wp += nmsg;
	if (msgq->wp >= msgq->num_msgs) {
		msgq->wp -= msgq->num_msgs;
	}

done:

	return rval;
}

size_t msgq_get_datalen(struct msgq *msgq)
{
	size_t used_msgs = 0;

	if (msgq == NULL) {
		used_msgs = 0;
		goto done;
	}

	if (msgq->wp >= msgq->rp) {
		/* Data length is 0 if wp catch up rp after init,
		   but it is wrong. This function is correct only if wp will
		   not catch up rp after init. */
		used_msgs = msgq->wp - msgq->rp;
	} else {
		used_msgs = msgq->num_msgs - (msgq->rp - msgq->wp);
	}

done:

	return used_msgs;
}

size_t msgq_get_freespace(struct msgq *msgq)
{
	size_t free_msgs;

	if (msgq == NULL) {
		free_msgs = 0;
		goto done;
	}

	if (msgq->rp > msgq->wp) {
		/* Free space is queue length if rp catch up wp after init,
		   but it is wrong. This function is correct only if rp will
		   not catch wp after init. */
		free_msgs = msgq->rp - msgq->wp;
	} else {
		free_msgs = msgq->num_msgs - (msgq->wp - msgq->rp);
	}

done:

	return free_msgs;
}

size_t msgq_get_heartbeat(struct msgq *msgq)
{
	if (msgq == NULL) {
		return 0;
	}

	memory_sync_read(&msgq->heartbeat, sizeof(msgq->heartbeat));

	return msgq->heartbeat;
}
