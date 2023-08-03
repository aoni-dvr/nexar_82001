/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <lib/mmio.h>
#include <plat_private.h>
#include <drivers/delay_timer.h>

#define REF_CLK_FREQ			24000000UL

#define PLL_CTRL_INTPROG(x)		((x >> 24) & 0x7F)
#define PLL_CTRL_SOUT(x)		((x >> 16) & 0xF)
#define PLL_CTRL_SDIV(x)		((x >> 12) & 0xF)
#define PLL_CTRL_FRAC_MODE(x)		(x & 0x8)
#define PLL_FRAC_VAL(f)			(f & 0x7FFFFFFF)
#define PLL_FRAC_VAL_NEGA(f)		(f & 0x80000000)
#define PLL_SCALER_JDIV(x)		(((x >> 4) & 0xF) + 1)

static uint64_t rct_get_integer_pll_freq(uint32_t ctrl, uint32_t ctrl2, uint32_t pres, uint32_t posts)
{
	uint32_t ctrl2_8, ctrl2_9, ctrl2_11, ctrl2_12;
	uint32_t intp, sout, sdiv;
	uint64_t fvco, freq;

	if (ctrl & 0x20)
		return 0;

	if (ctrl & 0x00200004) {
		intp = REF_CLK_FREQ;
		intp /= pres;
		intp /= posts;
		return intp;
	}

	ctrl2_12 = ((ctrl2 >> 12) & 0x1);
	ctrl2_11 = ((ctrl2 >> 11) & 0x1) + 1;
	ctrl2_9 = ((ctrl2 >> 9) & 0x1) + 1;
	ctrl2_8 = ((ctrl2 >> 8) & 0x1) + 1;

	intp = ((ctrl >> 24) & 0x7f) + 1;
	sout = ((ctrl >> 16) & 0xf) + 1;
	sdiv = ((ctrl >> 12) & 0xf) + 1;

	fvco = REF_CLK_FREQ * ctrl2_8 * ctrl2_9 * sdiv * intp;

	if (ctrl2_12)
		freq = fvco;
	else
		freq = fvco / ctrl2_8 / ctrl2_11 / sout;

	return freq / posts;
}

uint32_t get_sys_timer_parent_freq_hz(void)
{
	uint32_t ctrl, ctrl2;

#if (AXI_SYS_TIMER_INDEPENDENT == 1)
	ctrl = mmio_read_32(RCT_REG(PLL_ENET_CTRL_OFFSET));
	ctrl2 = mmio_read_32(RCT_REG(PLL_ENET_CTRL2_OFFSET));
#else
	ctrl = mmio_read_32(RCT_REG(PLL_CORTEX_CTRL_OFFSET));
	ctrl2 = mmio_read_32(RCT_REG(PLL_CORTEX_CTRL2_OFFSET));
#endif

	return rct_get_integer_pll_freq(ctrl, ctrl2, 1, 1);
}

uint32_t get_core_bus_freq_hz(void)
{
	return rct_get_integer_pll_freq(mmio_read_32(RCT_REG(PLL_CORE_CTRL_OFFSET)),
		mmio_read_32(RCT_REG(PLL_CORE_CTRL2_OFFSET)), 1, 1);
}

uint32_t get_ddr_freq_hz(void)
{
#if defined(AMBARELLA_CV5)
	return rct_get_integer_pll_freq(mmio_read_32(DDRH0_REG(PLL_DDR_CTRL_OFFSET)),
		mmio_read_32(DDRH0_REG(PLL_DDR_CTRL2_OFFSET)), 1, 1) / 2;
#else
	return rct_get_integer_pll_freq(mmio_read_32(RCT_REG(PLL_DDR_CTRL_OFFSET)),
		mmio_read_32(RCT_REG(PLL_DDR_CTRL2_OFFSET)), 1, 1) / 2;
#endif
}

void rct_soft_reset_vp_cluster(void)
{
	mmio_clrbits_32(RCT_REG(CLUSTER_SOFT_RESET_OFFSET),
			CLUSTER_SOFT_RESET_VP);
	mmio_setbits_32(RCT_REG(CLUSTER_SOFT_RESET_OFFSET),
			CLUSTER_SOFT_RESET_VP);
	mmio_clrbits_32(RCT_REG(CLUSTER_SOFT_RESET_OFFSET),
			CLUSTER_SOFT_RESET_VP);
}
