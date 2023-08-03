#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "AmbaEventNotifier.h"
#include "AmbaEventDef.h"


static void cleanup(void)
{
	AmbaEventNotify_Release();
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
	printf("data length: %d\n",length);
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

static void cbEvent(unsigned int event, char *param, unsigned int param_len, void *userdata)
{
	switch(event) {
	case EVENTNOTIFIER_RTOSEVENT_CARD_INSERT:
		printf("Got RTOS Card Inserted\n");
		if(param_len<=0){
			printf("Invalid event param, length is %d\n",param_len);
			break;
		}
		printf("%s is inserted\n",param);
		break;
	case EVENTNOTIFIER_RTOSEVENT_CARD_REMOVE:
		printf("Got RTOS Card Removed\n");
		if(param_len<=0){
			printf("Invalid event param, length is %d\n",param_len);
			break;
		}
		printf("%s is removed\n",param);
		break;
	case EVENTNOTIFIER_RTOSEVENT_CARD_FORMAT:
		printf("Got RTOS Card Formatted\n");
		if(param_len<=0){
			printf("Invalid event param, length is %d\n",param_len);
			break;
		}
		printf("%s is formatted\n",param);
		break;
	case EVENTNOTIFIER_RTOSEVENT_FILE_CREATE:
		printf("Got RTOS File Created\n");
		if(param_len<=0){
			printf("Invalid event param, length is %d\n",param_len);
			break;
		}
		printf("%s is created\n",param);
		break;
	case EVENTNOTIFIER_RTOSEVENT_FILE_DELETE:
		printf("Got RTOS File Deleted\n");
		if(param_len<=0){
			printf("Invalid event param, length is %d\n",param_len);
			break;
		}
		printf("%s is deleted\n",param);
		break;
	default:
		printf("%s: unknown event 0x%08x\n",__FUNCTION__,event);
		dump_data(param, param_len);
		break;
	}
	printf("\n");
}

/*
 * Command line arguments.
 */
static struct option longopts[] =
{
	{ "en_fg",	no_argument,	NULL, 'f', },
	{ NULL, 0, NULL, 0, },
};

void ShowUsage(char *p_name)
{
	printf("usage: %s [-f]\n",p_name);
	printf("Paramters:\n");
	printf("\t \"-f\": same as \"en_fg\", to run at fore-ground.\n");
	exit(1);
}

int main (int argc, char *argv[])
{
	int rval = 0;
	unsigned char en_fg = 0;

	/* Get command line options */
	while ((rval = getopt_long(argc, argv, "acf", longopts, NULL)) != -1) {
		switch (rval) {
		case 'f':
			en_fg = 1;
			break;
		default:
			ShowUsage(argv[0]);
			break;
		}
	}

	rval = AmbaEventNotify_Init();
	if(rval < 0){
		printf("@@ Fail to do AmbaEventNotify_Init()\n");
		return -1;
	}

	if(!en_fg) {
		daemon(0, 1);
	}

	atexit(cleanup);
	/* Allow ourselves to be shut down gracefully by a signal */
	signal(SIGTERM, signalHandlerShutdown);
	signal(SIGHUP, signalHandlerShutdown);
	signal(SIGUSR1, signalHandlerShutdown);
	signal(SIGQUIT, signalHandlerShutdown);
	signal(SIGINT, signalHandlerShutdown);
	signal(SIGKILL, signalHandlerShutdown);

	AmbaEventNotifier_Reg_cbEvent(cbEvent, NULL);

	AmbaEventNotifier_ExecEventProcess(); //run forever

	return 0;
}

