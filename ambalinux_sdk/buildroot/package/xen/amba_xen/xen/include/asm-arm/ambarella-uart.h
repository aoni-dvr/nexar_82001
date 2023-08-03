/*
 * xen/include/asm/ambarella-uart.h
 *
 * Copyright (C) 2020, Ambarella Inc.
 */

#ifndef __ASM_AMBARELLA_UART_H
#define __ASM_AMBARELLA_UART_H

/* Ambarella UART registers */
#define UART_RBR	0x00
#define UART_THR	0x00
#define UART_DLL	0x00
#define UART_IER	0x04
#define UART_DLH	0x04
#define UART_IIR	0x08
#define UART_FCR	0x08
#define UART_MCR	0x10
#define UART_LSR	0x14
#define UART_MSR	0x18
#define UART_SCR	0x1c
#define UART_DMAE	0x28
#define UART_USR	0x7c
#define UART_TFL	0x80
#define UART_RFL	0x84
#define UART_SRR	0x88
#define UART_HTX	0xa4
#define UART_RTR	0xac
#define UART_TTR	0xb0
#define UART_STATE	0xb4
#define UART_BUSY	0xb8
#define UART_CPR	0xf4
#define UART_UCV	0xf8
#define UART_CTR	0xfc

/* UART_IER bits */
#define UART_IER_ERBFI			0x1
#define UART_IER_ETBEI			0x2
#define UART_IER_ELSI			0x4
#define UART_IER_EDSSI			0x8
#define UART_IER_EBDI			0x10
#define UART_IER_RTOI			0x20
#define UART_IER_ERETOI			0x40
#define UART_IER_PTIME			0x80

/* UART_IIR bits */
#define UART_IIR_MODEM_STATUS_CHANGED	0x0
#define UART_IIR_NO_INT_PENDING		0x1
#define UART_IIR_THR_EMPTY		0x2
#define UART_IIR_RCV_DATA_AVAIL		0x4
#define UART_IIR_RCV_STATUS		0x6
#define UART_IIR_BUSY_DETECT		0x7
#define UART_IIR_CHAR_TIMEOUT		0x12
#define UART_IUR_CHAR_TIMEOUT_FIFO_EMP	0x13

/* UART_USR bits */
#define UART_USR_BUSY			0x1
#define UART_USR_TFNF			0x2
#define UART_USR_TFE			0x4
#define UART_USR_RFNE			0x8
#define UART_USR_RFF			0x10

#endif

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
