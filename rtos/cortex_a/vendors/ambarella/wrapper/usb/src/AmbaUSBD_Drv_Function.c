/**
 *  @file AmbaUSBD_Drv_Function.c
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
 *  @details USB device driver hook functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static UINT32 dispatch_impl(USBD_UDC_s *UsbdUdc, UINT Function, const void *Parameter)
{
    UINT32 uret;
    switch (Function) {
    case UX_DCD_GET_FRAME_NUMBER:
        uret = USBD_DrvFrameNumberGet(UsbdUdc, USB_UtilityVoidP2UInt32P(Parameter));
        break;

    case UX_DCD_TRANSFER_REQUEST:
        uret = USBD_DrvTransferRequest(UsbdUdc, USB_UtilityVoidP2UsbxSlaveXfer(Parameter));
        break;

    case UX_DCD_CREATE_ENDPOINT:
        uret = USBD_DrvEndpointCreate(UsbdUdc, USB_UtilityVoidP2UsbxEp(Parameter));
        break;

    case UX_DCD_DESTROY_ENDPOINT:
        uret = USBD_DrvEndpointDestroy(UsbdUdc, USB_UtilityVoidP2UsbxEp(Parameter));
        break;

    case UX_DCD_RESET_ENDPOINT:
        uret = USBD_DrvEndpointReset(UsbdUdc, USB_UtilityVoidP2UsbxEp(Parameter));
        break;
    case UX_DCD_STALL_ENDPOINT:
        USBD_DrvEndpointStall(USB_UtilityVoidP2UsbxEp(Parameter));
        uret = UX_SUCCESS;
        break;

    case UX_DCD_SET_DEVICE_ADDRESS:
        uret = UX_SUCCESS;
        break;

    case UX_DCD_CHANGE_STATE:
        uret = UX_SUCCESS;
        break;

    case UX_DCD_ENDPOINT_STATUS:
        uret = USBD_DrvEndpointStatusGet(UsbdUdc, USB_UtilityVoid2UInt32(Parameter));
        break;

    case UX_DCD_TRANSFER_ABORT:
        uret = USBD_DrvTransferAbort(USB_UtilityVoidP2UsbxSlaveXfer(Parameter));
        break;

    default:
        uret = UX_FUNCTION_NOT_SUPPORTED;
        break;
    }

    return uret;
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX and it dispatches commands from USBX
 * */
UINT USBD_DrvDispatchFunction(const struct UX_SLAVE_DCD_STRUCT *Dcd, UINT Function, const void *Parameter)
{
    USBD_UDC_s *usbd_udc;
    UINT32 uret;

    // Check the status of the controller.
    if (Dcd != NULL) {
        if (Dcd->ux_slave_dcd_status != (UINT32)UX_UNUSED) {
            uret = USBD_UdcUdcInstanceGet(&usbd_udc);
            if (uret == USB_ERR_SUCCESS) {
                if (usbd_udc == NULL) {
                    uret = UX_PARAMETER_ERROR;
                } else if (Parameter == NULL) {
                    uret = UX_PARAMETER_ERROR;
                } else {
                    // Look at the function and route it.
                    uret = dispatch_impl(usbd_udc, Function, Parameter);
                }
            }
        } else {
            uret = UX_CONTROLLER_UNKNOWN;
        }
    } else {
        uret = UX_PARAMETER_ERROR;
    }
    return uret;
}
/** @} */
