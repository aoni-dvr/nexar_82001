#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static int sock_sd;

static void cleanup(void)
{
	if(sock_sd>0){
		close(sock_sd);
	}
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
	printf("[CLNT]data length: %d\n",length);
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
	char *dataBuf = NULL;
	int dataBufLen = 2048;
	int read_len = 0;

	dataBuf = (char *)malloc(dataBufLen * sizeof(char));
	if(dataBuf==NULL){
		fprintf(stderr, "%s: Fail to malloc dataBuf!\n",__FUNCTION__);
		exit(1);
	}

	read_len = recv(sock_sd,dataBuf,dataBufLen,0);
	if(read_len<=0) {
		fprintf(stderr, "%s: Fail to do recv()! %d\n",__FUNCTION__,read_len);
	} else { //got data
		printf("Client got \"%s\"\n",dataBuf);
		dump_data(dataBuf,read_len);
	}
}

static int sock_init(int *sock_des, int port)
{
	struct sockaddr_in server;
	int sock_addrlen;
	int sock;
	int local_port;

	*sock_des = -1;

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket\n");
		return -1;
	}
	printf("Socket created\n");

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		close(sock);
		return -1;
	}
	printf("Connected\n");

	sock_addrlen = sizeof(server);
	getsockname(sock, (struct sockaddr*)&server, (socklen_t*)&sock_addrlen);
	local_port = ntohs(server.sin_port);
	printf("%s: Local port is %d\n", __FUNCTION__, local_port);

	*sock_des = sock;
	return  local_port;
}

void show_usage(char *pname)
{
	printf("usage: %s <port> <test string>\n",pname);
}

int main (int argc, char *argv[])
{
	int rval, datalen;
	int port, local_port;

	if(argc<3) {
		show_usage(argv[0]);
		return -1;
	}

	port = atoi(argv[1]);

	local_port = sock_init(&sock_sd, port);
	if(local_port < 0){
		printf("%s: sock_init fail!\n",__FUNCTION__);
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

	while(1) {
		datalen = strlen(argv[2]);
		rval = send(sock_sd, argv[2], datalen, MSG_NOSIGNAL);
			if(rval != datalen){
				perror("SocketSvc_Send");
				rval = -1;
				break;
			}
		socket_recever();
		sleep(5);
	}

	return 0;
}
