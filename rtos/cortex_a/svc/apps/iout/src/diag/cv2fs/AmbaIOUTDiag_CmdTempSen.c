/**
 *  @file AmbaDiag_TempSensor.c
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
 *  @details Diagnostic functions for Dram Controller.
 *
 */

#include "AmbaTypes.h"

#include "AmbaKAL.h"
#include "AmbaIOUtility.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaSYS_Ctrl.h"
#include "AmbaTempSensor.h"

static void IoDiag_PrintFormattedInt(const char *pFmtString, UINT32 Value, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[64];
    UINT32 ArgUINT32[2];

    ArgUINT32[0] = Value;
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, ArgUINT32);
    PrintFunc(StrBuf);
}

static void IoDiag_TempSensorCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void) pArgVector;
    PrintFunc("Usage: \n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" [channel ID]\n");
    PrintFunc(" e.g. diag ts 1\n");
    PrintFunc(pArgVector[0]);
    PrintFunc(" unittest\n");
}

void AmbaDiag_TempSensor(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i;
    UINT32 ChannelID = 0;
    DOUBLE TempValue;

    if (AmbaUtility_StringCompare(pArgVector[1], "unittest", 8U) == 0) {
        PrintFunc("[Diag][TempSensor]:\n");
        for(i = 0U; i < AMBA_TEMPSEN_NUM; i++) {
            AmbaTempSensor_GetTemp(1U, i, &TempValue);
            IoDiag_PrintFormattedInt("[Current Channel(%d) ", i, PrintFunc);
            if (TempValue < 0U) {
                PrintFunc("Temp(C)] = -");
                TempValue = TempValue * (-1);
                IoDiag_PrintFormattedInt("%d \n", (UINT32)TempValue, PrintFunc);
            } else {
                IoDiag_PrintFormattedInt("Temp(C)] = %d \n", (UINT32)TempValue, PrintFunc);
            }
        }
    } else if (ArgCount > 2) {
        (void)AmbaUtility_StringToUInt32(pArgVector[2], &ChannelID);
        AmbaTempSensor_GetTemp(1U, ChannelID, &TempValue);
        IoDiag_PrintFormattedInt("[Current Channel(%d) ", ChannelID, PrintFunc);
        if (TempValue < 0U) {
            PrintFunc("Temp(C)] = -");
            TempValue = TempValue * (-1);
            IoDiag_PrintFormattedInt("%d \n", (UINT32)TempValue, PrintFunc);
        } else {
            IoDiag_PrintFormattedInt("Temp(C)] = %d \n", (UINT32)TempValue, PrintFunc);
        }
    } else {
        IoDiag_TempSensorCmdUsage(pArgVector, PrintFunc);
    }
}

