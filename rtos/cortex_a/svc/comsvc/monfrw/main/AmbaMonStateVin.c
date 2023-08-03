/**
 *  @file AmbaMonStateVin.c
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
 *  @details Amba Monitor State VIN
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDSP_Capability.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonStateVin.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_STATE_VIN_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR              Data;
    void                                *pVoid;
    AMBA_MON_MESSAGE_PORT_s             *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s           *pMsgHead;
    UINT8                               *pUint8;
    //UINT32                              *pUint32;
    AMBA_MON_DSP_RAW_STATE_s            *pRawState;
    AMBA_MON_VIN_ISR_STATE_s            *pVinIsrState;
    AMBA_MON_VIN_SERDES_STATE_s         *pVinSerdesState;
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
    AMBA_MON_AAA_CFA_STATE_s            *pAaaCfaState;
#endif
    AMBA_MON_DSP_VIN_TIMEOUT_STATE_s    *pVinTimeout;
} AMBA_MON_STATE_VIN_MEM_u;

typedef struct /*_AMBA_MON_STATE_VIN_MEM_s_*/ {
    AMBA_MON_STATE_VIN_MEM_u    Ctx;
} AMBA_MON_STATE_VIN_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonStateVinTaskId;
static void AmbaMonStateVin_Task(UINT32 Param);
static void *AmbaMonStateVin_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateVin_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateVin_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateVin_Ack;

static UINT32 AmbaMonStateVin_Cmd = (UINT32) STATE_VIN_TASK_CMD_STOP;

static AMBA_MON_MESSAGE_PORT_s AmbaMonStateVin_EchoPort = {0};

/**
 *  @private
 *  Amba monitor state vin create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVin_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_STATE_VIN_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonStateVinTaskStack[AMBA_MON_STATE_VIN_TASK_STACK_SIZE];
    static char AmbaMonStateVinTaskName[] = "MonitorStVin";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateVin_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateVin_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateVin_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create state vin task");
        {
            AMBA_MON_STATE_VIN_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonStateVinTaskId,
                                             AmbaMonStateVinTaskName,
                                             Priority,
                                             AmbaMonStateVin_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonStateVinTaskStack,
                                             AMBA_MON_STATE_VIN_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonStateVinTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonStateVinTaskId);
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
 *  Amba monitor state vin delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVin_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonStateVinTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonStateVinTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateVin_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateVin_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateVin_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor state vin active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVin_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonStateVin_Cmd == (UINT32) STATE_VIN_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVin_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonStateVin_Cmd = (UINT32) STATE_VIN_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVin_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVin_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor state vin inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVin_Inactive(void)
{
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s MsgHeader;

    if (AmbaMonStateVin_Cmd == (UINT32) STATE_VIN_TASK_CMD_START) {
        AmbaMonStateVin_Cmd = (UINT32) STATE_VIN_TASK_CMD_STOP;
        /* echo msg */
        MsgHeader.Ctx.Data[0] = 0ULL;
        MsgHeader.Ctx.Data[1] = 0ULL;
        MsgHeader.Ctx.Data[2] = 0ULL;
        MsgHeader.Ctx.Com.Id = (UINT8) AMBA_MON_MSG_ID_ECHO;
        pMem = AmbaMonMessage_Put(&AmbaMonStateVin_EchoPort, &MsgHeader, (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s));
        if (pMem == NULL) {
            /* */
        }
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor state vin idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateVin_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVin_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVin_Ready);
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
 *  Amba monitor state vin message process
 *  @param[in] pStateMsg pointer to the message header
 *  @param[in] pStateEvent pointer to the state vin event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVin_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pStateMsg, const AMBA_MON_STATE_VIN_EVENT_s *pStateEvent)
{
    AMBA_MON_STATE_VIN_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pStateMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pStateEvent->Flag == pStateEvent->SofStart.Flag) {
        /* sof start */
        AmbaMonPrintEx2(S_PRINT_FLAG_STATE, "sof start", MemInfo.Ctx.pVinIsrState->VinId, 10U, (UINT32) (MemInfo.Ctx.pVinIsrState->Dts & 0xFFFFFFFFULL), 16U);
    } else if (pStateEvent->Flag == pStateEvent->EofStart.Flag) {
        /* eof start */
        AmbaMonPrintEx2(S_PRINT_FLAG_STATE, "eof start", MemInfo.Ctx.pVinIsrState->VinId, 10U, (UINT32) (MemInfo.Ctx.pVinIsrState->Dts & 0xFFFFFFFFULL), 16U);
    } else if (pStateEvent->Flag == pStateEvent->RawStart.Flag) {
        /* raw start */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "vin raw start", MemInfo.Ctx.pRawState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->DefRawStart.Flag) {
        /* def raw start */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "vin def raw start", MemInfo.Ctx.pRawState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->SerdesLink.Flag) {
        /* serdes state */
        AmbaMonPrintEx2(S_PRINT_FLAG_STATE, "vin serdes state (normal)", MemInfo.Ctx.pVinSerdesState->VinId, 10U, MemInfo.Ctx.pVinSerdesState->LinkId, 16U);
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
    } else if (pStateEvent->Flag == pStateEvent->AaaCfaState.Flag) {
        /* aaa cfa state */
        AmbaMonPrint2(S_PRINT_FLAG_STATE, "vin aaa cfa state", MemInfo.Ctx.pAaaCfaState->FovId, (UINT32) MemInfo.Ctx.pAaaCfaState->StatusId.Bits.CdZero);
#endif
    } else if (pStateEvent->Flag == pStateEvent->VinTimeout.Flag) {
        /* vin timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "vin dsp vin timeout", MemInfo.Ctx.pVinTimeout->VinId);
    } else {
        /* */
    }
}

/**
 *  Amba monitor state vin timeout process
 *  @param[in] pStateMsg pointer to the message header
 *  @param[in] pStateEvent pointer to the state vin event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVin_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pStateMsg, const AMBA_MON_STATE_VIN_EVENT_s *pStateEvent)
{
    AMBA_MON_STATE_VIN_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pStateMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pStateEvent->Flag == pStateEvent->SofTimeout.Flag) {
        /* sof timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "sof timeout", MemInfo.Ctx.pVinIsrState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->EofTimeout.Flag) {
        /* eof timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "eof timeout", MemInfo.Ctx.pVinIsrState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->RawTimeout.Flag) {
        /* vin raw timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "vin raw timeout", MemInfo.Ctx.pRawState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->SerdesLink.Flag) {
        /* serdes state */
        AmbaMonPrintEx2(S_PRINT_FLAG_STATE, "vin serdes state (timeout)", MemInfo.Ctx.pVinSerdesState->VinId, 10U, MemInfo.Ctx.pVinSerdesState->LinkId, 16U);
    } else {
        /* */
    }
}

/**
 *  Amba monitor state vin graph delete
 *  @param[in] pStatePort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVin_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pStatePort)
{
    UINT32 FuncRetCode;

    /* echo port in state */
    FuncRetCode = AmbaMonMessage_Close(&AmbaMonStateVin_EchoPort);
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state echo port close", FuncRetCode, 16U);
    }

    /* message port in state */
    FuncRetCode = AmbaMonMessage_Close(pStatePort);
    if (FuncRetCode != OK_UL) {
        /* state port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor state vin graph create
 *  @param[in] pStatePort pointer to message port
 *  @param[in] pStateEvent pointer to state event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVin_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pStatePort, AMBA_MON_STATE_VIN_EVENT_s *pStateEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_STATE_VIN_MEM_s MemInfo;

    UINT32 VinEventId;
    UINT64 VinEventFlag;

    /* echo port in state */
    FuncRetCode = AmbaMonMessage_Open(&AmbaMonStateVin_EchoPort, "vin_state", "echo");
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        MemInfo.Ctx.pMsgPort = &AmbaMonStateVin_EchoPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state echo port", (UINT32) MemInfo.Ctx.Data, 16U);
    }

    /* message port in state */
    FuncRetCode = AmbaMonMessage_Open(pStatePort, "vin_state", NULL);
    if (FuncRetCode == OK_UL) {
        /* state port success */
        MemInfo.Ctx.pMsgPort = pStatePort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "vin state port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* sof start event flag get */
        pStateEvent->SofStart.pName = "sof_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->SofStart.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->SofStart.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->SofStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->SofStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->SofStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* sof timeout event flag get */
        pStateEvent->SofTimeout.pName = "sof_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->SofTimeout.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->SofTimeout.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->SofTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->SofTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->SofTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* eof start event flag get */
        pStateEvent->EofStart.pName = "eof_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->EofStart.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->EofStart.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->EofStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->EofStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->EofStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* eof timeout event flag get */
        pStateEvent->EofTimeout.pName = "eof_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->EofTimeout.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->EofTimeout.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->EofTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->EofTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->EofTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* raw start event flag get */
        pStateEvent->RawStart.pName = "raw_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->RawStart.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->RawStart.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->RawStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->RawStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->RawStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* raw timeout event flag get */
        pStateEvent->RawTimeout.pName = "raw_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->RawTimeout.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->RawTimeout.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->RawTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->RawTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->RawTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* def raw start event flag get */
        pStateEvent->DefRawStart.pName = "def_raw_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->DefRawStart.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->DefRawStart.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->DefRawStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->DefRawStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->DefRawStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* serdes link event flag get */
        pStateEvent->SerdesLink.pName = "serdes_link";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->SerdesLink.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->SerdesLink.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->SerdesLink.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->SerdesLink.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->SerdesLink.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  no event flag");
        }
#ifdef AMBA_MON_LISTEN_AAA_CHANGE_DETECTION
        /* aaa cfa state event flag get */
        pStateEvent->AaaCfaState.pName = "aaa_cfa";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->AaaCfaState.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->AaaCfaState.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->AaaCfaState.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->AaaCfaState.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->AaaCfaState.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
#endif
        /* vin timeout event flag get */
        pStateEvent->VinTimeout.pName = "vin_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->VinTimeout.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->VinTimeout.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->VinTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->VinTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->VinTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vin echo event flag get */
        pStateEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->Echo.pName, &VinEventId, &VinEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= VinEventFlag;
            pStateEvent->Echo.Flag = VinEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (VinEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) VinEventFlag, 16U);
        } else {
            pStateEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* state port fail */
        MemInfo.Ctx.pMsgPort = pStatePort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: vin state port", (UINT32) MemInfo.Ctx.Data, 16U);
    }
}

/**
 *  Amba monitor state vin task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonStateVin_TaskEx(void *pParam)
{
    AMBA_MON_STATE_VIN_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonStateVin_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor state vin task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateVin_Task(UINT32 Param)
{
    static UINT32 AmbaMonStateVin_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonStateVin_StatePort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_STATE_VIN_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pStatePort;
    AMBA_MON_STATE_VIN_EVENT_s StateEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pStateMsg;
    void *pMsg;

    (void) Param;
    pStatePort = &(AmbaMonStateVin_StatePort);

    AmbaMonStateVin_Cmd = (UINT32) STATE_VIN_TASK_CMD_STOP;

    while (AmbaMonStateVin_DummyFlag > 0U) {
        if (AmbaMonStateVin_Cmd == (UINT32) STATE_VIN_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonStateVin_GraphDelete(pStatePort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVin_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vin task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateVin_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vin task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonStateVin_GraphCreate(pStatePort, &StateEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pStatePort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vin port seek");
            }
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateVin_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vin task ack");
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
                        AmbaMonStateVin_MsgProc(pStateMsg, &StateEvent);
                    } else {
                        AmbaMonStateVin_TimeoutProc(pStateMsg, &StateEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pStatePort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state vin port seek (reset)");
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

