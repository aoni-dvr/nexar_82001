/**
*  @file ArmTask.h
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
*   @details The Arm task (thread) utility
*
*/

#ifndef ARM_TASK_H
#define ARM_TASK_H

#include "AmbaTypes.h"
#include "pthread.h"
#include "unistd.h"

#define ARM_TASK_CORE0             0x01U
#define ARM_TASK_CORE1             0x02U
#define ARM_TASK_CORE2             0x04U
#define ARM_TASK_CORE3             0x08U

#define ARM_TASK_NULL_RET          NULL

typedef   pthread_t         ArmTask_t;
typedef   void*             ArmTaskEntryArg_t;
typedef   void*             ArmTaskRet_t;
typedef   ArmTaskRet_t (*ARM_TASK_ENTRY_f) (ArmTaskEntryArg_t EntryArg);

typedef struct {
  UINT32              TaskPriority;
  ARM_TASK_ENTRY_f    EntryFunction;
  ArmTaskEntryArg_t   EntryArg;
  void                *pStackBase;
  UINT32              StackByteSize;
  UINT32              CoreSel;
} ARM_TASK_CREATE_ARG_s;

/******************************************************************************
 *  Defined in ArmTask.c
 ******************************************************************************/
UINT32 ArmTask_Create(ArmTask_t *pTask, char *pTaskName, ARM_TASK_CREATE_ARG_s *pTaskCreateArg);
UINT32 ArmTask_Delete(ArmTask_t *pTask);
UINT32 ArmTask_Sleep(UINT32 MilliSecond);

#endif /* ARM_TASK_H */

