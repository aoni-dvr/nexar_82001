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
#ifndef _RPC_PROG_LU_SUPERCAM_GETSYSTIME_H_
#define _RPC_PROG_LU_SUPERCAM_GETSYSTIME_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

#define AMBA_RPC_PROG_LU_SUPERCAM_GETSYSTIME_PROG_ID		0x2000001D
#define AMBA_RPC_PROG_LU_SUPERCAM_GETSYSTIME_VER			1
#define LU_SUPERCAM_GETSYSTIME_NAME "SUPERCAM_GETSYSTIME_LUSVC"
#define LU_SUPERCAM_GETSYSTIME_DEFULT_TIMEOUT (FOREVER)

#define AMBA_RPC_PROG_LU_GETSYSTIME            1


//============ RPC_FUNC definition ============
enum _LU_GETSYSTIME_FUNC_e_ {
    LU_SUPERCAM_GETSYSTIME_FUNC_NUM = 1,
    LU_SUPERCAM_GETSYSTIME_FUNC_AMOUNT
};

typedef struct SUPERCAM_SYSTIME_s {
    int sec;
    int nsec;
}SUPERCAM_SYSTIME_t;

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_GET_SYSTIME_Clnt(int *enable, int *pResult, CLIENT_ID_t Clnt);
void AmbaRpcProg_LU_GET_SYSTIME_Svc(int *enable, AMBA_IPC_SVC_RESULT_s *pRet);


#endif /* _RPC_PROG_LU_SUPERCAM_GETSYSTIME_H_ */

