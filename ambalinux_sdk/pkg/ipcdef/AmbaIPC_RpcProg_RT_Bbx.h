/**
*  @file AmbaIPC_RpcProg_RT_Bbx.h
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
*  @details Definitions for AmbaIPC Bounding Box.
*
*/

#ifndef _RPC_PROG_RT_BBX_H_
#define _RPC_PROG_RT_BBX_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define RT_BBX_PROG_ID 0x10000201
#define RT_BBX_HOST    AMBA_IPC_HOST_THREADX
#define RT_BBX_VER     (1)
#define RT_BBX_DEFULT_TIMEOUT  (FOREVER)
#define RT_BBX_NAME    "RPC_SVC_BBX"

//Data types
typedef struct RT_Bbx_Buffer_Setting_s_ {
    unsigned int MemType;
    unsigned int Size;
    unsigned int Align;
} RT_Bbx_Buffer_Setting_s;

typedef struct RT_Bbx_Buffer_Info_s_ {
    unsigned long long RawAddr;
    unsigned long long AlignedAddr;
} RT_Bbx_Buffer_Info_s;

typedef struct RT_Bbx_Info_s_ {
    unsigned int Size;
    unsigned long long Addr;
} RT_Bbx_Info_s;

//============ RPC_FUNC definition ============
enum _RT_BBX_FUNC_e_ {
    RT_BBX_FUNC_CREATEBUF = 1,
    RT_BBX_FUNC_FREEBUF,
    RT_BBX_FUNC_SEND,

    RT_BBX_FUNC_AMOUNT
};

/**
 * [in] RT_Bbx_Buffer_Setting_s
 * [out] RT_Bbx_Buffer_Info_s
 */
AMBA_IPC_REPLY_STATUS_e RT_Bbx_CreateBuf_Clnt(RT_Bbx_Buffer_Setting_s *pArg, RT_Bbx_Buffer_Info_s *pResult, CLIENT_ID_t Clnt);
void RT_Bbx_CreateBuf_Svc(RT_Bbx_Buffer_Setting_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_Bbx_Buffer_Info_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_Bbx_FreeBuf_Clnt(RT_Bbx_Buffer_Info_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void RT_Bbx_FreeBuf_Svc(RT_Bbx_Buffer_Info_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] RT_Bbx_Info_s
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_Bbx_Send_Clnt(RT_Bbx_Info_s *pArg, int *pResult, CLIENT_ID_t Clnt);
void RT_Bbx_Send_Svc(RT_Bbx_Info_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_RT_CAMCTRL_H_ */

