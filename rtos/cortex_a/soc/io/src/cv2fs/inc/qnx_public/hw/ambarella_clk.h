/*
 * $QNXLicenseC:
 * Copyright 2020, QNX Software Systems. All Rights Reserved.
 * Copyright 2020, Ambarella International LP
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#ifndef _CLK_LIB_H_INCLUDED
#define _CLK_LIB_H_INCLUDED

#include <stdint.h>
#include "AmbaSYS.h"

typedef enum {
    AMBA_PLL_AUDIO = 0,
    AMBA_PLL_CORE,
    AMBA_PLL_CORTEX0,
    AMBA_PLL_CORTEX1,
    AMBA_PLL_DDR,
    AMBA_PLL_ENET,
    AMBA_PLL_VIDEO_A,
    AMBA_PLL_VIDEO_B,
    AMBA_PLL_IDSP,
    AMBA_PLL_NAND,
    AMBA_PLL_SD,
    AMBA_PLL_SENSOR0,
    AMBA_PLL_SENSOR2,
    AMBA_PLL_VISION,
    AMBA_PLL_FEX,
    AMBA_NUM_PLL
} pll_id_t;

typedef struct _clk_freq {
    ctrl_id_t id;
    uint32_t freq;
} clk_freq_t;

typedef struct _pll_freq {
    pll_id_t id;
    uint32_t freq;
} pll_freq_t;

typedef struct _clk_config {
    ctrl_id_t id;
    uint32_t config;
    uint32_t feature;
    uint32_t enable;
} clk_config_t;

typedef struct _clk_info {
    ctrl_id_t id;
    pll_id_t src_id;
    uint32_t freq;
    int enabled;
} clk_info_t;

typedef struct _clk_boot_info {
    uint32_t BootMode;
} clk_boot_info_t;

typedef struct _clk_orc_timer {
    uint32_t TimeTick;
} clk_orc_timer_t;

typedef struct _clk_reset_info {
    uint32_t ResetStatus;
} clk_reset_info_t;

/*
 * The following devctls are used by a client application
 * to control the Clock interface.
 */
#include <devctl.h>

#define _DCMD_CLOCK   _DCMD_MISC

#define DCMD_CLOCK_SET_FREQ               __DIOT(_DCMD_CLOCK, 0, clk_freq_t)
#define DCMD_CLOCK_GET_FREQ               __DIOTF(_DCMD_CLOCK, 1, clk_freq_t)
#define DCMD_CLOCK_ENABLE                 __DIOT(_DCMD_CLOCK, 2, ctrl_id_t)
#define DCMD_CLOCK_DISABLE                __DIOT(_DCMD_CLOCK, 3, ctrl_id_t)
#define DCMD_CLOCK_INFO                   __DIOTF(_DCMD_CLOCK, 4, ctrl_id_t)
#define DCMD_CLOCK_GET_PLL_FREQ           __DIOTF(_DCMD_CLOCK, 5, pll_id_t)
#define DCMD_CLOCK_SET_PLL_FREQ           __DIOT(_DCMD_CLOCK, 6, clk_freq_t)
#define DCMD_CLOCK_CLK_CONFIG             __DIOT(_DCMD_CLOCK, 7, clk_config_t)
#define DCMD_CLOCK_GETBOOTMODE            __DIOTF(_DCMD_CLOCK, 8, clk_boot_info_t)
#define DCMD_CLOCK_SETFEATURE             __DIOT(_DCMD_CLOCK, 9, clk_config_t)
#define DCMD_CLOCK_DISFEATURE             __DIOT(_DCMD_CLOCK, 10, clk_config_t)
#define DCMD_CLOCK_CHECKFEATURE           __DIOTF(_DCMD_CLOCK, 11, clk_config_t)
#define DCMD_CLOCK_GETORCTIMER            __DIOTF(_DCMD_CLOCK, 12, clk_orc_timer_t)
#define DCMD_SYS_RESET                    __DION(_DCMD_CLOCK, 13)
#define DCMD_CLOCK_GET_RESET_ST           __DIOTF(_DCMD_CLOCK, 14, clk_reset_info_t)

#endif

