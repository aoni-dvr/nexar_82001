/**
 *  @file SvcCmdDiag.c
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
 *  @details svc diag command functions
 *
 */

#include "AmbaShell.h"
#include "AmbaADC.h"
#include "AmbaIOUTDiag.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcCmd.h"
#include "SvcCmdDiag.h"
#if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && (defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52))
#include "AmbaDiag_CmdDDR.h"
#endif

#define SVC_LOG_CMDDIAG     "CMDDIAG"

static void CmdDiagUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc(" diag commands: \n");
    PrintFunc("   gpio       : gpio test command\n");
    PrintFunc("   i2c        : i2c test command\n");
    //PrintFunc("   dram       : dram test command\n");
    #if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
    PrintFunc("   ddr        : ddr test command\n");
    #endif
    #if defined(CONFIG_ICAM_CVBS_EXIST)
    PrintFunc("   cvbs       : cvbs test command\n");
    #endif
    #if defined(AMBA_ADC_CHANNEL_T2V)
    PrintFunc("   temperature: show chip temperature\n");
    #endif
}

static void CmdDiagEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32  Rval = SVC_NG;

    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("gpio", pArgVector[1U])) {
            AmbaIOUTDiag_CmdGPIO(ArgCount - 1U, &pArgVector[1], PrintFunc);
            Rval = SVC_OK;
        } else if (0 == SvcWrap_strcmp("i2c", pArgVector[1U])) {
            AmbaIOUTDiag_CmdI2C(ArgCount - 1U, &pArgVector[1], PrintFunc);
            Rval = SVC_OK;
        //} else if (0 == SvcWrap_strcmp("dram", pArgVector[1U])) {
        //    AmbaIOUTDiag_CmdDRAM(ArgCount - 1U, &pArgVector[1], PrintFunc);
        //    Rval = SVC_OK;
        #if !defined(CONFIG_SOC_CV2FS) && !defined(CONFIG_SOC_CV22FS)
        } else if (0 == SvcWrap_strcmp("ddr", pArgVector[1U])) {
        #if defined(CONFIG_BUILD_COMSVC_DRAMSHMOO) && (defined(CONFIG_SOC_CV5) || defined (CONFIG_SOC_CV52))
            AmbaDiag_CmdDDR(ArgCount - 1U, &pArgVector[1], PrintFunc);
        #else
            AmbaIOUTDiag_CmdDDR(ArgCount - 1U, &pArgVector[1], PrintFunc);
        #endif
            Rval = SVC_OK;
        #endif
        #if defined(CONFIG_ICAM_CVBS_EXIST)
        } else if (0 == SvcWrap_strcmp("cvbs", pArgVector[1U])) {
            AmbaIOUTDiag_CmdCVBS(ArgCount - 1U, &pArgVector[1], PrintFunc);
            Rval = SVC_OK;
        #endif
        #if defined(AMBA_ADC_CHANNEL_T2V)
        } else if (0 == SvcWrap_strcmp("temperature", pArgVector[1U])) {
            char    StrBuf[256] = {'\0'};
            UINT32  AdcVal, BufLen = 256U, Len;
            DOUBLE  Tc;

            Rval = AmbaADC_SingleRead(AMBA_ADC_CHANNEL_T2V, &AdcVal);
            if (Rval == SVC_OK) {
                /* Tc = (Adc_Value – 1517.6) / 14.799 */
                Tc = ((DOUBLE)AdcVal - 1517.6) / 14.799;

                SVC_WRAP_SNPRINT            "\nTemperature is %.3f degrees Celsius\n\n"
                    SVC_SNPRN_ARG_S         StrBuf
                    SVC_SNPRN_ARG_DOUBLE    Tc
                    SVC_SNPRN_ARG_BSIZE     BufLen
                    SVC_SNPRN_ARG_RLEN      &Len
                    SVC_SNPRN_ARG_E
                PrintFunc(StrBuf);
            }
        #endif
        } else {
            /* do nothing */
        }
    }

    if (Rval != SVC_OK) {
        CmdDiagUsage(PrintFunc);
    }
}

/**
* install of diag shell commands
* @return none
*/
void SvcCmdDiag_Install(void)
{
    AMBA_SHELL_COMMAND_s  SvcCmdDiag;

    SvcCmdDiag.pName    = "diag";
    SvcCmdDiag.MainFunc = CmdDiagEntry;
    SvcCmdDiag.pNext    = NULL;

    if (SvcCmd_CommandRegister(&SvcCmdDiag) != SHELL_ERR_SUCCESS) {
        SvcLog_NG(SVC_LOG_CMDDIAG, "## fail to install diag command", 0U, 0U);
    }
}
