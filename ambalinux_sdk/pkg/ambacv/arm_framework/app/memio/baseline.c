#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <signal.h>
#include <execinfo.h> /* for backtrace() */

#include "schdr_api.h"
#include "schdr_util.h"
#include "memio.h"

#define DEFAULT_KEY     0xaabbccddeeff
#define DEFAULT_INET    "127.0.0.1"
#define DEFAULT_PORT    8699
#define OPTIONS         "k:s:t:f:b:d:o:m:n:x:y:z:h:w:"

static int boot_flag = 1;
static int cvtask_connected = 0;
static MEMIO_INFO_s memio_info = {MEMIO_MODE_CVTASK_L_APP_RR, 1, 0, "/tmp/SD0/1.yuv", "/tmp/SD0/1/", "18-03-25-000025-kr.mp4_"};

static void print_help(const char *exe)
{
    printf("\nusage: %s [args]\n", exe);
    printf("\t-b arg: set memort mode 0[malloc]/1[mmap]/2[sharedmem]\n");
    printf("\t-s arg: pairing server inet address\n");
    printf("\t-k arg: 64bit pairing key\n");
    printf("\t-d arg: set dram size\n");
    printf("\t-m arg: set memio mode 0[app in rtos, cvtask in linux]/1[app in linux, cvtask in linux]\n");
    printf("\t                       2[source app in rtos, sink app in linux, cvtask in linux]\n");
    printf("\t                       3[source app in rtos, sink app in linux, cvtask in rtos]\n");
    printf("\t-n arg: set memio debug mode\n");
    printf("\t-x arg: set memio type 0[classification]/1[objdetection]/2[segmentation]\n");
    printf("\t-y arg: set memio input file\n");
    printf("\t-z arg: set memio output folder\n");
    printf("\t-t arg: cvtable file name\n");
    printf("\t-f arg: system flow file name\n");
    printf("\t-o arg: number of sub-schedulers, default 1\n");
    printf("\nfor example: %s ", exe);
    printf("-s 10.1.11.16 -k 0x1234567890\n\n");
}

static int load_binary(char *buff, int max_size, void *token)
{
    const char *name = (const char *)token;
    FILE *ifp;
    int size;

    ifp = fopen(name, "rb");
    if (ifp == NULL) {
        printf("Can't open input file %s!\n", name);
        exit(-1);
    }
    fseek(ifp, 0, SEEK_END);
    size = ftell(ifp);

    if (buff != NULL) {
        if (size > max_size) {
            printf("The provided buff too small for %s!\n", name);
            exit(-1);
        }
        fseek(ifp, 0, SEEK_SET);
        assert(size == fread(buff, 1, size, ifp));
    }

    fclose(ifp);
    return size;
}

static void set_memio_mode(int mode)
{
    memio_info.mode = mode;
}

static void set_memio_type(int type)
{
    memio_info.type = type;
}

static void set_memio_debug(int debug)
{
    memio_info.debug = debug;
}

static void set_memio_input(char *input)
{
    strncpy(memio_info.input, input, sizeof(memio_info.input));
}

static void set_memio_output(char *output)
{
    strncpy(memio_info.output, output, sizeof(memio_info.output));
}

static void set_memio_output_filename_prefix(char *output)
{
    memio_info.outputfilename_prefix[127] = '\0';
    strncpy(memio_info.outputfilename_prefix, output, sizeof(memio_info.outputfilename_prefix) - 1);
}

static void add_cvtable(char *path)
{
    static int cvtable_cnt;
    if (++cvtable_cnt > MAX_CVTABLE_COUNT) {
        printf("Too many cvtables! limit is %d\n", MAX_CVTABLE_COUNT);
        exit(-1);
    }
    schdr_add_cvtable((void*)path, load_binary);
}

static void add_sysflow(char *path)
{
    static int sysflow_cnt;
    if (++sysflow_cnt > MAX_SYSFLOW_COUNT) {
        printf("Too many sysflow! limit is %d\n", MAX_SYSFLOW_COUNT);
        exit(-1);
    }
    schdr_add_sysflow((void*)path, load_binary);
}

static void parse_app_opt(int argc, char**argv)
{
    int c;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'm':
            set_memio_mode(strtoul(optarg, NULL, 0));
            break;
        case 'n':
            set_memio_debug(strtoul(optarg, NULL, 0));
            break;
        case 'x':
            set_memio_type(strtoul(optarg, NULL, 0));
            break;
        case 'y':
            set_memio_input(optarg);
            break;
        case 'z':
            set_memio_output(optarg);
            break;
        case 'w':
            set_memio_output_filename_prefix(optarg);
            break;
        default:
            break;
        }
    }
}

static void parse_amalgam_opt(int argc, char**argv)
{
    int c, port = DEFAULT_PORT;
    char *inet = DEFAULT_INET;
    long long unsigned int key = DEFAULT_KEY;
    unsigned int dram_size = 0x20000000;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'b':
            schdr_set_dram_mode(SCHDR_DRAM_MODE_SHM, optarg);
            break;
        case 'k':
            key = strtoull(optarg, NULL, 0);
            break;
        case 's':
            inet = optarg;
            break;
        case 'd':
            dram_size = strtoul(optarg, NULL, 0);
            break;
        case 'o':
            // This is not an amalgam option.
            // However, boot_flag is needed for schdr_init.
            boot_flag = strtol(optarg, NULL, 0);
            break;
        default:
            break;
        }
    }
    amalgam_config(inet, port, key, dram_size);
}

static void parse_schdr_opt(int argc, char **argv)
{
    int c;

    optind = 1;
    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c) {
        case 'b':
        case 'k':
        case 's':
        case 'd':
        case 'o':
        case 'm':
        case 'n':
        case 'x':
        case 'y':
        case 'z':
        case 'w':
            break;
        case 't':
            add_cvtable(optarg);
            break;
        case 'f':
            add_sysflow(optarg);
            break;
        default:
            print_help(argv[0]);
            exit(-1);
        }
    }
}

static int app_callback(int type, void *arg)
{
    int i, ret = 0;

    switch(type) {
    case SCHDR_CB_START_REGISTRATION:
        printf("====BASELINE starts cvtask registration\n");
        break;
    case SCHDR_CB_START_QUERY:
        printf("====BASELINE starts cvtask query\n");
        break;
    case SCHDR_CB_START_INIT:
        printf("====BASELINE is about to init cvtasks\n");
        break;
    case SCHDR_CB_START_RUN:
        printf("====BASELINE is about to run cvtasks\n");
        cvtask_connected = 1;
        break;
    case SCHDR_CB_START_SHUTDOWN:
        printf("====SUPERDAG is shutting down\n");
        MemIO_Finish(&memio_info);
        /* return -1 to allow the framework to take default action */
        ret = -1;
        break;
    default:
        printf("Warning: skip unknow callback type %d\n", type);
        ret = -1;
    }
    return ret;
}

static void cleanup(void)
{
    MemIO_TriggerTask(&memio_info);
    if (memio_info.mode == MEMIO_MODE_CVTASK_R_APP_RL){
        MemIO_Finish(&memio_info);
    } else {
        schdr_shutdown(0);
    }
}

static void signalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n",__FILE__,sig);
    exit(0);
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

int main(int argc, char **argv)
{
    atexit(cleanup);
    /* Allow ourselves to be shut down gracefully by a signal */
    signal(SIGTERM, signalHandlerShutdown);
    signal(SIGHUP, signalHandlerShutdown);
    signal(SIGUSR1, signalHandlerShutdown);
    signal(SIGQUIT, signalHandlerShutdown);
    signal(SIGINT, signalHandlerShutdown);
    signal(SIGKILL, signalHandlerShutdown);
    signal(SIGSEGV, SEGVhandler);

    /* parse opt to config app before we init scheduler */
    parse_app_opt(argc, argv);

    /* init memio */
    MemIO_Init(&memio_info);

    if (memio_info.mode != MEMIO_MODE_CVTASK_R_APP_RL){
        schdr_prefetch(0);

        /* parse opt to config amalgam before we init scheduler */
        parse_amalgam_opt(argc, argv);

        /* init scheduler */
        schdr_init(0xD, 0, boot_flag);
        schdr_set_callback(app_callback);

        /* parse the reset of scheduler to add tbar and sysflow */
        parse_schdr_opt(argc, argv);

        /* start the scheduler */
        schdr_start();
        while (cvtask_connected == 0){
        }
    }

    /* start the memio */
    MemIO_Run(&memio_info);
    MemIO_Stop(&memio_info);

    return 0;
}
