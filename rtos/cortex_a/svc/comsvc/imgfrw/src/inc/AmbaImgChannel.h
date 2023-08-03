/**
 *  @file AmbaImgChannel.h
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
 *  @details Constants and Definitions for Amba Image Channel
 *
 */

#ifndef AMBA_IMG_CHANNEL_H
#define AMBA_IMG_CHANNEL_H

#ifndef AMBA_DSP_CAPABILITY_H
#include "AmbaDSP_Capability.h"
#endif

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
    const char                       *pName;
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

  void AmbaImgChannel_Register(AMBA_IMG_CHANNEL_s **pImageChannel);
UINT32 AmbaImgChannel_TotalZoneIdGet(UINT32 VinId);
UINT32 AmbaImgChannel_VrMap(UINT32 VinId, AMBA_IMG_CHANNEL_s * const *pImageChannel);

#endif  /* AMBA_IMG_CHANNEL_H */
