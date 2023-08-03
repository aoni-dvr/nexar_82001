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

#ifndef AMBINT_H_FILE
#define AMBINT_H_FILE

#if defined(CONFIG_LINUX)
#include <stdint.h>
#elif !(defined(CONFIG_CC_USESTD) && defined(VCAST_AMBA))
#if defined(ORCVIS)
//For VISORC
typedef unsigned long long      uint64_t;
typedef long long               int64_t;
typedef unsigned int            uint32_t;
typedef int                     int32_t;
typedef unsigned short          uint16_t;
typedef short                   int16_t;
typedef unsigned char           uint8_t;
typedef signed char             int8_t;

#elif defined(CONFIG_THREADX)
//For ThreadX
// AmbaFS.h prfile will provide those typedef in header file.
// So, there is no need to typedef here.
#include "AmbaFS.h"
#else
// For linux
#ifdef __KERNEL__
#include <linux/types.h>
typedef unsigned int                    ptr32_t;
#else /* !__KERNEL__ */
#include <stdint.h>
#endif /* ?__KERNEL__ */

#endif

#else // for !(defined(CONFIG_CC_USESTD) && defined(VCAST_AMBA))

// For Vcast and using CONFIG_CC_USESTD
#include <stdint.h>

#endif

typedef uint64_t           ptr64_t;     /* 64-bit address pointer */
typedef uint32_t           ptr32_t;     /* 32-bit address pointer */
typedef uint32_t           rptr32_t;    /* relative 32-bit address pointer */

typedef struct
{
  uint8_t segofs_msb8;    /* [39:32] of 40-bit offset */
  uint8_t rsvd0;
  uint8_t rsvd1;
  uint8_t g_seg_id;       /* global segment id */
} u64_globaladdr_upper_t;

typedef struct
{
  uint32_t                gaddr_lo;
  u64_globaladdr_upper_t  gaddr_hi;
} gaddr64_t;

#define u64gau_t          u64_globaladdr_upper_t      // Shortcut name for structure u64_globaladdr_upper_t

#endif /* !AMBINT_H_FILE */

