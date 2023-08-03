/**
 * @file WrpFormatLib.h
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
#ifndef WRP_FORMATLIB_H
#define WRP_FORMATLIB_H

#include "format/SvcFormat.h"
#include "AmbaMux.h"
#include "AmbaExifFormatter.h"

#define AMBA_MP4FMT_ERR_DATA_EMPTY       (0x00000004U)  /**< Data empty */
#define AMBA_MP4FMT_ERR_DATA_FULL        (0x00000005U)  /**< Data full */

#define WRP_MEDIA_TRACK_TYPE_VIDEO   (0x01U)    /**< The video track type */
#define WRP_MEDIA_TRACK_TYPE_AUDIO   (0x02U)    /**< The audio track type */
#define WRP_MEDIA_TRACK_TYPE_TEXT    (0x03U)    /**< The text track type */

#define WRP_FORMAT_WAIT_TIMEOUT (30000U)        /**< Time out */

static inline UINT32 WRAP2W(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        Rval = AMBA_MP4FMT_ERR_FATAL;
    }
    return Rval;
}

static inline UINT32 K2W(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        Rval = AMBA_MP4FMT_ERR_FATAL;
    }
    return Rval;
}

static inline UINT32 F2W(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == FIFO_ERR_0000) {
            Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
        } else if (Ret == FIFO_ERR_0002) {
            Rval = AMBA_MP4FMT_ERR_DATA_EMPTY;
        } else if (Ret == FIFO_ERR_0003) {
            Rval = AMBA_MP4FMT_ERR_DATA_FULL;
        } else {
            Rval = AMBA_MP4FMT_ERR_FATAL;
        }
    }
    return Rval;
}

static inline UINT32 M2W(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == MUXER_ERR_0000) {
            Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
        } else if (Ret == MUXER_ERR_0002) {
            Rval = AMBA_MP4FMT_ERR_IO_ERROR;
        } else {
            Rval = AMBA_MP4FMT_ERR_FATAL;
        }
    }
    return Rval;
}

static inline UINT32 S2W(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == STREAM_ERR_0000) {
            Rval = AMBA_MP4FMT_ERR_INVALID_ARG;
        } else if (Ret == STREAM_ERR_0002) {
            Rval = AMBA_MP4FMT_ERR_IO_ERROR;
        } else {
            Rval = AMBA_MP4FMT_ERR_FATAL;
        }
    }
    return Rval;
}

static inline UINT32 WRAP2E(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        Rval = AMBA_EXIF_ERR_FATAL;
    }
    return Rval;
}

static inline UINT32 K2E(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        Rval = AMBA_EXIF_ERR_FATAL;
    }
    return Rval;
}

static inline UINT32 F2E(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == FIFO_ERR_0000) {
            Rval = AMBA_EXIF_ERR_INVALID_ARG;
        } else {
            Rval = AMBA_EXIF_ERR_FATAL;
        }
    }
    return Rval;
}

static inline UINT32 M2E(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == MUXER_ERR_0000) {
            Rval = AMBA_EXIF_ERR_INVALID_ARG;
        } else if (Ret == MUXER_ERR_0002) {
            Rval = AMBA_EXIF_ERR_IO_ERROR;
        } else {
            Rval = AMBA_EXIF_ERR_FATAL;
        }
    }
    return Rval;
}

static inline UINT32 S2E(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == STREAM_ERR_0000) {
            Rval = AMBA_EXIF_ERR_INVALID_ARG;
        } else if (Ret == STREAM_ERR_0002) {
            Rval = AMBA_EXIF_ERR_IO_ERROR;
        } else {
            Rval = AMBA_EXIF_ERR_FATAL;
        }
    }
    return Rval;
}

static inline UINT32 V2S(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        Rval = STREAM_ERR_0001;
    }
    return Rval;
}

static inline UINT64 WRP_FORMAT_LIB_DTS_TO_TIME(UINT64 DTS, UINT32 TimeScale)
{
    return (((DTS / (UINT64)TimeScale) * 1000U) + (((DTS % (UINT64)TimeScale) * 1000U) / TimeScale));
}

/**
 * Reset muxer movie information
 *
 * @param [out] Movie Movie information
 */
void WrpFormatLib_ResetMuxMovieInfo(SVC_MOVIE_INFO_s *Movie);

/**
 * Get shortest track
 *
 * @param [in] VideoTracks Video track information
 * @param [in] VideoTrackCount Video track counter
 * @param [in] AudioTracks Audio track information
 * @param [in] AudioTrackCount Audio track counter
 * @param [in] TextTracks Text track information
 * @param [in] TextTrackCount Text track counter
 * @param [out] TrackIdx The returned track index
 * @return The type of the shortest track
 */
UINT8 WrpFormatLib_GetShortestTrack(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx);

/**
 * Normalize media's timescale
 *
 * @param [in,out] Media Media information
 * @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 WrpFormatLib_NormalizeTimeScale(const SVC_MEDIA_INFO_s *Media);

#endif

