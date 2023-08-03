/**
*  @file AmbaExifMem.c
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
*  @details Entry point of SVC
*
*/

#include "AmbaTypes.h"
#include "AmbaWrap.h"
#include "AmbaExifMem.h"
#include "AmbaFrwk.h"
#include "AmbaExifUtil.h"

#define NG                                              1U

//#define EXIFMLOG_OK       AmbaFrwk_LogOK
#define EXIFMLOG_NG       AmbaFrwk_LogNG
//#define EXIFMLOG_DBG      AmbaFrwk_LogDBG

void AmbaExif_MemInit(AMBA_EXIF_MEM* pMem, UINT8* pBuf, UINT32 BufSize)
{
    pMem->pBuf = pBuf;
    pMem->Position = 0;
    pMem->Length = BufSize;
}

UINT32 AmbaExif_MemGetByte(AMBA_EXIF_MEM* pMem)
{
    UINT8  Val = 0U;
    UINT32 Rval;

    Rval = AmbaExif_MemRead(pMem, &Val, 1, 1);

    if (Rval != 1U) {
        //DBG_MEMOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = Val;
    }

    return Rval;

}

UINT32 AmbaExif_MemGetLe16(AMBA_EXIF_MEM* pMem)
{
    UINT8  Val[2U] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_MemRead(pMem, Val, 1, 2);

    if (Rval != 2U) {
        //DBG_MEMOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (Val[0U] | ((UINT32)Val[1U]<<8U));
    }

    return Rval;
}

UINT32 AmbaExif_MemGetLe32(AMBA_EXIF_MEM* pMem)
{
    UINT8  Val[4] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_MemRead(pMem, Val, 1, 4);

    if (Rval != 4U) {
        //DBG_MEMOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (Val[0] | ((UINT32)Val[1]<<8) | ((UINT32)Val[2]<<16) | ((UINT32)Val[3]<<24));
    }

    return Rval;

}

UINT32 AmbaExif_MemGetBe16(AMBA_EXIF_MEM* pMem)
{
    UINT8  Val[2] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_MemRead(pMem, Val, 1, 2);

    if (Rval != 2U) {
        //DBG_MEMOP_ERR("%s : Fio Read data error, %d!", __func__, __LINE__);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (((UINT32)Val[0]<<8) | Val[1]);
    }

    return Rval;

}

UINT32 AmbaExif_MemGetBe32(AMBA_EXIF_MEM* pMem)
{
    UINT8  Val[4] = {0U};
    UINT32 Rval;

    Rval = AmbaExif_MemRead(pMem, Val, 1, 4);

    if (Rval != 4U) {
        //DBG_MEMOP_ERR("%s : Fio Read data error!, %d", __func__, __LINE__);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = 0xFFFFFFFFU;
    } else {
        Rval = (((UINT32)Val[0]<<24) | ((UINT32)Val[1]<<16) | ((UINT32)Val[2]<<8) | Val[3]);
    }

    return Rval;

}

UINT32 AmbaExif_MemSeek(AMBA_EXIF_MEM* pMem, INT32 Offset, UINT32 Mode)
{
    INT64  Tmp;
    UINT32 rtval;

    if (Mode == MM_SEEK_SET) {
        //pMem->pBuf -= pMem->Position;
        pMem->pBuf = AmbaExifUtil_PtrShift(pMem->pBuf, - (INT32)pMem->Position);
        pMem->Position = 0;
    } else if (Mode == MM_SEEK_END) {
        pMem->pBuf = AmbaExifUtil_PtrShift(pMem->pBuf, (INT32)pMem->Length - (INT32)pMem->Position);
        pMem->Position = pMem->Length;
    } else {
        //pass vcast
    }

    Tmp = (INT64)(pMem->Position) + Offset;
    if (Tmp > (INT64)(pMem->Length)) {
        rtval = NG;
    } else if (Tmp < 0) {
        rtval = NG;
    } else {
        INT32 Position = (INT32)(pMem->Position) + Offset;

        pMem->pBuf = AmbaExifUtil_PtrShift(pMem->pBuf, Offset);
        pMem->Position = (UINT32)Position;
        rtval = OK;
    }

    return rtval;
}

UINT32 AmbaExif_MemRead(AMBA_EXIF_MEM* pMem, void* pBuf, UINT32 ByteSize, UINT32 Count)
{
    UINT32 temp = pMem->Position + (ByteSize * Count);
    INT32 Int32Temp;
    UINT32 rval;

    if (temp > pMem->Length) {
        //memcpy(pBuf, pMem->pBuf, pMem->Length - pMem->Position);
        UINT32 MemCpySize = pMem->Length - pMem->Position;
        if (AmbaWrap_memcpy(pBuf, pMem->pBuf, (SIZE_t)MemCpySize) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }
        Int32Temp = (INT32)pMem->Length - (INT32)pMem->Position;
    } else {
        //memcpy(pBuf, pMem->pBuf, ByteSize * Count);
        UINT32 MemCpySize = ByteSize * Count;
        if (AmbaWrap_memcpy(pBuf, pMem->pBuf, (SIZE_t)MemCpySize) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }
        Int32Temp = (INT32)ByteSize * (INT32)Count;
    }

    pMem->pBuf = AmbaExifUtil_PtrShift(pMem->pBuf, Int32Temp);

    rval = (UINT32)Int32Temp;
    pMem->Position += rval;

    return rval;
}

UINT32 AmbaExif_MemEof(const AMBA_EXIF_MEM* pMem)
{
    UINT32 Rval;

    if (pMem->Position >= (pMem->Length-1U)){
        Rval = NG;
    } else {
        Rval = OK;
    }

    return Rval;
}

UINT32 AmbaExif_MemPutBe16(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data)
{
    UINT8 W[2];
    const UINT8 *pTmpData = (UINT8*)&Data;
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(*pBufferCur);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pBufferBase);
    const UINT8 *temp3 = AmbaExifUtil_PtrShift(*pBufferCur, 2);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(temp3);
    UINT32 temp5 = AmbaExifUtil_PtrToUint32(pBufferLimit);

    if (temp1 < temp2) {
        //DBG_MEMOP_ERR("memput_be16: pBufferCur %x out of (%x, %x)" ,*pBufferCur, pBufferBase, pBufferLimit);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if (temp4 > temp5) {
        //DBG_MEMOP_ERR("memput_be16: pBufferCur %x out of (%x, %x)" ,*pBufferCur, pBufferBase, pBufferLimit);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval != NG){
        W[1] = pTmpData[0];
        W[0] = pTmpData[1];
        //memcpy(*pBufferCur, W, 2);
        if (AmbaWrap_memcpy(*pBufferCur, W, 2) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }

        *pBufferCur = AmbaExifUtil_PtrShift(*pBufferCur, 2);
        Rval = OK;
    }

    return Rval;
}

UINT32 AmbaExif_MemPutBe32(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data)
{
    UINT8 W[4];
    const UINT8 *pTmpData = (UINT8*)&Data;
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(*pBufferCur);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pBufferBase);
    const UINT8 *temp3 = AmbaExifUtil_PtrShift(*pBufferCur, 4);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(temp3);
    UINT32 temp5 = AmbaExifUtil_PtrToUint32(pBufferLimit);

    if (temp1 < temp2) {
        //DBG_MEMOP_ERR("AmbaExif_MemPutBe32: pBufferCur %x out of (%x, %x)" ,*pBufferCur, pBufferBase, pBufferLimit);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if (temp4 > temp5){
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval != NG){
        W[3] = pTmpData[0];
        W[2] = pTmpData[1];
        W[1] = pTmpData[2];
        W[0] = pTmpData[3];
        //memcpy(*pBufferCur, W, 4);
        if (AmbaWrap_memcpy(*pBufferCur, W, 4) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }

        *pBufferCur = AmbaExifUtil_PtrShift(*pBufferCur, 4);
        Rval = OK;
    }

    return Rval;
}

UINT32 AmbaExif_MemPutLe16(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data)
{
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(*pBufferCur);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pBufferBase);
    const UINT8 *temp3 = AmbaExifUtil_PtrShift(*pBufferCur, 2);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(temp3);
    UINT32 temp5 = AmbaExifUtil_PtrToUint32(pBufferLimit);

    if (temp1 < temp2) {
        //DBG_MEMOP_ERR("AmbaExif_MemPutBe32: pBufferCur %x out of (%x, %x)" ,*pBufferCur, pBufferBase, pBufferLimit);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if (temp4 > temp5){
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval != NG){

        //memcpy(*pBufferCur, &Data, 2);
        if (AmbaWrap_memcpy(*pBufferCur, &Data, 2) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }

        *pBufferCur = AmbaExifUtil_PtrShift(*pBufferCur, 2);
        Rval = OK;
    }

    return Rval;
}

UINT32 AmbaExif_MemPutLe32(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data)
{
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(*pBufferCur);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pBufferBase);
    const UINT8 *temp3 = AmbaExifUtil_PtrShift(*pBufferCur, 4);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(temp3);
    UINT32 temp5 = AmbaExifUtil_PtrToUint32(pBufferLimit);

    if (temp1 < temp2) {
        //DBG_MEMOP_ERR("AmbaExif_MemPutBe32: pBufferCur %x out of (%x, %x)" ,*pBufferCur, pBufferBase, pBufferLimit);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if (temp4 > temp5){
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval != NG){
        //memcpy(*pBufferCur, &Data, 2);
        if (AmbaWrap_memcpy(*pBufferCur, &Data, 4) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }

        *pBufferCur = AmbaExifUtil_PtrShift(*pBufferCur, 4);
        Rval = OK;
    }

    return Rval;
}

UINT32 AmbaExif_MemWrite(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Size, const void *pData)
{
    UINT32 Rval;

    UINT32 temp1 = AmbaExifUtil_PtrToUint32(*pBufferCur);
    UINT32 temp2 = AmbaExifUtil_PtrToUint32(pBufferBase);
    const UINT8 *temp3 = AmbaExifUtil_PtrShift(*pBufferCur, 4);
    UINT32 temp4 = AmbaExifUtil_PtrToUint32(temp3);
    UINT32 temp5 = AmbaExifUtil_PtrToUint32(pBufferLimit);

    if (temp1 < temp2) {
        //DBG_MEMOP_ERR("AmbaExif_MemPutBe32: pBufferCur %x out of (%x, %x)" ,*pBufferCur, pBufferBase, pBufferLimit);
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else if (temp4 > temp5){
        EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        Rval = NG;
    } else {
        Rval = OK;
    }

    if(Rval != NG){
        //memcpy(*pBufferCur, pData, Size);
        if (AmbaWrap_memcpy(*pBufferCur, pData, Size) != OK) {
            EXIFMLOG_NG(__func__, "Line %d", __LINE__, 0U);
        }

        *pBufferCur = AmbaExifUtil_PtrShift(*pBufferCur, (INT32)Size);
        Rval = OK;
    }

    return Rval;
}
