/**
 *  @file AmbaIOUTDiag_CmdIR.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details IR diagnostic command
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"

#include "AmbaKAL.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"

#include "AmbaIOUTDiag.h"
#include "AmbaIRIF.h"

/* Task control */
static AMBA_KAL_TASK_t DiagIrMonitorTask;

static void IoDiag_IrCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" unittest    : Start IR monitor task\n");
    PrintFunc("       ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" stop        : Stop IR monitor task\n");
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_IrMonitorTaskEntry
 *
 *  @Description:: IR monitor task
 *
 *  @Input      ::
 *      EntryArg: Task entry Argument
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
static void *IoDiag_IrMonitorTaskEntry(void *Arg)
{
    static UINT16 IrDataBuf[64];
    UINT32 ActualNumIrData = 0U;
    UINT32 i = 0U;
    UINT32 Loop = 1U;

    AmbaMisra_TouchUnused(Arg);

    while(1U == Loop) {
        (void)AmbaIRIF_Read(64U, IrDataBuf, &ActualNumIrData, 500U);
        if (ActualNumIrData != 0U) {
            AmbaPrint_PrintUInt5("[Diag][IR] Size = %u", ActualNumIrData, 0U, 0U, 0U, 0U);
            for (i = 0U; i < ActualNumIrData; i ++) {
                AmbaPrint_PrintUInt5("[Diag][IR] Data[%u] = 0x%04X", i, IrDataBuf[i], 0U, 0U, 0U);
            }
        }
    }

    return NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_IrTaskStart
 *
 *  @Description:: Start IR monitor task
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_IrTaskStart(AMBA_SHELL_PRINT_f PrintFunc)
{
    static UINT8 DiagIrMonitorTaskStack[1024] GNU_SECTION_NOZEROINIT;
    static char DiagIrTaskName[20] = "DiagIrRemoteMonitor";
    UINT32 RetVal;

    (void)AmbaIRIF_Start(13000U); /* Default sample frequency: 13000Hz */

    RetVal = AmbaKAL_TaskCreate(&DiagIrMonitorTask,
                                DiagIrTaskName,
                                AMBA_KAL_TASK_LOWEST_PRIORITY,
                                IoDiag_IrMonitorTaskEntry,
                                0U,
                                DiagIrMonitorTaskStack,
                                sizeof(DiagIrMonitorTaskStack),
                                AMBA_KAL_AUTO_START);

    if (RetVal == OK) {
        PrintFunc("[Diag][IR] IR Monitor Task Start\n");
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: IoDiag_IrTaskStop
 *
 *  @Description:: Stop IR monitor task
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     ::
 *          error code
\*-----------------------------------------------------------------------------------------------*/
static UINT32 IoDiag_IrTaskStop(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;

    (void)AmbaKAL_TaskTerminate(&DiagIrMonitorTask);
    RetVal = AmbaKAL_TaskDelete(&DiagIrMonitorTask);

    if (RetVal == OK) {
        PrintFunc("[Diag][IR] IR Monitor Task Stop\n");
        (void)AmbaWrap_memset(&DiagIrMonitorTask, 0, sizeof(AMBA_KAL_TASK_t));
        (void)AmbaIRIF_Stop();
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaIOUTDiag_CmdIR
 *
 *  @Description:: IR diagnostic command
 *
 *  @Input      ::
 *      ArgCount:   Argument count
 *      pArgVector: Argument vector
 *      PrintFunc:  function to print messages on shell task
 *
 *  @Output     :: none
 *
 *  @Return     :: none
\*-----------------------------------------------------------------------------------------------*/
void AmbaIOUTDiag_CmdIR(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        IoDiag_IrCmdUsage(pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "unittest", 8U) == 0) {
            (void)IoDiag_IrTaskStart(PrintFunc);
        }
        if (AmbaUtility_StringCompare(pArgVector[1], "stop", 4U) == 0) {
            (void)IoDiag_IrTaskStop(PrintFunc);
        }
    }
}
