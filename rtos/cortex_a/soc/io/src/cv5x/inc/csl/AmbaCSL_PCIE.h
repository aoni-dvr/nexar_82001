/**
 *  @file AmbaCSL_PCIE.h
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Definitions & Constants for PCIE
 *
 */

#ifndef AMBA_CSL_PCIE_H
#define AMBA_CSL_PCIE_H

#define CMN_SSM_BIAS_TMR                (0x0022U * 4U)
#define CMN_PLLSM0_PLLPRE_TMR           (0x002AU * 4U)
#define CMN_PLLSM0_PLLLOCK_TMR          (0x002CU * 4U)
#define CMN_PLLSM1_PLLPRE_TMR           (0x0032U * 4U)
#define CMN_PLLSM1_PLLLOCK_TMR          (0x0034U * 4U)

#define CMN_CDIAG_REFCLK_DRV0_CTRL      (0x0050U * 4U)
#define CMN_BGCAL_INIT_TMR              (0x0064U * 4U)
#define CMN_BGCAL_ITER_TMR              (0x0065U * 4U)
#define CMN_IBCAL_INIT_TMR              (0x0074U * 4U)
#define CMN_IBCAL_ITER_TMR              (0x0075U * 4U)

#define CMN_PLL0_VCOCAL_INIT_TMR        (0x0084U * 4U)
#define CMN_PLL0_VCOCAL_ITER_TMR        (0x0085U * 4U)
#define CMN_PLL0_VCOCAL_REFTIM_START    (0x0086U * 4U)
#define CMN_PLL0_VCOCAL_PLLCNT_START    (0x0088U * 4U)
#define CMN_PLL0_INTDIV_M0              (0x0090U * 4U)
#define CMN_PLL0_FRACDIVL_M0            (0x0091U * 4U)
#define CMN_PLL0_FRACDIVH_M0            (0x0092U * 4U)
#define CMN_PLL0_HIGH_THR_M0            (0x0093U * 4U)
#define CMN_PLL0_DSM_DIAG_M0            (0x0094U * 4U)

#define CMN_PLL0_LOCK_REFCNT_START      (0x009CU * 4U)
#define CMN_PLL0_LOCK_PLLCNT_START      (0x009EU * 4U)
#define CMN_PLL0_LOCK_PLLCNT_THR        (0x009FU * 4U)
#define CMN_PLL0_INTDIV_M1              (0x00A0U * 4U)
#define CMN_PLL0_FRACDIVL_M1            (0x00A1U * 4U)
#define CMN_PLL0_FRACDIVH_M1            (0x00A2U * 4U)
#define CMN_PLL0_HIGH_THR_M1            (0x00A3U * 4U)
#define CMN_PLL0_DSM_DIAG_M1            (0x00A4U * 4U)

#define CMN_PLL1_VCOCAL_INIT_TMR        (0x00C4U * 4U)
#define CMN_PLL1_VCOCAL_ITER_TMR        (0x00C5U * 4U)
#define CMN_PLL1_VCOCAL_REFTIM_START    (0x00C6U * 4U)
#define CMN_PLL1_VCOCAL_PLLCNT_START    (0x00C8U * 4U)
#define CMN_PLL1_INTDIV_M0              (0x00D0U * 4U)
#define CMN_PLL1_FRACDIVL_M0            (0x00D1U * 4U)
#define CMN_PLL1_FRACDIVH_M0            (0x00D2U * 4U)
#define CMN_PLL1_HIGH_THR_M0            (0x00D3U * 4U)
#define CMN_PLL1_DSM_DIAG_M0            (0x00D4U * 4U)

#define CMN_PLL1_LOCK_REFCNT_START      (0x00DCU * 4U)
#define CMN_PLL1_LOCK_PLLCNT_START      (0x00DEU * 4U)
#define CMN_PLL1_LOCK_PLLCNT_THR        (0x00DFU * 4U)

#define CMN_TXPUCAL_INIT_TMR            (0x0104U * 4U)
#define CMN_TXPUCAL_ITER_TMR            (0x0105U * 4U)

#define CMN_TXPDCAL_INIT_TMR            (0x010CU * 4U)
#define CMN_TXPDCAL_ITER_TMR            (0x010DU * 4U)
#define CMN_RXCAL_INIT_TMR              (0x0114U * 4U)
#define CMN_RXCAL_ITER_TMR              (0x0115U * 4U)
#define CMN_SD_CAL_INIT_TMR             (0x0124U * 4U)
#define CMN_SD_CAL_ITER_TMR             (0x0125U * 4U)
#define CMN_SD_CAL_REFTIM_START         (0x0126U * 4U)
#define CMN_SD_CAL_PLLCNT_START         (0x0128U * 4U)
#define CMN_PDIAG_PLL0_CTRL_M0          (0x01A0U * 4U)

#define CMN_PDIAG_PLL0_CP_PADJ_M0       (0x01A4U * 4U)
#define CMN_PDIAG_PLL0_CP_IADJ_M0       (0x01A5U * 4U)
#define CMN_PDIAG_PLL0_FILT_PADJ_M0     (0x01A6U * 4U)
#define CMN_PDIAG_PLL0_CTRL_M1          (0x01B0U * 4U)
#define CMN_PDIAG_PLL0_CP_PADJ_M1       (0x01B4U * 4U)
#define CMN_PDIAG_PLL0_CP_IADJ_M1       (0x01B5U * 4U)
#define CMN_PDIAG_PLL0_FILT_PADJ_M1     (0x01B6U * 4U)
#define CMN_PDIAG_PLL1_CTRL_M0          (0x01C0U * 4U)

#define CMN_PDIAG_PLL1_CP_PADJ_M0       (0x01C4U * 4U)
#define CMN_PDIAG_PLL1_CP_IADJ_M0       (0x01C5U * 4U)
#define CMN_PDIAG_PLL1_FILT_PADJ_M0     (0x01C6U * 4U)

#define DRV_DIAG_LANE_FCM_EN_TO         (0x40C0U * 4U)
#define DRV_DIAG_LANE_FCM_EN_MGN_TMR    (0x40C2U * 4U)

#define TX_RCVDET_ST_TMR                (0x4123U * 4U)

/* PMA RX Lane registers */
#define RX_SDCAL0_INIT_TMR              (0x8044U * 4U)
#define RX_SDCAL0_ITER_TMR              (0x8045U * 4U)
#define RX_SDCAL1_INIT_TMR              (0x804CU * 4U)
#define RX_SDCAL1_ITER_TMR              (0x804DU * 4U)

#define PHY_PIPE_LANE_DSBL              (0xC005U * 4U)

/* PHY PCS lane registers */
#define PHY_PCS_ISO_LINK_CTRL           (0xD00BU * 4U)


/* Scratchpad registers */
#define PCIE_PMA_CTRL_REG             (UINT32)0x180U
#define PCIEP_CTRL_REG                (UINT32)0x188U
#define PCIEC_CTRL1_REG               (UINT32)0x190U
#define PCIEC_CTRL2_REG               (UINT32)0x194U
#define PCIEC_STATUS_REG              (UINT32)0x198U
#define PCIEC_PWR_REG                 (UINT32)0x19CU
#define PCIEC_DEBUG_REG               (UINT32)0x1A0U
#define PCIEC_INT_CTRL1_REG           (UINT32)0x1A4U
#define PCIEC_INT_CTRL2_REG           (UINT32)0x1A8U
#define PCIEC_INT_CTRL3_REG           (UINT32)0x1ACU

#define PCIE_I_MSI_CTRL_REG           (UINT32)0x90U
#define PCIE_I_PCIE_DEV_CTRL_STATUS   (UINT32)0xC8U
#define PCIE_I_LINK_CTRL_STATUS       (UINT32)0xD0U
#define PCIE_I_L1_PM_CAP              (UINT32)0x904U
#define PCIE_I_LANE_EQ_CONTROL_0      (UINT32)0x30CU
#define PCIE_I_LANE_EQ_CONTROL_1      (UINT32)0x310U


void AmbaCSL_PciePhyWrite(UINT32 offset, UINT32 Value);
UINT32 AmbaCSL_PciePhyRead(UINT32 offset);
void AmbaCSL_PcieSpWrite(UINT32 offset, UINT32 Value);
UINT32 AmbaCSL_PcieSpRead(UINT32 offset);
void AmbaCSL_PcieWrite(UINT32 offset, UINT32 Value);
UINT32 AmbaCSL_PcieRead(UINT32 offset);

#endif /* AMBA_CSL_PCIE_H */
