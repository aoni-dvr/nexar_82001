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

#define AMBOOT_BST_SIZE		AMBOOT_BST_FIXED_SIZE
#define AMBOOT_BLD_SIZE 	(AMBOOT_MIN_PART_SIZE * 8)
#define AMBOOT_PTB_SIZE		(AMBOOT_MIN_PART_SIZE * 7)
#define AMBOOT_ATF_SIZE 	(AMBOOT_MIN_PART_SIZE * 8)
#define AMBOOT_PBA_SIZE 	0
#if defined(CONFIG_AMBARELLA_ROOTFS_CPIO)
#define AMBOOT_PRI_SIZE 	(80 * 1024 * 1024)
#else
#define AMBOOT_PRI_SIZE 	(32 * 1024 * 1024)
#endif
#define AMBOOT_SEC_SIZE		(0 * 1024 * 1024)
#define AMBOOT_BAK_SIZE		0
#define AMBOOT_RMD_SIZE		(32 * 1024 * 1024)
#define AMBOOT_ROM_SIZE		0
#define AMBOOT_DSP_SIZE		0
#if defined(CONFIG_AMBARELLA_ROOTFS_CPIO)
#define AMBOOT_LNX_SIZE		(0 * 1024 * 1024)
#else
#define AMBOOT_LNX_SIZE		(128 * 1024 * 1024)
#endif
#define AMBOOT_SWP_SIZE		0
#define AMBOOT_ADD_SIZE		0
#define AMBOOT_ADC_SIZE		0

#define DRAM_SIZE		0x80000000

#define	DEFAULT_GPIO0_AFSEL	0xFFF7F9FE
#define	DEFAULT_GPIO0_DIR	0x00080001
#define	DEFAULT_GPIO0_MASK	0x00080601
#define	DEFAULT_GPIO0_DATA	0x00000600
#define	DEFAULT_GPIO1_AFSEL	0xFFFFFEFF
#define	DEFAULT_GPIO1_DIR	0x00000000
#define	DEFAULT_GPIO1_MASK	0x00000100
#define	DEFAULT_GPIO1_DATA	0x00000100
#define	DEFAULT_GPIO2_AFSEL	0xFFECFFFF
#define	DEFAULT_GPIO2_DIR	0x00130000
#define	DEFAULT_GPIO2_MASK	0x00130000
#define	DEFAULT_GPIO2_DATA	0x00000000
#define	DEFAULT_GPIO3_AFSEL	0x000003FF
#define	DEFAULT_GPIO3_DIR	0x00000000
#define	DEFAULT_GPIO3_MASK	0xFFFFFC00
#define	DEFAULT_GPIO3_DATA	0x00000800

#define	DEFAULT_GPIO0_CTRL_ENA	0x00080601
#define	DEFAULT_GPIO0_CTRL_DIR	0x00000600
#define	DEFAULT_GPIO1_CTRL_ENA	0x00000100
#define	DEFAULT_GPIO1_CTRL_DIR	0x00000100
#define	DEFAULT_GPIO2_CTRL_ENA	0x00130000
#define	DEFAULT_GPIO2_CTRL_DIR	0x00000000
#define	DEFAULT_GPIO3_CTRL_ENA	0xFFFFFC00
#define	DEFAULT_GPIO3_CTRL_DIR	0x00000800

#define	DEFAULT_IOMUX_REG0_0	0x0FF601FE
#define	DEFAULT_IOMUX_REG0_1	0x0601F806
#define	DEFAULT_IOMUX_REG0_2	0xF0000000
#define	DEFAULT_IOMUX_REG1_0	0x000000C0
#define	DEFAULT_IOMUX_REG1_1	0xFFFFE000
#define	DEFAULT_IOMUX_REG1_2	0x00001EFF
#define	DEFAULT_IOMUX_REG2_0	0xFFEC0000
#define	DEFAULT_IOMUX_REG2_1	0x0000FFFF
#define	DEFAULT_IOMUX_REG2_2	0x00000000
#define	DEFAULT_IOMUX_REG3_0	0x000003E1
#define	DEFAULT_IOMUX_REG3_1	0x000001E0
#define	DEFAULT_IOMUX_REG3_2	0x0000001E

#define	DEFAULT_GPIO_DS0_REG_0	0xFFFFFFFF
#define	DEFAULT_GPIO_DS1_REG_0	0xFFFFFFFF
#define	DEFAULT_GPIO_DS0_REG_1	0xFFFFFFFF
#define	DEFAULT_GPIO_DS1_REG_1	0xFFFFFFFF
#define	DEFAULT_GPIO_DS0_REG_2	0xFFFFFFFF
#define	DEFAULT_GPIO_DS1_REG_2	0xFFFFFFFF
#define	DEFAULT_GPIO_DS0_REG_3	0xFFFFFFFF
#define	DEFAULT_GPIO_DS1_REG_3	0xFFFFFFFF


#define DEAFULT_PHY_ADDR	(3)

#endif /* __BSP_H__ */
