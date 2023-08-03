/**
 * @file AvbStack.h
 * Amba AVB stack header file
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

#ifndef AMBA_ENET_AVB_H
#define AMBA_ENET_AVB_H

/*  Interval for Pdelay_Resp, Pdelay_Resp_Follow_Up */
#define PTP2_FOREVER_INTERVAL         0x7f

/* Sync Interval */
//#define PTP2_SYNC_INTERVAL            0x7f
#define PTP2_SYNC_INTERVAL            -3

/* Announce Interval */
//#define PTP2_AN_INTERVAL              0x7f
#define PTP2_AN_INTERVAL              1

/* Pdelay_Req Interval */
//#define PTP2_PDLYREQ_INTERVAL         0x7f
#define PTP2_PDLYREQ_INTERVAL         0

/* Priority for BMCA (Best Master Clock Algorithm) */
#define PTP2_GM_UNLIKELY              254U
#define PTP2_GM_AUTO                  249U
#define PTP2_GM_LIKELY                247U
#define PTP2_GM_CLOCKCLASS            254U
#define PTP2_GM_PRIORITY2             254U

/* for AvbLoggingCtrl */
#define AVB_LOGGING_PTPTX            1U
#define AVB_LOGGING_PTPRX            2U
#define AVB_LOGGING_PTPREPORT        4U

#define AMBA_AVB_STACK_GETTXBUF_f   AMBA_NET_STACK_GETTXBUF_f
#define AMBA_AVB_STACK_GETRXBUF_f   AMBA_NET_STACK_GETRXBUF_f
#define AMBA_AVB_STACK_DOTX_f       AMBA_NET_STACK_DOTX_f
#define AMBA_AVB_STACK_DOTXWAIT_f   AMBA_NET_STACK_DOTXWAIT_f
#define AMBA_AVB_STACK_RX_f         AMBA_NET_STACK_RX_f

#define AVB_PTP_AUTO                  0U
#define AVB_PTP_MASTER                1U
#define AVB_PTP_SLAVE                 2U

#define AVTP_FORMAT_SUBTYPE_MJPEG                   0x00U
#define AVTP_FORMAT_SUBTYPE_H264                    0x01U
#define AVTP_FORMAT_SUBTYPE_JPEG2000                0x02U
#define AVTP_FORMAT_SUBTYPE_RESERVED_CARINFO_ACF    0x03U
#define AVTP_FORMAT_SUBTYPE_RESERVED_PSD_ACF        0x04U
#define AVTP_FORMAT_SUBTYPE_RESERVED_OWS_ACF        0x05U
#define AVTP_FORMAT_SUBTYPE_RESERVED_GRID_ACF       0x06U

#define AVB_PTP_TASKSTACKSIZE  (8192U)
#define AVB_AVTP_TASKSTACKSIZE (8192U)

#define AVB_AVTP_MAX_TALKER_NUM         0x4U
#define AVB_AVTP_MAX_LISTENER_NUM       0x6U

typedef void (*AMBA_AVB_STACK_TASKENTRY_f)(const UINT32 Idx);
typedef UINT32 (*AMBA_AVB_AVDECC_f)(const UINT32 Idx,
                                    const UINT8 dmac[6],
                                    const UINT8 smac[6],
                                    const void *frame,
                                    const UINT16 Len);

typedef struct
{
    UINT32  SmpCoreSet;
    UINT32  TaskPri;
    UINT32  MstSlv;
    UINT8   PtpPri;
    UINT16  tci;
} AMBA_AVB_PTP_CONFIG_s;

typedef struct
{
    UINT8                           Mac[6];
    UINT16                          UniqueID;
} AMBA_AVB_AVTP_STREAMID_s;

typedef struct {
    UINT8                           *pBase;
    UINT32                          Len;
}AMBA_AVB_AVTP_IOVEC_s;

typedef struct {
    AMBA_AVB_AVTP_IOVEC_s           Vec[2];
    UINT8                           VecNum;
    UINT32                          FrameType;
    UINT32                          FrameTimeStamp;
    UINT32                          AvtpTimeStamp;
    UINT8                           Completed;
}AMBA_AVB_AVTP_FRAME_INFO_s;

typedef struct
{
    AMBA_AVB_STACK_GETTXBUF_f       pGetTxBufCb;
    AMBA_AVB_STACK_GETRXBUF_f       pGetRxBufCb;
    AMBA_AVB_STACK_DOTX_f           pDoTxCb;
    AMBA_AVB_STACK_DOTXWAIT_f       pDoTxWaitCb;
    AMBA_AVB_STACK_RX_f             pRxCb;
    AMBA_AVB_AVDECC_f               pAvdeccCb;
    UINT32                          Idx;
    UINT8                           Mac[6];
    UINT32                          AvbLoggingCtrl;
    AMBA_AVB_PTP_CONFIG_s           PtpConfig;
} AMBA_AVB_STACK_CONFIG_s;

typedef struct
{
    AMBA_AVB_STACK_CONFIG_s         *pAvbConfig;
    AMBA_AVB_AVTP_STREAMID_s        StreamID;
    UINT8                           MultiCastMac[6];
    UINT16                          tci;
} AMBA_AVB_AVTP_TALKER_s;

typedef struct
{
    AMBA_AVB_STACK_CONFIG_s         *pAvbConfig;
    AMBA_AVB_AVTP_STREAMID_s        StreamID;
    UINT8                           MultiCastMac[6];
    UINT8                           *pBuf;
    UINT32                          BufSize;
    AMBA_AVB_AVTP_FRAME_INFO_s      *pFrameQueue;
    UINT32                          FrameQueueNum;
} AMBA_AVB_AVTP_LISTENER_s;

/* API */
UINT32 AmbaAvbStack_SetConfig(UINT32 Idx, AMBA_AVB_STACK_CONFIG_s *pAvbConfig);
UINT32 AmbaAvbStack_GetConfig(UINT32 Idx, AMBA_AVB_STACK_CONFIG_s **pAvbConfig);
UINT32 AmbaAvbStack_EnetInit(AMBA_AVB_STACK_CONFIG_s *pAvbConfig);
UINT32 AmbaAvbStack_GetPhc(UINT32 Idx, UINT32 *pSec, UINT32 *pNs);
UINT32 AmbaAvbStack_GetPhcNs(UINT32 Idx, UINT64 *pPhcNs);
UINT32 AmbaAvbStack_AvtpListenerReg(AMBA_AVB_AVTP_LISTENER_s *pListener);
UINT32 AmbaAvbStack_AvtpListenerUnReg(const AMBA_AVB_AVTP_LISTENER_s *pListener);
UINT32 AmbaAvbStack_AvtpListenerRx(const AMBA_AVB_AVTP_LISTENER_s *pListener, const AMBA_AVB_AVTP_FRAME_INFO_s *pFrame, UINT32 TimeOut);
UINT32 AmbaAvbStack_AvtpListenerRxDone(const AMBA_AVB_AVTP_LISTENER_s *pListener, const AMBA_AVB_AVTP_FRAME_INFO_s *pFrame);
UINT32 AmbaAvbStack_AvtpTalkerReg(AMBA_AVB_AVTP_TALKER_s *pTalker);
UINT32 AmbaAvbStack_AvtpTalkerUnReg(const AMBA_AVB_AVTP_TALKER_s *pTalker);
UINT32 AmbaAvbStack_AvtpTalkerTx(const AMBA_AVB_AVTP_TALKER_s *pTalker, const AMBA_AVB_AVTP_FRAME_INFO_s *Frame);
#endif  /* AMBA_ENET_AVB_H */
