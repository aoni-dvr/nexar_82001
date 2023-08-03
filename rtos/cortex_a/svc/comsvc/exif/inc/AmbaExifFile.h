/**
*  @file AmbaExifFile.h
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

#ifndef AMBA_EXIF_FILE_H
#define AMBA_EXIF_FILE_H

typedef struct {
    UINT8   *pBaseBuf;
    UINT8   *pLimitBuf;
    UINT8   *pCurBuf;
    UINT32  CurSize;
} CIRCULAR_BUF_s;

/*-----------------------------------------------------------------------------------------------*\
 * Defined in SvcFileOp.c
\*-----------------------------------------------------------------------------------------------*/
UINT32 AmbaExif_FileGetByte(AMBA_FS_FILE *pFile);
UINT32 AmbaExif_FileGetLe16(AMBA_FS_FILE *pFile);
UINT32 AmbaExif_FileGetLe32(AMBA_FS_FILE *pFile);
UINT32 AmbaExif_FileGetBe16(AMBA_FS_FILE *pFile);
UINT32 AmbaExif_FileGetBe32(AMBA_FS_FILE *pFile);
UINT32 AmbaExif_FilePutByte(AMBA_FS_FILE *pFile, UINT8 Code);
UINT32 AmbaExif_FilePutBe16(AMBA_FS_FILE *pFile, UINT32 Code);
UINT32 AmbaExif_FileBufferPutBe16(UINT32 StreamID, UINT32 Code);
UINT32 AmbaExif_FilePutBe32(AMBA_FS_FILE *pFile, UINT32 Code);
UINT32 AmbaExif_FilePutLe16(AMBA_FS_FILE *pFile, UINT32 Code);
UINT32 AmbaExif_FileByteSwap(UINT32 Code);
//UINT32 AmbaExif_FileCircularRead(AMBA_FS_FILE *pFile, const CIRCULAR_BUF_s *pCirBuf);
//UINT32 AmbaExif_FileCircularWrite(AMBA_FS_FILE *pFile, const CIRCULAR_BUF_s *pCirBuf);
UINT32 AmbaExif_FileCircularBufferWrite(UINT32 StreamID, const CIRCULAR_BUF_s *pCirBuf);
UINT32 AmbaExif_FileBufferWrite(UINT32 StreamID, UINT32 Size, const UINT8 *pDataBuf);
UINT32 AmbaExif_FileRead(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
UINT32 AmbaExif_FileWrite(void *pBuf, UINT32 Size, UINT32 Count, AMBA_FS_FILE *pFile);
UINT64 AmbaExif_FileTell(AMBA_FS_FILE * pFile);

#endif //AMBA_EXIF_FILE_H