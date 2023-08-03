/**
*  @file SvcLog.c
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
*  @details svc log functions
*
*/

#include "AmbaKAL.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"

#include "SvcWrap.h"
#include "SvcLog.h"

#define LOG_BUF_SIZE        512U
#define LOG_ARGC            2U

#define DBG_PRN_NATIVE      (0U)

/**
* OK log message
* @param [in] pModule module description
* @param [in] pFormat string format
* @param [in] Arg1 argument1
* @param [in] Arg2 argument2
* @return none
*/
void SvcLog_OK(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if DBG_PRN_NATIVE
    char        LogBuf[LOG_BUF_SIZE];

    sprintf(LogBuf, "[%s|OK]: %s\n", pModule, pFormat);
    printf(LogBuf, Arg1, Arg2);
#else
    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = SvcWrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|OK]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    }
#endif
}

/**
* NG log message
* @param [in] pModule module description
* @param [in] pFormat string format
* @param [in] Arg1 argument1
* @param [in] Arg2 argument2
* @return none
*/
void SvcLog_NG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if DBG_PRN_NATIVE
    char        LogBuf[LOG_BUF_SIZE];

    sprintf(LogBuf, "\033""[0;31m[%s|NG]: %s\n""\033[0m" , pModule, pFormat);
    printf(LogBuf, Arg1, Arg2);
#else
    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = SvcWrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "\033""[0;31m""[%s|NG]: %s""\033""[0m" , Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
#ifndef CONFIG_QNX
        AmbaPrint_Flush();
#endif
    }
#endif
}

/**
* debug log message
* @param [in] pModule module description
* @param [in] pFormat string format
* @param [in] Arg1 argument1
* @param [in] Arg2 argument2
* @return none
*/
void SvcLog_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
#if DBG_PRN_NATIVE
    char        LogBuf[LOG_BUF_SIZE];

    sprintf(LogBuf, "[%s|DBG]: %s\n", pModule, pFormat);
    printf(LogBuf, Arg1, Arg2);
#else
    char        LogBuf[LOG_BUF_SIZE];
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = SvcWrap_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|DBG]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    }
#endif
}
