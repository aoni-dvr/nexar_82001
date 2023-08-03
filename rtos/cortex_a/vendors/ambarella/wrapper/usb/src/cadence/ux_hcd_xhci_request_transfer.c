/**
 *  @file ux_hcd_xhci_request_transfer.c
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
 *  @details Ambarella USBX XHCI driver for transferring data
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

#include <ux_hcd_xhci.h>

static UINT  ux_hcd_xhci_request_control_transfer(const UX_HCD_XHCI *HcdXhci, UX_TRANSFER *Transfer)
{
    UINT uret;

    if ((Transfer != NULL) && (HcdXhci != NULL)) {
        UINT function = Transfer->ux_transfer_request_function;
        if (function == (UINT32)UX_SET_ADDRESS) {
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "ux_hcd_xhci_request_control_transfer(): set address.");
            uret = USBHCDNS3_DeviceAddress(HcdXhci->cdns_drv_res, 1);
        } else {
            USB_CH9_SETUP_s setup_packet;
            setup_packet.bmRequestType = (UINT8)Transfer->ux_transfer_request_type;
            setup_packet.bRequest      = (UINT8)function;
            setup_packet.wIndex        = (UINT16)Transfer->ux_transfer_request_index;
            setup_packet.wValue        = (UINT16)Transfer->ux_transfer_request_value;
            setup_packet.wLength       = (UINT16)Transfer->ux_transfer_request_requested_length;

            USB_UtilityCacheFlushUInt8(Transfer->ux_transfer_request_data_pointer, (UINT32)Transfer->ux_transfer_request_requested_length);
            uret = USBHCDNS3_ControlTransfer(HcdXhci->cdns_drv_res, &setup_packet, Transfer->ux_transfer_request_data_pointer);
            if (uret == 0U) {
                Transfer->ux_transfer_request_actual_length = Transfer->ux_transfer_request_requested_length;
                USB_UtilityCacheInvdUInt8(Transfer->ux_transfer_request_data_pointer, (UINT32)Transfer->ux_transfer_request_requested_length);
            }
        }

    } else {
        uret = UX_FUNCTION_NOT_SUPPORTED;
    }

    return uret;
}

static void transfer_complete_bulk(XHC_CDN_DRV_RES_s *Arg, UINT32 SlotID, UINT32 EpIndex, UINT32 Status, const XHCI_RING_ELEMENT_s *EventPtr, UINT8 *Buffer, UINT32 ActualLength)
{

    (void)SlotID;
    (void)EpIndex;
    (void)Status;
    (void)EventPtr;
    (void)ActualLength;

    AmbaMisra_TouchUnused(Arg);
    AmbaMisra_TouchUnused(Buffer);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "transfer_complete_bulk(): SlotID %d, EpIndex %d, status 0x%X, actualLength %d", SlotID, EpIndex, Status, ActualLength, 0);

    if (Arg != NULL) {
        USBD_UDC_s *udc = Arg->Udc;
        if (udc != NULL) {
            UHC_ENDPOINT_INFO_s *ep_info = &udc->HostEndpointsInfoArray[EpIndex];
            UX_TRANSFER *ux_request = ep_info->Transfer;

            // invalidate cache to get real data from memory
            USB_UtilityCacheInvdUInt8(ux_request->ux_transfer_request_data_pointer, (UINT32)ux_request->ux_transfer_request_requested_length);

            if (Status != 0U) {
                if (Status == XHCI_TRB_CMPL_STALL_ERROR) {
                    ux_request->ux_transfer_request_completion_code = UX_TRANSFER_STALLED;
                    ep_info->EpState = UX_TRANSFER_STALLED;
                } else {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "transfer_complete_bulk(): Unknown status code %d", Status, 0, 0, 0, 0);
                    ux_request->ux_transfer_request_completion_code = Status;
                }
                //ux_request->ux_transfer_request_completion_code = Status;
                ux_request->ux_transfer_request_status = UX_SUCCESS;
                ux_request->ux_transfer_request_actual_length = 0;
            } else {
                ux_request->ux_transfer_request_completion_code = UX_SUCCESS;
                ux_request->ux_transfer_request_status = UX_SUCCESS;
                ux_request->ux_transfer_request_actual_length = ActualLength;
                ep_info->EpState = UX_SUCCESS;
            }
            if (USB_UtilitySemaphoreGive(&ux_request->ux_transfer_request_semaphore) != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "transfer_complete_bulk(): fail to give semaphore");
            }
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "transfer_complete_bulk(): UDC is NULL");
        }
    }
}


static UINT  ux_hcd_xhci_request_bulk_transfer(const UX_HCD_XHCI *HcdXhci, UX_TRANSFER *Transfer)
{
    UINT         uret = 0;
    const UINT8 *data_ptr;
    UINT         logical_address;
    UINT         physical_address;
    const USBD_UDC_s          *udc;
    const UHC_ENDPOINT_INFO_s *ep_info;

    if ((Transfer != NULL) && (HcdXhci != NULL)) {
        data_ptr         = Transfer->ux_transfer_request_data_pointer;
        logical_address  = Transfer->ux_transfer_request_endpoint->ux_endpoint_descriptor.bEndpointAddress;
        physical_address = USBDCDNS3_PhyEndpointIdxGet(logical_address);

        udc = HcdXhci->cdns_drv_res->Udc;
        if (udc != NULL) {
            ep_info = &udc->HostEndpointsInfoArray[physical_address];
            if (ep_info->Transfer != Transfer) {
                USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_request_bulk_transfer(): Transfer pointer mismatch 0x%X != 0x%X",
                                    IO_UtilityPtrToU64Addr(ep_info->Transfer),
                                    IO_UtilityPtrToU64Addr(Transfer),
                                    (UINT64)0, (UINT64)0, (UINT64)0);
            }

            USB_UtilityCacheFlushUInt8(data_ptr, (UINT32)Transfer->ux_transfer_request_requested_length);

            if (ep_info->EpState == (UINT32)UX_TRANSFER_STALLED) {
                Transfer->ux_transfer_request_completion_code = UX_TRANSFER_STALLED;
                Transfer->ux_transfer_request_status = UX_SUCCESS;
                Transfer->ux_transfer_request_actual_length = 0;
                uret = USB_UtilitySemaphoreGive(&Transfer->ux_transfer_request_semaphore);
            } else {
                uret = USBCDNS3_DataTransfer(HcdXhci->cdns_drv_res,
                                           physical_address,
                                           IO_UtilityPtrToU64Addr(data_ptr),
                                           (UINT32)Transfer->ux_transfer_request_requested_length,
                                           transfer_complete_bulk);
            }

            // upper layer protocol will wait for the completion

        }

    } else {
        uret = UX_FUNCTION_NOT_SUPPORTED;
    }

    return uret;
}

UINT  ux_hcd_xhci_request_transfer(UX_HCD_XHCI *HcdXhci, UX_TRANSFER *Transfer)
{
    UINT uret;

    if ((Transfer != NULL) && (HcdXhci != NULL)) {

        const UX_ENDPOINT            *ux_endpoint;
        const UX_ENDPOINT_DESCRIPTOR *ux_ep_desc;
        UINT                    ep_type;

        AmbaMisra_TouchUnused(HcdXhci);

        ux_endpoint = Transfer->ux_transfer_request_endpoint;
        if (ux_endpoint != NULL) {
            ux_ep_desc  = &ux_endpoint->ux_endpoint_descriptor;
            Transfer->ux_transfer_request_actual_length =  0;

            ep_type = ux_ep_desc->bmAttributes & UX_MASK_ENDPOINT_TYPE;

            switch (ep_type) {
                case UX_CONTROL_ENDPOINT:
                    uret = ux_hcd_xhci_request_control_transfer(HcdXhci, Transfer);
                    break;
                case UX_BULK_ENDPOINT:
                    uret = ux_hcd_xhci_request_bulk_transfer(HcdXhci, Transfer);
                    break;
                case UX_INTERRUPT_ENDPOINT:
                case UX_ISOCHRONOUS_ENDPOINT:
                default:
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ux_hcd_xhci_request_transfer(): no support for ep_type %d.", ep_type, 0, 0, 0, 0);
                    uret = UX_ERROR;
                    break;
            }
        } else {
            uret = UX_FUNCTION_NOT_SUPPORTED;
        }
    } else {
        uret = UX_FUNCTION_NOT_SUPPORTED;
    }

    return uret;
}
