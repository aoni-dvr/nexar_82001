#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_Test.h"


/*
 * PROGRAM {
 *        printf(void *) = 1;        [async]
 *        sum(int array[2]) = 2;
 * }
 *
 */

/*
 * sigaction to make sure svc is unregistered before process terminates
 */
static void sg_svc_unregister(int sig, siginfo_t *siginfo, void *context)
{
	ambaipc_svc_unregister(AMBA_RPC_PROG_LU_TEST_PROG_ID, AMBA_RPC_PROG_LU_TEST_VER);
	exit(0);
}

void AmbaRpcProg_LU_Test_Print_Svc(const char *msg, AMBA_IPC_SVC_RESULT_s *pRet)
{
	printf("service echo: [%s]\n", msg);
	pRet->Status = AMBA_IPC_REPLY_SUCCESS;
	pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
}

void AmbaRpcProg_LU_Test_Sum_Svc(AMBA_RPC_PROG_LU_TEST_SUM_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *ret)
{
	int sum = 0;
	if (pArg->a >= 256 || pArg->b >= 256) {
		ret->Status = AMBA_IPC_REPLY_PARA_INVALID;
	} else {
		sum = pArg->a + pArg->b;
		memcpy(ret->pResult, &sum, 4);
		ret->Length = 4;
		ret->Status = AMBA_IPC_REPLY_SUCCESS;
	}
	printf("sum is %d\n", sum);
	ret->Mode = AMBA_IPC_SYNCHRONOUS;
}

int main(int argc, char *argv[])
{
	struct sigaction act;
	AMBA_IPC_PROG_INFO_s prog_info[1];
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sg_svc_unregister;
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGTERM, &act, NULL);
	prog_info->ProcNum = 2;
    prog_info->pProcInfo = malloc(prog_info->ProcNum*sizeof(AMBA_IPC_PROC_s));
    prog_info->pProcInfo[0].Mode = AMBA_IPC_ASYNCHRONOUS;
    prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_LU_Test_Print_Svc;
    prog_info->pProcInfo[1].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info->pProcInfo[1].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_LU_Test_Sum_Svc;
 
	ambaipc_svc_register(AMBA_RPC_PROG_LU_TEST_PROG_ID, AMBA_RPC_PROG_LU_TEST_VER, "linux_svc_test", prog_info, 1);

	sleep(10000000);
	free(prog_info->pProcInfo);
	return 0;
}
