/**
 * Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 */
#ifndef __AMBAEXAMFRAMESHARERTSP_H__
#define __AMBAEXAMFRAMESHARERTSP_H__

#ifdef  __cplusplus
extern "C" {
#endif
#include "AmbaIPC_RpcProg_RT_ExamFrameShareRtsp.h"

#define AMBA_EXAMFRAMESHARERTSP_STREAMLIST_MAX_ENTRY 16
#define AMBA_EXAMFRAMESHARERTSP_MAX_STREAMNAME_LENGTH 64

typedef struct _AMBA_EXAMFRAMESHARERTSP_STREAM_ITEM_s_{
    char Name[AMBA_EXAMFRAMESHARERTSP_MAX_STREAMNAME_LENGTH];/**< The stream name used for comparing with RTSP url */
    unsigned int Active;               /**< Indicate whether this stream is playable or not */
    unsigned int StrmId;
} AMBA_EXAMFRAMESHARERTSP_STREAM_ITEM_s;

typedef struct _AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s_ {
    int Amount;
    AMBA_EXAMFRAMESHARERTSP_STREAM_ITEM_s StreamItemList[AMBA_EXAMFRAMESHARERTSP_STREAMLIST_MAX_ENTRY];
} AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s;

//AllenLiu start



#define AMBA_EXAMFRAMESHARERTSP_MAX_TRACK_PER_MEDIA  4   /**< The max track number of a media. */

#define AMBA_EXAMFRAMESHARERTSP_MARK_EOS           0x00FFFFFF



typedef enum _AMBA_EXAMFRAMESHARERTSP_STATUS_e_ {
    AMBA_EXAMFRAMESHARERTSP_STATUS_START = 1, /**< The start event of the NetFifo. The NetFifo start to run. */
    AMBA_EXAMFRAMESHARERTSP_STATUS_END, /**< The end event of the NetFifo. The NetFifo complete stopped. */
    AMBA_EXAMFRAMESHARERTSP_STATUS_SWITCHENCSESSION, /**< The end event of the NetFifo. The NetFifo is stopped for enc session switching. */

    AMBA_EXAMFRAMESHARERTSP_STATUS_GENERAL_ERROR = 0xF0 /**< The error event of the NetFifo. */
} AMBA_EXAMFRAMESHARERTSP_STATUS_e;

typedef struct _AMBA_EXAMFRAMESHARERTSP_BITS_DESC_s_ {
    unsigned int TrackType;
    unsigned int SeqNum;
    unsigned long long Pts;
    unsigned char Type;
    unsigned char Completed;
    unsigned short  Align;
    unsigned long long StartAddr;
    unsigned int Size;
    unsigned int StrmId;
} AMBA_EXAMFRAMESHARERTSP_BITS_DESC_s;


typedef struct _AMBA_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s_ {
    unsigned int nGOPSize;            /**< The number of the picture between IDR pictures. */
    unsigned int nTrickRecDen;        /**< For AmpFormat_ConvertPTS(), the Denominator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    unsigned int nTrickRecNum;        /**< For AmpFormat_ConvertPTS(), the Numerator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    unsigned int nCodecTimeScale;     /**< TODO: It needs get from H264 bitstream, it is not configurable. */
    unsigned short  nWidth;              /**< Picture width */
    unsigned short  nHeight;             /**< Picture height */
    unsigned short  nM;                  /**< The number of the picture between reference pictures(IDR, I, P) */
    unsigned short  nN;                  /**< The number of the picture between I pictures */
    unsigned short  nIRCycle;           /**< Intra refresh cycle*/
    unsigned short  nRecoveryFrameCnt;  /**< The value of recovery_frame_cnt in SEI-recovery point */
    unsigned char bDefault;             /**< The flag defines the track as default video track, if the media hasn't one video track. */
    unsigned char nMode;                /**< The value defines the picture mode of the video. It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample, See AMP_VIDEO_MODE_s */
    unsigned char bClosedGOP;           /**< The structure of the Close GOP is I P B B P B B. The structure of the Open GOP is I B B P B B, If resume or auto split, the value always is Open GOP. */
} AMBA_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s;

typedef struct _AMBA_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s_ {
    unsigned int nSampleRate;         /**< The sample rate(Hz) of the audio track. */
    unsigned char bDefault;             /**< The flag defines the track as default audio track, if the media hasn't one audio track. */
    unsigned char nChannels;            /**< The number of audio channel. */
    unsigned char nBitsPerSample;       /**< The per sample size of the audio track. Ex: 8 bits, 16 bits, ....n bits */
} AMBA_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s;

typedef enum _AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_e_ {
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX = 1,   /**< The track's type is Video */
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX = 32,
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX = 33,  /**< The track's type is Audio */
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX = 64,
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MIN_IDX  = 65,  /**< The track's type is Text */
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MAX_IDX  = 96,
} AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_e;


typedef struct _AMBA_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s_ {
    unsigned char bDefault;             /**< The flag defines the track as default text track, if the media hasn't one text track. */
} AMBA_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s;


typedef enum _AMBA_EXAMFRAMESHARERTSP_EVENT_e_ {
    AMBA_EXAMFRAMESHARERTSP_TYPE_MJPEG_FRAME = 0,    ///< MJPEG frame type
    AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME = 1,    ///< Idr frame type
    AMBA_EXAMFRAMESHARERTSP_TYPE_I_FRAME = 2,      ///< I frame type
    AMBA_EXAMFRAMESHARERTSP_TYPE_P_FRAME = 3,      ///< P frame type
    AMBA_EXAMFRAMESHARERTSP_TYPE_B_FRAME = 4,      ///< B frame type
    AMBA_EXAMFRAMESHARERTSP_TYPE_JPEG_FRAME = 5,   ///< jpeg main frame
    AMBA_EXAMFRAMESHARERTSP_TYPE_THUMBNAIL_FRAME = 6,  ///< jpeg thumbnail frame
    AMBA_EXAMFRAMESHARERTSP_TYPE_SCREENNAIL_FRAME = 7, ///< jpeg screennail frame
    AMBA_EXAMFRAMESHARERTSP_TYPE_AUDIO_FRAME = 8,      ///< audio frame
    AMBA_EXAMFRAMESHARERTSP_TYPE_UNDEFINED = 9,        ///< others

    AMBA_EXAMFRAMESHARERTSP_TYPE_DECODE_MARK = 101, ///< used when feeding bitstream to dsp. will push out all frame. */
    AMBA_EXAMFRAMESHARERTSP_TYPE_EOS = 255,                  ///< eos bits that feed to raw buffer

    AMBA_EXAMFRAMESHARERTSP_TYPE_LAST = AMBA_EXAMFRAMESHARERTSP_TYPE_EOS

} AMBA_EXAMFRAMESHARERTSP_EVENT_e;

typedef struct _AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s_{
    unsigned int nMediaId;            /**< The media type of the track. The id is media id, See AMP_FORMAT_MID_e */
    unsigned int nTimeScale;          /**< Time scale, the same as the LCM of those of all tracks */
    unsigned int nTimePerFrame;       /**< The time of the frame that the unit of it is Time scale. */
    unsigned int nInitDelay;          /**< Initial delay time(ms) of the track. */
    void *hCodec;       /**< the codec which this track is working on. */
    unsigned char *pBufferBase;            /**< The start address of the FIFO of the track. User pushs data to the FIFO, the FIFO will write data to the address of the buffer. */
    unsigned char *pBufferLimit;           /**< The end address of the FIFO of the track, The data can't write overlap the address, FIFO size = FIFO buffer limit - FIFO buffer base. */
    union {
        AMBA_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s Video;
        AMBA_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s Audio;
        AMBA_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s Text;
    } Info;
    unsigned char nTrackType;           /**< Track type, See AMP_MEDIA_TRACK_TYPE_e */
} AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s;


typedef struct _AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s_ {
    AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s Track[AMBA_EXAMFRAMESHARERTSP_MAX_TRACK_PER_MEDIA];
    unsigned char nTrack;               /**< The number of Tracks in the movie. */
} AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s;


//AllenLiu end


typedef int (*cbFrameShareEvent)(int type, void* info);


long long AmbaFrameShareRtsp_get_mmap_offset(int frame_type);
long long AmbaFrameShareRtspGSensor_get_mmap_offset(void);

extern int AmbaFrameShareRtsp_Reg_cbFrameShareEvent(cbFrameShareEvent cb, void *user_data);
extern int AmbaFrameShareRtspGSensor_Reg_cbFrameShareEvent(cbFrameShareEvent cb, void *user_data);
extern int AmbaExamFrameShareRtsp_GetMediaStreamIDList(AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s *res);

extern int AmbaExamFrameShareRtsp_GetMediaInfo(int StreamID, AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s *res);
int AmbaNetFifo_PlayBack_OP(AMBA_NETFIFO_PLAYBACK_OP_PARAM_s *cmd, AMBA_NETFIFO_PLAYBACK_OP_PARAM_s *res);
int AmbaNetFifo_ReportStatus(unsigned int status);

int AmbaFrameShareRtsp_init(int *event_hndlr);
int AmbaFrameShareRtsp_Rpcinit(void);
void AmbaFrameShareRtsp_release(void);

int AmbaFrameShareRtsp_fifoCreate();
void AmbaFrameShareRtsp_fifoStop();

void AmbaFrameShareRtsp_Ready(void);

#ifdef  __cplusplus
}
#endif
#endif

