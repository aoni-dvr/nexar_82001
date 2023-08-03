/**
 *  @file AmbaMonListenVin.c
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
 *  @details Amba Monitor Listen VIN
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaVIN_Def.h"

#include "AmbaDSP_Capability.h"
#include "AmbaDSP_EventInfo.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"
#include "AmbaMonMain.h"
#include "AmbaMonMain_Internal.h"

#include "AmbaMonListenVin.h"

#ifndef AMBA_MON_MESSAGE_PUT2
#include "AmbaWrap.h"
#endif

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_LISTEN_VIN_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR       Data;
    const void                   *pCvoid;
    void                         *pVoid;
    AMBA_MON_MESSAGE_PORT_s      *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s    *pMsgHead;
    UINT8                        *pUint8;
    //UINT32                       *pUint32;
    AMBA_MON_VIN_ISR_INFO_s      *pVinIsrInfo;
    AMBA_MON_VIN_ISR_STATE_s     *pVinIsrState;
} AMBA_MON_LISTEN_VIN_MEM_u;

typedef struct /*_AMBA_MON_LISTEN_VIN_MEM_s_*/ {
    AMBA_MON_LISTEN_VIN_MEM_u    Ctx;
} AMBA_MON_LISTEN_VIN_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonListenVinTaskId;
static void AmbaMonListenVin_Task(UINT32 Param);
static void *AmbaMonListenVin_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenVin_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenVin_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenVin_Ack;

static UINT32 AmbaMonListenVin_Cmd = (UINT32) LISTEN_VIN_TASK_CMD_STOP;

/* vin state message port (sof) */
static AMBA_MON_MESSAGE_PORT_s SofStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s SofTimeoutPort GNU_SECTION_NOZEROINIT;
/* vin state message port (eof) */
static AMBA_MON_MESSAGE_PORT_s EofStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s EofTimeoutPort GNU_SECTION_NOZEROINIT;

static AMBA_MON_LISTEN_VIN_STATE_s LastVinState[AMBA_MON_NUM_VIN_CHANNEL] = {0};

/**
 *  @private
 *  Amba monitor listen vin create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVin_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_LISTEN_VIN_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonListenVinTaskStack[AMBA_MON_LISTEN_VIN_TASK_STACK_SIZE];
    static char AmbaMonListenVinTaskName[] = "MonitorLsnVin";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenVin_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenVin_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenVin_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create listen vin task");
        {
            AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonListenVinTaskId,
                                             AmbaMonListenVinTaskName,
                                             Priority,
                                             AmbaMonListenVin_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonListenVinTaskStack,
                                             AMBA_MON_LISTEN_VIN_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonListenVinTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonListenVinTaskId);
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
 *  Amba monitor listen vin delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVin_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonListenVinTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonListenVinTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenVin_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenVin_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenVin_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen vin active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVin_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonListenVin_Cmd == (UINT32) LISTEN_VIN_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVin_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonListenVin_Cmd = (UINT32) LISTEN_VIN_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVin_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVin_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor listen vin inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVin_Inactive(void)
{
    if (AmbaMonListenVin_Cmd == (UINT32) LISTEN_VIN_TASK_CMD_START) {
        AmbaMonListenVin_Cmd = (UINT32) LISTEN_VIN_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor listen vin idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVin_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVin_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVin_Ready);
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
 *  Amba monitor listen vin sof state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the state data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVin_SofStatePut(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 SofStateBuf[256];
#else
    UINT8 SofStateBuf[32];
#endif
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    const void *pMem;

    UINT32 VinId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_VIN_ISR_STATE_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vin id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VinId = MemInfo.Ctx.pVinIsrState->VinId;

    /* msg head get */
    MemInfo.Ctx.pUint8 = SofStateBuf;
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
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, pEvent, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = pEvent;
#endif
    /* msg put */
    if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = SofStateBuf;
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
 *  Amba monitor lisetn vin sof start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVin_SofStart(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    AMBA_MON_VIN_ISR_STATE_s VinIsrState;

    /* sof isr start state */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrState.VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
    VinIsrState.SensorId = MemInfo.Ctx.pVinIsrInfo->SensorId;
    VinIsrState.Dts = MemInfo.Ctx.pVinIsrInfo->Dts;

    /* sof isr start state put */
    MemInfo.Ctx.pVinIsrState = &VinIsrState;
    FuncRetCode = AmbaMonListenVin_SofStatePut(pPort, MemInfo.Ctx.pCvoid, 0);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "sof start (state) put fail...", MemInfo.Ctx.pVinIsrState->VinId);
    }

    return FuncRetCode;
}

/**
 *  Amba monitor lisetn vin sof timeout
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVin_SofTimeout(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    AMBA_MON_VIN_ISR_STATE_s VinIsrState;

    /* sof isr timeout state */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrState.VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
    VinIsrState.SensorId = MemInfo.Ctx.pVinIsrInfo->SensorId;
    VinIsrState.Dts = MemInfo.Ctx.pVinIsrInfo->Dts;

    /* sof isr timeout state put */
    MemInfo.Ctx.pVinIsrState = &VinIsrState;
    FuncRetCode = AmbaMonListenVin_SofStatePut(pPort, MemInfo.Ctx.pCvoid, 1);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "sof timout (state) put fail...", MemInfo.Ctx.pVinIsrState->VinId);
    }

    return FuncRetCode;
}

/**
 *  Amba monitor lisetn vin eof state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the event data
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVin_EofStatePut(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 EofStateBuf[256];
#else
    UINT8 EofStateBuf[32];
#endif
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    const void *pMem;

    UINT32 VinId;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(AMBA_MON_VIN_ISR_STATE_s);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* vin id get */
    MemInfo.Ctx.pCvoid = pEvent;
    VinId = MemInfo.Ctx.pVinIsrState->VinId;

    /* msg head get */
    MemInfo.Ctx.pUint8 = EofStateBuf;
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
#ifndef AMBA_MON_MESSAGE_PUT2
    /* msg data copy */
    FuncRetCode = AmbaWrap_memcpy(pMsgData, pEvent, DataSize);
    if (FuncRetCode != OK_UL) {
        /* */
    }
#else
    /* msg data get */
    pMsgData = pEvent;
#endif
    /* msg put */
    if (VinId < AMBA_MON_NUM_VIN_CHANNEL) {
        MemInfo.Ctx.pUint8 = EofStateBuf;
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
 *  Amba monitor lisetn vin eof start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVin_EofStart(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    AMBA_MON_VIN_ISR_STATE_s VinIsrState;

    /* eof isr start state */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrState.VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
    VinIsrState.SensorId = MemInfo.Ctx.pVinIsrInfo->SensorId;
    VinIsrState.Dts = MemInfo.Ctx.pVinIsrInfo->Dts;

    /* eof isr start state put */
    MemInfo.Ctx.pVinIsrState = &VinIsrState;
    FuncRetCode = AmbaMonListenVin_EofStatePut(pPort, MemInfo.Ctx.pCvoid, 0);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "eof start (state) put fail...", MemInfo.Ctx.pVinIsrState->VinId);
    }

    return FuncRetCode;
}

/**
 *  Amba monitor lisetn vin eof timeout
 *  @param[in] pPort pointer to the message port
 *  @param[in] pEvent pointer to the event data
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVin_EofTimeout(AMBA_MON_MESSAGE_PORT_s *pPort, const void *pEvent)
{
    UINT32 FuncRetCode;
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    AMBA_MON_VIN_ISR_STATE_s VinIsrState;

    /* eof isr timeout state */
    MemInfo.Ctx.pCvoid = pEvent;
    VinIsrState.VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
    VinIsrState.SensorId = MemInfo.Ctx.pVinIsrInfo->SensorId;
    VinIsrState.Dts = MemInfo.Ctx.pVinIsrInfo->Dts;

    /* eof isr timeout state put */
    MemInfo.Ctx.pVinIsrState = &VinIsrState;
    FuncRetCode = AmbaMonListenVin_EofStatePut(pPort, MemInfo.Ctx.pCvoid, 1);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrint(S_PRINT_FLAG_DBG, "eof timeout (state) put fail...", MemInfo.Ctx.pVinIsrState->VinId);
    }

    return FuncRetCode;
}

/**
 *  Amba monitor listen vin message process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen vin event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVin_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_VIN_EVENT_s *pListenEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->Sof.Flag) {
        /* sof isr */
        UINT32 VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
        /* vin sof state update */
        if ((VinId < AMBA_MON_NUM_VIN_CHANNEL) && (LastVinState[VinId].Sof != (UINT32) VIN_STATE_START)) {
            FuncRetCode = AmbaMonListenVin_SofStart(&SofStartPort, MemInfo.Ctx.pCvoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastVinState[VinId].Sof = (UINT32) VIN_STATE_START;
        }
    } else if (pListenEvent->Flag == pListenEvent->Eof.Flag) {
        /* eof isr */
        UINT32 VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
        /* vin eof state update */
        if ((VinId < AMBA_MON_NUM_VIN_CHANNEL) && (LastVinState[VinId].Eof != (UINT32) VIN_STATE_START)) {
            FuncRetCode = AmbaMonListenVin_EofStart(&EofStartPort, MemInfo.Ctx.pCvoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastVinState[VinId].Eof = (UINT32) VIN_STATE_START;
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen vin timeout process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen vin event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVin_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_VIN_EVENT_s *pListenEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->Sof.Flag) {
        /* sof timeout */
        UINT32 VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
        /* vin sof state update */
        if ((VinId < AMBA_MON_NUM_VIN_CHANNEL) && (LastVinState[VinId].Sof != (UINT32) VIN_STATE_TIMEOUT)) {
            FuncRetCode = AmbaMonListenVin_SofTimeout(&SofTimeoutPort, MemInfo.Ctx.pCvoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastVinState[VinId].Sof = (UINT32) VIN_STATE_TIMEOUT;
        }
    } else if (pListenEvent->Flag == pListenEvent->Eof.Flag) {
        /* eof timeout */
        UINT32 VinId = MemInfo.Ctx.pVinIsrInfo->VinId;
        /* vin sof state update */
        if ((VinId < AMBA_MON_NUM_VIN_CHANNEL) && (LastVinState[VinId].Eof != (UINT32) VIN_STATE_TIMEOUT)) {
            FuncRetCode = AmbaMonListenVin_EofTimeout(&EofTimeoutPort, MemInfo.Ctx.pCvoid);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastVinState[VinId].Eof = (UINT32) VIN_STATE_TIMEOUT;
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen vin graph delete
 *  @param[in] pListenPort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVin_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pListenPort)
{
    UINT32 FuncRetCode;

    /* listen port in timeline (close) */
    FuncRetCode = AmbaMonMessage_Close(pListenPort);
    if (FuncRetCode != OK_UL) {
        /* liste port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin listen port close", FuncRetCode, 16U);
    }

    /* state port close */
    FuncRetCode = AmbaMonMessage_Close(&SofStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state sof start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&SofTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state sof timeout port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&EofStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state eof start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&EofTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state eof timeout port open", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor listen vin graph create
 *  @param[in] pListenPort pointer to message port
 *  @param[in] pListenEvent pointer to listen event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVin_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pListenPort, AMBA_MON_LISTEN_VIN_EVENT_s *pListenEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;

    UINT32 VinEventId;
    UINT64 VinEventFlag;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Open(pListenPort, "timeline", NULL);
    if (FuncRetCode == OK_UL) {
        /* liste port success */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vin listen port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* sof event flag get */
        pListenEvent->Sof.pName = pVinSofName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Sof.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pListenEvent->Sof.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Sof.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pListenEvent->Sof.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Sof.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* eof event flag get */
        pListenEvent->Eof.pName = pVinEofName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Eof.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pListenEvent->Eof.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Eof.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pListenEvent->Eof.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Eof.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* echo event flag get */
        pListenEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Echo.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pListenEvent->Echo.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pListenEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* listen port fail */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin listen port open", (UINT32) MemInfo.Ctx.Data, 16U);
    }

    /* state port open */
    FuncRetCode = AmbaMonMessage_Open(&SofStartPort, "vin_state", "sof_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state sof start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&SofTimeoutPort, "vin_state", "sof_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state sof timeout port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&EofStartPort, "vin_state", "eof_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state eof start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&EofTimeoutPort, "vin_state", "eof_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state eof timeout port open", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor listen vin task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonListenVin_TaskEx(void *pParam)
{
    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonListenVin_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor listen vin task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVin_Task(UINT32 Param)
{
    static UINT32 AmbaMonListenVin_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonListenVin_ListenPort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_LISTEN_VIN_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pListenPort;
    AMBA_MON_LISTEN_VIN_EVENT_s ListenEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pListenMsg;
    void *pMsg;

    (void) Param;
    pListenPort = &(AmbaMonListenVin_ListenPort);

    AmbaMonListenVin_Cmd = (UINT32) LISTEN_VIN_TASK_CMD_STOP;

    while (AmbaMonListenVin_DummyFlag > 0U) {
        if (AmbaMonListenVin_Cmd == (UINT32) LISTEN_VIN_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonListenVin_GraphDelete(pListenPort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVin_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vin task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVin_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vin task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonListenVin_GraphCreate(pListenPort, &ListenEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vin port seek");
            }
            /* state reset */
            for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                LastVinState[i].Sof = (UINT32) VIN_STATE_IDLE;
                LastVinState[i].Eof = (UINT32) VIN_STATE_IDLE;
            }
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVin_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vin task ack");
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
                        AmbaMonListenVin_MsgProc(pListenMsg, &ListenEvent);
                    } else {
                        AmbaMonListenVin_TimeoutProc(pListenMsg, &ListenEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vin port seek (reset)");
                    }
                    /* state reset */
                    for (i = 0U; i < AMBA_MON_NUM_VIN_CHANNEL; i++) {
                        LastVinState[i].Sof = (UINT32) VIN_STATE_IDLE;
                        LastVinState[i].Eof = (UINT32) VIN_STATE_IDLE;
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

