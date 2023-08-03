/**
 *  @file AmbaDSP_Liveview_Def.h
 *
 *  @copyright Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for Ambarella DSP Driver Liveview Definition
 *
 */
#ifndef AMBA_DSP_LIVEVIEW_DEF_H
#define AMBA_DSP_LIVEVIEW_DEF_H

#include "AmbaDSP.h"
#include "AmbaDSP_EventInfo.h"

typedef struct {
    UINT16 AllocType;
    UINT16 BufNum;
    AMBA_DSP_YUV_IMG_BUF_s YuvBuf;
    ULONG  *pYuvBufTbl;
} AMBA_DSP_LIVEVIEW_YUV_BUF_s;

typedef struct {
    AMBA_DSP_WINDOW_s CropWindow;       /* lane detect active ViewZone */
    UINT16 HierSource;                  /* lane detect source from which Pyramid output */
    UINT16 Width;                       /* final lane detect output width */
    UINT16 Height;                      /* final lane detect output height */
    UINT16 Reserved;
} AMBA_DSP_LIVEVIEW_LANE_DETECT_s;

#define DSP_LV_SOURCE_VIN           (0U)
#define DSP_LV_SOURCE_MEM           (1U)
#define DSP_LV_SOURCE_MEM_YUV422    (2U)
#define DSP_LV_SOURCE_MEM_YUV420    (3U)
#define DSP_LV_SOURCE_MEM_DEC       (4U)
#define DSP_LV_SOURCE_MEM_RECON     (5U)    /* VinId indicate EncStrmIdx */
#define DSP_LV_SOURCE_NUM           (6U)

#define DSP_DRAM_PIPE_NORMAL        (0U)    /* Normal pipe */
#define DSP_DRAM_PIPE_EFCY          (1U)    /* Dram efcy with limited vertical warp */
#define DSP_DRAM_PIPE_MIPI_YUV      (2U)    /* Mipi yuv with limited YuvStrm number */
#define DSP_DRAM_PIPE_RAW_ONLY      (3U)    /* Only run Raw capture */
#define DSP_DRAM_PIPE_NUM           (4U)
typedef struct {
    UINT16 ViewZoneId;
    UINT16 VinSource;
    UINT16 VinId;                       /* Vin index, Bit[15] means VirtualVin */
    UINT16 VinVirtChId;                 /* B[7:0] VirtChanIdx, B[15:8] TimeDivisionIdx */
    UINT8  IsVirtChan;                  /* B[0] IsVirtChan
                                           B[1] IsTimeDivision
                                           B[2] IsVinDecimation
                                           B[3] ProcRawDramOut
                                           B[4] InputMuxSel */
    AMBA_DSP_WINDOW_s VinRoi;
    UINT16 MainWidth;
    UINT16 MainHeight;
    UINT8  RotateFlip;
    UINT8  HdrExposureNum;
    UINT16 HdrBlendHieght;
    UINT8  MctfDisable;                 /* B[0] Mctf Off
                                           B[7:6] Mctf compression needed */
    UINT8  MctsDisable;                 /* B[0] Mcts Off
                                           B[7] Mcts dram out disable */
    UINT8  LinearCE;
    AMBA_DSP_LIVEVIEW_PYRAMID_s Pyramid;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s *pPyramidBuf;
    AMBA_DSP_LIVEVIEW_LANE_DETECT_s LaneDetect;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s *pLaneDetectBuf;
    UINT8  VinDecimation;               /* DecimationRate, only valid when IsVirtChan:B[2] = 1U */
    UINT8  DramUtilityPipe;             /* DSP_DRAM_PIPE_NUM */
    UINT8  Rsvd[2U];
    AMBA_DSP_LIVEVIEW_YUV_BUF_s *pMainY12Buf; /* Only Valid in CV2FS, LumaOnly */
    UINT32 InputMuxSelCfg;              /* Valid when IsVirtChan:B[4] = 1U
                                         * B[31:30] InputSourceSelect : [0]From Vin [1]From VinAux0 [2]From VinAux1
                                         * B[19:10] H Decimation      : 0 means 1x
                                         * B[9:0]   V Decimation      : 0 means 1x
                                         */
} AMBA_DSP_LIVEVIEW_VIEWZONE_CFG_s;

#define AMBA_DSP_MAX_YUVSTRM_BLD_NUM    (2U)
typedef struct {
    UINT16 ViewZoneId;
    AMBA_DSP_WINDOW_s ROI;      // ROI indicates area in ViewZoneCfg.MainW/H
    AMBA_DSP_WINDOW_s Window;
    UINT8  RotateFlip;
    UINT8  BlendNum;            // upto 2 blend number
    ULONG LumaAlphaTable;      // point to AMBA_DSP_BUF_s, cfg order means process order
} AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s;

#define DSP_LV_STRM_PURPOSE_MEMORY_IDX  (0U)
#define DSP_LV_STRM_PURPOSE_ENCODE_IDX  (1U)
#define DSP_LV_STRM_PURPOSE_VOUT_IDX    (2U)
#define DSP_LV_STRM_PURPOSE_RAWCAP_IDX  (3U)

typedef struct {
    UINT16 StreamId;
    UINT16 Purpose;
    UINT16 DestVout;
    UINT32 DestEnc;
    UINT16 Width;
    UINT16 Height;
    UINT16 MaxWidth;
    UINT16 MaxHeight;
    AMBA_DSP_LIVEVIEW_YUV_BUF_s StreamBuf;
    UINT16 NumChan;
    AMBA_DSP_LIVEVIEW_CHANNEL_WINDOW_s *pChanCfg;   /* channel order means process order */
    UINT32 MaxChanBitMask;                          /* All possible channel for YuvStream, BitMask for ViewZone index */
    UINT16 MaxChanNum;                              /* Max channel number for YuvStream */
    AMBA_DSP_WINDOW_DIMENSION_s *pMaxChanWin;       /* Max possible channel size, 16 ViewZone and fill in fixed index */
    UINT32 DestDeciRate;                            /* yuv decimation rate */
} AMBA_DSP_LIVEVIEW_STREAM_CFG_s;

typedef struct {
    UINT16 ViewZoneId;
    UINT32 CtxIndex;
    UINT32 CfgIndex;
    ULONG  CfgAddress;
    UINT32 HdrCfgIndex;
} AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s;

typedef struct {
    UINT8  EnaVinCompand;
    ULONG  VinCompandTableAddr;
    ULONG  VinDeCompandTableAddr;
} AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s;

typedef struct {
    UINT16              IsVirtual;      /* is virtual channel, virtaul and Physical channel is exclusive */
    UINT16              Index;          /* Sub Channel Index, same as VinId when phyical channel */
} AMBA_DSP_VIN_SUB_CHAN_s;

typedef struct {
    AMBA_DSP_RAW_BUF_s      RawBuffer;      /* raw buffer information */
    AMBA_DSP_RAW_BUF_s      CeBuffer;       /* CE buffer information */
    UINT16                  VinId;          /* Vin index, Bit[15] means VirtualVin */
    AMBA_DSP_VIN_SUB_CHAN_s SubChan;        /* Sub channel index */
    UINT8                   IsLast;         /* Is last frame */
    UINT64                  CapPts;         /* capture pts tick */
    UINT64                  CapSequence;    /* capture sequence number */
} AMBA_DSP_LIVEVIEW_EXT_RAW_s;

/* Vin capture option */
#define AMBA_DSP_VIN_CAP_OPT_PROG       (0x0U)
#define AMBA_DSP_VIN_CAP_OPT_INTC       (0x1U)
#define AMBA_DSP_VIN_CAP_OPT_EMBD       (0x2U) /* capture embedded data, it depends sensor side support it or not
                                                * HW request one more line when capture EMBD data,
                                                * If sensor output #EmbdLine, set CaptureWindow.Height = #EmbdLine + 1
                                                */
#define AMBA_DSP_VIN_CAP_OPT_NUM        (0x3U)
typedef struct {
    AMBA_DSP_VIN_SUB_CHAN_s SubChan;        /* Sub channel index */
    UINT16                  Option;         /* Sub Channel Capture option */
    UINT16                  ConCatNum;      /* Concate number when progress mode */
    UINT16                  IntcNum;        /* interleave number when Interlace mode */
    AMBA_DSP_WINDOW_s       CaptureWindow;  /* Overall Capture window */
    UINT16                  TDNum;          /* Number of Time division, upto AMBA_DSP_MAX_VIN_TD_NUM */
    UINT16                  *pTDFrmNum;     /* frame number per Time division, upto AMBA_DSP_MAX_VIN_TD_FRM_NUM */
} AMBA_DSP_VIN_SUB_CHAN_CFG_s;

/* Vin post config, bit-wise */
#define AMBA_DSP_VIN_POST_NONE      (0x0U)
#define AMBA_DSP_VIN_CONFIG_POST    (0x1U)
#define AMBA_DSP_VIN_START_POST     (0x2U)

/* Yuv stream sync operation */
#define AMBA_DSP_YUVSTRM_SYNC_START     (0x0U)
#define AMBA_DSP_YUVSTRM_SYNC_EXECUTE   (0x1U)

typedef struct {
    UINT32 TargetViewZoneMask;
    UINT16 Opt;
    UINT16 Reserved;
    UINT32 Reserved1[2U];
} AMBA_DSP_LIVEVIEW_SYNC_CTRL_s;

typedef struct {
    UINT32 GeoCfgId;
    ULONG  GeoCfgAddr;
    UINT32 Reserved[2U];
} AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s;

typedef struct {
    UINT16 ViewZoneId;
    UINT16 VinSource;
    UINT16 VinId;
    UINT16 VinVirtChId;                 /* B[7:0] VirtChanIdx, B[15:8] TimeDivisionIdx */
    UINT8  IsVirtChan;                  /* B[0] IsVirtChan
                                           B[1] IsTimeDivision
                                           B[2] IsVinDecimation */
    UINT8  VinDecimation;               /* DecimationRate, only valid when IsVirtChan:B[2] = 1U, NOT SUPPORT YET */
    UINT8  Rsvd[2U];
    AMBA_DSP_WINDOW_s VinRoi;
} AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s;

typedef struct {
    UINT8  SliceNumCol;         /* default 0, 0 means decide by DSP driver */
    UINT8  SliceNumRow;         /* >1 means enable low delay */
    UINT8  EncSyncOpt;          /* 1 : enable it, MJPG only */
    UINT8  Rsvd[1U];
    UINT16 WarpLumaWaitLine;    /* 0 means default 48 lines */
    UINT16 WarpChromaWaitLine;  /* 0 means default 48 lines */
    UINT16 WarpOverLap;         /* 0 means default 128 lines */
    UINT16 VinDragLine;         /* 0 means default 128 drag lines, all ViewZone relate to same Vin shall have same value */
} AMBA_DSP_LIVEVIEW_SLICE_CFG_s;

#define AMBA_DSP_VZ_POSTPONE_STAGE_PRE_R2Y  (0U)
#define AMBA_DSP_VZ_POSTPONE_STAGE_NUM      (1U)
typedef struct {
    UINT16 StageId;
    UINT16 Rsvd;
    UINT32 DelayTimeIn10xMs;            /* 100usec unit */
    UINT32 Rsvd1[2U];
} AMBA_DSP_LV_VZ_POSTPONE_CFG_s;

#define AMBA_DSP_VIN_STATE_DISCNNT      (1U) /* Vin signal lost */
#define AMBA_DSP_VIN_STATE_NUM          (2U)
typedef struct {
    UINT16 VinId;
    UINT16 State;
    AMBA_DSP_VIN_SUB_CHAN_s SubChan;        /* Sub channel index */
    UINT16 Operation;
    UINT16 Rsvd;
    UINT32 Rsvd1[5U];
} AMBA_DSP_LV_VIN_STATE_s;

#endif  /* AMBA_DSP_LIVEVIEW_DEF_H */
