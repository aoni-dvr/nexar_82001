/*
 * ambarella/comm.c
 *
 * Copyright (C) 2021, Ambarella Inc.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/memblock.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>

#include "armhsm.h"
#include "amalgam.h"
#include "armhsm_private.h"
#include "comm.h"

// #define DEBUG

/*
 * The idea is to choose a memory region below 0x80000000,
 * which is current amalgam configuration.
 *
 * From cat /proc/iomem, choose a memory region,
 *	7de40000-7e17ffff : reserved
 *
 * Use 0x20000 for comm memory.
 * Before Init_HSM_MsgQueue, comm memory should be in SHMEM.
 * After Init_HSM_MsgQueue, comm memory is specified by client.
 */

// FIXME: use correct shmem address
#undef SHMEM_BASE
#define SHMEM_BASE	0xFF00000000UL
#define HSM_SHMEM_BASE	0xFFF20000UL

#define COMM_MEM_SIZE	0x20000

/* Offset of message */
#define SHMEM_MSGS_ARRAY_OFFSET ((sizeof(struct msgq) * 2) * 1)
#define DRAM_MSGS_ARRAY_OFFSET	((sizeof(struct msgq) * 2) * 4)

uint32_t hsm_api_hash = 0x0;

struct hsm_comm arm0_comm;
struct hsm_comm arm0_pro_comm;
void __iomem *shmem_comm_virt;
phys_addr_t shmem_comm_phys;


static void show_comm_info(struct hsm_comm *comm)
{
#if defined(DEBUG)
#if defined(CONFIG_AMALGAM)
	memory_sync_read(comm->tx_mq, sizeof(*comm->tx_mq));
	memory_sync_read(comm->rx_mq, sizeof(*comm->rx_mq));
#endif
	pr_info("tx_mq = 0x%llx\n", virt_to_phys(comm->tx_mq));
	pr_info("name addr = 0x%llx\n", (uint64_t)comm->tx_mq->name);
	pr_info("name = %s\n", comm->tx_mq->name);
	pr_info("base_addr = 0x%x\n", (uint32_t) comm->tx_mq->base_addr);
	pr_info("msg_size = 0x%x\n", comm->tx_mq->msg_size);
	pr_info("num_msgs = 0x%x\n", comm->tx_mq->num_msgs);

	pr_info("rx_mq = 0x%llx\n", virt_to_phys(comm->rx_mq));
	pr_info("name = %s\n", comm->rx_mq->name);
	pr_info("base_addr = 0x%x\n", (uint32_t) comm->rx_mq->base_addr);
	pr_info("msg_size = 0x%x\n", comm->rx_mq->msg_size);
	pr_info("num_msgs = 0x%x\n", comm->rx_mq->num_msgs);
#endif
}


/*
 * FIXME:
 * Setup early comm queue from shmem here. It actually should be setup in
 * ATF and in linux, it should call Init_HSM_MsgQueue at kernel initial stage
 * and armhsm driver should use the queue address for future communication.
 */
struct hsm_comm *comm_init_early(unsigned int domain)
{
	struct hsm_comm *comm = NULL;
	size_t queue_size;
	void *virt_addr;

	shmem_comm_phys = SHMEM_BASE;
	/* ioremap comm memory to non-cachable */
	shmem_comm_virt = ioremap_wc(SHMEM_BASE, 0x08000000);
	if (!shmem_comm_virt)  {
		pr_err("[armhsm]: %s ioremap_wc error\n", __func__);
		return NULL;
	}

	pr_info("[armhsm]: %s at domain %u physical addr 0x%lx\n",
		 __func__, domain, SHMEM_BASE);

	hsm_api_hash = fnv1a_hash((void *) hsm_api_ptrs, sizeof(hsm_api_ptrs),
				  0x811c9dc5U);

#if defined(CONFIG_AMALGAM)
	if (am_is_connected() == 0) {
		am_connect();
	}
#endif
	queue_size = HSM_MSG_BYTE_SIZE * HSM_NUM_MSG + sizeof(struct msgq);
	virt_addr = shmem_comm_virt + domain * queue_size * 2;

	comm = &arm0_comm;
	comm->rx_mq = (struct msgq *) virt_addr;
	comm->tx_mq = (struct msgq *) (virt_addr + queue_size);

#if defined(CONFIG_AMALGAM)
	memory_sync_read(comm->tx_mq, sizeof(*comm->tx_mq));
	memory_sync_read(comm->rx_mq, sizeof(*comm->rx_mq));
#endif

	if ((comm->tx_mq->apihash != hsm_api_hash) ||
	    (comm->rx_mq->apihash != hsm_api_hash)) {
		panic("HSM API struct hash mismatch: 0x%x != 0x%x\n",
		      hsm_api_hash, comm->tx_mq->apihash);
		comm = NULL;
		goto done;
	}

	show_comm_info(comm);

done:

	return comm;
}

#if ARMHSM_USE_DMA
struct hsm_comm *comm_init(void *priv_data, unsigned int domain,
			   unsigned int instance)
{
	struct armhsm *armhsm = (struct armhsm *)priv_data;
	struct hsm_comm *comm = NULL;
	struct hsm_dom_msgq *dom_msgq;
	void *dom2hsm, *hsm2dom;
	size_t queue_size;
	dma_addr_t dom_msgq_dma;
	dma_addr_t dom2hsm_dma;
	dma_addr_t hsm2dom_dma;

	pr_debug("[armhsm]: %s at domain %u instance %u\n",
		 __func__, domain, instance);

	queue_size = HSM_MSG_BYTE_SIZE * HSM_NUM_MSG + sizeof(struct msgq);

	dom_msgq = dma_alloc_coherent(armhsm->dev, sizeof(*dom_msgq),
				      &dom_msgq_dma, GFP_KERNEL);
	if (!dom_msgq) {
		dev_err(armhsm->dev, "%s: alloc domain msgq error.\n", __func__);
		return NULL;
	}
	dom2hsm = dma_alloc_coherent(armhsm->dev, queue_size,
				     &dom2hsm_dma, GFP_KERNEL);
	if (!dom2hsm) {
		dev_err(armhsm->dev, "%s: alloc dom2hsm error.\n", __func__);
		return NULL;
	}

	hsm2dom = dma_alloc_coherent(armhsm->dev, queue_size,
				     &hsm2dom_dma, GFP_KERNEL);
	if (!hsm2dom) {
		dev_err(armhsm->dev, "%s: alloc hsm2dom error.\n", __func__);
		return NULL;
	}

	armhsm->comm_info[instance].dom2hsm_virt = dom2hsm;
	armhsm->comm_info[instance].dom2hsm_phys = dom2hsm_dma;
	armhsm->comm_info[instance].hsm2dom_size = queue_size;

	armhsm->comm_info[instance].hsm2dom_virt = hsm2dom;
	armhsm->comm_info[instance].hsm2dom_phys = hsm2dom_dma;
	armhsm->comm_info[instance].hsm2dom_size = queue_size;

	dom_msgq->domid = domain;
	dom_msgq->instance = instance;
	dom_msgq->remove = 0;
	dom_msgq->dom2hsm_addr = (hsm_ptr_t) dom2hsm_dma;
	dom_msgq->hsm2dom_addr = (hsm_ptr_t) hsm2dom_dma;
	dom_msgq->dom2hsm_size = queue_size;
	dom_msgq->hsm2dom_size = queue_size;

	if (AmbaHSM_Init_HSM_MsgQueue((struct hsm_dom_msgq *)dom_msgq_dma) < 0) {
		goto done;
	}

	comm = &arm0_comm;
	comm->rx_mq = (struct msgq *) hsm2dom;
	comm->tx_mq = (struct msgq *) dom2hsm;

#if defined(ARMHSM_ATF_SHMEM)
	hsm_api_hash = fnv1a_hash((void *) hsm_api_ptrs, sizeof(hsm_api_ptrs),
				  0x811c9dc5U);
#endif

	if ((comm->tx_mq->apihash != hsm_api_hash) ||
	    (comm->rx_mq->apihash != hsm_api_hash)) {
		pr_err("Mismatch API struct hash between armhsm driver "
		       "and orchsm: 0x%x != 0x%x\n",
		       hsm_api_hash, comm->tx_mq->apihash);
		comm = NULL;
		goto done;
	}

	show_comm_info(comm);
done:

	if (dom_msgq)
		dma_free_coherent(armhsm->dev,
				  sizeof(struct hsm_dom_msgq),
				  dom_msgq, dom_msgq_dma);

	return comm;
}
#else
struct hsm_comm *comm_init(void *priv_data, unsigned int domain,
			   unsigned int instance)
{
	struct hsm_comm *comm = NULL;
	struct hsm_dom_msgq *dom_msgq;
	void *dom2hsm, *hsm2dom;
	size_t queue_size;

	pr_debug("[armhsm]: %s at domain %u instance %u\n",
		 __func__, domain, instance);

	queue_size = HSM_MSG_BYTE_SIZE * HSM_NUM_MSG + sizeof(struct msgq);

	dom_msgq = kzalloc(sizeof(*dom_msgq), GFP_KERNEL);
	if (dom_msgq == NULL)
		goto done;

	dom2hsm = kzalloc(queue_size, GFP_KERNEL);
	if (dom2hsm == NULL)
		goto done;

	hsm2dom = kzalloc(queue_size, GFP_KERNEL);
	if (hsm2dom == NULL)
		goto done;

	dom_msgq->domid = domain;
	dom_msgq->instance = instance;
	dom_msgq->remove = 0;
	dom_msgq->dom2hsm_addr = (hsm_ptr_t) dom2hsm;
	dom_msgq->hsm2dom_addr = (hsm_ptr_t) hsm2dom;
	dom_msgq->dom2hsm_size = queue_size;
	dom_msgq->hsm2dom_size = queue_size;

	if (AmbaHSM_Init_HSM_MsgQueue(dom_msgq) < 0)
		goto done;

	comm = &arm0_comm;
	comm->rx_mq = (struct msgq *) hsm2dom;
	comm->tx_mq = (struct msgq *) dom2hsm;

#if defined(CONFIG_AMALGAM)
	memory_sync_read(comm->tx_mq, sizeof(*comm->tx_mq));
	memory_sync_read(comm->rx_mq, sizeof(*comm->rx_mq));
#endif

	if ((comm->tx_mq->apihash != hsm_api_hash) ||
	    (comm->rx_mq->apihash != hsm_api_hash)) {
		pr_err("Mismatch API struct hash between armhsm driver "
		       "and orchsm: 0x%x != 0x%x\n",
		       hsm_api_hash, comm->tx_mq->apihash);
		comm = NULL;
		goto done;
	}

	show_comm_info(comm);
done:

	if (dom_msgq)
		kfree(dom_msgq);

	return comm;
}
#endif

struct hsm_comm *comm_init_pro(struct hsm_register_nvm_pro *nvm_pro)
{
	struct hsm_comm *comm = NULL;
	struct hsm_dom_msgq *dom_msgq;
	void *pro2hsm, *hsm2pro, *probuf;
	size_t queue_size;

	pr_debug("[armhsm]: %s\n", __func__);

	queue_size = (HSM_MSG_BYTE_SIZE * nvm_pro->num_msgs) +
		sizeof(struct msgq);

	dom_msgq = kzalloc(sizeof(*dom_msgq), GFP_KERNEL);
	if (dom_msgq == NULL)
		goto done;

	pro2hsm = kzalloc(queue_size, GFP_KERNEL);
	if (pro2hsm == NULL)
		goto done;

	hsm2pro = kzalloc(queue_size, GFP_KERNEL);
	if (hsm2pro == NULL)
		goto done;

	probuf = kmalloc(nvm_pro->probuf_size, GFP_KERNEL);
	if (probuf == NULL)
		goto done;

	nvm_pro->probuf_phyaddr = virt_to_phys(probuf);

	dom_msgq->pro2hsm_addr = (hsm_ptr_t) pro2hsm;
	dom_msgq->hsm2pro_addr = (hsm_ptr_t) hsm2pro;
	dom_msgq->pro2hsm_size = queue_size;
	dom_msgq->hsm2pro_size = queue_size;
	dom_msgq->probuf = (hsm_ptr_t) probuf;
	dom_msgq->probuf_size = nvm_pro->probuf_size;

	if (AmbaHSM_Init_HSM_MsgQueue(dom_msgq) < 0) {
		goto done;
	} else {
		hsmnvm_set_state(1);
	}

	comm = &arm0_pro_comm;
	comm->rx_mq = (struct msgq *) hsm2pro;
	comm->tx_mq = (struct msgq *) pro2hsm;

#if defined(CONFIG_AMALGAM)
	memory_sync_read(comm->tx_mq, sizeof(*comm->tx_mq));
	memory_sync_read(comm->rx_mq, sizeof(*comm->rx_mq));
#endif

	if ((comm->tx_mq->apihash != hsm_api_hash) ||
	    (comm->rx_mq->apihash != hsm_api_hash)) {
		pr_err("Mismatch API struct hash between armhsm driver "
		       "and orchsm: 0x%x != 0x%x\n",
		       hsm_api_hash, comm->tx_mq->apihash);
		comm = NULL;
		goto done;
	}

	show_comm_info(comm);

done:

	if (dom_msgq)
		kfree(dom_msgq);

	return comm;
}

int comm_deinit_pro(struct hsm_register_nvm_pro *nvm_pro)
{
	int ret = 0;
	struct hsm_comm *comm;
	struct hsm_dom_msgq *dom_msgq;
	void *pro2hsm, *hsm2pro, *probuf;

	dom_msgq = kzalloc(sizeof(*dom_msgq), GFP_KERNEL);
	if (dom_msgq == NULL) {
		ret = -1;
		goto done;
	}

	probuf = phys_to_virt(nvm_pro->probuf_phyaddr);

	comm = &arm0_pro_comm;
	hsm2pro = comm->rx_mq;
	pro2hsm = comm->tx_mq;

	dom_msgq->remove = 1;
	dom_msgq->pro2hsm_addr = (hsm_ptr_t) pro2hsm;
	dom_msgq->hsm2pro_addr = (hsm_ptr_t) hsm2pro;
	dom_msgq->probuf = (hsm_ptr_t) probuf;

	/*
	 * Set hsmnvm offline if hsmd attempts to unregister NVM.
	 * In other words, HSM nvm request should fail as hsmd
	 * is unregistering NVM.
	 */
	hsmnvm_set_state(0);

	if (AmbaHSM_Init_HSM_MsgQueue(dom_msgq) < 0) {
		ret = -1;
		goto done;
	}

	if (probuf)
		kfree(probuf);

done:

	if (dom_msgq)
		kfree(dom_msgq);

	return ret;
}
