/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * ambarella-pci-epf.h - PCI epf uapi defines
 *
 * Copyright (C) 2017 Texas Instruments
 * Author: Kishon Vijay Abraham I <kishon@ti.com>
 *
 * Copyright (C) 2022 by Ambarella, Inc.
 */

/*
 * TODO: use this file for our epf test and epf driver, just like
 * what tools/pci/pcitest.c does
 */
#ifndef __UAPI_LINUX_PCI_EPF_H
#define __UAPI_LINUX_PCI_EPF_H

#define PCI_EPF_BAR		_IO('P', 0x1)
#define PCI_EPF_LEGACY_IRQ	_IO('P', 0x2)
#define PCI_EPF_MSI		_IOW('P', 0x3, int)
#define PCI_EPF_WRITE		_IOW('P', 0x4, unsigned long)
#define PCI_EPF_READ		_IOW('P', 0x5, unsigned long)
#define PCI_EPF_COPY		_IOW('P', 0x6, unsigned long)
#define PCI_EPF_MSIX		_IOW('P', 0x7, int)
#define PCI_EPF_SET_IRQTYPE	_IOW('P', 0x8, int)
#define PCI_EPF_GET_IRQTYPE	_IO('P', 0x9)
#define PCI_EPF_CLEAR_IRQ	_IO('P', 0x10)

#define PCI_EPF_FLAGS_USE_DMA	0x00000001

struct pci_endpoint_epf_xfer_param {
	unsigned long size;
	unsigned char flags;
};

#endif /* __UAPI_LINUX_PCI_EPF_H */
