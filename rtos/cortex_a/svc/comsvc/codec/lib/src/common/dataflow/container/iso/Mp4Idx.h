/**
 * @file Mp4Idx.h
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
#ifndef MP4_IDX_H
#define MP4_IDX_H

#include "format/SvcFormat.h"
#include "IsoBoxDef.h"
#include "IsoIdx.h"
#include "../index/IndexCache.h"


/**
 *  Mp4 index
 */
typedef struct {
    SVC_STREAM_HDLR_s *Stream; /**< Stream handler */
    SVC_MOVIE_INFO_s *Movie;   /**< Movie information */
#if 0
    AMBA_INDEX_HDLR_s *Index;   /**< Index handler */
    UINT64 MaxDuration;         /**< Max duration to allocate index (using track's timescale as ticks)*/
#endif
    UINT32 MaxIdxNum;                                                   /**< Maximum number of index */
    ISO_IDX_TRACK_INFO_s TrackInfo[SVC_FORMAT_MAX_TRACK_PER_MEDIA];     /**< ISO_IDX_TRACK_INFO_s */
    INDEX_CACHE_HDLR_s IndexCache;                                      /**< Index cache handler */
    ISO_IDX_STTS_BUFFER_INFO_s SttsBuffer[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];    /**< Stts buffer */
    UINT8 CreateMode;           /**< ISO_IDX_MODE_s */
} MP4_IDX_s;

/**
 * Create MP4 index API.
 *
 * @param [in] Mode create iso index mode.
 * @param [in] Movie movie information for create function reference.
 * @param [in] Stream stream handler for create function read/write file.
 * @param [in] MaxIdxNum define index size of each index item.
 * @param [out] Mp4Idx MP4 index api get buffer.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Idx_Create(UINT8 Mode, SVC_MOVIE_INFO_s *Movie, SVC_STREAM_HDLR_s *Stream, UINT32 MaxIdxNum, MP4_IDX_s *Mp4Idx);

/**
 * Delete MP4 index API.
 *
 * @param [in] Mp4Idx MP4 index api.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Idx_Delete(MP4_IDX_s *Mp4Idx);

/**
 * Get all of the information of target frame.
 *
 * @param [in] Mp4Idx index manager.
 * @param [in] TrackId track id of media information.
 * @param [in] FrameNo target frame number of index data to get.
 * @param [in] Direction get data direction.
 * @param [out] FrameInfo buffer to write the return frame information.
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 Mp4Idx_GetFrameInfo(MP4_IDX_s *Mp4Idx, UINT8 TrackId, UINT32 FrameNo, UINT8 Direction, SVC_FRAME_INFO_s *FrameInfo);

#endif /* MP4_IDX_H */

