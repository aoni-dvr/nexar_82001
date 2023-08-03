/**
*  @file SvcTask.c
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
*  @details task create/destroy functions
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaUtility.h"
#include "AmbaKAL.h"

#include "SvcWrap.h"
#include "SvcTask.h"

/**
* wrap of os task create
* @param [in] pName task name
* @param [in] pCtrl control block of task
* @return 0-OK, 1-NG
*/
UINT32 SvcTask_Create(const char *pName, SVC_TASK_CTRL_s *pCtrl)
{
    char        *pTaskName;
    UINT32       Rval;
    const ULONG  *pArg = &(pCtrl->EntryArg);

    AmbaMisra_TypeCast(&pTaskName, &pName);

    Rval = AmbaKAL_TaskCreate(&(pCtrl->Task),
                                pTaskName,
                                pCtrl->Priority,
                                pCtrl->EntryFunc,
                                pArg,
                                pCtrl->pStackBase,
                                pCtrl->StackSize,
                                0);

    if (KAL_ERR_NONE == Rval) {
        Rval = AmbaKAL_TaskSetSmpAffinity(&(pCtrl->Task), pCtrl->CpuBits);
        if (KAL_ERR_NONE == Rval) {
            Rval = AmbaKAL_TaskResume(&(pCtrl->Task));
        }
    }

    return Rval;
}

/**
* wrap of os task destroy
* @param [in] pCtrl control block of task
* @return 0-OK, 1-NG
*/
UINT32 SvcTask_Destroy(SVC_TASK_CTRL_s *pCtrl)
{
    UINT32  Rval;

    Rval = AmbaKAL_TaskTerminate(&(pCtrl->Task));
    if (KAL_ERR_NONE == Rval) {
        Rval = AmbaKAL_TaskDelete(&(pCtrl->Task));
    }

    return Rval;
}
