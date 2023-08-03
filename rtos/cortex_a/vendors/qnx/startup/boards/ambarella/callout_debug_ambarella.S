/**
 *  @file callout_debug_ambarella.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Console callout APIs
 *
 */

/*
 * Ambarella UART specific polled serial I/O
 */

#include "callout.ah"
#include <arm/ambarella.h>


/*
 * Patch interrupt callouts to access rw data.
 * The first call will also map the uart.
 *
 * Patcher routine takes the following arguments:
 *    x0 - syspage paddr
 *    x1 - vaddr of callout
 *    x2 - offset from start of syspage to start of callout routine
 *    x3 - offset from start of syspage to rw storage
 *    x4 - patch data
 *    x5 - callout_rtn
 */

patch_debug:
#if !defined(CONFIG_XEN_SUPPORT_QNX)
	sub	sp, sp, #16
	stp	x19, x30, [sp]

	add	x19, x0, x2                // x19 = address of callout routine

	/*
	 * Map UART using patch_data parameter
	 */
	mov	x0, #0x1000
	ldr	x1, [x4]
	bl	callout_io_map

	/*
	 * Patch callout with mapped virtual address in x0
	 */
	CALLOUT_PATCH    x19, w6, w7

	ldp	x19, x30, [sp]
	add	sp, sp, #16
#endif
	ret

/*
 * -----------------------------------------------------------------------
 * void    display_char_xzynq(struct sypage_entry *, char)
 *
 * x0: syspage pointer
 * x1: character
 * -----------------------------------------------------------------------
 */
CALLOUT_START(display_char_ambarella, 0, patch_debug)
#if !defined(CONFIG_XEN_SUPPORT_QNX)
	mov	x7, #0xabcd                // UART base address (patched)
	movk	x7, #0xabcd, lsl #16
	movk	x7, #0xabcd, lsl #32
	movk	x7, #0xabcd, lsl #48

UartPolling:
	add	w0, w0, #1
	tst	w0, #0x1000
	b.ne	Uart_Reset
	ldr	w2, [x7, #AMBA_UART_SR_REG]
	tst	w2, #AMBA_UART_SR_TXNOTFULL
	b.eq	UartPolling

	and	w0, w1, #0xff
	str	w0, [x7, #AMBA_UART_FIFO_REG]
	ret

Uart_Reset:
	mov	w0, #0
	str	w0, [x7, #AMBA_UART_HaltTx_REG]
	mov	w0, #4
	str	w0, [x7, #AMBA_UART_RESET_REG]

	and	w0, w1, #0xff
	str	w0, [x7, #AMBA_UART_FIFO_REG]
#endif
	ret
CALLOUT_END(display_char_ambarella)


/*
 * -----------------------------------------------------------------------
 * char    poll_key_ambarella(struct sypage_entry *)
 *
 * x0: syspage pointer
 * -----------------------------------------------------------------------
 */
CALLOUT_START(poll_key_ambarella, 0, patch_debug)
#if !defined(CONFIG_XEN_SUPPORT_QNX)
	mov	x7, #0xabcd                // UART base address (patched)
	movk	x7, #0xabcd, lsl #16
	movk	x7, #0xabcd, lsl #32
	movk	x7, #0xabcd, lsl #48

	ldr	w1, [x7, #AMBA_UART_LINESR_REG]
	tst	w1, #0x1
	b.eq	0f

	ldr	w0, [x7, #AMBA_UART_FIFO_REG]
	ret

0:
#endif
	mov        w0, #-1
	ret
CALLOUT_END(poll_key_ambarella)

/*
 * -----------------------------------------------------------------------
 * int break_detect(struct syspage_entry *)
 *
 * x0: syspage pointer
 *
 * Return 1 if break detected, 0 otherwise
 * -----------------------------------------------------------------------
 */
CALLOUT_START(break_detect_ambarella, 0, patch_debug)
#if !defined(CONFIG_XEN_SUPPORT_QNX)	
	mov	x7, #0xabcd                // UART base address (patched)
	movk	x7, #0xabcd, lsl #16
	movk	x7, #0xabcd, lsl #32
	movk	x7, #0xabcd, lsl #48

	ldr	w0, [x7, #AMBA_UART_FIFO_REG]
	mov	w0, #1
	ret
0:
#endif
	mov	w0, #0
	ret
CALLOUT_END(break_detect_ambarella)
