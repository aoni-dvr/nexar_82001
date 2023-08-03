/**
 *  @file SvcRtsp.c
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
 *  @details netx/lwip based rtsp service
 *
 */

#include "SvcRtspServer.h"
#include "SvcRtsp.h"
#include "AmbaUtility.h"
#if defined(CONFIG_NETX_ENET)
#include "NetXStack.h"
#else
#endif
#include "AmbaCache.h"
#include "SvcStreamMgr.h"
#include "AmbaWrap.h"
#include "AmbaSvcWrap.h"
#include "AmbaAudio_AENC.h"
#include "AmbaAudio_AIN.h"
#include "AmbaAudio_AOUT.h"
#include "AmbaAudio_ADEC.h"
#include "AmbaAudioEfx.h"
#include "AmbaAudioBuf.h"
#include "AmbaAudio.h"
#include "AmbaPlayer.h"

static UINT32 RtspStreamingMode = NET_RTSP_STREAM_VIDEO_ENABLE;
static SVC_VIDEOENC_STREAM_LIST_s StreamingList = {0};

static UINT8 *RtspBuf;
static UINT32 RtspBufSize;

#define RTSPSERVICE_MAXSTREAMNUM   (1U)

static SVC_RTSP_STREAM_s   RtspServiceStreamInfo[RTSPSERVICE_MAXSTREAMNUM];

#define NETSTREAM_ON 1U
#define NETSTREAM_OFF 0U
#define NETSTREAM_ALLOWED_CLIENT_AMOUNT 2U

/* Map RecStreamId to resource name */
static const char *NetStreamNames[SVC_VIDEO_ENC_STREAM_ID_NUM] = {
    "liveMain",
    "live",
    0,
    0,
};

static SVC_VIDEOENC_STREAM_INFO_s* RtspGetStreamInfo(UINT32 streamId)
{
    UINT32 i = 0U;
    SVC_VIDEOENC_STREAM_INFO_s *PtrVStream = NULL;

    if (0U == StreamingList.StreamCount) {
        AmbaPrint_PrintUInt5("RtspGetStreamInfo No valid stream exist", 0U, 0U, 0U, 0U, 0U);
        PtrVStream = NULL;
    } else {
        for (i = 0U; i<StreamingList.StreamCount; i++) {
            if (StreamingList.StreamList[i].Id == streamId) {
                PtrVStream = &(StreamingList.StreamList[i]);
                break;
            }
        }
    }

    if (NULL == PtrVStream) {
        AmbaPrint_PrintUInt5("RtspGetStreamInfo No matched stream found", 0U, 0U, 0U, 0U, 0U);
    }

    return PtrVStream;
}

static UINT32 RtspGetValidStream(SVC_VIDEOENC_STREAM_LIST_s *pStreamList)
{
    UINT32 Count = 0U;
    SVC_VIDEOENC_STREAM_INFO_s *pStream = NULL;
    UINT32 EncodeType = VIDEO_ENC_STREAM_TYPE_H264;
    UINT32 RetVal = OK;

    if (NULL == pStreamList) {
        RetVal = ERR_NA;
    } else {
        pStream = &(pStreamList->StreamList[0]);
        pStream->Id = SVC_VIDEO_ENC_STREAM_ID_PRIMARY;
        pStream->VideoCodingFormat = EncodeType;

        {

            if (AmbaWrap_memset(pStream->StreamName, 0, sizeof(pStream->StreamName))!= 0U) { }

            // (void)AmbaAdvSnPrint(pStream->StreamName, sizeof(pStream->StreamName), "%s", "liveMain");
            AmbaUtility_StringAppend(pStream->StreamName, sizeof(pStream->StreamName), "liveMain");

            AmbaPrint_PrintStr5("%s, stream Name %s", __func__, pStream->StreamName, NULL, NULL, NULL);
        }

        /*  Primary stream will not be activated by default for reducing system loading.
            It should be activated if dual stream is disabled. */
        pStream->Active = 0U;
        Count++;
        pStream++;

        {
            pStream->Id = SVC_VIDEO_ENC_STREAM_ID_SECONDARY;
            pStream->VideoCodingFormat = EncodeType;

            {
                if (AmbaWrap_memset(pStream->StreamName, 0, sizeof(pStream->StreamName))!= 0U) { }

                // (void)AmbaAdvSnPrint(pStream->StreamName, sizeof(pStream->StreamName), "%s", "live");
                AmbaUtility_StringAppend(pStream->StreamName, sizeof(pStream->StreamName), "live");

                AmbaPrint_PrintStr5("%s, StreamName %s", __func__, pStream->StreamName, NULL, NULL, NULL);
            }

            pStream->Active = 1;
            Count++;
        }

        pStreamList->StreamCount = Count;
    }

    return RetVal;
}

static UINT32 RtspGetStreamIDList(SVC_RTSP_MEDIA_STREAM_ITEM_LIST_s *slist)
{
    UINT32 Index, SubIndex, TrackIdx;
    UINT32 RetVal = OK;
    UINT32 TrackNum;

    if (NULL == slist) {
        AmbaPrint_PrintUInt5("RtspGetStreamIDList invalid param ('slist' is NULL)", 0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        if (AmbaWrap_memset(&StreamingList, 0, sizeof(SVC_VIDEOENC_STREAM_LIST_s))!= 0U) { }
        RetVal = RtspGetValidStream(&StreamingList);

        if (OK == RetVal) {
            slist->Amount = RTSPSERVICE_MAXSTREAMNUM;

            AmbaPrint_PrintUInt5("RtspGetStreamIDList Amount: %d, %d", \
                                slist->Amount, StreamingList.StreamCount, 0U, 0U, 0U);

            for (Index = 0U; Index < slist->Amount; Index++) {
                slist->StreamItemList[Index].Name = NetStreamNames[Index];
                TrackNum = (RtspServiceStreamInfo[Index].NumTrack < SVC_RTSP_MAX_TRACK_PER_MEDIA) ? \
                    RtspServiceStreamInfo[Index].NumTrack : SVC_RTSP_MAX_TRACK_PER_MEDIA;

                /* if contains invalid stream, set as inactive */
                for (TrackIdx = 0U; TrackIdx < TrackNum; TrackIdx++) {
                    for (SubIndex = 0U; SubIndex < StreamingList.StreamCount; SubIndex++) {
                        if (RtspServiceStreamInfo[Index].TrackId[TrackIdx].EncoderStreamId == StreamingList.StreamList[SubIndex].Id) {
                            slist->StreamItemList[Index].Active = RtspServiceStreamInfo[Index].IsUsed;
                            break;
                        }
                    }

                    if (SubIndex == StreamingList.StreamCount ) {
                        slist->StreamItemList[Index].Active = 0;
                        break;
                    }
                }

                AmbaPrint_PrintUInt5("RtspGetStreamIDList StreamItemList[%d]: Active(%u)", \
                                Index, slist->StreamItemList[Index].Active, 0U, 0U, 0U);
                AmbaPrint_PrintStr5("Stream name %s", \
                                slist->StreamItemList[Index].Name, NULL, NULL, NULL, NULL);
            }
        }
    }

    return RetVal;
}

static VOID RtspSetupTrackInfoV(UINT32 StreamId, SVC_RTSP_MOVIE_INFO_CFG_s *MediaInfo)
{
    const AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pEncStr = NULL;
    UINT32 MediaId = AMBA_FORMAT_MID_AVC;
    UINT32 RetVal = OK;
    UINT32 i;
    ULONG  SrcUL;
    UINT8 *pUint8;

    for (i = 0U; i < RTSPSERVICE_MAXSTREAMNUM; i++) {
        if ((1U == RtspServiceStreamInfo[i].IsUsed) && (StreamId == RtspServiceStreamInfo[i].RecStreamId)) {
            pEncStr = RtspServiceStreamInfo[i].pVideoCfg;
            break;
        }
    }

    if (NULL == pEncStr) {
        RtspStreamingMode &= ~NET_RTSP_STREAM_VIDEO_ENABLE;
        AmbaPrint_PrintUInt5("Invalid video config", 0U, 0U, 0U, 0U, 0U);
    } else {
        switch ((UINT32)pEncStr->CodingFmt) {
            case AMBA_DSP_ENC_CODING_TYPE_H264:
                MediaId = AMBA_FORMAT_MID_AVC;
                break;
#if defined(AMBA_DSP_ENC_CODING_TYPE_H265)
            case AMBA_DSP_ENC_CODING_TYPE_H265:
                MediaId = AMBA_FORMAT_MID_H265;
                break;
#endif
            default:
                RetVal = ERR_NA;
                break;
        }

        if (OK == RetVal) {
            MediaInfo->Track[MediaInfo->nTrack].nMediaId = MediaId;
            MediaInfo->Track[MediaInfo->nTrack].nTimeScale = pEncStr->FrameRate.TimeScale;
            MediaInfo->Track[MediaInfo->nTrack].nTimePerFrame = pEncStr->FrameRate.NumUnitsInTick;
            MediaInfo->Track[MediaInfo->nTrack].Codec = 0x1U;

            SrcUL = pEncStr->EncConfig.BitsBufAddr;
            AmbaMisra_TypeCast(&pUint8, &SrcUL);
            MediaInfo->Track[MediaInfo->nTrack].pBufferBase = pUint8;

            SrcUL = pEncStr->EncConfig.BitsBufAddr + pEncStr->EncConfig.BitsBufSize;
            AmbaMisra_TypeCast(&pUint8, &SrcUL);
            MediaInfo->Track[MediaInfo->nTrack].pBufferLimit = pUint8;
            MediaInfo->Track[MediaInfo->nTrack].nTrackType = SVC_RTSP_MEDIA_TRACK_TYPE_VIDEO;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.Default = 1;

            if (pEncStr->FrameRate.Interlace == (UINT8)0) {
                MediaInfo->Track[MediaInfo->nTrack].Info.Video.nMode = (UINT8)SVC_VIDEO_MODE_P;
            } else {
                MediaInfo->Track[MediaInfo->nTrack].Info.Video.nMode = (UINT8)SVC_VIDEO_MODE_I_A_FLD_PER_SMP;
            }

            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nM = (UINT16)pEncStr->EncConfig.GOPCfg.M;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nN = (UINT16)pEncStr->EncConfig.GOPCfg.N;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nGOPSize = (UINT32)pEncStr->EncConfig.GOPCfg.N * pEncStr->EncConfig.GOPCfg.IdrInterval;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nWidth = (UINT16)pEncStr->Window.Width;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nHeight = (UINT16)pEncStr->Window.Height;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nCodecTimeScale = pEncStr->FrameRate.TimeScale;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nTrickRecNum = \
                (pEncStr->FrameRate.TimeScale / (((UINT32)pEncStr->FrameRate.Interlace + 1UL) * pEncStr->FrameRate.NumUnitsInTick)) / 30UL;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nTrickRecDen = 1U;
            MediaInfo->Track[MediaInfo->nTrack].Info.Video.nIRCycle = (UINT16)pEncStr->EncConfig.IntraRefreshCtrl.IntraRefreshCycle;
            if (MediaInfo->Track[MediaInfo->nTrack].Info.Video.nIRCycle > 0U) {
                MediaInfo->Track[MediaInfo->nTrack].Info.Video.nN                = 0xFFFF;
                MediaInfo->Track[MediaInfo->nTrack].Info.Video.nRecoveryFrameCnt = MediaInfo->Track[MediaInfo->nTrack].Info.Video.nIRCycle - 1U;
            }
        }else {
            // Do nothing
        }
    }
}

static VOID RtspSetupTrackInfoA(UINT32 StreamId, SVC_RTSP_MOVIE_INFO_CFG_s *MediaInfo)
{
    const SVC_RTSP_AUD_CONFIG_s *pAudConfig = NULL;
    UINT32 i;
    ULONG  SrcUL;
    UINT8 *pUint8;

    for (i = 0U; i < RTSPSERVICE_MAXSTREAMNUM; i++) {
        if ((1U==RtspServiceStreamInfo[i].IsUsed) && (StreamId==RtspServiceStreamInfo[i].RecStreamId)) {
            pAudConfig = &(RtspServiceStreamInfo[i].AudInfo);
            break;
        }
    }

    if ((pAudConfig != NULL) && (pAudConfig->pEncInfo != NULL)) {
        if (pAudConfig->Format == SVC_RTSP_AUD_AAC) {
            MediaInfo->Track[MediaInfo->nTrack].nMediaId = (UINT32)AMBA_FORMAT_MID_AAC;
        } else if (pAudConfig->Format == SVC_RTSP_AUD_PCM) {
            MediaInfo->Track[MediaInfo->nTrack].nMediaId = (UINT32)AMBA_FORMAT_MID_PCM;
        } else {
            AmbaPrint_PrintUInt5("Invalid audio coding format(%u)", pAudConfig->Format, 0U, 0U, 0U, 0U);
        }

        MediaInfo->Track[MediaInfo->nTrack].nTimeScale = pAudConfig->pEncInfo->SampleFreq;
        MediaInfo->Track[MediaInfo->nTrack].nTimePerFrame = pAudConfig->pEncInfo->FrameSize;
        MediaInfo->Track[MediaInfo->nTrack].Codec = 0x2U;
        MediaInfo->Track[MediaInfo->nTrack].pBufferBase = pAudConfig->pBsBufBase;

        pUint8 = pAudConfig->pBsBufBase;
        AmbaMisra_TypeCast(&SrcUL, &pUint8);
        SrcUL = SrcUL + pAudConfig->BsBufSize;
        AmbaMisra_TypeCast(&pUint8, &SrcUL);

        MediaInfo->Track[MediaInfo->nTrack].pBufferLimit = pUint8;
        MediaInfo->Track[MediaInfo->nTrack].nTrackType = (UINT8)SVC_RTSP_MEDIA_TRACK_TYPE_AUDIO;

        MediaInfo->Track[MediaInfo->nTrack].Info.Audio.SampleRate = pAudConfig->pEncInfo->SampleFreq;
        MediaInfo->Track[MediaInfo->nTrack].Info.Audio.Default = (UINT8)1;
        MediaInfo->Track[MediaInfo->nTrack].Info.Audio.Channels = (UINT8)pAudConfig->pEncInfo->ChannelNum;
        MediaInfo->Track[MediaInfo->nTrack].Info.Audio.BitsPerSample = pAudConfig->pEncInfo->SampleResolution;
    } else {
        RtspStreamingMode &= ~NET_RTSP_STREAM_AUDIO_ENABLE;
        AmbaPrint_PrintUInt5("Invalid audio config", 0U, 0U, 0U, 0U, 0U);
    }
}

static UINT32 RtspSetupMediaInfo(UINT32 NetStreamIndex, SVC_RTSP_MOVIE_INFO_CFG_s *MediaInfo)
{
    UINT32 Index;
    UINT32 StreamID = 0;
    UINT32 TrackId = 0;
    UINT32 RetVal = OK;
    const SVC_VIDEOENC_STREAM_INFO_s *PtrStreamInfo = NULL;
    UINT32 NetStreamMode = 0;

    if (NetStreamIndex >= RTSPSERVICE_MAXSTREAMNUM) {
        AmbaPrint_PrintUInt5("RtspSetupMediaInfo unexpected net stream idx: %d", \
                        NetStreamIndex, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        NetStreamMode = RtspStreamingMode;

        for (Index = 0U; Index < SVC_RTSP_MAX_TRACK_PER_MEDIA; Index++) {
            StreamID = RtspServiceStreamInfo[NetStreamIndex].TrackId[Index].EncoderStreamId;
            TrackId = RtspServiceStreamInfo[NetStreamIndex].TrackId[Index].TrackId;
            PtrStreamInfo = RtspGetStreamInfo(StreamID);

            if (NULL == PtrStreamInfo) {
                AmbaPrint_PrintStr5("%s, PtrStreamInfo is NULL", __func__, NULL, NULL, NULL, NULL);
                RetVal = ERR_NA;
            } else {
                if ((TrackId==SVC_MEDIA_TRACK_TYPE_VIDEO) && (0U!=(NetStreamMode & NET_RTSP_STREAM_VIDEO_ENABLE))) {
                    RtspSetupTrackInfoV(StreamID, MediaInfo);
                    MediaInfo->nTrack++;
                } else if ((TrackId==SVC_MEDIA_TRACK_TYPE_AUDIO) && (0U!=(NetStreamMode & NET_RTSP_STREAM_AUDIO_ENABLE))) {
                    RtspSetupTrackInfoA(StreamID, MediaInfo);
                    MediaInfo->nTrack++;
                } else {
                    //track isn't enabled, do nothing
                }
            }
        }
    }

    return RetVal;
}

static UINT32 RtspGetMediaInfo(UINT32 NetStreamIndex, SVC_RTSP_MOVIE_INFO_CFG_s *MediaInfo)
{
    UINT32 RetVal = OK;

    if (NULL == MediaInfo) {
        AmbaPrint_PrintUInt5("RtspGetMediaInfo invalid param. (media_info is NULL)", \
                        0U, 0U, 0U, 0U, 0U);
        RetVal = ERR_NA;
    } else {
        AmbaPrint_PrintUInt5("RtspGetMediaInfo request MediaInfo for Stream %d", \
                        NetStreamIndex, 0U, 0U, 0U, 0U);

        if (AmbaWrap_memset(MediaInfo, 0, sizeof(SVC_RTSP_MOVIE_INFO_CFG_s))!= 0U) { }
        StreamingList.ActiveStreamID = NetStreamIndex;
        RetVal = RtspSetupMediaInfo(NetStreamIndex, MediaInfo);
    }

    return RetVal;
}

static UINT32 RtspGetStreamListCB(void *Hdlr, UINT32 Event, const void* Info)
{
    UINT32 RetVal;
    SVC_RTSP_MEDIA_STREAM_ITEM_LIST_s *ptrStreamItemList;

    AmbaMisra_TouchUnused(Hdlr);
    AmbaMisra_TouchUnused(&Event);

    AmbaMisra_TypeCast(&ptrStreamItemList, &Info);
    RetVal = RtspGetStreamIDList(ptrStreamItemList);

    return RetVal;
}

static UINT32 RtspGetStreamInfoCB(void *Hdlr, UINT32 Event, const void* Info)
{
    UINT32 StreamId;
    UINT32 RetVal;
    SVC_RTSP_MOVIE_INFO_CFG_s *ptrMovieInfo;

    AmbaMisra_TouchUnused(Hdlr);

    StreamId = Event & 0x0000ffffU;

    AmbaMisra_TypeCast(&ptrMovieInfo, &Info);
    RetVal = RtspGetMediaInfo(StreamId, ptrMovieInfo);

    return RetVal;
}

/**
* Retrieve the required mem size
* @param [in]  the returned size
* @return OK/ERR_NA.
*/
UINT32 SvcRtsp_GetMemSize(UINT32 *pSize)
{
    UINT32 RetVal;
    SVC_RTSPSERVER_INIT_CFG_s RtspCfg;
    UINT32 RTSPRequiredMemSize;

    AmbaUtility_MemorySetU8((UINT8 *)&RtspCfg, 0, sizeof(SVC_RTSPSERVER_INIT_CFG_s));
    (VOID)SvcRtspServer_GetInitDefaultCfg(&RtspCfg);

    RtspCfg.NumMaxClient = NETSTREAM_ALLOWED_CLIENT_AMOUNT;
    RetVal = SvcRtspServer_GetRequiredMemorySize(&RtspCfg, &RTSPRequiredMemSize);
    *pSize = RTSPRequiredMemSize;

    return RetVal;
}

/**
* Set the buffer info for rtsp service
* @param [in]  buffer base
* @param [in]  buffer size
*/
VOID SvcRtsp_SetServiceBuf(ULONG RtspBufBase, UINT32 RtspBufSz)
{
    AmbaMisra_TypeCast(&RtspBuf, &RtspBufBase);
    RtspBufSize = RtspBufSz;
}

/**
* Init rtsp service
* @return OK/ERR_NA.
*/
UINT32 SvcRtsp_Init(VOID)
{
    UINT32                    RetVal = OK;
    SVC_RTSPSERVER_INIT_CFG_s RtspCfg;
    static UINT32             RtspServerInitFlag = 0U;

    AmbaUtility_MemorySetU8((UINT8 *)&RtspCfg, 0, sizeof(SVC_RTSPSERVER_INIT_CFG_s));

    if (0U == RtspServerInitFlag) {
        (VOID)SvcRtspServer_GetInitDefaultCfg(&RtspCfg);

        RtspCfg.NumMaxClient = NETSTREAM_ALLOWED_CLIENT_AMOUNT;
        RtspStreamingMode = NET_RTSP_STREAM_VIDEO_ENABLE;
        RtspStreamingMode |= NET_RTSP_STREAM_AUDIO_ENABLE;

        RtspCfg.cbMediaInfo = RtspGetStreamInfoCB;
        RtspCfg.cbStreamList = RtspGetStreamListCB;
        RtspCfg.MemoryPoolAddr = RtspBuf;
        RtspCfg.MemoryPoolSize = RtspBufSize;

        RetVal = SvcRtspServer_Init(&RtspCfg);
        if (RetVal != OK) {
            AmbaPrint_PrintUInt5("SvcRtsp_Init, RTSP server init fail. RetVal %d", RetVal, 0U, 0U, 0U, 0U);
        } else {
            RtspServerInitFlag = 1U;
        }
    }

    RetVal = SvcRtspServer_Start();
    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("SvcRtsp_Init, RTSP server start fail. RetVal %d", RetVal, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("RTSP server start done", 0U, 0U, 0U, 0U, 0U);
        AmbaPrint_PrintUInt5("SvcRtsp_Init, RtspStreamingMode %d.", RtspStreamingMode, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

/**
* DeInit rtsp service
* @return OK/ERR_NA.
*/
UINT32 SvcRtsp_DeInit(VOID)
{
    UINT32 RetVal = OK;

    RetVal = SvcRtspServer_Stop();
    if (RetVal != OK) {
        AmbaPrint_PrintUInt5("SvcRtsp_DeInit, RTSP server stop fail. RetVal %d", RetVal, 0U, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintUInt5("RTSP server stop done", 0U, 0U, 0U, 0U, 0U);
    }

    return RetVal;
}

/**
* Set buffer info for video/audio stream
* @param [in]  record stream id
* @param [in]  video info
* @param [in]  audio info
*/
VOID SvcRtsp_SetInfo(UINT32 RecStreamId,
                     AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s *pVideoCfg,
                     const SVC_RTSP_AUD_CONFIG_s *pAudConfig)
{
    static UINT32 InfoInited = 0U;
    UINT32 i;
    UINT32 Rval = ERR_NA;

    if (InfoInited == 0U) {
        for (i = 0U; i < RTSPSERVICE_MAXSTREAMNUM; i++) {
            RtspServiceStreamInfo[i].IsUsed = 0U;
        }

        InfoInited = 1U;
    }

    for (i = 0U; i < RTSPSERVICE_MAXSTREAMNUM; i++) {
        if (RtspServiceStreamInfo[i].IsUsed == 0U) {
            UINT32 TrackId = 0;

            RtspServiceStreamInfo[i].RecStreamId        = RecStreamId;
            if (pVideoCfg != NULL) {
                RtspServiceStreamInfo[i].pVideoCfg          = pVideoCfg;

                RtspServiceStreamInfo[i].TrackId[TrackId].EncoderStreamId = RecStreamId;
                RtspServiceStreamInfo[i].TrackId[TrackId].TrackId = SVC_MEDIA_TRACK_TYPE_VIDEO;
                TrackId += 1;
            }

            if (pAudConfig != NULL) {
                RtspServiceStreamInfo[i].AudInfo.Format     = pAudConfig->Format;
                RtspServiceStreamInfo[i].AudInfo.pEncInfo   = pAudConfig->pEncInfo;
                RtspServiceStreamInfo[i].AudInfo.pBsBufBase = pAudConfig->pBsBufBase;
                RtspServiceStreamInfo[i].AudInfo.BsBufSize  = pAudConfig->BsBufSize;

                RtspServiceStreamInfo[i].TrackId[TrackId].EncoderStreamId = RecStreamId;
                RtspServiceStreamInfo[i].TrackId[TrackId].TrackId = SVC_MEDIA_TRACK_TYPE_AUDIO;
                TrackId += 1;
            } else {
                RtspServiceStreamInfo[i].AudInfo.Format     = 0U;
                RtspServiceStreamInfo[i].AudInfo.pEncInfo   = NULL;
                RtspServiceStreamInfo[i].AudInfo.pBsBufBase = NULL;
                RtspServiceStreamInfo[i].AudInfo.BsBufSize  = 0U;

                RtspServiceStreamInfo[i].TrackId[1].EncoderStreamId = 0;
                RtspServiceStreamInfo[i].TrackId[1].TrackId = 0;
            }
            RtspServiceStreamInfo[i].IsUsed             = 1U;
            RtspServiceStreamInfo[i].NumTrack           = TrackId;

            Rval = OK;
            break;
        }
    }

    if (Rval == ERR_NA) {
        AmbaPrint_PrintUInt5("RTSP Stream is fulled", 0U, 0U, 0U, 0U, 0U);
    }
}

VOID SvcRtsp_Delete(UINT32 RecStreamId)
{
    UINT32 i;

    for (i = 0U; i < RTSPSERVICE_MAXSTREAMNUM; i++) {
        if ((1U==RtspServiceStreamInfo[i].IsUsed) && (RecStreamId==RtspServiceStreamInfo[i].RecStreamId)) {
            RtspServiceStreamInfo[i].IsUsed = 0U;
            break;
        }
    }
}

/**
* Notify new source info
* @param [in]  record stream id
* @param [in]  video/audio
* @param [in]  source descriptor
* @return OK/ERR_NA.
*/
UINT32 SvcRtsp_Notify(UINT32 RecStreamId, UINT32 Type, const void *pDesc)
{
    SVC_RTSP_FRAME_DESC_s Desc;
    const AMBA_DSP_ENC_PIC_RDY_s     *pVideo;
    const AMBA_AENC_AUDIO_DESC_s     *pAudio;
    UINT32 Rval = OK;
    UINT32 i, StreamId = 0U;
    ULONG  SrcUL;
    const UINT8 *ptrU8;

    for (i = 0U; i < RTSPSERVICE_MAXSTREAMNUM; i++) {
        if ((1U==RtspServiceStreamInfo[i].IsUsed) && (RecStreamId==RtspServiceStreamInfo[i].RecStreamId)) {
            StreamId = i;
            break;
        }
    }

    if (RTSPSERVICE_MAXSTREAMNUM == i) {
        Rval = ERR_NA;
        AmbaPrint_PrintUInt5("Unknown RecStreamId(%u)", RecStreamId, 0U, 0U, 0U, 0U);
    } else {
        if (Type == RTSP_SERVICE_NOTIFY_VID) {
            AmbaMisra_TypeCast(&pVideo, &pDesc);

            if (pVideo->PicSize == AMBA_DSP_ENC_END_MARK) {
                Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_EOS;
            } else {
                switch(pVideo->FrameType) {
                case PIC_FRAME_IDR:
                    Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_IDR_FRAME;
                    break;
                case PIC_FRAME_I:
                    Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_I_FRAME;
                    break;
                case PIC_FRAME_P:
                    Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_P_FRAME;
                    break;
                case PIC_FRAME_B:
                    Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_B_FRAME;
                    break;
                case PIC_FRAME_JPG:
                    Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_MJPEG_FRAME;
                    break;
                default:
                    AmbaPrint_PrintUInt5("Unknown frame type(%u)", pVideo->FrameType, 0U, 0U, 0U, 0U);
                    Rval = ERR_NA;
                    break;
                }
            }

            if (OK == Rval) {
                if (pVideo->SliceIdx == (pVideo->NumSlice - 1U)) {
                    Desc.Completed = 1U;
                } else {
                    Desc.Completed = 0U;
                }

                Desc.SeqNum    = (UINT32)pVideo->FrmNo;
                Desc.Pts       = pVideo->Pts;
                Desc.StartAddr = pVideo->StartAddr;
                Desc.Size      = pVideo->PicSize;
                Desc.Align     = 32U;

                (VOID)SvcRtspStrmNotify(RTSP_SERVICE_HANDLER_VID, &Desc);
            } else {
                //Do nothing
            }
        } else if (Type == RTSP_SERVICE_NOTIFY_AUD) {
            ULONG Addr;

            const SVC_RTSP_AUD_CONFIG_s   *pAudInfo = &(RtspServiceStreamInfo[StreamId].AudInfo);
            AmbaMisra_TypeCast(&pAudio, &pDesc);
            AmbaMisra_TypeCast(&(Addr), &(pAudio->pBufAddr));

            if (1U == pAudio->Eos) {
                Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_EOS;
            } else {
                Desc.Type = (UINT8)SVC_RTSP_FRAME_TYPE_AUDIO_FRAME;
            }
            Desc.Pts       = (pAudio->EncodedSamples);
            Desc.SeqNum    = ((UINT32)pAudio->EncodedSamples) / pAudInfo->pEncInfo->FrameSize;

            Desc.StartAddr = Addr;
            Desc.Size      = pAudio->DataSize;
            Desc.Align     = 32U;
            Desc.Completed = 1U;

            ptrU8 = pAudInfo->pBsBufBase;
            AmbaMisra_TypeCast(&SrcUL, &ptrU8);
            if (0U != AmbaCache_DataClean(SrcUL, pAudInfo->BsBufSize)) {
                // AmbaPrint_PrintStr5("Cache clean failed", NULL, NULL, NULL, NULL, NULL);
            }

            (VOID)SvcRtspStrmNotify(RTSP_SERVICE_HANDLER_AUD, &Desc);
        } else {
            AmbaPrint_PrintUInt5("Unknown data type(%u)", Type, 0U, 0U, 0U, 0U);
            Rval = ERR_NA;
        }
    }

    return Rval;
}

