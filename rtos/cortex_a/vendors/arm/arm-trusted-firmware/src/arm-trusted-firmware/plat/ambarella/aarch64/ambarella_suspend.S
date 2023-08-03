/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <platform_def.h>

	.globl ambarella_relocate_suspend

#if (DDRC_VERSION > 1)

.macro trigger_uinst
	str	w1, [x21, #0xac]	/* DRAM_UINST1 */
	str	w2, [x21, #0xb0]	/* DRAM_UINST2 */
	str	w4, [x21, #0xb8]	/* DRAM_UINST4 */
	mov	w0, #1
	str	w0, [x21, #0xbc]	/* DRAM_UINST5 */
.endm

.macro wait_complete, op, bit, label
	tbz	w10, #1, 100f
	\op	w0, \bit, \label
100:	tbz	w10, #2, 101f
	\op	w1, \bit, \label
101:
.endm

.align	3
ambarella_suspend_exec:
	add	x21, x20, #0x4000	/* broadcast base register */
	add	x22, x20, #0x5000	/* ddrc0 base regsiter */
	add	x23, x20, #0x6000	/* ddrc1 base regsiter */

	ldr	w10, [x20]		/* used for checking host num */

	/* disable DRAM access */
	ldr	w0, [x21]
	bic	w0, w0, #(1 << 0)
	str	w0, [x21]

	/* wait for DDRC idle */
1:	ldr	w0, [x22, #0x3a4]
	ldr	w1, [x23, #0x3a4]
	wait_complete tbz, #2, 1b

	/* wait for 8*tRFC to allow refresh credit to be maxed out */
	mov	w0, #0x1
	str	w0, [x25, #RCT_TIMER_CTRL_OFFSET]
	str	wzr, [x25, #RCT_TIMER_CTRL_OFFSET]
2:	ldr	w0, [x25, #RCT_TIMER_OFFSET]
	cmp	w0, #240	/* 10us */
	blo	2b

	/* disable auto-refresh */
	ldr	w0, [x21]
	bic	w0, w0, #(1 << 1)
	str	w0, [x21]

	/* disable LP5 background ZQ calibration */
	mov	w0, #0x9f1c0000
	orr	w0, w0, #0x2
	str	w0, [x21, #0x30]
	/* wait for complete */
3:	ldr	w0, [x22, #0x30]
	ldr	w1, [x23, #0x30]
	wait_complete tbnz, #31, 3b

	/* turnoff WCK Buffer in LP5 device */
	mov	w1, #0x0000000d
	mov	w2, #0x0000007c
	mov	w4, #0x00000303
	trigger_uinst
	/* wait for complete */
4:	ldr	w0, [x22, #0xbc]
	ldr	w1, [x23, #0xbc]
	wait_complete tbnz, #0, 4b

	/* issue SRE to enter into SR */
	mov	w1, #0x0000000d
	mov	w2, #0x00004068
	mov	w4, #0x00000303
	trigger_uinst
	/* wait for complete */
5:	ldr	w0, [x22, #0xbc]
	ldr	w1, [x23, #0xbc]
	wait_complete tbnz, #0, 5b

	wfi			/* cpu down */
	b	.

#else

.align	3
ambarella_suspend_exec:
	add	x21, x20, #0x0600	/* broadcast base register */
	add	x22, x20, #0x0800	/* ddrc0 base regsiter */
	add	x23, x20, #0x0a00	/* ddrc1 base regsiter */

	ldr	w10, [x20, #0x00]	/* used for checking host num */

	/* disable DRAM access from DDRC0 */
	ldr	w2, [x22, #0x00]	/* DRAM_CTL */
	bic	w2, w2, #(1 << 0)
	str	w2, [x22, #0x00]

	tbz	w10, #0, 1f		/* check if 2 hosts */

	/* disable DRAM access from DDRC1 */
	ldr	w2, [x23, #0x00]
	bic	w2, w2, #(1 << 0)
	str	w2, [x23, #0x00]

	/* enter into SR for DDRC0 */
1:	ldr	w2, [x22, #0x20]	/* DRAM_SREF_CTL */
	orr	w2, w2, #(1 << 31)
	str	w2, [x22, #0x20]

2:	ldr	w2, [x22, #0x20]
	tbz	w2, #28, 2b

	tbz	w10, #0, 4f		/* check if 2 hosts */

	/* enter into SR for DDRC1 */
	ldr	w2, [x23, #0x20]
	orr	w2, w2, #(1 << 31)
	str	w2, [x23, #0x20]

3:	ldr	w2, [x23, #0x20]
	tbz	w2, #28, 3b

4:	b	.

#endif

.align	3
ambarella_suspend_sz:
	.quad	. - ambarella_suspend_exec

ambarella_relocate_suspend:
	ldr	x0, =FIO_BASE
	ldr	x1, [x0]
	orr	x1, x1, #(1 << 1)
	str	x1, [x0]

	ldr	x0, =ambarella_suspend_exec
	ldr	x1, =FIO_4K_BASE

	/* x2 = (((ambarella_suspend_sz + 15) / 16) * 16) */
	ldr	x2, ambarella_suspend_sz
	add	x2, x2, #0x0f
	and	x2, x2, #(~0x0f)

1:	ldp	x3, x4, [x0], #16
	stp	x3, x4, [x1], #16
	sub	x2, x2, #16
	cbnz	x2, 1b

	ldr	x20, =DDRC_BASE
	ldr	x25, =RCT_BASE
	ldr	x26, =AXI_BASE

	ldr	x1, =FIO_4K_BASE
	br	x1

