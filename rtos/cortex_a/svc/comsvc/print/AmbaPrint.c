/**
 *  @file AmbaPrint.c
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambarella Print System functions.
 *
 */

#include <AmbaKAL.h>
#include "AmbaPrint.h"
#include <AmbaUtility.h>
#include <AmbaWrap.h>
#include <AmbaMisraFix.h>

typedef enum {
    MESSAGE_TO_LOG_BUFFER = 0,
    MESSAGE_TO_PRINT_BUFFER,
    MESSAGE_TO_BOTH
} MESSAGE_DESTINATION_e;

typedef struct {
    UINT32 StrPoolSize;             /* Byte size of ready to print string buffer */
    UINT8 *pStrPool;                /* Pointer to ready to print string buffer */
    UINT32 WriteIdx;                /* The next write buffer position */
    UINT32 ReadIdx;                 /* The next read buffer position */
    UINT32  LineBreakPos[2048];     /* Ring buffer to store line message start position of pStrPool */
    UINT32  NextLineBreakIdx;       /* Array index to store the next line break position */
    UINT8   WrapAround;             // Indicate if the message have been wrap-around
} AMBA_PRINT_MSG_BUF_CTRL_s;

typedef struct {
    AMBA_KAL_TASK_t         Task;               /* Print Task */
    AMBA_KAL_EVENT_FLAG_t   EventFlag;
    AMBA_KAL_MUTEX_t        Mutex;
    AMBA_KAL_MUTEX_t        RtosMsgWriteMutex;
    AMBA_KAL_MUTEX_t        RtosMsgReadMutex;

    AMBA_PRINT_PUT_CHAR_f   OutPutFunc;
    AMBA_PRINT_MSG_BUF_CTRL_s PrintBufCtrl; // Print buffer control
    AMBA_PRINT_MSG_BUF_CTRL_s LogBufCtrl; // Log buffer control
    UINT8                   IsStopped;
} AMBA_PRINT_CTRL_s;

#define LINE_BUFFER_LENGTH      (4096U)
#define ALLOW_LIST_ENTRY_NUMBER (65536U/32U)
static AMBA_PRINT_CTRL_s AmbaPrintCtrl __attribute__((section(".bss.noinit")));
static char AmbaPrint_LineBuf[LINE_BUFFER_LENGTH] __attribute__((section(".bss.noinit"))); /* Temporary buffer */
static UINT32 module_allow_list[ALLOW_LIST_ENTRY_NUMBER] __attribute__((section(".bss.noinit")));

static void Print_ConsolePutChar(UINT32 TxDataSize, const char *pTxDataBuf)
{
    if (AmbaPrintCtrl.OutPutFunc != NULL) {
        AmbaPrintCtrl.OutPutFunc(TxDataSize, pTxDataBuf, AMBA_KAL_WAIT_FOREVER);
    }
    return;
}

static void Print_Flush(void)
{
    AMBA_PRINT_MSG_BUF_CTRL_s *pPrintBufCtrl = &AmbaPrintCtrl.PrintBufCtrl;
    UINT32 TxSize;

    if (pPrintBufCtrl->pStrPool == NULL) {
        /* not initialized */
    } else {
        UINT32 rIdx, wIdx;
        rIdx = pPrintBufCtrl->ReadIdx;
        wIdx = pPrintBufCtrl->WriteIdx;
        while (rIdx != wIdx) {
            if (wIdx > rIdx) {
                /* No wrap around */
                TxSize = wIdx - rIdx;
                Print_ConsolePutChar(TxSize, (char *)&pPrintBufCtrl->pStrPool[rIdx]);
            } else {
                /* Wrap around circular buffer */
                TxSize = pPrintBufCtrl->StrPoolSize - rIdx;
                Print_ConsolePutChar(TxSize, (char *)&pPrintBufCtrl->pStrPool[rIdx]);
            }

            if (TxSize > 0U) {
                rIdx += TxSize;
                if (rIdx >= pPrintBufCtrl->StrPoolSize) {
                    rIdx = 0;
                }
            }
        }
        pPrintBufCtrl->ReadIdx = wIdx;
    }
}

static void *Print_TaskEntry(void *EntryArg)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &AmbaPrintCtrl;
    UINT32 ActualFlags;
    UINT32 FlagLoop = 1;

    /* pass misra check */
    AmbaMisra_TouchUnused(EntryArg);

    while (FlagLoop == 1U) {
        if (AmbaKAL_EventFlagGet(&pPrintCtrl->EventFlag, 0x1, 0, 1, &ActualFlags, 500) != KAL_ERR_NONE) {
            // ignore this error
        }

        // if stopped, then no action
        if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
            if (AmbaKAL_MutexTake(&pPrintCtrl->RtosMsgReadMutex, 0) == KAL_ERR_NONE) {
                Print_Flush();
                if (AmbaKAL_MutexGive(&pPrintCtrl->RtosMsgReadMutex) != KAL_ERR_NONE) {
                    // ignore this error
                }
            }
        } else {
            FlagLoop = 0;
        }
    }
    return NULL;
}

/**
* print module init
* @param [in] pPrintConfig pointer to print config
* @return ErrorCode
*
*/
UINT32 AmbaPrint_Init(const AMBA_PRINT_CONFIG_s *pPrintConfig)
{
    AMBA_PRINT_CTRL_s *pPrintCtrl = &AmbaPrintCtrl;
    AMBA_PRINT_MSG_BUF_CTRL_s *pPrintBufCtrl = &AmbaPrintCtrl.PrintBufCtrl;
    AMBA_PRINT_MSG_BUF_CTRL_s *pLogBufCtrl = &AmbaPrintCtrl.LogBufCtrl;
    UINT32 RetVal = PRINT_ERR_SUCCESS;
    static char ambaprint_task_name[] = "AmbaPrintTask";
    static char ambaprint_mutex_name[] = "AmbaPrintMutex";
    static char ambaprint_write_mutex_name[] = "AmbaPrintMutexWrite";
    static char ambaprint_read_mutex_name[] = "AmbaPrintMutexRead";
    static char ambaprint_eventflag_name[] = "AmbaPrintEventFlag";

    if (AmbaWrap_memset(&AmbaPrintCtrl, 0, sizeof(AMBA_PRINT_CTRL_s)) != 0U) {
        RetVal = PRINT_ERR_RESOURCE_CREATE;
    }

    if (AmbaKAL_MutexCreate(&pPrintCtrl->Mutex, ambaprint_mutex_name) != KAL_ERR_NONE) {
        RetVal = PRINT_ERR_RESOURCE_CREATE;
    }

    if (AmbaKAL_MutexCreate(&pPrintCtrl->RtosMsgWriteMutex, ambaprint_write_mutex_name) != KAL_ERR_NONE) {
        RetVal = PRINT_ERR_RESOURCE_CREATE;
    }

    if (AmbaKAL_MutexCreate(&pPrintCtrl->RtosMsgReadMutex, ambaprint_read_mutex_name) != KAL_ERR_NONE) {
        RetVal =  PRINT_ERR_RESOURCE_CREATE;
    }

    if (AmbaKAL_EventFlagCreate(&pPrintCtrl->EventFlag, ambaprint_eventflag_name) != KAL_ERR_NONE) {
        RetVal =  PRINT_ERR_RESOURCE_CREATE;
    }

    if ((pPrintConfig->pPrintBuffer == NULL) || (pPrintConfig->PrintBufferSize == 0U)) {
        RetVal =  PRINT_ERR_INVALID_PARAMETER;
    }

    if ((pPrintConfig->pLogBuffer == NULL) || (pPrintConfig->LogBufferSize == 0U)) {
        if (pPrintConfig->PutCharFunc != NULL) {
            const char error_string_seperator[56] = "----------------------------------------------------\r\n";
            const char error_string1[48] = "AmbaPrint_Init(): Input parameters invalid!\r\n";
            const char error_string2[76] = "AmbaPrint_Init(): No pLogBuffer or LogBufferSize assigned for new APIs.\r\n";
            pPrintConfig->PutCharFunc(sizeof(error_string_seperator), error_string_seperator, 100);
            pPrintConfig->PutCharFunc(sizeof(error_string1), error_string1, 100);
            pPrintConfig->PutCharFunc(sizeof(error_string2), error_string2, 100);
            pPrintConfig->PutCharFunc(sizeof(error_string_seperator), error_string_seperator, 100);
        }
        RetVal =  PRINT_ERR_INVALID_PARAMETER;
    }

    if (pPrintConfig->PutCharFunc == NULL) {
        RetVal =  PRINT_ERR_INVALID_PARAMETER;
    }

    if (RetVal == PRINT_ERR_SUCCESS) {
        pPrintCtrl->OutPutFunc = pPrintConfig->PutCharFunc;
        pPrintCtrl->IsStopped = PRINT_COND_NO;
        pPrintBufCtrl->pStrPool = pPrintConfig->pPrintBuffer;
        pPrintBufCtrl->StrPoolSize = (UINT32)pPrintConfig->PrintBufferSize;
        pLogBufCtrl->pStrPool = pPrintConfig->pLogBuffer;
        pLogBufCtrl->StrPoolSize = (UINT32)pPrintConfig->LogBufferSize;

        // set module allow list to false
        {
            UINT32 i;
            for (i = 0; i < ALLOW_LIST_ENTRY_NUMBER; i++) {
                module_allow_list[i] = 0;
            }
        }

        AmbaUtility_MemorySetU8(pPrintBufCtrl->pStrPool, 0x0, pPrintBufCtrl->StrPoolSize);
        AmbaUtility_MemorySetU8(pLogBufCtrl->pStrPool, 0x0, pLogBufCtrl->StrPoolSize);

        pPrintBufCtrl->ReadIdx = 0;
        pPrintBufCtrl->WriteIdx = 0;
        pPrintBufCtrl->WrapAround = PRINT_COND_NO;

        pLogBufCtrl->ReadIdx = 0;
        pLogBufCtrl->WriteIdx = 0;
        pLogBufCtrl->WrapAround = PRINT_COND_NO;

        AmbaUtility_MemorySetU32(pPrintBufCtrl->LineBreakPos, 0, (UINT32)(sizeof(pPrintBufCtrl->LineBreakPos)));
        AmbaUtility_MemorySetU32(pLogBufCtrl->LineBreakPos, 0, (UINT32)(sizeof(pLogBufCtrl->LineBreakPos)));

        pPrintBufCtrl->NextLineBreakIdx = 0;
        pLogBufCtrl->NextLineBreakIdx = 0;

        RetVal = AmbaKAL_TaskCreate(&AmbaPrintCtrl.Task,           /* pTask */
                                    ambaprint_task_name,            /* pTaskName */
                                    pPrintConfig->TaskPriority,     /* Priority */
                                    Print_TaskEntry,                /* EntryFunction */
                                    NULL,                           /* EntryArg */
                                    pPrintConfig->pTaskStack,       /* pStackBase */
                                    pPrintConfig->TaskStackSize,    /* StackByteSize */
                                    0);                             /* AutoStart */

        if (RetVal != KAL_ERR_NONE) {
            /* can't create task */
        } else {
            RetVal = AmbaKAL_TaskSetSmpAffinity(&AmbaPrintCtrl.Task, pPrintConfig->SmpCoreSet);
            if (RetVal != KAL_ERR_NONE) {
                /* can't set task affinity */
            }
        }
        RetVal = AmbaKAL_TaskResume(&AmbaPrintCtrl.Task);
    }

    return RetVal;
}

/**
* print flush
* @return none
*
*/
void AmbaPrint_Flush(void)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.RtosMsgReadMutex, 0xFFFFFFFFU) != KAL_ERR_NONE) {
            /* The call in interrupt context cause TX_WAIT_ERROR/TX_CALLER_ERROR */
        } else {
            Print_Flush();
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.RtosMsgReadMutex) != KAL_ERR_NONE) {
                // ignore error
            }
        }
    }
}

static void Print_MessageCopy(UINT8 *pDestination, const char *pSource, UINT32 Size)
{
    UINT32 i;
    for (i = 0; i < Size; i++) {
        pDestination[i] = (UINT8)pSource[i];
    }
}

static void Print_InsertMsg(UINT32 MsgSize, const char *pMsg, MESSAGE_DESTINATION_e MessageDestination)
{
    AMBA_KAL_MUTEX_t *pMutex = &AmbaPrintCtrl.RtosMsgWriteMutex;
    AMBA_PRINT_MSG_BUF_CTRL_s *pPrintBufCtrl;
    UINT32 Head;
    UINT32 Tmp;

    if ((pMsg == NULL) || (MsgSize == 0U) || (AmbaPrintCtrl.PrintBufCtrl.pStrPool == NULL) || (AmbaPrintCtrl.LogBufCtrl.pStrPool == NULL)) {
        // no action
    } else {
        // lock mutex
        if (AmbaKAL_MutexTake(pMutex, 0xFFFFFFFFU) != KAL_ERR_NONE) {
            //no action
        } else {
            if ((MessageDestination == MESSAGE_TO_PRINT_BUFFER) || (MessageDestination == MESSAGE_TO_BOTH)) {
                pPrintBufCtrl = &AmbaPrintCtrl.PrintBufCtrl;
                /* Update to Print circular buffer in memory */
                Head = pPrintBufCtrl->WriteIdx;
                if ((Head + MsgSize) >= pPrintBufCtrl->StrPoolSize) {
                    /* Wrap around circular buffer */
                    Tmp = (pPrintBufCtrl->StrPoolSize - 1U) - Head;
                    pPrintBufCtrl->WriteIdx = MsgSize - Tmp;
                    Print_MessageCopy(&pPrintBufCtrl->pStrPool[Head], &pMsg[0], Tmp);
                    Print_MessageCopy(&pPrintBufCtrl->pStrPool[0], &pMsg[Tmp], MsgSize - Tmp);
                    pPrintBufCtrl->WrapAround = PRINT_COND_YES;
                } else {
                    /* No wrap */
                    pPrintBufCtrl->WriteIdx = Head + MsgSize;
                    Print_MessageCopy(&pPrintBufCtrl->pStrPool[Head], pMsg, MsgSize);
                }
            }

            if ((MessageDestination == MESSAGE_TO_LOG_BUFFER) || (MessageDestination == MESSAGE_TO_BOTH)) {
                pPrintBufCtrl = &AmbaPrintCtrl.LogBufCtrl;
                /* Update to Log circular buffer in memory */
                Head = pPrintBufCtrl->WriteIdx;
                if ((Head + MsgSize) >= pPrintBufCtrl->StrPoolSize) {
                    /* Wrap around circular buffer */
                    Tmp = (pPrintBufCtrl->StrPoolSize - 1U) - Head;
                    pPrintBufCtrl->WriteIdx = MsgSize - Tmp;
                    Print_MessageCopy(&pPrintBufCtrl->pStrPool[Head], &pMsg[0], Tmp);
                    Print_MessageCopy(&pPrintBufCtrl->pStrPool[0], &pMsg[Tmp], MsgSize - Tmp);
                    pPrintBufCtrl->WrapAround = PRINT_COND_YES;
                } else {
                    /* No wrap */
                    pPrintBufCtrl->WriteIdx = Head + MsgSize;
                    Print_MessageCopy(&pPrintBufCtrl->pStrPool[Head], pMsg, MsgSize);
                }
            }

            // unlock mutex
            if (AmbaKAL_MutexGive(pMutex) != KAL_ERR_NONE) {
                // ignore error
            }
        }
    }
    return;
}

static void Print_SetFooter(MESSAGE_DESTINATION_e MessageDestination)
{
    AMBA_PRINT_MSG_BUF_CTRL_s *pPrintBufCtrl = &AmbaPrintCtrl.PrintBufCtrl;
    static UINT32 MaxSize = (UINT32)((sizeof(pPrintBufCtrl->LineBreakPos) / sizeof((pPrintBufCtrl->LineBreakPos)[0])));

    static const char FooterString[8] = "\033""[0m\r\n";   /* Close the color control */

    Print_InsertMsg((UINT32)AmbaUtility_StringLength(FooterString), &FooterString[0], MessageDestination);

    if (pPrintBufCtrl->NextLineBreakIdx == (MaxSize - 1U)) {
        pPrintBufCtrl->NextLineBreakIdx = 0;
    } else {
        pPrintBufCtrl->NextLineBreakIdx++;
    }

    pPrintBufCtrl->LineBreakPos[pPrintBufCtrl->NextLineBreakIdx] = pPrintBufCtrl->WriteIdx;

    if (AmbaKAL_EventFlagSet(&AmbaPrintCtrl.EventFlag, 0x1) != KAL_ERR_NONE) {
        // ignore error
    }
}

static void Print_SetHeader(MESSAGE_DESTINATION_e MessageDestination)
{
    static const char * const pCpuCoreName[4] = {"CPU0", "CPU1", "CPU2", "CPU3"};

    UINT32 CpuID;
    char   HeaderString[40];
    SIZE_t HeaderSize;
    UINT32 Ticks = 0;
    char IntString[UTIL_MAX_INT_STR_LEN];

    if (AmbaKAL_GetSmpCpuID(&CpuID) != 0U) {
        CpuID = 0;
    }

    HeaderString[0] = '\0';
    AmbaUtility_StringAppend(HeaderString, 40, "\033""[0m");

    /* Msg prefix: System time */

    AmbaUtility_StringAppend(HeaderString, 40, "[");
    if (AmbaKAL_GetSysTickCount(&Ticks) != KAL_ERR_NONE) {
        // ignore this error
        Ticks = 0;
    }
    HeaderSize = AmbaUtility_UInt32ToStr(IntString, UTIL_MAX_INT_STR_LEN, Ticks, 10);
    if (HeaderSize < 8U) {
        SIZE_t i;
        for (i = 0; i < (8U - HeaderSize); i++) {
            AmbaUtility_StringAppend(HeaderString, 40, "0");
        }
    }
    AmbaUtility_StringAppend(HeaderString, 40, IntString);
    AmbaUtility_StringAppend(HeaderString, 40, "]");

    /* Msg prefix: Processor name */

    AmbaUtility_StringAppend(HeaderString, 40, "[");
    if (CpuID < 4U) {
        AmbaUtility_StringAppend(HeaderString, 40, pCpuCoreName[CpuID]);
    } else {
        AmbaUtility_StringAppend(HeaderString, 40, "CAXX_X?");
    }
    AmbaUtility_StringAppend(HeaderString, 40, "]");


    HeaderSize = AmbaUtility_StringLength(HeaderString);
    HeaderString[HeaderSize] = ' ';
    HeaderSize++;
    HeaderString[HeaderSize] = '\0';
    HeaderSize++;

    Print_InsertMsg((UINT32)HeaderSize, &HeaderString[0], MessageDestination);
}

/**
* This function is like C standard function printf() but all the inputs are type of string and maximum 5 arguments are allowed.
* @param [in] pFmt pointer to desired print format
* @param [in] pArg1 pointer to string arg 1
* @param [in] pArg2 pointer to string arg 2
* @param [in] pArg3 pointer to string arg 3
* @param [in] pArg4 pointer to string arg 4
* @param [in] pArg5 pointer to string arg 5
* @return none
*
*/
void AmbaPrint_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, KAL_WAIT_FOREVER) == KAL_ERR_NONE) {

            const char *Arg[5];
            UINT32  RetVal;

            Print_SetHeader(MESSAGE_TO_BOTH);

            Arg[0] = pArg1;
            Arg[1] = pArg2;
            Arg[2] = pArg3;
            Arg[3] = pArg4;
            Arg[4] = pArg5;
            RetVal = AmbaUtility_StringPrintStr(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, MESSAGE_TO_BOTH);
            Print_SetFooter(MESSAGE_TO_BOTH);

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* This function is like C standard function printf() but all the inputs are type of UINT64 and a maximum of five arguments are allowed.
* @param [in] pFmt pointer to desired print format
* @param [in] Arg1 Argument 1
* @param [in] Arg2 Argument 2
* @param [in] Arg3 Argument 3
* @param [in] Arg4 Argument 4
* @param [in] Arg5 Argument 5
* @return none
*
*/
void AmbaPrint_PrintULL5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            UINT64 Arg[5];
            UINT32  RetVal;

            Print_SetHeader(MESSAGE_TO_BOTH);

            Arg[0] = Arg1;
            Arg[1] = Arg2;
            Arg[2] = Arg3;
            Arg[3] = Arg4;
            Arg[4] = Arg5;
            RetVal = AmbaUtility_StringPrintUInt64(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, MESSAGE_TO_BOTH);
            Print_SetFooter(MESSAGE_TO_BOTH);

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* This function is like C standard function printf() but all the inputs are type of UINT32 and a maximum of five arguments are allowed.
* @param [in] pFmt pointer to desired print format
* @param [in] Arg1 Argument 1
* @param [in] Arg2 Argument 2
* @param [in] Arg3 Argument 3
* @param [in] Arg4 Argument 4
* @param [in] Arg5 Argument 5
* @return none
*
*/
void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            UINT32 Arg[5];
            UINT32  RetVal;

            Print_SetHeader(MESSAGE_TO_BOTH);

            Arg[0] = Arg1;
            Arg[1] = Arg2;
            Arg[2] = Arg3;
            Arg[3] = Arg4;
            Arg[4] = Arg5;
            RetVal = AmbaUtility_StringPrintUInt32(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, MESSAGE_TO_BOTH);
            Print_SetFooter(MESSAGE_TO_BOTH);

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* This function is like C standard function printf() but all the inputs are type of INT32 and a maximum of five arguments are allowed.
* @param [in] pFmt pointer to desired print format
* @param [in] Arg1 Argument 1
* @param [in] Arg2 Argument 2
* @param [in] Arg3 Argument 3
* @param [in] Arg4 Argument 4
* @param [in] Arg5 Argument 5
* @return none
*
*/
void AmbaPrint_PrintInt5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        /*-----------------------------------------------------------------------*\
         * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            INT32 Arg[5];
            UINT32  RetVal;

            Print_SetHeader(MESSAGE_TO_BOTH);

            Arg[0] = Arg1;
            Arg[1] = Arg2;
            Arg[2] = Arg3;
            Arg[3] = Arg4;
            Arg[4] = Arg5;
            RetVal = AmbaUtility_StringPrintInt32(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, MESSAGE_TO_BOTH);
            Print_SetFooter(MESSAGE_TO_BOTH);

            /*-----------------------------------------------------------------------*\
             * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* print module set allow list
* @param [in] ModuleID target print module ID
* @param [in] Enable enable or disable printing
* @return uret ErrorCode
*
*/
UINT32 AmbaPrint_ModuleSetAllowList(UINT16 ModuleID, UINT8 Enable)
{
    UINT32 uret = PRINT_ERR_SUCCESS;
    if (Enable > 1U) {
        uret = PRINT_ERR_INVALID_PARAMETER;
    } else {
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {
            UINT16 idx_main = ModuleID / 32U;
            UINT16 shift    = ModuleID % 32U;
            UINT32 value_1  = 1U;
            UINT32 value    = value_1 << shift;

            if (Enable == PRINT_COND_NO) {
                module_allow_list[idx_main] &= ~value;
            } else {
                module_allow_list[idx_main] |= value;
            }
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                uret = PRINT_ERR_MUTEX_UNLOCK_FAIL;
            }
        } else {
            uret = PRINT_ERR_MUTEX_LOCK_FAIL;
        }
    }
    return uret;
}

static UINT8 is_in_allow_list(UINT16 ModuleID)
{
    UINT16 idx_main = ModuleID / 32U;
    UINT16 shift    = ModuleID % 32U;
    UINT32 value    = (module_allow_list[idx_main] >> shift) & 0x01U;
    UINT8  ret = PRINT_COND_NO;
    if (value != 0U) {
        ret = PRINT_COND_YES;
    }
    return ret;
}

void AmbaPrint_StopAndFlush(void)
{
    const AMBA_PRINT_MSG_BUF_CTRL_s *pPrintBufCtrl = &AmbaPrintCtrl.LogBufCtrl;
    UINT32 tx_size;

    // To stop all print functions, just mark IsStopped to TRUE
    AmbaPrintCtrl.IsStopped = PRINT_COND_YES;

    // print all in log buffer
    if (pPrintBufCtrl->pStrPool == NULL) {
        /* not initialized */
    } else {
        // lock read mutex to wait until print task stopped
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.RtosMsgReadMutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {
            // ignore error
        }
        // even lock failed, we still need to print all messages for debug purpose.
        if (pPrintBufCtrl->WrapAround == PRINT_COND_YES) {
            // message have been wrap-around
            // +--------+
            // |        | Part 2
            // |        |
            // |        +---> write index
            // |        | Part 1
            // +--------+
            //
            tx_size = pPrintBufCtrl->StrPoolSize - pPrintBufCtrl->WriteIdx;
            Print_ConsolePutChar(tx_size, (char *)&pPrintBufCtrl->pStrPool[pPrintBufCtrl->WriteIdx]);
            tx_size = pPrintBufCtrl->WriteIdx;
            Print_ConsolePutChar(tx_size, (char *)&pPrintBufCtrl->pStrPool[0]);
        } else {
            // message is not wrap-around
            // print from 0 -> WriteIdx
            tx_size = pPrintBufCtrl->WriteIdx;
            Print_ConsolePutChar(tx_size, (char *)&pPrintBufCtrl->pStrPool[0]);
        }
        if (AmbaKAL_MutexGive(&AmbaPrintCtrl.RtosMsgReadMutex) != KAL_ERR_NONE) {
            // ignore error
        }
    }
}

/**
* print module string 5
* @param [in] ModuleID If the ID is in allow-list, then it will be printed to log and print buffer. Otherwise only the log buffer is printed.
* @param [in] pFmt pointer to desired print format
* @param [in] pArg1 pointer to string arg 1
* @param [in] pArg2 pointer to string arg 2
* @param [in] pArg3 pointer to string arg 3
* @param [in] pArg4 pointer to string arg 4
* @param [in] pArg5 pointer to string arg 5
* @return none
*
*/
void AmbaPrint_ModulePrintStr5(UINT16 ModuleID, const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {

        UINT8 is_allowed = is_in_allow_list(ModuleID);
        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

        if (is_allowed == PRINT_COND_YES) {
            message_destination = MESSAGE_TO_BOTH;
        }

        /*-----------------------------------------------------------------------*\
            * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            const char *Arg[5];
            UINT32  RetVal;

            Print_SetHeader(message_destination);

            Arg[0] = pArg1;
            Arg[1] = pArg2;
            Arg[2] = pArg3;
            Arg[3] = pArg4;
            Arg[4] = pArg5;
            RetVal = AmbaUtility_StringPrintStr(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, message_destination);
            Print_SetFooter(message_destination);

            /*-----------------------------------------------------------------------*\
                * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* This function is like AmbaPrint_PrintULL5() and applications should assign a module ID for print option.
* @param [in] ModuleID If the ID is in allow-list, then it will be printed to log and print buffer. Otherwise only log buffer is printed.
* @param [in] pFmt pointer to desired print format
* @param [in] Arg1 Argument 1
* @param [in] Arg2 Argument 2
* @param [in] Arg3 Argument 3
* @param [in] Arg4 Argument 4
* @param [in] Arg5 Argument 5
* @return none
*
*/
void AmbaPrint_ModulePrintULL5(UINT16 ModuleID, const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        UINT8 is_allowed = is_in_allow_list(ModuleID);
        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

        if (is_allowed == PRINT_COND_YES) {
            message_destination = MESSAGE_TO_BOTH;
        }

        /*-----------------------------------------------------------------------*\
            * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            UINT64 Arg[5];
            UINT32  RetVal;

            Print_SetHeader(message_destination);

            Arg[0] = Arg1;
            Arg[1] = Arg2;
            Arg[2] = Arg3;
            Arg[3] = Arg4;
            Arg[4] = Arg5;
            RetVal = AmbaUtility_StringPrintUInt64(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, message_destination);
            Print_SetFooter(message_destination);

            /*-----------------------------------------------------------------------*\
                * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* This function is like AmbaPrint_PrintUInt5() and applications should assign a module ID for print option.
* @param [in] ModuleID If the ID is in allow-list, then it will be printed to log and print buffer. Otherwise only log buffer is printed.
* @param [in] pFmt pointer to desired print format
* @param [in] Arg1 Argument 1
* @param [in] Arg2 Argument 2
* @param [in] Arg3 Argument 3
* @param [in] Arg4 Argument 4
* @param [in] Arg5 Argument 5
* @return none
*
*/
void AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        UINT8 is_allowed = is_in_allow_list(ModuleID);
        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

        if (is_allowed == PRINT_COND_YES) {
            message_destination = MESSAGE_TO_BOTH;
        }

        /*-----------------------------------------------------------------------*\
            * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            UINT32 Arg[5];
            UINT32  RetVal;

            Print_SetHeader(message_destination);

            Arg[0] = Arg1;
            Arg[1] = Arg2;
            Arg[2] = Arg3;
            Arg[3] = Arg4;
            Arg[4] = Arg5;
            RetVal = AmbaUtility_StringPrintUInt32(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, message_destination);
            Print_SetFooter(message_destination);

            /*-----------------------------------------------------------------------*\
                * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}

/**
* This function is like AmbaPrint_PrintInt5() and applications should assign a module ID for print option.
* @param [in] ModuleID If the ID is in allow-list, then it will be printed to log and print buffer. Otherwise only log buffer is printed.
* @param [in] pFmt pointer to desired print format
* @param [in] Arg1 Argument 1
* @param [in] Arg2 Argument 2
* @param [in] Arg3 Argument 3
* @param [in] Arg4 Argument 4
* @param [in] Arg5 Argument 5
* @return none
*
*/
void AmbaPrint_ModulePrintInt5(UINT16 ModuleID, const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    if (AmbaPrintCtrl.IsStopped == PRINT_COND_NO) {
        UINT8 is_allowed = is_in_allow_list(ModuleID);
        MESSAGE_DESTINATION_e message_destination = MESSAGE_TO_LOG_BUFFER;

        if (is_allowed == PRINT_COND_YES) {
            message_destination = MESSAGE_TO_BOTH;
        }

        /*-----------------------------------------------------------------------*\
            * Take the Mutex
        \*-----------------------------------------------------------------------*/
        if (AmbaKAL_MutexTake(&AmbaPrintCtrl.Mutex, 0xFFFFFFFFU) == KAL_ERR_NONE) {

            INT32 Arg[5];
            UINT32  RetVal;

            Print_SetHeader(message_destination);

            Arg[0] = Arg1;
            Arg[1] = Arg2;
            Arg[2] = Arg3;
            Arg[3] = Arg4;
            Arg[4] = Arg5;
            RetVal = AmbaUtility_StringPrintInt32(AmbaPrint_LineBuf, LINE_BUFFER_LENGTH, pFmt, 5, Arg);
            Print_InsertMsg(RetVal, AmbaPrint_LineBuf, message_destination);
            Print_SetFooter(message_destination);

            /*-----------------------------------------------------------------------*\
                * Release the Mutex
            \*-----------------------------------------------------------------------*/
            if (AmbaKAL_MutexGive(&AmbaPrintCtrl.Mutex) != KAL_ERR_NONE) {
                /* make misra happy */
            }
        }
    }
}
