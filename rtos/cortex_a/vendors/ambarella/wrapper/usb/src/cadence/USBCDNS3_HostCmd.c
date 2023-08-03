/**
 *  @file USBCDNS3_HostCmd.c
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
 *  @details USB driver for Cadence USB host commands.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static void host_cmd_cmpl_enable_slot(XHC_CDN_DRV_RES_s * DrvRes)
{

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_cmd_cmpl_enable_slot(): <%d>", DrvRes->instanceNo, 0, 0, 0, 0);

    DrvRes->ActualDeviceSlot = USBCDNS3_TrbSlotIdGet(&DrvRes->SavedCommandEvent);
    if (DrvRes->ActualDeviceSlot > DrvRes->MaxDeviceSlot) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "host_cmd_cmpl_enable_slot(): <%d> ActualDeviceSlot (%d) greater than max slots (%d)",
                               DrvRes->instanceNo, DrvRes->ActualDeviceSlot, DrvRes->MaxDeviceSlot, 0, 0);
    }

    DrvRes->EnableSlotPending = 0U;
    DrvRes->ContextEntries = 1; // A0 and A1 enabled

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
        "host_cmd_cmpl_enable_slot: <%d> Actual dev. slotID: %d, running_flag = %d",
        DrvRes->instanceNo,
        DrvRes->ActualDeviceSlot,
        DrvRes->Ep0Queue.IsRunningFlag,
        0, 0);
}

/**
 * Issue enable slot command in Host mode.
 * Function must not be called from interrupt context.
 * @param DrvRes [IN] driver resources
 * @retval 0 Success
 */
UINT32 USBHCDNS3_SlotEnable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        uret = USBCDNS3_SlotEnable(DrvRes);

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBSSP_IssueEnableSlotCommand(): fail to wait semaphore");
            } else {
                host_cmd_cmpl_enable_slot(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}

static void host_cmd_cmpl_disable_slot(const XHC_CDN_DRV_RES_s * DrvRes)
{
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_cmd_cmpl_disable_slot(): <%d>", DrvRes->instanceNo, 0, 0, 0, 0);
}

/**
 * Issue disable slot command in Host mode.
 * Function must not be called from interrupt context.
 * @param DrvRes [IN] driver resources
 * @retval 0 Success
 */
UINT32 USBHCDNS3_SlotDisable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        uret = USBCDNS3_SlotDisable(DrvRes);

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBHCDNS3_SlotDisable(): fail to wait semaphore");
            } else {
                host_cmd_cmpl_disable_slot(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}


static void host_cmd_cmpl_address_device(XHC_CDN_DRV_RES_s * DrvRes)
{
    DrvRes->DevAddress = 1;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "host_cmd_cmpl_address_device(): <%d> start, modeflag %d, addr = %d", DrvRes->instanceNo, DrvRes->DeviceModeFlag, DrvRes->DevAddress, 0, 0);

}

/**
 * Issue set address command in Host mode.
 * Function must not be called from interrupt context.
 * @param DrvRes [IN] driver resources
 * @retval 0 Success
 */
UINT32 USBHCDNS3_DeviceAddress(XHC_CDN_DRV_RES_s *DrvRes, UINT32 FlagWait)
{
    UINT32 uret = 0;

    if (DrvRes != NULL) {

        USBCDNS3_XhcCmdSetAddress(DrvRes, 0);

        if (FlagWait == 1U) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBHCDNS3_DeviceAddress(): fail to wait semaphore");
            } else {
                host_cmd_cmpl_address_device(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}


static void host_cmd_cmpl_enable_endpoint(XHC_CDN_DRV_RES_s * DrvRes)
{
    USBCDNS3_DeviceConfigFlagSet(DrvRes, 1);
}

/**
 * Issue enable endpoint command in Host mode.
 * Function must not be called from interrupt context.
 * @param DrvRes [IN] driver resources
 * @param Desc   [IN] Endpoint descriptor followed with SS/SSP endpoint companion descriptor
 * @retval 0 Success
 */
UINT32 USBHCDNS3_EndpointEnable(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *Desc, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        uret = USBCDNS3_EndpointEnable(DrvRes, Desc);

        if (uret == 0U) {
            USBCDNS3_XhcCmdConfigEndpoint(DrvRes);
        }

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBHCDNS3_EndpointEnable(): fail to wait semaphore");
            } else {
                host_cmd_cmpl_enable_endpoint(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;

}

/**
 * Issue disable endpoint command in Host mode.
 * Function must not be called from interrupt context.
 * @param DrvRes [IN] driver resources
 * @param LogicEpIndex [IN] Logical endpoint address
 * @retval 0 Success
 */
UINT32 USBHCDNS3_EndpointDisable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 LogicEpIndex, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        uret = USBCDNS3_EndpointDisable(DrvRes, LogicEpIndex);

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBHCDNS3_EndpointDisable(): fail to wait semaphore");
            } else {
                //host_cmd_cmpl_enable_endpoint(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;

}


static void host_cmd_cmpl_endpoint_reset(const XHC_CDN_DRV_RES_s * DrvRes)
{
    UINT32 ep_index = USBCDNS3_TrbEndpointIdGet(DrvRes->CommandQueue.DequeuePtr);
    UINT32 endpoint_state = USBCDNS3_EpStatusGet(DrvRes, ep_index);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> USBSSP_TRB_RESET_ENDPOINT_COMMAND completed on ep %d", DrvRes->instanceNo, ep_index, 0, 0, 0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> Endpoint(%d) status: %d", DrvRes->instanceNo, ep_index, endpoint_state, 0, 0);

    // set dequeue pointer command allowed only for stopped endpoint
    if (endpoint_state == XHCI_EPCTX_STATE_STOPPED) {

    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "host_cmd_cmpl_endpoint_reset():<%d> Endpoint(%d) is not in stopped state", DrvRes->instanceNo, ep_index, 0, 0, 0);
    }
}

/**
 * Issue reset endpoint command in Host mode.
 * Function must not be called from interrupt context.
 * @param DrvRes [IN] driver resources
 * @retval 0 Success
 */
UINT32 USBHCDNS3_EndpointReset(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        uret = USBCDNS3_EndpointReset(DrvRes, EpIndex);

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBH_ResetEndpointCommandIssue(): fail to wait semaphore");
            } else {
                host_cmd_cmpl_endpoint_reset(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}

UINT32 USBHCDNS3_EndpointStop(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        uret = USBCDNS3_EndpointStop(DrvRes, EpIndex);

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBH_ResetEndpointCommandIssue(): fail to wait semaphore");
            } else {
                //host_cmd_cmpl_endpoint_reset(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}


UINT32 USBHCDNS3_TRDequeuePtrSet(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT32 FlagWait)
{
    UINT32 uret = 0;

    if (DrvRes != NULL) {
        XHC_CDN_PRODUCER_QUEUE_s   *transfer_obj = &DrvRes->EpQueueArray[EpIndex];
        const XHCI_RING_ELEMENT_s  *enqueue_ptr  = transfer_obj->EnqueuePtr;

        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_L1,
                                 "USBHCDNS3_TRDequeuePtrSet(): enque ptr = 0x%08X0x%08X",
                                 IO_UtilityPtrToU64Addr(enqueue_ptr));

        USBCDNS3_EpCtxTRDequeuePtrSet(transfer_obj);

        USBCDNS3_XhcCmdSetTrDequeuePtr(DrvRes, EpIndex, USBCDNS3_PtrToU64PhyAddr(&enqueue_ptr->DWord0));

        if (FlagWait == 1U) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBHCDNS3_TRDequeuePtrSet(): fail to wait semaphore");
            } else {
                //host_cmd_cmpl_enable_slot(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}



UINT32 USBHCDNS3_PortStatusRead(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortID)
{
    UINT32 uret = 0;
    if ((DrvRes != NULL) && (PortID < 2U)) {
        AmbaMisra_TouchUnused(DrvRes);
        uret = USBCDNS3_Read32(&DrvRes->Registers.xhciPortControl[PortID].PORTSC);
    }
    return uret;
}

UINT32 USBHCDNS3_FrameNumberRead(XHC_CDN_DRV_RES_s *DrvRes, UINT32 *FrameNumber)
{

    UINT32 value;
    UINT32 uret = UX_SUCCESS;

    if (DrvRes != NULL) {
        AmbaMisra_TouchUnused(DrvRes);
        // Read the micro frame number register.
        value = USBCDNS3_Read32(&DrvRes->Registers.xhciRuntime->MFINDEX);
        // The register is based on micro frames, so we need to divide the
        // value by 8 to get to the millisecond frame number.
        value = (value >> 3U) & 0x03FFU;
        *FrameNumber = value;
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}


