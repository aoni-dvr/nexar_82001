/**
 *  @file ControlSubsession.c
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
 *  @details AVB Control subsession
 *
 */
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "AvbAvtp.h"

static UINT32 ControlSubsession_TxVector(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, const AMBA_AVB_AVTP_PAYLOAD_INFO_s *Payload, UINT32 Type)
{
    const AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    const AMBA_AVB_AVTP_TALKER_s *pTalker;
    void *ptr;
    AVTP_TSCF_s *avtp;
    UINT16 framelen = (UINT16)(sizeof(AVB_AVTP_HDR_s) + 4U);
    UINT32 PhcNs32;
    UINT64 PhcNs64;
    ETH_HDR_s *ethhdr;
    UINT8 i = 0;
    UINT16 data_length = 0U;
    UINT8 *tmp;
    UINT32 Ret = NET_ERR_NONE;

    /* arg check */
    if (pTalkerInfo == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if (Payload == NULL) {
        EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Payload NULL!");
        Ret = NET_ERR_EINVAL;
    } else {
        /* code start */
        pTalker = pTalkerInfo->pTalker;
        pAvbConfig = pTalker->pAvbConfig;
        (void)pAvbConfig->pGetTxBufCb(pAvbConfig->Idx, &ptr);
        AmbaMisra_TypeCast32(&ethhdr, &ptr);
        if (AmbaWrap_memcpy(ethhdr->dst, pTalker->MultiCastMac, 6)!= 0U) { }
        if (AmbaWrap_memcpy(ethhdr->src, pAvbConfig->Mac, 6)!= 0U) { }
        (void)AmbaNetStack_InsertEthHdr(&ptr, &framelen, pTalker->tci, ETH_AVTP_TYPE);

        AmbaMisra_TypeCast32(&avtp, &ptr);
        avtp->avtphdr.subtype = AVTP_SUBTYPE_TSCF;
        avtp->avtphdr.ptv_M_evt = Payload->Marker;
        avtp->avtphdr.sv_ver_mr_tv = 0x81U; /* b7 av, b0 tv = 1 */
        avtp->avtphdr.sequence_num = pTalkerInfo->SeqNum;
        pTalkerInfo->SeqNum += 1U;
        avtp->avtphdr.tu = 0U;

        if (AmbaWrap_memcpy(avtp->avtphdr.stream_id, &(pTalker->StreamID), 8)!= 0U) { }
        (void)AmbaAvbStack_GetPhcNs(pAvbConfig->Idx, &PhcNs64);
        PhcNs64 %= 0xffffffffU;

        PhcNs32 = (UINT32)PhcNs64;
        //if (AmbaWrap_memcpy(&PhcNs32, &PhcNs64, sizeof(PhcNs32))!= 0U) { }
        avtp->avtphdr.avtp_timestamp = AmbaNetStack_Htonl(PhcNs32);
        avtp->avtphdr.format = 0U;
        avtp->avtphdr.format_subtype = 0U;
        avtp->avtphdr.reserved1 = 0U;
        for(i = 0; i < Payload->VecNum; i++) {
            if ((UINT32)(data_length + Payload->pVec[i].Len) > (UINT32)(sizeof(avtp->acf_msg_payload))) {
                AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "AvbAvtpTxVector: Invalid Payload len, data_length = %d + Payload->vec[i].len = %d", data_length, i, Payload->pVec[i].Len, 0U, 0U);
                AmbaPrint_Flush();
                Ret = NET_ERR_EINVAL;
                break;
            } else {
                tmp = &(avtp->acf_msg_payload[data_length]);
                if (AmbaWrap_memcpy(tmp, Payload->pVec[i].pBase, Payload->pVec[i].Len)!= 0U) { }
                data_length += (UINT16)Payload->pVec[i].Len;
            }
        }
        if (Ret == NET_ERR_NONE) {
            UINT16 AcfMsgType = 0U;
            framelen += data_length;
            avtp->avtphdr.stream_data_length = AmbaNetStack_Htons((data_length + (UINT16)sizeof(avtp->acf_msg_type_length)));
            switch (Type) {
            case AVTP_FORMAT_SUBTYPE_RESERVED_CARINFO_ACF:
                AcfMsgType = AVTP_ACF_MSG_TYPE_ACF_USER_CAN;
                break;
            case AVTP_FORMAT_SUBTYPE_RESERVED_PSD_ACF:
                AcfMsgType = AVTP_ACF_MSG_TYPE_ACF_USER_PSD;
                break;
            case AVTP_FORMAT_SUBTYPE_RESERVED_OWS_ACF:
                AcfMsgType = AVTP_ACF_MSG_TYPE_ACF_USER_OWS;
                break;
            case AVTP_FORMAT_SUBTYPE_RESERVED_GRID_ACF:
                AcfMsgType = AVTP_ACF_MSG_TYPE_ACF_USER_GRID;
                break;
            default:
                EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Invalid frame type!");
                Ret = NET_ERR_EINVAL;
                break;
            }
            if (Ret == NET_ERR_NONE) {
                avtp->acf_msg_type_length = (UINT16)((UINT32)AcfMsgType << 9) | (data_length + (UINT16)sizeof(avtp->acf_msg_type_length)); /* acf_msg_length = sizeof(acf_msg_type) + sizeof(acf_msg_length) + sizeof(acf_msg_payload) */
                (void)pAvbConfig->pDoTxCb(pAvbConfig->Idx, framelen);
            }
        }
    }
    return Ret;
}

static UINT32 ControlSubsession_FUA(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, UINT8* Buffer,
                                    UINT32 Len, UINT32 Mtu, UINT32 Timestamp, UINT32 Type)
{
    UINT32 i;
    UINT32 Segment = Len / Mtu;
    UINT32 FragLen = 0U;
    UINT32 Ret = NET_ERR_NONE;
    AMBA_AVB_AVTP_PAYLOAD_INFO_s PayloadInfo;
    AMBA_AVB_AVTP_IOVEC_s Vec[1U];

    for (i = 0U; i < Segment; i++) {
        Vec[0].pBase = &Buffer[FragLen];
        Vec[0].Len = Mtu;
        PayloadInfo.pVec = &Vec[0];
        PayloadInfo.VecNum = 1U;
        PayloadInfo.TimeStamp = Timestamp;
        PayloadInfo.Marker = 0U;
        if (i == 0U) {
            PayloadInfo.Marker = (UINT8)Segment;
            if ((Len % Mtu) != 0U) {
                PayloadInfo.Marker += 1U;
            }
        }
        Ret = ControlSubsession_TxVector(pTalkerInfo, &PayloadInfo, Type);
        if (Ret != NET_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, ControlSubsession_TxVector() failed!", __func__, NULL, NULL, NULL, NULL);
            break;
        }
        FragLen += Vec[0].Len;
    }
    if (Ret == NET_ERR_NONE) {
        if ((Len % Mtu) != 0U) {
            Vec[0].pBase = &Buffer[FragLen];
            Vec[0].Len = Len - FragLen;
            PayloadInfo.pVec = &Vec[0];
            PayloadInfo.VecNum = 1U;
            PayloadInfo.TimeStamp = Timestamp;
            PayloadInfo.Marker = 0U;
            Ret = ControlSubsession_TxVector(pTalkerInfo, &PayloadInfo, Type);
        }
    }
    return Ret;
}

static UINT32 ControlSubsession_SendPacket(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, UINT8* start_addr, UINT32 length, UINT32 timestamp, UINT32 type)
{
    UINT32 AcfPayloadMaxSize = sizeof(((AVTP_TSCF_s *)0)->acf_msg_payload);
    UINT32 Ret = NET_ERR_NONE;
    AMBA_AVB_AVTP_PAYLOAD_INFO_s PayloadInfo;
    AMBA_AVB_AVTP_IOVEC_s Vec[1U];

    if (length > AcfPayloadMaxSize) {
        (void)ControlSubsession_FUA(pTalkerInfo, start_addr, length, AcfPayloadMaxSize, timestamp, type);
    } else {
        Vec[0].pBase = start_addr;
        Vec[0].Len = length;
        PayloadInfo.pVec = &Vec[0];
        PayloadInfo.VecNum = 1U;
        PayloadInfo.TimeStamp = timestamp; // need to add - thiz->ts_offset;
        PayloadInfo.Marker = 1U;
        Ret = ControlSubsession_TxVector(pTalkerInfo, &PayloadInfo, type);
        if (Ret != NET_ERR_NONE) {
            AmbaPrint_PrintStr5("%s, ControlSubsession_TxVector() failed!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Ret;
}

UINT32 ControlSubsession_Packetize(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, const AMBA_AVB_AVTP_FRAME_INFO_s *Frame)
{
    UINT32 Ret = ControlSubsession_SendPacket(pTalkerInfo, Frame->Vec[0].pBase, Frame->Vec[0].Len, Frame->FrameTimeStamp, Frame->FrameType);
    if (Ret != NET_ERR_NONE) {
        AmbaPrint_PrintStr5("%s, ControlSubsession_SendPacket() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Ret;
}

UINT32 ControlSubsession_UnPacketize(AMBA_AVB_AVTP_LISTENER_INFO_s *pListenerInfo, const AVTP_TSCF_s *pPkt, UINT16 Len)
{
    static UINT32 SegmentTotalNum = 0U;
    static UINT32 SegmentReceiveNum = 0U;
    UINT32 AvbPayload, UnUsedBuf = 0U, Ret = NET_ERR_NONE, TimeStamp = 0U;
    AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket;
    AMBA_AVB_AVTP_FRAME_INFO_s *pFrameInfo;
    UINT8 *pBufCur;
    AMBA_AVB_QUEUE_s *pBufQueue;
    AMBA_KAL_MSG_QUEUE_t *pFrameQueue;
    AMBA_KAL_MSG_QUEUE_INFO_s MsgQueueInfo;
    void *pMsg;

    AmbaMisra_TouchUnused(&Len);

    AvbPayload = AmbaNetStack_Ntohs(pPkt->avtphdr.stream_data_length);
    // TODO: TimeStamp = AmbaNetStack_Ntohl(pPkt->h264_timestamp);

    pBufQueue = &(pListenerInfo->BufQueue);
    pFrameQueue = &(pListenerInfo->FrameQueue);
    pUnpacket = &(pListenerInfo->Unpacket);
    pFrameInfo = &(pUnpacket->FrameInfo);
    (void) AvbAvtp_BufQueueCurrentIn(pBufQueue, &pBufCur);
    (void) AvbAvtp_BufQueueUnused(pBufQueue, &UnUsedBuf);
    (void) AmbaKAL_MsgQueueQuery(pFrameQueue, &MsgQueueInfo);
    if ((MsgQueueInfo.NumAvailable > 0U) || (UnUsedBuf > (pUnpacket->TotalBytes + AvbPayload))) { /* Check Un-used frame */
        if (pPkt->avtphdr.ptv_M_evt != 0U) { /* Frame start */
            (void) AvbAvtp_UnPacketReset(pBufQueue, pUnpacket);
            if (pUnpacket->Drop == 1U) { /* new frame start -> reset SeqNum */
                pListenerInfo->SeqNum = pPkt->avtphdr.sequence_num;
            } else {
                (void) AvbAvtp_UnPacketInit(pBufQueue, pUnpacket, pBufCur, TimeStamp);
            }
            SegmentTotalNum = pPkt->avtphdr.ptv_M_evt;
            SegmentReceiveNum = 0U; /* Reset frame receive number */
            pListenerInfo->SeqNum = pPkt->avtphdr.sequence_num;
        }
        if (pListenerInfo->SeqNum != pPkt->avtphdr.sequence_num) {
            if(pUnpacket->Drop == 0U) {
                AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener SeqNum error expect[%d],receive[%d]", pListenerInfo->SeqNum, pPkt->avtphdr.sequence_num, 0U, 0U, 0U);
            }
            pUnpacket->Drop = 1U;
        } else {
            pListenerInfo->SeqNum += 1U;
            pUnpacket->Drop = 0U;
        }
        if (pUnpacket->Drop == 0U) {
            (void) AvbAvtp_UnPacketCopyData(pBufQueue, pUnpacket, &pPkt->acf_msg_payload[0], (AvbPayload - sizeof(pPkt->acf_msg_type_length)));
            SegmentReceiveNum += 1U;
            if (SegmentReceiveNum == SegmentTotalNum) { /* frame end, commit a frame */
                UINT16 AcfMsgType = pPkt->acf_msg_type_length >> 9U;
                pFrameInfo->Completed = 1U;
                switch (AcfMsgType) {
                case AVTP_ACF_MSG_TYPE_ACF_USER_CAN:
                    pFrameInfo->FrameType = AVTP_FORMAT_SUBTYPE_RESERVED_CARINFO_ACF;
                    break;
                case AVTP_ACF_MSG_TYPE_ACF_USER_PSD:
                    pFrameInfo->FrameType = AVTP_FORMAT_SUBTYPE_RESERVED_PSD_ACF;
                    break;
                case AVTP_ACF_MSG_TYPE_ACF_USER_OWS:
                    pFrameInfo->FrameType = AVTP_FORMAT_SUBTYPE_RESERVED_OWS_ACF;
                    break;
                case AVTP_ACF_MSG_TYPE_ACF_USER_GRID:
                    pFrameInfo->FrameType = AVTP_FORMAT_SUBTYPE_RESERVED_GRID_ACF;
                    break;
                default:
                    EnetInfo(__func__, __LINE__, AVB_MODULE_ID, ANSI_RED, "Invalid frame type!");
                    Ret = NET_ERR_EINVAL;
                    break;
                }
                if (Ret == NET_ERR_NONE) {
                    pFrameInfo->AvtpTimeStamp = AmbaNetStack_Ntohl(pPkt->avtphdr.avtp_timestamp);
                    Ret = AvbAvtp_BufQueueSeIn(pBufQueue, pUnpacket->TotalBytes);
                    if(Ret == NET_ERR_NONE) {
                        AmbaMisra_TypeCast32(&pMsg, &pFrameInfo);
                        Ret = AmbaKAL_MsgQueueSend(pFrameQueue, pMsg, 0U);
                        if (Ret == KAL_ERR_NONE) {
                            Ret = AvbAvtp_UnPacketReset(pBufQueue, pUnpacket);
                            if(Ret != NET_ERR_NONE) {
                               AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s, AvbAvtp_UnPacketReset() failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s, AmbaKAL_MsgQueueSend() failed!", __func__, NULL, NULL, NULL, NULL);
                            Ret = NET_ERR_EPERM;
                        }
                    } else {
                        if (AmbaAvbStack_AvtpListenerUnReg(pListenerInfo->pListener) != NET_ERR_NONE) {
                            AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s, AmbaAvbStack_AvtpListenerUnReg() failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                        Ret = NET_ERR_EPERM;
                    }
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "Avtp listener rx buffer is full ", NULL, NULL, NULL, NULL, NULL);
        Ret = NET_ERR_ENOMEM;
    }
    return Ret;
}

