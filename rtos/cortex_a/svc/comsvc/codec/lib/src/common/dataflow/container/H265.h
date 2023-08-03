/**
 * @file H265.h
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
#ifndef CONTAINER_H265_H
#define CONTAINER_H265_H

#include "format/SvcFormat.h"
/**
 *  H265
 */
#define SVC_H265_STRAT_CODE_SIZE           (4U)     /**< H265 start code size */
#define SVC_H265_GOP_NALU_SIZE             (22U)    /**< H265 gop nalu size */

#define SVC_H265_MAX_SHORT_TERM_RPS_COUNT  (64U)    /**< H265 max short-term rps count */

static inline UINT8 SVC_H265_NALU_TYPE(UINT8 Value) {return (Value & 0x7eU) >> 1U;}

/* NAL unit type */
#define    SVC_H265_NALU_TYPE_TRAIL_N      0U   /**< TRAIL_N */
#define    SVC_H265_NALU_TYPE_TRAIL_R      1U   /**< TRAIL_R */
#define    SVC_H265_NALU_TYPE_TSA_N        2U   /**< TSA_N */
#define    SVC_H265_NALU_TYPE_TSA_R        3U   /**< TSA_R */
#define    SVC_H265_NALU_TYPE_STSA_N       4U   /**< STSA_N */
#define    SVC_H265_NALU_TYPE_STSA_R       5U   /**< STSA_R */
#define    SVC_H265_NALU_TYPE_RADL_N       6U   /**< RADL_N */
#define    SVC_H265_NALU_TYPE_RADL_R       7U   /**< RADL_R */
#define    SVC_H265_NALU_TYPE_RASL_N       8U   /**< RASL_N */
#define    SVC_H265_NALU_TYPE_RASL_R       9U   /**< RASL_R */
#define    SVC_H265_NALU_TYPE_BLA_W_LP     16U  /**< BLA_W_LP */
#define    SVC_H265_NALU_TYPE_BLA_W_RADL   17U  /**< BLA_W_RADL */
#define    SVC_H265_NALU_TYPE_BLA_N_LP     18U  /**< BLA_N_LP */
#define    SVC_H265_NALU_TYPE_IDR_W_RDAL   19U  /**< IDR_W_RADL */
#define    SVC_H265_NALU_TYPE_IDR_N_LP     20U  /**< IDR_N_LP */
#define    SVC_H265_NALU_TYPE_CRA_NUT      21U  /**< CRA_NUT*/
#define    SVC_H265_NALU_TYPE_VPS          32U  /**< VPS */
#define    SVC_H265_NALU_TYPE_SPS          33U  /**< SPS */
#define    SVC_H265_NALU_TYPE_PPS          34U  /**< PPS */
#define    SVC_H265_NALU_TYPE_AUD          35U  /**< AUD */
#define    SVC_H265_NALU_TYPE_SEI_PREFIX   39U  /**< SEI_PREFIX */
#define    SVC_H265_NALU_TYPE_SEI_SUFFIX   40U  /**< SEI_SUFFIX */


/**
 *  H265 NALU
 */
typedef struct {
    UINT8 FirstNALU;        /**< First NALU */
    INT32 PrefixLen;        /**< start code prefix length, eg: 3 = 0x000001 */
    UINT32 Len;             /**< NAL length */
    UINT32 ForbiddenBit;    /**< should be 0 */
    UINT8 NaluType;         /**< AMP_H265_NALU_TYPE_e */
    UINT32 NuhLayerId;      /**< NuhLayerId */
    UINT32 TemporalId;      /**< TemporalId */
} SVC_H265_NALU_s;


/**
 *  H265 VPS
 */
typedef struct {
    UINT8 Generalconfiguration[SVC_FORMAT_MAX_VPS_GEN_CFG_LENGTH]; /**< VPS general config */
    UINT8 VpsMaxSubLayersMinus1;     /**< Profile information */
} SVC_H265_VPS_s;

/**
 *  H265 VUI
 */
typedef struct {
    UINT8 AspectRatioInfoPresentFlag;               /**< u(1) */
    UINT8 AspectRatioIdc;                           /**< u(8) */
    UINT16 SarWidth;                                /**< u(16) */
    UINT16 SarHeight;                               /**< u(16) */
    UINT32 NumUnitsInTick;                          /**< u(32) */
    UINT32 TimeScale;                               /**< u(32) */
} SVC_H265_VUI_s;

/**
 *  H265 SPS
 */
typedef struct {
    UINT8 ChromaFormat;             /**< chroma format in sps */
    UINT8 BitDepthLumaMinus8;       /**< The bit depth of the samples of the luma in sps */
    UINT8 BitDepthChromaMinus8;     /**< The bit depth of the samples of the chroma in sps */
    UINT8 SpsMaxSubLayersMinus1;    /**< Profile information */
    UINT8 TemporalIdNested;         /**< Nested level */
    UINT8 PicSizeInCtbsY;           /**< PicSizeInCtbsY */
    SVC_H265_VUI_s VuiParam;        /**< vui_seq_parameters_t */
} SVC_H265_SPS_s;

/**
 *  H265 decode configuration
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
    UINT16 SarWidth;        /**< SarWidth */
    UINT16 SarHeight;       /**< SarHeight */
    UINT32 NumUnitsInTick;  /**< NumUnitsInTick */
    UINT32 TimeScale;       /**< TimeScale */
} SVC_H265_DEC_CONFIG_s;

/**
 * Parse H.265 VPS/SPS
 *
 * @param [in] Buffer Source buffer
 * @param [in] BufferSize Buffer length
 * @param [out] VPS Space to store VPS
 * @param [out] SPS Space to store SPS
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_HeaderParse(UINT8 *Buffer, UINT32 BufferSize, SVC_H265_VPS_s *VPS, SVC_H265_SPS_s *SPS);

/**
 * Read from stream
 *
 * @param [in] Stream Stream handler
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [in] FrameSize Frame size
 * @param [out] DataSize The returned data size
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 FrameSize, UINT32 *DataSize);

/**
 * Get Hvc configuration
 *
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [out] H265 The returned configuration
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_GetHvcConfig(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H265_DEC_CONFIG_s *H265);

/**
 * Get frame size
 *
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [in] Align Alignment
 * @param [out] FrameSize Frame size
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_GetFrameSize(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, UINT32 Align, UINT32 *FrameSize);

/**
 * Write to stream
 *
 * @param [in] Stream Stream handler
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [out] WriteSize The return written size
 * @param [in] Align Alignment
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *WriteSize, UINT32 Align);

/**
 * Put gop header to buffer
 *
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [in] Video Video track information
 * @param [in] SkipFirstI Skip first I frame
 * @param [in] SkipLastI Skip last I frame
 * @param [in] PTS Pts
 * @param [in] TimePerFrame Time per frame
 * @param [out] NewBufferOffset The resulted buffer offset
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH265_PutGOPHeader(UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, const SVC_VIDEO_TRACK_INFO_s *Video, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS, UINT32 TimePerFrame, UINT32 *NewBufferOffset);
#endif

