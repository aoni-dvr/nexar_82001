/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_Fexidag.h
 *
 * Header file for Stream RPC Services (RTOS side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_RT_FLEXIDAG_H_
#define _RPC_PROG_RT_FLEXIDAG_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_RT_FLEXIDAG					1U	/* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_RT_FLEXIDAG_PROG_ID			0x10000403U /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_RT_FLEXIDAG_VER				1U /* RPC Program version */
#define AMBA_RPC_PROG_RT_FLEXIDAG_HOST				AMBA_IPC_HOST_THREADX

#define AMBA_RPC_PROG_RT_FLEXIDAG_CB				1U


#endif /* _RPC_PROG_RT_FLEXIDAG_H_ */
