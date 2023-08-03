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
#include <arch_cv5/drctl.h>

#define CG_UART_OFFSET(port)	\
	((port) == 0 ? 0x038 :	\
	 (port) == 1 ? 0x714 :	\
	 (port) == 2 ? 0x718 :	\
	 (port) == 3 ? 0x71c :	\
	 0x720)

static const unsigned int init_pins[][2] = {
	{10, 1},{11, 1},
};

void plat_usb_reset(void)
{
	setbitsl(AHBSP_NS_BASE + 0x12C, 0x1);
	mdelay(1);
	clrbitsl(AHBSP_NS_BASE + 0x12c, 0x1);
	mdelay(1);
}

void plat_set_uart_pll_div(int port, unsigned int div)
{
	writel(RCT_BASE + CG_UART_OFFSET(port), div);
}

void plat_enable_usb(void)
{
	setbitsl(RCT_BASE + 0x050, 0x6);
}

void plat_pinmux_init(void)
{
	pinmux_init(init_pins, ARRAY_SIZE(init_pins));
}

int plat_get_dram_size_mb(void)
{
	unsigned int value, config_reg, mode_reg;
	unsigned long long dram_size;

	config_reg = readl(DRAMC_DDRC_BASE + DRAM_CONFIG1);
	mode_reg = readl(DRAMC_DRAM_BASE + 0x0);
	value = (config_reg & DRAM_CONFIG_DRAM_SIZE_MASK) >> DRAM_CONFIG_DRAM_SIZE_SHIFT;
	dram_size = (value & (1 << 0)) ? 0x3UL << 30 /* 3Gb */ : 0x2UL << 30 /* 2Gb */;
	dram_size = dram_size * (2 << (value >> 1));
	dram_size = (config_reg & (1 << 26)) ? dram_size * 2 : dram_size;
	dram_size = (config_reg & (1 << 6)) ? dram_size * 2 : dram_size;
	dram_size = ((mode_reg & 0x6) == 0x6) ? dram_size * 2 : dram_size;

	/* convert to MB */
	return dram_size >> 23;
}

