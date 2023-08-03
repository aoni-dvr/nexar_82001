/*
 * $QNXLicenseC:
 * Copyright 2007, 2008, 2018, QNX Software Systems.
 * Copyright 2020, Ambarella International LP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef CEHU_LIB_H_INCLUDED
#define CEHU_LIB_H_INCLUDED

#include <stdint.h>
#include <AmbaKAL.h>

typedef struct {
    UINT32        InstanceID;
    UINT32        ErrorID;
    UINT32        Data0;
    UINT32        Data1;
} cehu_msg_t;

/*
 * The following devctls are used by a client application
 * to control the CEHU interface.
 */
#include <devctl.h>

#define AMBA_CEHU_MASK_ENABLE        1
#define AMBA_CEHU_MASK_DISABLE       2
#define AMBA_CEHU_MASK_GET           3
#define AMBA_CEHU_ERROR_GET          4
#define AMBA_CEHU_ERROR_CLEAR        5
#define AMBA_CEHU_SAFETY_MODE_GET    6
#define AMBA_CEHU_SAFETY_MODE_SET    7

#define _DCMD_CEHU   _DCMD_MISC

#define DCMD_CEHU_MASK_ENABLE           __DIOT(_DCMD_CEHU, AMBA_CEHU_MASK_ENABLE,       cehu_msg_t)
#define DCMD_CEHU_MASK_DISABLE          __DIOT(_DCMD_CEHU, AMBA_CEHU_MASK_DISABLE,      cehu_msg_t)
#define DCMD_CEHU_MASK_GET             __DIOTF(_DCMD_CEHU, AMBA_CEHU_MASK_GET,          cehu_msg_t)
#define DCMD_CEHU_ERROR_GET            __DIOTF(_DCMD_CEHU, AMBA_CEHU_ERROR_GET,         cehu_msg_t)
#define DCMD_CEHU_ERROR_CLEAR           __DIOT(_DCMD_CEHU, AMBA_CEHU_ERROR_CLEAR,       cehu_msg_t)
#define DCMD_CEHU_SAFETY_MODE_GET      __DIOTF(_DCMD_CEHU, AMBA_CEHU_SAFETY_MODE_GET,   cehu_msg_t)
#define DCMD_CEHU_SAFETY_MODE_SET       __DIOT(_DCMD_CEHU, AMBA_CEHU_SAFETY_MODE_SET,   cehu_msg_t)

#endif
