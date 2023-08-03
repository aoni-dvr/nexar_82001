/**
 *  @file BufQueue.c
 *
 * Copyright (c) [2020] Ambarella International LP
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
 * This file includes sample code and is only for internal testing and evaluation.  If you
 * distribute this sample code (whether in source, object, or binary code form), it will be
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details BufQueue
 *
 */
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaENET.h"
#include "NetStack.h"
#include "AvbStack.h"
#include "AvbAvtp.h"

static UINT32 AvbAvtp_QueueInit(AMBA_AVB_QUEUE_s* Thiz, char *pName, UINT32 pNameSize, UINT8 *pBase, UINT32 Size)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(pBase == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(Size == 0U) {
        Ret = NET_ERR_EINVAL;
    } else {
        Thiz->In = 0U;
        Thiz->Out = 0U;
        Thiz->Size = Size;
        Thiz->pBase = pBase;
        if (AmbaWrap_memcpy(&Thiz->Name[0] , pName, pNameSize)!= 0U) { }
        if (OK != AmbaKAL_MutexCreate(&Thiz->QueueMutex, pName)) {
            AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s : AmbaKAL_MutexCreate pName = %s fail", __func__, pName, NULL, NULL, NULL);
            AmbaPrint_Flush();
            Ret = NET_ERR_EPERM;
        } else {
            if (OK != AmbaKAL_EventFlagCreate(&Thiz->QueueEvent, pName)) {
                AmbaPrint_ModulePrintStr5(AVB_MODULE_ID, "%s : AmbaKAL_EventFlagCreate pName = %s fail", __func__, pName, NULL, NULL, NULL);
                AmbaPrint_Flush();
                Ret =  NET_ERR_EPERM;
            }
        }
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueDeInit(AMBA_AVB_QUEUE_s* Thiz)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        Ret =  AmbaKAL_MutexDelete(&Thiz->QueueMutex);
        if(Ret == 0U) {
            Ret =  AmbaKAL_EventFlagDelete(&Thiz->QueueEvent);
        }
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueMutexLock(const AMBA_AVB_QUEUE_s* Thiz)
{
    UINT32 Ret = NET_ERR_NONE;

    (void)(Thiz);
    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        //(void) AmbaKAL_MutexTake(&Thiz->QueueMutex, AMBA_KAL_WAIT_FOREVER);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueMutexUnLock(const AMBA_AVB_QUEUE_s* Thiz)
{
    UINT32 Ret = NET_ERR_NONE;

    (void)(Thiz);
    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        //(void) AmbaKAL_MutexGive(&Thiz->QueueMutex);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueUsed(const AMBA_AVB_QUEUE_s* Thiz, UINT32 *pNum)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(pNum == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueMutexLock(Thiz);
        *pNum = Thiz->In - Thiz->Out;
        (void) AvbAvtp_QueueMutexUnLock(Thiz);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueUnused(const AMBA_AVB_QUEUE_s* Thiz, UINT32 *pNum)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(pNum == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueMutexLock(Thiz);
        *pNum = Thiz->Size - (Thiz->In - Thiz->Out);
        (void) AvbAvtp_QueueMutexUnLock(Thiz);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueGetCurrentIn(const AMBA_AVB_QUEUE_s* Thiz, UINT32 *pIndex)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(pIndex == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueMutexLock(Thiz);
        *pIndex = Thiz->In%Thiz->Size;
        (void) AvbAvtp_QueueMutexUnLock(Thiz);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueSetIn(AMBA_AVB_QUEUE_s* Thiz, UINT32 Len)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(Len == 0U) {
        Ret = NET_ERR_EINVAL;
    } else {
        Thiz->In += Len;
        (void) AvbAvtp_QueueMutexUnLock(Thiz);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueGetCurrentOut(const AMBA_AVB_QUEUE_s* Thiz, UINT32 *pIndex)
{
    UINT32 Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(pIndex == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueMutexLock(Thiz);
        *pIndex = Thiz->Out%Thiz->Size;
        (void) AvbAvtp_QueueMutexUnLock(Thiz);
    }

    return Ret;
}

static UINT32 AvbAvtp_QueueSetOut(AMBA_AVB_QUEUE_s* Thiz, UINT32 Len)
{
    UINT32 Num = 0U, Ret = NET_ERR_NONE;

    if(Thiz == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(Len == 0U) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueUsed(Thiz , &Num);
        if(Num < Len) {
            Ret = NET_ERR_ENOMEM;
        } else {
            (void) AvbAvtp_QueueMutexLock(Thiz);
            Thiz->Out += Len;
            (void) AvbAvtp_QueueMutexUnLock(Thiz);
        }
    }

    return Ret;
}

UINT32 AvbAvtp_BufQueueToIndex(const AMBA_AVB_QUEUE_s *pQueue, const UINT8 *pBuf, UINT32 *pIndex)
{
    UINT32 Ret = NET_ERR_NONE;
    UINT32 Tmp1 = 0U,Tmp2 = 0U;

    if( pQueue == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else if(pBuf == NULL) {
        Ret = NET_ERR_EINVAL;
    } else if(pIndex == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        AmbaMisra_TypeCast32(&Tmp1, &pQueue->pBase);
        AmbaMisra_TypeCast32(&Tmp2, &pBuf);
        if(Tmp2 == Tmp1) {
            *pIndex = 0U;
        } else if (Tmp2 > Tmp1){
            *pIndex = (Tmp2 - Tmp1)/sizeof(UINT8);
        } else {
            Ret = NET_ERR_EINVAL;
        }
    }

    return Ret;
}

UINT32 AvbAvtp_BufQueueCurrentIn(const AMBA_AVB_QUEUE_s *pQueue, UINT8 **ppBuf)
{
    UINT32 Ret = NET_ERR_NONE,Index;

    if( pQueue == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else if (ppBuf == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueGetCurrentIn(pQueue, &Index);
        *ppBuf = &(pQueue->pBase[Index]);
    }

    return Ret;
}

UINT32 AvbAvtp_BufQueueCurrentOut(const AMBA_AVB_QUEUE_s *pQueue, UINT8 **ppBuf)
{
    UINT32 Ret = NET_ERR_NONE,Index;

    if( pQueue == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else if (ppBuf == NULL) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueGetCurrentOut(pQueue, &Index);
        *ppBuf = &(pQueue->pBase[Index]);
    }

    return Ret;
}

UINT32 AvbAvtp_BufQueueSeIn(AMBA_AVB_QUEUE_s *pQueue, UINT32 Len)
{
    UINT32 Ret, Num = 0U;

    Ret = AvbAvtp_QueueUnused(pQueue , &Num);
    if(Ret == 0U) {
        if(Num < Len) {
            Ret = NET_ERR_ENOMEM;
        } else {
            (void) AvbAvtp_QueueMutexLock(pQueue);
            (void) AvbAvtp_QueueSetIn(pQueue, Len);
        }
    }

    return Ret;
}

UINT32 AvbAvtp_BufQueueSeOut(AMBA_AVB_QUEUE_s *pQueue, UINT32 Len)
{
    return AvbAvtp_QueueSetOut(pQueue, Len);
}

UINT32 AvbAvtp_BufQueueUnused(const AMBA_AVB_QUEUE_s *pQueue, UINT32 *pNum)
{
    return AvbAvtp_QueueUnused(pQueue, pNum);
}

UINT32 AvbAvtp_BufQueueInit(AMBA_AVB_QUEUE_s *pQueue, UINT8 *pBase, UINT32 Size)
{
    UINT32 Ret = NET_ERR_NONE;
    char name[64] = {"AvtpBufQueue"};
    if( pQueue == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else if( pBase == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueInit(pQueue, name, sizeof(name), pBase, Size);
    }

    return Ret;
}

UINT32 AvbAvtp_BufQueueDeInit(AMBA_AVB_QUEUE_s *pQueue)
{
    UINT32 Ret = NET_ERR_NONE;

    if( pQueue == NULL ) {
        Ret = NET_ERR_EINVAL;
    } else {
        (void) AvbAvtp_QueueDeInit(pQueue);
    }

    return Ret;
}

