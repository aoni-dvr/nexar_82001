/**
 *  @file H264Subsession.c
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
 *  @details AVB H264 subsession
 *
 */
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "AvbAvtp.h"


static UINT32 H264Subsession_TxVector(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, const AMBA_AVB_AVTP_PAYLOAD_INFO_s *Payload)
{
    const AMBA_AVB_STACK_CONFIG_s *pAvbConfig;
    const AMBA_AVB_AVTP_TALKER_s *pTalker;
    void *ptr;
    AVTP_H264_s *avtp;
    UINT16 framelen = (UINT16)(sizeof(AVB_AVTP_HDR_s) + 4U);
    UINT32 PhcNs32;
    UINT64 PhcNs64;
    ETH_HDR_s *ethhdr;
    UINT8 i;
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
        avtp->avtphdr.subtype = AVTP_SUBTYPE_CVF;
        /* b7 av, b0 tv = 1 */
        avtp->avtphdr.sv_ver_mr_tv = 0x81U;
        avtp->avtphdr.sequence_num = pTalkerInfo->SeqNum;
        pTalkerInfo->SeqNum += 1U;
        avtp->avtphdr.tu = 0U;
        if (AmbaWrap_memcpy(avtp->avtphdr.stream_id, &(pTalker->StreamID), 8)!= 0U) { }
        (void)AmbaAvbStack_GetPhcNs(pAvbConfig->Idx, &PhcNs64);
        PhcNs64 %= 0xffffffffU;

        PhcNs32 = (UINT32)PhcNs64;
        //if (AmbaWrap_memcpy(&PhcNs32, &PhcNs64, sizeof(PhcNs32))!= 0U) { }
        /* for accuracy: get timstamp from dsp isr, not here */
        avtp->avtphdr.avtp_timestamp = AmbaNetStack_Htonl(PhcNs32);

        avtp->avtphdr.format = AVTP_FORMAT_RFC;
        avtp->avtphdr.format_subtype = AVTP_FORMAT_SUBTYPE_H264;
        /* b6 ptv = 1 */
        if(Payload->Marker == 0U) {
            avtp->avtphdr.ptv_M_evt = 0x40U;
        } else {
            avtp->avtphdr.ptv_M_evt = (UINT8)(0x40U|0x10U);
        }

        /* h264 hook function inserts:
         * 1. stream id
         * 2. stream_data_length
         * 3. marker bit, ptv bit
         * 4. h264_timestamp
         * 5. h264_payload
         */
        for(i = 0; i < Payload->VecNum; i++) {
            if( (UINT32)(data_length + Payload->pVec[i].Len) > (UINT32)(sizeof(avtp->h264_payload))) {
                AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "AvbAvtpTxVector: Invalid Payload len, data_length = %d + Payload->vec[i].len = %d", data_length, i, Payload->pVec[i].Len, 0U, 0U);
                AmbaPrint_Flush();
                Ret = NET_ERR_EINVAL;
                break;
            } else {
                tmp = &(avtp->h264_payload[data_length]);
                if (AmbaWrap_memcpy(tmp, Payload->pVec[i].pBase, Payload->pVec[i].Len)!= 0U) { }
                data_length += (UINT16)Payload->pVec[i].Len;
            }
        }

        if(Ret == 0U) {
            /* stream_data_length includes h264_timestamp and h264_payload */
            framelen += data_length;
            avtp->avtphdr.stream_data_length = AmbaNetStack_Htons(data_length);
            avtp->h264_timestamp = AmbaNetStack_Htonl(Payload->TimeStamp);
            (void)pAvbConfig->pDoTxCb(pAvbConfig->Idx, framelen);
            (void)avtp->avtphdr.reserved1;
            (void)avtp->avtphdr.reserved2;
            (void)avtp->h264_payload;
        }
    }

    return Ret;
}

static UINT32 H264Subsession_FUA(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, UINT8* buf,
                            UINT32 len, UINT32 mtu, UINT8 Completed, UINT32 timestamp)
{
    UINT32 frag_len;
    UINT32 remain = len;
    const UINT8* nalu = buf;
    UINT8* payload = &buf[1];// skip the NAL byte
    UINT8 fua_indicator = ((nalu[0]&0xe0U) | 0x1CU);
    UINT8 fua_header = (nalu[0] & 0x1fU);
    UINT8 start = 1U;
    UINT8 fua[2];   // fua_indicator + fua_header
    AMBA_AVB_AVTP_PAYLOAD_INFO_s payloadInfo;
    AMBA_AVB_AVTP_IOVEC_s vec[2];

    remain--;
    while(remain > 0U) {
        fua[0] = fua_indicator;
        if(start == 1U) {
            fua[1] = fua_header | (1U << 7U);
            start = 0U;
        } else {
            fua[1] = fua_header;
        }
        if(remain > (mtu - 2U) ) {
            frag_len = (mtu - 2U);
        } else {
            frag_len = remain;
            fua[1] = fua_header | (1U << 6U);
        }

        vec[0].pBase = fua;
        vec[0].Len = 2U;
        vec[1].pBase = payload;
        vec[1].Len = frag_len;
        payloadInfo.pVec = &vec[0];
        payloadInfo.VecNum = 2U;
        payloadInfo.TimeStamp = timestamp;
        if( (remain <= frag_len) && (Completed == 1U) ) {
            payloadInfo.Marker = 1U;
        } else {
            payloadInfo.Marker = 0U;
        }
        //AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "fua_buf = %x fua_len = %x ", (UINT32)(payloadInfo.fua_buf), payloadInfo.fua_len, 0U, 0U, 0U);
        //AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "buf = %x len = %x mark = %d timestamp = %d", (UINT32)(payloadInfo.buf), payloadInfo.len, payloadInfo.maker, payloadInfo.timestamp, 0U);
        //AmbaPrint_Flush();
        (void)H264Subsession_TxVector(pTalkerInfo, &payloadInfo);
        payload = &payload[frag_len];
        remain -= frag_len;
        //nalu += frag_len;
    }
    return NET_ERR_NONE;
}

static UINT32 H264Subsession_SendPacket(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, UINT8* start_addr, UINT32 length, UINT8 Completed, UINT32 timestamp)
{
    AMBA_AVB_AVTP_PAYLOAD_INFO_s payloadInfo;
    AMBA_AVB_AVTP_IOVEC_s vec[1];

    if (length > AVB_PAYLOAD_MAX) {
        (void)H264Subsession_FUA(pTalkerInfo, start_addr, length, AVB_PAYLOAD_MAX, Completed, timestamp);
    } else {
        vec[0].pBase = start_addr;
        vec[0].Len = length;
        payloadInfo.pVec = &vec[0];
        payloadInfo.VecNum = 1U;
        payloadInfo.TimeStamp = timestamp; // need to add - thiz->ts_offset;
        payloadInfo.Marker = (Completed & 0x01U);
        //AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "buf = %x len = %x mark = %d timestamp = %d", (UINT32)(payloadInfo.buf), payloadInfo.len, payloadInfo.maker, payloadInfo.timestamp, 0U);
        //AmbaPrint_Flush();
        (void)H264Subsession_TxVector(pTalkerInfo, &payloadInfo);
    }

    return NET_ERR_NONE;
}

UINT32 H264Subsession_Packetize(AMBA_AVB_AVTP_TALKER_INFO_s *pTalkerInfo, const AMBA_AVB_AVTP_FRAME_INFO_s *Frame)
{
    UINT8 *p, *last_start,*pBuf;
    const UINT8 *q;
    UINT32 w;
    UINT8  nalu,nalu_type;
    UINT32 frame_len;
    UINT32 Ret = NET_ERR_NONE;
    UINT32 last_len;
    UINT32 Tmp1 = 0U,Tmp2 = 0U;

    pBuf = Frame->Vec[0].pBase;
    frame_len = Frame->Vec[0].Len;
    w = ((UINT32)pBuf[3]) | ((UINT32)pBuf[2] << 8U) | ((UINT32)pBuf[1] << 16U) | ((UINT32)pBuf[0] << 24U);
    // normal case, find start code
    if(Frame->VecNum != 1U) {
        AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s: Tx not support VecNum != 1", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Ret = NET_ERR_EINVAL;
    } else if (w != 0x00000001U) {
        AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s: cannot find start code", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        Ret = NET_ERR_EINVAL;
    } else {
        // skip the start code
        last_start = &pBuf[4];

        p = last_start;
        q = &pBuf[frame_len];
        // it's a hack for amba solution, in bitstream from amba,
        // every frame only have one slice nalu, the slice nalu alway be the last nalu.
        if (AmbaWrap_memcpy(&nalu, &last_start[0], 1U)!= 0U) { }
        nalu_type = nalu & NALU_TYPE_MASK;
        if( nalu_type >= 6U ) { //it's not slice nalu
            AmbaMisra_TypeCast32(&Tmp1, &p);
            AmbaMisra_TypeCast32(&Tmp2, &q);
            while ( Tmp1 < Tmp2 ) {
                w = ((UINT32)p[3]) | ((UINT32)p[2] << 8U) | ((UINT32)p[1] << 16U) | ((UINT32)p[0] << 24U);
                if ( (w==0x00000001U) || ( (w & 0xffffff00U)==0x00000100U) ) {//got next nalu, p point to next start-code

                    if( (nalu_type == 0x1aU) || (nalu_type == 0x09U) ) {
                        break; //omit AMBA proprietary GOP Header and omit AU-delimiter
                    //} else if(nalu.type == 0x06U && OMIT_SEI(sub)) {
                    //    break; //omit SEI in Intra Refresh mode
                    } else {
                        AmbaMisra_TypeCast32(&Tmp1, &p);
                        AmbaMisra_TypeCast32(&Tmp2, &last_start);
                        (void)H264Subsession_SendPacket(pTalkerInfo, last_start, (UINT32)(Tmp1 - Tmp2), 0U, Frame->FrameTimeStamp);
                    }

                    if(p[3] == 0x01U) {
                        // skip 4, for next search
                        p = &p[4];
                    } else {
                        p = &p[3];
                    }
                    last_start = p;
                    if (AmbaWrap_memcpy(&nalu, &last_start[0], 1U)!= 0U) { }
                    nalu_type = nalu & NALU_TYPE_MASK;
                    if(( (nalu_type > 0U) && (nalu_type < 6U) )){
                        p = &pBuf[frame_len];
                    }
                } else if(p[3] > 1U) {
                    p = &p[4];
                } else if (p[2] > 1U){
                    p = &p[3];
                } else if (p[1] > 1U){
                    p = &p[2];
                } else {
                    p++;
                }
                AmbaMisra_TypeCast32(&Tmp1, &p);
                AmbaMisra_TypeCast32(&Tmp2, &q);
            }
        }
        //last nalu
        AmbaMisra_TypeCast32(&Tmp1, &q);
        AmbaMisra_TypeCast32(&Tmp2, &last_start);
        last_len = (UINT32)(Tmp1 - Tmp2);
        (void)H264Subsession_SendPacket(pTalkerInfo, last_start, last_len, Frame->Completed, Frame->FrameTimeStamp);
    }

    return Ret;
}


UINT32 H264Subsession_UnPacketize(AMBA_AVB_AVTP_LISTENER_INFO_s *pListenerInfo,const AVTP_H264_s *pPkt, UINT16 Len)
{
    UINT32 AvbPayload, UnUsedBuf = 0U, Ret = NET_ERR_NONE, Ret1,TimeStamp;
    AMBA_AVB_AVTP_UNPACKET_INFO_s   *pUnpacket;
    AMBA_AVB_AVTP_FRAME_INFO_s      *pFrameInfo;
    UINT8                           *pBufCur;
    AMBA_AVB_QUEUE_s                *pBufQueue;
    AMBA_KAL_MSG_QUEUE_t            *pFrameQueue;
    AMBA_KAL_MSG_QUEUE_INFO_s       MsgQueueInfo;

    const UINT8 nalu_type = pPkt->h264_payload[AVTP_NALU] & AVTP_NALU_TYPE_MASK;
    const UINT8 startcode[4] = {0x00, 0x00, 0x00, 0x01};
    UINT8 Nalu = 0U;
    void *pMsg;

    //AvbPayload = RxLen - avtphdr - h264_timestamp = payload
    AvbPayload = AmbaNetStack_Ntohs(pPkt->avtphdr.stream_data_length);
    TimeStamp = AmbaNetStack_Ntohl(pPkt->h264_timestamp);

    //Get Info
    pBufQueue = &(pListenerInfo->BufQueue);
    pFrameQueue = &(pListenerInfo->FrameQueue);
    pUnpacket = &(pListenerInfo->Unpacket);
    pFrameInfo = &(pUnpacket->FrameInfo);
    (void) AvbAvtp_BufQueueCurrentIn(pBufQueue, &pBufCur);
    (void) AvbAvtp_BufQueueUnused(pBufQueue, &UnUsedBuf);
    (void) AmbaKAL_MsgQueueQuery(pFrameQueue, &MsgQueueInfo);

    //Find first SPS pkt
    if ( (pListenerInfo->FirstIdr == 0U) && (nalu_type != AVTP_NALU_TYPE_SPS) ) {

    } else if ( AvbPayload > ((UINT32)Len - (sizeof(AVB_AVTP_HDR_s) + 4U) ) ){                    //Check payload len
        if(pUnpacket->Drop == 0U) {
            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener payload len error expect[%d],receive[%d]", AvbPayload, ((UINT32)Len - (sizeof(AVB_AVTP_HDR_s) + 4U) ), 0U, 0U, 0U);
        }
        pUnpacket->Drop = 1U;
    } else if( (nalu_type > AVTP_NALU_TYPE_PPS) && (nalu_type != AVTP_NALU_TYPE_FUA) ) {    //Check NALU type
        if(pUnpacket->Drop == 0U) {
            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Avtp listener unknow nalu type", nalu_type, 0U, 0U, 0U, 0U);
        }
    } else if( (MsgQueueInfo.NumAvailable == 0U) || (UnUsedBuf < (pUnpacket->TotalBytes + AvbPayload)) ) {          //Check Un-used frame
        if(pUnpacket->Drop == 0U) {
            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener rx buffer is full ", 0U, 0U, 0U, 0U, 0U);
        }
        pUnpacket->Drop = 1U;
    } else {
        pListenerInfo->FirstIdr = 1U;

        // First packet in frame
        if( (nalu_type == AVTP_NALU_TYPE_SPS) && (pUnpacket->TotalBytes== 0U) ) {              //SPS+ PPS + IDR frame
           (void) AvbAvtp_UnPacketInit(pBufQueue, pUnpacket, pBufCur, TimeStamp);
           pListenerInfo->SeqNum = pPkt->avtphdr.sequence_num + 1U;
        } else if( (nalu_type == AVTP_NALU_TYPE_SEI) && (pUnpacket->TotalBytes == 0U) ) {       //non-IDR frame with SEI
            (void) AvbAvtp_UnPacketInit(pBufQueue, pUnpacket, pBufCur, TimeStamp);
            pListenerInfo->SeqNum = pPkt->avtphdr.sequence_num + 1U;
        } else if( (nalu_type < AVTP_NALU_TYPE_SEI) && (pUnpacket->TotalBytes == 0U) ) {        //non-IDR frame without SEI
            (void) AvbAvtp_UnPacketInit(pBufQueue, pUnpacket, pBufCur, TimeStamp);
            pListenerInfo->SeqNum = pPkt->avtphdr.sequence_num + 1U;
        } else if( (nalu_type == AVTP_NALU_TYPE_FUA) && (pUnpacket->TotalBytes == 0U) ) {       //non-IDR frame (FUA) without SEI
            if((pPkt->h264_payload[AVTP_FUA_HEAD] & AVTP_FUA_HEAD_SBIT_MASK) != 0U) {
                (void) AvbAvtp_UnPacketInit(pBufQueue, pUnpacket, pBufCur, TimeStamp);
                pListenerInfo->SeqNum = pPkt->avtphdr.sequence_num + 1U;
            } else {
                pUnpacket->Drop = 1U;
            }
        } else if(pUnpacket->TotalBytes == 0U){
            pUnpacket->Drop = 1U;
        } else {
            // Check seqnum and timestamp in same frame
            if(pUnpacket->FrameInfo.FrameTimeStamp != TimeStamp) {
                if(pUnpacket->Drop == 0U) {
                    AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener timestamp error expect[%d],receive[%d]", pUnpacket->FrameInfo.FrameTimeStamp, TimeStamp, 0U, 0U, 0U);
                }
                pUnpacket->Drop = 1U;
            }

            if(pListenerInfo->SeqNum != pPkt->avtphdr.sequence_num) {
                if(pUnpacket->Drop == 0U) {
                    AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener SeqNum error expect[%d],receive[%d]", pListenerInfo->SeqNum, pPkt->avtphdr.sequence_num, 0U, 0U, 0U);
                }
                pUnpacket->Drop = 1U;
            } else {
                pListenerInfo->SeqNum += 1U;
            }
        }

        //Skip packet
        if( (pUnpacket->Drop == 1U) && ((pPkt->avtphdr.ptv_M_evt&0x10U) == 0U) ) {          //not last packet in drop frame
        } else if( (pUnpacket->Drop == 1U) && ((pPkt->avtphdr.ptv_M_evt&0x10U) != 0U) ){    //last packet in drop frame
            (void) AvbAvtp_UnPacketReset(pBufQueue, pUnpacket);
        } else {
            //Fill packet in frame
            if(nalu_type <= AVTP_NALU_TYPE_PPS) {       //non-fua
                //Insert start code
                Ret1 = AvbAvtp_UnPacketCopyData(pBufQueue, pUnpacket, &startcode[0], sizeof(startcode));
                if( Ret1 != 0U ) {
                    if(pUnpacket->Drop == 0U) {
                        AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener line(%d) copy data error ret = %d", __LINE__, Ret1, 0U, 0U, 0U);
                    }
                    pUnpacket->Drop = 1U;
                    Ret = NET_ERR_EPERM;
                } else {
                    //Insert payload
                    Ret1 = AvbAvtp_UnPacketCopyData(pBufQueue, pUnpacket, &pPkt->h264_payload[0], AvbPayload);
                    if( Ret1  != 0U ) {
                        if(pUnpacket->Drop == 0U) {
                            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener line(%d) copy data error ret = %d", __LINE__, Ret1, 0U, 0U, 0U);
                        }
                        pUnpacket->Drop = 1U;
                        Ret = NET_ERR_EPERM;
                    }
                }
            } else {   //fua; because nalu_type has been checked in previous lines, it must equal to AVTP_NALU_TYPE_FUA
                if( (pPkt->h264_payload[AVTP_FUA_HEAD] & AVTP_FUA_HEAD_SBIT_MASK) != 0U) {// First FUA pkg, insert start code
                    //Insert start code
                    if( AvbAvtp_UnPacketCopyData(pBufQueue, pUnpacket, &startcode[0], sizeof(startcode)) != 0U ) {
                        pUnpacket->Drop = 1U;
                        Ret = NET_ERR_EPERM;
                    } else {
                        //Insert NALU header
                        Nalu = (pPkt->h264_payload[AVTP_FUA_IDENT] & (UINT8)(AVTP_FUA_IDENT_FBIT_MASK | AVTP_FUA_IDENT_REF_IDC_MASK));
                        Nalu |= (pPkt->h264_payload[AVTP_FUA_HEAD] & (UINT8)(AVTP_FUA_HEAD_UNIT_TYPE_MASK));
                        Ret1 = AvbAvtp_UnPacketCopyData(pBufQueue, pUnpacket, &Nalu, sizeof(Nalu));
                        if( Ret1 != 0U ) {
                            if(pUnpacket->Drop == 0U) {
                                AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener line(%d) copy data error ret = %d", __LINE__, Ret1, 0U, 0U, 0U);
                            }
                            pUnpacket->Drop = 1U;
                            Ret = NET_ERR_EPERM;
                        }
                    }
                }

                if(Ret == 0U)
                {
                    //Remove FUA Identifier and Header and Insert payload
                    Ret1 = AvbAvtp_UnPacketCopyData(pBufQueue, pUnpacket, &pPkt->h264_payload[2], AvbPayload - 2U);
                    if( Ret1 != 0U ) {
                        if(pUnpacket->Drop == 0U) {
                            AmbaPrint_ModulePrintUInt5(AVB_MODULE_ID, "Rrop frame : Avtp listener line(%d) copy data error ret = %d", __LINE__, Ret1, 0U, 0U, 0U);
                        }
                        pUnpacket->Drop = 1U;
                        Ret = NET_ERR_EPERM;
                    }
                }
            }

            if(Ret == 0U) {
                if( (pPkt->avtphdr.ptv_M_evt&0x10U) != 0U) {
                    pFrameInfo->Completed = 1U;
                    pFrameInfo->FrameType = AVTP_FORMAT_SUBTYPE_H264;
                    pFrameInfo->AvtpTimeStamp = AmbaNetStack_Ntohl(pPkt->avtphdr.avtp_timestamp);
                    Ret = AvbAvtp_BufQueueSeIn(pBufQueue, pUnpacket->TotalBytes);
                    if(Ret != 0U) {
                        Ret = NET_ERR_EPERM;
                    } else {
                        AmbaMisra_TypeCast32(&pMsg, &pFrameInfo);
                        Ret = AmbaKAL_MsgQueueSend(pFrameQueue, pMsg, 0U);
                        (void) AvbAvtp_UnPacketReset(pBufQueue, pUnpacket);
                        if(Ret != 0U) {
                            Ret = NET_ERR_EPERM;
                        }
                    }
                }
            }
        }
    }

    if(Ret != 0U) {
        (void) AmbaAvbStack_AvtpListenerUnReg(pListenerInfo->pListener);
    }

    return Ret;
}


