/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <assert.h>
#include <errno.h>
#include <lib/utils_def.h>
#include <libfdt.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <plat/common/platform.h>
#include <plat_private.h>
#include <boot_cookie.h>

int32_t ambarella_fdt_init(void)
{
	boot_cookie_t *cookie = boot_cookie_ptr();
	uint64_t dtb_base;
	int32_t rval = 0;

	if (cookie->dtb_ram_start == 0)
		return -ENOENT;

	dtb_base = cookie->dtb_ram_start;

	rval = mmap_add_dynamic_region(dtb_base, dtb_base, AMBARELLA_MAX_DTB_SIZE,
					MT_MEMORY | MT_RW | MT_SECURE);
	assert(rval == 0);

	rval = fdt_check_header((void *)dtb_base);
	assert(rval == 0);

	INFO("Non Secure DTB at 0x%llx\n", dtb_base);

	return 0;
}

