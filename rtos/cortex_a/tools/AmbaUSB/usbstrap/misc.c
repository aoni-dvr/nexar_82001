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
#include <common.h>
#include <io.h>

void pinmux_init(const unsigned int (*pins)[2], int num)
{
	unsigned int i, j, pin, mux;

	for (i = 0; i < num; i++) {
		pin = pins[i][0];
		mux = pins[i][1];

		for (j = 0; j < 3; j++) {
			if (mux & 1 << j)
				setbitsl(IOMUX_BASE + (pin >> 5) * 0xc + j * 0x4,
						1 << (pin % 32));
			else
				clrbitsl(IOMUX_BASE + (pin >> 5) * 0xc + j * 0x4,
						1 << (pin % 32));
		}
	}

	writel(IOMUX_BASE + 0xF0, 0x1);
	writel(IOMUX_BASE + 0xF0, 0x0);
}

void mdelay(unsigned int ms)
{
	unsigned int value = REF_CLK_FREQ * ms / 1000;

	writel(RCT_BASE + 0x258, 0x1);
	writel(RCT_BASE + 0x258, 0x0);

	while(readl(RCT_BASE + 0x254) < value)
		;
}

void udelay(unsigned int us)
{
	unsigned int value = (REF_CLK_FREQ / 1000000) * us;

	writel(RCT_BASE + 0x258, 0x1);
	writel(RCT_BASE + 0x258, 0x0);

	while(readl(RCT_BASE + 0x254) < value)
		;
}

void memory_tester(void)
{
#define	MEMTEST_SIZE		0x100000
#define MEMTEST_MAGIC		0x5A5A5A5A
	int i;
	unsigned int *mem;
	mem = (unsigned int *)CONFIG_DEFAULT_LOADADDR;

	const char *error = "Data corruption, please check the DRAM parameter\r\n";

	for (i = 0 ; i < MEMTEST_SIZE / 4; i++)
		mem[i] = MEMTEST_MAGIC;

	for (i = 0; i < MEMTEST_SIZE / 4; i++)
		if (mem[i] != MEMTEST_MAGIC) {
			uart_putstr(error);
			__asm__ volatile("b	.");
		}
}

void setup_vector(void)
{
	int i;
	unsigned int *vector = (unsigned int *)0UL;

	for (i = 0 ; i < 16; i++)
		vector[i * 0x20] = 0x14000000;		/* `b	.` */

	asm volatile("msr vbar_el3, %0":"+r"(vector)::"memory");
}
