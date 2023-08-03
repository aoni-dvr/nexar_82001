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
#ifndef _RPC_PROG_LU_SUPERDRONE_STATUSUPDATE_H_
#define _RPC_PROG_LU_SUPERDRONE_STATUSUPDATE_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define LU_SUPERDRONE_STATUSUPDATE_PROG_ID	0x20000200
#define LU_SUPERDRONE_STATUSUPDATE_HOST AMBA_IPC_HOST_LINUX
#define LU_SUPERDRONE_STATUSUPDATE_VER (1)
#define LU_SUPERDRONE_STATUSUPDATE_DEFULT_TIMEOUT (FOREVER)
#define LU_SUPERDRONE_STATUSUPDATE_NAME "SUPERDRONE_STATUS_LUSVC"

typedef struct _LU_STATUSUPDATE_CAPSEQNUM_ARG_s_ {
    unsigned int CapSeqNum;
} LU_SUPERDRONE_CAPSEQNUM_ARG_s;


//============ RPC_FUNC definition ============
enum _LU_STATUSUPDATE_FUNC_e_ {
    LU_SUPERDRONE_STATUSUPDATE_FUNC_CAPSEQNUM = 1,
    LU_SUPERDRONE_STATUSUPDATE_FUNC_AMOUNT
};

/**
 * [in] LU_SUPERDRONE_STATUSUPDATE_ARG_s
 * [out] int
 */
void LU_SUPERDRONE_STATUSUPDATE_CapSeqNum_Svc(LU_SUPERDRONE_CAPSEQNUM_ARG_s *pArg,
                                             AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_LU_SUPERDRONE_STATUSUPDATE_H_ */

