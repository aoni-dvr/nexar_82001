/*
*  @file AmbaTUNE_Parser_cv2x.h
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

#ifndef AMBA_TUNE_PARSER_CV2X_H
#define AMBA_TUNE_PARSER_CV2X_H
#include "AmbaTypes.h"
#define MAX_NAME_LENS (128U)
#define MAX_LINE_CHARS (35000U) // (27000)
typedef enum {
    PATH_EXT = 0,
    STRING,
    U_8,
    S_8,
    U_16,
    S_16,
    U_32,
    S_32,
    S_DB,
    TYPE_MAX,
} TUNE_Param_Value_Type_e;

typedef struct {
    INT32 Index;
    char Name[MAX_NAME_LENS];
    TUNE_Param_Value_Type_e ValType;
    UINT32 ValCount;
    char *ValStr;
    UINT8 Attribute;
} TUNE_PARAM_s;

typedef void (ituner_proc_t)(INT32 Index, UINT64 *pParamStatus, const TUNE_PARAM_s *pParam);

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
    TUNE_PARAM_s *ParamList;
    ituner_proc_t *Process;
    UINT8 Attribute;
} TUNE_REG_s;


typedef struct {
    TUNE_REG_s **RegList;
    INT32 RegCount;
    char *LineBuf;
    INT32 BufSize;
} TUNE_Parser_Object_t;

void TUNE_Parser_Enc_Proc(const TUNE_PARAM_s *pParam, const void *pField);
void TUNE_Parser_Dec_Proc(const TUNE_PARAM_s *pParam, const void *pField);
INT32 TUNE_Parser_Create(TUNE_Parser_Object_t **Object);
//INT32 TUNE_Parser_Destory(TUNE_Parser_Object_t *Object);
INT32 TUNE_Parser_Add_Reg(TUNE_REG_s *pReg, TUNE_Parser_Object_t *Object);
INT32 TUNE_Parser_Set_Reglist_Valid(INT64 Valid, const TUNE_Parser_Object_t *Object);
//INT32 TUNER_Parser_Set_Reg_Valid(const char *Name, INT32 IsMultipleReg, UINT64 Valid, const TUNE_Parser_Object_t *Object);
//TUNE_REG_s* TUNE_Parser_Lookup_Reg(const char *Name, INT32 IsMultipleReg, const TUNE_Parser_Object_t *Object);
//TUNE_PARAM_s* TUNE_Parser_Lookup_Param(const char *Name, const TUNE_REG_s *Reg);
INT32 TUNE_Parser_Get_LineBuf(const TUNE_Parser_Object_t *Object, char **Line, INT32 *Size);
INT32 TUNE_Parser_Parse_Line(const TUNE_Parser_Object_t *Object);
//UINT32 TUNE_Parser_Check_Param_Completeness(const TUNE_Parser_Object_t *pObject, UINT8 (*pIsFilterValidFunc)(UINT8 Index));
TUNE_REG_s* TUNE_Parser_Get_Reg(INT32 RegIdx, const TUNE_Parser_Object_t *Object);
TUNE_PARAM_s* TUNE_Parser_Get_Param(const TUNE_REG_s *Reg, INT32 ParamIdx);
INT32 TUNE_Parser_Generate_Line(INT32 RegIdx, INT32 RegNumIdx, INT32 ParamIdx, const TUNE_Parser_Object_t *Object);
#endif

