/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/mutex.h
 * @brief	Generic mutex primitives for libmetal.
 */

#ifndef __METAL_MUTEX__H__
#error "Include metal/mutex.h instead of metal/generic/mutex.h"
#endif

#ifndef __METAL_GENERIC_MUTEX__H__
#define __METAL_GENERIC_MUTEX__H__

#ifdef CONFIG_THREADX
#include <AmbaKAL.h>
#else
#include <metal/atomic.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_THREADX
typedef AMBA_KAL_MUTEX_t metal_mutex_t;

#else // Other OS
typedef struct {
	atomic_int v;
} metal_mutex_t;
#endif

#ifdef CONFIG_THREADX
static inline void __metal_mutex_init(metal_mutex_t *mutex)
{
	UINT32 ret;

	ret = AmbaKAL_MutexCreate(mutex, NULL);
	if (ret) {
		// TODO
	}

	return;
}

static inline void __metal_mutex_deinit(metal_mutex_t *mutex)
{
	UINT32 ret;

	ret = AmbaKAL_MutexDelete(mutex);
	if (ret) {
		// TODO
		(void)ret;
	}

	return;
}

static inline int __metal_mutex_try_acquire(metal_mutex_t *mutex)
{
	UINT32 ret;

	ret = AmbaKAL_MutexTake(mutex, 10);
	if (ret) {
		// not acquired
		ret = 0;
	} else {
		// acquired
		ret = 1;
	}

	return ret;
}

static inline void __metal_mutex_acquire(metal_mutex_t *mutex)
{
	UINT32 ret;

	ret = AmbaKAL_MutexTake(mutex, KAL_WAIT_FOREVER);
	if (ret) {
		// TODO
		(void)ret;
	}

	return;
}

static inline void __metal_mutex_release(metal_mutex_t *mutex)
{
	UINT32 ret;

	ret = AmbaKAL_MutexGive(mutex);
	if (ret) {
		// TODO
		(void)ret;
	}

	return;
}

static inline int __metal_mutex_is_acquired(metal_mutex_t *mutex)
{
	AMBA_KAL_MUTEX_INFO_s MutexInfo;
	UINT32 ret;

	ret = AmbaKAL_MutexQuery(mutex, &MutexInfo);
	if (ret) {
		// TODO
		ret = 1;
	} else {
		if (MutexInfo.NumWaitTask != 0u) {
			ret = 1;
		}
	}

	return ret;
}

#else // Other OS
/*
 * METAL_MUTEX_INIT - used for initializing an mutex elmenet in a static struct
 * or global
 */
#define METAL_MUTEX_INIT(m) { ATOMIC_VAR_INIT(0) }
/*
 * METAL_MUTEX_DEFINE - used for defining and initializing a global or
 * static singleton mutex
 */
#define METAL_MUTEX_DEFINE(m) metal_mutex_t m = METAL_MUTEX_INIT(m)

static inline void __metal_mutex_init(metal_mutex_t *mutex)
{
	atomic_store(&mutex->v, 0);
}

static inline void __metal_mutex_deinit(metal_mutex_t *mutex)
{
	(void)mutex;
}

static inline int __metal_mutex_try_acquire(metal_mutex_t *mutex)
{
	return 1 - atomic_flag_test_and_set(&mutex->v);
}

static inline void __metal_mutex_acquire(metal_mutex_t *mutex)
{
	while (atomic_flag_test_and_set(&mutex->v)) {
		;
	}
}

static inline void __metal_mutex_release(metal_mutex_t *mutex)
{
	atomic_flag_clear(&mutex->v);
}

static inline int __metal_mutex_is_acquired(metal_mutex_t *mutex)
{
	return atomic_load(&mutex->v);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __METAL_GENERIC_MUTEX__H__ */
