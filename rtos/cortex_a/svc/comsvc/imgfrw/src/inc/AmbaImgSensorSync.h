/**
 *  @file AmbaImgSensorSync.h
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
 *  @details Constants and Definitions for Amba Image Sensor Sync
 *
 */

#ifndef AMBA_IMG_SENSOR_SYNC_H
#define AMBA_IMG_SENSOR_SYNC_H

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
        UINT32  Reserved0:       3;
        UINT32  Sls:             1;
        UINT32  Reserved1:       7;
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

typedef struct /*_AMBA_IMG_SENSOR_SYNC_SNAP_DATA_s_*/ {
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
    SYNC_WAIT_SOF = 0L,
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

UINT32 AmbaImgSensorSync_Init(void);
UINT32 AmbaImgSensorSync_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgSensorSync_Reset(UINT32 VinId);
UINT32 AmbaImgSensorSync_Stop(UINT32 VinId);

UINT32 AmbaImgSensorSync_RawCapSeqPut(UINT32 VinId, UINT32 RawCapSeq);
UINT32 AmbaImgSensorSync_TimingCb(UINT32 VinId, AMBA_IMG_SENSOR_SYNC_TIMING_CB_f pFunc);

UINT32 AmbaImgSensorSync_UserBuffer(AMBA_IMG_CHANNEL_ID_s ImageChanId, UINT32 Size, UINT32 Count, UINT8 *pMem);
 void *AmbaImgSensorSync_UserBufferGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);

UINT32 AmbaImgSensorSync_Sof(UINT32 VinId);
UINT32 AmbaImgSensorSync_Eof(UINT32 VinId);

UINT32 AmbaImgSensorSync_Wait(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort, UINT32 Mode);
UINT32 AmbaImgSensorSync_WaitPush(AMBA_IMG_SENSOR_SYNC_WAIT_PORT_s *pPort);

UINT32 AmbaImgSensorSync_Request(AMBA_IMG_CHANNEL_ID_s ImageChanId, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);
UINT32 AmbaImgSensorSync_SnapSequence(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_SENSOR_SYNC_SNAP_PORT_s *pPort, const AMBA_IMG_SENSOR_SYNC_DATA_s *pSyncData, UINT32 Mode);

#endif  /* AMBA_IMG_SENSOR_SYNC_H */
