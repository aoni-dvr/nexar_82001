/**
 *  @file Transfer_impl_MemIO.h
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
 *  @details Header of Transfer interface by Socket
 *
 */

#ifndef TRANSFER_IMPL_MEMIO_H
#define TRANSFER_IMPL_MEMIO_H

#ifdef __unix__
#include <stdint.h>

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef int INT32;
#endif


#define TRANS_MSGQ_OS_NONE          0U
#define TRANS_MSGQ_THREADX          1U
#define TRANS_MSGQ_LINUX            2U

//// from cvapi_memio_interface.h /////////////
#define MEMIO_OWNER_CVTASK                  0U
#define MEMIO_OWNER_APP                     1U

#define TRANS_MSGQ_MAX                      8U

typedef struct {
    uint32_t              SubChannel;
    uint32_t              Owner;
} memio_setting_t;
/////////////////////////////////////////////


typedef struct {
    UINT32  MsgQIndex;
    void    *pMsgQCtrlNative;
    void    *pMsgQCtrlRemote;
    UINT32  MsgQAppOs;
    UINT32  MsgQCvtaskOs;
} Transfer_MsgQ_Ctx_s;

void Transfer_GetCtxByIndex(UINT32 Index, Transfer_MsgQ_Ctx_s **ppCtx);

UINT32 Transfer_Native_Init(void);
UINT32 Transfer_Native_Deinit(void);
UINT32 Transfer_Native_Connect(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner);
UINT32 Transfer_Native_Disconnect(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner);
UINT32 Transfer_Native_Send(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len);
UINT32 Transfer_Native_Recv(Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len);

UINT32 Transfer_Remote_Init(void);
UINT32 Transfer_Remote_Deinit(void);
UINT32 Transfer_Remote_Connect(const Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner);
UINT32 Transfer_Remote_Disconnect(const Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner);
UINT32 Transfer_Remote_Send(const Transfer_MsgQ_Ctx_s *pCtx, UINT32 Owner, void *pData, UINT32 Len);
#endif /* TRANSFER_IMPL_MEMIO_H */
