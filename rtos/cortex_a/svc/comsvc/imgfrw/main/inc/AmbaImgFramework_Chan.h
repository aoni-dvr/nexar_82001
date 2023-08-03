/**
 *  @file AmbaImgFramework_Chan.h
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
 *  @details Constants and Definitions for Amba Image Framework Channel
 *
 */

#ifndef AMBA_IMG_FRW_CHAN_H
#define AMBA_IMG_FRW_CHAN_H

#ifdef CONFIG_QNX
#ifndef AMBA_KAL_H
#include "AmbaKAL.h"
#endif
#endif

#ifndef AMBA_IMG_FRW_LIB_COMSVC
#if 0
/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgChannel
\*---------------------------------------------------------------------------*/
#define AMBA_IMGFRW_NUM_VIN_CHANNEL    AMBA_NUM_VIN_CHANNEL
#define AMBA_IMGFRW_NUM_VIN_SENSOR     4U
#define AMBA_IMGFRW_NUM_VIN_ALGO       4U
#define AMBA_IMGFRW_NUM_FOV_CHANNEL    AMBA_DSP_MAX_VIEWZONE_NUM

typedef union /*_AMBA_IMG_CHANNEL_ID_u_*/ {
    UINT64 Data;
    struct {
        UINT32 VinId:        5;
        UINT32 ChainId:      2;
        UINT32 SensorId:     4;
        UINT32 HdrId:        4;
        UINT32 AlgoId:       6;
        UINT32 Reserved_L:  10;
        UINT32 ZoneMsb:      1;
        UINT32 ZoneId:      32;
    } Bits;
} AMBA_IMG_CHANNEL_ID_u;

typedef struct /*_AMBA_IMG_CHANNEL_ID_s_*/ {
    AMBA_IMG_CHANNEL_ID_u    Ctx;
} AMBA_IMG_CHANNEL_ID_s;

typedef union /*_AMBA_IMG_CHANNEL_VR_ID_u_*/ {
    UINT64 Data;
    struct {
        UINT32 VrId:        32;
        UINT32 VrAltId:      4;
        UINT32 Reserved_H:  28;
    } Bits;
} AMBA_IMG_CHANNEL_VR_ID_u;

typedef struct /*_AMBA_IMG_CHANNEL_VR_ID_s_*/ {
    AMBA_IMG_CHANNEL_VR_ID_u    Ctx;
} AMBA_IMG_CHANNEL_VR_ID_s;

typedef struct /*_AMBA_IMG_CHANNEL_VR_MAP_s_*/ {
    AMBA_IMG_CHANNEL_VR_ID_s    Id;
    UINT32 Cnt;
    UINT32 Flag[AMBA_IMGFRW_NUM_VIN_CHANNEL];
} AMBA_IMG_CHANNEL_VR_MAP_s;

typedef union /*_AMBA_IMG_CHANNEL_INTER_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Id:          32;
        UINT32 FirstId:      5;
        UINT32 Num:          6;
        UINT32 Skip:         3;
        UINT32 Mode:         5;
        UINT32 Batch:        5;
        UINT32 Reserved:     8;
    } Bits;
} AMBA_IMG_CHANNEL_INTER_u;

typedef struct /*_AMBA_IMG_CHANNEL_INTER_s_*/ {
    AMBA_IMG_CHANNEL_INTER_u    Ctx;
} AMBA_IMG_CHANNEL_INTER_s;

typedef union /*_AMBA_IMG_CHANNEL_AIK_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Reserved:    32;
        UINT32 IsoDisId:    32;
    } Bits;
} AMBA_IMG_CHANNEL_AIK_u;

typedef struct /*_AMBA_IMG_CHANNEL_AIK_s_*/ {
    AMBA_IMG_CHANNEL_AIK_u    Ctx;
} AMBA_IMG_CHANNEL_AIK_s;

typedef union /*_AMBA_IMG_CHANNEL_PAIK_u_*/ {
    UINT64 Data;
    struct {
        UINT32 ZoneId:      32;
        UINT32 Reserved:    32;
    } Bits;
} AMBA_IMG_CHANNEL_PAIK_u;

typedef struct /*_AMBA_IMG_CHANNEL_PAIK_s_*/ {
    AMBA_IMG_CHANNEL_PAIK_u    Ctx;
} AMBA_IMG_CHANNEL_PAIK_s;

typedef union /*_AMBA_IMG_CHANNEL_SNAP_u_*/ {
    UINT64 Data;
    struct {
        UINT32 AebId:         12;
        UINT32 Reserved_L:    20;
        UINT32 Reserved_H:    32;
    } Bits;
    struct {
        UINT32 AebCnt:         4;
        UINT32 AebNum:         4;
        UINT32 AebDen:         4;
        UINT32 Reserved_L:    20;
        UINT32 Reserved_H:    32;
    } Bits2;
} AMBA_IMG_CHANNEL_SNAP_u;

typedef struct /*_AMBA_IMG_CHANNEL_SNAP_s_*/ {
    AMBA_IMG_CHANNEL_SNAP_u    Ctx;
} AMBA_IMG_CHANNEL_SNAP_s;

typedef struct /*_AMBA_IMG_CHANNEL_AIK_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t    Req;
    AMBA_KAL_SEMAPHORE_t    Ack;
} AMBA_IMG_CHANNEL_AIK_SEM_s;

typedef UINT32 (*AMBA_IMG_CHANNEL_AIK_SYNC_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId);

typedef struct /*_AMBA_IMG_CHANNEL_AIK_SLOT_s_*/{
    UINT32                         En;
    AMBA_IMG_CHANNEL_AIK_SEM_s     Sem;
    UINT32                         Msg[8];
    const char                     *pName;
    AMBA_IMG_CHANNEL_AIK_SYNC_f    pFunc;
} AMBA_IMG_CHANNEL_AIK_SLOT_s;

typedef enum /*_AMBA_IMG_CHANNEL_AIK_SLOT_ID_*/ {
    IMG_CHAN_AIK_SLOT_ADJ = 0,
    IMG_CHAN_AIK_SLOT_AWB,
    IMG_CHAN_AIK_SLOT_TOTAL
} AMBA_IMG_CHANNEL_AIK_SLOT_ID_e;

typedef struct /*_AMBA_IMG_CHANNEL_AIK_SYNC_s_*/ {
    AMBA_IMG_CHANNEL_AIK_SLOT_s    Slot[IMG_CHAN_AIK_SLOT_TOTAL];
} AMBA_IMG_CHANNEL_AIK_SYNC_s;

typedef enum /*_AMBA_IMG_CHANNEL_EFOV_MODE_e_*/ {
    IMG_CHAN_EFOV_NONE = 0,
    IMG_CHAN_EFOV_MASTER,
    IMG_CHAN_EFOV_SLAVE,
    IMG_CHAN_EFOV_TOTAL
} AMBA_IMG_CHANNEL_EFOV_MODE_e;

typedef union /*_AMBA_IMG_CHANNEL_EFOV_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Var:           32;
        UINT32 Id:             6;
        UINT32 Mode:           2;
        UINT32 Reserved_H:    24;
    } Bits;
} AMBA_IMG_CHANNEL_EFOV_u;

typedef struct /*_AMBA_IMG_CHANNEL_EFOV_s_*/ {
    AMBA_IMG_CHANNEL_EFOV_u    Ctx;
} AMBA_IMG_CHANNEL_EFOV_s;

typedef struct /*_AMBA_IMG_CHANNEL_CONTEXT_s_*/ {
    AMBA_IMG_CHANNEL_ID_s          Id;
    AMBA_IMG_CHANNEL_VR_MAP_s      VrMap;
    AMBA_IMG_CHANNEL_INTER_s       Inter;
    AMBA_IMG_CHANNEL_AIK_s         Aik;
    AMBA_IMG_CHANNEL_PAIK_s        PAik;
    AMBA_IMG_CHANNEL_SNAP_s        Snap;
    AMBA_IMG_CHANNEL_AIK_SYNC_s    *pAikSync;
    AMBA_IMG_CHANNEL_EFOV_s        EFov;
} AMBA_IMG_CHANNEL_CONTEXT_s;

typedef union /*_AMBA_IMG_CHANNEL_AIK_PROC_MSG_u_*/ {
    UINT64 Data;
    struct {
        UINT32 Reserved_L:  32;
        UINT32 Reserved_H:  31;
        UINT32 PreAik:       1;
    } Bits;
    struct {
        UINT32 Var:         32;
        UINT32 Id:           6;
        UINT32 Raw:          1;
        UINT32 AikExec:      1;
        UINT32 Adj:          1;
        UINT32 Awb:          1;
        UINT32 Ae:           1;
        UINT32 SofEnd:       1;
        UINT32 Reserved_H:  20;
    } Bits2;
} AMBA_IMG_CHANNEL_AIK_PROC_MSG_u;

typedef struct /*_AMBA_IMG_CHANNEL_AIK_PROC_MSG_s_*/ {
    AMBA_IMG_CHANNEL_AIK_PROC_MSG_u    Ctx;
} AMBA_IMG_CHANNEL_AIK_PROC_MSG_s;

typedef struct /*_AMBA_IMG_CHANNEL_ISO_INFO_s_*/ {
    UINT32 RawCapSeq;
    UINT32 AikCnt;
    UINT32 VinSkipFrame;
    UINT32 ZoneId;
    UINT32 ContextId;
    UINT32 IkId;
    void   *pIsoCfg;
    UINT64 UserData;
} AMBA_IMG_CHANNEL_ISO_INFO_s;

typedef UINT32 (*AMBA_IMG_CHANNEL_AIK_PROC_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 User);
typedef UINT32 (*AMBA_IMG_CHANNEL_AIK_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId);
typedef UINT32 (*AMBA_IMG_CHANNEL_PAIK_CB_f)(UINT64 PostAikId);
typedef UINT32 (*AMBA_IMG_CHANNEL_SOF_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId);
typedef UINT32 (*AMBA_IMG_CHANNEL_EOF_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId);
typedef UINT32 (*AMBA_IMG_CHANNEL_SLS_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 User);
typedef UINT32 (*AMBA_IMG_CHANNEL_EFOV_TX_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 TxData);
typedef UINT32 (*AMBA_IMG_CHANNEL_EFOV_RX_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 Msg, UINT64 RxData);
typedef UINT32 (*AMBA_IMG_CHANNEL_ISO_CB_f)(const AMBA_IMG_CHANNEL_ISO_INFO_s *pIsoInfo);

typedef struct /*_AMBA_IMG_CHANNEL_TIMING_LINK_s_*/ {
    struct AMBA_IMG_CHANNEL_TIMING_PORT_s_    *Up;
    struct AMBA_IMG_CHANNEL_TIMING_PORT_s_    *Down;
} AMBA_IMG_CHANNEL_TIMING_LINK_s;

typedef UINT32 (*AMBA_IMG_CHANNEL_TIMING_FUNC_f)(UINT64 Data);

typedef union /*_AMBA_IMG_CHANNEL_TIMING_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Timetick:          16;
        UINT32 Reserved:          10;
        UINT32 Id:                 4;
        UINT32 Enable:             2;
    } Bits;
} AMBA_IMG_CHANNEL_TIMING_OP_u;

typedef struct AMBA_IMG_CHANNEL_TIMING_PORT_s_ {
    const char                        *pName;
    AMBA_IMG_CHANNEL_TIMING_OP_u      Op;
    UINT64                            Data;
    UINT32                            SkipFrame;
    AMBA_IMG_CHANNEL_TIMING_FUNC_f    pFunc;
    AMBA_IMG_CHANNEL_TIMING_LINK_s    Link;
} AMBA_IMG_CHANNEL_TIMING_PORT_s;

typedef enum /*_AMBA_IMG_CHANNEL_TIMING_ID_e_*/ {
    CHANNEL_TIMING_ID_POST_SOF = 0,
    CHANNEL_TIMING_ID_ADV_DSP,
    CHANNEL_TIMING_ID_ADV_EOF,
    CHANNEL_TIMING_ID_AIK_EXEC,
    CHANNEL_TIMING_ID_TOTAL,
    CHANNEL_TIMING_ID_MOF
} AMBA_IMG_CHANNEL_TIMING_ID_e;

typedef struct /*_AMBA_IMG_CHANNEL_TIMING_s_*/ {
    AMBA_IMG_CHANNEL_TIMING_PORT_s    Port[CHANNEL_TIMING_ID_TOTAL];
} AMBA_IMG_CHANNEL_TIMING_s;

typedef enum /*_AMBA_IMG_CHANNEL_EFOV_CB_ID_*/ {
    IMG_CHAN_EFOV_CB_ID_AE = 0,
    IMG_CHAN_EFOV_CB_ID_ADJ,
    IMG_CHAN_EFOV_CB_ID_AWB,
    IMG_CHAN_EFOV_CB_ID_TOTAL
} AMBA_IMG_CHANNEL_EFOV_CB_ID_e;

typedef struct /*_AMBA_IMG_CHANNEL_s_*/ {
    UINT32                           Magic;
    AMBA_IMG_CHANNEL_CONTEXT_s       *pCtx;
    AMBA_IMG_CHANNEL_AIK_PROC_f      AikProc;
    AMBA_IMG_CHANNEL_AIK_CB_f        AikCb;
    AMBA_IMG_CHANNEL_PAIK_CB_f       PostAikCb;
    AMBA_IMG_CHANNEL_SOF_CB_f        SofCb;
    AMBA_IMG_CHANNEL_EOF_CB_f        EofCb;
    AMBA_IMG_CHANNEL_SLS_CB_f        SlsCb;
    AMBA_IMG_CHANNEL_TIMING_s        *pTiming;
    AMBA_IMG_CHANNEL_EFOV_TX_CB_f    EFovTxCb[IMG_CHAN_EFOV_CB_ID_TOTAL];
    AMBA_IMG_CHANNEL_EFOV_RX_CB_f    EFovRxCb[IMG_CHAN_EFOV_CB_ID_TOTAL];
    AMBA_IMG_CHANNEL_ISO_CB_f        IsoCb;
} AMBA_IMG_CHANNEL_s;

extern AMBA_IMG_CHANNEL_TIMING_PORT_s AmbaImgChannel_MofTimingPort[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSystem
\*---------------------------------------------------------------------------*/
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

typedef union /*_AMBA_IMG_SYSTEM_PAIK_IDX_u_*/ {
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

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgEvent
\*---------------------------------------------------------------------------*/
typedef struct /*_AMBA_IMG_EVENT_CONTEXT_s_*/ {
    AMBA_KAL_EVENT_FLAG_t    Group[2];
} AMBA_IMG_EVENT_CONTEXT_s;

typedef struct /*_AMBA_IMG_EVETN_CHAN_s_*/ {
    UINT32                      Magic;
    AMBA_IMG_EVENT_CONTEXT_s    *pCtx;
} AMBA_IMG_EVENT_CHAN_s;

typedef struct /*_AMBA_IMG_EVENT_VIN_s_*/ {
    AMBA_KAL_EVENT_FLAG_t    Group[2];
} AMBA_IMG_EVENT_VIN_s;

extern AMBA_IMG_EVENT_VIN_s AmbaImgEvent_Vin[AMBA_IMGFRW_NUM_VIN_CHANNEL];
extern AMBA_IMG_EVENT_CHAN_s *AmbaImgEvent_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*-----------------------------------------------------------------------------------------------*\
 * Definitons of AmbaImgMessage
\*-----------------------------------------------------------------------------------------------*/
typedef enum /*_AMBA_IMG_MESSAGE_ID_e_*/ {
    AMBA_IMG_MESSAGE_ID_SVR_CHG = 0,
    AMBA_IMG_MESSAGE_ID_TOTAL
} AMBA_IMG_MESSAGE_ID_e;

typedef UINT32 (*AMBA_IMG_MESSAGE_CB_FUNC_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT64 MsgId, UINT64 User);

typedef struct /*_AMBA_IMG_MESSAGE_CHAN_s_*/ {
    UINT32                        Magic;
    AMBA_IMG_MESSAGE_CB_FUNC_f    pFunc;
} AMBA_IMG_MESSAGE_CHAN_s;

extern AMBA_IMG_MESSAGE_CHAN_s *AmbaImgMessage_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSensorHAL
\*---------------------------------------------------------------------------*/
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
    AMBA_IMG_SENSOR_HAL_DATA_REG_s    In;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s    Act;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s    Last;
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

typedef struct /*_AMBA_IMG_SENSOR_HAL_HDR_FIFO_BUF_s_*/ {
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

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSensorSync
\*---------------------------------------------------------------------------*/
typedef enum /*_AMBA_IMG_SENSOR_SYNC_CMD_e_*/ {
    SYNC_CMD_ACK = 0,
    SYNC_CMD_FIFO,
    SYNC_CMD_DIRECT,
    SYNC_CMD_SNAP,
    SYNC_CMD_HDR_PRELOAD,
    SYNC_CMD_SOF,
    SYNC_CMD_MOF,
    SYNC_CMD_EOF
} AMBA_IMG_SENSOR_SYNC_CMD_e;

#define SYNC_ACK              ((UINT32) SYNC_CMD_ACK           << 24U)
#define SYNC_FIFO             ((UINT32) SYNC_CMD_FIFO          << 24U)
#define SYNC_DIRECT           ((UINT32) SYNC_CMD_DIRECT        << 24U)
#define SYNC_SNAP             ((UINT32) SYNC_CMD_SNAP          << 24U)
#define SYNC_HDR_PRELOAD      ((UINT32) SYNC_CMD_HDR_PRELOAD   << 24U)
#define SYNC_SOF              ((UINT32) SYNC_CMD_SOF           << 24U)
#define SYNC_MOF              ((UINT32) SYNC_CMD_MOF           << 24U)
#define SYNC_EOF              ((UINT32) SYNC_CMD_EOF           << 24U)

#define SYNC_HDR              0x000100U
#define SYNC_USER             0x000200U
#define SYNC_AIK              0x000400U
#define SYNC_PRE_AIK          0x000800U
#define SYNC_FORCE            0x800000U

#define SYNC_WB_ID_FE         0x001000U
#define SYNC_WB_ID_FE1        0x002000U
#define SYNC_WB_ID_FE2        0x003000U
#define SYNC_WB_ID_BE         0x004000U
#define SYNC_WB_ID_BE_ALT     0x005000U

#define SYNC_SLS              0x008000U

typedef union /*_AMBA_IMG_SENSOR_SYNC_CMD_MSG_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Msg:            24;
        UINT32  Cmd:             8;
    } Com;
    struct {
        UINT32  Count:           8;
        UINT32  Reserved:       16;
        UINT32  Cmd:             8;
    } Wait;
    struct {
        UINT32  Count:           8;
        UINT32  Reserved:       16;
        UINT32  Cmd:             8;
    } HdrPreload;
    struct {
        UINT32  Count:           8;
        UINT32  Hdr:             1;
        UINT32  Reserved:       15;
        UINT32  Cmd:             8;
    } Fifo;
    struct {
        UINT32  Count:           8;
        UINT32  Hdr:             1;
        UINT32  User:            1;
        UINT32  Aik:             1;
        UINT32  PreAik:          1;
        UINT32  WbId:            3;
        UINT32  Sls:             1;
        UINT32  Reserved:        7;
        UINT32  Force:           1;
        UINT32  Cmd:             8;
    } Video;
    struct {
        UINT32  Count:           8;
        UINT32  Hdr:             1;
        UINT32  User:            1;
        UINT32  Aik:             1;
        UINT32  PreAik:          1;
        UINT32  Reserved_L:      3;
        UINT32  Sls:             1;
        UINT32  Reserved_H:      7;
        UINT32  Force:           1;
        UINT32  Cmd:             8;
    } Snap;
} AMBA_IMG_SENSOR_SYNC_CMD_MSG_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_CMD_MSG_s_*/ {
    AMBA_IMG_SENSOR_SYNC_CMD_MSG_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_CMD_MSG_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_AGC_DATA_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Agc:        32;
        UINT32  Reserved:   32;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_AGC_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_AGC_DATA_s_*/ {
    AMBA_IMG_SENSOR_SYNC_AGC_DATA_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_AGC_DATA_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_DGC_DATA_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Dgc:        32;
        UINT32  DDgc:       32;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_DGC_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_DGC_DATA_s_*/ {
    AMBA_IMG_SENSOR_SYNC_DGC_DATA_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_DGC_DATA_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_SHR_DATA_u_*/ {
    UINT64 Data;
    struct {
        UINT32  Shr:        32;
        UINT32  Reserved:   32;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_SHR_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_SHR_DATA_s_*/ {
    AMBA_IMG_SENSOR_SYNC_SHR_DATA_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_SHR_DATA_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_WGC_DATA_u_*/ {
    UINT64 Data;
    struct {
        UINT64  Wgc;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_WGC_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_WGC_DATA_s_*/ {
    AMBA_IMG_SENSOR_SYNC_WGC_DATA_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_WGC_DATA_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_USR_DATA_u_*/ {
    UINT64 Data;
    struct {
        UINT64  User;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_USR_DATA_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_USR_DATA_s_*/ {
    AMBA_IMG_SENSOR_SYNC_USR_DATA_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_USR_DATA_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_DATA_s_*/ {
    AMBA_IMG_SENSOR_SYNC_AGC_DATA_s    Agc;
    AMBA_IMG_SENSOR_SYNC_DGC_DATA_s    Dgc;
    AMBA_IMG_SENSOR_SYNC_SHR_DATA_s    Shr;
    AMBA_IMG_SENSOR_SYNC_WGC_DATA_s    Wgc;
    AMBA_IMG_SENSOR_SYNC_USR_DATA_s    User;
} AMBA_IMG_SENSOR_SYNC_DATA_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_AIK_CB_u_*/ {
    UINT64 Msg;
    struct {
        UINT32  Reserved_L:    32;
        UINT32  Reserved_H:    31;
        UINT32  PreAik:         1;
    } Bits;
    struct {
        UINT32  Var:           32;
        UINT32  Id:             6;
        UINT32  Raw:            1;
        UINT32  AikExec:        1;
        UINT32  Adj:            1;
        UINT32  Awb:            1;
        UINT32  Ae:             1;
        UINT32  SofEnd:         1;
        UINT32  Reserved_H:    20;
    } Bits2;
} AMBA_IMG_SENSOR_SYNC_AIK_CB_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_AIK_CB_s_*/ {
    AMBA_IMG_SENSOR_SYNC_AIK_CB_u    Ctx;
} AMBA_IMG_SENSOR_SYNC_AIK_CB_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_FIFO_ACK_s_*/ {
    AMBA_KAL_SEMAPHORE_t    SsiSvr;
    AMBA_KAL_SEMAPHORE_t    Shr;
    AMBA_KAL_SEMAPHORE_t    Agc;
    AMBA_KAL_SEMAPHORE_t    Dgc;
} AMBA_IMG_SENSOR_SYNC_FIFO_ACK_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_FIFO_s_*/ {
    AMBA_KAL_MUTEX_t                   Mutex;
    AMBA_KAL_SEMAPHORE_t               Rdy;
    AMBA_IMG_SENSOR_SYNC_FIFO_ACK_s    Ack;
} AMBA_IMG_SENSOR_SYNC_FIFO_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_SNAP_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Count:        8;
        UINT32 Reserved:    23;
        UINT32 Active:       1;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_SNAP_OP_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_SNAP_INFO_s_*/ {
    AMBA_IMG_SENSOR_HAL_CTRL_REG_s    Ctrl;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s    Data;
    AMBA_IMG_SENSOR_HAL_HDR_DATA_s    HdrData;
} AMBA_IMG_SENSOR_SYNC_SNAP_INFO_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_SNAP_LINK_s_*/ {
    struct AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s_    *Up;
    struct AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s_    *Down;
} AMBA_IMG_SENSOR_SYNC_SNAP_LINK_s;

typedef struct AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s_ {
    UINT32                              Magic;
    UINT32                              Count;
    AMBA_IMG_SENSOR_SYNC_SNAP_INFO_s    Info;
    AMBA_IMG_SENSOR_SYNC_SNAP_LINK_s    Link;
} AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_SNAP_s_*/ {
    AMBA_KAL_MUTEX_t                    Mutex;
    AMBA_KAL_SEMAPHORE_t                Req;
    AMBA_IMG_SENSOR_SYNC_SNAP_OP_u      Op;
    AMBA_IMG_SENSOR_SYNC_SNAP_INFO_s    In;
    AMBA_IMG_SENSOR_SYNC_SNAP_INFO_s    Last;
    AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s    *pList;
} AMBA_IMG_SENSOR_SYNC_SNAP_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_USER_s_*/ {
    AMBA_KAL_MUTEX_t    Mutex;
} AMBA_IMG_SENSOR_SYNC_USER_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_CONTEXT_s_*/ {
    AMBA_IMG_SENSOR_HAL_HDR_DATA_s    HdrPreload;
    AMBA_IMG_SENSOR_SYNC_FIFO_s       Fifo;
    AMBA_IMG_SENSOR_SYNC_SNAP_s       Snap;
    AMBA_IMG_SENSOR_SYNC_USER_s       User;
} AMBA_IMG_SENSOR_SYNC_CONTEXT_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_CHAN_s_*/ {
    UINT32                            Magic;
    AMBA_IMG_SENSOR_SYNC_CONTEXT_s    *pCtx;
} AMBA_IMG_SENSOR_SYNC_CHAN_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_VIN_MUTEX_s_*/ {
    AMBA_KAL_MUTEX_t    Sof;
    AMBA_KAL_MUTEX_t    Eof;
} AMBA_IMG_SENSOR_SYNC_VIN_MUTEX_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_VIN_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t    Timer;
} AMBA_IMG_SENSOR_SYNC_VIN_SEM_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_WAIT_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32 Count:            8;
        UINT32 Reserved:        22;
        UINT32 Id:               2;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_WAIT_OP_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_WAIT_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t    Ack;
} AMBA_IMG_SENSOR_SYNC_WAIT_SEM_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_WAIT_LINK_s_*/ {
    struct AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s_ *Up;
    struct AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s_ *Down;
} AMBA_IMG_SENSOR_SYNC_WAIT_LINK_s;

typedef struct AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s_ {
    UINT32                              Magic;
    AMBA_IMG_CHANNEL_ID_s               ImageChanId;
    AMBA_IMG_SENSOR_SYNC_WAIT_OP_u      Op;
    AMBA_IMG_SENSOR_SYNC_WAIT_SEM_s     Sem;
    AMBA_IMG_SENSOR_SYNC_WAIT_LINK_s    Link;
} AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_WAIT_s_*/ {
    AMBA_KAL_MUTEX_t                    Mutex;
    AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s    *pList;
} AMBA_IMG_SENSOR_SYNC_WAIT_s;

typedef union /*_AMBA_IMG_SENSOR_SYNC_DEBUG_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Msg:          1;
        UINT32  Reserved:    31;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_DEBUG_u;

typedef union /*_AMBA_IMG_SENSOR_SYNC_VIN_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Reset:            1;
        UINT32  AikExec:          1;
        UINT32  Reserved:        30;
    } Bits;
} AMBA_IMG_SENSOR_SYNC_VIN_OP_u;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s_*/ {
    UINT32  Sof;
    UINT32  Eof;
    UINT32  Raw;
    UINT32  RawInMof;
    UINT32  RawInSof;
} AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_VIN_TIMETICK_s_*/ {
    UINT32  Sof;
    UINT32  LastSof;
    UINT32  Eof;
    UINT32  LastEof;
} AMBA_IMG_SENSOR_SYNC_VIN_TIMETICK_s;

typedef enum /*_AMBA_IMG_SENSOR_SYNC_WAIT_ID_e_*/ {
    SYNC_WAIT_SOF = 0,
    SYNC_WAIT_MOF,
    SYNC_WAIT_EOF,
    SYNC_WAIT_TOTAL
} AMBA_IMG_SENSOR_SYNC_WAIT_ID_e;

typedef UINT32 (*AMBA_IMG_SENSOR_SYNC_TIMING_CB_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr);

typedef struct /*_AMBA_IMG_SENSOR_SYNC_TIMING_s_*/ {
    AMBA_IMG_SENSOR_SYNC_TIMING_CB_f    pFunc;
    AMBA_KAL_MUTEX_t                    Mutex;
    AMBA_IMG_CHANNEL_TIMING_PORT_s      *pSofList;
    AMBA_IMG_CHANNEL_TIMING_PORT_s      *pEofList;
} AMBA_IMG_SENSOR_SYNC_TIMING_s;

typedef struct /*_AMBA_IMG_SENSOR_SYNC_VIN_s_*/ {
    AMBA_IMG_SENSOR_SYNC_VIN_MUTEX_s       Mutex;
    AMBA_IMG_SENSOR_SYNC_VIN_SEM_s         Sem;
    AMBA_IMG_SENSOR_SYNC_DEBUG_u           Debug;
    AMBA_IMG_SENSOR_SYNC_VIN_OP_u          Op;
    AMBA_IMG_SENSOR_SYNC_WAIT_s            Wait[SYNC_WAIT_TOTAL];
    AMBA_IMG_SENSOR_SYNC_VIN_COUNTER_s     Counter;
    AMBA_IMG_SENSOR_SYNC_VIN_TIMETICK_s    Timetick;
    AMBA_IMG_SENSOR_SYNC_TIMING_s          Timing;
} AMBA_IMG_SENSOR_SYNC_VIN_s;

extern AMBA_IMG_SENSOR_SYNC_VIN_s AmbaImgSensorSync_Vin[AMBA_IMGFRW_NUM_VIN_CHANNEL];
extern AMBA_IMG_SENSOR_SYNC_CHAN_s *AmbaImgSensorSync_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgStatistics
\*---------------------------------------------------------------------------*/
typedef enum /*_AMBA_IMG_STATISTICS_CMD_e_*/ {
    STAT_CMD_NO_WAIT = 0,
    STAT_CMD_WAIT_FOREVER,
    STAT_CMD_WAIT_TIME
} AMBA_IMG_STATISTICS_CMD_e;

typedef enum /*_AMBA_IMG_STATISTICS_e_*/ {
    STAT_NO_WAIT         = ((UINT32) STAT_CMD_NO_WAIT         << 24U),
    STAT_WAIT_FOREVER    = ((UINT32) STAT_CMD_WAIT_FOREVER    << 24U),
    STAT_WAIT_TIME       = ((UINT32) STAT_CMD_WAIT_TIME       << 24U)
} AMBA_IMG_STATISTICS_e;

/* compatible definition */
#define REQUEST_NO_WAIT         ((UINT32) STAT_NO_WAIT)
#define REQUEST_WAIT_FOREVER    ((UINT32) STAT_WAIT_FOREVER)

typedef union /*_AMBA_IMG_STATISTICS_CMD_MSG_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Msg:         24;
        UINT32  Cmd:          8;
    } Bits;
} AMBA_IMG_STATISTICS_CMD_MSG_u;

typedef struct /*_AMBA_IMG_STATISTICS_CMD_MSG_s_*/ {
    AMBA_IMG_STATISTICS_CMD_MSG_u    Ctx;
} AMBA_IMG_STATISTICS_CMD_MSG_s;

typedef union /*_AMBA_IMG_STATISTICS_CHAN_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Enable:          1;
        UINT32  Reserved:       31;
    } Bits;
} AMBA_IMG_STATISTICS_CHAN_OP_u;

typedef union /*_AMBA_IMG_STATISTICS_COUNTER_u_*/ {
    UINT64  Data;
    struct {
        UINT32  Num:              32;
        UINT32  Reserved_H:       31;
        UINT32  Sig:               1;
    } Bits;
} AMBA_IMG_STATISTICS_COUNTER_u;

typedef struct /*_AMBA_IMG_STATISTICS_ZONE_ID_s_*/ {
    UINT32  Req;
    UINT32  Acc;
} AMBA_IMG_STATISTICS_ZONE_ID_s;

typedef struct /*_AMBA_IMG_STATISTICS_SEM_s_*/ {
    AMBA_KAL_SEMAPHORE_t  Req;
    AMBA_KAL_SEMAPHORE_t  Rdy;
} AMBA_IMG_STATISTICS_SEM_s;

typedef void (*AMBA_IMG_STATISTICS_FUNC_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const void **pCfa, const void **pRgb, UINT32 Counter);

typedef struct /*_AMBA_IMG_STATISTICS_PROC_s_*/ {
    AMBA_IMG_STATISTICS_FUNC_f       pFunc;
} AMBA_IMG_STATISTICS_PROC_s;

typedef struct /*_AMBA_IMG_STATISTICS_DATA_s_*/ {
    const void **pCfa;
    const void **pRgb;
} AMBA_IMG_STATISTICS_DATA_s;

typedef struct /*_AMBA_IMG_STATISTICS_LINK_s_*/ {
    struct AMBA_IMG_STATISTICS_PORT_s_    *Up;
    struct AMBA_IMG_STATISTICS_PORT_s_    *Down;
} AMBA_IMG_STATISTICS_LINK_s;

typedef struct AMBA_IMG_STATISTICS_PORT_s_ {
    UINT32                            Magic;
    AMBA_IMG_CHANNEL_ID_s             ImageChanId;
    AMBA_IMG_STATISTICS_COUNTER_u     Counter;
    AMBA_IMG_STATISTICS_ZONE_ID_s     ZoneId;
    AMBA_IMG_STATISTICS_SEM_s         Sem;
    AMBA_IMG_STATISTICS_PROC_s        Proc;
    AMBA_IMG_STATISTICS_DATA_s        Data;
    AMBA_IMG_SENSOR_HAL_DATA_REG_s    Reg;
    AMBA_IMG_STATISTICS_LINK_s        Link;
} AMBA_IMG_STATISTICS_PORT_s;

typedef struct /*_AMBA_IMG_STATISTICS_CONTEXT_s_*/ {
    AMBA_KAL_MUTEX_t                 Mutex;
    AMBA_IMG_STATISTICS_CHAN_OP_u    Op;
    AMBA_IMG_STATISTICS_PORT_s       *pList;
} AMBA_IMG_STATISTICS_CONTEXT_s;

typedef struct /*_AMBA_IMG_STATISTICS_CHAN_s_*/ {
    UINT32                           Magic;
    AMBA_IMG_STATISTICS_CONTEXT_s    *pCtx;
} AMBA_IMG_STATISTICS_CHAN_s;

typedef union /*_AMBA_IMG_STATISTICS_DEBUG_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Msg:          1;
        UINT32  Timing:       1;
        UINT32  Reserved:    30;
    } Bits;
} AMBA_IMG_STATISTICS_DEBUG_u;

typedef union /*_AMBA_IMG_STATISTICS_VIN_OP_u_*/ {
    UINT32 Data;
    struct {
        UINT32  Reset:       1;
        UINT32  Inter:       1;
        UINT32  Reserved:   30;
    } Bits;
} AMBA_IMG_STATISTICS_VIN_OP_u;

typedef struct /*_AMBA_IMG_STATISTICS_VIN_STATUS_s_*/ {
    UINT32  ZoneId;
    UINT32  Counter;
} AMBA_IMG_STATISTICS_VIN_STATUS_s;

typedef struct /*_AMBA_IMG_STATISTICS_VIN_s_*/ {
    AMBA_IMG_STATISTICS_DEBUG_u         Debug;
    AMBA_IMG_STATISTICS_VIN_OP_u        Op;
    AMBA_IMG_STATISTICS_VIN_STATUS_s    Status;
} AMBA_IMG_STATISTICS_VIN_s;

extern AMBA_IMG_STATISTICS_VIN_s AmbaImgStatistics_Vin[AMBA_IMGFRW_NUM_VIN_CHANNEL];
extern AMBA_IMG_STATISTICS_CHAN_s *AmbaImgStatistics_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgSensorDrv
\*---------------------------------------------------------------------------*/
#define AMBA_NUM_SWB_CHANNEL    4U

typedef struct /*_AMBA_IMG_SENSOR_WB_s_*/ {
    UINT32 Gain[AMBA_NUM_SWB_CHANNEL];
} AMBA_IMG_SENSOR_WB_s;

typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_REG_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT8 *pBuf, UINT8 Size);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_AGC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pAgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_DGC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pDgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SHR_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SVR_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSvr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_MSC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pMsc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SLS_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pSls);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_WGC_WRITE_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT64 *pWgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_GAIN_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pAgc, UINT32 *pDgc, AMBA_IMG_SENSOR_WB_s *pWgc);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SHUTTER_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pShr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SVR_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pShr, UINT32 *pSvr);
typedef  UINT32 (*AMBA_IMG_SENSOR_DRV_SLS_CONVERT_f)(AMBA_IMG_CHANNEL_ID_s ImageChanId, const UINT32 *pFactor, UINT32 *pSls);

typedef struct /*_AMBA_IMG_SENSOR_DRV_FUNC_s_*/ {
    AMBA_IMG_SENSOR_DRV_REG_WRITE_f          RegWrite;
    AMBA_IMG_SENSOR_DRV_AGC_WRITE_f          AgcWrite;
    AMBA_IMG_SENSOR_DRV_DGC_WRITE_f          DgcWrite;
    AMBA_IMG_SENSOR_DRV_SHR_WRITE_f          ShrWrite;
    AMBA_IMG_SENSOR_DRV_SVR_WRITE_f          SvrWrite;
    AMBA_IMG_SENSOR_DRV_MSC_WRITE_f          MscWrite;
    AMBA_IMG_SENSOR_DRV_SLS_WRITE_f          SlsWrite;
    AMBA_IMG_SENSOR_DRV_WGC_WRITE_f          WgcWrite;
    AMBA_IMG_SENSOR_DRV_GAIN_CONVERT_f       GainConvert;
    AMBA_IMG_SENSOR_DRV_SHUTTER_CONVERT_f    ShutterConvert;
    AMBA_IMG_SENSOR_DRV_SVR_CONVERT_f        SvrConvert;
    AMBA_IMG_SENSOR_DRV_SLS_CONVERT_f        SlsConvert;
} AMBA_IMG_SENSOR_DRV_FUNC_s;

typedef struct /*_AMBA_IMG_SENSOR_DRV_CHAN_s_*/ {
    UINT32                        Magic;
    AMBA_IMG_SENSOR_DRV_FUNC_s    *pFunc;
} AMBA_IMG_SENSOR_DRV_CHAN_s;

extern AMBA_IMG_SENSOR_DRV_CHAN_s *AmbaImgSensorDrv_Chan[AMBA_IMGFRW_NUM_VIN_CHANNEL];

/*---------------------------------------------------------------------------*\
 * Definitions of AmbaImgFramework_ChanTable
\*---------------------------------------------------------------------------*/
#define AMBA_IMG_NUM_VIN_CHANNEL    AMBA_IMGFRW_NUM_VIN_CHANNEL
#define AMBA_IMG_NUM_VIN_SENSOR     AMBA_IMGFRW_NUM_VIN_SENSOR
#define AMBA_IMG_NUM_VIN_ALGO       AMBA_IMGFRW_NUM_VIN_ALGO
#define AMBA_IMG_NUM_FOV_CHANNEL    AMBA_IMGFRW_NUM_FOV_CHANNEL

extern AMBA_IMG_SYSTEM_INFO_s AmbaImgSystem_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_INFO_s AmbaImgSensorHAL_Info[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_HDR_INFO_s AmbaImgSensorHAL_HdrInfo[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern AMBA_IMG_SENSOR_HAL_AUX_s AmbaImgSensorHAL_AuxInfo[AMBA_IMG_NUM_VIN_CHANNEL];
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_Entry[AMBA_IMG_NUM_VIN_CHANNEL];
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_EntryTable[AMBA_IMG_NUM_VIN_CHANNEL][AMBA_IMG_NUM_VIN_ALGO];
extern UINT32 AmbaImgMain_ChanMemInit(void);
extern AMBA_IMG_CHANNEL_s *AmbaImgChannel_UserEntry(UINT32 VinId, UINT32 ChainIndex);
AMBA_IMG_CHANNEL_s **AmbaImgChannel_Select(UINT32 VinId, UINT32 ChainIndex);
UINT32 AmbaImgChannel_SensorIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SensorId);
UINT32 AmbaImgChannel_AlgoIdPut(UINT32 VinId, UINT32 ChainId, UINT32 AlgoId);
UINT32 AmbaImgChannel_ZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneId);
UINT32 AmbaImgChannel_ZoneMsbIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneMsbId);
UINT32 AmbaImgChannel_VrIdPut(UINT32 VinId, UINT32 ChainId, UINT32 VrId, UINT32 VrAltId);
UINT32 AmbaImgChannel_InterIdPut(UINT32 VinId, UINT32 ChainId, UINT32 InterId, UINT32 InterNum, UINT32 SkipFrame, UINT32 Mode);
UINT32 AmbaImgChannel_PostZoneIdPut(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId);
UINT32 AmbaImgChannel_PostZoneIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 PostZoneId);
UINT32 AmbaImgChannel_ZoneIsoDisIdPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId);
UINT32 AmbaImgChannel_ZoneIsoDisIdAdd(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIsoDisId);
UINT32 AmbaImgChannel_ZoneIsoCb(UINT32 VinId, UINT32 ChainId, AMBA_IMG_CHANNEL_ISO_CB_f pFunc);
UINT32 AmbaImgChannel_SnapAebIdPut(UINT32 VinId, UINT32 ChainId, UINT32 SnapAebId);
UINT32 AmbaImgChannel_SnapAebIdGet(UINT32 VinId, UINT32 ChainId, UINT32 *pSnapAebId);
UINT32 AmbaImgChannel_PipeOutPut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Width, UINT32 Height);
UINT32 AmbaImgChannel_PipeOutGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pWidth, UINT32 *pHeight);
UINT32 AmbaImgChannel_PipeModePut(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 Mode);
UINT32 AmbaImgChannel_PipeModeGet(UINT32 VinId, UINT32 ChainId, UINT32 ZoneIdx, UINT32 *pMode);
UINT32 AmbaImgChannel_EFovIdPut(UINT32 VinId, UINT32 ChainId, UINT64 EFovId);
UINT32 AmbaImgChannel_EFovIdGet(UINT32 VinId, UINT32 ChainId, UINT64 *pEFovId);
#else
#include "AmbaImgFramework.h"
#endif

#else

#include "AmbaImgFramework.h"

#endif

#endif  /* AMBA_IMG_FRW_CHAN_H */
