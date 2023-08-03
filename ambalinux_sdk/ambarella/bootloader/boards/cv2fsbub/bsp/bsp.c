/**
 * Author: Cao Rongrong <rrcao@ambarella.com>
 *
 * Copyright (c) 2017 Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <bldfunc.h>
#include <ambhw/gpio.h>
#include <ambhw/sd.h>
#include <ambhw/idc.h>
#include <sdmmc.h>
#include <peripheral.h>

int amboot_bsp_hw_init(void)
{
#if defined(CONFIG_AMBOOT_ENABLE_IDC)
	idc_bld_init(IDC_MASTER4, 100000);
#endif

#if defined(CONFIG_AMBOOT_ENABLE_PCA953X)
	pca9539_port_init(IDC_MASTER4);
#endif

	return 0;
}

int amboot_bsp_entry(flpart_table_t *pptb)
{
	int retval = 0;
	flpart_table_t ptb;

	/* Read the partition table */
	retval = flprog_get_part_table(&ptb);
	if (retval < 0) {
		return retval;
	}

	/* BIOS boot */
	if (ptb.dev.rsv[0] > 0) {
		printf("Find BIOS boot flag\n");
		retval = ptb.dev.rsv[0];
	}

	return retval;
}

int amboot_bsp_sd_slot_init(int slot, int volt)
{
	int gpio_pwr = (slot == 0) ? GPIO(109) : GPIO(110);
	int gpio_1v8 = (slot == 0) ? GPIO(156) : GPIO(157);

	if (volt == SDMMC_VOLTAGE_3V3) {
		gpio_config_sw_out(gpio_1v8);
		gpio_clr(gpio_1v8);

		/* power off, then power on */
		gpio_config_sw_out(gpio_pwr);
		gpio_clr(gpio_pwr);
		rct_timer_dly_ms(10);
		gpio_set(gpio_pwr);
		rct_timer_dly_ms(10);
	} else {
		gpio_config_sw_out(gpio_1v8);
		gpio_set(gpio_1v8);
	}

	return 0;
}

int amboot_bsp_sd_phy_init(int slot, int mode)
{
	return 0;
}

int amboot_bsp_eth_init_pre(void *dev)
{
#if defined(CONFIG_AMBOOT_ENABLE_PCA953X)
	/* power up PHY */
	pca9539_set_gpio(3, 11, 1);
#endif
	return 0;
}

int amboot_bsp_eth_init_post(void *dev)
{
	return 0;
}
