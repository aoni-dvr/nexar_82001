/*
 * Copyright (c) 2018, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	assert.h
 * @brief	Generic assertion support.
 */

#ifndef __METAL_ASSERT__H__
#error "Include metal/assert.h instead of metal/generic/assert.h"
#endif

#ifndef __METAL_GENERIC_ASSERT__H__
#define __METAL_GENERIC_ASSERT__H__

#include <assert.h>

/**
 * @brief Assertion macro for bare-metal applications.
 * @param cond Condition to evaluate.
 */
#ifdef CONFIG_CC_USESTD
#define metal_sys_assert(cond) assert(cond)
#else
#define metal_sys_assert(cond) { if (!(cond)) { __asm__ volatile ("b ."); } }
#endif


#endif /* __METAL_GENERIC_ASSERT__H__ */

