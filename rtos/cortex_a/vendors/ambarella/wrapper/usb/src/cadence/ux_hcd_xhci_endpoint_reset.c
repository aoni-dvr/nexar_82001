/**
 *  @file ux_hcd_xhci_endpoint_reset.c
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
 *  @details Ambarella USBX XHCI driver for reseting endpoint
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>


#include <ux_hcd_xhci.h>

UINT  ux_hcd_xhci_endpoint_reset(XHC_CDN_DRV_RES_s *DrvRes, UX_ENDPOINT *UxEndpoint)
{
    UINT uret;
    const UX_ENDPOINT_DESCRIPTOR *ux_ep_desc;
    UHC_ENDPOINT_INFO_s    *ep_info;
    UINT physical_address;

    if ((UxEndpoint != NULL) && (DrvRes != NULL)) {

        AmbaMisra_TouchUnused(UxEndpoint);

        ux_ep_desc = &UxEndpoint->ux_endpoint_descriptor;
        if (ux_ep_desc->bEndpointAddress != 0U) {

            physical_address = USBDCDNS3_PhyEndpointIdxGet(ux_ep_desc->bEndpointAddress);

            uret = USBHCDNS3_EndpointReset(DrvRes, physical_address, 1);

            if (uret == 0U) {
                ep_info = &DrvRes->Udc->HostEndpointsInfoArray[physical_address];
                ep_info->EpState = UX_SUCCESS;

                // since we reset endpoint, we also update Transfer Ring Dequeue Pointer to empty TRB.
                // otherwise the previous occupied TRB still takes effect.
                // here the doorbell is not ringed so it is safe to assing it to empty TRB.
                uret = USBHCDNS3_TRDequeuePtrSet(DrvRes, physical_address, 1);
            }
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "ux_hdc_xhci_endpoint_reset(): not support for control-endpoint.");
            uret = UX_SUCCESS;
        }

    } else {
        uret = UX_FUNCTION_NOT_SUPPORTED;
    }

    return uret;
}
