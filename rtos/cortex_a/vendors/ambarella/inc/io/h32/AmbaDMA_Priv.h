/**
 *  @file AmbaDMA_Priv.h
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
 *  @details Definitions & Constants for DMA APIs.
 *
 */

#ifndef AMBA_DMA_PRIV_H
#define AMBA_DMA_PRIV_H

/*
 * Number of DMA channels.
 */
#define AMBA_DMA_CHANNEL_0              (0U)   /* DMA channel 0 */
#define AMBA_DMA_CHANNEL_1              (1U)   /* DMA channel 1 */
#define AMBA_DMA_CHANNEL_2              (2U)   /* DMA channel 2 */
#define AMBA_DMA_CHANNEL_3              (3U)   /* DMA channel 3 */
#define AMBA_DMA_CHANNEL_4              (4U)   /* DMA channel 4 */
#define AMBA_DMA_CHANNEL_5              (5U)   /* DMA channel 5 */
#define AMBA_DMA_CHANNEL_6              (6U)   /* DMA channel 6 */
#define AMBA_DMA_CHANNEL_7              (7U)   /* DMA channel 7 */
#define AMBA_DMA_CHANNEL_8              (8U)   /* DMA channel 8 */
#define AMBA_DMA_CHANNEL_9              (9U)   /* DMA channel 9 */
#define AMBA_DMA_CHANNEL_10             (10U)  /* DMA channel 10 */
#define AMBA_DMA_CHANNEL_11             (11U)  /* DMA channel 11 */
#define AMBA_DMA_CHANNEL_12             (12U)  /* DMA channel 12 */
#define AMBA_DMA_CHANNEL_13             (13U)  /* DMA channel 13 */
#define AMBA_DMA_CHANNEL_14             (14U)  /* DMA channel 14 */
#define AMBA_DMA_CHANNEL_15             (15U)  /* DMA channel 15 */
#define AMBA_NUM_DMA_CHANNEL            (16U)

#define AMBA_NUM_DMA_MODULE             (2U)

#define AMBA_DMA_CHANNEL_MEM_TRX        (0U)  /* DMA channel for DRAM-to-DRAM */
#define AMBA_DMA_CHANNEL_SPI0_TX        (1U)  /* DMA channel for SPI Master0 TX */
#define AMBA_DMA_CHANNEL_SPI0_RX        (2U)  /* DMA channel for SPI Master0 RX */
#define AMBA_DMA_CHANNEL_SPI1_TX        (3U)  /* DMA channel for SPI Master1 TX */
#define AMBA_DMA_CHANNEL_SPI1_RX        (4U)  /* DMA channel for SPI Master1 RX */
#define AMBA_DMA_CHANNEL_NOR_SPI_TX     (5U)  /* DMA channel for NOR-SPI TX */
#define AMBA_DMA_CHANNEL_NOR_SPI_RX     (6U)  /* DMA channel for NOR-SPI RX */
#define AMBA_DMA_CHANNEL_SPI_SLAVE_TX   (7U)  /* DMA channel for SPI Slave TX */
#define AMBA_DMA_CHANNEL_SPI_SLAVE_RX   (8U)  /* DMA channel for SPI Slave RX */
#define AMBA_DMA_CHANNEL_UART0_TX       (9U)  /* DMA channel for UART0 TX */
#define AMBA_DMA_CHANNEL_UART0_RX       (10U) /* DMA channel for UART0 RX */
#define AMBA_DMA_CHANNEL_UART1_TX       (11U) /* DMA channel for UART1 TX */
#define AMBA_DMA_CHANNEL_UART1_RX       (12U) /* DMA channel for UART1 RX */
#define AMBA_DMA_CHANNEL_UART2_TX       (13U) /* DMA channel for UART2 TX */
#define AMBA_DMA_CHANNEL_UART2_RX       (14U) /* DMA channel for UART2 RX */
#define AMBA_DMA_CHANNEL_UART3_TX       (15U) /* DMA channel for UART3 TX */
#define AMBA_DMA_CHANNEL_UART3_RX       (16U) /* DMA channel for UART3 RX */
#define AMBA_DMA_CHANNEL_SPI2_TX        (17U) /* DMA channel for SPI Master2 TX */
#define AMBA_DMA_CHANNEL_SPI2_RX        (18U) /* DMA channel for SPI Master2 RX */
#define AMBA_DMA_CHANNEL_SPI3_TX        (19U) /* DMA channel for SPI Master3 TX */
#define AMBA_DMA_CHANNEL_SPI3_RX        (20U) /* DMA channel for SPI Master3 RX */
#define AMBA_DMA_CHANNEL_I2S_TX         (21U) /* DMA channel for I2S0 TX */
#define AMBA_DMA_CHANNEL_I2S_RX         (22U) /* DMA channel for I2S0 RX */
#define AMBA_NUM_DMA_CHANNEL_TYPE       (23U)

#endif /* AMBA_DMA_PRIV_H */
