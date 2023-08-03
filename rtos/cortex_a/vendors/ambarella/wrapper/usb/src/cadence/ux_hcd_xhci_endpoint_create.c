/**
 *  @file ux_hcd_xhci_endpoint_create.c
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
 *  @details Ambarella USBX XHCI driver for creating endpoint
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

#include <ux_hcd_xhci.h>

UINT  ux_hcd_xhci_endpoint_create(XHC_CDN_DRV_RES_s *DrvRes, UX_ENDPOINT *UxEndpoint)
{
    UINT uret;
    const UX_ENDPOINT_DESCRIPTOR *ux_ep_desc;
    UHC_ENDPOINT_INFO_s    *ep_info;
    UINT physical_address;

    if ((UxEndpoint != NULL) && (DrvRes != NULL)) {
        ux_ep_desc = &UxEndpoint->ux_endpoint_descriptor;
        if (ux_ep_desc->bEndpointAddress != 0U) {

            UINT8 usb_ep_desc[21];

            physical_address = USBDCDNS3_PhyEndpointIdxGet(ux_ep_desc->bEndpointAddress);

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "ux_hdc_xhci_endpoint_create(): LA = 0x%X, PA = 0x%X",
                                  ux_ep_desc->bEndpointAddress, physical_address, 0, 0, 0);

            // convert to standard USB Endpoint Descriptor in bytes
            usb_ep_desc[0] = 0x07; // bLength
            usb_ep_desc[1] = (UINT8)ux_ep_desc->bDescriptorType;
            usb_ep_desc[2] = (UINT8)ux_ep_desc->bEndpointAddress;
            usb_ep_desc[3] = (UINT8)ux_ep_desc->bmAttributes;
            usb_ep_desc[4] = (UINT8)(ux_ep_desc->wMaxPacketSize);
            usb_ep_desc[5] = (UINT8)(ux_ep_desc->wMaxPacketSize >> 8U);
            usb_ep_desc[6] = (UINT8)ux_ep_desc->bInterval;
            // convert to standard SuperSpeed Endpoint Companion descriptor in bytes
            usb_ep_desc[7] = (UINT8)UxEndpoint->ux_slave_ss_comp_descriptor.bLength;
            usb_ep_desc[8] = (UINT8)UxEndpoint->ux_slave_ss_comp_descriptor.bDescriptorType;
            usb_ep_desc[9] = (UINT8)UxEndpoint->ux_slave_ss_comp_descriptor.bMaxBurst;
            usb_ep_desc[10] = (UINT8)UxEndpoint->ux_slave_ss_comp_descriptor.bmAttributes;
            usb_ep_desc[11] = (UINT8)UxEndpoint->ux_slave_ss_comp_descriptor.wBytesPerInterval;
            usb_ep_desc[12] = (UINT8)(UxEndpoint->ux_slave_ss_comp_descriptor.wBytesPerInterval >> 8U);
            // no support for SuperSpeedPlus Isochronous Endpoint Companion Descriptor
            usb_ep_desc[13] = 0;

            uret = USBHCDNS3_EndpointEnable(DrvRes, usb_ep_desc, 1);

            ep_info = &DrvRes->Udc->HostEndpointsInfoArray[physical_address];
            ep_info->LogicalEpIndex  = ux_ep_desc->bEndpointAddress;
            ep_info->PhysicalEpIndex = physical_address;
            ep_info->UxEndpoint      = UxEndpoint;
            ep_info->Transfer        = &UxEndpoint->ux_endpoint_transfer_request;

            if (uret == 0U) {
                UxEndpoint->ux_endpoint_ed = DrvRes;
            }

        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "ux_hdc_xhci_endpoint_create(): control-endpoint is already created.");
            uret = UX_SUCCESS;
        }

    } else {
        uret = UX_FUNCTION_NOT_SUPPORTED;
    }

    return uret;
}
