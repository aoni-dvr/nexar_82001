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
#include "ArmStdC.h"

#define ARM_LOG_TASK          "ArmUtil_Task"

#define MAX_STACK_SIZE            (0x10000U)
#define MAX_TSK_NUM               (32U)

typedef struct {
    UINT8      Buf[MAX_STACK_SIZE];
    UINT32     Valid;
    ArmTask_t  *pTask;
} STACK_BUF_CTRL_s;

static STACK_BUF_CTRL_s g_StackBufCtrl[MAX_TSK_NUM] = {0};

/*---------------------------------------------------------------------------*\
 *  @RoutineName:: GetAvailableStackBufCtrl
 *
 *  @Description:: Get available stack buffer control
 *
 *  @Input      ::
 *
 *  @Output     ::
 *     pCtrl:     The stack buffer control
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static STACK_BUF_CTRL_s *GetAvailableStackBufCtrl(void)
{
    /* Get available stack buffer control */
    STACK_BUF_CTRL_s *pCtrl = NULL;
    UINT32 Idx  = 0U;
    for (Idx = 0U; Idx < MAX_TSK_NUM; Idx++) {
        if (g_StackBufCtrl[Idx].Valid == 0U) {
            g_StackBufCtrl[Idx].Valid = 1;
            pCtrl = &g_StackBufCtrl[Idx];
            break;
        }
    }

    if (MAX_TSK_NUM == Idx) {
        ArmLog_ERR(ARM_LOG_TASK, "## GetAvailableStackBufCtrl fail (No more stack buf to use)", 0U, 0U);
    }

    return pCtrl;
}


/*---------------------------------------------------------------------------*\
 *  @RoutineName:: FreeStackBufCtrl
 *
 *  @Description:: Free stack buffer control
 *
 *  @Input      ::
 *     pTask:      The task to terminate
 *
 *  @Output     :: None
 *
 *  @Return     ::
 *     UINT32      ARM_OK(0)/ARM_NG(0xFFFFFFFF)
\*---------------------------------------------------------------------------*/
static UINT32 FreeStackBufCtrl(ArmTask_t *pTask)
{
    UINT32 Rval = ARM_OK;

    /* 1. Sanity check */
    if (pTask == NULL) {
        ArmLog_ERR(ARM_LOG_TASK, "## pTask is null", 0U, 0U);
        Rval = ARM_NG;
    }

    /* 2. Free stack buffer control */
    if (Rval == ARM_OK) {
        UINT32 Idx  = 0U;
        for (Idx = 0U; Idx < MAX_TSK_NUM; Idx++) {
            if (g_StackBufCtrl[Idx].pTask == pTask) {
                g_StackBufCtrl[Idx].Valid = 0;
                g_StackBufCtrl[Idx].pTask = NULL;
                break;
            }
        }

        if (MAX_TSK_NUM == Idx) {
            ArmLog_ERR(ARM_LOG_TASK, "## FreeStackBufCtrl fail (Can't find given task)", 0U, 0U);
            Rval = ARM_NG;
        }

    }

    return Rval;
}


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
    } else {
        STACK_BUF_CTRL_s* pStackBufCtrl = GetAvailableStackBufCtrl();

        if (pStackBufCtrl != NULL) {
            /* 2. Create task (AmbaKAL_TaskCreate() -> AmbaKAL_TaskSetSmpAffinity() -> AmbaKAL_TaskResume()) */
            AMBA_KAL_TASK_ENTRY_f EntryFunction;

            // EntryFunction = (AMBA_KAL_TASK_ENTRY_f) pTaskCreateArg->EntryFunction;
            (void) ArmStdC_memcpy(&EntryFunction, &pTaskCreateArg->EntryFunction, sizeof(AMBA_KAL_TASK_ENTRY_f));
            Rval = AmbaKAL_TaskCreate(pTask, pTaskName, pTaskCreateArg->TaskPriority, EntryFunction,
                                      pTaskCreateArg->EntryArg, (void *) &pStackBufCtrl->Buf[0], MAX_STACK_SIZE, 0);
            pStackBufCtrl->pTask = pTask;
            if (Rval != KAL_ERR_NONE) {
                ArmLog_ERR(ARM_LOG_TASK, "## AmbaKAL_TaskCreate() fail (Rval = 0x%x)", Rval, 0U);
                Rval = ARM_NG;
            } else {
                Rval = AmbaKAL_TaskSetSmpAffinity(pTask, pTaskCreateArg->CoreSel);
                if (Rval != KAL_ERR_NONE) {
                    ArmLog_ERR(ARM_LOG_TASK, "## AmbaKAL_TaskSetSmpAffinity() fail (Rval = 0x%x)", Rval, 0U);
                    Rval = ARM_NG;
                } else {
                    Rval = AmbaKAL_TaskResume(pTask);
                    if (Rval != KAL_ERR_NONE) {
                        ArmLog_ERR(ARM_LOG_TASK, "## AmbaKAL_TaskResume() fail (Rval = 0x%x)", Rval, 0U);
                        Rval = ARM_NG;
                    }
                }
            }
        } else {
            ArmLog_ERR(ARM_LOG_TASK, "## Fail to allocate task stack buffer", 0U, 0U);
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

    /* 2. Delete task (AmbaKAL_TaskTerminate() -> AmbaKAL_TaskDelete()) */
    if (Rval == ARM_OK) {
        Rval = AmbaKAL_TaskTerminate(pTask);
        if (Rval != KAL_ERR_NONE) {
            ArmLog_ERR(ARM_LOG_TASK, "## AmbaKAL_TaskTerminate() fail (Rval = 0x%x)", Rval, 0U);
            Rval = ARM_NG;
        } else {
            Rval = AmbaKAL_TaskDelete(pTask);
            if (Rval != KAL_ERR_NONE) {
                ArmLog_ERR(ARM_LOG_TASK, "## AmbaKAL_TaskDelete() fail (Rval = 0x%x)", Rval, 0U);
                Rval = ARM_NG;
            }
        }

        (void) FreeStackBufCtrl(pTask);
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
    UINT32 Rval;

    Rval = AmbaKAL_TaskSleep(MilliSecond);
    if (Rval != KAL_ERR_NONE) {
        ArmLog_ERR(ARM_LOG_TASK, "## AmbaKAL_TaskSleep() fail (Rval = 0x%x)", Rval, 0U);
        Rval = ARM_NG;
    }

    return Rval;
}

