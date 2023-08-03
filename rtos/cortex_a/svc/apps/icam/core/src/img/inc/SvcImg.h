/**
*  @file SvcImg.h
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
*  @details Constants and Definitions for SVC Image
*
*/

#ifndef SVC_IMG_H
#define SVC_IMG_H

#ifndef AMBA_DSP_CAPABILITY_H
#include "AmbaDSP_Capability.h"
#endif

#ifndef AMBA_IK_IMAGE_FILTER_H
#include "AmbaDSP_ImageFilter.h"
#endif

#define SVC_IMG_MEM_ADDR    ULONG

typedef enum /*_SVC_IMG_CMD_e_*/ {
      SVC_LIVEVIEW_CMD_START = 0,
      SVC_LIVEVIEW_CMD_STOP,
    SVC_STATISTICS_CMD_START,
    SVC_STATISTICS_CMD_MASK,
    SVC_STATISTICS_CMD_INTER,
    SVC_STATISTICS_CMD_STOP,
          SVC_SYNC_CMD_START,
          SVC_SYNC_CMD_ENABLE,
          SVC_SYNC_CMD_STOP,
      SVC_LIVEVIEW_CMD_LOCK,
      SVC_LIVEVIEW_CMD_UNLOCK,
       SVC_CAPTURE_CMD_START,
       SVC_CAPTURE_CMD_STOP,
           SVC_VIN_CMD_CAP,
           SVC_VIN_CMD_SENSOR,
          SVC_PIPE_CMD_MODE,
          SVC_PIPE_CMD_IN,
          SVC_PIPE_CMD_OUT,
          SVC_PIPE_CMD_HDR,
         SVC_IMAGE_CMD_ALGO,
         SVC_IMAGE_CMD_CHAN,
           SVC_AVM_CMD_START,
           SVC_AVM_CMD_STOP,
           SVC_FOV_CMD_START,
           SVC_FOV_CMD_STOP,
           SVC_EIS_CMD_START,
           SVC_EIS_CMD_STOP,
       SVC_CAPTURE_CMD_AAA,
       SVC_CAPTURE_CMD_ISO,
       SVC_CAPTURE_CMD_EXIF,
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
       SVC_CAPTURE_CMD_PIV,
#endif
#ifdef CONFIG_BUILD_IMGFRW_SMC
           SVC_SMC_CMD_ON,
           SVC_SMC_CMD_OFF,
#endif
          SVC_EFOV_CMD_TXCB,
          SVC_EFOV_CMD_RX,
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
          SVC_IMAGE_CMD_ISO_CB,
#endif
           SVC_IMG_CMD_TOTAL
} SVC_IMG_CMD_e;

typedef enum /*_SVC_IMG_MSG_CHAN_e_*/ {
    SVC_CHAN_MSG_SELECT = 0,
    SVC_CHAN_MSG_SENSOR,
    SVC_CHAN_MSG_ALGO,
    SVC_CHAN_MSG_ZONE,
    SVC_CHAN_MSG_ZONE_MSB,
    SVC_CHAN_MSG_VR,
    SVC_CHAN_MSG_IQ,
    SVC_CHAN_MSG_INTER,
    SVC_CHAN_MSG_AF,
    SVC_CHAN_MSG_AVM,
    SVC_CHAN_MSG_AVM_IQ,
    SVC_CHAN_MSG_FOV,
    SVC_CHAN_MSG_FOV_IQ,
    SVC_CHAN_MSG_POST_ZONE,
    SVC_CHAN_MSG_POST_ZONE_ADD,
    SVC_CHAN_MSG_EIS,
    SVC_CHAN_MSG_IMG_MODE,
    SVC_CHAN_MSG_SNAP_AEB,
    SVC_CHAN_MSG_EFOV,
    SVC_CHAN_MSG_AAA_CORE,
    SVC_CHAN_MSG_AVM_CORE,
    SVC_CHAN_MSG_EIS_CORE,
    SVC_CHAN_MSG_SLOW_SHUTTER,
#ifdef CONFIG_BUILD_IMGFRW_SMC
    SVC_CHAN_MSG_SMC,
#endif
    SVC_CHAN_MSG_VIN_TONE_CURVE,
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
    SVC_CHAN_MSG_FOV_WARP,
#endif
    SVC_CHAN_MSG_R2Y_USER_DELAY,
#ifdef CONFIG_BUILD_IMGFRW_EIS_GYRODLY
    SVC_CHAN_MSG_GYRO_USER_DELAY,
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
    SVC_CHAN_MSG_ISO_DIS,
    SVC_CHAN_MSG_ISO_DIS_ADD,
#endif
    SVC_CHAN_MSG_TOTAL
} SVC_IMG_MSG_CHAN_e;

typedef enum /*_SVC_IMG_MSG_ALGO_e_*/ {
    SVC_ALGO_MSG_IQ_PRELOAD = 0,
    SVC_ALGO_MSG_IQ_INIT,
    SVC_ALGO_MSG_DEFOG,
    SVC_ALGO_MSG_INFO_GET,
    SVC_ALGO_MSG_AVM_MODE,
    SVC_ALGO_MSG_AVM_ROI,
    SVC_ALGO_MSG_AE_STABLE_WAIT
} SVC_IMG_MSG_ALGO_e;

typedef enum /*_SVC_IMG_VIDEO_OP_e_*/ {
    SVC_VIDEO_OP_VIDEO = 0,
    SVC_VIDEO_OP_PHOTO_PREVIEW,
    SVC_VIDEO_OP_RAWENC
} SVC_IMG_VIDEO_OP_e;

typedef enum /*_SVC_IMG_STILL_OP_e_*/ {
    SVC_STILL_OP_CAPTURE = 0xF
} SVC_IMG_STILL_OP_e;

typedef enum /*_SVC_IMG_IQ_LOAD_e_*/ {
    SVC_ALGO_IQ_LOAD_ALL = 0,
    SVC_ALGO_IQ_LOAD_VIDEO,
    SVC_ALGO_IQ_LOAD_STILL
} SVC_IMG_IQ_LAOD_e;

typedef enum /*_SVC_IMG_PIPE_MODE_e_*/ {
    SVC_IMAGE_PIPE_NORMAL = 0,
    SVC_IMAGE_PIPE_DRAM_EFCY
} SVC_IMG_PIPE_MODE_e;

typedef union /*_SVC_IMG_CMD_MSG_u_*/ {
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
} SVC_IMG_CMD_MSG_u;

typedef struct /*_SVC_IMG_CMD_MSG_s_*/ {
    SVC_IMG_CMD_MSG_u    Ctx;
} SVC_IMG_CMD_MSG_s;

typedef union /*_SVC_IMG_AUX_DATA_u_*/ {
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
} SVC_IMG_AUX_DATA_u;

typedef struct /*_SVC_IMG_AUX_DATA_s_*/ {
    SVC_IMG_AUX_DATA_u    Ctx;
} SVC_IMG_AUX_DATA_s;

#define SVC_NUM_VIN_SENSOR        4U
#define SVC_NUM_FOV_CHANNEL       32U

typedef struct /*_SVC_IMG_TIME_DIVISION_s_*/ {
    UINT32 Num;
    UINT32 FrameGroupCnt;
    UINT32 FrameSelectBits[AMBA_DSP_MAX_VIN_TD_NUM];
    UINT32 AaaIqTable[SVC_NUM_VIN_SENSOR][AMBA_DSP_MAX_VIN_TD_NUM];
} SVC_IMG_TIME_DIVISION_s;

typedef struct /*_SVC_IMG_FOV_ADJ_s_*/ {
    UINT32 GroupId;
    UINT32 IqTable;
} SVC_IMG_FOV_ADJ_s;

typedef struct /*_SVC_IMG_FOV_CFG_s_*/ {
    UINT32 LinearCE;
} SVC_IMG_FOV_CFG_s;

typedef struct /*_SVC_IMG_CFG_s_*/ {
    UINT32                     VinSelectBits;
    UINT32                     VinSensorId[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
    UINT32                     SensorSelectBits[AMBA_NUM_VIN_CHANNEL];
    UINT32                     FovSelectBitsInSensor[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
    UINT32                     SensorAaaGroup[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
    UINT32                     AaaIqTable[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
    SVC_IMG_TIME_DIVISION_s    TimeDivision[AMBA_NUM_VIN_CHANNEL];
    SVC_IMG_FOV_ADJ_s          FovAdj[SVC_NUM_FOV_CHANNEL];
    SVC_IMG_FOV_CFG_s          FovCfg[SVC_NUM_FOV_CHANNEL];
    UINT64                     SensorEFov[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
#ifdef CONFIG_BUILD_IMGFRW_SMC
    UINT32                     SmcI2sId[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
#endif
} SVC_IMG_CFG_s;

typedef union /*_SVC_IMG_ALGO_s_*/ {
    UINT32 Data;
    struct {
        UINT32 Id:           6;
        UINT32 IqTable:      8;
        UINT32 HdrId:        8;
        UINT32 Reserved:     9;
        UINT32 Valid:        1;
    } Bits;
} SVC_IMG_ALGO_u;

typedef struct /*_SVC_IMG_ALGO_s_*/ {
    SVC_IMG_ALGO_u    Ctx[AMBA_DSP_MAX_VIN_TD_NUM];
} SVC_IMG_ALGO_s;

typedef struct /*_SVC_IMG_INFO_s_*/ {
    SVC_IMG_CFG_s     Cfg;
    SVC_IMG_ALGO_s    Algo[AMBA_NUM_VIN_CHANNEL][SVC_NUM_VIN_SENSOR];
} SVC_IMG_INFO_s;

typedef struct /*_SVC_IMG_ALGO_AWB_INFO_s_*/ {
    UINT32 Wgc[3];
} SVC_IMG_ALGO_AWB_INFO_s;

typedef struct /*_SVC_IMG_ALGO_AE_INFO_s_*/ {
    FLOAT ExposureTime;
    FLOAT Gain;
} SVC_IMG_ALGO_AE_INFO_s;

typedef struct /*_SVC_IMG_ALGO_FOV_WB_s_*/ {
    AMBA_IK_FE_WB_GAIN_s    Fe[3];
    AMBA_IK_WB_GAIN_s       Be;
    AMBA_IK_WB_GAIN_s       BeAlt;
} SVC_IMG_ALGO_FOV_WB_s;

typedef struct /*_SVC_IMG_ALGO_FOV_PIPE_s_*/ {
    SVC_IMG_ALGO_FOV_WB_s    Wb;
} SVC_IMG_ALGO_FOV_PIPE_s;

typedef struct /*_SVC_IMG_ALGO_FOV_INFO_s_*/ {
    UINT32                     SelectBits;
    SVC_IMG_ALGO_FOV_PIPE_s    *pPipe[AMBA_DSP_MAX_VIEWZONE_NUM];
} SVC_IMG_ALGO_FOV_INFO_s;

typedef struct /*_SVC_IMG_ALGO_CTX_s_*/ {
    UINT32 Id;
    void   *pAeInfo;
    void   *pAaaInfo;
} SVC_IMG_ALGO_CTX_s;

typedef struct /*_SVC_IMG_ALGO_INFO_s_*/ {
    SVC_IMG_ALGO_FOV_INFO_s    Fov;
    UINT32                     ExposureNum;
    SVC_IMG_ALGO_AE_INFO_s     Ae[4];
    SVC_IMG_ALGO_AWB_INFO_s    Awb;
    SVC_IMG_ALGO_CTX_s         Ctx;
} SVC_IMG_ALGO_INFO_s;

typedef struct /*_SVC_IMG_ALGO_MSG_s_*/ {
    UINT32                 RetCode;
    UINT32                 RawCapSeq;
    SVC_IMG_ALGO_INFO_s    Info;
} SVC_IMG_ALGO_MSG_s;

typedef struct /*_SVC_IMG_CMD_MSG_RET_s_*/ {
    UINT32 RetCode;
    void   *pData;
} SVC_IMG_CMD_MSG_RET_s;

#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
typedef struct /*_SVC_IMG_ISO_INFO_s_*/ {
    UINT32 RawCapSeq;
    UINT32 AikCnt;
    UINT32 VinSkipFrame;
    UINT32 ZoneId;
    UINT32 ContextId;
    UINT32 IkId;
    void   *pIsoCfg;
    UINT64 UserData;
} SVC_IMG_ISO_INFO_s;

typedef UINT32 (*SVC_IMG_ISO_CB_f)(const void *pData);
#endif

#ifdef CONFIG_BUILD_IMGFRW_EFOV
typedef UINT32 (*SVC_IMG_EFOV_TX_CB_f)(UINT64 TxData, UINT32 Size);
  void SvcImg_EFovTxHook(SVC_IMG_EFOV_TX_CB_f pFunc);
  void SvcImg_EFovRxPut(UINT64 RxData, UINT32 Size);
#endif
#ifdef CONFIG_BUILD_IMGFRW_ISO_DIS
  void SvcImg_IsoDisCbHook(SVC_IMG_ISO_CB_f pFunc);
#endif

UINT32 SvcImg_IqMemSizeQuery(UINT32 MaxAlgoNum);
  void SvcImg_IqMemInit(UINT32 MaxAlgoNum, void *pMem, UINT32 MemSizeInByte);
UINT32 SvcImg_AlgoMemSizeQuery(UINT32 MaxAlgoNum, UINT32 MaxExposureNum);
  void SvcImg_AlgoMemInit(void *pMem, UINT32 MemSizeInByte);
UINT32 SvcImg_MemSizeQuery(void);
  void SvcImg_MemInit(void *pMem, UINT32 MemSizeInByte);

  void SvcImg_Init(UINT32 MaxAlgoNum, UINT32 MaxExposureNum);
UINT32 SvcImg_AlgoIdPreGet(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId);
UINT32 SvcImg_AlgoIdPreGetEx(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId, UINT32 *pAlgoNum);
  void SvcImg_Config(const SVC_IMG_CFG_s *pImgCfg);
  void SvcImg_ConfigEx(const SVC_IMG_CFG_s *pImgCfg, UINT32 IqSecId);
  void SvcImg_Stop(void);
UINT32 SvcImg_AlgoIdGet(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId);
UINT32 SvcImg_AlgoIdGetEx(UINT32 VinId, UINT32 SensorId, UINT32 *pAlgoId, UINT32 *pAlgoNum);
  void SvcImg_CmdInstall(void);
  void SvcImg_AlgoIqPreload(void);
  void SvcImg_AlgoIqPreloadEx(UINT32 SecId);
  void SvcImg_AlgoIqInitEx(UINT32 SecId);
  void SvcImg_AlgoDefog(UINT32 VinId, UINT32 SensorId, UINT32 Enable);
  void SvcImg_AlgoInfoGet(UINT32 VinId, UINT32 RawCapSeq, SVC_IMG_ALGO_INFO_s *pAlgoInfo, UINT32 *pAlgoNum);
  void SvcImg_StatisticsMask(UINT32 Mask);
  void SvcImg_SyncEnable(UINT32 VinId, UINT32 Enable);

  void SvcImg_AaaStart(UINT32 VinId);
  void SvcImg_AaaStop(UINT32 VinId);

UINT32 SvcImg_AvmAlgoIdGet(UINT32 *pAlgoId);
  void SvcImg_AvmAlgoMode(UINT32 Mode);
  void SvcImg_AvmRoiRgbPut(void *pData);

  void SvcImg_Aeb(UINT32 VinId, UINT32 SensorId, UINT32 AebCnt, UINT32 AebNum, UINT32 AebDen);

  void SvcImg_Lock(UINT32 VinId);
  void SvcImg_Unlock(UINT32 VinId);

  void SvcImg_StillStart(UINT32 VinId);
  void SvcImg_StillAaa(UINT32 VinId, UINT32 SensorId, UINT32 SnapCount, UINT32 *pIsHiso);
  void SvcImg_StillIso(UINT32 VinId, UINT32 SensorId, void *pCfa, UINT32 IsPiv, UINT32 IsHiso, void *pImgMode);
  void SvcImg_StillStop(UINT32 VinId);
  void SvcImg_StillExif(UINT32 VinId, UINT32 SensorId, void **pExifInfo);
#ifdef CONFIG_BUILD_IMGFRW_RAW_PIV
  void SvcImg_StillPiv(UINT32 VinId, UINT32 SensorId, UINT32 RawCapSeq, UINT32 *pIsHiso);
#endif
#ifdef CONFIG_BUILD_IMGFRW_SMC
  void SvcImg_SmcOn(UINT32 VinId);
  void SvcImg_SmcOff(UINT32 VinId);
#endif
UINT32 SvcImg_AeStableWait(UINT32 VinId, UINT32 SensorId, UINT32 Timeout);
#ifdef CONFIG_BUILD_IMGFRW_EIS_WARP_AC
  void SvcImg_LivWarpInfo(UINT32 FovId, AMBA_IK_WARP_INFO_s *pWarpInfo);
#endif
#if 0
  void SvcImg_MemV2P(void *pParam1, const SVC_IMG_MEM_ADDR Param2);
  void SvcImg_MemP2P(void *pParam1, const void *pParam2);
  void SvcImg_MemP2V(void *pParam1, const void *pParam2);
  void SvcImg_MemF2P(void *pParam1, const void *pParam2);
#endif
#endif  /* SVC_IMG_H */
