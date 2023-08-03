#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "aipc_user.h"
#include "AmbaTransfer.h"
#include "Transfer_impl_MemIO.h"
#include "AmbaIPC_RpcProg_RT_Memio.h"
#include "AmbaIPC_RpcProg_LU_Memio.h"

#define USER_PROC_INFO 1024U
static CLIENT_ID_t RpcMemIOClient = NULL;
static unsigned int ProcInfo[USER_PROC_INFO];

static void AmbaRpcProg_LU_Memio_Svc_Connect(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AMBA_RPC_RT_MEMIO_CONNECT_s *pConnect = pMsg;
    Transfer_MsgQ_Ctx_s *pCtx;
    UINT32 *pStatus;

    pStatus = (UINT32*)(pRet->pResult);
    Transfer_GetCtxByIndex(pConnect->Index, &pCtx);
    Transfer_Native_Connect(pCtx, pConnect->Owner);
    if(pCtx->pMsgQCtrlNative != NULL) {
        if(pConnect->Owner == MEMIO_OWNER_APP) {
            pCtx->MsgQAppOs = TRANS_MSGQ_THREADX;
        } else {
            pCtx->MsgQCvtaskOs = TRANS_MSGQ_THREADX;
        }
        *pStatus = 0;
    } else {
        *pStatus = 1;
    }
    //send ipc reply
    pRet->Length = sizeof(UINT32);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void AmbaRpcProg_LU_Memio_Svc_Disconnect(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AMBA_RPC_RT_MEMIO_DISCONNECT_s *pDisconnect = pMsg;
    Transfer_MsgQ_Ctx_s *pCtx;
    UINT32 *pStatus;

    pStatus = (UINT32*)(pRet->pResult);
    Transfer_GetCtxByIndex(pDisconnect->Index, &pCtx);
    if(pCtx->pMsgQCtrlNative != NULL) {
        if(pDisconnect->Owner == MEMIO_OWNER_APP) {
            pCtx->MsgQAppOs = TRANS_MSGQ_OS_NONE;
        } else {
            pCtx->MsgQCvtaskOs = TRANS_MSGQ_OS_NONE;
        }
        *pStatus = 0;
    } else {
        *pStatus = 1;
    }
    Transfer_Native_Disconnect(pCtx, pDisconnect->Owner);
    //send ipc reply
    pRet->Length = sizeof(UINT32);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

static void AmbaRpcProg_LU_Memio_Svc_Send(void *pMsg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AMBA_RPC_RT_MEMIO_SEND_s *pSend = pMsg;
    Transfer_MsgQ_Ctx_s *pCtx;
    UINT32 *pStatus;

    pStatus = (UINT32*)(pRet->pResult);
    Transfer_GetCtxByIndex(pSend->Index, &pCtx);
    Transfer_Native_Send(pCtx, pSend->Owner, (void *)pSend->Data, pSend->DataLen);
    *pStatus = 0;
    //send ipc reply
    pRet->Length = sizeof(UINT32);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

void AmbaTransfer_Memio_Send(UINT32 Index, UINT32 Owner, void *pData, UINT32 Len)
{
    Transfer_MsgQ_Ctx_s *pCtx;

    Transfer_GetCtxByIndex(Index, &pCtx);
    Transfer_Native_Send(pCtx, Owner, pData, Len);
}

unsigned int AmbaRpcProg_LU_Memio_Init(void)
{
    unsigned int ret;
    int status;
	AMBA_IPC_PROG_INFO_s prog_info[1];

    prog_info->ProcNum = 3;
    if( (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)) > USER_PROC_INFO) {
        printf("AmbaRpcProg_LU_Memio_Init (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s))(0x%x) is big than 0x%x\n", (unsigned int)(prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)), USER_PROC_INFO);
        ret = TRANSFER_ERR_INVALID_INFO;
    } else {
        prog_info->pProcInfo = (AMBA_IPC_PROC_s *)ProcInfo;
        prog_info->pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_LU_Memio_Svc_Connect;
        prog_info->pProcInfo[1].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info->pProcInfo[1].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_LU_Memio_Svc_Disconnect;
        prog_info->pProcInfo[2].Mode = AMBA_IPC_SYNCHRONOUS;
        prog_info->pProcInfo[2].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_LU_Memio_Svc_Send;
        status = ambaipc_svc_register(AMBA_RPC_PROG_LU_MEMIO_PROG_ID, AMBA_RPC_PROG_LU_MEMIO_VER, "memio_rpc_lu_svc", prog_info, 1);
        if(status != 0) {
            printf("AmbaRpcProg_LU_Memio_Init AmbaIPC_SvcRegister fail (%d)\n", status);
            ret = TRANSFER_ERR_INVALID_INFO;
        } else {
            ret = TRANSFER_OK;
        }
    }

	return ret;
}

unsigned int AmbaRpcProg_LU_Memio_Deinit(void)
{
    unsigned int ret = TRANSFER_OK;

    if(RpcMemIOClient != NULL) {
        ambaipc_clnt_destroy(RpcMemIOClient);
    }
    ambaipc_svc_unregister(AMBA_RPC_PROG_LU_MEMIO_PROG_ID, AMBA_RPC_PROG_LU_MEMIO_VER);
    return ret;
}

unsigned int AmbaRpcProg_LU_Memio_Connect(Transfer_MsgQ_Ctx_s *pCtx, unsigned int Owner)
{
    unsigned int ret = TRANSFER_OK,Result;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_RT_MEMIO_CONNECT_s In;

    if(RpcMemIOClient == NULL) {
        RpcMemIOClient = ambaipc_clnt_create(AMBA_RPC_PROG_RT_MEMIO_HOST,
                                    AMBA_RPC_PROG_RT_MEMIO_PROG_ID,
                                    AMBA_RPC_PROG_RT_MEMIO_VER);
        if(RpcMemIOClient == NULL) {
            ret = TRANSFER_ERR_INVALID_INFO;
        }
    }

    if(ret == TRANSFER_OK) {
        In.Index = pCtx->MsgQIndex;
        In.Owner = Owner;
        status = ambaipc_clnt_call(RpcMemIOClient, AMBA_RPC_PROG_RT_MEMIO_CONNECT,
                                    (void *) &In, sizeof(AMBA_RPC_RT_MEMIO_CONNECT_s), &Result, sizeof(unsigned int), 5000);
        if(status != AMBA_IPC_REPLY_SUCCESS) {
            printf("AmbaRpcProg_LU_Memio_Connect AmbaIPC_ClientCall fail (%d)\n", status);
            ret = TRANSFER_ERR_INVALID_INFO;
        } else {
            while(Result == 0U) {
                if( (Owner == MEMIO_OWNER_CVTASK) && (pCtx->MsgQAppOs != TRANS_MSGQ_OS_NONE) ) {
                    break;
                } else if( (Owner == MEMIO_OWNER_APP) && (pCtx->MsgQCvtaskOs != TRANS_MSGQ_OS_NONE) ) {
                    break;
                }
            }
        }
    }

    return ret;
}

unsigned int AmbaRpcProg_LU_Memio_Disconnect(Transfer_MsgQ_Ctx_s *pCtx, unsigned int Owner)
{
    unsigned int ret = TRANSFER_OK;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_RT_MEMIO_DISCONNECT_s In;

    In.Index = pCtx->MsgQIndex;
    In.Owner = Owner;

    status = ambaipc_clnt_call(RpcMemIOClient, AMBA_RPC_PROG_RT_MEMIO_DISCONNECT, (void *) &In,
                                sizeof(AMBA_RPC_RT_MEMIO_DISCONNECT_s), NULL, 0, 5000);
    if(status != AMBA_IPC_REPLY_SUCCESS) {
        printf("AmbaRpcProg_LU_Memio_Disconnect AmbaIPC_ClientCall fail (%d)\n", status);
        ret = TRANSFER_ERR_INVALID_INFO;
    }

    return ret;
}

unsigned int AmbaRpcProg_LU_Memio_Send(Transfer_MsgQ_Ctx_s *pCtx, unsigned int Owner, void *pData, unsigned int Len)
{
    unsigned int ret = TRANSFER_OK;
    AMBA_IPC_REPLY_STATUS_e status;
    AMBA_RPC_RT_MEMIO_SEND_s In;

    In.Index = pCtx->MsgQIndex;
    In.Owner = Owner;
    In.DataLen = Len;
    if(Len > AMBA_RPC_RT_MEMIO_SEND_DATA) {
        printf("AmbaRpcProg_LU_Memio_Send Len (%d) > AMBA_RPC_RT_MEMIO_SEND_DATA (%d)\n", Len, AMBA_RPC_RT_MEMIO_SEND_DATA);
        ret = TRANSFER_ERR_INVALID_INFO;
    } else {
        memcpy(&In.Data[0], pData, Len);
        status = ambaipc_clnt_call(RpcMemIOClient, AMBA_RPC_PROG_RT_MEMIO_SEND, &In,
                                    (Len+AMBA_RPC_RT_MEMIO_SEND_HEAD), NULL, 0, 5000);
        if(status != AMBA_IPC_REPLY_SUCCESS) {
            printf("AmbaRpcProg_LU_Memio_Send AmbaIPC_ClientCall fail (%d)\n", status);
            ret = TRANSFER_ERR_INVALID_INFO;
        }
    }

    return ret;
}

