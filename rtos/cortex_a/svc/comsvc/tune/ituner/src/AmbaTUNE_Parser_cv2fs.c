/*
*  @file AmbaTUNE_Parser_cv2fs.c
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

#include "AmbaTUNE_Parser_cv2fs.h"
#include "AmbaTUNE_SystemApi_cv2fs.h"
#include "AmbaMisraFix.h"
// #include "AmbaImgCalibItuner.h"

#define MAX_REGS (200)

typedef void (*ituner_pfunc_conv_t)(INT32 N, const void *Src, const void *Dst);

static void str_to_str(INT32 N, const void *Src, const void *Dst);
static void str_to_u8(INT32 N, const void *Src, const void *Dst);
static void str_to_s8(INT32 N, const void *Src, const void *Dst);
static void str_to_u16(INT32 N, const void *Src, const void *Dst);
static void str_to_s16(INT32 N, const void *Src, const void *Dst);
static void str_to_u32(INT32 N, const void *Src, const void *Dst);
static void str_to_s32(INT32 N, const void *Src, const void *Dst);
static void str_to_db(INT32 N, const void *Src, const void *Dst);
static void u8_to_str(INT32 N, const void *Src, const void *Dst);
static void s8_to_str(INT32 N, const void *Src, const void *Dst);
static void u16_to_str(INT32 N, const void *Src, const void *Dst);
static void s16_to_str(INT32 N, const void *Src, const void *Dst);
static void u32_to_str(INT32 N, const void *Src, const void *Dst);
static void s32_to_str(INT32 N, const void *Src, const void *Dst);
static void db_to_str(INT32 N, const void *Src, const void *Dst);

void TUNE_Parser_Enc_Proc(const TUNE_PARAM_s *pParam, const void *pField)
{
    static const ituner_pfunc_conv_t convstrs[TYPE_MAX] = {
        str_to_str,
        str_to_str,
        u8_to_str,
        s8_to_str,
        u16_to_str,
        s16_to_str,
        u32_to_str,
        s32_to_str,
        db_to_str,
    };
    convstrs[pParam->ValType]((INT32)pParam->ValCount, pField, pParam->ValStr);
}

void TUNE_Parser_Dec_Proc(const TUNE_PARAM_s *pParam, const void *pField)
{
    static const ituner_pfunc_conv_t strconvs[TYPE_MAX] = {
        str_to_str,
        str_to_str,
        str_to_u8,
        str_to_s8,
        str_to_u16,
        str_to_s16,
        str_to_u32,
        str_to_s32,
        str_to_db,
    };
    strconvs[pParam->ValType]((INT32)pParam->ValCount, pParam->ValStr, pField);
}

static void str_to_str(INT32 N, const void *Src, const void *Dst)
{
    const char *pSrc;
    char *pDst;
    if (N==0) {
        //misraC fix
    }
    (void)ituner_memcpy (&pSrc, &Src, sizeof(const char *));
    (void)ituner_memcpy (&pDst, &Dst, sizeof(UINT8 *));
    ituner_strcpy((char *)pDst, (const char *)pSrc);
}

static void str_to_u8(INT32 N, const void *Src, const void *Dst)
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

    (void)ituner_memcpy (&pStr, &Src, sizeof(char *));
    (void)ituner_memcpy (&pU8, &Dst, sizeof(UINT8 *));

    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (UINT8)ituner_strtoul(Token, &atoi_sep, 10);
        (void)ituner_memcpy(&pU8[i], &Value, sizeof(UINT8));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}

static void str_to_s8(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    INT8 *pS8, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";

    (void)ituner_memcpy (&pStr, &Src, sizeof(char *));
    (void)ituner_memcpy (&pS8, &Dst, sizeof(INT8 *));

    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (INT8)ituner_strtol(Token, &atoi_sep, 10);
        (void)ituner_memcpy(&pS8[i], &Value, sizeof(INT8));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void str_to_u16(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    UINT16 *pU16, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    (void)ituner_memcpy (&pStr, &Src, sizeof(char *));
    (void)ituner_memcpy (&pU16, &Dst, sizeof(UINT16 *));

    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (UINT16)ituner_strtoul(Token, &atoi_sep, 10);
        (void)ituner_memcpy(&pU16[i], &Value, sizeof(UINT16));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void str_to_s16(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    INT16 *pS16, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    (void)ituner_memcpy (&pStr, &Src, sizeof(char *));
    (void)ituner_memcpy (&pS16, &Dst, sizeof(INT16 *));
    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (INT16)ituner_strtol(Token, &atoi_sep, 10);
        (void)ituner_memcpy(&pS16[i], &Value, sizeof(INT16));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void str_to_u32(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    UINT32 *pU32, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    (void)ituner_memcpy (&pStr, &Src, sizeof(char *));
    (void)ituner_memcpy (&pU32, &Dst, sizeof(UINT32 *));
    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (UINT32)ituner_strtoul(Token, &atoi_sep, 10);
        (void)ituner_memcpy(&pU32[i], &Value, sizeof(UINT32));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}
static void str_to_s32(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    INT32 *pS32, Value = 0;
//    char *pStr, Seps[] = " \t", *atoi_sep = "\0";
    const char *Token ;
    char end_of_str = '\0';
    char *pStr, *atoi_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    (void)ituner_memcpy (&pStr, &Src, sizeof(char *));
    (void)ituner_memcpy (&pS32, &Dst, sizeof(INT32 *));
    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token!=NULL) && (i < N)) {
        Value = (INT32)ituner_strtol(Token, &atoi_sep, 10);
        (void)ituner_memcpy(&pS32[i], &Value, sizeof(INT32));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}

static void str_to_db(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pchar = NULL;
    DOUBLE *pDb, Value = 0.0;
    const char *Token;
//    char end_of_str = '\0';
    char *pStr; //, *atof_sep = &end_of_str;//"\0";
    const char Seps[] = " \t";
    (void) ituner_memcpy(&pStr, &Src, sizeof(char *));
    (void) ituner_memcpy(&pDb, &Dst, sizeof(DOUBLE *));
    Token = ituner_strtok_r(pStr, Seps, &pchar);
    while ((Token != NULL) && (i < N)) {
        Value = (DOUBLE) ituner_atof(Token);
        (void) ituner_memcpy(&pDb[i], &Value, sizeof(DOUBLE));
        Token = ituner_strtok_r(NULL, Seps, &pchar);
        i++;
    }
}

static void u8_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const UINT8 *pU8;
    UINT8 Value = 0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pU8, &Src, sizeof(UINT8 *));
    (void)ituner_memcpy(&Value, &pU8[i], sizeof(UINT8));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_u32_str(Token, 16UL, (UINT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    //sprintf(OneStr, "%d", Value);
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pU8[i], sizeof(UINT8));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_u32_str(Token, 16UL, (UINT32)Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        //sprintf(OneStr, " %d", Value);
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void s8_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT8 *pS8;
    INT8 Value = 0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pS8, &Src, sizeof(INT8 *));
    (void)ituner_memcpy(&Value, &pS8[i], sizeof(INT8));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_s32_str(Token, 16UL, (INT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    //sprintf(OneStr, "%d", Value);
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pS8[i], sizeof(INT8));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_s32_str(Token, 16UL, (INT32)Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        //sprintf(OneStr, " %d", Value);
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void u16_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const UINT16 *pU16;
    UINT16 Value = 0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pU16, &Src, sizeof(UINT16 *));
    (void)ituner_memcpy(&Value, &pU16[i], sizeof(UINT16));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_u32_str(Token, 16UL, (UINT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    //sprintf(OneStr, "%d", Value);
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pU16[i], sizeof(UINT16));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_u32_str(Token, 16UL, (UINT32)Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        //sprintf(OneStr, " %d", Value);
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void s16_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT16 *pS16;
    INT16 Value = 0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pS16, &Src, sizeof(INT16 *));
    (void)ituner_memcpy(&Value, &pS16[i], sizeof(INT16));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_s32_str(Token, 16UL, (INT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    //sprintf(OneStr, "%d", Value);
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pS16[i], sizeof(INT16));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_s32_str(Token, 16UL, (INT32)Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        //sprintf(OneStr, " %d", Value);
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void u32_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const UINT32 *pU32;
    UINT32 Value = 0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pU32, &Src, sizeof(UINT32 *));
    (void)ituner_memcpy(&Value, &pU32[i], sizeof(UINT32));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_u32_str(Token, 16UL, (UINT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    //sprintf(OneStr, "%d", (INT32) Value);
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pU32[i], sizeof(UINT32));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_u32_str(Token, 16UL, (UINT32)Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        //sprintf(OneStr, " %d", (INT32) Value);
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}
static void s32_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT32 *pS32;
    INT32 Value = 0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pS32, &Src, sizeof(INT32 *));
    (void)ituner_memcpy(&Value, &pS32[i], sizeof(INT32));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_s32_str(Token, 16UL, (INT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    //sprintf(OneStr, "%d", (INT32) Value);
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pS32[i], sizeof(INT32));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_s32_str(Token, 16UL, Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        //sprintf(OneStr, " %d", Value);
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}

static void db_to_str(INT32 N, const void *Src, const void *Dst)
{
    INT32 i = 0;
    char *pStr, OneStr[16];
    const INT32 *pDb;
    DOUBLE Value = 0.0;
    char Token[16];
    (void)ituner_memcpy (&pStr, &Dst, sizeof(char *));
    (void)ituner_memcpy (&pDb, &Src, sizeof(DOUBLE *));
    (void)ituner_memcpy(&Value, &pDb[i], sizeof(DOUBLE));
    (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
    (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
    (void)ituner_s32_str(Token, 16UL, (INT32)Value, 10U);
    ituner_strncat(OneStr, Token, sizeof(OneStr));
    ituner_strcpy(pStr, OneStr);
    for (i = 1; i < N; i++) {
        (void)ituner_memcpy(&Value, &pDb[i], sizeof(DOUBLE));
        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
        (void)ituner_memset(&OneStr[0], 0x0, sizeof(char)*16U);
        (void)ituner_double_str(Token, 16UL, Value, 10U);
        ituner_strncat(OneStr, " ", sizeof(OneStr));
        ituner_strncat(OneStr, Token, sizeof(OneStr));
        ituner_strncat(pStr, OneStr, MAX_LINE_CHARS);
    }
}

INT32 TUNE_Parser_Create(TUNE_Parser_Object_t **Object)
{
    INT32 i = 0, ret;
    static TUNE_REG_s *Reg_List[MAX_REGS];
    static char LineBuf[MAX_LINE_CHARS] GNU_SECTION_NOZEROINIT GNU_ALIGNED_CACHESAFE;
    static TUNE_Parser_Object_t Parser_Object[1];

    (void)ituner_memset(LineBuf, 0x0, MAX_LINE_CHARS);

    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret = -1;
    } else {
        for (i = MAX_REGS - 1; i >= 0; i--) {
            Reg_List[i] = NULL;
        }
        Parser_Object[0].RegList = Reg_List;
        Parser_Object[0].RegCount = 0;
        Parser_Object[0].LineBuf = LineBuf;
        Parser_Object[0].BufSize = (INT32)MAX_LINE_CHARS;
        *Object = &Parser_Object[0];
        ret = 0;
    }

    return ret;
}

INT32 TUNE_Parser_Destory(TUNE_Parser_Object_t *Object)
{
    INT32 ret;
    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
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

INT32 TUNE_Parser_Add_Reg(TUNE_REG_s *pReg, TUNE_Parser_Object_t *Object)
{
    INT32 Ret;
    if (Object->RegCount < MAX_REGS) {
        Object->RegList[Object->RegCount] = pReg;
        Ret = Object->RegCount;
        Object->RegCount++;
    } else {
        ituner_print_str_5("[PASER][WARNING] %s Exceed maximum REG number", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    }
    return Ret;
}

INT32 TUNE_Parser_Set_Reglist_Valid(INT64 Valid, const TUNE_Parser_Object_t *Object)
{
    INT32 i, j, Ret;
    if ((Object == NULL) || (Object->RegList == NULL) || (Object->RegCount <= 0)) {
        ituner_print_str_5("[PASER][WARNING] %s invalid Object", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
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

INT32 TUNER_Parser_Set_Reg_Valid(const char *Name, INT32 IsMultipleReg, UINT64 Valid, const TUNE_Parser_Object_t *Object)
{
    TUNE_REG_s *Reg;
    INT32 i, Ret;
    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = -1;
    } else {
        Reg = TUNE_Parser_Lookup_Reg(Name, IsMultipleReg, Object);
        if (Reg == NULL) {
            if (IsMultipleReg > 0) {
                ituner_print_str_5("[PASER][WARNING] %s , %s%s Not Exist", __func__, Name, "[]", DC_S, DC_S);
                ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
            } else {
                ituner_print_str_5("[PASER][WARNING] %s , %s%s Not Exist", __func__, Name, "", DC_S, DC_S);
                ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
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
TUNE_REG_s *TUNE_Parser_Get_Reg(INT32 RegIdx, const TUNE_Parser_Object_t *Object)
{
    TUNE_REG_s *ret;
    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret = NULL;
    } else if (RegIdx >= Object->RegCount) {
        ituner_print_str_5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d, RegIdx %d > RegCount %d ", __LINE__, (UINT32)RegIdx, (UINT32)Object->RegCount, DC_U, DC_U);
        ret = NULL;
    } else {
        ret = Object->RegList[RegIdx];
    }
    return ret;
}

TUNE_PARAM_s *TUNE_Parser_Get_Param(const TUNE_REG_s *Reg, INT32 ParamIdx)
{
    TUNE_PARAM_s *ret;
    if (Reg == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret = NULL;
    } else if (ParamIdx >= Reg->ParamCount) {
        ituner_print_str_5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret = NULL;
    } else {
        ret = &Reg->ParamList[ParamIdx];
    }
    return ret;
}
TUNE_REG_s *TUNE_Parser_Lookup_Reg(const char *Name, INT32 IsMultipleReg, const TUNE_Parser_Object_t *Object)
{
    TUNE_REG_s *Ret = NULL;
    INT32 i, check;
    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object= NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = NULL;
    } else {
        for (i = Object->RegCount - 1; i >= 0; i--) {
            if (ituner_strcmp(Name, Object->RegList[i]->Name) == 0) {
                if((Object->RegList[i]->RegNum - 1)>0) {
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
            if (IsMultipleReg > 0) {
                ituner_print_str_5("[PASER][SKIP] %s , Unknown REG: %s%s ", __func__, Name, "[]", DC_S, DC_S);
                ituner_print_uint32_5("[PASER][SKIP]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
            } else {
                ituner_print_str_5("[PASER][SKIP] %s , Unknown REG: %s%s ", __func__, Name, "", DC_S, DC_S);
                ituner_print_uint32_5("[PASER][SKIP]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
            }
        }
    }

    return Ret;
}

TUNE_PARAM_s *TUNE_Parser_Lookup_Param(const char *Name, const TUNE_REG_s *Reg)
{
    TUNE_PARAM_s *Ret = NULL;
    INT32 i;
    if (Reg == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        Ret = NULL;
    } else {
        for (i = Reg->ParamCount - 1; i >= 0; i--) {
            if (ituner_strcmp(Name, Reg->ParamList[i].Name) == 0) {
                Ret = &Reg->ParamList[i];
                break;
            } else {
                Ret = NULL;
            }
        }
        if (Ret == NULL) {
            ituner_print_str_5("[PASER][SKIP] %s, Reg  %s Unknown Param %s", __func__, Reg->Name, Name, DC_S, DC_S);
        }
    }
    return Ret;
}

static INT32 skip_line(const char *Linebuf)
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

static INT32 split_reg_id(char *linebuf, char **reg_id, char **param)
{
    UINT32 pos;
    const char seps[] = ".";
    INT32 Ret;
    pos = ituner_strcspn(linebuf, seps);
    if (pos == ituner_strlen(linebuf)) {
        Ret = 0;
    } else {
        linebuf[pos] = '\0';
        *reg_id = linebuf;
        *param = &linebuf[pos + 1U];
        Ret = 1;
    }
    return Ret;
}

static INT32 split_param_value(char *ParamStr, char **Param, char **Val)
{
    UINT32 Pos;
    const char Seps[] = " \t";
    INT32 Ret;
    Pos = ituner_strcspn(ParamStr, Seps);
    if (Pos == ituner_strlen(ParamStr)) {
        Ret = 0;
    } else {
        ParamStr[Pos] = '\0';
        *Param = ParamStr;
        *Val = &ParamStr[Pos + 1U];
        Ret = 1;
    }
    return Ret;
}

static INT32 split_reg_param(char *Linebuf, char **Reg, char **Param)
{
    UINT32 Pos;
    const char Seps[] = "[.";
    INT32 ret;
    Pos = ituner_strcspn(Linebuf, Seps);
    if (Pos == ituner_strlen(Linebuf)) {
        ret = 0;
    } else {
        Linebuf[Pos] = '\0';
        *Reg = Linebuf;
        *Param = &Linebuf[Pos + 1U];
        ret = 1;
    }
    return ret;
}

INT32 TUNE_Parser_Generate_Line(INT32 RegIdx, INT32 RegNumIdx, INT32 ParamIdx, const TUNE_Parser_Object_t *Object)
{
    const TUNE_REG_s *Reg = NULL;
    TUNE_PARAM_s *Param = NULL;
    char RegNumStr[12];
    char Token[16];
    INT32 ret = 0, MisraI32;
    UINT8 check;
    UINT64 MisraU64;
    UINT32 MisraU32;
    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object= NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret = -1;
    } else if (RegIdx >= Object->RegCount) {
        ituner_print_str_5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d , RegIdx %d >= RegCount %d", __LINE__, (UINT32)RegIdx, (UINT32)Object->RegCount, DC_U, DC_U);
        ret = -1;
    } else {
        Reg = Object->RegList[RegIdx];
        if (ParamIdx >= Reg->ParamCount) {
            ituner_print_str_5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
            ituner_print_uint32_5("[PASER][WARNING]: %d , ParamIdx %d >= ParamCount %d", __LINE__, (UINT32)ParamIdx, (UINT32)Reg->ParamCount, DC_U, DC_U);
            ret = -1;
        } else {
            Reg = TUNE_Parser_Get_Reg(RegIdx, Object);
            if (Reg != NULL) {
                if ((Reg->Attribute & ATTRIBUTE_HIDE) != 0U)  {
                    ret = -1;
                } else if ((Reg->ParamList[ParamIdx].Attribute & ATTRIBUTE_HIDE) != 0U) {
                    ret = -1;
                } else {
                    Param = &Reg->ParamList[ParamIdx];
                    // TODO: Avoid bufer overflow
                    if((Reg->Attribute & ATTRIBUTE_COMMENT_OUT)>0U) {
                        check = 1;
                    } else if ((Reg->ParamList[ParamIdx].Attribute & ATTRIBUTE_COMMENT_OUT)>0U) {
                        check = 1;
                    } else {
                        check = 0;
                    }
                    if (check > 0U) {
                        Object->LineBuf[0] = '#';
                        ituner_strcpy(&Object->LineBuf[1], Reg->Name);
                    } else {
                        ituner_strcpy(Object->LineBuf, Reg->Name);
                    }
                    if (Reg->RegNum > 1) {
                        (void)ituner_memset(&Token[0], 0x0, sizeof(char)*16U);
                        (void)ituner_s32_str(Token, 16, (INT32)RegNumIdx, 10);
                        ituner_strncat(RegNumStr, "[", sizeof(RegNumStr));
                        ituner_strncat(RegNumStr, Token, sizeof(RegNumStr));
                        ituner_strncat(RegNumStr, "]", sizeof(RegNumStr));
                        //sprintf(RegNumStr, "[%d]", RegNumIdx);
                        ituner_strncat(Object->LineBuf, RegNumStr, MAX_LINE_CHARS);
                    }
                    ituner_strncat(Object->LineBuf, ".", MAX_LINE_CHARS);
                    ituner_strncat(Object->LineBuf, Param->Name, MAX_LINE_CHARS);
                    ituner_strncat(Object->LineBuf, " ", MAX_LINE_CHARS);
                    Param->ValStr = &Object->LineBuf[ ituner_strlen(Object->LineBuf) ]; //? char * + len of string
                    Reg->Process(ParamIdx + (RegNumIdx * Reg->ParamCount), NULL, Param); //4096
                    ituner_strncat(Object->LineBuf, "\n", MAX_LINE_CHARS);
                    MisraI32 = (ParamIdx % 64L);
                    MisraU32 = (UINT32)MisraI32;
                    MisraU64 = (UINT64)MisraU32;
                    if ((Reg->ParamStatus[ParamIdx / 64] & (0x1ULL << MisraU64)) != 0U) {
                        ret = 0;
                    } else {
                        ret = 1;
                    }
                }
            } else {
                ituner_print_str_5("[PASER][WARNING] %s Reg = NULL", __func__, DC_S, DC_S, DC_S, DC_S);
                ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
                ret = -1;
            }
        }
    }
    return ret;
}

INT32 TUNE_Parser_Get_LineBuf(const TUNE_Parser_Object_t *Object, char **Line, INT32 *Size)
{
    INT32 ret ;
    if (Object == NULL) {
        ituner_print_str_5("[PASER][WARNING] %s Object= NULL", __func__, DC_S, DC_S, DC_S, DC_S);
        ituner_print_uint32_5("[PASER][WARNING]: %d ", __LINE__, DC_U, DC_U, DC_U, DC_U);
        ret = -1;
    } else if ((Object->LineBuf == NULL) || (Object->BufSize == 0)) {
        const UINT32 *pUint32;
        UINT32 MisraU32;
        ituner_print_str_5("[PASER][WARNING] %s ", __func__, DC_S, DC_S, DC_S, DC_S);
        (void)ituner_memcpy (&pUint32, &Object->LineBuf, sizeof(UINT32 *));
        AmbaMisra_TypeCast32(&MisraU32, pUint32);
        ituner_print_uint32_5("[PASER][WARNING]: %d,  LineBuf = 0x%p, Size = %d", __LINE__, MisraU32, (UINT32)Object->BufSize, DC_U, DC_U);
        ret = -1;
    } else {
        *Line = Object->LineBuf;
        *Size = Object->BufSize;
        ret = 0;
    }
    return ret;
}

INT32 TUNE_Parser_Parse_Line(const TUNE_Parser_Object_t *Object)
{
    char *RegStr = NULL, *ParamValStr = NULL, *ParamStr = NULL, *ValStr = NULL;
    char *RegIdStr = NULL;
    TUNE_REG_s *pReg;
    TUNE_PARAM_s *pParam;
    INT32 RegId = 0;
    INT32 IsMultipleReg = 0;
    INT32 ret ;
    if (skip_line(Object->LineBuf) != 0) {
        ret = 0;
    } else {
        //AmbaPrint("## %s\n", Object->LineBuf);
        if (split_reg_param(Object->LineBuf, &RegStr, &ParamValStr) != 0) {
            if (NULL != ituner_strstr(ParamValStr, "].")) {
                IsMultipleReg = 1;
            }
            pReg = TUNE_Parser_Lookup_Reg(RegStr, IsMultipleReg, Object);
            if (pReg == NULL) {
                ;
            } else {
                /* Check if multiple reg instances*/
                if (pReg->RegNum > 1) {
                    if (split_reg_id(ParamValStr, &RegIdStr, &ParamValStr) != 0) {
                        str_to_s32(1, RegIdStr, &RegId);
                    }
                }

                if (split_param_value(ParamValStr, &ParamStr, &ValStr) != 0) {
                    pParam = TUNE_Parser_Lookup_Param(ParamStr, pReg);
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

static UINT32 Parser_Get_Param_Status(INT32 RegIdx, INT32 ParamIdx, const TUNE_REG_s *Reg)
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

UINT32 TUNE_Parser_Check_Param_Completeness(const TUNE_Parser_Object_t *pObject, UINT8 (*pIsFilterValidFunc)(UINT8 Index))
{
    INT32 Filter_Idx;
    INT32 Param_Idx;
    UINT32 Ret = 0;
    const TUNE_REG_s* Reg = NULL;
    const TUNE_PARAM_s* Param = NULL;

    for (Filter_Idx = 0; Filter_Idx < pObject->RegCount; Filter_Idx++) {
        Reg = TUNE_Parser_Get_Reg(Filter_Idx, pObject);
        if (Reg == NULL) {
            //AmbaPrint("call TUNE_Param_Get_Reg Fail");
            ituner_print_str_5("call TUNE_Param_Get_Reg Fail", NULL, NULL, NULL, NULL, NULL);
            continue;
        }
        if (pIsFilterValidFunc((UINT8)Reg->Index) == 1U) {
            if (Reg->RegNum == 1) {
                for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                    if (Parser_Get_Param_Status(0, Param_Idx, Reg) == 0U) {
                        Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                        ituner_print_str_5("Lost Param:", Reg->Name, Param->Name, NULL, NULL, NULL);
                        Ret = 0xFFFFFFFFU;
                    }
                }
            } else {
                INT32 i;
                for (i = 0; i < Reg->RegNum; i++) {
                    INT32 LoseParamNum = 0;
                    for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                        if (Parser_Get_Param_Status(i, Param_Idx, Reg) == 0U) {
                            LoseParamNum++;
                        }
                    }
                    if (LoseParamNum != Reg->ParamCount) {
                        for (Param_Idx = 0; Param_Idx < Reg->ParamCount; Param_Idx++) {
                            if (Parser_Get_Param_Status(i, Param_Idx, Reg) == 0U) {
                                Param = TUNE_Parser_Get_Param(Reg, Param_Idx % Reg->ParamCount);
                                ituner_print_str_5("Lost Param:", Reg->Name, "[]", Param->Name, NULL, NULL);
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

