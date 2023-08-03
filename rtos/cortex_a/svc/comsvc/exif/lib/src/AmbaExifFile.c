/**
*  @file AmbaExifFile.c
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
*  @details File opeation
*
*/
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaFS.h"
//#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaCache.h"

#include "AmbaFrwk.h"
#include "AmbaExifUtil.h"
#include "AmbaExifFile.h"
#include "AmbaVfs.h"

#include "AmbaWrap.h"
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"


#define NG                                              1U

#define EXIFFLOG_OK       AmbaFrwk_LogOK
#define EXIFFLOG_NG       AmbaFrwk_LogNG
//#define EXIFFLOG_DBG      AmbaFrwk_LogDBG

UINT32 AmbaExif_FileGetByte(AMBA_FS_FILE *pFile)
{
    UINT8  Val = 0U;
    UINT32 Rval;

    Rval = AmbaExif_FileRead(&Val, 1, 1, pFile);

    if (Rval != 1U) {
        //DBG_FILEOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        Rval = 0xFFFFFFFFU;
    }

    return Rval;
}

UINT32 AmbaExif_FileGetLe16(AMBA_FS_FILE *pFile)
{
    UINT8  Val[2U] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_FileRead(Val, 1U, 2U, pFile);

    if (Rval != 2U) {
        //DBG_FILEOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        EXIFFLOG_OK(__func__, "Line %d", __LINE__, 0U);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (Val[0U] | ((UINT32)Val[1U]<<8U));
    }

    return Rval;
}

UINT32 AmbaExif_FileGetLe32(AMBA_FS_FILE *pFile)
{
    UINT8  Val[4U] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_FileRead(Val, 1U, 4U, pFile);

    if (Rval != 4U) {
        //DBG_FILEOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (Val[0U] | ((UINT32)Val[1U]<<8U) | ((UINT32)Val[2U]<<16U) | ((UINT32)Val[3U]<<24U));
    }

    return Rval;
}

UINT32 AmbaExif_FileGetBe16(AMBA_FS_FILE *pFile)
{
    UINT8  Val[2U] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_FileRead(Val, 1U, 2U, pFile);

    if (Rval != 2U) {
        //DBG_FILEOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (((UINT32)Val[0U]<<8U) | Val[1U]);
    }

#if 0
SVC_WRAP_PRINT "0x%x"
SVC_PRN_ARG_S __func__
SVC_PRN_ARG_PROC EXIFFLOG_OK
SVC_PRN_ARG_UINT32  Rval
SVC_PRN_ARG_E
#endif

    return Rval;
}

UINT32 AmbaExif_FileGetBe32(AMBA_FS_FILE *pFile)
{
    UINT8  Val[4U] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_FileRead(Val, 1U, 4U, pFile);

    if (Rval != 4U) {
        //DBG_FILEOP_ERR("%s : Fio Read data error!, %d", __func__, __LINE__);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (((UINT32)Val[0U]<<24U) | ((UINT32)Val[1U]<<16U) | ((UINT32)Val[2U]<<8U) | Val[3U]);
    }

#if 0
SVC_WRAP_PRINT "0x%x"
SVC_PRN_ARG_S __func__
SVC_PRN_ARG_PROC EXIFFLOG_OK
SVC_PRN_ARG_UINT32  Rval
SVC_PRN_ARG_E
#endif

    return Rval;
}

UINT32 AmbaExif_FilePutByte(AMBA_FS_FILE *pFile, UINT8 Code)
{
    return AmbaExif_FileWrite(&Code, 1, 1, pFile);
}

UINT32 AmbaExif_FilePutBe16(AMBA_FS_FILE *pFile, UINT32 Code)
{
    UINT8 Val[2U];
    const UINT8 *pCode = (UINT8*)&Code;

    Val[1U] = pCode[0U];
    Val[0U] = pCode[1U];

    return AmbaExif_FileWrite(Val, 1U, 2U, pFile);
}

UINT32 AmbaExif_FileBufferPutBe16(UINT32 StreamID, UINT32 Code)
{
    UINT8 Val[2U];
    const UINT8 *pCode = (UINT8*)&Code;

    Val[1U] = pCode[0U];
    Val[0U] = pCode[1U];

    return AmbaExif_FileBufferWrite(StreamID, 2U, Val);
}

UINT32 AmbaExif_FilePutBe32(AMBA_FS_FILE *pFile, UINT32 Code)
{
    UINT8 Val[4U];
    const UINT8 *pCode = (UINT8*)&Code;

    Val[3U] = pCode[0U];
    Val[2U] = pCode[1U];
    Val[1U] = pCode[2U];
    Val[0U] = pCode[3U];

    return AmbaExif_FileWrite(Val, 1U, 4U, pFile);
}

UINT32 AmbaExif_FilePutLe16(AMBA_FS_FILE *pFile, UINT32 Code)
{
    UINT8 Val[2U];
    const UINT8 *pCode = (UINT8*)&Code;

    Val[1U] = pCode[1U];
    Val[0U] = pCode[0U];

    return AmbaExif_FileWrite(Val, 1U, 2U, pFile);
}

UINT32 AmbaExif_FileByteSwap(UINT32 Code)
{
    UINT32 Rval;

    Rval  = (Code & 0x000000FFU) << 24U;
    Rval += (Code & 0x0000FF00U) << 8U;
    Rval += (Code & 0x00FF0000U) >> 8U;
    Rval += (Code & 0xFF000000U) >> 24U;

    return Rval;
}

#if 0
UINT32 AmbaExif_FileCircularRead(AMBA_FS_FILE *pFile, const CIRCULAR_BUF_s *pCirBuf)
{
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(pCirBuf->pCurBuf);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pCirBuf->pBaseBuf);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(pCirBuf->pLimitBuf);

    if (temp1 < temp2) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if(temp1 > temp4) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval == OK) {
        UINT32 temp5 = (pCirBuf->CurSize + temp1) - 1U;
        if (temp5 > temp4) {
            UINT32 CopySize = (temp4 - temp1) + 1U;
            Rval = AmbaExif_FileRead(pCirBuf->pCurBuf, 1, CopySize, pFile);
            if (Rval != CopySize) {
                //DBG_FILEOP_ERR("%s : Fio Read data error!, %d", __func__, __LINE__);
                Rval = NG;
            }

            if(Rval == OK) {
                CopySize = pCirBuf->CurSize - CopySize;
                Rval += AmbaExif_FileRead(pCirBuf->pBaseBuf, 1, CopySize, pFile);
                if (Rval != pCirBuf->CurSize) {
                    //DBG_FILEOP_ERR("%s : Fio Read data error!, %d", __func__, __LINE__);
                    Rval = NG;
                }
            }
        } else {
            Rval = AmbaExif_FileRead(pCirBuf->pCurBuf, 1, pCirBuf->CurSize, pFile);
            if (Rval != pCirBuf->CurSize) {
                //DBG_FILEOP_ERR("%s : Fio Read data error!, %d", __func__, __LINE__);
                Rval = NG;
            }
        }
    }

    return Rval;
}

static UINT32 AmbaExif_FileFileWriteFromBuf(UINT8 *pBuf, UINT32 CopySize, AMBA_FS_FILE *pFile)
{
    return AmbaExif_FileWrite(pBuf, 1, CopySize, pFile);
}

UINT32 AmbaExif_FileCircularWrite(AMBA_FS_FILE *pFile, const CIRCULAR_BUF_s *pCirBuf)
{
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(pCirBuf->pCurBuf);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pCirBuf->pBaseBuf);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(pCirBuf->pLimitBuf);

    if (temp1 < temp2) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if(temp1 > temp4) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval == OK) {
        UINT32 temp5 = (pCirBuf->CurSize + temp1) - 1U;
        if (temp5 > temp4) {
            UINT32 CopySize = (temp4 - temp1) + 1U;
            Rval = AmbaExif_FileFileWriteFromBuf(pCirBuf->pCurBuf, CopySize, pFile);
            if (Rval != CopySize) {
                //DBG_FILEOP_ERR("%s : Fio Write data error!, %d", __func__, __LINE__);
                Rval = NG;
            }

            if(Rval == OK) {
                CopySize = pCirBuf->CurSize - CopySize;
                Rval += AmbaExif_FileFileWriteFromBuf(pCirBuf->pBaseBuf, CopySize, pFile);
                if (Rval != pCirBuf->CurSize) {
                    //DBG_FILEOP_ERR("%s : Fio Write data error!, %d", __func__, __LINE__);
                    Rval = NG;
                }
            }
        } else {
            Rval = AmbaExif_FileFileWriteFromBuf(pCirBuf->pCurBuf, pCirBuf->CurSize, pFile);
            if (Rval != pCirBuf->CurSize) {
                //DBG_FILEOP_ERR("%s : Fio Write data error!, %d", __func__, __LINE__);
                Rval = NG;
            }
        }
    }

    return Rval;
}
#endif

static UINT32 AmbaExif_FileFileBufferWriteFromBuf(const UINT8 *pBuf, UINT32 CopySize, UINT32 StreamID)
{
    return AmbaExif_FileBufferWrite(StreamID, CopySize, pBuf);
}

UINT32 AmbaExif_FileCircularBufferWrite(UINT32 StreamID, const CIRCULAR_BUF_s *pCirBuf)
{
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(pCirBuf->pCurBuf);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pCirBuf->pBaseBuf);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(pCirBuf->pLimitBuf);

    if (temp1 < temp2) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if(temp1 > temp4) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval == OK) {
        UINT32 temp5 = (pCirBuf->CurSize + temp1) - 1U;
        if (temp5 > temp4) {
            UINT32 CopySize = (temp4 - temp1) + 1U;

            Rval = AmbaExif_FileFileBufferWriteFromBuf(pCirBuf->pCurBuf, CopySize, StreamID);
            if (Rval != OK) {
                EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
            CopySize = pCirBuf->CurSize - CopySize;
            Rval = AmbaExif_FileFileBufferWriteFromBuf(pCirBuf->pBaseBuf, CopySize, StreamID);
            if (Rval != OK) {
                EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
            }
        } else {
            Rval = AmbaExif_FileFileBufferWriteFromBuf(pCirBuf->pCurBuf, pCirBuf->CurSize, StreamID);
        }
    }

    return Rval;
}

UINT32 AmbaExif_FileBufferWrite(UINT32 StreamID, UINT32 Size, const UINT8 *pDataBuf)
{
    UINT32                 DataAddr = 0U, *pBuf = NULL;
    UINT32                 Rval, NumSuccess = 0U;
    const AMBA_VFS_FILE_s  *pFile = NULL;

    AmbaMisra_TypeCast(&DataAddr, &pDataBuf);
    Rval = AmbaCache_DataClean(DataAddr, Size);
    if (Rval != OK) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }
    AmbaMisra_TouchUnused(&(Rval));

    AmbaMisra_TypeCast(&pFile, &StreamID);
    AmbaMisra_TypeCast(&pBuf, &DataAddr);

    return AmbaVFS_Write(pBuf, Size, 1U, pFile, &NumSuccess);
}

UINT32 AmbaExif_FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 NumSuccess = 0U;
    UINT32 Rval;

    Rval = AmbaFS_FileRead(pBuf, Size, Count, pFile, &NumSuccess);
    if (Rval != OK) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }

    return NumSuccess;
}

UINT32 AmbaExif_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile)
{
    UINT32 NumSuccess = 0U;
    UINT32 Rval;

    Rval = AmbaFS_FileWrite(pBuf, Size, Count, pFile, &NumSuccess);
    if (Rval != OK) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }

    return NumSuccess;
}

UINT64 AmbaExif_FileTell(AMBA_FS_FILE * pFile)
{
    UINT64 FilePos = 0U;
    UINT32 Rval;

    Rval = AmbaFS_FileTell(pFile, &FilePos);
    if (Rval != OK) {
        EXIFFLOG_NG(__func__, "Line %d", __LINE__, 0U);
    }

    return FilePos;
}


