/*
 * xen/arch/arm/arm64/debug-ambarella.S
 *
 * Copyright (c) 2020, Ambarella Inc.
 */

#include <asm/asm_defns.h>

/*
 * Ambarella UART initialization
 * xb: register which containts the UART base address
 * c: scratch register number
 */
.macro early_uart_init xb, c
	/* No initialization is performed or necessary here... */
        /* Assume one of BST, BLD, or ATF has already initialized the UART */
.endm

/*
 * Ambarella UART wait UART to be ready to transmit
 * xb: register which contains the UART base address
 * c: scratch register number
 */
.macro early_uart_ready xb, c
1:
        ldrh  w\c, [\xb, #0x7c]      /* <- UART Status register */
        tst   w\c, #0x2              /* Check BUSY bit */
        b.eq  1b                     /* Wait for the UART to be ready */
.endm

/*
 * Ambarella UART transmit character
 * xb: register which contains the UART base address
 * wt: register which contains the character to transmit
 */
.macro early_uart_transmit xb, wt
        strb  \wt, [\xb]             /* -> UART DataBuf register */
.endm

/*
 * Local variables:
 * mode: ASM
 * indent-tabs-mode: nil
 * End:
 */
