/**
 *  @file AmbaShell_Misc.c
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
 *  @details Shell cmd "general" functions
 *
 */

#include "AmbaADC.h"
#include "AmbaIRIF.h"
#include "AmbaWDT.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"

#include "AmbaShell_Utility.h"
#include "AmbaShell_Commands.h"

static void usage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [adc] : Adc normal test\n");

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [ir] : Ir normal test\n");
#endif
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [wdt] : Wdt normal test, \n");
}
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
static void Shell_AdcTest(void)
{
    UINT32 i, Data = 0U;

    for (i = 0U; i < AMBA_NUM_ADC_CHANNEL; i++) {
        (void)AmbaADC_SingleRead(i, &Data);
        AmbaPrint_PrintUInt5("[ADC] Channel %d, Val = %d ", i, Data, 0U, 0U, 0U);
    }
}

static void *IoDiag_IrMonitorTaskEntry(void *EntryArg)
{
    static UINT16 IrDataBuf[64];
    UINT32 ActualNumIrData = 0U;
    UINT32 i = 0U;
    UINT32 FlagLoop = 0;

    AmbaMisra_TouchUnused(EntryArg);
    while (FlagLoop < 0xFFFFFFFFU) {
        (void)AmbaIRIF_Read(64U, IrDataBuf, &ActualNumIrData, 500U);
        if (ActualNumIrData != 0U) {
            AmbaPrint_PrintUInt5("[IR] Size = %u", ActualNumIrData, 0U, 0U, 0U, 0U);
            for (i = 0U; i < ActualNumIrData; i ++) {
                AmbaPrint_PrintUInt5("[IR] Data[%u] = 0x%04X", i, IrDataBuf[i], 0U, 0U, 0U);
            }
        }
        FlagLoop++;
    }

    AmbaPrint_PrintUInt5("[IR] Loop terminated", 0, 0, 0U, 0U, 0U);
    return NULL;
}

static void IoDiag_IrTaskStart(AMBA_SHELL_PRINT_f PrintFunc)
{
    static AMBA_KAL_TASK_t DiagIrMonitorTask;
    static UINT8 DiagIrMonitorTaskStack[1024] GNU_SECTION_NOZEROINIT;
    static char DiagIrTaskName[20] = "DiagIrRemoteMonitor";
    UINT32 RetVal;

    (void)AmbaIRIF_Start(13000U); /* Default sample frequency: 13000Hz */
    RetVal = AmbaKAL_TaskCreate(&DiagIrMonitorTask,
                                DiagIrTaskName,
                                AMBA_KAL_TASK_LOWEST_PRIORITY,
                                IoDiag_IrMonitorTaskEntry,
                                NULL,
                                DiagIrMonitorTaskStack,
                                (UINT32)sizeof(DiagIrMonitorTaskStack),
                                AMBA_KAL_AUTO_START);
    if (RetVal == OK) {
        PrintFunc("[IR] IR Monitor Task Start\n");
    }
}
#endif
static void Shell_WdtTest(AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)AmbaWDT_Start(5000U, 10U);
    PrintFunc("WDT start to count down, 5 seconds later will reboot\n");
}

/**
 *  AmbaShell_CommandMisc - execute command misc
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandMisc(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        usage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "wdt", 3) == 0) {
            Shell_WdtTest(PrintFunc);
#if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
        } else if (AmbaUtility_StringCompare(pArgVector[1], "ir", 2) == 0) {
            IoDiag_IrTaskStart(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "adc", 3) == 0) {
            Shell_AdcTest();
#endif
        } else {
            /* for misraC checking, do nothing */
        }
    }
}
