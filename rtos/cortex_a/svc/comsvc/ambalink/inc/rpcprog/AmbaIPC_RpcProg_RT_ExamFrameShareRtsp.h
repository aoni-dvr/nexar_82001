/**
 *
 * Header file for RPC Services
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_RT_EXAMFRAMESHARERTSP_H_
#define _RPC_PROG_RT_EXAMFRAMESHARERTSP_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define RT_EXAMFRAMESHARERTSP_PROG_ID   0x1000000E
#define RT_EXAMFRAMESHARERTSP_HOST AMBA_IPC_HOST_THREADX
#define RT_EXAMFRAMESHARERTSP_VER (1)
#define RT_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT (FOREVER)
#define RT_EXAMFRAMESHARERTSP_NAME "AMBAEXAMFRAMESHARERTSP_RTSVC"


typedef enum _RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_e_ {
    RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO = 0x01,  /**< The track's type is Video */
    RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO = 0x02,  /**< The track's type is Audio */
    RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT = 0x03,   /**< The track's type is Text */
    RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_MAX = 0x04     /**< Max value, for check use */
} RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_e;

typedef struct _RT_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s_ {
    unsigned int nGOPSize;            /**< The number of the picture between IDR pictures. */
    unsigned int nTrickRecDen;        /**< For AmpFormat_ConvertPTS(), the Denominator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    unsigned int nTrickRecNum;        /**< For AmpFormat_ConvertPTS(), the Numerator of the trick record. If is high frame rate, the denominator and numerator are specify the factor than default 30fps, Ex: 120fps -> Den = 1, Num = 4 */
    unsigned int nCodecTimeScale;     /**< TODO: It needs get from H264 bitstream, it is not configurable. */
    unsigned short  nWidth;              /**< Picture width */
    unsigned short  nHeight;             /**< Picture height */
    unsigned short  nM;                  /**< The number of the picture between reference pictures(IDR, I, P) */
    unsigned short  nN;                  /**< The number of the picture between I pictures */
    unsigned short  nIRCycle;            /**< Intra refresh cycle number */
    unsigned short  nRecoveryFrameCnt;   /**< recovery_frame_cnt value of SEI recovery point */
    unsigned char bDefault;             /**< The flag defines the track as default video track, if the media hasn't one video track. */
    unsigned char nMode;                /**< The value defines the picture mode of the video. It has progressive and interlaced mode. Interlaced mode has Field Per Sample and Frame Per Sample, See AMP_VIDEO_MODE_s */
    unsigned char bClosedGOP;           /**< The structure of the Close GOP is I P B B P B B. The structure of the Open GOP is I B B P B B, If resume or auto split, the value always is Open GOP. */
} RT_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s;

typedef struct _RT_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s_ {
    unsigned int nSampleRate;         /**< The sample rate(Hz) of the audio track. */
    unsigned char bDefault;             /**< The flag defines the track as default audio track, if the media hasn't one audio track. */
    unsigned char nChannels;            /**< The number of audio channel. */
    unsigned char nBitsPerSample;       /**< The per sample size of the audio track. Ex: 8 bits, 16 bits, ....n bits */
} RT_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s;

typedef struct _RT_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s_ {
    unsigned char bDefault;             /**< The flag defines the track as default text track, if the media hasn't one text track. */
} RT_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s;

typedef struct _RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s_{
    unsigned int nMediaId;            /**< The media type of the track. The id is media id, See AMP_FORMAT_MID_e */
    unsigned int nTimeScale;          /**< Time scale, the same as the LCM of those of all tracks */
    unsigned int nTimePerFrame;       /**< The time of the frame that the unit of it is Time scale. */
    unsigned int nInitDelay;          /**< Initial delay time(ms) of the track. */
    unsigned long long hCodec;                     /**< the codec which this track is working on. */
    unsigned long long pBufferBase;       /**< The start address of the FIFO of the track. User pushs data to the FIFO, the FIFO will write data to the address of the buffer. */
    unsigned long long pBufferLimit;      /**< The end address of the FIFO of the track, The data can't write overlap the address, FIFO size = FIFO buffer limit - FIFO buffer base. */
    union {
        RT_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s Video;
        RT_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s Audio;
        RT_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s Text;
    } Info;
    unsigned char nTrackType;         /**< Track type, See AMP_MEDIA_TRACK_TYPE_e */
} RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s;

#define RT_EXAMFRAMESHARERTSP_MAX_TRACK_PER_MEDIA  8   /**< The max track number of a media. */

typedef struct _RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s_ {
    RT_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s Track[RT_EXAMFRAMESHARERTSP_MAX_TRACK_PER_MEDIA];
    unsigned char nTrack;               /**< The number of Tracks in the movie. */
} RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s;

//===== self sepcific =====

#define RT_EXAMFRAMESHARERTSP_STREAMLIST_MAX_ENTRY 16
#define RT_EXAMFRAMESHARERTSP_MAX_STREAMNAME_LENGTH 64
typedef struct _RT_EXAMFRAMESHARERTSP_STREAM_ITEM_s_{
    char Name[RT_EXAMFRAMESHARERTSP_MAX_STREAMNAME_LENGTH];                  /**< The stream name used for comparing with RTSP url */
    unsigned int Active;               /**< Indicate whether this stream is playable or not */
    unsigned int StrmId;
} RT_EXAMFRAMESHARERTSP_STREAM_ITEM_s;
typedef struct _RT_EXAMFRAMESHARERTSP_STREAMITEM_LIST_s_ {
    int Amount;
    RT_EXAMFRAMESHARERTSP_STREAM_ITEM_s StreamItemList[RT_EXAMFRAMESHARERTSP_STREAMLIST_MAX_ENTRY];
} RT_EXAMFRAMESHARERTSP_STREAMITEM_LIST_s;

typedef struct _RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s_ {
    unsigned long long v_hndlr; /**< hndlr for enc stream */
    unsigned long long a_hndlr; /**< hndlr for enc stream */
    unsigned long long gsensor_hndlr; /**< hndlr for enc stream */
    unsigned int media_type;
    unsigned long long buf_base_v; /**< base address of data buffer */
    unsigned long long  buf_base_phy_v; /**< physical base address of data buffer */
    unsigned int buf_size_v; /**< base address of data buffer */
    unsigned long long  buf_base_a;
    unsigned long long  buf_base_phy_a;
    unsigned int buf_size_a;
    unsigned long long  buf_base_gsensor;
    unsigned long long  buf_base_phy_gsensor;
    unsigned int buf_size_gsensor;
} RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s;


//============ RPC_FUNC definition ============
enum _RT_EXAMFRAMESHARERTSP_FUNC_e_ {
    RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIASTREAMID = 1,
    RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIAINFO,
    RT_EXAMFRAMESHARERTSP_FUNC_GETENCINFO,
    RT_EXAMFRAMESHARERTSP_FUNC_PLAYBACK_OP,
    RT_EXAMFRAMESHARERTSP_FUNC_REPORTSTATUS,

    RT_EXAMFRAMESHARERTSP_FUNC_AMOUNT
};

typedef struct _AMBA_NETFIFO_PLAYBACK_OP_PARAM_s_ {
    unsigned long long  Hdlr;
    unsigned int        OP;
    unsigned char       Param[128];
} AMBA_NETFIFO_PLAYBACK_OP_PARAM_s;

typedef struct _RT_NETFIFO_PLAYBACK_OP_PARAM_s_ {
    unsigned long long      pHdlr;
    unsigned int            OP;
    unsigned char           Param[128];
} RT_NETFIFO_PLAYBACK_OP_PARAM_s;

/**
 * [in] void
 * [out] RT_EXAMFRAMESHARERTSP_MEDIA_STREAMID_LIST_s
 */
AMBA_IPC_REPLY_STATUS_e RT_EXAMFRAMESHARERTSP_GetMediaStreamIDList_Clnt(void *pArg, int *pResult, int Clnt);
void RT_EXAMFRAMESHARERTSP_GetMediaStreamIDList_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] int
 * [out] RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s
 */
AMBA_IPC_REPLY_STATUS_e RT_EXAMFRAMESHARERTSP_GetMediaInfo_Clnt(unsigned int *pArg, int *pResult, int Clnt);
void RT_EXAMFRAMESHARERTSP_GetMediaInfo_Svc(unsigned int *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] int
 * [out] RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s
 */
AMBA_IPC_REPLY_STATUS_e RT_EXAMFRAMESHARERTSP_GetEncInfo_Clnt(unsigned int *pArg, int *pResult, int Clnt);
void RT_EXAMFRAMESHARERTSP_GetEncInfo_Svc(unsigned int *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_RT_EXAMFRAMESHARERTSP_H_ */
