/**
 *  @file USBCDNS3_Transfer.c
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
 * calculate number of bytes for last memory page
 * @param BuffAddr pointer to transfered data
 * @param Size size of data buffer
 * @return number of buffer for last memory page
 */
static UINT32 last_page_num_bytes_calc(UINT64 BuffAddr, UINT32 Size) {
    UINT32 num_of_bytes = (UINT32) ((BuffAddr + (UINT64) Size) % XHCI_SYSTEM_MEMORY_PAGE_SIZE);
    return num_of_bytes;
}


/**
 * calculate number of bytes for first memory page
 * @param PageStart address of first page
 * @param PageEnd address of last page
 * @param BuffAddr pointer to transfered data
 * @param Size size of data buffer
 * @return number of buffer for first memory page
 */
static UINT32 first_page_num_bytes_calc(UINT64 PageStart, UINT64 PageEnd, UINT64 BuffAddr, UINT32 Size) {
    UINT32 num_of_bytes;
    if (PageStart == PageEnd) {
        // whole TD is located on the same page
        num_of_bytes = Size;
    } else {
        // TD exceed single page
        num_of_bytes = (UINT32) (XHCI_SYSTEM_MEMORY_PAGE_SIZE - (BuffAddr % XHCI_SYSTEM_MEMORY_PAGE_SIZE));
    }
    return num_of_bytes;
}


static void td_all_mem_pages_index_create(USBCDNS3_TD_CREATE_PARAM_s * tdParams, const UINT64 BuffAddr, UINT32 const Size)
{

    // calculate number of memory pages used for this TD
    tdParams->PageStart = BuffAddr / XHCI_SYSTEM_MEMORY_PAGE_SIZE;

    // check if size is greater than zero
    if (Size > 0U) {
        tdParams->PageEnd = ((BuffAddr + (UINT64) Size) - 1U) / XHCI_SYSTEM_MEMORY_PAGE_SIZE;
    } else {
        tdParams->PageEnd = tdParams->PageStart;
    }
    tdParams->NumOfPages = (tdParams->PageEnd - tdParams->PageStart) + 1U;
}

static UINT32 single_trb_length_calc(UINT32 NumOfTrb, UINT32 DataLength, UINT64 BuffStart) {

    UINT32 single_trb_length;

    // if there is only one TRB size equals to dataLength for this page
    if (NumOfTrb == 1U) {
        single_trb_length = DataLength;
    } else {
        single_trb_length = (UINT32) (XHCI_TRB_MAX_TRANSFER_LENGTH - (BuffStart % XHCI_TRB_MAX_TRANSFER_LENGTH));
    }

    return single_trb_length;
}

/**
 * Function gets BurstCount for an endpoint
 * BurstCount is similar to MULT, i.e.
 * burstCount = (Max # of packets) / (actual burst size)
 * @param epObj
 */
static UINT32 ep_burst_count_get(const XHC_CDN_PRODUCER_QUEUE_s *XhcEpInfo, const USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{
    // get burst size from EP context
    UINT32 burst_size  = (XhcEpInfo->HWContext[1] & XHCI_EPCTX_MAX_BURST_SZ_MASK) >> XHCI_EPCTX_MAX_BURST_SZ_POS;
    UINT32 burst_count = (tdParams->TdPacketCount + burst_size + 1U) / (burst_size + 1U);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "ep_burst_count_get: %d", (burst_count - 1U), 0, 0, 0, 0);

    return (burst_count - 1U);
}

static UINT32 ep_burst_last_count_get(const XHC_CDN_PRODUCER_QUEUE_s *XhcEpInfo, const USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{
    // get burst size from EP context
    UINT32 burst_size  = (XhcEpInfo->HWContext[1] & XHCI_EPCTX_MAX_BURST_SZ_MASK) >> XHCI_EPCTX_MAX_BURST_SZ_POS;
    UINT32 residue;
    UINT32 uret;

    residue = tdParams->TdPacketCount % (burst_size + 1U);

    if (residue == 0U) {
        /*
         * If residue is zero, the last burst contains (max_burst + 1)
         * number of packets, but the TLBPC field is zero-based.
         */
        uret = burst_size;
    } else {
        uret = residue - 1U;
    }

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "ep_burst_last_count_get: %d", uret, 0, 0, 0, 0);

    return uret;
}


/**
 * Function updates TRB for isochronous endpoint
 * Note that in a TD, only first TRB will be marked ISO
 * @param ep endpoint object
 */
static void updateForIsoTrb(XHC_CDN_DRV_RES_s const *DrvRes, const XHC_CDN_PRODUCER_QUEUE_s *ep, const USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{

    UINT32 ete = DrvRes->RegistersQuickAccess.xHCCaps.HCSPARAMS2 & 0x100U; // Missing USBSSP__HCSPARAMS2__ETE_MASK
    UINT32 burstCount       = ep_burst_count_get(ep, tdParams);
    UINT32 last_burst_count = ep_burst_last_count_get(ep, tdParams);

    // get TRB's last DWORD
    UINT32 tempDword3 = ep->EnqueuePtr->DWord3;

    // set TRB type as isochronous
    UINT32 trbType = XHCI_TRB_ISOCH;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4,
                          "updateForIsoTrb(): hcsparams2(0x%X): ete(0x%X)\n",
                          DrvRes->RegistersQuickAccess.xHCCaps.HCSPARAMS2,
                          ete,
                          0, 0, 0);

    // check if FrameID is valid
    if ((ep->FrameID & 0x8000U) > 0U) {
        // reset FrameID
        tempDword3 &= ~((UINT32) 0x7FFU << XHCI_TRB_ISOCH_FRAME_ID_POS);
        tempDword3 &= ~((UINT32) 1 << XHCI_TRB_ISOCH_SIA_POS);
        // set FrameID
        tempDword3 |= ((ep->FrameID & 0x7FFU) << XHCI_TRB_ISOCH_FRAME_ID_POS);
    } else {
        // send frame at once when FrameID = 0;
        tempDword3 |= ((UINT32) 1 << XHCI_TRB_ISOCH_SIA_POS);
    }

    // clear TRB field
    tempDword3 &= ~(trbType << XHCI_TRB_TYPE_POS);

    // set ISO TRB field
    tempDword3 |= (trbType << XHCI_TRB_TYPE_POS);

    // clear TBC_TBSTs field
    tempDword3 &= ~(XHCI_TRB_TBC_TBSTS_MASK);

    if (ete == 0U) {

        // modify dword2

        UINT32 tempDword2 = ep->EnqueuePtr->DWord2;

        // clear TDSIZE_TBC
        tempDword2 &= ~(XHCI_TRB_TDSIZE_TBC_MASK);
        tempDword2 |= (burstCount & 0x1FU) << XHCI_TRB_TDSIZE_TBC_POS;

        ep->EnqueuePtr->DWord2 = tempDword2;

        // modify dword 3

        // set TBC (Transfer Burst Count, 2-bit, range [0-3])
        tempDword3 |= (burstCount & 0x3U) << XHCI_TRB_TBC_TBSTS_POS;
        // set TLBPC (Transfer Last Burst Packet Count, 4-bit, range [0-15])
        tempDword3 |= (last_burst_count & 0x0FU) << 16U;
    } else {
        UINT32 tempDword2 = ep->EnqueuePtr->DWord2;

        // Clear TDSIZE_TBC
        tempDword2 &= ~(XHCI_TRB_TDSIZE_TBC_MASK);
        tempDword2 |= (burstCount & 0x1FU) << XHCI_TRB_TDSIZE_TBC_POS;

        ep->EnqueuePtr->DWord2 = tempDword2;
    }
    // update TRB
    ep->EnqueuePtr->DWord3 = tempDword3;
}

static void updateEpObjToStream(XHC_CDN_PRODUCER_QUEUE_s **epObj)
{

    const XHC_CDN_PRODUCER_QUEUE_s *ep = *epObj;

    // if stream used, switch endpoint object to stream object
    if (ep->ActualSID > 0U) {
        // get data size from endpoint object
        UINT32 size = ep->NumOfBytes;
        *epObj = ep->StreamQueueArray[ep->ActualSID - 1U];
        ep->StreamQueueArray[ep->ActualSID - 1U]->NumOfBytes = size;
    }
}

/**
 * Calculate TD Size value of TRB field
 * @param singleTrbLength data length
 * @param tdParams pointer to extra parameters (used internally)
 * @return value of TD size (0-31)
 */
static UINT32 td_size_calculate(UINT32 singleTrbLength, USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{

    UINT32 tdSize;

    // calculate tdSize
    tdParams->TrbTransferLengthSum += singleTrbLength;

    if (tdParams->EpMaxPacketSize > 0U) {
        // round down PacketTransfered
        tdParams->PacketTransfered = tdParams->TrbTransferLengthSum / tdParams->EpMaxPacketSize;
    }

    // set tdSize to zero for the last TRB in TD
    if ((tdParams->IsLastBuffer == 1U) && (tdParams->IsLastTrb == 1U) && (tdParams->IsLastPage == 1U)) {
        tdSize = 0U;
    } else {
        tdSize = ((tdParams->TdPacketCount - tdParams->PacketTransfered) > 31U) ? 31U : (tdParams->TdPacketCount - tdParams->PacketTransfered);
    }
    return tdSize;
}


/**
 * Create single TRB in transfer ring
 * @param res driver resources
 * @param dataPtr address of data
 * @param singleTrbLength length of this data chunk
 * @param tdParams pointer to extra parameters (used internally)
 */
static void createSingleTrb(XHC_CDN_DRV_RES_s *DrvRes, UINT64 dataPtr, UINT32 singleTrbLength, USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{
    // default transfer TRB as normal
    UINT32 trbType = XHCI_TRB_NORMAL;
    UINT32 linkTrbChainFlag = XHCI_TRB_NORMAL_CH_MASK;

    // get endpoint object
    XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[tdParams->EpIndex];

    // set by default CHAIN flag
    UINT32 flags = XHCI_TRB_NORMAL_ISP_MASK | XHCI_TRB_NORMAL_CH_MASK;

    // calculate TD size
    UINT32 tdSize = td_size_calculate(singleTrbLength, tdParams);

    // switch to stream object if stream used
    updateEpObjToStream(&ep);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4,
                        "createSingleTrb(): IsLastTrb %d, IsLastPage %d, ExtraFlags 0x%X",
                        tdParams->IsLastTrb,
                        tdParams->IsLastPage,
                        ep->ExtraFlags, 0, 0);

    if ((tdParams->IsLastTrb == 1U) && (tdParams->IsLastPage == 1U)) {
        if ((ep->ExtraFlags & (UINT8) USBCDNS3_EFLAGS_FORCELINKTRB) == 0U) {
            if (ep->ActualSID == 0U) {
                flags = XHCI_TRB_NORMAL_IOC_MASK;
                linkTrbChainFlag = 0;
            } else {
                flags = XHCI_TRB_NORMAL_ENT_MASK | XHCI_TRB_NORMAL_CH_MASK;
            }
        }
    }

    // Create TRB
    USBCDNS3_U64ValueSet(
            &ep->EnqueuePtr->DWord0,
            &ep->EnqueuePtr->DWord1,
            USBCDNS3_CpuToLe64(dataPtr)
            );

    ep->EnqueuePtr->DWord2 = (ep->InterrupterIdx << XHCI_TRB_INTR_TRGT_POS)
            | (singleTrbLength & 0x0001FFFFU)
            | (tdSize << 17U);

    ep->EnqueuePtr->DWord3 =
            (trbType << XHCI_TRB_TYPE_POS)
            | flags
            | ((UINT32) ep->ToogleBit);

    // update TRB for iso transfer only in first TRB of TD
    if ((ep->epDesc[3] == CH9_USB_EP_ISOCHRONOUS) && (tdParams->IsFirstPage == 1U) && (tdParams->IsFirstTrb == 1U)) {
        updateForIsoTrb(DrvRes, ep, tdParams);
    }

    /* remember first TRB in TD */
    if ((tdParams->IsFirstPage == 1U) && (tdParams->IsFirstTrb == 1U)) {
        ep->FirstQueuedTRB = ep->EnqueuePtr;
    }

    // remember last TRB in TD
    ep->LastQueuedTRB = ep->EnqueuePtr;


    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                    "createSingleTrb(): address: 0x%X, length: %d, tdSize: %d, flags: %08X, TrbTransferLengthSum: %d",
                    dataPtr,
                    (UINT64)singleTrbLength,
                    (UINT64)tdSize,
                    (UINT64)flags,
                    (UINT64)tdParams->TrbTransferLengthSum);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4,
                    "createSingleTrb(): PacketTransfered: %d",
                    tdParams->PacketTransfered, 0, 0, 0, 0);

    USBCDNS3_QueuePointerUpdate(ep, linkTrbChainFlag, "EP.DATA.XFER.");
}
/* parasoft-end-suppress MISRA2012-RULE-8_13_a */


/**
 * Function creates TRBs for single memory page
 * @param res driver resources
 * @param dataPtr pointer to memory where actual data pointer is stored
 * @param dataLength data length
 * @param tdParams pointer to extra parameters (used internally)
 */
static void trbSinglePage(XHC_CDN_DRV_RES_s *DrvRes, UINT64 *dataPtr, UINT32 dataLength, USBCDNS3_TD_CREATE_PARAM_s * tdParams) {

    UINT32  trbIndex; // used as enumerator in for loop
    UINT64  buffStart = *dataPtr; // keeps original address of data start
    UINT64  endAddress = buffStart + (UINT64) dataLength;

    // calculate number of TRBs
    UINT32 numOfTrb = dataLength / XHCI_TRB_MAX_TRANSFER_LENGTH;

    tdParams->IsLastTrb = 0U;

    // round up number of packets
    if ((dataLength % XHCI_TRB_MAX_TRANSFER_LENGTH) > 0U) {
        ++numOfTrb;
    }

    // for data length = 0
    if (dataLength == 0U) {
        numOfTrb = 1U;
    }

    for (trbIndex = 0U; trbIndex < numOfTrb; trbIndex++) {

        // calculate date length of single TRB
        UINT32 singleTrbLength;

        // reset flag
        tdParams->IsFirstTrb = 0U;

        // for first TRB
        if (trbIndex == 0U) {

            // mark first TRB
            tdParams->IsFirstTrb = 1U;
            singleTrbLength = single_trb_length_calc(numOfTrb, dataLength, buffStart);

        } else if (trbIndex == (numOfTrb - 1U)) {
            // for last TRB
            singleTrbLength = (UINT32) (endAddress - (*dataPtr));
        } else {
            // elsewhere
            singleTrbLength = XHCI_TRB_MAX_TRANSFER_LENGTH;
        }

        // check if TRB is last
        if (trbIndex == (numOfTrb - 1U)) {
            tdParams->IsLastTrb = 1U;
        }

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "trbSinglePage(): TRB(%d):", trbIndex, 0, 0, 0, 0);

        // create single TRB
        createSingleTrb(DrvRes, *dataPtr, singleTrbLength, tdParams);

        // move data pointer
        (*dataPtr) += singleTrbLength;
    }
}


static void createTdAllMemPages(XHC_CDN_DRV_RES_s *DrvRes, USBCDNS3_TD_CREATE_PARAM_s * tdParams, const UINT64 buff, UINT32 const size)
{

    UINT64 pageIndex; // page enumerator

    // calculate first, last pages
    td_all_mem_pages_index_create(tdParams, buff, size);

    tdParams->IsLastPage = 0;

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                          "createTdAllMemPages(): <%d> PageStart: 0x%X, PageEnd: 0x%X, numOfpages: %d TdPacketCount: %d",
                          (UINT64)DrvRes->instanceNo,
                          tdParams->PageStart,
                          tdParams->PageEnd,
                          tdParams->NumOfPages,
                          (UINT64)tdParams->TdPacketCount);

    // create TRBs for every memory page
    for (pageIndex = 0; pageIndex < tdParams->NumOfPages; pageIndex++) {

        UINT32 numOfBytes;
        tdParams->IsFirstPage = 0U;

        // for first page
        if (pageIndex == 0U) {
            tdParams->IsFirstPage = 1U;
            numOfBytes = first_page_num_bytes_calc(tdParams->PageStart, tdParams->PageEnd, buff, size);
            // for last page
        } else if (pageIndex == (tdParams->NumOfPages - 1U)) {
            numOfBytes = last_page_num_bytes_calc(buff, size);
        } else {
            // for middle pages
            numOfBytes = (UINT32) XHCI_SYSTEM_MEMORY_PAGE_SIZE;
        }

        // check if page is last page in TD
        if (pageIndex == (tdParams->NumOfPages - 1U)) {
            tdParams->IsLastPage = 1U;
        }

        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                                 "createTdAllMemPages(): <%d> page(%d)",
                                 (UINT64)DrvRes->instanceNo,
                                 pageIndex,
                                 (UINT64)0,
                                 (UINT64)0,
                                 (UINT64)0);

        // create single page TRBS
        trbSinglePage(DrvRes, &tdParams->DataAddr, numOfBytes, tdParams);
    }
}

/**
 * Functions creates event data TRB
 * @param ep endpoint object
 * @param eventDataLo event data low DWORD
 * @param eventDataHi event data high DWORD
 * @param flags extra flags
 */
static void addEventDataTRB(XHC_CDN_PRODUCER_QUEUE_s *ep, UINT32 eventDataLo, UINT32 eventDataHi, UINT32 flags) {

    // create data event TRB
    ep->EnqueuePtr->DWord0 = USBCDNS3_CpuToLe32(eventDataLo);
    ep->EnqueuePtr->DWord1 = USBCDNS3_CpuToLe32(eventDataHi);
    ep->EnqueuePtr->DWord2 = USBCDNS3_CpuToLe32(0);
    ep->EnqueuePtr->DWord3 = USBCDNS3_CpuToLe32(
            ((UINT32)XHCI_TRB_EVENT_DATA << XHCI_TRB_TYPE_POS)
            | flags
            | (UINT32) ep->ToogleBit);
    USBCDNS3_QueuePointerUpdate(ep, 0U, "EP.EVENT_DATA_TRB.");
}

/**
 * Function returns max packet size for selected endpoint
 * @param epObj endpoint object
 * @return max packet size value
 */
static UINT16 getMaxPacketSize(const XHC_CDN_PRODUCER_QUEUE_s * epObj) {

    // get max packet size value from endpoint context
    UINT16 maxPacketSize = (UINT16) (epObj->HWContext[1] >> XHCI_EPCTX_MAX_PKT_SZ_POS);
    return (maxPacketSize);
}


/**
 * Function creates transfer descriptor in TRB ring
 * @param res driver resources
 * @param index endpoint index in device context
 * @param buff data user buffer
 * @param size data length of user buffer
 */
static void td_create(XHC_CDN_DRV_RES_s *DrvRes, UINT32 const index, const UINT64 buff, UINT32 const size, USBCDNS3_TD_CREATE_PARAM_s *tdInputParams)
{

    USBCDNS3_TD_CREATE_PARAM_s  *tdParams;
    USBCDNS3_TD_CREATE_PARAM_s   tdParamsAlloc;

    // get endpoint object
    XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[index];

    // check if tdParams is from external function
    if (tdInputParams != NULL) {
        tdParams = tdInputParams;
    } else {
        // create own tdParams object
        tdParams = &tdParamsAlloc;

        // set extra variables used for tdSize calculation
        tdParams->PacketTransfered = 0U;
        tdParams->TrbTransferLengthSum = 0U;
        tdParams->IsLastPage = 0U;
        tdParams->EpIndex = index;
        tdParams->IsLastBuffer = 1U; // only one user buffer is sent

        // get max packet size
        tdParams->EpMaxPacketSize = getMaxPacketSize(&DrvRes->EpQueueArray[index]);

        // calculate number of USB packets
        // first check if maxPacketSize > 0
        if (tdParams->EpMaxPacketSize == 0U) {
            tdParams->TdPacketCount = 1U;
        } else {
            tdParams->TdPacketCount = (size / tdParams->EpMaxPacketSize);

            // round up TdPacketCount
            if ((size % tdParams->EpMaxPacketSize) > 0U) {
                ++tdParams->TdPacketCount;
            }
        }
    }

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4, "td_create(): <%d> buff: %p, size: %d",
                             (UINT64)DrvRes->instanceNo,
                             buff,
                             (UINT64)size,
                             (UINT64)0,
                             (UINT64)0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "td_create(): <%d> Endpoint(index %d) maxPacketSize: %d", DrvRes->instanceNo, index, tdParams->EpMaxPacketSize, 0, 0);

    tdParams->DataAddr = buff;
    createTdAllMemPages(DrvRes, tdParams, buff, size);

    // if stream used add event data TRB at the end of whole TD
    if (ep->ActualSID > 0U) {
        UINT16 ActualSID = ep->ActualSID;
        ep = (XHC_CDN_PRODUCER_QUEUE_s *) ep->StreamQueueArray[ep->ActualSID - 1U];
        addEventDataTRB(ep, (UINT32) index, (UINT32) ActualSID, XHCI_TRB_NORMAL_IOC_MASK);
    }
}

static UINT32 transfer_data_doorbell(const XHC_CDN_DRV_RES_s *DrvRes, XHC_CDN_PRODUCER_QUEUE_s const *XhcEpInfo)
{

    UINT32 uret = 0;

    // get endpoint context index
    UINT32 epIndex = XhcEpInfo->ContextIndex;

    // get endpoint state
    UINT32 endpointState = USBCDNS3_EpStatusGet(DrvRes, epIndex);

    // handle not stalled endpoint
    if (endpointState != XHCI_EPCTX_STATE_HALTED) {

        UINT32 drblReg = ((UINT32) XhcEpInfo->ActualSID) << 16UL;
        drblReg |= (UINT32) epIndex;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "transfer_data_doorbell(): <%d> DRBL(%d)", DrvRes->instanceNo, epIndex, 0, 0, 0);

        // send clear feature to endpoint in host mode if endpoint halted
        //if ((DrvRes->DeviceModeFlag == USBCDNS3_MODE_HOST) && (ep->IsDisabledFlag != 0U)) {
        //    uret = USBCDNS3_EndpointFeatureSet(DrvRes, epIndex, 0);
        //}

        USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, drblReg);
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "transfer_data_doorbell(): <%d> ENDPOINT %d is in not RUNNING state, can not issue DOORBELL - current status: %d",
                        DrvRes->instanceNo, epIndex, endpointState, 0, 0);
        uret = USB_ERR_XHCI_EP_NOT_RUNNING;
    }
    return uret;
}

/**
 * Transfer data on given endpoint. This function is non-blocking type. The XHCI
 * operation result should be checked in complete callback function.
 *
 * @param DrvRes  [IN] driver resources
 * @param EpIndex [IN] index of endpoint according to xhci spec e.g for ep1out
              ep_index=2, for ep1in ep_index=3, for ep2out ep_index=4 end so on
 * @param Buff [IN/OUT] buffer for data to send or to receive
 * @param Size [IN] size of data in bytes
 * @param CompleteFunc [IN] pointer to complete callback function
 *
 */
UINT32 USBCDNS3_DataTransfer(XHC_CDN_DRV_RES_s *DrvRes,
        UINT32 EpIndex,
        UINT64 Buffer,
        UINT32 Size,
        XHC_CDN_COMPLETE_f CompleteFunc)
{
    // check parameters correctness
    UINT32 uret = USBSSP_TransferDataSF(DrvRes, EpIndex);

    if ((uret != 0U) || (DrvRes == NULL)) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                "USBCDNS3_DataTransfer(): Critical error! Wrong value in one of function parameters, 0x%X",
                uret, 0, 0, 0, 0);
    } else {

        /* check if transfers are enabled on this endpoint */
        uret = USBCDNS3_EpXferEnabledCheck(DrvRes, EpIndex);

        if (uret == 0U) {
            // get endpoint object
            XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[EpIndex];

            ep->CompleteFunc = CompleteFunc;
            ep->NumOfBytes = Size;
            ep->IsRunningFlag = 1;

            // create TRB's chain
            td_create(DrvRes, EpIndex, Buffer, Size, NULL);

            // set DRBL only on last packet in chain
            if ((ep->ExtraFlags & USBCDNS3_EFLAGS_NODORBELL) == 0U) {
                uret = transfer_data_doorbell(DrvRes, ep);
            }
        }
    }
    return uret;
}

/**
 * calculate data sum of all scatter/gather elements
 * @param SizeArray size vector
 * @param ElementConut number of scatter/gather elements
 * @return sum of data transfered in scatter/gather transfer
 */
static UINT32 calcSGDataLength(const UINT32 *SizeArray, UINT32 ElementConut) {

    UINT32 i;
    UINT32 wholeSGLength = 0U;

    // sum of all elements in size vector
    for (i = 0; i < ElementConut; i++) {
        wholeSGLength += SizeArray[i];
    }

    return wholeSGLength;
}

/**
 * calculate number of USB packets in whole transfer descriptor
 * @param tdParams
 * @param wholeSGLength
 */
static void calcTdPackNum(USBCDNS3_TD_CREATE_PARAM_s *tdParams, UINT32 wholeSGLength) {
    // calculate number of USB packets
    // first check if maxPacketSize > 0

    if (tdParams->EpMaxPacketSize == 0U) {
        tdParams->TdPacketCount = 1U;
    } else {
        tdParams->TdPacketCount = (wholeSGLength / tdParams->EpMaxPacketSize);

        // round up tdPacketCount
        if ((wholeSGLength % tdParams->EpMaxPacketSize) > 0U) {
            ++tdParams->TdPacketCount;
        }
    }
}


/**
 * Scatter/gather transfer function
 * @param res driver resources
 * @param epIndex endpoint index
 * @param BufferArray buffer for user data buffers
 * @param SizeArray buffer for user data buffer sizes
 * @param ElementConut number of user buffers
 * @param complete completion callback
 * @return CDN_EOK if success, error code elsewhere
 */
UINT32 USBCDNS3_DataTransfer2(XHC_CDN_DRV_RES_s *DrvRes,
                              UINT32 EpIndex,
                              USBCDNS3_SG_PARAM paramT,
                              XHC_CDN_COMPLETE_f CompleteFunc)
{

    // check input parameters correctness
    UINT32 uret = USBSSP_TransferData2SF(DrvRes, EpIndex);
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                "USBCDNS3_DataTransfer2(): <%d> Critical error! Wrong value in one of function parameters",
                DrvRes->instanceNo, 0, 0, 0, 0);
    } else {
        XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[EpIndex]; // get endpoint object
        UINT32 i;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "USBCDNS3_DataTransfer2(): <%d> Transfer data on ep: %d",
                DrvRes->instanceNo, EpIndex, 0, 0, 0);
        /* check if transfers are enabled on this endpoint */
        uret = USBCDNS3_EpXferEnabledCheck(DrvRes, EpIndex);

        if (uret == 0U) {
            const UINT64    *BufferArray = paramT.BufferArray;
            const UINT32    *SizeArray = paramT.SizeArray;
            UINT32           ElementConut = paramT.ElementConut;
            UINT32           wholeSGLength;
            USBCDNS3_TD_CREATE_PARAM_s tdParams;
            UINT32           drblReg;

            // set extra variables used for tdSize calculation
            tdParams.PacketTransfered = 0U;
            tdParams.TrbTransferLengthSum = 0U;
            tdParams.IsLastPage = 0U;
            tdParams.EpIndex = EpIndex;
            tdParams.IsLastBuffer = 0U;

            // get max packet size
            tdParams.EpMaxPacketSize = getMaxPacketSize(&DrvRes->EpQueueArray[EpIndex]);

            // calculate of whole length
            wholeSGLength = calcSGDataLength(SizeArray, ElementConut);

            // calculate number of packets
            calcTdPackNum(&tdParams, wholeSGLength);

            drblReg = ((UINT32) ep->ActualSID) << 16U;
            drblReg |= EpIndex;

            // set complete callback
            ep->CompleteFunc = CompleteFunc;
            ep->NumOfBytes = wholeSGLength; // ?

            // set extra flags, following TRB will be ISP and CH bits set
            ep->ExtraFlags = (UINT8) USBCDNS3_EFLAGS_FORCELINKTRB;

            // build TD with USBSSP_TransferData function with blocked DRBL
            for (i = 0; i < (ElementConut - 1U); i++) {
                td_create(DrvRes, EpIndex, BufferArray[i], SizeArray[i], &tdParams);
            }

            // put last TRB with IOC enabled: clear all extra flags
            ep->ExtraFlags = ~((UINT8) USBCDNS3_EFLAGS_FORCELINKTRB);
            tdParams.IsLastBuffer = 1U;
            td_create(DrvRes, EpIndex, BufferArray[i], SizeArray[i], &tdParams);
            USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, drblReg);
            ep->IsRunningFlag = 1;
            // start DRBL
        }
    }
    return uret;
}

static void iso_trb_single_page_create(XHC_CDN_DRV_RES_s *DrvRes, UINT64 dataPtr, UINT32 singleTrbLength, USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{
    // default transfer TRB as normal
    UINT32 trbType = XHCI_TRB_NORMAL;
    UINT32 linkTrbChainFlag = XHCI_TRB_NORMAL_CH_MASK;

    // get endpoint object
    XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[tdParams->EpIndex];

    // set by default CHAIN flag
    //UINT32 flags = XHCI_TRB_NORMAL_ISP_MASK | XHCI_TRB_NORMAL_CH_MASK;
    UINT32 flags = XHCI_TRB_NORMAL_CH_MASK;

    // calculate TD size
    UINT32 tdSize = td_size_calculate(singleTrbLength, tdParams);

    // switch to stream object if stream used
    updateEpObjToStream(&ep);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4,
                        "iso_trb_single_page_create(): IsLastTrb %d, IsLastPage %d, ExtraFlags 0x%X",
                        tdParams->IsLastTrb,
                        tdParams->IsLastPage,
                        ep->ExtraFlags, 0, 0);

    if ((tdParams->IsLastTrb == 1U) && (tdParams->IsLastPage == 1U)) {
        if ((ep->ExtraFlags & (UINT8) USBCDNS3_EFLAGS_FORCELINKTRB) == 0U) {
            if (ep->ActualSID == 0U) {
                flags = XHCI_TRB_NORMAL_IOC_MASK;
                linkTrbChainFlag = 0;
            } else {
                flags = XHCI_TRB_NORMAL_ENT_MASK | XHCI_TRB_NORMAL_CH_MASK;
            }
        }
    }

    // Create TRB
    USBCDNS3_U64ValueSet(
            &ep->EnqueuePtr->DWord0,
            &ep->EnqueuePtr->DWord1,
            USBCDNS3_CpuToLe64(dataPtr)
            );

    ep->EnqueuePtr->DWord2 = (ep->InterrupterIdx << XHCI_TRB_INTR_TRGT_POS)
            | (singleTrbLength & 0x0001FFFFU)
            | (tdSize << 17U);

    ep->EnqueuePtr->DWord3 =
            (trbType << XHCI_TRB_TYPE_POS)
            | flags
            | ((UINT32) ep->ToogleBit);

    // update TRB for iso transfer only in first TRB of TD
    if ((ep->epDesc[3] == CH9_USB_EP_ISOCHRONOUS) && (tdParams->IsFirstPage == 1U) && (tdParams->IsFirstTrb == 1U)) {
        if (tdParams->IsIsoTrbSet == 0U) {
            updateForIsoTrb(DrvRes, ep, tdParams);
            tdParams->IsIsoTrbSet = 1;
        }
    }

    /* remember first TRB in TD */
    if ((tdParams->IsFirstPage == 1U) && (tdParams->IsFirstTrb == 1U)) {
        ep->FirstQueuedTRB = ep->EnqueuePtr;
    }

    // remember last TRB in TD
    ep->LastQueuedTRB = ep->EnqueuePtr;


    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                    "iso_trb_single_page_create(): address: 0x%X, length: %d, tdSize: %d, flags: %08X, TrbTransferLengthSum: %d",
                    dataPtr,
                    (UINT64)singleTrbLength,
                    (UINT64)tdSize,
                    (UINT64)flags,
                    (UINT64)tdParams->TrbTransferLengthSum);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4,
                    "iso_trb_single_page_create(): PacketTransfered: %d",
                    tdParams->PacketTransfered, 0, 0, 0, 0);

    USBCDNS3_QueuePointerUpdate(ep, linkTrbChainFlag, "EP.DATA.XFER.");
}


static void iso_trb_single_page(XHC_CDN_DRV_RES_s *DrvRes, UINT64 *dataPtr, UINT32 dataLength, USBCDNS3_TD_CREATE_PARAM_s * tdParams)
{

    UINT32  trbIndex; // used as enumerator in for loop
    UINT64  buffStart = *dataPtr; // keeps original address of data start
    UINT64  endAddress = buffStart + (UINT64) dataLength;

    // calculate number of TRBs
    UINT32 numOfTrb = dataLength / XHCI_TRB_MAX_TRANSFER_LENGTH;

    tdParams->IsLastTrb = 0U;

    // round up number of packets
    if ((dataLength % XHCI_TRB_MAX_TRANSFER_LENGTH) > 0U) {
        ++numOfTrb;
    }

    // for data length = 0
    if (dataLength == 0U) {
        numOfTrb = 1U;
    }

    for (trbIndex = 0U; trbIndex < numOfTrb; trbIndex++) {

        // calculate date length of single TRB
        UINT32 singleTrbLength;

        // reset flag
        tdParams->IsFirstTrb = 0U;

        // for first TRB
        if (trbIndex == 0U) {

            // mark first TRB
            tdParams->IsFirstTrb = 1U;
            singleTrbLength = single_trb_length_calc(numOfTrb, dataLength, buffStart);

        } else if (trbIndex == (numOfTrb - 1U)) {
            // for last TRB
            singleTrbLength = (UINT32) (endAddress - (*dataPtr));
        } else {
            // elsewhere
            singleTrbLength = XHCI_TRB_MAX_TRANSFER_LENGTH;
        }

        // check if TRB is last
        if (trbIndex == (numOfTrb - 1U)) {
            tdParams->IsLastTrb = 1U;
        }

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "iso_trb_single_page(): TRB(%d):", trbIndex, 0, 0, 0, 0);

        // create single TRB
        iso_trb_single_page_create(DrvRes, *dataPtr, singleTrbLength, tdParams);

        // move data pointer
        (*dataPtr) += singleTrbLength;
    }
}


static void iso_td_all_pages_create(XHC_CDN_DRV_RES_s *DrvRes, USBCDNS3_TD_CREATE_PARAM_s * tdParams, const UINT64 buff, UINT32 const size)
{

    UINT64 pageIndex; // page enumerator

    // calculate first, last pages
    td_all_mem_pages_index_create(tdParams, buff, size);

    tdParams->IsLastPage = 0;

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                          "iso_td_all_pages_create(): <%d> PageStart: 0x%X, PageEnd: 0x%X, numOfpages: %d TdPacketCount: %d",
                          (UINT64)DrvRes->instanceNo,
                          tdParams->PageStart,
                          tdParams->PageEnd,
                          tdParams->NumOfPages,
                          (UINT64)tdParams->TdPacketCount);

    // create TRBs for every memory page
    for (pageIndex = 0; pageIndex < tdParams->NumOfPages; pageIndex++) {

        UINT32 numOfBytes;
        tdParams->IsFirstPage = 0U;

        // for first page
        if (pageIndex == 0U) {
            tdParams->IsFirstPage = 1U;
            numOfBytes = first_page_num_bytes_calc(tdParams->PageStart, tdParams->PageEnd, buff, size);
            // for last page
        } else if (pageIndex == (tdParams->NumOfPages - 1U)) {
            numOfBytes = last_page_num_bytes_calc(buff, size);
        } else {
            // for middle pages
            numOfBytes = (UINT32) XHCI_SYSTEM_MEMORY_PAGE_SIZE;
        }

        // check if page is last page in TD
        if (pageIndex == (tdParams->NumOfPages - 1U)) {
            tdParams->IsLastPage = 1U;
        }

        USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4,
                                 "iso_td_all_pages_create(): <%d> page(%d)",
                                 (UINT64)DrvRes->instanceNo,
                                 pageIndex,
                                 (UINT64)0,
                                 (UINT64)0,
                                 (UINT64)0);

        // create single page TRBS
        iso_trb_single_page(DrvRes, &tdParams->DataAddr, numOfBytes, tdParams);
    }
}


static void iso_td_create(XHC_CDN_DRV_RES_s *DrvRes, UINT32 const index, const UINT64 buff, UINT32 const size, USBCDNS3_TD_CREATE_PARAM_s *tdInputParams)
{

    USBCDNS3_TD_CREATE_PARAM_s  *tdParams;

    // check if tdParams is from external function
    tdParams = tdInputParams;

    USB_UtilityPrintUInt64_5(USB_PRINT_FLAG_L4, "iso_td_create(): <%d> buff: %p, size: %d",
                             (UINT64)DrvRes->instanceNo,
                             buff,
                             (UINT64)size,
                             (UINT64)0,
                             (UINT64)0);
    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L4, "iso_td_create(): <%d> Endpoint(index %d) maxPacketSize: %d", DrvRes->instanceNo, index, tdParams->EpMaxPacketSize, 0, 0);

    tdParams->DataAddr = buff;
    iso_td_all_pages_create(DrvRes, tdParams, buff, size);

}


/**
 * Scatter/gather transfer function for isochronous
 * @param res driver resources
 * @param epIndex endpoint index
 * @param complete completion callback
 * @return CDN_EOK if success, error code elsewhere
 */
UINT32 USBCDNS3_DataTransfer3(XHC_CDN_DRV_RES_s *DrvRes,
                              UINT32 EpIndex,
                              USBCDNS3_SG_PARAM paramT,
                              XHC_CDN_COMPLETE_f CompleteFunc)
{

    // check input parameters correctness
    UINT32 uret = USBSSP_TransferData2SF(DrvRes, EpIndex);
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                "USBCDNS3_DataTransfer3(): <%d> Critical error! Wrong value in one of function parameters",
                DrvRes->instanceNo, 0, 0, 0, 0);
    } else {
        XHC_CDN_PRODUCER_QUEUE_s *ep = &DrvRes->EpQueueArray[EpIndex]; // get endpoint object
        UINT32 i;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L2, "USBCDNS3_DataTransfer3(): <%d> Transfer data on ep: %d",
                DrvRes->instanceNo, EpIndex, 0, 0, 0);
        /* check if transfers are enabled on this endpoint */
        uret = USBCDNS3_EpXferEnabledCheck(DrvRes, EpIndex);

        if (uret == 0U) {
            const UINT64    *BufferArray = paramT.BufferArray;
            const UINT32    *SizeArray = paramT.SizeArray;
            UINT32           ElementConut = paramT.ElementConut;
            UINT32           wholeSGLength;
            USBCDNS3_TD_CREATE_PARAM_s tdParams;
            UINT32           drblReg;

            // set extra variables used for tdSize calculation
            tdParams.PacketTransfered = 0U;
            tdParams.TrbTransferLengthSum = 0U;
            tdParams.IsLastPage = 0U;
            tdParams.EpIndex = EpIndex;
            tdParams.IsLastBuffer = 0U;
            tdParams.IsIsoTrbSet = 0U;

            // get max packet size
            tdParams.EpMaxPacketSize = getMaxPacketSize(&DrvRes->EpQueueArray[EpIndex]);

            // calculate of whole length
            wholeSGLength = calcSGDataLength(SizeArray, ElementConut);

            // calculate number of packets
            calcTdPackNum(&tdParams, wholeSGLength);

            drblReg = ((UINT32) ep->ActualSID) << 16U;
            drblReg |= EpIndex;

            // set complete callback
            ep->CompleteFunc = CompleteFunc;
            ep->NumOfBytes = wholeSGLength; // ?

            // set extra flags, following TRB will be ISP and CH bits set
            ep->ExtraFlags = (UINT8) USBCDNS3_EFLAGS_FORCELINKTRB;

            // build TD with USBSSP_TransferData function with blocked DRBL
            for (i = 0; i < (ElementConut - 1U); i++) {
                iso_td_create(DrvRes, EpIndex, BufferArray[i], SizeArray[i], &tdParams);
            }

            // put last TRB with IOC enabled: clear all extra flags
            ep->ExtraFlags = ~((UINT8) USBCDNS3_EFLAGS_FORCELINKTRB);
            tdParams.IsLastBuffer = 1U;
            iso_td_create(DrvRes, EpIndex, BufferArray[i], SizeArray[i], &tdParams);

            if ((SG_FLAG_DOORBELL & paramT.Flags) == SG_FLAG_DOORBELL) {
                //USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_DataTransfer3(): doorbell.");
                USBCDNS3_DoorbellWrite(DrvRes, DrvRes->ActualDeviceSlot, drblReg);
                ep->IsRunningFlag = 1;
            } else {
                //USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_DataTransfer3(): NO doorbell.");
            }
            // start DRBL
        }
    }
    return uret;
}


