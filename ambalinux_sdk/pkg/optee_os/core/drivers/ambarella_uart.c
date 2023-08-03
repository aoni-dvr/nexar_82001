/*
 * Copyright (C) 2017 Ambarella Inc.
 * All rights reserved.
 *
 * Author: Cao Rongrong <rrcao@ambarella.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <drivers/ambarella_uart.h>
#include <io.h>
#include <keep.h>
#include <util.h>

#define D_DISABLE_UART

#define UART_RB_OFFSET			0x00
#define UART_TH_OFFSET			0x00
#define UART_DLL_OFFSET			0x00
#define UART_IE_OFFSET			0x04
#define UART_DLH_OFFSET			0x04
#define UART_II_OFFSET			0x08
#define UART_FC_OFFSET			0x08
#define UART_LC_OFFSET			0x0c
#define UART_MC_OFFSET			0x10
#define UART_LS_OFFSET			0x14
#define UART_MS_OFFSET			0x18
#define UART_SC_OFFSET			0x1c	/* Byte */
#define UART_DMAE_OFFSET		0x28
#define UART_DMAF_OFFSET		0x40	/* DMA fifo */
#define UART_US_OFFSET			0x7c
#define UART_TFL_OFFSET			0x80
#define UART_RFL_OFFSET			0x84
#define UART_SRR_OFFSET			0x88


#define UART_LS_TEMT			0x40
#define UART_LS_DR			0x01

static vaddr_t chip_to_base(struct serial_chip *chip)
{
	struct ambarella_uart_data *pd =
		container_of(chip, struct ambarella_uart_data, chip);

	return io_pa_or_va(&pd->base, SIZE_4K);
}

static void ambarella_uart_flush(struct serial_chip *chip)
{
#ifdef D_DISABLE_UART
	return;
#else
	vaddr_t base = chip_to_base(chip);

	while(!(io_read8(base + UART_LS_OFFSET) & UART_LS_TEMT));
#endif
}

static int ambarella_uart_getchar(struct serial_chip *chip)
{
#ifdef D_DISABLE_UART
	return 0;
#else
	vaddr_t base = chip_to_base(chip);

	while(!(io_read8(base + UART_LS_OFFSET) & UART_LS_DR));

	return io_read8(base + UART_RB_OFFSET);
#endif
}

static void ambarella_uart_putc(struct serial_chip *chip, int ch)
{
#ifdef D_DISABLE_UART
	return;
#else
	vaddr_t base = chip_to_base(chip);

	while(!(io_read8(base + UART_LS_OFFSET) & UART_LS_TEMT));

	io_write8(base + UART_TH_OFFSET, ch);
#endif
}

static const struct serial_ops ambarella_uart_ops = {
	.flush = ambarella_uart_flush,
	.getchar = ambarella_uart_getchar,
	.putc = ambarella_uart_putc,
};

void ambarella_uart_init(struct ambarella_uart_data *pd, paddr_t base)
{
	pd->base.pa = base;
	pd->chip.ops = &ambarella_uart_ops;

	/*
	 * Do nothing, debug uart share with normal world,
	 * everything for uart initialization is done in bootloader.
	 */
}
