/**
 * @file AmbaIPC_RpcProg_LU_Regression.h
 *
 * Copyright (C) 2020, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef RPC_PROG_LU_REGRESSION_H_
#define RPC_PROG_LU_REGRESSION_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_REGRESSION_PROG_ID (0x20000500)
#define LU_REGRESSION_HOST AMBA_IPC_HOST_LINUX
#define LU_REGRESSION_VER (1)
#define LU_REGRESSION_DEFULT_TIMEOUT (FOREVER)
#define LU_REGRESSION_NAME "AMBAREGR_LUSVC"

#define LU_REGRESSION_MAX_RESULT_AMOUNT (8)

typedef struct LU_REGRESSION_DATA_BLK_s_ {
    unsigned int PhyAddr;
    unsigned int Size;
} __attribute__((packed)) LU_REGRESSION_DATA_BLK_s;

typedef struct LU_REGRESSION_WRITERESULT_ARG_s_ {
    unsigned int Index;
    unsigned int Type;
    unsigned int Amount;
    LU_REGRESSION_DATA_BLK_s Data[LU_REGRESSION_MAX_RESULT_AMOUNT];
} __attribute__((packed)) LU_REGRESSION_WRITERESULT_ARG_s;

//============ RPC_FUNC definition ============
enum LU_REGRESSION_FUNC_e_ {
	LU_REGRESSION_FUNC_START = 1,
    LU_REGRESSION_FUNC_WRTITERESULT,
    LU_REGRESSION_FUNC_STOP,
    LU_REGRESSION_FUNC_SETMEMREGION,

	LU_REGRESSION_FUNC_AMOUNT
};

#endif /* RPC_PROG_LU_REGRESSION_H_ */

