/**
 *  @file AmbaIOUTDiag_CmdPWM.c
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
 *  @details PWM diagnostic command
 *
 */

#include "AmbaTypes.h"

#include "AmbaShell.h"
#include "AmbaUtility.h"

#include "AmbaIOUTDiag.h"
#include "AmbaPWM.h"

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_PwmShowInfo(UINT32 PinPairID, UINT32 Period, UINT32 Duty, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 ActualFreq;

    if (Period > 0U) {
        if (AmbaPWM_GetInfo(PinPairID, &ActualFreq) == OK) {
            IoDiag_PrintFormattedInt("[Diag][PWM][%d]: ", PinPairID, PrintFunc);
            PrintFunc("Frequency = ");
            IoDiag_PrintFormattedInt("%d\n", ActualFreq / Period, PrintFunc);

            IoDiag_PrintFormattedInt("[Diag][PWM][%d]: ", PinPairID, PrintFunc);
            PrintFunc("Duty Cycle = ");
            IoDiag_PrintFormattedInt("%d%%\n", (Duty * 100U) / Period, PrintFunc);
        }
    }
}

static void IoDiag_PwmCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" <Channel> <Freq> <Period> <Duty>\n");
    PrintFunc("       Example: pwm 0 10000 100 50\n");
}

/**
 *  PWM diagnostic command
 *  @param[in] ArgCount Argument count
 *  @param[in] pArgVector Argument vector
 *  @param[in] PrintFunc function to print messages on shell task
 */
void AmbaIOUTDiag_CmdPWM(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 PinPairID, Freq, Period, Duty;
    UINT32 RetVal;

    if (ArgCount < 2U) {
        IoDiag_PwmCmdUsage(pArgVector, PrintFunc);
    } else {
        (void)AmbaUtility_StringToUInt32(pArgVector[1], &PinPairID);

        if (ArgCount > 2U) {
            (void)AmbaUtility_StringToUInt32(pArgVector[2], &Freq);
        } else {
            Freq = 1000U;
        }

        if (ArgCount > 3U) {
            (void)AmbaUtility_StringToUInt32(pArgVector[3], &Period);
        } else {
            Period = 2U;
        }

        if (ArgCount > 4U) {
            (void)AmbaUtility_StringToUInt32(pArgVector[4], &Duty);
        } else {
            Duty = 1U;
        }

        if (PinPairID >= AMBA_NUM_PWM_CHANNEL) {
            PrintFunc("[Diag][PWM] ERROR:Invalid Channel\n");
            RetVal = ERR_ARG;
        } else {
            RetVal = ERR_NONE;
        }

        if (RetVal == (UINT32)ERR_NONE) {
            if (Freq == 0U) {
                PrintFunc("[Diag][PWM] ERROR:Invalid Frequency\n");
                RetVal = ERR_ARG;
            } else {
                RetVal = ERR_NONE;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            if (Period == 0U) {
                PrintFunc("[Diag][PWM] ERROR:Invalid Period\n");
                RetVal = ERR_ARG;
            } else {
                RetVal = ERR_NONE;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            if (Duty > Period) {
                PrintFunc("[Diag][PWM] ERROR:Duty is greater than Period\n");
                RetVal = ERR_ARG;
            } else {
                RetVal = ERR_NONE;
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Config(PinPairID, Freq * Period);

            if (RetVal != (UINT32)ERR_NONE) {
                PrintFunc("[Diag][PWM] ERROR:AmbaPWM_Config failed\n");
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            RetVal = AmbaPWM_Start(PinPairID, Period, Duty);

            if (RetVal != (UINT32)ERR_NONE) {
                PrintFunc("[Diag][PWM] ERROR:AmbaPWM_Start failed\n");
            }
        }

        if (RetVal == (UINT32)ERR_NONE) {
            IoDiag_PwmShowInfo(PinPairID, Period, Duty, PrintFunc);
        }
    }
}

