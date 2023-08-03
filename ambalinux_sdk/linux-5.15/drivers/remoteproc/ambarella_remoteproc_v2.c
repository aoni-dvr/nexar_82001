/*
 * Remote processor messaging transport
 *
 *  (C) 2016 Ambarella, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/remoteproc.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/virtio_ids.h>
#include <linux/virtio_config.h>
#include <linux/rpmsg.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>
#include <linux/irqdomain.h>
#include <linux/arm-smccc.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_address.h>
#include <soc/ambarella/ambalink/remoteproc.h>
#include <soc/ambarella/ambalink/ambalink_cfg.h>
#include <soc/ambarella/ambalink/rpmsg_compat.h>
#include <soc/ambarella/iav_helper.h>
#include <asm/cacheflush.h>

#include "remoteproc_internal.h"

//#define DEBUG

#if defined(CONFIG_AMBALINK_SD) || defined(CONFIG_AMBALINK_SD_MODULE)
#include <linux/aipc/rpmsg_sd.h>
/* module memory not mapped, so put memory here to debug when CONFIG_AMBALINK_SD=m */
struct rpdev_sdinfo G_rpdev_sdinfo[3];
EXPORT_SYMBOL(G_rpdev_sdinfo);

struct rpdev_sdresp G_rpdev_sdresp;
EXPORT_SYMBOL(G_rpdev_sdresp);
#endif

#ifdef RPMSG_DEBUG
static AMBA_RPMSG_STATISTIC_s *rpmsg_stat;
extern struct ambalink_shared_memory_layout ambalink_shm_layout;
#endif

/* arm-trusted-firmware/include/lib/psci/psci.h */
#define PSCI_CPU_ON_AARCH64             (0xC4000003u)

/* amba-specific sip */
#define AMBA_SIP_BOOT_RTOS              (0x8200FF04u)
#define S_SWITCH_AARCH32                (2)

/* to compatible with old experimental threadx firmware download */
/* TODO: get from dtb */
#define AHBSP_DATA0                     (0xe0022000 + 0x6c)

/* [start] rtos boot physical address */
static int bootaddr = 0xD00000;
static int ambarella_set_bootaddr(const char *str, const struct kernel_param *kp)
{
	int retval;
	unsigned int value;

	param_set_uint(str, kp);
	retval = kstrtou32(str, 10, &value);

	bootaddr = value;
	printk("0x%x\n", bootaddr);

	return retval;
}
static struct kernel_param_ops param_ops_bootaddr = {
	.set = ambarella_set_bootaddr,
	.get = param_get_int,
};
module_param_cb(bootaddr, &param_ops_bootaddr, &(bootaddr), 0644);
/* [end] rtos boot physical address */

/* [start] rtos firmware path */
static char *firmware = "amba_ssp.bin";
static int ambarella_set_firmware(const char *val, const struct kernel_param *kp)
{
	const struct firmware *fw;
	void *virt;
	char *data = strstrip((char *)val);
	int ret;

	virt = phys_to_virt((phys_addr_t)bootaddr);
	ret = request_firmware_into_buf(&fw, data, NULL, virt, 0xa00000);
	printk("request_firmware(%s) returned %d", data, ret);
#if defined(MODULE)
	ambcache_inv_range(virt, fw->size);
#else
	dcache_clean_inval_poc((unsigned long)virt, fw->size);
#endif
	release_firmware(fw);

	return param_set_charp(data, kp);
}
static struct kernel_param_ops param_ops_firmware = {
	.set = ambarella_set_firmware,
	.get = param_get_charp,
	.free = param_free_charp,
};
module_param_cb(firmware, &param_ops_firmware, &firmware, 0644);
/* [end] rtos firmware path */

/* [start] rtos boot core number */
static int bootcore = 0x1;
static int ambarella_set_bootcore(const char *str, const struct kernel_param *kp)
{
	int retval;
	unsigned int value;
	struct arm_smccc_res res;
	extern void rtos_dummyEntry(void);
	phys_addr_t entry = virt_to_phys(rtos_dummyEntry);
	void __iomem *regs = ioremap(AHBSP_DATA0, 4*4);

	param_set_uint(str, kp);

	retval = kstrtou32(str, 10, &value);
	bootcore = value;

	writel(AMBA_SIP_BOOT_RTOS, regs);
	writel(bootaddr, regs + 4);
	writel(0, regs + 8);
	writel(S_SWITCH_AARCH32, regs + 12);

	iounmap(regs);

	arm_smccc_smc(PSCI_CPU_ON_AARCH64, bootcore, (u64)entry,
			AHBSP_DATA0,
			0, 0, 0, 0, &res);

	printk("PSCI_CPU_ON_AARCH64 0x%llx core %d\n", entry, bootcore);

	return retval;
}
static struct kernel_param_ops param_ops_bootcore = {
	.set = ambarella_set_bootcore,
	.get = param_get_int,
};
module_param_cb(bootcore, &param_ops_bootcore,
	&(bootcore), 0644);
/* [end] rtos boot core number */


struct ambarella_rproc_rsc {
	struct resource_table		rsc_tbl;
	uint32_t			offset;
	struct fw_rsc_hdr		hdr; // type = RSC_VDEV
	struct fw_rsc_vdev		vdev; // here is one vring as tx
	struct fw_rsc_vdev_vring	vring_a;
	struct fw_rsc_vdev_vring	vring_b;
} __packed;

static struct resource_table *ambarella_gen_rsc_table(uint32_t *tablesz,
	struct ambarella_rproc_pdata *pdata)
{
	struct ambarella_rproc_rsc *rsc;

	*tablesz = sizeof(*rsc);
	rsc = kzalloc((*tablesz), GFP_KERNEL);
	if (!rsc) {
		*tablesz = 0;
		return NULL;
	}

	rsc->rsc_tbl.ver = 1;
	rsc->rsc_tbl.num = 1;
	rsc->offset = sizeof(struct resource_table) + sizeof(uint32_t);
	rsc->hdr.type = RSC_VDEV;

	rsc->vdev.id =  VIRTIO_ID_RPMSG;
	rsc->vdev.notifyid = 111;
	rsc->vdev.dfeatures = (1 << VIRTIO_RPMSG_F_NS);
	if (pdata->rpmsg_role)
		rsc->vdev.dfeatures |= (1 << VIRTIO_RPMSG_F_SLV);
	rsc->vdev.config_len = 0;
	rsc->vdev.num_of_vrings = 2;

	// TODO: check pdata->rpmsg_role
	rsc->vring_a.da = FW_RSC_ADDR_ANY;
	rsc->vring_a.align = PAGE_SIZE;
	rsc->vring_a.num = pdata->rpmsg_num / 2;
	rsc->vring_a.notifyid = -1; // allocated by framework

	rsc->vring_b.da = FW_RSC_ADDR_ANY;
	rsc->vring_b.align = PAGE_SIZE;
	rsc->vring_b.num = pdata->rpmsg_num / 2;
	rsc->vring_b.notifyid = -1; // allocated by framework

	return (struct resource_table *)rsc;
}

static void rpmsg_set_irq(struct ambarella_rproc_pdata *pdata, unsigned long irq)
{
	if (pdata) {
		regmap_write_bits(pdata->reg_ahb_scr,
				pdata->sp_swi_set_offset,
				0x1 << (irq - (pdata->sp_swi0)),
				0x1 << (irq - (pdata->sp_swi0)));
	} else {
		/* not possible */
		dump_stack();
	}
}

static void rpmsg_clr_irq(struct ambarella_rproc_pdata *pdata, unsigned long irq)
{
	if (pdata) {
		regmap_write(pdata->reg_ahb_scr,
				pdata->sp_swi_clear_offset,
				0x1 << (irq - (pdata->sp_swi0)));
	} else {
		/* not possible */
		dump_stack();
	}
}

static void ambarella_rproc_kick(struct rproc *rproc, int vqid)
{
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	if (vqid == 0) {
		pr_debug("\e[1;32m ack rvq %d\n \e[0m", pdata->rvq_tx_irq);
		rpmsg_set_irq(pdata, pdata->rvq_tx_irq);
	} else if (vqid == 1) {
		pr_debug("\e[1;32m kick svq %d\n \e[0m", pdata->svq_tx_irq);
		rpmsg_set_irq(pdata, pdata->svq_tx_irq);
	} else {
		pr_debug("\e[1;32m Invalid vqid: %d\n \e[0m", vqid);
	}
}

static void rproc_svq_worker(struct work_struct *work)
{
	struct ambarella_rproc_pdata *pdata;
	struct rproc *rproc;

	pdata = container_of(work, struct ambarella_rproc_pdata, svq_work);
	rproc = pdata->rproc;

	rproc_vq_interrupt(rproc, 1);
}

static void rproc_rvq_worker(struct work_struct *work)
{
	struct ambarella_rproc_pdata *pdata;
	struct rproc *rproc;
	struct rproc_vring *rvring;
	struct irq_data *idata;

	pdata = container_of(work, struct ambarella_rproc_pdata, rvq_work);
	rproc = pdata->rproc;
	rvring = idr_find(&pdata->rproc->notifyids, 0);

	while (1) {
#if 1
		if (rproc_vq_interrupt(rproc, 0) == IRQ_NONE) {
			if (rvring && rvring->vq)
				virtqueue_enable_cb(rvring->vq);
			break;
		}
#else // Orig
		if (rproc_vq_interrupt(rproc, 0) == IRQ_HANDLED)
			continue;

		if (rvring && rvring->vq) {
			virtqueue_enable_cb(rvring->vq);
		}

		if (rproc_vq_interrupt(rproc, 0) == IRQ_HANDLED) {
			if (rvring && rvring->vq) {
				virtqueue_disable_cb(rvring->vq);
			}
			continue;
		}

		break;
#endif
	}

	idata = irq_get_irq_data(pdata->rvq_rx_irq);
	/*re-enable rproc_ambarella_isr*/
	idata->chip->irq_unmask(idata);
}

static void rproc_virtio_worker(struct work_struct *work)
{
	struct ambarella_rproc_pdata *pdata;
	struct rproc *rproc;

	pdata = container_of(work, struct ambarella_rproc_pdata, virtio_work);
	rproc = pdata->rproc;
	if (pdata->rpmsg_role != AMBA_RPMSG_MASTER) {
		// Maybe master is ready before we startup
		struct ambarella_rproc_rsc *rsc = (struct ambarella_rproc_rsc *)rproc->table_ptr;

		if (rsc) {
			if (rsc->vdev.status & VIRTIO_CONFIG_S_DRIVER) {
				rsc->vdev.status |= VIRTIO_CONFIG_S_MASTER_OK;
			} else {
				// Check later
				schedule_work(&pdata->virtio_work);
			}
		} else {
			// Check later
			schedule_work(&pdata->virtio_work);
		}
	}
}

static irqreturn_t rproc_ambarella_isr(int irq, void *dev_id)
{
	struct ambarella_rproc_pdata *pdata = dev_id;
	struct rproc_vring *rvring;
	struct irq_data *idata = NULL;


	if (irq == pdata->rvq_rx_irq) {
		pr_debug("\e[1;32m enter rvq_irq %d\n \e[0m", irq);

		idata = irq_get_irq_data(pdata->rvq_rx_irq);
		/*rproc_rvq_worker re-enable rvq_rx_irq by itself*/
		idata->chip->irq_mask(idata);

		if ((pdata->rpmsg_role == AMBA_RPMSG_MASTER) ||
		   ((pdata->rpmsg_role != AMBA_RPMSG_MASTER) && pdata->virtio_status)) {
			struct rproc *rproc = pdata->rproc;
			struct ambarella_rproc_rsc *rsc = (struct ambarella_rproc_rsc *)rproc->table_ptr;

			if (rsc->vdev.status & VIRTIO_CONFIG_S_DRIVER_OK) {
				rvring = idr_find(&pdata->rproc->notifyids, 0);
				virtqueue_disable_cb(rvring->vq);
				schedule_work(&pdata->rvq_work);
			} else {
				schedule_work(&pdata->virtio_work);
			}
		} else if (pdata->rpmsg_role != AMBA_RPMSG_MASTER) {
			if (!pdata->virtio_status) {
				pdata->virtio_status = 1;
				schedule_work(&pdata->virtio_work);
			} else {
				BUG();
			}
		} else {
			BUG();
		}
	} else if (irq == pdata->svq_rx_irq) {
		pr_debug("\e[1;32m enter svq_rx_irq %d\n \e[0m", irq);
		idata = irq_get_irq_data(pdata->svq_rx_irq);
		schedule_work(&pdata->svq_work);
	} else {
		// TODO
		WARN_ONCE(1, "Master bootup faster than my initialization.\n");
	}

	rpmsg_clr_irq(pdata, idata->hwirq);

	return IRQ_HANDLED;
}

static int ambarella_rproc_start(struct rproc *rproc)
{
	return 0;
}

static int ambarella_rproc_stop(struct rproc *rproc)
{
	return 0;
}

static int ambarella_rproc_dummy_load_segments(struct rproc *rproc,
						const struct firmware *fw)
{
	return 0;
}

// 1 as without cache
#define MAP_WC (1)

/* ignore: remoteproc0#vdev0buffer: vdev 0 buffer already mapped */
static int ambarella_rproc_mem_vring_alloc(struct rproc *rproc,
						struct rproc_mem_entry *mem)
{
	struct device *dev = rproc->dev.parent;
	void *va;

	dev_dbg(dev, "map memory: 0x%llx + 0x%lx\n", mem->dma, mem->len);
#if MAP_WC
	va = ioremap_wc(mem->dma, mem->len);
#else
	/* NOTE: attribution of cache setting should be matched to bsp */
//	va = ioremap(mem->dma, mem->len);
	va = __ioremap(mem->dma, mem->len, __pgprot(PROT_NORMAL));
#endif
	if (IS_ERR_OR_NULL(va)) {
		dev_err(dev, "Unable to map memory region: 0x%llx + 0x%lx\n",
			mem->dma, mem->len);
		return -ENOMEM;
	} else {
		dev_info(dev, "(0x%llx + 0x%lx) map to VA: 0x%lx\n",
			mem->dma, mem->len, (uintptr_t)va);
	}

	/* Update memory entry va */
	mem->va = va;

	return 0;
}

static int ambarella_rproc_mem_vring_release(struct rproc *rproc,
				   struct rproc_mem_entry *mem)
{
	dev_dbg(rproc->dev.parent, "unmap memory: 0x%llx\n", mem->dma);
	iounmap(mem->va);

	return 0;
}

#if !MAP_WC
static int ambarella_rproc_mem_vbuf_alloc(struct rproc *rproc,
						struct rproc_mem_entry *mem)
{
	struct device *dev = rproc->dev.parent;
	void *va;

	dev_dbg(dev, "map memory: 0x%llx + 0x%lx\n", mem->dma, mem->len);
#if MAP_WC
	va = ioremap_wc(mem->dma, mem->len);
#else
	/* NOTE: attribution of cache setting should be matched to bsp */
//	va = ioremap(mem->dma, mem->len);
	va = __ioremap(mem->dma, mem->len, __pgprot(PROT_NORMAL));
#endif
	if (IS_ERR_OR_NULL(va)) {
		dev_err(dev, "Unable to map memory region: 0x%llx + 0x%lx\n",
			mem->dma, mem->len);
		return -ENOMEM;
	} else {
		dev_info(dev, "(0x%llx + 0x%lx) map to VA: 0x%lx\n",
			mem->dma, mem->len, (uintptr_t)va);
	}

	/* Update memory entry va */
	mem->va = va;

	return 0;
}

static int ambarella_rproc_mem_vbuf_release(struct rproc *rproc,
				   struct rproc_mem_entry *mem)
{
	dev_dbg(rproc->dev.parent, "unmap memory: 0x%llx\n", mem->dma);
	iounmap(mem->va);

	return 0;
}
#endif // !MAP_WC

static int ambarella_rproc_dummy_find_rsc_table(struct rproc *rproc, const struct firmware *fw)
{
	uint32_t tablesz;
	struct ambarella_rproc_pdata *pdata = rproc->priv;
	struct resource_table *table;

	table = ambarella_gen_rsc_table(&tablesz, pdata);
	if (!table) {
		dev_err(&(rproc->dev), "No memory for resource table\n");
		return -ENOMEM;
	}

	rproc->table_sz = tablesz;
	rproc->cached_table = kmemdup(table, tablesz, GFP_KERNEL);
	rproc->table_ptr = rproc->cached_table;

	/* Set memory */
	{
		struct device *dev = rproc->dev.parent;
		struct device_node *np = dev->of_node;
		struct of_phandle_iterator it;
		struct rproc_mem_entry *mem;
		struct reserved_mem *rmem;
		int index = 0;

		/* Register associated reserved memory regions */
		of_phandle_iterator_init(&it, np, "memory-region", NULL, 0);
		while (of_phandle_iterator_next(&it) == 0) {
			rmem = of_reserved_mem_lookup(it.node);
			if (!rmem) {
				dev_err(dev, "unable to acquire memory-region\n");
				return -EINVAL;
			}

			/*  No need to map vdev buffer but vring */
			if (strcmp(it.node->name, "vdev0buffer")) {
				/* Register memory region */
				mem = rproc_mem_entry_init(dev, NULL,
						(dma_addr_t)rmem->base,
						rmem->size, rmem->base,
						ambarella_rproc_mem_vring_alloc,
						ambarella_rproc_mem_vring_release,
						it.node->name);

				if (mem)
					rproc_coredump_add_segment(rproc, rmem->base,
							rmem->size);
			} else {
				/* rproc_find_carveout_by_name() will map to non-cached */
				pdata->virtio_buf = rmem->base;

#if MAP_WC // alloc by framework w/o cache
				/* Register reserved memory for vdev buffer alloc */
				mem = rproc_of_resm_mem_entry_init(dev, index,
						rmem->size,
						rmem->base,
						it.node->name);
#else // alloc by ourselves
				mem = rproc_mem_entry_init(dev, NULL,
						(dma_addr_t)rmem->base,
						rmem->size, rmem->base,
						ambarella_rproc_mem_vbuf_alloc,
						ambarella_rproc_mem_vbuf_release,
						it.node->name);

				if (mem)
					rproc_coredump_add_segment(rproc, rmem->base,
							rmem->size);
#endif
			}

			if (!mem)
				return -ENOMEM;

			rproc_add_carveout(rproc, mem);
			index++;
		}
	}


	return 0;
}

static struct rproc_ops ambarella_rproc_ops = {
	.start			= ambarella_rproc_start,
	.stop			= ambarella_rproc_stop,
	.kick			= ambarella_rproc_kick,
	.load			= ambarella_rproc_dummy_load_segments, // No firmware to be loaded.
	.find_loaded_rsc_table	= NULL, // load() also load resource table
	.parse_fw		= ambarella_rproc_dummy_find_rsc_table,
	.sanity_check		= NULL,
	.get_boot_addr		= NULL
};


static const struct of_device_id ambarella_rproc_of_match[] = {
#if 0
	{ .compatible = "ambarella,rproc", .data = &pdata_cortex, },
	{ .compatible = "c1_and_c2,rproc", .data = &pdata2_cortex, },
#else
	{ .compatible = "ambarella,rproc", },
#endif
	{},
};

static const char dummy_firmware[] = "ambadummy";

int ambarella_get_rpmsg_size(struct virtio_device *vdev)
{
	struct rproc_vdev *rvdev = vdev_to_rvdev(vdev);
	struct rproc *rproc = rvdev->rproc;
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	return pdata->rpmsg_size;
}
EXPORT_SYMBOL(ambarella_get_rpmsg_size);

int ambarella_get_rpmsg_num(struct virtio_device *vdev)
{
	struct rproc_vdev *rvdev = vdev_to_rvdev(vdev);
	struct rproc *rproc = rvdev->rproc;
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	return pdata->rpmsg_num;
}
EXPORT_SYMBOL(ambarella_get_rpmsg_num);

phys_addr_t ambarella_get_virtio_base(struct virtio_device *vdev)
{
	struct rproc_vdev *rvdev = vdev_to_rvdev(vdev);
	struct rproc *rproc = rvdev->rproc;
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	return pdata->virtio_buf;
}
EXPORT_SYMBOL(ambarella_get_virtio_base);

static int ambarella_rproc_probe(struct platform_device *pdev)
{
	int ret;
	struct device *dev = &pdev->dev;
	struct ambarella_rproc_pdata *pdata;
	uint32_t val;
	struct rproc *rproc;

	ret = dma_set_coherent_mask(dev, DMA_BIT_MASK(64));
	if (ret) {
		dev_err(pdev->dev.parent, "dma_set_coherent_mask: %d\n", ret);
		return ret;
	}

	rproc = rproc_alloc(dev, dev->of_node->name, &ambarella_rproc_ops,
			    "ambadummy", sizeof(*pdata));
	if (!rproc) {
		ret = -ENOMEM;
		goto probe_fail;
	}
	dev_info(dev, "%s device registered\n", __func__);
	rproc->dev.parent = dev;

	pdata = rproc->priv;
	pdata->rproc = rproc;
	rproc->has_iommu = false;
	mutex_init(&rproc->lock);
	platform_set_drvdata(pdev, rproc);

	/*
	 * When we are rpmsg master:
	 * irq N.   amb,rvq_ack  = rvq_tx_irq: master ack  rvq -> slave  svq (notify host online)
	 * irq N-1. amb,svq_kick = svq_tx_irq: master kick svq -> slave  rvq (TX, can be the same as rvq_tx_irq)
	 * irq N-2.                svq_rx_irq: slave  ack  rvq -> master svq (not used, platform_get_irq 1)
	 * irq N-3.                rvq_rx_irq: slave  kick svq -> master rvq (RX,       platform_get_irq 0)
	 */
	ret = of_property_read_u32(dev->of_node, "amb,svq_kick", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,svq_kick: %d\n", ret);
		goto probe_fail;
	}
	pdata->svq_tx_irq = val;

	ret = of_property_read_u32(dev->of_node, "amb,rvq_ack", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,rvq_ack: %d\n", ret);
		goto probe_fail;
	}
	pdata->rvq_tx_irq = val;

	/* Get RPMSG paremeters */
	ret = of_property_read_u32(dev->of_node, "amb,rpmsg_num", &val);
	if (ret < 0) {
		val = MAX_RPMSG_NUM_BUFS;
		dev_err(pdev->dev.parent, "amb,rpmsg_num not specified, use %d\n", val);
	}
	pdata->rpmsg_num = val;
	ret = of_property_read_u32(dev->of_node, "amb,rpmsg_size", &val);
	if (ret < 0) {
		val = RPMSG_BUF_SIZE;
		dev_err(pdev->dev.parent, "amb,rpmsg_size not specified, use %d\n", val);
	}
	pdata->rpmsg_size = val;
	ret = of_property_read_u32(dev->of_node, "amb,rpmsg_role", &val);
	if (ret < 0) {
		/* master(0) or slave(1) */
		val = AMBA_RPMSG_MASTER;
		dev_err(pdev->dev.parent, "amb,rpmsg_role not specified, use %d\n", val);
	}
	pdata->rpmsg_role = val;
	if (pdata->rpmsg_role != AMBA_RPMSG_MASTER) {
		pr_debug("RPMSG: Slave/Remote mode\n");
	} else {
		pr_debug("RPMSG: Master mode\n");
	}

	/* set scratchpad sw interrupt   */
	ret = of_property_read_u32(dev->of_node, "amb,scr-swiset", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,scr-swiset\n");
		goto probe_fail;
	}
	pr_debug("sp_swi_set_offset 0x%x\n", val);
	pdata->sp_swi_set_offset = val;

	/* clear scratchpad sw interrupt */
	ret = of_property_read_u32(dev->of_node, "amb,scr-swiclear", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get amb,scr-swiset\n");
		goto probe_fail;
	}
	pr_debug("sp_swi_clear_offset 0x%x\n", val);
	pdata->sp_swi_clear_offset = val;

	pdata->reg_ahb_scr = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
			"amb,scr-regmap");
	if (IS_ERR(pdata->reg_ahb_scr)) {
		printk(KERN_ERR "no scr regmap!\n");
		ret = PTR_ERR(pdata->reg_ahb_scr);
		goto probe_fail;
	}

	ret = of_property_read_u32(dev->of_node, "amb,scr-swi0", &val);
	if (ret < 0) {
		dev_err(pdev->dev.parent, "Can not get scr-swi0\n");
		goto probe_fail;
	}
	pr_debug("sp_swi0 0x%x\n", val);
	pdata->sp_swi0 = val;

	INIT_WORK(&pdata->svq_work, rproc_svq_worker);
	INIT_WORK(&pdata->rvq_work, rproc_rvq_worker);
	INIT_WORK(&pdata->virtio_work, rproc_virtio_worker);

	/*
	 * request_irq at final stage. Master or Remote would boot up earily than us.
	 */
	/* rvq_rx_irq */
	val = platform_get_irq(pdev, 0);
	if (val< 0) {
		dev_err(dev, "No IRQ for rvq: %d\n", ret);
		goto probe_fail;
	}
	pdata->rvq_rx_irq = val;
	pr_debug("virtual rvq_rx_irq %d\n", pdata->rvq_rx_irq);
	ret = request_irq(val, rproc_ambarella_isr,
			IRQF_SHARED | IRQF_TRIGGER_HIGH,
			"rvq_RX", pdata);

	if (ret) {
		dev_err(&pdev->dev, "request_irq error: %d\n", ret);
		goto probe_fail;
	}

	/* No need for svq_rx_irq (svq_ack) */

	//rproc->fw_ops = &rproc_dummy_fw_ops;
	ret = rproc_add(rproc);
	if (ret)
		goto probe_fail;

	/* Should have only one virtio */
        BUG_ON(rproc->nb_vdev != 1);

	if (pdata->rpmsg_role != AMBA_RPMSG_MASTER) {
		// Maybe master is ready before we startup
		if (pdata->virtio_status) {
//			if (pdata->rvq_rx_irq == pdata->svq_rx_irq)
			schedule_work(&pdata->rvq_work);

			// Create devices of application
			{
				struct list_head *node = rproc->rvdevs.next;
				struct rproc_vdev *rvdev = container_of(node, struct rproc_vdev, node);
				struct device *dev;

				/* Should have only one virtio */
				dev = device_find_child_by_name(&rvdev->dev, "virtio0");
				if (dev) {
					struct rpmsg_device * rpmsg_create_new_device(struct virtio_device *vdev, const char *name);

					struct virtio_device *vdev = container_of(dev, struct virtio_device, dev);
					struct device_node *child;
					const char *name;

					for_each_available_child_of_node(pdev->dev.of_node, child) {
						ret = of_property_read_string(child, "amb,rpmsg_id", &name);
						if (ret < 0)
							continue;

						dev_info(dev, "Create app device named as %s\n", name);
						rpmsg_create_new_device(vdev, name);
					}
				} else {
					pr_err("%s Can not found VirtIO device\n", __func__);
				}

			}
		}
	}

	return 0;

probe_fail:
	devm_kfree(dev, pdata);

	return ret;
}

static int ambarella_rproc_remove(struct platform_device *pdev)
{
#if 0 // TODO
	struct rproc *rproc = platform_get_drvdata(pdev);
	struct ambarella_rproc_pdata *pdata = rproc->priv;

	//disable_irq(pdata->svq_rx_irq); //not used
	disable_irq(pdata->rvq_rx_irq);
	//free_irq(pdata->svq_rx_irq, pdata); //not used
	free_irq(pdata->rvq_rx_irq, pdata);

	/* TODO: release memory for table */

	rproc_del(rproc);
	rproc_free(rproc);
#endif // TODO

	return 0;
}

static struct platform_driver ambarella_rproc_driver = {
	.probe = ambarella_rproc_probe,
	.remove = ambarella_rproc_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "ambarella-rproc",
		.of_match_table = ambarella_rproc_of_match,
	},
};

int __init ambarella_rproc_init(void)
{
	return platform_driver_register(&ambarella_rproc_driver);
}

void __exit ambarella_rproc_exit(void)
{
	// TODO: release resource table ,remove rproc

	platform_driver_unregister(&ambarella_rproc_driver);
}

device_initcall(ambarella_rproc_init);
module_exit(ambarella_rproc_exit);
//module_platform_driver(ambarella_rproc_driver);

MODULE_DESCRIPTION("Ambarella Remote Processor control driver");
MODULE_LICENSE("GPL");
