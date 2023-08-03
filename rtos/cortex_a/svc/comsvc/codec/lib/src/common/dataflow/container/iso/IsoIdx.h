/**
 * @file IsoIdx.h
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
#ifndef ISO_IDX_H
#define ISO_IDX_H

#include "format/SvcFormat.h"
#include "IsoBoxDef.h"
#include "../index/IndexCache.h"

/**
 *  Iso index mode
 */
#define ISO_IDX_MODE_WRITE      (0U)    /**< Write mode */
#define ISO_IDX_MODE_READ       (1U)    /**< Read mode */
#define ISO_IDX_MODE_RECOVER    (2U)    /**< Recover mode */
#define ISO_IDX_MODE_MAX        (3U)    /**< For check use */

/**
 *  Iso index item type
 */
#define ISO_IDX_TYPE_IDX_INFO   (0U)    /**< Index information */
#define ISO_IDX_TYPE_MEDIA_INFO (1U)    /**< Media infotmation */
#define ISO_IDX_TYPE_V          (2U)    /**< Video size */
#define ISO_IDX_TYPE_VO         (3U)    /**< Video offset */
#define ISO_IDX_TYPE_CTTS       (4U)    /**< Video ctts */
#define ISO_IDX_TYPE_K          (5U)    /**< Video key frame number */
#define ISO_IDX_TYPE_V_STTS     (6U)    /**< Video stts */
#define ISO_IDX_TYPE_V_DTS      (7U)    /**< Video DTS */
#define ISO_IDX_TYPE_A          (8U)    /**< Audio size */
#define ISO_IDX_TYPE_AO         (9U)    /**< Audio offset */
#define ISO_IDX_TYPE_A_STTS     (10U)   /**< Audio stts */
#define ISO_IDX_TYPE_T          (11U)   /**< Text size */
#define ISO_IDX_TYPE_TO         (12U)   /**< Text offset */
#define ISO_IDX_TYPE_T_STTS     (13U)   /**< Text stts */
#define ISO_IDX_TYPE_MAX        (14U)   /**< For check use */

/**
 *  Iso index STTS information
 */
typedef struct {
    UINT32 SampleNo;       /**< Sample number */
    UINT32 SampleDelta;    /**< Time-scale of the media */
} ISO_IDX_STTS_INFO_s;

/**
 *  Iso index buffer information
 */
typedef struct {
    UINT32 StartFrameNo;        /**< IsoIdx buffer read/write start number */
    UINT32 EndFrameNo;          /**< IsoIdx buffer read/write end number */
    UINT32 FrameCount;          /**< IsoIdx buffer read/write count */
    UINT32 PreSttsEntryNo;      /**< PreSttsEntryNo */
    UINT32 PreSttsNo;           /**< PreSttsNo */
    UINT64 PreSttsSum;          /**< PreSttsSum */
    UINT64 SttsSum;             /**< SttsSum*/
    UINT64 Buffer[INDEX_CACHE_MAX_CACHED_COUNT];    /**< Buffer */
} ISO_IDX_STTS_BUFFER_INFO_s;

/**
 *  Iso video index information
 */
typedef struct {
    UINT32 GOPSize;                 /**< GOP size */
    UINT16 M;                       /**< Number of pictures between reference pictures */
    UINT16 N;                       /**< Number of pictures between I pictures */
    UINT8 Mode;                     /**< SVC_VIDEO_MODE_P, SVC_VIDEO_MODE_I_A_FRM_PER_SMP, SVC_VIDEO_MODE_I_A_FLD_PER_SMP */
} ISO_IDX_VIDEO_INFO_s;

/**
 *  Iso index track information
 */
typedef struct {
    UINT8 TrackId;      /**< Track id */
    UINT8 TrackType;    /**< Track type (SVC_MEDIA_TRACK_TYPE_VIDEO, SVC_MEDIA_TRACK_TYPE_AUDIO, SVC_MEDIA_TRACK_TYPE_TEXT)  */
    UINT32 TimeScale;           /**< The ticks per second */
    UINT32 OrigTimeScale;       /**< The original TimeScale */
    UINT32 TimePerFrame;        /**< The ticks per frame */
    ISO_IDX_VIDEO_INFO_s Video; /**< ISO_IDX_VIDEO_INFO_s */
} ISO_IDX_TRACK_INFO_s;

#endif
