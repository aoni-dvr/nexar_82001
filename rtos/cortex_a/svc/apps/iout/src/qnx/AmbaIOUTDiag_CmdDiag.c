/**
 *  @file AmbaIOUTDiag_CmdDiag.c
 *
 *  @copyright Copyright (c) 2022 Ambarella, Inc.
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
 *  @details Entry of IO UT diagnostic commands
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"

#include "AmbaIOUTDiag.h"

static AMBA_SHELL_COMMAND_s IoDiagCmdList[] = {
    { .pName = "i2c",   .MainFunc = AmbaIOUTDiag_CmdI2C,       .pNext = NULL },
    { .pName = "gpio",  .MainFunc = AmbaIOUTDiag_CmdGPIO,      .pNext = NULL },
    { .pName = "vin",   .MainFunc = AmbaIOUTDiag_CmdVIN,       .pNext = NULL },
};

static void DiagCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;

    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [");
    PrintFunc(IoDiagCmdList[0].pName);
    for (i = 1U; i < (sizeof(IoDiagCmdList)/sizeof(IoDiagCmdList[0U])); i ++) {
        PrintFunc("|");
        PrintFunc(IoDiagCmdList[i].pName);
    }
    PrintFunc("]\n");
}

/**
 *  Run IOUT diagnostic tools
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 *  @return none
 */
void AmbaIOUTDiag_CmdDiag(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i = 0U;

    if (ArgCount < 2U) {
        DiagCmdUsage(pArgVector, PrintFunc);
    } else {
        for (i = 0U; i < (sizeof(IoDiagCmdList)/sizeof(IoDiagCmdList[0U])); i++) {
            if (AmbaUtility_StringCompare(pArgVector[1], IoDiagCmdList[i].pName, AmbaUtility_StringLength(IoDiagCmdList[i].pName)) == 0) { //FixMe Size of string
                IoDiagCmdList[i].MainFunc(ArgCount - 1U, &pArgVector[1], PrintFunc);
            }
        }
    }
}
