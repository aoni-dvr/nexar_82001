/**
 *  @file AmbaUSBH_Drv_Uhc.c
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
 *  @details USB kernel driver phy function.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <usbx/usbx_host_controllers/ux_hcd_ehci.h>
#include <usbx/usbx_host_controllers/ux_hcd_ohci.h>

static void phy_enable(UINT32 PowerOn)
{
    if (PowerOn == 1U) {
        // Power up PLL
        USB_PhyPllEnable(1, UPORT_MODE_HOST);
        // reset uhc
        AmbaRTSL_RctUsbHostSoftReset();
    } else {
        // Power off PLL
        USB_PhyPllEnable(0, UPORT_MODE_HOST);
    }
}

static void phy0_owner_set(UINT32 Phy0IsHost)
{
    if (Phy0IsHost != 0U) {
        USB_PhyPhy0OwnerSet(UHC_OWN_PORT);
    } else {
        USB_PhyPhy0OwnerSet(UDC_OWN_PORT);
    }
}

// disable NEON registers usage in ISR
#pragma GCC push_options
#pragma GCC target("general-regs-only")

static void ehci_isr_entry(UINT32 IntID, UINT32 UserArg)
{
    (void)IntID;
    (void)UserArg;

    _ux_hcd_ehci_interrupt_handler();
}

static void ohci_isr_entry(UINT32 IntID, UINT32 UserArg)
{
    (void)IntID;
    (void)UserArg;

    _ux_hcd_ohci_interrupt_handler();
}

#pragma GCC pop_options

static void overcurrent_polarity_set(UINT32 LowActive)
{
    if (LowActive != 0U) {
        AmbaRTSL_USBSetEhciOCPolarity(1);
    } else {
        AmbaRTSL_USBSetEhciOCPolarity(0);
    }
}

static void interrupt_init(UINT32 SelectHcd)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

    switch (SelectHcd) {
    case USBH_HCD_EHCI:
        (void)AmbaRTSL_USBIsrHook(hws->Ehci.IntPin, hws->Ehci.IntType, ehci_isr_entry);
        (void)AmbaRTSL_USBIsrEnable(hws->Ehci.IntPin);
        break;
    case USBH_HCD_OHCI:
        (void)AmbaRTSL_USBIsrHook(hws->Ohci.IntPin, hws->Ohci.IntType, ohci_isr_entry);
        (void)AmbaRTSL_USBIsrEnable(hws->Ohci.IntPin);
        break;
    case USBH_HCD_EHCI_OHCI:
        (void)AmbaRTSL_USBIsrHook(hws->Ehci.IntPin, hws->Ehci.IntType, ehci_isr_entry);
        (void)AmbaRTSL_USBIsrEnable(hws->Ehci.IntPin);
        (void)AmbaRTSL_USBIsrHook(hws->Ohci.IntPin, hws->Ohci.IntType, ohci_isr_entry);
        (void)AmbaRTSL_USBIsrEnable(hws->Ohci.IntPin);
        break;
    default:
        // shall not be here since it's checked before.
        break;
    }
}

static void interrupt_deinit(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

    (void)AmbaRTSL_USBIsrDisable(hws->Ehci.IntPin);
    (void)AmbaRTSL_USBIsrDisable(hws->Ohci.IntPin);
}
/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by driver to initialize Host controller.
 * It configures related hardware registers exept UHCI/EHCI.
 * UHCI/EHCI controller is handled by USBX.
*/
void USBH_UhcInit(const USBH_CLASS_CONFIG_s *Config)
{
    // phy setup
    phy_enable(1);

    // Set phy0 onwer
    phy0_owner_set(Config->Phy0Owner);

    // overcurrent polarity.
    overcurrent_polarity_set(Config->EhciOcPolarity);

    // interrupt setup
    interrupt_init(Config->SelectHcd);
}
/**
 * Called by driver to de-initialize Host controller.
 * It configures related hardware registers exept UHCI/EHCI.
 * UHCI/EHCI controller is handled by USBX.
*/
void USBH_UhcDeInit(void)
{
    // deinit interrupt.
    interrupt_deinit();

    // phy deinit
    phy_enable(0);
}
/** @} */
