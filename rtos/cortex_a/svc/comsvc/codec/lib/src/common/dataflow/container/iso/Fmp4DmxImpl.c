/**
 * @file Fmp4DmxImpl.c
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
#include "format/SvcIso.h"
#include "Fmp4DmxImpl.h"
#include "../FormatAPI.h"
#include "../MemByteOp.h"
#include "../ByteOp.h"
#include "../BitOp.h"
#include "../H264.h"
#include "../H265.h"

static UINT32 Fmp4Dmx_GetFrameInfo(void *IdxMgr, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    FRAGMENT_IDX_s *Fmp4Idx;
    AmbaMisra_TypeCast(&Fmp4Idx, &IdxMgr);
    AmbaMisra_TouchUnused(IdxMgr);
    return Fmp4Idx_GetFrameInfo(Fmp4Idx, TrackId, FrameNo, Direction, FrameInfo);
}

#define MAX_TAIL_SIZE (12U + (12U * SVC_FORMAT_MAX_TRACK_PER_MEDIA) + 4U + 1U)  /**< Maximum tail size */
static UINT32 Fmp4Dmx_CheckTailCrc32(const SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT8 SubVer)
{
    UINT32 Rval = S2F(Stream->Func->Seek(Stream, (INT64)(-1 * (INT64)Size), SVC_STREAM_SEEK_END));
    if (Rval == FORMAT_OK) {
        UINT8 TailBuffer[MAX_TAIL_SIZE];
        UINT32 Pos = 0U;
        if (SubVer == 0U) {
            /* v3.0, [BoxHeadr][ClipInfo][U32: CRC(BoxHeader + ClipInfo)] */
            Rval = SvcFormat_ReadStream(Stream, Size - 4U, Pos, TailBuffer, MAX_TAIL_SIZE, &Pos);
            if (Rval == OK) {
                UINT32 TmpU32;
                UINT32 Crc = AmbaUtility_Crc32(TailBuffer, Size - 4U);
                Rval = SvcFormat_GetBe32(Stream, &TmpU32);
                if (Rval == OK) {
                    if (Crc != TmpU32) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Crc incorrect!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ReadStream failed!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            /* v3.1, [BoxHeadr][ClipInfo][U32: CRC(BoxHeader+ClipInfo+CreateTime)][U8: 1(Validity)] */
            Rval = SvcFormat_ReadStream(Stream, Size - 5U, Pos, TailBuffer, MAX_TAIL_SIZE, &Pos);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutMemBe32(&Pos, TailBuffer, MAX_TAIL_SIZE, Movie->MediaInfo.CreationTime);
                if (Rval == FORMAT_OK) {
                    UINT32 TmpU32;
                    UINT32 Crc = AmbaUtility_Crc32(TailBuffer, Size - 1U);
                    Rval = SvcFormat_GetBe32(Stream, &TmpU32);
                    if (Rval == FORMAT_OK) {
                        if (Crc == TmpU32) {
                            UINT8 TmpU8;
                            Rval = SvcFormat_GetByte(Stream, &TmpU8);
                            if (Rval == FORMAT_OK) {
                                if (TmpU8 != 1U) {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Last byte is not 1!", __func__, NULL, NULL, NULL, NULL);
                                    Rval = FORMAT_ERR_INVALID_FORMAT;
                                }
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Crc incorrect!", __func__, NULL, NULL, NULL, NULL);
                            Rval = FORMAT_ERR_INVALID_FORMAT;
                        }
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcFormat_ReadStream failed!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_IO_ERROR;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 Fmp4Dmx_GetTailSize(SVC_STREAM_HDLR_s *Stream, const SVC_MOVIE_INFO_s *Movie, UINT32 *TailSize)
{
    // size|AMBA|frag|dst_0|count_0|dst_1|count_1|...
    UINT32 Rval = FORMAT_OK;
    UINT32 Size = 12U + ((8U + 4U) * ((UINT32) Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount));
    UINT8 Type, MainVer, SubVer;
    SvcFormat_GetAmbaBoxVersion(&Movie->MediaInfo, &Type, &MainVer, &SubVer);
    if ((Type == SVC_AMBA_BOX_TYPE_MW) && (MainVer == 3U)) {
        Size += 4U; // CRC
        if (SubVer == 1U) {
            Size += 1U;
        }
        Rval = Fmp4Dmx_CheckTailCrc32(Movie, Stream, Size, SubVer);
    }
    *TailSize = Size;
    return Rval;
}

typedef struct {
    UINT32 FragmentCount;
    UINT32 FrameCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA];
    UINT64 Duration[SVC_FORMAT_MAX_TRACK_PER_MEDIA];
} FMP4_TAIL_s;

static UINT32 Fmp4Dmx_ParseTailAmbaBox(SVC_STREAM_HDLR_s *Stream, FMP4_TAIL_s *Tail, UINT8 VideoTrackCount, UINT8 AudioTrackCount, UINT8 TextTrackCount)
{
    // size|AMBA|frag|dts_0|count_0|dts_1|count_1|...
    UINT32 Rval = SvcFormat_GetBe32(Stream, &Tail->FragmentCount);
    if (Rval == FORMAT_OK) {
        UINT8 i;
        UINT8 Idx = 0;
        for (i = 0; i < VideoTrackCount; i++) {
            Rval = SvcFormat_GetBe64(Stream, &Tail->Duration[Idx]);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe32(Stream, &Tail->FrameCount[Idx]);
            }
            if (Rval != FORMAT_OK) {
                break;
            }
            Idx++;
        }
        if (Rval == FORMAT_OK) {
            for (i = 0; i < AudioTrackCount; i++) {
                Rval = SvcFormat_GetBe64(Stream, &Tail->Duration[Idx]);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBe32(Stream, &Tail->FrameCount[Idx]);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
                Idx++;
            }
        }
        if (Rval == FORMAT_OK) {
            for (i = 0; i < TextTrackCount; i++) {
                Rval = SvcFormat_GetBe64(Stream, &Tail->Duration[Idx]);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBe32(Stream, &Tail->FrameCount[Idx]);
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
                Idx++;
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseTail(SVC_STREAM_HDLR_s *Stream, SVC_MOVIE_INFO_s *Movie)
{
    // seek to tail to check size|AMBA|frag|dts_0|count_0|dts_1|count_1|...
    UINT32 TailSize, Rval;
    Rval = Fmp4Dmx_GetTailSize(Stream, Movie, &TailSize);
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, (INT64)(-1 * (INT64)TailSize), SVC_STREAM_SEEK_END));
        if (Rval == FORMAT_OK) {
            UINT32 BoxSize, BoxTag;
            /* Get Box Size, Tag */
            Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
            if (Rval == FORMAT_OK) {
                if ((BoxSize == TailSize) && (BoxTag == TAG_AMBA)) {
                    FMP4_TAIL_s Tail;
                    Rval = Fmp4Dmx_ParseTailAmbaBox(Stream, &Tail, Movie->VideoTrackCount, Movie->AudioTrackCount, Movie->TextTrackCount);
                    if (Rval == FORMAT_OK) {
                        // parse ok, update track fno&dts in one shot, to avoid partial update
                        UINT8 i;
                        UINT32 Idx = 0;
                        SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
                        Movie->IsoInfo.FragmentCount = Tail.FragmentCount;
                        for (i = 0; i < Movie->VideoTrackCount; i++) {
                            Track = &Movie->VideoTrack[i].Info;
                            Track->FrameCount = Tail.FrameCount[Idx];
                            Track->DTS = Track->InitDTS + Tail.Duration[Idx];
                            Track->NextDTS = Track->DTS;
                            Idx++;
                        }
                        for (i = 0; i < Movie->AudioTrackCount; i++) {
                            Track = &Movie->AudioTrack[i].Info;
                            Track->FrameCount = Tail.FrameCount[Idx];
                            Track->DTS = Track->InitDTS + Tail.Duration[Idx];
                            Track->NextDTS = Track->DTS;
                            Idx++;
                        }
                        for (i = 0; i < Movie->TextTrackCount; i++) {
                            Track = &Movie->TextTrack[i].Info;
                            Track->FrameCount = Tail.FrameCount[Idx];
                            Track->DTS = Track->InitDTS + Tail.Duration[Idx];
                            Track->NextDTS = Track->DTS;
                            Idx++;
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "AMBA not found", NULL, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Fmp4Dmx_GetTailSize failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseH264SliceType(SVC_STREAM_HDLR_s *Stream, UINT8 *SliceType)
{
    UINT8 SliceHeader[64];
    UINT32 OffsetTmp = 0U;
    UINT32 Rval = SvcFormat_ReadStream(Stream, 64, 0U, SliceHeader, 64U, &OffsetTmp);
    if (Rval == FORMAT_OK) {
        SVC_BIT_OP_s Ctx;
        UINT32 TmpU32;
        SvcFormat_InitBitBuf(&Ctx, SliceHeader, 64);
        Rval = SvcFormat_GetUe(&Ctx, &TmpU32); /* FirstMbInSlice */
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetUe(&Ctx, &TmpU32);
            if (Rval == FORMAT_OK) {
                if (TmpU32 > 4U) {
                    TmpU32 -= 5U;
                }
                *SliceType = (UINT8)TmpU32;
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseH264FrameType(SVC_STREAM_HDLR_s *Stream, UINT32 FramePos, UINT32 FrameSize, const SVC_ISO_VIDEO_AVC_TRACK_INFO_s *Avc, UINT8 *FrameType)
{
    UINT32 Rval = S2F(Stream->Func->Seek(Stream, (INT64)FramePos, SVC_STREAM_SEEK_START));
    if (Rval == FORMAT_OK) {
        UINT32 NaluSize, NALULenSize, ReadSize = 0U;
        UINT8 NaluType;
        NALULenSize = ((UINT32)Avc->NALULength & 0x03U) + 1U;
        while (ReadSize < FrameSize) {
            if (NALULenSize == 4U) {
                Rval = SvcFormat_GetBe32(Stream, &NaluSize);
            } else { // NALULenSize == 2U
                UINT16 Tmp16 = 0U;
                Rval = SvcFormat_GetBe16(Stream, &Tmp16);
                NaluSize = (UINT32)Tmp16;
            }
            if (Rval == FORMAT_OK) {
                ReadSize += NALULenSize;
                Rval = SvcFormat_GetByte(Stream, &NaluType);
                if (Rval == FORMAT_OK) {
                    NaluType &= 0x1FU;
                    if (NaluType == (UINT8)SVC_H264_NALU_TYPE_IDR) {
                        *FrameType = SVC_FIFO_TYPE_IDR_FRAME;
                    } else if (NaluType == SVC_H264_NALU_TYPE_SLICE) {
                        UINT8 SliceType = 0U;
                        Rval = Fmp4Dmx_ParseH264SliceType(Stream, &SliceType);
                        if (Rval == FORMAT_OK) {
                            switch (SliceType) {
                            case P_SLICE:
                            case SP_SLICE:
                                *FrameType = SVC_FIFO_TYPE_P_FRAME;
                                break;
                            case B_SLICE:
                                *FrameType = SVC_FIFO_TYPE_B_FRAME;
                                break;
                            case I_SLICE:
                            case SI_SLICE:
                                *FrameType = SVC_FIFO_TYPE_I_FRAME;
                                break;
                            default:
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unknown SliceType!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_INVALID_FORMAT;
                                break;
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get Fmp4Dmx_ParseH264SliceType failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        ReadSize += NaluSize;
                        Rval = S2F(Stream->Func->Seek(Stream, (INT64)FramePos + (INT64)ReadSize, SVC_STREAM_SEEK_START));
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get NaluType failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get NaluSize failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if ((Rval != FORMAT_OK) || (*FrameType != SVC_FIFO_TYPE_UNDEFINED)) {
                break;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseH265SliceType_NotFirstSlice(SVC_BIT_OP_s *PpsCtx, SVC_BIT_OP_s *SliceCtx, UINT8 PicSizeInCtbsY, UINT8 *NumExtraSliceHeaderBits, UINT8 *DependentSliceSegmentFlag)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 DependentSliceSegmentsEnabledFlag = 0U;
    UINT32 TmpU32;
    Rval = SvcFormat_GetUe(PpsCtx, &TmpU32); /* pps_pic_parameter_set_id */
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetUe(PpsCtx, &TmpU32); /* pps_seq_parameter_set_id */
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_Get1Bit(PpsCtx, &DependentSliceSegmentsEnabledFlag); /* dependent_slice_segments_enabled_flag */
        if (Rval == FORMAT_OK) {
            UINT8 TmpU8;
            Rval = SvcFormat_Get1Bit(PpsCtx, &TmpU8); /* output_flag_present_flag */
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBit(PpsCtx, 3U, &TmpU32);  /* num_extra_slice_header_bits : u(3) */
        if (Rval == FORMAT_OK) {
            *NumExtraSliceHeaderBits = (UINT8)TmpU32;
        }
    }
    if (Rval == FORMAT_OK) {
        if (DependentSliceSegmentsEnabledFlag != 0U) {
            Rval = SvcFormat_Get1Bit(SliceCtx, DependentSliceSegmentFlag);  /* dependent_slice_segment_flag */
        }
    }
    if (Rval == FORMAT_OK) {
        UINT8 SliceSegmentAddressSize, i, TmpU8;
        SliceSegmentAddressSize  = PicSizeInCtbsY;
        for (i = 0U; i < SliceSegmentAddressSize; i++) {
            Rval = SvcFormat_Get1Bit(SliceCtx, &TmpU8);
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseH265SliceType(SVC_STREAM_HDLR_s *Stream, SVC_ISO_VIDEO_HVC_TRACK_INFO_s *Hvc, UINT8 NaluType, UINT8 *SliceType)
{
    UINT8 SliceHeader[64];
    UINT32 OffsetTmp = 0U;
    UINT32 Rval = SvcFormat_ReadStream(Stream, 64, 0U, SliceHeader, 64U, &OffsetTmp);
    UINT8 Found = 0U;
    if (Rval == FORMAT_OK) {
        SVC_BIT_OP_s SliceCtx, PpsCtx;
        UINT32 TmpU32;
        UINT8 FirstSliceSegmentInPicFlag = 0U, TmpU8;
        SvcFormat_InitBitBuf(&SliceCtx, SliceHeader, 64U);
        SvcFormat_InitBitBuf(&PpsCtx, &Hvc->PPS[2], (UINT32)Hvc->PPSLen - 2U);
        Rval = SvcFormat_Get1Bit(&SliceCtx, &FirstSliceSegmentInPicFlag); /* first_slice_segment_in_pic_flag */
        if (Rval == FORMAT_OK) {
            if ((NaluType >= SVC_H265_NALU_TYPE_BLA_W_LP) && (NaluType <= SVC_H265_NALU_TYPE_CRA_NUT)) {
                Rval = SvcFormat_Get1Bit(&SliceCtx, &TmpU8); /* no_output_of_prior_pics_flag */
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetUe(&SliceCtx, &TmpU32); /* slice_pic_parameter_set_id */
            }
        }

        if (Rval == FORMAT_OK) {
            UINT8 DependentSliceSegmentFlag = 0U;
            UINT8 NumExtraSliceHeaderBits = 0U;
            if (FirstSliceSegmentInPicFlag == 0U) {
                Rval = Fmp4Dmx_ParseH265SliceType_NotFirstSlice(&PpsCtx, &SliceCtx, Hvc->PicSizeInCtbsY, &NumExtraSliceHeaderBits, &DependentSliceSegmentFlag);
            }
            if (Rval == FORMAT_OK) {
                if (DependentSliceSegmentFlag == 0U) {
                    UINT8 i;
                    for(i = 0U; i < NumExtraSliceHeaderBits; i++) {
                        Rval = SvcFormat_Get1Bit(&SliceCtx, &TmpU8);
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetUe(&SliceCtx, &TmpU32);
                        if (Rval == FORMAT_OK) {
                            Found = 1U;
                            *SliceType = (UINT8)TmpU32;
                        }
                    }
                }
            }
        }
    }
    if ((Rval == FORMAT_OK) && (Found == 0U)) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SliceType not found!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

#define SVC_H265_SLICE_TYPE_B   0U  /**< H265 Slice Type B */
#define SVC_H265_SLICE_TYPE_P   1U  /**< H265 Slice Type P */
#define SVC_H265_SLICE_TYPE_I   2U  /**< H265 Slice Type I */
static UINT32 Fmp4Dmx_ParseH265FrameType(SVC_STREAM_HDLR_s *Stream, UINT32 FramePos, UINT32 FrameSize, SVC_ISO_VIDEO_HVC_TRACK_INFO_s *Hvc, UINT8 *FrameType)
{
    UINT32 Rval = S2F(Stream->Func->Seek(Stream, (INT64)FramePos, SVC_STREAM_SEEK_START));
    if (Rval == FORMAT_OK) {
        UINT32 NaluSize, NALULenSize, ReadSize = 0U;
        UINT8 NaluType, TmpU8;
        NALULenSize = ((UINT32)Hvc->NALULength & 0x03U) + 1U;
        while (ReadSize < FrameSize) {
            if (NALULenSize == 4U) {
                Rval = SvcFormat_GetBe32(Stream, &NaluSize);
            } else { // NALULenSize == 2U
                UINT16 Tmp16 = 0U;
                Rval = SvcFormat_GetBe16(Stream, &Tmp16);
                NaluSize = (UINT32)Tmp16;
            }
            if (Rval == FORMAT_OK) {
                ReadSize += NALULenSize;
                Rval = SvcFormat_GetByte(Stream, &NaluType);
                if (Rval == FORMAT_OK) {
                    NaluType &= 0x7EU;
                    NaluType >>= 1U;
                    Rval = SvcFormat_GetByte(Stream, &TmpU8);
                    if (Rval == FORMAT_OK) {
                        switch (NaluType) {
                        case SVC_H265_NALU_TYPE_IDR_W_RDAL:
                        case SVC_H265_NALU_TYPE_IDR_N_LP:
                            *FrameType = SVC_FIFO_TYPE_IDR_FRAME;
                            break;
                        case SVC_H265_NALU_TYPE_TRAIL_N:
                        case SVC_H265_NALU_TYPE_TRAIL_R:
                        case SVC_H265_NALU_TYPE_TSA_N:
                        case SVC_H265_NALU_TYPE_TSA_R:
                        case SVC_H265_NALU_TYPE_STSA_N:
                        case SVC_H265_NALU_TYPE_STSA_R:
                        case SVC_H265_NALU_TYPE_RADL_N:
                        case SVC_H265_NALU_TYPE_RADL_R:
                        case SVC_H265_NALU_TYPE_RASL_N:
                        case SVC_H265_NALU_TYPE_RASL_R:
                        case SVC_H265_NALU_TYPE_BLA_W_LP:
                        case SVC_H265_NALU_TYPE_BLA_W_RADL:
                        case SVC_H265_NALU_TYPE_BLA_N_LP:
                        case SVC_H265_NALU_TYPE_CRA_NUT:
                            {
                                UINT8 SliceType = 0U;
                                Rval = Fmp4Dmx_ParseH265SliceType(Stream, Hvc, NaluType, &SliceType);
                                if (Rval == FORMAT_OK) {
                                    if (SliceType == SVC_H265_SLICE_TYPE_B) {
                                        *FrameType = SVC_FIFO_TYPE_B_FRAME;
                                    } else if (SliceType == SVC_H265_SLICE_TYPE_P) {
                                        *FrameType = SVC_FIFO_TYPE_P_FRAME;
                                    } else if (SliceType == SVC_H265_SLICE_TYPE_I) {
                                        *FrameType = SVC_FIFO_TYPE_I_FRAME;
                                    } else {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unknown SliceType!", __func__, NULL, NULL, NULL, NULL);
                                        Rval = FORMAT_ERR_INVALID_FORMAT;
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get Fmp4Dmx_ParseH265SliceType failed!", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                            break;
                        default:
                            ReadSize += NaluSize;
                            Rval = S2F(Stream->Func->Seek(Stream, (INT64)FramePos + (INT64)ReadSize, SVC_STREAM_SEEK_START));
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
                            }
                            break;
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get NaluType failed!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get NaluSize failed!", __func__, NULL, NULL, NULL, NULL);
            }
            if ((Rval != FORMAT_OK) || (*FrameType != SVC_FIFO_TYPE_UNDEFINED)) {
                break;
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseFrameType(SVC_STREAM_HDLR_s *Stream, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, UINT8 MediaId, UINT32 FramePos, UINT32 FrameSize, UINT8 *FrameType)
{
    UINT32 Rval;
    *FrameType = SVC_FIFO_TYPE_UNDEFINED;
    if ((MediaId == SVC_FORMAT_MID_AVC) || (MediaId == SVC_FORMAT_MID_H264)) {
        Rval = Fmp4Dmx_ParseH264FrameType(Stream, FramePos, FrameSize, &IsoTrack->Avc, FrameType);
    } else if ((MediaId == SVC_FORMAT_MID_HVC) || (MediaId == SVC_FORMAT_MID_H265)) {
        Rval = Fmp4Dmx_ParseH265FrameType(Stream, FramePos, FrameSize, &IsoTrack->Hvc, FrameType);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported MediaId!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static UINT32 ParseGopStructureMN_Idr(UINT32 FrameCount)
{
    UINT32 Rval = FORMAT_OK;
    if (FrameCount != 0U) {
        Rval = FORMAT_ERR_INVALID_FORMAT;
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 ParseGopStructureMN_I(UINT32 FrameCount, UINT32 ICount, UINT32 PCount, UINT32 BCount, UINT32 *M, UINT32 *N)
{
    UINT32 Rval = FORMAT_OK;
    if (*M == 0U) {
        *M = BCount + 1U;
    } else {
        if (((ICount + PCount) * (*M)) != FrameCount) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure! (M)", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    if (*N == 0U) {
        *N = FrameCount;
    } else {
        if ((ICount * (*N)) != FrameCount) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure! (N)", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 ParseGopStructureMN_P(UINT32 FrameCount, UINT32 ICount, UINT32 PCount, UINT32 BCount, UINT32 *M)
{
    UINT32 Rval = FORMAT_OK;
    if (*M == 0U) {
        *M = BCount + 1U;
    } else {
        if (((ICount + PCount) * (*M)) != FrameCount) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure! (M)", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 ParseGopStructureMN_B(UINT32 FrameCount, UINT32 PCount, UINT32 BCount, UINT32 M, UINT32 N)
{
    UINT32 Rval = FORMAT_OK;
    if ((BCount == 1U) && (PCount > 0U)) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure! (PCount, BCount)", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    if ((M != 0U) && ((FrameCount % M) == 0U)) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure! (M)", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    if ((N != 0U) && ((FrameCount % N) == 0U)) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid structure! (N)", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseGopStructureMN(SVC_STREAM_HDLR_s *Stream, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, UINT8 MediaId, UINT32 SampleCount, const UINT32 *FramePos, const UINT32 *FrameSize, UINT32 *M, UINT32 *N)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 IdrCount = 0U, ICount = 0U, PCount = 0U, BCount = 0U;
    UINT32 TmpM = 0, TmpN = 0, i;
    for (i = 0; i < SampleCount; i++) {
        UINT8 FrameType;
        Rval = Fmp4Dmx_ParseFrameType(Stream, IsoTrack, MediaId, FramePos[i], FrameSize[i], &FrameType);
        if (Rval == FORMAT_OK) {
            if (FrameType == SVC_FIFO_TYPE_IDR_FRAME) {
                IdrCount++;
                Rval = ParseGopStructureMN_Idr(i);
            } else if (FrameType == SVC_FIFO_TYPE_I_FRAME) {
                ICount++;
                Rval = ParseGopStructureMN_I(i, ICount, PCount, BCount, &TmpM, &TmpN);
            } else if (FrameType == SVC_FIFO_TYPE_P_FRAME) {
                PCount++;
                Rval = ParseGopStructureMN_P(i, ICount, PCount, BCount, &TmpM);
            } else if (FrameType == SVC_FIFO_TYPE_B_FRAME) {
                BCount++;
                Rval = ParseGopStructureMN_B(i, PCount, BCount, TmpM, TmpN);
            } else {
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseGopStructure: Unsupported FrameType. (#%u Type %u)", i, FrameType, 0U, 0U, 0U);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            }
        }
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseGopStructure: Unsupported GOP. (#%u Type %u ICount %u PCount %u BCount %u)", i, FrameType, ICount, PCount, BCount);
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseGopStructure: Unsupported GOP. (#%u Type %u IdrCount %u M %u N %u)", i, FrameType, IdrCount, TmpM, TmpN);
            break;
        }
    }
    if (IdrCount != 1U) {
        Rval = FORMAT_ERR_INVALID_FORMAT;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseGopStructure: Unsupported GOP. (#%u IdrCount %u M %u N %u)", i, IdrCount, TmpM, TmpN, 0U);
    }
    *M = TmpM;
    *N = TmpN;
    return Rval;
}

/* TODO: IPBB series */
static UINT32 Fmp4Dmx_ParseGopStructure(SVC_STREAM_HDLR_s *Stream, SVC_VIDEO_TRACK_INFO_s *Video, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack, UINT64 DataOffset, UINT32 SampleCount, UINT32 Flag)
{
    UINT32 Rval = FORMAT_OK, i;
    UINT32 AccumulateOffset = 0U;
    if (SampleCount <= SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG) {
        UINT32 FramePos[SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG] = {0};
        UINT32 FrameSize[SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG] = {0};
        for (i = 0; i < SampleCount; i++) {
            Rval = SvcFormat_GetBe32(Stream, &FrameSize[i]);
            if (Rval == FORMAT_OK) {
                FramePos[i] = (UINT32)DataOffset + AccumulateOffset;
                AccumulateOffset += FrameSize[i];
                if ((Flag & 0x400U) != 0U) {    // sample flags preset
                    UINT32 Tmp32;
                    Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                }
            }
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_GetBe32() error!", __func__, NULL, NULL, NULL, NULL);
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            UINT32 M = 0U, N = 0U;
            Rval = Fmp4Dmx_ParseGopStructureMN(Stream, IsoTrack, Video->Info.MediaId, SampleCount, FramePos, FrameSize, &M, &N);
            if (Rval == FORMAT_OK) {
                Video->M = (M == 0U)? (UINT16)SampleCount : (UINT16)M;
                Video->N = (N == 0U)? (UINT16)SampleCount : (UINT16)N;
                Video->GOPSize = SampleCount;
                if (((Video->N % Video->M) != 0U) || ((Video->GOPSize % (UINT32)Video->N) != 0U)) {
                    AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseGopStructure: Unsupported GOP. (M %u N %u GopSize %u)", Video->M, Video->N, Video->GOPSize, 0U, 0U);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseGopStructure: Too many samples per fragment! (%u > %u)", SampleCount, SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseTraf(SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT32 *SampleCount, UINT32 *DecodeTime, UINT8 *CaptureTimeMode)
{
    UINT32 Rval, BoxSize, BoxTag;
    UINT64 ParsePos = ParseStartPos;
    Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
    if (Rval == FORMAT_OK) {
        if (BoxTag == TAG_TFHD) {
            ParsePos += BoxSize;
            Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParsePos, SVC_STREAM_SEEK_START));
            if (Rval == FORMAT_OK) {
                Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                if (Rval == FORMAT_OK) {
                    if (BoxTag == TAG_TFDT) {
                        UINT32 Tmp32;
                        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                        if (Rval == FORMAT_OK) {
                            /* Get BaseMediaDecodeTime */
                            /* BaseMediaDecodeTime = (the total sample count of (n-1) run)*(defaule sample duration) */
                            Rval = SvcFormat_GetBe32(Stream, DecodeTime);
                            if (Rval == FORMAT_OK) {
                                ParsePos += BoxSize;
                                Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParsePos, SVC_STREAM_SEEK_START));
                                if (Rval == FORMAT_OK) {
                                    Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                                    if (Rval == FORMAT_OK) {
                                        if (BoxTag == TAG_TRUN) {
                                            Rval = SvcFormat_GetBe32(Stream, &Tmp32); /* Flag */
                                            if (Rval == FORMAT_OK) {
                                                if ((Tmp32 & 0x100U) != 0U) {
                                                    *CaptureTimeMode = 1U;
                                                }
                                                Rval = SvcFormat_GetBe32(Stream, SampleCount);
                                            }
                                        } else {
                                            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoof: Unsupported tag %x! (expected TRUN)", BoxTag, 0U, 0U, 0U, 0U);
                                            Rval = FORMAT_ERR_INVALID_FORMAT;
                                        }
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TFDT!", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoof: Unsupported tag %x! (expected TFDT)", BoxTag, 0U, 0U, 0U, 0U);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    }
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TFHD!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoof: Unsupported tag %x! (expected TFHD) ", BoxTag, 0U, 0U, 0U, 0U);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParseEndPos, SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TAG_TRAF!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseTrafSvc(SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT32 *SampleCount, SVC_VIDEO_TRACK_INFO_s *Video, SVC_ISO_VIDEO_TRACK_INFO_s *IsoTrack)
{
    UINT32 Rval, BoxSize, BoxTag;
    UINT64 ParsePos = ParseStartPos;
    Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
    if (Rval == FORMAT_OK) {
        if (BoxTag == TAG_TFHD) {
            UINT64 BaseDataOffset = 0U;
            UINT32 Tmp32;
            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe32(Stream, &Tmp32);
            }
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetBe64(Stream, &BaseDataOffset);
            }
            if (Rval == FORMAT_OK) {
                ParsePos += BoxSize;
                Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParsePos, SVC_STREAM_SEEK_START));
                if (Rval == FORMAT_OK) {
                    Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                    if (Rval == FORMAT_OK) {
                        if (BoxTag == TAG_TRUN) {   /* No TFDT from SVC's clips. */
                            UINT32 Flag;
                            Rval = SvcFormat_GetBe32(Stream, &Flag);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetBe32(Stream, SampleCount);
                                if (Rval == FORMAT_OK) {
                                    if (Video != NULL) {
                                        // need to parse GOP structure
                                        UINT32 DataOffset;
                                        Rval = SvcFormat_GetBe32(Stream, &DataOffset);
                                        if (Rval == FORMAT_OK) {
                                            if ((Flag & 0xFFU) == 0x5U) {
                                                Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                                            }
                                            if (Rval == FORMAT_OK) {
                                                Rval = Fmp4Dmx_ParseGopStructure(Stream, Video, IsoTrack, BaseDataOffset + (UINT64)DataOffset, *SampleCount, Flag);
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoofSvc: Unsupported tag %x! (expected TRUN)", BoxTag, 0U, 0U, 0U, 0U);
                            Rval = FORMAT_ERR_INVALID_FORMAT;
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TFHD!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoofSvc: Unsupported tag %x! (expected TFHD) ", BoxTag, 0U, 0U, 0U, 0U);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParseEndPos, SVC_STREAM_SEEK_START));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TAG_TRAF!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 Fmp4Dmx_ParseMoof(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT8 Type, UINT32 *SampleCount)
{
    UINT32 Rval;
    UINT32 BoxSize, BoxTag;
    UINT64 ParsePos = ParseStartPos;
    UINT64 RemainSize = ParseEndPos - ParseStartPos;
    UINT8 TrackCount = 0;
    UINT64 DecodeTime[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0U};       // Base media decode time of the moof (MW: by TFDT / SVC: by FrameCount)
    UINT32 MySampleCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0U};    // SampleCount will be used later, so use a local array to keep it and update in one shot
    do {
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
            } else {
                if (BoxTag == TAG_MFHD) {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in TAG_MFHD!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else if (BoxTag == TAG_TRAF) {
                    if (Type == SVC_AMBA_BOX_TYPE_MW) {
                        UINT8 CaptureTimeMode = 0;
                        UINT32 Time = 0;
                        Rval = Fmp4Dmx_ParseTraf(Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), &MySampleCount[TrackCount], &Time, &CaptureTimeMode);
                        if (Rval == FORMAT_OK) {
                            DecodeTime[TrackCount] = Time;
                            if (Movie->IsoInfo.FragmentCount == 0U) {
                                if (TrackCount < Movie->VideoTrackCount) {
                                    // video track, assign capture time mode
                                    Movie->VideoTrack[TrackCount].CaptureTimeMode = CaptureTimeMode;
                                }
                            }
                        }
                    } else {
                        if ((Movie->IsoInfo.FragmentCount == 0U) && (TrackCount < Movie->VideoTrackCount)) {
                            // first GOP's video track, need to parse GOP structure
                            Rval = Fmp4Dmx_ParseTrafSvc(Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), &MySampleCount[TrackCount], &Movie->VideoTrack[TrackCount], &Movie->IsoInfo.VideoTrack[TrackCount]);
                        } else {
                            Rval = Fmp4Dmx_ParseTrafSvc(Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), &MySampleCount[TrackCount], NULL, NULL);
                        }
                        if (Rval == FORMAT_OK) {
                            // calculate DecodeTime (SVC clip, must be CFR)
                            const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
                            UINT8 Idx = TrackCount;
                            if (TrackCount < Movie->VideoTrackCount) {
                                Track = &Movie->VideoTrack[Idx].Info;
                            } else {
                                Idx -= Movie->VideoTrackCount;
                                if (Idx < Movie->AudioTrackCount) {
                                    Track = &Movie->AudioTrack[Idx].Info;
                                } else {
                                    Idx -= Movie->AudioTrackCount;
                                    Track = &Movie->TextTrack[Idx].Info;
                                }
                            }
                            DecodeTime[TrackCount] = (UINT64)Track->FrameCount * (UINT64)Track->TimePerFrame;
                        }
                    }
                    TrackCount++;
                } else {
                    AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoof: Unsupport tag %x!", BoxTag, 0U, 0U, 0U, 0U);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
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
        // check if track count is correct
        if (TrackCount != (Movie->VideoTrackCount + Movie->AudioTrackCount + Movie->TextTrackCount)) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_ParseMoof: Incorrect track count %u!", TrackCount, 0U, 0U, 0U, 0U);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        } else {
            // update frame count (count up to the current fragment) in one shot, to avoid corruption
            // keep decode time as DTS, i.e., only count up to the previous fragment
            UINT8 Idx = 0;
            UINT8 i;
            SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
            for (i = 0; i < Movie->VideoTrackCount; i++) {
                SampleCount[Idx] = MySampleCount[Idx];  // update SampleCount for further use
                Track = &Movie->VideoTrack[i].Info;
                Track->FrameCount += SampleCount[Idx];
                if (Movie->IsoInfo.FragmentCount == 0U) {
                    Track->InitDTS = DecodeTime[Idx];
                }
                Track->DTS = DecodeTime[Idx];
                Track->NextDTS = Track->DTS;
                Idx++;
            }
            for (i = 0; i < Movie->AudioTrackCount; i++) {
                SampleCount[Idx] = MySampleCount[Idx];  // update SampleCount for further use
                Track = &Movie->AudioTrack[i].Info;
                Track->FrameCount += SampleCount[Idx];
                if (Movie->IsoInfo.FragmentCount == 0U) {
                    Track->InitDTS = DecodeTime[Idx];
                }
                Track->DTS = DecodeTime[Idx];
                Track->NextDTS = Track->DTS;
                Idx++;
            }
            for (i = 0; i < Movie->TextTrackCount; i++) {
                SampleCount[Idx] = MySampleCount[Idx];  // update SampleCount for further use
                Track = &Movie->TextTrack[i].Info;
                Track->FrameCount += SampleCount[Idx];
                if (Movie->IsoInfo.FragmentCount == 0U) {
                    Track->InitDTS = DecodeTime[Idx];
                }
                Track->DTS = DecodeTime[Idx];
                Track->NextDTS = Track->DTS;
                Idx++;
            }
            Movie->IsoInfo.FragmentCount++;
        }
    }
    return Rval;
}

static void Fmp4Dmx_RemoveLastFragment(SVC_MOVIE_INFO_s *Movie, const UINT32 *SampleCount)
{
    /* Ignore the last moof due to incomplete clip.
     * Only need to update the frame count cause the dts is taken from BaseMediaDecodeTime.
     * BaseMediaDecodeTime = (the total sample count of (n-1) run)*(defaule sample duration)
     */
    UINT32 i, Idx = 0;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
    Movie->IsoInfo.FragmentCount--;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        Track = &Movie->VideoTrack[i].Info;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Remove : video[%u] %u => %u!", i, Track->FrameCount, Track->FrameCount - SampleCount[Idx], 0, 0);
        Track->FrameCount -= SampleCount[Idx];
        Idx++;
    }
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        Track = &Movie->AudioTrack[i].Info;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Remove : audio[%u] %u => %u!", i, Track->FrameCount, Track->FrameCount - SampleCount[Idx], 0, 0);
        Track->FrameCount -= SampleCount[Idx];
        Idx++;
    }
    for (i = 0; i < Movie->TextTrackCount; i++) {
        Track = &Movie->TextTrack[i].Info;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Remove : text[%u] %u => %u!", i, Track->FrameCount, Track->FrameCount - SampleCount[Idx], 0, 0);
        Track->FrameCount -= SampleCount[Idx];
        Idx++;
    }
}

static void Fmp4Dmx_FinalizeLastFragment(SVC_MOVIE_INFO_s *Movie)
{
    UINT32 i;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
    /* for svc's sdk7's clip, fixed TimePerFrame. */
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        Track = &Movie->VideoTrack[i].Info;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Finalize : video[%u] %u => %u!", i, (UINT32)Track->DTS, (UINT32)Track->InitDTS + (Track->FrameCount * Track->TimePerFrame), 0U, 0U);
        Track->DTS = Track->InitDTS + ((UINT64)Track->FrameCount * (UINT64)Track->TimePerFrame);
        Track->NextDTS = Track->DTS;
    }
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        Track = &Movie->AudioTrack[i].Info;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Finalize : audio[%u] %u => %u!", i, (UINT32)Track->DTS, (UINT32)Track->InitDTS + (Track->FrameCount * Track->TimePerFrame), 0U, 0U);
        Track->DTS = Track->InitDTS + ((UINT64)Track->FrameCount * (UINT64)Track->TimePerFrame);
        Track->NextDTS = Track->DTS;
    }
    for (i = 0; i < Movie->TextTrackCount; i++) {
        Track = &Movie->TextTrack[i].Info;
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Finalize : text[%u] %u => %u!", i, (UINT32)Track->DTS, (UINT32)Track->InitDTS + (Track->FrameCount * Track->TimePerFrame), 0U, 0U);
        Track->DTS = Track->InitDTS + ((UINT64)Track->FrameCount * (UINT64)Track->TimePerFrame);
        Track->NextDTS = Track->DTS;
    }
}

static UINT32 Fmp4Dmx_FastParse(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT64 ParseStartPos, UINT64 ParseEndPos, UINT32 *SampleCount)
{
    UINT32 Rval = Fmp4Dmx_ParseMoof(Movie, Stream, ParseStartPos, ParseEndPos, SVC_AMBA_BOX_TYPE_MW, SampleCount);
#ifndef CONFIG_ENABLE_CAPTURE_TIME_MODE
    if (Rval == FORMAT_OK) {
        UINT8 i;
        for (i = 0; i < Movie->VideoTrackCount; i++) {
            if (Movie->VideoTrack[i].CaptureTimeMode == 1U) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s CaptureTimeMode not supported!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
                break;
            }
        }
    }
#endif
    if (Rval == FORMAT_OK) {
        Rval = Fmp4Dmx_ParseTail(Stream, Movie);
        if (Rval == FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s ParseTail OK!", __func__, NULL, NULL, NULL, NULL);
            // will finish ParseClip(), no need to seek to MOOF end
        } else {
            if ((Rval == FORMAT_ERR_INVALID_FORMAT) || (Rval == FORMAT_ERR_NOT_ENOUGH_BYTES)) {
                // seek to MOOF end
                Rval = S2F(Stream->Func->Seek(Stream, (INT64)ParseEndPos, SVC_STREAM_SEEK_START));
                if (Rval == FORMAT_OK) {
                    // return not enough byte to denote tail is not available
                    Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in MOOF!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    return Rval;
}

#define FTYP_DONE   (0x01U) /**< FTYP is parsed */
#define MOOV_DONE   (0x02U) /**< MDAT is parsed */
#define MOOF_DONE   (0x04U) /**< the 1st MOOF is parsed */
#define TAIL_DONE   (0x08U) /**< TAIL is parsed */
#define ALL_DONE    (FTYP_DONE | MOOV_DONE | MOOF_DONE | TAIL_DONE)

static UINT32 Fmp4Dmx_ParseClip(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    SVC_MEDIA_INFO_s * const Media = &Movie->MediaInfo;
    SVC_ISO_PRIV_INFO_s * const IsoDmx = &Movie->IsoInfo;
    UINT32 SampleCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA] = {0U};  // the sampe count of the last moof
    UINT64 ParsePos = 0;
    UINT64 RemainSize = Media->Size;
    UINT32 BoxSize, BoxTag;
    UINT8 Type = 0;
    UINT8 Done = 0;
    do {
        /* Get Box Size, Tag */
        Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
        if (Rval == FORMAT_OK) {
            if (RemainSize < BoxSize) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                // can go ahead even if the clip is incomplete
                Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
            } else {
                if (BoxTag == TAG_FTYP) {
                    Rval = IsoDmx_ParseFtyp(Stream, IsoDmx, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
                    if (Rval == FORMAT_OK) {
                        Done |= FTYP_DONE;
                    }
                } else if (BoxTag == TAG_MOOV) {
                    Rval = IsoDmx_ParseMoov(Media, Stream, IsoDmx, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), 1U);
                    if (Rval == FORMAT_OK) {
                        Done |= MOOV_DONE;
                        Type = Movie->MediaInfo.SubFormat >> 6U;
                    }
                } else if (BoxTag == TAG_MDAT) {
                    if (BoxSize == 1U) { /* Large size(64bits)version */
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s CO64 not supported!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_INVALID_FORMAT;
                    } else {
                        Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                        if (Rval != FORMAT_OK) {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in MDAT!", __func__, NULL, NULL, NULL, NULL);
                        }
                    }
                } else if (BoxTag == TAG_MOOF) {
                    if (Movie->IsoInfo.FragmentCount == 0U) {
                        // keep the 1st moof's position, to speed up Fmp4Idx's parsing
                        Movie->IsoInfo.FirstFragmentPos = ParsePos;
                    }
                    if ((Type == SVC_AMBA_BOX_TYPE_MW) && (Movie->IsoInfo.FragmentCount == 0U)) {
                        // MW clip, the 1st MOOF, try fast parse
                        Rval = Fmp4Dmx_FastParse(Movie, Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), SampleCount);
                        if (Rval == FORMAT_OK) {
                            // tail is available, set TAIL_DONE to finish parse
                            Done |= TAIL_DONE;
                        } else {
                            if (Rval == FORMAT_ERR_NOT_ENOUGH_BYTES) {
                                // the 1st MOOF is ok but tail is invalid
                                Rval = FORMAT_OK;
                            }
                        }
                    } else {
                        // SVC clip or MW corrupted clip, slow parse
                        Rval = Fmp4Dmx_ParseMoof(Movie, Stream, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize), Type, SampleCount);
                    }
                    if (Rval == FORMAT_OK) {
                        // at least one MOOF is done, set MOOF_DONE
                        Done |= MOOF_DONE;
                    }
                } else if (BoxTag == TAG_FREE) {
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in FREE!", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_Parse: Unsupport tag %x!", BoxTag, 0U, 0U, 0U, 0U);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
                if (Rval == FORMAT_OK) {
                    ParsePos += BoxSize;
                    RemainSize -= BoxSize;
                    if (((Done & ALL_DONE) == ALL_DONE) || (RemainSize == 0U)) {
                        break;
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get box header error!", __func__, NULL, NULL, NULL, NULL);
        }
    } while (Rval == FORMAT_OK);
    if ((Rval == FORMAT_OK) || (Rval == FORMAT_ERR_NOT_ENOUGH_BYTES)) {
        if (Type != SVC_AMBA_BOX_TYPE_MW) {
            // SVC clip has no tail, so there is simple way to know if the clip is really complete
            if ((Rval == FORMAT_OK) && (RemainSize == 0U)) {
                if (Movie->IsoInfo.FragmentCount > 0U) {
                    // the clip is fully parsed, and at least 1 fragment => assume ok, finalize last fragment
                    Fmp4Dmx_FinalizeLastFragment(Movie);
                    Done |= TAIL_DONE;
                }
            }
        }
        switch (Done) {
        case ALL_DONE:
            // do nothing (Rval must be FORMAT_OK)
            break;
        case (FTYP_DONE | MOOV_DONE | MOOF_DONE):
            if (Movie->IsoInfo.FragmentCount > 1U) {
                // corrupted clip with at least 2 moof, remove the last fragment
                Fmp4Dmx_RemoveLastFragment(Movie, SampleCount);
                Rval = FORMAT_OK;
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Not enough MOOF!", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
            }
            break;
        default:
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Miss essential boxes!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
            break;
        }
#if 0
        if (Rval == FORMAT_OK) {
            UINT32 i;
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track;
            for (i = 0; i < Movie->VideoTrackCount; i++) {
                Track = &Movie->VideoTrack[i].Info;
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "video[%u] FrameCount = %u, InitDTS = %u, DTS = %u!", i, Track->FrameCount, (UINT32)Track->InitDTS, (UINT32)Track->DTS, 0U);
            }
            for (i = 0; i < Movie->AudioTrackCount; i++) {
                Track = &Movie->AudioTrack[i].Info;
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "audio[%u] FrameCount = %u, InitDTS = %u, DTS = %u!", i, Track->FrameCount, (UINT32)Track->InitDTS, (UINT32)Track->DTS, 0U);
            }
            for (i = 0; i < Movie->TextTrackCount; i++) {
                Track = &Movie->TextTrack[i].Info;
                AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "text[%u] FrameCount = %u, InitDTS = %u, DTS = %u!", i, Track->FrameCount, (UINT32)Track->InitDTS, (UINT32)Track->DTS, 0U);
            }
        }
#endif
    }
    return Rval;
}

/**
 *  Parsing movie information.
 *
 *  @param [out] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Parse(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    SVC_MEDIA_INFO_s * const Media = &Movie->MediaInfo;
    Media->Valid = 1U;
    Rval = S2F(Stream->Func->GetLength(Stream, &Media->Size));
    if (Rval == FORMAT_OK) {
        Rval = S2F(Stream->Func->Seek(Stream, 0, SVC_STREAM_SEEK_START));
        if (Rval == FORMAT_OK) {
            Rval = Fmp4Dmx_ParseClip(Movie, Stream);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_NormalizeTimeScale(Media);
            }
            /**
             *  It means invalid format when
             *  (1) read bytes are not as expected.
             *  (2) wrong tag value and box size.
             */
            if ((Rval == FORMAT_ERR_NOT_ENOUGH_BYTES) || (Rval == FORMAT_ERR_INVALID_FORMAT)) {
                Rval = FORMAT_ERR_INVALID_FORMAT;
                Media->Valid = 0U;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek the beginning of the file failed!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Get file length error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 *  Process of demuxing a movie.
 *
 *  @param [in] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] End Indicating the end of process
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Process(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, FRAGMENT_IDX_s *Fmp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 Direction, UINT8 End, UINT8 *Event)
{
    return IsoDmx_Process(&Movie->MediaInfo, Stream, Fmp4Idx, FeedInfo, TargetTime, Direction, End, Event);
}

/**
 *  Open a Fmp4 demuxing format handler.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr, FRAGMENT_IDX_s *Fmp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo)
{
    UINT32 Rval = FORMAT_OK;
    SVC_MEDIA_INFO_s * const Media = Hdlr->Media;
    AmbaMisra_TouchUnused(Hdlr);
    if ((Media->Valid == 1U) && (Media->MediaType == SVC_MEDIA_INFO_MOVIE)) {
        Rval = Fmp4Idx_Create(Fmp4Idx, Media, Hdlr->Stream);
        if (Rval == FORMAT_OK) {
            SVC_MOVIE_INFO_s *Movie;
            AmbaMisra_TypeCast(&Movie, &Media);
            Rval = IsoDmx_InitFeedInfo(Movie, FeedInfo, Fmp4Dmx_GetFrameInfo, Hdlr->Direction, Hdlr->Speed);
            // should never error, so no need to release Fmp4Idx on error
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Open: Invalid media %u or media type %u", Media->Valid, Media->MediaType, 0U, 0U, 0U);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    if (Rval == FORMAT_ERR_INVALID_FORMAT) {
        Media->Valid = 0U;
    }
    return Rval;
}

/**
 *  Close a Fmp4 demuxing format handler.
 *
 *  @param [in] Fmp4Idx Fmp4 index
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Close(FRAGMENT_IDX_s *Fmp4Idx)
{
    return Fmp4Idx_Delete(Fmp4Idx);
}

/**
 *  Seek and set the start time of demuxing.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] Speed Processing speed after seeking
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, FRAGMENT_IDX_s *Fmp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT32 Direction, UINT32 Speed)
{
    return IsoDmx_Seek(Hdlr, Fmp4Idx, FeedInfo, TargetTime, Direction, Speed, Fmp4Dmx_GetFrameInfo);
}

/**
 *  Feeding a frame from the fmp4 file.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Fmp4Idx Fmp4 index
 *  @param [in] TrackId Track id
 *  @param [in] TargetTime The time of the target frame
 *  @param [in] FrameType Frame type
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Dmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, FRAGMENT_IDX_s *Fmp4Idx, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType)
{
    return IsoDmx_FeedFrame(Hdlr, Fmp4Idx, TrackId, TargetTime, FrameType, Fmp4Dmx_GetFrameInfo);
}

