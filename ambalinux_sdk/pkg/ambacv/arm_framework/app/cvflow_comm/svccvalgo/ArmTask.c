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
#include <stdint.h>

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
UINT32 ArmTask_Create(ArmTask_t *pTask, char *pTaskName, const ARM_TASK_CREATE_ARG_s *pTaskCreateArg)
{
    UINT32 Rval = ARM_OK;
    struct sched_param param;
    //preempt & starve low pri tasks, use round-robin for same pri task
    const int policy = SCHED_RR;
    int32_t max_pri_RR = sched_get_priority_max(policy);
    int32_t min_pri_RR = sched_get_priority_min(policy);
    pthread_attr_t attr;

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

    pthread_attr_init(&attr);

    /* 2. Create thread (pthread_create()) */
    if (Rval == ARM_OK) {
        Rval = pthread_create(pTask, &attr, pTaskCreateArg->EntryFunction, pTaskCreateArg->EntryArg);

        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_TASK, "## pthread_create() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        }
    }

    /* 3. Set attributes */
    if (Rval == ARM_OK) {
        /*
         * NOTE:
         * The two OS use different range of priority, like these:
         * ThreadX uses:  1 (highest) <-----> 127 (lowest)
         * Linux RR uses: 1 (lowest)  <-----> 99 (highest)
         * (KAL_TASK_PRIORITY_LOWEST = 127 and KAL_TASK_PRIORITY_HIGHEST = 1)
         * Hence, We need to map them for consistency.
         *
         * Because the range of Linux is smaller than the range of ThreadX,
         * the last lower priority which are more than 99 priority
         * will be mapped to lowest priority in Linux RR which is 1.
         *
         * Appendix:
         * There are two categories for Linux scheduling policy:
         * (1) 'Normal' scheduling policies: SCHED_OTHER, SCHED_BATCH and SCHED_IDLE
         * (2) 'Real-time' scheduling policies: SCHED_FIFO and SCHED_RR
         *
         * for normal processes: PR = 20 + NI (NI is nice and ranges from -20 to 19)
         * for real time processes: PR = - 1 - real_time_priority (real_time_priority ranges from 1 to 99)
         *
         */
        if ((int32_t)pTaskCreateArg->TaskPriority > max_pri_RR) {
            /* Revert priority value to the lowest for Linux RR, ex: 100->1, 101->1 */
            param.sched_priority = min_pri_RR;
        } else if ((int32_t)pTaskCreateArg->TaskPriority < min_pri_RR) {
            /* Revert priority value to the highest for Linux RR,
             * ex: 0->99 (this may not exist in current setting) */
            param.sched_priority = max_pri_RR;
        } else {
            /* Revert priority value, ex: 1->99, 2->98,..., 98->2, 99->1 */
            param.sched_priority = (max_pri_RR + min_pri_RR) - (int32_t)pTaskCreateArg->TaskPriority;
        }

        Rval = pthread_setschedparam(*pTask, policy, &param);
        if (Rval != 0U) {
            ArmLog_ERR(ARM_LOG_TASK, "## pthread_setschedparam() fail (Rval = 0x%x)", Rval, 0U);
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

