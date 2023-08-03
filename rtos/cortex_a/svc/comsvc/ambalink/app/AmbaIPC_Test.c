/**
 * @file AmbaIPC_Test.c
 *
 * Copyright (c) 2020 Ambarella International LP
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
 * @details Test/Reference code for AmbaIPC
 *
 */

#include "AmbaIntrinsics.h"
#include "AmbaKAL.h"
#include "AmbaLink.h"
#include "AmbaIPC_Lock.h"
#include "AmbaPrint.h"
#include "AmbaSvcWrap.h"
#include "AmbaNVM_Partition.h"
#include "AmbaUtility.h"
#include "AmbaIOUtility.h"
#include "AmbaLinkPrivate.h"

//#pragma GCC optimize ("O0")

#if defined(CONFIG_AMBALINK_RPMSG_G1) && defined(CONFIG_AMBALINK_BOOT_OS)
#include "AmbaSYS.h"
#include "AmbaWrap.h"
#include "AmbaIPC_List.h"
#include "AmbaIPC_Rpc.h"
#include "AmbaIPC_RpcProg_R_Test.h"
#include "AmbaIPC_RpcProg_LU_Test.h"
#include "AmbaIPC_RpcProg_LU_Util.h"
#include "AmbaIPC_RpcProg_LU_PT.h"
#include "AmbaIPC_RpcProg_R_PT.h"
#include "AmbaShell_Commands.h"
#include "AmbaUART.h"
#include "AmbaIPC_Rfs.h"
#include "AmbaIPC_Clock.h"
#include "AmbaIPC_SD.h"
#include <AmbaIPC_LinkCtrl.h>
#endif  //#if defined(CONFIG_AMBALINK_RPMSG_G1) && defined(CONFIG_AMBALINK_BOOT_OS)

#define ECHO_MAX_LENGTH (100u)

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_MsgHandler
 *
 *  @Description::  Channel Message Handler
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static INT32 AmbaIPC_MsgHandler(AMBA_IPC_HANDLE IpcHandle, AMBA_IPC_MSG_CTRL_s *pMsgCtrl)
{
    char * str;

#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TypeCast(&str, &pMsgCtrl->pMsgData);
    AmbaMisra_TouchUnused(IpcHandle);
#else
    str = (char *)pMsgCtrl->pMsgData;
#endif
    /* Prevent garbage data (buffer overflow) */
    if (pMsgCtrl->Length < ECHO_MAX_LENGTH) {
        str[pMsgCtrl->Length] = '\0';
    } else {
        str[ECHO_MAX_LENGTH] = '\0';
    }
    (void)IpcHandle;
    /* [MISRA Rule 8.13] could be declared as pointing to const */
    (void)pMsgCtrl;

    //(void)str;
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "recv message: [%s]", str, NULL, NULL, NULL, NULL);
    //AmbaPrint_Flush();
    //(void) AmbaKAL_TaskSleep(5000);
#ifdef CONFIG_AMBALINK_RPMSG_G1
    {
        /* application callback */
        extern void AmbaLink_EchoCh_Callback(char *Msg) __attribute__((weak));

        if (AmbaLink_EchoCh_Callback != NULL) {
            AmbaLink_EchoCh_Callback((char *)pMsgCtrl->pMsgData);
        }
    }
#endif

    return 0;
}

#if defined(CONFIG_AMBALINK_RPMSG_G1) && defined(CONFIG_AMBALINK_BOOT_OS)
static char PT_USB;
#ifdef RPC_DEBUG
extern void CalcRpcProfile(void);
extern void AmbaIPC_InitProfile(void);
#endif
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Print_Svc
 *
 *  @Description::  test case 1 for RPC
 *
 *  @Return     ::
 *      void :
 *
\*----------------------------------------------------------------------------*/
void AmbaRpcProg_R_Test_Print_Svc(const char *msg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "\t[svc echo] %s\n", (char *)msg, NULL, NULL, NULL, NULL);
    AmbaPrint_Flush();
    pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaRpcProg_PT_Svc
 *
 *  @Description:: get async data from linux pseudo terminal client
 *
 *  @Return     ::
 *      void :
 *
\*----------------------------------------------------------------------------*/
void AmbaRpcProg_R_PT_Svc(const char *msg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    UINT32 SentSize;

#if 0
    /* send to linux pseudo terminal */
    if(PT_USB)
        AppCdcAcmMultid_USBWrite(1, IO_UtilityStringLength(msg) + 1U, (char *)msg);
    else
#endif
    (void) AmbaUART_Write(0U, 0U, IO_UtilityStringLength(msg) + 1U, (UINT8 *)msg, &SentSize, 1U);
    pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;

}

/*----------------------------------------------------------------------------*\
 *  @RoutineName:: AmbaRpcProg_PT_Clnt
 *
 *  @Description:: send async data to linux pseudo terminal server
 *
 *  @Return     ::
 *      INT32 :
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_PT_Clnt(const char *pStr, INT32 *pResult,
                                                CLIENT_ID_t Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;

    (void)(pResult);
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_PT,
                                (void *) pStr, IO_UtilityStringLength(pStr)+1, NULL, 0, 0);
    return status;
}

/* called by USB cdc acm driver for linux pseudo terminal */
INT32 AmbaRpcProg_USB_PT_Clnt(UINT32 id, void *buf, UINT32 len)
{
    CLIENT_ID_t clnt;

    (void)(id);
    if(1 == len) {
        ((char*)buf)[len] = '\0';
    }
    clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX,
                                AMBA_RPC_PROG_LU_PT_PROG_ID,
                                AMBA_RPC_PROG_LU_PT_VER);
    if (clnt == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Client creation failed\n", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return -1;
    }
    PT_USB = 1;

    AmbaRpcProg_LU_PT_Clnt(buf, NULL, clnt);
    AmbaIPC_ClientDestroy(clnt);

    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Sum_Svc
 *
 *  @Description::  test case 2 for RPC
 *
 *  @Return     ::
 *      INT32 :
 *
\*----------------------------------------------------------------------------*/
void AmbaRpcProg_R_Test_Sum_Svc(AMBA_RPC_PROG_R_TEST_SUM_ARG_s *pArg,
                                AMBA_IPC_SVC_RESULT_s *ret)
{
    INT32 sum;

    if (pArg->a >= 256 || pArg->b >= 256) {
        ret->Status = AMBA_IPC_REPLY_PARA_INVALID;
    } else {
        sum = pArg->a + pArg->b;
        AmbaWrap_memcpy(ret->pResult, &sum, 4U);
        ret->Length = 4U;
        ret->Status = AMBA_IPC_REPLY_SUCCESS;
    }
    ret->Mode = AMBA_IPC_SYNCHRONOUS;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Print_Clnt
 *
 *  @Description::  test case 1 for RPC client
 *
 *  @Return     ::
 *      INT32 :
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_Test_Print_Clnt(const char *pStr,
                                                        INT32 *pResult,
                                                        CLIENT_ID_t Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;

    (void)(pResult);
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_TEST_PRINT,
                                (void *) pStr, IO_UtilityStringLength(pStr) + 1, NULL, 0, 0);
    return status;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaRpcProg_Test_Sum_Clnt
 *
 *  @Description::  test case 2 for RPC client
 *
 *  @Return     ::
 *      INT32 :
 *
\*----------------------------------------------------------------------------*/
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_LU_Test_Sum_Clnt(AMBA_RPC_PROG_LU_TEST_SUM_ARG_s *pArg,
                                                        INT32 *Result,
                                                        CLIENT_ID_t Clnt)
{
    AMBA_IPC_REPLY_STATUS_e status;
    status = AmbaIPC_ClientCall(Clnt, AMBA_RPC_PROG_LU_TEST_SUM, pArg,
                                sizeof(AMBA_RPC_PROG_LU_TEST_SUM_ARG_s),
                                Result, 4, 1000);
    return status;
}

static void IpcTestRpcUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("supported test commands\n");
    PrintFunc("    rpc list:    list registered program\n");
    PrintFunc("    rpc svc [prog_name] +: register program\n");
    PrintFunc("    rpc svc [prog_name] -: unregister program\n");
    PrintFunc("             supported programs: test (test rpc program)\n");
    PrintFunc("                                 exec (execute command in Linux)\n");
    PrintFunc("                                 pt (execute command in Linux pseudo terminal)\n");
    PrintFunc("    rpc clnt test 1 str: echo [str]\n");
    PrintFunc("    rpc clnt test 2 a: calulate a+a\n");
    PrintFunc("    rpc clnt test stress times: create 2 clients and do consecutive rpc calls at once.\n");
    PrintFunc("                                user can specify the cycles.\n");
    PrintFunc("    rpc clnt exec1 [command] (execute command in Linux and return the result)\n");
    PrintFunc("    rpc clnt exec2 [command] (execute command in Linux)\n");
    PrintFunc("    rpc clnt pt [command] (execute command in Linux pseudo terminal)\n");
}

#define FLAG_CLNT_START     1
#define FLAG_CLNT_END       2

#define CLNT_NUM            2
typedef struct _TEST_CLNT_ARG_s_ {
    INT32 id;
    AMBA_KAL_EVENT_FLAG_t flag;
    AMBA_SHELL_PRINT_f PrintFunc;
} TEST_CLNT_ARG_s;

INT32 stress_times;
INT32 clnt_create = 0;
AMBA_KAL_TASK_t clnt_task[CLNT_NUM];
TEST_CLNT_ARG_s clnt_arg[CLNT_NUM];
char clnt_stack[CLNT_NUM][0x1000];
static void *IpcStressClient(void *arg)
{
    TEST_CLNT_ARG_s *clnt_arg = (TEST_CLNT_ARG_s *)arg;
    INT32 i, j, id, ret, status;
    CLIENT_ID_t clnt;
    AMBA_RPC_PROG_LU_TEST_SUM_ARG_s sum;
    char string[32];
    UINT32 actFlags;

    id = clnt_arg->id;
    AmbaUtility_StringAppendUInt32(string, sizeof(string), id, 10U);
    while(1) {
        status = AmbaKAL_EventFlagGet(
                &clnt_arg->flag,
                FLAG_CLNT_START,
                1U, 1U,
                &actFlags,
                AMBA_KAL_WAIT_FOREVER);
        if( status != TX_SUCCESS ){
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Event Flag Take Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

        for(i=0; i<stress_times; i++) {
            clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX,
                                            AMBA_RPC_PROG_LU_TEST_PROG_ID,
                                            AMBA_RPC_PROG_LU_TEST_VER);

            if(clnt == NULL) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s client create failed\n", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_Flush();
                return NULL;
            }
            AmbaRpcProg_LU_Test_Print_Clnt(string, NULL, clnt);
            for(j=0; j<32; j++) {
                sum.a = sum.b = j;
                AmbaRpcProg_LU_Test_Sum_Clnt(&sum, &ret, clnt);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\treturn value is %d\n", ret, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            AmbaIPC_ClientDestroy(clnt);
        }
        status = AmbaKAL_EventFlagSet(&clnt_arg->flag, FLAG_CLNT_END);
        if( status != TX_SUCCESS ){
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Give Clnt End Flag Error: status is 0x%x", status, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }
    }
    return NULL;
}

static INT32 IpcTestClient(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    INT32 ret;
    CLIENT_ID_t clnt;
    AMBA_RPC_PROG_LU_TEST_SUM_ARG_s sum;

    (void)(argc);
    (void)(PrintFunc);
    clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX,
                                AMBA_RPC_PROG_LU_TEST_PROG_ID,
                                AMBA_RPC_PROG_LU_TEST_VER);

    if(clnt == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "client create failed\n", NULL, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return 0;
    }

    switch (argv[3][0]) {
        case '1':
            AmbaRpcProg_LU_Test_Print_Clnt(argv[4], NULL, clnt);
            break;

        case '2':
            AmbaUtility_StringToUInt32(argv[4], (UINT32 *)&(sum.a));
            sum.b = sum.a;
            AmbaRpcProg_LU_Test_Sum_Clnt(&sum, &ret, clnt);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\treturn value is %d\n", ret, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
        default:
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\tunknown case: %d\n", argv[3][0], 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            break;
    }

    AmbaIPC_ClientDestroy(clnt);
    return 1;
}

static INT32 ProcessTestClient(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    INT32 priority = 150, i, errno;
    char name[32];
    UINT32 actFlags;

    /* t ipc clnt test stress 100 */
    if (0 == AmbaUtility_StringCompare(argv[3], "stress", AmbaUtility_StringLength("stress"))) {
        AmbaUtility_StringToUInt32(argv[4], (UINT32 *)&stress_times);
    } else {
        /* normal rpc clnt test */
        IpcTestClient(argc, argv, PrintFunc);
    }

    if (0 == AmbaUtility_StringCompare(argv[3], "stress", AmbaUtility_StringLength("stress"))) {
        for(i=0; i<CLNT_NUM; i++) {
            clnt_arg[i].id = i;
            clnt_arg[i].PrintFunc = PrintFunc;
            AmbaUtility_StringAppendUInt32(name, sizeof(name), i, 10U);
            errno = AmbaKAL_TaskCreate(&clnt_task[i], name, priority + 2*i,
                    IpcStressClient, (void *)&clnt_arg[i],
                    (void *) &clnt_stack[i][0], 0x1000, 0U);
            if(errno) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "task created failed (%d)", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            errno = AmbaKAL_TaskSetSmpAffinity(&clnt_task[i], (0x1) & AMBA_KAL_CPU_CORE_MASK);
            if (errno) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskSetSmpAffinity is failed! (0x%x)", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            errno = AmbaKAL_EventFlagCreate(&clnt_arg[i].flag,"TestClient");
            if(errno) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "event flag created failed (%d)", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            errno = AmbaKAL_TaskResume(&clnt_task[i]);
            if (errno) {
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "AmbaKAL_TaskResume is failed! (%d)", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            errno = AmbaKAL_EventFlagSet(&clnt_arg[i].flag, FLAG_CLNT_START);
            if( errno != TX_SUCCESS ){
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Set Clnt Start Flag Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
        }

        for(i=0; i<CLNT_NUM; i++) {
            errno = AmbaKAL_EventFlagGet(
                &clnt_arg[i].flag,
                FLAG_CLNT_END,
                1U, 1U,
                &actFlags,
                AMBA_KAL_WAIT_FOREVER);
            if( errno != TX_SUCCESS ){
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Event Flag Take Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            errno = AmbaKAL_TaskTerminate(&clnt_task[i]);
            if( errno != TX_SUCCESS ){
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Terminate Task Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            errno = AmbaKAL_TaskDelete(&clnt_task[i]);
            if( errno != TX_SUCCESS ){
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: ", __func__, NULL, NULL, NULL, NULL);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "Task Delete Error: status is 0x%x", errno, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
            AmbaKAL_EventFlagDelete(&clnt_arg[i].flag);
        }
    }
    return 0;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestRpc
 *
 *  @Description::  test case for RPC
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
#define USER_PROC_INFO 1024
static UINT8 RpcTestStack[0x4000] GNU_SECTION_NOZEROINIT;
static UINT8 RpcPtStack[0x4000] GNU_SECTION_NOZEROINIT;

static void IpcTestRpc(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    AMBA_RPC_PROG_EXEC_ARG_s *pExec = NULL;
    INT32 ret, i, rem_len;
    CLIENT_ID_t clnt = NULL;
    char output[2048];
    char buffer[1024];
    AMBA_IPC_PROG_INFO_s prog_info[1];
    extern void AmbaIPC_ListSvc(AMBA_SHELL_PRINT_f PrintFunc);
    UINT32 ProcInfo[USER_PROC_INFO];
    UINT8 ExecBuf[256];

    if (argc == 1U) {
        IpcTestRpcUsage(PrintFunc);
        return;
    }

    if (0 == AmbaUtility_StringCompare(argv[1], "list", AmbaUtility_StringLength("list"))) {
        AmbaIPC_ListSvc(PrintFunc);
    } else if (0 == AmbaUtility_StringCompare(argv[1], "svc", AmbaUtility_StringLength("svc"))) {
        if (argc != 4) {
            IpcTestRpcUsage(PrintFunc);
            return;
        }

        if (0 == AmbaUtility_StringCompare(argv[2], "test", AmbaUtility_StringLength("test"))) {
            if (argv[3][0] == '+') {
                prog_info->ProcNum = 2;
                if( (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)) > USER_PROC_INFO) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "(prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s))(%d) is big than %d\n", (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)), USER_PROC_INFO, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                    return;
                }
                AmbaWrap_memset(ProcInfo, 0 ,sizeof(ProcInfo));
                prog_info->pProcInfo = (AMBA_IPC_PROC_s *)ProcInfo;
                prog_info->pProcInfo[0].Mode = AMBA_IPC_ASYNCHRONOUS;
                prog_info->pProcInfo[1].Mode = AMBA_IPC_SYNCHRONOUS;
                prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_R_Test_Print_Svc;
                prog_info->pProcInfo[1].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_R_Test_Sum_Svc;
                AmbaIPC_SvcRegister(AMBA_RPC_PROG_R_TEST_PROG_ID, AMBA_RPC_PROG_R_TEST_VER, "test_rpc_svc",
                                    65, (void *)&RpcTestStack[0], 0x4000, prog_info, 1);
            } else {
                AmbaIPC_SvcUnregister(AMBA_RPC_PROG_R_TEST_PROG_ID, AMBA_RPC_PROG_R_TEST_VER);
                // should free @stack in real implemenation
            }
        }

        if (0 == AmbaUtility_StringCompare(argv[2], "pt", AmbaUtility_StringLength("pt"))) {
            if (argv[3][0] == '+') {
                prog_info->ProcNum = 1;
                if( (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)) > USER_PROC_INFO) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "(prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s))(%d) is big than %d\n", (prog_info->ProcNum * sizeof(AMBA_IPC_PROC_s)), USER_PROC_INFO, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                    return;
                }
                AmbaWrap_memset(ProcInfo, 0 ,sizeof(ProcInfo));
                prog_info->pProcInfo = (AMBA_IPC_PROC_s *)ProcInfo;
                prog_info->pProcInfo[0].Mode = AMBA_IPC_ASYNCHRONOUS;
                prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_R_PT_Svc;
                AmbaIPC_SvcRegister(AMBA_RPC_PROG_R_PT_PROG_ID, AMBA_RPC_PROG_R_PT_VER, "pt_rpc_svc",
                                    65, (void *)&RpcPtStack[0], 0x4000, prog_info, 1);
            } else {
                AmbaIPC_SvcUnregister(AMBA_RPC_PROG_R_PT_PROG_ID, AMBA_RPC_PROG_R_PT_VER);
                // should free @stack in real implemenation
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[1], "clnt", AmbaUtility_StringLength("clnt"))) {
        if (argc < 3) {
            IpcTestRpcUsage(PrintFunc);
            return;
        }

        if (0 == AmbaUtility_StringCompare(argv[2], "test", AmbaUtility_StringLength("test"))) {
            ProcessTestClient(argc, argv, PrintFunc);
            return;
        } else if (0 == AmbaUtility_StringCompare(argv[2], "exec1", AmbaUtility_StringLength("exec1")) ||
                   0 == AmbaUtility_StringCompare(argv[2], "exec2", AmbaUtility_StringLength("exec2")) ) {
            if (argc < 4) {
                IpcTestRpcUsage(PrintFunc);
                return;
            }
            clnt = AmbaIPC_ClientCreate(AMBA_RPC_PROG_LU_UTIL_HOST,
                                        AMBA_RPC_PROG_LU_UTIL_PROG_ID,
                                        AMBA_RPC_PROG_LU_UTIL_VER);
            if( sizeof(AMBA_RPC_PROG_EXEC_ARG_s) + 100 * sizeof(char) > 256) {
                    AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "sizeof(AMBA_RPC_PROG_EXEC_ARG_s) + 100 * sizeof(char)(%d) is big than %d\n", sizeof(AMBA_RPC_PROG_EXEC_ARG_s) + 100 * sizeof(char), 256, 0U, 0U, 0U);
                    AmbaPrint_Flush();
                    return;
            }
            AmbaWrap_memset(ExecBuf, 0, sizeof(ExecBuf));
            pExec = (AMBA_RPC_PROG_EXEC_ARG_s *)ExecBuf;
            AmbaWrap_memset(output, 0, EXEC_OUTPUT_SIZE);
            IO_UtilityStringCopy(pExec->command, sizeof(ExecBuf), argv[3]);
            /* Concatenate the arguments of the command from the input*/
            for (i = 4; i < (INT32)argc; i++) {
                IO_UtilityStringAppend(pExec->command, sizeof(ExecBuf), " ");
                IO_UtilityStringAppend(pExec->command, sizeof(ExecBuf), argv[i]);
            }
        } else if (0 == AmbaUtility_StringCompare(argv[2], "pt", AmbaUtility_StringLength("pt"))) {
            if (argc < 4) {
                IpcTestRpcUsage(PrintFunc);
                return;
            }
           clnt = AmbaIPC_ClientCreate(AMBA_IPC_HOST_LINUX,
                                       AMBA_RPC_PROG_LU_PT_PROG_ID,
                                       AMBA_RPC_PROG_LU_PT_VER);
        } else {
            IpcTestRpcUsage(PrintFunc);
            return;
        }

        if (clnt == NULL) {
            AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "Client creation failed\n", NULL, NULL, NULL, NULL, NULL);
            AmbaPrint_Flush();
            return;
        }

        if (0 == AmbaUtility_StringCompare(argv[2], "exec1", AmbaUtility_StringLength("exec1"))) {
            ret = AmbaRpcProg_Util_Exec1_Clnt(pExec, (INT32 *)output, clnt);
            if (ret != 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\tExecute exec1 failed: status %d\n", ret, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
                return;
            }

            /* Note: The string length is limited to 1024 in AmbaShell_Print */
            rem_len = IO_UtilityStringLength(output) + 1;
            i = 0;
            while ( rem_len > 0 ) {
                if (rem_len > 1024) {
                    AmbaWrap_memcpy(buffer, &output[i], 1023);
                    buffer[1023] = '\0';
                } else {
                    AmbaWrap_memcpy(buffer, &output[i], rem_len);
                }
                AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s", buffer, NULL, NULL, NULL, NULL);
                rem_len -= 1023;
                i += 1023;
            }
        } else if (0 == AmbaUtility_StringCompare(argv[2], "exec2", AmbaUtility_StringLength("exec2"))) {
            ret = AmbaRpcProg_Util_Exec2_Clnt(pExec, (INT32 *)output, clnt);
            if (ret != 0) {
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "\tExecute exec1 failed: status %d\n", ret, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
        } else if (0 == AmbaUtility_StringCompare(argv[2], "pt", AmbaUtility_StringLength("pt"))) {
            IO_UtilityStringCopy(buffer, sizeof(buffer), argv[3]);
            for (i = 4; i < (INT32)argc; i++) {
                IO_UtilityStringAppend(buffer, sizeof(buffer), " ");
                IO_UtilityStringAppend(buffer, sizeof(buffer), argv[i]);
            }

            IO_UtilityStringAppend(buffer, sizeof(buffer), "\n");
            PT_USB = 0;
            AmbaRpcProg_LU_PT_Clnt(buffer, NULL, clnt);
        }
        AmbaIPC_ClientDestroy(clnt);
    } else if (0 == AmbaUtility_StringCompare(argv[1], "profile", AmbaUtility_StringLength("profile"))) {
#ifndef RPC_DEBUG
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "please turn on RPC_DEBUG and recompile\n", 0U, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
            return;
#else
        if (0 == AmbaUtility_StringCompare(argv[2], "start", AmbaUtility_StringLength("start"))) {
            AmbaIPC_InitProfile();  // clear the rpc profile
        } else if (0 == AmbaUtility_StringCompare(argv[2], "stop", AmbaUtility_StringLength("stop"))) {
            CalcRpcProfile();
        }
#endif
    } else {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "unknown  command %s\n", argv[1], NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
    }

    return;
}

/*============================================================================*\
 *                                                                            *
 *                              TEST CODE for RPMSG                           *
 *                                                                            *
\*============================================================================*/
#ifdef RPC_DEBUG
extern void InitRpmsgProfile(void);
#endif
#ifdef RPMSG_DEBUG
extern void CalcRpmsgProfile(void);
#endif

#if 0
static void ShowRFSUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("\n");
    PrintFunc("rpmsg commands for AmbaRFS:\n");
    PrintFunc("Please Note, you cannot use AmbaRFS commands with AmbaVFS driver!\n");
    PrintFunc("    rpmsg rfs start\n");
    PrintFunc("    rpmsg rfs read\n");
    PrintFunc("    rpmsg rfs write\n");
    PrintFunc("    rpmsg rfs fsync\n");
    PrintFunc("    rpmsg rfs size\n");
    PrintFunc("    rpmsg rfs stat\n");
    PrintFunc("    rpmsg rfs getdev\n");
    PrintFunc("    rpmsg rfs move [old_name] [new_name]\n");
    PrintFunc("    rpmsg rfs remove [fname]\n");
    PrintFunc("    rpmsg rfs chmod [fname] [mode]; mode: bitwise value. ex: 0x1ff for rwxrwxrwx.\n");
    PrintFunc("    rpmsg rfs mkdir [dname]\n");
    PrintFunc("    rpmsg rfs rmdir [dname]\n");
    PrintFunc("    rpmsg rfs chdmod [dname] [mode]; mode: bitwise value. ex: 0x1ff for rwxrwxrwx.\n");
    PrintFunc("    rpmsg rfs mount [dev_name] [dir_name] [type]\n");
    PrintFunc("    rpmsg rfs umount [dir_name]\n");
    PrintFunc("    rpmsg rfs sync\n");
    PrintFunc("    rpmsg rfs dirlist [path]\n");
}

static INT32 rfs_enabled = 0;
static void RFS_Test(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    INT32 i, res;

    if (rfs_enabled == 0) {
        if (0 == AmbaUtility_StringCompare(argv[2], "start", AmbaUtility_StringLength("start"))) {
            extern void AmbaIPC_RfsInit(void);
            AmbaIPC_RfsInit();
            rfs_enabled = 1;
        } else {
            PrintFunc("AmbaRFS does not start yet!\n");
            PrintFunc("Please do 'rfs start' first!\n");
        }
        return;
    }

    if (0 == AmbaUtility_StringCompare(argv[2], "read", AmbaUtility_StringLength("read"))) {
        UINT64 fptr;

        if (argc > 3) {
            PrintFunc("Try to read ");
            PrintFunc(argv[3]);
            PrintFunc("\n");
            fptr = AmbaIPC_fopen(argv[3], "rb");
        } else {
            fptr = AmbaIPC_fopen("/etc/init.d/S99bootdone", "rb");
        }

        if (fptr == 0UL) {
            PrintFunc("Fail to do AmbaIPC_fopen()!\n");
        } else {
            char buf[4096] = {0};

            while(AmbaIPC_feof(fptr) != 0) {
                res = AmbaIPC_fread(buf, 4096, fptr);
                if (res <= 0) {
                    AmbaPrint_PrintInt5("Fail to do AmbaIPC_fread(). err=%d\n", res,0,0,0,0);
                    break;
                } else {
                    AmbaPrint_PrintInt5("AmbaIPC_fread() read %d bytes\n", res,0,0,0,0);
                    AmbaPrint_Flush();
                    PrintFunc(buf);
                    //AmbaPrint_PrintInt5("%02x %02x %02x %02x %02x", buf[0],buf[1],buf[2],buf[3],buf[4]);
                    //AmbaPrint_PrintInt5("%02x %02x %02x %02x %02x", buf[5],buf[6],buf[7],buf[8],buf[9]);
                    //AmbaPrint_PrintInt5("%02x %02x %02x %02x %02x", buf[10],buf[11],buf[12],buf[13],buf[14]);
                    //AmbaPrint_PrintInt5("%02x %02x %02x %02x %02x", buf[15],buf[16],buf[17],buf[18],buf[19]);
                }
            }
            PrintFunc("\n");

            res = AmbaIPC_fclose(fptr);
            if (res < 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fclose(). err=%d\n", res,0,0,0,0);
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "write", AmbaUtility_StringLength("write"))) {
        UINT64 fptr;

        if (argc > 3) {
            fptr = AmbaIPC_fopen(argv[3], "w+");
        } else {
            fptr = AmbaIPC_fopen("/tmp/rfs_test.txt", "w+");
        }

        if (fptr == 0UL) {
            PrintFunc("Fail to do AmbaIPC_fopen()!\n");
        } else {
            char buf[] = "This is RFS test from RTOS\n\n";

            res = AmbaIPC_fwrite(buf, IO_UtilityStringLength(buf), fptr);
            if (res <= 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fwrite(). err=%d\n", res,0,0,0,0);
            } else {
                AmbaPrint_PrintInt5("AmbaIPC_fwrite() write %d bytes\n", res,0,0,0,0);
            }

            res = AmbaIPC_fclose(fptr);
            if (res < 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fclose(). err=%d\n", res,0,0,0,0);
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "fsync", AmbaUtility_StringLength("fsync"))) {
        UINT64 fptr;

        fptr = AmbaIPC_fopen("/tmp/test_sync.txt", "w+");
        if (fptr == 0UL) {
            PrintFunc("Fail to do AmbaIPC_fopen()!\n");
        } else {
            char buf[64] = "This is RFS test from RTOS\n\n";

            res = AmbaIPC_fwrite(buf, IO_UtilityStringLength(buf), fptr);
            if (res <= 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fwrite(). err=%d\n", res,0,0,0,0);
            } else {
                AmbaPrint_PrintInt5("AmbaIPC_fwrite() write %d bytes\n", res,0,0,0,0);

                for(i=0; i<5; i++) {
                    AmbaKAL_TaskSleep(1000);
                    PrintFunc(".");
                }
                PrintFunc("\n");

                PrintFunc("doing AmbaIPC_fsync()\n");
                res = AmbaIPC_fsync(fptr);
                if (res < 0) {
                    AmbaPrint_PrintInt5("Fail to do AmbaIPC_fsync(). err=%d\n", res,0,0,0,0);
                }

                for(i=0; i<5; i++) {
                    AmbaKAL_TaskSleep(1000);
                    PrintFunc(".");
                }
                PrintFunc("\n");

                IO_UtilityStringCopy(buf, sizeof(buf), "After fsync...\n");
                res = AmbaIPC_fwrite(buf, IO_UtilityStringLength(buf), fptr);
                if (res < 0) {
                    AmbaPrint_PrintInt5("Fail to do AmbaIPC_fwrite(). err=%d\n", res,0,0,0,0);
                }
            }

            res = AmbaIPC_fclose(fptr);
            if (res < 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fclose(). err=%d\n", res,0,0,0,0);
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "size", AmbaUtility_StringLength("size"))) {
        UINT64 fptr;

        if (argc > 3) {
            fptr = AmbaIPC_fopen(argv[3], "rb");
        } else {
            fptr = AmbaIPC_fopen("/etc/init.d/S99bootdone", "rb");
        }
        if (fptr == 0UL) {
            PrintFunc("Fail to do AmbaIPC_fopen()!\n");
        } else {
            INT64 FileSize;

            res = AmbaIPC_fseek(fptr,0UL,AMBA_IPC_RFS_SEEK_END);
            if (res < 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fseek(END). err=%d\n", res,0,0,0,0);
            } else {
                FileSize = AmbaIPC_ftell(fptr);
                if (FileSize < 0) {
                    AmbaPrint_PrintInt5("Fail to do AmbaIPC_ftell(). err=%d\n", FileSize,0,0,0,0);
                } else {
                    AmbaPrint_PrintInt5("size: %lld\n",FileSize,0,0,0,0);
                    AmbaPrint_Flush();
                    res = AmbaIPC_fseek(fptr,0UL,AMBA_IPC_RFS_SEEK_SET);
                    if (res < 0) {
                        AmbaPrint_PrintInt5("Fail to do AmbaIPC_fseek(SET). err=%d\n", res,0,0,0,0);
                    }
                }
            }

            res = AmbaIPC_fclose(fptr);
            if (res < 0) {
                AmbaPrint_PrintInt5("Fail to do AmbaIPC_fclose(). err=%d\n", res,0,0,0,0);
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "stat", AmbaUtility_StringLength("stat"))) {
        AMBA_IPC_RFS_STAT_s StatRes;

        if (argc > 3) {
            res = AmbaIPC_stat(argv[3], &StatRes);
        } else {
            res = AmbaIPC_stat("/etc/init.d/S99bootdone", &StatRes);
        }
        if (res < 0) {
            PrintFunc("Fail to do AmbaIPC_stat()!\n");
        } else {
            AmbaPrint_PrintUInt5("ino:%llu, dev:%u, mode:%u, nlink:%u, rdev:%u",StatRes.ino,StatRes.dev,StatRes.mode,StatRes.nlink,StatRes.rdev);
            AmbaPrint_PrintUInt5("gid:%u, uid:%u, size:%lld, blksize:%u, blocks:%llu",StatRes.gid,StatRes.uid,StatRes.size,StatRes.blksize,StatRes.blocks);
            AmbaPrint_PrintUInt5("accessed time:%d.%d",StatRes.atime_sec,StatRes.atime_nsec,0,0,0);
            AmbaPrint_PrintUInt5("modified time:%d.%d",StatRes.mtime_sec,StatRes.mtime_nsec,0,0,0);
            AmbaPrint_PrintUInt5("created time :%d.%d",StatRes.ctime_sec,StatRes.ctime_nsec,0,0,0);
            AmbaPrint_Flush();
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "getdev", AmbaUtility_StringLength("getdev"))) {
        AMBA_IPC_RFS_DEVINF_s DevRes;

        if (argc > 3) {
            res = AmbaIPC_getdev(argv[3], &DevRes);
        } else {
            res = AmbaIPC_getdev("/tmp", &DevRes);
        }
        if (res < 0) {
            PrintFunc("Fail to do AmbaIPC_getdev()!\n");
        } else {
            UINT64 freespace;
            char str_freespace[32] = {0};

            AmbaPrint_PrintUInt5("cls:%u, ecl:%u, bps:%u, spc:%u",DevRes.cls,DevRes.ecl,DevRes.bps,DevRes.spc,0);
            AmbaPrint_PrintUInt5("cpg:%u, ecg:%u, fmt:%u",DevRes.cpg,DevRes.ecg,DevRes.fmt,0,0);
            AmbaPrint_Flush();

            freespace = DevRes.spc * DevRes.bps;
            freespace = freespace * DevRes.ecl;
            AmbaUtility_UInt64ToStr(str_freespace,32,freespace,10);

            PrintFunc("free space for '");
            if (argc > 3) {
                PrintFunc(argv[3]);
            } else {
                PrintFunc("/tmp");
            }
            PrintFunc("' is ");
            PrintFunc(str_freespace);
            PrintFunc(" bytes.\n");
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "move", AmbaUtility_StringLength("move"))) {
        if (argc < 4) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaIPC_move(argv[3], argv[4]);
            if (res < 0) {
                PrintFunc("Fail to do AmbaIPC_move()!\n");
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "remove", AmbaUtility_StringLength("remove"))) {
        if (argc < 3) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaIPC_remove(argv[3]);
            if (res < 0) {
                PrintFunc("Fail to do AmbaIPC_remove()!\n");
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "chmod", AmbaUtility_StringLength("chmod"))) {
        INT32 t_mod;

        if (argc < 4) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaUtility_StringToUInt32(argv[4], (UINT32 *)&t_mod);
            if(res != 0) {
                PrintFunc("Fail to do AmbaUtility_StringToUInt32()!!\n");
            } else {
                //AmbaPrint_PrintUInt5("mode=%05u",t_mod,0,0,0,0);
                res = AmbaIPC_chmod(argv[3],t_mod);
                if (res < 0) {
                    PrintFunc("Fail to do AmbaIPC_chmod()!\n");
                }
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "mkdir", AmbaUtility_StringLength("mkdir"))) {
        if (argc < 3) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaIPC_mkdir(argv[3]);
            if (res < 0) {
                PrintFunc("Fail to do AmbaIPC_mkdir()!\n");
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "rmdir", AmbaUtility_StringLength("rmdir"))) {
        if (argc < 3) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaIPC_rmdir(argv[3]);
            if (res < 0) {
                PrintFunc("Fail to do AmbaIPC_rmdir()!\n");
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "chdmod", AmbaUtility_StringLength("chdmod"))) {
        INT32 t_mod;

        if (argc < 4) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaUtility_StringToUInt32(argv[4], (UINT32 *)&t_mod);
            if(res != 0) {
                PrintFunc("Fail to do AmbaUtility_StringToUInt32()!!\n");
            } else {
                res = AmbaIPC_chdmod(argv[3],t_mod);
                if (res < 0) {
                    PrintFunc("Fail to do AmbaIPC_chdmod()!\n");
                }
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "mount", AmbaUtility_StringLength("mount"))) {
        if (argc < 5) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaIPC_mount(argv[3], argv[4], argv[5]);
            if (res < 0) {
                PrintFunc("Fail to do AmbaIPC_mount()!\n");
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "umount", AmbaUtility_StringLength("umount"))) {
        if (argc < 3) {
            ShowRFSUsage(PrintFunc);
        } else {
            res = AmbaIPC_umount(argv[3]);
            if (res < 0) {
                PrintFunc("Fail to do AmbaIPC_umount()!\n");
            }
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "sync", AmbaUtility_StringLength("sync"))) {
        res = AmbaIPC_sync();
        if (res < 0) {
            PrintFunc("Fail to do AmbaIPC_sync()!\n");
        }
    } else if (0 == AmbaUtility_StringCompare(argv[2], "dirlist", AmbaUtility_StringLength("dirlist"))) {
        UINT64 dptr;

        if (argc < 3) {
            ShowRFSUsage(PrintFunc);
        } else {
            dptr = AmbaIPC_opendir(argv[3]);
            if (dptr == 0UL) {
                PrintFunc("Fail to do AmbaIPC_opendir()!\n");
            } else {
                AMBA_IPC_RFS_DIRENT_s *dirent;

                for(;;) {
                    dirent = AmbaIPC_readdir(dptr);
                    if (dirent == NULL) {
                        //PrintFunc("Fail to do AmbaIPC_readdir()!\n");
                        break;
                    } else {
                        //AmbaPrint_PrintUInt5("ino=%llu, off=%llu, reclen=%u, type=%u",dirent->ino, dirent->off, dirent->reclen, dirent->type,0);
                        //AmbaPrint_Flush();
                        if (dirent->type == 0) {
                            PrintFunc("[Unknown] ");
                        } else if (dirent->type == 1) {
                            PrintFunc("[FiFo] ");
                        } else if (dirent->type == 2) {
                            PrintFunc("[Char Dev] ");
                        } else if (dirent->type == 4) {
                            PrintFunc("[Dir] ");
                        } else if (dirent->type == 6) {
                            PrintFunc("[Block Dev] ");
                        } else if (dirent->type == 8) {
                            PrintFunc("[File] ");
                        } else if (dirent->type == 10) {
                            PrintFunc("[Sym Link] ");
                        } else if (dirent->type == 12) {
                            PrintFunc("[Socket] ");
                        } else if (dirent->type == 14) {
                            PrintFunc("[WHT] ");
                        } else {
                            PrintFunc("Undefined Type: ");
                        }

                        PrintFunc(dirent->name);
                        PrintFunc("\n");
                    }
                }

                res = AmbaIPC_closedir(dptr);
                if (res < 0) {
                    PrintFunc("Fail to do AmbaIPC_closedir()!\n");
                }
            }
        }
    }
}
#endif  //#if 0

static AMBA_IPC_HANDLE EchoChannel;
#if !defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_TestInit
 *
 *  @Description::  Init test module
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
static INT32 AmbaIPC_TestInit(void)
{
    /*
     * Note: When run as CONFIG_AMBALINK_RPMSG_ROLE_MAST and connect to Linux,
     *       the name is "echo_ca9_b"
     */
#ifdef CONFIG_OPENAMP
    EchoChannel = AmbaIPC_Alloc(RPMSG_DEV_OAMP, "echo_cortex", AmbaIPC_MsgHandler);
#else
    EchoChannel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "echo_cortex", AmbaIPC_MsgHandler);
#endif

    if (EchoChannel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        return -1;
    }

    (void)AmbaIPC_RegisterChannel(EchoChannel, NULL);

    return 0;
}
#endif  //#if !defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)

static void ShowRPMSGTestUsage(AMBA_SHELL_PRINT_f PrintFunc)
{
    PrintFunc("supported rpmsg commands\n");
    PrintFunc("    rpmsg test:    send 8 echo rpmsg continuously\n");
    PrintFunc("    rpmsg test [str] [count] : send number of count echo [str] rpmsg continuously\n");
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestRpmsg
 *
 *  @Description::  test case for a "echo" RPMSG channel
 *
 *  @Return     ::
 *      void :
 *
\*----------------------------------------------------------------------------*/
static void IpcTestRpmsg(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, count = 8U;
    char msg[128], *base = NULL;

    if (argc == 1U) {
        ShowRPMSGTestUsage(PrintFunc);
        return;
    }

    if (0 == AmbaUtility_StringCompare(argv[1], "test", AmbaUtility_StringLength("test"))) {
        if (argc == 2U) {
            base = "Hello Linux! ";
        } else {
            base = argv[2];
            if (argc == 4U) {
                (void) AmbaUtility_StringToUInt32(argv[3], (UINT32 *)&count);
                AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "count is %d\n", count, 0U, 0U, 0U, 0U);
                AmbaPrint_Flush();
            }
        }

        for (i = 1U; i <= count; i++) {
            IO_UtilityStringCopy(msg, sizeof(msg), base);
            AmbaUtility_StringAppendUInt32(msg, sizeof(msg), i, 10U);
            (void)AmbaIPC_Send(EchoChannel, msg, IO_UtilityStringLength(msg) + 1);
        }
#ifdef CONFIG_AMBALINK_BOOT_OS
    } else if (0 == AmbaUtility_StringCompare(argv[1], "profile", AmbaUtility_StringLength("profile"))) {
#ifdef RPMSG_DEBUG
        if (0 == AmbaUtility_StringCompare(argv[2], "start", AmbaUtility_StringLength("start"))) {
            InitRpmsgProfile();
            return;
        } else if (0 == AmbaUtility_StringCompare(argv[2], "stop", AmbaUtility_StringLength("stop"))) {
            CalcRpmsgProfile();
            return;
        }
#endif
#if 0
    } else if (0 == AmbaUtility_StringCompare(argv[1], "rfs", AmbaUtility_StringLength("rfs"))) {
        RFS_Test(argc, argv, PrintFunc);
#endif
#endif // CONFIG_AMBALINK_BOOT_OS
    } else {
        PrintFunc("Unknown command: ");
        PrintFunc(argv[1]);
        PrintFunc("\n");
        ShowRPMSGTestUsage(PrintFunc);
    }

    return;
}

/**
 * @brief       Start basic RPMSG App
 */
void AmbaLink_StartBasicRpmsgApp(void)
{
    extern void AmbaIPC_VfsInit(void);

#if !defined(CONFIG_AMBALINK_RPMSG_ROLE_MAST)
    extern AMBA_LINK_CTRL_s AmbaLinkCtrl;
    UINT32 NvmID;
    UINT32 Rval;

    // init Clock module
    AmbaIPC_ClockInit();

    Rval = AmbaSvcWrap_GetNVMID(&NvmID, 1U, AMBA_USER_PARTITION_LINUX_KERNEL);
    if (Rval == 0U) {
        if ((NvmID == AMBA_NVM_eMMC) && (1U == AmbaLinkCtrl.AmbaLinkMachineRev))  {
            // init SD driver module
            AmbaIPC_SdInit();
        }
    }

    // init RPC module
    AmbaIPC_RpcInit();

    // init VFS module
    AmbaIPC_VfsInit();

    AmbaIPC_LinkCtrl_Init();

    /* Test app (echo) for RPMSG_DEV_AMBA */
    AmbaIPC_TestInit();
#endif

#ifdef CONFIG_AMBALINK_RPMSG_HIBER
#if !defined(CONFIG_CPU_CORTEX_R52) && !defined(CONFIG_OPENAMP)
    if (AmbaLinkBootType != AMBALINK_COLD_BOOT) {
        AmbaIPC_RpmsgRestoreEnable(0);

        AmbaIPC_RpmsgRestoreData();
    }
#endif
#endif // CONFIG_AMBALINK_RPMSG_HIBER
}
#endif  //#if defined(CONFIG_AMBALINK_RPMSG_G1) && defined(CONFIG_AMBALINK_BOOT_OS)

static AMBA_IPC_HANDLE G2EchoChannel;
/*----------------------------------------------------------------------------*\
 *  @RoutineName::  IpcTestg2
 *
 *  @Description::  test case for a "echo" g2 channel
 *
 *  @Return     ::
 *      void :
 *
\*----------------------------------------------------------------------------*/
static void IpcTestg2(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    UINT32 i, count = 8;
    const char buf[16] = "Hello";
    const char *str;
    char msg[128];

    if (argc == 1U) {
        PrintFunc("supported g2 commands\n");
        PrintFunc("    g2 test:    send 8 echo rpmsg continuously\n");
        PrintFunc("    g2 test [str] [count] : send number of count echo [str] rpmsg continuously\n");
    }
    else if (0 == AmbaUtility_StringCompare(argv[1], "test", AmbaUtility_StringLength("test"))) {
        if (argc != 2U) {
            str = &argv[2][0];
        } else {
            str = &buf[0];
        }

        if (argc == 4U) {
            (void) AmbaUtility_StringToUInt32(argv[3], (UINT32 *)&count);
            AmbaPrint_ModulePrintUInt5(AMBALINK_MODULE_ID, "count is %d\n", count, 0U, 0U, 0U, 0U);
            AmbaPrint_Flush();
        }

        for (i = 1U; i <= count; i++) {
            SIZE_t strLen;
            INT32 s32_strLen;
            IO_UtilityStringCopy(msg, sizeof(msg), str);
            AmbaUtility_StringAppendUInt32(msg, sizeof(msg), i, 10U);
            strLen = IO_UtilityStringLength(msg);
            if (strLen < (SIZE_t)AMBALINK_INT32_MAX) {
                s32_strLen = (INT32)strLen;
            }
            else {
                IpcDebug("IPC %s(%d) msg length is larger than INT32 size\n", __func__, __LINE__);
                s32_strLen = AMBALINK_INT32_MAX - 1;
            }
            (void)AmbaIPC_Send(G2EchoChannel, msg, s32_strLen + 1);
        }
    }
    else {
        PrintFunc("supported g2 commands\n");
        PrintFunc("    g2 test:    send 8 echo rpmsg continuously\n");
        PrintFunc("    g2 test [str] [count] : send number of count echo [str] rpmsg continuously\n");
    }

    return;
}

/*----------------------------------------------------------------------------*\
 *  @RoutineName::  AmbaIPC_G2Echo
 *
 *  @Description::  echo between R52 rtos & A53 rtos
 *
 *  @Return     ::
 *      INT32 : OK(0)/NG(-1)
 *
\*----------------------------------------------------------------------------*/
INT32 AmbaIPC_G2Echo(void);
INT32 AmbaIPC_G2Echo(void)
{
    INT32 ret = 0;
    G2EchoChannel = AmbaIPC_Alloc(RPMSG_DEV_AMBA, "g2_echo", AmbaIPC_MsgHandler);

    if (G2EchoChannel == NULL) {
        AmbaPrint_ModulePrintStr5(AMBALINK_MODULE_ID, "%s: AmbaIPC_Alloc failed!", __func__, NULL, NULL, NULL, NULL);
        AmbaPrint_Flush();
        ret = -1;
    }
    else {
        (void)AmbaIPC_RegisterChannel(G2EchoChannel, NULL);
    }

    return ret;
}

struct test_list_t {
    /** name of test */
    char name[32];
    /** function to run */
    AMBA_SHELL_PROC_f func;
};

INT32 IpcTestEntry(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc);
INT32 IpcTestEntry(UINT32 argc, char * const * argv, AMBA_SHELL_PRINT_f PrintFunc)
{
    INT32 subcmd_invalid = 1, i;
    static const struct test_list_t test_lists[] = {
        {"g2", IpcTestg2},
#if defined(CONFIG_AMBALINK_RPMSG_G1) && defined(CONFIG_AMBALINK_BOOT_OS)
        {"rpmsg", IpcTestRpmsg},
        {"rpc",   IpcTestRpc},
#endif // CONFIG_AMBALINK_RPMSG_G1
    };
#define TEST_LIST_SIZE (sizeof(test_lists)/sizeof(test_lists[0]))

    if (argc >= 2U) {
        for (i = 0; i < (INT32)TEST_LIST_SIZE; i++) {
            if (0 == AmbaUtility_StringCompare(argv[1], test_lists[i].name, AmbaUtility_StringLength(test_lists[i].name))) {
                subcmd_invalid = 0;
                test_lists[i].func(argc - 1U, &argv[1], PrintFunc);
                break;
            }
        }
    }

    if (subcmd_invalid == 1) {
        PrintFunc("Supported test commands:\n");
        PrintFunc("rpmsg\n");
        PrintFunc("rpc\n");
        PrintFunc("g2\n");
    }

    return 0;
}

/*
 * ml: m as mutex, s as spinlock
 * action: 0 release, 1 request
 * All action on id 0
 */
INT32 AmbaIPC_LockTest(const char *ml, const char *action, AMBA_SHELL_PRINT_f PrintFunc);
INT32 AmbaIPC_LockTest(const char *ml, const char *action, AMBA_SHELL_PRINT_f PrintFunc)
{
    if (0 == AmbaUtility_StringCompare(ml, "m", AmbaUtility_StringLength("m"))) {
        UINT32 err;

        if (0 == AmbaUtility_StringCompare(action, "0", AmbaUtility_StringLength("0"))) {
            err = AmbaIPC_MutexGive(AMBA_IPC_MUTEX_NAND);
        } else {
            err = AmbaIPC_MutexTake(AMBA_IPC_MUTEX_NAND, 100);
        }
        if (err != 0U) {
            PrintFunc("mutex failed\n");
        } else {
            PrintFunc("mutex OK\n");
        }
    } else if (0 == AmbaUtility_StringCompare(ml, "s", AmbaUtility_StringLength("s"))) {
        if (0 == AmbaUtility_StringCompare(action, "0", AmbaUtility_StringLength("0"))) {
            (void)AmbaIPC_SpinUnlock(0);
        } else {
            (void)AmbaIPC_SpinLock(0);
        }
    } else {
        PrintFunc("m or s\n");
    }

    return 0;
}
