/**
 * @file amba_metal.h
 *
 * @copyright Copyright (c) 2022 Ambarella, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef AMBA_METAL_H
#define AMBA_METAL_H

#include <metal/sys.h>


#if defined __cplusplus
extern "C" {
#endif

int amba_init_metal_system(void);
void amba_cleanup_metal_system(void);

#if defined __cplusplus
}
#endif

#endif /* AMBA_METAL_H_ */

