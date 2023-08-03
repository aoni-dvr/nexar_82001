/**
 * @file FormatAPI.h
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
#ifndef CONTAINER_FORMAT_API_H
#define CONTAINER_FORMAT_API_H

#include "format/SvcFormat.h"
#include "Muxer.h"

#define SVC_FORMAT_WAIT_TIMEOUT    (30000U) /**< Time out */

#define SVC_AMBA_BOX_TYPE_MW            (0U)    /**< MW's AMBA box */
#define SVC_AMBA_BOX_TYPE_SVC           (1U)    /**< SVC's AMBA box */
#define SVC_AMBA_BOX_DEFAULT_TYPE       SVC_AMBA_BOX_TYPE_MW    /**< Default AMBA box type */
#define SVC_AMBA_BOX_DEFAULT_MAIN_VER   (3U)    /**< Default AMBA box main version */
#define SVC_AMBA_BOX_DEFAULT_SUB_VER    (1U)    /**< Default AMBA box sub version */
#define SVC_AMBA_BOX_MAX_TYPE           (0x3U)  /**< 2 bits for type */
#define SVC_AMBA_BOX_MAX_MAIN_VER       (0x7U)  /**< 3 bits for main version */
#define SVC_AMBA_BOX_MAX_SUB_VER        (0x7U)  /**< 3 bits for sub version */
#define SVC_AMBA_BOX_MAGIC_NUM          (0x12345678U)   /**< AMBA box magic number */

#define FORMAT_OK                    OK     /**< Execution OK */
#define FORMAT_ERR_GENERAL_ERROR    (1U)    /**< General error */
#define FORMAT_ERR_IO_ERROR         (2U)    /**< Stream io error */
#define FORMAT_ERR_INVALID_ARG      (3U)    /**< Incorrect value range */
#define FORMAT_ERR_OS_ERROR         (4U)    /**< OS error */
#define FORMAT_ERR_DATA_EMPTY       (5U)    /**< Data empty */
#define FORMAT_ERR_FIFO_FULL        (6U)    /**< FIFO full */
#define FORMAT_ERR_FIFO_ERROR       (7U)    /**< FIFO error */
#define FORMAT_ERR_NOT_ENOUGH_BYTES (8U)    /**< Not enough bytes to read from SvcFormat_GetXXXX() */
#define FORMAT_ERR_INVALID_FORMAT   (9U)    /**< Invalid format */

#define FORMAT_EVENT_NONE           (0x00U) /**< No special event occurs. */
#define FORMAT_EVENT_REACH_END      (0x01U) /**< The event raised when Muxer completes the muxing */
#define FORMAT_EVENT_REACH_LIMIT    (0x02U) /**< The event raised in reaching the limitation of media time or file size */

#define SVC_MEDIA_TRACK_TYPE_VIDEO  (0x01U) /**< The video track type */
#define SVC_MEDIA_TRACK_TYPE_AUDIO  (0x02U) /**< The audio track type */
#define SVC_MEDIA_TRACK_TYPE_TEXT   (0x03U) /**< The text track type */

#define SVC_MUXER_PRINT_MODULE_ID   ((UINT16)(MUXER_ERR_BASE >> 16U))   /**< Module ID for AmbaPrint */
#define SVC_DEMUXER_PRINT_MODULE_ID ((UINT16)(DEMUXER_ERR_BASE >> 16U)) /**< Module ID for AmbaPrint */

/**
 *  AMBA box general track information
 */
typedef struct {
    UINT64 InitDTS;         /**< The initial DTS, based on the original timescale */
    UINT64 Duration;        /**< The duration, based on the original timescale */
    UINT32 TimePerFrame;    /**< The original time per frame */
    UINT32 TimeScale;       /**< The original time scale */
    UINT32 FrameCount;      /**< The frame count */
    UINT8 TrackType;        /**< The track type */
} SVC_AMBA_BOX_TRACK_GENERAL_INFO_s;

/**
 *  AMBA box video track information
 */
typedef struct {
    SVC_AMBA_BOX_TRACK_GENERAL_INFO_s Info; /**< General information */
    UINT32 GOPSize;     /**< Gop size */
    UINT32 TrickRecDen; /**< Denominator of TrickRec */
    UINT32 TrickRecNum; /**< Numerator of TrickRec */
    UINT16 PixelArX;    /**< Pixel aspect ration X */
    UINT16 PixelArY;    /**< Pixel aspect ration Y */
    UINT16 M;           /**< M */
    UINT16 N;           /**< N */
    UINT8 Mode;         /**< Mode */
    UINT8 ClosedGOP;    /**< Closed gop */
    UINT8 ColorStyle;   /**< Color style */
    UINT8 IsVFR;        /**< IsVFR */
    UINT8 IsDefault;    /**< IsDefault */
} SVC_AMBA_BOX_VIDEO_TRACK_INFO_s;

/**
 *  AMBA box audio track information
 */
typedef struct {
    SVC_AMBA_BOX_TRACK_GENERAL_INFO_s Info; /**< General information */
    UINT8 IsDefault;    /**< IsDefault */
    UINT8 Endian;       /**< Endian */
} SVC_AMBA_BOX_AUDIO_TRACK_INFO_s;

/**
 *  AMBA box text track information
 */
typedef struct {
    SVC_AMBA_BOX_TRACK_GENERAL_INFO_s Info; /**< General information */
    UINT8 IsDefault;            /**< The flag indicating the track is the default text track  */
} SVC_AMBA_BOX_TEXT_TRACK_INFO_s;

/**
 *  AMBA box information
 */
typedef struct {
    UINT8 Type;     /**< Type */
    UINT8 MainVer;  /**< Main version */
    UINT8 SubVer;   /**< Sub version */
    UINT8 VideoTrackCount;  /**< Video track count */
    UINT8 AudioTrackCount;  /**< Audio track count */
    UINT8 TextTrackCount;   /**< Text track count */
    SVC_AMBA_BOX_VIDEO_TRACK_INFO_s VideoTrack[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];   /**< Video tracks */
    SVC_AMBA_BOX_AUDIO_TRACK_INFO_s AudioTrack[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];   /**< Audio tracks */
    SVC_AMBA_BOX_TEXT_TRACK_INFO_s TextTrack[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA];      /**< Text tracks */
} SVC_AMBA_BOX_INFORMATION_s;

/**
 * Frame information
 */
typedef struct {
    UINT8 FrameType;        /**< Frame type (See UINT32.) */
    UINT32 FrameSize;       /**< Frame size (byte) */
    UINT64 FramePos;        /**< Frame position */
    UINT64 PTS;             /**< Frame PTS */
    UINT64 DTS;             /**< Frame DTS */
    UINT32 TimePerFrame;    /**< TimePerFrame */
} SVC_FRAME_INFO_s;

/**
 * Frame feeder parameters
 */
typedef struct {
    UINT8 *VPS; /**< VPS */
    UINT8 *SPS; /**< SPS */
    UINT8 *PPS; /**< PPS */
    UINT16 VPSLen;  /**< VPS length */
    UINT16 SPSLen;  /**< SPS length */
    UINT16 PPSLen;  /**< PPS length */
} SVC_FEED_FRAME_PARAM_s;

//#define TEST_FEED
#define CLOCK   (90000U)        /**< The standard clock frequency of the system (It is 90KHz in the system.) */
static inline UINT64 NORMALIZE_TO_TIMESCALE(UINT64 X, UINT64 Y, UINT64 Z)   {return (X == 0U) ? (0U) : (((UINT64)(Y) * (Z)) / (X));}
static inline UINT64 NORMALIZE_CLOCK(UINT64 X, UINT64 Y)   {return ((Y) == 0U)? (0U) : (((X) * CLOCK) / (Y));}   /**< Normalize the codec clock to DSP Clock. */

static inline UINT32 W2F(UINT32 Ret)
{
    UINT32 Rval = FORMAT_OK;
    if (Ret != OK) {
        Rval = FORMAT_ERR_GENERAL_ERROR;
    }
    return Rval;
}

static inline UINT32 K2F(UINT32 Ret)
{
    UINT32 Rval = FORMAT_OK;
    if (Ret != OK) {
        Rval = FORMAT_ERR_OS_ERROR;
    }
    return Rval;
}

static inline UINT32 S2F(UINT32 Ret)
{
    UINT32 Rval = FORMAT_OK;
    if (Ret != OK) {
        if (Ret == STREAM_ERR_0000) {
            Rval = FORMAT_ERR_INVALID_ARG;
        } else if (Ret == STREAM_ERR_0002) {
            Rval = FORMAT_ERR_IO_ERROR;
        } else {
            Rval = FORMAT_ERR_GENERAL_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 F2F(UINT32 Ret)
{
    UINT32 Rval = FORMAT_OK;
    if (Ret != OK) {
        if (Ret == FIFO_ERR_0000) {
            Rval = FORMAT_ERR_INVALID_ARG;
        } else if (Ret == FIFO_ERR_0002) {
            Rval = FORMAT_ERR_DATA_EMPTY;
        } else if (Ret == FIFO_ERR_0003) {
            Rval = FORMAT_ERR_FIFO_FULL;
        } else {
            Rval = FORMAT_ERR_FIFO_ERROR;
        }
    }
    return Rval;
}

static inline UINT32 F2M(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == FORMAT_ERR_INVALID_ARG) {
            Rval = MUXER_ERR_0000;
        } else if (Ret == FORMAT_ERR_IO_ERROR) {
            Rval = MUXER_ERR_0002;
        } else {
            Rval = MUXER_ERR_0001;
        }
    }
    return Rval;
}

static inline UINT32 F2D(UINT32 Ret)
{
    UINT32 Rval = OK;
    if (Ret != OK) {
        if (Ret == FORMAT_ERR_INVALID_ARG) {
            Rval = DEMUXER_ERR_0000;
        } else if ((Ret == FORMAT_ERR_IO_ERROR) || (Ret == FORMAT_ERR_INVALID_FORMAT)) {
            Rval = DEMUXER_ERR_0002;
        } else if (Ret == FORMAT_ERR_FIFO_FULL) {
            Rval = OK;
        } else {
            Rval = DEMUXER_ERR_0001;
        }
    }
    return Rval;
}

/**
 * usually used for duration check (auto split, index allocation)
 */
static inline UINT64 SVC_FORMAT_TIME_TO_DTS(UINT64 Time, UINT32 TimeScale)
{
    return ((Time / 1000U) * (UINT64)TimeScale) + ((((Time % 1000U) * (UINT64)TimeScale) + 999U) / 1000U);
}

/**
 * usually used for muxing/demuxing progress control
 */
static inline UINT64 SVC_FORMAT_TIME_TO_DTS_FLOOR(UINT64 Time, UINT32 TimeScale)
{
    return ((Time / 1000U) * (UINT64)TimeScale) + (((Time % 1000U) * (UINT64)TimeScale) / 1000U);
}

/**
 * usually used to get current progress (forward), so use floor as default
 */
static inline UINT64 SVC_FORMAT_DTS_TO_TIME(UINT64 DTS, UINT32 TimeScale)              {
    UINT64 Rval;
    if (TimeScale == 0U) {
        Rval = 0U;
    } else {
        Rval = ((DTS / TimeScale) * 1000U) + (((DTS % TimeScale) * 1000U) / TimeScale);
    }
    return Rval;
}

static inline UINT64 SVC_FORMAT_DTS_TO_TIME_CEIL(UINT64 DTS, UINT32 TimeScale)
{
    return ((DTS / TimeScale) * 1000U) + ((((DTS % TimeScale) * 1000U) + TimeScale - 1U) / TimeScale);
}

/**
 *  Function pointer to get frame information
 */
typedef UINT32 (*SVC_FORMAT_GET_FRAME_INFO_FP)(void *IdxMgr, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo);

/**
 *  Function pointer to feed a video frame
 */
typedef UINT32 (*SVC_FORMAT_FEED_VIDEO_FRAME_FP)(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, SVC_FEED_FRAME_PARAM_s *FeedParam, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo);

/**
 *  Function pointer to feed a audio frame
 */
typedef UINT32 (*SVC_FORMAT_FEED_AUDIO_FRAME_FP)(SVC_AUDIO_TRACK_INFO_s *Audio, SVC_STREAM_HDLR_s *Stream, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo);

/**
 *  Function pointer to feed a text frame
 */
typedef UINT32 (*SVC_FORMAT_FEED_TEXT_FRAME_FP)(SVC_TEXT_TRACK_INFO_s *Text, SVC_STREAM_HDLR_s *Stream, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo);

/**
 *  Frame feeder function pointer
 */
typedef struct {
    SVC_FORMAT_FEED_VIDEO_FRAME_FP FeedVideoFrame;  /**< Function pointer to video frame feeder */
    SVC_FORMAT_FEED_AUDIO_FRAME_FP FeedAudioFrame;  /**< Function pointer to audio frame feeder */
    SVC_FORMAT_FEED_TEXT_FRAME_FP FeedTextFrame;    /**< Function pointer to text frame feeder */
} SVC_FORMAT_FEED_FRAME_FP_s;

/**
 * Minus buffer address
 *
 * @param [in] MinusSize The size to minus
 * @param [in] Offset The current position
 * @param [in] Size The size of the FIFO buffer
 * @param [out] NewOffset The resulted position
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_MinusAddr(UINT32 MinusSize, UINT32 Offset, UINT32 Size, UINT32 *NewOffset);

/**
 * Add buffer address
 *
 * @param [in] PlusSize The size to plus
 * @param [in] Offset The current position
 * @param [in] Size The size of the FIFO buffer
 * @param [out] NewOffset The resulted position
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PlusAddr(UINT32 PlusSize, UINT32 Offset, UINT32 Size, UINT32 *NewOffset);

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
UINT32 SvcFormat_WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 DataSize, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize);

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
UINT32 SvcFormat_WriteStreamMem(UINT8 *DstBuffer, UINT32 DataSize, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset);

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
UINT32 SvcFormat_ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 DataSize, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset);

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
UINT32 SvcFormat_ReadStreamMem(const UINT8 *Data, UINT32 DataSize, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *NewBufferOffset);

/**
 * Get string of Movie information
 *
 * @param [in] Movie Movie information
 * @param [out] Buffer String buffer
 * @param [in] Size String size
 */
void SvcFormat_GetMovieInfoString(const SVC_MOVIE_INFO_s *Movie, char *Buffer, UINT32 Size);

/**
 * Get string of Image information
 *
 * @param [in] Image Image information
 * @param [out] Buffer String buffer
 * @param [in] Size String size
 */
void SvcFormat_GetImageInfoString(const SVC_IMAGE_INFO_s *Image, char *Buffer, UINT32 Size);


/**
 * Get AMBA box size
 *
 * @param [in] Media Media information
 * @return The returned AMBA box size
 */
INT32 SvcFormat_GetAmbaBoxSize(const SVC_MEDIA_INFO_s *Media);

/**
 * Put AMBA box
 *
 * @param [in] Stream Stream information
 * @param [in] Media Media information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutAmbaBox(SVC_STREAM_HDLR_s *Stream, const SVC_MEDIA_INFO_s *Media);

/**
 * Parse AMBA box
 *
 * @param [in] Stream Stream information
 * @param [in] MediaType Media type
 * @param [in] BoxSize Box size
 * @param [out] AmbaboxInfo AMBA box information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_ParseAmbaBox(SVC_STREAM_HDLR_s *Stream, UINT8 MediaType, UINT32 BoxSize, SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo);

/**
 * Get media information
 *
 * @param [out] Media Media information
 * @param [in] AmbaboxInfo AMBA box information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_GetMediaInfo(SVC_MEDIA_INFO_s *Media, const SVC_AMBA_BOX_INFORMATION_s *AmbaboxInfo);

/**
 * Initialize AMBA box version
 *
 * @param [in] Media Media information
 */
void SvcFormat_InitAmbaBoxVersion(SVC_MEDIA_INFO_s *Media);

/**
 * Get AMBA box version
 *
 * @param [in] Media Media information
 * @param [out] Type Media type
 * @param [out] MainVer Main version
 * @param [out] SubVer Sub version
 */
void SvcFormat_GetAmbaBoxVersion(const SVC_MEDIA_INFO_s *Media, UINT8 *Type, UINT8 *MainVer, UINT8 *SubVer);

/**
 * Put end marker
 *
 * @param [in] Fifo Fifo handler
 * @param [in] PTS Current PTS
 * @param [in] TimeScale Time scale
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutEndMark(SVC_FIFO_HDLR_s *Fifo, UINT64 PTS, UINT32 TimeScale);

/**
 * Put decode marker
 *
 * @param [in] Fifo Fifo handler
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_PutDecodeMark(SVC_FIFO_HDLR_s *Fifo);

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
UINT64 SvcFormat_GetMediaTime(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 Direction);

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
UINT32 SvcFormat_PrepareFrame(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *Frame, UINT32 Size, UINT8 Type, UINT64 PTS, UINT32 TimeScale);

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
UINT32 SvcFormat_WriteFrame(const SVC_FIFO_HDLR_s *Fifo, SVC_FIFO_BITS_DESC_s *Frame, UINT32 Size, UINT8 Type, UINT64 PTS, UINT32 TimeScale, UINT8 Completed);

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
UINT32 SvcFormat_FeedVideoFrame(SVC_VIDEO_TRACK_INFO_s *Video, SVC_STREAM_HDLR_s *Stream, SVC_FEED_FRAME_PARAM_s *FeedParam, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo);

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
UINT32 SvcFormat_FeedAudioFrame(SVC_AUDIO_TRACK_INFO_s *Audio, SVC_STREAM_HDLR_s *Stream, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo);

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
UINT32 SvcFormat_FeedTextFrame(SVC_TEXT_TRACK_INFO_s *Text, SVC_STREAM_HDLR_s *Stream, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT32 FrameNo);

/**
 * Get the default video track information
 *
 * @param [in] Tracks Video tracks
 * @param [in] TrackCount Number of video tracks
 * @param [out] TrackIdx The index of the returned default track
 * @return The default video track
 */
SVC_VIDEO_TRACK_INFO_s *SvcFormat_GetDefaultVideoTrack(SVC_VIDEO_TRACK_INFO_s *Tracks, UINT8 TrackCount, UINT8 *TrackIdx);

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
UINT8 SvcFormat_GetShortestTrack(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx);

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
UINT8 SvcFormat_GetLongestTrack(const SVC_VIDEO_TRACK_INFO_s *VideoTracks, UINT8 VideoTrackCount, const SVC_AUDIO_TRACK_INFO_s *AudioTracks, UINT8 AudioTrackCount, const SVC_TEXT_TRACK_INFO_s *TextTracks, UINT8 TextTrackCount, UINT8 *TrackIdx);

/**
 * Normalize media time scale
 *
 * @param [in] Media Media information
 * @return FORMAT_OK=>ok, others=>NG
 */
UINT32 SvcFormat_NormalizeTimeScale(const SVC_MEDIA_INFO_s *Media);

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
UINT32 SvcFormat_SeekVideo(SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS);

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
UINT32 SvcFormat_SeekAudio(SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS);

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
UINT32 SvcFormat_SeekText(SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, UINT8 Direction, UINT64 DTS);

/**
 * Check if there is a track with empty clip or not
 *
 * @param [in] Media Media information
 * @return 1U of 0U
 */
UINT8 SvcFormat_HasEmptyTrack(const SVC_MEDIA_INFO_s *Media);

/**
 * Update the general information of a track
 *
 * @param [in] Track Track information
 * @param [in] FrameCount Number of frames
 */
void SvcFormat_UpdateTrack(SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, UINT32 FrameCount);

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
UINT32 SvcFormat_CheckLimit(const SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, SVC_STREAM_HDLR_s *Stream, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *Limit);

/**
 * Update slice index when receiving a frame
 *
 * @param [in] Frame Frame descriptor
 * @param [out] Video Video track information
 */
void SvcFormat_UpdateFrameSliceIdx(const SVC_FIFO_BITS_DESC_s *Frame, SVC_VIDEO_TRACK_INFO_s *Video);

/**
 * Get video frame type accroding to sample index and GOP structure
 *
 * @param [in] Mode SVC_VIDEO_MODE_P/SVC_VIDEO_MODE_I_A_FRM_PER_SMP/SVC_VIDEO_MODE_I_A_FLD_PER_SMP
 * @param [in] M GOP's M
 * @param [in] N GOP's N
 * @param [in] GOPSize GOP's size
 * @param [in] ClosedGOP closed GOP or not (only the 1st GOP of the entire record session can be closed GOP)
 * @param [in] SampleIdx sample index of the current GOP
 * @return frame type (SVC_FIFO_TYPE_IDR_FRAME/SVC_FIFO_TYPE_I_FRAME/SVC_FIFO_TYPE_P_FRAME/SVC_FIFO_TYPE_B_FRAME
 */
UINT8 SvcFormat_GetVideoFrameType(UINT8 Mode, UINT16 M, UINT16 N, UINT32 GOPSize, UINT8 ClosedGOP, UINT32 SampleIdx);

#endif

