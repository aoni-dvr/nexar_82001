#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "ambasocksvc.h"

#define DEBUG(...) //printf(...)

#define REBINDSLEEP	6
#define MAXREBIND	2

typedef struct _socket_svc_client_info_s_ {
	int sd;
	unsigned long long port;
} socket_svc_client_info_s ;

static int svc_inited = 0;
static int max_clients = 10;
static int master_sd = 0;
static socket_svc_client_info_s *ClientInfo = NULL;

int SocketSvc_init(unsigned short port, int max_clnts, int max_qlen)
{
	int i, opt = 1;
	struct sockaddr_in address;

	if(svc_inited) {
		printf("%s: aleady inited!\n",__FUNCTION__);
		return 0;
	}

	max_clients = max_clnts;
	ClientInfo = (socket_svc_client_info_s *)malloc(max_clnts*sizeof(socket_svc_client_info_s));
	if(ClientInfo == NULL) {
		perror("create ClientInfo");
		return -1;
	}
	memset(ClientInfo, 0 , max_clnts*sizeof(socket_svc_client_info_s));

	//create a master socket
	master_sd = socket(AF_INET , SOCK_STREAM , 0);
	if(master_sd == 0)
	{
		perror("socket failed");
		return -1;
	}

	//set master socket to allow multiple connections
	if( setsockopt(master_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	//bind the socket to localhost port
	for (i = 1; i <= MAXREBIND; i++) {
		if (bind(master_sd, (struct sockaddr *)&address, sizeof(address)) != -1) {
			break;
		}
		if (MAXREBIND == i) {
			close(master_sd);
			return -2;
		}
		perror((char *) strerror(errno));
		sleep(REBINDSLEEP);
	}

	//try to specify maximum of "max_qlen" pending connections for the master socket
	if (listen(master_sd, max_qlen) < 0)
	{
		perror("listen");
		close(master_sd);
		return -3;
	}
	DEBUG("Listener on port %d \n", port);

	svc_inited = 1;
    return 0;
}

/*
 * return  0 - new client
 *        <0 - error
 *        >0 - Request data lenght
 */
int SocketSvc_waitRequest(int *sender_sd, char *ReqBuf, int ReqBufLen)
{
	int addrlen, sd, max_sd, new_sd, valread, activity, i;
	struct sockaddr_in address;
	fd_set readfds;

	if(svc_inited == 0) {
		printf("%s: SocketSvc is NOT inited!\n",__FUNCTION__);
		return -1;
	}

	addrlen = sizeof(address);

	while(1) {
		FD_ZERO(&readfds);
		FD_SET(master_sd, &readfds);
		max_sd = master_sd;

		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++) {
			sd = ClientInfo[i].sd;

			if(sd > 0) {
				FD_SET(sd, &readfds);

				if(sd > max_sd) {
					max_sd = sd;
				}
			}
		}

		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
		if (activity < 0){
			printf("%s: Fail to do select(), err:%d\n",__FUNCTION__,errno);
			if(errno==EINTR) {
				perror("selec got EINTR");
				continue;
			} else {
				perror("select error");
			}
			return -1;
		}

		//If something happened on the master socket , then its an incoming connection
		if (FD_ISSET(master_sd, &readfds)) {
			if ((new_sd = accept(master_sd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
				perror("accept");
				return -2;
			}

			//inform user of socket number - used in send and receive commands
			DEBUG("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_sd , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++) {
				if( ClientInfo[i].sd == 0 ) {
					ClientInfo[i].sd = new_sd;
					ClientInfo[i].port = ntohs(address.sin_port);
					DEBUG("Adding to list of sockets as %d\n" , i);
					break;
				}
			}
			if(i >= max_clients) {
				printf("Reach max client amount!!\n");
				return -3;
			}
			*sender_sd = new_sd;
			return 0;
		} else { //else its some IO operation on some other socket
			for (i = 0; i < max_clients; i++) {
				sd = ClientInfo[i].sd;
				if (FD_ISSET( sd , &readfds)) {
					//Check if it was for closing , and also read the incoming message
					valread = read(sd , ReqBuf, ReqBufLen);
					if(valread <= 0) {
						DEBUG("%s: Fail to do read(), ret=%d, err:%d\n",__FUNCTION__,valread,errno);
						//perror("read operation: ");

						if((valread<0)&&(errno == EINTR)) { //read again
							continue;
						}

						//Maybe Somebody disconnected , get his details and print
						getpeername(sd, (struct sockaddr*)&address , (socklen_t*)&addrlen);
						DEBUG("Client disconnected: port %d \n" , ntohs(address.sin_port));

						//Close the socket and mark as 0 in list for reuse
						*sender_sd = sd;
						close(sd);
						ClientInfo[i].sd = 0;
						ClientInfo[i].port = 0;
						return -88;
					} else {
						*sender_sd = sd;
						return valread;
					}
				}
			}
			printf("%s[%d]: Cannot find valid client_sd\n",__FUNCTION__,__LINE__);
		}
	}

	return -1;
}

int SocketSvc_Recv(int sd, char *data, int datalen)
{
	int ret = -99, i;

	for (i = 0; i < max_clients; i++) {
		if(sd == ClientInfo[i].sd){
			ret = read(sd , data, datalen);
			if(ret == 0){
				struct sockaddr_in address;
				int addrlen;

				addrlen = sizeof(address);
				//Somebody disconnected , get his details and print
				getpeername(sd, (struct sockaddr*)&address , (socklen_t*)&addrlen);
				DEBUG("%s: Client disconnected: port %d\n",__FUNCTION__,ntohs(address.sin_port));

				//Close the socket and mark as 0 in list for reuse
				close(sd);
				ClientInfo[i].sd = 0;
				ClientInfo[i].port = 0;

				return 0;
			}
		}
	}

	if(ret==-99){
		printf("%s: invalid sd(%d)!\n",__FUNCTION__,sd);
	}

	return ret;
}

int SocketSvc_Send(int sd, char *data, int datalen)
{
	int ret = -99, i;

	for (i = 0; i < max_clients; i++) {
		if(sd == ClientInfo[i].sd){
			ret = send(sd, data, datalen, MSG_NOSIGNAL);
			if(ret != datalen){
				perror("SocketSvc_Send");
				ret = -1;
			}
		}
	}

	if(ret==-99){
		printf("%s: invalid sd(%d)!\n",__FUNCTION__,sd);
	}

	return ret;
}

int SocketSvc_release(void)
{
	int i;

	svc_inited = 0;

	if(ClientInfo!=NULL) {
		for (i = 0; i < max_clients; i++) {
			if(ClientInfo[i].sd != 0){
				shutdown(ClientInfo[i].sd, SHUT_RDWR);
				close(ClientInfo[i].sd);
				ClientInfo[i].sd = 0;
				ClientInfo[i].port = 0;
			}
		}
		free(ClientInfo);
		ClientInfo = NULL;
	}

	if(master_sd!=0){
		close(master_sd);
		master_sd=0;
	}

	return 0;
}

int SocketSvc_QuerySD(unsigned long long client_port)
{
	int i;
	int sd = 0;

	for (i = 0; i < max_clients; i++) {
		if(client_port == ClientInfo[i].port){
			sd = ClientInfo[i].sd;
			break;
		}
	}

	return sd;
}

int SocketSvc_getClntSD(int index)
{
	if(index >= max_clients) {
		return 0;
	}

	return ClientInfo[index].sd;
}

int SocketSvc_getMaxAmount(void)
{
	return max_clients;
}

