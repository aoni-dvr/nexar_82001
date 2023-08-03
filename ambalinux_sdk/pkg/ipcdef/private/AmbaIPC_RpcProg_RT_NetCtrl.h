/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_NetCtrl.h
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

#ifndef _RPC_PROG_RT_NETCTRL_H_
#define _RPC_PROG_RT_NETCTRL_H_

#include "AmbaIPC_Rpc_Def.h"

#define RT_NETCTRL_PROG_ID      0x10000005
#define RT_NETCTRL_VER          1


/**
 *  nctrl cmd Procedure ID at RTOS side
 */
typedef enum _NCTRL_CMD_RT_SUB_FUNC_e_
{
    NCTRL_CMD_RT_UNKNOW        = 0,
    NCTRL_CMD_RT_RECV_REMOTE,
    NCTRL_CMD_RT_NOTIFY_SERVER_STATE,
    NCTRL_CMD_RT_GET_MMAP_INFO,

    NCTRL_CMD_RT_TOTAL
} NCTRL_CMD_RT_SUB_FUNC_e;

#ifndef NCTRL_CMD_RT_BASE_DECLARATION
#define NCTRL_CMD_RT_BASE_DECLARATION
/**
 *  The base class of cmd notify
 *  All Procedure MUST Inherited from NCTRL_CMD_RT_BASE_s
 */
typedef struct _NCTRL_CMD_RT_BASE_s_
{
    NCTRL_CMD_RT_SUB_FUNC_e Id;
    unsigned int            bHaveChild;

} NCTRL_CMD_RT_BASE_s;
#endif


/**
 * network control command with json string
 */
typedef struct _NCTRL_CMD_RT_REQ_s_
{
    NCTRL_CMD_RT_BASE_s  RpcBase;
    char                 Param[1024];    /**< json string of cmd */
    unsigned int         ParamSize;      /**< size of json string to cmd  */
    unsigned int         ClientId;       /**< socket descriptor of accepted client */
    unsigned int         ClientPort;     /**< the connected port */
    unsigned long long   ClientAddress;  /**< the client address such as IP address or mac address */
} NCTRL_CMD_RT_REQ_s;

/**
 *  cmd server stat
 */
typedef enum _NCTRL_CMD_NOTIFY_STATE_e_
{
    NCTRL_CMD_NOTIFY_STATE_READY     = 0,    /**< The handler of commands in the linux is ready. */
    NCTRL_CMD_NOTIFY_STATE_EXIT,             /**< The handler of commands in the linux is terminated normally. */
    NCTRL_CMD_NOTIFY_STATE_ERROR,            /**< An error is gotten in the handler of commands in the linux. */
} NCTRL_CMD_NOTIFY_STATE_e;

/**
 *  cmd server stat info
 */
typedef struct _NCTRL_CMD_NOTIFY_STATE_INFO_s_
{
    NCTRL_CMD_RT_BASE_s         RpcBase;
    NCTRL_CMD_NOTIFY_STATE_e    State;
} NCTRL_CMD_NOTIFY_STATE_INFO_s;


/**
 *  cmd server get memory info
 */
typedef struct _NCTRL_CMD_GET_MEM_s_
{
    NCTRL_CMD_RT_BASE_s     RpcBase;
    unsigned int            MemType;    /**< reserved. */
} NCTRL_CMD_GET_MEM_s;

/**
 *  cmd server mmap (RTOS) info
 */
typedef struct _NCTRL_CMD_MMAP_INFO_s_
{
    NCTRL_CMD_RT_BASE_s     RpcBase;
    unsigned long long      BaseAddr;       /**< For mmap, base address from RTOS. */
    unsigned long long      PhyAddr;        /**< For mmap, physical address from RTOS. */
    unsigned long long      Size;           /**< For mmap, mapping size from RTOS. */
    unsigned int            IsVerified;     /**< This info is verified or not. */
} NCTRL_CMD_MMAP_INFO_s;


#endif /* _RPC_PROG_RT_NETCTRL_H_ */

