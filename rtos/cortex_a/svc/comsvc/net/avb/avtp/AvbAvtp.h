/**
 *  @file AvbAvtp.h
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details networks stack avtp header
 *
 */

#ifndef AMBA_ENET_AVTP_H
#define AMBA_ENET_AVTP_H

/* AVTP ethernet type */
#define AVB_PAYLOAD_MAX               1466U
#define NALU_TYPE_MASK                0x1FU

#define AVTP_ETH_TYPE                 0x22f0U

#define AVTP_SUBTYPE_61883            0x00U
#define AVTP_SUBTYPE_AAF              0x02U
#define AVTP_SUBTYPE_CVF              0x03U
#define AVTP_SUBTYPE_CRF              0x04U
#define AVTP_SUBTYPE_TSCF             0x05U

#define AVTP_FORMAT_RFC               0x02U


#define FLAG_AVPT_FRAME_READY           0x1U

//NALU Type
#define AVTP_NALU_TYPE_NON_IDR          0x01U
#define AVTP_NALU_TYPE_IDR              0x05U
#define AVTP_NALU_TYPE_SEI              0x06U
#define AVTP_NALU_TYPE_SPS              0x07U
#define AVTP_NALU_TYPE_PPS              0x08U
#define AVTP_NALU_TYPE_FUA              0x1CU

//NALU
#define AVTP_NALU                       0x00U

//NALU mask
#define AVTP_NALU_FBIT_MASK             0x80U
#define AVTP_NALU_REF_IDC_MASK          0x60U
#define AVTP_NALU_TYPE_MASK             0x1FU

//FUA
#define AVTP_FUA_IDENT                  0x00U
#define AVTP_FUA_HEAD                   0x01U

//FUA Identifier mask
#define AVTP_FUA_IDENT_FBIT_MASK        0x80U
#define AVTP_FUA_IDENT_REF_IDC_MASK     0x60U
#define AVTP_FUA_IDENT_TYPE_MASK        0x1FU

//FUA Header mask
#define AVTP_FUA_HEAD_SBIT_MASK         0x80U
#define AVTP_FUA_HEAD_EBIT_MASK         0x40U
#define AVTP_FUA_HEAD_FORBIDDEN_MASK    0x20U
#define AVTP_FUA_HEAD_UNIT_TYPE_MASK    0x1FU

/* IEEE 1722-2016 ACF message types */
#define AVTP_ACF_MSG_TYPE_ACF_USER_CAN  0x78U
#define AVTP_ACF_MSG_TYPE_ACF_USER_PSD  0x79U
#define AVTP_ACF_MSG_TYPE_ACF_USER_OWS  0x7AU
#define AVTP_ACF_MSG_TYPE_ACF_USER_GRID 0x7BU

typedef struct {
    AMBA_AVB_AVTP_IOVEC_s       *pVec;
    UINT8                       VecNum;
    UINT32                      TimeStamp;
    UINT8                       Marker;
}AMBA_AVB_AVTP_PAYLOAD_INFO_s;

typedef struct
{
    AMBA_AVB_AVTP_FRAME_INFO_s      FrameInfo;
    UINT32                          TotalBytes;
    UINT8                           *pCur;
    UINT8                           Drop;
} AMBA_AVB_AVTP_UNPACKET_INFO_s;

typedef struct
{
    UINT8             subtype;
    UINT8             sv_ver_mr_tv;
    UINT8             sequence_num;
    UINT8             tu;
    UINT8             stream_id[8];
    UINT32            avtp_timestamp;
    UINT8             format;
    UINT8             format_subtype;
    UINT16            reserved1;
    UINT16            stream_data_length;
    UINT8             ptv_M_evt;
    UINT8             reserved2;
} __attribute__((packed)) AVB_AVTP_HDR_s;

/* H.264 Cpmpressed Video Format */
typedef struct
{
    AVB_AVTP_HDR_s    avtphdr;
    UINT32            h264_timestamp;
    UINT8             h264_payload[1472];
} __attribute__((packed)) AVTP_H264_s;

/* IEEE 1722-2016 Time-Synchronous Control Format */
typedef struct
{
    AVB_AVTP_HDR_s    avtphdr;
    UINT16            acf_msg_type_length;
    UINT8             acf_msg_payload[1474];
} __attribute__((packed)) AVTP_TSCF_s;

typedef struct {
    char                            Name[64];
    UINT32                          Out;
    UINT32                          In;
    UINT32                          Size;
    UINT8                           *pBase;
    AMBA_KAL_MUTEX_t                QueueMutex;
    AMBA_KAL_EVENT_FLAG_t           QueueEvent;
} AMBA_AVB_QUEUE_s;

typedef struct
{
    AMBA_AVB_AVTP_TALKER_s          *pTalker;
    UINT8                           Register;
    UINT8                           SeqNum;
} AMBA_AVB_AVTP_TALKER_INFO_s;

typedef struct
{
    AMBA_AVB_AVTP_LISTENER_s        *pListener;
    UINT8                           Register;
    UINT8                           SeqNum;
    AMBA_AVB_QUEUE_s                BufQueue;
    AMBA_KAL_MSG_QUEUE_t            FrameQueue;
    UINT8                           FirstIdr;
    AMBA_AVB_AVTP_UNPACKET_INFO_s   Unpacket;
} AMBA_AVB_AVTP_LISTENER_INFO_s;

extern AMBA_AVB_AVTP_TALKER_INFO_s      AvtpTalkerInfo[AVB_AVTP_MAX_TALKER_NUM];
extern AMBA_AVB_AVTP_LISTENER_INFO_s    AvtpListenerInfo[AVB_AVTP_MAX_LISTENER_NUM];

UINT32 H264Subsession_Packetize(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, const AMBA_AVB_AVTP_FRAME_INFO_s *Frame);
UINT32 H264Subsession_UnPacketize(AMBA_AVB_AVTP_LISTENER_INFO_s *pListenerInfo,const AVTP_H264_s *pPkt, UINT16 Len);
UINT32 ControlSubsession_Packetize(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, const AMBA_AVB_AVTP_FRAME_INFO_s *Frame);
UINT32 ControlSubsession_UnPacketize(AMBA_AVB_AVTP_LISTENER_INFO_s *pListenerInfo,const AVTP_TSCF_s *pPkt, UINT16 Len);
UINT32 AvbAvtp_BufQueueToIndex(const AMBA_AVB_QUEUE_s *pQueue, const UINT8 *pBuf, UINT32 *pIndex);
UINT32 AvbAvtp_BufQueueCurrentIn(const AMBA_AVB_QUEUE_s *pQueue, UINT8 **ppBuf);
UINT32 AvbAvtp_BufQueueCurrentOut(const AMBA_AVB_QUEUE_s *pQueue, UINT8 **ppBuf);
UINT32 AvbAvtp_BufQueueSeIn(AMBA_AVB_QUEUE_s *pQueue, UINT32 Len);
UINT32 AvbAvtp_BufQueueSeOut(AMBA_AVB_QUEUE_s *pQueue, UINT32 Len);
UINT32 AvbAvtp_BufQueueUnused(const AMBA_AVB_QUEUE_s *pQueue, UINT32 *pNum);
UINT32 AvbAvtp_BufQueueInit(AMBA_AVB_QUEUE_s *pQueue, UINT8 *pBase, UINT32 Size);
UINT32 AvbAvtp_BufQueueDeInit(AMBA_AVB_QUEUE_s *pQueue);
UINT32 AvbAvtp_UnPacketReset(const AMBA_AVB_QUEUE_s *pQueue, AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket);
UINT32 AvbAvtp_UnPacketInit(const AMBA_AVB_QUEUE_s *pQueue, AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket, UINT8 *pBuf, UINT32 TimeStamp);
UINT32 AvbAvtp_UnPacketCopyData(const AMBA_AVB_QUEUE_s *pQueue, AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket, const UINT8 *pSrcBuf, UINT32 Len);
#endif  /* AMBA_ENET_AVTP_H */
