/**
 *  @file USBCDNS3_EndpointEnable.c
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
 * sets endpoint type field in endpoint context
 * @param EpQueue
 */
static void epctx_eptype_set(const XHC_CDN_PRODUCER_QUEUE_s *EpQueue)
{
    // this value reflects endpoint context bytes order
    UINT32 ep_type;

    // in endpoint descriptor, type is done on third byte, mask it with two less
    // significant bits
    UINT8  ep_desc_type = EpQueue->epDesc[3] & 0x03U;

    if (ep_desc_type == 0U) {
        ep_desc_type = 4U; // Control - Bidirectional
    } else if ((EpQueue->epDesc[2] & CH9_USB_EP_DIR_IN) != 0U) {
        // check if endpoint in or out
        // address is kept on second byte of endpoint descriptor
        // according to XHCI endpoint type coding convention, for IN extra 1 or fourth
        // position must be added for this direction
        ep_desc_type |= XHCI_EPCTX_DIR_IN;
    } else {
        // MISRA: do nothing for out descriptors
    }

    ep_type = ((UINT32) ep_desc_type << XHCI_EPCTX_TYPE_POS) & XHCI_EPCTX_TYPE_MASK;

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_L3,
                          "epctx_eptype_set(): <%d> epType: %d",
                          EpQueue->Parent->instanceNo,
                          ep_type >> XHCI_EPCTX_TYPE_POS,
                          0, 0, 0);

    EpQueue->HWContext[1] |= ep_type;
}

/**
 * Function sets MULT field in endpoint context structure
 * @param EpQueue
 */
static void epctx_mult_set(XHC_CDN_PRODUCER_QUEUE_s *EpQueue)
{
    const XHC_CDN_DRV_RES_s *drv_res = EpQueue->Parent;
    UINT8  mult = 0U;
    UINT32 lec;
    UINT32 actual_speed = drv_res->ActualSpeed;
    UINT8  ep_desc_type = EpQueue->epDesc[3] & 0x03U;

    AmbaMisra_TouchUnused(EpQueue);

    lec = USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_LEC_MASK,
                               XHCI_HCCPARAMS2_LEC_SHIFT,
                               drv_res->RegistersQuickAccess.xHCCaps.HCCPARAMS2);

    // if LEC bit = 0, mult should be set correctly
    // if LEC bit = 1, mult should be 0

    // this filed is different from only if Large ESIT Payload is not supported
    if (lec == 0U) {
        if ((actual_speed >= CH9_USB_SPEED_SUPER) && (ep_desc_type == CH9_USB_EP_ISOCHRONOUS)) {
            // check if isochronous endpoint companion descriptor does not exists
            if ((EpQueue->epDesc[10] & 0x80U) != 0x80U) {
                mult = EpQueue->epDesc[10] & 0x03U;
            }
        }
    }
    if (mult > 0U) {
        EpQueue->HWContext[0] |= (UINT32) mult << XHCI_EPCTX_MULT_POS;
    }
}

/**
 * Function initializes stream object
 * @param epObj endpoint object
 * @param stream stream object
 * @param iter iterator equal to (stream ID - 1)
 */
static void initStreamObj(XHC_CDN_PRODUCER_QUEUE_s const * epObj, XHC_CDN_PRODUCER_QUEUE_s * stream, UINT32 iter) {

    const XHC_CDN_DRV_RES_s *res = epObj->Parent;

    // initialize ring of stream
    stream->RingArray = &(res->XhciMemResources->StreamRing[epObj->ContextIndex - 2U][iter][0]);
    stream->EnqueuePtr = stream->RingArray;
    stream->DequeuePtr = stream->RingArray;
    stream->ContextIndex = epObj->ContextIndex; // All child stream objects have parent's (endpoint) ContextIndex
    stream->ToogleBit = 1U;
    stream->ActualSID = (UINT16) iter + 1U;
    stream->InterrupterIdx = 0U; // Set interrupter to default

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                          "initStreamObj(): <%d> Memory allocated for epIndex: %02X, streamID (%d) ring: 0x%X",
                          res->instanceNo, stream->ContextIndex, stream->ActualSID, IO_UtilityPtrToU32Addr(stream->RingArray), 0);

    // update stream context
    USBCDNS3_U64ValueSet(
            &epObj->RingArray[stream->ActualSID].DWord0,
            &epObj->RingArray[stream->ActualSID].DWord1,
            USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&stream->EnqueuePtr->DWord0) | 0x02UL | stream->ToogleBit) // SCT = 1, PRIMARY string, transfer ring, spec 6.2.4.1
            );

}

static void setMaxPStreams(XHC_CDN_PRODUCER_QUEUE_s *epObj) {

    UINT8 maxPStreams = 0U;
    const XHC_CDN_DRV_RES_s *drv_res = epObj->Parent;
    UINT8 epDescType = epObj->epDesc[3] & 0x03U;

    if (epDescType == CH9_USB_EP_BULK) {
        UINT8 hwMaxPStreams = (UINT8) USBCDNS3_U32BitsRead(
                            XHCI_HCCPARAMS1_MAXPSASIZE_MASK,
                            XHCI_HCCPARAMS1_MAXPSASIZE_SHIFT,
                            drv_res->RegistersQuickAccess.xHCCaps.HCCPARAMS1);
        UINT32 streamId;

        // maxPStreams should be set to minimal o three factors:
        // MAX_STREMS_PER_EP, hccparams1, companion descriptor
        // first check if driver allows to use full hardware stream number and limit if NO
        hwMaxPStreams = (hwMaxPStreams > AMBA_XHCI_MAX_STREMS_PER_EP) ? AMBA_XHCI_MAX_STREMS_PER_EP : hwMaxPStreams;

        // Then check if descriptor companion streams number exceeds hardware number and limit if Yes
        maxPStreams = epObj->epDesc[10] & 0x1FU;
        maxPStreams = (maxPStreams > hwMaxPStreams) ? hwMaxPStreams : maxPStreams;

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                              "setMaxPStreams(): <%d> MAX_PSA_SIZE: %d, maxPStreams: %d",
                              drv_res->instanceNo, hwMaxPStreams, maxPStreams, 0, 0);

        // ------------------- initialize streams ------------------
        if (maxPStreams > 0U) {
            if ((epObj->ContextIndex - 2U) < AMBA_XHCI_MAX_EP_NUM_STRM_EN) {
                epObj->StreamCount = AMBA_XHCI_STREAM_ARRAY_SIZE;
                for (streamId = 0; streamId < (AMBA_XHCI_STREAM_ARRAY_SIZE - 1U); streamId++) {

                    XHC_CDN_PRODUCER_QUEUE_s *stream;
                    XHC_CDN_PRODUCER_QUEUE_s(*streamObj)[AMBA_XHCI_MAX_EP_NUM_STRM_EN][AMBA_XHCI_STREAM_ARRAY_SIZE] = &drv_res->XhciMemResources->StreamMemoryPool;
                    // get reference to single stream object within stream container
                    epObj->StreamQueueArray[streamId] = &((*streamObj)[epObj->ContextIndex - 2U][streamId]);
                    stream = (XHC_CDN_PRODUCER_QUEUE_s *) epObj->StreamQueueArray[streamId];

                    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                                          "setMaxPStreams(): <%d> Memory allocated for stream (%d) object: %p",
                                          drv_res->instanceNo, streamId + 1U, IO_UtilityPtrToU32Addr(stream), 0, 0);

                    initStreamObj(epObj, stream, streamId);
                }
            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                                      "setMaxPStreams(): <%d> maxPStreams(%d) for unsupported endpoint (%d)",
                                      epObj->Parent->instanceNo, maxPStreams, (epObj->ContextIndex), 0, 0);
            }
        }

        if (maxPStreams > 0U) {
            epObj->HWContext[0] |= (UINT32) maxPStreams << XHCI_EPCTX_PMAXSTREAMS_POS;
        }
    }//if BULK
}

/**
 * Function sets interval field in endpoint context
 * @param epObj endpoint object
 */
static void epctx_interval_set(XHC_CDN_PRODUCER_QUEUE_s *EpQueue) {

    UINT8 interval = EpQueue->epDesc[6];
    UINT8 ep_desc_type = EpQueue->epDesc[3] & 0x03U;

    AmbaMisra_TouchUnused(EpQueue);

    // Convert interval to endpoint valid value (Table 65, section 6.2.3.6 of the xHCI Spec).
    switch (EpQueue->Parent->ActualSpeed) {

        case CH9_USB_SPEED_LOW:
            if (ep_desc_type == CH9_USB_EP_INTERRUPT) {
                // Table 65 - LS Interrupt - covert interval (1-255) to (3-10).
                interval = USBSSP_CalcFsLsEPIntrptInterval(interval);
            }
            // If none of the above leave interval unchanged as it should be zero already.
            break;

        case CH9_USB_SPEED_FULL:
            // Table 65 - FS Isoch. - convert interval (1-16) to (3-18) i.e. increment by 2.
            if (ep_desc_type == CH9_USB_EP_ISOCHRONOUS) {
                interval = interval + 2U;
            } else {
                if (ep_desc_type == CH9_USB_EP_INTERRUPT) {
                    // Table 65 - FS Interrupt - covert interval (1-255) to (3-10).
                    interval = USBSSP_CalcFsLsEPIntrptInterval(interval);
                }
            }
            // If none of the above leave interval unchanged.
            break;

        default:
            // Table 65 - SS or HS (Interrupt/Isoch) - convert interval (1-16) to (0-15) i.e. decrement by 1.
            // Bulk value will be left unchanged
            if (ep_desc_type != CH9_USB_EP_BULK) {
                if (interval > 0U) {
                    interval = interval - 1U;
                }
            }
            break;
    }
    if (interval > 0U) {
        EpQueue->HWContext[0] |= (UINT32) interval << XHCI_EPCTX_INTERVAL_POS;
    }
}

/**
 * Calculate bytes per interval in term of LEC parameter
 * @param EpQueue endpoint object
 * @param BytesPerInterval pointer to bytesPerInterval variable
 */
static void epctx_max_esit_payload_patch(XHC_CDN_PRODUCER_QUEUE_s const * EpQueue, UINT32 *BytesPerInterval)
{
    // read LEC parameter from capabilities
    UINT32 hccparams2 = EpQueue->Parent->RegistersQuickAccess.xHCCaps.HCCPARAMS2;
    UINT32 lec;

    lec = USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_LEC_MASK, XHCI_HCCPARAMS2_LEC_SHIFT, hccparams2);
    // update bytesPerInterval field depending on LED field
    if ((lec == 1U) && (*BytesPerInterval > (64U * 1024U))) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "epctx_max_esit_payload_patch(): <%d> WARNING: LEC(%d) bytesPerInterval(%d) > 64KB, Patching to 64KB",
                              EpQueue->Parent->instanceNo, lec, *BytesPerInterval, 0, 0);
        *BytesPerInterval = 0x10000U;
    } else if ((lec == 0U) && (*BytesPerInterval > (48U * 1024U))) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "epctx_max_esit_payload_patch(): <%d> WARNING: LEC(%d) bytesPerInterval(%d) > 48KB, Patching to 48KB\n",
                              EpQueue->Parent->instanceNo, lec, *BytesPerInterval, 0, 0);
        *BytesPerInterval = 0xC000U;
    } else {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                              "epctx_max_esit_payload_patch(): <%d> bytesPerInterval = 0x%X",
                              EpQueue->Parent->instanceNo, *BytesPerInterval, 0, 0, 0);
        // pass misra check
    }
}

/**
 * Sets Max ESIT payload for SS mode
 * @param epObj endpoint object
 * @param bytesPerInterval pointer to bytesPerInterval variable
 */
static void ssp_bytes_per_interval_get(XHC_CDN_PRODUCER_QUEUE_s const * EpQueue, UINT32 *BytesPerInterval)
{

    UINT8  ep_desc_type = EpQueue->epDesc[3] & 0x03U;
    UINT8  const *desc = EpQueue->epDesc;

    if (ep_desc_type == CH9_USB_EP_ISOCHRONOUS) {
        // check bmAttribute field in SuperSpeed Endpoint Companion Descriptor
        // if bit[7] is 1, then SuperSpeedPlus Isochronous Endpoint Companion descriptor is added
        if ((desc[10] & 0x80U) == 0x80U) {
            // we should get the SSP Isochronous Endpoint Companion Descriptor
            if ((desc[13] == USB_DESC_LENGTH_ISO_EP_COMP) &&
                (desc[14] == USB_DESC_TYPE_SSP_ISO_COMP)) {

                USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                                     "ssp_bytes_per_interval_get(): <%d> SSP Isochronous Endpoint companion found-",
                                     EpQueue->Parent->instanceNo, 0, 0, 0, 0);

                // bytes per interval should be described directly inside SSP Isochronous Endpoint Companion Descriptor
                *BytesPerInterval = ((UINT32) desc[20] << 24U) |
                                    ((UINT32) desc[19] << 16U) |
                                    ((UINT32) desc[18] << 8U)  |
                                     (UINT32) desc[17];

            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                                      "ssp_bytes_per_interval_get(): <%d> ERROR: Missing SSP Isochronous Endpoint Companion Descriptor",
                                      EpQueue->Parent->instanceNo, 0, 0, 0, 0);
                // pass misra check
            }
        } else {
            *BytesPerInterval = ((UINT32) desc[12] << 8U) | ((UINT32) desc[11]);
        }
    } else if (ep_desc_type == CH9_USB_EP_INTERRUPT) {

        // bytes 5 (11 when added endpoint descriptor ) and 6
        //(12 when added endpoint descriptor) of companion descriptor is
        // wBytesPerInterval value
        *BytesPerInterval = ((UINT32) desc[12] << 8U) | ((UINT32) desc[11]);

    } else {
        // required by MISRA
    }
}

/**
 * Get Bytes Per Interval value for Low Speed
 * @param epMaxPacketSize
 * @param epObj
 * @return uint32_t number of bytes per interval
 */
static UINT32 ls_bytes_per_interval_get(XHC_CDN_PRODUCER_QUEUE_s const *EpQueue, UINT16 EpMaxPacketSize)
{
    UINT32 bytes_per_interval = EpMaxPacketSize;

    if (EpMaxPacketSize > 8U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                              "ls_bytes_per_interval_get(): <%d> WARNING: epMaxPacketSize (%d) > 8",
                              EpQueue->Parent->instanceNo, EpMaxPacketSize, 0, 0, 0);
    }
    return bytes_per_interval;
}

/**
 * Get Bytes Per Interval value for Full Speed
 * @param EpMaxPacketSize
 * @param EpDescType
 * @return uint32_t number of bytes per interval
 */
static UINT32 fs_bytes_per_interval_get(UINT8 EpDescType, UINT16 EpMaxPacketSize)
{
    UINT32 bytes_per_interval = 0U;
    if ((EpDescType == CH9_USB_EP_ISOCHRONOUS) || (EpDescType == CH9_USB_EP_INTERRUPT)) {
        bytes_per_interval = EpMaxPacketSize;
    }
    return bytes_per_interval;
}

/**
 * Get Bytes Per Interval value for High Speed
 * @param EpMaxPacketSize
 * @param EpDescType
 * @param BurstValue
 * @return uint32_t number of bytes per interval
 */
static UINT32 hs_bytes_per_interval_get(UINT8 EpDescType, UINT32 EpMaxPacketSize, UINT32 BurstValue)
{
    UINT32 bytes_per_interval = 0U;
    if ((EpDescType == CH9_USB_EP_ISOCHRONOUS) || (EpDescType == CH9_USB_EP_INTERRUPT)) {
        bytes_per_interval = EpMaxPacketSize * (BurstValue + 1U);
    }
    return bytes_per_interval;
}

/**
 * Get Bytes Per Interval value for Super Speed
 * @param EpDescType
 * @param Desc
 * @return uint32_t number of bytes per interval
 */
static UINT32 ss_bytes_per_interval_get(UINT8 EpDescType, UINT8 const *Desc)
{
    UINT32 bytes_per_interval = 0U;

    if ((EpDescType == CH9_USB_EP_ISOCHRONOUS) || (EpDescType == CH9_USB_EP_INTERRUPT)) {
        // bytes 5 (11 when added endpoint descriptor ) and 6
        //(12 when added endpoint descriptor) of companion descriptor is
        // wBytesPerInterval value
        bytes_per_interval = ((UINT32) Desc[12] << 8U) | ((UINT32) Desc[11]);
    }
    return bytes_per_interval;
}

/**
 * Function sets max ESIT payload field in endpoint context
 * @param epObj endpoint object
 */
static void epctx_max_esit_payload_set(const XHC_CDN_PRODUCER_QUEUE_s * EpQueue)
{
    const UINT8 *desc = EpQueue->epDesc;
    UINT32 bytes_per_interval = 0U;
    UINT8  ep_desc_type       = EpQueue->epDesc[3] & 0x03U;
    UINT16 ep_max_packet_size = (((UINT16) desc[5] & 0x7U) << 8U) | (UINT16) desc[4];
    UINT8  burst_value        = (desc[5] & 0x18U) >> 3U;

    // calculate bytesPerInterval depending on different operating speed
    switch (EpQueue->Parent->ActualSpeed) {

        // for low speed
        case CH9_USB_SPEED_LOW:
            bytes_per_interval = ls_bytes_per_interval_get(EpQueue, ep_max_packet_size);
            break;

        // full speed
        case CH9_USB_SPEED_FULL:
            bytes_per_interval = fs_bytes_per_interval_get(ep_desc_type, ep_max_packet_size);
            break;

        // high speed
        case CH9_USB_SPEED_HIGH:
            bytes_per_interval = hs_bytes_per_interval_get(ep_desc_type, ep_max_packet_size, burst_value);
            break;

        // super speed
        case CH9_USB_SPEED_SUPER:
            bytes_per_interval = ss_bytes_per_interval_get(ep_desc_type, desc);
            break;

        // super speed plus
        case CH9_USB_SPEED_SUPER_PLUS:
            ssp_bytes_per_interval_get(EpQueue, &bytes_per_interval);
            break;

        default:
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                                  "epctx_max_esit_payload_set(): <%d> Incorrect speed value: %d!",
                                  EpQueue->Parent->instanceNo, EpQueue->Parent->ActualSpeed, 0, 0, 0);
            break;
    }

    epctx_max_esit_payload_patch(EpQueue, &bytes_per_interval);

    // sets BytesPerInterval in endpoint context
    if (bytes_per_interval > 0U) {
        EpQueue->HWContext[0] |= ((bytes_per_interval >> 16U) << XHCI_EPCTX_MAXESITPLD_HI_POS);
        EpQueue->HWContext[4] |= ((bytes_per_interval & 0xFFFFU) << XHCI_EPCTX_MAXESITPLD_LO_POS);
    }
}

/**
 * Function sets CErr field in endpoint context structure
 * @param epObj endpoint object
 */
static void epctx_cerr_set(XHC_CDN_PRODUCER_QUEUE_s * EpQueue)
{

    UINT8  ep_desc_type = EpQueue->epDesc[3] & 0x03U;
    UINT32 cerr = 0U;

    AmbaMisra_TouchUnused(EpQueue);

    // cerr field should be set to tree only for bulk and interrupt endpoint
    if (ep_desc_type != CH9_USB_EP_ISOCHRONOUS) {
        cerr = XHCI_EPCTX_3ERR;
    }

    if (cerr > 0U) {
        cerr <<= XHCI_EPCTX_CERR_POS;
        EpQueue->HWContext[1] |= cerr;
    }
}

/**
 * Function sets max burst value for SS and SSP mode (bMaxBurst value in SuperSpeed Endpoint Companion Descriptor )
 * @param Desc [IN] endpoint descriptor
 * @param MaxBurstSize [OUT] max burst value
 */
static void ss_max_burst_size_get(UINT8 const *Desc, UINT8 * MaxBurstSize)
{
    // in endpoint descriptor, type is done on third byte, mask it with two less
    // significant bits
    UINT8 ep_desc_type = Desc[3] & 0x03U;

    // check if super speed endpoint companion available
    if ((Desc[7] == USB_DESC_LENGTH_SS_EP_COMP) && (Desc[8] == USB_DESC_TYPE_SS_EP_COMP)) {

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "ss_max_burst_size_get(): SuperSpeed Endpoint companion found-", 0, 0, 0, 0, 0);

        *MaxBurstSize = Desc[9];

        if (ep_desc_type == CH9_USB_EP_INTERRUPT) {
            if (*MaxBurstSize > 2U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ss_max_burst_size_get(): WARNING: burst value for interrupt endpoint > 2");
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "ss_max_burst_size_get(): WARNING: Limit burst value to 2");
                *MaxBurstSize = 2;
            }
        }
    }
}

/**
 * Function gets max burst value for HS mode (mult value in endpoint descriptor)
 * @param Desc endpoint descriptor
 * @param MaxBurstSize max burst value
 */
static void hs_max_burst_size_get(UINT8 const *Desc, UINT8 *MaxBurstSize)
{
    UINT8 ep_desc_type = Desc[3] & 0x03U;

    // calculate burst value from wMaxPacketSize field for HS speed & periodic endpoints
    if ((ep_desc_type == CH9_USB_EP_INTERRUPT) || (ep_desc_type == CH9_USB_EP_ISOCHRONOUS)) {

        USB_UtilityPrint(USB_PRINT_FLAG_INFO, "hs_max_burst_size_get(): Periodic endpoint found, setting burst value-");

        *MaxBurstSize = (Desc[5] & 0x18U) >> 3U;

        if (*MaxBurstSize > 2U) {

            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "hs_max_burst_size_get(): WARNING: burst value for interrupt endpoint > 2");
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "hs_max_burst_size_get(): WARNING: Limit burst value to 2");

            *MaxBurstSize = 2U;
        }
    }
}


/**
 * Function sets max burst size field in endpoint context
 * @param EpQueue endpoint object
 */
static void epctx_max_burst_size_set(const XHC_CDN_PRODUCER_QUEUE_s * EpQueue) {

    UINT8        max_burst_size = 0U;
    const UINT8 *desc = EpQueue->epDesc;

    if (EpQueue->Parent->ActualSpeed >= CH9_USB_SPEED_SUPER) {
        ss_max_burst_size_get(desc, &max_burst_size);
    } else if (EpQueue->Parent->ActualSpeed == CH9_USB_SPEED_HIGH) {
        hs_max_burst_size_get(desc, &max_burst_size);
    } else {
        // required for MISRA
    }

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                          "epctx_max_burst_size_set(): <%d> bMaxBurst: %d",
                          EpQueue->Parent->instanceNo, max_burst_size, 0, 0, 0);

    if (max_burst_size > 0U) {
        EpQueue->HWContext[1] |= ((UINT32) max_burst_size << XHCI_EPCTX_MAX_BURST_SZ_POS) & XHCI_EPCTX_MAX_BURST_SZ_MASK;
    }
}

/**
 * Set endpoint context
 * @param[in] EpQueue endpoint object
 */
static void ep_ctx_set(XHC_CDN_PRODUCER_QUEUE_s *EpQueue)
{
    // setup context
    epctx_eptype_set(EpQueue);
    epctx_mult_set(EpQueue);
    setMaxPStreams(EpQueue);
    epctx_interval_set(EpQueue);
    epctx_max_esit_payload_set(EpQueue);
    epctx_cerr_set(EpQueue);
    epctx_max_burst_size_set(EpQueue);
}

/**
 * Function stores descriptors in endpoint object
 * @param res driver resources
 * @param epObj endpoint object
 * @param desc endpoint descriptor
 * @return CDN_EOK if for correct descriptor, error code elsewhere
 */
static UINT32 storeEpDesc(XHC_CDN_DRV_RES_s const *DrvRes, XHC_CDN_PRODUCER_QUEUE_s *epObj, UINT8 const *desc) {

    UINT32 uret = 0;
    UINT8  epDescType = desc[3] & 0x03U; // get endpoint attributes

    // store descriptor endpoint in endpoint object, will be used by upper layers
    USB_UtilityMemoryCopy(epObj->epDesc, desc, USB_DESC_LENGTH_ENDPOINT);
    if (DrvRes->ActualSpeed >= CH9_USB_SPEED_SUPER) {

        // first check if companion descriptor exists aligned in memory to
        // endpoint descriptor and return error if doesn't
        if (desc[USB_DESC_LENGTH_ENDPOINT] != USB_DESC_LENGTH_SS_EP_COMP) {
            uret = USB_ERR_XHCI_DEV_DESC_INVALID;

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                "storeEpDesc(): <%d> Endpoint companion descriptor does not exist for SSx device",
                DrvRes->instanceNo, 0, 0, 0, 0);

        } else {
            USB_UtilityMemoryCopy(&epObj->epDesc[USB_DESC_LENGTH_ENDPOINT], &desc[USB_DESC_LENGTH_ENDPOINT], USB_DESC_LENGTH_SS_EP_COMP);
        }
    }
    if ((DrvRes->ActualSpeed == CH9_USB_SPEED_SUPER_PLUS) && (uret == 0U) && (epDescType == CH9_USB_EP_ISOCHRONOUS)) {
        // check if isochronous endpoint companion descriptor exists in memory
        UINT32 descOffset = USB_DESC_LENGTH_ENDPOINT + USB_DESC_LENGTH_SS_EP_COMP;
        if (desc[descOffset] != USB_DESC_LENGTH_ISO_EP_COMP) {
            // device mode should support iso endpoint companion descriptor
            // for host mode it may happen that VIP doesn't support it
            if (DrvRes->DeviceModeFlag == USBCDNS3_MODE_DEVICE) {
                uret = USB_ERR_XHCI_DEV_DESC_INVALID;
            }

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                "storeEpDesc(): <%d> Isochronous endpoint companion descriptor does not exist for SSP device",
                DrvRes->instanceNo, 0, 0, 0, 0);

        } else {
            USB_UtilityMemoryCopy(&epObj->epDesc[descOffset], &desc[descOffset], USB_DESC_LENGTH_ISO_EP_COMP);
        }
    }
    return uret;
}

/**
 * Function sets max packet size field in endpoint context
 * @param epObj endpoint object
 */
static void setMaxPacketSize(const XHC_CDN_PRODUCER_QUEUE_s * epObj) {

    const UINT8 *desc = epObj->epDesc;

    // calculate max packet size from bytes: 4 and 5 written in Little endian
    UINT16 epMaxPacketSize = (((UINT16) desc[5] & 0x7U) << 8U) | (UINT16) desc[4];

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO,
                          "setMaxPacketSize(): <%d> epMaxPacketSize: %d",
                          epObj->Parent->instanceNo, epMaxPacketSize, 0, 0, 0);

    epObj->HWContext[1] |= (UINT32) epMaxPacketSize << XHCI_EPCTX_MAX_PKT_SZ_POS;
}

/**
 * Function sets average TRB length in endpoint context
 * @param epObj endpoint object
 */
static void setAverageTRBLength(XHC_CDN_PRODUCER_QUEUE_s * epObj) {

    UINT8  epDescType = epObj->epDesc[3] & 0x03U;
    UINT32 averageTRBLength = 0U;

    AmbaMisra_TouchUnused(epObj);

    // check endpoint transfer type
    switch (epDescType) {
        case CH9_USB_EP_ISOCHRONOUS:
            averageTRBLength = XHCI_EPCTX_ISO_AVGTRB_LEN;
            break;
        case CH9_USB_EP_INTERRUPT:
            averageTRBLength = XHCI_EPCTX_INT_AVGTRB_LEN;
            break;
        case CH9_USB_EP_BULK:
            averageTRBLength = XHCI_EPCTX_BULK_AVGTRB_LEN;
            break;
        default: // Assume control EP
            averageTRBLength = XHCI_EPCTX_CTRL_AVGTRB_LEN;
            break;
    }
    averageTRBLength <<= XHCI_EPCTX_EP_AVGTRBLEN_POS;
    epObj->HWContext[4] |= averageTRBLength;

}

/**
 * Configure and enable single endpoint
 * Note it doesn't issue configure endpoint command to Host
 * This function is called only for non EP0 endpoints
 * @param[in] res driver resources
 * @param[in] desc endpoint descriptor
 */
UINT32 USBCDNS3_EndpointEnable(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *desc)
{

    UINT32 uret;
    UINT8  epIn;
    UINT32 epIndex;
    XHC_CDN_PRODUCER_QUEUE_s *epObj;

    uret = USBSSP_EnableEndpointSF(DrvRes, desc);
    if (uret != 0U) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR,
                             "USBCDNS3_EndpointEnable(): <%d> Critical error! Wrong value in one of function parameters",
                              DrvRes->instanceNo, 0, 0, 0, 0);
    } else {
        UINT8 epAddress = desc[2]; // get endpoint address from descriptor
        UINT8 epDescType = desc[3] & 0x03U; // get endpoint attributes

        // calculate endpoint direction
        if (epDescType == CH9_USB_EP_CONTROL) {
            epIn = 1U; // For control endpoint flag should be set
        } else {
            //epIn = ((epAddress & CH9_USB_EP_DIR_IN) > 0U) ? (UINT8) 1U : (UINT8) 0U;
            if ((epAddress & CH9_USB_EP_DIR_IN) > 0U) {
                epIn = 1;
            } else {
                epIn = 0;
            }
        }

        // calculate endpoint index
        //epIndex = ((((epAddress & 0x7FU) - 1U) * 2U) + ((epIn > 0U) ? 1U : 0U));
        epIndex = ((UINT32)epAddress) & 0x07FU;
        epIndex = epIndex - 1U;
        epIndex = epIndex * 2U;
        if (epIn > 0U) {
            epIndex = epIndex + 1U;
        }

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
            "USBCDNS3_EndpointEnable(): epIn = %d, epIndex = %d, epAddress 0x%X, descType 0x%X",
            epIn, epIndex + XHCI_EPX_CONTEXT_OFFSET, epAddress, epDescType, 0);

        // get endpoint object from endpoint container
        epObj = &DrvRes->EpQueueArray[epIndex + XHCI_EPX_CONTEXT_OFFSET];

        // store context index
        epObj->ContextIndex = epIndex + XHCI_EPX_CONTEXT_OFFSET;

        // set endpoint object's hardware context
        epObj->HWContext = DrvRes->InputContext->EpContext[epIndex];

        // set endpoint parent
        epObj->Parent = DrvRes;

        // set interrupter to default interrupter(0)
        epObj->InterrupterIdx = 0U;

        uret = storeEpDesc(DrvRes, epObj, desc);

        if (uret == 0U) {
            ep_ctx_set(epObj);
            setMaxPacketSize(epObj);
            USBCDNS3_EpCtxTRDequeuePtrSet(epObj);
            setAverageTRBLength(epObj);

            // update input control context, checking 32 is required by Misra
            if (epObj->ContextIndex < 32U) {

                UINT32 leAddMask = USBCDNS3_CpuToLe32((UINT32) (1UL << epObj->ContextIndex));
                DrvRes->InputContext->InputControlContext[1] |= leAddMask;

                // If endpoint was previously registered to be dropped (Dx==1)_clearing Dx flag
                if ((DrvRes->InputContext->InputControlContext[0] & leAddMask) != 0U) {
                    DrvRes->InputContext->InputControlContext[0] &= ~leAddMask;
                }
            }

            // update slot context when required
            if (epObj->ContextIndex > DrvRes->ContextEntries) {
                DrvRes->ContextEntries = epObj->ContextIndex;
            }

            DrvRes->InputContext->SlotContext[0] = USBCDNS3_CpuToLe32(
                    ((UINT32) DrvRes->ContextEntries << XHCI_SLOTCTX_ENTRY_POS)
                    | ((UINT32) USBCDNS3_Ch9SpeedToXhciSpeed(DrvRes->ActualSpeed) << XHCI_SLOTCTX_SPEED_POS)
                    ); //6.2.2 set slot context entries and speed
            //if (DrvRes->usbsspCallbacks.inputContextCallback != NULL) {
            //    vDbgMsg(USBSSP_DBG_DRV, DBG_FYI, "<%d> Calling inputContextCallback()\n", DrvRes->instanceNo);
            //    DrvRes->usbsspCallbacks.inputContextCallback(DrvRes);
            //}
        }
    }
    return (uret);
}


