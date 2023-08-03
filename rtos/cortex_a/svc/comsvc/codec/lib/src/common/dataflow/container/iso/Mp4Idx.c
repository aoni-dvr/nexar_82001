/**
 * @file Mp4Idx.c
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
#include "format/SvcFormat.h"
#include "IsoDmx.h"
#include "Mp4Idx.h"
#include "../ByteOp.h"
#include "../FormatAPI.h"
#include "format/SvcDemuxer.h"
#include "../index/IndexCache.h"

static UINT32 Mp4Idx_Read(INDEX_CACHE_HDLR_s *IndexCache, UINT8 TrackId, UINT8 ItemId, UINT32 Index, UINT8 Direction, void *Data);
static UINT32 Mp4Idx_GetKeyFrameNo(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 FrameNo, void *Data);
static UINT32 Mp4Idx_GetStts(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT8 SttsItemNo, UINT32 FrameNo, void *pStts);

static UINT8 ISO_IDX_GET_FRAME_TYPE(UINT32 FrameSize)
{
    return (UINT8)((FrameSize & 0xE0000000U) >> 29U);
}

static UINT32 ISO_IDX_GET_FRAME_SIZE(UINT32 FrameSize)
{
    return (FrameSize & 0x03FFFFFFU);
}

static void Mp4Idx_SetIndexItemConfig(INDEX_CACHE_ITEM_CFG_s *Item, UINT32 BytePerIndex, UINT32 IndexCount, UINT64 FileOffset)
{
    Item->BytePerIndex = (UINT8)BytePerIndex;
    Item->FileOffset = FileOffset;
    Item->IndexCount = IndexCount;
}

static UINT32 Mp4Idx_InitIndexCfg(const MP4_IDX_s *Mp4Idx, INDEX_CACHE_CFG_s *IndexCfg)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MOVIE_INFO_s * const Movie = Mp4Idx->Movie;
    IndexCfg->TrackCount = Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount;
    IndexCfg->Stream = Mp4Idx->Stream;
    if (Mp4Idx->CreateMode == ISO_IDX_MODE_READ) {
        IndexCfg->Mode = INDEX_MODE_READ_STREAM;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s only support read!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
        INDEX_CACHE_TRACK_CFG_s *TrackCfg;
        const SVC_ISO_VIDEO_TRACK_INFO_s *IsoVideo;
        const SVC_ISO_AUDIO_TRACK_INFO_s *IsoAudio;
        const SVC_ISO_TEXT_TRACK_INFO_s *IsoText;
        UINT8 i, TrackId = 0;
        for (i = 0; i < Movie->VideoTrackCount; i++) {
            Track = &Movie->VideoTrack[i].Info;
            IsoVideo = &Movie->IsoInfo.VideoTrack[i];
            TrackCfg = &IndexCfg->Track[TrackId];
            TrackCfg->MaxCachedIndex = Mp4Idx->MaxIdxNum;
            TrackCfg->ItemCount = 5;
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[0], (UINT32)sizeof(UINT32), Track->FrameCount, IsoVideo->StszOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[1], (UINT32)sizeof(UINT32), Track->FrameCount, IsoVideo->StcoOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[2], (UINT32)sizeof(CTTS_ENTRY_s), Track->FrameCount, IsoVideo->CttsOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[3], (UINT32)sizeof(UINT32), IsoVideo->KeyFrameCount, IsoVideo->StssOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[4], (UINT32)sizeof(STTS_ENTRY_s), IsoVideo->SttsCount, IsoVideo->SttsOffset);
            TrackId++;
        }
        for (i = 0; i < Movie->AudioTrackCount; i++) {
            Track = &Movie->AudioTrack[i].Info;
            IsoAudio = &Movie->IsoInfo.AudioTrack[i];
            TrackCfg = &IndexCfg->Track[TrackId];
            TrackCfg->MaxCachedIndex = Mp4Idx->MaxIdxNum;
            TrackCfg->ItemCount = 3;
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[0], (UINT32)sizeof(UINT32), Track->FrameCount, IsoAudio->StszOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[1], (UINT32)sizeof(UINT32), Track->FrameCount, IsoAudio->StcoOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[2], (UINT32)sizeof(STTS_ENTRY_s), IsoAudio->SttsCount, IsoAudio->SttsOffset);
            TrackId++;
        }
        for (i = 0; i < Movie->TextTrackCount; i++) {
            Track = &Movie->TextTrack[i].Info;
            IsoText = &Movie->IsoInfo.TextTrack[i];
            TrackCfg = &IndexCfg->Track[TrackId];
            TrackCfg->MaxCachedIndex = Mp4Idx->MaxIdxNum;
            TrackCfg->ItemCount = 3;
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[0], (UINT32)sizeof(UINT32), Track->FrameCount, IsoText->StszOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[1], (UINT32)sizeof(UINT32), Track->FrameCount, IsoText->StcoOffset);
            Mp4Idx_SetIndexItemConfig(&TrackCfg->Item[2], (UINT32)sizeof(STTS_ENTRY_s), IsoText->SttsCount, IsoText->SttsOffset);
            TrackId++;
        }
    }
    return Rval;
}

static UINT32 Mp4Idx_AllocateIndexBuffer(MP4_IDX_s *Mp4Idx)
{
    INDEX_CACHE_CFG_s IndexCfg = {0};
    UINT32 Rval = Mp4Idx_InitIndexCfg(Mp4Idx, &IndexCfg);
    if (Rval == FORMAT_OK) {
        Rval = IndexCache_Create(&Mp4Idx->IndexCache, &IndexCfg);
    }
    return Rval;
}

static UINT32 Mp4Idx_InitIsoIdxInfo(MP4_IDX_s *Mp4Idx)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 i;
    const SVC_MOVIE_INFO_s * const Movie = Mp4Idx->Movie;
    const SVC_VIDEO_TRACK_INFO_s *Video;
    ISO_IDX_VIDEO_INFO_s *IdxVideo;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        Video = &Movie->VideoTrack[i];
        IdxVideo = &Mp4Idx->TrackInfo[i].Video;
        IdxVideo->GOPSize = Video->GOPSize;
        IdxVideo->M = Video->M;
        IdxVideo->N = Video->N;
        IdxVideo->Mode = Video->Mode;
        if (IdxVideo->GOPSize > INDEX_CACHE_MAX_CACHED_COUNT) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s too large GOP size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
            break;
        }
    }
    return Rval;
}

/**
 * Create MP4 index API.
 *
 * @param [in] Mode create iso index mode.
 * @param [in] Movie movie information for create function reference.
 * @param [in] Stream stream handler for create function read/write file.
 * @param [in] MaxIdxNum define index size of each index item.
 * @param [out] Mp4Idx MP4 index api get buffer.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 * @see AMP_ER_CODE_e
 */
UINT32 Mp4Idx_Create(UINT8 Mode, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT32 MaxIdxNum, MP4_IDX_s *Mp4Idx)
{
    UINT32 Rval = W2F(AmbaWrap_memset(Mp4Idx, 0, sizeof(MP4_IDX_s)));
    if (Rval == FORMAT_OK) {
        Mp4Idx->CreateMode = Mode;
        Mp4Idx->Movie = Movie;
        Mp4Idx->Stream = Stream;
        Mp4Idx->MaxIdxNum = MaxIdxNum;
        if (Mode == ISO_IDX_MODE_READ) {
            Rval = Mp4Idx_AllocateIndexBuffer(Mp4Idx);
            if (Rval == FORMAT_OK) {
                Rval = Mp4Idx_InitIsoIdxInfo(Mp4Idx);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    }
    return Rval;
}

static UINT32 Mp4Idx_DeInitIsoIdxInfo(MP4_IDX_s *Mp4Idx)
{
    AmbaMisra_TouchUnused(Mp4Idx);
    return FORMAT_OK;
}

static UINT32 Mp4Idx_ReleaseIndexBuffer(MP4_IDX_s *Mp4Idx)
{
    return IndexCache_Delete(&Mp4Idx->IndexCache);
}

/**
 * Delete MP4 index API.
 *
 * @param [in] Mp4Idx MP4 index api.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Idx_Delete(MP4_IDX_s *Mp4Idx)
{
    UINT32 Rval = Mp4Idx_DeInitIsoIdxInfo(Mp4Idx);
    if (Rval == FORMAT_OK) {
        Rval = Mp4Idx_ReleaseIndexBuffer(Mp4Idx);
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memset(Mp4Idx, 0, sizeof(MP4_IDX_s)));
    }
    return Rval;
}

static void Mp4Idx_FrameTypeGen(ISO_IDX_VIDEO_INFO_s *IsoVideo, UINT8 ClosedGOP, UINT32 SampleIdx, UINT8 *VideoTmpJ)
{
    const UINT8 Type = SvcFormat_GetVideoFrameType(IsoVideo->Mode, IsoVideo->M, IsoVideo->N, IsoVideo->GOPSize, ClosedGOP, SampleIdx);
    switch (Type) {
    case SVC_FIFO_TYPE_IDR_FRAME:
        *VideoTmpJ = *VideoTmpJ | (UINT8)SvcFormat_Le2Be32(0x20000000U);
        break;
    case SVC_FIFO_TYPE_I_FRAME:
        *VideoTmpJ = *VideoTmpJ | (UINT8)SvcFormat_Le2Be32(0x40000000U);
        break;
    case SVC_FIFO_TYPE_P_FRAME:
        *VideoTmpJ = *VideoTmpJ | (UINT8)SvcFormat_Le2Be32(0x60000000U);
        break;
    case SVC_FIFO_TYPE_B_FRAME:
        *VideoTmpJ = *VideoTmpJ | (UINT8)SvcFormat_Le2Be32(0x80000000U);
        break;
    default:
        // other frame type
        break;
    }
    AmbaMisra_TouchUnused(IsoVideo);
}

static UINT32 Mp4Idx_GetVideoFrameSize(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, void *Data)
{
    UINT32 KeyFrameNo = 0;
    UINT32 Rval = Mp4Idx_GetKeyFrameNo(Mp4Idx, TrackId, FrameNo, &KeyFrameNo);
    if (Rval == FORMAT_OK) {
        ISO_IDX_TRACK_INFO_s * const IsoTrack = &Mp4Idx->TrackInfo[TrackId];
        ISO_IDX_VIDEO_INFO_s * const IsoVideo = &IsoTrack->Video;
        UINT32 Tmp32;
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 0, FrameNo, Direction, &Tmp32);
        if (Rval == FORMAT_OK) {
            // do not support CloseGOP
            Mp4Idx_FrameTypeGen(IsoVideo, 0, FrameNo - KeyFrameNo, (UINT8 *)&Tmp32);
            Rval = W2F(AmbaWrap_memcpy(Data, &Tmp32, sizeof(UINT32)));
        }
    }
    return Rval;
}

static UINT32 Mp4Idx_Read(INDEX_CACHE_HDLR_s *IndexCache, UINT8 TrackId, UINT8 ItemId, UINT32 Index, UINT8 Direction, void *Data)
{
    return IndexCache_Read(IndexCache, TrackId, ItemId, Index, Direction, Data);
}

static UINT32 Mp4Idx_GetKeyFrameNo(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 FrameNo, void *Data)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MOVIE_INFO_s * const Movie = Mp4Idx->Movie;
    UINT32 KeyFrameCount = Movie->IsoInfo.VideoTrack[TrackId].KeyFrameCount;
    UINT32 KeyFrameNo = 0;
    if (KeyFrameCount > 0U) {
        UINT32 i;
        UINT32 Tmp1, Tmp2;
        //Find a proper keyframe count before search: Max = Fno/1 , Min = Fno/GOPSize
        UINT32 ReadStart = FrameNo / Movie->VideoTrack[TrackId].GOPSize;
        UINT32 ReadLimit = FrameNo + 1U;
        if (ReadLimit > KeyFrameCount) {
            ReadLimit = KeyFrameCount;
        }
        for (i = ReadStart; i < ReadLimit; i++) {
            UINT8 Found = 0U;
            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 3, i, SVC_FORMAT_DIR_FORWARD, &Tmp1);
            if (Rval == FORMAT_OK) {
                if (i == (KeyFrameCount - 1U)) {
                    if ((FrameNo >= (SvcFormat_Le2Be32(Tmp1) - 1U))) {
                        KeyFrameNo = SvcFormat_Le2Be32(Tmp1) - 1U;
                        Found = 1U;
                    }
                } else {
                    Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 3, i + 1U, SVC_FORMAT_DIR_FORWARD, &Tmp2);
                    if (Rval == FORMAT_OK) {
                        if ((FrameNo >= (SvcFormat_Le2Be32(Tmp1) - 1U)) && (FrameNo < (SvcFormat_Le2Be32(Tmp2) - 1U))) {
                            KeyFrameNo = SvcFormat_Le2Be32(Tmp1) - 1U;
                            Found = 1U;
                        }
                    }
                }
            }
            if ((Rval != FORMAT_OK) || (Found == 1U)) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            if (i == ReadLimit) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Can't get key frame number!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s no key frame number!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if (Rval == FORMAT_OK){
        Rval = W2F(AmbaWrap_memcpy(Data, &KeyFrameNo, sizeof(UINT32)));
    }
    return Rval;
}

#if 0
static UINT32 Mp4Idx_SearchStts(MP4_IDX_s *Mp4Idx, UINT32 TrackId, UINT32 FrameNo, UINT32 SearchNum, STTS_ENTRY_s *pStts, UINT32 *SttsEntryNo, UINT32 *SttsNo)
{
    UINT32 Rval = FORMAT_OK;
    if (Mp4Idx->CreateMode == ISO_IDX_MODE_READ) {
        UINT32 i;
        STTS_ENTRY_s Stts;
        UINT32 SttsCountSum = 0;
        for (i = 0; (Rval == FORMAT_OK) && (i < SearchNum); i++) {
            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, i, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&Stts);
            if (Rval == FORMAT_OK) {
                SttsCountSum += SvcFormat_Le2Be32(Stts.SampleCount);
                if ((SttsCountSum - 1) >= FrameNo) {
                   *pStts = Stts;
                   *SttsEntryNo = i;
                   *SttsNo = FrameNo - (SttsCountSum - SvcFormat_Le2Be32(Stts.SampleCount));
                   break;
                }
            }
        }
        if (Rval == FORMAT_OK) {
            if (i == SearchNum) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Can't search Stts!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}
#endif

static UINT32 Mp4Idx_GetStts(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT8 SttsItemNo, UINT32 FrameNo, void *pStts)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MOVIE_INFO_s * const Movie = Mp4Idx->Movie;
    UINT32 SttsEntryCount = 0U;
#if 0
    UINT32 MaxIdxNum = Mp4Idx->MaxIdxNum;
#endif
    STTS_ENTRY_s SttsTmp2 = {0};
    AmbaMisra_TouchUnused(&FrameNo);
    if (TrackId < Movie->VideoTrackCount) {
        SttsEntryCount = Movie->IsoInfo.VideoTrack[TrackId].SttsCount;
    } else if (TrackId < (Movie->VideoTrackCount + Movie->AudioTrackCount)) {
        UINT8 AudioIdx = TrackId - Movie->VideoTrackCount;
        SttsEntryCount = Movie->IsoInfo.AudioTrack[AudioIdx].SttsCount;
    } else if (TrackId < (Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount)) {
        UINT8 TextIdx = TrackId - Movie->VideoTrackCount - Movie->AudioTrackCount;
        SttsEntryCount = Movie->IsoInfo.TextTrack[TextIdx].SttsCount;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect track ID!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if (Rval == FORMAT_OK) {
        if (SttsEntryCount == 0U) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s no Stts!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        } else if (SttsEntryCount == 1U) {
            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, SttsItemNo, 0, SVC_FORMAT_DIR_FORWARD, &SttsTmp2);
        } else {
#if 0
            UINT32 ReadStart;
            UINT32 ReadNum;
            UINT32 SttsEntryNo;
            UINT32 SttsNo;
            STTS_ENTRY_s SttsTmp1;
            if (SttsEntryCount <= MaxIdxNum) {
                ReadStart = 0;
                ReadNum = SttsEntryCount;
                Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, SttsItemNo, ReadStart, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp1);
                if (Rval == FORMAT_OK) {
                    Rval = Mp4Idx_SearchStts(Mp4Idx, TrackId, FrameNo, ReadNum, &SttsTmp2, &SttsEntryNo, &SttsNo);
                }
            } else {
                UINT32 i;
                UINT32 MaxSttsEntryUnit = SttsEntryCount / MaxIdxNum;
                for (i = 0; (Rval == FORMAT_OK) && (i < MaxSttsEntryUnit); i++) {
                    ReadStart = i * MaxIdxNum;
                    ReadNum = MaxIdxNum;
                    Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, SttsItemNo, ReadStart, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp1);
                    if (Rval == FORMAT_OK) {
                        Rval = Mp4Idx_SearchStts(Mp4Idx, TrackId, FrameNo, ReadNum, &SttsTmp2, &SttsEntryNo, &SttsNo);
                        if (Rval == FORMAT_OK) {
                            break;
                        }
                    }
                }
                if (Rval == FORMAT_OK) {
                    if (i == MaxSttsEntryUnit) {
                        MaxSttsEntryUnit = SttsEntryCount % MaxIdxNum;
                        if (MaxSttsEntryUnit != 0U) {
                            ReadStart = SttsEntryCount - MaxSttsEntryUnit;
                            ReadNum = MaxSttsEntryUnit;
                            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, SttsItemNo, ReadStart, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp1);
                            if (Rval == FORMAT_OK) {
                                Rval = Mp4Idx_SearchStts(Mp4Idx, TrackId, FrameNo, ReadNum, &SttsTmp2, &SttsEntryNo, &SttsNo);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not found!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                        }
                    }
                }
            }
#else
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
#endif
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(pStts, &SttsTmp2, sizeof(STTS_ENTRY_s)));
    }
    return Rval;
}

#if 0
static UINT32 Mp4Idx_GetSttsSum(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 StartSttsNo, UINT32 StartSttsEntryNo, UINT32 EndSttsNo, UINT32 EndSttsEntryNo, UINT64 *pSttsSum)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 SttsSum = 0;
    UINT32 StartEntryNo;
    UINT32 EndEntryNo;
    UINT32 StartNo;
    UINT32 EndNo;
    STTS_ENTRY_s SttsTmp;
    if (StartSttsEntryNo <= EndSttsEntryNo) {
        StartEntryNo = StartSttsEntryNo;
        StartNo = StartSttsNo;
        EndEntryNo = EndSttsEntryNo;
        EndNo = EndSttsNo;
    } else {
        StartEntryNo = EndSttsEntryNo;
        StartNo = EndSttsNo;
        EndEntryNo = StartSttsEntryNo;
        EndNo = StartSttsNo;
    }
    if (StartEntryNo == EndEntryNo) {
       Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, StartEntryNo, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp);
       if (Rval == FORMAT_OK) {
           if (StartNo == EndNo) {
               SttsSum = 0;
           } else {
               SttsSum = ((EndNo > StartNo) ? (EndNo - StartNo) : (StartNo - EndNo)) * SvcFormat_Le2Be32(SttsTmp.SampleDelta);
           }
       }
    } else {
        if (Mp4Idx->CreateMode == ISO_IDX_MODE_READ) {
            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, StartEntryNo, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp);
            if (Rval == FORMAT_OK) {
                SttsSum = (SvcFormat_Le2Be32(SttsTmp.SampleCount) - StartNo) * SvcFormat_Le2Be32(SttsTmp.SampleDelta);
                StartEntryNo++;
                while (StartEntryNo < EndEntryNo) {
                    Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, StartEntryNo, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp);
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                    SttsSum += (SvcFormat_Le2Be32(SttsTmp.SampleCount) * SvcFormat_Le2Be32(SttsTmp.SampleDelta));
                    StartEntryNo++;
                }
                if (Rval == FORMAT_OK) {
                    Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, EndEntryNo, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp);
                    if (Rval == FORMAT_OK) {
                        SttsSum += (EndNo * SvcFormat_Le2Be32(SttsTmp.SampleDelta));
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not supported!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    *pSttsSum = SttsSum;
    //LOG_VERB("*SttsSum = %llu", *SttsSum);
    return Rval;
}
#endif

static UINT32 Mp4Idx_GetVideoDTS(const MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 FrameNo, void *Dts)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 U64Dts = 0;
#if 0
    UINT32 MaxIdxNum = Mp4Idx->MaxIdxNum;
    UINT32 SttsEntryNo;
    UINT32 SttsNo;
    STTS_ENTRY_s SttsTmp1;
    STTS_ENTRY_s SttsTmp2;
#endif
    const SVC_MOVIE_INFO_s * const Movie = Mp4Idx->Movie;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->VideoTrack[TrackId].Info;
#if 0
    UINT32 MaxFrameCount = Track->FrameCount;
    UINT32 NormalizeFactor = Track->TimeScale / Track->OrigTimeScale;
#endif
    UINT32 SttsEntryCount = Movie->IsoInfo.VideoTrack[TrackId].SttsCount;
    if (SttsEntryCount == 0U) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect SttsEntryCount!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else if (SttsEntryCount == 1U) {
        U64Dts = SVC_FORMAT_DURATION(FrameNo, Track->TimePerFrame) + Track->InitDTS;
    } else {
#if 0
        ISO_IDX_STTS_BUFFER_INFO_s * const SttsBuffer = Mp4Idx->SttsBuffer[TrackId];
        if (((FrameNo < SttsBuffer->StartFrameNo) || (FrameNo > SttsBuffer->EndFrameNo)) ||
            ((SttsBuffer->StartFrameNo == 0) && (SttsBuffer->EndFrameNo == 0))) {
            if (SttsEntryCount <= MaxIdxNum) {
                Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, 0, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp1);
                if (Rval == FORMAT_OK) {
                    Rval = Mp4Idx_SearchStts(Mp4Idx, TrackId, FrameNo, SttsEntryCount, &SttsTmp2, &SttsEntryNo, &SttsNo);
                }
            } else {
                UINT32 ReadStart;
                UINT32 ReadNum;
                UINT32 i = 0;
                UINT32 MaxSttsEntryUnit = SttsEntryCount / MaxIdxNum;
                for (i = 0; (Rval == FORMAT_OK) && (i < MaxSttsEntryUnit); i++) {
                    ReadStart = i * MaxIdxNum;
                    ReadNum = MaxIdxNum;
                    Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, ReadStart, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp1);
                    if (Rval == FORMAT_OK) {
                        Rval = Mp4Idx_SearchStts(Mp4Idx, TrackId, FrameNo, ReadNum, &SttsTmp2, &SttsEntryNo, &SttsNo);
                        break;
                    }
                    if (Rval != FORMAT_OK) {
                        break;
                    }
                }
                if (Rval == FORMAT_OK) {
                    if (i == MaxSttsEntryUnit) {
                        MaxSttsEntryUnit = SttsEntryCount % MaxIdxNum;
                        if (MaxSttsEntryUnit != 0U) {
                            ReadStart = SttsEntryCount - MaxSttsEntryUnit;
                            ReadNum = MaxSttsEntryUnit;
                            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, ReadStart, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp1);
                            if (Rval == FORMAT_OK) {
                                Rval = Mp4Idx_SearchStts(Mp4Idx, TrackId, FrameNo, ReadNum, &SttsTmp2, &SttsEntryNo, &SttsNo);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not found!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                        }
                    }
                }
            }
            if (Rval == FORMAT_OK) {
                UINT32 LoadSttsNo;
                UINT32 LoadFrameCount;
                UINT32 BufferIdx;
                UINT64 *Buffer = (UINT64 *)SttsBuffer->Buffer;
                Rval = Mp4Idx_GetSttsSum(Mp4Idx, TrackId, SttsBuffer->PreSttsNo, SttsBuffer->PreSttsEntryNo, SttsNo, SttsEntryNo, &SttsBuffer->SttsSum);
                if (Rval == FORMAT_OK) {
                    LoadFrameCount = SvcFormat_Le2Be32(SttsTmp2.SampleCount) - SttsNo;
                    if (SttsBuffer->PreSttsEntryNo < SttsEntryNo) {
                        Buffer[0] = SttsBuffer->PreSttsSum + SttsBuffer->SttsSum;
                    } else if (SttsBuffer->PreSttsEntryNo > SttsEntryNo) {
                        Buffer[0] = SttsBuffer->PreSttsSum - SttsBuffer->SttsSum;
                    } else {
                        if (SttsBuffer->PreSttsNo < SttsNo) {
                            Buffer[0] = SttsBuffer->PreSttsSum + SttsBuffer->SttsSum;
                        } else if (SttsBuffer->PreSttsNo > SttsNo) {
                            Buffer[0] = SttsBuffer->PreSttsSum - SttsBuffer->SttsSum;
                        } else {
                            Buffer[0] = SttsBuffer->PreSttsSum;
                        }
                    }
                    BufferIdx = 1;
                    while (BufferIdx < LoadFrameCount) {
                        Buffer[BufferIdx] = Buffer[BufferIdx - 1] + SvcFormat_Le2Be32(SttsTmp2.SampleDelta);
                        BufferIdx++;
                        if (BufferIdx >= MaxIdxNum) {
                            break;
                        }
                    }
                    if (Mp4Idx->CreateMode == ISO_IDX_MODE_READ) {
                        for (LoadSttsNo = (SttsEntryNo + 1); LoadSttsNo < SttsEntryCount; LoadSttsNo++) {
                            if (BufferIdx >= MaxIdxNum)
                                break;
                            Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 4, LoadSttsNo, SVC_FORMAT_DIR_FORWARD, (UINT8 *)&SttsTmp2);
                            if (Rval == FORMAT_OK) {
                                LoadFrameCount += SvcFormat_Le2Be32(SttsTmp2.SampleCount);
                                while (BufferIdx < LoadFrameCount) {
                                    Buffer[BufferIdx] = Buffer[BufferIdx - 1] + SvcFormat_Le2Be32(SttsTmp2.SampleDelta);
                                    BufferIdx++;
                                    if (BufferIdx >= MaxIdxNum) {
                                        break;
                                    }
                                }
                            }
                            if (Rval != FORMAT_OK) {
                                break;
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not supported!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                    if (Rval == FORMAT_OK) {
                        SttsBuffer->StartFrameNo = FrameNo;
                        SttsBuffer->FrameCount = FrameNo + BufferIdx;
                        SttsBuffer->EndFrameNo = SttsBuffer->FrameCount - 1;
                        SttsBuffer->PreSttsNo = SttsNo;
                        SttsBuffer->PreSttsEntryNo = SttsEntryNo;
                        SttsBuffer->PreSttsSum = Buffer[0];
                        U64Dts = Buffer[(FrameNo - SttsBuffer->StartFrameNo)] + Track->InitDTS;
                        U64Dts = U64Dts * NormalizeFactor;
                        //LOG_VERB("FrameNo[%u %u] U64Dts = %llu", FrameNo, NormalizeFactor, U64Dts);
                    }
                }
            }
        } else {
            UINT64 *Buffer = (UINT64 *)SttsBuffer->Buffer;
            U64Dts = Buffer[FrameNo - SttsBuffer->StartFrameNo] + Track->InitDTS;
            U64Dts = U64Dts * NormalizeFactor;
            //LOG_VERB("FrameNo[%u %u] U64Dts = %llu", FrameNo, NormalizeFactor, U64Dts);
        }
#else
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s not supported!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
#endif
    }
    if (Rval == FORMAT_OK) {
        Rval = W2F(AmbaWrap_memcpy(Dts, &U64Dts, sizeof(UINT64)));
    }
    return Rval;
}

static UINT32 Mp4Idx_GetVideo(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT8 Type, UINT32 FrameNo, UINT8 Direction, void *Data)
{
    UINT32 Rval;
    switch (Type) {
    case ISO_IDX_TYPE_V:
        Rval = Mp4Idx_GetVideoFrameSize(Mp4Idx, TrackId, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_VO:
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 1, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_CTTS:
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 2, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_K:
        Rval = Mp4Idx_GetKeyFrameNo(Mp4Idx, TrackId, FrameNo, Data);
        break;
    case ISO_IDX_TYPE_V_STTS:
        Rval = Mp4Idx_GetStts(Mp4Idx, TrackId, 4, FrameNo, Data);
        break;
    case ISO_IDX_TYPE_V_DTS:
        Rval = Mp4Idx_GetVideoDTS(Mp4Idx, TrackId, FrameNo, Data);
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect Type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
        break;
    }
    return Rval;
}

static UINT32 Mp4Idx_GetAudio(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT8 Type, UINT32 FrameNo, UINT8 Direction, void *Data)
{
    UINT32 Rval;
    switch (Type) {
    case ISO_IDX_TYPE_A:
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 0, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_AO:
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 1, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_A_STTS:
        Rval = Mp4Idx_GetStts(Mp4Idx, TrackId, 2, FrameNo, Data);
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect Type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
        break;
    }
    return Rval;
}

static UINT32 Mp4Idx_GetText(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT8 Type, UINT32 FrameNo, UINT8 Direction, void *Data)
{
    UINT32 Rval;
    switch (Type) {
    case ISO_IDX_TYPE_T:
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 0, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_TO:
        Rval = Mp4Idx_Read(&Mp4Idx->IndexCache, TrackId, 1, FrameNo, Direction, Data);
        break;
    case ISO_IDX_TYPE_T_STTS:
        Rval = Mp4Idx_GetStts(Mp4Idx, TrackId, 2, FrameNo, Data);
        break;
    default:
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect Type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
        break;
    }
    return Rval;
}

static UINT32 Mp4Idx_GetFrameInfoVideo(MP4_IDX_s *Mp4Idx, UINT8 TrackId, const SVC_VIDEO_TRACK_INFO_s *Video, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval;
    UINT32 FrameSize;
    /* Get Video frame size */
    Rval = Mp4Idx_GetVideo(Mp4Idx, TrackId, ISO_IDX_TYPE_V, FrameNo, Direction, &FrameSize);
    if (Rval == FORMAT_OK) {
        UINT64 FramePos;
        /* Get Video frame offset */
        UINT32 Tmp32;
        Rval = Mp4Idx_GetVideo(Mp4Idx, TrackId, ISO_IDX_TYPE_VO, FrameNo, Direction, &Tmp32);
        FramePos = SvcFormat_Le2Be32(Tmp32);
        if (Rval == FORMAT_OK) {
            CTTS_ENTRY_s CttsTmp = {0};
            FrameInfo->FramePos = FramePos;
            /* Get Video frame ctts */
            if (Video->M > 1U) {
                Rval = Mp4Idx_GetVideo(Mp4Idx, TrackId, ISO_IDX_TYPE_CTTS, FrameNo, Direction, &CttsTmp);
            }
            if (Rval == FORMAT_OK) {
                FrameInfo->FrameType = ISO_IDX_GET_FRAME_TYPE(SvcFormat_Le2Be32(FrameSize));
                if (FrameInfo->FrameType <= SVC_FIFO_TYPE_UNDEFINED) {
                    FrameInfo->FrameSize = ISO_IDX_GET_FRAME_SIZE(SvcFormat_Le2Be32(FrameSize)); /* Real frame size */
                    if ((FrameInfo->FrameSize < SVC_FIFO_MARK_EOS) && (FrameInfo->FrameSize > 0U)) {
                        Rval = Mp4Idx_GetVideo(Mp4Idx, TrackId, ISO_IDX_TYPE_V_DTS, FrameNo, Direction, &FrameInfo->DTS);
                        if (Rval == FORMAT_OK) {
                            FrameInfo->PTS = FrameInfo->DTS + SvcFormat_Le2Be32(CttsTmp.SampleOffset);
                            FrameInfo->TimePerFrame = Video->Info.TimePerFrame;
                            //AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Video: No = %u, Type = %u, PTS = %u, Pos = %x, Size = %x", FrameNo, FrameInfo->FrameType, (UINT32)FrameInfo->PTS, (UINT32)FrameInfo->FramePos, FrameInfo->FrameSize);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Undefined frame type!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            }
        }
    }
    return Rval;
}

static UINT32 Mp4Idx_GetFrameInfoAudio(MP4_IDX_s *Mp4Idx, UINT8 TrackId, const SVC_AUDIO_TRACK_INFO_s *Audio, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 FrameSize;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Audio->Info;
    /* Get Audio frame size */
    if (Track->MediaId == SVC_FORMAT_MID_ADPCM) {
        UINT32 SamplesPerChunk = (UINT32)(((UINT64)Track->TimePerFrame * (UINT64)Track->OrigTimeScale) / (UINT64)Track->TimeScale);
        FrameSize = SvcFormat_Le2Be32((((SamplesPerChunk - 1U) * 4U * (UINT32)Audio->Channels) / 8U) + (4U * (UINT32)Audio->Channels));
    } else if ((Track->MediaId == SVC_FORMAT_MID_PCM) || (Track->MediaId == SVC_FORMAT_MID_LPCM)) {
        UINT32 SamplesPerChunk = (UINT32)(((UINT64)Track->TimePerFrame * (UINT64)Track->OrigTimeScale) / (UINT64)Track->TimeScale);
        FrameSize = SvcFormat_Le2Be32((SamplesPerChunk * ((UINT32)Audio->BitsPerSample >> 3U)) * (UINT32)Audio->Channels);
    } else {
        Rval = Mp4Idx_GetAudio(Mp4Idx, TrackId, ISO_IDX_TYPE_A, FrameNo, Direction, &FrameSize);
    }
    if (Rval == FORMAT_OK) {
        UINT64 FramePos;
        /* Get Video frame offset */
        UINT32 Tmp32;
        Rval = Mp4Idx_GetAudio(Mp4Idx, TrackId, ISO_IDX_TYPE_AO, FrameNo, Direction, &Tmp32);
        FramePos = SvcFormat_Le2Be32(Tmp32);
        if (Rval == FORMAT_OK) {
            FrameInfo->FramePos = FramePos;
            FrameInfo->FrameType = SVC_FIFO_TYPE_AUDIO_FRAME;
            FrameInfo->FrameSize = SvcFormat_Le2Be32(FrameSize);
            if ((FrameInfo->FrameSize < SVC_FIFO_MARK_EOS) && (FrameInfo->FrameSize > 0U)) {
                FrameInfo->DTS = SVC_FORMAT_DURATION(FrameNo, Track->TimePerFrame) + Track->InitDTS;
                FrameInfo->PTS = FrameInfo->DTS + Track->TimePerFrame;
                FrameInfo->TimePerFrame = Track->TimePerFrame;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            }
        }
    }
    return Rval;
}

static UINT32 Mp4Idx_GetFrameInfoText(MP4_IDX_s *Mp4Idx, UINT8 TrackId, const SVC_TEXT_TRACK_INFO_s *Text, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval;
    UINT32 FrameSize;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Text->Info;
    /* Get Text frame size */
    Rval = Mp4Idx_GetText(Mp4Idx, TrackId, ISO_IDX_TYPE_T, FrameNo, Direction, &FrameSize);
    if (Rval == FORMAT_OK) {
        UINT64 FramePos;
        /* Get Video frame offset */
        UINT32 Tmp32;
        Rval = Mp4Idx_GetText(Mp4Idx, TrackId, ISO_IDX_TYPE_TO, FrameNo, Direction, &Tmp32);
        FramePos = SvcFormat_Le2Be32(Tmp32);
        if (Rval == FORMAT_OK) {
            FrameInfo->FramePos = FramePos;
            FrameInfo->FrameType = SVC_FIFO_TYPE_UNDEFINED;
            FrameInfo->FrameSize = SvcFormat_Le2Be32(FrameSize);
            if ((FrameInfo->FrameSize < SVC_FIFO_MARK_EOS) && (FrameInfo->FrameSize > 0U)) {
                if (Track->MediaId == SVC_FORMAT_MID_TEXT) { //see QT spec TEXT media, remove 16-bit length to get actual text.
                    if (FrameInfo->FrameSize > 2U) {
                        FrameInfo->FramePos += 2U;
                        FrameInfo->FrameSize -= 2U;
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Too small frame size!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    }
                }
                if (Rval == FORMAT_OK) {
                    FrameInfo->DTS = SVC_FORMAT_DURATION(FrameNo, Track->TimePerFrame) + Track->InitDTS;
                    FrameInfo->PTS = FrameInfo->DTS + Track->TimePerFrame;
                    FrameInfo->TimePerFrame = Track->TimePerFrame;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect frame size!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            }
        }
    }
    return Rval;
}

/**
 * Get all of the information of target frame.
 *
 * @param [in] Mp4Idx index manager.
 * @param [in] TrackId track id of media information.
 * @param [in] FrameNo target frame number of index data to get.
 * @param [in] Direction get data direction.
 * @param [out] FrameInfo buffer to write the return frame information.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Idx_GetFrameInfo(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    UINT32 Rval = FORMAT_OK;
    const SVC_MOVIE_INFO_s * const Movie = Mp4Idx->Movie;
    if (TrackId < Movie->VideoTrackCount) {
        Rval = Mp4Idx_GetFrameInfoVideo(Mp4Idx, TrackId, &Movie->VideoTrack[TrackId], FrameNo, Direction, FrameInfo);
    } else if (TrackId < (Movie->VideoTrackCount + Movie->AudioTrackCount)) {
        Rval = Mp4Idx_GetFrameInfoAudio(Mp4Idx, TrackId, &Movie->AudioTrack[TrackId - Movie->VideoTrackCount], FrameNo, Direction, FrameInfo);
    } else if (TrackId < (Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount)) {
        Rval = Mp4Idx_GetFrameInfoText(Mp4Idx, TrackId, &Movie->TextTrack[TrackId - Movie->VideoTrackCount - Movie->AudioTrackCount], FrameNo, Direction, FrameInfo);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect track ID!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

