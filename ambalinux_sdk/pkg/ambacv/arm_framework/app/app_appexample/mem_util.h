/*
 * mem_util.h
 *
 * Copyright 2019 Ambarella Inc.
 *
 */

#ifndef _MEM_UTIL_H
#define _MEM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cvapi_ambacv_flexidag.h"

#define CACHE_ALIGN(x)              ((x + 0x3F)& ~0x3F)

/* For CMA operations */
uint32_t MemUtil_MemblkAlloc(uint32_t buf_size, flexidag_memblk_t* buf);
uint32_t MemUtil_MemblkClean(flexidag_memblk_t* buf);
uint32_t MemUtil_MemblkInvalid(flexidag_memblk_t* buf);
uint32_t MemUtil_MemblkFree(flexidag_memblk_t *buf);

/* For PPM/PPM2/CV_RTOS_USER operations */
uint32_t MemUtil_Init(uint32_t en_cache);
void MemUtil_Release(void);
void *MemUtil_P2V(unsigned long phy_addr);
unsigned long MemUtil_V2P(void *virt_target);
void MemUtil_CacheClean(void *virt_target, uint32_t length);
void MemUtil_CacheInvalid(void *virt_target, uint32_t length);
void MemUtil_CacheCleanPhy(unsigned long phy_addr, uint32_t length);
void MemUtil_CacheInvalidPhy(unsigned long phy_addr, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif	/* _MEM_UTIL_H */
