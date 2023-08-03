/**
 *  @file AmbaIOUTDiag_CmdDiag.c
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
 *  @details Entry of IO UT diagnostic commands
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"

#include "AmbaIOUTDiag.h"

static AMBA_SHELL_COMMAND_s IoDiagCmdList[] = {
    { .pName = "cvbs",  .MainFunc = AmbaIOUTDiag_CmdCVBS,      .pNext = NULL },
    { .pName = "ddr",   .MainFunc = AmbaIOUTDiag_CmdDDR,       .pNext = NULL },
    { .pName = "dma",   .MainFunc = AmbaIOUTDiag_CmdDMA,       .pNext = NULL },
    { .pName = "dram",  .MainFunc = AmbaIOUTDiag_CmdDRAM,      .pNext = NULL },
    { .pName = "gdma",  .MainFunc = AmbaIOUTDiag_CmdGDMA,      .pNext = NULL },
    { .pName = "gpio",  .MainFunc = AmbaIOUTDiag_CmdGPIO,      .pNext = NULL },
    { .pName = "hdmi",  .MainFunc = AmbaIOUTDiag_CmdHDMI,      .pNext = NULL },
    { .pName = "i2c",   .MainFunc = AmbaIOUTDiag_CmdI2C,       .pNext = NULL },
    { .pName = "os",    .MainFunc = AmbaIOUTDiag_CmdOS,        .pNext = NULL },
    { .pName = "pwm",   .MainFunc = AmbaIOUTDiag_CmdPWM,       .pNext = NULL },
    { .pName = "spi",   .MainFunc = AmbaIOUTDiag_CmdSPI,       .pNext = NULL },
    { .pName = "uart",  .MainFunc = AmbaIOUTDiag_CmdUART,      .pNext = NULL },
    { .pName = "vin",   .MainFunc = AmbaIOUTDiag_CmdVIN,       .pNext = NULL },
    { .pName = "vout",  .MainFunc = AmbaIOUTDiag_CmdVOUT,      .pNext = NULL },
    { .pName = "fio",   .MainFunc = AmbaIOUTDiag_CmdFio,       .pNext = NULL },
    { .pName = "wdt",   .MainFunc = AmbaIOUTDiag_CmdWDT,       .pNext = NULL },
    //{ .pName = "ts",    .MainFunc = AmbaDiag_TempSensor,        .pNext = NULL },
    //{ .pName = "can",   .MainFunc = AmbaShell_CommandCANTest,   .pNext = NULL },
#if defined(CONFIG_BUILD_SSP_USB_LIBRARY)
    { .pName = "usb",   .MainFunc = AmbaIOUTDiag_CmdUsb,       .pNext = NULL },
#endif
#ifdef CONFIG_ENABLE_INTERNAL_DIAG_CMDS
    { .pName = "otp",   .MainFunc = AmbaIOUTDiag_CmdOTP,       .pNext = NULL },
#endif
    //{ .pName = "atf",   .MainFunc = AmbaShell_CommandATF,       .pNext = NULL },
    //{ .pName = "rtsl_otp",   .MainFunc = AmbaDiag_RTSL_OTP,       .pNext = NULL },
    //{ .pName = "memtester",   .MainFunc = AmbaShell_CommandMemTester,       .pNext = NULL },
#ifdef CONFIG_ENABLE_INTERNAL_DIAG_CMDS
    { .pName = "rng",   .MainFunc = AmbaShell_CommandRNGTest,    .pNext = NULL },
#else
    { .pName = "rng",   .MainFunc = AmbaIOUTDiag_CmdRNG,       .pNext = NULL },
#endif
    { .pName = "adc",   .MainFunc = AmbaIOUTDiag_CmdADC,       .pNext = NULL },
    { .pName = "ir",    .MainFunc = AmbaIOUTDiag_CmdIR,        .pNext = NULL },
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
