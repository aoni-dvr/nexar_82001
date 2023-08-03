/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <lib/mmio.h>
#include <plat_private.h>

static void ambarella_set_arbiter(void)
{
#if defined(AMBARELLA_CV2)
	mmio_write_32(DDRC_BASE + 0x008, 0x200); /* DRAM_THROTTLE_DLN */
	mmio_write_32(DDRC_BASE + 0x080, 0x002a0036); /* AXI0/Cortex */
	mmio_write_32(DDRC_BASE + 0x084, 0x002a0006); /* ARM_DMA0/AHB */
	mmio_write_32(DDRC_BASE + 0x088, 0x002a0006); /* ARM_DMA1/AHB */
	mmio_write_32(DDRC_BASE + 0x08c, 0x102a0017); /* ENET*/
	mmio_write_32(DDRC_BASE + 0x090, 0x102a0017); /* Flash DMA (FDMA) */
	mmio_write_32(DDRC_BASE + 0x094, 0x102a0006); /* CANC0 */
	mmio_write_32(DDRC_BASE + 0x098, 0x102a0006); /* CANC1 */
	mmio_write_32(DDRC_BASE + 0x09c, 0x102a0000); /* GDMA */
	mmio_write_32(DDRC_BASE + 0x0a0, 0x102a0017); /* SDXC0 */
	mmio_write_32(DDRC_BASE + 0x0a4, 0x102a0017); /* SDXC1 */
	mmio_write_32(DDRC_BASE + 0x0a8, 0x102a0006); /* USB20 (device) */
	mmio_write_32(DDRC_BASE + 0x0ac, 0x102a0006); /* USB20 (host) */
	mmio_write_32(DDRC_BASE + 0x0b0, 0x102a0008); /* OrcMe */
	mmio_write_32(DDRC_BASE + 0x0b4, 0x102a0008); /* OrcCode */
	mmio_write_32(DDRC_BASE + 0x0b8, 0x102a0004); /* OrcVp */
	mmio_write_32(DDRC_BASE + 0x0bc, 0x102a0004); /* OrcL2 Cache */
	mmio_write_32(DDRC_BASE + 0x0c0, 0x103b0005); /* SMEM */
	mmio_write_32(DDRC_BASE + 0x0c4, 0x102a0004); /* VMEM0 */
	mmio_write_32(DDRC_BASE + 0x0c8, 0x102a0002); /* FEX */
	mmio_write_32(DDRC_BASE + 0x0cc, 0x102a0002); /* BMEM */
	mmio_write_32(DDRC_BASE + 0x0fc, 0x003b0009); /* SMEM hi_priority */
	mmio_write_32(DDRC_BASE + 0x10c, 0x20703); /* DRAM_SMEM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
	mmio_write_32(DDRC_BASE + 0x110, 0x103); /* DRAM_SMEM_USAGE_TARGET FDMA (0%) */
	mmio_write_32(DDRC_BASE + 0x120, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC0 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x124, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC1 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x140, 0x1e0); /* DRAM_SMEM_USAGE_TARGET SMEM (87.5%) */
	mmio_write_32(DDRC_BASE + 0x190, 0xC00); /* rw throttle mandatory = 48 */
	mmio_write_32(DDRC_BASE + 0x194, 0x20); /* bank throttle mandatory = 32 */

#elif defined(AMBARELLA_CV22)
	mmio_write_32(DDRC_BASE + 0x008, 0x200); /* DRAM_THROTTLE_DLN */
	mmio_write_32(DDRC_BASE + 0x080, 0x002a0036); /* AXI0/Cortex */
	mmio_write_32(DDRC_BASE + 0x084, 0x002a0006); /* ARM_DMA0/AHB */
	mmio_write_32(DDRC_BASE + 0x088, 0x002a0006); /* ARM_DMA1/AHB */
	mmio_write_32(DDRC_BASE + 0x08c, 0x102a0017); /* ENET*/
	mmio_write_32(DDRC_BASE + 0x090, 0x102a0017); /* Flash DMA (FDMA) */
	mmio_write_32(DDRC_BASE + 0x094, 0x102a0006); /* CANC0 */
	mmio_write_32(DDRC_BASE + 0x098, 0x102a0000); /* GDMA */
	mmio_write_32(DDRC_BASE + 0x09c, 0x102a0017); /* SDXC0 */
	mmio_write_32(DDRC_BASE + 0x0a0, 0x102a0017); /* SDXC1 */
	mmio_write_32(DDRC_BASE + 0x0a4, 0x102a0006); /* USB20 (device)  */
	mmio_write_32(DDRC_BASE + 0x0a8, 0x102a0006); /* USB20 (host) */
	mmio_write_32(DDRC_BASE + 0x0ac, 0x102a0008); /* OrcMe */
	mmio_write_32(DDRC_BASE + 0x0b0, 0x102a0008); /* OrcCode */
	mmio_write_32(DDRC_BASE + 0x0b4, 0x102a0004); /* OrcVp */
	mmio_write_32(DDRC_BASE + 0x0b8, 0x102a0004); /* OrcL2 Cache */
	mmio_write_32(DDRC_BASE + 0x0bc, 0x103b0005); /* SMEM */
	mmio_write_32(DDRC_BASE + 0x0c0, 0x102a0004); /* VMEM0 */
	mmio_write_32(DDRC_BASE + 0x0fc, 0x003b0009); /* SMEM hi_priority */
	mmio_write_32(DDRC_BASE + 0x10c, 0x20703); /* DRAM_SMEM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
	mmio_write_32(DDRC_BASE + 0x110, 0x103); /* DRAM_SMEM_USAGE_TARGET FDMA (1.17%) */
	mmio_write_32(DDRC_BASE + 0x11c, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC0 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x120, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC1 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x13C, 0x1e0); /* DRAM_SMEM_USAGE_TARGET SMEM (87.5%) */
	mmio_write_32(DDRC_BASE + 0x190, 0xC00); /* rw throttle mandatory = 48 */
	mmio_write_32(DDRC_BASE + 0x194, 0x20); /* bank throttle mandatory = 32 */

#elif defined(AMBARELLA_CV25) || defined(AMBARELLA_CV28)
	mmio_write_32(DDRC_BASE + 0x008, 0x200); /* DRAM_THROTTLE_DLN */
	mmio_write_32(DDRC_BASE + 0x080, 0x002a0036); /* AXI0/Cortex */
	mmio_write_32(DDRC_BASE + 0x084, 0x002a0006); /* ARM_DMA0/AHB */
	mmio_write_32(DDRC_BASE + 0x088, 0x002a0006); /* ARM_DMA1/AHB */
	mmio_write_32(DDRC_BASE + 0x08c, 0x102a0017); /* ENET*/
	mmio_write_32(DDRC_BASE + 0x090, 0x102a0017); /* Flash DMA (FDMA) */
	mmio_write_32(DDRC_BASE + 0x094, 0x102a0006); /* CANC0 */
	mmio_write_32(DDRC_BASE + 0x098, 0x102a0000); /* GDMA */
	mmio_write_32(DDRC_BASE + 0x09c, 0x102a0017); /* SDXC0 */
	mmio_write_32(DDRC_BASE + 0x0a0, 0x102a0017); /* SDXC1 */
	mmio_write_32(DDRC_BASE + 0x0a4, 0x102a0017); /* SDXC2 */
	mmio_write_32(DDRC_BASE + 0x0a8, 0x102a0006); /* USB20 (device)  */
	mmio_write_32(DDRC_BASE + 0x0ac, 0x102a0006); /* USB20 (host) */
	mmio_write_32(DDRC_BASE + 0x0b0, 0x102a0008); /* OrcMe */
	mmio_write_32(DDRC_BASE + 0x0b4, 0x102a0008); /* OrcCode */
	mmio_write_32(DDRC_BASE + 0x0b8, 0x102a0004); /* OrcVp */
	mmio_write_32(DDRC_BASE + 0x0bc, 0x102a0004); /* OrcL2 Cache */
	mmio_write_32(DDRC_BASE + 0x0c0, 0x103b0005); /* SMEM */
	mmio_write_32(DDRC_BASE + 0x0c4, 0x102a0004); /* VMEM0 */
	mmio_write_32(DDRC_BASE + 0x0fc, 0x003b0009); /* SMEM hi_priority */
	mmio_write_32(DDRC_BASE + 0x10c, 0x20703); /* DRAM_SMEM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
	mmio_write_32(DDRC_BASE + 0x110, 0x103); /* DRAM_SMEM_USAGE_TARGET FDMA (1.17%) */
	mmio_write_32(DDRC_BASE + 0x11c, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC0 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x120, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC1 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x124, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC2 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x140, 0x1e0); /* DRAM_SMEM_USAGE_TARGET SMEM (87.5%) */
	mmio_write_32(DDRC_BASE + 0x190, 0xC00); /* rw throttle mandatory = 48 */
	mmio_write_32(DDRC_BASE + 0x194, 0x20); /* bank throttle mandatory = 32 */

#elif defined(AMBARELLA_S6LM)
	mmio_write_32(DDRC_BASE + 0x008, 0x200); /* DRAM_THROTTLE_DLN */
	mmio_write_32(DDRC_BASE + 0x080, 0x00280035); /* AXI0/Cortex */
	mmio_write_32(DDRC_BASE + 0x084, 0x00280005); /* ARM_DMA0/AHB */
	mmio_write_32(DDRC_BASE + 0x088, 0x00280005); /* ARM_DMA1/AHB */
	mmio_write_32(DDRC_BASE + 0x08c, 0x10280016); /* ENET*/
	mmio_write_32(DDRC_BASE + 0x090, 0x10280016); /* Flash DMA (FDMA) */
	mmio_write_32(DDRC_BASE + 0x094, 0x10280000); /* GDMA */
	mmio_write_32(DDRC_BASE + 0x098, 0x10280016); /* SDXC0 */
	mmio_write_32(DDRC_BASE + 0x09c, 0x10280016); /* SDXC1 */
	mmio_write_32(DDRC_BASE + 0x0a0, 0x10280016); /* SDXC2 */
	mmio_write_32(DDRC_BASE + 0x0a4, 0x10280005); /* USB20 (device)  */
	mmio_write_32(DDRC_BASE + 0x0a8, 0x10280005); /* USB20 (host) */
	mmio_write_32(DDRC_BASE + 0x0ac, 0x10290006); /* OrcMe */
	mmio_write_32(DDRC_BASE + 0x0b0, 0x10290006); /* OrcCode */
	mmio_write_32(DDRC_BASE + 0x0b4, 0x10390004); /* SMEM */
	mmio_write_32(DDRC_BASE + 0x0fc, 0x003a0007); /* SMEM hi_priority */
	mmio_write_32(DDRC_BASE + 0x10c, 0x20703); /* DRAM_SMEM_USAGE_TARGET ENET (r:1.17%, w:1.17%) */
	mmio_write_32(DDRC_BASE + 0x110, 0x103); /* DRAM_SMEM_USAGE_TARGET FDMA (1.17%) */
	mmio_write_32(DDRC_BASE + 0x118, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC0 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x11c, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC1 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x120, 0x103); /* DRAM_SMEM_USAGE_TARGET SDXC2 (1.17%) */
	mmio_write_32(DDRC_BASE + 0x134, 0x1e0); /* DRAM_SMEM_USAGE_TARGET SMEM (87.5%) */
	mmio_write_32(DDRC_BASE + 0x190, 0xC00); /* rw throttle mandatory = 48 */
	mmio_write_32(DDRC_BASE + 0x194, 0x20); /* bank throttle mandatory = 32 */

#elif defined(AMBARELLA_CV5)
	mmio_write_32(DDRC_BASE + 0x110, 0x002d0069);   // 0: AXI0/Cortex
	mmio_write_32(DDRC_BASE + 0x114, 0x002d0069);   // 1: AXI1/Cortex
	mmio_write_32(DDRC_BASE + 0x118, 0x002d0007);   // 2: OrcL2 Cache
	mmio_write_32(DDRC_BASE + 0x11c, 0x002d0049);   // 3: USB3
	mmio_write_32(DDRC_BASE + 0x120, 0x002d0049);   // 4: PCIE
	mmio_write_32(DDRC_BASE + 0x124, 0x002d004a);   // 5: ENET0
	mmio_write_32(DDRC_BASE + 0x128, 0x002d004a);   // 6: ENET1
	mmio_write_32(DDRC_BASE + 0x12c, 0x002d004a);   // 7: Flash DMA (FDMA)
	mmio_write_32(DDRC_BASE + 0x130, 0x002d004a);   // 8: SDAXI0
	mmio_write_32(DDRC_BASE + 0x134, 0x002d004a);   // 9: SDAXI1
	mmio_write_32(DDRC_BASE + 0x138, 0x002d004a);   // 10: SDAHB0
	mmio_write_32(DDRC_BASE + 0x13c, 0x002d0009);   // 11: USB2_device
	mmio_write_32(DDRC_BASE + 0x140, 0x002d0009);   // 12: ARM_DMA0/AHB
	mmio_write_32(DDRC_BASE + 0x144, 0x002d0009);   // 13: ARM_DMA1/AHB
	mmio_write_32(DDRC_BASE + 0x148, 0x002d0009);   // 14: CANC0
	mmio_write_32(DDRC_BASE + 0x14c, 0x002d0003);   // 15: GDMA
	mmio_write_32(DDRC_BASE + 0x150, 0x002d000b);   // 16: OrcMe0
	mmio_write_32(DDRC_BASE + 0x154, 0x002d000b);   // 17: OrcCode0
	mmio_write_32(DDRC_BASE + 0x158, 0x002d000b);   // 18: OrcMe0
	mmio_write_32(DDRC_BASE + 0x15c, 0x002d000b);   // 19: OrcCode1
	mmio_write_32(DDRC_BASE + 0x160, 0x002d0007);   // 20: OrcVp
	mmio_write_32(DDRC_BASE + 0x164, 0x103e1108);   // 21: SMEM_WR: Granularity = 2KB
	mmio_write_32(DDRC_BASE + 0x168, 0x103e1108);   // 22: SMEM_RD: Granularity = 2KB
	mmio_write_32(DDRC_BASE + 0x16c, 0x002d0057);   // 23: VMEM0
	mmio_write_32(DDRC_BASE + 0x170, 0x002d0005);   // 24: DBSE
	mmio_write_32(DDRC_BASE + 0x190, 0x103f220f);   // SMEM_WR hi_priority: Granularity = 2KB
	mmio_write_32(DDRC_BASE + 0x194, 0x103f220f);   // SMEM_RD hi_priority: Granularity = 2KB
	mmio_write_32(DDRC_BASE + 0x198, 0x800);        // DRAM_THROTTLE_DLN (2048 cycles)
	mmio_write_32(DDRC_BASE + 0x1a8, 0x103);        // DRAM_USAGE_TARGET USB3 (1.17%)
	mmio_write_32(DDRC_BASE + 0x1ac, 0x108);        // DRAM_USAGE_TARGET PCIE (3.12%)
	mmio_write_32(DDRC_BASE + 0x1b0, 0x20703);      // DRAM_USAGE_TARGET ENET0 (r:1.17%, w:1.17%)
	mmio_write_32(DDRC_BASE + 0x1b4, 0x20703);      // DRAM_USAGE_TARGET ENET1 (r:1.17%, w:1.17%)
	mmio_write_32(DDRC_BASE + 0x1b8, 0x103);        // DRAM_USAGE_TARGET FDMA (1.17%)
	mmio_write_32(DDRC_BASE + 0x1bC, 0x103);        // DRAM_USAGE_TARGET SDAXI0 (1.17%)
	mmio_write_32(DDRC_BASE + 0x1c0, 0x103);        // DRAM_USAGE_TARGET SDAXI1 (1.17%)
	mmio_write_32(DDRC_BASE + 0x1c4, 0x103);        // DRAM_USAGE_TARGET SDAHB0 (1.17%)
	mmio_write_32(DDRC_BASE + 0x1f0, 0x170);        // DRAM_USAGE_TARGET SMEM_WR (43.75%)
	mmio_write_32(DDRC_BASE + 0x1f4, 0x170);        // DRAM_USAGE_TARGET SMEM_RD (43.75%)
	mmio_write_32(DDRC_BASE + 0x1f8, 0x130);        // DRAM_USAGE_TARGET VMEM0 (18.5%)
	mmio_write_32(DDRC_BASE + 0x21c, 0x01400C20);   // rw throttle mandatory = 32
	mmio_write_32(DDRC_BASE + 0x220, 0x01401030);   // bank throttle mandatory = 48
	mmio_write_32(DDRC_BASE + 0x224, 0x88);         // DIE_BG_THROTTLE: different_die/bg_disable_cycles = 8
#endif
}

void ambarella_soc_fixup(void)
{
	uint32_t core_freq = get_core_bus_freq_hz();

	ambarella_set_arbiter();

	if (core_freq < 466000000)
		mmio_setbits_32(RCT_REG(SYS_CONFIG_OFFSET), POC_PERIPHERAL_CLK_MODE);
	else
		mmio_clrbits_32(RCT_REG(SYS_CONFIG_OFFSET), POC_PERIPHERAL_CLK_MODE);
}

