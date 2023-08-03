/**
 *  @file AmbaCWrap.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details
 *      Sample codes to wrap standard C APIs neeed by wolfSSL.
 */

#include "AmbaCWrap.h"

#include <AmbaKAL.h>
#include <AmbaWrap.h>

void *AmbaCWrap_memcpy(void *dest, const void *src, size_t n)
{
    (void)AmbaWrap_memcpy(dest, src, n);

    return dest;
}

void *AmbaCWrap_memset(void *s, int c, size_t n)
{
    (void)AmbaWrap_memset(s, c, n);

    return s;
}

int AmbaCWrap_memcmp(const void *s1, const void *s2, size_t n)
{
    UINT32 err;
    INT32 v;

    err = AmbaWrap_memcmp(s1, s2, n, &v);
    if (err == 0u) {
        return v;
    } else {
        return 0;
    }
}

void *AmbaCWrap_memmove(void *dest, const void *src, size_t n)
{
    (void)AmbaWrap_memmove(dest, src, n);

    return dest;
}


#if 0
#include <string.h>
#else
size_t strlen(const char *s);
char *strncpy(char *dest, const char *src, size_t n);
char *strstr(const char *haystack, const char *needle);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncat(char *dest, const char *src, size_t n);
int strncasecmp(const char *s1, const char *s2, size_t n);
#endif

size_t AmbaCWrap_strlen(const char *s)
{
    return strlen(s);
}

char *AmbaCWrap_strncpy(char *dest, const char *src, size_t n)
{
    return strncpy(dest, src, n);
}

char *AmbaCWrap_strstr(const char *haystack, const char *needle)
{
    return strstr(haystack, needle);
}


int AmbaCWrap_strncmp(const char *s1, const char *s2, size_t n)
{
    return strncmp(s1, s2, n);
}

char *AmbaCWrap_strncat(char *dest, const char *src, size_t n)
{
    return strncat(dest, src, n);
}

int AmbaCWrap_strncasecmp(const char *s1, const char *s2, size_t n)
{
    return strncasecmp(s1, s2, n);
}


#if 0
int AmbaCWrap_snprintf(char *str, size_t size, const char *format, ...)
{
    (void)str;
    (void)size;
    (void)format;

    return 0;
}

int AmbaCWrap_printf(const char *format, ...)
{
    (void)format;

    return 0;
}
#endif

unsigned long my_time(unsigned long* timer)
{
    *timer = 0;

    return 0;
}

#if 0
#include <stdlib.h>
#else
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
#endif

void *myMalloc(size_t n, void* heap, int type)
{
    (void)n;
    (void)heap;
    (void)type;

    return malloc(n);
}

void myFree(void *p, void* heap, int type)
{
    (void)p;
    (void)heap;
    (void)type;

    free(p);
}

void *myRealloc(void *p, size_t n, void* heap, int type)
{
    (void)p;
    (void)n;
    (void)heap;
    (void)type;

    return realloc(p, n);
}


int AmbaCWrap_toupper(int c)
{
    if ((c >= 'a') && (c <= 'z')) {
        return c - 'a' + 'A';
    } else {
        return c;
    }
}

int AmbaCWrap_tolower(int c)
{
    if ((c >= 'A') && (c <= 'Z')) {
        return c - 'A' + 'a';
    } else {
        return c;
    }
}

unsigned int my_rng_seed_gen(void)
{
    static UINT32 s = 0u;
    UINT32 t = 0;
    UINT32 v = 0;

    (void)AmbaKAL_GetSysTickCount(&t);
    if (s == 0u) {
        (void)AmbaWrap_srand(t);
    }

    (void)AmbaWrap_rand(&v);

    s = t ^ v;
    (void)AmbaWrap_srand(s);

    return v;
}


double current_time(int reset)
{
    UINT32 err;
    UINT32 v;

    (void)reset;

    err = AmbaKAL_GetSysTickCount(&v);
    if (err == KAL_ERR_NONE) {
        return ((double)v / (double)TX_TIMER_TICKS_PER_SECOND);
    } else {
        return 0.;
    }
}

