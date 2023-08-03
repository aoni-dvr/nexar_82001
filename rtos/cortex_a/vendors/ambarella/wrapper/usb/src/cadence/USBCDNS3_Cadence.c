/**
 *  @file USBCDNS3_Cadence.c
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
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_CadenceSanity.h>


/**
 * Constructs a uint16_t value from uint8_t pointer
 *
 * @param dataPtr Pointer to uint8 buffer
 * @return uint16_t value constructed from uint8 buffer
 */
static inline UINT16 getU16ValFromU8Ptr(const UINT8 *dataPtr) {

    // Constructs a uint32_t value from uint8_t pointer
    UINT16 value = (UINT16) dataPtr[0];
    UINT16 byte1 = ((UINT16) dataPtr[1]) << 8U;

    value += byte1;
    return value;
}

/**
 * Construct CH9 Setup packet from U8 data
 * @param SetupData [IN] Pointer to the UINT8 * buffer having setup data
 * @param Ch9Setup  [OUT] pointer to the output struct USB_CH9_SETUP_s
 */
static void ch9_setup_construct(const UINT8 *SetupData, USB_CH9_SETUP_s* CH9Setup)
{
    // map the bytes to the struct values
    CH9Setup->bmRequestType = SetupData[0];
    CH9Setup->bRequest = SetupData[1];
    CH9Setup->wValue = getU16ValFromU8Ptr(&SetupData[2]);
    CH9Setup->wIndex = getU16ValFromU8Ptr(&SetupData[4]);
    CH9Setup->wLength = getU16ValFromU8Ptr(&SetupData[6]);
}


/**
 * Checks whether the specified ep is enabled for data transfer
 * @param DrvRes [IN] driver resources
 * @param EpIndex [IN] index of endpoint according to xhci spec e.g for ep1out
              EpIndex=2, for ep1in EpIndex=3, for ep2out EpIndex=4 end so on
 * @retval  0 success
 * @retval  USB_ERR_XHCI_NO_CONNECT not connected error
 * @retval  USB_ERR_XHCI_EP_NO_CONFIG EP not configured for transfer
 * @retval  USB_ERR_XHCI_EP_NO_INIT
 * @retval  USB_ERR_XHCI_EP0_RUNNING
 */
UINT32 USBCDNS3_EpXferEnabledCheck(XHC_CDN_DRV_RES_s const *DrvRes, UINT32 EpIndex)
{
    UINT32 uret = 0;

    /* operation not permitted on disconnected device */
    if ((DrvRes->DevAddress == 0U) || (DrvRes->Connected == 0U)) {
        uret = USB_ERR_XHCI_NO_CONNECT;
    } else if (EpIndex > 1U) {
        /* check endpoint descriptor */
        if (DrvRes->EpQueueArray[EpIndex].epDesc[0] != USB_DESC_LENGTH_ENDPOINT) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EpXferEnabledCheck(): <%d> Error: Endpoint index %d not initialized correctly",
                                  DrvRes->instanceNo, EpIndex, 0, 0, 0);
            uret = USB_ERR_XHCI_EP_NO_INIT;
        } else if (DrvRes->DevConfigFlag == 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EpXferEnabledCheck(): <%d> Error: Endpoint index %d NOT configured",
                                  DrvRes->instanceNo, EpIndex, 0, 0, 0);
            uret = USB_ERR_XHCI_EP_NO_CONFIG;
        } else {
            /* Else required by MISRA*/
        }
    } else {
        /* For EP0 check if last transfer is complete
         * since Ep0 supports only control transfer */
        if (DrvRes->Ep0Queue.IsRunningFlag == 1U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EpXferEnabledCheck(): <%d> Error: EP0 is already running",
                                  DrvRes->instanceNo, 0, 0, 0, 0);
            uret = USB_ERR_XHCI_EP0_RUNNING;
        }
    }

    return uret;
}

/**
 * Reset port.
 * @param[in] DrvRes driver resources
 */
UINT32 USBCDNS3_RootHubPortReset(XHC_CDN_DRV_RES_s const *DrvRes)
{

    // Check if parameters are valid.
    UINT32 uret = USBSSP_ResetRootHubPortSF(DrvRes);

    if ((uret != 0U) || (DrvRes == NULL)) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_RootHubPortReset(): Critical error! Wrong value in one of function parameters");
    } else if (DrvRes->ActualPort == 0U) {
        uret = USB_ERR_XHCI_NO_CONNECT;
    } else {
        UINT32 port_index = DrvRes->ActualPort;
        UINT32 reg = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[port_index - 1U].PORTSC);
        reg |= XHCI_PORTSC_PR_MASK;

        USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[port_index - 1U].PORTSC, reg);

        // in host mode we need to pool PED bit of PORTSC
        if (DrvRes->DeviceModeFlag == USBCDNS3_MODE_HOST) {
            UINT32 ped;

            // pool PED until set to 1
            do {
                ped = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTSC) & XHCI_PORTSC_PED_MASK;
                USB_UtilityTaskSleep(1);
            } while (ped == 0U);
        }
    }
    return uret;
}


/**
 * Ring doorbell for command queue
 * @param DrvRes [IN] driver resources
 */
void USBCDNS3_HostCommandDoorbell(const XHC_CDN_DRV_RES_s *DrvRes)
{
    USBCDNS3_DoorbellWrite(DrvRes, 0, 0);
}

/*
 * Get the ED bit of Event TRB
 */
static UINT8 event_trb_event_data_bit_get(XHCI_RING_ELEMENT_s const *Trb)
{
    return (UINT8) ((Trb->DWord3 >> 2U) & 1U);
}

/*
 * Get the trb transfer length (Normal TRB)
 */
static UINT32 trb_transfer_length_get(XHCI_RING_ELEMENT_s const *Trb)
{
    return (Trb->DWord2 & XHCI_TRB_TRANSFER_LENGTH_MASK);
}

/*
 * Get the trb transfer length (Event TRB)
 */
static UINT32 event_trb_transfer_length_get(XHCI_RING_ELEMENT_s const *Trb)
{
    return (Trb->DWord2 & XHCI_TRB_EVT_RESIDL_LEN_MSK);
}


static XHCI_RING_ELEMENT_s * previous_trb_get(const XHC_CDN_PRODUCER_QUEUE_s *Queue, XHCI_RING_ELEMENT_s *CurrentTrb)
{

    XHCI_RING_ELEMENT_s *prev_trb      = CurrentTrb;
    UINT64               pool_start    = IO_UtilityPtrToU64Addr(Queue->RingArray);
    UINT64               prev_trb_addr = IO_UtilityPtrToU64Addr(prev_trb);
    const void          *ptr_void;

    //--prev_trb;
    prev_trb_addr -= sizeof(XHCI_RING_ELEMENT_s);
    //expand 'prev_trb = IO_UtilityU64AddrToPtr(prev_trb_addr);'
    ptr_void = IO_UtilityU64AddrToPtr(prev_trb_addr);
    AmbaMisra_TypeCast(&prev_trb, &ptr_void);

    // check if TD overturn on ring
    if (prev_trb_addr < pool_start) {
        UINT8 tb_prev_trb;
        UINT8 tb_actual_trb;

        prev_trb = &Queue->RingArray[AMBA_XHCI_PRODUCER_QUEUE_SIZE - 2U];

        tb_prev_trb   = USBCDNS3_TrbToggleBitGet(prev_trb);
        tb_actual_trb = USBCDNS3_TrbToggleBitGet(CurrentTrb);

        // if cycle bit has not been changed return error
        if (tb_prev_trb == tb_actual_trb) {
            prev_trb = NULL;
        }
    }
    return prev_trb;
}

// get XHCI endpoint state
// @param EpIndex [IN] XHCI Device Context Index (DCI)
UINT32 USBCDNS3_EpStatusGet(XHC_CDN_DRV_RES_s const *DrvRes, UINT32 EpIndex)
{

    UINT32 uret;

    // check if enIndex is within correct index range
    if (EpIndex < XHCI_EP0_CONTEXT_OFFSET) {
        // display error
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "<%d> Endpoint context Index error < %d",
                              DrvRes->instanceNo, XHCI_EP0_CONTEXT_OFFSET,
                              0, 0, 0);

        uret = XHCI_EPCTX_STATE_ERROR;
    } else {

        // keeps integer value returned from endpoint context
        UINT32 hwValue;

        // need to invalid device context first?
        USB_UtilityCacheInvdUInt8((UINT8 *)&DrvRes->XhciMemResources->OutputContext, (UINT32)sizeof(XHCI_OUTPUT_CONTEXT_s));

        // get state of default endpoint
        if (EpIndex == XHCI_EP0_CONTEXT_OFFSET) {
            hwValue = DrvRes->XhciMemResources->OutputContext.Ep0Context[0] & XHCI_EPCTX_STATE_MASK;
        } else { // get status of no default endpoint
            // calculate index of endpoint in epContext array
            UINT32 ep_base = EpIndex - XHCI_EPX_CONTEXT_OFFSET;

            // read endpoint state from output context updated by controller
            hwValue = DrvRes->XhciMemResources->OutputContext.EpContext[ep_base][0] & XHCI_EPCTX_STATE_MASK;
        }
        // transcode integer values to enumerated values - MISRA requirement
        switch (hwValue) {

                // endpoint state disabled
            case 0: uret = XHCI_EPCTX_STATE_DISABLED;
                break;

                // // endpoint state running
            case 1: uret = XHCI_EPCTX_STATE_RUNNING;
                break;

                // endpoint state halted
            case 2: uret = XHCI_EPCTX_STATE_HALTED;
                break;

                // endpoint state stopped
            case 3: uret = XHCI_EPCTX_STATE_STOPPED;
                break;

                // endpoint state error
            default: uret = XHCI_EPCTX_STATE_ERROR;
                break;

        }
    }

    return (uret);
}


/**
 * Insert Link TRB
 * @param Queue [IN] pointer to producer queue
 * @param LinkTrbChainFlag [IN] Chain flag if using Link TRB
 */
static void link_trb_insert(const XHC_CDN_PRODUCER_QUEUE_s *Queue, UINT32 LinkTrbChainFlag)
{

    UINT32 trb_type = XHCI_TRB_LINK;
    UINT32 link_trb_flags = (trb_type << XHCI_TRB_TYPE_POS)
            | XHCI_TRB_LNK_TGLE_CYC_MSK
            | ((UINT32) (Queue->ToogleBit))
            | LinkTrbChainFlag;

    // last TRB in the ring is always Link TRB
    XHCI_RING_ELEMENT_s *target_trb = &Queue->RingArray[AMBA_XHCI_PRODUCER_QUEUE_SIZE - 1U];

    /* set DWORD0 and DWORD1 */
    USBCDNS3_U64ValueSet(
            &target_trb->DWord0,
            &target_trb->DWord1,
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&Queue->RingArray[0].DWord0))
            );
    /* set DWORD2 */
    target_trb->DWord2 = 0;
    /* set flags in DWORD3 */
    target_trb->DWord3 = link_trb_flags;

    // flush trb data
    USB_UtilityCacheFlushUInt8((UINT8 *)target_trb, (UINT32)sizeof (XHCI_RING_ELEMENT_s));

    USBCDNS3_TrbInfoDisplay(Queue->EnqueuePtr, "LINK.");

}


/**
 * Update Enqueue Pointer. Also update Link TRB if wrap-around.
 * @param Queue [IN] pointer to producer queue
 */
void USBCDNS3_QueuePointerUpdate(XHC_CDN_PRODUCER_QUEUE_s *Queue, UINT32 LinkTrbChainFlag, const char *DebugString)
{

    XHCI_RING_ELEMENT_s *old_ptr = Queue->EnqueuePtr;
    // calculate where ring ends - the last element is a LINK TRB
    const XHCI_RING_ELEMENT_s *poolEnd = &Queue->RingArray[(AMBA_XHCI_PRODUCER_QUEUE_SIZE - 2U)];
    UINT64                     enqueue_ptr_addr;
    UINT64                     pool_end_addr;
    //void                      *ptr_void;

    USBCDNS3_TrbInfoDisplay(Queue->EnqueuePtr, DebugString);

    // get address of pool end
    pool_end_addr = IO_UtilityPtrToU64Addr(poolEnd);

    // need to do cache flush?
    USB_UtilityCacheFlushUInt8((UINT8 *)Queue->EnqueuePtr, (UINT32)sizeof(XHCI_RING_ELEMENT_s));

    // increase enqueue pointer
    ++Queue->EnqueuePtr;

    // get address of enqueue pointer
    enqueue_ptr_addr = IO_UtilityPtrToU64Addr(Queue->EnqueuePtr);

    // check if EnqueuePtr exceeded ring pool and turn back to origin if yes
    if (enqueue_ptr_addr > pool_end_addr) {

        Queue->EnqueuePtr = Queue->RingArray;

        /* update TRB when queue is not full */
        if (Queue->EnqueuePtr != Queue->DequeuePtr) {
            /* insert LINK TRB */
            link_trb_insert(Queue, LinkTrbChainFlag);
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "USBCDNS3_QueuePointerUpdate(): Insert Link TRB", 0, 0, 0, 0, 0);
            /* toggle the cycle bit */
            Queue->ToogleBit ^= 1U;
        }
    }

    // check if queue full and if yes do nothing
    if (Queue->EnqueuePtr == Queue->DequeuePtr) {
        Queue->EnqueuePtr = old_ptr;
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_QueuePointerUpdate(): QUEUE FULL!", 0, 0, 0, 0, 0);
    }
}


/**
 * Function updates DrvRes->DevConfigFlag
 * @param DrvRes [IN] driver resource
 * @param NewCfgFlag [IN] flag to be set as actual
 */
void USBCDNS3_DeviceConfigFlagSet(XHC_CDN_DRV_RES_s *DrvRes, UINT8 NewCfgFlag)
{
    UINT8 prev_cfg_flag = DrvRes->DevConfigFlag;

    if (NewCfgFlag != prev_cfg_flag) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                              "USBCDNS3_DeviceConfigFlagSet(): <%d> Configured status changed from %d to %d",
                              DrvRes->instanceNo,
                              prev_cfg_flag,
                              NewCfgFlag,
                              0, 0);
        DrvRes->DevConfigFlag = NewCfgFlag;
    }
}


/**
 * Enables U1 for (Time > 0), disables U1 for (Time = 0)
 */
void USBCDNS3_XhcU1TimeoutSet(const XHC_CDN_DRV_RES_s *DrvRes, UINT8 Time)
{
    UINT32 reg_value = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTPMSC);
    if (Time > 0U) {
        reg_value |= 0x00000001U;
    } else {
        reg_value &= ~0x00000001U;
    }
    USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTPMSC, reg_value);
}

/**
 * Enables U2 for (Time > 0), disables U2 for (Time = 0)
 */
void USBCDNS3_XhcU2TimeoutSet(const XHC_CDN_DRV_RES_s *DrvRes, UINT8 Time)
{

    UINT32 reg_value = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTPMSC);
    if (Time > 0U) {
        reg_value |= 0x00000100U;
    } else {
        reg_value &= ~0x00000100U;
    }
    USBCDNS3_Write32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTPMSC, reg_value);
}


/**
 * Updates DrvRes->ActualSpeed (CH9 speed) value
 * @param DrvRes [IN] driver resources
 */
void USBCDNS3_ConnectSpeedUpdate(XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT32 port_status;
    UINT32 port_speed;

    if (DrvRes->ActualPort > 0U) {
        // get protocol speed ID
        port_status = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[DrvRes->ActualPort - 1U].PORTSC);
        port_speed  = USBCDNS3_U32BitsRead(XHCI_PORTSC_PORTSPEED_MASK, XHCI_PORTSC_PORTSPEED_SHIFT, port_status);

        /* according to Table 157: Default USB Speed ID Mapping */
        switch (port_speed) {

            // full speed
            case 1U:
                DrvRes->ActualSpeed = CH9_USB_SPEED_FULL;
                _ux_system_slave->ux_system_slave_speed = UX_FULL_SPEED_DEVICE;
                _ux_system_slave->device_framework        = _ux_system_slave->device_framework_full_speed;
                _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_full_speed;
                break;

            // low speed
            case 2U:
                DrvRes->ActualSpeed = CH9_USB_SPEED_LOW;
                _ux_system_slave->ux_system_slave_speed = UX_LOW_SPEED_DEVICE;
                break;

            // high speed
            case 3U:
                DrvRes->ActualSpeed = CH9_USB_SPEED_HIGH;
                _ux_system_slave->ux_system_slave_speed = UX_HIGH_SPEED_DEVICE;
                _ux_system_slave->device_framework        = _ux_system_slave->device_framework_high_speed;
                _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_high_speed;
                break;

            // super speed
            case 4U:
                DrvRes->ActualSpeed = CH9_USB_SPEED_SUPER;
                _ux_system_slave->ux_system_slave_speed = UX_SUPER_SPEED_DEVICE;
                _ux_system_slave->device_framework        = _ux_system_slave->device_framework_super_speed;
                _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_super_speed;
                break;

            // super speed plus
            case 5U:
                DrvRes->ActualSpeed = CH9_USB_SPEED_SUPER_PLUS;
                _ux_system_slave->ux_system_slave_speed = UX_SUPER_SPEED_DEVICE;
                _ux_system_slave->device_framework        = _ux_system_slave->device_framework_super_speed;
                _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_super_speed;
                break;

            // speed unknown
            default:
                DrvRes->ActualSpeed = CH9_USB_SPEED_UNKNOWN;
                _ux_system_slave->device_framework        = _ux_system_slave->device_framework_high_speed;
                _ux_system_slave->device_framework_length = _ux_system_slave->device_framework_length_high_speed;
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_ConnectSpeedUpdate(): <%d> unknown port speed: %d", DrvRes->instanceNo, port_speed, 0, 0, 0);
                break;
        }

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBCDNS3_ConnectSpeedUpdate(): <%d> actual speed: %d", DrvRes->instanceNo, DrvRes->ActualSpeed, 0, 0, 0);

    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_ConnectSpeedUpdate(): no port enabled");
    }
}


/**
 * Clear XHC USBSTS events
 * @param DrvRes
 */
void USBCDNS3_XhcUsbStsEventClear(const XHC_CDN_DRV_RES_s *DrvRes)
{
    // Get the ISR status word
    UINT32 *usbsts_reg_ptr = &DrvRes->Registers.xhciOperational->USBSTS;
    UINT32  reg = USBCDNS3_Read32(usbsts_reg_ptr);

    // No clear bits for Save/Restore Error, Port Change Detect, Host System Error
    reg &= ~(XHCI_USBSTS_SRE_MASK | XHCI_USBSTS_PCD_MASK | XHCI_USBSTS_HSE_MASK);
    // clear bit for Event Interrupt
    reg |= XHCI_USBSTS_EINT_MASK;

    USBCDNS3_Write32(usbsts_reg_ptr, reg);
}

/*
 * Check if the SlotID is valid in the Event TRB
 * @retval 0                               valid
 * @retval USB_ERR_XHCI_INVALID_TRB_SLOTID invalid
 */
static UINT32 event_trb_slot_id_check(const XHCI_RING_ELEMENT_s *Trb)
{
    UINT32 uret = 0U;
    UINT32 trb_type = USBCDNS3_TrbTypeGet(Trb);

    // check if slotId is within correct range
    // slot id is valid only for TRB's checked below
    if ((trb_type == XHCI_TRB_TRANSFER_EVENT)
            || (trb_type == XHCI_TRB_CMD_CMPL_EVT)
            || (trb_type == XHCI_TRB_BNDWTH_RQ_EVT)
            || (trb_type == XHCI_TRB_DOORBELL_EVENT)
            || (trb_type == XHCI_TRB_DEV_NOTIFCN_EVT)) {
        UINT32 value = (UINT32) USBCDNS3_TrbSlotIdGet(Trb);

        // check if slot id is within correct range
        if (value > AMBA_XHCI_MAX_DEV_SLOT_NUM) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "event_trb_slot_id_check(): Slot ID(%d) exceeded %d", value, AMBA_XHCI_MAX_DEV_SLOT_NUM, 0, 0, 0);
            uret = USB_ERR_XHCI_INVALID_TRB_SLOTID;
        }
    }
    return uret;
}

/**
 * Validate whether a dequeue pointer is correct. Works only for contiguous queues
 *
 * @param Queue [IN] The queue corresponding to this pointer.
 * @param DequeueAddr [IN] De-queue pointer to validate.
 * @retval 0 dequeue-pointer lies in the given queue.
 * @retval USB_ERR_XHCI_INVALID_TRB_PTR dequeue-pointer does NOT lie in the given queue.
 */
static UINT32 dequeue_ptr_validate(const XHC_CDN_PRODUCER_QUEUE_s *Queue, UINT64 DequeueAddr)
{
    UINT32               uret = 0;
    const XHCI_RING_ELEMENT_s *pool_end = &Queue->RingArray[(AMBA_XHCI_PRODUCER_QUEUE_SIZE - 1U)];
    UINT64 pool_start_addr  = USBCDNS3_PtrToU64PhyAddr(&Queue->RingArray[0].DWord0);
    UINT64 pool_end_addr    = USBCDNS3_PtrToU64PhyAddr(&pool_end->DWord0);

    if ((DequeueAddr < pool_start_addr) || (DequeueAddr > pool_end_addr)) {
        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_ERROR,
                                 "dequeue_ptr_validate(): INCORRECT TRB PTR value: 0x%X (0x%X~0x%X)",
                                 DequeueAddr,
                                 pool_start_addr,
                                 pool_end_addr, (UINT64)0, (UINT64)0);
        uret = USB_ERR_XHCI_INVALID_TRB_PTR;
    }
    return uret;
}

/**
 * Check whether a transfer event TRB is a valid TRB
 * @param DrvRes [IN] Pointer to driver resources
 * @param Trb [IN] Pointer to the TRB
 * @retval 0 valid TRB
 * @retval Others invalid TRB
 */
static UINT32 event_trb_transfer_event_check(const XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s *Trb)
{
    UINT32  uret = 0;
    UINT8   endpoint_id  = USBCDNS3_TrbEndpointIdGet(Trb);
    UINT64  dequeue_addr = USBCDNS3_TrbDequeueAddrGet(Trb);

    if (endpoint_id == XHCI_EP0_CONTEXT_OFFSET) {
        uret = dequeue_ptr_validate(&DrvRes->Ep0Queue, dequeue_addr);
        if (uret != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "event_trb_transfer_event_check(): ptr check failed for ep0.");
        }
    } else {
        UINT32 completion_code = USBCDNS3_TrbCmplCodeGet(Trb); // returns code within [0:255]
        // check if event NOT triggered by stream
        if ((event_trb_event_data_bit_get(DrvRes->EventPtr) == 0U)
                && (completion_code != XHCI_TRB_CMPL_RING_UNDERRUN)
                && (completion_code != XHCI_TRB_CMPL_RING_OVERRUN)
                && (completion_code != XHCI_TRB_CMPL_VF_EVTRNGFL_ER)
                && (completion_code != XHCI_TRB_CMPL_STOP_LEN_INV)) {

            uret = dequeue_ptr_validate(&DrvRes->EpQueueArray[endpoint_id], dequeue_addr);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "event_trb_transfer_event_check(): ptr check failed for epX");
            }
        }
    }

    return uret;
}

/**
 * Check whether a Command Completion event TRB is a valid TRB
 * @param DrvRes [IN] Pointer to driver resources
 * @param Trb [IN] Pointer to the TRB
 * @retval 0 valid
 * @retval Others invalid
 */
static UINT32 event_trb_cmd_cmpl_check(const XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s *Trb)
{
    UINT64  dequeue_addr = USBCDNS3_TrbDequeueAddrGet(Trb);
    UINT32  uret = dequeue_ptr_validate(&DrvRes->CommandQueue, dequeue_addr);
    if (uret != 0U) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "event_trb_cmd_cmpl_check(): ptr check failed.");
    }

    return uret;
}

/**
 * Check whether the Event TRB is valid
 *
 * @param DrvRes [IN] Pointer to driver resources
 * @param Trb [IN] Pointer to the Event TRB
 * @retval 0 valid event TRB
 * @retval Others invalid event TRB
 */
UINT32 USBCDNS3_EventTrbCheck(const XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s *Trb)
{
    UINT32 trb_type;
    UINT32 value;
    UINT32 result = 0;

    // check if TRB type field in within correct value
    trb_type = USBCDNS3_TrbTypeGet(Trb);
    if (trb_type > XHCI_TRB_VF_SEC_VIOLN_EVT) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EventTrbCheck(): INCORRECT TRB Type value: %d", trb_type, 0, 0, 0, 0);
        result = USB_ERR_XHCI_INVALID_TRB_TYPE;
    }

    if (result == 0U) {
        // check if completion is within correct range
        value = USBCDNS3_TrbCmplCodeGet(Trb); // returns code within [0:255]
        if ((value > XHCI_TRB_CMPL_SPLT_TRNSCN_ER) &&
            (value < XHCI_TRB_CMPL_CDNSDEF_ERCODES)) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EventTrbCheck(): INCORRECT COMPLETION value: %d", value, 0, 0, 0, 0);
            result = USB_ERR_XHCI_INVALID_TRB_CMPL_CODE;
        }
    }

    if (result == 0U) {
        result = event_trb_slot_id_check(Trb);
    }

    if (result == 0U) {
        if (trb_type == XHCI_TRB_TRANSFER_EVENT) {
            result = event_trb_transfer_event_check(DrvRes, Trb);
        } else if (trb_type == XHCI_TRB_CMD_CMPL_EVT) {
            result = event_trb_cmd_cmpl_check(DrvRes, Trb);
        } else {
            /*
             * All 'if ... else if' constructs shall be terminated with an 'else' statement
             * (MISRA2012-RULE-15_7-3)
             */
        }
    }
    return result;
}


/**
 * Update event pointer
 * @param DrvRes [IN] driver resources
 */
void USBCDNS3_EventPtrUpdate(XHC_CDN_DRV_RES_s *DrvRes)
{

    // get address of last element in event ring
    const XHCI_RING_ELEMENT_s *pool_end = &DrvRes->XhciMemResources->EventPool[AMBA_XHCI_EVENT_QUEUE_SIZE - 1U];
    UINT64                     pool_end_addr = IO_UtilityPtrToU64Addr(pool_end);
    UINT64                     event_ptr_addr;

    //USB_UtilityCacheFlushUInt8((UINT8 *)DrvRes->EventPtr, sizeof(XHCI_RING_ELEMENT_s));

    // step event pointer
    ++DrvRes->EventPtr;

    // get address of current event pointer
    event_ptr_addr = IO_UtilityPtrToU64Addr(DrvRes->EventPtr);

    // check if event pointer wraps
    if (event_ptr_addr > pool_end_addr) {

        // event pointer wraps
        //  1. set event pointer to 1st event trb in event pool
        //  2. change toggle bit
        DrvRes->EventPtr = DrvRes->XhciMemResources->EventPool;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "USBCDNS3_EventPtrUpdate(): <%d> Change toggle bit, current %d", DrvRes->instanceNo, DrvRes->EventToogleBit, 0, 0, 0);

        if (DrvRes->EventToogleBit == 1U) {
            DrvRes->EventToogleBit = 0U;
        } else {
            DrvRes->EventToogleBit = 1U;
        }
    }

    // invalidate data for current event TRB
    USB_UtilityCacheInvdUInt8((UINT8 *)DrvRes->EventPtr, (UINT32)sizeof(XHCI_RING_ELEMENT_s));
}


/**
 * Calculate full/low speed endpoint interval based on bInterval
 * See xHCI spec Section 6.2.3.6 for more details.
 * @param bInterval [IN]
 * @return valid endpoint context interval value
 */
UINT8 USBSSP_CalcFsLsEPIntrptInterval(UINT8 bInterval)
{

    UINT8 interval = 2U;
    UINT8 bitOffset; // register with '1' circulating
    UINT8 res1; // used for finding the most significant 1 from left to right
    UINT8 res2 = 0U; // used for finding the first 1 looking from right to left

    if (bInterval > 0U) {
        // find the oldest bit
        bitOffset = 0x80U;
        do {
            res1 = bInterval & bitOffset;
            bitOffset >>= 1U;
        } while ((res1 == 0U) && (bitOffset > 0U));

        // calculate context interrupt value
        bitOffset = 0x01;
        do {
            res2 = res1 & bitOffset;
            ++interval;
            bitOffset <<= 1U;
        } while (res2 == 0U);
    } else {
        interval = 0U; // what to return if bInterval is zero?
    }

    return interval;
}


/**
 * Return dequeue cycle state bit from endpoint context
 * @param EpQueue endpoint object
 * @return dequeue cycle state bit
 */
static UINT32 epctx_dequeue_cycle_state_get(XHC_CDN_PRODUCER_QUEUE_s const * EpQueue)
{
    UINT32 dcs;
    UINT32 maxPStreams;

    // get actual value of maxPStreams
    maxPStreams = (EpQueue->HWContext[0] >> XHCI_EPCTX_PMAXSTREAMS_POS) & 0x1FU;

    if (maxPStreams > 0U) {
        dcs = 0;
    } else {
        dcs = EpQueue->ToogleBit;
    }

    return dcs;
}

/**
 * Sets TR dequeue pointer field in endpoint context
 */
void USBCDNS3_EpCtxTRDequeuePtrSet(XHC_CDN_PRODUCER_QUEUE_s * EpQueue)
{
    UINT32 dcs = epctx_dequeue_cycle_state_get(EpQueue);
    USBCDNS3_U64ValueSet(
            &EpQueue->HWContext[2],
            &EpQueue->HWContext[3],
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&EpQueue->EnqueuePtr->DWord0) | dcs));
    EpQueue->DequeuePtr = EpQueue->EnqueuePtr;
}

/**
 * Disables single endpoint (before issuing CONFIGURE_ENDPOINT command)
 * @param DrvRes [IN] driver resources
 * @param LogicalAddress [IN] Logical Endpoint Address
 */
UINT32 USBCDNS3_EndpointDisable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 LogicalAddress)
{

    // check if res is not NULL
    UINT32 uret = USBSSP_DisableEndpointSF(DrvRes);

    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                "USBCDNS3_EndpointDisable(): <%d> Critical error! wrong value in one of function parameters",
                DrvRes->instanceNo, 0, 0, 0, 0);
    }

    if (uret == 0U) {
        UINT32  epIn;
        UINT32  epBase;
        UINT32  contextEntry;
        UINT32  leDropMask = 0U;

        // calculate context index from endpoint address
        //epIn = (UINT8) (((epAddress & CH9_USB_EP_DIR_IN) > 0U) ? 1U : 0U);
        if ((LogicalAddress & CH9_USB_EP_DIR_IN) > 0U) {
            epIn = 1;
        } else {
            epIn = 0;
        }
        //epBase = (UINT8) ((((epAddress & 0x7FU) - 1U) * 2U) + ((epIn > 0U) ? 1U : 0U));
        epBase = LogicalAddress & 0x07FU;
        epBase = epBase - 1U;
        epBase = epBase * 2U;
        if (epIn > 0U) {
            epBase = epBase + 1U;
        }

        contextEntry = epBase + XHCI_EPX_CONTEXT_OFFSET;

        // enqueue stop endpoint command
        uret = USBCDNS3_EndpointStop(DrvRes, contextEntry);

        if (contextEntry < 32U) {
            leDropMask = USBCDNS3_CpuToLe32((UINT32) (1UL << contextEntry));
        }

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
                "USBCDNS3_EndpointDisable(): <%d> ep_address: %02X (EP%d_%d) leDropMask:0x%08X",
                DrvRes->instanceNo,
                LogicalAddress,
                (LogicalAddress & 0x0FU),
                ((epIn != 0U) ? (UINT32)1U : (UINT32)0U),
                leDropMask);

        // Setting Dx flag ...
        DrvRes->InputContext->InputControlContext[0] |= leDropMask;

        // ... and clearing Ax flag, if set
        if ((DrvRes->InputContext->InputControlContext[1] & leDropMask) != 0U) {
            DrvRes->InputContext->InputControlContext[1] &= ~leDropMask;
        }

        USB_UtilityMemoryCopy(&DrvRes->InputContextCopy, DrvRes->InputContext, sizeof (XHCI_INPUT_CONTEXT_s));
    }

    return uret;
}

/**
 * Configure and enable all endpoints according to Configuration Descriptor
 * @param DrvRes     [IN] driver resources
 * @param ConfigDesc [IN] configuration descriptor
 * @
 */
static UINT32 endpoints_configure(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *ConfigDesc, UINT16 Length)
{

    UINT32 i = 0;
    UINT32 uret = 0;

    // Ax flags (within Input Control Context) need to be: A0 = 1, A1 = 0, Dx should be 0
    DrvRes->InputContext->InputControlContext[0] = 0; // Dx = 0
    DrvRes->InputContext->InputControlContext[1] = USBCDNS3_CpuToLe32(1); // A0 = 1, all other Ax = 0

    USB_UtilityMemoryCopy(&DrvRes->InputContextCopy, DrvRes->InputContext, sizeof (XHCI_INPUT_CONTEXT_s));

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
            "configureEndpoints(): <%d> configureEndpoints: %02X %d",
            DrvRes->instanceNo, ConfigDesc[0], Length, 0, 0);

    while ((i < Length) && (uret == 0U)) {
        /* descriptor type has offset 1 in descriptor */
        if (ConfigDesc[i + 1U] == USB_DESC_TYPE_ENDPOINT) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "configureEndpoints(): <%d> ---Endpoint found---\n", DrvRes->instanceNo, 0, 0, 0, 0);
            uret = USBCDNS3_EndpointEnable(DrvRes, &ConfigDesc[i]);
        }
        i += ConfigDesc[i];
    }

    if (uret == 0U) {
        USBCDNS3_XhcCmdConfigEndpoint(DrvRes);
    }
    return uret;
}


/**
 * Set configuration. Function configures SSP controller as well as device connected
 * to this SSP controller. Function must not be called from interrupt context.
 *
 * @param DrvRes [IN] driver resources
 * @param ConfigValue [IN] USB device's configuration selector
 *
 * @return CDN_EOK on success
 * @return complete_code XHCI transfer complete status code
 */
UINT32 USBCDNS3_ConfigurationSet(XHC_CDN_DRV_RES_s *DrvRes,
                                 UINT32             ConfigValue,
                                 const UINT8       *EpCfgBuffer,
                                 UINT16             EpCfgBufferLen,
                                 XHC_CDN_COMPLETE_f CompleteFunc)
{

    // check the parameters
    UINT32 uret = USBSSP_SetConfigurationSF(DrvRes, EpCfgBuffer);

    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                "USBCDNS3_ConfigurationSet(): <%d> Critical error! wrong value in one of function parameters",
                DrvRes->instanceNo, 0, 0, 0, 0);
    } else {
        uret = USBCDNS3_EpXferEnabledCheck(DrvRes, 1U);
        if (uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                    "USBCDNS3_ConfigurationSet(): <%d> Critical error! EP0 not ready",
                    DrvRes->instanceNo, 0, 0, 0, 0);
            // pass misra check
        } else {
            const UINT8  *buffer = EpCfgBuffer;
            UINT16        length = EpCfgBufferLen;
            if (buffer == NULL) {
                buffer = DrvRes->Ep0Buff;
                length = ((UINT16) buffer[3] << 8) | (UINT16) buffer[2];
            }

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
                    "USBCDNS3_ConfigurationSet(): <%d> buffer:0x%X length:%d",
                    DrvRes->instanceNo, IO_UtilityPtrToU32Addr(buffer), length, 0, 0);

            DrvRes->Ep0Queue.AggregatedCompleteFunc = NULL;
            DrvRes->Ep0Queue.CompleteFunc = CompleteFunc;
            DrvRes->HostConfigValue = ConfigValue;

            // configure endpoints
            uret = endpoints_configure(DrvRes, buffer, length);
        }
    }

    return uret;
}

/**
 * issue enable slot command
 * @param DrvRes [IN] driver resources
 * @retval 0 Success
 */
UINT32 USBCDNS3_SlotEnable(XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT32 uret = 0;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "USBCDNS3_SlotEnable(): ActualDeviceSlot: %d",
                            DrvRes->ActualDeviceSlot,
                            0, 0, 0, 0);

    // enable slot if no actual device slot is active.
    if (DrvRes->ActualDeviceSlot == 0U) {
        uret = USBCDNS3_XhcCmdEnableSlot(DrvRes);
        if (uret != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR,
                             "USBCDNS3_SlotEnable(): Critical error! wrong value in one of function parameters");
        }
    }
    return uret;
}

/**
 * issue disable slot command
 * @param res driver resources
 * @return CDN_EOK on success, CDN_EINVAL on critical error
 */
UINT32 USBCDNS3_SlotDisable(XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT32 uret = 0;
    // disable slot if any device slot is active.
    if (DrvRes->ActualDeviceSlot > 0U) {
        uret = USBCDNS3_XhcCmdDisableSlot(DrvRes);
        if (uret != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR,
                             "USBCDNS3_SlotDisable(): Critical error! wrong value in one of function parameters");
        }
    }
    return uret;
}

/**
 * Reset of endpoint. Function sends RESET_ENDPOINT_COMMAND to SSP controller
 *
 * @param DrvRes [IN] driver resources
 * @param EndpointIndex [IN] DCI index of endpoint to reset
 * @retval 0 if no errors
 * @retval Others when driver's settings doesn't suit to native platform settings
 */
UINT32 USBCDNS3_EndpointReset(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EndpointIndex)
{

    // check input parameters
    UINT32 uret = USBSSP_ResetEndpointSF(DrvRes, EndpointIndex);

    // if input parameters are not correct return CDN_EINVAL error
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EndpointReset(): <%d> Critical error! Wrong value in one of function parameters", DrvRes->instanceNo, 0, 0, 0, 0);
    } else {
        // enqueue reset endpoint command
        USBCDNS3_XhcCmdResetEndpoint(DrvRes, EndpointIndex);
        USBCDNS3_HostCommandDoorbell(DrvRes);
    }

    return uret;
}

/*
 * Sets feature in device mode
 */
static void device_endpoint_feature_set(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT8 Feature)
{
    // handle in device mode
    UINT32 dw3;
    UINT32 endpoint_state;

    if (Feature > 0U) {
        // set stall, need to halt endpoint
        DrvRes->CommandQueue.EnqueuePtr->DWord0 = 0;
        DrvRes->CommandQueue.EnqueuePtr->DWord1 = 1;
        DrvRes->CommandQueue.EnqueuePtr->DWord2 = 2;

        dw3 = (DrvRes->ActualDeviceSlot << XHCI_TRB_SLOT_ID_POS);
        dw3 |= (EpIndex << XHCI_TRB_ENDPOINT_POS);
        dw3 |= (XHCI_TRB_HALT_ENDP_CMD << XHCI_TRB_TYPE_POS);
        dw3 |= (DrvRes->CommandQueue.ToogleBit & 0x01U);

        DrvRes->CommandQueue.EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(dw3);

        USBCDNS3_QueuePointerUpdate(&DrvRes->CommandQueue, 0U, "CMD.HALT_ENDP.");
        DrvRes->CommandQueue.IsRunningFlag = 1;
        USBCDNS3_HostCommandDoorbell(DrvRes);
    } else {
        // clear stall, reset endpoint if halted
        endpoint_state = USBCDNS3_EpStatusGet(DrvRes, EpIndex);
        if (endpoint_state == XHCI_EPCTX_STATE_HALTED) {
            if (USBCDNS3_EndpointReset(DrvRes, EpIndex) != 0U) {
                // action TBD
            }
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "<%d> Endpoint (0x%02X) not in halted state, current state: %d, reset anyway?\n",
                                  DrvRes->instanceNo, EpIndex, endpoint_state, 0, 0);
            // TODO return correct error number
            if (USBCDNS3_EndpointReset(DrvRes, EpIndex) != 0U) {
                // action TBD
            }
        }
    }
}


/*
 * Sets feature in host mode (send Set/Clear Feature control request to device)
 * @param EpIndex [IN] DCI index of endpoint to set/clear feature on
 * @param Feature [in] when 1 sets stall, when 0 clears stall
 */
static UINT32 host_endpoint_feature_set(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT8 Feature)
{
    UINT8  logical_address;
    UINT32 uret;
    USB_CH9_SETUP_s ch9setup;

    // GET configuration descriptor pattern
    UINT8 setup[] = {
        (UINT8)CH9_USB_REQ_RECIPIENT_ENDPOINT,
        (UINT8)USB_REQ_CLEAR_FEATURE,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };

    //calculate endpoint address from ep_index
    logical_address = (EpIndex / 2U) | (((EpIndex % 2U) != 0U) ? CH9_USB_EP_DIR_IN : 0U);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_endpoint_feature_set(): <%d> Endpoint address: %02X", DrvRes->instanceNo, logical_address, 0, 0, 0);

    // replace endpoint selector with required one
    setup[4] = logical_address;

    // replace clear feature request with set feature request
    if (Feature > 0U) {
        setup[1] = USB_REQ_SET_FEATURE;
    }

    ch9_setup_construct(&setup[0], &ch9setup);
    uret = USBHCDNS3_ControlTransfer(DrvRes, &ch9setup, DrvRes->Ep0Buff);

    if (uret != 0U) {
        // action TBD
    }

    // check if any data queued to transfer
    if (Feature == 0U) {
        UINT32 endpointState = USBCDNS3_EpStatusGet(DrvRes, EpIndex);
        // put endpoint to run state
        if (endpointState == XHCI_EPCTX_STATE_STOPPED) {
            USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, DrvRes->EpQueueArray[EpIndex].ContextIndex);
        }
    }
    return uret;
}


/**
 * Set feature on device's endpoint. Functions sends setup requested to device
 * with set/cleared endpoint feature
 *
 * @param DrvRes [IN] driver resources
 * @param EpIndex [IN] DCI index of endpoint to set/clear feature on
 * @param Feature [in] when 1 sets stall, when 0 clears stall
 *
 * @return CDN_EOK on success
 * @return complete_code XHCI transfer complete status code
 */
UINT32 USBCDNS3_EndpointFeatureSet(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT8 Feature)
{

    // check parameter correctness
    UINT32 uret = USBSSP_EndpointSetFeatureSF(DrvRes, EpIndex);

    if (uret == 0U) {
        if (Feature > 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "USBCDNS3_EndpointFeatureSet(): <%d> Set Feature on ep:%d", DrvRes->instanceNo, EpIndex, 0, 0, 0);
        } else {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "USBCDNS3_EndpointFeatureSet(): <%d> Clear Feature on ep:%d", DrvRes->instanceNo, EpIndex, 0, 0, 0);
        }

        // call handle feature
        if (DrvRes->DeviceModeFlag == USBCDNS3_MODE_HOST) {
            uret = host_endpoint_feature_set(DrvRes, EpIndex, Feature);
        } else {
            device_endpoint_feature_set(DrvRes, EpIndex, Feature);
        }
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_EndpointFeatureSet(): <%d> Critical error! wrong value in one of function parameters, EpIndex 0x%X",
                                DrvRes->instanceNo, EpIndex, 0, 0, 0);
    }

    return uret;
}

/**
 *
 * @param ep
 * @param dequeueAddr
 * @return
 */
static INT32 getStreamIdFromDequeuePtr(const XHC_CDN_PRODUCER_QUEUE_s *ep, UINT64 dequeueAddr)
{
    INT32 streamId = -1;
    INT32 streamIdx = 0;
    /* Get pointer to the last valid TRB for this endpoint */
    const XHCI_RING_ELEMENT_s *poolEnd = &ep->RingArray[(AMBA_XHCI_PRODUCER_QUEUE_SIZE - 1U)];
    UINT64 poolStartAddr = USBCDNS3_PtrToU64PhyAddr(&ep->RingArray[0].DWord0);
    UINT64 poolEndAddr   = USBCDNS3_PtrToU64PhyAddr(&poolEnd->DWord0);

    if ((dequeueAddr >= poolStartAddr) && (dequeueAddr <= poolEndAddr)) {
        /* set streamId to '0' to indicate default endpoint */
        streamId = 0;
    } else {
        for (streamIdx = 0; streamIdx < (INT32) ep->StreamCount ; streamIdx++) {
            const XHC_CDN_PRODUCER_QUEUE_s *steamContext = ep->StreamQueueArray[streamIdx];
            poolEnd = &steamContext->RingArray[(AMBA_XHCI_PRODUCER_QUEUE_SIZE - 1U)];
            poolStartAddr = USBCDNS3_PtrToU64PhyAddr(&steamContext->RingArray[0].DWord0);
            poolEndAddr   = USBCDNS3_PtrToU64PhyAddr(&poolEnd->DWord0);
            if ((dequeueAddr >= poolStartAddr) && (dequeueAddr <= poolEndAddr)) {
                streamId = streamIdx + 1;
                break;
            }
        }
    }

    return streamId;
}


/**
 * Sets some transfer object parameters
 * @param DrvRes [IN] driver resources
 * @param transferObj transfer object
 */
static void setTransferObjInterParams(XHC_CDN_DRV_RES_s *DrvRes, XHC_CDN_PRODUCER_QUEUE_s *TransferObj)
{
    // get endpoint ID
    UINT8 endpoint_id = USBCDNS3_TrbEndpointIdGet(DrvRes->EventPtr);

    // store EventPtr as complete pTR
    TransferObj->CompletePtr = DrvRes->EventPtr;

    // transfer is stoped
    TransferObj->IsRunningFlag = 0U;

    DrvRes->LastEpIntIndex = endpoint_id;
}

/**
 * Get corresponding endpoint Queue object according to last event TRB.
 * @param DrvRes [IN] driver resources
 * @return Queue pointer to endpoint queue object
 */
XHC_CDN_PRODUCER_QUEUE_s *USBCDNS3_TransferObjectGet(XHC_CDN_DRV_RES_s *DrvRes)
{

    XHC_CDN_PRODUCER_QUEUE_s      *queue_object;
    const XHCI_RING_ELEMENT_s     *event_trb_ptr = DrvRes->EventPtr;
    UINT8  endpoint_id                = USBCDNS3_TrbEndpointIdGet(event_trb_ptr);
    UINT64 trb_addr                   = USBCDNS3_TrbDequeueAddrGet(event_trb_ptr);
    XHCI_RING_ELEMENT_s      *trb_ptr = USBCDNS3_U64AddrToTrbRingPtr(trb_addr);

    // handle EP0
    if (endpoint_id == XHCI_EP0_CONTEXT_OFFSET) {
        queue_object = &DrvRes->Ep0Queue;
        queue_object->DequeuePtr = trb_ptr;
    } else {
        // check if event triggered by Event Data TRB
        if (event_trb_event_data_bit_get(DrvRes->EventPtr) > 0U) {

            UINT32 ActualSID = DrvRes->EventPtr->DWord1;

            // get the stream object corresponding to this event data trb
            queue_object = DrvRes->EpQueueArray[endpoint_id].StreamQueueArray[ActualSID - 1U];

            // calculate dequeue pointer ???
            queue_object->DequeuePtr = queue_object->LastQueuedTRB;
            // update ActualSID field in parent endpoint
            DrvRes->EpQueueArray[endpoint_id].ActualSID = queue_object->ActualSID;

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "epObj: 0x%X, epIndex: %d,  ActualSID: %d",
                                IO_UtilityPtrToU32Addr(queue_object), queue_object->ContextIndex, queue_object->ActualSID, 0, 0);

            // use parent's complete to notify higher
            // layers about stream transfer complete
            queue_object->CompleteFunc = DrvRes->EpQueueArray[endpoint_id].CompleteFunc;
        } else {
            INT32 streamId = getStreamIdFromDequeuePtr(&DrvRes->EpQueueArray[endpoint_id], trb_addr);
            if (streamId < 1) {
                // no stream endpoint
                queue_object = &DrvRes->EpQueueArray[endpoint_id];
                queue_object->DequeuePtr = trb_ptr;
            } else {
                // update ActualSID field in parent endpoint
                DrvRes->EpQueueArray[endpoint_id].EventSID = (UINT16) streamId;

                // get the stream object corresponding to this event data trb
                queue_object = DrvRes->EpQueueArray[endpoint_id].StreamQueueArray[streamId - 1];

                // calculate dequeue pointer ???
                queue_object->DequeuePtr = queue_object->LastQueuedTRB;

                USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3, "epObj: 0x%X, epIndex: %d,  ActualSID: %d",
                                 IO_UtilityPtrToU32Addr(queue_object), queue_object->ContextIndex, queue_object->ActualSID, 0, 0);

                // use parent's complete to notify higher
                // layers about stream transfer complete
                queue_object->CompleteFunc = DrvRes->EpQueueArray[endpoint_id].CompleteFunc;
            }
        }
    }
    setTransferObjInterParams(DrvRes, queue_object);
    queue_object->CompletionCode = (UINT8) USBCDNS3_TrbCmplCodeGet(event_trb_ptr);
    return queue_object;
}

/**
 * Handle completion code for non-default endpoint
 * it traverse whole chained trbs and calculate real transferred bytes
 * @param DrvRes [IN] driver resources
 * @param Queue pointer to endpoint queue object
 * @return TrbType of the event TRB in the Queue
 */
UINT32 USBCDNS3_EpCompletionHandler(XHC_CDN_DRV_RES_s const *DrvRes, XHC_CDN_PRODUCER_QUEUE_s *Queue)
{

    // transfered data length sum
    UINT32 userDataLenSum = 0;

    // really transferred number of bytes
    UINT32 realDataLenSum = 0;

    // get number of residue bytes
    UINT32 numOfResidue = event_trb_transfer_length_get(DrvRes->EventPtr);

    // used in for-each loop in TD parsing
    XHCI_RING_ELEMENT_s *ring_iterator = Queue->DequeuePtr;

    UINT32 trb_type = (ring_iterator == NULL) ? 0U : USBCDNS3_TrbTypeGet(ring_iterator);

    UINT64 buffer_phy_addr = 0;

    // in current version is not full support for streams implemented, we
    // assume that stream has always completed
    if (Queue->ActualSID > 0U) {
        numOfResidue = 0U;
    }

    if ((ring_iterator != NULL) && ((trb_type == XHCI_TRB_NORMAL) ||
        (trb_type == XHCI_TRB_DATA_STAGE) ||
        (trb_type == XHCI_TRB_ISOCH))) {

        UINT8 chain_bit;

        do {

            UINT32 userTrbLength;
            UINT32 realTrbLength;

            // recreate user data length
            userDataLenSum += trb_transfer_length_get(ring_iterator);

            // get length of actual TRB
            userTrbLength = trb_transfer_length_get(ring_iterator);

            // calculate real TRB length
            realTrbLength = userTrbLength - numOfResidue;

            // check if short packet has been received
            if (realTrbLength < userTrbLength) {
                realDataLenSum = 0U;
            }

            // calculate number of bytes really transferred
            realDataLenSum += realTrbLength;

            // display TRB
            USBCDNS3_TrbInfoDisplay(ring_iterator, "COMPLETE TR RING ");

            buffer_phy_addr = USBCDNS3_TrbDequeueAddrGet(ring_iterator);

            // get previous TRB and exit if NULL
            ring_iterator = previous_trb_get(Queue, ring_iterator);
            if (ring_iterator == NULL) {
                break;
            }
            // check chain bit
            chain_bit = USBCDNS3_TrbChainBitGet(ring_iterator);
        } while (chain_bit > 0U);

        // recreate number of bytes for transfer
        Queue->NumOfBytes = userDataLenSum;

        // store really transfered bytes per transfer
        Queue->NumOfResidue = userDataLenSum - realDataLenSum;

        Queue->LastXferActualLength = userDataLenSum - numOfResidue;
        Queue->LastXferBufferPhyAddr = buffer_phy_addr;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "<%d> NumOfBytes: %d, realDataLenSum: %d, numOfResidue: %d",
                    DrvRes->instanceNo,
                    Queue->NumOfBytes,
                    realDataLenSum,
                    Queue->NumOfResidue,
                    0);

    }
    return trb_type;
}

