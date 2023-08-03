/**
 *  @file AmbaRTOSWrapper.h
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

#ifndef AMBARTOSWRAPPER_H
#define AMBARTOSWRAPPER_H
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BUILD_CV_THREADX

#include "AmbaTypes.h"


#define AmbaMisra_TouchUnused(x)
#define AmbaMisra_TypeCast32(a, b) memcpy((void *)a, (void *)b, 4)
#define AmbaMisra_TypeCast64(a, b) memcpy((void *)a, (void *)b, 8)
#define AmbaUtility_StringCopy(a, b, c) strncpy(a, c, b)
#define AmbaCache_DataClean //ambacv_cache_clean
#define AmbaCache_DataInvalidate //ambacv_cache_invalidate
#define FILE_WRAP       FILE
#define SEEK_CUR_WRAP       SEEK_CUR
#define SEEK_SET_WRAP       SEEK_SET
#define SEEK_END_WRAP       SEEK_END
#define fopen_wrap fopen
#define fread_wrap fread
#define fwrite_wrap fwrite
#define fclose_wrap fclose
#define ftell_wrap ftell
#define fseek_wrap fseek

#define AMBAOD_MAX_BBX_NUM  200


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

#endif
#ifdef __cplusplus
}
#endif
#endif /* AMBARTOSWRAPPER_H */
