/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_Memio.h
 *
 * Header file for Memio RPC Services (Linux side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_MEMIO_H_
#define _RPC_PROG_LU_MEMIO_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_LU_MEMIO                  1U	/* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_LU_MEMIO_PROG_ID		    0x20000401U /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_LU_MEMIO_VER			    1U /* RPC Program version */
#define AMBA_RPC_PROG_LU_MEMIO_HOST			    AMBA_IPC_HOST_LINUX

#define AMBA_RPC_PROG_LU_MEMIO_CONNECT          1U
#define AMBA_RPC_PROG_LU_MEMIO_DISCONNECT       2U
#define AMBA_RPC_PROG_LU_MEMIO_SEND             3U

#define AMBA_RPC_LU_MEMIO_SEND_HEAD            12U
#define AMBA_RPC_LU_MEMIO_SEND_DATA            (1024U - AMBA_RPC_LU_MEMIO_SEND_HEAD)

typedef struct{
    unsigned int Index;
    unsigned int Owner;
} __attribute__((packed)) AMBA_RPC_LU_MEMIO_CONNECT_s;

typedef struct{
    unsigned int Index;
    unsigned int Owner;
} __attribute__((packed)) AMBA_RPC_LU_MEMIO_DISCONNECT_s;

typedef struct{
    unsigned int Index;
    unsigned int Owner;
    unsigned int DataLen;
    unsigned char  Data[AMBA_RPC_LU_MEMIO_SEND_DATA];
} __attribute__((packed)) AMBA_RPC_LU_MEMIO_SEND_s;

unsigned int AmbaRpcProg_LU_Memio_Init(void);
unsigned int AmbaRpcProg_LU_Memio_Deinit(void);
unsigned int AmbaRpcProg_LU_Memio_Connect(Transfer_MsgQ_Ctx_s *pCtx, unsigned int Owner);
unsigned int AmbaRpcProg_LU_Memio_Disconnect(Transfer_MsgQ_Ctx_s *pCtx, unsigned int Owner);
unsigned int AmbaRpcProg_LU_Memio_Send(Transfer_MsgQ_Ctx_s *pCtx, unsigned int Owner, void *pData, unsigned int Len);

#endif /* _RPC_PROG_LU_MEMIO_H_ */
