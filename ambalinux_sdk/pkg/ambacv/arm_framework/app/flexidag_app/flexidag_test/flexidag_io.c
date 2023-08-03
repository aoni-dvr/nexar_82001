#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include "cvapi_ambacv_flexidag.h"
#include "cvapi_memio_interface.h"
#include "AmbaFlexidagIO.h"
#include <signal.h>
#include <semaphore.h>

static bool stop;

#ifndef CACHE_ALIGN
#define CACHE_ALIGN(x) ((x + 0x3F) & ~0x3F)
#endif /* CACHE_ALIGN */

/*
void sighandler(int sig)
{
    printf("got signalq, exit\n");
    stop = true;
}
*/

//pthread_mutex_t mutex;
static sem_t main_sem;
int channel  = 0;
//flexidag_memblk_t           buf0;

void get_FlexidagIO_0(void)
{
    memio_sink_send_out_t       out;
	memio_source_recv_picinfo_t pic_info;
	flexidag_memblk_t           buf;
	unsigned int                len;
	size_t                      counter = 0;
	
	
	if (AmbaIPC_FlexidagIO_Init(0) != FLEXIDAGIO_OK) {
        printf("channel_0 AmbaIPC_FlexidagIO_Init() fail\n");
        exit(EXIT_FAILURE);
    }

    if (AmbaIPC_FlexidagIO_Config(0, FLEXIDAGIO_RESULT_OUTTYPE_SEG) !=
        FLEXIDAGIO_OK) {
        printf("channel_0 AmbaIPC_FlexidagIO_Config() fail\n");
        exit(EXIT_FAILURE);
    }

    if (AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(64), 1, &buf) != 0) {
        printf("channel_0 AmbaCV_UtilityCmaMemAlloc() fail\n");
        exit(EXIT_FAILURE);
    }
	while(1) {
		 
	//	pthread_mutex_lock(&mutex);
		if (AmbaIPC_FlexidagIO_GetInput(0, &pic_info, &len) != FLEXIDAGIO_OK) {
			printf("channel_0 AmbaIPC_FlexidagIO_GetInput() fail\n");
			//continue;
		    exit(EXIT_FAILURE);
		}

		counter++;
		for (int i = 0; i < MAX_HALF_OCTAVES; i++) {
			printf("channel_0 %u: %d x %d\n", counter,
				   pic_info.pic_info.pyramid.half_octave[i].roi_width_m1 + 1,
				   pic_info.pic_info.pyramid.half_octave[i].roi_height_m1 + 1);
			/* ambacv_p2v(pic_info.pic_info.rpLumaLeft[i]); */
		}

		out.cvtask_frameset_id = pic_info.pic_info.frame_num;
		out.num_of_io = 1;
		out.io[0].addr = buf.buffer_daddr;
		out.io[0].size = 64;

		if (AmbaIPC_FlexidagIO_SetResult(0, &out, sizeof(out)) !=
			FLEXIDAGIO_OK) {
			printf("channel_0 AmbaIPC_FlexidagIO_SetResult() fail\n");
			//continue;
		    exit(EXIT_FAILURE);
		}
     //   pthread_mutex_unlock(&mutex);		
	}
	printf("0pthread_exit\r\n");
	printf("0pthread_exit\r\n");
	printf("0pthread_exit\r\n");
    pthread_exit(NULL);
}

//flexidag_memblk_t           buf1;
void get_FlexidagIO_1(void)
{
    memio_sink_send_out_t       out;
	memio_source_recv_picinfo_t pic_info;
	flexidag_memblk_t           buf;
	unsigned int                len;
	size_t                      counter = 0;
	
	if (AmbaIPC_FlexidagIO_Init(1) != FLEXIDAGIO_OK) {
        printf("channel_1 AmbaIPC_FlexidagIO_Init() fail\n");
        exit(EXIT_FAILURE);
    }

    if (AmbaIPC_FlexidagIO_Config(1, FLEXIDAGIO_RESULT_OUTTYPE_SEG) !=
        FLEXIDAGIO_OK) {
        printf("channel_1 AmbaIPC_FlexidagIO_Config() fail\n");
        exit(EXIT_FAILURE);
    }

    if (AmbaCV_UtilityCmaMemAlloc(CACHE_ALIGN(64), 1, &buf) != 0) {
        printf("channel_1 AmbaCV_UtilityCmaMemAlloc() fail\n");
        exit(EXIT_FAILURE);
    }
     
	while(1) {
	//	pthread_mutex_lock(&mutex);
		if (AmbaIPC_FlexidagIO_GetInput(1, &pic_info, &len) != FLEXIDAGIO_OK) {
			printf("channel_1 AmbaIPC_FlexidagIO_GetInput() fail\n");
			//continue;
		    exit(EXIT_FAILURE);
		}

		counter++;
		for (int i = 0; i < MAX_HALF_OCTAVES; i++) {
			printf("channel_1 %u: %d x %d\n", counter,
				   pic_info.pic_info.pyramid.half_octave[i].roi_width_m1 + 1,
				   pic_info.pic_info.pyramid.half_octave[i].roi_height_m1 + 1);
			/* ambacv_p2v(pic_info.pic_info.rpLumaLeft[i]); */
		}

		out.cvtask_frameset_id = pic_info.pic_info.frame_num;
		out.num_of_io = 1;
		out.io[0].addr = buf.buffer_daddr;
		out.io[0].size = 64;

		if (AmbaIPC_FlexidagIO_SetResult(1, &out, sizeof(out)) !=
			FLEXIDAGIO_OK) {
			printf("channel_1 AmbaIPC_FlexidagIO_SetResult() fail\n");
			//continue;
		    exit(EXIT_FAILURE);
		}
	//	pthread_mutex_unlock(&mutex);
	}
    pthread_exit(NULL);
}

int FlexidagIO_Deinit(void)
{
	if(channel & 1) {
		//AmbaCV_UtilityCmaMemClean(&buf0);
		AmbaIPC_FlexidagIO_Deinit(0);


	}
	if(channel & 2) {
		//AmbaCV_UtilityCmaMemClean(&buf1);
		AmbaIPC_FlexidagIO_Deinit(1);


	}
	channel = 0;
	//pthread_mutex_destroy(&mutex);
    return 0;
}
static void clean_up(int signal)
{
    static int bExit = 0;
    if (bExit == 0) {
        bExit = 1;
        printf("Application exit by signal:%d\n", signal);
        FlexidagIO_Deinit();
       // exit(1);
    }
}

static void SignalHandler(int signal)
{

	exit(1);

}

static void capture_all_signal()
{
    //int i = 0;
	atexit(clean_up);
	
	signal(SIGTERM, SignalHandler);
    signal(SIGHUP, SignalHandler);
    signal(SIGUSR1, SignalHandler);
    signal(SIGQUIT, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGKILL, SignalHandler);
    signal(SIGSEGV, SignalHandler);
#if 0 
    for(i = 0; i < 32; i ++) {
        if ( (i == SIGPIPE) || (i == SIGCHLD) || (i == SIGALRM) || (i == SIGPROF)) {
            signal(i, SIG_IGN);
        } else {
            signal(i, signal_handler);
        }
    }
#endif
}

int main(int argc, char *argv[])
{

    pthread_t thread_Flexi0;
	pthread_t thread_Flexi1;
	void *ret;
/*    if (signal(SIGINT, sighandler) == SIG_ERR) {
        printf("signal(SIGINT) fail\n");
        exit(EXIT_FAILURE);
    }
*/
    capture_all_signal();
    if(argc == 2) {
		channel = atoi(argv[1]);
		//pthread_mutex_init(&mutex, NULL);
		
		if(channel & 1) {
			if (pthread_create(&thread_Flexi0 , NULL, get_FlexidagIO_0, NULL) != 0) {
					perror("pthread_create");

			}
		}
		if(channel & 2) {
			if (pthread_create(&thread_Flexi1 , NULL, get_FlexidagIO_1, NULL) != 0) {
					perror("pthread_create");

			}
		}
		sleep(2);
		if(channel & 1) {
			pthread_join(thread_Flexi0, &ret);
		}
		if(channel & 2) {
			pthread_join(thread_Flexi1, &ret);
		}
	} else {
		printf("param error\r\n");
	}
	
	
	//sem_init(&main_sem, 0, 0);
	//sem_wait(&main_sem);
	//sem_destroy(&main_sem);
    //while(1)
   // AmbaCV_UtilityCmaMemClean(&buf);

    return 0;
}
