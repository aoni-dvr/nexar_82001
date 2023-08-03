/**
 * @file SvcIso.h
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
#ifndef CONTAINER_ISO_H
#define CONTAINER_ISO_H

#include "format/SvcFormatDef.h"

#define SVC_ISO_BOX_SIZE_TAG_SIZE  (8U) /**< Iso box header size */

#define SVC_ISO_MAX_VIDEO_FPS   (120U)  /**< Max number of video frames per second (120fps) */
#define SVC_ISO_MAX_AUDIO_FPS   (48U)   /**< Max number of audio frames per second (48fps) */
#define SVC_ISO_MAX_TEXT_FPS    (120U)   /**< Max number of text frames per second (120fps) */

#define SVC_ISO_MAX_FRAGMENTS               (10U * 60U)                     /**< Max number of fragments in a clip (10 minutes) */
#ifdef CONFIG_ENABLE_CAPTURE_TIME_MODE
#define SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG    (SVC_ISO_MAX_VIDEO_FPS * 2U)    /**< Max number of video frames in a fragment (fps * 2, frame rate may vary in CaptureTimeMode) */
#define SVC_ISO_MAX_AUDIO_FRAME_PER_FRAG    (SVC_ISO_MAX_AUDIO_FPS * 2U)    /**< Max number of audio frames in a fragment (fps * 2, frame rate may vary in CaptureTimeMode) */
#define SVC_ISO_MAX_TEXT_FRAME_PER_FRAG     (SVC_ISO_MAX_TEXT_FPS * 2U)     /**< Max number of text frames in a fragment (fps * 2, frame rate may vary in CaptureTimeMode) */
#else
#ifdef CONFIG_SUPPORT_SDK7_SVC_CLIPS
#define SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG    SVC_ISO_MAX_VIDEO_FPS           /**< Max number of video frames in a fragment (fps) */
#define SVC_ISO_MAX_AUDIO_FRAME_PER_FRAG    (SVC_ISO_MAX_AUDIO_FPS + ((SVC_ISO_MAX_AUDIO_FPS + 9U) / 10U))   /**< Max number of audio frames in a fragment (fps + 10% tolerance, since svc in sdk7 didn't limit the length of audio track) */
#define SVC_ISO_MAX_TEXT_FRAME_PER_FRAG     (SVC_ISO_MAX_TEXT_FPS + ((SVC_ISO_MAX_TEXT_FPS + 9U) / 10U))     /**< Max number of text frames in a fragment (fps + 10% tolerance, since svc in sdk7 didn't limit the length of text track) */
#else
#define SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG    SVC_ISO_MAX_VIDEO_FPS           /**< Max number of video frames in a fragment (fps) */
#define SVC_ISO_MAX_AUDIO_FRAME_PER_FRAG    SVC_ISO_MAX_AUDIO_FPS           /**< Max number of audio frames in a fragment (fps) */
#define SVC_ISO_MAX_TEXT_FRAME_PER_FRAG     SVC_ISO_MAX_TEXT_FPS            /**< Max number of text frames in a fragment (fps) */
#endif
#endif
#define SVC_ISO_MAX_FRAME_PER_FRAG          SVC_ISO_MAX_VIDEO_FRAME_PER_FRAG    /**< Max number of frames per fragment */

/**
 *  Rotation definition
 */
#define SVC_ISO_ROTATION_NONE          (0x01U) /**< No rotation */
#define SVC_ISO_ROTATION_FLIP_HORI     (0x02U) /**< Flip horizontally */
#define SVC_ISO_ROTATION_ROTATE180     (0x03U) /**< Rotation by 180 degrees */
#define SVC_ISO_ROTATION_FLIP_VERT     (0x04U) /**< Flip vertically */
#define SVC_ISO_ROTATION_TRANSPOSE     (0x05U) /**< Transpose */
#define SVC_ISO_ROTATION_ROTATE90      (0x06U) /**< Rotation by 90 degrees */
#define SVC_ISO_ROTATION_TRANSVERSE    (0x07U) /**< Rotate transversely */
#define SVC_ISO_ROTATION_ROTATE270     (0x08U) /**< Rotation by 270 degrees */

/**
 * ISO Video HVCC track information
 */
typedef struct {
    UINT8 Generalconfiguration[SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH]; /**< VPS general config */
    UINT8 ChromaFormat;             /**< chroma format in sps */
    UINT8 BitDepthLumaMinus8;       /**< The bit depth of the samples of the luma in sps */
    UINT8 BitDepthChromaMinus8;     /**< The bit depth of the samples of the chroma in sps */
    UINT8 NumTemporalLayers;        /**< Profile information */
    UINT8 TemporalIdNested;         /**< Nested level */
    UINT8 NumofArray;       /**< The number of parameter set */
    UINT8 NALULength;       /**< NALU length */
    UINT8 PicSizeInCtbsY;   /**< PicSizeInCtbsY */
    UINT16 VPSLen;          /**< The SPS size of H265 */
    UINT16 SPSLen;          /**< The SPS size of H265 */
    UINT16 PPSLen;          /**< The PPS size of H265 */
    UINT8 VPS[SVC_FORMAT_MAX_VPS_LENGTH];   /**< The VPS of H265 */
    UINT8 SPS[SVC_FORMAT_MAX_SPS_LENGTH];   /**< The SPS of H265 */
    UINT8 PPS[SVC_FORMAT_MAX_PPS_LENGTH];   /**< The PPS of H265 */
} SVC_ISO_VIDEO_HVC_TRACK_INFO_s;

/**
 *  AVCC('avcc')
 */
typedef struct {
    UINT8 NALULength;       /**< NALU length */
    UINT16 SPSLen;          /**< The SPS size of H264 */
    UINT16 PPSLen;          /**< The PPS size of H264 */
    UINT8 SPS[SVC_FORMAT_MAX_SPS_LENGTH];   /**< The SPS of H264 */
    UINT8 PPS[SVC_FORMAT_MAX_PPS_LENGTH];   /**< The PPS of H264 */
} SVC_ISO_VIDEO_AVC_TRACK_INFO_s;

/**
 * ISO Audio track information
 */
typedef struct {
    UINT32 SttsCount;           /**< The count of the STTS entry in current index buffer */
    UINT64 StszOffset;          /**< The start offset of the index (It stores the size entry of the audio track.) */
    UINT64 StcoOffset;          /**< The start offset of the index (It stores the file offset entry of the audio track.) */
    UINT64 SttsOffset;          /**< The start offset of the index (It stores the STTS entry of the audio track.) */
} SVC_ISO_AUDIO_TRACK_INFO_s;

/**
 * ISO Text track information
 */
typedef struct {
    UINT32 SttsCount;           /**< The count of the STTS entry in current index buffer */
    UINT64 StszOffset;          /**< The start offset of the index (It stores the size entry of the text track.) */
    UINT64 StcoOffset;          /**< The start offset of the index (It stores the file offset entry of the text track.) */
    UINT64 SttsOffset;          /**< The start offset of the index (It stores the STTS entry of the text track.) */
} SVC_ISO_TEXT_TRACK_INFO_s;

/**
 * ISO Video track information
 */
typedef struct {
    UINT32 KeyFrameNo;              /**< The number of the key frame in current index buffer */
    UINT32 KeyFrameCount;           /**< The count of the key frame in current index buffer */
    UINT32 SttsCount;               /**< The count of the STTS entry in current index buffer */
    UINT32 FrameNumGOP;             /**< The frame number counter in a GOP (Reset in Idr) */
    UINT64 StszOffset;              /**< The start offset of the index (It stores the size entry of the video track.) */
    UINT64 StcoOffset;              /**< The start offset of the index (It stores the file offset entry of the video track.) */
    UINT64 StssOffset;              /**< The start offset of the index (It stores the key frame entry of the video track.) */
    UINT64 CttsOffset;              /**< The start offset of the index (It stores the CTTS entry of the video track.) */
    UINT64 SttsOffset;              /**< The start offset of the index (It stores the STTS entry of the video track.) */
    SVC_ISO_VIDEO_HVC_TRACK_INFO_s Hvc; /**< The track information in the private data (See AMBA_ISO_TRACK_INFO_s.) */
    SVC_ISO_VIDEO_AVC_TRACK_INFO_s Avc; /**< The track information in the private data (See AMBA_ISO_TRACK_INFO_s.) */
} SVC_ISO_VIDEO_TRACK_INFO_s;

/**
 * ISO Private information
 */
typedef struct {
    UINT32 FtypSize;        /**< The box size of the Iso ftyp */
    UINT32 FragmentCount;   /**< The fragment count (for FMP4) */
    UINT64 FirstFragmentPos;    /** The position of the 1st fragment (for FMP4) */
    SVC_ISO_VIDEO_TRACK_INFO_s VideoTrack[SVC_FORMAT_MAX_VIDEO_TRACK_PER_MEDIA];/**< The video track information in the private data */
    SVC_ISO_AUDIO_TRACK_INFO_s AudioTrack[SVC_FORMAT_MAX_AUDIO_TRACK_PER_MEDIA];/**< The audio track information in the private data */
    SVC_ISO_TEXT_TRACK_INFO_s TextTrack[SVC_FORMAT_MAX_TEXT_TRACK_PER_MEDIA];   /**< The text track information in the private data */
} SVC_ISO_PRIV_INFO_s;

#endif

