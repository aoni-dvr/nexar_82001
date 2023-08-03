/**
 *
 * Header file for RPC Services (Log ThreadX)
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_EXAMLOGTHREADX_H_
#define _RPC_PROG_LU_EXAMLOGTHREADX_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_EXAMLOG_PROG_ID	0x20000011
#define LU_EXAMLOG_HOST AMBA_IPC_HOST_LINUX
#define LU_EXAMLOG_VER (1)
#define LU_EXAMLOG_DEFULT_TIMEOUT (FOREVER)
#define LU_EXAMLOG_NAME "AMBAEXAMLOG_LUSVC"

#define HDL_LOG_MAX_STREAM 0x8

typedef struct _LU_EXAMLOGTHREADX_CONTROL_ARG_s_ {
    unsigned long long hndlr;
    unsigned int cmd;
    unsigned long long param1;
    unsigned long long param2;
} LU_EXAMLOGTHREADX_CONTROL_ARG_s;

typedef struct _LU_EXAMLOGTHREADX_FRAME_ARG_s_ {
    unsigned long long hndlr;
    unsigned long long faddr;           /**< start address of data */
    unsigned int fsize;                          /**< real data size */
} LU_EXAMLOGTHREADX_FRAME_ARG_s;

/*
typedef struct _LU_EXAMLOGTHREADX_FRAMEEVENT_ARG_s_ {
    unsigned long long hndlr;
    LU_EXAMLOGTHREADX_FRAME_DESC_s InfoPtr;
} LU_EXAMLOGTHREADX_FRAMEEVENT_ARG_s;
*/

typedef struct _LU_EXAMLOGTHREADX_BUFINFO_ARG_s_ {
    unsigned long long hndlr;
    unsigned int media_type;
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
} LU_EXAMLOGTHREADX_BUFINFO_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_EXAMLOG_FUNC_e_ {
    LU_EXAMLOG_FUNC_SET_BUFINFO = 1,
    LU_EXAMLOG_FUNC_SET_CMD,
    LU_EXAMLOG_FUNC_SET_FRAMEINFO,

    LU_EXAMLOG_FUNC_AMOUNT
};

/**
 * [in] LU_EXAMLOG_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMLOGTHREADX_ControlEvent_Clnt(LU_EXAMLOGTHREADX_CONTROL_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMLOGTHREADX_ControlEvent(const LU_EXAMLOGTHREADX_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMLOGTHREADX_FRAMEEVENT_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMLOGTHREADX_SetFrameInfo_Clnt(LU_EXAMLOGTHREADX_FRAME_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMLOGTHREADX_SetFrameInfo(const LU_EXAMLOGTHREADX_FRAME_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMLOGTHREADX_ENCINFO_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMLOGTHREADX_SetBufInfo_Clnt(LU_EXAMLOGTHREADX_BUFINFO_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMLOGTHREADX_SetBufInfo(const LU_EXAMLOGTHREADX_BUFINFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_EXAMLOGTHREADX_H_ */

