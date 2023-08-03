/**
 *  @file AmbaDramTrain_SpiNOR.h
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
 *  @details Definitions & Constants for LPDDR4 DRAM Training with SPI-NOR
 *
 */

#ifndef AMBA_DRAM_TRAIN_SPINOR_H
#define AMBA_DRAM_TRAIN_SPINOR_H

#include "AmbaDramTrain.h"

#define SPI_NOR_DEVICE_OFFSET       0x31000
#define SPI_NOR_LENGTH_OFFSET       0x00
#define SPI_NOR_CTRL_OFFSET         0x04
#define SPI_NOR_CFG_OFFSET          0x08
#define SPI_NOR_CMD_OFFSET          0x0c
#define SPI_NOR_ADDRHI_OFFSET       0x10
#define SPI_NOR_ADDRLO_OFFSET       0x14
#define SPI_NOR_DMACTRL_OFFSET      0x18
#define SPI_NOR_TXFIFOTHLV_OFFSET   0x1c
#define SPI_NOR_RXFIFOTHLV_OFFSET   0x20
#define SPI_NOR_TXFIFOLV_OFFSET     0x24
#define SPI_NOR_RXFIFOLV_OFFSET     0x28
#define SPI_NOR_FIFOSTA_OFFSET      0x2c
#define SPI_NOR_INTRMASK_OFFSET     0x30
#define SPI_NOR_INTR_OFFSET         0x34
#define SPI_NOR_RAWINTR_OFFSET      0x38
#define SPI_NOR_CLRINTR_OFFSET      0x3c
#define SPI_NOR_TXFIFORST_OFFSET    0x40
#define SPI_NOR_RXFIFORST_OFFSET    0x44
#define SPI_NOR_START_OFFSET        0x50
#define SPI_NOR_TXDATA_OFFSET       0x100
#define SPI_NOR_RXDATA_OFFSET       0x200

/* DMA Channel AHB_SCRATCHPAD */
#define AMBA_DMA_CHANNEL_SLELECT_OFFSET 0x30
#define AMBA_DMA_CHANNEL_NOR_SPI_RX     0x6
#define DMA_RX_OFFSET                   0x300
#define DMA_CTRL_OFFSET                 0
#define DMA_SRCADDR_OFFSET              0x4
#define DMA_DESADDR_OFFSET              0x8
#define DMA_STATUS_OFFSET               0xC
#define DMA_INT_OFFSET                  0xf0

#define SPI_NOR_DATA_TRASDONE       0x20
#define SPI_NOR_RX_DMA_CHAN         0
#define SPI_NOR_DMA_BUF_SIZE        4096

#endif  /* AMBA_DRAM_TRAIN_SPINOR_H */
