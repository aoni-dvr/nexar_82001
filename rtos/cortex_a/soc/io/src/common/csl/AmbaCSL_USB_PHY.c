/**
 *  @file AmbaCSL_USB_PHY.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
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
 *  @details USB PHY Control CSL
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"

#include "AmbaCSL_USB.h"

#if (defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)) && defined(CONFIG_BUILD_CADENCE_USB3_DRIVER)

#include <AmbaCortexA76.h>
#include <AmbaIOUtility.h>

#define SCRATCHPAD_NS_BAR (AMBA_CORTEX_A76_SCRATCHPAD_NS_BASE_ADDR)
#define USB32C_CTRL_REG   (SCRATCHPAD_NS_BAR + 0x16CU) //!< USB32 Controller control register
#define USB32P_CTRL_REG   (SCRATCHPAD_NS_BAR + 0x174U) //!< USB32 PHY control register

// USB32 Controller control register
#define USB32C_CTRL_SOFT_RST_SFT       ((UINT32)(0U))
#define USB32C_CTRL_MODE_SFT           ((UINT32)(1U))
#define USB32C_CTRL_SOFT_RST_MASK      (((UINT32)0x01U)/* << USB32C_CTRL_SOFT_RST_SFT*/)
#define USB32C_CTRL_MODE_MASK          (((UINT32)0x03U) << USB32C_CTRL_MODE_SFT)

// USB32 PHY control register
#define USB32P_CTRL_SOFT_RST_SFT       ((UINT32)(0U))
#define USB32P_CTRL_APB_SOFT_RST_SFT   ((UINT32)(1U))
#define USB32P_CTRL_SOFT_RST_MASK      (((UINT32)0x01U)/* << USB32P_CTRL_SOFT_RST_SFT*/)
#define USB32P_CTRL_APB_SOFT_RST_MASK  (((UINT32)0x01U) << USB32P_CTRL_APB_SOFT_RST_SFT)

// Put USB32 PHY and Controller into reset or run state.
// It should be called only from AmbaRTSL_PwcReboot()
// @param [IN] value:
//              0: run state
//              1: reset state
void AmbaCSL_RctSetUsbHostSoftReset(UINT32 value)
{
    // put controller into reset/run state
    IO_UtilityRegMaskWrite32(USB32C_CTRL_REG, value, USB32C_CTRL_SOFT_RST_MASK, USB32C_CTRL_SOFT_RST_SFT);
    // put PHY into reset/run state
    IO_UtilityRegMaskWrite32(USB32P_CTRL_REG, value, USB32P_CTRL_SOFT_RST_MASK, USB32P_CTRL_SOFT_RST_SFT);
    // put PHY APB into reset/run state
    IO_UtilityRegMaskWrite32(USB32P_CTRL_REG, value, USB32P_CTRL_APB_SOFT_RST_MASK, USB32P_CTRL_APB_SOFT_RST_SFT);
}

#else

// ------------------------------------------------------
// USB RESET CONTROL Register
// ------------------------------------------------------

void AmbaCSL_RctSetUsbHostSoftReset(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBC0_CTRL_POS,
                              value,
                              RCT_USBC0_RESET_HOST_MASK,
                              RCT_USBC0_RESET_HOST_SFT);
}

// ------------------------------------------------------
// USB PHY0 SELECT Register
// ------------------------------------------------------
void AmbaCSL_RctSetUsbPhy0Select(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_SEL_POS,
                              value,
                              RCT_USBP0_SEL0_MASK,
                              RCT_USBP0_SEL0_SFT);
}

// ------------------------------------------------------
// ANA_PWR Registers for USB
// ------------------------------------------------------
static void AmbaCSL_RctSetUsbPhy0Suspend(void)
{
    //pAmbaRCT_Reg->AnalogPowerCtrl.UsbPhySuspendDevice = 0U;
    AmbaCSL_UsbMaskClearWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS, RCT_USBP0_SPND_DEV_MASK);
}

static void AmbaCSL_RctSetUsbPhy0On(void)
{
    //pAmbaRCT_Reg->AnalogPowerCtrl.UsbPhySuspendDevice = 1U;
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS,
                              1,
                              RCT_USBP0_SPND_DEV_MASK,
                              RCT_USBP0_SPND_DEV_SFT);
}

static void AmbaCSL_RctSetUsbPhy0NvrSuspend(UINT32 d)
{
    //pAmbaRCT_Reg->AnalogPowerCtrl.UsbPhyNeverSuspend = ((UINT8)d);
    if (d == 0U) {
        AmbaCSL_UsbMaskClearWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS, RCT_USBP0_NVR_SPND_DEV_MASK);
    } else {
        AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS,
                                  1,
                                  RCT_USBP0_NVR_SPND_DEV_MASK,
                                  RCT_USBP0_NVR_SPND_DEV_SFT);
    }
}

void AmbaCSL_RctSetUsbHostOn(void)
{
    //pAmbaRCT_Reg->AnalogPowerCtrl.UsbPhySuspendHost = 1U;
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS,
                              1,
                              RCT_USBP0_SPND_HOST_MASK,
                              RCT_USBP0_SPND_HOST_SFT);
}

void AmbaCSL_RctSetUsbHostOff(void)
{
    //pAmbaRCT_Reg->AnalogPowerCtrl.UsbPhySuspendHost = 0U;
    AmbaCSL_UsbMaskClearWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS, RCT_USBP0_SPND_HOST_MASK);
}

UINT32 AmbaCSL_RctGetUsbPhy0NvrSuspend(void)
{
    //return pAmbaRCT_Reg->AnalogPowerCtrl.UsbPhyNeverSuspend;
    UINT32 value = AmbaCSL_UsbRead32(g_csl_usb_hws.Rct.BaseAddress + RCT_USB_ANA_PWR_POS);
    UINT32 uret = 0;
    if (CheckBits(value, RCT_USBP0_NVR_SPND_DEV_MASK) != 0U) {
        uret = 1;
    }
    return uret;
}

void AmbaCSL_RctSetUsbPhy0Off(void)
{
    AmbaCSL_RctSetUsbPhy0Suspend();
    AmbaCSL_RctSetUsbPhy0NvrSuspend(0);
}

void AmbaCSL_RctSetUsbPhy0AlwaysOn(void)
{
    AmbaCSL_RctSetUsbPhy0On();
    AmbaCSL_RctSetUsbPhy0NvrSuspend(1);
}

// ------------------------------------------------------
// USB PHY0 Control0 Register
// ------------------------------------------------------

void AmbaCSL_RctSetUsbPhy0Commononn(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL_POS,
                              value,
                              RCT_USBP0_COMNONONN_MASK,
                              RCT_USBP0_COMNONONN_SFT);
}

void AmbaCSL_RctSetUsbPhy0TxRiseTune(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL_POS,
                              value,
                              RCT_USBP0_TX_RISE_TUNE_MASK,
                              RCT_USBP0_TX_RISE_TUNE_SFT);
}

void AmbaCSL_RctSetUsbPhy0TxvRefTune(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL_POS,
                              value,
                              RCT_USBP0_TX_VREF_TUNE_MASK,
                              RCT_USBP0_TX_VREF_TUNE_SFT);
}

void AmbaCSL_RctSetUsbPhy0TxFslsTune(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL_POS,
                              value,
                              RCT_USBP0_TX_FSLS_TUNE_MASK,
                              RCT_USBP0_TX_FSLS_TUNE_SFT);
}

// ------------------------------------------------------
// USB PHY0 Control1 Register
// ------------------------------------------------------
void AmbaCSL_RctSetUsbPhy0Ctrl1(UINT32 value)
{
    AmbaCSL_UsbWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS, value);
    //pAmbaRCT_Reg->UsbPhyCtrl1 = value;
}

UINT32 AmbaCSL_RctGetUsbPhy0Ctrl1(void)
{
    return AmbaCSL_UsbRead32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS);
    //return pAmbaRCT_Reg->UsbPhyCtrl1;
}

void AmbaCSL_RctSetUsbPhy0TxPreEmp(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS,
                              value,
                              RCT_USBP0_TX_AMP_TUNE_MASK,
                              RCT_USBP0_TX_AMP_TUNE_SFT);
}

void AmbaCSL_RctSetUsbPhy0DCDetect(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS,
                              value,
                              RCT_USBP0_DCD_ENABLE_MASK,
                              RCT_USBP0_DCD_ENABLE_SFT);
}

void AmbaCSL_RctSetUsbPhy0DSrcVolt(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS,
                              value,
                              RCT_USBP0_SRC_VOL_EN_MASK,
                              RCT_USBP0_SRC_VOL_EN_SFT);
}

void AmbaCSL_RctSetUsbPhy0DDetVolt(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS,
                              value,
                              RCT_USBP0_DET_VOL_EN_MASK,
                              RCT_USBP0_DET_VOL_EN_SFT);
}

void AmbaCSL_RctSetUsbPhy0ChgrSrcSel(UINT32 value)
{
    AmbaCSL_UsbMaskSetWrite32(g_csl_usb_hws.Rct.BaseAddress + RCT_USBP0_CTRL1_POS,
                              value,
                              RCT_USBP0_CHG_SRC_SEL_MASK,
                              RCT_USBP0_CHG_SRC_SEL_SFT);
}

void AmbaCSL_RctUsbHostSoftReset(void)
{
    // reset Host controller
    AmbaCSL_RctSetUsbHostSoftReset(1);
    AmbaCSL_RctSetUsbHostSoftReset(0);
}

#endif

//------------ USB part end ------------------
