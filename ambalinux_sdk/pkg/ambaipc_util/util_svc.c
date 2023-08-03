#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <getopt.h>
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_Util.h"

static struct option longopts[] = {
	{"background", no_argument, NULL, 'b'},
	{"help", no_argument, NULL, 'h'}
};

extern int rpc_size;
/*
 * sigaction to make sure svc is unregistered before process terminates
 */
static void sg_svc_unregister(int sig, siginfo_t *siginfo, void *context)
{
	printf("unregister bt sig %d", sig);
	ambaipc_svc_unregister(AMBA_RPC_PROG_LU_UTIL_PROG_ID, AMBA_RPC_PROG_LU_UTIL_VER);
	exit(0);
}
/*
 * Server will send the exectuin result back to client
 */
void AmbaRpcProg_Util_Exec1_Svc(AMBA_RPC_PROG_EXEC_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
	FILE *fd;
	char buffer[100];
	char *exec_result;
	int total_len, len;

	exec_result = (char *)malloc(4096*sizeof(char));
	fd = popen(pArg->command, "r");

	if(fd == NULL){
		printf("popen() error!\n");
		pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
		pRet->Mode = AMBA_IPC_SYNCHRONOUS;
		return;
	}
	fgets(buffer, 100, fd);
	strcpy(exec_result, buffer);
	total_len = strlen(buffer);
	while(fgets(buffer, 100, fd) != NULL){
		len = strlen(buffer);
		total_len += len;
		if(total_len >= rpc_size){
			break;
		}
		strcat(exec_result, buffer);
	}
	memcpy(pRet->pResult, exec_result, strlen(exec_result)+1);
	pRet->Length = strlen(exec_result)+1;
	pRet->Status = AMBA_IPC_REPLY_SUCCESS;
	pRet->Mode = AMBA_IPC_SYNCHRONOUS;
	free(exec_result);
	pclose(fd);
}

/*
 * Server does not send the execution result back.
 */
void AmbaRpcProg_Util_Exec2_Svc(AMBA_RPC_PROG_EXEC_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
	int success;
	success = system(pArg->command);

	if(success!=0) {
		printf("execute command failed!\n");
		pRet->Status = AMBA_IPC_REPLY_PARA_INVALID;
		pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
		return;
	}
	pRet->Status = AMBA_IPC_REPLY_SUCCESS;
	pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
}

int main(int argc, char *argv[])
{
	struct sigaction act;
	int ch;
	int ret;
	AMBA_IPC_PROG_INFO_s prog_info[1];
	int rval = 0;

	while((ch = getopt_long(argc, argv, "bh", longopts, NULL)) != -1){
		switch (ch) {
			case 'b':
				if(daemon(0,1)){
					fprintf(stderr, "failed to be a daemon process\n");
				}
				break;
			case 'h':
				printf("Usage: util_svc [OPTION]\n");
				printf("-b, --background\t run in the background\n");
				printf("-h, --help\t print the help for ipcbind\n");
				printf("util_svc will run in the foreground, if no option is given.\n");
				return 0;
			default:
				return -1;
		}
	}

	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sg_svc_unregister;
	act.sa_flags = SA_SIGINFO;
	ret = sigaction(SIGTERM, &act, NULL);
	if(ret < 0){
		printf("sigaction error: SIGTERM\n");
	}

	prog_info->ProcNum = 2;
    	prog_info->pProcInfo = malloc(prog_info->ProcNum*sizeof(AMBA_IPC_PROC_s));
    	prog_info->pProcInfo[0].Mode = AMBA_IPC_SYNCHRONOUS;
    	prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_Util_Exec1_Svc;
    	prog_info->pProcInfo[1].Mode = AMBA_IPC_ASYNCHRONOUS;
    	prog_info->pProcInfo[1].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_Util_Exec2_Svc;

	while( 0 != (rval = ambaipc_svc_register(AMBA_RPC_PROG_LU_UTIL_PROG_ID, AMBA_RPC_PROG_LU_UTIL_VER, "linux_util_svc", prog_info, 1)) ) {
		fprintf(stderr,"ambaipc_svc_register 'linux_util_svc' returned %d \n", rval);
		ambaipc_svc_unregister(AMBA_RPC_PROG_LU_UTIL_PROG_ID, AMBA_RPC_PROG_LU_UTIL_VER);
		usleep(10000);
	}
	while(1){
		sleep(10000000);
	}
	free(prog_info->pProcInfo);
	return 0;
}
