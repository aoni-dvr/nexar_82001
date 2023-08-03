/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_FlexidagIO.h
 *
 * Header file for FlexidagIO RPC Services (Linux side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_FLEXIDAGIO_H_
#define _RPC_PROG_LU_FLEXIDAGIO_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_LU_FLEXIDAGIO_NUM             8U

#define AMBA_RPC_PROG_LU_FLEXIDAGIO                 1U  /* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_LU_FLEXIDAGIO_PROG_ID         0x20000410U /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_LU_FLEXIDAGIO_VER             1U /* RPC Program version */
#define AMBA_RPC_PROG_LU_FLEXIDAGIO_HOST            AMBA_IPC_HOST_LINUX

#define AMBA_RPC_PROG_LU_FLEXIDAGIO_INPUT           1U
#define AMBA_RPC_PROG_LU_FLEXIDAGIO_PROC_NUM        1U

#define AMBA_RPC_LU_FLEXIDAGIO_RESULT_HEAD          8U
#define AMBA_RPC_LU_FLEXIDAGIO_RESULT_DATA          (1024U - AMBA_RPC_LU_FLEXIDAGIO_RESULT_HEAD)

typedef struct{
    unsigned int Channel;
    unsigned int OutType;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAGIO_CONFIG_s;

typedef struct{
    unsigned int Channel;
    unsigned int DataLen;
    unsigned char  Data[AMBA_RPC_LU_FLEXIDAGIO_RESULT_DATA];
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAGIO_RESULT_s;


#endif /* _RPC_PROG_LU_FLEXIDAGIO_H_ */
