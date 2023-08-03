/**
 *  @file AmbaShell_Date.c
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
 *  @details Shell command for date
 *
 */

#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaShell_Commands.h"

#include "AmbaRTC.h"

static void SHELL_PrintUInt32(UINT32 Value, const char *pFmtString, AMBA_SHELL_PRINT_f PrintFunc)
{
    char StrBuf[UTIL_MAX_INT_STR_LEN];
    (void)AmbaUtility_StringPrintUInt32(StrBuf, sizeof(StrBuf), pFmtString, 1U, &Value);
    PrintFunc(StrBuf);
}

static UINT32 SHELL_ShowAsciiTime(const AMBA_RTC_DATE_TIME_s *pCalendar, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal = OK;

    if (pCalendar == NULL) {
        RetVal = ERR_ARG;
    } else {
        /* YYYY/MM/DD HH:MM:SS */
        SHELL_PrintUInt32(pCalendar->Year, "%4u", PrintFunc);
        PrintFunc("/");
        SHELL_PrintUInt32(pCalendar->Month, "%2u", PrintFunc);
        PrintFunc("/");
        SHELL_PrintUInt32(pCalendar->Day, "%2u", PrintFunc);
        PrintFunc(" ");
        SHELL_PrintUInt32(pCalendar->Hour, "%2u", PrintFunc);
        PrintFunc(":");
        SHELL_PrintUInt32(pCalendar->Minute, "%2u", PrintFunc);
        PrintFunc(":");
        SHELL_PrintUInt32(pCalendar->Second, "%2u", PrintFunc);
        PrintFunc("\n");
    }

    return RetVal;
}

static void SHELL_ParseAsciiTime(AMBA_RTC_DATE_TIME_s *pCalendar, char * const *pArgVector)
{
    char BufferTmp[UTIL_MAX_STR_TOKEN_NUMBER][UTIL_MAX_STR_TOKEN_LENGTH];
    UINT32 Count = 0;

    (void)AmbaUtility_StringToken(pArgVector[2], '/', BufferTmp, &Count);
    (void)AmbaUtility_StringToUInt32(BufferTmp[0], &pCalendar->Year);
    (void)AmbaUtility_StringToUInt32(BufferTmp[1], &pCalendar->Month);
    (void)AmbaUtility_StringToUInt32(BufferTmp[2], &pCalendar->Day);

    (void)AmbaUtility_StringToken(pArgVector[3], ':', BufferTmp, &Count);
    (void)AmbaUtility_StringToUInt32(BufferTmp[0], &pCalendar->Hour);
    (void)AmbaUtility_StringToUInt32(BufferTmp[1], &pCalendar->Minute);
    (void)AmbaUtility_StringToUInt32(BufferTmp[2], &pCalendar->Second);
}

static inline void SHELL_DateCmdUsage(char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("Usage: ");
    PrintFunc(pArgVector[0]);
    PrintFunc(" -s [YYYY/MM/DD HH:MM:SS]\n");
}

/**
 *  AmbaShell_CommandCRC32Sum - execute command date
 *  @param[in] ArgCount argument count
 *  @param[in] pArgVector argument
 *  @param[in] PrintFunc print function point
 */
void AmbaShell_CommandDate(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_RTC_DATE_TIME_s Calendar;

    (void)AmbaRTC_Config(NULL, NULL, NULL, NULL);

    if (ArgCount == 1U) {
        (void)AmbaRTC_GetSysTime(&Calendar);
        (void)SHELL_ShowAsciiTime(&Calendar, PrintFunc);
    } else if (ArgCount >= 4U) {
        if (AmbaUtility_StringCompare(pArgVector[1], "-s", 2U) == 0) {
            SHELL_ParseAsciiTime(&Calendar, pArgVector);
            (void)AmbaRTC_SetSysTime(&Calendar);
        }
    } else {
        SHELL_DateCmdUsage(pArgVector, PrintFunc);
    }
}

