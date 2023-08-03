/**
 *  @file SvcFlowControl.c
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
 *  @details svc flow control module
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaUtility.h"
#include "AmbaShell.h"
#include "AmbaWrap.h"

#include "SvcErrCode.h"
#include "SvcLog.h"
#include "SvcTask.h"
#include "SvcWrap.h"
#include "SvcCmd.h"

#include "SvcFlowControl.h"

#define SVC_LOG_FLOW_CONTROL        "FLOW_CTRL"

#define FLOW_CONTROL_LIST_SIZE_MAX     (2U)

#if defined(CONFIG_ICAM_CV_STEREO_AUTO_CALIB)
#define FLOW_CONTROL_STACK_SIZE        (0x80000U)
#else
#if 1
#define FLOW_CONTROL_STACK_SIZE        (0x100000U)
#else
#define FLOW_CONTROL_STACK_SIZE        (0x24000U)
#endif
#endif

#define FLOW_CONTROL_CMD_HISTORY_NUM    (16U)
#define FLOW_CONTROL_CMD_STR_LEN        (16U)

typedef struct {
    char    Cmds[FLOW_CONTROL_CMD_HISTORY_NUM][FLOW_CONTROL_CMD_STR_LEN];
    UINT8   WrIdx;
    UINT8   RdIdx;
} SVC_FLOW_CONTROL_CMD_HISTORY_s;

/* Hooked list */
static SVC_FLOW_CONTROL_CMD_LIST_LINKER_s pSvcFlowControlCmdListLinker[FLOW_CONTROL_LIST_SIZE_MAX];

/* Message queue resource */
static AMBA_KAL_MSG_QUEUE_t SvcFlowControlQueue;
/* Mutex resource */
static AMBA_KAL_MUTEX_t SvcFlowControlMutex;
static SVC_FLOW_CONTROL_CMD_HISTORY_s FlowControlCmdHistory;

/* Control task entry */
static void* FlowControl_Entry(void* EntryArg);
static void FlowControl_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc);

/**
* initialization of flow control task
* @param [in] Priority task priority
* @param [in] CpuBits task cpu bits
* @return 0-OK, 1-NG
*/
UINT32 SvcFlowControl_Init(UINT32 Priority, UINT32 CpuBits)
{
    UINT32 RetVal;

    static SVC_TASK_CTRL_s FlowControlCtrl GNU_SECTION_NOZEROINIT;
    static UINT8 FlowControlStack[FLOW_CONTROL_STACK_SIZE] GNU_SECTION_NOZEROINIT;

    static char FlowControlQueueName[16] = "FlowControlQueue";
    static char FlowControlQueueBuf[32][16];

    static char FlowControlMutexName[16] = "FlowControlMutex";

    SvcLog_OK(SVC_LOG_FLOW_CONTROL, "Task init start", 0U, 0U);

    /* Create control task entry function waiting for control cmd */
    FlowControlCtrl.Priority   = Priority;
    FlowControlCtrl.EntryFunc  = FlowControl_Entry;
    FlowControlCtrl.pStackBase = FlowControlStack;
    FlowControlCtrl.StackSize  = FLOW_CONTROL_STACK_SIZE;
    FlowControlCtrl.CpuBits    = CpuBits;

    /* Create mutex */
    RetVal = AmbaKAL_MutexCreate(&SvcFlowControlMutex, FlowControlMutexName);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_FLOW_CONTROL, "mutex create failed with %d", RetVal, 0U);
        SvcFlowControl_Assert();
    }

    /* Initialize step start */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MutexTake(&SvcFlowControlMutex, AMBA_KAL_WAIT_FOREVER);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "mutex take failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Create control cmd message queue */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MsgQueueCreate(&SvcFlowControlQueue, FlowControlQueueName, 32U * (UINT32)sizeof(char), FlowControlQueueBuf, (UINT32)sizeof(FlowControlQueueBuf));
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "msg queue create failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    /* Create control task */
    if (SVC_OK == RetVal) {
        RetVal = SvcTask_Create("SvcFlowControl", &FlowControlCtrl);
        if (RetVal != SVC_OK) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "SvcFlowControl isn't created", 0U, 0U);
            SvcFlowControl_Assert();
        }
    }

    {
        AMBA_SHELL_COMMAND_s  SvcCmdFlow;

        UINT32  Rval;

        SvcCmdFlow.pName    = "svc_flow";
        SvcCmdFlow.MainFunc = FlowControl_CmdEntry;
        SvcCmdFlow.pNext    = NULL;

        Rval = SvcCmd_CommandRegister(&SvcCmdFlow);
        if (SHELL_ERR_SUCCESS != Rval) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "## fail to install svc flow command", 0U, 0U);
        }
    }

    /* Initialize done */
    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MutexGive(&SvcFlowControlMutex);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "mutex give failed with %d", RetVal, 0U);
            SvcFlowControl_Assert();
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_FLOW_CONTROL, "Task init done", 0U, 0U);
    }

    return RetVal;

}

/**
* execution of flow control task
* @param [in] pCmd command to execute
* @return 0-OK, 1-NG
*/
UINT32 SvcFlowControl_Exec(const char *pCmd)
{
    UINT32 RetVal = SVC_OK;
    char *pInputCmd;

    AmbaMisra_TypeCast(&pInputCmd, &pCmd);

    RetVal = AmbaKAL_MsgQueueSend(&SvcFlowControlQueue, pInputCmd, AMBA_KAL_WAIT_FOREVER);
    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_FLOW_CONTROL, "Cmd Message send failed with %d", RetVal, 0U);
    }

    return RetVal;
}

/**
* hook of command/operation list
* @param [in] pList command list
* @param [in] CmdNum number of command
* @param [in] pUsage command usage
* @return 0-OK, 1-NG
*/
UINT32 SvcFlowControl_CmdHook(SVC_FLOW_CONTROL_CMD_LIST_s *pList, UINT8 CmdNum, SVC_FLOW_CONTROL_USAGE pUsage)
{
    UINT32 RetVal = SVC_OK;
    UINT8 i;

    SvcLog_OK(SVC_LOG_FLOW_CONTROL, "SvcFlowControl_CmdHook() start", 0U, 0U);

    for (i = 0; i < FLOW_CONTROL_LIST_SIZE_MAX; i++) {
        if (pSvcFlowControlCmdListLinker[i].pList == NULL) {
            pSvcFlowControlCmdListLinker[i].pList = pList;
            pSvcFlowControlCmdListLinker[i].ListCmdNum = CmdNum;
            pSvcFlowControlCmdListLinker[i].pUsage = pUsage;
            break;
        }
    }

    if (i == FLOW_CONTROL_LIST_SIZE_MAX) {
        SvcLog_NG(SVC_LOG_FLOW_CONTROL, "SvcFlowControl_CmdHook() failed. No remain space for hook", 0U, 0U);
        RetVal = SVC_NG;
    }

    SvcLog_OK(SVC_LOG_FLOW_CONTROL, "SvcFlowControl_CmdHook() done", 0U, 0U);

    return RetVal;
}

/**
* assert the system
* @return none
*/
void SvcFlowControl_Assert(void)
{
    SvcLog_NG(SVC_LOG_FLOW_CONTROL, "Flow error. Force assertion", 0U, 0U);
#if !defined(CONFIG_LINUX) && !defined(CONFIG_QNX)
    AmbaPrint_Flush();
#endif
    AmbaAssert();
}

static void* FlowControl_Entry(void* EntryArg)
{
    UINT32 RetVal;
    char CmdName[FLOW_CONTROL_CMD_STR_LEN];
    UINT8 i, j;

    AmbaMisra_TouchUnused(EntryArg);

    /* Make sure mutex is done */
    RetVal = AmbaKAL_MutexTake(&SvcFlowControlMutex, AMBA_KAL_WAIT_FOREVER);

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_FLOW_CONTROL, "Initialize failed, mutex take with %d", RetVal, 0U);
    }

    if (SVC_OK == RetVal) {
        RetVal = AmbaKAL_MutexGive(&SvcFlowControlMutex);
        if (SVC_OK != RetVal) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "Initialize failed, mutex give with %d", RetVal, 0U);
        }
    }

    if (SVC_OK == RetVal) {
        SvcLog_OK(SVC_LOG_FLOW_CONTROL, "Before task loop, check initialization has been done", 0U, 0U);
    }

    while (RetVal == SVC_OK) {
        if (0U != AmbaWrap_memset(CmdName, 0, sizeof(CmdName))) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "memset CmdName get failed", 0U, 0U);
        }
        SvcLog_OK(SVC_LOG_FLOW_CONTROL, "FlowControl_Entry waiting command", 0U, 0U);
        RetVal = AmbaKAL_MsgQueueReceive(&SvcFlowControlQueue, CmdName, AMBA_KAL_WAIT_FOREVER);
        AmbaPrint_PrintStr5("FlowControl_Entry command: %s", CmdName, NULL, NULL, NULL, NULL);
        if (SVC_OK !=AmbaWrap_memcpy(FlowControlCmdHistory.Cmds[FlowControlCmdHistory.WrIdx], CmdName, sizeof(CmdName))) {
            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "LastCmd get failed", 0U, 0U);
        }
        /* Record the flow control cmd history */
        FlowControlCmdHistory.WrIdx = (FlowControlCmdHistory.WrIdx + 1U) % FLOW_CONTROL_CMD_HISTORY_NUM;
        if (FlowControlCmdHistory.WrIdx == FlowControlCmdHistory.RdIdx) {
            FlowControlCmdHistory.RdIdx = (FlowControlCmdHistory.RdIdx + 1U) % FLOW_CONTROL_CMD_HISTORY_NUM;
        }

        if (RetVal == SVC_OK) {
            UINT32 CmdExec = 0U;
            for (i = 0; i < FLOW_CONTROL_LIST_SIZE_MAX; i++) {
                /* Get hooked command list */
                const SVC_FLOW_CONTROL_CMD_LIST_s *pList = pSvcFlowControlCmdListLinker[i].pList;
                UINT8 ListCmdNum = pSvcFlowControlCmdListLinker[i].ListCmdNum;

                for (j = 0; j < ListCmdNum; j++) {
                    if (SvcWrap_strcmp(pList[j].CmdName, CmdName) == 0) {
                        RetVal = pList[j].pFuncEntry();
                        if (SVC_OK != RetVal) {
                            SvcLog_NG(SVC_LOG_FLOW_CONTROL, "Command Exec failed with %d", RetVal, 0U);
                            RetVal = SVC_OK;
                        }
                        CmdExec = 1U;
                        break;
                    }
                }
            }
            if (CmdExec == 0U) {
                for (i = 0; i < FLOW_CONTROL_LIST_SIZE_MAX; i++) {
                    if (pSvcFlowControlCmdListLinker[i].pUsage != NULL) {
                        pSvcFlowControlCmdListLinker[i].pUsage();
                    }
                }
            }
        }
    }

    return NULL;
}

static void FlowControl_CmdEntry(UINT32 ArgCount, char * const *pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 RetVal;

    if (1U < ArgCount) {
        if (0 == SvcWrap_strcmp("_last_cmd", pArgVector[1U])) {
            UINT8 RdIdx, WrIdx;

            RdIdx = FlowControlCmdHistory.RdIdx;
            WrIdx = FlowControlCmdHistory.WrIdx;

            AmbaPrint_PrintStr5("[FLOW_CTRL|DBG] FlowControl_Entry last cmd\n", "", "", "", "", "");
            while (RdIdx != WrIdx) {
                AmbaPrint_PrintStr5("[FLOW_CTRL|DBG] \"%s\"", FlowControlCmdHistory.Cmds[RdIdx], "", "", "", "");
                RdIdx = (RdIdx + 1U) % FLOW_CONTROL_CMD_HISTORY_NUM;
            }
            RetVal = SVC_OK;
        } else {
            PrintFunc("svc_flow exec ...\n");
            RetVal = SvcFlowControl_Exec(pArgVector[1U]);
        }
    } else {
        RetVal = SVC_NG;
    }

    if (SVC_OK != RetVal) {
        SvcLog_NG(SVC_LOG_FLOW_CONTROL, "FlowControl_CmdEntry() failed with %d", RetVal, 0U);
    }

}
