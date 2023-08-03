/**
 *  @file dram_mmap.h
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
 *  @details Definitions & Constants for dram sync
 *
 */

#ifndef DAG_DRAM_MMAP_H
#define DAG_DRAM_MMAP_H

#include "ambint.h"
#if defined(ENABLE_AMBA_MAL)
#include "AmbaMAL.h"
#endif

typedef struct {
    char*     va_base;
    uint64_t  pa_base;
    uint64_t  mm_size;
} dram_region_t;

extern dram_region_t       schdr_region;
extern dram_region_t       schdr_cma_region;
extern dram_region_t       schdr_rtos_region;
extern dram_region_t       schdr_kregion;

#if defined(ENABLE_AMBA_MAL)
extern AMBA_MAL_INFO_s     schdr_core_info;
extern AMBA_MAL_INFO_s     schdr_cma_info;
extern AMBA_MAL_INFO_s     schdr_rtos_info;
extern AMBA_MAL_INFO_s     dsp_data_info;
#endif

uint32_t map_region(dram_region_t *r);
void unmap_region(dram_region_t *r);
uint32_t cvtask_mmap_init(uint64_t start, uint64_t end);
uint32_t cvtask_mmap_remap(uint64_t new_start);
uint32_t schdr_cma_mmap_init(uint64_t start, uint64_t end);
uint32_t schdr_rtos_mmap_init(uint64_t start, uint64_t end);
uint32_t schdr_mmap_init(uint64_t start, uint64_t size);
uint32_t schdr_shutdown_unmap(void);

void* ambacv_p2v(uint64_t pa);
uint64_t ambacv_v2p(void *va);
uint64_t ambacv_p2c(uint64_t pa);
uint64_t ambacv_c2p(uint64_t ca);
void* ambacv_c2v(uint64_t ca);
uint64_t ambacv_v2c(void *va);
uint32_t ambacv_att_init(void);

#endif //DAG_DRAM_MMAP_H