/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/shmem.c
 * @brief	Generic libmetal shared memory handling.
 */

#include <metal/shmem.h>

int metal_shmem_open(const char *name, size_t size,
		     struct metal_io_region **io)
{
#if 0 // generic
	return metal_shmem_open_generic(name, size, io);
#else // TODO
    (void)name;
    (void)size;
    (void)io;
    return 0;
#endif
}
