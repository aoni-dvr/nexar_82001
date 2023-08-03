/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaIPC_RpcProg_RT_AmbaFSNotify.h
 *
 *  @Copyright      :: Copyright (C) 2015 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions for AmbaIPC RPC ambafs_notify program.
 *
 *
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _RPC_PROG_RT_AMBAFS_NOTIFY_H_
#define _RPC_PROG_RT_AMBAFS_NOTIFY_H_

#include "AmbaIPC_Rpc_Def.h"

#define RT_AMBAFS_NOTIFY_PROG_ID	0x1000000A
#define RT_AMBAFS_NOTIFY_VER		1


#define RT_AMBAFS_NOTIFY_CTRL			0x1

#define RT_AMBAFS_NOTIFY_INIT_CMD		0x1
#define RT_AMBAFS_NOTIFY_CLOSE_CMD		0x2

typedef struct _RT_AMBAFS_NOTIFY_CTRL_s_ {
	CLIENT_ID_t	client_id;
	int		cmd;
} RT_AMBAFS_NOTIFY_CTRL_s;

void RT_Amabafs_Notify_Ctrl_Svc(RT_AMBAFS_NOTIFY_CTRL_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
AMBA_IPC_REPLY_STATUS_e RT_Ambafs_Notify_Ctrl_Clnt(RT_AMBAFS_NOTIFY_CTRL_s *pArg, int *pResult, CLIENT_ID_t Clnt);

#define RT_AMBAFS_NOTIFY_ADD_WATCH		0x2

typedef struct _RT_AMBAFS_NOTIFY_WATCH_s_ {
	CLIENT_ID_t	client_id;
	unsigned int	mask;
	unsigned int	len;
	char		name[0];
} RT_AMBAFS_NOTIFY_WATCH_s;

void RT_Ambafs_Notify_AddWatch_Svc(RT_AMBAFS_NOTIFY_WATCH_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet);
AMBA_IPC_REPLY_STATUS_e RT_Ambafs_Notify_AddWatch_Clnt(RT_AMBAFS_NOTIFY_WATCH_s *pArg, int *pResult, CLIENT_ID_t Clnt);

#define RT_AMBAFS_NOTIFY_GET_EVENT		0x3

typedef struct _RT_AMBAFS_NOTIFY_EVENT_s_ {
	int		wd;
	unsigned int	mask;
	unsigned int	len;
	char		name[0];
} RT_AMBAFS_NOTIFY_EVENT_s;

void RT_Ambafs_Notify_GetEvent_Svc(int *client_id, AMBA_IPC_SVC_RESULT_s *pRet);
AMBA_IPC_REPLY_STATUS_e RT_Ambafs_Notify_GetEvent_Clnt(int *pArg, void *pResult, CLIENT_ID_t Clnt);

#endif
