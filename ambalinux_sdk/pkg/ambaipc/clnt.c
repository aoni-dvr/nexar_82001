#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include "aipc_user.h"
#include "aipc_crc.h"
#include "aipc_priv_crc.h"

#define fill_pkt_header(to, from) (*(to)) = (from)
#define MAGIC_NUMBER              0xBABEFACE

struct clnt_handle {
	int sock;
	int ctrl_sock;			// record the socket for receiving control info
	unsigned int      magic;
	struct aipc_pkt   hdr;
	struct aipc_pkt  *pkt;
	struct aipc_pkt  *ctrl_pkt;
	pthread_mutex_t   lock;
	AMBA_IPC_COMMUICATION_MODE_e    *proc_mode;   // record the communication modes for procedures
	pthread_t	  ctrl_tsk;	// recv control info
};

extern unsigned int rpc_size;

#ifdef RPC_DEBUG
	extern unsigned int read_timer();
	extern void rpc_record_stats(struct aipc_xprt *xprt, AMBA_IPC_COMMUICATION_MODE_e mode);
#endif

static int ambaipc_clnt_crc_check(unsigned int svc_crc, int prog)
{
	unsigned int clnt_crc= getCrc(prog);
	if(clnt_crc==0){	// if it can't be found in public rpc, try to find it in private rpc.
		clnt_crc=getPrivCrc(prog);
	}

	if(clnt_crc!=svc_crc){
		printf("The crc in Linux: %u\n", clnt_crc);
		printf("The crc in Thread: %u\n", svc_crc);
		printf("The CRC values are not matching\n");
		return -1;	//The CRC values are not matching
	}

	return 0;	//All of the RPC header files are consistent in dual OSes
}

/*
 * send bind message to binder
 */
static AMBA_IPC_REPLY_STATUS_e send_binder_msg(struct clnt_handle *clnt){
	AMBA_IPC_REPLY_STATUS_e clnt_status;
	int len;
	// send binder_bind message
	clnt->hdr.msg.type = AIPC_MSG_CALL;
	clnt->hdr.msg.u.call.proc  = AIPC_BINDER_BIND;
	clnt->hdr.xprt.server_port = AIPC_BINDING_PORT;
	clnt->hdr.xprt.mode = AMBA_IPC_SYNCHRONOUS;

	fill_pkt_header(clnt->pkt, clnt->hdr);
	clnt->pkt->msg.parameters[0] = getpid();
	nl_send_pkt(clnt->sock, clnt->pkt, RPC_PARAM_SIZE);

	// get the service port of the host from reply
	len = nl_recv_pkt(clnt->sock, clnt->pkt, rpc_size, AIPC_WAIT_TIME);
	if( len == 0 ){
		clnt->pkt->msg.u.reply.status = AMBA_IPC_REPLY_PROG_UNAVAIL;
	}
	clnt_status = clnt->pkt->msg.u.reply.status;
	if( clnt_status != AMBA_IPC_REPLY_SUCCESS ){
		clnt->hdr.xprt.server_port = 0;
	}
	return clnt_status;
}
/*
 * After binding successfully, record the server port and do CRC checking
 */
static int clnt_init_connection(struct clnt_handle *clnt) {
	int crc_status;
	int proc_num;
	/* record the server port */
	clnt->hdr.xprt.server_port = clnt->pkt->msg.parameters[0];
	/* record the proc information */
	proc_num = clnt->pkt->msg.parameters[2];
	clnt->proc_mode = malloc(proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
	memset(clnt->proc_mode, 0, proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
	memcpy(clnt->proc_mode, &clnt->pkt->msg.parameters[3], proc_num*sizeof(AMBA_IPC_COMMUICATION_MODE_e));
	/* CRC checking */
	crc_status = ambaipc_clnt_crc_check((unsigned int)clnt->pkt->msg.parameters[1], clnt->hdr.msg.u.call.prog);
	if ( crc_status != 0 ){
		printf("CRC values check error!\n");
		ambaipc_clnt_destroy((CLIENT_ID_t)clnt);
		return AMBA_IPC_CRC_ERROR;
	}

	return 0;
}

/*
 * receiving control pkt
 */
static void* clnt_ctrl_recv(void *arg)
{
	struct clnt_handle *clnt = (struct clnt_handle *)arg;
	int len;

	while (1) {
		len = nl_recv_pkt(clnt->ctrl_sock, clnt->ctrl_pkt, rpc_size, AIPC_WAIT_TIME);

		if(len < 0) {
			printf("%s receiving error: %s\n", __func__, strerror(errno));
			continue;
		}

		switch(clnt->ctrl_pkt->msg.u.call.proc) {
			case AMBA_IPC_CLNT_REBIND:
			// update the server information
				pthread_mutex_lock(&clnt->lock);
				clnt->hdr.xprt.server_port = clnt->ctrl_pkt->msg.parameters[0];
				printf("%s update server 0x%x info\n", __func__, clnt->hdr.msg.u.call.prog);
				pthread_mutex_unlock(&clnt->lock);
				break;
		}

	}
	return NULL;
}

/*
 * create a new IPC client
 */
CLIENT_ID_t ambaipc_clnt_create(int host, int prog, int vers)
{
	struct clnt_handle *clnt;
	pthread_mutexattr_t mta;
	unsigned int port;
	AMBA_IPC_REPLY_STATUS_e bind_status;

	clnt = malloc(sizeof(struct clnt_handle));

	if(clnt == NULL) {
		return 0;
	}

	clnt->sock = nl_open_socket(&port);
	clnt->pkt  = nl_alloc_pkt(rpc_size);
	clnt->magic = MAGIC_NUMBER;
	// create a mutex to be MT-safe
	pthread_mutexattr_init(&mta);
	pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&clnt->lock, &mta);

	// fill the header information
	clnt->hdr.msg.u.call.prog  = prog;
	clnt->hdr.msg.u.call.vers  = vers;
	clnt->hdr.xprt.client_addr = AIPC_HOST_LINUX;
	clnt->hdr.xprt.client_port = port;
	clnt->hdr.xprt.server_addr = host;

	clnt->ctrl_sock = nl_open_socket(&clnt->hdr.xprt.client_ctrl_port);
	clnt->ctrl_pkt  = nl_alloc_pkt(rpc_size);

#ifdef RPC_DEBUG
	// check whether rpc_debug is enabled
	rpc_proc_open();
#endif

	// send binder the bind message to get server port and CRC value of the header file
	bind_status = send_binder_msg(clnt);

	if ( bind_status == AMBA_IPC_REPLY_SUCCESS ) {
		if(clnt_init_connection(clnt) == AMBA_IPC_CRC_ERROR) {
			ambaipc_clnt_destroy((CLIENT_ID_t)clnt);
			return 0;
		}
	}
	else {	//server is not created yet.
		printf("can not find svc for program %d! Client needs to bind again next time!\n", prog);
	}

	// create a thread to recv control info such as svc port info.
	pthread_create(&clnt->ctrl_tsk, 0, clnt_ctrl_recv, (void *)clnt);

	return (CLIENT_ID_t) clnt;
}

int ambaipc_clnt_destroy(CLIENT_ID_t clnt_id)
{
	struct clnt_handle *clnt = (struct clnt_handle*)clnt_id;

	pthread_mutex_lock(&clnt->lock);
	nl_close_socket(clnt->sock);
	nl_free_pkt(clnt->pkt);
	nl_free_pkt(clnt->ctrl_pkt);
	clnt->magic = 0x0;
	pthread_mutex_unlock(&clnt->lock);
	pthread_mutex_destroy(&clnt->lock);
	pthread_cancel(clnt->ctrl_tsk);
	pthread_join(clnt->ctrl_tsk, NULL);
	nl_close_socket(clnt->ctrl_sock);
	free((void*)clnt->proc_mode);
	free((void*)clnt);

	return 0;
}

AMBA_IPC_REPLY_STATUS_e ambaipc_clnt_call(CLIENT_ID_t clnt_id, int proc,
		void *in,  int in_len,
		void *out, int out_len,
		unsigned int timeout)
{
	struct clnt_handle *clnt = (struct clnt_handle*)clnt_id;
	int len;
	AMBA_IPC_REPLY_STATUS_e status = AIPC_REPLY_SUCCESS;
	AMBA_IPC_REPLY_STATUS_e bind_status;

	if(clnt == NULL) {
		printf("%s Client id is invalid\n", __func__);
		return -AMBA_IPC_INVALID_CLIENT_ID;
	}

	pthread_mutex_lock(&clnt->lock);
	if (clnt->magic != MAGIC_NUMBER) {
		status = AIPC_REPLY_SYSTEM_ERROR;
		goto done;
	}

	// check whether the binding is successful or not
	if( clnt->hdr.xprt.server_port == 0 ) {	//binding was failed before
		bind_status = send_binder_msg(clnt);	//binding again
		if ( bind_status == AMBA_IPC_REPLY_SUCCESS ) {
			if(clnt_init_connection(clnt) == AMBA_IPC_CRC_ERROR) {
				status = AMBA_IPC_CRC_ERROR;
				goto done;
			}
		}
		else {
			if (bind_status == AMBA_IPC_REPLY_TIMEOUT) {
				printf("%s Binding svc program timeout.\n", __func__);
			}
			else if (bind_status == AMBA_IPC_REPLY_PROG_UNAVAIL) {
				printf("%s Can not find svc for program id %d.\n", __func__, clnt->hdr.msg.u.call.prog);
			}
			else {	//still can not find the server.
				printf("%s Other binding problem.\n", __func__);
				bind_status = AMBA_IPC_REPLY_SYSTEM_ERROR;
			}
			status = bind_status;
			goto done;
		}
	}
	// send call message to server port
	fill_pkt_header(clnt->pkt, clnt->hdr);
	clnt->pkt->msg.u.call.proc = proc;
	clnt->pkt->xprt.mode = clnt->proc_mode[proc-1];
	memcpy(clnt->pkt->msg.parameters, in, in_len);
	nl_send_pkt(clnt->sock, clnt->pkt, in_len);

	// check if this is a batching call
	if ( (!timeout) || (clnt->proc_mode[proc-1] == AMBA_IPC_ASYNCHRONOUS)) {
		status = AIPC_REPLY_SUCCESS;
		goto done;
	}

	len = nl_recv_pkt(clnt->sock, clnt->pkt, rpc_size, timeout);
	status = clnt->pkt->msg.u.reply.status;
	if(len < 0 && errno == EAGAIN){	//check whether timeout occurs or not
		printf("receiving: timeout error!\n");
		status = AMBA_IPC_REPLY_TIMEOUT;
		errno = 0; //reset errno
	}
	#ifdef RPC_DEBUG
		//calculate all the received packets
		if(status != AMBA_IPC_REPLY_TIMEOUT) {
			clnt->pkt->xprt.lk_to_lu_end = read_timer();
			rpc_record_stats(&clnt->pkt->xprt, clnt->proc_mode[proc-1]);
		}
	#endif
	if (status == AIPC_REPLY_SUCCESS) {
		if (out_len > len) out_len = len;
		if (out){
			memcpy(out, clnt->pkt->msg.parameters, out_len);
		}
	}

done:
	pthread_mutex_unlock(&clnt->lock);
	return -status;
}
