/**
 * @file Fmp4MuxImpl.h
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
#ifndef CONTAINER_FRAG_MUX_H
#define CONTAINER_FRAG_MUX_H

#include "format/SvcFormat.h"
#include "IsoBoxDef.h"
#include "IsoMux.h"

/**
 *  Fragment Track
 */
typedef struct {
    UINT8 TrackType;            /**< Track Type */
    UINT16 M;                   /**< M, only for video */
    UINT32 SampleDataOffset;    /**< Sample data offset. Relative offset to MdatOffset */
    UINT32 SampleSize[SVC_ISO_MAX_FRAME_PER_FRAG];  /**< Sample sizes */
    UINT32 SampleCount;         /**< Sample count */
    UINT32 SampleDuration;      /**< Sample duration */
    UINT32 AdjustedSampleCount; /**< Sample count with adjusted duration */
    UINT32 AdjustedSampleDuration[SVC_ISO_MAX_FRAME_PER_FRAG];  /**< Adjusted sample duration */
    UINT64 BaseMediaDecodeTime; /**< Base media decode time */
} FRAGMENT_TRACK_s;

/**
 *  Fragment Queue
 */
typedef struct {
    UINT32 SequenceNum; /**< Sequence Number */
    UINT64 MoofOffset;  /**< Moof offset. Not include size+tag */
    UINT32 MdatSize;    /**< Mdat size */
    UINT64 MdatOffset;  /**< Mdat offset. Not include size+tag */
    FRAGMENT_TRACK_s Traf[SVC_FORMAT_MAX_TRACK_PER_MEDIA];  /**< Fragment tracks */
} FRAGMENT_QUEUE_s;

/**
 *  Fragment Information
 */
typedef struct {
    SVC_VIDEO_TRACK_INFO_s VideoTrack[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];    /**< Video tracks info */
    SVC_AUDIO_TRACK_INFO_s AudioTrack[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];    /**< Audio tracks info */
    SVC_TEXT_TRACK_INFO_s TextTrack[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA];       /**< Text tracks info */
    FRAGMENT_TRACK_s Traf[SVC_FORMAT_MAX_TRACK_PER_MEDIA];      /**< Traf */
    FRAGMENT_QUEUE_s QueFragment;   /**< Queued fragment  */
    UINT64 BaseDataOffset;          /**< Base data offset. Not include size+tag */
    UINT32 PopSampleCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA];      /**< Popped sample count */
    UINT32 LastPopSampleCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA];  /**< Last popped sample count */
    UINT32 SequenceNum;     /**< Sequence number */
    UINT32 MdatSize;        /**< Size of the current mdat */
    UINT8 VideoTrackCount;  /**< Video track count */
    UINT8 AudioTrackCount;  /**< Audio track count */
    UINT8 TextTrackCount;   /**< Text track count */
    UINT8 Event;            /**< Event */
    UINT8 Parsed;           /**< Parsed flag */
    UINT8 LogDone;          /**< Log done flag  */
    UINT32 EntryLogged[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];   /**< Number of logged video entries */
    UINT32 EntryWritten[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];  /**< Number of written video entries */
    UINT32 AudioEntryLogged[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];  /**< Number of logged audio entries */
    UINT32 AudioEntryWritten[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA]; /**< Number of written audio entries */
    UINT32 TextEntryLogged[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA];    /**< Number of logged text entries */
    UINT32 TextEntryWritten[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA];   /**< Number of written text entries */
    SVC_VIDEO_TRACK_INFO_s TempVideoTrack[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];    /**< Temp. video tracks */
    SVC_AUDIO_TRACK_INFO_s TempAudioTrack[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];    /**< Temp. audio tracks */
    SVC_TEXT_TRACK_INFO_s TempTextTrack[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA];       /**< Temp. text tracks */
} FRAGMENT_INFO_s;

//typedef UINT32 (*SVC_ISOMUX_PUT_FTYP_FP)(SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream);

/*
 * Track duration formula = (MaxPTS - MiPTS) + (MaxPTS - SecondMaxPTS)
 * Track's duration = sum of the durations of all of the track¡¦s edits,
 *                    include empty time of the track.
 * Edts box = segment_duration present the initDTS, if initDTS != 0,
 *            the track have offset time need to skip present.
 *
 */

/**
 *  Process the Fmp4 muxing format.
 *
 *  @param [in] Fmp4Mux Fmp4 fragment information
 *  @param [in] Media Media information
 *  @param [in] Stream Stream handler
 *  @param [in] MoovAtom Moov atom
 *  @param [in] TargetTime Target time to process
 *  @param [in] MaxDuration Maximum duration of a clip
 *  @param [in] MaxSize Maximum size of a clip
 *  @param [out] Event The returned process event
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Mux_ProcessImpl(FRAGMENT_INFO_s *Fmp4Mux, SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream, ISO_MOOV_BOX_s *MoovAtom, UINT64 TargetTime, UINT32 MaxDuration, UINT64 MaxSize, UINT8 *Event);

/**
 *  Open the Fmp4 muxing format.
 *
 *  @param [in] Fmp4Mux Fmp4 fragment information
 *  @param [in] Hdlr Muxing format handler
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Mux_OpenImpl(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MUX_FORMAT_HDLR_s *Hdlr);

/**
 *  Close the Fmp4 muxing format.
 *
 *  @param [in] Fmp4Mux Fmp4 fragment information
 *  @param [in] Hdlr Muxing format handler
 *  @param [in] Mode Close mode (SVC_MUX_FORMAT_CLOSE_XXXX)
 *  @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Mux_CloseImpl(FRAGMENT_INFO_s *Fmp4Mux, const SVC_MUX_FORMAT_HDLR_s *Hdlr, UINT8 Mode);

#endif
