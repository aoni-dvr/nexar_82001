/**
 *  @file AmbaIOUTDiag_CmdRNG.c
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
 *  @details RNG diagnostic Command
 */
#if defined(CONFIG_LINUX) && !defined(VOID)
#define VOID void
#endif
#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include "AmbaShell.h"
#include "AmbaRNG.h"
#include <AmbaUtility.h>
#include <AmbaIOUTDiag.h>

static void IoDiag_RngCmdUsage(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void) ArgCount;
    (void) pArgVector;

    PrintFunc(" rng start : Start RNG \n");
    PrintFunc(" rng gen   : Generate random numbers \n");
    PrintFunc(" rng stop  : Stop RNG \n");
}

static void IoDiag_RngPrintUInt32(AMBA_SHELL_PRINT_f PrintFunc,
                                  const char *pFmtString,
                                  UINT32 Count,
                                  UINT32 Arg1,
                                  UINT32 Arg2,
                                  UINT32 Arg3,
                                  UINT32 Arg4,
                                  UINT32 Arg5)
{
    char StrBuf[256];
    UINT32 Arg[5];

    Arg[0] = Arg1;
    Arg[1] = Arg2;
    Arg[2] = Arg3;
    Arg[3] = Arg4;
    Arg[4] = Arg5;

    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, Count, Arg);
    PrintFunc(StrBuf);
}

static void IoDiag_RngStart(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Retstatus;

    Retstatus = AmbaRNG_Init();
    if (Retstatus != RNG_ERR_NONE) {
        IoDiag_RngPrintUInt32(PrintFunc, "Failed to start RNG = 0x%x \n", 1, Retstatus, 0, 0, 0, 0);
    }
}

static void IoDiag_RngGen(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Retstatus;
    UINT32 data[5];

#ifdef CONFIG_SOC_CV2
    Retstatus = AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], NULL);
#else
    Retstatus = AmbaRNG_DataGet(&data[0], &data[1], &data[2], &data[3], &data[4]);
#endif
    if (Retstatus != RNG_ERR_NONE) {
        IoDiag_RngPrintUInt32(PrintFunc, "Failed to generate numbers = 0x%x \n", 1, Retstatus, 0, 0, 0, 0);
    } else {
#ifdef CONFIG_SOC_CV2
        IoDiag_RngPrintUInt32(PrintFunc, "[0] 0x%x, [1] 0x%x, [2] 0x%x, [3] 0x%x \n",
                              4,
                              data[0],
                              data[1],
                              data[2],
                              data[3],
                              0);
#else
        IoDiag_RngPrintUInt32(PrintFunc, "[0] 0x%x, [1] 0x%x, [2] 0x%x, [3] 0x%x, [4] 0x%x \n",
                              5,
                              data[0],
                              data[1],
                              data[2],
                              data[3],
                              data[4]);
#endif
    }
}

static void IoDiag_RngStop(AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 Retstatus;

    Retstatus = AmbaRNG_Deinit();
    if (Retstatus != RNG_ERR_NONE) {
        IoDiag_RngPrintUInt32(PrintFunc, "Failed to stop RNG = 0x%x \n", 1, Retstatus, 0, 0, 0, 0);
    }
}

void AmbaIOUTDiag_CmdRNG(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        IoDiag_RngCmdUsage(ArgCount, pArgVector, PrintFunc);
    } else {
        if (AmbaUtility_StringCompare(pArgVector[1], "start", 5U) == 0) {
            IoDiag_RngStart(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "gen", 3U) == 0) {
            IoDiag_RngGen(PrintFunc);
        } else if (AmbaUtility_StringCompare(pArgVector[1], "stop", 4U) == 0) {
            IoDiag_RngStop(PrintFunc);
        } else {
            IoDiag_RngCmdUsage(ArgCount, pArgVector, PrintFunc);
        }
    }
}
