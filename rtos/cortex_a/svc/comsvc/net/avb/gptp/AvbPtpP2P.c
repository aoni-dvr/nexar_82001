/**
 *  @file AvbPtpP2P.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details AVB gptp Peer to Peer networks stack
 *
 */

#include "AmbaKAL.h"

#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "./AvbPtp.h"

static AMBA_KAL_SEMAPHORE_t PtpPeerSem[AVB_STACK_INSTANCES];

void AvbPtpPeerInit(UINT32 Idx)
{
    static char AvbPtpPeerName[16] = "AvbPtpPeer";
    if (PtpPeerSem[Idx].tx_semaphore_id == 0U) {
        if (OK != AmbaKAL_SemaphoreCreate(&PtpPeerSem[Idx], AvbPtpPeerName, 1)) {
            AvbPtpInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaKAL_SemaphoreCreate failed");
        }
    }
}

UINT32 AvbPtpRxPDlyReq(UINT32 Idx, const void *frame, UINT16 framelen)
{
    const PTP_PDLYREQ_s *pdlyreq;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];

    (void)framelen;
    AmbaMisra_TypeCast(&pdlyreq, &frame);
    (void)AmbaEnet_GetRxTs(Idx, &pTsProc->Pt2pssv.Sec, &pTsProc->Pt2pssv.Ns);

    if (AmbaWrap_memcpy(pTsProc->PClkId, pdlyreq->ptphdr.clockId, sizeof(pTsProc->PClkId))!= 0U) { }
    pTsProc->PPortId = AmbaNetStack_Ntohs(pdlyreq->ptphdr.sourcePortId);
    pTsProc->Pseq = AmbaNetStack_Ntohs(pdlyreq->ptphdr.sequenceId);

    return 0;
}

UINT32 AvbPtpTxPDlyReq(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    void *ptr;
    PTP_PDLYREQ_s *pdlyreq;
    UINT16 framelen = (UINT16)sizeof(PTP_PDLYREQ_s);
    UINT16 msglen = (UINT16)sizeof(PTP_PDLYREQ_s);
    static UINT16 PDlyReqSeq = 0;
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP | PTP2_PATH_DELAY_REQ);
    ETH_HDR_s *ethhdr;

    (void)AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);
    AmbaMisra_TypeCast(&ethhdr, &ptr);
    if (AmbaWrap_memcpy(ethhdr->dst, PTP2_GPTP_DST, 6)!= 0U) { }
    if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
    (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

    AmbaMisra_TypeCast(&pdlyreq, &ptr);
    pdlyreq->ptphdr.messageType = msgtype;
    pdlyreq->ptphdr.versionPTP = 2;
    pdlyreq->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
    pdlyreq->ptphdr.domainNumber = 0x0U;
    pdlyreq->ptphdr.flagField = AmbaNetStack_Htons(0x0U);
    pdlyreq->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
    pdlyreq->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
    pdlyreq->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
    pdlyreq->ptphdr.clockId[3] = 0xff;
    pdlyreq->ptphdr.clockId[4] = 0xfe;
    if (AmbaWrap_memcpy(pdlyreq->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
    if (AmbaWrap_memcpy(&pdlyreq->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
    pdlyreq->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
    pdlyreq->ptphdr.sequenceId = AmbaNetStack_Htons(PDlyReqSeq);
    PDlyReqSeq++;
    pdlyreq->ptphdr.controlField = PTP1_OTHER;
    pdlyreq->ptphdr.logMessageInterval = PTP2_PDLYREQ_INTERVAL;

    pdlyreq->HighSec =  AmbaNetStack_Htons(0x0);
    pdlyreq->Sec =  AmbaNetStack_Htonl(0x0);
    pdlyreq->Ns =  AmbaNetStack_Htonl(0x0);

    /* wait current Pt2~Pt4 collected before trigger next Pt1 */
    AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] PDelay_Req");
    if (AmbaKAL_SemaphoreTake(&PtpPeerSem[Idx], 0x0U) != 0U) {
        AvbDebug("ENET %s(%d) No gPTP peer", __func__, __LINE__);
    }
    (void)pAvbConfig->pDoTxWaitCb(Idx, framelen, &pTsProc->Pt1.Sec, &pTsProc->Pt1.Ns);

    return 0;
}

UINT32 AvbPtpRxPDlyResp(UINT32 Idx, const void *frame, UINT16 framelen)
{
    const PTP_RESP_s *pdlyresp;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];

    (void)framelen;
    (void)AmbaEnet_GetRxTs(Idx, &pTsProc->Pt4.Sec, &pTsProc->Pt4.Ns);
    AmbaMisra_TypeCast(&pdlyresp, &frame);
    pTsProc->Pt2.Sec = AmbaNetStack_Ntohl(pdlyresp->Sec);
    pTsProc->Pt2.Ns = AmbaNetStack_Ntohl(pdlyresp->Ns);

    return 0;
}

UINT32 AvbPtpTxPDlyResp(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    void *ptr;
    PTP_RESP_s *pdlyresp;
    UINT16 framelen = (UINT16)sizeof(PTP_RESP_s);
    UINT16 msglen = (UINT16)sizeof(PTP_RESP_s);
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP | PTP2_PATH_DELAY_RESP);
    ETH_HDR_s *ethhdr;

    (void)AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);
    AmbaMisra_TypeCast(&ethhdr, &ptr);
    if (AmbaWrap_memcpy(ethhdr->dst, PTP2_GPTP_DST, 6)!= 0U) { }
    if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
    (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

    AmbaMisra_TypeCast(&pdlyresp, &ptr);
    pdlyresp->ptphdr.messageType = msgtype;
    pdlyresp->ptphdr.versionPTP = 2;
    pdlyresp->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
    pdlyresp->ptphdr.domainNumber = 0x0U;
    pdlyresp->ptphdr.flagField = AmbaNetStack_Htons(PTP2_FLAGS_PTP_TWO_STEP);
    pdlyresp->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
    pdlyresp->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
    pdlyresp->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
    pdlyresp->ptphdr.clockId[3] = 0xff;
    pdlyresp->ptphdr.clockId[4] = 0xfe;
    if (AmbaWrap_memcpy(pdlyresp->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
    if (AmbaWrap_memcpy(&pdlyresp->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
    pdlyresp->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
    pdlyresp->ptphdr.sequenceId = AmbaNetStack_Htons(pTsProc->Pseq);
    pdlyresp->ptphdr.controlField = PTP1_OTHER;
    pdlyresp->ptphdr.logMessageInterval = PTP2_FOREVER_INTERVAL;

    pdlyresp->HighSec =  AmbaNetStack_Htons(0x0);
    pdlyresp->Sec =  AmbaNetStack_Htonl(pTsProc->Pt2pssv.Sec);
    pdlyresp->Ns =  AmbaNetStack_Htonl(pTsProc->Pt2pssv.Ns);
    if (AmbaWrap_memcpy(pdlyresp->clockId, pTsProc->PClkId, sizeof(pTsProc->PClkId))!= 0U) { }
    pdlyresp->sourcePortId = AmbaNetStack_Ntohs(pTsProc->PPortId);
    AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] PDelay_Resp");
    (void)pAvbConfig->pDoTxWaitCb(Idx, framelen, &pTsProc->Pt3pssv.Sec, &pTsProc->Pt3pssv.Ns);

    return 0;
}

UINT32 AvbPtpRxPDlyFu(UINT32 Idx, const void *frame, UINT16 framelen)
{
    const PTP_RESP_s *pdlyresp;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    INT64 sec, myoffset, path;
    INT64 Pt1Sec = (INT64)pTsProc->Pt1.Sec;
    INT64 Pt1Ns =  (INT64)pTsProc->Pt1.Ns;
    INT64 Pt2Sec = (INT64)pTsProc->Pt2.Sec;
    INT64 Pt2Ns =  (INT64)pTsProc->Pt2.Ns;
    INT64 Pt3Sec;
    INT64 Pt3Ns;
    INT64 Pt4Sec = (INT64)pTsProc->Pt4.Sec;
    INT64 Pt4Ns =  (INT64)pTsProc->Pt4.Ns;

    (void)framelen;
    AmbaMisra_TypeCast(&pdlyresp, &frame);
    pTsProc->Pt3.Sec = AmbaNetStack_Ntohl(pdlyresp->Sec);
    Pt3Sec = (INT64)pTsProc->Pt3.Sec;
    pTsProc->Pt3.Ns = AmbaNetStack_Ntohl(pdlyresp->Ns);
    Pt3Ns =  (INT64)pTsProc->Pt3.Ns;

    /**
     * Pdelay
     *  My (PDlyReq Sender A) offset = ((Pt4-Pt3) - (Pt2-Pt1))/2
     *  B-to-A path delay = Pt4 - ATimeOffset -Pt3
     * Not_Used
     *  Peer offset = ((Pt2-Pt1) - (Pt4-Pt3))/2
     *  pathDelay   to peer = (Pt2 - Peer offset) - Pt1
    */
    sec       = Pt4Sec;
    sec      -= Pt3Sec;
    sec      -= Pt2Sec;
    sec      += Pt1Sec;
    myoffset  = sec * (INT64)NS_PER_SECOND;
    myoffset += Pt4Ns;
    myoffset -= Pt3Ns;
    myoffset -= Pt2Ns;
    myoffset += Pt1Ns;
    myoffset /= 2;

    sec       = Pt4Sec;
    sec      -= Pt3Sec;
    path      = sec * (INT64)NS_PER_SECOND;
    path     += Pt4Ns;
    path     -= Pt3Ns;
    path     -= myoffset;

    if (path >= 0) {
        (void) AvbPtpNsToTs((UINT64)path, &pTsProc->Pdelay);
    } else {
        AvbDebug("!path     %19lld", path);
    }
    if (pTsProc->Pdelay.Sec != 0U) {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, "ErrPdelay %10u.%09u", pTsProc->Pdelay.Sec, pTsProc->Pdelay.Ns, 0U, 0U, 0U);
    }

    (void) AmbaKAL_SemaphoreGive(&PtpPeerSem[Idx]);

    AvbDebug(" Pt1     %10u.%09u", pTsProc->Pt1.Sec, pTsProc->Pt1.Ns);
    AvbDebug(" Pt2     %10u.%09u", pTsProc->Pt2.Sec, pTsProc->Pt2.Ns);
    AvbDebug(" Pt3     %10u.%09u", pTsProc->Pt3.Sec, pTsProc->Pt3.Ns);
    AvbDebug(" Pt4     %10u.%09u", pTsProc->Pt4.Sec, pTsProc->Pt4.Ns);
    AvbDebug(" Pdelay  %10u.%09u", pTsProc->Pdelay.Sec, pTsProc->Pdelay.Ns);
    AvbDebug(" myoffset     %15lld", myoffset);

    return 0;
}

UINT32 AvbPtpTxPDlyFu(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    const AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    void *ptr;
    PTP_RESP_s *pdlyresp;
    UINT16 framelen = (UINT16)sizeof(PTP_RESP_s);
    UINT16 msglen = (UINT16)sizeof(PTP_RESP_s);
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP | PTP2_PATH_DELAY_FOLLOWUP);
    ETH_HDR_s *ethhdr;

    (void)AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);
    AmbaMisra_TypeCast(&ethhdr, &ptr);
    if (AmbaWrap_memcpy(ethhdr->dst, PTP2_GPTP_DST, 6)!= 0U) { }
    if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
    (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

    AmbaMisra_TypeCast(&pdlyresp, &ptr);
    pdlyresp->ptphdr.messageType = msgtype;
    pdlyresp->ptphdr.versionPTP = 2;
    pdlyresp->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
    pdlyresp->ptphdr.domainNumber = 0x0U;
    pdlyresp->ptphdr.flagField = AmbaNetStack_Htons(0x0U);
    pdlyresp->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
    pdlyresp->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
    pdlyresp->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
    pdlyresp->ptphdr.clockId[3] = 0xff;
    pdlyresp->ptphdr.clockId[4] = 0xfe;
    if (AmbaWrap_memcpy(pdlyresp->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
    if (AmbaWrap_memcpy(&pdlyresp->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
    pdlyresp->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
    pdlyresp->ptphdr.sequenceId = AmbaNetStack_Htons(pTsProc->Pseq);
    pdlyresp->ptphdr.controlField = PTP1_OTHER;
    pdlyresp->ptphdr.logMessageInterval = PTP2_FOREVER_INTERVAL;

    pdlyresp->HighSec =  AmbaNetStack_Htons(0x0);
    pdlyresp->Sec =  AmbaNetStack_Htonl(pTsProc->Pt3pssv.Sec);
    pdlyresp->Ns =  AmbaNetStack_Htonl(pTsProc->Pt3pssv.Ns);
    if (AmbaWrap_memcpy(pdlyresp->clockId, pTsProc->PClkId, sizeof(pTsProc->PClkId))!= 0U) { }
    pdlyresp->sourcePortId = AmbaNetStack_Ntohs(pTsProc->PPortId);
    AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] Pdelay_Resp_Follow_Up");
    (void)pAvbConfig->pDoTxCb(Idx, framelen);

    return 0;
}
