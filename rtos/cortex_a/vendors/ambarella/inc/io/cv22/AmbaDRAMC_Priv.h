/**
 *  @file AmbaDRAMC_Priv.h
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
 *  @details Definitions & Constants for DRAMC APIs
 *
 */

#ifndef AMBA_DRAMC_PRIV_H
#define AMBA_DRAMC_PRIV_H

/**
 * DRAM clinet id
 */
#define DRAM_CLIENT_00              (0U)
#define DRAM_CLIENT_01              (1U)
#define DRAM_CLIENT_02              (2U)
#define DRAM_CLIENT_03              (3U)
#define DRAM_CLIENT_04              (4U)
#define DRAM_CLIENT_05              (5U)
#define DRAM_CLIENT_06              (6U)
#define DRAM_CLIENT_07              (7U)
#define DRAM_CLIENT_08              (8U)
#define DRAM_CLIENT_09              (9U)
#define DRAM_CLIENT_10              (10U)
#define DRAM_CLIENT_11              (11U)
#define DRAM_CLIENT_12              (12U)
#define DRAM_CLIENT_13              (13U)
#define DRAM_CLIENT_14              (14U)
#define DRAM_CLIENT_15              (15U)
#define DRAM_CLIENT_16              (16U)
#define NUM_DRAM_CLIENT             (17U)

#define DRAM_CLIENT_CA53            DRAM_CLIENT_00
#define DRAM_CLIENT_DMA0            DRAM_CLIENT_01
#define DRAM_CLIENT_DMA1            DRAM_CLIENT_02
#define DRAM_CLIENT_ENET            DRAM_CLIENT_03
#define DRAM_CLIENT_FDMA            DRAM_CLIENT_04
#define DRAM_CLIENT_CAN0            DRAM_CLIENT_05
#define DRAM_CLIENT_GDMA            DRAM_CLIENT_06
#define DRAM_CLIENT_SD0             DRAM_CLIENT_07
#define DRAM_CLIENT_SD1             DRAM_CLIENT_08
#define DRAM_CLIENT_USB_DEVICE      DRAM_CLIENT_09
#define DRAM_CLIENT_USB_HOST        DRAM_CLIENT_10
#define DRAM_CLIENT_ORCME           DRAM_CLIENT_11
#define DRAM_CLIENT_ORCCODE         DRAM_CLIENT_12
#define DRAM_CLIENT_ORCVP           DRAM_CLIENT_13
#define DRAM_CLIENT_ORCL2           DRAM_CLIENT_14
#define DRAM_CLIENT_SMEM            DRAM_CLIENT_15
#define DRAM_CLIENT_VMEM            DRAM_CLIENT_16

/**
 * DRAM clinet id, these definitions are temporarily used to compatible with some SDK7 code
 */
#define AMBA_DRAM_CLIENT_AXI            DRAM_CLIENT_CA53
#define AMBA_DRAM_CLIENT_DMA0           DRAM_CLIENT_DMA0
#define AMBA_DRAM_CLIENT_DMA1           DRAM_CLIENT_DMA1
#define AMBA_DRAM_CLIENT_ENET           DRAM_CLIENT_ENET
#define AMBA_DRAM_CLIENT_FDMA           DRAM_CLIENT_FDMA
#define AMBA_DRAM_CLIENT_CAN0           DRAM_CLIENT_CAN0
#define AMBA_DRAM_CLIENT_GDMA           DRAM_CLIENT_GDMA
#define AMBA_DRAM_CLIENT_SDXC0          DRAM_CLIENT_SD0
#define AMBA_DRAM_CLIENT_SDXC1          DRAM_CLIENT_SD1
#define AMBA_DRAM_CLIENT_USB_DEVICE     DRAM_CLIENT_USB_DEVICE
#define AMBA_DRAM_CLIENT_USB_HOST       DRAM_CLIENT_USB_HOST
#define AMBA_DRAM_CLIENT_ORCME          DRAM_CLIENT_ORCME
#define AMBA_DRAM_CLIENT_ORCCODE        DRAM_CLIENT_ORCCODE
#define AMBA_DRAM_CLIENT_ORCVP          DRAM_CLIENT_ORCVP
#define AMBA_DRAM_CLIENT_ORCL2          DRAM_CLIENT_ORCL2
#define AMBA_DRAM_CLIENT_SMEM           DRAM_CLIENT_SMEM
#define AMBA_DRAM_CLIENT_VMEM           DRAM_CLIENT_VMEM
#define AMBA_NUM_DRAM_CLIENT            NUM_DRAM_CLIENT

#endif /* AMBA_DRAMC_PRIV_H */
