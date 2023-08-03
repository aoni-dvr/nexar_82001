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
#ifndef _RPC_PROG_LU_EXAMFRAMESHARERTSP_H_
#define _RPC_PROG_LU_EXAMFRAMESHARERTSP_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_EXAMFRAMESHARERTSP_PROG_ID   0x2000000C
#define LU_EXAMFRAMESHARERTSP_HOST AMBA_IPC_HOST_LINUX
#define LU_EXAMFRAMESHARERTSP_VER (1)
#define LU_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT (FOREVER)
#define LU_EXAMFRAMESHARERTSP_NAME "AMBAEXAMFRAMESHARERTSP_LUSVC"

typedef struct _LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s_ {
    unsigned int Cmd;
    unsigned long long Param1;
    unsigned long long Param2;
} LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s;

typedef enum _RT_EXAMFRAMESHARERTSP_FRMAE_TYPE_e_ {
    RT_EXAMFRAMESHARERTSP_TYPE_MJPEG_FRAME = 0,    ///< MJPEG frame type
    RT_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME = 1,    ///< Idr frame type
    RT_EXAMFRAMESHARERTSP_TYPE_I_FRAME = 2,      ///< I frame type
    RT_EXAMFRAMESHARERTSP_TYPE_P_FRAME = 3,      ///< P frame type
    RT_EXAMFRAMESHARERTSP_TYPE_B_FRAME = 4,      ///< B frame type
    RT_EXAMFRAMESHARERTSP_TYPE_JPEG_FRAME = 5,   ///< jpeg main frame
    RT_EXAMFRAMESHARERTSP_TYPE_THUMBNAIL_FRAME = 6,  ///< jpeg thumbnail frame
    RT_EXAMFRAMESHARERTSP_TYPE_SCREENNAIL_FRAME = 7, ///< jpeg screennail frame
    RT_EXAMFRAMESHARERTSP_TYPE_AUDIO_FRAME = 8,      ///< audio frame
    RT_EXAMFRAMESHARERTSP_TYPE_UNDEFINED = 9,        ///< others

    RT_EXAMFRAMESHARERTSP_TYPE_DECODE_MARK = 101, ///< used when feeding bitstream to dsp. will push out all frame. */
    RT_EXAMFRAMESHARERTSP_TYPE_EOS = 255,                  ///< eos bits that feed to raw buffer

    RT_EXAMFRAMESHARERTSP_TYPE_LAST = RT_EXAMFRAMESHARERTSP_TYPE_EOS
} RT_EXAMFRAMESHARERTSP_FRMAE_TYPE_e;


typedef struct _LU_EXAMFRAMESHARERTSP_FRAME_DESC_s_ {
    unsigned int TrackType; /**< track type */
    unsigned int SeqNum; /**< sequential number of bits buffer */
    unsigned long long Pts; /**< time stamp in ticks */
    unsigned char Type; /**< data type of the entry. see LU_EXAMFRAMESHARE_FRAME_FRMAE_TYPE_e*/
    unsigned char Completed; /**< if the buffer content a complete entry */
    unsigned short  Align; /** data size alignment (in bytes, align = 2^n, n is a integer )*/
    unsigned long long StartAddr; /**< start address of data */
    unsigned int Size; /**< real data size */
    unsigned int StrmId;
} LU_EXAMFRAMESHARERTSP_FRAME_DESC_s;

typedef struct _LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    LU_EXAMFRAMESHARERTSP_FRAME_DESC_s InfoPtr;
} LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s;


typedef struct _LU_EXAMFRAMESHARERTSP_ENCINFO_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    unsigned int media_type;
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
} LU_EXAMFRAMESHARERTSP_ENCINFO_ARG_s;

//============ RPC_FUNC definition ============
enum _LU_EXAMFRAMESHARERTSP_FUNC_e_ {
    LU_EXAMFRAMESHARERTSP_FUNC_CONTROLEVENT = 1,
    LU_EXAMFRAMESHARERTSP_FUNC_FRAMEEVENT,
    LU_EXAMFRAMESHARERTSP_FUNC_PB_FRAMEEVENT,
    LU_EXAMFRAMESHARERTSP_FUNC_GSENSOR_FRAMEEVENT,
    LU_EXAMFRAMESHARERTSP_FUNC_AMOUNT
};

/**
 * [in] LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMFRAMESHARERTSP_ControlEvent_Clnt(LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt );
void LU_EXAMFRAMESHARERTSP_ControlEvent_Svc(LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMFRAMESHARERTSP_FrameEvent_Clnt(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt );
void LU_EXAMFRAMESHARERTSP_FrameEvent_Svc(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
void LU_EXAMFRAMESHARERTSP_PbFrameEvent_Svc(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
void LU_EXAMFRAMESHARERTSP_GSensorFrameEvent_Svc(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] LU_EXAMFRAMESHARERTSP_ENCINFO_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_EXAMFRAMESHARERTSP_SetEncInfo_Clnt(LU_EXAMFRAMESHARERTSP_ENCINFO_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt );
void LU_EXAMFRAMESHARERTSP_SetEncInfo_Svc(LU_EXAMFRAMESHARERTSP_ENCINFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_EXAMFRAMESHARERTSP_H_ */

