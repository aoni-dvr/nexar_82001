/**
 *  @file AmbaDramTrain.h
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
 *  @details Definitions & Constants for LPDDR4 DRAM Training
 *
 */

#ifndef AMBA_DRAM_TRAIN_H
#define AMBA_DRAM_TRAIN_H

#include "AmbaCortexA53.h"
#include "AmbaDebugPort.h"

#define SYS_CONFIG_NAND_FLASH_PAGE_4K   0x00040000  /* 0: 2K Byte, 1: 4K Byte */
#define SYS_CONFIG_NAND_SPARE_2X        0x00008000  /* 0: NAND Spare cell 1X, 1: 2X */

#define SYS_RESET_REG                   0x068
#define SYS_CONFIG_REG                  0x034
#define SYS_CONFIG_RAW_REG              0x2FC

/**
 * DRAM Controller Registers
 */
#define DRAMC_MODE_REG                  0x00

#define AMBA_SCRATCH_PAD_BASE_ADDR      0xe8020000U
#define AHB_SCRATCH_PAD_LOG_SPACE       (AMBA_SCRATCH_PAD_BASE_ADDR + 0x1200U)
#define AHB_SCRATCH_PAD_PARAM_SPACE     (AHB_SCRATCH_PAD_LOG_SPACE - 0x10U)

#define AHB_SCRATCH_PAD_LOG_SPACE_END           (AMBA_SCRATCH_PAD_BASE_ADDR + 0x1800U)
#define AMBA_RAM_DRAM_TRAINING_STAGE2_OFFSET    (AMBA_SCRATCH_PAD_BASE_ADDR + 0x1800U)

#define DDRC_UINST_MPCW                     0x1
#define DDRC_UINST_MPCR                     0x2
#define DDRC_UINST_MPCR_DQ                  0x3
#define DDRC_UINST_MRW                      0x4
#define DDRC_UINST_CKED                     0x5
#define DDRC_UINST_CKEU                     0x6
#define DDRC_UINST_DQD                      0x7
#define DDRC_UINST_DQC                      0x8
#define DDRC_UINST_CAD                      0x9
#define DDRC_UINST_ROR                      0xa
#define DDRC_UINST_REFRESH                  0xb
#define DDRC_UINST_DQS0D                    0xc
#define DDRC_UINST_DQS0U                    0xd
#define DDRC_UINST_MRR                      0xe
#define DDRC_UINST_SR                       0xf

#define HOST0_DDRC_REG_BASE_ADDR            (AMBA_CORTEX_A53_DDR_CTRL0_BASE_ADDR - AMBA_CORTEX_A53_DRAM_CONFIG_VIRT_BASE_ADDR + AMBA_CORTEX_A53_DRAM_CONFIG_PHYS_BASE_ADDR)
#define DDRC_CONTROL_REG                    0x0
#define DDRC_CONFIG_REG                     0x4
#define DDRC_INIT_CTL_REG                   0x18
#define DDRC_MODE_REG                       0x1c
#define DDRC_SELF_REFRESH_REG               0x20
#define DDRC_DQS_SYNC_REG                   0x24
#define DDRC_TIMING5_LP4TRAIN               0x44
#define DDRC_WRITE_FIFO_BASE_REG            0x50
#define DDRC_READ_FIFO_BASE_REG             0x70
#define DDRC_UINST_REG                      0x90
#define DDRC_UINST_BUSY_REG                 0x9c
#define DDRC_DQ_CAPTURE_REG                 0xa0
#define DDRC_WRITE_DQ_DELAY_REG             0xa4
#define DDRC_READ_DQ_DELAY_REG              0xa8
#define DDRC_READ_DQ_VREF_REG               0xb4
#define DDRC_SCRATCHPAD_REG                 0xb8
#define DDRC_MODEREAD_REG                   0xc8
#define HOST0_DLL0_REG                      0x100
#define HOST0_DLL1_REG                      0x104
#define HOST0_DLL2_REG                      0x108
#define HOST0_DLL3_REG                      0x10C
#define DDRC_DLL_CTRL_SEL_MISC_REG          0x110
#define DDRC_DLL_CTRL_SEL_0_REG             0x120
#define DDRIO_DLL_STATUS_BASE               0x130

#endif
