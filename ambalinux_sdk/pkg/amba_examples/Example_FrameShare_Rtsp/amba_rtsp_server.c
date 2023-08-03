
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <execinfo.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <time.h>

#include "rtsp_server.h"
#include "media_session.h"
#include "media_subsession.h"
#include "ambastream.h"

#ifndef AMBA_CAMERA
#include "aipc_msg.h"
#include "AmbaIPC_RpcProg_LU_ExamFrameShareRtsp.h"
#endif

#include <syslog.h>  //AllenLiu

static rtsp_server_t* srv = NULL;

static void print_reason(void)
{
    void *array[30];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 30);
    strings = backtrace_symbols(array, size);
    printf("Obtained %zd stack frames.\n", size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    free(strings);
}

static void sigsegv_handler(int sig) {
    fprintf(stderr, "Got Segmentation fault!!\n");
    print_reason();
    exit(1);
}

static void cleanup(void)
{
    if(srv != NULL) {
        srv->exit_loop(srv);
    }
}

static void sig_handler(int sig) {
    fprintf(stderr, "recieve signal %d\n", sig);
    cleanup();
}

#define MAX_PORT_AMOUNT (3)

/*
 * Print usage info and exit.
 */
static void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s [--en_audio] [--en_rtcp]\n", argv[0]);
    exit(EXIT_FAILURE);
}

/*
 * Command line arguments.
 */
static struct option longopts[] =
{
    { "en_audio",   no_argument,    NULL, 'a', },
    { "en_text",    no_argument,    NULL, 't', },
    { "en_rtcp",    no_argument,    NULL, 'c', },
    { "en_ap",      no_argument,    NULL, 'p', },
    { "en_fg",  no_argument,    NULL, 'f', },
    { NULL, 0, NULL, 0, },
};
static int get_live_brate()
{
    int brate = 0;
    brate = AmbaStream_send_PbCmd(AMBASTREAM_GET_LIVE_BITRATE, 0, NULL, 0, 0);
    return brate;
}
static int get_live_avg_brate()
{
    int brate = 0;
    brate = AmbaStream_send_PbCmd(AMBASTREAM_GET_LIVE_AVG_BITRATE, 0, NULL, 0, 0);
    return brate;
}
static int change_brate(int bitrate)
{
    return AmbaStream_send_PbCmd(AMBASTREAM_SET_LIVE_BITRATE, (void*)&bitrate, NULL, 0, 0);
}
static int change_net_bandwidth(void* streamer, int bandwidth)
{
    return AmbaStream_send_PbCmd(AMBASTREAM_SET_NET_BANDWIDTH, (void*)&bandwidth, AmbaStreamer_GetCodec(streamer), 0, 0);
}
static int send_rr_stat(void* stm, unsigned int fr_lost, unsigned jitter, double pg_delay)
{
    return AmbaStream_send_RRstat(stm, fr_lost, jitter, pg_delay);
}
static int get_uitrontime(double *ref_time)
{
    return AmbaStream_get_itrontime(ref_time);
}

int main(int argc, char** argv)
{
    int            i = 0;
    unsigned short ports[MAX_PORT_AMOUNT] = {554, 8554, 9554};
    unsigned char  en_foreground = 0, en_ap = 0;
    int            fd_CloseIdx;

    /*
        close all file descriptors execpt for standard in/out/error
    */
    for (fd_CloseIdx = getdtablesize(); fd_CloseIdx > 2; --fd_CloseIdx) {
        close(fd_CloseIdx);
    }
    /*
       Enlarge net/core/wmem_max size for kernel 5.10.16,
       otherwise there are send errors
    */
    if (system("echo 1048576 > /proc/sys/net/core/wmem_max") < 0) {
        fprintf(stderr, "Fail to configure net/core/wmem_max\n");
    } else {
        fprintf(stderr, "Success to configure net/core/wmem_max\n");
    }

    /* Get command line options */
    while ((i = getopt_long(argc, argv, "acft", longopts, NULL)) != -1) {
        switch (i) {
        case 'a':
            fprintf(stderr, "AmbaRTSPServer: en_audio option is deprecated, it's determined by rtos now\n");
            break;
        case 't':
            fprintf(stderr, "AmbaRTSPServer: en_text option is deprecated, it's determined by rtos now\n");
            break;
        case 'c':
            fprintf(stderr, "AmbaRTSPServer: en_rtcp option is deprecated, it's default on now\n");
            break;
        case 'p':
            fprintf(stderr, "AmbaRTSPServer: Aggregation Packet option is enabled\n");
            en_ap = 1;
            break;
        case 'f':
            en_foreground = 1;
            break;
        default:
            usage(argc, argv);
            break;
        }
    }

    if(!en_foreground) {
        daemon(0, 1);
    }

    i = AmbaStream_init();

    if(i<0){
        fprintf(stderr, "failed to init AmbaStream! %d\n",i);
        return -1;
    }

    atexit(cleanup);

    for(i = 0; i < MAX_PORT_AMOUNT; i++) {
        srv = rtsp_server_create(ports[i], en_ap);
        if(srv != NULL) {
            break;
        }
    }

    if(srv == NULL) {
        fprintf(stderr, "failed to create rtsp server\n");
        return -1;
    }

    srv->get_systime = get_uitrontime;
    srv->change_brate = change_brate;
    srv->change_net_bandwidth = change_net_bandwidth;
    srv->get_live_brate = get_live_brate;
    srv->get_live_avg_brate = get_live_avg_brate;
    srv->send_rr_stat = send_rr_stat;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sig_handler);
    signal(SIGQUIT, sig_handler);
    signal(SIGINT,  sig_handler);
    signal(SIGHUP,  sig_handler);
    signal(SIGUSR1, sig_handler);
    signal(SIGKILL, sig_handler);
    signal(SIGSEGV, sigsegv_handler);

    srv->run(srv);

    fprintf(stderr,"server down\n");

    rtsp_server_release(srv);

    srv = NULL;
    AmbaStream_release();

    return 0;
}
