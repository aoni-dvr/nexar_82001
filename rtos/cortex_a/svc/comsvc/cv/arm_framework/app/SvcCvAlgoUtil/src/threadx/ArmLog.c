/**
*  @file ArmLog.c
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
*   @details The arm print log functions
*
*/

/* ESC header */
#include "ArmStdC.h"
#include "ArmLog.h"

#define LOG_BUF_SIZE        (256U)
#define LOG_ARGC            (4U)

#define RED         "\033" "[1;31m"
#define GREEN       "\033" "[1;32m"
#define YELLOW      "\033" "[1;33m"

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PrintLog
 *
 *  @Description:: static function to print log on console
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pType:     The type string to print (OK/ERR/WARN/DBG)
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void PrintLog(const char *pModule, const char *pType, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    char        LogBuf[LOG_BUF_SIZE] = {'\0'};
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc = 0U;

    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pType;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = ArmStdC_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|%s]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintStr5("[%s|WARN] Exceed log buffer size", pModule, NULL, NULL, NULL, NULL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PrintColorLog
 *
 *  @Description:: static function to print color log on console
 *
 *  @Input      ::
 *      pColor:    The color to print
 *      pModule:   The module string to print
 *      pType:     The type string to print (OK/ERR/WARN/DBG)
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void PrintColorLog(const char *pColor, const char *pModule, const char *pType, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    char        LogBuf[LOG_BUF_SIZE] = {'\0'};
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc = 0U;

    ArgS[Argc] = pColor;
    Argc++;
    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pType;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = ArmStdC_sprintfStr(LogBuf, LOG_BUF_SIZE, "%s[%s|%s]: %s\033" "[m", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintUInt5(LogBuf, Arg1, Arg2, 0U, 0U, 0U);
    } else {
        AmbaPrint_PrintStr5("[%s|WARN] Exceed log buffer size", pModule, NULL, NULL, NULL, NULL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PrintString
 *
 *  @Description:: static function to print string on console
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pType:     The type string to print (OK/ERR/WARN/DBG)
 *      pFormat:   The print format
 *      pArg1:     The first string to print
 *      pArg2:     The second string to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void PrintString(const char *pModule, const char *pType, const char *pFormat, const char *pArg1, const char *pArg2)
{
    char        LogBuf[LOG_BUF_SIZE] = {'\0'};
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc = 0U;

    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pType;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = ArmStdC_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|%s]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        AmbaPrint_PrintStr5(LogBuf, pArg1, pArg2, NULL, NULL, NULL);
    } else {
        AmbaPrint_PrintStr5("[%s|WARN] Exceed log buffer size", pModule, NULL, NULL, NULL, NULL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: PrintFloat
 *
 *  @Description:: static function to print float on console
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pType:     The type string to print (OK/ERR/WARN/DBG)
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
static void PrintFloat(const char *pModule, const char *pType, const char *pFormat, FLOAT Arg1, FLOAT Arg2)
{
    char        LogBuf[LOG_BUF_SIZE] = {'\0'};
    const char  *ArgS[LOG_ARGC];
    UINT32      Rval, Argc = 0U;

    ArgS[Argc] = pModule;
    Argc++;
    ArgS[Argc] = pType;
    Argc++;
    ArgS[Argc] = pFormat;
    Argc++;

    Rval = ArmStdC_sprintfStr(LogBuf, LOG_BUF_SIZE, "[%s|%s]: %s", Argc, ArgS);
    if (Rval < LOG_BUF_SIZE) {
        FLOAT LA1, LA2;
        LA1 = Arg1*1000.0f;
        LA2 = Arg2*1000.0f;
        AmbaPrint_PrintUInt5(LogBuf, (UINT32) Arg1, ((UINT32)LA1)%1000U, (UINT32) Arg2, ((UINT32)LA2)%1000U, 0U);
    } else {
        AmbaPrint_PrintStr5("[%s|WARN] Exceed log buffer size", pModule, NULL, NULL, NULL, NULL);
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_OK
 *
 *  @Description:: Print OK message ([Module|OK] XXX)
 *                 The OK message should be used on specfic position to track system flow
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void ArmLog_OK(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    PrintColorLog(YELLOW, pModule, "OK", pFormat, Arg1, Arg2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_ERR
 *
 *  @Description:: Print Error message ([Module|ERR] XXX)
 *                 The ERR message is used when system hit error and can't work normally
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void ArmLog_ERR(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    PrintColorLog(RED, pModule, "ERR", pFormat, Arg1, Arg2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_WARN
 *
 *  @Description:: Print WARN message ([Module|WARN] XXX)
 *                 The WARN message is similar to NG.
 *                 NG => a serious error and system may hang or abnormal
 *                 WARN => a minor error and system still can work well after that
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void ArmLog_WARN(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    PrintColorLog(GREEN, pModule, "WARN", pFormat, Arg1, Arg2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_DBG
 *
 *  @Description:: Print DBG message ([Module|DBG] XXX)
 *                 The DBG message is used to debug
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pFormat:   The print format
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void ArmLog_DBG(const char *pModule, const char *pFormat, UINT32 Arg1, UINT32 Arg2)
{
    PrintLog(pModule, "DBG", pFormat, Arg1, Arg2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_STR
 *
 *  @Description:: Print DBG "string" message ([Module|DBG] XXX)
 *                 The DBG message is used to debug
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pFormat:   The print format
 *      pArg1:     The first string to print
 *      pArg2:     The second string to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void ArmLog_STR(const char *pModule, const char *pFormat, const char *pArg1, const char *pArg2)
{
    PrintString(pModule, "DBG", pFormat, pArg1, pArg2);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_FLT
 *
 *  @Description:: Print DBG message ([Module|DBG] XXX)
 *                 The DBG message is used to debug
 *
 *  @Input      ::
 *      pModule:   The module string to print
 *      pFormat:   The print format  (use %d.%d to represent %f)
 *      Arg1:      The first arg to print
 *      Arg2:      The second arg to print
 *
 *  @Output     :: None
 *
 *  @Return     :: None
\*-----------------------------------------------------------------------------------------------*/
void ArmLog_FLT(const char *pModule, const char *pFormat, FLOAT Arg1, FLOAT Arg2)
{
    PrintFloat(pModule, "DBG", pFormat, Arg1, Arg2);
}

