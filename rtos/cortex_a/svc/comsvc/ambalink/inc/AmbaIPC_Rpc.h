/**
 * @file AmbaIPC_Rpc.h
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
 * @details AmbaIPC Remote-Processor-Call framework APIs
 *
 */

#ifndef AMBAIPC_RPC_H
#define AMBAIPC_RPC_H

#ifndef _AMBA_LINK_H_
#include "AmbaLink.h"
#endif

/*---------------------------------------------------------------------------*\
 * AmbaIPC remote-processor-call framework APIs
\*---------------------------------------------------------------------------*/
/**
 * @defgroup AmbaIPC_RPC Remote-Processor Call of Ambalink
 * @ingroup AmbaLink
 * @{
 */
#include "AmbaIPC_Rpc_Def.h"
//#define RPMSG_DEBUG
//#define RPC_DEBUG   /**< RPMSG_DEBUG must be enabled if RPC_DEBUG is on */
/**
 * @brief This function is used to register a service for a rpc program.
 * In this function, you can specify whether the service run on a defalut a new
 * thread. If a new thread is requested, the parameters related to the new thread
 * should be given.
 *
 * @param [in] prog RPC program ID
 * @param [in] vers RPC progran version
 * @param [in] name The name for RPC program
 * @param [in] priority The priority for the new thread executing the svc
 * @param [in] stack The stack for the new thread
 * @param [in] stack_size The stack size for the new thread
 * @param [in] info RPC program information
 * @param [in] new_thread Specify if a new thread is to run the svc.
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SvcUnregister
 */
int AmbaIPC_SvcRegister(
    INT32 prog,
    INT32 vers,
    char * const name,
    UINT32 priority,
    void *stack, UINT32 stack_size, AMBA_IPC_PROG_INFO_s *info,
    int new_thread);

/**
 * @brief This function is used to unrefister a service.
 *
 * @param [in] program RPC program ID
 * @param [in] version RPC program version
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_SvcRegister
 */
int AmbaIPC_SvcUnregister(INT32 program, INT32 version);

/**
 * The defintion for rpc client ID
 *
 * @see AmbaIPC_ClientCreate
 */
typedef void* CLIENT_ID_t;
/**
 * @brief This function is used to create a client to the RPC program.
 * A connection is created, when a client is created. The connection is
 * for the specific program id and version. Note that RPC program id and
 * version should be consistent with the server. The client id will be
 * returned, if creating a client is successful.
 *
 * @param [in] host Specify the server is in Linux or RTOS.
 * @param [in] program RPC program ID
 * @param [in] version RPC program version
 *
 * @return Nonzero - OK, NULL - NG
 * @see AmbaIPC_ClientDestroy
 * @see AMBA_IPC_HOST_e
 */
CLIENT_ID_t AmbaIPC_ClientCreate(INT32 host, INT32 program, INT32 version);

/**
 * @brief This function is used to destroy a client to the RPC program.
 *
 * @param [in] clnt Client id returned by AmbaIPC_ClientCreate function.
 *
 * @return 0 - OK, others - NG
 * @see AmbaIPC_ClientCreate
 */
int AmbaIPC_ClientDestroy(CLIENT_ID_t clnt);

/**
 * @brief This function is used to create invoke RPC call.
 * After a connection is created, You can invoke RPC procedure of the RPC
 * RPC program. To remotely call the procedure, the procedure number should
 * be specified and the input and out parameter are also have to given.
 *
 * @param [in] client Client id retruned by AmbaIPC_ClientCreate function.
 * @param [in] proc RPC procedure number
 * @param [in] in The pointer to the input parameters
 * @param [in] in_len The size of the input parameters
 * @param [in] out The pointer to the output parameters
 * @param [in] out_len The size of the output parameters
 * @param [in] timeout The timeout value for waiting server response.
 *
 * @return 0 - OK, others - AMBA_IPC_REPLY_STATUS_e
 * @see AMBA_IPC_REPLY_STATUS_e
 */
AMBA_IPC_REPLY_STATUS_e AmbaIPC_ClientCall(CLIENT_ID_t client, INT32 proc,
        void *in,  INT32 in_len,
        void *out, INT32 out_len,
        UINT32 timeout);

/**
 * @brief This function is used to get the description of error code.
 *
 * @param [in] ErrNum The error code got from rpc related function.
 *
 * @return - The error description.
 * @see AMBA_IPC_REPLY_STATUS_e
 */
const char *AmbaIPC_RpcStrError(int ErrNum);

#include "AmbaShell.h"

void AmbaIPC_RpcInit(void);
void AmbaIPC_ListSvc(AMBA_SHELL_PRINT_f PrintFunc);
void AmbaIPC_InitBinder(void);
void AmbaIPC_InitClntCtrl(void);

int  xprt_init_port(AMBA_IPC_PORT_s *, void *contex, PORT_RECV_f recv);
void xprt_exit_port(AMBA_IPC_PORT_s *);
int  xprt_send_pkt (AMBA_IPC_PORT_s *, AMBA_IPC_PKT_s *pkt, int size);

/** @} */ // end of group AmbaIPC_RPC

#endif /* AMBAIPC_RPC_H */

