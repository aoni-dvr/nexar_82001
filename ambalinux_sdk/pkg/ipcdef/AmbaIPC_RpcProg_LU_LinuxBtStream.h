/**
 * @file AmbaIPC_RpcProg_LU_IpcCommunication.h
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

#ifndef _AMBA_IPC_RPC_PROG_LU_LINUX_BT_STREAM_H_
#define _AMBA_IPC_RPC_PROG_LU_LINUX_BT_STREAM_H_

#include "AmbaIPC_Rpc_Def.h"

typedef struct _bt_stream_param_s {
    unsigned int size;
    unsigned char eos;
    int timezone;
} bt_stream_param_s;

enum _LU_BT_STREAM_FUNC_e_ {
    LU_BT_STREAM_FUNC_IMU_NOTIFY = 1,
    LU_BT_STREAM_FUNC_GNSS_NOTIFY,
    LU_BT_STREAM_FUNC_AMOUNT
};

#define AMBA_RPC_PROG_LU_BT_STREAM                  2	/* This is ModuleName */

#define AMBA_RPC_PROG_LU_BT_STREAM_PROG_ID		0x3000000D
#define AMBA_RPC_PROG_LU_BT_STREAM_VER			1

#define AMBA_RPC_PROG_LU_BT_STREAM_IPC            1

#endif
