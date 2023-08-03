#ifndef _LIBWRAPC_H
#define _LIBWRAPC_H
/**
 *  @file wrapc.h
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details Header file of wrapping C API.
 *
 */

#include "AmbaUtility.h"
#include "AmbaIOUtility.h"
#include "AmbaWrap.h"

#if defined(CONFIG_CC_USESTD)
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//strnlen is not always available in string.h
#undef strnlen
size_t wrapc_strnlen(const char *str, size_t maxlen);
#define strnlen wrapc_strnlen
#else  /* CONFIG_CC_USESTD */
/* Types for `void *' pointers.  */
#if defined(CONFIG_ARM64)
#if !defined(__intptr_t_defined) && !defined(intptr_t_defined)
typedef long int                intptr_t;
#define __intptr_t_defined
#define intptr_t_defined
#endif
typedef unsigned long int       uintptr_t;
#else
#if !defined(__intptr_t_defined) && !defined(intptr_t_defined)
typedef int                     intptr_t;
#define __intptr_t_defined
#define intptr_t_defined
#endif
typedef unsigned int            uintptr_t;
#endif

#ifdef memcpy
#undef memcpy
#endif

#ifdef memset
#undef memset
#endif

#ifndef size_t
#define size_t        UINT32
#endif

extern unsigned long int StdWrap_strtoul(const char *nptr, char **endptr, int base);
#define strtoul StdWrap_strtoul

extern int StdWrap_strncmp(const char *s1, const char *s2i, long unsigned int n);
#define strncmp StdWrap_strncmp

size_t wrapc_strlen(const char *str);
#define strlen wrapc_strlen

size_t wrapc_strnlen(const char *str, size_t maxlen);
#define strnlen wrapc_strnlen

char *wrapc_strcat(char *dest, const char *src);
#define strcat wrapc_strcat

char *wrapc_strcpy(char *dest, const char *src);
#define strcpy wrapc_strcpy

int wrapc_strcmp(const char *str1, const char *str2);
#define strcmp wrapc_strcmp

char *wrapc_strchr(const char *str, int c);
#define strchr wrapc_strchr

char *wrapc_strrchr(const char *s, int c);
#define strrchr wrapc_strrchr

void *wrapc_memcpy(void *str1, const void *str2, size_t n);
#define memcpy wrapc_memcpy

void *wrapc_memset(void *str, int c, size_t n);
#define memset wrapc_memset

int wrapc_memcmp(const void *str1, const void *str2, size_t n);
#define memcmp wrapc_memcmp

void *wrapc_memmove(void *str1, const void *str2, size_t n);
#define memmove wrapc_memmove

void *wrapc_memchr(const void *str, int c, size_t n);
#define memchr wrapc_memchr

int abs(int var);
int atoi(char *str);
#endif /* CONFIG_CC_USESTD */
#endif /* _LIBWRAPC_H */
