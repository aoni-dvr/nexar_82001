/**
 *  @file AmbaFS_KAL.c
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details  Kernel abstraction layer of Ambarella File System.
 */

#include "AmbaTypes.h"
#include "AmbaFS_KAL.h"
#include "AmbaMisraFix.h"

#if PF_USE_API_STUB
/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_create_task
 *
 *  @Description:: Create a task and assign an ID
 *
 *  @Input   ::
 *      pTaskCreate:    Pointer to task information structure
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : Task ID(positive)/Failure(negative/zero)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_create_task(PFK_CTSK* pTaskCreate)
{
    INT32 RetStatus = NG;

    if (pTaskCreate != NULL) {
        RetStatus = AmbaKAL_TaskCreate(&pTaskCreate->Task, pTaskCreate->pTaskName, pTaskCreate->Priority,
                                       pTaskCreate->EntryFunction, pTaskCreate->EntryArg,
                                       pTaskCreate->pStackBase, pTaskCreate->StackByteSize,
                                       0);
    }

    return (RetStatus == OK) ? (PF_S_LONG)&pTaskCreate->Task : (PF_S_LONG)NULL;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_delete_task
 *
 *  @Description:: Delete a task
 *
 *  @Input   ::
 *      TaskID:     Task ID
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_delete_task(PF_S_LONG TaskID)
{
    return (PF_S_LONG)AmbaKAL_TaskDelete((AMBA_KAL_TASK_t *)TaskID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_start_task
 *
 *  @Description:: Start a task
 *
 *  @Input   ::
 *      TaskID:     Task ID
 *      StartCode:  Task start code (enhancing information)
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_start_task(PF_S_LONG TaskID, PF_S_LONG StartCode)
{
    return (PF_S_LONG)AmbaKAL_TaskResume((AMBA_KAL_TASK_t *)TaskID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_terminate_task
 *
 *  @Description:: Terminate a task
 *
 *  @Input   ::
 *      TaskID:     Task ID
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_terminate_task(PF_S_LONG TaskID)
{
    return (PF_S_LONG)AmbaKAL_TaskTerminate((AMBA_KAL_TASK_t *)TaskID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_sleep_task
 *
 *  @Description:: Stop a task
 *
 *  @Input   :: none
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_sleep_task(void)
{
    AMBA_KAL_TASK_t *pTmp = NULL;

    AmbaKAL_TaskIdentify(&pTmp);
    return (PF_S_LONG)AmbaKAL_TaskSuspend(pTmp);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_wakeup_task
 *
 *  @Description:: Cancel the stop status of a task
 *
 *  @Input   ::
 *      TaskID:     Task ID
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_wakeup_task(PF_S_LONG TaskID)
{
    return (PF_S_LONG)AmbaKAL_TaskResume((AMBA_KAL_TASK_t *)TaskID);
}


/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_create_mailbox
 *
 *  @Description:: Create a resource to transmit a message and assign an ID.
 *
 *  @Input   ::
 *      pMailboxCreate: Pointer to resource information structure for message transmission
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : Mailbox ID(positive)/Failure(negative/zero)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_create_mailbox(PFK_CMBX * pMailboxCreate)
{
    INT32 RetStatus = 0;

    if (pMailboxCreate != NULL) {
        if (OK == AmbaKAL_MsgQueueCreate(&pMailboxCreate->MsgQueue, NULL, pMailboxCreate->MsgSize, pMailboxCreate->pMsgQueueBase,
                                         pMailboxCreate->MaxNumMsg)) {
            RetStatus = &pMailboxCreate->MsgQueue;
        }
    }

    return RetStatus;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_delete_mailbox
 *
 *  @Description:: Delete the resouce to execute message transmission
 *
 *  @Input   ::
 *      MailboxID:  Message transmission resource ID
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_delete_mailbox(PF_S_LONG MailboxID)
{
    return (PF_S_LONG)AmbaKAL_MsgQueueDelete((AMBA_KAL_MSG_QUEUE_t *)MailboxID);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_receive_message
 *
 *  @Description:: Delete the resouce to execute message transmission
 *
 *  @Input   ::
 *      MailboxID:  Message transmission resource ID
 *
 *  @Output  ::
 *      pMsgDest:   Pointer to message buffer
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG  pfk_receive_message(PF_S_LONG MailboxID, PFK_MSG ** pMsgDest)
{
    return AmbaKAL_MsgQueueReceive((AMBA_KAL_MSG_QUEUE_t *)MailboxID, *pMsgDest, TX_WAIT_FOREVER);
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_send_message
 *
 *  @Description:: Delete the resouce to execute message transmission
 *
 *  @Input   ::
 *      MailboxID:  Message transmission resource ID
 *      pMsgSource: Pointer to message buffer
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG  pfk_send_message(PF_S_LONG MailboxID, PFK_MSG * pMsgSource)
{
    return AmbaKAL_MsgQueueSend((AMBA_KAL_MSG_QUEUE_t *)MailboxID, pMsgSource, KAL_WAIT_FOREVER);
}
#endif

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_get_task_id
 *
 *  @Description:: Acquire a task ID
 *
 *  @Input   :: none
 *
 *  @Output  ::
 *      pTaskID:    Pointer to buffer to store the Task ID
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_get_task_id(PF_S_LONG* pTaskID)
{
    AMBA_KAL_TASK_t *pTask = NULL;

    PF_S_LONG RetVal = 0;

    if (pTaskID == NULL) {
        RetVal = -1;
    } else {
        if (AmbaKAL_TaskIdentify(&pTask) != 0U) {
            RetVal =  -1;
        } else {
            AmbaMisra_TypeCast32(&RetVal, &pTask);
            (*pTaskID) = RetVal;
            RetVal = 0;
        }
    }

    return RetVal;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_create_semaphore
 *
 *  @Description:: Create a resource to execute exclusive control and assign an ID
 *
 *  @Input   ::
 *      pSemCreate: Pointer to resource information structure for exclusive control
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : Semaphore ID(positive)/Failure(negative/zero)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG pfk_create_semaphore(PFK_CSEM *pSemCreate)
{
    INT32 RetStatus = -1;
    const AMBA_KAL_SEMAPHORE_t *pSemID = NULL;

    if (pSemCreate != NULL) {
        AmbaMisra_TouchUnused(&pSemCreate);

        if (KAL_ERR_NONE == AmbaKAL_SemaphoreCreate(&pSemCreate->Sem, NULL, pSemCreate->InitCount)) {
            pSemID = &pSemCreate->Sem;
            AmbaMisra_TypeCast32(&RetStatus, &pSemID);
        }
    }

    return RetStatus;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_delete_semaphore
 *
 *  @Description:: Delete the exclusive control resouce
 *
 *  @Input   ::
 *      SemID:      ID of resource for exclusive control
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG  pfk_delete_semaphore(PF_S_LONG SemID)
{
    INT32 Rval = -1;
    AMBA_KAL_SEMAPHORE_t *pSem = NULL;

    AmbaMisra_TypeCast32(&pSem, &SemID);

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreDelete(pSem)) {
        Rval = 0;
    }
    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_get_semaphore
 *
 *  @Description:: Acquire a resource to execute exclusive control
 *
 *  @Input   ::
 *      SemID:      ID of resource for exclusive control
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG  pfk_get_semaphore(PF_S_LONG SemID)
{
    INT32 Rval = -1;
    AMBA_KAL_SEMAPHORE_t *pSem = NULL;

    AmbaMisra_TypeCast32(&pSem, &SemID);

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreTake(pSem, 0xFFFFFFFFU)) {
        Rval = 0;
    }

    return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: pfk_release_semaphore
 *
 *  @Description:: Release the exclusive control resource
 *
 *  @Input   ::
 *      SemID:      ID of resource for exclusive control
 *
 *  @Output  :: none
 *
 *  @Return  ::
 *       PF_S_LONG : OK(0)/NG(-1)
\*-----------------------------------------------------------------------------------------------*/
PF_S_LONG  pfk_release_semaphore(PF_S_LONG SemID)
{
    INT32 Rval = -1;

    AMBA_KAL_SEMAPHORE_t *pSem = NULL;

    AmbaMisra_TypeCast32(&pSem, &SemID);

    if (KAL_ERR_NONE == AmbaKAL_SemaphoreGive(pSem)) {
        Rval = 0;
    }

    return Rval;
}
