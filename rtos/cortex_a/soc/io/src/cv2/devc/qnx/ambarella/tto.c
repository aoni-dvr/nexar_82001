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

#include "externs.h"

int tto(TTYDEV *ttydev, int action, int arg1)
{
    TTYBUF *bup = &ttydev->obuf;
    DEV_UART *dev = (DEV_UART *)ttydev;
    uintptr_t  base = dev->base;
    unsigned char c;
    unsigned int TxEmpty = 0;
    unsigned int count = 0;
    unsigned int RetVal = 0;
    unsigned int ActualCount = 0U;
    unsigned int amba_uart_channel = dev->uart_id;

    (void) arg1;
    switch (action) {
    case TTO_STTY:
        ser_stty(dev, SER_STTY);
        return 0;

    case TTO_LINESTATUS:
        return (in32(base + AMBA_UART_LINESR_REG));

    case TTO_CTRL:
    case TTO_DATA:
    case TTO_EVENT:
        break;

    default:
        return 0;
    }

    while (bup->cnt > 0) {
        /*
         * If the OSW_PAGED_OVERRIDE flag is set then allow
         * transmit of character even if output is suspended via
         * the OSW_PAGED flag. This flag implies that the next
         * character in the obuf is a software flow control
         * character (STOP/START).
         * Note: tx_inject sets it up so that the control
         *       character is at the start (tail) of the buffer.
         */
        if ((dev->tty.flags & (OHW_PAGED | OSW_PAGED)) &&
            !(dev->tty.xflags & OSW_PAGED_OVERRIDE)) {
            break;
        }

        if ((in32(base + AMBA_UART_SR_REG) & 0x2) == 0) {
            break;
        }

        /*
         * Get the next character to print from the output buffer
         */
        count = 0;
        while (bup->cnt > 0) {
            dev_lock(&dev->tty);
            c = tto_getchar(&dev->tty);
            dev_unlock(&dev->tty);

            if (c == '\n') {
                if ((Wptr == 0U) && (PrintBuffer[4095U] == '\r')) {
                    PrintBuffer[Wptr] = c;
                    Wptr++;
                    count++;
                } else if (PrintBuffer[Wptr - 1U] == '\r') {
                    PrintBuffer[Wptr] = c;
                    Wptr++;
                    count++;
                } else {
                    if ((Wptr != 4095U) && ((Wptr + 1) != Rptr)) {
                        PrintBuffer[Wptr] = '\r';
                        PrintBuffer[Wptr + 1] = c;
                        Wptr += 2;
                        count += 2;
                    } else if (Wptr == 4095U) {
                        PrintBuffer[Wptr] = '\r';
                        PrintBuffer[0] = c;
                        Wptr = 1U;
                        count++;
                    } else {
                        PrintBuffer[Wptr - 1] = '\r';
                        PrintBuffer[Wptr] = c;
                        Wptr = 1U;
                        count++;
                    }
                }
            } else {
                PrintBuffer[Wptr] = c;
                Wptr++;
                count++;
            }
            if ((Wptr == Rptr) || (Wptr == 4096U)) {
                if (Wptr == 4096U) {
                    Wptr = 0U;
                }
                break;
            }
        }
        while ((RetVal == 0) && (Rptr != Wptr)) {
            RetVal = AmbaRTSL_UartWrite(amba_uart_channel, count, &PrintBuffer[Rptr], &ActualCount);
            if (RetVal == 0) {
                count -= ActualCount;
                Rptr += ActualCount;
                Rptr = Rptr & 0xfffU;

                /* Some chips are much weaker so that we implement a recovery solution here */
                if (ActualCount == 0U) {
                    //UartResetTx(dev);
                }
            }

            if ((RetVal != 0) || (count == 0U)) {
                break;
            }
        }

        count = 0;
        do {
            (void)AmbaRTSL_UartGetTxEmpty(amba_uart_channel, &TxEmpty);
            count++;
            if (count > 5000U) {   /* Assume all the tx fifo data could be transmitted within 10ms */
                //UartResetTx(dev);
                break;
            }
        } while (TxEmpty == 0U);

        dev->tty.un.s.tx_tmr = 0;

        /* Clear the OSW_PAGED_OVERRIDE flag as we only want
         * one character to be transmitted in this case.
         */
        if (dev->tty.xflags & OSW_PAGED_OVERRIDE) {
            atomic_clr(&dev->tty.xflags, OSW_PAGED_OVERRIDE);
        }
    }

    return (tto_checkclients(&dev->tty));
}

void ser_stty(DEV_UART *dev, int init)
{
    (void) dev;
    (void) init;

    return;
}
