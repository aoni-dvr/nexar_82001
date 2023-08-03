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

#ifndef EXTERNS_H
#define EXTERNS_H

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/neutrino.h>
#include <termios.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <sys/iomsg.h>
#include <atomic.h>
#include <hw/inout.h>
#include <sys/io-char.h>
#include <sys/hwinfo.h>
#include <drvr/hwinfo.h>
#include <pthread.h>
#include <sys/rsrcdbmgr.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <arm/ambarella.h>

#include "AmbaUART_Def.h"
#include "AmbaReg_UART.h"
#include "AmbaRTSL_UART.h"

#define FIFO_SIZE   32

typedef struct dev_uart {
    TTYDEV          tty;
    unsigned        intr[2];
    int             iid[2];
    unsigned        clk;
    unsigned        fifo;
    uintptr_t       base;
    unsigned        cr;
    unsigned        mr;
    int             isr;
    int             port;
    int             uart_id;
    int             uart_clk_id;
} DEV_UART;

DEV_UART *devptr;

typedef struct ttyinit_ambarella {
    TTYINIT     tty;
    unsigned    intr[2];   /* Interrupts */
    int         isr;
    int         uart_id;
    int         uart_clk_id;
} TTYINIT_AMBA;

TTYCTRL     ttyctrl;

#include "proto.h"

AMBA_UART_REG_s *pAmbaUART_Reg[AMBA_NUM_UART_CHANNEL];
unsigned char PrintBuffer[4096];
unsigned int Rptr;
unsigned int Wptr;

#endif //EXTERNS_H
