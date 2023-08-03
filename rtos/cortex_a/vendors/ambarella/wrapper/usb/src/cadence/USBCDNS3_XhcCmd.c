/**
 *  @file USBCDNS3_XhcCmd.c
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
 *  @details USB driver for Cadence USB device/host controller.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_CadenceSanity.h>


/**
 * Function builds configure endpoint command TRB
 * @param res driver resources
 */
void USBCDNS3_XhcCmdConfigEndpoint(XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_CONF_EP_CMD;

    // fulsh input context?
    USB_UtilityCacheFlushUInt8((UINT8 *)DrvRes->InputContext, (UINT32)sizeof(XHCI_INPUT_CONTEXT_s));

    // set input context pointer in TRB
    USBCDNS3_U64ValueSet(
            &DrvRes->CommandQueue.EnqueuePtr->DWord0,
            &DrvRes->CommandQueue.EnqueuePtr->DWord1,
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(DrvRes->InputContext->InputControlContext))
            );

    // set device slot, cycle bit, TRB type
    dw3 = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (DrvRes->CommandQueue.ToogleBit& 0x01U);
    DrvRes->CommandQueue.EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(dw3);

    USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.CONF_EP_CMD.");
    DrvRes->CommandQueue.IsRunningFlag = 1;
    USBCDNS3_HostCommandDoorbell(DrvRes);
}

/**
 * Function disables slot on enabled device
 * @param[in] res driver resources
 */
UINT32 USBCDNS3_XhcCmdDisableSlot(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32 uret = USBSSP_DisableSlotSF(DrvRes);
    UINT32 trb_type = XHCI_TRB_DISABLE_SLOT_COMMAND;

    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcCmdDisableSlot(): <%d> Critical error! Wrong value in one of function parameters",
                                DrvRes->instanceNo, 0, 0, 0, 0);
        // pass misra check
    } else {

        UINT8 epIndex;
        UINT32 epState;
        UINT32 dword3_value;

        // first execute stop endpoint command on all running endpoints
        for (epIndex = 1U; epIndex < XHCI_EP_CONTEXT_MAX; epIndex++) {
            epState = USBCDNS3_EpStatusGet(DrvRes, epIndex);
            if (epState != XHCI_EPCTX_STATE_RUNNING) {
                continue;
            } else {
                UINT32 retValue;
                retValue = USBCDNS3_EndpointStop(DrvRes, epIndex);
                if (retValue != 0U) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcCmdDisableSlot(): <%d> Stop endpoint(%d) command error",
                            DrvRes->ActualDeviceSlot,
                            epIndex, 0, 0, 0);
                }
            }
        }

        dword3_value =  (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
        dword3_value |= (trb_type << XHCI_TRB_TYPE_POS);
        dword3_value |= (DrvRes->CommandQueue.ToogleBit & 0x01U);

        DrvRes->CommandQueue.EnqueuePtr->DWord3 = dword3_value;

        USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.DISABLE_SLOT.");
        DrvRes->ActualPort = 0;
        DrvRes->ActualSpeed = CH9_USB_SPEED_UNKNOWN;
        DrvRes->ActualDeviceSlot = 0;
        DrvRes->DevAddress = 0;
        DrvRes->Ep0Queue.IsRunningFlag = 0;
        USBCDNS3_DeviceConfigFlagSet(DrvRes, 0);
        USBCDNS3_HostCommandDoorbell(DrvRes);
    }
    return uret;
}

/**
 * Function enables slot on connected device
 * @param[in] res driver resources
 */
UINT32 USBCDNS3_XhcCmdEnableSlot(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32 uret = USBSSP_EnableSlotSF(DrvRes);

    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcCmdEnableSlot(): <%d> Critical error! Wrong value in one of function parameters", DrvRes->instanceNo, 0, 0, 0, 0);
        // pass misra check
    } else if (DrvRes->EnableSlotPending == 0U) {
        UINT32 trb_type     = XHCI_TRB_ENABLE_SLOT_COMMAND;
        UINT32 dword3_value = (trb_type << XHCI_TRB_TYPE_POS);
        dword3_value |= (DrvRes->CommandQueue.ToogleBit & 0x01U);
        DrvRes->CommandQueue.EnqueuePtr->DWord3 = dword3_value;
        USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.ENABLE_SLOT.");
        USBCDNS3_HostCommandDoorbell(DrvRes);
        DrvRes->EnableSlotPending = 1U;
    } else {
        /*
         * All 'if ... else if' constructs shall be terminated with an 'else' statement
         * (MISRA2012-RULE-15_7-3)
         */
    }
    return uret;
}

/**
 * Reset of connected device. Function sends RESET_DEVICE_COMMAND to SSP controller
 * in order to issue reset state on USB bus.
 *
 * @param[in] res driver resources
 * @return CDN_EINVAL when driver's settings doesn't suit to native platform settings
 * @return CDN_EOK if selected endpoint is within available endpoint range
 */
UINT32 USBCDNS3_XhcCmdResetDevice(XHC_CDN_DRV_RES_s *DrvRes) {

    // check if input parameter is not NULL
    UINT32 uret = USBSSP_ResetDeviceSF(DrvRes);
    UINT32 dword3_value;
    UINT32 trb_type = XHCI_TRB_RESET_DEVICE_COMMAND;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBCDNS3_XhcCmdResetDevice(): <%d> ",
            DrvRes->instanceNo, 0, 0, 0, 0);

    // return error code when input parameter is NULL
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "USBCDNS3_XhcCmdResetDevice(): <%d> Critical error! wrong value in one of function parameters",
                              DrvRes->instanceNo, 0, 0, 0, 0);
    } else {
        // reset device address
        DrvRes->DevAddress = 0;
        DrvRes->Ep0Queue.IsRunningFlag = 0;

        // issue reset device command to controller
        dword3_value = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
        dword3_value |= (trb_type << XHCI_TRB_TYPE_POS);
        dword3_value |= (DrvRes->CommandQueue.ToogleBit & 0x01U);
        DrvRes->CommandQueue.EnqueuePtr->DWord3 = dword3_value;

        USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.RESET_DEVICE.");
        USBCDNS3_HostCommandDoorbell(DrvRes);
    }

    return uret;
}

/**
 * Enqueues RESET_ENDPOINT_COMMAND in the command queue. Doesn't ring doorbell
 *
 * @param[in] res driver resources
 * @param[in] endpoint index of endpoint to reset
 */
void USBCDNS3_XhcCmdResetEndpoint(XHC_CDN_DRV_RES_s *DrvRes, UINT32 endpoint)
{
    UINT32 dw3;
    UINT32 toggle_bit_mask = 0x01U;
    UINT32 trb_type = XHCI_TRB_RESET_EP_CMD;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "USBCDNS3_XhcCmdResetEndpoint(): <%d> reset endpoint %d", DrvRes->instanceNo, endpoint, 0, 0, 0);

    dw3 = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
    dw3 |= (endpoint << XHCI_TRB_ENDPOINT_POS);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (DrvRes->CommandQueue.ToogleBit & toggle_bit_mask);

    DrvRes->CommandQueue.EnqueuePtr->DWord3 =USBCDNS3_CpuToLe32(dw3);

    USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.RESET_EP.");
}

/**
 * Gets max packet size for default endpoint
 * @param DrvRes [IN] driver resources
 * @return max packet size for default endpoint according to current connect speed
 */
static UINT32 ep0_max_pkt_size_get(XHC_CDN_DRV_RES_s const *DrvRes) {

    UINT32 epMaxPacketSize;

    switch (DrvRes->ActualSpeed) {

            // low speed
        case CH9_USB_SPEED_LOW: epMaxPacketSize = CH9_USB_EP0_MAX_LOW;
            break;

            // full speed
        case CH9_USB_SPEED_FULL: epMaxPacketSize = CH9_USB_EP0_MAX_FULL;
            break;

            // high speed
        case CH9_USB_SPEED_HIGH: epMaxPacketSize = CH9_USB_EP0_MAX_HIGH;
            break;

            // super, super speed plus
        case CH9_USB_SPEED_SUPER:
        case CH9_USB_SPEED_SUPER_PLUS:
            epMaxPacketSize = CH9_USB_EP0_MAX_SUPER;
            break;

            // unknown speed
        default: epMaxPacketSize = CH9_USB_EP0_MAX_UNKNOWN;
            break;
    }

    return epMaxPacketSize;
}


/**
 * set the slot context
 * @param res driver resource
 */
static void setSlotContext(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32        epMaxPacketSize;
    UINT32        slotSpeed;
    UINT32        u32_value;
    UINT32        ep0_type = XHCI_EPCXT_TYPE_CTRL_BI;
    UINT32        value_1 = 1;

    // set EP0 max packet size
    epMaxPacketSize = ep0_max_pkt_size_get(DrvRes);

    // CH9_UsbSpeed coding is different than XHCI one
    slotSpeed = USBCDNS3_Ch9SpeedToXhciSpeed(DrvRes->ActualSpeed);

    //6.2.2 set slot context entries and speed
    u32_value = (value_1 << XHCI_SLOTCTX_ENTRY_POS);
    u32_value |= (slotSpeed << XHCI_SLOTCTX_SPEED_POS);
    DrvRes->InputContext->SlotContext[0] = u32_value;

    DrvRes->InputContext->SlotContext[1] = DrvRes->ActualPort << XHCI_SLOTCTX_RHPORT_NUM_POS;

    // set USB device address
    DrvRes->InputContext->SlotContext[3] = DrvRes->DevAddress;

    // set default endpoint context
    u32_value = (ep0_type << XHCI_EPCTX_TYPE_POS);
    u32_value |= (epMaxPacketSize << XHCI_EPCTX_MAX_PKT_SZ_POS);
    u32_value |= (XHCI_EPCTX_3ERR << XHCI_EPCTX_CERR_POS);
    DrvRes->InputContext->Ep0Context[1] = u32_value; // endpoint 0 control type, max packet, error count

    USBCDNS3_U64ValueSet(
            &DrvRes->InputContext->Ep0Context[2],
            &DrvRes->InputContext->Ep0Context[3],
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&DrvRes->Ep0Queue.EnqueuePtr->DWord0) | DrvRes->Ep0Queue.ToogleBit)
            );

    DrvRes->Ep0Queue.ContextIndex = 1;

    DrvRes->InputContext->Ep0Context[4] =
            (XHCI_EPCTX_CTRL_AVGTRB_LEN << XHCI_EPCTX_EP_AVGTRBLEN_POS); // endpoint 0 control type - average trb length must be set to 8.

}


/**
 * Function builds SET ADDRESS command TRB
 * @param res driver resources
 * @param bsrVal BSR bit value
 */
static void setAddressCommandTrb(const XHC_CDN_DRV_RES_s *DrvRes, UINT32 bsrVal) {

    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_ADDR_DEV_CMD;

    // two first DWORDs are pointer to input context
    USBCDNS3_U64ValueSet(
            &DrvRes->CommandQueue.EnqueuePtr->DWord0,
            &DrvRes->CommandQueue.EnqueuePtr->DWord1,
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(DrvRes->InputContext->InputControlContext))
            );

    DrvRes->CommandQueue.EnqueuePtr->DWord2 = 0;

    dw3 = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= ((bsrVal & 0x01U) << XHCI_TRB_BSR_POS);
    dw3 |= (DrvRes->CommandQueue.ToogleBit & 0x01U);

    // DWORD3 is a TRB code and required flags
    DrvRes->CommandQueue.EnqueuePtr->DWord3 = dw3;

    USB_UtilityCacheFlushUInt8(
            (UINT8 *) DrvRes->InputContext,
            (UINT32)sizeof (XHCI_INPUT_CONTEXT_s));
}

/**
 * Set address. Function executes set address request on connected device
 * @param DrvRes [in] driver resources
 */
void USBCDNS3_XhcCmdSetAddress(XHC_CDN_DRV_RES_s *DrvRes, UINT8 bsrVal)
{

    // If slot is not enabled (SLOT_ENABLED not completed) - ending
    if ((DrvRes->ActualDeviceSlot != 0U) && (DrvRes->ActualPort != 0U)) {

        void *void_ptr_input_context;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBCDNS3_XhcCmdSetAddress(): actual port: %d, BSR: %d", DrvRes->ActualPort, bsrVal, 0, 0, 0);

        USBCDNS3_ConnectSpeedUpdate(DrvRes);

        AmbaMisra_TypeCast(&void_ptr_input_context, &DrvRes->InputContext);
        USB_UtilityMemorySet(void_ptr_input_context, 0, sizeof (XHCI_INPUT_CONTEXT_s));

        // On a set address also reserve the memory for the output context
        USB_UtilityCacheInvdUInt8((UINT8 *)&DrvRes->XhciMemResources->OutputContext, (UINT32)sizeof (XHCI_OUTPUT_CONTEXT_s));
        USB_UtilityMemorySet(&DrvRes->XhciMemResources->OutputContext, 0, sizeof (XHCI_OUTPUT_CONTEXT_s));
        USB_UtilityCacheFlushUInt8((UINT8 *)&DrvRes->XhciMemResources->OutputContext, (UINT32)sizeof (XHCI_OUTPUT_CONTEXT_s));

        // set input control context: A0 and A1, all Dx should be 0
        DrvRes->InputContext->InputControlContext[0] = 0; // Dx = 0
        DrvRes->InputContext->InputControlContext[1] = 3; // A0/A1=1/1, all other Ax = 0

        /* set slot context */
        setSlotContext(DrvRes);

        // send command to XHCI
        setAddressCommandTrb(DrvRes, bsrVal);

        //if (DrvRes->usbsspCallbacks.InputContextCallback != NULL) {
        //    vDbgMsg(USBSSP_DBG_DRV, DBG_FYI, "<%d> Calling InputContextCallback()\n", res->instanceNo);
        //    res->usbsspCallbacks.InputContextCallback(res);
        //}
        USB_UtilityMemoryCopy(&DrvRes->InputContextCopy, DrvRes->InputContext, sizeof (XHCI_INPUT_CONTEXT_s));

        USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.SET_ADDRESS.");
        USBCDNS3_HostCommandDoorbell(DrvRes);
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "setAddress() Warning: res->ActualDeviceSlot: %d, DrvRes->ActualPort %d", DrvRes->ActualDeviceSlot, DrvRes->ActualPort, 0, 0, 0);
    }
}

/**
 * Set dequeue pointer. Function sends SET_TR_DEQUEUE_POINTER_COMMAND to SSP controller
 *
 * @param[in] res driver resources
 * @param[in] endpoint index to set dequeue pointer to
 */
void USBCDNS3_XhcCmdSetTrDequeuePtr(XHC_CDN_DRV_RES_s *DrvRes, UINT32 epIndex, UINT64 newDequeuePtr)
{
    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_SET_TR_DQ_PTR_CMD;
    #ifdef USB_PRINT_SUPPORT
    //USB_UtilityPrintUInt5("USBCDNS3_XhcCmdSetTrDequeuePtr(): <%d> SET dequeue pointer of endpoint(%d) to 0x%08x%08x\n",
    //                        DrvRes->instanceNo,
    //                        epIndex,
    //                        (UINT32) ((newDequeuePtr & 0xFFFFFFFF00000000ULL) >> 32U),
    //                        (UINT32) ((newDequeuePtr & 0xFFFFFFFFULL)),
    //                        0);
    #endif

    if (epIndex == XHCI_EP0_CONTEXT_OFFSET) {
        USBCDNS3_U64ValueSet(
                &DrvRes->CommandQueue.EnqueuePtr->DWord0,
                &DrvRes->CommandQueue.EnqueuePtr->DWord1,
                USBCDNS3_CpuToLe64(newDequeuePtr | DrvRes->Ep0Queue.ToogleBit)
                //USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&res->Ep0Queue.EnqueuePtr->dword0) | res->Ep0Queue.ToogleBit)
                );
    } else {
        USBCDNS3_U64ValueSet(
                &DrvRes->CommandQueue.EnqueuePtr->DWord0,
                &DrvRes->CommandQueue.EnqueuePtr->DWord1,
                //USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(& res->EpQueueArray[endpoint].EnqueuePtr->dword0) | res->EpQueueArray[endpoint].ToogleBit)
                USBCDNS3_CpuToLe64(newDequeuePtr | DrvRes->EpQueueArray[epIndex].ToogleBit)
                );
    }

    dw3 = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
    dw3 |= (((UINT32)epIndex) << XHCI_TRB_ENDPOINT_POS);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (DrvRes->CommandQueue.ToogleBit & 0x01U);

    DrvRes->CommandQueue.EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(dw3);

    USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.SET_TR_DQ_PTR.");
    USBCDNS3_HostCommandDoorbell(DrvRes);
}

/**
 * Handles Status Phase of control Xfer
 * @param res driver resources
 * @param statusResp Indicates status response 1: ACK 0: Stall
 */
void USBDCDNS3_XhcCmdCtrlStatusStage(XHC_CDN_DRV_RES_s *DrvRes, UINT32 statusResp)
{

    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_STATUS_STAGE;
    UINT32 speedId = (DrvRes->ActualSpeed > CH9_USB_SPEED_HIGH) ?
                XHCI_TRB_STS_SPEED_ID_3 : XHCI_TRB_STS_SPEED_ID_2;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "<%d> Control Xfer Status phase - device mode, statusResp: 0x%08X",
                            (statusResp << XHCI_TRB_STS_STG_STAT_POS), 0, 0, 0, 0);

    // create status TRB
    DrvRes->Ep0State = USBCDNS3_EP0_STATUS_PHASE;
    DrvRes->Ep0Queue.EnqueuePtr->DWord0 = 0;
    DrvRes->Ep0Queue.EnqueuePtr->DWord1 = 0;
    DrvRes->Ep0Queue.EnqueuePtr->DWord2 = 0;

    dw3 = (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (XHCI_TRB_NORMAL_IOC_MASK);
    dw3 |= (DrvRes->Ep0Queue.ToogleBit & 0x01U);
    dw3 |= (statusResp << XHCI_TRB_STS_STG_STAT_POS);
    dw3 |= (DrvRes->SetupID << XHCI_TRB_STS_SETUPID_POS);
    dw3 |= speedId;

    DrvRes->Ep0Queue.EnqueuePtr->DWord3 = dw3;

    USBCDNS3_QueuePointerUpdate(&DrvRes->Ep0Queue, 0U, "EP0.CTRL.STATUS.");
}

