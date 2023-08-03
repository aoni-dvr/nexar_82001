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

#include "ipl.h"
#include <hw/inout.h>
#include "ipl_ambarella.h"

#if !defined(CONFIG_XEN_SUPPORT_QNX)

unsigned char amba_uart_pollkey();
unsigned char amba_uart_getchar();
void amba_uart_putchar(unsigned char);

static const ser_dev amba_dev = {
        amba_uart_getchar,
        amba_uart_putchar,
        amba_uart_pollkey
        };

unsigned char amba_uart_pollkey(void)
{
    if (AMBA_CONSOLE_SR & AMBA_SR_READ_STATUS_MASK)
        return 1;
    else
        return 0;
}

unsigned char amba_uart_getchar(void)
{
    return ((unsigned char) AMBA_CONSOLE_FIFO);
}

void UartGetTxEmpty(unsigned int *pTxEmpty)
{
    AMBA_UART_REG_s *pUartReg;
    pUartReg = (AMBA_UART_REG_s *)AMBA_CONSOLE_BASE;

    if ((pUartReg->LineStatus & 0x40U) != 0UL) {
        pUartReg->ResetCtrl = 4U;
        *pTxEmpty = 1UL;
    } else {
        *pTxEmpty = 0UL;
    }
}

void amba_uart_putchar(unsigned char data1)
{
    unsigned int TxEmpty = 0;
    do {
        (void)UartGetTxEmpty(&TxEmpty);
    } while (TxEmpty == 0U);
    AMBA_CONSOLE_FIFO = (unsigned short) data1;
}

void init_serial(void)
{
    AMBA_UART_REG_s *pUartReg;
    unsigned int delay = 0xffff;
    unsigned int Divider = 0;

    unsigned int *pTmp;

    pTmp = (unsigned int *) (0Xed0801c8);
    *pTmp = 0U;
    pTmp = (unsigned int *) (0Xed080038);
    *pTmp = 1U;

    pUartReg = (AMBA_UART_REG_s *)AMBA_CONSOLE_BASE;

    pUartReg->IntCtrl = 0U;
    /* reset UART */
    pUartReg->ResetCtrl = 0x1U;   /* Reset UART */
    while (delay > 0) {
        (delay --);
    }
    pUartReg->ResetCtrl = 0x0U;

    Divider = AMBA_CONSOLE_BR << 4U;
    Divider = (UART_FREQ + (Divider >> 1U)) / Divider;
    /* enable Divisor Latch Access */
    pUartReg->LineCtrl.DLAB = 0x1UL;

    /* set Divisor Latch value (16-bit) */
    pUartReg->DataBuf = Divider & 0xffU;
    pUartReg->IntCtrl = (Divider >> 8U) & 0xffU;

    /* disable Divisor Latch Access */
    pUartReg->LineCtrl.DLAB = 0UL;

    pUartReg->LineCtrl.DataBits = 3U;
    pUartReg->LineCtrl.StopBits = 0U;
    pUartReg->LineCtrl.ParityEnable = 0U;
    pUartReg->LineCtrl.EvenParity = 0U;

    pUartReg->FifoCtrl = 0x31U;
    pUartReg->ResetCtrl = 6U;
    /* enable all interrupts except Tx FIFO empty */
    pUartReg->IntCtrl = 0x3dU;
    pUartReg->DmaCtrl = 0U;

    init_serdev((ser_dev *)&amba_dev);
}

#else

extern void _flush_d_cache_range(void *addr, unsigned int size);
extern void HYPERVISOR_console_io(int op, int len, char *buf);

static char outbuf[256];
static unsigned int outlen = 0;

unsigned char xen_uart_getchar(void)
{
    return 0;
}

void xen_uart_putchar(unsigned char data1)
{
    outbuf[outlen] = (char) data1;
    outlen++;

    if ('\n' == (char) data1 || outlen >= sizeof(outbuf)) {
        _flush_d_cache_range(outbuf, sizeof(outbuf));
        HYPERVISOR_console_io(0, outlen, outbuf);
        outlen = 0;
    }
}

unsigned char xen_uart_pollkey(void)
{
    return 0;
}

static const ser_dev xen_dev = {
    xen_uart_getchar,
    xen_uart_putchar,
    xen_uart_pollkey,
};

void init_serial_xen(void)
{
    outlen = 0;
    init_serdev((ser_dev *) &xen_dev);
}

#endif  /* CONFIG_XEN_SUPPORT_QNX */

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
