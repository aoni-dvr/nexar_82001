/**
*  @file AmbaFrwk.h
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
*  @details svc framework log functions
*
*/

#ifndef SVC_FRWK_H
#define SVC_FRWK_H

#include "AmbaUtility.h"
#include "AmbaPrint.h"

#define SVC_FRWK_OK     (0x00000000U)
#define SVC_FRWK_NG     (0x00000001U)

#define FRWKLOG_BUF_SIZE    (512U)
#define FRWKLOG_ARGC        (2U)

static inline void AmbaFrwk_LogOK(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    char        LogBuf[FRWKLOG_BUF_SIZE];
    const char  *ArgS[FRWKLOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = AmbaUtility_StringPrintStr(LogBuf, FRWKLOG_BUF_SIZE, "[%s|OK]: %s", Argc, ArgS);
    if (Rval < FRWKLOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    }
}

static inline void AmbaFrwk_LogNG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    char        LogBuf[FRWKLOG_BUF_SIZE];
    const char  *ArgS[FRWKLOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = AmbaUtility_StringPrintStr(LogBuf, FRWKLOG_BUF_SIZE, "\033""[0;31m[%s|NG]: %s", Argc, ArgS);
    if (Rval < FRWKLOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    }
}

static inline void AmbaFrwk_LogDBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    char        LogBuf[FRWKLOG_BUF_SIZE];
    const char  *ArgS[FRWKLOG_ARGC];
    UINT32      Rval, Argc;

    Argc = 0;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = AmbaUtility_StringPrintStr(LogBuf, FRWKLOG_BUF_SIZE, "[%s|DBG]: %s", Argc, ArgS);
    if (Rval < FRWKLOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    }
}

#endif  /* SVC_LOG_H */
