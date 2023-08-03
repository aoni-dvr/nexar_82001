#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <getopt.h>
#include <mqueue.h>
#include <time.h>

#include "aipc_user.h"
#include "AmbaIPC_RpcProg_LU_ExamFrameShareRtsp.h"
#include "AmbaIPC_RpcProg_RT_ExamFrameShareRtsp.h"
#include "amba_frameshare.h"
#include "ambastream.h"

//#define DEBUG printf
#define DEBUG(...) //printf(...)

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

typedef enum _MEDIA_TYPE_e_{
    MEDIA_TYPE_VIDEO = 1,  /**< The type is Video */
    MEDIA_TYPE_AUDIO = 2,  /**< The type is Audio */
    MEDIA_TYPE_TEXT = 3,   /**< The type is Text */
    MEDIA_TYPE_MAX = 4     /**< Max value, for check use */
} MEDIA_TYPE_e;

typedef struct efs_fifo_info_s {
    void *hndlr;
    void *base;
    void *limit;
    cbFrameShareEvent frameshare_event_func;
    void *userdata_frameshareevent_func;
    pthread_mutex_t mutex;
    unsigned char *mmap_base;
    unsigned int mmap_size;
    long long mmap_offset;
} efs_fifo_info_t;

//AllenLiu start
typedef struct ambaipc_clnt_s {
    int host;
    int prog;
    int ver;
    int timeout;
} ambaipc_clnt_t;

ambaipc_clnt_t rt_examframesharertsp_prog = {
    RT_EXAMFRAMESHARERTSP_HOST,
    RT_EXAMFRAMESHARERTSP_PROG_ID,
    RT_EXAMFRAMESHARERTSP_VER,
    RT_EXAMFRAMESHARERTSP_DEFULT_TIMEOUT
};

static efs_fifo_info_t *g_video_fifo = NULL;
static efs_fifo_info_t *g_audio_fifo = NULL;
static efs_fifo_info_t *g_gsensor_fifo = NULL;

#define NETFIFO_GSENSOR_STREAM_ID (0x5a5a)

//program status
static int efs_status = 0;
static unsigned char RTSPInfoInit    = 0;
static unsigned char RTSPFrameInit   = 0;
static unsigned char FrameShareReady = 0;

//AllenLiu start
static CLIENT_ID_t clnt_examframesharertsp = 0;

int RT_ExamFrameShareRtsp_init(void)
{
    printf("%s\n",__FUNCTION__);

    if (clnt_examframesharertsp!=0){
        printf("client already exists.\n");
        return 0;
    }

    clnt_examframesharertsp = ambaipc_clnt_create(rt_examframesharertsp_prog.host, rt_examframesharertsp_prog.prog, rt_examframesharertsp_prog.ver);
    if (clnt_examframesharertsp==0) {
        fprintf(stderr,"%s: Fail to create client.\n",__FUNCTION__);
        return -1;
    }


    return 0;
}

int RT_ExamFrameShareRtsp_release(void)
{
    int rval = 0;

    printf("%s\n",__FUNCTION__);

    if (clnt_examframesharertsp != 0) {
        rval = ambaipc_clnt_destroy(clnt_examframesharertsp);
        if (rval == 0) {
            clnt_examframesharertsp = 0;
        }
    }

    return rval;
}

//AllenLiu end

static int init_RPC(void)
{
    int rval = 0;
    int func_slot;
    AMBA_IPC_PROG_INFO_s prog_info;

    prog_info.ProcNum = LU_EXAMFRAMESHARERTSP_FUNC_AMOUNT - 1;
    prog_info.pProcInfo = malloc(prog_info.ProcNum * sizeof(AMBA_IPC_PROC_s));

    func_slot = LU_EXAMFRAMESHARERTSP_FUNC_CONTROLEVENT - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARERTSP_ControlEvent_Svc;

    func_slot = LU_EXAMFRAMESHARERTSP_FUNC_FRAMEEVENT - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARERTSP_FrameEvent_Svc;

    func_slot = LU_EXAMFRAMESHARERTSP_FUNC_PB_FRAMEEVENT - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARERTSP_PbFrameEvent_Svc;

    func_slot = LU_EXAMFRAMESHARERTSP_FUNC_GSENSOR_FRAMEEVENT - 1;
    prog_info.pProcInfo[func_slot].Mode = AMBA_IPC_SYNCHRONOUS;
    prog_info.pProcInfo[func_slot].Proc = (AMBA_IPC_PROC_f) &LU_EXAMFRAMESHARERTSP_GSensorFrameEvent_Svc;

    rval = ambaipc_svc_register(LU_EXAMFRAMESHARERTSP_PROG_ID,
                    LU_EXAMFRAMESHARERTSP_VER,
                    LU_EXAMFRAMESHARERTSP_NAME,
                    &prog_info, 1);
    if (rval != 0) {
        fprintf(stderr,"%s: Error to do ambaipc_svc_register %d\n ",__FUNCTION__,rval);
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
        rval = ambaipc_svc_unregister(LU_EXAMFRAMESHARERTSP_PROG_ID, LU_EXAMFRAMESHARERTSP_VER);
        if (rval == 0) {
            efs_status = 0;
        } else {
            printf("%s: fail to do ambaipc_svc_unregister()",__FUNCTION__);
        }
    }
}

static int FrameShareRTSPInfo_Init(void)
{
    int Rval = 0;

    if (RTSPInfoInit == 0U) {
        #ifdef AMBA_CAMERA
            Rval = RTSPInfo_Init();
        #else
            Rval = RT_ExamFrameShareRtsp_init();
        #endif

        if (Rval == 0) {
            RTSPInfoInit = 1U;
        }
    }

    return Rval;
}

 int FrameShareRTSPInfo_DeInit(void)
{
    int Rval = 0;

    if (RTSPInfoInit == 1U) {
        #ifdef AMBA_CAMERA
            Rval = RTSPInfo_DeInit();
        #else
            Rval = RT_ExamFrameShareRtsp_release();
        #endif

        if (Rval == 0) {
            RTSPInfoInit = 0U;
        }
    }

    return Rval;
}

static int FrameShareRTSPFrame_Init(void)
{
    int Rval = 0;

    if (RTSPFrameInit == 0U) {
        #ifdef AMBA_CAMERA
            Rval = RTSPFrame_Init();
        #else
            Rval = init_RPC();
        #endif

        if (Rval == 0) {
            RTSPFrameInit = 1U;
        }
    }

    return Rval;
}

int FrameShareRTSPFrame_DeInit(void)
{
    int Rval = 0;

    if (RTSPFrameInit == 1U) {
        #ifdef AMBA_CAMERA
        #else
            release_RPC();
        #endif

        if (Rval == 0) {
            RTSPFrameInit = 0U;
        }
    }

    return Rval;
}

//phy_addr and size need to be 4K alignment. (page size)
static unsigned char *do_mmap(unsigned long long phy_addr, unsigned int size)
{
    unsigned char *map_base;
    int fd;

    printf("%s: phy_addr=0x%llx, size=%u\n",__FUNCTION__,phy_addr,size);

    fd = open("/dev/ppm", O_RDWR|O_SYNC);
    if (fd == -1) {
        perror("Fail to open /dev/ppm");
        return NULL;
    }

    map_base = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, phy_addr);
    if (map_base == MAP_FAILED) {
        perror("Cannot do mmap");
        close(fd);
        return NULL;
    } else {
        printf("Successfull! mmap 0x%llx to 0x%llx, size=%u\n", phy_addr, (unsigned long long)map_base, size);
    }

    close(fd);
    return map_base;
}

static int do_munmap(efs_fifo_info_t *finfo)
{
    if(finfo->mmap_base != NULL) {
            if(munmap(finfo->mmap_base, finfo->mmap_size)!=0){
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
        fprintf(stderr,"%s: Fail to malloc efs_fifo_info_t!\n",__FUNCTION__);
        return NULL;
    }
    memset(tmp_finfo, 0, sizeof(efs_fifo_info_t));
    if(pthread_mutex_init(&tmp_finfo->mutex, NULL)!=0){
        fprintf(stderr,"%s: Fail to create mutex!\n",__FUNCTION__);
    }
    return tmp_finfo;
}



static void delet_finfo(efs_fifo_info_t *finfo)
{
    if(finfo == NULL) {
        return;
    }

    if(finfo->mmap_base != NULL) {
        do_munmap(finfo);
    }
    pthread_mutex_destroy(&finfo->mutex);
    free(finfo);
}

long long AmbaFrameShareRtsp_get_mmap_offset(int frame_type){
    long long offset = 0;

    if((frame_type >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
       (frame_type <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {
        offset = g_video_fifo->mmap_offset;
    } else if ((frame_type >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX) &&
               (frame_type <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX)) {
        offset = g_audio_fifo->mmap_offset;
    }

    return offset;
}

long long AmbaFrameShareRtspGSensor_get_mmap_offset(void){
    return g_gsensor_fifo->mmap_offset;
}

int AmbaFrameShareRtsp_Rpcinit(){
    FrameShareRTSPFrame_Init();
    return 0;
}

int AmbaExamFrameShareRtsp_GetEncInfo(int StreamID, RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s *res)
{
    int status = 0, rval = -1;
    RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s rt_result;

    printf("%s,%d\n",__FUNCTION__,__LINE__);
    memset(res, 0, sizeof(RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s));

    if (RTSPInfoInit == 0U) {
        status = FrameShareRTSPInfo_Init();
    }

    if(status == 0){
        status = ambaipc_clnt_call(clnt_examframesharertsp, RT_EXAMFRAMESHARERTSP_FUNC_GETENCINFO,
                (void *)&StreamID, sizeof(int),
                (void *)&rt_result, sizeof(RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s),
                rt_examframesharertsp_prog.timeout);
        if (status != 0) {
            // fprintf(stderr, "%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
        } else {
            memcpy(res, &rt_result, sizeof(RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s));
            rval = 0;
        }
    }

    printf("%s,%d\n",__FUNCTION__,__LINE__);
    return rval;
}




int AmbaExamFrameShareRtsp_GetMediaStreamIDList(AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s *res)
{
    int status = 0;
    RT_EXAMFRAMESHARERTSP_STREAMITEM_LIST_s rt_result;

    printf("%s,%d\n",__FUNCTION__,__LINE__);
    memset(res, 0, sizeof(AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s));

    if (RTSPInfoInit == 0U) {
        status = FrameShareRTSPInfo_Init();
    }
    if(status == 0){
        status = ambaipc_clnt_call(clnt_examframesharertsp, RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIASTREAMID,
                NULL, 0,
                (void *)&rt_result, sizeof(RT_EXAMFRAMESHARERTSP_STREAMITEM_LIST_s),
                rt_examframesharertsp_prog.timeout);
        if (status != 0) {
            fprintf(stderr, "%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
        } else {
            memcpy(res, &rt_result, sizeof(AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s));
        }
    }

    printf("%s,%d\n",__FUNCTION__,__LINE__);
    return 0;
}

int AmbaExamFrameShareRtsp_GetMediaInfo(int StreamID, AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s *res)
{
    int status = 0;
    RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s rt_result;

    printf("%s,%d\n",__FUNCTION__,__LINE__);
    memset(res, 0, sizeof(AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s));

    if (RTSPInfoInit == 0U) {
        status = FrameShareRTSPInfo_Init();
    }
    if(status == 0){
        status = ambaipc_clnt_call(clnt_examframesharertsp, RT_EXAMFRAMESHARERTSP_FUNC_GETMEDIAINFO,
                (void *)&StreamID, sizeof(int),
                (void *)&rt_result, sizeof(RT_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s),
                rt_examframesharertsp_prog.timeout);

        if (status != 0) {
            fprintf(stderr, "%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
        } else {
            int i,j;
            fprintf(stderr, "%s: mmap_offset=0x%llx\n", __FUNCTION__,g_video_fifo->mmap_offset);
            res->nTrack = rt_result.nTrack;
            for (i=0,j=0; i<rt_result.nTrack; i++,j++) {
                res->Track[j].nTrackType = rt_result.Track[i].nTrackType;
                res->Track[j].nMediaId = rt_result.Track[i].nMediaId;
                res->Track[j].nTimeScale = rt_result.Track[i].nTimeScale;
                res->Track[j].nTimePerFrame = rt_result.Track[i].nTimePerFrame;
                res->Track[j].nInitDelay = rt_result.Track[i].nInitDelay;
                res->Track[j].hCodec = (void *)(rt_result.Track[i].hCodec);

                if((rt_result.Track[i].nTrackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
                   (rt_result.Track[i].nTrackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {

                    RT_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s *Video;
                    Video = (RT_EXAMFRAMESHARERTSP_VIDEO_TRACK_CFG_s *)(&rt_result.Track[j].Info.Video);
                    res->Track[i].Info.Video.nGOPSize = Video->nGOPSize;
                    res->Track[i].Info.Video.nTrickRecDen = Video->nTrickRecDen;
                    res->Track[i].Info.Video.nTrickRecNum = Video->nTrickRecNum;
                    res->Track[i].Info.Video.nCodecTimeScale = Video->nCodecTimeScale;
                    res->Track[i].Info.Video.nWidth = Video->nWidth;
                    res->Track[i].Info.Video.nHeight = Video->nHeight;
                    res->Track[i].Info.Video.nM = Video->nM;
                    res->Track[i].Info.Video.nN = Video->nN;
                    res->Track[i].Info.Video.nIRCycle = Video->nIRCycle;
                    res->Track[i].Info.Video.nRecoveryFrameCnt = Video->nRecoveryFrameCnt;
                    res->Track[i].Info.Video.bDefault = Video->bDefault;
                    res->Track[i].Info.Video.nMode = Video->nMode;
                    res->Track[i].Info.Video.bClosedGOP = Video->bClosedGOP;
                    res->Track[i].pBufferBase = (unsigned char *)(rt_result.Track[i].pBufferBase + g_video_fifo->mmap_offset);
                    res->Track[i].pBufferLimit = (unsigned char *)(rt_result.Track[i].pBufferLimit + g_video_fifo->mmap_offset);

                } else if ((rt_result.Track[i].nTrackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX) &&
                           (rt_result.Track[i].nTrackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX)) {

                    RT_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s *Audio;
                    Audio = (RT_EXAMFRAMESHARERTSP_AUDIO_TRACK_CFG_s *)(&rt_result.Track[j].Info.Audio);
                    res->Track[i].Info.Audio.nSampleRate = Audio->nSampleRate;
                    res->Track[i].Info.Audio.bDefault = Audio->bDefault;
                    res->Track[i].Info.Audio.nChannels = Audio->nChannels;
                    res->Track[i].Info.Audio.nBitsPerSample = Audio->nBitsPerSample;
                    res->Track[i].pBufferBase = (unsigned char *)(rt_result.Track[i].pBufferBase + g_audio_fifo->mmap_offset);
                    res->Track[i].pBufferLimit = (unsigned char *)(rt_result.Track[i].pBufferLimit + g_audio_fifo->mmap_offset);

#if 0
                } else if ((rt_result.Track[i].nTrackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MIN_IDX) &&
                           (rt_result.Track[i].nTrackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MAX_IDX)) {

                    RT_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s *Text;
                    Text = (RT_EXAMFRAMESHARERTSP_TEXT_TRACK_CFG_s *)(&rt_result.Track[j].Info.Text);
                    res->Track[i].Info.Text.bDefault = Text->bDefault;
#endif
                } else {
                    printf("xxxxxxxxxxxx %s: Unknown TrackType 0x%x\n",__FUNCTION__, rt_result.Track[i].nTrackType);
                    res->nTrack--;
                    j--;
                }
            }
        }
    }

    printf("%s,%d\n",__FUNCTION__,__LINE__);
    return 0;
}

int AmbaNetFifo_PlayBack_OP(AMBA_NETFIFO_PLAYBACK_OP_PARAM_s *cmd, AMBA_NETFIFO_PLAYBACK_OP_PARAM_s *res)
{
    RT_NETFIFO_PLAYBACK_OP_PARAM_s in;
    int status = 0;
    RT_NETFIFO_PLAYBACK_OP_PARAM_s rt_result;

    memset(res, 0, sizeof(AMBA_NETFIFO_PLAYBACK_OP_PARAM_s));
    in.OP    = cmd->OP;
    in.pHdlr = cmd->Hdlr;
    memcpy(in.Param, cmd->Param, 128);

    if (RTSPInfoInit == 0U) {
        status = FrameShareRTSPInfo_Init();
    }

    if(status == 0){
        status = ambaipc_clnt_call(clnt_examframesharertsp, RT_EXAMFRAMESHARERTSP_FUNC_PLAYBACK_OP,
                (void *)&in, sizeof(RT_NETFIFO_PLAYBACK_OP_PARAM_s),
                (void *)&rt_result, sizeof(RT_NETFIFO_PLAYBACK_OP_PARAM_s),
                rt_examframesharertsp_prog.timeout);

        if (status != 0) {
            fprintf(stderr, "%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
        } else {
            res->OP = rt_result.OP;
            memcpy(res->Param, rt_result.Param, 128);
        }
    }

    return 0;
}

static int RT_ReportStatus(unsigned int *in, int *rt_result)
{
    int status = 0;
    int rval = 0;

    if (RTSPInfoInit == 0U) {
        status = FrameShareRTSPInfo_Init();
    }

    if(status == 0){
        status = ambaipc_clnt_call(clnt_examframesharertsp, RT_EXAMFRAMESHARERTSP_FUNC_REPORTSTATUS,
                (void *)in, sizeof(unsigned int),
                (void *)rt_result, sizeof(int),
                rt_examframesharertsp_prog.timeout);
        if (status != 0) {
            fprintf(stderr, "%s: fail to do ambaipc_clnt_call(). %d\n", __FUNCTION__, status);
            rval = -1;
        }
    }

    return rval;
}

int AmbaNetFifo_ReportStatus(unsigned int status)
{
    unsigned int event;
    int rt_result;

    event = status;
    if(RT_ReportStatus(&event, &rt_result)<0){
        fprintf(stderr, "%s: fail to do RT_ReportStatus(). %d\n", __FUNCTION__, status);
        rt_result = -1;
    } else {
        fprintf(stderr, "%s: RTOS return: %d\n", __FUNCTION__, status);
    }

    return rt_result;
}

void LU_EXAMFRAMESHARERTSP_ControlEvent_Svc(LU_EXAMFRAMESHARERTSP_CONTROL_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;

    while (FrameShareReady == 0) {
        usleep(100000);
    }

    switch (pArg->Cmd){
    case CMD_STARTENC:
        printf("%s: RTOS Encode Start!!\n",__FUNCTION__);
        AmbaStreamerLive_StartStream();
        ret = 0;
        break;
    case CMD_STOPENC:
        printf("%s: RTOS Encode Stopped!!\n",__FUNCTION__);
        AmbaStreamerLive_StopStream();
        ret = 0;
        break;
    default:
        // printf("%s: unknown cmd (%u)!\n", __FUNCTION__, pArg->Cmd);
        fprintf(stderr, "%s: unknown cmd (%u)!\n", __FUNCTION__, pArg->Cmd);
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

void LU_EXAMFRAMESHARERTSP_FrameEvent_Svc(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;
    //int frame_hndlr = 0;

    while (FrameShareReady == 0) {
        usleep(100000);
    }

    //printf("%s started!__________________________\n", __FUNCTION__);

    //if(((void *)(pArg->hndlr)) == g_video_fifo->hndlr){
        //frame_hndlr = 1;
    //} else if(((void *)(pArg->hndlr)) == g_audio_fifo->hndlr){
        //frame_hndlr = 2;
    //}

    //printf("frame_hndlr = %d\n", frame_hndlr);
    //printf("pArg->InfoPtr.Align = %d\n", pArg->InfoPtr.Align);
    //printf("pArg->InfoPtr.Completed = %d\n", pArg->InfoPtr.Completed);
    //printf("pArg->InfoPtr.Pts = %lld\n", pArg->InfoPtr.Pts);
    //printf("pArg->InfoPtr.SeqNum = %d\n", pArg->InfoPtr.SeqNum);
    //printf("pArg->InfoPtr.Size = %d\n", pArg->InfoPtr.Size);
    //printf("pArg->InfoPtr.StartAddr = %p\n", (void*)(pArg->InfoPtr.StartAddr));
    //printf("pArg->InfoPtr.Type = %d\n", pArg->InfoPtr.Type);

    if(g_video_fifo->frameshare_event_func != NULL) {
        //(*g_video_fifo->frameshare_event_func)(pArg->InfoPtr.TrackType, (void*)&(pArg->InfoPtr));
		(*g_video_fifo->frameshare_event_func)(pArg->InfoPtr.TrackType, (void*)&(pArg->InfoPtr));
    }

    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;

    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;

    //printf("%s ended!______________________________\n", __FUNCTION__);
}

void LU_EXAMFRAMESHARERTSP_PbFrameEvent_Svc(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

void LU_EXAMFRAMESHARERTSP_GSensorFrameEvent_Svc(LU_EXAMFRAMESHARERTSP_FRAMEEVENT_ARG_s *pArg, AMBA_IPC_SVC_RESULT_s *pRet)
{
    int ret = 0;
    int *i_ptr;

    if (((void *)(pArg->hndlr)) == g_gsensor_fifo->hndlr){
        if (g_gsensor_fifo->frameshare_event_func != NULL) {
            (*g_gsensor_fifo->frameshare_event_func)(0, (void*)&(pArg->InfoPtr));
        }
    }

    i_ptr = (int *)(pRet->pResult);
    *i_ptr = ret;

    //send ipc reply
    pRet->Length = sizeof(int);
    pRet->Status = AMBA_IPC_REPLY_SUCCESS;
    pRet->Mode = AMBA_IPC_SYNCHRONOUS;
}

//AllenLiu start
int AmbaFrameShareRtsp_Reg_cbFrameShareEvent(cbFrameShareEvent cb, void *user_data)
{
    g_video_fifo->frameshare_event_func = cb;
    g_video_fifo->userdata_frameshareevent_func = user_data;

    return 0;
}

int AmbaFrameShareRtspGSensor_Reg_cbFrameShareEvent(cbFrameShareEvent cb, void *user_data)
{
    g_gsensor_fifo->frameshare_event_func = cb;
    g_gsensor_fifo->userdata_frameshareevent_func = user_data;

    return 0;
}

int AmbaFrameShareRtsp_mmap( )
{
    efs_fifo_info_t *finfo_v = NULL;
    efs_fifo_info_t *finfo_a = NULL;
    int rval = -1;
    unsigned long long p_addr, b_addr;

    RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s encinfo;

    rval = AmbaExamFrameShareRtsp_GetEncInfo(0, &encinfo);
    if (rval != 0) {
        fprintf(stderr, "%s: get enc info failed\n", __FUNCTION__);
        return -1;
    }

    if (g_video_fifo != NULL && g_audio_fifo != NULL) {
        finfo_v = g_video_fifo;
        finfo_a = g_audio_fifo;
    }

    if ((finfo_v != NULL) && (finfo_a != NULL)) {
        //do mmap video
        finfo_v->hndlr = (void *)(encinfo.v_hndlr);
        p_addr = (unsigned long long)encinfo.buf_base_phy_v;
        b_addr = (unsigned long long)encinfo.buf_base_v;
        finfo_v->mmap_size = encinfo.buf_size_v;

        if ((finfo_v->mmap_size & 0x0fff) != 0){ //must be 4K aligned
            finfo_v->mmap_size = (finfo_v->mmap_size+0x1000) & (~0x0fff);
        }

        if ((p_addr & 0x0fff) != 0){ //must be 4K align
            p_addr = p_addr & (~0x0fff); //aligned to 4k for mmap
            b_addr = b_addr & (~0x0fff); //aligned to 4k
            finfo_v->mmap_size += 0x1000;
            printf("%s: mmap addr is not aligned to 4K (0x%llx to 0x%llx)!!\n",
                   __FUNCTION__,(unsigned long long)encinfo.buf_base_phy_v,p_addr);
        }

        finfo_v->mmap_base = do_mmap(p_addr, finfo_v->mmap_size);
        if(finfo_v->mmap_base != NULL) {
            finfo_v->mmap_offset = (unsigned long long)finfo_v->mmap_base - b_addr;
            finfo_v->base = (void *)(encinfo.buf_base_v + finfo_v->mmap_offset); //convert to LU's Virtual address
            finfo_v->limit = (void *)(encinfo.buf_base_v + encinfo.buf_size_v + finfo_v->mmap_offset); //convert to LU's Virtual address
        } else {
            finfo_v->mmap_size = 0;
            finfo_v->base = NULL;
            finfo_v->limit = NULL;
        }

        //do mmap audio
        finfo_a->hndlr = (void *)(encinfo.a_hndlr);
        p_addr = (unsigned long long)encinfo.buf_base_phy_a;
        b_addr = (unsigned long long)encinfo.buf_base_a;
        finfo_a->mmap_size = encinfo.buf_size_a;

        if ((finfo_a->mmap_size & 0x0fff) != 0){ //must be 4K aligned
            finfo_a->mmap_size = (finfo_a->mmap_size+0x1000) & (~0x0fff);
        }

        if ((p_addr & 0x0fff) != 0){ //must be 4K align
            p_addr = p_addr & (~0x0fff); //aligned to 4k for mmap
            b_addr = b_addr & (~0x0fff); //aligned to 4k
            finfo_a->mmap_size += 0x1000;
            printf("%s: mmap addr is not aligned to 4K (0x%llx to 0x%llx)!!\n",
                   __FUNCTION__,(unsigned long long)encinfo.buf_base_phy_a,p_addr);
        }

        finfo_a->mmap_base = do_mmap(p_addr, finfo_a->mmap_size);
        if(finfo_a->mmap_base != NULL) {
            finfo_a->mmap_offset = (unsigned long long)finfo_a->mmap_base - b_addr;
            finfo_a->base = (void *)(encinfo.buf_base_a + finfo_a->mmap_offset); //convert to LU's Virtual address
            finfo_a->limit = (void *)(encinfo.buf_base_a + encinfo.buf_size_a + finfo_a->mmap_offset); //convert to LU's Virtual address
        } else {
            finfo_a->mmap_size = 0;
            finfo_a->base = NULL;
            finfo_a->limit = NULL;
        }
    }

    return 0;
}

int AmbaFrameShareRtspGSensor_mmap( )
{
    efs_fifo_info_t *finfo_gsensor = NULL;
    int rval = -1;
    unsigned long long p_addr, b_addr;
    RT_EXAMFRAMESHARERTSP_ENCINFO_ARG_s encinfo;

    rval = AmbaExamFrameShareRtsp_GetEncInfo(NETFIFO_GSENSOR_STREAM_ID, &encinfo);
    if (rval != 0) {
        fprintf(stderr, "%s: get enc info failed\n", __FUNCTION__);
        return -1;
    }

    if (g_gsensor_fifo != NULL) {
        finfo_gsensor = g_gsensor_fifo;
    }

    if (finfo_gsensor != NULL) {
        //do mmap gsensor
        finfo_gsensor->hndlr = (void *)(encinfo.gsensor_hndlr);
        p_addr = (unsigned long long)encinfo.buf_base_phy_gsensor;
        b_addr = (unsigned long long)encinfo.buf_base_gsensor;
        finfo_gsensor->mmap_size = encinfo.buf_size_gsensor;

        if ((finfo_gsensor->mmap_size & 0x0fff) != 0){ //must be 4K aligned
            finfo_gsensor->mmap_size = (finfo_gsensor->mmap_size+0x1000) & (~0x0fff);
        }

        if ((p_addr & 0x0fff) != 0){ //must be 4K align
            p_addr = p_addr & (~0x0fff); //aligned to 4k for mmap
            b_addr = b_addr & (~0x0fff); //aligned to 4k
            finfo_gsensor->mmap_size += 0x1000;
            printf("%s: mmap addr is not aligned to 4K (0x%llx to 0x%llx)!!\n",
                   __FUNCTION__,(unsigned long long)encinfo.buf_base_phy_gsensor,p_addr);
        }

        finfo_gsensor->mmap_base = do_mmap(p_addr, finfo_gsensor->mmap_size);
        if(finfo_gsensor->mmap_base != NULL) {
            finfo_gsensor->mmap_offset = (unsigned long long)finfo_gsensor->mmap_base - b_addr;
            finfo_gsensor->base = (void *)(encinfo.buf_base_gsensor + finfo_gsensor->mmap_offset); //convert to LU's Virtual address
            finfo_gsensor->limit = (void *)(encinfo.buf_base_gsensor + encinfo.buf_size_gsensor + finfo_gsensor->mmap_offset); //convert to LU's Virtual address
        } else {
            finfo_gsensor->mmap_size = 0;
            finfo_gsensor->base = NULL;
            finfo_gsensor->limit = NULL;
        }
    }

    return 0;
}

int AmbaFrameShareRtsp_init(int *event_hndlr)
{
    int rval = 0;

    printf("%s,%d\n",__FUNCTION__,__LINE__);

    rval = FrameShareRTSPInfo_Init();
    if(rval < 0) {
        printf("@@ Fail to do FrameShareRTSPInfo_Init()\n");
        return -1;
    }
    g_video_fifo = create_finfo();
    if (g_video_fifo == NULL) {
        fprintf(stderr,"%s: Fail to malloc g_video_fifo!\n",__FUNCTION__);
        return -1;
    }
    g_audio_fifo = create_finfo();
    if (g_audio_fifo == NULL) {
        fprintf(stderr,"%s: Fail to malloc g_video_fifo!\n",__FUNCTION__);
        return -1;
    }
    g_gsensor_fifo = create_finfo();
    if (g_gsensor_fifo == NULL) {
        fprintf(stderr,"%s: Fail to malloc g_gsensor_fifo!\n",__FUNCTION__);
        return -1;
    }
    AmbaFrameShareRtsp_mmap();
    AmbaFrameShareRtspGSensor_mmap();
    *event_hndlr = 1; //non-zero value, so that netfifo would pick LU_SEND_EVENT cb

    return rval;
}

int AmbaFrameShareRtsp_fifoCreate()
{
    g_video_fifo = create_finfo();
    if (g_video_fifo == NULL) {
        fprintf(stderr,"%s: Fail to malloc g_video_fifo!\n",__FUNCTION__);
        return -1;
    }
    g_audio_fifo = create_finfo();
    if (g_audio_fifo == NULL) {
        fprintf(stderr,"%s: Fail to malloc g_video_fifo!\n",__FUNCTION__);
        return -1;
    }
    AmbaFrameShareRtsp_mmap();

    return 0;
}

void AmbaFrameShareRtsp_fifoStop()
{
    delet_finfo(g_video_fifo);
    g_video_fifo = NULL;

    delet_finfo(g_audio_fifo);
    g_audio_fifo = NULL;
}

void AmbaFrameShareRtsp_release( )
{
    release_RPC();
    RT_ExamFrameShareRtsp_release();

    delet_finfo(g_video_fifo);
    g_video_fifo = NULL;

    delet_finfo(g_audio_fifo);
    g_audio_fifo = NULL;

    delet_finfo(g_gsensor_fifo);
    g_gsensor_fifo = NULL;
}

void AmbaFrameShareRtsp_Ready(void)
{
    FrameShareReady = 1U;
}


