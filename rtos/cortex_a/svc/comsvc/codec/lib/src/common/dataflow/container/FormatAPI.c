/**
 * @file FormatAPI.c
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
#include "FormatAPI.h"
#include "ByteOp.h"
#include "Muxer.h"
#include "H264.h"
#include "H265.h"

#define SVC_AMBA_BOX_VIDEO_SIZE_V2  ((3U * 4U) + (2U * 8U) + (4U * 4U) + (2U * 2U) + (4U * 1U)) /**< AMBA box video info size v2 */
#define SVC_AMBA_BOX_VIDEO_SIZE_V3  ((3U * 4U) + (2U * 8U) + (2U * 4U) + (2U * 2U) + (2U * 1U)) /**< AMBA box video info size v3 */
#define SVC_AMBA_BOX_AUDIO_SIZE     ((3U * 4U) + (2U * 8U) + 4U + (2U * 1U))    /**< AMBA box audio info size */
#define SVC_AMBA_BOX_TEXT_SIZE      ((3U * 4U) + (2U * 8U) + 4U + 1U)           /**< AMBA box text info size */

static inline UINT64 SVC_FORMAT_DTS_TO_FNO(UINT64 DTS, UINT32 TimePerFrame)
{
    return DTS / TimePerFrame;
}

static inline UINT64 SVC_FORMAT_DTS_TO_FNO_CEIL(UINT64 DTS, UINT32 TimePerFrame)
{
    return (DTS + TimePerFrame - 1U) / TimePerFrame;
}

static inline UINT64 SVC_FORMAT_SPLIT_DTS(UINT64 InitDTS, UINT32 MaxDuration, UINT32 TimeScale)
{
    return (InitDTS) + SVC_FORMAT_TIME_TO_DTS(MaxDuration, TimeScale);
}

/**
 * Initialize AMBA box version
 *
 * @param [in] Media Media information
 */
void SvcFormat_InitAmbaBoxVersion(SVC_MEDIA_INFO_s *Media)
{
    Media->SubFormat = SVC_AMBA_BOX_DEFAULT_TYPE << 6U;
    Media->SubFormat |= (SVC_AMBA_BOX_DEFAULT_MAIN_VER << 3U);
    Media->SubFormat |= SVC_AMBA_BOX_DEFAULT_SUB_VER;
}

/**
 * Get AMBA box version
 *
 * @param [in] Media Media information
 * @param [out] Type Media type
 * @param [out] MainVer Main version
 * @param [out] SubVer Sub version
 */
void SvcFormat_GetAmbaBoxVersion(const SVC_MEDIA_INFO_s *Media, UINT8 *Type, UINT8 *MainVer, UINT8 *SubVer)
{
    *Type = Media->SubFormat >> 6U;
    *MainVer = (Media->SubFormat >> 3U) & 3U;
    *SubVer = Media->SubFormat & 3U;
}

static UINT64 SvcFormat_GetGCD(UINT64 U, UINT64 V)
{
    UINT64 Rval = 0U;
    /* GCD(0,x) := x */
    if ((U == 0U) || (V == 0U)) {
        Rval = U | V;
    } else {
        UINT32 Shift = 0U;
        UINT64 TempU = U, TempV = V;
        /* Let Shift := lg K, where K is the greatest power of 2 dividing both U and V. */
        while (((TempU | TempV) & 1U) == 0U) {
            Shift++;
            TempU >>= 1;
            TempV >>= 1;
        }
        while ((TempU & 1U) == 0U) {
            TempU >>= 1;
        }
        /* From here on, U is always odd. */
        do {
            while ((TempV & 1U) == 0U) {  /* Loop X */
                TempV >>= 1;
            }
            /* Now U and V are both odd, so diff(U, V) is even. Let U = min(U, V), V = diff(U, V)/2. */
            if (TempU < TempV) {
                TempV -= TempU;
            } else {
                UINT64 diff = TempU - TempV;
                TempU = TempV;
                TempV = diff;
            }
            TempV >>= 1;
        } while (TempV != 0U);
        Rval = TempU << Shift;
    }
    return Rval;
}

/**
 * A helper funciton to compute the LCM
 *
 * @param [in] V the array of numbers
 * @param [in] Count the size of the array
 * @return the LCM of the numbers in v
 */
static UINT64 SvcFormat_GetLCM(const UINT64 *V, UINT32 Count)
{
    UINT32 i;
    UINT64 Rval = V[0];
    for (i = 1U; i < Count; i++) {
        UINT64 Ret = SvcFormat_GetGCD(Rval, V[i]);
        if (Ret != 0U) {
            Rval = (Rval * V[i]) / Ret;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_GetTimeScaleLCM(const SVC_MEDIA_INFO_s *Media, UINT32 *TimeScaleLCM)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 TrackCount = 0U;
    UINT64 TimeScale[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA + SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA + SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA + 1U] = {0};
    TimeScale[0] = *TimeScaleLCM;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        UINT8 i;
        const SVC_MOVIE_INFO_s * Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        for (i = 0U; i < Movie->VideoTrackCount; i++) {
            TimeScale[TrackCount + i + 1U] = (UINT64)Movie->VideoTrack[i].Info.TimeScale;
        }
        TrackCount = Movie->VideoTrackCount;
        for (i = 0U; i < Movie->AudioTrackCount; i++) {
            TimeScale[TrackCount + i + 1U] = (UINT64)Movie->AudioTrack[i].Info.TimeScale;
        }
        TrackCount += Movie->AudioTrackCount;
        for (i = 0U; i < Movie->TextTrackCount; i++) {
            TimeScale[TrackCount + i + 1U] = (UINT64)Movie->TextTrack[i].Info.TimeScale;
        }
        TrackCount += Movie->TextTrackCount;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong media type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    if (Rval == FORMAT_OK) {
        // compute the common Rate
        UINT64 LCM = SvcFormat_GetLCM(TimeScale, (UINT32)TrackCount + 1U);
        if (LCM > 0xFFFFFFFFU) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value of LCM", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
        *TimeScaleLCM = (UINT32)LCM;
    }
    return Rval;
}

/**
 * Minus buffer address
 *
 * @param [in] MinusSize The size to minus
 * @param [in] Offset The current position
 * @param [in] Size The size of the FIFO buffer
 * @param [out] NewOffset The resulted position
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_MinusAddr(UINT32 MinusSize, UINT32 Offset, UINT32 Size, UINT32 *NewOffset)
{
    UINT32 Rval = FORMAT_OK;
    if (Offset >= Size) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong offset!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        if (Offset >= MinusSize) {
            *NewOffset = Offset - MinusSize;
        } else {
            *NewOffset = Size - (MinusSize - Offset);
        }
    }
    return Rval;
}

/**
 * Add buffer address
 *
 * @param [in] PlusSize The size to plus
 * @param [in] Offset The current position
 * @param [in] Size The size of the FIFO buffer
 * @param [out] NewOffset The resulted position
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PlusAddr(UINT32 PlusSize, UINT32 Offset, UINT32 Size, UINT32 *NewOffset)
{
    UINT32 Rval = FORMAT_OK;
    if (Offset >= Size) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong offset!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_GENERAL_ERROR;
    } else {
        if ((Offset + PlusSize) < Size) {
            *NewOffset = Offset + PlusSize;
        } else {
            *NewOffset = PlusSize - (Size - Offset);
        }
    }
    return Rval;
}

static UINT32 WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT8 *Addr)
{
    UINT32 Rval;
    UINT32 Count;
    Rval = S2F(Stream->Func->Write(Stream, Size, Addr, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != Size) {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_IO_ERROR;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Write data to Stream fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Read data from FIFO to stream
 *
 * @param [in] Stream Stream
 * @param [in] DataSize The data size
 * @param [in] BufferOffset The current position
 * @param [in] BufferBase The FIFO buffer base
 * @param [in] BufferSize The size of the FIFO buffer
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 DataSize, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize)
{
    UINT32 Rval = FORMAT_OK;
    if (BufferOffset >= BufferSize) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong Addr!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    } else {
        if ((BufferOffset + DataSize) <= BufferSize) {
            Rval = WriteStream(Stream, DataSize, &BufferBase[BufferOffset]);
        } else {
            const UINT32 Rear = BufferSize - BufferOffset;
            Rval = WriteStream(Stream, Rear, &BufferBase[BufferOffset]);
            if (Rval == FORMAT_OK) {
                Rval = WriteStream(Stream, DataSize - Rear, BufferBase);
            }
        }
    }
    return Rval;
}

/**
 * Write data from FIFO to buffer
 *
 * @param [in] DstBuffer Destination buffer
 * @param [in] DataSize Size in bytes to be written
 * @param [in] BufferOffset The current position of FIFO buffer
 * @param [in] BufferBase Base of FIFO buffer
 * @param [in] BufferSize Size of FIFO buffer
 * @param [out] NewBufferOffset The resulted position of FIFO buffer
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_WriteStreamMem(UINT8 *DstBuffer, UINT32 DataSize, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset)
{
    UINT32  Rval = FORMAT_OK;
    *NewBufferOffset = BufferOffset;
    if (BufferOffset >= BufferSize) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong addr!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    } else {
        if ((BufferOffset + DataSize) <= BufferSize) {
            Rval = W2F(AmbaWrap_memcpy(DstBuffer, &BufferBase[BufferOffset], DataSize));
            *NewBufferOffset = BufferOffset + DataSize;
        } else {
            const UINT32 Rear = BufferSize - BufferOffset;
            Rval = W2F(AmbaWrap_memcpy(DstBuffer, &BufferBase[BufferOffset], Rear));
            if (Rval == FORMAT_OK) {
                Rval = W2F(AmbaWrap_memcpy(&DstBuffer[Rear], BufferBase, (ULONG)DataSize - (ULONG)Rear));
            }
            *NewBufferOffset = DataSize - Rear;
        }
        if (*NewBufferOffset == BufferSize) {
            *NewBufferOffset = 0U;
        }
    }
    return Rval;
}

static UINT32 ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT8 *Addr)
{
    UINT32 Rval;
    UINT32 Count;
    UINT64 Pos = 0;
    Rval = S2F(Stream->Func->Read(Stream, Size, Addr, &Count));
    if (Rval == FORMAT_OK) {
        if (Count != Size) {
            (void)Stream->Func->GetPos(Stream, &Pos);
            AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ReadStream(%u) : Read data from %u fail!", Size, (UINT32)Pos, 0, 0, 0);
            Rval = FORMAT_ERR_NOT_ENOUGH_BYTES;
        }
    } else {
        (void)Stream->Func->GetPos(Stream, &Pos);
        AmbaPrint_ModulePrintUInt5(SVC_MUXER_PRINT_MODULE_ID, "ReadStream(%u) : Read data from %u fail!", Size, (UINT32)Pos, 0, 0, 0);
    }
    return Rval;
}

/**
 * Read data from stream to FIFO
 *
 * @param [in] Stream Stream
 * @param [in] DataSize Size in bytes to be read
 * @param [in] BufferOffset The current position of FIFO buffer
 * @param [in] BufferBase Base of FIFO buffer
 * @param [in] BufferSize Size of FIFO buffer
 * @param [out] NewBufferOffset The resulted position of FIFO buffer
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 DataSize, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset)
{
    UINT32 Rval = FORMAT_OK;
    if (BufferOffset >= BufferSize) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong Addr!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    } else {
        if ((BufferOffset + DataSize) <= BufferSize) {
            Rval = ReadStream(Stream, DataSize, &BufferBase[BufferOffset]);
            if (Rval == FORMAT_OK) {
                *NewBufferOffset = BufferOffset + DataSize;
            }
        } else {
            const UINT32 Rear = BufferSize - BufferOffset;
            Rval = ReadStream(Stream, Rear, &BufferBase[BufferOffset]);
            if (Rval == FORMAT_OK){
                Rval = ReadStream(Stream, DataSize - Rear, BufferBase);
                if (Rval == FORMAT_OK) {
                    *NewBufferOffset = DataSize - Rear;
                }
            }
        }
        if (Rval == FORMAT_OK) {
            if (*NewBufferOffset == BufferSize) {
                *NewBufferOffset = 0U;
            }
        }
    }
    return Rval;
}

/**
 * Read data from memory to FIFO
 *
 * @param [out] Data Data
 * @param [in] DataSize Data size
 * @param [in] BufferOffset The current position of FIFO buffer
 * @param [in] BufferBase Base of FIFO buffer
 * @param [in] BufferSize Size of FIFO buffer
 * @param [out] NewBufferOffset The resulted position of FIFO buffer
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_ReadStreamMem(const UINT8 *Data, UINT32 DataSize, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset)
{
    UINT32 Rval = FORMAT_OK;
    if (BufferOffset >= BufferSize) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong Addr!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    } else {
        if ((BufferOffset + DataSize) <= BufferSize) {
            Rval = W2F(AmbaWrap_memcpy(&BufferBase[BufferOffset], Data, DataSize));
            *NewBufferOffset = BufferOffset + DataSize;
        } else {
            const UINT32 Rear = BufferSize - BufferOffset;
            Rval = W2F(AmbaWrap_memcpy(&BufferBase[BufferOffset], Data, Rear));
            if (Rval == FORMAT_OK) {
                Rval = W2F(AmbaWrap_memcpy(BufferBase, &Data[Rear], (ULONG)DataSize - (ULONG)Rear));
            }
            *NewBufferOffset = DataSize - Rear;
        }
        if (*NewBufferOffset == BufferSize) {
            *NewBufferOffset = 0U;
        }
    }
    return Rval;
}

/**
 * Get string of Movie information
 *
 * @param [in] Movie Movie information
 * @param [out] Buffer String buffer
 * @param [in] Size String size
 */
void SvcFormat_GetMovieInfoString(const SVC_MOVIE_INFO_s *Movie, char *Buffer, UINT32 Size)
{
    const char *Prefix = "Movie(";
    UINT8 i;
    AmbaUtility_StringAppend(Buffer, Size, Prefix);
    for (i = 0U; i < Movie->VideoTrackCount; i++) {
#if (SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA > 1U)
        if (i > 0U) {
            AmbaUtility_StringAppend(Buffer, Size, ",");
        }
#endif
        AmbaUtility_StringAppendUInt32(Buffer, Size, Movie->VideoTrack[i].Info.FrameCount, 10U);
    }
    if ((Movie->AudioTrackCount > 0U) || (Movie->TextTrackCount > 0U)) {
        AmbaUtility_StringAppend(Buffer, Size, "/");
    }
    for (i = 0U; i < Movie->AudioTrackCount; i++) {
#if (SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA > 1U)
        if (i > 0U) {
            AmbaUtility_StringAppend(Buffer, Size, ",");
        }
#endif
        AmbaUtility_StringAppendUInt32(Buffer, Size, Movie->AudioTrack[i].Info.FrameCount, 10U);
    }
    if (Movie->TextTrackCount > 0U) {
        AmbaUtility_StringAppend(Buffer, Size, "/");
    }
    for (i = 0U; i < Movie->TextTrackCount; i++) {
#if (SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA > 1U)
        if (i > 0U) {
            AmbaUtility_StringAppend(Buffer, Size, ",");
        }
#endif
        AmbaUtility_StringAppendUInt32(Buffer, Size, Movie->TextTrack[i].Info.FrameCount, 10U);
    }
    AmbaUtility_StringAppend(Buffer, Size, ")");
}

/**
 * Get string of Image information
 *
 * @param [in] Image Image information
 * @param [out] Buffer String buffer
 * @param [in] Size String size
 */
void SvcFormat_GetImageInfoString(const SVC_IMAGE_INFO_s *Image, char *Buffer, UINT32 Size)
{
    const char *Prefix = "Image(";
    AmbaUtility_StringAppend(Buffer, Size, Prefix);
    AmbaUtility_StringAppendUInt32(Buffer, Size, Image->FrameCount, 10U);
    AmbaUtility_StringAppend(Buffer, Size, ")");
}

/**
 * Get the default video track information
 *
 * @param [in] Tracks Video tracks
 * @param [in] TrackCount Number of video tracks
 * @param [out] TrackIdx The index of the returned default track
 * @return The default video track
 */
SVC_VIDEO_TRACK_INFO_s *SvcFormat_GetDefaultVideoTrack(SVC_VIDEO_TRACK_INFO_s *Tracks, UINT8 TrackCount, UINT8 *TrackIdx)
{
    UINT8 i;
    SVC_VIDEO_TRACK_INFO_s *Rval = NULL;
    for (i = 0U; i < TrackCount; i++) {
        if (Tracks[i].IsDefault == 1U) {
            Rval = &Tracks[i];
            *TrackIdx = i;
            break;
        }
    }
    if (Rval == NULL) {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s No default video track!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT8 SvcFormat_GetShortestVideoTrack(const SVC_VIDEO_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = SVC_FORMAT_MAX_TIMESTAMP;
    UINT8 i, Idx = TrackCount;
    // return the track with the min DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_VIDEO_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = p->Info.DTS;
        if (TmpDTS < DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != SVC_FORMAT_MAX_TIMESTAMP)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

static UINT8 SvcFormat_GetShortestAudioTrack(const SVC_AUDIO_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = SVC_FORMAT_MAX_TIMESTAMP;
    UINT8 i, Idx = TrackCount;
    // return the track with the min DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_AUDIO_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = p->Info.DTS;
        if (TmpDTS < DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != SVC_FORMAT_MAX_TIMESTAMP)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

static UINT8 SvcFormat_GetShortestTextTrack(const SVC_TEXT_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = SVC_FORMAT_MAX_TIMESTAMP;
    UINT8 i, Idx = TrackCount;
    // return the track with the min DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_TEXT_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = p->Info.DTS;
        if (TmpDTS < DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != SVC_FORMAT_MAX_TIMESTAMP)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

/**
 * Get the shortest track of the media
 *
 * @param [in] VideoTracks Video tracks
 * @param [in] VideoTrackCount Number of video tracks
 * @param [in] AudioTracks Audio tracks
 * @param [in] AudioTrackCount Number of audio tracks
 * @param [in] TextTracks Text tracks
 * @param [in] TextTrackCount Number of text tracks
 * @param [out] TrackIdx The index of the shortest track
 * @return The type of the shortest track
 */
UINT8 SvcFormat_GetShortestTrack(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx)
{
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
    UINT8 TrackType = 0U;
    if (VideoTracks != NULL) {
        UINT8 Idx = SvcFormat_GetShortestVideoTrack(VideoTracks, VideoTrackCount);
        if (Idx != VideoTrackCount) {
            Track = &VideoTracks[Idx].Info;
            TrackType = SVC_MEDIA_TRACK_TYPE_VIDEO;
            *TrackIdx = Idx;
        }
    }
    if (AudioTracks != NULL) {
        UINT8 Idx = SvcFormat_GetShortestAudioTrack(AudioTracks, AudioTrackCount);
        if (Idx != AudioTrackCount) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *AudioTrack = &AudioTracks[Idx].Info;
            if ((Track == NULL) || (AudioTrack->DTS < Track->DTS)) {
                Track = AudioTrack;
                TrackType = SVC_MEDIA_TRACK_TYPE_AUDIO;
                *TrackIdx = Idx;
            }
        }
    }
    if (TextTracks != NULL) {
        UINT8 Idx = SvcFormat_GetShortestTextTrack(TextTracks, TextTrackCount);
        if (Idx != TextTrackCount) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *TextTrack = &TextTracks[Idx].Info;
            if ((Track == NULL) || (TextTrack->DTS < Track->DTS)) {
                TrackType = SVC_MEDIA_TRACK_TYPE_TEXT;
                *TrackIdx = Idx;
            }
        }
    }
    return TrackType;
}

static UINT8 SvcFormat_GetLongestVideoTrack(const SVC_VIDEO_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = 0ULL;
    UINT8 i, Idx = TrackCount;
    // return the track with the max DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_VIDEO_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = (p->Info.DTS == p->Info.InitDTS) ? 0U : p->Info.DTS;
        if (TmpDTS > DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != 0U)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

static UINT8 SvcFormat_GetLongestAudioTrack(const SVC_AUDIO_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = 0ULL;
    UINT8 i, Idx = TrackCount;
    // return the track with the max DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_AUDIO_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = (p->Info.DTS == p->Info.InitDTS) ? 0U : p->Info.DTS;
        if (TmpDTS > DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != 0U)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

static UINT8 SvcFormat_GetLongestTextTrack(const SVC_TEXT_TRACK_INFO_s *Tracks, UINT8 TrackCount)
{
    UINT64 DTS = 0ULL;
    UINT8 i, Idx = TrackCount;
    // return the track with the max DTS
    for (i = 0U; i < TrackCount; i++) {
        const SVC_TEXT_TRACK_INFO_s *p = &Tracks[i];
        UINT64 TmpDTS = (p->Info.DTS == p->Info.InitDTS) ? 0U : p->Info.DTS;
        if (TmpDTS > DTS) {
            DTS = TmpDTS;
            Idx = i;
        } else {
            if ((TmpDTS == DTS) && (DTS != 0U)) {
                // default track has the highest priority
                if (p->IsDefault == 1U) {
                    Idx = i;
                }
            }
        }
    }
    return Idx;
}

/**
 * Get the shortest track of the media
 *
 * @param [in] VideoTracks Video tracks
 * @param [in] VideoTrackCount Number of video tracks
 * @param [in] AudioTracks Audio tracks
 * @param [in] AudioTrackCount Number of audio tracks
 * @param [in] TextTracks Text tracks
 * @param [in] TextTrackCount Number of text tracks
 * @param [out] TrackIdx The index of the longest track
 * @return The type of the Longest track
 */
UINT8 SvcFormat_GetLongestTrack(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx)
{
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
    UINT8 trackType = 0;
    if (VideoTracks != NULL) {
        UINT8 Idx = SvcFormat_GetLongestVideoTrack(VideoTracks, VideoTrackCount);
        if (Idx != VideoTrackCount) {
            Track = &VideoTracks[Idx].Info;
            trackType = SVC_MEDIA_TRACK_TYPE_VIDEO;
            *TrackIdx = Idx;
        }
    }
    if (AudioTracks != NULL) {
        UINT8 Idx = SvcFormat_GetLongestAudioTrack(AudioTracks, AudioTrackCount);
        if (Idx != AudioTrackCount) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *AudioTrack = &AudioTracks[Idx].Info;
            if ((Track == NULL) || (AudioTrack->DTS > Track->DTS)) {
                Track = AudioTrack;
                trackType = SVC_MEDIA_TRACK_TYPE_AUDIO;
                *TrackIdx = Idx;
            }
        }
    }
    if (TextTracks != NULL) {
        UINT8 Idx = SvcFormat_GetLongestTextTrack(TextTracks, TextTrackCount);
        if (Idx != TextTrackCount) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *TextTrack = &TextTracks[Idx].Info;
            if ((Track == NULL) || (TextTrack->DTS > Track->DTS)) {
                trackType = SVC_MEDIA_TRACK_TYPE_TEXT;
                *TrackIdx = Idx;
            }
         }
    }
    return trackType;
}

/**
 * Get AMBA box size
 *
 * @param [in] Media Media information
 * @return The returned AMBA box size
 */
INT32 SvcFormat_GetAmbaBoxSize(const SVC_MEDIA_INFO_s *Media)
{
    INT32 Rval = -1;
    UINT8 Type, MainVer, SubVer;
    /** Amba box size */
    SvcFormat_GetAmbaBoxVersion(Media, &Type, &MainVer, &SubVer);
    if (MainVer < 2U) {
        if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            if (MainVer == 0U) {
                const UINT8 AmbaBoxVer_0[2] = {36, 40};
                Rval = (INT32)AmbaBoxVer_0[SubVer] - 8;
            } else { /* MainVer == 1U */
                const UINT16 AmbaBoxVer_1[6] = {128, 128, 384, 384, 128, 128};
                Rval = (INT32)AmbaBoxVer_1[SubVer] - 8;
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s unsupported Media type", __func__, NULL, NULL, NULL, NULL);
        }
    } else if (MainVer == 2U) {
        if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            const SVC_MOVIE_INFO_s *Movie;
            INT32 Size = 4 + 1 + 1;   /* magic number + main version + subversion */
            UINT32 i;
            AmbaMisra_TypeCast(&Movie, &Media);
            Size += 4; /* Common Data's Size */
            Size += 1 + 1 + 1; /* track count */
            for (i = 0; i < Movie->VideoTrackCount; i++) {
                Size += 4; /* Track Data's Size */
                Size += ((2 * 4) + (2 * 8) + (4 * 4) + (2 * 2) + (4 * 1));
            }
            for (i = 0; i < Movie->AudioTrackCount; i++) {
                Size += 4; /* Track Data's Size */
                Size += ((2 * 4) + (2 * 8) + 4 + (2 * 1));
            }
            for (i = 0; i < Movie->TextTrackCount; i++) {
                Size += 4; /* Track Data's Size */
                Size += ((2 * 4) + (2 * 8) + 4 + 1);
            }
            Rval = Size;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s unsupported Media type", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            const SVC_MOVIE_INFO_s *Movie;
            INT32 Size = 4 + 1 + 1;   /* magic number + main version + subversion */
            UINT32 i;
            AmbaMisra_TypeCast(&Movie, &Media);
            Size += 4; /* Common Data's Size */
            Size += 1 + 1 + 1; /* track count */
            for (i = 0; i < Movie->VideoTrackCount; i++) {
                Size += 4; /* Track Data's Size */
                Size += ((2 * 4) + (2 * 8) + (2 * 4) + (2 * 2) + (2 * 1));
            }
            for (i = 0; i < Movie->AudioTrackCount; i++) {
                Size += 4; /* Track Data's Size */
                Size += ((2 * 4) + (2 * 8) + 4 + (2 * 1));
            }
            for (i = 0; i < Movie->TextTrackCount; i++) {
                Size += 4; /* Track Data's Size */
                Size += ((2 * 4) + (2 * 8) + 4 + 1);
            }
            Rval = Size;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s unsupported Media type", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 SvcFormat_PutVideo_AmbaBox3x(SVC_STREAM_HDLR_s *Stream, const SVC_VIDEO_TRACK_INFO_s *Video, UINT64 MinInitDTS)
{
    UINT32 Rval, DataSize;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    DataSize = SVC_AMBA_BOX_VIDEO_SIZE_V3;
    Rval = SvcFormat_PutBe32(Stream, DataSize);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe32(Stream, Track->OrigTimeScale);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutLe32(Stream, (UINT32)NORMALIZE_TO_TIMESCALE((UINT64)Track->TimeScale, (UINT64)Track->TimePerFrame, (UINT64)Track->OrigTimeScale));
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutLe64(Stream, NORMALIZE_TO_TIMESCALE((UINT64)Track->TimeScale, Track->InitDTS - MinInitDTS, Track->OrigTimeScale));
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutLe64(Stream, NORMALIZE_TO_TIMESCALE((UINT64)Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale));
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutLe32(Stream, Track->FrameCount);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutLe32(Stream, Video->GOPSize);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_PutLe16(Stream, Video->M);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_PutLe16(Stream, Video->N);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_PutByte(Stream, Video->Mode);
                                        if (Rval == FORMAT_OK) {
                                            Rval = SvcFormat_PutByte(Stream, Video->IsDefault);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcFormat_PutAudio_AmbaBox3x(SVC_STREAM_HDLR_s *Stream, const SVC_AUDIO_TRACK_INFO_s *Audio, UINT64 MinInitDTS)
{
    UINT32 Rval, DataSize;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
    DataSize = SVC_AMBA_BOX_AUDIO_SIZE;
    Rval = SvcFormat_PutBe32(Stream, DataSize);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe32(Stream, Track->OrigTimeScale);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutLe32(Stream, (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame , Track->OrigTimeScale));
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutLe64(Stream, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->InitDTS - MinInitDTS, Track->OrigTimeScale));
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutLe64(Stream, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale));
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutLe32(Stream, Track->FrameCount);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutByte(Stream, Audio->Endian);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_PutByte(Stream, Audio->IsDefault);
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcFormat_PutText_AmbaBox3x(SVC_STREAM_HDLR_s *Stream, const SVC_TEXT_TRACK_INFO_s *Text, UINT64 MinInitDTS)
{
    UINT32 Rval, DataSize;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
    DataSize = SVC_AMBA_BOX_TEXT_SIZE;
    Rval = SvcFormat_PutBe32(Stream, DataSize);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutLe32(Stream, Track->OrigTimeScale);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PutLe32(Stream, (UINT32)NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->TimePerFrame , Track->OrigTimeScale));
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutLe64(Stream, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->InitDTS - MinInitDTS, Track->OrigTimeScale));
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutLe64(Stream, NORMALIZE_TO_TIMESCALE(Track->TimeScale, Track->NextDTS - Track->InitDTS, Track->OrigTimeScale));
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_PutLe32(Stream, Track->FrameCount);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutByte(Stream, Text->IsDefault);
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT64 SvcFormat_GetMinInitDTS(const SVC_MOVIE_INFO_s *Movie)
{
    UINT8 i;
    UINT64 MinInitDTS = SVC_FORMAT_MAX_TIMESTAMP;
    for (i = 0; i < Movie->VideoTrackCount; i++) {   /* get min init DTS */
        const SVC_VIDEO_TRACK_INFO_s *Track;
        Track = &Movie->VideoTrack[i];
        if (Track->Info.InitDTS <= MinInitDTS) {
            MinInitDTS = Track->Info.InitDTS;
        }
    }
    for (i = 0; i < Movie->AudioTrackCount; i++) {   /* get min init DTS */
        const SVC_AUDIO_TRACK_INFO_s *Track;
        Track = &Movie->AudioTrack[i];
        if (Track->Info.InitDTS <= MinInitDTS) {
            MinInitDTS = Track->Info.InitDTS;
        }
    }
    for (i = 0; i < Movie->TextTrackCount; i++) {   /* get min init DTS */
        const SVC_TEXT_TRACK_INFO_s *Track;
        Track = &Movie->TextTrack[i];
        if (Track->Info.InitDTS <= MinInitDTS) {
            MinInitDTS = Track->Info.InitDTS;
        }
    }
    return MinInitDTS;
}

static UINT32 SvcFormat_PutAmbaBox3x(SVC_STREAM_HDLR_s *Stream, SVC_MOVIE_INFO_s *Movie, UINT8 SubVer)
{
    UINT32 Rval = FORMAT_OK;
    if (SubVer < 2U) {  /* v3.0, v3.1 */
        UINT8 Tmp;
        const SVC_VIDEO_TRACK_INFO_s * DefTrack = SvcFormat_GetDefaultVideoTrack(Movie->VideoTrack, Movie->VideoTrackCount, &Tmp);
        if (DefTrack != NULL) {
            Rval = SvcFormat_PutLe32(Stream, SVC_AMBA_BOX_MAGIC_NUM);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PutByte(Stream, 3U);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_PutByte(Stream, SubVer);
                    if (Rval == FORMAT_OK) {
                        UINT32 DataSize = 4U + 1U + 1U + 1U;
                        Rval = SvcFormat_PutBe32(Stream, DataSize);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_PutByte(Stream, Movie->VideoTrackCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_PutByte(Stream, Movie->AudioTrackCount);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_PutByte(Stream, Movie->TextTrackCount);
                                    if (Rval == FORMAT_OK) {
                                        UINT8 i;
                                        UINT64 MinInitDTS = SvcFormat_GetMinInitDTS(Movie);
                                        for (i = 0; i < Movie->VideoTrackCount; i++) {
                                            Rval = SvcFormat_PutVideo_AmbaBox3x(Stream, &Movie->VideoTrack[i], MinInitDTS);
                                            if (Rval != FORMAT_OK) {
                                                break;
                                            }
                                        }
                                        if (Rval == FORMAT_OK) {
                                            for (i = 0; i < Movie->AudioTrackCount; i++) {
                                                Rval = SvcFormat_PutAudio_AmbaBox3x(Stream, &Movie->AudioTrack[i], MinInitDTS);
                                                if (Rval != FORMAT_OK) {
                                                    break;
                                                }
                                            }
                                            if (Rval == FORMAT_OK) {
                                                for (i = 0; i < Movie->TextTrackCount; i++) {
                                                    Rval = SvcFormat_PutText_AmbaBox3x(Stream, &Movie->TextTrack[i], MinInitDTS);
                                                    if (Rval != FORMAT_OK) {
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s No default track!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong sub version!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

/**
 * Put AMBA box
 *
 * @param [in] Stream Stream information
 * @param [in] Media Media information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutAmbaBox(SVC_STREAM_HDLR_s *Stream, const SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval = FORMAT_OK;
    UINT8 Type, MainVer, SubVer;
    SvcFormat_GetAmbaBoxVersion(Media, &Type, &MainVer, &SubVer);
    if ((Type == SVC_AMBA_BOX_TYPE_MW) && (MainVer == 3U)) {
        if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
            SVC_MOVIE_INFO_s *Movie;
            AmbaMisra_TypeCast(&Movie, &Media);
            Rval = SvcFormat_PutAmbaBox3x(Stream, Movie, SubVer);
        } else {
            AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s unsupported", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_ARG;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong main version!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

static UINT32 SvcFormat_CheckAmbaBoxVersion(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, UINT8 Type, UINT8 *MainVer, UINT8 *SubVer)
{
    UINT32 Rval = FORMAT_OK;
    if (Type == SVC_AMBA_BOX_TYPE_MW) {
        UINT64 Pos;
        Rval = S2F(Stream->Func->GetPos(Stream, &Pos));
        if (Rval == FORMAT_OK) {
            UINT32 Tmp1;
            Rval = SvcFormat_GetLe32(Stream, &Tmp1);
            if (Rval == FORMAT_OK) {
                if (Tmp1 == SVC_AMBA_BOX_MAGIC_NUM) {
                    Rval = SvcFormat_GetByte(Stream, MainVer);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetByte(Stream, SubVer);
                    }
                } else {    /* ISO, Amba box 1.0 ~ 1.5 */
                    const UINT32 VerPos = BoxSize - SVC_ISO_BOX_SIZE_TAG_SIZE - 2U;
                    Rval = S2F(Stream->Func->Seek(Stream, ((INT64)Pos + (INT64)VerPos), SVC_STREAM_SEEK_START));
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetByte(Stream, MainVer);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetByte(Stream, SubVer);
                            if (Rval == FORMAT_OK) {
                                Rval = S2F(Stream->Func->Seek(Stream, (INT64)Pos, SVC_STREAM_SEEK_START));
                                if (Rval != FORMAT_OK) {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() fail!", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() fail!", __func__, NULL, NULL, NULL, NULL);
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        if (BoxSize == 24U) {
            *MainVer = 1U;
            *SubVer = 0U;
        } else {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Unsupported SVC AmbxBox Version! Size %u", BoxSize, 0, 0, 0, 0);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    if (Rval == FORMAT_OK) {
        if ((*MainVer > SVC_AMBA_BOX_MAX_MAIN_VER) || (*SubVer > SVC_AMBA_BOX_MAX_SUB_VER)) {
            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Wrong AmbxBox Version %u %u!", *MainVer, *SubVer, 0, 0, 0);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_ParseVideoTrack_AmbaBox1x(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval;
    SVC_AMBA_BOX_VIDEO_TRACK_INFO_s *Video = &AmbaboxInfo->VideoTrack[0];
    /* General AMBA Box Data = 28 Bytes*/
    Rval = SvcFormat_GetBe16(Stream, &Video->PixelArX);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetBe16(Stream, &Video->PixelArY);
        if (Rval == FORMAT_OK) {
            UINT8 Tmp8;
            Rval = SvcFormat_GetByte(Stream, &Tmp8);
            if (Rval == FORMAT_OK) {
                Video->M = Tmp8;
                Rval = SvcFormat_GetByte(Stream, &Tmp8);
                if (Rval == FORMAT_OK) {
                    Video->N = Tmp8;
                    Rval = SvcFormat_GetByte(Stream, &Video->ClosedGOP);
                    if (Rval == FORMAT_OK) {
                        UINT32 Tmp32;
                        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                        if (Rval == FORMAT_OK) {
                        /* To backward compatible with old muxer
                        In old muxer, p_movie->idr_interval = 0,
                        but GOP structure is 4/32/128, thus
                        we assign p_movie->idr_interval to be 4 */
                            if (Tmp32 == 0U) {
                                Tmp32 = 4U;
                            }
                            Video->GOPSize = Video->N * Tmp32;
                            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return Rval;
}

static UINT32 SvcFormat_ParseAmbaBox1x(SVC_STREAM_HDLR_s *Stream, UINT32 BoxSize, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval = FORMAT_OK;
    UINT64 Pos;
    Rval = S2F(Stream->Func->GetPos(Stream, &Pos));
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_ParseVideoTrack_AmbaBox1x(Stream, AmbaboxInfo);
        if (Rval == FORMAT_OK) {
            SVC_AMBA_BOX_VIDEO_TRACK_INFO_s * const Video = &AmbaboxInfo->VideoTrack[0];
            SVC_AMBA_BOX_AUDIO_TRACK_INFO_s * const Audio = &AmbaboxInfo->AudioTrack[0];
            UINT8 Tmp8;
            UINT32 Tmp32;
            UINT64 Tmp64;
            UINT32 i = 0U;
            for (i = 0U; i <= AmbaboxInfo->SubVer; i++) {
                if (i == 0U) {   /* Ver 1.0 */
                    ;
                } else if (i == 1U) {    /* Ver 1.1 */
                /* color domain */
                    Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                    if (Rval == FORMAT_OK) {
                        Video->ColorStyle = (UINT8)Tmp32;
                        Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                        }
                    }
                } else if (i == 2U) {    /* Ver 1.2 */
                    /* rotation */
                    Rval = SvcFormat_GetByte(Stream, &Tmp8);
                } else if (i == 3U) {    /* Ver 1.3 */
                    Rval = SvcFormat_GetByte(Stream, &Audio->Endian);
                } else if (i == 4U) {    /* Ver 1.4 */
                    /* p_movie->vin_aim_scene */
                    Rval = SvcFormat_GetBe32(Stream, &Tmp32);
                } else if (i == 5U) {    /* Ver 1.5 */
                    /* p_movie->trick_rec_pts_factor high fps */
                    Rval = SvcFormat_GetBe64(Stream, &Tmp64);
                    if (Rval == FORMAT_OK) {
                        Video->TrickRecNum = 1U; /* (UINT32)((double) Tmp64/0x1000000); */
                    }
                } else {
                //    LOG_ERR("incorrect SubVer");
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
                if (Rval != FORMAT_OK) {
                    break;
                }
            }
            if (Rval == FORMAT_OK) {
                Rval = S2F(Stream->Func->Seek(Stream, ((INT64)Pos + (INT64)BoxSize - 8), SVC_STREAM_SEEK_START));
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() fail!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 SvcFormat_AmbaBox2x_ParseVideoTrack(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_VIDEO_TRACK_INFO_s *Video)
{
    UINT32 Rval, DataSize;
    Rval = SvcFormat_GetBe32(Stream, &DataSize);
    if (Rval == FORMAT_OK) {
        if (DataSize == SVC_AMBA_BOX_VIDEO_SIZE_V2) {
            SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *Track = &Video->Info;
            Rval = SvcFormat_GetLe32(Stream, &Track->TimeScale);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetLe32(Stream, &Track->TimePerFrame);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetLe64(Stream, &Track->InitDTS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetLe64(Stream, &Track->Duration);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetLe32(Stream, &Track->FrameCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetLe32(Stream, &Video->GOPSize);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetLe32(Stream, &Video->TrickRecDen);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_GetLe32(Stream, &Video->TrickRecNum);
                                        if (Rval == FORMAT_OK) {
                                            Rval = SvcFormat_GetLe16(Stream, &Video->M);
                                            if (Rval == FORMAT_OK) {
                                                Rval = SvcFormat_GetLe16(Stream, &Video->N);
                                                if (Rval == FORMAT_OK) {
                                                    Rval = SvcFormat_GetByte(Stream, &Video->Mode);
                                                    if (Rval == FORMAT_OK) {
                                                        Rval = SvcFormat_GetByte(Stream, &Video->ColorStyle);
                                                        if (Rval == FORMAT_OK) {
                                                            Rval = SvcFormat_GetByte(Stream, &Video->IsVFR);
                                                            if (Rval == FORMAT_OK) {
                                                                if (Video->IsVFR == 1U) {
                                                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s VFR is not supported!", __func__, NULL, NULL, NULL, NULL);
                                                                    Rval = FORMAT_ERR_INVALID_FORMAT;
                                                                } else {
                                                                    Rval = SvcFormat_GetByte(Stream, &Video->IsDefault);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect data size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_AmbaBox2x_ParseAudioTrack(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_AUDIO_TRACK_INFO_s * Audio)
{
    UINT32 Rval, DataSize;
    Rval = SvcFormat_GetBe32(Stream, &DataSize);
    if (Rval == FORMAT_OK) {
        if (DataSize == SVC_AMBA_BOX_AUDIO_SIZE) {
            SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
            Rval = SvcFormat_GetLe32(Stream, &Track->TimeScale);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetLe32(Stream, &Track->TimePerFrame);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetLe64(Stream, &Track->InitDTS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetLe64(Stream, &Track->Duration);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetLe32(Stream, &Track->FrameCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetByte(Stream, &Audio->Endian);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetByte(Stream, &Audio->IsDefault);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect data size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_AmbaBox2x_ParseTextTrack(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_TEXT_TRACK_INFO_s * Text)
{
    UINT32 Rval, DataSize;
    Rval = SvcFormat_GetBe32(Stream, &DataSize);
    if (Rval == FORMAT_OK) {
        if (DataSize == SVC_AMBA_BOX_TEXT_SIZE) {
            SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *Track = &Text->Info;
            Rval = SvcFormat_GetLe32(Stream, &Track->TimeScale);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetLe32(Stream, &Track->TimePerFrame);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetLe64(Stream, &Track->InitDTS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetLe64(Stream, &Track->Duration);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetLe32(Stream, &Track->FrameCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetByte(Stream, &Text->IsDefault);
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect data size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_ParseAmbaBox2x_Ver6(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval;
    UINT8 TrackCount;
    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "SDK6 clip", NULL, NULL, NULL, NULL, NULL);
    Rval = SvcFormat_GetByte(Stream, &TrackCount);
    if (Rval == FORMAT_OK) {
        UINT8 i, VideoId = 0, AudioId = 0, TextId = 0;
        for (i = 0; i < TrackCount; i++) {
            UINT8 TrackType;
            Rval = SvcFormat_GetByte(Stream, &TrackType);
            if (Rval == FORMAT_OK) {
                switch (TrackType) {
                case SVC_MEDIA_TRACK_TYPE_VIDEO:
                    Rval = SvcFormat_AmbaBox2x_ParseVideoTrack(Stream, &AmbaboxInfo->VideoTrack[VideoId]);
                    VideoId++;
                    break;
                case SVC_MEDIA_TRACK_TYPE_AUDIO:
                    Rval = SvcFormat_AmbaBox2x_ParseAudioTrack(Stream, &AmbaboxInfo->AudioTrack[AudioId]);
                    AudioId++;
                    break;
                case SVC_MEDIA_TRACK_TYPE_TEXT:
                    Rval = SvcFormat_AmbaBox2x_ParseTextTrack(Stream, &AmbaboxInfo->TextTrack[TextId]);
                    TextId++;
                    break;
                default:
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect track type!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                    break;
                }
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            AmbaboxInfo->VideoTrackCount = VideoId;
            AmbaboxInfo->AudioTrackCount = AudioId;
            AmbaboxInfo->TextTrackCount = TextId;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_ParseAmbaBox2x_Ver7(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval = SvcFormat_GetByte(Stream, &AmbaboxInfo->VideoTrackCount);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_GetByte(Stream, &AmbaboxInfo->AudioTrackCount);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetByte(Stream, &AmbaboxInfo->TextTrackCount);
            if (Rval == FORMAT_OK) {
#if 0
                Rval = S2F(Stream->Func->Seek(Stream, ((INT64)Pos + (INT64)DataSize), SVC_STREAM_SEEK_START));
                if (Rval == FORMAT_OK) {
#endif
                    UINT8 i;
                    for (i = 0; i < AmbaboxInfo->VideoTrackCount; i++) {
                        Rval = SvcFormat_AmbaBox2x_ParseVideoTrack(Stream, &AmbaboxInfo->VideoTrack[i]);
                        if (Rval != FORMAT_OK) {
                            break;
                        }
                    }
                    if (Rval == FORMAT_OK) {
                        for (i = 0; i < AmbaboxInfo->AudioTrackCount; i++) {
                            Rval = SvcFormat_AmbaBox2x_ParseAudioTrack(Stream, &AmbaboxInfo->AudioTrack[i]);
                            if (Rval != FORMAT_OK) {
                                break;
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            for (i = 0; i < AmbaboxInfo->TextTrackCount; i++) {
                                Rval = SvcFormat_AmbaBox2x_ParseTextTrack(Stream, &AmbaboxInfo->TextTrack[i]);
                                if (Rval != FORMAT_OK) {
                                    break;
                                }
                            }
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_AmbaBox2x_ParseTextTrack fail!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_AmbaBox2x_ParseAudioTrack fail!", __func__, NULL, NULL, NULL, NULL);
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_AmbaBox2x_ParseVideoTrack fail!", __func__, NULL, NULL, NULL, NULL);
                    }
#if 0
                }
#endif
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->Seek() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

static UINT32 SvcFormat_ParseAmbaBox2x(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval = FORMAT_OK;
    if (AmbaboxInfo->SubVer == 0U) {
        UINT64 Pos;
        Rval = S2F(Stream->Func->GetPos(Stream, &Pos));
        if (Rval == FORMAT_OK) {
            UINT32 DataSize;
            Rval = SvcFormat_GetBe32(Stream, &DataSize);
            if (Rval == FORMAT_OK) {
                // use this byte to distinguish SDK6/SDK7
                if (DataSize == 7U) {
                    // SDK7
                    Rval = SvcFormat_ParseAmbaBox2x_Ver7(Stream, AmbaboxInfo);
                } else if (DataSize == 5U) {
                    // SDK6
                    Rval = SvcFormat_ParseAmbaBox2x_Ver6(Stream, AmbaboxInfo);
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect data size!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong sub version!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 SvcFormat_AmbaBox3x_ParseVideoTrack(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_VIDEO_TRACK_INFO_s *Video)
{
    UINT32 Rval, DataSize;
    Rval = SvcFormat_GetBe32(Stream, &DataSize);
    if (Rval == FORMAT_OK) {
        if (DataSize == SVC_AMBA_BOX_VIDEO_SIZE_V3) {
            SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *Track = &Video->Info;
            Rval = SvcFormat_GetLe32(Stream, &Track->TimeScale);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetLe32(Stream, &Track->TimePerFrame);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetLe64(Stream, &Track->InitDTS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetLe64(Stream, &Track->Duration);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetLe32(Stream, &Track->FrameCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetLe32(Stream, &Video->GOPSize);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetLe16(Stream, &Video->M);
                                    if (Rval == FORMAT_OK) {
                                        Rval = SvcFormat_GetLe16(Stream, &Video->N);
                                        if (Rval == FORMAT_OK) {
                                            Rval = SvcFormat_GetByte(Stream, &Video->Mode);
                                            if (Rval == FORMAT_OK) {
                                                Rval = SvcFormat_GetByte(Stream, &Video->IsDefault);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect data size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_AmbaBox3x_ParseAudioTrack(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_AUDIO_TRACK_INFO_s * Audio)
{
    UINT32 Rval, DataSize;
    Rval = SvcFormat_GetBe32(Stream, &DataSize);
    if (Rval == FORMAT_OK) {
        if (DataSize == SVC_AMBA_BOX_AUDIO_SIZE) {
            SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
            Rval = SvcFormat_GetLe32(Stream, &Track->TimeScale);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetLe32(Stream, &Track->TimePerFrame);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetLe64(Stream, &Track->InitDTS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetLe64(Stream, &Track->Duration);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetLe32(Stream, &Track->FrameCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetByte(Stream, &Audio->Endian);
                                if (Rval == FORMAT_OK) {
                                    Rval = SvcFormat_GetByte(Stream, &Audio->IsDefault);
                                }
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect data size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_AmbaBox3x_ParseTextTrack(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_TEXT_TRACK_INFO_s * Text)
{
    UINT32 Rval, DataSize;
    Rval = SvcFormat_GetBe32(Stream, &DataSize);
    if (Rval == FORMAT_OK) {
        if (DataSize == SVC_AMBA_BOX_TEXT_SIZE) {
            SVC_AMBA_BOX_TRACK_GENERAL_INFO_s *Track = &Text->Info;
            Rval = SvcFormat_GetLe32(Stream, &Track->TimeScale);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetLe32(Stream, &Track->TimePerFrame);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetLe64(Stream, &Track->InitDTS);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetLe64(Stream, &Track->Duration);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetLe32(Stream, &Track->FrameCount);
                            if (Rval == FORMAT_OK) {
                                Rval = SvcFormat_GetByte(Stream, &Text->IsDefault);
                            }
                        }
                    }
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Incorrect data size!", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_INVALID_FORMAT;
        }
    }
    return Rval;
}

static UINT32 SvcFormat_ParseAmbaBox3x(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval = FORMAT_OK;
    if (AmbaboxInfo->SubVer < 2U) {  /* v3.0, v3.1 */
        UINT64 Pos;
        Rval = S2F(Stream->Func->GetPos(Stream, &Pos));
        if (Rval == FORMAT_OK) {
            UINT32 DataSize;
            Rval = SvcFormat_GetBe32(Stream, &DataSize);
            if (Rval == FORMAT_OK) {
                if (DataSize == 7U) {
                    Rval = SvcFormat_GetByte(Stream, &AmbaboxInfo->VideoTrackCount);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_GetByte(Stream, &AmbaboxInfo->AudioTrackCount);
                        if (Rval == FORMAT_OK) {
                            Rval = SvcFormat_GetByte(Stream, &AmbaboxInfo->TextTrackCount);
                            if (Rval == FORMAT_OK) {
                                UINT8 i;
                                for (i = 0; i < AmbaboxInfo->VideoTrackCount; i++) {
                                    Rval = SvcFormat_AmbaBox3x_ParseVideoTrack(Stream, &AmbaboxInfo->VideoTrack[i]);
                                    if (Rval != FORMAT_OK) {
                                        break;
                                    }
                                }
                                if (Rval == FORMAT_OK) {
                                    for (i = 0; i < AmbaboxInfo->AudioTrackCount; i++) {
                                        Rval = SvcFormat_AmbaBox3x_ParseAudioTrack(Stream, &AmbaboxInfo->AudioTrack[i]);
                                        if (Rval != FORMAT_OK) {
                                            break;
                                        }
                                    }
                                    if (Rval == FORMAT_OK) {
                                        for (i = 0; i < AmbaboxInfo->TextTrackCount; i++) {
                                            Rval = SvcFormat_AmbaBox3x_ParseTextTrack(Stream, &AmbaboxInfo->TextTrack[i]);
                                            if (Rval != FORMAT_OK) {
                                                break;
                                            }
                                        }
                                        if (Rval != FORMAT_OK) {
                                            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_AmbaBox3x_ParseTextTrack fail!", __func__, NULL, NULL, NULL, NULL);
                                        }
                                    } else {
                                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_AmbaBox3x_ParseAudioTrack fail!", __func__, NULL, NULL, NULL, NULL);
                                    }
                                } else {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_AmbaBox3x_ParseVideoTrack fail!", __func__, NULL, NULL, NULL, NULL);
                                }
                            }
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s incorrect data size!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Stream->Func->GetPos() fail!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong sub version!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

static UINT32 SvcFormat_ParseSvcAmbaBox(SVC_STREAM_HDLR_s *Stream, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval, TmpU32;
    Rval = SvcFormat_GetBe32(Stream, &TmpU32);
    if (Rval == FORMAT_OK) {
        SVC_AMBA_BOX_VIDEO_TRACK_INFO_s *Video = &AmbaboxInfo->VideoTrack[0];
        Video->M = (UINT16)TmpU32;
        Rval = SvcFormat_GetBe32(Stream, &TmpU32);
        if (Rval == FORMAT_OK) {
            UINT32 IdrInterval;
            Video->N = (UINT16)TmpU32;
            Rval = SvcFormat_GetBe32(Stream, &IdrInterval);
            if (Rval == FORMAT_OK) {
                UINT16 TmpU16;
                Video->GOPSize = IdrInterval * (UINT32)Video->N;
                Rval = SvcFormat_GetBe16(Stream, &TmpU16);/* IRCycle */
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_GetBe16(Stream, &TmpU16);/* FirstGOPStartB */
                    if ((Video->M > 1U) && (TmpU16 == 0U)) {
                        Video->ClosedGOP = 1U;
                    }
                }
            }
        }
    }
    return Rval;
}

/**
 * Parse AMBA box
 *
 * @param [in] Stream Stream information
 * @param [in] MediaType Media type
 * @param [in] BoxSize Box size
 * @param [out] AmbaboxInfo AMBA box information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_ParseAmbaBox(SVC_STREAM_HDLR_s *Stream, UINT8 MediaType, UINT32 BoxSize, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval = SvcFormat_CheckAmbaBoxVersion(Stream, BoxSize, AmbaboxInfo->Type, &AmbaboxInfo->MainVer, &AmbaboxInfo->SubVer);
    if (Rval == FORMAT_OK) {
        if (AmbaboxInfo->Type == SVC_AMBA_BOX_TYPE_MW) {
            if (AmbaboxInfo->MainVer == 1U) {
                if (MediaType == SVC_MEDIA_INFO_MOVIE) {
                    Rval = SvcFormat_ParseAmbaBox1x(Stream, BoxSize, AmbaboxInfo);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcFormat_ParseAmbaBox1x failed", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ParseAmbaBox1x unsupported media", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            } else if (AmbaboxInfo->MainVer == 2U) {
                if (MediaType == SVC_MEDIA_INFO_MOVIE) {
                    Rval = SvcFormat_ParseAmbaBox2x(Stream, AmbaboxInfo);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcFormat_ParseAmbaBox2x failed", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ParseAmbaBox2x unsupported media", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            } else if (AmbaboxInfo->MainVer == 3U) {
                if (MediaType == SVC_MEDIA_INFO_MOVIE) {
                    Rval = SvcFormat_ParseAmbaBox3x(Stream, AmbaboxInfo);
                    if (Rval != FORMAT_OK) {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcFormat_ParseAmbaBox3x failed", __func__, NULL, NULL, NULL, NULL);
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_ParseAmbaBox3x unsupported media", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s unsupported version", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_INVALID_FORMAT;
            }
        } else {
            Rval = SvcFormat_ParseSvcAmbaBox(Stream, AmbaboxInfo);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "SvcFormat_ParseSvcAmbaBox failed", __func__, NULL, NULL, NULL, NULL);
            }
        }
    }
    return Rval;
}

static UINT32 SvcFormat_GetVideoTrackInfo(SVC_VIDEO_TRACK_INFO_s *VideoTrack, const SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo, UINT8 TrackCount)
{
    UINT32 Rval = FORMAT_OK;
    if (AmbaboxInfo->Type == SVC_AMBA_BOX_TYPE_MW) {
        UINT8 i;
        for (i = 0; i < TrackCount; i++) {
            if (AmbaboxInfo->MainVer >= 2U) {
                if ((VideoTrack[i].Info.TimePerFrame % AmbaboxInfo->VideoTrack[i].Info.TimePerFrame) == 0U) {
                    /* TimePerFrame & VFR parameters should not be used in demuxer & editor. */
                    VideoTrack[i].Info.TimePerFrame = AmbaboxInfo->VideoTrack[i].Info.TimePerFrame; // use original TimePerFrame.
                    if (AmbaboxInfo->VideoTrack[i].Info.TimeScale == VideoTrack[i].Info.TimeScale) {
                        // 0 is for FMP4
                        if (VideoTrack[i].Info.NextDTS != 0ULL) {
                            //check duration: NextDTS, InitDts is inaccurate after normalized, tolerance time = 1/2 frame
                            if ((AmbaboxInfo->VideoTrack[i].Info.Duration - (VideoTrack[i].Info.NextDTS - VideoTrack[i].Info.InitDTS)) > ((UINT64)AmbaboxInfo->VideoTrack[i].Info.TimePerFrame >> 1)) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track duration different!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            }
                            if (Rval == FORMAT_OK) {
                                if (AmbaboxInfo->VideoTrack[i].Info.FrameCount != VideoTrack[i].Info.FrameCount) {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track FrameCount different!", __func__, NULL, NULL, NULL, NULL);
                                    Rval = FORMAT_ERR_GENERAL_ERROR;
                                }
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track TimeScale different!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track TimePerFrame different!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            }

            if (Rval == FORMAT_OK) {
                VideoTrack[i].GOPSize = AmbaboxInfo->VideoTrack[i].GOPSize;
                VideoTrack[i].M = AmbaboxInfo->VideoTrack[i].M;
                VideoTrack[i].N = AmbaboxInfo->VideoTrack[i].N;
                VideoTrack[i].Mode = AmbaboxInfo->VideoTrack[i].Mode;
                VideoTrack[i].IsDefault = AmbaboxInfo->VideoTrack[i].IsDefault;
                if (AmbaboxInfo->MainVer < 2U) {
                    VideoTrack[i].PixelArX = AmbaboxInfo->VideoTrack[i].PixelArX;
                    VideoTrack[i].PixelArY = AmbaboxInfo->VideoTrack[i].PixelArY;
                    VideoTrack[i].IsDefault = 1U;
                }
                #if 0
                LOG_VERB("TimePerFrame = %u", VideoTrack[i].Info.TimePerFrame);
                LOG_VERB("PixelArX = %u", VideoTrack[i].PixelArX);
                LOG_VERB("PixelArY = %u", VideoTrack[i].PixelArY);
                LOG_VERB("M = %u", VideoTrack[i].M);
                LOG_VERB("N = %u", VideoTrack[i].N);
                LOG_VERB("IsDefault = %u", VideoTrack[i].IsDefault);
                #endif
            } else {
                break;
            }
        }
    } else {
        VideoTrack[0].GOPSize = AmbaboxInfo->VideoTrack[0].GOPSize;
        VideoTrack[0].M = AmbaboxInfo->VideoTrack[0].M;
        VideoTrack[0].N = AmbaboxInfo->VideoTrack[0].N;
        VideoTrack[0].IsDefault = 1U;
    }
    return Rval;
}

static UINT32 SvcFormat_GetAudioTrackInfo(SVC_AUDIO_TRACK_INFO_s *AudioTrack, const SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo, UINT8 TrackCount)
{
    UINT32 Rval = FORMAT_OK;
    if (AmbaboxInfo->Type == SVC_AMBA_BOX_TYPE_MW) {
        UINT8 i;
        for (i = 0; i < TrackCount; i++) {
            if (AmbaboxInfo->MainVer >= 2U) {
                if (AmbaboxInfo->AudioTrack[i].Info.TimePerFrame == AudioTrack[i].Info.TimePerFrame) {
                    if (AmbaboxInfo->AudioTrack[i].Info.TimeScale == AudioTrack[i].Info.TimeScale) {
                        // 0 is for FMP4
                        if (AudioTrack[i].Info.NextDTS != 0ULL) {
                            //check duration: NextDTS, InitDts is inaccurate after normalized, tolerance time = 1/2 frame
                            if ((AmbaboxInfo->AudioTrack[i].Info.Duration - (AudioTrack[i].Info.NextDTS - AudioTrack[i].Info.InitDTS)) > ((UINT64)AmbaboxInfo->AudioTrack[i].Info.TimePerFrame >> 1)) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track duration different!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            } else {
                                if (AmbaboxInfo->AudioTrack[i].Info.FrameCount != AudioTrack[i].Info.FrameCount) {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track FrameCount different!", __func__, NULL, NULL, NULL, NULL);
                                    Rval = FORMAT_ERR_GENERAL_ERROR;
                                }
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            AudioTrack[i].Endian = AmbaboxInfo->AudioTrack[i].Endian;
                            AudioTrack[i].IsDefault = AmbaboxInfo->AudioTrack[i].IsDefault;
                            if (AmbaboxInfo->MainVer < 2U) {
                                AudioTrack[i].IsDefault = 1U;
                            }
#if 0
                            LOG_VERB("Endian = %u", AudioTrack[i].Endian);
                            LOG_VERB("IsDefault = %u", AudioTrack[i].IsDefault);
#endif
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track TimeScale different!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track TimePerFrame different!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    if (AudioTrack != NULL) {
        AmbaMisra_TouchUnused(AudioTrack);
    }
    return Rval;
}

static UINT32 SvcFormat_GetTextTrackInfo(SVC_TEXT_TRACK_INFO_s *TextTrack, const SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo, UINT8 TrackCount)
{
    UINT32 Rval = FORMAT_OK;
    if (AmbaboxInfo->Type == SVC_AMBA_BOX_TYPE_MW) {
        UINT8 i;
        for (i = 0; i < TrackCount; i++) {
            if (AmbaboxInfo->MainVer >= 2U) {
                if (AmbaboxInfo->TextTrack[i].Info.TimePerFrame == TextTrack[i].Info.TimePerFrame) {
                    if (AmbaboxInfo->TextTrack[i].Info.TimeScale == TextTrack[i].Info.TimeScale) {
                        // 0 is for FMP4
                        if (TextTrack[i].Info.NextDTS != 0ULL) {
                            //check duration: NextDTS, InitDts is inaccurate after normalized, tolerance time = 1/2 frame
                            if ((AmbaboxInfo->TextTrack[i].Info.Duration - (TextTrack[i].Info.NextDTS - TextTrack[i].Info.InitDTS)) > ((UINT64)AmbaboxInfo->TextTrack[i].Info.TimePerFrame >> 1)) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track duration different!", __func__, NULL, NULL, NULL, NULL);
                                Rval = FORMAT_ERR_GENERAL_ERROR;
                            } else {
                                if (AmbaboxInfo->TextTrack[i].Info.FrameCount != TextTrack[i].Info.FrameCount) {
                                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track FrameCount different!", __func__, NULL, NULL, NULL, NULL);
                                    Rval = FORMAT_ERR_GENERAL_ERROR;
                                }
                            }
                        }
                        if (Rval == FORMAT_OK) {
                            TextTrack[i].IsDefault = AmbaboxInfo->TextTrack[i].IsDefault;
                            if(AmbaboxInfo->MainVer < 2U) {
                                TextTrack[i].IsDefault = 1U;
                            }
                        }
                    } else {
                        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track TimeScale different!", __func__, NULL, NULL, NULL, NULL);
                        Rval = FORMAT_ERR_GENERAL_ERROR;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Track TimePerFrame different!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_GENERAL_ERROR;
                }
            }
            if (Rval != FORMAT_OK) {
                break;
            }
        }
    }
    if (TextTrack != NULL) {
        AmbaMisra_TouchUnused(TextTrack);
    }
    return Rval;
}

//#define DBGMSG
/**
 * Get media information
 *
 * @param [out] Media Media information
 * @param [in] AmbaboxInfo AMBA box information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetMediaInfo(SVC_MEDIA_INFO_s *Media, const SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo)
{
    UINT32 Rval;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = SvcFormat_GetVideoTrackInfo(Movie->VideoTrack, AmbaboxInfo, Movie->VideoTrackCount);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_GetAudioTrackInfo(Movie->AudioTrack, AmbaboxInfo, Movie->AudioTrackCount);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_GetTextTrackInfo(Movie->TextTrack, AmbaboxInfo, Movie->TextTrackCount);
            }
        }
        // trick: put AmbaBox version to SubFormat
        if (Rval == FORMAT_OK) {
            Media->SubFormat = AmbaboxInfo->Type << 6U;
            Media->SubFormat |= (AmbaboxInfo->MainVer << 3U);
            Media->SubFormat |= AmbaboxInfo->SubVer;
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Unsupported media type!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    return Rval;
}

/**
 * Get the current media time
 *
 * @param [in] VideoTracks Video tracks
 * @param [in] VideoTrackCount Number of video tracks
 * @param [in] AudioTracks Audio tracks
 * @param [in] AudioTrackCount Number of audio tracks
 * @param [in] TextTracks Text tracks
 * @param [in] TextTrackCount Number of text tracks
 * @param [in] Direction Deirection
 * @return The current media time
 */
UINT64 SvcFormat_GetMediaTime(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 Direction)
{
    UINT64 Time = 0;
    if (Direction == SVC_FORMAT_DIR_FORWARD) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
        UINT8 TrackType, TrackIdx;
        // forward, use shortest
        TrackType = SvcFormat_GetShortestTrack(VideoTracks, VideoTrackCount, AudioTracks, AudioTrackCount, TextTracks, TextTrackCount, &TrackIdx);
        switch(TrackType) {
        case SVC_MEDIA_TRACK_TYPE_VIDEO:
            Track = &VideoTracks[TrackIdx].Info;
            break;
        case SVC_MEDIA_TRACK_TYPE_AUDIO:
            Track = &AudioTracks[TrackIdx].Info;
            break;
        case SVC_MEDIA_TRACK_TYPE_TEXT:
            Track = &TextTracks[TrackIdx].Info;
            break;
        default:
            /**/
            break;
        }

        if (Track == NULL) {
            Time = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            Time = SVC_FORMAT_DTS_TO_TIME(Track->DTS, Track->TimeScale);
        }
    } else {
        if (Direction == SVC_FORMAT_DIR_BACKWARD) {
            const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = NULL;
            UINT8 TrackType, TrackIdx;
            // forward, use shortest
            TrackType = SvcFormat_GetLongestTrack(VideoTracks, VideoTrackCount, AudioTracks, AudioTrackCount, TextTracks, TextTrackCount, &TrackIdx);
            switch(TrackType) {
            case SVC_MEDIA_TRACK_TYPE_VIDEO:
                Track = &VideoTracks[TrackIdx].Info;
                break;
            case SVC_MEDIA_TRACK_TYPE_AUDIO:
                Track = &AudioTracks[TrackIdx].Info;
                break;
            case SVC_MEDIA_TRACK_TYPE_TEXT:
                Track = &TextTracks[TrackIdx].Info;
                break;
            default:
                /**/
                break;
            }

            if (Track == NULL) {
                Time = 0;
            } else {
                Time = SVC_FORMAT_DTS_TO_TIME_CEIL(Track->DTS, Track->TimeScale);
            }

        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Wrong directio!", __func__, NULL, NULL, NULL, NULL);
        }
    }

    return Time;
}

/**
 * Put end marker
 *
 * @param [in] Fifo Fifo handler
 * @param [in] PTS Current PTS
 * @param [in] TimeScale Time scale
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutEndMark(SVC_FIFO_HDLR_s *Fifo, UINT64 PTS, UINT32 TimeScale)
{
    UINT32 Rval = FORMAT_OK;
    SVC_FIFO_BITS_DESC_s Frame = {0};
    while (Rval == FORMAT_OK) {
        Rval = F2F(SvcFIFO_PrepareEntry(Fifo, &Frame));
        if (Rval == FORMAT_OK) {
            break;
        }
        if (Rval == FORMAT_ERR_FIFO_FULL) {
            Rval = K2F(AmbaKAL_TaskSleep(10));
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s AmbaKAL_TaskSleep() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFIFO_PrepareEntry() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Frame.Size = SVC_FIFO_MARK_EOS;
        Frame.FrameType = SVC_FIFO_TYPE_EOS;
        Frame.Pts = NORMALIZE_CLOCK(PTS, TimeScale);
        Rval = F2F(SvcFIFO_WriteEntry(Fifo, &Frame, 0U));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFIFO_WriteEntry() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Fifo);
    return Rval;
}

/**
 * Put decode marker
 *
 * @param [in] Fifo Fifo handler
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutDecodeMark(SVC_FIFO_HDLR_s *Fifo)
{
    UINT32 Rval = FORMAT_OK;
    SVC_FIFO_BITS_DESC_s Frame = {0};
    while (Rval == FORMAT_OK) {
        Rval = F2F(SvcFIFO_PrepareEntry(Fifo, &Frame));
        if (Rval == FORMAT_OK) {
            break;
        }
        if (Rval == FORMAT_ERR_FIFO_FULL) {
            Rval = K2F(AmbaKAL_TaskSleep(10));
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s AmbaKAL_TaskSleep() error!", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFIFO_PrepareEntry() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        Frame.Size = 0;
        Frame.FrameType = SVC_FIFO_TYPE_DECODE_MARK;
        Rval = F2F(SvcFIFO_WriteEntry(Fifo, &Frame, 0U));
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFIFO_WriteEntry() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Fifo);
    return Rval;
}

static UINT32 SvcFormat_PutSPSPPS(const SVC_FEED_FRAME_PARAM_s *H264, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset)
{
    UINT32 Rval;
    const UINT8 Start[SVC_H264_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    Rval = SvcFormat_ReadStreamMem(Start, SVC_H264_STRAT_CODE_SIZE, BufferOffset, BufferBase, BufferSize, NewBufferOffset);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_ReadStreamMem(H264->SPS, H264->SPSLen, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_ReadStreamMem(Start, SVC_H264_STRAT_CODE_SIZE, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_ReadStreamMem(H264->PPS, H264->PPSLen, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
            }
        }
    }
    return Rval;
}

static UINT32 SvcFormat_PutVPSSPSPPS(const SVC_FEED_FRAME_PARAM_s *H265, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset)
{
    UINT32 Rval;
    const UINT8 Start[SVC_H265_STRAT_CODE_SIZE] = {0, 0, 0, 1};
    Rval = SvcFormat_ReadStreamMem(Start, SVC_H265_STRAT_CODE_SIZE, BufferOffset, BufferBase, BufferSize, NewBufferOffset);
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_ReadStreamMem(H265->VPS, H265->VPSLen, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_ReadStreamMem(Start, SVC_H265_STRAT_CODE_SIZE, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_ReadStreamMem(H265->SPS, H265->SPSLen, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
                if (Rval == FORMAT_OK) {
                    Rval = SvcFormat_ReadStreamMem(Start, SVC_H265_STRAT_CODE_SIZE, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_ReadStreamMem(H265->PPS, H265->PPSLen, *NewBufferOffset, BufferBase, BufferSize, NewBufferOffset);
                    }
                }
            }
        }
    }
    return Rval;
}

/**
 * Prepare a frame
 *
 * @param [in] Fifo Fifo handler
 * @param [out] Frame Frame descriptor
 * @param [in] Size Frame size
 * @param [in] Type Frame type
 * @param [in] PTS Frame PTS
 * @param [in] TimeScale Time scale
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PrepareFrame(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *Frame, UINT32 Size, UINT8 Type, UINT64 PTS, UINT32 TimeScale)
{
    UINT32 Rval = F2F(SvcFIFO_PrepareEntry(Fifo, Frame));
    if (Rval == FORMAT_OK) {
        if (Frame->Size >= Size) {
            Frame->FrameType = Type;
            Frame->Pts = NORMALIZE_CLOCK(PTS, TimeScale);
        } else {
            Rval = FORMAT_ERR_FIFO_FULL;
        }
    } else {
        if (Rval != FORMAT_ERR_FIFO_FULL) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFIFO_PrepareEntry() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    return Rval;
}

/**
 * Write a frame
 *
 * @param [in] Fifo Fifo handler
 * @param [in] Frame Frame descriptor
 * @param [in] Size Frame size
 * @param [in] Type Frame type
 * @param [in] PTS Frame PTS
 * @param [in] TimeScale Time scale
 * @param [in] Completed Completed flag of the frame
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_WriteFrame(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *Frame, UINT32 Size, UINT8 Type, UINT64 PTS, UINT32 TimeScale, UINT8 Completed)
{
    UINT32 Rval;
    Frame->Size = Size;
    Frame->FrameType = Type;
    Frame->Pts = NORMALIZE_CLOCK(PTS, TimeScale);
    Frame->Align = 0;
    Frame->Completed = Completed;
    Rval = F2F(SvcFIFO_WriteEntry(Fifo, Frame, 0U));
    if (Rval != FORMAT_OK) {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFIFO_WriteEntry() error!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

static UINT32 GetOffset(const UINT8 *Base, const UINT8 *Addr)
{
    ULONG AddrUL, BaseUL, OffsetUL;
    AmbaMisra_TypeCast(&AddrUL, &Addr);
    AmbaMisra_TypeCast(&BaseUL, &Base);
    OffsetUL = AddrUL - BaseUL;
    return (UINT32)OffsetUL;
}

static UINT32 SvcFormat_PutKeyFrameHeader(const SVC_VIDEO_TRACK_INFO_s *Video, const SVC_FEED_FRAME_PARAM_s *FeedParam, UINT64 Pts, UINT32 TimePerFrame, UINT32 *BufferOffset)
{
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Video->Info;
    UINT32 CurrPos = *BufferOffset;
    UINT32 Rval;
    if ((Track->MediaId == SVC_FORMAT_MID_AVC) || (Track->MediaId == SVC_FORMAT_MID_H264)) {
        /* Put GOP Header */
        Rval = SvcH264_PutGOPHeader(CurrPos, Track->BufferBase, Track->BufferSize, Video, 0U, 0U, Pts, TimePerFrame, &CurrPos);
        if (Rval == FORMAT_OK) {
            if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                /* Put sps, pps */
                Rval = SvcFormat_PutSPSPPS(FeedParam, CurrPos, Track->BufferBase, Track->BufferSize, &CurrPos);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutSPSPPS() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcH265_PutGOPHeader() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        /* Put GOP Header */
        Rval = SvcH265_PutGOPHeader(CurrPos, Track->BufferBase, Track->BufferSize, Video, 0U, 0U, Pts, TimePerFrame, &CurrPos);
        if (Rval == FORMAT_OK) {
            if (Track->MediaId == SVC_FORMAT_MID_HVC) {
                /* Put vps sps, pps */
                Rval = SvcFormat_PutVPSSPSPPS(FeedParam, CurrPos, Track->BufferBase, Track->BufferSize, &CurrPos);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutVPSSPSPPS() error!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcH265_PutGOPHeader() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    *BufferOffset = CurrPos;
    return Rval;
}

static UINT32 SvcFormat_PutVideoFrameData(const SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, const SVC_FRAME_INFO_s *FrameInfo1, const SVC_FRAME_INFO_s *FrameInfo2, UINT32 *BufferOffset, UINT32 *DataSize)
{
    /* Put Frame data */
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Video->Info;
    UINT32 CurrPos = *BufferOffset;
    UINT32 CurrSize = 0U;
    UINT32 Rval = S2F(Stream->Func->Seek(Stream, (INT64) FrameInfo1->FramePos, SVC_STREAM_SEEK_START));
    if (Rval == FORMAT_OK) {
        if ((Track->MediaId == SVC_FORMAT_MID_H264) || (Track->MediaId == SVC_FORMAT_MID_H265)) {
            Rval = SvcFormat_ReadStream(Stream, FrameInfo1->FrameSize, CurrPos, Track->BufferBase, Track->BufferSize, &CurrPos);
            if (Rval == FORMAT_OK) {
                CurrSize = FrameInfo1->FrameSize;
            }
        } else if (Track->MediaId == SVC_FORMAT_MID_AVC) {
            Rval = SvcH264_ReadStream(Stream, CurrPos, Track->BufferBase, Track->BufferSize, FrameInfo1->FrameSize, &CurrSize);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PlusAddr(CurrSize, CurrPos, Track->BufferSize, &CurrPos);
            }
        } else {
            Rval = SvcH265_ReadStream(Stream, CurrPos, Track->BufferBase, Track->BufferSize, FrameInfo1->FrameSize, &CurrSize);
            if (Rval == FORMAT_OK) {
                Rval = SvcFormat_PlusAddr(CurrSize, CurrPos, Track->BufferSize, &CurrPos);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
    }
    if ((Rval == FORMAT_OK) && (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP)) {
        Rval = S2F(Stream->Func->Seek(Stream, (INT64) FrameInfo2->FramePos, SVC_STREAM_SEEK_START));
        if (Rval == FORMAT_OK) {
            UINT32 Tmp;
            if ((Track->MediaId == SVC_FORMAT_MID_H264) || (Track->MediaId == SVC_FORMAT_MID_H265)) {
                Rval = SvcFormat_ReadStream(Stream, FrameInfo2->FrameSize, CurrPos, Track->BufferBase, Track->BufferSize, &CurrPos);
                if (Rval == FORMAT_OK) {
                    CurrSize += FrameInfo2->FrameSize;
                }
            } else if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                Rval = SvcH264_ReadStream(Stream, CurrPos, Track->BufferBase, Track->BufferSize, FrameInfo2->FrameSize, &Tmp);
                if (Rval == FORMAT_OK) {
                    CurrSize += Tmp;
                }
            } else {
                Rval = SvcH265_ReadStream(Stream, CurrPos, Track->BufferBase, Track->BufferSize, FrameInfo2->FrameSize, &Tmp);
                if (Rval == FORMAT_OK) {
                    CurrSize += Tmp;
                }
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    *BufferOffset = CurrPos;
    *DataSize = CurrSize;
    return Rval;
}

static UINT32 SvcFormat_FeedIdrFrame(const SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, const SVC_FEED_FRAME_PARAM_s *FeedParam, const SVC_FRAME_INFO_s *FrameInfo1, const SVC_FRAME_INFO_s *FrameInfo2)
{
    UINT32 Rval;
    UINT32 CurrPos = 0U;
    UINT32 FrameSize = 0U;
    UINT32 DataSize = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Video->Info;
    const UINT64 Pts = FrameInfo1->PTS;
    SVC_FIFO_BITS_DESC_s  Frame;

    Rval = W2F(AmbaWrap_memset(&Frame, 0, sizeof(Frame)));
    if (Rval == FORMAT_OK) {
        if (Track->MediaId == SVC_FORMAT_MID_H264) {
            DataSize = SVC_H264_GOP_NALU_SIZE + FrameInfo1->FrameSize;
        } else if (Track->MediaId == SVC_FORMAT_MID_H265) {
            DataSize = SVC_H265_GOP_NALU_SIZE + FrameInfo1->FrameSize;
        } else if (Track->MediaId == SVC_FORMAT_MID_AVC) {
            DataSize = SVC_H264_GOP_NALU_SIZE + FrameInfo1->FrameSize + (SVC_H264_STRAT_CODE_SIZE*2U) + FeedParam->SPSLen + FeedParam->PPSLen;
        } else {
            DataSize = SVC_H265_GOP_NALU_SIZE + FrameInfo1->FrameSize + (SVC_H265_STRAT_CODE_SIZE*3U) + FeedParam->VPSLen + FeedParam->SPSLen + FeedParam->PPSLen;
        }
        DataSize += (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? FrameInfo2->FrameSize : 0U;
        Rval = SvcFormat_PrepareFrame(Track->Fifo, &Frame, DataSize, SVC_FIFO_TYPE_IDR_FRAME, Pts, Track->TimeScale);
    }
    if (Rval == FORMAT_OK) {
        CurrPos = GetOffset(Track->BufferBase, Frame.StartAddr);
        Rval = SvcFormat_PutKeyFrameHeader(Video, FeedParam, Pts, FrameInfo1->TimePerFrame, &CurrPos);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutKeyFrameHeader fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        /* Put Frame data */
        Rval = SvcFormat_PutVideoFrameData(Video, Stream, FrameInfo1, FrameInfo2, &CurrPos, &FrameSize);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutVideoFrameData fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        if ((Track->MediaId == SVC_FORMAT_MID_H264)) {
            DataSize = SVC_H264_GOP_NALU_SIZE + FrameSize;
        } else if ((Track->MediaId == SVC_FORMAT_MID_H265)) {
            DataSize = SVC_H265_GOP_NALU_SIZE + FrameSize;
        } else if (Track->MediaId == SVC_FORMAT_MID_AVC) {
            DataSize = SVC_H264_GOP_NALU_SIZE + FrameSize + (SVC_H264_STRAT_CODE_SIZE*2U) + FeedParam->SPSLen + FeedParam->PPSLen;
        } else {
            DataSize = SVC_H265_GOP_NALU_SIZE + FrameSize + (SVC_H265_STRAT_CODE_SIZE*3U) + FeedParam->VPSLen + FeedParam->SPSLen + FeedParam->PPSLen;
        }
        Rval = SvcFormat_WriteFrame(Track->Fifo, &Frame, DataSize, SVC_FIFO_TYPE_IDR_FRAME, Pts, Track->TimeScale, 1U);
    }
    return Rval;
}

static UINT32 SvcFormat_FeedIFrame(const SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, const SVC_FEED_FRAME_PARAM_s *FeedParam, const SVC_FRAME_INFO_s *FrameInfo1, const SVC_FRAME_INFO_s *FrameInfo2, UINT8 Direction)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 CurrPos = 0U;
    UINT32 FrameSize = 0U;
    UINT32 DataSize = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Video->Info;
    const UINT64 Pts = FrameInfo1->PTS;
    SVC_FIFO_BITS_DESC_s  Frame;

    Rval = W2F(AmbaWrap_memset(&Frame, 0, sizeof(Frame)));
    if (Rval == FORMAT_OK) {
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            DataSize = FrameInfo1->FrameSize + ((Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? FrameInfo2->FrameSize : 0U);
            Rval = SvcFormat_PrepareFrame(Track->Fifo, &Frame, DataSize, SVC_FIFO_TYPE_I_FRAME, Pts, Track->TimeScale);
            if (Rval == FORMAT_OK) {
                CurrPos = GetOffset(Track->BufferBase, Frame.StartAddr);
            }
        } else {
            if (Track->MediaId == SVC_FORMAT_MID_H264) {
                DataSize = SVC_H264_GOP_NALU_SIZE + FrameInfo1->FrameSize;
            } else if (Track->MediaId == SVC_FORMAT_MID_H265) {
                DataSize = SVC_H265_GOP_NALU_SIZE + FrameInfo1->FrameSize;
            } else if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                DataSize = SVC_H264_GOP_NALU_SIZE + FrameInfo1->FrameSize + (SVC_H264_STRAT_CODE_SIZE*2U) + FeedParam->SPSLen + FeedParam->PPSLen;
            } else {
                DataSize = SVC_H265_GOP_NALU_SIZE + FrameInfo1->FrameSize + (SVC_H265_STRAT_CODE_SIZE*3U) + FeedParam->VPSLen + FeedParam->SPSLen + FeedParam->PPSLen;
            }
            DataSize += (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? FrameInfo2->FrameSize : 0U;
            Rval = SvcFormat_PrepareFrame(Track->Fifo, &Frame, DataSize, SVC_FIFO_TYPE_I_FRAME, Pts, Track->TimeScale);
            if (Rval == FORMAT_OK) {
                CurrPos = GetOffset(Track->BufferBase, Frame.StartAddr);
                Rval = SvcFormat_PutKeyFrameHeader(Video, FeedParam, Pts, FrameInfo1->TimePerFrame, &CurrPos);
                if (Rval != FORMAT_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutKeyFrameHeader fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        Rval = SvcFormat_PutVideoFrameData(Video, Stream, FrameInfo1, FrameInfo2, &CurrPos, &FrameSize);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutVideoFrameData fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            DataSize = FrameSize;
        } else {
            if (Track->MediaId == SVC_FORMAT_MID_H264) {
                DataSize = SVC_H264_GOP_NALU_SIZE + FrameSize;
            } else if (Track->MediaId == SVC_FORMAT_MID_H265) {
                DataSize = SVC_H265_GOP_NALU_SIZE + FrameSize;
            } else if (Track->MediaId == SVC_FORMAT_MID_AVC) {
                DataSize = SVC_H264_GOP_NALU_SIZE + FrameSize + (SVC_H264_STRAT_CODE_SIZE*2U) + FeedParam->SPSLen + FeedParam->PPSLen;
            } else {
                DataSize = SVC_H265_GOP_NALU_SIZE + FrameSize + (SVC_H265_STRAT_CODE_SIZE*3U) + FeedParam->VPSLen + FeedParam->SPSLen + FeedParam->PPSLen;
            }
        }
        Rval = SvcFormat_WriteFrame(Track->Fifo, &Frame, DataSize, SVC_FIFO_TYPE_I_FRAME, Pts, Track->TimeScale, 1U);
    }
    return Rval;
}

static UINT32 SvcFormat_FeedPBFrame(const SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, const SVC_FRAME_INFO_s *FrameInfo1, const SVC_FRAME_INFO_s *FrameInfo2)
{
    UINT32 Rval;
    SVC_FIFO_BITS_DESC_s Frame;
    UINT32 FrameSize = 0U;
    UINT32 DataSize = 0U;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Video->Info;
    const UINT64 Pts = FrameInfo1->PTS;

    Rval = W2F(AmbaWrap_memset(&Frame, 0, sizeof(Frame)));
    if (Rval == FORMAT_OK) {
        DataSize = FrameInfo1->FrameSize + ((Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? FrameInfo2->FrameSize : 0U);
        Rval = SvcFormat_PrepareFrame(Track->Fifo, &Frame, DataSize, FrameInfo1->FrameType, Pts, Track->TimeScale);
    }
    if (Rval == FORMAT_OK) {
        UINT32 CurrPos = GetOffset(Track->BufferBase, Frame.StartAddr);
        Rval = SvcFormat_PutVideoFrameData(Video, Stream, FrameInfo1, FrameInfo2, &CurrPos, &FrameSize);
        if (Rval != FORMAT_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s SvcFormat_PutVideoFrameData fail!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    if (Rval == FORMAT_OK) {
        DataSize = FrameSize;
        Rval = SvcFormat_WriteFrame(Track->Fifo, &Frame, DataSize, FrameInfo1->FrameType, Pts, Track->TimeScale, 1U);
    }
    return Rval;
}

/**
 * Feed an audio frame
 *
 * @param [in] Audio Audio track
 * @param [in] Stream Stream handler
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] Direction Direction
 * @param [in] FrameNo Target frame number
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_FeedAudioFrame(SVC_AUDIO_TRACK_INFO_s *Audio, SVC_STREAM_HDLR_s *Stream, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo)
{
    SVC_FRAME_INFO_s FrameInfo;
    UINT32 Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, Direction, &FrameInfo);
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
        const UINT64 Pts = Track->DTS;
        SVC_FIFO_BITS_DESC_s Frame;
        Rval = W2F(AmbaWrap_memset(&Frame, 0, sizeof(Frame)));
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PrepareFrame(Track->Fifo, &Frame, FrameInfo.FrameSize, SVC_FIFO_TYPE_AUDIO_FRAME, Pts, Track->TimeScale);
            if (Rval == FORMAT_OK) {
                /* Put Frame data */
                Rval = S2F(Stream->Func->Seek(Stream, (INT64) FrameInfo.FramePos, SVC_STREAM_SEEK_START));
                if (Rval == FORMAT_OK) {
                    UINT32 CurrPos = GetOffset(Track->BufferBase, Frame.StartAddr);
                    Rval = SvcFormat_ReadStream(Stream, FrameInfo.FrameSize, CurrPos, Track->BufferBase, Track->BufferSize, &CurrPos);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_WriteFrame(Track->Fifo, &Frame, FrameInfo.FrameSize, SVC_FIFO_TYPE_AUDIO_FRAME, Pts, Track->TimeScale, 1U);
                        if (Rval == FORMAT_OK) {
                            //AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "[Audio] frameNo = %u, PTS = %llu, FrameSize = %u, FrameType = %u DTS =%llu", FrameNo, FrameInfo.PTS, FrameInfo.FrameSize, FrameInfo.FrameType, Track->DTS);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo failed", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Audio);
    return Rval;
}

/**
 * Feed a text frame
 *
 * @param [in] Text Text track
 * @param [in] Stream Stream handler
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] Direction Direction
 * @param [in] FrameNo Target frame number
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_FeedTextFrame(SVC_TEXT_TRACK_INFO_s *Text, SVC_STREAM_HDLR_s *Stream, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo)
{
    SVC_FRAME_INFO_s FrameInfo;
    UINT32 Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, Direction, &FrameInfo);
    if (Rval == FORMAT_OK) {
        const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
        const UINT64 Pts = Track->DTS;
        SVC_FIFO_BITS_DESC_s Frame;
        Rval = W2F(AmbaWrap_memset(&Frame, 0, sizeof(Frame)));
        if (Rval == FORMAT_OK) {
            Rval = SvcFormat_PrepareFrame(Track->Fifo, &Frame, FrameInfo.FrameSize, SVC_FIFO_TYPE_UNDEFINED, Pts, Track->TimeScale);
            if (Rval == FORMAT_OK) {
                /* Put Frame data */
                Rval = S2F(Stream->Func->Seek(Stream, (INT64) FrameInfo.FramePos, SVC_STREAM_SEEK_START));
                if (Rval == FORMAT_OK) {
                    UINT32 CurrPos = GetOffset(Track->BufferBase, Frame.StartAddr);
                    Rval = SvcFormat_ReadStream(Stream, FrameInfo.FrameSize, CurrPos, Track->BufferBase, Track->BufferSize, &CurrPos);
                    if (Rval == FORMAT_OK) {
                        Rval = SvcFormat_WriteFrame(Track->Fifo, &Frame, FrameInfo.FrameSize, SVC_FIFO_TYPE_UNDEFINED, Pts, Track->TimeScale, 1U);
                        if (Rval == FORMAT_OK) {
                            //LOG_VERB("[Text] frameNo = %u, PTS = %llu, FrameSize = %u, FrameType = %u DTS =%llu", FrameNo, FrameInfo.PTS, FrameInfo.FrameSize, FrameInfo.FrameType, Track->DTS);
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek fail!", __func__, NULL, NULL, NULL, NULL);
                }
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo failed", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Text);
    return Rval;
}

/**
 * Feed a video frame
 *
 * @param [in] Video Video track
 * @param [in] Stream Stream handler
 * @param [in] FeedParam Frame feeder parameters
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] Direction Direction
 * @param [in] FrameNo Target frame number
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_FeedVideoFrame(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, SVC_FEED_FRAME_PARAM_s *FeedParam, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo)
{
    SVC_FRAME_INFO_s FrameInfo1;
    SVC_FRAME_INFO_s FrameInfo2;
    UINT32 Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, Direction, &FrameInfo1);
    if (Rval == FORMAT_OK) {
        if (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
            Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo + 1U, Direction, &FrameInfo2);
            if (Rval != FORMAT_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
            }
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
    }
    if (Rval == FORMAT_OK) {
        if (FrameInfo1.FrameType == SVC_FIFO_TYPE_IDR_FRAME) {
            Rval = SvcFormat_FeedIdrFrame(Video, Stream, FeedParam, &FrameInfo1, &FrameInfo2);
        } else if (FrameInfo1.FrameType == SVC_FIFO_TYPE_I_FRAME) {
            Rval = SvcFormat_FeedIFrame(Video, Stream, FeedParam, &FrameInfo1, &FrameInfo2, Direction);
        } else {
            Rval = SvcFormat_FeedPBFrame(Video, Stream, &FrameInfo1, &FrameInfo2);
        }
        //LOG_VERB("[Video] speed[%u] frameNo = %u, PTS = %llu, FrameSize = %u, FrameType = %u DTS =%llu", Speed, FrameNo, FrameInfo1.PTS, FrameInfo1.FrameSize, FrameInfo1.FrameType, Track->DTS);
    }
    AmbaMisra_TouchUnused(FeedParam);
    AmbaMisra_TouchUnused(Video);
    return Rval;
}

static inline UINT64 FORMAT_DTS_NORMALIZE(UINT64 DTS, UINT32 TrackGcd, UINT32 Ratio)
{
    return (((DTS / (UINT64)TrackGcd) * Ratio) + (((DTS % (UINT64)TrackGcd) * Ratio) / TrackGcd));
}

static UINT32 SvcFormat_NormalizeTimeScaleM(SVC_MOVIE_INFO_s *Movie)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 i;
    UINT32 GCD_V[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA] = {0U};
    UINT32 GCD_A[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA] = {0U};
    UINT32 GCD_T[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA] = {0U};
    // keep OrigTimeScale, reduce TimeScale and TimePerFrame
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->VideoTrack[i].Info;
        Track->OrigTimeScale = Track->TimeScale;
        /* Do not apply fraction reduction in CaptureTimeMode */
        /* Ensure that the normalized TimeScale is large enough and is a multiple of the original TimeScale. */
        /* E.g. Track1's GopSize = 11, frame rate = 1/11 --- reduction gcd --> 1/11
                Track2's GopSize = 29, frame rate = 700/2900 --- reduction gcd --> 7/29
                --> LCM: 77/319 --> since 77 is too small, make it greater than 1000.
                --> (77*13)/(319*13) = 1001/4147
                --> 4147 is not a multiple of 2900
                Delta_TPF = Delta_Total * (OriginalTimeScale / NormalizedTimeScale) / GopSize
                E.g. Delta_TPF_Ori = 208 * 2900/4147  / 29 = 5
                     Delta_TPF_Nor = 5 * 4147/2900 -> information lost
                     -> The normalized DTS and original DTS can't be matched.
                Issue might occurs. e.g. Trim on TPF. */
        if (Movie->VideoTrack[i].CaptureTimeMode == 0U) {
            GCD_V[i] = (UINT32)SvcFormat_GetGCD(Track->TimeScale, Track->TimePerFrame);
            if (GCD_V[i] == 0U) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value of GCD", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
                break;
            }
            Track->TimeScale /= GCD_V[i];
            Track->TimePerFrame /= GCD_V[i];
        } else {
            // not to reduce because a small TimeScale could make frame rate hard to be adjusted
            GCD_V[i] = 1U;
        }
    }
    if (Rval == FORMAT_OK) {
        for (i = 0U; i < Movie->AudioTrackCount; i++) {
            SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->AudioTrack[i].Info;
            Track->OrigTimeScale = Track->TimeScale;
            GCD_A[i] = (UINT32)SvcFormat_GetGCD(Track->TimeScale, Track->TimePerFrame);
            if (GCD_A[i] == 0U) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value of GCD", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
                break;
            }
            Track->TimeScale /= GCD_A[i];
            Track->TimePerFrame /= GCD_A[i];
        }
    }
    if (Rval == FORMAT_OK) {
        for (i = 0U; i < Movie->TextTrackCount; i++) {
            SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->TextTrack[i].Info;
            Track->OrigTimeScale = Track->TimeScale;
            GCD_T[i] = (UINT32)SvcFormat_GetGCD(Track->TimeScale, Track->TimePerFrame);
            if (GCD_T[i] == 0U) {
                AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Wrong value of GCD", __func__, NULL, NULL, NULL, NULL);
                Rval = FORMAT_ERR_GENERAL_ERROR;
                break;
            }
            Track->TimeScale /= GCD_T[i];
            Track->TimePerFrame /= GCD_T[i];
        }
    }
    if (Rval == FORMAT_OK) {
        UINT32 TimeScaleLCM = 1000U;
        Rval = SvcFormat_GetTimeScaleLCM(&Movie->MediaInfo, &TimeScaleLCM);
        if (Rval == FORMAT_OK) {
            for (i = 0; i < Movie->VideoTrackCount; i++) {
                SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->VideoTrack[i].Info;
                UINT32 Ratio = TimeScaleLCM / Track->TimeScale;
                Track->FrameNo = 0U;
                Track->TimePerFrame *= Ratio;
                Track->TimeScale = TimeScaleLCM;
                Track->InitDTS = FORMAT_DTS_NORMALIZE(Track->InitDTS, GCD_V[i], Ratio);
                Track->DTS = FORMAT_DTS_NORMALIZE(Track->DTS, GCD_V[i], Ratio);
                Track->NextDTS = FORMAT_DTS_NORMALIZE(Track->NextDTS, GCD_V[i], Ratio);
            }
            for (i = 0U; i < Movie->AudioTrackCount; i++) {
                SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->AudioTrack[i].Info;
                UINT32 Ratio = TimeScaleLCM / Track->TimeScale;
                Track->FrameNo = 0U;
                Track->TimePerFrame *= Ratio;
                Track->TimeScale = TimeScaleLCM;
                Track->InitDTS = FORMAT_DTS_NORMALIZE(Track->InitDTS, GCD_A[i], Ratio);
                Track->DTS = FORMAT_DTS_NORMALIZE(Track->DTS, GCD_A[i], Ratio);
                Track->NextDTS = FORMAT_DTS_NORMALIZE(Track->NextDTS, GCD_A[i], Ratio);
            }
            for (i = 0U; i < Movie->TextTrackCount; i++) {
                SVC_MEDIA_TRACK_GENERAL_INFO_s * const Track = &Movie->TextTrack[i].Info;
                UINT32 Ratio = TimeScaleLCM / Track->TimeScale;
                Track->FrameNo = 0U;
                Track->TimePerFrame *= Ratio;
                Track->TimeScale = TimeScaleLCM;
                Track->InitDTS = FORMAT_DTS_NORMALIZE(Track->InitDTS, GCD_T[i], Ratio);
                Track->DTS = FORMAT_DTS_NORMALIZE(Track->DTS, GCD_T[i], Ratio);
                Track->NextDTS = FORMAT_DTS_NORMALIZE(Track->NextDTS, GCD_T[i], Ratio);
            }
        }
    }
    return Rval;
}

/**
 * Normalize media time scale
 *
 * @param [in] Media Media information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_NormalizeTimeScale(const SVC_MEDIA_INFO_s *Media)
{
    UINT32 Rval;
    if (Media->MediaType == SVC_MEDIA_INFO_MOVIE) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = SvcFormat_NormalizeTimeScaleM(Movie);
    } else {
        AmbaPrint_ModulePrintStr5(SVC_MUXER_PRINT_MODULE_ID, "%s Invalid arguement!", __func__, NULL, NULL, NULL, NULL);
        Rval = FORMAT_ERR_INVALID_ARG;
    }
    return Rval;
}

/**
 * Default Video Track-
 * Forward: TargetFrame.DTS <= DTS < TargetFrame.DTS + TimePerFrame
 * Backward: TargetFrame.DTS < DTS <= TargetFrame.DTS + TimePerFrame
 */
static UINT32 SvcFormat_TimeSearchDefVideo_Forward(SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT64 DTS, UINT32 *FrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TargetFrameType;
    UINT32 SearchFactor;
    UINT32 RealFrameNo, TmpFrameNo, TargetFrameNo;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    /* TmpFrameNo is used to speed up search. */
    TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO(DTS, Track->TimePerFrame);
    TmpFrameNo = (TmpFrameNo >= Track->FrameCount) ? (Track->FrameCount - 1U) : TmpFrameNo;

    if (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
        TmpFrameNo -= TmpFrameNo % 2U;
    }
    SearchFactor = (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2U : 1U;
    TargetFrameType = SVC_FIFO_TYPE_IDR_FRAME;
    TargetFrameNo = 0U;

    for (RealFrameNo = TmpFrameNo; RealFrameNo > 0U; RealFrameNo -= SearchFactor) {
        SVC_FRAME_INFO_s FrameInfo = {0};
        Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
        if ((Rval != FORMAT_OK) || ((FrameInfo.DTS <= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        TargetFrameNo = RealFrameNo;
        if (Video->CaptureTimeMode == 1U) {
            /* Search the opposite direction if the FrameRate is not fixed. */
            for (RealFrameNo = TmpFrameNo + SearchFactor; RealFrameNo < Track->FrameCount; RealFrameNo += SearchFactor) {
                SVC_FRAME_INFO_s FrameInfo = {0};
                Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
                if ((Rval != FORMAT_OK) || (FrameInfo.DTS > DTS)) {
                    break;
                } else {
                    if (FrameInfo.FrameType == TargetFrameType) {
                        TargetFrameNo = RealFrameNo;
                    }
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        *FrameNo = TargetFrameNo;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Video);
    return Rval;
}

static UINT32 SvcFormat_TimeSearchDefVideo_BwdNotFound(const SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT32 SearchFactor, UINT32 StartFrameNo, UINT32 *TargetFrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TmpFrameNo, RealFrameNo;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    /* IDR not found. Find the previous IDR instead. */
    if (Video->CaptureTimeMode == 1U) {
        TmpFrameNo = Track->FrameCount;
    } else {
        TmpFrameNo = StartFrameNo;
    }
    for (RealFrameNo = TmpFrameNo; RealFrameNo >= SearchFactor; RealFrameNo -= SearchFactor) {
        SVC_FRAME_INFO_s FrameInfo = {0};
        Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo - SearchFactor, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
        if ((Rval != FORMAT_OK) || (FrameInfo.FrameType == SVC_FIFO_TYPE_IDR_FRAME)) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        if (RealFrameNo >= SearchFactor) {
            *TargetFrameNo = RealFrameNo - SearchFactor;
        } else {
            *TargetFrameNo = 0U;
        }
    }
    return Rval;
}

/**
 * Default Video Track-
 * Forward: TargetFrame.DTS <= DTS < TargetFrame.DTS + TimePerFrame
 * Backward: TargetFrame.DTS < DTS <= TargetFrame.DTS + TimePerFrame
 */
static UINT32 SvcFormat_TimeSearchDefVideo_Backward(SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT64 DTS, UINT32 *FrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TargetFrameType;
    UINT32 SearchFactor;
    UINT32 TmpFrameNo, RealFrameNo, TargetFrameNo;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO_CEIL(DTS, Track->TimePerFrame);
    if (TmpFrameNo > 0U) {
        TmpFrameNo -= 1U;
    }
    TmpFrameNo = (TmpFrameNo >= Track->FrameCount) ? (Track->FrameCount - 1U) : TmpFrameNo;
    // FrameNo is used to speed up search.
    if (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
        TmpFrameNo -= TmpFrameNo % 2U;
    }
    SearchFactor = (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2U : 1U;
    TargetFrameType = SVC_FIFO_TYPE_IDR_FRAME;
    TargetFrameNo = 0U;

    for (RealFrameNo = TmpFrameNo; RealFrameNo < Track->FrameCount; RealFrameNo += SearchFactor) {
        SVC_FRAME_INFO_s FrameInfo = {0};
        Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
        if ((Rval != FORMAT_OK) || (((FrameInfo.DTS + ((UINT64)FrameInfo.TimePerFrame * (UINT64)SearchFactor)) >= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        TargetFrameNo = RealFrameNo;
        if (RealFrameNo == Track->FrameCount) {
            Rval = SvcFormat_TimeSearchDefVideo_BwdNotFound(Video, IdxMgr, TrackId, GetFrameInfo, SearchFactor, TmpFrameNo, &TargetFrameNo);
        } else {
            if (Video->CaptureTimeMode == 1U) {
                /* Search the opposite direction if the FrameRate is not fixed. */
                for (RealFrameNo = TmpFrameNo; RealFrameNo >= SearchFactor; RealFrameNo -= SearchFactor) {
                    SVC_FRAME_INFO_s FrameInfo = {0};
                    Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo - SearchFactor, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
                    if ((Rval != FORMAT_OK) || ((FrameInfo.DTS + ((UINT64)FrameInfo.TimePerFrame * (UINT64)SearchFactor)) < DTS)){
                        break;
                    } else {
                        if (FrameInfo.FrameType == TargetFrameType) {
                            TargetFrameNo = RealFrameNo - SearchFactor;
                        }
                    }
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        *FrameNo = TargetFrameNo;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Video);
    return Rval;
}

/**
 * Forward: DTS <= TargetFrame.DTS
 * Backward: TargetFrame.DTS + TimePerFrame <= DTS
 */
static UINT32 SvcFormat_TimeSearchVideo_Forward(SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT64 DTS, UINT32 *FrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TargetFrameType;
    UINT32 TmpFrameNo, RealFrameNo, TargetFrameNo = 0U;
    UINT32 SearchFactor;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    /**/
    TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO_CEIL(DTS, Track->TimePerFrame);
    TmpFrameNo = (TmpFrameNo >= Track->FrameCount) ? (Track->FrameCount - 1U) : TmpFrameNo;
    // FrameNo is used to speed up search.

    if (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
        TmpFrameNo -= TmpFrameNo % 2U;
    }
    SearchFactor = (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2U : 1U;
    TargetFrameType = SVC_FIFO_TYPE_IDR_FRAME;

    for (RealFrameNo = TmpFrameNo; RealFrameNo < Track->FrameCount; RealFrameNo += SearchFactor) {
        SVC_FRAME_INFO_s FrameInfo = {0};
        Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
        if ((Rval != FORMAT_OK) || ((FrameInfo.DTS >= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        TargetFrameNo = RealFrameNo;
        if (Video->CaptureTimeMode == 1U) {
            /* Search the opposite direction if the FrameRate is not fixed. */
            for (RealFrameNo = TmpFrameNo; RealFrameNo >= SearchFactor; RealFrameNo -= SearchFactor) {
                SVC_FRAME_INFO_s FrameInfo = {0};
                Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo - SearchFactor, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
                if ((Rval != FORMAT_OK) || (FrameInfo.DTS < DTS)){
                    break;
                } else {
                    if (FrameInfo.FrameType == TargetFrameType) {
                        TargetFrameNo = RealFrameNo - SearchFactor;
                    }
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        *FrameNo = TargetFrameNo;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Video);
    return Rval;
}

/**
 * Forward: DTS <= TargetFrame.DTS
 * Backward: TargetFrame.DTS + TimePerFrame <= DTS
 */
static UINT32 SvcFormat_TimeSearchVideo_Backward(SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT64 DTS, UINT32 *FrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TargetFrameType;
    UINT32 TmpFrameNo, RealFrameNo, TargetFrameNo = 0;
    UINT32 SearchFactor;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO(DTS, Track->TimePerFrame);
    if (TmpFrameNo > 0U) {
        TmpFrameNo -= 1U;
    }
    TmpFrameNo = (TmpFrameNo >= Track->FrameCount) ? (Track->FrameCount - 1U) : TmpFrameNo;
    // FrameNo is used to speed up search.
    if (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
        TmpFrameNo -= TmpFrameNo % 2U;
    }
    SearchFactor = (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) ? 2U : 1U;
    TargetFrameType = SVC_FIFO_TYPE_IDR_FRAME;

    for (RealFrameNo = TmpFrameNo; RealFrameNo > 0U; RealFrameNo -= SearchFactor) {
        SVC_FRAME_INFO_s FrameInfo = {0};
        Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
        if ((Rval != FORMAT_OK) || (((FrameInfo.DTS + ((UINT64)FrameInfo.TimePerFrame * (UINT64)SearchFactor)) <= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
            break;
        }
    }
    if (Rval == FORMAT_OK) {
        TargetFrameNo = RealFrameNo;
        if (RealFrameNo == 0U) {
            SVC_FRAME_INFO_s FrameInfo = {0};
            Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
            if ((Rval == FORMAT_OK) && ((FrameInfo.DTS + ((UINT64)FrameInfo.TimePerFrame * (UINT64)SearchFactor)) > DTS)) {
                // not found.
                TargetFrameNo = Track->FrameCount;
            }
        } else {
            if (Video->CaptureTimeMode == 1U) {
                /* Search the opposite direction if the FrameRate is not fixed. */
                for (RealFrameNo = TmpFrameNo + SearchFactor; RealFrameNo < Track->FrameCount; RealFrameNo += SearchFactor) {
                    SVC_FRAME_INFO_s FrameInfo = {0};
                    Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
                    if ((Rval != FORMAT_OK) || ((FrameInfo.DTS + ((UINT64)FrameInfo.TimePerFrame * (UINT64)SearchFactor)) > DTS)) {
                        break;
                    } else {
                        if (FrameInfo.FrameType == TargetFrameType) {
                            TargetFrameNo = RealFrameNo;
                        }
                    }
                }
            }
        }
    }
    if (Rval == FORMAT_OK) {
        *FrameNo = TargetFrameNo;
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
    }
    AmbaMisra_TouchUnused(Video);
    return Rval;
}

/**
 * Forward: DTS <= TargetFrame.DTS
 * Backward: TargetFrame.DTS + TimePerFrame <= DTS
 */
static UINT32 SvcFormat_TimeSearchAudio(SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS, UINT32 *FrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TargetFrameType;
    UINT32 TmpFrameNo = 0;
    UINT32 RealFrameNo = 0;
    UINT32 SearchFactor;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
    if (Direction == SVC_FORMAT_DIR_FORWARD) {
        TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO_CEIL(DTS, Track->TimePerFrame);
    } else {
        TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO(DTS, Track->TimePerFrame);
        if (TmpFrameNo > 0U) {
            TmpFrameNo -= 1U;
        }
    }
    TmpFrameNo = (TmpFrameNo >= Track->FrameCount) ? (Track->FrameCount - 1U) : TmpFrameNo;
    // FrameNo is used to speed up search.
    SearchFactor = 1;
    TargetFrameType = SVC_FIFO_TYPE_AUDIO_FRAME;

    if (Direction == SVC_FORMAT_DIR_FORWARD) {
        for (RealFrameNo = TmpFrameNo; RealFrameNo < Track->FrameCount; RealFrameNo += SearchFactor) {
            SVC_FRAME_INFO_s FrameInfo = {0};
            Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_FORWARD, &FrameInfo);
            if ((Rval != FORMAT_OK) || ((FrameInfo.DTS >= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            *FrameNo = RealFrameNo;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        for (RealFrameNo = TmpFrameNo; RealFrameNo > 0U; RealFrameNo -= SearchFactor) {
            SVC_FRAME_INFO_s FrameInfo = {0};
            Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
            if ((Rval != FORMAT_OK) || (((FrameInfo.DTS + Track->TimePerFrame) <= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            if (RealFrameNo == 0U) {
                SVC_FRAME_INFO_s FrameInfo = {0};
                Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
                if ((Rval == FORMAT_OK) && ((FrameInfo.DTS + Track->TimePerFrame) > DTS)) {
                    // not found.
                    RealFrameNo = Track->FrameCount;
                }
            }
            *FrameNo = RealFrameNo;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Audio);
    return Rval;
}

/**
 * Forward: DTS <= TargetFrame.DTS
 * Backward: TargetFrame.DTS + TimePerFrame <= DTS
 */
static UINT32 SvcFormat_TimeSearchText(SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS, UINT32 *FrameNo)
{
    UINT32 Rval = FORMAT_OK;
    UINT32 TargetFrameType;
    UINT32 TmpFrameNo = 0;
    UINT32 RealFrameNo = 0;
    UINT32 SearchFactor;
    const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
    if (Direction == SVC_FORMAT_DIR_FORWARD) {
        TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO_CEIL(DTS, Track->TimePerFrame);
    } else {
        TmpFrameNo = (UINT32) SVC_FORMAT_DTS_TO_FNO(DTS, Track->TimePerFrame);
        if (TmpFrameNo > 0U) {
            TmpFrameNo -= 1U;
        }
    }
    TmpFrameNo = (TmpFrameNo >= Track->FrameCount) ? (Track->FrameCount - 1U) : TmpFrameNo;
    // FrameNo is used to speed up search.
    SearchFactor = 1;
    TargetFrameType = SVC_FIFO_TYPE_UNDEFINED;

    if (Direction == SVC_FORMAT_DIR_FORWARD) {
        for (RealFrameNo = TmpFrameNo; RealFrameNo < Track->FrameCount; RealFrameNo += SearchFactor) {
            SVC_FRAME_INFO_s FrameInfo = {0};
            Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
            if ((Rval != FORMAT_OK) || ((FrameInfo.DTS >= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            *FrameNo = RealFrameNo;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        for (RealFrameNo = TmpFrameNo; RealFrameNo > 0U; RealFrameNo -= SearchFactor) {
            SVC_FRAME_INFO_s FrameInfo = {0};
            Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
            if ((Rval != FORMAT_OK) || (((FrameInfo.DTS + Track->TimePerFrame) <= DTS) && (FrameInfo.FrameType == TargetFrameType))) {
                break;
            }
        }
        if (Rval == FORMAT_OK) {
            if (RealFrameNo == 0U) {
                SVC_FRAME_INFO_s FrameInfo = {0};
                Rval = GetFrameInfo(IdxMgr, TrackId, RealFrameNo, SVC_FORMAT_DIR_BACKWARD, &FrameInfo);
                if ((Rval == FORMAT_OK) && ((FrameInfo.DTS + Track->TimePerFrame) > DTS)) {
                    // not found.
                    RealFrameNo = Track->FrameCount;
                }
            }
            *FrameNo = RealFrameNo;
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
        }
    }
    AmbaMisra_TouchUnused(Text);
    return Rval;
}

/**
 * Seek and set the start time of a video track
 *
 * @param [in] Video Video track
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] Direction Seek direction
 * @param [in] DTS Target DTS
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_SeekVideo(SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS)
{
    UINT32 Rval = FORMAT_OK;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Video->Info;
    if (Track->Fifo != NULL) {
        UINT32 FrameNo;
        //Forward : FrameInfo.DTS >= dts , Backward : FrameInfo.DTS <= dts
        if (Video->IsDefault == 1U) {
            if (Direction == SVC_FORMAT_DIR_FORWARD) {
                Rval = SvcFormat_TimeSearchDefVideo_Forward(Video, IdxMgr, TrackId, GetFrameInfo, DTS, &FrameNo);
            } else {
                Rval = SvcFormat_TimeSearchDefVideo_Backward(Video, IdxMgr, TrackId, GetFrameInfo, DTS, &FrameNo);
            }
        } else {
            if (Direction == SVC_FORMAT_DIR_FORWARD) {
                Rval = SvcFormat_TimeSearchVideo_Forward(Video, IdxMgr, TrackId, GetFrameInfo, DTS, &FrameNo);
            } else {
                Rval = SvcFormat_TimeSearchVideo_Backward(Video, IdxMgr, TrackId, GetFrameInfo, DTS, &FrameNo);
            }
        }
        if (Rval == FORMAT_OK) {
            if (FrameNo < Track->FrameCount){
                SVC_FRAME_INFO_s FrameInfo;
                Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, Direction, &FrameInfo);
                if (Rval == FORMAT_OK) {
                    Track->FrameNo = FrameNo;
                    Track->DTS = FrameInfo.DTS;
                    if (Direction == SVC_FORMAT_DIR_BACKWARD) {
                        if (Video->Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
                            Track->DTS += ((UINT64)FrameInfo.TimePerFrame << 1U);
                        } else {
                            Track->DTS += (UINT64)FrameInfo.TimePerFrame;
                        }
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                // non-default track could reach EOS.
                Track->FrameNo = FrameNo;
                Track->DTS = (Direction == SVC_FORMAT_DIR_FORWARD)? SVC_FORMAT_MAX_TIMESTAMP : 0U;
            }
        }
    } else {
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            Track->DTS = 0U;
        }
    }
    //LOG_VERB("[%u]Track->FrameNo = %u, Track->DTS = %llu", Track->TrackId, Track->FrameNo, Track->DTS);
    return Rval;
}

/**
 * Seek and set the start time of a audio track
 *
 * @param [in] Audio Audio track
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] Direction Seek direction
 * @param [in] DTS Target DTS
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_SeekAudio(SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS)
{
    UINT32 Rval = FORMAT_OK;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Audio->Info;
    if (Track->Fifo != NULL) {
        UINT32 FrameNo;
        //Forward : FrameInfo.DTS >= dts , Backward : FrameInfo.DTS <= dts
        Rval = SvcFormat_TimeSearchAudio(Audio, IdxMgr, TrackId, GetFrameInfo, Direction, DTS, &FrameNo);
        if (Rval == FORMAT_OK) {
            if (FrameNo < Track->FrameCount){
                SVC_FRAME_INFO_s FrameInfo;
                Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, Direction, &FrameInfo);
                if (Rval == FORMAT_OK) {
                    Track->FrameNo = FrameNo;
                    Track->DTS = FrameInfo.DTS;
                    if (Direction == SVC_FORMAT_DIR_BACKWARD) {
                        Track->DTS += (UINT64)Track->TimePerFrame;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                // non-default track could reach EOS.
                Track->FrameNo = FrameNo;
                Track->DTS = (Direction == SVC_FORMAT_DIR_FORWARD)? SVC_FORMAT_MAX_TIMESTAMP : 0U;
            }

        }
    } else {
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            Track->DTS = 0U;
        }
    }
    //LOG_VERB("[%u]track->FrameNo = %u, track->DTS = %llu", track->TrackId, track->FrameNo, track->DTS);
    return Rval;
}

/**
 * Seek and set the start time of a text track
 *
 * @param [in] Text Text track
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] Direction Seek direction
 * @param [in] DTS Target DTS
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_SeekText(SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS)
{
    UINT32 Rval = FORMAT_OK;
    SVC_MEDIA_TRACK_GENERAL_INFO_s *Track = &Text->Info;
    if (Track->Fifo != NULL) {
        UINT32 FrameNo;
        //Forward : FrameInfo.DTS >= dts , Backward : FrameInfo.DTS <= dts
        Rval = SvcFormat_TimeSearchText(Text, IdxMgr, TrackId, GetFrameInfo, Direction, DTS, &FrameNo);
        if (Rval == FORMAT_OK) {
            if (FrameNo < Track->FrameCount){
                SVC_FRAME_INFO_s FrameInfo;
                Rval = GetFrameInfo(IdxMgr, TrackId, FrameNo, Direction, &FrameInfo);
                if (Rval == FORMAT_OK) {
                    Track->FrameNo = FrameNo;
                    Track->DTS = FrameInfo.DTS;
                    if (Direction == SVC_FORMAT_DIR_BACKWARD) {
                        Track->DTS += (UINT64)Track->TimePerFrame;
                    }
                } else {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetFrameInfo() error!", __func__, NULL, NULL, NULL, NULL);
                }
            } else {
                // non-default track could reach EOS.
                Track->FrameNo = FrameNo;
                Track->DTS = (Direction == SVC_FORMAT_DIR_FORWARD)? SVC_FORMAT_MAX_TIMESTAMP : 0U;
            }
        }
    } else {
        if (Direction == SVC_FORMAT_DIR_FORWARD) {
            Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
        } else {
            Track->DTS = 0U;
        }
    }
    //LOG_VERB("[%u]Track->FrameNo = %u, Track->DTS = %llu", Track->TrackId, Track->FrameNo, Track->DTS);
    return Rval;
}

/**
 * Check if there is a track with empty clip or not
 *
 * @param [in] Media Media information
 * @return 1U of 0U
 */
UINT8 SvcFormat_HasEmptyTrack(const SVC_MEDIA_INFO_s *Media)
{
    const SVC_VIDEO_TRACK_INFO_s *VideoTracks = NULL;
    const SVC_AUDIO_TRACK_INFO_s *AudioTracks = NULL;
    const SVC_TEXT_TRACK_INFO_s *TextTracks = NULL;
    UINT8 VideoTrackCount = 0, AudioTrackCount = 0, TextTrackCount = 0;
    UINT32 i;
    UINT8 Rval = 0U;
    const SVC_MOVIE_INFO_s *Movie;
    AmbaMisra_TypeCast(&Movie, &Media);
    VideoTracks = Movie->VideoTrack;
    AudioTracks = Movie->AudioTrack;
    TextTracks = Movie->TextTrack;
    VideoTrackCount = Movie->VideoTrackCount;
    AudioTrackCount = Movie->AudioTrackCount;
    TextTrackCount = Movie->TextTrackCount;

    for (i = 0; i < VideoTrackCount; i++) {
        const SVC_VIDEO_TRACK_INFO_s *Video = &VideoTracks[i];
        const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Video->Info;
        if (Track->FrameCount < 1U) {
            Rval = 1U;
            break;
        }
    }

    if (Rval != 1U) {
        for (i = 0; i < AudioTrackCount; i++) {
            const SVC_AUDIO_TRACK_INFO_s *Audio = &AudioTracks[i];
            const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Audio->Info;
            if (Track->FrameCount < 1U) {
                Rval = 1U;
                break;
            }
        }
        if (Rval != 1U) {
            for (i = 0; i < TextTrackCount; i++) {
                const SVC_TEXT_TRACK_INFO_s *Text = &TextTracks[i];
                const SVC_MEDIA_TRACK_GENERAL_INFO_s * Track = &Text->Info;
                if (Track->FrameCount < 1U) {
                    Rval = 1U;
                    break;
                }
            }
        }
    }
    return Rval;
}

/**
 * Update the general information of a track
 *
 * @param [in] Track Track information
 * @param [in] FrameCount Number of frames
 */
void SvcFormat_UpdateTrack(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, UINT32 FrameCount)
{
    Track->FrameNo += FrameCount;
    Track->FrameCount += FrameCount;
    Track->DTS = Track->DTS + SVC_FORMAT_DURATION(FrameCount, Track->TimePerFrame);
    Track->NextDTS = Track->DTS;
}

/**
 * Check if the clip reaches limit
 *
 * @param [in] Track Track information
 * @param [in] Stream Stream handler
 * @param [in] MaxDuration Duration limitation
 * @param [in] MaxSize Size limitation
 * @param [out] Limit If the clip reaches limit or not
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_CheckLimit(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *Limit)
{
    UINT32 Rval = FORMAT_OK;
    const UINT64 MaxDTS = SVC_FORMAT_SPLIT_DTS(Track->InitDTS, MaxDuration, Track->TimeScale);
    if (Track->DTS >= MaxDTS) {
        *Limit = 1U;
    } else {
        if (Stream != NULL) {
            UINT64 Len;
            Rval = S2F(Stream->Func->GetLength(Stream, &Len));
            if (Rval == FORMAT_OK) {
                if (Len >= MaxSize) {
                    *Limit = 1U;
                }
            } else {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s GetLength() failed", __func__, NULL, NULL, NULL, NULL);
            }
        } else {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Invalid Stream", __func__, NULL, NULL, NULL, NULL);
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

/**
 * Update slice index when receiving a frame
 *
 * @param [in] Frame Frame descriptor
 * @param [out] Video Video track information
 */
void SvcFormat_UpdateFrameSliceIdx(const SVC_FIFO_BITS_DESC_s *Frame, SVC_VIDEO_TRACK_INFO_s *Video)
{
    if (Frame->Completed == 1U) {
        Video->SliceIdx = 0;
    } else {
        Video->SliceIdx++;
    }
}

UINT8 SvcFormat_GetVideoFrameType(UINT8 Mode, UINT16 M, UINT16 N, UINT32 GOPSize, UINT8 ClosedGOP, UINT32 SampleIdx)
{
    UINT8 Rval = SVC_FIFO_TYPE_P_FRAME;    // p frame as default
    UINT8 IsBtmFld = 0U;
    UINT32 FrameIdx = SampleIdx;
    if (Mode == SVC_VIDEO_MODE_I_A_FLD_PER_SMP) {
        FrameIdx = SampleIdx >> 1U;
        if ((SampleIdx % 2U) != 0U) {
            IsBtmFld = 1U;
        }
    }
    if (FrameIdx == 0U) {
        // field_per_sample mode : IDR + P
        if (IsBtmFld == 0U) {
            Rval = SVC_FIFO_TYPE_IDR_FRAME;
        }
    } else {
        if (ClosedGOP != 0U) {
            FrameIdx += ((UINT32)M - 1U);
        }
        if ((FrameIdx % GOPSize) == 0U) {
            if (IsBtmFld == 0U) {
                // field_per_sample mode IDR + P
                Rval = SVC_FIFO_TYPE_IDR_FRAME;
            }
        } else if ((FrameIdx % (UINT32)N) == 0U) {
            // TODO: imprecise for pause&resume
            if (IsBtmFld == 0U) {
                // field_per_sample mode : I + P
                Rval = SVC_FIFO_TYPE_I_FRAME;
            }
        } else {
            if ((FrameIdx % (UINT32)M) != 0U) {
                Rval = SVC_FIFO_TYPE_B_FRAME;
            }
        }
    }
    return Rval;
}

