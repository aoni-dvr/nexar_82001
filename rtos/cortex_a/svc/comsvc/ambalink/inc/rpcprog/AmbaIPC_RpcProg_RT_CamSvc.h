/**
*  @file AmbaIPC_RpcProg_RT_CamSvc.h
*
*  @copyright Copyright (c) 2018 Ambarella, Inc.
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
*  @details Definitions for AmbaIPC Cam Service.
*
*/

#ifndef _RPC_PROG_RT_CAMSVC_H_
#define _RPC_PROG_RT_CAMSVC_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define RT_CAMSVC_PROG_ID 0x10000202
#define RT_CAMSVC_HOST    AMBA_IPC_HOST_THREADX
#define RT_CAMSVC_VER     (1)
#define RT_CAMSVC_DEFULT_TIMEOUT  (FOREVER)
#define RT_CAMSVC_NAME    "RPC_SVC_CAMSVC"

//Data types
typedef struct RT_CamSvc_AeCurrLv_s_ {
    unsigned int Res;
    unsigned int CurrLv;
} RT_CamSvc_AeCurrLv_s;

//============ RPC_FUNC definition ============
enum _RT_CAMSVC_FUNC_e_ {
    RT_CAMSVC_FUNC_GETAECURRLV = 1,

    RT_CAMSVC_FUNC_AMOUNT
};

/**
 * [in] unsigned int
 * [out] RT_CamSvc_AeCurrLv_s
 */
AMBA_IPC_REPLY_STATUS_e RT_CamSvc_GetAeCurrLv_Clnt(unsigned int *pArg, RT_CamSvc_AeCurrLv_s *pResult, CLIENT_ID_t Clnt);
void RT_CamSvc_GetAeCurrLv_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_RT_CAMSVC_H_ */

