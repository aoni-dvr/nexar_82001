/**
*  @file AmbaMux.h
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
*  @details amba muxer
*
*/

#ifndef AMBA_MUX_H
#define AMBA_MUX_H

#include "AmbaMux_Def.h"

#define AMBA_MP4FMT_ERR_INVALID_ARG      (0x00000001U)  /**< Invalid argument */
#define AMBA_MP4FMT_ERR_FATAL            (0x00000002U)  /**< Fatal error */
#define AMBA_MP4FMT_ERR_IO_ERROR         (0x00000003U)  /**< IO error */

#define UNIX_MAC_TIME_DIFF      (2082844800U)   /**< UNIX MAX TIME DIFF */

#define TRACK_TYPE_VIDEO        (0U)    /**< Track type video */
#define TRACK_TYPE_AUDIO        (1U)    /**< Track type audio */
#define TRACK_TYPE_DATA         (2U)    /**< Track type data */
#define TRACK_TYPE_MAX          (3U)    /**< Maximum number of track types */

#define AMBA_MP4FMT_FLUSH_NONE          (0U)    /**< None */
#define AMBA_MP4FMT_FLUSH_REACH_GOP     (1U)    /**< Reach GOP */
#define AMBA_MP4FMT_FLUSH_REACH_LIMIT   (2U)    /**< Split file (extend with InitDTS) */
#define AMBA_MP4FMT_FLUSH_REACH_END     (3U)    /**< End of the session */
#define AMBA_MP4FMT_FLUSH_MAX           (4U)    /**< Maximum number of flush option */

/**
 *  Mp4Fmt video configuration
 */
typedef struct {
    UINT32 VideoCoding;         /**< Video coding */
    UINT32 M;                   /**< M interval */
    UINT32 N;                   /**< N interval */
    UINT32 IdrInterval;         /**< Idr interval */
    UINT32 TimeScale;           /**< Time scale */
    UINT32 VdWidth;             /**< Video width */
    UINT32 VdHeight;            /**< Video height */
    UINT32 Orientation;         /**< Tkhd */
    UINT8  IRCycle;             /**< Video IRCycle*/
    UINT8  IsSpherical;         /**< Enable spherical metadata*/
    UINT8  NumSlice;            /**< Numeber of slices */
    UINT8  NumTile;             /**< Number of tiles */
    UINT8  ClosedGop;           /**< If the first GOP is a closed GOP. 1: Closed(IPBB...), 0: Open(IBBP...) */
    UINT8  CaptureTimeMode;     /**< Capture time mode. 0: Disable, 1: Enable */
    UINT32 CaptureTimeScale;    /**< Time scale of capture timestamp */
} AMBA_MP4FMT_VIDEO_CFG_s;

/**
 *  Mp4Fmt audio configuration
 */
typedef struct {
    UINT32 AudioCoding;
    UINT32 AuSample;            /**< Audio sample rate */
    UINT32 AuVolume;            /**< Audio volume */
    UINT32 AuWBitsPerSample;    /**< Audio sample size */
    UINT32 AuChannels;          /**< Audio channel number */
    UINT32 AuBrate;             /**< Audio average bit rate */
    UINT32 AuFormat;            /**< 0; little, 1: big endian */
} AMBA_MP4FMT_AUDIO_CFG_s;

typedef struct {
    /* filled by user */
    UINT8  Enable;              /**< enable mehd box or not */
    UINT32 TimeScale;           /**< timescale for duration, in muhd */
    UINT32 DefDuration;         /**< default duration written in mehd */

    /* filled by mp4fmt */
    UINT64 FileOffset;          /**< mehd file offset */
    UINT32 FragDuration;        /**< actual mehd fragment duration */
                                /**< user should update the actual duration if (FragDuration != InitDuration) */
} AMBA_MP4FMT_MEHD_CTRL_s;

/**
 *  Mp4Fmt format inforamtion
 */
typedef struct {
    UINT32                   SubType;            /**< SubType */
    UINT32                   CreationTime;       /**< Creation time */
    UINT32                   ModificationTime;   /**< Modification time */
    UINT32                   UserDataSize;       /**< User data size */
    UINT8                    *UserData;          /**< User data buffer. */
                                                 /**< If pfnBufCopy allows DMA copy and the size exceeds AMBA_CFS_DMA_SIZE_THRESHOLD, */
                                                 /**< the buffer should be non-cached. */
    AMBA_MP4FMT_VIDEO_CFG_s  VideoCfg;           /**< Video configuration */
    AMBA_MP4FMT_AUDIO_CFG_s  AudioCfg;           /**< Audio configuration */

    AMBA_MP4FMT_MEHD_CTRL_s  MehdCtrl;           /**< Mehd control */
} AMBA_MP4FMT_HDR_s;

/**
 *  Mp4Fmt buffer information
 */
typedef struct {
    ULONG   BufferBase;                     /**< Buffer address */
    UINT32  BufferSize;                     /**< Buffer size, query via AmbaMux_EvalFragSize */
    UINT32  SampleTicks[TRACK_TYPE_MAX];    /**< Ticks per sample*/
} MP4FMT_BUF_INFO_s;

/**
 *  Mp4Fmt controller
 */
typedef struct {
    AMBA_MP4FMT_HDR_s  Mp4Hdr;                      /**< AMBA_MP4FMT_HDR_s */
    MP4FMT_BUF_INFO_s  BufInfo;                     /**< MP4FMT_BUF_INFO_s */
    ULONG              DescBase;                    /**< Descriptor buffer */
    UINT32             DescSize;                    /**< Descriptor buffer size, query via AmbaMux_EvalDescSize */
    UINT32             TrafValid[TRACK_TYPE_MAX];   /**< Valid flag */
    UINT32             (*pfnBufCopy)(ULONG Dst, ULONG Src, UINT32 Size);  /**< Function pointer that copy data to the buffer */
} MP4FMT_CTRL_s;

/**
 *  initialize muxer.
 *
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_Init(void);

/**
 *  Set the initial delay time for each track.
 *  (For emergency event record, the initial diff will be lost.)
 *
 *  @param [in] pCtrl Format controller
 *  @param [in] InitDelay Initial delay in ms (An array of size TRACK_TYPE_MAX)
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_SetInitDelay(MP4FMT_CTRL_s *pCtrl, const UINT32 *InitDelay);

/**
 *  Process the muxing format controller.
 *
 *  @param [in] pCtrl Format controller
 *  @param [in] pDesc The incoming resource descriptor
 *  @param [in] IsFlush Flush option (AMBA_MP4FMT_FLUSH_XXXX)
 *  @param [out] Size The size of data that was flushed
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_Proc(MP4FMT_CTRL_s *pCtrl, const AMBA_RSC_DESC_s *pDesc, UINT32 IsFlush, UINT32 *Size);

/**
 *  Evaluate the size of the descriptor buffer.
 *
 *  @param [in] SubType Subtype of the foramt
 *  @param [in] pEval Evaluating information
 *  @param [out] DescBufferSize The returned descriptor buffer size
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_EvalDescSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, UINT32 *DescBufferSize);

/**
 *  Evaluate the size of a fragment buffer.
 *
 *  @param [in] SubType Subtype of the foramt
 *  @param [in] pEval Evaluating information
 *  @param [out] pBufInfo The returned buffer information
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_EvalFragSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, MP4FMT_BUF_INFO_s *pBufInfo);

/**
 *  Evaluate the size of a file.
 *
 *  @param [in] SubType Subtype of the foramt
 *  @param [in] pEval Evaluating information
 *  @param [out] FileSize The returned file size
 *  @return 0 - OK, others - AMBA_MP4FMT_ERR_XXXX
 */
UINT32 AmbaMux_EvalFileSize(UINT32 SubType, const AMBA_REC_EVAL_s *pEval, UINT32 *FileSize);

#endif  /* AMBA_MUX_H */
