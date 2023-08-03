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
#include <sdmmc.h>

/* ==========================================================================*/

int amboot_bsp_sd_slot_init(int slot, int volt)
{
	int gpio_pwr = (slot == 0) ? GPIO(109) : GPIO(110);
	int gpio_1v8 = (slot == 0) ? GPIO(156) : GPIO(157);

	if (volt == SDMMC_VOLTAGE_3V3) {
		if (slot == 1)
			return 0;

		gpio_config_sw_out(gpio_1v8);
		gpio_clr(gpio_1v8);

		/* power off, then power on */
		gpio_config_sw_out(gpio_pwr);
		gpio_clr(gpio_pwr);
		rct_timer_dly_ms(10);
		gpio_set(gpio_pwr);
		rct_timer_dly_ms(10);
	} else {
		if (slot == 1) {
			printf("Can not switch voltage to 1.8v\n");
			return -1;
		}

		gpio_config_sw_out(gpio_1v8);
		gpio_set(gpio_1v8);
	}

	return 0;
}

int amboot_bsp_sd_phy_init(int slot, int mode)
{
	return 0;
}

