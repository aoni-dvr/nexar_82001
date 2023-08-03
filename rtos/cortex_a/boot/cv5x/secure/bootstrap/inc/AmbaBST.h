/**
 *  @file AmbaBST.h
 *
 *  Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Constants & Definitions for Bootstrap
 *
 */

#ifndef AMBA_BST_H
#define AMBA_BST_H

#include "AmbaCortexA76.h"
#include "AmbaDebugPort.h"
#include "AmbaMemMap.h"

/**
 * RCT: DDR PLL/DLL Control Registers
 */
#define PLL_CORE_PLL_CTRL_REG           0x000
#define PLL_CORE_PLL_CTRL2_REG          0x100
#define PLL_CORE_PLL_CTRL3_REG          0x104
#define PLL_CORTEX_PLL_CTRL_REG         0x264
#define PLL_CORTEX_PLL_CTRL2_REG        0x26c
#define PLL_CORTEX_PLL_CTRL3_REG        0x270
#define PLL_DSU_PLL_CTRL_REG            0x814
#define PLL_DSU_PLL_CTRL2_REG           0x81c
#define PLL_DSU_PLL_CTRL3_REG           0x820
#define PLL_DDR_PLL_CTRL_REG            0x0
#define PLL_DDR_PLL_CTRL2_REG           0x8
#define PLL_DDR_PLL_CTRL3_REG           0xc

#define PLL_DDR_CG_INIT_REG             0x14

#define SCP_RNG_CTRL_REG                0x0
#define SCP_RNG_DATA0                   0x4
#define SCP_RNG_DATA1                   0x8
#define SCP_RNG_DATA2                   0xc
#define SCP_RNG_DATA3                   0x10
#define SCP_RNG_DATA4                   0xB0
#define SCP_SEC_BOOT_STS_OFFSET         0x94

#define RCT_POC_REG                     0x34
#define RCT_CLOCK_CTRL_REG              0x8C
#define RCT_RNG_CTRL_REG                0x1A4
#define RCT_TIMER_COUNT_REG             0x254
#define RCT_TIMER_CTRL_REG              0x258
#define RCT_TIMER_2_COUNT_REG           0x494
#define RCT_TIMER_2_CTRL_REG            0x498

#define FIO_RESET_REG                   0x074
#define WDT_RESET_REG                   0x078
#define WDT_RESET_CLEAR_ENABLE_REG      0x260

#define SYS_RESET_REG                   0x068
#define SYS_CONFIG_REG                  0x034
#define SYS_CONFIG_RAW_REG              0x2FC

#define PLL_SD48_POST_SCALER_REG        0x000c
#define PLL_SD_CTRL_REG                 0x04ac

#define SYS_CONFIG_NAND_FLASH_PAGE_4K   0x00020000  /* 1: 2K Byte, 0: 4K Byte */
#define SYS_CONFIG_BOOT_MEDIA           0x00000001  /* 0: NAND, 1: NOR */
#define SYS_CONFIG_SPI_BOOT             0x00001000
#define SYS_CONFIG_NAND_BCH             0x00010000  /* 0: Disable, 1: Enable */
#define SYS_CONFIG_NAND_SPARE_2X        0x00004000  /* 0: NAND Spare cell 1X, 1: 2X, POC[14] */
#define SYS_CONFIG_SPINOR_BOOT_OCT      0x00004000  /* 0: NAND Spare cell 1X, 1: 2X, POC[14] */
#define SYS_CONFIG_SD8_BOOT             0x00008000  /* SD8 boot */
#define SYS_CONFIG_SD4_BOOT             0x00010000  /* SD4 boot */

#define SYS_CLOCK_CTRL_INIT_VALUE       0x00050000

/**
 * DRAM Controller Registers
 */
#define DRAMC_MODE_REG                  0x00
#define DRAMC_MODE_HOSR_CONFIG_MASK     0x06
#if defined(CONFIG_SOC_CV5)
#define DRAMC_HOST_MAX_NUM              0x02
#elif defined(CONFIG_SOC_CV52)
#define DRAMC_HOST_MAX_NUM              0x01
#endif

/**
 * DDR Controller Registers
 */
#define DDRC_CTRL_REG                   0x00
#define DDRC_CONFIG1_REG                0x04
#define DDRC_CONFIG2_REG                0x08
#define DDRC_TIMING1_REG                0x0c
#define DDRC_TIMING2_REG                0x10
#define DDRC_TIMING3_REG                0x14
#define DDRC_TIMING4_REG                0x18
#define DDRC_TIMING5_REG                0x1c
#define DDRC_DUAL_DIE_TIMING_REG        0x20
#define DDRC_REFRESH_TIMING_REG         0x24
#define DDRC_LP5_TIMING_REG             0x28
#define DDRC_INIT_CTRL_REG              0x2C
#define DDRC_MODE_REG                   0x30
#define DDRC_RSVD_SPACE_REG             0x38
#define DDRC_BYTE_MAP_REG               0x3C
#define DRAM_MPC_WDATA                  0x40
#define DRAM_MPC_WMASK                  0x60
#define DRAM_MPC_RDATA                  0x64
#define DRAM_MPC_RMASK                  0xA4
#define DDRC_UINST1_REG                 0XAC
#define DDRC_UINST2_REG                 0XB0
#define DDRC_UINST3_REG                 0XB4
#define DDRC_UINST4_REG                 0XB8
#define DDRC_UINST5_REG                 0XBC
#define DDRC_UINST6_REG                 0XC0
#define DDRC_SCRATCHPAD_REG             0xC4
#define DDRC_WDQS_REG                   0xC8
#define DRAM_CLEAR_MPC_DATA             0xcc
#define DDRC_IO_CTRL_REG                0xD0
#define DRAM_DTTE_CONFIG                0x100
#define DRAM_DTTE_DELAY_MAP             0x134
#define DDRC_WRITE_VREF_0_REG           0x164
#define DDRC_WRITE_VREF_1_REG           0x168
#define DDRC_DTTE_TIMING_REG            0x16C
#define DDRC_DLL_SBC_0_REG              0x200
#define DDRC_DLL_SBC_1_REG              0x204
#define DDRC_DLL_SBC_2_REG              0x208
#define DDRC_DLL_SBC_3_REG              0x20C
#define DDRC_DLL_CTRL_SEL0_DIE0_REG     0x210
#define DDRC_DLL_CTRL_SEL0_DIE1_REG     0x214
#define DDRC_DLL_CTRL_SEL1_DIE0_REG     0x218
#define DDRC_DLL_CTRL_SEL1_DIE1_REG     0x21C
#define DDRC_DLL_CTRL_SEL2_DIE0_REG     0x220
#define DDRC_DLL_CTRL_SEL2_DIE1_REG     0x224

#define DDRC_BYTE0_DIE0_DLY0_REG        0x228
#define DDRC_BYTE0_DIE0_DLY1_REG        0x22C
#define DDRC_BYTE0_DIE0_DLY2_REG        0x230
#define DDRC_BYTE0_DIE0_DLY3_REG        0x234
#define DDRC_BYTE0_DIE0_DLY4_REG        0x238
#define DDRC_BYTE0_DIE0_DLY5_REG        0x23C
#define DDRC_BYTE1_DIE0_DLY0_REG        0x240
#define DDRC_BYTE1_DIE0_DLY1_REG        0x244
#define DDRC_BYTE1_DIE0_DLY2_REG        0x248
#define DDRC_BYTE1_DIE0_DLY3_REG        0x24C
#define DDRC_BYTE1_DIE0_DLY4_REG        0x250
#define DDRC_BYTE1_DIE0_DLY5_REG        0x254
#define DDRC_BYTE2_DIE0_DLY0_REG        0x258
#define DDRC_BYTE2_DIE0_DLY1_REG        0x25C
#define DDRC_BYTE2_DIE0_DLY2_REG        0x260
#define DDRC_BYTE2_DIE0_DLY3_REG        0x264
#define DDRC_BYTE2_DIE0_DLY4_REG        0x268
#define DDRC_BYTE2_DIE0_DLY5_REG        0x26C
#define DDRC_BYTE3_DIE0_DLY0_REG        0x270
#define DDRC_BYTE3_DIE0_DLY1_REG        0x274
#define DDRC_BYTE3_DIE0_DLY2_REG        0x278
#define DDRC_BYTE3_DIE0_DLY3_REG        0x27C
#define DDRC_BYTE3_DIE0_DLY4_REG        0x280
#define DDRC_BYTE3_DIE0_DLY5_REG        0x284
#define DDRC_BYTE0_DIE1_DLY0_REG        0x288
#define DDRC_BYTE0_DIE1_DLY1_REG        0x28C
#define DDRC_BYTE0_DIE1_DLY2_REG        0x290
#define DDRC_BYTE0_DIE1_DLY3_REG        0x294
#define DDRC_BYTE0_DIE1_DLY4_REG        0x298
#define DDRC_BYTE0_DIE1_DLY5_REG        0x29C
#define DDRC_BYTE1_DIE1_DLY0_REG        0x2A0
#define DDRC_BYTE1_DIE1_DLY1_REG        0x2A4
#define DDRC_BYTE1_DIE1_DLY2_REG        0x2A8
#define DDRC_BYTE1_DIE1_DLY3_REG        0x2AC
#define DDRC_BYTE1_DIE1_DLY4_REG        0x2B0
#define DDRC_BYTE1_DIE1_DLY5_REG        0x2B4
#define DDRC_BYTE2_DIE1_DLY0_REG        0x2B8
#define DDRC_BYTE2_DIE1_DLY1_REG        0x2BC
#define DDRC_BYTE2_DIE1_DLY2_REG        0x2C0
#define DDRC_BYTE2_DIE1_DLY3_REG        0x2C4
#define DDRC_BYTE2_DIE1_DLY4_REG        0x2C8
#define DDRC_BYTE2_DIE1_DLY5_REG        0x2CC
#define DDRC_BYTE3_DIE1_DLY0_REG        0x2D0
#define DDRC_BYTE3_DIE1_DLY1_REG        0x2D4
#define DDRC_BYTE3_DIE1_DLY2_REG        0x2D8
#define DDRC_BYTE3_DIE1_DLY3_REG        0x2DC
#define DDRC_BYTE3_DIE1_DLY4_REG        0x2E0
#define DDRC_BYTE3_DIE1_DLY5_REG        0x2E4
#define DDRC_CK_DLY_REG                 0x2E8
#define DDRC_CA_DELAY_COARSE_REG        0x2EC
#define DDRC_CKE_DELAY_COARSE_REG       0x314

#define DDRIO_WCK_DUTY_CTRL_B0          0x33c
#define DDRIO_WCK_DUTY_CTRL_B1          0x340
#define DDRIO_WCK_DUTY_CTRL_B2          0x344
#define DDRIO_WCK_DUTY_CTRL_B3          0x348
#define DDRIO_CA_DUTY_CTRL_ChA          0x34C


#define DDRC_PAD_TERM_REG               0x360
#define DDRC_DQS_PUPD_REG               0x364
#define DDRC_ZCTRL_REG                  0x36c
#define DDRC_CA_PAD_CTRL_REG            0x370
#define DDRC_DQ_PAD_CTRL_REG            0x374
#define DDRC_VREF_0_REG                 0x378
#define DDRC_VREF_1_REG                 0x37C
#define DDRC_IBIAS_REG                  0x380
#define DDRIO_ZCTRL_STATUS              0x384

//register masks
#define DDRC_IO_CTRL_CKE_0_0            0x0001
#define DDRC_IO_CTRL_CKE_0_1            0x0002
#define DDRC_IO_CTRL_CKE_1_0            0x0004
#define DDRC_IO_CTRL_CKE_1_1            0x0008
#define DDRC_IO_CTRL_RESET              0x0010
#define DDRC_IO_CTRL_CMD_DIS            0x0020

#define DDRC_ZQ_CTRL_FORCE              0x0001
#define DDRC_ZQ_CTRL_FORCE_IMPP         0x0002
#define DDRC_ZQ_CTRL_FORCE_IMPN         0x0004
#define DDRC_ZQ_CTRL_LONG               0x0080
#define DDRC_ZQ_CTRL_SHORT              0x0100
#define DDRC_ZQ_CTRL_RESET              0x0200
#define DDRC_ZQ_CTRL_PU_MOD_SEL         0x0400
#define DDRIO_ZCTRL_STATUS_ACK          0x2000

#define DDRC_INIT_CTRL_DIE1             0x00400
#define DDRC_INIT_CTRL_SPAD_CLIB        0x0040
#define DDRC_INIT_CTRL_PAD_CLIB         0x0020
#define DDRC_INIT_CTRL_ZQ_CLIB          0x0010
#define DDRC_INIT_CTRL_DLL_RESET        0x0008
#define DDRC_INIT_CTRL_GET_RTT          0x0004
#define DDRC_INIT_CTRL_IMM_REF          0x0002
#define DDRC_INIT_CTRL_PRE_ALL          0x0001

#define DDRC_LPDDR4_DQ0_CA_VREF_REG     0x238
#define DDRC_LPDDR4_DQ1_CA_VREF_REG     0x2b8
#define DDRC_LPDDR4_DQ2_CA_VREF_REG     0x338
#define DDRC_LPDDR4_DQ3_CA_VREF_REG     0x3b8
#define DDRC_DLL_CTRL_SEL_MISC_REG      0xec
#define DDRC_PAD_TERM4_REG              0xa4
#define DDRC_WDQS_MODE_REG              0xdc
#define DDRC_DTTE_REG_ADDR              0xf8
#define DDRC_DTTE_REG_DATA              0xfc
#define DDRC_DTTE_PARAM_REG             0x140

#define DDRC_LPDDR4_DQ0_READ_DLYA_REG   0X218
#define DDRC_LPDDR4_DQ1_READ_DLYA_REG   0X298
#define DDRC_LPDDR4_DQ2_READ_DLYA_REG   0X318
#define DDRC_LPDDR4_DQ3_READ_DLYA_REG   0X398
#define DDRC_LPDDR4_DQ0_READ_DLYB_REG   0X21C
#define DDRC_LPDDR4_DQ1_READ_DLYB_REG   0X29C
#define DDRC_LPDDR4_DQ2_READ_DLYB_REG   0X31C
#define DDRC_LPDDR4_DQ3_READ_DLYB_REG   0X39C

#define DDRC_LPDDR4_DQ0_WRITE_DLYA_REG  0x200
#define DDRC_LPDDR4_DQ1_WRITE_DLYA_REG  0x280
#define DDRC_LPDDR4_DQ2_WRITE_DLYA_REG  0x300
#define DDRC_LPDDR4_DQ3_WRITE_DLYA_REG  0x380

#define DDRC_LPDDR4_DQ0_WRITE_DLYB_REG  0x204
#define DDRC_LPDDR4_DQ1_WRITE_DLYB_REG  0x284
#define DDRC_LPDDR4_DQ2_WRITE_DLYB_REG  0x304
#define DDRC_LPDDR4_DQ3_WRITE_DLYB_REG  0x384

#define DDRC_LPDDR4_DQ0_WRITE_DLYC_REG  0x208
#define DDRC_LPDDR4_DQ1_WRITE_DLYC_REG  0x288
#define DDRC_LPDDR4_DQ2_WRITE_DLYC_REG  0x308
#define DDRC_LPDDR4_DQ3_WRITE_DLYC_REG  0x388

#define DDRC_LPDDR4_DQ0_WRITE_DLYA_DIE1_REG  0x20c
#define DDRC_LPDDR4_DQ1_WRITE_DLYA_DIE1_REG  0x28c
#define DDRC_LPDDR4_DQ2_WRITE_DLYA_DIE1_REG  0x30c
#define DDRC_LPDDR4_DQ3_WRITE_DLYA_DIE1_REG  0x38c

#define DDRC_LPDDR4_DQ0_WRITE_DLYB_DIE1_REG  0x210
#define DDRC_LPDDR4_DQ1_WRITE_DLYB_DIE1_REG  0x290
#define DDRC_LPDDR4_DQ2_WRITE_DLYB_DIE1_REG  0x310
#define DDRC_LPDDR4_DQ3_WRITE_DLYB_DIE1_REG  0x390

#define DDRC_LPDDR4_DQ0_WRITE_DLYC_DIE1_REG  0x214
#define DDRC_LPDDR4_DQ1_WRITE_DLYC_DIE1_REG  0x294
#define DDRC_LPDDR4_DQ2_WRITE_DLYC_DIE1_REG  0x314
#define DDRC_LPDDR4_DQ3_WRITE_DLYC_DIE1_REG  0x394

#define DDRC_LPDDR4_DQS0_DLY_REG        0x228
#define DDRC_LPDDR4_DQS1_DLY_REG        0x2a8
#define DDRC_LPDDR4_DQS2_DLY_REG        0x328
#define DDRC_LPDDR4_DQS3_DLY_REG        0x3a8

#define DDRC_MODE_REG0_WRITE0           0x01000000
#define DDRC_MODE_REG1_WRITE0           0x01010000
#define DDRC_MODE_REG1_OCD_DEFAULT      0x00000380
#define DDRC_MODE_REG2_WRITE0           0x01020000
#define DDRC_MODE_REG3_WRITE0           0x01030000
#define DDRC_MODE_REG0_DLL_RESET        0x00000100
#define DDRC_MODE_DIE0_ENABLE           0x02000000
#define DDRC_MODE_DIE1_ENABLE           0x04000000
#define DDRC_MODE_BUSY                  0x80000000
#define DDRC_MODE_REG13_FS_WR_EN        0x40

#define DDRC_CTRL_MASK                  0x007f
#define DDRC_CTRL_DRAM_CS               0x0040
#define DDRC_CTRL_DEEP_PD_EN            0x0020
#define DDRC_CTRL_IDDQ_TST_EN           0x0010
#define DDRC_CTRL_CKE                   0x0004
#define DDRC_CTRL_AUTO_REF_EN           0x0002
#define DDRC_CTRL_ENABLE                0x0001
#define DDRC_CTRL_DIE0_DISABLE_MASK     0x20000000
#define DDRC_CTRL_DIE1_DISABLE_MASK     0x40000000
#define DDRC_CTRL_CHIP_SEL_MASK         0x60000000

#define DDRC_LPDDR4_CTRL_TRAINING_MODE  0x01000000
#define DDRC_UINST_SRX                  0xf0000000

#define DDRC_CS_DIE1_DISABLE            0x80000000
#define DDRC_CS_DIE0_DISABLE            0x40000000
#define DDRC_CS_MASK                    0xc0000000

#define ECCH_MODE_REG_ADDR              0xdffe1780
#define ECCH_STATUS_REG_ADDR            0xdffe17e0

#define DTTE_CTRL_REG                   0x0
#define DTTE_MODE_REG                   0x4
#define DTTE_MR14_REG                   0xec

#define RTC_ALARM_TIME_WRITE_REG        0x2c
#define RTC_CUR_TIME_WRITE_REG          0x30
#define RTC_CUR_TIME_READ_REG           0x34
#define RTC_ALARM_TIME_READ_REG         0x38
#define RTC_STATUS_REG                  0x3C
#define RTC_RESET_REG                   0x40
#define PWC_STATUS_READ_REG             0xB4
#define PWC_STATUS_WRITE_REG            0xC0
#define RTC_STATUS_REG                  0x3C

#define I2S_PING_PONG_ENABLE            0x50

#define DLL_SETTING0D0PARAM           (0x0  << 2)
#define DLL_SETTING0D1PARAM           (0x1  << 2)
#define DLL_SETTING1D0PARAM           (0x2  << 2)
#define DLL_SETTING1D1PARAM           (0x3  << 2)
#define DLL_SETTING2D0PARAM           (0x4  << 2)
#define DLL_SETTING2D1PARAM           (0x5  << 2)
#define DLL_SBC0PARAM                 (0x6  << 2)
#define DLL_SBC1PARAM                 (0x7  << 2)
#define DLL_SBC2PARAM                 (0x8  << 2)
#define DLL_SBC3PARAM                 (0x9  << 2)
#define DRAM_BYTEMAPPARAM             (0xA  << 2)
#define DRAM_DIE0BYTE0DLY0PARAM       (0xB  << 2)
#define DRAM_DIE0BYTE0DLY1PARAM       (0xC  << 2)
#define DRAM_DIE0BYTE0DLY2PARAM       (0xD  << 2)
#define DRAM_DIE0BYTE0DLY3PARAM       (0xE  << 2)
#define DRAM_DIE0BYTE0DLY4PARAM       (0xF  << 2)
#define DRAM_DIE0BYTE0DLY5PARAM       (0x10 << 2)
#define DRAM_DIE0BYTE1DLY0PARAM       (0x11 << 2)
#define DRAM_DIE0BYTE1DLY1PARAM       (0x12 << 2)
#define DRAM_DIE0BYTE1DLY2PARAM       (0x13 << 2)
#define DRAM_DIE0BYTE1DLY3PARAM       (0x14 << 2)
#define DRAM_DIE0BYTE1DLY4PARAM       (0x15 << 2)
#define DRAM_DIE0BYTE1DLY5PARAM       (0x16 << 2)
#define DRAM_DIE0BYTE2DLY0PARAM       (0x17 << 2)
#define DRAM_DIE0BYTE2DLY1PARAM       (0x18 << 2)
#define DRAM_DIE0BYTE2DLY2PARAM       (0x19 << 2)
#define DRAM_DIE0BYTE2DLY3PARAM       (0x1A << 2)
#define DRAM_DIE0BYTE2DLY4PARAM       (0x1B << 2)
#define DRAM_DIE0BYTE2DLY5PARAM       (0x1C << 2)
#define DRAM_DIE0BYTE3DLY0PARAM       (0x1D << 2)
#define DRAM_DIE0BYTE3DLY1PARAM       (0x1E << 2)
#define DRAM_DIE0BYTE3DLY2PARAM       (0x1F << 2)
#define DRAM_DIE0BYTE3DLY3PARAM       (0x20 << 2)
#define DRAM_DIE0BYTE3DLY4PARAM       (0x21 << 2)
#define DRAM_DIE0BYTE3DLY5PARAM       (0x22 << 2)
#define DRAM_DIE1BYTE0DLY0PARAM       (0x23 << 2)
#define DRAM_DIE1BYTE0DLY1PARAM       (0x24 << 2)
#define DRAM_DIE1BYTE0DLY2PARAM       (0x25 << 2)
#define DRAM_DIE1BYTE0DLY3PARAM       (0x26 << 2)
#define DRAM_DIE1BYTE0DLY4PARAM       (0x27 << 2)
#define DRAM_DIE1BYTE0DLY5PARAM       (0x28 << 2)
#define DRAM_DIE1BYTE1DLY0PARAM       (0x29 << 2)
#define DRAM_DIE1BYTE1DLY1PARAM       (0x2A << 2)
#define DRAM_DIE1BYTE1DLY2PARAM       (0x2B << 2)
#define DRAM_DIE1BYTE1DLY3PARAM       (0x2C << 2)
#define DRAM_DIE1BYTE1DLY4PARAM       (0x2D << 2)
#define DRAM_DIE1BYTE1DLY5PARAM       (0x2E << 2)
#define DRAM_DIE1BYTE2DLY0PARAM       (0x2F << 2)
#define DRAM_DIE1BYTE2DLY1PARAM       (0x30 << 2)
#define DRAM_DIE1BYTE2DLY2PARAM       (0x31 << 2)
#define DRAM_DIE1BYTE2DLY3PARAM       (0x32 << 2)
#define DRAM_DIE1BYTE2DLY4PARAM       (0x33 << 2)
#define DRAM_DIE1BYTE2DLY5PARAM       (0x34 << 2)
#define DRAM_DIE1BYTE3DLY0PARAM       (0x35 << 2)
#define DRAM_DIE1BYTE3DLY1PARAM       (0x36 << 2)
#define DRAM_DIE1BYTE3DLY2PARAM       (0x37 << 2)
#define DRAM_DIE1BYTE3DLY3PARAM       (0x38 << 2)
#define DRAM_DIE1BYTE3DLY4PARAM       (0x39 << 2)
#define DRAM_DIE1BYTE3DLY5PARAM       (0x3A << 2)
#define DRAM_CKDLYPARAM               (0x3B << 2)
#define DRAM_CADELAYCOARSEPARAM       (0x3C << 2)
#define DRAM_CKEDLYPARAM              (0x3D << 2)
#define DRAM_VREF0PARAM               (0x3E << 2)
#define DRAM_VREF1PARAM               (0x3F << 2)
#define DRAM_WRITEVREF0PARAM          (0x40 << 2)
#define DRAM_WRITEVREF1PARAM          (0x41 << 2)
#define DRAM_PADTERMPARAM             (0x42 << 2)
#define DRAM_DQSPUPDPARAM             (0x43 << 2)
#define DRAM_CAPADCTRLPARAM           (0x44 << 2)
#define DRAM_DQPADCTRLPARAM           (0x45 << 2)
#define DRAM_IBIASPARAM               (0x46 << 2)
/* LPDDR5 */
#define LPDDR5_MODEREG1PARAMFSP0      (0x47 << 2)
#define LPDDR5_MODEREG2PARAMFSP0      (0x48 << 2)
#define LPDDR5_MODEREG3PARAMFSP0      (0x49 << 2)
#define LPDDR5_MODEREG10PARAMFSP0     (0x4A << 2)
#define LPDDR5_MODEREG11PARAMFSP0     (0x4B << 2)
#define LPDDR5_MODEREG12C0D0PARAM     (0x4C << 2)
#define LPDDR5_MODEREG12C0D1PARAM     (0x4D << 2)
#define LPDDR5_MODEREG12C1D0PARAM     (0x4E << 2)
#define LPDDR5_MODEREG12C1D1PARAM     (0x4F << 2)
#define LPDDR5_MODEREG13PARAMFSP0     (0x50 << 2)
#define LPDDR5_MODEREG14C0D0PARAM     (0x51 << 2)
#define LPDDR5_MODEREG14C0D1PARAM     (0x52 << 2)
#define LPDDR5_MODEREG14C1D0PARAM     (0x53 << 2)
#define LPDDR5_MODEREG14C1D1PARAM     (0x54 << 2)
#define LPDDR5_MODEREG15C0D0PARAM     (0x55 << 2)
#define LPDDR5_MODEREG15C0D1PARAM     (0x56 << 2)
#define LPDDR5_MODEREG15C1D0PARAM     (0x57 << 2)
#define LPDDR5_MODEREG15C1D1PARAM     (0x58 << 2)
#define LPDDR5_MODEREG16PARAMFSP0     (0x59 << 2)
#define LPDDR5_MODEREG17PARAMFSP0     (0x5A << 2)
#define LPDDR5_MODEREG18PARAMFSP0     (0x5B << 2)
#define LPDDR5_MODEREG19PARAMFSP0     (0x5C << 2)
#define LPDDR5_MODEREG20PARAMFSP0     (0x5D << 2)
#define LPDDR5_MODEREG22PARAMFSP0     (0x5E << 2)
#define LPDDR5_MODEREG24PARAMFSP0     (0x5F << 2)
#define LPDDR5_MODEREG28PARAMFSP0     (0x60 << 2)
#define LPDDR5_MODEREG30C0D0PARAMFSP0 (0x61 << 2)
#define LPDDR5_MODEREG30C0D1PARAMFSP0 (0x62 << 2)
#define LPDDR5_MODEREG30C1D0PARAMFSP0 (0x63 << 2)
#define LPDDR5_MODEREG30C1D1PARAMFSP0 (0x64 << 2)
#define LPDDR5_MODEREG41PARAMFSP0     (0x65 << 2)
#define LPDDR5_MODEREG46PARAMFSP_DIE0 (0x66 << 2)
#define LPDDR5_MODEREG46PARAMFSP_DIE1 (0x67 << 2)
/* LPDDR4 */
#define LPDDR4_MODEREG1PARAMFSP0      (0x47 << 2)
#define LPDDR4_MODEREG2PARAMFSP0      (0x48 << 2)
#define LPDDR4_MODEREG3PARAMFSP0      (0x49 << 2)
#define LPDDR4_MODEREG11PARAMFSP0     (0x4A << 2)
#define LPDDR4_MODEREG12C0D0PARAM     (0x4B << 2)
#define LPDDR4_MODEREG12C0D1PARAM     (0x4C << 2)
#define LPDDR4_MODEREG12C1D0PARAM     (0x4D << 2)
#define LPDDR4_MODEREG12C1D1PARAM     (0x4E << 2)
#define LPDDR4_MODEREG13PARAMFSP0     (0x4F << 2)
#define LPDDR4_MODEREG14C0D0PARAM     (0x50 << 2)
#define LPDDR4_MODEREG14C0D1PARAM     (0x51 << 2)
#define LPDDR4_MODEREG14C1D0PARAM     (0x52 << 2)
#define LPDDR4_MODEREG14C1D1PARAM     (0x53 << 2)
#define LPDDR4_MODEREG22PARAMFSP0     (0x54 << 2)

/**
 * Mapping for BST LP4/LP5 training
 */
#define DRAM_CONTRL                     DDRC_CTRL_REG
#define DRAM_UINST1                     DDRC_UINST1_REG
#define DRAM_UINST2                     DDRC_UINST2_REG
#define DRAM_UINST4                     DDRC_UINST4_REG
#define DRAM_UINST5                     DDRC_UINST5_REG
#define DRAM_MODE_REG                   DDRC_MODE_REG
#define DDRIO_BYTE_DLY0(b, d)           (0x00000228 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY1(b, d)           (0x0000022c + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY2(b, d)           (0x00000230 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY3(b, d)           (0x00000234 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY4(b, d)           (0x00000238 + (b) * 24 + (d) * 96)
#define DDRIO_BYTE_DLY5(b, d)           (0x0000023c + (b) * 24 + (d) * 96)
#define DDRIO_VREF_0                    DDRC_VREF_0_REG
#define DDRIO_VREF_1                    DDRC_VREF_1_REG
#define DRAM_CONFIG2                    DDRC_CONFIG2_REG
#define DRAM_INIT_CTL                   DDRC_INIT_CTRL_REG
#define DRAM_INIT_CTL_GET_RTT_EN        DDRC_INIT_CTRL_GET_RTT
#define DRAM_DDRC_MISC_2                0x000003c0
#define DRAM_DDRC_STATUS                0x000003a4
#define DRAM_DDRC_MR46                  0x000003d4
#define DRAM_BROADCAST_OFFSET           0x4000
#define DRAM_CONTROL_AUTO_REF_EN        DDRC_CTRL_AUTO_REF_EN
#define DRAM_CONTROL_ENABLE             DDRC_CTRL_ENABLE
#define RCT_BASE                        AMBA_DBG_PORT_RCT_BASE_ADDR
#define RCT_TIMER_OFFSET                RCT_TIMER_COUNT_REG
#define RCT_TIMER_CTRL_OFFSET           RCT_TIMER_CTRL_REG
#define DRAM_WRITE_VREF_0               DDRC_WRITE_VREF_0_REG
#define DRAM_WRITE_VREF_1               DDRC_WRITE_VREF_1_REG

/**
 * Mapping for the WDT external pin function
 */
#if defined(CONFIG_WDT_PIN_GPIO_0) || defined(CONFIG_WDT_PIN_GPIO_5) || defined(CONFIG_WDT_PIN_GPIO_19)
#define GPIO_BASE_ADDR          AMBA_CORTEX_A76_GPIO0_BASE_ADDR
#define IOMUX_REG_0             0x0
#define IOMUX_REG_1             0x4
#define IOMUX_REG_2             0x8
#elif defined(CONFIG_WDT_PIN_GPIO_36)
#define GPIO_BASE_ADDR          AMBA_CORTEX_A76_GPIO1_BASE_ADDR
#define IOMUX_REG_0             0xc
#define IOMUX_REG_1             0x10
#define IOMUX_REG_2             0x14
#elif defined(CONFIG_WDT_PIN_GPIO_116) || defined(CONFIG_WDT_PIN_GPIO_119)
#define GPIO_BASE_ADDR          AMBA_CORTEX_A76_GPIO3_BASE_ADDR
#define IOMUX_REG_0             0x24
#define IOMUX_REG_1             0x28
#define IOMUX_REG_2             0x2c
#elif defined(CONFIG_WDT_PIN_GPIO_138)
#define GPIO_BASE_ADDR          AMBA_CORTEX_A76_GPIO4_BASE_ADDR
#define IOMUX_REG_0             0x30
#define IOMUX_REG_1             0x34
#define IOMUX_REG_2             0x38
#endif
#if defined(CONFIG_WDT_PIN_GPIO_0) || defined(CONFIG_WDT_PIN_GPIO_5) || defined(CONFIG_WDT_PIN_GPIO_116)
#define IOMUX_FUNC_ID           0x2
#elif defined(CONFIG_WDT_PIN_GPIO_36)
#define IOMUX_FUNC_ID           0x3
#elif defined(CONFIG_WDT_PIN_GPIO_119) || defined(CONFIG_WDT_PIN_GPIO_138)
#define IOMUX_FUNC_ID           0x4
#elif defined(CONFIG_WDT_PIN_GPIO_19)
#define IOMUX_FUNC_ID           0x5
#endif

#if defined(CONFIG_WDT_PIN_GPIO_0)
#define PIN_ID           0
#elif defined(CONFIG_WDT_PIN_GPIO_5)
#define PIN_ID           5
#elif defined(CONFIG_WDT_PIN_GPIO_19)
#define PIN_ID           19
#elif defined(CONFIG_WDT_PIN_GPIO_36)
#define PIN_ID           36
#elif defined(CONFIG_WDT_PIN_GPIO_116)
#define PIN_ID           116
#elif defined(CONFIG_WDT_PIN_GPIO_119)
#define PIN_ID           119
#elif defined(CONFIG_WDT_PIN_GPIO_138)
#define PIN_ID           138
#endif

#define PIN_OFFSET              (PIN_ID&0x1f)
/**
 * Mapping for BST Large size
 */
#define SYS_CONFIG_OFFSET               RCT_POC_REG
#define SCALER_SD0_OFFSET               PLL_SD48_POST_SCALER_REG
#define FIO_BASE                        AMBA_CORTEX_A76_FLASH_CPU_BASE_ADDR
#define SPINOR_BASE                     AMBA_CORTEX_A76_NOR_SPI_BASE_ADDR
#define AHB_SP0_RAM_BASE                0x20e0030000

/**
 * Mapping copied from AmbaBLD.h in BLD to share the same definition
 */
#define DRAM_TRAIN_PROC_OPMODE_OFST             24U
#define DRAM_TRAIN_OPMODE_BOTH_RW_DONE          13U      // Add for BL2 training stroage

#endif  /* AMBA_BST_H */
