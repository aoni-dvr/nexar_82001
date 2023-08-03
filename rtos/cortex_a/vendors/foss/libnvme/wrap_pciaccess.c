#include "nvme_internal.h"
#include "AmbaPCIE.h"

#define NVME_AXI_BASE PCIE_AXI_BASE
#define NVME_RP_OB_REGION0_AXI_ADDR PCIE_RP_OB_REGION0_AXI_ADDR

/* AXI address */
// inbound:  0x00.0000.0000 - 0x00.FFFF.FFFF  4GB, 31+1 bits
// config:   0x20.0000.0000 - 0x20.0000.0FFF  4KB, 11+1 bits
// outbound: 0x20.0200.0000 - 0x20.0100.3FFF 16KB, 13+1 bits
#define NVME_RP_OB_AXI      0x0002000000UL
#define NVME_RP_OB_SIZE     (16*1024)
#define NVME_RP_IB_MEM      0x0000000000UL
#define NVME_RP_IB_SIZE     CONFIG_DDR_SIZE  // DRAM size

/* PCI address*/
// outbound: 0x02.0000.0000 - 0x02.0000.3FFF
// inbound:  0x00.0000.0000 - 0x00.FFFF.FFFF
#define NVME_RP_OB_PCI      0x0200000000ULL
#define NVME_RP_IB_PCI      0x0000000000UL

int wrap_nvme_pcicfg_map_bar(struct pci_device *dev, void *addr)
{
    PCIER_OB_PARAM_s ob_parm;

    *(uint64_t *)addr = NVME_AXI_BASE + NVME_RP_OB_AXI;

    // setup bar0 to enable nvme control registers
    ob_parm.AxiAddr = NVME_RP_OB_AXI;
    ob_parm.PciAddr = NVME_RP_OB_PCI;
    ob_parm.Size    = NVME_RP_OB_SIZE;
    return AmbaPCIER_Outbound(&ob_parm);
}

inline void wrap_nvme_pcicfg_read32(struct pci_device *dev, void *value, uint32_t offset)
{
    uint32_t data = *((volatile uint32_t *)(dev->base + offset));
    *((uint32_t *)value) = data;
}

inline void wrap_nvme_pcicfg_write32(struct pci_device *dev, void *value, uint32_t offset)
{
    *((volatile uint32_t *)(dev->base + offset)) = *((uint32_t *)value);
}

static struct pci_device dev;

struct pci_device *wrap_nvme_pci_ctrlr_probe(void)
{
    PCIE_INIT_PARAM_s sys_parm = {
            .MemConfig = NULL,
            .Mode      = PCIE_RC_MODE,
            .Gen       = PCIE_CTRL_GEN,
            .Lane      = PCIE_LANE_WIDTH,
            .ClockSrc  = PCIE_CLK_SRC,
            .IsrEnable = PCIE_ISR_DISABLE,
    };
    PCIER_IB_PARAM_s ib_parm;

    // Init driver
    AmbaPCIE_Init(&sys_parm);

    // Start link training
    AmbaPCIE_Start();

    // config ib mmio
    ib_parm.MemAddr = NVME_RP_IB_MEM;
    ib_parm.PciAddr = NVME_RP_IB_PCI;
    if ((NVME_RP_IB_SIZE & (NVME_RP_IB_SIZE - 1)) != 0UL) {
        uint64_t size;
        for (size = 1; size <= NVME_RP_IB_SIZE; size *= 2);
        ib_parm.Size = size;
    } else {
        ib_parm.Size = NVME_RP_IB_SIZE;
    }
    (void)AmbaPCIER_Inbound(&ib_parm);

    dev.base = NVME_RP_OB_REGION0_AXI_ADDR;

    return &dev;
}
