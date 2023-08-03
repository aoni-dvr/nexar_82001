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

#include "AmbaDSP_ImgArchSpec.h"
#include "AmbaDSP_ImgArchSystemAPI.h"
#include "AmbaDSP_IkcSystemInfo.h"
#include "ik_data_type.h"

#if (defined(__unix__) && !defined(__QNX__) && !defined(CONFIG_LINUX))

ik_system_memcpy_t ik_system_memcpy;
ik_system_memset_t ik_system_memset;
ik_system_memcmp_t ik_system_memcmp;
ik_system_create_mutex_t ik_system_create_mutex;
ik_system_destroy_mutex_t ik_system_destroy_mutex;
ik_system_lock_mutex_t ik_system_lock_mutex;
ik_system_unlock_mutex_t ik_system_unlock_mutex;
ik_system_convert_virtual_to_physical_t ik_system_virtual_to_physical;
ik_system_convert_physical_to_virtual_t ik_system_physical_to_virtual;
ik_system_print_t ik_system_print;
ik_system_clean_cache_t ik_system_clean_cache;
ik_system_sqrt_t ik_system_sqrt;


void img_arch_registe_system_memcpy(const ik_system_memcpy_t *p_system_memcpy)
{
    ik_system_memcpy = *p_system_memcpy;
}

void img_arch_registe_system_memset(const ik_system_memset_t *p_system_memset)
{
    ik_system_memset = *p_system_memset;
}

void img_arch_registe_system_memcmp(const ik_system_memcmp_t *p_system_memcmp)
{
    ik_system_memcmp = *p_system_memcmp;
}
void img_arch_registe_system_create_mutex(const ik_system_create_mutex_t *p_system_create_mutex)
{
    ik_system_create_mutex = *p_system_create_mutex;
}

void img_arch_registe_system_destroy_mutex(const ik_system_destroy_mutex_t *p_system_destroy_mutex)
{
    ik_system_destroy_mutex = *p_system_destroy_mutex;
}

void img_arch_registe_system_lock_mutex(const ik_system_lock_mutex_t *p_system_lock_mutex)
{
    ik_system_lock_mutex = *p_system_lock_mutex;
}

void img_arch_registe_system_unlock_mutex(const ik_system_unlock_mutex_t *p_system_unlock_mutex)
{
    ik_system_unlock_mutex = *p_system_unlock_mutex;
}

void img_arch_registe_system_print(const ik_system_print_t *p_system_print)
{
    ik_system_print = *p_system_print;
}

void img_arch_registe_system_clean_cache(const ik_system_clean_cache_t *p_system_clean_cache)
{
    ik_system_clean_cache = *p_system_clean_cache;
}

void img_arch_registe_system_virtual_to_physical_address(const ik_system_convert_virtual_to_physical_t *p_system_virtual_to_physical_address)
{
    ik_system_virtual_to_physical = *p_system_virtual_to_physical_address;
}

void img_arch_registe_system_physical_to_virtual_address(const ik_system_convert_physical_to_virtual_t *p_system_physical_to_virtual_address)
{
    ik_system_physical_to_virtual = *p_system_physical_to_virtual_address;
}

void img_arch_registe_system_sqrt(const ik_system_sqrt_t *p_system_sqrt)
{
    ik_system_sqrt = *p_system_sqrt;
}

void img_arch_unregiste_system_memcpy(void)
{
    ik_system_memcpy = NULL;
}

void img_arch_unregiste_system_memset(void)
{
    ik_system_memset = NULL;
}

void img_arch_unregiste_system_memcmp(void)
{
    ik_system_memcmp = NULL;
}

void img_arch_unregiste_system_create_mutex(void)
{
    ik_system_create_mutex = NULL;
}

void img_arch_unregiste_system_destroy_mutex(void)
{
    ik_system_destroy_mutex = NULL;
}

void img_arch_unregiste_system_lock_mutex(void)
{
    ik_system_lock_mutex = NULL;
}

void img_arch_unregiste_system_unlock_mutex(void)
{
    ik_system_unlock_mutex = NULL;
}

void img_arch_unregiste_system_print(void)
{
    ik_system_print = NULL;
}

void img_arch_unregiste_system_clean_cache(void)
{
    ik_system_clean_cache = NULL;
}

void img_arch_unregiste_system_virtual_to_physical_address(void)
{
    ik_system_virtual_to_physical = NULL;
}

void img_arch_unregiste_system_physical_to_virtual_address(void)
{
    ik_system_physical_to_virtual = NULL;
}

void img_arch_unregiste_system_sqrt(void)
{
    ik_system_sqrt = NULL;
}

uint32 img_arch_check_system_control_callback_function(void)
{
    uint32 rval = IK_OK;

    if((ik_system_memcpy == NULL) ||\
       (ik_system_memset == NULL) ||\
       (ik_system_memcmp == NULL) ||\
       (ik_system_create_mutex == NULL) ||\
       (ik_system_destroy_mutex == NULL) ||\
       (ik_system_lock_mutex == NULL) ||\
       (ik_system_unlock_mutex == NULL) ||\
       (ik_system_virtual_to_physical == NULL) ||\
       (ik_system_physical_to_virtual == NULL) ||\
       (ik_system_print == NULL) ||\
       (ik_system_clean_cache == NULL) ||\
       (ik_system_sqrt == NULL)) {
        if(ik_system_print != NULL) {
            (void)ik_system_print("ik_system_memcpy %p \n", ik_system_memcpy);
            (void)ik_system_print("ik_system_memset %p \n", ik_system_memset);
            (void)ik_system_print("ik_system_memcmp %p \n", ik_system_memcmp);
            (void)ik_system_print("ik_system_create_mutex %p \n", ik_system_create_mutex);
            (void)ik_system_print("ik_system_destroy_mutex %p \n", ik_system_destroy_mutex);
            (void)ik_system_print("ik_system_lock_mutex %p \n", ik_system_lock_mutex);
            (void)ik_system_print("ik_system_unlock_mutex %p \n", ik_system_unlock_mutex);
            (void)ik_system_print("ik_system_virtual_to_physical %p \n", ik_system_virtual_to_physical);
            (void)ik_system_print("ik_system_physical_to_virtual %p \n", ik_system_physical_to_virtual);
            (void)ik_system_print("ik_system_print %p \n", ik_system_print);
            (void)ik_system_print("ik_system_clean_cache %p \n", ik_system_clean_cache);
            (void)ik_system_print("ik_system_sqrt %p \n", ik_system_sqrt);
        }
        rval = IK_ERR_0005;

    } else {
        rval = IK_OK;
    }

    return rval;

}

uint32 ik_import_system_callback_func(const ik_system_api_t *p_sys_api)
{
    {
        img_arch_registe_system_memcpy(&(p_sys_api->system_memcpy));
        img_arch_registe_system_memset(&(p_sys_api->system_memset));
        img_arch_registe_system_memcmp(&(p_sys_api->system_memcmp));
        img_arch_registe_system_create_mutex(&(p_sys_api->system_create_mutex));
        img_arch_registe_system_destroy_mutex(&(p_sys_api->system_destroy_mutex));
        img_arch_registe_system_lock_mutex(&(p_sys_api->system_lock_mutex));
        img_arch_registe_system_unlock_mutex(&(p_sys_api->system_unlock_mutex));
        img_arch_registe_system_print(&(p_sys_api->system_print));
        img_arch_registe_system_clean_cache(&(p_sys_api->system_clean_cache));
        img_arch_registe_system_virtual_to_physical_address(&(p_sys_api->system_virtual_to_physical_address));
        img_arch_registe_system_physical_to_virtual_address(&(p_sys_api->system_physical_to_virtual_address));
        img_arch_registe_system_sqrt(&(p_sys_api->system_sqrt));
    }
    {
        ikc_system_api_t ikc_system_api;

        ikc_system_api.system_memcpy = (ikc_system_memcpy_t)p_sys_api->system_memcpy;
        ikc_system_api.system_memset = (ikc_system_memset_t)p_sys_api->system_memset;
        ikc_system_api.system_memcmp = (ikc_system_memcmp_t)p_sys_api->system_memcmp;
        ikc_system_api.system_print = (ikc_system_print_t)p_sys_api->system_print;

        ikc_import_system_callback_func(&ikc_system_api);
    }
    return IK_OK;
}
#elif (defined(__unix__) && (defined(__QNX__) || defined(CONFIG_LINUX)))
static uint64 offset = 0xDEADBEEFDEADBEEFUL;

void* amba_ik_system_virt_to_phys(const void* p_vir_addr)
{
    const void *temp;
    uint64 misra_uint64_vir_addr;
    uint64 misra_uint64_phy_addr = 0UL;
#if defined(CONFIG_LINUX)
    uint32 misar_uint32_phy_addr = 0U;
#endif
    uint64 current_mmap_offset;
    void *p_misra_void_phy_addr = NULL;
    uint32 rval = 0U;
    intptr error_return = -1;

    if(p_vir_addr == NULL) {
        (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &error_return, sizeof(void*));//error.
    } else {
        temp = p_vir_addr;
        (void)amba_ik_system_memcpy(&misra_uint64_vir_addr, &temp, sizeof(uint64));
#if defined(CONFIG_LINUX)
        rval = AmbaDSP_VirtToPhys(misra_uint64_vir_addr, &misar_uint32_phy_addr);
        (void)amba_ik_system_memcpy(&misra_uint64_phy_addr, &misar_uint32_phy_addr, sizeof(uint32));
#else
        rval = AmbaMMU_Virt64ToPhys64(misra_uint64_vir_addr, &misra_uint64_phy_addr);
#endif
        if (rval == 0U) {
            current_mmap_offset = misra_uint64_vir_addr - misra_uint64_phy_addr;
            if(offset == 0xDEADBEEFDEADBEEFUL) { //init mmap offset from init-arch.
                offset = current_mmap_offset;
                (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &misra_uint64_phy_addr, sizeof(void*));
            } else if (current_mmap_offset != offset) { // examine mmap offset shouldn't run-time change
                (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &error_return, sizeof(void*));//error.
            } else {
                (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &misra_uint64_phy_addr, sizeof(void*));
            }

        } else {
            (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &error_return, sizeof(void*));//error.
        }
    }

    return p_misra_void_phy_addr;
}
void* amba_ik_system_phys_to_virt(const void* p_phy_addr)
{
    const void *temp;
    uint64 misra_uint64_phy_addr;
    uint64 misra_uint64_vir_addr;
    void *p_misra_void_vir_addr = NULL;
    intptr error_return = -1;

    if (p_phy_addr == NULL) {
        (void)amba_ik_system_memcpy(&p_misra_void_vir_addr, &error_return, sizeof(void*));//error.
    } else {
        temp = p_phy_addr;
        (void)amba_ik_system_memcpy(&misra_uint64_phy_addr, &temp, sizeof(uint64));
        misra_uint64_vir_addr = misra_uint64_phy_addr + offset;
        (void)amba_ik_system_memcpy(&p_misra_void_vir_addr, &misra_uint64_vir_addr, sizeof(void*));
    }

    return p_misra_void_vir_addr;
}

#if defined(__QNX__)
void amba_ik_system_clean_cache(const void *addr, uint32 size)
{
    uint64 misra_uint64;

    (void)amba_ik_system_memcpy(&misra_uint64, &addr, sizeof(uint64));
    (void)AmbaCache_DataClean(misra_uint64, (uint64)size);
}
#else
void amba_ik_system_clean_cache(const void *addr, uint32 size)
{
    uint64 misra_uint64;

    (void)amba_ik_system_memcpy(&misra_uint64, &addr, sizeof(uint64));
    (void)AmbaDSP_MainCacheBufOp(DSP_CACHE_OP_CLEAN, misra_uint64, size);
}
#endif


void ik_system_print_uint64_5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintULL5((UINT16)ik_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void ik_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintUInt5((UINT16)ik_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void ik_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintInt5((UINT16)ik_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void ik_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintStr5((UINT16)ik_error_base_u16, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

//coverity rule 4.7 refine.
void amba_ik_system_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if(AmbaWrap_memcpy(pDst, pSrc, num) == AMBA_OK) {;}
}
//coverity rule 4.7 refine.
void amba_ik_system_memset(void *ptr, INT32 v, SIZE_t n)
{
    if(AmbaWrap_memset(ptr, v, n) == AMBA_OK) {;}
}

#else ///////////////////////////////

static uint64 offset=0;
void* amba_ik_system_virt_to_phys(const void* p_vir_addr)
{
    const void *temp;
    ULONG misra_ulong_vir_addr = 0UL;
    ULONG misra_ulong_phy_addr = 0UL;
    uint32 misra_uint32_phy_addr = 0U;
    void *p_misra_void_phy_addr = NULL;
    uint32 rval = 0U;
    intptr error_return = -1;

    (void)p_vir_addr;//Misra
    if(p_vir_addr == NULL) {
        (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &error_return, sizeof(void*));//error.
    } else {
        temp = p_vir_addr;
        (void)amba_ik_system_memcpy(&misra_ulong_vir_addr, &temp, sizeof(uint64));
        rval = AmbaDSP_VirtToCli(misra_ulong_vir_addr, &misra_uint32_phy_addr);
        (void)amba_ik_system_memcpy(&misra_ulong_phy_addr, &misra_uint32_phy_addr, sizeof(uint32));
        //rval = AmbaMMU_Virt64ToPhys64(misra_ulong_vir_addr, &misra_ulong_phy_addr);
        if (rval == 0U) {
            offset = (uint64)misra_ulong_vir_addr - (uint64)misra_ulong_phy_addr;
            (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &misra_ulong_phy_addr, sizeof(void*));
        } else {
            (void)amba_ik_system_memcpy(&p_misra_void_phy_addr, &error_return, sizeof(void*));//error.
        }
    }
    return p_misra_void_phy_addr;
}

void* amba_ik_system_phys_to_virt(const void* p_phy_addr)
{
    const void *temp;
    uint64 misra_uint64_phy_addr;
    uint64 misra_uint64_vir_addr;
    void *p_misra_void_vir_addr = NULL;
    intptr error_return = -1;

    (void)p_phy_addr;//Misra
    if (p_phy_addr == NULL) {
        (void)amba_ik_system_memcpy(&p_misra_void_vir_addr, &error_return, sizeof(void*));//error.
    } else {
        temp = p_phy_addr;
        (void)amba_ik_system_memcpy(&misra_uint64_phy_addr, &temp, sizeof(uint64));
        misra_uint64_vir_addr = misra_uint64_phy_addr + offset;
        (void)amba_ik_system_memcpy(&p_misra_void_vir_addr, &misra_uint64_vir_addr, sizeof(void*));
    }

    return p_misra_void_vir_addr;
}

#if 0
void* amba_ik_system_virt_to_phys(void* p_vir_addr)
{
    const void *temp;
    uint32 misra_uint32_vir_addr;
    uint32 misra_uint32_phy_addr;
    void *misra_void_phy_addr;
    uint32 rval;

    temp = p_vir_addr;
    (void)amba_ik_system_memcpy(&misra_uint32_vir_addr, &temp, sizeof(uint32));
    rval = AmbaMMU_Virt32ToPhys32(misra_uint32_vir_addr, &misra_uint32_phy_addr);
    if (rval==0U) {
        (void)amba_ik_system_memcpy(&misra_void_phy_addr, &misra_uint32_phy_addr, sizeof(uint32*));
    } else {
        misra_void_phy_addr = p_vir_addr;
        amba_ik_system_print_str_5("amba_ik_system_virt_to_phys fail to get addr.", DC_S, DC_S, DC_S, DC_S, DC_S);
    }

    return misra_void_phy_addr;
}
void* amba_ik_system_phys_to_virt(void* p_phy_addr)
{
    const void *temp;
    uint32 misra_uint32_phy_addr;
    uint32 misra_uint32_vir_addr;
    void *misra_void_vir_addr;
    uint32 rval;

    temp = p_phy_addr;
    (void)amba_ik_system_memcpy(&misra_uint32_phy_addr, &temp, sizeof(uint32));
    rval = AmbaMMU_Phys32ToVirt32(misra_uint32_phy_addr, &misra_uint32_vir_addr);
    if (rval==0U) {
        (void)amba_ik_system_memcpy(&misra_void_vir_addr, &misra_uint32_vir_addr, sizeof(uint32*));
    } else {
        misra_void_vir_addr = p_phy_addr;
        amba_ik_system_print_str_5("amba_ik_system_phys_to_virt fail to get addr.", DC_S, DC_S, DC_S, DC_S, DC_S);
    }

    return misra_void_vir_addr;
}
#endif

void amba_ik_system_clean_cache(const void *addr, UINT32 size)
{
    ULONG misra_ulong_addr;

    (void)amba_ik_system_memcpy(&misra_ulong_addr, &addr, sizeof(ULONG));
    (void)AmbaCache_DataClean(misra_ulong_addr, size);
}

void ik_system_print_uint64_5(const char *pFmt, UINT64 Arg1, UINT64 Arg2, UINT64 Arg3, UINT64 Arg4, UINT64 Arg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintULL5((UINT16)ik_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void ik_system_print_uint32_5(const char *pFmt, UINT32 Arg1, UINT32 Arg2, UINT32 Arg3, UINT32 Arg4, UINT32 Arg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintUInt5((UINT16)ik_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void ik_system_print_int32_5(const char *pFmt, INT32 Arg1, INT32 Arg2, INT32 Arg3, INT32 Arg4, INT32 Arg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintInt5((UINT16)ik_error_base_u16, pFmt, Arg1, Arg2, Arg3, Arg4,Arg5);
}
void ik_system_print_str_5(const char *pFmt, const char *pArg1, const char *pArg2, const char *pArg3, const char *pArg4, const char *pArg5)
{
    uint32 ik_error_base_u16 = AMBA_IK_ERR_BASE>>16ULL;
    AmbaPrint_ModulePrintStr5((UINT16)ik_error_base_u16, pFmt, pArg1, pArg2, pArg3, pArg4, pArg5);
}

//coverity rule 4.7 refine.
void amba_ik_system_memcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if(AmbaWrap_memcpy(pDst, pSrc, num) == AMBA_OK) {;}
}
//coverity rule 4.7 refine.
void amba_ik_system_memset(void *ptr, INT32 v, SIZE_t n)
{
    if(AmbaWrap_memset(ptr, v, n) == AMBA_OK) {;}
}

#endif

