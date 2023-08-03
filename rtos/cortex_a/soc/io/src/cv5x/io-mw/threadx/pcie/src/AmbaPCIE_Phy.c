/**
 *  @file AmbaPCIE_Phy.c
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
 *  @details PCIe driver for PHY functions.
 */

#include "AmbaPCIE_Drv.h"
#include "AmbaRTSL_PCIE.h"
#include "AmbaPCIE.h"

static void setup_refclk_24mhz_int(void)
{
    UINT32 value;

    PCIE_UtilityPrint("Setting up scratchpad & PMA to use 24MHz internal reference clock");

    value = AmbaRTSL_PcieSpRead(PCIE_PMA_CTRL_REG);
    value |= ((UINT32)1U) << 23U; // pma_cmn_pll0_refclk_sel
    value |= ((UINT32)1U) << 22U; // pma_cmn_pll1_refclk_sel
    value |= ((UINT32)1U) << 19U; // pma_cmn_refclk_dig_sel
    value |= ((UINT32)3U) << 17U; // pma_cmn_refclk_int_mode
    value |= ((UINT32)2U) << 15U; // pma_cmn_refclk_mode

    AmbaRTSL_PcieSpWrite(PCIE_PMA_CTRL_REG, value);

    //refclk programming
    AmbaRTSL_PciePhyWrite(CMN_SSM_BIAS_TMR, 0x0018);
    AmbaRTSL_PciePhyWrite(CMN_PLLSM0_PLLPRE_TMR, 0x0030);
    AmbaRTSL_PciePhyWrite(CMN_PLLSM0_PLLLOCK_TMR, 0x00f0);
    AmbaRTSL_PciePhyWrite(CMN_PLLSM1_PLLLOCK_TMR, 0x00f0);
    AmbaRTSL_PciePhyWrite(CMN_BGCAL_INIT_TMR, 0x0078);
    AmbaRTSL_PciePhyWrite(CMN_BGCAL_ITER_TMR, 0x0078);
    AmbaRTSL_PciePhyWrite(CMN_IBCAL_INIT_TMR, 0x0018);
    AmbaRTSL_PciePhyWrite(CMN_IBCAL_ITER_TMR, 0x0011);
    AmbaRTSL_PciePhyWrite(CMN_TXPUCAL_INIT_TMR, 0x001d);
    AmbaRTSL_PciePhyWrite(CMN_TXPUCAL_ITER_TMR, 0x0006);
    AmbaRTSL_PciePhyWrite(CMN_TXPDCAL_INIT_TMR, 0x001d);
    AmbaRTSL_PciePhyWrite(CMN_TXPDCAL_ITER_TMR, 0x0006);
    AmbaRTSL_PciePhyWrite(CMN_RXCAL_INIT_TMR, 0x02d0);
    AmbaRTSL_PciePhyWrite(CMN_RXCAL_ITER_TMR, 0x0006);
    AmbaRTSL_PciePhyWrite(CMN_SD_CAL_INIT_TMR, 0x0006);
    AmbaRTSL_PciePhyWrite(CMN_SD_CAL_ITER_TMR, 0x0002);
    AmbaRTSL_PciePhyWrite(CMN_SD_CAL_REFTIM_START, 0x000e);
    AmbaRTSL_PciePhyWrite(CMN_SD_CAL_PLLCNT_START, 0x0137);

    // multi-write for all-lane?
    AmbaRTSL_PciePhyWrite(DRV_DIAG_LANE_FCM_EN_TO, 0x01e0);
    AmbaRTSL_PciePhyWrite(DRV_DIAG_LANE_FCM_EN_MGN_TMR, 0x0090);
    AmbaRTSL_PciePhyWrite(RX_SDCAL0_INIT_TMR, 0x0018);
    AmbaRTSL_PciePhyWrite(RX_SDCAL0_ITER_TMR, 0x0078);
    AmbaRTSL_PciePhyWrite(RX_SDCAL1_INIT_TMR, 0x0018);
    AmbaRTSL_PciePhyWrite(RX_SDCAL1_ITER_TMR, 0x0078);
    AmbaRTSL_PciePhyWrite(TX_RCVDET_ST_TMR, 0x0960);

    // (PLL settings for all VCO rates)
    AmbaRTSL_PciePhyWrite(CMN_PLL0_DSM_DIAG_M0, 0x0004);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_DSM_DIAG_M1, 0x0004);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_VCOCAL_INIT_TMR, 0x00f0);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_VCOCAL_ITER_TMR, 0x0004);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_VCOCAL_REFTIM_START, 0x02f8);

    //pll1 must be programmed same as pll0
    AmbaRTSL_PciePhyWrite(CMN_PLL1_DSM_DIAG_M0, 0x0004);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_VCOCAL_INIT_TMR, 0x00f0);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_VCOCAL_ITER_TMR, 0x0004);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_VCOCAL_REFTIM_START, 0x02f8);

    //PLL settings for 10GHz VCO - pll0_mode0
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_CP_PADJ_M0, 0x030d);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_CP_IADJ_M0, 0x0e01);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_FILT_PADJ_M0, 0x0d05);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_INTDIV_M0, 0x01a0);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_FRACDIVL_M0, 0xaaab);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_FRACDIVH_M0, 0x0002);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_HIGH_THR_M0, 0x0116);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_CTRL_M0, 0x0022);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_VCOCAL_PLLCNT_START, 0x3ded);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_LOCK_REFCNT_START, 0x0013);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_LOCK_PLLCNT_START, 0x01a0);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_LOCK_PLLCNT_THR, 0x0005);

    // PLL settings for 8GHz VCO - pll0_mode1
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_CP_PADJ_M1, 0x030d);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_CP_IADJ_M1, 0x0e01);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_FILT_PADJ_M1, 0x0d05);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_INTDIV_M1, 0x014d);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_FRACDIVL_M1, 0x5555);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_FRACDIVH_M1, 0x0002);
    AmbaRTSL_PciePhyWrite(CMN_PLL0_HIGH_THR_M1, 0x00e0);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL0_CTRL_M1, 0x0022);

    //program the same in pll1
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL1_CP_PADJ_M0, 0x030d);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL1_CP_IADJ_M0, 0x0e01);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL1_FILT_PADJ_M0, 0x0d05);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_INTDIV_M0, 0x014d);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_FRACDIVL_M0, 0x5555);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_FRACDIVH_M0, 0x0002);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_HIGH_THR_M0, 0x00e0);
    AmbaRTSL_PciePhyWrite(CMN_PDIAG_PLL1_CTRL_M0, 0x0022);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_VCOCAL_PLLCNT_START, 0x3ded);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_LOCK_REFCNT_START, 0x0013);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_LOCK_PLLCNT_START, 0x01a0);
    AmbaRTSL_PciePhyWrite(CMN_PLL1_LOCK_PLLCNT_THR, 0x0005);
}

void Impl_PcieInit(UINT32 mode, UINT32 gen, UINT32 lane, UINT32 use24mhz)
{
    UINT32 value;
    UINT32 cnt = 0;

    if (mode == 1U) {
        PCIE_UtilityPrint("Init Link Training (RC)");
        // 1. Program the Mode Strap register in the scratchpad region to select Root Port/ Endpoint Mode
        value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
        AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value | 0x00200000U); //bit 21 - mode select
    } else {
        PCIE_UtilityPrint("Init Link Training (EP)");
    }

    // 2. Initialize PCIE PHY
    /*--- set up controller registers ---- */
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);

    // select cointroller generation
    if (gen == 1U) {
        //Gen 1 generational select = 0
        value = value & 0xfff9ffffU; //clear bit 18:17
    } else if (gen == 2U) {
        value = value & 0xfff9ffffU; //clear bits 18:17
        value = value | 0xfffbffffU; //set bit 17
    } else {
        //default gen_select is Gen3
    }

    // select lane-width
    if (lane == 1U) {
        value = value & 0xffe7ffffU; //clear bit 20:19. For x1
    } else if (lane == 2U) {
        value = value & 0xffe7ffffU; //clear bit 20:19.
        value = value | 0x80000U; //set bit 19. For x2
    } else {
        //default x4
    }
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value);

    /*--- set up PHY registers ---- */
    value = AmbaRTSL_PcieSpRead(PCIEP_CTRL_REG);
    AmbaRTSL_PcieSpWrite(PCIEP_CTRL_REG, value & 0xfffffffeU); //apb_reset_n

    AmbaRTSL_PciePhyWrite(0x3404c, 0x01010101); //for core clk to be at 250 mhz

    // disable unused lane to save power
    if (lane == 1U) {
        AmbaRTSL_PciePhyWrite(PHY_PIPE_LANE_DSBL, 0xFE); // disable all lanes, except for lane 0
    } else if (lane == 2U) {
        AmbaRTSL_PciePhyWrite(PHY_PIPE_LANE_DSBL, 0xFC); // disable all lanes, except for lane 0 and 1.
    } else {
        //default x4
    }

    if (use24mhz == 1U) {
        setup_refclk_24mhz_int();
    }

    // 3. De-assert the PCIE PHY reset and wait for PCIE PHY clock outputs to be stable
    //de-assert phy pipe rst_n after configuring phyreg
    value = AmbaRTSL_PcieSpRead(PCIEP_CTRL_REG);
    AmbaRTSL_PcieSpWrite(PCIEP_CTRL_REG, value & 0xfffffffdU);

    //set correct ratio in apb core clk ratio
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value | 0x00000800U); // apb core clk ratio set to 4 (bit 13:9)

    value = AmbaRTSL_PciePhyRead(PHY_PCS_ISO_LINK_CTRL);
    while ((value & 0x0002U) == 2U) {
        value = AmbaRTSL_PciePhyRead(PHY_PCS_ISO_LINK_CTRL);;
        if (cnt++ > 200U) {
            break;
        }
        (void)AmbaKAL_TaskSleep(1);
    }
    PCIE_UtilityPrintUInt5("link0_clk_stable = 0x%x now... polling %d times",value,cnt,0,0,0);

    // 4. De-assert the PCIE controller resets
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xffffff7fU); // clear bit 7 - perst
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xffffffbfU); // clear bit 6 - pipe_reset
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xfffffffeU); // clear bit 0 - resetN
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xfffffffbU); // clear bit 2 - axi_reset
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xfffffffdU); // clear bit 1 - apb_reset
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xffffffefU); // clear bit 4 - mgmt_sticky_reset
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xffffffdfU); // clear bit 5 - mgmt_reset
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value & 0xfffffff7U); // clear bit 3 - pm_reset

}

void Impl_LinkStart(UINT32 mode, UINT32 gen)
{
    UINT32 value;
    UINT32 cnt = 0;

    // 5. Program the registers in the PCIE controller as required and initiate link training
    if (mode == 0U) {

        //write some regs through local mgmt bus to avoid differences between device and monitor
        value = AmbaRTSL_PcieRead(PCIE_I_MSI_CTRL_REG);
        value = value & 0xffff8fffU;
        value = value | 0x00004000U;
        AmbaRTSL_PcieWrite(PCIE_I_MSI_CTRL_REG, value);

        value = AmbaRTSL_PcieRead(PCIE_I_L1_PM_CAP);
        AmbaRTSL_PcieWrite(PCIE_I_L1_PM_CAP, value & 0xff2fffffU); // Port power val

        AmbaRTSL_PcieWrite(PCIE_I_LANE_EQ_CONTROL_0, 0x0);
        AmbaRTSL_PcieWrite(PCIE_I_LANE_EQ_CONTROL_1, 0x0);

    } else {
        AmbaRTSL_PcieWrite(PCIE_I_LANE_EQ_CONTROL_0 | 0x200000U, 0x0); //RP MGNT
        AmbaRTSL_PcieWrite(PCIE_I_LANE_EQ_CONTROL_1 | 0x200000U, 0x0); //RP MGNT
    }

    //Initiates the Link Training
    value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
    AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value | 0x00400000U); // set bit 22 - link training enable

    if (gen == 2U) {
        value = AmbaRTSL_PcieSpRead(PCIEC_DEBUG_REG);
        while ((value & 0x001fU) != 0x10U) {
            value = AmbaRTSL_PcieSpRead(PCIEC_DEBUG_REG);
            // Polling LTSSM state for Gen1 Speed
        }
        value = AmbaRTSL_PcieRead(PCIE_I_LINK_CTRL_STATUS);
        AmbaRTSL_PcieWrite(PCIE_I_LINK_CTRL_STATUS, value | 0x20U); // Retrain link
    }

    if (mode == 0U) {
        value = AmbaRTSL_PcieSpRead(PCIEC_CTRL1_REG);
        AmbaRTSL_PcieSpWrite(PCIEC_CTRL1_REG, value | 0x02000000U); // set bit 25 -  config enable in EP mode
    }

    value = AmbaRTSL_PcieSpRead(PCIEC_STATUS_REG);
    while ((value & 0x000cU) != 0x000cU) {
        value = AmbaRTSL_PcieSpRead(PCIEC_STATUS_REG);
        cnt++;
        (void)AmbaKAL_TaskSleep(1);
    }
    PCIE_UtilityPrintUInt5("link_status_reg == 0x%x now... polling %d times",value,cnt,0,0,0);

}
