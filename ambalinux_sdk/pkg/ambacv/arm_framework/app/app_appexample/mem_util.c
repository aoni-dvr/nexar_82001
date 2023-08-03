#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h> //for mutex
#include <sys/ioctl.h>
#include "ppm.h"
#include "cvtask_ossrv.h"
#include "mem_util.h"

/**
 * This function is to create a memory block for CMA region.
 **/
uint32_t MemUtil_MemblkAlloc(uint32_t buf_size, flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemAlloc(buf_size, 1, buf);
    if(ret != 0) {
        printf("MemUtil_MemblkAlloc: AmbaCV_UtilityCmaMemAlloc fail. ret=%u\n", ret);
    }

    return ret;
}

/**
 * This function is to clean a memory block in cache for CMA region. (write back to DRAM)
 **/
uint32_t MemUtil_MemblkClean(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemClean(buf);
    if(ret != 0U) {
        printf("MemUtil_MemblkClean: AmbaCV_UtilityCmaMemClean fai. ret=%u\n", ret);
    }

    return ret;
}

/**
 * This function is to invalid a memory block in cache for CMA region. (drop cache)
 **/
uint32_t MemUtil_MemblkInvalid(flexidag_memblk_t* buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemInvalid(buf);
    if(ret != 0U) {
        printf("MemUtil_MemblkInvalid: AmbaCV_UtilityCmaMemInvalid fail. ret=%u\n", ret);
    }

    return ret;
}

/**
 * This function is to free a memory block for CMA region.
 **/
uint32_t MemUtil_MemblkFree(flexidag_memblk_t *buf)
{
    uint32_t ret = 0U;

    ret = AmbaCV_UtilityCmaMemFree(buf);
    if(ret != 0) {
        printf("MemUtil_MemblkFree: AmbaCV_UtilityCmaMemFree(%p) fail! ret=%u\n",buf,ret);
    }

    return ret;
}

typedef struct {
    uint8_t *virt_base;
    uint8_t *virt_target;
    uint32_t virt_size;
    unsigned long phy_addr;
    unsigned long phy_size;
    uint32_t cached;
} memutil_region_s;

typedef struct mmap_data_node_s_{
    memutil_region_s info;
    struct mmap_data_node_s_ *prev;
    struct mmap_data_node_s_ *next;
} mmap_data_node_s;

static mmap_data_node_s *List_Head = NULL, *List_End = NULL;
static pthread_mutex_t List_mutex=PTHREAD_MUTEX_INITIALIZER;

static uint32_t ListAdd(unsigned long phy_addr, unsigned long phy_size, void *base, void *target, uint32_t size, uint32_t cached)
{
    mmap_data_node_s *node_new;
    uint32_t rval = 0;

    pthread_mutex_lock(&List_mutex);

    node_new = (mmap_data_node_s *)malloc(sizeof(mmap_data_node_s));
    if (node_new == NULL) {
        printf("Fail to create mmap_data_node!\n");
        rval = 1;
    } else {
        memset(node_new, 0, sizeof(mmap_data_node_s));
        node_new->info.phy_addr = phy_addr;
        node_new->info.phy_size = phy_size;
        node_new->info.virt_base = base;
        node_new->info.virt_target = target;
        node_new->info.virt_size = size;
        node_new->info.cached = cached;

        if (List_Head == NULL) {
            List_Head = node_new;
            List_End = List_Head;
        } else {
            node_new->prev = List_End;
            List_End->next = node_new;
            List_End = node_new;
        }
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
        printf("[%s] Cannot find %p in mmap list!\n", __func__, target);
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
        printf("[%s] Cannot find 0x%lx in mmap list!\n", __func__, phy_addr);
        return NULL;
    }

    return curr;
}

static uint32_t ListDel(mmap_data_node_s *node)
{
    uint32_t rval = 0;

    if (node == NULL) {
        printf("ListDel(): node is NULL!\n");
        return 1;
    }

    pthread_mutex_lock(&List_mutex);

    if (List_Head == NULL) {
        printf("ListDel() when List_Head is NULL!\n");
        rval = 1;
    } else {
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

static int mem_fd = -1;
static memutil_region_s mem_region[3] = {0};
static int MemUtil_Inited = 0;

/**
 * This function is to mmap all known area.
 * User have to do this before using all other APIs.
 **/
uint32_t MemUtil_Init(uint32_t en_cache)
{
    uint32_t ret = 0, desc_id;
    int err, res;
    unsigned long aligned_addr, aligned_size;
    char *mmap_addr, *target_addr;
    struct PPM_MEM_INFO_s mem_info = {0};

    if (MemUtil_Inited != 0) {
        return 0;
    }

    if (mem_fd < 0) {
        if (en_cache == 0) {
            mem_fd = open("/dev/ppm", O_SYNC | O_RDWR);
        } else {
            mem_fd = open("/dev/ppm", O_RDWR);
        }
        if (mem_fd < 0) {
            printf("Cannot open device file /dev/ppm !!!\n\n");
            return 1;
        }
    }

//init CV_RTOS_MEM area
    err = ioctl(mem_fd, PPM_PREPARE_CVRTOS, &res);
    if (res != 0) {
        printf("[%s] Fail to do PPM_PREPARE_CVRTOS\n", __func__);
    }

//mmap PPM
    err = ioctl(mem_fd, PPM_GET_INFO_PPM, &mem_info);
    if (err) {
        printf("[%s] Fail to do PPM_GET_INFO_PPM\n", __func__);
        return 2;
    }
    if ((mem_info.phys != 0) && (mem_info.size != 0)) {
        ret = 0;
        if ((mem_info.phys & 0xfff) != 0) {
            ret++;
        }
        aligned_addr = mem_info.phys;
        aligned_addr >>= 12;
        aligned_addr <<= 12;

        if ((mem_info.size & 0xfff) != 0) {
            ret++;
        }
        aligned_size = mem_info.size;
        aligned_size >>= 12;
        aligned_size += ret;
        aligned_size <<= 12;

        mmap_addr = (char*)mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, aligned_addr);
        if (mmap_addr == MAP_FAILED) {
            printf("fail to do mmap(), phy_addr=0x%08x(aligned:0x%lx), size=0x%08x(aligned:0x%lx)\n",
                mem_info.phys, aligned_addr, mem_info.size, aligned_size);
            return 3;
        } else {
#if DEBUG
            printf("PPM region info: virt=0x%lx, phy_addr=0x%08x(aligned:0x%lx), size=0x%08x(aligned:0x%lx)\n",
                mmap_addr, mem_info.phys, aligned_addr, mem_info.size, aligned_size);
#endif
            target_addr = mmap_addr + (mem_info.phys - aligned_addr);

            ret = ListAdd(mem_info.phys, mem_info.size, mmap_addr, target_addr, aligned_size, en_cache);
            if (ret != 0) {
                printf("Fail to ListAdd()\n");
                return 4;
            }
        }
    }

//mmap PPM2
    mem_info.phys = 0;
    mem_info.size = 0;
    err = ioctl(mem_fd, PPM_GET_INFO_PPM2, &mem_info);
    if (err) {
        printf("[%s] Fail to do PPM_GET_INFO_PPM2\n", __func__);
        return 2;
    }
    if ((mem_info.phys != 0) && (mem_info.size != 0)) {
        ret = 0;
        if ((mem_info.phys & 0xfff) != 0) {
            ret++;
        }
        aligned_addr = mem_info.phys;
        aligned_addr >>= 12;
        aligned_addr <<= 12;

        if ((mem_info.size & 0xfff) != 0) {
            ret++;
        }
        aligned_size = mem_info.size;
        aligned_size >>= 12;
        aligned_size += ret;
        aligned_size <<= 12;

        mmap_addr = (char*)mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, aligned_addr);
        if (mmap_addr == MAP_FAILED) {
            printf("fail to do mmap(), phy_addr=0x%08x(aligned:0x%lx), size=0x%08x(aligned:0x%lx)\n",
                mem_info.phys, aligned_addr, mem_info.size, aligned_size);
            return 3;
        } else {
#if DEBUG
            printf("PPM2 region info: virt=0x%lx, phy_addr=0x%08x(aligned:0x%lx), size=0x%08x(aligned:0x%lx)\n",
                mmap_addr, mem_info.phys, aligned_addr, mem_info.size, aligned_size);
#endif
            target_addr = mmap_addr + (mem_info.phys - aligned_addr);

            ret = ListAdd(mem_info.phys, mem_info.size, mmap_addr, target_addr, aligned_size, en_cache);
            if (ret != 0) {
                printf("Fail to ListAdd()\n");
                return 4;
            }
        }
    }

//mmap CVRTOS
    mem_info.phys = 0;
    mem_info.size = 0;
    err = ioctl(mem_fd, PPM_GET_INFO_CVRTOS, &mem_info);
    if (err) {
        printf("[%s] Fail to do PPM_GET_INFO_CVRTOS\n", __func__);
        return 2;
    }
    if ((mem_info.phys != 0) && (mem_info.size != 0)) {
        ret = 0;
        if ((mem_info.phys & 0xfff) != 0) {
            ret++;
        }
        aligned_addr = mem_info.phys;
        aligned_addr >>= 12;
        aligned_addr <<= 12;

        if ((mem_info.size & 0xfff) != 0) {
            ret++;
        }
        aligned_size = mem_info.size;
        aligned_size >>= 12;
        aligned_size += ret;
        aligned_size <<= 12;

        mmap_addr = (char*)mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, aligned_addr);
        if (mmap_addr == MAP_FAILED) {
            printf("fail to do mmap(), phy_addr=0x%08x(aligned:0x%lx), size=0x%08x(aligned:0x%lx)\n",
                mem_info.phys, aligned_addr, mem_info.size, aligned_size);
            return 3;
        } else {
#if DEBUG
            printf("CVRTOS region info: virt=0x%lx, phy_addr=0x%08x(aligned:0x%lx), size=0x%08x(aligned:0x%lx)\n",
                mmap_addr, mem_info.phys, aligned_addr, mem_info.size, aligned_size);
#endif
            target_addr = mmap_addr + (mem_info.phys - aligned_addr);

            ret = ListAdd(mem_info.phys, mem_info.size, mmap_addr, target_addr, aligned_size, en_cache);
            if (ret != 0) {
                printf("Fail to ListAdd()\n");
                return 4;
            }
        }
    }

    MemUtil_Inited = 1;
    return 0;
}

/**
 * This function is to unmmap all known area.
 **/
void MemUtil_Release(void)
{
    int ret;
    mmap_data_node_s *node;

    node = ListGetHead();
    while (node != NULL) {
        ret = munmap(node->info.virt_base, node->info.virt_size);
        if (ret != 0) {
            perror("[[MemUtil_Release]] ");
        }

        ret = ListDel(node);
        if (ret != 0) {
            printf("Fail to ListDel()\n");
        }

        node = ListGetHead();
    }

    if (mem_fd >= 0) {
        close(mem_fd);
        mem_fd = -2;
    }
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
 * This function is to clean cache for the specified area. (cached data will be writen back)
 **/
void MemUtil_CacheClean(void *virt_target, uint32_t length)
{
    int offset, ret;
    mmap_data_node_s *node;
    unsigned long phy_target = 0;
    struct PPM_MEM_INFO_s mem_info = {0};

    node = ListSearch(virt_target);
    if (node != NULL) {
        offset = (uint8_t *)virt_target - node->info.virt_target;
        mem_info.phys = node->info.phy_addr + offset;
        mem_info.size = length;

        ret = ioctl(mem_fd, PPM_CACHE_CLEAN, &mem_info);
        if (ret != 0) {
            printf("[Error] PPM_CACHE_CLEAN returns %d\n", ret);
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
    int offset, ret;
    mmap_data_node_s *node;
    unsigned long phy_target = 0;
    struct PPM_MEM_INFO_s mem_info = {0};

    node = ListSearch(virt_target);
    if (node != NULL) {
        offset = (uint8_t *)virt_target - node->info.virt_target;
        mem_info.phys = node->info.phy_addr + offset;
        mem_info.size = length;

        ret = ioctl(mem_fd, PPM_CACHE_INVALID, &mem_info);
        if (ret != 0) {
            printf("[Error] PPM_CACHE_INVALID returns %d\n", ret);
        }
    }
}

/**
 * This function is to clean cache for the area specified with physical address. (cached data will be writen back)
 **/
void MemUtil_CacheCleanPhy(unsigned long phy_addr, uint32_t length)
{
    int ret;
    mmap_data_node_s *node;
    struct PPM_MEM_INFO_s mem_info = {0};

    node = ListSearchPhy(phy_addr);
    if (node != NULL) {
        mem_info.phys = phy_addr;
        mem_info.size = length;

        ret = ioctl(mem_fd, PPM_CACHE_CLEAN, &mem_info);
        if (ret != 0) {
            printf("[Error] PPM_CACHE_CLEAN returns %d\n", ret);
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
    int ret;
    mmap_data_node_s *node;
    struct PPM_MEM_INFO_s mem_info = {0};

    node = ListSearchPhy(phy_addr);
    if (node != NULL) {
        mem_info.phys = phy_addr;
        mem_info.size = length;

        ret = ioctl(mem_fd, PPM_CACHE_INVALID, &mem_info);
        if (ret != 0) {
            printf("[Error] PPM_CACHE_INVALID returns %d\n", ret);
        }
    }
}

