#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_AmbaFSNotify.h"

#define MAX_FILE_NAME_LEN			512
#define MAX_READ_TIME				1500

AMBA_IPC_REPLY_STATUS_e RT_Ambafs_Notify_Ctrl_Clnt(RT_AMBAFS_NOTIFY_CTRL_s *pArg, int *pResult, CLIENT_ID_t Clnt)
{
	AMBA_IPC_REPLY_STATUS_e status;
	status = ambaipc_clnt_call(Clnt, RT_AMBAFS_NOTIFY_CTRL, pArg, sizeof(RT_AMBAFS_NOTIFY_CTRL_s), pResult, 4, AIPC_WAIT_TIME);

	return status;
}

AMBA_IPC_REPLY_STATUS_e RT_Ambafs_Notify_AddWatch_Clnt(RT_AMBAFS_NOTIFY_WATCH_s *pArg, int *pResult, CLIENT_ID_t Clnt)
{
	AMBA_IPC_REPLY_STATUS_e status;
	status = ambaipc_clnt_call(Clnt, RT_AMBAFS_NOTIFY_ADD_WATCH, pArg, sizeof(RT_AMBAFS_NOTIFY_WATCH_s) + pArg->len, pResult, 4, AIPC_WAIT_TIME);

	return status;
}

AMBA_IPC_REPLY_STATUS_e RT_Ambafs_Notify_GetEvent_Clnt(int *pArg, void *pResult, CLIENT_ID_t Clnt)
{
	AMBA_IPC_REPLY_STATUS_e status;
	status = ambaipc_clnt_call(Clnt, RT_AMBAFS_NOTIFY_GET_EVENT, &Clnt, 4, pResult, sizeof(RT_AMBAFS_NOTIFY_EVENT_s) + MAX_FILE_NAME_LEN, AIPC_WAIT_TIME);
	return status;
}
