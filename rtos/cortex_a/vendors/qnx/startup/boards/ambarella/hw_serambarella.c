/**
 *  @file hw_serambarella.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Console APIs
 *
 */

#include "startup.h"
#include <arm/ambarella.h>

#if !defined(CONFIG_XEN_SUPPORT_QNX)

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

static void parse_line(unsigned channel, const char *line, unsigned *baud,
        unsigned *clk)
{
    /*
     * Get device base address and register stride
     */
    if (*line != '.' && *line != '\0') {
        dbg_device[channel].base = strtoul(line, (char **) &line, 16);
        if (*line == '^')
            dbg_device[channel].shift = strtoul(line + 1, (char **) &line, 0);
    }

    /*
     * Get baud rate
     */
    if (*line == '.')
        ++line;
    if (*line != '.' && *line != '\0')
        *baud = strtoul(line, (char **) &line, 0);

    /*
     * Get clock rate
     */
    if (*line == '.')
        ++line;
    if (*line != '.' && *line != '\0')
        *clk = strtoul(line, (char **) &line, 0);
}

/*
 * Initialise one of the serial ports
 */
void Amba_init_Uart(unsigned channel, const char *init, const char *defaults)
{
    unsigned int baud, clk;

    parse_line(channel, defaults, &baud, &clk);
    parse_line(channel, init, &baud, &clk);
}

static void UartGetTxEmpty(unsigned int *pTxEmpty)
{
    AMBA_UART_REG_s *pUartReg;
    pUartReg = (AMBA_UART_REG_s *)dbg_device[0].base;

    if ((pUartReg->LineStatus & 0x40U) != 0UL) {
        pUartReg->ResetCtrl = 4U;
        *pTxEmpty = 1UL;
    } else {
        *pTxEmpty = 0UL;
    }
}

/*
 * Send a character
 */
void Amba_put_char(int c)
{
    unsigned base = dbg_device[0].base;
    unsigned int TxEmpty = 0;
    AMBA_UART_REG_s *pUartReg;
    pUartReg = (AMBA_UART_REG_s *)dbg_device[0].base;

    do {
        (void)UartGetTxEmpty(&TxEmpty);
    } while (TxEmpty == 0U);

    if (c == '\n') {
        pUartReg->DataBuf = '\r';
    }
    out32(base + AMBA_UART_FIFO_REG, c);
}

#else

extern void _flush_d_cache_range(void *addr, unsigned int size);
extern void HYPERVISOR_console_io(int op, int len, char *buf);

static char outbuf[256];
static int outlen = 0;

void xen_ambarella_console_init(unsigned int channel,
                                const char *init, const char *defaults)
{
    outlen = 0;
    (void) (channel);
    (void) (init);
    (void) (defaults);
}

void xen_ambarella_console_putc(int c)
{
    outbuf[outlen] = (char) c;
    outlen++;

    if ('\n' == (char) c || outlen >= sizeof(outbuf)) {
        _flush_d_cache_range(outbuf, sizeof(outbuf));
        HYPERVISOR_console_io(0, outlen, outbuf);
        outlen = 0;
    }
}

#endif

/*
 * Local variables:
 * mode: C
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
