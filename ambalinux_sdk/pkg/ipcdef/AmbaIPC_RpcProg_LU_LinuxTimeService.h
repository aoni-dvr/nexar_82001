/**
 * @file AmbaIPC_RpcProg_LU_LinuxTimeService.h
 *
 * Header file for Pseudo Terminal RPC Services
 *
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _AMBA_IPC_RPC_PROG_LU_LINUX_TIME_SERVICE_H_
#define _AMBA_IPC_RPC_PROG_LU_LINUX_TIME_SERVICE_H_

#include "AmbaIPC_Rpc_Def.h"

typedef struct _r_time_s_ {
    int year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
} r_time_s;

typedef struct _response_time_s {
    unsigned long long tv_sec;
    unsigned long long tv_usec;
    r_time_s local_time;
    r_time_s utc_time;
} response_time_s;

#define AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE               2	/* This is ModuleName */
#define AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_PROG_ID		0x5000000D
#define AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_VER			1

#define AMBA_RPC_PROG_LU_LINUX_TIME_SERVICE_IPC           1

#endif
