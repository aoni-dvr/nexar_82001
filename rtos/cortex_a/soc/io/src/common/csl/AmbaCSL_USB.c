/**
 *  @file AmbaCSL_USB.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Universal Serial Bus (USB) Device/Host CSL Driver
 *
 */

#include <AmbaDef.h>
#include "AmbaCSL_USB.h"
#include "AmbaCSL_GIC.h"
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>

CSL_USB_HW_SETTING_s g_csl_usb_hws= {
#if defined(CONFIG_SOC_CV2FS) || defined(CONFIG_SOC_CV22FS)
    // UDC
    { 0xE0006000UL, 71U + 32U, 0},
    // OHCI
    { 0xE001E000UL, 70U + 32U, 0},
    // EHCI
    { 0xE001F000UL, 69U + 32U, 0},
    // RCT
    { 0xED080000UL, 0U, 0},
#elif defined(CONFIG_SOC_CV22) || defined(CONFIG_SOC_CV25) || defined(CONFIG_SOC_CV28) || defined(CONFIG_SOC_H32)
    // UDC
    { 0xE0006000UL, 67U + 32U, 0},
    // OHCI
    { 0xE001E000UL, 66U + 32U, 0},
    // EHCI
    { 0xE001F000UL, 65U + 32U, 0},
    // RCT
    { 0xED080000UL, 0U, 0},
#elif defined(CONFIG_SOC_CV2)
    // UDC
    { 0xE0006000UL, 67U + 32U, 0},
    // OHCI
    { 0xE0008000UL, 66U + 32U, 0},
    // EHCI
    { 0xE0009000UL, 65U + 32U, 0},
    // RCT
    { 0xED080000UL, 0U, 0},
#elif defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    // UDC
    { 0x20E0006000UL, 67U + 32U, 0},
    // OHCI
    { 0x0UL, 0U, 0},
    // EHCI
    { 0x0UL, 0U, 0},
    // RCT
    { 0x20ED080000UL, 0U, 0},
#else
#error "AmbaCSL_USB.c: unsupport chip version."
#endif
};

#define DEV_CONFIG_OFFSET (0x400UL)

UINT32 AmbaCSL_UsbGetDevConfig(void)
{
    UINT64 addr = g_csl_usb_hws.Udc.BaseAddress + DEV_CONFIG_OFFSET;
    return AmbaCSL_UsbRead32(addr);
}

#if ((defined(CONFIG_BUILD_SYNOPSYS_USB2_DRIVER)) || (defined(CONFIG_DEVICE_TREE_SUPPORT)))

static void *csl_usb_u64_to_ptr(UINT64 Value)
{
    void *ptr;

#if defined(CONFIG_ARM32)
    UINT32 addr = (UINT32)Value;
    if (AmbaWrap_memcpy(&ptr, &addr, sizeof(void *)) != 0U) {
        // action TBD
    }
#else
    if (AmbaWrap_memcpy(&ptr, &Value, sizeof(void *)) != 0U) {
        // action TBD
    }
#endif
    return ptr;
}

/**
 * Configure UDC interrupt
 * @param PinID
 * @param PinType
 */
void AmbaCSL_UsbVbusVicConfig(UINT32 PinID, UINT32 PinType)
{
#ifndef CONFIG_QNX
    AmbaCSL_GicSetIntConfig(PinID, PinType);
#endif
}

/*
 *  @RoutineName:: AmbaCSL_UsbVbusVicConfig
 *
 *  @Description:: Get Vbus VIC raw status
 *
 *  @Input      :: none
 *
 *  @Output     :: none
 *
 *  @Return     :: UINT32 1/0
 */
UINT32 AmbaCSL_UsbVbusVicRawStaus(void)
{
    return AmbaCSL_Usb3GetDeviceConn();
}

static void AmbaCSL_SetRegisterBits(volatile UINT32 *RegPtr, UINT32 Value, UINT32 Mask, UINT32 Shift)
{
    UINT32 x = (Value << Shift ) & Mask;
    *RegPtr = (*RegPtr & (~Mask)) | x;
}

static UINT32 AmbaCSL_GetRegisterBits(volatile UINT32 RegValue, UINT32 Mask, UINT32 Shift)
{
    return (RegValue & Mask) >> Shift;
}

// ------------------------------------------------------
// Device Configuration Register
// ------------------------------------------------------

void AmbaCSL_UsbSetDevConfig(UINT32 value)
{
    pAmbaUSB_Reg->DevConfig = value;
}

void AmbaCSL_UsbSetDevConfigSpd(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_SPEED_MASK,
                            CONFIG_REG_SPEED_SHIFT);

}

void AmbaCSL_UsbSetDevConfigRwkp(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_REMOTE_WAKEUP_MASK,
                            CONFIG_REG_REMOTE_WAKEUP_SHIFT);
}

void AmbaCSL_UsbSetDevConfigSp(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_SELF_POWERED_MASK,
                            CONFIG_REG_SELF_POWERED_SHIFT);
}

void AmbaCSL_UsbSetDevConfigPyType(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_PHY_TYPE_MASK,
                            CONFIG_REG_PHY_TYPE_SHIFT);
}

void AmbaCSL_UsbSetDevConfigRev(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_RESERVED_MASK,
                            CONFIG_REG_RESERVED_SHIFT);
}

void AmbaCSL_UsbSetDevConfigHaltSts(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_HALT_STATUS_MASK,
                            CONFIG_REG_HALT_STATUS_SHIFT);
}

void AmbaCSL_UsbSetDevConfigDynProg(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_DYNAMIC_PROGRAM_MASK,
                            CONFIG_REG_DYNAMIC_PROGRAM_SHIFT);
}

void AmbaCSL_UsbSetDevConfigSetDesc(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevConfig,
                            value,
                            CONFIG_REG_SET_DESC_MASK,
                            CONFIG_REG_SET_DESC_SHIFT);
}

// ------------------------------------------------------
// Device Control Register
// ------------------------------------------------------
void AmbaCSL_UsbSetDevCtl(UINT32 ctrl)
{
    pAmbaUSB_Reg->DevCtrl = ctrl;
}

void AmbaCSL_UsbSetDevCtlRde(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_RXDMA_ENABLE_MASK, CONTROL_REG_RXDMA_ENABLE_SHIFT);
}

UINT32 AmbaCSL_UsbGetDevCtlRde(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevCtrl, CONTROL_REG_RXDMA_ENABLE_MASK, CONTROL_REG_RXDMA_ENABLE_SHIFT);
}

void AmbaCSL_UsbSetDevCtlTde(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_TXDMA_ENABLE_MASK, CONTROL_REG_TXDMA_ENABLE_SHIFT);
}

void AmbaCSL_UsbSetDevCtlBe(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_BIG_ENDIAN_MASK, CONTROL_REG_BIG_ENDIAN_SHIFT);
}

void AmbaCSL_UsbSetDevCtlBren(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_BURST_ENABLE_MASK, CONTROL_REG_BURST_ENABLE_SHIFT);
}
void AmbaCSL_UsbSetDevCtlMode(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_MODE_MASK, CONTROL_REG_MODE_SHIFT);
}
void AmbaCSL_UsbSetDevCtlSd(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_SOFT_DISCONNECT_MASK, CONTROL_REG_SOFT_DISCONNECT_SHIFT);
}
void AmbaCSL_UsbSetDevCtlCsrdone(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_CSR_DONE_MASK, CONTROL_REG_CSR_DONE_SHIFT);
}
void AmbaCSL_UsbSetDevCtlSrxflush(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_RXFIFO_FLUSH_MASK, CONTROL_REG_RXFIFO_FLUSH_SHIFT);
}
void AmbaCSL_UsbSetDevCtlBrlen(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevCtrl, value, CONTROL_REG_BURST_LENGTH_MASK, CONTROL_REG_BURST_LENGTH_SHIFT);
}

// ------------------------------------------------------
// Device Status Register
// ------------------------------------------------------

UINT32 AmbaCSL_UsbGetDevStatusCfg(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevStatus, DEV_STATUS_REG_CONFIG_MASK, DEV_STATUS_REG_CONFIG_SHIFT);
}

UINT32 AmbaCSL_UsbGetDevStatusIntf(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevStatus, DEV_STATUS_REG_INTERFACE_MASK, DEV_STATUS_REG_INTERFACE_SHIFT);
}

UINT32 AmbaCSL_UsbGetDevStatusAlt(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevStatus, DEV_STATUS_REG_ALTERNATE_MASK, DEV_STATUS_REG_ALTERNATE_SHIFT);
}

UINT32 AmbaCSL_UsbGetDevStatusEnumSpd(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevStatus, DEV_STATUS_REG_ENUM_SPEED_MASK, DEV_STATUS_REG_ENUM_SPEED_SHIFT);
}

UINT32 AmbaCSL_UsbGetDevStatusRFEmpty(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevStatus, DEV_STATUS_REG_RXFIFO_EMPTY_MASK, DEV_STATUS_REG_RXFIFO_EMPTY_SHIFT);
}

UINT32 AmbaCSL_UsbGetDevStatusTs(void)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->DevStatus, DEV_STATUS_REG_TIME_STAMP_MASK, DEV_STATUS_REG_TIME_STAMP_SHIFT);
}

// ------------------------------------------------------
// Device Interrupt Status Register
// ------------------------------------------------------
void AmbaCSL_UsbSetDevIntStatus(UINT32 status)
{
    pAmbaUSB_Reg->DevIntStatus = status;
}

void AmbaCSL_UsbClearDevIntSc(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_SET_CONFIG_MASK, DEV_INTSTS_REG_SET_CONFIG_SHIFT);
}

void AmbaCSL_UsbClearDevIntSi(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_SET_INTERFACE_MASK, DEV_INTSTS_REG_SET_INTERFACE_SHIFT);
}
void AmbaCSL_UsbClearDevIntES(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_IDLE_MASK, DEV_INTSTS_REG_IDLE_SHIFT);
}

void AmbaCSL_UsbClearDevIntUR(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1,  DEV_INTSTS_REG_RESET_MASK, DEV_INTSTS_REG_RESET_SHIFT);
}

void AmbaCSL_UsbClearDevIntUS(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_SUSPEND_MASK, DEV_INTSTS_REG_SUSPEND_SHIFT);
}

void AmbaCSL_UsbClearDevIntSOF(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_SOF_MASK, DEV_INTSTS_REG_SOF_SHIFT);
}

void AmbaCSL_UsbClearDevIntENUM(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_ENUM_DONE_MASK, DEV_INTSTS_REG_ENUM_DONE_SHIFT);
}
void AmbaCSL_UsbClearDevIntRMTWKP(void)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntStatus, 1, DEV_INTSTS_REG_REMOTE_WAKEUP_MASK, DEV_INTSTS_REG_REMOTE_WAKEUP_SHIFT);
}

// ------------------------------------------------------
// Device Interrupt Mask Register
// ------------------------------------------------------
void AmbaCSL_UsbSetDevIntMask(UINT32 value)
{
    pAmbaUSB_Reg->DevIntMask = value;
}
void AmbaCSL_UsbSetDevIntMaskSc(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_SET_CONFIG_MASK, DEV_INTMASK_REG_SET_CONFIG_SHIFT);
}

void AmbaCSL_UsbSetDevIntMaskSi(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_SET_INTERFACE_MASK, DEV_INTMASK_REG_SET_INTERFACE_SHIFT);
}
void AmbaCSL_UsbSetDevIntMaskES(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_IDLE_MASK, DEV_INTMASK_REG_IDLE_SHIFT);
}

void AmbaCSL_UsbSetDevIntMaskUR(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_RESET_MASK, DEV_INTMASK_REG_RESET_SHIFT);
}
void AmbaCSL_UsbSetDevIntMaskUS(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_SUSPEND_MASK, DEV_INTMASK_REG_SUSPEND_SHIFT);
}
void AmbaCSL_UsbSetDevIntMaskSOF(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_SOF_MASK, DEV_INTMASK_REG_SOF_SHIFT);
}
void AmbaCSL_UsbSetDevIntMaskENUM(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_ENUM_DONE_MASK, DEV_INTMASK_REG_ENUM_DONE_SHIFT);
}
void AmbaCSL_UsbSetDevIntMaskRMTWKP(UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->DevIntMask, value, DEV_INTMASK_REG_REMOTE_WAKEUP_MASK, DEV_INTMASK_REG_REMOTE_WAKEUP_SHIFT);
}

// ------------------------------------------------------
// Endpoint Interrupt Status Register
// ------------------------------------------------------

UINT32 AmbaCSL_UsbGetEpInt(void)
{
    return pAmbaUSB_Reg->EndpointIntStatus;
}
void AmbaCSL_UsbSetEpInt(UINT32 d)
{
    pAmbaUSB_Reg->EndpointIntStatus = d;
}

// ------------------------------------------------------
// Endpoint Interrupt Mask Register
// ------------------------------------------------------

void AmbaCSL_UsbEnInEpInt(UINT32 id)
{
    UINT32 base = 1U;
    pAmbaUSB_Reg->EndpointIntMask &= ~(base << (id));          /* enable in id interrupt */
}

void AmbaCSL_UsbEnOutEpInt(UINT32 id)
{
    UINT32 base = 1U;
    UINT32 shift = id + 16U;
    pAmbaUSB_Reg->EndpointIntMask &= ~(base << shift);     /* enable out id interrupt */
}

void AmbaCSL_UsbDisInEpInt(UINT32 id)
{
    UINT32 base = 1U;
    pAmbaUSB_Reg->EndpointIntMask |= (base << (id));           /* disable in id interrupt */
}

void AmbaCSL_UsbDisOutEpInt(UINT32 id)
{
    UINT32 base = 1U;
    UINT32 shift = id + 16U;
    pAmbaUSB_Reg->EndpointIntMask |= (base << shift);      /* disable out id interrupt */
}

/*
 *  USB_udc20 Endpoint Register
 */

UINT32 AmbaCSL_UsbGetEp20LogicalID(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->Udc20Endpoint[id],
                                   UDC20_EP_REG_LOGIC_ID_MASK,
                                   UDC20_EP_REG_LOGIC_ID_SHIFT);
}

void AmbaCSL_UsbSetEp20(UINT32 id, UINT32 value)
{
    pAmbaUSB_Reg->Udc20Endpoint[id] = value;
}
void AmbaCSL_UsbSetEp20LogicalID(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_LOGIC_ID_MASK,
                            UDC20_EP_REG_LOGIC_ID_SHIFT);
}
void AmbaCSL_UsbSetEp20EndPointDir(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_DIRECTION_MASK,
                            UDC20_EP_REG_DIRECTION_SHIFT);
}
void AmbaCSL_UsbSetEp20EndPointType(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_TYPE_MASK,
                            UDC20_EP_REG_TYPE_SHIFT);
}
void AmbaCSL_UsbSetEp20ConfigID(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_CONFIG_ID_MASK,
                            UDC20_EP_REG_CONFIG_ID_SHIFT);
}
void AmbaCSL_UsbSetEp20Interface(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_INTF_ID_MASK,
                            UDC20_EP_REG_INTF_ID_SHIFT);
}
void AmbaCSL_UsbSetEp20AltSetting(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_ALT_ID_MASK,
                            UDC20_EP_REG_ALT_ID_SHIFT);
}
void AmbaCSL_UsbSetEp20MaxPacketSize(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->Udc20Endpoint[id],
                            value,
                            UDC20_EP_REG_MPS_MASK,
                            UDC20_EP_REG_MPS_SHIFT);
}



// ------------------------------------------------------
// Endpoint Control Register
// ------------------------------------------------------

UINT32 AmbaCSL_UsbGetEpInStall(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, EP_CONTROL_REG_STALL_MASK, EP_CONTROL_REG_STALL_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpInNAK(UINT32 id)
{
    // nak bit is read only
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, EP_CONTROL_REG_NAK_MASK, EP_CONTROL_REG_NAK_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpInPollDemand(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, EP_CONTROL_REG_POLL_DEMAND_MASK, EP_CONTROL_REG_POLL_DEMAND_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpInEndPointType(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, EP_CONTROL_REG_TYPE_MASK, EP_CONTROL_REG_TYPE_SHIFT);
}

void AmbaCSL_UsbSetEpInCtrl(UINT32 id, UINT32 ctrl)
{
    pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl = ctrl;
}

void AmbaCSL_UsbSetEpInStall(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, value, EP_CONTROL_REG_STALL_MASK, EP_CONTROL_REG_STALL_SHIFT);
}

void AmbaCSL_UsbSetEpInCtrlF(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, value, EP_CONTROL_REG_FLUSH_TXFIFO_MASK, EP_CONTROL_REG_FLUSH_TXFIFO_SHIFT);
}

void AmbaCSL_UsbSetEpInPollDemand(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, value, EP_CONTROL_REG_POLL_DEMAND_MASK, EP_CONTROL_REG_POLL_DEMAND_SHIFT);
}
void AmbaCSL_UsbSetEpInEndPointType(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, value, EP_CONTROL_REG_TYPE_MASK, EP_CONTROL_REG_TYPE_SHIFT);
}

void AmbaCSL_UsbSetEpInNAK(UINT32 id, UINT32 value)
{
    // snak bit is write only
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, value, EP_CONTROL_REG_SET_NAK_MASK, EP_CONTROL_REG_SET_NAK_SHIFT);
}
void AmbaCSL_UsbClearEpInNAK(UINT32 id, UINT32 value)
{
    // cnak bit is write only
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointCtrl, value, EP_CONTROL_REG_CLEAR_NAK_MASK, EP_CONTROL_REG_CLEAR_NAK_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpOutNAK(UINT32 id)
{
    // nak bit is read only
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl, EP_CONTROL_REG_NAK_MASK, EP_CONTROL_REG_NAK_SHIFT);
}

void AmbaCSL_UsbSetEpOutCtrl(UINT32 id, UINT32 ctrl)
{
    pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl = ctrl;
}

void AmbaCSL_UsbSetEpOutStall(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl, value, EP_CONTROL_REG_STALL_MASK, EP_CONTROL_REG_STALL_SHIFT);
}

void AmbaCSL_UsbSetEpOutEndPointType(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl, value, EP_CONTROL_REG_TYPE_MASK, EP_CONTROL_REG_TYPE_SHIFT);
}

void AmbaCSL_UsbSetEpOutNAK(UINT32 id, UINT32 value)
{
    // snak bit is write only
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl, value, EP_CONTROL_REG_SET_NAK_MASK, EP_CONTROL_REG_SET_NAK_SHIFT);
}
void AmbaCSL_UsbClearEpOutNAK(UINT32 id, UINT32 value)
{
    // cnak bit is write only
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl, value, EP_CONTROL_REG_CLEAR_NAK_MASK, EP_CONTROL_REG_CLEAR_NAK_SHIFT);
}
void AmbaCSL_UsbSetEpOutRxReady(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointCtrl, value, EP_CONTROL_REG_RX_READY_MASK, EP_CONTROL_REG_RX_READY_SHIFT);
}

/*
 *  USB_end[n]_status_in Registers
 */
UINT32 AmbaCSL_UsbGetEpInStatus(UINT32 id)
{
    return pAmbaUSB_Reg->EndpointIn[id].EndpointStatus;
}

UINT32 AmbaCSL_UsbGetEpInStatusBna(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointStatus, EP_STATUS_REG_BNA_MASK, EP_STATUS_REG_BNA_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpInStatusTdc(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                                   EP_STATUS_REG_TXDMA_COMPLETE_MASK,
                                   EP_STATUS_REG_TXDMA_COMPLETE_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpInStatusRcs(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                                   EP_STATUS_REG_RX_CLR_STALL_MASK,
                                   EP_STATUS_REG_RX_CLR_STALL_SHIFT);

}
UINT32 AmbaCSL_UsbGetEpInStatusRss(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                                   EP_STATUS_REG_RX_SET_STALL_MASK,
                                   EP_STATUS_REG_RX_SET_STALL_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpInStatusTxEmpty(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                                   EP_STATUS_REG_TX_FIFO_EMPTY_MASK,
                                   EP_STATUS_REG_TX_FIFO_EMPTY_SHIFT);
}

void AmbaCSL_UsbSetEpInStatus(UINT32 id, UINT32 sts)
{
    pAmbaUSB_Reg->EndpointIn[id].EndpointStatus = sts;
}

void AmbaCSL_UsbClrEpInStatusIn(UINT32 id, UINT32 value)
{
    // For In id Only
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_IN_MASK,
                            EP_STATUS_REG_IN_SHIFT);
}

void AmbaCSL_UsbClrEpInStatusBna(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_BNA_MASK,
                            EP_STATUS_REG_BNA_SHIFT);
}

void AmbaCSL_UsbClrEpInStatusTdc(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_TXDMA_COMPLETE_MASK,
                            EP_STATUS_REG_TXDMA_COMPLETE_SHIFT);
}

void AmbaCSL_UsbClrEpInStatusRcs(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_RX_CLR_STALL_MASK,
                            EP_STATUS_REG_RX_CLR_STALL_SHIFT);
}
void AmbaCSL_UsbClrEpInStatusRss(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_RX_SET_STALL_MASK,
                            EP_STATUS_REG_RX_SET_STALL_SHIFT);
}
void AmbaCSL_UsbClrEpInStatusTxEmpty(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_TX_FIFO_EMPTY_MASK,
                            EP_STATUS_REG_TX_FIFO_EMPTY_SHIFT);
}

/*
 *  USB_end[n]_status_out Registers
 */
UINT32 AmbaCSL_UsbGetEpOutStatus(UINT32 id)
{
    return pAmbaUSB_Reg->EndpointOut[id].EndpointStatus;
}
UINT32 AmbaCSL_UsbGetEpOutStatusOut(UINT32 id)
{
    // For Out id Only
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                                   EP_STATUS_REG_OUT_MASK,
                                   EP_STATUS_REG_OUT_SHIFT);
}
UINT32 AmbaCSL_UsbGetEpOutStatusBna(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                                   EP_STATUS_REG_BNA_MASK,
                                   EP_STATUS_REG_BNA_SHIFT);
}

UINT32 AmbaCSL_UsbGetEpOutStatusRcs(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                                   EP_STATUS_REG_RX_CLR_STALL_MASK,
                                   EP_STATUS_REG_RX_CLR_STALL_SHIFT);
}
UINT32 AmbaCSL_UsbGetEpOutStatusRss(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                                   EP_STATUS_REG_RX_SET_STALL_MASK,
                                   EP_STATUS_REG_RX_SET_STALL_SHIFT);
}

void AmbaCSL_UsbSetEpOutStatus(UINT32 id, UINT32 sts)
{
    pAmbaUSB_Reg->EndpointOut[id].EndpointStatus = sts;
}
void AmbaCSL_UsbClrEpOutStatusOut(UINT32 id, UINT32 value)
{
    // For Out id Only, 2 bits
    UINT32 x = (value << EP_STATUS_REG_OUT_SHIFT) & EP_STATUS_REG_OUT_MASK;
    pAmbaUSB_Reg->EndpointOut[id].EndpointStatus |= x;
}
void AmbaCSL_UsbClrEpOutStatusBna(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_BNA_MASK,
                            EP_STATUS_REG_BNA_SHIFT);
}
void AmbaCSL_UsbClrEpOutStatusRcs(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_RX_CLR_STALL_MASK,
                            EP_STATUS_REG_RX_CLR_STALL_SHIFT);
}
void AmbaCSL_UsbClrEpOutStatusRss(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointOut[id].EndpointStatus,
                            value,
                            EP_STATUS_REG_RX_SET_STALL_MASK,
                            EP_STATUS_REG_RX_SET_STALL_SHIFT);
}

/*
 *  USB_end[n]_buffsize_in Registers
 */
UINT32 AmbaCSL_UsbGetEpInBuffsize(UINT32 id)
{
    return pAmbaUSB_Reg->EndpointIn[id].EndpointBufSize;
}

UINT32 AmbaCSL_UsbGetEpInBfszSize(UINT32 id)
{
    return AmbaCSL_GetRegisterBits(pAmbaUSB_Reg->EndpointIn[id].EndpointBufSize,
                                   EP_BUF_REG_SIZE_MASK,
                                   EP_BUF_REG_SIZE_SHIFT);

}

void AmbaCSL_UsbSetEpInBfszSize(UINT32 id, UINT32 value)
{
    AmbaCSL_SetRegisterBits(&pAmbaUSB_Reg->EndpointIn[id].EndpointBufSize,
                            value,
                            EP_BUF_REG_SIZE_MASK,
                            EP_BUF_REG_SIZE_SHIFT);
}

/*
 *  USB_end[n]_max_pktsz Registers
 */
void  AmbaCSL_UsbSetEpInMaxpksz(UINT32 id, UINT32 sz)
{
    pAmbaUSB_Reg->EndpointIn[id].EndpointMaxPktSize = sz;
}

void   AmbaCSL_UsbSetEpOutMaxpksz(UINT32 id, UINT32 sz)
{
    pAmbaUSB_Reg->EndpointOut[id].EndpointMaxPktSize = sz;
}

/*
 *  USB_end[n]_setup_buffptr Registers
 */
UINT32 AmbaCSL_UsbGetEpOutSetupbuf(UINT32 id)
{
    return pAmbaUSB_Reg->EndpointOut[id].EndpointSetupDesc;
}
void AmbaCSL_UsbSetEpOutSetupbuf(UINT32 id, UINT32 ptr)
{
    pAmbaUSB_Reg->EndpointOut[id].EndpointSetupDesc = ptr;
}

/*
 *  USB_end[n]_desptr_out Registers
 */
UINT32 AmbaCSL_UsbGetEpInDesptr(UINT32 id)
{
    return pAmbaUSB_Reg->EndpointIn[id].EndpointDataDesc;
}
void   AmbaCSL_UsbSetEpInDesptr(UINT32 id, UINT32 ptr)
{
    pAmbaUSB_Reg->EndpointIn[id].EndpointDataDesc = ptr;
}

void   AmbaCSL_UsbSetEpOutDesptr(UINT32 id, UINT32 ptr)
{
    pAmbaUSB_Reg->EndpointOut[id].EndpointDataDesc = ptr;
}

/*
 *  USB_end[n]_packet_fm_out Registers
 */

/*
 *  USB_end[n]_read/write_confir Registers
 */

void AmbaCSL_UsbWrite32(UINT64 Address, UINT32 Value)
{
    volatile UINT32 *ptr;
    const void *void_ptr = csl_usb_u64_to_ptr(Address);
    AmbaMisra_TypeCast(&ptr, &void_ptr);
    if (ptr != NULL) {
        *ptr = Value;
    }
}

UINT32 AmbaCSL_UsbRead32(UINT64 Address)
{
    volatile const UINT32 *ptr;
    UINT32 uret = 0;
    const void *void_ptr = csl_usb_u64_to_ptr(Address);
    AmbaMisra_TypeCast(&ptr, &void_ptr);
    if (ptr != NULL) {
        uret = *ptr;
    }
    return uret;
}

#if 0
UINT32 AmbaCSL_UsbMaskRead32(UINT64 Address, UINT32 Mask, UINT32 Shift)
{
    UINT32 value = AmbaCSL_UsbRead32(Address);
    return (value & Mask) >> Shift;
}
#endif

void AmbaCSL_UsbMaskSetWrite32(UINT64 Address, UINT32 Value, UINT32 Mask, UINT32 Shift)
{
    UINT32 reg_value = AmbaCSL_UsbRead32(Address);
    UINT32 x = (Value << Shift ) & Mask;
    reg_value = (reg_value & (~Mask)) | x;
    AmbaCSL_UsbWrite32(Address, reg_value);
    return;
}

void AmbaCSL_UsbMaskClearWrite32(UINT64 Address, UINT32 Mask)
{
    UINT32 value = AmbaCSL_UsbRead32(Address);
    value = ClearBits(value, Mask);
    AmbaCSL_UsbWrite32(Address, value);
    return;
}

#endif

void AmbaCSL_UsbSetHwInfo(const CSL_USB_HW_SETTING_s *HwSettings)
{
    g_csl_usb_hws = *HwSettings;
    if (g_csl_usb_hws.Udc.BaseAddress != 0U) {
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
        //pAmbaUSB_Reg = csl_usb_u64_to_ptr(g_csl_usb_hws.Udc.BaseAddress);
        const void *void_ptr = csl_usb_u64_to_ptr(g_csl_usb_hws.Udc.BaseAddress);
        if (AmbaWrap_memcpy(&pAmbaUSB_Reg, &void_ptr, sizeof(void *))!= 0U) {
            // action TBD
        }
#else
        // do nothing
#endif
    }
}

