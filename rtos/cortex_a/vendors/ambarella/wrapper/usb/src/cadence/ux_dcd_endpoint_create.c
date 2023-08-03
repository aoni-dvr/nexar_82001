/**
 *  @file ux_dcd_endpoint_create.c
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
 *  @details USB device driver for UX endpoint create functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

// Logical    Physical
//  0x01      0x02
//  0x81      0x03
//  0x02      0x04
//  0x82      0x05
//
UINT32 USBDCDNS3_PhyEndpointIdxGet(UINT32 LogicalAddress)
{
    UINT32 endpoint_phy_index;
    UINT32 flag_in;
    UINT32 uret;

    // calculate endpoint physical index
    if (LogicalAddress != 0U) {
        if ((LogicalAddress & 0x80U) > 0U) {
            flag_in = 1;
        } else {
            flag_in = 0;
        }

        //endpoint_phy_index = (((LogicalAddress & 0x7FU) - 1U) * 2U) + ((flag_in > 0U) ? 1U : 0U);
        endpoint_phy_index = LogicalAddress & 0x7FU;
        endpoint_phy_index = (endpoint_phy_index - 1U) * 2U;
        endpoint_phy_index = endpoint_phy_index + flag_in;
        uret = endpoint_phy_index + XHCI_EPX_CONTEXT_OFFSET;
    } else {
        uret = XHCI_EP0_CONTEXT_OFFSET;
    }
    return uret;
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to create Endpoint resources.
 * */
UINT32 USBDCDNS3_EndpointCreate(struct USBD_UDC_ts *Udc, UX_SLAVE_ENDPOINT *Endpoint)
{
    UINT32 logical_address;
    UINT32 uret = USB_ERR_SUCCESS;

    logical_address = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBDCDNS3_EndpointCreate(): logical_address 0x%X", logical_address, 0, 0, 0, 0);

    // Considering the chance endpoint interrupt happens before enumeration, endpoint buffer of Ep-0 is allocated in "udc_control_endpoint_create".
    if (logical_address != 0U) {
        UINT8 endpoint_descriptor[21];
        UINT32 endpoint_phy_index;
        UDC_ENDPOINT_INFO_s *udc_ep_info= NULL;

        // calculate endpoint physical index
        endpoint_phy_index = USBDCDNS3_PhyEndpointIdxGet(logical_address);
        if (endpoint_phy_index < UDC_MAX_ENDPOINT_NUM) {
            udc_ep_info                         = &Udc->DcdEndpointsInfoArray[endpoint_phy_index];
            udc_ep_info->UxEndpoint             = Endpoint;
            udc_ep_info->PhysicalEpIndex        = endpoint_phy_index;
            udc_ep_info->EpState               |= USBD_UDC_ED_STATUS_USED; // mark this endpoint as USED
        }

        // convert to standard USB Endpoint Descriptor in bytes
        endpoint_descriptor[0] = (UINT8)Endpoint->ux_slave_endpoint_descriptor.bLength;
        endpoint_descriptor[1] = (UINT8)Endpoint->ux_slave_endpoint_descriptor.bDescriptorType;
        endpoint_descriptor[2] = (UINT8)Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
        endpoint_descriptor[3] = (UINT8)Endpoint->ux_slave_endpoint_descriptor.bmAttributes;
        endpoint_descriptor[4] = (UINT8)Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
        endpoint_descriptor[5] = (UINT8)(Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize >> 8U);
        endpoint_descriptor[6] = (UINT8)Endpoint->ux_slave_endpoint_descriptor.bInterval;
        // convert to standard SuperSpeed Endpoint Companion descriptor in bytes
        endpoint_descriptor[7] = (UINT8)Endpoint->ux_slave_ss_comp_descriptor.bLength;
        endpoint_descriptor[8] = (UINT8)Endpoint->ux_slave_ss_comp_descriptor.bDescriptorType;
        endpoint_descriptor[9] = (UINT8)Endpoint->ux_slave_ss_comp_descriptor.bMaxBurst;
        endpoint_descriptor[10] = (UINT8)Endpoint->ux_slave_ss_comp_descriptor.bmAttributes;
        endpoint_descriptor[11] = (UINT8)Endpoint->ux_slave_ss_comp_descriptor.wBytesPerInterval;
        endpoint_descriptor[12] = (UINT8)(Endpoint->ux_slave_ss_comp_descriptor.wBytesPerInterval >> 8U);
        // convert to standard SuperSpeedPlus Isochronous Endpoint Companion descriptor in bytes
        USBD_UdcEndpointBufferAllocate(Endpoint);
        Endpoint->ux_slave_endpoint_ed = USB_UtilityUdcEd2VoidP(udc_ep_info);
        uret = USBCDNS3_EndpointEnable(&Udc->CDNDrvRes, endpoint_descriptor);
    } else {
        UINT32 speed = Udc->CDNDrvRes.ActualSpeed;

        if ((speed == CH9_USB_SPEED_SUPER) || (speed == CH9_USB_SPEED_SUPER_PLUS)) {
            Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize = 512;
        }
    }


    return uret;
}
/** @} */
