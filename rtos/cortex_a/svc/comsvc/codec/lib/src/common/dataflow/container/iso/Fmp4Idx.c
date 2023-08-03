/**
 * @file Fmp4Idx.c
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
#include "../FormatAPI.h"
#include "Fmp4DmxImpl.h"
#include "../ByteOp.h"

#define PARSE_LENGTH    (10U)  /**< the max number of MOOF to be parsed each time */

/*
 * -------------------
 * | MOOF    |  Frame |
 * | Cache   |  Cache |
 * |         |        |
 * --------------------
 */
static UINT32 Fmp4Idx_ParseFrameInfo(FRAGMENT_IDX_s *Fmp4Idx, const SVC_MEDIA_INFO_s *Media);

static UINT32 FragmentCache_AllocateVideoBuffer(const SVC_MOVIE_INFO_s *Movie, FRAGMENT_CACHE_s *Caches, UINT8 TrackIdx, UINT32 FramePerFrag)
{
    UINT32 Rval = FORMAT_OK;
    AmbaMisra_TouchUnused(&Movie);
    if (FramePerFrag <= SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG) {
        UINT32 i;
        MOOF_INFO_CACHE_s *Moof;
        const UINT8 TrackId = TrackIdx;
        //const UINT32 Size = FramePerFrag * sizeof(SVC_FRAME_INFO_s);
        for (i = 0; i < Caches->FragmentCount; i++) {
            Moof = &Caches->Moof[i];
            Moof->FrameInfo[TrackId] = Moof->VideoFrameInfo[TrackIdx];
            //(void)AmbaWrap_memset(Moof->FrameInfo[TrackId], 0, Size);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s too many video frames", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 FragmentCache_AllocateAudioBuffer(const SVC_MOVIE_INFO_s *Movie, FRAGMENT_CACHE_s *Caches, UINT8 TrackIdx, UINT32 FramePerFrag)
{
    UINT32 Rval = FORMAT_OK;
    if (FramePerFrag <= SVC_ISO_MAX_AUDIO_FRAME_PER_FRAG) {
        UINT32 i;
        MOOF_INFO_CACHE_s *Moof;
        const UINT32 TrackId = (UINT32) Movie->VideoTrackCount + TrackIdx;
        //const UINT32 Size = FramePerFrag * sizeof(SVC_FRAME_INFO_s);
        for (i = 0; i < Caches->FragmentCount; i++) {
            Moof = &Caches->Moof[i];
            Moof->FrameInfo[TrackId] = Moof->AudioFrameInfo[TrackIdx];
            //(void)AmbaWrap_memset(Moof->FrameInfo[TrackId], 0, Size);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s too many audio frames", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 FragmentCache_AllocateTextBuffer(const SVC_MOVIE_INFO_s *Movie, FRAGMENT_CACHE_s *Caches, UINT8 TrackIdx, UINT32 FramePerFrag)
{
    UINT32 Rval = FORMAT_OK;
    if (FramePerFrag <= SVC_ISO_MAX_TEXT_FRAME_PER_FRAG) {
        UINT32 i;
        MOOF_INFO_CACHE_s *Moof;
        const UINT8 TrackId = Movie->VideoTrackCount + Movie->AudioTrackCount + TrackIdx;
        //const UINT32 Size = FramePerFrag * sizeof(SVC_FRAME_INFO_s);
        for (i = 0; i < Caches->FragmentCount; i++) {
            Moof = &Caches->Moof[i];
            Moof->FrameInfo[TrackId] = Moof->TextFrameInfo[TrackIdx];
            //(void)AmbaWrap_memset(Moof->FrameInfo[TrackId], 0, Size);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s too many text frames", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 GetFramePerFrag(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, const SVC_VIDEO_TRACK_INFO_s *DefVideo, UINT64 FragmentTime)
{
    UINT64 FramePerFrag;
    if (DefVideo->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
        FramePerFrag = (FragmentTime + ((UINT64) Track->TimePerFrame << 1ULL) - 1ULL) / Track->TimePerFrame;
    } else {
        FramePerFrag = (FragmentTime + Track->TimePerFrame - 1ULL) / Track->TimePerFrame;
    }
    if (DefVideo->CaptureTimeMode == 1U) {
        FramePerFrag *= 2ULL; /* Enlarge for drifted/variable frame rate */
    }
    return (UINT32)FramePerFrag;
}

static UINT32 FragmentCache_Create(FRAGMENT_CACHE_s *Caches, SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 TrackCount = Movie->VideoTrackCount, Tmp;
    const SVC_VIDEO_TRACK_INFO_s * const DefVideo = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, TrackCount, &Tmp);
    if (DefVideo != NULL) {
        /* assume there is no VFR */
        UINT32 i;
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
        UINT64 FragmentTime;
        UINT32 FramePerFrag;
        UINT32 GOPSize = DefVideo->GOPSize;
        UINT8 Type = Movie->MediaInfo.SubFormat >> 6U;
        if (DefVideo->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
            GOPSize = GOPSize << 1;
        }
        FragmentTime = (UINT64)GOPSize * DefVideo->Info.TimePerFrame;
        for (i = 0; i < TrackCount; i++) {
            Track = &Movie->VideoTrack[i].Info;
            if (Track == &DefVideo->Info) {
                FramePerFrag = GOPSize;
            } else {
                FramePerFrag = GetFramePerFrag(Track, DefVideo, FragmentTime);
            }
            Caches->Track[i].MaxFrameCount = FramePerFrag;
            Rval = FragmentCache_AllocateVideoBuffer(Movie, Caches, (UINT8) i, FramePerFrag);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            /* trick, to notify no cache is loaded */
            Caches->TrackCount = TrackCount;
            TrackCount = Movie->AudioTrackCount;
            for (i = 0; i < TrackCount; i++) {
                Track = &Movie->AudioTrack[i].Info;
                if (Type == SVC_AMBA_BOX_TYPE_MW) {
                    FramePerFrag = GetFramePerFrag(Track, DefVideo, FragmentTime);
                } else {
                    FramePerFrag = SVC_ISO_MAX_AUDIO_FRAME_PER_FRAG;
                }
                Caches->Track[i + Movie->VideoTrackCount].MaxFrameCount = FramePerFrag;
                Rval = FragmentCache_AllocateAudioBuffer(Movie, Caches, (UINT8) i, FramePerFrag);
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
            if (Rval == FORMAT_OK) {
                /* trick, to notify no cache is loaded */
                Caches->TrackCount += TrackCount;
                TrackCount = Movie->TextTrackCount;
                for (i = 0; i < TrackCount; i++) {
                    Track = &Movie->TextTrack[i].Info;
                    if (Type == SVC_AMBA_BOX_TYPE_MW) {
                        FramePerFrag = GetFramePerFrag(Track, DefVideo, FragmentTime);
                    } else {
                        FramePerFrag = SVC_ISO_MAX_TEXT_FRAME_PER_FRAG;
                    }
                    Caches->Track[i + Movie->VideoTrackCount + Movie->AudioTrackCount].MaxFrameCount = FramePerFrag;
                    Rval = FragmentCache_AllocateTextBuffer(Movie, Caches, (UINT8) i, FramePerFrag);
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
                if (Rval == FORMAT_OK) {
                    /* trick, to notify no cache is loaded */
                    Caches->TrackCount += TrackCount;
                }
            }
        }
    } else {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Idx_AllocateBuffer(FRAGMENT_IDX_s *Fmp4Idx)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_INFO_s * const Media = Fmp4Idx->Media;
    if (Media->Valid != 0U) {
        const SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Fmp4Idx->Caches.FragmentCount = Movie->IsoInfo.FragmentCount;
        if (Fmp4Idx->Caches.FragmentCount <= SVC_ISO_MAX_FRAGMENTS) {
            //LOG_INFO("MaxFragment =  %u", Fmp4Idx->Caches.FragmentCount);
            //(void)AmbaWrap_memset(Fmp4Idx->Caches.Moof, 0, sizeof(Fmp4Idx->Caches.Moof));
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s too many fragments", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s invalid media", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 CheckGetFrameInfoParam(const FRAGMENT_CACHE_s *Caches, const MOOF_INFO_CACHE_s *Fragment, UINT8 TrackId, UINT32 FrameIdx)
{
    UINT32 Rval = FORMAT_OK;
    if (Caches->SequenceIdx > Caches->FragmentCount) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckGetFrameInfoParam incorrect SequenceIdx=%u, %u", Caches->SequenceIdx,Caches->FragmentCount, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    if (Fragment != &Caches->Moof[Caches->SequenceIdx]) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect Fragment", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    if (FrameIdx >= Caches->Moof[Caches->SequenceIdx].FrameCount[TrackId]) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "CheckGetFrameInfoParam incorrect FrameIdx %u %u", FrameIdx, Caches->Moof[Caches->SequenceIdx].FrameCount[TrackId], 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static SVC_FRAME_INFO_s *FragmentCache_GetFrameInfo(const FRAGMENT_CACHE_s *Caches, const MOOF_INFO_CACHE_s *Fragment, UINT8 TrackId, UINT32 FrameIdx)
{
    SVC_FRAME_INFO_s *Rval = NULL;
    if (CheckGetFrameInfoParam(Caches, Fragment, TrackId, FrameIdx) == FORMAT_OK) {
        const UINT32 SequenceIdx = Caches->SequenceIdx;
        Rval = &Caches->Moof[SequenceIdx].FrameInfo[TrackId][FrameIdx];
    }
    return Rval;
}

static UINT32 Fmp4Idx_GetFrameInfo_Next(FRAGMENT_IDX_s *Fmp4Idx, UINT8 TrackId, UINT32 FrameNo, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 FragmentIdx = Fmp4Idx->Caches.SequenceIdx + 1U;
    const MOOF_INFO_CACHE_s * const Moof = Fmp4Idx->Caches.Moof;
    while ((Rval == FORMAT_OK) && (FragmentIdx < Fmp4Idx->Caches.FragmentCount)) {
        const MOOF_INFO_CACHE_s *Fragment = &Moof[FragmentIdx];
        if (Fragment->Position == 0U) {
            Rval = Fmp4Idx_ParseFrameInfo(Fmp4Idx, Fmp4Idx->Media);
        }
        if (Rval == FORMAT_OK) {
            if (FrameNo < Fragment->FrameNo[TrackId]) {
                // PrevFragment = Fragment - 1;
                const MOOF_INFO_CACHE_s * const PrevFragment = &Moof[FragmentIdx - 1U];
                if (FrameNo < PrevFragment->FrameNo[TrackId]) {
                    AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_GetFrameInfo incorrect FrameNo %u %u", FrameNo, PrevFragment->FrameNo[TrackId], 0U, 0U, 0U);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                } else {
                    const SVC_FRAME_INFO_s *Frame;
                    UINT32 FrameIdx = FrameNo - PrevFragment->FrameNo[TrackId];
                    Fmp4Idx->Caches.SequenceIdx = Fragment->SequenceNum;//FragmentIdx
                    Frame = FragmentCache_GetFrameInfo(&Fmp4Idx->Caches, Fragment, TrackId, FrameIdx);
                    if (Frame != NULL) {
                        FrameInfo->FrameType = Frame->FrameType;
                        FrameInfo->FrameSize = Frame->FrameSize;
                        FrameInfo->FramePos = Frame->FramePos;
                        FrameInfo->PTS = Frame->PTS;
                        FrameInfo->DTS = Frame->DTS;
                        FrameInfo->TimePerFrame = Frame->TimePerFrame;
                        //AmbaPrint_ModulePrintUInt5("Cache : %u", Fmp4Idx->Caches.SequenceIdx, 0U, 0U, 0U, 0U);
                    } else {
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    }
                }
                break;
            }
            FragmentIdx++;
        }
    }
    if (Rval == FORMAT_OK) {
        if (FragmentIdx == Fmp4Idx->Caches.FragmentCount) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s no corresponding point", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 Fmp4Idx_GetFrameInfo_Prev(FRAGMENT_IDX_s *Fmp4Idx, UINT8 TrackId, UINT32 FrameNo, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 FragmentIdx = Fmp4Idx->Caches.SequenceIdx;
    const MOOF_INFO_CACHE_s * const Moof = Fmp4Idx->Caches.Moof;
    do {
        if ((FragmentIdx == 0U) || (FrameNo >= Moof[FragmentIdx - 1U].FrameNo[TrackId])) {
            const MOOF_INFO_CACHE_s *Fragment = &Moof[FragmentIdx];
            if (FrameNo >= Fragment->FrameNo[TrackId]) {
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_GetFrameInfo incorrect FrameNo %u %u", FrameNo, Fragment->FrameNo[TrackId], 0U, 0U, 0U);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                const SVC_FRAME_INFO_s *Frame;
                UINT32 FrameIdx;
                if (FragmentIdx == 0U) {
                    FrameIdx = FrameNo;
                } else {
                    FrameIdx = FrameNo - Moof[FragmentIdx - 1U].FrameNo[TrackId];
                }
                Fmp4Idx->Caches.SequenceIdx = Fragment->SequenceNum; //FragmentIdx
                Frame = FragmentCache_GetFrameInfo(&Fmp4Idx->Caches, Fragment, TrackId, FrameIdx);
                if (Frame == NULL) {
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                } else {
                    FrameInfo->FrameType = Frame->FrameType;
                    FrameInfo->FrameSize = Frame->FrameSize;
                    FrameInfo->FramePos = Frame->FramePos;
                    FrameInfo->PTS = Frame->PTS;
                    FrameInfo->DTS = Frame->DTS;
                    FrameInfo->TimePerFrame = Frame->TimePerFrame;
                    //AmbaPrint_ModulePrintUInt5("Cache : %u", Fmp4Idx->Caches.SequenceIdx, 0U, 0U, 0U, 0U);
                    //AmbaPrint_ModulePrintUInt5("[%u] : %u %u %u %u", FrameIdx, FrameInfo->FrameType, FrameInfo->FrameSize, (UINT32)FrameInfo->FramePos, (UINT32)FrameInfo->PTS);
                    break;
                }
            }
        } else {
            FragmentIdx--;
        }
    } while (Rval == FORMAT_OK);
    return Rval;
}

/**
 * Get information of the target frame
 *
 * @param [in] Fmp4Idx Index manager
 * @param [in] TrackId Track id of media information
 * @param [in] FrameNo Target frame number of index data to get
 * @param [in] Direction Get data direction
 * @param [out] FrameInfo Buffer to write the return frame information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Idx_GetFrameInfo(FRAGMENT_IDX_s *Fmp4Idx, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MEDIA_INFO_s * const Media = Fmp4Idx->Media;;
    AmbaMisra_TouchUnused(&Direction);
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        const MOOF_INFO_CACHE_s *Fragment = &Fmp4Idx->Caches.Moof[Fmp4Idx->Caches.SequenceIdx];
        if (Fragment->Position == 0U) {
            Rval = Fmp4Idx_ParseFrameInfo(Fmp4Idx, Media);
        }
        if (Rval == FORMAT_OK) {
            if (Fragment->FrameNo[TrackId] <= FrameNo) {
                Rval = Fmp4Idx_GetFrameInfo_Next(Fmp4Idx, TrackId, FrameNo, FrameInfo);
            } else {
                Rval = Fmp4Idx_GetFrameInfo_Prev(Fmp4Idx, TrackId, FrameNo, FrameInfo);
            }
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_GetFrameInfo incorrect media type %u", Media->MediaType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Idx_ParseTfhd(SVC_STREAM_HDLR_s *Stream, INT64 ParseEndPos, UINT64 *BaseDataOffset)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        UINT32 Tmp32;
        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe64(Stream, BaseDataOffset);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, ParseEndPos, SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
             AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 Fmp4Idx_ParseTfdt(SVC_STREAM_HDLR_s *Stream, UINT32 *BaseMediaDecodeTime)
{
    UINT8 Tmp8;
    UINT32 Rval = SvcFormat_GetByte(Stream, &Tmp8);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &Tmp8);
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, BaseMediaDecodeTime);
    }
    return Rval;
}

static UINT32 Fmp4Idx_ParseTrun(SVC_STREAM_HDLR_s *Stream, FMP4_IDX_TRACK_INFO_s *Track, UINT64 BaseDataOffset, SVC_FRAME_INFO_s *FrameBuf, UINT32 *FrameCount)
{
    UINT32 SampleCount = 0U, DataOffset = 0U, Tmp32;
    UINT32 Flag = 0U;
    UINT32 i;
    UINT32 Rval = SvcFormat_GetBe32(Stream, &Flag);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &SampleCount);
        if (SampleCount > Track->MaxFrameCount) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_ParseTrun incorrect FrameCount %u/%u", SampleCount,  Track->MaxFrameCount, 0U, 0U, 0U);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe32(Stream, &DataOffset);
    }
    if (Rval == FORMAT_OK) {
        // first sample Flag
        if ((Flag & 0x4U) != 0U) {
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
        }
    }
    if (Rval == FORMAT_OK) {
        const ISO_IDX_VIDEO_INFO_s * Video = &Track->IdxInfo.Video;
        UINT64 AccumulatOffset = 0;
        UINT32 TimePerFrame = Track->IdxInfo.TimePerFrame;
        UINT32 CTTS = 0;
        for (i = 0; i < SampleCount; i++) {
            SVC_FRAME_INFO_s *FrameInfo = &FrameBuf[i];
            UINT32 SampleDuration = 0U;
            if ((Flag & 0x100U) != 0U) {
                Rval = SvcFormat_GetBe32(Stream, &SampleDuration);
                SampleDuration = (UINT32)NORMALIZE_TO_TIMESCALE(Track->IdxInfo.OrigTimeScale, (UINT64)SampleDuration, Track->IdxInfo.TimeScale);
            } else {
                SampleDuration = TimePerFrame;  // use TimePerFrame as default
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe32(Stream, &FrameInfo->FrameSize);
            }
            if (Rval == FORMAT_OK) {
                // sample flags preset
                if ((Flag & 0x400U) != 0U) {
                    Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                }
            }
            if (Rval == FORMAT_OK) {
                // CTTS flags preset
                if ((Flag & 0x800U) != 0U) {
                    Rval = SvcFormat_GetBe32(Stream, &CTTS);
                    CTTS = (UINT32)NORMALIZE_TO_TIMESCALE(Track->IdxInfo.OrigTimeScale, (UINT64)CTTS, Track->IdxInfo.TimeScale);
                } else {
                    CTTS = 0;
                }
            }
            if (Rval != FORMAT_OK) {
                break;
            }
            FrameInfo->FramePos = BaseDataOffset + (UINT64)DataOffset + AccumulatOffset;
            AccumulatOffset += FrameInfo->FrameSize;
            FrameInfo->DTS = Track->DTS;
            if ((Flag & 0x800U) != 0U) {
                /* PTS = DTS + CTTS - ref. IDR/I/P's TPF, while the TPF is the same in a GOP. */
                FrameInfo->PTS = FrameInfo->DTS + CTTS - SampleDuration;
            } else {
                FrameInfo->PTS = FrameInfo->DTS;
            }
            FrameInfo->TimePerFrame = SampleDuration;
            // assign frame type
            if (Track->IdxInfo.TrackType == SVC_MEDIA_TRACK_TYPE_VIDEO) {
                UINT8 ClosedGOP = ((SampleCount % Video->M) == 0U) ? 0U : 1U;
                FrameInfo->FrameType = SvcFormat_GetVideoFrameType(Video->Mode, Video->M, Video->N, Video->GOPSize, ClosedGOP, i);
            } else if (Track->IdxInfo.TrackType == SVC_MEDIA_TRACK_TYPE_AUDIO) {
                FrameInfo->FrameType = SVC_FIFO_TYPE_AUDIO_FRAME;
            } else {
                FrameInfo->FrameType = SVC_FIFO_TYPE_UNDEFINED;
            }
            //AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "The %u-th Frame Type=%u DTS=%u PTS=%u, TPF=%u", i, FrameInfo->FrameType, (UINT32)FrameInfo->DTS, (UINT32)FrameInfo->PTS, FrameInfo->TimePerFrame);
            // update DTS
            Track->DTS += FrameInfo->TimePerFrame;
        }
        *FrameCount = SampleCount;
    }
    return Rval;
}

static UINT32 Fmp4Idx_ParseTraf(SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, FMP4_IDX_TRACK_INFO_s *Track, SVC_FRAME_INFO_s *FrameBuf, UINT32 *FrameCount)
{
    UINT32 Rval;
    UINT32 BoxSize, BoxTag;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT64 BaseDataOffset = 0;
    UINT32 BaseMediaDecodeTime = 0U;
    do {
        /* Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_TFHD) {
                    Rval = Fmp4Idx_ParseTfhd(Stream, ((INT64) ParsePos + (INT64) BoxSize), &BaseDataOffset);
                } else if (BoxTag == TAG_TFDT) {
                    Rval = Fmp4Idx_ParseTfdt(Stream, &BaseMediaDecodeTime);
                    if (Rval == FORMAT_OK) {
                        Track->DTS = NORMALIZE_TO_TIMESCALE(Track->IdxInfo.OrigTimeScale, (UINT64)BaseMediaDecodeTime, Track->IdxInfo.TimeScale);
                    }
                } else if (BoxTag == TAG_TRUN) {
                    Rval = Fmp4Idx_ParseTrun(Stream, Track, BaseDataOffset, FrameBuf, FrameCount);
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                         AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    //AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "TRAF : decode_time=%u, data_offset=%u", BaseMediaDecodeTime, (UINT32)BaseDataOffset, 0, 0, 0);
    return Rval;
}

static UINT32 Fmp4Idx_ParseMoof(SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, const UINT32 *FrameNo, MOOF_INFO_CACHE_s *Moof, FMP4_IDX_TRACK_INFO_s *Track)
{
    UINT32 Rval;
    UINT32 BoxSize, BoxTag;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT8 TrackCount = 0;
    do {
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            } else {
                if (BoxTag == TAG_TRAF) {
                    Rval = Fmp4Idx_ParseTraf(Stream, ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, &Track[TrackCount], Moof->FrameInfo[TrackCount], &Moof->FrameCount[TrackCount]);
                    TrackCount++;
                } else {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TAG_MFHD!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (RemainSize == 0U) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    if (Rval == FORMAT_OK) {
        UINT32 i;
        for (i = 0; i < TrackCount; i++) {
            Moof->FrameNo[i] = FrameNo[i] + Moof->FrameCount[i];
            //AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_ParseMoof(%u) : track[%u] FrameCount = %u, FrameNo = %u", Moof->SequenceNum, i, Moof->FrameCount[i], Moof->FrameNo[i], 0);
        }
    }
    return Rval;
}

static UINT32 Fmp4Idx_ParseFrameInfo(FRAGMENT_IDX_s *Fmp4Idx, const SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval = FORMAT_OK;
    SVC_STREAM_HDLR_s *Stream = Fmp4Idx->Stream;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_ParseFrameInfo start %u", (UINT32)Fmp4Idx->Caches.ParsedPos, 0, 0, 0, 0);
        Rval = S2F(Stream->Func->Seek(Stream, (INT64) Fmp4Idx->Caches.ParsedPos, SVC_STREAM_SEEK_START));
        if (Rval == FORMAT_OK) {
            FRAGMENT_CACHE_s *Caches = &Fmp4Idx->Caches;
            UINT64 ParseEndPos = Media->Size;
            UINT64 ParsePos = Caches->ParsedPos;
            UINT64 RemainSize = ParseEndPos - ParsePos;
            UINT32 BoxTag, BoxSize;
            UINT32 Limit = Caches->FragmentCount;
            UINT32 Count = Caches->ParsedCount;
            if ((Count + PARSE_LENGTH) < Limit) {
                Limit = Count + PARSE_LENGTH;
            }
            while ((Rval == FORMAT_OK) && (Count < Limit)) {
                /** Get Box Size, Tag */
                Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                if (Rval == FORMAT_OK) {
                    if (RemainSize < BoxSize) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Container is corrupted!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    } else {
                        if (BoxTag == TAG_MOOF) {
                            if (Count >= Caches->FragmentCount) {
                                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "incorrect Count %u %u", Count, Fmp4Idx->Caches.FragmentCount, 0U, 0U, 0U);
                                Rval = FORMAT_ERR_INVALID_FORMAT;
                            } else {
                                UINT32 FrameNo[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0U};
                                MOOF_INFO_CACHE_s *Moof = &Caches->Moof[Count];
                                if (Count > 0U) {
                                    UINT8 i, TrackCount;
                                    SVC_MOVIE_INFO_s *Movie;
                                    AmbaMisra_TypeCast(&Movie, &Media);
                                    TrackCount = Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount;
                                    for (i = 0; i < TrackCount; i++) {
                                        FrameNo[i] = Caches->Moof[Count - 1U].FrameNo[i];
                                    }
                                }
                                Moof->Position = ParsePos;
                                Moof->SequenceNum = Count;
                                Rval = Fmp4Idx_ParseMoof(Stream, ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE, ParsePos + BoxSize, FrameNo, Moof, Caches->Track);
                                if (Rval == FORMAT_OK) {
                                    Count++;
                                }
                            }
                        }  else {
                            Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            ParsePos += BoxSize;
                            RemainSize -= BoxSize;
                            if (RemainSize == 0U) {
                                break;
                            }
                        }
                    }
                }
            }
            Caches->ParsedPos = ParsePos;
            Caches->ParsedCount = Count;
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_ParseFrameInfo : Pos=%u, Count=%u/%u", (UINT32) Fmp4Idx->Caches.ParsedPos, Fmp4Idx->Caches.ParsedCount, Fmp4Idx->Caches.FragmentCount, 0U, 0U);
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_ParseFrameInfo end", NULL, NULL, NULL, NULL, NULL);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_ParseFrameInfo incorrect Media type %u", Media->MediaType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 Fmp4Idx_InitIsoIdxInfo(FRAGMENT_IDX_s *Fmp4Idx, const SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i = 0;
    UINT8 Idx = 0;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
    FMP4_IDX_TRACK_INFO_s *IdxTrack;
    ISO_IDX_TRACK_INFO_s *IdxInfo;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        const SVC_VIDEO_TRACK_INFO_s *Video = &Movie->VideoTrack[i];
        Track = &Video->Info;
        IdxTrack = &Fmp4Idx->Caches.Track[Idx];
        IdxTrack->DTS = Track->InitDTS;
        IdxInfo = &IdxTrack->IdxInfo;
        IdxInfo->TrackId = Idx;
        IdxInfo->TrackType = SVC_MEDIA_TRACK_TYPE_VIDEO;
        IdxInfo->TimeScale = Track->TimeScale;
        IdxInfo->OrigTimeScale = Track->OrigTimeScale;
        IdxInfo->TimePerFrame = Track->TimePerFrame;
        IdxInfo->Video.GOPSize = Video->GOPSize;
        IdxInfo->Video.M = Video->M;
        IdxInfo->Video.N = Video->N;
        IdxInfo->Video.Mode = Video->Mode;
        Idx++;
    }
    for (i = 0 ; i < Movie->AudioTrackCount; i++) {
        Track = &Movie->AudioTrack[i].Info;
        IdxTrack = &Fmp4Idx->Caches.Track[Idx];
        IdxTrack->DTS = Track->InitDTS;
        IdxInfo = &IdxTrack->IdxInfo;
        IdxInfo->TrackId = Idx;
        IdxInfo->TrackType = SVC_MEDIA_TRACK_TYPE_AUDIO;
        IdxInfo->TimeScale = Track->TimeScale;
        IdxInfo->OrigTimeScale = Track->OrigTimeScale;
        IdxInfo->TimePerFrame = Track->TimePerFrame;
        Idx++;
    }
    for (i = 0 ; i < Movie->TextTrackCount; i++) {
        Track = &Movie->TextTrack[i].Info;
        IdxTrack = &Fmp4Idx->Caches.Track[Idx];
        IdxTrack->DTS = Track->InitDTS;
        IdxInfo = &IdxTrack->IdxInfo;
        IdxInfo->TrackId = Idx;
        IdxInfo->TrackType = SVC_MEDIA_TRACK_TYPE_TEXT;
        IdxInfo->TimeScale = Track->TimeScale;
        IdxInfo->OrigTimeScale = Track->OrigTimeScale;
        IdxInfo->TimePerFrame = Track->TimePerFrame;
        Idx++;
    }
    // set cached parse position to the 1st moof
    Fmp4Idx->Caches.ParsedPos = Movie->IsoInfo.FirstFragmentPos;
    return Rval;
}

/**
 * Create FMP4 index
 *
 * @param [in] Fmp4Idx Fmp4 index
 * @param [in] Media Media information
 * @param [in] Stream Stream handler
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Idx_Create(FRAGMENT_IDX_s *Fmp4Idx, SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        Rval = W2F(AmbaWrap_memset(Fmp4Idx, 0, sizeof(FRAGMENT_IDX_s)));
        if (Rval == FORMAT_OK) {
            Fmp4Idx->Media = Media;
            Fmp4Idx->Stream = Stream;
            Rval = Fmp4Idx_AllocateBuffer(Fmp4Idx);
            if (Rval == FORMAT_OK) {
                SVC_MOVIE_INFO_s *Movie;
                AmbaMisra_TypeCast(&Movie, &Media);
                Rval = FragmentCache_Create(&Fmp4Idx->Caches, Movie);
                if (Rval == FORMAT_OK) {
                    Rval = Fmp4Idx_InitIsoIdxInfo(Fmp4Idx, Movie);
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Idx_Create incorrect Media type %u", Media->MediaType, 0U, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

/**
 * Delete FMP4 index
 *
 * @param [in] Fmp4Idx Fmp4 index
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Idx_Delete(FRAGMENT_IDX_s *Fmp4Idx)
{
    AmbaMisra_TouchUnused(Fmp4Idx);
    return FORMAT_OK;
}

