/*
 * Copyright (c) 2014-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#include <arch.h>
#include <common/tbbr/tbbr_img_def.h>
#include <lib/utils_def.h>
#include <plat/common/common_def.h>
#include "ambarella_def.h"

#define PLATFORM_LINKER_FORMAT		"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH		aarch64

/******************************************************************************
 * Generic platform constants
 ******************************************************************************/

/*
 * Boot memory layout
 *
 * + ------ + ------------------ +
 * |        |         BL2        |
 * |        + ------------------ +
 * | S-MEM  |         BL31       |
 * |        + ------------------ + - - -
 * |        |    BL32 Image      |       \
 * |        + ------------------ +        \
 * |        |    BL32 TZRAM      |         + BL32 is an optional stage for secure boot
 * + ------ + ------------------ +        /
 * |        | BL32 shared memory |       /
 * |        + ------------------ + - - -
 * |        | EL2 Reserved memory|
 * | NS-MEM + ------------------ +
 * |        |   Bootloader(BL33) |
 * |        + ------------------ +
 * |        |   Block buffer     |
 * +        + ------------------ +
 * |        |       ...          |
 * + ------ + ------------------ +
 *
 * Note: If BL32 is enabled, this memory layout must be consistent with optee_os.
 *       Please check BR2_TARGET_OPTEE_OS_ADDITIONAL_VARIABLES
 */
#if defined(CONFIG_ATF_AMBALINK)
#define BL2_SIZE			CONFIG_ATF_BL2_SIZE
#define BL31_SIZE			CONFIG_ATF_BL31_SIZE
#else
#define BL2_SIZE			0x00080000
#define BL31_SIZE			0x00080000
#endif

#ifndef SPD_none
#if defined(CONFIG_ATF_AMBALINK)
#define BL32_SHMEM_SIZE			CONFIG_ATF_BL32_SHMEM_SIZE
#define BL32_SIZE			CONFIG_ATF_BL32_SIZE	/* This is max size of bl32 image */
#else
#define BL32_SHMEM_SIZE			PLAT_CFG_BL32_SHMEM_SIZE
#define BL32_SIZE			0x00100000		/* This is max size of bl32 image */
#endif
#else
#define BL32_SHMEM_SIZE			0x00000000
#define BL32_SIZE			0x00000000
#endif

#if defined(AMBARELLA_CV5)
#define EL2_RSVD_SIZE			0x00400000
#else
#define EL2_RSVD_SIZE			0x00200000
#endif

#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_ATF_BL2_BASE)
#define BL2_BASE			CONFIG_ATF_BL2_BASE
#else
#define BL2_BASE			(DRAM_SPACE_START)
#endif
#define BL2_LIMIT			(BL2_BASE + BL2_SIZE)

#if defined(CONFIG_ATF_AMBALINK)
#define BL31_BASE			(CONFIG_ATF_BL31_BASE)
#else
#define BL31_BASE			BL2_LIMIT
#endif
#define BL31_LIMIT			(BL31_BASE + BL31_SIZE)

#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_ATF_BL32_BASE)
#define BL32_BASE			CONFIG_ATF_BL32_BASE
#else
#define BL32_BASE			BL31_LIMIT
#endif
#define BL32_LIMIT			(BL32_BASE + BL32_SIZE)

#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_ATF_BL33_BLD_BASE)
#define BL33_BASE			CONFIG_ATF_BL33_BLD_BASE
#elif defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_ATF_BL33_BASE)
#define BL33_BASE			CONFIG_ATF_BL33_BASE
#else
#define BL33_BASE			PLAT_CFG_BL33_BASE
#endif

/* Size of cacheable stacks */
#define PLATFORM_STACK_SIZE		0x1000
#if defined(AMBARELLA_CV5)
#define PLATFORM_CORE_COUNT		U(2)
#else
#define PLATFORM_CORE_COUNT		U(4)
#endif
#define PLAT_MAX_PWR_LVL		MPIDR_AFFLVL1
#define PLAT_MAX_RET_STATE		U(1)
#define PLAT_MAX_OFF_STATE		U(2)

/* IO framework user */
#define MAX_IO_HANDLES			2
#define MAX_IO_DEVICES			2
#define MAX_IO_BLOCK_DEVICES		1

/* Block buffer: Non-secure memory, used by io storage in BL2 stage. */
#define AMBARELLA_BLOCK_BUF_SIZE	0x080000
#define AMBARELLA_MAX_BLD_SIZE		0x200000
#define AMBARELLA_MAX_DTB_SIZE		0x080000

/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#if defined(AMBARELLA_CV5)
#define VTCR_EL2_FLAG			0x80023559
#define PLAT_PHY_ADDR_SPACE_SIZE	(1ULL << 39)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ULL << 39)
#else
#define VTCR_EL2_FLAG			0x80003560
#define PLAT_PHY_ADDR_SPACE_SIZE	(1ULL << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ULL << 32)
#endif

#if defined(CONFIG_ATF_AMBALINK) && !defined(CONFIG_ATF_HAVE_BL2) /* reduce size of xlat_table */
#define MAX_MMAP_REGIONS		0x10
#define MAX_XLAT_TABLES			0x10
#else
#define MAX_MMAP_REGIONS		0x20
#define MAX_XLAT_TABLES			0x20
#endif

#define CACHE_WRITEBACK_SHIFT		6
#define CACHE_WRITEBACK_GRANULE		(1 << CACHE_WRITEBACK_SHIFT)


/*******************************************************************************
 * GIC/IRQ/FIO Definition
 ******************************************************************************/

#define ARM_IRQ_VIRT_MAINT		25
#define ARM_IRQ_HYP_TIMER		26
#define ARM_IRQ_VIRT_TIMER		27
#define ARM_IRQ_LEGACY_FIQ		28
#define ARM_IRQ_SEC_PHY_TIMER		29
#define ARM_IRQ_NON_SEC_PHY_TIMER	30
#define ARM_IRQ_LEGACY_IRQ		31

#define ARM_IRQ_SEC_SGI_0		8
#define ARM_IRQ_SEC_SGI_1		9
#define ARM_IRQ_SEC_SGI_2		10
#define ARM_IRQ_SEC_SGI_3		11
#define ARM_IRQ_SEC_SGI_4		12
#define ARM_IRQ_SEC_SGI_5		13
#define ARM_IRQ_SEC_SGI_6		14
#define ARM_IRQ_SEC_SGI_7		15

/*
 * Define properties of Group 1 Secure and Group 0 interrupts as per GICv3
 * terminology. On a GICv2 system or mode, the lists will be merged and treated
 * as Group 0 interrupts.
 */
#define AMBARELLA_G1S_IRQ_PROPS(grp) \
	INTR_PROP_DESC(ARM_IRQ_SEC_PHY_TIMER, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_LEVEL), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_0, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_1, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_2, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_3, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_4, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_5, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_6, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE), \
	INTR_PROP_DESC(ARM_IRQ_SEC_SGI_7, GIC_HIGHEST_SEC_PRIORITY, grp, \
			GIC_INTR_CFG_EDGE)

#define AMBARELLA_G0_IRQ_PROPS(grp)

#endif /* __PLATFORM_DEF_H__ */

