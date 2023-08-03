/**
 *  @file USBCDNS3_HostControlTransfer.c
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
 *  @details USB driver for Cadence USB host control transfer
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
 * Function creates TRB for setup request - only host mode
 * @param DrvRes     [IN] driver resources
 * @param Setup      [IN] pointer to setup packet
 * @param DataLength [IN] data length
 * @param Dir        [IN] data direction flag. 0: OUT, 1: IN
 */
static void host_ctrl_xfer_setup_enqueue(XHC_CDN_DRV_RES_s *DrvRes, const USB_CH9_SETUP_s* Setup, UINT16 DataLength, UINT32 Dir)
{
    UINT32 trt = 0;
    UINT32 dw0;
    UINT32 dw1;
    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_SETUP_STAGE;

    // setup TRB
    dw0 = (((UINT32)Setup->wValue) << XHCI_TRB_WVALUE_POS);
    dw0 |= (((UINT32) Setup->bRequest) << XHCI_TRB_BREQUEST_POS);
    dw0 |= ((UINT32) Setup->bmRequestType);

    DrvRes->Ep0Queue.EnqueuePtr->DWord0 = USBCDNS3_CpuToLe32(dw0);

    dw1 = (((UINT32) Setup->wLength) << XHCI_TRB_WLENGTH_POS);
    dw1 |= ((UINT32) Setup->wIndex);

    DrvRes->Ep0Queue.EnqueuePtr->DWord1 = USBCDNS3_CpuToLe32(dw1);

    DrvRes->Ep0Queue.EnqueuePtr->DWord2 = USBCDNS3_CpuToLe32((UINT32) 8U); // setup is always 8 bytes length

    if (DataLength > 0U) {
        if (Dir == 0U) {
            trt = XHCI_TRB_SETUP_TRT_OUT_DATA << XHCI_TRB_SETUP_TRT_POS;
        } else {
            trt = XHCI_TRB_SETUP_TRT_IN_DATA << XHCI_TRB_SETUP_TRT_POS;
        }
    }

    dw3 = (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (XHCI_TRB_SETUP_IDT_MASK);
    dw3 |= (DrvRes->Ep0Queue.ToogleBit & 0x01U);
    dw3 |= (trt);

    DrvRes->Ep0Queue.EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(dw3);

    USBCDNS3_QueuePointerUpdate(&DrvRes->Ep0Queue, 0U, "EP0.CTRL.XFER.SETUP.");
}

/**
 * function handles data phase of setup request in host mode
 * @param DrvRes     [IN] driver resources
 * @param pData      [IN] pointer to data
 * @param DataLength [IN] data length
 * @param Dir        [IN] data direction flag. 0: OUT, 1: IN
 */
static void host_ctrl_xfer_data_enqueue(XHC_CDN_DRV_RES_s *DrvRes, const UINT8 *pData, UINT16 DataLength, UINT32 Dir)
{
    UINT32 ioc_flag = (DrvRes->DeviceModeFlag == 0U) ? XHCI_TRB_NORMAL_IOC_MASK : 0U;
    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_DATA_STAGE;

    // set data buffer address
    USBCDNS3_U64ValueSet(
            &DrvRes->Ep0Queue.EnqueuePtr->DWord0,
            &DrvRes->Ep0Queue.EnqueuePtr->DWord1,
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(pData))
            );

    // set data length
    DrvRes->Ep0Queue.EnqueuePtr->DWord2 = USBCDNS3_CpuToLe32((UINT32) DataLength);

    // set flags
    dw3 = ((Dir & 0x01U) << XHCI_TRB_STS_TRANSFER_DIR_POS);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (ioc_flag);
    dw3 |= (DrvRes->Ep0Queue.ToogleBit & 0x01U);

    DrvRes->Ep0Queue.EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(dw3);

    USBCDNS3_QueuePointerUpdate(&DrvRes->Ep0Queue, 0U, "EP0.CTRL.XFER.DATA.");
}


/**
 * Enqueues non-blocking control transfer requests
 * It includes Setup, Data, Status stage TRBs
 * @param DrvRes [IN] driver resources
 * @param Setup  [IN] keeps setup packet
 * @param pData  [IN] pointer for data to send/receive
 */
static void host_ctrl_xfers_enqueue(XHC_CDN_DRV_RES_s *DrvRes, const USB_CH9_SETUP_s *Setup, const UINT8 *pData)
{
    UINT32 dir;
    UINT16 data_length;
    UINT8  data_in_flag = 0U;
    UINT32 dw3;
    UINT32 trb_type = XHCI_TRB_STATUS_STAGE;

    /* see spec in 4.11.2.2 */
    dir = ((UINT32)Setup->bmRequestType) & ((UINT32)CH9_USB_EP_DIR_IN);

    /* check data phase direction: set 1 for IN and 0 for OUT */
    if (dir > 0U) {
        dir = 1;
    } else {
        dir = 0;
    }

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
              "host_ctrl_xfers_enqueue(): <%d> start",
              DrvRes->instanceNo, 0, 0, 0, 0);

    data_length = Setup->wLength;

    /* handle setup stage without IOC */
    host_ctrl_xfer_setup_enqueue(DrvRes, Setup, data_length, dir);

    /* data TRB when exists */
    if (data_length > 0U) {

        /* handle data stage without IOC */
        host_ctrl_xfer_data_enqueue(DrvRes, pData, data_length, dir);
        if (dir > 0U) {
            data_in_flag = 1U;
        }
    }

    if (data_in_flag > 0U) {
        dir = 0;
    } else {
        dir = 1;
    }

    /* status TRB - This is the only TRB with IOC */
    DrvRes->Ep0Queue.EnqueuePtr->DWord0 = 0;
    DrvRes->Ep0Queue.EnqueuePtr->DWord1 = 0;
    DrvRes->Ep0Queue.EnqueuePtr->DWord2 = 0;

    dw3 = ((dir & 0x01U) << XHCI_TRB_STS_TRANSFER_DIR_POS);
    dw3 |= (trb_type << XHCI_TRB_TYPE_POS);
    dw3 |= (XHCI_TRB_NORMAL_IOC_MASK);
    dw3 |= (DrvRes->Ep0Queue.ToogleBit& 0x01U);

    DrvRes->Ep0Queue.EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(dw3);

    USBCDNS3_QueuePointerUpdate(&DrvRes->Ep0Queue, 0U, "EP0.CTRL.STATUS_STAGE.");

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
        "host_ctrl_xfers_enqueue(): <%d> DRBL: Ring doorbell on EP0",
        DrvRes->instanceNo, 0, 0, 0, 0);

    USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, DrvRes->Ep0Queue.ContextIndex);
}


/**
 * Control transfer. Function executes control transfer. Information about transfer
 * like: data direction, data length, wIndex, wValue etc. are passed in 'setup'
 * parameter. No blocking version, result is returned to callback function
 *
 * @param DrvRes [IN] driver resources
 * @param Setup  [IN] keeps setup packet
 * @param pData  [IN] pointer for data to send/receive
 * @param CompleteFunc [IN] callback function for transfer complete
 *
 */
static UINT32 host_ctrl_xfers_non_blocking(XHC_CDN_DRV_RES_s *DrvRes, USB_CH9_SETUP_s const *Setup, UINT8 const *pData, XHC_CDN_COMPLETE_f CompleteFunc)
{

    UINT32 uret;

    uret = USBSSP_NBControlTransferSF(DrvRes, Setup);
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
            "host_ctrl_xfers_non_blocking(): <%d> Critical error! Wrong value in one of function parameters",
            DrvRes->instanceNo, 0, 0, 0, 0);
    } else {

        /* check if transfers are enabled on this endpoint */
        uret = USBCDNS3_EpXferEnabledCheck(DrvRes, 1);
    }

    if (uret == 0U) {
        DrvRes->Ep0Queue.CompleteFunc = CompleteFunc;
        DrvRes->Ep0Queue.IsRunningFlag = 1;
        host_ctrl_xfers_enqueue(DrvRes, Setup, pData);
    }

    return uret;
}

static void host_ctrl_xfer_complete_func(XHC_CDN_DRV_RES_s *Arg, UINT32 SlotID, UINT32 EpIndex, UINT32 Status, const XHCI_RING_ELEMENT_s *EventPtr, UINT8 *Buffer, UINT32 ActualLength)
{
    UINT32 trb_type;

    (void)SlotID;
    (void)EpIndex;
    (void)Status;
    (void)EventPtr;
    (void)ActualLength;

    AmbaMisra_TouchUnused(Buffer);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "host_ctrl_xfer_complete_func(): SlotID %d, Status %d", SlotID, Status, 0, 0, 0);

    if (Arg != NULL) {
        // we have to save current Event data for later use
        Arg->SavedEp0Event = *EventPtr;
        trb_type = USBCDNS3_TrbTypeGet(Arg->Ep0Queue.DequeuePtr);

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "host_ctrl_xfer_complete_func(): trb_type %d", trb_type, 0, 0, 0, 0);

        // we only want to handle status stage TRB
        if (trb_type == XHCI_TRB_STATUS_STAGE) {
            if (USB_UtilitySemaphoreGive(&Arg->SemaphoreEp0Transfer) != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "host_ctrl_xfer_complete_func(): failed to give semaphore");
            }
        }
    }
}

/**
 * Control transfer. Function executes control transfer. Information about transfer
 * like: data direction, data length, wIndex, wValue etc. are passed in 'setup'
 * parameter.
 *
 * Caller must have different context with ISR thread.
 *
 * @param DrvRes [IN] driver resources
 * @param Setup  [IN] keeps setup packet
 * @param pData  [IN] pointer for data to send/receive
 *
 * @return 0 on success
 */
UINT32 USBHCDNS3_ControlTransfer(XHC_CDN_DRV_RES_s *DrvRes, USB_CH9_SETUP_s const *Setup, UINT8 const *pData)
{

    UINT32 uret = USBSSP_ControlTransferSF(DrvRes, Setup);

    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                        "USBHCDNS3_ControlTransfer(): <%d> Critical error! wrong value in one of function parameters.",
                        DrvRes->instanceNo, 0, 0, 0, 0);
    } else {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "USBHCDNS3_ControlTransfer(): <%d> Connected %d, DevAddress %d",
                              DrvRes->instanceNo,
                              DrvRes->Connected,
                              DrvRes->DevAddress,
                              0, 0);

        if ((DrvRes->Connected == 0U) || (DrvRes->DevAddress == 0U)) {
            uret = USB_ERR_XHCI_NO_CONNECT;
        }

        if (uret == 0U) {
            uret = host_ctrl_xfers_non_blocking(DrvRes, Setup, pData, host_ctrl_xfer_complete_func);
            if (uret != 0U) {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                        "USBHCDNS3_ControlTransfer(): <%d> Critical error! wrong value in one of function parameters",
                        DrvRes->instanceNo, 0, 0, 0, 0);
            } else {
                // wait for response
                if (USB_UtilitySemaphoreTake(&DrvRes->SemaphoreEp0Transfer, USBCDNS3_DEFAULT_TIMEOUT) != 0U) {
                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                        "USBHCDNS3_ControlTransfer(): <%d> failed to wait transfer done.",
                        DrvRes->instanceNo, 0, 0, 0, 0);
                }
            }
        }

        if (uret == 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
                    "USBHCDNS3_ControlTransfer(): <%d> CONTROL TRANSFER completed.",
                    DrvRes->instanceNo, 0, 0, 0, 0);

            // check result and translate from XHCI to Cadence error code
            uret = USBCDNS3_TrbCmplCodeGet(DrvRes->Ep0Queue.CompletePtr);

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1, "USBHCDNS3_ControlTransfer(): <%d> Completion Code: %d",
                    DrvRes->instanceNo, uret, 0, 0, 0);

            if (uret == XHCI_TRB_CMPL_SUCCESS) {
                uret = USB_ERR_SUCCESS;
            }
        }
    }
    return uret;
}


