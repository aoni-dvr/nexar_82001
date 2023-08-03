/**
 * @file SvcFormatDef.h
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
#ifndef CONTAINER_FORMATDEF_H
#define CONTAINER_FORMATDEF_H

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaPrint.h>
#include <AmbaCFS.h>
#include <AmbaWrap.h>
#include <AmbaUtility.h>
#include <AmbaMisraFix.h>

/**
 * Error code
 */
#define MUXER_ERR_0000      (MUXER_ERR_BASE)            /**< Invaid arguement */
#define MUXER_ERR_0001      (MUXER_ERR_BASE | 0x1U)     /**< Fatal error */
#define MUXER_ERR_0002      (MUXER_ERR_BASE | 0x2U)     /**< IO error */

#define DEMUXER_ERR_0000    (DEMUXER_ERR_BASE)          /**< Invaid arguement */
#define DEMUXER_ERR_0001    (DEMUXER_ERR_BASE | 0x1U)   /**< Fatal error */
#define DEMUXER_ERR_0002    (DEMUXER_ERR_BASE | 0x2U)   /**< IO error */

/**
 * Muxer event (in UINT8)
 */
#define SVC_MUXER_EVENT_NONE           (0x00U) /**< No special event occurs. */
#define SVC_MUXER_EVENT_REACH_END      (0x01U) /**< The event raised when Muxer completes the muxing */
#define SVC_MUXER_EVENT_REACH_LIMIT    (0x02U) /**< The event raised in reaching the limitation of media time or file size */

#define SVC_FORMAT_MAX_DATE_SIZE   (20U)   /**< The max size of the date information */
#define SVC_FORMAT_MAX_TIME_SIZE   (12U)   /**< the max size of the time information */

#define SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA   (1U)    /**< The maximum number of video tracks in a media */
#define SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA   (1U)    /**< The maximum number of audio tracks in a media */
#define SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA    (1U)    /**< The maximum number of text tracks in a media */
/** The maximum number of tracks in a media */
#define SVC_FORMAT_MAX_TRACK_PER_MEDIA         (SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA + SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA + SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA)

#define SVC_FORMAT_MAX_FRAME_PER_IMAGE         (3U)    /**< The maximum number of frames in an image (not support MPO) */

#define SVC_FORMAT_MAX_TIMESTAMP   (0xFFFFFFFFFFFFFFFFULL) /**< The max timestamp of the format */

#define SVC_FORMAT_MAX_VPS_LENGTH  (64U)   /**< The maximum length of the VPS */
#define SVC_FORMAT_MAX_SPS_LENGTH  (96U)   /**< The maximum length of the SPS */
#define SVC_FORMAT_MAX_PPS_LENGTH  (64U)   /**< The maximum length of the PPS */
#define SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH  (12U)   /**< The length of the general config in VPS */

/**
 * The base structure of a media information object
 */
typedef struct {
    UINT8 MediaType;        /**< The media type (See AMBA_MEDIA_INFO_TYPE_e.) */
    UINT8 Valid;            /**< The value indicating if the media context is valid */
    UINT8 SubFormat;        /**< The sub_format of the media (MSMP4, FUJIMOV) */
    UINT64 Size;            /**< The file size */
    UINT32 CreationTime;    /**< Creation time of the media */
    UINT32 ModificationTime;/**< Modification time of the media */
    char Name[AMBA_CFS_MAX_FILENAME_LENGTH];    /**< The media name */
    char Date[SVC_FORMAT_MAX_DATE_SIZE];    /**< The media date */
} SVC_MEDIA_INFO_s;

#define SVC_FORMAT_MID_A_MASK  (0x20U) /**< Audio media mask */
#define SVC_FORMAT_MID_T_MASK  (0x40U) /**< Text media mask */


#define SVC_FORMAT_MID_H264    (0x01U) /**< H264 media id */
#define SVC_FORMAT_MID_AVC     (0x02U) /**< AVC media id */
#define SVC_FORMAT_MID_MJPEG   (0x03U) /**< MJPEG media id */
#define SVC_FORMAT_MID_H265    (0x04U) /**< H265 media id */
#define SVC_FORMAT_MID_HVC     (0x05U) /**< HVC media id */
#define SVC_FORMAT_MID_AAC     (SVC_FORMAT_MID_A_MASK | 0X01U) /**< AAC media id, SVC_FORMAT_MID_A_MASK | 0x01*/
#define SVC_FORMAT_MID_PCM     (SVC_FORMAT_MID_A_MASK | 0X02U) /**< PCM media id, SVC_FORMAT_MID_A_MASK | 0x02 */
#define SVC_FORMAT_MID_ADPCM   (SVC_FORMAT_MID_A_MASK | 0x03U) /**< ADPCM media id, SVC_FORMAT_MID_A_MASK | 0x03 */
#define SVC_FORMAT_MID_MP3     (SVC_FORMAT_MID_A_MASK | 0x04U) /**< MP3 media id, SVC_FORMAT_MID_A_MASK | 0x04 */
#define SVC_FORMAT_MID_AC3     (SVC_FORMAT_MID_A_MASK | 0x05U) /**< AC3 media id, SVC_FORMAT_MID_A_MASK | 0x05 */
#define SVC_FORMAT_MID_WMA     (SVC_FORMAT_MID_A_MASK | 0x06U) /**< WMA media id, SVC_FORMAT_MID_A_MASK | 0x06 */
#define SVC_FORMAT_MID_OPUS    (SVC_FORMAT_MID_A_MASK | 0x07U) /**< OPUS media id, SVC_FORMAT_MID_A_MASK | 0x07*/
#define SVC_FORMAT_MID_LPCM    (SVC_FORMAT_MID_A_MASK | 0x08U) /**< LPCM media id, SVC_FORMAT_MID_A_MASK | 0x08 */
#define SVC_FORMAT_MID_TEXT    (SVC_FORMAT_MID_T_MASK | 0x01U) /**< Text media id, SVC_FORMAT_MID_T_MASK | 0x01 */
#define SVC_FORMAT_MID_MP4S    (SVC_FORMAT_MID_T_MASK | 0x02U) /**< MP4S media id, SVC_FORMAT_MID_T_MASK | 0x02 */

#endif
