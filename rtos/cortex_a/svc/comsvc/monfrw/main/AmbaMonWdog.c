/**
 *  @file AmbaMonWdog.c
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
 *  @details Amba Monitor Watchdog
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

#include "AmbaMonWdog.h"

#define OK_UL  ((UINT32) 0U)
#define NG_UL  ((UINT32) 1U)

typedef union /*_AMBA_MON_MAIN_WDOG_MEM_u_*/ {
    AMBA_MON_MAIN_MEM_ADDR    Data;
    void                      *pVoid;
    AMBA_MON_TIMEOUT_CB_s     *pTimeoutCb;
} AMBA_MON_WDOG_MEM_u;

typedef struct /*_AMBA_MON_MAIN_WDOG_MEM_s_*/ {
    AMBA_MON_WDOG_MEM_u    Ctx;
} AMBA_MON_WDOG_MEM_s;

static AMBA_KAL_TASK_t GNU_SECTION_NOZEROINIT AmbaMonWdogTaskId;
static void AmbaMonWdog_Task(UINT32 Param);
static void *AmbaMonWdog_TaskEx(void *pParam);

static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonWdog_Ready;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonWdog_Go;
static AMBA_KAL_SEMAPHORE_t GNU_SECTION_NOZEROINIT AmbaMonWdog_Ack;

static UINT32 AmbaMonWdog_Cmd = (UINT32) AMBA_MON_WDOG_TASK_CMD_STOP;

/**
 *  @private
 *  Amba monitor main watchdog create
 *  @param[in] Priority task priority id
 *  @param[in] CoreInclusion cpu core inclusion (bits)
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWdog_Create(UINT32 Priority, UINT32 CoreInclusion)
{
#define AMBA_MON_WDOG_TASK_STACK_SIZE    0xA000
    static UINT8 GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE AmbaMonWdogTaskStack[AMBA_MON_WDOG_TASK_STACK_SIZE];
    static char AmbaMonWdogTaskName[] = "MonitorWdog";

    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonWdog_Ready, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonWdog_Go, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreCreate(&AmbaMonWdog_Ack, NULL, 0U);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    if (RetCode == OK_UL) {
        AmbaMonPrintStr(S_PRINT_FLAG_MSG, "amba mon create watchdog task");
        {
            AMBA_MON_WDOG_MEM_s MemInfo;
            MemInfo.Ctx.Data = 0U;
            FuncRetCode = AmbaKAL_TaskCreate(&AmbaMonWdogTaskId,
                                             AmbaMonWdogTaskName,
                                             Priority,
                                             AmbaMonWdog_TaskEx,
                                             MemInfo.Ctx.pVoid,
                                             AmbaMonWdogTaskStack,
                                             AMBA_MON_WDOG_TASK_STACK_SIZE,
                                             AMBA_KAL_DONT_START);
        }
        if (FuncRetCode == KAL_ERR_NONE) {
            FuncRetCode = AmbaKAL_TaskSetSmpAffinity(&AmbaMonWdogTaskId, CoreInclusion);
            if (FuncRetCode != KAL_ERR_NONE) {
                RetCode = NG_UL;
            }
            FuncRetCode = AmbaKAL_TaskResume(&AmbaMonWdogTaskId);
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
 *  Amba monitor main watchdog delete
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWdog_Delete(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    FuncRetCode = AmbaKAL_TaskTerminate(&AmbaMonWdogTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_TaskDelete(&AmbaMonWdogTaskId);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonWdog_Ready);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonWdog_Go);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }
    FuncRetCode = AmbaKAL_SemaphoreDelete(&AmbaMonWdog_Ack);
    if (FuncRetCode != KAL_ERR_NONE) {
        RetCode = NG_UL;
    }

    return RetCode;
}

/**
 *  @private
 *  Amba monitor main watchdog active
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWdog_Active(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    if (AmbaMonWdog_Cmd == (UINT32) AMBA_MON_WDOG_TASK_CMD_STOP) {
        /* ready take */
        FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonWdog_Ready, AMBA_KAL_WAIT_FOREVER);
        if (FuncRetCode == KAL_ERR_NONE) {
            /* cmd start */
            AmbaMonWdog_Cmd = (UINT32) AMBA_MON_WDOG_TASK_CMD_START;
            /* go release */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonWdog_Go);
            if (FuncRetCode == KAL_ERR_NONE) {
                /* ack take */
                FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonWdog_Ack, AMBA_KAL_WAIT_FOREVER);
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
 *  Amba monitor main watchdog inactive
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWdog_Inactive(void)
{
    if (AmbaMonWdog_Cmd == (UINT32) AMBA_MON_WDOG_TASK_CMD_START) {
        AmbaMonWdog_Cmd = (UINT32) AMBA_MON_WDOG_TASK_CMD_STOP;
    }

    return OK_UL;
}

/**
 *  @private
 *  Amba monitor main watchdog idle wait
 *  @return error code
 *  @note this function is intended for internal use only
 */
UINT32 AmbaMonWdog_IdleWait(void)
{
    UINT32 RetCode = OK_UL;
    UINT32 FuncRetCode;

    /* ready take */
    FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonWdog_Ready, AMBA_KAL_WAIT_FOREVER);
    if (FuncRetCode == KAL_ERR_NONE) {
        /* ready give */
        FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonWdog_Ready);
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
 *  Amba monitor main watchdog task ex
 *  @param[in] pParam pointer to parameter
 *  @note this function is intended for internal use only
 */
static void *AmbaMonWdog_TaskEx(void *pParam)
{
    AMBA_MON_WDOG_MEM_s MemInfo;
    MemInfo.Ctx.pVoid = pParam;
    AmbaMonWdog_Task((UINT32) MemInfo.Ctx.Data);
    return NULL;
}

/**
 *  Amba monitor main watchdog task
 *  @param[in] Param (unused)
 *  @note this function is intended for internal use only
 */
static void AmbaMonWdog_Task(UINT32 Param)
{
    static UINT32 AmbaMonWdog_DummyFlag = 1U;

    UINT32 FuncRetCode;

    AMBA_MON_MESSAGE_PORT_s *pPort;
    UINT32 Period;
    void *pUser;

    AMBA_MON_WDOG_MEM_s MemInfo;

    (void) Param;

    AmbaMonWdog_Cmd = (UINT32) AMBA_MON_WDOG_TASK_CMD_STOP;

    while (AmbaMonWdog_DummyFlag > 0U) {
        if (AmbaMonWdog_Cmd == (UINT32) AMBA_MON_WDOG_TASK_CMD_STOP) {
            /* ready give */
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonWdog_Ready);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon watchdog task idle");
            }
            /* go take */
            FuncRetCode = AmbaKAL_SemaphoreTake(&AmbaMonWdog_Go, AMBA_KAL_WAIT_FOREVER);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon watchdog task go");
            }
            FuncRetCode = AmbaKAL_SemaphoreGive(&AmbaMonWdog_Ack);
            if (FuncRetCode != KAL_ERR_NONE) {
                AmbaMonPrintStr(S_PRINT_FLAG_ERR, "error: amba mon watchdog task ack");
            }
        }

        /* watchdog timeot get */
        FuncRetCode = AmbaMonMessage_TimeoutGet(&pPort, &Period, &pUser);
        if (FuncRetCode == OK_UL) {
            /* timeout */
            MemInfo.Ctx.pVoid = pUser;
            /* timeout msg put with watchdog refresh */
            (void) MemInfo.Ctx.pTimeoutCb->pFunc(MemInfo.Ctx.pTimeoutCb->Param.pCvoid);
            /* debug */
            AmbaMonPrintStr(S_PRINT_FLAG_WDOG, MemInfo.Ctx.pTimeoutCb->pName);
            AmbaMonPrintEx(S_PRINT_FLAG_WDOG, "period", Period, 16U);
            AmbaMonPrintEx(S_PRINT_FLAG_WDOG, "time2wait", pPort->WatchdogPort.Time2Wait, 16U);
        }
    }
}

