/**
 *  @file AmbaCWrap.h
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

#ifndef AMBACWRAP_H
#define AMBACWRAP_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

#if 1 //def CONFIG_CC_USESTD
#include <stddef.h>

#else
#ifndef NULL
#ifndef __cplusplus
#define NULL ((void *)0)
#else
#define NULL 0
#endif /* C++ */
#endif

#ifndef size_t
#define size_t __SIZE_TYPE__
#endif

#ifndef uint32_t
#define uint32_t __UINT32_TYPE__
#endif
#endif // CONFIG_CC_USESTD

void *AmbaCWrap_memcpy(void *dest, const void *src, size_t n);
void *AmbaCWrap_memset(void *s, int c, size_t n);
int AmbaCWrap_memcmp(const void *s1, const void *s2, size_t n);
void *AmbaCWrap_memmove(void *dest, const void *src, size_t n);

size_t AmbaCWrap_strlen(const char *s);
char *AmbaCWrap_strncpy(char *dest, const char *src, size_t n);
char *AmbaCWrap_strstr(const char *haystack, const char *needle);

int AmbaCWrap_strncmp(const char *s1, const char *s2, size_t n);
char *AmbaCWrap_strncat(char *dest, const char *src, size_t n);
int AmbaCWrap_strncasecmp(const char *s1, const char *s2, size_t n);

int AmbaCWrap_snprintf(char *str, size_t size, const char *format, ...);
int AmbaCWrap_printf(const char *format, ...);



void *myMalloc(size_t n, void* heap, int type);
void myFree(void *p, void* heap, int type);
void *myRealloc(void *p, size_t n, void* heap, int type);

int AmbaCWrap_toupper(int c);
int AmbaCWrap_tolower(int c);


#endif // __ASSEMBLY__

#ifdef __cplusplus
}
#endif
#endif // AMBACWRAP_H

