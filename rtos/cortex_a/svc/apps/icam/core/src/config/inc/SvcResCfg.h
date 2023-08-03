/**
*  @file SvcResCfg.h
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
*  @details svc resolution config
*
*/

#ifndef SVC_RES_CFG_H
#define SVC_RES_CFG_H

#include "AmbaDSP_Liveview_Def.h"
#include "SvcClock.h"

#define SVC_RES_CV_FLOW_NUM         (16U)
#define SVC_RES_CV_MAX_INPUT_NUM    (4U)
#define SVC_RES_CV_MAX_OUTPUT_PORT  (2U)
#define SVC_RES_CV_MAX_CFG_PARAM    (4U)
#define SVC_RES_CV_MAX_ROI          (3U)
#define SVC_RES_CV_DOWNSTRM_CHAN_NUM (1U)

#define SVC_RES_SERDES_TYPE_NONE    (0U)
#define SVC_RES_SERDES_TYPE_B6      (1U)
#define SVC_RES_SERDES_TYPE_MAXIM   (2U)
#define SVC_RES_SERDES_TYPE_TI      (2U)

#define SVC_RES_DISP_ALT_NUM        (8U)

#define SVC_RES_VIEW_CTRL_OP_DZOOM  (0U)
#define SVC_RES_VIEW_CTRL_OP_PREV   (1U)

#define SVC_MAX_NUM_SENSOR_PER_VIN  (16U)
#define SVC_CAP_MAX_SENSOR_PER_VIN  (4U)

#define SVC_MAX_NUM_MEM_INPUT       (4U)

#define SVC_RES_BLD_TBL_NON         (0U)
#define SVC_RES_BLD_TBL_FROM_CALIB  (1U)
#define SVC_RES_BLD_TBL_FROM_ROM    (2U)

#define SVC_RES_VIN_REAL            (0U)
#define SVC_RES_VIN_VIRTUAL_1       (1U)
#define SVC_RES_VIN_VIRTUAL_2       (2U)
#define SVC_RES_VIN_VIRTUAL_3       (3U)

#define DEFAULT_MAX_VIN_TIMEOUT  (1000U) // MS

typedef struct {
    UINT32  RotateFlip;
    UINT32  HdrExposureNum;
    UINT32  HdrBlendHieght;
    #define MCTF_BITS_OFF           (0x01U)
    #define MCTF_BITS_ON_CMPR       (0x40U)
    UINT32  MctfDisable;
    #define MCTS_BITS_OFF           (0x01U)
    #define MCTS_BITS_OFF_DRAMOUT   (0x80U)
    #define MCTS_BITS_FORCE_RES    (0x100U)
    UINT32  MctsDisable;
    #define LINEAR_CE_ENABLE        (0x01U)
    #define LINEAR_CE_FORCE_DISABLE (0x80U)     /* For backward compatible, forcing to disable CE no matter the sensor mode, priority higher then LINEAR_CE_ENABLE */
    UINT32  LinearCE;
    UINT32  RawCompression;
    UINT32  ViewCtrlOperation;
    /* low delay parameters */
    #define SVC_LDY_PIDX_SLICE_NUM          (0) /* 0 or 1: disable, 1 < enable */
    #define SVC_LDY_PIDX_LINE_WARP_OVERLAP  (1) /* lines of warp overlap, 0 means default 128 lines */
    #define SVC_LDY_PIDX_LINE_VIN_DRAG      (2) /* lines of vin drag, 0 means default 128 lines */
    #define SVC_LDY_PIDX_NUM                (3)
    UINT32  LowDelayParams[SVC_LDY_PIDX_NUM];
    #define SVC_VDOPIPE_NORMAL              (0U)
    #define SVC_VDOPIPE_DRAMEFCY            (1U)
    #define SVC_VDOPIPE_MIPIYUV             (2U)
    #define SVC_VDOPIPE_RAW_ONLY            (3U)    /* Dram Utility - Only run Raw capture */
    #define SVC_VDOPIPE_MIPIRAW             (4U)    /* Dram Utility - Mipi Raw 16bit [P0L8/P0H8/P1L8/P1H8/...] */
    UINT32  PipeMode;                               /* Dram Utility setting */
    #define SVC_RES_SENSOR_RGB              (0U)
    #define SVC_RES_SENSOR_RGB_IR           (1U)
    #define SVC_RES_SENSOR_RCCC             (2U)
    UINT32  ForceSensorType;
    UINT32  MaxHorWarpCompensation;
    UINT32  MainY12Out;                         /* enable 12 bits Y main output. CV2FS only. */
    UINT32  WarpLumaWaitLine;
    UINT32  WarpChromaWaitLine;
    UINT32  MaxWarpDma;
    #define SVC_RES_FOV_FROM_VIRT_VIN       (0x8000)
    UINT32  FromVirtVin;                        /* The case that the FOV comes from other FOV, to be carried in VinId when AmbaDSP_LiveviewConfig() */
    UINT32  VirtVinSrc;                         /** Only valid when FromVirtVin is 0x8000,
                                                 * if FovIdx is needed, then bit[0] means FovIdx = 0, [1] means FovIdx = 1.
                                                 * Or Stream ID is specified.
                                                 */
} SVC_PIPE_CFG_s;

typedef struct {
    char   RomFileName[32U];
    AMBA_DSP_WINDOW_s Win;
} SVC_BLD_TBL_s;

typedef struct {
    UINT32 Boot;
    UINT32 Liveview;
} SVC_VIN_TIMEOUT_s;

typedef struct {
    void                        *pDriver;     /* Driver could be YUV driver or Sensor driver */
    void                        *pEEPROMDriver;
    UINT32                      SerdesType;   /* SerdesType is used for Image Framework */
    AMBA_DSP_WINDOW_s           CapWin;
    UINT32                      SubChanNum;
    AMBA_DSP_VIN_SUB_CHAN_CFG_s SubChanCfg[AMBA_DSP_MAX_VIRT_CHAN_NUM];
    UINT16                      SubChanTDFrmNum[AMBA_DSP_MAX_VIRT_CHAN_NUM][AMBA_DSP_MAX_VIN_TD_NUM];
    AMBA_DSP_VIN_SUB_CHAN_CFG_s EmbChanCfg;
    SVC_VIN_TIMEOUT_s           TimeoutCfg;
    AMBA_DSP_FRAME_RATE_s       FrameRate;
    UINT8                       FixedFrameRate; /* 0 - adjustable by SW PLL; 1 - non-adjustable */
    UINT8                       SkipSwpll; /* 0 - normal; 1 - skip SW PLL */
    UINT8                       DisableMasterSync;  /* If 1, the sensor will NOT enable Master-Sync (If driver support) */
} SVC_VIN_CFG_s;

typedef struct {
    UINT32            SensorMode;
    UINT32            SensorGroup;  /* The sensor in the same group will be global 3A
                                       bit[0:3] aaa group id
                                       bit[4:11] aaa smp core bits
                                       bit[12:13] slow shutter index for evaluation only
                                       bit[14] af enable/disable bit
                                       bit[15] aaa statistic fov msb bit
                                       bit[16:19] avm group id
                                       bit[20:27] avm smp core bits
                                       bit[28:29] sub sensor mode.
                                       bit[30] force slow shutter for smc evaluation only
                                       bit[31] avm statistic fov msb bit */
    UINT32            IQTable;      /* IQTable need to reference AmbaIQParamHandlerSample.h
                                       bit[0:7] aaa iq table
                                       bit[8:15] avm iq table
                                       bit[16:22] aaa hdr mode
                                       bit[23] force to disable vin tone curve update
                                       bit[24:30] avm hdr mode
                                       bit[31] reserved */
    UINT32            TimeDivisionIQTable[AMBA_DSP_MAX_VIN_TD_NUM];
    UINT64            SensorEFov;   /* external fov for evaluation only */
    UINT32            SmcI2sId;     /* smc i2s id for evaluation only
                                       bit[0:3]: smc off i2s clk div
                                       bit[4:7]: smc off i2s clk scale
                                       bit[8:11]: smc on i2s clk div
                                       bit[12:15]: smc on i2s clk scale
                                       bit[16:19]: smc i2s channel
                                       bit[20:31]: reserved */
    UINT32            SsAuxId;      /* bit[0:4]: ss frame length line scale for status usage of sensor driver,
                                                 if multi-sensor are connected to signle-vin and configurated together
                                       bit[5:31]: reserved */
} SVC_SENSOR_CFG_s;

typedef struct {
    UINT16      Top;
    UINT16      Right;
    UINT16      Bottom;
    UINT16      Left;
} SVC_MARGIN_CFG_s;

typedef struct {
    UINT32 Enable;
    UINT32 Id;
    UINT32 InputMuxSelCfg;  /* Valid when SVC_RES_PIPE_VIRTUAL_CHAN_INPUT_MUX_SEL
                             * B[31:30] InputSourceSelect : [0]From Vin [1]From VinAux0 [2]From VinAux1
                             * B[19:10] H Decimation      : 0 means 1x
                             * B[9:0]   V Decimation      : 0 means 1x
                             */
    UINT32 InputSubsample;  /* Valid when SVC_RES_PIPE_VIRTUAL_CHAN_INPUT_MUX_SEL
                             * B[31:16] H SubSample
                             * B[15:0]  V SubSample
                             */
} SVC_PIPE_VIRTUAL_CHAN_s;

typedef struct {
    UINT16 SourceScale;                    /* source of the scaler. Should be one of Pyramid output */
    AMBA_DSP_WINDOW_s SrcWin;
    AMBA_DSP_WINDOW_DIMENSION_s DstWin;
} SVC_PYRAMID_EX_SCALE_CFG_s;

typedef struct {
    AMBA_DSP_WINDOW_s            RawWin;
    AMBA_DSP_WINDOW_s            ActWin;
    AMBA_DSP_WINDOW_DIMENSION_s  MainWin;
    UINT32                       PyramidBits;
    UINT32                       PyramidFlag;
    #define SVC_RES_PYRAMID_ENABLE_CROP         (0x1U)
    #define SVC_RES_PYRAMID_12BITS_Y            (0x2U)
    #define SVC_RES_PYRAMID_DOWNSCALE_2X        (0x4U)  /* Downscale ratio of layers */
    #define SVC_RES_PYRAMID_ENABLE_EX_SCALE     (0x8U)  /* 2nd scaler for one of pyramid layer */
    UINT8                        PyramidDeciRate;       /* Frame decimation rate */
    AMBA_DSP_WINDOW_DIMENSION_s  HierWin;               /* Invalid */
    AMBA_DSP_WINDOW_s            HierCropWin[AMBA_DSP_MAX_HIER_NUM];
    SVC_PYRAMID_EX_SCALE_CFG_s   PyramidExScale;        /* 2nd scaler for one of pyramid layer */
    SVC_PIPE_CFG_s               PipeCfg;
    SVC_PIPE_VIRTUAL_CHAN_s      VirtChan;
    /* VirtChan definition is in AmbaDSP_Liveview.h */
    #define SVC_RES_PIPE_VIRTUAL_CHAN                  (0x1U)
    #define SVC_RES_PIPE_VIRTUAL_CHAN_TIME_DIV         (0x2U)
    #define SVC_RES_PIPE_VIRTUAL_CHAN_VIN_DECI         (0x4U)
    #define SVC_RES_PIPE_VIRTUAL_CHAN_PROC_RAW_DRAMOUT (0x8U)
    #define SVC_RES_PIPE_VIRTUAL_CHAN_INPUT_MUX_SEL    (0x10U)
    #define SVC_RES_PIPE_VIRTUAL_CHAN_FROM_UPSTREAM    (0x20U)
    UINT32                       FovGroup; /* The fov in the same group will be global adj
                                              bit[0:3] fov adj group
                                              bit[4:15] rserved
                                              bit[16:19] fov eis group for evaluation only
                                              bit[20:27] eis smp core bits
                                              bit[28:31] reserved */
    UINT32                       IQTable;  /* IQTable need to reference AmbaIQParamHandlerSample.h
                                              bit[0:7] fov adj iq table (0:disable, other:enable)
                                              bit[8] fov eis enable/disable (0:disable, 1:enable) for evaluation only
                                              bit[9:15] eis r2y user delay
                                              bit[16:22] fov adj hdr mode
                                              bit[23:29] eis gryo user delay
                                              bit[30:31] reserved */
    UINT32                       CalUpdBits;
    UINT32                       DecFeedBits;  /* The fovs (bits) to be fed from this decoder */
} SVC_FOV_CFG_s;

typedef struct {
    UINT32             FovId;
    AMBA_DSP_WINDOW_s  SrcWin;
    AMBA_DSP_WINDOW_s  DstWin;
    SVC_MARGIN_CFG_s   Margin;
    UINT8              RotateFlip;
    UINT8              BlendEnable;
    SVC_BLD_TBL_s      BlendTable;  /* Take effect when BlendEnable == 2 */
} SVC_CHAN_CFG_s;

typedef struct {
    AMBA_DSP_WINDOW_DIMENSION_s  Win;
    AMBA_DSP_WINDOW_DIMENSION_s  MaxWin;
    UINT32                       NumChan;
    SVC_CHAN_CFG_s               ChanCfg[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM];
} SVC_STRM_CFG_s;

typedef struct {
    #define                 SVC_GUI_ATTR_TIMER             (0x01U)
    #define                 SVC_GUI_ATTR_VOUT              (0x02U)
    #define                 SVC_GUI_ATTR_CUSTOM_RATE       (0x04U)
    #define                 SVC_GUI_ATTR_DISABLE           (0x10U)
    UINT32                  Attribute;           /* If attribute is not set. GUI will update following VOUT interrupt.
                                                    [0]: Update following timer. If UpdateFrameRate is not specified, 60FPS by default
                                                    [1]: Update following VOUT interrupt.
                                                    [2]: Update following VOUT interrupt. But specified the UpdateFrameRate
                                                    [3]: Reserved.
                                                    [4]: Do not enable GUI. */
    AMBA_DSP_FRAME_RATE_s   UpdateFrameRate;     /* Take effect only when */
} SVC_GUI_CFG_s;

typedef struct {
    UINT32 SyncVinID;   // for cv2xfs/cv5x. others chip only support sync with vin_0 now
    UINT32 DelayTime;   // 1 unit = 100 us
} SVC_VIN_VOUT_SYNC_CFG_s;

typedef struct {
    UINT32          VoutID;
    void            *pDriver;
    SVC_STRM_CFG_s  StrmCfg;
    UINT8           VideoRotateFlip;
    UINT8           DevMode;
    AMBA_DSP_FRAME_RATE_s  FrameRate;
    SVC_GUI_CFG_s   GuiCfg;
    SVC_VIN_VOUT_SYNC_CFG_s VinVoutSyncCfg;
    UINT32 DualVoutSyncDelay;   // 1 unit = 100 us
} SVC_DISP_STRM_s;

typedef struct {
    UINT32           DispNum;
    UINT32           DispBits;
    SVC_DISP_STRM_s  DispStrm[AMBA_DSP_MAX_VOUT_NUM];
} SVC_DISP_ALT_s;

typedef struct {
    #define SVC_REC_SRC_VIDEO       (0x00000001U)
    #define SVC_REC_SRC_AUDIO       (0x00000002U)
    #define SVC_REC_SRC_DATA        (0x00000004U)
    UINT32                 SrcBits;

    #define SVC_REC_DST_FILE        (0x00000001U)
    #define SVC_REC_DST_NMLEVT      (0x00000002U)
    #define SVC_REC_DST_EMGEVT      (0x00000004U)
    #define SVC_REC_DST_NET         (0x00000008U)
    UINT32                 DestBits;

    UINT32                 RecId;
    UINT32                 AEncBits;
    UINT32                 M;
    UINT32                 N;
    UINT32                 IdrInterval;
    UINT32                 BitRate;
    #define SVC_TLS_BIT_ON          (0x01U)
    #define SVC_TLS_BIT_FRC_SIMGOP  (0x10U)
    UINT32                 TimeLapse;
    AMBA_DSP_FRAME_RATE_s  FrameRate;
    UINT32                 MVInfoFlag;
    UINT32                 NumTile;
    UINT32                 NumSlice;
    UINT32                 IRCycle;
    UINT32                 GOPStruct;
    UINT32                 UseAuxStg;
    UINT32                 BootToRec;
    UINT8                  Rotate;
    UINT32                 VEngineId;
} SVC_REC_SETTING_s;

typedef struct {
    SVC_REC_SETTING_s  RecSetting;
    SVC_STRM_CFG_s     StrmCfg;
} SVC_REC_STRM_s;

typedef struct {
    UINT16  Index;
    UINT32  StartX;
    UINT32  StartY;
    UINT32  Reserved0;
    UINT32  Reserved1;
} SVC_RES_CV_ROI_s;

typedef struct {
    UINT16  DataSrc;
    UINT32  StrmId;
    UINT16  FrameWidth;
    UINT16  FrameHeight;
    UINT16  NumRoi;
    SVC_RES_CV_ROI_s Roi[SVC_RES_CV_MAX_ROI];
} SVC_RES_CV_INPUT_s;

typedef struct {
    UINT16  InputNum;
    SVC_RES_CV_INPUT_s Input[SVC_RES_CV_MAX_INPUT_NUM];
    UINT32  SrcRate;

    UINT8   EnableFeeder;               /* Enable ImgFeeder utility. Depend on driver. */
    UINT32  FeederID;
    UINT32  FeederFlag;
} SVC_RES_CV_INPUT_CFG_s;

typedef struct {
    UINT32 OutputTag[SVC_RES_CV_MAX_OUTPUT_PORT];
} SVC_RES_CV_OUTPUT_CFG_s;

typedef struct {
    /* CVFlow Driver */
    UINT32                  Chan;
    void                    *pCvObj;
    UINT32                  CvModeID;
    SVC_RES_CV_INPUT_CFG_s  InputCfg;
    SVC_RES_CV_OUTPUT_CFG_s OutputCfg;
    UINT32                  PrivFlag;
    UINT32                  Config[SVC_RES_CV_MAX_CFG_PARAM];

    /* Application */
    UINT32 CvFlowType;
    UINT32 CvAppFlag;
    UINT8  DownstrmChanNum;
    UINT8  DownstrmChan[SVC_RES_CV_DOWNSTRM_CHAN_NUM];  /* Downstream CvFlow channel */
} SVC_CV_FLOW_s;

typedef struct {
    UINT8               Compressed;     /* 1 - compressed raw data, 0 - uncompressed raw data */
    UINT8               CeNeeded;
    UINT8               FrameNum;
    UINT16              Width;          /* for capture resource */
    UINT16              Height;
    UINT16              EffectW;        /* for capture process */
    UINT16              EffectH;
    UINT32              SensorMode;
} SVC_RAW_BUF_s;

typedef struct {
    UINT16  OffsetX;                    /* Horizontal offset of the window */
    UINT16  OffsetY;                    /* Vertical offset of the window */
    UINT16  Width;                      /* Number of pixels per line in the raw cropping window */
    UINT16  Height;                     /* Number of lines in the raw cropping window */
    UINT8   R2yScale;                   /* To do yuv scaling during r2y */
    UINT16  OutWidth;                   /* Number of pixels per line for the output yuv when R2yScale = 1 */
    UINT16  OutHeight;                  /* Number of lines for the output yuv when R2yScale = 1 */
} AMBA_R2Y_WINDOW_s;

typedef struct {
    SVC_RAW_BUF_s       MaxRaw;
    UINT8               ChanMask;
    AMBA_R2Y_WINDOW_s   ChanWin[SVC_CAP_MAX_SENSOR_PER_VIN];
} SVC_STILL_RAW_CFG_s;

typedef struct {
    UINT8  Enable;                         /* quick view enabled */
    UINT8  NumStrm;                        /* number of quick view streams */
    UINT8  StrmId[AMBA_DSP_MAX_VOUT_NUM];  /* yuv stream id */
    UINT8  VoutId[AMBA_DSP_MAX_VOUT_NUM];  /* vout id */
    UINT8  QvOnly[AMBA_DSP_MAX_VOUT_NUM];  /* quickview only. 1 - qview only. 0 - qview + PIV */
} SVC_PIV_QVIEW_CFG_s;

typedef struct {
    UINT32 VoutId;
    UINT32 Width;
    UINT32 Height;
} SVC_STILL_QVIEW_WINDOW_s;

typedef struct {
    UINT8  Enable;                                          /* quick view enabled under r2y flow */
    UINT8  NumQview;                                        /* number of quick view streams */
    SVC_STILL_QVIEW_WINDOW_s Win[AMBA_DSP_MAX_VOUT_NUM];    /* quick view size */
} SVC_STILL_QVIEW_CFG_s;

typedef struct {
    UINT8               EnableStill;
    SVC_PIV_QVIEW_CFG_s PivQview;
    UINT8               EnableStillRaw;
    UINT8               EnableHISO;
    UINT8               EnableHDR;
    UINT8               NumVin;
    SVC_STILL_RAW_CFG_s RawCfg[AMBA_DSP_MAX_VIN_NUM];
    SVC_STILL_QVIEW_CFG_s Qview;
} SVC_STILL_CONFIG_s;

typedef struct {
    char             GroupName[32];
    char             FormatName[128];
    char             VinTree[1024];

    UINT32           VinNum;
    SVC_VIN_CFG_s    VinCfg[AMBA_DSP_MAX_VIN_NUM];

    SVC_SENSOR_CFG_s SensorCfg[AMBA_DSP_MAX_VIN_NUM][SVC_MAX_NUM_SENSOR_PER_VIN];

    UINT32           FovNum;
    SVC_FOV_CFG_s    FovCfg[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT32           DispNum;
    UINT32           DispBits;
    SVC_DISP_STRM_s  DispStrm[AMBA_DSP_MAX_VOUT_NUM];

    UINT32           DispAltNum;
    UINT32           DispAltIdx;
    SVC_DISP_ALT_s   DispAlt[SVC_RES_DISP_ALT_NUM];

    UINT32           RecNum;
    UINT32           RecBits;
    SVC_REC_STRM_s   RecStrm[AMBA_DSP_MAX_STREAM_NUM];

    UINT32           CvFlowNum;
    UINT32           CvFlowBits;
    SVC_CV_FLOW_s    CvFlow[SVC_RES_CV_FLOW_NUM];

    SVC_STILL_CONFIG_s StillCfg;
    UINT32             UserFlag;

    SVC_CLK_CFG_s  ClkCfg;
} SVC_RES_CFG_s;

typedef struct {
    UINT32                      VinBits;
    UINT32                      VinSrc[AMBA_DSP_MAX_VIN_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s VinWin[AMBA_DSP_MAX_VIN_NUM];
    SVC_VIN_TIMEOUT_s           VinTimeOut[AMBA_DSP_MAX_VIN_NUM];
    UINT32                      VinOutputNum[AMBA_DSP_MAX_VIN_NUM];

    UINT32                      FovNum;
    UINT32                      FovPyramidEnable[AMBA_DSP_MAX_VIEWZONE_NUM];
    UINT32                      FovPyramidExScaleEnable[AMBA_DSP_MAX_VIEWZONE_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s FovWin[AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT32                      DispNum;
    UINT32                      DispFormat[AMBA_DSP_MAX_VOUT_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s DispMaxWin[AMBA_DSP_MAX_VOUT_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s DispWin[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                      DispChanNum[AMBA_DSP_MAX_VOUT_NUM];
    UINT32                      DispChanBits[AMBA_DSP_MAX_VOUT_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s DispChan[AMBA_DSP_MAX_VOUT_NUM][AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT32                      RecNum;
    UINT32                      RecGopM[AMBA_DSP_MAX_STREAM_NUM];
    UINT32                      RecFormat[AMBA_DSP_MAX_STREAM_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s RecMaxWin[AMBA_DSP_MAX_STREAM_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s RecWin[AMBA_DSP_MAX_STREAM_NUM];
    UINT32                      RecChanNum[AMBA_DSP_MAX_STREAM_NUM];
    UINT32                      RecChanBits[AMBA_DSP_MAX_STREAM_NUM];
    AMBA_DSP_WINDOW_DIMENSION_s RecChan[AMBA_DSP_MAX_STREAM_NUM][AMBA_DSP_MAX_VIEWZONE_NUM];

    UINT32                      StillNum;
    UINT32                      StillFormat;
    AMBA_DSP_WINDOW_DIMENSION_s StillWin;
    AMBA_DSP_WINDOW_DIMENSION_s StillQviewWin;
    UINT32                      StillEn;
    UINT32                      StillYuvCapBufSize;
    UINT32                      StillRawCapBufSize;
    UINT32                      StillRawW;
    UINT32                      StillRawH;
    UINT32                      StillCeEn;
    UINT32                      StillRawUncomp;

    UINT32                      VirtVinBits;
    UINT32                      VirtVinOutputNum[AMBA_DSP_MAX_VIN_NUM];
    UINT32                      VirtVinWidth[AMBA_DSP_MAX_VIN_NUM];
    UINT32                      VirtVinHeight[AMBA_DSP_MAX_VIN_NUM];
    UINT32                      MaxVinTimeout;
} SVC_RES_CFG_MAX_s;

UINT32 SvcResCfg_Init(const SVC_RES_CFG_s* pResCfg);
UINT32 SvcResCfg_Config(const SVC_RES_CFG_s* pResCfg);
UINT32 SvcResCfg_ConfigMax(const SVC_RES_CFG_s *pResCfgArr, UINT32 Num);
SVC_RES_CFG_s* SvcResCfg_Get(void);
SVC_RES_CFG_MAX_s* SvcResCfg_GetMax(void);
UINT32 SvcResCfg_GetVinVirtualChan(UINT32 VinID, UINT32 *pIsVirtual, UINT32 *pFromVinID);
UINT32 SvcResCfg_GetVinSrc(UINT32 VinID, UINT32 *pVinSrc);
UINT32 SvcResCfg_GetFovSrc(UINT32 FovIdx, UINT32 *pFovSrc);
UINT32 SvcResCfg_GetVinIDs(UINT32 *pVinID, UINT32 *pVinNum);
UINT32 SvcResCfg_GetVinIDOfFovIdx(UINT32 FovIdx, UINT32 *pVinID);
UINT32 SvcResCfg_GetDecIDOfFovIdx(UINT32 FovIdx, UINT32 *pDecID);
UINT32 SvcResCfg_GetSensorIDOfSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pSensorID);
UINT32 SvcResCfg_GetSensorIDOfFovIdx(UINT32 FovIdx, UINT32 *pVinID, UINT32 *pSensorID);
UINT32 SvcResCfg_GetSensorIdxOfFovIdx(UINT32 FovIdx, UINT32 *pVinID, UINT32 *pSensorIdx);
UINT32 SvcResCfg_GetSensorIDInVinID(UINT32 VinID, UINT32 *pSensorID);
UINT32 SvcResCfg_GetSensorIdxsInVinID(UINT32 VinID, UINT32 *pSensorIdx, UINT32 *pSensorNum);
UINT32 SvcResCfg_GetFovIdxs(UINT32 *pFovIdx, UINT32 *pFovNum);
UINT32 SvcResCfg_GetFovIdxsInVinID(UINT32 VinID, UINT32 *pFovIdx, UINT32 *pFovNum);
UINT32 SvcResCfg_GetFovIdxsInSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pFovIdx, UINT32 *pFovNum);
UINT32 SvcResCfg_GetSensorGroupOfSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pSensorGroup);
UINT32 SvcResCfg_GetSerdesIdxOfSensorIdx(UINT32 VinID, UINT32 SensorIdx, UINT32 *pSerdesIdx);
UINT32 SvcResCfg_GetSerdesIdxOfFovIdx(UINT32 FovIdx, UINT32 *pSerdesIdx);

void   SvcResCfg_GetVinBitsOfRecIdx(UINT32 RecIdx, UINT32 *pVinBits);
void   SvcResCfg_GetFovBitsOfRecIdx(UINT32 RecIdx, UINT32 *pFovBits);

UINT32 SvcResCfg_GetFovIdxsFromDec(UINT32 *pFovIdx, UINT32 *pFovNum);
UINT32 SvcResCfg_GetFovIdxsFromMem(UINT32 *pFovIdx, UINT32 *pFovNum);
UINT32 SvcResCfg_GetFovIdxBypassFrom(UINT32 FovIdx, UINT32 *pFovIdxFrom, UINT32 *pValid);

UINT32 SvcResCfg_ConfigDispAlt(UINT32 DispAltIdx);

UINT32 SvcResCfg_SetDebugEnable(UINT32 DebugEnable);

UINT32 SvcResCfg_GetStillEnable(UINT8 *pStillEnable);

UINT32 SvcResCfg_Dump(void);
UINT32 SvcResCfg_DumpMax(void);
#if defined(CONFIG_APP_FLOW_CARDV_AONI)
UINT32 SvcResCfg_GetVinIDOfRecIdx(UINT32 RecIdx, UINT32 *pVinId);
#endif

#endif  /* SVC_RES_CFG_H */
