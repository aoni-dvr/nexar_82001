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
#ifndef _RPC_PROG_LU_SUPERCAM_GETFWIMAGE_H_
#define _RPC_PROG_LU_SUPERCAM_GETFWIMAGE_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

#define LENGTH_FW_PATH 44

//RPC_INFO definition
#define LU_SUPERCAM_GETFWIMAGE_PROG_ID	0x2000000F
#define LU_SUPERCAM_GETFWIMAGE_HOST AMBA_IPC_HOST_LINUX
#define LU_SUPERCAM_GETFWIMAGE_VER (1)
#define LU_SUPERCAM_GETFWIMAGE_DEFULT_TIMEOUT (FOREVER)
#define LU_SUPERCAM_GETFWIMAGE_NAME "SUPERCAM_GETFWIMAGE_LUSVC"

typedef struct _LU_SUPERCAM_GETFWIMAGE_ARG_s_ {
    char path[LENGTH_FW_PATH];
} LU_SUPERCAM_GETFWIMAGE_ARG_s;


//============ RPC_FUNC definition ============
enum _LU_EXAMFRAMESHARE_FUNC_e_ {
    LU_SUPERCAM_GETFWIMAGE_FUNC_GETFWIMAGE = 1,
    LU_SUPERCAM_GETFWIMAGE_FUNC_AMOUNT
};

/**
 * [in] LU_SUPERCAM_GETFWIMAGE_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_SUPERCAM_GETFWIMAGE_GetFWImage_Clnt(LU_SUPERCAM_GETFWIMAGE_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_SUPERCAM_GETFWIMAGE_GetFWImage_Svc(LU_SUPERCAM_GETFWIMAGE_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_SUPERCAM_GETFWIMAGE_H_ */

