/**
*  @file ArmLog.c
*
* Copyright (c) [2021] Ambarella International LP
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
/* Standard header */
#include <stdio.h>
#include <errno.h>
#include <string.h>

/* Arm header */
#include "ArmLog.h"

#define LOG_BUF_SIZE        (256)

//Regular bold text
#define RED         "1;31"
#define GREEN       "1;32"
#define YELLOW      "1;33"

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
void ArmLog_OK(const char *pModule, const char *pFormat, uint32_t Arg1, uint32_t Arg2)
{
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "\033[%sm[%s|%s]: %s\033[m\n", YELLOW, pModule, "OK", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, Arg1, Arg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
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
void ArmLog_ERR(const char *pModule, const char *pFormat, uint32_t Arg1, uint32_t Arg2)
{
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "\033[%sm[%s|%s]: %s\033[m\n", RED, pModule, "ERR", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, Arg1, Arg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
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
void ArmLog_WARN(const char *pModule, const char *pFormat, uint32_t Arg1, uint32_t Arg2)
{
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "\033[%sm[%s|%s]: %s\033[m\n", GREEN, pModule, "WARN", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, Arg1, Arg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
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
void ArmLog_DBG(const char *pModule, const char *pFormat, uint32_t Arg1, uint32_t Arg2)
{
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "[%s|%s]: %s\n", pModule, "DBG", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, Arg1, Arg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
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
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "[%s|%s]: %s\n", pModule, "DBG", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, pArg1, pArg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_FLT
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
void ArmLog_FLT(const char *pModule, const char *pFormat, float Arg1, float Arg2)
{
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "[%s|%s]: %s\n", pModule, "DBG", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, Arg1, Arg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmLog_U64
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
void ArmLog_U64(const char *pModule, const char *pFormat, uint64_t Arg1, uint64_t Arg2)
{
    char log_buf[LOG_BUF_SIZE] = {'\0'};
    int32_t ret;

    ret = sprintf(log_buf, "[%s|%s]: %s\n", pModule, "DBG", pFormat);
    if (!(ret >= LOG_BUF_SIZE || ret < 0)) {
        printf((const char*)log_buf, Arg1, Arg2);
    } else {
        if (ret < 0) {
            printf("\033[%sm[%s|ERR]: sprintf() fail, %s\033[m\n", RED, pModule, strerror(errno));
        } else {
            printf("\033[%sm[%s|WARN]: Exceed log buffer size\033[m\n", GREEN, pModule);
        }
    }
}

