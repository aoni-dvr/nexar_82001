#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "ambasocksvc.h"


static void cleanup(void)
{
	SocketSvc_release();
}

static void signalHandlerShutdown(int sig)
{
	printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
	exit(0);
}

static void dump_data(char *data, int length)
{
	int i;

	printf("\n");
	printf("[SVC] data length: %d\n",length);
	for (i=0; i<length; i++) {
		printf("%02x ",data[i]);
		if((i&0xf)==0){
			if(i!=0) {
				printf("\n");
			}
		}
	}
	printf("\n");
}

static void socket_recever(void)
{
	int sender_sd;
	char *ReqBuf = NULL;
	int ReqBufLen = 2048;
	int read_len = 0, rval = 0;
	unsigned char fatal_err = 0;

	ReqBuf = (char *)malloc(ReqBufLen * sizeof(char));
	if(ReqBuf==NULL){
		fprintf(stderr, "%s: Fail to malloc Reqbuf!\n",__FUNCTION__);
		exit(1);
	}

	while(!fatal_err) {
		read_len = SocketSvc_waitRequest(&sender_sd, ReqBuf, ReqBufLen);
		if(read_len<0) {
			if(read_len == -88) {//client disconnect
				printf("%s: client %d disconnected!\n",__FUNCTION__,sender_sd);
				continue;
			}
			fprintf(stderr, "%s: Fail to do SocketSvc_waitRequest()! %d\n",__FUNCTION__,read_len);
			fatal_err = 1;
			break;
		} else if(read_len == 0) { //new client
			printf("%s: new client %d!\n",__FUNCTION__,sender_sd);
			continue;
		} else { //got data
			printf("SVC got \"%s\"\n",ReqBuf);
			dump_data(ReqBuf,read_len);

			rval = SocketSvc_Send(sender_sd, ReqBuf, read_len);
			if(rval<0){
				printf("[SVC] Fail to do SocketSvc_Send()\n");
				fatal_err = 1;
				break;
			}
		}
	}

	fprintf(stderr, "%s: Exit!!(f=%u)\n",__FUNCTION__,fatal_err);
}

void show_usage(char *pname)
{
	printf("usage: %s <port> <client_amount> <queue_length>\n",pname);
}

int main (int argc, char *argv[])
{
	int rval;
	int port, max_clnts, max_qlen;

	if(argc<4) {
		show_usage(argv[0]);
		return -1;
	}

	port = atoi(argv[1]);
	max_clnts = atoi(argv[2]);
	max_qlen = atoi(argv[3]);

	rval = SocketSvc_init(port, max_clnts, max_qlen);
	if(rval < 0) {
		fprintf(stderr, "@@ Fail to do SocketSvc_init!\n");
		return -1;
	}

	atexit(cleanup);
	/* Allow ourselves to be shut down gracefully by a signal */
	signal(SIGTERM, signalHandlerShutdown);
	signal(SIGHUP, signalHandlerShutdown);
	signal(SIGUSR1, signalHandlerShutdown);
	signal(SIGQUIT, signalHandlerShutdown);
	signal(SIGINT, signalHandlerShutdown);
	signal(SIGKILL, signalHandlerShutdown);

	socket_recever();

	return 0;
}
