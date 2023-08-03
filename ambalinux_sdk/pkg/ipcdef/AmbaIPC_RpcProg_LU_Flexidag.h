/**
 * @file inc/mw/net/rpcprog/AmbaIPC_RpcProg_LU_FLEXIDAG.h
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
#ifndef _RPC_PROG_LU_FLEXIDAG_H_
#define _RPC_PROG_LU_FLEXIDAG_H_

#include "AmbaIPC_Rpc_Def.h"

#define AMBA_RPC_PROG_LU_FLEXIDAG                   1U	/* This is ModuleName */

/* user needs to define the program id*/
#define AMBA_RPC_PROG_LU_FLEXIDAG_PROG_ID           0x20000403U /* Please follw this naming rule: ModuleName_PROG_ID */
#define AMBA_RPC_PROG_LU_FLEXIDAG_VER               1U /* RPC Program version */
#define AMBA_RPC_PROG_LU_FLEXIDAG_HOST              AMBA_IPC_HOST_LINUX

#define AMBA_RPC_PROG_LU_FLEXIDAG_OPEN              		1U
#define AMBA_RPC_PROG_LU_FLEXIDAG_INIT              		2U
#define AMBA_RPC_PROG_LU_FLEXIDAG_RUN               		3U
#define AMBA_RPC_PROG_LU_FLEXIDAG_RUN_NONBLOCK      		4U
#define AMBA_RPC_PROG_LU_FLEXIDAG_SEND_MSG          		5U
#define AMBA_RPC_PROG_LU_FLEXIDAG_CLOSE             		6U
#define AMBA_RPC_PROG_LU_FLEXIDAG_FILE_SIZE         		7U
#define AMBA_RPC_PROG_LU_FLEXIDAG_FILE_LOAD         		8U
#define AMBA_RPC_PROG_LU_FLEXIDAG_MEM_INIT          		9U
#define AMBA_RPC_PROG_LU_FLEXIDAG_MEM_ALLOC                	10U
#define AMBA_RPC_PROG_LU_FLEXIDAG_DUMP_LOG                 	11U
#define AMBA_RPC_PROG_LU_FLEXIDAG_SCHLDR_START             	12U
#define AMBA_RPC_PROG_LU_FLEXIDAG_GET_FLOWID_BY_UUID       	13U
#define AMBA_RPC_PROG_LU_FLEXIDAG_GET_FLOWID_BY_NAME       	14U
#define AMBA_RPC_PROG_LU_FLEXIDAG_SET_FREQ                 	15U
#define AMBA_RPC_PROG_LU_FLEXIDAG_SET_FEEDBACK_BUF         	16U
#define AMBA_RPC_PROG_LU_FLEXIDAG_SET_PARAM                	17U

#define AMBA_RPC_PROG_LU_FLEXIDAG_MSG_LEN		  128U
#define AMBA_RPC_PROG_LU_FLEXIDAG_PATH_LEN		   64U
#define AMBA_RPC_PROG_LU_FLEXIDAG_NAME_LEN		   64U
#define AMBA_RPC_PROG_LU_FLEXIDAG_FLOWID_NUM	           16U


typedef struct
{
  uint32_t  buffer_daddr;
  uint32_t  buffer_cacheable;
  uint32_t  buffer_size;  /* Max buffer size: (2048MB - 1) */
} __attribute__((packed)) AMBA_RPC_MEMBLK_s;

typedef struct {
	uint32_t                        num_of_buf;
	AMBA_RPC_MEMBLK_s               buf[FLEXIDAG_MAX_OUTPUTS];
} __attribute__((packed)) AMBA_RPC_IO_s;

typedef struct {
	uint16_t                        flow_id;
	char                           	Message[AMBA_RPC_PROG_LU_FLEXIDAG_MSG_LEN];
	uint32_t                        length;
} __attribute__((packed)) AMBA_RPC_MSG_s;

typedef struct {
	uint32_t handle;
	flexidag_memory_requirements_t mem_req;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_OPEN_s;

typedef struct{
	uint32_t handle;
	AMBA_RPC_MEMBLK_s state_buf;
	AMBA_RPC_MEMBLK_s temp_buf;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_INIT_s;

typedef struct{
	uint32_t handle;
	AMBA_RPC_IO_s in;
	AMBA_RPC_IO_s out;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_RUN_s;

typedef struct{
	uint32_t handle;
	AMBA_RPC_IO_s in;
	AMBA_RPC_IO_s out;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_RUN_NONBLOCK_s;

typedef struct{
	uint32_t handle;
	AMBA_RPC_MSG_s msg;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_SEND_MSG_s;

typedef struct{
	char path[AMBA_RPC_PROG_LU_FLEXIDAG_PATH_LEN];
	AMBA_RPC_MEMBLK_s bin_buf;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_FILE_LOAD_s;

typedef struct{
	AMBA_RPC_MEMBLK_s pool_buf;
	uint32_t pool_cur;
	AMBA_RPC_MEMBLK_s req_buf;
	uint32_t req_size;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_MEM_ALLOC_s;

typedef struct{
	uint32_t handle;
	uint32_t flags;
	char path[AMBA_RPC_PROG_LU_FLEXIDAG_PATH_LEN];
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_DUMP_LOG_s;

typedef struct{
	int32_t flexidag_slot;
	int32_t cavalry_slot;
	int32_t cpu_map;
	int32_t log_level;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_SCHLDR_START_s;

typedef struct{
	uint32_t handle;
	uint32_t uuid;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_UUID_s;

typedef struct{
	uint32_t handle;
	uint8_t is_instance;
	uint8_t is_algorithm;
	uint8_t is_step;
	uint8_t is_cvtask;
	char instance[AMBA_RPC_PROG_LU_FLEXIDAG_NAME_LEN];
	char algorithm[AMBA_RPC_PROG_LU_FLEXIDAG_NAME_LEN];
	char step[AMBA_RPC_PROG_LU_FLEXIDAG_NAME_LEN];
	char cvtask[AMBA_RPC_PROG_LU_FLEXIDAG_NAME_LEN];
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_NAME_s;

typedef struct{
	uint32_t num_found;
	uint16_t flow_id[AMBA_RPC_PROG_LU_FLEXIDAG_FLOWID_NUM];
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_NAME_RPLY_s;

typedef struct{
	uint32_t handle;
	uint32_t num_of_array;
	uint16_t flow_id[AMBA_RPC_PROG_LU_FLEXIDAG_FLOWID_NUM];
	uint8_t freq[AMBA_RPC_PROG_LU_FLEXIDAG_FLOWID_NUM];
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_FREQ_s;

typedef struct{
	uint32_t handle;
	uint32_t feedback_num;
	AMBA_RPC_MEMBLK_s buf;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_FEEDBACK_BUF_s;

typedef struct{
	uint32_t handle;
	uint32_t id;
	uint32_t value;
} __attribute__((packed)) AMBA_RPC_LU_FLEXIDAG_PARAM_s;

#endif /* _RPC_PROG_LU_FLEXIDAG_H_ */
