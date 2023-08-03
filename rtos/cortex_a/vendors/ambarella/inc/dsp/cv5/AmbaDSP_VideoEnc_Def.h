/**
 *  @file AmbaDSP_VideoEnc_Def.h
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Definitions & Constants for Ambarella DSP Driver Video Encode Definition
 */
#ifndef AMBA_DSP_VIDEO_ENC_DEF_H
#define AMBA_DSP_VIDEO_ENC_DEF_H

#include "AmbaDSP.h"
#include "AmbaDSP_EventInfo.h"

#define AMBA_DSP_ENC_CODING_TYPE_H264   (1U)
#define AMBA_DSP_ENC_CODING_TYPE_MJPG   (2U)
#define AMBA_DSP_ENC_CODING_TYPE_H265   (3U)

#define AMBA_DSP_VIDEO_ENC_STOP_IMMEDIATELY     (0U)
#define AMBA_DSP_VIDEO_ENC_STOP_NEXT_IP         (1U)
#define AMBA_DSP_VIDEO_ENC_STOP_NEXT_I          (2U)
#define AMBA_DSP_VIDEO_ENC_STOP_NEXT_IDR        (3U)

#define AMBA_DSP_VIDEO_DROP_FRAME_ONLY          (0U)
#define AMBA_DSP_VIDEO_DROP_AND_REPEAT_FRAME    (1U)
#define AMBA_DSP_VIDEO_REPEAT_FRAME             (2U)

#define AMBA_DSP_VIDEO_ENC_GOP_SIMPLE    (0U)
#define AMBA_DSP_VIDEO_ENC_GOP_HIER_B    (1U)
#define AMBA_DSP_VIDEO_ENC_GOP_HIER_P    (2U)

#define AMBA_DSP_DEBLOCK_ENABLE_IDX                   (0U)
#define AMBA_DSP_DEBLOCK_EXT_PARAM_IDX                (1U)
#define AMBA_DSP_DEBLOCK_DISABLE_SLICE_BOUNDARY_IDX   (2U)

#define AMBA_DSP_BRC_DISABLE_AND_FIX_QP     (0U)
#define AMBA_DSP_BRC_CBR                    (1U)
#define AMBA_DSP_BRC_VBR                    (2U)
#define AMBA_DSP_BRC_SMART_VBR              (3U)
#define AMBA_DSP_BRC_CMPR_STRO              (4U)
#define AMBA_DSP_BRC_MV                     (5U)
#define AMBA_DSP_BRC_NUM                    (6U)

#define DSP_ENC_DESC_CAT_CAPTS              (0U) // CaptureTimeStamp format
#define DSP_ENC_DESC_CAT_NUM                (1U)

#define DSP_ENC_CAPTS_OPT_TIME              (0U) // TimeStamp occupy U64 length
#define DSP_ENC_CAPTS_OPT_SEQ_TIME          (1U) // CapSeq occupy B[63:32] TimeStamp occupy B[31:0]

#define AMBA_DSP_OSD_BLEND_AREA_EMBED_CODE  (0xFFU)

typedef struct {
    UINT16 QualityLevel;
    UINT16 TargetBitRate;
    UINT32 Tolerance;
    UINT32 RateCurvePoints;
    UINT32 MaxEncLoop;
    ULONG  RateCurvAddr;
} AMBA_DSP_JPEG_BIT_RATE_CTRL_s;

typedef struct {
    UINT8  IntraRefreshCycle;
    UINT8  IntraRefreshMode;
    UINT8  IntraRefreshFrame;
    INT8   IntraRefreshQpAdj;
    UINT8  IntraRefreshMaxDecodeDelay;
} AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s;

typedef struct {
    UINT8  BitRateCtrl;
    UINT32 QualityLevel;
    UINT32 BitRate;
    UINT32 CPBSize;
    UINT8  VBRComplexLvl;
    UINT8  VBRPercent;
    UINT16 VBRMinRatio;
    UINT16 VBRMaxRatio;
    UINT8  RcPMul;
} AMBA_DSP_VIDEO_BIT_RATE_CFG_s;

typedef struct {
    UINT8  VuiEnable;
    UINT8  AspectRatioInfoPresentFlag;
    UINT8  OverscanInfoPresentFlag;
    UINT8  OverscanAppropriateFlag;
    UINT8  VideoSignalTypePresentFlag;
    UINT8  VideoFullRangeFlag;
    UINT8  ColourDescriptionPresentFlag;
    UINT8  ChromaLocInfoPresentFlag;
    UINT8  TimingInfoPresentFlag;
    UINT8  FixedFrameRateFlag;
    UINT8  NalHrdParametersPresentFlag;
    UINT8  VclHrdParametersPresentFlag;
    UINT8  LowDelayHrdFlag;
    UINT8  PicStructPresentFlag;
    UINT8  BitstreamRestrictionFlag;
    UINT8  MotionVectorsOverPicBoundariesFlag;
    UINT8  AspectRatioIdc;
    UINT16 SarWidth;
    UINT16 SarHeight;
    UINT8  VideoFormat;
    UINT8  ColourPrimaries;
    UINT8  TransferCharacteristics;
    UINT8  MatrixCoefficients;
    UINT8  ChromaSampleLocTypeTopField;
    UINT8  ChromaSampleLocTypeBottomField;
    UINT32 VbrCbpRate;
    UINT8  Log2MaxMvLengthHorizontal;
    UINT8  Log2MaxMvLengthVertical;
    UINT16 NumReorderFrames;
    UINT16 MaxDecFrameBuffering;
    UINT16 MaxBytesPerPicDenom;
    UINT16 MaxBitsPerMbDenom;
} AMBA_DSP_VIDEO_ENC_VUI_s;

typedef struct {
    UINT8  CroppingFlag;
    UINT16 CropLeftOffset;
    UINT16 CropRightOffset;
    UINT16 CropTopOffset;
    UINT16 CropBottomOffset;
} AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s;

typedef struct {
    UINT8  EnableLoopFilter;
    UINT8  LoopFilterAlpha;
    UINT8  LoopFilterBeta;
} AMBA_DSP_VIDEO_DEBLOCK_CTRL_s;

typedef struct {
    UINT8  GOPStruct;
    UINT8  M;
    UINT16 N;           // 0xFFFF means infinite GOP
    UINT32 IdrInterval;
} AMBA_DSP_VIDEO_GOP_CONFIG_s;

typedef struct {
    UINT8  ProfileIDC;
    UINT8  LevelIDC;
    UINT8  IsCabac;
    AMBA_DSP_VIDEO_GOP_CONFIG_s GOPCfg;
    UINT8  NumPRef;
    UINT8  NumBRef;
    UINT8  FirstGOPStartB;
    AMBA_DSP_VIDEO_DEBLOCK_CTRL_s DeblockFilter;
    AMBA_DSP_VIDEO_FRAME_CROP_CTRL_s FrameCrop;
    AMBA_DSP_VIDEO_ENC_VUI_s Vui;
    AMBA_DSP_VIDEO_BIT_RATE_CFG_s BitRateCfg;
    ULONG  BitsBufAddr;
    UINT32 BitsBufSize;
    UINT8  NumSlice;
    UINT8  NumTile;
    UINT8  NumExtraRecon;
    AMBA_DSP_VIDEO_INTRA_REFRESH_CTRL_s IntraRefreshCtrl;
    UINT8  PicOrderCntType;
    UINT8  SPSPPSHeaderInsertMode;
    UINT8  AUDMode;
    UINT8  NonRefPIntvl;
    ULONG  QuantMatrixAddr;
    AMBA_DSP_JPEG_BIT_RATE_CTRL_s JpgBitRateCtrl;
    UINT8  InsertSeiUserData; // 0: No UserData buffer 1: 128B UserData buffer
    UINT8  EncOption; // B[0]: IsMonoEnc B[1]:LowDlyQ
} AMBA_DSP_VIDEO_ENC_CONFIG_s;

typedef struct {
    UINT8  CodingFmt;
    UINT16 SrcYuvStreamId;  //0xFFFF means this EncStrm comes from ExtMem
    AMBA_DSP_WINDOW_s Window;
    AMBA_DSP_FRAME_RATE_s FrameRate;
    UINT8  Rotate;
    UINT8  IntervalCapture;
    AMBA_DSP_VIDEO_ENC_CONFIG_s EncConfig;
    UINT32 Affinity;
} AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s;

typedef struct {
    UINT32 EncDuration;
    UINT32 EncSkipFrame;    //skip first several frames after encode start
} AMBA_DSP_VIDEO_ENC_START_CONFIG_s;

typedef struct {
    UINT8  Count;
    UINT8  DropRepeatOption;
} AMBA_DSP_VIDEO_REPEAT_DROP_CONFIG_s;

typedef struct {
    UINT8  BlendAreaIdx;
    UINT8  Enable;
    UINT8  EnableCLUT;
    AMBA_DSP_YUV_IMG_BUF_s BlendYuvBuf;
    AMBA_DSP_YUV_IMG_BUF_s AlphaBuf;
} AMBA_DSP_VIDEO_BLEND_CONFIG_s;

typedef struct {
    UINT8  QpMinIFrame;
    UINT8  QpMaxIFrame;
    UINT8  QpMinPFrame;
    UINT8  QpMaxPFrame;
    UINT8  QpMinBFrame;
    UINT8  QpMaxBFrame;
    UINT8  QpReduceIFrame;
    UINT8  QpReducePFrame;
    UINT8  Aqp;
} AMBA_DSP_VIDEO_QP_CONFIG_s;

typedef struct {
    UINT32 Category:8;      /* Macroblock level control for user_class */
    UINT32 QpAdjust:8;      /* int8, Macroblock level control for qp_offset. The qp_offset is added onto
                               the existing QP already calculated by RC and User class bin. The final
                               QP from RC + qp_offset is then used for this MB */
    UINT32 Rsvd:2;
    UINT32 ZmvThreshold:8;  /* Macroblock level control of threshold for zero MV. If MB's sum of Mvs
                               is less than this value, skip mode is encouraged. Value 8 can be used
                               for low bit rate at good lighting. Value 24 can be used for low bit
                               rate at low lighting (high noise) */
    UINT32 Rsvd1:6;
} AMBA_DSP_VIDEO_ROI_H264_MAP_s;

typedef struct {
    UINT32 MdCatCU16_0:3;       /* Select md_category on 16x16 CU[0]. 0: default; 1: used as force zmv flag. */
    UINT32 MdCatCU16_1:3;       /* Select md_category on 16x16 CU[1]. 0: default; 1: used as force zmv flag. */
    UINT32 MdCatCU16_2:3;       /* Select md_category on 16x16 CU[2]. 0: default; 1: used as force zmv flag. */
    UINT32 MdCatCU16_3:3;       /* Select md_category on 16x16 CU[3]. 0: default; 1: used as force zmv flag. */
    UINT32 MdCatCU32:3;         /* Select md_category on 32x32 CU. 0: default; 1: used as force zmv flag. */
    UINT32 Rsvd0:13;
    UINT32 MaxiCatCU32:4;       /* max category value for 32x32 CU. Valid range is 15. */

    UINT32 Rsvd1:8;
    UINT32 QpAdjustCU32:7;      /* qp_adjust value for 32x32 CU. Valid range is -51 ~ 51. */
    UINT32 Rsvd2:1;
    UINT32 MaxiCatCU16_0:4;     /* max category value for 16x16 CU[0]. Valid range is 15. */
    UINT32 MaxiCatCU16_1:4;     /* max category value for 16x16 CU[1]. Valid range is 15. */
    UINT32 MaxiCatCU16_2:4;     /* max category value for 16x16 CU[2]. Valid range is 15. */
    UINT32 MaxiCatCU16_3:4;     /* max category value for 16x16 CU[3]. Valid range is 15. */

    UINT32 QpAdjustCU16_0:7;    /* qp_adjust value for 16x16 CU[0]. Valid range is -51 ~ 51. */
    UINT32 Rsvd3:1;
    UINT32 QpAdjustCU16_1:7;    /* qp_adjust value for 16x16 CU[1]. Valid range is -51 ~ 51. */
    UINT32 Rsvd4:1;
    UINT32 QpAdjustCU16_2:7;    /* qp_adjust value for 16x16 CU[2]. Valid range is -51 ~ 51. */
    UINT32 Rsvd5:1;
    UINT32 QpAdjustCU16_3:7;    /* qp_adjust value for 16x16 CU[3]. Valid range is -51 ~ 51. */
    UINT32 Rsvd6:1;

    UINT32 Rsvd7;               /* 16Byte align */
} AMBA_DSP_VIDEO_ROI_H265_MAP_s;

typedef struct {
    ULONG  IFrmMapAddr;         /* when h264, each entry is AMBA_DSP_VIDEO_ROI_H264_MAP_s.
                                   when h265, each entry is AMBA_DSP_VIDEO_ROI_H265_MAP_s. */
    ULONG  PFrmMapAddr;         /* when h264, each entry is AMBA_DSP_VIDEO_ROI_H264_MAP_s.
                                   when h265, each entry is AMBA_DSP_VIDEO_ROI_H265_MAP_s. */
    ULONG  BFrmMapAddr;         /* when h264, each entry is AMBA_DSP_VIDEO_ROI_H264_MAP_s.
                                   when h265, each entry is AMBA_DSP_VIDEO_ROI_H265_MAP_s. */
    ULONG  AuxFrmMapAddr;       /* obsoleted */
    UINT8  MdCatLUT[8U];        /* H265 only */
    UINT16 IntraBias[2U];       /* H264 only, P/B frame Catergory1 and 2 */
    UINT16 DirectBias[2U];      /* H264 only, P/B frame Catergory1 and 2 */
} AMBA_DSP_VIDEO_ROI_CONFIG_s;

typedef struct {
    UINT8  DemandIDR;
    UINT8  BitRateCtrl;
    UINT8  GOPCtrl;
    UINT8  QPCtrl;
    UINT32 BitRate;
    AMBA_DSP_VIDEO_GOP_CONFIG_s GOPCfg;
    AMBA_DSP_VIDEO_QP_CONFIG_s QpCfg;
    UINT8  RoiCtrl;
    AMBA_DSP_VIDEO_ROI_CONFIG_s RoiCfg;
} AMBA_DSP_VIDEO_ENC_QUALITY_s;

#define DSP_ENC_GRP_PURPOSE_RC_IDX  (0U)
typedef struct {
    UINT16 GrpIdx;
    UINT16 Purpose;

    UINT16 NumStrm;
    UINT8  Stride; //B[7]:MonotoneIncrease, B[6:0]:stride
    UINT8  Rsvd;

    ULONG  RcCfgAddr; //use when PURPOSE_RC

    UINT16 *pStrmIdx;
} AMBA_DSP_VIDEO_ENC_GROUP_s;

#define AMBA_DSP_ENC_MV_FMT_AVC     (1U)
#define AMBA_DSP_ENC_MV_FMT_NUM     (2U)
typedef struct {
    ULONG  BufAddr;     //MV buf pool address
    UINT32 BufSize;     //MV buf pool size
    UINT32 UnitSize;    //MV unit size, from AmbaDSP_CalcEncMvBufInfo
    UINT32 Option;      //B[7:0] FmtType, see AMBA_DSP_ENC_MV_FMT_NUM
} AMBA_DSP_VIDEO_ENC_MV_CFG_s;

#endif  /* AMBA_DSP_VIDEO_ENC_DEF_H */
