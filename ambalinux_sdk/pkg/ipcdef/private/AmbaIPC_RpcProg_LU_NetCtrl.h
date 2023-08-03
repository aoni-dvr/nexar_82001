/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_NetCtrl.h
 *
 * Header file for NetCtrl RPC Services
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _RPC_PROG_LU_NETCTRL_H_
#define _RPC_PROG_LU_NETCTRL_H_

#include "AmbaIPC_Rpc_Def.h"

#define LU_NETCTRL_PROG_ID      0x20000005
#define LU_NETCTRL_VER          1

/**
 *  nctrl cmd Procedure ID at linux side
 */
typedef enum _NCTRL_CMD_LU_SUB_FUNC_e_
{
    NCTRL_CMD_LU_UNKNOW        = 0,
    NCTRL_CMD_LU_REPLY_REMOTE,

    NCTRL_CMD_LU_TOTAL
} NCTRL_CMD_LU_SUB_FUNC_e;

#ifndef NCTRL_CMD_LU_BASE_DECLARATION
#define NCTRL_CMD_LU_BASE_DECLARATION
/**
 *  The base class of cmd notify
 *  All Procedure MUST Inherited from NCTRL_CMD_LU_BASE_s
 */
typedef struct _NCTRL_CMD_LU_BASE_s_
{
    NCTRL_CMD_LU_SUB_FUNC_e Id;
    unsigned int            bHaveChild;

} NCTRL_CMD_LU_BASE_s;
#endif

/**
 * response of network control command
 */
typedef struct _NCTRL_CMD_RESP_s_ {
    NCTRL_CMD_LU_BASE_s  RpcBase;
    unsigned long long   Param;              /**< json string context */
    unsigned int         ParamSize;          /**< size of json string */
    unsigned int         ClientId;           /**< socket descriptor of accepted client */
    unsigned int         ClientPort;         /**< the connected port */
    unsigned long long   ClientAddress;      /**< the client address such as IP address or mac address */
} NCTRL_CMD_RESP_s;

#endif /* _RPC_PROG_LU_NETCTRL_H_ */

