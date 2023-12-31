/**
 * bld/arm64/armcache.S
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
#include <macro.S>

.globl	_enable_icache
_enable_icache:
	armv8_switch_el x1, 1f, 2f, 3f
1:	mrs	x0, sctlr_el1
	orr	x0, x0, #0x1000
	msr	sctlr_el1, x0
	ret
2:	b	.
3:	mrs	x0, sctlr_el3
	orr	x0, x0, #0x1000
	msr	sctlr_el3, x0
	ret

.globl	_disable_icache
_disable_icache:
	armv8_switch_el x1, 1f, 2f, 3f
1:	mrs	x0, sctlr_el1
	bic	x0, x0, #0x1000
	msr	sctlr_el1, x0
	ret
2:	b	.
3:	mrs	x0, sctlr_el3
	bic	x0, x0, #0x1000
	msr	sctlr_el3, x0
	ret

.globl	_enable_dcache
_enable_dcache:
	armv8_switch_el x1, 1f, 2f, 3f
1:	mrs	x0, sctlr_el1
	orr	x0, x0, #0x4
	msr	sctlr_el1, x0
	dsb	ish
	isb
	ret
2:	b	.
3:	mrs	x0, sctlr_el3
	orr	x0, x0, #0x4
	msr	sctlr_el3, x0
	dsb	ish
	isb
	ret

.globl	_drain_write_buffer
_drain_write_buffer:
	ret

.globl	_disable_dcache
_disable_dcache:
	armv8_switch_el  x1, 1f, 2f, 3f
1:	mrs	x0, sctlr_el1
	bic	x0, x0, #0x4
	msr	sctlr_el1, x0
	ret
2:	b	.
3:	mrs	x0, sctlr_el3
	bic	x0, x0, #0x4
	msr	sctlr_el3, x0
	ret

.globl disable_mmu
disable_mmu:
	armv8_switch_el  x1, 1f, 2f, 3f
1:	mrs	x0, sctlr_el1
	bic	x0, x0, #0x1		/* disable MMU */
	msr	sctlr_el1, x0
	ret
2:	b	.
3:	mrs	x0, sctlr_el3
	bic	x0, x0, #0x1		/* disable MMU */
	msr	sctlr_el3, x0
	ret

.globl	_clean_flush_d_cache
_clean_flush_d_cache:
	stp     x8, x9, [sp, #-16]!
	invcln_all_cache x0, x1, x2, x3, x4, x5, x6, x7, x8, x9
	ldp     x8, x9, [sp], #16
	ret

.global _clean_flush_all_cache
_clean_flush_all_cache:
	/* invalid all icache */
	ic	ialluis
	isb	sy
	/* invalid all dcache */
	stp     x8, x9, [sp, #-16]!
	invcln_all_cache x0, x1, x2, x3, x4, x5, x6, x7, x8, x9
	ldp     x8, x9, [sp], #16
	ret

.global _flush_d_cache
_flush_d_cache:
	stp     x8, x9, [sp, #-16]!
	invalidate_all_cache x0, x1, x2, x3, x4, x5, x6, x7, x8, x9
	ldp     x8, x9, [sp], #16
	ret

.global _clean_d_cache
_clean_d_cache:
	stp     x8, x9, [sp, #-16]!
	clean_all_cache x0, x1, x2, x3, x4, x5, x6, x7, x8, x9
	ldp     x8, x9, [sp], #16
	ret


.globl _clean_d_cache_range
_clean_d_cache_range:
	add x1, x1, x0
	clean_invalidate_dcache cvac, x0, x1, x2, x3
	ret

.globl _flush_d_cache_range
_flush_d_cache_range:
	add x1, x1, x0
	clean_invalidate_dcache ivac, x0, x1, x2, x3
	ret

.globl _clean_flush_d_cache_range
_clean_flush_d_cache_range:
	add x1, x1, x0
	clean_invalidate_dcache  civac, x0, x1, x2, x3
	ret

.globl __invalid_icache_range
__invalid_icache_range:
	add x1, x1, x0
	invalidate_icache ivau, x0, x1, x2, x3
	ret
