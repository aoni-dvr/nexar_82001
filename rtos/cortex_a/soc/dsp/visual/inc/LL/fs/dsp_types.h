/**
*  @file dsp_types.h
*
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
*  @details data type header for CmdMsg
*
*/

#ifndef DSP_TYPES_H
#define DSP_TYPES_H

#define SUPPORT_DSP_MSG_CRC

#include "AmbaTypes.h"
#if defined (CONFIG_THREADX)
#ifndef __ASM__
#if !(defined(CONFIG_CC_USESTD) && defined(VCAST_AMBA))
#include "AmbaFS.h" // for uint64_t/uint32_t/uint16_t/uint8_t/int64_t/int32_t/int16_t/int8_t
//typedef UINT64  uint64_t;
//typedef UINT32  uint32_t;
//typedef UINT16  uint16_t;
//typedef UINT8   uint8_t;
//typedef INT64   int64_t;
//typedef INT32   int32_t;
//typedef INT16   int16_t;
//typedef INT8    int8_t;
#else
#include <stdint.h>
#endif
#else
#ifndef __ASSEMBLER__
#ifndef GENERAL_C
typedef unsigned long long uint64_t;
typedef long long      int64_t;
#endif
typedef int (BOOL);
#else
#include <stdint.h>
#endif
#endif

//#ifndef NULL
//#define NULL    (0)
//#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif
#elif defined (CONFIG_LINUX)
#include <linux/types.h>
//typedef unsigned long long  uint64_t;
typedef unsigned int        uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;
//typedef long long           int64_t;
typedef int                 int32_t;
typedef short               int16_t;
#else
#include <stdint.h>
#endif

#endif //DSP_TYPES_H

