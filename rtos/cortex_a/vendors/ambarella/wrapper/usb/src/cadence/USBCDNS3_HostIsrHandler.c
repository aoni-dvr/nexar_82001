/**
 *  @file USBCDNS3_HostIsrHandler.c
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
 *  @details USB ISR handler for Cadence USB host controller.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_CadenceSanity.h>

static void host_connect_status_change_connect(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortStatus)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_connect_status_change_connect(): <%d> Device Connected on port: %d",
                            DrvRes->instanceNo,
                            DrvRes->ActualPort,
                            0, 0, 0);

    DrvRes->Connected = 1U;

    // check if port enabled
    if ((PortStatus & XHCI_PORTSC_PED_MASK) > 0U) {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_connect_status_change_connect(): <%d> Port %d ENABLED!",
                                DrvRes->instanceNo,
                                DrvRes->ActualPort,
                                0, 0, 0);

        if (DrvRes->ActualPort > 0U) {
            // just let upper layer (protocol layer) handle this
            if (DrvRes->HostCallbacks.ConnectionChanged != NULL) {
                DrvRes->HostCallbacks.ConnectionChanged(DrvRes->ActualPort - 1U);
            } else {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR,
                    "host_connect_status_change_connect(): No callback function registered for port enabled!");
            }
        } else {

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "host_connect_status_change_connect(): <%d> PortID %d should be > 0!",
                                    DrvRes->instanceNo,
                                    DrvRes->ActualPort,
                                    0, 0, 0);
        }

    } else {
        // port is disabled
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_connect_status_change_connect(): <%d> Port %d DISABLED!",
                                DrvRes->instanceNo,
                                DrvRes->ActualPort,
                                0, 0, 0);

        if (DrvRes->ActualPort > 0U) {
            // just let upper layer (protocol layer) handle this
            if (DrvRes->HostCallbacks.ConnectionChanged != NULL) {
                DrvRes->HostCallbacks.ConnectionChanged(DrvRes->ActualPort - 1U);
            } else {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR,
                    "host_connect_status_change_connect(): No callback function registered for port disabled!");
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "host_connect_status_change_connect(): <%d> PortID %d should be > 0!",
                                    DrvRes->instanceNo,
                                    DrvRes->ActualPort,
                                    0, 0, 0);
        }
    }
}

static void host_connect_status_change_disconnect(XHC_CDN_DRV_RES_s *DrvRes)
{
    // set U1 nd U2 to default value
    USBCDNS3_XhcU1TimeoutSet(DrvRes, 0);
    USBCDNS3_XhcU2TimeoutSet(DrvRes, 0);

    if (DrvRes->ActualPort > 0U) {
        // just let upper layer (protocol layer) handle this
        if (DrvRes->HostCallbacks.ConnectionChanged != NULL) {
            DrvRes->HostCallbacks.ConnectionChanged(DrvRes->ActualPort - 1U);
        } else {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR,
                "host_connect_status_change_disconnect(): No callback function registered for disconnect!");
        }
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "host_connect_status_change_disconnect(): <%d> PortID %d should be > 0!",
                                DrvRes->instanceNo,
                                DrvRes->ActualPort,
                                0, 0, 0);
    }

    DrvRes->Connected = 0U;

}

static void host_connect_status_change(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortStatus) {

    // check CCS bit, check if device is connected
    if ((PortStatus & XHCI_PORTSC_CCS_MASK) != 0U) {
        host_connect_status_change_connect(DrvRes, PortStatus);
    } else {
        host_connect_status_change_disconnect(DrvRes);
    }
}


static void host_event_process_port_change(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32 portStatus;
    UINT32 port_id;
    UINT32 maskAllChangeBits = XHCI_PORTSC_CSC_MASK
            | XHCI_PORTSC_PEC_MASK | XHCI_PORTSC_WRC_MASK
            | XHCI_PORTSC_PRC_MASK | XHCI_PORTSC_PLC_MASK
            | XHCI_PORTSC_CEC_MASK | XHCI_PORTSC_OCC_MASK;

    port_id = USBCDNS3_TrbPortIdGet(DrvRes->EventPtr);
    DrvRes->ActualPort = port_id;
    portStatus = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTSC);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_event_process_port_change(): <%d> Port ID: %d, PORTSC: 0x%08X PORTPMSC: 0x%08X",
                            DrvRes->instanceNo,
                            DrvRes->ActualPort,
                            portStatus,
                            USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTPMSC),
                            0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "    CCS %d, PED %d, PR %d, PP %d",
                        (portStatus & XHCI_PORTSC_CCS_MASK),
                        (portStatus & XHCI_PORTSC_PED_MASK) >> XHCI_PORTSC_PED_SHIFT,
                        (portStatus & XHCI_PORTSC_PR_MASK) >> XHCI_PORTSC_PR_SHIFT,
                        (portStatus & XHCI_PORTSC_PP_MASK) >> XHCI_PORTSC_PP_SHIFT, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "    PLS %d, PortSpeed %d, PRC %d",
                        (portStatus & XHCI_PORTSC_PLS_MASK) >> XHCI_PORTSC_PLS_SHIFT,
                        (portStatus & XHCI_PORTSC_PORTSPEED_MASK) >> XHCI_PORTSC_PORTSPEED_SHIFT,
                        (portStatus & XHCI_PORTSC_PRC_MASK) >> XHCI_PORTSC_PRC_SHIFT,
                        0, 0);

    do {
        // clear all interrupts
        USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTSC, portStatus & ~(XHCI_PORTSC_PED_MASK));

        // handle connect status change
        if ((portStatus & XHCI_PORTSC_CSC_MASK) != 0U) {
            host_connect_status_change(DrvRes, portStatus);
        }

        // handle port reset change IF port is connected
        if (((portStatus & XHCI_PORTSC_PRC_MASK) != 0U) &&
            ((portStatus & XHCI_PORTSC_CCS_MASK) != 0U)) {
            //host_reset_change(DrvRes, portStatus);
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "host_event_process_port_change(): no handler for port reset change");
        }

        // handle port link state change
        if ((portStatus & XHCI_PORTSC_PLC_MASK) != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "host_event_process_port_change(): no handler for port link state change");
            //dev_port_link_change(DrvRes, portStatus);
        }

        // update portStatus - in meantime new change may have happen
        portStatus = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTSC);

    } while ((portStatus & maskAllChangeBits) > 0U);
}

static void host_event_process_commad_completion(XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT32 command;
    UINT32 completionCode    = USBCDNS3_TrbCmplCodeGet(DrvRes->EventPtr);
    UINT64 DequeuePtrPhyAddr = USBCDNS3_TrbDequeueAddrGet(DrvRes->EventPtr);

    DrvRes->CommandQueue.DequeuePtr = USBCDNS3_U64AddrToTrbRingPtr(DequeuePtrPhyAddr);

    DrvRes->CommandQueue.IsRunningFlag = 0;
    DrvRes->CommandQueue.CompletePtr = DrvRes->EventPtr;
    DrvRes->CommandQueue.CompletionCode = (UINT8) completionCode;


    command = USBCDNS3_TrbTypeGet(DrvRes->CommandQueue.DequeuePtr);
    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L3, "<%d> XHCI_TRB_COMMAND_CMPL_EVENT (cmd@%p, type=0x%02x)",
                    (UINT64)DrvRes->instanceNo,
                    IO_UtilityPtrToU64Addr(DrvRes->CommandQueue.DequeuePtr),
                    (UINT64)command,
                    (UINT64)0U, (UINT64)0U);

    // check if completion is successful
    if (completionCode != XHCI_TRB_CMPL_SUCCESS) {

        static char str_buffer[255];
        static char int_buffer[255];
        str_buffer[0] = '\0';
        command = USBCDNS3_TrbTypeGet(DrvRes->CommandQueue.DequeuePtr);
        IO_UtilityStringAppend(str_buffer, 255, "<");
        if (IO_UtilityUInt32ToStr(int_buffer, 255, DrvRes->instanceNo, 10) != 0U) {
            // ignore return value
        }
        IO_UtilityStringAppend(str_buffer, 255, int_buffer);
        IO_UtilityStringAppend(str_buffer, 255, "> Command ");
        if (IO_UtilityUInt32ToStr(int_buffer, 255, command, 10) != 0U) {
            // ignore return value
        }
        IO_UtilityStringAppend(str_buffer, 255, int_buffer);
        IO_UtilityStringAppend(str_buffer, 255, "(");
        IO_UtilityStringAppend(str_buffer, 255, USBCDNS3_TrbTypeStringGet(command));
        IO_UtilityStringAppend(str_buffer, 255, "), code ");
        if (IO_UtilityUInt32ToStr(int_buffer, 255, completionCode, 10) != 0U) {
            // ignore return value
        }
        IO_UtilityStringAppend(str_buffer, 255, int_buffer);
        IO_UtilityStringAppend(str_buffer, 255, "(");
        IO_UtilityStringAppend(str_buffer, 255, USBCDNS3_TrbCmplCodeStringGet(completionCode));
        IO_UtilityStringAppend(str_buffer, 255, ")");
        USB_UtilityPrint(USB_PRINT_FLAG_L3, str_buffer);

        //USB_UtilityPrintUInt5("<%d> Command %d failed, code: %d",
        //            DrvRes->instanceNo,
        //            command,
        //            completionCode,
        //            0, 0);

    }

    if (DrvRes->CommandQueue.CompleteFunc != NULL) {
        UINT32 slot_id = USBCDNS3_TrbSlotIdGet(DrvRes->EventPtr);
        DrvRes->CommandQueue.CompleteFunc(DrvRes, slot_id, 0, completionCode, DrvRes->EventPtr, NULL, 0);
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "host_event_process_commad_completion(): no complete function!");
    }
}


static UINT8 host_event_transfer_cmpl_sts(XHC_CDN_DRV_RES_s *DrvRes, XHC_CDN_PRODUCER_QUEUE_s *TransferObj)
{

    UINT8 uret = 0U;
    UINT32 ep_index = TransferObj->ContextIndex;

    static char str_buffer[255];
    static char int_buffer[255];
    str_buffer[0] = '\0';
    if (IO_UtilityUInt32ToStr(int_buffer, 255, DrvRes->instanceNo, 10) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, "<");
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    IO_UtilityStringAppend(str_buffer, 255, "> ContextIndex: 0x");
    if (IO_UtilityUInt32ToStr(int_buffer, 255, TransferObj->ContextIndex, 16) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    IO_UtilityStringAppend(str_buffer, 255, " (EP");
    if (IO_UtilityUInt32ToStr(int_buffer, 255, (TransferObj->ContextIndex >> 1U), 10) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    if ((TransferObj->ContextIndex & 1U) != 0U) {
        IO_UtilityStringAppend(str_buffer, 255, "_IN) ");
    } else {
        IO_UtilityStringAppend(str_buffer, 255, "_OUT) ");
    }
    IO_UtilityStringAppend(str_buffer, 255, "Completion Code: ");
    if (IO_UtilityUInt32ToStr(int_buffer, 255, TransferObj->CompletionCode, 10) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    IO_UtilityStringAppend(str_buffer, 255, "(");
    IO_UtilityStringAppend(str_buffer, 255, USBCDNS3_TrbCmplCodeStringGet(TransferObj->CompletionCode));
    IO_UtilityStringAppend(str_buffer, 255, ")");
    USB_UtilityPrint(USB_PRINT_FLAG_L3, str_buffer);

    //USB_UtilityPrintUInt5("<%d> ContextIndex: 0x%02X (EP%d_%d)  Completion code: %d",
    //                    DrvRes->instanceNo,
    //                    transferObj->ContextIndex,
    //                    (transferObj->ContextIndex >> 1U),
    //                    (transferObj->ContextIndex & 1U),
    //                     transferObj->CompletionCode);

    // handle different completion codes
    switch (TransferObj->CompletionCode) {

            // for success
        case XHCI_TRB_CMPL_SUCCESS:
            uret = 1U;
            break;

            // when endpoint stalled
        case XHCI_TRB_CMPL_STALL_ERROR:

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_event_transfer_cmpl_sts(): EP %d stalled", ep_index, 0, 0, 0, 0);

            if (ep_index > 1U) {
                // get transfer object, it may be default endpoint, endpoint or stream object
                if (TransferObj->CompleteFunc != NULL) {
                   TransferObj->CompleteFunc(DrvRes, DrvRes->ActualDeviceSlot, ep_index, XHCI_TRB_CMPL_STALL_ERROR, TransferObj->DequeuePtr, NULL, 0U);
                }
            }

            //(void) USBSSP_ResetEndpoint(DrvRes, ep_index);
            //TransferObj->IsDisabledFlag = 1U;
            break;

            // when short packet received
        case XHCI_TRB_CMPL_SHORT_PKT:
            if (TransferObj->IgnoreShortPacket == 0U) {
                uret = 1U;
            } else {
                uret = 0U;
                if (USBCDNS3_TrbChainBitGet(TransferObj->DequeuePtr) == 0U) {
                    TransferObj->IgnoreShortPacket = 0;
                }
            }

            if (USBCDNS3_TrbChainBitGet(TransferObj->DequeuePtr) > 0U) {
                TransferObj->IgnoreShortPacket = 1U;
            }
            break;

            // when missed service error
        case XHCI_TRB_CMPL_MISSED_SRV_ER:
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> MISSED_SERVICE_ERROR Endpoint(%d)",
                                DrvRes->instanceNo, ep_index, 0, 0, 0);
            // since we always set IOC for the last TRB of the td, only call completion for the TRB with IOC
            if (USBCDNS3_TrbChainBitGet(TransferObj->DequeuePtr) == 0U) {
                uret = 1U;
            }
            break;

        case XHCI_TRB_CMPL_RING_UNDERRUN:
        case XHCI_TRB_CMPL_RING_OVERRUN:
        case XHCI_TRB_CMPL_NO_PNG_RSP_ER:
            uret = 1U;
            break;
        default:
            // do nothing by default
            break;
    }
    return uret;
}

static void host_event_transfer_cmpl_cbk(XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s * const EventPtr) {

    UINT8  ep_index    = USBCDNS3_TrbEndpointIdGet(EventPtr);
    UINT8  slot_id     = USBCDNS3_TrbSlotIdGet(EventPtr);

    // get transfer object, it may be default endpoint, endpoint or stream object
    XHC_CDN_PRODUCER_QUEUE_s * transfer_obj = USBCDNS3_TransferObjectGet(DrvRes);

    if (transfer_obj->CompleteFunc != NULL) {
        transfer_obj->CompleteFunc(DrvRes, slot_id, ep_index, 0, EventPtr,
                (UINT8 *)USBCDNS3_U64AddrToTrbRingPtr(transfer_obj->LastXferBufferPhyAddr),
                transfer_obj->LastXferActualLength);
    } else {
        UINT32 trb_type;
        trb_type = USBCDNS3_EpCompletionHandler(DrvRes, transfer_obj);
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_event_transfer_cmpl_cbk(): <%d> No callback defined for trb_type %d", DrvRes->instanceNo, trb_type, 0, 0, 0);
   }
}


static void host_event_transfer(XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s * const EventPtr) {

    UINT8 callback_flag;

    // get transfer object, it may be default endpoint, endpoint or stream object
    XHC_CDN_PRODUCER_QUEUE_s *transferObj = USBCDNS3_TransferObjectGet(DrvRes);

    // check transfer status
    // handle stall etc.
    callback_flag = host_event_transfer_cmpl_sts(DrvRes, transferObj);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "host_event_transfer(): callback_flag = %d", callback_flag, 0, 0, 0, 0);

    if (callback_flag == 1U) {
        host_event_transfer_cmpl_cbk(DrvRes, EventPtr);
    }
}


static void host_event_process_one(XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s * const EventPtr, UINT32 TrbType) {

    // check event type
    switch (TrbType) {
        case XHCI_TRB_PORT_ST_CHG_EVT:
            // handle events on port
            host_event_process_port_change(DrvRes);
            break;

            // handle command completion
        case XHCI_TRB_CMD_CMPL_EVT:
            host_event_process_commad_completion(DrvRes);
            break;

            // handle transfer event
        case XHCI_TRB_TRANSFER_EVENT:
            host_event_transfer(DrvRes, EventPtr);
            break;
            // handle host controller event
        case XHCI_TRB_HOST_CTRL_EVT:
            // TBD
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_event_process_one(): <%d> Host Controller ERROR:", DrvRes->instanceNo, 0, 0, 0, 0);
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "    Completion Code: %d", DrvRes->EventPtr->DWord2 >> XHCI_TRB_CMPL_CODE_POS, 0, 0, 0, 0);
            break;

            // not ready notification
        case XHCI_TRB_NRDY_EVT:
            // TBD
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "host_event_process_one(): <%d> NRDY on endpoint index %d", DrvRes->instanceNo, USBCDNS3_TrbEndpointIdGet(DrvRes->EventPtr), 0, 0, 0);
            break;

        default:
            // TBD
            USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_ERROR, "host_event_process_one(): <%d> unknown TRB type %d, Ptr 0x%08X%08X",
                                     (UINT64)DrvRes->instanceNo,
                                     (UINT64)TrbType,
                                     IO_UtilityPtrToU64Addr(EventPtr),
                                     (UINT64)0U,
                                     (UINT64)0U);
            break;
    }
    return;
}

static void host_event_process_all(XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT32 trb_type;

    do {

        USBCDNS3_TrbInfoDisplay(DrvRes->EventPtr, "EVENT.");

        trb_type = USBCDNS3_TrbTypeGet(DrvRes->EventPtr);
        host_event_process_one(DrvRes, DrvRes->EventPtr, trb_type);

        USBCDNS3_EventPtrUpdate(DrvRes);

        // check if any pending event still on event ring and handle it if yes
    } while (USBCDNS3_TrbToggleBitGet(DrvRes->EventPtr) == DrvRes->EventToogleBit);
}


/**
 * Process ISR event and update event read pointer
 * @param DrvRes [IN] driver resources
 */
static UINT32 host_event_process(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT8 retFunction = 0;
    UINT32 uret = 0;

    // invalidate event data
    USB_UtilityCacheInvdUInt8((UINT8 *)DrvRes->EventPtr, (UINT32)sizeof(XHCI_RING_ELEMENT_s));

    // check babble interrupt
    if (USBCDNS3_TrbToggleBitGet(DrvRes->EventPtr) != DrvRes->EventToogleBit) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "host_event_process(): Babble interrupt: Host %d != Drv %d", USBCDNS3_TrbToggleBitGet(DrvRes->EventPtr), DrvRes->EventToogleBit, 0, 0, 0);
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_ERROR, "DrvRes->EventPtr        = 0x%X", IO_UtilityPtrToU64Addr(DrvRes->EventPtr));
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_ERROR, "DrvRes->epRingPoolStart = 0x%X", IO_UtilityPtrToU64Addr(DrvRes->XhciMemResources->EventPool));
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_ERROR, "DrvRes->epRingPoolEnd   = 0x%X", IO_UtilityPtrToU64Addr(&DrvRes->XhciMemResources->EventPool[AMBA_XHCI_EVENT_QUEUE_SIZE - 1U]));
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "sizeof EventRingPool %d, size of XHCI_RING_ELEMENT_s %d", (UINT32)sizeof(DrvRes->XhciMemResources->EventPool), (UINT32)sizeof(XHCI_RING_ELEMENT_s), 0, 0, 0);
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ERDP = 0x%X", IO_UtilityRegRead32(0x2020005038UL), 0, 0, 0, 0);
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "host_event_process(): Assertion!");
        USB_UtilityAssert();
        retFunction = 1;
        uret = USB_ERR_XHCI_EVT_BABBLE;
    }

    if (retFunction == 0U) {
        if (USBCDNS3_EventTrbCheck(DrvRes, DrvRes->EventPtr) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "host_event_process(): trb event check failed", 0, 0, 0, 0, 0);
            retFunction = 1;
            USBCDNS3_EventPtrUpdate(DrvRes);
        }
    }
    if (retFunction == 0U) {
        host_event_process_all(DrvRes);
    }
    return uret;
}


/* ISR handler for Host mode */
UINT32 USBHCDNS3_Isr(void)
{
    XHC_CDN_DRV_RES_s *cdn_drv_res = USBCDNS3_DrvInstanceGet();
    UINT32 uret = USBSSP_IsrSF(cdn_drv_res);

    if (uret == 0U) {

        USBCDNS3_XhcUsbStsEventClear(cdn_drv_res);

        // Enable the Interrupter by writing a '1' to the Interrupt Pending (IP)
        USBCDNS3_Write32(&cdn_drv_res->Registers.xhciInterrupter[0].IMAN, XHCI_IMAN0_IE_MASK | XHCI_IMAN0_IP_MASK);

        uret = host_event_process(cdn_drv_res);

        USBCDNS3_Write64(&cdn_drv_res->Registers.xhciInterrupter[0].ERDP,
                        USBCDNS3_PtrToU64PhyAddr(&cdn_drv_res->EventPtr->DWord0) | XHCI_ERDP0_LO_EHB_MASK);

    }

    return uret;

}



