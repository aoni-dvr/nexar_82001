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

#ifndef _RTC_LIB_H_INCLUDED
#define _RTC_LIB_H_INCLUDED


#include <stdint.h>
#include <AmbaKAL.h>

typedef struct _rtc_config_msg {
    UINT32        second;
} rtc_msg_t;

/*
 * The following devctls are used by a client application
 * to control the rtc interface.
 */
#include <devctl.h>

#define _DCMD_RTC   _DCMD_MISC

#define DCMD_RTC_SET             __DIOT(_DCMD_RTC, 0, rtc_msg_t)
#define DCMD_RTC_GET             __DIOTF(_DCMD_RTC, 1, rtc_msg_t)



#endif
