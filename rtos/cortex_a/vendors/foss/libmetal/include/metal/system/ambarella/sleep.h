/*
 * Copyright (c) 2018, Linaro Limited. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/sleep.h
 * @brief	Generic sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/generic/sleep.h"
#endif

#ifndef __METAL_GENERIC_SLEEP__H__
#define __METAL_GENERIC_SLEEP__H__

#include <metal/utilities.h>

#ifdef CONFIG_THREADX
#include <AmbaKAL.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline int __metal_sleep_usec(unsigned int usec)
{
	/* Fix me: RPMSG us msec, but thhis API use usrc */
#ifdef CONFIG_THREADX
	AmbaKAL_TaskSleep(usec);
#endif

	return 0;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_GENERIC_SLEEP__H__ */
