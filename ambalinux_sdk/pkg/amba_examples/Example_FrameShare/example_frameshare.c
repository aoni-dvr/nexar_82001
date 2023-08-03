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
#include "AmbaIPC_RpcProg_LU_ExamFrameShare.h"


/**
 *  debug or workaround
 */
#define HAVE_PRVATE_MEMCPY
#include "local_mem_util.h"

#define err(str, args...)       do{ fprintf(stderr, "%s[#%d]" str, __func__, __LINE__, ## args); }while(0)
#define msg(str, args...)       do{ fprintf(stderr, str, ## args); }while(0)
#if 0
    #define DEBUG(str, args...)       do{ fprintf(stderr, "%s[#%d]"str, __func__, __LINE__, ## args); usleep(20000);}while(0)
#else
    #define DEBUG(str, args...)
#endif


#define MAX_CACHED_DESC_NUM (256)
#define FRAME_MARK_EOS (0x00FFFFFF)

typedef enum _CONTROL_CMD_e_ {
    CMD_STARTENC = 1, /**< Start encode.*/
    CMD_STOPENC, /**< Stop encode*/
} CONTROL_CMD_e;

typedef enum _FRAME_TYPE_e_ {
    FRAME_TYPE_MJPEG_FRAME = 0,    ///< MJPEG frame type
    FRAME_TYPE_IDR_FRAME = 1,    ///< Idr frame type
    FRAME_TYPE_I_FRAME = 2,      ///< I frame type
    FRAME_TYPE_P_FRAME = 3,      ///< P frame type
    FRAME_TYPE_B_FRAME = 4,      ///< B frame type
    FRAME_TYPE_JPEG_FRAME = 5,   ///< jpeg main frame
    FRAME_TYPE_THUMBNAIL_FRAME = 6,  ///< jpeg thumbnail frame
    FRAME_TYPE_SCREENNAIL_FRAME = 7, ///< jpeg screennail frame
    FRAME_TYPE_AUDIO_FRAME = 8,      ///< audio frame
    FRAME_TYPE_UNDEFINED = 9,        ///< others

    FRAME_TYPE_EOS = 255,                  ///< eos bits that feed to raw buffer

    FRAME_TYPE_LAST = FRAME_TYPE_EOS
} FRAME_TYPE_e;

typedef enum _MEDIA_TYPE_e_ {
    MEDIA_TYPE_VIDEO = 1,  /**< The type is Video */
    MEDIA_TYPE_AUDIO = 2,  /**< The type is Audio */
    MEDIA_TYPE_TEXT = 3,   /**< The type is Text */
    MEDIA_TYPE_MAX = 4     /**< Max value, for check use */
} MEDIA_TYPE_e;

typedef struct efs_buffer_info_s {
    LU_EXAMFRAMESHARE_FRAME_DESC_s *desc_buf;
    unsigned char *desc_buf_limit;
    LU_EXAMFRAMESHARE_FRAME_DESC_s *wp;
    LU_EXAMFRAMESHARE_FRAME_DESC_s *rp;
    unsigned int frame_count;
} efs_buffer_info_t;

typedef struct efs_fifo_info_s {
    void *hndlr;
    void *base;
    void *limit;
    efs_buffer_info_t buf_info;
    pthread_mutex_t mutex;
    unsigned char *mmap_base;
    unsigned int mmap_size;
    long long mmap_offset;
} efs_fifo_info_t;


//To keep fifo/frame info
static efs_fifo_info_t *g_video_fifo = NULL;
static efs_fifo_info_t *g_audio_fifo = NULL;

//program status
static int efs_status = 0;
static unsigned char daemon_mode = 0;
static unsigned char debug_mode = 0;
static unsigned char showframe_mode = 0;

static unsigned char running = 0;
static unsigned char eos_video = 0;
static unsigned char eos_audio = 0;

//DEBUG
static unsigned int v_frame_count = 0;
static unsigned int a_frame_count = 0;


static int init_RPC(void)
{
    int rval = 0;
    int func_slot;
    AMBA_IPC_PROG_INFO_s prog_info;

    if (debug_mode) {
        msg("%s\n", __FUNCTION__);
    }
    prog_info.ProcNum = LU_EXAMFRAMESHARE_FUNC_AMOUNT - 1;
    prog_info.pProcInfo = malloc(prog_info.ProcNum * sizeof(AMBA_IPC_PROC_s));

    func_slot = LU_EXAMFRAMESHARE_FUNC_CONTROLEVENT - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARE_ControlEvent_Svc;

    func_slot = LU_EXAMFRAMESHARE_FUNC_FRAMEEVENT - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARE_FrameEvent_Svc;

    func_slot = LU_EXAMFRAMESHARE_FUNC_SETENCINFO - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARE_SetEncInfo_Svc;

    rval = ambaipc_svc_register(LU_EXAMFRAMESHARE_PROG_ID,
                                LU_EXAMFRAMESHARE_VER,
                                LU_EXAMFRAMESHARE_NAME,
                                &prog_info, 1);
    if(rval != 0) {
        err("Error to do ambaipc_svc_register %d\n ", rval);
        return -1;
    }
    efs_status = 1;

    free(prog_info.pProcInfo);
    return 0;
}

static void release_RPC(void)
{
    int rval;

    if (efs_status != 0) {
        rval = ambaipc_svc_unregister(LU_EXAMFRAMESHARE_PROG_ID, LU_EXAMFRAMESHARE_VER);
        if (rval == 0) {
            efs_status = 0;
        } else {
            err("%s", "fail to do ambaipc_svc_unregister()\n");
        }
    }
}

//phy_addr and size need to be 4K alignment. (page size)
static unsigned char *do_mmap(unsigned long long phy_addr, unsigned int size)
{
    unsigned char *map_base;
    int fd;

    msg("phy_addr= x%llx, size= %u\n", phy_addr, size);

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
        msg("OK ! mmap x%llx ~ x%llx to %p ~ %p, size= %u\n",
            phy_addr, phy_addr + size,
            map_base, map_base + size,
            size);
    }

    close(fd);
    return map_base;
}

static int do_munmap(efs_fifo_info_t *finfo)
{
    if(finfo->mmap_base != NULL) {
        if(munmap(finfo->mmap_base, finfo->mmap_size) != 0) {
            perror("Fail to do munmap:");
            return -1;
        }
        finfo->mmap_base = NULL;
        finfo->mmap_size = 0;
        finfo->mmap_offset = 0;
    }

    return 0;
}

static efs_fifo_info_t *create_finfo(void)
{
    efs_fifo_info_t *tmp_finfo;

    tmp_finfo = (efs_fifo_info_t *)malloc(sizeof(efs_fifo_info_t));
    if(tmp_finfo == NULL) {
        err("%s", "Fail to malloc efs_fifo_info_t!\n");
        return NULL;
    }
    memset(tmp_finfo, 0, sizeof(efs_fifo_info_t));
    if(pthread_mutex_init(&tmp_finfo->mutex, NULL) != 0) {
        err("%s", "Fail to create mutex!\n");
    }
    tmp_finfo->buf_info.desc_buf = (LU_EXAMFRAMESHARE_FRAME_DESC_s *)malloc(sizeof(LU_EXAMFRAMESHARE_FRAME_DESC_s) * MAX_CACHED_DESC_NUM);
    if(tmp_finfo->buf_info.desc_buf == NULL) {
        err("%s", "Fail to malloc desc_buf!\n");
        return NULL;
    }
    memset(tmp_finfo->buf_info.desc_buf, 0, (sizeof(LU_EXAMFRAMESHARE_FRAME_DESC_s)*MAX_CACHED_DESC_NUM));

    tmp_finfo->buf_info.wp = tmp_finfo->buf_info.desc_buf;
    tmp_finfo->buf_info.rp = tmp_finfo->buf_info.desc_buf;
    tmp_finfo->buf_info.desc_buf_limit = (unsigned char *)tmp_finfo->buf_info.desc_buf + (sizeof(LU_EXAMFRAMESHARE_FRAME_DESC_s) * MAX_CACHED_DESC_NUM);

    return tmp_finfo;
}

static void delet_finfo(efs_fifo_info_t *finfo)
{
    if(finfo == NULL) {
        return;
    }

    if(finfo->buf_info.desc_buf != NULL) {
        free(finfo->buf_info.desc_buf);
        finfo->buf_info.desc_buf = NULL;
    }
    if(finfo->mmap_base != NULL) {
        do_munmap(finfo);
    }
    pthread_mutex_destroy(&finfo->mutex);
    free(finfo);
}

/**
 * Clean up resource when program exit.
 */
static void cleanup(void)
{
    release_RPC();

    delet_finfo(g_video_fifo);
    g_video_fifo = NULL;

    delet_finfo(g_audio_fifo);
    g_audio_fifo = NULL;
}

/**
 * Linux signal handler.
 */
static void signalHandlerShutdown(int sig)
{
    err("Got signal %d, program exits!\n", sig);
    exit(0);
}

/**
 * Increase valid frame count.
 */
static inline void inc_frame(efs_fifo_info_t *finfo)
{
    pthread_mutex_lock(&finfo->mutex);
    finfo->buf_info.wp++;
    if((unsigned char *)finfo->buf_info.wp >= finfo->buf_info.desc_buf_limit) {
        finfo->buf_info.wp = finfo->buf_info.desc_buf;
    }
    finfo->buf_info.frame_count++;
    pthread_mutex_unlock(&finfo->mutex);
}

/**
 * Decrease valid frame count.
 */
static inline void dec_frame(efs_fifo_info_t *finfo)
{
    pthread_mutex_lock(&finfo->mutex);
    finfo->buf_info.rp++;
    if((unsigned char *)finfo->buf_info.rp >= finfo->buf_info.desc_buf_limit) {
        finfo->buf_info.rp = finfo->buf_info.desc_buf;
    }
    finfo->buf_info.frame_count--;
    pthread_mutex_unlock(&finfo->mutex);
}

/**
 * Reset valid frame count.
 */
static inline void clean_framecount(efs_fifo_info_t *finfo)
{
    pthread_mutex_lock(&finfo->mutex);
    finfo->buf_info.wp = finfo->buf_info.desc_buf;
    finfo->buf_info.rp = finfo->buf_info.desc_buf;
    finfo->buf_info.frame_count = 0;
    pthread_mutex_unlock(&finfo->mutex);
}

/**
 * Check is there any valid frame.
 */
static inline int check_frame(efs_fifo_info_t *finfo)
{
    if(finfo->buf_info.frame_count == 0) {
        return 0;
    }
    return 1;
}

void LU_EXAMFRAMESHARE_ControlEvent_Svc(LU_EXAMFRAMESHARE_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;


    switch (pArg->Cmd) {
        case CMD_STARTENC:
            msg("%s", "RTOS Encode Start!!\n");
            running = 1;
            if(g_video_fifo->hndlr != NULL) {
                eos_video = 0;
            }
            if(g_audio_fifo->hndlr != NULL) {
                eos_audio = 0;
            }
            ret = 0;
            break;
        case CMD_STOPENC:
            msg("%s", "RTOS Encode Stopped!!\n");
            if(g_video_fifo->hndlr != NULL) {
                eos_video = 1;
            }
            if(g_audio_fifo->hndlr != NULL) {
                eos_audio = 1;
            }
            ret = 0;
            break;
        default:
            err("unknown cmd (%u)!\n", pArg->Cmd);
            ret = -1;
            break;
    }

    //prepare RPC result
    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;

    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

void LU_EXAMFRAMESHARE_FrameEvent_Svc(LU_EXAMFRAMESHARE_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    efs_fifo_info_t *finfo = NULL;
    int ret = 0;
    int *i_ptr;

    if(g_video_fifo && (pArg->hndlr == (unsigned long long)(uintptr_t)g_video_fifo->hndlr)) {
        finfo = g_video_fifo;
    } else if(g_audio_fifo && (pArg->hndlr == (unsigned long long)(uintptr_t)g_audio_fifo->hndlr)) {
        finfo = g_audio_fifo;
    } else {
        err("unsupported hndlr x%llx !\n", pArg->hndlr);
        ret = -1;
    }

    if(finfo != NULL) {
        //wait until there is enough space
        while(finfo->buf_info.frame_count >= MAX_CACHED_DESC_NUM) {
            usleep(5000);
        }

        if (debug_mode) {
            msg("\thndlr          = x%llx\n", pArg->hndlr);
            msg("\tdesc->SeqNum   = %d\n", pArg->InfoPtr.SeqNum);
            msg("\tdesc->Pts      = %llu\n", pArg->InfoPtr.Pts);
            msg("\tdesc->Type     = %u\n", pArg->InfoPtr.Type);
            msg("\tdesc->Completed= %u\n", pArg->InfoPtr.Completed);
            msg("\tdesc->Align    = %u\n", pArg->InfoPtr.Align);
            msg("\tdesc->StartAddr= x%llx\n", pArg->InfoPtr.StartAddr);
            msg("\tdesc->Size     = %u\n", pArg->InfoPtr.Size);
        }

        finfo->buf_info.wp->SeqNum = pArg->InfoPtr.SeqNum;
        finfo->buf_info.wp->Pts = pArg->InfoPtr.Pts;
        finfo->buf_info.wp->Type = pArg->InfoPtr.Type;
        finfo->buf_info.wp->Completed = pArg->InfoPtr.Completed;
        finfo->buf_info.wp->Align = pArg->InfoPtr.Align;
        if(finfo->mmap_base != NULL) {
            finfo->buf_info.wp->StartAddr = pArg->InfoPtr.StartAddr + finfo->mmap_offset;
        } else {
            finfo->buf_info.wp->StartAddr = 0ull;
        }
        finfo->buf_info.wp->Size = pArg->InfoPtr.Size;
        DEBUG("StartAddr= x%llx, mmap_offset= x%llx, wp->StartAddr= x%llx\n", pArg->InfoPtr.StartAddr, finfo->mmap_offset, finfo->buf_info.wp->StartAddr);

        inc_frame(finfo);
    }

    //prepare RPC result
    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;

    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

void LU_EXAMFRAMESHARE_SetEncInfo_Svc(LU_EXAMFRAMESHARE_ENCINFO_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    efs_fifo_info_t *finfo = NULL;
    int ret = 0;
    unsigned long long p_addr, b_addr;
    int *i_ptr;

    if(pArg->media_type == MEDIA_TYPE_VIDEO) {
        finfo = g_video_fifo;
    } else if(pArg->media_type == MEDIA_TYPE_AUDIO) {
        finfo = g_audio_fifo;
    } else {
        err("unsupported media_type %d!\n", pArg->media_type);
        ret = -1;
    }

    if(finfo != NULL) {
        if (debug_mode) {
            msg("got: hndlr=x%llx, media_type=%u, base=x%llx, phy_base=x%llx, size=%u\n",
                pArg->hndlr, pArg->media_type, pArg->buf_base, pArg->buf_base_phy, pArg->buf_size);
        }

        if(finfo->hndlr != NULL) {
            err("Enc info for media_type %d already exists!\n", pArg->media_type);
            ret = -2;
        } else {
            finfo->hndlr = (void *)(uintptr_t)pArg->hndlr;
            finfo->base = (void *)(uintptr_t)pArg->buf_base;
            finfo->limit = (void *)(uintptr_t)(pArg->buf_base + pArg->buf_size);

            //do mmap
            p_addr = (unsigned long long)pArg->buf_base_phy;
            b_addr = (unsigned long long)pArg->buf_base;
            finfo->mmap_size = pArg->buf_size;

            if ((finfo->mmap_size & 0x0fff) != 0) { //must be 4K aligned
                finfo->mmap_size = (finfo->mmap_size + 0x1000) & (~0x0fff);
            }

            if ((p_addr & 0x0fff) != 0) { //must be 4K align
                p_addr = p_addr & (~0x0fff); //aligned to 4k for mmap
                b_addr = b_addr & (~0x0fff); //aligned to 4k
                finfo->mmap_size += 0x1000;
                msg("mmap addr is not aligned to 4K (x%llx to 0x%llx)!!\n",
                    pArg->buf_base_phy, p_addr);
            }

            finfo->mmap_base = do_mmap(p_addr, finfo->mmap_size);
            if(finfo->mmap_base != NULL) {
                finfo->mmap_offset = (unsigned long long)(uintptr_t)(finfo->mmap_base - b_addr);
                finfo->limit += finfo->mmap_offset;
                finfo->base  += finfo->mmap_offset;
            } else {
                finfo->mmap_size = 0;
                err("%s", "do mmap fail \n");
            }
        }
    }

    //prepare RPC result
    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;

    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

/**
 * Check frame type for video.
 */
static inline int is_VideoFrame(unsigned char desc_type)
{
    int rval = 0;

    switch(desc_type) {
        case FRAME_TYPE_IDR_FRAME:
        case FRAME_TYPE_I_FRAME:
        case FRAME_TYPE_P_FRAME:
        case FRAME_TYPE_B_FRAME:
        case FRAME_TYPE_EOS:
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
        case FRAME_TYPE_AUDIO_FRAME:
        case FRAME_TYPE_EOS:
            rval = 1;
            break;
        default:
            rval = 0;
            break;
    }

    return rval;
}

static void dump_data(unsigned char *data, unsigned int len)
{
    int i;

    msg("Data[%d]:\n", len);
    for(i = 0; i < len; i++) {
        if((i & 0xf) == 0xf) {
            msg("%02x\n", data[i]);
        } else {
            msg("%02x ", data[i]);
        }
    }
    msg("\n");
}

static void show_framedata(LU_EXAMFRAMESHARE_FRAME_DESC_s *desc, void *base, void *limit)
{
    unsigned char *fdata, *r_ptr, *w_ptr;
    long len;

    if(desc->StartAddr == 0ull) {
        err("%s", "Frame address is NULL! Maybe mmap() fail or not done yet.\n");
        return;
    }

    fdata = (unsigned char *) malloc(desc->Size);
    if(fdata == NULL) {
        err("%s", "fail to malloc frame data!\n");
        return;
    }

    r_ptr = (unsigned char *)(uintptr_t)desc->StartAddr;
    DEBUG("%p= %02x, %02x %02x %02x, Size= %u\n",
          r_ptr, r_ptr[0], r_ptr[1], r_ptr[2], r_ptr[3], desc->Size);
    w_ptr = fdata;
    len = desc->Size;
    if((r_ptr + len) >= (unsigned char *)limit) {
        len = (unsigned char *)limit - r_ptr;
        memcpy(w_ptr, r_ptr, len);
        r_ptr = (unsigned char *)base;
        w_ptr += len;
        len = desc->Size - len;
    }
    memcpy(w_ptr, r_ptr, len);

    msg("Frame[%d], type=%d, PTS=%llu\n", desc->SeqNum, desc->Type, desc->Pts);
    dump_data(fdata, desc->Size);
    free(fdata);
}

/**
 * processing video frame.
 */
static int process_video_frame(efs_fifo_info_t *finfo)
{
    LU_EXAMFRAMESHARE_FRAME_DESC_s *desc;
    int rval = 0;

    desc = finfo->buf_info.rp;
    if((desc->Type == FRAME_TYPE_EOS) || (desc->Size == FRAME_MARK_EOS)) { //It is EOS
        err("video size is EOS mark!(%x)\n", desc->Size);
        rval = -99;
    } else {
        if(!is_VideoFrame(desc->Type)) {
            err("It is not Video frame!(%x)\n", desc->Type);
            return 0;
        }

        //Do frame precess here...
        v_frame_count++;
        if((v_frame_count & 0xff) == 0) {
            msg("got %u video frames. PTS=%llu\n", v_frame_count, desc->Pts);
        }
        if (showframe_mode) {
            if (desc->Type == FRAME_TYPE_IDR_FRAME) {
                show_framedata(desc, finfo->base, finfo->limit);
            }
        }
    }

    dec_frame(finfo);

    return rval;
}

/**
 * processing audio frame.
 */
static int process_audio_frame(efs_fifo_info_t *finfo)
{
    LU_EXAMFRAMESHARE_FRAME_DESC_s *desc;
    int rval = 0;

    desc = finfo->buf_info.rp;
    if((desc->Type == FRAME_TYPE_EOS) || (desc->Size == FRAME_MARK_EOS)) { //It is EOS
        err("audio size is EOS mark!(%x)\n", desc->Size);
        rval = -99;
    } else {
        if(!is_AudioFrame(desc->Type)) {
            err("It is not Audio frame!(%x)\n", desc->Type);
            return 0;
        }

        //Do frame precess here...
        a_frame_count++;
        if((a_frame_count & 0xff) == 0) {
            msg("got %u audio frames\n", a_frame_count);
            if (showframe_mode) {
                show_framedata(desc, finfo->base, finfo->limit);
            }
        }
    }

    dec_frame(finfo);

    return rval;
}

static void usage(int argc, char **argv)
{
    fprintf(stderr, "Usage: %s [Options]\n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-d: run as daemon mode (will not exit while record stop)\n");
    fprintf(stderr, "\t-v: enable more debug message\n");
    fprintf(stderr, "\t-f: show Frame data\n");
    exit(EXIT_FAILURE);
}

static struct option longopts[] = {
    { "en_daemon",   no_argument,    NULL, 'd', },
    { "en_debug",    no_argument,    NULL, 'v', },
    { "en_showframe",    no_argument,    NULL, 'f', },
    { NULL, 0, NULL, 0, },
};

int main (int argc, char *argv[])
{
    int rval = 0, i, processed = 0;
    efs_fifo_info_t *finfo;

    daemon_mode = 0;
    debug_mode = 0;
    showframe_mode = 0;

    /* Get command line options */
    while ((i = getopt_long(argc, argv, "dvf", longopts, NULL)) != -1) {
        switch (i) {
            case 'd':
                daemon_mode = 1;
                break;
            case 'v':
                debug_mode = 1;
                break;
            case 'f':
                showframe_mode = 1;
                break;
            default:
                usage(argc, argv);
                break;
        }
    }

    daemon(0, 1);

    //init NetFiFo
    rval = init_RPC();
    if(rval < 0) {
        err("%s", "@@ Fail to do init_RPC()\n");
        return -1;
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
    running = 0;
    eos_video = 0;
    eos_audio = 0;

    g_video_fifo = create_finfo();
    if(g_video_fifo == NULL) {
        err("%s", "Fail to malloc g_video_fifo!\n");
        return -1;
    }

    g_audio_fifo = create_finfo();
    if(g_audio_fifo == NULL) {
        err("%s", "Fail to malloc g_video_fifo!\n");
        return -1;
    }

    //process frame
    for(processed = 0;; processed = 0) {
        finfo = g_video_fifo;
        if(check_frame(finfo)) {
            rval = process_video_frame(finfo);
            if(rval == -99) { //EOS
                finfo->hndlr = NULL;
                clean_framecount(finfo);
            } else {
                processed = 1;
            }
        } else if(eos_video) {
            if(finfo->hndlr != NULL) {
                finfo->hndlr = NULL;
            }
            clean_framecount(finfo);
        }

        finfo = g_audio_fifo;
        if (check_frame(finfo)) {
            rval = process_audio_frame(finfo);
            if(rval == -99) { //EOS
                finfo->hndlr = NULL;
                clean_framecount(finfo);
            } else {
                processed = 1;
            }
        } else if(eos_audio) {
            if(finfo->hndlr != NULL) {
                finfo->hndlr = NULL;
            }
            clean_framecount(finfo);
        }

        if(running) {
            //reset
            if((g_video_fifo->hndlr == NULL) &&
               (g_audio_fifo->hndlr == NULL)) {
                running = 0;
                eos_video = 0;
                eos_audio = 0;
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

