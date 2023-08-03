#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <getopt.h>
#include <execinfo.h> /* for backtrace() */

#include "schdr_api.h"
#include "cvtask_ossrv.h"
#include "cvapi_ambacv_flexidag.h"
#include "cam_util.h"
#include "mem_util.h"
#include "dag_util.h"
#include "dag_impl.h"

//#include "NMS.h"

uint32_t debug; //show debug message
static char default_flexidagpath[]="./flexibin";

/*
 * Command line arguments.
 */
#define OPTSTRING "v"
static struct option longopts[] =
{
    { "en_dbg", no_argument,        NULL, 'v', },
    { NULL, 0, NULL, 0, },
};

/**
 * disply usage of parameters
 **/
static void ShowHelp(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t--en_dbg \t enable debug message.\n");
}

/**
 * parse program parameters
 **/
static void ParseOpt(int argc, char **argv)
{
    int i;
    while ((i = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (i) {
        case 'v':
            debug = 1;
            break;
        default:
            ShowHelp(argv[0]);
            exit(1);
        }
    }
}

/**
 * Relase program resource
 **/
static void ReleaseAPP(void)
{
    CamUtil_Release(0);
    DagFlow_Release(0);
    CamUtil_Release(1);
    DagFlow_Release(1);
}

/**
 * aexit() callback
 * This function will be invoked when program exits.
 **/
static void CleanUp(void)
{
    printf("program exit!\n");
    ReleaseAPP();
    printf("Program built at %s - %s\n", __DATE__, __TIME__);
}

/**
 * signal() callback
 * This function will be invoked when got related signal.
 **/
static void SignalHandler(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(1);
}

static void SEGVhandler(int signo)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "%s: Got segmentation fault (%d)\n", __FILE__, signo);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

/**
 * init program resource.
 * This function will hook system callback, parse command-line parameters and init resource.
 **/
static uint32_t InitAPP(int argc, char **argv)
{
    uint32_t ret = 0;

    ParseOpt(argc,argv);

    atexit(CleanUp);

    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, SignalHandler);
    signal(SIGHUP, SignalHandler);
    signal(SIGUSR1, SignalHandler);
    signal(SIGQUIT, SignalHandler);
    signal(SIGINT, SignalHandler);
    signal(SIGKILL, SignalHandler);
    signal(SIGSEGV, SEGVhandler);

    return ret;
}

/**
 * init CV framework.
 * This function will init CV framework.
 **/
static uint32_t InitCVFlow(void)
{
    uint32_t ret = 0;
    AMBA_CV_FLEXIDAG_SCHDR_CFG_s cfg;

    /* init scheduler */
    cfg.cpu_map = 0x9;
    cfg.log_level = LVL_DEBUG;
    AmbaCV_FlexidagSchdrStart(&cfg);

    return ret;
}

/**
 * Give Result to RTOS.
 * This function will send result to RTOS.
 **/
static uint32_t GiveResult(uint32_t ch, uint32_t frameId, void *result)
{
    uint32_t retcode = 0;
    flexidag_memblk_t *res_blk = NULL;

    /* Retrieve output */
    res_blk = (flexidag_memblk_t *)result;

    /* Send OSD buffer to RTOS */
    if (1) {
        memio_sink_send_out_t out = {0};

        out.cvtask_frameset_id = frameId;
        out.num_of_io = 1;
        out.io[0].addr = res_blk->buffer_daddr;
        out.io[0].size = res_blk->buffer_size;

        retcode = CamUtil_SendResult(ch, &out);
    }

    return retcode;
}

/**
 * Implementation for sensor input handling.
 * It will retrieve frame from RTOS, feed into flexidag and then send result to RTOS.
 **/
void *Thread_Ch0(void *data)
{
    uint32_t retcode, ch = 0;
    char *path;
    memio_source_recv_picinfo_t RecvData;
    flexidag_memblk_t *result;

    if (data != NULL) {
        path = (char *)data;
    } else {
        path = &default_flexidagpath[0];
    }

    /* init DagFlow. This will init your flow implementation. */
    retcode = DagFlow_Init(ch, path);
    if (retcode != 0) {
        printf("[Ch%u] DagFlow_Init fail!\n", ch);
        return NULL;
    }

    /* Init CamUtil. RTOS will be signaled for frame feeding during this operation */
    CamUtil_Init(ch);

    for(;;) {
        /* retrieve Frame */
        retcode = CamUtil_GetFrame(ch, &RecvData);
        if (retcode != 0) {
            fprintf(stderr, "[Ch%u] CamUtil_GetFrame fail!(%u)\n", ch, retcode);
            break;
        }

        /* Feed into flexidag(s) and get result */
        retcode = DagFlow_Process(ch, &RecvData, (void **)&result);
        if (retcode != 0) {
            printf("[Ch%u] DagFlow_Process fail!\n",ch);
            break;
        }

        /* send result */
        retcode = GiveResult(ch, RecvData.pic_info.frame_num, result);
        if (retcode != 0) {
            printf("[Ch%u] GiveResult fail!\n",ch);
            break;
        }
    }

    fprintf(stderr, "[Ch%u] Thread exit!(%u)\n", ch, retcode);
    pthread_exit(NULL);
}

/**
 * Implementation for sensor input handling.
 * It will retrieve frame from RTOS, feed into flexidag and then send result to RTOS.
 **/
void *Thread_Ch1(void *data)
{
    uint32_t retcode, ch = 1;
    char *path;
    memio_source_recv_picinfo_t RecvData;
    flexidag_memblk_t *result;

    if (data != NULL) {
        path = (char *)data;
    } else {
        path = &default_flexidagpath[0];
    }

    /* init DagFlow. This will init your flow implementation. */
    retcode = DagFlow_Init(ch, path);
    if (retcode != 0) {
        printf("[Ch%u] DagFlow_Init fail!\n", ch);
        return NULL;
    }

    /* Init CamUtil. RTOS will be signaled for frame feeding during this operation */
    CamUtil_Init(ch);

    for(;;) {
        /* retrieve Frame */
        retcode = CamUtil_GetFrame(ch, &RecvData);
        if (retcode != 0) {
            fprintf(stderr, "[Ch%u] CamUtil_GetFrame fail!(%u)\n", ch, retcode);
            break;
        }

        /* Feed into flexidag(s) and get result */
        retcode = DagFlow_Process(ch, &RecvData, (void **)&result);
        if (retcode != 0) {
            printf("[Ch%u] DagFlow_Process fail!\n",ch);
            break;
        }

        /* send result */
        retcode = GiveResult(ch, RecvData.pic_info.frame_num, result);
        if (retcode != 0) {
            printf("[Ch%u] GiveResult fail!\n",ch);
            break;
        }
    }

    fprintf(stderr, "[Ch%u] Thread exit!(%u)\n", ch, retcode);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    uint32_t retcode;
    pthread_t t_ch0, t_ch1;
    void *ret;

    retcode = InitAPP(argc, argv);
    if (retcode != 0) {
        printf("main: InitAPP fail!\n");
        return retcode;
    }

    retcode = InitCVFlow();
    if (retcode != 0) {
        printf("main: InitCV fail!\n");
        return retcode;
    }

    /* create processing thread */
    pthread_create(&t_ch0, NULL, Thread_Ch0, NULL);
    pthread_create(&t_ch1, NULL, Thread_Ch1, NULL);

    /* wait processing done */
    pthread_join(t_ch0, &ret);
    pthread_join(t_ch1, &ret);

    /* Program exit */
    printf("main process finish.\n");
    return 0;
}

