/**
 *  @file AmbaTransfer_impl_DummyCamctrl.c
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
 *  @details Implementation of AmbaTransfer interface by Queue
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

#define CAMCTRL_SERVER_PORT 11009

#define ErrMsg(fmt,...) printf("%s: "fmt,__FUNCTION__,##__VA_ARGS__)
#define Debug ErrMsg

#define CV_MAX_PARAM_SIZE (1016)
typedef struct _CV_Request_s_ {
    unsigned int Msg;
    unsigned int ParamSize;
    unsigned char Param[CV_MAX_PARAM_SIZE];
} CV_Request_s;

static int sockfd;
static int Connected = 0;

static UINT32 Camctrl_Init(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Camctrl_Release(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Camctrl_Register(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Camctrl_Unregister(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Camctrl_Recv(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Camctrl_Connect(void *Input)
{
    UINT32 Rval;
    struct sockaddr_in dest = {0};

    Rval = TRANSFER_OK;
    if (Connected == 0) {
        sockfd = socket(PF_INET, SOCK_STREAM, 0);

        /* initialize value in dest */
        bzero(&dest, sizeof(dest));
        dest.sin_family = PF_INET;
        dest.sin_port = htons(CAMCTRL_SERVER_PORT);
        dest.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* Connecting to server */
        Rval = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));

        if(Rval != 0) {
            perror("connect() fail: ");
            Rval = TRANSFER_ERR_IMPL_ERROR;
        } else {
            Rval = TRANSFER_OK;
            Connected = 1;
        }
    }

    return Rval;
}

static UINT32 Camctrl_Disconnect(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

static UINT32 Camctrl_Send(void *Input)
{
    UINT32 Rval = AMBATRANSFER_OK;
    char buf[10];
    AMBA_TRANSFER_SEND_s *SendReq = (AMBA_TRANSFER_SEND_s *)Input;
    INT32 ret;

    ret = write(sockfd, (char*)SendReq->PayloadAddr, SendReq->Size);
    if(ret == -1) {
        perror("send() write fail: ");
    }
    recv(sockfd, buf, sizeof(buf), 0);

    return Rval;
}

static UINT32 Camctrl_GetStatus(void *Input)
{
    UINT32 Rval;

    (void) Input;
    Debug("Called");
    Rval = TRANSFER_OK;

    return Rval;
}

AMBA_TRANSFER_IMPL_s Transfer_Impl_Camctrl = {
    .Init          = Camctrl_Init,
    .Release       = Camctrl_Release,
    .Register      = Camctrl_Register,
    .Unregister    = Camctrl_Unregister,
    .Recv          = Camctrl_Recv,
    .Connect       = Camctrl_Connect,
    .Disconnect    = Camctrl_Disconnect,
    .Send          = Camctrl_Send,
    .GetStatus     = Camctrl_GetStatus
};

