/**
 *  @file Transfer_impl_MemIO.c
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
 *  @details Implementation of Transfer interface for memio
 *
 */

#ifdef __unix__
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#else
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#endif
#include "AmbaTransfer.h"
#include "Transfer_impl_MemIO.h"
//#include "cvapi_memio_interface.h"

#ifdef __unix__
#define AmbaMisra_TypeCast32(a, b) memcpy(a, b, sizeof(void *))
#define AmbaWrap_memset memset
#define ErrMsg(fmt,...) printf("%s: "fmt"\n",__FUNCTION__,##__VA_ARGS__)
#define Debug ErrMsg
#endif


/*-----------------------------------------------------------------------------------------------*\
 *  Instance
\*-----------------------------------------------------------------------------------------------*/
static UINT32 MsgQ_Init = 0U;
static Transfer_MsgQ_Ctx_s G_MsgQ_Ctx[TRANS_MSGQ_MAX];

void Transfer_GetCtxByIndex(UINT32 Index, Transfer_MsgQ_Ctx_s **ppCtx)
{
    *ppCtx = &G_MsgQ_Ctx[Index];
}

static UINT32 Inst_MsgQ_Init(void *Input)
{
    UINT32 Rval = TRANSFER_OK,i;

    (void) Input;

    if(MsgQ_Init != 0U) {
        ErrMsg("Inst_MsgQ_Init register twice");
        Rval = TRANSFER_ERR_INVALID_INFO;
    } else {
        (void)AmbaWrap_memset(G_MsgQ_Ctx, 0, sizeof(G_MsgQ_Ctx));
        for(i = 0U; i < TRANS_MSGQ_MAX; i++)
        {
            G_MsgQ_Ctx[i].MsgQIndex = i;
        }
        Rval = Transfer_Native_Init();
        if(Rval == TRANSFER_OK) {
            Rval = Transfer_Remote_Init();
            if(Rval == TRANSFER_OK) {
                MsgQ_Init = 1U;
            } else {
                ErrMsg("Inst_MsgQ_Init native init fail");
            }
        } else {
            ErrMsg("Inst_MsgQ_Init remote init fail");
        }
    }

    return Rval;
}

static UINT32 Inst_MsgQ_Deinit(void *Input)
{
    UINT32 Rval;

    (void) Input;

    Rval = Transfer_Native_Deinit();
    if(Rval == TRANSFER_OK) {
        Rval = Transfer_Remote_Deinit();
        if(Rval != TRANSFER_OK) {
            ErrMsg("Inst_MsgQ_Deinit native de-init fail");
        } else {
            MsgQ_Init = 0U;
        }
    } else {
        ErrMsg("Inst_MsgQ_Deinit remote de-init fail");
    }

    return Rval;
}

static UINT32 Inst_MsgQ_Connect(void *Input)
{
    UINT32 Rval;
    const AMBA_TRANSFER_CONNECT_s *pPtr;
    Transfer_MsgQ_Ctx_s *pCtx;
    const memio_setting_t *pSet;

    if(Input == NULL) {
        ErrMsg("Inst_MsgQ_Connect Input == NULL");
        Rval = TRANSFER_ERR_INVALID_INFO;
    } else {
        (void) Input;
        AmbaMisra_TypeCast32(&pPtr, &Input);
        if(pPtr->Settings == NULL) {
            ErrMsg("Inst_MsgQ_Connect pPtr->Settings == NULL");
            Rval = TRANSFER_ERR_INVALID_INFO;
        } else {
            AmbaMisra_TypeCast32(&pSet, &(pPtr->Settings));
            if( pSet->SubChannel >= TRANS_MSGQ_MAX ) {
                ErrMsg("Inst_MsgQ_Connect pSet->Index (%d) Owner (%d) over range", pSet->SubChannel, pSet->Owner);
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                Transfer_GetCtxByIndex(pSet->SubChannel, &pCtx);
                if( (pCtx->MsgQCvtaskOs != TRANS_MSGQ_OS_NONE) && (pSet->Owner == MEMIO_OWNER_CVTASK)) {
                    ErrMsg("Inst_MsgQ_Connect (%d) Owner (%d) connect twice", pSet->SubChannel, pSet->Owner);
                    Rval = TRANSFER_ERR_INVALID_INFO;
                } else if( (pCtx->MsgQAppOs != TRANS_MSGQ_OS_NONE) && (pSet->Owner == MEMIO_OWNER_APP)) {
                    ErrMsg("Inst_MsgQ_Connect (%d) Owner (%d) connect twice", pSet->SubChannel, pSet->Owner);
                    Rval = TRANSFER_ERR_INVALID_INFO;
                } else {
                    if( Transfer_Native_Connect(pCtx, pSet->Owner) == TRANSFER_OK ) {
                         if( Transfer_Remote_Connect(pCtx, pSet->Owner) == TRANSFER_OK ) {
                            Rval = TRANSFER_OK;
                         } else {
                            ErrMsg("Inst_MsgQ_Connect (%d) Owner (%d) remote connect fail", pSet->SubChannel, pSet->Owner);
                            Rval = TRANSFER_ERR_INVALID_INFO;
                         }
                    } else {
                        ErrMsg("Inst_MsgQ_Connect (%d) Owner (%d) native connect fail", pSet->SubChannel, pSet->Owner);
                        Rval = TRANSFER_ERR_INVALID_INFO;
                    }
                }
            }
        }
    }

    return Rval;
}

static UINT32 Inst_MsgQ_Disconnect(void *Input)
{
    UINT32 Rval = TRANSFER_OK;
    const AMBA_TRANSFER_DISCONNECT_s *pPtr;
    Transfer_MsgQ_Ctx_s *pCtx;
    const memio_setting_t *pSet;

    if(Input == NULL) {
        ErrMsg("Inst_MsgQ_Disconnect Input == NULL");
        Rval = TRANSFER_ERR_INVALID_INFO;
    } else {
        (void) Input;
        AmbaMisra_TypeCast32(&pPtr, &Input);
        if(pPtr->Settings == NULL) {
            ErrMsg("Inst_MsgQ_Disconnect pPtr->Settings == NULL");
            Rval = TRANSFER_ERR_INVALID_INFO;
        } else {
            AmbaMisra_TypeCast32(&pSet, &(pPtr->Settings));
            if( pSet->SubChannel >= TRANS_MSGQ_MAX ) {
                ErrMsg("Inst_MsgQ_Disconnect pSet->Index (%d) Owner (%d) over range", pSet->SubChannel, pSet->Owner);
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                Transfer_GetCtxByIndex(pSet->SubChannel, &pCtx);
                if( (pCtx->MsgQCvtaskOs == TRANS_MSGQ_OS_NONE) && (pSet->Owner == MEMIO_OWNER_CVTASK) ) {
                    //ErrMsg("Inst_MsgQ_Disconnect (%d) Owner (%d) disconnect twice", pSet->SubChannel, pSet->Owner);
                } else if( (pCtx->MsgQAppOs == TRANS_MSGQ_OS_NONE) && (pSet->Owner == MEMIO_OWNER_APP) ) {
                    //ErrMsg("Inst_MsgQ_Disconnect (%d) Owner (%d) disconnect twice", pSet->SubChannel, pSet->Owner);
                } else {
                    if( Transfer_Native_Disconnect(pCtx, pSet->Owner) == TRANSFER_OK ) {
                         if( Transfer_Remote_Disconnect(pCtx, pSet->Owner) == TRANSFER_OK ) {
                            Rval = TRANSFER_OK;
                         } else {
                            ErrMsg("Inst_MsgQ_Disconnect (%d) Owner (%d) remote disconnect fail", pSet->SubChannel, pSet->Owner);
                            Rval = TRANSFER_ERR_INVALID_INFO;
                         }
                    } else {
                        ErrMsg("Inst_MsgQ_Disconnect (%d) Owner (%d) native disconnect fail", pSet->SubChannel, pSet->Owner);
                        Rval = TRANSFER_ERR_INVALID_INFO;
                    }
                }
            }
        }
    }

    return Rval;
}

static UINT32 Inst_MsgQ_Recv(void *Input)
{
    UINT32 Rval;
    const AMBA_TRANSFER_RECEIVE_s *pRecv;
    Transfer_MsgQ_Ctx_s *pCtx;
    const memio_setting_t *pSet;
    void *ptr;

    if(Input == NULL) {
        ErrMsg("Inst_MsgQ_Recv Input == NULL");
        Rval = TRANSFER_ERR_INVALID_INFO;
    } else {
        (void) Input;
        AmbaMisra_TypeCast32(&pRecv, &Input);
        if(pRecv->Settings == NULL) {
            ErrMsg("Inst_MsgQ_Recv pPtr->Settings == NULL");
            Rval = TRANSFER_ERR_INVALID_INFO;
        } else {
            AmbaMisra_TypeCast32(&pSet, &(pRecv->Settings));
            if( pSet->SubChannel >= TRANS_MSGQ_MAX ) {
                ErrMsg("Inst_MsgQ_Recv pSet->Index (%d) Owner (%d) over range", pSet->SubChannel, pSet->Owner);
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                Transfer_GetCtxByIndex(pSet->SubChannel, &pCtx);
                AmbaMisra_TypeCast32(&ptr, &pRecv->BufferAddr);
                Rval = Transfer_Native_Recv(pCtx, pSet->Owner, ptr, pRecv->Size);
                if ( Rval != TRANSFER_OK ) {
                    ErrMsg("Inst_MsgQ_Recv() native Index (%d) Owner (%d) fail. Rval=%d", pSet->SubChannel, pSet->Owner, Rval);
                    Rval = TRANSFER_ERR_IMPL_ERROR;
                }
            }
        }
    }

    return Rval;
}

static UINT32 Inst_MsgQ_Send(void *Input)
{
    UINT32 Rval;
    const AMBA_TRANSFER_SEND_s *pSend;
    Transfer_MsgQ_Ctx_s *pCtx;
    const memio_setting_t *pSet;

    if(Input == NULL) {
        ErrMsg("Inst_MsgQ_Send Input == NULL");
        Rval = TRANSFER_ERR_INVALID_INFO;
    } else {
        (void) Input;
        AmbaMisra_TypeCast32(&pSend, &Input);
        if(pSend->Settings == NULL) {
            ErrMsg("Inst_MsgQ_Send pPtr->Settings == NULL");
            Rval = TRANSFER_ERR_INVALID_INFO;
        } else {
            AmbaMisra_TypeCast32(&pSet, &(pSend->Settings));
            if( pSet->SubChannel >= TRANS_MSGQ_MAX ) {
                ErrMsg("Inst_MsgQ_Send pSet->Index (%d) Owner (%d) over range", pSet->SubChannel, pSet->Owner);
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                Transfer_GetCtxByIndex(pSet->SubChannel, &pCtx);
                if( pCtx->MsgQAppOs == TRANS_MSGQ_OS_NONE ) {
                     ErrMsg("Inst_MsgQ_Send() Index (%d) Owner (%d) not connect", pSet->SubChannel, pSet->Owner);
                     Rval = TRANSFER_ERR_INVALID_INFO;
                } else if( pCtx->MsgQCvtaskOs == TRANS_MSGQ_OS_NONE ) {
                     ErrMsg("Inst_MsgQ_Send() Index (%d) Owner (%d) not connect", pSet->SubChannel, pSet->Owner);
                     Rval = TRANSFER_ERR_INVALID_INFO;
                } else if ( pCtx->MsgQCvtaskOs == pCtx->MsgQAppOs ) {
                    Rval = Transfer_Native_Send(pCtx, pSet->Owner, (void *)pSend->PayloadAddr, pSend->Size);
                    if ( Rval != TRANSFER_OK ) {
                        ErrMsg("Inst_MsgQ_Send() native Index (%d) Owner (%d) fail. Rval=%d", pSet->SubChannel, pSet->Owner, Rval);
                        Rval = TRANSFER_ERR_IMPL_ERROR;
                    }
                } else {
                    Rval = Transfer_Remote_Send(pCtx, pSet->Owner, (void *)pSend->PayloadAddr, pSend->Size);
                    if ( Rval != TRANSFER_OK ) {
                        ErrMsg("Inst_MsgQ_Send() remote Index (%d) Owner (%d) fail. Rval=%d", pSet->SubChannel, pSet->Owner, Rval);
                        Rval = TRANSFER_ERR_IMPL_ERROR;
                    }
                }
            }
        }
    }

    return Rval;
}

static UINT32 Inst_MsgQ_GetStatus(void *Input)
{
    UINT32 Rval;
    const AMBA_TRANSFER_GETSTATUS_s *pStatus;
    Transfer_MsgQ_Ctx_s *pCtx;
    const memio_setting_t *pSet;

    if(Input == NULL) {
        //ErrMsg("Inst_MsgQ_GetStatus Input == NULL");
        Rval = TRANSFER_ERR_INVALID_INFO;
    } else {
        (void) Input;
        AmbaMisra_TypeCast32(&pStatus, &Input);
        if(pStatus->Settings == NULL) {
            //ErrMsg("Inst_MsgQ_GetStatus pPtr->Settings == NULL");
            Rval = TRANSFER_ERR_INVALID_INFO;
        } else {
            AmbaMisra_TypeCast32(&pSet, &(pStatus->Settings));
            if( pSet->SubChannel >= TRANS_MSGQ_MAX ) {
                //ErrMsg("Inst_MsgQ_GetStatus pSet->Index (%d) Owner (%d) over range", pSet->SubChannel, pSet->Owner);
                Rval = TRANSFER_ERR_INVALID_INFO;
            } else {
                Transfer_GetCtxByIndex(pSet->SubChannel, &pCtx);
                if( pCtx->MsgQAppOs == TRANS_MSGQ_OS_NONE ) {
                     //ErrMsg("Inst_MsgQ_GetStatus() Index (%d) Owner (%d) not connect", pSet->SubChannel, pSet->Owner);
                     Rval = TRANSFER_ERR_INVALID_INFO;
                } else if( pCtx->MsgQCvtaskOs == TRANS_MSGQ_OS_NONE ) {
                     //ErrMsg("Inst_MsgQ_GetStatus() Index (%d) Owner (%d) not connect", pSet->SubChannel, pSet->Owner);
                     Rval = TRANSFER_ERR_INVALID_INFO;
                } else {
                     Rval = TRANSFER_OK;
                }
            }
        }
    }

    return Rval;
}

AMBA_TRANSFER_IMPL_s Transfer_Impl_Memio_Inst = {
    .Init          = Inst_MsgQ_Init,
    .Release       = Inst_MsgQ_Deinit,
    .Register      = NULL,
    .Unregister    = NULL,
    .Connect       = Inst_MsgQ_Connect,
    .Disconnect    = Inst_MsgQ_Disconnect,
    .Send          = Inst_MsgQ_Send,
    .Recv          = Inst_MsgQ_Recv,
    .GetStatus     = Inst_MsgQ_GetStatus,
};

