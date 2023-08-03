/**
 * @file DemuxerImpl.c
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
#include "../FormatAPI.h"

/**
 * Reset the tracks
 *
 * @param [in] Track The track information
 * @param [in] Fifo The fifo handler
 */
static void SvcDemuxer_ResetTrack(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, const SVC_FIFO_HDLR_s *Fifo)
{
    Track->FrameNo = 0;
    if (Fifo != NULL) {
        Track->DTS = Track->InitDTS;
    } else {
        Track->DTS = SVC_FORMAT_MAX_TIMESTAMP;
    }
}

/**
 * Reset the movie object
 *
 * @param [in] Movie The movie object information
 */
static void SvcDemuxer_ResetMovieInfo(SVC_MOVIE_INFO_s *Movie)
{
    UINT32 i;
    for (i = 0; i < Movie->VideoTrackCount; i++) {
        SvcDemuxer_ResetTrack(&Movie->VideoTrack[i].Info, Movie->VideoTrack[i].Info.Fifo);
    }
    for (i = 0; i < Movie->AudioTrackCount; i++) {
        SvcDemuxer_ResetTrack(&Movie->AudioTrack[i].Info, Movie->AudioTrack[i].Info.Fifo);
    }
    for (i = 0; i < Movie->TextTrackCount; i++) {
        SvcDemuxer_ResetTrack(&Movie->TextTrack[i].Info, Movie->TextTrack[i].Info.Fifo);
    }
}

/**
 * Reset the media information
 *
 * @param [in] Media The media information want to reset.
 */
static void SvcDemuxer_ResetMediaInfo(const SVC_MEDIA_INFO_s *Media)
{
    switch (Media->MediaType) {
    case SVC_MEDIA_INFO_MOVIE:
        {
            SVC_MOVIE_INFO_s *Movie;
            AmbaMisra_TypeCast(&Movie, &Media);
            SvcDemuxer_ResetMovieInfo(Movie);
        }
        break;
    default:
        /* SVC_MEDIA_INFO_IMAGE */
        break;
    }
}

/**
 * Update information of the track
 *
 * @param [in] Track The track information
 * @param [in] FrameCount The frame counter
 * @param [in] Offset Offset of DTS
 */
void SvcDemuxer_UpdateTrack(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, INT32 FrameCount, INT32 Offset)
{
    Track->FrameNo += (UINT32) FrameCount;
    Track->DTS = Track->DTS + (UINT64) Offset;
}

/**
 * Print out the pipe information
 *
 * @param [in] Pipe pipe The demuxer pipe
 * @param [in] Prefix The prefix string
 */
void SvcDemuxer_PrintPipeInfo(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, const char *Prefix)
{
    char Buffer[256] = {'\0'};
    UINT8 i;
    AmbaUtility_StringCopy(Buffer, sizeof(Buffer), Prefix);
    for (i = 0; i < Pipe->FormatCount; i++) {
        const SVC_DMX_FORMAT_HDLR_s * const Format = Pipe->Format[i];
        if (i > 0U) {
            AmbaUtility_StringAppend(Buffer, (UINT32)sizeof(Buffer), " ");
        }
        switch (Format->Media->MediaType) {
        case SVC_MEDIA_INFO_MOVIE:
            {
                const SVC_MOVIE_INFO_s *Movie;
                AmbaMisra_TypeCast(&Movie, &Format->Media);
                SvcFormat_GetMovieInfoString(Movie, Buffer, (UINT32)sizeof(Buffer));
            }
            break;
        default:
            /* SVC_MEDIA_INFO_IMAGE */
            {
                const SVC_IMAGE_INFO_s *Image;
                AmbaMisra_TypeCast(&Image, &Format->Media);
                SvcFormat_GetImageInfoString(Image, Buffer, (UINT32)sizeof(Buffer));
            }
            break;
        }
    }
    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s", Buffer, NULL, NULL, NULL, NULL);
    AmbaMisra_TouchUnused(Pipe);
}

/**
 * Start the demuxer pipe
 *
 * @param [in] Pipe The demuxer pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_StartPipe(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = DEMUXER_OK;
    UINT32 i;
    for (i = 0; i < Pipe->FormatCount; i++) {
        SVC_DMX_FORMAT_HDLR_s * const Format = Pipe->Format[i];
        SvcDemuxer_ResetMediaInfo(Format->Media);
        Rval = Format->Func->Open(Format);
        if (Rval != DEMUXER_OK) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Open format handler fail!", __func__, NULL, NULL, NULL, NULL);
            break;
        }
    }
    if (Rval == DEMUXER_OK) {
        Pipe->State = SVC_DEMUXER_STATE_RUNNING;
    } else {
        if (Rval == DEMUXER_ERR_IO_ERROR) {
            UINT32 j, Ret;
            for (j = 0; j < i; j++) {
                SVC_DMX_FORMAT_HDLR_s * const Format = Pipe->Format[j];
                Ret = Format->Func->Close(Format);
                if (Ret != DEMUXER_OK) {
                    AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Fmt->Func->Close() failed!", __func__, NULL, NULL, NULL, NULL);
                    Rval = Ret;
                    if (Rval != DEMUXER_ERR_IO_ERROR) {
                        // abort on fatal error
                        break;
                    }
                }
            }
        }
        Pipe->State = SVC_DEMUXER_STATE_ERROR;  /* to avoid closing again */
    }
    return Rval;
}

/**
 * Process the media of the format to end
 *
 * @param [in] Pipe The demuxer pipe
 * @param [in] Format The format handler
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
static UINT32 SvcDemuxer_ProcessToEnd(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe, SVC_DMX_FORMAT_HDLR_s *Format)
{
    UINT32 Rval = DEMUXER_OK;
    UINT8 Event;
    const SVC_MEDIA_INFO_s *Media = Format->Media;
    AmbaMisra_TouchUnused(Pipe);
    switch (Media->MediaType) {
    case SVC_MEDIA_INFO_MOVIE:
        Rval = Format->Func->Process(Format, (Format->Direction == SVC_FORMAT_DIR_FORWARD) ? SVC_FORMAT_MAX_TIMESTAMP : 0ULL, &Event);
        break;
    default:
        Event = SVC_DEMUXER_EVENT_REACH_END;
        break;
    }
    if (Rval == DEMUXER_OK) {
        if (Event != SVC_DEMUXER_EVENT_REACH_END) {
            AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Did not reach end!", __func__, NULL, NULL, NULL, NULL);
        }
    } else {
        AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Format->Func->Process() failed!", __func__, NULL, NULL, NULL, NULL);
    }
    return Rval;
}

/**
 * Entry when pipe end
 *
 * @param [in] Pipe The demuxer pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_OnPipeEnd(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = DEMUXER_OK;
    UINT32 i;
    if (Pipe->State == SVC_DEMUXER_STATE_RUNNING) {
        /* master has reached end, process others to end */
        for (i = 1; i < Pipe->FormatCount; i++) {
            SVC_DMX_FORMAT_HDLR_s * const Fmt = Pipe->Format[i];
            Rval = SvcDemuxer_ProcessToEnd(Pipe, Fmt);
            if (Rval != DEMUXER_OK) {
                break;
            }
        }
    }
    if (Rval == DEMUXER_OK) {
        char Buffer[32];
        UINT32 pArgs[2];
        UINT32 Len;
        pArgs[0] = Pipe->Hdlr.PipeId;
        pArgs[1] = Pipe->State;
        Len = AmbaUtility_StringPrintUInt32(Buffer, (UINT32)sizeof(Buffer), "DMXER End [Pipe#%u:0x%u]: ", 2, pArgs);
        //sprintf(Buffer, "DMXER End [%p:0x%x]: ", Pipe, Pipe->State);
        SvcDemuxer_PrintPipeInfo(Pipe, Buffer);
        /* close all formats */
        for (i = 0; i < Pipe->FormatCount; i++) {
            SVC_DMX_FORMAT_HDLR_s * const Fmt = Pipe->Format[i];
            UINT32 Ret = Fmt->Func->Close(Fmt);
            if (Ret != DEMUXER_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Close format heandler fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = Ret;
                if (Rval != DEMUXER_ERR_IO_ERROR) {
                    // abort on fatal error
                    break;
                }
            }
        }
        if (Rval == DEMUXER_OK) {
            Pipe->State = SVC_DEMUXER_STATE_END;
        } else {
            Pipe->State = SVC_DEMUXER_STATE_ERROR;
        }
        AmbaMisra_TouchUnused(&Len);
    }
    return Rval;
}

/**
 * Entry when pipe got errors
 *
 * @param [in] Pipe The demuxer pipe
 * @return 0 - OK, others - DEMUXER_ERR_XXXX
 */
UINT32 SvcDemuxer_OnPipeError(SVC_DEMUXER_PIPE_HDLR_IMPL_s *Pipe)
{
    UINT32 Rval = DEMUXER_OK, Len;
    char Buffer[32];
    UINT32 pArgs[2];
    if (Pipe->State == SVC_DEMUXER_STATE_RUNNING) {
        UINT32 i;
        /* close all format */
        for (i = 0; i < Pipe->FormatCount; i++) {
            SVC_DMX_FORMAT_HDLR_s * const Fmt = Pipe->Format[i];
            UINT32 Ret = Fmt->Func->Close(Fmt);
            if (Ret != DEMUXER_OK) {
                AmbaPrint_ModulePrintStr5(SVC_DEMUXER_PRINT_MODULE_ID, "%s Close format handler fail!", __func__, NULL, NULL, NULL, NULL);
                Rval = Ret;
                if (Rval != DEMUXER_ERR_IO_ERROR) {
                    // abort on fatal error
                    break;
                }
            }
        }
    }
    Pipe->State = SVC_DEMUXER_STATE_ERROR;
    pArgs[0] = Pipe->Hdlr.PipeId;
    pArgs[1] = Pipe->State;
    Len = AmbaUtility_StringPrintUInt32(Buffer, (UINT32)sizeof(Buffer), "DMXER Error [Pipe#%u:0x%x]: ", 2, pArgs);
    SvcDemuxer_PrintPipeInfo(Pipe, Buffer);
    AmbaMisra_TouchUnused(&Len);
    return Rval;
}

/**
 * Initialize the related parameters of movie object
 *
 * @param [in] Format The format handler
 * @param [in] Func The interface of format operations
 * @param [in] Stream The stream handler
 */
void SvcDemuxer_InitMovieDmxHdlr(SVC_DMX_FORMAT_HDLR_s *Format, SVC_DMX_FORMAT_s *Func, SVC_STREAM_HDLR_s *Stream)
{
    Format->Func = Func;
    Format->Stream = Stream;
    Format->Direction = SVC_FORMAT_DIR_FORWARD;
    Format->Speed = 1;
    Format->End = 1U;
}

/**
 * Initialize the related parameters of image object
 *
 * @param [in] Format The format handler
 * @param [in] Func The interface of format operations
 * @param [in] Stream The stream handler
 */
void SvcDemuxer_InitImageDmxHdlr(SVC_DMX_FORMAT_HDLR_s *Format, SVC_DMX_FORMAT_s *Func, SVC_STREAM_HDLR_s *Stream)
{
    Format->Func = Func;
    Format->Stream = Stream;
}

