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

#ifndef ARCH_OSAL_H
#define ARCH_OSAL_H
#include "ik_data_type.h"

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

typedef struct {
    uint8 resered[28];
} OS_MUTEX_TYPE;

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))

#include "string.h" //memcpy
#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaMMU_Def.h" //AmbaMmuPhysToVirt
#include "AmbaKAL.h" // mutex
#include "AmbaCache.h" // AmbaCache_Clean
#include "math.h" //sqrt

#define OS_MUTEX_TYPE AMBA_KAL_MUTEX_t

#else

#include "string.h" //memcpy
#include "AmbaTypes.h"
#include "AmbaPrint.h"
#include "AmbaMMU_Def.h" //AmbaMmuPhysToVirt
#include "AmbaKAL.h" // mutex
#include "AmbaCache.h" // AmbaCache_Clean
#include "math.h" //sqrt

#define OS_MUTEX_TYPE AMBA_KAL_MUTEX_t

#endif


typedef void* (*ik_system_memcpy_t)(void *p_str1, const void *p_str2, size_t n);
typedef void* (*ik_system_memset_t)(void *p_str, int32 c, size_t n);
typedef int32 (*ik_system_memcmp_t)(const void *p_str1, const void *p_str2, size_t n);
typedef int32 (*ik_system_create_mutex_t)(OS_MUTEX_TYPE *p_mutex);
typedef int32 (*ik_system_destroy_mutex_t)(OS_MUTEX_TYPE *p_mutex);
typedef int32 (*ik_system_lock_mutex_t)(OS_MUTEX_TYPE *p_mutex, uint32 timeout);
typedef int32 (*ik_system_unlock_mutex_t)(OS_MUTEX_TYPE *p_mutex);
typedef int32 (*ik_system_print_t)(const char *p_format, ...);
typedef int32 (*ik_system_clean_cache_t)(const void *p_str, size_t n);
typedef void* (*ik_system_convert_virtual_to_physical_t)(void* addr);
typedef void* (*ik_system_convert_physical_to_virtual_t)(void* addr);
typedef double64 (*ik_system_sqrt_t)(double64 x);


#endif
