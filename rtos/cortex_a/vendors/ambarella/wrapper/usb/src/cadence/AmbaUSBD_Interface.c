/**
 *  @file AmbaUSBD_Interface.c
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
 *  @details USB device driver UX interface for Cadence USB SSP IP
 */

#include <AmbaTypes.h>
#define AMBA_KAL_SOURCE_CODE
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_Buffer.h>
#include <usbx/ux_device_stack.h>

#define USBD_EP_ISO_MAX_BUFFER_PER_XFER    (((UINT32)0x02UL) << 13U)           // [13:0] for Tx Bytes

static UINT32 udc_get_endpoint_type(const UX_SLAVE_ENDPOINT *UsbxEndpoint)
{
    return (UsbxEndpoint->ux_slave_endpoint_descriptor.bmAttributes & (UINT)UX_MASK_ENDPOINT_TYPE) |
           (UsbxEndpoint->ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)UX_ENDPOINT_DIRECTION);
}


/**
 * Called by driver to allocate buffer for one UX endpoint.
 * */
void USBD_UdcEndpointBufferAllocate(UX_SLAVE_ENDPOINT *Endpoint)
{
    UINT32 uret;
    UINT8 *buffer_ptr;
    UINT32 buffer_size;
    UINT32 ux_ep_type = udc_get_endpoint_type(Endpoint);

    switch (ux_ep_type) {
        case UX_CONTROL_ENDPOINT:
            uret = USB_BufferU8Attach(UBUF_USBX_CTRL, &buffer_ptr);
            if (uret == USB_ERR_SUCCESS) {
                buffer_size = USB_BufferSizeGet(UBUF_USBX_CTRL);
            } else {
                buffer_size = 0;
            }
            break;
        case UX_BULK_ENDPOINT_IN:
        case UX_INTERRUPT_ENDPOINT_IN:
            uret = USB_BufferU8Attach(UBUF_USBX_BULK_IN, &buffer_ptr);
            if (uret == USB_ERR_SUCCESS) {
                buffer_size = USB_BufferSizeGet(UBUF_USBX_BULK_IN);
                (void)uxd_ep_in_max_dlen_set(buffer_size);
            } else {
                buffer_size = 0;
            }
            break;
        case UX_BULK_ENDPOINT_OUT:
        case UX_INTERRUPT_ENDPOINT_OUT:
            uret = USB_BufferU8Attach(UBUF_USBX_BULK_OUT, &buffer_ptr);
            if (uret == USB_ERR_SUCCESS) {
                buffer_size = USB_BufferSizeGet(UBUF_USBX_BULK_OUT);
                (void)uxd_ep_out_max_dlen_set(buffer_size);
            } else {
                buffer_size = 0;
            }
            break;
        case UX_ISOCHRONOUS_ENDPOINT_IN:
        {
            UINT32 desc_number = (USBD_EP_ISO_MAX_BUFFER_PER_XFER) / (Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize & 0x7FFU);

            uret = USB_ERR_SUCCESS;
            // Won't attach the data buffer because it's provided by the application.
            // The driver only decides the maximum buffer size.
            buffer_size = Endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize & 0x7FFU;
            buffer_size = desc_number * buffer_size;
            buffer_ptr  = NULL;
        }
            break;
        default:
            buffer_ptr  = NULL;
            buffer_size = 0;
            uret        = USB_ERR_FAIL;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] EP type 0x%x IS NOT IMPLEMENT!", ux_ep_type, 0, 0, 0, 0);
            break;
    }

    if (uret == USB_ERR_SUCCESS) {
        UX_SLAVE_TRANSFER *transfer_request;
        UINT32 addr_transfer_request;
        UINT32 addr_buffer;
        transfer_request                                         = &Endpoint->ux_slave_endpoint_transfer_request;
        transfer_request->ux_slave_transfer_request_data_pointer = buffer_ptr;
        transfer_request->max_buffer_size                        = buffer_size;

        // Create endpoint-specified semaphore.
        if (USB_UtilitySemaphoreCreate(&transfer_request->ux_slave_transfer_request_semaphore, 0) != USB_ERR_SUCCESS) {
            // error handling
            UINT32 LogAddr        = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] Failed to create semaphore for endpoint 0x%x", LogAddr, 0, 0, 0, 0);
        }

        addr_transfer_request = IO_UtilityPtrToU32Addr(transfer_request);
        addr_buffer           = IO_UtilityPtrToU32Addr(buffer_ptr);

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
            "USBD_UdcEndpointBufferAllocate(): transfer_request 0x%X, 0x%X, size %d",
            addr_transfer_request,
            addr_buffer,
            buffer_size, 0 ,0);

    } else {
        // error handling
        UINT32 LogAddr        = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] Failed to allocate buffer for endpoint 0x%x", LogAddr, 0, 0, 0, 0);
    }
}


/**
 * Called by driver to release buffer for one UX endpoint.
 * */
void USBD_UdcEndpointBufferDestroy(UX_SLAVE_ENDPOINT *Endpoint)
{
    UX_SLAVE_TRANSFER *transfer_request = &Endpoint->ux_slave_endpoint_transfer_request;
    const UINT8 *buffer_ptr             = transfer_request->ux_slave_transfer_request_data_pointer;
    UINT32 ux_ep_type                   = udc_get_endpoint_type(Endpoint);

    switch (ux_ep_type) {
    case UX_CONTROL_ENDPOINT:
        USB_BufferU8Detach(UBUF_USBX_CTRL, buffer_ptr);
        break;
    case UX_BULK_ENDPOINT_IN:
    case UX_INTERRUPT_ENDPOINT_IN:
        USB_BufferU8Detach(UBUF_USBX_BULK_IN, buffer_ptr);
        break;
    case UX_BULK_ENDPOINT_OUT:
    case UX_INTERRUPT_ENDPOINT_OUT:
        USB_BufferU8Detach(UBUF_USBX_BULK_OUT, buffer_ptr);
        break;
    case UX_ISOCHRONOUS_ENDPOINT_IN:
        // do nothting here.
        break;
    default:
        // no action
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer destroy] EP Mask 0x%x IS NOT IMPLEMENT!", ux_ep_type, 0, 0, 0, 0);
        break;
    }

    transfer_request->ux_slave_transfer_request_data_pointer = NULL;

    if (USB_UtilitySemaphoreDelete(&transfer_request->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
        // error handling
        UINT32 logical_address = Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[endpoint_buffer allocate] Failed to delete semaphore for endpoint 0x%x", logical_address, 0, 0, 0, 0);
    }

    // reset the semaphore.
    USB_UtilityMemorySet(&transfer_request->ux_slave_transfer_request_semaphore, 0, sizeof(AMBA_KAL_SEMAPHORE_t));
}


/**
 * Called by driver to get endpoint map information by logical endpoint address.
 * */
UDC_ENDPOINT_MAP_s *USBD_UdcEndpointMapGet(USBD_UDC_s *Udc, UINT32 LogAddr)
{
    static UDC_ENDPOINT_MAP_s ep0_map =
    {
        0, // Logical Endpoint address
        0, // UDC20 Endpoint Index
        0, // Physical Endpoint address
        0, // Endpoint Direction
        1  // Used
    };

    UDC_ENDPOINT_MAP_INFO_s *map_info = &Udc->EndpointMapInfo;
    UINT32 i;
    UDC_ENDPOINT_MAP_s *ret_map = NULL;

    if (LogAddr == 0U) {
        // control endpoint would be 0, and the map might be empty in some flow.
        // make the map for endpoint 0 always available.
        ret_map = &ep0_map;
    } else {
        for (i = 0; i < UDC_MAX_ENDPOINT_NUM; i++) {
            UDC_ENDPOINT_MAP_s *map = &(map_info->EndpointMap[i]);
            if ((map->LogicalAddr == LogAddr) && (map->Used != 0U)) {
                ret_map = map;
                break;
            }
        }
    }

    if (ret_map == NULL) {
        // error handling
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_UdcEndpointMapGet(): No map for logical address 0x%X.", LogAddr, 0, 0, 0, 0);
    }

    return ret_map;
}

UX_SLAVE_TRANSFER *USBDCDNS3_UxTransferGet(const struct USBD_UDC_ts *Udc, UINT32 LogicalAddress)
{
    //UDC_ENDPOINT_MAP_s  *udc_ep_map;
    const UDC_ENDPOINT_INFO_s *udc_ep_info;
    UX_SLAVE_ENDPOINT         *usbx_endpoint;
    UX_SLAVE_TRANSFER         *tr = NULL;

    if (Udc != NULL) {
        UINT32 phy_endpoint_idx = USBDCDNS3_PhyEndpointIdxGet(LogicalAddress);

        //udc_ep_map = USBD_UdcEndpointMapGet(Udc, LogicalAddress);
        //if (udc_ep_map != NULL) {
            // Get the physical endpoint associated with this endpoint.
            udc_ep_info              = &Udc->DcdEndpointsInfoArray[phy_endpoint_idx];
            //udc_ep_info->InterruptStatus = AmbaRTSL_UsbGetEpInStatus(physical_endpoint_index);
            usbx_endpoint = udc_ep_info->UxEndpoint;
            if (usbx_endpoint != NULL) {
                tr = &usbx_endpoint->ux_slave_endpoint_transfer_request;
            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_UxTransferGet(): usbx endpoint for 0x%X is NULL!", LogicalAddress, 0, 0, 0, 0);
            }
        //} else {
        //    #ifdef USB_PRINT_SUPPORT
        //    USB_UtilityPrintUInt5("USBD_UxTransferGet(): endpoint map for 0x%X is NULL!", LogicalAddress, 0, 0, 0, 0);
        //    #endif
        //}
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBD_UxTransferGet(): Udc is NULL!", 0, 0, 0, 0, 0);
    }

    return tr;
}


void USBD_UdcDeviceDataConnSet(UINT32 value)
{
    (void)value;
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBD_UdcDeviceDataConnSet(): No implement.",  0, 0, 0, 0, 0);
}

UINT32 USBD_ConnectChgInit(void)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBD_ConnectChgInit(): No implement.",  0, 0, 0, 0, 0);
    return 0;
}

static UINT32 usbd_cb_func_setup(struct USBD_UDC_ts *Udc, const USB_CH9_SETUP_s *SetupInfo)
{
    UX_SLAVE_TRANSFER   *tr;
    UINT8               *data_ptr;
    UINT32               uret = 0;
    UINT8                is_request_in = SetupInfo->bRequest & (UINT8)UX_REQUEST_IN;

    AmbaMisra_TouchUnused(Udc);

    tr  = USBDCDNS3_UxTransferGet(Udc, 0);

    if (tr != NULL) {

        // fill setup data into USBX control endpoint
        data_ptr      = tr->ux_slave_transfer_request_setup;

        data_ptr[0] = SetupInfo->bmRequestType;
        data_ptr[1] = SetupInfo->bRequest;
        data_ptr[2] = (UINT8)(SetupInfo->wValue);
        data_ptr[3] = (UINT8)(SetupInfo->wValue >> 8U);
        data_ptr[4] = (UINT8)(SetupInfo->wIndex);
        data_ptr[5] = (UINT8)(SetupInfo->wIndex >> 8U);
        data_ptr[6] = (UINT8)(SetupInfo->wLength);
        data_ptr[7] = (UINT8)(SetupInfo->wLength >> 8U);

        tr->ux_slave_transfer_request_actual_length = 8;

        // Mark the phase as SETUP.
        tr->ux_slave_transfer_request_type = UX_TRANSFER_PHASE_SETUP;

        // Mark the transfer as successful.
        tr->ux_slave_transfer_request_completion_code = UX_SUCCESS;

        uret = _ux_device_stack_control_request_process(tr);

        // status stage is handled in drvier so we just need to handl USBX part
        if (uret == 0U) {
            // Handle Status Phase.
            if (is_request_in != 0U) {
                // control-in request
                // receive a ZLP
                tr->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;
            } else {
                // control-out request
                // send a ZLP
                tr->ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_OUT;
            }
        }

        // notify application if needed
        if ((SetupInfo->bmRequestType & CH9_USB_REQ_TYPE_MASK) == CH9_USB_REQ_TYPE_STANDARD) {
            // we only process standard requests
            if (SetupInfo->bRequest == USB_REQ_CLEAR_FEATURE) {
                // clear feature or set feature
                USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_CLEAR_FEATURED,
                                          SetupInfo->bmRequestType,
                                          SetupInfo->wValue,
                                          SetupInfo->wIndex);
            } else if (SetupInfo->bRequest == USB_REQ_SET_FEATURE) {
                // clear feature or set feature
                USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_SET_FEATURED,
                                          SetupInfo->bmRequestType,
                                          SetupInfo->wValue,
                                          SetupInfo->wIndex);
            } else if (SetupInfo->bRequest == USB_REQ_SET_CONFIGURATION) {
                USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_CONFIGURED,
                                          SetupInfo->wValue,
                                          0, 0);
            } else if (SetupInfo->bRequest == USB_REQ_SET_INTERFACE) {
                USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_SET_INTERFACE,
                                          SetupInfo->wIndex,
                                          SetupInfo->wValue,
                                          0);
            } else {
                // do nothing
            }
        }

    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "usbd_cb_func_setup(): No transfer for control endpoint.",  0, 0, 0, 0, 0);
        uret = USB_ERR_XHCI_NO_TRANSFER_REQUEST;
    }

    return uret;
}

static void usbd_cb_func_connect(XHC_CDN_DRV_RES_s *DrvRes)
{
    AmbaMisra_TouchUnused(DrvRes);
    USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_CONNECT, 0, 0, 0);
}

static void usbd_cb_func_disconnect(XHC_CDN_DRV_RES_s *DrvRes)
{
    AmbaMisra_TouchUnused(DrvRes);
    if (_ux_device_stack_disconnect() != 0U) {
        // (todo) shall we check this?
    }
    USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_DISCONNECT, 0, 0, 0);
}

static void usbd_cb_func_reset(XHC_CDN_DRV_RES_s *DrvRes)
{
    AmbaMisra_TouchUnused(DrvRes);
    if (_ux_device_stack_disconnect() != 0U) {
        // (todo) shall we check this?
    }
    USBD_SystemAppEventNotify(USBD_SYSTEM_EVENT_RESET, 0, 0, 0);
}

UINT32 USBD_UdcInit(void)
{
    static USBDCDNS3_CALLBACKS_s usbd_callbacks = {
        .Connect    = usbd_cb_func_connect,
        .Disconnect = usbd_cb_func_disconnect,
        .Setup      = usbd_cb_func_setup,
        .Reset      = usbd_cb_func_reset,
    };

    return USBDCDNS3_ControllerInit(&usbd_callbacks);
}

UINT32 USBD_IntInit(void)
{
    return USBDCDNS3_IntInit();
}

UINT32 USBD_IntDeInit(void)
{
    return USBDCDNS3_IntDeInit();
}

UINT32 USBD_UdcDeInit(void)
{
    return USBDCDNS3_ControllerDeInit();
}

UINT32 USBD_ConnectChgDeInit(void)
{
    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBD_ConnectChgDeInit(): No implement.");
    return 0x0;
}

UINT32 USBD_UdcEnumSpeedGet(void)
{
    USBD_UDC_s *udc;

    UINT32 uret_func  = USBDCDNS3_UdcInstanceSafeGet(&udc);
    UINT32 uret_speed = 0;

    if (uret_func == 0U) {
        UINT32 ch9_speed = udc->CDNDrvRes.ActualSpeed;
        switch(ch9_speed) {
            case CH9_USB_SPEED_UNKNOWN: uret_speed = 0; break;
            case CH9_USB_SPEED_LOW: uret_speed = 0; break;
            case CH9_USB_SPEED_FULL: uret_speed = 0; break;
            case CH9_USB_SPEED_HIGH: uret_speed = USB_CONNECT_SPEED_HIGH; break;
            case CH9_USB_SPEED_SUPER: uret_speed = USB_CONNECT_SPEED_SUPER; break;
            case CH9_USB_SPEED_SUPER_PLUS: uret_speed = USB_CONNECT_SPEED_SUPER_PLUS; break;
            default: uret_speed = 0; break;
        }
    }
    return uret_speed;
}

UINT32 USBD_ConnectChgVbusStatusGet(void)
{
    USBD_UDC_s *udc;

    UINT32 uret_func  = USBDCDNS3_UdcInstanceSafeGet(&udc);
    UINT32 port_id;
    UINT32 uret = 0;

    if (uret_func == 0U) {
        port_id = udc->CDNDrvRes.ActualPort;
        if (port_id != 0U) {
            port_id = port_id - 1U;
            if (port_id == USBCDNS3_DEV_MODE_32_PORT_IDX) {
                // if USB32 port is active, VBUS is connected.
                uret = 1;
            } else {
                // if USB20 port is active, check port reset(PR) bit
                // PR = 1: vbus disconnected
                // PR = 0: vbus connected
                UINT32 port_status = USBDCDNS3_PortStatusRead(&udc->CDNDrvRes, port_id);
                if ((port_status & XHCI_PORTSC_PR_MASK) == 0U) {
                    uret = 1;
                }
            }
        }
    }
    return uret;

}

UINT32 USBD_UdcIsConfigured(void)
{
    USBD_UDC_s *udc;

    UINT32 uret_func  = USBDCDNS3_UdcInstanceSafeGet(&udc);
    UINT8  flag_configured;
    UINT32 uret = 0;

    if (uret_func == 0U) {
        flag_configured = udc->CDNDrvRes.FlagDevConfigured;
        if (flag_configured != 0U) {
            uret = 1;
        }
    }
    return uret;
}

void USB_PhyPllEnable(UINT32 OnOff, UINT32 PortMode)
{
    (void)OnOff;
    (void)PortMode;
    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USB_PhyPllEnable(): No implement.");
    return;
}

void USB_PhyPhy0OwnerSet(USB0_PORT_OWNER_e Owner)
{
    (void)Owner;
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USB_PhyPhy0OwnerSet(): No implement.",  0, 0, 0, 0, 0);
    return;
}

/**
 * Called by system to get UDC instance
 * */
UINT32 USBD_UdcUdcInstanceGet(USBD_UDC_s **Udc)
{
    return USBDCDNS3_UdcInstanceSafeGet(Udc);
}

UINT32 USBD_DrvEndpointCreate(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *Endpoint)
{
    return USBDCDNS3_EndpointCreate(Udc, Endpoint);
}

UINT32 USBD_DrvEndpointStatusGet(const USBD_UDC_s *Udc, UINT32 EndpointIndex)
{
    return USBDCDNS3_EndpointStatusGet(Udc, EndpointIndex);
}

void USBD_DrvEndpointStall(const UX_SLAVE_ENDPOINT *Endpoint)
{
    USBDCDNS3_EndpointStall(Endpoint);
    return;
}

UINT32 USBD_DrvEndpointReset(USBD_UDC_s *Udc, const UX_SLAVE_ENDPOINT *Endpoint)
{
    return USBDCDNS3_EndpointReset(Udc, Endpoint);
}

UINT32 USBD_DrvEndpointDestroy(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *Endpoint)
{
    return USBDCDNS3_EndpointDestroy(Udc, Endpoint);
}

UINT32 USBD_DrvTransferAbort(const UX_SLAVE_TRANSFER *TransferRequest)
{
    return USBDCDNS3_TransferAbort(TransferRequest);
}

UINT32 USBD_DrvTransferRequest(USBD_UDC_s *Udc, UX_SLAVE_TRANSFER *TransferRequest)
{
    return USBDCDNS3_TransferRequest(Udc, TransferRequest);
}

UINT32 USBD_DrvFrameNumberGet(USBD_UDC_s *Udc, UINT32 *FrameNumber)
{
    return USBDCDNS3_FrameNumberGet(Udc, FrameNumber);
}


