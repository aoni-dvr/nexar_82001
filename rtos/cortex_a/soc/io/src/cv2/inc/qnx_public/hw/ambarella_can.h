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

#ifndef _CAB_LIB_H_INCLUDED
#define _CAN_LIB_H_INCLUDED


#include <stdint.h>
#include <AmbaKAL.h>
#include "AmbaCAN.h"

typedef struct _can_enable {
    UINT32            CanCh;
    AMBA_CAN_CONFIG_s Config;
    UINT32            Ret;
} can_enable_t;

typedef struct _can_msg {
    UINT32            CanCh;
    AMBA_CAN_MSG_s    Message;
    UINT32            Timeout;
    UINT32            Ret;
} can_msg_t;

typedef struct _can_fd_msg {
    UINT32            CanCh;
    AMBA_CAN_FD_MSG_s FdMessage;
    UINT32            Timeout;
    UINT32            Ret;
} can_fd_msg_t;

typedef struct _can_disable {
    UINT32            CanCh;
    UINT32            Ret;
} can_disable_t;

typedef struct _can_info {
    UINT32              CanCh;
    AMBA_CAN_BIT_INFO_s BitInfo;
    UINT32              NumFilter;
    AMBA_CAN_FILTER_s   Filter[32]; // max mailbox number = 32
    UINT32              Ret;
} can_info_t;


/*
 * The following devctls are used by a client application
 * to control the can interface.
 */
#include <devctl.h>

#define _DCMD_CAN   _DCMD_MISC

#define DCMD_CAN_ENABLE             __DIOTF(_DCMD_CAN, 0, can_enable_t)
#define DCMD_CAN_WRITE              __DIOTF(_DCMD_CAN, 1, can_msg_t)
#define DCMD_CAN_FD_WRITE           __DIOTF(_DCMD_CAN, 2, can_fd_msg_t)
#define DCMD_CAN_READ               __DIOTF(_DCMD_CAN, 3, can_msg_t)
#define DCMD_CAN_FD_READ            __DIOTF(_DCMD_CAN, 4, can_fd_msg_t)
#define DCMD_CAN_GET_INFO           __DIOTF(_DCMD_CAN, 5, can_info_t)
#define DCMD_CAN_DISABLE            __DIOTF(_DCMD_CAN, 6, can_disable_t)

#endif
