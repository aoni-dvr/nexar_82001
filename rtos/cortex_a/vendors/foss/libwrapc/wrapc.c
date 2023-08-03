/**
 *  @file wrapc.c
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
 *  @details Wrapping C API.
 *
 */

#include "libwrapc.h"

#if !defined(CONFIG_CC_USESTD)
int abs(int var)
{
    if ( var < 0)
    var = -var;
    return var;
}

int atoi(char *str)
{
    int res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    // return result.
    return res;
}

size_t wrapc_strlen(const char *str)
{
    size_t len;
    len = IO_UtilityStringLength(str);
    return len;
}

size_t wrapc_strnlen(const char *str, size_t maxlen)
{
    size_t len;
    len = IO_UtilityStringLength(str);
    return (maxlen <= len ? len : maxlen);
}

char *wrapc_strcat(char *dest, const char *src)
{
    UINT32 len1,len2;

    len1 = IO_UtilityStringLength(src);
    len2 = IO_UtilityStringLength(dest);

    IO_UtilityStringAppend(dest, len1+len2+1, src);
    return dest;
}

char *wrapc_strcpy(char *dest, const char *src)
{
    UINT32 len;

    len = IO_UtilityStringLength(src);
    if(src[len] == '\0')
       len++;
    IO_UtilityStringCopy(dest, len, src);
    return dest;
}

int wrapc_strcmp(const char *str1, const char *str2)
{
    UINT32 len1,len2;

    len1 = IO_UtilityStringLength(str1);
    len2 = IO_UtilityStringLength(str2);
    if(len1 > len2)
        return IO_UtilityStringCompare(str1, str2,len2);
    else
        return IO_UtilityStringCompare(str1, str2,len1);
}

char *wrapc_strchr(const char *str, int c)
{
    while (*str != (char)c)
        if (!*str++)
            return 0;
    return (char *)str;
}

char *wrapc_strrchr(const char *str, int c)
{
    int i;

    if (str)
        for (i = strlen (str); i >= 0; i--)
            if (str[i] == c)
                return ((char *)(&str[i]));

    return NULL;
}

void *wrapc_memcpy(void *str1, const void *str2, size_t n)
{
    AmbaWrap_memcpy(str1, str2, n);
    return str1;
}

void *wrapc_memset(void *str, int c, size_t n)
{
    AmbaWrap_memset(str, c, n);
    return str;
}

int wrapc_memcmp(const void *str1, const void *str2, size_t n)
{
    int v;

    AmbaWrap_memcmp(str1, str2, n,(void *) &v);
    return v;
}

void *wrapc_memmove(void *str1, const void *str2, size_t n)
{
    signed char operation;
    size_t end;
    size_t current;

    if(str1 != str2) {
        if(str1 < str2) {
            operation = 1;
            current = 0;
            end = n;
        } else {
            operation = -1;
            current = n - 1;
            end = -1;
        }

        for( ; current != end; current += operation) {
            *(((unsigned char*)str1) + current) = *(((unsigned char*)str2) + current);
        }
    }
    return str1;
}

void *wrapc_memchr(const void *str, int c, size_t n)
{
    unsigned char *p = (unsigned char*)str;
    while( n-- )
        if( *p != (unsigned char)c )
            p++;
        else
            return p;
    return 0;
}
#endif
