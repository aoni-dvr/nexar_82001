/**
 *  @file AmbaCT_Parser.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */
#include "AmbaCT_Parser.h"
#include "AmbaCT_SystemApi.h"
//#include "AmbaMisraFix.h"
// #include "AmbaImgCalibItuner.h"
#ifndef GNU_SECTION_NOZEROINIT
#define GNU_SECTION_NOZEROINIT
#endif

#define MAX_REGS (200)

typedef void (*pfunc_conv_t)(INT32 N, const void *Src, const void *Dst);

static void CT_str_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_u8(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_s8(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_u16(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_s16(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_u32(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_s32(INT32 N, const void *Src, const void *Dst);
static void CT_str_to_db(INT32 N, const void *Src, const void *Dst);
static void CT_u8_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_s8_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_u16_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_s16_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_u32_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_s32_to_str(INT32 N, const void *Src, const void *Dst);
static void CT_db_to_str(INT32 N, const void *Src, const void *Dst);

static void CT_CheckRval(UINT32 Rval, const char *pFuncName, const char *pCallerName)
{
    if (Rval != 0U) {
        AmbaPrint_PrintStr5("Error %s() call %s() Fail", pCallerName, pFuncName, NULL, NULL, NULL);
    }
}

void CT_Parser_Enc_Proc(const PARAM_s *pParam, const void *pField)
{
    static const pfunc_conv_t convstrs[TYPE_MAX] = {
        CT_str_to_str,
        CT_str_to_str,
        CT_u8_to_str,
        CT_s8_to_str,
        CT_u16_to_str,
        CT_s16_to_str,
        CT_u32_to_str,
        CT_s32_to_str,
        CT_db_to_str,
    };
    convstrs[pParam->ValType]((INT32)pParam->ValCount, pField, pParam->ValStr);
}

void CT_Parser_Dec_Proc(const PARAM_s *pParam, const void *pField)
{
    static const pfunc_conv_t strconvs[TYPE_MAX] = {
        CT_str_to_str,
        CT_str_to_str,
        CT_str_to_u8,
        CT_str_to_s8,
        CT_str_to_u16,
        CT_str_to_s16,
        CT_str_to_u32,
        CT_str_to_s32,
        CT_str_to_db,
    };
    strconvs[pParam->ValType]((INT32)pParam->ValCount, pParam->ValStr, pField);
}

static void CT_str_to_str(INT32 N, const void *Src, const void *Dst)
{
    const char *pSrc;
    char *pDst;
    if (N==0) {
        //misraC fix
    }
    CT_CheckRval(CT_memcpy (&pSrc, &Src, sizeof(const char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pDst, &Dst, sizeof(UINT8 *)), "CT_memcpy", __func__);
    (void)CT_strcpy((char *)pDst, (const char *)pSrc);
}

static void CT_str_to_u8(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    UINT8 *pU8, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
//    const char *Token = NULL;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    const char *Token;

    CT_CheckRval(CT_memcpy (&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pU8, &Dst, sizeof(UINT8 *)), "CT_memcpy", __func__);

    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (UINT8)CT_strtoul(Token, &atoi_sep, 10);
        CT_CheckRval(CT_memcpy(&pU8[i], &Value, sizeof(UINT8)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}

static void CT_str_to_s8(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    INT8 *pS8, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    CT_CheckRval(CT_memcpy (&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pS8, &Dst, sizeof(INT8 *)), "CT_memcpy", __func__);

    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (INT8)CT_strtol(Token, &atoi_sep, 10);
        CT_CheckRval(CT_memcpy(&pS8[i], &Value, sizeof(INT8)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void CT_str_to_u16(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    UINT16 *pU16, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    CT_CheckRval(CT_memcpy (&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pU16, &Dst, sizeof(UINT16 *)), "CT_memcpy", __func__);

    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (UINT16)CT_strtoul(Token, &atoi_sep, 10);
        CT_CheckRval(CT_memcpy(&pU16[i], &Value, sizeof(UINT16)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void CT_str_to_s16(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    INT16 *pS16, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    CT_CheckRval(CT_memcpy (&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pS16, &Dst, sizeof(INT16 *)), "CT_memcpy", __func__);
    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (INT16)CT_strtol(Token, &atoi_sep, 10);
        CT_CheckRval(CT_memcpy(&pS16[i], &Value, sizeof(INT16)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void CT_str_to_u32(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    UINT32 *pU32, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    CT_CheckRval(CT_memcpy (&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pU32, &Dst, sizeof(UINT32 *)), "CT_memcpy", __func__);
    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (UINT32)CT_strtoul(Token, &atoi_sep, 10);
        CT_CheckRval(CT_memcpy(&pU32[i], &Value, sizeof(UINT32)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void CT_str_to_s32(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    INT32 *pS32, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    CT_CheckRval(CT_memcpy (&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pS32, &Dst, sizeof(INT32 *)), "CT_memcpy", __func__);
    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (INT32)CT_strtol(Token, &atoi_sep, 10);
        CT_CheckRval(CT_memcpy(&pS32[i], &Value, sizeof(INT32)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}

static void CT_str_to_db(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    DOUBLE *pDb, Value = 0.0;
    const char *Token;
//    char end_of_str = '\0';
    char *pStr; //, *atof_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    CT_CheckRval(CT_memcpy(&pStr, &Src, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&pDb, &Dst, sizeof(DOUBLE *)), "CT_memcpy", __func__);
    Token = CT_strtok_r(pStr, Seps, &pchar);
    while ((Token != NULL) && (i < N)) {
        Value = (DOUBLE) CT_atof(Token);
        CT_CheckRval(CT_memcpy(&pDb[i], &Value, sizeof(DOUBLE)), "CT_memcpy", __func__);
        Token = CT_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}

static void CT_u8_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const UINT8 *pU8;
    UINT8 Value = 0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pU8, &Src, sizeof(UINT8 *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pU8[i], sizeof(UINT8)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_uint32_to_string(Token, 16U, (UINT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    //sprintf(OneStr, "%d", Value);
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pU8[i], sizeof(UINT8)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_uint32_to_string(Token, 16U, (UINT32)Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        //sprintf(OneStr, " %d", Value);
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void CT_s8_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT8 *pS8;
    INT8 Value = 0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pS8, &Src, sizeof(INT8 *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pS8[i], sizeof(INT8)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_int32_to_string(Token, 16U, (INT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    //sprintf(OneStr, "%d", Value);
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pS8[i], sizeof(INT8)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_int32_to_string(Token, 16U, (INT32)Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        //sprintf(OneStr, " %d", Value);
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void CT_u16_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const UINT16 *pU16;
    UINT16 Value = 0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pU16, &Src, sizeof(UINT16 *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pU16[i], sizeof(UINT16)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_uint32_to_string(Token, 16U, (UINT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    //sprintf(OneStr, "%d", Value);
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pU16[i], sizeof(UINT16)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_uint32_to_string(Token, 16U, (UINT32)Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        //sprintf(OneStr, " %d", Value);
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void CT_s16_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT16 *pS16;
    INT16 Value = 0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pS16, &Src, sizeof(INT16 *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pS16[i], sizeof(INT16)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_int32_to_string(Token, 16U, (INT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    //sprintf(OneStr, "%d", Value);
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pS16[i], sizeof(INT16)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_int32_to_string(Token, 16U, (INT32)Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        //sprintf(OneStr, " %d", Value);
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void CT_u32_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const UINT32 *pU32;
    UINT32 Value = 0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pU32, &Src, sizeof(UINT32 *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pU32[i], sizeof(UINT32)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_uint32_to_string(Token, 16U, (UINT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    //sprintf(OneStr, "%d", (INT32) Value);
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pU32[i], sizeof(UINT32)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_uint32_to_string(Token, 16U, (UINT32)Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        //sprintf(OneStr, " %d", (INT32) Value);
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void CT_s32_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT32 *pS32;
    INT32 Value = 0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pS32, &Src, sizeof(INT32 *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pS32[i], sizeof(INT32)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_int32_to_string(Token, 16U, (INT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    //sprintf(OneStr, "%d", (INT32) Value);
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pS32[i], sizeof(INT32)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_int32_to_string(Token, 16U, Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        //sprintf(OneStr, " %d", Value);
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}

static void CT_db_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT32 *pDb;
    DOUBLE Value = 0.0;
    char Token[16];
    CT_CheckRval(CT_memcpy (&pStr, &Dst, sizeof(char *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy (&pDb, &Src, sizeof(DOUBLE *)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memcpy(&Value, &pDb[i], sizeof(DOUBLE)), "CT_memcpy", __func__);
    CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
    (void)CT_int32_to_string(Token, 16U, (INT32)Value, 10U);
    (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
    (void)CT_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        CT_CheckRval(CT_memcpy(&Value, &pDb[i], sizeof(DOUBLE)), "CT_memcpy", __func__);
        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        CT_CheckRval(CT_memset(&OneStr[0], 0x0, sizeof(char)*16U), "CT_memset", __func__);
        (void)CT_double_to_string(Token, 16U, Value, 10U);
        (void)CT_strcat(OneStr, " ", CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(OneStr, Token, CT_sizeT_to_U32(sizeof(OneStr)));
        (void)CT_strcat(pStr, OneStr, MAX_LINE_CHARS);
    }
}

INT32 CT_Parser_Create(CT_Parser_Object_t **Object)
{
    INT32 i = 0 , ret;
    static REG_s *pReg_List[MAX_REGS] GNU_SECTION_NOZEROINIT;
    static char LineBuf[MAX_LINE_CHARS] GNU_SECTION_NOZEROINIT;
    static CT_Parser_Object_t Parser_Object[1];
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = -1;
    } else {
        for (i = MAX_REGS - 1; i >= 0; i--) {
            pReg_List[i] = NULL;
        }
        Parser_Object[0].RegList = pReg_List;
        Parser_Object[0].RegCount = 0;
        Parser_Object[0].LineBuf = LineBuf;
        Parser_Object[0].BufSize = (INT32)MAX_LINE_CHARS;
        *Object = &Parser_Object[0];
        ret = 0;
    }

    return ret;
}

INT32 CT_Parser_Destory(CT_Parser_Object_t *Object)
{
    INT32 ret;
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = -1;
    } else {
        Object->RegList = NULL;
        Object->RegCount = 0;
        Object->LineBuf = NULL;
        Object->BufSize = 0;
        ret = 0;
    }
    return ret;
}

INT32 CT_Parser_Add_Reg(REG_s *pReg, CT_Parser_Object_t *Object)
{
    INT32 Ret;
    if (Object->RegCount < MAX_REGS) {
        Object->RegList[Object->RegCount] = pReg;
        Ret = Object->RegCount;
        Object->RegCount++;
    } else {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Exceed maximum REG number", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        Ret = -1;
    }
    return Ret;
}

INT32 CT_Parser_Set_Reglist_Valid(INT64 Valid, const CT_Parser_Object_t *Object)
{
    INT32 i, j, Ret;
    if ((Object == NULL) || (Object->RegList == NULL) || (Object->RegCount <= 0)) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s invalid Object", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        Ret = -1;
    } else {
        for (i = Object->RegCount - 1; i >= 0; i--) {
            for (j = 0; j < PARAM_STATUS_ARRAY_NUM; j++) {
                Object->RegList[i]->ParamStatus[j] = (UINT64)Valid;
            }
        }
        Ret = 0;
    }
    return Ret;
}

INT32 CT_Parser_Set_Reg_Valid(const char *Name, INT32 IsMultipleReg, UINT64 Valid, const CT_Parser_Object_t *Object)
{
    REG_s *Reg;
    INT32 i, Ret;
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        Ret = -1;
    } else {
        Reg = CT_Parser_Lookup_Reg(Name, IsMultipleReg, Object);
        if (Reg == NULL) {
            if (IsMultipleReg > 0){
                AmbaPrint_PrintStr5("[PASER][WARNING] %s , %s%s Not Exist", __func__, Name, "[]", DC_S, DC_S);
                AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintStr5("[PASER][WARNING] %s , %s%s Not Exist", __func__, Name, "", DC_S, DC_S);
                AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
            }
            Ret = -1;
        } else {
            for (i = 0; i < PARAM_STATUS_ARRAY_NUM; i++) {
                Reg->ParamStatus[i] = Valid;
            }
            Ret = 0;
        }
    }

    return Ret;
}
REG_s *CT_Parser_Get_Reg(INT32 RegIdx, const CT_Parser_Object_t *Object)
{
    REG_s *ret;
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = NULL;
    } else if (RegIdx >= Object->RegCount) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d, RegIdx %d > RegCount %d ", __LINE__, (UINT32)RegIdx, (UINT32)Object->RegCount, 0U, 0U);
        ret = NULL;
    } else {
        ret = Object->RegList[RegIdx];
    }
    return ret;
}

PARAM_s *CT_Parser_Get_Param(const REG_s *Reg, INT32 ParamIdx)
{
    PARAM_s *ret;
    if (Reg == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = NULL;
    } else if (ParamIdx >= Reg->ParamCount) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = NULL;
    } else {
        ret = &Reg->ParamList[ParamIdx];
    }
    return ret;
}
REG_s *CT_Parser_Lookup_Reg(const char *Name, INT32 IsMultipleReg, const CT_Parser_Object_t *Object)
{
    REG_s *Ret = NULL;
    INT32 i, check;
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object= NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        Ret = NULL;
    } else {
        for (i = Object->RegCount - 1; i >= 0; i--) {
            if (CT_strcmp(Name, Object->RegList[i]->Name) == 0) {
                if((Object->RegList[i]->RegNum - 1)>0){
                    check = 1;
                } else {
                    check = 0;
                }
                if (IsMultipleReg == check) {
                    Ret = Object->RegList[i];
                    break;
                } else {
                    Ret = NULL;
                }
            } else {
                Ret = NULL;
            }
        }
        if (Ret == NULL) {
            if (IsMultipleReg > 0){
                AmbaPrint_PrintStr5("[PASER][WARNING] %s , Unknown REG: %s%s ", __func__, Name, "[]", DC_S, DC_S);
                AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
            } else {
                AmbaPrint_PrintStr5("[PASER][WARNING] %s , Unknown REG: %s%s ", __func__, Name, "", DC_S, DC_S);
                AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
            }
        }
    }

    return Ret;
}

PARAM_s *CT_Parser_Lookup_Param(const char *Name, const REG_s *Reg)
{
    PARAM_s *Ret = NULL;
    INT32 i;
    if (Reg == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        Ret = NULL;
    } else {
        for (i = Reg->ParamCount - 1; i >= 0; i--) {
            if (CT_strcmp(Name, Reg->ParamList[i].Name) == 0) {
                Ret = &Reg->ParamList[i];
                break;
            } else {
                Ret = NULL;
            }
        }
        if (Ret == NULL) {
            AmbaPrint_PrintStr5("[PASER][WARNING] %s, Reg  %s Unknown Param %s", __func__, Reg->Name, Name, DC_S, DC_S);
            AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        }
    }
    return Ret;
}

static INT32 CT_skip_line(const char *Linebuf)
{
    INT32 ret;
    if (*Linebuf == '\0') {
        ret = 1;
    } else if (*Linebuf == '#') {
        ret = 1;
    } else {
        ret = 0;
    }

    return ret;
}

static INT32 CT_split_reg_id(char *linebuf, char **reg_id, char **param)
{
    UINT32 pos;
    const char seps[] = ".";
    INT32 Ret;
    pos = CT_strcspn(linebuf, seps);
    if (pos == CT_strlen(linebuf)){
        Ret = 0;
    } else {
        linebuf[pos] = '\0';
        *reg_id = linebuf;
        *param = &linebuf[pos + 1U];
        Ret = 1;
    }
    return Ret;
}

static INT32 CT_split_param_value(char *ParamStr, char **Param, char **Val)
{
    UINT32 Pos;
    const char Seps[] = " \t";
    INT32 Ret;
    Pos = CT_strcspn(ParamStr, Seps);
    if (Pos == CT_strlen(ParamStr)){
        Ret = 0;
    } else {
        ParamStr[Pos] = '\0';
        *Param = ParamStr;
        *Val = &ParamStr[Pos + 1U];
        Ret = 1;
    }
    return Ret;
}

static INT32 CT_split_reg_param(char *Linebuf, char **Reg, char **Param)
{
    UINT32 Pos;
    const char Seps[] = "[.";
    INT32 ret;
    Pos = CT_strcspn(Linebuf, Seps);
    if (Pos == CT_strlen(Linebuf)){
        ret = 0;
    } else {
        Linebuf[Pos] = '\0';
        *Reg = Linebuf;
        *Param = &Linebuf[Pos + 1U];
        ret = 1;
    }
    return ret;
}

INT32 CT_Parser_Generate_Line(INT32 RegIdx, INT32 RegNumIdx, INT32 ParamIdx, const CT_Parser_Object_t *Object)
{
    const REG_s *Reg = NULL;
    PARAM_s *Param = NULL;
    char RegNumStr[12] = "";
    char Token[16];
    INT32 ret = 0, MisraI32;
    UINT8 check;
    UINT64 MisraU64;
    UINT32 MisraU32;
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object= NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = -1;
    } else if (RegIdx >= Object->RegCount) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d , RegIdx %d >= RegCount %d", __LINE__, (UINT32)RegIdx, (UINT32)Object->RegCount, 0U, 0U);
        ret = -1;
    } else {
        Reg = Object->RegList[RegIdx];
        if (ParamIdx >= Reg->ParamCount) {
            AmbaPrint_PrintStr5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
            AmbaPrint_PrintUInt5("[PASER][WARNING]: %d , ParamIdx %d >= ParamCount %d", __LINE__, (UINT32)ParamIdx, (UINT32)Reg->ParamCount, 0U, 0U);
            ret = -1;
        } else {
            Reg = CT_Parser_Get_Reg(RegIdx, Object);
            if (Reg != NULL) {
                if ((Reg->Attribute & ATTRIBUTE_HIDE) != 0U)  {
                    ret = -1;
                } else if ((Reg->ParamList[ParamIdx].Attribute & ATTRIBUTE_HIDE) != 0U) {
                    ret = -1;
                } else {
                    Param = &Reg->ParamList[ParamIdx];
                    // TODO: Avoid bufer overflow
                    if((Reg->Attribute & ATTRIBUTE_COMMENT_OUT)>0U){
                        check = 1;
                    } else if ((Reg->ParamList[ParamIdx].Attribute & ATTRIBUTE_COMMENT_OUT)>0U) {
                        check = 1;
                    } else {
                        check = 0;
                    }
                    if (check > 0U) {
                        Object->LineBuf[0] = '#';
                        (void)CT_strcpy(&Object->LineBuf[1], Reg->Name);
                    } else {
                        (void)CT_strcpy(Object->LineBuf, Reg->Name);
                    }
                    if (Reg->RegNum > 1) {
                        CT_CheckRval(CT_memset(&Token[0], 0x0, sizeof(char)*16U), "CT_memcpy", __func__);
                        (void)CT_int32_to_string(Token, 16, (INT32)RegNumIdx, 10);
                        (void)CT_strcat(RegNumStr, "[", CT_sizeT_to_U32(sizeof(RegNumStr)));
                        (void)CT_strcat(RegNumStr, Token, CT_sizeT_to_U32(sizeof(RegNumStr)));
                        (void)CT_strcat(RegNumStr, "]", CT_sizeT_to_U32(sizeof(RegNumStr)));
                        //sprintf(RegNumStr, "[%d]", RegNumIdx);
                        (void)CT_strcat(Object->LineBuf, RegNumStr, MAX_LINE_CHARS);
                    }
                    (void)CT_strcat(Object->LineBuf, ".", MAX_LINE_CHARS);
                    (void)CT_strcat(Object->LineBuf, Param->Name, MAX_LINE_CHARS);
                    (void)CT_strcat(Object->LineBuf, " ", MAX_LINE_CHARS);
                    Param->ValStr = &Object->LineBuf[ CT_strlen(Object->LineBuf) ]; //? char * + len of string
                    Reg->Process(ParamIdx + (RegNumIdx * Reg->ParamCount), NULL, Param); //4096
                    (void)CT_strcat(Object->LineBuf, "\n", MAX_LINE_CHARS);
                    MisraI32 = (INT32)(ParamIdx % 64L);
                    MisraU32 = (UINT32)MisraI32;
                    MisraU64 = (UINT64)MisraU32;
                    if ((Reg->ParamStatus[ParamIdx / 64] & (0x1ULL << MisraU64)) != 0U) {
                        ret = 0;
                    } else {
                        ret = 1;
                    }
                }
            }else {
                AmbaPrint_PrintStr5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
                AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
                ret = -1;
            }
        }
    }
    return ret;
}

INT32 CT_Parser_Get_LineBuf(const CT_Parser_Object_t *Object, char **Line, INT32 *Size)
{
    INT32 ret ;
    if (Object == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s Object= NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        AmbaPrint_PrintUInt5("[PASER][WARNING]: %d ", __LINE__, 0U, 0U, 0U, 0U);
        ret = -1;
    } else if (Object->LineBuf == NULL) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s LineBuf is NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ret = -1;
    } else if (Object->BufSize == 0) {
        AmbaPrint_PrintStr5("[PASER][WARNING] %s BufSize is 0", __func__, DC_S, DC_S, DC_S, DC_S);
        ret = -1;
    } else {
        *Line = Object->LineBuf;
        *Size = Object->BufSize;
        ret = 0;
    }
    return ret;
}

INT32 CT_Parser_Parse_Line(const CT_Parser_Object_t *Object)
{
    char *RegStr = NULL, *ParamValStr = NULL, *ParamStr = NULL, *ValStr = NULL;
    char *RegIdStr = NULL;
    REG_s *pReg;
    PARAM_s *pParam;
    INT32 RegId = 0;
    INT32 IsMultipleReg = 0;
    INT32 ret ;
    if (CT_skip_line(Object->LineBuf) != 0) {
        ret = 0;
    } else {
        // AmbaPrint("## %s\n", Object->LineBuf);
        if (CT_split_reg_param(Object->LineBuf, &RegStr, &ParamValStr) != 0) {
            if (NULL != CT_strstr(ParamValStr, "].")) {
                IsMultipleReg = 1;
            }
            pReg = CT_Parser_Lookup_Reg(RegStr, IsMultipleReg, Object);
            if (pReg == NULL) {
                ;
            } else {
                /* Check if multiple reg instances*/
                if (pReg->RegNum > 1) {
                    if (CT_split_reg_id(ParamValStr, &RegIdStr, &ParamValStr) != 0) {
                        CT_str_to_s32(1, RegIdStr, &RegId);
                    }
                }

                if (CT_split_param_value(ParamValStr, &ParamStr, &ValStr) != 0) {
                    pParam = CT_Parser_Lookup_Param(ParamStr, pReg);
                    if (pParam == NULL) {
                            ;
                    } else {
                        pParam->ValStr = ValStr;
                        (*pReg->Process)(pParam->Index + (RegId * pReg->ParamCount), pReg->ParamStatus, pParam);
                    }
                }
            }
        }
        ret = 0;
    }
    return ret;
}

static UINT32 CT_Parser_Get_Param_Status(INT32 RegIdx, INT32 ParamIdx, const REG_s *Reg)
{
    INT32 AbsoluteParamIdx = (Reg->ParamCount * RegIdx) + ParamIdx;
    UINT32 Rval;
    if ((Reg->ParamStatus[AbsoluteParamIdx / 64] & (0x1ULL << ((UINT32)AbsoluteParamIdx % 64U))) != 0U) {
        Rval = 1U;
    } else {
        Rval = 0U;
    }
    return Rval;
}

UINT32 CT_Parser_Check_Param_Completeness(const CT_Parser_Object_t *pObject, UINT8 (*pIsFilterValidFunc)(UINT8 Index))
{
    INT32 Filter_Idx;
    INT32 Param_Idx;
    UINT32 Ret = 0;
    const REG_s* Reg = NULL;
    const PARAM_s* Param = NULL;

    for (Filter_Idx = 0; Filter_Idx < pObject->RegCount; Filter_Idx++) {
        Reg = CT_Parser_Get_Reg(Filter_Idx, pObject);
        if (Reg == NULL) {
            //AmbaPrint("call TUNE_Param_Get_Reg Fail");
            AmbaPrint_PrintStr5("call TUNE_Param_Get_Reg Fail", NULL, NULL, NULL, NULL, NULL);
            continue;
        }
        if (pIsFilterValidFunc((UINT8)Reg->Index) == 1U) {
            if (Reg->RegNum == 1) {
                for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                    if (CT_Parser_Get_Param_Status(0, Param_Idx, Reg) == 0U) {
                        Param = CT_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                        AmbaPrint_PrintStr5("Lost Param: %s %s", Reg->Name, Param->Name, NULL, NULL, NULL);

                        Ret = 0xFFFFFFFFU;
                    }
                }
            } else {
                INT32 i;
                for (i = 0; i < Reg->RegNum; i++) {
                    INT32 LoseParamNum = 0;
                    for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                        if (CT_Parser_Get_Param_Status(i, Param_Idx, Reg) == 0U) {
                            LoseParamNum++;
                        }
                    }
                    if (LoseParamNum != Reg->ParamCount) {
                        for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                            if (CT_Parser_Get_Param_Status(i, Param_Idx, Reg) == 0U) {
                                Param = CT_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                                AmbaPrint_PrintStr5("Lost Param:", Reg->Name, "[]", Param->Name, NULL, NULL);
                                Ret = 0xFFFFFFFFU;
                            }
                        }
                    }
                }
            }
        }
    }
    return Ret;
}

