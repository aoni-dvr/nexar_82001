/*
 * ambarella/comm.h
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#ifndef AMBARELLA_COMM_H
#define AMBARELLA_COMM_H

#include "armhsm.h"
#include "msgq.h"
#include "armhsm_private.h"

extern void memory_sync_read(void *addr, size_t n);
extern void memory_sync_write(void *addr, size_t n);

struct hsm_comm {
	struct msgq *tx_mq;
	struct msgq *rx_mq;
};

extern struct hsm_comm arm0_comm;
extern struct hsm_comm arm0_pro_comm;

static inline uint32_t fnv1a_hash(const uint8_t *buf,
				  size_t size, uint32_t hash_in)
{
	uint32_t retval;

	retval = hash_in;

	if (buf != NULL) {
		unsigned int loop;

		for (loop = 0; loop < size; loop++) {
			retval ^= buf[loop];
			retval *= 16777619U;
		}
	}

	return retval;
}

static inline uint32_t call_chksum(const uint8_t *src)
{
	uint32_t chksum;
	const uint8_t *bottom_half = (const uint8_t *)
		(src + HSM_CALL_ARG_OFFSET);

	chksum = fnv1a_hash(src,
			    (HSM_CALL_ARG_OFFSET - 4),
			    0x811C9DC5U);
	chksum = fnv1a_hash(bottom_half,
			    (HSM_MSG_BYTE_SIZE - HSM_CALL_ARG_OFFSET),
			    chksum);

	return chksum;
}

static inline uint32_t reply_chksum(const uint8_t *src)
{
	uint32_t chksum;
	const uint8_t *bottom_half =
		(const uint8_t *) (src + HSM_REPLY_RET_OFFSET);

	chksum = fnv1a_hash(src,
			    (HSM_REPLY_RET_OFFSET - 4),
			    0x811C9DC5U);
	chksum = fnv1a_hash(bottom_half,
			    (HSM_MSG_BYTE_SIZE - HSM_REPLY_RET_OFFSET),
			    chksum);

	return chksum;
}

/*
 * Initialize struct hsm_comm. It setup early comm queue for domain.
 */
extern struct hsm_comm *comm_init_early(unsigned int domain);

/*
 * Initialize struct hsm_comm. It setup domain comm queue for domain.
 */
extern struct hsm_comm *comm_init(void *priv_data, unsigned int domain, unsigned int instance);

/*
 * Initialize struct hsm_comm. It setup provider comm queue for domain.
 */
extern struct hsm_comm *comm_init_pro(struct hsm_register_nvm_pro *nvm_pro);

/*
 * De-initialize provider queue and buffer.
 */
extern int comm_deinit_pro(struct hsm_register_nvm_pro *nvm_pro);

/*
 * Push messages to communication queue.
 *
 * Returns the number of messages pushed
 */
static inline unsigned int comm_msg_push(struct hsm_comm *comm,
					 void *const msgin,
					 unsigned int num_msg)
{
	unsigned int nmsg_push;

	if (comm == NULL || msgin == NULL || num_msg != 1) {
		nmsg_push = 0;
		goto done;
	}

	/* msgq_push already does the memory_sync in inside it */
	nmsg_push = msgq_push(comm->tx_mq, msgin, num_msg);

done:

	return nmsg_push;
}

/*
 * Pop messages from communication queue.
 *
 * Returns the number of messages popped
 */
static inline unsigned int comm_msg_pop(struct hsm_comm *comm,
					void *msgout, unsigned int num_msg)
{
	unsigned int nmsg_pop;

	if (comm == NULL || msgout == NULL) {
		nmsg_pop = 0;
		goto done;
	}

	/* msgq_pop already does the memory_sync in inside it */
	nmsg_pop = msgq_pop(comm->rx_mq, msgout, num_msg);

done:

	return nmsg_pop;
}

/*
 * Get number of message avaiable to pop and associated address for
 * the receiver queue.
 */
static inline int comm_get_msg_pop(struct hsm_comm *comm,
				   void **msgout, unsigned int *num_msg)
{
	int rval = 0;
	unsigned int n;
	if (comm == NULL || msgout == NULL || num_msg == NULL) {
		rval = -1;
		goto done;
	}

	memory_sync_read(comm->rx_mq, sizeof(*comm->rx_mq));
	*msgout = (void *) msgq_get_readaddr(comm->rx_mq);

	n = msgq_get_datalen(comm->rx_mq);
	if ((comm->rx_mq->rp + n) > comm->rx_mq->num_msgs) {
		/* only pop num_msgs to avoid rp warp around */
		n = comm->rx_mq->num_msgs - comm->rx_mq->rp;
	}

	if (n > 0) {
		memory_sync_read(*msgout, comm->rx_mq->msg_size * n);
	}

	*num_msg = n;

done:

	return rval;
}

/*
 * Update consumer (read) index of the receiver queue.
 */
static inline int comm_update_msg_pop(struct hsm_comm *comm,
				      unsigned int num_msg)
{
	int rval = 0;

	if (comm == NULL) {
		rval = -1;
		goto done;
	}

	memory_sync_read(comm->rx_mq, sizeof(*comm->rx_mq));
	rval = msgq_update_rp(comm->rx_mq, num_msg);
	memory_sync_write(comm->rx_mq, sizeof(*comm->rx_mq));

done:

	return rval;
}

static inline int comm_use_dram(void)
{
	if (armhsm_comm_instance() > HSM_SHMEM_COMM)
		return 1;

	return 0;
}
static inline void *comm_dma_to_virt(dma_addr_t phys, uint32_t dir)
{
	struct comm_inst_info *comm = armhsm_comm();

	if (dir)
		return comm->dom2hsm_virt + (phys - comm->dom2hsm_phys);
	else
		return comm->hsm2dom_virt + (phys - comm->hsm2dom_phys);
}

#endif	/* COMM_H */
