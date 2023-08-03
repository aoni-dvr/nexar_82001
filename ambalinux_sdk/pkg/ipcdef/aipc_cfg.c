#include "AmbaIPC_Rpc_Def.h"

/****user input but need modify rpmsg value in kernel****/
#define RPMSG_BUF_SIZE			2048


/************predefine values***********************/
#define RPMSG_HDR			16
#define RPC_HDR				128


unsigned int rpmsg_size 	= RPMSG_BUF_SIZE;
unsigned int rpc_size 		= (RPMSG_BUF_SIZE - RPMSG_HDR - RPC_HDR);
