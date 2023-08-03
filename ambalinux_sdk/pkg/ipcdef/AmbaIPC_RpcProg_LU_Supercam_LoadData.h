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
#ifndef _RPC_PROG_LU_SUPERCAM_LOADDATA_H_
#define _RPC_PROG_LU_SUPERCAM_LOADDATA_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_SUPERCAM_LOADDATA_PROG_ID	0x20000104
#define LU_SUPERCAM_LOADDATA_HOST AMBA_IPC_HOST_LINUX
#define LU_SUPERCAM_LOADDATA_VER (1)
#define LU_SUPERCAM_LOADDATA_DEFULT_TIMEOUT (FOREVER)
#define LU_SUPERCAM_LOADDATA_NAME "SUPERCAM_LOADDATATOMEM_LUSVC"

#define LU_SUPERCAM_LOADDATA_PATH_LENGTH  (256)
typedef struct _LU_LOADDATA_CONTROL_ARG_s_ {    
    unsigned long long buf_base; /**< base address of data buffer */
    unsigned long long buf_base_phy; /**< physical base address of data buffer */
    unsigned int buf_size; /**< base address of data buffer */
    unsigned long long file_start_offset; /**< file start offset to read */
    unsigned int file_read_size; /**< specific file read size, 0 means the whole file */
    char file_path[LU_SUPERCAM_LOADDATA_PATH_LENGTH];
} LU_SUPERCAM_LOADDATA_ARG_s;


//============ RPC_FUNC definition ============
enum _LU_LOADIQ_FUNC_e_ {
    LU_SUPERCAM_LOADDATA_FUNC_LOADDATATOMEM = 1,
    LU_SUPERCAM_LOADDATA_FUNC_AMOUNT
};

/**
 * [in] LU_SUPERCAM_LOADDATA_ARG_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e LU_SUPERCAM_LOADDATA_LoadDataToMem_Clnt(LU_SUPERCAM_LOADDATA_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void LU_SUPERCAM_LOADDATA_LoadDataToMem_Svc(LU_SUPERCAM_LOADDATA_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_SUPERCAM_READDATA_H_ */

