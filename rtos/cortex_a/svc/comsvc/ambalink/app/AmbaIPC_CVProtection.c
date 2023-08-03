/**
 *  @file AmbaIPC_CVProtection.c
 *
 *  Copyright (c) 2020 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  This file includes sample code and is only for internal testing and evaluation.  If you
 *  distribute this sample code (whether in source, object, or binary code form), it will be
 *  without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Test/Reference code for AmbaIPC CV Protection
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaINT.h"
#include "AmbaLink.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaMisraFix.h"
#include "AmbaMMU.h"
#include "AmbaShell_Commands.h"
#include "libwrapc.h"
#include "AmbaLinkPrivate.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_RT_CVProtection.h"

#define USER_PROC_INFO             1024U

extern void HL_GetProtectBufInfo(ULONG *Addr, UINT32 *Size);

static void AmbaIPC_CVProtection_Svc_Info(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AMBA_RPC_RT_CVPROTECTION_INFO_s *ptr;
    ULONG Addr = 0U;
    UINT32 Size = 0U;
    void *pAddr;

    (void) pMsg;
    HL_GetProtectBufInfo(&Addr, &Size);

    ptr = (AMBA_RPC_RT_CVPROTECTION_INFO_s *)(pRet->pResult);
    AmbaMisra_TypeCast(&pAddr, &Addr);
    AmbaWrap_memcpy(&ptr->Info[0], pAddr, Size);
    //send ipc reply
    pRet->Length = Size;
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

UINT32 AmbaIPC_CVProtectionSvcInit(void)
{
    UINT32 ret = 0U;
    int status;
    AMBA_IPC_PROG_INFO_s prog_info[1];
    static UINT8 RpcCVProtectionSvcStack[0x4000U] GNU_SECTION_NOZEROINIT;
    static UINT32 ProcInfo[USER_PROC_INFO];

    prog_info->ProcNum = 1;
    if( (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)) > USER_PROC_INFO) {
        AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_CVProtection_Init (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s))(%d) is big than %d\n", (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)), USER_PROC_INFO, 0U, 0U, 0U);
        AmbaPrint_Flush();
        ret = 1U;
    } else {
        memset(ProcInfo, 0 ,sizeof(ProcInfo));
        prog_info->pProcInfo = (AMBA_IPC_PROC_s *)ProcInfo;
        prog_info->pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaIPC_CVProtection_Svc_Info;
        status = AmbaIPC_SvcRegister(AMBA_RPC_PROG_RT_CVPROTECTION_PROG_ID, AMBA_RPC_PROG_RT_CVPROTECTION_VER, "CVProtection_rpc_rt_svc",
                    65U, (void *)&RpcCVProtectionSvcStack[0U], 0x4000U, prog_info, 1);
        if(status != 0) {
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaIPC_CVProtection_Init AmbaIPC_SvcRegister fail (%d)\n", status, 0U, 0U, 0U, 0U);
            ret = 1U;
        }
    }

    return ret;
}
