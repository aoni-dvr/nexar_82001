/**
 *  @file AmbaUSBD_Drv_TransferRequest.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details USB kernel driver to do transfer request.
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
    return (Endpoint->ux_slave_endpoint_descriptor.bmAttributes & (ULONG)UX_MASK_ENDPOINT_TYPE) |
           (Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & (ULONG)UX_ENDPOINT_DIRECTION);
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

static UINT32 semaphore_check_and_reset(UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 semaphore_count = 0;

    if (USB_UtilitySemaphoreQuery(&TransferRequest->ux_slave_transfer_request_semaphore, &semaphore_count) == USB_ERR_SUCCESS) {
        if (semaphore_count != 0U) {
            UINT32 i;
            for (i = 0; i < semaphore_count; i++) {
                if (USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore, USB_NO_WAIT) != USB_ERR_SUCCESS) {
                    // shall not be here.
                }
            }
        }
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "semaphore_check_and_reset: can't get semaphore info");
        semaphore_count = 0;
    }

    return semaphore_count;
}

static void ongoing_txdma_stop(UINT32 PhysicalIdx, UINT32 SafeCnt)
{
    UINT32 count = SafeCnt;

    // Try to clean the data transfer by TDMA. At least a flush if SaftCnt is zero.
    while (AmbaRTSL_UsbGetEpInPollDemand(PhysicalIdx) != 0U) {
        AmbaRTSL_UsbSetEpInCtrlF(PhysicalIdx, 1);
        // Add delay here to make sure flush command takes effect
        USB_UtilityTaskSleep(1);
        if (count == 0U) {
            if (AmbaRTSL_UsbGetEpInPollDemand(PhysicalIdx) == 1U) {
                // TXFIFO Flush still under going
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Failed to flush [%d] TxFIFO", PhysicalIdx, 0, 0, 0, 0);
            }
            break;
        }
        count--;
        // Poll demand won't set to 0 until all desc are done.
    }
    // Do one more flush to clear the TXFIFO.
    AmbaRTSL_UsbSetEpInCtrlF(PhysicalIdx, 1);
    if (AmbaRTSL_UsbGetEpInStatusTdc(PhysicalIdx) != 0U) {
        AmbaRTSL_UsbClrEpInStatusTdc(PhysicalIdx, 1);
    }
}

static UINT32 tx_control_request_process(UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 uret;
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 max_pkt_size;

    usbx_endpoint  = TransferRequest->ux_slave_transfer_request_endpoint;
    max_pkt_size   = usbx_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
    if (TransferRequest->ux_slave_transfer_request_requested_length == 0U) {
        USBD_UdcCtrlNullPacketSend();
        uret = UX_SUCCESS;
    } else {
        do {
            UINT32 flag_leave = 0;

            // disable the EP interrupt bit to make sure the EP descriptor is not polluted during setting new transacation .
            AmbaRTSL_UsbDisInEpInt(0);

            USBD_UdcTxCtrlDescSetup(TransferRequest);

            // Reset semaphore.
            if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {
                if (AmbaRTSL_UsbGetEpInNAK(0) != 0U) {
                    AmbaRTSL_UsbClearEpInNAK(0, 1);
                }
                // enable the interrupt of this endpoint to get IN token and TX DMA done
                AmbaRTSL_UsbEnInEpInt(0);

                // We should wait for the semaphore to wake us up.
                uret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore, 500);
                if (uret == USB_ERR_SUCCESS) {
                    // Disable In endpoint till next routine.
                    AmbaRTSL_UsbSetEpInNAK(0, 1);
                    AmbaRTSL_UsbDisInEpInt(0);

                    // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
                    if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                        AmbaRTSL_UsbEnInEpInt(0);
                        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "completion_code = 0x%x", TransferRequest->ux_slave_transfer_request_completion_code, 0, 0, 0, 0);
                        uret = TransferRequest->ux_slave_transfer_request_completion_code;
                        flag_leave  = 1;
                    } else {
                        if (((TransferRequest->ux_slave_transfer_request_requested_length % max_pkt_size) == 0U) &&
                            (TransferRequest->ux_slave_transfer_request_in_transfer_length == 0U)) {
                            if (TransferRequest->ux_slave_transfer_request_force_zlp == 1U) {
                                USBD_UdcCtrlNullPacketSendEx();
                            }
                            uret = UX_SUCCESS;
                            flag_leave  = 1;
                        } else {
                            uret = UX_SUCCESS;
                        }
                    }
                } else {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "tx_control_request_process: sema In error 0x%2x\n", uret, 0, 0, 0, 0);
                    flag_leave = 1;
                    if (uret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                        uret = TX_NO_INSTANCE;
                    } else {
                        uret = UX_TRANSFER_ERROR;
                    }
                }

                if (TransferRequest->ux_slave_transfer_request_in_transfer_length == 0U) {
                    flag_leave = 1;
                }
            } else {
                uret = UX_SEMAPHORE_ERROR;
                flag_leave = 1;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);

            }

            if (flag_leave == 1U) {
                break;
            }
        } while (TransferRequest->ux_slave_transfer_request_actual_length < TransferRequest->ux_slave_transfer_request_requested_length);
    }

    return uret;
}

static UINT32 tx_interrupt_request_zlp(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;
    UINT32 uret;
    UINT32 func_ret;

    // setup DMA descriptor
    USBD_UdcTxDescSetupZeroLength(UdcEpInfo);

    // reset semaphore
    if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {

        // clear NAK to enable IN token interrupt.
        AmbaRTSL_UsbClearEpInNAK(physical_endpoint_index, 1);

        // Enable Endpoint Interrupt
        AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);

        // wait until TX done
        func_ret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                            TransferRequest->ux_slave_transfer_request_timeout);
        if (func_ret != USB_ERR_SUCCESS) {
            usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] EP 0x%X TX error - can't wait Semaphore, code 0x%02X, timeout %d",
                                  usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                  func_ret,
                                  TransferRequest->ux_slave_transfer_request_timeout,
                                  0,
                                  0);
            if (func_ret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                uret = TX_NO_INSTANCE;
            } else {
                uret = UX_TRANSFER_ERROR;
            }
        } else {
            // Disable In Endpoint till next routine.
            AmbaRTSL_UsbSetEpInNAK(physical_endpoint_index, 1);
            AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);
            uret = UX_SUCCESS;
        }
    } else {
        uret = UX_SEMAPHORE_ERROR;
        {
            usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "tx_interrupt_request_zlp(): Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
        }
    }
    return uret;
}

static UINT32 tx_interrupt_request_process(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *Endpoint;
    UINT32 uret;
    UINT32 func_ret;
    UINT32 physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;

    // Get the pointer to the logical endpoint from the transfer request.
    Endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

    // disable the EP interrupt bit to make sure the EP descriptor is not polluted  during setting new transacation
    AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);

    USBD_UdcTxDescSetup(UdcEpInfo, TransferRequest, 0);

    if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {
        // clear NAK to enable IN token interrupt.
        AmbaRTSL_UsbClearEpInNAK(physical_endpoint_index, 1);

        // Enable the interrupt on this endpoint.
        AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);

        // We should wait for the semaphore to wake us up.
        func_ret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                            TransferRequest->ux_slave_transfer_request_timeout);

        if (func_ret != USB_ERR_SUCCESS) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] EP 0x%X TX error - can't wait Semaphore, code 0x%02X, timeout %d",
                                  Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                  func_ret,
                                  TransferRequest->ux_slave_transfer_request_timeout,
                                  0,
                                  0);

            if (func_ret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                uret = TX_NO_INSTANCE;
            } else {
                uret = UX_TRANSFER_ERROR;
            }
        } else {
            // Disable In endpoint till next routine.
            AmbaRTSL_UsbSetEpInNAK(physical_endpoint_index, 1);
            AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);

            // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
            if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);
                uret = TransferRequest->ux_slave_transfer_request_completion_code;
            } else {
                // Should we send a zero packet?
                if (TransferRequest->ux_slave_transfer_request_force_zlp == 1U) {
                    TransferRequest->ux_slave_transfer_request_force_zlp = 0;
                    uret = tx_interrupt_request_zlp(UdcEpInfo, TransferRequest);
                } else {
                    uret = UX_SUCCESS;
                }

                // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
                if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                    AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);
                    uret = TransferRequest->ux_slave_transfer_request_completion_code;
                }
            }
        }
    } else {
        uret = UX_SEMAPHORE_ERROR;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Reset [0x%x] sema fail", Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
    }

    return uret;
}

static void tx_bulk_request_zlp(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 *FlagLeave, UINT32 *Status)
{
    UINT32 physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;

    USBD_UdcTxDescSetupZeroLength(UdcEpInfo);
    if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {
        UINT32 func_ret;
        AmbaRTSL_UsbClearEpInNAK(physical_endpoint_index, 1);
        AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);
        func_ret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                            TransferRequest->ux_slave_transfer_request_timeout);

        // Disable In endpoint till next routine.
        AmbaRTSL_UsbSetEpInNAK(physical_endpoint_index, 1);
        AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);
        if (func_ret != USB_ERR_SUCCESS) {
            const UX_SLAVE_ENDPOINT *usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] EP 0x%X TX error - can't wait Semaphore, code 0x%02X, timeout %d",
                                  usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                  func_ret,
                                  TransferRequest->ux_slave_transfer_request_timeout,
                                  0, 0);

            if (func_ret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                *Status = TX_NO_INSTANCE;
            } else {
                *Status = UX_TRANSFER_ERROR;
            }
            *FlagLeave = 1;
        }
    } else {
        *Status = UX_SEMAPHORE_ERROR;
        *FlagLeave  = 1;
        {
            const UX_SLAVE_ENDPOINT *usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
        }
    }
}

static UINT32 tx_bulk_request_process(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *Endpoint;
    UINT32 uret;
    UINT32 physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;

    // Handle null packet
    if (TransferRequest->ux_slave_transfer_request_requested_length == 0U) {
        // Same as interrupt tx
        uret = tx_interrupt_request_process(UdcEpInfo, TransferRequest);
    } else {
        UINT32 flag_leave = 0;
        uret = UX_SUCCESS;

        // Get the pointer to the logical endpoint from the transfer request.
        Endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

        // When the previous transfer is timeout due to bus suspend, even if the ongoing dma is stop by flush the txfifo,
        // there's still an txfifo empty flag is set when bus is resume, which would cause the following ISR blocked.
        // Clear it here.
        if (AmbaRTSL_UsbGetEpInStsTxEmpty(physical_endpoint_index) != 0U) {
            if (AmbaRTSL_UsbGetEpInNAK(physical_endpoint_index) != 0U) {
                AmbaRTSL_UsbClrEpInStsTxEmpty(physical_endpoint_index, 1);
            }
        }

        // disable the EP interrupt bit to make sure the EP descriptor is not polluted during setting new transacation
        AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);

        if (USBD_IntIsEnabled() == 0U) {
            USBD_IntEnable();
        }

        while (TransferRequest->ux_slave_transfer_request_in_transfer_length != 0U) {
            // mark completion code to SUCCESS
            TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

            USBD_UdcTxDescSetup(UdcEpInfo, TransferRequest, 0);

            if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {
                UINT32 func_ret;
                // clear NAK to enable IN token interrupt
                AmbaRTSL_UsbClearEpInNAK(physical_endpoint_index, 1);
                // Enable the interrupt on this endpoint.
                AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);

                // Update the status register for the endpoint by raising the TXPKTRDY flag
                // so the controller knows the buffer is ready to be sent when the next IN
                // token is received.
                // This will arm the first packet, it is possible that there are multiple packets for this
                // transfer in which case the callback function will proceed with the rest until all is transferred.
                // We should wait for the semaphore to wake us up.
                func_ret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                                    TransferRequest->ux_slave_transfer_request_timeout);

                // Disable In endpoint till next routine.
                AmbaRTSL_UsbSetEpInNAK(physical_endpoint_index, 1);
                AmbaRTSL_UsbDisInEpInt(physical_endpoint_index);

                // Check the completion code. */
                if (func_ret == USB_ERR_SUCCESS) {
                    if ((TransferRequest->ux_slave_transfer_request_status == (UINT32)UX_TRANSFER_STATUS_ABORT) ||
                        (TransferRequest->ux_slave_transfer_request_completion_code == (UINT32)UX_TRANSFER_BUS_RESET)) {
                        ongoing_txdma_stop(physical_endpoint_index, UdcEpInfo->DmaDescNum);
                        uret = UX_TRANSFER_BUS_RESET;
                        flag_leave  = 1;
                    } else {
                        uret = UX_SUCCESS;
                    }
                } else {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] EP 0x%X TX error - can't wait Semaphore, code 0x%02X, timeout %d",
                                          Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                          func_ret,
                                          TransferRequest->ux_slave_transfer_request_timeout,
                                          0, 0);
                    ongoing_txdma_stop(physical_endpoint_index, UdcEpInfo->DmaDescNum);
                    if (func_ret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                        uret = TX_NO_INSTANCE;
                    } else {
                        uret = UX_TRANSFER_ERROR;
                    }
                    flag_leave = 1;
                }
            } else {
                uret = UX_SEMAPHORE_ERROR;
                flag_leave  = 1;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Reset [0x%x] sema fail", Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
            }

            if (flag_leave == 1U) {
                break;
            }
        }

        if (flag_leave == 0U) {
            // Should we send a zero packet?
            if (TransferRequest->ux_slave_transfer_request_force_zlp == 1U) {
                TransferRequest->ux_slave_transfer_request_force_zlp = 0;
                tx_bulk_request_zlp(UdcEpInfo, TransferRequest, &flag_leave, &uret);
            }

            if (flag_leave == 0U) {
                // Check the transfer request completion code. We may have had a BUS reset or a device disconnection.
                if (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS) {
                    AmbaRTSL_UsbEnInEpInt(physical_endpoint_index);
                    uret = TransferRequest->ux_slave_transfer_request_completion_code;
                }
            }
        }
    }

    return uret;
}

#if defined(USBD_SUPPORT_ISO)

static UINT32 ux_ep_max_pkt_size_get(const UX_SLAVE_ENDPOINT *UsbxEndpoint)
{
    return UsbxEndpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
}

static UINT32 tx_iso_request_process(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint = UdcEpInfo->UxEndpoint;
    UINT32 max_pkt_size                    = ux_ep_max_pkt_size_get(usbx_endpoint);
    UINT32 uret;

    // Transfer request length should not exceed max descriptor chain (2<<13).
    // Besides, descriptor number is determined by endpoint max packet size.
    if (TransferRequest->ux_slave_transfer_request_in_transfer_length > (UdcEpInfo->DmaDescNum * max_pkt_size)) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "tx_iso_request_process: transfer request length is too long");
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "request length %d > maximum transfer length %d", TransferRequest->ux_slave_transfer_request_in_transfer_length,
                              UdcEpInfo->DmaDescNum * max_pkt_size, 0, 0, 0);
        uret = UX_ERROR;
    } else {
        TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

        USBD_UdcTxDescSetup(UdcEpInfo, TransferRequest, 1);

        if (TransferRequest->ux_slave_transfer_request_open_iso_terminal != 0U) {
            AmbaRTSL_UsbEnInEpInt(UdcEpInfo->PhysicalEpIndex);
            AmbaRTSL_UsbClearEpInNAK(UdcEpInfo->PhysicalEpIndex, 1);
        } else {
            AmbaRTSL_UsbDisInEpInt(UdcEpInfo->PhysicalEpIndex);
            AmbaRTSL_UsbSetEpInNAK(UdcEpInfo->PhysicalEpIndex, 1);
        }
        uret = UX_SUCCESS;
    }

    return uret;
}

#else

static UINT32 tx_iso_request_process(const UDC_ENDPOINT_INFO_s *Ed, UX_SLAVE_TRANSFER *TransferRequest)
{
    (void)Ed;
    (void)TransferRequest;
    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "tx_iso_request_process: do not support isochronous transfer");
    return UX_ERROR;
}

#endif

static UINT32 tx_request_process(UDC_ENDPOINT_INFO_s *Ed, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 uret;
    UINT32 ux_ep_type;

    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
    if (usbx_endpoint == NULL) {
        uret = USB_ERR_NO_INSTANCE;
    } else {
        ux_ep_type = ux_endpoint_type_get(usbx_endpoint);

        switch (ux_ep_type) {
        case UX_CONTROL_ENDPOINT:
            uret = tx_control_request_process(TransferRequest);
            break;

        case UX_BULK_ENDPOINT_IN:
            uret = tx_bulk_request_process(Ed, TransferRequest);
            break;

        case UX_INTERRUPT_ENDPOINT_IN:
            uret = tx_interrupt_request_process(Ed, TransferRequest);
            break;

        case UX_ISOCHRONOUS_ENDPOINT_IN:
            uret = tx_iso_request_process(Ed, TransferRequest);
            break;

        default:
            // Not Support Yet
            uret = UX_ERROR;
            break;
        }
    }
    return uret;
}

static void rx_workaround_mtp(void)
{
    static UINT32 flag_first_rx = 1;

    // Workaround : When using MTP class, there's chance the 1st bulk out data can not
    //              write to appointed memory address but descriptor shows dma done.
    //              Try to add 1ms delay after we get set configuration and pima thread
    //              is resumed then issue disappears.

    if (flag_first_rx == 1U) {
        USB_UtilityTaskSleep(1);
        flag_first_rx = 0;
    }
}

static UINT32 rx_workaround_miss_interrupt(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpMask, UINT32 Result)
{
    UINT32 rx_length = 0;
    UINT32 check_count = 0;
    UINT32 physical_endpoint_index;
    UINT32 uret;

    physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;

    // D.3. Invalidate cache memory to make sure we get the true content of descirptor.
    if ((UdcEpInfo->DmaDesc == NULL) && (UdcEpInfo->DmaDescSize != 0U)) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "rx_workaround_miss_interrupt: Tried to invalidate illegal range");
        uret = UX_CACHE_RANGE_ERROR;
    } else {
        const UDC_DATA_DESC_s *dma_desc;

        USB_UtilityCacheInvdDataDesc(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescSize);

        // D.4. It's a protection for ISR happen right after semaphore time out. In this case kernel would think
        //      recevie fail then system would lost this data forever.

        if (EpMask == (UINT32)UX_BULK_ENDPOINT) {
            dma_desc = USBD_UdcLastDescGet(UdcEpInfo->DmaDesc, USBD_BULK_OUT_CHAIN_DESC_NUM);
        } else {
            dma_desc = USBD_UdcLastDescGet(UdcEpInfo->DmaDesc, USBD_INTERRUPT_CHAIN_DESC_NUM);
        }

        while ((dma_desc != NULL) && ((dma_desc->Status & USBD_DMA_STATUS_MASK) != USBD_DMA_DONE)) {
            USB_UtilityTaskSleep(1);
            check_count++;
            if (check_count == 5U) {
                break;
            }
        }

        if ((dma_desc != NULL) && ((dma_desc->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_DONE)) {
            // D.5. It's another protection for:
            //      1. If ISR do not happen since ep interrupt is disabled but DMA shows job done.
            //      2. If ISR happens but DMA not shows job done, and then DMA shows job done at timeout occurs.

            // Clear Endpoint status
            if (AmbaRTSL_UsbGetEpOutStatusOut(physical_endpoint_index) == EP_OUT_RCV_DATA) {
                AmbaRTSL_UsbClrEpOutStatusOut(physical_endpoint_index, EP_OUT_RCV_DATA);
            }

            // Adjust the buffer address.
            rx_length                                                       = dma_desc->Status & 0xFFFFU;
            TransferRequest->ux_slave_transfer_request_current_data_pointer = &TransferRequest->ux_slave_transfer_request_current_data_pointer[rx_length];
            // Update the length of the data received.
            TransferRequest->ux_slave_transfer_request_actual_length += rx_length;
            uret                                                      = UX_SUCCESS;
        } else {
            // clean DMA descriptor to prevent USB DMA engine to receive data again.
            USBD_UdcInitDmaDesc(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescSize / sizeof(UDC_DATA_DESC_s));
            USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
            UdcEpInfo->RxDmaState &= ~UDC_RX_DMA_STAT_REQ_PENDING;
            USBD_UdcRxStateMutexPut();
            uret = Result;
        }
    }
    return uret;
}

static void rx_non_control_ep_status_check(UINT32 RxResult, UINT32 PhysicalEndpointIndex)
{
    static UINT32 loop_count = 0;

    if ((RxResult != (UINT32)UX_SUCCESS)) {
        while (AmbaRTSL_UsbGetEpOutStatusOut(PhysicalEndpointIndex) != 0U) {
            USB_UtilityTaskSleep(1);
            loop_count++;
            if (loop_count == 2U) {
                loop_count = 0;
                break;
            }
        }
    }
}

static UINT32 rx_non_control_result_check(UINT32 RxResult, const UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 timeout;
    static UINT32 enable_timeout_message = 0;
    UINT32 uret;

    if (RxResult != USB_ERR_SUCCESS) {
        timeout = TransferRequest->ux_slave_transfer_request_timeout;
        if ((timeout != USB_WAIT_FOREVER) && (enable_timeout_message != 0U)) {
            const UX_SLAVE_ENDPOINT *usbx_endpoint;
            // Get the pointer to the logical endpoint from the transfer request.
            usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] EP %d RX error - can't wait Semaphore, code 0x%2X, timeout %d",
                                  usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                  RxResult,
                                  timeout, 0, 0);
        }
        if (RxResult == USB_ERR_SEMA_TAKE_TIMEOUT) {
            uret = TX_NO_INSTANCE;
        } else {
            uret = UX_TRANSFER_ERROR;
        }
    } else {
        uret = UX_SUCCESS;
    }
    return uret;
}

static UINT32 rx_non_control_process(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpMask)
{

    UINT32 uret         = UX_SUCCESS;
    UINT32 physical_endpoint_index;
    UINT32 residue_length = 0;
    UINT8 *residue_ptr = NULL;
    UINT32 flag_done       = 0;

    physical_endpoint_index = UdcEpInfo->PhysicalEpIndex;

    TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

    // A. Turn off RDMA, disable ep ISR and set nak before chaining dma descriptor.
    AmbaRTSL_UsbSetEpOutNAK(physical_endpoint_index, 1);

    rx_workaround_mtp();

    // B. Chain RX descriptor. Reset semaphore. Enable Rdma, Ep ISR and clear nak.
    // check whether there are data in the swfifo before we start setup the dma descriptors.
    if (semaphore_check_and_reset(TransferRequest) != 0U) {
        USBD_UdcRxSwfifoPop(UdcEpInfo, TransferRequest, &residue_ptr, &residue_length);
        if (residue_length == 0U) {
            uret = UX_SUCCESS;
            flag_done   = 1;
        }
    } else {
        residue_ptr = TransferRequest->ux_slave_transfer_request_data_pointer;
        residue_length = TransferRequest->ux_slave_transfer_request_requested_length;
    }

    if (flag_done == 0U) {
        UINT32 safety_count;
        // Setup the dma descriptors. Then check whether there are data push to swfifo during this period.
        // If yes, pop the data from swfifo to data buffer and setup the descriptors again.
        // Repeat above steps till no data is left in the swfifo.
        // 1. Why do we check the swfifo so many times?
        // A: It's because in some platforms like H22 at full speed, the swfifo data push by using the alternate descriptor
        //    defers, whcih could make the concurrent isr-bottomhalf task to put the semaphore after we reset it.
        // 2. Is it possible that the task issuing the transfer request starves due to this recheck?
        // A: It should not happen since we Nak the endpoint. The data store in the RXFIFO is limited.
        //    Actually in our test (H22 MSC at FS), there's at most one recheck during one transfer request.
        //    However we still use a safety counter to avoid the infinite loop.
        // 3. What if the deferred swfifo data push happens after we start waiting the semaphore? In that case the UDC_RX_DMA_STAT_ALT_DESC
        //    flag of RxDmaState has been cleraed and the ISR bottomhalf task cannot aware the deferred event and run the wrong flow.
        // A: When this happens, the dma status check in udc_get_receive_data_length() cannot find the valid DMA_DONE descriptor in the primary
        //    descriptor. Alternately it should check the substitue descriptor then set the UDC_RX_DMA_STAT_ALT_DESC again to run the correct flow.
        //    See udc_get_receive_data_length() for details.
        for (safety_count = 0; safety_count < 5U; safety_count++) {
            UINT32 flag_leave = 0;

            USBD_UdcRxDescSetup(UdcEpInfo, TransferRequest, residue_ptr, residue_length, 1);
            if (semaphore_check_and_reset(TransferRequest) != 0U) {
                USBD_UdcRxSwfifoPop(UdcEpInfo, TransferRequest, &residue_ptr, &residue_length);
                if (residue_length == 0U) {
                    uret       = UX_SUCCESS;
                    flag_leave = 1;
                    flag_done  = 1;
                }
            } else {
                flag_leave = 1;
            }

            if (flag_leave == 1U) {
                break;
            }
        }

        if (safety_count == 5U) {
            // timeout
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "rx_non_control_process: Reach the safety cnt limit");
        }

        if (flag_done == 0U) {
            UINT32 func_ret;

            AmbaRTSL_UsbEnOutEpInt(physical_endpoint_index);
            AmbaRTSL_UsbClearEpOutNAK(physical_endpoint_index, 1);

            // C. Waiting for ISR give semaphore or semaphore time out.
            func_ret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore,
                                                TransferRequest->ux_slave_transfer_request_timeout);

            if ((TransferRequest->ux_slave_transfer_request_data_pointer == NULL) &&
                (TransferRequest->ux_slave_transfer_request_requested_length != 0U)) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "rx_non_control_process: Tried to invalidate illegal range");
                uret = UX_CACHE_RANGE_ERROR;
            } else {
                USB_UtilityCacheInvdUInt8(TransferRequest->ux_slave_transfer_request_data_pointer,
                                          TransferRequest->ux_slave_transfer_request_requested_length);

                uret = rx_non_control_result_check(func_ret, TransferRequest);

                // D. To decide if the transaction is successful. Make sure all out token is nak till next Rx Process
                AmbaRTSL_UsbSetEpOutNAK(physical_endpoint_index, 1);

                // D.1. After receive semaphore, ep status should be cleared. If not, we assume another ISR may happen later. Wait till ISR occur.
                rx_non_control_ep_status_check(uret, physical_endpoint_index);

                // D.2. Disable EP ISR. Do Not turn off RDMA here since later control pipe transfer could fail.
                if ((TransferRequest->ux_slave_transfer_request_completion_code == (UINT32)UX_TRANSFER_BUS_RESET) &&
                    (TransferRequest->ux_slave_transfer_request_status == (UINT32)UX_TRANSFER_STATUS_ABORT)) {
                    // If the request is aborted, then it might be caused by Reset or Cable Remove.
                    uret = UX_TRANSFER_BUS_RESET;
                } else {
                    if (uret != (UINT32)UX_SUCCESS) {
                        uret = rx_workaround_miss_interrupt(UdcEpInfo, TransferRequest, EpMask, uret);
                    }

                    // Check the transfer request completion code. We may have had a BUS reset or
                    //   a device disconnection.
                    if ((uret == (UINT32)UX_SUCCESS) && (TransferRequest->ux_slave_transfer_request_completion_code != (UINT32)UX_SUCCESS)) {
                        uret = TransferRequest->ux_slave_transfer_request_completion_code;
                    }
                }
            }
        }
    }

    return uret;
}

static UINT32 rx_control_process(UDC_ENDPOINT_INFO_s *Ed, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 uret;
    UINT32 actual_length = 0, packet_length = 0, max_pkt_size = 0;
    UINT32 max_buf_size;

    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
    max_buf_size  = USB_BufferSizeGet(UBUF_UDC_CTRL_OUT);

    if (TransferRequest->ux_slave_transfer_request_requested_length > max_buf_size ) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "Ctrl OUT buffer is not enough, request %d but only allocate %d",
                              TransferRequest->ux_slave_transfer_request_requested_length,
                              max_buf_size,
                              0,
                              0,
                              0);
    }

    do {
        UINT32 flag_leave = 0;

        max_pkt_size   = usbx_endpoint->ux_slave_endpoint_descriptor.wMaxPacketSize;
        actual_length  = TransferRequest->ux_slave_transfer_request_actual_length;
        // Turn off RDMA, disable ep ISR and set nak before chaining dma descriptor.
        USBD_IntDisable();
        AmbaRTSL_UsbDisOutEpInt(0);

        AmbaRTSL_UsbSetEpOutNAK(0, 1);
        USBD_UdcRxDescSetup(Ed,
                            TransferRequest,
                            TransferRequest->ux_slave_transfer_request_data_pointer,
                            TransferRequest->ux_slave_transfer_request_requested_length,
                            0);

        if (semaphore_reset(TransferRequest) == USB_ERR_SUCCESS) {
            UINT32 func_ret;

            AmbaRTSL_UsbEnOutEpInt(0);
            USBD_IntEnable();

            AmbaRTSL_UsbSetEpOutRxReady(0, 1);
            if (AmbaRTSL_UsbGetDevCtlRde() == 0U) {
                AmbaRTSL_UsbSetDevCtlRde(1);
            }

            AmbaRTSL_UsbClearEpOutNAK(0, 1);

            // Waiting for ISR give semaphore or semaphore time out.

            func_ret = USB_UtilitySemaphoreTake(&TransferRequest->ux_slave_transfer_request_semaphore, 500);
            if (func_ret == USB_ERR_SUCCESS) {
                // Check if it's the last packet.
                packet_length = TransferRequest->ux_slave_transfer_request_actual_length - actual_length;
                if (packet_length < max_pkt_size) {
                    flag_leave = 1;
                }
                uret = UX_SUCCESS;
            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[USBD] EP %d RX error - can't wait Semaphore, code 0x%2X",
                                      usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                      func_ret, 0, 0, 0);
                flag_leave = 1;
                if (func_ret == USB_ERR_SEMA_TAKE_TIMEOUT) {
                    uret = TX_NO_INSTANCE;
                } else {
                    uret = UX_TRANSFER_ERROR;
                }
            }
        } else {
            uret = UX_SEMAPHORE_ERROR;
            flag_leave  = 1;
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Reset [0x%x] sema fail", usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress, 0, 0, 0, 0);
        }

        if (flag_leave == 1U) {
            break;
        }
    } while (TransferRequest->ux_slave_transfer_request_actual_length < TransferRequest->ux_slave_transfer_request_requested_length);
    AmbaRTSL_UsbSetEpOutNAK(0, 1);
    AmbaRTSL_UsbDisOutEpInt(0);

    return uret;
}

static UINT32 rx_request_process(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest)
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

        if (ux_ep_mask != (UINT32)UX_CONTROL_ENDPOINT) { /* BULK or Interrupt endpoint */
            uret = rx_non_control_process(UdcEpInfo, TransferRequest, ux_ep_mask);
        } else {
            uret = rx_control_process(UdcEpInfo, TransferRequest);
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
UINT32 USBD_DrvTransferRequest(USBD_UDC_s *Udc, UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT32 uret;
    UDC_ENDPOINT_INFO_s *udc_ep_info = USB_UtilityVoidP2UdcEd(TransferRequest->ux_slave_transfer_request_endpoint->ux_slave_endpoint_ed);

    AmbaMisra_TouchUnused(Udc);

    if (udc_ep_info != NULL) {
        // PHASE_DATA_OUT here is the "IN" direction of USB specification
        if (TransferRequest->ux_slave_transfer_request_phase == (UINT32)UX_TRANSFER_PHASE_DATA_OUT) {
            uret = tx_request_process(udc_ep_info, TransferRequest);
        } else {       // NOT a Data Out phase
            uret = rx_request_process(udc_ep_info, TransferRequest);
        }
    } else {
        uret = USB_ERR_NO_INSTANCE;
    }

    return uret;
}
/** @} */
