#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <getopt.h>

#include "aipc_user.h"
#include "AmbaIPC_RpcProg_RT_SharePreview.h"

/**
 *  debug or workaround
 */
#define HAVE_PRVATE_MEMCPY
#include "local_mem_util.h"


#define err_msg(str, args...)       fprintf(stderr, "%s[#%u] " str, __func__, __LINE__, ## args)

#if 0
    #define dbg_msg(str, args...)       fprintf(stderr, "%s[#%u] " str, __func__, __LINE__, ## args)
#else
    #define dbg_msg(str, args...)
#endif

typedef struct _mmapInfo_s_ {
    unsigned char *base;
    unsigned int size;
    long long offset;
} mmapInfo_s;

static mmapInfo_s WR_mmapInfo;
static mmapInfo_s RD_mmapInfo;
static RT_SHAREPREVIEW_MEMINFO_s RTOS_meminfo;
static CLIENT_ID_t clnt_sharepreview = 0;

static int daemon_mode = 0;
static int debug_mode = 0;

static inline CLIENT_ID_t create_clnt(void)
{
    return ambaipc_clnt_create(RT_SHAREPREVIEW_HOST, RT_SHAREPREVIEW_PROG_ID, RT_SHAREPREVIEW_VER);
}

static inline int delete_clnt(void)
{
    int rval = 0;

    if (clnt_sharepreview != 0) {
        rval = ambaipc_clnt_destroy(clnt_sharepreview);
        if(rval == 0) {
            clnt_sharepreview = 0;
        }
    }

    return rval;
}

static int RT_SharePreview_GetMemInfo(RT_SHAREPREVIEW_MEMINFO_s *minfo)
{
    int status = 0;
    int rval = 0;

    if(minfo == NULL) {
        err_msg("invalid minfo (%p)\n", minfo);
        return -1;
    }

    if (clnt_sharepreview == 0) {
        rval = -1;
        clnt_sharepreview = create_clnt();
        if (clnt_sharepreview == 0) {
            err_msg("%s", "Fail to create client.\n");
            status = -1;
        } else {
            status = 0;
        }
    }

    if(status == 0) {
        status = ambaipc_clnt_call(clnt_sharepreview, RT_SHAREPREVIEW_FUNC_GETSHAREMEMINFO,
                                   NULL, 0,
                                   (void *)minfo, sizeof(RT_SHAREPREVIEW_MEMINFO_s),
                                   RT_SHAREPREVIEW_DEFULT_TIMEOUT);
        if (status != 0) {
            err_msg("fail to do ambaipc_clnt_call(). %d\n", status);
            rval = -1;
        } else {
            rval = 0;
        }
    }

    return rval;
}

static int RT_SharePreview_Enable(int enable)
{
    int status = 0;
    int rt_result = 0;

    if (clnt_sharepreview == 0) {
        rt_result = -1;
        clnt_sharepreview = create_clnt();
        if (clnt_sharepreview == 0) {
            err_msg("%s", "Fail to create client.\n");
            status = -1;
        } else {
            status = 0;
        }
    }

    if(status == 0) {
        status = ambaipc_clnt_call(clnt_sharepreview, RT_SHAREPREVIEW_FUNC_ENABLE,
                                   (void *)&enable, sizeof(int),
                                   (void *)&rt_result, sizeof(int),
                                   RT_SHAREPREVIEW_DEFULT_TIMEOUT);
        if (status != 0) {
            err_msg("fail to do ambaipc_clnt_call(). %d\n", status);
            rt_result = -1;
        } else {
            dbg_msg("RTOS return x%x\n", rt_result);
        }
    }

    return rt_result;
}

static int RT_SharePreview_GetStatus(void)
{
    int status = 0;
    int rval = 0;
    int rt_result;

    if (clnt_sharepreview == 0) {
        rval = -1;
        clnt_sharepreview = create_clnt();
        if (clnt_sharepreview == 0) {
            err_msg("%s", "Fail to create client.\n");
            status = -1;
        } else {
            status = 0;
        }
    }

    if(status == 0) {
        status = ambaipc_clnt_call(clnt_sharepreview, RT_SHAREPREVIEW_FUNC_GETSTATUS,
                                   NULL, 0,
                                   (void *)&rt_result, sizeof(int),
                                   RT_SHAREPREVIEW_DEFULT_TIMEOUT);
        if (status != 0) {
            err_msg("fail to do ambaipc_clnt_call(). %d\n", status);
            rval = -1;
        } else {
            rval = rt_result;
        }
    }

    return rval;
}

//phy_addr and size need to be 4K alignment. (page size)
static unsigned char *do_mmap(unsigned long long phy_addr, unsigned int size)
{
    unsigned char *map_base;
    int fd;

    dbg_msg("phy_addr= x%llx, size= x%x\n", phy_addr, size);

    fd = open("/dev/ppm", O_RDWR | O_SYNC);
    if (fd == -1) {
        perror("Fail to open /dev/ppm");
        return NULL;
    }

    map_base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phy_addr);
    if (map_base == MAP_FAILED) {
        perror("Fail to do mmap");
        close(fd);
        return NULL;
    } else {
        dbg_msg("Successfull! mmap x%llx to x%llx, size= x%x\n", phy_addr, (unsigned long long)map_base, size);
    }

    close(fd);
    return map_base;
}

static int do_munmap(mmapInfo_s *info)
{
    if(info->base != NULL) {
        if(munmap(info->base, info->size) != 0) {
            perror("Fail to do munmap:");
            return -1;
        }
        info->base   = NULL;
        info->size   = 0;
        info->offset = 0ull;
    }

    return 0;
}

static int fnum_count = 0;
static void SharePreview_process_data(void)
{
    RT_SHAREPREVIEW_FRAMEINFO_s *finfo = NULL;
    unsigned char *rp;
    unsigned int active = 0;

    active = ((RT_SHAREPREVIEW_WRINFO_s*)(uintptr_t)RTOS_meminfo.ShareInfo)->Active;
    ((RT_SHAREPREVIEW_RDINFO_s*)(uintptr_t)RTOS_meminfo.ReaderInfo)->Reading = active;
    if(active == 0) {
        err_msg("%s", "active=0, there is no valid data, yet\n");
        return;
    }
    finfo = &((RT_SHAREPREVIEW_WRINFO_s*)(uintptr_t)RTOS_meminfo.ShareInfo)->Finfo[(active - 1)];
    rp    = (unsigned char *)(uintptr_t)(((RT_SHAREPREVIEW_WRINFO_s*)(uintptr_t)RTOS_meminfo.ShareInfo)->Data[(active - 1)] + WR_mmapInfo.offset);

#if 0
    if(finfo->Size <= TEST_PROCESS_DATA_BUFSIZE) {
        memcpy(test_process_data_buffer, rp, finfo->Size);
    } else {
        SHAREPREVIEW_ERR("%s: Frame too large(buf_size=%u)", __FUNCTION__, TEST_PROCESS_DATA_BUFSIZE);

        DEBUG("Process Data:");
        DEBUG("active: %u", active);
        DEBUG("reading: %u", ReaderInfo->reading);
        DEBUG("size: %u", finfo->Size);
        DEBUG("pitch: %u", finfo->Pitch);
        DEBUG("width: %u", finfo->Width);
        DEBUG("height: %u", finfo->Height);
        DEBUG("rp: %p", rp);

        return;
    }
#endif

    dbg_msg("Process Data:\n"
        "\t active : %u\n"
        "\t reading: %u\n"
        "\t size   : %u\n"
        "\t pitch  : %u\n"
        "\t width  : %u\n"
        "\t height : %u\n"
        "\t rp     : %p\n"
        "\n",
        active,
        ((RT_SHAREPREVIEW_RDINFO_s *)(uintptr_t)RTOS_meminfo.ReaderInfo)->Reading,
        finfo->Size, finfo->Pitch, finfo->Width, finfo->Height,
        rp);

#if 1
    do {
        char Fn[64] = {0};
        FILE *fout_y = NULL;

        /* Write RAW_Y to file */
        snprintf(Fn, 64, "/tmp/SD0/PreviewY_pitch_%u_width_%u_height_%u_%06d.y", finfo->Pitch, finfo->Width, finfo->Height, fnum_count);
        fout_y = fopen(Fn, "w+b"); // [WB_MODE]
        fwrite(rp, 1, finfo->Size, fout_y);
        fclose(fout_y);
        fnum_count++;
    } while(0);
#endif

}

/**
 * Clean up resource when program exit.
 */
static void cleanup(void)
{
    RT_SharePreview_Enable(0);
    delete_clnt();
    do_munmap(&WR_mmapInfo);
    do_munmap(&RD_mmapInfo);
}

/**
 * Linux signal handler.
 */
static void signalHandlerShutdown(int sig)
{
    printf("%s: Got signal %d, program exits!\n", __FILE__, sig);
    exit(0);
}

static void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s [Options]\n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-d: run as background daemon\n");
    fprintf(stderr, "\t-v: enable more debug message\n");
    fprintf(stderr, "\t-i [us]: interval (in us) to do frame processing\n");
    exit(EXIT_FAILURE);
}

static struct option longopts[] =
{
    { "en_daemon",  no_argument,        NULL, 'd', },
    { "en_debug",   no_argument,        NULL, 'v', },
    { "interval",   required_argument,  NULL, 'i', },
    { NULL, 0, NULL, 0, },
};

int main (int argc, char *argv[])
{
    int rval = 0, i;
    unsigned int interval = 1000000;

    daemon_mode = 0;
    debug_mode = 0;

    /* Get command line options */
    while ((i = getopt_long(argc, argv, "dvi:", longopts, NULL)) != -1) {
        switch (i) {
            case 'd':
                daemon_mode = 1;
                break;
            case 'v':
                debug_mode = 1;
                break;
            case 'i':
                interval = atoi(optarg);
                dbg_msg("Set interval as %d. (%s)\n", interval, optarg);
                break;
            default:
                usage(argc, argv);
                break;
        }
    }

    if(daemon_mode) {
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

    //init Resource
    memset(&WR_mmapInfo, 0, sizeof(mmapInfo_s));
    memset(&RD_mmapInfo, 0, sizeof(mmapInfo_s));
    memset(&RTOS_meminfo, 0, sizeof(RT_SHAREPREVIEW_MEMINFO_s));
    clnt_sharepreview = 0;

    //check if RTOS enabled Service
    rval = RT_SharePreview_GetStatus();
    if (rval == RT_SHAREPREVIEW_STATUS_NOTINIT) {
        fprintf(stderr, "RTOS Did not enable the SharePreview service! Please check it! %d\n", rval);
        return rval;
    }

    //Get share meminfo and do mmap
    rval = RT_SharePreview_GetMemInfo(&RTOS_meminfo);
    if(rval < 0) {
        fprintf(stderr, "Fail to do RT_SharePreview_GetMemInfo! %d\n", rval);
        return rval;
    }

    do {
        unsigned long long  b_addr;
        unsigned int        size;
        char                *ptr;

        //map WRInfo
        b_addr = (unsigned long long)(RTOS_meminfo.ShareInfoPhyAddr);
        size = RTOS_meminfo.ShareInfoSize;
        //do 4K alignment
        if ((size & 0x0fff) != 0) { //must be 4K aligned
            size = (size + 0x1000) & (~0x0fff);
        }
        if ((b_addr & 0x0fff) != 0) { //must be 4K align
            dbg_msg("ShareInfo addr is not aligned to 4K (x%llx to x%llx)!!", b_addr, (b_addr & (~0x0fff)));
            size += 0x1000;
            b_addr = b_addr & (~0x0fff); //aligned to 4k for mmap;
        }
        WR_mmapInfo.base = do_mmap(b_addr, size);
        WR_mmapInfo.offset = (unsigned long long)(uintptr_t)WR_mmapInfo.base - b_addr;
        WR_mmapInfo.offset -= (unsigned long long)RTOS_meminfo.ShareInfo - (unsigned long long)RTOS_meminfo.ShareInfoPhyAddr;
        WR_mmapInfo.size = size;

        ptr = (char *)(uintptr_t)RTOS_meminfo.ShareInfo;
        ptr += WR_mmapInfo.offset;
        RTOS_meminfo.ShareInfo = (unsigned long long)(uintptr_t)ptr;


        //map RDInfo
        b_addr = (unsigned long long)(RTOS_meminfo.ReaderInfoPhyAddr);
        size = RTOS_meminfo.ReaderInfoSize;
        //do 4K alignment
        if ((size & 0x0fff) != 0) { //must be 4K aligned
            size = (size + 0x1000) & (~0x0fff);
        }
        if ((b_addr & 0x0fff) != 0) { //must be 4K align
            dbg_msg("ReaderInfo addr is not aligned to 4K (x%llx to x%llx)!!", b_addr, (b_addr & (~0x0fff)));
            size += 0x1000;
            b_addr = b_addr & (~0x0fff); //aligned to 4k for mmap;
        }
        RD_mmapInfo.base = do_mmap(b_addr, size);
        RD_mmapInfo.offset = (unsigned long long)(uintptr_t)RD_mmapInfo.base - b_addr;
        RD_mmapInfo.offset -= (unsigned long long)RTOS_meminfo.ReaderInfo - (unsigned long long)RTOS_meminfo.ReaderInfoPhyAddr;
        RD_mmapInfo.size = size;

        ptr = (char *)(uintptr_t)RTOS_meminfo.ReaderInfo;
        ptr += RD_mmapInfo.offset;
        RTOS_meminfo.ReaderInfo = (unsigned long long)(uintptr_t)ptr;
    } while(0);

    RT_SharePreview_Enable(1);

    for(i = 0; i < 10; i++) {
        SharePreview_process_data();
        usleep(interval);
    }

    RT_SharePreview_Enable(0);

    return 0;
}

