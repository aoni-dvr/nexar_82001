/**
 * @file AvbPtp.h
 * networks stack gptp header
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

#ifndef AMBA_ENET_PTP_H
#define AMBA_ENET_PTP_H
/* deprecated PTP1 msg type for PTP2 controlField */
#define PTP1_SYNC                     0x00U
#define PTP1_DELAY_REQ                0x01U
#define PTP1_FOLLOWUP                 0x02U
#define PTP1_DELAY_RESP               0x03U
#define PTP1_MANAGEMENT               0x04U
#define PTP1_OTHER                    0x05U

/* PTP2 msg type */
#define PTP2_SYNC                     0x00U
#define PTP2_DELAY_REQ                0x01U
#define PTP2_PATH_DELAY_REQ           0x02U
#define PTP2_PATH_DELAY_RESP          0x03U
#define PTP2_FOLLOWUP                 0x08U
#define PTP2_DELAY_RESP               0x09U
#define PTP2_PATH_DELAY_FOLLOWUP      0x0AU
#define PTP2_ANNOUNCE                 0x0BU
#define PTP2_SIGNALLING               0x0CU
#define PTP2_MANAGEMENT               0x0DU

/* TVL */
#define PTP2_MGNT_TVL_TYPE            0x001U
#define PTP2_MGNTERRSTAT_TVL_TYPE     0x002U
#define PTP2_PATHTRACE_TVL_TYPE       0x008U
#define PTP2_PATHTRACE_TVL_LEN        0x008U
#define PTP2_ORGNZTN_TVL_TYPE         0x003U
#define PTP2_FU_TVL_LEN               0x01cU
#define PTP2_CSRO_TVL_TYPE            0x7D3U
#define PTP2_CSRO_TVL_LEN             0x008U

/* gPTP transportSpecific*/
#define PTP2_TRANSPORT_GPTP           (0x01U << 4U)

/* missed number of Announces before expire */
#define PTP2_AN_EXPIRE_TIMEOUT        0x3U

/* PTP timescale TAI */
#define PTP2_FLAGS_PTP_TIMESCALE      0x0008U
#define PTP2_FLAGS_PTP_TWO_STEP       0x0200U

/* TAI is ahead of UTC by 36 seconds */
#define PTP2_UTC_OFFSET               36U

/* ptp time spec */
typedef struct
{
    UINT32 Sec;
    UINT32 Ns;
} AVB_PTP_TS_s;

/* time processing */
typedef struct
{
    /* for Master */
    AVB_PTP_TS_s Mst;

    /* for Slave */
    AVB_PTP_TS_s t1;
    AVB_PTP_TS_s t2;
    AVB_PTP_TS_s t3;
    AVB_PTP_TS_s t4;
    AVB_PTP_TS_s c1;
    AVB_PTP_TS_s t1_1;
    AVB_PTP_TS_s t2_1;
    AVB_PTP_TS_s c1_1;
    INT64          t12Cnt;
    INT64          t12Sum;
    INT64          t12Avg;
    INT32          ppb;

    /* for Peer Responder (passive) */
    AVB_PTP_TS_s Pt2pssv;
    AVB_PTP_TS_s Pt3pssv;
    UINT8          PClkId[8];
    UINT16         PPortId;
    UINT16         Pseq;

    /* for Peer Requestor */
    AVB_PTP_TS_s Pt1;
    AVB_PTP_TS_s Pt2;
    AVB_PTP_TS_s Pt3;
    AVB_PTP_TS_s Pt4;
    AVB_PTP_TS_s Pdelay;

} AVB_PTP_PROC_s;

typedef struct
{
    UINT8  messageType;
    UINT8  versionPTP;
    UINT16 messageLength;
    UINT8  domainNumber;
    UINT8  reserved1;
    UINT16 flagField;
    UINT16 cHighNs;
    UINT32 cNs;
    UINT16 cSubNs;
    UINT8  reserved2[4];
    UINT8  clockId[8];
    UINT16 sourcePortId;
    UINT16 sequenceId;
    UINT8  controlField;
    INT8  logMessageInterval;
} __attribute__((packed)) AVB_PTP_HDR_s;

typedef struct
{
    UINT16 type;
    UINT16 len;
    /* 8 bytes value */
    UINT8  value[PTP2_PATHTRACE_TVL_LEN];
} __attribute__((packed)) PTP_TLV_PT_HDR_s;

/* Follow_Up information TLV */
typedef struct
{
    UINT16 type;
    UINT16 len;
    /* 28 bytes value */
    UINT8  organizationId[3];
    UINT8  organizationSubType[3];
    UINT32 cumulativeScaledRateOffset;
    UINT16 gmTimeBaseIndicator;
    UINT8  lastGmPhaseChange[12];
    UINT32 scaledLastGmFreqChange;
} __attribute__((packed)) PTP_TLV_FU_s;

/* Announce */
typedef struct
{
    AVB_PTP_HDR_s     ptphdr;
    UINT16            HighSec;
    UINT32            Sec;
    UINT32            Ns;
    UINT16            currentUtcOffset;
    UINT8             reserved1;
    UINT8             priority1;
    UINT8             clockClass;
    UINT8             clockAccuracy;
    UINT16            offsetScaledLogVariance;
    UINT8             priority2;
    UINT8             clockId[8];
    UINT16            stepsRemoved;
    UINT8             timeSource;
    PTP_TLV_PT_HDR_s  tlvPt;
} __attribute__((packed)) PTP_AN_s;

/* SYNC */
/* Delay_Req */
typedef struct
{
    AVB_PTP_HDR_s ptphdr;
    UINT16        HighSec;
    UINT32        Sec;
    UINT32        Ns;
} __attribute__((packed)) AVB_PTP_SYNC_s;

/* Follow_Up */
typedef struct
{
    AVB_PTP_HDR_s     ptphdr;
    UINT16            HighSec;
    UINT32            Sec;
    UINT32            Ns;
    /* followup tlv is gPTP only */
    PTP_TLV_FU_s      tlvFu;
} __attribute__((packed)) AVB_PTP_FU_s;

/* Pdelay_Req */
typedef struct
{
    AVB_PTP_HDR_s ptphdr;
    UINT16        HighSec;
    UINT32        Sec;
    UINT32        Ns;
    UINT8         reserved[10];
} __attribute__((packed)) PTP_PDLYREQ_s;

/* Delay_Resp */
/* Pdelay_Resp */
/* Pdelay_Resp_Follow_Up */
typedef struct
{
    AVB_PTP_HDR_s ptphdr;
    UINT16        HighSec;
    UINT32        Sec;
    UINT32        Ns;
    UINT8         clockId[8];
    UINT16        sourcePortId;
} __attribute__((packed)) PTP_RESP_s;

/* dst for PTP2 msg except P2P */
extern const UINT8 PTP2_DST[6];

/* dst for gPTP and PTP2 P2P */
extern const UINT8 PTP2_GPTP_DST[6];
extern AVB_PTP_PROC_s *pPtPTsProc[AVB_STACK_INSTANCES];

UINT32 AvbPtpNsToTs(const UINT64 Ns, AVB_PTP_TS_s *pTs);
UINT32 AvbPtpTsToNs(const AVB_PTP_TS_s *pTs, UINT64 *pNs);
void AvbPtpPeerInit(UINT32 Idx);
UINT32 AvbPtpUpdatePhc(UINT32 Idx);
UINT32 AvbPtpSetPhcNs(UINT32 Idx, const UINT64 PhcNs);
void AvbPtpInfo(const char *func, const UINT32 line, const UINT16 ModuleID, const char *color, const char *msg);
void AvbPtpPrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void AvbPtpPrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);

/* TX/RX */
UINT32 AvbPtpRxSync(UINT32 Idx, const void *frame, UINT16 framelen);
UINT32 AvbPtpTxSync(UINT32 Idx);
UINT32 AvbPtpRxFollowUp(UINT32 Idx, const void *frame, UINT16 framelen);
UINT32 AvbPtpTxFollowUp(UINT32 Idx);
UINT32 AvbPtpRxPDlyReq(UINT32 Idx, const void *frame, UINT16 framelen);
UINT32 AvbPtpTxPDlyReq(UINT32 Idx);
UINT32 AvbPtpRxPDlyResp(UINT32 Idx, const void *frame, UINT16 framelen);
UINT32 AvbPtpTxPDlyResp(UINT32 Idx);
UINT32 AvbPtpRxPDlyFu(UINT32 Idx, const void *frame, UINT16 framelen);
UINT32 AvbPtpTxPDlyFu(UINT32 Idx);

/* API */
void *AvbPtp_TaskEntry(void *arg);
UINT32 AvbPtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen);
#endif  /* AMBA_ENET_PTP_H */
