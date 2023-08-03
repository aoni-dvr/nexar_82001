/**
 *  @file AmbaMonListenDsp.c
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
 *  @details Amba Monitor Listen DSP
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"
#include "AmbaWrap.h"
#ifndef CONFIG_QNX
#include "AmbaRTSL_PLL.h"
#else
#include "AmbaSYS.h"
#endif

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"
#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"
#include "AmbaMonDsp.h"

#include "AmbaMonListenDsp.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_LISTEN_DSP_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR              Data;
    const void                          *pCvoid;
    void                                *pVoid;
    AMBA_MON_MESSAGE_PORT_s             *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s           *pMsgHead;
    UINT8                               *pUint8;
    AMBA_DSP_RAW_DATA_RDY_s             *pRawRdy;
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s      *pYuvRdy;
    AMBA_DSP_YUV_DATA_RDY_EXTEND_s      *pDispRdy;
    AMBA_DSP_VOUT_DATA_INFO_s           *pVoutStatus;
    AMBA_DSP_VIDEO_PATH_INFO_s          *pVideoPath;
    AMBA_MON_DSP_RAW_STATE_s            *pRawState;
    AMBA_MON_DSP_FOV_STATE_s            *pFovState;
    AMBA_MON_DSP_VOUT_STATE_s           *pVoutState;
    AMBA_MON_DSP_VIN_TIMEOUT_STATE_s    *pVinTimeoutState;
} AMBA_MON_LISTEN_DSP_MEM_u;

typedef struct /*_AMBA_MON_LISTEN_DSP_MEM_s_*/ {
    AMBA_MON_LISTEN_DSP_MEM_u    Ctx;
} AMBA_MON_LISTEN_DSP_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonListenDspTaskId;
static void AmbaMonListenDsp_Task(UINT32 Param);
static void *AmbaMonListenDsp_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenDsp_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenDsp_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenDsp_Ack;

static UINT32 AmbaMonListenDsp_Cmd = (UINT32) LISTEN_DSP_TASK_CMD_STOP;

/* dsp state message port (raw/vout) */
static AMBA_MON_MESSAGE_PORT_s RawStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s RawTimeoutPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s DefRawStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s FovStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s FovTimeoutPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s VoutStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s VoutTimeoutPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s VinTimeoutPort GNU_SECTION_NOZEROINIT;

/* vin state message prot */
static AMBA_MON_MESSAGE_PORT_s VinRawStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s VinRawTimeoutPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s VinDefRawStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s VinDspVinTimeoutPort GNU_SECTION_NOZEROINIT;

static AMBA_MON_LISTEN_DSP_STATE_s LastDspRawState[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static AMBA_MON_LISTEN_DSP_STATE_s LastDspDefRawState[AMBA_MON_NUM_VIN_CHANNEL] = {0};
static AMBA_MON_LISTEN_DSP_STATE_s LastDspFovState[AMBA_MON_NUM_FOV_CHANNEL] = {0};
static AMBA_MON_LISTEN_DSP_STATE_s LastDspVoutState[AMBA_MON_NUM_VOUT_CHANNEL] = {0};
static AMBA_MON_LISTEN_DSP_STATE_s LastDspVinTimeoutState[AMBA_MON_NUM_VIN_CHANNEL] = {0};

/* dsp raw/fov/disp msg ring */
static AMBA_MON_LISTEN_DSP_MSG_s AmbaMonListenDsp_MsgRing GNU_SECTION_NOZEROINIT;
static UINT32 AmbaMonListenDsp_LastDispRingIndex[AMBA_MON_NUM_VOUT_CHANNEL] = {0};

static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenDsp_RawRepeat[AMBA_MON_NUM_VIN_CHANNEL];
static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenDsp_YuvRepeat[AMBA_MON_NUM_FOV_CHANNEL];
static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenDsp_YuvAltRepeat[AMBA_MON_NUM_FOV_CHANNEL];
static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenDsp_DispYuvRepeat[AMBA_MON_NUM_DISP_CHANNEL];
static UINT32 GNU_SECTION_NOZEROINIT  AmbaMonListenDsp_VoutYuvRepeat[AMBA_MON_NUM_VOUT_CHANNEL];

/**
 *  @private
 *  Amba monitor listen dsp create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenDsp_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_LISTEN_DSP_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonListenDspTaskStack[AMBA_MON_LISTEN_DSP_TASK_STACK_SIZE];
    static char AmbaMonListenDspTaskName[] = "MonitorLsnDsp";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenDsp_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenDsp_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenDsp_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create listen dsp task");
        {
            AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonListenDspTaskId,
                                             AmbaMonListenDspTaskName,
                                             Priority,
                                             AmbaMonListenDsp_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonListenDspTaskStack,
                                             AMBA_MON_LISTEN_DSP_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonListenDspTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonListenDspTaskId);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
        } else {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen dsp delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenDsp_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonListenDspTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonListenDspTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenDsp_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenDsp_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenDsp_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen dsp active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenDsp_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonListenDsp_Cmd == (UINT32) LISTEN_DSP_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenDsp_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonListenDsp_Cmd = (UINT32) LISTEN_DSP_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenDsp_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenDsp_Ack, AMBA_KAL_WAIT_FOREVER);
                if (FuncRetCode != KAL_ERR_NONE) {
                    /* ack fail */
                    RetCode = NG_UL;
                }
            } else {
                /* go fail */
                RetCode = NG_UL;
            }
        } else {
            /* not ready */
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen dsp inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenDsp_Inactive(void)
{
    if (AmbaMonListenDsp_Cmd == (UINT32) LISTEN_DSP_TASK_CMD_START) {
        AmbaMonListenDsp_Cmd = (UINT32) LISTEN_DSP_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor listen dsp idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenDsp_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenDsp_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenDsp_Ready);
        if (FuncRetCode != KAL_ERR_NONE) {
            /* ready give fail */
            RetCode = NG_UL;
        }
    } else {
        /* ready take fail */
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  Amba monitor listen dsp raw state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pRawState pointer to the raw state information
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_RawStatePut(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_RAW_STATE_s *pRawState, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 RawStateBuf[256];
#else
    UINT8 RawStateBuf[32];
#endif
    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_DSP_RAW_STATE_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* msg head get */
    MemInfo.Ctx.pUint8 = RawStateBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;
#endif
    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    pMsgHead->Ctx.Chunk.Timeout = TimeoutFlag;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;

    MemInfo.Ctx.pRawState = pRawState;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, MemInfo.Ctx.pCvoid, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = MemInfo.Ctx.pCvoid;
#endif
    /* msg put */
    if (pRawState->VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = RawStateBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(pPort, MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(pPort, MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  Amba monitor listen dsp raw start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pRawState pointer to the raw state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_RawStart(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_RAW_STATE_s *pRawState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_RawStatePut(pPort, pRawState, 0);
    return RetCode;
}

/**
 *  Amba monitor listen dsp raw timeout
 *  @param[in] pPort pointer to the message port
 *  @param[in] pRawState pointer to the raw state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_RawTimeout(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_RAW_STATE_s *pRawState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_RawStatePut(pPort, pRawState, 1);
    return RetCode;
}

/**
 *  Amba monitor listen dsp vin timeout put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVinTimeoutState pointer to the vin timeout state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_VinTimeoutPut(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_VIN_TIMEOUT_STATE_s *pVinTimeoutState)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 VinStateBuf[256];
#else
    UINT8 VinStateBuf[32];
#endif
    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_DSP_VIN_TIMEOUT_STATE_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* msg head get */
    MemInfo.Ctx.pUint8 = VinStateBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;
#endif
    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    pMsgHead->Ctx.Chunk.Timeout = 0U;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;

    MemInfo.Ctx.pVinTimeoutState = pVinTimeoutState;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, MemInfo.Ctx.pCvoid, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = MemInfo.Ctx.pCvoid;
#endif
    /* msg put */
    if (pVinTimeoutState->VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = VinStateBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(pPort, MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(pPort, MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  Amba monitor listen dsp vin timeout start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVinTimeoutState pointer to the vin timeout state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_VinTimeoutStart(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_VIN_TIMEOUT_STATE_s *pVinTimeoutState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_VinTimeoutPut(pPort, pVinTimeoutState);
    return RetCode;
}

/**
 *  Amba monitor listen dsp fov state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pFovState pointer to the fov state information
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_FovStatePut(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_FOV_STATE_s *pFovState, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 FovStateBuf[256];
#else
    UINT8 FovStateBuf[32];
#endif
    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_DSP_FOV_STATE_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* msg head get */
    MemInfo.Ctx.pUint8 = FovStateBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;
#endif
    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    pMsgHead->Ctx.Chunk.Timeout = TimeoutFlag;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;

    MemInfo.Ctx.pFovState = pFovState;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, MemInfo.Ctx.pCvoid, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = MemInfo.Ctx.pCvoid;
#endif
    /* msg put */
    if (pFovState->FovId < AMBA_MON_NUM_FOV_CHANNEL) {
        MemInfo.Ctx.pUint8 = FovStateBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(pPort, MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(pPort, MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  Amba monitor listen dsp fov start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pFovState pointer to the fov state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_FovStart(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_FOV_STATE_s *pFovState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_FovStatePut(pPort, pFovState, 0);
    return RetCode;
}

/**
 *  Amba monitor listen dsp fov timeout
 *  @param[in] pPort pointer to the message port
 *  @param[in] pFovState pointer to the fov state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_FovTimeout(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_FOV_STATE_s *pFovState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_FovStatePut(pPort, pFovState, 1);
    return RetCode;
}

/**
 *  Amba monitor listen dsp vout state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVoutState pointer to the vout state information
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_VoutStatePut(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_VOUT_STATE_s *pVoutState, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 VoutStateBuf[256];
#else
    UINT8 VoutStateBuf[32];
#endif
    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_DSP_VOUT_STATE_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* msg head get */
    MemInfo.Ctx.pUint8 = VoutStateBuf;
    pMsgHead = MemInfo.Ctx.pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data get */
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);
    pMsgData = MemInfo.Ctx.pVoid;
#endif
    /* msg header reset */
    pMsgHead->Ctx.Data[0] = 0ULL;
    pMsgHead->Ctx.Data[1] = 0ULL;
    pMsgHead->Ctx.Data[2] = 0ULL;

    /* msg header */
    pMsgHead->Ctx.Chunk.Id = (UINT8) AMBA_MON_MSG_ID_DATA;
    pMsgHead->Ctx.Chunk.Size = (UINT8) MsgLength;
    pMsgHead->Ctx.Chunk.Timeout = TimeoutFlag;
    pMsgHead->Ctx.Chunk.Timetick = Timetick;

    MemInfo.Ctx.pVoutState = pVoutState;
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, MemInfo.Ctx.pCvoid, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = MemInfo.Ctx.pCvoid;
#endif
    /* msg put */
    if (pVoutState->VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
        MemInfo.Ctx.pUint8 = VoutStateBuf;
#ifndef AMBA_MON_MESSAGE_PUT2
        pMem = AmbaMonMessage_Put(pPort, MemInfo.Ctx.pVoid, (UINT32) pMsgHead->Ctx.Chunk.Size);
#else
        pMem = AmbaMonMessage_Put2(pPort, MemInfo.Ctx.pVoid, HeaderSize, pMsgData, DataSize);
#endif
        if (pMem == NULL) {
            RetCode = NG_UL;
        }
    }

    return RetCode;
}

/**
 *  Amba monitor listen dsp vout start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVoutState pointer to the vout state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_VoutStart(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_VOUT_STATE_s *pVoutState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_VoutStatePut(pPort, pVoutState, 0);
    return RetCode;
}

/**
 *  Amba monitor listen dsp vout timeout
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVoutState pointer to the vout state information
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenDsp_VoutTimeout(AMBA_MON_MESSAGE_PORT_s *pPort, AMBA_MON_DSP_VOUT_STATE_s *pVoutState)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenDsp_VoutStatePut(pPort, pVoutState, 1);
    return RetCode;
}

/**
 *  Amba monitor listen dsp message process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen dsp event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenDsp_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_DSP_EVENT_s *pListenEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->Raw.Flag) {
        /* dsp raw */
        AMBA_MON_DSP_RAW_STATE_s RawState;
        RawState.VinId = MemInfo.Ctx.pRawRdy->VinId;
        RawState.SeqNum = MemInfo.Ctx.pRawRdy->CapSequence;
        RawState.Pts = MemInfo.Ctx.pRawRdy->CapPts;
        /* listen raw ring put */
        if (RawState.VinId < AMBA_MON_NUM_VIN_CHANNEL) {
            UINT32 RawRingIndex = (UINT32) (RawState.SeqNum % AMBA_MON_NUM_RING_CHANNEL);
            /* listen raw ring put */
            FuncRetCode = AmbaWrap_memcpy(&(AmbaMonListenDsp_MsgRing.RawInfo[RawState.VinId][RawRingIndex]), MemInfo.Ctx.pRawRdy, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
        /* dsp raw state update */
        if ((RawState.VinId < AMBA_MON_NUM_VIN_CHANNEL) && (LastDspRawState[RawState.VinId].Msg != (UINT32) DSP_STATE_START)) {
            FuncRetCode = AmbaMonListenDsp_RawStart(&RawStartPort, &RawState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaMonListenDsp_RawStart(&VinRawStartPort, &RawState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastDspRawState[RawState.VinId].Msg = (UINT32) DSP_STATE_START;
            LastDspDefRawState[RawState.VinId].Msg = (UINT32) DSP_STATE_IDLE;
            LastDspVinTimeoutState[RawState.VinId].Msg = (UINT32) DSP_STATE_IDLE;
        }
        /* dsp raw repeat check */
        if ((RawState.VinId < AMBA_MON_NUM_VIN_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenDsp_RawRepeat[RawState.VinId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp raw repeat", RawState.VinId, 10U, AmbaMonListenDsp_RawRepeat[RawState.VinId], 16U);
        }
    } else if (pListenEvent->Flag == pListenEvent->DefRaw.Flag) {
        /* dsp def raw */
        AMBA_MON_DSP_RAW_STATE_s RawState;
        RawState.VinId = MemInfo.Ctx.pRawRdy->VinId;
        RawState.SeqNum = MemInfo.Ctx.pRawRdy->CapSequence;
        RawState.Pts = MemInfo.Ctx.pRawRdy->CapPts;
        /* listen raw ring put */
        if (RawState.VinId < AMBA_MON_NUM_VIN_CHANNEL) {
            UINT32 RawRingIndex = (UINT32) (RawState.SeqNum % AMBA_MON_NUM_RING_CHANNEL);
            /* listen raw ring put */
            FuncRetCode = AmbaWrap_memcpy(&(AmbaMonListenDsp_MsgRing.RawInfo[RawState.VinId][RawRingIndex]), MemInfo.Ctx.pRawRdy, sizeof(AMBA_DSP_RAW_DATA_RDY_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
        /* dsp def raw state update */
        if ((RawState.VinId < AMBA_MON_NUM_VIN_CHANNEL) && (LastDspRawState[RawState.VinId].Msg != (UINT32) DSP_STATE_START)) {
            if (LastDspDefRawState[RawState.VinId].Msg == (UINT32) DSP_STATE_IDLE) {
                FuncRetCode = AmbaMonListenDsp_RawStart(&DefRawStartPort, &RawState);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaMonListenDsp_RawStart(&VinDefRawStartPort, &RawState);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                LastDspDefRawState[RawState.VinId].Msg = (UINT32) DSP_STATE_START;
            }
        }
    } else if (pListenEvent->Flag == pListenEvent->Yuv.Flag) {
        /* yuv */
        UINT32 FovId = MemInfo.Ctx.pYuvRdy->ViewZoneId;
        UINT64 FovSeqNum = MemInfo.Ctx.pYuvRdy->YuvSequence;
        /* listen yuv ring put */
        if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
            UINT32 FovRingIndex = (UINT32) (FovSeqNum % AMBA_MON_NUM_RING_CHANNEL);
            FuncRetCode = AmbaWrap_memcpy(&(AmbaMonListenDsp_MsgRing.YuvInfo[FovId][FovRingIndex]), MemInfo.Ctx.pYuvRdy, sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
        /* dsp fov repeat check */
        if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenDsp_YuvRepeat[FovId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp fov repeat", FovId, 10U, AmbaMonListenDsp_YuvRepeat[FovId], 16U);
        }
    } else if (pListenEvent->Flag == pListenEvent->YuvAlt.Flag) {
        /* alt yuv */
        UINT32 VinId = MemInfo.Ctx.pYuvRdy->VinId;
        UINT32 FovId = MemInfo.Ctx.pYuvRdy->ViewZoneId;
        UINT64 FovSeqNum = MemInfo.Ctx.pYuvRdy->YuvSequence;
        AMBA_MON_DSP_FOV_STATE_s FovState;
        FovState.FovId = FovId;
        FovState.YuvSeqNum = FovSeqNum;
        FovState.Latency = 0U;
        /* listen yuv alt ring put */
        if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
            UINT32 FovRingIndex = (UINT32) (FovSeqNum % AMBA_MON_NUM_RING_CHANNEL);
            FuncRetCode = AmbaWrap_memcpy(&(AmbaMonListenDsp_MsgRing.YuvAltInfo[FovId][FovRingIndex]), MemInfo.Ctx.pYuvRdy, sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
        /* yuv2raw msg link */
        if ((FovId < AMBA_MON_NUM_FOV_CHANNEL) &&
            (VinId < AMBA_MON_NUM_VIN_CHANNEL)) {
            if (AmbaMonMain_FovLatencySkip[FovId] > 0U) {
                AmbaMonMain_FovLatencySkip[FovId]--;
            } else {
                UINT64 RawCapSequence;
                UINT64 YuvPts;
                UINT64 RawPts;

                UINT32 DiffPts;
                 FLOAT VarF;
                UINT32 Latency;

                UINT32 AudioFreq;

                AMBA_MON_LISTEN_DSP_MEM_s MemInfo2;
                AMBA_MON_LISTEN_DSP_MEM_s MemInfo3;

                /* audio frequency */
#ifndef CONFIG_QNX
                AudioFreq = AmbaRTSL_PllGetAudioClk();
#else
                (void) AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioFreq);
#endif
                /* latency */
                RawCapSequence = MemInfo.Ctx.pYuvRdy->CapSequence;
                YuvPts = MemInfo.Ctx.pYuvRdy->YuvPts;
                MemInfo2.Ctx.pRawRdy = &(AmbaMonListenDsp_MsgRing.RawInfo[VinId][RawCapSequence % AMBA_MON_NUM_RING_CHANNEL]);
                if (((UINT32) (RawCapSequence & 0xFFFFFFFFULL)) == ((UINT32) (MemInfo2.Ctx.pRawRdy->CapSequence & 0xFFFFFFFFULL))/*TBD*/) {
                    RawPts = MemInfo2.Ctx.pRawRdy->CapPts;
                    if (YuvPts > RawPts) {
                        DiffPts = (UINT32) (YuvPts - RawPts);
                        VarF = (((FLOAT) DiffPts) * 1000.0f) / ((FLOAT) AudioFreq);
                        Latency = (VarF > 0.0f) ? (UINT32) VarF : 0U;
                        /* debug msg */
                        if (AmbaMonMain_Info.Debug.Bits.Latency > 0U) {
                            AmbaMonMain_TimingMarkPutByFov(MemInfo.Ctx.pYuvRdy->ViewZoneId, "Latency_F");
                            {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                str[0] = 'f';
                                svar_utoa(FovId, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                                str[0] = 'd';
                                svar_utoa(Latency, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByFov(FovId, str);
                            }
                        }
                        /* dsp fov latency put */
                        FovState.Latency = Latency;
                        /* dsp fov latency msg put */
                        MemInfo3.Ctx.pFovState = &FovState;
                        FuncRetCode = AmbaMonDsp_FovDelay(MemInfo3.Ctx.pCvoid);
                        if (FuncRetCode != OK_UL) {
                            /* */
                        }
                    }
                }
            }
        }
        /* dsp fov state update */
        if ((FovState.FovId < AMBA_MON_NUM_FOV_CHANNEL) &&
            (LastDspFovState[FovState.FovId].Msg != (UINT32) DSP_STATE_START)) {
            FuncRetCode = AmbaMonListenDsp_FovStart(&FovStartPort, &FovState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastDspFovState[FovState.FovId].Msg = (UINT32) DSP_STATE_START;
        }
        /* dsp fov alt repeat check */
        if ((FovState.FovId < AMBA_MON_NUM_FOV_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenDsp_YuvAltRepeat[FovState.FovId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp fov alt repeat", FovState.FovId, 10U, AmbaMonListenDsp_YuvAltRepeat[FovState.FovId], 16U);
        }
    } else if (pListenEvent->Flag == pListenEvent->DefYuvAlt.Flag) {
        /* def yuv alt */
        UINT32 FovId = MemInfo.Ctx.pYuvRdy->ViewZoneId;
        UINT64 FovSeqNum = MemInfo.Ctx.pYuvRdy->YuvSequence;
        /* listen yuv alt ring put */
        if (FovId < AMBA_MON_NUM_FOV_CHANNEL) {
            UINT32 FovRingIndex = (UINT32) (FovSeqNum % AMBA_MON_NUM_RING_CHANNEL);
            FuncRetCode = AmbaWrap_memcpy(&(AmbaMonListenDsp_MsgRing.YuvAltInfo[FovId][FovRingIndex]), MemInfo.Ctx.pYuvRdy, sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
        }
    } else if (pListenEvent->Flag == pListenEvent->Disp.Flag) {
        /* disp yuv */
        UINT32 DispId = MemInfo.Ctx.pDispRdy->ViewZoneId;
        UINT64 DispSeqNum = MemInfo.Ctx.pYuvRdy->YuvSequence;
        /* listen disp ring put */
        if (DispId < AMBA_MON_NUM_DISP_CHANNEL) {
            UINT32 DispRingIndex = (UINT32) (DispSeqNum % AMBA_MON_NUM_RING_CHANNEL);
            FuncRetCode = AmbaWrap_memcpy(&(AmbaMonListenDsp_MsgRing.DispInfo[DispId][DispRingIndex]), MemInfo.Ctx.pDispRdy, sizeof(AMBA_DSP_YUV_DATA_RDY_EXTEND_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            AmbaMonListenDsp_LastDispRingIndex[DispId] = DispRingIndex;
        }
        /* dsp disp repeat check */
        if ((DispId < AMBA_MON_NUM_DISP_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenDsp_DispYuvRepeat[DispId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp disp repeat", DispId, 10U, AmbaMonListenDsp_DispYuvRepeat[DispId], 16U);
        }
    } else if (pListenEvent->Flag == pListenEvent->Vout.Flag) {
        /* dsp vout */
        AMBA_MON_DSP_VOUT_STATE_s VoutState;
        VoutState.VoutId = MemInfo.Ctx.pVoutStatus->VoutIdx;
        VoutState.Pts = MemInfo.Ctx.pVoutStatus->DispStartTime;
        VoutState.DispSeqNum = 0ULL;
        VoutState.DispLatency = 0U;
        VoutState.FovSelectBits = 0U;
        /* vout2raw msg link */
        if (VoutState.VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
            if (AmbaMonMain_VoutLatencySkip[VoutState.VoutId] > 0U) {
                AmbaMonMain_VoutLatencySkip[VoutState.VoutId]--;
            } else {
                UINT32 i, j;

                UINT32 VoutId;
                UINT64 VoutAddr;
                UINT64 VoutPts;

                UINT32 DispRingIndex;
                UINT64 DispAddr;
                UINT64 DispPts;

                UINT32 FovRingIndex;
                UINT32 FovSelectBits;
                UINT32 FovSeqNum;
                //UINT64 FovPts;

                UINT32 VinId;
                UINT32 RawRingIndex;
                UINT32 RawSeqNum;
                UINT64 RawPts;

                UINT32 DiffPts;
                 FLOAT VarF;
                UINT32 Latency;

                UINT32 CheckFlag = OK_UL;

                UINT32 AudioFreq;

                AMBA_MON_LISTEN_DSP_MEM_s MemInfo2;
                AMBA_MON_LISTEN_DSP_MEM_s MemInfo3;
                AMBA_MON_LISTEN_DSP_MEM_s MemInfo4;
                AMBA_MON_LISTEN_DSP_MEM_s MemInfo5;

                /* audio frequency */
#ifndef CONFIG_QNX
                AudioFreq = AmbaRTSL_PllGetAudioClk();
#else
                (void) AmbaSYS_GetClkFreq(AMBA_SYS_CLK_AUD_0, &AudioFreq);
#endif
                /* vout id */
                VoutId = MemInfo.Ctx.pVoutStatus->VoutIdx;
                /* vout addr */
                VoutAddr = (UINT64) MemInfo.Ctx.pVoutStatus->YuvBuf.BaseAddrY;
                /* vout pts */
                VoutPts = MemInfo.Ctx.pVoutStatus->DispStartTime;

                if (VoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
                    /* last disp index */
                    DispRingIndex = AmbaMonListenDsp_LastDispRingIndex[VoutId];
                    /* disp info find */
                    for (i = 0U; i < AMBA_MON_NUM_RING_CHANNEL; i++) {
                        /* disp info get */
                        MemInfo4.Ctx.pDispRdy = &(AmbaMonListenDsp_MsgRing.DispInfo[VoutId][DispRingIndex]);
                        /* buf addr check */
                        if (MemInfo4.Ctx.pDispRdy != NULL) {
                            /* disp addr */
                            DispAddr = (UINT64) MemInfo4.Ctx.pDispRdy->Buffer.BaseAddrY;
                            /* compare disp addr with vout addr */
                            if (DispAddr == VoutAddr) {
                                break;
                            }
                        }
                        /* next */
                        DispRingIndex = (DispRingIndex + AMBA_MON_NUM_RING_CHANNEL - 1U) % AMBA_MON_NUM_RING_CHANNEL;
                    }
                    /* match? */
                    if (i < AMBA_MON_NUM_RING_CHANNEL) {
                        /* disp pts */
                        DispPts =  MemInfo4.Ctx.pDispRdy->YuvPts;
                        /* vout2disp latency */
                        if (VoutPts > DispPts) {
                            DiffPts = (UINT32) (VoutPts - DispPts);
                            VarF = (((FLOAT) DiffPts) * 1000.0f) / ((FLOAT) AudioFreq);
                            Latency = (VarF > 0.0f) ? (UINT32) VarF : 0U;
                        } else {
                            Latency = 0U;
                        }
                        /* disp latency */
                        VoutState.DispLatency = Latency;
                        /* disp seq no */
                        VoutState.DispSeqNum = MemInfo4.Ctx.pDispRdy->YuvSequence;
                        /* fov ring index */
                        FovRingIndex = (UINT32) (MemInfo4.Ctx.pDispRdy->YuvSequence % AMBA_MON_NUM_RING_CHANNEL);
                        /* fov select bits */
                        FovSelectBits = MemInfo4.Ctx.pDispRdy->SourceViewBit;
                        /* debug msg */
                        if (AmbaMonMain_Info.Debug.Bits.Latency > 0U) {
                            AmbaMonMain_TimingMarkPutByVout(VoutId, "Latency_O");
                            {
                                char str[11];
                                str[0] = ' ';str[1] = ' ';
                                str[0] = 'n';
                                svar_utoa((UINT32) (VoutState.DispSeqNum & 0xFFFFFFFFULL), &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                str[0] = 'd';
                                svar_utoa(Latency, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                            }
                        }
                        /* fov info find */
                        for (j = 0U; (j < AMBA_MON_NUM_FOV_CHANNEL) && ((FovSelectBits >> j) > 0U); j++) {
                            /* fov bits check */
                            if ((FovSelectBits & (((UINT32) 1U) << j)) > 0U) {
                                /* fov (alt) info get */
                                MemInfo3.Ctx.pYuvRdy = &(AmbaMonListenDsp_MsgRing.YuvAltInfo[j][FovRingIndex]);
                                /* fov check */
                                if (MemInfo3.Ctx.pYuvRdy != NULL) {
                                    /* fov pts */
                                    //FovPts = MemInfo3.Ctx.pYuvRdy->YuvPts;
                                    /* fov seq num */
                                    FovSeqNum = (UINT32) (MemInfo3.Ctx.pYuvRdy->YuvSequence & 0xFFFFFFFFULL);
                                    if (AmbaMonMain_Info.Debug.Bits.Latency > 0U) {
                                        char str[11];
                                        str[0] = ' ';str[1] = ' ';
                                        str[0] = 'f';
                                        svar_utoa(j, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                        AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                        str[0] = 'n';
                                        svar_utoa(FovSeqNum, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                        AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                    }
                                    /* raw vin id */
                                    VinId = MemInfo3.Ctx.pYuvRdy->VinId;
                                    /* raw ring index */
                                    RawRingIndex = (UINT32) (MemInfo3.Ctx.pYuvRdy->CapSequence % AMBA_MON_NUM_RING_CHANNEL);
                                    /* raw info get */
                                    MemInfo2.Ctx.pRawRdy = &(AmbaMonListenDsp_MsgRing.RawInfo[VinId][RawRingIndex]);
                                    /* raw check */
                                    if (MemInfo2.Ctx.pRawRdy != NULL) {
                                        /* raw pts */
                                        RawPts = MemInfo2.Ctx.pRawRdy->CapPts;
                                        /* raw seq num */
                                        RawSeqNum = (UINT32) (MemInfo2.Ctx.pRawRdy->CapSequence & 0xFFFFFFFFULL);
                                        /* debug msg */
                                        if (AmbaMonMain_Info.Debug.Bits.Latency > 0U) {
                                            char str[11];
                                            str[0] = ' ';str[1] = ' ';
                                            str[0] = 'v';
                                            svar_utoa(VinId, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                            AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                            str[0] = 'c';
                                            svar_utoa(RawSeqNum, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                            AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                        }
                                    } else {
                                        /* raw null? */
                                        RawPts = 0ULL;
                                        CheckFlag = NG_UL;
                                    }
                                } else {
                                    /* yuv null? */
                                    RawPts = 0ULL;
                                    CheckFlag = NG_UL;
                                }
                                /* vout2raw latency */
                                if ((CheckFlag == OK_UL) &&
                                    (VoutPts > RawPts)) {
                                    /* latency */
                                    DiffPts = (UINT32) (VoutPts - RawPts);
                                    if (AudioFreq > 0U) {
                                        VarF = (((FLOAT) DiffPts) * 1000.0f) / ((FLOAT) AudioFreq);
                                    } else {
                                        VarF = 0.0f;
                                    }
                                    Latency = (VarF > 0.0f) ? (UINT32) VarF : 0U;
                                    /* debug msg */
                                    if (AmbaMonMain_Info.Debug.Bits.Latency > 0U) {
                                        char str[11];
                                        str[0] = ' ';str[1] = ' ';
                                        svar_utoa(AudioFreq, &(str[2]), 16U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                        AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                        str[0] = 't';
                                        svar_utoa(Latency, &(str[2]), 10U, 8U, (UINT32) SVAR_LEADING_SPACE);
                                        AmbaMonMain_TimingMarkPutByVout(VoutId, str);
                                    }
                                } else {
                                    /* pts err? */
                                    Latency = 0U;
                                }
                                /* fov bits */
                                VoutState.FovSelectBits |= (((UINT32) 1U) << j);
                                /* latency */
                                VoutState.Latency[j] = Latency;
                            }
                        }
                    }
                    /* dsp vout latency msg put */
                    MemInfo5.Ctx.pVoutState = &VoutState;
                    FuncRetCode = AmbaMonDsp_VoutDelay(MemInfo5.Ctx.pCvoid);
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                }
            }
        }
        /* dsp vout state update */
        if ((VoutState.VoutId < AMBA_MON_NUM_VOUT_CHANNEL) &&
            (LastDspVoutState[VoutState.VoutId].Msg != (UINT32) DSP_STATE_START)) {
            FuncRetCode = AmbaMonListenDsp_VoutStart(&VoutStartPort, &VoutState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastDspVoutState[VoutState.VoutId].Msg = (UINT32) DSP_STATE_START;
        }
        /* dsp vout repeat check */
        if ((VoutState.VoutId < AMBA_MON_NUM_VOUT_CHANNEL) && (pListenMsg->Ctx.Chunk.Repeat > 0U)) {
            AmbaMonListenDsp_VoutYuvRepeat[VoutState.VoutId]++;
            AmbaMonPrintEx2(S_PRINT_FLAG_REPEAT, "dsp vout repeat", VoutState.VoutId, 10U, AmbaMonListenDsp_VoutYuvRepeat[VoutState.VoutId], 16U);
        }
    } else if (pListenEvent->Flag == pListenEvent->VinTimeout.Flag) {
        /* vin timeout msg */
        AMBA_MON_DSP_VIN_TIMEOUT_STATE_s VinTimeoutState;
        VinTimeoutState.VinId = MemInfo.Ctx.pVideoPath->Data;
        /* vin timeout state update */
        if ((VinTimeoutState.VinId < AMBA_MON_NUM_VIN_CHANNEL) &&
            (LastDspRawState[VinTimeoutState.VinId].Msg != (UINT32) DSP_STATE_START)) {
            if (LastDspVinTimeoutState[VinTimeoutState.VinId].Msg == (UINT32) DSP_STATE_IDLE) {
                FuncRetCode = AmbaMonListenDsp_VinTimeoutStart(&VinTimeoutPort, &VinTimeoutState);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                FuncRetCode = AmbaMonListenDsp_VinTimeoutStart(&VinDspVinTimeoutPort, &VinTimeoutState);
                if (FuncRetCode != OK_UL) {
                    /* */
                }
                LastDspVinTimeoutState[VinTimeoutState.VinId].Msg = (UINT32) DSP_STATE_START;
            }
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen dsp timeout process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen dsp event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenDsp_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_DSP_EVENT_s *pListenEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->Raw.Flag) {
        /* dsp raw timeout */
        AMBA_MON_DSP_RAW_STATE_s RawState;
        RawState.VinId = MemInfo.Ctx.pRawRdy->VinId;
        /* dsp raw timeout update */
        if ((RawState.VinId < AMBA_MON_NUM_VIN_CHANNEL) &&
            (LastDspRawState[RawState.VinId].Msg != (UINT32) DSP_STATE_TIMEOUT)) {
            FuncRetCode = AmbaMonListenDsp_RawTimeout(&RawTimeoutPort, &RawState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            FuncRetCode = AmbaMonListenDsp_RawTimeout(&VinRawTimeoutPort, &RawState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastDspRawState[RawState.VinId].Msg = (UINT32) DSP_STATE_TIMEOUT;
        }
    } else if (pListenEvent->Flag == pListenEvent->YuvAlt.Flag) {
        /* dsp fov timeout */
        AMBA_MON_DSP_FOV_STATE_s FovState;
        AMBA_MON_LISTEN_DSP_MEM_s MemInfo2;
        FovState.FovId = MemInfo.Ctx.pYuvRdy->ViewZoneId;
        FovState.YuvSeqNum = MemInfo.Ctx.pYuvRdy->YuvSequence;
        FovState.Latency = 0U;
        /* dsp fov latency msg put (timeout) */
        MemInfo2.Ctx.pFovState = &FovState;
        FuncRetCode = AmbaMonDsp_FovDelayTimeout(MemInfo2.Ctx.pCvoid);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* dsp fov timeout update */
        if ((FovState.FovId < AMBA_MON_NUM_FOV_CHANNEL) &&
            (LastDspFovState[FovState.FovId].Msg != (UINT32) DSP_STATE_TIMEOUT)) {
            FuncRetCode = AmbaMonListenDsp_FovTimeout(&FovTimeoutPort, &FovState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastDspFovState[FovState.FovId].Msg = (UINT32) DSP_STATE_TIMEOUT;
        }
    } else if (pListenEvent->Flag == pListenEvent->Vout.Flag) {
        /* dsp vout timeout */
        AMBA_MON_DSP_VOUT_STATE_s VoutState;
        AMBA_MON_LISTEN_DSP_MEM_s MemInfo2;
        VoutState.VoutId = MemInfo.Ctx.pVoutStatus->VoutIdx;
        /* dsp vout latency msg put (timeout) */
        MemInfo2.Ctx.pVoutState = &VoutState;
        FuncRetCode = AmbaMonDsp_VoutDelayTimeout(MemInfo2.Ctx.pCvoid);
        if (FuncRetCode != OK_UL) {
            /* */
        }
        /* dsp vout timeout update */
        if ((VoutState.VoutId < AMBA_MON_NUM_VOUT_CHANNEL) &&
            (LastDspVoutState[VoutState.VoutId].Msg != (UINT32) DSP_STATE_TIMEOUT)) {
            FuncRetCode = AmbaMonListenDsp_VoutTimeout(&VoutTimeoutPort, &VoutState);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastDspVoutState[VoutState.VoutId].Msg = (UINT32) DSP_STATE_TIMEOUT;
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen dsp graph delete
 *  @param[in] pListenPort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenDsp_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pListenPort)
{
    UINT32 FuncRetCode;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Close(pListenPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp listen port close", FuncRetCode, 16U);
    }

    /* raw state port close (dsp) */
    FuncRetCode = AmbaMonMessage_Close(&RawStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state raw start port close", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&RawTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state raw timeout port close", FuncRetCode, 16U);
    }
    /* def raw state port close (dsp) */
    FuncRetCode = AmbaMonMessage_Close(&DefRawStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state def raw start port close", FuncRetCode, 16U);
    }
    /* fov state port close (dsp) */
    FuncRetCode = AmbaMonMessage_Close(&FovStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state fov start port close", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&FovTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state fov timeout port close", FuncRetCode, 16U);
    }
    /* vout state port close (dsp) */
    FuncRetCode = AmbaMonMessage_Close(&VoutStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state vout start port close", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&VoutTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state vout timeout port close", FuncRetCode, 16U);
    }
    /* vin timeout state port close (dsp) */
    FuncRetCode = AmbaMonMessage_Close(&VinTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state vin timeout port close", FuncRetCode, 16U);
    }
    /* raw state port close (vin) */
    FuncRetCode = AmbaMonMessage_Close(&VinRawStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state raw start port close", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&VinRawTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state raw timeout port close", FuncRetCode, 16U);
    }
    /* def raw state port close (vin) */
    FuncRetCode = AmbaMonMessage_Close(&VinDefRawStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state def raw start port close", FuncRetCode, 16U);
    }
    /* vin timeout port close (vin) */
    FuncRetCode = AmbaMonMessage_Close(&VinDspVinTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state vin timeout port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor listen dsp graph create
 *  @param[in] pListenPort pointer to message port
 *  @param[in] pListenEvent pointer to listen event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenDsp_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pListenPort, AMBA_MON_LISTEN_DSP_EVENT_s *pListenEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;

    UINT32 DspEventId;
    UINT64 DspEventFlag;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Open(pListenPort, "timeline", NULL);
    if (FuncRetCode == OK_UL) {
        /* listen port success */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "dsp listen port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* raw event flag get */
        pListenEvent->Raw.pName = pDspRawName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Raw.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->Raw.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Raw.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->Raw.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Raw.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* def raw event flag get */
        pListenEvent->DefRaw.pName = pDspDefRawName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->DefRaw.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->DefRaw.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->DefRaw.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->DefRaw.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->DefRaw.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* yuv event flag get */
        pListenEvent->Yuv.pName = pDspYuvName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Yuv.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->Yuv.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Yuv.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->Yuv.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Yuv.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* yuv alt event flag get */
        pListenEvent->YuvAlt.pName = pDspYuvAltName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->YuvAlt.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->YuvAlt.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->YuvAlt.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->YuvAlt.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->YuvAlt.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* def yuv alt event flag get */
        pListenEvent->DefYuvAlt.pName = pDspDefYuvAltName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->DefYuvAlt.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->DefYuvAlt.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->DefYuvAlt.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->DefYuvAlt.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->DefYuvAlt.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* disp event flag get */
        pListenEvent->Disp.pName = pDspDispName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Disp.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->Disp.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Disp.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->Disp.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Disp.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vout event flag get */
        pListenEvent->Vout.pName = pDspVoutName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Vout.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->Vout.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Vout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->Vout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Vout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vin timeout event flag get */
        pListenEvent->VinTimeout.pName = pDspVinTimeoutName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->VinTimeout.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->VinTimeout.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->VinTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->VinTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->VinTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* echo event flag get */
        pListenEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Echo.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pListenEvent->Echo.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pListenEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* listen port fail */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp listen port", (UINT32) MemInfo.Ctx.Data, 16U);
    }

    /* raw state port open (dsp) */
    FuncRetCode = AmbaMonMessage_Open(&RawStartPort, "dsp_state", "raw_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state raw start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&RawTimeoutPort, "dsp_state", "raw_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state raw timeout port open", FuncRetCode, 16U);
    }
    /* def raw state port open (dsp) */
    FuncRetCode = AmbaMonMessage_Open(&DefRawStartPort, "dsp_state", "def_raw_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state def raw start port open", FuncRetCode, 16U);
    }
    /* fov state port open (dsp) */
    FuncRetCode = AmbaMonMessage_Open(&FovStartPort, "dsp_state", "fov_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state fov start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&FovTimeoutPort, "dsp_state", "fov_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state fov timeout port open", FuncRetCode, 16U);
    }
    /* vout state port open (dsp) */
    FuncRetCode = AmbaMonMessage_Open(&VoutStartPort, "dsp_state", "vout_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state vout start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&VoutTimeoutPort, "dsp_state", "vout_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state vout timeout port open", FuncRetCode, 16U);
    }
    /* vin timeout state port open (dsp) */
    FuncRetCode = AmbaMonMessage_Open(&VinTimeoutPort, "dsp_state", "vin_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state vin timeout port open", FuncRetCode, 16U);
    }
    /* raw state port open (vin) */
    FuncRetCode = AmbaMonMessage_Open(&VinRawStartPort, "vin_state", "raw_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state raw start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&VinRawTimeoutPort, "vin_state", "raw_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state raw timeout port open", FuncRetCode, 16U);
    }
    /* def raw state port open (vin) */
    FuncRetCode = AmbaMonMessage_Open(&VinDefRawStartPort, "vin_state", "def_raw_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state def raw start port open", FuncRetCode, 16U);
    }
    /* vin timeout port open (vin) */
    FuncRetCode = AmbaMonMessage_Open(&VinDspVinTimeoutPort, "vin_state", "vin_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state vin timeout port open", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor listen dsp task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonListenDsp_TaskEx(void *pParam)
{
    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonListenDsp_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor listen dsp task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenDsp_Task(UINT32 Param)
{
    static UINT32 AmbaMonListenDsp_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonListenDsp_ListenPort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_LISTEN_DSP_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pListenPort;
    AMBA_MON_LISTEN_DSP_EVENT_s ListenEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pListenMsg;
    void *pMsg;

    (void) Param;
    pListenPort = &(AmbaMonListenDsp_ListenPort);

    AmbaMonListenDsp_Cmd = (UINT32) LISTEN_DSP_TASK_CMD_STOP;

    while (AmbaMonListenDsp_DummyFlag > 0U) {
        if (AmbaMonListenDsp_Cmd == (UINT32) LISTEN_DSP_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonListenDsp_GraphDelete(pListenPort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenDsp_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen dsp task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenDsp_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen dsp task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonListenDsp_GraphCreate(pListenPort, &ListenEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen dsp port seek");
            }
            /* msg repeat reset */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                AmbaMonListenDsp_RawRepeat[i] = 0U;
            }
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                AmbaMonListenDsp_YuvRepeat[i] = 0U;
                AmbaMonListenDsp_YuvAltRepeat[i] = 0U;
            }
            for (i = 0U; i < AMBA_MON_NUM_DISP_CHANNEL; i++) {
                AmbaMonListenDsp_DispYuvRepeat[i] = 0U;
            }
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                AmbaMonListenDsp_VoutYuvRepeat[i] = 0U;
            }
            /* state reset */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                LastDspRawState[i].Msg = (UINT32) DSP_STATE_IDLE;
                LastDspDefRawState[i].Msg = (UINT32) DSP_STATE_IDLE;
                LastDspVinTimeoutState[i].Msg = (UINT32) DSP_STATE_IDLE;
            }
            for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                LastDspFovState[i].Msg = (UINT32) DSP_STATE_IDLE;
            }
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                LastDspVoutState[i].Msg = (UINT32) DSP_STATE_IDLE;
            }
            /* msg ring reset */
            FuncRetCode = AmbaWrap_memset(&AmbaMonListenDsp_MsgRing, 0, sizeof(AMBA_MON_LISTEN_DSP_MSG_s));
            if (FuncRetCode != OK_UL) {
                /* */
            }
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                AmbaMonListenDsp_LastDispRingIndex[i] = 0U;
            }
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenDsp_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen dsp task ack");
            }
        }

        ActualFlags = AmbaMonMessage_Wait(pListenPort, EventFlag, (UINT32) MON_EVENT_FLAG_OR_CLR, (UINT32) AMBA_KAL_WAIT_FOREVER);
        if ((ActualFlags & EventFlag) > 0ULL) {
            do {
                MsgRetCode = AmbaMonMessage_Get(pListenPort, &pMsg);
                if (MsgRetCode == OK_UL) {
                    /* msg get */
                    MemInfo.Ctx.pVoid = pMsg;
                    /* event flag get */
                    ListenEvent.Flag = MemInfo.Ctx.pMsgHead->Ctx.Event.Flag;
                    /* link check */
                    if (MemInfo.Ctx.pMsgHead->Ctx.Com.Id == (UINT8) AMBA_MON_MSG_ID_LINK) {
                        MemInfo.Ctx.Data = (AMBA_MON_MAIN_MEM_ADDR) MemInfo.Ctx.pMsgHead->Ctx.Link.Pointer;
                    }
                    /* chunk itself */
                    pListenMsg = MemInfo.Ctx.pMsgHead;
                    /* msg parse */
                    if (pListenMsg->Ctx.Chunk.Timeout == 0U) {
                        AmbaMonListenDsp_MsgProc(pListenMsg, &ListenEvent);
                    } else {
                        AmbaMonListenDsp_TimeoutProc(pListenMsg, &ListenEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen dsp port seek (reset)");
                    }
                    /* msg repeat reset */
                    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                        AmbaMonListenDsp_RawRepeat[i] = 0U;
                    }
                    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                        AmbaMonListenDsp_YuvRepeat[i] = 0U;
                        AmbaMonListenDsp_YuvAltRepeat[i] = 0U;
                    }
                    for (i = 0U; i < AMBA_MON_NUM_DISP_CHANNEL; i++) {
                        AmbaMonListenDsp_DispYuvRepeat[i] = 0U;
                    }
                    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                        AmbaMonListenDsp_VoutYuvRepeat[i] = 0U;
                    }
                    /* state reset */
                    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                        LastDspRawState[i].Msg = (UINT32) DSP_STATE_IDLE;
                        LastDspDefRawState[i].Msg = (UINT32) DSP_STATE_IDLE;
                        LastDspVinTimeoutState[i].Msg = (UINT32) DSP_STATE_IDLE;
                    }
                    for (i = 0U; i < AMBA_MON_NUM_FOV_CHANNEL; i++) {
                        LastDspFovState[i].Msg = (UINT32) DSP_STATE_IDLE;
                    }
                    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                        LastDspVoutState[i].Msg = (UINT32) DSP_STATE_IDLE;
                    }
                    /* msg ring reset */
                    FuncRetCode = AmbaWrap_memset(&AmbaMonListenDsp_MsgRing, 0, sizeof(AMBA_MON_LISTEN_DSP_MSG_s));
                    if (FuncRetCode != OK_UL) {
                        /* */
                    }
                    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                        AmbaMonListenDsp_LastDispRingIndex[i] = 0U;
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

