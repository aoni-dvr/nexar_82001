/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_CmdHndlr.h
 *
 * Header file for NetCtrl RPC Services
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _RPC_PROG_RT_LNXCMDHNDLR_H_
#define _RPC_PROG_RT_LNXCMDHNDLR_H_

#include "AmbaIPC_Rpc_Def.h"

/* The Module Name is : RT_LNXCMDHNDLR */
#define RT_LNXCMDHNDLR_PROG_ID        0x1000000B
#define RT_LNXCMDHNDLR_VER            1

/**
 *  nctrl cmd Procedure ID at RTOS side
 */
typedef enum _NCTRL_LNX_CMD_RT_SUB_FUNC_e_
{
    NCTRL_LNX_CMD_RT_UNKNOW  = 0,
    NCTRL_LNX_CMD_RT_REPORT_STATE,
    NCTRL_LNX_CMD_RT_GET_MMAP_INFO,

    NCTRL_LNX_CMD_RT_TOTAL
} NCTRL_LNX_CMD_RT_SUB_FUNC_e;


#ifndef NCTRL_LNX_CMD_RT_BASE_DECLARATION
#define NCTRL_LNX_CMD_RT_BASE_DECLARATION
/**
 *  The base class of cmd notify
 *  All Procedure MUST Inherited from NCTRL_CMD_RT_BASE_s
 */
typedef struct _NCTRL_LNX_CMD_RT_BASE_s_
{
    NCTRL_LNX_CMD_RT_SUB_FUNC_e   Id;
    unsigned int                  bHaveChild;

} NCTRL_LNX_CMD_RT_BASE_s;
#endif

/**
 *  state of executed linux cmd
 */
typedef enum _NCTRL_LNX_CMD_RT_NOTIFY_STATE_e_
{
    NCTRL_LNX_CMD_RT_NOTIFY_STATE_READY = 0,  /**< The server of shell(linux) commands in the linux is ready. */
    NCTRL_LNX_CMD_RT_NOTIFY_STATE_EXIT,       /**< The server of shell(linux) commands in the linux is terminated normally. */
    NCTRL_LNX_CMD_RT_NOTIFY_STATE_ERROR       /**< An error is gotten in the server of shell(linux) commands in the linux. */
} NCTRL_LNX_CMD_RT_NOTIFY_STATE_e;

typedef struct _NCTRL_LNX_CMD_RT_STATE_INFO_s_
{
    NCTRL_LNX_CMD_RT_BASE_s          RpcBase;
    NCTRL_LNX_CMD_RT_NOTIFY_STATE_e  State;
} NCTRL_LNX_CMD_RT_STATE_INFO_s;


/**
 *  linux cmd linux cmd get memory info
 */
typedef struct _NCTRL_LNX_CMD_RT_GET_MEM_s_
{
    NCTRL_LNX_CMD_RT_BASE_s     RpcBase;
    unsigned int                MemType;    /**< reserved. */
} NCTRL_LNX_CMD_RT_GET_MEM_s;

/**
 *  linux cmd mmap (RTOS) info
 */
typedef struct _NCTRL_LNX_CMD_RT_MMAP_INFO_s_
{
    NCTRL_LNX_CMD_RT_BASE_s RpcBase;
    unsigned long long      BaseAddr;       /**< For mmap, base address from RTOS. */
    unsigned long long      PhyAddr;        /**< For mmap, physical address from RTOS. */
    unsigned long long      Size;           /**< For mmap, mapping size from RTOS. */
    unsigned int            IsVerified;     /**< This info is verified or not. */
} NCTRL_LNX_CMD_RT_MMAP_INFO_s;


#endif /* RPC_PROG_RT_LNXCMDHNDLR_H */

