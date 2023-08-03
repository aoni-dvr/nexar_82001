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
#ifndef _RPC_PROG_LU_SUPERCAM_PREVMETADATA_H_
#define _RPC_PROG_LU_SUPERCAM_PREVMETADATA_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_SUPERCAM_PREVMETADATA_PROG_ID	0x20000102
#define LU_SUPERCAM_PREVMETADATA_HOST AMBA_IPC_HOST_LINUX
#define LU_SUPERCAM_PREVMETADATA_VER (1)
#define LU_SUPERCAM_PREVMETADATA_DEFULT_TIMEOUT (FOREVER)
#define LU_SUPERCAM_PREVMETADATA_NAME "SUPERCAM_PREVMETADATA_LUSVC"

typedef struct _LU_PREVMETADATA_CONTROL_ARG_s_ {
    unsigned long long hndlr; /**< hndlr for enc stream */
    SUPERCAM_PREV_METADATA_s SupercamPrevMetaInfo;
} LU_SUPERCAM_PREVMETADATA_ARG_s;


//============ RPC_FUNC definition ============
enum _LU_PREVMETADATA_FUNC_e_ {
    LU_SUPERCAM_PREVMETADATA_FUNC_SEND = 1,
    LU_SUPERCAM_PREVMETADATA_FUNC_AMOUNT
};

/**
 * [in] LU_SUPERCAM_PREVMETADATA_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_SUPERCAM_PREVMETADATA_SendStatus_Clnt(LU_SUPERCAM_PREVMETADATA_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_SUPERCAM_PREVMETADATA_SendStatus_Svc(LU_SUPERCAM_PREVMETADATA_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_SUPERCAM_PREVMETADATA_H_ */

