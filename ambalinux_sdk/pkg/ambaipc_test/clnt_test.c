#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_R_Test.h"

/*
 * PROGRAM {
 *        printf(void *) = 1;        [async]
 *        sum(int array[2]) = 2;
 * }
 *
 */

const char msg[] = "hello server!!";

int host;
AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_Test_Print_Clnt(const char *pStr, int *pResult, CLIENT_ID_t Clnt)
{
	AMBA_IPC_REPLY_STATUS_e status;
	status = ambaipc_clnt_call(Clnt, AMBA_RPC_PROG_R_TEST_PRINT, (void *) pStr, strlen(pStr)+1, 0, 0, 0);
	return status;
}

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_Test_Sum_Clnt(AMBA_RPC_PROG_R_TEST_SUM_ARG_s *pArg, int *pResult, CLIENT_ID_t Clnt)
{
	AMBA_IPC_REPLY_STATUS_e status;
	status = ambaipc_clnt_call(Clnt, AMBA_RPC_PROG_R_TEST_SUM, pArg, sizeof(AMBA_RPC_PROG_R_TEST_SUM_ARG_s), pResult, 4, 100);
	return status;
}
static void* clnt_thread_func(void *arg)
{
	int i, status, result;
	CLIENT_ID_t clnt;
	AMBA_RPC_PROG_R_TEST_SUM_ARG_s pSum[1];
	clnt = ambaipc_clnt_create(host, AMBA_RPC_PROG_R_TEST_PROG_ID, AMBA_RPC_PROG_R_TEST_VER);
	if (!clnt)
		goto done;

	/* test proc=1, batching mode */
	AmbaRpcProg_R_Test_Print_Clnt(msg, NULL, clnt);

	/* test proc=2, returns sume of array[0] and array[1] */
	for (i = 0; i < 32; i++) {
		pSum->a = pSum->b = i;
		status = AmbaRpcProg_R_Test_Sum_Clnt(pSum, &result, clnt);
		printf("clnt[0x%016lx]: status is %d, sum is %d\n", (unsigned long) clnt, status, result);
	}

	/* test IPC_REPLY_PARA_INVALID */
	pSum->a = pSum->b = 1024;
	status = AmbaRpcProg_R_Test_Sum_Clnt(pSum,  &result, clnt);
	printf("clnt[0x%016lx]: IPC_REPLY_PARA_INVALID status is %d\n", (unsigned long) clnt, status);

	ambaipc_clnt_destroy(clnt);

	/* test clnt_call after destroy */
	status = AmbaRpcProg_R_Test_Sum_Clnt(pSum, &result, clnt);
	printf("clnt[0x%016lx]: status is %d\n", (unsigned long) clnt, status);

done:
	pthread_exit(&result);
}

int main(int argc, char *argv[])
{
        pthread_t t1, t2;
        unsigned long prog = AMBA_RPC_PROG_R_TEST_PROG_ID;
	if (argc != 2) {
		printf("\tusage %s host \n", argv[0]);
		printf("\t      for Linux   sevice, host = 0\n");
		printf("\t      for ThreadX sevice, host = 1\n");
		exit(1);
	}

        host = atoi(argv[1]);
	pthread_create(&t1, 0, clnt_thread_func, (void*)prog);
	pthread_create(&t2, 0, clnt_thread_func, (void*)prog);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}
