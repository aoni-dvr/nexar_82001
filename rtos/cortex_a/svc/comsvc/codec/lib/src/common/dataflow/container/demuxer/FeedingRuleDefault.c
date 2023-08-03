/**
 * @file FeedingRuleDefault.c
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

/**
 * Update the feed frame information
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] FeedNum The feed frame number
 * @param [in] FeedCount The feed frame count
 * @param [in] FeedTime The feed time
 */
static void SvcFeedingRuleDefault_UpdateFeedInfo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, INT32 FeedNum, INT32 FeedCount, INT32 FeedTime)
{
    FeedInfo->FrameNo += (UINT32) FeedNum;
    FeedInfo->FrameCount += (UINT32) FeedCount;
    FeedInfo->FeedTime += (UINT64) FeedTime;
}

/**
 * Process audio track with default feeding rule
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Audio Audio track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFeedingRuleDefault_ProcessAudio(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
    if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
        Rval = FeedInfo->FeedAudioFrame(Audio, Stream, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Direction, FeedInfo->FrameNo);
        if (Rval != FORMAT_OK) {
            if (Rval != FORMAT_ERR_FIFO_FULL) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s: Feed frame error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            *NextFrameOffset = 1;
            *NextTimeOffset = (INT32) Track->TimePerFrame;
            if ((FeedInfo->FrameNo + (UINT32)*NextFrameOffset) >= Track->FrameCount) {
                INT64 NextTimeOffsetI64 = (INT64) SVC_FORMAT_MAX_TIMESTAMP - (INT64) FeedInfo->FeedTime;
                *NextTimeOffset = (INT32) NextTimeOffsetI64;
            }
        }
        if (Rval == FORMAT_OK) {
            SvcFeedingRuleDefault_UpdateFeedInfo(FeedInfo, *NextFrameOffset, *NextFrameOffset, *NextTimeOffset);
        }
    } else {
        Rval = FeedInfo->FeedAudioFrame(Audio, Stream, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Direction, FeedInfo->FrameNo);
        if (Rval != FORMAT_OK) {
            if (Rval != FORMAT_ERR_FIFO_FULL) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s: Feed frame error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            *NextFrameOffset = -1;
            *NextTimeOffset = -((INT32) Track->TimePerFrame);
        }
        if (Rval == FORMAT_OK) {
            SvcFeedingRuleDefault_UpdateFeedInfo(FeedInfo, *NextFrameOffset, -(*NextFrameOffset), *NextTimeOffset);
        }
    }
    return Rval;
}

/**
 * Process text track with default feeding rule
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Text Text track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFeedingRuleDefault_ProcessText(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
    if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
        Rval = FeedInfo->FeedTextFrame(Text, Stream, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Direction, FeedInfo->FrameNo);
        if (Rval != FORMAT_OK) {
            if (Rval != FORMAT_ERR_FIFO_FULL) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s: Feed frame error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            *NextFrameOffset = 1;
            *NextTimeOffset = (INT32) Track->TimePerFrame;
            if ((FeedInfo->FrameNo + (UINT32)*NextFrameOffset) >= Track->FrameCount) {
                INT64 NextTimeOffsetI64 = (INT64) SVC_FORMAT_MAX_TIMESTAMP - (INT64) FeedInfo->FeedTime;
                *NextTimeOffset = (INT32) NextTimeOffsetI64;
            }
        }
        if (Rval == FORMAT_OK) {
            SvcFeedingRuleDefault_UpdateFeedInfo(FeedInfo, *NextFrameOffset, *NextFrameOffset, *NextTimeOffset);
        }
    } else if (FeedInfo->Direction == SVC_FORMAT_DIR_BACKWARD) {
        Rval = FeedInfo->FeedTextFrame(Text, Stream, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Direction, FeedInfo->FrameNo);
        if (Rval != FORMAT_OK) {
            if (Rval != FORMAT_ERR_FIFO_FULL) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s: Feed frame error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            *NextFrameOffset = -1;
            *NextTimeOffset = -((INT32) Track->TimePerFrame);
        }
        if (Rval == FORMAT_OK) {
            SvcFeedingRuleDefault_UpdateFeedInfo(FeedInfo, *NextFrameOffset, -(*NextFrameOffset), *NextTimeOffset);
        }
    } else {
        /* Do nothing */
    }
    return Rval;
}

