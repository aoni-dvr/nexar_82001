/**
 * @file AmbaIPC_RpcProg_Util.c
 *
 * Copyright (c) 2020 Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @details AmbaIPC RPC utility programs are implemented here
 *
 */
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaLink.h"
#include "AmbaPrint.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#include "libwrapc.h"
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Exec1_Clnt
 *
 *  @Description::  The execution result will be put in Result.
 *
 *  @Return     ::  AMBA_IPC_REPLY_STATUS
 *
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_Util_Exec1_Clnt(AMBA_RPC_PROG_EXEC_ARG_s *pArg,
                                                    int *Result, CLIENT_ID_t Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_UTIL_EXEC1, pArg,
                    sizeof(AMBA_RPC_PROG_EXEC_ARG_s) + strlen(pArg->command) + 1,
                    Result, EXEC_OUTPUT_SIZE, 5000);
    return status;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Exec2_Clnt
 *
 *  @Description::  Server does not return the exeuction result.
 *
 *  @Return     ::  AMBA_IPC_REPLY_STATUS
 *
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_Util_Exec2_Clnt(AMBA_RPC_PROG_EXEC_ARG_s *pArg,
                                                    int *Result, CLIENT_ID_t Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;

    Result = Result;
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_UTIL_EXEC2, pArg,
                    sizeof(AMBA_RPC_PROG_EXEC_ARG_s) + strlen(pArg->command) + 1,
                    NULL, 0, 0);
    return status;
}