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

#ifndef IKC_SPEC_H
#define IKC_SPEC_H
#include "ik_data_type.h"

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))

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


#ifdef IK_RELEASE
#define STATIC static
#define INLINE inline
#else
#define STATIC
#define INLINE
#endif
#ifndef MAX_CONTEXT_NUM
#define MAX_CONTEXT_NUM (16U)
#endif
#ifndef MAX_CONFIG_NUM
#define MAX_CONFIG_NUM (10U)
#endif
#define MAX_CONTAINER_IDSP_CMD_NUM (10U)
#define MAX_CONTAINER_VIN_CMD_NUM (10U)
#define MAX_VIN_NUM (4U)
#define IKC_MAX_HDR_EXPOSURE_NUM (3U)

#define DC_I    99999L   /*PRINT DONT CARE FOR INT*/
#define DC_U    99999UL /*PRINT DONT CARE FOR UINT*/
#define DC_S    NULL    /*PRINT DONT CARE FOR STRING*/

/***** ability definition *****/
/* PIPE  */
#ifndef AMBA_IKC_PIPE_VIDEO
#define AMBA_IKC_PIPE_VIDEO (0UL)
#endif
#ifndef AMBA_IKC_PIPE_STILL
#define AMBA_IKC_PIPE_STILL (1UL)
#endif
/* VIDEO */
#ifndef AMBA_IKC_VIDEO_LINEAR
#define AMBA_IKC_VIDEO_LINEAR (0UL)
#endif
#ifndef AMBA_IKC_VIDEO_LINEAR_CE
#define AMBA_IKC_VIDEO_LINEAR_CE (1UL)
#endif
#ifndef AMBA_IKC_VIDEO_HDR_EXPO_2
#define AMBA_IKC_VIDEO_HDR_EXPO_2 (2UL)
#endif
#ifndef AMBA_IKC_VIDEO_HDR_EXPO_3
#define AMBA_IKC_VIDEO_HDR_EXPO_3 (3UL)
#endif
#ifndef AMBA_IKC_VIDEO_LINEAR_MD
#define AMBA_IKC_VIDEO_LINEAR_MD (4UL)
#endif
#ifndef AMBA_IKC_VIDEO_LINEAR_CE_MD
#define AMBA_IKC_VIDEO_LINEAR_CE_MD (5UL)
#endif
#ifndef AMBA_IKC_VIDEO_HDR_EXPO_2_MD
#define AMBA_IKC_VIDEO_HDR_EXPO_2_MD (6UL)
#endif
#ifndef AMBA_IKC_VIDEO_HDR_EXPO_3_MD
#define AMBA_IKC_VIDEO_HDR_EXPO_3_MD (7UL)
#endif
#ifndef AMBA_IKC_VIDEO_Y2Y
#define AMBA_IKC_VIDEO_Y2Y (8UL)
#endif
#ifndef AMBA_IKC_VIDEO_Y2Y_MD
#define AMBA_IKC_VIDEO_Y2Y_MD (9UL)
#endif
#ifndef AMBA_IKC_VIDEO_FUSION_CE_MD
#define AMBA_IKC_VIDEO_FUSION_CE_MD (10UL)
#endif
#ifndef AMBA_IKC_VIDEO_FUSION_CE
#define AMBA_IKC_VIDEO_FUSION_CE (11UL)
#endif
#ifndef AMBA_IKC_VIDEO_Y2Y_MIPI
#define AMBA_IKC_VIDEO_Y2Y_MIPI (12UL)
#endif
#ifndef AMBA_IKC_VIDEO_PRE_NN_PROCESS
#define AMBA_IKC_VIDEO_PRE_NN_PROCESS (13UL)
#endif
#ifndef AMBA_IKC_VIDEO_MAX
#define AMBA_IKC_VIDEO_MAX (14UL)
#endif

#ifndef AMBA_IKC_STILL_BASE
#define AMBA_IKC_STILL_BASE (32UL)
#endif
#ifndef AMBA_IKC_STILL_LISO
#define AMBA_IKC_STILL_LISO (AMBA_IKC_STILL_BASE + 1UL)
#endif
#ifndef AMBA_IKC_STILL_HISO
#define AMBA_IKC_STILL_HISO (AMBA_IKC_STILL_BASE + 2UL)
#endif
#ifndef AMBA_IKC_STILL_MAX
#define AMBA_IKC_STILL_MAX (AMBA_IKC_STILL_BASE + 3UL)
#endif

#ifndef AMBA_IKC_ARCH_DEFINE
#define AMBA_IKC_ARCH_DEFINE
#define AMBA_IKC_ARCH_HARD_INIT 0UL
#define AMBA_IKC_ARCH_SOFT_INIT 1UL
#endif

typedef struct {
    uint32 iso_config_common_update : 1 ;
    uint32 iso_config_color_update : 1 ;
    uint32 iso_config_mctf_update : 1 ;
    uint32 iso_config_step1_update : 1 ;
    uint32 iso_config_aaa_update : 1 ; // AAA setup update
    uint32 iso_config_extra_update : 1 ;
    uint32 iso_config_motion_update : 1 ;
    uint32 iso_config_exp_update : 1 ;
    uint32 reserved : 24;
} ikc_iso_config_update;

typedef enum {
    amba_ikc_func_mode_fv = 0x00U,
    amba_ikc_func_mode_qv = 0x01U,
    amba_ikc_func_mode_piv = 0X02U,
    amba_ikc_func_mode_r2r = 0x04U,
    amba_ikc_func_mode_vhdr = 0x08U,
    amba_ikc_func_mode_y2y = 0x10U,
    amba_ikc_func_mode_md = 0x20U,
    amba_ikc_func_mode_y2y_bypass,
} amba_ikc_func_mode_t;

typedef struct {
    uint8 pipe;
    uint32 still_pipe;
    uint32 video_pipe;
} ikc_ability_t;

typedef void* (*ikc_system_memcpy_t)(void *p_str1, const void *p_str2, size_t n);
typedef void* (*ikc_system_memset_t)(void *p_str, int32 c, size_t n);
typedef int32 (*ikc_system_memcmp_t)(const void *p_str1, const void *p_str2, size_t n);
typedef int32 (*ikc_system_print_t)(const char *p_format, ...);
typedef int32 (*ikc_system_clean_cache_t)(const void *p_str, size_t n);
typedef void* (*ikc_system_convert_virtual_to_physical_t)(void* addr);
typedef void* (*ikc_system_convert_physical_to_virtual_t)(void* addr);
typedef double64 (*ikc_system_sqrt_t)(double64 x);

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

typedef struct {
    uint8 resered[28];
} IKC_OS_MUTEX_TYPE;

typedef int32 (*ikc_system_create_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ikc_system_destroy_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ikc_system_lock_mutex_t)(uint32 context_id, uint32 timeout);
typedef int32 (*ikc_system_unlock_mutex_t)(uint32 context_id);
extern ikc_system_memcpy_t ikc_system_memcpy;
extern ikc_system_memset_t ikc_system_memset;
extern ikc_system_memcmp_t ikc_system_memcmp;
extern ikc_system_create_mutex_t ikc_system_create_mutex;
extern ikc_system_destroy_mutex_t ikc_system_destroy_mutex;
extern ikc_system_lock_mutex_t ikc_system_lock_mutex;
extern ikc_system_unlock_mutex_t ikc_system_unlock_mutex;
extern ikc_system_convert_virtual_to_physical_t ikc_system_virtual_to_physical;
extern ikc_system_convert_physical_to_virtual_t ikc_system_physical_to_virtual;
extern ikc_system_print_t ikc_system_print;
extern ikc_system_clean_cache_t ikc_system_clean_cache;
extern ikc_system_sqrt_t ikc_system_sqrt;

#define amba_ikc_system_memcpy ikc_system_memcpy
#define amba_ikc_system_memset ikc_system_memset
#define amba_ikc_system_memcmp ikc_system_memcmp
#define amba_ikc_system_create_mutex ikc_system_create_mutex
#define amba_ikc_system_destroy_mutex ikc_system_destroy_mutex
#define amba_ikc_system_lock_mutex ikc_system_lock_mutex
#define amba_ikc_system_unlock_mutex ikc_system_unlock_mutex
#define amba_ikc_system_virt_to_phys ikc_system_virtual_to_physical
#define amba_ikc_system_phys_to_virt ikc_system_physical_to_virtual
#define amba_ikc_system_print_uint32_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_int32_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_str_5(fmt, arg...) ikc_system_print(fmt"\n", ##arg)//(...) 0//AmbaPrint
#define amba_ikc_system_print_flush(...)
#define amba_ikc_system_clean_cache ikc_system_clean_cache

typedef struct {
    ikc_system_memcpy_t system_memcpy;
    ikc_system_memset_t system_memset;
    ikc_system_memcmp_t system_memcmp;
    ikc_system_print_t system_print;
    ikc_system_create_mutex_t system_create_mutex;
    ikc_system_destroy_mutex_t system_destroy_mutex;
    ikc_system_lock_mutex_t system_lock_mutex;
    ikc_system_unlock_mutex_t system_unlock_mutex;
    ikc_system_convert_virtual_to_physical_t system_virtual_to_physical_address;
    ikc_system_convert_physical_to_virtual_t system_physical_to_virtual_address;
    ikc_system_clean_cache_t system_clean_cache;
    ikc_system_sqrt_t system_sqrt;
} ikc_system_api_t;


uint32 ikc_import_system_callback_func(const ikc_system_api_t *p_sys_api);

void ikc_registe_system_memcpy(const ikc_system_memcpy_t *p_system_memcpy);
void ikc_registe_system_memset(const ikc_system_memset_t *p_system_memset);
void ikc_registe_system_memcmp(const ikc_system_memcmp_t *p_system_memcmp);
void ikc_registe_system_create_mutex(const ikc_system_create_mutex_t *p_system_create_mutex);
void ikc_registe_system_destroy_mutex(const ikc_system_destroy_mutex_t *p_system_destroy_mutex);
void ikc_registe_system_lock_mutex(const ikc_system_lock_mutex_t *p_system_lock_mutex);
void ikc_registe_system_unlock_mutex(const ikc_system_unlock_mutex_t *p_system_unlock_mutex);
void ikc_registe_system_print(const ikc_system_print_t *p_system_print);
void ikc_registe_system_clean_cache(const ikc_system_clean_cache_t *p_system_clean_cache);
void ikc_registe_system_virtual_to_physical_address(const ikc_system_convert_virtual_to_physical_t *p_system_virtual_to_physical_address);
void ikc_registe_system_physical_to_virtual_address(const ikc_system_convert_physical_to_virtual_t *p_system_physical_to_virtual_address);
void ikc_unregiste_system_memcpy(void);
void ikc_unregiste_system_memset(void);
void ikc_unregiste_system_memcmp(void);
void ikc_unregiste_system_create_mutex(void);
void ikc_unregiste_system_destroy_mutex(void);
void ikc_unregiste_system_lock_mutex(void);
void ikc_unregiste_system_unlock_mutex(void);
void ikc_unregiste_system_print(void);
void ikc_unregiste_system_clean_cache(void);
void ikc_unregiste_system_virtual_to_physical_address(void);
void ikc_unregiste_system_physical_to_virtual_address(void);
void ikc_registe_system_sqrt(const ikc_system_sqrt_t *p_system_sqrt);
void ikc_unregiste_system_sqrt(void);
uint32 img_arch_check_system_control_callback_function(void);

#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))

void ikc_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void ikc_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void ikc_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);

#define IKC_OS_MUTEX_TYPE AMBA_KAL_MUTEX_t
//rtos
//#define amba_ikc_system_memcpy AmbaWrap_memcpy
void amba_ikc_system_memcpy(void *pDst, const void *pSrc, SIZE_t num);
//#define amba_ikc_system_memset AmbaWrap_memset
void amba_ikc_system_memset(void *ptr, INT32 v, SIZE_t n);
#define amba_ikc_system_memcmp AmbaWrap_memcmp
#define amba_ikc_system_create_mutex AmbaKAL_MutexCreate
#define amba_ikc_system_destroy_mutex AmbaKAL_MutexDelete
//#define amba_ikc_system_lock_mutex AmbaKAL_MutexTake
//#define amba_ikc_system_unlock_mutex AmbaKAL_MutexGive
UINT32 amba_ikc_system_lock_mutex(UINT32 context_id, UINT32 timeout);
UINT32 amba_ikc_system_unlock_mutex(UINT32 context_id);
#define amba_ikc_system_print_uint32_5 ikc_system_print_uint32_5 // AmbaPrint_PrintUInt5
#define amba_ikc_system_print_int32_5 ikc_system_print_int32_5 // AmbaPrint_PrintInt5
#define amba_ikc_system_print_str_5 ikc_system_print_str_5 // AmbaPrint_PrintStr5
#define amba_ikc_system_print_flush AmbaPrint_Flush

void* amba_ikc_system_virt_to_phys(void* p_vir_addr);
void* amba_ikc_system_phys_to_virt(void* p_phy_addr);
void amba_ikc_system_clean_cache(const void *addr, UINT32 size);

typedef int32 (*ikc_system_create_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ikc_system_destroy_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ikc_system_lock_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, uint32 timeout);
typedef int32 (*ikc_system_unlock_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex);

#else

void ikc_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5);
void ikc_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5);
void ikc_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5);


#define IKC_OS_MUTEX_TYPE AMBA_KAL_MUTEX_t
//rtos
//#define amba_ikc_system_memcpy AmbaWrap_memcpy //memcpy
void amba_ikc_system_memcpy(void *pDst, const void *pSrc, SIZE_t num);
//#define amba_ikc_system_memset AmbaWrap_memset //memset
void amba_ikc_system_memset(void *ptr, INT32 v, SIZE_t n);
#define amba_ikc_system_memcmp AmbaWrap_memcmp //memcmp
#define amba_ikc_system_create_mutex AmbaKAL_MutexCreate
#define amba_ikc_system_destroy_mutex AmbaKAL_MutexDelete
//#define amba_ikc_system_lock_mutex AmbaKAL_MutexTake
//#define amba_ikc_system_unlock_mutex AmbaKAL_MutexGive
UINT32 amba_ikc_system_lock_mutex(UINT32 context_id, UINT32 timeout);
UINT32 amba_ikc_system_unlock_mutex(UINT32 context_id);
#define amba_ikc_system_print_uint32_5 ikc_system_print_uint32_5 // AmbaPrint_PrintUInt5
#define amba_ikc_system_print_int32_5 ikc_system_print_int32_5 // AmbaPrint_PrintInt5
#define amba_ikc_system_print_str_5 ikc_system_print_str_5 // AmbaPrint_PrintStr5
#define amba_ikc_system_print_flush AmbaPrint_Flush

void* amba_ikc_system_virt_to_phys(void* p_vir_addr);
void* amba_ikc_system_phys_to_virt(void* p_phy_addr);
void amba_ikc_system_clean_cache(const void *addr, UINT32 size);

typedef int32 (*ikc_system_create_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ikc_system_destroy_mutex_t)(IKC_OS_MUTEX_TYPE *p_mutex, char *mutex_name);
typedef int32 (*ikc_system_lock_mutex_t)(uint32 context_id, uint32 timeout);
typedef int32 (*ikc_system_unlock_mutex_t)(uint32 context_id);
#endif

#endif
