/**
 * @file H264.h
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
#ifndef CONTAINER_H264_H
#define CONTAINER_H264_H

#include "format/SvcFormat.h"

#define H264_SIMPLE_PARSE /**< Simple parse */

/**
 *  H.264
 */
#define SVC_H264_STRAT_CODE_SIZE   (4U)     /**< H264 start code size */
#define SVC_H264_GOP_NALU_SIZE     (26U)    /**< H264 GOP nalu size */
#define SVC_H264_EOS_NALU_SIZE     (5U)     /**< H264 eos nalu size */
#define SVC_H264_ALIGN_SIZE        (32U)    /**< H264 align size */

static inline UINT8 SVC_H264_NALU_TYPE(UINT8 Value) {return (Value & 0x1fU);}

/* chroma format */
#define YUV400  (0U)    /**< YUV400 */
#define YUV420  (1U)    /**< YUV420 */
#define YUV422  (2U)    /**< YUV422 */
#define YUV444  (3U)    /**< YUV444 */

/* NAL unit type */
#define SVC_H264_NALU_TYPE_UNKNOWN     (0U) /**< H264 NALU type UNKNOWN */
#define SVC_H264_NALU_TYPE_SLICE       (1U) /**< H264 NALU type SLICE */
#define SVC_H264_NALU_TYPE_DPA         (2U) /**< H264 NALU type DPA */
#define SVC_H264_NALU_TYPE_DPB         (3U) /**< H264 NALU type DPB */
#define SVC_H264_NALU_TYPE_DPC         (4U) /**< H264 NALU type DPC */
#define SVC_H264_NALU_TYPE_IDR         (5U) /**< H264 NALU type IDR */
#define SVC_H264_NALU_TYPE_SEI         (6U) /**< H264 NALU type SEI */
#define SVC_H264_NALU_TYPE_SPS         (7U) /**< H264 NALU type SPS */
#define SVC_H264_NALU_TYPE_PPS         (8U) /**< H264 NALU type PPS */
#define SVC_H264_NALU_TYPE_AUD         (9U) /**< H264 NALU type AUD */
#define SVC_H264_NALU_TYPE_EOSEQ       (10U)/**< H264 NALU type EOSEQ */
#define SVC_H264_NALU_TYPE_EOSTREAM    (11U)/**< H264 NALU type EOSTREAM */
#define SVC_H264_NALU_TYPE_FILL        (12U)/**< H264 NALU type FILL */

/* slice type*/
#define P_SLICE     (0U)    /**< P slice */
#define B_SLICE     (1U)    /**< B slice */
#define I_SLICE     (2U)    /**< I slice */
#define SP_SLICE    (3U)    /**< SP slice */
#define SI_SLICE    (4U)    /**< SI slice */

/**
 *  H264 NALU
 */
typedef struct {
    UINT8 FirstNALU;                /**< the flag of lst nalu */
    INT32 PrefixLen;                /**< start code prefix length, eg: 3 = 0x000001 */
    UINT32 Len;                     /**< NAL length */
    UINT32 ForbiddenBit;            /**< should be 0 */
    UINT32 NalReferenceIdc;         /**< NalReferenceIdc */
    UINT8 NaluType;                 /**< SVC_H264_NALU_TYPE_e */
} SVC_H264_NALU_s;

/**
 *  H264 slice information
 */
typedef struct {
    UINT32 FirstMbInSlice;  /**< FirstMbInSlice */
    UINT32 FrameNum;        /**< FrameNum */
    UINT8 PicParameterSetId;/**< PicParameterSetId */
    UINT8 FieldPicFlag;     /**< FieldPicFlag */
    UINT16 BottomFieldFlag; /**< BottomFieldFlag */
    UINT16 IdrPicId;        /**< IdrPicId */
    UINT8 SliceType;        /**< SliceType */
} SVC_H264_SliceInfo_s;

/**
 *  H264 HRD
 */
typedef struct {
    UINT8 CpbCntMinus1;                         /**< ue(v) */
    UINT8 CbrFlag[32];                          /**< u(1) */
    UINT32 BitRateScale;                        /**< u(4) */
    UINT32 CpbSizeScale;                        /**< u(4) */
    UINT32 BitRateValueMinus1[32];              /**< ue(v) */
    UINT32 CpbSizeValueMinus1[32];              /**< ue(v) */
    UINT16 InitialCpbRemovalDelayLengthMinus1;  /**< u(5) */
    UINT16 CpbRemovalDelayLengthMinus1;         /**< u(5) */
    UINT16 DpbOutputDelayLengthMinus1;          /**< u(5) */
    UINT16 TimeOffsetLength;                    /**< u(5) */
} SVC_H264_HRD_s;

/**
 *  H264 VUI
 */
typedef struct {
    UINT8 AspectRatioInfoPresentFlag;           /**< u(1) */
    UINT8 AspectRatioIdc;                       /**< u(8) */
    UINT8 OverscanInfoPresentFlag;              /**< u(1) */
    UINT8 OverscanAppropriateFlag;              /**< u(1) */
    UINT8 VideoSignalTypePresentFlag;           /**< u(1) */
    UINT8 VideoFormat;                          /**< u(3) */
    UINT8 VideoFullRangeFlag;                   /**< u(1) */
    UINT8 ColourDescriptionPresentFlag;         /**< u(1) */
    UINT8 ColourPrimaries;                      /**< u(8) */
    UINT8 TransferCharacteristics;              /**< u(8) */
    UINT8 MatrixCoefficients;                   /**< u(8) */
    UINT8 ChromaLocationInfoPresentFlag;        /**< u(1) */
    UINT8 ChromaSampleLocTypeTopField;          /**< ue(v) */
    UINT8 ChromaSampleLocTypeBottomField;       /**< ue(v) */
    UINT8 TimingInfoPresentFlag;                /**< u(1) */
    UINT8 FixedFrameRateFlag;                   /**< u(1) */
    UINT8 NalHrdParametersPresentFlag;          /**< u(1) */
    UINT8 LowDelayHrdFlag;                      /**< u(1) */
    UINT8 PicStructPresentFlag;                 /**< u(1) */
    UINT8 BitstreamRestrictionFlag;             /**< u(1) */
    UINT8 MotionVectorsOverPicBoundariesFlag;   /**< u(1) */
    UINT8 MaxBytesPerPicDenom;                  /**< ue(v) */
    UINT8 MaxBitsPerMbDenom;                    /**< ue(v) */
    UINT8 VclHrdParametersPresentFlag;          /**< u(1) */
    UINT16 Log2MaxMvLengthVertical;             /**< ue(v) */
    UINT16 Log2MaxMvLengthHorizontal;           /**< ue(v) */
    UINT16 SarWidth;                            /**< u(16) */
    UINT16 SarHeight;                           /**< u(16) */
    UINT32 NumReorderFrames;                    /**< ue(v) */
    UINT32 MaxDecFrameBuffering;                /**< ue(v) */
    UINT32 NumUnitsInTick;                      /**< u(32) */
    UINT32 TimeScale;                           /**< u(32) */
    SVC_H264_HRD_s NalHrdParam;                 /**< hrd_paramters_t */
    SVC_H264_HRD_s VclHrdParam;                 /**< hrd_paramters_t */
} SVC_H264_VUI_s;

/**
 *  H264 PPS
 */
typedef struct {
    INT32 Valid;                                /**< indicates the parameter set is Valid */
    UINT8 PicParameterSetId;                    /**< ue(v) */
    UINT8 SeqParameterSetId;                    /**< ue(v) */
    UINT8 EntropyCodingModeFlag;                /**< u(1) */
    UINT8 Transform8x8ModeFlag;                 /**< u(1) */
    UINT8 PicScalingMatrixPresentFlag;          /**< u(1) */
    UINT8 PicScalingListPresentFlag[12];        /**< u(1) */
    UINT8 SliceGroupChangeDirectionFlag;        /**< u(1) */
    UINT8 WeightedPredFlag;                     /**< u(1) */
    UINT8 WeightedBipredIdc;                    /**< u(2) */
    UINT8 DeblockingFilterControlPresentFlag;   /**< u(1) */
    UINT8 ConstrainedIntraPredFlag;             /**< u(1) */
    UINT8 RedundantPicCntPresentFlag;           /**< u(1) */
    UINT8 PicOrderPresentFlag;                  /**< u(1) */
    UINT8 UseDefaultScalingMatrix4x4Flag[6];    /**< u(1) */
    UINT8 UseDefaultScalingMatrix8x8Flag[6];    /**< u(1) */
    UINT16 NumRefIdxL0ActiveMinus1;             /**< ue(v) */
    UINT16 NumRefIdxL1ActiveMinus1;             /**< ue(v) */
    UINT32 SliceGroupMapType;                   /**< ue(v) */
    UINT32 NumSliceGroupsMinus1;                /**< ue(v) */
    UINT32 RunLengthMinus1[8];                  /**< ue(v) */
    UINT32 TopLeft[8];                          /**< ue(v) */
    UINT32 BottomRight[8];                      /**< ue(v) */
    UINT32 SliceGroupChangeRateMinus1;          /**< ue(v) */
    UINT32 NumSliceGroupMapUnitsMinus1;         /**< ue(v) */
    UINT8 SliceGroupId[8];                      /**< complete MBAmap u(v) */
    INT32 PicInitQpMinus26;                     /**< se(v) */
    INT32 PicInitQsMinus26;                     /**< se(v) */
    INT32 ChromaQpIndexOffset;                  /**< se(v) */
    INT32 SecondCchromaQpIndexOffset;           /**< se(v) */
#ifndef H264_SIMPLE_PARSE
    INT32 ScalingList4x4[6][16];                /**< se(v) */
    INT32 ScalingList8x8[6][64];                /**< se(v) */
#endif
} SVC_H264_PPS_s;

/**
 *  H264 SPS
 */
typedef struct {
    INT32 Valid;                                /**< indicates the parameter set is Valid */
    UINT8 ProfileIdc;                           /**< u(8) */
    UINT8 ConstrainedSet0Flag;                  /**< u(1) */
    UINT8 ConstrainedSet1Flag;                  /**< u(1) */
    UINT8 ConstrainedSet2Flag;                  /**< u(1) */
    UINT8 ConstrainedSet3Flag;                  /**< u(1) */
    UINT8 LevelIdc;                             /**< u(8) */
    UINT8 SeqParameterSetId;                    /**< ue(v) */
    UINT8 SeqScalingMatrixPresentFlag;          /**< u(1) */
    UINT8 SeqScalingListPresentFlag[12];        /**< u(1) */
    UINT8 UseDefaultScalingMatrix4x4Flag[6];    /**< u(1) */
    UINT8 UseDefaultScalingMatrix8x8Flag[6];    /**< u(1) */
    UINT8 BitDepthLumaMinus8;                   /**< ue(v) */
    UINT8 BitDepthChromaMinus8;                 /**< ue(v) */
    UINT8 Log2MaxFrameNumMinus4;                /**< ue(v) */
    UINT8 PicOrderCntType;                      /**< u(1) */
    UINT8 Log2MaxPicOrderCntLsbMinus4;          /**< ue(v) */
    UINT8 DeltaPicOrderAlwaysZeroFlag;          /**< u(1) */
    UINT8 NumRefFramesInPicOrderCntCycle;       /**< ue(v) */
    UINT8 GapsInFrameNumValueAllowedFlag;       /**< u(1) */
    UINT8 FrameMbsOnlyFlag;                     /**< u(1) */
    UINT8 MbAdaptiveFrameFieldFlag;             /**< u(1) */
    UINT8 Direct8x8InferenceFlag;               /**< u(1) */
    UINT8 FrameCroppingFlag;                    /**< u(1) */
    UINT8 VuiParametersPresentFlag;             /**< u(1) */
    UINT8 SeparateColourPlaneFlag;              /**< u(1) */
    UINT8 LosslessQpprimeFlag;                  /**< u(1) */
#ifdef H264_SIMPLE_PARSE
    UINT8 OffsetForRefFrame[32];                /**< se(v) */
#else
    UINT8 OffsetForRefFrame[256];               /**< se(v) */
#endif
    UINT8 ChromaFormatIdc;                      /**< ue(v) */
    INT32 OffsetForNonRefPic;                   /**< se(v) */
    INT32 OffsetForTopToBottomField;            /**< se(v) */
    UINT32 NumRefFrames;                        /**< ue(v) */
    UINT32 PicWidthInMbsMinus1;                 /**< ue(v) */
    UINT32 PicHeightInMapUnitsMinus1;           /**< ue(v) */
    UINT16 FrameCroppingRectLeftOffset;         /**< ue(v) */
    UINT16 FrameCroppingRectRightOffset;        /**< ue(v) */
    UINT16 FrameCroppingRectTopOffset;          /**< ue(v) */
    UINT16 FrameCroppingRectBottomOffset;       /**< ue(v) */
#ifndef H264_SIMPLE_PARSE
    INT32 ScalingList4x4[6][16];                /**< se(v) */
    INT32 ScalingList8x8[6][64];                /**< se(v) */
#endif
    SVC_H264_VUI_s VuiParam;                    /**< vui_seq_parameters_t */
} SVC_H264_SPS_s;

/**
 *  H264 decode configuration
 */
typedef struct {
    UINT8 NALULength;       /**< NALU length */
    UINT16 SPSLen;          /**< The SPS size of H264 */
    UINT16 PPSLen;          /**< The PPS size of H264 */
    UINT8 SPS[SVC_FORMAT_MAX_SPS_LENGTH];   /**< The SPS of H264 */
    UINT8 PPS[SVC_FORMAT_MAX_PPS_LENGTH];   /**< The PPS of H264 */
    UINT16 SarWidth;        /**< SarWidth */
    UINT16 SarHeight;       /**< SarHeight */
    UINT32 NumUnitsInTick;  /**< NumUnitsInTick */
    UINT32 TimeScale;       /**< TimeScale */
} SVC_H264_DEC_CONFIG_s;

/**
 * Parse H.264 NAL/SPS/PPS/SLICE
 *
 * @param [in] Buffer Source buffer
 * @param [in] BufferSize Buffer length
 * @param [out] SPS Space to store SPS
 * @param [out] PPS Space to store PPS
 * @param [out] SliceInfo Space to store slice information
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH264_HeaderParse(UINT8 *Buffer, UINT32 BufferSize, SVC_H264_SPS_s *SPS, SVC_H264_PPS_s *PPS, SVC_H264_SliceInfo_s *SliceInfo);

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
UINT32 SvcH264_ReadStream(SVC_STREAM_HDLR_s *Stream, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 FrameSize, UINT32 *DataSize);

/**
 * Get Avc configuration
 *
 * @param [in] Size Size
 * @param [in] BufferOffset Buffer offset
 * @param [in] BufferBase Pointer to buffer base
 * @param [in] BufferSize Buffer size
 * @param [out] H264 The returned configuration
 * @return 0 - OK, others - FORMAT_ERR_XXXX
 */
UINT32 SvcH264_GetAvcConfig(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, SVC_H264_DEC_CONFIG_s *H264);

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
UINT32 SvcH264_GetFrameSize(UINT32 Size, UINT32 BufferOffset, const UINT8 *BufferBase, UINT32 BufferSize, UINT32 Align, UINT32 *FrameSize);

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
UINT32 SvcH264_WriteStream(SVC_STREAM_HDLR_s *Stream, UINT32 Size, UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, UINT32 *WriteSize, UINT32 Align);

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
UINT32 SvcH264_PutGOPHeader(UINT32 BufferOffset, UINT8 *BufferBase, UINT32 BufferSize, const SVC_VIDEO_TRACK_INFO_s *Video, UINT8 SkipFirstI, UINT8 SkipLastI, UINT64 PTS, UINT32 TimePerFrame, UINT32 *NewBufferOffset);

//UINT8 *AmbaH264_PutEOS(UINT8 *Addr, UINT8 *BufferBase, UINT8 *BufferLimit);

#endif

