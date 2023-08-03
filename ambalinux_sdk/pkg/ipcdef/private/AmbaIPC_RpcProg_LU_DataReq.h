/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_Lu_DataReq.h
 *
 * Header file for NetCtrl RPC Services
 *
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef _RPC_PROG_LU_DATA_REQ_H_
#define _RPC_PROG_LU_DATA_REQ_H_

#include "AmbaIPC_Rpc_Def.h"

#define LU_DATA_REQ_PROG_ID                 0x20000007
#define LU_DATA_REQ_VER                     1


/**
 *  data req Procedure ID
 */
typedef enum _DATA_REQ_SUB_FUNC_e_
{
    DATA_REQ_UNKNOW     = 0,
    DATA_REQ_INFO,
    DATA_REQ_GET_STATUS,
    DATA_REQ_CANCEL_TRANS,
    DATA_REQ_CLOSE_CONN,

    DATA_REQ_TOTAL
} DATA_REQ_SUB_FUNC_e;

#ifndef DATA_REQ_BASE_DECLARATION
#define DATA_REQ_BASE_DECLARATION
/**
 *  The base class of data notify
 *  All Procedure MUST Inherited from DATA_REQ_BASE_s
 */
typedef struct _DATA_REQ_BASE_s_
{
    DATA_REQ_SUB_FUNC_e     Id;
    unsigned int            bHaveChild;

} DATA_REQ_BASE_s;
#endif

/**
 * struct of data request info
 */
typedef struct _DATA_REQ_INFO_s_
{
    DATA_REQ_BASE_s     RpcBase;
    unsigned int        MsgId;              /**< send data type*/
    unsigned long long  Offset;             /**< file offset */
    unsigned long long  FetchFilesize;      /**< fetch file size */
    unsigned int        BlockSize;          /**< the size of block (Kbyte)*/
    unsigned int        MaximalSpeed;       /**< the maximal speed of transmitting file (Kbyte/sec)*/
    unsigned long long  BufferAddr;         /**< data buffer */
    char                Filepath[512];      /**< filepath */
    char                TransportType[16];  /**< transport protocol type */
    unsigned char       ClientInfo[128];    /**< client identifier */
    char                Md5sum[32];         /**< md5sum */
} DATA_REQ_INFO_s;

/**
 * struct of data request result
 */
typedef struct _DATA_REQ_RESULT_s_
{
    DATA_REQ_BASE_s     RpcBase;
    int                 Rval;               /**< result value */
    unsigned long long  RemSize;            /**< remain file size */
    unsigned long long  TotalFilesize;      /**< total file size */
} DATA_REQ_RESULT_s;


/**
 * struct of data transmission status
 */
typedef struct _DATA_REQ_TRANS_INFO_s_
{
    DATA_REQ_BASE_s     RpcBase;
    unsigned char       ClientInfo[128];        /**< client identifier */
    char                TransportType[16];      /**< transport protocol type */
} DATA_REQ_TRANS_INFO_s;


/**
 * cancel data transmission info
 */
typedef struct _DATA_REQ_CANCEL_TRANS_s_
{
    DATA_REQ_BASE_s     RpcBase;
    int                 MsgId;              /**< data request type */
    unsigned char       ClientInfo[128];    /**< client identifier */
    char                TransportType[16];  /**< transport protocol type */
    unsigned long long  SentSize;           /**< size of file (AMBA_PUT_FILE) data portion */
} DATA_REQ_CANCEL_TRANS_s;

/**
 * cancel data transmission result
 */
typedef struct _DATA_REQ_CANCEL_RESULT_s_
{
    DATA_REQ_BASE_s     RpcBase;
    int                 Rval;             /**< result value */
    unsigned long long  TransSize;        /**< transferred size */
} DATA_REQ_CANCEL_RESULT_s;


/**
 * struct of close data connection
 */
typedef struct _DATA_REQ_CLOSE_CONNECT_s_
{
    DATA_REQ_BASE_s     RpcBase;
    unsigned char       ClientInfo[128];        /**< client identifier */
    char                TransportType[16];      /**< transport protocol type */
} DATA_REQ_CLOSE_CONNECT_s;


#endif /* RPC_PROG_LU_DATA_REQ_H */

