/**
 *  @file ux_hcd_xhci_destroy.c
 *
 *  Copyright (c) 2022 Ambarella International LP
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
 *  @details Ambarella USBX XHCI driver for release resourcres
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

#include <ux_hcd_xhci.h>

UX_HCD *ux_hcd_xhci_hcd_find(void)
{
    UINT                   hcd_index;
    UX_HCD                *hcd = NULL;
    static char            hcd_xhci_name[] = "ux_hcd_xhci";
    const char            *ptr_char = hcd_xhci_name;
    UCHAR                 *hcd_xhci_name_uchar;
    
    // expand: hcd_xhci_name_uchar = (char *) hcd_xhci_name;
    AmbaMisra_TypeCast(&hcd_xhci_name_uchar, &ptr_char);

    for(hcd_index = 0; hcd_index < _ux_system_host -> ux_system_host_registered_hcd; hcd_index++) {
        /* Pickup HCD pointer.  */
        hcd =  &_ux_system_host->ux_system_host_hcd_array[hcd_index];
        if (_ux_utility_string_compare((const char*)hcd->ux_hcd_name,
                                       hcd_xhci_name,
                                       _ux_utility_string_length_get(hcd_xhci_name_uchar)) == 0) {
            break;
        }       
    }
    return hcd;
}

UINT  ux_hcd_xhci_destroy(void)
{
    const UX_HCD_XHCI     *hcd_xhci = NULL;
    UINT                   uret = 0;
    const UX_HCD          *hcd;
    
    hcd = ux_hcd_xhci_hcd_find();

    if (hcd == NULL) {
        _ux_utility_print("ux_hcd_xhci_destroy(): xhci hcd is not registered");
        uret = 0xFFFFFFFFU;
    } else {
        XHC_CDN_DRV_RES_s *drv_res;

        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_destroy(): 2");

        // expand: hcd_xhci = (UX_HCD_XHCI*)hcd -> ux_hcd_controller_hardware;
        AmbaMisra_TypeCast(&hcd_xhci, &hcd->ux_hcd_controller_hardware);

        drv_res = hcd_xhci->cdns_drv_res;
        if (drv_res != NULL) {
            // delete semaphore
            if (USB_UtilitySemaphoreDelete(&drv_res->SemaphoreCommandQueue) != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_destroy(): can't delete semaphore for command queue");
                uret = UX_SEMAPHORE_ERROR;
            }
            if (USB_UtilitySemaphoreDelete(&drv_res->SemaphoreEp0Transfer) != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_destroy(): can't delete semaphore for EP0 queue");
                if (uret == 0U) {
                    uret = UX_SEMAPHORE_ERROR;
                }
            }
        }
    }

    return uret;
}


