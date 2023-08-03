/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/alloc.c
 * @brief	generic libmetal memory allocattion definitions.
 */

#ifndef __METAL_ALLOC__H__
#error "Include metal/alloc.h instead of metal/generic/alloc.h"
#endif

#ifndef __METAL_GENERIC_ALLOC__H__
#define __METAL_GENERIC_ALLOC__H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_CC_USESTD
/* cf. AmbaLinkPrivate.h */
#define MEM_POOL_LIBMETAL_BUF_e	(18u)
extern void *AmbaLink_Malloc(unsigned int Num, unsigned int Size);
extern void AmbaLink_Free(unsigned int Num, void *pPtr);
#endif

static inline void *metal_allocate_memory(unsigned int size)
{
#ifdef CONFIG_CC_USESTD
	return (malloc(size));
#else
	return AmbaLink_Malloc(MEM_POOL_LIBMETAL_BUF_e, size);
#endif
}

static inline void metal_free_memory(void *ptr)
{
#ifdef CONFIG_CC_USESTD
	free(ptr);
#else
	AmbaLink_Free(MEM_POOL_LIBMETAL_BUF_e, ptr);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_GENERIC_ALLOC__H__ */
