// SPDX-License-Identifier: GPL-2.0
/*
 * Platform driver for the Cadence uDMA Controller
 *
 * Copyright (C) 2022 Ambarella.Inc
 * Author: Li Chen <lchen@ambarella.com>
 */

#include <linux/dmaengine.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/bits.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/overflow.h>
#include <linux/dmapool.h>
#include <linux/of.h>
#include <linux/bitfield.h>
#include <linux/dma-mapping.h>
#include <uapi/linux/types.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include "virt-dma.h"

/*
 * TODO:
 * - cdns_udma ff38600000.pcie2-udma: WARN: Device release
 *   is not defined so it is not safe to unbind this driver while in use
 * - implement debugfs support
 * - implement scatter mode and gather mode
 */

#define CONTINUE_TO_EXECUTE_LINKED_LIST 1
#define DONT_CONTINUE_TO_EXECUTE_LINKED_LIST 0

#define CDNS_UDMA_MAX_CHANNELS 8

#define DMA_OB_CMD 0x3
#define DMA_IB_CMD 0x1

/* Length of xfer in bytes(0 indicates maximum length xfer 2 ^ 24 bytes) */
#define REG_FIELD_LENGTH_MASK GENMASK(23, 0)
#define REG_FIELD_CONTROL_MASK GENMASK(31, 24) /* Control Byte */

#define CHANNEL_CTRL_OFFSET(id) (0x0 + 0x14 * id)
#define CHANNEL_SP_L_OFFSET(id) (0x4 + 0x14 * id)
#define CHANNEL_SP_U_OFFSET(id) (0x8 + 0x14 * id)
#define CHANNEL_ATTR_L_OFFSET(id) (0xc + 0x14 * id)
#define CHANNEL_ATTR_U_OFFSET(id) (0x10 + 0x14 * id)
#define COMMON_UDMA_INT_OFFSET 0xa0
#define COMMON_UDMA_INT_ENA_OFFSET 0xa4
#define COMMON_UDMA_INT_DIS_OFFSET 0xa8

#define COMMON_UDMA_IB_ECC_UNCORRECTABLE_ERRORS_OFFSET 0xac
#define COMMON_UDMA_IB_ECC_CORRECTABLE_ERRORS_OFFSET 0xb0
#define COMMON_UDMA_IB_ECC_UNCORRECTABLE_ERRORS_MASK GENMASK(15, 0)
#define COMMON_UDMA_IB_ECC_CORRECTABLE_ERRORS_MASK GENMASK(15, 0)

#define COMMON_UDMA_OB_ECC_UNCORRECTABLE_ERRORS_OFFSET 0xb4
#define COMMON_UDMA_OB_ECC_CORRECTABLE_ERRORS_OFFSET 0xb8
#define COMMON_UDMA_OB_ECC_UNCORRECTABLE_ERRORS_MASK GENMASK(15, 0)
#define COMMON_UDMA_OB_ECC_CORRECTABLE_ERRORS_MASK GENMASK(15, 0)

#define CTRL_BYTE_INT_MASK BIT(0)
#define CTRL_BYTE_CONTINUITY_MASK GENMASK(2, 1) /* R/W or Prefetch or Write */
#define CTRL_BYTE_CONTINUE_MASK BIT(5)

#define COMMON_UDMA_CONFIG_OFFSET 0xfc
#define COMMON_UDMA_CAP_VER_MIN_VER_MASK GENMASK(7, 0)
#define COMMON_UDMA_CAP_VER_MAJ_VER_MASK GENMASK(15, 8)

#define COMMON_UDMA_CAP_VER_OFFSET 0xf8
#define COMMON_UDMA_CONFIG_NUM_CHANNELS_MASK GENMASK(3, 0)
#define COMMON_UDMA_CONFIG_NUM_PARTITIONS_MASK GENMASK(7, 4)
#define COMMON_UDMA_CONFIG_PARTITIONS_SIZE_MASK GENMASK(11, 8)
#define COMMON_UDMA_CONFIG_SYS_AW_GT_32_MASK BIT(12)
#define COMMON_UDMA_CONFIG_SYS_TW_GT_32_MASK BIT(13)
#define COMMON_UDMA_CONFIG_EXT_AW_GT_32_MASK BIT(14)
#define COMMON_UDMA_CONFIG_EXT_TW_GT_32_MASK BIT(15)

#define BULK_XFER_MAX_SIZE_PER_DESC SZ_16M

enum cdns_udma_dir {
	OUTBOUND,
	INBOUND,
};

struct pcie_status {
	u8 sys_status; /* System (local) bus status */
	u8 ext_status; /* External (remote) bus status */
	u8 chnl_status; /* uDMA channel status */
	u8 reserved_0; /** Reserved */
};

/*
 * The pcie_master_AXI_AR/WSIZE variation from its max value of (4)
 * is not allowed when pcie_master_AXI_ARLEN is not zero in a request.
 *
 * So use 32 instead of 64 bytes variable here.
 *
 */
struct cdns_udma_lli {
	__le32 sys_lo_addr; /* local-axi-addr */
	__le32 sys_hi_addr;
	__le32 sys_attr;

	__le32 ext_lo_addr; /* ext-pci-bus-addr */
	__le32 ext_hi_addr;
	__le32 ext_attr;

	__le32 size_and_ctrl_bits;
	struct pcie_status status;
	__le32 next;
	__le32 next_hi_addr;
};

struct cdns_udma_desc_node {
	struct cdns_udma_lli *lli;
	dma_addr_t lli_dma_addr;
};

struct cdns_desc {
	struct virt_dma_desc vd;
	unsigned int count;
	enum cdns_udma_dir dir;
	struct cdns_udma_desc_node node[];
};

struct cdns_udma_chan {
	struct virt_dma_chan vc;
	struct cdns_desc *desc;
	struct dma_pool *desc_pool;
	struct cdns_udma_dev *udma_dev;
	u8 idx;
	struct dma_slave_config config;
	dma_addr_t local_addr;
	dma_addr_t external_addr;
	struct tasklet_struct irqtask;
};

struct cdns_udma_dev {
	struct device *dev;
	struct dma_device dma_dev;
	u32 chan_num;
	int irq;
	void __iomem *dma_base;
	struct cdns_udma_chan chan[];
};

static inline struct cdns_udma_chan *to_cdns_udma_chan(struct dma_chan *c)
{
	return container_of(c, struct cdns_udma_chan, vc.chan);
}

static struct cdns_desc *to_cdns_udma_desc(struct virt_dma_desc *vd)
{
	return container_of(vd, struct cdns_desc, vd);
}

static inline struct cdns_desc *to_cdns_desc(struct virt_dma_desc *vd)
{
	return container_of(vd, struct cdns_desc, vd);
}

static int cdns_udma_unmask_irq(struct cdns_udma_chan *chan)
{
	unsigned long stat = 0;

	if (chan->idx > chan->udma_dev->chan_num) {
		dev_err(chan->udma_dev->dev, "invalid chan idx %d\n",
			chan->idx);
		return -EINVAL;
	}

	stat = readl(chan->udma_dev->dma_base + COMMON_UDMA_INT_ENA_OFFSET);
	set_bit(chan->idx, &stat);
	writel(stat, chan->udma_dev->dma_base + COMMON_UDMA_INT_ENA_OFFSET);
	return 0;
}

enum pcie_udma_operation {
	PCIE_READ_WRITE = 0,
	PCIE_PREFETCH = 1,
	PCIE_POSTWRITE = 2,
};

static u8 set_contro_byte(enum pcie_udma_operation operation,
			  bool interrupt_value, bool continue_on_value)
{
	return FIELD_PREP(CTRL_BYTE_INT_MASK, interrupt_value) |
	       FIELD_PREP(CTRL_BYTE_CONTINUITY_MASK, operation) |
	       FIELD_PREP(CTRL_BYTE_CONTINUE_MASK, continue_on_value);
}

static bool is_buswidth_valid(u8 buswidth)
{
	if (buswidth != DMA_SLAVE_BUSWIDTH_4_BYTES &&
	    buswidth != DMA_SLAVE_BUSWIDTH_8_BYTES)
		return false;
	else
		return true;
}

static int cdns_udma_device_config(struct dma_chan *dma_chan,
				   struct dma_slave_config *config)
{
	struct cdns_udma_chan *chan = to_cdns_udma_chan(dma_chan);

	/* TODO: is aligned needed? like IS_ALIGNED(cfg->src_addr, cfg->src_addr_width) */

	/* Reject definitely invalid configurations */
	if (!is_buswidth_valid(config->src_addr_width) ||
	    !is_buswidth_valid(config->dst_addr_width))
		return -EINVAL;

	memcpy(&chan->config, config, sizeof(*config));
	return 0;
}

static void cdns_udma_free_desc(struct cdns_udma_chan *chan,
				struct cdns_desc *desc)
{
	int i = 0;

	for (i = 0; i < desc->count; i++)
		dma_pool_free(chan->desc_pool, desc->node[i].lli,
			      desc->node[i].lli_dma_addr);

	kfree(desc);
}

static void cdns_udma_vchan_free_desc(struct virt_dma_desc *vd)
{
	cdns_udma_free_desc(to_cdns_udma_chan(vd->tx.chan),
			    to_cdns_udma_desc(vd));
}

static struct cdns_desc *cdns_udma_alloc_desc(struct cdns_udma_chan *chan,
					      u32 count)
{
	struct cdns_desc *desc;
	int i;
	struct device *dev = chan->udma_dev->dev;

	desc = kzalloc(struct_size(desc, node, count), GFP_NOWAIT);
	if (!desc)
		return NULL;

	for (i = 0; i < count; i++) {
		desc->node[i].lli = dma_pool_alloc(chan->desc_pool, GFP_NOWAIT,
						   &desc->node[i].lli_dma_addr);
		if (!desc->node[i].lli)
			goto err;
	}

	desc->count = count;

	return desc;

err:
	dev_err(dev, "Failed to allocate descriptor\n");
	while (--i >= 0)
		dma_pool_free(chan->desc_pool, desc->node[i].lli,
			      desc->node[i].lli_dma_addr);
	kfree(desc);
	return NULL;
}

/*
 * TODO: currently only implemented bulk mode, we should also
 * implement scatter mdoe and gather mode.
 */
static int cdns_udma_setup_bulk_lli(struct cdns_udma_chan *chan,
				    struct cdns_desc *desc, u32 index,
				    dma_addr_t external_addr,
				    dma_addr_t local_addr, u32 len,
				    bool is_last)

{
	struct cdns_udma_lli *lli;
	u32 next = index + 1;
	u8 ctrl_bits;

	if (len > BULK_XFER_MAX_SIZE_PER_DESC) {
		dev_err(chan->udma_dev->dev,
			"%s: invalid xfer size %x for bulk mode\n", __func__,
			len);
		return -EINVAL;
	}
	if (len == 0)
		dev_warn(chan->udma_dev->dev,
			 "Note: len is 0, uDMA will xfer max size: %x",
			 BULK_XFER_MAX_SIZE_PER_DESC);

	lli = desc->node[index].lli;

	lli->sys_lo_addr = lower_32_bits(local_addr);
	lli->sys_hi_addr = upper_32_bits(local_addr);

	lli->ext_lo_addr = lower_32_bits(external_addr);
	lli->ext_hi_addr = upper_32_bits(external_addr);

	if (is_last) {
		ctrl_bits =
			set_contro_byte(PCIE_READ_WRITE, true,
					DONT_CONTINUE_TO_EXECUTE_LINKED_LIST);
		lli->next = 0;
		lli->next_hi_addr = 0;
	} else {
		ctrl_bits = set_contro_byte(PCIE_READ_WRITE, false,
					    CONTINUE_TO_EXECUTE_LINKED_LIST);
		lli->next = lower_32_bits(desc->node[next].lli_dma_addr);
		lli->next_hi_addr =
			upper_32_bits(desc->node[next].lli_dma_addr);
	}
	lli->size_and_ctrl_bits = FIELD_PREP(REG_FIELD_LENGTH_MASK, len) |
				  FIELD_PREP(REG_FIELD_CONTROL_MASK, ctrl_bits);

	return 0;
}

static struct dma_async_tx_descriptor *
cdns_udma_prep_slave_sg(struct dma_chan *dma_chan, struct scatterlist *sgl,
			unsigned int sg_len,
			enum dma_transfer_direction direction,
			unsigned long flags, void *context)
{
	struct cdns_udma_chan *chan = to_cdns_udma_chan(dma_chan);
	struct cdns_udma_dev *udma_dev = chan->udma_dev;
	struct device *dev = udma_dev->dev;
	struct cdns_desc *desc;
	struct scatterlist *sg;
	int i, ret;
	dma_addr_t local_addr, external_addr;

	if (chan->config.direction != direction) {
		dev_err(dev, "%s error, mismatch with sconf dir\n", __func__);
		return NULL;
	}

	desc = cdns_udma_alloc_desc(chan, sg_len);
	if (!desc)
		return NULL;
	chan->desc = desc;

	desc->dir = direction == DMA_DEV_TO_MEM ? INBOUND : OUTBOUND;

	for_each_sg(sgl, sg, sg_len, i) {
		if (direction == DMA_DEV_TO_MEM) {
			external_addr = chan->config.src_addr;
			local_addr = sg_dma_address(sg);
		} else {
			local_addr = sg_dma_address(sg);
			external_addr = chan->config.dst_addr;
		}

		ret = cdns_udma_setup_bulk_lli(chan, desc, i, external_addr,
					       local_addr, sg_dma_len(sg),
					       i == sg_len - 1);
		if (ret) {
			cdns_udma_free_desc(chan, desc);
			return NULL;
		}
	}

	if (cdns_udma_unmask_irq(chan) < 0) {
		cdns_udma_free_desc(chan, desc);
		return NULL;
	}

	return vchan_tx_prep(&chan->vc, &desc->vd, flags);
}

static void cdns_udma_start_transfer(struct cdns_udma_chan *chan)
{
	struct cdns_udma_dev *udma_dev = chan->udma_dev;
	struct cdns_desc *desc;
	struct virt_dma_desc *vd = vchan_next_desc(&chan->vc);
	struct cdns_udma_lli *lli;

	if (!vd)
		return;

	list_del(&vd->node);
	desc = to_cdns_desc(vd);

	lli = desc->node[0].lli;

	dev_dbg(udma_dev->dev, "%s %d, chan->idx is %x\n", __func__, __LINE__,
		chan->idx);

	if (chan->idx > udma_dev->chan_num) {
		dev_err(udma_dev->dev, "invalid channel index: %x!\n",
			chan->idx);
		return;
	}

	/* Clear attr */
	writel(0, &lli->sys_attr);
	writel(0, &lli->ext_attr);

	/* Clear status */
	writeb(0, &lli->status.sys_status);
	writeb(0, &lli->status.ext_status);
	writeb(0, &lli->status.chnl_status);

	/* Set up starting descriptor */
	writel((u32)(lower_32_bits(desc->node[0].lli_dma_addr)),
	       udma_dev->dma_base + CHANNEL_SP_L_OFFSET(chan->idx));
	writel((u32)(upper_32_bits(desc->node[0].lli_dma_addr)),
	       udma_dev->dma_base + CHANNEL_SP_U_OFFSET(chan->idx));

	/* Clear channel attr */
	writel(0, udma_dev->dma_base + CHANNEL_ATTR_L_OFFSET(chan->idx));
	writel(0, udma_dev->dma_base + CHANNEL_ATTR_U_OFFSET(chan->idx));

	/* let's go */
	if (desc->dir == OUTBOUND)
		writel(DMA_OB_CMD,
		       udma_dev->dma_base + CHANNEL_CTRL_OFFSET(chan->idx));
	else
		writel(DMA_IB_CMD,
		       udma_dev->dma_base + CHANNEL_CTRL_OFFSET(chan->idx));
}

/*
 * WOCLR: write bit id to clear interrupt status,
 * otherwise will always get triggered
 */

static void cdns_udma_clear_irq(int id, void *base)
{
	unsigned long val;

	val = readl(base + COMMON_UDMA_INT_OFFSET);
	set_bit(id, &val);
	writel(val, base + COMMON_UDMA_INT_OFFSET);
}
static void cdns_udma_issue_pending(struct dma_chan *c)
{
	struct cdns_udma_chan *chan = to_cdns_udma_chan(c);
	unsigned long flags;

	spin_lock_irqsave(&chan->vc.lock, flags);

	if (vchan_issue_pending(&chan->vc))
		cdns_udma_start_transfer(chan);

	spin_unlock_irqrestore(&chan->vc.lock, flags);
}

static int cdns_udma_dump_error(struct cdns_desc *desc,
				struct cdns_udma_dev *udma_dev,
				struct cdns_udma_chan *chan, int id)
{
	int i;
	struct cdns_udma_lli *lli;
	bool has_error = false;

	/* TODO: translate error status code to message */
	for (i = 0; i < desc->count; i++) {
		lli = chan->desc->node[i].lli;
		if (lli->status.chnl_status != 1) {
			dev_err(udma_dev->dev,
				"channel %x lli %x channel status error: %x\n",
				id, i, lli->status.chnl_status);
			has_error = true;
		}

		if (lli->status.ext_status != 0) {
			dev_err(udma_dev->dev,
				"channel %x lli %x PCIe Bus Status error: %x\n",
				id, i, lli->status.ext_status);
			has_error = true;
		}
		if (lli->status.sys_status != 0) {
			dev_err(udma_dev->dev,
				"channel %x lli %x Local Bus Status error: %x\n",
				id, i, lli->status.sys_status);
			has_error = true;
		}
	}
	if (has_error)
		return -1;
	return 0;
}
static irqreturn_t cdns_udma_irq(int irq, void *data)
{
	u32 id, common_udma_int;
	struct cdns_udma_dev *udma_dev = data;
	unsigned long flags;
	struct cdns_udma_chan *chan;
	struct cdns_desc *desc;

	common_udma_int = readl(udma_dev->dma_base + COMMON_UDMA_INT_OFFSET);
	if (hweight32(common_udma_int) != 1) {
		dev_err(udma_dev->dev,
			"invalid interrupts mask, pls check if there was unhandled irq\n");
		return IRQ_NONE;
	}

	/* Find out which channel trigger DONE or ERROR interrupt */
	id = __ffs(common_udma_int);

	/* Get Error interrupt */
	if (id >= CDNS_UDMA_MAX_CHANNELS) {
		/* 0..7 are done interrupts, 7-13 are error interrupts */
		chan = &udma_dev->chan[id - CDNS_UDMA_MAX_CHANNELS];
		desc = chan->desc;

		cdns_udma_clear_irq(id, udma_dev->dma_base);
		cdns_udma_dump_error(desc, udma_dev, chan, id);
		return IRQ_NONE;
	}
	chan = &udma_dev->chan[id];

	cdns_udma_clear_irq(id, udma_dev->dma_base);

	if (!chan) {
		dev_err(udma_dev->dev, "uDMA channel not initialized\n");
		return IRQ_NONE;
	}
	desc = chan->desc;
	if (!desc) {
		dev_err(udma_dev->dev, "uDMA channel desc not initialized\n");
		return IRQ_NONE;
	}

	spin_lock_irqsave(&chan->vc.lock, flags);
	cdns_udma_dump_error(desc, udma_dev, chan, id);
	spin_unlock_irqrestore(&chan->vc.lock, flags);
	tasklet_schedule(&chan->irqtask);
	return IRQ_HANDLED;
}

static void cdns_udma_task(struct tasklet_struct *task)
{
	struct cdns_udma_chan *chan = from_tasklet(chan, task, irqtask);
	struct cdns_desc *desc;

	desc = chan->desc;

	/* invoke callback for epf completion, may sleep */
	dmaengine_desc_get_callback_invoke(&desc->vd.tx, NULL);
	vchan_cookie_complete(&chan->desc->vd);
}

/* FIXME: impelement correct tx_status */
static enum dma_status cdns_udma_tx_status(struct dma_chan *c,
					   dma_cookie_t cookie,
					   struct dma_tx_state *txstate)
{
	return dma_cookie_status(c, cookie, txstate);
}

/* This function enables all hw channels in a device */
static int cdns_udma_enable_hw_channels(struct cdns_udma_dev *udma_dev)
{
	int ret;
	int i;

	ret = devm_request_irq(udma_dev->dev, udma_dev->irq, cdns_udma_irq,
			       IRQF_ONESHOT, "cdns,udma", udma_dev);
	if (ret) {
		dev_err(udma_dev->dev,
			"fail to request irq for udma channel!\n");
		return ret;
	}

	for (i = 0; i < udma_dev->chan_num; i++) {
		udma_dev->chan[i].idx = i;
		udma_dev->chan[i].udma_dev = udma_dev;
		udma_dev->chan[i].vc.desc_free = cdns_udma_vchan_free_desc;
		vchan_init(&udma_dev->chan[i].vc, &udma_dev->dma_dev);
	}

	return 0;
}

static int cdns_udma_alloc_chan_resources(struct dma_chan *dma_chan)
{
	struct cdns_udma_chan *chan = to_cdns_udma_chan(dma_chan);
	int ret = 0;

	chan->desc_pool =
		dmam_pool_create(dev_name(chan->udma_dev->dev),
				 chan->udma_dev->dev,
				 sizeof(struct cdns_udma_lli),
				 __alignof__(struct cdns_udma_lli), 0);
	if (!chan->desc_pool) {
		dev_err(chan->udma_dev->dev,
			"failed to allocate descriptor pool\n");
		return -ENOMEM;
	}

	tasklet_setup(&chan->irqtask, cdns_udma_task);

	/*  TODO: disable irq here
	 * ret = cdns_udma_disable_chan(chan);
	 */

	return ret;
}

static void cdns_udma_free_chan_resources(struct dma_chan *dma_chan)
{
	struct cdns_udma_chan *chan = to_cdns_udma_chan(dma_chan);
	struct device *dev = &dma_chan->dev->device;

	dev_dbg(dev, "Freeing channel %d\n", chan->idx);

	/*
	 * TODO: if busy, impelement cdns_udma_stop(chan)
	 * to disable chan and clear interrupt,
	 * see stm32_mdma_free_chan_resources
	 */

	vchan_free_chan_resources(to_virt_chan(dma_chan));
	dmam_pool_destroy(chan->desc_pool);
	chan->desc_pool = NULL;
	tasklet_kill(&chan->irqtask);
}

static int cdns_udma_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cdns_udma_dev *udma_dev;
	struct dma_device *dma_dev;
	int ret;
	u32 common_udma_config, common_udma_cap_ver;
	void __iomem *dma_base;
	int nr_channels;

	dma_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(dma_base)) {
		dev_err(dev, "missing \"reg\"\n");
		return PTR_ERR(dma_base);
	}

	common_udma_cap_ver = readl(dma_base + COMMON_UDMA_CAP_VER_OFFSET);

	common_udma_config = readl(dma_base + COMMON_UDMA_CONFIG_OFFSET);

	if (FIELD_GET(COMMON_UDMA_CAP_VER_MIN_VER_MASK, common_udma_cap_ver) !=
		    1 ||
	    FIELD_GET(COMMON_UDMA_CAP_VER_MAJ_VER_MASK, common_udma_cap_ver) !=
		    0) {
		dev_err(dev,
			"Current version of driver only supports uDMA v1.0\n");
		return -ENXIO;
	}

	dev_dbg(dev,
		"dma channel number is 0x%lx, partition size is 0x%lx, partition number is 0x%lx, sys addr width %s 32-bits, sys attr width %s 32-bits, ext addr width %s 32-bits, ext attr width %s 32-bits\n",
		FIELD_GET(COMMON_UDMA_CONFIG_NUM_CHANNELS_MASK,
			  common_udma_config),
		FIELD_GET(COMMON_UDMA_CONFIG_PARTITIONS_SIZE_MASK,
			  common_udma_config),
		FIELD_GET(COMMON_UDMA_CONFIG_NUM_PARTITIONS_MASK,
			  common_udma_config),
		FIELD_GET(COMMON_UDMA_CONFIG_SYS_AW_GT_32_MASK,
			  common_udma_config) ?
			      ">" :
			      "<",
		FIELD_GET(COMMON_UDMA_CONFIG_SYS_TW_GT_32_MASK,
			  common_udma_config) ?
			      ">" :
			      "<",
		FIELD_GET(COMMON_UDMA_CONFIG_EXT_AW_GT_32_MASK,
			  common_udma_config) ?
			      ">" :
			      "<",
		FIELD_GET(COMMON_UDMA_CONFIG_EXT_TW_GT_32_MASK,
			  common_udma_config) ?
			      ">" :
			      "<");

	nr_channels = FIELD_GET(COMMON_UDMA_CONFIG_NUM_CHANNELS_MASK,
				common_udma_config);

	udma_dev = devm_kzalloc(dev, struct_size(udma_dev, chan, nr_channels),
				GFP_KERNEL);
	if (!udma_dev)
		return -EINVAL;

	udma_dev->irq = platform_get_irq(pdev, 0);
	if (udma_dev->irq < 0)
		return udma_dev->irq;

	udma_dev->dev = &pdev->dev;

	udma_dev->chan_num = nr_channels;
	udma_dev->dma_base = dma_base;

	ret = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(64));
	if (ret)
		return ret;

	dma_dev = &udma_dev->dma_dev;
	dma_cap_zero(dma_dev->cap_mask);
	dma_cap_set(DMA_SLAVE, dma_dev->cap_mask);
	dma_cap_set(DMA_PRIVATE, dma_dev->cap_mask);
	// TODO: impelment device_terminate_all
	dma_dev->device_prep_slave_sg = cdns_udma_prep_slave_sg;
	dma_dev->device_issue_pending = cdns_udma_issue_pending;
	dma_dev->device_config = cdns_udma_device_config;
	dma_dev->device_tx_status = cdns_udma_tx_status;
	dma_dev->device_alloc_chan_resources = cdns_udma_alloc_chan_resources;
	dma_dev->device_free_chan_resources = cdns_udma_free_chan_resources;
	dma_dev->directions = BIT(DMA_MEM_TO_DEV) | BIT(DMA_DEV_TO_MEM);
	dma_dev->src_addr_widths = BIT(DMA_SLAVE_BUSWIDTH_4_BYTES) |
				   BIT(DMA_SLAVE_BUSWIDTH_8_BYTES);
	dma_dev->dst_addr_widths = BIT(DMA_SLAVE_BUSWIDTH_4_BYTES) |
				   BIT(DMA_SLAVE_BUSWIDTH_8_BYTES);

	dma_dev->dev = dev;
	INIT_LIST_HEAD(&dma_dev->channels);

	ret = cdns_udma_enable_hw_channels(udma_dev);
	if (ret < 0) {
		dev_err(dev, "failed to enable hw channel!\n");
		return ret;
	}

	if (ret)
		return ret;

	ret = dmaenginem_async_device_register(dma_dev);
	if (ret < 0)
		dev_err(dev, "failed to register device!\n");

	return ret;
}

static const struct of_device_id cdns_udma_match[] = {
	{ .compatible = "cdns,udma" },
	{},
};
MODULE_DEVICE_TABLE(of, cdns_udma_match);

/*
 * Use platform_driver instead of pci_driver because if udma works for EP-mode
 * controller, the controller itself is also platform_driver, and there may be
 * no RC controller to scan pci device.
 *
 * XXX: but why dw-edma is a pci_driver? Is it because dw-edma works as PCI device?
 *
 */
static struct platform_driver cdns_udma_platform_driver = {
	.driver = {
		.name = "cdns_udma",
		.of_match_table = cdns_udma_match,
	},
	.probe = cdns_udma_probe,
};
module_platform_driver(cdns_udma_platform_driver);

MODULE_AUTHOR("Li Chen <lchen@ambarella.com>");
MODULE_DESCRIPTION("Candence uDMA controller driver");
MODULE_LICENSE("GPL v2");
