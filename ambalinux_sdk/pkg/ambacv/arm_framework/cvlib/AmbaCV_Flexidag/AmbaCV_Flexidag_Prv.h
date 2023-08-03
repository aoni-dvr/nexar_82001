/**
 *  @file AmbaCV_Flexidag_Prv.h
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Data type definitions of RTOS
 *
 */

#ifndef AMBACV_FLEXIDAG_PRIV_H
#define AMBACV_FLEXIDAG_PRIV_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_AMBA_KAL
#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"

#define SCHDR_ERR_0000      (SSP_ERR_BASE | 0x00400000UL)
#define AMBA_SCHDR_PRINT_MODULE_ID      ((UINT16)(SCHDR_ERR_0000 >> 16U))     /**< Module ID for AmbaPrint */

#define FILE_WRAP                       AMBA_FS_FILE
#define SEEK_CUR_WRAP                   AMBA_FS_SEEK_CUR       /* current              */
#define SEEK_SET_WRAP                   AMBA_FS_SEEK_START     /* head                 */
#define SEEK_END_WRAP                   AMBA_FS_SEEK_END       /* end                  */

static inline uint32_t mutex_init(mutex_t *mutex)
{
    uint32_t ret;

    ret = AmbaKAL_MutexCreate(mutex, NULL);
    return ret;
}

static inline uint32_t mutex_lock(mutex_t *mutex)
{
    uint32_t ret;

    ret = AmbaKAL_MutexTake(mutex, AMBA_KAL_WAIT_FOREVER);
    return ret;
}

static inline uint32_t mutex_unlock(mutex_t *mutex)
{
    uint32_t ret;

    ret = AmbaKAL_MutexGive(mutex);
    return ret;
}

static inline uint32_t mutex_delete(mutex_t *mutex)
{
    uint32_t ret;

    ret = AmbaKAL_MutexDelete(mutex);
    return ret;
}

static inline FILE_WRAP *fopen_wrap(const char *path, const char *mode)
{
    uint32_t rval;
    FILE_WRAP *ifp;

    rval = AmbaFS_FileOpen(path, mode, &ifp);
    if (rval != 0U) {
        ifp = NULL;
    }

    return ifp;
}

static inline uint32_t fclose_wrap(FILE_WRAP *fp)
{
    return AmbaFS_FileClose(fp);
}

static inline uint32_t fread_wrap(
    void *ptr,
    uint32_t size,
    uint32_t nmemb,
    FILE_WRAP *stream)
{
    uint32_t count;

    (void) AmbaFS_FileRead(ptr, size, nmemb, stream, &count);

    return count;
}

static inline uint32_t fwrite_wrap(
    const void *ptr,
    uint32_t size,
    uint32_t nmemb,
    FILE_WRAP *stream)
{
    uint32_t count;
    void *ptr_void;
    AmbaMisra_TypeCast(&ptr_void, &ptr);
    (void) AmbaFS_FileWrite(ptr_void, size, nmemb, stream, &count);

    return count;
}

static inline uint64_t ftell_wrap(FILE_WRAP *stream)
{
    uint32_t rval;
    uint64_t fpos;
    UINT64 fpos_64 = 0UL;

    rval = AmbaFS_FileTell(stream, &fpos_64);
    fpos = (uint64_t)fpos_64;
    (void) rval;

    return fpos;
}

static inline uint32_t fseek_wrap(FILE_WRAP *stream, int64_t offset, int32_t whence)
{
    return AmbaFS_FileSeek(stream, offset, whence);
}

#else
#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef signed char         INT8;           /* 8 bits, [-128, 127] */
typedef short               INT16;          /* 16 bits */
typedef int                 INT32;          /* 32 bits */
typedef long long           INT64;          /* 64 bits */

typedef unsigned char       UINT8;          /* 8 bits, [0, 255] */
typedef unsigned short      UINT16;         /* 16 bits */
typedef unsigned int        UINT32;         /* 32 bits */
typedef unsigned long long  UINT64;         /* 64 bits */

typedef unsigned short      WCHAR;          /* 16 bits */
typedef long unsigned int   SIZE_t;

typedef float               FLOAT;
typedef double              DOUBLE;
typedef long                LONG;
typedef unsigned long       ULONG;

#define AMBA_SCHDR_PRINT_MODULE_ID      0U

#define AmbaUtility_StringCopy(a, b, c) strncpy(a, c, b)

#define FILE_WRAP           FILE
#define SEEK_CUR_WRAP       SEEK_CUR
#define SEEK_SET_WRAP       SEEK_SET
#define SEEK_END_WRAP       SEEK_END
#define fopen_wrap          fopen
#define fread_wrap          fread
#define fwrite_wrap         fwrite
#define fclose_wrap         fclose
#define ftell_wrap          ftell
#define fseek_wrap          fseek

#define mutex_init(mutex)       pthread_mutex_init(mutex, NULL)
#define mutex_lock(mutex)       pthread_mutex_lock(mutex)
#define mutex_unlock(mutex)     pthread_mutex_unlock(mutex)
#define mutex_delete(mutex)     pthread_mutex_destroy(mutex)

static uint32_t ambacv_debug_log = 0U;

static inline void AmbaPrint_ModulePrintUInt5(UINT16 ModuleID, const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    (void) ModuleID;
    if(ambacv_debug_log == 1U) {
        printf(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    }
    return;
}

static inline void AmbaMisra_TypeCast(void * pNewType, const void * pOldType)
{
    memcpy(pNewType, pOldType, sizeof(void *));
    return;
}

#define AmbaMisra_TouchUnused(x)


static inline UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n)
{
    UINT32 err = 0U;

    if (ptr == NULL) {
        err = 1U;
    } else {
        memset(ptr, v, n);
    }

    return err;
}

static inline UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    UINT32 err = 0U;

    if ((pDst == NULL) ||
        (pSrc == NULL) ||
        (num == 0U)) {
        err = 1U;
    } else {
        memcpy(pDst, pSrc, num);
    }

    return err;
}

static inline void AmbaPrint_PrintUInt5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    if (Arg5) {
        printf(pFmt, Arg1, Arg2, Arg3, Arg4, Arg5);
    } else if (Arg4) {
        printf(pFmt, Arg1, Arg2, Arg3, Arg4);
    } else if (Arg3) {
        printf(pFmt, Arg1, Arg2, Arg3);
    } else if (Arg2) {
        printf(pFmt, Arg1, Arg2);
    } else {
        printf(pFmt, Arg1);
    }
    printf("\n");
}

static inline void AmbaPrint_PrintStr5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    if (pArg5 != NULL ) {
        printf(pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
    } else if (pArg4 != NULL) {
        printf(pFmt, pArg1, pArg2, pArg3, pArg4);
    } else if (pArg3 != NULL) {
        printf(pFmt, pArg1, pArg2, pArg3);
    } else if (pArg2 != NULL) {
        printf(pFmt, pArg1, pArg2);
    } else {
        printf(pFmt, pArg1);
    }
    printf("\n");
}

static inline UINT32 AmbaUtility_StringPrintUInt32(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const UINT32 *pArgs)
{
    return snprintf(pBuffer, BufferSize, pFmtString, *pArgs);
}

static inline UINT32 AmbaUtility_StringPrintStr(char *pBuffer, UINT32 BufferSize, const char *pFmtString, UINT32 Count, const char * const pArgs[])
{
    return snprintf(pBuffer, BufferSize, pFmtString, pArgs[0]);
}

#endif


#ifdef __cplusplus
}
#endif
#endif /* AMBACV_FLEXIDAG_PRIV_H */
