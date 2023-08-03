/**
 * Copyright (c) 2021 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <config.h>
#include <memory.h>
#include <uart.h>
#include <io.h>
#include <common.h>

#define UART_CONSOLE		CONFIG_UART_PORT
#define UART_PLL_DIV		1

static void __uart_init(int baud)
{
	unsigned int clk;
	unsigned short dl;

	/* set uart pll divider */
	plat_set_uart_pll_div(UART_CONSOLE, UART_PLL_DIV);

	writeb(UART_REG(UART_CONSOLE, UART_SRR_OFFSET), 0x00);

	clk = REF_CLK_FREQ / UART_PLL_DIV;

	dl = clk * 10 / baud / 16;
	if (dl % 10 >= 5)
		dl = (dl / 10) + 1;
	else
		dl = (dl / 10);

	writeb(UART_REG(UART_CONSOLE, UART_LC_OFFSET), UART_LC_DLAB);
	writeb(UART_REG(UART_CONSOLE, UART_DLL_OFFSET), dl & 0xff);
	writeb(UART_REG(UART_CONSOLE, UART_DLH_OFFSET), dl >> 8);
	writeb(UART_REG(UART_CONSOLE, UART_LC_OFFSET), UART_LC_8N1);

}
static int __uart_poll(void)
{
	return !!(readb(UART_REG(UART_CONSOLE, UART_LS_OFFSET)) & UART_LS_DR);
}

static void __uart_putchar(char c)
{
	while (!(readb(UART_REG(UART_CONSOLE, UART_LS_OFFSET)) & UART_LS_TEMT));
	writeb(UART_REG(UART_CONSOLE, UART_TH_OFFSET), c);
}

static int __uart_getchar(void)
{
	while (!(__uart_poll()));

	return readb(UART_REG(UART_CONSOLE, UART_RB_OFFSET));
}

static void __uart_flush_input(void)
{
	while (__uart_poll()) {
		readb(UART_REG(UART_CONSOLE, UART_RB_OFFSET));
	}
}

static void __uart_putstr(const char *str)
{
	const char *c = str;

	while (*c) {
		if (*c == '\n')
			__uart_putchar('\r');
		__uart_putchar(*c);
		c++;
	}
}

void uart_init(void)
{
	__uart_init(CONFIG_UART_BAUDRATE);
}

void uart_putchar(char c)
{
	__uart_putchar(c);
}

int uart_getchar(void)
{
	return __uart_getchar();
}

void uart_putstr(const char *str)
{
	__uart_putstr(str);
}
void uart_flush_input(void)
{
	__uart_flush_input();
}
