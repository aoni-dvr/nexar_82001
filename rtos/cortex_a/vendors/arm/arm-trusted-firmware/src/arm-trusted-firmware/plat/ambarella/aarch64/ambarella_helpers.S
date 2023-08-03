/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <asm_macros.S>
#include <platform_def.h>

#if defined(CONFIG_ATF_AMBALINK)
#if WARMBOOT_ENABLE_DCACHE_EARLY
/* ---------------------
 * Common CPU init code, cf. tegra_helpers.S
 * ---------------------
 */
/*******************************************************************************
 * Implementation defined ACTLR_EL3 bit definitions
 ******************************************************************************/
#define ACTLR_EL3_L2ACTLR_BIT		(U(1) << 6)
#define ACTLR_EL3_L2ECTLR_BIT		(U(1) << 5)
#define ACTLR_EL3_L2CTLR_BIT		(U(1) << 4)
#define ACTLR_EL3_CPUECTLR_BIT		(U(1) << 1)
#define ACTLR_EL3_CPUACTLR_BIT		(U(1) << 0)
#define ACTLR_EL3_ENABLE_ALL_MASK	(ACTLR_EL3_L2ACTLR_BIT | \
								ACTLR_EL3_L2ECTLR_BIT | \
					 			ACTLR_EL3_L2CTLR_BIT | \
					 			ACTLR_EL3_CPUECTLR_BIT | \
					 			ACTLR_EL3_CPUACTLR_BIT)
#define ACTLR_EL3_ENABLE_ALL_ACCESS	(ACTLR_EL3_L2ACTLR_BIT | \
					 			ACTLR_EL3_L2ECTLR_BIT | \
					 			ACTLR_EL3_L2CTLR_BIT | \
					 			ACTLR_EL3_CPUECTLR_BIT | \
					 			ACTLR_EL3_CPUACTLR_BIT)
#endif /* WARMBOOT_ENABLE_DCACHE_EARLY */
#endif /* CONFIG_ATF_AMBALINK */
	.globl	plat_my_core_pos
	.globl	platform_mem_init
	.globl	plat_reset_handler
	.globl	plat_crash_console_init
	.globl	plat_crash_console_putc
	.globl	plat_crash_console_flush
	.globl	plat_get_my_entrypoint
	.globl	ambarella_sec_entrypoint

#if defined(CONFIG_ATF_AMBALINK)
#if WARMBOOT_ENABLE_DCACHE_EARLY
	/* ---------------------
	 * Common CPU init code, cf. tegra_helpers.S
	 * ---------------------
	 */
.macro	cpu_init_common

	/* -------------------------------------------------------
	 * Enable L2 and CPU ECTLR RW access from non-secure world
	 * -------------------------------------------------------
	 */
	mrs	x0, actlr_el3
	mov	x1, #ACTLR_EL3_ENABLE_ALL_MASK
	bic	x0, x0, x1
	mov	x1, #ACTLR_EL3_ENABLE_ALL_ACCESS
	orr	x0, x0, x1
	msr	actlr_el3, x0
	mrs	x0, actlr_el2
	mov	x1, #ACTLR_EL3_ENABLE_ALL_MASK
	bic	x0, x0, x1
	mov	x1, #ACTLR_EL3_ENABLE_ALL_ACCESS
	orr	x0, x0, x1
	msr	actlr_el2, x0
	isb
.endm
#endif /* WARMBOOT_ENABLE_DCACHE_EARLY */
#endif /* CONFIG_ATF_AMBALINK */

	/* -----------------------------------------------------
	 *  unsigned int plat_my_core_pos(void)
	 *  This function uses the plat_arm_calc_core_pos()
	 *  definition to get the index of the calling CPU.
	 * -----------------------------------------------------
	 */
func plat_my_core_pos
	mrs	x0, mpidr_el1
	b	plat_arm_calc_core_pos
endfunc plat_my_core_pos


	/* -----------------------------------------------------
	 *  unsigned int plat_arm_calc_core_pos(u_register_t mpidr)
	 *  Helper function to calculate the core position.
	 *  With this function for CA53: CorePos = (ClusterId * 4) + CoreId(AFF0)
	 *  With this function for CA76: CorePos = CoreID (AFF1)
	 *
	 *  In ARMv8.2, AFF2 is cluster id, AFF1 is core id and
	 *  AFF0 is thread id. There is only one cluster in ARMv8.2
	 *  and one thread in current implementation.
	 *  we do it with x0 = (x0 >> 8) & 0xff
	 * -----------------------------------------------------
	 */
func plat_arm_calc_core_pos
	tbz	x0, #24, 1f		/* MT bit */
	mov	x1, #MPIDR_AFFLVL_MASK
	and	x0, x1, x0, lsr #MPIDR_AFF1_SHIFT
	b	2f
1:	and	x1, x0, #MPIDR_CPU_MASK
	and	x0, x0, #MPIDR_CLUSTER_MASK
#if defined(CONFIG_ATF_AMBALINK) && (defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS))
	/* cv2fs core0=0x100->0x0, core3=0x103->0x3*/
	sub	x0, x0, #0x100
#endif
	add	x0, x1, x0, LSR #6
2:	ret
endfunc plat_arm_calc_core_pos


	/* -----------------------------------------------------
	 * void platform_mem_init(void);
	 * Setup the secure memory region if secure boot is enabled.
	 * -----------------------------------------------------
	 */
func platform_mem_init
	ret
endfunc platform_mem_init


	/* -----------------------------------------------------
	 * void plat_reset_handler(void);
	 * -----------------------------------------------------
	 */
func plat_reset_handler
#if defined(CONFIG_ATF_AMBALINK)
#if WARMBOOT_ENABLE_DCACHE_EARLY
	cpu_init_common
#endif
#endif
	b	plat_crash_console_init
	ret
endfunc plat_reset_handler


	/* ---------------------------------------------
	 * int plat_crash_console_init(void)
	 * Function to initialize the crash console
	 * without a C Runtime to print crash report.
	 * Clobber list : x0, x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_init
	mov_imm	x0, UART0_BASE
	mov_imm	x1, UART_CLOCK
	mov_imm	x2, UART_BAUDRATE
	b	console_ambarella_core_init
	ret
endfunc plat_crash_console_init


	/* ---------------------------------------------
	 * int plat_crash_console_putc(void)
	 * Function to print a character on the crash
	 * console without a C Runtime.
	 * Clobber list : x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_putc
	mov_imm x1, UART0_BASE
	b	console_ambarella_core_putc
	ret
endfunc plat_crash_console_putc


	/* ---------------------------------------------
	 * int plat_crash_console_flush()
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * Out : return -1 on error else return 0.
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func plat_crash_console_flush
	mov	x0, #0
	ret
endfunc plat_crash_console_flush


	/* -----------------------------------------------------
	 * unsigned long plat_get_my_entrypoint (void);
	 *
	 * Main job of this routine is to distinguish between
	 * a cold and warm boot. If the ambarella_sec_entrypoint for
	 * this CPU is present, then it's a warm boot.
	 *
	 * -----------------------------------------------------
	 */
func plat_get_my_entrypoint
	adr	x1, ambarella_sec_entrypoint
	ldr	x0, [x1]
	ret
endfunc plat_get_my_entrypoint


/* data section */
.data
.align	3

ambarella_sec_entrypoint:
	.quad	0

