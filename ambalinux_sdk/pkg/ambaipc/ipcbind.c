/*
 * pkg/ambaipc/ipcbind.c
 *
 * Author: Joey Li <jli@ambarella.com>
 *
 * Copyright (C) 2013, Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <getopt.h>
#include "aipc_msg.h"
#include "aipc_user.h"
#include "list.h"
#include "aipc_crc.h"
#include "aipc_priv_crc.h"

#define LOGE(format, ...) printf("[IPCBINDER->] " format, ##__VA_ARGS__);
#define LOGD(format, ...)

#define MAX_SVC_NAME_SIZE      32

struct prog_record {
	int prog_id;
	int vers_id;
	unsigned int port_id;
	char name[MAX_SVC_NAME_SIZE];
	int proc_num;
	AMBA_IPC_COMMUICATION_MODE_e *mode;	//recore the communication modes of procedures
	unsigned int rebind_cnt;
};

struct prog_entry {
	struct prog_record record;
	struct list_head   list;
};

static struct option longopts[] = {
	{"background", no_argument, NULL, 'b'},
	{"help", no_argument, NULL, 'h'}
};

static LIST_HEAD(head);
static int sock_fd;
extern unsigned int rpc_size;

/* never used */
#if 0
/*
 * dump the program list
 */
static void dump_program()
{
	struct prog_entry *entry;

	list_for_each_entry(entry, &head, list) {
		printf("%d %d %X %s\n",
			entry->record.prog_id, entry->record.vers_id,
			entry->record.port_id, entry->record.name);
	}
}
#endif

/*
 * find a program from the list
 */
static struct prog_entry* find_program(struct aipc_pkt *pkt)
{
	struct prog_entry *entry;
	int prog = pkt->msg.u.call.prog;
	int vers = pkt->msg.u.call.vers;

	list_for_each_entry(entry, &head, list) {
		if (entry->record.prog_id == prog &&
		    entry->record.vers_id == vers) {
			return entry;
		}
	}

	return NULL;
}

/*
 * add a new program to the list
 */
static void add_program(struct aipc_pkt *pkt)
{
	struct prog_entry *entry = find_program(pkt);
	int str_len;
	int status = 0;

	if (entry) {
		LOGE("service %d already exit, ignore new request\n",
			pkt->msg.u.call.prog);
		status = -1;
		goto done;
	}

	entry = calloc(1, sizeof(struct prog_entry));
	entry->record.prog_id = pkt->msg.u.call.prog;
	entry->record.vers_id = pkt->msg.u.call.vers;
	entry->record.port_id = pkt->xprt.client_port;
	entry->record.proc_num = pkt->msg.parameters[0];
	// record the procedure information
	entry->record.mode = calloc(entry->record.proc_num, sizeof(AMBA_IPC_COMMUICATION_MODE_e));
	memcpy(entry->record.mode, &pkt->msg.parameters[1],
		entry->record.proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
	str_len = strlen((void*)&(pkt->msg.parameters[1+entry->record.proc_num]))+1;
	if (str_len > MAX_SVC_NAME_SIZE)
		str_len = MAX_SVC_NAME_SIZE;
	memcpy(entry->record.name, &pkt->msg.parameters[1+entry->record.proc_num], str_len);

	list_add(&entry->list, &head);

done:
	pkt->msg.type = AIPC_MSG_REPLY;
	pkt->msg.u.reply.status = AIPC_REPLY_SUCCESS;
	pkt->msg.parameters[0] = status;	//register OK or NG
	nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE);
}

/*
 * del a program from the list
 */
static void del_program(struct aipc_pkt *pkt)
{
	struct prog_entry *entry = find_program(pkt);

	if (entry) {
		list_del(&entry->list);
		free(entry->record.mode);
		free(entry);
	}
}

/*
 * find a registered program and return its service port
 */
static void bind_program(struct aipc_pkt *pkt)
{
	struct prog_entry *entry = find_program(pkt);
	int prog;
	unsigned int crc;
	pkt->msg.type = AIPC_MSG_REPLY;
	if (!entry) {
		pkt->msg.u.reply.status = AIPC_REPLY_PROG_UNAVAIL;
		nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE);
	} else {
		prog = pkt->msg.u.call.prog;	// to record the prog id cause u.reply will overwrite later.
		pkt->msg.u.reply.status = AIPC_REPLY_SUCCESS;
		pkt->msg.parameters[0] = entry ? entry->record.port_id : 0;
		crc = getCrc(prog);
		if(crc==0){
			crc = getPrivCrc(prog);
		}
		pkt->msg.parameters[1] = crc;
		pkt->msg.parameters[2] = entry->record.proc_num;
		// we only need to send the communication mode of procedures.
		memcpy(&pkt->msg.parameters[3], entry->record.mode, entry->record.proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
        nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE*3 + entry->record.proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
	}

}

/*
 * list all of registered programs
 */
static void list_program(struct aipc_pkt *pkt)
{
	struct prog_entry *entry;

	pkt->msg.type = AIPC_MSG_REPLY;
	pkt->msg.u.reply.status = AIPC_REPLY_SUCCESS;
	list_for_each_entry(entry, &head, list) {
		pkt->msg.parameters[0] = entry->record.prog_id;
		pkt->msg.parameters[1] = entry->record.vers_id;
		pkt->msg.parameters[2] = entry->record.port_id;
		strcpy((char*)&pkt->msg.parameters[3], entry->record.name);
		nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE*3 + strlen(entry->record.name) + 1);
	}

	// send a terminating nl message
	memset(pkt->msg.parameters, 0, 12);
	nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE*3);
}

static void rebind_program(struct aipc_pkt *pkt, int len)
{
	struct prog_entry *entry = find_program(pkt);

	if(!entry) {
		/* the svc does not exist */
		pkt->msg.type = AIPC_MSG_REPLY;
		pkt->msg.u.reply.status = AIPC_REPLY_PROG_UNAVAIL;
		nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE);
	} else {
		/* relay the client call */
		entry->record.rebind_cnt++;
		if (entry->record.rebind_cnt <= 1) {
			pkt->xprt.server_port = entry->record.port_id;
			pkt->msg.u.call.proc = pkt->xprt.private; /* restore the original proc */
			nl_send_pkt(sock_fd, pkt, len);	/* send to the right server */

			/* send svc new info to client controller */
			pkt->msg.type = AMBA_IPC_MSG_CALL;
			pkt->xprt.server_port = AIPC_CLNT_CONTROL_PORT;
			pkt->xprt.server_addr = AIPC_HOST_THREADX;
			pkt->xprt.client_addr = AIPC_HOST_LINUX;
			pkt->msg.u.call.proc = AMBA_IPC_CLNT_REBIND;
			pkt->msg.parameters[0] = entry->record.port_id;
			nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE);
		} else {
			del_program(pkt);
			pkt->msg.type = AIPC_MSG_REPLY;
			pkt->msg.u.reply.status = AIPC_REPLY_PROG_UNAVAIL;
			nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE);
		}
	}

}

/*
 * process an incoming packet
 */
static void process_pkt(struct aipc_pkt *pkt, int len)
{
	LOGD("Received: %u %u %u %u %d\n",
		pkt->xprt.client_addr, pkt->xprt.client_port,
		pkt->xprt.server_addr, pkt->xprt.server_port,
		pkt->msg.u.call.proc);

	switch (pkt->msg.u.call.proc) {
	case AIPC_BINDER_BIND:
		bind_program(pkt);
		break;
	case AIPC_BINDER_REGISTER:
		add_program(pkt);
		break;
	case AIPC_BINDER_UNREGISTER:
		del_program(pkt);
		break;
	case AIPC_BINDER_LIST:
		list_program(pkt);
		break;
	case AIPC_BINDER_REBIND:
		rebind_program(pkt, len);
		break;
	case AIPC_BINDER_FIND:
		if(find_program(pkt)) {
			pkt->msg.parameters[0] = 1; // the program is found.
		} else{
			pkt->msg.parameters[0] = 0; // the program is not found.
		}
		pkt->msg.type = AIPC_MSG_REPLY;
		pkt->msg.u.reply.status = AIPC_REPLY_SUCCESS;
		nl_send_pkt(sock_fd, pkt, RPC_PARAM_SIZE);
		break;
	default:
		LOGE("unknown command %d\n", pkt->msg.u.call.proc);
	}
}

int main(int argc, char *argv[])
{
	struct sockaddr_nl src_addr;
	struct aipc_pkt    *pkt;
	int ret, ch, len;

	while((ch = getopt_long(argc, argv, "bh", longopts, NULL)) != -1){
		switch (ch) {
			case 'b':
				if(daemon(0,1)){
					fprintf(stderr, "failed to be a daemon process\n");
				}
				break;
			case 'h':
				printf("Usage: ipcbind [OPTION]\n");
				printf("-b, --background\t run in the background\n");
				printf("-h, --help\t print the help for ipcbind\n");
				printf("ipcbind will run in the foreground, if no option is given.\n");
				return 0;
			default:
				return -1;
		}
	}

	sock_fd = socket(PF_NETLINK, SOCK_RAW, NL_PROTO_AMBAIPC);
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = AIPC_BINDING_PORT;
	ret = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
	if (ret) {
		printf("failed to open binding port with code %d\n", ret);
		exit(1);
	}

	pkt = nl_alloc_pkt(rpc_size);
	while (1) {
		len = nl_recv_pkt(sock_fd, pkt, rpc_size, AIPC_WAIT_TIME);
		process_pkt(pkt, len);
	}
	nl_free_pkt(pkt);

	close(sock_fd);
	return 0;
}
