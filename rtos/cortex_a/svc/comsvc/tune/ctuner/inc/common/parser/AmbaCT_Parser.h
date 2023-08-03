/**
 *  @file AmbaCT_Parser.h
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
#ifndef AMBA_CT_Parser_H
#define AMBA_CT_Parser_H
#include "AmbaTypes.h"
#define MAX_NAME_LENS (128U)
#define MAX_LINE_CHARS (35000U)
typedef enum {
    EXT_PATH = 0,
    STRING,
    U_8,
    S_8,
    U_16,
    S_16,
    U_32,
    S_32,
    S_DB,
    TYPE_MAX,
} Param_Value_Type_e;

typedef struct {
    INT32 Index;
    char Name[MAX_NAME_LENS];
    Param_Value_Type_e ValType;
    UINT32 ValCount;
    char *ValStr;
    UINT8 Attribute;
} PARAM_s;

typedef void (proc_t)(INT32 Index, UINT64 *pParamStatus, const PARAM_s *pParam);

#define PARAM_STATUS_ARRAY_NUM (5)
#define PARAM_STATUS_DEFAULT {0, 0, 0, 0, 0}
#define ATTRIBUTE_HIDE (0x1U)
#define ATTRIBUTE_COMMENT_OUT (0x2U)

typedef struct {
    UINT32 Index;
    char Name[MAX_NAME_LENS];
    INT32 RegNum;
    INT32 RegEntrySize;
    INT32 ParamCount;
    UINT64 ParamStatus[PARAM_STATUS_ARRAY_NUM];
    PARAM_s *ParamList;
    proc_t *Process;
    UINT8 Attribute;
} REG_s;

typedef struct {
    REG_s **RegList;
    INT32 RegCount;
    char *LineBuf;
    INT32 BufSize;
} CT_Parser_Object_t;

void CT_Parser_Enc_Proc(const PARAM_s *pParam, const void *pField);
void CT_Parser_Dec_Proc(const PARAM_s *pParam, const void *pField);
INT32 CT_Parser_Create(CT_Parser_Object_t **Object);
INT32 CT_Parser_Destory(CT_Parser_Object_t *Object);
INT32 CT_Parser_Add_Reg(REG_s *pReg, CT_Parser_Object_t *Object);
INT32 CT_Parser_Set_Reglist_Valid(INT64 Valid, const CT_Parser_Object_t *Object);
INT32 CT_Parser_Set_Reg_Valid(const char *Name, INT32 IsMultipleReg, UINT64 Valid, const CT_Parser_Object_t *Object);
REG_s* CT_Parser_Lookup_Reg(const char *Name, INT32 IsMultipleReg, const CT_Parser_Object_t *Object);
PARAM_s* CT_Parser_Lookup_Param(const char *Name, const REG_s *Reg);
INT32 CT_Parser_Get_LineBuf(const CT_Parser_Object_t *Object, char **Line, INT32 *Size);
INT32 CT_Parser_Parse_Line(const CT_Parser_Object_t *Object);
UINT32 CT_Parser_Check_Param_Completeness(const CT_Parser_Object_t *pObject, UINT8 (*pIsFilterValidFunc)(UINT8 Index));
REG_s* CT_Parser_Get_Reg(INT32 RegIdx, const CT_Parser_Object_t *Object);
PARAM_s* CT_Parser_Get_Param(const REG_s *Reg, INT32 ParamIdx);
INT32 CT_Parser_Generate_Line(INT32 RegIdx, INT32 RegNumIdx, INT32 ParamIdx, const CT_Parser_Object_t *Object);
#endif

