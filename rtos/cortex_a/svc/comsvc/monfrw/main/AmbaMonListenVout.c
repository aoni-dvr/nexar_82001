/**
 *  @file AmbaMonListenVout.c
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
 *  @details Amba Monitor Listen VOUT
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

#include "AmbaMonListenVout.h"

#ifndef AMBA_MON_MESSAGE_PUT2
#include "AmbaWrap.h"
#endif

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_LISTEN_VOUT_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR       Data;
    const void                   *pCvoid;
    void                         *pVoid;
    AMBA_MON_MESSAGE_PORT_s      *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s    *pMsgHead;
    UINT8                        *pUint8;
    UINT32                       *pUint32;
} AMBA_MON_LISTEN_VOUT_MEM_u;

typedef struct /*_AMBA_MON_LISTEN_VOUT_MEM_s_*/ {
    AMBA_MON_LISTEN_VOUT_MEM_u    Ctx;
} AMBA_MON_LISTEN_VOUT_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonListenVoutTaskId;
static void AmbaMonListenVout_Task(UINT32 Param);
static void *AmbaMonListenVout_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenVout_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenVout_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonListenVout_Ack;

static UINT32 AmbaMonListenVout_Cmd = (UINT32) LISTEN_VOUT_TASK_CMD_STOP;

/* vout state message port (Isr) */
static AMBA_MON_MESSAGE_PORT_s IsrStartPort GNU_SECTION_NOZEROINIT;
static AMBA_MON_MESSAGE_PORT_s IsrTimeoutPort GNU_SECTION_NOZEROINIT;

static AMBA_MON_LISTEN_VOUT_STATE_s LastVoutState[AMBA_MON_NUM_VOUT_CHANNEL] = {0};

/**
 *  @private
 *  Amba monitor listen vout create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVout_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_LISTEN_VOUT_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonListenVoutTaskStack[AMBA_MON_LISTEN_VOUT_TASK_STACK_SIZE];
    static char AmbaMonListenVoutTaskName[] = "MonitorLsnVout";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenVout_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenVout_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonListenVout_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create listen vout task");
        {
            AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonListenVoutTaskId,
                                             AmbaMonListenVoutTaskName,
                                             Priority,
                                             AmbaMonListenVout_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonListenVoutTaskStack,
                                             AMBA_MON_LISTEN_VOUT_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonListenVoutTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonListenVoutTaskId);
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
 *  Amba monitor listen vout delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVout_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonListenVoutTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonListenVoutTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenVout_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenVout_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonListenVout_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor listen vout active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVout_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonListenVout_Cmd == (UINT32) LISTEN_VOUT_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVout_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonListenVout_Cmd = (UINT32) LISTEN_VOUT_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVout_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVout_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor listen vout inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVout_Inactive(void)
{
    if (AmbaMonListenVout_Cmd == (UINT32) LISTEN_VOUT_TASK_CMD_START) {
        AmbaMonListenVout_Cmd = (UINT32) LISTEN_VOUT_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor listen vout idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonListenVout_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVout_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVout_Ready);
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
 *  Amba monitor listen vout isr state put
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVoutId pointer to the vout id
 *  @param[in] TimeoutFlag timeout flag
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVout_IsrStatePut(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 *pVoutId, UINT8 TimeoutFlag)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    UINT32 Timetick = 0U;
#ifndef AMBA_MON_MESSAGE_PUT2
    UINT8 IsrStateBuf[256];
#else
    UINT8 IsrStateBuf[32];
#endif
    AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s *pMsgHead;
#ifndef AMBA_MON_MESSAGE_PUT2
    void *pMsgData;
#else
    const void *pMsgData;
#endif
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);
    UINT32 DataSize = (UINT32) sizeof(UINT32);
    UINT32 MsgLength = HeaderSize + DataSize;

    FuncRetCode = AmbaKAL_GetSysTickCount(&Timetick);
    if (FuncRetCode != KAL_ERR_NONE) {
        /* */
    }

    /* msg head get */
    MemInfo.Ctx.pUint8 = IsrStateBuf;
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

    MemInfo.Ctx.pUint32 = pVoutId;
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
    if (*pVoutId < AMBA_MON_NUM_VOUT_CHANNEL) {
        MemInfo.Ctx.pUint8 = IsrStateBuf;
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
 *  Amba monitor lisetn vout isr start
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVoutId pointer to the vout id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVout_IsrStart(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 *pVoutId)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenVout_IsrStatePut(pPort, pVoutId, 0);
    return RetCode;
}

/**
 *  Amba monitor lisetn vout isr timeout
 *  @param[in] pPort pointer to the message port
 *  @param[in] pVoutId pointer to the vout id
 *  @return error code
 *  @note this function is intended for internal use only
 */
static UINT32 AmbaMonListenVout_IsrTimeout(AMBA_MON_MESSAGE_PORT_s *pPort, UINT32 *pVoutId)
{
    UINT32 RetCode;
    RetCode = AmbaMonListenVout_IsrStatePut(pPort, pVoutId, 1);
    return RetCode;
}

/**
 *  Amba monitor listen vout message process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen vout event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVout_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_VOUT_EVENT_s *pListenEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->Isr.Flag) {
        /* vout isr */
        UINT32 VoutId = *(MemInfo.Ctx.pUint32);
        /* vout isr state update */
        if ((VoutId < AMBA_MON_NUM_VOUT_CHANNEL) && (LastVoutState[VoutId].Isr != (UINT32) VOUT_STATE_START)) {
            FuncRetCode = AmbaMonListenVout_IsrStart(&IsrStartPort, &VoutId);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastVoutState[VoutId].Isr = (UINT32) VOUT_STATE_START;
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen vout timeout process
 *  @param[in] pListenMsg pointer to the message header
 *  @param[in] pListenEvent pointer to the listen vout event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVout_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pListenMsg, const AMBA_MON_LISTEN_VOUT_EVENT_s *pListenEvent)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pListenMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pListenEvent->Flag == pListenEvent->Isr.Flag) {
        /* vout isr timeout */
        UINT32 VoutId = *(MemInfo.Ctx.pUint32);
        /* vout isr state update */
        if ((VoutId < AMBA_MON_NUM_VOUT_CHANNEL) && (LastVoutState[VoutId].Isr != (UINT32) VOUT_STATE_TIMEOUT)) {
            FuncRetCode = AmbaMonListenVout_IsrTimeout(&IsrTimeoutPort, &VoutId);
            if (FuncRetCode != OK_UL) {
                /* */
            }
            LastVoutState[VoutId].Isr = (UINT32) VOUT_STATE_TIMEOUT;
        }
    } else {
        /* */
    }
}

/**
 *  Amba monitor listen vout graph delete
 *  @param[in] pListenPort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVout_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pListenPort)
{
    UINT32 FuncRetCode;

    /* listen port in timeline (close) */
    FuncRetCode = AmbaMonMessage_Close(pListenPort);
    if (FuncRetCode != OK_UL) {
        /* liste port close fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout listen port close", FuncRetCode, 16U);
    }

    /* state port close */
    FuncRetCode = AmbaMonMessage_Close(&IsrStartPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state isr start port close", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Close(&IsrTimeoutPort);
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state isr timeout port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor listen vout graph create
 *  @param[in] pListenPort pointer to message port
 *  @param[in] pListenEvent pointer to listen event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVout_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pListenPort, AMBA_MON_LISTEN_VOUT_EVENT_s *pListenEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;

    UINT32 VoutEventId;
    UINT64 VoutEventFlag;

    /* listen port in timeline */
    FuncRetCode = AmbaMonMessage_Open(pListenPort, "timeline", NULL);
    if (FuncRetCode == OK_UL) {
        /* liste port success */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vout listen port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* isr event flag get */
        pListenEvent->Isr.pName = pVoutName;
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Isr.pName, &VoutEventId, &VoutEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VoutEventFlag;
            pListenEvent->Isr.Flag = VoutEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Isr.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VoutEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VoutEventFlag, 16U);
        } else {
            pListenEvent->Isr.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Isr.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* echo event flag get */
        pListenEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pListenPort, pListenEvent->Echo.pName, &VoutEventId, &VoutEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VoutEventFlag;
            pListenEvent->Echo.Flag = VoutEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pListenEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VoutEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VoutEventFlag, 16U);
        } else {
            pListenEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pListenEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* listen port fail */
        MemInfo.Ctx.pMsgPort = pListenPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout listen port", (UINT32) MemInfo.Ctx.Data, 16U);
    }

    /* state port open */
    FuncRetCode = AmbaMonMessage_Open(&IsrStartPort, "vout_state", "isr_start");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state isr start port open", FuncRetCode, 16U);
    }
    FuncRetCode = AmbaMonMessage_Open(&IsrTimeoutPort, "vout_state", "isr_timeout");
    if (FuncRetCode != OK_UL) {
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state isr timeout port open", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor listen vout task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonListenVout_TaskEx(void *pParam)
{
    AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonListenVout_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor listen vout task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonListenVout_Task(UINT32 Param)
{
    static UINT32 AmbaMonListenVout_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonListenVout_ListenPort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;
    UINT32 i;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_LISTEN_VOUT_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pListenPort;
    AMBA_MON_LISTEN_VOUT_EVENT_s ListenEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pListenMsg;
    void *pMsg;

    (void) Param;
    pListenPort = &(AmbaMonListenVout_ListenPort);

    AmbaMonListenVout_Cmd = (UINT32) LISTEN_VOUT_TASK_CMD_STOP;

    while (AmbaMonListenVout_DummyFlag > 0U) {
        if (AmbaMonListenVout_Cmd == (UINT32) LISTEN_VOUT_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonListenVout_GraphDelete(pListenPort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVout_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vout task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonListenVout_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vout task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonListenVout_GraphCreate(pListenPort, &ListenEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vout port seek");
            }
            /* state reset */
            for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                LastVoutState[i].Isr = (UINT32) VOUT_STATE_IDLE;
            }
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonListenVout_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vout task ack");
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
                        AmbaMonListenVout_MsgProc(pListenMsg, &ListenEvent);
                    } else {
                        AmbaMonListenVout_TimeoutProc(pListenMsg, &ListenEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pListenPort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon listen vout port seek (reset)");
                    }
                    /* state reset */
                    for (i = 0U; i < AMBA_MON_NUM_VOUT_CHANNEL; i++) {
                        LastVoutState[i].Isr = (UINT32) VOUT_STATE_IDLE;
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

