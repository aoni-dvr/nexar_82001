/**
 * @file FrameFeeder.h
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
#ifndef FRAME_FEEDER_H
#define FRAME_FEEDER_H

#include "../FormatAPI.h"

#define SVC_FEEDING_RULE_H264  (0x00U)  /**< H264 Feeding Rule */
#define SVC_FEEDING_RULE_AVC   (0x01U)  /**< AVC Feeding Rule */
#define SVC_FEEDING_RULE_H265  (0x02U)  /**< H265 Feeding Rule */
#define SVC_FEEDING_RULE_HVC   (0x03U)  /**< HVC Feeding Rule */

/**
 *  Frame feeder information
 */
typedef struct {
    UINT8 *SPS;     /**< SPS */
    UINT8 *PPS;     /**< PPS */
    UINT16 SPSLen;  /**< SPS length */
    UINT16 PPSLen;  /**< PPS length */
} SVC_FEEDING_RULE_H264_INFO_s;

/**
 *  Frame feeder information
 */
typedef struct {
    UINT8 *VPS;     /**< VPS */
    UINT8 *SPS;     /**< SPS */
    UINT8 *PPS;     /**< PPS */
    UINT16 VPSLen;  /**< VPS length */
    UINT16 SPSLen;  /**< SPS length */
    UINT16 PPSLen;  /**< PPS length */
} SVC_FEEDING_RULE_H265_INFO_s;

/**
 *  Feeding rule information
 */
typedef struct {
    SVC_FEEDING_RULE_H264_INFO_s H264; /**< If the media is AVC, the user need to fill the parameter */
    SVC_FEEDING_RULE_H265_INFO_s H265; /**< If the media is HEVC, the user need to fill the parameter */
} SVC_FEEDING_RULE_INFO_s;

/**
 *  Frame feeder information
 */
typedef struct {
    UINT32 MediaId;         /**< Media Id */
    UINT32 TargetFrameNo;   /**< The target frame number that should be fed next round. */
    UINT32 FrameNo;         /**< This time feed frame number */
    UINT32 FrameCount;      /**< This time feed frame count */
    UINT64 FeedTime;        /**< This time feed time */
    UINT8 Direction;        /**< This time feed direction */
    UINT8 Speed;            /**< This time feed speed */
    SVC_FEEDING_RULE_INFO_s Info;                   /**< Feeding rule information */
    SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo;      /**< Funcion pointers to get frame information */
    SVC_FORMAT_FEED_VIDEO_FRAME_FP FeedVideoFrame;  /**< Function pointer to feed video frame */
    SVC_FORMAT_FEED_AUDIO_FRAME_FP FeedAudioFrame;  /**< Function pointer to feed audio frame */
    SVC_FORMAT_FEED_TEXT_FRAME_FP FeedTextFrame;    /**< Function pointer to feed text frame */
} SVC_FRAME_FEEDER_INFO_s;

/**
 * Init feeder information
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Track The track information
 * @param [in] TrackId The track id
 * @param [in] GetFrameInfo Function pointer to get frame information
 * @param [in] FeedFrame Function pointer to feed frames
 * @param [in] TargetTime Target time
 * @param [in] Direction Feeding direction
 * @param [in] Speed Speed
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_InitFeedInfo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_MEDIA_TRACK_GENERAL_INFO_s *Track, UINT8 TrackId, SVC_FORMAT_GET_FRAME_INFO_FP GetFrameInfo, const SVC_FORMAT_FEED_FRAME_FP_s *FeedFrame, UINT64 TargetTime, UINT8 Direction, UINT8 Speed);

/**
 * Process video track
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Video Video track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @param [in] End Reaching end or not
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_ProcessVideo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset, UINT8 End);

/**
 * Process audio track
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Audio Audio track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @param [in] End Reaching end or not
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_ProcessAudio(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset, UINT8 End);

/**
 * Process text track
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Text Text track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @param [in] End Reaching end or not
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFrameFeeder_ProcessText(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset, UINT8 End);
#if 0 /* Unused. Use FeedingRuleH264/H265 instead */
/**
 * Process video track with default feeding rule
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
UINT32 SvcFeedingRuleDefault_ProcessVideo(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_VIDEO_TRACK_INFO_s *Video, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset);
#endif
/**
 * Process audio track with default feeding rule
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Audio Audio track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFeedingRuleDefault_ProcessAudio(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_AUDIO_TRACK_INFO_s *Audio, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset);

/**
 * Process text track with default feeding rule
 *
 * @param [in] FeedInfo The feed frame information
 * @param [in] Stream The I/O stream handler
 * @param [in] Text Text track information
 * @param [in] IdxMgr Index manager
 * @param [in] TrackId Track id
 * @param [out] NextFrameOffset Offset to next frame number
 * @param [out] NextTimeOffset Offset to next frame time
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcFeedingRuleDefault_ProcessText(SVC_FRAME_FEEDER_INFO_s *FeedInfo, SVC_STREAM_HDLR_s *Stream, SVC_TEXT_TRACK_INFO_s *Text, void *IdxMgr, UINT8 TrackId, INT32 *NextFrameOffset, INT32 *NextTimeOffset);

#endif

