/**
 * @file FrameFeeder.c
 *
 * Copyright (c) 2019 Ambarella International LP
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
 */
#include "DemuxerImpl.h"
#include "FrameFeeder.h"
#include "FeedingRuleH264.h"
#include "FeedingRuleH265.h"

/**
 * Init feeder information
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Track The track information
 * @param [in] TrackId The track Id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] FeedFrame Function pointer to feed frames
 * @param [in] TargetTime Target time
 * @param [in] Direction Feeding direction
 * @param [in] Speed Speed
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_InitFeedInfo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, const SVC_FORMAT_FEED_FRAME_FP_s *FeedFrame, UINT64 TargetTime, UINT8 Direction, UINT8 Speed)
{
    FeedInfo->MediaId = Track->MediaId;
    FeedInfo->TargetFrameNo = Track->FrameNo;
    FeedInfo->FrameNo = Track->FrameNo;
    FeedInfo->FrameCount = 0;
    FeedInfo->Speed = Speed;
    FeedInfo->Direction = Direction;
    FeedInfo->GetFrameInfo = GetFrameInfo;
    FeedInfo->FeedVideoFrame = FeedFrame->FeedVideoFrame;
    FeedInfo->FeedAudioFrame = FeedFrame->FeedAudioFrame;
    FeedInfo->FeedTextFrame = FeedFrame->FeedTextFrame;
    FeedInfo->FeedTime = TargetTime;
    // FIFO NULL means not to feed, directly set DTS to MAX or MIN
    if (Track->Fifo == NULL) {
        if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            Track->DTS = 0;
        }
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Track %u is disabled", TrackId, 0, 0, 0, 0);
    }
    return FORMAT_OK;
}

/**
 * Process video track
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Video Video track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @param [in] End Reaching end or not
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_ProcessVideo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset, UINT8 End)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((FeedInfo->MediaId == SVC_FORMAT_MID_AVC) || (FeedInfo->MediaId == SVC_FORMAT_MID_H264)) {
        Rval = SvcFeedingRuleH264_Process(FeedInfo, Stream, Video, IdxMgr, TrackId, NextFrameOffset, NextTimeOffset);
    } else if ((FeedInfo->MediaId == SVC_FORMAT_MID_HVC) || (FeedInfo->MediaId == SVC_FORMAT_MID_H265)) {
        Rval = SvcFeedingRuleH265_Process(FeedInfo, Stream, Video, IdxMgr, TrackId, NextFrameOffset, NextTimeOffset);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media id!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
        if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
            if (FeedInfo->FeedTime == SVC_FORMAT_MAX_TIMESTAMP) {
                if (End != 0U) {
                    Rval = SvcFormat_PutEndMark(Track->Fifo, Track->DTS, Track->TimeScale);
                }
            }
        } else {
            if (FeedInfo->FeedTime <= Track->InitDTS) {
                Rval = SvcFormat_PutEndMark(Track->Fifo, Track->DTS, Track->TimeScale);
            }
        }

    }
    return Rval;
}

/**
 * Process audio track
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Audio Audio track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @param [in] End Reaching end or not
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_ProcessAudio(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset, UINT8 End)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((FeedInfo->MediaId == SVC_FORMAT_MID_AAC) || (FeedInfo->MediaId == SVC_FORMAT_MID_ADPCM) || (FeedInfo->MediaId == SVC_FORMAT_MID_PCM) || (FeedInfo->MediaId == SVC_FORMAT_MID_LPCM)) {
        Rval = SvcFeedingRuleDefault_ProcessAudio(FeedInfo, Stream, Audio, IdxMgr, TrackId, NextFrameOffset, NextTimeOffset);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media id!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
        if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
            if (FeedInfo->FeedTime == SVC_FORMAT_MAX_TIMESTAMP) {
                if (End != 0U) {
                    Rval = SvcFormat_PutEndMark(Track->Fifo, Track->DTS, Track->TimeScale);
                }
            }
        } else {
            if (FeedInfo->FeedTime <= Track->InitDTS) {
                Rval = SvcFormat_PutEndMark(Track->Fifo, Track->DTS, Track->TimeScale);
            }
        }
    }
    return Rval;
}

/**
 * Process text track
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Text Text track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @param [in] End Reaching end or not
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_ProcessText(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset, UINT8 End)
{
    UINT32 Rval = FORMAT_ERR_GENERAL_ERROR;
    if ((FeedInfo->MediaId == SVC_FORMAT_MID_TEXT) || (FeedInfo->MediaId == SVC_FORMAT_MID_MP4S)) {
        Rval = SvcFeedingRuleDefault_ProcessText(FeedInfo, Stream, Text, IdxMgr, TrackId, NextFrameOffset, NextTimeOffset);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong media id!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
        if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
            if (FeedInfo->FeedTime == SVC_FORMAT_MAX_TIMESTAMP) {
                if (End != 0U) {
                    Rval = SvcFormat_PutEndMark(Track->Fifo, Track->DTS, Track->TimeScale);
                }
            }
        } else {
            if (FeedInfo->FeedTime <= Track->InitDTS) {
                Rval = SvcFormat_PutEndMark(Track->Fifo, Track->DTS, Track->TimeScale);
            }
        }
    }
    return Rval;
}

