/**
 * bld/arm64/armv8_switch.S
 *
 * History:
 *    2015/11/20 - [Jorney Tu] AARCH64
 *
 * Copyright (c) 2016 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#if 0
#include <bldfunc.h>
#include <smccc.h>
#include <ambhw/cortex.h>
#include <ambhw/rct.h>
#include <ambhw/gic.h>
#else
#include <ambtw.h>
#endif
#include <macro.S>

	.global __switch_to_el2
	.global __switch_to_el1
	.global __switch_el1t_to_el1h
	.global __secondary_gic_init
	.global __disable_mmu
	.global __setup_aarch64_cntfrq
	.global __arm_smccc_smc

	.global  el3_common_cpu_init
	.global amboot_psci_exit
	.global amboot_power_down_wfi

el3_common_cpu_init:
	ldr	x0, =0x30d00800
	msr	sctlr_el1, x0
	msr	actlr_el1, xzr

	ldr	x0, =0x30080000000
	msr	hcr_el2, x0
	ldr	x0, =0x33ff
	msr	cptr_el2, x0

	ldr	x0, =0x3
	msr	cnthctl_el2, x0
	msr	cntvoff_el2, xzr
	msr	vttbr_el2, xzr
	ldr	x0, =0x6
	msr	mdcr_el2, x0
	msr	hstr_el2, xzr
	msr	cnthp_ctl_el2, xzr

	mrs	x0, midr_el1
	msr	vpidr_el2, x0
	mrs	x0, mpidr_el1
	msr	vmpidr_el2, x0
	ret

amboot_psci_exit:
#if defined(AMBOOT_HANDLE_SMC_CALLING)
	bl	el3_common_cpu_init
	smc_restore_and_exit
#endif
	b	.

amboot_power_down_wfi:
	dsb	sy
	wfi
	b	.


__switch_to_el2:
	/* Non-secure EL0/EL1 | HVC | 64bit EL2 */
	mov	x0, #0x531
	msr	scr_el3, x0

	/* Disable coprocessor traps to EL3 */
	msr	cptr_el3, xzr
	/* Disable coprocessor traps to EL2 */
	mov	x0, #0x33ff
	msr	cptr_el2, x0

	/* clear counter-timer virtual offset */
	msr	cntvoff_el2, xzr

	ldr	x0, =0x30C50830
	msr	sctlr_el2, x0

	/* Migrate SP and VBAR */
	mov	x0, sp
	msr	sp_el2, x0
	mrs	x0, vbar_el3
	msr	vbar_el2, x0

	mov	x0, #0x3c9
	msr	spsr_el3, x0

	msr	elr_el3, x30
	eret


__switch_to_el1:
	/* Enable EL1 access to timers */
	mrs	x0, cnthctl_el2
	orr	x0, x0, #0x3
	msr	cnthctl_el2, x0
	/* clear counter-timer virtual offset */
	msr	cntvoff_el2, xzr

	/* Initilize MIDR/MPIDR registers */
	mrs	x0, midr_el1
	msr	vpidr_el2, x0
	mrs	x0, mpidr_el1
	msr	vmpidr_el2, x0

	/* Disable coprocessor traps to EL2 */
	mov	x0, #0x33ff
	msr	cptr_el2, x0
	/* Disable AArch32 coprocessor traps to EL2 */
	msr	hstr_el2, xzr

	/* Enable FP/SIMD at EL1 */
	mov	x0, #(0x3 << 20)
	msr	cpacr_el1, x0

	ldr	x0, =0x30d00800
	msr	sctlr_el1, x0

	/* Migrate SP and VBAR */
	mov	x0, sp
	msr	sp_el1, x0
	mrs	x0, vbar_el2
	msr	vbar_el1, x0

#ifdef CONFIG_ARMV8_AARCH32
	/*AArch32 EL1 */
	mov	x0, #(0x1 << 29)
	mov	x1, #0x1d3
#else
	/* AArch64 EL1 */
	mov	x0, #(0x5 << 29)
	mov	x1, #0x3c5
#endif
	msr	hcr_el2, x0
	msr	spsr_el2, x1

#ifdef CONFIG_ARMV8_AARCH32
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, x28			/* dtb address */
	mov	x3, xzr
#else
	mov	x0, x28			/* dtb address */
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
#endif
	msr	elr_el2, x29
	eret


__switch_el1t_to_el1h:
#ifdef CONFIG_ARMV8_AARCH32
	ldr	x0, =SMCCC_SIP_CALL(AMBA_SIP_SWITCH_TO_AARCH32, AMBA_SIP_AARCH32_KERNEL)
	mov	x1, x29			/* kernel entry */
	mov	x2, x28			/* dtb address */
	mov	x3, xzr
	mov	x4, xzr
	smc	#0
	/* never return here */
	b	.
#else
	mov	x0, x28			/* dtb address */
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	mov	x20, #0x3c5
#endif
	msr	spsr_el1, x20
	msr	elr_el1, x29
	eret

__secondary_gic_init:
#if (AXI_SUPPORT_SECURITY > 0)
	ldr	x0, =GICD_BASE
	mov	w1, 0xffffffff
	str	w1, [x0, GICD_IGROUPR]
	ldr	x0, =GICC_BASE
	mov	w1, #0x1e7
	str	w1, [x0, GICC_CTRL]
	mov	w1, #0xf0
	str	w1, [x0, GICC_PMR]
#endif
	ret


__disable_mmu:
	armv8_switch_el x1, 1f, 2f, 3f
1:	mrs	x0, sctlr_el1
	bic	x0, x0, #0x1
	msr	sctlr_el1, x0
	b	4f
2:	b	.
3:	mrs	x0, sctlr_el3
	bic	x0, x0, #0x1
	msr	sctlr_el3, x0
4:	ret


__setup_aarch64_cntfrq:
#ifndef CONFIG_AARCH64_TRUSTZONE
#if (AXI_SYS_TIMER_INDEPENDENT == 1)
	ldr	x0, =PLL_ENET_CTRL_REG
#else
	ldr	x0, =PLL_CORTEX_CTRL_REG
#endif
	ldr	w1, [x0]
	ubfx	w2, w1, #24, #7
	add	w2, w2, #1		/* intprog */
	ubfx	w3, w1, #16, #4
	add	w3, w3, #1		/* sout */
	ubfx	w4, w1, #12, #4
	add	w4, w4, #1		/* sdiv */

	ldr	w0, =REF_CLK_FREQ
	umull	x0, w0, w2
	udiv	x0, x0, x3
	umull	x0, w0, w4

	ldr	w2, =AXI_SYS_TIMER_DIVISOR
	udiv	x0, x0, x2

#if (SCALER_SYS_CNT_POST_REG == RCT_INVALID_REG)
	ldr	x1, =AXI_SYS_TIMER_REG
	ldr	w2, [x1]
	ubfx	w3, w2, #1, #4
	lsr	x0, x0, x3
#endif

	msr	cntfrq_el0, x0
#endif
	ret


__arm_smccc_smc:
	.cfi_startproc
	smc	#0
	ldr	x4, [sp]		/* the last item, i.e., struct smccc_result *res */
	cbz	x4, 1f
	stp	x0, x1, [x4, #0x00]
	stp	x2, x3, [x4, #0x10]
1:	ret
	.cfi_endproc


/*-----------------------------------------------------------------------------
 * Name:	Writer
 * Purpose:	Performs sequential write into memory, as fast as possible.
 * Params:
 *	x0 = address
 *	x1 = length, multiple of 256
 *	x2 = loop
 *-----------------------------------------------------------------------------*/
.globl Writer
	Writer:
	bic	x1, x1, #0x7f
	mov	x4, x0
	mov	x5, x1

	/*
	 * x4 = temp address
	 * x5 = temp length
	 */

.L01:
	mov	x0, x4
	mov	x1, x5

.L11:
	stp x3, x6, [x0]
	stp x3, x6, [x0, #16]
	stp x3, x6, [x0, #32]
	stp x3, x6, [x0, #48]
	stp x3, x6, [x0, #64]
	stp x3, x6, [x0, #80]
	stp x3, x6, [x0, #96]
	stp x3, x6, [x0, #112]
	stp x3, x6, [x0, #128]
	stp x3, x6, [x0, #144]
	stp x3, x6, [x0, #160]
	stp x3, x6, [x0, #176]
	stp x3, x6, [x0, #192]
	stp x3, x6, [x0, #208]
	stp x3, x6, [x0, #224]
	stp x3, x6, [x0, #240]
	add x0, x0, #256

	sub	x1, x1, #256
	cbnz	x1, .L11

	sub	x2, x2, #1
	cbnz	x2, .L01

	ret
