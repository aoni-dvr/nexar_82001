
//#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <syslog.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <signal.h>
#include <getopt.h>
#if 1
#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_PT.h"
#include "AmbaIPC_RpcProg_R_PT.h"
#endif

int grantpt(int fd);
int unlockpt(int fd);

/* /dev/ptmx */
static int fdm;

#if defined(AMBA_RPC_PROG_LU_PT_PROG_ID)&&defined(AMBA_RPC_PROG_LU_PT_VER)
void AmbaRpcProg_LU_PT_Svc(const char *pStr, AMBA_IPC_SVC_RESULT_s *pRet)
{
	/* user input from threadx, assume NULL-terminated */
	write(fdm, pStr, strlen(pStr));

	pRet->Status = AMBA_IPC_REPLY_SUCCESS;
	pRet->Mode = AMBA_IPC_ASYNCHRONOUS;
}

AMBA_IPC_REPLY_STATUS_e AmbaRpcProg_R_PT_Clnt(const char *pStr, int *pResult, CLIENT_ID_t Clnt)
{
	AMBA_IPC_REPLY_STATUS_e status;
	status = ambaipc_clnt_call(Clnt, AMBA_RPC_PROG_R_PT, (void *) pStr, strlen(pStr)+1, 0, 0, 0);
	return status;
}

static int output_to_threadx(const char *output)
{
	int ret;
	CLIENT_ID_t clnt;

	clnt = ambaipc_clnt_create(1, AMBA_RPC_PROG_R_PT_PROG_ID, AMBA_RPC_PROG_R_PT_VER);
	if (!clnt) {
		syslog(LOG_ERR, "ambaipc_clnt_create failed");
		return -1;
	}

	ret = AmbaRpcProg_R_PT_Clnt(output, NULL, clnt);
	ambaipc_clnt_destroy(clnt);

	return ret;
}

static void sg_svc_unregister(int sig, siginfo_t *siginfo, void *context)
{
	syslog(LOG_ERR, "got signal %d", sig);
	ambaipc_svc_unregister(AMBA_RPC_PROG_LU_PT_PROG_ID, AMBA_RPC_PROG_LU_PT_VER);
	exit(0);
}
#endif

static void print_usage (void)
{
	fprintf (stderr, "	-f	--foreground	do not fork to allow stdin \n");
	exit(1);
}

int
main(int argc, char *argv[])
{
	int ret;
	int fds;
	int opt, foreground = 0;
	struct sigaction act;
	const char* const short_options = "fh";
	const struct option long_options[] = {
		{ "foreground", 0, NULL, 'f' },
		{ "help", 0, NULL, 'h' },
	};
	char *ptsname(int fd);

	do {
		opt = getopt_long (argc, argv, short_options, long_options, NULL);
		switch (opt) {
			case 'f':
				foreground = 1;
				break;

			case 'h':
				print_usage();
				opt = -1;
				break;

			case -1:
				/* Done with options */
				break;

			default:
				abort();
				break;
		}
	} while (opt != -1);

	if (0 == foreground) {
		daemon(0, 1);
	}

	fdm = open("/dev/ptmx", O_RDWR);
	if (fdm < 0) {
		fprintf(stderr, "err open /dev/ptmx\n");
		return -1;
	}

#if defined(AMBA_RPC_PROG_LU_PT_PROG_ID)&&defined(AMBA_RPC_PROG_LU_PT_VER)
	AMBA_IPC_PROG_INFO_s prog_info[0];

	prog_info->ProcNum = 1;
	prog_info->pProcInfo = malloc(prog_info->ProcNum*sizeof(AMBA_IPC_PROC_s));
	prog_info->pProcInfo[0].Mode = AMBA_IPC_ASYNCHRONOUS;
	prog_info->pProcInfo[0].Proc = (AMBA_IPC_PROC_f) &AmbaRpcProg_LU_PT_Svc;

	if (0 != (ret = ambaipc_svc_register(AMBA_RPC_PROG_LU_PT_PROG_ID, AMBA_RPC_PROG_LU_PT_VER, "PT_svc", prog_info, 1))) {
		fprintf(stderr, "ambaipc_svc_register returned %d\n", ret);
		ambaipc_svc_unregister(AMBA_RPC_PROG_LU_PT_PROG_ID, AMBA_RPC_PROG_LU_PT_VER);
		ambaipc_svc_register(AMBA_RPC_PROG_LU_PT_PROG_ID, AMBA_RPC_PROG_LU_PT_VER, "PT_svc", prog_info, 1);
	}

	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sg_svc_unregister;
	act.sa_flags = SA_SIGINFO;
	ret = sigaction(SIGTERM, &act, NULL);
	if(ret < 0){
		printf("sigaction error: SIGTERM\n");
	}
	signal(SIGINT, SIG_IGN);

#endif
	ret = grantpt(fdm);
	ret = unlockpt(fdm);

	/* pseudo terminal slave */
	fds = open(ptsname(fdm), O_RDWR);

	if (fork()) {
		fd_set fd_in;
		struct timeval tv;
		char input[150];
		char output[512];

		/* Parent */
		close(fds);

		FD_ZERO(&fd_in);
		FD_SET(0, &fd_in);
		FD_SET(fdm, &fd_in);

		/* pseudo terminal master */
		while (1)
		{
			tv.tv_sec = 600;
			tv.tv_usec = 0;

			FD_ZERO(&fd_in);
			FD_SET(0, &fd_in);
			FD_SET(fdm, &fd_in);

			if (select(fdm + 1, &fd_in, NULL, NULL, &tv) <= 0) {

			}

			if (foreground && FD_ISSET(0, &fd_in)) {
				/* user input from linux shell */
				ret = read(0, input, sizeof(input) - 1);
				if (ret < 0) {
					fprintf(stderr, "err read from stdin\n");
					exit(1);
				}

				write(fdm, input, ret);
			}

			if (FD_ISSET(fdm, &fd_in)) {
				ret = read(fdm, output, sizeof(output) - 1);
				if (ret < 0) {
					fprintf(stderr, "err read from child\n");
					exit(1);
				}

				output[ret] = '\0';

				/* output to linux user shell */
				write(1, output, ret);

#if defined(AMBA_RPC_PROG_LU_PT_PROG_ID)&&defined(AMBA_RPC_PROG_LU_PT_VER)
				output_to_threadx(output);
#endif
			}
		}

		return 0;
	} else {
//		char str2[150];
		struct termios slave_orig_term_settings; // Saved terminal settings
		struct termios new_term_settings; // Current terminal settings

		/* Child */
		close(fdm);

		/* new stdin from fdm */
		/* new stdout to fdm */
		dup2(fds, 0);
		dup2(fds, 1);
		dup2(fds, 2);
		close(fds);

		ret = tcgetattr(fds, &slave_orig_term_settings);
		new_term_settings = slave_orig_term_settings;
		cfmakeraw(&new_term_settings);
		tcsetattr(fds, TCSANOW, &new_term_settings);

		setsid();
		ioctl(0, TIOCSCTTY, 1);
		execl("/bin/sh", "/bin/sh", NULL);

//		/* won't be executed */
//		while (1)
//		{
//			ret = read(fds, str2, sizeof(str2) - 1);
//
//			if (ret > 0)
//			{
//				printf("read from master: %s", str2);
//			} else
//				break;
//		}

		return 0;
	}
}
