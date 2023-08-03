/**
 *  @file AmbaWrap.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Header file of vendors/ambarella/wrapper
 *
 */

#ifndef AMBAWRAP_H
#define AMBAWRAP_H

#ifndef AMBA_TYPES_H
#include <AmbaTypes.h>
#endif
#ifndef AMBA_ERROR_CODE_H
#include <AmbaErrorCode.h>
#endif

/** Invalid argument or parameters. */
#define LIBWRAP_ERR_0000    (LIBWRAP_ERR_BASE)
#define LIBWRAP_ERR_INVAL   LIBWRAP_ERR_0000    //< Invalid argument */

#ifdef  __cplusplus
extern  "C" {
#endif

/*------------------------------------------------------------------------------
 * For libwrap_std.a
 */
/* Local implementation */
extern UINT32 AmbaWrap_memcpy(void *pDst, const void *pSrc, SIZE_t num);

/* libc.a */
extern UINT32 AmbaWrap_abs(INT32 n, void *pV);
extern UINT32 AmbaWrap_memcmp(const void *p1, const void *p2, SIZE_t n, void *pV);
extern UINT32 AmbaWrap_memmove(void *p1, const void *p2, SIZE_t n);
extern UINT32 AmbaWrap_memset(void *ptr, INT32 v, SIZE_t n);
extern UINT32 AmbaWrap_rand(void *pV);
extern UINT32 AmbaWrap_srand(UINT32 v);

/* libm.a */
extern UINT32 AmbaWrap_acos(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_asin(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_atan(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_atan2(DOUBLE y, DOUBLE x, void *pV);
extern UINT32 AmbaWrap_ceil(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_cos(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_exp(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_fabs(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_floor(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_fmod(DOUBLE x, DOUBLE y, void *pV);
extern UINT32 AmbaWrap_log(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_log2(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_log10(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_pow(DOUBLE base, DOUBLE exponent, void *pV);
extern UINT32 AmbaWrap_sin(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_sqrt(DOUBLE x, void *pV);
extern UINT32 AmbaWrap_tan(DOUBLE x, void *pV);

#ifdef  __cplusplus
}
#endif

#endif /* AMBAWRAP_H */

