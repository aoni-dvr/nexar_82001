/*
 * ambarella/msgq.h
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#ifndef AMBARELLA_MSGQ_H
#define AMBARELLA_MSGQ_H

#include <hsm/comm.h>

/*
 * Initialization of msgq. Input structure, msgq, is initialized and
 * will be used by all other msqq APIs.
 */
extern int msgq_init(struct msgq *msgq, uint64_t addr,
		     size_t msg_size, size_t nmsg, const char *name);

/*
 * Clean up of msgq.
 */
extern int msgq_cleanup(struct msgq *msgq);

/*
 * Push data to msgq from msgs array and wp is update.
 */
extern size_t msgq_push(struct msgq *msgq,
			void *const msgs, size_t nmsg);

/*
 * Pop data from msgq to msgs array and rp is update.
 */
extern size_t msgq_pop(struct msgq *msgq, void *msgs, size_t nmsg);

/*
 * Get read address pointer in msgq.
 */
extern void *msgq_get_readaddr(struct msgq *msgq);

/*
 * Get write address pointer in msgq.
 */
extern void *msgq_get_writeaddr(struct msgq *msgq);

/*
 * Set the read position of msgq with offset from last rp.
 * The offset is in unit of item.
 */
extern int msgq_update_rp(struct msgq *msgq, size_t nmsg);

/*
 * Set the write position of msgq with offset from last wp.
 * The offset is in unit of item.
 */
extern int msgq_update_wp(struct msgq *msgq, size_t nmsg);

/*
 * Get data legnth in number of msgs in msgq.
 */
extern size_t msgq_get_datalen(struct msgq *msgq);

/*
 * Get free space in number of msgs in msgq.
 */
extern size_t msgq_get_freespace(struct msgq *msgq);

/*
 * Get heartbeat field in struct msgq.
 */
extern size_t msgq_get_heartbeat(struct msgq *msgq);

#endif /* MSGQ_H */

