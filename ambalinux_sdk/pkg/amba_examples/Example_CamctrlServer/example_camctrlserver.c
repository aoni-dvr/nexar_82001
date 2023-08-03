#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <getopt.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_CamCtrl.h"
#include "CamCtrlMsgDef.h"

#define SERVER_PORT 11009
#define NG (-1)
#define OK (0)

#define PrintFunc printf

static int sockfd = -1;
static int clientfd = -1;

int CamCtrl_ProcessRequest(unsigned int Msg, unsigned char *Param, unsigned int ParamSize)
{
    AMBA_IPC_REPLY_STATUS_e status;
    RT_CV_Request_s Arg = {0};
    int Result = OK;
    CLIENT_ID_t Clnt;

    if (ParamSize > CV_MAX_PARAM_SIZE) {
        PrintFunc("%s: ParamSize(%d) reach limit(%d)!!\n",__FUNCTION__, ParamSize, CV_MAX_PARAM_SIZE);
        return NG;
    }

    Clnt = ambaipc_clnt_create(1, RT_CAMCTRL_PROG_ID, RT_CAMCTRL_VER);
    if (!Clnt) {
        PrintFunc("ambaipc_clnt_create failed\n");
        return NG;
    }

    Arg.Msg = Msg;
    Arg.ParamSize = ParamSize;
    memcpy(Arg.Param, Param, ParamSize);

    status = ambaipc_clnt_call(Clnt, RT_CAMCTRL_FUNC_REQUEST,
						       &Arg, sizeof(RT_CV_Request_s),
						       &Result, sizeof(int),
						       RT_CAMCTRL_DEFULT_TIMEOUT);

    if(status != AMBA_IPC_REPLY_SUCCESS) {
        Result = NG;
    }

    ambaipc_clnt_destroy(Clnt);

    return Result;
}

/**
 * Clean up resource when program exit.
 */
static void cleanup(void)
{
    if (clientfd != -1) {
        close(clientfd);
    }
    if (sockfd != -1) {
        close(sockfd);
    }
}

/**
 * Linux signal handler.
 */
static void signalHandlerShutdown(int sig)
{
    PrintFunc("Got signal %d, program exits!\n", sig);
    exit(0);
}

int main()
{
    struct sockaddr_in dest;
    CV_Request_s Request;
    char buf[] = "OK";

    /* create sockett */
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        printf("socket creation failed\n");
        exit(-1);
    }

    /* initialize structure dest */
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERVER_PORT);
    /* this line is different from client */
    dest.sin_addr.s_addr = INADDR_ANY;

    /* Assign a port number to socket */
    if (bind(sockfd, (struct sockaddr*)&dest, sizeof(dest)) == -1) {
        printf("Bind error.\n");
        exit(-1);
    }

    /* make it listen to socket with max 20 connections */
    if (listen(sockfd, 20) == -1) {
        printf("Listen error!\n");
    }

    daemon(0, 1);
    atexit(cleanup);
    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, signalHandlerShutdown);
    signal(SIGHUP, signalHandlerShutdown);
    signal(SIGUSR1, signalHandlerShutdown);
    signal(SIGQUIT, signalHandlerShutdown);
    signal(SIGINT, signalHandlerShutdown);
    signal(SIGKILL, signalHandlerShutdown);

    /* infinity loop -- accepting connection from client forever */
    while(1)
    {
        struct sockaddr_in client_addr;
        int addrlen = sizeof(client_addr);
        int res;

        /* Wait and Accept connection */
        clientfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t *)&addrlen);

        if (clientfd) {
            /* Send message */
            res = recv(clientfd, &Request, sizeof(Request), 0);
            send(clientfd, buf, sizeof(buf), 0);
            while(res > 0) {
                //PrintFunc("receive message from client: Msg = 0x%x, total %d bytes\n", Request.Msg, res);
                CamCtrl_ProcessRequest(Request.Msg, Request.Param, Request.ParamSize);
                res = recv(clientfd, &Request, sizeof(Request), 0);
                send(clientfd, buf, sizeof(buf), 0);
            }

            /* close(client) */
            close(clientfd);
        }
    }

    /* close(server) , but never get here because of the loop */
    close(sockfd);
    return 0;
}


