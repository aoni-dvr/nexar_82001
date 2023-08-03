/**
 *  @file AmbaFS_KAL.h
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
 *  @details Kernel abstraction layer of Ambarella File System.
 *
 */

#ifndef AMBA_FS_KAL_H
#define AMBA_FS_KAL_H

#ifndef AMBA_TYPES_H
#include "AmbaTypes.h"
#endif
#include "AmbaKAL.h"

#include "prfile2/pf_types.h"

/*----------------------------------------------------------------------------*\
  Structure to create task
\*----------------------------------------------------------------------------*/
typedef struct {
    AMBA_KAL_TASK_t Task;
    char *pTaskName;
    UINT32 Priority;
    void (*EntryFunction)(UINT32 EntryArg);
    UINT32 EntryArg;
    void *pStackBase;
    UINT32 StackByteSize;
} PFK_CTSK;

/*----------------------------------------------------------------------------*\
  Structure to create mailbox
\*----------------------------------------------------------------------------*/
typedef struct {
    AMBA_KAL_MSG_QUEUE_t MsgQueue;
    void *pMsgQueueBase;
    UINT32 MsgSize;
    UINT32 MaxNumMsg;
} PFK_CMBX;

typedef UINT32  PFK_MSG;

/*----------------------------------------------------------------------------*\
  Structure to create semaphore
\*----------------------------------------------------------------------------*/
typedef struct {
    AMBA_KAL_SEMAPHORE_t Sem;
    UINT32 InitCount;
} PFK_CSEM;

PF_S_LONG pfk_create_task(PFK_CTSK * pTaskCreate);
PF_S_LONG pfk_delete_task(PF_S_LONG TaskID);
PF_S_LONG pfk_start_task(PF_S_LONG TaskID, PF_S_LONG StartCode);
PF_S_LONG pfk_terminate_task(PF_S_LONG TaskID);
PF_S_LONG pfk_get_task_id(PF_S_LONG* pTaskID);
PF_S_LONG pfk_sleep_task(void);
PF_S_LONG pfk_wakeup_task(PF_S_LONG TaskID);
PF_S_LONG pfk_create_mailbox(PFK_CMBX * pMailboxCreate);
PF_S_LONG pfk_delete_mailbox(PF_S_LONG MailboxID);
PF_S_LONG pfk_receive_message(PF_S_LONG MailboxID, PFK_MSG ** pMsgDest);
PF_S_LONG pfk_send_message(PF_S_LONG MailboxID, PFK_MSG * pMsgSource);
PF_S_LONG pfk_create_semaphore(PFK_CSEM * pSemCreate);
PF_S_LONG pfk_delete_semaphore(PF_S_LONG SemID);
PF_S_LONG pfk_get_semaphore(PF_S_LONG SemID);
PF_S_LONG pfk_release_semaphore(PF_S_LONG SemID);

#endif /* AMBA_FS_KAL_H */
