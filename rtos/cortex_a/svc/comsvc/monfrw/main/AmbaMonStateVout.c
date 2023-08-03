/**
 *  @file AmbaMonStateVout.c
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
 *  @details Amba Monitor State VOUT
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDSP_Capability.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonStateVout.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_STATE_VOUT_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR       Data;
    void                         *pVoid;
    AMBA_MON_MESSAGE_PORT_s      *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s    *pMsgHead;
    UINT8                        *pUint8;
    UINT32                       *pUint32;
} AMBA_MON_STATE_VOUT_MEM_u;

typedef struct /*_AMBA_MON_STATE_VOUT_MEM_s_*/ {
    AMBA_MON_STATE_VOUT_MEM_u    Ctx;
} AMBA_MON_STATE_VOUT_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonStateVoutTaskId;
static void AmbaMonStateVout_Task(UINT32 Param);
static void *AmbaMonStateVout_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateVout_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateVout_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateVout_Ack;

static UINT32 AmbaMonStateVout_Cmd = (UINT32) STATE_VOUT_TASK_CMD_STOP;

static AMBA_MON_MESSAGE_PORT_s AmbaMonStateVout_EchoPort = {0};

/**
 *  @private
 *  Amba monitor state vout create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVout_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_STATE_VOUT_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonStateVoutTaskStack[AMBA_MON_STATE_VOUT_TASK_STACK_SIZE];
    static char AmbaMonStateVoutTaskName[] = "MonitorStVout";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateVout_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateVout_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateVout_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create state vout task");
        {
            AMBA_MON_STATE_VOUT_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonStateVoutTaskId,
                                             AmbaMonStateVoutTaskName,
                                             Priority,
                                             AmbaMonStateVout_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonStateVoutTaskStack,
                                             AMBA_MON_STATE_VOUT_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonStateVoutTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonStateVoutTaskId);
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
 *  Amba monitor state vout delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVout_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonStateVoutTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonStateVoutTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateVout_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateVout_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateVout_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor state vout active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVout_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonStateVout_Cmd == (UINT32) STATE_VOUT_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVout_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonStateVout_Cmd = (UINT32) STATE_VOUT_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVout_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVout_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor state vout inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVout_Inactive(void)
{
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s MsgHeader;

    if (AmbaMonStateVout_Cmd == (UINT32) STATE_VOUT_TASK_CMD_START) {
        AmbaMonStateVout_Cmd = (UINT32) STATE_VOUT_TASK_CMD_STOP;
        /* echo msg */
        MsgHeader.Ctx.Data[0] = 0ULL;
        MsgHeader.Ctx.Data[1] = 0ULL;
        MsgHeader.Ctx.Data[2] = 0ULL;
        MsgHeader.Ctx.Com.Id = (UINT8) AMBA_MON_MSG_ID_ECHO;
        pMem = AmbaMonMessage_Put(&AmbaMonStateVout_EchoPort, &MsgHeader, (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s));
        if (pMem == NULL) {
            /* */
        }
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor state vout idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVout_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVout_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVout_Ready);
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
 *  Amba monitor state vout message process
 *  @param[in] pStateMsg pointer to the message header
 *  @param[in] pStateEvent pointer to the state vout event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVout_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pStateMsg, const AMBA_MON_STATE_VOUT_EVENT_s *pStateEvent)
{
    AMBA_MON_STATE_VOUT_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pStateMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pStateEvent->Flag == pStateEvent->IsrStart.Flag) {
        /* vout isr start */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "vout isr start", *(MemInfo.Ctx.pUint32));
    } else {
        /* */
    }
}

/**
 *  Amba monitor state vout timeout process
 *  @param[in] pStateMsg pointer to the message header
 *  @param[in] pStateEvent pointer to the state vout event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVout_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pStateMsg, const AMBA_MON_STATE_VOUT_EVENT_s *pStateEvent)
{
    AMBA_MON_STATE_VOUT_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pStateMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pStateEvent->Flag == pStateEvent->IsrTimeout.Flag) {
        /* vout isr timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "vout isr timeout", *(MemInfo.Ctx.pUint32));
    } else {
        /* */
    }
}

/**
 *  Amba monitor state vout graph delete
 *  @param[in] pStatePort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVout_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pStatePort)
{
    UINT32 FuncRetCode;

    /* echo port in state */
    FuncRetCode = AmbaMonMessage_Close(&AmbaMonStateVout_EchoPort);
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state echo port close", FuncRetCode, 16U);
    }

    /* message port in state */
    FuncRetCode = AmbaMonMessage_Close(pStatePort);
    if (FuncRetCode != OK_UL) {
        /* state port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor state vout graph create
 *  @param[in] pStatePort pointer to message port
 *  @param[in] pStateEvent pointer to state event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVout_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pStatePort, AMBA_MON_STATE_VOUT_EVENT_s *pStateEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_STATE_VOUT_MEM_s MemInfo;

    UINT32 VoutEventId;
    UINT64 VoutEventFlag;

    /* echo port in state */
    FuncRetCode = AmbaMonMessage_Open(&AmbaMonStateVout_EchoPort, "vout_state", "echo");
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        MemInfo.Ctx.pMsgPort = &AmbaMonStateVout_EchoPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state echo port", (UINT32) MemInfo.Ctx.Data, 16U);
    }

    /* message port in state */
    FuncRetCode = AmbaMonMessage_Open(pStatePort, "vout_state", NULL);
    if (FuncRetCode == OK_UL) {
        /* state port success */
        MemInfo.Ctx.pMsgPort = pStatePort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vout state port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* isr start event flag get */
        pStateEvent->IsrStart.pName = "isr_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->IsrStart.pName, &VoutEventId, &VoutEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VoutEventFlag;
            pStateEvent->IsrStart.Flag = VoutEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->IsrStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VoutEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VoutEventFlag, 16U);
        } else {
            pStateEvent->IsrStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->IsrStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* isr timeout event flag get */
        pStateEvent->IsrTimeout.pName = "isr_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->IsrTimeout.pName, &VoutEventId, &VoutEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VoutEventFlag;
            pStateEvent->IsrTimeout.Flag = VoutEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->IsrTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VoutEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VoutEventFlag, 16U);
        } else {
            pStateEvent->IsrTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->IsrTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* echo event flag get */
        pStateEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->Echo.pName, &VoutEventId, &VoutEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VoutEventFlag;
            pStateEvent->Echo.Flag = VoutEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VoutEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VoutEventFlag, 16U);
        } else {
            pStateEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* state port fail */
        MemInfo.Ctx.pMsgPort = pStatePort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vout state port", (UINT32) MemInfo.Ctx.Data, 16U);
    }
}

/**
 *  Amba monitor state vout task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonStateVout_TaskEx(void *pParam)
{
    AMBA_MON_STATE_VOUT_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonStateVout_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor state vout task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVout_Task(UINT32 Param)
{
    static UINT32 AmbaMonStateVout_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonStateVout_StatePort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_STATE_VOUT_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pStatePort;
    AMBA_MON_STATE_VOUT_EVENT_s StateEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pStateMsg;
    void *pMsg;

    (void) Param;
    pStatePort = &(AmbaMonStateVout_StatePort);

    AmbaMonStateVout_Cmd = (UINT32) STATE_VOUT_TASK_CMD_STOP;

    while (AmbaMonStateVout_DummyFlag > 0U) {
        if (AmbaMonStateVout_Cmd == (UINT32) STATE_VOUT_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonStateVout_GraphDelete(pStatePort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVout_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vout task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVout_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vout task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonStateVout_GraphCreate(pStatePort, &StateEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pStatePort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vout port seek");
            }
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVout_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vout task ack");
            }
        }

        ActualFlags = AmbaMonMessage_Wait(pStatePort, EventFlag, (UINT32) MON_EVENT_FLAG_OR_CLR, (UINT32) AMBA_KAL_WAIT_FOREVER);
        if ((ActualFlags & EventFlag) > 0ULL) {
            do {
                MsgRetCode = AmbaMonMessage_Get(pStatePort, &pMsg);
                if (MsgRetCode == OK_UL) {
                    /* msg get */
                    MemInfo.Ctx.pVoid = pMsg;
                    /* event flag get */
                    StateEvent.Flag = MemInfo.Ctx.pMsgHead->Ctx.Event.Flag;
                    /* link check */
                    if (MemInfo.Ctx.pMsgHead->Ctx.Com.Id == (UINT8) AMBA_MON_MSG_ID_LINK) {
                        MemInfo.Ctx.Data = (AMBA_MON_MAIN_MEM_ADDR) MemInfo.Ctx.pMsgHead->Ctx.Link.Pointer;
                    }
                    /* chunk itself */
                    pStateMsg = MemInfo.Ctx.pMsgHead;
                    /* msg parse */
                    if (pStateMsg->Ctx.Chunk.Timeout == 0U) {
                        AmbaMonStateVout_MsgProc(pStateMsg, &StateEvent);
                    } else {
                        AmbaMonStateVout_TimeoutProc(pStateMsg, &StateEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pStatePort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vout port seek (reset)");
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

