/*
 * Copyright (c) 2017-2018 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CVTASK_OSSRV_H_FILE
#define CVTASK_OSSRV_H_FILE

/******************************************************************************/
/*                                  THREADX BUILD                             */
/******************************************************************************/
#ifdef  CONFIG_BUILD_CV
#ifndef AMBA_KAL_SOURCE_CODE
#define AMBA_KAL_SOURCE_CODE
#endif
#include "AmbaTypes.h"
#include "AmbaUtility.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaFS.h"
#include "AmbaShell.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"
#include "AmbaCache.h"

#define SCHDR_ERR_0000      (SSP_ERR_BASE | 0x00400000UL)
#define CVTASK_ERR_0000     (SSP_ERR_BASE | 0x00410000UL)

#define AMBA_SCHDR_PRINT_MODULE_ID      ((UINT16)(SCHDR_ERR_0000 >> 16U))     /**< Module ID for AmbaPrint */
#define AMBA_CVTASK_PRINT_MODULE_ID     ((UINT16)(CVTASK_ERR_0000 >> 16U))     /**< Module ID for AmbaPrint */

extern void console_printS5(
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

extern void console_printU5(
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

extern void module_printS5(
    uint16_t module,
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

extern void module_printU5(
    uint16_t module,
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);


/******************************************************************************/
/*                                  Utilities                                 */
/******************************************************************************/

#if !defined(CONFIG_CC_USESTD)

#if !defined(VCAST_AMBA)
static inline void* cvtask_memcpy(void *pDst, const void *pSrc, size_t num)
{
    uint32_t ret;

    ret = AmbaWrap_memcpy(pDst, pSrc, num);
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("memcpy() : AmbaWrap_memcpy fail ret !", ret, 0U, 0U, 0U, 0U);
    }
    return pDst;
}

static inline void* cvtask_memset(void *ptr, int32_t v, size_t n)
{
    uint32_t ret;

    ret = AmbaWrap_memset(ptr, v, n);
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("memset() : AmbaWrap_memset fail ret !", ret, 0U, 0U, 0U, 0U);
    }

    return ptr;
}

static inline int32_t cvtask_strncmp(const char *s1, const char *s2, size_t n)
{
    return AmbaUtility_StringCompare(s1, s2, n);
}

static inline int32_t cvtask_strcmp(const char *s1, const char *s2)
{
    return AmbaUtility_StringCompare(s1, s2, AmbaUtility_StringLength(s1));
}

static inline size_t cvtask_strlen(const char *s)
{
    return AmbaUtility_StringLength(s);
}
static inline uint32_t cvtask_strtoul(const char *nptr, char **endptr, int32_t base)
{
    uint32_t rval = 0U, val = 0U;

    (void) endptr;
    (void) base;

    rval = AmbaUtility_StringToUInt32(nptr, &val);
    AmbaMisra_TouchUnused(&rval);
    AmbaMisra_TouchUnused(endptr);
    return val;
}

static inline char *cvtask_strncpy(char *dest, const char *src, size_t n)
{
    size_t CopyLength;
    size_t i;

    if ((dest == NULL) || (src == NULL) || (n == 0U)) {
        // no action
    } else {
        CopyLength = AmbaUtility_StringLength(src);
        if (CopyLength > n) {
            // overflow condition
            CopyLength = n;
        }
        for (i = 0; i < CopyLength; i++) {
            dest[i] = src[i];
        }

        if (CopyLength < n) {
            dest[CopyLength] = '\0';
        }
    }

    return dest;
}

static inline char *cvtask_strcpy(char *dest, const char *src)
{
    SIZE_t len;

    len = AmbaUtility_StringLength(src) + 1U;
    AmbaUtility_StringCopy(dest, len, src);
    return dest;
}

static inline char *cvtask_strcat(char *dest, const char *src)
{
    SIZE_t d_len, i;


    d_len = AmbaUtility_StringLength(dest);
    for (i = 0; (src[i]!='\0'); i++) {
        dest[d_len+i] = src[i];
    }
    dest[d_len+i] = '\0';

    return dest;
}
#endif // !defined(VCAST_AMBA))
#else
#include <string.h>

#define cvtask_memcpy       memcpy
#define cvtask_memset       memset
#define cvtask_strncmp      strncmp
#define cvtask_strcmp       strcmp
#define cvtask_strlen       strlen
#define cvtask_strtoul      strtoul
#define cvtask_strncpy      strncpy
#define cvtask_strcpy       strcpy
#define cvtask_strcat       strcat

#endif // !defined(CONFIG_CC_USESTD)

static inline uint32_t cvtask_sprintf_str(char *str, const char *str_src)
{
    size_t len;

    len = AmbaUtility_StringLength(str_src);
    AmbaUtility_StringCopy(str, len + 1U, str_src);

    return (uint32_t)len;
}

static inline uint32_t cvtask_snprintf_uint1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0)
{
    uint32_t rval;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

static inline uint32_t cvtask_snprintf_uint2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1)
{
    uint32_t rval;
    uint32_t args[2];

    args[0] = arg0;
    args[1] = arg1;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 2, args);

    return rval;
}

static inline uint32_t cvtask_snprintf_uint3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2)
{
    uint32_t rval;
    uint32_t args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 3, args);

    return rval;
}

static inline uint32_t cvtask_snprintf_uint4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3)
{
    uint32_t rval;
    uint32_t args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 4, args);

    return rval;
}

static inline uint32_t cvtask_snprintf_uint5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4)
{
    uint32_t rval;
    uint32_t args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 5, args);

    return rval;
}

static inline uint32_t cvtask_snprintf_uint6(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4,
    uint32_t arg5)
{
    uint32_t rval;
    uint32_t args[6];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;
    args[5] = arg5;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 6, args);

    return rval;
}


static inline uint32_t cvtask_snprintf_uint7(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    uint32_t arg0,
    uint32_t arg1,
    uint32_t arg2,
    uint32_t arg3,
    uint32_t arg4,
    uint32_t arg5,
    uint32_t arg6)
{
    uint32_t rval;
    uint32_t args[7];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;
    args[5] = arg5;
    args[6] = arg6;

    rval = AmbaUtility_StringPrintUInt32(
               str, strbuf_size, format, 7, args);

    return rval;
}

static inline uint32_t cvtask_snprintf_str1(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0)
{
    uint32_t rval;

    rval = AmbaUtility_StringPrintStr(
               str, strbuf_size, format, 1, &arg0);

    return rval;
}

static inline uint32_t cvtask_snprintf_str2(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1)
{
    const char *args[2];

    args[0] = arg0;
    args[1] = arg1;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 2, args);
}

static inline uint32_t cvtask_snprintf_str3(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2)
{
    const char *args[3];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 3, args);
}

static inline uint32_t cvtask_snprintf_str4(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3)
{
    const char *args[4];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 4, args);
}

static inline uint32_t cvtask_snprintf_str5(
    char *str,
    uint32_t strbuf_size,
    const char *format,
    const char *arg0,
    const char *arg1,
    const char *arg2,
    const char *arg3,
    const char *arg4)
{
    const char *args[5];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;

    return AmbaUtility_StringPrintStr(str, strbuf_size, format, 5, args);
}

extern void* ambacv_p2v(uint64_t pa);
extern uint64_t ambacv_v2p(void *va);
extern uint64_t ambacv_p2c(uint64_t pa);
extern uint64_t ambacv_c2p(uint64_t ca);
extern void* ambacv_c2v(uint64_t ca);
extern uint64_t ambacv_v2c(void *va);
/******************************************************************************/
/*                                  File System                               */
/******************************************************************************/

#define CVTASK_FILE                  AMBA_FS_FILE
#define CVTASK_SEEK_CUR              AMBA_FS_SEEK_CUR       /* current              */
#define CVTASK_SEEK_SET              AMBA_FS_SEEK_START     /* head                 */
#define CVTASK_SEEK_END              AMBA_FS_SEEK_END       /* end                  */

static inline CVTASK_FILE *cvtask_fopen(const char *path, const char *mode)
{
    CVTASK_FILE *ifp;
    uint32_t ret;

    ret = AmbaFS_FileOpen(path, mode, &ifp);
    if(ret != 0U) {
        AmbaPrint_PrintUInt5("fopen_wrap() : AmbaFS_FileOpen fail ret !", ret, 0U, 0U, 0U, 0U);
    }

    return ifp;
}

static inline uint32_t cvtask_fclose(CVTASK_FILE *fp)
{
    return AmbaFS_FileClose(fp);
}

static inline uint32_t cvtask_fread(
    void *ptr,
    uint32_t size,
    uint32_t nmemb,
    CVTASK_FILE *stream)
{
    uint32_t count;

    (void) AmbaFS_FileRead(ptr, size, nmemb, stream, &count);

    return count;
}

static inline uint32_t cvtask_fwrite(
    const void *ptr,
    uint32_t size,
    uint32_t nmemb,
    CVTASK_FILE *stream)
{
    uint32_t count;
    void *ptr_void;
    AmbaMisra_TypeCast(&ptr_void, &ptr);
    (void) AmbaFS_FileWrite(ptr_void, size, nmemb, stream, &count);

    return count;
}

static inline uint64_t cvtask_ftell(CVTASK_FILE *stream)
{
    uint32_t rval;
    uint64_t fpos;
    UINT64 fpos_64 = 0UL;

    rval = AmbaFS_FileTell(stream, &fpos_64);
    fpos = (uint64_t)fpos_64;
    AmbaMisra_TouchUnused(&rval);
    return fpos;
}

static inline uint32_t cvtask_fseek(CVTASK_FILE *stream, int64_t offset, int32_t whence)
{
    return AmbaFS_FileSeek(stream, offset, whence);
}

/******************************************************************************/
extern void logwriter_fclose(void);
extern uint32_t ambacv_get_cpumap(void);

/******************************************************************************/
/*                                   LINUX BUILD                              */
/******************************************************************************/
#else

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

extern void* ambacv_p2v(uint64_t pa);
extern uint64_t ambacv_v2p(void *va);
extern uint64_t ambacv_p2c(uint64_t pa);
extern uint64_t ambacv_c2p(uint64_t ca);
extern void* ambacv_c2v(uint64_t ca);
extern uint64_t ambacv_v2c(void *va);

#define AMBA_SCHDR_PRINT_MODULE_ID      0x1U
#define AMBA_CVTASK_PRINT_MODULE_ID     0x2U

#define cvtask_memcpy       memcpy
#define cvtask_memset       memset
#define cvtask_strncmp      strncmp
#define cvtask_strcmp       strcmp
#define cvtask_strlen       strlen
#define cvtask_strtoul      strtoul
#define cvtask_strncpy      strncpy
#define cvtask_strcpy       strcpy
#define cvtask_strcat       strcat

#define CVTASK_FILE         FILE
#define CVTASK_SEEK_CUR     SEEK_CUR
#define CVTASK_SEEK_SET     SEEK_SET
#define CVTASK_SEEK_END     SEEK_END

#define cvtask_fopen        fopen
#define cvtask_fclose       fclose
#define cvtask_fread        fread
#define cvtask_fwrite       fwrite
#define cvtask_ftell        ftell
#define cvtask_fseek        fseek

extern void console_printS5(
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

extern void console_printU5(
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

extern void module_printS5(
    uint16_t module,
    const char *fmt,
    const char *argv0,
    const char *argv1,
    const char *argv2,
    const char *argv3,
    const char *argv4);

extern void module_printU5(
    uint16_t module,
    const char *fmt,
    uint64_t argv0,
    uint64_t argv1,
    uint64_t argv2,
    uint64_t argv3,
    uint64_t argv4);

#endif

#endif  /* !CVTASK_OSSRV_H_FILE */

