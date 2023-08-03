/**
 *  @file USBCDNS3_DeviceIsrHandler.c
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
 *  @details USB ISR handler for Cadence USB device controller.
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

static void dev_ep0_std_req_preproc_set_addr(XHC_CDN_DRV_RES_s *DrvRes, UINT8 DeviceAddress)
{
    // when the device receives a set_address control packet:
    //   1. send ADDRESS_DEVICE command to controller
    //   2. At the completion of the ADDRESS_DEVICE command:
    //      2.1 sends STATUS_STAGE packet to controller
    //      2.2 controller completes set_address control packet

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
        "dev_ep0_std_req_preproc_set_addr(): <%d> address = %d",
        DrvRes->instanceNo,
        DeviceAddress, 0, 0, 0);

    DrvRes->DevAddress = DeviceAddress;

    if ((DrvRes->ActualDeviceSlot > 0U) && (DrvRes->EnableSlotPending == 0U)) {
        // send command to SSP controller
        USBCDNS3_XhcCmdSetAddress(DrvRes, 0U);
    } else if (DrvRes->ActualDeviceSlot == 0U) {

        // For some reasons like disconnect/reset we are unable to issue
        // set address command to SSP controller now
        DrvRes->Ep0State = USBCDNS3_EP0_UNCONNECTED;
        DrvRes->Ep0Queue.IsRunningFlag = 1U;
    } else {
        // required by MISRA
    }
}

static void dev_ep0_std_req_preproc_set_config(XHC_CDN_DRV_RES_s *DrvRes)
{
    // when the device receives a set_config control packet:
    //   1. setup input context for endpoint 0
    //   2. call upper layer to create corresponding endpoints
    //      2.1 when creating endpoints, the corresponding input context for the endpoint will be configured.
    //   3. sends CONFIGURE_ENDPOINT command to controller
    //   4. At the completion of the CONFIGURE_ENDPOINT command:
    //      4.1 sends STATUS_STAGE packet to controller
    //      4.2 controller completes set_address control packet

    // Before CONFIGURE_ENDPOINT command is issued - Dx/Ax flags
    // (within Input Context) should be cleared (all except A0, that
    // need to be set); they are set individually by CUSBD_EpEnable / CUSBD_EpDisable
    DrvRes->InputContext->InputControlContext[0] = 0; // Dx = 0
    DrvRes->InputContext->InputControlContext[1] = 1; // A0 = 1, rest Ax = 0

    // make software clone of input context
    USB_UtilityMemoryCopy(&DrvRes->InputContextCopy, DrvRes->InputContext, sizeof (XHCI_INPUT_CONTEXT_s));
}

static UINT32 dev_ep0_usbcv_check(const USB_CH9_SETUP_s *Setup)
{
    UINT32 uret = 0;
    if ((Setup->bmRequestType == 0x80U) &&
        (Setup->bRequest == USB_REQ_GET_DESCRIPTOR) &&
        (Setup->wValue == 0xFE00U) &&
        (Setup->wIndex == 0U) &&
        (Setup->wLength== 0x0012U)) {

        // USB CV3 TD 9.10 Bad Descriptor Test
        // USBX stack can not handle it well.....
        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "dev_ep0_usbcv_check(): USB CV3 TD 9.10 Bad Descriptor Test");

        uret = 1;
    }

    if ((Setup->bmRequestType == 0x00U) &&
        (Setup->wIndex == 0U) &&
        (Setup->wLength== 0x0U)) {

        if ((Setup->bRequest == USB_REQ_CLEAR_FEATURE) || (Setup->bRequest == USB_REQ_SET_FEATURE)) {

            // USB CV3 TD 9.11 Bad Feature Test
            UINT16 feature_selector = Setup->wValue;
            if ((feature_selector > 6U) && (feature_selector < 48U)) {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "dev_ep0_usbcv_check(): USB CV3 TD 9.11 Bad Feature Test");
                uret = 1;
            } else if (feature_selector > 53U) {
                USB_UtilityPrint(USB_PRINT_FLAG_INFO, "dev_ep0_usbcv_check(): USB CV3 TD 9.11 Bad Feature Test");
                uret = 1;
            } else {
                // pass misra-c check
            }
        }
    }

    return uret;
}

static void dev_ep0_std_req_preproc(XHC_CDN_DRV_RES_s *DrvRes, const USB_CH9_SETUP_s * const Setup)
{
    if (Setup->bRequest == USB_REQ_SET_ADDRESS) {
        // handle SET ADDRESS request
        dev_ep0_std_req_preproc_set_addr(DrvRes, (UINT8) Setup->wValue);
    } else if (Setup->bRequest == USB_REQ_SET_CONFIGURATION) {
        // handle set configuration before calling callback
        dev_ep0_std_req_preproc_set_config(DrvRes);
    } else if (Setup->bRequest == USB_REQ_CLEAR_FEATURE) {
        if (Setup->wValue == CH9_USB_FS_U1_ENABLE) {
            USBCDNS3_XhcU1TimeoutSet(DrvRes, 0U);
        } else if (Setup->wValue == CH9_USB_FS_U2_ENABLE) {
            USBCDNS3_XhcU2TimeoutSet(DrvRes, 0U);
        } else {
            // required by MISRA
        }
    }  else if (Setup->bRequest == USB_REQ_SET_FEATURE) {
        if (Setup->wValue == CH9_USB_FS_U1_ENABLE) {
            USBCDNS3_XhcU1TimeoutSet(DrvRes, 1U);
        } else if (Setup->wValue == CH9_USB_FS_U2_ENABLE) {
            USBCDNS3_XhcU2TimeoutSet(DrvRes, 1U);
        } else {
            // required by MISRA
        }
    } else {
        // required by MISRA
    }
}

static UINT32 dev_ep0_setup_packet_impl(XHC_CDN_DRV_RES_s *DrvRes, const USB_CH9_SETUP_s *setup)
{

    UINT32 uret = 0;

    if (((setup->bmRequestType & CH9_USB_REQ_TYPE_MASK) == CH9_USB_REQ_TYPE_STANDARD) &&
        ((setup->bmRequestType & CH9_REQ_RECIPIENT_MASK) == CH9_USB_REQ_RECIPIENT_DEVICE)) {
        dev_ep0_std_req_preproc(DrvRes, setup);
    }

    if (((setup->bmRequestType & 0x80U) == CH9_USB_DIR_HOST_TO_DEVICE) && (setup->wLength > 0U)) {

        const UDC_ENDPOINT_INFO_s  *udc_ep_info   = USBDCDNS3_EndpointInfoGet(XHCI_EP0_CONTEXT_OFFSET);
        const UX_SLAVE_ENDPOINT    *usbx_endpoint = udc_ep_info->UxEndpoint;
        const UINT8                *data_ptr;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_ep0_setup_packet_impl(): <%d> setup->bRequest %d: to receive data...Length %d", DrvRes->instanceNo, setup->bRequest, setup->wLength, 0, 0);

        // we need to receive data from Host
        // since we are in ISR task, need to wait until rx complete and then call upper layer and check result

        // save current setup packet
        DrvRes->DevCtrlSetupDataPending = *setup;

        data_ptr = usbx_endpoint->ux_slave_endpoint_transfer_request.ux_slave_transfer_request_data_pointer;
        USB_UtilityCacheInvdUInt8(data_ptr, (UINT32)usbx_endpoint->ux_slave_endpoint_transfer_request.max_buffer_size);

        uret = USBDCDNS3_ControlTransfer(DrvRes,
                                         data_ptr,
                                         setup->wLength,
                                         0);
        if (uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_ep0_setup_packet_impl(): <%d> ERROR: ailed to setup transfer, code 0x%X", DrvRes->instanceNo, uret, 0, 0, 0);
        }
        DrvRes->DevCtrlRxPending = 1;
    } else {
        // send request to upper layer and check result
        if (DrvRes->DeviceCallbacks.Setup != NULL) {
            uret = dev_ep0_usbcv_check(setup);
            if (uret == 0U) {
                uret = DrvRes->DeviceCallbacks.Setup(USBCDNS3_UdcInstanceGet(), setup);
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_ep0_setup_packet_impl(): <%d> ERROR: No callback defined for CUSBD module !", DrvRes->instanceNo, 0, 0, 0, 0);
            uret = USB_ERR_XHCI_NO_CALLBACK;
        }

        // post user application handler
        if (uret == 0U) {

            switch (setup->bRequest) {

                case USB_REQ_SET_ADDRESS:
                    break;

                case USB_REQ_SET_CONFIGURATION:
                {
                    UINT32 addMask;
                    UINT32 dropMask;

                    USBCDNS3_XHCInputContexDisplay(DrvRes->InputContext);

                    // we configure endpoints in post handler only when higher
                    // layer returned CDN_EOK for current setup
                    USBCDNS3_XhcCmdConfigEndpoint(DrvRes);

                    addMask = DrvRes->InputContext->InputControlContext[1];
                    dropMask = DrvRes->InputContext->InputControlContext[0];

                    DrvRes->EnabledEndpsMask |= addMask;
                    DrvRes->EnabledEndpsMask &= ~dropMask;
                    DrvRes->EnabledEndpsMask &= 0xFFFFFFFCU; // Slot context and EP0 - not considered

                    DrvRes->FlagDevConfigured = 1;

                    break;
                }

                default:
                    if (setup->bRequest == USB_REQ_SET_INTERFACE) {
                        UINT32 addMask;
                        UINT32 dropMask;

                        USBCDNS3_XHCInputContexDisplay(DrvRes->InputContext);

                        // we configure endpoints in post handler only when higher
                        // layer returned CDN_EOK for current setup
                        USBCDNS3_XhcCmdConfigEndpoint(DrvRes);

                        addMask = DrvRes->InputContext->InputControlContext[1];
                        dropMask = DrvRes->InputContext->InputControlContext[0];

                        DrvRes->EnabledEndpsMask |= addMask;
                        DrvRes->EnabledEndpsMask &= ~dropMask;
                        DrvRes->EnabledEndpsMask &= 0xFFFFFFFCU; // Slot context and EP0 - not considered

                    }

                    if (setup->wLength == 0U) {
                        // send status stage for requests without data stage
                        uret = USBDCDNS3_ControlTransfer(DrvRes, NULL, 0, 0);
                        if (uret != 0U) {
                            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_ep0_setup_packet_impl(): <%d> Status Stage ERROR", DrvRes->instanceNo, 0, 0, 0, 0);
                        }
                    }
                    break;
            }
        }
    }
    return (uret);
}


static void dev_ep0_setup_packet(XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT32 uret;
    const USB_CH9_SETUP_s *setup = &DrvRes->DevSetupReq;

    DrvRes->Ep0State = USBCDNS3_EP0_SETUP_PHASE;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
                          "bmRequestType: 0x%02X, bRequest: 0x%02X, wValue: 0x%04X, wIndex: 0x%04X, wLength: 0x%04X",
                            setup->bmRequestType,
                            setup->bRequest,
                            setup->wValue,
                            setup->wIndex,
                            setup->wLength);

    uret = dev_ep0_setup_packet_impl(DrvRes, setup);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_ep0_setup_packet(): <%d> uret = %d, wLength = %d", DrvRes->instanceNo, uret, setup->wLength, 0, 0);

    // set stall if request is not supported
    if (uret != 0U) {
        if (setup->wLength != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_ep0_setup_packet(): <%d> Halt control endpoint at data stage", DrvRes->instanceNo, 0, 0, 0, 0);

            // STALL EP0 if control transfer has data phase
            (void) USBCDNS3_EndpointFeatureSet(DrvRes, 1, 1);
            DrvRes->Ep0State = USBCDNS3_EP0_HALT_PENDING;
        } else {
            // send STALL response in status phase
            USBDCDNS3_XhcCmdCtrlStatusStage(DrvRes, 0U);
            USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, DrvRes->Ep0Queue.ContextIndex);
            DrvRes->Ep0Queue.IsRunningFlag = 1;
        }
    }
}

/**
 * Completion handler for Enable Slot command
 * @param res driver resources
 */
static void dev_cmd_cmpl_enable_slot(XHC_CDN_DRV_RES_s * DrvRes)
{
    UINT32 func_uret;
    // here we received command completion for EnableSlot command

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> CMPL USBSSP_TRB_ENABLE_SLOT_COMMAND", DrvRes->instanceNo, 0, 0, 0, 0);

    // set actual device slot id
    DrvRes->ActualDeviceSlot = USBCDNS3_TrbSlotIdGet(DrvRes->EventPtr);
    if (DrvRes->ActualDeviceSlot > DrvRes->MaxDeviceSlot) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "dev_cmd_cmpl_enable_slot(): <%d> ActualDeviceSlot (%d) greater than max slots (%d)",
                              DrvRes->instanceNo, DrvRes->ActualDeviceSlot, DrvRes->MaxDeviceSlot, 0, 0);
    }

    DrvRes->EnableSlotPending = 0U;
    DrvRes->ContextEntries = 1; // A0 and A1 enabled

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_enable_slot(): <%d> Actual slotID: %d, running_flag = %d", DrvRes->instanceNo, DrvRes->ActualDeviceSlot, DrvRes->Ep0Queue.IsRunningFlag, 0, 0);

    // check if SET_ADDRESS setup request already handled
    if (DrvRes->Ep0Queue.IsRunningFlag == 1U) {
        USBCDNS3_XhcCmdSetAddress(DrvRes, 0U);
    } else {
        USBCDNS3_XhcCmdSetAddress(DrvRes, 1U);
    }

    func_uret = USBDCDNS3_DrvInitComplete();

    if (func_uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                             "dev_cmd_cmpl_enable_slot(): <%d> failed to complete driver, code, 0x%X, Actual slotID: %d, running_flag = %d",
                             DrvRes->instanceNo, func_uret, DrvRes->ActualDeviceSlot, DrvRes->Ep0Queue.IsRunningFlag, 0);
    }
}


/**
 * Handle reset endpoint command completion
 * @param res driver resources
 */
static void dev_cmd_cmpl_reset_endpoint0(XHC_CDN_DRV_RES_s *DrvRes)
{

    /* in device mode we wouldn't queue in TRBs after Stall */
    /* handle setup request */
    if (DrvRes->Ep0State == USBCDNS3_EP0_SETUP_PENDING) {
        USB_UtilityPrint(USB_PRINT_FLAG_L1, "dev_cmd_cmpl_reset_endpoint0(): call dev_ep0_setup_packet()");
        dev_ep0_setup_packet(DrvRes);
    }
}


/**
 * Handle reset endpoint command completion
 * @param res driver resources
 */
static void dev_cmd_cmpl_reset_endpoint(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT8  epIndex = USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr);
    UINT32 endpointState = USBCDNS3_EpStatusGet(DrvRes, epIndex);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "dev_cmd_cmpl_reset_endpoint(): <%d> ep %d, status %d", DrvRes->instanceNo, epIndex, endpointState, 0, 0);

    // set dequeue pointer command allowed only for stopped endpoint
    if (endpointState == XHCI_EPCTX_STATE_STOPPED) {

        // handle default endpoint
        if (epIndex == XHCI_EP0_CONTEXT_OFFSET) {
            dev_cmd_cmpl_reset_endpoint0(DrvRes);
        } else {
            // handle no default endpoint
            // get endpoint object
            const XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[epIndex];
            if (ep->IsDisabledFlag > 0U) {
                UINT32 result = USBCDNS3_EndpointFeatureSet(DrvRes, epIndex, 0);
                if (result != 0U) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_cmd_cmpl_reset_endpoint(): <%d> Clear stall on endpoint %d failed!\n", DrvRes->instanceNo, epIndex, 0, 0, 0);
                }
            } else if (DrvRes->DeviceModeFlag != USBCDNS3_MODE_HOST) {
                // ring doorbell to transition to Running state
                USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, epIndex);
            } else {
            /*
             * All 'if ... else if' constructs shall be terminated with an 'else' statement
             * (MISRA2012-RULE-15_7-3)
             */
            }
        }
    }
}

/**
 * Handle reset device command completion
 * @param res driver resources
 */
static void dev_cmd_cmpl_reset_device(XHC_CDN_DRV_RES_s *DrvRes) {

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_reset_device(): <%d> Device address = %d.", DrvRes->instanceNo, DrvRes->DevAddress, 0, 0, 0);

    if ((DrvRes->DevAddress > 0U) && (DrvRes->ActualSpeed > CH9_USB_SPEED_HIGH)) {
        USBCDNS3_XhcCmdSetAddress(DrvRes, 0U);
    }
}

/**
 * handleDisableSlot
 * @param res driver resources
 */
// parasoft-begin-suppress MISRA2012-RULE-2_7-4 "Parameter res not used in function handleDisableSlot", DRV-5631
/* parasoft-begin-suppress MISRA2012-RULE-8_13_a "Pass parameter res with const specifier, DRV-3806" */
static void dev_cmd_cmpl_disalbe_slot(XHC_CDN_DRV_RES_s * DrvRes)
{
    AmbaMisra_TouchUnused(DrvRes);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_disalbe_slot(): <%d>", DrvRes->instanceNo, 0, 0, 0, 0);
    return;
}

/**
 * handleForceHeader
 * @param res  driver resources
 */
static void dev_cmd_cmpl_force_header(XHC_CDN_DRV_RES_s *DrvRes) {

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_force_header(): <%d>", DrvRes->instanceNo, 0, 0, 0, 0);

    if (DrvRes->forceHeaderComplete != NULL) {
        DrvRes->forceHeaderComplete(DrvRes);
    }
}

/**
 * handleHaltEndpoint
 * @param res driver resources
 */
static void dev_cmd_cmpl_halt_endpoint(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT8 epIndex = USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_halt_endpoint(): <%d> ep = %d", DrvRes->instanceNo, USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr), 0, 0, 0);

    // device mode only
    if (epIndex == 1U) {
        if (DrvRes->Ep0State == USBCDNS3_EP0_HALT_PENDING) {
            USB_UtilityPrint(USB_PRINT_FLAG_L1, "set ep0 state to USBSSP_EP0_HALTED");
            DrvRes->Ep0State = USBCDNS3_EP0_HALTED;
            // stay in HALT state till a new setup request arrives.
        } else if ( DrvRes->Ep0State == USBCDNS3_EP0_HALT_SETUP_PENDING) {
            // enqueue reset ep0 command - before handling setup request
            USBCDNS3_XhcCmdResetEndpoint(DrvRes, 1U);
            USBCDNS3_HostCommandDoorbell(DrvRes);
            DrvRes->Ep0State = USBCDNS3_EP0_SETUP_PENDING;

            USB_UtilityPrint(USB_PRINT_FLAG_L1, "set ep0 state to USBSSP_EP0_SETUP_PENDING");

        } else {
            // required by MISRA
        }
    }
    return;
}

/**
 * handleHaltStopEndpoint
 * @param res driver resources
 */
static void dev_cmd_cmpl_stop_endpoint(XHC_CDN_DRV_RES_s * DrvRes)
{

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_stop_endpoint(): <%d> ep = %d", DrvRes->instanceNo, USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr), 0, 0, 0);

    if ((DrvRes->FlagDevCommandWait != 0U) && (DrvRes->DevCommandToWait == XHCI_TRB_STOP_EP_CMD)) {
        // notify command done
        if (USB_UtilitySemaphoreGive(&DrvRes->SemaphoreCommandQueue) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_cmd_cmpl_stop_endpoint(): <%d> ep = %d, fail to give semaphore!", DrvRes->instanceNo, USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr), 0, 0, 0);
        }
    }

    return;
}

/**
 * handleForceEvent
 * @param res driver resources
 */
static void dev_cmd_cmpl_force_event(XHC_CDN_DRV_RES_s * DrvRes)
{
    AmbaMisra_TouchUnused(DrvRes);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_force_event(): <%d>", DrvRes->instanceNo, 0, 0, 0, 0);
    return;
}

/**
 * Handle set transfer dequeue pointer command completion
 * @param res driver resources
 */
static void dev_cmd_cmpl_set_tr_dequeue_pointer(XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT8  epIndex = USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr);
    UINT32 endpointState = USBCDNS3_EpStatusGet(DrvRes, epIndex);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "dev_cmd_cmpl_set_tr_dequeue_pointer(): <%d> ep = %d",
                            DrvRes->instanceNo,
                            epIndex,
                            0, 0, 0);

    if (epIndex == XHCI_EP0_CONTEXT_OFFSET) {
        //res->Ep0Queue.DequeuePtr = res->Ep0Queue.EnqueuePtr;
        USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, epIndex);
    } else {
        if (epIndex > XHCI_EP0_CONTEXT_OFFSET) {
            //res->EpQueueArray[epIndex].DequeuePtr = res->EpQueueArray[epIndex].EnqueuePtr;
            if (endpointState == XHCI_EPCTX_STATE_STOPPED) {

                if (DrvRes->EpQueueArray[epIndex].IsRunningFlag != 0U) {
                    // ring doorbell to put endpoint in running state
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "<%d> DRBL: Ring doorbell on ep_index: %d\n",
                                            DrvRes->instanceNo, epIndex, 0, 0, 0);
                    USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, epIndex);
                }
            }

            // notify command done
            if ((DrvRes->FlagDevCommandWait != 0U) && (DrvRes->DevCommandToWait == XHCI_TRB_SET_TR_DQ_PTR_CMD)) {
                // notify command done
                if (USB_UtilitySemaphoreGive(&DrvRes->SemaphoreCommandQueue) != 0U) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_cmd_cmpl_set_tr_dequeue_pointer(): <%d> ep = %d, fail to give semaphore!", DrvRes->instanceNo, epIndex, 0, 0, 0);
                }
            }
        }
    }
}

/**
 * Handle address device command completion
 * @param res driver resources
 */
static void dev_cmd_cmpl_address_device(XHC_CDN_DRV_RES_s * DrvRes)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_address_device(): <%d> Device in addressed state, Addr = 0x%X", DrvRes->instanceNo, DrvRes->DevAddress, 0, 0, 0);

    if (DrvRes->DevAddress > 0U) {
        // we have received a setup packet for set_address before
        // and ADDRESS_DEVICE command completed, we need to send control-status back to host.
        (void) USBDCDNS3_ControlTransfer(DrvRes, NULL, 0, 0);
    }
}

/**
 * Handle configure endpoint command completion
 * @param res driver resources
 */
static void dev_cmd_cmpl_configure_endpoint(XHC_CDN_DRV_RES_s *DrvRes)
{

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_cmd_cmpl_configure_endpoint(): <%d>", DrvRes->instanceNo, 0, 0, 0 ,0);

    USBCDNS3_DeviceConfigFlagSet(DrvRes, 1);

    if (DrvRes->ActualSpeed > CH9_USB_SPEED_HIGH) {
        // enable U1 and U2 for ss and ssp speed
        USBCDNS3_XhcU1TimeoutSet(DrvRes, 1);
        USBCDNS3_XhcU2TimeoutSet(DrvRes, 1);
    } else {
        UINT32 usb2_portpmsc_val = XHCI_PORTPMSC1USB2_HLE_MASK;

         /* define CDNSP_DEFAULT_BESL 5 */
         usb2_portpmsc_val = USBCDNS3_U32BitsWrite(XHCI_PORTPMSC1USB2_BESL_MASK, XHCI_PORTPMSC1USB2_BESL_SHIFT, usb2_portpmsc_val, 1U);
         usb2_portpmsc_val = USBCDNS3_U32BitsWrite(XHCI_PORTPMSC1USB2_L1S_MASK, XHCI_PORTPMSC1USB2_L1S_SHIFT, usb2_portpmsc_val, 2U);
        // configure USB2 LPM
        USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[USBCDNS3_DEV_MODE_20_PORT_IDX].PORTPMSC, usb2_portpmsc_val);
    }

    // we should have received a setup packet for set configuration
    // need to send status stage for this control request
    (void) USBDCDNS3_ControlTransfer(DrvRes, NULL, 0, 0);
}

/**
 * handleNoOp
 * @param res  driver resources
 */
static void dev_cmd_cmpl_no_op(XHC_CDN_DRV_RES_s * DrvRes)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "dev_cmd_cmpl_no_op(): <%d>", DrvRes->instanceNo, 0, 0, 0, 0);

    if (DrvRes->nopComplete != NULL) {
        DrvRes->nopComplete(DrvRes);
    }
}


/**
 * handleDefault
 * @param res driver resources
 */
static void dev_cmd_cmpl_default(XHC_CDN_DRV_RES_s * DrvRes)
{
    AmbaMisra_TouchUnused(DrvRes);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_cmd_cmpl_default(): <%d> Unknown/not supported cmd...",
                    DrvRes->instanceNo,
                    0, 0, 0, 0);

    return;
}

/**
 * Checks for the presence of set in array and returns the index
 * @param set
 * @param function array
 * @return
 */
static UINT32 remap(UINT32 command, UINT32 const *array, UINT32 size) {
    UINT32 i, res = 0;
    for (i = 0; i < size; i++) {
        if (array[i] == command) {
            res = i;
            break;
        } else {
            //Returns the size(default case)
            res = size;
        }

    }
    return res;
}

typedef void (*CMD_CMPL_FUNC_f)(XHC_CDN_DRV_RES_s *DrvRes);


/**
 * Handle successfully completed command
 * @param res driver resources
 * @param command command code
 */
static void dev_command_completion_success(XHC_CDN_DRV_RES_s * DrvRes, UINT32 Command) {
    static const UINT32 commands[] = {
        XHCI_TRB_ENABLE_SLOT_COMMAND,
        XHCI_TRB_ADDR_DEV_CMD,
        XHCI_TRB_CONF_EP_CMD,
        XHCI_TRB_NO_OP_COMMAND,
        XHCI_TRB_RESET_EP_CMD,
        XHCI_TRB_SET_TR_DQ_PTR_CMD,
        XHCI_TRB_RESET_DEVICE_COMMAND,
        XHCI_TRB_DISABLE_SLOT_COMMAND,
        XHCI_TRB_FORCE_HEADER_COMMAND,
        XHCI_TRB_HALT_ENDP_CMD,
        XHCI_TRB_STOP_EP_CMD,
        XHCI_TRB_FORCE_EVENT_COMMAND
    };
    UINT32 ch = remap(Command, commands, (UINT32) (sizeof (commands) / sizeof (UINT32)));
    static const CMD_CMPL_FUNC_f function_ptr[] = {
        dev_cmd_cmpl_enable_slot,
        dev_cmd_cmpl_address_device,
        dev_cmd_cmpl_configure_endpoint,
        dev_cmd_cmpl_no_op,
        dev_cmd_cmpl_reset_endpoint,
        dev_cmd_cmpl_set_tr_dequeue_pointer,
        dev_cmd_cmpl_reset_device,
        dev_cmd_cmpl_disalbe_slot,
        dev_cmd_cmpl_force_header,
        dev_cmd_cmpl_halt_endpoint, // device mode only (HALT_ENDPOINT)
        dev_cmd_cmpl_stop_endpoint,
        dev_cmd_cmpl_force_event,
        dev_cmd_cmpl_default
    };

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "dev_command_completion_success(): %d",
                        Command,
                        0, 0, 0, 0);

    function_ptr[ch](DrvRes);
}


/**
 * Function handles command completion
 * @param res driver resources
 */
static void dev_event_process_command_completion(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32 command;
    UINT32 completionCode    = USBCDNS3_TrbCmplCodeGet(DrvRes->EventPtr);
    UINT64 DequeuePtrPhyAddr = USBCDNS3_TrbDequeueAddrGet(DrvRes->EventPtr);

    DrvRes->CommandQueue.DequeuePtr = USBCDNS3_U64AddrToTrbRingPtr(DequeuePtrPhyAddr);

    command = USBCDNS3_TrbTypeGet(DrvRes->CommandQueue.DequeuePtr);
    DrvRes->CommandQueue.IsRunningFlag = 0;
    DrvRes->CommandQueue.CompletePtr = DrvRes->EventPtr;
    DrvRes->CommandQueue.CompletionCode = (UINT8) completionCode;

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L2, "<%d> XHCI_TRB_COMMAND_CMPL_EVENT (cmd@%p, type=0x%02x)",
                    (UINT64)DrvRes->instanceNo,
                    IO_UtilityPtrToU64Addr(DrvRes->CommandQueue.DequeuePtr),
                    (UINT64)command,
                    (UINT64)0, (UINT64)0);

    // check if completion is successful
    if (completionCode == XHCI_TRB_CMPL_SUCCESS) {
        dev_command_completion_success(DrvRes, command);
    } else {
        static char str_buffer[255];
        static char int_buffer[255];
        str_buffer[0] = '\0';
        IO_UtilityStringAppend(str_buffer, 255, "dev_event_process_command_completion(): <");
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
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, str_buffer);

        //USB_UtilityPrintUInt5("<%d> Command %d failed, code: %d",
        //            DrvRes->instanceNo,
        //            command,
        //            completionCode,
        //            0, 0);

    }
}

/**
 * handle connect status change for connect case
 * @param res driver resources
 * @param portStatus port status
 */
static void dev_connect_status_change_connect(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortStatus) {

    UINT32 uret;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_connect_status_change_connect(): <%d> Device Connected on port: %d",
                            DrvRes->instanceNo,
                            DrvRes->ActualPort,
                            0, 0, 0);

    DrvRes->Connected = 1U;

    // call connect callback of cusbd module
    if (DrvRes->DeviceCallbacks.Connect != NULL) {
        DrvRes->DeviceCallbacks.Connect(DrvRes);
    }

    // check if port enabled
    if ((PortStatus & XHCI_PORTSC_PED_MASK) > 0U) {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_connect_status_change_connect(): <%d> Port %d ENABLED!",
                                DrvRes->instanceNo,
                                DrvRes->ActualPort,
                                0, 0, 0);

        uret = USBCDNS3_SlotEnable(DrvRes);
        if (uret != 0U) {
            // ignore this error
        }

    } else {

        // port is disabled
        UINT32 actual_speed = USBCDNS3_U32BitsRead(XHCI_PORTSC_PORTSPEED_MASK, XHCI_PORTSC_PORTSPEED_SHIFT, PortStatus);

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_connect_status_change_connect(): <%d> Port %d DISABLED!",
                                DrvRes->instanceNo,
                                DrvRes->ActualPort,
                                0, 0, 0);

        // do reset for USB20
        if (actual_speed < CH9_USB_SPEED_SUPER) {
            uret = USBCDNS3_RootHubPortReset(DrvRes);
            if (uret != 0U) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_connect_status_change_connect(): <%d> Critical error! wrong value in one of function parameters",
                                        DrvRes->instanceNo,
                                        0, 0, 0, 0);
            }
            uret = USBCDNS3_SlotEnable(DrvRes);
            if (uret != 0U) {
                // ignore this error
            }
        }
    }
}

/**
 * Function aborts currently executed command if any
 * @param res driver resources
 */
static void abortCurrentCommand(XHC_CDN_DRV_RES_s const *DrvRes)
{

    UINT64 crcr = USBCDNS3_Read64(&DrvRes->Registers.xhciOperational->CRCR);

    // check if any command is pending
    if ((crcr & ((UINT64) XHCI_CRCR_LO_CRR_MASK)) > 0U) {
        // read CRCR register value and set CA (command abort) bit
        crcr |= (UINT64) XHCI_CRCR_LO_CA_MASK;
        //vDbgMsg(USBSSP_DBG_DRV, DBG_FYI, "<%d> CRCR:%016X\n", res->instanceNo, crcr);
        USBCDNS3_Write64(&DrvRes->Registers.xhciOperational->CRCR, crcr);

        // wait until CRR (Command Ring Running) is not active
        do {
            crcr = USBCDNS3_Read64(&DrvRes->Registers.xhciOperational->CRCR);
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "abortCurrentCommand(): Command abort", 0, 0, 0, 0, 0);
            USB_UtilityTaskSleep(1);
        } while ((crcr & ((UINT64) XHCI_CRCR_LO_CRR_MASK)) > 0U);
    }
}


/**
 * handle connect status change for disconnect case
 * @param res driver resources
 * @param portStatus port status
 */
static void dev_connect_status_change_disconnect(XHC_CDN_DRV_RES_s *DrvRes) {

    // device is disconnected

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_connect_status_change_disconnect(): <%d> Device disconnected",
                            DrvRes->instanceNo,
                            0, 0, 0, 0);

    // set U1 nd U2 to default value
    USBCDNS3_XhcU1TimeoutSet(DrvRes, 0);
    USBCDNS3_XhcU2TimeoutSet(DrvRes, 0);

    // call disconnect callback of cusbd module
    if (DrvRes->DeviceModeFlag == USBCDNS3_MODE_DEVICE) {
        if (DrvRes->DeviceCallbacks.Disconnect != NULL) {
            DrvRes->DeviceCallbacks.Disconnect(DrvRes);
        }
    }

    // abort current command
    abortCurrentCommand(DrvRes);
    DrvRes->Connected = 0U;
    DrvRes->FlagDevConfigured = 0U;

    // issue disable slot command
    (void) USBCDNS3_SlotDisable(DrvRes);

}


/**
 * Function handles connect status change event
 * @param res driver resources
 * @param portStatus port status read from portsc register
 */
static void dev_connect_status_change(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortStatus) {

    // check CCS bit, check if device is connected
    if ((PortStatus & XHCI_PORTSC_CCS_MASK) != 0U) {
        dev_connect_status_change_connect(DrvRes, PortStatus);
    } else {
        dev_connect_status_change_disconnect(DrvRes);
    }
}


/**
 * Function handles port reset change
 * @param res driver resources
 * @param portStatus port status read from portsc register
 */
static void dev_port_reset_change(XHC_CDN_DRV_RES_s *DrvRes, UINT32 portStatus) {

    UINT32 uret = 0;
    UINT32 portpmsc;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_port_reset_change(): <%d> Port reset change",
                            DrvRes->instanceNo,
                            0, 0, 0, 0);

    // set U1 and U2 timeouts to default value
    USBCDNS3_XhcU1TimeoutSet(DrvRes, 0);
    USBCDNS3_XhcU2TimeoutSet(DrvRes, 0);

    // call disconnect callback of CUSBD module
    //UINT32 portpmsc = CPS_FLD_WRITE(USBSSP__PORTPMSC1USB2, L1S, 0, 2U);
    portpmsc = 2; // set L1 status to 2 (Not Yet)
    USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[USBCDNS3_DEV_MODE_20_PORT_IDX].PORTPMSC, portpmsc);

    // check if port enabled
    if ((portStatus & XHCI_PORTSC_PED_MASK) != 0U) {
        // check if EnableSlot command is not already pending
        if (DrvRes->EnableSlotPending == 0U) {
            if (DrvRes->ActualDeviceSlot != 0U) {
                USBCDNS3_ConnectSpeedUpdate(DrvRes);
                uret = USBCDNS3_XhcCmdResetDevice(DrvRes);
                if (uret != 0U) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_port_reset_change(): <%d> Could not reset controller",
                                          DrvRes->instanceNo,
                                          0, 0, 0, 0);
                }

                // @ 2021/Aug/11
                // looks we need to place RESET_DEVICE command to controller first
                // and then place DISABLE_ENDPOINT commands
                // Otherwise controller keeps on waiting for RESET_DEVICE command
                // and no EVENT reported.
                // it could be reproduced by USBCV tool.
                // no device found if not doing this.
                if (DrvRes->DeviceCallbacks.Reset != NULL) {

                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_port_reset_change(): <%d> Connected %d, DevConfigFlag %d\r\n",
                                          DrvRes->instanceNo,
                                          DrvRes->Connected,
                                          DrvRes->DevConfigFlag, 0, 0);

                    DrvRes->DeviceCallbacks.Reset(DrvRes);

                }

            } else {
                // Slot not enabled yet, enable it
                uret = USBCDNS3_SlotEnable(DrvRes);
                if (uret != 0U) {
                    // ignore this error
                }
            }
        }
    }
}

static void dev_port_link_change(const XHC_CDN_DRV_RES_s *DrvRes, UINT32 portStatus) {

    //TODO no enum values generated for PLS field!
    UINT32 pls_value = USBCDNS3_U32BitsRead(XHCI_PORTSC_PLS_MASK, XHCI_PORTSC_PLS_SHIFT, portStatus);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_port_link_change(): <%d> Device link state %d", DrvRes->instanceNo, pls_value, 0, 0, 0);

    if (pls_value == 15U /* RESUME, USB2 only */) {

        UINT32 portsc;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_port_link_change(): <%d> Device resumed from low power mode", DrvRes->instanceNo, 0, 0, 0, 0);

        // PLS=U0 LWS=1
        portsc = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTSC);
        portsc = USBCDNS3_U32BitsWrite(XHCI_PORTSC_PLS_MASK, XHCI_PORTSC_PLS_SHIFT, portsc, 0); // PLS = 0 (U0)
        portsc = USBCDNS3_U32BitSet(XHCI_PORTSC_LWS_WIDTH,
                                     XHCI_PORTSC_LWS_MASK,
                                     XHCI_PORTSC_LWS_WOCLR,
                                     portsc); // LWS = 1

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_port_link_change(): <%d> portsc: %08X",
                                DrvRes->instanceNo,
                                portsc & ~(XHCI_PORTSC_PED_MASK),
                                0, 0, 0);

        USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTSC, portsc & ~(XHCI_PORTSC_PED_MASK));
    }
}


/**
 * Port change detection. Function handles all changes on port
 * @param[in] res driver resources
 */
static void dev_event_process_port_change(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32 port_status;
    UINT32 port_id;
    UINT32 mask_all_change_bits = XHCI_PORTSC_CSC_MASK
            | XHCI_PORTSC_PEC_MASK | XHCI_PORTSC_WRC_MASK
            | XHCI_PORTSC_PRC_MASK | XHCI_PORTSC_PLC_MASK
            | XHCI_PORTSC_CEC_MASK | XHCI_PORTSC_OCC_MASK;

    port_id = USBCDNS3_TrbPortIdGet(DrvRes->EventPtr);
    DrvRes->ActualPort = port_id;
    port_status = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTSC);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "dev_event_process_port_change(): <%d> Port ID: %d, PORTSC: 0x%08X PORTPMSC: 0x%08X",
                            DrvRes->instanceNo,
                            DrvRes->ActualPort,
                            port_status,
                            USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTPMSC),
                            0);

    do {
        // clear all interrupts except Port Enabled/Disabled bit
        USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTSC, port_status & ~(XHCI_PORTSC_PED_MASK));

        // handle connect status change
        if ((port_status & XHCI_PORTSC_CSC_MASK) != 0U) {
            dev_connect_status_change(DrvRes, port_status);
        }

        // handle port reset change IF port is connected
        if (((port_status & XHCI_PORTSC_PRC_MASK) != 0U) &&
            ((port_status & XHCI_PORTSC_CCS_MASK) != 0U)) {
            dev_port_reset_change(DrvRes, port_status);
        }

        // handle port link state change
        if ((port_status & XHCI_PORTSC_PLC_MASK) != 0U) {
            dev_port_link_change(DrvRes, port_status);
        }

        // update portStatus - in meantime new change may have happen
        port_status = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_id - 1U].PORTSC);

    } while ((port_status & mask_all_change_bits) > 0U);
}

/*
 * Get setup ID of TRB
 */
static UINT8 getSetupId(XHCI_RING_ELEMENT_s const *trb) {
    return (UINT8) ((trb->DWord3 & XHCI_TRB_STS_SETUPID_MASK) >> XHCI_TRB_STS_SETUPID_POS);
}

/*
 * Get the request type of Setup Stage Event TRB (Device mode only)
 */
static inline UINT8 getSetupBmRequestType(XHCI_RING_ELEMENT_s const *trb) {
    return (UINT8) (trb->DWord0 & 0xFFU);
}

/*
 * Get the setup request of Setup Stage Event TRB (Device mode only)
 */
static inline UINT8 getSetupBrequest(XHCI_RING_ELEMENT_s const *trb) {
    return (UINT8) ((trb->DWord0 >> 8UL) & 0xFFU);
}

/*
 * Get the value of Setup Stage Event TRB (Device mode only)
 */
static inline UINT16 getwValue(XHCI_RING_ELEMENT_s const *trb) {
    return (UINT16) ((trb->DWord0 >> 16UL) & 0xFFFFU);
}

/*
 * Get the length of Setup Stage Event TRB (Device mode only)
 */
static inline UINT16 getwLength(XHCI_RING_ELEMENT_s const *trb) {
    return (UINT16) ((trb->DWord1 >> 16UL) & 0xFFFFU);
}

/*
 * Get the Index of Setup Stage Event TRB (Device mode only)
 */
static inline UINT16 getwIndex(XHCI_RING_ELEMENT_s const *trb) {
    return (UINT16) (trb->DWord1 & 0xFFFFU);
}

/**
 * Displaying of setup request. This is internal driver function, called only when
 * SSP controller works in device mode.
 *
 * @param[in] res driver resources
 */
static void displaySetupRequest(XHC_CDN_DRV_RES_s *DrvRes) {

    USB_CH9_SETUP_s *setup = &DrvRes->DevSetupReq;

    DrvRes->SetupID = getSetupId(DrvRes->EventPtr);

    #ifdef USB_PRINT_SUPPORT
    //USB_UtilityPrintUInt5("<%d> Received SETUP packet ID: %d", DrvRes->instanceNo, DrvRes->SetupID, 0, 0, 0);
    #endif

    setup->bRequest = getSetupBrequest(DrvRes->EventPtr);
    setup->bmRequestType = getSetupBmRequestType(DrvRes->EventPtr);
    setup->wValue = getwValue(DrvRes->EventPtr);
    setup->wIndex = getwIndex(DrvRes->EventPtr);
    setup->wLength = getwLength(DrvRes->EventPtr);

    // display setup request
    //XHCI_DISP_DEV_SETUP_REQ(setup);
}


/**
 * Handling of setup request. This is internal driver function, called only when
 * SSP controller works in device mode. Function parses incoming setup and replies
 * to USB host accordingly.
 *
 * @param[in] res driver resources
 */
static void dev_event_process_setup_packet(XHC_CDN_DRV_RES_s *DrvRes) {

    displaySetupRequest(DrvRes);

    if ((DrvRes->Ep0State == USBCDNS3_EP0_HALT_PENDING) || (DrvRes->Ep0State == USBCDNS3_EP0_HALT_SETUP_PENDING)) {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_event_process_setup_packet(): <%d> mark as USBSSP_EP0_SETUP_PENDING", DrvRes->instanceNo, 0, 0, 0, 0);

        // if EP0 is being halted - mark setup as pending
        DrvRes->Ep0State = USBCDNS3_EP0_HALT_SETUP_PENDING;

    } else if (DrvRes->Ep0State == USBCDNS3_EP0_HALTED) {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_event_process_setup_packet(): <%d> reset endpoint 0", DrvRes->instanceNo, 0, 0, 0, 0);

        // if EP0 is halted - reset EP0 to transition out from halt state
        (void) USBCDNS3_EndpointReset(DrvRes, 1U);
        DrvRes->Ep0State = USBCDNS3_EP0_SETUP_PENDING;

    } else if (DrvRes->Ep0State == USBCDNS3_EP0_SETUP_PENDING) {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_event_process_setup_packet: <%d> New setup received while USBSSP_EP0_SETUP_PENDING", DrvRes->instanceNo, 0, 0, 0, 0);

    } else {
        if ((DrvRes->Ep0State == USBCDNS3_EP0_SETUP_PHASE) || (DrvRes->Ep0State == USBCDNS3_EP0_DATA_PHASE)) {

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "dev_event_process_setup_packet(): <%d> setup request is being handled already", DrvRes->instanceNo, 0, 0, 0, 0);

        }
        // handle setup request
        dev_ep0_setup_packet(DrvRes);
    }
}

/**
 * Handling transfer completion status cases
 * @param res driver resources
 * @param transferObj transfer objectsetd
 * @return 1 when user callback needs to be called, 0 elsewhere
 */
static UINT8 dev_transfer_cmpl_status(XHC_CDN_DRV_RES_s * DrvRes, XHC_CDN_PRODUCER_QUEUE_s *transferObj) {

    UINT8 ret = 0U;
    UINT32 epIndex = transferObj->ContextIndex;

    static char str_buffer[255];
    static char int_buffer[255];
    str_buffer[0] = '\0';
    if (IO_UtilityUInt32ToStr(int_buffer, 255, DrvRes->instanceNo, 10) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, "<");
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    IO_UtilityStringAppend(str_buffer, 255, "> ContextIndex: 0x");
    if (IO_UtilityUInt32ToStr(int_buffer, 255, transferObj->ContextIndex, 16) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    IO_UtilityStringAppend(str_buffer, 255, " (EP");
    if (IO_UtilityUInt32ToStr(int_buffer, 255, (transferObj->ContextIndex >> 1U), 10) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    if ((transferObj->ContextIndex & 1U) != 0U) {
        IO_UtilityStringAppend(str_buffer, 255, "_IN) ");
    } else {
        IO_UtilityStringAppend(str_buffer, 255, "_OUT) ");
    }
    IO_UtilityStringAppend(str_buffer, 255, "Completion Code: ");
    if (IO_UtilityUInt32ToStr(int_buffer, 255, transferObj->CompletionCode, 10) != 0U) {
        // ignore return value
    }
    IO_UtilityStringAppend(str_buffer, 255, int_buffer);
    IO_UtilityStringAppend(str_buffer, 255, "(");
    IO_UtilityStringAppend(str_buffer, 255, USBCDNS3_TrbCmplCodeStringGet(transferObj->CompletionCode));
    IO_UtilityStringAppend(str_buffer, 255, ")");
    USB_UtilityPrint(USB_PRINT_FLAG_L4, str_buffer);

    //USB_UtilityPrintUInt5("<%d> ContextIndex: 0x%02X (EP%d_%d)  Completion code: %d",
    //                    DrvRes->instanceNo,
    //                    transferObj->ContextIndex,
    //                    (transferObj->ContextIndex >> 1U),
    //                    (transferObj->ContextIndex & 1U),
    //                     transferObj->CompletionCode);


    // handle different completion codes
    switch (transferObj->CompletionCode) {

            // for success
        case XHCI_TRB_CMPL_SUCCESS:
            ret = 1U;
            break;

            // when endpoint stalled
        case XHCI_TRB_CMPL_STALL_ERROR:
            (void) USBCDNS3_EndpointReset(DrvRes, epIndex);
            transferObj->IsDisabledFlag = 1U;
            break;

            // when short packet received
        case XHCI_TRB_CMPL_SHORT_PKT:
            if (transferObj->IgnoreShortPacket == 0U) {
                ret = 1U;
            } else {
                ret = 0U;
                if (USBCDNS3_TrbChainBitGet(transferObj->DequeuePtr) == 0U) {
                    transferObj->IgnoreShortPacket = 0;
                }
            }

            if (USBCDNS3_TrbChainBitGet(transferObj->DequeuePtr) > 0U) {
                transferObj->IgnoreShortPacket = 1U;
            }
            break;

            // when missed service error
        case XHCI_TRB_CMPL_MISSED_SRV_ER:
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "<%d> MISSED_SERVICE_ERROR Endpoint(%d)",
                                DrvRes->instanceNo, epIndex, 0, 0, 0);

            // since we always set IOC for the last TRB of the td, only call completion for the TRB with IOC
            if (USBCDNS3_TrbChainBitGet(transferObj->DequeuePtr) == 0U) {
                ret = 1U;
            }
            break;

        case XHCI_TRB_CMPL_RING_UNDERRUN:
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "<%d> RING_UNDERRUN Endpoint(%d)",
                                DrvRes->instanceNo, epIndex, 0, 0, 0);
            ret = 0;
            break;
        case XHCI_TRB_CMPL_RING_OVERRUN:
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> RING_OVERRUN Endpoint(%d)",
                                DrvRes->instanceNo, epIndex, 0, 0, 0);
            ret = 1;
            break;
        case XHCI_TRB_CMPL_NO_PNG_RSP_ER:
            ret = 1U;
            break;
        default:
            // do nothing by default
            break;
    }
    return ret;
}

/**
 * Send status stage for setup with data phase
 * @param res driver resources
 * @param transferObj pointer to endpoint transfer event
 */
static void dev_ep0_status_stage_send(XHC_CDN_DRV_RES_s *DrvRes, XHC_CDN_PRODUCER_QUEUE_s const * transferObj)
{
    if ((DrvRes->DeviceModeFlag == USBCDNS3_MODE_DEVICE) && (transferObj->ContextIndex == XHCI_EP0_CONTEXT_OFFSET)) {

        // make sure that interrupt is from data stage
        if (DrvRes->Ep0State == USBCDNS3_EP0_DATA_PHASE) {
            // and send status stage
            (void) USBDCDNS3_ControlTransfer(DrvRes, NULL, 0, 0);
        }
    }
}

/**
 * data transfer completion callback handler
 * @param res driver resources
 */
static void dev_transfer_cmpl_callback(XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s * const EventPtr)
{

    UINT32 trbType;
    UINT8  ep_index    = USBCDNS3_TrbEndpointIdGet(EventPtr);
    UINT8  slot_id     = USBCDNS3_TrbSlotIdGet(EventPtr);
    const void         *ptr_void;
    XHC_CDN_COMPLETE_f  ptr_func;

    // get transfer object, it may be default endpoint, endpoint or stream object
    XHC_CDN_PRODUCER_QUEUE_s * transferObj = USBCDNS3_TransferObjectGet(DrvRes);

    trbType = USBCDNS3_EpCompletionHandler(DrvRes, transferObj);

    ptr_func = transferObj->CompleteFunc;
    AmbaMisra_TypeCast(&ptr_void, &ptr_func);

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                          "dev_transfer_cmpl_callback(): DevCtrlRxPending = %d, cmpl_func = 0x%X, trb_type = %d",
                          (UINT64)DrvRes->DevCtrlRxPending,
                          IO_UtilityPtrToU64Addr(ptr_void),
                          (UINT64)trbType,
                          (UINT64)0U,
                          (UINT64)0U);

    /* In device mode, we wait for the data st */
    if (trbType == XHCI_TRB_STATUS_STAGE) {
        // call user complete callback
        if (transferObj->CompleteFunc != NULL) {
            transferObj->CompleteFunc(DrvRes, slot_id, ep_index, 0, EventPtr, NULL, 0U);
        }
    } else {
        if (transferObj->CompleteFunc != NULL) {
            transferObj->CompleteFunc(DrvRes, slot_id, ep_index, 0, EventPtr,
                    (UINT8 *)USBCDNS3_U64AddrToTrbRingPtr(transferObj->LastXferBufferPhyAddr),
                    transferObj->LastXferActualLength);
        } else {
            if ((DrvRes->DevCtrlRxPending == 1U) && (trbType == XHCI_TRB_DATA_STAGE)) {
                // here we receieve a transfer completion event for CONTROL DATA_STAGE packet
                // we need to call upper layer for handling whole setup packet
                UINT32              uret;
                const USB_CH9_SETUP_s *setup = &DrvRes->DevCtrlSetupDataPending;;
                // send request to higher layer and check result
                if (DrvRes->DeviceCallbacks.Setup != NULL) {
                    uret = DrvRes->DeviceCallbacks.Setup(USBCDNS3_UdcInstanceGet(), setup);
                    if (uret != 0U) {
                        // action TBD
                    }
                } else {

                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_transfer_cmpl_callback(): ERROR: No callback defined for CUSBD module !", DrvRes->instanceNo, 0, 0, 0, 0);

                    uret = USB_ERR_XHCI_NO_CALLBACK;
                }

                DrvRes->DevCtrlRxPending = 0;

                // post user application handler
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "dev_transfer_cmpl_callback(): Ep0State = %d, uret = %d", DrvRes->Ep0State, uret, 0, 0, 0);

            }
        /*
         * All 'if ... else if' constructs shall be terminated with an 'else' statement
         * (MISRA2012-RULE-15_7-3)
         */
        }

        // send CONTROL STAGE_STAGE if necessary
        dev_ep0_status_stage_send(DrvRes, transferObj);
    }

}



/**
 * data transfer completion handler
 * @param res driver resources
 */
static void dev_event_process_transfer(XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s * const EventPtr) {

    UINT8 callbackFlag;

    // get transfer object, it may be default endpoint, endpoint or stream object
    XHC_CDN_PRODUCER_QUEUE_s *transferObj = USBCDNS3_TransferObjectGet(DrvRes);

    // check transfer status
    // handle stall etc.
    callbackFlag = dev_transfer_cmpl_status(DrvRes, transferObj);

    if (callbackFlag == 1U) {
        dev_transfer_cmpl_callback(DrvRes, EventPtr);
    }
}


/**
 *
 * @param res
 * @param trbType
 */
static void dev_event_process_one(XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s * const EventPtr, UINT32 TrbType)
{
    // check event type
    switch (TrbType) {
        case XHCI_TRB_PORT_ST_CHG_EVT:
            // handle events on port
            dev_event_process_port_change(DrvRes);
            break;

            // handle command completion
        case XHCI_TRB_CMD_CMPL_EVT:
            dev_event_process_command_completion(DrvRes);
            break;

            // handle transfer event
        case XHCI_TRB_TRANSFER_EVENT:
            dev_event_process_transfer(DrvRes, EventPtr);
            break;

            // for device only
        case XHCI_TRB_SETUP_STAGE:
            dev_event_process_setup_packet(DrvRes);
            break;

            // handle host controller event
        case XHCI_TRB_HOST_CTRL_EVT:
            // TBD
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_event_process_one(): <%d> Host Controller ERROR:", DrvRes->instanceNo, 0, 0, 0, 0);
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "    Completion Code: %d", DrvRes->EventPtr->DWord2 >> XHCI_TRB_CMPL_CODE_POS, 0, 0, 0, 0);
            break;

            // not ready notification
        case XHCI_TRB_NRDY_EVT:
            // TBD
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "dev_event_process_one(): <%d> NRDY on endpoint index %d", DrvRes->instanceNo, USBCDNS3_TrbEndpointIdGet(DrvRes->EventPtr), 0, 0, 0);
            break;

        default:
            // TBD
            USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_ERROR, "dev_event_process_one(): <%d> unknown TRB type %d, Ptr 0x%8X",
                                     (UINT64)DrvRes->instanceNo,
                                     (UINT64)TrbType,
                                     IO_UtilityPtrToU64Addr(EventPtr),
                                     (UINT64)0U,
                                     (UINT64)0U);
            break;
    }
    return;
}


/**
 * Handling of all events
 *
 * @param[in] res driver resources
 */
static void dev_event_process_all(XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT32 trb_type;

    do {

        USBCDNS3_TrbInfoDisplay(DrvRes->EventPtr, "EVENT.");

        trb_type = USBCDNS3_TrbTypeGet(DrvRes->EventPtr);
        dev_event_process_one(DrvRes, DrvRes->EventPtr, trb_type);

        USBCDNS3_EventPtrUpdate(DrvRes);

        // check if any pending event still on event ring and handle it if yes
    } while (USBCDNS3_TrbToggleBitGet(DrvRes->EventPtr) == DrvRes->EventToogleBit);
}


/**
 * Process ISR event and update event read pointer
 * @param res driver resources
 */
static UINT32 dev_event_process(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT8  func_uret = 0;
    UINT32 uret = 0;

    // invalidate event data
    USB_UtilityCacheInvdUInt8((UINT8 *)DrvRes->EventPtr, (UINT32)sizeof(XHCI_RING_ELEMENT_s));

    // check babble interrupt
    if (USBCDNS3_TrbToggleBitGet(DrvRes->EventPtr) != DrvRes->EventToogleBit) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_event_process(): Babble interrupt: Host %d != Drv %d", USBCDNS3_TrbToggleBitGet(DrvRes->EventPtr), DrvRes->EventToogleBit, 0, 0, 0);
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_ERROR, "DrvRes->EventPtr        = 0x%X", IO_UtilityPtrToU64Addr(DrvRes->EventPtr));
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_ERROR, "DrvRes->epRingPoolStart = 0x%X", IO_UtilityPtrToU64Addr(DrvRes->XhciMemResources->EventPool));
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_ERROR, "DrvRes->epRingPoolEnd   = 0x%X", IO_UtilityPtrToU64Addr(&DrvRes->XhciMemResources->EventPool[AMBA_XHCI_EVENT_QUEUE_SIZE - 1U]));
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "sizeof EventRingPool %d, size of XHCI_RING_ELEMENT_s %d", (UINT32)sizeof(DrvRes->XhciMemResources->EventPool), (UINT32)sizeof(XHCI_RING_ELEMENT_s), 0, 0, 0);
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "ERDP = 0x%X", IO_UtilityRegRead32(0x2020005038UL), 0, 0, 0, 0);
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "dev_event_process(): Assertion!");
        USB_UtilityAssert();
        func_uret = 1;
        uret = USB_ERR_XHCI_EVT_BABBLE;
    }

    if (func_uret == 0U) {
        if (USBCDNS3_EventTrbCheck(DrvRes, DrvRes->EventPtr) != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "dev_event_process(): trb event check failed", 0, 0, 0, 0, 0);
            func_uret = 1;
            USBCDNS3_EventPtrUpdate(DrvRes);
        }
    }
    if (func_uret == 0U) {
        dev_event_process_all(DrvRes);
    }
    return uret;
}


UINT32 USBDCDNS3_IsrHandler(void)
{
    UINT32 uret;
    XHC_CDN_DRV_RES_s *cdn_drv_res = USBCDNS3_DrvInstanceGet();

    uret = USBSSP_IsrSF(cdn_drv_res);

    if (uret == 0U) {

        USBCDNS3_XhcUsbStsEventClear(cdn_drv_res);

        // Enable the Interrupter by writing a '1' to the Interrupt Pending (IP)
        USBCDNS3_Write32(&cdn_drv_res->Registers.xhciInterrupter[0].IMAN, XHCI_IMAN0_IE_MASK | XHCI_IMAN0_IP_MASK);

        uret = dev_event_process(cdn_drv_res);

        USBCDNS3_Write64(&cdn_drv_res->Registers.xhciInterrupter[0].ERDP,
                        USBCDNS3_PtrToU64PhyAddr(&cdn_drv_res->EventPtr->DWord0) | XHCI_ERDP0_LO_EHB_MASK);

    }

    return uret;

}


