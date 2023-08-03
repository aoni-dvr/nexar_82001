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

#ifndef _AMBA_IPC_RPC_PROG_LU_LINUX_STREAM_SHARE_H_
#define _AMBA_IPC_RPC_PROG_LU_LINUX_STREAM_SHARE_H_

#include "AmbaIPC_Rpc_Def.h"

enum _LU_STREAM_FUNC_e_ {
    LU_STREAM_SHARE_FUNC_NMEA_NOTIFY = 1,
	LU_STREAM_SHARE_FUNC_MIC_NOTIFY,
    LU_STREAM_SHARE_FUNC_AMOUNT
};

typedef struct _stream_share_param_s {
    unsigned int size;
    unsigned char eos;
    int extra_param;
} stream_share_param_s;

#define AMBA_RPC_PROG_LU_STREAM_SHARE                  2	/* This is ModuleName */

#define AMBA_RPC_PROG_LU_STREAM_SHARE_PROG_ID		0x6000000D
#define AMBA_RPC_PROG_LU_STREAM_SHARE_VER			1

#define AMBA_RPC_PROG_LU_STREAM_SHARE_IPC            1

#endif
