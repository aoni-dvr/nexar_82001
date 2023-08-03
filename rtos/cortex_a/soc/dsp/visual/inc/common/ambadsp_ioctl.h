/**
 *  @file ambadsp_ioctl.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Header of Ambadsp devcie cmd handler
 *
 */

#ifndef AMBADSP_IOCTL_H
#define AMBADSP_IOCTL_H

#if defined (CONFIG_QNX)
#include <stdint.h>
#include <sys/ioctl.h>
#else
#include "dsp_types.h"
#endif
#include "AmbaDSP.h"
#include "AmbaDSP_Event.h"
#include "AmbaDSP_VOUT.h"
#include "AmbaDSP_Liveview.h"
#include "AmbaDSP_Liveview_Def.h"
#include "AmbaDSP_VideoEnc.h"
#include "AmbaDSP_StillCapture.h"
#include "AmbaDSP_VideoDec.h"
#include "AmbaDSP_StillDec.h"
#include "AmbaDSP_Image3aStatistics.h"

#ifdef CONFIG_ENABLE_DSP_MONITOR
#include "AmbaDSP_Monitor.h"
#endif

#define IN
#define OUT

/******************   Ultility  ******************/
#define DSP_DBG_TYPE_API            (0U)
#define DSP_DBG_TYPE_API_CHK        (1U)
#define DSP_DBG_TYPE_CMD            (2U)
#define DSP_DBG_TYPE_CMD_ALL        (3U)
#define DSP_DBG_TYPE_MSG            (4U)
#define DSP_DBG_TYPE_MSG_ALL        (5U)
#define DSP_DBG_TYPE_LOG_THRD       (6U)
#define DSP_DBG_TYPE_LOG_LVL        (7U)
#define DSP_DBG_TYPE_LL_LOG_SHOW    (8U)
#define DSP_DBG_TYPE_INIT_DATA_SHOW (9U)
#define DSP_DBG_TYPE_STOP_DSP       (10U)
#define DSP_DBG_TYPE_SYSCFG         (11U)


typedef struct {
    IN  UINT32 Type;
    IN  UINT32 Data[7];
} dsp_debug_dump_t;

typedef struct {
    IN  UINT32 Type; // NUM_DSP_STATUS_TYPE
    IN  UINT32 Id;
    OUT UINT32 Data[6];
} dsp_status_t;

typedef struct {
    IN  UINT16 MaxMainWidth;
    IN  UINT16 MaxMainHeight;
    IN  UINT16 MaxHierWidth;
    IN  UINT16 MaxHierHeight;
    IN  UINT16 OctaveMode;
    OUT UINT32 HierBufSize;
    OUT UINT16 HierBufWidth;
    OUT UINT16 HierBufHeight;
} dsp_hier_buf_calc_t;

typedef struct {
    IN  UINT16 Width;
    IN  UINT16 CmprRate;
    OUT UINT16 RawWidth;
    OUT UINT16 RawPitch;
} dsp_raw_pitch_calc_t;

typedef struct {
    IN  UINT16 Width;
    IN  UINT16 Height;
    IN  UINT32 Option;
    OUT UINT32 BufSize;
} dsp_enc_mv_buf_calc_t;

typedef struct {
    IN  UINT16 Width;
    IN  UINT16 Height;
    IN  UINT16 BufType;
    IN  UINT32 Option;
    IN  ULONG  MvBufAddr;
} dsp_enc_mv_buf_parse_t;

typedef struct {
    IN  UINT32 NumMsgs;
    OUT UINT32 MsgSize;
} dsp_vp_msg_buf_t;

typedef struct {
    IN  UINT32 Op;  //NUM_DSP_CACHE_OP
    IN  ULONG  Addr;
    IN  UINT32 Size;
} dsp_cache_buf_op_t;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
typedef struct {
    IN  UINT8  NumTestFrame;
    IN  UINT8  Reserved[3U];
    IN  UINT32 Interval;            /* In 100usec unit, interval time wil only valid in last TestFrameCfg */
    IN  ULONG  TestFrameCfg[DSP_MAX_TEST_FRAME_NUM];
    IN  ULONG  TestFrameInp[DSP_MAX_TEST_INP_MAX];
    IN  ULONG  BitsBufAddr;          /* Used when TEST_STAGE_VDSP_0, the Addr MUST be identical to normal encode one */
    IN  UINT32 BitsBufSize;          /* Used when TEST_STAGE_VDSP_0, the Size MUST be identical to normal encode one */
} dsp_test_frame_config_in_t;

typedef struct {
    IN  UINT32 StageId;
    IN  dsp_test_frame_config_in_t TfCfg;
} dsp_test_frame_config_t;

typedef struct {
    IN  UINT32 NumStage;
    IN  AMBA_DSP_TEST_FRAME_CTRL_s TfCtrl;
} dsp_test_frame_control_t;

typedef struct {
    IN  UINT32 Id;
    IN  UINT32 SubId;
    OUT UINT32 Status;
} dsp_main_safety_check_t;

typedef struct {
    IN  UINT32 Id;
    IN  UINT32 Val0;
    IN  UINT32 Val1;
} dsp_main_safety_config_t;
#endif

#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
typedef struct {
    IN  UINT16 Id;  /* B[15:12]Type B[11:0]Id */
                    /*   Type [0] Liveview, not used now */
                    /*   Type [1] StlR2Y */
                    /*   Type [2] StlR2R */
                    /*   Type [3] StlY2Y */
                    /*   Id, not used now */
    IN  AMBA_DSP_WINDOW_DIMENSION_s Input;
    IN  AMBA_DSP_WINDOW_DIMENSION_s Output;
    OUT AMBA_DSP_SLICE_CFG_s SliceCfg;
} dsp_slice_cfg_t;
#endif

/******************   Event    ******************/
typedef struct {
    IN  UINT16 EventId;
    IN  UINT16 MaxNumHandlers;
} dsp_evnt_hdlr_cfg_t;

typedef struct {
    IN  UINT16 EventId;
} dsp_evnt_hdlr_op_t;

typedef struct {
    OUT ULONG  Addr;
    OUT UINT16 BufIdx; //for release event info buffer, Bit[15:12]=BufCategory, Bit[11:0]=PoolIdx
} dsp_stat_buf_t;

typedef struct {
    IN  UINT32 EventId:16;
    OUT UINT32 EvntDataValid:1;   //indicates it is valid event data
    OUT UINT32 PoolEmpty:1;       //indicates there is remaining data or not
    IN  UINT32 rsvd:14;

    OUT dsp_stat_buf_t EventData;
} dsp_evnt_data_t;

/******************    Main    ******************/
typedef struct {
    ULONG  Addr;
    UINT32 Size;
    UINT32 IsCached;
} dsp_mem_t;

typedef struct {
    IN  UINT32 Flag;
    OUT UINT32 ActualFlag;
    IN  UINT32 TimeOut;
} dsp_wait_sig_t;

typedef struct {
    IN  UINT32 Enable;
    IN  UINT32 Data;
} dsp_partial_load_cfg_t;

typedef struct {
    IN  UINT16 RegionId;
    IN  UINT16 SubRegionId;
} dsp_partial_load_unlock_t;

typedef struct {
    UINT32 Data;
} sys_drv_cfg_t;

#define DSP_SYS_DRV_CFG_TYPE_IDSP_CLK       (0x00U)
#define DSP_SYS_DRV_CFG_TYPE_CORE_CLK       (0x01U)
#define DSP_SYS_DRV_CFG_TYPE_DDR_CLK        (0x02U)
#define DSP_SYS_DRV_CFG_TYPE_AUD_CLK        (0x03U)
#define DSP_SYS_DRV_CFG_TYPE_IDSPV_CLK      (0x04U)
#define DSP_SYS_DRV_CFG_TYPE_CORE_CLK_PRE   (0x101U)
#define NUM_DSP_SYS_DRV_CFG_TYPE            (0x102U)
typedef struct {
    IN  UINT16 Type;
    IN  UINT16 Rsvd;
    IN  sys_drv_cfg_t SysDrvCfg;
} dsp_sys_drv_cfg_t;

typedef struct {
    IN  UINT32 Type;
    IN  ULONG  Addr;
    IN  UINT32 Size;
    IN  UINT32 IsCached;
} dsp_protect_area_t;

#define DSP_EXT_BUF_TYPE_RESC_RAW   (0U)
#define NUM_DSP_EXT_BUF_TYPE        (1U)
typedef struct {
    IN  UINT32 Type;
    IN  UINT16 Id;
    IN  UINT16 SubId;
    IN  ULONG  BufTbl[MAX_EXT_DISTINCT_DEPTH];      //8*64 = 512B
    IN  ULONG  AuxBufTbl[MAX_EXT_DISTINCT_DEPTH];   //8*64 = 512B
} dsp_ext_buf_tbl_t;

typedef struct {
    IN  UINT16 AllocType;
    IN  UINT16 BufNum;
    IN  AMBA_DSP_RAW_BUF_s Buf;
    IN  AMBA_DSP_RAW_BUF_s AuxBuf;
    IN  UINT16 RawFormat;
} dsp_liveview_raw_buf_t;

typedef struct {
    IN  UINT32 VideoPipe;
    IN  UINT16 LowDelayMode;
    IN  UINT16 MaxVinBit;
    IN  UINT16 MaxViewZoneNum;
    IN  UINT16 MaxVinVirtChanBit[AMBA_DSP_MAX_VIN_NUM];
    IN  UINT16 MaxVinVirtChanOutputNum[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    IN  UINT16 MaxVinVirtChanWidth[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    IN  UINT16 MaxVinVirtChanHeight[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    IN  dsp_liveview_raw_buf_t RawBuf[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    IN  UINT32 MaxVinBootTimeout[AMBA_DSP_MAX_VIN_NUM];
    IN  UINT32 MaxVinTimeout[AMBA_DSP_MAX_VIN_NUM];
    IN  UINT16 MaxRaw2YuvDelay[AMBA_DSP_MAX_VIN_NUM];
    IN  UINT16 MaxWarpDma[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxWarpWaitLineLuma[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxWarpWaitLineChroma[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxHierWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxHierHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxLndtWidth[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxLndtHeight[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT16 MaxVirtVinBit;
    IN  UINT16 MaxVirtVinOutputNum[AMBA_DSP_MAX_VIRT_VIN_NUM];
    IN  UINT16 MaxVirtVinWidth[AMBA_DSP_MAX_VIRT_VIN_NUM];
    IN  UINT16 MaxVirtVinHeight[AMBA_DSP_MAX_VIRT_VIN_NUM];
    IN  AMBA_DSP_DEFAULT_RAW_ISOCFG_CTRL_s DefaultRawIsoConfigAddr[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  AMBA_DSP_RAW_BUF_s DefaultRawBuf[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    IN  AMBA_DSP_RAW_BUF_s DefaultAuxRawBuf[AMBA_DSP_MAX_VIN_NUM][AMBA_DSP_MAX_VIRT_CHAN_NUM];
    IN  UINT16 DefaultRawFormat[AMBA_DSP_MAX_VIN_NUM];
} dsp_liveview_resc_t;

typedef struct {
    IN  dsp_liveview_resc_t             LiveviewResource;
    IN  AMBA_DSP_ENCODE_RESOURCE_s      EncodeResource;
    IN  AMBA_DSP_STILL_RESOURCE_s       StillResource;
    IN  AMBA_DSP_DECODE_RESOURCE_s      DecodeResource;
    IN  AMBA_DSP_DISPLAY_RESOURCE_s     DisplayResource;
    IN  AMBA_DSP_EXTMEM_ENC_RESOURCE_s  ExtMemEncResource;
    IN  AMBA_DSP_LIVEVIEW_RESOURCE_EX_s LiveviewResourceEx;
} dsp_resc_limit_cfg_t;

typedef struct {
    IN  UINT32 Type;
    OUT dsp_mem_t Buf;
} dsp_buf_info_t;

/******************    Vout    ******************/
typedef struct {
    IN  UINT8  VoutIdx;
    IN  AMBA_DSP_VOUT_MIXER_CONFIG_s MixerCfg;
} dsp_vout_mixer_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT32 BackColorYUV;
} dsp_vout_mixer_bgc_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT8  LumaThreshold;
    IN  UINT32 HighlightColorYUV;
} dsp_vout_mixer_hlc_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT8  CscCtrl;
} dsp_vout_mixer_csc_cfg_t;

typedef struct {
    INT32 Coef[3][3];
    INT32 Offset[3];
    UINT16 MinVal[3];
    UINT16 MaxVal[3];
} dsp_vout_csc_matrix_s;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  dsp_vout_csc_matrix_s CscMatrix;
} dsp_vout_mixer_csc_matrix_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  AMBA_DSP_VOUT_OSD_BUF_CONFIG_s BufCfg;
} dsp_vout_osd_buf_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT8  Enable;
    IN  UINT8  SyncWithVin;
    OUT UINT64 AttachedRawSeq;
} dsp_vout_osd_ctrl_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  AMBA_DSP_DISPLAY_CONFIG_s Cfg;
} dsp_vout_disp_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  ULONG  TableAddr;
} dsp_vout_disp_gamma_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT8  Enable;
} dsp_vout_disp_gamma_ctrl_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  AMBA_DSP_VOUT_VIDEO_CFG_s Cfg;
} dsp_vout_video_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT8  Enable;
    IN  UINT8  SyncWithVin;
    OUT UINT64 AttachedRawSeq;
} dsp_vout_video_ctrl_t;

typedef struct {
    IN  UINT8  NumVout;
    IN  UINT8  VoutIdx[AMBA_DSP_MAX_VOUT_NUM];
    IN  AMBA_DSP_VOUT_DATA_PATH_CFG_s PathCfg[AMBA_DSP_MAX_VOUT_NUM];
} dsp_vout_mixer_bind_cfg_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  UINT8  DveMode;
} dsp_vout_dve_cfg_t;

typedef struct {
    /* DispCfg[512] MixerCsc[36] Tve[512]  */
    UINT8  Data[512U];
} vout_drv_cfg_t;

#define DSP_VOUT_DRV_CFG_TYPE_DISP      (0U)
#define DSP_VOUT_DRV_CFG_TYPE_MIXER_CSC (1U)
#define DSP_VOUT_DRV_CFG_TYPE_TVE       (2U)
#define NUM_DSP_VOUT_DRV_CFG_TYPE       (3U)
typedef struct {
    IN  UINT16 VoutId;
    IN  UINT16 Type;
    IN  vout_drv_cfg_t VoutDrvCfg;
} dsp_vout_drv_cfg_t;

/******************  Liveview  ******************/
typedef struct {
    IN  UINT16 AllocType;
    IN  UINT16 BufNum;
    IN  AMBA_DSP_YUV_IMG_BUF_s YuvBuf; //27B
    IN  ULONG YuvBufTbl[MAX_EXT_DISTINCT_DEPTH]; //8*64 = 512B
} dsp_liveview_yuv_buf_t; //543B

typedef struct {
    IN  UINT16 ViewZoneId;
    IN  AMBA_DSP_WINDOW_s ROI; //8B
    IN  AMBA_DSP_WINDOW_s Window; //8B
    IN  UINT8 RotateFlip;
    IN  UINT8 BlendNum;
    IN  AMBA_DSP_BUF_s BldBuf[AMBA_DSP_MAX_YUVSTRM_BLD_NUM]; //18*2 = 36B
} dsp_liveview_chan_cfg_t; //56B

typedef struct {
    IN  UINT16 ViewZoneId;
    IN  UINT16 VinSource;
    IN  UINT16 VinId;
    IN  UINT16 VinVirtChId;
    IN  UINT8  IsVirtChan;
    IN  AMBA_DSP_WINDOW_s VinRoi; //8B
    IN  UINT16 MainWidth;
    IN  UINT16 MainHeight;
    IN  UINT8  RotateFlip;
    IN  UINT8  HdrExposureNum;
    IN  UINT16 HdrBlendHieght;
    IN  UINT8  MctfDisable;
    IN  UINT8  MctsDisable;
    IN  UINT8  LinearCE;
    IN  AMBA_DSP_LIVEVIEW_PYRAMID_s Pyramid; //60B
    IN  dsp_liveview_yuv_buf_t PyramidBuf; //543B | 31B(NoExtTbl)
    IN  AMBA_DSP_LIVEVIEW_LANE_DETECT_s LaneDetect; //16B
    IN  dsp_liveview_yuv_buf_t LaneDetectBuf; //543B | 31B(NoExtTbl)
    IN  UINT8  VinDecimation;
    IN  UINT8  DramUtilityPipe;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IN  UINT8  ExtraHorWarp;
    IN  UINT8  Rsvd[1U];
#else
    IN  UINT8  Rsvd[2U];
#endif
    IN  UINT32 InputMuxSelCfg;
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IN  dsp_liveview_yuv_buf_t MainY12Buf;
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IN  dsp_liveview_yuv_buf_t MainIrBuf;
#endif
#else
    IN  UINT32 UpstreamSource;
    IN  dsp_liveview_yuv_buf_t IntMainBuf;
#endif
} dsp_liveview_viewzone_cfg_t; //1198B | 174B(NoExtTbl)

typedef struct {
    IN  UINT16 StreamId;
    IN  UINT16 Purpose;
    IN  UINT16 DestVout;
    IN  UINT16 DestEnc;
    IN  UINT16 Width;
    IN  UINT16 Height;
    IN  UINT16 MaxWidth;
    IN  UINT16 MaxHeight;
    IN  dsp_liveview_yuv_buf_t StreamBuf; //543B | 31B(NoExtTbl)
    IN  UINT16 NumChan;
    IN  dsp_liveview_chan_cfg_t ChanCfg[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM]; //56*16=896B
    IN  UINT32 MaxChanBitMask;
    IN  UINT16 MaxChanNum;
    IN  AMBA_DSP_WINDOW_DIMENSION_s MaxChanWin[AMBA_DSP_MAX_YUVSTRM_VIEW_NUM]; //4*16 = 64B
    IN  UINT32 DestDeciRate;
    IN  UINT32 OptimizeOption;
} dsp_liveview_strm_cfg_t; //1531B | 1019B(NoExtTbl)

#define DSP_LV_CFG_TYPE_VIEWZONE    (0U)
#define DSP_LV_CFG_TYPE_YUVSTRM     (1U)
typedef struct {
    /* 128align : max of {dsp_liveview_viewzone_cfg_t[1804], dsp_liveview_strm_cfg_t[1176]} */
    UINT8 Data[1920U];
} dsp_lv_cfg_t;

typedef struct {
    IN  UINT32 Type:8;
    IN  UINT32 FreshNew:1;  //reset lv-cfg on given type
    IN  UINT32 CfgMask:2;  //B[0]viewzone, B[1]YuvStrm
    IN  UINT32 rsvd:21;

    IN  UINT16 TotalNum;
    IN  UINT16 Index;
    IN  dsp_lv_cfg_t cfg;
} dsp_liveview_cfg_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  UINT16 ViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT8  Enable[AMBA_DSP_MAX_VIEWZONE_NUM];
} dsp_liveview_ctrl_t;

typedef struct {
    IN  UINT32 FreshNew:1;  //reset lv-upt-cfg
    IN  UINT32 rsvd:31;

    IN  UINT16 TotalNum;
    IN  UINT16 Index;

    IN  dsp_liveview_strm_cfg_t YuvStrmCfg; //1531*32=48992 | 1019*32=32608B(NoExtTbl)
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_update_cfg_t; //1547*32=49504 | 1035*32=33120B(NoExtTbl)

typedef struct {
    IN  UINT16 NumViewZone;
    IN  AMBA_DSP_LIVEVIEW_ISOCFG_CTRL_s IsoCfgCtrl[AMBA_DSP_MAX_VIEWZONE_NUM];
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_update_isocfg_t;

typedef struct {
    IN  UINT16 VinId;
    IN  UINT16 SubChNum;
    IN  AMBA_DSP_VIN_SUB_CHAN_s SubCh[AMBA_DSP_MAX_VIRT_CHAN_NUM]; //4*1=4B
    IN  AMBA_DSP_LIVEVIEW_VINCFG_CTRL_s LvVinCfgCtrl[AMBA_DSP_MAX_VIRT_CHAN_NUM]; //18*1=18B
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_update_vincfg_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  UINT16 ViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  AMBA_DSP_LIVEVIEW_PYRAMID_s Pyramid[AMBA_DSP_MAX_VIEWZONE_NUM]; //60*16=960B
    IN  dsp_liveview_yuv_buf_t PyramidBuf[AMBA_DSP_MAX_VIEWZONE_NUM]; //543*16=8688B | 496B(NoExtTbl)
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_update_pymdcfg_t;

typedef struct {
    IN  AMBA_DSP_VIN_SUB_CHAN_s SubChan;
    IN  UINT16 Option;
    IN  UINT16 ConCatNum;
    IN  UINT16 IntcNum;
    IN  AMBA_DSP_WINDOW_s CaptureWindow;
    IN  UINT16 TDNum;
    IN  UINT16 TDFrmNum[AMBA_DSP_MAX_VIN_TD_NUM];
    IN  UINT16 ProcDeciRate;
} dsp_vin_sub_chan_cfg_t; //38B

typedef struct {
    IN  UINT16 VinId;
    IN  UINT16 SubChNum;
    IN  dsp_vin_sub_chan_cfg_t SubChCfg[AMBA_DSP_MAX_VIRT_CHAN_NUM]; //38*1=38B
} dsp_liveview_vin_cap_cfg_t; //42B

typedef struct {
    IN  UINT8  Type;
    IN  UINT16 VinId;
} dsp_liveview_vin_post_cfg_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  UINT16 ViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  AMBA_DSP_LIVEVIEW_EXT_RAW_s ExtBuf[AMBA_DSP_MAX_VIEWZONE_NUM];
} dsp_liveview_feed_raw_data_t;

typedef struct {
    IN  AMBA_DSP_YUV_IMG_BUF_s ExtYuvBuf;
    IN  AMBA_DSP_BUF_s ExtME1Buf;
    IN  AMBA_DSP_BUF_s ExtME0Buf;
    IN  UINT8 IsLast;
} dsp_ext_yuv_buf_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  UINT16 ViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  dsp_ext_yuv_buf_t ExtBuf[AMBA_DSP_MAX_VIEWZONE_NUM];
} dsp_liveview_feed_yuv_data_t;

typedef struct {
    IN  UINT16 YuvStrmIdx;
    IN  AMBA_DSP_LIVEVIEW_SYNC_CTRL_s YuvStrmSyncCtrl;
    OUT UINT32 SyncJobId;
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_yuvstrm_sync_t;

typedef struct {
    IN  UINT16 ViewZoneId;
    IN  AMBA_DSP_LIVEVIEW_GEOCFG_CTRL_s GeoCfgCtrl;
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_update_geocfg_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  AMBA_DSP_LIVEVIEW_VZ_SRC_CFG_s VzSrcCfg[AMBA_DSP_MAX_VIEWZONE_NUM];
    OUT UINT64 AttachedRawSeq;
} dsp_liveview_update_vz_src_t;

typedef struct {
    IN  UINT16 ViewZoneId;
    IN  UINT8  NumBand;
    IN  ULONG  SidebandBufAddr[AMBA_DSP_MAX_TOKEN_ARRAY];
} dsp_liveview_update_sideband_t;

typedef struct {
    IN  UINT16 ViewZoneId;
    IN  AMBA_DSP_LIVEVIEW_SLICE_CFG_s LvSliceCfg;
} dsp_liveview_slice_cfg_t;

typedef struct {
    IN  UINT16 ViewZoneId;
    IN  AMBA_DSP_LV_VZ_POSTPONE_CFG_s VzPostPoneCfg;
} dsp_liveview_vz_postpone_cfg_t;

typedef struct {
    IN  UINT16 NumVin;
    IN  AMBA_DSP_LV_VIN_STATE_s VinState[AMBA_DSP_MAX_VIN_NUM];
} dsp_liveview_update_vin_state_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  UINT16 ViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT32 Ratio[AMBA_DSP_MAX_VIEWZONE_NUM];
} dsp_liveview_slow_shutter_ctrl_t;

typedef struct {
    IN  UINT16 NumViewZone;
    IN  UINT16 ViewZoneId[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT8  Method[AMBA_DSP_MAX_VIEWZONE_NUM];
    IN  UINT8  Option[AMBA_DSP_MAX_VIEWZONE_NUM];
} dsp_liveview_drop_repeat_ctrl_t;

typedef struct {
    IN  AMBA_DSP_FRAME_RATE_s FrameRate;
#define DSP_VIN_COLOR_SPACE_RGB     (0U)
#define DSP_VIN_COLOR_SPACE_YUV     (1U)
#define DSP_VIN_COLOR_SPACE_RGBIR   (2U)
#define DSP_VIN_COLOR_SPACE_RCCC    (3U)
    IN  UINT8  ColorSpace;
    IN  UINT8  BayerPattern;
    IN  UINT8  YuvOrder;
    IN  UINT32 NumDataBits;
    IN  UINT8  NumSkipFrame;
} dsp_vin_drv_cfg_cfa_t;

typedef struct {
    /* 128align : max of {VinCfg[128], dsp_vin_drv_cfg_cfa_t[24]} */
    UINT8  Data[128U];
} vin_drv_cfg_t;

#define DSP_VIN_DRV_CFG_TYPE_DATA   (0U)    // 128byte data
#define DSP_VIN_DRV_CFG_TYPE_CFA    (1U)    // dsp_vin_drv_cfg_cfa_t
#define NUM_DSP_VIN_DRV_CFG_TYPE    (2U)
typedef struct {
    IN  UINT16 VinId;
    IN  UINT16 Type;
    IN  vin_drv_cfg_t VinDrvCfg;
} dsp_liveview_vin_drv_cfg_t;

#define DSP_IK_DOL_OFST_NUM     (3U)
typedef struct {
    IN  UINT8  Pipe;
    IN  UINT32 Y[DSP_IK_DOL_OFST_NUM];
} dsp_ik_drv_cfg_dol_offset_t;

typedef struct {
    IN UINT32 Id;
    IN ULONG  Addr;
} dsp_ik_drv_cfg_iso_t;

typedef struct {
#define DSP_IK_PIPE_VDO             (0U)
#define DSP_IK_PIPE_STL             (1U)
    IN UINT8  Pipe;
#define DSP_IK_STL_PIPE_LI          (0U)
#define DSP_IK_STL_PIPE_HI          (1U)
    IN UINT8  StlPipe;
} dsp_ik_drv_cfg_ability_t;

typedef struct {
#define DSP_IK_SNSR_MODE_NORMAL     (0U)
#define DSP_IK_SNSR_MODE_RGB_IR     (1U)
#define DSP_IK_SNSR_MODE_RCCC       (2U)
    IN UINT8  Pipe;
    IN UINT32 SensorMode;
    IN UINT32 Compression;
} dsp_ik_drv_cfg_sensor_info_t;

typedef struct {
    IN UINT8  Pipe;
    IN UINT32 Compression;
} dsp_ik_drv_cfg_ext_raw_cmpr_info_t;

typedef struct {
    /* 128align : max of {dsp_ik_drv_cfg_sensor_info_t[12],
     *                    dsp_ik_drv_cfg_ability_t[2],
     *                    dsp_ik_drv_cfg_iso_t[16],
     *                    dsp_ik_drv_cfg_dol_offset_t[20]}
     */
    IN UINT8 Data[128U];
} ik_drv_cfg_t;

#define DSP_IK_DRV_CFG_TYPE_SNSR_INFO       (0U)
#define DSP_IK_DRV_CFG_TYPE_ABILITY         (1U)
#define DSP_IK_DRV_CFG_TYPE_ISO             (2U)
#define DSP_IK_DRV_CFG_TYPE_DOL_OFST        (3U)
#define DSP_IK_DRV_CFG_TYPE_EXT_RAW_CMPR    (4U)
#define NUM_DSP_IK_DRV_CFG_TYPE             (5U)
typedef struct {
    IN  UINT16 ViewZoneId;
    IN  UINT16 Type;
    IN  ik_drv_cfg_t IkDrvCfg;
} dsp_liveview_ik_drv_cfg_t;

typedef struct {
    IN  UINT16 ViewZoneId;
    OUT ULONG  IdspCfgAddr;
} dsp_liveview_idsp_cfg_t;

#define DSP_VIN_HIST_MAIN    (0U) // LE
#define DSP_VIN_HIST_HDR     (1U) // SE
#define DSP_VIN_HIST_HDR2    (2U) // VSE
#define DSP_VIN_HIST_NUM     (3U)

/* debug_cfg_info_t */
typedef struct {
  ULONG  CfgAddr;
  UINT32 CfgSize;
} IDSP_CFG_INFO_s;

/* debug_yuv_info_t */
typedef struct {
  ULONG  LumaAddr;
  ULONG  ChromaAddr;
  UINT16 LumaPitch;
  UINT16 LumaWidth;
  UINT16 LumaHeight;
  UINT16 ChromaPitch;
  UINT16 ChromaWidth;
  UINT16 ChromaHeight;
} IDSP_YUV_INFO_s;

/* debug_buf_info_t */
typedef struct {
  ULONG  Addr;
  UINT16 Pitch;
  UINT16 Width;
  UINT16 Height;
  UINT16 reserved;
} IDSP_BUF_INFO_s;

/* debug_c2y_info_t */
typedef struct {
  UINT32 CapSeqNo;

  IDSP_CFG_INFO_s Sec4Cfg;
  IDSP_CFG_INFO_s Sec2Cfg;

  IDSP_YUV_INFO_s Sec2R2y;
  IDSP_BUF_INFO_s Sec2Me1;

  UINT32 Rsvd[16]; //padding
} IDSP_C2Y_INFO_s;

/* debug_me1_smooth_info_t */
typedef struct {
  UINT32 CapSeqNo;

  IDSP_CFG_INFO_s Sec2Cfg;
  IDSP_CFG_INFO_s Sec6Cfg;
  IDSP_CFG_INFO_s Sec5Cfg;

  IDSP_BUF_INFO_s vWarpMe1;
  IDSP_BUF_INFO_s Me1Dn3;

  UINT32 Rsvd[16]; //padding
} IDSP_ME1_SMOOTH_INFO_s;

/* debug_md_info_t */
typedef struct {
  UINT32 CapSeqNo;

  IDSP_CFG_INFO_s Sec2Cfg[3U]; // 0: mda, 1: mdb, 2: mdc
  IDSP_CFG_INFO_s Sec6Cfg;
  IDSP_CFG_INFO_s Sec5Cfg;
  IDSP_CFG_INFO_s MctfCfg;

  IDSP_BUF_INFO_s MdMbB;
  IDSP_BUF_INFO_s MdMbC;
  IDSP_BUF_INFO_s MoAsf;

  UINT32 Rsvd[16]; //padding
} IDSP_MD_INFO_s;

typedef struct {
  UINT32 CapSeqNo;

  IDSP_CFG_INFO_s Sec3Cfg;
  IDSP_CFG_INFO_s MctfCfg;
  IDSP_CFG_INFO_s MctsCfg;

  IDSP_BUF_INFO_s Sec3Me1;
  IDSP_YUV_INFO_s Mctf;
  IDSP_YUV_INFO_s Mcts;
  IDSP_BUF_INFO_s Mctf2bit;

  UINT32 Rsvd[16]; //padding
} IDSP_VWARP_MCTF_INFO_s;

#define IDSP_MAX_AAA_SLICE_NUM  (16U)
typedef struct {
    UINT32 CapSeqNo;
    UINT16 SliceX;
    UINT16 SliceY;
    ULONG  Addr[IDSP_MAX_AAA_SLICE_NUM];
    ULONG  DstAddr;

    UINT32 Rsvd[15]; //padding
} IDSP_AAA_STAT_s;

typedef struct {
    UINT32 CapSeqNo;
    IDSP_CFG_INFO_s VinSecCfg;

    ULONG  CompandTblAddr; // store what SSP rcved from App
    ULONG  DeCompandTblAddr; // store what SSP rcved from App
    UINT32 Rsvd[14]; //padding
} IDSP_VIN_CFG_s;

typedef struct {
    ULONG  IdspInfoBaseAddr;
    UINT32 VinId;
    UINT32 CapSeqNo;
    ULONG  CfgAddr;
} IDSP_VIN_CFG_RDY_s;

typedef struct {
    UINT32 CapSeqNo;

    IDSP_CFG_INFO_s Sec2Cfg;
    IDSP_CFG_INFO_s Sec4Cfg;
    IDSP_CFG_INFO_s Sec5Cfg;
    IDSP_CFG_INFO_s Sec6Cfg;
    IDSP_CFG_INFO_s Sec7Cfg;
    IDSP_CFG_INFO_s Sec9Cfg;
    IDSP_CFG_INFO_s MctfCfg;
    IDSP_CFG_INFO_s MctsCfg;

    IDSP_YUV_INFO_s Sec2;
    IDSP_YUV_INFO_s Sec5;
    IDSP_YUV_INFO_s Sec6;
    IDSP_YUV_INFO_s Sec7;
    IDSP_YUV_INFO_s Sec9;
    IDSP_YUV_INFO_s Mctf;
    IDSP_YUV_INFO_s Mcts;

    UINT32 Rsvd[12]; //padding
} IDSP_HISO_INFO_s;

#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define IDSP_MAX_MEM_NUM    (2U)
typedef struct {
    ULONG  VirtBase;
    ULONG  PhysBase;
    UINT32 Size;
} IDSP_MEM_BLK_s;
#endif

#define DSP_C2Y_TILE_X_NUM  (12U)
#define DSP_C2Y_TILE_Y_NUM  (8U)
#define DSP_HISO_STEP_NUM   (14U) //see DSP_IMG_PASS_HISO_STEP_XX
#define DSP_VIEWZONE_ID_BIT_IDX         (0U)
#define DSP_VIEWZONE_ID_LEN             (12U)
#define DSP_VIEWZONE_TYPE_IDX           (12U)
#define DSP_VIEWZONE_TYPE_LEN           (4U)

#define DSP_VIEWZONE_TYPE_LV            (0U)
#define DSP_VIEWZONE_TYPE_STL_R2Y       (1U)
#define DSP_VIEWZONE_TYPE_STL_R2R       (2U)
#define DSP_VIEWZONE_TYPE_STL_Y2Y       (3U)
#define NUM_DSP_VIEWZONE_TYPE           (4U)
typedef struct {
    UINT32 ChannelId; //for sec2 or later
    UINT32 VinId; //for Vin
    IDSP_C2Y_INFO_s C2Y;
    IDSP_VWARP_MCTF_INFO_s vWarpMctf;
    IDSP_ME1_SMOOTH_INFO_s Me1Smooth;
    IDSP_MD_INFO_s Md;
    IDSP_AAA_STAT_s CfaAAA;
    IDSP_AAA_STAT_s PgAAA;
    IDSP_AAA_STAT_s HistAAA[DSP_VIN_HIST_NUM];
    IDSP_VIN_CFG_s VinCfg;
#define IDSP_PROC_FMT_LISO  (0U)
#define IDSP_PROC_FMT_HISO  (1U)
#define IDSP_PROC_FMT_NUM   (2U)
    UINT32 PipeDescProcFmt:8;
    UINT32 PipeDescRsvd:24;
    IDSP_CFG_INFO_s Sec2TileCfg[DSP_C2Y_TILE_X_NUM][DSP_C2Y_TILE_Y_NUM];
    IDSP_HISO_INFO_s HISO[DSP_HISO_STEP_NUM];
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IDSP_CFG_INFO_s Sec4TileCfg[DSP_C2Y_TILE_X_NUM][DSP_C2Y_TILE_Y_NUM];
#endif
    IDSP_CFG_INFO_s Sec3TileCfg[DSP_C2Y_TILE_X_NUM][DSP_C2Y_TILE_Y_NUM];
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS) || defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IDSP_CFG_INFO_s MctfTileCfg[DSP_C2Y_TILE_X_NUM][DSP_C2Y_TILE_Y_NUM];
    IDSP_BUF_INFO_s MainY12;
    IDSP_MEM_BLK_s MemBlk[IDSP_MAX_MEM_NUM];
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
    IDSP_BUF_INFO_s MainIr;
#endif
} IDSP_INFO_s;

/******************  VideoEnc  ******************/
typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  AMBA_DSP_VIDEO_ENC_STRM_CONFIG_s StreamConfig[AMBA_DSP_MAX_STREAM_NUM];
} dsp_video_enc_cfg_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  AMBA_DSP_VIDEO_ENC_START_CONFIG_s  StartConfig[AMBA_DSP_MAX_STREAM_NUM];
    OUT UINT64 AttachedRawSeq;
} dsp_video_enc_start_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  UINT8  StopOption[AMBA_DSP_MAX_STREAM_NUM];
    OUT UINT64 AttachedRawSeq;
} dsp_video_enc_stop_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  UINT32 Divisor[AMBA_DSP_MAX_STREAM_NUM];
    OUT UINT64 AttachedRawSeq;
} dsp_video_enc_frmrate_ctrl_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  AMBA_DSP_VIDEO_BLEND_CONFIG_s BlendCfg[AMBA_DSP_MAX_STREAM_NUM];
} dsp_video_enc_blend_ctrl_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  AMBA_DSP_VIDEO_ENC_QUALITY_s QCtrl[AMBA_DSP_MAX_STREAM_NUM];
} dsp_video_enc_quality_ctrl_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_STREAM_NUM];
    IN  dsp_ext_yuv_buf_t ExtBuf[AMBA_DSP_MAX_STREAM_NUM];
} dsp_video_enc_feed_yuv_data_t;

typedef struct {
    IN  UINT16 StreamIdx;
    IN  AMBA_DSP_VIDEO_ENC_MV_CFG_s MvCfg;
} dsp_video_enc_mv_cfg_t;

typedef struct {
    IN  UINT16 StreamIdx;
    IN  UINT16 CatIdx;
    IN  UINT32 OptVal;
} dsp_video_enc_desc_fmt_cfg_t;

/******************  StillEnc  ******************/

typedef struct {
    IN  UINT8  CapDataType;             /* Data type to be captured */
    IN  UINT8  AuxDataNeeded;           /* 1: HDS data when TYPE_RAW */
    IN  UINT16 Index;                   /* Indicate VinIdx when TYPE_RAW,
                                         * YuvStrmId when TYPE_YUV
                                         * ViewZoneId when TYPE_COMP_RAW
                                         * EncStrmId when TYPE_SYNCED_YUV/TYPE_VIDEO_YUV */

    IN  UINT16 AllocType;               /* MUST be ALLOC_EXTERNAL_DISTINCT and ALLOC_EXTERNAL_CYCLIC */
    IN  UINT16 BufNum;
    IN  UINT8  OverFlowCtrl;            /* 0 : Wait for Append, 1: Rounding */
    IN  UINT8  CmprRate;                /* compression rate, refer to IK_RAW_COMPRESS_XX.
                                           Only valid when CapDataTyep=TYPE_RAW,
                                           when Liveview is running, CmprRate must be then same as Liveview's setting */
    IN  UINT8  CmptRate;                /* compact rate, refer to IK_RAW_COMPACT_XX - 255U.
                                           Only valid when CapDataTyep=TYPE_RAW,
                                           when Liveview is running, CmprRate must be then same as Liveview's setting */
    IN  UINT8  HdrExposureNum;          /* B[2:0] Exposure numbr
                                           B[7:6] Type
                                                  [0] DOL (DONT CARE NOW)
                                                  [1] VC Fixed
                                         */

    IN  AMBA_DSP_BUF_s DataBuf;         /* BaseAddr valid when ALLOC_EXTERNAL_CYCLIC */
    IN  AMBA_DSP_BUF_s AuxDataBuf;      /* HDS data when RawInput, BaseAddr valid when ALLOC_EXTERNAL_CYCLIC
                                           ME data when TYPE_VIDEO_YUV, BaseAddr valid when ALLOC_EXTERNAL_CYCLIC **/
    IN  ULONG  BufTbl[MAX_EXT_DISTINCT_DEPTH];      /* valid when ALLOC_EXTERNAL_DISTINCT */ //8*64 = 512B
    IN  ULONG  AuxBufTbl[MAX_EXT_DISTINCT_DEPTH];   /* valid when ALLOC_EXTERNAL_DISTINCT */ //8*64 = 512B
} dsp_stl_data_cap_cfg_in_t;

typedef struct {
    IN  UINT16 CapInstance;
    IN  dsp_stl_data_cap_cfg_in_t DataCapCfg;
} dsp_stl_data_cap_cfg_t;

typedef struct {
    IN  UINT16 BufNum;
    IN  UINT16 AllocType;               /* MUST be ALLOC_EXTERNAL_DISTINCT and ALLOC_EXTERNAL_CYCLIC, same as CFG one */
    IN  UINT32 Rsvd;

    IN  ULONG  BufAddr;                 /* BaseAddr valid when ALLOC_EXTERNAL_CYCLIC */
    IN  ULONG  AuxBufAddr;              /* BaseAddr valid when ALLOC_EXTERNAL_CYCLIC */
    IN  ULONG  BufTbl[MAX_EXT_DISTINCT_DEPTH];      /* valid when ALLOC_EXTERNAL_DISTINCT */ //8*64 = 512B
    IN  ULONG  AuxBufTbl[MAX_EXT_DISTINCT_DEPTH];   /* valid when ALLOC_EXTERNAL_DISTINCT */ //8*64 = 512B
} dsp_stl_data_cap_buf_cfg_t;

typedef struct {
    IN  UINT16 CapInstance;
    IN  dsp_stl_data_cap_buf_cfg_t CapBuf;
    OUT UINT64 AttachedRawSeq;
} dsp_stl_update_cap_buf_t;

typedef struct {
    IN  UINT16 NumCapInstance;
    IN  UINT16 CapInstance[AMBA_DSP_MAX_DATACAP_NUM];
    IN  AMBA_DSP_DATACAP_CTRL_s DataCapCtrl[AMBA_DSP_MAX_DATACAP_NUM];
    OUT UINT64 AttachedRawSeq;
} dsp_stl_data_cap_ctrl_t;

typedef struct {
    IN  AMBA_DSP_YUV_IMG_BUF_s YuvIn;
    IN  AMBA_DSP_YUV_IMG_BUF_s YuvOut;
    IN  AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    IN  UINT32 Opt;
    OUT UINT64 AttachedRawSeq;
} dsp_stl_y2y_t;

typedef struct {
    IN  UINT16 StreamIdx;
    IN  AMBA_DSP_STLENC_CTRL_s StlEncCtrl;
    OUT UINT64 AttachedRawSeq;
} dsp_stl_enc_ctrl_t;

typedef struct {
    IN  UINT16 StreamIdx;
    IN  UINT16 BufType;
    OUT UINT16 BufPitch;
    OUT UINT32 BufUnitSize;
} dsp_stl_yuv_extbuf_clac_t;

typedef struct {
    IN  AMBA_DSP_RAW_BUF_s RawIn;
    IN  AMBA_DSP_BUF_s AuxBufIn;
    IN  AMBA_DSP_YUV_IMG_BUF_s YuvOut;
    IN  AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    IN  UINT32 Opt;
    OUT UINT64 AttachedRawSeq;
} dsp_stl_r2y_t;

typedef struct {
    IN  AMBA_DSP_RAW_BUF_s RawIn;
    IN  AMBA_DSP_BUF_s AuxBufIn;
    IN  AMBA_DSP_RAW_BUF_s RawOut;
    IN  AMBA_DSP_ISOCFG_CTRL_s IsoCfg;
    IN  UINT32 Opt;
    OUT UINT64 AttachedRawSeq;
} dsp_stl_r2r_t;

/******************  Decode    ******************/
typedef struct {
    IN  UINT16 MaxNumStream;
    IN  AMBA_DSP_VIDDEC_STREAM_CONFIG_s StrmCfg[AMBA_DSP_MAX_DEC_STREAM_NUM];
} dsp_video_dec_cfg_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_DEC_STREAM_NUM];
    IN  AMBA_DSP_VIDDEC_START_CONFIG_s StartCfg[AMBA_DSP_MAX_DEC_STREAM_NUM];
} dsp_video_dec_start_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_DEC_STREAM_NUM];
    IN  UINT8  ShowLastFrame[AMBA_DSP_MAX_DEC_STREAM_NUM];
} dsp_video_dec_stop_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_DEC_STREAM_NUM];
    IN  AMBA_DSP_VIDDEC_TRICKPLAY_s TrickPlay[AMBA_DSP_MAX_DEC_STREAM_NUM];
} dsp_video_dec_trickplay_t;

typedef struct {
    IN  UINT16 NumStream;
    IN  UINT16 StreamIdx[AMBA_DSP_MAX_DEC_STREAM_NUM];
    IN  AMBA_DSP_VIDDEC_BITS_FIFO_s BitsFifo[AMBA_DSP_MAX_DEC_STREAM_NUM];
} dsp_video_dec_bits_update_t;

typedef struct {
    IN  UINT16 StreamIdx;
    IN  UINT16 NumPostCtrl;
    IN  AMBA_DSP_VIDDEC_POST_CTRL_s PostCtrl[NUM_VOUT_IDX];
} dsp_video_dec_post_ctrl_t;

typedef struct {
    IN  UINT16 StreamIdx;
    IN  AMBA_DSP_STLDEC_START_CONFIG_s DecConfig;
} dsp_still_dec_start_t;

typedef struct {
    IN  AMBA_DSP_YUV_IMG_BUF_s SrcYuvBufAddr;
    IN  AMBA_DSP_YUV_IMG_BUF_s DestYuvBufAddr;
    IN  AMBA_DSP_STLDEC_YUV2YUV_s Operation;
} dsp_still_dec_y2y_t;

typedef struct {
    IN  AMBA_DSP_YUV_IMG_BUF_s Src1YuvBufAddr;
    IN  AMBA_DSP_YUV_IMG_BUF_s Src2YuvBufAddr;
    IN  AMBA_DSP_YUV_IMG_BUF_s DestYuvBufAddr;
    IN  AMBA_DSP_STLDEC_BLEND_s Operation;
} dsp_still_dec_blend_t;

typedef struct {
    IN  UINT8  VoutIdx;
    IN  AMBA_DSP_YUV_IMG_BUF_s YuvBufAddr;
    IN  AMBA_DSP_VOUT_VIDEO_CFG_s VoutConfig;
} dsp_still_dec_disp_yuv_t;

#ifdef CONFIG_ENABLE_DSP_MONITOR
typedef struct {
    IN  UINT32  ModuleId;
    OUT  AMBA_DSP_MONITOR_CONFIG_s  MonitorCfg;
} dsp_mon_get_cfg_t;

typedef struct {
    IN  AMBA_DSP_MONITOR_CRC_CMPR_s  CrcCmpr;
} dsp_mon_crc_cmpr_t;

typedef struct {
    IN  AMBA_DSP_MONITOR_ERR_NOTIFY_s  ErrNtfy;
} dsp_mon_err_notify_t;

typedef struct {
    IN  AMBA_DSP_MONITOR_HEARTBEAT_s  HearBeatCfg;
} dsp_mon_heartbeat_cfg_t;
#endif

/*
 * IO-CMD definition B[31:30]Direction B[29:16]Size B[15:8]Group B[7:0]MagicNumber
 * In QNX, use _IO/_IOR/_IOW/_IOWR/, so we dont need to fill Direction field
 *         _IO  (Group, MagicNumber)
 *         _IOR (Group, MagicNumber, Size)
 *         _IOW (Group, MagicNumber, Size)
 *         _IOWR(Group, MagicNumber, Size)
 */

/******************   Ultility  ******************/
#define AMBADSP_GET_VERSION                 _IOR  ('s', 0x00U, AMBA_DSP_VERSION_INFO_s)
#define AMBADSP_GET_DSP_BIN_ADDR            _IOR  ('s', 0x01U, AMBA_DSP_BIN_ADDR_s)
#define AMBADSP_DEBUG_DUMP                  _IOW  ('s', 0x03U, dsp_debug_dump_t)
#define AMBADSP_CALC_HIER_BUF               _IOWR ('s', 0x04U, dsp_hier_buf_calc_t)
#define AMBADSP_CALC_RAW_PITCH              _IOWR ('s', 0x05U, dsp_raw_pitch_calc_t)
#define AMBADSP_CALC_ENC_MV_BUF             _IOWR ('s', 0x06U, dsp_enc_mv_buf_calc_t)
#define AMBADSP_PARSE_ENC_MV_BUF            _IOW  ('s', 0x07U, dsp_enc_mv_buf_parse_t)
#define AMBADSP_CALC_VP_MSG_BUF             _IOWR ('s', 0x08U, dsp_vp_msg_buf_t)
#define AMBADSP_GET_PROTECT_BUF             _IOR  ('s', 0x09U, AMBA_DSP_PROTECT_s)
#define AMBADSP_GET_BUF_INFO                _IOWR ('s', 0x0AU, dsp_buf_info_t)
#define AMBADSP_CACHE_BUF_OPERATE           _IOW  ('s', 0x0BU, dsp_cache_buf_op_t)
#if defined (CONFIG_SOC_CV2FS) || defined (CONFIG_SOC_CV22FS)
#define AMBADSP_TEST_FRAME_CFG              _IOW  ('s', 0x0CU, dsp_test_frame_config_t)
#define AMBADSP_TEST_FRAME_CTRL             _IOW  ('s', 0x0DU, dsp_test_frame_control_t)
#define AMBADSP_MAIN_SAFETY_CHECK           _IOWR ('s', 0x0EU, dsp_main_safety_check_t)
#define AMBADSP_MAIN_SAFETY_CONFIG          _IOW  ('s', 0x0FU, dsp_main_safety_config_t)
#endif
#if defined (CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52)
#define AMBADSP_SLICE_CFG_CALC              _IOWR ('s', 0x0EU, dsp_slice_cfg_t)
#endif
#define AMBADSP_GET_STATUS                  _IOWR ('s', 0x10U, dsp_status_t)

/******************   Event    ******************/
#define AMBADSP_EVENT_HDLR_CONFIG           _IOW  ('s', 0x20U, dsp_evnt_hdlr_cfg_t)
#define AMBADSP_EVENT_HDLR_RESET            _IOW  ('s', 0x21U, uint16_t)
#define AMBADSP_EVENT_HDLR_REGISTER         _IOW  ('s', 0x22U, dsp_evnt_hdlr_op_t)
#define AMBADSP_EVENT_HDLR_DEREGISTER       _IOW  ('s', 0x23U, dsp_evnt_hdlr_op_t)
#define AMBADSP_EVENT_GET_DATA              _IOR  ('s', 0x24U, dsp_evnt_data_t)
#define AMBADSP_EVENT_RELEASE_INFO_POOL     _IOW  ('s', 0x25U, uint16_t)

/******************   Main     ******************/
#define AMBADSP_GET_DEFAULT_SYSCFG          _IOR  ('s', 0x30U, AMBA_DSP_SYS_CONFIG_s)
#define AMBADSP_MAIN_INIT                   _IOW  ('s', 0x31U, AMBA_DSP_SYS_CONFIG_s)
#define AMBADSP_MAIN_SUSPEND                _IO   ('s', 0x32U)
#define AMBADSP_MAIN_RESUME                 _IO   ('s', 0x33U)
#define AMBADSP_SET_WORK_MEMORY             _IOW  ('s', 0x34U, dsp_mem_t)
#define AMBADSP_MAIN_MSG_PARSE              _IOW  ('s', 0x35U, uint32_t)
#define AMBADSP_MAIN_WAIT_VIN_INT           _IOWR ('s', 0x36U, dsp_wait_sig_t)
#define AMBADSP_MAIN_WAIT_VOUT_INT          _IOWR ('s', 0x37U, dsp_wait_sig_t)
#define AMBADSP_MAIN_WAIT_FLAG              _IOWR ('s', 0x38U, dsp_wait_sig_t)
#define AMBADSP_RESC_LIMIT_CONFIG           _IOW  ('s', 0x39U, dsp_resc_limit_cfg_t)
#define AMBADSP_PARLOAD_CONFIG              _IOWR ('s', 0x3AU, dsp_partial_load_cfg_t)
#define AMBADSP_PARLOAD_REGION_UNLOCK       _IOWR ('s', 0x3BU, dsp_partial_load_unlock_t)
#define AMBADSP_SYS_DRV_CFG                 _IOW  ('s', 0x3CU, dsp_sys_drv_cfg_t)
#define AMBADSP_SET_PROTECT_AREA            _IOW  ('s', 0x3DU, dsp_protect_area_t)
#define AMBADSP_EXT_RAW_BUF_TBL_CFG         _IOW  ('s', 0x3EU, dsp_ext_buf_tbl_t)

/******************    Vout    ******************/
#define AMBADSP_VOUT_RESET                  _IOW  ('s', 0x50U, uint8_t)
#define AMBADSP_VOUT_MIXER_CFG              _IOW  ('s', 0x51U, dsp_vout_mixer_cfg_t)
#define AMBADSP_VOUT_MIXER_BGC_CFG          _IOW  ('s', 0x52U, dsp_vout_mixer_bgc_cfg_t)
#define AMBADSP_VOUT_MIXER_HLC_CFG          _IOW  ('s', 0x53U, dsp_vout_mixer_hlc_cfg_t)
#define AMBADSP_VOUT_MIXER_CSC_CFG          _IOW  ('s', 0x54U, dsp_vout_mixer_csc_cfg_t)
#define AMBADSP_VOUT_MIXER_CSC_MATRIX_CFG   _IOW  ('s', 0x55U, dsp_vout_mixer_csc_matrix_cfg_t)
#define AMBADSP_VOUT_MIXER_CTRL             _IOW  ('s', 0x56U, uint8_t)
#define AMBADSP_VOUT_OSD_BUF_CFG            _IOW  ('s', 0x57U, dsp_vout_osd_buf_cfg_t)
#define AMBADSP_VOUT_OSD_CTRL               _IOW  ('s', 0x58U, dsp_vout_osd_ctrl_t)
#define AMBADSP_VOUT_DISP_CFG               _IOW  ('s', 0x59U, dsp_vout_disp_cfg_t)
#define AMBADSP_VOUT_DISP_CTRL              _IOW  ('s', 0x5AU, uint8_t)
#define AMBADSP_VOUT_DISP_GAMMA_CFG         _IOW  ('s', 0x5BU, dsp_vout_disp_gamma_cfg_t)
#define AMBADSP_VOUT_DISP_GAMMA_CTRL        _IOW  ('s', 0x5CU, dsp_vout_disp_gamma_ctrl_t)
#define AMBADSP_VOUT_VIDEO_CFG              _IOW  ('s', 0x5DU, dsp_vout_video_cfg_t)
#define AMBADSP_VOUT_VIDEO_CTRL             _IOW  ('s', 0x5EU, dsp_vout_video_ctrl_t)
#define AMBADSP_VOUT_MIXER_BIND_CFG         _IOW  ('s', 0x5FU, dsp_vout_mixer_bind_cfg_t)
#define AMBADSP_VOUT_DVE_CFG                _IOW  ('s', 0x60U, dsp_vout_dve_cfg_t)
#define AMBADSP_VOUT_DVE_CTRL               _IOW  ('s', 0x61U, uint8_t)
#define AMBADSP_VOUT_DRV_CFG                _IOW  ('s', 0x62U, dsp_vout_drv_cfg_t)

/******************  Liveview  ******************/
#define AMBADSP_LIVEVIEW_CFG                _IOW  ('s', 0x70U, dsp_liveview_cfg_t)
#define AMBADSP_LIVEVIEW_CTRL               _IOW  ('s', 0x71U, dsp_liveview_ctrl_t)
#define AMBADSP_LIVEVIEW_UPDATE_CFG         _IOW  ('s', 0x72U, dsp_liveview_update_cfg_t)
#define AMBADSP_LIVEVIEW_UPDATE_ISOCFG      _IOWR ('s', 0x73U, dsp_liveview_update_isocfg_t)
#define AMBADSP_LIVEVIEW_UPDATE_VINCFG      _IOW  ('s', 0x74U, dsp_liveview_update_vincfg_t)
#define AMBADSP_LIVEVIEW_UPDATE_PYMDCFG     _IOW  ('s', 0x75U, dsp_liveview_update_pymdcfg_t)
#define AMBADSP_LIVEVIEW_VIN_CAP_CFG        _IOW  ('s', 0x76U, dsp_liveview_vin_cap_cfg_t)
#define AMBADSP_LIVEVIEW_VIN_POST_CFG       _IOW  ('s', 0x77U, dsp_liveview_vin_post_cfg_t)
#define AMBADSP_LIVEVIEW_FEED_RAW           _IOW  ('s', 0x78U, dsp_liveview_feed_raw_data_t)
#define AMBADSP_LIVEVIEW_FEED_YUV           _IOW  ('s', 0x79U, dsp_liveview_feed_yuv_data_t)
#define AMBADSP_LIVEVIEW_YUVSTRM_SYNC       _IOWR ('s', 0x7AU, dsp_liveview_yuvstrm_sync_t)
#define AMBADSP_LIVEVIEW_UPDATE_GEOCFG      _IOWR ('s', 0x7BU, dsp_liveview_update_geocfg_t)
#define AMBADSP_LIVEVIEW_UPDATE_VZ_SRC      _IOWR ('s', 0x7CU, dsp_liveview_update_vz_src_t)
#define AMBADSP_LIVEVIEW_UPDATE_SIDEBAND    _IOW  ('s', 0x7DU, dsp_liveview_update_sideband_t)
#define AMBADSP_LIVEVIEW_SLICE_CFG          _IOW  ('s', 0x7EU, dsp_liveview_slice_cfg_t)
#define AMBADSP_LIVEVIEW_VZ_POSTPONE_CFG    _IOW  ('s', 0x7FU, dsp_liveview_vz_postpone_cfg_t)
#define AMBADSP_LIVEVIEW_UPDATE_VIN_STATE   _IOW  ('s', 0x80U, dsp_liveview_update_vin_state_t)
#define AMBADSP_LIVEVIEW_PARSE_VP_MSG       _IOW  ('s', 0x81U, ULONG)
#define AMBADSP_LIVEVIEW_VIN_DRV_CFG        _IOW  ('s', 0x82U, dsp_liveview_vin_drv_cfg_t)
#define AMBADSP_LIVEVIEW_IK_DRV_CFG         _IOW  ('s', 0x83U, dsp_liveview_ik_drv_cfg_t)
#define AMBADSP_LIVEVIEW_SLICE_CFG_CALC     _IOWR ('s', 0x84U, dsp_liveview_slice_cfg_t)
#define AMBADSP_LIVEVIEW_GET_IDSPCFG        _IOWR ('s', 0x85U, dsp_liveview_idsp_cfg_t)
#define AMBADSP_LIVEVIEW_SLOW_SHUTTER_CTRL  _IOW  ('s', 0x86U, dsp_liveview_slow_shutter_ctrl_t)
#define AMBADSP_LIVEVIEW_DROP_REPEAT_CTRL   _IOW  ('s', 0x87U, dsp_liveview_drop_repeat_ctrl_t)

/******************  VideoEnc  ******************/
#define AMBADSP_VIDEO_ENC_CFG               _IOW  ('s', 0x90U, dsp_video_enc_cfg_t)
#define AMBADSP_VIDEO_ENC_START             _IOW  ('s', 0x91U, dsp_video_enc_start_t)
#define AMBADSP_VIDEO_ENC_STOP              _IOW  ('s', 0x92U, dsp_video_enc_stop_t)
#define AMBADSP_VIDEO_ENC_FRMRATE_CTRL      _IOW  ('s', 0x93U, dsp_video_enc_frmrate_ctrl_t)
#define AMBADSP_VIDEO_ENC_BLEND_CTRL        _IOW  ('s', 0x94U, dsp_video_enc_blend_ctrl_t)
#define AMBADSP_VIDEO_ENC_QUALITY_CTRL      _IOW  ('s', 0x95U, dsp_video_enc_quality_ctrl_t)
#define AMBADSP_VIDEO_ENC_FEED_YUV          _IOW  ('s', 0x96U, dsp_video_enc_feed_yuv_data_t)
#define AMBADSP_VIDEO_ENC_GRP_CFG           _IOW  ('s', 0x97U, AMBA_DSP_VIDEO_ENC_GROUP_s)
#define AMBADSP_VIDEO_ENC_MV_CFG            _IOW  ('s', 0x98U, dsp_video_enc_mv_cfg_t)
#define AMBADSP_VIDEO_ENC_DESC_FMT_CFG      _IOW  ('s', 0x99U, dsp_video_enc_desc_fmt_cfg_t)

/******************  StillEnc  ******************/
#define AMBADSP_STL_DATA_CAP_CFG            _IOW  ('s', 0xB0U, dsp_stl_data_cap_cfg_t)
#define AMBADSP_STL_UPDATE_CAP_BUF          _IOW  ('s', 0xB1U, dsp_stl_update_cap_buf_t)
#define AMBADSP_STL_DATA_CAP_CTRL           _IOW  ('s', 0xB2U, dsp_stl_data_cap_ctrl_t)
#define AMBADSP_STL_Y2Y                     _IOW  ('s', 0xB3U, dsp_stl_y2y_t)
#define AMBADSP_STL_ENC_CTRL                _IOW  ('s', 0xB4U, dsp_stl_enc_ctrl_t)
#define AMBADSP_STL_YUV_EXTBUF_CACL         _IOWR ('s', 0xB5U, dsp_stl_yuv_extbuf_clac_t)
#define AMBADSP_STL_R2Y                     _IOW  ('s', 0xB6U, dsp_stl_r2y_t)
#define AMBADSP_STL_R2R                     _IOW  ('s', 0xB7U, dsp_stl_r2r_t)

/******************  Decode    ******************/
#define AMBADSP_VIDEO_DEC_CFG               _IOW  ('s', 0xC0U, dsp_video_dec_cfg_t)
#define AMBADSP_VIDEO_DEC_START             _IOW  ('s', 0xC1U, dsp_video_dec_start_t)
#define AMBADSP_VIDEO_DEC_STOP              _IOW  ('s', 0xC2U, dsp_video_dec_stop_t)
#define AMBADSP_VIDEO_DEC_TRICKPLAY         _IOW  ('s', 0xC3U, dsp_video_dec_trickplay_t)
#define AMBADSP_VIDEO_DEC_BITS_UPDATE       _IOW  ('s', 0xC4U, dsp_video_dec_bits_update_t)
#define AMBADSP_VIDEO_DEC_POST_CTRL         _IOW  ('s', 0xC5U, dsp_video_dec_post_ctrl_t)
#define AMBADSP_STILL_DEC_START             _IOW  ('s', 0xC6U, dsp_still_dec_start_t)
#define AMBADSP_STILL_DEC_STOP              _IO   ('s', 0xC7U)
#define AMBADSP_STILL_DEC_Y2Y               _IOW  ('s', 0xC8U, dsp_still_dec_y2y_t)
#define AMBADSP_STILL_DEC_BLEND             _IOW  ('s', 0xC9U, dsp_still_dec_blend_t)
#define AMBADSP_STILL_DEC_DISP_YUV          _IOW  ('s', 0xCAU, dsp_still_dec_disp_yuv_t)

/******************  Diag    ********************/
#define AMBADSP_DIAG_CASE                   _IOW  ('s', 0xD0U, UINT32)

#ifdef CONFIG_ENABLE_DSP_MONITOR
/******************  Monitor    ******************/
#define AmbaDSP_MONITOR_INIT                _IO   ('s', 0xD1U)
#define AmbaDSP_MONITOR_GET_CFG             _IOWR ('s', 0xD2U, dsp_mon_get_cfg_t)
#define AmbaDSP_MONITOR_CRC_CMPR            _IOW  ('s', 0xD3U, dsp_mon_crc_cmpr_t)
#define AmbaDSP_MONITOR_ERR_NOTIFY          _IOW  ('s', 0xD4U, dsp_mon_err_notify_t)
#define AmbaDSP_MONITOR_HEARTBEAT_CFG       _IOW  ('s', 0xD5U, dsp_mon_heartbeat_cfg_t)

#endif

#define AMBADSP_IOCTL_CMD_MAX_NUM           0xE0

uint32_t dsp_ioctl_impl(const void *f, uint32_t cmd, void *arg);

#endif

