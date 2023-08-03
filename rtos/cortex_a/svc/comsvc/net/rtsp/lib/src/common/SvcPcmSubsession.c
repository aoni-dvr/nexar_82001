/**
 *  @file SvcPcmSubsession.c
 *
 * Copyright (c) [2022] Ambarella International LP
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details PCM subsession module
 *
 */

#include "AmbaTypes.h"
//#include <common/common.h>
#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include "AmbaWrap.h"

#include "SvcMediaSubsession.h"
#include "SvcStreamMgr.h"
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"

static void UpdatePts(const SVC_RTSP_FRAME_DESC_s* pDesc,
                      MEDIA_SUBSESSION_s* pSub,
                      UINT32 *pTmpTs,
                      UINT32 *pFramePts,
                      UINT8 StreamRestart) {

    AMBA_STREAM_s *pStreamer = NULL;
    DOUBLE TempDouble64 = (DOUBLE)pDesc->Pts;
    UINT64 FramePtsU64;

    AmbaMisra_TypeCast(&pStreamer, &(pSub->Streamer));

    TempDouble64 = TempDouble64 * pSub->VectorClk;
    FramePtsU64 = (UINT64)TempDouble64;
    FramePtsU64 = FramePtsU64 & 0x00000000FFFFFFFFLU;
    *pFramePts = (UINT32)FramePtsU64;

    if (1U == StreamRestart) {
        pSub->CurTimestamp = SvcRtpSink_GetCurTimestamp(&pSub->Rtp, 0U);
        pSub->ChangeTick = 0U;
    } else {
        *pTmpTs = *pFramePts;
        if (*pTmpTs < pSub->LastPts) {
            *pTmpTs = *pTmpTs + (0xFFFFFFFFU - pSub->LastPts) + 1U;
        } else {
            *pTmpTs = *pTmpTs - pSub->LastPts;
        }

        if (1U == pSub->ChangeTick) {
            if(OK != SvcRtspStrmChangeTickTimer(pStreamer, pSub->TicksPerFrame, *pTmpTs)) {
                AmbaPrint_PrintUInt5("[SvcPcmSubsession]<UpdatePts>: SvcRtspStrmChangeTickTimer failed.", 0U, 0U, 0U, 0U, 0U);
            }
            pSub->TicksPerFrame = *pTmpTs;
            pSub->ChangeTick = 0U;
            AmbaPrint_PrintUInt5("[SvcPcmSubsession]<UpdatePts>: change TickPerFrame to %lu",
                pSub->TicksPerFrame, 0U, 0U, 0U, 0U);
        }

        /* Threadx UINT32 overflow will keep value 0xFFFFFFFF, handle it */
        FramePtsU64 = ((UINT64)pSub->CurTimestamp + (UINT64)pSub->TicksPerFrame);
        if (FramePtsU64 > 0x00000000FFFFFFFFLU) {
            FramePtsU64 -= 0x00000000FFFFFFFFLU;
        }
        pSub->CurTimestamp = (UINT32)FramePtsU64;
    }

    if (((*pFramePts - pSub->LastPts) != pSub->TicksPerFrame)
        && ((0U != *pFramePts) || (0U != pSub->LastPts))) {
        pSub->ChangeTick = 1U;
    }
    pSub->LastPts = *pFramePts;
}

static const char *GetFormatName(UINT32 SampleSize)
{
    AmbaPrint_PrintUInt5(">>>>>>> PCM GetFormatName( %u )", SampleSize, 0, 0, 0, 0);
    switch(SampleSize) {
        case 8: return "L8";
        case 16: return "L16";
        default:
            return 0;
    }
}

static const char* GetSdpLines(void const *thiz)
{
    MEDIA_SUBSESSION_s *pMediaSubsession = NULL;
    char const* pRetString;
    void const* pSdpLines;

    AmbaMisra_TypeCast(&pMediaSubsession, &thiz);

    if (NULL == pMediaSubsession) {
        AmbaPrint_PrintStr5("GetSdpLines: thiz == NULL", NULL, NULL, NULL, NULL, NULL);
        pRetString = NULL;
    } else if (NULL != pMediaSubsession->SdpLines){
        pRetString = pMediaSubsession->SdpLines;
    } else {
        char const* pMediaType = "audio";
        char const* pSdpServer = "c=IN IP4 0.0.0.0\r\n";
        UINT32 RtpPayloadType = 96U + pMediaSubsession->TrackNum - 1U;
        UINT32 SdpFmtSize;
        /* TODO: pass actual codec name */
        char const* const pCodecName = GetFormatName(16);//pMediaSubsession->PayloadCtx.Pcm.BitsPerSample);
        char const* const pSdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "%s"
            "b=AS:%u\r\n"
            "a=rtpmap:%u %s/%u/%u\r\n"
            "a=control:%s\r\n";

        SdpFmtSize  = AmbaUtility_StringLength(pSdpFmt);
        SdpFmtSize += AmbaUtility_StringLength(pMediaType);
        SdpFmtSize += 5U;   /* max short len*/
        SdpFmtSize += 3U;   /* max char len */
        SdpFmtSize += AmbaUtility_StringLength(pSdpServer);
        SdpFmtSize += 20U;  /* max int len  */
        SdpFmtSize += 4U;   /* format length: L8, L16*/
        SdpFmtSize += 20U;  /* max int len  */
        SdpFmtSize += 3U;   /* max char len */
        SdpFmtSize += 3U;
        SdpFmtSize += AmbaUtility_StringLength(SvcMediaSubsession_GetTrackID(pMediaSubsession));

        pSdpLines = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, SdpFmtSize + 1U);
        AmbaMisra_TypeCast(&(pMediaSubsession->SdpLines), &pSdpLines);

        if (NULL == pMediaSubsession->SdpLines) {
            AmbaPrint_PrintUInt5("GetSdpLines: malloc sdp_len(%u) fail", SdpFmtSize, 0U, 0U, 0U, 0U);
            pRetString = NULL;
        } else {
            {
                if (AmbaWrap_memset(pMediaSubsession->SdpLines, 0, SdpFmtSize)!= 0U) { }

                {
                    char    *StrBuf = pMediaSubsession->SdpLines;
                    UINT32  BufSize = SdpFmtSize;
                    UINT32  CurStrLen;

                    // "m=%s %u RTP/AVP %d\r\n"
                    // "%s"
                    // "b=AS:%u\r\n"
                    // "a=rtpmap:%u %s/%u/%u\r\n"
                    // "%s"
                    // "a=control:%s\r\n"
                    AmbaUtility_StringAppend(StrBuf, BufSize, "m=");
                    AmbaUtility_StringAppend(StrBuf, BufSize, pMediaType);
                    AmbaUtility_StringAppend(StrBuf, BufSize, " ");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)0U, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, " RTP/AVP ");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(UINT8)RtpPayloadType, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (pMediaSubsession->TrackNum > 1U)? "" : pSdpServer);
                    AmbaUtility_StringAppend(StrBuf, BufSize, "b=AS:");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)1000U, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, "a=rtpmap:");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)(UINT8)RtpPayloadType, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, " ");
                    AmbaUtility_StringAppend(StrBuf, BufSize, pCodecName);
                    AmbaUtility_StringAppend(StrBuf, BufSize, "/");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)pMediaSubsession->PayloadCtx.Pcm.SamplingFrequency, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, "/");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)pMediaSubsession->PayloadCtx.Pcm.ChannelNum, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, "a=control:");
                    AmbaUtility_StringAppend(StrBuf, BufSize, SvcMediaSubsession_GetTrackID(pMediaSubsession));
                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                }

                AmbaPrint_PrintStr5("%s, sdp %s", __func__, pMediaSubsession->SdpLines, NULL, NULL, NULL);
            }

            SvcRtspServer_MemPool_Free(SVC_RTSP_POOL_SDP_LINE_BUF, pMediaSubsession->AuxSdpLine);
            pMediaSubsession->AuxSdpLine = NULL;

            pRetString = pMediaSubsession->SdpLines;
        }
    }

    return pRetString;
}

static void CopyBytesToBE16(UINT8 *Dest, const UINT8 *Src, UINT32 Length)
{
    UINT16 *Ptr = (UINT16*)Src;
    UINT16 *End = (UINT16*)(Src + Length);
    UINT16 *Dst = (UINT16*)Dest;

    while(Ptr < End) {
        UINT16 Val = *Ptr++;
        *Dst++ = (UINT16)((Val & 0x00ffU) << 8) | (UINT16)((Val & 0xff00U) >> 8);
    }
}

static void SendPackets(MEDIA_SUBSESSION_s* pSub, UINT8* pStartAddr, UINT32 Length, UINT32 OffsetBytes, UINT32 Mark)
{
    PCM_PAYLOAD_CONTEXT_s *Pcm = &pSub->PayloadCtx.Pcm;
    UINT32 BytesPerSample = Pcm->BitsPerSample * Pcm->ChannelNum / 8;
    /* Make sure we have all channels for each sample we send */
    UINT32 PacketSize = (RTP_OVER_UDP_MAX / BytesPerSample) * BytesPerSample;
    const UINT8 * const pEndAddr = pStartAddr + Length;
    RTP_PAYLOAD_INFO_s PayloadInfo;
    RTP_IO_VEC_s Vec[1U];

    while(pStartAddr < pEndAddr) {
        UINT32 Size = pEndAddr - pStartAddr;
        if (Size > PacketSize) {
            Size = PacketSize;
        }

        Vec[0U].base = pStartAddr;
        Vec[0U].Len = Size;
        PayloadInfo.Len = Size;
        PayloadInfo.TimeStamp = pSub->CurTimestamp + OffsetBytes / BytesPerSample;
        PayloadInfo.Maker = (UINT8)Mark;
        if (ERR_NA == SvcRtpSink_SendPacketVector(&pSub->Rtp, &PayloadInfo, &Vec[0U], 1U)) {
            AmbaPrint_PrintUInt5("[SvcPcmSubsession]<SendPacket> Send Packet Vector Failed.", 0U, 0U, 0U, 0U, 0U);
        }

        OffsetBytes += Size;
        pStartAddr += Size;
    };
}

static void SendFrame(MEDIA_SUBSESSION_s* pSub, UINT8* pStartAddr, UINT32 Length, UINT32 Mark)
{
    PCM_PAYLOAD_CONTEXT_s *Pcm = &pSub->PayloadCtx.Pcm;
    UINT8 *pEndAddr = pStartAddr + Length;
    UINT32 ChunkSize;

    if (pEndAddr > pSub->pBufferLimit){
        pEndAddr = pSub->pBufferLimit;
    }

    ChunkSize = pEndAddr - pStartAddr;

    if (Pcm->BitsPerSample == 16 && Pcm->CodingLE) {
        // Temp buffer for biggest frame: 1024 sampels * 2 chan * 4 bytes/sample
        // In case we need to convert LE->BE
        static UINT8 DataBuffer[8120];

        CopyBytesToBE16(DataBuffer, pStartAddr, ChunkSize);
        if (ChunkSize < Length) {
            AmbaPrint_PrintInt5(">>>>>> Wraparound  %u / %u", ChunkSize, Length, 0, 0, 0);
            CopyBytesToBE16(DataBuffer + ChunkSize, pStartAddr + ChunkSize, Length - ChunkSize );
        }

        pStartAddr = DataBuffer;
        pEndAddr = pStartAddr + Length;
        ChunkSize = Length;
    }

    SendPackets(pSub, pStartAddr, ChunkSize, 0, Mark);
    if (ChunkSize < Length) {
        SendPackets(pSub, pSub->pBufferBase, Length - ChunkSize, ChunkSize, Mark);
    }
}

static void StreamerFunc(SVC_RTSP_FRAME_DESC_s const * Desc, void const * Ctx)
{
    MEDIA_SUBSESSION_s* pSub;
    UINT8 StreamRestart = 0;
    UINT32 TmpTs, FramePts = 0;
    UINT8 *pSrcU8;
    ULONG DescStartAddr;

    if ((NULL == Desc) || (NULL == Ctx)){
        AmbaPrint_PrintStr5("Invalid input", NULL, NULL, NULL, NULL, NULL);
    } else {
        AmbaMisra_TypeCast(&pSub, &Ctx);

        if (STREAM_MARK_DROP_FRAME == Desc->Size) {
            UpdatePts(Desc, pSub, &TmpTs, &FramePts, StreamRestart);
        } else {
            if ((Desc->Type != SVC_RTSP_FRAME_TYPE_AUDIO_FRAME) &&
                (Desc->Type != SVC_RTSP_FRAME_TYPE_EOS)) {
                AmbaPrint_PrintUInt5("invalid frame type %u", Desc->Type, 0U, 0U, 0U, 0U);
            } else {
                if ((((UINT32)Desc->Type) != SVC_RTSP_FRAME_TYPE_EOS) && (Desc->Size != 0U)) {
                    if (0U == pSub->TotalReadFrames) {
                        StreamRestart = 1U;
                    } else if (((pSub->CurTimestamp >= pSub->NextSrPts)&& ((pSub->CurTimestamp - pSub->NextSrPts) < 0xf0000000U))) {
                        pSub->Rtp.NeedToSendSrSdes = 1U;
                    } else {
                        // do nothing, just for misra-c
                    }

                    UpdatePts(Desc,pSub,&TmpTs,&FramePts,StreamRestart);

                    //if need to send sr_sdes, set next sr pts
                    if ((pSub->Rtp.NeedToSendSrSdes == 1U) || (StreamRestart == 1U)) {
                        pSub->NextSrPts = pSub->CurTimestamp + (pSub->RefClock * 5U);//min_sr_period
                    }

                    DescStartAddr = Desc->StartAddr;
                    AmbaMisra_TypeCast(&pSrcU8, &DescStartAddr);
                    SendFrame(pSub, pSrcU8, Desc->Size, Desc->Completed);
                    pSub->TotalReadFrames++;
                } else {
                    AmbaPrint_PrintStr5("Good-Bye", NULL, NULL, NULL, NULL, NULL);
                    if(ERR_NA == SvcRtcp_SendGoodBye(&pSub->Rtp)) {
                        AmbaPrint_PrintStr5("[SvcPcmSubsession]: Good-Bye sent failed", NULL, NULL, NULL, NULL, NULL);
                    }
                }
            }
        }
    }
}

/**
* create PCM Subsession.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  SVC_STREAM_LIVE_MEDIA_s or SVC_RTSP_MEDIA_TRACK_CFG_s object pointer.
*              PCM track info.
* @param [in]  whether PCM track is live.
* @return ErrorCode
*/
MEDIA_SUBSESSION_s* SvcPcmSubsession_Create(void const * pSrv, void const * pInfo, UINT32 IsLive)
{
    MEDIA_SUBSESSION_s* pSub;
    SVC_RTSP_MEDIA_TRACK_CFG_s Track;
    SVC_RTSP_MEDIA_TRACK_CFG_s* pTrk;
    SVC_STREAM_LIVE_MEDIA_s const* pLiveMedia;
    AMBA_STREAM_s *pStm;
    const SVC_RTSP_SERVER_s* pRtspSrv = NULL;
    MEDIA_SUBSESSION_s * pRetMediaSubsession;
    void const *pTempPointer;

    AmbaMisra_TypeCast(&pRtspSrv, &pSrv);

    if ((NULL == pRtspSrv) || ((0U == IsLive) && (NULL == pInfo))) {
        AmbaPrint_PrintStr5("Invalid parameter.", NULL, NULL, NULL, NULL, NULL);
        pRetMediaSubsession = NULL;
    } else {
        pTempPointer = SvcRtspServer_GetSubsessionSlot(pRtspSrv);
        AmbaMisra_TypeCast(&pSub, &pTempPointer);

        if (NULL == pSub) {
            AmbaPrint_PrintStr5("no available subsession", NULL, NULL, NULL, NULL, NULL);
            pRetMediaSubsession = NULL;
        } else {
            if (0U == IsLive) {
                AmbaMisra_TypeCast(&pTrk, &pInfo);
                pStm = SvcRtspStrmCreate();
            } else {
                AmbaMisra_TypeCast(&pLiveMedia, &pInfo);
                pStm = SvcRtspStrmBind(&Track, pLiveMedia->Stm, StreamerFunc, pSub);
                pTrk = &Track;
            }

            pRetMediaSubsession = NULL;

            if (pStm != NULL) {
                DOUBLE RefClock;
                DOUBLE nTimeScale;

                pSub->GetSdpLines = GetSdpLines;
                pSub->CodecID = pTrk->nMediaId;
                pSub->RefClock = pTrk->Info.Audio.SampleRate;
                pSub->PayloadCtx.Pcm.SamplingFrequency = pTrk->Info.Audio.SampleRate;
                pSub->PayloadCtx.Pcm.ChannelNum = pTrk->Info.Audio.Channels;
                // This is hardcoded because of broken data chain
                pSub->PayloadCtx.Pcm.BitsPerSample = 16; //pTrk->Info.Audio.BitsPerSample;
                pSub->PayloadCtx.Pcm.CodingLE = 1;

                pSub->Streamer = pStm;
                pSub->IsLive = (UINT8)IsLive;
                pSub->MaxTransport = (0U == pSub->IsLive) ? 1U : MAX_SUBSESSION_TRANSPORT; //prevent from multi-playback on same file

                // streamerFunc need these informations
                RefClock = (DOUBLE)pSub->RefClock;
                nTimeScale = (DOUBLE)pTrk->nTimeScale;

                pSub->VectorClk = RefClock / nTimeScale;
                pSub->TicksPerFrame = pTrk->nTimePerFrame * pSub->RefClock / pTrk->nTimeScale;
                pSub->pBufferBase = pTrk->pBufferBase;
                pSub->pBufferLimit = pTrk->pBufferLimit;
                pRetMediaSubsession = pSub;
            }
        }
    }
    return pRetMediaSubsession;
}
