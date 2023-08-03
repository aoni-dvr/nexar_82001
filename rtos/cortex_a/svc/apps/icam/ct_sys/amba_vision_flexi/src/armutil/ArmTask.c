/**
*  @file ArmTask.c
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

/* Arm header */
#include "ArmTask.h"
#include "ArmErrCode.h"
#include "ArmLog.h"

#include <errno.h>

#define ARM_LOG_TASK          "ArmUtil_Task"

/*---------------------------------------------------------------------------*\
 * APIs
\*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmTask_Create
 *
 *  @Description:: Create Task
 *
 *  @Input      ::
 *    pTaskName:      The task name
 *    pTaskCreateArg: Pointer to the task create arguments
 *
 *  @Output     ::
 *    pTask:          Pointer to the task
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmTask_Create(ArmTask_t *pTask, char *pTaskName, ARM_TASK_CREATE_ARG_s *pTaskCreateArg)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for input parameters */
  if (pTask == NULL) {
    ArmLog_ERR(ARM_LOG_TASK, "## pTask is null", 0U, 0U);
    Rval = ARM_NG;
  } else if (pTaskName == NULL) {
    ArmLog_ERR(ARM_LOG_TASK, "## pTaskName is null", 0U, 0U);
    Rval = ARM_NG;
  } else if (pTaskCreateArg == NULL) {
    ArmLog_ERR(ARM_LOG_TASK, "## pTaskCreateArg is null", 0U, 0U);
    Rval = ARM_NG;
  }


  /* 2. Create thread (pthread_create()) */
  if (Rval == ARM_OK) {
    Rval = pthread_create(pTask, NULL, pTaskCreateArg->EntryFunction, pTaskCreateArg->EntryArg);
    if (Rval != 0U) {
      ArmLog_ERR(ARM_LOG_TASK, "## pthread_create() fail (Rval = 0x%x)", Rval, 0U);
      Rval = ARM_NG;
    }
  }

  if (Rval == ARM_OK) {
    ArmLog_STR(ARM_LOG_TASK, "Success to create Task (%s)", pTaskName, NULL);
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmTask_Delete
 *
 *  @Description:: Delete Task
 *
 *  @Input      ::
 *    pTask:       Pointer to the task
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmTask_Delete(ArmTask_t *pTask)
{
  UINT32 Rval = ARM_OK;

  /* 1. Sanity check for input parameters */
  if (pTask == NULL) {
    ArmLog_ERR(ARM_LOG_TASK, "## pTask is null", 0U, 0U);
    Rval = ARM_NG;
  }

  /* 2. Delete thread (pthread_cancel() -> pthread_join()) */
  if (Rval == ARM_OK) {
    Rval = pthread_cancel(*pTask);
    if (Rval != 0) {
      ArmLog_ERR(ARM_LOG_TASK, "## pthread_cancel() fail (Rval = 0x%x)", Rval, 0U);
      Rval = ARM_NG;
    } else {
      Rval = pthread_join(*pTask, NULL);
      if (Rval != 0) {
        ArmLog_ERR(ARM_LOG_TASK, "## pthread_join() fail (Rval = 0x%x)", Rval, 0U);
        Rval = ARM_NG;
      }
    }
  }

  return Rval;
}

/*-----------------------------------------------------------------------------------------------*\
 *  @RoutineName:: ArmTask_Sleep
 *
 *  @Description:: Task sleep
 *
 *  @Input      ::
 *    MilliSecond: The sleep time (unit: ms)
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *    UINT32:      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*-----------------------------------------------------------------------------------------------*/
UINT32 ArmTask_Sleep(UINT32 MilliSecond)
{
  UINT32 Rval = ARM_OK;

  if (usleep(MilliSecond*1000) != 0) {
    ArmLog_ERR(ARM_LOG_TASK, "## usleep() fail (errno = %d)", errno, 0U);
    Rval = ARM_NG;
  }

  return Rval;
}

