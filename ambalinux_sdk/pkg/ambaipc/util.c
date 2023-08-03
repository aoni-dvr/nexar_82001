#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "aipc_user.h"
#include <errno.h>


#ifdef RPC_DEBUG
AMBA_RPC_STATISTIC_s *rpc_stat;
int rpc_debug = 0;
#define RPC_MAGIC 0xD0
#define READ_IOCTL _IOR(RPC_MAGIC, 0, int)
#define WRITE_IOCTL _IOW(RPC_MAGIC, 1, int)
/*
 * get current time
 *
 */
unsigned int read_timer(){
	int fd;
	char buf[20];
	unsigned int cur_time;

	if(rpc_debug == 0)
		return 0;

	if((fd = open("/dev/rpc_profile", O_RDWR)) < 0) {
		printf("open rpc_file failed!\n");
		return 0;
	}
	memset(buf, 0x0, sizeof(buf));
	if(ioctl(fd, READ_IOCTL, buf) < 0) {
		printf("%s read error\n", __func__);
	}
	cur_time = (unsigned int) atoll(buf);
	close(fd);
	return cur_time;
}
/*
 * calculate time
 *
 */
static unsigned int calc_timer_diff(unsigned int start, unsigned int end){
	unsigned int diff;
	if(end <= start) {
		diff = start - end;
	}
	else{
		diff = 0xFFFFFFFF - end + 1 + start;
	}
	return diff;
}
/*
 * write to the shared memory by rpmsg_rpc_timer_proc_write
 * condition:  1: add diff to the value in adder, 2: identify whether diff is larger
 *			   3: identify whther diff is smaller
 */
static void rpc_proc_write(int condition, unsigned int addr, unsigned int diff){
	int fd;
	char buf[200];

	if(rpc_debug == 0)
		return;

	if((fd = open("/dev/rpc_profile", O_RDWR)) < 0) {
		printf("open rpc_file failed!\n");
		return;
	}

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d %u %u", condition, addr, diff);
	if(ioctl(fd, WRITE_IOCTL, buf) < 0) {
		printf("%s write failed\n", __func__);
	}

	close(fd);
}

void rpc_proc_open(void)
{
	char buf[10];
	int len;

	FILE *rpc_file = fopen("/proc/ambarella/rpc_debug", "r");
	if(rpc_file == NULL){
		printf("open rpc_debug failed!\n");
		return;
	}
	memset(buf, 0x0, sizeof(buf));
	len = fread(buf, 1, sizeof(buf), rpc_file);
	if(len < 0) {
		printf("read error!\n");
	}
	rpc_debug = atoi(buf);
	fclose(rpc_file);
}
/*
 * record the rpc statistics
 *
 */
void rpc_record_stats(struct aipc_xprt *xprt, AMBA_IPC_COMMUICATION_MODE_e mode){
	unsigned int diff;

	if(rpc_debug == 0)
		return;

		/*to assign the starting address to rpc_stat */
	rpc_stat = (AMBA_RPC_STATISTIC_s *)0; 	//using starting addr 0, and rpc_proc_write will help to translate the addr
	/* Threax RPC time */
	if( xprt->client_addr == AIPC_HOST_LINUX && xprt->server_addr == AIPC_HOST_LINUX ){
		return;	// the messages between ipcbind and svc are not profilied
	}
	diff = calc_timer_diff(xprt->tx_rpc_send_start, xprt->tx_rpc_send_end);
	rpc_proc_write(1, (unsigned int) &rpc_stat->TxRpcSendTime, diff);

	/*  ThreadX to Linux user space */
	diff = calc_timer_diff(xprt->tx_rpc_send_start, xprt->lk_to_lu_end);
	rpc_proc_write(1, (unsigned int) &rpc_stat->TxToLuTime, diff);
	rpc_proc_write(2, (unsigned int) &rpc_stat->MaxTxToLuTime, diff); // identify if(diff > rpc_stat->MaxLxToLuTime)
	rpc_proc_write(3, (unsigned int) &rpc_stat->MinTxToLuTime, diff); // identify if(diff < rpc_stat->MinLxToLuTime)
	rpc_proc_write(1, (unsigned int) &rpc_stat->TxToLuCount, 1);	// rpc_stat->TxToLuCount++

	/*  Linux kernel space to Linux user space */
	diff = calc_timer_diff(xprt->lk_to_lu_start, xprt->lk_to_lu_end);
	rpc_proc_write(1, (unsigned int) &rpc_stat->LkToLuTime, diff);
	rpc_proc_write(2, (unsigned int) &rpc_stat->MaxLkToLuTime, diff); // identify if (diff > rpc_stat->MaxLxToLuTime)
   	rpc_proc_write(3, (unsigned int) &rpc_stat->MinLkToLuTime, diff); // identify if (diff < rpc_stat->MinLxToLuTime)

	if(mode == AMBA_IPC_SYNCHRONOUS){
		diff = calc_timer_diff(xprt->tx_rpc_send_start, xprt->lk_to_lu_end);
		rpc_proc_write(1, (unsigned int) &rpc_stat->RoundTripTime, diff);	// calculate the roundtrip time
		rpc_proc_write(1, (unsigned int) &rpc_stat->SynPktCount, 1);	// rpc_stat->SynPktCount++
		/* Count twice for one synchronous packet. */
	}
	else if(mode == AMBA_IPC_ASYNCHRONOUS){
		diff = calc_timer_diff(xprt->tx_rpc_send_start, xprt->lk_to_lu_end);
		rpc_proc_write(1, (unsigned int) &rpc_stat->OneWayTime, diff);	// calculate the deliver time one way
		rpc_proc_write(1, (unsigned int) &rpc_stat->AsynPktCount, 1);	// rpc_stat->Asyn
	}
}
#endif


/*
 * get the description of error code
 *
 */
const char *ambaipc_strerror(int error)
{
	static const char *errstr[] = {
		"success",
		"rpc program is unavailable",
		"rpc parameter is invalid",
		"system error",
		"timeout",
		"invalid client id",
		"the svc is not registered",
		"the svc is double registered",
		"the memory allocation for svc is failed",
		"ambalink is not ready",
		"crc values of rpc headers are inconsistent",
	};
	if(abs(error) < AMBA_IPC_STATUS_NUM) {
		return errstr[abs(error)];
	}
	else {
		return "no such error code";
	}
}

/*
 * create a new NL socket, returns socket fd
 *
 */
int nl_open_socket(unsigned int *out_port)
{
	struct sockaddr_nl addr;
	int sock_fd, ret;
	socklen_t len = sizeof(struct sockaddr_nl);

	memset(&addr, 0, len);
	addr.nl_family = AF_NETLINK;
	ret = socket(PF_NETLINK, SOCK_RAW, NL_PROTO_AMBAIPC);
	if (ret < 0) {
		printf("can't open nl socket. ret=%d\n",ret);
		perror("socket()");
		return ret;
	}
	sock_fd = ret;

	ret = bind(sock_fd, (struct sockaddr*)&addr, len);
	if (ret) {
		printf("can't create new socket\n");
		close(sock_fd);
		return ret;
	}

	if (out_port) {
		getsockname(sock_fd, (struct sockaddr*)&addr, &len);
		*out_port = addr.nl_pid;
	}

	return sock_fd;
}

/*
 * close a NL socket
 */
int nl_close_socket(int sock)
{
	return close(sock);
}

/*
 * wrap a pakcet in NLMSG and sent to kernel
 */
int nl_send_pkt(int sock, struct aipc_pkt *pkt, int size)
{
	struct nlmsghdr *nlh = (struct nlmsghdr*)((void*)pkt - NLMSG_HDRLEN);
	int len = size + NLMSG_HDRLEN + AIPC_HDRLEN;
#ifdef RPC_DEBUG
	rpc_proc_write(4, (unsigned int) &rpc_stat->LuLastInjectTime, (unsigned int)&rpc_stat->LuTotalInjectTime);
	pkt->xprt.lu_to_lk_start = read_timer();
#endif
	nlh->nlmsg_len = len;
	return send(sock, nlh, len, 0);
}

/*
 * receive a packet
 * timeout: millisecond
 */
int nl_recv_pkt(int sock, struct aipc_pkt *pkt, int size, unsigned int timeout)
{
	struct nlmsghdr *nlh = (struct nlmsghdr*)((void*)pkt - NLMSG_HDRLEN);
	int len = size + NLMSG_HDRLEN + AIPC_HDRLEN;
	struct timeval tout;

	if(timeout != 0xFFFFFFFF){	//here we think 0xFFFFFFFF means blocking infinitely
		timeout = timeout*1000;
		tout.tv_sec = (time_t) (timeout/1000000);
		tout.tv_usec = (suseconds_t) (timeout%1000000);	//Note: tv_usec cannot exceed 10^6
		if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tout, sizeof(tout)) < 0){
			printf("setsockopt failed\n");
		}
	}
	len = recv(sock, nlh, len, 0);	//blocking recv if setsockopt does not invoke

	if (len > 0)
	{
		len -= NLMSG_HDRLEN + AIPC_HDRLEN;
	}
	return len;
}

/*
 * allocate a aipc_pkt with payload length @len
 */
struct aipc_pkt* nl_alloc_pkt(int len)
{
	struct nlmsghdr *nlh;
	struct aipc_pkt *pkt = NULL;

	len += AIPC_HDRLEN;
	nlh = (struct nlmsghdr*) calloc(1, NLMSG_SPACE(len));
	if (nlh)
		pkt = (struct aipc_pkt*)NLMSG_DATA(nlh);

	return pkt;
}

/*
 * free a aipc_pkt
 */
void nl_free_pkt(struct aipc_pkt *pkt)
{
	void *buf = pkt;
	free(buf - NLMSG_HDRLEN);
}
