/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <plat/common/platform.h>

static const unsigned char plat_power_domain_tree_desc[] = {1, PLATFORM_CORE_COUNT};

const unsigned char *plat_get_power_domain_tree_desc(void)
{
	return plat_power_domain_tree_desc;
}

int plat_core_pos_by_mpidr(u_register_t mpidr)
{
	if (read_mpidr() & MPIDR_MT_MASK) {
		/* Cortex A76 */
		if (mpidr & (MPIDR_AFFLVL_MASK << MPIDR_AFF0_SHIFT))
			return -1;

		return (mpidr >> MPIDR_AFF1_SHIFT) & MPIDR_CPU_MASK;
	} else {
#if !defined(CONFIG_ATF_AMBALINK) && !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
		/* Cortex A53 */
		if (mpidr & MPIDR_CLUSTER_MASK)
			return -1;
#endif
		if ((mpidr & MPIDR_CPU_MASK) >= PLATFORM_CORE_COUNT)
			return -1;

		return mpidr & MPIDR_CPU_MASK;
	}
}

