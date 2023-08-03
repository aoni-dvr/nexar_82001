/*
 * Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <lib/mmio.h>
#include <lib/libc/errno.h>
#include <lib/el3_runtime/context_mgmt.h>
#include <bl31/bl31.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <drivers/generic_delay_timer.h>
#include <plat/common/platform.h>
#include <uart_ambarella.h>
#include <plat_private.h>
#include <boot_cookie.h>
#include <libfdt.h>

/*
 * The RCT doc said max vco frequency is 1.8GHz, but there is no
 * much margine left when vco frequency is 1.8GHz, so we limit max
 * vco frequency to 1.6GHz.
 */
#define MAX_VCO_FREQ				1600000000ULL
#define REF_CLK_FREQ				24000000ULL

union ctrl_reg_u {
	struct {
		uint32_t write_enable		: 1;	/* [0] */
		uint32_t reserved1			: 1;	/* [1] */
		uint32_t bypass			: 1;	/* [2] */
		uint32_t frac_mode			: 1;	/* [3] */
		uint32_t force_reset			: 1;	/* [4] */
		uint32_t power_down			: 1;	/* [5] */
		uint32_t halt_vco			: 1;	/* [6] */
		uint32_t tristate			: 1;	/* [7] */
		uint32_t tout_async			: 4;	/* [11:8] */
		uint32_t sdiv			: 4;	/* [15:12] */
		uint32_t sout			: 4;	/* [19:16] */
		uint32_t force_lock			: 1;	/* [20] */
		uint32_t force_bypass		: 1;	/* [21] */
		uint32_t reserved2			: 2;	/* [23:22] */
		uint32_t intp			: 7;	/* [30:24] */
		uint32_t reserved3			: 1;	/* [31] */
	} s;

	uint32_t w;
};

union frac_reg_u {
	struct {
		uint32_t frac			: 31;	/* [30:0] */
		uint32_t nega			: 1;	/* [31] */
	} s;

	uint32_t w;
};

union ctrl2_reg_u {
	struct {
		uint32_t vco_div			: 4;	/* [3:0] */
		uint32_t fsdiv			: 4;	/* [7:4] */
		uint32_t fsout			: 4;	/* [11:8] */
		uint32_t bypass_hs_div		: 1;	/* [12] */
		uint32_t bypass_mdiv			: 1;	/* [13] */
		uint32_t diff_vco_en			: 1;	/* [14] */
		uint32_t duty_cycle_tune		: 1;	/* [15] */
		uint32_t charge_pump_cur		: 8;	/* [23:16] */
		uint32_t reserved1			: 4;	/* [27:24] */
		uint32_t change_pump_dc_bias		: 2;	/* [29:28] */
		uint32_t dsm_mode_ctrl		: 2;	/* [31:30] */
	} s;

	uint32_t w;
};

union ctrl3_reg_u {
	struct {
		uint32_t reserved1			: 1;	/* [0] */
		uint32_t pll_vco_range		: 2;	/* [2:1] */
		uint32_t pll_vco_clamp		: 2;	/* [4:3] */
		uint32_t reserved2			: 2;	/* [6:5] */
		uint32_t dsm_dither_gain		: 2;	/* [8:7] */
		uint32_t reserved3			: 4;	/* [12:9] */
		uint32_t ff_zero_resistor_sel	: 4;	/* [16:13] */
		uint32_t bias_current_ctrl		: 3;	/* [19:17] */
		uint32_t bypass_jdiv			: 1;	/* [20] */
		uint32_t bypass_jout			: 1;	/* [21] */
		uint32_t reserved4			: 10;	/* [31:22] */
	} s;

	uint32_t w;
};

enum {
	PHANDLE_OFFSET = 0,
	CTRL_OFFSET,
	FRAC_OFFSET,
	CTRL2_OFFSET,
	CTRL3_OFFSET,
	PRES_OFFSET,
	POST_OFFSET,
	REG_NUM,
};

struct amb_clk_pll {
	char clk_name[32];
	uint32_t reg_offset[REG_NUM];

	uint32_t parsed_done : 1;
	uint32_t extra_pre_scaler : 1;
	uint32_t extra_post_scaler : 1;
	uint32_t frac_mode : 1;
	uint32_t frac_nega : 1;

	uint32_t ctrl2_val;
	uint32_t ctrl3_val;
	uint32_t fix_divider;
	unsigned long max_vco;
};

static uint32_t div64_32(uint64_t *n, uint32_t base)
{
	uint32_t rem;
	uint64_t divdend = (*n), part;

	part = divdend / base;
	rem = divdend % base;
	(*n) = part;

	return rem;
}

#define do_div(n,base) ({			\
			unsigned int __rem;	\
			__rem = div64_32(&(n), base);	\
			__rem;						\
		 })

#define DIV_ROUND_CLOSEST_ULL(x, divisor)(		\
{							\
	typeof(divisor) __d = divisor;			\
	unsigned long long _tmp = (x) + (__d) / 2;	\
	do_div(_tmp, __d);				\
	_tmp;						\
}							\
)

#define roundup(x, y)		round_up(x, y)
#define rounddown(x, y)		round_down(x, y)

static char *strncpy(char *dest, const char *src, unsigned int n)
{
	while (n > 0) {
		n--;
		if ((*dest++ = *src++) == '\0')
			break;
	}

	return dest;
}

#define BUG_ON(x, format...)						\
		if ((x)) {						\
			NOTICE("%s(%d): ", __func__, __LINE__);		\
			NOTICE("BUG_ON: "format);			\
			for (;;);					\
		}

static void ambarella_pll_set_ctrl3(struct amb_clk_pll *clk_pll, unsigned long pre_scaler,
		unsigned long intp, unsigned long sdiv, unsigned long parent_rate, unsigned long rate)
{
	uint32_t *reg = clk_pll->reg_offset;
	union ctrl3_reg_u ctrl3_val;
	unsigned long pllvco;

	// tmp hard code for HDMI 4kp60 clock
	if (rate == 5940000000ULL) {
		ctrl3_val.w = 0x88006;
		goto exit;
	}

	if (clk_pll->ctrl3_val != 0) {
		ctrl3_val.w =clk_pll->ctrl3_val;
		goto exit;
	}

	ctrl3_val.w = mmio_read_32(RCT_REG(reg[CTRL3_OFFSET]));

	if (clk_pll->frac_nega) {
		if (clk_pll->frac_mode)
			ctrl3_val.w |= (1 << 12);
		else
			ctrl3_val.w &= ~(1 << 12);
	} else {
		pllvco = parent_rate / pre_scaler * intp * sdiv;

		if (pllvco > 980000000ULL)
			ctrl3_val.s.pll_vco_range = 3;
		else if (pllvco > 700000000ULL)
			ctrl3_val.s.pll_vco_range = 2;
		else if (pllvco > 530000000ULL)
			ctrl3_val.s.pll_vco_range = 1;
		else
			ctrl3_val.s.pll_vco_range = 0;
	}
exit:
	mmio_write_32(RCT_REG(reg[CTRL3_OFFSET]), ctrl3_val.w);
}

static void ambarella_pll_set_ctrl2(struct amb_clk_pll *clk_pll, unsigned long rate)
{
	uint32_t *reg = clk_pll->reg_offset, ctrl2_val;

	// tmp hard code for HDMI 4kp60 clock
	if (rate == 5940000000ULL) {
		ctrl2_val = 0x30520040;
	} else if (clk_pll->ctrl2_val != 0) {
		ctrl2_val = clk_pll->ctrl2_val;
	} else {
		ctrl2_val = mmio_read_32(RCT_REG(reg[CTRL2_OFFSET]));
	}

	mmio_write_32(RCT_REG(reg[CTRL2_OFFSET]), ctrl2_val);
}

static unsigned long ambarella_pll_recalc_rate(struct amb_clk_pll *clk_pll, unsigned long parent_rate)
{
	uint32_t *reg = clk_pll->reg_offset;
	uint32_t pre_scaler, post_scaler, intp, sdiv, sout;
	uint32_t ctrl2_8, ctrl2_9, ctrl2_11, ctrl2_12;
	uint64_t dividend, divider, frac;
	unsigned long rate_ref;
	union ctrl_reg_u ctrl_val;
	union ctrl2_reg_u ctrl2_val;
	union frac_reg_u frac_val;

	ctrl_val.w = mmio_read_32(RCT_REG(reg[CTRL_OFFSET]));
	ctrl2_val.w = mmio_read_32(RCT_REG(reg[CTRL2_OFFSET]));
	frac_val.w = mmio_read_32(RCT_REG(reg[FRAC_OFFSET]));

	if ((ctrl_val.s.power_down == 1) || (ctrl_val.s.halt_vco == 1))
		return 0;

	if (reg[PRES_OFFSET] != 0) {
		pre_scaler = mmio_read_32(RCT_REG(reg[PRES_OFFSET]));
		if (clk_pll->extra_pre_scaler) {
			pre_scaler >>= 4;
			pre_scaler++;
		}
	} else {
		pre_scaler = 1;
	}

	if (reg[POST_OFFSET] != 0) {
		post_scaler = mmio_read_32(RCT_REG(reg[POST_OFFSET]));
		if (clk_pll->extra_post_scaler) {
			post_scaler >>= 4;
			post_scaler++;
		}
	} else {
		post_scaler = 1;
	}

	if (ctrl_val.s.bypass || ctrl_val.s.force_bypass)
		return parent_rate / pre_scaler / post_scaler;

	rate_ref = (ctrl2_val.s.fsdiv == 4) ? parent_rate * 2 : parent_rate;

	ctrl2_12 = ((ctrl2_val.w >> 12) & 0x1);
	ctrl2_11 = ((ctrl2_val.w >> 11) & 0x1) + 1;
	ctrl2_9 = ((ctrl2_val.w >> 9) & 0x1) + 1;
	ctrl2_8 = ((ctrl2_val.w >> 8) & 0x1) + 1;

	intp = ctrl_val.s.intp + 1;
	sdiv = ctrl_val.s.sdiv + 1;
	sout = ctrl_val.s.sout + 1;

	dividend = rate_ref;
	dividend *= (uint64_t)intp;
	dividend *= (uint64_t)sdiv;
	dividend *= (uint64_t)ctrl2_8;
	dividend *= (uint64_t)ctrl2_9;

	if (ctrl_val.s.frac_mode) {
		if (clk_pll->frac_nega) {
			if (frac_val.s.nega) {
				/* Negative */
				frac = 0x80000000 - frac_val.s.frac;
				frac = rate_ref * frac * sdiv;
				frac >>= 32;
				dividend = dividend - frac;
			} else {
				/* Positive */
				frac = frac_val.s.frac;
				frac = rate_ref * frac * sdiv;
				frac >>= 32;
				dividend = dividend + frac;
			}
		} else {
			frac = rate_ref * frac_val.w * sdiv;
			frac >>= 32;
			dividend = dividend + frac;
		}
	}

	if (ctrl2_12)
		divider = clk_pll->fix_divider;
	else
		divider = pre_scaler * sout * ctrl2_8 * ctrl2_11 * post_scaler * clk_pll->fix_divider;

	BUG_ON(divider == 0);

	do_div(dividend, divider);

	return dividend;
}

static void rational_best_approximation(
	unsigned long given_numerator, unsigned long given_denominator,
	unsigned long max_numerator, unsigned long max_denominator,
	unsigned long *best_numerator, unsigned long *best_denominator)
{
	unsigned long multi = 0, dir =0, temp;
	unsigned long delta, max_delta = 0xFFFFFFFF;
	unsigned long index_x = max_numerator;
	unsigned long index_y = max_denominator;
	unsigned long maybe_best_numerator = 0;
	unsigned long maybe_best_denominator = 0;

	if (given_numerator > given_denominator) {
		dir = 1;
	} else {
		temp = given_numerator;
		given_numerator = given_denominator;
		given_denominator = temp;
		dir = 0;
	}

	multi = (given_numerator / given_denominator);
	for (index_x = max_numerator; index_x > 0; index_x--) {
		for(index_y = (index_x / multi);
			((index_y > 0) && (index_y <= max_denominator)); index_y--) {
			if (given_numerator * index_y > given_denominator * index_x) {
				delta = given_numerator * index_y - given_denominator * index_x;
			} else {
				delta = given_denominator * index_x - given_numerator * index_y;
			}
			if (delta < max_delta) {
				max_delta = delta;
				maybe_best_numerator = index_x;
				maybe_best_denominator = index_y;
			}
		}
	}

	if (dir) {
		*best_numerator = maybe_best_numerator;
		*best_denominator = maybe_best_denominator;
	} else {
		*best_numerator = maybe_best_denominator;
		*best_denominator = maybe_best_numerator;
	}
}

static int ambarella_pll_set_rate(struct amb_clk_pll *clk_pll, unsigned long rate,
			unsigned long parent_rate)
{
	uint32_t *reg = clk_pll->reg_offset;
	long distance = 0;
	unsigned long max_numerator, max_denominator;
	unsigned long rate_ref, rate_tmp, rate_resolution, pre_scaler = 1, post_scaler = 1;
	unsigned long intp, sdiv = 1, sout = 1, intp_tmp, sout_tmp;
	uint64_t dividend, divider, diff;
	union ctrl_reg_u ctrl_val;
	union frac_reg_u frac_val;

	if (rate == 0) {
		ctrl_val.w = mmio_read_32(RCT_REG(reg[CTRL_OFFSET]));
		ctrl_val.s.power_down = 1;
		ctrl_val.s.halt_vco = 1;
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w | 0x1);
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
		return 0;
	}

	rate *= clk_pll->fix_divider;

	/*
	 * The RCT doc said:
	 *   If VCO frequency exceeds 3GHz, use PLL_*_CTRL2_REG[2] = 1 and use
	 *   half the frequency setting in PLL_*_CTRL_REG".
	 */
	rate_ref = rate > 3000000000ULL ? parent_rate * 2 : parent_rate;

	if (rate < rate_ref && (reg[POST_OFFSET] != 0)) {
		rate *= 16;
		post_scaler = 16;
	}

	if (rate < rate_ref) {
		NOTICE("%s: Error: target rate is too slow: %ld!\n",
				clk_pll->clk_name, rate);
		return -EINVAL;
	}

	/*
	 * Non-HDMI fvco should be less than 1.6GHz and higher than 700MHz.
	 * HDMI fvco should be less than 5.5GHz, and much higher than 700MHz,
	 * probably higher than 2GHz, but not sure, need VLSI's confirmation.
	 *
	 * The pll_vco_range in CTRL3 register need to match the VCO frequency.
	 * Note: If the VCO frequency is larger than 1.6GHz, please take a look
	        at the CTRL2 and CTRL3 again(Especially the pll_vco_range field).
	 *
	 * In addition, for 10nm and later chips, the formula of PLL calculation
	 * is changed, there is no negative frac any more.
	 */
	rate_tmp = rate;
	max_numerator = MIN(128ULL, clk_pll->max_vco / REF_CLK_FREQ);
	max_denominator = 16;
	rational_best_approximation(rate_tmp, rate_ref, max_numerator, max_denominator,
				&intp, &sout);

	if (!clk_pll->frac_nega) {
		rate_resolution = rate_ref / post_scaler / 16;
		/*
		 * 10nm chips don't have negative fraction mode, so the
		 * calculated rate must be less than the required rate.
		 */
		while (rate_ref * intp * sdiv / sout > rate) {
			rate_tmp -= rate_resolution;
			rational_best_approximation(rate_tmp, rate_ref, max_numerator, max_denominator,
						&intp, &sout);
		}
	}

	intp_tmp = intp;
	sout_tmp = sout;

	while (rate_ref / 1000000 * intp * sdiv / pre_scaler < 700) {
		if (sout > 8 || intp > 64)
			break;
		intp += intp_tmp;
		sout += sout_tmp;
	}

	BUG_ON(intp > max_numerator || sout > max_denominator || sdiv > 16);
	BUG_ON(pre_scaler > 16 || post_scaler > 16);

	if (reg[PRES_OFFSET] != 0) {
		if (clk_pll->extra_pre_scaler == 1) {
			mmio_write_32(RCT_REG(reg[PRES_OFFSET]), (pre_scaler - 1) << 4);
			mmio_write_32(RCT_REG(reg[PRES_OFFSET]), ((pre_scaler - 1) << 4) | 0x1);
			mmio_write_32(RCT_REG(reg[PRES_OFFSET]), (pre_scaler - 1) << 4);
		} else
			mmio_write_32(RCT_REG(reg[PRES_OFFSET]), pre_scaler);
	}

	if (reg[POST_OFFSET] != 0) {
		if (clk_pll->extra_post_scaler == 1) {
			mmio_write_32(RCT_REG(reg[POST_OFFSET]), (post_scaler - 1) << 4);
			mmio_write_32(RCT_REG(reg[POST_OFFSET]), ((post_scaler - 1) << 4) | 0x1);
			mmio_write_32(RCT_REG(reg[POST_OFFSET]), (post_scaler - 1) << 4);
		} else
			mmio_write_32(RCT_REG(reg[POST_OFFSET]), post_scaler);
	}

	ctrl_val.w = mmio_read_32(RCT_REG(reg[CTRL_OFFSET]));
	if(ctrl_val.s.frac_mode) {
		ctrl_val.s.force_reset = 1;
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), (ctrl_val.w) | 0x1);
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
	}


	ctrl_val.s.intp = intp - 1;
	ctrl_val.s.sdiv = sdiv - 1;
	ctrl_val.s.sout = sout - 1;
	ctrl_val.s.bypass = 0;
	ctrl_val.s.frac_mode = 0;
	ctrl_val.s.force_reset = 0;
	ctrl_val.s.power_down = 0;
	ctrl_val.s.halt_vco = 0;
	ctrl_val.s.tristate = 0;
	ctrl_val.s.force_lock = 1;
	ctrl_val.s.force_bypass = 0;
	ctrl_val.s.write_enable = 0;
	mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
	mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), (ctrl_val.w) | 0x1);
	mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);

	ambarella_pll_set_ctrl2(clk_pll, rate);
	ambarella_pll_set_ctrl3(clk_pll, pre_scaler, intp, sdiv, rate_ref, rate);

	if (clk_pll->frac_mode) {
		rate_tmp = ambarella_pll_recalc_rate(clk_pll, parent_rate);
		rate_tmp *= clk_pll->fix_divider * post_scaler;
		if (rate_tmp <= rate)
			diff = rate - rate_tmp;
		else
			diff = rate_tmp - rate;

		if (diff) {
			dividend = diff * pre_scaler * sout;
			dividend = dividend << 32;
			divider = (uint64_t)sdiv * rate_ref;
			dividend = DIV_ROUND_CLOSEST_ULL(dividend, divider);
			if (clk_pll->frac_nega) {
				if (rate_tmp <= rate) {
					frac_val.s.nega	= 0;
					frac_val.s.frac	= dividend;
				} else {
					frac_val.s.nega	= 1;
					frac_val.s.frac	= 0x80000000 - dividend;
				}
			} else {
				frac_val.w = dividend;
			}
			mmio_write_32(RCT_REG(reg[FRAC_OFFSET]), frac_val.w);
			ctrl_val.s.frac_mode = 1;
		}

		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), (ctrl_val.w) | 0x1);
		mmio_write_32(RCT_REG(reg[CTRL_OFFSET]), ctrl_val.w);
	}

#define ABS(x) ((x) < 0 ? -(x):(x))

	/* check if result rate is precise or not */
	rate_tmp = ambarella_pll_recalc_rate(clk_pll, parent_rate);
	distance = rate_tmp - (rate / clk_pll->fix_divider / post_scaler);
	if (ABS(distance) > 10) {
		NOTICE("[Warning] %s: request %lu, but got %lu\n",
			clk_pll->clk_name,
			(unsigned long)(rate / clk_pll->fix_divider / post_scaler), rate_tmp);
	}

	NOTICE(" cpufreq adjust done \n");
	return 0;
}

/*****************************************************************************/

static struct amb_clk_pll gclk_cortex_pll;	/* 0: cortex */
static struct amb_clk_pll gclk_core_pll;	/* 1: core */
static struct amb_clk_pll *secure_clk_pll[] = {
	&gclk_cortex_pll,
	&gclk_core_pll,
	NULL,
};

static struct amb_clk_pll *ambarella_find_secure_clk_pll(unsigned long clk_idx)
{
	if (clk_idx > 2) {
		return NULL;
	}

	return secure_clk_pll[clk_idx];
}

static void security_cpufreq_parse(void *fdt, int32_t offset, struct amb_clk_pll *clk_pll)
{
	const fdt32_t *reg;
	int32_t i, lenp = 0;
	uint64_t base_offset = 0;
	const char *clkname = NULL;

	if (offset < 0)
		return;

	clkname = fdt_stringlist_get(fdt, offset, "clock-output-names", 0, &lenp);
	memset(clk_pll->clk_name, 0, sizeof(clk_pll->clk_name));
	strncpy(clk_pll->clk_name, clkname, sizeof(clk_pll->clk_name) - 1);

	INFO("clk_pll->clk_name = %s \n", clk_pll->clk_name);

	clk_pll->extra_pre_scaler = !!fdt_get_property(fdt, offset, "amb,extra-pre-scaler", NULL);
	clk_pll->extra_post_scaler = !!fdt_get_property(fdt, offset, "amb,extra-post-scaler", NULL);
	clk_pll->frac_mode = !!fdt_get_property(fdt, offset, "amb,frac-mode", NULL);
	clk_pll->frac_nega = !!fdt_get_property(fdt, offset, "amb,frac-nega", NULL);

	reg = fdt_getprop(fdt, offset, "amb,ctrl2-val", NULL);
	if (!reg) {
		clk_pll->ctrl2_val = 0;
	} else
		clk_pll->ctrl2_val = fdt32_to_cpu(*reg);

	reg = fdt_getprop(fdt, offset, "amb,ctrl3-val", NULL);
	if (!reg) {
		clk_pll->ctrl3_val = 0;
	} else
		clk_pll->ctrl3_val = fdt32_to_cpu(*reg);

	reg = fdt_getprop(fdt, offset, "amb,fix-divider", NULL);
	if (!reg) {
		clk_pll->fix_divider = 1;
	} else
		clk_pll->fix_divider = fdt32_to_cpu(*reg);

	do {
		reg = fdt_getprop(fdt, offset, "amb,max-vco", NULL);
		if (reg) {
			clk_pll->max_vco = fdt32_to_cpu(*reg);
			break;
		}
		reg = fdt_getprop(fdt, offset, "amb,max-vco-mhz", NULL);
		if (reg) {
			clk_pll->max_vco = fdt32_to_cpu(*reg) * 1000000ULL;
			break;
		}
		clk_pll->max_vco = MAX_VCO_FREQ;
	} while (0);

	if (fdt_get_property(fdt, offset, "reg", &lenp)) {
		base_offset = ambarella_node_addr_base(fdt, offset);
		reg = fdt_getprop(fdt, offset, "reg", &lenp);
		for (i = 0; i < lenp / 8U; i++, reg += 2) {
			/* +1 to rsved space for 'phandle' */
			if (fdt32_to_cpu(*reg) != 0)
				clk_pll->reg_offset[i + 1] = (uint32_t)(fdt32_to_cpu(*reg) + base_offset - RCT_BASE);
			else
				clk_pll->reg_offset[i + 1] = 0;
		}
		clk_pll->parsed_done = 1;
	} else if (fdt_get_property(fdt, offset, "amb,clk-regmap", &lenp)) {
		reg = fdt_getprop(fdt, offset, "amb,clk-regmap", &lenp);
		/* +1 to rsved space for 'phandle' */
		for (i = 1; i < lenp / 4U; i++, reg++) {
			clk_pll->reg_offset[i] = fdt32_to_cpu(*reg);
		}
		clk_pll->parsed_done = 1;
	}
}

void ambarella_cpufreq_parse(void)
{
	void *fdt;
	int32_t offset, lenp;
	const fdt32_t *phandle;

	if (!ambarella_is_secure_boot())
		return;

	fdt = (void *)(uintptr_t)(boot_cookie_ptr()->dtb_ram_start);
	if (!fdt)
		return;

	offset = fdt_path_offset(fdt, "/cpufreq");
	if (offset < 0)
		return;

	phandle = fdt_getprop(fdt, offset, "clocks", &lenp);
	if (!phandle)
		return;

	memset(&gclk_cortex_pll, 0, sizeof(gclk_cortex_pll));
	memset(&gclk_core_pll, 0, sizeof(gclk_core_pll));

	/* cortex */
	offset = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(phandle[0]));
	security_cpufreq_parse(fdt, offset, &gclk_cortex_pll);

	/* core */
	offset = fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(phandle[1]));
	security_cpufreq_parse(fdt, offset, &gclk_core_pll);
}

int ambarella_secure_cpufreq_update(unsigned long clk_idx,
		unsigned long rate, unsigned long parent_rate)
{
	struct amb_clk_pll *clk_pll;

	clk_pll = ambarella_find_secure_clk_pll(clk_idx);
	if (!clk_pll) {
		ERROR(" No secure clock pll, Check again \n");
		return -EINVAL;
	}

	if (clk_pll->parsed_done)
		return ambarella_pll_set_rate(clk_pll, rate, parent_rate);
	else
		return -EINVAL;
}
