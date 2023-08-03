/**
 *  @file Transfer_impl_MemIO_threadx.c
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
 *  @details Implementation of Transfer interface for memio threadx
 *
 */

#ifdef __unix__
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#else
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaUtility.h"
#endif
#include "AmbaTransfer.h"
#include "Transfer_impl_MemIO.h"
#include "AmbaIPC_RpcProg_LU_Memio.h"

#ifdef __unix__
#define AmbaMisra_TypeCast32(a, b) memcpy(a, b, sizeof(void *))
#define AmbaWrap_memset memset
#define ErrMsg(fmt,...) printf("%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)
#define Debug(fmt,...) //printf("%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)
#endif

#define TRANS_MSGQ_OS               TRANS_MSGQ_LINUX


typedef struct {
    INT32   ServerFd;
    INT32   ClientFd;
    INT32   AcceptFd;
    INT32   ServerOwner;
    INT32   ClientOwner;
    struct sockaddr_in  SockAddr;
} Transfer_Socket_Ctrl_s;

static Transfer_Socket_Ctrl_s G_Socket_Ctrl[TRANS_MSGQ_MAX];

static UINT16 NetStack_Htons(UINT16 n)
{
    return ((n & 0xffU) << 8) | ((n & 0xff00U) >> 8);
}

static UINT32 NetStack_Htonl(UINT32 n)
{
  return ((n & 0xffU) << 24) |
    ((n & 0xff00U) << 8) |
    ((n & 0xff0000U) >> 8) |
    ((n & 0xff000000U) >> 24);
}

static UINT32 NetXStack_Digits2IP(UINT32 a, UINT32 b, UINT32 c, UINT32 d)
{
  return ((a & 0xffU) << 24) |
         ((b & 0xffU) << 16) |
         ((c & 0xffU) << 8) |
         ((d & 0xffU));
}

UINT32 Transfer_Native_Init(void)
{
    UINT32 Rval;
    static UINT32 MsgQ_Ctrl_Init = 0U;

    if(MsgQ_Ctrl_Init != 0U) {
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else {
        (void)AmbaWrap_memset(G_Socket_Ctrl, 0, sizeof(G_Socket_Ctrl));
        MsgQ_Ctrl_Init = 1U;
        Rval = TRANSFER_OK;
    }

    return Rval;
}

UINT32 Transfer_Native_Deinit(void)
{
    UINT32 Rval,i;
    Transfer_MsgQ_Ctx_s *pCtx;

    Rval = TRANSFER_OK;
    for(i = 0; i< TRANS_MSGQ_MAX; i++) {
        Transfer_GetCtxByIndex(i, &pCtx);
        if(pCtx->pMsgQCtrlNative != NULL) {
            if( pCtx->MsgQAppOs == TRANS_MSGQ_THREADX ) {
                Transfer_Native_Disconnect(pCtx, MEMIO_OWNER_APP);
            }

            if( pCtx->MsgQCvtaskOs == TRANS_MSGQ_THREADX ) {
                Transfer_Native_Disconnect(pCtx, MEMIO_OWNER_CVTASK);
            }
        }
    }

    return Rval;
}

UINT32 Transfer_Native_Connect(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner)
{
    UINT32 Rval;
    INT32 RvalI32;
    Transfer_Socket_Ctrl_s *pSockCtrl;

    Rval = TRANSFER_OK;
    Debug("Index (%d) Owner (%d)", pCtx->MsgQIndex, Owner);
    pSockCtrl = &G_Socket_Ctrl[pCtx->MsgQIndex];
    memset(&pSockCtrl->SockAddr, 0, sizeof(struct sockaddr_in));
    pSockCtrl->SockAddr.sin_family = AF_INET;
    pSockCtrl->SockAddr.sin_addr.s_addr = NetStack_Htonl(NetXStack_Digits2IP(127U, 0U, 0U, 1U));
    pSockCtrl->SockAddr.sin_port = NetStack_Htons(6660U + pCtx->MsgQIndex);

    if((pCtx->MsgQAppOs == TRANS_MSGQ_OS_NONE) && (pCtx->MsgQCvtaskOs == TRANS_MSGQ_OS_NONE)){
        pSockCtrl->ServerOwner = Owner;
    }

    if (pSockCtrl->ServerOwner != Owner) {
        pSockCtrl->ClientFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (pSockCtrl->ClientFd == -1) {
            ErrMsg("[Client] Failed to create socket");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        }

        if (Rval == TRANSFER_OK) {
            Debug("[Client] connect fd=%d", pSockCtrl->ClientFd);
            RvalI32 = connect(pSockCtrl->ClientFd, (struct sockaddr *)&pSockCtrl->SockAddr, sizeof(struct sockaddr_in));
            if (RvalI32 == -1) {
                ErrMsg("[Client] Failed to connect to socket server");
                Rval = TRANSFER_ERR_IMPL_ERROR;
            }
        }
    } else {
        pSockCtrl->ServerFd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (pSockCtrl->ServerFd == -1) {
            ErrMsg("[Server] Failed to create socket");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        }

        if (Rval == TRANSFER_OK) {
            Debug("[Server] bind fd=%d", pSockCtrl->ServerFd);
            RvalI32 = bind(pSockCtrl->ServerFd, (struct sockaddr *)&pSockCtrl->SockAddr, sizeof(struct sockaddr_in));
            if (RvalI32 == -1) {
                ErrMsg("[Server] Failed to connect to socket server");
                Rval = TRANSFER_ERR_IMPL_ERROR;
            }
        }

        if (Rval == TRANSFER_OK) {
            Debug("[Server] listen fd=%d", pSockCtrl->ServerFd);
            RvalI32 = listen(pSockCtrl->ServerFd, 128);
            if (RvalI32 == -1) {
                ErrMsg("[Server] Failed to listen to socket server");
                Rval = TRANSFER_ERR_IMPL_ERROR;
            }
        }
    }

    if( Rval == TRANSFER_OK ){
        pCtx->pMsgQCtrlNative = pSockCtrl;
        if(Owner == MEMIO_OWNER_APP) {
            pCtx->MsgQAppOs = TRANS_MSGQ_OS;
        } else {
            pCtx->MsgQCvtaskOs = TRANS_MSGQ_OS;
        }
    }
    return Rval;
}

UINT32 Transfer_Native_Disconnect(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner)
{
    UINT32 Rval;
    INT32 RvalI32;
    Transfer_Socket_Ctrl_s *pSockCtrl;

    Rval = TRANSFER_OK;
    Debug("Index (%d) Owner (%d)", pCtx->MsgQIndex, Owner);
    AmbaMisra_TypeCast32(&pSockCtrl, &pCtx->pMsgQCtrlNative);
    if(pSockCtrl->ServerOwner != Owner) {
        pCtx->MsgQAppOs = TRANS_MSGQ_OS_NONE;
        RvalI32 = close(pSockCtrl->ClientFd);
        if (RvalI32 == -1) {
            ErrMsg("[Client] Failed to close connection");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        }
    } else {
        pCtx->MsgQCvtaskOs = TRANS_MSGQ_OS_NONE;
        RvalI32 = close(pSockCtrl->ServerFd);
        if (RvalI32 == -1) {
            ErrMsg("[Server] Failed to close connection");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        }
    }

    if( (pCtx->MsgQAppOs == TRANS_MSGQ_OS_NONE) && (pCtx->MsgQCvtaskOs == TRANS_MSGQ_OS_NONE)) {
        pCtx->pMsgQCtrlNative = NULL;
    }
    return Rval;
}

UINT32 Transfer_Native_Send(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len)
{
    UINT32 Rval;
    INT32 RvalI32;
    Transfer_Socket_Ctrl_s *pSockCtrl;

    Rval = TRANSFER_OK;
    if(pCtx->pMsgQCtrlNative == NULL) {
        ErrMsg("Index (%d) Owner (%d) pMsgQCtrlNative == NULL ", pCtx->MsgQIndex, Owner);
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else if(pData == NULL){
        ErrMsg("Index (%d) Owner (%d) pData == NULL ", pCtx->MsgQIndex, Owner);
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else if(Len == 0U){
        ErrMsg("Index (%d) Owner (%d) Len (%d) == 0 ", pCtx->MsgQIndex, Owner, Len);
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else {
        Debug("Index (%d) Owner (%d)", pCtx->MsgQIndex, Owner);
        (void) pCtx;
        AmbaMisra_TypeCast32(&pSockCtrl, &pCtx->pMsgQCtrlNative);
        if(pSockCtrl->ServerOwner != Owner) {
            Debug("[Client] send fd=%d", pSockCtrl->ClientFd);
            RvalI32 = send(pSockCtrl->ClientFd, (char *)pData, Len, 0);
            if (RvalI32 != (INT32)Len) {
                ErrMsg("[Client] Failed to send buffer");
                Rval = TRANSFER_ERR_IMPL_ERROR;
            }
        } else {
            if (pSockCtrl->AcceptFd <= 0) {
                Debug("[Server] accept fd=%d", pSockCtrl->ServerFd);
                UINT32 addrlen = sizeof(struct sockaddr_in);
                struct sockaddr_in ClientSockAddr;
                pSockCtrl->AcceptFd = accept(pSockCtrl->ServerFd,
                    (struct sockaddr *)&ClientSockAddr, (socklen_t*)&addrlen);
                if (pSockCtrl->AcceptFd == -1) {
                    ErrMsg("[Server] Failed to accept to socket server");
                    Rval = TRANSFER_ERR_IMPL_ERROR;
                }
            }

            if (Rval == TRANSFER_OK) {
                Debug("[Server] send fd=%d", pSockCtrl->AcceptFd);
                RvalI32 = send(pSockCtrl->AcceptFd, (char *)pData, Len, 0);
                if (RvalI32 != (INT32)Len) {
                    ErrMsg("[Server] Failed to send buffer");
                    Rval = TRANSFER_ERR_IMPL_ERROR;
                }
            }
        }
    }

    return Rval;
}

UINT32 Transfer_Native_Recv(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len)
{
    UINT32 Rval;
    INT32 RvalI32;
    Transfer_Socket_Ctrl_s *pSockCtrl;

    Rval = TRANSFER_OK;
    if(pCtx->pMsgQCtrlNative == NULL) {
        ErrMsg("Index (%d) Owner (%d) pMsgQCtrlNative == NULL ", pCtx->MsgQIndex, Owner);
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else if(pData == NULL){
        ErrMsg("Index (%d) Owner (%d) pData == NULL ", pCtx->MsgQIndex, Owner);
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else if(Len == 0U){
        ErrMsg("Index (%d) Owner (%d) Len (%d) == 0 ", pCtx->MsgQIndex, Owner, Len);
        Rval = TRANSFER_ERR_IMPL_ERROR;
    } else {
        Debug("Index (%d) Owner (%d)", pCtx->MsgQIndex, Owner);
        (void) pCtx;
        AmbaMisra_TypeCast32(&pSockCtrl, &pCtx->pMsgQCtrlNative);
        if(pSockCtrl->ServerOwner != Owner) {
            Debug("[Client] recv fd=%d", pSockCtrl->ClientFd);
            RvalI32 = recv(pSockCtrl->ClientFd, (char *)pData, Len, 0);
            if (RvalI32 != (INT32)Len) {
                ErrMsg("[Client] Failed to recv buffer");
                Rval = TRANSFER_ERR_IMPL_ERROR;
            }
        } else {
            if (pSockCtrl->AcceptFd <= 0) {
                Debug("[Server] accept fd=%d", pSockCtrl->ServerFd);
                UINT32 addrlen = sizeof(struct sockaddr_in);
                struct sockaddr_in ClientSockAddr;
                pSockCtrl->AcceptFd = accept(pSockCtrl->ServerFd,
                    (struct sockaddr *)&ClientSockAddr, (socklen_t*)&addrlen);
                if (pSockCtrl->AcceptFd == -1) {
                    ErrMsg("[Server] Failed to accept to socket server");
                    Rval = TRANSFER_ERR_IMPL_ERROR;
                }
            }

            if (Rval == TRANSFER_OK) {
                Debug("[Server] recv fd=%d", pSockCtrl->AcceptFd);
                RvalI32 = recv(pSockCtrl->AcceptFd, (char *)pData, Len, 0);
                if (RvalI32 != (INT32)Len) {
                    ErrMsg("[Server] Failed to recv buffer");
                    Rval = TRANSFER_ERR_IMPL_ERROR;
                }
            }
        }
    }

    return Rval;
}

UINT32 Transfer_Remote_Init(void)
{
    UINT32 Rval;

    Rval = AmbaRpcProg_LU_Memio_Init();
    return Rval;
}

UINT32 Transfer_Remote_Deinit(void)
{
    UINT32 Rval;

    Rval = AmbaRpcProg_LU_Memio_Deinit();
    return Rval;
}

UINT32 Transfer_Remote_Connect(const Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner)
{
    UINT32 Rval;
    Transfer_MsgQ_Ctx_s *pMsgQCtx = (Transfer_MsgQ_Ctx_s *)pCtx;

    Rval = AmbaRpcProg_LU_Memio_Connect(pMsgQCtx, Owner);
    return Rval;
}

UINT32 Transfer_Remote_Disconnect(const Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner)
{
    UINT32 Rval;
    Transfer_MsgQ_Ctx_s *pMsgQCtx = (Transfer_MsgQ_Ctx_s *)pCtx;

    Rval = AmbaRpcProg_LU_Memio_Disconnect(pMsgQCtx, Owner);
    return Rval;
}

UINT32 Transfer_Remote_Send(const Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len)
{
    UINT32 Rval;
    Transfer_MsgQ_Ctx_s *pMsgQCtx = (Transfer_MsgQ_Ctx_s *)pCtx;

    Rval = AmbaRpcProg_LU_Memio_Send(pMsgQCtx, Owner, pData, Len);
    return Rval;
}
