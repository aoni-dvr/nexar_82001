/**
 *  @file AmbaIOUTDiag_CmdFaultInject.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details Fault Injection diagnostic Command
 */

#include <AmbaDef.h>
#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include "AmbaShell.h"
#include "AmbaGPIO.h"
#include <AmbaWrap.h>
#include <AmbaUtility.h>
#include <AmbaIOUTDiag.h>

static void IoDiag_FICmdUsage(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (VOID) ArgCount;
    (VOID) pArgVector;

    PrintFunc(" fainj [item] \n");
    PrintFunc("     [item]: \n");
    PrintFunc("         assertion: fault injection to Availability A53 assertion check\n");
}

void AmbaIOUTDiag_CmdFaultInject(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (ArgCount < 2U) {
        IoDiag_FICmdUsage(ArgCount, pArgVector, PrintFunc);
    } else {
        if (ArgCount >= 2U) {
            if (AmbaUtility_StringCompare(pArgVector[1], "assertion", 9U) == 0) {
                AmbaAssert();
            } else {
                IoDiag_FICmdUsage(ArgCount, pArgVector, PrintFunc);
            }
        } else {
            IoDiag_FICmdUsage(ArgCount, pArgVector, PrintFunc);
        }
    }
}
