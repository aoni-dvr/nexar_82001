/**
*  @file SvcLoadCtrl.c
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
*  @details svc cpu loading control
*
*/

#include "AmbaKAL.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaShell.h"
#include "AmbaUtility.h"
#include "AmbaSvcWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcWrap.h"
#include "SvcTask.h"
#include "SvcLoadCtrl.h"

#define LOG_LOADC    "LOADC"

typedef struct {
    UINT32           IsUsed;
    UINT32           Loading;
    UINT32           SleepMs;
    SVC_TASK_CTRL_s  Task;
    #define LOAD_CTRL_STACK_SIZE        (0x800U)
    UINT8            Stack[LOAD_CTRL_STACK_SIZE];
} SVC_LOAD_CTRL_s;

static SVC_LOAD_CTRL_s  g_LoadCtrl[CONFIG_SMP_NUM_CORES] GNU_SECTION_NOZEROINIT;

static void* LoadCtrl_TaskEntry(void* EntryArg)
{
    UINT32                 i, Num = 0U, Rval;
    UINT32                 BaseMs, CalcMs, LoadMs;
    const SVC_LOAD_CTRL_s  *pCtrl;
    const ULONG            *pArg;
    ULONG                  Arg;

    AmbaMisra_TouchUnused(EntryArg);
    AmbaMisra_TypeCast(&pArg, &EntryArg);
    Arg = (*pArg);
    AmbaMisra_TypeCast(&pCtrl, &Arg);

    LoadMs = 1000U - pCtrl->SleepMs;
    Rval = AmbaKAL_GetSysTickCount(&BaseMs);
    while (Rval == SVC_OK) {
        if (Num < 2U) {
            Num = 2U;
        }

        for (i = 2U; i <= (Num / 2U); ++i) {
            if ((Num % i) == 0U) {
                break;
            }
        }

        Rval = AmbaKAL_GetSysTickCount(&CalcMs);
        if (Rval == SVC_OK) {
            if (BaseMs <= CalcMs) {
                CalcMs -= BaseMs;
            } else {
                CalcMs += (0xFFFFFFFFU - BaseMs);
            }

            if (LoadMs <= CalcMs) {
                Rval = AmbaKAL_TaskSleep(pCtrl->SleepMs);
                if (Rval == SVC_OK) {
                    /* reset baseMs */
                    LoadMs = 1000U - pCtrl->SleepMs;
                    Rval = AmbaKAL_GetSysTickCount(&BaseMs);
                }
            }
            Num++;
        }
    }

    SvcLog_NG(LOG_LOADC, "LoadCtrl task terminate", 0U, 0U);
    return NULL;
}


/**
* create of loading control task
* @param [in] CpuBits cpu bits to run
* @param [in] Loading loading
* @return 0-OK, 1-NG
*/
UINT32 SvcLoadCtrl_Create(UINT32 CpuBits, UINT32 Loading)
{
    static UINT32  g_IsInited = 0U;

    UINT32           i, Rval = SVC_OK, Bit;
    SVC_LOAD_CTRL_s  *pCtrl;

    if (g_IsInited == 0U) {
        AmbaSvcWrap_MisraMemset(g_LoadCtrl, 0, sizeof(g_LoadCtrl));
        g_IsInited = 1U;
    }

    for (i = 0U; i < (UINT32)CONFIG_SMP_NUM_CORES; i++) {
        Bit = ((UINT32)0x01U << i);
        if (0U < (CpuBits & Bit)) {
            pCtrl = &(g_LoadCtrl[i]);
            if (pCtrl->IsUsed == 0U) {
                pCtrl->Task.Priority   = SVC_TASK_LOWEST_PRIORITY;
                pCtrl->Task.EntryFunc  = LoadCtrl_TaskEntry;
                pCtrl->Task.pStackBase = pCtrl->Stack;
                pCtrl->Task.StackSize  = LOAD_CTRL_STACK_SIZE;
                pCtrl->Task.CpuBits    = Bit;
                pCtrl->Loading         = Loading;
                pCtrl->SleepMs         = (100U - Loading) * 10U;
                AmbaMisra_TypeCast(&(pCtrl->Task.EntryArg), &pCtrl);

                if (SvcTask_Create("SvcLoadCtrl", &(pCtrl->Task)) == SVC_OK) {
                    pCtrl->IsUsed = 1U;
                } else {
                    Rval = SVC_NG;
                    SvcLog_NG(LOG_LOADC, "Fail to create task", 0U, 0U);
                }
            } else{
                SvcLog_DBG(LOG_LOADC, "Task is created", 0U, 0U);
            }
        }
    }

    return Rval;
}

/**
* destroy of loading control task
* @param [in] CpuBits cpu bits to run
* @return 0-OK, 1-NG
*/
UINT32 SvcLoadCtrl_Destroy(UINT32 CpuBits)
{
    UINT32           i, Rval = SVC_OK;
    SVC_LOAD_CTRL_s  *pCtrl;

    for (i = 0U; i < (UINT32)CONFIG_SMP_NUM_CORES; i++) {
        if (0U < (CpuBits & ((UINT32)0x01U << i))) {
            pCtrl = &(g_LoadCtrl[i]);
            if (0U < pCtrl->IsUsed) {
                if (SvcTask_Destroy(&(pCtrl->Task)) == SVC_OK) {
                    AmbaSvcWrap_MisraMemset(pCtrl, 0, sizeof(SVC_LOAD_CTRL_s));
                } else {
                    SvcLog_NG(LOG_LOADC, "Fail to destrou task", 0U, 0U);
                    Rval = SVC_NG;
                }
            } else {
                SvcLog_DBG(LOG_LOADC, "Task isn't existed", 0U, 0U);
            }
        }
    }

    return Rval;
}

/**
* status dump of loading control task
* @param [in] PrintFunc print function of shell
* @return none
*/
void SvcLoadCtrl_Dump(AMBA_SHELL_PRINT_f PrintFunc)
{
    char                   StrBuf[256];
    UINT32                 i, BufLen = 256U, Rval;
    const SVC_LOAD_CTRL_s  *pCtrl;

    for (i = 0U; i < (UINT32)CONFIG_SMP_NUM_CORES; i++) {
        pCtrl = &(g_LoadCtrl[i]);

        SVC_WRAP_SNPRINT            "[core_%u] is_used(%u), loading(%u/100), sleep(%u ms)\n"
            SVC_SNPRN_ARG_S         StrBuf
            SVC_SNPRN_ARG_UINT32    i              SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->IsUsed  SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->Loading SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_UINT32    pCtrl->SleepMs SVC_SNPRN_ARG_POST
            SVC_SNPRN_ARG_BSIZE     BufLen
            SVC_SNPRN_ARG_RLEN      &Rval
            SVC_SNPRN_ARG_E
        PrintFunc(StrBuf);
    }
}
