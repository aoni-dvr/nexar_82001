/**
*  @file AmbaVfs.h
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
*  @details Virtual layer of file system
*
*/

#ifndef AMBA_VFS_H
#define AMBA_VFS_H

#include "AmbaCFS.h"


#define AMBA_VFS_CB_TYPE_WRITE (0UL)
#define AMBA_VFS_CB_TYPE_MAX   (1UL)

typedef struct {
    AMBA_CFS_FILE_s  *pFile;
    UINT32           AsyncEnabled;
    UINT32           AsyncId;
    char             Drive;
} AMBA_VFS_FILE_s;

typedef struct {
    ULONG                        BufAddr;
    UINT32                       BufSize;
    UINT32                       TaskPriority;
    UINT32                       TaskCpuBits;
    AMBA_CFS_CHECK_CACHED_f      pCbCacheChk;
    AMBA_CFS_CACHE_CLEAN_f       pCbCacheCln;
    AMBA_CFS_CACHE_INVALIDATE_f  pCbCacheInv;
    AMBA_CFS_VIRT_TO_PHYS_f      pCbVirToPhy;
} AMBA_VFS_INIT_s;

typedef void (*VfsCb)(const AMBA_VFS_FILE_s *pVfsFile, void *pParam);

UINT32 AmbaVFS_EvalMemSize(UINT32 *pSize);
UINT32 AmbaVFS_Init(const AMBA_VFS_INIT_s *pInit);
void   AmbaVFS_RegCbFunc(UINT32 Type, VfsCb CbFunc);

UINT32 AmbaVFS_Open(const char *pFileName, const char *pMode, UINT32 EnableAsync, AMBA_VFS_FILE_s *pVfsFile);
UINT32 AmbaVFS_Close(const AMBA_VFS_FILE_s *pVfsFile);
UINT32 AmbaVFS_Read(void *pBuf, UINT32 Size, UINT32 Count, const AMBA_VFS_FILE_s *pVfsFile, UINT32 *pNumSuccess);
UINT32 AmbaVFS_Write(void *pBuf, UINT32 Size, UINT32 Count, const AMBA_VFS_FILE_s *pVfsFile, UINT32 *pNumSuccess);
UINT32 AmbaVFS_Tell(const AMBA_VFS_FILE_s *pVfsFile, UINT64* pFilePos);
UINT32 AmbaVFS_Seek(const AMBA_VFS_FILE_s *pVfsFile, INT64 Offset, INT32 Origin);
UINT32 AmbaVFS_GetPos(const AMBA_VFS_FILE_s *pVfsFile, UINT64 *pFilePos);
UINT32 AmbaVFS_SetPos(const AMBA_VFS_FILE_s *pVfsFile, const UINT64 *pFilePos);
UINT32 AmbaVFS_Eof(const AMBA_VFS_FILE_s* pVfsFile, UINT32* IsEof);
UINT32 AmbaVFS_Sync(const AMBA_VFS_FILE_s* pVfsFile);
UINT32 AmbaVFS_NullWrite(const AMBA_VFS_FILE_s* pVfsFile);

#endif  /* AMBA_VFS_H */
