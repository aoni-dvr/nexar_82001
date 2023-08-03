/**
 *  @file AmbaDramTrain_SpiNAND.h
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
 *  @details Definitions & Constants for Dram Training SpiNAND portion
 *
 */

#ifndef AMBA_DRAM_TRAIN_SPINAND_H
#define AMBA_DRAM_TRAIN_SPINAND_H

#include "AmbaDramTrain.h"

/**
 * Flash I/O and DMA Control Registers
 */
/* SPINAND CTRL REG */
#define SPINAND_CTRL_MAX_CMD_LOOP       0x0003ffff
#define SPINAND_CTRL_PlANE_SELECT_BIT5  0x10000000
#define SPINAND_CTRL_PlANE_SELECT_BIT6  0x20000000
#define SPINAND_CTRL_PlANE_SELECT_BIT7  0x30000000

/* SPINAND CUSTOM CMD1 REG */
#define SPINAND_CUSTOM_CMD1_ENABLE_AUTO_STATUS          0x40000000
#define SPINAND_CUSTOM_CMD1_DATA_SRC_DMA                0x00400000
#define SPINAND_CUSTOM_CMD1_ADDR_SRC_CYCLE              0x00001800
#define SPINAND_CUSTOM_CMD1_ADDR_SRC_BY_ROW             0x00000200

/* SPINAND CUSTOM CMD2 REG */
#define SPINAND_CUSTOM_CMD2_ENABLE                      0x04000000
#define SPINAND_CUSTOM_CMD2_DATA_SRC_DMA                0x00400000
#define SPINAND_CUSTOM_CMD2_DATA_DUMMY_ONE_CYCLE        0x00010000
#define SPINAND_CUSTOM_CMD2_ADDR_SRC_CYCLE              0x00001000
#define SPINAND_CUSTOM_CMD2_ADDR_SRC_BY_ROW_COLUMN      0x00000400
#define SPINAND_CUSTOM_CMD2_RE_TYPE                     0x00000100

#define FIO_CTRL2_SLECET_SPINAD         0x00000002
#define AMBA_SPINAND_CMD_READ_ARRAY     0x00000013
#define AMBA_SPINAND_CMD_READ           0x00030000

#define AMBA_SPINAND_ERROR_PATTERN      0x0

/* Flash I/O Subsystem Controller Registers */
#define FIO_CTRL_REG                    0x00
#define FIO_RAW_STATUS_REG              0x04
#define FIO_CTRL2_REG                   0x08
#define FIO_INT_ENABLE_REG              0x0C
#define FIO_INT_STATUS_REG              0x10

#define FIO_ECC_REPORT_CFG_REG          0xA0
#define FIO_ECC_STATUS_REG              0xA4
#define FIO_ECC_STATUS2_REG             0xA8

/* Flash Controller Registers - NAND Flash Mode */
#define NAND_CTRL_REG                   0x120
#define NAND_CMD_REG                    0x124
#define NAND_TIMING0_REG                0x128
#define NAND_TIMING1_REG                0x12c
#define NAND_TIMING2_REG                0x130
#define NAND_TIMING3_REG                0x134
#define NAND_TIMING4_REG                0x138
#define NAND_TIMING5_REG                0x13c
#define NAND_STATUS_REG                 0x140
#define NAND_ID_REG                     0x144
#define NAND_COPY_DEST_ADDR_REG         0x148
#define NAND_DEBUG0_REG                 0x14c
#define NAND_DEBUG1_REG                 0x150
#define NAND_EXT_CTRL_REG               0x15c
#define NAND_EXT_ID_REG                 0x160
#define NAND_TIMING6_REG                0x164
#define NAND_CUSTOM_CMDWORD_REG         0x174
#define SPINAND_CTRL_REG                0x1a0
#define SPINAND_CUSTOM_CMD1_REG         0x1a4
#define SPINAND_CUSTOM_CMD2_REG         0x1a8
#define SPINAND_DONE_PATTERN_REG        0x1ac
#define SPINAND_ERROR_PATTERN_REG       0x1b0
#define SPINAND_TIMING0_REG             0x1c0
#define SPINAND_TIMING1_REG             0x1c4
#define SPINAND_TIMING2_REG             0x1c8

/* Flash Controller Registers - FDMA CONTROLLER for spare area */
#define FDMA_SPARE_MEM_ADDR_REG         0x204
#define FDMA_SPARE_DESC_ADDR_REG        0x280

/* Flash Controller Registers - FDMA CONTROLLER for main area */
#define FDMA_MAIN_CTRL_REG              0x300
#define FDMA_MAIN_MEM_ADDR_REG          0x304
#define FDMA_MAIN_STATUS_REG            0x30c
#define FDMA_MAIN_DESC_ADDR_REG         0x380
#define FDMA_DSM_CTRL_REG               0x3a0

/**
 * FIO: flash I/O subsystem
 */
/* FIO CTRL REG */
#define FIO_CTRL_RANDOM_READ            0x00000002
#define FIO_CTRL_STOP_ON_ERROR          0x00000008
#define FIO_CTRL_ECC_BCH8               0x00000020
#define FIO_CTRL_ECC_BCH_ENABLE         0x00000040
#define FIO_CTRL_SKIP_BLANK_ECC         0x00000080


/* FIO DMA CTRL REG */
#define FIO_DMA_CTRL_ENABLE             0x80000000
#define FIO_DMA_CTRL_READ_MEM           0x40000000
#define FIO_DMA_CTRL_BUS_BLK_SIZE_512   0x06000000
#define FIO_DMA_CTRL_BUS_BLK_SIZE_32    0x02000000
#define FIO_DMA_CTRL_BUS_DATA_SIZE_8    0x00c00000
#define FIO_DMA_CTRL_BUS_DATA_SIZE_4    0x00800000

/* FIO RAW STATUS REG */
#define FIO_STATUS_CMD_DONE             0x00000001
#define FIO_STATUS_BCH_ERROR            0x00000010
#define FIO_STATUS_BCH_UNCORRECTABLE    0x00000020

/* NAND CTRL REG */
#define NAND_CTRL_SPARE_ADDR            0x08000000
#define NAND_CTRL_PAGE_ADDR_CYCLE_3     0x01000000
#define NAND_CTRL_READ_ID_CYCLE_4       0x00800000
#define NAND_CTRL_COPY_BACK_ENABLE      0x00100000
#define NAND_CTRL_ECC_CHK_MAIN_ENABLE   0x00080000
#define NAND_CTRL_ECC_CHK_SPARE_ENABLE  0x00040000
#define NAND_CTRL_ECC_GEN_MAIN_ENABLE   0x00020000
#define NAND_CTRL_ECC_GEN_SPARE_ENABLE  0x00010000
#define NAND_CTRL_CHIP_SIZE_8G          0x00000070

/* NAND IRQ STATUS REG */
#define NAND_IRQ_STATUS_CMD_DONE        0x1

/**
 * FDMA: DMA Control Registers for flash I/O subsystem
 */

/* FDMA DUAL_SPACE_MODE REG */
#define FDMA_DSM_SPARE_STRIDE_SIZE_16B  0x00000004
#define FDMA_DSM_SPARE_STRIDE_SIZE_32B  0x00000005
#define FDMA_DSM_SPARE_STRIDE_SIZE_64B  0x00000006
#define FDMA_DSM_SPARE_STRIDE_SIZE_128B 0x00000007
#define FDMA_DSM_SPARE_STRIDE_SIZE_256B 0x00000008
#define FDMA_DSM_MAIN_STRIDE_SIZE_512B  0x00000090
#define FDMA_DSM_MAIN_STRIDE_SIZE_2KB   0x000000B0
#define FDMA_DSM_MAIN_STRIDE_SIZE_4KB   0x000000C0

/* FDMA CTRL REG */
#define FDMA_CTRL_ENABLE                0x80000000
#define FDMA_CTRL_DESC_MODE             0x40000000
#define FDMA_CTRL_WRITE_MEM             0x20000000
#define FDMA_CTRL_READ_MEM              0x10000000
#define FDMA_CTRL_NO_ADDR_INC           0x08000000
#define FDMA_CTRL_BUS_BLK_SIZE_8        0x00000000  /* Bus transaction block size = 8 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_16       0x01000000  /* Bus transaction block size = 16 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_32       0x02000000  /* Bus transaction block size = 32 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_64       0x03000000  /* Bus transaction block size = 64 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_128      0x04000000  /* Bus transaction block size = 128 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_256      0x05000000  /* Bus transaction block size = 256 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_512      0x06000000  /* Bus transaction block size = 512 bytes */
#define FDMA_CTRL_BUS_BLK_SIZE_1024     0x07000000  /* Bus transaction block size = 1024 bytes */
#define FDMA_CTRL_BUS_DATA_SIZE_1       0x00000000  /* Bus data transfer size = 1 bytes */
#define FDMA_CTRL_BUS_DATA_SIZE_2       0x00400000  /* Bus data transfer size = 2 bytes */
#define FDMA_CTRL_BUS_DATA_SIZE_4       0x00800000  /* Bus data transfer size = 4 bytes */
#define FDMA_CTRL_BUS_DATA_SIZE_8       0x00c00000  /* Bus data transfer size = 8 bytes */

/* FDMA STATUS REG */
#define FDMA_STATUS_DMA_DONE            0x00400000

#define NAND_EXT_CTRL_PAGE_4K_ENABLE    0x02000000
#define NAND_EXT_CTRL_SPARE_SIZE_2X     0x00000001

#define NAND_CMD_READ                   (14)

#endif  /* AMBA_DRAM_TRAIN_SPINAND_H */
