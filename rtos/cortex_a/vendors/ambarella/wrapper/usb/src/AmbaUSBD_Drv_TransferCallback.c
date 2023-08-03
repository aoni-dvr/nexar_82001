/**
 *  @file AmbaUSBD_Drv_TransferCallback.c
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
 *  @details USB kernel driver to do transfer callback function.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static UINT32 flag_bypass_in_token = 0;

static void XferCb_DumpDmaDescriptor(const UDC_DATA_DESC_s *Desc, UINT32 Number)
{
    UINT i;

    for (i = 0; i < Number; i++) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "Desc[%d]: status = 0x%X", i, Desc[i].Status, 0, 0, 0);
    }
}

static UINT32 is_ctrl_ep_type(UINT32 EndpointType)
{
    UINT32 ret;

    if ((EndpointType & 0x03U) == (UINT32)UX_CONTROL_ENDPOINT) {
        ret = 1;
    } else {
        ret = 0;
    }
    return ret;
}

static UINT32 is_setup_packet(UINT32 EpDirection, UINT32 EpNum, UINT32 EpStatus)
{
    UINT32 ret = 0;

    if ((EpDirection == USBD_EP_DIR_OUT) && (EpNum == 0U) && (((EpStatus >> 4UL) & 0x03UL) == EP_OUT_RCV_SETUP)) {
        ret = 1;
    }
    return ret;
}

static UINT32 is_out_packet(UINT32 EpDirection, UINT32 EpStatus)
{
    UINT32 ret = 0;

    if ((EpDirection == (UINT32)USBD_EP_DIR_OUT) && (((EpStatus >> 4U) & 0x03U) == EP_OUT_RCV_DATA)) {
        ret = 1;
    }
    return ret;
}

static UINT32 is_in_packet(UINT32 EpDirection, UINT32 EpStatus)
{
    UINT32 ret = 0;

    if (EpDirection != (UINT32)USBD_EP_DIR_IN) {
        ret = 0;
    } else {
        if ((EpStatus & 0x40U) != 0U) {
            ret = 1;
        }
    }
    return ret;
}

static UINT32 is_txdma_done_occur(UINT32 EpDirection, UINT32 EpStatus, UINT32 Pending, UINT32 EpMask)
{
    UINT32 ret = 0;

    if (EpDirection != (UINT32)USBD_EP_DIR_IN) {
        ret = 0;
    } else {
        if (EpMask == (UINT32)UX_ISOCHRONOUS_ENDPOINT_IN) {
            if ((EpStatus & 0x400U) != 0U) {
                ret = 1;
            }
        } else {
            if (((EpStatus & 0x400U) != 0U) && (Pending != 0U)) {
                ret = 1;
            }
        }
    }
    return ret;
}

static UINT32 is_tx_empty_occur(UINT32 EpDirection, UINT32 EpStatus)
{
    UINT32 ret = 0;

    if (EpDirection != USBD_EP_DIR_IN) {
        ret = 0;
    } else {
        if ((EpStatus & 0x8000000U) != 0U) {
            ret = 1;
        }
    }
    return ret;
}

static UINT32 is_alt_desc_used(UINT32 EpType, const UDC_ENDPOINT_INFO_s *UdcEpInfo)
{
    UINT32 ret = 0;

    if (((EpType == (UINT32)UX_BULK_ENDPOINT_OUT) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_OUT)) &&
        ((UdcEpInfo->RxDmaState & UDC_RX_DMA_STAT_ALT_DESC) != 0U)) {
        ret = 1;
    }
    return ret;
}

static void notify_event_to_application(const USBD_SYS_CONFIG_s *UsbdSysConfig, UINT32 Code, UINT32 Param1, UINT32 Param2, UINT32 Param3)
{
    if ((UsbdSysConfig != NULL) &&
        (UsbdSysConfig->SystemEventCallback != NULL)) {
        if (UsbdSysConfig->SystemEventCallback(Code, Param1, Param2, Param3) != 0U) {
            // ignore error
        }
    }
}

static UINT32 process_setup_packet_pre(UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    const UDC_SETUP_DESC_s  *desc_setup;
    USBD_CLASS_CONFIG_s     *class_config;
    UINT8 *data_ptr;
    UINT32 uret;

    // The setup packet has its own buffer.
    data_ptr      = TransferRequest->ux_slave_transfer_request_setup;
    desc_setup    = USB_UtilityPhyI32ToVirSetupDesc(AmbaRTSL_UsbGetEpOutSetupbuf(0)); // remove 0xC0000000
    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

    USB_UtilityCacheInvdSetupDesc(desc_setup, sizeof(UDC_SETUP_DESC_s));

    USB_UtilityUInt32Put(data_ptr, desc_setup->Data0);
    USB_UtilityUInt32Put(&data_ptr[4], desc_setup->Data1);

#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
    _ux_utility_print("A SETUP OUT D0= 0x%x", Setup->data0);
    _ux_utility_print("A SETUP OUT D1= 0x%x", Setup->data1);
#endif
#endif

    // The MS specific descriptor (string index=0xee)
    // is not supported, so stall Ctrl IN EP
    // and re-init setup descriptor receiving
    // Host would issue a new round of enumeration
    // This issue only happens when connecting to
    // a Windows Host which is new for this device

    uret = USBD_SystemClassConfigGet(&class_config);
    if (uret == USB_ERR_SUCCESS) {
        if ((class_config->ClassID == USBD_CLASS_MSC) && (desc_setup->Data0 == 0x03ee0680U)) {
            // seems ok even not stall for XP
            // but MS suggests to stall
            USBD_DrvEndpointStall(usbx_endpoint);

            uret = UX_TRANSFER_STALLED;
        }
    }

    return uret;
}

static void process_setup_packet(UX_SLAVE_TRANSFER *TransferRequest)
{
    UINT8 is_request_in = (*TransferRequest->ux_slave_transfer_request_setup) & (UINT8)UX_REQUEST_IN;

#ifdef AMBA_USB_DEBUG
#ifdef  USB_PRINT_SUPPORT
    USB_UtilityPrint(USB_PRINT_FLAG_L1, "[SETUP PKT]");
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   bmRequestType: 0x%X", TransferRequest->ux_slave_transfer_request_setup[0], 0, 0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   bmRequest:  0x%X", TransferRequest->ux_slave_transfer_request_setup[1], 0, 0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   wValue:     0x%X %X", TransferRequest->ux_slave_transfer_request_setup[3],
                          TransferRequest->ux_slave_transfer_request_setup[2],
                          0,
                          0,
                          0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   wIndex:     0x%X %X", TransferRequest->ux_slave_transfer_request_setup[5],
                          TransferRequest->ux_slave_transfer_request_setup[4],
                          0,
                          0,
                          0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "   wLength:    0x%X %X", TransferRequest->ux_slave_transfer_request_setup[7],
                          TransferRequest->ux_slave_transfer_request_setup[6],
                          0,
                          0,
                          0);
#endif
#endif

    TransferRequest->ux_slave_transfer_request_actual_length = 8;

    // Mark the phase as SETUP.
    TransferRequest->ux_slave_transfer_request_type = UX_TRANSFER_PHASE_SETUP;

    // Mark the transfer as successful.
    TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

    // Check if the transaction is IN.
    if (is_request_in != 0U) {
        // Nak In-token until data prepare ready. It should be at transfer request.
        AmbaRTSL_UsbSetEpInNAK(0, 1);
        // Call the Control Transfer dispatcher.
        USBD_SetupCtrlRequestNotify();
    } else {// request OUT stage
        // Nak In/Out token until data prepare ready.
        AmbaRTSL_UsbSetEpOutNAK(0, 1);
        AmbaRTSL_UsbSetEpInNAK(0, 1);
        // Call the Control Transfer dispatcher.
        USBD_SetupCtrlRequestNotify();
    }
}

static void on_setup_packet(UX_SLAVE_TRANSFER *TransferRequest)
{
    // clear the setup packet bit
    AmbaRTSL_UsbClrEpOutStatusOut(0, EP_OUT_RCV_SETUP);
    // Reset ByPass Case Flag when receving Setup
    flag_bypass_in_token = 0;
    if (process_setup_packet_pre(TransferRequest) != (UINT32)UX_TRANSFER_STALLED) {
        process_setup_packet(TransferRequest);
    }
}

static UINT32 on_stall_clear(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpDirection, UINT32 EpNum)
{
    UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 logical_address = 0;
    UINT32 uret  = UX_SUCCESS;
    UINT32 rval;
    USBD_SYS_CONFIG_s *usbd_sys_config;

    rval = USBD_SystemConfigGet(&usbd_sys_config);
    if (rval == USB_ERR_SUCCESS) {
        // Get the pointer to the logical endpoint from the transfer request.
        usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

        if (EpDirection == USBD_EP_DIR_OUT) {
            if (AmbaRTSL_UsbGetEpOutStatusRcs(EpNum) == 1U) {
                AmbaRTSL_UsbSetEpOutStall(EpNum, 0);
                AmbaRTSL_UsbSetEpOutNAK(EpNum, 1);
                AmbaRTSL_UsbClrEpOutStatusRcs(EpNum, 1); // write to clear
                UdcEpInfo->EpState                    &= ~USBD_UDC_ED_STATUS_STALLED;
                usbx_endpoint->ux_slave_endpoint_state = UX_ENDPOINT_RESET;

                // USBCV Command set test. After clear stall, out packet can not received data normally
                TransferRequest->ux_slave_transfer_request_completion_code = UX_TRANSFER_STATUS_ABORT;
                if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                    // shall not be here.
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "on_stall_clear: Fail to put semaphore");
                }

                logical_address = AmbaRTSL_UsbGetEp20LogicalID(UdcEpInfo->UdcIndex);

                uret = UX_TRANSFER_STALLED;

                notify_event_to_application(usbd_sys_config, USBD_SYSTEM_EVENT_CLEAR_FEATURED, 0x02, 0, logical_address);
            }
        } else {
            if (AmbaRTSL_UsbGetEpInStatusRcs(EpNum) == 1U) {
                AmbaRTSL_UsbSetEpInStall(EpNum, 0);
                AmbaRTSL_UsbClrEpInStatusRcs(EpNum, 1); // write to clear
                UdcEpInfo->EpState                    &= ~USBD_UDC_ED_STATUS_STALLED;
                usbx_endpoint->ux_slave_endpoint_state = UX_ENDPOINT_RESET;

                // USBCV Command set test. After clear stall, out packet can not received data normally
                TransferRequest->ux_slave_transfer_request_completion_code = UX_TRANSFER_STATUS_ABORT;
                if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                    // shall not be here.
                }

                logical_address = AmbaRTSL_UsbGetEp20LogicalID(UdcEpInfo->UdcIndex) | 0x80U;
                notify_event_to_application(usbd_sys_config, USBD_SYSTEM_EVENT_CLEAR_FEATURED, 0x02, 0, logical_address);

            }
        }
    }

    return uret;
}

static void on_stall_set(const UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpDirection, UINT32 EpNum)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UINT32 rval;
    USBD_SYS_CONFIG_s *usbd_sys_config;

    rval = USBD_SystemConfigGet(&usbd_sys_config);
    if (rval == USB_ERR_SUCCESS) {
        UINT32 logical_address = (EpDirection == USBD_EP_DIR_IN) ? (0x80UL | EpNum) : (EpNum);

        // Get the pointer to the logical endpoint from the transfer request.
        usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

        if (EpDirection == USBD_EP_DIR_OUT) {
            if (AmbaRTSL_UsbGetEpOutStatusRss(EpNum) != 0U) {
                AmbaRTSL_UsbClrEpOutStatusRss(EpNum, 1);
                USBD_DrvEndpointStall(usbx_endpoint);

                notify_event_to_application(usbd_sys_config, USBD_SYSTEM_EVENT_SET_FEATURED, 0x02, 0, logical_address);
            }
        } else {
            if (AmbaRTSL_UsbGetEpInStatusRss(EpNum) != 0U) {
                AmbaRTSL_UsbClrEpInStatusRss(EpNum, 1);
                USBD_DrvEndpointStall(usbx_endpoint);

                notify_event_to_application(usbd_sys_config, USBD_SYSTEM_EVENT_SET_FEATURED, 0x02, 0, logical_address);
            }
        }
    }
}

// bna = Buffer Not Available
static void process_bna_endpoint_out(const UDC_ENDPOINT_INFO_s *UdcEpInfo, const USBD_SYS_CONFIG_s *UsbdSysConfig, UINT32 LogicalAddress)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[0x%X]: OUT BNA occurs, Status = 0x%X", LogicalAddress, UdcEpInfo->InterruptStatus, 0, 0, 0);

    if ((UdcEpInfo->DmaDesc == NULL) && (UdcEpInfo->DmaDescSize != 0U)) {
        // error handling
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "Xfer_BufferNotAvailableHandler(): Tried to invalidate illegal range");
    } else {

        USB_UtilityCacheInvdDataDesc(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescSize);

        {
            UINT32 DumpSize = 5;

            if (DumpSize > UdcEpInfo->DmaDescSize) {
                DumpSize = UdcEpInfo->DmaDescSize;
            }
            XferCb_DumpDmaDescriptor(UdcEpInfo->DmaDesc, DumpSize);
        }

        // write clear bna flag
        AmbaRTSL_UsbClrEpOutStatusBna(UdcEpInfo->PhysicalEpIndex, 1);

        // NAK this endpoint
        AmbaRTSL_UsbSetEpOutNAK(UdcEpInfo->PhysicalEpIndex, 1);

        // Notify the appliction.
        notify_event_to_application(UsbdSysConfig, USB_ERR_DEVICE_BNA, LogicalAddress, 0, 0);
    }
}

static void process_bna_endpoint_in(UX_SLAVE_TRANSFER         *TransferRequest,
                                    const UDC_ENDPOINT_INFO_s *UdcEpInfo,
                                    const USBD_SYS_CONFIG_s   *UsbdSysConfig,
                                    UINT32 LogicalAddress)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[0x%X]: IN BNA occurs, Status = 0x%X", LogicalAddress, UdcEpInfo->InterruptStatus, 0, 0, 0);

    // reset descriptor for bna situation
    USBD_UdcDmaDescReset(UdcEpInfo);

    // If bna occurs on control in, TxFifoEmpty may not pull up.
    if (LogicalAddress == 0x0U) {
        if (AmbaRTSL_UsbGetEpInStsTxEmpty(0) == 0U) {

            // To prevent timeout error, we need to release semaphore.
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "[0x%X]: Release semaphore.", LogicalAddress, 0, 0, 0, 0);

            if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                // shall not be here.
            }
            // print dma descriptor status
            {
                const UDC_DATA_DESC_s *desc = USB_UtilityPhyI32ToVirDataDesc(AmbaRTSL_UsbGetEpInDesptr(0));
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[0x%x]: Desc status  = 0x%X", LogicalAddress, desc->Status, 0, 0, 0);
            }
        }
    }


    // to change bna flag, poll demand should be set to zero
    AmbaRTSL_UsbSetEpInPollDemand(UdcEpInfo->PhysicalEpIndex, 0);

    // write clear bna flag
    AmbaRTSL_UsbClrEpInStatusBna(UdcEpInfo->PhysicalEpIndex, 1);

    // Notify the appliction.
    notify_event_to_application(UsbdSysConfig, USB_ERR_DEVICE_BNA, LogicalAddress, 0, 0);

}

static void on_buffer_not_available(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpDirection, UINT32 EpNum)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;
    UINT32 logical_address                 = usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress;
    UINT32 uret;
    USBD_SYS_CONFIG_s *usbd_sys_config;

    uret = USBD_SystemConfigGet(&usbd_sys_config);
    if (uret == USB_ERR_SUCCESS) {
        // DONT remove these codes until timing issue is clarified
        if (EpDirection == USBD_EP_DIR_OUT) {
            if (AmbaRTSL_UsbGetEpOutStatusBna(EpNum) == 1U) {
                // Out Buffer not available
                process_bna_endpoint_out(UdcEpInfo, usbd_sys_config, logical_address);
            }
        } else if (EpDirection == USBD_EP_DIR_IN) {
            if (AmbaRTSL_UsbGetEpInStatusBna(EpNum) == 1U) {
                process_bna_endpoint_in(TransferRequest, UdcEpInfo, usbd_sys_config, logical_address);
            }
        } else {
            // no action
        }
    }
}

static UDC_DATA_DESC_s *rx_data_length_get(UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 EpType, UINT32 EpNum, UINT32 *RxLength)
{
    UDC_DATA_DESC_s *desc_ret = NULL;

    // control out endpoint dummy data in SETUP stage
    if (EpNum == 0U) {
        USBD_UdcCtrlOutDataDescGet(&desc_ret);

        // invalidate DMA descriptor to get latest HW values.
        USB_UtilityCacheInvdDataDesc(desc_ret, sizeof(UDC_DATA_DESC_s));

        // check ctrl OUT length
        *RxLength = desc_ret->Status & 0x0FFFFU;
    } else {// other endpoint or EP #0 Out buf receives data
        if ((EpType == (UINT32)UX_BULK_ENDPOINT_OUT) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_OUT)) {
            if ((UdcEpInfo->DmaDesc != NULL) || (UdcEpInfo->DmaDescSize == 0U)) {
                desc_ret = USBD_UdcLastDescGet(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescNum);

                if ((desc_ret != NULL) && ((desc_ret->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_DONE)) {
                    *RxLength = desc_ret->Status & 0x0FFFFU;
                } else {
                    USB_UtilityCacheInvdDataDesc(&UdcEpInfo->SubDmaInfo.DmaDesc, sizeof(UDC_DATA_DESC_s));
                    if ((UdcEpInfo->SubDmaInfo.DmaDesc.Status & USBD_DMA_STATUS_MASK) == USBD_DMA_DONE) {
                        USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
                        UdcEpInfo->RxDmaState |= UDC_RX_DMA_STAT_ALT_DESC;
                        USBD_UdcRxStateMutexPut();
                        desc_ret  = &UdcEpInfo->SubDmaInfo.DmaDesc;
                        *RxLength = desc_ret->Status & 0xFFFFU;
                    } else {
                        if (desc_ret != NULL) {
                            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "DMA status = 0x%x", desc_ret->Status, 0, 0, 0, 0);
                            desc_ret = NULL;
                        } else {
                            // error handling
                            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "Desc Is Null");
                            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "GetReceiveDadaLength, sts = 0x%x", AmbaRTSL_UsbGetEpOutStatus(EpNum), 0, 0, 0, 0);
                        }
                    }
                }
            }
        }
    }

    return desc_ret;
}

static void process_out_data_alt_desc(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpNum)
{
    UINT32 flag_update    = 0;
    UINT32 residue_length = 0;
    UINT8 *residue_ptr    = NULL;

    // Since the alternate desc is in use, push the data to swfifo.
    USBD_UdcRxSwfifoPush(UdcEpInfo, TransferRequest);

    if ((UdcEpInfo->RxDmaState & UDC_RX_DMA_STAT_REQ_PENDING) != 0U) {
        // If a transfer request is pending, pop the data of swfifo to request buffer.
        USBD_UdcRxSwfifoPop(UdcEpInfo, TransferRequest, &residue_ptr, &residue_length);
        USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
        UdcEpInfo->RxDmaState &= ~UDC_RX_DMA_STAT_REQ_PENDING;
        USBD_UdcRxStateMutexPut();

        if (residue_length == 0U) {
            // The alternate desc receives a short packet or zero-length data, transfer is complete.
            // Nak when the transfer is complete.
            AmbaRTSL_UsbSetEpOutNAK(EpNum, 1);
            // No request is pending. Thus we use the alternate desc.
            USBD_UdcRxSubDescSetup(UdcEpInfo);
            // The transfer is completed.
            TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;
            flag_update                                       = 1;
        } else {
            // Re-configure the descriptor after the data is pop from swfifo.
            USBD_UdcRxDescSetup(UdcEpInfo, TransferRequest, residue_ptr, residue_length, 1);
        }
    } else {
        // Nak when the alternate desc is in use but no request is pending.
        AmbaRTSL_UsbSetEpOutNAK(EpNum, 1);
        // No request is pending. Thus we use the alternate desc.
        USBD_UdcRxSubDescSetup(UdcEpInfo);
        flag_update = 1;
    }
    // Enable the rdma.
    AmbaRTSL_UsbSetDevCtlRde(1);
    if (flag_update == 1U) {
        // Inform the request side the transfer is complete.
        if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
            // shall not be here.
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "process_out_data_alt_desc(): sema put out failed");
        }
    }
}

static void process_out_data_overflow(const UDC_ENDPOINT_INFO_s *UdcEpInfo,
                                      UX_SLAVE_TRANSFER         *TransferRequest,
                                      UINT32 RxLength,
                                      UINT32 EpType,
                                      UINT32 EpNum,
                                      UINT32 *FlagDone,
                                      UINT32 *Status)
{
    const UX_SLAVE_ENDPOINT *Endpoint;

    // Get the pointer to the logical endpoint from the transfer request.
    Endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "process_out_data(): Overflow condition for 0x%X, actal length = %d, req length = %d",
                          Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                          TransferRequest->ux_slave_transfer_request_actual_length + RxLength,
                          TransferRequest->ux_slave_transfer_request_requested_length,
                          0,
                          0);

    (void)RxLength;

    if ((EpType == (UINT32)UX_BULK_ENDPOINT_OUT) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_OUT)) {
        // pollo - 2014/09/10
        //     Going here means Device receives a packet with size > expected,
        //     what we can do is to return the size and mark this transfer complete.
        //     Applications should take care of this.

        // Set the completion code to no error.
        TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

        // Disable the interrupt on this endpoint.
        AmbaRTSL_UsbDisOutEpInt(EpNum);

        // clean DMA descriptor to prevent USB DMA engine to receive data again.
        USBD_UdcInitDmaDesc(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescNum);

        // The transfer is completed.
        TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

        if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
            // shall not be here.
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "process_out_data(): sema put out tag0 failed");
        }
        *FlagDone = 1;
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "Don't know how to handle overflow condition for EP 0x%X",
                              Endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                              0,
                              0,
                              0,
                              0);
        *Status = USB_ERR_FAIL;
    }
}

static void process_out_data_non_control_done(UDC_ENDPOINT_INFO_s *UdcEpInfo,
        UX_SLAVE_TRANSFER *TransferRequest,
        UINT32 EpNum)
{
    // Nak the endpoint because the current request is complete.
    AmbaRTSL_UsbSetEpOutNAK(EpNum, 1);

    // The transfer is completed.
    TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;
    USBD_UdcRxStateMutexGet(USB_WAIT_FOREVER);
    UdcEpInfo->RxDmaState &= ~UDC_RX_DMA_STAT_REQ_PENDING;
    USBD_UdcRxStateMutexPut();

    // Use the alternate desc.
    USBD_UdcRxSubDescSetup(UdcEpInfo);
    // Enable the RDMA.
    AmbaRTSL_UsbSetDevCtlRde(1);

    if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
        // shall not be here.
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "process_out_data(): sema put out tage 1 failed");
    }
}

static void process_out_data_control_data_copy(UX_SLAVE_TRANSFER *TransferRequest,
        const UDC_DATA_DESC_s *Desc,
        UINT32  RxLength,
        UINT8  *DataPointer,
        UINT32 *FlagDone,
        UINT32 *Status)
{
    const UINT8 *PacketBuf;

    // Adjust the buffer address.
    TransferRequest->ux_slave_transfer_request_current_data_pointer = &TransferRequest->ux_slave_transfer_request_current_data_pointer[RxLength];

    // Update the length of the data received.
    TransferRequest->ux_slave_transfer_request_actual_length += RxLength;

    if (Desc != NULL) {
        // Get the content of the DMA buffer into the client buffer.
        PacketBuf = USB_UtilityPhyI32ToVirU8P(Desc->DataPtr);
    } else {
        PacketBuf = NULL;
    }

    // Sanity check
    if (PacketBuf == NULL) {
        *Status     = USB_ERR_FAIL;
        *FlagDone   = 1;
    } else {
        USB_UtilityCacheInvdUInt8(PacketBuf, RxLength);

        // get data from out buffer to the pointer in transfer_reqeust
        USB_UtilityMemoryCopy(DataPointer, PacketBuf, RxLength);
    }
}

static void process_out_data_control_done(UX_SLAVE_TRANSFER *TransferRequest, UDC_DATA_DESC_s *Desc)
{
    // Disable the interrupt on this endpoint.
    AmbaRTSL_UsbDisOutEpInt(0);
    AmbaRTSL_UsbSetEpOutNAK(0, 1);

    // The transfer is completed.
    TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

    if (Desc != NULL) {
        // clean DMA descriptor to prevent USB DMA engine to receive data again.
        Desc->Status = USBD_DMA_LAST_DESC;

        USB_UtilityCacheFlushDataDesc(Desc, sizeof(UDC_DATA_DESC_s));
    }

    if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
        // shall not be here.
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "process_out_data(): sema put out tage 2 failed");
    }
}

static UINT32 process_out_data(UDC_ENDPOINT_INFO_s *UdcEpInfo,
                               UX_SLAVE_TRANSFER *TransferRequest,
                               UINT32 EpType,
                               UINT32 RxLength,
                               UDC_DATA_DESC_s *Desc,
                               UINT32 EpNum)
{
    UINT8 * data_ptr;
    UINT32 uret = USB_ERR_SUCCESS;

    // Obtain the current data buffer address.
    data_ptr = TransferRequest->ux_slave_transfer_request_current_data_pointer;

    // Check whether the RX dma done when alternate desc is in use.
    if (is_alt_desc_used(EpType, UdcEpInfo) != 0U) {
        process_out_data_alt_desc(UdcEpInfo, TransferRequest, EpNum);
        uret = USB_ERR_SUCCESS;
    } else {
        UINT32 flag_done = 0;

        if ((TransferRequest->ux_slave_transfer_request_actual_length + RxLength) >
            TransferRequest->ux_slave_transfer_request_requested_length) {
            // overflow condition
            process_out_data_overflow(UdcEpInfo, TransferRequest, RxLength, EpType, EpNum, &flag_done, &uret);
        }

        if (flag_done == 0U) {
            if ((EpType == (UINT32)UX_BULK_ENDPOINT_OUT) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_OUT)) {
                // Adjust the buffer address.
                TransferRequest->ux_slave_transfer_request_current_data_pointer = &TransferRequest->ux_slave_transfer_request_current_data_pointer[RxLength];
                // Update the length of the data received.
                TransferRequest->ux_slave_transfer_request_actual_length += RxLength;
            } else {
                // need to do memory copy for control endpoint
                process_out_data_control_data_copy(TransferRequest, Desc, RxLength, data_ptr, &flag_done, &uret);
            }

            if (flag_done == 0U) {
                // Set the completion code to no error.
                TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

                // Check what type of endpoint we are using, Control or non Control.
                if (EpNum != 0U) {
                    // non-control endpoint
                    process_out_data_non_control_done(UdcEpInfo, TransferRequest, EpNum);
                } else {
                    // control endpoint
                    process_out_data_control_done(TransferRequest, Desc);
                }
            }
        }
    }

    return uret;
}

static void on_out_data(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpType, UINT32 EpNum)
{
    UDC_DATA_DESC_s *dma_desc;
    UINT32           rx_length = 0;
    UINT32           flag_done = 0;

    // clear data out interrupt bit
    AmbaRTSL_UsbClrEpOutStatusOut(EpNum, EP_OUT_RCV_DATA);

    if (is_alt_desc_used(EpType, UdcEpInfo) != 0U) {
        dma_desc = NULL;
    } else {
        dma_desc = rx_data_length_get(UdcEpInfo, EpType, EpNum, &rx_length);

        if (dma_desc == NULL) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "desc is NULL for PHY EP %d", EpNum, 0, 0, 0, 0);
            flag_done = 1;
        } else {
            if (rx_length == 0xFFFFFFFFU) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "RXLength is 0xFFFFFFFF for PHY EP %d", EpNum, 0, 0, 0, 0);
                flag_done = 1;
            }
        }
    }

    if (flag_done == 0U) {
        if (process_out_data(UdcEpInfo, TransferRequest, EpType, rx_length, dma_desc, EpNum) != USB_ERR_SUCCESS) {
            /// shall not be here.
        }
    }
}

static void process_txdma_complete_iso(UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 EpNum)
{
    const UDC_ISO_REQUEST_s *iso_req          = &UdcEpInfo->IsoReq;
    UX_SLAVE_TRANSFER       *transfer_request = iso_req->TransferRequest;
    USBD_UDC_s              *usb_udc;

    // Set the completion code to no error.
    transfer_request->ux_slave_transfer_request_completion_code = UX_SUCCESS;
    USBD_UdcDmaDescReset(UdcEpInfo);

    // Update current wrapper progress status

    // Check if next wrapper is ready
    // If yes, patch next wrapper and issue poll demand.
    // If not, just return.

    if (transfer_request->completion_function != NULL) {
        transfer_request->completion_function(transfer_request);
    }
    iso_req->DescWrapper->Used = 0;

    // tell driver that there is no ongoing Isochronous TX
    USBD_UdcFlagIsoTxOngoingSet(0);

    if (USBD_UdcUdcInstanceGet(&usb_udc) == USB_ERR_SUCCESS) {
        if (USBD_UdcIsoDescPatch(usb_udc, UdcEpInfo, UdcEpInfo->DmaDescNum, 0) == USB_ERR_SUCCESS) {
            // Append transfer
            AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
        }
    }
}

// Control Endpoint: TX done and setup DMA descriptor for receiving ACK packet from HOST
static void txdma_complete_control_ack(UX_SLAVE_TRANSFER *TransferRequest)
{
    UDC_DATA_DESC_s *dma_desc;

    // Transfer 0 length data.
    // Reset the ZLP condition.
    TransferRequest->ux_slave_transfer_request_force_zlp = 0;
    // We are using a Control endpoint, if there is a callback, invoke it. We are still under ISR.
    flag_bypass_in_token = 0;
    if (TransferRequest->completion_function != NULL) {
        TransferRequest->completion_function(TransferRequest);
    }

    // reenable RX DMA and clear NAK
    USBD_UdcInitCtrlSetupDesc();
    // prepare control endpoint RX descriptor
    USBD_UdcCtrlOutDataDescGet(&dma_desc);
    dma_desc->Status = USBD_DMA_LAST_DESC;

    USB_UtilityCacheFlushDataDesc(dma_desc, sizeof(UDC_DATA_DESC_s));

    AmbaRTSL_UsbSetEpOutRxReady(0, 1);
    AmbaRTSL_UsbSetDevCtlRde(1);

    if (AmbaRTSL_UsbGetEpOutNAK(0) != 0U) {
        AmbaRTSL_UsbClearEpOutNAK(0, 1);
    }
}

// Non-Control Endpoint: TX done
static void txdma_complete_non_control_done(const UDC_ENDPOINT_INFO_s *UdcEpInfo,
        UX_SLAVE_TRANSFER         *TransferRequest,
        UINT32                     EpNum,
        UINT32                     CheckTxFifo)
{
    // Set the completion code to no error.
    TransferRequest->ux_slave_transfer_request_completion_code = UX_SUCCESS;

    // If TxFifoEmpty comes with TxDmaDone, release semaphore to prevent timeout error.

    // Max 2016/12/29 - Fix issue : Tx timeout in multiple TX and fast transfer case.
    // Root cause : The DMA DONE/TXFIFO EMPTY IRQ won't happen in multiple TX endpoints case.
    //              When sending tx transfer fast, there's chance DMA complete and
    //              TXFIFO empty IRQ are missed.
    // Solution :   We accumulate a counter of IN IRQ and when the counter reach threshold,
    //              we check the descriptor directly. In that case, we don't need to wait TXFIFO empty.
    if (CheckTxFifo != 0U) {
        if (is_tx_empty_occur(USBD_EP_DIR_IN, UdcEpInfo->InterruptStatus) != 0U) {
            // Disable the interrupt on this endpoint.
            AmbaRTSL_UsbDisInEpInt(EpNum);
            AmbaRTSL_UsbSetEpInNAK(EpNum, 1);

            // The transfer is completed.
            TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

            if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                // shall not be here.
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "sema put in failed 0x%02x");
            }
        }
    } else {
        // Disable the interrupt on this endpoint.
        AmbaRTSL_UsbDisInEpInt(EpNum);
        AmbaRTSL_UsbSetEpInNAK(EpNum, 1);

        // The transfer is completed.
        TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

        if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
            // shall not be here.
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "sema put in failed 0x%02x");
        }
    }
}

// Non-Control Endpoint: TX done and start next transfer?
static void txdma_complete_non_control_cont(const UDC_ENDPOINT_INFO_s *UdcEpInfo,
        UX_SLAVE_TRANSFER         *TransferRequest,
        UINT32                     EpNum,
        UINT32                     CheckTxFifo)
{
    if (CheckTxFifo != 0U) {
        if (is_tx_empty_occur(USBD_EP_DIR_IN, UdcEpInfo->InterruptStatus) != 0U) {
            // Disable the interrupt on this endpoint.
            AmbaRTSL_UsbDisInEpInt(EpNum);

            // The transfer is not finished yet
            TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_NOT_PENDING;
            if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                // shall not be here.
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "sema put in failed 0x%02x");
            }
        }
    }
}
// Control Endpoint: TX done and start next transfer?
static void txdma_complete_control_cont(const UDC_ENDPOINT_INFO_s *UdcEpInfo,
                                        UX_SLAVE_TRANSFER         *TransferRequest,
                                        UINT32                     EpNum,
                                        UINT32                     CheckTxFifo)
{
    // Only for Control In transfer
    // CNAK control-IN EP.
    if (AmbaRTSL_UsbGetEpInNAK(0) != 0U) {
        AmbaRTSL_UsbClearEpInNAK(0, 1);
    }

    // If TxFifoEmpty comes with TxDmaDone, release semaphore to prevent timeout error.
    if (CheckTxFifo != 0U) {
        if (is_tx_empty_occur(USBD_EP_DIR_IN, UdcEpInfo->InterruptStatus) != 0U) {
            // Disable the interrupt on this endpoint.
            AmbaRTSL_UsbDisInEpInt(EpNum);

            // The transfer is not finished yet
            TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_NOT_PENDING;
            if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
                // shall not be here.
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "sema put in failed 0x%02x");
            }
        }
    }
}

static void process_txdma_complete(UDC_ENDPOINT_INFO_s *UdcEpInfo,
                                   UX_SLAVE_TRANSFER   *TransferRequest,
                                   UINT32               EpType,
                                   UINT32               EpNum,
                                   UINT32               CheckTxFifo)
{
    if (EpType == (UINT32)UX_ISOCHRONOUS_ENDPOINT_IN) {
        process_txdma_complete_iso(UdcEpInfo, EpNum);
    } else {
        // important !! re-init and then DMA engine would not get confused
        if ((EpType == (UINT32)UX_BULK_ENDPOINT_IN) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_IN)) {
            USBD_UdcDmaDescReset(UdcEpInfo);
        }

        // Check if we have data to send.
        if (TransferRequest->ux_slave_transfer_request_in_transfer_length == 0U) {
            // There is no data to send but we may need to send a Zero Length Packet.
            if ((TransferRequest->ux_slave_transfer_request_force_zlp == 1U) && (EpNum == 0U)) {
                txdma_complete_control_ack(TransferRequest);
            } else {
                txdma_complete_non_control_done(UdcEpInfo, TransferRequest, EpNum, CheckTxFifo);
            }
        } else {
            // TX not finished yet, continue to send
            if ((EpType == (UINT32)UX_BULK_ENDPOINT_IN) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_IN)) {
                txdma_complete_non_control_cont(UdcEpInfo, TransferRequest, EpNum, CheckTxFifo);
            } else if (EpType == (UINT32)UX_CONTROL_ENDPOINT) {
                txdma_complete_control_cont(UdcEpInfo, TransferRequest, EpNum, CheckTxFifo);
            } else {
                // don't handler other endpoint type
            }
        }
    }
}

static void on_txdma_complete(UDC_ENDPOINT_INFO_s *UdcEpInfo,
                              UX_SLAVE_TRANSFER   *TransferRequest,
                              UINT32               EpType,
                              UINT32               EpNum,
                              UINT32               EpStatus)
{
    // Clear all status here. Some times there are other status occur. write and clear them.
    AmbaRTSL_UsbSetEpInStatus(EpNum, EpStatus);

    UdcEpInfo->TxPending = 0;

    // Clear control Nak when receving Tx dma complete ISR to make next in-token recevied
    if (EpNum == 0U) {
        if (AmbaRTSL_UsbGetEpInNAK(EpNum) != 0U) {
            AmbaRTSL_UsbClearEpInNAK(EpNum, 1);
        }
        flag_bypass_in_token = 1;
    }

    process_txdma_complete(UdcEpInfo, TransferRequest, EpType, EpNum, 1);
}

static void on_tx_empty(UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpNum)
{
    AmbaRTSL_UsbClrEpInStsTxEmpty(EpNum, 1);

    /* Disable the interrupt on this endpoint.  */
    AmbaRTSL_UsbDisInEpInt(EpNum);
    AmbaRTSL_UsbSetEpInNAK(EpNum, 1);

    if (EpNum == 0U) {
        flag_bypass_in_token = 0;
        if (TransferRequest->completion_function != NULL) {
            TransferRequest->completion_function(TransferRequest);
        }
    }

    // The transfer is completed.
    TransferRequest->ux_slave_transfer_request_status = UX_TRANSFER_STATUS_COMPLETED;

    // Non control endpoint operation, use semaphore.
    if (USB_UtilitySemaphoreGive(&TransferRequest->ux_slave_transfer_request_semaphore) != USB_ERR_SUCCESS) {
        // shall not be here.
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "sema put in failed 0x%02x");
    }
}
static void on_in_token_control_poll(UDC_ENDPOINT_INFO_s *UdcEpInfo, const UDC_DATA_DESC_s *DmaDesc, UINT32 EpNum)
{
    USB_UtilityCacheInvdDataDesc(DmaDesc, sizeof(UDC_DATA_DESC_s));
    if ((DmaDesc->Status & USBD_DMA_HOST_NOT_RDY) == USBD_DMA_HOST_READY) {
        // Nak current In token if it's a control endpoint. Clear Nak when receving next tx dam complete ISR
        //   It is to make sure data in tx fifo is ready when in token is comming
        AmbaRTSL_UsbSetEpInNAK(EpNum, 1);
        AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
        UdcEpInfo->TxPending = 1;
    }
}

static void on_in_token_bulk_poll(UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 EpNum)
{
    if (USBD_UdcLastDescGet(UdcEpInfo->DmaDesc, USBD_BULK_IN_CHAIN_DESC_NUM) != NULL) {
        if (AmbaRTSL_UsbGetEpInNAK(EpNum) != 0U) {
            AmbaRTSL_UsbClearEpInNAK(EpNum, 1);
        }
        AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
        UdcEpInfo->TxPending = 1;
    }
}

static void on_in_token_interrupt_poll(UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 EpNum)
{
    if (USBD_UdcLastDescGet(UdcEpInfo->DmaDesc, USBD_INTERRUPT_CHAIN_DESC_NUM) != NULL) {
        if (AmbaRTSL_UsbGetEpInNAK(EpNum) != 0U) {
            AmbaRTSL_UsbClearEpInNAK(EpNum, 1);
        }
        AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
        UdcEpInfo->TxPending = 1;
    }
}
static void on_in_token_iso_poll(UDC_ENDPOINT_INFO_s *UdcEpInfo, UINT32 EpNum)
{
    USBD_UDC_s *usbd_udc;
    // through transfer wrapper
    // Patch descriptor
    if (USBD_UdcUdcInstanceGet(&usbd_udc) == USB_ERR_SUCCESS) {
        if (USBD_UdcIsoDescPatch(usbd_udc, UdcEpInfo, UdcEpInfo->DmaDescNum, 1) == USB_ERR_SUCCESS) {
            // Start new transfer
            AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
        }
    }
}

static void on_in_token(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpType, UINT32 EpNum)
{
    const UDC_DATA_DESC_s *dma_desc = NULL;
    static UINT32 null_token_count  = 0;
    UINT32 flag_done                 = 0;

    // clear the bit
    AmbaRTSL_UsbClrEpInStatusIn(EpNum, 1);
    if ((EpNum != 0U) || (flag_bypass_in_token == 0U)) {
        // tx DMA not done yet ?
        if (UdcEpInfo->TxPending != 0U) {
            null_token_count++;

            if (is_ctrl_ep_type(EpType) == 0U) {
                dma_desc = USBD_UdcLastDescGet(UdcEpInfo->DmaDesc, UdcEpInfo->DmaDescNum);
                if (dma_desc != NULL) {
                    UINT32 flag_nak = AmbaRTSL_UsbGetEpInNAK(EpNum);
                    // Case 1 :make sure no NAK and then re-arm again
                    // Case 2 : In some case the 1st Poll Demand Fail. No DMA_DONE ISR and descriptor update,
                    // which causes In-Token ISR keep occupying system. If In-Token ISR accumulates 20 times,
                    // We set the poll_demand again.
                    // Both case should make sure the descriptor status is HOST_READY.

                    if (((dma_desc->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_HOST_READY) &&
                        ((flag_nak == 1U) || (null_token_count > 20U))) {
                        AmbaRTSL_UsbClearEpInNAK(EpNum, 1);
                        AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
                        null_token_count = 0;
                    }

                    // FIXME: a workaround since TX DMA done interrupt doesn't
                    //        come even the descriptor status is updated to DONE
                    //        since the next IN transacation is waiting, so we
                    //        assume the TX DMA DONE int is lost so go to the
                    //        handling if find case below.

                    if (((dma_desc->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_DONE) &&
                        (null_token_count > 10U)) {
                        process_txdma_complete(UdcEpInfo, TransferRequest, EpType, EpNum, 0);
                        null_token_count  = 0;
                        UdcEpInfo->TxPending = 0;
                    }
                } else {
                    // error handling
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "on_in_token: desc is null for ep %d (type 0x%X)", EpNum, EpType, 0, 0, 0);
                }
            } else {
                // Case 1 :make sure no NAK and then re-arm again
                // Case 2 : In some case the 1st Poll Demand Fail. No DMA_DONE ISR and descriptor update,
                // which causes In-Token ISR keep occupying system. If In-Token ISR accumulates 20 times,
                // We set the poll_demand again.
                UINT32 Nak = AmbaRTSL_UsbGetEpInNAK(EpNum);
                if ((Nak == 1U) || (null_token_count > 20U)) {
                    UDC_DATA_DESC_s *pre_alloc_desc;
                    USBD_UdcCtrlInDataDescGet(&pre_alloc_desc);
                    USB_UtilityCacheInvdDataDesc(pre_alloc_desc, sizeof(UDC_DATA_DESC_s));
                    if ((pre_alloc_desc->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_HOST_READY) {
                        AmbaRTSL_UsbClearEpInNAK(EpNum, 1);
                        AmbaRTSL_UsbSetEpInPollDemand(EpNum, 1);
                        null_token_count = 0;
                    } else {
                        if ((pre_alloc_desc->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_DONE) {
                            // The DMA descriptor has DONE status but there is no TX DMA DONE/TX FIFO EMPTY
                            // interrupt received.
                            // Assume the data has been moved to TXFIFO, clear NAK to complete
                            // control-in data stage.
                            // This only happens when USB HOST OS is Ubuntu with
                            // many back-to-back control requests.
                            AmbaRTSL_UsbClearEpInNAK(EpNum, 1);
                            null_token_count = 0;
                        }
                    }
                }
            }

            flag_done = 1;
        } else {
            null_token_count = 0;
        }

        if (flag_done == 0U) {
            // if TX descriptor is ready, issue POLL bit.
            if (EpType == (UINT32)UX_CONTROL_ENDPOINT) {
                dma_desc = USB_UtilityPhyI32ToVirDataDesc(AmbaRTSL_UsbGetEpInDesptr(EpNum));
                on_in_token_control_poll(UdcEpInfo, dma_desc, EpNum);
            } else if (EpType == (UINT32)UX_BULK_ENDPOINT_IN) {
                on_in_token_bulk_poll(UdcEpInfo, EpNum);
            } else if (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_IN) {
                on_in_token_interrupt_poll(UdcEpInfo, EpNum);
            } else if (EpType == (UINT32)UX_ISOCHRONOUS_ENDPOINT_IN) {
                on_in_token_iso_poll(UdcEpInfo, EpNum);
            } else {
                // shall not be here.
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "on_in_token: don't support endpoint type 0x%x", EpType, 0, 0, 0, 0);
            }
        }
    }
}

static void on_rxdma_complete(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT32 EpType, UINT32 EpNum)
{
    UDC_DATA_DESC_s *target_desc;
    UINT32 rx_length;
    UDC_DATA_DESC_s *dma_desc = UdcEpInfo->DmaDesc;

    if ((EpType == (UINT32)UX_BULK_ENDPOINT_OUT) || (EpType == (UINT32)UX_INTERRUPT_ENDPOINT_OUT)) {
        if ((dma_desc != NULL) || (UdcEpInfo->DmaDescSize == 0U)) {
            USB_UtilityCacheInvdDataDesc(dma_desc, UdcEpInfo->DmaDescSize);


            if (EpType == (UINT32)UX_BULK_ENDPOINT_OUT) {
                target_desc = USBD_UdcLastDescGet(dma_desc, USBD_BULK_OUT_CHAIN_DESC_NUM);
            } else {
                target_desc = USBD_UdcLastDescGet(dma_desc, USBD_INTERRUPT_CHAIN_DESC_NUM);
            }


            if ((target_desc != NULL) && ((target_desc->Status & USBD_DMA_STATUS_MASK) == USBD_DMA_DONE)) {
                rx_length = target_desc->Status & 0xFFFFU;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "[0x%X][2] RX length %d",
                                      TransferRequest->ux_slave_transfer_request_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress,
                                      rx_length,
                                      0,
                                      0,
                                      0);
                if (process_out_data(UdcEpInfo, TransferRequest, EpType, rx_length, target_desc, EpNum) != USB_ERR_SUCCESS) {
                    // error handling
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "on_rxdma_complete error tag 1");
                }
            } else {
                // error handling
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "on_rxdma_complete error tag 2");
            }
        }
    }
}

/** \addtogroup Internal_APIs
 *  @{
 * */
/**
 * Called by ISR task to handle interrupt events for one USB endpoint.
 * */
void USBD_DrvTransferCallback(USBD_UDC_s *Udc, UINT32 InOut, UINT32 EpNum, UX_SLAVE_TRANSFER *TransferRequest)
{
    const UX_SLAVE_ENDPOINT *usbx_endpoint;
    UDC_ENDPOINT_INFO_s *udc_ep_info;
    UINT32 udc_ep_mask;
    UINT32 status = UX_SUCCESS;

    AmbaMisra_TouchUnused(Udc);

    // Get the pointer to the logical endpoint from the transfer request.
    usbx_endpoint = TransferRequest->ux_slave_transfer_request_endpoint;

    if (usbx_endpoint != NULL) {
        // Build the endpoint mask from the endpoint descriptor.
        udc_ep_mask = (usbx_endpoint->ux_slave_endpoint_descriptor.bmAttributes & (UINT32)UX_MASK_ENDPOINT_TYPE) |
                      (usbx_endpoint->ux_slave_endpoint_descriptor.bEndpointAddress & (UINT32)UX_ENDPOINT_DIRECTION);

        // Get the udc endpoint.
        udc_ep_info = USB_UtilityVoidP2UdcEd(usbx_endpoint->ux_slave_endpoint_ed);
        if (udc_ep_info != NULL) {
#if defined(AMBA_USB_DEBUG)
#ifdef  USB_PRINT_SUPPORT
            if (InOut == USBD_EP_DIR_OUT) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "Out status = 0x%x for EP %d", udc_ep_info->InterruptStatus, EpNum, 0, 0, 0);
            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "In status = 0x%x for EP %d", udc_ep_info->InterruptStatus, EpNum, 0, 0, 0);
            }
#endif
#endif

            // Check if the request_tranfer is for Control endpoint && we have a SETUP condition.
            if (is_setup_packet(InOut, EpNum, udc_ep_info->InterruptStatus) != 0U) {
                on_setup_packet(TransferRequest);
            } else { // NOT CONTROL OUT SETUP packet
                status = on_stall_clear(udc_ep_info, TransferRequest, InOut, EpNum);

                if (status != (UINT32)UX_TRANSFER_STALLED) {
                    on_stall_set(TransferRequest, InOut, EpNum);

                    on_buffer_not_available(udc_ep_info, TransferRequest, InOut, EpNum);
                    // Check if we have a Reception of data  on a OUT endpoint
                    if (is_out_packet(InOut, udc_ep_info->InterruptStatus) != 0U) {
                        on_out_data(udc_ep_info, TransferRequest, udc_ep_mask, EpNum);
                    } else if (is_txdma_done_occur(InOut, udc_ep_info->InterruptStatus, udc_ep_info->TxPending, udc_ep_mask) != 0U) {
                        on_txdma_complete(udc_ep_info, TransferRequest, udc_ep_mask, EpNum, udc_ep_info->InterruptStatus);
                    } else if (is_tx_empty_occur(InOut, udc_ep_info->InterruptStatus) != 0U) {
                        on_tx_empty(TransferRequest, EpNum);
                    } else if (is_in_packet(InOut, udc_ep_info->InterruptStatus) != 0U) {
                        on_in_token(udc_ep_info, TransferRequest, udc_ep_mask, EpNum);
                    } else if (InOut == USBD_EP_DIR_OUT) {
                        on_rxdma_complete(udc_ep_info, TransferRequest, udc_ep_mask, EpNum);
                    } else {
                        // shall not be here.
                    }
                }
            }
        }
    }
}
/** @} */
