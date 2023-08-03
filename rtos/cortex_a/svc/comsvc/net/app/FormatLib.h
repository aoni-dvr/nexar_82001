/**
 *  @file FormatLib.h
 *
 * Copyright (c) [2020] Ambarella International LP
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
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Header File of Format Common Functions for UT
 *
 */
#ifndef FORMATLIB_H
#define FORMATLIB_H

#include <format/AmbaFormat.h>

#define UT_ERR_INVALID_ARG      (0x00000001U)
#define UT_ERR_DATA_EMPTY       (0x00000002U)
#define UT_ERR_DATA_FULL        (0x00000003U)
#define UT_ERR_OS_ERROR         (0x00000004U)
#define UT_ERR_IO_ERROR         (0x00000005U)
#define UT_ERR_GENERAL_ERROR    (0x00000006U)

#define UT_MEDIA_TRACK_TYPE_VIDEO   (0x01U) /**< The video track type */
#define UT_MEDIA_TRACK_TYPE_AUDIO   (0x02U) /**< The audio track type */
#define UT_MEDIA_TRACK_TYPE_TEXT    (0x03U) /**< The text track type */

static inline UINT32 K2U(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        Rval = UT_ERR_OS_ERROR;
    }
    return Rval;
}

static inline UINT32 C2U(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == CFS_ERR_0000) {
            Rval = UT_ERR_INVALID_ARG;
        } else if (Ret == CFS_ERR_0004) {
            Rval = UT_ERR_OS_ERROR;
        } else if (Ret == CFS_ERR_0002) {
            Rval = UT_ERR_IO_ERROR;
        } else {
            Rval = UT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 F2U(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == FIFO_ERR_0000) {
            Rval = UT_ERR_INVALID_ARG;
        } else if (Ret == FIFO_ERR_0007) {
            Rval = UT_ERR_OS_ERROR;
        } else if (Ret == FIFO_ERR_0002) {
            Rval = UT_ERR_DATA_EMPTY;
        } else if ((Ret == FIFO_ERR_0003) || (Ret == FIFO_ERR_0005)) {
            Rval = UT_ERR_DATA_FULL;
        } else {
            Rval = UT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 M2U(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == MUXER_ERR_0001) {
            Rval = UT_ERR_INVALID_ARG;
        } else if (Ret == MUXER_ERR_0003) {
            Rval = UT_ERR_OS_ERROR;
        } else if (Ret == MUXER_ERR_0004) {
            Rval = UT_ERR_IO_ERROR;
        } else {
            Rval = UT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 D2U(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == DEMUXER_ERR_0001) {
            Rval = UT_ERR_INVALID_ARG;
        } else if (Ret == DEMUXER_ERR_0003) {
            Rval = UT_ERR_OS_ERROR;
        } else if (Ret == DEMUXER_ERR_0004) {
            Rval = UT_ERR_IO_ERROR;
        } else {
            Rval = UT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 S2U(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == STREAM_ERR_0001) {
            Rval = UT_ERR_INVALID_ARG;
        } else if (Ret == STREAM_ERR_0002) {
            Rval = UT_ERR_OS_ERROR;
        } else if (Ret == STREAM_ERR_0003) {
            Rval = UT_ERR_IO_ERROR;
        } else {
            Rval = UT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT64 FORMAT_LIB_TIME_TO_DTS(UINT64 Time, UINT32 TimeScale)
{
    return ((Time / 1000U) * (UINT64)TimeScale) + ((((Time % 1000U) * (UINT64)TimeScale) + 999U) / 1000U);
}

static inline UINT64 FORMAT_LIB_DTS_TO_TIME(UINT64 DTS, UINT32 TimeScale)
{
    return (((DTS / (UINT64)TimeScale) * 1000U) + (((DTS % (UINT64)TimeScale) * 1000U) / TimeScale));
}

/**
 * The configuration of a media track for muxing
 */
typedef struct {
    UINT32 MediaId;             /**< The media type of a track (The id is a media ID. See AMBA_FORMAT_MID_e.) */
    UINT32 TimeScale;           /**< The ticks per second */
    UINT32 TimePerFrame;        /**< The ticks per frame */
    UINT32 InitDelay;           /**< Initial delay time of a track (ms) */
    AMBA_FIFO_HDLR_s *Fifo;      /**< The FIFO handler of a track (Each track has an individual FIFO handler.) */
    UINT8 *BufferBase;          /**< The start address of a FIFO buffer (Users push data into a FIFO; the FIFO will write the data to its buffer.) */
    UINT8 *BufferLimit;         /**< The end address of a FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
} MUX_MEDIA_TRACK_GENERAL_CFG_s;

/**
 * The configuration of a video track for muxing
 */
typedef struct {
    MUX_MEDIA_TRACK_GENERAL_CFG_s Info;
    UINT32 GOPSize;             /**< The number of pictures between IDR pictures */
    UINT32 CodecTimeScale;      /**< The time scale of the codec (TODO: It needs to be got from H264 bitstream; it is not configurable.) */
    UINT16 Width;               /**< Picture width */
    UINT16 Height;              /**< Picture height */
    UINT16 M;                   /**< The number of pictures between reference pictures (IDR, I, P) */
    UINT16 N;                   /**< The number of pictures between I pictures */
    UINT8 IsDefault;            /**< The flag indicating the track is the default video track */
    UINT8 Mode;                 /**< The picture mode of a video (It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample. See AMBA_VIDEO_MODE_s.) */
    UINT8 ClosedGOP;            /**< The flag indicating if a GOP structure is closed (The sequence pattern of a closed GOP structure is IPBBPBB, and the one of an open GOP structure is IBBPBB. If the functions of resuming or auto splitting a video are enabled, the value is always false, open GOP.) */
} MUX_VIDEO_TRACK_CFG_s;

/**
 * The configuration of an audio track for muxing
 */
typedef struct {
    MUX_MEDIA_TRACK_GENERAL_CFG_s Info;
    UINT32 SampleRate;          /**< The sample rate (Hz) of an audio track */
    UINT8 IsDefault;            /**< The flag indicating the track is the default audio track  */
    UINT8 Channels;             /**< The number of audio channels */
    UINT8 BitsPerSample;        /**< Bits per audio sample (e.g., 8 bits and 16 bits) */
} MUX_AUDIO_TRACK_CFG_s;

/**
 * The configuration of a text track for muxing
 */
typedef struct {
    MUX_MEDIA_TRACK_GENERAL_CFG_s Info;
    UINT8 IsDefault;            /**< The flag indicating the track is the default text track  */
} MUX_TEXT_TRACK_CFG_s;

/**
 * The configuration of a Movie Info object for muxing
 */
typedef struct {
    MUX_VIDEO_TRACK_CFG_s VideoTrack[AMBA_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];    /**< Video track configurations */
    MUX_AUDIO_TRACK_CFG_s AudioTrack[AMBA_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];    /**< Audio track configurations */
    MUX_TEXT_TRACK_CFG_s TextTrack[AMBA_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];      /**< Text track configurations */
    UINT8 VideoTrackCount;      /**< The number of video tracks in a Movie Info object */
    UINT8 AudioTrackCount;      /**< The number of audio tracks in a Movie Info object */
    UINT8 TextTrackCount;       /**< The number of text tracks in a Movie Info object */
} MUX_MOVIE_INFO_CFG_s;

/**
 * The configuration of a media track for demuxing
 */
typedef struct {
    AMBA_FIFO_HDLR_s *Fifo;      /**< The FIFO handler of a track (Each track has an individual FIFO handler.) */
    UINT8 *BufferBase;          /**< The start address of a FIFO buffer (Users push data into a FIFO; the FIFO will write the data to its buffer.) */
    UINT8 *BufferLimit;         /**< The end address of a FIFO buffer (FIFO size = FIFO buffer limit - FIFO buffer base) */
} DMX_MEDIA_TRACK_GENERAL_CFG_s;

/**
 * The configuration of a Demuxer video track
 */
typedef struct {
    DMX_MEDIA_TRACK_GENERAL_CFG_s Info;
    UINT8 Resv[4];      /**< Reserve */
} DMX_VIDEO_TRACK_CFG_s;

/**
 * The configuration of a Demuxer audio track
 */
typedef struct {
    DMX_MEDIA_TRACK_GENERAL_CFG_s Info;
    UINT8 Resv[4];      /**< Reserve */
} DMX_AUDIO_TRACK_CFG_s;

/**
 * The configuration of a Demuxer text track
 */
typedef struct {
    DMX_MEDIA_TRACK_GENERAL_CFG_s Info;
    UINT8 Resv[4];      /**< Reserve */
} DMX_TEXT_TRACK_CFG_s;

/**
 * The configuration of a Movie Info object for demuxing
 */
typedef struct {
    DMX_VIDEO_TRACK_CFG_s VideoTrack[AMBA_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];    /**< Video track configurations */
    DMX_AUDIO_TRACK_CFG_s AudioTrack[AMBA_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];    /**< Audio track configurations */
    DMX_TEXT_TRACK_CFG_s TextTrack[AMBA_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];      /**< Text track configurations */
    UINT32 InitTime;    /**< The initial time (ms) of the media */
} DMX_MOVIE_INFO_CFG_s;

typedef struct {
    UINT32 IdrInterval;
    UINT32 TimeScale;
    UINT32 TickPerPicture;
    UINT32 N;
    UINT32 M;
    UINT32 VideoWidth;
    UINT32 VideoHeight;
} VIDEO_USER_DATA_s;

typedef struct {
    UINT32 SampleRate;
    UINT16 Channels;
    UINT16 BitsPerSample;
    UINT16 Frame_Size;
    UINT32 Bitrate;
    UINT16 reserve;
} AUDIO_USER_DATA_s;

UINT32 FormatLib_ResetMuxMediaInfo(const AMBA_MEDIA_INFO_s *Media);

UINT32 FormatLib_ResetDmxMediaInfo(const AMBA_MEDIA_INFO_s *Media);

UINT8 FormatLib_GetShortestTrack(AMBA_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, AMBA_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, AMBA_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx);

UINT8 FormatLib_GetLongestTrack(AMBA_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, AMBA_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, AMBA_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx);

void FormatLib_ResetPTS(AMBA_VIDEO_TRACK_INFO_s *Video);

UINT32 FormatLib_InitMuxMovieInfo(AMBA_MOVIE_INFO_s *Movie, MUX_MOVIE_INFO_CFG_s *Config);

UINT32 FormatLib_InitDmxMovieInfo(AMBA_MOVIE_INFO_s *Movie, const DMX_MOVIE_INFO_CFG_s *Config);

UINT32 FormatLib_Allocate(UINT32 Size, UINT8 **BufAddr);

UINT32 FormatLib_WaitTaskComplete(AMBA_KAL_TASK_t *Task);

#endif

