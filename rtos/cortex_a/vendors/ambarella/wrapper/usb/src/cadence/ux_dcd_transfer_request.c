/**
 *  @file ux_dcd_transfer_request.c
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
 *  @details USB kernel driver to do UX transfer request.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_Buffer.h>

static UINT32 ux_endpoint_type_get(const UX_SLAVE_ENDPOINT *Endpoint)
{
    return (Endpoint->ux_slave_endpoint_descriptor.bmAttributes & (UINT32)UX_MASK_ENDPOINT_TYPE) |
            (Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & (UINT32)UX_ENDPOINT_DIRECTION);
}

static UINT32 semaphore_reset(UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 semaphore_count;
    UINT32 uret = USB_ERR_SUCCESS;

    if (USB_UtilitySemaphoreQuery(&TransferRequest->ux_slave_transfer_request_semaphore, &semaphore_count) == USB_ERR_SUCCESS) {
        if (semaphore_count != 0UL) {
            UINT32 i;
            for (i = 0; i < semaphore_count; i++) {
                if (USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore, USB_NO_WAIT) != USB_ERR_SUCCESS) {
                    // shall not be here.
                }
            }
        }
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "semaphore_reset: can't get semaphore info");
        uret = USB_ERR_FAIL;
    }

    return uret;
}

//static UX_SLAVE_TRANSFER *ux_rx_request = NULL;

static void rx_complete_func(XHC_CDN_DRV_RES_s *Arg, UINT32 SlotID, UINT32 EpIndex, UINT32 Status, const XHCI_RING_ELEMENT_s *EventPtr, UINT8 *Buffer, UINT32 ActualLength)
{

    (void)SlotID;
    (void)EpIndex;
    (void)Status;
    (void)EventPtr;
    (void)ActualLength;

    AmbaMisra_TouchUnused(Arg);
    AmbaMisra_TouchUnused(Buffer);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "rx_complete_func(): SlotID %d, EpIndex %d, status 0x%X, actualLength %d", SlotID, EpIndex, Status, ActualLength, 0);

    if (Arg != NULL) {
        const USBD_UDC_s *udc = Arg->Udc;
        if (udc != NULL) {
            const UDC_ENDPOINT_INFO_s *udc_ep_info = &udc->DcdEndpointsInfoArray[EpIndex];
            UX_SLAVE_TRANSFER *ux_request = &udc_ep_info->UxEndpoint->ux_slave_endpoint_transfer_request;
            ux_request->ux_slave_transfer_request_completion_code = UX_SUCCESS;
            ux_request->ux_slave_transfer_request_status = UX_SUCCESS;
            ux_request->ux_slave_transfer_request_actual_length = ActualLength;
            if (USB_UtilitySemaphoreGive(&ux_request->ux_slave_transfer_request_semaphore) != 0U) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "rx_complete_func(): fail to give semaphore, SlotID %d, EpIndex %d, status 0x%X, actualLength %d", SlotID, EpIndex, Status, ActualLength, 0);
            }
        }
    }
}

static void tx_complete_func(XHC_CDN_DRV_RES_s *Arg, UINT32 SlotID, UINT32 EpIndex, UINT32 Status, const XHCI_RING_ELEMENT_s *EventPtr, UINT8 *Buffer, UINT32 ActualLength)
{

    (void)SlotID;
    (void)EpIndex;
    (void)Status;
    (void)EventPtr;
    (void)ActualLength;

    AmbaMisra_TouchUnused(Arg);
    AmbaMisra_TouchUnused(Buffer);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "tx_complete_func(): SlotID %d, EpIndex %d, status 0x%X, actualLength %d", SlotID, EpIndex, Status, ActualLength, 0);

    if (Arg != NULL) {
        const USBD_UDC_s *udc = Arg->Udc;
        if (udc != NULL) {
            const UDC_ENDPOINT_INFO_s *udc_ep_info = &udc->DcdEndpointsInfoArray[EpIndex];
            UX_SLAVE_TRANSFER *ux_request = &udc_ep_info->UxEndpoint->ux_slave_endpoint_transfer_request;
            ux_request->ux_slave_transfer_request_completion_code = UX_SUCCESS;
            ux_request->ux_slave_transfer_request_status = UX_SUCCESS;
            ux_request->ux_slave_transfer_request_actual_length = ActualLength;
            if (USB_UtilitySemaphoreGive(&ux_request->ux_slave_transfer_request_semaphore) != 0U) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "tx_complete_func(): fail to give semaphore, SlotID %d, EpIndex %d, status 0x%X, actualLength %d", SlotID, EpIndex, Status, ActualLength, 0);
            }
        }
    }
}

static void cdns3_controller_request_remove_all(XHC_CDN_DRV_RES_s *DrvRes, UINT8 PhyEpIndex, UINT32 FlagWait)
{
    UINT32 func_uret;
    // To drop all requests from controller:
    //     1. stop endpoint
    //     2. reset Transfer Ring Dequeue Pointer for the endpoint
    //         a. the endpoint will be re-enabled then.
    // 1. stop endpoint
    func_uret = USBDCDNS3_EndpointStop(DrvRes, PhyEpIndex, FlagWait);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_controller_request_remove_all(): RX [0x%X] failed to stop endpoint 0x%X", PhyEpIndex, func_uret, 0, 0, 0);
    }

    // 2. reset transfer ring dequeue pointer
    func_uret = USBDCDNS3_TRDequeuePtrSet(DrvRes, PhyEpIndex, FlagWait);
    if (func_uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_controller_request_remove_all(): RX [0x%X] failed to set tr_dequeue pointer 0x%X", PhyEpIndex, func_uret, 0, 0, 0);
    }
}

static UINT32 cdns3_rx_process_bulk(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpMask)
{

    UINT32 uret;
    UINT32 transfer_size;
    USBD_UDC_s *udc;
    XHC_CDN_DRV_RES_s *drv_res = NULL;
    const UINT8 *data_ptr;
    const UX_SLAVE_ENDPOINT *usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

    (void)EpMask;

    if (USBDCDNS3_UdcInstanceSafeGet(&udc) == 0U) {
        drv_res = &udc->CDNDrvRes;
    }

    transfer_size  = (UINT32)TransferRequest->ux_slave_transfer_request_requested_length;

    TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

    if (semaphore_reset(TransferRequest) == 0U) {
        // invalidate data buffer
        data_ptr = TransferRequest->ux_slave_transfer_request_data_pointer;
        USB_UtilityCacheInvdUInt8(data_ptr, transfer_size);

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "cdns3_rx_process_bulk(): PhysicalEpIndex = %d", UdcEpInfo->PhysicalEpIndex, 0, 0, 0, 0);

        uret = USBCDNS3_DataTransfer(drv_res,
                                   (UINT8)UdcEpInfo->PhysicalEpIndex,
                                   IO_UtilityPtrToU64Addr(data_ptr),
                                   transfer_size,
                                   rx_complete_func);

        if (uret == 0U) {
            // need to wait until complete
            // C. Waiting for ISR give semaphore or semaphore time out.
            uret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                             (UINT32)TransferRequest->ux_slave_transfer_request_timeout);

            if (uret == 0U) {

                // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
                if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                                            "cdns3_rx_process_bulk(): [0x%X] completion_code = 0x%x",
                                            usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                            (UINT32)TransferRequest->ux_slave_transfer_request_completion_code, 0, 0, 0);
                    uret = (UINT32)TransferRequest->ux_slave_transfer_request_completion_code;

                    // If the transfer failed, the request might be still queued in controller.
                    // We need to drop this request from controller:
                    if (TransferRequest->ux_slave_transfer_request_completion_code == (ULONG)UX_TRANSFER_BUS_RESET) {
                        // for BUS Reset, no need to do this (controller always reports failed)
                        //cdns3_controller_request_remove_all(drv_res, (UINT8)UdcEpInfo->PhysicalEpIndex, 0);
                    } else {
                        cdns3_controller_request_remove_all(drv_res, (UINT8)UdcEpInfo->PhysicalEpIndex, 1);
                    }
                } else {
                    USB_UtilityCacheInvdUInt8(data_ptr, transfer_size);
                }

            } else {
                if (uret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                    uret = TX_NO_INSTANCE;
                } else {
                    uret = UX_TRANSFER_ERROR;
                }
                // If the transfer failed, the request might be still queued in controller.
                // We need to drop this request from controller:
                cdns3_controller_request_remove_all(drv_res, (UINT8)UdcEpInfo->PhysicalEpIndex, 1);
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_rx_process_bulk(): RX [0x%X] setup failed, code 0x%X", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, uret, 0, 0, 0);
        }
    } else {
        uret = UX_SEMAPHORE_ERROR;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_rx_process_bulk(): Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
    }

    return uret;
}



static UINT32 cdns3_rx_process(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 uret;
    UINT32 ux_ep_mask;

    // We have a request for a IN transaction from the host.
    // If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
    // thread to suspend.

    // Get the pointer to the logical endpoint from the transfer request.
    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

    if (usbx_endpoint == NULL) {
        uret = USB_ERR_NO_INSTANCE;
    } else {

        /* Build the endpoint mask from the endpoint descriptor.  */
        ux_ep_mask = ux_endpoint_type_get(usbx_endpoint);

        if (ux_ep_mask != (UINT32)UX_CONTROL_ENDPOINT) {
            /* BULK/Interrupt/Isochronous endpoint */
            uret = cdns3_rx_process_bulk(UdcEpInfo, TransferRequest, ux_ep_mask);
        } else {
            //uret = rx_control_process(UdcEpInfo, TransferRequest);
            USB_UtilityPrint(USB_PRINT_FLAG_INFO, "cdns3_rx_process(): control endpoint rx is already done in driver.");
            uret = 0;
        }
    }

    return uret;
}

static void cdns3_ctrl_null_send(XHC_CDN_DRV_RES_s *DrvRes)
{
    // TODO: need to wait complete?
    UINT32 uret = USBDCDNS3_ControlTransfer(DrvRes, NULL, 0, 0);
    if (uret != 0U) {
        // action TBD
    }
}

static UINT32 cdns3_tx_process_control(UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 uret;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 max_pkt_size;
    UINT32 max_transfer_size;
    UINT32 transfer_size;
    UINT32 remain_size;
    USBD_UDC_s *udc;
    XHC_CDN_DRV_RES_s *drv_res = NULL;
    UINT8 *data_ptr;

    if (USBDCDNS3_UdcInstanceSafeGet(&udc) == 0U) {
        drv_res = &udc->CDNDrvRes;
    }

    usbx_endpoint     = TransferRequest->ux_slave_transfer_request_endpoint;
    max_pkt_size      = usbx_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
    max_transfer_size = (UINT32)TransferRequest->max_buffer_size;

    if (TransferRequest->ux_slave_transfer_request_requested_length == 0U) {
        cdns3_ctrl_null_send(drv_res);
        uret = UX_SUCCESS;
    } else {

        remain_size = (UINT32)TransferRequest->ux_slave_transfer_request_requested_length;
        data_ptr = TransferRequest->ux_slave_transfer_request_data_pointer;

        do {
            UINT32 flag_leave = 0;

            // disable the EP interrupt bit to make sure the EP descriptor is not polluted during setting new transacation .
            // TODO: disable endpoint interrupt?

            // Reset semaphore.
            if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {

                // enable the interrupt of this endpoint to get IN token and TX DMA done
                // TODO: enabled endpoint interrupt?

                if (remain_size > max_transfer_size) {
                    transfer_size = max_transfer_size;
                } else {
                    transfer_size = remain_size;
                }

                USB_UtilityCacheFlushUInt8(data_ptr, transfer_size);

                // issue a request to device controller
                uret = USBDCDNS3_ControlTransfer(drv_res,
                                                 data_ptr,
                                                 transfer_size,
                                                 1);

                // We should wait for the semaphore to wake us up.
                //uret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore, 500);
                TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;
                TransferRequest->ux_slave_transfer_request_status = UX_SUCCESS;
                if (uret == USB_ERR_SUCCESS) {
                    // Disable In endpoint till next routine.
                    // TODO: disable endpoint interrupt?

                    // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
                    if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                                              "cdns3_tx_process_control(): [0x%X]completion_code = 0x%x",
                                              usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                              (UINT32)TransferRequest->ux_slave_transfer_request_completion_code, 0, 0, 0);
                        uret = (UINT32)TransferRequest->ux_slave_transfer_request_completion_code;
                        flag_leave  = 1;
                    } else {
                        // success
                        data_ptr = &data_ptr[transfer_size];
                        remain_size = remain_size - transfer_size;
                        uret = UX_SUCCESS;
                        // check if a ZLP is necessary
                        if (((TransferRequest->ux_slave_transfer_request_requested_length % max_pkt_size) == 0U) &&
                            (remain_size == 0U)) {
                            if (TransferRequest->ux_slave_transfer_request_force_zlp == 1U) {
                                cdns3_ctrl_null_send(drv_res);
                            }
                        }
                    }
                } else {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process_control(): sema In error 0x%2x\n", uret, 0, 0, 0, 0);
                    flag_leave = 1;
                    if (uret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                        uret = TX_NO_INSTANCE;
                    } else {
                        uret = UX_TRANSFER_ERROR;
                    }
                }

            } else {
                uret = UX_SEMAPHORE_ERROR;
                flag_leave = 1;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process_control(): Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
            }

            if (flag_leave == 1U) {
                break;
            }
        } while (remain_size > 0U);
    }

    return uret;
}

static void cdns3_tx_process_bulk_zlp(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PhysicalEpIndex)
{
    AmbaMisra_TouchUnused(DrvRes);
    (void)PhysicalEpIndex;

    USB_UtilityPrintUInt1(USB_PRINT_FLAG_ERROR,
                          "cdns3_tx_process_bulk_zlp(): ZLP is not supported for EP 0x%X",
                          PhysicalEpIndex);
}

static UINT32 cdns3_tx_process_bulk(const UDC_ENDPOINT_INFO_s *UdcEndpointInfo, UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 uret;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 max_pkt_size;
    UINT32 max_transfer_size;
    UINT32 transfer_size;
    UINT32 remain_size;
    USBD_UDC_s *udc;
    XHC_CDN_DRV_RES_s *drv_res = NULL;
    UINT8 *data_ptr;

    if (USBDCDNS3_UdcInstanceSafeGet(&udc) == 0U) {
        drv_res = &udc->CDNDrvRes;
    }

    usbx_endpoint     = TransferRequest->ux_slave_transfer_request_endpoint;
    max_pkt_size      = usbx_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
    max_transfer_size = (UINT32)TransferRequest->max_buffer_size;

    if (TransferRequest->ux_slave_transfer_request_requested_length == 0U) {
        cdns3_tx_process_bulk_zlp(drv_res, UdcEndpointInfo->PhysicalEpIndex);
        uret = UX_SUCCESS;
    } else {

        remain_size = (UINT32)TransferRequest->ux_slave_transfer_request_requested_length;
        data_ptr    = TransferRequest->ux_slave_transfer_request_data_pointer;

        do {
            UINT32 flag_leave = 0;

            // Reset semaphore.
            if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {

                if (remain_size > max_transfer_size) {
                    transfer_size = max_transfer_size;
                } else {
                    transfer_size = remain_size;
                }

                USB_UtilityCacheFlushUInt8(data_ptr, transfer_size);

                // issue a request to device controller
                uret = USBCDNS3_DataTransfer(drv_res,
                                           (UINT8)UdcEndpointInfo->PhysicalEpIndex,
                                           IO_UtilityPtrToU64Addr(data_ptr),
                                           transfer_size,
                                           tx_complete_func);

                if (uret == 0U) {
                    // We should wait for the semaphore to wake us up.
                    uret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                                     (UINT32)TransferRequest->ux_slave_transfer_request_timeout);
                    if (uret == USB_ERR_SUCCESS) {

                        // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
                        if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process_bulk(): [0x%X] completion_code = 0x%x",
                                                  usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                                  (UINT32)TransferRequest->ux_slave_transfer_request_completion_code, 0, 0, 0);
                            uret = (UINT32)TransferRequest->ux_slave_transfer_request_completion_code;
                            flag_leave  = 1;

                            // If the transfer failed, the request might be still queued in controller.
                            // We need to drop this request from controller:
                            if (TransferRequest->ux_slave_transfer_request_completion_code == (ULONG)UX_TRANSFER_BUS_RESET) {
                                // for BUS Reset, no need to do this (controller always reports failed)
                                //cdns3_controller_request_remove_all(drv_res, (UINT8)UdcEndpointInfo->PhysicalEpIndex, 0);
                            } else {
                                cdns3_controller_request_remove_all(drv_res, (UINT8)UdcEndpointInfo->PhysicalEpIndex, 1);
                            }

                        } else {
                            // success
                            data_ptr = &data_ptr[transfer_size];
                            remain_size = remain_size - transfer_size;
                            uret = UX_SUCCESS;
                            // check if a ZLP is necessary
                            if (((TransferRequest->ux_slave_transfer_request_requested_length % max_pkt_size) == 0U) &&
                                (remain_size == 0U)) {
                                if (TransferRequest->ux_slave_transfer_request_force_zlp == 1U) {
                                    cdns3_tx_process_bulk_zlp(drv_res, UdcEndpointInfo->PhysicalEpIndex);
                                }
                            }
                        }
                    } else {
                        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process_bulk(): [0x%X] sema In error 0x%2x\n", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, uret, 0, 0, 0);
                        flag_leave = 1;
                        if (uret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                            uret = TX_NO_INSTANCE;
                        } else {
                            uret = UX_TRANSFER_ERROR;
                        }
                        // If the transfer failed, the request might be still queued in controller.
                        // We need to drop this request from controller:
                        cdns3_controller_request_remove_all(drv_res, (UINT8)UdcEndpointInfo->PhysicalEpIndex, 1);
                    }
                } else {
                    uret = UX_TRANSFER_NOT_READY;
                    flag_leave = 1;
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process_bulk(): [0x%x] setup transfer fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
                }
            } else {
                uret = UX_SEMAPHORE_ERROR;
                flag_leave = 1;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process_bulk(): Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
            }

            if (flag_leave == 1U) {
                break;
            }
        } while (remain_size > 0U);
    }

    return uret;
}

static UINT32 cdns3_tx_process(const UDC_ENDPOINT_INFO_s *Ed, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 uret;
    UINT32 ux_ep_type;

    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
    if (usbx_endpoint == NULL) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdns3_tx_process(): UX endpoint is NULL.",  0, 0, 0, 0, 0);
        uret = USB_ERR_NO_INSTANCE;
    } else {
        ux_ep_type = ux_endpoint_type_get(usbx_endpoint);

        switch (ux_ep_type) {
        case UX_CONTROL_ENDPOINT:
            uret = cdns3_tx_process_control(TransferRequest);
            break;

        case UX_BULK_ENDPOINT_IN:
        case UX_INTERRUPT_ENDPOINT_IN:
            uret = cdns3_tx_process_bulk(Ed, TransferRequest);
            break;
        case UX_ISOCHRONOUS_ENDPOINT_IN:
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "cdns3_tx_process(): iso endpoint is supported.");
            uret = UX_ERROR;
            break;

        default:
            // Not Support Yet
            uret = UX_ERROR;
            break;
        }
    }
    return uret;
}


/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by USBX to start USB data transfer.
 * */
UINT32 USBDCDNS3_TransferRequest(struct USBD_UDC_ts *Udc, UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 uret;
    const UDC_ENDPOINT_INFO_s *udc_ep_info = USB_UtilityVoidP2UdcEd(TransferRequest->ux_slave_transfer_request_endpoint->ux_slave_endpoint_ed);

    AmbaMisra_TouchUnused(Udc);

    if (udc_ep_info != NULL) {
        // PHASE_DATA_OUT here is the "IN" direction of USB specification
        if (TransferRequest->ux_slave_transfer_request_phase == (UINT32)UX_TRANSFER_PHASE_DATA_OUT) {
            uret = cdns3_tx_process(udc_ep_info, TransferRequest);
        } else {       // NOT a Data Out phase
            uret = cdns3_rx_process(udc_ep_info, TransferRequest);
        }
    } else {
        uret = USB_ERR_NO_INSTANCE;
    }

    return uret;
}
/** @} */
