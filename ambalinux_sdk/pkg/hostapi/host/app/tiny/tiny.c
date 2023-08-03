#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include "AmbaHost_api.h"

#define CONTROL(x) ((x)&0x1F)

#define TEST_LOG_FROM_FILE_ID

#ifdef  TEST_LOG_FROM_FILE_ID
static void *hostlog_entry(void *arg)
{
	int rfd = *(int*)arg;
	fd_set fds;
	char buf[256];

	while (1) {
		FD_ZERO(&fds);
		FD_SET(rfd, &fds);
		select(rfd+1, &fds, NULL, NULL, NULL);
		if (FD_ISSET(rfd, &fds)) {
			int nBytes = read(rfd, buf, sizeof(buf));
			if (nBytes <= 0) {
				break;
			}
			printf("[HOSTAPI]: %s", buf);
		}
	}

	return NULL;
}
#else
static void log_host_output(int pid, const char *log, int size)
{
	assert(pid == HOSTAPI_LIBRARY_PRINTER_ID);
	printf("[HOSTAPI]: %s", log);
}
#endif

static void log_target_output(int pid, const char *log, int size)
{
	int i;

	if (log == NULL) {
		/* remote end closed connection */
		exit(0);
	}

	for (i = 0; i < size; i++)
		putchar(log[i]);
	fflush(0);
}

static void* ishell_entry(void *arg)
{
	struct termios tin, traw;
	int sid, cid = *(int *)arg;
	char buf[256];
	fd_set fds;

	AmbaHost_get_addr(cid, buf, sizeof(buf));
	printf("Connecting to %s..., escape char is '^]'.\n", buf);

	/* set terminal to raw mode */
	if (tcgetattr(STDIN_FILENO, &tin) < 0) {
		printf("Can't get tty settings\n");
	}
	memcpy(&traw, &tin, sizeof(tin));
	cfmakeraw(&traw);
	tcsetattr(STDIN_FILENO, TCSANOW, &traw);

	sid = AmbaHost_shell_init(cid, 0);
#ifdef TEST_LOG_FROM_FILE_ID
	int rfd = AmbaHost_shell_open(sid, NULL);
	while (1) {
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		FD_SET(rfd, &fds);
		select(rfd+1, &fds, NULL, NULL, NULL);
		if (FD_ISSET(STDIN_FILENO, &fds)) {
			char c = getc(stdin);
			if (c == CONTROL(']'))
				break;
			AmbaHost_shell_sendstring(sid, &c, 1);
		}
		if (FD_ISSET(rfd, &fds)) {
			int  i, nBytes;
			nBytes = read(rfd, buf, sizeof(buf));
			if (nBytes <= 0) {
				break;
			}
			for (i = 0; i < nBytes; i++) {
				putchar(buf[i]);
			}
			fflush(0);
		}
	}
#else
	AmbaHost_shell_open(sid, log_target_output);
	while (1) {
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		select(STDIN_FILENO+1, &fds, NULL, NULL, NULL);
		if (FD_ISSET(STDIN_FILENO, &fds)) {
			char c = getc(stdin);
			if (c == CONTROL(']'))
				break;
			AmbaHost_shell_sendstring(sid, &c, 1);
		}
	}
#endif

	AmbaHost_shell_close(sid);
	AmbaHost_shell_fini(sid);

	if (tcsetattr(STDIN_FILENO, TCSANOW, &tin) < 0) {
		printf("Can't restore tty settings\n");
	}

	printf("Interactive shell closed\n");
	return NULL;
}

int main(int argc, char **argv)
{
	int ret, i, cid0, cid1;
	unsigned char buf[4096];
	pthread_t ishell_thread;

	memset(buf, 0xab, sizeof(buf));

#ifdef  TEST_LOG_FROM_FILE_ID
	pthread_t hostlog_thread;
	int rfs = AmbaHost_init(0, NULL);
	pthread_create(&hostlog_thread, NULL, hostlog_entry, &rfs);
	ret = rfs;
#else
	ret = AmbaHost_init(0, log_host_output);
#endif
	if (ret < 0) {
		printf("AmbaHost_init fails with err %d\n", ret);
		exit(-1);
	}

	/* open a connection to target */
	cid0 = AmbaHost_connection_open(argv[1], 0);
	if (cid0 <= AMBA_HOST_ERR_OK) {
		printf("AmbaHost_connect fails with err %d\n", cid0);
		exit(-1);
	}

	/* open another connection to the same target */
	cid1 = AmbaHost_connection_open(argv[1], 0);
	if (cid1 <= AMBA_HOST_ERR_OK) {
		printf("AmbaHost_connect fails with err %d\n", cid1);
		exit(-1);
	}

	ret = AmbaHost_loopback(1234, "test");
	if (ret != AMBA_HOST_ERR_INVALID_CID) {
		printf("Can't detect invalid cid\n");
		exit(-1);
	}

	/* check if loopback works, this should be the simplest test */
	AmbaHost_loopback(cid0, "Hello world 0");
	AmbaHost_loopback(cid1, "Hello world 1");

	/* check memory read/write */
	ret = AmbaHost_write_mem(cid1, buf, 0x30000000, sizeof(buf));
	printf("AmbaHost_write_mem returns %d\n", ret);
	memset(buf, 0x00, sizeof(buf));
	ret = AmbaHost_read_mem(cid1, buf, 0x30000000, sizeof(buf));
	printf("AmbaHost_read_mem returns %d\n", ret);
	for (i = 0; i < sizeof(buf); i++) {
		if (buf[i] != 0xab) {
			printf("\twrong data %X @ %d!!!\n", buf[i], i);
			break;
		}
	}

	/* check file copy */
	ret = AmbaHost_push_file(cid0,
			  "/tmp/mnt/test_target.bin",
			  "/pub/jli/ambacv.ko");
	printf("push file returns %d\n", ret);
	ret = AmbaHost_pull_file(cid0,
			  "/pub/jli/test_host.bin",
			  "/tmp/mnt/test_target.bin");
	printf("pull file returns %d\n", ret);

	/* check command execution */
	AmbaHost_exec(cid0, log_target_output, "date");
	AmbaHost_exec(cid0, log_target_output, "cd /tmp/mnt");
	AmbaHost_exec(cid0, log_target_output, "ls");

	/* run interactive shell */
	pthread_create(&ishell_thread, NULL, ishell_entry, &cid0);
	pthread_join(ishell_thread, NULL);

	AmbaHost_connection_close(cid0);
	AmbaHost_connection_close(cid1);
	AmbaHost_fini();

	return 0;
}
