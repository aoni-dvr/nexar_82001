/**
 * @file amba_metal.c
 *
 * @copyright Copyright (c) 2020 Ambarella, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/sys.h>


int amba_init_metal_system(void)
{
    static int init                      = 0;
    struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

    if (init == 0) {
        init = 1;
    } else {
        return 0;
    }
    metal_param.log_level = METAL_LOG_DEBUG;
    metal_init(&metal_param);

    return 0;
}

void amba_cleanup_metal_system(void)
{
    metal_finish();
}

