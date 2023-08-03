/**
 *  @file AmbaMonStateDsp.c
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
 *  @details Amba Monitor State DSP
 *
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"

#include "AmbaDSP_Capability.h"

#include "AmbaMonFramework.h"
#include "AmbaMonDef.h"

#include "AmbaMonStateDsp.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_STATE_DSP_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR              Data;
    void                                *pVoid;
    AMBA_MON_MESSAGE_PORT_s             *pMsgPort;
    AMBA_MON_MESSAGE_HEADER_s           *pMsgHead;
    UINT8                               *pUint8;
    AMBA_MON_DSP_RAW_STATE_s            *pRawState;
    AMBA_MON_DSP_FOV_STATE_s            *pFovState;
    AMBA_MON_DSP_VOUT_STATE_s           *pVoutState;
    AMBA_MON_DSP_VIN_TIMEOUT_STATE_s    *pVinTimeout;
} AMBA_MON_STATE_DSP_MEM_u;

typedef struct /*_AMBA_MON_STATE_DSP_MEM_s_*/ {
    AMBA_MON_STATE_DSP_MEM_u    Ctx;
} AMBA_MON_STATE_DSP_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonStateDspTaskId;
static void AmbaMonStateDsp_Task(UINT32 Param);
static void *AmbaMonStateDsp_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateDsp_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateDsp_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonStateDsp_Ack;

static UINT32 AmbaMonStateDsp_Cmd = (UINT32) STATE_DSP_TASK_CMD_STOP;

static AMBA_MON_MESSAGE_PORT_s AmbaMonStateDsp_EchoPort = {0};

/**
 *  @private
 *  Amba monitor state dsp create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateDsp_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_STATE_DSP_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonStateDspTaskStack[AMBA_MON_STATE_DSP_TASK_STACK_SIZE];
    static char AmbaMonStateDspTaskName[] = "MonitorStDsp";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateDsp_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateDsp_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonStateDsp_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create state dsp task");
        {
            AMBA_MON_STATE_DSP_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonStateDspTaskId,
                                             AmbaMonStateDspTaskName,
                                             Priority,
                                             AmbaMonStateDsp_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonStateDspTaskStack,
                                             AMBA_MON_STATE_DSP_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonStateDspTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonStateDspTaskId);
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
 *  Amba monitor state dsp delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateDsp_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonStateDspTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonStateDspTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateDsp_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateDsp_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonStateDsp_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor state dsp active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateDsp_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonStateDsp_Cmd == (UINT32) STATE_DSP_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateDsp_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonStateDsp_Cmd = (UINT32) STATE_DSP_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateDsp_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateDsp_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor state dsp inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateDsp_Inactive(void)
{
    const void *pMem;

    AMBA_MON_MESSAGE_HEADER_s MsgHeader;

    if (AmbaMonStateDsp_Cmd == (UINT32) STATE_DSP_TASK_CMD_START) {
        AmbaMonStateDsp_Cmd = (UINT32) STATE_DSP_TASK_CMD_STOP;
        /* echo msg */
        MsgHeader.Ctx.Data[0] = 0ULL;
        MsgHeader.Ctx.Data[1] = 0ULL;
        MsgHeader.Ctx.Data[2] = 0ULL;
        MsgHeader.Ctx.Com.Id = (UINT8) AMBA_MON_MSG_ID_ECHO;
        pMem = AmbaMonMessage_Put(&AmbaMonStateDsp_EchoPort, &MsgHeader, (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s));
        if (pMem == NULL) {
            /* */
        }
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor state dsp idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonStateDsp_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateDsp_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateDsp_Ready);
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
 *  Amba monitor state dsp message process
 *  @param[in] pStateMsg pointer to the message header
 *  @param[in] pStateEvent pointer to the state dsp event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateDsp_MsgProc(AMBA_MON_MESSAGE_HEADER_s *pStateMsg, const AMBA_MON_STATE_DSP_EVENT_s *pStateEvent)
{
    AMBA_MON_STATE_DSP_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pStateMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pStateEvent->Flag == pStateEvent->RawStart.Flag) {
        /* raw start */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "raw start", MemInfo.Ctx.pRawState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->DefRawStart.Flag) {
        /* def raw start */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "def raw start", MemInfo.Ctx.pRawState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->FovStart.Flag) {
        /* fov start */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "dsp fov start", MemInfo.Ctx.pFovState->FovId);
        AmbaMonPrint(S_PRINT_FLAG_STATE, "fov latency", MemInfo.Ctx.pFovState->Latency);
    } else if (pStateEvent->Flag == pStateEvent->VoutStart.Flag) {
        /* dsp vout start */
        UINT32 FovSelectBits = MemInfo.Ctx.pVoutState->FovSelectBits;
        AmbaMonPrint(S_PRINT_FLAG_STATE, "dsp vout start", MemInfo.Ctx.pVoutState->VoutId);
        AmbaMonPrintEx(S_PRINT_FLAG_STATE, "disp seq num", (UINT32) (MemInfo.Ctx.pVoutState->DispSeqNum & 0xFFFFFFFFULL), 16U);
        AmbaMonPrint(S_PRINT_FLAG_STATE, "disp latency", (UINT32) MemInfo.Ctx.pVoutState->DispLatency);
        AmbaMonPrintEx(S_PRINT_FLAG_STATE, "fov bits", FovSelectBits, 16U);
        for (UINT32 i = 0U; (i < AMBA_MON_NUM_FOV_CHANNEL) && ((FovSelectBits >> i) > 0U); i++) {
            if ((FovSelectBits & (((UINT32) 1U) << i)) > 0U) {
                AmbaMonPrint2(S_PRINT_FLAG_STATE, "raw latency", i, MemInfo.Ctx.pVoutState->Latency[i]);
            }
        }
    } else if (pStateEvent->Flag == pStateEvent->VinTimeout.Flag) {
        /* dsp vin timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "dsp vin timeout", MemInfo.Ctx.pVinTimeout->VinId);
    } else {
        /* */
    }
}

/**
 *  Amba monitor state dsp timeout process
 *  @param[in] pStateMsg pointer to the message header
 *  @param[in] pStateEvent pointer to the state dsp event information
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateDsp_TimeoutProc(AMBA_MON_MESSAGE_HEADER_s *pStateMsg, const AMBA_MON_STATE_DSP_EVENT_s *pStateEvent)
{
    AMBA_MON_STATE_DSP_MEM_s MemInfo;
    UINT32 HeaderSize = (UINT32) sizeof(AMBA_MON_MESSAGE_HEADER_s);

    MemInfo.Ctx.pMsgHead = pStateMsg;
    MemInfo.Ctx.pUint8 = &(MemInfo.Ctx.pUint8[HeaderSize]);

    if (pStateEvent->Flag == pStateEvent->RawTimeout.Flag) {
        /* raw timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "raw timeout", MemInfo.Ctx.pRawState->VinId);
    } else if (pStateEvent->Flag == pStateEvent->FovTimeout.Flag) {
        /* fov timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "dsp fov timeout", MemInfo.Ctx.pFovState->FovId);
    } else if (pStateEvent->Flag == pStateEvent->VoutTimeout.Flag) {
        /* dsp vout timeout */
        AmbaMonPrint(S_PRINT_FLAG_STATE, "dsp vout timeout", MemInfo.Ctx.pVoutState->VoutId);
    } else {
        /* */
    }
}

/**
 *  Amba monitor state dsp graph delete
 *  @param[in] pStatePort pointer to message port
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateDsp_GraphDelete(AMBA_MON_MESSAGE_PORT_s *pStatePort)
{
    UINT32 FuncRetCode;

    /* echo port in state */
    FuncRetCode = AmbaMonMessage_Close(&AmbaMonStateDsp_EchoPort);
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state echo port close", FuncRetCode, 16U);
    }

    /* message port in state */
    FuncRetCode = AmbaMonMessage_Close(pStatePort);
    if (FuncRetCode != OK_UL) {
        /* state port fail */
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state port close", FuncRetCode, 16U);
    }
}

/**
 *  Amba monitor state dsp graph create
 *  @param[in] pStatePort pointer to message port
 *  @param[in] pStateEvent pointer to state event
 *  @param[in] pEventFlag pointer to accumulated event flag
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateDsp_GraphCreate(AMBA_MON_MESSAGE_PORT_s *pStatePort, AMBA_MON_STATE_DSP_EVENT_s *pStateEvent, UINT64 *pEventFlag)
{
    UINT32 FuncRetCode;

    AMBA_MON_STATE_DSP_MEM_s MemInfo;

    UINT32 DspEventId;
    UINT64 DspEventFlag;

    /* echo port in state */
    FuncRetCode = AmbaMonMessage_Open(&AmbaMonStateDsp_EchoPort, "dsp_state", "echo");
    if (FuncRetCode != OK_UL) {
        /* echo port fail */
        MemInfo.Ctx.pMsgPort = &AmbaMonStateDsp_EchoPort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state echo port", (UINT32) MemInfo.Ctx.Data, 16U);
    }

    /* message port in state */
    FuncRetCode = AmbaMonMessage_Open(pStatePort, "dsp_state", NULL);
    if (FuncRetCode == OK_UL) {
        /* state port success */
        MemInfo.Ctx.pMsgPort = pStatePort;
        AmbaMonPrintEx(S_PRINT_FLAG_DBG, "dsp state port", (UINT32) MemInfo.Ctx.Data, 16U);
        /* raw start event flag get */
        pStateEvent->RawStart.pName = "raw_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->RawStart.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->RawStart.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->RawStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->RawStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->RawStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* raw timeout event flag get */
        pStateEvent->RawTimeout.pName = "raw_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->RawTimeout.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->RawTimeout.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->RawTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->RawTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->RawTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* def raw start event flag get */
        pStateEvent->DefRawStart.pName = "def_raw_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->DefRawStart.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->DefRawStart.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->DefRawStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->DefRawStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->DefRawStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* fov start event flag get */
        pStateEvent->FovStart.pName = "fov_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->FovStart.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->FovStart.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->FovStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->FovStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->FovStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* fov timeout event flag get */
        pStateEvent->FovTimeout.pName = "fov_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->FovTimeout.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->FovTimeout.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->FovTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->FovTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->FovTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vout start event flag get */
        pStateEvent->VoutStart.pName = "vout_start";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->VoutStart.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->VoutStart.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->VoutStart.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->VoutStart.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->VoutStart.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vout timeout event flag get */
        pStateEvent->VoutTimeout.pName = "vout_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->VoutTimeout.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->VoutTimeout.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->VoutTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->VoutTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->VoutTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vin timeout event flag get */
        pStateEvent->VinTimeout.pName = "vin_timeout";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->VinTimeout.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->VinTimeout.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->VinTimeout.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->VinTimeout.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->VinTimeout.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
        /* vin timeout event flag get */
        pStateEvent->Echo.pName = "echo";
        FuncRetCode = AmbaMonMessage_Find(pStatePort, pStateEvent->Echo.pName, &DspEventId, &DspEventFlag);
        if (FuncRetCode == OK_UL) {
            *pEventFlag |= DspEventFlag;
            pStateEvent->Echo.Flag = DspEventFlag;
            AmbaMonPrintStr(S_PRINT_FLAG_DBG, pStateEvent->Echo.pName);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "  event flag", (UINT32) (DspEventFlag >> 32ULL), 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_DBG, "            ", (UINT32) DspEventFlag, 16U);
        } else {
            pStateEvent->Echo.Flag = 0ULL;
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, pStateEvent->Echo.pName);
            AmbaMonPrintStr(S_PRINT_FLAG_ERR, "  event flag fail");
        }
    } else {
        /* state port fail */
        MemInfo.Ctx.pMsgPort = pStatePort;
        AmbaMonPrintEx(S_PRINT_FLAG_ERR, "error: dsp state port", (UINT32) MemInfo.Ctx.Data, 16U);
    }
}

/**
 *  Amba monitor state dsp task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonStateDsp_TaskEx(void *pParam)
{
    AMBA_MON_STATE_DSP_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonStateDsp_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor state dsp task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonStateDsp_Task(UINT32 Param)
{
    static UINT32 AmbaMonStateDsp_DummyFlag = 1U;
    static AMBA_MON_MESSAGE_PORT_s AmbaMonStateDsp_StatePort = {0};

     UINT8 FirstActive = 1U;
    UINT32 FuncRetCode;

    UINT32 MsgRetCode;
    UINT64 ActualFlags;

    AMBA_MON_STATE_DSP_MEM_s MemInfo;

    UINT64 EventFlag = 0ULL;

    AMBA_MON_MESSAGE_PORT_s *pStatePort;
    AMBA_MON_STATE_DSP_EVENT_s StateEvent = {0};

    AMBA_MON_MESSAGE_HEADER_s *pStateMsg;
    void *pMsg;

    (void) Param;
    pStatePort = &(AmbaMonStateDsp_StatePort);

    AmbaMonStateDsp_Cmd = (UINT32) STATE_DSP_TASK_CMD_STOP;

    while (AmbaMonStateDsp_DummyFlag > 0U) {
        if (AmbaMonStateDsp_Cmd == (UINT32) STATE_DSP_TASK_CMD_STOP) {
            if (FirstActive == 1U) {
                FirstActive = 0U;
            } else {
                /* graph delete */
                AmbaMonStateDsp_GraphDelete(pStatePort);
            }
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateDsp_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state dsp task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonStateDsp_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state dsp task go");
            }
            /* even flag reset */
            EventFlag = 0ULL;
            /* graph create */
            AmbaMonStateDsp_GraphCreate(pStatePort, &StateEvent, &EventFlag);
            /* port reset */
            FuncRetCode = AmbaMonMessage_RSeek(pStatePort, 0U, (UINT32) R_SEEK_PASS_BEGIN);
            if(FuncRetCode != OK_UL) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state dsp port seek.");
            }
            /* ack give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonStateDsp_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state dsp task ack");
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
                        AmbaMonStateDsp_MsgProc(pStateMsg, &StateEvent);
                    } else {
                        AmbaMonStateDsp_TimeoutProc(pStateMsg, &StateEvent);
                    }
                } else if (MsgRetCode == MON_RING_RESET) {
                    /* port reset */
                    FuncRetCode = AmbaMonMessage_RSeek(pStatePort, 0U, (UINT32) R_SEEK_PASS_LAST);
                    if(FuncRetCode == OK_UL) {
                        MsgRetCode = OK_UL;
                    } else {
                        AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon state dsp port seek (reset)");
                    }
                } else {
                    /* */
                }
            } while (MsgRetCode == OK_UL);
        }
    }
}

