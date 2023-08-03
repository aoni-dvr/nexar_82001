#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_AMBARPC 25
#define MAX_PAYLOAD    1024

struct sockaddr_nl src_addr;
struct sockaddr_nl dst_addr;
struct nlmsghdr*   nlh = NULL;
struct msghdr      msg;
struct iovec       iov;
int                sock_fd;

int main(int argc, char *argv[])
{
	int port, len = sizeof(src_addr);
	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_AMBARPC);
 
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	//src_addr.nl_pid = 0x1234;
	bind(sock_fd, (struct sockaddr*)&src_addr, len);
	getsockname(sock_fd, &src_addr, &len);
	port = src_addr.nl_pid;
	printf("nl_pid is %d\n", port);

	memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.nl_family = AF_NETLINK;
    
	nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = port;
	nlh->nlmsg_flags = 0;
	strcpy(NLMSG_DATA(nlh), "Hello kernel!");

	iov.iov_base = (void*)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void*)&dst_addr;
	msg.msg_namelen = sizeof(dst_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf("Sending message to kernel\n");
	sendmsg(sock_fd, &msg, 0);
    
	printf("Waiting for kernel to reply\n");
	recvmsg(sock_fd, &msg, 0);
	printf("Received: %s\n", (char*)NLMSG_DATA(nlh));

	close(sock_fd);
	return 0;
}
