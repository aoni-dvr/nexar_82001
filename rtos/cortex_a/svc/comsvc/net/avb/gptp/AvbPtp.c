/**
 * @file AvbPtp.c
 * AVB gptp networks stack
 *
 * @ingroup enet
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
 */
#include "AmbaKAL.h"

#include "AmbaENET.h"
#include "NetStack.h"
#include "AmbaUtility.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "AmbaPrint.h"
#include "./AvbPtp.h"

#if (AVB_STACK_INSTANCES > 1U)
static AVB_PTP_PROC_s AvbPtp_Proc[2];
AVB_PTP_PROC_s *pPtPTsProc[2] =
{
    [0] = &AvbPtp_Proc[0],
    [1] = &AvbPtp_Proc[1],
};
#else
static AVB_PTP_PROC_s AvbPtp_Proc[1];
AVB_PTP_PROC_s *pPtPTsProc[1] =
{
    [0] = &AvbPtp_Proc[0],
};
#endif

/* dst for gPTP and PTP2 P2P */
const UINT8 PTP2_GPTP_DST[6] = { 0x01U, 0x80U, 0xc2U, 0x00U, 0x00U, 0x0eU };

/* dst for PTP2 msg except P2P */
const UINT8 PTP2_DST[6] = { 0x01U, 0x1bU, 0x19U, 0x00U, 0x00U, 0x00U };

static AMBA_KAL_EVENT_FLAG_t AvbPtpFlag[AVB_STACK_INSTANCES];
#define PTP_TIMER        250
#define FLAG_AN         0x1U
#define FLAG_PDLYREQ    0x2U
#define FLAG_SYNC       0x4U
#define FLAG_PDLYRESP   0x8U
#define FLAG_PDLYFU    0x10U

static void PtPTimerExpire(UINT32 Idx)
{
    INT32 PdlyItvl, AnItvl, SyncItvl;
    INT32 i;
    static INT32 PdlyCredit = 0;
    static INT32 AnCredit = 0;
    static INT32 SyncCredit = 0;
    UINT32 flg = 0U;

#if (PTP2_PDLYREQ_INTERVAL > 0)
    PdlyItvl = (INT32) ((UINT16)1 << (UINT16)PTP2_PDLYREQ_INTERVAL);
    PdlyItvl *= 1000;
#elif (PTP2_PDLYREQ_INTERVAL < 0)
    PdlyItvl = -PTP2_PDLYREQ_INTERVAL;
    for (i = 1; i <= (INT32)(-PTP2_PDLYREQ_INTERVAL); i++) {
        PdlyItvl *= 2;
    }
    PdlyItvl = 1000 / PdlyItvl;
#else
    PdlyItvl = 1000;
#endif
#if (PTP2_AN_INTERVAL > 0)
    AnItvl = (INT32) ((UINT16)1 << (UINT16)PTP2_AN_INTERVAL);
    AnItvl *= 1000;
#elif (PTP2_AN_INTERVAL < 0)
    AnItvl = -PTP2_AN_INTERVAL;
    for (i = 1; i <= (INT32)(-PTP2_AN_INTERVAL); i++) {
        AnItvl *= 2;
    }
    AnItvl = 1000 / AnItvl;
#else
    AnItvl = 1000;
#endif
#if (PTP2_SYNC_INTERVAL > 0)
    SyncItvl = (INT32) ((UINT16)1 << (UINT16)PTP2_SYNC_INTERVAL);
    SyncItvl *= 1000;
#elif (PTP2_SYNC_INTERVAL < 0)
    SyncItvl = -PTP2_SYNC_INTERVAL;
    for (i = 1; i <= (INT32)(-PTP2_SYNC_INTERVAL); i++) {
        SyncItvl *= 2;
    }
    SyncItvl = 1000 / SyncItvl;
#else
    SyncItvl = 1000;
#endif

    if (PdlyCredit <= 0) {
        PdlyCredit = PdlyItvl;
        flg |= FLAG_PDLYREQ;
    }

    if (AnCredit <= 0) {
        AnCredit = AnItvl;
        flg |= FLAG_AN;
    }

    if (SyncCredit <= 0) {
        SyncCredit = SyncItvl;
        flg |= FLAG_SYNC;
    }

    if (flg != 0U) {
        (void) AmbaKAL_EventFlagSet(&AvbPtpFlag[Idx], flg);
    }

    PdlyCredit -= PTP_TIMER;
    AnCredit -= PTP_TIMER;
    SyncCredit -= PTP_TIMER;
}

static UINT32 AvbPtpRxAn(UINT32 Idx, const void *frame, UINT16 framelen)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AMBA_AVB_PTP_CONFIG_s *pPtpConfig;
    const PTP_AN_s *ann;

    (void)AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    pPtpConfig = &(pAvbConfig->PtpConfig);
    (void)framelen;
    AmbaMisra_TypeCast(&ann, &frame);

    if (pPtpConfig->MstSlv == AVB_PTP_AUTO) {
        if (pAvbConfig->PtpConfig.PtpPri < ann->priority1) {
            pPtpConfig->MstSlv = AVB_PTP_MASTER;
            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "[RX] Announce AVB_PTP_MASTER %u < %u", pAvbConfig->PtpConfig.PtpPri,  ann->priority1, 0U, 0U, 0U);
        }
        if (pAvbConfig->PtpConfig.PtpPri > ann->priority1) {
            pPtpConfig->MstSlv = AVB_PTP_SLAVE;
            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "[RX] Announce AVB_PTP_MASTER %u > %u", pAvbConfig->PtpConfig.PtpPri,  ann->priority1, 0U, 0U, 0U);
        }
    }

    return 0;
}

static UINT32 AvbPtpTxAn(UINT32 Idx)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    void *ptr;
    PTP_AN_s *ann;
    UINT16 framelen = (UINT16)sizeof(PTP_AN_s);
    UINT16 msglen = (UINT16)sizeof(PTP_AN_s);
    static UINT16 AnSeq = 0U;
    UINT8 msgtype = (UINT8)(PTP2_TRANSPORT_GPTP | PTP2_ANNOUNCE);

    (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    /* gPTP: must participate in BMCA even if it is not grandmaster capable */
    if ((pAvbConfig->PtpConfig.MstSlv != AVB_PTP_SLAVE) || (0U == AnSeq))
    {
        ETH_HDR_s *ethhdr;

        (void)pAvbConfig->pGetTxBufCb(Idx, &ptr);
        AmbaMisra_TypeCast(&ethhdr, &ptr);
        if (AmbaWrap_memcpy(ethhdr->dst, PTP2_GPTP_DST, 6)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
        (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pAvbConfig->PtpConfig.tci, ETH_PTP2_TYPE);

        AmbaMisra_TypeCast(&ann, &ptr);
        ann->ptphdr.messageType = msgtype;
        ann->ptphdr.versionPTP = 2;
        ann->ptphdr.messageLength = AmbaNetStack_Htons(msglen);
        ann->ptphdr.domainNumber = 0x0U;
        ann->ptphdr.flagField = AmbaNetStack_Htons(PTP2_FLAGS_PTP_TIMESCALE);
        ann->ptphdr.cHighNs = AmbaNetStack_Htons(0x0U);
        ann->ptphdr.cNs = AmbaNetStack_Htonl(0x0U);
        ann->ptphdr.cSubNs = AmbaNetStack_Htons(0x0U);
        ann->ptphdr.clockId[3] = 0xff;
        ann->ptphdr.clockId[4] = 0xfe;
        if (AmbaWrap_memcpy(ann->ptphdr.clockId, pAvbConfig->Mac, 3)!= 0U) { }
        if (AmbaWrap_memcpy(&ann->ptphdr.clockId[5], &pAvbConfig->Mac[3], 3)!= 0U) { }
        ann->ptphdr.sourcePortId = AmbaNetStack_Htons(0x1U);
        ann->ptphdr.sequenceId = AmbaNetStack_Htons(AnSeq);
        AnSeq++;
        ann->ptphdr.controlField = PTP1_OTHER;
        ann->ptphdr.logMessageInterval = PTP2_AN_INTERVAL;

        ann->HighSec =  AmbaNetStack_Htons(0x0);
        ann->Sec =  AmbaNetStack_Htonl(0x0);
        ann->Ns =  AmbaNetStack_Htonl(0x0);
        ann->currentUtcOffset =  AmbaNetStack_Htons(PTP2_UTC_OFFSET);
        ann->priority1 = pAvbConfig->PtpConfig.PtpPri;
        ann->clockClass = PTP2_GM_CLOCKCLASS;
        ann->clockAccuracy = 0xfe;
        ann->offsetScaledLogVariance = AmbaNetStack_Htons(0xffff);
        ann->priority2 = PTP2_GM_PRIORITY2;
        if (AmbaWrap_memcpy(ann->clockId, ann->ptphdr.clockId, 8)!= 0U) { }
        ann->stepsRemoved = AmbaNetStack_Htons(0x0);
        ann->timeSource = 0xa0;
        ann->tlvPt.type = AmbaNetStack_Htons(PTP2_PATHTRACE_TVL_TYPE);
        ann->tlvPt.len = AmbaNetStack_Htons(PTP2_PATHTRACE_TVL_LEN);
        if (AmbaWrap_memcpy(ann->tlvPt.value, ann->ptphdr.clockId, PTP2_PATHTRACE_TVL_LEN)!= 0U) { }

        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPTX, ANSI_GREEN, "ENET [TX] Announce");
        (void)pAvbConfig->pDoTxCb(Idx, framelen);
    }

    return 0;
}

UINT32 AvbPtpNsToTs(const UINT64 Ns, AVB_PTP_TS_s *pTs)
{
    UINT64 Sec64, Ns64;

    Sec64 = Ns / NS_PER_SECOND;
    Ns64 = Ns - (Sec64 * NS_PER_SECOND);

    /* pTs->Sec = Sec64; */
    if (AmbaWrap_memcpy(&pTs->Sec, &Sec64, sizeof(pTs->Sec))!= 0U) { }

    /* pTs->Ns = Ns64; */
    if (AmbaWrap_memcpy(&pTs->Ns, &Ns64, sizeof(pTs->Ns))!= 0U) { }

    return 0;
}

UINT32 AvbPtpTsToNs(const AVB_PTP_TS_s *pTs, UINT64 *pNs)
{
    *pNs = pTs->Ns;
    *pNs  += pTs->Sec * NS_PER_SECOND;

    return 0;
}

/* apply time offset */
static void PtpAdjPhc(UINT32 Idx, UINT8 DoSubtract, const AVB_PTP_TS_s *pTs)
{
    AVB_PTP_PROC_s *const pTsProc = pPtPTsProc[Idx];

    (void) AmbaEnet_PhcUpdaTs(Idx, DoSubtract, pTs->Sec, pTs->Ns);

    if (DoSubtract == 0U) {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_YELLOW " phc        +%7u.%09u" ANSI_RESET, pTs->Sec, pTs->Ns, 0U, 0U, 0U);
    } else {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_YELLOW " phc        -%7u.%09u" ANSI_RESET, pTs->Sec, pTs->Ns, 0U, 0U, 0U);
    }

    pTsProc->t1_1.Sec = 0U;
    pTsProc->t1_1.Ns = 0U;
    pTsProc->t2_1.Sec = 0U;
    pTsProc->t2_1.Ns = 0U;
    pTsProc->c1_1.Sec = 0U;
    pTsProc->c1_1.Ns = 0U;
}
/* adjust hardware clock oscillator freq diff ratio */
static INT32 PtpAdjPpb(UINT32 Idx, UINT8 DoSubtract, const AVB_PTP_TS_s *pTs)
{
    AVB_PTP_PROC_s *const pTsProc = pPtPTsProc[Idx];
    UINT64 nNs, n_1Ns;
    UINT64 MstPeriod = 0, SlvPeriod = 0;
    INT32 ppb32 = 0, ret;
    UINT64 ppbu64, offset;

    if (DoSubtract == 0U) {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_RED " offset            -%10u" ANSI_RESET, pTs->Ns, 0U, 0U, 0U, 0U);
    } else {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_RED " offset            +%10u" ANSI_RESET, pTs->Ns, 0U, 0U, 0U, 0U);
    }

    AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " path     %10u.%09u", pTsProc->Pdelay.Sec, pTsProc->Pdelay.Ns, 0U, 0U, 0U);

    /* MstPeriod */
    if (pTsProc->t1_1.Sec != 0U) {
        (void)AvbPtpTsToNs(&pTsProc->t1, &nNs);
        (void)AvbPtpTsToNs(&pTsProc->t1_1, &n_1Ns);
        nNs += pTsProc->c1.Ns;
        n_1Ns += pTsProc->c1_1.Ns;
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " t1       %10u.%09u", pTsProc->t1.Sec, pTsProc->t1.Ns, 0U, 0U, 0U);
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " t1_1     %10u.%09u", pTsProc->t1_1.Sec, pTsProc->t1_1.Ns, 0U, 0U, 0U);
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " c1       %10u.%09u", pTsProc->c1.Sec, pTsProc->c1.Ns, 0U, 0U, 0U);
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " c1_1     %10u.%09u", pTsProc->c1_1.Sec, pTsProc->c1_1.Ns, 0U, 0U, 0U);
        if (nNs > n_1Ns) {
            MstPeriod = nNs - n_1Ns;
        }
    }

    /* SlvPeriod */
    if (pTsProc->t2_1.Sec != 0U) {
        (void)AvbPtpTsToNs(&pTsProc->t2, &nNs);
        (void)AvbPtpTsToNs(&pTsProc->t2_1, &n_1Ns);
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " t2       %10u.%09u", pTsProc->t2.Sec, pTsProc->t2.Ns, 0U, 0U, 0U);
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " t2_1     %10u.%09u", pTsProc->t2_1.Sec, pTsProc->t2_1.Ns, 0U, 0U, 0U);
        if (nNs > n_1Ns) {
            SlvPeriod = nNs - n_1Ns;
        }
    }

    /* offset */
    if (MstPeriod > SlvPeriod) {
        offset = MstPeriod - SlvPeriod;
    } else {
        offset = SlvPeriod - MstPeriod;
    }

    /* ppb32 */
    ppbu64 = offset * NS_PER_SECOND;
    if (SlvPeriod != 0U) {
        ppbu64 /= SlvPeriod;
        // ppb32 = ppbu64;
        if (AmbaWrap_memcpy(&ppb32, &ppbu64, sizeof(ppb32))!= 0U) { }
    }
    AvbPtpPrintInt5(AVB_LOGGING_PTPREPORT, " ppb32               %09d", ppb32, 0, 0, 0, 0);

    /* fine tuning 0.0002% */
    if ((pTsProc->ppb != 0) &&
        (pTs->Ns < 10000U) &&
        (ppb32 >= 1999)) {
        ppb32 = 1999;
        AvbPtpPrintInt5(AVB_LOGGING_PTPREPORT, " ppb32               %09d", ppb32, 0, 0, 0, 0);
    }

    if (MstPeriod < SlvPeriod) {
        ppb32 = -ppb32;
        AvbPtpPrintInt5(AVB_LOGGING_PTPREPORT, " ppb32               %09d", ppb32, 0, 0, 0, 0);
    }

    /* ~20% hard limit */
    ret = 0;
    if (((pTsProc->ppb) <= 199999999) &&
        ((pTsProc->ppb) >= -199999999)) {
        if (((DoSubtract == 1U) && (MstPeriod < SlvPeriod)) ||
            ((DoSubtract == 0U) && (MstPeriod > SlvPeriod))) {
            extern INT32 AmbaEnet_PhcGetPpb(UINT32 Idx);
            pTsProc->ppb += ppb32;
            /* absolute ppb */
            (void) AmbaEnet_PhcAdjFreq(Idx, pTsProc->ppb);
            /* relative ppb */
            //(void) AmbaEnet_PhcAdjFreq(Idx, ppb32);
            AvbPtpPrintInt5(AVB_LOGGING_PTPREPORT, ANSI_RED " ppb updted          %09d" ANSI_RESET, pTsProc->ppb, 0, 0, 0, 0);
            AvbPtpPrintInt5(AVB_LOGGING_PTPREPORT, ANSI_YELLOW " ppb                 %09d" ANSI_RESET, AmbaEnet_PhcGetPpb(0U), 0, 0, 0, 0);
            ret = ppb32;
        }
    }

    pTsProc->t1_1.Sec = 0U;
    pTsProc->t1_1.Ns = 0U;
    pTsProc->t2_1.Sec = 0U;
    pTsProc->t2_1.Ns = 0U;
    pTsProc->c1_1.Sec = 0U;
    pTsProc->c1_1.Ns = 0U;

    return ret;
}

/* report time offset */
static void print_sample(UINT8 DoSubtract, const AVB_PTP_TS_s *pTs)
{
    if (DoSubtract == 0U) {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " tooslow            %10u", pTs->Ns, 0U, 0U, 0U, 0U);
    } else {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " toofast            %10u", pTs->Ns, 0U, 0U, 0U, 0U);
    }
}

/* report ptp avg offset */
static void print_report(UINT32 Idx)
{
    const AVB_PTP_PROC_s *const pTsProc = pPtPTsProc[Idx];
    UINT8 DoSubtract = (pTsProc->t12Avg > 0) ? 1U : 0U;
    UINT32 Ns32;
    INT64 avg64;
    static UINT32 doreport = 0U;

    doreport ++;
    doreport %= 0x8U;
    if (doreport == 0U) {
        avg64 = (pTsProc->t12Avg > 0) ? (pTsProc->t12Avg) : (-pTsProc->t12Avg);
        if (AmbaWrap_memcpy(&Ns32, &avg64, sizeof(Ns32))!= 0U) { }
        if (DoSubtract == 0U) {
            AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_BLUE " avg offset        -%10u" ANSI_RESET, Ns32, 0U, 0U, 0U, 0U);
        } else {
            AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_BLUE " avg offset        +%10u" ANSI_RESET, Ns32, 0U, 0U, 0U, 0U);
        }
    }
}

/* get time offset */
static void PtpCalOffset(UINT32 Idx, UINT8 *pDoSubtract, UINT64 *pOffset)
{
    const AVB_PTP_PROC_s *const pTsProc = pPtPTsProc[Idx];
    UINT64 c1Ns, t2Ns;

    /**
     * B(Slave) Time Offset = t2 - t1 - c1 - pathDelayofFinalHop
     * correctionField = c1
     * t12 = t1 - t2
     */
    (void)AvbPtpTsToNs(&pTsProc->t1, &c1Ns);
    (void)AvbPtpTsToNs(&pTsProc->t2, &t2Ns);
    if ((pTsProc->Pdelay.Sec == 0U) &&
        (pTsProc->Pdelay.Ns != 0U)){
        c1Ns += pTsProc->Pdelay.Ns;
    }

    if ((pTsProc->c1.Sec == 0U) &&
        (pTsProc->c1.Ns != 0U)) {
        c1Ns += pTsProc->c1.Ns;
        AvbDebug(" c1       %10u.%09u", pTsProc->c1.Sec, pTsProc->c1.Ns);
    }

    *pOffset = (t2Ns > c1Ns) ? (t2Ns - c1Ns) : (c1Ns - t2Ns);
    *pDoSubtract = (t2Ns > c1Ns) ? (1U) : (0U);
}

/* calculate t12Avg = t12Sum / t12Cnt */
static void PtpServo(UINT32 Idx, UINT8 DoSubtract, const AVB_PTP_TS_s *pTs)
{
    AVB_PTP_PROC_s *const pTsProc = pPtPTsProc[Idx];

    if (DoSubtract == 0U) {
        pTsProc->t12Sum -= (INT64)pTs->Ns;
    } else {
        pTsProc->t12Sum += (INT64)pTs->Ns;
    }
    pTsProc->t12Cnt++;
    pTsProc->t12Avg = pTsProc->t12Sum / pTsProc->t12Cnt;
}

/*
 * clock servo algorithms to reduce delay fluctuation
 *   valid samples:
 *                             25% cur offset < avg offset
 *                             avg offset = 0
 *                             cur offset < 100ns
 *   PHC quick sync mode:
 *                             cur offset > 900us, 1 valid sample
 *                             avg offset > 100ns and avg offset > Pdelay, 1 valid sample
 *   PHC stabilized sync mode:
 *                             cur offset <  100ns and avg offset <  100ns, 8 valid samples
 *                             cur offset < Pdelay and avg offset < Pdelay, 8 total samples
 */
UINT32 AvbPtpUpdatePhc(UINT32 Idx)
{
    const UINT32 ONESEC = 8U, FIVESEC = 5U*ONESEC, GOODACCUR = 100U, LOSTACCUR = 900000U;
    AVB_PTP_PROC_s *const pTsProc = pPtPTsProc[Idx];
    AVB_PTP_TS_s Ts;
    UINT8 DoSubtract, ResetServo = 0U;
    UINT32 Ns32;
    UINT64 curoffu64;
    INT64 avg64;
    static UINT32 toofast = 0U, tooslow = 0U, startup = 1U;

    PtpCalOffset(Idx, &DoSubtract, &curoffu64);
    (void) AvbPtpNsToTs(curoffu64, &Ts);

    if (DoSubtract == 0U) {
        tooslow++;
    } else {
        toofast++;
    }

    print_report(Idx);
    avg64 = (pTsProc->t12Avg > 0) ? (pTsProc->t12Avg) : (-pTsProc->t12Avg);
    print_sample(DoSubtract, &Ts);

    /* choose sync mode */
    if ((curoffu64 < (UINT64)GOODACCUR) && (avg64 < (INT64)GOODACCUR) && (pTsProc->t12Cnt >= (INT64)ONESEC))
    {
        /* PHC stable */
        startup = 0U;
    }
    else if (curoffu64 > (UINT64)LOSTACCUR)
    {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_BLUE "PHC Re-Calibrating" ANSI_RESET, 0U, 0U, 0U, 0U, 0U);
        pTsProc->ppb = 0;
        ResetServo = 1U;
        startup = 1U;
        PtpAdjPhc(Idx, DoSubtract, &Ts);
        (void) AmbaEnet_PhcAdjFreq(Idx, pTsProc->ppb);
    }
    else {
        Ns32 = (UINT32)avg64;
        Ns32 >>= 2U;
        if ((Ns32 > GOODACCUR) && (Ns32 > pTsProc->Pdelay.Ns)) {
            /* time diff > path*4 */
            startup = 1U;
        }
    }

    if ((toofast == 0U) && (tooslow >= ONESEC) && (startup == 1U))
    {
        /* (quick sync) too slow */
        (void) AvbPtpNsToTs((UINT64)avg64, &Ts);
        DoSubtract = 0U;
        if ((Ts.Ns < pTsProc->Pdelay.Ns) && (avg64 < (INT64)pTsProc->Pdelay.Ns)) {
            startup = 0U;
        }
        (void)PtpAdjPpb(Idx, DoSubtract, &Ts);
        PtpAdjPhc(Idx, DoSubtract, &Ts);
        ResetServo = 1U;
    }
    else if ((tooslow >= FIVESEC) && (startup == 0U) && (pTsProc->t12Avg < 0))
    {
        /* (stabilized sync) too slow */
        (void) AvbPtpNsToTs((UINT64)avg64, &Ts);
        DoSubtract = 0U;
        if (0 == PtpAdjPpb(Idx, DoSubtract, &Ts)) {
            PtpAdjPhc(Idx, DoSubtract, &Ts);
        }
        ResetServo = 1U;
    }
    else if ((tooslow == 0U) && (toofast >= ONESEC) && (startup == 1U))
    {
        /* (quick sync) too fast */
        (void) AvbPtpNsToTs((UINT64)avg64, &Ts);
        DoSubtract = 1U;
        if ((Ts.Ns < pTsProc->Pdelay.Ns) && (avg64 < (INT64)pTsProc->Pdelay.Ns)) {
            startup = 0U;
        }
        (void)PtpAdjPpb(Idx, DoSubtract, &Ts);
        PtpAdjPhc(Idx, DoSubtract, &Ts);
        ResetServo = 1U;
    }
    else if ((toofast >= FIVESEC) && (startup == 0U) && (pTsProc->t12Avg > 0))
    {
        /* (stabilized sync) too fast */
        (void) AvbPtpNsToTs((UINT64)avg64, &Ts);
        DoSubtract = 1U;
        if (0 == PtpAdjPpb(Idx, DoSubtract, &Ts)) {
            PtpAdjPhc(Idx, DoSubtract, &Ts);
        }
        ResetServo = 1U;
    }
    else if (((toofast > (tooslow + ONESEC)) || (tooslow > (toofast + ONESEC))) && (startup == 1U))
    {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " toofast %5u  tooslow  %5u", toofast, tooslow, 0U, 0U, 0U);
        /* quick restart */
        ResetServo = 1U;
    }
    else if (((toofast > (tooslow + FIVESEC)) || (tooslow > (toofast + FIVESEC))))
    {
        AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, " toofast %5u  tooslow  %5u", toofast, tooslow, 0U, 0U, 0U);
        /* force restart */
        ResetServo = 1U;
    }
    else if (pTsProc->t12Cnt < 0)
    {
        /* skip after reset */
        pTsProc->t12Cnt++;
    }
    else if (pTsProc->t12Cnt == 0)
    {
        /* ppb period restart */
        if ((pTsProc->t1.Sec == pTsProc->t2.Sec) &&
            (pTsProc->t1_1.Sec == 0U) &&
            (pTsProc->t2_1.Sec == 0U)) {
            pTsProc->t1_1.Sec = pTsProc->t1.Sec;
            pTsProc->t1_1.Ns = pTsProc->t1.Ns;
            pTsProc->t2_1.Sec = pTsProc->t2.Sec;
            pTsProc->t2_1.Ns = pTsProc->t2.Ns;
            pTsProc->c1_1.Sec = pTsProc->c1.Sec;
            pTsProc->c1_1.Ns = pTsProc->c1.Ns;
            AvbPtpPrintUInt5(AVB_LOGGING_PTPREPORT, ANSI_BLUE "ppb period restart" ANSI_RESET, 0U, 0U, 0U, 0U, 0U);
        }

        /* collect first sample */
        if (DoSubtract == 0U) {
            pTsProc->t12Sum -= (INT64)Ts.Ns;
        } else {
            pTsProc->t12Sum += (INT64)Ts.Ns;
        }
        pTsProc->t12Avg = pTsProc->t12Sum;
        pTsProc->t12Cnt++;
    }
    else
    {
        /* collect sample */
        Ns32 = Ts.Ns;
        Ns32 >>= 2U;
        if (((INT64)Ns32 < avg64) || (avg64 == 0) || (Ns32 < GOODACCUR)) {
            PtpServo(Idx, DoSubtract, &Ts);
        }
    }

    if (ResetServo != 0U) {
        tooslow = 0U;
        toofast = 0U;
        pTsProc->t12Cnt = -1;
        pTsProc->t12Sum = 0;
        pTsProc->t12Avg = 0;
    }

    return 0U;
}

UINT32 AvbPtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    AMBA_AVB_PTP_CONFIG_s *pPtpConfig;
    UINT8 transportSpecific, messageType;
    const AVB_PTP_HDR_s *ptphdr;
    void *ptr = NULL;
    UINT16 Len1 = framelen;

    (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
    pPtpConfig = &(pAvbConfig->PtpConfig);
    AmbaMisra_TypeCast(&ptr, &frame);
    (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);

    AmbaMisra_TypeCast(&ptphdr, &ptr);

    transportSpecific = 0xF0U & ptphdr->messageType;
    if (transportSpecific != PTP2_TRANSPORT_GPTP) {
        AvbDebug("ENET %s(%d) unknown gPTP transport %u", __func__, __LINE__, transportSpecific);
    }

    if (ptphdr->versionPTP != 2U) {
        AvbPtpInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "unknown PTP version");
    }

    messageType = 0x0FU & ptphdr->messageType;
    switch (messageType) {
    case PTP2_SYNC:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Sync");
        if (pPtpConfig->MstSlv == AVB_PTP_AUTO) {
            pPtpConfig->MstSlv = AVB_PTP_SLAVE;
            AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_WHITE, "[RX] Sync, AVB_PTP_SLAVE");
        }
        (void) AvbPtpRxSync(Idx, ptr, Len1);
        break;
    case PTP2_DELAY_REQ:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Delay Req");
        break;
    case PTP2_PATH_DELAY_REQ:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Path Delay Req");
        (void) AvbPtpRxPDlyReq(Idx, ptr, Len1);
        (void) AmbaKAL_EventFlagSet(&AvbPtpFlag[Idx], FLAG_PDLYRESP);
        (void) AmbaKAL_EventFlagSet(&AvbPtpFlag[Idx], FLAG_PDLYFU);
        /* do not block rx performance
         * (void) AvbPtpTxPDlyResp(Idx);
         * (void) AvbPtpTxPDlyFu(Idx);
         */
        break;
    case PTP2_PATH_DELAY_RESP:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Path Delay Resp");
        (void) AvbPtpRxPDlyResp(Idx, ptr, Len1);
        break;
    case PTP2_FOLLOWUP:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Follow Up");
        if (pPtpConfig->MstSlv == AVB_PTP_AUTO) {
            pPtpConfig->MstSlv = AVB_PTP_SLAVE;
            AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_WHITE, "[RX] Follow Up, AVB_PTP_SLAVE");
        }

        if (pAvbConfig->PtpConfig.MstSlv != AVB_PTP_MASTER) {
            (void) AvbPtpRxFollowUp(Idx, ptr, Len1);
        }
#if 0
        (void) AvbPtp_TxDlyReq(Idx);
#endif
        break;
    case PTP2_DELAY_RESP:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Delay Resp");
        if (pPtpConfig->MstSlv == AVB_PTP_AUTO) {
            pPtpConfig->MstSlv = AVB_PTP_SLAVE;
            AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_WHITE, "[RX] Delay Resp, AVB_PTP_SLAVE");
        }
        break;
    case PTP2_PATH_DELAY_FOLLOWUP:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Path Delay Resp Follow Up");
        (void) AvbPtpRxPDlyFu(Idx, ptr, Len1);
        break;
    case PTP2_ANNOUNCE:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Announce");
        (void) AvbPtpRxAn(Idx, ptr, Len1);
        break;
    case PTP2_SIGNALLING:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Signalling");
        break;
    case PTP2_MANAGEMENT:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_MAGENTA, "ENET [RX] Management");
        break;
    default:
        AvbPtpInfo(__func__, __LINE__, AVB_LOGGING_PTPRX, ANSI_RED, "ENET [RX] Drop unknown PTP");
        break;
    }

    return 0U;
}

void *AvbPtp_TaskEntry(void *arg)
{
    static AMBA_KAL_TIMER_t PtPTimer = {0};
    UINT32 flg, Idx;
    UINT8 CreateFlag = 0U;
    UINT8 ErrorFlag = 0U;
    static char AvbPtpName[16] = "AvbPtp";

    /*Idx = (UINT32)arg;*/
    (void)arg;
    if (AmbaWrap_memcpy(&Idx, &arg, sizeof(Idx))!= 0U) { }
    AvbPtpPeerInit(Idx);

    for (;;) {
        if (CreateFlag == 0U) {
            if (AvbPtpFlag[Idx].tx_event_flags_group_id == 0U) {
                if (OK != AmbaKAL_EventFlagCreate(&AvbPtpFlag[Idx], AvbPtpName)) {
                    AvbPtpInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaKAL_EventFlagCreate failed");
                    ErrorFlag = 1U;
                }
            }
            if (OK != AmbaKAL_TimerCreate(&PtPTimer, AvbPtpName, PtPTimerExpire, Idx,
                PTP_TIMER, PTP_TIMER, 0x1)) {
                AvbPtpInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "AmbaKAL_TimerCreate failed");
                ErrorFlag = 1U;
            }
            if (ErrorFlag == 1U) {
                break;
            }
            else {
                CreateFlag = 1U;
            }
        }

        (void) AmbaKAL_EventFlagGet(&AvbPtpFlag[Idx], 0xffffU, 0x0, 0x1, &flg, AMBA_KAL_WAIT_FOREVER);
        AvbDebug("ENET %s(%d) flg 0x%x", __func__, __LINE__, flg);
        if ((flg & FLAG_AN) != 0U) {
            (void) AvbPtpTxAn(Idx);
        }
        if ((flg & FLAG_PDLYREQ) != 0U) {
            (void) AvbPtpTxPDlyReq(Idx);
        }
        if ((flg & FLAG_SYNC) != 0U) {
            (void) AvbPtpTxSync(Idx);
            (void) AvbPtpTxFollowUp(Idx);
        }
        /* MUST check FLAG_PDLYRESP before FLAG_PDLYFU */
        if ((flg & FLAG_PDLYRESP) != 0U) {
            (void) AvbPtpTxPDlyResp(Idx);
        }
        if ((flg & FLAG_PDLYFU) != 0U) {
            (void) AvbPtpTxPDlyFu(Idx);
        }
    }

    return NULL;
}

UINT32 AvbPtpSetPhcNs(UINT32 Idx, const UINT64 PhcNs)
{
    AVB_PTP_TS_s Ts;

    (void) AvbPtpNsToTs(PhcNs, &Ts);

    (void) AmbaEnet_PhcSetTs(Idx, Ts.Sec, Ts.Ns);

    return 0;
}

static UINT32 PtpGetLog(const UINT16 ModuleID)
{
    AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    UINT32 logging = 0U;
    UINT32 Idx;

    for (Idx = 0U; Idx < AVB_STACK_INSTANCES; Idx++) {
        (void) AmbaAvbStack_GetConfig(Idx, &pAvbConfig);
        if (pAvbConfig == NULL) {
            continue;
        }

        if ((0U != (pAvbConfig->AvbLoggingCtrl & AVB_LOGGING_PTPTX)) &&
            (ModuleID == AVB_LOGGING_PTPTX)) {
            logging = 1U;
        }
        if ((0U != (pAvbConfig->AvbLoggingCtrl & AVB_LOGGING_PTPRX)) &&
            (ModuleID == AVB_LOGGING_PTPRX)) {
            logging = 1U;
        }
        if ((0U != (pAvbConfig->AvbLoggingCtrl & AVB_LOGGING_PTPREPORT)) &&
            (ModuleID == AVB_LOGGING_PTPREPORT)) {
            logging = 1U;
        }
    }

    return logging;
}

void AvbPtpInfo(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg)
{
    char Buf[22];

    if (PtpGetLog(ModuleID) != 0U) {
        (void)AmbaUtility_UInt32ToStr(Buf, sizeof(Buf), line, 10U);
        AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s GPTP %s(%s) %s %s", color, func, Buf, msg, ANSI_RESET);
    }
}

void AvbPtpPrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (PtpGetLog(ModuleID) != 0U) {
        AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}

void AvbPtpPrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    if (PtpGetLog(ModuleID) != 0U) {
        AmbaPrint_ModulePrintInt5(AVB_MODULE_ID, pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
}
