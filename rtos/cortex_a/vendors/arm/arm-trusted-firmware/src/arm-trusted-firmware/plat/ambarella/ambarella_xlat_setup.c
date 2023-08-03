/*
 * Copyright (c) 2017, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <plat/common/platform.h>
#include <plat_private.h>

static const mmap_region_t ambarella_device_mmap[] = {
	MAP_REGION_FLAT(DEVICE_BASE, DEVICE_SIZE, MT_DEVICE | MT_RW | MT_SECURE),
#if !defined(AMBARELLA_CV5)
	MAP_REGION_FLAT(DDRC_BASE, DDRC_SIZE, MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(SRAM_BASE, SRAM_SIZE, MT_MEMORY | MT_RW | MT_SECURE),
#endif
	{0}
};

void ambarella_mmap_setup(const struct mmap_region *mmap)
{
	VERBOSE("Trusted RAM seen by this BL image: %p - %p\n",
		(void *)BL_CODE_BASE, (void *)BL_END);
	mmap_add_region(BL_CODE_BASE, BL_CODE_BASE,
			round_up(BL_END, PAGE_SIZE) - BL_CODE_BASE,
			MT_MEMORY | MT_RW | MT_SECURE);

	/* remap the code section */
	VERBOSE("Code region: %p - %p\n",
		(void *)BL_CODE_BASE, (void *)BL_CODE_END);
	mmap_add_region(BL_CODE_BASE, BL_CODE_BASE,
			round_up(BL_CODE_END, PAGE_SIZE) - BL_CODE_BASE,
			MT_CODE | MT_SECURE);

	/* Re-map the read-only data section */
	VERBOSE("Read-only data region: %p - %p\n",
		(void *)BL_RO_DATA_BASE, (void *)BL_RO_DATA_END);
	mmap_add_region(BL_RO_DATA_BASE, BL_RO_DATA_BASE,
			BL_RO_DATA_END - BL_RO_DATA_BASE,
			MT_RO_DATA | MT_SECURE);

#if USE_COHERENT_MEM
	/* remap the coherent memory region */
	VERBOSE("Coherent region: %p - %p\n",
		(void *)BL_COHERENT_RAM_BASE, (void *)BL_COHERENT_RAM_END);
	mmap_add_region(BL_COHERENT_RAM_BASE, BL_COHERENT_RAM_BASE,
			BL_COHERENT_RAM_END - BL_COHERENT_RAM_BASE,
			MT_DEVICE | MT_RW | MT_SECURE);
#endif

#if defined(AMBARELLA_CV5)
#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_AUTO_DRAM_TRAINING) && !defined(CONFIG_BST_DRAM_TRAINING)
	/* remap the DRAM training region (RO) */
	VERBOSE("DRAM training region (RO): %p - %p\n",
		(void *)DRAM_TRAINING_RO_START, (void *)DRAM_TRAINING_RO_START);
	mmap_add_region(DRAM_TRAINING_RO_START, DRAM_TRAINING_RO_START,
			DRAM_TRAINING_RO_SIZE,
			MT_CODE | MT_SECURE);

	/* remap the DRAM training region (RW) */
	VERBOSE("DRAM training region (RW): %p - %p\n",
		(void *)DRAM_TRAINING_RW_START, (void *)DRAM_TRAINING_RW_START);
	mmap_add_region(DRAM_TRAINING_RW_START, DRAM_TRAINING_RW_START,
			DRAM_TRAINING_RW_SIZE,
			MT_MEMORY | MT_RW | MT_SECURE);

	/* remap the DRAM training region (stack) */
	VERBOSE("DRAM training region (stack): %p - %p\n",
		(void *)DRAM_TRAINING_STACK_START, (void *)DRAM_TRAINING_STACK_START);
	mmap_add_region(DRAM_TRAINING_STACK_START, DRAM_TRAINING_STACK_START,
			DRAM_TRAINING_STACK_SIZE,
			MT_MEMORY | MT_RW | MT_SECURE);
#endif
#endif

	/* peripherals device registers */
	mmap_add(ambarella_device_mmap);

	/* additional regions if needed */
	if (mmap)
		mmap_add(mmap);

	init_xlat_tables();
	enable_mmu(0);
}
