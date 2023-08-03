/**
 *  @file StdXHCI_Bits.h
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
 *  @details Standard XHCI bits definitions header file.
 */

#ifndef STD_XHCI_BITS_H
#define STD_XHCI_BITS_H

#ifndef STD_USB_H
#include <StdUSB.h>
#endif

// XHCI CAPLENGTH & VERSION register
#define XHCI_LENGTH_VER_READ_MASK                      0xFFFFFFFFU
#define XHCI_LENGTH_VER_WRITE_MASK                     0xFFFFFFFFU
#define XHCI_LENGTH_VER_CAPLENGTH_MASK                 0x000000FFU
#define XHCI_LENGTH_VER_CAPLENGTH_SHIFT                         0U
#define XHCI_LENGTH_VER_CAPLENGTH_WIDTH                         8U
#define XHCI_LENGTH_VER_RSVD1_MASK                     0x0000FF00U
#define XHCI_LENGTH_VER_RSVD1_SHIFT                             8U
#define XHCI_LENGTH_VER_RSVD1_WIDTH                             8U
#define XHCI_LENGTH_VER_HCIVERSION_MASK                0xFFFF0000U
#define XHCI_LENGTH_VER_HCIVERSION_SHIFT                       16U
#define XHCI_LENGTH_VER_HCIVERSION_WIDTH                       16U

// XHCI HCSPARAMS1 register
#define XHCI_HCSPARAMS1_READ_MASK                                0xFFFFFFFFU
#define XHCI_HCSPARAMS1_WRITE_MASK                               0xFFFFFFFFU
#define XHCI_HCSPARAMS1_MAXSLOTS_MASK                            0x000000FFU
#define XHCI_HCSPARAMS1_MAXSLOTS_SHIFT                                    0U
#define XHCI_HCSPARAMS1_MAXSLOTS_WIDTH                                    8U
#define XHCI_HCSPARAMS1_MAXINTRS_MASK                            0x0007FF00U
#define XHCI_HCSPARAMS1_MAXINTRS_SHIFT                                    8U
#define XHCI_HCSPARAMS1_MAXINTRS_WIDTH                                   11U
#define XHCI_HCSPARAMS1_RSVD1_MASK                               0x00F80000U
#define XHCI_HCSPARAMS1_RSVD1_SHIFT                                      19U
#define XHCI_HCSPARAMS1_RSVD1_WIDTH                                       5U
#define XHCI_HCSPARAMS1_MAXPORTS_MASK                            0xFF000000U
#define XHCI_HCSPARAMS1_MAXPORTS_SHIFT                                   24U
#define XHCI_HCSPARAMS1_MAXPORTS_WIDTH                                    8U

// XHCI HCSPARAMS2 register
#define XHCI_HCSPARAMS2_READ_MASK                                0xFFFFFFFFU
#define XHCI_HCSPARAMS2_WRITE_MASK                               0xFFFFFFFFU
#define XHCI_HCSPARAMS2_IST_MASK                                 0x0000000FU
#define XHCI_HCSPARAMS2_IST_SHIFT                                         0U
#define XHCI_HCSPARAMS2_IST_WIDTH                                         4U
#define XHCI_HCSPARAMS2_ERSTMAX_MASK                             0x000000F0U
#define XHCI_HCSPARAMS2_ERSTMAX_SHIFT                                     4U
#define XHCI_HCSPARAMS2_ERSTMAX_WIDTH                                     4U
#define XHCI_HCSPARAMS2_RSVD1_MASK                               0x001FFF00U
#define XHCI_HCSPARAMS2_RSVD1_SHIFT                                       8U
#define XHCI_HCSPARAMS2_RSVD1_WIDTH                                      13U
#define XHCI_HCSPARAMS2_MAXSPBUFHI_MASK                          0x03E00000U
#define XHCI_HCSPARAMS2_MAXSPBUFHI_SHIFT                                 21U
#define XHCI_HCSPARAMS2_MAXSPBUFHI_WIDTH                                  5U
#define XHCI_HCSPARAMS2_SPR_MASK                                 0x04000000U
#define XHCI_HCSPARAMS2_SPR_SHIFT                                        26U
#define XHCI_HCSPARAMS2_SPR_WIDTH                                         1U
#define XHCI_HCSPARAMS2_SPR_WOCLR                                         0U
#define XHCI_HCSPARAMS2_SPR_WOSET                                         0U
#define XHCI_HCSPARAMS2_MAXSPBUFLO_MASK                          0xF8000000U
#define XHCI_HCSPARAMS2_MAXSPBUFLO_SHIFT                                 27U
#define XHCI_HCSPARAMS2_MAXSPBUFLO_WIDTH                                  5U

// XHCI HCSPARAMS3 register
#define XHCI_HCSPARAMS3_READ_MASK                                0xFFFFFFFFU
#define XHCI_HCSPARAMS3_WRITE_MASK                               0xFFFFFFFFU
#define XHCI_HCSPARAMS3_U1DEVEXITLAT_MASK                        0x000000FFU
#define XHCI_HCSPARAMS3_U1DEVEXITLAT_SHIFT                                0U
#define XHCI_HCSPARAMS3_U1DEVEXITLAT_WIDTH                                8U
#define XHCI_HCSPARAMS3_RSVD1_MASK                               0x0000FF00U
#define XHCI_HCSPARAMS3_RSVD1_SHIFT                                       8U
#define XHCI_HCSPARAMS3_RSVD1_WIDTH                                       8U
#define XHCI_HCSPARAMS3_U2DEVEXITLAT_MASK                        0xFFFF0000U
#define XHCI_HCSPARAMS3_U2DEVEXITLAT_SHIFT                               16U
#define XHCI_HCSPARAMS3_U2DEVEXITLAT_WIDTH                               16U

// XHCI HCCPARAMS1 register
#define XHCI_HCCPARAMS1_READ_MASK                                0xFFFFFFFFU
#define XHCI_HCCPARAMS1_WRITE_MASK                               0xFFFFFFFFU
#define XHCI_HCCPARAMS1_AC64_MASK                                0x00000001U
#define XHCI_HCCPARAMS1_AC64_SHIFT                                        0U
#define XHCI_HCCPARAMS1_AC64_WIDTH                                        1U
#define XHCI_HCCPARAMS1_AC64_WOCLR                                        0U
#define XHCI_HCCPARAMS1_AC64_WOSET                                        0U
#define XHCI_HCCPARAMS1_BNC_MASK                                 0x00000002U
#define XHCI_HCCPARAMS1_BNC_SHIFT                                         1U
#define XHCI_HCCPARAMS1_BNC_WIDTH                                         1U
#define XHCI_HCCPARAMS1_BNC_WOCLR                                         0U
#define XHCI_HCCPARAMS1_BNC_WOSET                                         0U
#define XHCI_HCCPARAMS1_CSZ_MASK                                 0x00000004U
#define XHCI_HCCPARAMS1_CSZ_SHIFT                                         2U
#define XHCI_HCCPARAMS1_CSZ_WIDTH                                         1U
#define XHCI_HCCPARAMS1_CSZ_WOCLR                                         0U
#define XHCI_HCCPARAMS1_CSZ_WOSET                                         0U
#define XHCI_HCCPARAMS1_PPC_MASK                                 0x00000008U
#define XHCI_HCCPARAMS1_PPC_SHIFT                                         3U
#define XHCI_HCCPARAMS1_PPC_WIDTH                                         1U
#define XHCI_HCCPARAMS1_PPC_WOCLR                                         0U
#define XHCI_HCCPARAMS1_PPC_WOSET                                         0U
#define XHCI_HCCPARAMS1_PIND_MASK                                0x00000010U
#define XHCI_HCCPARAMS1_PIND_SHIFT                                        4U
#define XHCI_HCCPARAMS1_PIND_WIDTH                                        1U
#define XHCI_HCCPARAMS1_PIND_WOCLR                                        0U
#define XHCI_HCCPARAMS1_PIND_WOSET                                        0U
#define XHCI_HCCPARAMS1_LHRC_MASK                                0x00000020U
#define XHCI_HCCPARAMS1_LHRC_SHIFT                                        5U
#define XHCI_HCCPARAMS1_LHRC_WIDTH                                        1U
#define XHCI_HCCPARAMS1_LHRC_WOCLR                                        0U
#define XHCI_HCCPARAMS1_LHRC_WOSET                                        0U
#define XHCI_HCCPARAMS1_LTC_MASK                                 0x00000040U
#define XHCI_HCCPARAMS1_LTC_SHIFT                                         6U
#define XHCI_HCCPARAMS1_LTC_WIDTH                                         1U
#define XHCI_HCCPARAMS1_LTC_WOCLR                                         0U
#define XHCI_HCCPARAMS1_LTC_WOSET                                         0U
#define XHCI_HCCPARAMS1_NSS_MASK                                 0x00000080U
#define XHCI_HCCPARAMS1_NSS_SHIFT                                         7U
#define XHCI_HCCPARAMS1_NSS_WIDTH                                         1U
#define XHCI_HCCPARAMS1_NSS_WOCLR                                         0U
#define XHCI_HCCPARAMS1_NSS_WOSET                                         0U
#define XHCI_HCCPARAMS1_PAE_MASK                                 0x00000100U
#define XHCI_HCCPARAMS1_PAE_SHIFT                                         8U
#define XHCI_HCCPARAMS1_PAE_WIDTH                                         1U
#define XHCI_HCCPARAMS1_PAE_WOCLR                                         0U
#define XHCI_HCCPARAMS1_PAE_WOSET                                         0U
#define XHCI_HCCPARAMS1_SPC_MASK                                 0x00000200U
#define XHCI_HCCPARAMS1_SPC_SHIFT                                         9U
#define XHCI_HCCPARAMS1_SPC_WIDTH                                         1U
#define XHCI_HCCPARAMS1_SPC_WOCLR                                         0U
#define XHCI_HCCPARAMS1_SPC_WOSET                                         0U
#define XHCI_HCCPARAMS1_SEC_MASK                                 0x00000400U
#define XHCI_HCCPARAMS1_SEC_SHIFT                                        10U
#define XHCI_HCCPARAMS1_SEC_WIDTH                                         1U
#define XHCI_HCCPARAMS1_SEC_WOCLR                                         0U
#define XHCI_HCCPARAMS1_SEC_WOSET                                         0U
#define XHCI_HCCPARAMS1_CFC_MASK                                 0x00000800U
#define XHCI_HCCPARAMS1_CFC_SHIFT                                        11U
#define XHCI_HCCPARAMS1_CFC_WIDTH                                         1U
#define XHCI_HCCPARAMS1_CFC_WOCLR                                         0U
#define XHCI_HCCPARAMS1_CFC_WOSET                                         0U
#define XHCI_HCCPARAMS1_MAXPSASIZE_MASK                          0x0000F000U
#define XHCI_HCCPARAMS1_MAXPSASIZE_SHIFT                                 12U
#define XHCI_HCCPARAMS1_MAXPSASIZE_WIDTH                                  4U
#define XHCI_HCCPARAMS1_XECP_MASK                                0xFFFF0000U
#define XHCI_HCCPARAMS1_XECP_SHIFT                                       16U
#define XHCI_HCCPARAMS1_XECP_WIDTH                                       16U

// XHCI DBOFF register
#define XHCI_DBOFF_READ_MASK                                     0xFFFFFFFFU
#define XHCI_DBOFF_WRITE_MASK                                    0xFFFFFFFFU
#define XHCI_DBOFF_RSVD1_MASK                                    0x00000003U
#define XHCI_DBOFF_RSVD1_SHIFT                                            0U
#define XHCI_DBOFF_RSVD1_WIDTH                                            2U
#define XHCI_DBOFF_DAO_MASK                                      0xFFFFFFFCU
#define XHCI_DBOFF_DAO_SHIFT                                              2U
#define XHCI_DBOFF_DAO_WIDTH                                             30U

// XHCI RTSOFF register
#define XHCI_RTSOFF_READ_MASK                                    0xFFFFFFFFU
#define XHCI_RTSOFF_WRITE_MASK                                   0xFFFFFFFFU
#define XHCI_RTSOFF_RSVD1_MASK                                   0x0000001FU
#define XHCI_RTSOFF_RSVD1_SHIFT                                           0U
#define XHCI_RTSOFF_RSVD1_WIDTH                                           5U
#define XHCI_RTSOFF_RRSO_MASK                                    0xFFFFFFE0U
#define XHCI_RTSOFF_RRSO_SHIFT                                            5U
#define XHCI_RTSOFF_RRSO_WIDTH                                           27U

// XHCI HCCPARAMS2 register
#define XHCI_HCCPARAMS2_READ_MASK                                0xFFFFFFFFU
#define XHCI_HCCPARAMS2_WRITE_MASK                               0xFFFFFFFFU
#define XHCI_HCCPARAMS2_U3C_MASK                                 0x00000001U
#define XHCI_HCCPARAMS2_U3C_SHIFT                                         0U
#define XHCI_HCCPARAMS2_U3C_WIDTH                                         1U
#define XHCI_HCCPARAMS2_U3C_WOCLR                                         0U
#define XHCI_HCCPARAMS2_U3C_WOSET                                         0U
#define XHCI_HCCPARAMS2_CMC_MASK                                 0x00000002U
#define XHCI_HCCPARAMS2_CMC_SHIFT                                         1U
#define XHCI_HCCPARAMS2_CMC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_CMC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_CMC_WOSET                                         0U
#define XHCI_HCCPARAMS2_FSC_MASK                                 0x00000004U
#define XHCI_HCCPARAMS2_FSC_SHIFT                                         2U
#define XHCI_HCCPARAMS2_FSC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_FSC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_FSC_WOSET                                         0U
#define XHCI_HCCPARAMS2_CTC_MASK                                 0x00000008U
#define XHCI_HCCPARAMS2_CTC_SHIFT                                         3U
#define XHCI_HCCPARAMS2_CTC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_CTC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_CTC_WOSET                                         0U
#define XHCI_HCCPARAMS2_LEC_MASK                                 0x00000010U
#define XHCI_HCCPARAMS2_LEC_SHIFT                                         4U
#define XHCI_HCCPARAMS2_LEC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_LEC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_LEC_WOSET                                         0U
#define XHCI_HCCPARAMS2_CIC_MASK                                 0x00000020U
#define XHCI_HCCPARAMS2_CIC_SHIFT                                         5U
#define XHCI_HCCPARAMS2_CIC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_CIC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_CIC_WOSET                                         0U
#define XHCI_HCCPARAMS2_ETC_MASK                                 0x00000040U
#define XHCI_HCCPARAMS2_ETC_SHIFT                                         6U
#define XHCI_HCCPARAMS2_ETC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_ETC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_ETC_WOSET                                         0U
#define XHCI_HCCPARAMS2_ETC_TSC_MASK                             0x00000080U
#define XHCI_HCCPARAMS2_ETC_TSC_SHIFT                                     7U
#define XHCI_HCCPARAMS2_ETC_TSC_WIDTH                                     1U
#define XHCI_HCCPARAMS2_ETC_TSC_WOCLR                                     0U
#define XHCI_HCCPARAMS2_ETC_TSC_WOSET                                     0U
#define XHCI_HCCPARAMS2_GSC_MASK                                 0x00000100U
#define XHCI_HCCPARAMS2_GSC_SHIFT                                         8U
#define XHCI_HCCPARAMS2_GSC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_GSC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_GSC_WOSET                                         0U
#define XHCI_HCCPARAMS2_VTC_MASK                                 0x00000200U
#define XHCI_HCCPARAMS2_VTC_SHIFT                                         9U
#define XHCI_HCCPARAMS2_VTC_WIDTH                                         1U
#define XHCI_HCCPARAMS2_VTC_WOCLR                                         0U
#define XHCI_HCCPARAMS2_VTC_WOSET                                         0U
#define XHCI_HCCPARAMS2_RSVD3_MASK                               0xFFFFFC00U
#define XHCI_HCCPARAMS2_RSVD3_SHIFT                                      10U
#define XHCI_HCCPARAMS2_RSVD3_WIDTH                                      22U

// XHCI USBCMD register
#define XHCI_USBCMD_READ_MASK                                    0xFFFFFFFFU
#define XHCI_USBCMD_WRITE_MASK                                   0xFFFFFFFFU
#define XHCI_USBCMD_R_S_MASK                                     0x00000001U
#define XHCI_USBCMD_R_S_SHIFT                                             0U
#define XHCI_USBCMD_R_S_WIDTH                                             1U
#define XHCI_USBCMD_R_S_WOCLR                                             0U
#define XHCI_USBCMD_R_S_WOSET                                             0U
#define XHCI_USBCMD_HCRST_MASK                                   0x00000002U
#define XHCI_USBCMD_HCRST_SHIFT                                           1U
#define XHCI_USBCMD_HCRST_WIDTH                                           1U
#define XHCI_USBCMD_HCRST_WOCLR                                           0U
#define XHCI_USBCMD_HCRST_WOSET                                           0U
#define XHCI_USBCMD_INTE_MASK                                    0x00000004U
#define XHCI_USBCMD_INTE_SHIFT                                            2U
#define XHCI_USBCMD_INTE_WIDTH                                            1U
#define XHCI_USBCMD_INTE_WOCLR                                            0U
#define XHCI_USBCMD_INTE_WOSET                                            0U
#define XHCI_USBCMD_HSEE_MASK                                    0x00000008U
#define XHCI_USBCMD_HSEE_SHIFT                                            3U
#define XHCI_USBCMD_HSEE_WIDTH                                            1U
#define XHCI_USBCMD_HSEE_WOCLR                                            0U
#define XHCI_USBCMD_HSEE_WOSET                                            0U
#define XHCI_USBCMD_RSVDP1_MASK                                  0x00000070U
#define XHCI_USBCMD_RSVDP1_SHIFT                                          4U
#define XHCI_USBCMD_RSVDP1_WIDTH                                          3U
#define XHCI_USBCMD_LHCRST_MASK                                  0x00000080U
#define XHCI_USBCMD_LHCRST_SHIFT                                          7U
#define XHCI_USBCMD_LHCRST_WIDTH                                          1U
#define XHCI_USBCMD_LHCRST_WOCLR                                          0U
#define XHCI_USBCMD_LHCRST_WOSET                                          0U
#define XHCI_USBCMD_CSS_MASK                                     0x00000100U
#define XHCI_USBCMD_CSS_SHIFT                                             8U
#define XHCI_USBCMD_CSS_WIDTH                                             1U
#define XHCI_USBCMD_CSS_WOCLR                                             0U
#define XHCI_USBCMD_CSS_WOSET                                             0U
#define XHCI_USBCMD_CRS_MASK                                     0x00000200U
#define XHCI_USBCMD_CRS_SHIFT                                             9U
#define XHCI_USBCMD_CRS_WIDTH                                             1U
#define XHCI_USBCMD_CRS_WOCLR                                             0U
#define XHCI_USBCMD_CRS_WOSET                                             0U
#define XHCI_USBCMD_EWE_MASK                                     0x00000400U
#define XHCI_USBCMD_EWE_SHIFT                                            10U
#define XHCI_USBCMD_EWE_WIDTH                                             1U
#define XHCI_USBCMD_EWE_WOCLR                                             0U
#define XHCI_USBCMD_EWE_WOSET                                             0U
#define XHCI_USBCMD_EU3S_MASK                                    0x00000800U
#define XHCI_USBCMD_EU3S_SHIFT                                           11U
#define XHCI_USBCMD_EU3S_WIDTH                                            1U
#define XHCI_USBCMD_EU3S_WOCLR                                            0U
#define XHCI_USBCMD_EU3S_WOSET                                            0U
#define XHCI_USBCMD_RSVDP2_MASK                                  0x00001000U
#define XHCI_USBCMD_RSVDP2_SHIFT                                         12U
#define XHCI_USBCMD_RSVDP2_WIDTH                                          1U
#define XHCI_USBCMD_RSVDP2_WOCLR                                          0U
#define XHCI_USBCMD_RSVDP2_WOSET                                          0U
#define XHCI_USBCMD_CME_MASK                                     0x00002000U
#define XHCI_USBCMD_CME_SHIFT                                            13U
#define XHCI_USBCMD_CME_WIDTH                                             1U
#define XHCI_USBCMD_CME_WOCLR                                             0U
#define XHCI_USBCMD_CME_WOSET                                             0U
#define XHCI_USBCMD_ETE_MASK                                     0x00004000U
#define XHCI_USBCMD_ETE_SHIFT                                            14U
#define XHCI_USBCMD_ETE_WIDTH                                             1U
#define XHCI_USBCMD_ETE_WOCLR                                             0U
#define XHCI_USBCMD_ETE_WOSET                                             0U
#define XHCI_USBCMD_TSC_EN_MASK                                  0x00008000U
#define XHCI_USBCMD_TSC_EN_SHIFT                                         15U
#define XHCI_USBCMD_TSC_EN_WIDTH                                          1U
#define XHCI_USBCMD_TSC_EN_WOCLR                                          0U
#define XHCI_USBCMD_TSC_EN_WOSET                                          0U
#define XHCI_USBCMD_RSVDP3_MASK                                  0xFFFF0000U
#define XHCI_USBCMD_RSVDP3_SHIFT                                         16U
#define XHCI_USBCMD_RSVDP3_WIDTH                                         16U
#define XHCI_D_USBCMD_RSVDP3_MASK                                0x00010000U
#define XHCI_D_USBCMD_RSVDP3_SHIFT                                       16U
#define XHCI_D_USBCMD_RSVDP3_WIDTH                                        1U
#define XHCI_D_USBCMD_RSVDP3_WOCLR                                        0U
#define XHCI_D_USBCMD_RSVDP3_WOSET                                        0U
#define XHCI_D_USBCMD_DEVEN_MASK                                 0x00020000U
#define XHCI_D_USBCMD_DEVEN_SHIFT                                        17U
#define XHCI_D_USBCMD_DEVEN_WIDTH                                         1U
#define XHCI_D_USBCMD_DEVEN_WOCLR                                         0U
#define XHCI_D_USBCMD_DEVEN_WOSET                                         0U
#define XHCI_D_USBCMD_RSVDP4_MASK                                0xFFFC0000U
#define XHCI_D_USBCMD_RSVDP4_SHIFT                                       18U
#define XHCI_D_USBCMD_RSVDP4_WIDTH                                       14U

// XHCI USBSTS register
#define XHCI_USBSTS_READ_MASK                                    0xFFFFFFFFU
#define XHCI_USBSTS_WRITE_MASK                                   0xFFFFFFFFU
#define XHCI_USBSTS_WOCLR_MASK                                   0x0000041CU
#define XHCI_USBSTS_HCH_MASK                                     0x00000001U
#define XHCI_USBSTS_HCH_SHIFT                                             0U
#define XHCI_USBSTS_HCH_WIDTH                                             1U
#define XHCI_USBSTS_HCH_WOCLR                                             0U
#define XHCI_USBSTS_HCH_WOSET                                             0U
#define XHCI_USBSTS_RSVDZ1_MASK                                  0x00000002U
#define XHCI_USBSTS_RSVDZ1_SHIFT                                          1U
#define XHCI_USBSTS_RSVDZ1_WIDTH                                          1U
#define XHCI_USBSTS_RSVDZ1_WOCLR                                          0U
#define XHCI_USBSTS_RSVDZ1_WOSET                                          0U
#define XHCI_USBSTS_HSE_MASK                                     0x00000004U
#define XHCI_USBSTS_HSE_SHIFT                                             2U
#define XHCI_USBSTS_HSE_WIDTH                                             1U
#define XHCI_USBSTS_HSE_WOCLR                                             1U
#define XHCI_USBSTS_HSE_WOSET                                             0U
#define XHCI_USBSTS_EINT_MASK                                    0x00000008U
#define XHCI_USBSTS_EINT_SHIFT                                            3U
#define XHCI_USBSTS_EINT_WIDTH                                            1U
#define XHCI_USBSTS_EINT_WOCLR                                            1U
#define XHCI_USBSTS_EINT_WOSET                                            0U
#define XHCI_USBSTS_PCD_MASK                                     0x00000010U
#define XHCI_USBSTS_PCD_SHIFT                                             4U
#define XHCI_USBSTS_PCD_WIDTH                                             1U
#define XHCI_USBSTS_PCD_WOCLR                                             1U
#define XHCI_USBSTS_PCD_WOSET                                             0U
#define XHCI_USBSTS_RSVDZ2_MASK                                  0x000000E0U
#define XHCI_USBSTS_RSVDZ2_SHIFT                                          5U
#define XHCI_USBSTS_RSVDZ2_WIDTH                                          3U
#define XHCI_USBSTS_SSS_MASK                                     0x00000100U
#define XHCI_USBSTS_SSS_SHIFT                                             8U
#define XHCI_USBSTS_SSS_WIDTH                                             1U
#define XHCI_USBSTS_SSS_WOCLR                                             0U
#define XHCI_USBSTS_SSS_WOSET                                             0U
#define XHCI_USBSTS_RSS_MASK                                     0x00000200U
#define XHCI_USBSTS_RSS_SHIFT                                             9U
#define XHCI_USBSTS_RSS_WIDTH                                             1U
#define XHCI_USBSTS_RSS_WOCLR                                             0U
#define XHCI_USBSTS_RSS_WOSET                                             0U
#define XHCI_USBSTS_SRE_MASK                                     0x00000400U
#define XHCI_USBSTS_SRE_SHIFT                                            10U
#define XHCI_USBSTS_SRE_WIDTH                                             1U
#define XHCI_USBSTS_SRE_WOCLR                                             1U
#define XHCI_USBSTS_SRE_WOSET                                             0U
#define XHCI_USBSTS_CNR_MASK                                     0x00000800U
#define XHCI_USBSTS_CNR_SHIFT                                            11U
#define XHCI_USBSTS_CNR_WIDTH                                             1U
#define XHCI_USBSTS_CNR_WOCLR                                             0U
#define XHCI_USBSTS_CNR_WOSET                                             0U
#define XHCI_USBSTS_HCE_MASK                                     0x00001000U
#define XHCI_USBSTS_HCE_SHIFT                                            12U
#define XHCI_USBSTS_HCE_WIDTH                                             1U
#define XHCI_USBSTS_HCE_WOCLR                                             0U
#define XHCI_USBSTS_HCE_WOSET                                             0U
#define XHCI_USBSTS_RSVDP_MASK                                   0xFFFFE000U
#define XHCI_USBSTS_RSVDP_SHIFT                                          13U
#define XHCI_USBSTS_RSVDP_WIDTH                                          19U

// XHCI portsc registers
#define XHCI_PORTSC_READ_MASK                               0xFFFFFFFFU
#define XHCI_PORTSC_WRITE_MASK                              0xFFFFFFFFU
#define XHCI_PORTSC_WOSET_MASK                              0x80000010U
#define XHCI_PORTSC_WOCLR_MASK                              0x00FE0002U
#define XHCI_PORTSC_CCS_MASK                                0x00000001U
#define XHCI_PORTSC_CCS_SHIFT                                        0U
#define XHCI_PORTSC_CCS_WIDTH                                        1U
#define XHCI_PORTSC_CCS_WOCLR                                        0U
#define XHCI_PORTSC_CCS_WOSET                                        0U
#define XHCI_PORTSC_PED_MASK                                0x00000002U
#define XHCI_PORTSC_PED_SHIFT                                        1U
#define XHCI_PORTSC_PED_WIDTH                                        1U
#define XHCI_PORTSC_PED_WOCLR                                        1U
#define XHCI_PORTSC_PED_WOSET                                        0U
#define XHCI_PORTSC_RSVDZ1_MASK                             0x00000004U
#define XHCI_PORTSC_RSVDZ1_SHIFT                                     2U
#define XHCI_PORTSC_RSVDZ1_WIDTH                                     1U
#define XHCI_PORTSC_RSVDZ1_WOCLR                                     0U
#define XHCI_PORTSC_RSVDZ1_WOSET                                     0U
#define XHCI_PORTSC_OCA_MASK                                0x00000008U
#define XHCI_PORTSC_OCA_SHIFT                                        3U
#define XHCI_PORTSC_OCA_WIDTH                                        1U
#define XHCI_PORTSC_OCA_WOCLR                                        0U
#define XHCI_PORTSC_OCA_WOSET                                        0U
#define XHCI_PORTSC_PR_MASK                                 0x00000010U
#define XHCI_PORTSC_PR_SHIFT                                         4U
#define XHCI_PORTSC_PR_WIDTH                                         1U
#define XHCI_PORTSC_PR_WOCLR                                         0U
#define XHCI_PORTSC_PR_WOSET                                         1U
#define XHCI_PORTSC_PLS_MASK                                0x000001E0U
#define XHCI_PORTSC_PLS_SHIFT                               ((UINT32)5U)
#define XHCI_PORTSC_PLS_WIDTH                                        4U
#define XHCI_PORTSC_PP_MASK                                 0x00000200U
#define XHCI_PORTSC_PP_SHIFT                                         9U
#define XHCI_PORTSC_PP_WIDTH                                         1U
#define XHCI_PORTSC_PP_WOCLR                                         0U
#define XHCI_PORTSC_PP_WOSET                                         0U
#define XHCI_PORTSC_PORTSPEED_MASK                          0x00003C00U
#define XHCI_PORTSC_PORTSPEED_SHIFT                                 10U
#define XHCI_PORTSC_PORTSPEED_WIDTH                                  4U
#define XHCI_PORTSC_PIC_MASK                                0x0000C000U
#define XHCI_PORTSC_PIC_SHIFT                                       14U
#define XHCI_PORTSC_PIC_WIDTH                                        2U
#define XHCI_PORTSC_LWS_MASK                                0x00010000U
#define XHCI_PORTSC_LWS_SHIFT                                       16U
#define XHCI_PORTSC_LWS_WIDTH                                        1U
#define XHCI_PORTSC_LWS_WOCLR                                        0U
#define XHCI_PORTSC_LWS_WOSET                                        0U
#define XHCI_PORTSC_CSC_MASK                                0x00020000U
#define XHCI_PORTSC_CSC_SHIFT                                       17U
#define XHCI_PORTSC_CSC_WIDTH                                        1U
#define XHCI_PORTSC_CSC_WOCLR                                        1U
#define XHCI_PORTSC_CSC_WOSET                                        0U
#define XHCI_PORTSC_PEC_MASK                                0x00040000U
#define XHCI_PORTSC_PEC_SHIFT                                       18U
#define XHCI_PORTSC_PEC_WIDTH                                        1U
#define XHCI_PORTSC_PEC_WOCLR                                        1U
#define XHCI_PORTSC_PEC_WOSET                                        0U
#define XHCI_PORTSC_WRC_MASK                                0x00080000U
#define XHCI_PORTSC_WRC_SHIFT                                       19U
#define XHCI_PORTSC_WRC_WIDTH                                        1U
#define XHCI_PORTSC_WRC_WOCLR                                        1U
#define XHCI_PORTSC_WRC_WOSET                                        0U
#define XHCI_PORTSC_OCC_MASK                                0x00100000U
#define XHCI_PORTSC_OCC_SHIFT                                       20U
#define XHCI_PORTSC_OCC_WIDTH                                        1U
#define XHCI_PORTSC_OCC_WOCLR                                        1U
#define XHCI_PORTSC_OCC_WOSET                                        0U
#define XHCI_PORTSC_PRC_MASK                                0x00200000U
#define XHCI_PORTSC_PRC_SHIFT                                       21U
#define XHCI_PORTSC_PRC_WIDTH                                        1U
#define XHCI_PORTSC_PRC_WOCLR                                        1U
#define XHCI_PORTSC_PRC_WOSET                                        0U
#define XHCI_PORTSC_PLC_MASK                                0x00400000U
#define XHCI_PORTSC_PLC_SHIFT                                       22U
#define XHCI_PORTSC_PLC_WIDTH                                        1U
#define XHCI_PORTSC_PLC_WOCLR                                        1U
#define XHCI_PORTSC_PLC_WOSET                                        0U
#define XHCI_PORTSC_CEC_MASK                                0x00800000U
#define XHCI_PORTSC_CEC_SHIFT                                       23U
#define XHCI_PORTSC_CEC_WIDTH                                        1U
#define XHCI_PORTSC_CEC_WOCLR                                        1U
#define XHCI_PORTSC_CEC_WOSET                                        0U
#define XHCI_PORTSC_CAS_MASK                                0x01000000U
#define XHCI_PORTSC_CAS_SHIFT                                       24U
#define XHCI_PORTSC_CAS_WIDTH                                        1U
#define XHCI_PORTSC_CAS_WOCLR                                        0U
#define XHCI_PORTSC_CAS_WOSET                                        0U
#define XHCI_PORTSC_WCE_MASK                                0x02000000U
#define XHCI_PORTSC_WCE_SHIFT                                       25U
#define XHCI_PORTSC_WCE_WIDTH                                        1U
#define XHCI_PORTSC_WCE_WOCLR                                        0U
#define XHCI_PORTSC_WCE_WOSET                                        0U
#define XHCI_PORTSC_WDE_MASK                                0x04000000U
#define XHCI_PORTSC_WDE_SHIFT                                       26U
#define XHCI_PORTSC_WDE_WIDTH                                        1U
#define XHCI_PORTSC_WDE_WOCLR                                        0U
#define XHCI_PORTSC_WDE_WOSET                                        0U
#define XHCI_PORTSC_WOE_MASK                                0x08000000U
#define XHCI_PORTSC_WOE_SHIFT                                       27U
#define XHCI_PORTSC_WOE_WIDTH                                        1U
#define XHCI_PORTSC_WOE_WOCLR                                        0U
#define XHCI_PORTSC_WOE_WOSET                                        0U
#define XHCI_PORTSC_RSVDZ2_MASK                             0x30000000U
#define XHCI_PORTSC_RSVDZ2_SHIFT                                    28U
#define XHCI_PORTSC_RSVDZ2_WIDTH                                     2U
#define XHCI_PORTSC_DR_MASK                                 0x40000000U
#define XHCI_PORTSC_DR_SHIFT                                        30U
#define XHCI_PORTSC_DR_WIDTH                                         1U
#define XHCI_PORTSC_DR_WOCLR                                         0U
#define XHCI_PORTSC_DR_WOSET                                         0U
#define XHCI_PORTSC_WPR_MASK                                0x80000000U
#define XHCI_PORTSC_WPR_SHIFT                                       31U
#define XHCI_PORTSC_WPR_WIDTH                                        1U
#define XHCI_PORTSC_WPR_WOCLR                                        0U
#define XHCI_PORTSC_WPR_WOSET                                        1U

// XHCI portpmsc registers (USB2)
#define XHCI_PORTPMSC1USB2_READ_MASK                             0xFFFFFFFFU
#define XHCI_PORTPMSC1USB2_WRITE_MASK                            0xFFFFFFFFU
#define XHCI_PORTPMSC1USB2_L1S_MASK                              0x00000007U
#define XHCI_PORTPMSC1USB2_L1S_SHIFT                                      0U
#define XHCI_PORTPMSC1USB2_L1S_WIDTH                                      3U
#define XHCI_PORTPMSC1USB2_RWE_MASK                              0x00000008U
#define XHCI_PORTPMSC1USB2_RWE_SHIFT                                      3U
#define XHCI_PORTPMSC1USB2_RWE_WIDTH                                      1U
#define XHCI_PORTPMSC1USB2_RWE_WOCLR                                      0U
#define XHCI_PORTPMSC1USB2_RWE_WOSET                                      0U
#define XHCI_PORTPMSC1USB2_BESL_MASK                             0x000000F0U
#define XHCI_PORTPMSC1USB2_BESL_SHIFT                                     4U
#define XHCI_PORTPMSC1USB2_BESL_WIDTH                                     4U
#define XHCI_PORTPMSC1USB2_L1DS_MASK                             0x0000FF00U
#define XHCI_PORTPMSC1USB2_L1DS_SHIFT                                     8U
#define XHCI_PORTPMSC1USB2_L1DS_WIDTH                                     8U
#define XHCI_PORTPMSC1USB2_HLE_MASK                              0x00010000U
#define XHCI_PORTPMSC1USB2_HLE_SHIFT                                     16U
#define XHCI_PORTPMSC1USB2_HLE_WIDTH                                      1U
#define XHCI_PORTPMSC1USB2_HLE_WOCLR                                      0U
#define XHCI_PORTPMSC1USB2_HLE_WOSET                                      0U
#define XHCI_PORTPMSC1USB2_RSVD2_MASK                            0x0FFE0000U
#define XHCI_PORTPMSC1USB2_RSVD2_SHIFT                                   17U
#define XHCI_PORTPMSC1USB2_RSVD2_WIDTH                                   11U
#define XHCI_PORTPMSC1USB2_PTC_MASK                              0xF0000000U
#define XHCI_PORTPMSC1USB2_PTC_SHIFT                                     28U
#define XHCI_PORTPMSC1USB2_PTC_WIDTH                                      4U


// XHCI crcr register
#define XHCI_CRCR_LO_READ_MASK                                   0xFFFFFFFFU
#define XHCI_CRCR_LO_WRITE_MASK                                  0xFFFFFFFFU
#define XHCI_CRCR_LO_WOSET_MASK                                  0x00000006U
#define XHCI_CRCR_LO_RCS_MASK                                    0x00000001U
#define XHCI_CRCR_LO_RCS_SHIFT                                            0U
#define XHCI_CRCR_LO_RCS_WIDTH                                            1U
#define XHCI_CRCR_LO_RCS_WOCLR                                            0U
#define XHCI_CRCR_LO_RCS_WOSET                                            0U
#define XHCI_CRCR_LO_CS_MASK                                     0x00000002U
#define XHCI_CRCR_LO_CS_SHIFT                                             1U
#define XHCI_CRCR_LO_CS_WIDTH                                             1U
#define XHCI_CRCR_LO_CS_WOCLR                                             0U
#define XHCI_CRCR_LO_CS_WOSET                                             1U
#define XHCI_CRCR_LO_CA_MASK                                     0x00000004U
#define XHCI_CRCR_LO_CA_SHIFT                                             2U
#define XHCI_CRCR_LO_CA_WIDTH                                             1U
#define XHCI_CRCR_LO_CA_WOCLR                                             0U
#define XHCI_CRCR_LO_CA_WOSET                                             1U
#define XHCI_CRCR_LO_CRR_MASK                                    0x00000008U
#define XHCI_CRCR_LO_CRR_SHIFT                                            3U
#define XHCI_CRCR_LO_CRR_WIDTH                                            1U
#define XHCI_CRCR_LO_CRR_WOCLR                                            0U
#define XHCI_CRCR_LO_CRR_WOSET                                            0U
#define XHCI_CRCR_LO_RSVD1_MASK                                  0x00000030U
#define XHCI_CRCR_LO_RSVD1_SHIFT                                          4U
#define XHCI_CRCR_LO_RSVD1_WIDTH                                          2U
#define XHCI_CRCR_LO_CRPTR_L_MASK                                0xFFFFFFC0U
#define XHCI_CRCR_LO_CRPTR_L_SHIFT                                        6U
#define XHCI_CRCR_LO_CRPTR_L_WIDTH                                       26U

#define XHCI_CRCR_HI_READ_MASK                                    0xFFFFFFFFU
#define XHCI_CRCR_HI_WRITE_MASK                                   0xFFFFFFFFU
#define XHCI_CRCR_HI_CRPTR_H_MASK                                 0xFFFFFFFFU
#define XHCI_CRCR_HI_CRPTR_H_SHIFT                                         0U
#define XHCI_CRCR_HI_CRPTR_H_WIDTH                                        32U

// XHCI Operation->config register
#define XHCI_CONFIG_READ_MASK                                    0xFFFFFFFFU
#define XHCI_CONFIG_WRITE_MASK                                   0xFFFFFFFFU
#define XHCI_CONFIG_MAXSLOTSEN_MASK                              0x000000FFU
#define XHCI_CONFIG_MAXSLOTSEN_SHIFT                                      0U
#define XHCI_CONFIG_MAXSLOTSEN_WIDTH                                      8U
#define XHCI_CONFIG_U3E_MASK                                     0x00000100U
#define XHCI_CONFIG_U3E_SHIFT                                             8U
#define XHCI_CONFIG_U3E_WIDTH                                             1U
#define XHCI_CONFIG_U3E_WOCLR                                             0U
#define XHCI_CONFIG_U3E_WOSET                                             0U
#define XHCI_CONFIG_CIE_MASK                                     0x00000200U
#define XHCI_CONFIG_CIE_SHIFT                                             9U
#define XHCI_CONFIG_CIE_WIDTH                                             1U
#define XHCI_CONFIG_CIE_WOCLR                                             0U
#define XHCI_CONFIG_CIE_WOSET                                             0U
#define XHCI_CONFIG_RSVDP1_MASK                                  0xFFFFFC00U
#define XHCI_CONFIG_RSVDP1_SHIFT                                         10U
#define XHCI_CONFIG_RSVDP1_WIDTH                                         22U

// XHCI Interrupt Management register
#define XHCI_IMAN0_READ_MASK                                     0xFFFFFFFFU
#define XHCI_IMAN0_WRITE_MASK                                    0xFFFFFFFFU
#define XHCI_IMAN0_WOCLR_MASK                                    0x00000001U
#define XHCI_IMAN0_IP_MASK                                       0x00000001U
#define XHCI_IMAN0_IP_SHIFT                                               0U
#define XHCI_IMAN0_IP_WIDTH                                               1U
#define XHCI_IMAN0_IP_WOCLR                                               1U
#define XHCI_IMAN0_IP_WOSET                                               0U
#define XHCI_IMAN0_IE_MASK                                       0x00000002U
#define XHCI_IMAN0_IE_SHIFT                                               1U
#define XHCI_IMAN0_IE_WIDTH                                               1U
#define XHCI_IMAN0_IE_WOCLR                                               0U
#define XHCI_IMAN0_IE_WOSET                                               0U
#define XHCI_IMAN0_RSVDP1_MASK                                   0xFFFFFFFCU
#define XHCI_IMAN0_RSVDP1_SHIFT                                           2U
#define XHCI_IMAN0_RSVDP1_WIDTH                                          30U

// XHCI Interrupt Moderation register
#define XHCI_IMOD0_READ_MASK                                   0xFFFFFFFFU
#define XHCI_IMOD0_WRITE_MASK                                  0xFFFFFFFFU
#define XHCI_IMOD0_IMODI_MASK                                  0x0000FFFFU
#define XHCI_IMOD0_IMODI_SHIFT                                          0U
#define XHCI_IMOD0_IMODI_WIDTH                                         16U
#define XHCI_IMOD0_IMODC_MASK                                  0xFFFF0000U
#define XHCI_IMOD0_IMODC_SHIFT                                         16U
#define XHCI_IMOD0_IMODC_WIDTH                                         16U


// XHCI Event Ring Dequeue Pointer register
#define XHCI_ERDP0_LO_READ_MASK                                  0xFFFFFFFFU
#define XHCI_ERDP0_LO_WRITE_MASK                                 0xFFFFFFFFU
#define XHCI_ERDP0_LO_WOCLR_MASK                                 0x00000008U
#define XHCI_ERDP0_LO_DESI_MASK                                  0x00000007U
#define XHCI_ERDP0_LO_DESI_SHIFT                                          0U
#define XHCI_ERDP0_LO_DESI_WIDTH                                          3U
#define XHCI_ERDP0_LO_EHB_MASK                                   0x00000008U
#define XHCI_ERDP0_LO_EHB_SHIFT                                           3U
#define XHCI_ERDP0_LO_EHB_WIDTH                                           1U
#define XHCI_ERDP0_LO_EHB_WOCLR                                           1U
#define XHCI_ERDP0_LO_EHB_WOSET                                           0U
#define XHCI_ERDP0_LO_ERDPTR_MASK                                0xFFFFFFF0U
#define XHCI_ERDP0_LO_ERDPTR_SHIFT                                        4U
#define XHCI_ERDP0_LO_ERDPTR_WIDTH                                       28U

#define XHCI_ERDP0_HI_READ_MASK                                0xFFFFFFFFU
#define XHCI_ERDP0_HI_WRITE_MASK                               0xFFFFFFFFU
#define XHCI_ERDP0_HI_ERDPTR_HI_MASK                           0xFFFFFFFFU
#define XHCI_ERDP0_HI_ERDPTR_HI_SHIFT                                   0U
#define XHCI_ERDP0_HI_ERDPTR_HI_WIDTH                                  32U

//
// Isoch TRB
//
#define    XHCI_TRB_TDSIZE_TBC_POS              ((UINT32)17U)         //!< position of TD size/TBC (DWORD 2)
#define    XHCI_TRB_TDSIZE_TBC_MASK             ((UINT32)0x3E0000U)   //!< mask of TD size/TBC     (DWORD 2)
#define    XHCI_TRB_TBC_TBSTS_POS               ((UINT32)7U)          //!< position of Transfer Burst Count (DWORD 3)
#define    XHCI_TRB_TBC_TBSTS_MASK              ((UINT32)0x180U)      //!< mask of Transfer Burst Count (DWORD 3)
#define    XHCI_TRB_ISOCH_FRAME_ID_POS          ((UINT32)20U)         //!< position of Frame ID    (DWORD 3)
#define    XHCI_TRB_ISOCH_FRAME_ID_MASK         ((UINT32)0x7FF00000U) //!< mask of Frame ID        (DWORD 3)
#define    XHCI_TRB_ISOCH_SIA_POS               ((UINT32)31U)         //!< position of Start Isoch ASAP (DWORD 3)
#define    XHCI_TRB_ISOCH_SIA_MASK              ((UINT32)0x80000000U) //!< mask of Start Isoch ASAP     (DWORD 3)

//
// Slot Context
//
#define    XHCI_SLOTCTX_SPEED_POS          (20U) //!< position of Speed                (DWORD 0)
#define    XHCI_SLOTCTX_ENTRY_POS          (27U) //!< position of Context Entries      (DWORD 0)
#define    XHCI_SLOTCTX_RHPORT_NUM_POS     (16U) //!< position of Root Hub Port Number (DWORD 1)
#define    XHCI_SLOTCTX_NUM_PORTS_POS      (24U) //!< position of Number of Ports      (DWORD 1)
#define    XHCI_SLOTCTX_STATE_POS          (27U) //!< position of Slot State           (DWORD 3)

//
// Endpoint Context
//
#define    XHCI_EPCTX_STATE_MASK         ((UINT32)7U) //!< mask of EP State            (DWORD 0)
#define    XHCI_EPCTX_MULT_POS                     8U //!< position of Mult            (DWORD 0)
#define    XHCI_EPCTX_MULT_MASK               0x300UL //!< mask of Mult                (DWORD 0)
#define    XHCI_EPCTX_PMAXSTREAMS_POS             10U //!< position of MaxPStreams     (DWORD 0)
#define    XHCI_EPCTX_INTERVAL_POS                16U //!< position of Interval        (DWORD 0)
#define    XHCI_EPCTX_MAXESITPLD_HI_POS           24U //!< position of Max ESIT Payload Hi (DWORD 0)
#define    XHCI_EPCTX_CERR_POS           ((UINT32)1U) //!< position of Error Count     (DWORD 1)
#define    XHCI_EPCTX_CERR_MASK                 0x6UL //!< mask of Error Count         (DWORD 1)
#define    XHCI_EPCTX_3ERR               ((UINT32)3U) //!< value (3) of CERR           (DWORD 1)
#define    XHCI_EPCTX_MAX_BURST_SZ_POS             8U //!< position of Max Burst Size  (DWORD 1)
#define    XHCI_EPCTX_MAX_BURST_SZ_MASK       0xFF00U //!< mask of Max Burst Size      (DWORD 1)
#define    XHCI_EPCTX_MAX_PKT_SZ_POS              16U //!< position of Max Packet Size (DWORD 1)
#define    XHCI_EPCTX_TYPE_POS                     3U //!< position of EP Type         (DWORD 2)
#define    XHCI_EPCTX_TYPE_MASK                 0x38U //!< mask of EP Type             (DWORD 2)
#define    XHCI_EPCTX_DIR_IN                       4U //!< mask of EP Type (In direction)      (DWORD 2)
#define    XHCI_EPCTX_DIR_OUT                      0U //!< mask of EP Type (Out direction)     (DWORD 2)
#define    XHCI_EPCTX_EP_AVGTRBLEN_POS   ((UINT32)0U) //!< position of Average TRB Length  (DWORD 4)
#define    XHCI_EPCTX_CTRL_AVGTRB_LEN    ((UINT32)8U) //!< value of Ctrl Average Length (DWORD 4)
#define    XHCI_EPCTX_INT_AVGTRB_LEN  ((UINT32)1024U) //!< value of Intr Average Length (DWORD 4)
#define    XHCI_EPCTX_ISO_AVGTRB_LEN  ((UINT32)3072U) //!< value of Isoc Average Length (DWORD 4)
#define    XHCI_EPCTX_BULK_AVGTRB_LEN ((UINT32)3072U) //!< value of Bulk Average Length (DWORD 4)
#define    XHCI_EPCTX_MAXESITPLD_LO_POS ((UINT32)16U) //!< position of Max ESIT Payload Lo (DWORD 4)

#define    XHCI_EP0_CONTEXT_OFFSET         ((UINT32)1U) //!< Device Context Index for Endpoint 0
#define    XHCI_EPX_CONTEXT_OFFSET         ((UINT32)2U) //!< Device Context Index for non-control Endpoints
#define    XHCI_EP_CONTEXT_MAX            ((UINT32)32U) //!< Maximum Device Context Index

// Setup Stage TRB
#define    XHCI_TRB_BREQUEST_POS                ((UINT32)8U)   //!< position of bmRequest (DWORD 0)
#define    XHCI_TRB_WVALUE_POS                  ((UINT32)16U)  //!< position of wValue    (DWORD 0)
#define    XHCI_TRB_WLENGTH_POS                 ((UINT32)16U)  //!< position of wLength   (DWORD 1)
#define    XHCI_TRB_SETUP_TRT_POS               ((UINT32)16U)  //!< position of TRT       (DWORD 3)
#define    XHCI_TRB_SETUP_TRT_NO_DATA           ((UINT32)0x0U) //!< value of TRT          (DWORD 3)
#define    XHCI_TRB_SETUP_TRT_OUT_DATA          ((UINT32)0x2U) //!< value of TRT          (DWORD 3)
#define    XHCI_TRB_SETUP_TRT_IN_DATA           ((UINT32)0x3U) //!< value of TRT          (DWORD 3)
#define    XHCI_TRB_SETUP_IDT_MASK              ((UINT32)0x40U) //!< mask of IDT (Immediate Data)   (DWORD 3)

// Data Stage TRB
#define    XHCI_TRB_TRANSFER_DIR_POS            ((UINT32)16UL) //!< position of DIR     (DWORD 3)

//
// Status Stage TRB
//
#define    XHCI_TRB_STS_STG_STAT_POS       ((UINT32)     6U) //!< Cadence Device mode only: position of Status Response (STAT). 0: Stall, 1: ACK (DWORD 3)
#define    XHCI_TRB_STS_STG_STAT_ACK                    (1U) //!< Value of STAT: Cadence Device mode only. Device should answer with STALL in Status Stage
#define    XHCI_TRB_STS_STG_STAT_STALL                  (0U) //!< Value of STAT: Cadence Device mode only. Device should answer with ACK in Status Stage
#define    XHCI_TRB_STS_SETUPID_MASK       ((UINT32)0x300UL) //!< Cadence Device mode only: mask of Setup ID      (DWORD 3)
#define    XHCI_TRB_STS_SETUPID_POS          ((UINT32)   8U) //!< Cadence Device mode only: position of Setup IDs (DWORD 3)
#define    XHCI_TRB_STS_TRANSFER_DIR_POS     ((UINT32)  16U) //!< position of Direction. 0: OUT (Host-to-device), 1: IN (Device-to-host) (DWORD 3)
#define    XHCI_TRB_STS_SPEED_ID_2           ((UINT32)0x00U) //!< Cadence Device mode only: speed under USB 3.0 (HS, FS)
#define    XHCI_TRB_STS_SPEED_ID_3           ((UINT32)0x80U) //!< Cadence Device mode only: speed over USB 3.0 (SS, SSP)

// Address Device command TRB
#define    XHCI_TRB_BSR_POS               ((UINT32)9UL)  //!< BSR (Block Set Address Request) (DWORD 3)

// Event TRB
#define    XHCI_TRB_EVT_RESIDL_LEN_MSK    ((UINT32)0x0FFFFFFUL)  //!< mask of Transfer Length     (DWORD 2)
#define    XHCI_TRB_CMPL_CODE_POS         ((UINT32)24UL) //!< position of Completion Code (DWORD 2)
#define    XHCI_TRB_ENDPOINT_POS          ((UINT32)16UL) //!< position of Endpoint ID     (DWORD 3)
#define    XHCI_TRB_SLOT_ID_POS           ((UINT32)24UL) //!< position of Slot ID         (DWORD 3)


// Normal TRB
#define    XHCI_TRB_NORMAL_ENT_MASK       ((UINT32)0x02U) //!< mask of ENT (Evaluate Next TRB)         (DWORD 2)
#define    XHCI_TRB_NORMAL_ISP_MASK       ((UINT32)0x04U) //!< mask of ISP (Interrupt-on Short Packet) (DWORD 2)
#define    XHCI_TRB_NORMAL_CH_MASK        ((UINT32)0x10U) //!< mask of CH (Chain Bit)                  (DWORD 2)
#define    XHCI_TRB_NORMAL_IOC_MASK       ((UINT32)0x20U) //!< mask of IOC (Interrupt-on Completion)   (DWORD 2)
#define    XHCI_TRB_TRANSFER_LENGTH_MASK  ((UINT32)0x001FFFFUL) //!< mask of Transfer Length           (DWORD 2)
#define    XHCI_TRB_INTR_TRGT_POS         ((UINT32)22UL) //!< position of Interrupter target position  (DWORD 2)
#define    XHCI_TRB_TYPE_POS              ((UINT32)10UL) //!< position of Trb Type                     (DWORD 3)

// Link TRB
#define    XHCI_TRB_LNK_TGLE_CYC_MSK      ((UINT32)0x02U) //!< mask of TC (Toggle Cycle) (DWORD 3)


//
// Values of Port Link State (PORTSC)
//
#define    XHCI_PLS_U0_STATE          ((UINT32)0U)
#define    XHCI_PLS_U1_STATE          ((UINT32)1U)
#define    XHCI_PLS_U2_STATE          ((UINT32)2U)
#define    XHCI_PLS_U3_STATE          ((UINT32)3U)
#define    XHCI_PLS_DISABLE           ((UINT32)4U)
#define    XHCI_PLS_RXDETECT          ((UINT32)5U)
#define    XHCI_PLS_INACTIVE          ((UINT32)6U)
#define    XHCI_PLS_POLLING           ((UINT32)7U)
#define    XHCI_PLS_RECOVERY          ((UINT32)8U)
#define    XHCI_PLS_HOT_RESET         ((UINT32)9U)
#define    XHCI_PLS_COMPLILANCE_MODE  ((UINT32)10U)
#define    XHCI_PLS_TEST_MODE         ((UINT32)11U)
#define    XHCI_PLS_RESUME            ((UINT32)15U)

#define    XHCI_TRB_MAX_TRANSFER_LENGTH         ((UINT32)0x10000UL)
#define    XHCI_SYSTEM_MEMORY_PAGE_SIZE         ((UINT32)0x10000UL)

#endif

