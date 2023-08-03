// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
*  Copyright (c) 2021 Ambarella International LP
*/

/*================================================================================================*/
/* Directly imported from ambarella/private/cavalry_drv git repository                            */
/*                                                                                                */
/* File source : "cavalry_cma.c"                                                                  */
/* File size   : 14201 bytes                                                                      */
/* File md5sum : 3d2a80020059785a2a5f0ee571614798                                                 */
/* Git refhash : 5ab455629964ee0db3dbbf1f50f76bf8f4cba2f8                                         */
/*                                                                                                */
/* Changes made while importing:                                                                  */
/*  + Added function for cavalry_cma_v2p()                                                        */
/*  + Added function for cavalry_cma_p2v()                                                        */
/*================================================================================================*/

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/chip.h>
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif
#include "cavalry_ioctl.h"
#include "ambacv_kal.h"
#include "ambacv_kernel.h"
#include "cache_kernel.h"
#include "cavalry_cma.h"

#if !defined(ENABLE_AMBA_MAL)

#define CMA_PROC_NAME "cavalry_cma"

#define DMA_ALLOC dma_alloc_wc
#define DMA_FREE dma_free_wc

static struct cma_mmb *mmb_alloc(struct ambarella_cavalry *cavalry)
{
    struct cma_mmb *mmb = NULL;

    if (!list_empty(&cavalry->free_mmb_list)) {
        mmb = list_first_entry(&cavalry->free_mmb_list, struct cma_mmb, list);
        list_del_init(&mmb->list);
    }

    if (mmb == NULL) {
        mmb = kmalloc(sizeof(struct cma_mmb), GFP_KERNEL);
        if (!mmb) {
            printk(KERN_ERR "kmalloc err\n");
            return NULL;
        }
    }

    memset(mmb, 0, sizeof(struct cma_mmb));
    INIT_LIST_HEAD(&mmb->list);

    return mmb;
}

static void mmb_add(struct ambarella_cavalry *cavalry, struct cma_mmb *mmb)
{
    struct cma_mmb *_mmb = NULL, *__mmb = NULL;

    if (!list_empty(&cavalry->mmb_list)) {
        list_for_each_entry_safe(_mmb, __mmb, &cavalry->mmb_list, list) {
            if (mmb->phys_addr < _mmb->phys_addr) {
                break;
            }
        }
        list_add(&mmb->list, _mmb->list.prev);
    } else {
        list_add_tail(&mmb->list, &cavalry->mmb_list);
    }
}

static void mmb_release(struct ambarella_cavalry *cavalry, struct cma_mmb *mmb)
{
    list_move(&mmb->list, &cavalry->free_mmb_list);
    mmb->virt_addr = NULL;
    mmb->phys_addr = 0UL;
    mmb->size = 0UL;
    mmb->owner = 0UL;
}

struct cma_mmb *cma_alloc(struct ambarella_cavalry *cavalry, uint64_t owner, uint64_t size,int cache_en)
{
    struct cma_mmb *mmb = NULL;
    void *virt = NULL;
    uint64_t phys = 0UL;
    uint64_t align_size = 0UL;

    mmb = mmb_alloc(cavalry);
    if (mmb == NULL) {
        printk(KERN_ERR "mmb_alloc err\n");
        return NULL;
    }

    align_size = PAGE_ALIGN(size);
    virt = DMA_ALLOC(cavalry->dev, align_size, &phys, GFP_KERNEL);
    if ((virt == NULL) || (phys == 0)) {
        printk(KERN_ERR "dma alloc size: 0x%llx err\n", align_size);
        //list_add_tail(&mmb->list, &cavalry->free_mmb_list);
        kfree(mmb); /* Free immediately avoid countinus err cause memory leak */
        mmb = NULL;
    } else {
        mmb->owner = owner;
        mmb->phys_addr = phys;
        mmb->virt_addr = virt;
        mmb->size = align_size;
        mmb->cache_en = !!cache_en;
        mmb->user_ref_cnt = 0;
        printk(KERN_DEBUG "phys: 0x%llx, size: 0x%llx, virt: %p\n",
               mmb->phys_addr, mmb->size, mmb->virt_addr);
        mmb_add(cavalry, mmb);
    }

    return mmb;
}

static int cma_free(struct ambarella_cavalry *cavalry, uint64_t phys_addr)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL;
    int found = 0, rval = 0;

    if (!list_empty(&cavalry->mmb_list)) {
        list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
            if ((phys_addr >= mmb->phys_addr) && (phys_addr < mmb->phys_addr + mmb->size)) {
                found = 1;
                break;
            }
        }
    }

    if (found) {
        if (mmb->user_ref_cnt == 0U) {
            DMA_FREE(cavalry->dev, mmb->size, mmb->virt_addr, mmb->phys_addr);
            printk(KERN_DEBUG "phys: 0x%llx, size: 0x%llx, virt: %p\n",
                   mmb->phys_addr, mmb->size, mmb->virt_addr);
            mmb_release(cavalry, mmb);
        }
    }

    return rval;
}

static unsigned long cma_get_max_contig_free(struct ambarella_cavalry *cavalry)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL, *prev_mmb = NULL;
    unsigned long max_free = 0, free = 0;

    if (!list_empty(&cavalry->mmb_list)) {
        /* Check the last mmb to end */
        mmb = list_last_entry(&cavalry->mmb_list, struct cma_mmb, list);
        max_free = krn_c2p(cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys) +
                   cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size -
                   (mmb->phys_addr + mmb->size);

        list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
            if (prev_mmb != NULL) {
                free = mmb->phys_addr - (prev_mmb->phys_addr + prev_mmb->size);
                if (free > max_free) {
                    max_free = free;
                }
            }
            prev_mmb = mmb;
        }
    } else {
        max_free = cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size;
    }

    return max_free;
}

static int cavalry_cma_proc_show(struct seq_file *m, void *v)
{
    struct ambarella_cavalry *cavalry = NULL;
    struct cavalry_cma_info *info = NULL;
    struct cma_mmb *mmb = NULL, *_mmb = NULL;
    uint64_t total_size = 0UL;
    unsigned long max_free = 0;
    uint32_t id = 0, free_cnt = 0;

    cavalry = (struct ambarella_cavalry *)m->private;
    info = &cavalry->cma_private;
    seq_printf(m, "mmb   phys[start-end]        size        kern-virt[start-end]"
               "                   ref_cnt  cache\n");

    if (info->virt) {
        seq_printf(m, "%04d  0x%llx-0x%llx  0x%llx  0x%p-0x%p\n", id++,
                   krn_c2p(info->phys), krn_c2p(info->phys + info->size), info->size,
                   info->virt, info->virt + info->size);
        total_size += info->size;
    }
    seq_printf(m, "----\n");

    mutex_lock(&cavalry->cavalry_cma_mutex);
    if (!list_empty(&cavalry->mmb_list)) {
        list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
            seq_printf(m, "%04d  0x%llx-0x%llx  0x%llx  0x%p-0x%p  %u        %u\n",
                       id++, mmb->phys_addr, mmb->phys_addr + mmb->size, mmb->size,
                       mmb->virt_addr, mmb->virt_addr + mmb->size,
                       mmb->user_ref_cnt, mmb->cache_en);
            total_size += mmb->size;
        }
    }
    if (!list_empty(&cavalry->free_mmb_list)) {
        list_for_each_entry_safe(mmb, _mmb, &cavalry->free_mmb_list, list) {
            free_cnt++;
        }
    }
    max_free = cma_get_max_contig_free(cavalry);
    mutex_unlock(&cavalry->cavalry_cma_mutex);

    seq_printf(m, "\nFree mmb list count: %u, max contiguous free size: 0x%lx\n",
               free_cnt, max_free);
    seq_printf(m, "Total used mmb size: 0x%llx, about %ldMB\n",
               total_size, total_size & 0xFFFFF ? (total_size >> 20) + 1 : (total_size >> 20));

    return 0;
}

static int cavalry_cma_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, cavalry_cma_proc_show, PDE_DATA(inode));
}

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
static const struct file_operations cavalry_cma_proc_fops = {
    .open = cavalry_cma_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
};
#else
static const struct proc_ops cavalry_cma_proc_fops = {
    .proc_open = cavalry_cma_proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
};
#endif

extern int    enable_arm_cache;
int cavalry_cma_init(struct ambarella_cavalry *cavalry)
{
    int rval = 0;

    mutex_init(&cavalry->cavalry_cma_mutex);
    INIT_LIST_HEAD(&cavalry->mmb_list);
    INIT_LIST_HEAD(&cavalry->free_mmb_list);

    do {
        cavalry->dev->coherent_dma_mask = DMA_BIT_MASK(32);
        if (!cavalry->dev->dma_mask) {
            cavalry->dev->dma_mask = &cavalry->dev->coherent_dma_mask;
        }

#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
        rval = of_dma_configure(cavalry->dev, cavalry->dev->of_node);
#else
        rval = of_dma_configure(cavalry->dev, cavalry->dev->of_node,true);
#endif
        if (rval < 0) {
            printk(KERN_ERR "of_dma_configure err: %d\n", rval);
            break;
        }

        /* If  enable_arm_cache == 1, mapping cacheable*/
        if(enable_arm_cache == 1) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
            cavalry->dev->archdata.dma_coherent = 1;
#else
#if (defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_DEVICE) || defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU) || defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU_ALL))
            cavalry->dev->dma_coherent = 1;
#endif /* (?CONFIG_ARCH_HAS_SYNC_DMA_FOR_DEVICE || ?CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU || ?CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) */
#endif
        }

        rval = of_reserved_mem_device_init(cavalry->dev);
        if (rval < 0) {
            printk(KERN_ERR "of_reserved_mem_device_init err: %d\n", rval);
            break;
        }

        cavalry->cma_proc = proc_create_data(CMA_PROC_NAME, S_IRUGO,
                                             get_ambarella_proc_dir(), &cavalry_cma_proc_fops, cavalry);
        if (cavalry->cma_proc == NULL) {
            printk(KERN_ERR "create proc file (cavalry_cma) err\n");
            rval = -ENOMEM;
            break;
        }
    } while (0);

    return rval;
}

int cavalry_cma_alloc_schdr(struct ambarella_cavalry *cavalry)
{
    void *virt = NULL;
    uint64_t phys = 0UL;
    int rval = 0;

    /* alloc prelloced mem */
    virt = dma_alloc_wc(cavalry->dev,
                        cavalry->cma_private.size, &phys, GFP_KERNEL);
    if ((virt == NULL) || (phys == 0)) {
        printk(KERN_ERR "dma alloc err\n");
        rval = -ENOMEM;
    } else {
        printk(KERN_DEBUG "phys: 0x%llx, size: 0x%llx, virt: %p\n",
               phys, cavalry->cma_private.size, virt);
        cavalry->cma_private.phys = krn_p2c(phys);
        cavalry->cma_private.virt = virt;
    }

    return rval;
}

void cavalry_cma_exit(struct ambarella_cavalry *cavalry)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL;

    if (cavalry->cma_proc) {
        remove_proc_entry(CMA_PROC_NAME, get_ambarella_proc_dir());
        cavalry->cma_proc = NULL;
    }

    mutex_lock(&cavalry->cavalry_cma_mutex);
    if (!list_empty(&cavalry->mmb_list)) {
        list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
            if (mmb->user_ref_cnt) {
                printk(KERN_ERR "phys: 0x%llx err since it has ref_cnt: %u\n",
                       mmb->phys_addr, mmb->user_ref_cnt);
            } else {
                cma_free(cavalry, mmb->phys_addr);
            }
        }
    }
    if (!list_empty(&cavalry->free_mmb_list)) {
        list_for_each_entry_safe(mmb, _mmb, &cavalry->free_mmb_list, list) {
            if (mmb) {
                list_del(&mmb->list);
                kfree(mmb);
            }
        }
    }
    mutex_unlock(&cavalry->cavalry_cma_mutex);

    /* Free prelloced mem */
    if (cavalry->cma_private.virt != NULL) {
        dma_free_wc(cavalry->dev, cavalry->cma_private.size,
                    cavalry->cma_private.virt, krn_c2p(cavalry->cma_private.phys));
        cavalry->cma_private.virt = NULL;
        cavalry->cma_private.size = 0;
        cavalry->cma_private.phys = 0;

        if (cavalry->dev) {
            of_reserved_mem_device_release(cavalry->dev);
        }
    }
}

int cavalry_cma_alloc(struct ambarella_cavalry *cavalry, struct cavalry_mem *mem, uint64_t owner)
{
    struct cma_mmb *mmb = NULL;
    unsigned long max_free = 0;
    int rval = 0;

    mutex_lock(&cavalry->cavalry_cma_mutex);

    do {
        if ((mem->length == 0) ||
            (mem->length > cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size)) {
            printk(KERN_ERR "invalid mem size: 0x%lx > 0x%lx\n",
                   mem->length, cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size);
            rval = -EINVAL;
            break;
        }

        mmb = cma_alloc(cavalry, owner, mem->length, mem->cache_en);
        if (mmb == NULL) {
            max_free = cma_get_max_contig_free(cavalry);
            if (mem->length > max_free) {
                printk(KERN_ERR "cma_alloc size large than max contig free: PAGE(0x%lx) > 0x%lx\n",
                       mem->length, max_free);
                rval = -ENOMEM;
            } else {
                printk(KERN_ERR "cma_alloc size: 0x%lx err\n", mem->length);
                rval = -EFAULT;
            }
            break;
        } else {
            mem->length = mmb->size;
            mem->offset = mmb->phys_addr;
        }
    } while (0);

    mutex_unlock(&cavalry->cavalry_cma_mutex);

    return rval;
}

int cavalry_cma_free(struct ambarella_cavalry *cavalry, struct cavalry_mem *mem)
{
    int rval = 0;

    mutex_lock(&cavalry->cavalry_cma_mutex);

    do {
        if ((mem->offset < krn_c2p(cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys)) ||
            (mem->offset > krn_c2p(cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys) +
             cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size)) {
            printk(KERN_ERR "out of range user mem phys: 0x%lx\n", mem->offset);
            rval = -EINVAL;
            break;
        }

        rval = cma_free(cavalry, mem->offset);
        if (rval < 0) {
            printk(KERN_ERR "cma_free err\n");
            rval = -EFAULT;
            break;
        }
    } while (0);

    mutex_unlock(&cavalry->cavalry_cma_mutex);

    return rval;
}

int cavalry_cma_reference(struct ambarella_cavalry *cavalry, void *arg)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL;
    int found = 0, rval = 0;

    mutex_lock(&cavalry->cavalry_cma_mutex);

    do {
        if (!list_empty(&cavalry->mmb_list)) {
            list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                if (mmb == arg) {
                    ++mmb->user_ref_cnt;
                    printk(KERN_DEBUG "phys: 0x%llx, ref_cnt: %u\n", mmb->phys_addr, mmb->user_ref_cnt);
                    found = 1;
                    break;
                }
            }
        }

        if (!found) {
            printk(KERN_ERR "mmb: 0x%lx not found\n", (uint64_t)arg);
            rval = -EINVAL;
        }
    } while (0);

    mutex_unlock(&cavalry->cavalry_cma_mutex);

    return rval;
}

int cavalry_cma_unreference(struct ambarella_cavalry *cavalry, void *arg)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL;
    int found = 0, rval = 0;

    mutex_lock(&cavalry->cavalry_cma_mutex);

    do {
        if (!list_empty(&cavalry->mmb_list)) {
            list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                if (mmb == arg) {
                    --mmb->user_ref_cnt;
                    printk(KERN_DEBUG "phys: 0x%llx, ref_cnt: %u\n", mmb->phys_addr, mmb->user_ref_cnt);
                    if (mmb->user_ref_cnt == 0U) {
                        DMA_FREE(cavalry->dev, mmb->size, mmb->virt_addr, mmb->phys_addr);
                        printk("phys: 0x%llx, size: 0x%llx, virt: %p\n",
                               mmb->phys_addr, mmb->size, mmb->virt_addr);
                        mmb_release(cavalry, mmb);
                    }
                    found = 1;
                    break;
                }
            }
        }

        if (!found) {
            printk(KERN_ERR "mmb: 0x%lx not found\n", (uint64_t)arg);
            rval = -EINVAL;
        }
    } while (0);

    mutex_unlock(&cavalry->cavalry_cma_mutex);

    return rval;
}

int cavalry_cma_sync_cache(struct ambarella_cavalry *cavalry, struct cavalry_cache_mem *cache)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL;
    unsigned long offset = 0, remain_sz = 0;
    int found = 0, rval = 0;

    mutex_lock(&cavalry->cavalry_cma_mutex);

    do {
        if ((cache->offset < krn_c2p(cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys)) ||
            (cache->offset > krn_c2p(cavalry->cavalry_mem_info[CAVALRY_MEM_USER].phys) +
             cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size)) {
            printk(KERN_ERR "out of range user cache mem phys: 0x%lx\n", cache->offset);
            rval = -EINVAL;
            break;
        }

        if (!list_empty(&cavalry->mmb_list)) {
            list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                if ((mmb->phys_addr <= cache->offset) &&
                    (cache->offset < mmb->phys_addr + mmb->size)) {
                    found = 1;
                    break;
                }
            }
        }

        if (found) {
            if (mmb->cache_en == 0) {
                printk(KERN_ERR "no need sync on non-cached mem\n");
                rval = -EINVAL;
                break;
            }
            offset = cache->offset - mmb->phys_addr;
            remain_sz = mmb->size - offset;
            if (cache->length > remain_sz) {
                printk(KERN_ERR "cache[0x%lx, 0x%lx] out range[0x%llx, 0x%llx]\n",
                       cache->offset, cache->offset + cache->length,
                       mmb->phys_addr, mmb->phys_addr + mmb->size);
                rval = -EINVAL;
                break;
            }
            if (cache->clean) {
                ambcache_clean_range(mmb->virt_addr + offset, cache->length);
            }
            if (cache->invalid) {
                ambcache_inv_range(mmb->virt_addr + offset, cache->length);
            }
        } else {
            printk(KERN_ERR "phys: 0x%lx sync mem not found\n", cache->offset);
            rval = -EINVAL;
        }
    } while (0);

    mutex_unlock(&cavalry->cavalry_cma_mutex);

    return rval;
}

int cavalry_cma_get_usage(struct ambarella_cavalry *cavalry, struct cavalry_usage_mem *usage)
{
    struct cma_mmb *mmb = NULL, *_mmb = NULL;
    int rval = 0;

    usage->used_length = 0;
    mutex_lock(&cavalry->cavalry_cma_mutex);

    do {
        if (!list_empty(&cavalry->mmb_list)) {
            list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                usage->used_length += mmb->size;
            }
        }
        usage->free_length = cavalry->cavalry_mem_info[CAVALRY_MEM_USER].size -
                             usage->used_length;
    } while (0);

    mutex_unlock(&cavalry->cavalry_cma_mutex);

    return rval;
}

/*================================================================================================*/
/* CVFLOW specific : start                                                                        */
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

int cavalry_cma_p2v(struct ambarella_cavalry *cavalry, void **vppBuffer, uint64_t phyaddr, uint32_t *pCacheFlag)
{
    int rval;
    if ((cavalry == NULL) || (vppBuffer == NULL) || (phyaddr == 0) || (pCacheFlag == NULL)) {
        rval = -EINVAL;
    } else {
        mutex_lock(&cavalry->cavalry_cma_mutex);
        do {
            struct cma_mmb *mmb = NULL, *_mmb = NULL;
            int found = 0;

            if (!list_empty(&cavalry->mmb_list)) {
                list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                    if ((mmb->phys_addr <= phyaddr) && (phyaddr < (mmb->phys_addr + mmb->size))) {
                        found = 1;
                        break;
                    }
                }
            }
            if (found) {
                unsigned long offset;
                rval        = 0;
                offset      = phyaddr - mmb->phys_addr;
                *vppBuffer  = (void *)(mmb->virt_addr + offset);
                *pCacheFlag = mmb->cache_en;
            } else {
                printk(KERN_ERR "phys: 0x%llx sync mem not found\n", phyaddr);
                rval = -EINVAL;
            }
        } while (0);
        mutex_unlock(&cavalry->cavalry_cma_mutex);
    }
    return rval;
}

int cavalry_cma_v2p(struct ambarella_cavalry *cavalry, uint64_t *pPhyAddr, void *vpBuffer, uint32_t *pCacheFlag)
{
    int rval;
    if ((cavalry == NULL) || (vpBuffer == NULL) || (pPhyAddr == NULL) || (pCacheFlag == NULL)) {
        rval = -EINVAL;
    } else {
        mutex_lock(&cavalry->cavalry_cma_mutex);
        do {
            struct cma_mmb *mmb = NULL, *_mmb = NULL;
            int found = 0;
            if (!list_empty(&cavalry->mmb_list)) {
                list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                    if ((mmb->virt_addr <= vpBuffer) && (vpBuffer < (mmb->virt_addr + mmb->size))) {
                        found = 1;
                        break;
                    }
                }
            }
            if (found) {
                unsigned long offset;
                rval        = 0;
                offset      = vpBuffer - mmb->virt_addr;
                *pPhyAddr   = mmb->phys_addr + offset;
                *pCacheFlag = mmb->cache_en;
            } else {
                printk(KERN_ERR "virt: %p sync mem not found\n", vpBuffer);
                rval = -EINVAL;
            }
        } while (0);
        mutex_unlock(&cavalry->cavalry_cma_mutex);
    }
    return rval;
}

void cavalry_cma_dump_range(struct ambarella_cavalry *cavalry)
{
    if (cavalry != NULL) {
        mutex_lock(&cavalry->cavalry_cma_mutex);
        do {
            struct cma_mmb *mmb = NULL, *_mmb = NULL;
            if (!list_empty(&cavalry->mmb_list)) {
                list_for_each_entry_safe(mmb, _mmb, &cavalry->mmb_list, list) {
                    printk("[AMBACV] CMA ALLOC     :   [0x%llx--0x%llx] @ (%p)\n", mmb->phys_addr, mmb->phys_addr + mmb->size, mmb->virt_addr);
                }
            }
        } while (0);
        mutex_unlock(&cavalry->cavalry_cma_mutex);
    }
}
#endif

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/* CVFLOW specific : end                                                                          */
/*================================================================================================*/

