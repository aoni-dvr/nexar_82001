#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h> //for mutex
#include <sys/ioctl.h>
#include "AmbaMAL.h"
#include "mem_util.h"
#include "debug_msg.h"

typedef struct {
    uint32_t id;
    uint32_t cached;
    uint32_t need_unmap;
    void *extra_data;
    uint8_t *virt_base;
    uint8_t *virt_target;
    uint32_t virt_size;
    unsigned long phy_addr;
    unsigned long phy_size;
} memutil_region_s;

typedef struct mmap_data_node_s_{
    memutil_region_s info;
    struct mmap_data_node_s_ *prev;
    struct mmap_data_node_s_ *next;
} mmap_data_node_s;

static int MemUtil_Inited = 0;
static uint32_t g_EnableCached = 0;

static mmap_data_node_s *List_Head = NULL, *List_End = NULL;
static pthread_mutex_t List_mutex=PTHREAD_MUTEX_INITIALIZER;

static uint32_t ListAdd(memutil_region_s *info)
{
    mmap_data_node_s *node_new;
    uint32_t rval = 0;

    pthread_mutex_lock(&List_mutex);

    node_new = (mmap_data_node_s *)malloc(sizeof(mmap_data_node_s));
    if (node_new == NULL) {
        printf("[%s] Fail to create mmap_data_node!\n", __func__);
        rval = 1;
    } else {
        memset(node_new, 0, sizeof(mmap_data_node_s));
        memcpy(&node_new->info, info, sizeof(memutil_region_s));

        if (List_Head == NULL) {
            List_Head = node_new;
            List_End = List_Head;
        } else {
            node_new->prev = List_End;
            List_End->next = node_new;
            List_End = node_new;
        }

        DBG_MSG("[%s] node=%p, id=%u, phy=0x%lx, virt=%p, need_unmap=%u\n", __func__,
            node_new, node_new->info.id, node_new->info.phy_addr, node_new->info.virt_target, node_new->info.need_unmap);
    }

    pthread_mutex_unlock(&List_mutex);

    return rval;
}

static mmap_data_node_s *ListGetHead(void)
{
    return List_Head;
}

static mmap_data_node_s *ListSearch(void *virt_target)
{
    mmap_data_node_s *curr;
    uint8_t *base, *limit, *target;
    int found = 0;

    pthread_mutex_lock(&List_mutex);
    target = (uint8_t *)virt_target;
    curr = List_Head;
    while(!found) {
        if (curr == NULL) {
            break;
        }

        base = curr->info.virt_base;
        limit = base + curr->info.virt_size;

        if ((target >= base) && (target < limit)) {
            found = 1;
            break;
        }

        curr = curr->next;
    }
    pthread_mutex_unlock(&List_mutex);

    if (!found) {
        DBG_MSG("[%s] Cannot find %p in mmap list!\n", __func__, target);
        return NULL;
    }

    return curr;
}

static mmap_data_node_s *ListSearchPhy(unsigned long phy_addr)
{
    mmap_data_node_s *curr;
    unsigned long base, limit;
    int found = 0;

    pthread_mutex_lock(&List_mutex);
    curr = List_Head;
    while(!found) {
        if (curr == NULL) {
            break;
        }

        base = curr->info.phy_addr;
        limit = base + curr->info.phy_size;

        if ((phy_addr >= base) && (phy_addr < limit)) {
            found = 1;
            break;
        }

        curr = curr->next;
    }
    pthread_mutex_unlock(&List_mutex);

    if (!found) {
        DBG_MSG("[%s] Cannot find 0x%lx in mmap list!\n", __func__, phy_addr);
        return NULL;
    }

    return curr;
}

static uint32_t ListDel(mmap_data_node_s *node)
{
    if (node == NULL) {
        printf("[%s] node is NULL!\n", __func__);
        return 1;
    }

    pthread_mutex_lock(&List_mutex);

    if (List_Head == NULL) {
        DBG_MSG("[%s] List_Head is NULL!\n", __func__);
    } else {
        DBG_MSG("[%s]: node=%p, id=%u, phy=0x%lx, virt=%p\n",__func__,
            node, node->info.id, node->info.phy_addr, node->info.virt_target);

        if (node == List_Head) {
            if (List_Head == List_End) { //delete the only one
                List_Head = NULL;
                List_End = NULL;
            } else {
                List_Head = List_Head->next;
                if (List_Head != NULL) {
                    List_Head->prev = NULL;
                }
            }
        } else if (node == List_End) {
            List_End = List_End->prev;
            if (List_End != NULL) {
                List_End->next = NULL;
            }
        } else {
            mmap_data_node_s *node_prev, *node_next;

            node_prev = node->prev;
            node_next = node->next;

            if (node_prev != NULL) {
                node_prev->next = node_next;
            }
            if (node_next != NULL) {
                node_next->prev = node_prev;
            }
        }
        free(node);
    }
    pthread_mutex_unlock(&List_mutex);

    return 0;
}

/**
 * This function is to mmap all known non-alloc-able area.
 * User have to do this before using all other APIs.
 **/
uint32_t MemUtil_Init(uint32_t en_cache)
{
    uint32_t ret = 0;
    uint32_t i;
    uint64_t Attri = 0;
    void *pVirt = NULL;
    AMBA_MAL_INFO_s MalInfo = {0};

    if (MemUtil_Inited != 0) {
        return 0;
    }

    //map all AMBA_MAL_TYPE_DEDICATED region
    for (i = 0; i < AMBA_MAL_ID_MAX; i++) {
        ret = AmbaMAL_GetInfo(i, &MalInfo);
        if (ret != 0) {
            printf("[%s] AmbaMAL_GetInfo(%u) fail!\n", __func__, i);
            continue;
        } else if (MalInfo.Type != AMBA_MAL_TYPE_DEDICATED) {
            continue;
        }

        if (en_cache) {
            Attri = AMBA_MAL_ATTRI_CACHE;
        } else {
            Attri = AMBA_MAL_ATTRI_NON_CACHE;
        }

        ret = AmbaMAL_Map(MalInfo.Id, MalInfo.PhysAddr, MalInfo.Size, Attri, &pVirt);
        if ((ret != 0) || (pVirt == NULL)) {
            printf("[%s] AmbaMAL_Map(0x%x) fail. ret=0x%x, pVirt=%p\n",__func__, MalInfo.Id, ret, pVirt);
        } else {
            memutil_region_s info = {0};

            printf("[%s] AmbaMAL_Map done. ID[%u] PhysAddr=0x%llx, size=0x%llx map to pVirt(%p)\n",
                __func__, MalInfo.Id, MalInfo.PhysAddr, MalInfo.Size, pVirt);

            info.id = MalInfo.Id;
            info.cached = en_cache;
            info.need_unmap = 1;
            info.virt_base = (uint8_t *)pVirt;
            info.virt_target = (uint8_t *)pVirt;
            info.virt_size = MalInfo.Size;
            info.phy_addr = MalInfo.PhysAddr;
            info.phy_size = MalInfo.Size;

            ret = ListAdd(&info);
            if (ret != 0) {
                printf("[%s] ListAdd() fail. ret=0x%x\n",__func__, ret);
            }
        }
    }

    g_EnableCached = en_cache;

    if (ret == 0) {
        MemUtil_Inited = 1;
    }

    return ret;
}

/**
 * This function is to unmmap all known area.
 **/
void MemUtil_Release(void)
{
    uint32_t ret;
    mmap_data_node_s *node;
    memutil_region_s *info;

    if (MemUtil_Inited == 0) { //not inited
        return;
    }

    node = ListGetHead();
    while(node != NULL) {
        info = &(node->info);
        if(info->need_unmap) {
            ret = AmbaMAL_Unmap(info->id, info->virt_base, info->phy_size);
            if (ret != 0) {
                printf("[%s] AmbaMAL_Unmap(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
            }
        }

        ret = ListDel(node);
        if (ret != 0) {
            printf("[%s] ListDel(%p) fail. ret=0x%x\n", __func__, node, ret);
        }

        node = ListGetHead();
    }

    MemUtil_Inited = 0;
}

/**
 * This function is to get virtual address by specified physical address.
 **/
void *MemUtil_P2V(unsigned long phy_addr)
{
    int offset;
    mmap_data_node_s *node;
    uint8_t *ret = NULL;

    node = ListSearchPhy(phy_addr);
    if (node != NULL) {
        offset = phy_addr - node->info.phy_addr;
        ret = node->info.virt_target + offset;
    }

    return (void *)ret;
}

void *MemUtil_MalP2V(unsigned long phy_addr)
{
    void *rval = NULL;
    uint32_t ret;
    uint64_t Attri = 0;
    AMBA_MAL_INFO_s MalInfo = {0};

    if (g_EnableCached) {
        Attri = AMBA_MAL_ATTRI_CACHE;
    } else {
        Attri = AMBA_MAL_ATTRI_NON_CACHE;
    }

    ret = AmbaMAL_GetInfoByPhys(phy_addr, &MalInfo);
    if (ret != 0) {
        DBG_MSG("[%s] AmbaMAL_GetInfoByPhys() fail!\n", __func__);
        return NULL;
    } else if (MalInfo.Type == AMBA_MAL_TYPE_INVALID) {
        DBG_MSG("[%s] MalInfo.Type == AMBA_MAL_TYPE_INVALID!\n", __func__);
        return NULL;
    }

    ret = AmbaMAL_Phys2Virt(MalInfo.Id, phy_addr, Attri, &rval);
    if ((ret != 0) || (rval == NULL)) {
        DBG_MSG("[%s] AmbaMAL_Phys2Virt() fail!\n", __func__);
        return NULL;
    }

    return rval;
}

/**
 * This function is to get physical address by specified virtual address.
 **/
unsigned long MemUtil_V2P(void *virt_target)
{
    int offset;
    mmap_data_node_s *node;
    unsigned long ret = 0;

    node = ListSearch(virt_target);
    if (node != NULL) {
        offset = (uint8_t *)virt_target - node->info.virt_target;
        ret = node->info.phy_addr + offset;
    }

    return ret;
}

/**
 * This function is to do remap and get virtual address by specified physical address.
 **/
void *MemUtil_Remap(unsigned long phy_addr, uint32_t length)
{
    void *rval = NULL;
    uint32_t ret = 0;

    rval = MemUtil_P2V(phy_addr);
    if(rval == NULL) { //not exist in list, need to remap()
        memutil_region_s MemRegInfo = {0};
        AMBA_MAL_INFO_s MalInfo = {0};
        void *VirtAddr = NULL;
        uint64_t Attri = 0;

        //Get ID
        ret = AmbaMAL_GetInfoByPhys(phy_addr, &MalInfo);
        if (ret != 0) { //cannot find Mal region
            printf("[%s] unknown physical addr 0x%lx!\n",__func__, phy_addr);
            return NULL;
        }

        if (MalInfo.Type == AMBA_MAL_TYPE_INVALID) {
            printf("[%s] physical addr 0x%lx is in invalid region (%u)!\n",__func__, phy_addr, MalInfo.Id);
            return NULL;
        }

        //if ((phy_addr+length) > (MalInfo.PhysAddr + MalInfo.Size)) {
        //    printf("[%s] phy_addr(0x%lx)+length(0x%x) out of boundary (0x%llx, 0x%llx)\n",
        //        __func__, phy_addr, length, MalInfo.PhysAddr, MalInfo.Size);
        //    return NULL;
        //}

        if (g_EnableCached) {
            Attri = AMBA_MAL_ATTRI_CACHE;
        } else {
            Attri = AMBA_MAL_ATTRI_NON_CACHE;
        }

        //do map
        ret = AmbaMAL_Map(MalInfo.Id, phy_addr, length, Attri, &VirtAddr);
        if ((ret != 0) || (VirtAddr == NULL)) {
            printf("[%s] AmbaMAL_Map() fail! ret=0x%x, VirtAddr=%p\n",__func__, ret, VirtAddr);
            return NULL;
        }

        MemRegInfo.id = MalInfo.Id;
        MemRegInfo.cached = g_EnableCached;
        MemRegInfo.need_unmap = 1;
        MemRegInfo.virt_base = (uint8_t *)VirtAddr;
        MemRegInfo.virt_target = (uint8_t *)VirtAddr;
        MemRegInfo.virt_size = length;
        MemRegInfo.phy_addr = phy_addr;
        MemRegInfo.phy_size = length;

        ret = ListAdd(&MemRegInfo);
        if (ret != 0) {
            printf("[%s] Fail to ListAdd()\n", __func__);
            rval = NULL;
        } else {
            rval = VirtAddr;
            DBG_MSG("[%s] MemUtil_Remap(%lx)=%p\n", __func__, phy_addr, rval);
        }
    }

    return rval;
}

uint32_t MemUtil_Unmap(void *virt_target)
{
    mmap_data_node_s *node;
    uint32_t ret = 0;

    node = ListSearch(virt_target);
    if (node != NULL) {
        if (node->info.need_unmap) {
            memutil_region_s *info = &(node->info);

            ret = AmbaMAL_Unmap(info->id, info->virt_base, info->phy_size);
            if (ret != 0) {
                printf("[%s] AmbaMAL_Unmap(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
            } else {
                ret = ListDel(node);
            }
        }
    }

    return ret;
}

uint32_t MemUtil_UnmapPhy(unsigned long phy_addr)
{
    mmap_data_node_s *node;
    uint32_t ret = 0;

    node = ListSearchPhy(phy_addr);
    if (node != NULL) {
        if (node->info.need_unmap) {
            memutil_region_s *info = &(node->info);

            ret = AmbaMAL_Unmap(info->id, info->virt_base, info->phy_size);
            if (ret != 0) {
                printf("[%s] AmbaMAL_Unmap(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
            } else {
                ret = ListDel(node);
            }
        }
    }

    return ret;
}

/**
 * This function is to clean cache for the specified area. (cached data will be writen back)
 **/
void MemUtil_CacheClean(void *virt_target, uint32_t length)
{
    uint32_t ret;
    mmap_data_node_s *node;

    node = ListSearch(virt_target);
    if (node != NULL) {
        memutil_region_s *info = &(node->info);

        ret = AmbaMAL_CacheClean(info->id, virt_target, length);
        if (ret != 0) {
            printf("[%s] AmbaMAL_CacheClean(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
        }
    }
}

/**
 * This function is to drop cache for the specified area.
 * Please note, if there is any dirty cache line, i.e. any data in this cache area is modified,
 * this function will do write back and then drop cache. (behave like flush)
 **/
void MemUtil_CacheInvalid(void *virt_target, uint32_t length)
{
    uint32_t ret;
    mmap_data_node_s *node;

    node = ListSearch(virt_target);
    if (node != NULL) {
        memutil_region_s *info = &(node->info);

        ret = AmbaMAL_CacheInvalid(info->id, virt_target, length);
        if (ret != 0) {
            printf("[%s] AmbaMAL_CacheInvalid(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
        }
    }
}

/**
 * This function is to clean cache for the area specified with physical address. (cached data will be writen back)
 **/
void MemUtil_CacheCleanPhy(unsigned long phy_addr, uint32_t length)
{
    uint32_t ret;
    int offset;
    mmap_data_node_s *node;
    uint8_t *virt_target;

    node = ListSearchPhy(phy_addr);
    if (node != NULL) {
        memutil_region_s *info = &(node->info);

        offset = phy_addr - info->phy_addr;
        virt_target = info->virt_target + offset;

        ret = AmbaMAL_CacheClean(info->id, virt_target, length);
        if (ret != 0) {
            printf("[%s] AmbaMAL_CacheClean(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
        }
    }
}

/**
 * This function is to drop cache for the area specified with physical address.
 * Please note, if there is any dirty cache line, i.e. any data in this cache area is modified,
 * this function will do write back and then drop cache. (behave like flush)
 **/
void MemUtil_CacheInvalidPhy(unsigned long phy_addr, uint32_t length)
{
    uint32_t ret;
    int offset;
    mmap_data_node_s *node;
    uint8_t *virt_target;

    node = ListSearchPhy(phy_addr);
    if (node != NULL) {
        memutil_region_s *info = &(node->info);

        offset = phy_addr - info->phy_addr;
        virt_target = info->virt_target + offset;

        ret = AmbaMAL_CacheInvalid(info->id, virt_target, length);
        if (ret != 0) {
            printf("[%s] AmbaMAL_CacheInvalid(%p) fail. ret=0x%x\n", __func__, info->virt_base, ret);
        }
    }
}

