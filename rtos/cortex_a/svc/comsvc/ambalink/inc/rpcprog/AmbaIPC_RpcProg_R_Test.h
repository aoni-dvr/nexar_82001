/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIPC_RpcProg_Test.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for AmbaIPC RPC test program.
 *
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IPC_RPC_PROG_R_TEST_H_
#define _AMBA_IPC_RPC_PROG_R_TEST_H_

/*----------------------------------------------------------------------------*\
 * PROGRAM Test {
 *        void printf(const char *pStr) = 1; [async]
 *        int sum(int a, int b) = 2;
 * }
\*----------------------------------------------------------------------------*/
#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_R_TEST                  1	/* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_R_TEST_PROG_ID		0x10000001 /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_R_TEST_VER			1 /* RPC Program version */


/* printf(void *) = 1; [async] */
#define AMBA_RPC_PROG_R_TEST_PRINT            1
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_Test_Print_Clnt(const char *pStr, int *pResult, CLIENT_ID_t Clnt);
void AmbaRpcProg_R_Test_Print_Svc(const char *pStr, AMBA_IPC_SVC_RESULT_s *pRet);


/* int sum(int a, int b) = 2; */
#define AMBA_RPC_PROG_R_TEST_SUM              2
typedef struct _AMBA_RPC_PROG_R_TEST_SUM_ARG_s_ {
    int a;
    int b;
} AMBA_RPC_PROG_R_TEST_SUM_ARG_s;

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_Test_Sum_Clnt(AMBA_RPC_PROG_R_TEST_SUM_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void AmbaRpcProg_R_Test_Sum_Svc(AMBA_RPC_PROG_R_TEST_SUM_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif
