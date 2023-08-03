/**
 *  @file ux_hcd_xhci_transfer_abort.c
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
 *  @details Ambarella USBX XHCI driver for aborting transfer request
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>


#include <ux_hcd_xhci.h>

static void ux_hcd_xhci_request_remove_all(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PhyEpIndex, UINT32 FlagWait)
{
    UINT32 func_uret;
    // To drop all requests from controller:
    //     1. stop endpoint
    //     2. reset Transfer Ring Dequeue Pointer for the endpoint
    //         a. the endpoint will be re-enabled then.
    // 1. stop endpoint
    func_uret = USBHCDNS3_EndpointStop(DrvRes, PhyEpIndex, FlagWait);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_request_remove_all(): RX [0x%X] failed to stop endpoint 0x%X", PhyEpIndex, func_uret, 0, 0, 0);
    }

    // 2. reset transfer ring dequeue pointer
    func_uret = USBHCDNS3_TRDequeuePtrSet(DrvRes, PhyEpIndex, FlagWait);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_request_remove_all(): RX [0x%X] failed to set tr_dequeue pointer 0x%X", PhyEpIndex, func_uret, 0, 0, 0);
    }
}


UINT  ux_hcd_xhci_request_abort(UX_HCD_XHCI *HcdXhci, UX_TRANSFER *UxTransfer)
{
    UINT         uret;
    UINT         logical_address;
    UINT         physical_address;

    if ((UxTransfer != NULL) && (HcdXhci != NULL)) {

        const UX_ENDPOINT            *ux_endpoint;
        const UX_ENDPOINT_DESCRIPTOR *ux_ep_desc;
        XHC_CDN_DRV_RES_s            *drv_res = HcdXhci->cdns_drv_res;

        AmbaMisra_TouchUnused(HcdXhci);
        AmbaMisra_TouchUnused(UxTransfer);

        ux_endpoint = UxTransfer->ux_transfer_request_endpoint;

        if ((ux_endpoint != NULL) && (drv_res != NULL)) {

            ux_ep_desc  = &ux_endpoint->ux_endpoint_descriptor;

            logical_address  = ux_ep_desc->bEndpointAddress;
            physical_address = USBDCDNS3_PhyEndpointIdxGet(logical_address);

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "ux_hcd_xhci_request_abort(): abort for endpoint 0x%X (PA 0x%X)", logical_address, physical_address, 0, 0, 0);

            ux_hcd_xhci_request_remove_all(drv_res, physical_address, 1);

            uret = UX_SUCCESS;

        } else {
            uret = UX_FUNCTION_NOT_SUPPORTED;
        }
    } else {
        uret = UX_FUNCTION_NOT_SUPPORTED;
    }

    return uret;

}
