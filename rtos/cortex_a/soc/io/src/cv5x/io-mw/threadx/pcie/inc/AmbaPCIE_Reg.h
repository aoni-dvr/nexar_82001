/**
 *  @file AmbaPCIE_Reg.h
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
 *  @details PCIe register header file.
 */

#ifndef AMBAPCIE_REG_H_
#define AMBAPCIE_REG_H_

#define RP_LANE_EQ_CTRL0_DNTP0_MASK           0x0000000FU
#define RP_LANE_EQ_CTRL0_DNTP0_SHIFT                   0U
#define RP_LANE_EQ_CTRL0_DNRPH0_MASK          0x00000070U
#define RP_LANE_EQ_CTRL0_DNRPH0_SHIFT                  4U
#define RP_LANE_EQ_CTRL0_UPTP0_MASK           0x00000F00U
#define RP_LANE_EQ_CTRL0_UPTP0_SHIFT                   8U
#define RP_LANE_EQ_CTRL0_UPRPH0_MASK          0x00007000U
#define RP_LANE_EQ_CTRL0_UPRPH0_SHIFT                 12U

#define RP_LANE_EQ_CTRL0_DNTP1_MASK           0x000F0000U
#define RP_LANE_EQ_CTRL0_DNTP1_SHIFT                  16U
#define RP_LANE_EQ_CTRL0_DNRPH1_MASK          0x00700000U
#define RP_LANE_EQ_CTRL0_DNRPH1_SHIFT                 20U
#define RP_LANE_EQ_CTRL0_UPTP1_MASK           0x0F000000U
#define RP_LANE_EQ_CTRL0_UPTP1_SHIFT                  24U
#define RP_LANE_EQ_CTRL0_UPRPH1_MASK          0x70000000U
#define RP_LANE_EQ_CTRL0_UPRPH1_SHIFT                 28U

#define RP_LANE_EQ_CTRL1_DNTP0_MASK           0x0000000FU
#define RP_LANE_EQ_CTRL1_DNTP0_SHIFT                   0U
#define RP_LANE_EQ_CTRL1_DNRPH0_MASK          0x00000070U
#define RP_LANE_EQ_CTRL1_DNRPH0_SHIFT                  4U
#define RP_LANE_EQ_CTRL1_UPTP0_MASK           0x00000F00U
#define RP_LANE_EQ_CTRL1_UPTP0_SHIFT                   8U
#define RP_LANE_EQ_CTRL1_UPRPH0_MASK          0x00007000U
#define RP_LANE_EQ_CTRL1_UPRPH0_SHIFT                 12U

#define RP_LANE_EQ_CTRL1_DNTP1_MASK           0x000F0000U
#define RP_LANE_EQ_CTRL1_DNTP1_SHIFT                  16U
#define RP_LANE_EQ_CTRL1_DNRPH1_MASK          0x00700000U
#define RP_LANE_EQ_CTRL1_DNRPH1_SHIFT                 20U
#define RP_LANE_EQ_CTRL1_UPTP1_MASK           0x0F000000U
#define RP_LANE_EQ_CTRL1_UPTP1_SHIFT                  24U
#define RP_LANE_EQ_CTRL1_UPRPH1_MASK          0x70000000U
#define RP_LANE_EQ_CTRL1_UPRPH1_SHIFT                 28U

#define LM_LD_CTRL_AUTO_EN_MASK               0x01000000U
#define LM_LD_CTRL_AUTO_EN_SHIFT                      24U

#define LM_RX_FTS_COUNT_RFC5S_MASK            0x000000FFU
#define LM_RX_FTS_COUNT_RFC5S_SHIFT                    0U
#define LM_RX_FTS_COUNT_RFC8S_MASK            0x0000FF00U
#define LM_RX_FTS_COUNT_RFC8S_SHIFT                    8U

#define LM_TX_TLP_COUNT_TTC_MASK              0xFFFFFFFFU
#define LM_TX_TLP_COUNT_TTC_SHIFT                      0U
#define LM_TX_TLP_PLD_DW_COUNT_TTPBC_MASK     0xFFFFFFFFU
#define LM_TX_TLP_PLD_DW_COUNT_TTPBC_SHIFT             0U

#define LM_RX_TLP_COUNT_RTC_MASK              0xFFFFFFFFU
#define LM_RX_TLP_COUNT_RTC_SHIFT                      0U
#define LM_RX_TLP_PLD_DW_COUNT_RTPDC_MASK     0xFFFFFFFFU
#define LM_RX_TLP_PLD_DW_COUNT_RTPDC_SHIFT             0U

#define LM_TX_CRED_UPDATE_CFG0_MPUI_MASK      0x0000FFFFU
#define LM_TX_CRED_UPDATE_CFG0_MPUI_SHIFT              0U
#define LM_TX_CRED_UPDATE_CFG0_MPUI_WIDTH             16U
#define LM_TX_CRED_UPDATE_CFG0_MNUI_MASK      0xFFFF0000U
#define LM_TX_CRED_UPDATE_CFG0_MNUI_SHIFT             16U
#define LM_TX_CRED_UPDATE_CFG0_MNUI_WIDTH             16U
#define LM_TX_CRED_UPDATE_CFG1_CUI_MASK       0x0000FFFFU
#define LM_TX_CRED_UPDATE_CFG1_CUI_SHIFT               0U
#define LM_TX_CRED_UPDATE_CFG1_CUI_WIDTH              16U
#define LM_TX_CRED_UPDATE_CFG1_MUI_MASK       0xFFFF0000U
#define LM_TX_CRED_UPDATE_CFG1_MUI_SHIFT              16U
#define LM_TX_CRED_UPDATE_CFG1_MUI_WIDTH              16U

#define LM_VID_MASK                           0x0000FFFFU
#define LM_VID_SHIFT                                   0U
#define LM_VID_WIDTH                                  16U
#define LM_SVID_MASK                          0xFFFF0000U
#define LM_SVID_SHIFT                                 16U
#define LM_SVID_WIDTH                                 16U

#define LM_L0S_TO_LT_MASK                     0x0000FFFFU
#define LM_L0S_TO_LT_SHIFT                             0U
#define LM_L0S_TO_LT_WIDTH                            16U
#define LM_COMPL_TO0_CTL_MASK                 0x00FFFFFFU
#define LM_COMPL_TO0_CTL_SHIFT                         0U
#define LM_COMPL_TO0_CTL_WIDTH                        24U
#define LM_COMPL_TO1_CTL_MASK                 0x00FFFFFFU
#define LM_COMPL_TO1_CTL_SHIFT                         0U
#define LM_COMPL_TO1_CTL_WIDTH                        28U

#define LM_L1_REENTRY_DELAY_L1RD_MASK         0xFFFFFFFFU
#define LM_L1_REENTRY_DELAY_L1RD_SHIFT                 0U
#define LM_ASPM_L1_ENTRY_TO_L1T_MASK          0x000FFFFFU
#define LM_ASPM_L1_ENTRY_TO_L1T_SHIFT                  0U
#define LM_ASPM_L1_ENTRY_TO_L1T_WIDTH                 20U
#define LM_PME_TURNOFF_ACK_PTOAD_MASK         0x0000FFFFU
#define LM_PME_TURNOFF_ACK_PTOAD_SHIFT                 0U
#define LM_PME_TURNOFF_ACK_PTOAD_WIDTH                16U
#define LM_DLL_TMR_CONFIG_TSRT_MASK           0x000001FFU
#define LM_DLL_TMR_CONFIG_TSRT_SHIFT                   0U
#define LM_DLL_TMR_CONFIG_TSRT_WIDTH                   9U
#define LM_DLL_TMR_CONFIG_RSART_MASK          0x01FF0000U
#define LM_DLL_TMR_CONFIG_RSART_SHIFT                 16U
#define LM_DLL_TMR_CONFIG_RSART_WIDTH                  9U

#define LM_LTR_MSG_GEN_CTL_TMLMET_MASK        0x00000800U
#define LM_LTR_MSG_GEN_CTL_TMLMET_SHIFT               11U
#define LM_LTR_MSG_GEN_CTL_TMFPSC_MASK        0x00001000U
#define LM_LTR_MSG_GEN_CTL_TMFPSC_SHIFT               12U

#define LM_LD_CTRL_LDTIMER_MASK               0x00FFFFFFU
#define LM_LD_CTRL_LDTIMER_SHIFT                       0U
#define LM_LD_CTRL_LDTIMER_WIDTH                      24U

#define LM_TRANSM_CRED_LIM0_PPC_MASK          0x00000FFFU
#define LM_TRANSM_CRED_LIM0_PPC_SHIFT                  0U
#define LM_TRANSM_CRED_LIM0_PHC_MASK          0x000FF000U
#define LM_TRANSM_CRED_LIM0_PHC_SHIFT                 12U
#define LM_TRANSM_CRED_LIM0_NPPC_MASK         0xFFF00000U
#define LM_TRANSM_CRED_LIM0_NPPC_SHIFT                20U
#define LM_TRANSM_CRED_LIM1_NPHC_MASK         0x000000FFU
#define LM_TRANSM_CRED_LIM1_NPHC_SHIFT                 0U
#define LM_TRANSM_CRED_LIM1_CPC_MASK          0x000FFF00U
#define LM_TRANSM_CRED_LIM1_CPC_SHIFT                  8U
#define LM_TRANSM_CRED_LIM1_CHC_MASK          0xFF000000U
#define LM_TRANSM_CRED_LIM1_CHC_SHIFT                 24U

#define LM_RCV_CRED_LIM0_PPC_MASK             0x00000FFFU
#define LM_RCV_CRED_LIM0_PPC_SHIFT                     0U
#define LM_RCV_CRED_LIM0_PPC_WIDTH                    12U
#define LM_RCV_CRED_LIM0_PHC_MASK             0x000FF000U
#define LM_RCV_CRED_LIM0_PHC_SHIFT                    12U
#define LM_RCV_CRED_LIM0_PHC_WIDTH                     8U
#define LM_RCV_CRED_LIM0_NPPC_MASK            0xFFF00000U
#define LM_RCV_CRED_LIM0_NPPC_SHIFT                   20U
#define LM_RCV_CRED_LIM0_NPPC_WIDTH                   12U

#define LM_RCV_CRED_LIM1_NPHCL_MASK           0x000000FFU
#define LM_RCV_CRED_LIM1_NPHCL_SHIFT                   0U
#define LM_RCV_CRED_LIM1_NPHCL_WIDTH                   8U
#define LM_RCV_CRED_LIM1_CPC_MASK             0x000FFF00U
#define LM_RCV_CRED_LIM1_CPC_SHIFT                     8U
#define LM_RCV_CRED_LIM1_CPC_WIDTH                    12U
#define LM_RCV_CRED_LIM1_CHC_MASK             0xFF000000U
#define LM_RCV_CRED_LIM1_CHC_SHIFT                    24U
#define LM_RCV_CRED_LIM1_CHC_WIDTH                     8U

#define LM_PF_CONFIG0_BAR0A_MASK              0x0000001FU
#define LM_PF_CONFIG0_BAR0A_SHIFT                      0U
#define LM_PF_CONFIG0_BAR1A_MASK              0x00001F00U
#define LM_PF_CONFIG0_BAR1A_SHIFT                      8U
#define LM_PF_CONFIG0_BAR2A_MASK              0x001F0000U
#define LM_PF_CONFIG0_BAR2A_SHIFT                     16U
#define LM_PF_CONFIG0_BAR3A_MASK              0x1F000000U
#define LM_PF_CONFIG0_BAR3A_SHIFT                     24U

#define LM_PF_CONFIG1_BAR4A_MASK              0x0000001FU
#define LM_PF_CONFIG1_BAR4A_SHIFT                      0U
#define LM_PF_CONFIG1_BAR5A_MASK              0x00001F00U
#define LM_PF_CONFIG1_BAR5A_SHIFT                      8U

#define LM_PF_CONFIG0_BAR0C_MASK              0x000000E0U
#define LM_PF_CONFIG0_BAR0C_SHIFT                      5U
#define LM_PF_CONFIG0_BAR1C_MASK              0x0000E000U
#define LM_PF_CONFIG0_BAR1C_SHIFT                     13U
#define LM_PF_CONFIG0_BAR2C_MASK              0x00E00000U
#define LM_PF_CONFIG0_BAR2C_SHIFT                     21U
#define LM_PF_CONFIG0_BAR3C_MASK              0xE0000000U
#define LM_PF_CONFIG0_BAR3C_SHIFT                     29U

#define LM_PF_CONFIG1_BAR4C_MASK              0x000000E0U
#define LM_PF_CONFIG1_BAR4C_SHIFT                      5U
#define LM_PF_CONFIG1_BAR5C_MASK              0x0000E000U
#define LM_PF_CONFIG1_BAR5C_SHIFT                     13U

#define LM_RC_CONFIG_RCBAR0A_MASK             0x0000003FU
#define LM_RC_CONFIG_RCBAR0A_SHIFT                     0U
#define LM_RC_CONFIG_RCBAR0C_MASK             0x000001C0U
#define LM_RC_CONFIG_RCBAR0C_SHIFT                     6U
#define LM_RC_CONFIG_RCBAR1A_MASK             0x00003E00U
#define LM_RC_CONFIG_RCBAR1A_SHIFT                     9U
#define LM_RC_CONFIG_RCBAR1C_MASK             0x0001C000U
#define LM_RC_CONFIG_RCBAR1C_SHIFT                    14U
#define LM_RC_CONFIG_RCBARPME_MASK            0x00020000U
#define LM_RC_CONFIG_RCBARPME_SHIFT                   17U
#define LM_RC_CONFIG_RCBARPME_WIDTH                    1U
#define LM_RC_CONFIG_RCBARPME_WOCLR                    0U
#define LM_RC_CONFIG_RCBARPME_WOSET                    0U
#define LM_RC_CONFIG_RCBARPMS_MASK            0x00040000U
#define LM_RC_CONFIG_RCBARPMS_SHIFT                   18U
#define LM_RC_CONFIG_RCBARPMS_WIDTH                    1U
#define LM_RC_CONFIG_RCBARPMS_WOCLR                    0U
#define LM_RC_CONFIG_RCBARPMS_WOSET                    0U
#define LM_RC_CONFIG_RCBARPIE_MASK            0x00080000U
#define LM_RC_CONFIG_RCBARPIE_SHIFT                   19U
#define LM_RC_CONFIG_RCBARPIE_WIDTH                    1U
#define LM_RC_CONFIG_RCBARPIE_WOCLR                    0U
#define LM_RC_CONFIG_RCBARPIE_WOSET                    0U
#define LM_RC_CONFIG_RCBARPIS_MASK            0x00100000U
#define LM_RC_CONFIG_RCBARPIS_SHIFT                   20U
#define LM_RC_CONFIG_RCBARPIS_WIDTH                    1U
#define LM_RC_CONFIG_RCBARPIS_WOCLR                    0U
#define LM_RC_CONFIG_RCBARPIS_WOSET                    0U
#define LM_RC_CONFIG_RCBCE_MASK               0x80000000U
#define LM_RC_CONFIG_RCBCE_SHIFT                      31U
#define LM_RC_CONFIG_RCBCE_WIDTH                       1U
#define LM_RC_CONFIG_RCBCE_WOCLR                       0U
#define LM_RC_CONFIG_RCBCE_WOSET                       0U

#define LM_PL_CONFIG0_LS_MASK                 0x00000001U
#define LM_PL_CONFIG0_LS_SHIFT                         0U
#define LM_PL_CONFIG0_LTSSM_MASK              0x3F000000U
#define LM_PL_CONFIG0_LTSSM_SHIFT                     24U
#define LM_PL_CONFIG0_LTD_MASK                0x00000020U
#define LM_PL_CONFIG0_LTD_SHIFT                        5U
#define LM_PL_CONFIG0_APER_MASK               0x00000040U
#define LM_PL_CONFIG0_APER_SHIFT                       6U
#define LM_PL_CONFIG0_APER_WIDTH                       1U
#define LM_PL_CONFIG0_APER_WOCLR                       0U
#define LM_PL_CONFIG0_APER_WOSET                       0U
#define LM_PL_CONFIG0_TSS_MASK                0x00000080U
#define LM_PL_CONFIG0_TSS_SHIFT                        7U
#define LM_PL_CONFIG0_TSS_WIDTH                        1U
#define LM_PL_CONFIG0_TSS_WOCLR                        0U
#define LM_PL_CONFIG0_TSS_WOSET                        0U
#define LM_PL_CONFIG0_RFC_MASK                0x0000FF00U
#define LM_PL_CONFIG0_RFC_SHIFT                        8U
#define LM_PL_CONFIG0_NLC_MASK                0x00000006U
#define LM_PL_CONFIG0_NLC_SHIFT                        1U
#define LM_PL_CONFIG0_NS_MASK                 0x00000018U
#define LM_PL_CONFIG0_NS_SHIFT                         3U
#define LM_PL_CONFIG0_RLID_MASK               0x00FF0000U
#define LM_PL_CONFIG0_RLID_SHIFT                      16U
#define LM_PL_CONFIG0_R0_MASK                 0x40000000U
#define LM_PL_CONFIG0_R0_SHIFT                        30U
#define LM_PL_CONFIG0_MLE_MASK                0x80000000U
#define LM_PL_CONFIG0_MLE_SHIFT                       31U
#define LM_PL_CONFIG0_MLE_WIDTH                        1U
#define LM_PL_CONFIG0_MLE_WOCLR                        0U
#define LM_PL_CONFIG0_MLE_WOSET                        0U

#define LM_PL_CONFIG1_TLI_MASK                0x000000FFU
#define LM_PL_CONFIG1_TLI_SHIFT                        0U
#define LM_PL_CONFIG1_TFC1_MASK               0x0000FF00U
#define LM_PL_CONFIG1_TFC1_SHIFT                       8U
#define LM_PL_CONFIG1_TFC1_WIDTH                       8U
#define LM_PL_CONFIG1_TFC2_MASK               0x00FF0000U
#define LM_PL_CONFIG1_TFC2_SHIFT                      16U
#define LM_PL_CONFIG1_TFC2_WIDTH                       8U
#define LM_PL_CONFIG1_TFC3_MASK               0xFF000000U
#define LM_PL_CONFIG1_TFC3_SHIFT                      24U
#define LM_PL_CONFIG1_TFC3_WIDTH                       8U

#define LM_NEGO_LANE_MAP_NLM_MASK             0x0000000FU
#define LM_NEGO_LANE_MAP_NLM_SHIFT                     0U
#define LM_NEGO_LANE_MAP_LRS_MASK             0x00010000U
#define LM_NEGO_LANE_MAP_LRS_SHIFT                    16U

#define LM_LINKWIDTH_CTRL_TLM_MASK            0x0000000FU
#define LM_LINKWIDTH_CTRL_TLM_SHIFT                    0U
#define LM_LINKWIDTH_CTRL_RL_MASK             0x00010000U
#define LM_LINKWIDTH_CTRL_RL_SHIFT                    16U
#define LM_LINKWIDTH_CTRL_RL_WOCLR                     0U

#define LM_GEN3_LEQ_LANE0_LEQTXPR_MASK        0x0000000FU
#define LM_GEN3_LEQ_LANE0_LEQTXPR_SHIFT                0U
#define LM_GEN3_LEQ_LANE0_LEQTXPRV_MASK       0x00000010U
#define LM_GEN3_LEQ_LANE0_LEQTXPRV_SHIFT               4U
#define LM_GEN3_LEQ_LANE0_LEQTXCO_MASK        0x03FFFF00U
#define LM_GEN3_LEQ_LANE0_LEQTXCO_SHIFT                8U

#define LM_GEN3_LEQ_LANE1_LEQTXPR_MASK        0x0000000FU
#define LM_GEN3_LEQ_LANE1_LEQTXPR_SHIFT                0U
#define LM_GEN3_LEQ_LANE1_LEQTXPRV_MASK       0x00000010U
#define LM_GEN3_LEQ_LANE1_LEQTXPRV_SHIFT               4U
#define LM_GEN3_LEQ_LANE1_LEQTXCO_MASK        0x03FFFF00U
#define LM_GEN3_LEQ_LANE1_LEQTXCO_SHIFT                8U

#define LM_GEN3_LEQ_LANE2_LEQTXPR_MASK        0x0000000FU
#define LM_GEN3_LEQ_LANE2_LEQTXPR_SHIFT                0U
#define LM_GEN3_LEQ_LANE2_LEQTXPRV_MASK       0x00000010U
#define LM_GEN3_LEQ_LANE2_LEQTXPRV_SHIFT               4U
#define LM_GEN3_LEQ_LANE2_LEQTXCO_MASK        0x03FFFF00U
#define LM_GEN3_LEQ_LANE2_LEQTXCO_SHIFT                8U

#define LM_GEN3_LEQ_LANE3_LEQTXPR_MASK        0x0000000FU
#define LM_GEN3_LEQ_LANE3_LEQTXPR_SHIFT                0U
#define LM_GEN3_LEQ_LANE3_LEQTXPRV_MASK       0x00000010U
#define LM_GEN3_LEQ_LANE3_LEQTXPRV_SHIFT               4U
#define LM_GEN3_LEQ_LANE3_LEQTXCO_MASK        0x03FFFF00U
#define LM_GEN3_LEQ_LANE3_LEQTXCO_SHIFT                8U

#define LM_LOCAL_ERROR_STATUS_WOCLR_MASK      0xEE2C0FFFU

#define LM_LOCAL_INT_MASK_PRFPE_SHIFT                  0U
#define LM_LOCAL_INT_MASK_CRFPE_SHIFT                  1U
#define LM_LOCAL_INT_MASK_RRPE_SHIFT                   2U
#define LM_LOCAL_INT_MASK_PRFO_SHIFT                   3U
#define LM_LOCAL_INT_MASK_CRFO_SHIFT                   4U
#define LM_LOCAL_INT_MASK_RT_SHIFT                     5U
#define LM_LOCAL_INT_MASK_RTR_SHIFT                    6U
#define LM_LOCAL_INT_MASK_PE_SHIFT                     7U
#define LM_LOCAL_INT_MASK_MTR_SHIFT                    8U
#define LM_LOCAL_INT_MASK_UCR_SHIFT                    9U
#define LM_LOCAL_INT_MASK_FCE_SHIFT                   10U
#define LM_LOCAL_INT_MASK_CT_SHIFT                    11U
#define LM_LOCAL_INT_MASK_MMVC_SHIFT                  19U
#define LM_LOCAL_INT_MASK_HAWCD_SHIFT                 21U
#define LM_LOCAL_INT_MASK_MSIXMSK_SHIFT               25U
#define LM_LOCAL_INT_MASK_OB_BUFF_ER_UN_SHIFT         26U
#define LM_LOCAL_INT_MASK_IB_BUFF_ER_UN_SHIFT         27U
#define LM_LOCAL_INT_MASK_AXIMASTER_RFIFO_ER_UN_SHIFT 29U
#define LM_LOCAL_INT_MASK_AXISLAVE_WFIFO_ER_UN_SHIFT  30U
#define LM_LOCAL_INT_MASK_REORDER_ER_UN_SHIFT         31U


#define LM_LCRC_ERR_COUNT_WOCLR_MASK          0x0000FFFFU
#define LM_LCRC_ERR_COUNT_LEC_MASK            0x0000FFFFU
#define LM_LCRC_ERR_COUNT_LEC_SHIFT                    0U

#define LM_ECC_CORR_ERR_COUNT_WOCLR_MASK      0x00FFFFFFU
#define LM_ECC_CORR_ERR_COUNT_PFRCER_MASK     0x000000FFU
#define LM_ECC_CORR_ERR_COUNT_PFRCER_SHIFT             0U
#define LM_ECC_CORR_ERR_COUNT_SFRCER_MASK     0x0000FF00U
#define LM_ECC_CORR_ERR_COUNT_SFRCER_SHIFT             8U
#define LM_ECC_CORR_ERR_COUNT_RRCER_MASK      0x00FF0000U
#define LM_ECC_CORR_ERR_COUNT_RRCER_SHIFT             16U

#define LM_LTR_SNOOP_LAT_NSLV_MASK            0x000003FFU
#define LM_LTR_SNOOP_LAT_NSLV_SHIFT                    0U
#define LM_LTR_SNOOP_LAT_NSLV_WIDTH                   10U
#define LM_LTR_SNOOP_LAT_NSLS_MASK            0x00001C00U
#define LM_LTR_SNOOP_LAT_NSLS_SHIFT                   10U
#define LM_LTR_SNOOP_LAT_NSLS_WIDTH                    3U
#define LM_LTR_SNOOP_LAT_NSLR_MASK            0x00008000U
#define LM_LTR_SNOOP_LAT_NSLR_SHIFT                   15U
#define LM_LTR_SNOOP_LAT_SLV_MASK             0x03FF0000U
#define LM_LTR_SNOOP_LAT_SLV_SHIFT                    16U
#define LM_LTR_SNOOP_LAT_SLV_WIDTH                    10U
#define LM_LTR_SNOOP_LAT_SLS_MASK             0x1C000000U
#define LM_LTR_SNOOP_LAT_SLS_SHIFT                    26U
#define LM_LTR_SNOOP_LAT_SLS_WIDTH                     3U
#define LM_LTR_SNOOP_LAT_SL_MASK              0x80000000U
#define LM_LTR_SNOOP_LAT_SL_SHIFT                     31U

#define LM_LTR_MSG_GEN_CTL_SLM_MASK           0x00000400U
#define LM_LTR_MSG_GEN_CTL_SLM_SHIFT                  10U
#define LM_LTR_MSG_GEN_CTL_SLM_WOCLR                   0U

#define LM_PME_SVC_TO_PSTD_MASK               0x000FFFFFU
#define LM_PME_SVC_TO_PSTD_SHIFT                       0U
#define LM_PME_SVC_TO_PSTD_WIDTH                      20U

#define LM_RP_REQUESTOR_ID_RPRI_MASK          0x0000FFFFU
#define LM_RP_REQUESTOR_ID_RPRI_SHIFT                  0U

#define LM_DBG_MUX_CTRL_MS_MASK               0x0000001FU
#define LM_DBG_MUX_CTRL_MS_SHIFT                       0U
#define LM_DBG_MUX_CTRL_AWRPRI_SHIFT                  16U
#define LM_DBG_MUX_CTRL_DSHEC_SHIFT                   21U
#define LM_DBG_MUX_CTRL_DLRFE_SHIFT                   22U
#define LM_DBG_MUX_CTRL_DLUC_SHIFT                    23U
#define LM_DBG_MUX_CTRL_EFLT_SHIFT                    24U
#define LM_DBG_MUX_CTRL_ESPC_SHIFT                    25U
#define LM_DBG_MUX_CTRL_DGLUS_SHIFT                   27U
#define LM_DBG_MUX_CTRL_DEI_SHIFT                     28U
#define LM_DBG_MUX_CTRL_DFCUT_SHIFT                   29U
#define LM_DBG_MUX_CTRL_DOC_SHIFT                     30U
#define LM_DBG_MUX_CTRL_EFSRTCA_SHIFT                 31U


#define AXI_ADDR0_NUM_BITS_MASK               0x0000003FU
#define AXI_ADDR0_NUM_BITS_SHIFT                       0U
#define AXI_ADDR0_DATA_MASK                   0xFFFFFF00U
#define AXI_ADDR0_DATA_SHIFT                           8U
#define AXI_ADDR1_DATA_MASK                   0xFFFFFFFFU
#define AXI_ADDR1_DATA_SHIFT                           0U
#define AXI_DESC0_DATA_MASK                   0xFFFFFFFFU
#define AXI_DESC0_DATA_SHIFT                           0U


#define DMA_INT_CH0_DONE_MASK                 0x00000001U
#define DMA_INT_CH0_DONE_SHIFT                         0U
#define DMA_INT_CH1_DONE_MASK                 0x00000002U
#define DMA_INT_CH1_DONE_SHIFT                         1U
#define DMA_INT_CH2_DONE_MASK                 0x00000004U
#define DMA_INT_CH2_DONE_SHIFT                         2U
#define DMA_INT_CH3_DONE_MASK                 0x00000008U
#define DMA_INT_CH3_DONE_SHIFT                         3U

#define DMA_INT_CH0_ERR_MASK                  0x00000100U
#define DMA_INT_CH0_ERR_SHIFT                          8U
#define DMA_INT_CH1_ERR_MASK                  0x00000200U
#define DMA_INT_CH1_ERR_SHIFT                          9U
#define DMA_INT_CH2_ERR_MASK                  0x00000400U
#define DMA_INT_CH2_ERR_SHIFT                         10U
#define DMA_INT_CH3_ERR_MASK                  0x00000800U
#define DMA_INT_CH3_ERR_SHIFT                         11U

#define DMA_INT_ENA_CH0_DONE_MASK             0x00000001U
#define DMA_INT_ENA_CH0_DONE_SHIFT                     0U
#define DMA_INT_ENA_CH1_DONE_MASK             0x00000002U
#define DMA_INT_ENA_CH1_DONE_SHIFT                     1U
#define DMA_INT_ENA_CH2_DONE_MASK             0x00000004U
#define DMA_INT_ENA_CH2_DONE_SHIFT                     2U
#define DMA_INT_ENA_CH3_DONE_MASK             0x00000008U
#define DMA_INT_ENA_CH3_DONE_SHIFT                     3U
#define DMA_INT_ENA_DONE_WIDTH                         1U
#define DMA_INT_ENA_DONE_WOCLR                         1U
#define DMA_INT_ENA_DONE_WOSET                         0U

#define DMA_INT_ENA_CH0_ERR_MASK              0x00000100U
#define DMA_INT_ENA_CH0_ERR_SHIFT                      8U
#define DMA_INT_ENA_CH1_ERR_MASK              0x00000200U
#define DMA_INT_ENA_CH1_ERR_SHIFT                      9U
#define DMA_INT_ENA_CH2_ERR_MASK              0x00000400U
#define DMA_INT_ENA_CH2_ERR_SHIFT                     10U
#define DMA_INT_ENA_CH3_ERR_MASK              0x00000800U
#define DMA_INT_ENA_CH3_ERR_SHIFT                     11U
#define DMA_INT_ENA_ERR_WIDTH                          1U
#define DMA_INT_ENA_ERR_WOCLR                          1U
#define DMA_INT_ENA_ERR_WOSET                          0U

#define DMA_CONFIG_NUM_CHANNELS_MASK          0x0000000FU
#define DMA_CONFIG_NUM_CHANNELS_SHIFT                  0U
#define DMA_CONFIG_NUM_PARTITIONS_MASK        0x000000F0U
#define DMA_CONFIG_NUM_PARTITIONS_SHIFT                4U
#define DMA_CONFIG_PARTITION_SIZE_MASK        0x00000F00U
#define DMA_CONFIG_PARTITION_SIZE_SHIFT                8U

#define DMA_CONFIG_SYS_AW_GT_32_MASK          0x00001000U
#define DMA_CONFIG_SYS_AW_GT_32_SHIFT                 12U
#define DMA_CONFIG_SYS_TW_GT_32_MASK          0x00002000U
#define DMA_CONFIG_SYS_TW_GT_32_SHIFT                 13U
#define DMA_CONFIG_EXT_AW_GT_32_MASK          0x00004000U
#define DMA_CONFIG_EXT_AW_GT_32_SHIFT                 14U
#define DMA_CONFIG_EXT_TW_GT_32_MASK          0x00008000U
#define DMA_CONFIG_EXT_TW_GT_32_SHIFT                 15U

#define DMA_CAP_VER_MIN_MASK                  0x000000FFU
#define DMA_CAP_VER_MIN_SHIFT                          0U
#define DMA_CAP_VER_MAJ_MASK                  0x0000FF00U
#define DMA_CAP_VER_MAJ_SHIFT                          8U

#define DMA_INT_DIS_CH0_DONE_MASK             0x00000001U
#define DMA_INT_DIS_CH0_DONE_SHIFT                     0U
#define DMA_INT_DIS_CH1_DONE_MASK             0x00000002U
#define DMA_INT_DIS_CH1_DONE_SHIFT                     1U
#define DMA_INT_DIS_CH2_DONE_MASK             0x00000004U
#define DMA_INT_DIS_CH2_DONE_SHIFT                     2U
#define DMA_INT_DIS_CH3_DONE_MASK             0x00000008U
#define DMA_INT_DIS_CH3_DONE_SHIFT                     3U
#define DMA_INT_DIS_DONE_WIDTH                         1U
#define DMA_INT_DIS_DONE_WOCLR                         0U

#define DMA_INT_DIS_CH0_ERR_MASK              0x00000100U
#define DMA_INT_DIS_CH0_ERR_SHIFT                      8U
#define DMA_INT_DIS_CH1_ERR_MASK              0x00000200U
#define DMA_INT_DIS_CH1_ERR_SHIFT                      9U
#define DMA_INT_DIS_CH2_ERR_MASK              0x00000400U
#define DMA_INT_DIS_CH2_ERR_SHIFT                     10U
#define DMA_INT_DIS_CH3_ERR_MASK              0x00000800U
#define DMA_INT_DIS_CH3_ERR_SHIFT                     11U
#define DMA_INT_DIS_ERR_WIDTH                          1U
#define DMA_INT_DIS_ERR_WOCLR                          0U

typedef struct __attribute__((packed))
{
    struct {
        volatile UINT32 i_vendor_id_device_id;
        volatile UINT32 i_command_status;
        volatile UINT32 i_revision_id_class_code;
        volatile UINT32 i_bist_header_latency_cache_line;
        volatile UINT32 i_base_addr_0;
        volatile UINT32 i_base_addr_1;
        volatile UINT32 i_base_addr_2;
        volatile UINT32 i_base_addr_3;
        volatile UINT32 i_base_addr_4;
        volatile UINT32 i_base_addr_5;
        volatile UINT32 rsvd_0A;
        volatile UINT32 i_subsystem_vendor_id_subsystem_i;
        volatile UINT32 rsvd_0C;
        volatile UINT32 i_capabilities_pointer;
        volatile UINT32 rsvd_0E;
        volatile UINT32 i_intrpt_line_intrpt_pin;
        volatile UINT32 rsvd_010_01F[16];
    } i_pcie_base;
    struct {
        volatile UINT32 i_pwr_mgmt_cap;
        volatile UINT32 i_pwr_mgmt_ctrl_stat_rep;
        volatile UINT32 rsvd_022_023[2];
    } i_power_mgmt_cap_struct;
    struct {
        volatile UINT32 i_msi_ctrl_reg;
        volatile UINT32 i_msi_msg_low_addr;
        volatile UINT32 i_msi_msg_hi_addr;
        volatile UINT32 i_msi_msg_data;
        volatile UINT32 i_msi_mask;
        volatile UINT32 i_msi_pending_bits;
        volatile UINT32 rsvd_02A_02B[2];
    } i_MSI_cap_struct;
    struct {
        volatile UINT32 i_msix_ctrl;
        volatile UINT32 i_msix_tbl_offset;
        volatile UINT32 i_msix_pending_intrpt;
        volatile UINT32 rsvd_02F;
    } i_MSIX_cap_struct;
    struct {
        volatile UINT32 i_pcie_cap_list;
        volatile UINT32 i_pcie_dev_cap;
        volatile UINT32 i_pcie_dev_ctrl_status;
        volatile UINT32 i_link_cap;
        volatile UINT32 i_link_ctrl_status;
        volatile UINT32 rsvd_035;
        volatile UINT32 rsvd_036;
        volatile UINT32 rsvd_037_038[2];
        volatile UINT32 i_pcie_dev_cap_2;
        volatile UINT32 i_pcie_dev_ctrl_status_2;
        volatile UINT32 i_link_cap_2_reg;
        volatile UINT32 i_link_ctrl_status_2;
        volatile UINT32 rsvd_03D_03F[3];
    } i_PCIE_cap_struct;
    struct {
        volatile UINT32 i_AER_enhanced_cap_hdr;
        volatile UINT32 i_uncorr_err_status;
        volatile UINT32 i_uncorr_err_mask;
        volatile UINT32 i_uncorr_err_severity;
        volatile UINT32 i_corr_err_status;
        volatile UINT32 i_corr_err_mask;
        volatile UINT32 i_advcd_err_cap_ctrl;
        volatile UINT32 i_hdr_log_0;
        volatile UINT32 i_hdr_log_1;
        volatile UINT32 i_hdr_log_2;
        volatile UINT32 i_hdr_log_3;
        volatile UINT32 rsvd_04B_04D[3];
    } i_AER_cap_struct;
    volatile char pad__0[0x80U];
    struct {
        volatile UINT32 i_LTR_ext_cap_hdr;
        volatile UINT32 i_LTR_snoop_lat;
    } i_LTR_cap_struct;
    volatile char pad__1[0x140U];
    struct {
        volatile UINT32 i_sec_pcie_cap_hdr_reg;
        volatile UINT32 i_link_control3_reg;
        volatile UINT32 i_lane_error_status_reg;
        volatile UINT32 i_lane_equalization_control_0;
        volatile UINT32 i_lane_equalization_control_1;
    } i_PCIE_sec_ext_cap_struct;
    volatile char pad__2[0x5ECU];
    struct {
        volatile UINT32 i_L1_PM_ext_cap_hdr;
        volatile UINT32 i_L1_PM_cap;
        volatile UINT32 i_L1_PM_ctrl_1;
        volatile UINT32 i_L1_PM_ctrl_2;
    } i_regf_L1_PM_cap_struct;
}
PCIE_IClientPf0;

typedef struct __attribute__((packed)) PCIE_IClientRc_s {
    struct {
        volatile UINT32 i_vendor_id_device_id;
        volatile UINT32 i_command_status;
        volatile UINT32 i_revision_id_class_code;
        volatile UINT32 i_bist_header_latency_cache_line;
        volatile UINT32 i_RC_BAR_0;
        volatile UINT32 i_RC_BAR_1;
        volatile UINT32 i_pcie_bus_numbers;
        volatile UINT32 i_pcie_io_base_limit;
        volatile UINT32 i_pcie_mem_base_limit;
        volatile UINT32 i_pcie_prefetch_base_limit;
        volatile UINT32 i_pcie_prefetch_base_upper;
        volatile UINT32 i_pcie_prefetch_limit_upper;
        volatile UINT32 i_pcie_io_base_limit_upper;
        volatile UINT32 i_capabilities_pointer;
        volatile UINT32 rsvd_0E;
        volatile UINT32 i_intrpt_line_intrpt_pin;
        volatile char pad__0[0x40U];
        volatile UINT32 i_pwr_mgmt_cap;
        volatile UINT32 i_pwr_mgmt_ctrl_stat_rep;
        volatile char pad__1[0x8U];
        volatile UINT32 i_msi_ctrl_reg;
        volatile UINT32 i_msi_msg_low_addr;
        volatile UINT32 i_msi_msg_hi_addr;
        volatile UINT32 i_msi_msg_data;
        volatile UINT32 i_msi_mask;
        volatile UINT32 i_msi_pending_bits;
        volatile char pad__2[0x8U];
        volatile UINT32 i_msix_ctrl;
        volatile UINT32 i_msix_tbl_offset;
        volatile UINT32 i_msix_pending_intrpt;
        volatile char pad__3[0x4U];
        volatile UINT32 i_pcie_cap_list;
        volatile UINT32 i_pcie_cap;
        volatile UINT32 i_pcie_dev_ctrl_status;
        volatile UINT32 i_link_cap;
        volatile UINT32 i_link_ctrl_status;
        volatile UINT32 rsvd_35;
        volatile UINT32 i_slot_ctrl_status;
        volatile UINT32 i_root_ctrl_cap;
        volatile UINT32 i_root_status;
        volatile UINT32 i_pcie_cap_2;
        volatile UINT32 i_pcie_dev_ctrl_status_2;
        volatile UINT32 i_link_cap_2;
        volatile UINT32 i_link_ctrl_status_2;
        volatile char pad__4[0xCU];
        volatile UINT32 i_AER_enhncd_cap;
        volatile UINT32 i_uncorr_err_status;
        volatile UINT32 i_uncorr_err_mask;
        volatile UINT32 i_uncorr_err_severity;
        volatile UINT32 i_corr_err_status;
        volatile UINT32 i_corr_err_mask;
        volatile UINT32 i_adv_err_cap_ctl;
        volatile UINT32 i_hdr_log_0;
        volatile UINT32 i_hdr_log_1;
        volatile UINT32 i_hdr_log_2;
        volatile UINT32 i_hdr_log_3;
        volatile UINT32 i_root_err_cmd;
        volatile UINT32 i_root_err_stat;
        volatile UINT32 i_err_src_id;
        volatile char pad__5[0x1C8U];
        volatile UINT32 i_sec_pcie_cap_hdr_reg;
        volatile UINT32 i_link_control3;
        volatile UINT32 i_lane_error_status;
        volatile UINT32 i_lane_equalization_control_0;
        volatile UINT32 i_lane_equalization_control_1;
    } i_rc_pcie_base;
    volatile char pad__6[0x5ECU];
    struct {
        volatile UINT32 i_L1_PM_ext_cap_hdr;
        volatile UINT32 i_L1_PM_cap;
        volatile UINT32 i_L1_PM_ctrl_1;
        volatile UINT32 i_L1_PM_ctrl_2;
    } i_regf_L1_PM_cap_struct;
} PCIE_IClientRc;

typedef struct __attribute__((packed)) PCIE_IClientLm_s {
    struct {
        volatile UINT32 i_pl_config_0_reg;
        volatile UINT32 i_pl_config_1_reg;
        volatile UINT32 i_dll_tmr_config_reg;
        volatile UINT32 i_rcv_cred_lim_0_reg;
        volatile UINT32 i_rcv_cred_lim_1_reg;
        volatile UINT32 i_transm_cred_lim_0_reg;
        volatile UINT32 i_transm_cred_lim_1_reg;
        volatile UINT32 i_tx_cred_update_int_config_0_reg;
        volatile UINT32 i_tx_cred_update_int_config_1_reg;
        volatile UINT32 i_L0S_timeout_limit_reg;
        volatile UINT32 i_transmit_tlp_count_reg;
        volatile UINT32 i_transmit_tlp_payload_dword_count_reg;
        volatile UINT32 i_receive_tlp_count_reg;
        volatile UINT32 i_receive_tlp_payload_dword_count_reg;
        volatile UINT32 i_compln_tmout_lim_0_reg;
        volatile UINT32 i_compln_tmout_lim_1_reg;
        volatile UINT32 i_L1_st_reentry_delay_reg;
        volatile UINT32 i_vendor_id_reg;
        volatile UINT32 i_aspm_L1_entry_tmout_delay_reg;
        volatile UINT32 i_pme_turnoff_ack_delay_reg;
        volatile UINT32 i_linkwidth_control_reg;
        volatile UINT32 i_pl_config_2_reg;
        volatile char pad__0[0x1CU];
        volatile UINT32 i_sris_control_reg;
        volatile char pad__1[0x88U];
        volatile UINT32 i_shdw_hdr_log_0_reg;
        volatile UINT32 i_shdw_hdr_log_1_reg;
        volatile UINT32 i_shdw_hdr_log_2_reg;
        volatile UINT32 i_shdw_hdr_log_3_reg;
        volatile UINT32 i_shdw_func_num_reg;
        volatile UINT32 i_shdw_ur_err_reg;
        volatile char pad__2[0x28U];
        volatile UINT32 i_pm_clk_frequency_reg;
        volatile UINT32 i_debug_dllp_count_gen1_reg;
        volatile UINT32 i_debug_dllp_count_gen2_reg;
        volatile UINT32 i_debug_dllp_count_gen3_reg;
        volatile char pad__3[0x8U];
        volatile UINT32 i_vendor_defined_message_tag_reg;
        volatile char pad__4[0xA4U];
        volatile UINT32 i_negotiated_lane_map_reg;
        volatile UINT32 i_receive_fts_count_reg;
        volatile UINT32 i_debug_mux_control_reg;
        volatile UINT32 i_local_error_status_register;
        volatile UINT32 i_local_intrpt_mask_reg;
        volatile UINT32 i_lcrc_err_count_reg;
        volatile UINT32 i_ecc_corr_err_count_reg;
        volatile UINT32 i_ltr_snoop_lat_reg;
        volatile UINT32 i_ltr_msg_gen_ctl_reg;
        volatile UINT32 i_pme_service_timeout_delay_reg;
        volatile UINT32 i_root_port_requestor_id_reg;
        volatile UINT32 i_ep_bus_device_number_reg;
        volatile char pad__5[0x4U];
        volatile UINT32 i_debug_mux_control_2_reg;
        volatile UINT32 i_phy_status_1_reg;
        volatile UINT32 i_debug_mux_control_3_reg;
        volatile UINT32 i_pf_0_BAR_config_0_reg;
        volatile UINT32 i_pf_0_BAR_config_1_reg;
        volatile char pad__6[0x78U];
        volatile UINT32 i_pf_config_reg;
        volatile char pad__7[0x3CU];
        volatile UINT32 i_rc_BAR_config_reg;
        volatile char pad__8[0x5CU];
        volatile UINT32 i_gen3_default_preset_reg;
        volatile UINT32 i_gen3_link_eq_timeout_2ms_reg;
        volatile UINT32 i_pipe_fifo_latency_ctrl_reg;
        volatile char pad__9[0x10U];
        volatile UINT32 i_gen3_link_eq_ctrl_reg;
        volatile UINT32 i_gen3_leq_debug_status_lane0;
        volatile UINT32 i_gen3_leq_debug_status_lane1;
        volatile UINT32 i_gen3_leq_debug_status_lane2;
        volatile UINT32 i_gen3_leq_debug_status_lane3;
        volatile char pad__10[0x8F0U];
        volatile UINT32 i_ecc_corr_err_count_reg_axi;
        volatile char pad__11[0x4U];
        volatile UINT32 low_power_debug_and_control0;
        volatile UINT32 low_power_debug_and_control1;
        volatile UINT32 low_power_debug_and_control2;
        volatile UINT32 tl_internal_control;
        volatile char pad__12[0x68U];
        volatile UINT32 i_local_error_status_2_register;
        volatile UINT32 i_local_intrpt_mask_2_reg;
        volatile char pad__13[0x98U];
        volatile UINT32 i_ld_ctrl;
        volatile UINT32 rx_elec_idle_filter_control;
        volatile char pad__14[0xA4U];
        volatile UINT32 i_eq_debug_mon_control_reg;
        volatile UINT32 i_eq_debug_mon_status0_reg;
        volatile UINT32 i_eq_debug_mon_status_reg;
        volatile char pad__15[0x4U];
        volatile UINT32 i_axi_feature_reg;
        volatile UINT32 i_link_eq_control_2_reg;
        volatile UINT32 i_core_feature_reg;
        volatile char pad__16[0x20U];
        volatile UINT32 i_rx_invert_polarity_reg;
        volatile char pad__17[0x120U];
        volatile UINT32 i_ltssm_timer_control_reg0;
        volatile UINT32 i_ltssm_timer_control_reg1;
        volatile UINT32 i_ltssm_timer_control_reg2;
        volatile UINT32 i_ltssm_timer_control_reg3;
    } i_regf_lm_pcie_base;
} PCIE_IClientLm;

typedef struct {
    volatile UINT32 addr0;
    volatile UINT32 addr1;
    volatile UINT32 desc0;
    volatile UINT32 desc1;
    volatile char pad__0[0x8U];
    volatile UINT32 axi_addr0;
    volatile UINT32 axi_addr1;
} AxiWrapperOb;

typedef struct {
    volatile UINT32 addr0;
    volatile UINT32 addr1;
} AxiWrapperIb;

typedef struct __attribute__((packed)) PCIE_IClientAtu_s {
    AxiWrapperOb ob0;
    AxiWrapperOb ob1;
    AxiWrapperOb ob2;
    AxiWrapperOb ob3;
    AxiWrapperOb ob4;
    AxiWrapperOb ob5;
    AxiWrapperOb ob6;
    AxiWrapperOb ob7;
    volatile char pad__8[0x700U];
    AxiWrapperIb ib0;
    AxiWrapperIb ib1;
    AxiWrapperIb ib7;
    volatile char pad__9[0xCU];
    struct {
        volatile UINT32 L0;
    } link_down_indicator_bit;
    volatile char pad__10[0x18U];
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_0;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_1;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_2;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_3;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_4;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_5;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_6;
    struct {
        volatile UINT32 addr0;
        volatile UINT32 addr1;
    } func0_wrapper_ib_ep_7;
} PCIE_IClientAtu;

typedef struct __attribute__((packed)) PCIE_IClientUdma_s {
    struct {
        volatile UINT32 channel_ctrl;
        volatile UINT32 channel_sp_l;
        volatile UINT32 channel_sp_u;
        volatile UINT32 channel_attr_l;
        volatile UINT32 channel_attr_u;
    } dma_ch0;
    struct {
        volatile UINT32 channel_ctrl;
        volatile UINT32 channel_sp_l;
        volatile UINT32 channel_sp_u;
        volatile UINT32 channel_attr_l;
        volatile UINT32 channel_attr_u;
    } dma_ch1;
    struct {
        volatile UINT32 channel_ctrl;
        volatile UINT32 channel_sp_l;
        volatile UINT32 channel_sp_u;
        volatile UINT32 channel_attr_l;
        volatile UINT32 channel_attr_u;
    } dma_ch2;
    struct {
        volatile UINT32 channel_ctrl;
        volatile UINT32 channel_sp_l;
        volatile UINT32 channel_sp_u;
        volatile UINT32 channel_attr_l;
        volatile UINT32 channel_attr_u;
    } dma_ch3;
    volatile char pad__0[0x50U];
    struct {
        volatile UINT32 common_udma_int;
        volatile UINT32 common_udma_int_ena;
        volatile UINT32 common_udma_int_dis;
        volatile UINT32 common_udma_ib_ecc_uncorrectable_errors;
        volatile UINT32 common_udma_ib_ecc_correctable_errors;
        volatile UINT32 common_udma_ob_ecc_uncorrectable_errors;
        volatile UINT32 common_udma_ob_ecc_correctable_errors;
        volatile char pad__1[0x3CU];
        volatile UINT32 common_udma_cap_ver;
        volatile UINT32 common_udma_config;
    } dma_common;
} PCIE_IClientUdma;

#endif /* AMBAPCIE_REG_H_ */
