/**
 * include/macro.S
 *
 * Author: Anthony Ginger <hfjiang@ambarella.com>
 *
 * History:
 *    2015/11/26 - [Cao Rongrong] Add ARMv8 supported
 *
 * Copyright (c) 2015 Ambarella International LP
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

#if defined(__aarch64__)

#include <smccc_handler.h>

.macro smc_save_gp_registers
	stp	x0, x1, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X0]
	stp	x2, x3, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X2]
	stp	x4, x5, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X4]
	stp	x6, x7, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X6]
	stp	x8, x9, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X8]
	stp	x10, x11, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X10]
	stp	x12, x13, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X12]
	stp	x14, x15, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X14]
	stp	x16, x17, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X16]
	stp	x18, x19, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X18]
	stp	x20, x21, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X20]
	stp	x22, x23, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X22]
	stp	x24, x25, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X24]
	stp	x26, x27, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X26]
	stp	x28, x29, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X28]

	mrs	x18, sp_el0
	str	x18, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_SP_EL0]
.endm

.macro smc_restore_gp_registers
	ldp	x0, x1, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X0]
	ldp	x2, x3, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X2]
	ldp	x4, x5, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X4]
	ldp	x6, x7, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X6]
	ldp	x8, x9, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X8]
	ldp	x10, x11, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X10]
	ldp	x12, x13, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X12]
	ldp	x14, x15, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X14]
	ldp	x16, x17, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X16]
	ldp	x18, x19, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X18]
	ldp	x20, x21, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X20]
	ldp	x22, x23, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X22]
	ldp	x24, x25, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X24]
	ldp	x26, x27, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X26]
	ldr	x28, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_SP_EL0]
	msr	sp_el0, x28
	ldp	x28, x29, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_X28]
.endm

.macro smc_restore_and_exit
	/* -----------------------------------------------------
	 * Save the current SP_EL0 i.e. the EL3 runtime stack
	 * which will be used for handling the next SMC. Then
	 * switch to SP_EL3
	 * -----------------------------------------------------
	 */
	mov	x17, sp		/* SP_EL0 */
	msr	spsel, #1
	str	x17, [sp, #CTX_EL3STATE_OFFSET + CTX_RUNTIME_SP]

	/* -----------------------------------------------------
	 * Restore SPSR_EL3, ELR_EL3 and SCR_EL3 prior to ERET
	 * -----------------------------------------------------
	 */
	ldr	x18, [sp, #CTX_EL3STATE_OFFSET + CTX_SCR_EL3]
	ldp	x16, x17, [sp, #CTX_EL3STATE_OFFSET + CTX_SPSR_EL3]
	msr	scr_el3, x18
	msr	spsr_el3, x16
	msr	elr_el3, x17

	smc_restore_gp_registers

	ldr	x30, [sp, #CTX_GPREGS_OFFSET + CTX_GPREG_LR]
	eret
.endm

/*
 *	^ ------ ^
 *	|  spsr  |
 *	* ------ *
 *	| elr_elx|
 *	* ------ *
 *	| dummy  |	used for padding to have even number of registers.
 *	* ------ *
 *	|   x0   |
 *	* ------ *
 *	|   x1   |
 *	* ------ *
 *	|  ....  |
 *	* ------ *
 *	|   x30  |
 *	* ------ *
 */

.macro	save_context
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!

	armv8_switch_el x1, 1f, 2f, 3f
1:	mrs	x2, elr_el1
	mrs	x3, spsr_el1
	b	0f
2:	b	.
3:	mrs	x2, elr_el3
	mrs	x3, spsr_el3
0:	stp	xzr, x0, [sp, #-16]!
	stp	x3, x2, [sp, #-16]!

	stp	q0, q1, [sp, #-32]!
	stp	q2, q3, [sp, #-32]!
	stp	q4, q5, [sp, #-32]!
	stp	q6, q7, [sp, #-32]!
	stp	q8, q9, [sp, #-32]!
	stp	q10, q11, [sp, #-32]!
	stp	q12, q13, [sp, #-32]!
	stp	q14, q15, [sp, #-32]!
	stp	q16, q17, [sp, #-32]!
	stp	q18, q19, [sp, #-32]!
	stp	q20, q21, [sp, #-32]!
	stp	q22, q23, [sp, #-32]!
	stp	q24, q25, [sp, #-32]!
	stp	q26, q27, [sp, #-32]!
	stp	q28, q29, [sp, #-32]!
	stp	q30, q31, [sp, #-32]!
	mrs	x0, fpsr
	mrs	x1, fpcr
	stp	x0, x1, [sp, #-16]!

.endm

.macro	restore_context

	ldp	x0, x1, [sp], #16
	msr	fpcr, x0
	msr	fpsr, x1
	ldp	q30, q31, [sp], #32
	ldp	q28, q29, [sp], #32
	ldp	q26, q27, [sp], #32
	ldp	q24, q25, [sp], #32
	ldp	q22, q23, [sp], #32
	ldp	q20, q21, [sp], #32
	ldp	q18, q19, [sp], #32
	ldp	q16, q17, [sp], #32
	ldp	q14, q15, [sp], #32
	ldp	q12, q13, [sp], #32
	ldp	q10, q11, [sp], #32
	ldp	q8, q9, [sp], #32
	ldp	q6, q7, [sp], #32
	ldp	q4, q5, [sp], #32
	ldp	q2, q3, [sp], #32
	ldp	q0, q1, [sp], #32

	ldp	x3, x2, [sp], #16
	ldp	x1, x0, [sp], #16

	armv8_switch_el x1, 1f, 2f, 3f
1:	msr	spsr_el1, x3
	msr	elr_el1, x2
	b	0f
2:	b	.
3:	msr	spsr_el3, x3
	msr	elr_el3, x2

0:	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
.endm

/*
 * CLIDR_EL1: Identifies the type of caches, Read Only
 *
 * 31 30   27  24 23 21 20  18 17  15 14  12 11   9 8    6 5    3 2    0
 * ^----^----^---^-----^------^------^------^------^------^------^------^
 * |RES0|LoUU|LoC|LoUIS|Ctype7|Ctype6|Ctype5|Ctype4|Ctype3|Ctype2|Ctype1|
 *  ---------------------------------------------------------------------
 *
 *
 *
 * CSSELR_EL1: Select the current Cache Size ID Register
 *
 * 31                                                    4 3     1    0
 * ^------------------------------------------------------^--------^-----^
 * |                              RES0                    |  Level | InD |
 *  ---------------------------------------------------------------------
 *
 *
 * CCSIDR_EL1: Provide information of currently selected cache, Read Only
 *
 * 31   30   29   28	27            13 12                   3 2         0
 * ^----^----^----^----^----------------^----------------------^----------^
 * | WT | WB | RA | WA |     NumSets    |     Associativity    | Linesize |
 *  ---- ---- ---- ---- ---------------- ---------------------- ----------
 *
 * Linesize = log2(line size in bytes) - 4
 * Associativity = ways - 1
 * NumSets = sets - 1
 *
 *
 * DC ISW or DC CISW:
 *
 *  63         32 31   32-A  32-A-1    B B-1    L L-1     4 3    1   0
 * ^-------------^----------^-----------^--------^---------^------^-----^
 * |     RES0    |    Way   |    RES0   |   Set  |   RES0  | Level| RES0|
 *  --------------------------------------------------------------------
 *
 * A = log2(Associativity)
 * L = log2(Linesize)
 * S = log2(Set)
 * B = L + S
 *
 */

.macro	operate_all_cache, op, lvl, type, line, way, set, \
				wayp, wayv, cp_val, dc_val, tmp
	dsb	sy

	mov	\lvl, #0		/* lvl = level variable */

/* loop_level */
1:
	mrs	\cp_val, clidr_el1	/* read clidr_el1 to check Ctype */
	lsl	\tmp, \lvl, #1
	add	\tmp, \tmp, \lvl	/* tmp = lvl << 1 + lvl = lvl * 3 */
	lsr	\type, \cp_val, \tmp
	and	\type, \type, #7	/* Cache type */
	cbz	\type, 5f		/* return if no cache, no need to check upper layer */
	cmp	\type, #2
	b.lt	4f			/* skip if icache */

	/*---------- START Specified Level Operation -----------*/

	lsl	\tmp, \lvl, #1
	msr	csselr_el1, \tmp	/* Selects the current Cache level */
	isb				/* make sure csselr_el1 has taken effect */

	mrs	\cp_val, ccsidr_el1
	and	\line, \cp_val, #7
	add	\line, \line, #4	/* line = log2(cache line size) */
	lsr	\way, \cp_val, #3
	and	\way, \way, #0x3ff	/* way = ways - 1 */
	clz	\wayp, \way		/* wayp = way position in DC ISW/CSW/CISW */
	sub	\wayp, \wayp, #32	/* wayp = 64 - 32 - A = 32 - log2(ways) */
	lsr	\set, \cp_val, #13
	and	\set, \set, #0x7fff	/* set = sets - 1 */

/* loop_set */
2:
	mov	\wayv, \way		/* wayv = way variable */

/* loop_way */
3:
	lsl	\dc_val, \wayv, \wayp
	orr	\dc_val, \dc_val, \lvl, lsl #1
	lsl	\tmp, \set, \line
	orr	\dc_val, \dc_val, \tmp
	dc	\op, \dc_val

	subs	\wayv, \wayv, #1
	b.ge	3b			/* back to loop_way */
	subs	\set, \set, #1
	b.ge	2b			/* back to loop_set */

	/*---------- END Specified Level Operation -----------*/
4:
	add	\lvl, \lvl, #1		/* increment cache level */
	b	1b			/* back to loop_level */

5:
	mov	\tmp, #0
	msr	csselr_el1, \tmp	/* restore csselr_el1 */
	dsb	sy
	isb
.endm

.macro	invalidate_all_cache, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, \
				tmp7, tmp8, tmp9
	operate_all_cache isw, \tmp0, \tmp1, \tmp2, \tmp3, \tmp4, \
				 \tmp5, \tmp6, \tmp7, \tmp8, \tmp9
.endm

.macro	clean_all_cache, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, \
				tmp7, tmp8, tmp9
	operate_all_cache csw, \tmp0, \tmp1, \tmp2, \tmp3, \tmp4, \
				\tmp5, \tmp6, \tmp7, \tmp8, \tmp9
.endm

.macro	invcln_all_cache, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, \
				tmp7, tmp8, tmp9
	operate_all_cache cisw, \tmp0, \tmp1, \tmp2, \tmp3, \tmp4, \
				\tmp5, \tmp6, \tmp7, \tmp8, \tmp9
.endm

.macro	invalidate_all_tlb
	tlbi	alle3
	dsb	sy
.endm

.macro  clean_invalidate_dcache, ops, addr, end, tmp0, tmp1

	mrs	\tmp1, ctr_el0
	lsr	\tmp1, \tmp1, #16
	and	\tmp1, \tmp1, #0xf
	mov	\tmp0, #4
	lsl	\tmp0, \tmp0, \tmp1		/* cache line size */

	sub	\tmp1, \tmp0, #1
	bic	\addr, \addr, \tmp1
1: 	dc	\ops, \addr
	add	\addr, \addr, \tmp0
	cmp	\addr, \end
	b.lo	1b
	dsb	sy
.endm

.macro  invalidate_icache, ops, addr, end, tmp0, tmp1

	mrs	\tmp1, ctr_el0
	and	\tmp1, \tmp1, #0xf
	mov	\tmp0, #4
	lsl	\tmp0, \tmp0, \tmp1		/* cache line size */

	sub	\tmp1, \tmp0, #1
	bic	\addr, \addr, \tmp1
1: 	ic	\ops, \addr
	add	\addr, \addr, \tmp0
	cmp	\addr, \end
	b.lo	1b
	dsb	ish
	isb
.endm


.macro armv8_switch_el, reg, el1_label, el2_label, el3_label
	mrs		\reg, CurrentEL
	cmp		\reg, #0x4
	b.eq	\el1_label
	cmp		\reg, #0x8
	b.eq	\el2_label
	cmp		\reg, #0xc
	b.eq	\el3_label
	b		.
.endm

#else

/*
 *	^ ------ ^
 *	|  spsr  |
 *	* ------ *
 *	|   PC   |	Usr/System mode PC
 *	* ------ *
 *	|   R0   |
 *	* ------ *
 *	|   R1   |
 *	* ------ *
 *	|  ....  |
 *	* ------ *
 *	|   R12  |
 *	* ------ *
 *	| LR(R14)|	Usr/System mode LR
 *	* ------ *
 */

.macro	save_context
	sub	sp, sp, #56		/* Calling r0-r12, LR */
	stmia	sp, {r0 - r12, lr}^	/* push	{r0 - r12, lr}^ */
	push	{lr}			/* Save calling PC */
	mrs	r0, spsr
	push	{r0}			/* Save CPSR */
.endm

.macro	restore_context
	pop	{r0}
	msr	spsr_cxsf, r0		/* Restore CPSR */
	pop	{lr}			/* Get calling PC */
	ldmia	sp, {r0 - r12, lr}^	/* Calling r0-r12, LR */
	add	sp, sp, #56		/* pop {r0 - r12, lr}^ */
.endm

.macro	invalidate_all_tlb, tmp0
	mov		\tmp0, #0x00
	mcr		p15, 0, \tmp0, c8, c7, 0		/* Invalidate entire unified TLB */
	mcr		p15, 0, \tmp0, c8, c6, 0		/* Invalidate entire data TLB */
	mcr		p15, 0, \tmp0, c8, c5, 0		/* Invalidate entire instruction TLB */
.endm

.macro	operate_all_cache, cr, tmp0, tmp1, tmp2, tmp3, tmp4
	mov		\tmp0, #0
	mcr		p15, 0, \tmp0, c7, c5, 6		/* Invalidate entire branch prediction array */
	mcr		p15, 0, \tmp0, c7, c5, 0		/* Invalidate entire icache */
	mcr		p15, 2, \tmp0, c0, c0, 0		/* cache size selection register, select dcache */
	mrc		p15, 1, \tmp0, c0, c0, 0		/* cache size id register */
	mov		\tmp0, \tmp0, asr #13
	movw		\tmp2, #0x0fff
	and		\tmp0, \tmp0, \tmp2
	cmp		\tmp0, #0x7f
	moveq		\tmp0, #0x1000
	beq		1f
	cmp		\tmp0, #0xff
	moveq		\tmp0, #0x2000
	movne		\tmp0, #0x4000
1:
	mov		\tmp1, #0
	mov		\tmp2, #0x40000000
	mov		\tmp3, #0x80000000
	mov		\tmp4, #0xc0000000
2:
	mcr		p15, 0, \tmp1, c7, \cr, 2		/* invalidate dcache by set / way */
	mcr		p15, 0, \tmp2, c7, \cr, 2		/* invalidate dcache by set / way */
	mcr		p15, 0, \tmp3, c7, \cr, 2		/* invalidate dcache by set / way */
	mcr		p15, 0, \tmp4, c7, \cr, 2		/* invalidate dcache by set / way */
	add		\tmp1, \tmp1, #0x20
	add		\tmp2, \tmp2, #0x20
	add		\tmp3, \tmp3, #0x20
	add		\tmp4, \tmp4, #0x20
	cmp		\tmp1, \tmp0
	bne		2b
.endm

.macro	invalidate_all_cache, tmp0, tmp1, tmp2, tmp3, tmp4
	operate_all_cache c6, \tmp0, \tmp1, \tmp2, \tmp3, \tmp4
.endm

.macro	clean_all_cache, tmp0, tmp1, tmp2, tmp3, tmp4
	operate_all_cache c10, \tmp0, \tmp1, \tmp2, \tmp3, \tmp4
.endm

.macro	invcln_all_cache, tmp0, tmp1, tmp2, tmp3, tmp4
	operate_all_cache c14, \tmp0, \tmp1, \tmp2, \tmp3, \tmp4
.endm

#endif

