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
#ifndef _RPC_PROG_LU_YUVSHARE_H_
#define _RPC_PROG_LU_YUVSHARE_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_YUVSHARE_PROG_ID	0x20000103
#define LU_YUVSHARE_HOST AMBA_IPC_HOST_LINUX
#define LU_YUVSHARE_VER (1)
#define LU_YUVSHARE_DEFULT_TIMEOUT (FOREVER)
#define LU_YUVSHARE_NAME "AMBAYUVSHARE_LUSVC"

typedef enum _LU_YUVSHARE_MEDIA_TYPE_e_ {
    LU_YUVSHARE_MEDIA_TYPE_YUV = 0,              ///< YUV data type
    LU_YUVSHARE_MEDIA_TYPE_RAW,                           ///< RAW data type

    LU_YUVSHARE_MEDIA_TYPE_LAST = 0xFFFF
} LU_YUVSHARE_MEDIA_TYPE_e;


typedef struct _LU_YUVSHARE_CONTROL_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    unsigned int Cmd;
    unsigned long long Param1;
    unsigned long long Param2;
} LU_YUVSHARE_CONTROL_ARG_s;

typedef struct _LU_YUVSHARE_FRAME_DESC_s_ {
    unsigned int Type;
    unsigned long long YAddr;
    unsigned long long UVAddr;
    unsigned short Pitch;
    unsigned short Width;
    unsigned short Height;
    unsigned short Padding;
    unsigned int Size;
    unsigned long long Capseq;
    unsigned long long TimeStamp;
    unsigned char LinearMode;
    unsigned short ActWidth;
    unsigned short ActHeight;
    unsigned int RawOffset[2];
    unsigned int ExposureMs[2];
    unsigned int Param[4];

} LU_YUVSHARE_FRAME_DESC_s;

typedef struct _LU_YUVSHARE_FRAMEEVENT_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    LU_YUVSHARE_FRAME_DESC_s InfoPtr;
} LU_YUVSHARE_FRAMEEVENT_ARG_s;

typedef struct _LU_YUVSHARE_ENCINFO_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    unsigned int media_type;
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
} LU_YUVSHARE_ENCINFO_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_YUVSHARE_FUNC_e_ {
    LU_YUVSHARE_FUNC_CONTROLEVENT = 1,
    LU_YUVSHARE_FUNC_FRAMEEVENT,
    LU_YUVSHARE_FUNC_SETENCINFO,

    LU_YUVSHARE_FUNC_AMOUNT
};

/**
 * [in] LU_YUVSHARE_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_YUVSHARE_ControlEvent_Clnt(LU_YUVSHARE_CONTROL_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_YUVSHARE_ControlEvent_Svc(LU_YUVSHARE_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_YUVSHARE_FRAMEEVENT_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_YUVSHARE_FrameEvent_Clnt(LU_YUVSHARE_FRAMEEVENT_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_YUVSHARE_FrameEvent_Svc(LU_YUVSHARE_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_YUVSHARE_ENCINFO_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_YUVSHARE_SetEncInfo_Clnt(LU_YUVSHARE_ENCINFO_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_YUVSHARE_SetEncInfo_Svc(LU_YUVSHARE_ENCINFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_YUVSHARE_H_ */

