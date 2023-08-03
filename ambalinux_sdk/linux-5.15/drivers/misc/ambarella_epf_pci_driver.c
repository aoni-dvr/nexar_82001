// SPDX-License-Identifier: GPL-2.0-only
/**
 * Host side driver to endpoint functionality
 *
 * Copyright (C) 2017 Texas Instruments
 * Author: Kishon Vijay Abraham I <kishon@ti.com>
 *
 * Copyright (C) 2022 by Ambarella, Inc.
 */

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <uapi/linux/ambarella/pci-epf.h>

#include <linux/pci_regs.h>

#define DRV_MODULE_NAME				"ambarella-epf-driver"

#define IRQ_TYPE_UNDEFINED			-1
#define IRQ_TYPE_LEGACY				0
#define IRQ_TYPE_MSI				1
#define IRQ_TYPE_MSIX				2

#define PCI_EPF_MAGIC			0x0

#define PCI_EPF_COMMAND		0x4
#define COMMAND_RAISE_LEGACY_IRQ		BIT(0)
#define COMMAND_RAISE_MSI_IRQ			BIT(1)
#define COMMAND_RAISE_MSIX_IRQ			BIT(2)
#define COMMAND_READ				BIT(3)
#define COMMAND_WRITE				BIT(4)
#define COMMAND_COPY				BIT(5)

#define PCI_EPF_STATUS		0x8
#define STATUS_READ_SUCCESS			BIT(0)
#define STATUS_READ_FAIL			BIT(1)
#define STATUS_WRITE_SUCCESS			BIT(2)
#define STATUS_WRITE_FAIL			BIT(3)
#define STATUS_COPY_SUCCESS			BIT(4)
#define STATUS_COPY_FAIL			BIT(5)
#define STATUS_IRQ_RAISED			BIT(6)
#define STATUS_SRC_ADDR_INVALID			BIT(7)
#define STATUS_DST_ADDR_INVALID			BIT(8)

#define PCI_EPF_LOWER_SRC_ADDR	0x0c
#define PCI_EPF_UPPER_SRC_ADDR	0x10

#define PCI_EPF_LOWER_DST_ADDR	0x14
#define PCI_EPF_UPPER_DST_ADDR	0x18

#define PCI_EPF_SIZE			0x1c
#define PCI_EPF_CHECKSUM		0x20

#define PCI_EPF_IRQ_TYPE		0x24
#define PCI_EPF_IRQ_NUMBER		0x28

#define PCI_EPF_FLAGS			0x2c
#define FLAG_USE_DMA				BIT(0)

#define PCI_DEVICE_ID_AMBAEP	        0x0200

#define is_am654_pci_dev(pdev)		\
		((pdev)->device == PCI_DEVICE_ID_TI_AM654)

static DEFINE_IDA(pci_endpoint_epf_ida);

#define to_endpoint_epf(priv) container_of((priv), struct pci_endpoint_epf, \
					    miscdev)

static bool no_msi;
module_param(no_msi, bool, 0444);
MODULE_PARM_DESC(no_msi, "Disable MSI interrupt in pci_endpoint_epf");

static int irq_type = IRQ_TYPE_MSI;
module_param(irq_type, int, 0444);
MODULE_PARM_DESC(irq_type, "IRQ mode selection in pci_endpoint_epf (0 - Legacy, 1 - MSI, 2 - MSI-X)");

enum pci_barno {
	BAR_0,
	BAR_1,
	BAR_2,
	BAR_3,
	BAR_4,
	BAR_5,
};

struct pci_endpoint_epf {
	struct pci_dev	*pdev;
	void __iomem	*base;
	void __iomem	*bar[PCI_STD_NUM_BARS];
	struct completion irq_raised;
	int		last_irq;
	int		num_irqs;
	int		irq_type;
	struct miscdevice miscdev;
	enum pci_barno reg_bar;
	size_t alignment;
	const char *name;
} *epf;

struct pci_endpoint_epf_data {
	enum pci_barno reg_bar;
	size_t alignment;
	int irq_type;
};

static inline u32 pci_endpoint_epf_readl(struct pci_endpoint_epf *epf,
					  u32 offset)
{
	return readl(epf->base + offset);
}

static inline void pci_endpoint_epf_writel(struct pci_endpoint_epf *epf,
					    u32 offset, u32 value)
{
	writel(value, epf->base + offset);
}

static inline __iomem void *amba_get_bar_mem(enum pci_barno bar, u32 offset)
{
	int size;
	struct pci_dev *pdev = epf->pdev;

	if (!epf->bar[bar]) {
		pci_err(pdev, "bar %d is not available", bar);
		return NULL;
	}

	size = pci_resource_len(pdev, bar);

	if (offset > size) {
		pci_err(pdev, "offset is larger than bar size");
		return NULL;
	}

	return epf->bar[bar] + offset;
}

static u32 amba_bar_readl(enum pci_barno bar, u32 offset)
{
	void *__iomem *addr = amba_get_bar_mem(bar, offset);

	if (!addr) {
		pci_err(epf->pdev, "%s: offset %d is large than bar size",
			__func__, offset);
		return -1;
	}
	return readl(addr);
}

static void amba_bar_writel(enum pci_barno bar, u32 offset, u32 value)
{
	void *__iomem *addr = amba_get_bar_mem(bar, offset);

	if (!addr) {
		pci_err(epf->pdev, "%s: offset %d is large than bar size",
			__func__, offset);
		return;
	}
	writel(value, addr);
}

/**
 * amba_bar2_writel - get bar2's virtual mem
 * @offset: offset from bar2's base.
 * This function is to be used with memcpy_from/toio_ambarella, like
 *   - memcpy_fromio_ambarella(buf, amba_get_bar2_mem(8), sizeof(str));
 *   - memcpy_toio_ambarella(amba_get_bar2_mem(8), str, sizeof(str));
 */
inline __iomem void *amba_get_bar2_mem(u32 offset)
{
	return amba_get_bar_mem(BAR_2, offset);
}
EXPORT_SYMBOL(amba_get_bar2_mem);

/**
 * amba_bar2_readl - read EP's memory via bar2
 * @offset: offset from bar2's base.
 * Other bars are reserved.
 */
inline u32 amba_bar2_readl(u32 offset)
{
	return amba_bar_readl(BAR_2, offset);
}
EXPORT_SYMBOL(amba_bar2_readl);

/**
 * amba_bar2_writel - write EP's memory via bar2
 * @offset: offset from bar2's base.
 * Other bars are reserved.
 */
inline void amba_bar2_writel(u32 offset, u32 value)
{
	amba_bar_writel(BAR_2, offset, value);
}
EXPORT_SYMBOL(amba_bar2_writel);

static irqreturn_t pci_endpoint_epf_irqhandler(int irq, void *dev_id)
{
	struct pci_endpoint_epf *epf = dev_id;
	u32 reg;

	reg = pci_endpoint_epf_readl(epf, PCI_EPF_STATUS);
	if (reg & STATUS_IRQ_RAISED) {
		epf->last_irq = irq;
		complete(&epf->irq_raised);
		reg &= ~STATUS_IRQ_RAISED;
	}
	pci_endpoint_epf_writel(epf, PCI_EPF_STATUS,
				 reg);

	return IRQ_HANDLED;
}

static void pci_endpoint_epf_free_irq_vectors(struct pci_endpoint_epf *epf)
{
	struct pci_dev *pdev = epf->pdev;

	pci_free_irq_vectors(pdev);
	epf->irq_type = IRQ_TYPE_UNDEFINED;
}

static bool pci_endpoint_epf_alloc_irq_vectors(struct pci_endpoint_epf *epf,
						int type)
{
	int irq = -1;
	struct pci_dev *pdev = epf->pdev;
	struct device *dev = &pdev->dev;
	bool res = true;

	switch (type) {
	case IRQ_TYPE_LEGACY:
		irq = pci_alloc_irq_vectors(pdev, 1, 1, PCI_IRQ_LEGACY);
		if (irq < 0)
			dev_err(dev, "Failed to get Legacy interrupt\n");
		break;
	case IRQ_TYPE_MSI:
		irq = pci_alloc_irq_vectors(pdev, 1, 32, PCI_IRQ_MSI);
		if (irq < 0)
			dev_err(dev, "Failed to get MSI interrupts\n");
		break;
	case IRQ_TYPE_MSIX:
		irq = pci_alloc_irq_vectors(pdev, 1, 2048, PCI_IRQ_MSIX);
		if (irq < 0)
			dev_err(dev, "Failed to get MSI-X interrupts\n");
		break;
	default:
		dev_err(dev, "Invalid IRQ type selected\n");
	}

	if (irq < 0) {
		irq = 0;
		res = false;
	}

	epf->irq_type = type;
	epf->num_irqs = irq;

	return res;
}

static void pci_endpoint_epf_release_irq(struct pci_endpoint_epf *epf)
{
	int i;
	struct pci_dev *pdev = epf->pdev;
	struct device *dev = &pdev->dev;

	for (i = 0; i < epf->num_irqs; i++)
		devm_free_irq(dev, pci_irq_vector(pdev, i), epf);

	epf->num_irqs = 0;
}

static bool pci_endpoint_epf_request_irq(struct pci_endpoint_epf *epf)
{
	int i;
	int err;
	struct pci_dev *pdev = epf->pdev;
	struct device *dev = &pdev->dev;

	for (i = 0; i < epf->num_irqs; i++) {
		err = devm_request_irq(dev, pci_irq_vector(pdev, i),
				       pci_endpoint_epf_irqhandler,
				       IRQF_SHARED, epf->name, epf);
		if (err)
			goto fail;
	}

	return true;

fail:
	switch (irq_type) {
	case IRQ_TYPE_LEGACY:
		dev_err(dev, "Failed to request IRQ %d for Legacy\n",
			pci_irq_vector(pdev, i));
		break;
	case IRQ_TYPE_MSI:
		dev_err(dev, "Failed to request IRQ %d for MSI %d\n",
			pci_irq_vector(pdev, i),
			i + 1);
		break;
	case IRQ_TYPE_MSIX:
		dev_err(dev, "Failed to request IRQ %d for MSI-X %d\n",
			pci_irq_vector(pdev, i),
			i + 1);
		break;
	}

	return false;
}

static const struct file_operations pci_endpoint_epf_fops = {
	.owner = THIS_MODULE,
};

static int pci_endpoint_epf_probe(struct pci_dev *pdev,
				   const struct pci_device_id *ent)
{
	int err;
	int id;
	char name[24];
	enum pci_barno bar;
	void __iomem *base;
	struct device *dev = &pdev->dev;
	struct pci_endpoint_epf_data *data;
	enum pci_barno reg_bar = BAR_0;
	struct miscdevice *misc_device;

	if (pci_is_bridge(pdev))
		return -ENODEV;

	epf = devm_kzalloc(dev, sizeof(*epf), GFP_KERNEL);
	if (!epf)
		return -ENOMEM;

	epf->reg_bar = 0;
	epf->alignment = 0;
	epf->pdev = pdev;
	epf->irq_type = IRQ_TYPE_UNDEFINED;

	if (no_msi)
		irq_type = IRQ_TYPE_LEGACY;

	data = (struct pci_endpoint_epf_data *)ent->driver_data;
	if (data) {
		reg_bar = data->reg_bar;
		epf->reg_bar = reg_bar;
		epf->alignment = data->alignment;
		irq_type = data->irq_type;
	}

	init_completion(&epf->irq_raised);

	if ((dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(48)) != 0) &&
	    dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32)) != 0) {
		dev_err(dev, "Cannot set DMA mask\n");
		return -EINVAL;
	}

	err = pci_enable_device(pdev);
	if (err) {
		dev_err(dev, "Cannot enable PCI device\n");
		return err;
	}

	err = pci_request_regions(pdev, DRV_MODULE_NAME);
	if (err) {
		dev_err(dev, "Cannot obtain PCI resources\n");
		goto err_disable_pdev;
	}

	pci_set_master(pdev);

	if (!pci_endpoint_epf_alloc_irq_vectors(epf, irq_type)) {
		err = -EINVAL;
		goto err_disable_irq;
	}

	for (bar = 0; bar < PCI_STD_NUM_BARS; bar++) {
		if (pci_resource_flags(pdev, bar) & IORESOURCE_MEM) {
			base = pci_ioremap_bar(pdev, bar);
			if (!base) {
				dev_err(dev, "Failed to read BAR%d\n", bar);
				WARN_ON(bar == reg_bar);
			}
			epf->bar[bar] = base;
		}
	}

	epf->base = epf->bar[reg_bar];
	if (!epf->base) {
		err = -ENOMEM;
		dev_err(dev, "Cannot perform PCI epf without BAR%d\n",
			reg_bar);
		goto err_iounmap;
	}

	pci_set_drvdata(pdev, epf);

	id = ida_simple_get(&pci_endpoint_epf_ida, 0, 0, GFP_KERNEL);
	if (id < 0) {
		err = id;
		dev_err(dev, "Unable to get id\n");
		goto err_iounmap;
	}

	snprintf(name, sizeof(name), DRV_MODULE_NAME ".%d", id);
	epf->name = kstrdup(name, GFP_KERNEL);
	if (!epf->name) {
		err = -ENOMEM;
		goto err_ida_remove;
	}

	if (!pci_endpoint_epf_request_irq(epf)) {
		err = -EINVAL;
		goto err_kfree_epf_name;
	}

	misc_device = &epf->miscdev;
	misc_device->minor = MISC_DYNAMIC_MINOR;
	misc_device->name = kstrdup(name, GFP_KERNEL);
	if (!misc_device->name) {
		err = -ENOMEM;
		goto err_release_irq;
	}
	misc_device->parent = &pdev->dev;
	misc_device->fops = &pci_endpoint_epf_fops;

	err = misc_register(misc_device);
	if (err) {
		dev_err(dev, "Failed to register device\n");
		goto err_kfree_name;
	}

	return 0;

err_kfree_name:
	kfree(misc_device->name);

err_release_irq:
	pci_endpoint_epf_release_irq(epf);

err_kfree_epf_name:
	kfree(epf->name);

err_ida_remove:
	ida_simple_remove(&pci_endpoint_epf_ida, id);

err_iounmap:
	for (bar = 0; bar < PCI_STD_NUM_BARS; bar++) {
		if (epf->bar[bar])
			pci_iounmap(pdev, epf->bar[bar]);
	}

err_disable_irq:
	pci_endpoint_epf_free_irq_vectors(epf);
	pci_release_regions(pdev);

err_disable_pdev:
	pci_disable_device(pdev);

	return err;
}

static void pci_endpoint_epf_remove(struct pci_dev *pdev)
{
	int id;
	enum pci_barno bar;
	struct pci_endpoint_epf *epf = pci_get_drvdata(pdev);
	struct miscdevice *misc_device = &epf->miscdev;

	if (sscanf(misc_device->name, DRV_MODULE_NAME ".%d", &id) != 1)
		return;
	if (id < 0)
		return;

	misc_deregister(&epf->miscdev);
	kfree(misc_device->name);
	kfree(epf->name);
	ida_simple_remove(&pci_endpoint_epf_ida, id);
	for (bar = 0; bar < PCI_STD_NUM_BARS; bar++) {
		if (epf->bar[bar])
			pci_iounmap(pdev, epf->bar[bar]);
	}

	pci_endpoint_epf_release_irq(epf);
	pci_endpoint_epf_free_irq_vectors(epf);

	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

static const struct pci_endpoint_epf_data cv5_data = {
	.alignment = SZ_256,
	.irq_type = IRQ_TYPE_LEGACY,
};

static const struct pci_device_id pci_endpoint_epf_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_CDNS, PCI_DEVICE_ID_AMBAEP),
	  .driver_data = (kernel_ulong_t)&cv5_data,
	},
	{ }
};
MODULE_DEVICE_TABLE(pci, pci_endpoint_epf_tbl);

static struct pci_driver pci_endpoint_epf_driver = {
	.name		= DRV_MODULE_NAME,
	.id_table	= pci_endpoint_epf_tbl,
	.probe		= pci_endpoint_epf_probe,
	.remove		= pci_endpoint_epf_remove,
	.sriov_configure = pci_sriov_configure_simple,
};
module_pci_driver(pci_endpoint_epf_driver);

MODULE_DESCRIPTION("AMBARELLA PCI ENDPOINT HOST DRIVER");
MODULE_AUTHOR("Kishon Vijay Abraham I <kishon@ti.com>");
MODULE_AUTHOR("Li Chen <lchen@ambarella.com>");
MODULE_LICENSE("GPL v2");
