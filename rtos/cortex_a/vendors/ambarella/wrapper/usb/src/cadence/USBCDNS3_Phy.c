/**
 *  @file USBCDNS3_Phy.c
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
 *  @details USB kernel driver for PHY functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static void usb3phy_soft_reset_deassert(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3PHY: De-assert USB32 Phy Soft Reset");

    // CLEAR USB3 PHY SOFT RESET to de-assert phy_reset_n
    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG) & 0xFFFFFFFEU;
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG, reg_val);
}

static void usb3phy_apb_soft_reset_deassert(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3PHY: De-assert USB32 Phy APB Soft Reset");

    // CLEAR USB3 PHY APB SOFT RESET to de-assert phy_apb_reset_n
    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG) & 0xFFFFFFFDU;
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG, reg_val);
}

static void usb3phy_soft_reset_assert(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3PHY: Assert USB32 Phy Soft Reset");

    // set USB3 PHY SOFT RESET to assert phy_reset_n
    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG) | 0x1U;
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG, reg_val);
}

#if !defined(USB32_LINUX_SWITCH_SUPPORT)
static void usb3phy_apb_soft_reset_assert(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3PHY: Assert USB32 Phy APB Soft Reset");

    // set USB3 PHY APB SOFT RESET to assert phy_apb_reset_n
    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG) | 0x2U;
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32P_CTRL_REG, reg_val);
}
#endif

static void usb3phy_setup_refclk_100mhz_ext(void)
{
    UINT32 pma_ctrl;

    pma_ctrl = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32_PMA_CTRL_REG);

    pma_ctrl =  pma_ctrl |
                //((UINT32)0U << (UINT32)14U)  | // pma_cmn_pll0_refclk_sel, pma_cmn_ref_<p/m>
                //((UINT32)0U << (UINT32)13U)  | // pma_cmn_pll1_refclk_sel, pma_cmn_ref_<p/m>
                ((UINT32)2U << (UINT32)11U)  | // pma_cmn_refclk_dig_div, 2 - Divide by 4 (set for 100Mhz reference clock)
                //((UINT32)0U << (UINT32)10U)  | // pma_cmn_refclk_dig_sel, pma_cmn_ref_<p/m>
                ((UINT32)1U << (UINT32)8U)   | // pma_cmn_refclk_int_mode,
                //((UINT32)0U << (UINT32)6U)   | // pma_cmn_refclk_mode,
                ((UINT32)1U << (UINT32)5U)   | // pma_cmn_refclk_term_en
                ((UINT32)1U << (UINT32)2U)   ; // pma_suspend_override, 1 = disable supspend

    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32_PMA_CTRL_REG, pma_ctrl);

    // De-assert APB soft reset to the PHY after the strap settings in the scratchpad
    usb3phy_apb_soft_reset_deassert();

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3PHY: Setting up scratchpad & PMA to use 100MHz external reference clock");

}


static void usb3phy_init(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    UINT64 base_addr = hws->Phy32.BaseAddress;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "Initializing USB3PHY...");

    if (base_addr == 0U) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "Invalid USB3 PHY address");
    } else {

        // De-assert APB soft reset to the PHY after the strap settings in the scratchpad
        usb3phy_apb_soft_reset_deassert();

        // task pma_cmn_usb_cfg
        IO_UtilityRegWrite32(base_addr + USB3PHY_PHY_PLL_CFG, 0x0000);
        IO_UtilityRegWrite32(base_addr + USB3PHY_PIPE_GEN2_PRE_CFG0, 0x0A0A);
        IO_UtilityRegWrite32(base_addr + USB3PHY_PIPE_GEN2_POST_CFG0, 0x1000);
        IO_UtilityRegWrite32(base_addr + USB3PHY_PIPE_GEN2_POST_CFG1, 0x0010);
        IO_UtilityRegWrite32(base_addr + USB3PHY_CMN_PDIAG_PLL0_CLK_SEL_M0, 0x8600);
        IO_UtilityRegWrite32(base_addr + USB3PHY_CMN_CDIAG_CDB_PWRI_OVRD, 0x8200);
        IO_UtilityRegWrite32(base_addr + USB3PHY_CMN_CDIAG_XCVRC_PWRI_OVRD, 0x8200);
        IO_UtilityRegWrite32(base_addr + USB3PHY_CMN_PLL0_VCOCAL_TCTRL, 0x0003);
        IO_UtilityRegWrite32(base_addr + USB3PHY_CMN_PLL1_VCOCAL_TCTRL, 0x0003);

        // task pma_ln_usb_cfg
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing Tx/Rx Power State Controllers Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_TX_PSC_A0, 0x02FF);
        IO_UtilityRegWrite32(base_addr + USB3PHY_TX_PSC_A1, 0x06AF);
        IO_UtilityRegWrite32(base_addr + USB3PHY_TX_PSC_A2, 0x06AE);
        IO_UtilityRegWrite32(base_addr + USB3PHY_TX_PSC_A3, 0x06AE);
        IO_UtilityRegWrite32(base_addr + USB3PHY_TX_TXCC_CTRL, 0x2A82);
        IO_UtilityRegWrite32(base_addr + USB3PHY_TX_TXCC_CPOST_MULT_01, 0x0014);

        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_PSC_A0, 0x0D1D);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_PSC_A1, 0x0D1D);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_PSC_A2, 0x0D00);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_PSC_A3, 0x0500);
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing XCVR Controller and Diagnostics Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_XCVR_DIAG_PSC_OVRD, 0x0003);
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing Rx Signal Detect Filter Control Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_SIGDET_HL_FILT_TMR, 0x0013);
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing Rx Eq Engine Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_REE_GCSM1_CTRL, 0x0000);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_REE_ATTEN_THR, 0x0C02);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_REE_SMGM_CTRL1, 0x0330);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_REE_SMGM_CTRL2, 0x0300);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_REE_TAP1_CLIP,    0x0019);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_REE_TAP2TON_CLIP, 0x0019);
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing Rx Controller and Diagnostics Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_SIGDET_TUNE, 0x1004);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_NQST_CTRL, 0x00F9);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_DFE_AMP_TUNE_2, 0x0C01);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_DFE_AMP_TUNE_3, 0x0002);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_PI_CAP, 0x0000);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_PI_RATE, 0x0031);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_DIAG_ACYA, 0x0001);
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing CDRLF Configuration Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_CDRLF_CNFG,  0x018C);
        IO_UtilityRegWrite32(base_addr + USB3PHY_RX_CDRLF_CNFG3, 0x0003);
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "Writing USB Configuration Registers");
        IO_UtilityRegWrite32(base_addr + USB3PHY_XCVR_DIAG_PLLDRC_CTRL, 0x0041);
        IO_UtilityRegWrite32(base_addr + USB3PHY_XCVR_DIAG_HSCLK_SEL, 0x0000);
        IO_UtilityRegWrite32(base_addr + USB3PHY_XCVR_DIAG_HSCLK_DIV, 0x0001);

        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "Initializing USB3PHY Done");
    }
}



/** \addtogroup Internal_APIs
 *  @{
 * */
void USBCDNS3_PHYUp(void)
{
    usb3phy_setup_refclk_100mhz_ext();
    usb3phy_init();
    usb3phy_soft_reset_deassert();

    return;
}

void USBCDNS3_PHYDown(void)
{
    // according to PHY datasheet
    // To disable PHY:
    //   1. put PHY into reset state
    //   2. put PHY APB into reset state
    // To enable PHY again:
    //   1. just follow enable flow
    usb3phy_soft_reset_assert();
#if !defined(USB32_LINUX_SWITCH_SUPPORT)
    // if we want to support device/host switch between Linux and RTOS
    // we should not put PHY APB into reset state
    // since Linux driver will access PHY registers unconditionally.
    // and this will cause driver hang.
    usb3phy_apb_soft_reset_assert();
#endif
    return;
}



/** @} */
