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

#ifndef _AMBA_IPC_RPC_PROG_R_SUPERCAMCB_H_
#define _AMBA_IPC_RPC_PROG_R_SUPERCAMCB_H_

#include "AmbaIPC_Rpc_Def.h"

/* user needs to define the program id*/
#define AMBA_RPC_PROG_R_SUPERCAMCB_PROG_ID              0x1000001D /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_R_SUPERCAMCB_VER                  1 /* RPC Program version */

#define AMBA_RPC_PROG_R_SUPERCAMCB_HAL              1
typedef struct _AMBA_RPC_PROG_R_SUPERCAMCB_ARG_s_ {
    unsigned int cmd;
    unsigned int id_mask;
    unsigned int param[11];
} AMBA_RPC_PROG_R_SUPERCAMCB_ARG_t;

typedef enum {
        SUPERCAM_RPC_CMD_INIT = 1,
        SUPERCAM_RPC_CMD_PREVIEW_START = 2,
        SUPERCAM_RPC_CMD_ENCODE_START = 3,
        SUPERCAM_RPC_CMD_CAPTURE_START = 4,
        SUPERCAM_RPC_CMD_CAPTURE_STOP = 5,
        SUPERCAM_RPC_CMD_ENCODE_STOP = 6,
        SUPERCAM_RPC_CMD_PREVIEW_STOP = 7,
        SUPERCAM_RPC_CMD_DEINIT = 8,
        
        SUPERCAM_RPC_CMD_GET_STATUS = 9,
        SUPERCAM_RPC_CMD_POWER_ON = 0xA,
        SUPERCAM_RPC_CMD_POWER_OFF = 0xB,
        SUPERCAM_RPC_CMD_FW_UPGRADE = 0xC,

        SUPERCAM_RPC_CMD_LOG_DUMP = 0xD,
        SUPERCAM_RPC_CMD_DSP_LOG_CFG = 0xE,
        SUPERCAM_RPC_CMD_COPY_TO_PC = 0xF,
        
        SUPERCAM_RPC_CMD_AE_ROI = 0x10,
        SUPERCAM_RPC_CMD_RUNTIME_MODE = 0x11,

        SUPERCAM_RPC_CMD_RESET_3A = 0x12,
        SUPERCAM_RPC_CMD_PREVIEW_SETUP = 0x13,
        SUPERCAM_RPC_CMD_ITUNER = 0x14,

        SUPERCAM_RPC_CMD_CALI_SETUP_CAMCONFIG = 0x15,
        SUPERCAM_RPC_CMD_CALI_BACKUP = 0x16,
        SUPERCAM_RPC_CMD_CALI_RESTORE = 0x17,
        SUPERCAM_RPC_CMD_CALI_ERASE = 0x18,
        SUPERCAM_RPC_CMD_CALI_GETINFO = 0x19,
} SUPERCAM_RPC_CMD_ID_e;

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_SupercamCB_Hal_Clnt(AMBA_RPC_PROG_R_SUPERCAMCB_ARG_t *pArg, int *pResult, CLIENT_ID_t Clnt);
void AmbaRpcProg_R_SupercamCB_Hal_Svc(AMBA_RPC_PROG_R_SUPERCAMCB_ARG_t *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif
