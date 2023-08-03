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

    AmbaMisra_TypeCast32(&pString, &pBuf);
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

static void Exception_PrintHEX(UINT32 Value)
{
    static char HexStr[16] GNU_SECTION_NOZEROINIT;
    UINT32 StrLen;

    StrLen = AmbaUtility_UInt32ToStr(HexStr, 16U, Value, 16U);
    if (StrLen == 1U) {
        Exception_Print("0000000");
    } else if (StrLen == 2U) {
        Exception_Print("000000");
    } else if (StrLen == 3U) {
        Exception_Print("00000");
    } else if (StrLen == 4U) {
        Exception_Print("0000");
    } else if (StrLen == 5U) {
        Exception_Print("000");
    } else if (StrLen == 6U) {
        Exception_Print("00");
    } else if (StrLen == 7U) {
        Exception_Print("0");
    } else {
        /* Do nothing */
    }
    Exception_Print(HexStr);
}

static void Exception_PrintINT(UINT32 Value, UINT32 Radix)
{
    static char IntStr[16] GNU_SECTION_NOZEROINIT;

    (void)AmbaUtility_UInt32ToStr(IntStr, 16U, Value, Radix);
    Exception_Print(IntStr);
}

static void AmbaStack_FindRange(UINT32 Mode, UINT32 *pHighest, UINT32 *pLowest)
{
    const TX_THREAD *current_thread;
    UINT32 CpuID;

    if ((pHighest != NULL) && (pLowest != NULL)) {
        Exception_Print("+++++++++++++++++++++++++++++++\r\n");

        CpuID = _tx_thread_smp_core_get();

        if (Mode == 1U) {
            AmbaMisra_TypeCast32(pHighest, &AmbaSysStackBaseFIQ[CpuID]);
            AmbaMisra_TypeCast32(pLowest, &AmbaSysStackLimitFIQ[CpuID]);
            *pHighest -= 4U;
        } else if (Mode == 2U) {
            AmbaMisra_TypeCast32(pHighest, &AmbaSysStackBaseIRQ[CpuID]);
            AmbaMisra_TypeCast32(pLowest, &AmbaSysStackLimitIRQ[CpuID]);
            *pHighest -= 4U;
        } else if (Mode == 11U) {
            AmbaMisra_TypeCast32(pHighest, &AmbaSysStackBaseUND[CpuID]);
            AmbaMisra_TypeCast32(pLowest, &AmbaSysStackLimitUND[CpuID]);
            *pHighest -= 4U;
        } else if (Mode == 7U) {
            AmbaMisra_TypeCast32(pHighest, &AmbaSysStackBaseABT[CpuID]);
            AmbaMisra_TypeCast32(pLowest, &AmbaSysStackLimitABT[CpuID]);
            *pHighest -= 4U;
        } else if (Mode == 15U) {
            AmbaMisra_TypeCast32(pHighest, &AmbaSysStackBaseSYS[CpuID]);
            AmbaMisra_TypeCast32(pLowest, &AmbaSysStackLimitSYS[CpuID]);
            *pHighest -= 4U;
        } else if (Mode == 3U) {
            AmbaMisra_TypeCast32(pHighest, &AmbaSysStackBaseSVC[CpuID]);
            AmbaMisra_TypeCast32(pLowest, &AmbaSysStackLimitSVC[CpuID]);
            *pHighest -= 4U;

            /* Pickup thread pointer. */
            current_thread = (TX_THREAD *) _tx_thread_smp_current_thread_get();
            if (current_thread != NULL) {
                AmbaMisra_TypeCast32(pHighest, &current_thread->tx_thread_stack_end);
                AmbaMisra_TypeCast32(pLowest, &current_thread->tx_thread_stack_start);
                *pHighest -= 3U;

                Exception_Print("Task  : ");
                Exception_Print(current_thread->tx_thread_name);
                Exception_Print("\r\n");
            } else {
                /* The ThreadX system code doesn't use stack.
                 * You might see SP is still a pointer to a thread's stack.
                 * (It is usually the last thread's stack before scheduler enters idle)
                 */
            }
        } else {
            *pHighest = 0U;
            *pLowest = 0U;
        }

        if (*pHighest != 0U) {
            Exception_Print("Stack : [0x");
            Exception_PrintHEX((UINT32)*pLowest);
            Exception_Print("-0x");
            Exception_PrintHEX((UINT32)*pHighest + 3U);
            Exception_Print("]\r\n");
            Exception_Print("+++++++++++++++++++++++++++++++\r\n");
        }
    }
}

static void AmbaStack_UnwindAPCS(UINT32 RegPC, UINT32 RegLR, UINT32 RegSP, UINT32 RegFP, UINT32 StackHighest, UINT32 StackLowest)
{
    UINT32 i, TraceLimit, fp, pc, lr, sp;
    UINT32 Offset = 0;
    const UINT32 *pStack;
    const char *pStr;

    Exception_Print("--- Backtrace (ARM/APCS) ---\r\n");

    /* Show the execuption root cause */
    pStr = AmbaDbgSym_FindFunc(RegPC, &Offset);
    Exception_Print("[0]  pc : 0x");
    Exception_PrintHEX(RegPC);
    if (pStr != NULL) {
        Exception_Print(" (");
        Exception_Print(pStr);
        Exception_Print("+0x");
        Exception_PrintINT(Offset, 16U);
        Exception_Print(")\r\n");
    } else {
        Exception_Print("\r\n");
    }

    pStr = AmbaDbgSym_FindFunc(RegLR, &Offset);
    Exception_Print("     lr : 0x");
    Exception_PrintHEX(RegLR);
    if (pStr != NULL) {
        Exception_Print(" (");
        Exception_Print(pStr);
        Exception_Print("+0x");
        Exception_PrintINT(Offset, 16U);
        Exception_Print(")\r\n");
    } else {
        Exception_Print("\r\n");
    }

    TraceLimit = CONFIG_EXCEPTION_BACKTRACE_LEVEL;
    if (StackHighest == 0U) {
        Exception_Print("     sp : 0x");
        Exception_PrintHEX(RegSP);
        Exception_Print("\r\n");
        Exception_Print("     fp : 0x");
        Exception_PrintHEX(RegFP);
        Exception_Print("\r\n");
    } else {
        Exception_Print("     sp : 0x");
        Exception_PrintHEX(RegSP);
        if ((RegSP <= StackHighest) && (RegSP >= StackLowest)) {
            Exception_Print(" <valid>\r\n");
        } else {
            Exception_Print(" <invalid?>\r\n");
        }

        Exception_Print("     fp : 0x");
        Exception_PrintHEX(RegFP);
        if ((RegFP <= StackHighest) && (RegFP >= StackLowest)) {
            Exception_Print(" <valid>\r\n");
            TraceLimit = 0xffffffffU;
        } else {
            Exception_Print(" <invalid?>\r\n");
            TraceLimit = 0x0;
        }
    }

    /* Show the backtrace according to frame pointer */
    fp = RegFP;
    for (i = 1U; i <= TraceLimit; i++) {
        AmbaMisra_TypeCast32(&pStack, &fp);
        pc = pStack[0];
        lr = pStack[-1];
        sp = pStack[-2];
        fp = pStack[-3];

        pStr = AmbaDbgSym_FindFunc(pc, &Offset);
        Exception_Print("[");
        Exception_PrintINT(i, 10U);
        if (i < 10U) {
            Exception_Print("]  pc : 0x");
        } else {
            Exception_Print("] pc : 0x");
        }
        Exception_PrintHEX(pc);
        if (pStr != NULL) {
            Exception_Print(" (");
            Exception_Print(pStr);
            Exception_Print("+0x");
            Exception_PrintINT(Offset, 16U);
            Exception_Print(")\r\n");
        } else {
            Exception_Print("\r\n");
        }

        pStr = AmbaDbgSym_FindFunc(lr, &Offset);
        Exception_Print("     lr : 0x");
        Exception_PrintHEX(lr);
        if (pStr != NULL) {
            Exception_Print(" (");
            Exception_Print(pStr);
            Exception_Print("+0x");
            Exception_PrintINT(Offset, 16U);
            Exception_Print(")\r\n");
        } else {
            Exception_Print("\r\n");
        }

        if (StackHighest == 0U) {
            Exception_Print("     sp : 0x");
            Exception_PrintHEX(sp);
            Exception_Print("\r\n");
            Exception_Print("     fp : 0x");
            Exception_PrintHEX(fp);
            Exception_Print("\r\n");
        } else {
            Exception_Print("     sp : 0x");
            Exception_PrintHEX(sp);
            if ((sp <= StackHighest) && (sp >= StackLowest)) {
                Exception_Print(" <valid>\r\n");
            } else {
                Exception_Print(" <invalid?>\r\n");
            }

            Exception_Print("     fp : 0x");
            Exception_PrintHEX(fp);
            if ((fp <= StackHighest) && (fp >= StackLowest)) {
                Exception_Print(" <valid>\r\n");
            } else {
                Exception_Print(" <invalid?>\r\n");
                break;
            }
        }
    }
}

static void AmbaStack_Dump(UINT32 RegSP, UINT32 StackHighest, UINT32 StackLowest)
{
    UINT32 i, sp, Offset = 0;
    const UINT32 *pStack;
    const char *pStr;

    Exception_Print("--- Stack Dump (Max 1KB) ---\r\n");

    if (StackHighest != 0U) {
        for (i = 0U; ; i += 4U) {
            sp = RegSP + i;

            if ((sp <= StackHighest) && (sp >= StackLowest)) {
                AmbaMisra_TypeCast32(&pStack, &sp);
                if ((*pStack & 0x3U) == 0U) {
                    pStr = AmbaDbgSym_FindFunc(*pStack, &Offset);
                } else {
                    pStr = NULL;
                }

                if (i < 1024U) { /* Only dump the first 1KB. */
                    Exception_Print(" <sp[");
                    Exception_PrintINT(i, 10U);
                    if (i < 10U) {
                        Exception_Print("]>    : 0x");
                    } else if (i < 100U) {
                        Exception_Print("]>   : 0x");
                    } else if (i < 1000U) {
                        Exception_Print("]>  : 0x");
                    } else {
                        Exception_Print("]> : 0x");
                    }
                    Exception_PrintHEX(*pStack);
                    if (pStr != NULL) {
                        Exception_Print(" (");
                        Exception_Print(pStr);
                        Exception_Print("+0x");
                        Exception_PrintINT(Offset, 16U);
                        Exception_Print(")\r\n");
                    } else {
                        Exception_Print("\r\n");
                    }
                } else {
                    /* Skip less significant data. */
                }
            } else {
                break;
            }
        }
    } else {
        Exception_Print("Unknown stack range.");
    }
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
    UINT32 CpuID, StackHighest = 0, StackLowest = 0;

    /* NOTE: Here is just an example. The AmbaPrint_xXx function won't output any messages when exception occurs. */
    Exception_Print("\r\n");

    if (ExceptionID == AARCH32_EXCP_UNDEFINED_INSTRUCTION) {
        Exception_Print("--- AARCH32 UNDEFINED INSTRUCTION EXCEPTION ---\n\r");
    } else if (ExceptionID == AARCH32_EXCP_PREFETCH_ABORT) {
        Exception_Print("--- AARCH32 PREFETCH ABORT EXCEPTION ---\n\r");
    } else if (ExceptionID == AARCH32_EXCP_DATA_ABORT) {
        Exception_Print("--- AARCH32 DATA ABORT EXCEPTION ---\n\r");
    } else {
        /* Do nothing. The caller might not be an exception vector. */
    }

    if (ExceptionID == AARCH32_EXCP_DATA_ABORT) {
        pSavedRegs[AARCH32_REG_PC] -= 0x8U;
    } else {
        pSavedRegs[AARCH32_REG_PC] -= 0x4U;
    }

    AmbaMisra_TypeCast32(&pInst, &pSavedRegs[AARCH32_REG_PC]);

    CpuID = _tx_thread_smp_core_get();
    Exception_Print("Oops: CPU_");
    Exception_PrintINT(CpuID, 10U);
    Exception_Print(" Exception! Instruction : 0x");
    Exception_PrintHEX(*pInst);
    Exception_Print(", PSTATE : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_CPSR]);
    Exception_Print("\r\n");

    if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 0U) {
        Exception_Print("Mode : USR ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 1U) {
        Exception_Print("Mode : FIQ ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 2U) {
        Exception_Print("Mode : IRQ ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 3U) {
        Exception_Print("Mode : SVC ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 6U) {
        Exception_Print("Mode : MON ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 7U) {
        Exception_Print("Mode : ABT ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 10U) {
        Exception_Print("Mode : HYP ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 11U) {
        Exception_Print("Mode : UND ");
    } else if ((pSavedRegs[AARCH32_REG_CPSR] & 0xfU) == 15U) {
        Exception_Print("Mode : SYS ");
    } else {
        Exception_Print("Mode : RSVD");
    }
    if ((pSavedRegs[AARCH32_REG_CPSR] & 0x20U) == 0U) {
        Exception_Print("(A32), ");
    } else {
        Exception_Print("(T32), ");
    }

    Exception_Print("IRQ : ");
    if ((pSavedRegs[AARCH32_REG_CPSR] & 0x80U) == 0U) {
        Exception_Print("Not masked, ");
    } else {
        Exception_Print("Masked,     ");
    }

    Exception_Print("FIQ : ");
    if ((pSavedRegs[AARCH32_REG_CPSR] & 0x40U) == 0U) {
        Exception_Print("Not masked, ");
    } else {
        Exception_Print("Masked,     ");
    }

    Exception_Print("Flags: ");
    StrBuf[0] = ((pSavedRegs[AARCH32_REG_CPSR] & 0x80000000U) != 0U) ? 'N' : 'n';
    StrBuf[1] = ((pSavedRegs[AARCH32_REG_CPSR] & 0x40000000U) != 0U) ? 'Z' : 'z';
    StrBuf[2] = ((pSavedRegs[AARCH32_REG_CPSR] & 0x20000000U) != 0U) ? 'C' : 'c';
    StrBuf[3] = ((pSavedRegs[AARCH32_REG_CPSR] & 0x10000000U) != 0U) ? 'V' : 'v';
    StrBuf[4] = '\0';
    Exception_Print(StrBuf);
    Exception_Print("\r\n");

    Exception_Print("r15 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_PC]);
    Exception_Print(", r14 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_LR]);
    Exception_Print(", r13 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_SP]);
    Exception_Print(", r12 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_IP]);
    Exception_Print("\r\n");

    Exception_Print("r11 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_FP]);
    Exception_Print(", r10 : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R10]);
    Exception_Print(", r9  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R9]);
    Exception_Print(", r8  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R8]);
    Exception_Print("\r\n");

    Exception_Print("r7  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R7]);
    Exception_Print(", r6  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R6]);
    Exception_Print(", r5  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R5]);
    Exception_Print(", r4  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R4]);
    Exception_Print("\r\n");

    Exception_Print("r3  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R3]);
    Exception_Print(", r2  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R2]);
    Exception_Print(", r1  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R1]);
    Exception_Print(", r0  : 0x");
    Exception_PrintHEX(pSavedRegs[AARCH32_REG_R0]);
    Exception_Print("\r\n");

    AmbaStack_FindRange((pSavedRegs[AARCH32_REG_CPSR] & 0xfU), &StackHighest, &StackLowest);
    AmbaStack_UnwindAPCS(pSavedRegs[AARCH32_REG_PC], pSavedRegs[AARCH32_REG_LR], pSavedRegs[AARCH32_REG_SP], pSavedRegs[AARCH32_REG_FP], StackHighest, StackLowest);
    AmbaStack_Dump(pSavedRegs[AARCH32_REG_SP], StackHighest, StackLowest);

    Exception_Print("--- Done Stack Unwinding ---\r\n");
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
