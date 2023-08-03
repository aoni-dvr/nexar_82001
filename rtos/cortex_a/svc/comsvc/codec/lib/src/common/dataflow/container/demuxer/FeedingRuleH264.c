/**
 * @file FeedingRuleH264.c
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
#include "FeedingRuleH264.h"



static UINT32 GetNextFrameForward_SkipNone(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo,
        UINT32 StartFrameNo, UINT32 *NextFrameNo, UINT64 *NextTime, UINT32 *NextFrameType)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 SearchFrameNo = StartFrameNo;
    if (SearchFrameNo <  Track->FrameCount) {
        SVC_FRAME_INFO_s FrameInfo = {0};
        Rval = GetFrameInfo(IdxMgr, TrackId, SearchFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
        if (Rval == FORMAT_OK) {
            *NextFrameNo = SearchFrameNo;
            *NextTime = FrameInfo.DTS;
            *NextFrameType = FrameInfo.FrameType;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        *NextFrameNo = SearchFrameNo;
        *NextTime = SVC_FORMAT_MAX_TIMESTAMP;
        *NextFrameType = SVC_FIFO_TYPE_EOS;
    }
    return Rval;
}

static UINT32 GetNextFrameForward_SkipB(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 SearchFactor,
        UINT32 StartFrameNo, UINT32 *NextFrameNo, UINT64 *NextTime, UINT32 *NextFrameType)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 FindFlag = 0U;
    UINT32 SearchFrameNo;
    SVC_FRAME_INFO_s FrameInfo = {0};
    for (SearchFrameNo = StartFrameNo; SearchFrameNo < Track->FrameCount; SearchFrameNo += SearchFactor) {
        Rval = GetFrameInfo(IdxMgr, TrackId, SearchFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
        if (Rval == FORMAT_OK) {
            if ((FrameInfo.FrameType == SVC_FIFO_TYPE_IDR_FRAME) || (FrameInfo.FrameType == SVC_FIFO_TYPE_I_FRAME) ||
                (FrameInfo.FrameType == SVC_FIFO_TYPE_P_FRAME)) {
                FindFlag = 1U;
                *NextFrameNo = SearchFrameNo;
                *NextTime = FrameInfo.DTS;
                *NextFrameType = FrameInfo.FrameType;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if ((Rval != FORMAT_OK) || (FindFlag == 1U)) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        if (FindFlag != 1U) {
            *NextFrameNo = SearchFrameNo;
            *NextTime = SVC_FORMAT_MAX_TIMESTAMP;
            *NextFrameType = SVC_FIFO_TYPE_EOS;
        }
    }
    return Rval;
}

static UINT32 GetNextFrameForward_SkipPB(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 SearchFactor,
        UINT32 StartFrameNo, UINT32 *NextFrameNo, UINT64 *NextTime, UINT32 *NextFrameType)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 FindFlag = 0U;
    SVC_FRAME_INFO_s FrameInfo = {0};
    UINT32 SearchFrameNo;
    for (SearchFrameNo = StartFrameNo; SearchFrameNo < Track->FrameCount; SearchFrameNo += SearchFactor) {
        Rval = GetFrameInfo(IdxMgr, TrackId, SearchFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
        if (Rval == FORMAT_OK) {
            if ((FrameInfo.FrameType == SVC_FIFO_TYPE_IDR_FRAME) || (FrameInfo.FrameType == SVC_FIFO_TYPE_I_FRAME)) {
                FindFlag = 1U;
                *NextFrameNo = SearchFrameNo;
                *NextTime = FrameInfo.DTS;
                *NextFrameType = FrameInfo.FrameType;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if ((Rval != FORMAT_OK) || (FindFlag == 1U)) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        if (FindFlag != 1U) {
            *NextFrameNo = SearchFrameNo;
            *NextTime = SVC_FORMAT_MAX_TIMESTAMP;
            *NextFrameType = SVC_FIFO_TYPE_EOS;
        }
    }
    return Rval;
}

static UINT32 GetNextFrameForward(const SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Speed,
        UINT32 FrameNo, UINT32 TargetFrameNo, UINT32 *NextFrameNo, UINT32 *NextTargetFrameNo, UINT64 *NextTime, UINT32 *CurFrameType, UINT32 *NextFrameType)
{
    UINT32 Rval = FORMAT_OK;
    SVC_FRAME_INFO_s FrameInfo = {0};
    Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
        UINT8 SearchFactor = (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2U : 1U;
        *CurFrameType = FrameInfo.FrameType;
        *NextTargetFrameNo = TargetFrameNo + ((UINT32)Speed * (UINT32)SearchFactor);
        if (*NextTargetFrameNo > (Track->FrameCount - SearchFactor)) {
            *NextTargetFrameNo = Track->FrameCount - SearchFactor;
        }
        if (Speed <= Video->M) {
            if (Speed == 1U) {
                Rval = GetNextFrameForward_SkipNone(Track, IdxMgr, TrackId, GetFrameInfo, FrameNo + SearchFactor, NextFrameNo, NextTime, NextFrameType);
            } else {
                Rval = GetNextFrameForward_SkipB(Track, IdxMgr, TrackId, GetFrameInfo, SearchFactor, FrameNo + SearchFactor, NextFrameNo, NextTime, NextFrameType);
            }
        } else if ((Speed > Video->M) && (Speed <= Video->N)) {
            Rval = GetNextFrameForward_SkipPB(Track, IdxMgr, TrackId, GetFrameInfo, SearchFactor, FrameNo + SearchFactor, NextFrameNo, NextTime, NextFrameType);
        } else { /* (Speed > Video->N) */
            UINT32 N = (UINT32)Video->N * (UINT32)SearchFactor;
            UINT32 StartFrameNo;
            if ((FrameNo + N) > *NextTargetFrameNo) {
                StartFrameNo = Track->FrameCount; // There's no key frame left.
            } else {
                StartFrameNo = *NextTargetFrameNo - N + SearchFactor;
            }
            Rval = GetNextFrameForward_SkipPB(Track, IdxMgr, TrackId, GetFrameInfo, SearchFactor, StartFrameNo, NextFrameNo, NextTime, NextFrameType);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 GetNextFrameBackward_SkipPB(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 SearchFactor,
        UINT32 StartFrameNo, UINT32 *NextFrameNo, UINT64 *NextTime)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 FindFlag = 0U;
    SVC_FRAME_INFO_s FrameInfo = {0};
    UINT32 SearchFrameNo;
    for (SearchFrameNo = StartFrameNo; SearchFrameNo >= SearchFactor; SearchFrameNo -= SearchFactor) {
        Rval = GetFrameInfo(IdxMgr, TrackId, SearchFrameNo - SearchFactor, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
        if (Rval == FORMAT_OK) {
            if ((FrameInfo.FrameType == SVC_FIFO_TYPE_IDR_FRAME) || (FrameInfo.FrameType == SVC_FIFO_TYPE_I_FRAME)) {
                FindFlag = 1U;
                *NextFrameNo = SearchFrameNo - SearchFactor;
                *NextTime = FrameInfo.DTS + ((UINT64)FrameInfo.TimePerFrame * (UINT64)SearchFactor);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
        if ((Rval != FORMAT_OK) || (FindFlag == 1U)) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        if (FindFlag != 1U) {
            *NextFrameNo = 0;
            *NextTime = Track->InitDTS;
        }
    }
    return Rval;
}

static UINT32 GetNextFrameBackward(const SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Speed, UINT32 FrameNo, UINT32 TargetFrameNo, UINT32 *NextFrameNo, UINT32 *NextTargetFrameNo, UINT64 *NextTime)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    if (FrameNo == 0U) {
        *NextTargetFrameNo = 0;
        *NextFrameNo = 0;
        *NextTime = Track->InitDTS;
    } else {
        UINT8 SearchFactor = (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2U : 1U;
        *NextTargetFrameNo = (TargetFrameNo > ((UINT32)Speed * (UINT32)SearchFactor))? (TargetFrameNo - ((UINT32)Speed * (UINT32)SearchFactor)) : 0U;
        if (Speed <= Video->M) {
            Rval = GetNextFrameBackward_SkipPB(Track, IdxMgr, TrackId, GetFrameInfo, SearchFactor, FrameNo, NextFrameNo, NextTime);
        } else if ((Speed > Video->M) && (Speed <= Video->N)) {
            Rval = GetNextFrameBackward_SkipPB(Track, IdxMgr, TrackId, GetFrameInfo, SearchFactor, FrameNo, NextFrameNo, NextTime);
        } else { /* (Speed > Video->N) */
            UINT32 N = (UINT32)Video->N * (UINT32)SearchFactor;
            UINT32 StartFrameNo;
            if (FrameNo < (*NextTargetFrameNo + N)) {
                StartFrameNo =  0U; // There's no key frame left.
            } else {
                StartFrameNo = *NextTargetFrameNo + N;
            }
            Rval = GetNextFrameBackward_SkipPB(Track, IdxMgr, TrackId, GetFrameInfo, SearchFactor, StartFrameNo, NextFrameNo, NextTime);
        }
    }
    return Rval;
}

/**
 * Update the feed frame information
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] FeedNum The feed frame number
 * @param [in] FeedCount The feed frame count
 * @param [in] FeedTime The feed time
 * @param [in] TargetFrameNo The target frame number
 */
static void SvcFeedingRuleH264_UpdateFeedInfo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, INT32 FeedNum, INT32 FeedCount, INT32 FeedTime, UINT32 TargetFrameNo)
{
    FeedInfo->FrameNo += (UINT32) FeedNum;
    FeedInfo->FrameCount += (UINT32) FeedCount;
    FeedInfo->FeedTime += (UINT64) FeedTime;
    FeedInfo->TargetFrameNo = TargetFrameNo;
}

/**
 * Process of feeding frame (H264)
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Video Video track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFeedingRuleH264_Process(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    SVC_FEED_FRAME_PARAM_s FeedParam = {.VPS = NULL, .SPS = NULL, .PPS = NULL, .VPSLen = 0U, .SPSLen = 0U, .PPSLen = 0U};
    FeedParam.SPS = FeedInfo->Info.H264.SPS;
    FeedParam.PPS = FeedInfo->Info.H264.PPS;
    FeedParam.SPSLen = FeedInfo->Info.H264.SPSLen;
    FeedParam.PPSLen = FeedInfo->Info.H264.PPSLen;
    if (FeedInfo->Direction == SVC_FORMAT_DIR_FORWARD) {
        Rval = FeedInfo->FeedVideoFrame(Video, Stream, &FeedParam, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Direction, FeedInfo->FrameNo);
        if (Rval != FORMAT_OK) {
            if (Rval != FORMAT_ERR_FIFO_FULL) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Feed frame error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            UINT32 NextFrameNo, NextTargetFrameNo;
            UINT32 CurFrameType, NextFrameType = SVC_FIFO_TYPE_UNDEFINED;
            UINT64 NextTime;
            Rval = GetNextFrameForward(Video, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Speed,
                    FeedInfo->FrameNo, FeedInfo->TargetFrameNo, &NextFrameNo, &NextTargetFrameNo, &NextTime, &CurFrameType, &NextFrameType);
            if (Rval == FORMAT_OK) {
                INT64 NextTimeOffsetI64 = (INT64) NextTime - (INT64) FeedInfo->FeedTime;
                *NextFrameOffset = ((INT32) NextFrameNo - (INT32) FeedInfo->FrameNo);
                *NextTimeOffset = (INT32) NextTimeOffsetI64;
                if ((CurFrameType == SVC_FIFO_TYPE_IDR_FRAME) || (CurFrameType == SVC_FIFO_TYPE_I_FRAME)) { /* H264: when only feeding I/IDR frame, feed decode maker */
                    if ((NextFrameType == SVC_FIFO_TYPE_IDR_FRAME) || (NextFrameType == SVC_FIFO_TYPE_I_FRAME)) {
                        Rval = SvcFormat_PutDecodeMark(Track->Fifo);
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Put decode mark error!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                }
                if (Rval == FORMAT_OK) {
                    SvcFeedingRuleH264_UpdateFeedInfo(FeedInfo, *NextFrameOffset, *NextFrameOffset, *NextTimeOffset, NextTargetFrameNo);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFeedingRuleH264_GetNextFrame() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        Rval = FeedInfo->FeedVideoFrame(Video, Stream, &FeedParam, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Direction, FeedInfo->FrameNo);
        if (Rval != FORMAT_OK) {
            if (Rval != FORMAT_ERR_FIFO_FULL) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Feed frame error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            Rval = SvcFormat_PutDecodeMark(Track->Fifo);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutDecodeMark error!", __func__, NULL, NULL, NULL, NULL);
            }
            if (Rval == FORMAT_OK) {
                UINT32 NextFrameNo, NextTargetFrameNo;
                UINT64 NextTime;
                Rval = GetNextFrameBackward(Video, IdxMgr, TrackId, FeedInfo->GetFrameInfo, FeedInfo->Speed, FeedInfo->FrameNo, FeedInfo->TargetFrameNo, &NextFrameNo, &NextTargetFrameNo, &NextTime);
                if (Rval == FORMAT_OK) {
                    INT64 NextTimeOffsetI64 = (INT64) FeedInfo->FeedTime - (INT64) NextTime;
                    *NextTimeOffset = -((INT32) NextTimeOffsetI64);
                    *NextFrameOffset = -((INT32) FeedInfo->FrameNo - (INT32) NextFrameNo);
                    SvcFeedingRuleH264_UpdateFeedInfo(FeedInfo, *NextFrameOffset, -(*NextFrameOffset), *NextTimeOffset, NextTargetFrameNo);
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFeedingRuleH264_GetNextFrame() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    return Rval;
}

