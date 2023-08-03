/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_Stream.h
 *
 * Header file for Stream RPC Services (Linux side)
 *
 * Copyright (C) 2014, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */
#ifndef _RPC_PROG_LU_STREAM_H_
#define _RPC_PROG_LU_STREAM_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_LU_STREAM					1U	/* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_LU_STREAM_PROG_ID			0x20000402U /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_LU_STREAM_VER				1U /* RPC Program version */
#define AMBA_RPC_PROG_LU_STREAM_HOST			AMBA_IPC_HOST_LINUX

#define AMBA_RPC_PROG_LU_STREAM_OPEN			1U
#define AMBA_RPC_PROG_LU_STREAM_CLOSE			2U
#define AMBA_RPC_PROG_LU_STREAM_READ			3U
#define AMBA_RPC_PROG_LU_STREAM_SEEK			4U
#define AMBA_RPC_PROG_LU_STREAM_GETPOS			5U

#define AMBA_RPC_PROG_LU_MODE_RDONLY			(1U)    /**< Read only */
#define AMBA_RPC_PROG_LU_MODE_WRONLY			(2U)    /**< Write only */
#define AMBA_RPC_PROG_LU_MODE_RDWR				(3U)    /**< Read write */

#define AMBA_RPC_PROG_LU_SEEK_START				(1U)    /**< The beginning of a file (seek to the position relative to start position of file) */
#define AMBA_RPC_PROG_LU_SEEK_CUR				(2U)    /**< The current position of a file (seek to the position relative to current file position) */
#define AMBA_RPC_PROG_LU_SEEK_END				(3U)    /**< The end of a file (seek to the position relative to end position of file) */

#define AMBA_RPC_PROG_LU_FILE_LEN				124U

typedef unsigned int (*stream_stop_cb)(char *pName);

typedef struct{
	char 				Name[AMBA_RPC_PROG_LU_FILE_LEN];
	unsigned int 		Mode;
} __attribute__((packed)) AMBA_RPC_LU_STREAM_OPEN_s;

typedef struct{
	unsigned int		Fd;
} __attribute__((packed)) AMBA_RPC_LU_STREAM_CLOSE_s;

typedef struct{
	unsigned int		Fd;
	unsigned int 		Size;
	unsigned int 		PA_Buffer;
} __attribute__((packed)) AMBA_RPC_LU_STREAM_READ_s;

typedef struct{
	unsigned int		Fd;
	signed long long 	Pos;
	unsigned int 		Orig;
} __attribute__((packed)) AMBA_RPC_LU_STREAM_SEEK_s;

typedef struct{
	unsigned int		Fd;
} __attribute__((packed)) AMBA_RPC_LU_STREAM_GETPOS_s;

unsigned int AmbaRpcProg_LU_Stream_Init(void);
unsigned int AmbaRpcProg_LU_Stream_Deinit(void);
unsigned int AmbaRpcProg_LU_Stream_Start(const char *pName, unsigned int len, stream_stop_cb cb);

#endif /* _RPC_PROG_LU_STREAM_H_ */
