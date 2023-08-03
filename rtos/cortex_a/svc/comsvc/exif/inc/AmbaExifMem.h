/**
*  @file AmbaExifMem.h
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


#ifndef AMBA_EXIF_MEM_H
#define AMBA_EXIF_MEM_H

#define MM_SEEK_SET     0U
#define MM_SEEK_CUR     1U
#define MM_SEEK_END     2U

typedef struct {
    UINT8*  pBuf;
    UINT32  Position;
    UINT32  Length;
} AMBA_EXIF_MEM;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcMem.c
\*-----------------------------------------------------------------------------------------------*/
void   AmbaExif_MemInit(AMBA_EXIF_MEM* pMem, UINT8* pBuf, UINT32 BufSize);
UINT32 AmbaExif_MemGetByte(AMBA_EXIF_MEM* pMem);
UINT32 AmbaExif_MemGetLe16(AMBA_EXIF_MEM* pMem);
UINT32 AmbaExif_MemGetLe32(AMBA_EXIF_MEM* pMem);
UINT32 AmbaExif_MemGetBe16(AMBA_EXIF_MEM* pMem);
UINT32 AmbaExif_MemGetBe32(AMBA_EXIF_MEM* pMem);
UINT32 AmbaExif_MemRead(AMBA_EXIF_MEM* pMem, void* pBuf, UINT32 ByteSize, UINT32 Count);
UINT32 AmbaExif_MemSeek(AMBA_EXIF_MEM* pMem, INT32 Offset, UINT32 Mode);
UINT32 AmbaExif_MemEof(const AMBA_EXIF_MEM* pMem);

UINT32 AmbaExif_MemPutBe16(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data);
UINT32 AmbaExif_MemPutBe32(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data);
UINT32 AmbaExif_MemPutLe16(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data);
UINT32 AmbaExif_MemPutLe32(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Data);
UINT32 AmbaExif_MemWrite(UINT8 **pBufferCur, const UINT8 *pBufferBase, const UINT8 *pBufferLimit, UINT32 Size, const void *pData);

#endif /* AMBA_EXIF_MEM_H */
