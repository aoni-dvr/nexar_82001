/**
 *  @file dram_mmap.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Dram memory map APIs
 *
 */

#include "ambacv_kal.h"
#include "os_api.h"
#include "dram_mmap.h"
#include "schdr_api.h"

dram_region_t       schdr_region;
dram_region_t       schdr_cma_region;
dram_region_t       schdr_rtos_region;
dram_region_t       schdr_kregion;

#if defined(ENABLE_AMBA_MAL)
AMBA_MAL_INFO_s     schdr_core_info;
AMBA_MAL_INFO_s     schdr_cma_info;
AMBA_MAL_INFO_s     schdr_rtos_info;
AMBA_MAL_INFO_s     dsp_data_info;
#else
#ifdef USE_CVAPI_V7
static uint64_t cv_pa_start = 0UL,cv_pa_end = 0xFFFFFFFFFFUL;
static uint64_t cv_ca_start = 0UL,cv_ca_end = 0xFFFFFFFFFFUL;
#else /* !USE_CVAPI_V7 */
static uint64_t cv_pa_start = 0UL,cv_pa_end = 0xFFFFFFFFUL;
static uint64_t cv_ca_start = 0UL,cv_ca_end = 0xFFFFFFFFUL;
#endif /* ?USE_CVAPI_V7 */
#endif

#if defined(AMBACV_KERNEL_SUPPORT) || defined(RTOS_BUILD)
uint32_t map_region(dram_region_t *r)
{
    uint32_t retcode = ERRCODE_NONE;
    char *base = NULL;
    const void *ptr = NULL;
    static int32_t map_fd = -1;

    if(r == NULL) {
        retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        console_printU5("[ERROR] map_region() : r == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (r->mm_size == 0U) {
            console_printU5("[ERROR] map_region() : r->mm_size == 0 \n", 0U, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_INPUT_PARAM_INVALID;
        } else {
            if (map_fd < 0) {
                map_fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
            }
            if (map_fd < 0) {
                console_printU5("[ERROR] map_region() : can't open device file /dev/ambacv !!!\n\n", 0U, 0U, 0U, 0U, 0U);
                retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
            } else {
                ptr = ambacv_mmap(NULL, r->mm_size, PROT_READ | PROT_WRITE,
                                  MAP_SHARED, map_fd, r->pa_base);
                if(ptr == MAP_FAILED) {
                    console_printU5("[ERROR] map_region() : ptr == MAP_FAILED PA 0x%x Size 0x%x ", r->pa_base, r->mm_size, 0U, 0U, 0U);
                    retcode = ERR_DRV_SCHDR_MAP_FAIL;
                } else {
                    typecast(&base,&ptr);
                    r->va_base = base;
                }
            }
        }
    }

    return retcode;
}

void unmap_region(dram_region_t *r)
{
    if(r == NULL) {
        console_printU5("[ERROR] unmap_region() : r == NULL ", 0U, 0U, 0U, 0U, 0U);
    } else {
        if (r->mm_size != 0U) {
            if( ambacv_munmap(r->va_base, r->mm_size) != ERRCODE_NONE) {
                console_printU5("[ERROR] unmap_region() : ambacv_munmap fail pa 0x%x size 0x%x", r->pa_base, r->mm_size, 0U, 0U, 0U);
            }
            r->mm_size = 0U;
        }
    }
}


/*******************************************************************************
 * The region for SCHDERMSG between arm and orc is mapped separately.
 * This mapping is not available to general cvtask to avoid potential
 * memory disaster.
 ******************************************************************************/

uint32_t cvtask_mmap_remap(uint64_t new_start)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t end = 0UL, new_size = schdr_region.pa_base + schdr_region.mm_size - new_start;

    if (schdr_region.pa_base != new_start) {
        unmap_region(&schdr_region);
        end = new_start + new_size;
        end = (end + 0xFFFUL);
        end = (end & ~0xFFFUL);
        schdr_region.pa_base = (new_start & ~0xFFFUL);
        schdr_region.mm_size = end - schdr_region.pa_base;
        retcode = map_region(&schdr_region);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] cvtask_mmap_remap() : map_region fail ret 0x%x \n", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CVTASK_REMAP_FAIL;
        }
    }
    return retcode;
}

uint32_t ambacv_att_init(void)
{
    uint32_t retcode = ERRCODE_NONE;
#if !defined(ENABLE_AMBA_MAL)
    ambacv_att_region_t att_region = {0};
    int32_t fd = -1;

    fd = ambacv_open("/dev/ambacv", (O_SYNC | O_RDWR));
    if (fd < 0) {
        console_printU5("[ERROR] ambacv_att_init() : Can't open /dev/ambacv !\n", 0U, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_DEV_UNAVAILABLE;
    } /* if (ambacv_fd < 0) : open("/dev/ambacv", ...) */

    if(retcode == ERRCODE_NONE) {
        retcode = ambacv_ioctl(fd, AMBACV_GET_ATT, &att_region);
        if(retcode == ERRCODE_NONE) {
            cv_pa_start = att_region.pa;
            cv_pa_end = (att_region.pa + att_region.size);

            cv_ca_start = att_region.ca;
            cv_ca_end = (att_region.ca + att_region.size);
        } /* if(retcode == ERRCODE_NONE) */
        else { /* if(retcode != ERRCODE_NONE) */
            console_printU5("[WARN] : ATT driver unavailable (ioctl returns error 0x%08x); defaulting to disabled\n", retcode, 0U, 0U, 0U, 0U);
            cv_pa_start = 0U;
            cv_pa_end   = 0xFFFFFFFFU;

            cv_ca_start = 0U;
            cv_ca_end   = 0xFFFFFFFFU;
            retcode = ERRCODE_NONE;
        } /* if(retcode != ERRCODE_NONE) */
        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"  ATT PA region : [0x%llx -- 0x%llx]\n", cv_pa_start, cv_pa_end, 0U, 0U, 0U);
        module_printU5(AMBA_SCHDR_PRINT_MODULE_ID,"  ATT CA region : [0x%llx -- 0x%llx]\n", cv_ca_start, cv_ca_end, 0U, 0U, 0U);

    }

    if (fd >= 0) {
        retcode = ambacv_close(fd);
    }
#else
    if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SCHDR, &schdr_core_info) != 0U) {
        retcode = ERR_DRV_SCHDR_MAP_FAIL;
    }

    if(retcode == ERRCODE_NONE) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_SYS, &schdr_cma_info) != 0U) {
            retcode = ERR_DRV_SCHDR_MAP_FAIL;
        }
    }

    if(retcode == ERRCODE_NONE) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_CV_RTOS, &schdr_rtos_info) != 0U) {
            retcode = ERR_DRV_SCHDR_MAP_FAIL;
        }
    }

    if(retcode == ERRCODE_NONE) {
        if( AmbaMAL_GetInfo(AMBA_MAL_ID_DSP_DATA, &dsp_data_info) != 0U) {
            retcode = ERR_DRV_SCHDR_MAP_FAIL;
        }
    }
#endif
    return retcode;
}
#endif

int32_t schdr_set_dram_mode(int32_t mode, const void *arg)
{
    (void) mode;
    (void) arg;
    return 0;
}

uint32_t schdr_mmap_init(uint64_t start, uint64_t size)
{
    uint32_t retcode = ERRCODE_NONE;

    schdr_kregion.pa_base = start;
    schdr_kregion.mm_size = size;
    retcode = map_region(&schdr_kregion);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] schdr_mmap_init() : map_region fail ret 0x%x \n", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_CVTASK_REMAP_FAIL;
    }

    return retcode;
}

uint32_t  cvtask_mmap_init(uint64_t start, uint64_t end)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t start_align = 0UL,end_align = 0UL;

    start_align = (start & ~0xFFFUL);
    end_align = (end + 0xFFFUL);
    end_align = (end_align & ~0xFFFUL);
    schdr_region.mm_size = end_align - start_align;
    schdr_region.pa_base = start_align;
    retcode = map_region(&schdr_region);
    if(retcode != ERRCODE_NONE) {
        console_printU5("[ERROR] cvtask_mmap_init() : map_region fail ret = 0x%x ", retcode, 0U, 0U, 0U, 0U);
        retcode = ERR_DRV_SCHDR_CVTASK_MAP_FAIL;
    }
    return retcode;
}

uint32_t schdr_cma_mmap_init(uint64_t start, uint64_t end)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t start_align = 0UL,end_align = 0UL;

    if (start != end) {
        start_align = (start & ~0xFFFUL);
        end_align = (end + 0xFFFUL);
        end_align = (end_align & ~0xFFFUL);
        schdr_cma_region.mm_size = end_align - start_align;
        schdr_cma_region.pa_base = start_align;
#if defined(ENABLE_AMBA_MAL)
        schdr_cma_region.va_base = NULL;
#else
        retcode = map_region(&schdr_cma_region);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_cma_mmap_init() : map_region fail ret = 0x%x ", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_CMA_MAP_FAIL;
        }
#endif
    } else {
        schdr_cma_region.mm_size = 0UL;
        schdr_cma_region.pa_base = 0UL;
        schdr_cma_region.va_base = NULL;
    }
    return retcode;
}

uint32_t schdr_rtos_mmap_init(uint64_t start, uint64_t end)
{
    uint32_t retcode = ERRCODE_NONE;
    uint64_t start_align = 0UL,end_align = 0UL;

    if (start != end) {
        start_align = (start & ~0xFFFUL);
        end_align = (end + 0xFFFUL);
        end_align = (end_align & ~0xFFFUL);
        schdr_rtos_region.mm_size = end_align - start_align;
        schdr_rtos_region.pa_base = start_align;
#if defined(ENABLE_AMBA_MAL)
        schdr_rtos_region.va_base = NULL;
#else
        retcode = map_region(&schdr_rtos_region);
        if(retcode != ERRCODE_NONE) {
            console_printU5("[ERROR] schdr_rtos_mmap_init() : map_region fail ret = 0x%x ", retcode, 0U, 0U, 0U, 0U);
            retcode = ERR_DRV_SCHDR_RTOS_MAP_FAIL;
        }
#endif
    } else {
        schdr_rtos_region.mm_size = 0UL;
        schdr_rtos_region.pa_base = 0UL;
        schdr_rtos_region.va_base = NULL;
    }
    return retcode;
}

uint32_t schdr_shutdown_unmap(void)
{
    uint32_t retcode = ERRCODE_NONE;

    unmap_region(&schdr_region);
    unmap_region(&schdr_kregion);
#if !defined(ENABLE_AMBA_MAL)
    unmap_region(&schdr_cma_region);
    unmap_region(&schdr_rtos_region);
#endif
    return retcode;
}

void* ambacv_p2v(uint64_t pa)
{
    const char *base = NULL;
    void *ptr = NULL;
    uint64_t offset = 0UL;

    if (pa != 0U) {
        if (pa >= schdr_kregion.pa_base) {
            offset = pa - schdr_kregion.pa_base;
            if (offset < schdr_kregion.mm_size) {
                base = &schdr_kregion.va_base[offset];
            }
        }

        if ( (base == NULL) && (pa >= schdr_region.pa_base) ) {
            offset = pa - schdr_region.pa_base;
            if (offset < schdr_region.mm_size) {
                base = &schdr_region.va_base[offset];
            }
        }

#if defined(ENABLE_AMBA_MAL)
        if ( (base == NULL) && (pa >= schdr_cma_region.pa_base) ) {
            uint32_t ret;
            void *pVaddr = NULL;

            ret = AmbaMAL_Phys2Virt(AMBA_MAL_ID_CV_SYS, pa, AMBA_MAL_ATTRI_CACHE, &pVaddr);
            typecast(&base,&pVaddr);
            (void) ret;
        }

        if ( (base == NULL) && (pa >= schdr_rtos_region.pa_base) ) {
            uint32_t ret;
            void *pVaddr = NULL;

            ret = AmbaMAL_Phys2Virt(AMBA_MAL_ID_CV_RTOS, pa, AMBA_MAL_ATTRI_CACHE, &pVaddr);
            typecast(&base,&pVaddr);
            (void) ret;
        }

        if (base == NULL) {
            if ((pa >= dsp_data_info.PhysAddr) && (pa < (dsp_data_info.PhysAddr + dsp_data_info.Size))) {
                uint32_t ret;
                void *pVaddr = NULL;

                ret = AmbaMAL_Phys2Virt(AMBA_MAL_ID_DSP_DATA, pa, AMBA_MAL_ATTRI_CACHE, &pVaddr);
                typecast(&base,&pVaddr);
                (void) ret;
            }
        }
#else
        if ( (base == NULL) && (pa >= schdr_cma_region.pa_base) ) {
            offset = pa - schdr_cma_region.pa_base;
            if (offset < schdr_cma_region.mm_size) {
                base = &schdr_cma_region.va_base[offset];
            }
        }

        if ( (base == NULL) && (pa >= schdr_rtos_region.pa_base) ) {
            offset = pa - schdr_rtos_region.pa_base;
            if (offset < schdr_rtos_region.mm_size) {
                base = &schdr_rtos_region.va_base[offset];
            }
        }
#endif
        if(base == NULL) {
            console_printU5("[ERROR] ambacv_p2v() : fail pa 0x%x \n", pa, 0U, 0U, 0U, 0U);
        }
    } else {
        console_printU5("[ERROR] ambacv_p2v() : pa == 0U (0x%x)\n", 0U, 0U, 0U, 0U, 0U);
    }
    typecast(&ptr,&base);
    return ptr;
}

uint64_t ambacv_v2p(void *va)
{
    uint64_t  start = 0UL,target = 0UL;
    uint64_t offset = 0UL, pa = 0UL;

    (void) va;
    if (va != NULL) {
        typecast(&target,&va);
        typecast(&start,&schdr_kregion.va_base);
        if (target >= start) {
            offset = (target - start);
            if (offset < schdr_kregion.mm_size) {
                pa = schdr_kregion.pa_base + offset;
            }
        }

        typecast(&start,&schdr_region.va_base);
        if ((pa == 0U) && (target >= start)) {
            offset = (target - start);
            if (offset < schdr_region.mm_size) {
                pa = schdr_region.pa_base + offset;
            }
        }

#if defined(ENABLE_AMBA_MAL)
        if(pa == 0U) {
            uint32_t ret;

            ret = AmbaMAL_Virt2Phys(AMBA_MAL_ID_CV_SYS, va, (UINT64 *)&pa);
            (void) ret;
        }


        if(pa == 0U) {
            if(schdr_rtos_info.Type != AMBA_MAL_TYPE_INVALID) {
                uint32_t ret;

                ret = AmbaMAL_Virt2Phys(AMBA_MAL_ID_CV_RTOS, va, (UINT64 *)&pa);
                (void) ret;
            }
        }

        if(pa == 0U) {
            if(dsp_data_info.Type != AMBA_MAL_TYPE_INVALID) {
                uint32_t ret;

                ret = AmbaMAL_Virt2Phys(AMBA_MAL_ID_DSP_DATA, va, (UINT64 *)&pa);
                (void) ret;
            }
        }
#else
        typecast(&start,&schdr_cma_region.va_base);
        if ((pa == 0U) && (target >= start)) {
            offset = (target - start);
            if (offset < schdr_cma_region.mm_size) {
                pa = schdr_cma_region.pa_base + offset;
            }
        }

        typecast(&start,&schdr_rtos_region.va_base);
        if ((pa == 0U) && (target >= start)) {
            offset = (target - start);
            if (offset < schdr_rtos_region.mm_size) {
                pa = schdr_rtos_region.pa_base + offset;
            }
        }
#endif

        if(pa == 0U) {
            console_printU5("[ERROR] ambacv_v2p() : fail \n", 0U, 0U, 0U, 0U, 0U);
        }
    } else {
        console_printU5("[ERROR] ambacv_v2p() : _va == NULL (0x%x)\n", 0U, 0U, 0U, 0U, 0U);
    }

    return pa;
}

#if defined(ENABLE_AMBA_MAL)
uint64_t ambacv_p2c(uint64_t pa)
{
    uint64_t ca = 0UL;

    if(pa != 0U) {
        if( AmbaMAL_Phys2Global(pa,(UINT64 *)&ca) != 0U) {
            console_printU5("[ERROR] ambacv_p2c() : fail: pa (0x%llx)\n", pa, 0U, 0U, 0U, 0U);
        }
    }
    return ca;
}

uint64_t ambacv_c2p(uint64_t ca)
{
    uint64_t pa = 0UL;

    if( AmbaMAL_Global2Phys(ca,(UINT64 *)&pa) != 0U) {
        console_printU5("[ERROR] ambacv_c2p() : fail: ca (0x%llx)\n", ca, 0U, 0U, 0U, 0U);
    }
    return pa;
}
#else
uint64_t ambacv_p2c(uint64_t pa)
{
    uint64_t ca = 0UL;

    if(pa != 0U) {
        if( (pa >= cv_pa_start) && (pa <= cv_pa_end) ) {
            ca = cv_ca_start + (pa - cv_pa_start);
        } else {
            console_printU5("[ERROR] ambacv_p2c() : fail: pa (0x%llx)\n", pa, 0U, 0U, 0U, 0U);
        }
    }
    return ca;
}

uint64_t ambacv_c2p(uint64_t ca)
{
    uint64_t pa = 0UL;

    if( (ca >= cv_ca_start) && (ca <= cv_ca_end) ) {
        pa = cv_pa_start + (ca - cv_ca_start);
    } else {
        console_printU5("[ERROR] ambacv_c2p() : fail: ca (0x%llx)\n", ca, 0U, 0U, 0U, 0U);
    }
    return pa;
}
#endif

void* ambacv_c2v(uint64_t ca)
{
    uint64_t pa = 0UL;
    void *ptr = NULL;

    pa = ambacv_c2p(ca);
    if(pa != 0UL) {
        ptr = ambacv_p2v(pa);
    } else {
        console_printU5("[ERROR] ambacv_c2v() : fail: ca (0x%llx)\n", ca, 0U, 0U, 0U, 0U);
    }
    return ptr;
}

uint64_t ambacv_v2c(void *va)
{
    uint64_t ca = 0UL;
    uint64_t pa = 0UL;

    if(va == NULL) {
        console_printU5("[ERROR] ambacv_v2c() : _va == NULL (0x%x)\n", 0U, 0U, 0U, 0U, 0U);
    } else {
        pa = ambacv_v2p(va);
        ca = ambacv_p2c(pa);
    }
    return ca;
}

#if defined(ENABLE_AMBA_MAL)
uint32_t cvtask_v2off(void *va)
{
    uint32_t offset = 0U;

    (void) va;
    console_printU5("[ERROR] cvtask_v2off() : Not support when ENABLE_AMBA_MAL \n", 0U, 0U, 0U, 0U, 0U);
    return offset;
}
#else
uint32_t cvtask_v2off(void *va)
{
    uint64_t  start = 0UL, cma_start = 0UL, target = 0UL;
    uint32_t offset = 0U;

    (void) va;
    if (va != NULL) {
        typecast(&target,&va);
        typecast(&start,&schdr_region.va_base);
        typecast(&cma_start,&schdr_cma_region.va_base);
        if ((target >= start) && ((target - start) < schdr_region.mm_size)) {
            offset = (uint32_t)(target - start);
        } else if ((target >= cma_start) && ((target - cma_start) < schdr_cma_region.mm_size)) {
            offset = (uint32_t)(target - cma_start);
        } else {
            console_printU5("[ERROR] cvtask_v2off() : cvtask va address %x out of range !\n", (uint32_t)target, 0U, 0U, 0U, 0U);
        }
    } else {
        console_printU5("[ERROR] cvtask_v2off() : va == NULL (0x%x)\n", 0U, 0U, 0U, 0U, 0U);
    }

    return offset;
}
#endif
