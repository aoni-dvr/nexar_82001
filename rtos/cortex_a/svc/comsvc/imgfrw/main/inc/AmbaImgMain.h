/**
 *  @file AmbaImgMain.h
 *
 *  Copyright (c) [2020] Ambarella International LP
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details Constants and Definitions for Amba Image Main
 *
 */

#ifndef AMBA_IMG_MAIN_H
#define AMBA_IMG_MAIN_H

#ifndef AMBA_DSP_CAPABILITY_H
#include "AmbaDSP_Capability.h"
#endif

#ifndef AMBA_IK_IMAGE_FILTER_H
#include "AmbaDSP_ImageFilter.h"
#endif

#ifndef AMBA_SHELL_H
#include "AmbaShell.h"
#endif

typedef enum /*_AMBA_IMG_MAIN_CMD_e_*/ {
      LIVEVIEW_CMD_START = 0,
      LIVEVIEW_CMD_STOP,
    STATISTICS_CMD_START,
    STATISTICS_CMD_MASK,
    STATISTICS_CMD_INTER,
    STATISTICS_CMD_STOP,
          SYNC_CMD_START,
          SYNC_CMD_ENABLE,
          SYNC_CMD_STOP,
      LIVEVIEW_CMD_LOCK,
      LIVEVIEW_CMD_UNLOCK,
       CAPTURE_CMD_START,
       CAPTURE_CMD_STOP,
           VIN_CMD_CAP,
           VIN_CMD_SENSOR,
          PIPE_CMD_MODE,
          PIPE_CMD_IN,
          PIPE_CMD_OUT,
          PIPE_CMD_HDR,
         IMAGE_CMD_ALGO,
         IMAGE_CMD_CHAN,
           AVM_CMD_START,
           AVM_CMD_STOP,
           FOV_CMD_START,
           FOV_CMD_STOP,
       IMG_EIS_CMD_START,
       IMG_EIS_CMD_STOP,
       CAPTURE_CMD_AAA,
       CAPTURE_CMD_ISO,
       CAPTURE_CMD_EXIF,
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
       CAPTURE_CMD_PIV,
#endif
#ifdef CONFIG_BUILD_IMGFRW_SMC
           SMC_CMD_ON,
           SMC_CMD_OFF,
#endif
      IMG_EFOV_CMD_TXCB,
      IMG_EFOV_CMD_RX,
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
         IMAGE_CMD_ISO_CB,
#endif
      IMG_MAIN_CMD_TOTAL
} AMBA_IMG_MAIN_CMD_e;

typedef enum /*_AMBA_IMG_MSG_CHAN_e_*/ {
    CHAN_MSG_SELECT = 0,
    CHAN_MSG_SENSOR,
    CHAN_MSG_ALGO,
    CHAN_MSG_ZONE,
    CHAN_MSG_ZONE_MSB,
    CHAN_MSG_VR,
    CHAN_MSG_IQ,
    CHAN_MSG_INTER,
    CHAN_MSG_AF,
    CHAN_MSG_AVM,
    CHAN_MSG_AVM_IQ,
    CHAN_MSG_FOV,
    CHAN_MSG_FOV_IQ,
    CHAN_MSG_POST_ZONE,
    CHAN_MSG_POST_ZONE_ADD,
    CHAN_MSG_EIS,
    CHAN_MSG_IMG_MODE,
    CHAN_MSG_SNAP_AEB,
    CHAN_MSG_EFOV,
    CHAN_MSG_AAA_CORE,
    CHAN_MSG_AVM_CORE,
    CHAN_MSG_EIS_CORE,
    CHAN_MSG_SLOW_SHUTTER,
#ifdef CONFIG_BUILD_IMGFRW_SMC
    CHAN_MSG_SMC,
#endif
    CHAN_MSG_VIN_TONE_CURVE,
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
    CHAN_MSG_FOV_WARP,
#endif
    CHAN_MSG_R2Y_USER_DELAY,
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
    CHAN_MSG_GYRO_USER_DELAY,
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    CHAN_MSG_ISO_DIS,
    CHAN_MSG_ISO_DIS_ADD,
#endif
    CHAN_MSG_TOTAL
} AMBA_IMG_MSG_CHAN_e;

typedef enum /*_AMBA_IMG_MSG_ALGO_e_*/ {
    ALGO_MSG_IQ_PRELOAD = 0,
    ALGO_MSG_IQ_INIT,
    ALGO_MSG_DEFOG,
    ALGO_MSG_INFO_GET,
    ALGO_MSG_AVM_MODE,
    ALGO_MSG_AVM_ROI,
    ALGO_MSG_AE_STABLE_WAIT
} AMBA_IMG_MSG_ALGO_e;

typedef enum /*_AMBA_IMG_VIDEO_OP_e_*/ {
    VIDEO_OP_VIDEO = 0,
    VIDEO_OP_PHOTO_PREVIEW,
    VIDEO_OP_RAWENC
} AMBA_IMG_VIDEO_OP_e;

typedef enum /*_AMBA_IMG_STILL_OP_e_*/ {
    STILL_OP_CAPTURE = 0xF
} AMBA_IMG_STILL_OP_e;

typedef enum /*_IMAGE_IQ_LOAD_e_*/ {
    ALGO_IQ_LOAD_ALL = 0,
    ALGO_IQ_LOAD_VIDEO,
    ALGO_IQ_LOAD_STILL
} IMAGE_IQ_LAOD_e;

typedef enum /*_AMBA_IMG_PIPE_MODE_e_*/ {
    IMAGE_PIPE_NORMAL = 0,
    IMAGE_PIPE_DRAM_EFCY
} AMBA_IMG_PIPE_MODE_e;

typedef union /*_AMBA_IMG_MAIN_CMD_MSG_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Var:                   32;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } Bits;
    struct {
        UINT32 Var:                   32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } Bits2;
    struct {
        UINT32 Var:                   32;
        UINT32 Reserved_H:            10;
        UINT32 AuxId:                  4;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } Bits3;
    struct {
        UINT32 Width:                 16;
        UINT32 Height:                16;
        UINT32 Reserved_H:            22;
        UINT32 ChainId:                2;
        UINT32 Cmd:                    8;
    } VinCap;
    struct {
        UINT32 Id:                    32;
        UINT32 Reserved_H:            20;
        UINT32 Idx:                    4;
        UINT32 Cmd:                    8;
    } VinSensor;
    struct {
        UINT32 Id:                     8;
        UINT32 Reserved_L:            24;
        UINT32 Reserved_H:            17;
        UINT32 ZoneIdx:                5;
        UINT32 ChainId:                2;
        UINT32 Cmd:                    8;
    } PipeMode;
    struct {
        UINT32 Width:                 16;
        UINT32 Height:                16;
        UINT32 Reserved_H:            17;
        UINT32 ZoneIdx:                5;
        UINT32 ChainId:                2;
        UINT32 Cmd:                    8;
    } PipeOut;
    struct {
        UINT32 Enable:                 2;
        UINT32 ExposureNum:            3;
        UINT32 LCeEnable:              1;
        UINT32 BackWgc:                1;
        UINT32 BackAltWgc:             1;
        UINT32 BackDgc:                1;
        UINT32 Reserved_L:            23;
        UINT32 Reserved_H:            22;
        UINT32 ChainId:                2;
        UINT32 Cmd:                    8;
    } PipeHdr;
    struct {
        UINT32 ChainIndex:             8;
        UINT32 Reserved_L:            24;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanSelect;
    struct {
        UINT32 Id:                     4;
        UINT32 Reserved_L:            28;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanSensor;
    struct {
        UINT32 Id:                     6;
        UINT32 Reserved_L:            26;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanAlgo;
    struct {
        UINT32 Id:                    32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanZone;
    struct {
        UINT32 Id:                     1;
        UINT32 Reserved_L:            31;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanZoneMsb;
    struct {
        UINT32 Id:                    32;
        UINT32 AltId:                  4;
        UINT32 Reserved_H:            10;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanVr;
    struct {
        UINT32 Id:                     8;
        UINT32 HdrId:                  8;
        UINT32 Reserved_L:            16;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanIq;
    struct {
        UINT32 Id:                    32;
        UINT32 Num:                    6;
        UINT32 Skip:                   3;
        UINT32 Mode:                   5;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanInter;
    struct {
        UINT32 Enable:                 1;
        UINT32 Reserved_L:            31;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanAf;
    struct {
        UINT32 ZoneId:                32;
        UINT32 AlgoId:                 6;
        UINT32 Reserved_H:             8;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanAvm;
    struct {
        UINT32 ZoneId:                32;
        UINT32 AlgoId:                 6;
        UINT32 AdjId:                  6;
        UINT32 Reserved_H:             2;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanFov;
    struct {
        UINT32 Id:                     8;
        UINT32 HdrId:                  8;
        UINT32 AdjId:                  6;
        UINT32 Reserved_L:            10;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanFovIq;
    struct {
        UINT32 Id:                    32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanPostZone;
    struct {
        UINT32 ZoneId:                32;
        UINT32 AlgoId:                 6;
        UINT32 Id:                     6;
        UINT32 Reserved_H:             2;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanEis;
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    struct {
        UINT32 DisId:                 32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanIso;
#endif
    struct {
        UINT32 Reserved_L:            32;
        UINT32 Type:                   8;
        UINT32 Reserved_H:             6;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanImgMode;
    struct {
        UINT32 Cnt:                    4;
        UINT32 Num:                    4;
        UINT32 Den:                    4;
        UINT32 Reserved_L:            20;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanSnapAeb;
    struct {
        UINT32 Var:                   32;
        UINT32 Id:                     6;
        UINT32 Mode:                   2;
        UINT32 Reserved_H:             6;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanEFov;
    struct {
        UINT32 Id:                    32;
        UINT32 Reserved_H:            10;
        UINT32 AuxId:                  4;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanCore;
    struct {
        UINT32 Enable:                 2;
        UINT32 Reserved_L:            30;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanSlowShutter;
#ifdef CONFIG_BUILD_IMGFRW_SMC
    struct {
        UINT32 I2sId:                 32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanSmc;
#endif
    struct {
        UINT32 Enable:                32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageChanVinToneCurve;
    struct {
        UINT32 Id:                     8;
        UINT32 HdrId:                  8;
        UINT32 AlgoId:                 8;
        UINT32 SecId:                  8;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageAlgoIqPreload;
    struct {
        UINT32 Id:                     8;
        UINT32 HdrId:                  8;
        UINT32 AlgoId:                 8;
        UINT32 SecId:                  8;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageAlgoIqInit;
    struct {
        UINT32 Enable:                 8;
        UINT32 SceneMode:              8;
        UINT32 AlgoId:                 8;
        UINT32 Reserved_L:             8;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageAlgoDefog;
    struct {
        UINT32 Reserved_L:            32;
        UINT32 Reserved_H:            10;
        UINT32 Id:                     6;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } ImageAlgoInfo;
    struct {
        UINT32 Mask:                  32;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } Stat;
    struct {
        UINT32 Enable:                 1;
        UINT32 Reserved_L:            31;
        UINT32 Reserved_H:            16;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } Sync;
    struct {
        UINT32 R2yBackground:          1;
        UINT32 Stitch:                 1;
        UINT32 DeferredBlackLevel:     1;
        UINT32 LowDelay:               1;
        UINT32 MaxSlowShutterIndex:    2;
        UINT32 ForceSlowShutter:       1;
        UINT32 IqSecId:                2;
        UINT32 Reserved_L:            19;
        UINT32 SkipFrame:              3;
        UINT32 SkipFrameEn:            1;
        UINT32 Reserved_H:            16;
        UINT32 OpMode:                 8;
        UINT32 Cmd:                    8;
    } Video;
    struct {
        UINT32 R2yBackground:          1;
        UINT32 Reserved_L:            31;
        UINT32 Reserved_H:            16;
        UINT32 OpMode:                 8;
        UINT32 Cmd:                    8;
    } Still;
    struct {
        UINT32 Reserved_L:            32;
        UINT32 SnapCount:             12;
        UINT32 Reserved_H:             2;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } StillAaa;
    struct {
        UINT32 Reserved_L:            32;
        UINT32 IsPiv:                  2;
        UINT32 IsHiso:                 1;
        UINT32 Reserved_H:            11;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } StillIso;
    struct {
        UINT32 Reserved_L:            32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } StillExif;
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
    struct {
        UINT32 RawCapSeq:             32;
        UINT32 Reserved_H:            14;
        UINT32 ChainId:                2;
        UINT32 Msg:                    8;
        UINT32 Cmd:                    8;
    } StillPiv;
#endif
} AMBA_IMG_MAIN_CMD_MSG_u;

typedef struct /*_AMBA_IMG_MAIN_CMD_MSG_s_*/ {
    AMBA_IMG_MAIN_CMD_MSG_u    Ctx;
} AMBA_IMG_MAIN_CMD_MSG_s;

typedef union /*_AMBA_IMG_MAIN_AUX_DATA_u_*/ {
    UINT64 Data;
    struct {
        UINT64 Var;
    } Bits;
    struct {
        UINT64 Data;
    } ImageChanImgMode;
    struct {
        UINT64 Var;
    } ImageAlgoInfo;
    struct {
        UINT64 pIsHiso;
    } StillAaa;
    struct {
        UINT64 pCfa;
    } StillIso;
    struct {
        UINT64 Var;
    } StillExif;
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
    struct {
        UINT64 pIsHiso;
    } StillPiv;
#endif
} AMBA_IMG_MAIN_AUX_DATA_u;

typedef struct /*_AMBA_IMG_MAIN_AUX_DATA_s_*/ {
    AMBA_IMG_MAIN_AUX_DATA_u    Ctx;
} AMBA_IMG_MAIN_AUX_DATA_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_AWB_INFO_s_*/ {
    UINT32 Wgc[3];
} AMBA_IMG_MAIN_ALGO_AWB_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_AE_INFO_s_*/ {
    FLOAT ExposureTime;
    FLOAT Gain;
} AMBA_IMG_MAIN_ALGO_AE_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_FOV_WB_s_*/ {
    AMBA_IK_FE_WB_GAIN_s    Fe[3];
    AMBA_IK_WB_GAIN_s       Be;
    AMBA_IK_WB_GAIN_s       BeAlt;
} AMBA_IMG_MAIN_ALGO_FOV_WB_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_FOV_PIPE_s_*/ {
    AMBA_IMG_MAIN_ALGO_FOV_WB_s    Wb;
} AMBA_IMG_MAIN_ALGO_FOV_PIPE_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_FOV_INFO_s_*/ {
    UINT32                           SelectBits;
    AMBA_IMG_MAIN_ALGO_FOV_PIPE_s    *pPipe[AMBA_DSP_MAX_VIEWZONE_NUM];
} AMBA_IMG_MAIN_ALGO_FOV_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_CTX_s_*/ {
    UINT32 Id;
    void   *pAeInfo;
    void   *pAaaInfo;
} AMBA_IMG_MAIN_ALGO_CTX_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_INFO_s_*/ {
    AMBA_IMG_MAIN_ALGO_FOV_INFO_s    Fov;
    UINT32                           ExposureNum;
    AMBA_IMG_MAIN_ALGO_AE_INFO_s     Ae[4];
    AMBA_IMG_MAIN_ALGO_AWB_INFO_s    Awb;
    AMBA_IMG_MAIN_ALGO_CTX_s         Ctx;
} AMBA_IMG_MAIN_ALGO_INFO_s;

typedef struct /*_AMBA_IMG_MAIN_ALGO_MSG_s_*/ {
    UINT32                       RetCode;
    UINT32                       RawCapSeq;
    AMBA_IMG_MAIN_ALGO_INFO_s    Info;
} AMBA_IMG_MAIN_ALGO_MSG_s;

typedef struct /*_AMBA_IMG_MAIN_CMD_MSG_RET_s_*/ {
    UINT32 RetCode;
    void   *pData;
} AMBA_IMG_MAIN_CMD_MSG_RET_s;

#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
typedef struct /*_AMBA_IMG_MAIN_ISO_INFO_s_*/ {
    UINT32 RawCapSeq;
    UINT32 AikCnt;
    UINT32 VinSkipFrame;
    UINT32 ZoneId;
    UINT32 ContextId;
    UINT32 IkId;
    void   *pIsoCfg;
    UINT64 UserData;
} AMBA_IMG_MAIN_ISO_INFO_s;

typedef UINT32 (*AMBA_IMG_MAIN_ISO_CB_f)(const void *pData);
#endif

/* api for app */
  void AmbaImgMain_SysInit(void);
  void AmbaImgMain_AlgoInit(UINT32 MaxAlgoNum, UINT32 MaxExposureNum);
  void AmbaImgMain_Config(UINT32 VinId, UINT64 Param);
  void AmbaImgMain_ConfigEx(UINT32 VinId, UINT64 Param, UINT64 Param2);

UINT32 AmbaImgMain_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum);
  void AmbaImgMain_AlgoMemInit(void *pMem, UINT32 MemSizeInByte);

UINT32 AmbaImgMain_MemSizeQuery(void);
  void AmbaImgMain_MemInit(void *pMem, UINT32 MemSizeInByte);

UINT32 AmbaImgMain_IqMemSizeQuery(UINT32 MaxAlgoNum);
  void AmbaImgMain_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte);

  void AmbaImgMain_ImgfrwCommand(UINT32 Argc, char * const *Argv, AMBA_SHELL_PRINT_f PrintFunc);
#if 0
#define AMBA_IMG_MAIN_API_COMPATIBLE
/* compatible, will be removed */
  void ImageMain_SysInit(void);
  void ImageMain_AlgoInit(UINT32 MaxAlgoNum, UINT32 MaxExposureNum);
  void ImageMain_Config(UINT32 VinId, UINT64 Param);
  void ImageMain_ConfigEx(UINT32 VinId, UINT64 Param, UINT64 Param2);

UINT32 ImageMain_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum);
  void ImageMain_AlgoMemInit(void *pMem, UINT32 MemSizeInByte);

UINT32 ImageMain_MemSizeQuery(void);
  void ImageMain_MemInit(void *pMem, UINT32 MemSizeInByte);

UINT32 ImageMain_IqMemSizeQuery(UINT32 MaxAlgoNum);
  void ImageMain_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte);

#define IMAGE_MAIN_CMD_MSG_s AMBA_IMG_MAIN_CMD_MSG_s

  void ImagePrint_Enable(UINT32 Flag);
#endif

/* timing debug only */
UINT32 AmbaImgMain_TimingMarkPut(UINT32 VinId, const char *pName);
UINT32 AmbaImgMain_TimingMarkPutByFov(UINT32 FovId, const char *pName);
UINT32 AmbaImgMain_TimingMarkPutByVout(UINT32 VoutId, const char *pName);

#endif  /* AMBA_IMG_MAIN_H */
