/**
 *  @file AmbaBST_SpiNAND.h
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
 *  @details Definitions & Constants for Bootstrap NAND portion
 *
 */

#ifndef AMBA_BST_SPINAND_H
#define AMBA_BST_SPINAND_H

/**
 * FIO: flash I/O subsystem
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

#endif  /* AMBA_BST_SPINAND_H */
