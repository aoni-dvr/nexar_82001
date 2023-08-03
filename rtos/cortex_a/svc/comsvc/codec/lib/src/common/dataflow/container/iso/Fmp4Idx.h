/**
 * @file Fmp4Idx.h
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
#ifndef CONTAINER_FRAGMENT_IDX_H
#define CONTAINER_FRAGMENT_IDX_H

#include "format/SvcFormat.h"
#include "stream/SvcStream.h"
#include "IsoIdx.h"
#include "IsoBoxDef.h"

/**
 * Cached moof Information
 */
typedef struct {
    UINT32 SequenceNum; /**< Sequence number */
    UINT64 Position;    /**< Position */
    UINT32 FrameNo[SVC_FORMAT_MAX_TRACK_PER_MEDIA];             /**< The last Frame's no. */
    UINT32 FrameCount[SVC_FORMAT_MAX_TRACK_PER_MEDIA];          /**< Frame count */
    SVC_FRAME_INFO_s *FrameInfo[SVC_FORMAT_MAX_TRACK_PER_MEDIA];/**< Frame information */
    SVC_FRAME_INFO_s VideoFrameInfo[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA][SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG];/**< Video frame info */
    SVC_FRAME_INFO_s AudioFrameInfo[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA][SVC_ISO_MAX_AUDIO_FRAME_PER_FRAG];/**< Audio frame info */
    SVC_FRAME_INFO_s TextFrameInfo[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA][SVC_ISO_MAX_TEXT_FRAME_PER_FRAG];   /**< Text frame info */
} MOOF_INFO_CACHE_s;

typedef struct {
    UINT64 DTS;                     /**< The track's current DTS that has been parsed */
    UINT32 MaxFrameCount;           /**< Maximum frame number */
    ISO_IDX_TRACK_INFO_s IdxInfo;   /**< Iso index track information */
} FMP4_IDX_TRACK_INFO_s;

/**
 * Fragment Cache
 */
typedef struct {
    UINT8 TrackCount;       /**< Track count */
    UINT32 FragmentCount;   /**< Fragment count */
    UINT32 SequenceIdx;     /**< Sequence index */
    UINT32 ParsedCount;     /**< The number of MOOF that have been parsed */
    UINT64 ParsedPos;       /**< The position to parse */
    MOOF_INFO_CACHE_s Moof[SVC_ISO_MAX_FRAGMENTS];      /**< Cached moof information */
    FMP4_IDX_TRACK_INFO_s Track[SVC_FORMAT_MAX_TRACK_PER_MEDIA];    /**< The current track info that has been parsed */
} FRAGMENT_CACHE_s;

/**
 * Fragment Index
 */
typedef struct {
    SVC_MEDIA_INFO_s *Media;    /**< Media information */
    SVC_STREAM_HDLR_s *Stream;  /**< Stream handler */
    FRAGMENT_CACHE_s Caches;    /**< Fragment cache */
} FRAGMENT_IDX_s;

/**
 * Get information of the target frame
 *
 * @param [in] Fmp4Idx Index manager
 * @param [in] TrackId Track id of media information
 * @param [in] FrameNo Target frame number of index data to get
 * @param [in] Direction Get data direction
 * @param [out] FrameInfo Buffer to write the return frame information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Idx_GetFrameInfo(FRAGMENT_IDX_s *Fmp4Idx, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo);

/**
 * Create FMP4 index
 *
 * @param [in] Fmp4Idx Fmp4 index
 * @param [in] Media Media information
 * @param [in] Stream Stream handler
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Idx_Create(FRAGMENT_IDX_s *Fmp4Idx, SVC_MEDIA_INFO_s *Media, SVC_STREAM_HDLR_s *Stream);

/**
 * Delete FMP4 index
 *
 * @param [in] Fmp4Idx Fmp4 index
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Fmp4Idx_Delete(FRAGMENT_IDX_s *Fmp4Idx);

#endif

