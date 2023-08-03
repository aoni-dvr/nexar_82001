/*
 * drivers/mmc/host/ambarella_sd_para.c
 *
 * Copyright (C) 2016, Ambarella, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/mmc/slot-gpio.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/debugfs.h>
#include <linux/proc_fs.h>
#include <linux/regmap.h>
#include <linux/mfd/syscon.h>

static struct regmap *reg_rct = NULL;
static size_t sdio_regbase = 0;

#define SD_HOST_HIGH_SPEED            0x04
#define SDIO_DELAY_SEL0 (sdio_regbase + 0xd8)
#define SDIO_DELAY_SEL2 (sdio_regbase + 0xdc)
#define SDIO_HOST_OFFSET (sdio_regbase + 0x28)

//[29:27] D3 output delay control
#define SDIO_SEL0_OD3LY 27
//[23:21] D2 output delay control
#define SDIO_SEL0_OD2LY 21
//[17:15] D1 output delay control
#define SDIO_SEL0_OD1LY 15
//[11:9]  D0 output delay control
#define SDIO_SEL0_OD0LY 9
//[26:24] D3 input delay control
#define SDIO_SEL0_ID3LY 24
//[20:18] D2 input delay control
#define SDIO_SEL0_ID2LY 18
//[14:12] D1 input delay control
#define SDIO_SEL0_ID1LY 12
//[8:6]   D0 input delay control
#define SDIO_SEL0_ID0LY 6
//[5:3]   CMD output delay control
#define SDIO_SEL0_OCMDLY 3
//[2:0]   CMD input delay control
#define SDIO_SEL0_ICMDLY 0
//[24:22] clk_sdcard output delay control
#define SDIO_SEL2_OCLY 22

int init_sd_para(size_t regbase, struct device_node *np)
{
	sdio_regbase = regbase;

	reg_rct = syscon_regmap_lookup_by_phandle(np, "amb,rct-regmap");
	if (IS_ERR(reg_rct)) {
		printk(KERN_ERR "%s: err amb,rct-regmap\n", __func__);
		return -1;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(init_sd_para);

int read_rct_reg(unsigned int offset)
{
	int val;

	if (!reg_rct)
		return -1;

	regmap_read(reg_rct, offset, &val);

	return val;
}

int write_rct_reg(unsigned int offset, u32 value)
{
	if (!reg_rct)
		return -1;

	return regmap_update_bits(reg_rct, offset, 0xffffffff, value);
}

static int sdio_info_proc_read(struct seq_file *m, void *v)
{
	u32 reg, ds1;

	reg = readl_relaxed((void *)SDIO_HOST_OFFSET);
	pr_debug("readl 0x%zx = 0x%x\n", SDIO_HOST_OFFSET, reg);
	if (reg & SD_HOST_HIGH_SPEED)
		seq_printf(m, "SDIO high speed mode:            yes\n");
	else
		seq_printf(m, "SDIO high speed mode:            no\n");

	reg = readl_relaxed((void *)SDIO_DELAY_SEL0);
	ds1 = readl_relaxed((void *)SDIO_DELAY_SEL2);
	pr_debug("readl 0x%zx = 0x%x\n", SDIO_DELAY_SEL0, reg);
	pr_debug("readl 0x%zx = 0x%x\n", SDIO_DELAY_SEL2, reg);
	//SDIO output data delay control, assume all bits have same value
	seq_printf(m, "SDIO output data delay:          %u\n",
		(reg & (0x7 << SDIO_SEL0_OD0LY)) >> SDIO_SEL0_OD0LY);
	//SDIO output clock delay control
	seq_printf(m, "SDIO output clock delay:         %u\n",
		(ds1 & (0x7 << SDIO_SEL2_OCLY)) >> SDIO_SEL2_OCLY);
	//SDIO input data delay control, assume all bits have same value
	seq_printf(m, "SDIO input data delay:           %u\n",
		(reg & (0x7 << SDIO_SEL0_ID0LY)) >> SDIO_SEL0_ID0LY);

	return 0;
}

static int sdio_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, sdio_info_proc_read, NULL);
}

const struct proc_ops proc_fops_sdio_info = {
	.proc_open = sdio_info_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};
EXPORT_SYMBOL_GPL(proc_fops_sdio_info);

/*
 * return 0 if timing changed
 */
int amba_sdio_delay_post_apply(const int odly, const int ocly, const int idly)
{
	u32 reg_ori, reg_new;
	int i, retval;
	int biti;

	reg_ori = readl_relaxed((void *)SDIO_DELAY_SEL0);
	reg_new = reg_ori;
	pr_debug("readl 0x%zx = 0x%08x\n", SDIO_DELAY_SEL0, reg_ori);

	//SDIO output data delay control
	if (-1 != odly) {
		for (i = 0; i < 3; i++) {
			biti = odly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_OD0LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_OD0LY + i);
		}
		for (i = 0; i < 3; i++) {
			biti = odly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_OD1LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_OD1LY + i);
		}
		for (i = 0; i < 3; i++) {
			biti = odly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_OD2LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_OD2LY + i);
		}
		for (i = 0; i < 3; i++) {
			biti = odly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_OD3LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_OD3LY + i);
		}
	}

	//SDIO input data delay control
	if (-1 != idly) {
		for (i = 0; i < 3; i++) {
			biti = idly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_ID0LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_ID0LY + i);
		}
		for (i = 0; i < 3; i++) {
			biti = idly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_ID1LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_ID1LY + i);
		}
		for (i = 0; i < 3; i++) {
			biti = idly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_ID2LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_ID2LY + i);
		}
		for (i = 0; i < 3; i++) {
			biti = idly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL0_ID3LY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL0_ID3LY + i);
		}
	}

	if (reg_ori != reg_new) {
		writel_relaxed(reg_new, (void *)SDIO_DELAY_SEL0);
		pr_debug("writel 0x%zx 0x%x\n", SDIO_DELAY_SEL0, reg_new);
		retval = 0;
	} else
		retval = 1;

	reg_ori = readl_relaxed((void *)SDIO_DELAY_SEL2);
	reg_new = reg_ori;
	pr_debug("readl 0x%zx = 0x%08x\n", SDIO_DELAY_SEL2, reg_ori);

	//SDIO output clock delay control
	if (-1 != ocly) {
		for (i = 0; i < 3; i++) {
			biti = ocly >> i;
			if (biti & 1)
				reg_new = reg_new | BIT(SDIO_SEL2_OCLY + i);
			else
				reg_new = reg_new & ~BIT(SDIO_SEL2_OCLY + i);
		}
	}

	if (reg_ori != reg_new) {
		writel_relaxed(reg_new, (void *)SDIO_DELAY_SEL2);
		pr_debug("writel 0x%zx 0x%x\n", SDIO_DELAY_SEL2, reg_new);
	} else
		retval = 1;

	return retval;
}
EXPORT_SYMBOL_GPL(amba_sdio_delay_post_apply);

int ambarella_set_sdio_host_high_speed(const char *str, const struct kernel_param *kp)
{
	int retval;
	int value;
	u8 hostr;

	param_set_int(str, kp);
	retval = kstrtos32(str, 10, &value);
	hostr = readb_relaxed((void *)SDIO_HOST_OFFSET);
	pr_debug("readb_relaxed 0x%zx = 0x%x\n", SDIO_HOST_OFFSET, hostr);

	if(value == 1)
		hostr |= SD_HOST_HIGH_SPEED;
	else if(value == 0)
		hostr &= ~SD_HOST_HIGH_SPEED;

	writeb_relaxed(hostr, (void *)SDIO_HOST_OFFSET);
	pr_debug("writeb 0x%zx 0x%x\n", SDIO_HOST_OFFSET, hostr);
	return retval;
}
EXPORT_SYMBOL_GPL(ambarella_set_sdio_host_high_speed);

int ambarella_set_sdio_host_odly(const char *str, const struct kernel_param *kp)
{
	int retval;
	int value;

	param_set_int(str, kp);
	retval = kstrtos32(str, 10, &value);
	amba_sdio_delay_post_apply(value, -1, -1);

	return retval;
}
EXPORT_SYMBOL_GPL(ambarella_set_sdio_host_odly);

int ambarella_set_sdio_host_ocly(const char *str, const struct kernel_param *kp)
{
	int retval;
	int value;

	param_set_int(str, kp);
	retval = kstrtos32(str, 10, &value);
	amba_sdio_delay_post_apply(-1, value, -1);

	return retval;
}
EXPORT_SYMBOL_GPL(ambarella_set_sdio_host_ocly);

int ambarella_set_sdio_host_idly(const char *str, const struct kernel_param *kp)
{
	int retval;
	int value;

	param_set_int(str, kp);
	retval = kstrtos32(str, 10, &value);
	amba_sdio_delay_post_apply(-1, -1, value);

	return retval;
}
EXPORT_SYMBOL_GPL(ambarella_set_sdio_host_idly);

MODULE_DESCRIPTION("Ambarella Media Processor SD/MMC Host Controller Parameters");
MODULE_LICENSE("GPL");
