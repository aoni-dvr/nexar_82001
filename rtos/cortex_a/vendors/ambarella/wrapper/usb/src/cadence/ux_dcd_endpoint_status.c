/**
 *  @file ux_dcd_endpoint_status.c
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
 *  @details USB device driver for UX endpoint get status functions.
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
 * Called by USBX to get status of physical Endpoint.
 * */
UINT32 USBDCDNS3_EndpointStatusGet(const USBD_UDC_s *Udc, UINT32 LogicalAddress)
{

    const UDC_ENDPOINT_INFO_s *udc_ed_info;
    UINT32 uret;

    UINT32 phy_ep_index = USBDCDNS3_PhyEndpointIdxGet(LogicalAddress);


    // Fetch the address of the physical endpoint.
    udc_ed_info = &Udc->DcdEndpointsInfoArray[phy_ep_index];


    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                          "USBDCDNS3_EndpointStatusGet(): LogicalAddress = 0x%X, PhysicalIndex = %d state = 0x%X",
                          LogicalAddress,
                          phy_ep_index,
                          udc_ed_info->EpState,
                          0, 0);

    // Check the endpoint status, if it is free, we have a illegal endpoint.
    if ((udc_ed_info->EpState & USBD_UDC_ED_STATUS_USED) == 0U) {
        uret = 0xff;
    } else {
        // Check if the endpoint is stalled.
        if ((udc_ed_info->EpState & USBD_UDC_ED_STATUS_STALLED) == 0U) {
            uret = 0;
        } else {
            uret = 1;
        }
    }

    return uret;
}
/** @} */
