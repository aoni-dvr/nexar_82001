/**
 *  @file cache_kernel.h
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
 *  @details Definitions & Constants for cache kernel
 *
 */

#ifndef CACHE_KERNEL_H_FILE
#define CACHE_KERNEL_H_FILE

#include "ambacv_kal.h"

typedef struct {
    char     *pBuffer;
    char     *pBuffer_end;
    uint64_t  buffer_daddr;
    uint64_t  buffer_daddr_end;
    uint64_t  buffer_size;        /* Explicitly allow 4GB-1 max DRAM size */
    uint32_t  buffer_cacheable;
} kernel_memblk_t;

extern cvprintf_buffer_info_t *krn_print_info;

void * krn_p2v(uint64_t phy);
uint64_t krn_v2p(void *virt);
uint64_t krn_p2c(uint64_t pa);
uint64_t krn_c2p(uint64_t ca);
void * krn_c2v(uint64_t ca);
uint64_t krn_v2c(void *virt);
void  krn_mmap_add_master(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable);
void  krn_mmap_add_rtos(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable);
void  krn_mmap_add_dsp_data(void *virt, uint64_t phy, uint64_t size, uint32_t is_cacheable);
uint32_t krn_cache_invld_schdrmsg(armvis_msg_t *msg, uint32_t *changed);
uint32_t krn_cache_clean_schdrmsg(armvis_msg_t *msg);
uint32_t krn_cache_ioctl_clean(const ambacv_mem_region_t *arg);
uint32_t krn_cache_ioctl_invld(const ambacv_mem_region_t *arg);
#endif /* ?CACHE_KERNEL_H_FILE */

