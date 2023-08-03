/**
*  @file ArmMsgQueue.h
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

#ifndef ARM_MSG_QUEUE_H
#define ARM_MSG_QUEUE_H

#include "AmbaTypes.h"
#include "mqueue.h"

typedef   mqd_t   ArmMsgQueue_t;

typedef struct {
  UINT32  NumEnqueued;      /* The number of messages currently in the queue */
  UINT32  NumAvailable;     /* The number of messages the queue currently has space for */
} ARM_MSG_QUEUE_QUERY_INFO_s;

/******************************************************************************
 *  Defined in ArmMsgQueue.c
 ******************************************************************************/
UINT32 ArmMsgQueue_Create(ArmMsgQueue_t *pMsgQueue, char *pMsgQueueName, UINT32 MsgSize, UINT32 MaxMsgNum, void *pMsgQueueBuf);
UINT32 ArmMsgQueue_Delete(ArmMsgQueue_t *pMsgQueue, char *pMsgQueueName);
UINT32 ArmMsgQueue_Send(ArmMsgQueue_t *pMsgQueue, void *pMsg);
UINT32 ArmMsgQueue_Recv(ArmMsgQueue_t *pMsgQueue, void *pMsg);
UINT32 ArmMsgQueue_Query(ArmMsgQueue_t *pMsgQueue, ARM_MSG_QUEUE_QUERY_INFO_s *pQueryInfo);

#endif /* ARM_MSG_QUEUE_H */

