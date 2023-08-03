/*
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
*/

#ifndef DATATYPE_H
#define DATATYPE_H

#ifndef VARIANT_a //IK QNX compiled symbol.
//typedef __SIZE_TYPE__ size_t;
#else
#include <stddef.h>
#endif

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))
//#ifndef NULL
//#define NULL ((void *)0)
//#endif
#include <stddef.h>
#endif

typedef signed char int8;
typedef unsigned char uint8;
typedef short  int16;
typedef unsigned short uint16;
typedef int  int32;
typedef unsigned   uint32;
typedef long long  int64;
typedef unsigned long long   uint64;
typedef long intptr;
typedef unsigned long uintptr;

typedef float float32;
typedef double double64;

#define AMBA_OK             0x00000000UL /* General OK */
#define AMBA_SSP_ERR_BASE   0x01000000UL
#define AMBA_IK_ERR_BASE    (AMBA_SSP_ERR_BASE + 0x00100000UL) /* 0x01000000 to 0x01ffffff: Reserved for SSP */

#define REGULAR_EXECUTE 0UL
#define STAGE_1_FOR_EIS_EXECUTE 1UL
#define STAGE_2_FOR_EIS_EXECUTE 2UL
#define MAX_EIS_DELAY_FRAMES 8UL

// success
#ifndef IK_OK
#define IK_OK AMBA_OK
#endif
// error 0xFFFF0
#ifndef IK_ERR_0000
#define IK_ERR_0000 (AMBA_IK_ERR_BASE)                  // General error
#endif
#ifndef IK_ERR_0001
#define IK_ERR_0001 (IK_ERR_0000 | 0x00000001UL) // IK Arch un-init error
#endif
#ifndef IK_ERR_0002
#define IK_ERR_0002 (IK_ERR_0000 | 0x00000002UL) // Specified ContextId un-init error
#endif
#ifndef IK_ERR_0003
#define IK_ERR_0003 (IK_ERR_0000 | 0x00000003UL) // Invalid ContextId error
#endif
#ifndef IK_ERR_0004
#define IK_ERR_0004 (IK_ERR_0000 | 0x00000004UL) // Invalid api for specified context ability error
#endif
#ifndef IK_ERR_0005
#define IK_ERR_0005 (IK_ERR_0000 | 0x00000005UL) // Null pointer error
#endif
#ifndef IK_ERR_0006
#define IK_ERR_0006 (IK_ERR_0000 | 0x00000006UL) // Invalid buffer alignment error. Size or address are not aligned to design document specified value
#endif
#ifndef IK_ERR_0007
#define IK_ERR_0007 (IK_ERR_0000 | 0x00000007UL) // Invalid buffer size too small error
#endif
#ifndef IK_ERR_0008
#define IK_ERR_0008 (IK_ERR_0000 | 0x00000008UL) // One or more parameters are out of valid range error
#endif
#ifndef IK_ERR_0009
#define IK_ERR_0009 (IK_ERR_0000 | 0x00000009UL) // Invalid ik_id
#endif

// error 0xFFFF1
#ifndef IK_ERR_0100
#define IK_ERR_0100 (IK_ERR_0000 | 0x00000100UL) // One or more mandatory filters are not set
#endif
#ifndef IK_ERR_0101
#define IK_ERR_0101 (IK_ERR_0000 | 0x00000101UL) // Invalid window geometry error
#endif
#ifndef IK_ERR_0102
#define IK_ERR_0102 (IK_ERR_0000 | 0x00000102UL) // Invalid CC table version
#endif
#ifndef IK_ERR_0103
#define IK_ERR_0103 (IK_ERR_0000 | 0x00000103UL) // Invalid AAA setting
#endif
#ifndef IK_ERR_0104
#define IK_ERR_0104 (IK_ERR_0000 | 0x00000104UL) // Wrong calculation on warp
#endif
#ifndef IK_ERR_0105
#define IK_ERR_0105 (IK_ERR_0000 | 0x00000105UL) // Wrong calculation on ca
#endif
#ifndef IK_ERR_0106
#define IK_ERR_0106 (IK_ERR_0000 | 0x00000106UL) // Wrong calculation on sbp
#endif
#ifndef IK_ERR_0107
#define IK_ERR_0107 (IK_ERR_0000 | 0x00000107UL) // Wrong calculation on vig
#endif
#ifndef IK_ERR_0108
#define IK_ERR_0108 (IK_ERR_0000 | 0x00000108UL) // Invalid anti-aliasing setting
#endif
#ifndef IK_ERR_0109
#define IK_ERR_0109 (IK_ERR_0000 | 0x00000109UL) // Invalid wb gain setting
#endif
#ifndef IK_ERR_010A
#define IK_ERR_010A (IK_ERR_0000 | 0x0000010AUL) // Invalid ce setting
#endif

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))
#define INTERNAL_EARLY_TEST_ENV 1
#define SUPPORT_FUSION 1
#define PRE_NN_PROCESS 1
#else
#define INTERNAL_EARLY_TEST_ENV 0
#define SUPPORT_FUSION 0
#define PRE_NN_PROCESS 0
#endif

#endif //DATATYPE_H

