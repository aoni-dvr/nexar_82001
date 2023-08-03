/**
 *  @file AmbaIPC_Rpc_Def.h
 *
 *  @copyright Copyright (c) 2015 Ambarella, Inc.
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
 *  @details Common definitions for AmbaIPC RPC program.
 *
 */
#ifndef AMBAIPC_RPC_DEF_H
#define AMBAIPC_RPC_DEF_H


/**
 * RPC server should be either in Linux or ThreadX.
 * @see AmbaIPC_ClientCreate
 */
typedef enum {
	AMBA_IPC_HOST_LINUX = 0,
	AMBA_IPC_HOST_THREADX,
	AMBA_IPC_HOST_MAX
} AMBA_IPC_HOST_e;

/**
 * Definition of the status for RPC.
 *
 */
typedef enum {
	AMBA_IPC_REPLY_SUCCESS = 0,
	AMBA_IPC_REPLY_PROG_UNAVAIL,
	AMBA_IPC_REPLY_PARA_INVALID,
	AMBA_IPC_REPLY_SYSTEM_ERROR,
	AMBA_IPC_REPLY_TIMEOUT,
	AMBA_IPC_INVALID_CLIENT_ID,
	AMBA_IPC_UNREGISTERED_SERVER,
	AMBA_IPC_REREGISTERED_SERVER,
	AMBA_IPC_SERVER_MEM_ALLOC_FAILED,
	AMBA_IPC_IS_NOT_READY,
	AMBA_IPC_CRC_ERROR,
	AMBA_IPC_NETLINK_ERROR,
	AMBA_IPC_STATUS_NUM
} AMBA_IPC_REPLY_STATUS_e;

/**
 * The message type.
 *
 */
typedef enum {
	AMBA_IPC_MSG_CALL = 0,
	AMBA_IPC_MSG_REPLY
} AMBA_IPC_MSG_e;

/**
 * The communication mode for RPC procedure.
 * @see AMBA_IPC_SVC_RESULT_s
 *
 */
typedef enum {
	AMBA_IPC_SYNCHRONOUS = 0,
	AMBA_IPC_ASYNCHRONOUS
} AMBA_IPC_COMMUICATION_MODE_e ;

/**
 * The data structure for storing the server result.
 * @see AMBA_IPC_COMMUICATION_MODE_e
 * @see AMBA_IPC_PROC_f
 */
typedef struct {
	INT32 Length;							/**< The size of the result */
	void *pResult;						/**< Pointer to the calculated result */
	AMBA_IPC_COMMUICATION_MODE_e Mode;	/**< The communication mode of the procedure */
	AMBA_IPC_REPLY_STATUS_e Status;		/**< The status of the procedure */
} AMBA_IPC_SVC_RESULT_s;

/**
 * The function pointer prototype for RPC procedure.
 * All the RPC procedures need to follow this type.
 * @see AMBA_IPC_SVC_RESULT_s
 */
typedef void (*AMBA_IPC_PROC_f)(void *msg, AMBA_IPC_SVC_RESULT_s *result);

/**
 * The procedure infomation for RPC program.
 *
 * @see AmbaIPC_SvcRegister
 * @see AMBA_IPC_PROC_f
 * @see AMBA_IPC_COMMUICATION_MODE_e
 */
typedef struct {
    AMBA_IPC_PROC_f Proc;
    AMBA_IPC_COMMUICATION_MODE_e Mode;
} AMBA_IPC_PROC_s;


/**
 * The information for RPC program information such as the procedure information.
 *
 * @see AMBA_IPC_PROC_s
 */
typedef struct {
	INT32 ProcNum;					/**< The procedure number */
	AMBA_IPC_PROC_s *pProcInfo;		/**< The information of the procedures in the RPC program */
} AMBA_IPC_PROG_INFO_s;

#endif
