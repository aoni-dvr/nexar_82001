/**
 *  @file os_wrap.c
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
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
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
 *  @details OS wrap APIs
 *
 */

#if !defined(CONFIG_BUILD_CV_THREADX)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ambacv_kal.h"
#endif
#include "os_wrap.h"

#if defined(CONFIG_QNX)
#include <sys/ioctl.h>
#endif

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
#include "AmbaFDT.h"
#endif

#if defined(CONFIG_BUILD_CV_THREADX) || defined(CONFIG_QNX) || defined(CONFIG_LINUX)
AMBA_SHELL_PRINT_f ThreadX_PrintFunc;
#endif

#define DEBUG_REG_BASE 0xed000000U

void type_cast(void *pNewType, const void *pOldType)
{
    const void *dest;
    size_t num;

    num = sizeof(void *);
    dest = cvtask_memcpy(pNewType, pOldType, num);
    if(dest == NULL) {
#if !defined(CONFIG_BUILD_CV_THREADX)
        printf("type_cast(): cvtask_memcpy fail\n");
#else
        ThreadX_PrintFunc("type_cast(): cvtask_memcpy fail\n");
#endif
    }
}

uint32_t reg_agent_size(void)
{
    return (uint32_t)sizeof(reg_agent_t);
}

void open_reg_agent(reg_agent_t *r)
{
#if !defined(CONFIG_BUILD_CV_THREADX)
#define DEBUG_REG_SIZE 0x01000000U
#define CV_DRAM_SIZE   0x00300000U

    uint64_t pa;
    ambacv_mem_t mem;
    ambacv_att_region_t att_region;

#if defined(ENABLE_AMBA_MAL) && !defined(CONFIG_QNX)
    r->ambamal_fd = open("/dev/ambamal", O_SYNC | O_RDWR );
#endif

#if defined(CONFIG_QNX)
    r->amba_fd = open("/dev/ambacv", O_RDWR );
#else
    r->amba_fd = open("/dev/ambacv", O_SYNC | O_RDWR );
#endif

#if defined(ENABLE_AMBA_MAL) && !defined(CONFIG_QNX)
    if (r->ambamal_fd < 0) {
        printf("Can't open device file /dev/ambamal !!!\n");
        exit(-1);
    }
#endif

    if (r->amba_fd < 0) {
        printf("Can't open device file /dev/ambacv !!!\n");
        exit(-1);
    }

    if(ioctl(r->amba_fd, AMBACV_GET_MEM_LAYOUT, &mem)) {
        printf("Can't get AMBACV_GET_MEM_LAYOUT!!!\n");
        exit(-1);
    }

    r->cv_dram_base = mem.all.base + 0x1000000;
#if defined(CONFIG_QNX)
    r->cvreg = mmap_device_io(DEBUG_REG_SIZE, (DEBUG_REG_BASE + 0x2000000000UL));
#else
    r->cvreg = mmap(NULL, DEBUG_REG_SIZE, PROT_READ | PROT_WRITE,
                    MAP_SHARED, r->amba_fd, (DEBUG_REG_BASE + 0x2000000000UL));
#endif

    if (r->cvreg == MAP_FAILED) {
        printf("Can't mmap cv shared memory !!!\n");
    }

    // transfer ca to pa
    if(ioctl(r->amba_fd, AMBACV_GET_ATT, &att_region)) {
        printf("Can't get AMBACV_GET_ATT!!!\n");
        exit(-1);
    } else {
        if( (r->cv_dram_base >= att_region.ca) && (r->cv_dram_base <= att_region.ca + att_region.size) ) {
            pa = att_region.pa + (r->cv_dram_base - att_region.ca);
        } else {
            printf("Can't transfer cv_dram_base(0x%x) to phyaddr\n", r->cv_dram_base);
            exit(-1);
        }
    }

#if defined(CONFIG_QNX)
    r->cvdram = mmap(NULL, CV_DRAM_SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED|MAP_PHYS, NOFD, pa);
#else
#if defined(ENABLE_AMBA_MAL)
    r->cvdram = mmap(NULL, CV_DRAM_SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, r->ambamal_fd, pa);
#else
    r->cvdram = mmap(NULL, CV_DRAM_SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, r->amba_fd, pa);
#endif
#endif

    if (r->cvdram == MAP_FAILED) {
        printf("Can't mmap cv dram shared memory starting at 0x%x !!!\n", r->cv_dram_base);
    }

#else
#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    const void *fdt;
    int32_t ret, offset, len;
    const char *chr1;
    const uint32_t *ptr1;
    const struct fdt_property *prop = NULL;
    uint64_t addr;

    addr = (uint64_t)DEBUG_REG_BASE + (uint64_t)0x2000000000UL;
    type_cast(&r->cvreg,&addr);

    addr = (uint64_t)CONFIG_DTB_LOADADDR;
    type_cast(&fdt, &addr);

    ret = AmbaFDT_CheckHeader(fdt);
    if (ret != 0) {
        ThreadX_PrintFunc("open_reg_agent() : AmbaFDT_CheckHeader fail");
    } else {
        offset = AmbaFDT_PathOffset(fdt, "scheduler");
        if (offset < 0) {
            ThreadX_PrintFunc("open_reg_agent() : invalid fdt offset");
        } else {
            prop = AmbaFDT_GetProperty(fdt, offset, "cv_att_pa", &len);
            if ((prop != NULL) && (len > 2)) {
                chr1 = (const char *)&prop->data[0];
                type_cast(&ptr1, &chr1);
                addr = (uint64_t)AmbaFDT_Fdt32ToCpu(ptr1[0]) + 0x1000000UL;
                r->cv_dram_base = (uint32_t)ambacv_p2c(addr);
                type_cast(&r->cvdram,&addr);
            } else {
                ThreadX_PrintFunc("open_reg_agent() : invalid fdt property");
            }
        }
    }
#else
    uint64_t addr = 0UL;
    const uint32_t *pU32;

    addr = (uint64_t)DEBUG_REG_BASE + (uint64_t)0x2000000000UL;
    type_cast(&r->cvreg,&addr);

    pU32 = &__cv_schdr_start;
    type_cast(&addr,&pU32);
    addr = addr + 0x1000000UL;
    r->cv_dram_base = (uint32_t)ambacv_p2c(addr);
    type_cast(&r->cvdram,&addr);
#endif
#endif
    //printf("mapped addr = 0x%x\n", r->cvreg);
}

void close_reg_agent(reg_agent_t *r)
{
#if !defined(CONFIG_BUILD_CV_THREADX)
#if defined(CONFIG_QNX)
    munmap_device_io(r->cvreg, DEBUG_REG_SIZE);
#else
    munmap(r->cvreg, DEBUG_REG_SIZE);
#endif
    munmap(r->cvdram, CV_DRAM_SIZE);

    close( r->amba_fd );
#else
    r->cvreg = NULL;
    r->cvdram = NULL;
#endif
}

uint32_t readl(const reg_agent_t *r, uint32_t addr)
{
    uint32_t addr_align;

    addr_align = addr - DEBUG_REG_BASE;
    addr_align = addr_align >> 2U;

    //printf("mapped addr = 0x%x, word_addr=0x%x\n", r->cvreg, addr_align);
    return r->cvreg[addr_align];
}

void writel(const reg_agent_t *r, uint32_t addr, uint32_t value)
{
    uint32_t addr_align;

    addr_align = addr - DEBUG_REG_BASE;
    addr_align = addr_align >> 2U;

    r->cvreg[addr_align] = value;
}

void read_buf(const reg_agent_t *r, uint32_t addr, uint32_t size, uint8_t* buf)
{
    const uint32_t *src32;
    uint32_t *buf32;
    uint32_t addr_align,size_align;
    uint32_t i;

    addr_align = addr - DEBUG_REG_BASE;
    addr_align = addr_align >> 2U;
    size_align = size + 3U;
    size_align = size_align >> 2U;

    (void) buf;
    type_cast(&buf32,&buf);
    src32 = &r->cvreg[addr_align];

    for(i = 0U; i < size_align; i++) {
        buf32[i] = src32[i];
    }
}

uint32_t cv_dram_v2c(const reg_agent_t *r, const void *buf)
{
    uint64_t addr1 = 0UL,addr2 = 0UL;

    type_cast(&addr1,&buf);
    type_cast(&addr2,&r->cvdram);

    return r->cv_dram_base + (uint32_t)(addr1 - addr2);
}

uint8_t *cv_dram_c2v(const reg_agent_t *r, uint32_t base)
{
    uint8_t *buf;

    type_cast(&buf,&r->cvdram);
    buf = &buf[(base - r->cv_dram_base)];
    return buf;

}

uint32_t cv_cache_clean(const reg_agent_t *r, const void *ptr, uint32_t size)
{
    uint32_t retcode = 0U;
    uint64_t addr;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = 1U;
        } else {
#if defined(CONFIG_BUILD_CV_THREADX)
            (void) r;
            type_cast(&addr,&ptr);
            retcode = AmbaCache_DataClean((ULONG)addr, size);
#else
            ambacv_mem_region_t mem_region;
            addr = (uint64_t)cv_dram_v2c(r, ptr);
            mem_region.base = (uint64_t)ambacv_c2p(addr);
            mem_region.size = (uint64_t)size;
            retcode = ioctl(r->amba_fd, AMBACV_CACHE_CLEAN, &mem_region);
#endif
        }
    }

    return retcode;
}

uint32_t cv_cache_invalidate(const reg_agent_t *r, const void *ptr, uint32_t size)
{
    uint32_t retcode = 0U;
    uint64_t addr = 0UL;

    if(size != 0U) {
        if(ptr == NULL) {
            retcode = 1U;
        } else {
#if defined(CONFIG_BUILD_CV_THREADX)
            (void) r;
            type_cast(&addr,&ptr);
            retcode = AmbaCache_DataInvalidate(addr, size);
#else
            ambacv_mem_region_t mem_region;
            addr = cv_dram_v2c(r, ptr);
            mem_region.base = (uint64_t)ambacv_c2p(addr);
            mem_region.size = (uint64_t)size;
            retcode = ioctl(r->amba_fd, AMBACV_CACHE_INVLD, &mem_region);
#endif
        }
    }
    return retcode;
}

