/**
 *  @file AmbaImgSensorHAL.h
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
 *  @details Constants and Definitions for Amba Image Sensor HAL
 *
 */

#ifndef AMBA_IMG_SENSOR_HAL_H
#define AMBA_IMG_SENSOR_HAL_H

typedef enum /*_AMBA_IMG_DGC_TYPE_e_*/ {
    IMAGE_DGC_SENSOR = 0,
    IMAGE_DGC_DSP,
    IMAGE_DGC_BOTH,
    IMAGE_DGC_TOTAL
} AMBA_IMG_DGC_TYPE_e;

typedef enum /*_AMBA_IMG_SHR_TYPE_e_*/ {
    IMAGE_SHR_SSR = 0,
    IMAGE_SHR_SSG,
    IMAGE_SHR_TOTAL
} AMBA_IMG_SHR_TYPE_e;

typedef enum /*_AMBA_IMG_SS_TYPE_e_*/ {
    IMAGE_SS_TYPE_0 = 0,
    IMAGE_SS_TYPE_1,
    IMAGE_SS_TYPE_TOTAL
} AMBA_IMG_SS_TYPE_e;

typedef struct /*_AMBA_IMG_SENSOR_HAL_FIFO_INDEX_s_*/ {
    UINT32  Wr;
    UINT32  Rd;
} AMBA_IMG_SENSOR_HAL_FIFO_INDEX_s;

#define AMBA_IMG_SENSOR_HAL_FIFO_SIZE    5U

typedef struct /*_AMBA_IMG_SENSOR_HAL_FIFO_BUF_s_*/ {
    UINT32                              Count;
    AMBA_IMG_SENSOR_HAL_FIFO_INDEX_s    Index;
    UINT64                              Ctrl[AMBA_IMG_SENSOR_HAL_FIFO_SIZE];
    UINT64                              Data[AMBA_IMG_SENSOR_HAL_FIFO_SIZE];
} AMBA_IMG_SENSOR_HAL_FIFO_BUF_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_FIFO_s_*/ {
    AMBA_IMG_SENSOR_HAL_FIFO_BUF_s    Agc;
    AMBA_IMG_SENSOR_HAL_FIFO_BUF_s    Dgc;
    AMBA_IMG_SENSOR_HAL_FIFO_BUF_s    Wgc;
    AMBA_IMG_SENSOR_HAL_FIFO_BUF_s    Shr;
    AMBA_IMG_SENSOR_HAL_FIFO_BUF_s    DDgc;
    AMBA_IMG_SENSOR_HAL_FIFO_BUF_s    User;
} AMBA_IMG_SENSOR_HAL_FIFO_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_DATA_REG_s_*/ {
    UINT64  Agc;
    UINT64  Dgc;
    UINT64  Wgc;
    UINT64  Shr;
    UINT64  DDgc;
    UINT64  User;
} AMBA_IMG_SENSOR_HAL_DATA_REG_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_CTRL_REG_s_*/ {
    UINT64  Agc;
    UINT64  Dgc;
    UINT64  Wgc;
    UINT64  Shr;
    UINT64  DDgc;
    UINT64  User;
} AMBA_IMG_SENSOR_HAL_CTRL_REG_s;

#define AMBA_IMG_SENSOR_HAL_RING_SIZE      16U

typedef AMBA_IMG_SENSOR_HAL_FIFO_INDEX_s AMBA_IMG_SENSOR_HAL_RING_INDEX_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_RING_s_*/ {
    AMBA_KAL_MUTEX_t                    Mutex;
    UINT32                              Count;
    AMBA_IMG_SENSOR_HAL_RING_INDEX_s    Index;
    UINT32                              RawCapSeq[AMBA_IMG_SENSOR_HAL_RING_SIZE];
    AMBA_IMG_SENSOR_HAL_DATA_REG_s      Data[AMBA_IMG_SENSOR_HAL_RING_SIZE];
} AMBA_IMG_SENSOR_HAL_RING_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_OP_USER_s_*/ {
    UINT32 DgcType;
    UINT32 SsType;
    UINT32 SvrDelay;
    UINT32 ShrDelay;
    UINT32 AgcDelay;
    UINT32 DgcDelay;
    UINT32 WgcDelay;
    UINT32 AuxDelay;
    UINT32 StatDelay;
    UINT32 ForcePostSof;
    UINT32 ForceAdvEof;
    UINT32 AikExecTime;
    UINT32 EofMoveMaxTime;
    UINT32 WriteLatency;
    UINT32 ForceSsInVideo;
} AMBA_IMG_SENSOR_HAL_OP_USER_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_OP_CTRL_s_*/ {
    UINT32 R2yBackground;
    UINT32 AdvSvrUpd;
    UINT32 AdvSsiCmd;
    UINT32 AdvRgbCmd;
    UINT32 PreAikCmd;
    UINT32 AdvAikCmd;
} AMBA_IMG_SENSOR_HAL_OP_CTRL_s;

typedef union /*_AMBA_IMG_SENSOR_HAL_OP_TIMING_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Timetick:          16;
        UINT32 Reserved:          14;
        UINT32 Enable:             2;
    } Bits;
} AMBA_IMG_SENSOR_HAL_OP_TIMING_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_OP_TIMING_s_*/ {
    AMBA_IMG_SENSOR_HAL_OP_TIMING_u    PostSof;
    AMBA_IMG_SENSOR_HAL_OP_TIMING_u    Mof;
    AMBA_IMG_SENSOR_HAL_OP_TIMING_u    AdvDsp;
    AMBA_IMG_SENSOR_HAL_OP_TIMING_u    AdvEof;
    AMBA_IMG_SENSOR_HAL_OP_TIMING_u    AikExecute;
} AMBA_IMG_SENSOR_HAL_OP_TIMING_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_STATUS_s_*/ {
    UINT32 SsState;
    UINT32 SsIndex;
    UINT32 LastSsIndex;
    UINT32 LastSvr;
} AMBA_IMG_SENSOR_HAL_SS_STATUS_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_OP_s_*/ {
    AMBA_IMG_SENSOR_HAL_OP_USER_s      User;
    AMBA_IMG_SENSOR_HAL_OP_CTRL_s      Ctrl;
    AMBA_IMG_SENSOR_HAL_OP_TIMING_s    Timing;
    AMBA_IMG_SENSOR_HAL_SS_STATUS_s    Status;
} AMBA_IMG_SENSOR_HAL_OP_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_CFG_s_*/ {
    UINT32  Mode;
    UINT32  Bit;
    UINT32  FirstPixelColor;
    UINT32  Width;
    UINT32  Height;
    UINT32  Binning;
    UINT32  ShutterMode;
} AMBA_IMG_SENSOR_HAL_CFG_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_FRAME_s_*/ {
    UINT32  Rate;
    UINT32  Timetick;
    UINT32  TotalLine;
    UINT32  TotalShutterLine;
    UINT32  TotalNsInLine;
    UINT32  TotalNsInShutterLine;
} AMBA_IMG_SENSOR_HAL_FRAME_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_IMAGE_s_*/ {
    UINT32  StartX;
    UINT32  StartY;
    UINT32  Width;
    UINT32  Height;
    UINT32  HFactorNum;
    UINT32  HFactorDen;
    UINT32  VFactorNum;
    UINT32  VFactorDen;
} AMBA_IMG_SENSOR_HAL_IMAGE_s;

typedef union /*_AMBA_IMG_SENSOR_HAL_AUX_u_*/ {
    UINT64 Data;
    struct {
        UINT32 R2yBackground:          1;
        UINT32 Stitch:                 1;
        UINT32 DeferredBlackLevel:     1;
        UINT32 LowDelay:               1;
        UINT32 MaxSlowShutterIndex:    2;
        UINT32 ForceSlowShutter:       1;
        UINT32 IqSecId:                2;
        UINT32 Reserved_L:            19;
        UINT32 VinSkipFrame:           3;
        UINT32 VinSkipFrameEn:         1;
        UINT32 Reserved_H:            16;
        UINT32 OpMode:                 8;
        UINT32 Cmd:                    8;
    } Bits;
} AMBA_IMG_SENSOR_HAL_AUX_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_AUX_s_*/ {
    AMBA_IMG_SENSOR_HAL_AUX_u  Ctx;
} AMBA_IMG_SENSOR_HAL_AUX_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_INFO_s_*/ {
    AMBA_IMG_SENSOR_HAL_OP_s       Op;
    AMBA_IMG_SENSOR_HAL_CFG_s      Cfg;
    AMBA_IMG_SENSOR_HAL_FRAME_s    Frame;
    AMBA_IMG_SENSOR_HAL_IMAGE_s    Image;
    AMBA_IMG_SENSOR_HAL_AUX_s      *pAux;
} AMBA_IMG_SENSOR_HAL_INFO_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_STATUS_s_*/ {
    AMBA_IMG_SENSOR_HAL_DATA_REG_s     In;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s     Act;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s     Last;
} AMBA_IMG_SENSOR_HAL_STATUS_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_CONTEXT_s_*/ {
    AMBA_IMG_SENSOR_HAL_FIFO_s      Fifo;
    AMBA_IMG_SENSOR_HAL_RING_s      Ring;
    AMBA_IMG_SENSOR_HAL_STATUS_s    Status;
    AMBA_IMG_SENSOR_HAL_INFO_s      *pInfo;
} AMBA_IMG_SENSOR_HAL_CONTEXT_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_USER_RING_s_*/ {
    UINT32  Index;
    UINT32  Count;
    UINT32  Size;
    UINT8   *pMem;
} AMBA_IMG_SENSOR_HAL_USER_RING_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_USER_CONTEXT_s_*/ {
    AMBA_IMG_SENSOR_HAL_USER_RING_s    Ring;
} AMBA_IMG_SENSOR_HAL_USER_CONTEXT_s;

#define AMBA_IMG_SENSOR_HAL_HDR_SIZE         4U

typedef struct /*_INAGE_SENSOR_HAL_HDR_FIFO_BUF_s_*/ {
    UINT32                              Count;
    AMBA_IMG_SENSOR_HAL_FIFO_INDEX_s    Index;
    UINT64                              Data[AMBA_IMG_SENSOR_HAL_FIFO_SIZE][AMBA_IMG_SENSOR_HAL_HDR_SIZE];
} AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_FIFO_s_*/ {
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s    Agc;
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s    Dgc;
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s    Wgc;
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s    Shr;
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s    DDgc;
} AMBA_IMG_SENSOR_HAL_HDR_FIFO_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_DATA_s_*/ {
    UINT64  Agc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT64  Dgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT64  Wgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT64  Shr[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT64  DDgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
} AMBA_IMG_SENSOR_HAL_HDR_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_REG_s_*/ {
    UINT32  Agc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT32  Dgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT64  Wgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT32  Shr[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    UINT32  DDgc[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
} AMBA_IMG_SENSOR_HAL_HDR_REG_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_STATUS_s_*/ {
    AMBA_IMG_SENSOR_HAL_HDR_REG_s     Load;
    AMBA_IMG_SENSOR_HAL_HDR_REG_s     Last;
} AMBA_IMG_SENSOR_HAL_HDR_STATUS_s;

#define AMBA_IMG_SENSOR_HAL_HDR_SS_ALL     0U
#define AMBA_IMG_SENSOR_HAL_HDR_SS_LONG    1U
#define AMBA_IMG_SENSOR_HAL_HDR_SS_DEF     0xFFU

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_EXPO_s_*/ {
    UINT32  Max;
    UINT32  Min;
} AMBA_IMG_SENSOR_HAL_HDR_EXPO_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_FRAME_s_*/ {
    UINT32                            ChannelSsType;
    UINT32                            ChannelNum;
    UINT32                            TotalShutterLine[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
    AMBA_IMG_SENSOR_HAL_HDR_EXPO_s    ExposureLine[AMBA_IMG_SENSOR_HAL_HDR_SIZE];
} AMBA_IMG_SENSOR_HAL_HDR_FRAME_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_INFO_s_*/ {
    AMBA_IMG_SENSOR_HAL_HDR_FRAME_s    Frame;
} AMBA_IMG_SENSOR_HAL_HDR_INFO_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s_*/ {
    AMBA_IMG_SENSOR_HAL_HDR_FIFO_s      Fifo;
    AMBA_IMG_SENSOR_HAL_HDR_STATUS_s    Status;
    AMBA_IMG_SENSOR_HAL_HDR_INFO_s      *pInfo;
} AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_CHAN_s_*/ {
    UINT32                                Magic;
    AMBA_IMG_SENSOR_HAL_CONTEXT_s         *pCtx;
    AMBA_IMG_SENSOR_HAL_USER_CONTEXT_s    *pUserCtx;
    AMBA_IMG_SENSOR_HAL_HDR_CONTEXT_s     *pHdrCtx;
} AMBA_IMG_SENSOR_HAL_CHAN_s;

typedef union /*_AMBA_IMG_SENSOR_HAL_MARK_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Rdy:          1;
        UINT32 Enable:       1;
        UINT32 Reserved:    30;
    } Bits;
} AMBA_IMG_SENSOR_HAL_MARK_OP_u;

typedef AMBA_IMG_SENSOR_HAL_FIFO_INDEX_s AMBA_IMG_SENSOR_HAL_MARK_INDEX_s;
#define AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH   10U

typedef struct /*_AMBA_IMG_SENSOR_HAL_MARK_DATA_s_*/ {
    UINT32 Timetick;
    char   Name[AMBA_IMG_SENSOR_HAL_MARK_STRING_LENGTH+1U];
} AMBA_IMG_SENSOR_HAL_MARK_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_MARK_MSG_s_*/ {
    UINT32                             Num;
    AMBA_IMG_SENSOR_HAL_MARK_DATA_s    *Data;
} AMBA_IMG_SENSOR_HAL_MARK_MSG_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_MARK_s_*/ {
    AMBA_IMG_SENSOR_HAL_MARK_OP_u       Op;
    AMBA_KAL_MUTEX_t                    Mutex;
    AMBA_IMG_SENSOR_HAL_MARK_INDEX_s    Index;
    AMBA_IMG_SENSOR_HAL_MARK_MSG_s      Msg;
} AMBA_IMG_SENSOR_HAL_MARK_s;

typedef union /*_AMBA_IMG_SENSOR_HAL_DEBUG_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Msg:          1;
        UINT32  Reserved:    31;
    } Bits;
} AMBA_IMG_SENSOR_HAL_DEBUG_u;

typedef union /*_AMBA_IMG_SENSOR_HAL_VIN_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Out2Dsp:        1;
        UINT32 Out2Sensor:     1;
        UINT32 Reserved:      30;
    } Bits;
} AMBA_IMG_SENSOR_HAL_VIN_OP_u;

typedef struct /*_AMBA_IMG_SENSOR_HAL_COUNTER_s_*/ {
    UINT32  Raw;
} AMBA_IMG_SENSOR_HAL_COUNTER_s;

typedef struct /*_AMBA_IMG_SENSOR_HAL_VIN_s_*/ {
    AMBA_IMG_SENSOR_HAL_DEBUG_u      Debug;
    AMBA_IMG_SENSOR_HAL_VIN_OP_u     Op;
    AMBA_IMG_SENSOR_HAL_MARK_s       Timing;
    AMBA_IMG_SENSOR_HAL_COUNTER_s    Counter;
    AMBA_IMG_SENSOR_HAL_AUX_s        *pAux;
} AMBA_IMG_SENSOR_HAL_VIN_s;

extern AMBA_IMG_SENSOR_HAL_VIN_s AmbaImgSensorHAL_Vin[AMBA_IMGFRW_NUM_VIN_CHANNEL];
extern AMBA_IMG_SENSOR_HAL_CHAN_s *AmbaImgSensorHAL_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

UINT32 AmbaImgSensorHAL_Init(void);
UINT32 AmbaImgSensorHAL_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgSensorHAL_Reset(UINT32 VinId);
UINT32 AmbaImgSensorHAL_Stop(UINT32 VinId);

UINT32 AmbaImgSensorHAL_InfoGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_INFO_s **pInfo);
UINT32 AmbaImgSensorHAL_RingGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_HAL_DATA_REG_s *pData, UINT32 RawCapSeq);
#ifdef CONFIG_BUILD_IMGFRW_FIFO_PROT
UINT32 AmbaImgSensorHAL_FifoPutCheck(AMBA_IMG_CHANNEL_ID_s ImageChanId);
#endif
UINT32 AmbaImgSensorHAL_TimingMarkEn(UINT32 VinId, UINT32 Enable);
UINT32 AmbaImgSensorHAL_TimingMarkChk(UINT32 VinId);
UINT32 AmbaImgSensorHAL_TimingMarkClr(UINT32 VinId);
UINT32 AmbaImgSensorHAL_TimingMarkPutE(UINT32 VinId, const char *pName);
  void AmbaImgSensorHAL_TimingMarkPut(UINT32 VinId, const char *pName);
  void AmbaImgSensorHAL_TimingMarkPrt(UINT32 VinId);

#endif  /* AMBA_IMG_SENSOR_HAL_H */
