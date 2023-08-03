/**
 *  @file AmbaClkMon_Priv.h
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
 *  @details Definitions & Constants for Clock Monitor APIs
 *
 */

#ifndef AMBA_CLK_MON_PRIV_H
#define AMBA_CLK_MON_PRIV_H

// Clock Monitors inside DRAM
#define AMBA_CKM_IDX_DDRH0_DRAM_DIV2  (1U) //!< CEHU ID 138
#define AMBA_CKM_IDX_DDRH0_DRAM       (2U) //!< CEHU ID 139
#define AMBA_CKM_IDX_DDRHE_DRAM_DIV2  (3U) //!< CEHU ID 136
#define AMBA_CKM_IDX_DDRHE_DRAM       (4U) //!< CEHU ID 137
// Clock Monitors inside RCT DEBUG
#define AMBA_CKM_IDX_REF_CLK          (5U) //!< CEHU ID 132
#define AMBA_CKM_IDX_OSC              (6U) //!< CEHU ID 29
// Clock Monitors inside AXI
#define AMBA_CKM_IDX_CORTEX0          (7U) //!< CEHU ID 56
#define AMBA_CKM_IDX_CORTEX1          (8U) //!< CEHU ID 57
#define AMBA_CKM_IDX_AXI1             (9U) //!< CEHU ID 53
#define AMBA_CKM_IDX_N0_PCLK_DBG      (10U) //!< CEHU ID 54
#define AMBA_CKM_IDX_N1_PCLK_DBG      (11U) //!< CEHU ID 55
// Clock Monitors inside Scratchpad NS
#define AMBA_CKM_IDX_SSI2_DFT         (12U) //!< CEHU ID 59
#define AMBA_CKM_IDX_SSI3_DFT         (13U) //!< CEHU ID 60
#define AMBA_CKM_IDX_CORE_AHB         (14U) //!< CEHU ID 66
#define AMBA_CKM_IDX_CORE_DFT         (15U) //!< CEHU ID 67
#define AMBA_CKM_IDX_AHB_DFT          (16U) //!< CEHU ID 68
#define AMBA_CKM_IDX_APB_DFT          (17U) //!< CEHU ID 69
#define AMBA_CKM_IDX_DBG_DFT          (18U) //!< CEHU ID 70
#define AMBA_CKM_IDX_SDIO_DFT         (19U) //!< CEHU ID 71
#define AMBA_CKM_IDX_SD48_DFT         (20U) //!< CEHU ID 72
#define AMBA_CKM_IDX_NAND_DFT         (21U) //!< CEHU ID 73
#define AMBA_CKM_IDX_N0_SYSCNT_DFT    (22U) //!< CEHU ID 74
#define AMBA_CKM_IDX_N1_SYSCNT_DFT    (23U) //!< CEHU ID 75
#define AMBA_CKM_IDX_CAN_DFT          (24U) //!< CEHU ID 77
#define AMBA_CKM_IDX_AU_DFT           (25U) //!< CEHU ID 78
#define AMBA_CKM_IDX_GTX_DFT          (26U) //!< CEHU ID 79
#define AMBA_CKM_IDX_AXI1_DFT         (27U) //!< CEHU ID 80
#define AMBA_CKM_IDX_VISION_DFT       (28U) //!< CEHU ID 83
#define AMBA_CKM_IDX_DRAM_DIV2_DFT    (29U) //!< CEHU ID 84
#define AMBA_CKM_IDX_FEX_DFT          (30U) //!< CEHU ID 87
#define AMBA_CKM_IDX_AMBA1_DBG2DBG    (31U) //!< CEHU ID 88
#define AMBA_CKM_IDX_AMBA1_CORE_AHB   (32U) //!< CEHU ID 89
#define AMBA_CKM_IDX_AMBA1_CORE_DFT   (33U) //!< CEHU ID 90
#define AMBA_CKM_IDX_AMBA1_AHB_DFT    (34U) //!< CEHU ID 91
#define AMBA_CKM_IDX_AMBA1_AXI1_DFT   (35U) //!< CEHU ID 92
// Clock Monitors inside VP/DSP
#define AMBA_CKM_IDX_SMEM_CORE        (36U) //!< CEHU ID 122
#define AMBA_CKM_IDX_EORC_CORE        (37U) //!< CEHU ID 120
#define AMBA_CKM_IDX_EORC_DBG         (38U) //!< CEHU ID 121
#define AMBA_CKM_IDX_CORC_CORE        (39U) //!< CEHU ID 118
#define AMBA_CKM_IDX_CORC_DBG         (40U) //!< CEHU ID 119
#define AMBA_CKM_IDX_VDSP_CORE        (41U) //!< CEHU ID 123
#define AMBA_CKM_IDX_VDSP_DBG         (42U) //!< CEHU ID 124
#define AMBA_CKM_IDX_IDSP0_IDSP       (43U) //!< CEHU ID 95
#define AMBA_CKM_IDX_IDSP0_CORE       (44U) //!< CEHU ID 94
#define AMBA_CKM_IDX_IDSP0_DBG        (45U) //!< CEHU ID 93
#define AMBA_CKM_IDX_IDSP1_IDSP       (46U) //!< CEHU ID 98
#define AMBA_CKM_IDX_IDSP1_CORE       (47U) //!< CEHU ID 97
#define AMBA_CKM_IDX_IDSP1_DBG        (48U) //!< CEHU ID 96
#define AMBA_CKM_IDX_STORC_VORC       (49U) //!< CEHU ID 134
#define AMBA_CKM_IDX_STORC_L2C        (50U) //!< CEHU ID 133
#define AMBA_CKM_IDX_FEX_FEX          (51U) //!< CEHU ID 135
#define AMBA_CKM_IDX_VMEM_VISION      (52U) //!< CEHU ID 125
#define AMBA_CKM_IDX_VPA_VISION       (53U) //!< CEHU ID 126
#define AMBA_CKM_IDX_VPA_DBG          (54U) //!< CEHU ID 131
#define AMBA_CKM_IDX_VPB_VISION       (55U) //!< CEHU ID 127
#define AMBA_CKM_IDX_VPC_VISION       (56U) //!< CEHU ID 128
#define AMBA_CKM_IDX_VPD_VISION       (57U) //!< CEHU ID 129
#define AMBA_CKM_IDX_VPF_VISION       (58U) //!< CEHU ID 130
// Clock Monitors inside RCT
#define AMBA_CKM_IDX_PLLOUT_CORE      (59U) //!< CEHU ID 110
#define AMBA_CKM_IDX_PLLOUT_SD        (60U) //!< CEHU ID 103
#define AMBA_CKM_IDX_PLLOUT_ENET      (61U) //!< CEHU ID 106
#define AMBA_CKM_IDX_PLLOUT_AUDIO     (62U) //!< CEHU ID 112
#define AMBA_CKM_IDX_PLLOUT_DDR       (63U) //!< CEHU ID 107
#define AMBA_CKM_IDX_PLLOUT_CORTEX0   (64U) //!< CEHU ID 109
#define AMBA_CKM_IDX_PLLOUT_CORTEX1   (65U) //!< CEHU ID 108
#define AMBA_CKM_IDX_PLLOUT_FEX       (66U) //!< CEHU ID 116
#define AMBA_CKM_IDX_PLLOUT_VISION    (67U) //!< CEHU ID 113
#define AMBA_CKM_IDX_PLLOUT_NAND      (68U) //!< CEHU ID 114
#define AMBA_CKM_IDX_PLLOUT_IDSP      (69U) //!< CEHU ID 115
#define AMBA_CKM_IDX_PLLOUT_DDR_INIT  (70U) //!< CEHU ID 111
#define AMBA_CKM_IDX_GCLK_SO_VIN0     (71U) //!< CEHU ID 105
#define AMBA_CKM_IDX_GCLK_SO_VIN1     (72U) //!< CEHU ID 104
#define AMBA_CKM_IDX_GCLK_DBG_OUT     (73U) //!< CEHU ID 117
#define AMBA_CKM_IDX_GCLK_SO          (74U) //!< CEHU ID 101
#define AMBA_CKM_IDX_GCLK_SO2         (75U) //!< CEHU ID 102
// Clock Monitors inside Scratchpad NS
#define AMBA_CKM_IDX_SSI_DFT          (76U) //!< CEHU ID 58
#define AMBA_CKM_IDX_UART0_DFT        (77U) //!< CEHU ID 61
#define AMBA_CKM_IDX_UART1_DFT        (78U) //!< CEHU ID 62
#define AMBA_CKM_IDX_UART2_DFT        (79U) //!< CEHU ID 63
#define AMBA_CKM_IDX_UART3_DFT        (80U) //!< CEHU ID 64
#define AMBA_CKM_IDX_UART_APB_DFT     (81U) //!< CEHU ID 65
#define AMBA_CKM_IDX_PWM_DFT          (82U) //!< CEHU ID 76
#define AMBA_CKM_IDX_VO_A_DFT         (83U) //!< CEHU ID 81
#define AMBA_CKM_IDX_VO_B_DFT         (84U) //!< CEHU ID 82
#define AMBA_CKM_IDX_CLKA_CBPHY_DFT   (85U) //!< CEHU ID 85
#define AMBA_CKM_IDX_CLKB_CBPHY_DFT   (86U) //!< CEHU ID 86
// Clock Monitors inside RCT
#define AMBA_CKM_IDX_PLLOUT_VIDEO_A   (87U) //!< CEHU ID 100
#define AMBA_CKM_IDX_PLLOUT_VIDEO_B   (88U) //!< CEHU ID 99
#define AMBA_CKM_IDX_MAX              (89U) //!< Maximum

#endif  /* AMBA_CLK_MON_PRIV_H */
