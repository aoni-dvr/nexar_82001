/**
 *  @file AmbaTypes.h
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
 *  @details Data type definitions
 *
 */

#ifndef AMBA_TYPES_H
#define AMBA_TYPES_H

#ifndef _STDDEF_H
/* To obtain macro NULL */
/* Could also be stdio.h, stdlib.h and others in hosted environments */
#include <stddef.h>
#endif

#ifndef __ASM__
#include <stdint.h>

typedef int8_t    INT8;           /* 8 bits, [-128, 127] */
typedef int16_t   INT16;          /* 16 bits */
typedef int32_t   INT32;          /* 32 bits */
typedef int64_t   INT64;          /* 64 bits */

typedef uint8_t   UINT8;          /* 8 bits, [0, 255] */
typedef uint16_t  UINT16;         /* 16 bits */
typedef uint32_t  UINT32;         /* 32 bits */
typedef uint64_t  UINT64;         /* 64 bits */

typedef uint16_t  WCHAR;          /* 16 bits */
typedef uint32_t  SIZE_t;

typedef float     FLOAT;
typedef double    DOUBLE;
typedef int32_t   LONG;
typedef uint32_t  ULONG;


#endif  /* !__ASM__ */

#endif  /* AMBA_TYPES_H */
