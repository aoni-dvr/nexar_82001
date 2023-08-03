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

#ifndef ARCH_SYSTEM_API_H
#define ARCH_SYSTEM_API_H

//#include "AmbaDSP_ImgOSAL.h"
#include "ik_data_type.h"
#include "AmbaDSP_ImgIkcSpec.h"

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))
//#include "string.h" //memcpy
//#include "math.h" //sqrt

#include "AmbaTypes.h"
//#include "AmbaPrint.h" // PP dirty mark
//#include "AmbaMMU_Def.h" //AmbaMmuPhysToVirt
#include "AmbaMMU.h" //AmbaMmuPhysToVirt
#include "AmbaKAL.h" // mutex
#include "AmbaCache.h" // AmbaCache_Clean
//#include "AmbaWrap.h"
//#include "AmbaUtility.h"

#else
//#include "string.h" //memcpy
//#include "math.h" //sqrt

#include "AmbaTypes.h"
#include "AmbaPrint.h" // PP dirty mark
//#include "AmbaMMU_Def.h" //AmbaMmuPhysToVirt
#include "AmbaMMU.h" //AmbaMmuPhysToVirt
#include "AmbaKAL.h" // mutex
#include "AmbaCache.h" // AmbaCache_Clean
#include "AmbaWrap.h"
#include "AmbaUtility.h"

#endif

#define IK_VERSION_MAJOR 8200101UL
#define IK_VERSION_MINOR 10UL

//typedef struct {
//    uint8 resered[28];
//} OS_MUTEX_TYPE;
#define OS_MUTEX_TYPE IKC_OS_MUTEX_TYPE

typedef void* (*ik_system_memcpy_t)(void *p_str1, const void *p_str2, size_t n);
typedef void* (*ik_system_memset_t)(void *p_str, int32 c, size_t n);
typedef int32 (*ik_system_memcmp_t)(const void *p_str1, const void *p_str2, size_t n);
typedef int32 (*ik_system_create_mutex_t)(OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ik_system_destroy_mutex_t)(OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ik_system_lock_mutex_t)(OS_MUTEX_TYPE *p_mutex, uint32 timeout);
typedef int32 (*ik_system_unlock_mutex_t)(OS_MUTEX_TYPE *p_mutex);
typedef int32 (*ik_system_print_t)(const char *p_format, ...);
typedef int32 (*ik_system_clean_cache_t)(const void *p_str, size_t n);
typedef void* (*ik_system_convert_virtual_to_physical_t)(void* addr);
typedef void* (*ik_system_convert_physical_to_virtual_t)(void* addr);
typedef double64 (*ik_system_sqrt_t)(double64 x);

#define DC_I    99999L   /*PRINT DONT CARE FOR INT*/
#define DC_U    99999UL /*PRINT DONT CARE FOR UINT*/
#define DC_S    NULL    /*PRINT DONT CARE FOR STRING*/

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

extern ik_system_memcpy_t ik_system_memcpy;
extern ik_system_memset_t ik_system_memset;
extern ik_system_memcmp_t ik_system_memcmp;
extern ik_system_create_mutex_t ik_system_create_mutex;
extern ik_system_destroy_mutex_t ik_system_destroy_mutex;
extern ik_system_lock_mutex_t ik_system_lock_mutex;
extern ik_system_unlock_mutex_t ik_system_unlock_mutex;
extern ik_system_convert_virtual_to_physical_t ik_system_virtual_to_physical;
extern ik_system_convert_physical_to_virtual_t ik_system_physical_to_virtual;
extern ik_system_print_t ik_system_print;
extern ik_system_clean_cache_t ik_system_clean_cache;
extern ik_system_sqrt_t ik_system_sqrt;

#define amba_ik_system_memcpy ik_system_memcpy
#define amba_ik_system_memset ik_system_memset
#define amba_ik_system_memcmp ik_system_memcmp
#define amba_ik_system_create_mutex ik_system_create_mutex
#define amba_ik_system_destroy_mutex ik_system_destroy_mutex
#define amba_ik_system_lock_mutex ik_system_lock_mutex
#define amba_ik_system_unlock_mutex ik_system_unlock_mutex
#define amba_ik_system_virt_to_phys ik_system_virtual_to_physical
#define amba_ik_system_phys_to_virt ik_system_physical_to_virtual
#define amba_ik_system_print_uint32_5(fmt, arg...) ik_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ik_system_print_int32_5(fmt, arg...) ik_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ik_system_print_str_5(fmt, arg...) ik_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ik_system_print_flush(...)
#define amba_ik_system_clean_cache ik_system_clean_cache

void img_arch_registe_system_memcpy(const ik_system_memcpy_t *p_system_memcpy);
void img_arch_registe_system_memset(const ik_system_memset_t *p_system_memset);
void img_arch_registe_system_memcmp(const ik_system_memcmp_t *p_system_memcmp);
void img_arch_registe_system_create_mutex(const ik_system_create_mutex_t *p_system_create_mutex);
void img_arch_registe_system_destroy_mutex(const ik_system_destroy_mutex_t *p_system_destroy_mutex);
void img_arch_registe_system_lock_mutex(const ik_system_lock_mutex_t *p_system_lock_mutex);
void img_arch_registe_system_unlock_mutex(const ik_system_unlock_mutex_t *p_system_unlock_mutex);
void img_arch_registe_system_print(const ik_system_print_t *p_system_print);
void img_arch_registe_system_clean_cache(const ik_system_clean_cache_t *p_system_clean_cache);
void img_arch_registe_system_virtual_to_physical_address(const ik_system_convert_virtual_to_physical_t *p_system_virtual_to_physical_address);
void img_arch_registe_system_physical_to_virtual_address(const ik_system_convert_physical_to_virtual_t *p_system_physical_to_virtual_address);
void img_arch_unregiste_system_memcpy(void);
void img_arch_unregiste_system_memset(void);
void img_arch_unregiste_system_memcmp(void);
void img_arch_unregiste_system_create_mutex(void);
void img_arch_unregiste_system_destroy_mutex(void);
void img_arch_unregiste_system_lock_mutex(void);
void img_arch_unregiste_system_unlock_mutex(void);
void img_arch_unregiste_system_print(void);
void img_arch_unregiste_system_clean_cache(void);
void img_arch_unregiste_system_virtual_to_physical_address(void);
void img_arch_unregiste_system_physical_to_virtual_address(void);
void img_arch_registe_system_sqrt(const ik_system_sqrt_t *p_system_sqrt);
void img_arch_unregiste_system_sqrt(void);
uint32 img_arch_check_system_control_callback_function(void);
void amba_ik_get_mutex(uint32 context_id, OS_MUTEX_TYPE **p_mutex);
#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))

//void ik_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
//void ik_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
//void ik_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);


#define OS_MUTEX_TYPE IKC_OS_MUTEX_TYPE
//rtos
//#define amba_ik_system_memcpy amba_ikc_system_memcpy //memcpy
void amba_ik_system_memcpy(void *pDst, const void *pSrc, SIZE_t num);
//#define amba_ik_system_memset amba_ikc_system_memset //memset
void amba_ik_system_memset(void *ptr, INT32 v, SIZE_t n);
#define amba_ik_system_memcmp amba_ikc_system_memcmp //memcmp
#define amba_ik_system_create_mutex amba_ikc_system_create_mutex
#define amba_ik_system_destroy_mutex amba_ikc_system_destroy_mutex
#define amba_ik_system_lock_mutex amba_ikc_system_lock_mutex
#define amba_ik_system_unlock_mutex amba_ikc_system_unlock_mutex
#define amba_ik_system_print_uint32_5 amba_ikc_system_print_uint32_5 // AmbaPrint_PrintUInt5
#define amba_ik_system_print_int32_5 amba_ikc_system_print_int32_5 // AmbaPrint_PrintInt5
#define amba_ik_system_print_str_5 amba_ikc_system_print_str_5 // AmbaPrint_PrintStr5
#define amba_ik_system_print_flush amba_ikc_system_print_flush

#define amba_ik_system_virt_to_phys amba_ikc_system_virt_to_phys
#define amba_ik_system_phys_to_virt amba_ikc_system_phys_to_virt
#define amba_ik_system_clean_cache amba_ikc_system_clean_cache

void amba_ik_get_mutex(uint32 context_id, OS_MUTEX_TYPE **p_mutex);

#else

//void ik_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
//void ik_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
//void ik_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);


#define OS_MUTEX_TYPE IKC_OS_MUTEX_TYPE
//rtos
//#define amba_ik_system_memcpy amba_ikc_system_memcpy //memcpy
void amba_ik_system_memcpy(void *pDst, const void *pSrc, SIZE_t num);
//#define amba_ik_system_memset amba_ikc_system_memset //memset
void amba_ik_system_memset(void *ptr, INT32 v, SIZE_t n);
#define amba_ik_system_memcmp amba_ikc_system_memcmp //memcmp
#define amba_ik_system_create_mutex amba_ikc_system_create_mutex
#define amba_ik_system_destroy_mutex amba_ikc_system_destroy_mutex
#define amba_ik_system_lock_mutex amba_ikc_system_lock_mutex
#define amba_ik_system_unlock_mutex amba_ikc_system_unlock_mutex
#define amba_ik_system_print_uint32_5 amba_ikc_system_print_uint32_5 // AmbaPrint_PrintUInt5
#define amba_ik_system_print_int32_5 amba_ikc_system_print_int32_5 // AmbaPrint_PrintInt5
#define amba_ik_system_print_str_5 amba_ikc_system_print_str_5 // AmbaPrint_PrintStr5
#define amba_ik_system_print_flush amba_ikc_system_print_flush

#define amba_ik_system_virt_to_phys amba_ikc_system_virt_to_phys
#define amba_ik_system_phys_to_virt amba_ikc_system_phys_to_virt
#define amba_ik_system_clean_cache amba_ikc_system_clean_cache

#endif

typedef struct {
    ik_system_memcpy_t system_memcpy;
    ik_system_memset_t system_memset;
    ik_system_memcmp_t system_memcmp;
    ik_system_create_mutex_t system_create_mutex;
    ik_system_destroy_mutex_t system_destroy_mutex;
    ik_system_lock_mutex_t system_lock_mutex;
    ik_system_unlock_mutex_t system_unlock_mutex;
    ik_system_print_t system_print;
    ik_system_clean_cache_t system_clean_cache;
    ik_system_convert_virtual_to_physical_t system_virtual_to_physical_address;
    ik_system_convert_physical_to_virtual_t system_physical_to_virtual_address;
    ik_system_sqrt_t system_sqrt;
    // FIXME: convert to virtual
} ik_system_api_t;

void amba_ik_get_mutex(uint32 context_id, OS_MUTEX_TYPE **p_mutex);
#endif
