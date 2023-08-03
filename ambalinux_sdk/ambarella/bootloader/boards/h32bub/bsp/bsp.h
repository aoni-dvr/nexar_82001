/*
 * History:
 *	Author: Cao Rongrong <rrcao@ambarella.com>
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

#ifndef __BSP_H__
#define __BSP_H__

#define AMBOOT_BST_SIZE		(AMBOOT_BST_FIXED_SIZE)
#define AMBOOT_BLD_SIZE		(AMBOOT_MIN_PART_SIZE * 8)
#define AMBOOT_PTB_SIZE		(AMBOOT_MIN_PART_SIZE * 7)
#define AMBOOT_ATF_SIZE		(AMBOOT_MIN_PART_SIZE * 8)
#define AMBOOT_PBA_SIZE		(16 * 1024 * 1024)
#define AMBOOT_PRI_SIZE		(16 * 1024 * 1024)
#define AMBOOT_SEC_SIZE		(0  * 1024 * 1024)
#define AMBOOT_BAK_SIZE		(0  * 1024 * 1024)
#define AMBOOT_RMD_SIZE		(0  * 1024 * 1024)
#define AMBOOT_ROM_SIZE		(0  * 1024 * 1024)
#define AMBOOT_DSP_SIZE		(0  * 1024 * 1024)
#define AMBOOT_LNX_SIZE		(128 * 1024 * 1024)
#define AMBOOT_SWP_SIZE		(0  * 1024 * 1024)
#define AMBOOT_ADD_SIZE		(0  * 1024 * 1024)
#define AMBOOT_ADC_SIZE		(0  * 1024 * 1024)

#define DRAM_SIZE		0x20000000

#define DEAFULT_PHY_ADDR	(3)

#endif /* __BSP_H__ */
