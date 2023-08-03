/**
 *  @file USBCDNS3_DeviceCmd.c
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
 *  @details USB driver for Cadence USB device commands.
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
 * Function handles data phase of device setup request
 * @param res driver resources
 * @param pdata pointer to memory where data is/will be stored
 * @param length data length
 * @param dirFlag data direction flag, 0: OUT (Host-to-device), 1: IN (Device-to-host)
 * @return CDN_EOK if success, error code elsewhere
 */
static UINT32 controlXferDevDataPhase(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *pdata, UINT32 length, UINT32 dirFlag) {

    UINT32 uret = 0;
    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_DATA_STAGE;
    UINT32 speedId = (DrvRes->ActualSpeed > CH9_USB_SPEED_HIGH) ?
                XHCI_TRB_STS_SPEED_ID_3 : XHCI_TRB_STS_SPEED_ID_2;
    UINT64 dataAddrPhy = USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(pdata));

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L2,
                          "<%d> Control Xfer Data phase - device mode, pdata: %p, length: %d, dir: %d",
                            (UINT64)DrvRes->instanceNo,
                            dataAddrPhy,
                            (UINT64)length,
                            (UINT64)dirFlag,
                            (UINT64)0);

    // update default endpoint state
    DrvRes->Ep0State = USBCDNS3_EP0_DATA_PHASE;
    // set data buffer in TRB
    USBCDNS3_U64ValueSet( &DrvRes->Ep0Queue.EnqueuePtr->DWord0, &DrvRes->Ep0Queue.EnqueuePtr->DWord1, dataAddrPhy);

    // set data length in TRB
    DrvRes->Ep0Queue.EnqueuePtr->DWord2 = length;

    // set flags in TRB
    dw3 = ((dirFlag & 0x01U) << XHCI_TRB_STS_TRANSFER_DIR_POS);
    dw3 |= (XHCI_TRB_NORMAL_IOC_MASK);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (DrvRes->Ep0Queue.ToogleBit & 0x01U);
    dw3 |= (DrvRes->SetupID << XHCI_TRB_STS_SETUPID_POS);
    dw3 |= speedId;

    DrvRes->Ep0Queue.EnqueuePtr->DWord3 = dw3;

    USBCDNS3_QueuePointerUpdate(&DrvRes->Ep0Queue, 0U, "EP0.CTRL.DATA_STAGE.");

    return uret;
}

/**
 * Control transfer in device mode. Function used in response to setup event
 *
 * @param[in] res driver resources
 * @param[in] pdata pointer for data to send/receive
 * @param[in] length data length
 *
 * @return CDN_EOK on success
 * @return complete_code XHCI transfer complete status code*
 */
UINT32 USBDCDNS3_ControlTransfer(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *pdata, UINT32 length, UINT32 dirFlag)
{

    // check input parameter correctness
    UINT32 uret = USBSSP_ControlTransferDevSF(DrvRes);

    if ((uret == 0U) && (DrvRes != NULL)) {
        // check if data phase exist
        if (length > 0U) {
            uret = controlXferDevDataPhase(DrvRes, pdata, length, dirFlag);
        } else {
            // send status TRB
            USBDCDNS3_XhcCmdCtrlStatusStage(DrvRes, XHCI_TRB_STS_STG_STAT_ACK);
        }

        DrvRes->Ep0Queue.IsRunningFlag = 1;

        if ((DrvRes->ActualDeviceSlot == 0U) || (DrvRes->Ep0Queue.ContextIndex == 0U)) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBDCDNS3_ControlTransfer(): <%d> Warning - DRBL: Ring doorbell on EP0, slot %d, target id %d",
                    DrvRes->instanceNo, DrvRes->ActualDeviceSlot, DrvRes->Ep0Queue.ContextIndex, 0, 0);
        }

        // depending on data transfer direction write 0 or 1 to DRBL for ep0
        // this quirk applies for device mode only
        if ((length > 0U) && (dirFlag == 0U)) {
            USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, 0U);
        } else {
            USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, DrvRes->Ep0Queue.ContextIndex);
        }
        //CPS_MemoryBarrier();
    } else {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBDCDNS3_ControlTransfer(): Critical error! Wrong value in one of function parameters");
    }

    return uret;
}

UINT32 USBDCDNS3_EndpointStop(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT32 FlagWait)
{
    UINT32 uret;

    if (DrvRes != NULL) {

        if (FlagWait == 1U) {
            // fill information to wait for the command done
            DrvRes->DevCommandToWait   = XHCI_TRB_STOP_EP_CMD;
            DrvRes->FlagDevCommandWait = 1;
        }

        uret = USBCDNS3_EndpointStop(DrvRes, EpIndex);

        if ((FlagWait == 1U) && (uret == 0U)) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBDCDNS3_EndpointStop(): fail to wait semaphore");
            } else {
                //host_cmd_cmpl_endpoint_reset(DrvRes);
                DrvRes->DevCommandToWait   = 0;
                DrvRes->FlagDevCommandWait = 0;
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;
}

UINT32 USBDCDNS3_PortStatusRead(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortID)
{
    return USBHCDNS3_PortStatusRead(DrvRes, PortID);
}

UINT32 USBDCDNS3_TRDequeuePtrSet(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT32 FlagWait)
{
    UINT32 uret = 0;

    if (DrvRes != NULL) {
        XHC_CDN_PRODUCER_QUEUE_s       *transfer_obj = &DrvRes->EpQueueArray[EpIndex];
        const XHCI_RING_ELEMENT_s      *enqueue_ptr  = transfer_obj->EnqueuePtr;

        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_L2, "USBDCDNS3_TRDequeuePtrSet(): enque ptr = 0x%X",
                                 IO_UtilityPtrToU64Addr(enqueue_ptr));

        USBCDNS3_EpCtxTRDequeuePtrSet(transfer_obj);

        if (FlagWait == 1U) {
            // fill information to wait for the command done
            DrvRes->DevCommandToWait   = XHCI_TRB_SET_TR_DQ_PTR_CMD;
            DrvRes->FlagDevCommandWait = 1;
        }

        USBCDNS3_XhcCmdSetTrDequeuePtr(DrvRes, EpIndex, USBCDNS3_PtrToU64PhyAddr(&enqueue_ptr->DWord0));

        if (FlagWait == 1U) {
            uret = USB_UtilitySemaphoreTake(&DrvRes->SemaphoreCommandQueue, 3000);
            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBDCDNS3_TRDequeuePtrSet(): fail to wait semaphore");
            } else {
                //host_cmd_cmpl_enable_slot(DrvRes);
            }
        }
    } else {
        uret = USB_ERR_PARAMETER_INVALID;
    }

    return uret;

}



