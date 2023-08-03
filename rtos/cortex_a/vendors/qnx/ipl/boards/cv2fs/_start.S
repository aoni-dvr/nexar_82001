/*
 * $QNXLicenseC:
 * Copyright 2018, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */
    .text
    .align 2

    .globl   _start

_start:
    /*
     * Invalidate Instructions cache
     */
    ic       iallu                     // I cache Invalidate All to Point of Unification
    dsb      ish                       // Ensure completion of the invalidations
    isb                                // Synchronize the fetched instruction stream

    /*
     * Load stack pointer and jump to C
     */
    adr       x0, __stack_ptr
    mov       sp, x0
#if defined(CONFIG_XEN_SUPPORT_QNX)
    bl        show_hvc_banner
#endif	
    bl        ipl_main

oops:
    wfi
    b        oops

#if defined(CONFIG_XEN_SUPPORT_QNX)

/*
 * Output to hypervisor console
 */
show_hvc_banner:
	mov	x0, 0
	mov	x1, 25
	ldr	x2, =banner
	mov	x16, 18
	hvc	0xea1
	ret

banner:
	.string "QNX/XEN IPL Booting up!\r\n"

#endif

#ifdef __QNXNTO__
#ifdef __USESRCVERSION
.section .ident, "SM",%progbits,1;
.asciz "$URL: http://svn.ott.qnx.com/product/branches/7.0.0/trunk/hardware/ipl/boards/xzynq-zcu102/_start.S $ $Rev: 885792 $";
.previous
#endif
#endif
