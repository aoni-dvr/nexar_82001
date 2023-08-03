// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2016, Ambarella Inc. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

/* Make stacks aligned to data cache line length */
#define STACK_ALIGNMENT		64

#ifdef ARM64
#ifdef CFG_WITH_PAGER
#error "Pager not supported for ARM64"
#endif
#endif /* ARM64 */

/* Ambarella UART */
#ifndef CFG_UART_BASE
#if defined (ambarella_cv5) || defined(ambarella_cv52)
#define CONSOLE_UART_BASE	0x20E4000000UL /* UART0 */
#else
#define CONSOLE_UART_BASE	0xE4000000 /* UART0 */
#endif
#else
#define CONSOLE_UART_BASE	CFG_UART_BASE
#endif

#define CONSOLE_BAUDRATE	115200

#if defined (ambarella_cv5) || defined(ambarella_cv52)
#define CFG_TEE_CORE_NB_CORE	2
#else
#define CFG_TEE_CORE_NB_CORE	4
#endif

#if 1

#include <mm/generic_ram_layout.h> //Generic RAM layout configuration directives

/* EXAMPLE of cv2fs_svc_iout_optee_ambalink_defconfig
	cortex_a $ ./tools/hostutils/config.check.sh
	 +-----------------------------------+  0x00000000  (0 MB)
	 | Reserved memory                   |     Size: 0x00004000
	 +-----------------------------------+  0x00004000
	 | ATF (BL31)                        |     Size: 0x000FC000
	 +-----------------------------------+  0x00100000 (1 MB)
	 | ATF (BL32)                        |     Size: 0x00B00000 (11 MB)
	 +-----------------------------------+  0x00c00000 (12 MB)
	 | A53 ThreadX                       |     Size: 1648 MB
	 +-----------------------------------+  0x67c00000  (1660 MB)
	 | Linux Share memory                |     Size: 6 MB
	 +-----------------------------------+  0x68200000  (1666 MB)
	 | Linux DTB & MMU                   |     Size: 512 KB
	 +                                   +  0x68280000
	 | Linux Kernel                      |     Size: 122 MB - 512 KB
	 +-----------------------------------+  0x6fc00000
	 | ATF (BL32) BL32_SHMEM             |     Size: 0x400000 (4 MB)
	 +-----------------------------------+  0x70000000  (1792 MB)
	 | R52 ThreadX                       |     Size: 249 MB
	 +-----------------------------------+  0x7f900000  (2041 MB)
	 | R52 Share memory                  |     Size: 7 MB
	 | Shadow memory base                |  0x7fe00000  (2046 MB)
	 | TCM sram base (R52 OS)            |  0x7ff00000  (2047 MB)
	 +-----------------------------------+  0x80000000  (2048 MB)
	Loader:
	 R52BLD (Temporary)                     0x10000000  (256 MB)
	 BL2 BL3-Loader (Temporary)             0x01800000  (24 MB)
	 BL33 AmBoot (Temporary)                0x7D000000  (2000 MB)
 Example of BR2_TARGET_OPTEE_OS_ADDITIONAL_VARIABLES
	PLATFORM_ARCH=ambarella_cv2fs
  OPTEE secure RAM base address
	CFG_TZDRAM_START=0x00100000 = CONFIG_ATF_BL32_BASE = TZDRAM_BASE = TEE_RAM_START = TEE_LOAD_ADDR
  OPTEE secure RAM byte size
	CFG_TZDRAM_SIZE=0x00B00000 = CONFIG_ATF_BL32_SIZE = TZDRAM_SIZE
  Non-secure static shared memory physical base address
	CFG_SHMEM_START=0x6FC00000 = =A53_DDR_END-CONFIG_ATF_BL32_SHMEM_SIZE = TEE_SHMEM_START
  Non-secure static shared memory byte size
	CFG_SHMEM_SIZE=0x400000 = CONFIG_ATF_BL32_SHMEM_SIZE = TEE_SHMEM_SIZE
  OPTEE core OS RAM size (1MB)
	CFG_TEE_RAM_VA_SIZE=0x100000 = TEE_RAM_VA_SIZE = TEE_RAM_PH_SIZE
 */
#else
#if !defined(CFG_BL32_SHMEM_SIZE) || !defined(CFG_BL33_BASE)
#error "CFG_BL32_SHMEM_SIZE or CFG_BL33_BASE is not defined"
#endif

/*
 * Everything is in DDR memory.
 *
 * +---------+---------------------+
 * |         |     Linux memory    |
 * |         +---------------------+
 * |         |     Block Buffer    |
 * | NSDRAM  +---------------------+
 * |         |        BL33         |
 * +---------+------+--------------+  [CFG_BL33_BASE]
 * | EL2RSVD |   EL2 RSVDMEM 2MiB  |
 * +---------+------+--------------+  [EL2_RSVD_BASE]
 * | SHARED  |   BL32 SHMEM        |  [CFG_BL32_SHMEM_SIZE]
 * +---------+------+--------------+  [TEE_SHMEM_START]
 * |         |      | TA_RAM       |
 * |         | BL32 +--------------+  [TA_RAM_START]
 * |         |      | TEE_RAM 1MiB |
 * | TZDRAM  +------+--------------+ 0x0010_0000 [TZDRAM_BASE/TEE_RAM_START/TEE_LOAD_ADDR/BL32_BASE]
 * |         |        BL31         |
 * |         +---------------------+ 0x0008_0000
 * |         |        BL2          |
 * +---------+---------------------+ 0x0000_0000 [DRAM_SPACE_START]
 *
 */
#define DRAM_SPACE_START	0x00000000

#define BL2_SIZE		0x00080000
#define BL31_SIZE		0x00080000
#define BL32_SIZE		0x00100000
#if defined (ambarella_cv5) || defined(ambarella_cv52)
#define EL2_RSVD_SIZE		0x00400000
#else
#define EL2_RSVD_SIZE		0x00200000
#endif

/* CFG_BL32_SHMEM_SIZE needs to be 2MB aligned */
#define TEE_SHMEM_SIZE		CFG_BL32_SHMEM_SIZE
#define BL33_BASE		CFG_BL33_BASE

#define TZDRAM_BASE		(DRAM_SPACE_START + BL2_SIZE + BL31_SIZE)
#define TEE_SHMEM_START		(BL33_BASE - EL2_RSVD_SIZE - TEE_SHMEM_SIZE)
#define TZDRAM_SIZE		(TEE_SHMEM_START - TZDRAM_BASE)

#define TEE_RAM_START		TZDRAM_BASE
#define TEE_LOAD_ADDR		TZDRAM_BASE
#define TEE_RAM_VA_SIZE		BL32_SIZE
#define TEE_RAM_PH_SIZE		BL32_SIZE

#define TA_RAM_START		ROUNDUP((TZDRAM_BASE + TEE_RAM_VA_SIZE), \
					CORE_MMU_PGDIR_SIZE)
#define TA_RAM_SIZE		ROUNDDOWN((TZDRAM_SIZE - TEE_RAM_VA_SIZE), \
					CORE_MMU_PGDIR_SIZE)
#endif	//#if 1

#endif /* PLATFORM_CONFIG_H */
