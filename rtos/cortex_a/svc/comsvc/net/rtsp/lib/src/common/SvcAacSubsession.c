/**
 *  @file SvcAacSubsession.c
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
 *  @details AAC subsession module
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

#define RTSP_AAC_MAX_FRAME_SIZE 0x00F00000U


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
                AmbaPrint_PrintUInt5("[SvcAacSubsession]<UpdatePts>: SvcRtspStrmChangeTickTimer failed.", 0U, 0U, 0U, 0U, 0U);
            }
            pSub->TicksPerFrame = *pTmpTs;
            pSub->ChangeTick = 0U;
            AmbaPrint_PrintUInt5("[SvcAacSubsession]<UpdatePts>: change TickPerFrame to %lu",
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

static UINT32 GetConfigString(MEDIA_SUBSESSION_s *thiz, char* Buf)
{
    static const UINT8 FixedHeader[4]={0xff, 0xf1, 0x4c, 0x80}; // it's actually 3.5 bytes long. Fix data for Amba AAC
    const UINT32 Profile = ((((UINT32)(FixedHeader[2])) & 0xC0U) >> 6U); // 2 bits
    UINT32 Samplerate=0;
    UINT32 SamplingFrequencyIndex;
    UINT32 AudioSpecificConfig[2];
    UINT32 AudioObjectType;
    UINT32 RetVal = OK;
    static const UINT32 SamplingFrequencyTable[16U] = {
      96000U, 88200U, 64000U, 48000U,
      44100U, 32000U, 24000U, 22050U,
      16000U, 12000U, 11025U, 8000U,
      7350U,  0U,     0U,     0U
    };


    if (3U == Profile) {
        AmbaPrint_PrintStr5("Bad (reserved) 'profile': 3 in first frame of ADTS file\n", NULL, NULL, NULL, NULL, NULL);
        RetVal = ERR_NA;
    } else {

        Samplerate = thiz->PayloadCtx.Aac.SamplingFrequency;

        for (SamplingFrequencyIndex = 0U; SamplingFrequencyIndex < 16U; SamplingFrequencyIndex++) {
            if (SamplingFrequencyTable[SamplingFrequencyIndex] == Samplerate) {
                thiz->PayloadCtx.Aac.SamplingFrequency = Samplerate;
                break;
            }
        }

        if (SamplingFrequencyIndex >= 16U) { //cannot found related index
            AmbaPrint_PrintStr5("Bad 'sampling_frequency_index' in first frame of ADTS file\n", NULL, NULL, NULL, NULL, NULL);
            RetVal = ERR_NA;
        } else {

            // Construct the 'AudioSpecificConfig', and from it, the corresponding ASCII string:
            AudioObjectType = Profile + 1U;

            AudioSpecificConfig[0U] = (AudioObjectType << 3U) | (SamplingFrequencyIndex >> 1U);
            AudioSpecificConfig[1U] = (SamplingFrequencyIndex << 7U) | (thiz->PayloadCtx.Aac.ChannelConfiguration << 3U);

            {
                if (AmbaWrap_memset(Buf, 0, 32UL)!= 0U) { }

                // (void)AmbaAdvSnPrint(Buf, 32U, "%02X%02x", AudioSpecificConfig[0U] & 0xFFU, AudioSpecificConfig[1U] & 0xFFU);
                {
                    char    *StrBuf = Buf;
                    UINT32  BufSize = 32U;
                    UINT32  CurStrLen;

                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        char    NumBuf[16];
                        UINT32  NumLen;

                        if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                        (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)((UINT64)AudioSpecificConfig[0U] & 0xFFULL), 16U);
                        NumLen = AmbaUtility_StringLength(NumBuf);

                        while (NumLen < 2UL) {
                            AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                            NumLen++;
                        }

                        AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        char    NumBuf[16];
                        UINT32  NumLen;

                        if (AmbaWrap_memset(NumBuf, 0, sizeof(NumBuf))!= 0U) { }
                        (void)AmbaUtility_UInt64ToStr(NumBuf, sizeof(NumBuf), (UINT64)((UINT64)AudioSpecificConfig[1U] & 0xFFULL), 16U);
                        NumLen = AmbaUtility_StringLength(NumBuf);

                        while (NumLen < 2UL) {
                            AmbaUtility_StringAppend(StrBuf, BufSize, "0");
                            NumLen++;
                        }

                        AmbaUtility_StringAppend(StrBuf, BufSize, NumBuf);
                    }
                    AmbaUtility_StringAppend(StrBuf, BufSize, "");
                }

                AmbaPrint_PrintStr5("%s, Buf %s", __func__, Buf, NULL, NULL, NULL);
            }
        }
    }

    return RetVal;
}

static const char* GetAuxSdp(MEDIA_SUBSESSION_s* thiz)
{
    char const* pFmtMode = "AAC-hbr";
    char const* pEmptyConfString = "\"\"";
    char pConfigBuffer[32];
    void const* pAuxSdpLine;
    UINT32 FmtpFmtSize;
    char const* FmtpFmt =
            "a=fmtp:%d "
            "streamtype=5;profile-level-id=1;"
            "mode=%s;sizelength=13;indexlength=3;indexdeltalength=3;"
            "config=%s\r\n";

    if (AmbaWrap_memset(pConfigBuffer, 0, sizeof(pConfigBuffer))!= 0U) { }

    if(NULL == thiz->AuxSdpLine) {

        if (GetConfigString(thiz, pConfigBuffer) != OK) {
            AmbaUtility_StringCopy(pConfigBuffer, sizeof(pConfigBuffer), pEmptyConfString);
        }

        FmtpFmtSize  = AmbaUtility_StringLength(FmtpFmt);
        FmtpFmtSize += 3U; /* max char len */
        FmtpFmtSize += 3U; /* max char len */
        FmtpFmtSize += AmbaUtility_StringLength(pFmtMode);
        FmtpFmtSize += (UINT32)AmbaUtility_StringLength((char const*)pConfigBuffer);

        pAuxSdpLine = SvcRtspServer_MemPool_Malloc(SVC_RTSP_POOL_SDP_LINE_BUF, FmtpFmtSize + 1U);
        AmbaMisra_TypeCast(&(thiz->AuxSdpLine), &pAuxSdpLine);

        if(NULL != thiz->AuxSdpLine) {
            if (AmbaWrap_memset(thiz->AuxSdpLine, 0, FmtpFmtSize)!= 0U) { }

            {
                UINT32  CurStrLen;

                // "a=fmtp:%d "
                // "streamtype=5;profile-level-id=1;"
                // "mode=%s;sizelength=13;indexlength=3;indexdeltalength=3;"
                // "config=%s\r\n"
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, "a=fmtp:");
                CurStrLen = AmbaUtility_StringLength(thiz->AuxSdpLine);
                if (CurStrLen < FmtpFmtSize) {
                    (void)AmbaUtility_UInt64ToStr(&(thiz->AuxSdpLine[CurStrLen]), FmtpFmtSize - CurStrLen, (UINT64)(96ULL + (UINT64)thiz->TrackNum - 1ULL), 10U);
                }
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, " ");
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, "streamtype=5;profile-level-id=1;");
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, "mode=");
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, pFmtMode);
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, ";sizelength=13;indexlength=3;indexdeltalength=3;");
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, "config=");
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, pConfigBuffer);
                AmbaUtility_StringAppend(thiz->AuxSdpLine, FmtpFmtSize, "\r\n");
            }

            AmbaPrint_PrintStr5("%s, aux sdp %s", __func__, thiz->AuxSdpLine, NULL, NULL, NULL);
        }
    }

    return thiz->AuxSdpLine;
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
        char const* pAuxSDPLine = GetAuxSdp(pMediaSubsession);
        char const* const pSdpFmt =
            "m=%s %u RTP/AVP %d\r\n"
            "%s"
            "b=AS:%u\r\n"
            "a=rtpmap:%u MPEG4-GENERIC/%u/%u\r\n"
            "%s"
            "a=control:%s\r\n";

        if (NULL == pAuxSDPLine) {
            AmbaPrint_PrintStr5("AAC - GetSdpLines: auxSDPLine == NULL", NULL, NULL, NULL, NULL, NULL);
        }

        SdpFmtSize  = AmbaUtility_StringLength(pSdpFmt);
        SdpFmtSize += AmbaUtility_StringLength(pMediaType);
        SdpFmtSize += 5U;   /* max short len*/
        SdpFmtSize += 3U;   /* max char len */
        SdpFmtSize += AmbaUtility_StringLength(pSdpServer);
        SdpFmtSize += 20U;  /* max int len  */
        SdpFmtSize += 20U;  /* max int len  */
        SdpFmtSize += 3U;   /* max char len */
        SdpFmtSize += 3U;
        SdpFmtSize += ((NULL == pAuxSDPLine)? 0U:AmbaUtility_StringLength(pAuxSDPLine));
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
                    // "a=rtpmap:%u MPEG4-GENERIC/%u/%u\r\n"
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

                    AmbaUtility_StringAppend(StrBuf, BufSize, " MPEG4-GENERIC/");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)pMediaSubsession->PayloadCtx.Aac.SamplingFrequency, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, "/");

                    CurStrLen = AmbaUtility_StringLength(StrBuf);
                    if (CurStrLen < BufSize) {
                        (void)AmbaUtility_UInt64ToStr(&(StrBuf[CurStrLen]), BufSize - CurStrLen, (UINT64)pMediaSubsession->PayloadCtx.Aac.ChannelConfiguration, 10U);
                    }

                    AmbaUtility_StringAppend(StrBuf, BufSize, "\r\n");
                    AmbaUtility_StringAppend(StrBuf, BufSize, (pAuxSDPLine==NULL)? "":pAuxSDPLine);
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

static void SendPacket(MEDIA_SUBSESSION_s* pSub, UINT8* pStartAddr, UINT32 Length, UINT32 Mark)
{
    UINT8 AUHeader[4U];
    RTP_PAYLOAD_INFO_s PayloadInfo;
    RTP_IO_VEC_s Vec[3U];
    ULONG StartAddr, EndAddr, BufferLimitAddr;

    // Set the "AU Header Section".  This is 4 bytes: 2 bytes for the
    // initial "AU-headers-length" field, and 2 bytes for the first
    // (and only) "AU Header":
    AUHeader[0U] = 0U;
    AUHeader[1U] = 16U; // AU-headers-length in bits
    AUHeader[2U] = (UINT8)(Length >> 5U);
    AUHeader[3U] = (UINT8)((Length & 0x1FU) << 3U);


    AmbaMisra_TypeCast(&StartAddr, &pStartAddr);
    EndAddr = StartAddr + (Length * sizeof(pStartAddr[0]));
    AmbaMisra_TypeCast(&BufferLimitAddr, &pSub->pBufferLimit);

    if(Length > RTP_OVER_UDP_MAX) {
        AmbaPrint_PrintUInt5("Audio Frame size (%d) too big (limit:%d)!!\n", Length, RTP_OVER_UDP_MAX, 0U, 0U, 0U);
    } else if (EndAddr <= BufferLimitAddr) {
        Vec[0U].base = AUHeader;
        Vec[0U].Len = 4U; //AAC AU Header
        Vec[1U].base = pStartAddr;
        Vec[1U].Len = Length;
        PayloadInfo.Len = Length;
        PayloadInfo.TimeStamp = pSub->CurTimestamp;// need to add - priv->ts_offset;
        PayloadInfo.Maker = (UINT8)Mark;
        if (ERR_NA == SvcRtpSink_SendPacketVector(&pSub->Rtp, &PayloadInfo, &Vec[0U], 2U)) {
            AmbaPrint_PrintUInt5("[SvcAacSubsession]<SendPacket> Send Packet Vector Failed.", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        Vec[0U].base = AUHeader;
        Vec[0U].Len = 4U;
        Vec[1U].base = pStartAddr;
        Vec[1U].Len = BufferLimitAddr - StartAddr;
        Vec[2U].base = pSub->pBufferBase;
        Vec[2U].Len = Length - Vec[1U].Len;
        PayloadInfo.Len = Length;
        PayloadInfo.TimeStamp = pSub->CurTimestamp;// need to add - priv->ts_offset;
        PayloadInfo.Maker = (UINT8)Mark;
        if(ERR_NA == SvcRtpSink_SendPacketVector(&pSub->Rtp, &PayloadInfo, &Vec[0U], 3U)) {
            AmbaPrint_PrintUInt5("[SvcAacSubsession]<SendPacket> Send Packet Vector Failed..", 0U, 0U, 0U, 0U, 0U);
        }
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
                if ((((UINT32)Desc->Type) != SVC_RTSP_FRAME_TYPE_EOS)
                    && ((Desc->Size < RTSP_AAC_MAX_FRAME_SIZE) || (Desc->Size != 0U))) {
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
                    SendPacket(pSub, pSrcU8, Desc->Size, Desc->Completed);
                    pSub->TotalReadFrames++;
                } else {
                    AmbaPrint_PrintStr5("Good-Bye", NULL, NULL, NULL, NULL, NULL);
                    if(ERR_NA == SvcRtcp_SendGoodBye(&pSub->Rtp)) {
                        AmbaPrint_PrintStr5("[SvcAacSubsession]: Good-Bye sent failed", NULL, NULL, NULL, NULL, NULL);
                    }
                }
            }
        }
    }
}

/**
* create AAC Subsession.
* @param [in]  SVC_RTSP_SERVER_s object pointer.
* @param [in]  SVC_STREAM_LIVE_MEDIA_s or SVC_RTSP_MEDIA_TRACK_CFG_s object pointer.
*              AAC track info.
* @param [in]  whether AAC track is live.
* @return ErrorCode
*/
MEDIA_SUBSESSION_s* SvcAacSubsession_Create(void const * pSrv, void const * pInfo, UINT32 IsLive)
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
                pSub->PayloadCtx.Aac.SamplingFrequency = pTrk->Info.Audio.SampleRate;
                pSub->PayloadCtx.Aac.ChannelConfiguration = (UINT32)(pTrk->Info.Audio.Channels) + 1U;   //0:mono, 1:stereo

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

