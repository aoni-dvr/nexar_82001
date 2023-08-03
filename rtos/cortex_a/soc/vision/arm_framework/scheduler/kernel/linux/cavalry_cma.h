/**
 *  @file cavalry_cma.h
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
 *  @details Definitions & Constants for cavalry CMA APIs
 *
 */

/*================================================================================================*/
/* Directly imported from ambarella/private/cavalry_drv git repository                            */
/*                                                                                                */
/* File source : "cavalry_cma.h"                                                                  */
/* File size   : 2122 bytes                                                                       */
/* File md5sum : ec4fccb712689ec9d33837443e53ffc2                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing: None                                                             */
/*================================================================================================*/

#ifndef __CAVALRY_CMA_H__
#define __CAVALRY_CMA_H__


struct cma_mmb {
    uint64_t owner;
    uint64_t phys_addr;
    void *virt_addr;
    uint64_t size;

    uint32_t cache_en : 1;
    uint32_t reserved_0: 31;

    uint32_t user_ref_cnt;

    struct list_head list;
};

struct cavalry_mem_info {
    unsigned long phys;
    void *virt;
    unsigned long size;
};

struct cavalry_cma_info {
    uint64_t phys;
    void *virt;
    uint64_t size;
};

struct ambarella_cavalry {
    struct device *dev;
    struct cavalry_mem_info cavalry_mem_info[CAVALRY_MEM_NUM];
    struct cavalry_cma_info cma_private;

    /* CMA */
    struct list_head mmb_list;
    struct list_head free_mmb_list;
    struct proc_dir_entry *cma_proc;
    struct mutex cavalry_cma_mutex;
    /*-= Added for integration with CVFLOW =----------------------------------^^^-*/
};
int cavalry_cma_init(struct ambarella_cavalry *cavalry);
void cavalry_cma_exit(struct ambarella_cavalry *cavalry);
int cavalry_cma_alloc_schdr(struct ambarella_cavalry *cavalry);

int cavalry_cma_alloc(struct ambarella_cavalry *cavalry, struct cavalry_mem *mem, uint64_t owner);
int cavalry_cma_free(struct ambarella_cavalry *cavalry, struct cavalry_mem *mem);
int cavalry_cma_reference(struct ambarella_cavalry *cavalry, void *arg);
int cavalry_cma_unreference(struct ambarella_cavalry *cavalry, void *arg);
int cavalry_cma_sync_cache(struct ambarella_cavalry *cavalry, struct cavalry_cache_mem *cache);
int cavalry_cma_get_usage(struct ambarella_cavalry *cavalry, struct cavalry_usage_mem *usage);
void cavalry_cma_dump_range(struct ambarella_cavalry *cavalry);

int cavalry_cma_p2v(struct ambarella_cavalry *cavalry, void **vppBuffer, uint64_t phyaddr, uint32_t *pCacheFlag);
int cavalry_cma_v2p(struct ambarella_cavalry *cavalry, uint64_t *pPhyAddr, void *vpBuffer, uint32_t *pCacheFlag);

#endif
