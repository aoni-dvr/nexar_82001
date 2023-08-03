/**
 *  @file AvbPtpE2E.c
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
 *  @details AVB gptp End to End networks stack
 *
 */

#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "./AvbPtp.h"

UINT32 AvbPtpRxSync(UINT32 Idx, const void *frame, UINT16 framelen)
{
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];

    (void)frame;
    (void)framelen;
    (void)AmbaEnet_GetRxTs(Idx, &pTsProc->t2.Sec, &pTsProc->t2.Ns);

    return 0;
}

UINT32 AvbPtpTxSync(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    void *ptr;
    AVB_PTP_SYNC_s *sync;
    UINT16 framelen = (UINT16)sizeof(AVB_PTP_SYNC_s);
    UINT16 msglen = (UINT16)sizeof(AVB_PTP_SYNC_s);
#if (PTP2_SYNC == 0U)
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP);
#else
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP | PTP2_SYNC);
#endif
    static UINT16 SyncSeq = 0;

    (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    if (pAvbConfig->PtpConfig.MstSlv == AVB_PTP_MASTER)
    {
        ETH_HDR_s *ethhdr;
        (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);

        AmbaMisra_TypeCast(&ethhdr, &ptr);
        if (AmbaWrap_memcpy(ethhdr->dst, PTP2_GPTP_DST, 6)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
        (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

        AmbaMisra_TypeCast(&sync, &ptr);
        sync->ptphdr.messageType = msgtype;
        sync->ptphdr.versionPTP = 2;
        sync->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
        sync->ptphdr.domainNumber = 0x0U;
        sync->ptphdr.flagField = AmbaNetStack_Htons(PTP2_FLAGS_PTP_TWO_STEP);
        sync->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
        sync->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
        sync->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
        sync->ptphdr.clockId[3] = 0xff;
        sync->ptphdr.clockId[4] = 0xfe;
        if (AmbaWrap_memcpy(sync->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
        if (AmbaWrap_memcpy(&sync->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
        sync->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
        sync->ptphdr.sequenceId = AmbaNetStack_Htons(SyncSeq);
        sync->ptphdr.controlField = PTP1_SYNC;
        /* 1/8 sec sync interval */
        sync->ptphdr.logMessageInterval = PTP2_SYNC_INTERVAL;
        SyncSeq++;

        sync->HighSec =  AmbaNetStack_Htons(0x0);
        sync->Sec =  AmbaNetStack_Htonl(0x0);
        sync->Ns =  AmbaNetStack_Htonl(0x0);

        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] Sync");
        (void)pAvbConfig->pDoTxWaitCb(Idx, framelen, &pTsProc->Mst.Sec, &pTsProc->Mst.Ns);
    }

    return 0;
}

UINT32 AvbPtpRxFollowUp(UINT32 Idx, const void *frame, UINT16 framelen)
{
    const AVB_PTP_FU_s *fu;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];

    (void)framelen;
    if (pTsProc->t2.Sec != 0U) {
        AmbaMisra_TypeCast(&fu, &frame);

        pTsProc->t1.Sec = AmbaNetStack_Htonl(fu->Sec);
        pTsProc->t1.Ns = AmbaNetStack_Htonl(fu->Ns);
        pTsProc->c1.Sec = 0U;
        pTsProc->c1.Ns = AmbaNetStack_Htonl(fu->ptphdr.cNs);
        (void) AvbPtpUpdatePhc(Idx);
    }

    return 0;
}

UINT32 AvbPtpTxFollowUp(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    void *ptr;
    AVB_PTP_FU_s *fu;
    UINT16 framelen = (UINT16)sizeof(AVB_PTP_FU_s);
    UINT16 msglen = (UINT16)sizeof(AVB_PTP_FU_s);
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP | PTP2_FOLLOWUP);
    static UINT16 FuSeq = 0;

    (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    if (pAvbConfig->PtpConfig.MstSlv == AVB_PTP_MASTER)
    {
        ETH_HDR_s *ethhdr;
        (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);

        AmbaMisra_TypeCast(&ethhdr, &ptr);
        if (AmbaWrap_memcpy(ethhdr->dst, PTP2_GPTP_DST, 6)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
        (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

        AmbaMisra_TypeCast(&fu, &ptr);
        fu->ptphdr.messageType = msgtype;
        fu->ptphdr.versionPTP = 2;
        fu->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
        fu->ptphdr.domainNumber = 0x0U;
        fu->ptphdr.flagField = AmbaNetStack_Htons(0x0U);
        fu->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
        fu->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
        fu->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
        fu->ptphdr.clockId[3] = 0xff;
        fu->ptphdr.clockId[4] = 0xfe;
        if (AmbaWrap_memcpy(fu->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
        if (AmbaWrap_memcpy(&fu->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
        fu->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
        fu->ptphdr.sequenceId = AmbaNetStack_Htons(FuSeq);
        fu->ptphdr.controlField = PTP1_FOLLOWUP;
        /* 1/8 sec fu interval */
        fu->ptphdr.logMessageInterval = PTP2_SYNC_INTERVAL;
        FuSeq++;

        fu->HighSec =  AmbaNetStack_Htons(0x0);
        fu->Sec =  AmbaNetStack_Htonl(pTsProc->Mst.Sec);
        fu->Ns =  AmbaNetStack_Htonl(pTsProc->Mst.Ns);

        fu->tlvFu.type = AmbaNetStack_Htons(PTP2_ORGNZTN_TVL_TYPE);
        fu->tlvFu.len = AmbaNetStack_Htons(PTP2_FU_TVL_LEN);
        fu->tlvFu.organizationId[0] = 0x00;
        fu->tlvFu.organizationId[1] = 0x80;
        fu->tlvFu.organizationId[2] = 0xc2;
        fu->tlvFu.organizationSubType[0] = 0x00;
        fu->tlvFu.organizationSubType[1] = 0x00;
        fu->tlvFu.organizationSubType[2] = 0x01;
        fu->tlvFu.cumulativeScaledRateOffset = AmbaNetStack_Htonl(0x0U);
        fu->tlvFu.gmTimeBaseIndicator = AmbaNetStack_Htons(0x0U);
        ptr = &(fu->tlvFu.lastGmPhaseChange[0]);
        if (AmbaWrap_memset(ptr, 0, sizeof(fu->tlvFu.lastGmPhaseChange))!= 0U) { }
        fu->tlvFu.scaledLastGmFreqChange = AmbaNetStack_Htonl(0x0U);

        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] Follow_Up");
        (void)pAvbConfig->pDoTxWaitCb(Idx, framelen, &pTsProc->Mst.Sec, &pTsProc->Mst.Ns);
    }

    return 0;
}

#if 0
//Delay_Req does not exist on gPTP
UINT32 AvbPtp_TxDlyReq(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AVB_PTP_PROC_s *pTsProc = pPtPTsProc[Idx];
    void *ptr;
    AVB_PTP_SYNC_s *dlyreq;
    UINT16 framelen = (UINT16)sizeof(AVB_PTP_SYNC_s);
    UINT16 msglen = (UINT16)sizeof(AVB_PTP_SYNC_s);
    static UINT16 DlyReqSeq = 0;
    ETH_HDR_s *ethhdr;

    (void)AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);
    AmbaMisra_TypeCast(&ethhdr, &ptr);
    if (AmbaWrap_memcpy(ethhdr->dst, PTP2_DST, 6)!= 0U) { }
    if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
    (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

    AmbaMisra_TypeCast(&dlyreq, &ptr);
    dlyreq->ptphdr.messageType = PTP2_DELAY_REQ;
    dlyreq->ptphdr.versionPTP = 2;
    dlyreq->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
    dlyreq->ptphdr.domainNumber = 0x0U;
    dlyreq->ptphdr.flagField = AmbaNetStack_Htons(0x0U);
    dlyreq->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
    dlyreq->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
    dlyreq->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
    dlyreq->ptphdr.clockId[3] = 0xff;
    dlyreq->ptphdr.clockId[4] = 0xfe;
    if (AmbaWrap_memcpy(dlyreq->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
    if (AmbaWrap_memcpy(&dlyreq->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
    dlyreq->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
    dlyreq->ptphdr.sequenceId = AmbaNetStack_Htons(DlyReqSeq);
    dlyreq->ptphdr.controlField = PTP1_DELAY_REQ;
    dlyreq->ptphdr.logMessageInterval = 0x7F;
    DlyReqSeq++;

    dlyreq->HighSec =  AmbaNetStack_Htons(0x0);
    dlyreq->Sec =  AmbaNetStack_Htonl(0x0);
    dlyreq->Ns =  AmbaNetStack_Htonl(0x0);

    AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] Delay_Req");
    (void)pAvbConfig->pDoTxWaitCb(Idx, framelen, &pTsProc->t3.Sec, &pTsProc->t3.Ns);

    return 0;
}
#endif
