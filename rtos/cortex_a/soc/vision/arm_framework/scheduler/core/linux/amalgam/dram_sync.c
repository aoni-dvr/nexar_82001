/**
 *  @file dram_sync.c
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
 *  @details Dram sync APIs
 *
 */

#include "dram_sync.h"
#include "amalgam.h"
#include "os_api.h"

static uint64_t v2p(void *va)
{
#ifdef  LINUX_BUILD
#ifndef DAG_KERNEL_SUPPORT
    uint32_t addr;
    extern uint32_t schdr_v2p(void *);
    addr = schdr_v2p(va);
    if (addr != 0)
        return addr;
#endif
#endif
    return ambacv_v2p(va);
}

void* schdr_p2v(uint32_t pa)
{
    const char *base = NULL;
    uint32_t offset;
    void *ptr = NULL;

    if (pa != 0U) {
        if (pa >= schdr_kregion.pa_base ) {
            offset = pa - schdr_kregion.pa_base;
            if (offset < schdr_kregion.mm_size) {
                base = &schdr_kregion.va_base[offset];
            }
        }
    }

    typecast(&ptr,&base);
    return ptr;
}

uint32_t schdr_v2p(void *_va)
{
    uint64_t  start = 0UL,target = 0UL;
    uint32_t offset, pa = 0U;

    (void) _va;
    if (_va != NULL) {
        typecast(&target,&_va);
        typecast(&start,&schdr_kregion.va_base);
        if (target >= start) {
            offset = (uint32_t)(target - start);
            if (offset < schdr_kregion.mm_size) {
                pa = schdr_kregion.pa_base + offset;
            }
        }
    }
    return pa;
}

uint32_t ambacv_cache_init(void)
{
    int fd;

    fd = amalgam_init();
    amalgam_start(fd);
    amalgam_send_echo(OS_NAME " is alive");
    return 0;
}

uint32_t ambacv_cache_clean(void *ptr, uint64_t size)
{
    // write to amalgam @size bytes at address @ptr
    amalgam_send_write(v2p(ptr), ptr, size);
    return 0;
}

uint32_t ambacv_cache_clean_phy(void *ptr, uint32_t size, uint32_t addr)
{
    amalgam_send_write(addr, ptr, size);
    return 0;
}

uint32_t ambacv_cache_invalidate(void *ptr, uint64_t size)
{
    // read from amalgam @size bytes at address @ptr
    amalgam_send_read(v2p(ptr), size);
    return 0;
}

uint32_t ambacv_cache_invalidate_phy(void *ptr, uint64_t size, uint64_t addr)
{
    // read from amalgam @size bytes at address @ptr
    amalgam_send_read(addr, size);
    return 0;
}

void ambacv_cache_barrier(void)
{
    amalgam_wait_for_read_done();
}

void dag_send_sync_count(int id)
{
    amalgam_send_sync(id);
}

uint32_t map_region(dram_region_t *r)
{
    char *ptr;

    ptr = malloc(r->mm_size);
    assert(ptr != NULL);
    r->va_base = ptr;
    return 0;
}

void unmap_region(dram_region_t *r)
{
    if (r->mm_size == 0)
        return;

    free(r->va_base);
    r->mm_size = 0;
}

uint32_t cvtask_mmap_remap(uint64_t new_start)
{
    uint32_t retcode = ERRCODE_NONE;
    dram_region_t backup = schdr_region;
    uint32_t end, new_size = schdr_region.pa_base + schdr_region.mm_size - new_start;
    char *tmp = NULL;

    if (schdr_region.pa_base == new_start)
        return 0;

    // make a temp copy of current region
    tmp = malloc(schdr_region.mm_size);
    retcode = thread_memcpy(tmp, schdr_region.va_base, schdr_region.mm_size);
    unmap_region(&schdr_region);

    if(retcode == ERRCODE_NONE) {
        end = new_start + new_size;
        end = (end + 0xFFF) & ~0xFFF;
        schdr_region.pa_base = new_start & ~0xFFF;
        schdr_region.mm_size = end - schdr_region.pa_base;
        map_region(&schdr_region);

        // copy old data from tmp into new region
        char *src, *dst;
        if (schdr_region.mm_size >= backup.mm_size) {
            src = tmp;
            dst = schdr_region.va_base + (schdr_region.mm_size - backup.mm_size);
            retcode = thread_memcpy(dst, src, backup.mm_size);
        } else {
            src = tmp + (backup.mm_size - schdr_region.mm_size);
            dst = schdr_region.va_base;
            retcode = thread_memcpy(dst, src, schdr_region.mm_size);
        }
    }

    free(tmp);
    return 0;
}
