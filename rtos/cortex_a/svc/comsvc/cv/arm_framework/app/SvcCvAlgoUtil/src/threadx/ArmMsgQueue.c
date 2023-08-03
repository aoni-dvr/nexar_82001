/**
*  @file ArmMsgQueue.c
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
*   @details The Arm message queue utility
*
*/

/* Arm header */
#include "ArmMsgQueue.h"
#include "ArmErrCode.h"
#include "ArmLog.h"
#include "ArmStdC.h"

#define ARM_LOG_MSG_Q          "ArmUtil_MsgQ"

/*---------------------------------------------------------------------------*\
 * APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMsgQueue_Create
 *
 *  @Description:: Create message queue
 *
 *  @Input      ::
 *    pMsgQueueName: The name of message queue
 *    MsgSize:       The size of each message
 *    MaxMsgNum:     The maximum number of message
 *    pMsgQueueBuf:  The message queue buffer
 *
 *  @Output     ::
 *    pMsgQueue:   Pointer to the message queue
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMsgQueue_Create(ArmMsgQueue_t *pMsgQueue, char *pMsgQueueName, UINT32 MsgSize, UINT32 MaxMsgNum, void *pMsgQueueBuf)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsgQueueName == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueueName is null", 0U, 0U);
        Rval = ARM_NG;
    } else {
        // do nothing
    }

    /* 2. Create message queue */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_MsgQueueCreate(pMsgQueue, pMsgQueueName, MsgSize, pMsgQueueBuf, MsgSize*MaxMsgNum);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## AmbaKAL_MsgQueueCreate() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }

    if (Rval == ARM_OK) {
        ArmLog_STR(ARM_LOG_MSG_Q, "Success to create MsgQueue (%s)", pMsgQueueName, NULL);
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMsgQueue_Delete
 *
 *  @Description:: Delete message queue
 *
 *  @Input      ::
 *    pMsgQueue:   Pointer to the message queue
 *    pMsgQueueName: The name of message queue
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMsgQueue_Delete(ArmMsgQueue_t *pMsgQueue, const char *pMsgQueueName)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsgQueueName == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueueName is null", 0U, 0U);
        Rval = ARM_NG;
    } else {
        // do nothing
    }

    /* 2. Delete message queue */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_MsgQueueDelete(pMsgQueue);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## AmbaKAL_MsgQueueDelete() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMsgQueue_Send
 *
 *  @Description:: Send message to queue
 *
 *  @Input      ::
 *    pMsgQueue:   Pointer to the message queue
 *    pMsg:        Pointer to the sending message
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMsgQueue_Send(ArmMsgQueue_t *pMsgQueue, void *pMsg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsg == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsg is null", 0U, 0U);
        Rval = ARM_NG;
    } else {
        // do nothing
    }

    /* 2. Send message queue */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_MsgQueueSend(pMsgQueue, pMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## AmbaKAL_MsgQueueSend() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMsgQueue_Recv
 *
 *  @Description:: Receive message from queue
 *
 *  @Input      ::
 *    pMsgQueue:   Pointer to the message queue
 *
 *  @Output     ::
 *    pMsg:        Pointer to the received message
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMsgQueue_Recv(ArmMsgQueue_t *pMsgQueue, void *pMsg)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsg == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsg is null", 0U, 0U);
        Rval = ARM_NG;
    } else {
        // do nothing
    }

    /* 2. Receive message queue */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_MsgQueueReceive(pMsgQueue, pMsg, AMBA_KAL_WAIT_FOREVER);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## AmbaKAL_MsgQueueReceive() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmMsgQueue_Query
 *
 *  @Description:: Query message queue status
 *
 *  @Input      ::
 *    pMsgQueue:   Pointer to the message queue
 *
 *  @Output     ::
 *    pQueryInfo:  Pointer to the queried information
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmMsgQueue_Query(ArmMsgQueue_t *pMsgQueue, ARM_MSG_QUEUE_QUERY_INFO_s *pQueryInfo)
{
    UINT32 Rval = ARM_OK;
    AMBA_KAL_MSG_QUEUE_INFO_s Info;

    (void)ArmStdC_memset(&Info, 0, sizeof(Info));

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pQueryInfo == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pQueryInfo is null", 0U, 0U);
        Rval = ARM_NG;
    } else {
        /* 2. Query message queue */
        Rval = AmbaKAL_MsgQueueQuery(pMsgQueue, &Info);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## AmbaKAL_MsgQueueQuery() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            pQueryInfo->NumEnqueued  = Info.NumEnqueued;
            pQueryInfo->NumAvailable = Info.NumAvailable;
        }
    }
    return Rval;
}

