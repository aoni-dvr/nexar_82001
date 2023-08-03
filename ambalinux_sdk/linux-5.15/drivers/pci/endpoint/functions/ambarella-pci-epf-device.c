// SPDX-License-Identifier: GPL-2.0
/*
 * EPF driver for endpoint functionality
 *
 * Copyright (C) 2017 Texas Instruments
 * Author: Kishon Vijay Abraham I <kishon@ti.com>
 *
 * Copyright (C) 2022 by Ambarella, Inc.
 */

#include <linux/delay.h>
#include <linux/dmaengine.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/pci_ids.h>
#include <linux/random.h>

#include <linux/pci-epc.h>
#include <linux/pci-epf.h>
#include <linux/pci_regs.h>

#define IRQ_TYPE_LEGACY			0
#define IRQ_TYPE_MSI			1
#define IRQ_TYPE_MSIX			2

#define COMMAND_RAISE_LEGACY_IRQ	BIT(0)
#define COMMAND_RAISE_MSI_IRQ		BIT(1)
#define COMMAND_RAISE_MSIX_IRQ		BIT(2)
#define COMMAND_READ			BIT(3)
#define COMMAND_WRITE			BIT(4)
#define COMMAND_COPY			BIT(5)

#define STATUS_READ_SUCCESS		BIT(0)
#define STATUS_READ_FAIL		BIT(1)
#define STATUS_WRITE_SUCCESS		BIT(2)
#define STATUS_WRITE_FAIL		BIT(3)
#define STATUS_COPY_SUCCESS		BIT(4)
#define STATUS_COPY_FAIL		BIT(5)
#define STATUS_IRQ_RAISED		BIT(6)
#define STATUS_SRC_ADDR_INVALID		BIT(7)
#define STATUS_DST_ADDR_INVALID		BIT(8)

#define FLAG_USE_DMA			BIT(0)

#define TIMER_RESOLUTION		1

#define UPDATE_STATUS \
do { \
	reg->status = operation == WRITE ? STATUS_DST_ADDR_INVALID	\
		: STATUS_SRC_ADDR_INVALID; \
} while (0)

struct amba_epf_device {
	void			*reg[PCI_STD_NUM_BARS];
	struct pci_epf		*epf;
	enum pci_barno		reg_bar;
	size_t			msix_table_offset;
	struct completion	transfer_complete;
	const struct pci_epc_features *epc_features;
};

struct amba_epf_device_reg {
	u32	magic;
	u32	command;
	u32	status;
	u64	src_addr;
	u64	dst_addr;
	u32	size;
	u32	checksum;
	u32	irq_type;
	u32	irq_number;
	u32	flags;
} __packed;

static struct pci_epf_header header = {
	.vendorid	= PCI_ANY_ID,
	.deviceid	= PCI_ANY_ID,
	.baseclass_code = PCI_CLASS_OTHERS,
	.interrupt_pin	= PCI_INTERRUPT_INTA,
};

static size_t bar_size[] = { 512, 512, 1024, 16384, 131072, 1048576 };

static void amba_epf_device_unbind(struct pci_epf *epf)
{
	struct amba_epf_device *epf_device = epf_get_drvdata(epf);
	struct pci_epc *epc = epf->epc;
	struct pci_epf_bar *epf_bar;
	int bar;

	pci_epc_stop(epc);
	for (bar = 0; bar < PCI_STD_NUM_BARS; bar++) {
		epf_bar = &epf->bar[bar];

		if (epf_device->reg[bar]) {
			pci_epc_clear_bar(epc, epf->func_no, epf->vfunc_no,
					  epf_bar);
			pci_epf_free_space(epf, epf_device->reg[bar], bar,
					   PRIMARY_INTERFACE);
		}
	}
}

static int amba_epf_device_set_bar(struct pci_epf *epf)
{
	int bar, add;
	int ret;
	struct pci_epf_bar *epf_bar;
	struct pci_epc *epc = epf->epc;
	struct device *dev = &epf->dev;
	struct amba_epf_device *epf_device = epf_get_drvdata(epf);
	enum pci_barno reg_bar = epf_device->reg_bar;
	const struct pci_epc_features *epc_features;

	epc_features = epf_device->epc_features;

	for (bar = 0; bar < PCI_STD_NUM_BARS; bar += add) {
		epf_bar = &epf->bar[bar];
		/*
		 * pci_epc_set_bar() sets PCI_BASE_ADDRESS_MEM_TYPE_64
		 * if the specific implementation required a 64-bit BAR,
		 * even if we only requested a 32-bit BAR.
		 */
		add = (epf_bar->flags & PCI_BASE_ADDRESS_MEM_TYPE_64) ? 2 : 1;

		if (!!(epc_features->reserved_bar & (1 << bar)))
			continue;

		ret = pci_epc_set_bar(epc, epf->func_no, epf->vfunc_no,
				      epf_bar);
		if (ret) {
			pci_epf_free_space(epf, epf_device->reg[bar], bar,
					   PRIMARY_INTERFACE);
			dev_err(dev, "Failed to set BAR%d\n", bar);
			if (bar == reg_bar)
				return ret;
		}
	}

	return 0;
}

static int amba_epf_device_core_init(struct pci_epf *epf)
{
	struct amba_epf_device *epf_device = epf_get_drvdata(epf);
	struct pci_epf_header *header = epf->header;
	const struct pci_epc_features *epc_features;
	struct pci_epc *epc = epf->epc;
	struct device *dev = &epf->dev;
	bool msix_capable = false;
	bool msi_capable = true;
	int ret;

	epc_features = pci_epc_get_features(epc, epf->func_no, epf->vfunc_no);
	if (epc_features) {
		msix_capable = epc_features->msix_capable;
		msi_capable = epc_features->msi_capable;
	}

	if (epf->vfunc_no <= 1) {
		ret = pci_epc_write_header(epc, epf->func_no, epf->vfunc_no, header);
		if (ret) {
			dev_err(dev, "Configuration header write failed\n");
			return ret;
		}
	}

	ret = amba_epf_device_set_bar(epf);
	if (ret)
		return ret;

	if (msi_capable) {
		ret = pci_epc_set_msi(epc, epf->func_no, epf->vfunc_no,
				      epf->msi_interrupts);
		if (ret) {
			dev_err(dev, "MSI configuration failed\n");
			return ret;
		}
	}

	if (msix_capable) {
		ret = pci_epc_set_msix(epc, epf->func_no, epf->vfunc_no,
				       epf->msix_interrupts,
				       epf_device->reg_bar,
				       epf_device->msix_table_offset);
		if (ret) {
			dev_err(dev, "MSI-X configuration failed\n");
			return ret;
		}
	}

	return 0;
}

static int amba_epf_device_alloc_space(struct pci_epf *epf)
{
	struct amba_epf_device *epf_device = epf_get_drvdata(epf);
	struct device *dev = &epf->dev;
	struct pci_epf_bar *epf_bar;
	size_t msix_table_size = 0;
	size_t reg_bar_size;
	size_t pba_size = 0;
	bool msix_capable;
	void *base;
	int bar, add;
	enum pci_barno reg_bar = epf_device->reg_bar;
	const struct pci_epc_features *epc_features;
	size_t reg_size;

	epc_features = epf_device->epc_features;

	reg_bar_size = ALIGN(sizeof(struct amba_epf_device_reg), 128);

	msix_capable = epc_features->msix_capable;
	if (msix_capable) {
		msix_table_size = PCI_MSIX_ENTRY_SIZE * epf->msix_interrupts;
		epf_device->msix_table_offset = reg_bar_size;
		/* Align to QWORD or 8 Bytes */
		pba_size = ALIGN(DIV_ROUND_UP(epf->msix_interrupts, 8), 8);
	}
	reg_size = reg_bar_size + msix_table_size + pba_size;

	if (epc_features->bar_fixed_size[reg_bar]) {
		if (reg_size > bar_size[reg_bar])
			return -ENOMEM;
		reg_size = bar_size[reg_bar];
	}

	base = pci_epf_alloc_space(epf, reg_size, reg_bar,
				   epc_features->align, PRIMARY_INTERFACE);
	if (!base) {
		dev_err(dev, "Failed to allocated register space\n");
		return -ENOMEM;
	}
	epf_device->reg[reg_bar] = base;

	for (bar = 0; bar < PCI_STD_NUM_BARS; bar += add) {
		epf_bar = &epf->bar[bar];
		add = (epf_bar->flags & PCI_BASE_ADDRESS_MEM_TYPE_64) ? 2 : 1;

		if (bar == reg_bar)
			continue;

		if (!!(epc_features->reserved_bar & (1 << bar)))
			continue;

		base = pci_epf_alloc_space(epf, bar_size[bar], bar,
					   epc_features->align,
					   PRIMARY_INTERFACE);
		if (!base)
			dev_err(dev, "Failed to allocate space for BAR%d\n",
				bar);
		epf_device->reg[bar] = base;
	}

	return 0;
}

static void pci_epf_configure_bar(struct pci_epf *epf,
				  const struct pci_epc_features *epc_features)
{
	struct pci_epf_bar *epf_bar;
	bool bar_fixed_64bit;
	bool bar_prefetch;
	int i;

	for (i = 0; i < PCI_STD_NUM_BARS; i++) {
		epf_bar = &epf->bar[i];
		bar_fixed_64bit = !!(epc_features->bar_fixed_64bit & (1 << i));
		bar_prefetch = !!(epc_features->bar_prefetch & (1 << i));
		if (bar_fixed_64bit)
			epf_bar->flags |= PCI_BASE_ADDRESS_MEM_TYPE_64;
		if (epc_features->bar_fixed_size[i])
			bar_size[i] = epc_features->bar_fixed_size[i];
		if (bar_prefetch)
			epf_bar->flags |= PCI_BASE_ADDRESS_MEM_PREFETCH;
	}
}

static int amba_epf_device_bind(struct pci_epf *epf)
{
	int ret;
	struct amba_epf_device *epf_device = epf_get_drvdata(epf);
	const struct pci_epc_features *epc_features;
	enum pci_barno reg_bar = BAR_0;
	struct pci_epc *epc = epf->epc;
	bool core_init_notifier = false;

	if (WARN_ON_ONCE(!epc))
		return -EINVAL;

	epc_features = pci_epc_get_features(epc, epf->func_no, epf->vfunc_no);
	if (!epc_features) {
		dev_err(&epf->dev, "epc_features not implemented\n");
		return -EOPNOTSUPP;
	}

	core_init_notifier = epc_features->core_init_notifier;
	reg_bar = pci_epc_get_first_free_bar(epc_features);
	if (reg_bar < 0)
		return -EINVAL;
	pci_epf_configure_bar(epf, epc_features);

	epf_device->reg_bar = reg_bar;
	epf_device->epc_features = epc_features;

	ret = amba_epf_device_alloc_space(epf);
	if (ret)
		return ret;

	if (!core_init_notifier) {
		ret = amba_epf_device_core_init(epf);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct pci_epf_device_id amba_epf_device_ids[] = {
	{
		.name = "ambarella_epf_device",
	},
	{},
};

static int amba_epf_device_probe(struct pci_epf *epf)
{
	struct amba_epf_device *epf_device;
	struct device *dev = &epf->dev;

	epf_device = devm_kzalloc(dev, sizeof(*epf_device), GFP_KERNEL);
	if (!epf_device)
		return -ENOMEM;

	epf->header = &header;
	epf_device->epf = epf;


	epf_set_drvdata(epf, epf_device);
	return 0;
}

static struct pci_epf_ops ops = {
	.unbind	= amba_epf_device_unbind,
	.bind	= amba_epf_device_bind,
};

static struct pci_epf_driver epf_device = {
	.driver.name	= "ambarella_epf_device",
	.probe		= amba_epf_device_probe,
	.id_table	= amba_epf_device_ids,
	.ops		= &ops,
	.owner		= THIS_MODULE,
};

static int __init amba_epf_device_init(void)
{
	int ret;

	ret = pci_epf_register_driver(&epf_device);
	if (ret) {
		pr_err("Failed to register pci epf device --> %d\n", ret);
		return ret;
	}

	return 0;
}
module_init(amba_epf_device_init);

static void __exit amba_epf_device_exit(void)
{
	pci_epf_unregister_driver(&epf_device);
}
module_exit(amba_epf_device_exit);

MODULE_DESCRIPTION("AMBARELLA PCI EPF DEVICE");
MODULE_AUTHOR("Kishon Vijay Abraham I <kishon@ti.com>");
MODULE_AUTHOR("Li Chen <lichen@ambarella.com>");
MODULE_LICENSE("GPL v2");
