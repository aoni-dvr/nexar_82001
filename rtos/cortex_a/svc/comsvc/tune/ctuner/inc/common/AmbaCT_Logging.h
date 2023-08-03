/**
 *  @file AmbaCT_Logging.h
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
#ifndef CT_LOGGING_H
#define CT_LOGGING_H
#include "AmbaTypes.h"
#include "AmbaErrorCode.h"

#define CT_OK (0U)
#define CT_ERR_1 (TUNE_ERR_BASE | 0x00000001U)

#define CT_MAX_MSG_NUM (20U)
#define CT_MSG_POOL_NUM (4000U)

#define CT_LOG_LEVEL_DEBUG (0x1U)
#define CT_LOG_LEVEL_WARNING (0x2U)
#define CT_LOG_LEVEL_ERROR (0x4U)
#define CT_LOG_LEVEL_INFO (0x8U)

typedef enum {
    CT_LOG_TYPE_VAR_U8 = 0,
    CT_LOG_TYPE_VAR_S8,
    CT_LOG_TYPE_VAR_U16,
    CT_LOG_TYPE_VAR_S16,
    CT_LOG_TYPE_VAR_U32,
    CT_LOG_TYPE_VAR_S32,
    CT_LOG_TYPE_VAR_U64,
    CT_LOG_TYPE_VAR_S64,
    CT_LOG_TYPE_VAR_CHAR,
    CT_LOG_TYPE_VAR_DB,
    CT_LOG_TYPE_VAR_STR,
    CT_LOG_TYPE_PT_ADDR,
    CT_LOG_TYPE_END_OF_MSG,
    CT_LOG_TYPE_MAX,
} CT_LOG_TYPE_E;

typedef struct {
    CT_LOG_TYPE_E Id;
    UINT64 Data;
    /*
    union {
        UINT8 DataU8;
        INT8 DataS8;
        UINT16 DataU16;
        INT16 DataS16;
        UINT32 DataU32;
        INT32 DataS32;
        UINT64 DataU64;
        INT64 DataS64;
        char DataChar;
        DOUBLE DataDB;
        const char *pDataStr;
        void *pAddr;
    }Data;
    */
} CT_LOG_MSG_s;

typedef struct {
    UINT32 EndIdx;
    CT_LOG_MSG_s MsgPool[CT_MSG_POOL_NUM];
    UINT32 IsRing;
    UINT32 LogLevel;
} CT_LOG_CONTAINER_s;

UINT32 CT_LogInit(CT_LOG_CONTAINER_s *pMem, UINT32 LogLevel);
CT_LOG_MSG_s *CT_LogPutU8(CT_LOG_MSG_s *pMsg, UINT8 Data);
CT_LOG_MSG_s *CT_LogPutS8(CT_LOG_MSG_s *pMsg, INT8 Data);
CT_LOG_MSG_s *CT_LogPutU16(CT_LOG_MSG_s *pMsg, UINT16 Data);
CT_LOG_MSG_s *CT_LogPutS16(CT_LOG_MSG_s *pMsg, INT16 Data);
CT_LOG_MSG_s *CT_LogPutU32(CT_LOG_MSG_s *pMsg, UINT32 Data);
CT_LOG_MSG_s *CT_LogPutS32(CT_LOG_MSG_s *pMsg, INT32 Data);
CT_LOG_MSG_s *CT_LogPutU64(CT_LOG_MSG_s *pMsg, UINT64 Data);
CT_LOG_MSG_s *CT_LogPutS64(CT_LOG_MSG_s *pMsg, INT64 Data);
CT_LOG_MSG_s *CT_LogPutChar(CT_LOG_MSG_s *pMsg, char Data);
CT_LOG_MSG_s *CT_LogPutDB(CT_LOG_MSG_s *pMsg, DOUBLE Data);
CT_LOG_MSG_s *CT_LogPutStr(CT_LOG_MSG_s *pMsg, const char *pData);
CT_LOG_MSG_s *CT_LogPutAddr(CT_LOG_MSG_s *pMsg, const void *pData);
UINT32 CT_LogPackMsg(UINT32 LogLevel, const CT_LOG_MSG_s *pMsgStart, CT_LOG_MSG_s *pMsgEol);

UINT32 CT_U64toStr(char *pStr, UINT32 StrBufLen, UINT64 Number);
UINT32 CT_U32toStr(char *pStr, UINT32 StrBufLen, UINT32 Number);
UINT32 CT_U16toStr(char *pStr, UINT32 StrBufLen, UINT16 Number);
UINT32 CT_U8toStr(char *pStr, UINT32 StrBufLen, UINT8 Number);

UINT32 CT_S64toStr(char *pStr, UINT32 StrBufLen, INT64 Number);
UINT32 CT_S32toStr(char *pStr, UINT32 StrBufLen, INT32 Number);
UINT32 CT_S16toStr(char *pStr, UINT32 StrBufLen, INT16 Number);
UINT32 CT_S8toStr(char *pStr, UINT32 StrBufLen, INT8 Number);
UINT32 CT_DBtoStr(char *pStr, UINT32 StrBufLen, DOUBLE Number);
UINT32 CT_AddrtoStr(char *pStr, UINT32 StrBufLen, const void *pAddr);
UINT32 CT_U32toHexStr(char *pStr, UINT32 StrBufLen, UINT32 Number);
UINT32 CT_ChartoStr(char *pStr, UINT32 StrBufLen, UINT32 Index, const char C);
UINT32 CT_S16toStrinLength(char *pStr, UINT32 StrBufLen, INT16 Number, UINT32 Length);
UINT32 CT_U32toStrinLength(char *pStr, UINT32 StrBufLen, UINT32 Number, UINT32 Length);

#endif
