/**
 * @file SvcRpmsgApp.c
 *-------------------------------------------------------------------------------------------------------------------*\
 *  FileName       :: AmbaIPC_Test.c
 *
 *  Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  Description    :: Test/Reference code for AmbaIPC
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaLink.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_R_Test.h"
#include "AmbaIPC_RpcProg_LU_Test.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#include "AmbaIPC_RpcProg_LU_PT.h"
#include "AmbaIPC_RpcProg_R_PT.h"
#include "AmbaShell_Commands.h"
#include "libwrapc.h"
#include "AmbaUART.h"
#include "AmbaIPC_Rfs.h"
#include "SvcRpmsgApp.h"

#ifdef CONFIG_AMBALINK_BOOT_OS
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_MsgHandler
 *
 *  @Description::  Channel Message Handler
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static int SvcRpmsg_MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
#define EXECMAGIC "exec: "
    extern void AmbaLink_EchoCh_Callback(char *Msg) __attribute__((weak));

    IpcHandle = IpcHandle;
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "recv message: [%s]", (char *)pMsgCtrl->pMsgData, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    if (AmbaLink_EchoCh_Callback != NULL) {
        AmbaLink_EchoCh_Callback((char *)pMsgCtrl->pMsgData);
    }

    if (!strcmp((char *)pMsgCtrl->pMsgData, EXECMAGIC)) {
        extern UINT32 AmbaShell_ExecCommand(char *pCmdLine);
        AmbaShell_ExecCommand((char *)pMsgCtrl->pMsgData + strlen(EXECMAGIC));
    }
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_TestInit
 *
 *  @Description::  Init test module
 *
 *  @Return     ::
 *      int : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
int SvcRpmsgApp(void)
{
    AMBA_IPC_HANDLE EchoChannel;
    /*
     * Note: When run as CONFIG_AMBALINK_RPMSG_ROLE_MAST and connect to Linux,
     *       the name is "echo_ca9_b"
     */

#ifdef CONFIG_OPENAMP
    EchoChannel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "echo_cortex", SvcRpmsg_MsgHandler);
#else
    EchoChannel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "echo_cortex", SvcRpmsg_MsgHandler);
#endif
    if (EchoChannel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: SvcRpmsgApp failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return -1;
    }
    AmbaIPC_RegisterChannel(EchoChannel, NULL);

    return 0;
}
#endif
