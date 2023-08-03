/**
 *  @file ux_dcd_endpoint_destroy.c
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
 *  @details USB device driver for UX endpoint destroy functions.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to destroy Endpoint resources.
 * */
UINT32 USBDCDNS3_EndpointDestroy(struct USBD_UDC_ts *Udc, UX_SLAVE_ENDPOINT *Endpoint)
{
    UINT32 logical_address;
    UINT32 endpoint_phy_index;
    UDC_ENDPOINT_INFO_s *udc_ep_info = NULL;
    UINT32 uret = 0;

    logical_address = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBDCDNS3_EndpointDestroy(): logical address = 0x%X", logical_address, 0, 0, 0, 0);

    if (logical_address != 0U) {

        // calculate endpoint physical index
        endpoint_phy_index = USBDCDNS3_PhyEndpointIdxGet(logical_address);

        if (endpoint_phy_index < UDC_MAX_ENDPOINT_NUM) {

            udc_ep_info                         = &Udc->DcdEndpointsInfoArray[endpoint_phy_index];

            if (udc_ep_info->PhysicalEpIndex == endpoint_phy_index) {

                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBDCDNS3_EndpointDestroy(): PhysicalEpIndex = 0x%X", endpoint_phy_index, 0, 0, 0, 0);

                USBD_UdcEndpointBufferDestroy(Endpoint);
                udc_ep_info->UxEndpoint   = NULL;
                udc_ep_info->EpState     &= ~(USBD_UDC_ED_STATUS_USED); // mark this endpoint as NOT USED
                // issue DisableEndpoint command
                uret = USBCDNS3_EndpointDisable(&Udc->CDNDrvRes, logical_address);
            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBDCDNS3_EndpointDestroy(): endpoint_phy_index = 0x%X != PhysicalEpIndex = 0x%X", endpoint_phy_index, udc_ep_info->PhysicalEpIndex, 0, 0, 0);
                uret = UX_ERROR;
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBDCDNS3_EndpointDestroy(): endpoint_phy_index = 0x%X >= %X", endpoint_phy_index, UDC_MAX_ENDPOINT_NUM, 0, 0, 0);
            uret = UX_ERROR;
        }
    }

    return uret;
}
/** @} */
