#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <getopt.h>
#include <execinfo.h>
#include "AmbaNetFifo.h"

#define USE_SIGACTION (1)

typedef struct ef_video_param_s {
    int MediaId;
    unsigned int TimeScale;
    unsigned int TimePerFrame;
    unsigned int InitDelay;
    double vector_90k; //vector to 90K reference clock
} ef_video_param_t;

typedef struct ef_audio_param_s {
    int MediaId;
    unsigned int TimeScale;
    unsigned int TimePerFrame;
    int SampleRate;
    int channels;
    double vector_sr; //vector to sample rate
} ef_audio_param_t;

typedef struct ef_buffer_info_s {
    void *base;
    void *limit;
    void *param;
    int frame_count;
} ef_buffer_info_t;

typedef struct ef_fifo_info_s {
    unsigned long long fifo_hndlr;
    ef_buffer_info_t buf_info;
    pthread_mutex_t mutex;
} ef_fifo_info_t;

//To keep fifo/frame info
static ef_fifo_info_t g_video_fifo;
static ef_fifo_info_t g_audio_fifo;
static ef_video_param_t g_video_param;
static ef_audio_param_t g_audio_param;


//Event Hanler ID for NetFiFo
static int event_hndlr = -1;

//program status
static unsigned char running = 0;
static unsigned char eos_video = 0;
static unsigned char eos_audio = 0;
static unsigned char daemon_mode = 0;
static unsigned char switch_enc = 0;


//DEBUG
static unsigned int v_frame_count = 0;
static unsigned int a_frame_count = 0;
static unsigned char *v_frame = NULL;
static unsigned char *a_frame = NULL;
#define SIGNAL_VIDEO_FRAME_BUFFER_SIZE (512*1024) //4Mbits
#define SIGNAL_AUDIO_FRAME_BUFFER_SIZE (128*1024) //1Mbits

/**
 * Clean up resource when program exit.
 */
static void cleanup(void)
{
    AmbaNetFifo_release();
    pthread_mutex_destroy(&g_video_fifo.mutex);
    pthread_mutex_destroy(&g_audio_fifo.mutex);

    if(v_frame) {
        free(v_frame);
        v_frame = NULL;
    }
    if(a_frame) {
        free(a_frame);
        a_frame = NULL;
    }
}

/**
 * Linux signal handler.
 */
#if USE_SIGACTION //for sigaction() way
void signalHandlerShutdown(int sig, struct sigcontext ctx)
{
    printf("%s: Got signal %d, program exits!\n", __FILE__, sig);
    exit(0);
}

void sigsegv_handler(int sig, siginfo_t *info, void *secret)
{
    void *trace[30];
    char **messages = (char **)NULL;
    int i, trace_size = 0;
    ucontext_t *uc = (ucontext_t *)secret;

#ifdef USE_ARM
    /* Do something useful with siginfo_t */
    printf("Got signal %d, si_code= x%x, faulty address is %p, from 0x%lx\n",
       sig, info->si_code, info->si_addr, (uc->uc_mcontext.arm_pc));
#else
    /* Do something useful with siginfo_t */
    printf("Got signal %d, si_code= x%x, faulty address is %p, from 0x%llx\n",
           sig, info->si_code, info->si_addr, (uc->uc_mcontext.pc));
#endif

    trace_size = backtrace(trace, 30);
#ifdef USE_ARM
    /* overwrite sigaction with caller's address */
    trace[1] = (void *)uc->uc_mcontext.arm_pc;
#else
    /* overwrite sigaction with caller's address */
    trace[1] = (void *)uc->uc_mcontext.pc;
#endif

    messages = backtrace_symbols(trace, trace_size);
    /* skip first stack frame (points here) */
    printf("[backtrace] Execution path:\n");
    for (i = 1; i < trace_size; ++i)
        printf("[backtrace] %s\n", messages[i]);

    AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_END);
    usleep(20000);
    AmbaNetFifo_StopEventProcess();
    usleep(20000);
    exit(1);
}
#else //for signal() way.
static void signalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n", __FILE__, sig);
    exit(0);
}

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

    AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_END);
    usleep(20000);
    AmbaNetFifo_StopEventProcess();
    usleep(20000);

    free(strings);
}

static void sigsegv_handler(int sig)
{
    fprintf(stderr, "Got Segmentation fault!!\n");
    print_reason();
    exit(1);
}
#endif

/**
 * Increase valid frame count.
 */
static inline void inc_frame(ef_fifo_info_t *finfo)
{
    pthread_mutex_lock(&finfo->mutex);
    finfo->buf_info.frame_count++;
    pthread_mutex_unlock(&finfo->mutex);
}

/**
 * Decrease valid frame count.
 */
static inline void dec_frame(ef_fifo_info_t *finfo)
{
    pthread_mutex_lock(&finfo->mutex);
    finfo->buf_info.frame_count--;
    pthread_mutex_unlock(&finfo->mutex);
}

/**
 * Reset valid frame count.
 */
static inline void clean_framecount(ef_fifo_info_t *finfo)
{
    pthread_mutex_lock(&finfo->mutex);
    finfo->buf_info.frame_count = 0;
    pthread_mutex_unlock(&finfo->mutex);
}

/**
 * Check is there any valid frame.
 */
static inline int check_frame(ef_fifo_info_t *finfo)
{
    if(finfo->buf_info.frame_count == 0) {
        return 0;
    }
    return 1;
}

/**
 * Create virtual fifo to get frame data
 * @return  0 - success
 *          1 - Video Encode does not start yet
 *         <0 - fail
 */
static int create_vfifo()
{
    int rval = 0, i;
    unsigned long long                      fifo_hndlr = 0ull;
    AMBA_NETFIFO_MEDIA_STREAMITEM_LIST_s    slist = {0};
    AMBA_NETFIFO_MOVIE_INFO_CFG_s           mInfo = {{{0}}};
    AMBA_NETFIFO_CFG_s                      cfg = {0};
    ef_fifo_info_t                          *pFIinfo = 0;

    rval = AmbaNetFifo_GetMediaStreamIDList(&slist);
    if(rval < 0) {
        printf("%s: Fail to do AmbaNetFifo_GetMediaStreamIDList()\n", __FUNCTION__);
        return -1;
    }
    if(slist.Amount < 1) {
        printf("%s: There is no valid stream. Maybe video record does not started yet.\n", __FUNCTION__);
        return 1;
    }

    for (i = 0; i < slist.Amount; i++) {
        printf("%s: Checking '%s' (active= %d)\n", __FUNCTION__, slist.StreamItemList[i].Name, slist.StreamItemList[i].Active);
        if (slist.StreamItemList[i].Active) {
            break;
        }
    }

    if (i == slist.Amount) {
        fprintf(stderr, "%s: There is no active stream. Please check the net stream table.\n", __FUNCTION__);
        return -1;
    }

    rval = AmbaNetFifo_GetMediaInfo(i, &mInfo);
    if(rval < 0) {
        printf("%s: Fail to do AmbaNetFifo_GetMediaInfo()\n", __FUNCTION__);
        return -1;
    }

    rval = AmbaNetFifo_GetDefaultCFG(&cfg);
    if(rval < 0) {
        printf("%s: Fail to do AmbaNetFifo_GetDefaultCFG()\n", __FUNCTION__);
        return -1;
    }

    //create fifo
    if(mInfo.nTrack != 0) {
        for(i = 0; i < mInfo.nTrack; i++) {
            if(mInfo.Track[i].nTrackType == AMBA_NETFIFO_MEDIA_TRACK_TYPE_VIDEO) {
                g_video_param.MediaId      = mInfo.Track[i].nMediaId;
                g_video_param.TimeScale    = (unsigned int)(mInfo.Track[i].nTimeScale);
                g_video_param.TimePerFrame = (unsigned int)(mInfo.Track[i].nTimePerFrame);
                g_video_param.InitDelay    = (unsigned int)(mInfo.Track[i].nInitDelay);
                g_video_param.vector_90k   = (double)90000 / (double)g_video_param.TimeScale;
                printf("video stream: codec=0x%0x, TimeScale=%d, TimePerFrame=%d\n",
                       mInfo.Track[i].nMediaId, mInfo.Track[i].nTimeScale, mInfo.Track[i].nTimePerFrame);

                pFIinfo = &g_video_fifo;
            } else if(mInfo.Track[i].nTrackType == AMBA_NETFIFO_MEDIA_TRACK_TYPE_AUDIO) {
                g_audio_param.MediaId   = mInfo.Track[i].nMediaId;
                g_audio_param.TimeScale = (unsigned int)(mInfo.Track[i].nTimeScale) > 0 ?
                                          (unsigned int)(mInfo.Track[i].nTimeScale) : 90000;
                g_audio_param.TimePerFrame = (unsigned int)(mInfo.Track[i].nTimePerFrame);
                g_audio_param.SampleRate   = mInfo.Track[i].Info.Audio.nSampleRate;
                g_audio_param.channels     = mInfo.Track[i].Info.Audio.nChannels;
                g_audio_param.vector_sr    = (double)g_audio_param.SampleRate / (double)g_audio_param.TimeScale;
                printf("audio stream: codec=0x%0x, samplerate=%d, channels=%d, TimeScale=%d, TimePerFrame=%d\n",
                       mInfo.Track[i].nMediaId, mInfo.Track[i].Info.Audio.nSampleRate,
                       mInfo.Track[i].Info.Audio.nChannels, g_audio_param.TimeScale,
                       g_audio_param.TimePerFrame);

                pFIinfo = &g_audio_fifo;
            } else {
                printf("%s: unsupported track type. ignore...\n", __FUNCTION__);
                continue;
            }

            if(mInfo.Track[i].hCodec != NULL) {
                cfg.hCodec     = mInfo.Track[i].hCodec;
                cfg.cbEvent    = (unsigned int)event_hndlr;
                cfg.NumEntries = 256;
                cfg.IsVirtual  = 1;
                fifo_hndlr     = AmbaNetFifo_Create(&cfg);
                if(fifo_hndlr == 0ull) {
                    printf("%s: Fail to do AmbaNetFifo_Create()\n", __FUNCTION__);
                    return -1;
                } else {
                    printf("@@ Create new fifo 0x%llx for hCodec %p success!\n", fifo_hndlr, cfg.hCodec);
                    pFIinfo->fifo_hndlr     = fifo_hndlr;
                    pFIinfo->buf_info.base  = (void *)(mInfo.Track[i].pBufferBase);
                    pFIinfo->buf_info.limit = (void *)(mInfo.Track[i].pBufferLimit);
                    printf("=== get buf range: %p ~ %p\n", pFIinfo->buf_info.base, pFIinfo->buf_info.limit);
                    running = 1;
                }
            }
        }

        //reset frame count(DEBUG)
        v_frame_count = 0;
        a_frame_count = 0;
    }

    if(running) {
        //Report to RTOS when handlers started.
        AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_START);
    }
    return 0;
}

/**
 * Callback function for NetFiFo control event.
 */
static int cbCtrlEvent(unsigned int cmd, unsigned int param1, unsigned int param2, void *user_data)
{
    int er;

    switch(cmd) {
        case AMBA_NETFIFO_CMD_STARTENC:
            printf("%s: RTOS Encode Started!!\n", __FUNCTION__);
            if((g_video_fifo.fifo_hndlr == 0ull) && (g_audio_fifo.fifo_hndlr == 0ull)) {
                er = create_vfifo();
                if(er < 0) {
                    printf("%s: fail to create_fifo!! %d\n", __FUNCTION__, er);
                    return -1;
                }
            }
            break;
        case AMBA_NETFIFO_CMD_SWITCHENCSESSION:
            switch_enc = 1;
        case AMBA_NETFIFO_CMD_STOPENC:
        case AMBA_NETFIFO_CMD_RELEASE:
            printf("%s: RTOS Encode Stopped!!\n", __FUNCTION__);
            if(g_video_fifo.fifo_hndlr) {
                eos_video = 1;
            }
            if(g_audio_fifo.fifo_hndlr) {
                eos_audio = 1;
            }
            break;
        default:
            printf("%s: unhandled cmd x%x\n", __FUNCTION__, cmd);
            break;
    }

    return 0;
}

/**
 * Check frame type for video.
 */
static inline int is_VideoFrame(unsigned char desc_type)
{
    int rval = 0;

    switch(desc_type) {
        case AMBA_FRAMEINFO_TYPE_IDR_FRAME:
        case AMBA_FRAMEINFO_TYPE_I_FRAME:
        case AMBA_FRAMEINFO_TYPE_P_FRAME:
        case AMBA_FRAMEINFO_TYPE_B_FRAME:
        case AMBA_FRAMEINFO_TYPE_EOS:
            rval = 1;
            break;
        default:
            rval = 0;
            break;
    }

    return rval;
}

/**
 * Check frame type for audio.
 */
static inline int is_AudioFrame(unsigned char desc_type)
{
    int rval = 0;

    switch(desc_type) {
        case AMBA_FRAMEINFO_TYPE_AUDIO_FRAME:
        case AMBA_FRAMEINFO_TYPE_EOS:
            rval = 1;
            break;
        default:
            rval = 0;
            break;
    }

    return rval;
}

/**
 * Callback function for NetFiFo event.
 */
static int cbFrameReady(void *hdlr, unsigned int event, void *info, void *user_data)
{
    if (event == AMBA_NETFIFO_EVENT_DATA_READY) {
        if((unsigned long long)(uintptr_t)hdlr == g_video_fifo.fifo_hndlr) {
            inc_frame(&g_video_fifo);
        } else if((unsigned long long)(uintptr_t)hdlr == g_audio_fifo.fifo_hndlr) {
            inc_frame(&g_audio_fifo);
        } else {
            printf("%s: unknown hndlr. ignore\n", __FUNCTION__);
        }
    } else if (event == AMBA_NETFIFO_EVENT_DATA_EOS) {
        printf("%s: Got EOS (%08x)\n", __FUNCTION__, event);
        if((unsigned long long)(uintptr_t)hdlr == g_video_fifo.fifo_hndlr) {
            eos_video = 1;
        } else if((unsigned long long)(uintptr_t)hdlr == g_audio_fifo.fifo_hndlr) {
            eos_audio = 1;
        } else {
            printf("%s: unknown hndlr. ignore\n", __FUNCTION__);
        }
    } else {
        printf("%s: unhandled event (x%08x)\n", __FUNCTION__, event);
    }

    return 0;
}

/**
 * processing video frame.
 */
static int process_video_frame(ef_fifo_info_t *finfo)
{
    int er, rval = 0;
    AMBA_NETFIFO_PEEKENTRY_ARG_s entry;
    AMBA_NETFIFO_BITS_DESC_s desc;
    AMBA_NETFIFO_REMOVEENTRY_ARG_s r_entry;

    if(finfo->fifo_hndlr == 0ull) {
        return -1;
    }

    entry.fifo = finfo->fifo_hndlr;
    entry.distanceToLastEntry = 0;
    er = AmbaNetFifo_PeekEntry(&entry, &desc);
    if (er != 0) {
        printf("%s: Fail to do AmpFifo_PeekEntry.(%d)\n", __FUNCTION__, er);
        return 0;
    }

    if((desc.Type == AMBA_FRAMEINFO_TYPE_EOS) || (desc.Size == AMBA_NETFIFO_MARK_EOS)) { //It is EOS
        printf("%s: video size is EOS mark!(%x)\n", __FUNCTION__, desc.Size);
        rval = -99;
    } else {
        if(!is_VideoFrame(desc.Type)) {
            printf("%s: It is not Video frame!(%x)\n", __FUNCTION__, desc.Type);
            return 0;
        }

        /**
         *  Do frame precess here...
         *  In this example, it will count the retrieved frame amount and
         *  copy frame data into local buffer.
         */

        //if (desc.Type == AMBA_FRAMEINFO_TYPE_IDR_FRAME) {
        //      printf("%s: Got IDR frame\n", __FUNCTION__);
        //}

        v_frame_count++;
        if((v_frame_count & 0xff) == 0) {
            double              tmp_pts;
            unsigned long long  PTS_90K;
            ef_video_param_t    *vparam = (ef_video_param_t *)finfo->buf_info.param;

            if (vparam->vector_90k != 0) {
                tmp_pts = vparam->vector_90k * desc.Pts;
            } else {
                fprintf(stderr, "vector is %g. please check the mediainfo!(video)\n",
                        vparam->vector_90k);
                tmp_pts = desc.Pts;
            }
            PTS_90K = tmp_pts;

            printf("%s: got %u video frames. PTS=%llu (%llu@90K)\n", __FUNCTION__, v_frame_count, desc.Pts, PTS_90K);
        }

        //Retrieve Frame data
        if(v_frame != NULL) {
            char                    *start = (char *)desc.StartAddr;
            unsigned long long      b_limit = (unsigned long long)(uintptr_t)finfo->buf_info.limit;
            unsigned int            f_size = desc.Size;

            if(f_size > SIGNAL_VIDEO_FRAME_BUFFER_SIZE) {
                /* This should never happen in real case.
                 * Programer need to prepare enough space for the incoming frame;
                 * Otherwise, the frame will be missing/broken.
                 */
                fprintf(stderr, "Video Frame too large!(%u/%u) Skip retrieve operation!\n", f_size, SIGNAL_VIDEO_FRAME_BUFFER_SIZE);
            } else {
                if(((unsigned long long)(uintptr_t)(start + desc.Size)) > b_limit) {//ring buffer wrap around
                    long long   tmp_size = b_limit - (unsigned long long)(uintptr_t)start;
                    memcpy((void*)v_frame, (void*)start, tmp_size);

                    f_size -= tmp_size;
                    start = (char *)finfo->buf_info.base;
                }

                memcpy((void*)v_frame, (void*)start, f_size);
            }
        }
    }

    r_entry.EntriesToBeRemoved = 1;
    r_entry.fifo = finfo->fifo_hndlr;
    AmbaNetFifo_RemoveEnrty(&r_entry);

    dec_frame(finfo);

    return rval;
}

/**
 * processing audio frame.
 */
static int process_audio_frame(ef_fifo_info_t *finfo)
{
    int er, rval = 0;
    AMBA_NETFIFO_PEEKENTRY_ARG_s entry;
    AMBA_NETFIFO_BITS_DESC_s desc;
    AMBA_NETFIFO_REMOVEENTRY_ARG_s r_entry;

    if(finfo->fifo_hndlr == 0ull) {
        return -1;
    }

    entry.fifo = finfo->fifo_hndlr;
    entry.distanceToLastEntry = 0;
    er = AmbaNetFifo_PeekEntry(&entry, &desc);
    if (er != 0) {
        printf("%s: Fail to do AmpFifo_PeekEntry.(%d)\n", __FUNCTION__, er);
        return 0;
    }

    if((desc.Type == AMBA_FRAMEINFO_TYPE_EOS) || (desc.Size == AMBA_NETFIFO_MARK_EOS)) { //It is EOS
        printf("%s: audio size is EOS mark!(%x)\n", __FUNCTION__, desc.Size);
        rval = -99;
    } else {
        if(!is_AudioFrame(desc.Type)) {
            printf("%s: It is not Audio frame!(%x)\n", __FUNCTION__, desc.Type);
            return 0;
        }

        /**
         *  Do frame precess here...
         *  In this example, it will count the retrieved frame amount and
         *  copy frame data into local buffer.
         */
        a_frame_count++;
        if((a_frame_count & 0xff) == 0) {
            double tmp_pts;
            unsigned long long PTS_SR;
            ef_audio_param_t *aparam = (ef_audio_param_t *)finfo->buf_info.param;

            if (aparam->vector_sr != 0) {
                tmp_pts = aparam->vector_sr * desc.Pts;
            } else {
                fprintf(stderr, "vector is %g. please check the mediainfo!(audio)\n",
                        aparam->vector_sr);
                tmp_pts = desc.Pts;
            }
            PTS_SR = tmp_pts;

            printf("%s: got %u audio frames. PTS=%llu (%llu@SR)\n", __FUNCTION__, a_frame_count, desc.Pts, PTS_SR);
        }

        //Retrieve Frame data
        if(a_frame != NULL) {
            char *start = (char *)desc.StartAddr;
            unsigned long long b_limit = (unsigned long long)(uintptr_t)finfo->buf_info.limit;
            int f_size = desc.Size;

            if(f_size > SIGNAL_AUDIO_FRAME_BUFFER_SIZE) {
                /* This should never happen in real case.
                 * Programer need to prepare enough space for the incoming frame;
                 * Otherwise, the frame will be missing/broken.
                 */
                fprintf(stderr, "Audio Frame too large!(%u/%u) Skip retrieve operation!\n", f_size, SIGNAL_AUDIO_FRAME_BUFFER_SIZE);
            } else {
                if(((unsigned long long)(uintptr_t)(start + desc.Size)) > b_limit) {//ring buffer wrap around
                    long long tmp_size = b_limit - (unsigned long long)(uintptr_t)start;
                    memcpy(a_frame, start, tmp_size);

                    f_size -= tmp_size;
                    start = (char *) finfo->buf_info.base;
                }

                memcpy(a_frame, start, f_size);
            }
        }
    }

    r_entry.EntriesToBeRemoved = 1;
    r_entry.fifo = finfo->fifo_hndlr;
    AmbaNetFifo_RemoveEnrty(&r_entry);

    dec_frame(finfo);

    return rval;
}

/*
 * Command line arguments.
 */
static struct option longopts[] =
{
    { "en_daemon",   no_argument,    NULL, 'd', },
    { "en_fg",  no_argument,    NULL, 'f', },
    { NULL, 0, NULL, 0, },
};

static void usage(int argc, char *argv[])
{
    printf("Usage: %s [Options]\n", argv[0]);
    printf("Options:\n");
    printf("\t-d: run as daemon mode (will not exit while record stop)\n");
    printf("\t-f: run at foreground\n");
}

int main (int argc, char *argv[])
{
    int rval = 0, processed = 0;
    pthread_t tid_event;
    ef_fifo_info_t *finfo;
    unsigned char en_foreground = 0;

    daemon_mode = 0;

    /* Get command line options */
    while ((rval = getopt_long(argc, argv, "df", longopts, NULL)) != -1) {
        switch (rval) {
            case 'd':
                daemon_mode = 1;
                break;
            case 'f':
                en_foreground = 1;
                break;
            default:
                usage(argc, argv);
                return -1;
        }
    }

    if (!en_foreground) {
        daemon(0, 1);
    }

    //init NetFiFo
    rval = AmbaNetFifo_init(&event_hndlr);
    if(rval < 0) {
        printf("@@ Fail to do AmbaNetFifo_init()\n");
        return -1;
    }
    if(event_hndlr < 0) {
        printf("@@ invalid event_hndlr(%d)\n", event_hndlr);
        return -1;
    }

    atexit(cleanup);
    /* Allow ourselves to be shut down gracefully by a signal */
#if USE_SIGACTION
    do {
        struct sigaction sa;

        /* backtrace for sigsegv signal handler */
        sa.sa_sigaction = (void *)sigsegv_handler;
        sigemptyset (&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_SIGINFO;
        sigaction(SIGSEGV, &sa, NULL);

        /* all other signal handler */
        sa.sa_handler = (void *)signalHandlerShutdown;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGQUIT, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGHUP, &sa, NULL);
        sigaction(SIGUSR1, &sa, NULL);
        sigaction(SIGKILL, &sa, NULL);
    } while(0);
#else
    signal(SIGTERM, signalHandlerShutdown);
    signal(SIGQUIT, signalHandlerShutdown);
    signal(SIGINT, signalHandlerShutdown);
    signal(SIGHUP, signalHandlerShutdown);
    signal(SIGUSR1, signalHandlerShutdown);
    signal(SIGKILL, signalHandlerShutdown);

    signal(SIGSEGV, sigsegv_handler);
#endif

    //init Resource
    running = 0;
    eos_video = 0;
    eos_audio = 0;
    switch_enc = 0;

    memset(&g_video_param, 0, sizeof(ef_video_param_t));
    memset(&g_video_fifo, 0, sizeof(ef_fifo_info_t));
    g_video_fifo.buf_info.param = &g_video_param;
    if(pthread_mutex_init(&g_video_fifo.mutex, NULL) != 0) {
        fprintf(stderr, "%s: Fail to create video mutex!\n", __FUNCTION__);
    }

    memset(&g_audio_param, 0, sizeof(ef_audio_param_t));
    memset(&g_audio_fifo, 0, sizeof(ef_fifo_info_t));
    g_audio_fifo.buf_info.param = &g_audio_param;
    if(pthread_mutex_init(&g_audio_fifo.mutex, NULL) != 0) {
        fprintf(stderr, "%s: Fail to create audio mutex!\n", __FUNCTION__);
    }

    /* create buffer for retrieved frame */
    v_frame = (unsigned char *)malloc(SIGNAL_VIDEO_FRAME_BUFFER_SIZE);
    if(v_frame == NULL) {
        fprintf(stderr, "%s: Fail to malloc v_frame!!\n", __FUNCTION__);
        return -1;
    }

    a_frame = (unsigned char *)malloc(SIGNAL_AUDIO_FRAME_BUFFER_SIZE);
    if(a_frame == NULL) {
        fprintf(stderr, "%s: Fail to malloc a_frame!!\n", __FUNCTION__);
        return -1;
    }

    AmbaNetFifo_Reg_cbFifoEvent(cbFrameReady, NULL);
    AmbaNetFifo_Reg_cbControlEvent(cbCtrlEvent, NULL);

    if(pthread_create(&tid_event, NULL, (void *)&AmbaNetFifo_ExecEventProcess, NULL) != 0) {
        fprintf(stderr, "%s: Fail to create AmbaNetFifo_ExecEventProcess!!\n", __FUNCTION__);
        return -1;
    }

    //create vfifo
    rval = create_vfifo();
    if(rval < 0) {
        fprintf(stderr, "%s: Fail to create fifo!!\n", __FUNCTION__);
        return -1;
    }

    //process frame
    for(processed = 0;; processed = 0) {
        finfo = &g_video_fifo;
        if(check_frame(finfo)) {
            rval = process_video_frame(finfo);
            if(rval == -99) { //EOS
                AmbaNetFifo_Delete(finfo->fifo_hndlr);
                finfo->fifo_hndlr = 0ull;
                clean_framecount(finfo);
            } else {
                processed = 1;
            }
        } else if(eos_video) {
            if(finfo->fifo_hndlr) {
                AmbaNetFifo_Delete(finfo->fifo_hndlr);
                finfo->fifo_hndlr = 0ull;
            }
            clean_framecount(finfo);
        }

        finfo = &g_audio_fifo;
        if (check_frame(finfo)) {
            rval = process_audio_frame(finfo);
            if(rval == -99) { //EOS
                AmbaNetFifo_Delete(finfo->fifo_hndlr);
                finfo->fifo_hndlr = 0ull;
                clean_framecount(finfo);
            } else {
                processed = 1;
            }
        } else if(eos_audio) {
            if(finfo->fifo_hndlr) {
                AmbaNetFifo_Delete(finfo->fifo_hndlr);
                finfo->fifo_hndlr = 0ull;
            }
            clean_framecount(finfo);
        }

        if(running) {
            //Report to RTOS when handlers are all stopped.
            if((g_video_fifo.fifo_hndlr == 0ull) &&
               (g_audio_fifo.fifo_hndlr == 0ull)) {
                if(switch_enc) {
                    AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_SWITCHENCSESSION);
                } else {
                    AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_END);
                }
                running = 0;
                eos_video = 0;
                eos_audio = 0;
                switch_enc = 0;
                if(!daemon_mode) {
                    break;
                }
            }
        }

        if(!processed) {
            usleep(10000);
        }
    }

    return 0;
}

