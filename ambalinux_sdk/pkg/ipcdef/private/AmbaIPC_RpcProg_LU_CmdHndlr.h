/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_Lu_CmdHndlr.h
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

#ifndef _RPC_PROG_LU_CMD_HNDLR_H_
#define _RPC_PROG_LU_CMD_HNDLR_H_

#include "AmbaIPC_Rpc_Def.h"

#define LU_CMD_HNDLR_PROG_ID        0x20000006
#define LU_CMD_HNDLR_VER            1


/**
 *  nctrl linux cmd Procedure ID at linux side
 */
typedef enum _NCTRL_LNX_CMD_LU_SUB_FUNC_e_
{
    NCTRL_LNX_CMD_LU_UNKNOW        = 0,
    NCTRL_LNX_CMD_LU_EXECUTE,

    NCTRL_LNX_CMD_LU_TOTAL
} NCTRL_LNX_CMD_LU_SUB_FUNC_e;

#ifndef NCTRL_LNX_CMD_LU_BASE_DECLARATION
#define NCTRL_LNX_CMD_LU_BASE_DECLARATION
/**
 *  The base class of cmd notify
 *  All Procedure MUST Inherited from NCTRL_LNX_CMD_LU_BASE_s
 */
typedef struct _NCTRL_LNX_CMD_LU_BASE_s_
{
    NCTRL_LNX_CMD_LU_SUB_FUNC_e   Id;
    unsigned int                  bHaveChild;

} NCTRL_LNX_CMD_LU_BASE_s;
#endif

/**
 *  nctrl support linux cmd
 */
typedef enum _NCTRL_LNX_CMD_LU_e_
{
    NCTRL_LNX_CMD_LU_LS       = 0,
    NCTRL_LNX_CMD_LU_CD,
    NCTRL_LNX_CMD_LU_PWD,
    NCTRL_LNX_CMD_LU_GET_WIFI_STATE,
    NCTRL_LNX_CMD_LU_GET_WIFI_CFG,
    NCTRL_LNX_CMD_LU_SET_WIFI_CFG,
    NCTRL_LNX_CMD_LU_START_WIFI,
    NCTRL_LNX_CMD_LU_STOP_WIFI,
    NCTRL_LNX_CMD_LU_RESTART_WIFI

} NCTRL_LNX_CMD_LU_e;

/**
 *  args to execute linux cmd
 */
typedef struct _NCTRL_LNX_CMD_ARGS_s_
{
    NCTRL_LNX_CMD_LU_BASE_s RpcBase;
    unsigned int            LnxCmd;             /**< linux command id */
    unsigned int            OutSize;            /**< the maximum size of the output */
    unsigned long long      OutAddr;            /**< the result of execute linux command */
    unsigned int            CmdStringSize;      /**< the string length of the command */
    char                    CmdString[1024];    /**< command string */
    char                    Param[512];         /**< the parameter of command */
} NCTRL_LNX_CMD_ARGS_s;

/**
 * result of linux command operation
 */
typedef struct _NCTRL_LNX_CMD_RESP_s_
{
    NCTRL_LNX_CMD_LU_BASE_s     RpcBase;
    int                         Rval;           /**< result of execute linux command */
    unsigned int                ReportSize;     /**< message size of the executed linux cmd  */
} NCTRL_LNX_CMD_RESP_s;

#endif /* RPC_PROG_LU_CMD_HNDLR_H */

