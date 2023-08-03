/**
*  @file AmbaIPC_RpcProg_RT_SharePreview.h
*
*  @copyright Copyright (c) 2016 Ambarella, Inc.
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
*  @details Definitions for AmbaIPC Share Preview.
*
*/

#ifndef _RPC_PROG_RT_SHAREPREVIEW_H_
#define _RPC_PROG_RT_SHAREPREVIEW_H_

#include "AmbaIPC_Rpc_Def.h"

#ifndef FOREVER
#define FOREVER 0xFFFFFFFF
#endif

//RPC_INFO definition
#define RT_SHAREPREVIEW_PROG_ID 0x1000000C
#define RT_SHAREPREVIEW_HOST    AMBA_IPC_HOST_THREADX
#define RT_SHAREPREVIEW_VER     (1)
#define RT_SHAREPREVIEW_DEFULT_TIMEOUT  (FOREVER)
#define RT_SHAREPREVIEW_NAME    "APPLIB_RPC_SVC_SHAREPREVIEW"

//Data types
#define SHAREPREVIEW_CACHED_AMOUNT  (3)
typedef struct _RT_SHAREPREVIEW_FRAMEINFO_s_ {
    unsigned short Pitch;
    unsigned short Width;
    unsigned short Height;
    unsigned short Padding;
    unsigned int Size;
} RT_SHAREPREVIEW_FRAMEINFO_s;

//For sharemem info, it has to be AMBA_CACHE_LINE_SIZE aligned.
typedef struct _RT_SHAREPREVIEW_WRINFO_s_ {
    unsigned long long Rawaddr;
    unsigned int Active;
    RT_SHAREPREVIEW_FRAMEINFO_s Finfo[SHAREPREVIEW_CACHED_AMOUNT];
    unsigned long long Data[SHAREPREVIEW_CACHED_AMOUNT];
} RT_SHAREPREVIEW_WRINFO_s;

typedef struct _RT_SHAREPREVIEW_RDINFO_s_ {
    unsigned long long Rawaddr;
    unsigned int Reading;
} RT_SHAREPREVIEW_RDINFO_s;

typedef struct _RT_SHAREPREVIEW_MEMINFO_s_ {
    unsigned long long ShareInfo;
    unsigned long long ShareInfoPhyAddr;
    unsigned int ShareInfoSize;
    unsigned long long ReaderInfo;
    unsigned long long ReaderInfoPhyAddr;
    unsigned int ReaderInfoSize;
} RT_SHAREPREVIEW_MEMINFO_s;

enum _RT_SHAREPREVIEW_STATUS_e_ {
    RT_SHAREPREVIEW_STATUS_NOTINIT = 0, /**< Not inited */
    RT_SHAREPREVIEW_STATUS_INITED,      /**< Inited */
    RT_SHAREPREVIEW_STATUS_ENABLED,     /**< Enabled*/
};

//============ RPC_FUNC definition ============
enum _RT_SHAREPREVIEW_FUNC_e_ {
    RT_SHAREPREVIEW_FUNC_GETSHAREMEMINFO = 1,
    RT_SHAREPREVIEW_FUNC_ENABLE,
    RT_SHAREPREVIEW_FUNC_GETSTATUS,

    RT_SHAREPREVIEW_FUNC_AMOUNT
};

/**
 * [in] NULL
 * [out] RT_SHAREPREVIEW_MEMINFO_s
 */
AMBA_IPC_REPLY_STATUS_e RT_SharePreview_GetMemInfo_Clnt(void *pArg, RT_SHAREPREVIEW_MEMINFO_s *pResult, CLIENT_ID_t Clnt);
void RT_SharePreview_GetMemInfo_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] int
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_SharePreview_Enable_Clnt(int *pArg, int *pResult, CLIENT_ID_t Clnt);
void RT_SharePreview_Enable_Svc(int *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

/**
 * [in] NULL
 * [out] int
 */
AMBA_IPC_REPLY_STATUS_e RT_SharePreview_GetStatus_Clnt(void *pArg, int *pResult, CLIENT_ID_t Clnt);
void RT_SharePreview_GetStatus_Svc(void *pArg, AMBA_IPC_SVC_RESULT_s *pRet);

#endif /* _RPC_PROG_RT_SHAREPREVIEW_H_ */

