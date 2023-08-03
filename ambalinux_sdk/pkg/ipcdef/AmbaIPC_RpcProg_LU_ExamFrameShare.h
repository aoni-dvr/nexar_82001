/**
 *
 * Header file for RPC Services
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_EXAMFRAMESHARE_H_
#define _RPC_PROG_LU_EXAMFRAMESHARE_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_EXAMFRAMESHARE_PROG_ID	0x2000000B
#define LU_EXAMFRAMESHARE_HOST AMBA_IPC_HOST_LINUX
#define LU_EXAMFRAMESHARE_VER (1)
#define LU_EXAMFRAMESHARE_DEFULT_TIMEOUT (FOREVER)
#define LU_EXAMFRAMESHARE_NAME "AMBAEXAMFRAMESHARE_LUSVC"

typedef struct _LU_EXAMFRAMESHARE_CONTROL_ARG_s_ {
    unsigned int Cmd;
    unsigned long long Param1;
    unsigned long long Param2;
} LU_EXAMFRAMESHARE_CONTROL_ARG_s;

typedef struct _LU_EXAMFRAMESHARE_FRAME_DESC_s_ {
    unsigned int SeqNum; /**< sequential number of bits buffer */
    unsigned long long Pts; /**< time stamp in ticks */
    unsigned char Type; /**< data type of the entry. see LU_EXAMFRAMESHARE_FRAME_FRMAE_TYPE_e*/
    unsigned char Completed; /**< if the buffer content a complete entry */
    unsigned short  Align; /** data size alignment (in bytes, align = 2^n, n is a integer )*/
    unsigned long long StartAddr; /**< start address of data */
    unsigned int Size; /**< real data size */
} LU_EXAMFRAMESHARE_FRAME_DESC_s;

typedef struct _LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    LU_EXAMFRAMESHARE_FRAME_DESC_s InfoPtr;
} LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s;

typedef struct _LU_EXAMFRAMESHARE_ENCINFO_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    unsigned int media_type;
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
} LU_EXAMFRAMESHARE_ENCINFO_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_EXAMFRAMESHARE_FUNC_e_ {
    LU_EXAMFRAMESHARE_FUNC_CONTROLEVENT = 1,
    LU_EXAMFRAMESHARE_FUNC_FRAMEEVENT,
    LU_EXAMFRAMESHARE_FUNC_SETENCINFO,

    LU_EXAMFRAMESHARE_FUNC_AMOUNT
};

/**
 * [in] LU_EXAMFRAMESHARE_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMFRAMESHARE_ControlEvent_Clnt(LU_EXAMFRAMESHARE_CONTROL_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMFRAMESHARE_ControlEvent_Svc(LU_EXAMFRAMESHARE_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMFRAMESHARE_FrameEvent_Clnt(LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMFRAMESHARE_FrameEvent_Svc(LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMFRAMESHARE_ENCINFO_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMFRAMESHARE_SetEncInfo_Clnt(LU_EXAMFRAMESHARE_ENCINFO_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMFRAMESHARE_SetEncInfo_Svc(LU_EXAMFRAMESHARE_ENCINFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_EXAMFRAMESHARE_H_ */

