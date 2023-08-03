/**
  * Copyright (c) 2014 by Ambarella Inc.

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:

  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  * THE SOFTWARE.
 **/
#ifndef __AIPC_USER_H__
#define __AIPC_USER_H__

#include "aipc_msg.h"
#define NL_PROTO_AMBAIPC       25
#define AIPC_WAIT_TIME		0xFFFFFFFF
#define RPC_PARAM_SIZE		8

#define AIPC_BINDER_BIND       AMBA_IPC_BINDER_BIND
#define AIPC_BINDER_REGISTER   AMBA_IPC_BINDER_REGISTER
#define AIPC_BINDER_UNREGISTER AMBA_IPC_BINDER_UNREGISTER
#define AIPC_BINDER_LIST       AMBA_IPC_BINDER_LIST
#define AIPC_BINDER_REBIND     AMBA_IPC_BINDER_REBIND
#define AIPC_BINDER_FIND       AMBA_IPC_BINDER_FIND

/**
 * The procedure calls to client controller.
 *
 */
typedef enum _AMBA_IPC_CLNT_CTRL_e_ {
	AMBA_IPC_CLNT_REBIND = 0,
} AMBA_IPC_CLNT_CTRL_e;


#ifdef RPC_DEBUG
/*record the rpc profiling result, the definition is shared
between dual-OSes.*/
/* Note the statistics of rpmsg is stored after rpc.
If there is any modification with the structure AMBA_RPC_STATISTIC_s,
please update the rpmsg starting address in Ambalink_cfg.h*/
typedef struct _AMBA_RPC_STATISTIC_s_ {
	/***************ThreadX side*****************/
	unsigned int TxLastInjectTime;
	unsigned int TxTotalInjectTime;
	unsigned int TxRpcSendTime;
	unsigned int TxRpcRecvTime;
	/********************************************/
	/***************Linux side*******************/
	unsigned int LuLastInjectTime;
	unsigned int LuTotalInjectTime;
	unsigned int LkToLuTime;
	unsigned int LuToLkTime;
	unsigned int MaxLkToLuTime;
	unsigned int MinLkToLuTime;
	unsigned int MaxLuToLkTime;
	unsigned int MinLuToLkTime;
	/********************************************/
	/***************dual OSes********************/
	unsigned int TxToLuTime;
	unsigned int LuToTxTime;
	unsigned int MaxTxToLuTime;
	unsigned int MinTxToLuTime;
	unsigned int MaxLuToTxTime;
	unsigned int MinLuToTxTime;
	unsigned int TxToLuCount;
	unsigned int LuToTxCount;
	unsigned int SynPktCount;
	unsigned int AsynPktCount;
	unsigned int RoundTripTime;
	unsigned int OneWayTime;
	/********************************************/
} AMBA_RPC_STATISTIC_s;

#endif
/*
 * create a new NL socket, returns socket fd
 *
 */
int nl_open_socket(unsigned int *out_port);

/*
 * close a NL socket
 */
int nl_close_socket(int sock);

/*
 * send a packet to kernel
 */
int nl_send_pkt(int sock, struct aipc_pkt *pkt, int payload_len);

/*
 * receive a packet
 */
int nl_recv_pkt(int sock, struct aipc_pkt *pkt, int max_payload_len, unsigned int timeout);

/*
 * allocate an aipc_pkt with length @len
 */
struct aipc_pkt* nl_alloc_pkt(int len);

/*
 * free an aipc_pkt
 */
void nl_free_pkt(struct aipc_pkt *pkt);


int ambaipc_svc_register(
	int prog, int vers, char *name, AMBA_IPC_PROG_INFO_s *prog_info, int new_thread);

int ambaipc_svc_unregister(int program, int version);

int ambaipc_svc_sendreply(struct aipc_msg *msg, struct aipc_xprt *xprt, AMBA_IPC_SVC_RESULT_s *result);

CLIENT_ID_t ambaipc_clnt_create(int host, int program, int version);

int ambaipc_clnt_destroy(CLIENT_ID_t clnt);

AMBA_IPC_REPLY_STATUS_e ambaipc_clnt_call(CLIENT_ID_t clnt, int proc,
		void *in,  int in_len,
		void *out, int out_len,
		unsigned int timeout);

const char *ambaipc_strerror(int error);

#endif //__AIPC_USER_H__
