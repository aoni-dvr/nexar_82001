/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <drivers/console.h>
#include <common/desc_image_load.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <plat/common/platform.h>
#include <plat_private.h>
#include <uart_ambarella.h>
#include <boot_cookie.h>


static console_t bl2_console;

void bl2_el3_early_platform_setup(u_register_t x0, u_register_t x1,
				  u_register_t x2, u_register_t x3)
{
	boot_cookie_t *cookie;

	/* Initialize the console to provide early debug support */
	console_ambarella_register(UART0_BASE, UART_CLOCK, UART_BAUDRATE, &bl2_console);
	printf("\x1b[4l\r\n");	/* Set terminal to replacement mode */

	cookie = boot_cookie_init();
	assert(cookie);
}

void bl2_el3_plat_arch_setup(void)
{
	bl_mem_params_node_t *bl_mem_params;
	boot_cookie_t *cookie = boot_cookie_ptr();
	int rval;

#if defined(AMBARELLA_CV5)
#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_AUTO_DRAM_TRAINING) && !defined(CONFIG_BST_DRAM_TRAINING)
    /* This part of code was in BLD originally.
     * However, with BL2 enabled, to speed-up training with MMU enabled (cacheable),
     * we need to move code/data to SRAM in this early stage.
     *
     * After the moving is done,
     * MMU would be enabled with
     *  - DRAM training text/rodata section (in SRAM) set as RO EXECUTABLE.
     *  - DRAM training data/bss section (in SRAM) set as RW.
     * */

    extern uint64_t dram_training_ld_addr;
    extern uint64_t dram_training_ex_addr;
    extern uint64_t __ddrc_training_lp5_size;
    uint64_t* pTemp = &dram_training_ex_addr;
    uint64_t* pTempData = &dram_training_ld_addr;
    int i;

    /* move tcm data to SRAM */
    for (i = 0U; i < ((uint64_t)(&__ddrc_training_lp5_size) >> 2U); i++) {
        pTemp[i] = pTempData[i];
    }
#endif
#endif

	ambarella_mmap_setup(NULL);

	/* setup BL33 image info */
	bl_mem_params = get_bl_mem_params_node(BL33_IMAGE_ID);
	bl_mem_params->image_info.image_base = cookie->bld_ram_start;
	bl_mem_params->ep_info.pc = cookie->bld_ram_start;

	/* setup SOC_FW_CONFIG image info */
	bl_mem_params = get_bl_mem_params_node(SOC_FW_CONFIG_ID);
	bl_mem_params->image_info.image_base = cookie->dtb_ram_start;
	if (bl_mem_params->image_info.image_base == 0)
		bl_mem_params->image_info.h.attr = IMAGE_ATTRIB_SKIP_LOADING;

	rval = ambarella_io_setup();
	if (rval) {
		ERROR("failed to setup io devices: %d\n", rval);
		panic();
	}
#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_AUTO_DRAM_TRAINING) && !defined(CONFIG_BST_DRAM_TRAINING)
	{
		int ambarella_read_dramc_param(uint32_t offset, void *buff, uint32_t size);
		void BL2_CheckTrainingResult(int (*ambarella_read_dramc_param)(unsigned int offset, void *buff, unsigned int size));

		BL2_CheckTrainingResult(ambarella_read_dramc_param);
	}
#endif
}

void bl2_platform_setup(void)
{
}

void plat_flush_next_bl_params(void)
{
	flush_bl_params_desc();
}

bl_load_info_t *plat_get_bl_image_load_info(void)
{
	return get_bl_load_info_from_mem_params_desc();
}

bl_params_t *plat_get_next_bl_params(void)
{
	return get_next_bl_params_from_mem_params_desc();
}

int bl2_plat_handle_pre_image_load(unsigned int image_id)
{
	struct image_info *image_info;
	int ret = 0;

	image_info = &get_bl_mem_params_node(image_id)->image_info;

	if (image_info->h.attr & IMAGE_ATTRIB_SKIP_LOADING)
		return 0;

	ret = mmap_add_dynamic_region(image_info->image_base,
				      image_info->image_base,
				      image_info->image_max_size,
				      MT_MEMORY | MT_RW | MT_SECURE);
	if (ret < 0) {
		ERROR("map memory failed(%d) for IMAGE_ID(%d): 0x%lx, 0x%x\n", ret,
			image_id, image_info->image_base, image_info->image_max_size);
	}

	return ret;
}

int bl2_plat_handle_post_image_load(unsigned int image_id)
{
	boot_cookie_t *cookie = boot_cookie_ptr();
	uintptr_t recovery_flag;

	switch (image_id) {
	case BL33_IMAGE_ID:
		/* let BLD to know it's boot from recovery */
		recovery_flag = cookie->bld_ram_start + DEVFW_FLAG_OFFSET;
		if (cookie->bak_bld_media_start)
			*(uint32_t *)recovery_flag = DEVFW_MAGIC;
		break;
	}

	return 0;
}

