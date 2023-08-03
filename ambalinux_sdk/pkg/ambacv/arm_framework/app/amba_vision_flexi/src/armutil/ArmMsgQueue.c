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

#include <stdio.h>    // for snprintf
#include <errno.h>

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
    struct mq_attr attr = {0};
    char pName[128] = {0};

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsgQueueName == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueueName is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Create message queue (mq_open()) */
    if (Rval == ARM_OK) {
        attr.mq_maxmsg  = MaxMsgNum;
        attr.mq_msgsize = MsgSize;
        snprintf(pName, sizeof(pName), "/%s", pMsgQueueName); // The name must start from "/"

        *pMsgQueue = mq_open(pName, O_CREAT | O_RDWR, 0644, &attr);
        if (*pMsgQueue == -1) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_open() fail (errno = %d)", errno, 0U);
            Rval = ARM_NG;
        }
    }

    if (Rval == ARM_OK) {
        ArmLog_STR(ARM_LOG_MSG_Q, "Success to create MsgQueue (%s)", pName, NULL);
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
UINT32 ArmMsgQueue_Delete(ArmMsgQueue_t *pMsgQueue, char *pMsgQueueName)
{
    UINT32 Rval = ARM_OK;
    char pName[128] = {0};

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsgQueueName == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueueName is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Close and unlink message queue (mq_close() -> mq_unlink()) */
    if (Rval == ARM_OK) {
        snprintf(pName, sizeof(pName), "/%s", pMsgQueueName); // The name must start from "/"

        if (mq_close(*pMsgQueue) != 0) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_close() fail (errno = %d)", errno, 0U);
            Rval = ARM_NG;
        } else {
            if (mq_unlink(pName) != 0) {
                ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_unlink() fail (errno = %d)", errno, 0U);
                Rval = ARM_NG;
            }
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
    struct mq_attr attr = {0};

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsg == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsg is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Send message */
    if (Rval == ARM_OK) {
        if (mq_getattr(*pMsgQueue, &attr) != 0) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_getattr() fail (errno = %d)", errno, 0U);
            Rval = ARM_NG;
        } else {
            if (mq_send(*pMsgQueue, (char *)pMsg, attr.mq_msgsize, 0) != 0) {
                ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_send() fail (errno = %d)", errno, 0U);
                Rval = ARM_NG;
            }
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
    struct mq_attr attr = {0};

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pMsg == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsg is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Receive message */
    if (Rval == ARM_OK) {
        int r = 0;
        if (mq_getattr(*pMsgQueue, &attr) != 0) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_getattr() fail (errno = %d)", errno, 0U);
            Rval = ARM_NG;
        } else {
            if (mq_receive(*pMsgQueue, (char *)pMsg, attr.mq_msgsize, NULL) != attr.mq_msgsize) {
                ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_receive() fail (errno = %d)", errno, 0U);
                Rval = ARM_NG;
            }
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
    struct mq_attr attr = {0};

    /* 1. Sanity check for input parameters */
    if (pMsgQueue == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pMsgQueue is null", 0U, 0U);
        Rval = ARM_NG;
    } else if (pQueryInfo == NULL) {
        ArmLog_ERR(ARM_LOG_MSG_Q, "## pQueryInfo is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Query message queue */
    if (Rval == ARM_OK) {
        if (mq_getattr(*pMsgQueue, &attr) != 0) {
            ArmLog_ERR(ARM_LOG_MSG_Q, "## mq_getattr() fail (errno = %d)", errno, 0U);
            Rval = ARM_NG;
        } else {
            pQueryInfo->NumEnqueued  = attr.mq_curmsgs;
            pQueryInfo->NumAvailable = attr.mq_maxmsg - attr.mq_curmsgs;
        }
    }

    return Rval;
}

