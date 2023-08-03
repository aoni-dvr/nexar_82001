#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdarg.h>
#include "AmbaTest_msg.h"
#include "ambatestc.h"

int connect_to_server(const char *server_addr, int server_port)
{
	int fd, err;
	struct sockaddr_in serverAddr;
        struct timeval timeout = { .tv_sec = 1, };

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                        sizeof(timeout)) < 0) {
                printf("setsockopt failed\n");
                exit(-1);
        }

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(server_port);
	inet_aton(server_addr, &serverAddr.sin_addr);

	err = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (err != 0) {
		printf("No route to test server to %s:%d, exit!\n",
                       server_addr, server_port);
		exit(-1);
	}

        printf("Connected to test server %s:%d\n", server_addr, server_port);
        return fd;
}

const char *ambatest_strerror(int error)
{
        char *str;

        switch (error) {
        case AT_ERR_OK:
                str = "succeed";
                break;
        case AT_ERR_WRONG_STATUS:
                str = "client in wrong status";
                break;
        case AT_ERR_DEV_NOTFOUND:
                str = "device not found\n";
                break;
        case AT_ERR_CLIENT_DOWN:
                str = "client went offline\n";
                break;
        case AT_ERR_REQ_INVALID:
                str = "request invalid\n";
                break;
        case AT_ERR_FILE_NOTFOUND:
                str = "file not found on device\n";
                break;
        default:
                str = "unkown error";
                break;
        }

        return str;
}

int log_msg(int level, const char *fmt, ...)
{
        va_list args;
        int ret;
        time_t rawtime;
        struct tm * timeinfo;
        char header [80];

        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime(header, sizeof(header), "%H:%M:%S ", timeinfo);
        printf("%s", header);

        va_start(args, fmt);
        ret = vprintf(fmt, args);
        va_end(args);

        return ret;
}

