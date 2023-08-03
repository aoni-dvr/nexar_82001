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

#ifndef _IPL_AMBARELLA_H
#define _IPL_AMBARELLA_H

#include <stdint.h>
/*
 * Global configuration for the Ethernet load image
 */
#define ARP_RETRY    30
#define ARP_DELAY    100000 /* 1s between each ARP */
#define TFTP_RETRY   3
#define TFTP_DELAY   90000000L /* 10s between each TFTP request */

/*
 * Address where the binary will be loaded, then the
 * startup will be copied automatically to the correct
 * address during the image_setup()
 */
#define QNX_LOAD_ADDR    CONFIG_FWPROG_SYS_LOADADDR
extern uint8_t TFTP_IP_SERVER[4];

/* Macro to stringify a number to a string */
#define __stringify1(x)    #x
#define __stringify(x)    __stringify1(x)

#define __packed        __attribute__((packed))

#define LE_2_BE_32(l) \
    ((((l) & 0x000000FF) << 24) | \
    (((l) & 0x0000FF00) << 8)   | \
    (((l) & 0x00FF0000) >> 8)   | \
    (((l) & 0xFF000000) >> 24))

#if !defined(CONFIG_XEN_SUPPORT_QNX)

#define AMBA_CONSOLE_BASE      0xe4000000
#define AMBA_CONSOLE_BR        115200
#define AMBA_SDIO1_BASEADDR    0xE00050000

#define UART_FREQ   24000000

/* UART registers, offset from base address */
#define AMBA_CONSOLE_FIFO           *(volatile unsigned int *) (AMBA_CONSOLE_BASE + 0x0)      /* Transmit and Receive FIFO */
#define AMBA_CONSOLE_SR             *(volatile unsigned int *) (AMBA_CONSOLE_BASE + 0x7c)        /* Channel status register */
#define AMBA_SR_READ_STATUS_MASK    0x00000001

typedef struct UartLinCtrl {
    unsigned int  DataBits:           2;      /* [1:0]: 0 = 5bits, 1 = 6bits, 2 = 7bits, 3 = 8bits */
    unsigned int  StopBits:           1;      /* [2]: Stop Bits Per Character. 0 = 1stop, 1 = 1.5 or 2stop */
    unsigned int  ParityEnable:       1;      /* [3]: 1 = Parity Enabled */
    unsigned int  EvenParity:         1;      /* [4]: 0 = Odd Parity, 1 = Even Parity */
    unsigned int  Reserved0:          1;      /* [5]: Reserved */
    unsigned int  BreakCtrl:          1;      /* [6]: Break Control */
    unsigned int  DLAB:               1;      /* [7]: Divisor Latch Access Bit */
    unsigned int  Reserved1:          24;     /* [31:8]: Reserved */
} AMBA_UART_LINE_CTRL_REG_s;

typedef struct UartReg {
    volatile unsigned int                                     DataBuf;            /* 0x000(R,W): Receive(R) or Tx Holding(W) Data Buffer Register */
    volatile unsigned int                                     IntCtrl;            /* 0x004(RW): Interruput Enable Register */
    volatile unsigned int                                     FifoCtrl;           /* 0x008(R,W): Interrupt ID(R) or FIFO Control(W) Register */
    volatile AMBA_UART_LINE_CTRL_REG_s                        LineCtrl;           /* 0x00C(RW): Line Control Register */
    volatile unsigned int                                     ModemCtrl;          /* 0x010(RW): Modem Control Register */
    volatile unsigned int                                     LineStatus;         /* 0x014(R): Line Status Register */
    volatile unsigned int                                     ModemStatus;        /* 0x018(R): Modem Status Register */
    volatile unsigned int                                     Scratchpad;         /* 0x01C(RW): Scratchpad Register */
    volatile unsigned int                                     Reserved0[2];       /* 0x020-0x024: Reserved */
    volatile unsigned int                                     DmaCtrl;            /* 0x028: DMA Control Register */
    volatile unsigned int                                     Reserved1[5];       /* 0x02C-0x03C: Reserved */
    volatile unsigned int                                     DataBurst[8];       /* 0x040-0x05C(R,W): Receive Buffer(R) or Transmit Holding(W) Register */
    volatile unsigned int                                     Reserved2[7];       /* 0x060-0x078: Reserved */
    volatile unsigned int                                     Status;             /* 0x07C(R): UART Status Register */
    volatile unsigned int                                     TxFifoDataSize;     /* 0x080(R): Transmit FIFO Level Register */
    volatile unsigned int                                     RxFifoDataSize;     /* 0x084(R): Receive FIFO Level Register */
    volatile unsigned int                                     ResetCtrl;          /* 0x088(W): Software Reset Register */
    volatile unsigned int                                     Reserved3[6];       /* 0x08C-0x0A0: Reserved*/
    volatile unsigned int                                     HaltTx;             /* 0x0A4(RW): Halt Tx Register */
    volatile unsigned int                                     Reserved4;          /* 0x0A8: Reserved */
    volatile unsigned int                                     RxThreshold;        /* 0x0AC: Rx Trigger Threshold */
    volatile unsigned int                                     TxThreshold;        /* 0x0B0: Tx Trigger Threshold */
    volatile unsigned int                                     Reserved5[16];      /* 0x0B4-0x0F0: Reserved */
    volatile unsigned int                                     Info;               /* 0x0F4(R): Component Parameter Register */
    volatile unsigned int                                     Version;            /* 0x0F8(R): Component Version Register */
    volatile unsigned int                                     PeripheralID;       /* 0x0FC(R): Component Type Register */
} AMBA_UART_REG_s;

void init_serial(void);

#else

void init_serial_xen(void);

#endif  /* CONFIG_XEN_SUPPORT_QNX */

#endif //_IPL_AMBARELLA_H
