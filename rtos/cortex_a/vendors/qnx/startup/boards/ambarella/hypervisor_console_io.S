/*
 * hyperviosor_console_io.S
 *
 * Copyright (C) 2020, Ambarella Inc.
 */

	.text
	.align 4
	.global HYPERVISOR_console_io
HYPERVISOR_console_io:
	mov	x16, 18
	hvc	0xea1
	ret
