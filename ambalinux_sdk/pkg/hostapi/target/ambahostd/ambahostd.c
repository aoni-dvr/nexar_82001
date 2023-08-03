#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "AmbaHost_api.h"
#include "AmbaTest_msg.h"

static atd_spec_t devinfo = {
        .device_hw    = "CV22BUB",
        .device_os    = "AMBALINK",
};

int register_once(
        const char *addr,
        int port,
        const atd_spec_t *info)
{
	int fd, err, flag;
        uint32_t msg_type;
	struct sockaddr_in serverAddr;

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(fd >= 0);

	/* open connection to test server */
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_aton(addr, &serverAddr.sin_addr);
	err = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (err != 0) {
		printf("No route to test server to %s:%d, ", addr, port);
                printf("skip registration!\n\r");
                sleep(1);
                close(fd);
		return AMBA_HOST_ERR_NO_CONNECTION;
	}

        /* send registration message */
        printf("Register Device with test server %s\n", addr);
        msg_type =  AMBA_TEST_MSG_REGISTER;
        send(fd, &msg_type, sizeof(msg_type), 0);
        send(fd, info, sizeof(*info), 0);

        /* keep heartbeat with server */
        while (1) {
                err = recv(fd, &flag, sizeof(flag), 0);
                if (err == -1 || flag == HEARTBEAT_SHUTDOWN) {
                        /* Connection losts or server sends "close" flag */
                        /* Either way we should finish this reigration */
                        break;
                }
                send(fd, &flag, sizeof(flag), 0);
        }
        shutdown(fd, 2);
        close(fd);

        return AMBA_HOST_ERR_OK;
}

void register_with_test_server(
        const char *addr,
        int port,
        const atd_spec_t *info)
{
        while (1) {
                register_once(addr, port, info);
        }
}

int main(int argc, char **argv)
{
	int ret, c, daemon_mode = 0, port = AMBATEST_DEVICE_PORT;
        char *test_server_addr = NULL;

	optind = 1;
	while ((c = getopt(argc, argv, "t:o:s:d")) != -1) {
		switch (c) {
                case 't':
                        strcpy(devinfo.device_hw, optarg);
                        break;
                case 'o':
                        strcpy(devinfo.device_os, optarg);
                        break;
                case 's':
                        test_server_addr = optarg;
                        break;
		case 'd':
			daemon_mode = 1;
			break;
		}
	}

	if (daemon_mode) {
		daemon(0, 0);
	}

	ret = AmbaHost_init(0, NULL);
	if (ret != AMBA_HOST_ERR_OK) {
		printf("AmbaHost_init fails with err %d\n", ret);
		exit(-1);
	}

        /* register this device with test server */
        if (test_server_addr != NULL) {
                register_with_test_server(test_server_addr, port, &devinfo);
        }

        sleep(0xFFFFFFFF);
}
