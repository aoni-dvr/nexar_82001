/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIPC_RpcProg_SupercamCB.h
 *
 *  @Copyright      :: Copyright (C) 2017 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for AmbaIPC RPC Supercam ControlBox program
 *
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_IPC_RPC_PROG_R_SUPERCAMBUTTONLED_H_
#define _AMBA_IPC_RPC_PROG_R_SUPERCAMBUTTONLED_H_

#include "AmbaIPC_Rpc_Def.h"

/* user needs to define the program id*/
#define AMBA_RPC_PROG_R_SUPERCAMLED_PROG_ID              0x1000001E /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_R_SUPERCAMLED_VER                  1 /* RPC Program version */



typedef enum {
    SUPERCAM_LED_TYPE_REC = 0,
    SUPERCAM_LED_TYPE_AUTO = 1,
    
    SUPERCAM_LED_TYPE_TOTAL_NUM = 2
} SUPERCAM_LED_TYPE_e;

#define AMBA_RPC_PROG_R_SUPERCAMLED_HAL              1
typedef struct _AMBA_RPC_PROG_R_SUPERCAMLED_ARG_s_ {
    SUPERCAM_LED_TYPE_e type;
    int hz;
} AMBA_RPC_PROG_R_SUPERCAMLED_ARG_t;


AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_SupercamLed_Hal_Clnt(AMBA_RPC_PROG_R_SUPERCAMLED_ARG_t *pArg, int *pResult, CLIENT_ID_t Clnt);
void AmbaRpcProg_R_SupercamLed_Hal_Svc(AMBA_RPC_PROG_R_SUPERCAMLED_ARG_t *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif
