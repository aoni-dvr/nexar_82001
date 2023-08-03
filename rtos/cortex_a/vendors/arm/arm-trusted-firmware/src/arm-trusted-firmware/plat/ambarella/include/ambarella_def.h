/*
 * Copyright (c) 2013-2016, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AMBARELLA_DEF_H__
#define __AMBARELLA_DEF_H__


/*******************************************************************************
 * Ambarella SoC Bus Base
 ******************************************************************************/
#define DRAM_SPACE_START		0x0000000000
#if defined(AMBARELLA_CV5)
#define DRAM_SPACE_MAX_SIZE		0x1000000000	/* 64GB */
#else
#define DRAM_SPACE_MAX_SIZE		0x00DFC00000
#endif

#if defined(AMBARELLA_CV5)
#define DEVICE_BASE			0x1000000000
#define DEVICE_SIZE			0x2000000000	/* SIZE: 128GB */
#else
#define DEVICE_BASE			0xE0000000
#define DEVICE_SIZE			0x1FF00000
#endif

#if defined(AMBARELLA_CV5)
#define SRAM_BASE			0x20E0030000
#define SRAM_SIZE			0x00004000
#define DRAM_TRAINING_RO_START      0x20ee000000ULL
#define DRAM_TRAINING_RO_SIZE       0x3000ULL
#define DRAM_TRAINING_RW_START      0x20ee003000ULL
#define DRAM_TRAINING_RW_SIZE       0x1000ULL
#define DRAM_TRAINING_STACK_START   0x20e0030000ULL
#define DRAM_TRAINING_STACK_SIZE    (16 * 1024)
#else
#define SRAM_BASE			0xE8020000
#if defined(CONFIG_ATF_AMBALINK) && defined(CONFIG_AUTO_DRAM_TRAINING)
#define SRAM_SIZE			0x00002000
#else
#define SRAM_SIZE			0x00001000
#endif
#endif

#if defined(AMBARELLA_CV5)
#define DDRC_BASE			0x1000000000
#define DDRC_VERSION			2
#else
#define DDRC_BASE			0xDFFE0000
#define DDRC_VERSION			1
#endif
#define DDRC_SIZE			0x00020000	/* NOT VERY SURE */
#define DDRC_REG(x)			(DDRC_BASE + (x))

#if defined(AMBARELLA_CV5)
#define RCT_BASE			0x20ED080000
#else
#define RCT_BASE			0xED080000
#endif
#define RCT_SIZE			0x00010000
#define RCT_REG(x)			(RCT_BASE + (x))

#if defined(AMBARELLA_CV5)
#define AXI_BASE			0x20F2000000
#else
#define AXI_BASE			0xF2000000
#endif
#define AXI_SIZE			0x00001000
#define AXI_REG(x)			(AXI_BASE + (x))

/*******************************************************************************
 * Ambarella SoC Module Reg Base
 ******************************************************************************/

#if defined(AMBARELLA_CV5)
#define GICD_BASE			0x20F3001000
#define GICC_BASE			0x20F3002000
#define GICH_BASE			0x20F3004000
#define GICV_BASE			0x20F3006000
#else
#define GICD_BASE			0xF3001000
#define GICC_BASE			0xF3002000
#define GICH_BASE			0xF3004000
#define GICV_BASE			0xF3006000
#endif

#if defined(AMBARELLA_CV5)
#define FIO_BASE			0x20E0002000
#else
#define FIO_BASE			0xE0002000
#endif

#if defined(AMBARELLA_CV5)
#define FIO_4K_BASE			0x20E000C000
#elif defined(AMBARELLA_CV28)
#define FIO_4K_BASE			0xE000C000
#else
#define FIO_4K_BASE			0xE0003000
#endif
#define FIO_REG(x)			(FIO_BASE + (x))

#if defined(AMBARELLA_CV5)
#define EMMC0_BASE			0x20E0003000
#else
#define EMMC0_BASE			0xE0004000
#endif
#define EMMC0_REG(x)			(EMMC0_BASE + (x))

#if defined(AMBARELLA_CV5)
#define SPINOR_BASE			0x20E0001000
#else
#define SPINOR_BASE			0xE0001000
#endif
#define SPINOR_REG(x)			(SPINOR_BASE + (x))

#if defined(AMBARELLA_CV5)
#define DMA0_BASE			0x20E0020000
#elif defined(AMBARELLA_CV2)
#define DMA0_BASE			0xE000A000
#else
#define DMA0_BASE			0xE0020000
#endif
#define DMA0_REG(x)			(DMA0_BASE + (x))

#if defined(AMBARELLA_CV5)
#define GPIO0_BASE			0x20E4013000
#define GPIO1_BASE			0x20E4014000
#define GPIO2_BASE			0x20E4015000
#define GPIO3_BASE			0x20E4016000
#define GPIO4_BASE			0x20E4017000
#define GPIO5_BASE			0x20E4018000 /* fake on cv5 */
#define GPIO6_BASE			0x20E4019000 /* fake on cv5 */
#elif defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
#define GPIO0_BASE			0xE4010000
#define GPIO1_BASE			0xE4011000
#define GPIO2_BASE			0xE4012000
#define GPIO3_BASE			0xE4013000
#define GPIO4_BASE			0xE4014000
#define GPIO5_BASE			0xE4015000
#define GPIO6_BASE			0xE4016000
#else
#define GPIO0_BASE			0xEC003000
#define GPIO1_BASE			0xEC004000
#define GPIO2_BASE			0xEC005000
#define GPIO3_BASE			0xEC006000
#define GPIO4_BASE			0xEC007000
#define GPIO5_BASE			0xEC008000
#define GPIO6_BASE			0xEC009000
#endif

#define GPIO_BANK			7
#define NRGPIO_PER_BANK			32

#if defined(AMBARELLA_CV5)
#define IOMUX_BASE			0x20E4010000
#elif defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
#define IOMUX_BASE			0xE400E000
#else
#define IOMUX_BASE			0xEC000000
#endif

#if defined(AMBARELLA_CV5)
#define UART0_BASE			0x20E4000000
#else
#define UART0_BASE			0xE4000000
#endif

#if defined(AMBARELLA_CV5)
#define PWC_BASE			0x20E002F000
#elif defined(AMBARELLA_CV2)
#define PWC_BASE			0xEC001000
#else
#define PWC_BASE			0xE8001000
#endif
#define PWC_REG(x)			(PWC_BASE + (x))

#if defined(AMBARELLA_CV5)
#define SCRATCHPAD_BASE			0x20E0024000
#elif defined(AMBARELLA_CV2)
#define SCRATCHPAD_BASE			0xE8001000
#else
#define SCRATCHPAD_BASE			0xE0022000
#endif
#define SCRATCHPAD_REG(x)		(SCRATCHPAD_BASE + (x))

#if defined(AMBARELLA_CV5)
#define SECURE_SCRATCHPAD_BASE		0x20E002F000
#elif defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS)
#define SECURE_SCRATCHPAD_BASE		0xE002F000
#else
#define SECURE_SCRATCHPAD_BASE		0xE8001000
#endif
#define SECURE_SCRATCHPAD_REG(x)	(SECURE_SCRATCHPAD_BASE + (x))

/*******************************************************************************
 * Ambarella platform constants
 ******************************************************************************/

/*
 * Security Definition
 */
#if defined(AMBARELLA_CV2) || defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS)
#define AMBARELLA_SUPPORT_SEURITY_CTRL	0
#else
#define AMBARELLA_SUPPORT_SEURITY_CTRL	1
#endif

#if defined(AMBARELLA_CV5)
#define AXI_SEC_CTRL_BASE		0x20F2000000
#else
#define AXI_SEC_CTRL_BASE		0xF2000000
#endif

#if defined(AMBARELLA_CV5)
#define DRAM_ACCESS_VIRTUAL_OFFSET	0x400
#define DRAM_SECMEM_CTRL_OFFSET		0x304
#define DRAM_SECMEM_BASE_OFFSET		0x31C
#define DRAM_SECMEM_LIMIT_OFFSET	0x320
#define DRAM_VPN_BASE_OFFSET		0x40C
#define DRAM_VPN_BOUND_OFFSET		0x48C
#define DRAM_ATT_OFFSET			0x10000
#else
#define DRAM_ACCESS_VIRTUAL_OFFSET	0x214
#define DRAM_SECMEM_CTRL_OFFSET		0x218
#define DRAM_SECMEM_BASE_OFFSET		0x248
#define DRAM_SECMEM_LIMIT_OFFSET	0x24c
#define DRAM_VPN_BASE_OFFSET		0x300
#define DRAM_VPN_BOUND_OFFSET		0x380
#define DRAM_ATT_OFFSET			0x8000
#endif

/*
 * DDRC Register Definition
 */

#if defined(AMBARELLA_CV5)
#define DDRC_C0_OFFSET			0x5000
#define DDRC_C1_OFFSET			0x6000
#else
#define DDRC_C0_OFFSET			0x0800
#define DDRC_C1_OFFSET			0x0A00
#endif

/*
 * AXI Register Definition
 */
#define AXI_SYS_TIMER_OFFSET		0x14
#define CORTEX_RESET_OFFSET		0x28
#define CORTEX_RVBARADDR0_OFFSET	0x64
#define CORTEX_RVBARADDR1_OFFSET	0x68
#define CORTEX_RVBARADDR2_OFFSET	0x6c
#define CORTEX_RVBARADDR3_OFFSET	0x70
#if defined(CONFIG_ATF_AMBALINK)
#define AXI_BYPASS_BOOT_CHANGE		0x80
#endif
#define AXI_SEC0_CTRL_OFFSET		0x90
#define AXI_SEC1_CTRL_OFFSET		0x94
#define AXI_SEC2_CTRL_OFFSET		0x98

#if defined(AMBARELLA_CV5)
#define CORTEX_RVBAR_ADDR(x)		((x) >> 8)
#else
#define CORTEX_RVBAR_ADDR(x)		((x) >> 0)
#endif

#define CORTEX_CORE0_RESET_MASK		(1 << 16)
#define CORTEX_CORE1_RESET_MASK		(1 << 17)
#define CORTEX_CORE2_RESET_MASK		(1 << 18)
#define CORTEX_CORE3_RESET_MASK		(1 << 19)

#if defined(AMBARELLA_CV5)
#define NIC_GPV_BASE			0x20F1000000
#else
#define NIC_GPV_BASE			0x00F1000000
#endif
#define NIC_GPV_REG(x)			(NIC_GPV_BASE + (x))

#if defined(AMBARELLA_CV5)
#define NIC_GPV_MASTER_PORT		9
#else
#define NIC_GPV_MASTER_PORT		4
#endif

#if defined(AMBARELLA_CV2) || defined(AMBARELLA_CV22) || defined(AMBARELLA_CV25)
#define AXI_SYS_TIMER_DIVISOR		16
#define AXI_SYS_TIMER_INDEPENDENT	0
#else
#define AXI_SYS_TIMER_DIVISOR		12
#define AXI_SYS_TIMER_INDEPENDENT	1
#endif

#ifdef CONFIG_ATF_AMBALINK
#define NS_SWITCH_AARCH32               (0)
#define NS_SWITCH_AARCH64               (1)
#define  S_SWITCH_AARCH32               (2)
#define  S_SWITCH_AARCH64               (3)
#endif
/*
 * RCT Register Definition
 */
#define PLL_CORE_CTRL_OFFSET		0x00
#define PLL_CORE_CTRL2_OFFSET		0x100
#define SYS_CONFIG_OFFSET		0x34
#define CG_UART_OFFSET			0x38
#define ANA_PWR_OFFSET			0x50
#define SOFT_OR_DLL_RESET_OFFSET	0x68
#define RCT_TIMER_OFFSET		0x254
#define RCT_TIMER_CTRL_OFFSET		0x258
#define PLL_CORTEX_CTRL_OFFSET		0x264
#define PLL_CORTEX_CTRL2_OFFSET 	0x26C
#define PLL_ENET_CTRL_OFFSET		0x520
#define PLL_ENET_CTRL2_OFFSET		0x528
#define CLUSTER_SOFT_RESET_OFFSET	0x228
#if defined(AMBARELLA_CV5)
#define PLL_DDR_CTRL_OFFSET 		0x000
#define PLL_DDR_CTRL2_OFFSET		0x008
#define DDRH0_REG_BASE  		0x20ED180000
#define DDRH0_REG(x) 			(DDRH0_REG_BASE + (x))
#else
#define PLL_DDR_CTRL_OFFSET		0xDC
#define PLL_DDR_CTRL2_OFFSET		0x110
#endif

#define POC_PERIPHERAL_CLK_MODE		0x00000200

/* Boot Media */
#define POC_BOOT_FROM_SPINOR		0x00000000
#define POC_BOOT_FROM_NAND		0x00000010
#define POC_BOOT_FROM_EMMC		0x00000020
#define POC_BOOT_FROM_MASK		0x00000030

#if defined(AMBARELLA_CV5)
#define POC_BOOT_FROM_USB		0x00000400
#define POC_BOOT_FROM_PCIE		0x00000030
#endif

#define SYS_CONFIG_SECURE_BOOT		0x00000040

#if defined(AMBARELLA_CV2) || defined(AMBARELLA_CV22) || defined(AMBARELLA_CV25)
#define SYS_CONFIG_NAND_SPINAND		0x00400000
#define SYS_CONFIG_NAND_SCKMODE		0x00080000
#define SYS_CONFIG_NAND_4K_FIFO		0xffffffff /* not used */
#define SYS_CONFIG_NAND_8K_FIFO		0x00000000 /* not used */
#define SYS_CONFIG_NAND_PAGE_SIZE	0x00040000
#define SYS_CONFIG_NAND_READ_CONFIRM	0xffffffff /* not used */
#define SYS_CONFIG_NAND_ECC_BCH_EN	0x00010000
#define SYS_CONFIG_NAND_ECC_SPARE_2X	0x00008000
#elif defined(AMBARELLA_S6LM)
#define SYS_CONFIG_NAND_SPINAND		0xffffffff /* not used, spinand only */
#define SYS_CONFIG_NAND_SCKMODE		0x00080000
#define SYS_CONFIG_NAND_4K_FIFO		0xffffffff /* not used */
#define SYS_CONFIG_NAND_8K_FIFO		0x00000000 /* not used */
#define SYS_CONFIG_NAND_PAGE_SIZE	0x00040000
#define SYS_CONFIG_NAND_READ_CONFIRM	0xffffffff /* not used */
#define SYS_CONFIG_NAND_ECC_BCH_EN	0x00010000
#define SYS_CONFIG_NAND_ECC_SPARE_2X	0x00008000
#else
#define SYS_CONFIG_NAND_SPINAND		0xffffffff /* not used, spinand only */
#define SYS_CONFIG_NAND_SCKMODE		0x00040000
#define SYS_CONFIG_NAND_4K_FIFO		0xffffffff /* not used */
#define SYS_CONFIG_NAND_8K_FIFO		0x00100000
#define SYS_CONFIG_NAND_PAGE_SIZE	0x00020000
#define SYS_CONFIG_NAND_READ_CONFIRM	0xffffffff /* not used */
#define SYS_CONFIG_NAND_ECC_BCH_EN	0x00008000
#define SYS_CONFIG_NAND_ECC_SPARE_2X	0x00004000
#endif

#if defined(AMBARELLA_CV2) || defined(AMBARELLA_CV22) || defined(AMBARELLA_CV25) || defined(AMBARELLA_S6LM)
#define SYS_CONFIG_MMC_HS		0x00004000
#define SYS_CONFIG_MMC_DDR		0x00000000 /* not supported */
#define SYS_CONFIG_MMC_SDXC		0x00000000 /* not supported */
#define SYS_CONFIG_MMC_4BIT		0x00010000
#define SYS_CONFIG_MMC_8BIT		0x00008000
#else
#define SYS_CONFIG_MMC_HS		0x00002000
#define SYS_CONFIG_MMC_DDR		0x00000000 /* not supported */
#define SYS_CONFIG_MMC_SDXC		0x00000000 /* not supported */
#define SYS_CONFIG_MMC_4BIT		0x00008000
#define SYS_CONFIG_MMC_8BIT		0x00004000
#endif

#if defined(AMBARELLA_CV2)
#define CLUSTER_SOFT_RESET_VP		0x00000780
#elif defined(AMBARELLA_CV5)
#define CLUSTER_SOFT_RESET_VP		0x00000380
#else
#define CLUSTER_SOFT_RESET_VP		0x00000700
#endif

/* ==========================================================================*/

/*
 * ScratchPad Register Definition
 */

#if defined(AMBARELLA_CV2)
#define AHBSP_DMA0_SEL0_OFFSET		0x30
#else
#define AHBSP_DMA0_SEL0_OFFSET		0x50
#endif
#define AHBSP_DMA0_SEL0_REG		SCRATCHPAD_REG(AHBSP_DMA0_SEL0_OFFSET)

#define AHBSP_PWC_STROBE_OFFSET		0x90
#define SECSP_BOOT_STS_OFFSET		0x94
#define SECSP_BOOT_STS_REG		SECURE_SCRATCHPAD_REG(SECSP_BOOT_STS_OFFSET)

#if defined(AMBARELLA_CV2)
#define AHBSP_DATA0_OFFSET		0x18
#define AHBSP_DATA1_OFFSET		0x1C
#define AHBSP_DATA2_OFFSET		0x20
#define AHBSP_DATA3_OFFSET		0x24
#define AHBSP_JTAG_EN_OFFSET		0x68
#else
#define AHBSP_DATA0_OFFSET		0x6C
#define AHBSP_DATA1_OFFSET		0x70
#define AHBSP_DATA2_OFFSET		0x74
#define AHBSP_DATA3_OFFSET		0x78
#define AHBSP_JTAG_EN_OFFSET		0x18
#endif
#define AHBSP_JTAG_EN_REG		SECURE_SCRATCHPAD_REG(AHBSP_JTAG_EN_OFFSET)


/*
 * UART PINs and Register Definition
 */

#define UART_CLOCK			24000000
#define UART_BAUDRATE			115200

#if defined(AMBARELLA_CV22) || defined(AMBARELLA_CV2)
#define AMBARELLA_UART_PIN_RX		39
#define AMBARELLA_UART_PIN_TX		40
#elif defined(AMBARELLA_CV25)
#define AMBARELLA_UART_PIN_RX		17
#define AMBARELLA_UART_PIN_TX		18
#elif defined(AMBARELLA_S6LM) || defined(AMBARELLA_CV28) || defined(AMBARELLA_CV5)
#define AMBARELLA_UART_PIN_RX		10
#define AMBARELLA_UART_PIN_TX		11
#elif defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
#define AMBARELLA_UART_PIN_RX		16
#define AMBARELLA_UART_PIN_TX		17
#else
#error "Unknown chip for UART pins"
#endif

#define UART_RB_OFFSET			0x00
#define UART_TH_OFFSET			0x00
#define UART_DLL_OFFSET			0x00
#define UART_IE_OFFSET			0x04
#define UART_DLH_OFFSET			0x04
#define UART_II_OFFSET			0x08
#define UART_FC_OFFSET			0x08
#define UART_LC_OFFSET			0x0c
#define UART_MC_OFFSET			0x10
#define UART_LS_OFFSET			0x14
#define UART_MS_OFFSET			0x18
#define UART_SC_OFFSET			0x1c	/* Byte */
#define UART_DMAE_OFFSET		0x28
#define UART_DMAF_OFFSET		0x40	/* DMA fifo */
#define UART_US_OFFSET			0x7c
#define UART_TFL_OFFSET			0x80
#define UART_RFL_OFFSET			0x84
#define UART_SRR_OFFSET			0x88


/*
 * GPIO Register Definition
 */
#define GPIO_DATA_OFFSET		0x00
#define GPIO_DIR_OFFSET			0x04
#define GPIO_IS_OFFSET			0x08
#define GPIO_IBE_OFFSET			0x0c
#define GPIO_IEV_OFFSET			0x10
#define GPIO_IE_OFFSET			0x14
#define GPIO_AFSEL_OFFSET		0x18
#define GPIO_RIS_OFFSET			0x1c
#define GPIO_MIS_OFFSET			0x20
#define GPIO_IC_OFFSET			0x24
#define GPIO_MASK_OFFSET		0x28
#define GPIO_ENABLE_OFFSET		0x2c

#define IOMUX_CTRL_SET_OFFSET		0xf0
#define IOMUX_REG_OFFSET(b, n)		(((b) * 0xc) + ((n) * 4))


/*
 * PWC Register Definition
 */
#define PWC_SET_RTC_OFFSET		0x30
#define PWC_CUR_RTC_OFFSET		0x34
#define PWC_RESET_OFFSET		0x40
#if defined(AMBARELLA_CV2)
#define PWC_CUR_STA_OFFSET		0xB4
#define	PWC_SET_STA_OFFSET		0xC0
#else
#define PWC_CUR_STA_OFFSET		0x54
#define	PWC_SET_STA_OFFSET		0x58
#endif

/*
 *  OTP EXTERNAL POWER supply
 */
#if defined(AMBARELLA_S6LM)
#define GPIO_OTP_PWR_SW			35
#elif defined(AMBARELLA_CV28)
#define GPIO_OTP_PWR_SW			85
#define SYSCNFIG_LOCK_BIT
#elif defined(AMBARELLA_CV5)
#define SYSCNFIG_LOCK_BIT
#endif

/*
 *  OTP Zone A Lock check
 */
#if defined(AMBARELLA_CV2) || defined(AMBARELLA_CV22) || defined(AMBARELLA_CV25) || defined(AMBARELLA_CV28) || defined(AMBARELLA_S6LM)
#define OTP_LAYOUT_V1
#elif defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
#define OTP_LAYOUT_V1
#elif defined (AMBARELLA_CV5)
#define OTP_LAYOUT_V2
#endif


/*
 * OTP Layout
 */

/*
 *  OTP write lock
 */
#if defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
#define WRITE_LOCK_BIT_ADDR		(0xC0 * 2)	/* bit addr */
#else
#define WRITE_LOCK_BIT_ADDR		(0xC0)		/* bit addr */
#endif

/*
 *  OTP efuse bit and write lock bit
 */

#define SECURE_BOOT_BIT				6
#define JTAG_EFUSE_BIT				63

#ifdef OTP_LAYOUT_V1
#define LOCK_BIT_A				0
#define LOCK_BIT_CUSTOMER_ID			2
#define LOCK_BIT_TEST_REGION			3
#define LOCK_BIT_ROT_BASE			5
#define LOCK_BIT_HUK_NONCE			8
#define LOCK_BIT_AES_KEY_BASE			10
#define LOCK_BIT_ECC_KEY_BASE			14
#define LOCK_BIT_USR_SLOT_G0_BASE		18

#if defined(AMBARELLA_CV28)
#define LOCK_BIT_SYS_CONFIG			24
#endif

#elif defined (OTP_LAYOUT_V2)
#define EN_ANTI_ROLLBACK_BIT			0x3E90
#define DIS_SECURE_USB_BOOT_BIT			0x3E91

#define LOCK_BIT_A				0
#define LOCK_BIT_UNIQUE_ID			1
#define LOCK_BIT_SYS_CONFIG			3
#define LOCK_BIT_CST_PLANTED_SEED_CUK		4
#define LOCK_BIT_CUSTOMER_ID			5
#define LOCK_BIT_TEST_REGION			6
#define LOCK_BIT_ROT_BASE			8
#define LOCK_BIT_HUK_NONCE			11
#define LOCK_BIT_USR_PLANTED_CUK		12
#define LOCK_BIT_AES_KEY_BASE			13
#define LOCK_BIT_ECC_KEY_BASE			17
#define LOCK_BIT_USR_SLOT_G0_BASE		21
#define LOCK_BIT_USR_SLOT_G1_BASE		27
#endif

/*
 *  OTP sysconfig
 */
#define SYS_CONFIG_BITS			64

/*
 *  OTP ROT public key
 */
#if defined (OTP_LAYOUT_V1)
#define ROT_KEY_NUM			3
#define ROT_PUBKEY_BITS		4096
#define NON_REVOKABLE_KEY_INDEX		2
#elif defined (OTP_LAYOUT_V2)
#define ROT_KEY_NUM			16
#define ROT_PUBKEY_BITS		256
#define NON_REVOKABLE_KEY_INDEX		0
#endif

/*
 *  OTP HUK
 */
#define HUK_BITS		128
#define HW_NONCE_BITS		128

/*
 *  OTP customer defined AES and ECC keys
 */
#define AES_KEY_NUM		4
#define ECC_KEY_NUM		4
#define AES_KEY_BITS		256
#define ECC_KEY_BITS		256

/*
 *  OTP Unique ID
 */
#define UNIQUE_ID_BITS		128

/*
 *  OTP Customer ID (Customer serial number)
 */
#define CUSTOMER_ID_BITS	128

/*
 *  OTP Monotonic counter
 */
#define MONO_CNT_0_BITS		256

#if defined (OTP_LAYOUT_V1)
#define NUM_OF_MONO_CNT		1
#elif defined (OTP_LAYOUT_V2)
#define NUM_OF_MONO_CNT		3
#define MONO_CNT_1_BITS		256
#define MONO_CNT_2_BITS		512
#endif

/*
 *  OTP User Slot Group 0 (lockable)
 */
#define USR_SLOT_G0_NUM		6
#define USR_SLOT_G0_BITS	256

/*
 *  OTP User Slot Group 1 (lockable)
 */
#if defined (OTP_LAYOUT_V2)
#define USR_SLOT_G1_NUM		4
#define USR_SLOT_G1_BITS	1024
#endif

/*
 *  OTP User Data Group 0 (not lockable)
 */
#if defined (OTP_LAYOUT_V2)
#define USR_DATA_G0_NUM		3
#define USR_DATA_G0_BITS	1024
#endif

/*
 *  OTP Test Region
 */
#define TEST_REGION_BITS		128


/*
 *  OTP Misc Config
 */

#if defined (OTP_LAYOUT_V1)
#define MISC_CONFIG_ADDR		(0x3E83)
#define MISC_CONFIG_BITS		29
#elif defined (OTP_LAYOUT_V2)
#define MISC_CONFIG_ADDR		(0xBE92)
#define MISC_CONFIG_BITS		14
#endif

/*
 *  other OTP fields
 */
#ifdef OTP_LAYOUT_V1

#define SYS_CONFIG_BIT_ADDR		(0x00)		/* bit addr of sysconfig */
#if defined(CONFIG_ATF_AMBALINK) && (defined(AMBARELLA_CV2FS) || defined(AMBARELLA_CV22FS))
#define UNIQUE_ID_ADDR_ORICV25		(0x19c0 * 2)    /* bit addr */
#define UNIQUE_ID_ADDR			(0x3E00 * 2)	/* bit addr */
#define DATA_INVALID_BIT_ADDR		(0x3E80 * 2)	/* bit addr of revoke bits */
#define CUSTOMER_ID_ADDR		(0x3F00 * 2)	/* bit addr of customer id*/
#define TEST_REGION_ADDR		(0x3F80 * 2)	/* bit addr of test region */
#define HUK_ADDR			(0x4000 * 2)	/* bit addr */
#define HW_NONCE_ADDR			(0x4080 * 2)	/* bit addr */
#define MONO_CNT_0_ADDR			(0x4100 * 2)	/* bit addr */
#define AES_KEY_BASE_ADDR		(0x4200 * 2)	/* bit addr */
#define ECC_KEY_BASE_ADDR		(0x4600 * 2)	/* bit addr */
#define USR_SLOT_G0_ADDR		(0x4A00 * 2)	/* bit addr of usr slot group 0*/
#define ROT_PUBKEY_ADDR			(0x5000 * 2)	/* bit addr of ROT pubkey base */
#else
#define UNIQUE_ID_ADDR_ORICV25		(0x19C0)	/* bit addr */
#define UNIQUE_ID_ADDR			(0x3E00)	/* bit addr */

#define DATA_INVALID_BIT_ADDR		(0x3E80)	/* bit addr of revoke bits */
#define CUSTOMER_ID_ADDR		(0x3F00)	/* bit addr of customer id*/
#define TEST_REGION_ADDR		(0x3F80)	/* bit addr of test region */
#define HUK_ADDR			(0x4000)	/* bit addr */
#define HW_NONCE_ADDR			(0x4080)	/* bit addr */
#define MONO_CNT_0_ADDR			(0x4100)	/* bit addr */
#define AES_KEY_BASE_ADDR		(0x4200)	/* bit addr */
#define ECC_KEY_BASE_ADDR		(0x4600)	/* bit addr */
#define USR_SLOT_G0_ADDR		(0x4A00)	/* bit addr of usr slot group 0*/
#define ROT_PUBKEY_ADDR			(0x5000)	/* bit addr of ROT pubkey base */
#endif

#elif defined (OTP_LAYOUT_V2)

#define SYS_CONFIG_BIT_ADDR		(0x40)		/* bit addr of sysconfig */

#define CST_PLANTED_SEED_ADDR		(0xE0)	/* bit addr */

#define UNIQUE_ID_ADDR			(0x100)	/* bit addr */

#define CST_PLANTED_CUK_ADDR		(0x180)	/* bit addr */

#define DATA_INVALID_BIT_ADDR		(0xBE80)	/* bit addr of revoke bits */
#define BST_VER_ADDR			(0xBEA0)	/* bit addr */
#define CUSTOMER_ID_ADDR		(0xBF00)	/* bit addr */
#define TEST_REGION_ADDR		(0xBF80)	/* bit addr of test region */
#define HUK_ADDR			(0xC000)	/* bit addr */
#define HW_NONCE_ADDR			(0xC080)	/* bit addr */
#define USR_PLANTED_CUK_ADDR		(0xC100)	/* bit addr */
#define AES_KEY_BASE_ADDR		(0xC200)	/* bit addr */
#define ECC_KEY_BASE_ADDR		(0xC600)	/* bit addr */
#define USR_SLOT_G0_ADDR		(0xCA00)	/* bit addr of usr slot group 0*/
#define ROT_PUBKEY_ADDR			(0xD000)	/* bit addr of ROT pubkey base */
#define MONO_CNT_0_ADDR			(0xE000)	/* bit addr */
#define MONO_CNT_1_ADDR			(0xE100)	/* bit addr */
#define MONO_CNT_2_ADDR			(0xE200)	/* bit addr */
#define USR_DATA_G0_ADDR		(0xE400)	/* bit addr of usr data group 0*/
#define USR_SLOT_G1_ADDR		(0xF000)	/* bit addr of usr slot group 1*/

#define CST_PLANTED_SEED_BITS		32
#define CST_PLANTED_CUK_BITS		128
#define BST_VER_BITS			96
#define USR_PLANTED_CUK_BITS		256

#endif

/* Definition for Recovery, please see amboot/src/bst/inlcude/bst_devfw.h */
#define DEVFW_MAGIC			(0x33219fbd)
#define DEVFW_FLAG_OFFSET		(0x44)


/************************************************************************/
#if defined(AMBARELLA_CV5)
#define PWC_DBG_BASE			0x20ED0D0000
#else
#define PWC_DBG_BASE			0xED0D0000
#endif
#define PWC_DBG_REG(x)			(PWC_DBG_BASE + (x))

#define PWC_KEY_IN0_REG_OFFSET		0x000
#define PWC_KEY_IN1_REG_OFFSET		0x004
#define PWC_KEY_IN2_REG_OFFSET		0x008
#define PWC_KEY_IN3_REG_OFFSET		0x00c
#define PWC_KEY_IN4_REG_OFFSET		0x010
#define PWC_KEY_IN5_REG_OFFSET		0x014
#define PWC_KEY_IN6_REG_OFFSET		0x018
#define PWC_KEY_IN7_REG_OFFSET		0x01c
#define PWC_KEY_OUT0_REG_OFFSET		0x020
#define PWC_KEY_OUT1_REG_OFFSET		0x024
#define PWC_KEY_OUT2_REG_OFFSET		0x028
#define PWC_KEY_OUT3_REG_OFFSET		0x02c
#define PWC_KEY_OUT4_REG_OFFSET		0x030
#define PWC_KEY_OUT5_REG_OFFSET		0x034
#define PWC_KEY_OUT6_REG_OFFSET		0x038
#define PWC_KEY_OUT7_REG_OFFSET		0x03c

#endif
