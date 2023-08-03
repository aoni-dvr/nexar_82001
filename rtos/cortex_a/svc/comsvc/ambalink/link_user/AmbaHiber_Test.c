/**
 * @file AmbaHiber_Test.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details Test/Reference code for AmbaHiber.
 */

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaPrint.h"
#include "AmbaLink.h"
//#include "AmbaPLL.h"
#include "AmbaShell_Commands.h"
#include "libwrapc.h"

#define UNUSED(x) x = x
extern AMBA_LINK_CTRL_s AmbaLinkCtrl;

/*============================================================================*\
 *                                                                            *
 *                           Test module Management                           *
 *                                                                            *
\*============================================================================*/
static void HiberTest_Suspend(int Mode, AMBA_SHELL_PRINT_f PrintFunc)
{
    UNUSED(PrintFunc);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_LinkCtrlSuspendLinux(%d)\n", Mode, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaIPC_LinkCtrlSuspendLinux(Mode);

//    if (AmbaIPC_LinkCtrlWaitSuspendLinux(15000) == OK) {
//        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_LinkCtrlWaitSuspendLinux(%d) done.\n", Mode, 0U, 0U, 0U, 0U);
//        AmbaPrint_Flush();
//    }
}

static void HiberTest_Resume(int Mode, AMBA_SHELL_PRINT_f PrintFunc)
{
    UNUSED(PrintFunc);

    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_LinkCtrlResumeLinux(%d)\n", Mode, 0U, 0U, 0U, 0U);
    AmbaPrint_Flush();
    AmbaIPC_LinkCtrlResumeLinux(Mode);

    if (AmbaIPC_LinkCtrlWaitResumeLinux(8000) == OK) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_LinkCtrlWaitResumeLinux(%d) done.\n", Mode, 0U, 0U, 0U, 0U);
        AmbaPrint_Flush();
    }
}

static inline void HiberTestUsage(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UNUSED(argc);
    UNUSED(argv);
    PrintFunc("Usage: t ");
    PrintFunc(argv[0]);
    PrintFunc(" <command>, where command is:\n");
    PrintFunc("\tsuspend <mode>,         0: suspend to NAND,  1: suspend to RAM\n");
    PrintFunc("\tresume  <mode>,         0: resume from NAND, 1: resume from RAM\n");
    PrintFunc("\tstress  <mode> <count>, 0: hibernation,      1: suspend / resume\n");
    PrintFunc("\twipeout\n");
    PrintFunc("\tboottype\n");
}

int HiberTestEntry(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    extern int AmbaIPC_HiberWipeout(UINT32 Flag);

    int Rval = 0;
    UINT32 Count, i;

    if (argc == 3 && strcmp(argv[1], "suspend") == 0) {
        AmbaUtility_StringToUInt32(argv[2], &Rval);
        if (Rval != 0 && Rval != 1 && Rval != 3) {
            HiberTestUsage(argc, argv, PrintFunc);
            Rval = -1;
            goto Exit;
        }

        if (Rval == 3)
            Rval = 1;

        HiberTest_Suspend(Rval, PrintFunc);

    } else if (argc == 3 && strcmp(argv[1], "resume") == 0) {
        AmbaUtility_StringToUInt32(argv[2], &Rval);
        if (Rval != 0 && Rval != 1 && Rval != 3) {
            HiberTestUsage(argc, argv, PrintFunc);
            Rval = -1;
            goto Exit;
        }

        if (Rval == 3)
            Rval = 1;

        HiberTest_Resume(Rval, PrintFunc);

    } else if (argc == 4 && strcmp(argv[1], "stress") == 0) {
        AmbaUtility_StringToUInt32(argv[2], &Rval);
        AmbaUtility_StringToUInt32(argv[3], &Count);

        if (Rval != 0 && Rval != 1 && Rval != 3) {
            HiberTestUsage(argc, argv, PrintFunc);
            Rval = -1;
            goto Exit;
        }

        if (Rval == 3)
            Rval = 1;

        for (i = 0; i < Count; i++) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "===== stress loop %d =====\n", i, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            HiberTest_Suspend(Rval, PrintFunc);
            AmbaKAL_TaskSleep(10);
            HiberTest_Resume(Rval, PrintFunc);
            AmbaKAL_TaskSleep(10);
        }

    } else if (argc == 2 && strcmp(argv[1], "wipeout") == 0) {
        Rval = AmbaIPC_HiberWipeout(0);
    } else {
        HiberTestUsage(argc, argv, PrintFunc);
        return -1;
    }

Exit:
    return Rval;
}

