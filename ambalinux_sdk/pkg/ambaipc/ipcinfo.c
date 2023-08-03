/*
 * pkg/ambaipc/ipcinfo.c
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
#include "aipc_user.h"

extern unsigned int rpc_size;

int main(int argc, char *argv[])
{
	struct aipc_pkt *pkt;
	int sock, host = AIPC_HOST_LINUX;
	unsigned int port;

	if (argc == 2)
		host = atoi(argv[1]);

	sock = nl_open_socket(&port);
	pkt  = nl_alloc_pkt(rpc_size);

	// send BINDER_LIST command to binder port
	pkt->xprt.client_addr = AIPC_HOST_LINUX;
	pkt->xprt.client_port = port;
	pkt->xprt.server_addr = host;
	pkt->xprt.server_port = AIPC_BINDING_PORT;
	pkt->msg.type = AIPC_MSG_CALL;
	pkt->msg.u.call.proc = AIPC_BINDER_LIST;
	nl_send_pkt(sock, pkt, 0);

	printf("   program version  port  name\n");
	while (1) {
		nl_recv_pkt(sock, pkt, rpc_size, AIPC_WAIT_TIME);
		if (pkt->msg.parameters[0] == 0)
			break;

		printf(" %7ld %7ld %6ld   %s\n",
			pkt->msg.parameters[0],
			pkt->msg.parameters[1],
			pkt->msg.parameters[2],
			(char*)&pkt->msg.parameters[3]);
	}

	nl_free_pkt(pkt);
	nl_close_socket(sock);
	return 0;
}
