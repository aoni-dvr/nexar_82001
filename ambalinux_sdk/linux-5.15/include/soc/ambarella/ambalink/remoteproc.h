/*
 * arch/arm/plat-ambarella/include/plat/remoteproc.h
 *
 * Author: Tzu-Jung Lee <tjlee@ambarella.com>
 *
 * Copyright (C) 2012-2012, Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef __PLAT_AMBARELLA_REMOTEPROC_H
#define __PLAT_AMBARELLA_REMOTEPROC_H

#include <linux/remoteproc.h>

/*
 * The rpmsg profiling related data structure use the same definition in dual-OSes.
 * This data structure
 */
//#define RPMSG_DEBUG

#define AMBA_RPMSG_MASTER (0)
#define AMBA_RPMSG_SLAVE  (1)

struct ambarella_rproc_pdata {
	const char              *name;
	struct rproc            *rproc;
	const char              *firmware;
	unsigned int            svq_tx_irq;	// amb,svq_kick, Kick remot
	unsigned int            svq_rx_irq;	// interrupts[1]
	unsigned int            rvq_tx_irq;	// amb, rvq_ack
	unsigned int            rvq_rx_irq;	// interrupts[0], IRQ of Rx from remote
	const struct rproc_ops  *ops;
	unsigned long           buf_addr_pa;
	struct work_struct      svq_work;
	struct work_struct      rvq_work;
	struct work_struct      virtio_work;
	struct resource_table   *(*gen_rsc_table)(int *tablesz, int);
	struct regmap           *reg_ahb_scr;
	int 			shm_id;
	uint32_t		sp_swi_set_offset;	/* set scratchpad sw interrupt   */
	uint32_t		sp_swi_clear_offset;	/* clear scratchpad sw interrupt */
	uint32_t		sp_swi0;	        /* value of amba_axi_software_irq[0] */

	uint32_t		rpmsg_num;
	uint32_t		rpmsg_size;
	uint32_t		rpmsg_role;

	phys_addr_t		virtio_buf;		/* Start physicall address for VirtIO buffer used by dma-map */
	uint32_t		virtio_status;		/* for Slave/Remote, 1 as remote is OK */
};

#ifdef RPMSG_DEBUG
/******************************The defintion is shared between dual-OSes*******************************/
typedef struct {
	unsigned int ToGetSvqBuffer;
	unsigned int GetSvqBuffer;
	unsigned int SvqToSendInterrupt;
	unsigned int SvqSendInterrupt;
} AMBA_RPMSG_PROFILE_s;

typedef struct _AMBA_RPMSG_STATISTIC_s_ {
	/**********************************ThreadX side**************************/
	unsigned int TxLastInjectTime;
	unsigned int TxTotalInjectTime;
	unsigned int TxSendRpmsgTime;
	unsigned int TxResponseTime;
	unsigned int MaxTxResponseTime;
	unsigned int TxRecvRpmsgTime;
	unsigned int TxRecvCallBackTime;
	unsigned int TxReleaseVqTime;
	unsigned int TxToLxRpmsgTime;
	unsigned int MaxTxToLxRpmsgTime;
	unsigned int MinTxToLxRpmsgTime;
	unsigned int MaxTxRecvCBTime;
	unsigned int MinTxRecvCBTime;
	int TxToLxCount;
	int TxToLxWakeUpCount;
	/************************************************************************/
	/**********************************Linux side****************************/
	unsigned int LxLastInjectTime;
	unsigned int LxTotalInjectTime;
	unsigned int LxSendRpmsgTime;
	unsigned int LxResponseTime;
	unsigned int MaxLxResponseTime;
	unsigned int LxRecvRpmsgTime;
	unsigned int LxRecvCallBackTime;
	unsigned int LxReleaseVqTime;
	unsigned int LxToTxRpmsgTime;
	unsigned int MaxLxToTxRpmsgTime;
	unsigned int MinLxToTxRpmsgTime;
	unsigned int MaxLxRecvCBTime;
	unsigned int MinLxRecvCBTime;
	int LxRvqIsrCount;
	int LxToTxCount;
	/************************************************************************/
} AMBA_RPMSG_STATISTIC_s;
/*******************************************************************************************************/

struct profile_data{
	unsigned int ToGetSvqBuffer;
	unsigned int GetSvqBuffer;
	unsigned int SvqToSendInterrupt;
	unsigned int SvqSendInterrupt;
	unsigned int ToGetRvqBuffer;
	unsigned int GetRvqBuffer;
	unsigned int ToRecvData;
	unsigned int RecvData;
	unsigned int ReleaseRvq;
	unsigned int RvqToSendInterrupt;
	unsigned int RvqSendInterrupt;
} ;
#endif /* The data structure of RPMSG profiling. */

#endif /* __PLAT_AMBARELLA_REMOTEPROC_H */
