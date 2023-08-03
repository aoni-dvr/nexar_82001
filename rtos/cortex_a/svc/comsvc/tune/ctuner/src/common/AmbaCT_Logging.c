/**
 *  @file AmbaCT_Logging.c
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
#include "AmbaCT_Logging.h"
#include "AmbaCT_SystemApi.h"
#include "AmbaWrap.h"

static CT_LOG_CONTAINER_s *pContainer = NULL;

UINT32 CT_LogInit(CT_LOG_CONTAINER_s *pMem, UINT32 LogLevel)
{
    UINT32 Rval = CT_ERR_1;
    if (pContainer == NULL) {
        Rval = CT_OK;
        if(AmbaWrap_memset(pMem, 0x0, sizeof(CT_LOG_CONTAINER_s)) != CT_OK) {
            Rval = CT_ERR_1;
        }
        pContainer = pMem;
        pContainer->LogLevel = LogLevel;

    }
    return Rval;
}



static void CT_LogConvToU8(const UINT64 *pData, UINT8 **pDataU8)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataU8, &pDataU64, sizeof(UINT8*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToS8(const UINT64 *pData, INT8 **pDataS8)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataS8, &pDataU64, sizeof(INT8*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToU16(const UINT64 *pData, UINT16 **pDataU16)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataU16, &pDataU64, sizeof(UINT16*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToS16(const UINT64 *pData, INT16 **pDataS16)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataS16, &pDataU64, sizeof(INT16*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToU32(const UINT64 *pData, UINT32 **pDataU32)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataU32, &pDataU64, sizeof(UINT32*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToS32(const UINT64 *pData, INT32 **pDataS32)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataS32, &pDataU64, sizeof(INT32*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToS64(const UINT64 *pData, INT64 **pDataS64)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataS64, &pDataU64, sizeof(INT64*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToChar(const UINT64 *pData, char **pDataChar)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataChar, &pDataU64, sizeof(char*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

static void CT_LogConvToDB(const UINT64 *pData, DOUBLE **pDataDB)
{
    UINT32 Rval;
    const UINT64 *pDataU64 = pData;
    Rval = AmbaWrap_memcpy(pDataDB, &pDataU64, sizeof(DOUBLE*));
    if (Rval != CT_OK) {
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
}

CT_LOG_MSG_s *CT_LogPutU8(CT_LOG_MSG_s *pMsg, UINT8 Data)
{
    UINT8 *pDataU8;
    pMsg[0].Id = CT_LOG_TYPE_VAR_U8;
    CT_LogConvToU8(&pMsg[0].Data, &pDataU8);
    *pDataU8 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutS8(CT_LOG_MSG_s *pMsg, INT8 Data)
{
    INT8 *pDataS8;
    pMsg[0].Id = CT_LOG_TYPE_VAR_S8;
    CT_LogConvToS8(&pMsg[0].Data, &pDataS8);
    *pDataS8 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutU16(CT_LOG_MSG_s *pMsg, UINT16 Data)
{
    UINT16 *pDataU16;
    pMsg[0].Id = CT_LOG_TYPE_VAR_U16;
    CT_LogConvToU16(&pMsg[0].Data, &pDataU16);
    *pDataU16 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutS16(CT_LOG_MSG_s *pMsg, INT16 Data)
{
    INT16 *pDataS16;
    pMsg[0].Id = CT_LOG_TYPE_VAR_S16;
    CT_LogConvToS16(&pMsg[0].Data, &pDataS16);
    *pDataS16 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutU32(CT_LOG_MSG_s *pMsg, UINT32 Data)
{
    UINT32 *pDataU32;
    pMsg[0].Id = CT_LOG_TYPE_VAR_U32;
    CT_LogConvToU32(&pMsg[0].Data, &pDataU32);
    *pDataU32 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutS32(CT_LOG_MSG_s *pMsg, INT32 Data)
{
    INT32 *pDataS32;
    pMsg[0].Id = CT_LOG_TYPE_VAR_S32;
    CT_LogConvToS32(&pMsg[0].Data, &pDataS32);
    *pDataS32 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutU64(CT_LOG_MSG_s *pMsg, UINT64 Data)
{
    pMsg[0].Id = CT_LOG_TYPE_VAR_U64;
    pMsg[0].Data = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutS64(CT_LOG_MSG_s *pMsg, INT64 Data)
{
    INT64 *pDataS64;
    pMsg[0].Id = CT_LOG_TYPE_VAR_S64;
    CT_LogConvToS64(&pMsg[0].Data, &pDataS64);
    *pDataS64 = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutChar(CT_LOG_MSG_s *pMsg, char Data)
{
    char *pDataChar;
    pMsg[0].Id = CT_LOG_TYPE_VAR_CHAR;
    CT_LogConvToChar(&pMsg[0].Data, &pDataChar);
    *pDataChar = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutDB(CT_LOG_MSG_s *pMsg, DOUBLE Data)
{
    DOUBLE *pDataDB;
    pMsg[0].Id = CT_LOG_TYPE_VAR_DB;
    CT_LogConvToDB(&pMsg[0].Data, &pDataDB);
    *pDataDB = Data;
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutStr(CT_LOG_MSG_s *pMsg, const char *pData)
{
    const char *pDataConstChar = pData;
    pMsg[0].Id = CT_LOG_TYPE_VAR_STR;
    if(CT_OK != AmbaWrap_memcpy(&pMsg[0].Data, &pDataConstChar, sizeof(const char *))){
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }
    return &pMsg[1];
}

CT_LOG_MSG_s *CT_LogPutAddr(CT_LOG_MSG_s *pMsg, const void *pData)
{
    const void *pDataVoidPtr = pData;
    pMsg[0].Id = CT_LOG_TYPE_PT_ADDR;
    if(CT_OK != AmbaWrap_memcpy(&pMsg[0].Data, &pDataVoidPtr, sizeof(void *))){
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }

    return &pMsg[1];
}

static void CT_CastMsgPointer2UINT32(const CT_LOG_MSG_s *pMsg, UINT32 *pAddr)
{
    if(CT_OK != AmbaWrap_memcpy(pAddr, &pMsg, sizeof(UINT32))){
        AmbaPrint_PrintStr5("%s() call memcpy fail", __func__, NULL, NULL, NULL, NULL);
    }

}

UINT32 CT_LogPackMsg(UINT32 LogLevel, const CT_LOG_MSG_s *pMsgStart, CT_LOG_MSG_s *pMsgEol)
{
    UINT32 Rval = CT_OK;
    UINT32 MsgNum;
    UINT32 RingMsgNum;
    UINT32 MsgEolAddrMisra, MsgStartAddrMisra;
    if (pContainer != NULL) {
        pMsgEol->Id = CT_LOG_TYPE_END_OF_MSG;
        CT_CastMsgPointer2UINT32(pMsgEol, &MsgEolAddrMisra);
        CT_CastMsgPointer2UINT32(pMsgStart, &MsgStartAddrMisra);
        MsgNum = ((MsgEolAddrMisra - MsgStartAddrMisra) / CT_sizeT_to_U32(sizeof(CT_LOG_MSG_s))) + 1U;
        if (MsgNum > CT_MAX_MSG_NUM) {
            Rval = CT_ERR_1;
        } else {
            if ((pContainer->LogLevel & LogLevel) != 0U) {
                if ((pContainer->EndIdx + MsgNum) <= CT_MSG_POOL_NUM) {
                    if(CT_OK != AmbaWrap_memcpy(&pContainer->MsgPool[pContainer->EndIdx], &pMsgStart[0U], sizeof(CT_LOG_MSG_s) * MsgNum)) {
                        Rval = CT_ERR_1;
                    }
                    pContainer->EndIdx += MsgNum;
                } else {
                    pContainer->IsRing++;
                    RingMsgNum = (pContainer->EndIdx + MsgNum) - CT_MSG_POOL_NUM;
                    if ((MsgNum - RingMsgNum) > 0U) {
                        if(CT_OK != AmbaWrap_memcpy(&pContainer->MsgPool[pContainer->EndIdx], &pMsgStart[0U], sizeof(CT_LOG_MSG_s) * ((SIZE_t)MsgNum - (SIZE_t)RingMsgNum))) {
                            Rval = CT_ERR_1;
                        }
                    }
                    if(CT_OK != AmbaWrap_memcpy(&pContainer->MsgPool[0U], &pMsgStart[MsgNum - RingMsgNum], sizeof(CT_LOG_MSG_s) * RingMsgNum)) {
                        Rval = CT_ERR_1;
                    }
                    pContainer->EndIdx = RingMsgNum;
                }
            }
        }
    }
    return Rval;
}

static inline UINT32 CT_StrPutChar(char *pStr, UINT32 StrBufLen, UINT32 Index, const char C)
{
    UINT32 Rval = CT_OK;
    if (StrBufLen > Index) {
        pStr[Index] = C;
    } else {
        Rval = CT_ERR_1;
    }
    return Rval;
}

UINT32 CT_U64toStr(char *pStr, UINT32 StrBufLen, UINT64 Number)
{
    UINT32 Rval = CT_OK;
    if (Number == 0U) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        UINT64 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT64 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT64 CopyOfNumber;
        char ModResultCharMisra;
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10U;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra);
            if (Rval != 0U) {
                break;
            }
        }
        Rval = CT_StrPutChar(pStr, StrBufLen, Length,'\0');
    }
    return Rval;
}

UINT32 CT_U32toStr(char *pStr, UINT32 StrBufLen, UINT32 Number)
{
    UINT32 Rval = CT_OK;
    if (Number == 0U) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        UINT32 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT32 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT32 CopyOfNumber;
        char ModResultCharMisra;
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10U;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra);
            if (Rval != 0U) {
                break;
            }
        }
        Rval = CT_StrPutChar(pStr, StrBufLen, Length,'\0');
    }
    return Rval;
}

UINT32 CT_U16toStr(char *pStr, UINT32 StrBufLen, UINT16 Number)
{
    UINT32 Rval = CT_OK;
    if (Number == 0U) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        UINT16 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT16 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT16 CopyOfNumber;
        char ModResultCharMisra;
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10U;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra);
            if (Rval != 0U) {
                break;
            }
        }
        Rval = CT_StrPutChar(pStr, StrBufLen, Length,'\0');
    }
    return Rval;
}

UINT32 CT_U8toStr(char *pStr, UINT32 StrBufLen, UINT8 Number)
{
    UINT32 Rval = CT_OK;
    if (Number == 0U) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        UINT8 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT8 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT8 CopyOfNumber;
        char ModResultCharMisra;
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10U;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra);
            if (Rval != 0U) {
                break;
            }
        }
        Rval = CT_StrPutChar(pStr, StrBufLen, Length,'\0');
    }
    return Rval;
}

UINT32 CT_S64toStr(char *pStr, UINT32 StrBufLen, INT64 Number)
{
    UINT32 Rval = CT_OK;
    if(Number == 0) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        INT64 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT64 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT32 IsNegative = 0U;
        UINT64 CopyOfNumber;
        char ModResultCharMisra;
        if(NumberMisra < 0) {
            IsNegative = 1U;
            NumberMisra = 0 - NumberMisra;
            Length++;
        }
        CopyOfNumber = (UINT64)NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = (UINT64)NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra/*'0'*/);
            if (Rval != 0U) {
                break;
            }
        }
        if(IsNegative == 1U) {
            Rval |= CT_StrPutChar(pStr, StrBufLen, 0U,'-');
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');
    }
    return Rval;
}

UINT32 CT_S32toStr(char *pStr, UINT32 StrBufLen, INT32 Number)
{
    UINT32 Rval = CT_OK;
    if(Number == 0) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        INT32 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT32 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT32 IsNegative = 0U;
        UINT32 CopyOfNumber;
        char ModResultCharMisra;
        if(NumberMisra < 0) {
            IsNegative = 1U;
            NumberMisra = 0 - NumberMisra;
            Length++;
        }
        CopyOfNumber = (UINT32)NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = (UINT32) NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra/*'0'*/);
            if (Rval != 0U) {
                break;
            }
        }
        if(IsNegative == 1U) {
            Rval |= CT_StrPutChar(pStr, StrBufLen, 0U,'-');
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');
    }
    return Rval;
}

UINT32 CT_S16toStr(char *pStr, UINT32 StrBufLen, INT16 Number)
{
    UINT32 Rval = CT_OK;
    if(Number == 0) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        INT16 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT16 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT32 IsNegative = 0U;
        UINT16 CopyOfNumber;
        char ModResultCharMisra;
        if(NumberMisra < 0) {
            IsNegative = 1U;
            NumberMisra = 0 - NumberMisra;
            Length++;
        }
        CopyOfNumber = (UINT16)NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = (UINT16)NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra/*'0'*/);
            if (Rval != 0U) {
                break;
            }
        }
        if(IsNegative == 1U) {
            Rval |= CT_StrPutChar(pStr, StrBufLen, 0U,'-');
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');
    }
    return Rval;
}

UINT32 CT_S8toStr(char *pStr, UINT32 StrBufLen, INT8 Number)
{
    UINT32 Rval = CT_OK;
    if(Number == 0) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        INT8 NumberMisra = Number;
        UINT32 Divide = 0U;
        UINT8 ModResult, ModResultAdd48;
        UINT32 Length = 0U;
        UINT32 IsNegative = 0U;
        UINT8 CopyOfNumber;
        char ModResultCharMisra;
        if(NumberMisra < 0) {
            IsNegative = 1U;
            NumberMisra = 0 - NumberMisra;
            Length++;
        }
        CopyOfNumber = (UINT8)NumberMisra;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 10U;
        }
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = (UINT8)NumberMisra % 10U;
            ModResultAdd48 = ModResult + 48U;/*'0'*/
            ModResultCharMisra = (char)ModResultAdd48;
            NumberMisra = NumberMisra / 10;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide + 1U), ModResultCharMisra/*'0'*/);
            if (Rval != 0U) {
                break;
            }
        }
        if(IsNegative == 1U) {
            Rval |= CT_StrPutChar(pStr, StrBufLen, 0U,'-');
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');
    }
    return Rval;
}

static inline INT32 CT_DBtoS32(DOUBLE Number)
{
    return (INT32) Number;
}

static inline UINT64 CT_DBtoU64(DOUBLE Number)
{
    return (UINT64) Number;
}
UINT32 CT_DBtoStr(char *pStr, UINT32 StrBufLen, DOUBLE Number)
{
#define MAX_ACCURACY (6U)
    UINT32 Rval = CT_OK;
    if(Number == 0.0) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, '\0');
    } else {
        UINT32 Length = 0U;
        UINT32 IsNegative = 0U;
        UINT64 Decimal;
        UINT32 Index;
        UINT32 ZeroNum = 0;
        UINT32 Accuracy;
        DOUBLE PowMsira;
        DOUBLE NumberMisra = Number;
        if(NumberMisra < 0.0) {
            IsNegative = 1U;
            NumberMisra = 0.0 - NumberMisra;
            Length++;
        }
        if(IsNegative == 1U) {
            Rval = CT_StrPutChar(pStr, StrBufLen, 0U,'-');
        }
        Rval |= CT_U64toStr(&pStr[Length], StrBufLen - 1U, (UINT64)NumberMisra);
        NumberMisra = NumberMisra - (DOUBLE)(CT_DBtoS32(NumberMisra));
        Length = CT_sizeT_to_U32(AmbaUtility_StringLength(pStr));
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '.');
        Length++;
        // Note # check usable decimal num:
        Accuracy = StrBufLen - Length - 1U;
        if (Accuracy > MAX_ACCURACY) {
            Accuracy = MAX_ACCURACY;
        }


        Rval |= AmbaWrap_pow(10.0, (DOUBLE)Accuracy + 1.0, &PowMsira);
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        Decimal = CT_DBtoU64(NumberMisra * PowMsira);
        if ((Decimal % 10U) >= 5U){
            Decimal += 5U;
        }
        Decimal /= 10U;

        for (Index = 0; Index < Accuracy; Index++) {
            Rval |= AmbaWrap_pow(10.0, (DOUBLE)Index, &PowMsira);
            if (Decimal >= (UINT64)PowMsira) {
                ZeroNum = Accuracy - 1U - Index;
            }
        }
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        for (Index = 0; Index < ZeroNum; Index++) {
            pStr[Length] = '0';
            Length++;
        }
        Rval |= CT_U64toStr(&pStr[Length], StrBufLen - Length, (UINT64)Decimal);
    }
    return Rval;
}

UINT32 CT_U32toHexStr(char *pStr, UINT32 StrBufLen, UINT32 Number)
{
    UINT32 Rval = CT_OK;
    UINT32 AddrMisraC;

    UINT32 Divide = 0U;
    UINT32 ModResult = 0U;
    UINT32 Length = 0U;
    UINT32 CopyOfNumber;
    static const char HexChar[] = {
        '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
    };

    Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
    Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, 'x');

    if(Number == 0U) {
        Rval |= CT_StrPutChar(pStr, StrBufLen, 2U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 3U, '\0');
    } else {
        AddrMisraC = Number;
        CopyOfNumber = Number;
        while(CopyOfNumber != 0U) {
            Length++;
            CopyOfNumber /= 16U;
        }

        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = AddrMisraC % 16U;
            AddrMisraC = AddrMisraC / 16U;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length + 2U - (Divide + 1U), HexChar[ModResult]);
            if (Rval != 0U) {
                break;
            }
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length + 2U, '\0');
    }
    return Rval;
}
UINT32 CT_AddrtoStr(char *pStr, UINT32 StrBufLen, const void *pAddr)
{
    UINT32 Rval = CT_OK;
    if(pAddr == (void*)NULL) {
        if (StrBufLen > 3U) {
            pStr[0] = '0';
            pStr[1] = 'x';
            pStr[2] = '0';
            pStr[3] = '\0';
        } else {
            Rval = CT_ERR_1;
        }
    } else {
        UINT32 AddrMisraC;

        UINT32 Divide = 0U;
        UINT32 ModResult = 0U;
        UINT32 Length = 0U;
        UINT32 CopyOfNumber;
        static const char HexChar[] = {
            '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'
        };
        Rval |= AmbaWrap_memcpy(&AddrMisraC, &pAddr, sizeof(AddrMisraC));
        if(Rval != CT_OK) {
            Rval = CT_ERR_1;
        }

        CopyOfNumber = AddrMisraC;
        while(CopyOfNumber != 0U) {
            Length++;\
            CopyOfNumber /= 16U;
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, 0U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, 1U, 'x');
        for(Divide = 0U; Divide < Length; Divide++) {
            ModResult = AddrMisraC % 16U;
            AddrMisraC = AddrMisraC / 16U;
            Rval = CT_StrPutChar(pStr, StrBufLen, Length + 2U - (Divide + 1U), HexChar[ModResult]);
            if (Rval != 0U) {
                break;
            }
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length + 2U, '\0');
    }
    return Rval;
}

UINT32 CT_ChartoStr(char *pStr, UINT32 StrBufLen, UINT32 Index, const char C)
{
    UINT32 Rval = CT_OK;
    Rval |= CT_StrPutChar(pStr, StrBufLen, Index, C);
    Rval |= CT_StrPutChar(pStr, StrBufLen, Index + 1U, '\0');
    return Rval;
}

UINT32 CT_S16toStrinLength(char *pStr, UINT32 StrBufLen, INT16 Number, UINT32 Length)
{
    UINT32 Rval = CT_OK;
    INT16 NumberMisra = Number;
    UINT32 Divide = 0U;
    UINT16 ModResult, ModResultAdd48;
    UINT32 NumberLength = 0U;
    UINT32 IsNegative = 0U;
    UINT16 CopyOfNumber;
    char ModResultCharMisra;
    UINT32 MisraStart = 0U;
    if(Number == 0) {
        for(Divide = 0U; Divide < (Length - 1U); Divide++) {
            Rval |= CT_StrPutChar(pStr, StrBufLen, Divide,' ');
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length - 1U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');
    } else {
        if(NumberMisra < 0) {
            IsNegative = 1U;
            NumberMisra = 0 - NumberMisra;
            NumberLength++;
        }
        CopyOfNumber = (UINT16)NumberMisra;
        while(CopyOfNumber != 0U) {
            NumberLength++;
            CopyOfNumber /= 10U;
        }
        if(NumberLength >= Length){
            for(Divide = 0U; Divide < NumberLength; Divide++) {
                ModResult = (UINT16)NumberMisra % 10U;
                ModResultAdd48 = ModResult + 48U;/*'0'*/
                ModResultCharMisra = (char)ModResultAdd48;
                NumberMisra = NumberMisra / 10;
                Rval = CT_StrPutChar(pStr, StrBufLen, NumberLength - (Divide + 1U), ModResultCharMisra/*'0'*/);
                if (Rval != 0U) {
                    break;
                }
            }
            if(IsNegative == 1U) {
                Rval |= CT_StrPutChar(pStr, StrBufLen, 0U,'-');
            }
            Rval |= CT_StrPutChar(pStr, StrBufLen, NumberLength, '\0');
        } else {
            MisraStart = Length - NumberLength;
            for(Divide = 0U; Divide < MisraStart; Divide++) {
                Rval |= CT_StrPutChar(pStr, StrBufLen, Divide,' ');
            }
            for(Divide = MisraStart; Divide < Length; Divide++) {
                ModResult = (UINT16)NumberMisra % 10U;
                ModResultAdd48 = ModResult + 48U;/*'0'*/
                ModResultCharMisra = (char)ModResultAdd48;
                NumberMisra = NumberMisra / 10;
                Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide - MisraStart + 1U), ModResultCharMisra/*'0'*/);
                if (Rval != 0U) {
                    break;
                }
            }
            if(IsNegative == 1U) {
                Rval |= CT_StrPutChar(pStr, StrBufLen, MisraStart,'-');
            }
            Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');

        }
    }
    return Rval;
}

UINT32 CT_U32toStrinLength(char *pStr, UINT32 StrBufLen, UINT32 Number, UINT32 Length)
{
    UINT32 Rval = CT_OK;
    UINT32 NumberMisra = Number;
    UINT32 Divide = 0U;
    UINT32 ModResult, ModResultAdd48;
    UINT32 NumberLength = 0U;
    UINT32 CopyOfNumber;
    char ModResultCharMisra;
    UINT32 MisraStart = 0U;
    if(Number == 0U) {
        for(Divide = 0U; Divide < (Length - 1U); Divide++) {
            Rval |= CT_StrPutChar(pStr, StrBufLen, Divide,' ');
        }
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length - 1U, '0');
        Rval |= CT_StrPutChar(pStr, StrBufLen, Length, '\0');
    } else {
        CopyOfNumber = NumberMisra;
        while(CopyOfNumber != 0U) {
            NumberLength++;
            CopyOfNumber /= 10U;
        }
        if(NumberLength >= Length){
            for(Divide = 0U; Divide < NumberLength; Divide++) {
                ModResult = NumberMisra % 10U;
                ModResultAdd48 = ModResult + 48U;/*'0'*/
                ModResultCharMisra = (char)ModResultAdd48;
                NumberMisra = NumberMisra / 10U;
                Rval = CT_StrPutChar(pStr, StrBufLen, NumberLength - (Divide + 1U), ModResultCharMisra/*'0'*/);
                if (Rval != 0U) {
                    break;
                }
            }
            Rval = CT_StrPutChar(pStr, StrBufLen, NumberLength, '\0');
        } else {
            MisraStart = Length - NumberLength;
            for(Divide = 0U; Divide < MisraStart; Divide++) {
                Rval |= CT_StrPutChar(pStr, StrBufLen, Divide,' ');
            }
            for(Divide = MisraStart; Divide < Length; Divide++) {
                ModResult = NumberMisra % 10U;
                ModResultAdd48 = ModResult + 48U;/*'0'*/
                ModResultCharMisra = (char)ModResultAdd48;
                NumberMisra = NumberMisra / 10U;
                Rval = CT_StrPutChar(pStr, StrBufLen, Length - (Divide - MisraStart + 1U), ModResultCharMisra/*'0'*/);
                if (Rval != 0U) {
                    break;
                }
            }
            Rval = CT_StrPutChar(pStr, StrBufLen, Length, '\0');
        }
    }
    return Rval;
}


