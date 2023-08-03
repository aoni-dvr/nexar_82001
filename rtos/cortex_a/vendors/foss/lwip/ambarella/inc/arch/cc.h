/**
 *  @file cc.h
 *
 *  @copyright Copyright (c) 2018 Ambarella, Inc.
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
 *  @details Architecture environment, some compiler specific, some environment specific.
 *           Refer to doc/sys_arch.txt for detail.
 *
 */

#ifndef LWIP_AMBA_CC_H
#define LWIP_AMBA_CC_H

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */


#ifdef CONFIG_CC_USESTD
#define LWIP_NO_CTYPE_H     (0)
#define LWIP_NO_INTTYPES_H  (0)
#define LWIP_NO_LIMITS_H    (0)
#define LWIP_NO_STDDEF_H    (0)
#define LWIP_NO_STDINT_H    (0)
#define LWIP_NO_UNISTD_H    (0)
#else
#include <AmbaTypes.h>
#define LWIP_NO_CTYPE_H     (1)
#define LWIP_NO_INTTYPES_H  (1)
#define LWIP_NO_LIMITS_H    (1)
#define LWIP_NO_STDDEF_H    (1)
#define LWIP_NO_STDINT_H    (1)
#define LWIP_NO_UNISTD_H    (1)

// From internal definition of compiler.
#define INT_MAX __INT_MAX__
#endif


/* Define generic types used in lwIP */
#if LWIP_NO_STDINT_H
typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   int     u32_t;
typedef signed     int     s32_t;

typedef unsigned long mem_ptr_t;
#endif // LWIP_NO_STDINT_H

#ifndef CONFIG_CC_USESTD

#define MYLWIPDBG (0)

#if MYLWIPDBG
// Need to use original libc.a from toolchain for vsnprintf

#include <stdarg.h>
#include <io/AmbaUART.h>

extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);

inline static void myDebug(const char *fmt, ...)
{
#define SIZE_BUF (1024)
    u8_t buf[SIZE_BUF];
    va_list args;
    s32_t n;
    u32_t sz;

    va_start(args, fmt);
    n = vsnprintf((char *)buf, SIZE_BUF-2, (const char *)fmt, args);
    if (buf[n-1] == (u8_t)'\n') {
        buf[n++] = (u8_t)'\r';
    }
    buf[n++] = (u8_t)'\0';
    (void)AmbaUART_Write(0u, 0u, n, buf, &sz, 1u);
}
#endif // MYLWIPDBG
#endif // !CONFIG_CC_USESTD

/* prototypes for printf() and abort() */
/* Plaform specific diagnostic output */
#ifdef CONFIG_CC_USESTD
#include <stdio.h>
#include <stdlib.h>
#define LWIP_PLATFORM_DIAG(x)   do {printf x;} while(0)

#define LWIP_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                             x, __LINE__, __FILE__); fflush(NULL); abort();} while(0)
#else // !CONFIG_CC_USEDSTD
#if MYLWIPDBG
#define LWIP_PLATFORM_DIAG(x)   do { myDebug x; } while(0)
#define LWIP_PLATFORM_ASSERT(x) do { myDebug("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__); } while(0)
#else
#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)
#endif
#endif // CONFIG_CC_USEDSTD

#endif /* LWIP_AMBA_CC_H */


