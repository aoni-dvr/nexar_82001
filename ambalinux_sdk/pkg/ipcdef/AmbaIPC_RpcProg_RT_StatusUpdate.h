/**
*  @file AmbaIPC_RpcProg_RT_UpdateStatus.h
*
*  @copyright Copyright (c) 2017 Ambarella, Inc.
*
*  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
*  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
*  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
*  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
*  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
*  return this Software to Ambarella, Inc.
*
*  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
*  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  @details Definitions for AmbaIPC Status Update.
*
*/

#ifndef _RPC_PROG_RT_STATUSUPDATE_H_
#define _RPC_PROG_RT_STATUSUPDATE_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

#define RT_STATUSUPDATE_PROG_ID 0x10000101
#define RT_STATUSUPDATE_HOST    AMBA_IPC_HOST_THREADX
#define RT_STATUSUPDATE_VER     (1)
#define RT_STATUSUPDATE_DEFULT_TIMEOUT  (FOREVER)
#define RT_STATUSUPDATE_NAME    "RPC_SVC_UPDATESTATUS"


typedef struct _AMBA_RPC_PROG_R_BOOT_STATUS_ARG_s_ {
    int status;
} AMBA_RPC_PROG_R_BOOT_STATUS_ARG_s;

typedef struct _AMBA_RPC_CAP_INFO_s_ {
    unsigned long long pts;
    unsigned long long cap_seq_num;
} AMBA_RPC_CAP_INFO_s;

typedef enum {
    CAP_INFO_PREVIEW = 1,
    CAP_INFO_MAIN
} sys_info_type_e;

typedef struct _AMBA_RPC_PROG_R_SYS_INFO_ARG_s_ {
    int type;
    union {
        AMBA_RPC_CAP_INFO_s cap;
    }info;
} AMBA_RPC_PROG_R_SYS_INFO_ARG_s;

//============ RPC_FUNC definition ============
enum _RT_STATUSUPDATE_FUNC_e_ {
    RT_STATUSUPDATE_FUNC_BOOT_STATUS = 1,
    RT_STATUSUPDATE_FUNC_SYS_INFO    = 2, /*<Supercam status/information/message */
    RT_STATUSUPDATE_FUNC_AMOUNT
};

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_Boot_Status_Clnt(AMBA_RPC_PROG_R_BOOT_STATUS_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void AmbaRpcProg_R_Boot_Status_Svc(AMBA_RPC_PROG_R_BOOT_STATUS_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
void AmbaRpcProg_R_SYS_IFNO_Svc(AMBA_RPC_PROG_R_SYS_INFO_ARG_s* pArg,
                                AMBA_IPC_SVC_RESULT_s *pRet);

#endif
