/**
 * @file Mp4DmxImpl.c
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
#include "Mp4DmxImpl.h"
#include "../FormatAPI.h"
#include "../ByteOp.h"
#include "../H264.h"

static UINT32 Mp4Dmx_GetFrameInfo(void *IdxMgr, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo)
{
    MP4_IDX_s *Mp4Idx;
    AmbaMisra_TypeCast(&Mp4Idx, &IdxMgr);
    AmbaMisra_TouchUnused(IdxMgr);
    return Mp4Idx_GetFrameInfo(Mp4Idx, TrackId, FrameNo, Direction, FrameInfo);
}

static UINT32 Mp4Dmx_ParseMoov(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, SVC_ISO_PRIV_INFO_s *IsoDmx, UINT64 ParseStartPos, UINT64 ParseEndPos)
{
    return IsoDmx_ParseMoov(&Movie->MediaInfo, Stream, IsoDmx, ParseStartPos, ParseEndPos, 0U);
}

/**
 *  Parse movie information of a mp4 clip.
 *
 *  @param [out] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Parse(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream)
{
    UINT32 Rval;
    SVC_MEDIA_INFO_s * const Media = &Movie->MediaInfo;
    UINT64 ParseEndPos;
    Media->Valid = 1U;
    Rval = S2F(Stream->Func->GetLength(Stream, &ParseEndPos));
    if (Rval == FORMAT_OK) {
        SVC_ISO_PRIV_INFO_s *IsoDmx = &Movie->IsoInfo;
        Media->Size = ParseEndPos;
        Rval = S2F(Stream->Func->Seek(Stream, 0, SVC_STREAM_SEEK_START));
        if (Rval == FORMAT_OK) {
            UINT64 ParsePos = 0;
            UINT32 BoxSize;
            UINT32 BoxTag;
            do {
                if (ParsePos == ParseEndPos) {
                    break;
                } else if (ParsePos > ParseEndPos) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s parse position incorrect!", __func__, NULL, NULL, NULL, NULL);
                    Rval = FORMAT_ERR_INVALID_FORMAT;
                } else {
                    /* Get Box Size, Tag */
                    Rval = IsoDmx_GetBoxHeader(Stream, &BoxSize, &BoxTag);
                    if (Rval == FORMAT_OK) {
                        if (BoxTag == TAG_FTYP) {
                            Rval = IsoDmx_ParseFtyp(Stream, IsoDmx, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
                        } else if (BoxTag == TAG_MOOV) {
                            Rval = Mp4Dmx_ParseMoov(Movie, Stream, IsoDmx, (ParsePos + SVC_ISO_BOX_SIZE_TAG_SIZE), (ParsePos + BoxSize));
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
                        } else if (BoxTag == TAG_FREE) {
                            Rval = S2F(Stream->Func->Seek(Stream, ((INT64) ParsePos + (INT64) BoxSize), SVC_STREAM_SEEK_START));
                            if (Rval != FORMAT_OK) {
                                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Seek failed in FREE!", __func__, NULL, NULL, NULL, NULL);
                            }
                        } else {
                            AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Fmp4Dmx_Parse: Unsupport tag %x!", BoxTag, 0U, 0U, 0U, 0U);
                            Rval = FORMAT_ERR_GENERAL_ERROR;
                        }
                        if (Rval == FORMAT_OK) {
                            ParsePos += BoxSize;
                        }
                    }
                }
            } while (Rval == FORMAT_OK);
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
 *  Process of demuxing a mp4 movie.
 *
 *  @param [in] Movie Movie information
 *  @param [in] Stream Stream handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] End Indicating the end of process
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Process(SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, MP4_IDX_s *Mp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT8 Direction, UINT8 End, UINT8 *Event)
{
    return IsoDmx_Process(&Movie->MediaInfo, Stream, Mp4Idx, FeedInfo, TargetTime, Direction, End, Event);
}

/**
 *  Open a mp4 demuxing format handler.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] MaxIdxNum Maximum index number
 *  @param [in] FeedInfo Frame feeder information
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Open(SVC_DMX_FORMAT_HDLR_s *Hdlr, MP4_IDX_s *Mp4Idx, UINT32 MaxIdxNum, SVC_FRAME_FEEDER_INFO_s *FeedInfo)
{
    UINT32 Rval = FORMAT_OK;
    SVC_MEDIA_INFO_s * const Media = Hdlr->Media;
    AmbaMisra_TouchUnused(Hdlr);
    if ((Media->Valid == 1U) && (Media->MediaType == SVC_MEDIA_INFO_MOVIE)) {
        SVC_MOVIE_INFO_s *Movie;
        AmbaMisra_TypeCast(&Movie, &Media);
        Rval = Mp4Idx_Create(ISO_IDX_MODE_READ, Movie, Hdlr->Stream, MaxIdxNum, Mp4Idx);
        if (Rval == FORMAT_OK) {
            Rval = IsoDmx_InitFeedInfo(Movie, FeedInfo, Mp4Dmx_GetFrameInfo, Hdlr->Direction, Hdlr->Speed);
            // should never error, so no need to release Fmp4Idx on error
        }
    } else {
        AmbaPrint_ModulePrintUInt5(SVC_DEMUXER_PRINT_MODULE_ID, "Open: Invalid media %u or media type %u", Media->Valid, Media->MediaType, 0U, 0U, 0U);
        Rval = FORMAT_ERR_INVALID_FORMAT;
    }
    if (Rval == FORMAT_ERR_INVALID_FORMAT) {
        Media->Valid = 0U;
    }
    return Rval;
}

/**
 *  Close a mp4 demuxing format handler.
 *
 *  @param [in] Mp4Idx Mp4 index
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Close(MP4_IDX_s *Mp4Idx)
{
    return Mp4Idx_Delete(Mp4Idx);
}

/**
 *  Seek and set the start time of demuxing.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] FeedInfo Frame feeder information
 *  @param [in] TargetTime Target time to process
 *  @param [in] Direction Direction of process
 *  @param [in] Speed Processing speed after seeking
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_Seek(SVC_DMX_FORMAT_HDLR_s *Hdlr, MP4_IDX_s *Mp4Idx, SVC_FRAME_FEEDER_INFO_s *FeedInfo, UINT64 TargetTime, UINT32 Direction, UINT32 Speed)
{
    return IsoDmx_Seek(Hdlr, Mp4Idx, FeedInfo, TargetTime, Direction, Speed, Mp4Dmx_GetFrameInfo);
}

/**
 *  Feeding a frame from the mp4 file.
 *
 *  @param [in] Hdlr Demuxing format handler
 *  @param [in] Mp4Idx Mp4 index
 *  @param [in] TrackId Track id
 *  @param [in] TargetTime The time of the target frame
 *  @param [in] FrameType Frame type
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Dmx_FeedFrame(SVC_DMX_FORMAT_HDLR_s *Hdlr, MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT64 TargetTime, UINT8 FrameType)
{
    return IsoDmx_FeedFrame(Hdlr, Mp4Idx, TrackId, TargetTime, FrameType, Mp4Dmx_GetFrameInfo);
}

