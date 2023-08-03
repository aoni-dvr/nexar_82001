/**
 *  @file AvbAvtp.c
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
 *  @details AVB avtp networks stack
 *
 */
#include "AmbaKAL.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "EnetUtility.h"
#include "AvbStack.h"
#include "AvbAvtp.h"
#include "AmbaPrint.h"

AMBA_AVB_AVTP_TALKER_INFO_s     AvtpTalkerInfo[AVB_AVTP_MAX_TALKER_NUM];
AMBA_AVB_AVTP_LISTENER_INFO_s   AvtpListenerInfo[AVB_AVTP_MAX_LISTENER_NUM];

/* prototype */
UINT32 AvbAvtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen);
void AvbAvtp_TaskEntry(UINT32 Idx);

/* [AVTP automotive] pre-config dst */
//const UINT8 AVTP_DST[6] = { 0x91U, 0xe0U, 0xf0U, 0x00U, 0x0eU, 0x80U };

void AvbAvtp_TaskEntry(UINT32 Idx)
{
    (void)Idx;
}

UINT32 AvbAvtp_Rx(UINT32 Idx, const void *frame, UINT16 framelen)
{
    void *ptr = NULL;
    UINT16 Len1 = framelen;
    const AVB_AVTP_HDR_s *pkt =  NULL;
    UINT32 i;
    INT32 Value;
    const UINT8 LISTEN_WILDCARD[8] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };

    (void)Idx;
    AmbaMisra_TypeCast32(&ptr, &frame);
    (void) AmbaNetStack_RemoveEthHdr(&ptr, &Len1);

    AmbaMisra_TypeCast32(&pkt, &ptr);
    for(i = 0; i < AVB_AVTP_MAX_LISTENER_NUM; i++){
        if(AvtpListenerInfo[i].Register == 1U) {
            if (AmbaWrap_memcmp(&(AvtpListenerInfo[i].pListener->StreamID), pkt->stream_id, 8U, &Value)!= 0U) { }
            if (Value != 0) {
                if (AmbaWrap_memcmp(&(AvtpListenerInfo[i].pListener->StreamID), LISTEN_WILDCARD, 8U, &Value)!= 0U) { }
            }
            if (Value == 0) {
                if (pkt->subtype == (UINT8)AVTP_SUBTYPE_CVF) {
                    if (pkt->format_subtype == (UINT8)AVTP_FORMAT_SUBTYPE_H264) {
                        const AVTP_H264_s *PktH264 = NULL;
                        AmbaMisra_TypeCast32(&PktH264, &ptr);
                        (void) H264Subsession_UnPacketize(&(AvtpListenerInfo[i]), PktH264, Len1);
                    }
                } else if (pkt->subtype == (UINT8)AVTP_SUBTYPE_TSCF) {
                    const AVTP_TSCF_s *PktTscf = NULL;
                    AmbaMisra_TypeCast32(&PktTscf, &ptr);
                    (void) ControlSubsession_UnPacketize(&(AvtpListenerInfo[i]), PktTscf, Len1);
                } else {
                    AmbaPrint_PrintStr5("%s, Unsupport subtype!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }

    return 0;
}

UINT32 AvbAvtp_UnPacketInit(const AMBA_AVB_QUEUE_s *pQueue, AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket, UINT8 *pBuf, UINT32 TimeStamp)
{
    UINT32 Ret = NET_ERR_NONE;

    if (pQueue == NULL){
        Ret = NET_ERR_EINVAL;
    } else if (pUnpacket == NULL){
        Ret = NET_ERR_EINVAL;
    } else if (pBuf == NULL){
        Ret = NET_ERR_EINVAL;
    } else {
        pUnpacket->FrameInfo.FrameTimeStamp = TimeStamp;
        pUnpacket->FrameInfo.Vec[0].pBase = pBuf;
        pUnpacket->pCur = pBuf;
        pUnpacket->Drop = 0U;
    }

    return Ret;
}

UINT32 AvbAvtp_UnPacketReset(const AMBA_AVB_QUEUE_s *pQueue, AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket)
{
    UINT32 Ret = NET_ERR_NONE;

    if (pQueue == NULL){
        Ret = NET_ERR_EINVAL;
    } else if (pUnpacket == NULL){
        Ret = NET_ERR_EINVAL;
    } else {
        pUnpacket->FrameInfo.VecNum = 1U;
        pUnpacket->FrameInfo.Vec[0].pBase = NULL;
        pUnpacket->FrameInfo.Vec[0].Len = 0U;
        pUnpacket->FrameInfo.Vec[1].pBase = pQueue->pBase;
        pUnpacket->FrameInfo.Vec[1].Len = 0U;
        pUnpacket->FrameInfo.FrameType = 0U;
        pUnpacket->FrameInfo.FrameTimeStamp = 0U;
        pUnpacket->FrameInfo.Completed = 0U;
        pUnpacket->TotalBytes = 0U;
        pUnpacket->Drop = 0U;
        pUnpacket->pCur = NULL;
    }

    return Ret;
}

UINT32 AvbAvtp_UnPacketCopyData(const AMBA_AVB_QUEUE_s *pQueue, AMBA_AVB_AVTP_UNPACKET_INFO_s *pUnpacket, const UINT8 *pSrcBuf, UINT32 Len)
{
    UINT32 Ret = NET_ERR_NONE,Ret1 = NET_ERR_NONE;
    UINT32 MaxBufSize;
    UINT32 Vec0Index,DstIndex;
    AMBA_AVB_AVTP_FRAME_INFO_s *pFrameInfo;
    UINT8 *pDstBuf;

    if( pQueue == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else if (pUnpacket == NULL){
        Ret = NET_ERR_EINVAL;
    } else if (  (pSrcBuf == NULL) || (pUnpacket->pCur == NULL)){
        Ret = NET_ERR_EINVAL;
    } else if (Len == 0U){
        Ret = NET_ERR_EINVAL;
    } else {
        pDstBuf = pUnpacket->pCur;
        pFrameInfo = &(pUnpacket->FrameInfo);
        Ret1 += AvbAvtp_BufQueueToIndex(pQueue, pDstBuf, &DstIndex);
        Ret1 += AvbAvtp_BufQueueToIndex(pQueue, pFrameInfo->Vec[0].pBase, &Vec0Index);
        MaxBufSize = pQueue->Size;
        if(Ret1 != 0U) {
            Ret = NET_ERR_EFAULT;
        } else {
            if(pFrameInfo->VecNum == 1U){
                if( (DstIndex + Len) > MaxBufSize) {
                    pFrameInfo->VecNum = 2U;
                    pFrameInfo->Vec[0].Len = MaxBufSize - Vec0Index;
                    pFrameInfo->Vec[1].Len = Len - (MaxBufSize - DstIndex);
                    if (AmbaWrap_memcpy(pDstBuf, &(pSrcBuf[0]), (MaxBufSize - DstIndex))!= 0U) { }
                    if (AmbaWrap_memcpy(pFrameInfo->Vec[1].pBase, &pSrcBuf[MaxBufSize - DstIndex], Len - (MaxBufSize - DstIndex))!= 0U) { }
                    pUnpacket->pCur = &(pFrameInfo->Vec[1].pBase[(Len - (MaxBufSize - DstIndex))]);
                } else {
                    pFrameInfo->Vec[0].Len += Len;
                    if (AmbaWrap_memcpy(pDstBuf, &(pSrcBuf[0]), Len)!= 0U) { }
                    pUnpacket->pCur = &(pDstBuf[Len]);
                }
                pUnpacket->TotalBytes += Len;
            } else if(pFrameInfo->VecNum == 2U) {
                pFrameInfo->Vec[1].Len += Len;
                if (AmbaWrap_memcpy(pDstBuf, &(pSrcBuf[0]), Len)!= 0U) { }
                pUnpacket->pCur = &(pDstBuf[Len]);
                pUnpacket->TotalBytes += Len;
            } else {
                Ret = NET_ERR_EINVAL;
            }
        }
    }

    return Ret;
}

