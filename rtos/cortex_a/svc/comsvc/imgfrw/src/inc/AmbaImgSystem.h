/**
 *  @file AmbaImgSystem.h
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
 *  @details Constants and Definitions for Amba Image System
 *
 */

#ifndef AMBA_IMG_SYSTEM_H
#define AMBA_IMG_SYSTEM_H

typedef struct /*_AMBA_IMG_SYSTEM_OP_s_*/ {
    UINT32  Mode;
    UINT32  R2yUserDelay;
    UINT32  R2yDelay;
} AMBA_IMG_SYSTEM_OP_s;

typedef union /*_AMBA_IMG_SYSTEM_PIPE_SIZE_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Width:         16;
        UINT32  Height:        16;
        UINT32  Reserved_H:    32;
    } Bits;
} AMBA_IMG_SYSTEM_PIPE_SIZE_u;

typedef struct /*_AMBA_IMG_SYSTEM_CFG_s_*/ {
    AMBA_IMG_SYSTEM_PIPE_SIZE_u    In;
    AMBA_IMG_SYSTEM_PIPE_SIZE_u    Out;
} AMBA_IMG_SYSTEM_CFG_s;

typedef struct /*_AMBA_IMG_SYSTEM_PIPE_BLC_s_*/ {
    AMBA_KAL_MUTEX_t    Mutex;
    INT32               Offset[4];
} AMBA_IMG_SYSTEM_PIPE_BLC_s;

typedef enum /*_AMBA_IMG_SYSTEM_PIPE_WB_e_*/ {
    WB_ID_SENSOR = 0,
    WB_ID_FE,
    WB_ID_FE1,
    WB_ID_FE2,
    WB_ID_BE,
    WB_ID_BE_ALT,
    WB_ID_TOTAL
} AMBA_IMG_SYSTEM_PIPE_WB_e;

#define WB_PIPE_FE        (((UINT32) WB_ID_FE) << 16U)
#define WB_PIPE_FE1       (((UINT32) WB_ID_FE1) << 16U)
#define WB_PIPE_FE2       (((UINT32) WB_ID_FE2) << 16U)
#define WB_PIPE_BE        (((UINT32) WB_ID_BE) << 16U)
#define WB_PIPE_BE_ALT    (((UINT32) WB_ID_BE_ALT) << 16U)

typedef union /*_AMBA_IMG_SYSTEM_PIPE_WB_OP_u_*/ {
    UINT32  Data;
    struct {
        UINT32  Scaler:        16;
        UINT32  Reserved:      14;
        UINT32  Fine:           1;
        UINT32  Mix:            1;
    } Bits;
} AMBA_IMG_SYSTEM_PIPE_WB_OP_u;

typedef struct /*_AMBA_IMG_SYSTEM_PIPE_WB_CTX_s_*/ {
    AMBA_KAL_MUTEX_t                Mutex;
    AMBA_IMG_SYSTEM_PIPE_WB_OP_u    Op;
    UINT32                          Wgc[3];
    UINT32                          Igc;
    UINT32                          Dgc;
    UINT32                          Ggc;
    UINT32                          FineWgc[3];
    UINT32                          FineIgc;
    UINT32                          FineDgc;
    UINT32                          FineGgc;
} AMBA_IMG_SYSTEM_PIPE_WB_CTX_s;

typedef struct /*_AMBA_IMG_SYSTEM_PIPE_WB_s_*/ {
    AMBA_IMG_SYSTEM_PIPE_WB_CTX_s    Fe[3];
    AMBA_IMG_SYSTEM_PIPE_WB_CTX_s    Be;
    AMBA_IMG_SYSTEM_PIPE_WB_CTX_s    BeAlt;
} AMBA_IMG_SYSTEM_PIPE_WB_s;

typedef struct /*_AMBA_IMG_SYSTEM_IK_s_*/ {
    AMBA_KAL_MUTEX_t              Mutex;
    AMBA_IK_MODE_CFG_s            Mode;
    AMBA_IMG_SYSTEM_PIPE_BLC_s    Blc;
    AMBA_IMG_SYSTEM_PIPE_WB_s     Wb;
} AMBA_IMG_SYSTEM_PIPE_s;

typedef union /*_AMBA_IMG_SYSTEM_CAP_SIZE_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Width:         16;
        UINT32  Height:        16;
        UINT32  Reserved_H:    24;
        UINT32  Cmd:            8;
    } Bits;
} AMBA_IMG_SYSTEM_CAP_SIZE_u;

typedef struct /*_AMBA_IMG_SYSTEM_VIN_INFO_s_*/ {
    AMBA_IMG_SYSTEM_CAP_SIZE_u    Cap;
} AMBA_IMG_SYSTEM_VIN_INFO_s;

typedef union /*_AMBA_IMG_SYSTEM_PIPE_MODE_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Type:           4;
        UINT32  Reserved_L:    28;
        UINT32  Reserved_H:    24;
        UINT32  Cmd:            8;
    } Bits;
} AMBA_IMG_SYSTEM_PIPE_MODE_u;

typedef union /*_AMBA_IMG_SYSTEM_PIPE_HDR_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Enable:         2;
        UINT32  ExposureNum:    3;
        UINT32  LCeEnable:      1;
        UINT32  BackWgc:        1;
        UINT32  BackAltWgc:     1;
        UINT32  BackDgc:        1;
        UINT32  Reserved_L:    23;
        UINT32  Reserved_H:    24;
        UINT32  Cmd:            8;
    } Bits;
} AMBA_IMG_SYSTEM_PIPE_HDR_u;

typedef struct /*_AMBA_IMG_SYSTEM_PIPE_INFO_s_*/ {
    AMBA_IMG_SYSTEM_PIPE_MODE_u    Mode;
    AMBA_IMG_SYSTEM_PIPE_HDR_u     Hdr;
} AMBA_IMG_SYSTEM_PIPE_INFO_s;

typedef union /*_AMBA_IMG_SYSTEM_IK_BUF_u_*/ {
    UINT64 Data;
    struct {
        UINT32  CtxNum:         6;
        UINT32  CfgNum:         9;
        UINT32  IsoNum:         2;
        UINT32  RingType:       1;
        UINT32  Reserved:       9;
        UINT32  Enable:         1;
        UINT32  Reserved_L:     4;
        UINT32  Reserved_H:    24;
        UINT32  Cmd:            8;
    } Bits;
    struct {
        UINT32  CtxNum:         6;
        UINT32  CfgNum:         9;
        UINT32  IsoNum:         2;
        UINT32  RingType:       1;
        UINT32  CfgIndex:       9;
        UINT32  Enable:         1;
        UINT32  Reserved_L:     4;
        UINT32  Reserved_H:    24;
        UINT32  Cmd:            8;
    } Bits2;
} AMBA_IMG_SYSTEM_IK_BUF_u;

typedef struct /*_AMBA_IMG_SYSTEM_IK_RING_s_*/ {
    UINT32  Num;
    UINT32  Offset;
    UINT32  Index;
} AMBA_IMG_SYSTEM_IK_RING_s;

#define AMBA_IMG_SYSTEM_IK_CTX_MAX_NUM    32U

typedef struct /*_AMBA_IMG_SYSTEM_IK_INFO_s_*/ {
    AMBA_KAL_MUTEX_t             Mutex;
    AMBA_IMG_SYSTEM_IK_BUF_u     Buf;
    AMBA_IMG_SYSTEM_IK_RING_s    Ring[AMBA_IMG_SYSTEM_IK_CTX_MAX_NUM];
} AMBA_IMG_SYSTEM_IK_INFO_s;

typedef struct /*_AMBA_IMG_SYSTEM_AUX_INFO_s_*/ {
    AMBA_IMG_SYSTEM_IK_INFO_s    Ik;
} AMBA_IMG_SYSTEM_AUX_INFO_s;

typedef struct /*_AMBA_IMG_SYSTEM_INFO_s_*/ {
    AMBA_IMG_SYSTEM_VIN_INFO_s     Vin;
    AMBA_IMG_SYSTEM_PIPE_INFO_s    Pipe;
    AMBA_IMG_SYSTEM_AUX_INFO_s     *pAux;
} AMBA_IMG_SYSTEM_INFO_s;

typedef union /*_AMBA_IMG_SYSTEM_RING_INDEX_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Wr:         8;
        UINT32  Rd:         8;
        UINT32  Reserved:   8;
        UINT32  Count:      8;
    } Bits;
} AMBA_IMG_SYSTEM_RING_INDEX_u;

typedef struct /*_AMBA_IMG_SYSTEM_AIK_CTX_s_*/ {
    UINT32  Id;
    void    *pIsoCfg;
} AMBA_IMG_SYSTEM_AIK_CTX_s;

typedef struct /*_AMBA_IMG_SYSTEM_AIK_STATUS_s_*/ {
    AMBA_KAL_MUTEX_t                Mutex;
    UINT32                          Num;
    AMBA_IMG_SYSTEM_RING_INDEX_u    Index;
    AMBA_IMG_SYSTEM_AIK_CTX_s       *pCtx;
} AMBA_IMG_SYSTEM_AIK_STATUS_s;

typedef union /*_AMBA_IMG_SYSTEM_PAIK_IDX_s_*/ {
    UINT32 Data;
    struct {
        UINT32  Wr:       8;
        UINT32  Rd:       8;
        UINT32  Rd2:      8;
        UINT32  Count:    8;
    } Bits;
} AMBA_IMG_SYSTEM_PAIK_IDX_u;

typedef struct /*_AMBA_IMG_SYSTEM_PAIK_INFO_s_*/ {
    UINT32                   Magic;
    UINT32                   Id;
    UINT32                   RawCapSeq;
    UINT32                   PostAikCnt;
    UINT32                   VinSkipFrame;
    AMBA_IMG_CHANNEL_ID_s    ImageChanId;
    UINT64                   User;
    UINT32                   EorTimetick;
    UINT32                   R2yUserDelay;
    UINT64                   R2yUser;
} AMBA_IMG_SYSTEM_PAIK_INFO_s;

typedef struct /*_AMBA_IMG_SYSTEM_PAIK_RING_s_*/ {
    AMBA_KAL_MUTEX_t               Mutex;
    UINT32                         Num;
    AMBA_IMG_SYSTEM_PAIK_IDX_u     Index;
    AMBA_IMG_SYSTEM_PAIK_INFO_s    *pInfo;
} AMBA_IMG_SYSTEM_PAIK_RING_s;

typedef struct /*_AMBA_IMG_SYSTEM_PAIK_MTX_s_*/ {
    AMBA_KAL_MUTEX_t    Ack;
} AMBA_IMG_SYSTEM_PAIK_MTX_s;

typedef struct /*_AMBA_IMG_SYSTEM_PAIK_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t    Data;
    AMBA_KAL_SEMAPHORE_t    Timer;
    AMBA_KAL_SEMAPHORE_t    Ack;
} AMBA_IMG_SYSTEM_PAIK_SEM_s;

typedef struct /*_AMBA_IMG_SYSTEM_PAIK_s_*/ {
    AMBA_IMG_SYSTEM_PAIK_RING_s    Ring;
    AMBA_IMG_SYSTEM_PAIK_MTX_s     Mtx;
    AMBA_IMG_SYSTEM_PAIK_SEM_s     Sem;
} AMBA_IMG_SYSTEM_PAIK_s;

typedef struct /*_AMBA_IMG_SYSTEM_USER_RING_s_*/ {
    AMBA_KAL_MUTEX_t    Mutex;
    UINT32              Index;
    UINT32              Count;
    UINT32              Size;
    UINT8               *pMem;
} AMBA_IMG_SYSTEM_USER_RING_s;

typedef struct /*_AMBA_IMG_SYSTEM_USER_s_*/ {
    AMBA_IMG_SYSTEM_USER_RING_s    Ring;
} AMBA_IMG_SYSTEM_USER_s;

typedef struct /*_AMBA_IMG_SYSTEM_AIK_s_*/ {
    AMBA_IMG_SYSTEM_AIK_STATUS_s    *pStatus;
    AMBA_IMG_SYSTEM_PAIK_s          *pPost;
    AMBA_IMG_SYSTEM_USER_s          *pR2yUser;
} AMBA_IMG_SYSTEM_AIK_s;

typedef struct /*_AMBA_IMG_SYSTEM_RING_WB_s_*/ {
    AMBA_IK_FE_WB_GAIN_s    Fe[3];
    AMBA_IK_WB_GAIN_s       Be;
    AMBA_IK_WB_GAIN_s       BeAlt;
} AMBA_IMG_SYSTEM_RING_WB_s;

typedef struct /*_AMBA_IMG_SYSTEM_RING_PIPE_s_*/ {
    AMBA_IMG_SYSTEM_RING_WB_s    Wb;
} AMBA_IMG_SYSTEM_RING_PIPE_s;

typedef struct /*_AMBA_IMG_SYSTEM_RING_CTX_s_*/ {
    UINT32                         RawCapSeq;
    AMBA_IMG_SYSTEM_RING_PIPE_s    Pipe;
} AMBA_IMG_SYSTEM_RING_CTX_s;

typedef struct /*_AMBA_IMG_SYSTEM_RING_s_*/ {
    AMBA_KAL_MUTEX_t                Mutex;
    UINT32                          Num;
    AMBA_IMG_SYSTEM_RING_INDEX_u    Index;
    AMBA_IMG_SYSTEM_RING_CTX_s      *pCtx;
} AMBA_IMG_SYSTEM_RING_s;

#define AMBA_IMG_SYSTEM_ZONE_TYPE_NUM    2U

typedef struct /*_AMBA_IMG_SYSTEM_ZONE_s_*/ {
    UINT32                       InitFlag;
    AMBA_IMG_SYSTEM_OP_s         Op;
    AMBA_IMG_SYSTEM_CFG_s        Cfg;
    AMBA_IMG_SYSTEM_PIPE_s       Pipe[AMBA_IMG_SYSTEM_ZONE_TYPE_NUM];
    AMBA_IMG_SYSTEM_INFO_s       *pInfo;
    AMBA_IMG_SYSTEM_AIK_s        *pAik;
    AMBA_IMG_SYSTEM_RING_s       *pRing;
    AMBA_IMG_SYSTEM_AIK_CTX_s    ActAikCtx;
} AMBA_IMG_SYSTEM_ZONE_s;

typedef struct /*_AMBA_IMG_SYSTEM_CONTEXT_s_*/ {
    UINT32                    Magic;
    AMBA_IMG_SYSTEM_ZONE_s    *pZone;
} AMBA_IMG_SYSTEM_CONTEXT_s;

typedef struct /*_AMBA_IMG_SYSTEM_CHAN_s_*/ {
    UINT32                       Magic;
    AMBA_IMG_SYSTEM_CONTEXT_s    *pCtx;
    AMBA_IMG_SYSTEM_INFO_s       *pInfo;
} AMBA_IMG_SYSTEM_CHAN_s;

typedef union /*_AMBA_IMG_SYSTEM_DEBUG_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Msg:            1;
        UINT32 Timing:         1;
        UINT32 Reserved:      30;
    } Bits;
} AMBA_IMG_SYSTEM_DEBUG_u;

typedef union /*_AMBA_IMG_SYSTEM_VIN_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Reserved:     32;
    } Bits;
} AMBA_IMG_SYSTEM_VIN_OP_u;

typedef struct /*_AMBA_IMG_SYSTEM_COUNTER_s_*/ {
    UINT32  Raw;
} AMBA_IMG_SYSTEM_COUNTER_s;

typedef struct /*_AMBA_IMG_SYSTEM_VIN_s_*/ {
    AMBA_IMG_SYSTEM_DEBUG_u      Debug;
    AMBA_IMG_SYSTEM_VIN_OP_u     Op;
    AMBA_IMG_SYSTEM_COUNTER_s    Counter;
} AMBA_IMG_SYSTEM_VIN_s;

extern AMBA_IMG_SYSTEM_VIN_s AmbaImgSystem_Vin[AMBA_IMGFRW_NUM_VIN_CHANNEL];
extern AMBA_IMG_SYSTEM_CHAN_s *AmbaImgSystem_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];
extern AMBA_IMG_SYSTEM_AUX_INFO_s AmbaImgSystem_AuxInfo;

UINT32 AmbaImgSystem_Init(void);
UINT32 AmbaImgSystem_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgSystem_Reset(UINT32 VinId);

UINT32 AmbaImgSystem_ContextIdGet(UINT32 ZoneIndex);
UINT32 AmbaImgSystem_RingGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_RING_PIPE_s **pPipe, UINT32 RawCapSeq);

UINT32 AmbaImgSystem_ImageModeSet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const AMBA_IK_MODE_CFG_s *pImgMode);
UINT32 AmbaImgSystem_ImageModeGet(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, AMBA_IK_MODE_CFG_s **pImgMode);

UINT32 AmbaImgSystem_BlcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const INT32 *pOffset);
UINT32 AmbaImgSystem_BlcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, INT32 *pOffset);

UINT32 AmbaImgSystem_WbGainMix(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Enable);
UINT32 AmbaImgSystem_WbGainFine(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 Enable);

UINT32 AmbaImgSystem_WgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pWgc);
UINT32 AmbaImgSystem_IgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pIgc);
UINT32 AmbaImgSystem_DgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc);
UINT32 AmbaImgSystem_GgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pGgc);

UINT32 AmbaImgSystem_WgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pWgc);
UINT32 AmbaImgSystem_IgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pIgc);
UINT32 AmbaImgSystem_DgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pDgc);
UINT32 AmbaImgSystem_GgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pGgc);

UINT32 AmbaImgSystem_FineWgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pWgc);
UINT32 AmbaImgSystem_FineIgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pIgc);
UINT32 AmbaImgSystem_FineDgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pDgc);
UINT32 AmbaImgSystem_FineGgcWrite(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, const UINT32 *pGgc);

UINT32 AmbaImgSystem_FineWgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pWgc);
UINT32 AmbaImgSystem_FineIgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pIgc);
UINT32 AmbaImgSystem_FineDgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pDgc);
UINT32 AmbaImgSystem_FineGgcRead(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Mode, UINT32 *pGgc);

UINT32 AmbaImgSystem_R2yUserBuffer(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem);
UINT32 AmbaImgSystem_R2yUserDelay(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 R2yUserDelay, UINT32 R2yDelay);

UINT32 AmbaImgSystem_PostAikWait(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SYSTEM_PAIK_INFO_s **pPostAikInfo);
UINT32 AmbaImgSystem_PostAik(const AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo);

UINT32 AmbaImgSystem_R2yUserWait(const AMBA_IMG_SYSTEM_PAIK_INFO_s *pPostAikInfo);
UINT32 AmbaImgSystem_R2yUserDone(UINT64 PostAikId);

UINT32 AmbaImgSystem_AikExec(AMBA_IMG_CHANNEL_ID_s ImageChanId);

#endif  /* AMBA_IMG_SYSTEM_H */
