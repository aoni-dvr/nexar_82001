/**
 *  @file AmbaBackTrace.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Back Trace related functions
 *
 */

#include "AmbaMisraFix.h"
#include "AmbaSYS.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaUtility.h"

#include "AmbaKAL.h"
#include "AmbaIOUTDiag.h"
#include "AmbaRTSL_UART.h"

#ifndef AMBA_DEF_H
#include <AmbaDef.h>
#endif

#ifdef  CONFIG_XEN_SUPPORT
#include <AmbaXen.h>
#define amba_xen_print_buf(...)
#endif

#ifndef CONFIG_EXCEPTION_BACKTRACE_LEVEL
#define CONFIG_EXCEPTION_BACKTRACE_LEVEL 1U
#endif

#ifndef AMBA_UART_A53_CONSOLE
#define AMBA_UART_A53_CONSOLE       AMBA_UART_APB_CHANNEL0
#endif // TODO: set in all chip

UINT        _tx_thread_smp_core_get(void);
TX_THREAD   *_tx_thread_smp_current_thread_get(void);

char * AmbaDbgSym_FindFunc(UINT32 Addr, UINT32 *pOffset);

char * GNU_WEAK_SYMBOL AmbaDbgSym_FindFunc(UINT32 Addr, UINT32 *pOffset)
{
    AmbaMisra_TouchUnused(&Addr);
    AmbaMisra_TouchUnused(pOffset);
    return NULL;
}

static void Exception_Print(const char *pFmt)
{
    static UINT8 PrintBuf[128] GNU_SECTION_NOZEROINIT;
    UINT32 ActualTxSize, UartTxSize, Index = 0U;
    char *pString;
    const UINT8 *pBuf = PrintBuf;
    const char *pArg = NULL;

    AmbaMisra_TypeCast(&pString, &pBuf);
    UartTxSize = AmbaUtility_StringPrintStr(pString, 128U, pFmt, (UINT32)1U, &pArg);

#ifdef CONFIG_XEN_SUPPORT
    if (AmbaXen_is_guest()) {
        amba_xen_print_buf((const char *) PrintBuf, UartTxSize);
    } else {
#else
    {
#endif
        while (UartTxSize > 0U)
        {
            if (AmbaRTSL_UartWrite(AMBA_UART_A53_CONSOLE, UartTxSize, &PrintBuf[Index], &ActualTxSize) != UART_ERR_NONE) {
                break;
            } else {
                UartTxSize -= ActualTxSize;
                Index += ActualTxSize;
            }
        }
    }
}

static void Exception_PrintHEX(ULONG Value)
{
    static char HexStr[32] GNU_SECTION_NOZEROINIT;
    UINT32 StrLen;
    UINT32 i;

    StrLen = AmbaUtility_UInt64ToStr(HexStr, 32U, Value, 16U);
    for (i = 16 - StrLen; i > 0; i--) {
        Exception_Print("0");
    }
    Exception_Print(HexStr);
}

static void Exception_PrintINT(UINT32 Value, UINT32 Radix)
{
    static char IntStr[16] GNU_SECTION_NOZEROINIT;

    (void)AmbaUtility_UInt32ToStr(IntStr, 16U, Value, Radix);
    Exception_Print(IntStr);
}

/**
 *  Dumping a backtrace
 *  @param[in] ExceptionID Exception ID
 *  @param[in] pSavedRegs Saved cpu registers
 */
void AmbaIOUT_Backtrace(UINT32 ExceptionID, ULONG * pSavedRegs)
{
    char StrBuf[16];
    const UINT32 *pInst;
    UINT32 CpuID;
    ULONG SpAddr;

    /* NOTE: Here is just an example. The AmbaPrint_xXx function won't output any messages when exception occurs. */
    Exception_Print("\r\n");

    if (ExceptionID == AARCH64_EXCP_SYNC_ERR_EL0) {
        Exception_Print("--- AARCH64 Synchronous Exception with SP_EL0 ---\n\r");
    } else if (ExceptionID == AARCH64_EXCP_SYSTEM_ERROR_EL0) {
        Exception_Print("--- AARCH64 System Error Exception with SP_EL0 ---\n\r");
    } else if (ExceptionID == AARCH64_EXCP_SYNC_ERR_ELX) {
        Exception_Print("--- AARCH64 Synchronous Exception with SP_ELx ---\n\r");
    }  else if (ExceptionID == AARCH64_EXCP_SYSTEM_ERROR_ELX) {
        Exception_Print("--- AARCH64 System Error Exception with SP_ELx ---\n\r");
    } else {
        /* Do nothing. The caller might not be an exception vector. */
    }

    AmbaMisra_TypeCast(&pInst, &pSavedRegs[AARCH64_REG_PC]);
    AmbaMisra_TypeCast(&SpAddr, &pSavedRegs[AARCH64_REG_SP]);

    CpuID = _tx_thread_smp_core_get();
    Exception_Print("Oops: CPU_");
    Exception_PrintINT(CpuID, 10U);
    Exception_Print(" Exception! Instruction : 0x");
    Exception_PrintHEX((ULONG)*pInst);
    Exception_Print(", PSTATE : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_CPSR]);
    Exception_Print("\r\n");

    Exception_Print("Flags: ");
    StrBuf[0] = ((pSavedRegs[AARCH64_REG_CPSR] & 0x80000000U) != 0U) ? 'N' : 'n';
    StrBuf[1] = ((pSavedRegs[AARCH64_REG_CPSR] & 0x40000000U) != 0U) ? 'Z' : 'z';
    StrBuf[2] = ((pSavedRegs[AARCH64_REG_CPSR] & 0x20000000U) != 0U) ? 'C' : 'c';
    StrBuf[3] = ((pSavedRegs[AARCH64_REG_CPSR] & 0x10000000U) != 0U) ? 'V' : 'v';
    StrBuf[4] = '\0';
    Exception_Print(StrBuf);
    Exception_Print("\r\n");

    Exception_Print("PC : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_PC]);
    Exception_Print(", LR : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_LR]);
    Exception_Print(", SP : 0x");
    Exception_PrintHEX(SpAddr);
    Exception_Print("\r\n");

    Exception_Print("X29 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X29]);
    Exception_Print(", X28 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X28]);
    Exception_Print("\r\n");
    Exception_Print("X27 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X27]);
    Exception_Print(", X26 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X26]);
    Exception_Print("\r\n");

    Exception_Print("X25 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X25]);
    Exception_Print(", X24 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X24]);
    Exception_Print("\r\n");
    Exception_Print("X23 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X23]);
    Exception_Print(", X22 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X22]);
    Exception_Print("\r\n");

    Exception_Print("X21 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X21]);
    Exception_Print(", X20 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X20]);
    Exception_Print("\r\n");
    Exception_Print("X19 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X19]);
    Exception_Print(", X18 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X18]);
    Exception_Print("\r\n");

    Exception_Print("X17 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X17]);
    Exception_Print(", X16 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X16]);
    Exception_Print("\r\n");
    Exception_Print("X15 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X15]);
    Exception_Print(", X14 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X14]);
    Exception_Print("\r\n");

    Exception_Print("X13 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X13]);
    Exception_Print(", X12 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X12]);
    Exception_Print("\r\n");
    Exception_Print("X11 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X11]);
    Exception_Print(", X10 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X10]);
    Exception_Print("\r\n");

    Exception_Print("X09 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X09]);
    Exception_Print(", X08 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X08]);
    Exception_Print("\r\n");
    Exception_Print("X07 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X07]);
    Exception_Print(", X06 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X06]);
    Exception_Print("\r\n");

    Exception_Print("X05 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X05]);
    Exception_Print(", X04 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X04]);
    Exception_Print("\r\n");
    Exception_Print("X03 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X03]);
    Exception_Print(", X02 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X02]);
    Exception_Print("\r\n");

    Exception_Print("X01 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X01]);
    Exception_Print(", X00 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH64_REG_X00]);
    Exception_Print("\r\n");
}

void AmbaIOUT_StackOverflow(AMBA_KAL_TASK_t *pTask)
{
    UINT32  StackStart, StackEnd, StackPtr;

    if (NULL != pTask) {
        AmbaMisra_TypeCast32(&StackStart, &(pTask->tx_thread_stack_start));
        AmbaMisra_TypeCast32(&StackEnd, &(pTask->tx_thread_stack_end));
        AmbaMisra_TypeCast32(&StackPtr, &(pTask->tx_thread_stack_ptr));

        Exception_Print("+++++++++++++++++++++++++++++++\r\n");
        Exception_Print("Task  : ");
        Exception_Print(pTask->tx_thread_name);
        Exception_Print("\r\n");

        Exception_Print("Stack : [0x");
        Exception_PrintHEX(StackStart);
        Exception_Print("-0x");
        Exception_PrintHEX(StackEnd);
        Exception_Print("]\r\n");
        Exception_Print("sp    : [0x");
        Exception_PrintHEX(StackPtr);
        Exception_Print("]\r\n");
        Exception_Print("+++++++++++++++++++++++++++++++\r\n");
    }

    AmbaAssert();

    /* fix misra c */
    AmbaMisra_TouchUnused(pTask);
}
