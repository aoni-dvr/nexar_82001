/**
 *  @file AmbaBST.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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

#include "AmbaCortexA53.h"
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
#define PLL_DDR_PLL_CTRL_REG            0x0dc
#define PLL_DDR_PLL_CTRL2_REG           0x110
#define PLL_DDR_PLL_CTRL3_REG           0x114

#define CG_DDR_INIT_REG                 0x220

#define DDRC_DLL_SETTING_0_REG          0x100
#define DDRC_DLL_SETTING_1_REG          0x104
#define DDRC_DLL_SETTING_2_REG          0x108
#define DDRC_DLL_SETTING_3_REG          0x10c
#define DDRC_DLL_CTRL_SEL_0_REG         0x120
#define DDRC_DLL_CTRL_SEL_1_REG         0x124
#define DDRC_DLL_CTRL_SEL_2_REG         0x128
#define DDRC_DLL_CTRL_SEL_3_REG         0x12c
#define DDRC_DLL_CTRL_SEL_MISC_REG      0x110
#define DDRC_DLL_SYNC_CTRL_SEL_0_REG    0x140
#define DDRC_DLL_SYNC_CTRL_SEL_1_REG    0x144
#define DDRC_DLL_SYNC_CTRL_SEL_2_REG    0x148
#define DDRC_DLL_SYNC_CTRL_SEL_3_REG    0x14c

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

#define SYS_CONFIG_NAND_FLASH_PAGE_4K   0x00040000  /* 0: 2K Byte, 1: 4K Byte */
#define SYS_CONFIG_NAND_READ_CONFIRM    0x00020000  /* 0: use, 1: doesn't use */
#define SYS_CONFIG_BOOT_MEDIA           0x00000001  /* 0: NAND, 1: NOR */
#define SYS_CONFIG_SPI_BOOT             0x00001000
#define SYS_CONFIG_NAND_BCH             0x00010000  /* 0: Disable, 1: Enable */
#define SYS_CONFIG_NAND_SPARE_2X        0x00008000  /* 0: NAND Spare cell 1X, 1: 2X */
#define SYS_CONFIG_SD8_BOOT             0x00008000  /* SD8 boot */
#define SYS_CONFIG_SD4_BOOT             0x00010000  /* SD4 boot */

#ifdef CONFIG_ARCH_AMBA_CAMERA
//FIXME, it is used for Linux and used before function AmbaSYS_EnableFeature done.
#define SYS_CLOCK_CTRL_INIT_VALUE       0xFFFFFFFF
#else
#define SYS_CLOCK_CTRL_INIT_VALUE       0x00000400
#endif

/**
 * DRAM Controller Registers
 */
#define DRAMC_MODE_REG                  0x00
#define DRAMC_SEC_CTRL                  0x218
#define DRAMC_SEC_KEY0                  0X230   /* 0x230~0x240, 160 bits in total */
#define DRAMC_SEC_KEY1                  0X234
#define DRAMC_SEC_KEY2                  0X238
#define DRAMC_SEC_KEY3                  0X23c
#define DRAMC_SEC_KEY4                  0X240

/**
 * DDR Controller Registers
 */
#define DDRC_CTRL_REG                   0x00
#define DDRC_CONFIG_REG                 0x04
#define DDRC_TIMING1_REG                0x08
#define DDRC_TIMING2_REG                0x0c
#define DDRC_TIMING3_REG                0x10
#define DDRC_TIMING4_REG                0x14
#define DDRC_INIT_CTRL_REG              0x18
#define DDRC_MODE_REG                   0x1C
#define DDRC_SELF_REFRESH_REG           0x20
#define DDRC_DQS_SYNC_REG               0x24
#define DDRC_PAD_TERM_REG               0x28
#define DDRC_ZQ_CALIB_REG               0x2c
#define DDRC_RSVD_SPACE_REG             0x30
#define DDRC_BYTE_MAP_REG               0x34
#define DDRC_POWER_DOWN_CTRL_REG        0x38
#define DDRC_DLL_CALIB_REG              0x3c
#define DDRC_TIMING5_LP4TRAIN_REG       0x44
#define DDRC_PAD_TERM2_REG              0x48
#define DDRC_PAD_TERM3_REG              0x4c
#define DDRC_CTRL_UINST_REG             0x90
#define DDRC_UINST_BUSY_REG             0x9c
#define DDRC_LPDDR4_DQ_WRITE_DLY_REG    0xa4
#define DDRC_LPDDR4_DQ_READ_DLY_REG     0xa8
#define DDRC_LPDDR4_DQS_WRITE_DLY_REG   0xac
#define DDRC_LPDDR4_DQ_CA_VREF_REG      0xb4
#define DDRC_LPDDR4_TRAIN_SCRATCHPAD    0xb8
#define DDRC_LPDDR4_TRAIN_MPC_RDLY_REG  0xcc

#define DDRC_MODE_REG0_WRITE0           0x01000000
#define DDRC_MODE_REG1_WRITE0           0x01010000
#define DDRC_MODE_REG1_OCD_DEFAULT      0x00000380
#define DDRC_MODE_REG2_WRITE0           0x01020000
#define DDRC_MODE_REG3_WRITE0           0x01030000
#define DDRC_MODE_REG0_DLL_RESET        0x00000100
#define DDRC_MODE_BUSY                  0x80000000
#define DDRC_MODE_REG13_FS_WR_EN        0x40

#define DDRC_CTRL_MASK                  0x007f
#define DDRC_CTRL_DRAM_CS               0x0040
#define DDRC_CTRL_DEEP_PD_EN            0x0020
#define DDRC_CTRL_IDDQ_TST_EN           0x0010
#define DDRC_CTRL_RESET                 0x0008
#define DDRC_CTRL_CKE                   0x0004
#define DDRC_CTRL_AUTO_REF_EN           0x0002
#define DDRC_CTRL_ENABLE                0x0001
#define DDRC_CTRL_DIE0_DISABLE_MASK     0x20000000
#define DDRC_CTRL_DIE1_DISABLE_MASK     0x40000000
#define DDRC_CTRL_CHIP_SEL_MASK         0x60000000

#define DDRC_INIT_CTRL_PAD_CLIB         0x0020
#define DDRC_INIT_CTRL_ZQ_CLIB          0x0010
#define DDRC_INIT_CTRL_DLL_RESET        0x0008
#define DDRC_INIT_CTRL_GET_RTT          0x0004
#define DDRC_INIT_CTRL_IMM_REF          0x0002
#define DDRC_INIT_CTRL_PRE_ALL          0x0001

#define DDRC_LPDDR4_CTRL_TRAINING_MODE  0x01000000
#define DDRC_UINST_SRX                  0xf0000000

#define DDRC_CS_DIE1_DISABLE            0x80000000
#define DDRC_CS_DIE0_DISABLE            0x40000000
#define DDRC_CS_MASK                    0xc0000000
/**
 * Mapping for BST LP4 training
 */
#define DRAM_CTL                        DDRC_CTRL_REG
#define DRAM_CFG                        DDRC_CONFIG_REG
#define DRAM_INIT_CTL                   DDRC_INIT_CTRL_REG
#define DRAM_TIMING5_LP4TRAIN           DDRC_TIMING5_LP4TRAIN_REG
#define DRAM_MPC_WRITE_DATA             0x00000050
#define DRAM_MPC_READ_DATA              0x00000070
#define DRAM_UINST                      DDRC_CTRL_UINST_REG
#define DRAM_TRAINING_COMMAND_DONE      DDRC_UINST_BUSY_REG
#define DRAM_LPDDR4_DQ_WRITE_DELAY      DDRC_LPDDR4_DQ_WRITE_DLY_REG
#define DRAM_LPDDR4_DQ_READ_DELAY       DDRC_LPDDR4_DQ_READ_DLY_REG
#define DRAM_LPDDR4_DQCA_VREF           DDRC_LPDDR4_DQ_CA_VREF_REG
#define DRAM_LPDDR4_SCRATCHPAD          DDRC_LPDDR4_TRAIN_SCRATCHPAD
#define DDRIO0_DLL0_REG                 DDRC_DLL_SETTING_0_REG
#define DDRIO0_DLL_STATUS_SEL0          0x00000130
#define RCT_TIMER_OFFSET                RCT_TIMER_COUNT_REG
#define RCT_TIMER_CTRL_OFFSET           RCT_TIMER_CTRL_REG

/**
 * WDT Registers
 */
#define WDT_COUNTER_REG                 0x00
#define WDT_RELOAD_REG                  0x04
#define WDT_RESTART_REG                 0x08
#define WDT_CTRL_REG                    0x0c
#define WDT_TIMEOUT_STATUS_REG          0x10
#define WDT_TIMEOUT_CLEAR_REG           0x14
#define WDT_RESET_WIDTH_REG             0x18

#define WDT_CTRL_ENABLE                 0x01
#define WDT_CTRL_RESET                  0x02
#define WDT_CTRL_IRQ                    0x04

/**
 * MISC Registers
 */
#define RTC_PSEQ1_DELAY_REG             0x20
#define RTC_PSEQ2_DELAY_REG             0x24
#define RTC_PSEQ3_DELAY_REG             0x28
#define RTC_PSEQ4_DELAY_REG             0xD0

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

#endif  /* AMBA_BST_H */
