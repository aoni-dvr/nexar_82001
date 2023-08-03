/**
 *  @file AmbaImgStatistics.h
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
 *  @details Constants and Definitions for Amba Image Statistics
 *
 */

#ifndef AMBA_IMG_STATISTICS_H
#define AMBA_IMG_STATISTICS_H

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

UINT32 AmbaImgStatistics_Init(void);
UINT32 AmbaImgStatistics_Debug(UINT32 VinId, UINT32 Flag);
UINT32 AmbaImgStatistics_Reset(UINT32 VinId);
UINT32 AmbaImgStatistics_Inter(UINT32 VinId, UINT32 Enable);

UINT32 AmbaImgStatistics_Entry(UINT32 VinId, UINT32 ZoneId, const void **pCfa, const void **pRgb, UINT32 RawCapSeq);
UINT32 AmbaImgStatistics_Push(UINT32 VinId);

UINT32 AmbaImgStatistics_ZoneIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgStatistics_GZoneIdGet(AMBA_IMG_CHANNEL_ID_s ImageChanId);
UINT32 AmbaImgStatistics_TZoneIdGet(UINT32 VinId);

UINT32 AmbaImgStatistics_OpenEx(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_STATISTICS_PORT_s *pPort, AMBA_IMG_STATISTICS_FUNC_f pFunc);
UINT32 AmbaImgStatistics_Open(AMBA_IMG_CHANNEL_ID_s ImageChanId, AMBA_IMG_STATISTICS_PORT_s *pPort);
UINT32 AmbaImgStatistics_Request(AMBA_IMG_STATISTICS_PORT_s *pPort, UINT32 Flag);
UINT32 AmbaImgStatistics_Close(AMBA_IMG_STATISTICS_PORT_s *pPort);

#endif  /* AMBA_IMG_STATISTICS_H */
