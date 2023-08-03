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

#define FIN_WIDTH   (1920)
#define FIN_PITCH   (1984)
#define FIN_HEIGHT  (1080)
#define CHNUM   (1)

typedef struct {
    char *filename[CHNUM];
    uint32_t frameid[CHNUM];
    flexidag_memblk_t framebuf[CHNUM];
} PROGRAM_PARAMS_s;

static PROGRAM_PARAMS_s g_params = {0};
static uint8_t enable_finput = 0;
static char default_flexidagpath[]="./flexibin";

uint32_t debug; //show debug message

/*
 * Command line arguments.
 */
#define OPTSTRING "c:e:v"
static struct option longopts[] =
{
    { "fn_0",   required_argument,  NULL, 'c', },
    { "fid_0",  required_argument,  NULL, 'e', },
    { "en_dbg", no_argument,        NULL, 'v', },
    { NULL, 0, NULL, 0, },
};

/**
 * disply usage of parameters
 **/
static void ShowHelp(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t--fn_0 <filename>\t enable file input mode for flexidag_io CH0 flow.\n");
    printf("\t--fid_0 <frame id>\t specify frame id for CH0. (only for file input mode)\n");
    printf("default is to enable flexidag_io CH0 handle flow only! (single vin)\n");
    printf("It will be forced as file input mode if --fn_0 specified.\n");
    printf("For file input mode, you have to specify the frame id with --fid_0.\n");
}

/**
 * parse program parameters
 **/
static void ParseOpt(int argc, char **argv)
{
    int i;
    while ((i = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (i) {
        case 'c':
            enable_finput = 1;
            g_params.filename[0] = optarg;
            break;
        case 'e':
            g_params.frameid[0] = atoi(optarg);
            break;
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

        /* generate OSD */
        retcode = GiveResult(ch, RecvData.pic_info.frame_num, result);
        if (retcode != 0) {
            printf("[Ch%u] GiveResult fail!\n",ch);
            break;
        }
    }

    fprintf(stderr, "[Ch%u] Thread exit!(%u)\n", ch, retcode);
    pthread_exit(NULL);
}

static uint32_t PackPicInfo(uint32_t ch, memio_source_recv_picinfo_t *outPicInfo)
{
    cv_pic_info_t *pic_info;
    idsp_pyramid_t *pyramid;
    half_octave_info_t *half_octave;
    uint32_t i;

    pic_info = &(outPicInfo->pic_info);
    memset(pic_info, 0, sizeof(pic_info));
    pic_info->frame_num = g_params.frameid[ch];
    pic_info->capture_time = g_params.frameid[ch];

    /* fake all pyramid layer as the same setting */
    pyramid = &(pic_info->pyramid);
    pyramid->image_width_m1 = FIN_WIDTH - 1;
    pyramid->image_height_m1 = FIN_HEIGHT - 1;
    pyramid->image_pitch_m1 = FIN_PITCH - 1;
    for (i=0; i<MAX_HALF_OCTAVES; i++) {
        half_octave = &(pyramid->half_octave[i]);
        half_octave->ctrl.roi_pitch = FIN_PITCH;
        half_octave->roi_start_col = 0;
        half_octave->roi_start_row = 0;
        half_octave->roi_width_m1 = FIN_WIDTH - 1;
        half_octave->roi_height_m1 = FIN_HEIGHT - 1;

        pic_info->rpLumaLeft[i] = g_params.framebuf[ch].buffer_daddr; //physical address
        pic_info->rpChromaLeft[i] = pic_info->rpLumaLeft[i] + (FIN_PITCH * FIN_HEIGHT);
        pic_info->rpLumaRight[i] = pic_info->rpLumaLeft[i];
        pic_info->rpChromaRight[i] = pic_info->rpChromaLeft[i];
    }

#if 0
    { //save input content for checking
        char fn[128];
        FILE *out;
        uint8_t *data;

        sprintf(fn,"./input_%u_%ux%u.y", ch, pyramid->image_pitch_m1+1, pyramid->image_height_m1+1);
        out = fopen(fn, "wb");
        if (out != NULL) {
            data = (uint8_t *)ambacv_p2v(pic_info->rpLumaLeft[0]);
            fwrite(data, 1, ((pyramid->image_pitch_m1+1) * (pyramid->image_height_m1+1)), out);
            fclose(out);
        }

        sprintf(fn,"./input_%u_%ux%u.uv", ch, pyramid->image_pitch_m1+1, pyramid->image_height_m1+1);
        out = fopen(fn, "wb");
        if (out != NULL) {
            data = (uint8_t *)ambacv_p2v(pic_info->rpChromaLeft[0]);
            fwrite(data, 1, ((pyramid->image_pitch_m1+1) * (pyramid->image_height_m1+1))/2, out);
            fclose(out);
        }
    }
#endif

    return 0;
}

static uint32_t GetFramebyFile(uint32_t ch, memio_source_recv_picinfo_t *outPicInfo)
{
    uint32_t retcode;
    FILE *fptr;
    uint32_t file_size, frame_size;
    uint8_t *pframebuf = NULL;

    if (g_params.filename[ch] == NULL) {
        printf("GetFramebyFile[%u]: invalid Filename!\n",ch);
        return 1;
    }
    if (outPicInfo == NULL) {
        printf("GetFramebyFile[%u]: invalid outPicInfo!\n",ch);
        return 1;
    }

    /* create frame buffer for yuv420 frame */
    frame_size = (FIN_PITCH * FIN_HEIGHT) * 3 / 2;
    retcode = MemUtil_MemblkAlloc(CACHE_ALIGN(frame_size), &g_params.framebuf[ch]);
    if(retcode != 0) {
        printf("Fail to create framebuf[%u] buffer!! ret=%u\n", ch, retcode);
        return 2;
    }
    pframebuf = g_params.framebuf[ch].pBuffer;

    /* read input file */
    do {
        fptr = fopen(g_params.filename[ch], "rb");
        if (fptr == NULL) {
            printf("GetFramebyFile[%u]: fail to open file[%s]!\n", ch, g_params.filename[ch]);
            retcode = 3;
            break;
        }
        //get file size
        fseek(fptr, 0, SEEK_END);
        file_size = ftell(fptr);
        fseek(fptr, 0, SEEK_SET);

        //file has to be in yuv420 format
        if (file_size != frame_size) {
            printf("GetFramebyFile[%u]: file_size(%u) is not as expected(%u).\n", ch, file_size, frame_size);
            printf("Please check your input file. It should be %ux%u in yuv420(nv12) format.\n",FIN_PITCH,FIN_HEIGHT);
            printf("(width is %u with extra 64 padding pixels.)\n",FIN_WIDTH);
            retcode = 3;
            break;
        }

        //read
        retcode = fread(pframebuf, 1, file_size, fptr);
        if (retcode != file_size) {
            printf("GetFramebyFile[%u]: fail to read file! got %u, expect %u.\n", ch, retcode, file_size);
            retcode = 3;
            break;
        }
        retcode = 0;
    } while(0);
    if (fptr != NULL) {
        fclose(fptr);
    }

    /* wrap input as memio_source_recv_picinfo_t */
    if (retcode == 0) {
        retcode = PackPicInfo(ch, outPicInfo);
    }

    return retcode;
}

static uint32_t SaveResult(uint32_t ch, uint32_t frameId, void *result)
{
    uint32_t retcode = 0;
    flexidag_memblk_t *res_blk = NULL;
    FILE *out;

    /* Retrieve output */
    res_blk = (flexidag_memblk_t *)result;
    if (res_blk->pBuffer == NULL) {
        fprintf(stderr, "res_blk->pBuffer is NULL!!\n");
        return 1;
    } else {
        char out_fn[128];

        sprintf(out_fn, "./result_%u_%u.bin", ch, frameId);
        out = fopen(out_fn, "wb");
        if (out == NULL) {
            printf("SaveResult[%u]: fail to open file [%s]!\n", ch, out_fn);
            return 2;
        }
    }

    fwrite(res_blk->pBuffer, 1, res_blk->buffer_size, out);
    fclose(out);

    return retcode;
}

/**
 * Implementation for file input handling.
 * It will retrieve frame from file, feed into flexidag and then save result to SD card.
 **/
void *Thread_Fin_Ch0(void *data)
{
    uint32_t retcode = 0, ch = 0;
    char *path;
    memio_source_recv_picinfo_t RecvData;
    flexidag_memblk_t *result;

    if (g_params.filename[ch] == NULL) {
        printf("invalid input filename[%u]! please specify it in command parameter\n",ch);
        return NULL;
    }

    if (data != NULL) {
        path = (char *)data;
    } else {
        path = &default_flexidagpath[0];
    }

    retcode = DagFlow_Init(ch,path);
    if (retcode != 0) {
        printf("[Ch%u] DagFlow_Init fail!\n", ch);
        return NULL;
    }

    do {
        /* retrieve Frame */
        retcode = GetFramebyFile(ch, &RecvData);
        if (retcode != 0) {
            printf("[Ch%u] GetFramebyFile fail!\n",ch);
            break;
        }

        /* Feed into flexidag(s) and get result */
        retcode = DagFlow_Process(ch, &RecvData, (void **)&result);
        if (retcode != 0) {
            printf("[Ch%u] DagFlow_Process fail!\n",ch);
            break;
        }

        /* save result */
        retcode = SaveResult(ch, RecvData.pic_info.frame_num, result);
        if (retcode != 0) {
            printf("[Ch%u] SaveResult fail!\n",ch);
            break;
        }
    } while(0);

    fprintf(stderr, "[Ch%u] Thread exit!(%u)\n", ch, retcode);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    uint32_t retcode;
    pthread_t t_ch0;
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

    if (enable_finput) {
        pthread_create(&t_ch0, NULL, Thread_Fin_Ch0, NULL);
    } else {
        pthread_create(&t_ch0, NULL, Thread_Ch0, NULL);
    }

    pthread_join(t_ch0, &ret);

    /* Program exit */
    printf("main process finish.\n");
    return 0;
}

