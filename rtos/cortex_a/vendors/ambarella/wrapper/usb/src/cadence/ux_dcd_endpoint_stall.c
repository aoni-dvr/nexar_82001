/**
 *  @file ux_dcd_endpoint_stall.c
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
 *  @details USB device driver for UX endpoint stall functions.
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
 * Called by USBX to stall physical Endpoint.
 * */
void USBDCDNS3_EndpointStall(const UX_SLAVE_ENDPOINT *Endpoint)
{
    UDC_ENDPOINT_INFO_s *udc_ed_info;
    XHC_CDN_DRV_RES_s   *drv_res = NULL;
    USBD_UDC_s          *udc;
    UINT32               uret;

    if (USBDCDNS3_UdcInstanceSafeGet(&udc) == 0U) {
        drv_res = &udc->CDNDrvRes;
    }

    if ((Endpoint != NULL) && (drv_res != NULL)) {
        udc_ed_info = USB_UtilityVoidP2UdcEd(Endpoint->ux_slave_endpoint_ed);
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBDCDNS3_EndpointStall(): LogicalAddress = 0x%X, PhysicalIndex = %d",
                                    Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                    udc_ed_info->PhysicalEpIndex,
                                    0, 0, 0);
        if (udc_ed_info->PhysicalEpIndex != 1U) {
            uret = USBCDNS3_EndpointFeatureSet(drv_res, (UINT8)udc_ed_info->PhysicalEpIndex, 1);
            if (uret != 0U) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBDCDNS3_EndpointStall(): failed, code 0x%X", uret, 0, 0, 0, 0);
            } else {
                // set STALLED state
                udc_ed_info->EpState |= USBD_UDC_ED_STATUS_STALLED;
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBDCDNS3_EndpointStall(): no stall for endpoint 0", 0, 0, 0, 0, 0);
        }
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBDCDNS3_EndpointStall(): NULL pointer");
    }
}
/** @} */
