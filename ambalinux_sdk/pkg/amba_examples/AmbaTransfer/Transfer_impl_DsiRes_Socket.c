/**
 *  @file Transfer_impl_DsiRes_Socket.c
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
 *  @details Disparity result channel Implementation of Transfer interface by Socket
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include "AmbaTransfer.h"
#include "Transfer_Channel_DsiRes.h"
#include "Transfer_impl_DsiRes_Socket.h"


#define DSIRES_SOCKET_DEBUG (0U)

#define ErrMsg(fmt,...) printf("%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)
#define Debug ErrMsg


static INT32                SocketClient;
static struct sockaddr_in   SocketServerAddr;               /* Echo server address */

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

static UINT32 Socket_Init(void *Input)
{
    UINT32 Rval;
    const TRANSFER_DSIRES_SOCKET_INFO_s *SocketInfo;

    Debug("Called");
    memset(&SocketServerAddr, 0, sizeof(SocketServerAddr));
    if (Input != NULL) {
        SocketInfo = (TRANSFER_DSIRES_SOCKET_INFO_s *)Input;
        SocketServerAddr.sin_family = AF_INET;
        SocketServerAddr.sin_port = NetStack_Htons(SocketInfo->Port);
        (void)inet_pton(AF_INET, SocketInfo->IPv4, &(SocketServerAddr.sin_addr));
        Rval = TRANSFER_OK;
    } else {
        // use default ip address
        Debug("Use default ip address 10.0.0.1:5001");
        SocketServerAddr.sin_family = AF_INET;
        SocketServerAddr.sin_addr.s_addr = NetStack_Htonl(NetXStack_Digits2IP(10U, 0U, 0U, 1U));
        SocketServerAddr.sin_port = NetStack_Htons(5001U);
        Rval = TRANSFER_OK;
    }

    return Rval;
}

static UINT32 Socket_Release(void *Input)
{
    UINT32 Rval;

    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Socket_Register(void *Input)
{
    UINT32 Rval;

    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Socket_Unregister(void *Input)
{
    UINT32 Rval;

    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Socket_Recv(void *Input)
{
    INT32 SocketRval;
    UINT32 Rval;
    const AMBA_TRANSFER_RECEIVE_s *RecvReq;

    if (Input != NULL) {
        char Buffer[1024U] = {'\0'};
        RecvReq = (AMBA_TRANSFER_RECEIVE_s *)Input;
        SocketRval = recv(SocketClient, Buffer, (INT32)sizeof(Buffer), 0);
        Rval = recv(SocketClient, Buffer, sizeof(Buffer), 0);
        if (RecvReq->Result != NULL) {
            *((UINT32 *)RecvReq->Result) = (UINT32)SocketRval;
        }

        if (SocketRval == -1) {
            ErrMsg("Failed to recv buffer");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        } else {
            Rval = TRANSFER_OK;
        }
    } else {
        ErrMsg("Input is NULL!!!");
        Rval = TRANSFER_ERR_INVALID_INFO;
    }

    return Rval;
}

static UINT32 Socket_Connect(void *Input)
{
    INT32 SocketRval;
    UINT32 Rval;

    Rval = TRANSFER_OK;
    Debug("Called");
    SocketClient = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketClient == -1) {
        ErrMsg("Failed to create socket");
        Rval = TRANSFER_ERR_IMPL_ERROR;
    }

    if (Rval == TRANSFER_OK) {
        SocketRval = connect(SocketClient, (struct sockaddr *)&SocketServerAddr, sizeof(SocketServerAddr));
        if (SocketRval == -1) {
            ErrMsg("Failed to connect to socket server");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        }
    }

    return Rval;
}

static UINT32 Socket_Disconnect(void *Input)
{
    INT32 SocketRval;
    UINT32 Rval;

    Rval = TRANSFER_OK;
    Debug("Called");
    SocketRval = close(SocketClient);
    if (SocketRval == -1) {
        ErrMsg("Failed to close connection");
        Rval = TRANSFER_ERR_IMPL_ERROR;
    }

    return Rval;
}

static UINT32 Socket_Send(void *Input)
{
    INT32 SocketRval = 0;
    UINT32 Rval;
    const AMBA_TRANSFER_SEND_s *SendReq;

    Rval = TRANSFER_OK;
    if (Input != NULL) {
        SendReq = (AMBA_TRANSFER_SEND_s *)Input;
        if (SendReq->PayloadAddr != NULL) {
            TRANSFER_DSIRES_PKG_s *DsiResPkg = (TRANSFER_DSIRES_PKG_s *)SendReq->PayloadAddr;

            switch(DsiResPkg->Header.Type) {
            case TRANSFER_DSIRES_TYPE_HEADER:
            {
                ErrMsg("Unsupported Pkg type 0x%08x!", DsiResPkg->Header.Type);
                break;
            }
            case TRANSFER_DSIRES_TYPE_INFO:
            {
                TRANSFER_DSIMAP_LIST_s *ListPayload = (TRANSFER_DSIMAP_LIST_s *)DsiResPkg->Payload;
                Debug(">>>>>>>>>>>> Type=INFO[%x], Version=%u, PayloadSize=%u",
                    DsiResPkg->Header.Type, DsiResPkg->Header.Version, DsiResPkg->Header.PayloadSize);
                Debug(">> Version=%u, DataFormat=%u, DataSize=%u, CapTs=%u, CapSeq=%u CapScale=%u",
                    ListPayload->Header.Version, ListPayload->Header.DataFormat, ListPayload->Header.DataSize,
                    ListPayload->Header.CapTs, ListPayload->Header.CapSeq, ListPayload->Header.CapScale);
                Debug(">> Pitch=%u, Width=%u, Height=%u",
                    ListPayload->Header.Pitch, ListPayload->Header.Width, ListPayload->Header.Height);

                SocketRval = send(SocketClient, (char *)DsiResPkg, (INT32)sizeof(TRANSFER_DSIRES_PKG_HEADER_s) + (INT32)sizeof(TRANSFER_DSIMAP_LISTHEADER_s), 0);
                SocketRval = send(SocketClient, (char *)ListPayload->Map, (INT32)ListPayload->Header.DataSize, 0);
                break;
            }
            default:
                ErrMsg("Unsupported Pkg type 0x%08x!", DsiResPkg->Header.Type);
                Rval = TRANSFER_ERR_IMPL_ERROR;
                break;
            }

            if (SendReq->Result != NULL) {
                *((UINT32 *)SendReq->Result) = (UINT32)SocketRval;
            }
        } else {
            ErrMsg("PayloadAddr is NULL");
            Rval = TRANSFER_ERR_INVALID_INFO;
        }
    } else {
        ErrMsg("Input is NULL");
        Rval = TRANSFER_ERR_INVALID_INFO;
    }

    return Rval;
}

static UINT32 Socket_GetStatus(void *Input)
{
    UINT32 Rval;

    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

AMBA_TRANSFER_IMPL_s Transfer_Impl_DsiRes_Socket = {
    .Init          = Socket_Init,
    .Release       = Socket_Release,
    .Register      = Socket_Register,
    .Unregister    = Socket_Unregister,
    .Recv          = Socket_Recv,
    .Connect       = Socket_Connect,
    .Disconnect    = Socket_Disconnect,
    .Send          = Socket_Send,
    .GetStatus     = Socket_GetStatus
};

