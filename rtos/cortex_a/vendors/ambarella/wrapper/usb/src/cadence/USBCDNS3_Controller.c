/**
 *  @file USBCDNS3_Controller.c
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
 *  @details USB kernel driver for Controller functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static void usb3ctr_soft_reset_deassert(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3_CTROLLER: De-assert USB32 Controller Soft Reset");

    // CLEAR USB3 Controller SOFT RESET to de-assert reset
    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG) & 0xFFFFFFFEU;
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG, reg_val);
}

static void usb3ctr_soft_reset_assert(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3_CTROLLER: Assert USB32 Controller Soft Reset");

    // set USB3 Controller SOFT RESET to assert reset
    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG) | 0x1U;
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG, reg_val);
}

static void usb3ctr_mode_write(UINT32 Value)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3_CTROLLER: write USB32 Controller mode");

    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG);
    reg_val &= 0xFFFFFFF9U;
    reg_val |= (Value << 1U);
    IO_UtilityRegWrite32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG, reg_val);
}

static UINT32 usb3ctr_mode_read(void)
{
    UINT32 reg_val;

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USB3_CTROLLER: read USB32 Controller mode");

    reg_val = IO_UtilityRegRead32(SCRATCHPAD_NONSEC_ADDRESS + SCRATCHPAD_USB32C_CTRL_REG);
    reg_val &= 0x06U;
    reg_val = (reg_val >> 1U);
    return reg_val;
}

#define OTGCMD_OFFSET          (0x10U)
#define OTGSTS_OFFSET          (0x14U)
#define OTGPRT_OVRD_OFFSET     (0x3CU)

#define OTGCMD_DEV_BUS_REQ       ((UINT32)1U)
#define OTGCMD_HOST_BUS_REQ      ((UINT32)1U << 1U)
#define OTGCMD_OTG_DIS           ((UINT32)1U << 3U)
//#define OTGCMD_DEV_SESS_VLD_SET  ((UINT32)1U << 6U)
#define OTGCMD_DEV_SESS_VLD_CLR  ((UINT32)1U << 7U)
#define OTGCMD_DEV_BUS_DROP      ((UINT32)1U << 8U)
#define OTGCMD_HOST_BUS_DROP     ((UINT32)1U << 9U)
#define OTGCMD_DEV_POWER_OFF     ((UINT32)1U << 11U)
#define OTGCMD_HOST_POWER_OFF    ((UINT32)1U << 12U)

#define OTGSTS_DEV_ACTIVE        ((UINT32)1U << 3U)
#define OTGSTS_HOST_ACTIVE       ((UINT32)1U << 4U)

#define OTGSTS_DEV_READY         ((UINT32)1U << 26U)
#define OTGSTS_HOST_READY        ((UINT32)1U << 27U)

#define OTGPRT_OVRD_SSES_VLD_SEL_SFT (10U)
//#define OTGPRT_OVRD_SSES_VLD_SFR_SFT (11U)

#define OTGPRT_OVRD_SSES_VLD_SEL_MSK ((UINT32)1U << OTGPRT_OVRD_SSES_VLD_SEL_SFT)
//#define OTGPRT_OVRD_SSES_VLD_SFR_MSK ((UINT32)1U << OTGPRT_OVRD_SSES_VLD_SFR_SFT)

#define OTGPRT_OVRD_SSSES_VALD_PHY  (0U) //!< select vbus_valid/session valid signal from OTG PHY
#define OTGPRT_OVRD_SSSES_VALD_SFR  (1U) //!< select vbus_valid/session valid signal from SFR


/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * @param Mode 1: Host mode, 2: Device mode
*/
void USBCDNS3_ControllerUp(UINT32 Mode)
{
    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_ControllerUp(): start");

    if ((Mode == USBCDNS3_CTRLER_MODE_HOST) || (Mode == USBCDNS3_CTRLER_MODE_DEVICE) || (Mode == USBCDNS3_CTRLER_MODE_NONE)) {
        const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
        UINT64 base_addr = hws->Xhci.BaseAddress & 0xFFFFFFFFFFFF0000U;
        UINT64 otg_cmd_addr = base_addr + OTGCMD_OFFSET;
        UINT64 otg_sts_addr = base_addr + OTGSTS_OFFSET;
        UINT64 otg_port_override_addr = base_addr + OTGPRT_OVRD_OFFSET;

        // write OTG strap mode
        usb3ctr_mode_write(Mode);
        // let controller run
        usb3ctr_soft_reset_deassert();

        if (Mode == USBCDNS3_CTRLER_MODE_DEVICE) {
            // select vbus_valid/session valid signal from OTG PHY
            IO_UtilityRegMaskWrite32(otg_port_override_addr, OTGPRT_OVRD_SSSES_VALD_PHY, OTGPRT_OVRD_SSES_VLD_SEL_MSK, OTGPRT_OVRD_SSES_VLD_SEL_SFT);
            // request OTG controller to switch to device mode
            IO_UtilityRegWrite32(otg_cmd_addr, OTGCMD_DEV_BUS_REQ | OTGCMD_OTG_DIS | OTGCMD_DEV_SESS_VLD_CLR );
            // wait until device mode is ready
            if (USB_UtilityWaitForReg(otg_sts_addr, OTGSTS_DEV_READY, OTGSTS_DEV_READY, 1000) != 0U) {
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "USBCDNS3_ControllerUp(): can't wait for OTG_DEV_READY 0x%X", IO_UtilityRegRead32(otg_sts_addr));
            }
        }
        if (Mode == USBCDNS3_CTRLER_MODE_HOST) {
            // request OTG controller to switch to host mode
            IO_UtilityRegWrite32(otg_cmd_addr, OTGCMD_HOST_BUS_REQ | OTGCMD_OTG_DIS);
            // wait until host mode is ready
            if (USB_UtilityWaitForReg(otg_sts_addr, OTGSTS_HOST_READY, OTGSTS_HOST_READY, 1000) != 0U) {
                USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "USBCDNS3_ControllerUp(): can't wait for OTG_HOST_READY 0x%X", IO_UtilityRegRead32(otg_sts_addr));
            }
        }
    } else {
        // error case
        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "USBCDNS3_ControllerUp(): Unknown USB32 Controller mode %d", Mode);
    }
}

void USBCDNS3_ControllerDown(void)
{
    const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();
    UINT64 base_addr = hws->Xhci.BaseAddress & 0xFFFFFFFFFFFF0000U;
    UINT64 otg_cmd_addr = base_addr + OTGCMD_OFFSET;
    UINT64 otg_sts_addr = base_addr + OTGSTS_OFFSET;
    UINT64 otg_port_override_addr = base_addr + OTGPRT_OVRD_OFFSET;

    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_ControllerDown(): start");

    // request OTG controller to stop device and host mode
    IO_UtilityRegWrite32(otg_cmd_addr, OTGCMD_HOST_BUS_DROP | OTGCMD_DEV_BUS_DROP | OTGCMD_DEV_POWER_OFF | OTGCMD_HOST_POWER_OFF);

    // wait until device and host mode are stopped
    if (USB_UtilityWaitForReg(otg_sts_addr, OTGSTS_HOST_ACTIVE | OTGSTS_DEV_ACTIVE, 0, 1000) != 0U) {
        USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR, "USBCDNS3_ControllerDown(): can't wait for OTG_DEV_READY and OTG_HOST_READY 0x%X", IO_UtilityRegRead32(otg_sts_addr));
    }

    // need to change trap mode to NONE for next time Host or Device active
    // 1. put controller into reset state
    // 2. set trap mode to NONE
    // 3. put controller into run state (very important, otherwise Host won't detect device insertion next time)
    //    (looks a timing issue but no idea why)
    usb3ctr_soft_reset_assert();
    USB_UtilityTaskSleep(100);
    usb3ctr_mode_write(USBCDNS3_CTRLER_MODE_NONE);
    USB_UtilityTaskSleep(100);
    usb3ctr_soft_reset_deassert();

    // select vbus_valid/session valid signal from SFR
    IO_UtilityRegMaskWrite32(otg_port_override_addr, OTGPRT_OVRD_SSSES_VALD_SFR, OTGPRT_OVRD_SSES_VLD_SEL_MSK, OTGPRT_OVRD_SSES_VLD_SEL_SFT);
}

UINT32 USBCDNS3_ControllerModeGet(void)
{
    return usb3ctr_mode_read();
}


/** @} */
