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
#ifndef _RPC_PROG_LU_EXAMRTSPFRAMESHARE_H_
#define _RPC_PROG_LU_EXAMRTSPFRAMESHARE_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

#define RTSPFRAMESHARE_MSG_SIZE    32

//RPC_INFO definition
#define LU_EXAMRTSPFRAMESHARE_PROG_ID	0x2000000D
#define LU_EXAMRTSPFRAMESHARE_HOST AMBA_IPC_HOST_LINUX
#define LU_EXAMRTSPFRAMESHARE_VER (1)
#define LU_EXAMRTSPFRAMESHARE_DEFULT_TIMEOUT (FOREVER)
#define LU_EXAMRTSPFRAMESHARE_NAME "AMBAEXAMRTSPFRAMESHARE_LUSVC"

typedef struct _LU_EXAMRTSPFRAMESHARE_CONTROL_ARG_s_ {
    unsigned int Cmd;
    unsigned long long Param1;
    unsigned long long Param2;
} LU_EXAMRTSPFRAMESHARE_CONTROL_ARG_s;

typedef struct _LU_EXAMRTSPFRAMESHARE_FRAME_DESC_s_ {
    unsigned int ChannelId; /**< rtsp channel ID */
    unsigned int SeqNum; /**< sequential number of bits buffer */
    unsigned long long Pts; /**< time stamp in ticks */
    unsigned char Type; /**< data type of the entry. see LU_EXAMRTSPFRAMESHARE_FRAME_FRMAE_TYPE_e*/
    unsigned char Completed; /**< if the buffer content a complete entry */
    unsigned short  Align; /** data size alignment (in bytes, align = 2^n, n is a integer )*/
    unsigned long long StartAddr; /**< start address of data */
    unsigned int Size; /**< real data size */
} LU_EXAMRTSPFRAMESHARE_FRAME_DESC_s;

typedef struct _LU_EXAMRTSPFRAMESHARE_FRAMEEVENT_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    LU_EXAMRTSPFRAMESHARE_FRAME_DESC_s InfoPtr;
} LU_EXAMRTSPFRAMESHARE_FRAMEEVENT_ARG_s;

typedef struct _LU_EXAMRTSPFRAMESHARE_ENCINFO_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    unsigned int media_type;
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
} LU_EXAMRTSPFRAMESHARE_ENCINFO_ARG_s;

typedef struct _LU_EXAMRTSPFRAMESHARE_MSG_CAP_s_ {
    unsigned int ChannelId; /**< rtsp channel ID*/
    unsigned long long Pts; /**< time stamp in ticks */
    unsigned long long CapSeqNum; /**< Raw capture sequence number */
} LU_EXAMRTSPFRAMESHARE_MSG_CAP_s;

typedef enum _LU_EXAMRTSPFRAMESHARE_MSG_e_ {
    LU_EXAMRTSPFRAMESHARE_MSG_CAP = 1,        /**< Capture message */
} LU_EXAMRTSPFRAMESHARE_MSG_e;

typedef struct _LU_EXAMRTSPFRAMESHARE_MSG_ARG_s_ {
    unsigned int Msg;
    unsigned int Payload[RTSPFRAMESHARE_MSG_SIZE/4-1];
} LU_EXAMRTSPFRAMESHARE_MSG_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_EXAMRTSPFRAMESHARE_FUNC_e_ {
    LU_EXAMRTSPFRAMESHARE_FUNC_CONTROLEVENT = 1,
    LU_EXAMRTSPFRAMESHARE_FUNC_FRAMEEVENT,
    LU_EXAMRTSPFRAMESHARE_FUNC_SETENCINFO,
    LU_EXAMRTSPFRAMESHARE_FUNC_MSG,

    LU_EXAMRTSPFRAMESHARE_FUNC_AMOUNT
};

/**
 * [in] LU_EXAMRTSPFRAMESHARE_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMRTSPFRAMESHARE_ControlEvent_Clnt(LU_EXAMRTSPFRAMESHARE_CONTROL_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMRTSPFRAMESHARE_ControlEvent_Svc(LU_EXAMRTSPFRAMESHARE_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMRTSPFRAMESHARE_FRAMEEVENT_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMRTSPFRAMESHARE_FrameEvent_Clnt(LU_EXAMRTSPFRAMESHARE_FRAMEEVENT_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMRTSPFRAMESHARE_FrameEvent_Svc(LU_EXAMRTSPFRAMESHARE_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMRTSPFRAMESHARE_ENCINFO_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMRTSPFRAMESHARE_SetEncInfo_Clnt(LU_EXAMRTSPFRAMESHARE_ENCINFO_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMRTSPFRAMESHARE_SetEncInfo_Svc(LU_EXAMRTSPFRAMESHARE_ENCINFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMRTSPFRAMESHARE_MSG_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMRTSPFRAMESHARE_Msg_Clnt(LU_EXAMRTSPFRAMESHARE_MSG_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_EXAMRTSPFRAMESHARE_Msg_Svc(LU_EXAMRTSPFRAMESHARE_MSG_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_EXAMRTSPFRAMESHARE_H_ */

