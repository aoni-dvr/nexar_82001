/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_RT_DataNotify.h
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

#ifndef _RPC_PROG_RT_DATA_NOTIFY_H_
#define _RPC_PROG_RT_DATA_NOTIFY_H_

#include "AmbaIPC_Rpc_Def.h"

#define RT_DATA_NOTIFY_PROG_ID          0x10000006
#define RT_DATA_NOTIFY_VER              1

/**
 *  nctrl data notify Procedure ID
 */
typedef enum _DATA_NOTIFY_SUB_FUNC_e_
{
    DATA_NOTIFY_UNKNOW       = 0,
    DATA_NOTIFY_REPORTS,
    DATA_NOTIFY_SERVER_STATE,
    DATA_NOTIFY_GET_MMAP_INFO,

    DATA_NOTIFY_TOTAL
} DATA_NOTIFY_SUB_FUNC_e;

#ifndef DATA_NOTIFY_BASE_DECLARATION
#define DATA_NOTIFY_BASE_DECLARATION
/**
 *  The base class of data notify
 *  All Procedure MUST Inherited from DATA_NOTIFY_BASE_s
 */
typedef struct _DATA_NOTIFY_BASE_s_
{
    DATA_NOTIFY_SUB_FUNC_e  Id;
    unsigned int            bHaveChild;

} DATA_NOTIFY_BASE_s;
#endif

/**
 * data server report info
 */
typedef struct _DATA_NOTIFY_REPORT_s_
{
    DATA_NOTIFY_BASE_s  RpcBase;
    short               Status;             /**< send status of data server */
    unsigned long long  Bytes;              /**< send/recv data bytes */
    unsigned int        Type;               /**< notification type of data server */
    unsigned char       ClientInfo[128];    /**< client identifier */
    char                TransportType[16];  /**< transport protocol type */
    unsigned char       Md5sum[32];         /**< md5 checksum */
} DATA_NOTIFY_REPORT_s;


/**
 *  data server stat
 */
typedef enum _DATA_NOTIFY_STATE_e_
{
    DATA_NOTIFY_STATE_READY      = 0,        /**< The handler of data in the linux is ready. */
    DATA_NOTIFY_STATE_EXIT,                  /**< The handler of data in the linux is terminated normally. */
    DATA_NOTIFY_STATE_ERROR,                 /**< An error is gotten in the handler of data in the linux. */

} DATA_NOTIFY_STATE_e;

/**
 *  data server stat info
 */
typedef struct _DATA_NOTIFY_STATE_INFO_s_
{
    DATA_NOTIFY_BASE_s      RpcBase;
    DATA_NOTIFY_STATE_e     State;
} DATA_NOTIFY_STATE_INFO_s;

/**
 *  data server get memory info
 */
typedef struct _DATA_NOTIFY_GET_MEM_s_
{
    DATA_NOTIFY_BASE_s     RpcBase;
    unsigned int           MemType;    /**< reserved. */
} DATA_NOTIFY_GET_MEM_s;

/**
 *  data server mmap (RTOS) info
 */
typedef struct _DATA_NOTIFY_MMAP_INFO_s_
{
    DATA_NOTIFY_BASE_s      RpcBase;
    unsigned long long      BaseAddr;       /**< For mmap, base address from RTOS. */
    unsigned long long      PhyAddr;        /**< For mmap, physical address from RTOS. */
    unsigned long long      Size;           /**< For mmap, mapping size from RTOS. */
    unsigned int            IsVerified;     /**< This info is verified or not. */
} DATA_NOTIFY_MMAP_INFO_s;


#endif /* RPC_PROG_DATA_NOTIFY_H */

