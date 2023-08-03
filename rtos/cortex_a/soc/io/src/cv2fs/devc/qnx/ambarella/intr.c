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

static inline int tx_interrupt(DEV_UART *dev)
{
    int         status = 0;
    uintptr_t   base = dev->base;
    int         cr1;
    AMBA_UART_REG_s *pUartReg = (AMBA_UART_REG_s *)base;

    cr1 = pUartReg->IntCtrl;
    cr1 = in32(base + AMBA_UART_IRQC_REG);
    out32(base + AMBA_UART_IRQC_REG, cr1 & ~0x2U);

    dev->tty.un.s.tx_tmr = 0;

    /* Send event to io-char, tto() will be processed at thread time */
    atomic_set(&dev->tty.flags, EVENT_TTO);
    status |= 1;

    return (status);
}

static inline int rx_interrupt(DEV_UART *dev)
{
    int            status = 0;
    unsigned       key, rxdata;
    uintptr_t      base = dev->base;

    while ((in32(base + AMBA_UART_SR_REG) & AMBA_UART_SR_RXNOTEMT)) {
        /*
         * Read next character from FIFO
         */
        rxdata = in32(base + AMBA_UART_FIFO_REG);
        key = rxdata & 0xFF;
        status |= tti(&dev->tty, key);
    }
    return status;
}

static inline int do_interrupt(DEV_UART *dev, int id)
{
    uintptr_t base = dev->base;
    unsigned rxdata = 0;
    int status = 0;

    (void) id;
    switch ((in32(base + AMBA_UART_FIFOC_REG)) & 0xfU) {
    case AMBA_UART_IRQ_RX_DATA_READY:
        rxdata = (in32(base + AMBA_UART_FIFO_REG)) & 0xffU;
        status |= tti(&dev->tty, rxdata);
        break;
    case AMBA_UART_IRQ_MODEM_STATUS_CHANGED:
        in32(base + AMBA_UART_MODEMSR_REG);
        break;
    case AMBA_UART_IRQ_RX_LINE_STATUS:
        in32(base + AMBA_UART_LINESR_REG);
        break;
    case AMBA_UART_IRQ_BUSY:
        in32(base + AMBA_UART_SR_REG);
        break;
    case AMBA_UART_IRQ_TIME_OUT:
        in32(base + AMBA_UART_FIFO_REG);
        break;
    default:
        break;

    }

    return status;
}

/*
 * Serial interrupt handler
 */
static const struct sigevent * ser_intr(void *area, int id)
{
    DEV_UART *dev = area;

    if (do_interrupt(dev,id) && (dev->tty.flags & EVENT_QUEUED) == 0) {
        dev_lock(&ttyctrl);
        ttyctrl.event_queue[ttyctrl.num_events++] = &dev->tty;
        atomic_set(&dev->tty.flags, EVENT_QUEUED);
        dev_unlock(&ttyctrl);
        return &ttyctrl.event;
    }

    return 0;
}

int interrupt_event_handler (message_context_t * msgctp, int code, unsigned flags, void *handle)
{
    uint32_t status;
    DEV_UART *dev = (DEV_UART *) handle;

    (void) msgctp;
    (void) code;
    (void) flags;
    status = do_interrupt (dev, dev->iid[0]);

    if (status) {
        iochar_send_event (&dev->tty);
    }

    InterruptUnmask (dev->intr[0], dev->iid[0]);
    return (EOK);
}

void ser_attach_intr(DEV_UART *dev)
{
    struct sigevent event;

    if(dev->isr) {
        dev->iid[0] = InterruptAttach(dev->intr[0], ser_intr, dev, 0, _NTO_INTR_FLAGS_TRK_MSK);
        if (dev->intr[1] != 0xFFFF) {
            dev->iid[1] = InterruptAttach(dev->intr[1], ser_intr, dev, 0, _NTO_INTR_FLAGS_TRK_MSK);
        }
    } else {
        // Associate a pulse which will call the event handler.
        if ((event.sigev_code = pulse_attach (ttyctrl.dpp, MSG_FLAG_ALLOC_PULSE, 0, &interrupt_event_handler, dev)) == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Unable to attach event pulse: %s", __FUNCTION__, strerror(errno));
            return;
        }

        /* Init the pulse for interrupt event */
        event.sigev_notify = SIGEV_PULSE;
        event.sigev_coid = ttyctrl.coid;
        event.sigev_priority = 63; /* service interrupts at a higher priority then client requests */
        event.sigev_value.sival_int = 0;

        dev->iid[0] = InterruptAttachEvent (dev->intr[0], &event, _NTO_INTR_FLAGS_TRK_MSK);
        if (dev->iid[0] == -1) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Unable to attach InterruptEvent: %s", __FUNCTION__, strerror(errno));
        }
    }
}

