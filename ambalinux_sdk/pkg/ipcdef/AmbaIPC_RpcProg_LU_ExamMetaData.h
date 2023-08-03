/**
 *
 * Header file for RPC Services (Meta Data)
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_EXAMMETADATA_H_
#define _RPC_PROG_LU_EXAMMETADATA_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_EXAMMETADATA_PROG_ID	0x20000101
#define LU_EXAMMETADATA_HOST AMBA_IPC_HOST_LINUX
#define LU_EXAMMETADATA_VER (1)
#define LU_EXAMMETADATA_DEFULT_TIMEOUT (FOREVER)
#define LU_EXAMMETADATA_NAME "AMBAEXAMMETADATA_LUSVC"

typedef struct _LU_EXAMMETADATA_CONTROL_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for meta stream */
    unsigned int Cmd;
    unsigned long long Param1;
    unsigned long long Param2;
} LU_EXAMMETADATA_CONTROL_ARG_s;

typedef struct _LU_EXAMMETADATA_FRAME_DESC_s_ {
    unsigned int Type; /**< data type of the entry. see LU_EXAMFRAMESHARE_FRAME_FRMAE_TYPE_e*/
    unsigned long long StartAddr; /**< start address of data */
    unsigned int Size; /**< real data size */
} LU_EXAMMETADATA_FRAME_DESC_s;

typedef struct _LU_EXAMMETADATA_FRAMEEVENT_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    LU_EXAMMETADATA_FRAME_DESC_s InfoPtr;
} LU_EXAMMETADATA_FRAMEEVENT_ARG_s;

typedef struct _LU_EXAMMETADATA_INFO_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    unsigned int media_type;
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
} LU_EXAMMETADATA_INFO_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_EXAMMETADATA_FUNC_e_ {
    LU_EXAMMETADATA_FUNC_CONTROLEVENT = 1,
    LU_EXAMMETADATA_FUNC_FRAMEEVENT,
    LU_EXAMMETADATA_FUNC_SETINFO,

    LU_EXAMMETADATA_FUNC_AMOUNT
};

/**
 * [in] LU_EXAMMETADATA_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMMETADATA_ControlEvent_Clnt(LU_EXAMMETADATA_CONTROL_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMMETADATA_ControlEvent(LU_EXAMMETADATA_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMMETADATA_FRAMEEVENT_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMMETADATA_FrameEvent_Clnt(LU_EXAMMETADATA_FRAMEEVENT_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMMETADATA_FrameEvent(LU_EXAMMETADATA_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMMETADATA_ENCINFO_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMMETADATA_SetInfo_Clnt(LU_EXAMMETADATA_INFO_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMMETADATA_SetInfo(LU_EXAMMETADATA_INFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_EXAMMETADATA_H_ */

