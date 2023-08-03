#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include "amba_frameshare.h"
#include "ambastream.h"

typedef unsigned long		uintptr_t;
typedef struct AmbaStream_VideoParam_s {
    unsigned char vps[32];  /**< H265 VPS */
    unsigned int vps_len;   /**< H265 VPS size */
    unsigned char sps[64];  /**< H264 SPS */
    unsigned int sps_len;   /**< H264 SPS size */
    unsigned char pps[32];  /**< H264 PPS */
    unsigned int pps_len;   /**< H264 PPS size */
    unsigned int profileLevelid;
    unsigned int updated;   /**< updated? */
    unsigned int TimeScale;
    unsigned int TimePerFrame;
    unsigned int InitDelay;
    unsigned short gopM;     /**< GOP M */
    unsigned short gopN;     /**< GOP N */
    unsigned short gopSize;  /**< GOP Size */
    unsigned short IRCycle;  /**< Intra refresh cycle, only valid when N=255 */
    unsigned short recoveryFrameCnt; /**< recovery_frame_cnt value of SEI recovery point */
    double vector_90k;
} AmbaStream_VideoParam_t;

typedef struct AmbaStream_SPS_PPS_s {
    unsigned char sps[64];  /**< H264 SPS */
    unsigned int sps_len;   /**< H264 SPS size */
    unsigned char pps[32];  /**< H264 PPS */
    unsigned int pps_len;   /**< H264 PPS size */
} AmbaStream_SPS_PPS_t;

typedef struct AmbaStream_VPS_SPS_PPS_s {
    unsigned char vps[32];  /**< H265 VPS */
    unsigned int vps_len;   /**< H265 VPS size */
    unsigned char sps[60];  /**< H265 SPS */
    unsigned int sps_len;   /**< H265 SPS size */
    unsigned char pps[24];  /**< H265 PPS */
    unsigned int pps_len;   /**< H265 PPS size */
} AmbaStream_VPS_SPS_PPS_t;

typedef struct AmbaStream_AudioParam_s {
    unsigned int TimeScale;
    unsigned int TimePerFrame;
    int SampleRate;
    int channels;
    double vector_sr;       //vector to sample rate
    unsigned int updated;   /**< updated? */
} AmbaStream_AudioParam_t;

typedef struct AmbaStream_TextParam_s {
    int TimeScale;
    int TimePerFrame;
    double vector_1k;       //vector to 1kHZ
} AmbaStream_TextParam_t;

typedef struct AmbaStream_MediaHndlr_s {
    unsigned int handled_type;  //AllenLiu
    unsigned long long buf_base;
    unsigned long long buf_limit;
    unsigned int mediaId;
    void* hCodec;
    double vector;
    u8 enabled;
    u8 padding;
    u8 padding_1;
    u8 padding_2;

    frame_ready_cb cb_fr;
    void *ctx_cb_fr;
    buf_ready_cb cb_br;
    void *ctx_cb_br;
} AmbaStream_MediaHndlr_t;

typedef struct AmbaStream_Streamer_s {
    pthread_mutex_t mtx;
    AmbaStream_MediaHndlr_t media;
    int isLive;
    int trackId;
    int trackType;
    unsigned int strmId;
    union {
        AmbaStream_VideoParam_t video;
        AmbaStream_AudioParam_t audio;
        AmbaStream_TextParam_t  text;
    } param;
} AmbaStream_Streamer_t;

typedef struct streamerGroup_s {
    pthread_mutex_t mtx;            /**< Mutex to protect this structure */
    char* name;                     /**< url of this streamer group */
    unsigned int StrmId;
    AmbaStream_Streamer_t* stms[STREAMER_MGR_MAX_STREAM_PER_GROUP];  /**< streamers(array) of this group */
} streamerGroup_t;

typedef struct AmbaStream_GlobalParam_s {
    pthread_t tid_event;
    int EventHndlr;
    // Live stream group
    streamerGroup_t* liveGroup;  /**< array of liveGroup */
    int maxLiveGroup;
    int maxStreamerPerLiveGroup;
    int liveState;                /**< indicate the state of liveGroup */

    // Streamer pool
    AmbaStream_Streamer_t* streamerList; /**< array of streamer*/
    int maxStreamer;
} AmbaStream_GlobalParam_t;

static AmbaStream_GlobalParam_t *g_ambastream = NULL;
static AmbaStream_Streamer_t g_gsensor_streamer;

static inline int is_VideoFrame(unsigned char desc_type)
{
    int rval = 0;

    switch(desc_type) {
        case AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME:
        case AMBA_EXAMFRAMESHARERTSP_TYPE_I_FRAME:
        case AMBA_EXAMFRAMESHARERTSP_TYPE_P_FRAME:
        case AMBA_EXAMFRAMESHARERTSP_TYPE_B_FRAME:
        case AMBA_EXAMFRAMESHARERTSP_TYPE_EOS:
            rval = 1;
            break;
        default:
            rval = 0;
            break;
    }

    return rval;
}

static inline int is_AudioFrame(unsigned char desc_type)
{
    int rval = 0;

    switch(desc_type) {
        case AMBA_EXAMFRAMESHARERTSP_TYPE_AUDIO_FRAME:
        case AMBA_EXAMFRAMESHARERTSP_TYPE_EOS:
            rval = 1;
            break;
        default:
            rval = 0;
            break;
    }

    return rval;
}

static inline int is_TextFrame(unsigned char desc_type)
{
    int rval = 0;

    switch(desc_type) {
        case AMBA_EXAMFRAMESHARERTSP_TYPE_UNDEFINED:
        case AMBA_EXAMFRAMESHARERTSP_TYPE_EOS:
            rval = 1;
            break;
        default:
            rval = 0;
            break;
    }

    return rval;
}

static inline int is_ValidFrame(int trackType, unsigned char desc_type)
{
    int rval = 0;
    if((trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
       (trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {
        rval = is_VideoFrame(desc_type);
    } else if ((trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX) &&
               (trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX)) {
        rval = is_AudioFrame(desc_type);

#if 0
    } else if ((trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MIN_IDX) &&
               (trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_TEXT_MAX_IDX)) {
        rval = is_TextFrame(desc_type);
#endif
    } else {
        rval = 0;
    }

    return rval;
}

static void dump_data(char *data, int length)
{
    int i;

    printf("data length,%d :\n",length);
    for (i=0; i<length; i++) {
        if((i&0x0f)==0x0f){
            printf("%02x\n",data[i]);
        } else {
            printf("%02x ",data[i]);
        }
    }
    printf("\n\n");
}

/*
 * Get video SPS/PPS
 */
#define MAX_SPS_PPS_LEN 128
static void Parse_SPS_PPS(char *inbuf, int insize, char *base, char *limit, AmbaStream_VideoParam_t *p_sps_pps_info)
{
    unsigned char *ptr;
    short pad1, pad2;
    int i, found, pos, sps_end, len, got, size;
    unsigned int code;
    char working_buf[MAX_SPS_PPS_LEN];

    if ((p_sps_pps_info->pps_len != 0) && (p_sps_pps_info->updated != 0)) {
        /* already got SPS & PPS */
        return;
    }

  // check if our sps/pps cross ring-buffer boundary
    if (inbuf + MAX_SPS_PPS_LEN > limit) {
        unsigned int offset = limit - inbuf;
        memcpy(working_buf, inbuf, limit - inbuf);
        memcpy(working_buf + offset, base, MAX_SPS_PPS_LEN - offset);
        ptr = (unsigned char *)working_buf;
    } else {
        ptr = (unsigned char *)inbuf;
    }

    size = insize;
    if ((ptr[4]&0x1F) == 0x09) {
        /* Skip AU Delimiter at the begining */
        ptr = (unsigned char *)(inbuf+6);
        size = insize - 6;
    }

    if ((ptr[4]&0x1F) != 0x07) {
        /* not SPS */
        fprintf(stderr,"%s: cannot find SPS\n",__FUNCTION__);
        dump_data(inbuf, 64);
        return;
    }

    /* find end of SPS */
    code = 0;
    for (pos = 4, found=0; (!found) && (pos<size); pos++) {
        if (code == 0x00000001) {
            found = 1;
            break;
        } else {
            code = (code<<8) | ptr[pos];
        }
    }

    if(!found){
        fprintf(stderr,"CANNOT find sps\n");
        dump_data(inbuf, 128);
        return;
    }
    sps_end = pos - 5;
    len = sps_end - 4 + 1;
    pad1 = len & 3;
    if(pad1 > 0) {
        pad1 = 4 - pad1;
    }

    len += 4;//include 0x00000001
    p_sps_pps_info->sps_len = len + pad1;

    //for (i=0, got=4; i<len; i++, got++) { //got=4: skip 0x00000001
    for (i=0, got=0; i<len; i++, got++) { //got=0: include 0x00000001
        p_sps_pps_info->sps[i] = ptr[got];
    }
    for(i = len;i < len + pad1; i++) {
        p_sps_pps_info->sps[i] = 0;
    }


    /* fill profileLevelID (27 42 80 1e -> 0x42801e)*/
    //p_sps_pps_info->profileLevelid=(p_sps_pps_info->sps[1]<<16)|
    //  (p_sps_pps_info->sps[2]<<8)|(p_sps_pps_info->sps[3]);

    p_sps_pps_info->profileLevelid=(p_sps_pps_info->sps[5]<<16)|
        (p_sps_pps_info->sps[6]<<8)|(p_sps_pps_info->sps[7]); //include 0x00000001


    /* find End of PPS */
    //previous stop should be pps_start
    if ((ptr[pos]&0x1F) != 0x08) {
        /* not PPS */
        fprintf(stderr,"%s: cannot find PPS(%02x)\n",__FUNCTION__,ptr[pos]);
        return;
    }

    code = 0;
    for (found=0; (!found) && (pos<size); pos++) {
        if (code == 0x00000001) {
            found = 1;
            break;
        } else {
            code = (code<<8) | ptr[pos];
        }
    }

    if(!found){
        fprintf(stderr,"CANNOT find pps_end\n");
        dump_data(inbuf, 256);
        return;
    }

    len = pos - sps_end - 9; //(pos - 5) - (sps_end + 5) + 1;
    pad2 = len & 3;
    if(pad2 > 0) {
        pad2 = 4 - pad2;
    }

    len += 4; //include 0x00000001
    p_sps_pps_info->pps_len = len + pad2;

    //for (i=0, got=sps_pos+4; i<len; i++, got++) { //got=sps_pos+4: skip 0x00000001
    for (i=0, got=sps_end+1; i<len; i++, got++) { //got=sps_pos: include 0x00000001
        p_sps_pps_info->pps[i] = ptr[got];
    }
    for(i=len; i < len + pad2; i++) {
        p_sps_pps_info->pps[i] = 0;
    }

    p_sps_pps_info->updated = 1;

#if 0 //Debug
    printf("sps(%d):\n",p_sps_pps_info->sps_len);
    dump_data((char *)p_sps_pps_info->sps,p_sps_pps_info->sps_len);
    printf("pps(%d):\n",p_sps_pps_info->pps_len);
    dump_data((char *)p_sps_pps_info->pps,p_sps_pps_info->pps_len);
    printf("profileLevelid: 0x%08x\n",p_sps_pps_info->profileLevelid);
#endif
}

static void Parse_VPS_SPS_PPS(char *inbuf, int insize, char *base, char *limit, AmbaStream_VideoParam_t *p_sps_pps_info)
{
#define H265_NAL_VPS    32
#define H265_NAL_SPS    33
#define H265_NAL_PPS    34

    int             i, remain_len = insize;
    unsigned char   *pCur = (unsigned char*)inbuf;
    unsigned int    start_code = (unsigned int) - 1;
    unsigned int    nal_type = 0;
    unsigned int    unit_size = 0;

    unsigned int    prev_nal_type = 0;
    unsigned char   *pPrev_nal_addr = 0;

    while( remain_len ) {
        start_code = (start_code << 8) | *pCur;
        pCur++;
        remain_len--;

        if( start_code == 0x00000001 ||
                (start_code & 0x00FFFFFF) == 0x000001 ) {
            nal_type    = (*pCur >> 1) & 0x3f;

            if( pPrev_nal_addr ) {
                int     offset = (start_code == 0x00000001) ? 4 : 3;

                pCur -= offset;
                remain_len += offset;

                unit_size = (pCur - pPrev_nal_addr);

                switch(prev_nal_type) {
                case H265_NAL_VPS:
                    if( unit_size > 32 ) {
                        fprintf(stderr, "%s[#%d] VPS out buf: %d/32 bytes\n", __func__, __LINE__, unit_size);
                        break;
                    }

                    p_sps_pps_info->vps_len = unit_size;
                    for(i = 0; i < unit_size; ++i)
                        p_sps_pps_info->vps[i] = pPrev_nal_addr[i];

                    // memcpy(p_sps_pps_info->vps, pPrev_nal_addr, unit_size);
                    break;
                case H265_NAL_SPS:
                    if( unit_size > 64 ) {
                        fprintf(stderr, "%s[#%d] VPS out buf: %d/64 bytes\n", __func__, __LINE__, unit_size);
                        break;
                    }

                    p_sps_pps_info->sps_len = unit_size;
                    for(i = 0; i < unit_size; ++i)
                        p_sps_pps_info->sps[i] = pPrev_nal_addr[i];

                    // memcpy(p_sps_pps_info->sps, pPrev_nal_addr, unit_size);
                    break;
                case H265_NAL_PPS:
                    if( unit_size > 32 ) {
                        fprintf(stderr, "%s[#%d] VPS out buf: %d/32 bytes\n", __func__, __LINE__, unit_size);
                        break;
                    }

                    p_sps_pps_info->pps_len = unit_size;
                    for(i = 0; i < unit_size; ++i)
                        p_sps_pps_info->pps[i] = pPrev_nal_addr[i];

                    // memcpy(p_sps_pps_info->pps, pPrev_nal_addr, unit_size);
                    break;
                default:
                    break;
                }

                p_sps_pps_info->updated = (p_sps_pps_info->vps_len &&
                                           p_sps_pps_info->sps_len &&
                                           p_sps_pps_info->pps_len);
                if( p_sps_pps_info->updated )
                    break;

                pPrev_nal_addr = 0;
                continue;
            }

            prev_nal_type = nal_type;
            //printf("get NAL(%d)\n", nal_type);
            switch( nal_type ) {
            case H265_NAL_VPS:
            case H265_NAL_SPS:
            case H265_NAL_PPS:
                pPrev_nal_addr = pCur;
                break;
            default:
                pPrev_nal_addr = 0;
                break;
            }
        }
    }

    return;
}


AmbaStream_Streamer_t* AmbaStreamer_LockStreamerByHandler(int frame_type, unsigned int StrmId)
{
    int i, index = -1;
    AmbaStream_Streamer_t* stm;

    // find streamer who take care of this hndlr

    for (i = 0; i < g_ambastream->maxStreamer; i++) {
        stm = &g_ambastream->streamerList[i];
        if (pthread_mutex_lock(&stm->mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        if ((stm->trackType==frame_type) && (stm->strmId==StrmId)) {
            index = i;
            break;
        } else {
            pthread_mutex_unlock(&stm->mtx);
        }
    }
    if (index == -1) {
        return NULL;
    }

    return stm;

}

static void AmbaStreamer_UnlockStreamer(AmbaStream_Streamer_t* stm)
{
    pthread_mutex_unlock(&stm->mtx);
}

static AmbaStream_Streamer_t* AmbaStreamer_GetSteamerSlot(void)
{
    unsigned int i;
    AmbaStream_Streamer_t* stm;
    for (i=0; i < g_ambastream->maxStreamer ; i++) {
        stm = &g_ambastream->streamerList[i];
        if (pthread_mutex_lock(&stm->mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            return NULL;
        }
        if (g_ambastream->streamerList[i].media.handled_type == 0) {
            pthread_mutex_unlock(&stm->mtx);
            return stm;
        }
        pthread_mutex_unlock(&stm->mtx);
    }
    return NULL;
}

static int AmbaStreamerLive_EOS(void* hdlr, void*info)
{
    unsigned int i, j;
    AmbaStream_Streamer_t* stm;
    unsigned int liveStreamerRemain = 0;

    // find streamer who have fifo matched
    for (i = 0; i < g_ambastream->maxStreamer; i++) {
        stm = &g_ambastream->streamerList[i];
        if (pthread_mutex_lock(&stm->mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        if (stm->isLive == 1) {
            stm->media.hCodec = 0;
        }
        pthread_mutex_unlock(&stm->mtx);
    }

    // check if there is any liveGroup need to be deleted
    for (i = 0; i < g_ambastream->maxLiveGroup; i++) {
        if (pthread_mutex_lock(&g_ambastream->liveGroup[i].mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        if (g_ambastream->liveGroup[i].name != NULL) {
            liveStreamerRemain = 0;
            for (j = 0; j < g_ambastream->maxStreamerPerLiveGroup; j++) {
                stm = g_ambastream->liveGroup[i].stms[j];
                if (stm == NULL){
                    continue;
                }
                if (pthread_mutex_lock(&stm->mtx) != 0) {
                    fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
                    continue;
                }
                if (stm->media.hCodec == 0) {
                    g_ambastream->liveGroup[i].stms[j] = NULL;
                } else {
                    liveStreamerRemain++;
                }
                pthread_mutex_unlock(&stm->mtx);
            }
            if (liveStreamerRemain == 0) { // all streamer in this group are deleted
                free(g_ambastream->liveGroup[i].name);
                g_ambastream->liveGroup[i].name = NULL;
            }
        }
        pthread_mutex_unlock(&g_ambastream->liveGroup[i].mtx);
    }

    return 0;
}

static int AmbaStreamer_CopyTrackInfo(AmbaStream_Streamer_t* stm, AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_CFG_s* trk, unsigned int trackID)
{
    if((trk->nTrackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
       (trk->nTrackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {

        stm->param.video.TimeScale = (unsigned int)(trk->nTimeScale);
        stm->param.video.TimePerFrame = (unsigned int)(trk->nTimePerFrame);
        stm->param.video.InitDelay = (unsigned int)(trk->nInitDelay);
        stm->param.video.gopM = trk->Info.Video.nM;
        stm->param.video.gopN = trk->Info.Video.nN;
        stm->param.video.gopSize = trk->Info.Video.nGOPSize;
        stm->param.video.updated = 0;//need Parse_SPS_PPS
        stm->media.vector = (double)90000/(double)stm->param.video.TimeScale;
        printf("[StreamerMgr] video stream: codec=0x%0x, TimeScale=%d, TimePerFrame=%d, GOP=%d,%d,%d, vt %lf",
                trk->nMediaId, trk->nTimeScale,
                trk->nTimePerFrame,
                trk->Info.Video.nM, trk->Info.Video.nN,
                trk->Info.Video.nGOPSize, stm->media.vector);
        if(trk->Info.Video.nN == 0xff) {
            stm->param.video.IRCycle = trk->Info.Video.nIRCycle;
            stm->param.video.recoveryFrameCnt = trk->Info.Video.nRecoveryFrameCnt;
            printf(", IRcycle=%d, RecoveryFrameCnt=%d\n",
                    trk->Info.Video.nIRCycle,
                    trk->Info.Video.nRecoveryFrameCnt);
        } else {
            printf("\n");
        }

    } else if ((trk->nTrackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MIN_IDX) &&
               (trk->nTrackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO_MAX_IDX)) {

        stm->param.audio.TimeScale = (unsigned int)(trk->nTimeScale) > 0 ?
            (unsigned int)(trk->nTimeScale) : 90000;
        stm->param.audio.TimePerFrame = (unsigned int)(trk->nTimePerFrame);
        stm->param.audio.SampleRate = trk->Info.Audio.nSampleRate;
        stm->param.audio.channels = trk->Info.Audio.nChannels;
        stm->param.audio.updated = 1;
        stm->media.vector = (double)stm->param.audio.SampleRate/(double)stm->param.audio.TimeScale;
        printf("[StreamerMgr] audio stream: codec=0x%0x, samplerate=%d, channels=%d, TimeScale=%d, TimePerFrame=%d, vt %lf\n",
                trk->nMediaId, trk->Info.Audio.nSampleRate,
                trk->Info.Audio.nChannels,  stm->param.audio.TimeScale,
                stm->param.audio.TimePerFrame, stm->media.vector);
    }
  #if 0
    else if (trk->nTrackType == AMBA_NETFIFO_MEDIA_TRACK_TYPE_TEXT) {
        stm->param.text.TimeScale = (unsigned int)(trk->nTimeScale);
        stm->param.text.TimePerFrame = (unsigned int)(trk->nTimePerFrame);
        stm->media.vector = (double)1000/(double)stm->param.text.TimeScale;
        printf("[StreamerMgr] text stream: codec=0x%0x, TimeScale=%d, TimePerFrame=%d\n",
                trk->nMediaId, trk->nTimeScale, trk->nTimePerFrame);
    }
 #endif
    else {
        fprintf(stderr, "%s: unrecognized track type, omit it\n", __FUNCTION__);
    }
    stm->trackType = trk->nTrackType;
    stm->trackId = trackID;
    stm->media.mediaId = trk->nMediaId;
    stm->media.hCodec = trk->hCodec;
    stm->media.buf_base = (unsigned long long)(trk->pBufferBase);
    stm->media.buf_limit = (unsigned long long)(trk->pBufferLimit);
    stm->media.handled_type = trk->nTrackType;

    fprintf(stderr, "%s: trackType %d, trackId %d, media.mediaId %d, media.hCodec %p, handled_type %d, buf_base 0x%llx, buf_limit 0x%llx\n", __FUNCTION__, \
    stm->trackType, stm->trackId, stm->media.mediaId, stm->media.hCodec, stm->media.handled_type, stm->media.buf_base, stm->media.buf_limit);
    return 0;
}

//char Abuff[1024*1024];
//int Asize = 0;
static int AmbaStreamerLive_ProcessFrame(int type, AMBA_EXAMFRAMESHARERTSP_BITS_DESC_s* info)
{
    AmbaStream_Streamer_t* stm;
    AMBA_EXAMFRAMESHARERTSP_BITS_DESC_s desc;
    int frame_type = 0;
    long long mmap_offset = 0;

    frame_type = type;

    stm = AmbaStreamer_LockStreamerByHandler(frame_type, info->StrmId);
    if (stm == NULL) {
        fprintf(stderr,"%s: stm = NULL, into->type = %d, info->StrmId %d, frame_type=%d\n",__FUNCTION__, info->Type, info->StrmId, frame_type);
        return -1;
    }

    mmap_offset = AmbaFrameShareRtsp_get_mmap_offset(frame_type);
    desc.SeqNum = info->SeqNum;
    desc.Pts = info->Pts;
    desc.Type = info->Type;
    desc.Completed = info->Completed;
    desc.Align = info->Align;
    desc.StartAddr = info->StartAddr + mmap_offset;
    desc.Size = info->Size;

    // check valid frame type
    if(!is_ValidFrame(stm->trackType, desc.Type)){
        fprintf(stderr,"%s: Unsupported frame type %d, tracktype=%d\n",__FUNCTION__,desc.Type, stm->trackType);
        AmbaStreamer_UnlockStreamer(stm);
        return -1;
    }

    // preprocess
    if ((stm->trackType >= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MIN_IDX) &&
        (stm->trackType <= AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_VIDEO_MAX_IDX)) {
        if (stm->isLive == 1 && stm->param.video.updated == 0) {
            if (desc.Type == AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME && desc.Size != AMBA_EXAMFRAMESHARERTSP_MARK_EOS ) {
                switch(stm->media.mediaId)
                {
                    case AMP_FORMAT_MID_H264:
                    case AMP_FORMAT_MID_AVC:
                        Parse_SPS_PPS((char*)desc.StartAddr, desc.Size, (char*)stm->media.buf_base,
                            (char*)stm->media.buf_limit, &stm->param.video);
                        break;
                    case AMP_FORMAT_MID_H265:
                    case AMP_FORMAT_MID_HVC:
                        Parse_VPS_SPS_PPS((char*)desc.StartAddr, desc.Size, (char*)stm->media.buf_base,
                            (char*)stm->media.buf_limit, &stm->param.video);
                        break;
                    default:
                        fprintf(stderr, "Not support media id (x%x)\n", stm->media.mediaId);
                        break;
                }
            }
        }
    }

    // convert to codec clock domain
    desc.Pts = (stm->media.vector != 0)? (stm->media.vector * desc.Pts): desc.Pts;

    // call callback
    if((stm->media.cb_fr != NULL) && (stm->media.enabled == 1)) {
        AmbaStream_frameinfo_t f_info;
        f_info.frame_num = desc.SeqNum;
        f_info.pts = desc.Pts;
        f_info.pic_type = desc.Type;
        f_info.mark = desc.Completed;
        if(desc.Type == AMBA_EXAMFRAMESHARERTSP_TYPE_EOS) {
            printf("%s: prepare EOS frame!\n",__FUNCTION__);
            f_info.size = 0; //frame_reader use dec_size==0 to detect EOS.
            f_info.start_addr = 0;
        } else {
            f_info.size = desc.Size;
            f_info.start_addr = desc.StartAddr;
        }
        f_info.base_addr = stm->media.buf_base;
        f_info.limit_addr = stm->media.buf_limit;

        (*stm->media.cb_fr)(&f_info, stm->media.ctx_cb_fr);

    }

/*if ((stm->trackType == AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO)) {
    if( (desc.Type != AMBA_EXAMFRAMESHARERTSP_TYPE_EOS)) {
        printf("sa %p, sz %d, [0]0x%x, [10]0x%x, [30]0x%x\n",\
        (char*)desc.StartAddr, desc.Size, *((char*)desc.StartAddr), *((char*)(desc.StartAddr+10)), *((char*)(desc.StartAddr+30)));
    }
    }*/

/*{
        int bytes_cnt = 0, left;
        if ((stm->trackType == AMBA_EXAMFRAMESHARERTSP_MEDIA_TRACK_TYPE_AUDIO)) {
            if( (desc.Type != AMBA_EXAMFRAMESHARERTSP_TYPE_EOS)) {
                if(Asize+desc.Size<=1024*1024) {
                    left = stm->media.buf_limit - desc.StartAddr;
                    if(desc.Size<=left) {
                        memcpy(Abuff, (char*)desc.StartAddr, desc.Size);
                        Asize = Asize+desc.Size;
                    } else {
                        memcpy(Abuff, (char*)desc.StartAddr, left);
                        Asize = Asize+ left;

                        memcpy(Abuff, (char*)stm->media.buf_base, desc.Size-left);
                        Asize = Asize+ desc.Size-left;
                    }
                } else {
                    printf("shouldn't\n");
                    if (NULL != foutput) {
                        bytes_cnt = fwrite((char*)Abuff, 1, Asize, foutput);
                        if (bytes_cnt != Asize) {
                            printf("fail to write ambalink audio aac\n");
                        }
                        Asize = 0;
                        memset(Abuff, 0, sizeof(Abuff));

                        left = stm->media.buf_limit - desc.StartAddr;
                        if(desc.Size<=left) {
                            memcpy(Abuff, (char*)desc.StartAddr, desc.Size);
                            Asize = Asize+desc.Size;
                        } else {
                            memcpy(Abuff, (char*)desc.StartAddr, left);
                            Asize = Asize+ left;

                            memcpy(Abuff, (char*)stm->media.buf_base, desc.Size-left);
                            Asize = Asize+ desc.Size-left;
                        }
                    } else {
                        printf("audio aac file isn't opened\n");
                    }
                }
            }else {
                if (NULL != foutput) {
                    if (NULL != foutput) {
                        bytes_cnt = fwrite((char*)Abuff, 1, Asize, foutput);
                        if (bytes_cnt != Asize) {
                            printf("fail to write ambalink audio aac\n");
                        }
                        printf("Asize %d\n", Asize);
                        Asize = 0;
                        memset(Abuff, 0, sizeof(Abuff));
                    } else {
                        printf("audio aac file isn't opened\n");
                    }

                    fclose(foutput);
                    printf("scceed to close audio aac file\n");
                }
            }
    }
}*/

    if(desc.Size == AMBA_EXAMFRAMESHARERTSP_MARK_EOS){ //It is EOS
        fprintf(stderr,"%s: frameshare size is EOS mark!(%x)\n",__FUNCTION__, desc.Size);
        AmbaStreamerLive_EOS(NULL, info);// fifo and fifo_event handling
    }
    AmbaStreamer_UnlockStreamer(stm);
    return 0;
}

void* AmbaStreamer_GetGSensorStreamer(void)
{
    return (void *)&g_gsensor_streamer;
}

static int AmbaStreamerGSensor_ProcessFrame(int type, AMBA_EXAMFRAMESHARERTSP_BITS_DESC_s* info)
{
    if ((g_gsensor_streamer.media.cb_fr != NULL) && (g_gsensor_streamer.media.enabled == 1)) {
        long long mmap_offset = 0;
        AMBA_EXAMFRAMESHARERTSP_BITS_DESC_s desc;

        mmap_offset = AmbaFrameShareRtspGSensor_get_mmap_offset();
        desc.SeqNum = info->SeqNum;
        desc.Type = info->Type;
        desc.StartAddr = info->StartAddr + mmap_offset;
        desc.Size = info->Size;

        AmbaStream_frameinfo_t f_info;
        f_info.frame_num = desc.SeqNum;
        f_info.pic_type = desc.Type;
        if (desc.Type == AMBA_EXAMFRAMESHARERTSP_TYPE_EOS) {
           printf("%s: prepare EOS frame!\n",__FUNCTION__);
           f_info.size = 0; //frame_reader use dec_size==0 to detect EOS.
           f_info.start_addr = 0;
        } else {
           f_info.size = desc.Size;
           f_info.start_addr = desc.StartAddr;
        }
        g_gsensor_streamer.media.cb_fr(&f_info, g_gsensor_streamer.media.ctx_cb_fr);
    }

    return 0;
}

// Retrieve stream list from AppLib and fill active one into LiveGroup
// Create fifo and fill media info into these streamers
static int AmbaStreamerLive_StartEncode(void)
{
    unsigned int i, groupIndex, streamIndex;
    int status = -1, rval = -1;
    AmbaStream_Streamer_t* stm;
    AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s sList;
    AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s mInfo;

    if (g_ambastream == NULL) {
        return -1;
    }

    if (g_ambastream->liveState == AMBA_EXAMFRAMESHARERTSP_STATUS_START) {
        fprintf(stderr, "[StreamerMgr] live streams are already inited, ignore START_ENCODE request\n");
        return -1;
    }

    // get streamList
    rval = AmbaExamFrameShareRtsp_GetMediaStreamIDList(&sList);

    if (rval != 0 || sList.Amount < 1) {
        fprintf(stderr, "%s: no stream in streamList\n", __FUNCTION__);
        return -1;
    }


    for (streamIndex = 0, groupIndex = 0; streamIndex < sList.Amount; streamIndex++) {
        // check stream active flag
        if (sList.StreamItemList[streamIndex].Active == 0) {
            continue;
        }
        // check liveGroup space
        if (groupIndex >= g_ambastream->maxLiveGroup) {
            fprintf(stderr,"[StreamerMgr] No enough space for all active live stream\n");
            break;
        }
        // get media info of stream
        rval = AmbaExamFrameShareRtsp_GetMediaInfo(sList.StreamItemList[streamIndex].StrmId, &mInfo);
        if (rval != 0 || mInfo.nTrack == 0) {
            fprintf(stderr,"%s: no track in mediaInfo", __FUNCTION__);
            continue;
        } else if (mInfo.nTrack > g_ambastream->maxStreamerPerLiveGroup) {
            mInfo.nTrack = g_ambastream->maxStreamerPerLiveGroup;
            fprintf(stderr,"[StreamerMgr] No enough space to create all tracks in stream(%u)\n", streamIndex);
        }
        // lock this liveGroup
        if (pthread_mutex_lock(&g_ambastream->liveGroup[groupIndex].mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        // create vfifo of each track
        for (i=0; i < mInfo.nTrack; i++) {
            stm = AmbaStreamer_GetSteamerSlot();
            if (stm == NULL) {
                fprintf(stderr,"%s: no available streamer instance", __FUNCTION__);
                status = -1;
                break;
            }
            if (pthread_mutex_lock(&stm->mtx) != 0) {
                fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
                status = -1;
                break;
            }
            fprintf(stderr,"%s: i=%d", __FUNCTION__,i);
            AmbaStreamer_CopyTrackInfo(stm, &mInfo.Track[i], i);
            stm->isLive = 1;
            stm->strmId = sList.StreamItemList[streamIndex].StrmId;

            fprintf(stderr,"%s:[stm 0x%p] trktyp %d, strid %d\n", __FUNCTION__, stm, stm->trackType, stm->strmId);
            g_ambastream->liveGroup[groupIndex].stms[i] = stm; //bind streamer into LiveGroup
            status = 0;
            pthread_mutex_unlock(&stm->mtx);

        }
        if (status >=0) {
            int length = strlen(sList.StreamItemList[streamIndex].Name) + 1;
            g_ambastream->liveGroup[groupIndex].name = (char*) malloc(sizeof(char)*length);
            strncpy(g_ambastream->liveGroup[groupIndex].name, sList.StreamItemList[streamIndex].Name, length);
            g_ambastream->liveGroup[groupIndex].StrmId = sList.StreamItemList[streamIndex].StrmId;
            fprintf(stderr,"[StreamerMgr] Start stream(%u)(%s) StrmId# %d success\n", streamIndex, g_ambastream->liveGroup[groupIndex].name, \
                    g_ambastream->liveGroup[groupIndex].StrmId);
            pthread_mutex_unlock(&g_ambastream->liveGroup[groupIndex].mtx);
            groupIndex++;
        } else {
            pthread_mutex_unlock(&g_ambastream->liveGroup[groupIndex].mtx);
            break;
        }
    }

    for (i = 0; i < g_ambastream->maxStreamer; i++) {
        stm = &g_ambastream->streamerList[i];
        //fprintf(stderr,"%s:[stm 0x%p] trktyp %d, strid %d\n", __FUNCTION__, stm, stm->trackType, stm->strmId);
    }

    if (status >= 0) {
        fprintf(stderr,"[StreamerMgr] Start all live streams success\n");
        g_ambastream->liveState = AMBA_EXAMFRAMESHARERTSP_STATUS_START;
//        AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_START);
        return 0;
    } else {
        fprintf(stderr,"%s fail", __FUNCTION__);
        return -1;
    }
}
// Reset all active streamerGroup and delete fifo and reset the streamer
static int AmbaStreamerLive_StopEncode(void)
{
    unsigned int i, j;
    AmbaStream_Streamer_t* stm;
    if (g_ambastream->liveState != AMBA_EXAMFRAMESHARERTSP_STATUS_START) {
        fprintf(stderr,"[StreamerMgr] live streams are not inited, ignore STOP_ENCODE request");
        return -1;
    }

    for (i = 0; i < g_ambastream->maxLiveGroup; i++) {
        if (pthread_mutex_lock(&g_ambastream->liveGroup[i].mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        if (g_ambastream->liveGroup[i].name != NULL) {
            for (j = 0; j < g_ambastream->maxStreamerPerLiveGroup; j++) {
                stm = g_ambastream->liveGroup[i].stms[j];
                if (stm == NULL){
                    continue;
                }
                if (pthread_mutex_lock(&stm->mtx) != 0) {
                    fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
                    continue;
                }

                stm->media.hCodec = 0;
                pthread_mutex_unlock(&stm->mtx);
                g_ambastream->liveGroup[i].stms[j] = NULL;
            }
            free(g_ambastream->liveGroup[i].name);
            g_ambastream->liveGroup[i].name = NULL;
            g_ambastream->liveGroup[i].StrmId = 0xFF;
        }
        pthread_mutex_unlock(&g_ambastream->liveGroup[i].mtx);
    }
    g_ambastream->liveState = AMBA_EXAMFRAMESHARERTSP_STATUS_END;
//    AmbaNetFifo_ReportStatus(AMBA_NETFIFO_STATUS_END);
    return 0;

}
// Delete fifo without sending EOS to our client


static int cbFrameReady(int type, void* info)
{
    AmbaStreamerLive_ProcessFrame(type, info);
    return 0;
}

static int cbGSensorFrameReady(int type, void* info)
{
    AmbaStreamerGSensor_ProcessFrame(type, info);
    return 0;
}

int AmbaStream_init(void)
{
    int rval = 0, idx;

    if (g_ambastream != NULL) {
        printf("%s: already inited()\n",__FUNCTION__);
        return 0;
    }
    g_ambastream = (AmbaStream_GlobalParam_t *)malloc(sizeof(AmbaStream_GlobalParam_t));
    if (g_ambastream == NULL) {
        fprintf(stderr,"%s: fail to create g_ambastream!!\n", __FUNCTION__);
        return -1;
    }
    memset(g_ambastream, 0, sizeof(AmbaStream_GlobalParam_t));
    // initialize liveGroup structure
    g_ambastream->maxLiveGroup = STREAMER_MGR_MAX_ACTIVE_LIVE_STREAM_GROUP;
    g_ambastream->maxStreamer = STREAMER_MGR_MAX_STREAMERS;
    g_ambastream->maxStreamerPerLiveGroup = STREAMER_MGR_MAX_STREAM_PER_GROUP;
    g_ambastream->liveGroup = (streamerGroup_t*) malloc(g_ambastream->maxLiveGroup * sizeof(streamerGroup_t));
    memset(g_ambastream->liveGroup, 0, g_ambastream->maxLiveGroup * sizeof(streamerGroup_t));
    for (idx = 0; idx < g_ambastream->maxLiveGroup; idx++) {
        if(pthread_mutex_init(&g_ambastream->liveGroup[idx].mtx, NULL) != 0){
            fprintf(stderr,"%s: Fail to create liveGroup[%d] mutex!!\n", __FUNCTION__, idx);
            AmbaStream_release();
            return -1;
        }
    }
    g_ambastream->streamerList = (AmbaStream_Streamer_t*) malloc(g_ambastream->maxStreamer * sizeof(AmbaStream_Streamer_t));
    memset(g_ambastream->streamerList, 0, g_ambastream->maxStreamer * sizeof(AmbaStream_Streamer_t));
    for (idx = 0; idx < g_ambastream->maxStreamer; idx++) {
        if(pthread_mutex_init(&g_ambastream->streamerList[idx].mtx, NULL) != 0){
            fprintf(stderr,"%s: Fail to create streamer[%d] mutex!!\n", __FUNCTION__, idx);
            AmbaStream_release();
            return -1;
        }
    }

    rval = AmbaFrameShareRtsp_Rpcinit();
    if(rval < 0) {
        printf("@@ Fail to do init_RPC()\n");
        return -1;
    }

    rval = AmbaFrameShareRtsp_init(&g_ambastream->EventHndlr);
    if (rval < 0) {
        fprintf(stderr,"%s: Fail to do AmbaFrameShareRtsp_init()\n", __FUNCTION__);
        AmbaStream_release();
        return -1;
    }
    AmbaFrameShareRtsp_Reg_cbFrameShareEvent(cbFrameReady, NULL);
    AmbaFrameShareRtspGSensor_Reg_cbFrameShareEvent(cbGSensorFrameReady, NULL);
    /* Try start encode here in case RTSP server run after encode start */
    AmbaStreamerLive_StartEncode();

    AmbaFrameShareRtsp_Ready();

    return rval;
}

int AmbaStream_release(void)
{
    int idx;

    AmbaStreamerLive_StopEncode();
    AmbaFrameShareRtsp_release();

    // release streamerList
    for (idx = 0; idx < g_ambastream->maxStreamer; idx++) {
        pthread_mutex_destroy(&g_ambastream->streamerList[idx].mtx);
    }
    free(g_ambastream->streamerList);

    // release liveGroup
    for (idx = 0; idx < g_ambastream->maxLiveGroup; idx++) {
        pthread_mutex_destroy(&g_ambastream->liveGroup[idx].mtx);
    }
    free(g_ambastream->liveGroup);
    free(g_ambastream);

    return 0;
}

void AmbaStream_stop_waiting_encode(void* streamer)
{
    //printf("%s: dummy operation. s_type=%p !!\n",__FUNCTION__,streamer);
    return;
}

int AmbaStream_set_ServerLink(char *s_link)
{
    printf("%s: dummy operation!!\n",__FUNCTION__);
    printf("s_link: %s\n",s_link);
    return 0;
}

int AmbaStream_get_itrontime(double *ref_time)
{
    struct timespec tt;
    double rval;

    clock_gettime(CLOCK_MONOTONIC, &tt);
    rval = tt.tv_nsec;
    rval = rval/1000000000 + tt.tv_sec;

    *ref_time = rval;
    return 0;
}

int AmbaStreamer_enable(struct AmbaStream_Streamer_s* stm)
{
    if (stm == NULL) {
        return -1;
    }
    pthread_mutex_lock(&stm->mtx);
    stm->media.enabled = 1;
    pthread_mutex_unlock(&stm->mtx);

    return 0;
}

int AmbaStreamer_disable(struct AmbaStream_Streamer_s* stm)
{
    if (stm == NULL) {
        return -1;
    }
    pthread_mutex_lock(&stm->mtx);
    stm->media.enabled = 0;
    pthread_mutex_unlock(&stm->mtx);

    return 0;
}

int AmbaStreamer_RegisterFrameReadyCallback(struct AmbaStream_Streamer_s* stm, frame_ready_cb cb_func, void* ctx)
{
    if (stm == NULL) {
        return -1;
    }
    pthread_mutex_lock(&stm->mtx);
    stm->media.cb_fr = cb_func;
    stm->media.ctx_cb_fr = ctx;
    pthread_mutex_unlock(&stm->mtx);

    return 0;
}

int AmbaStreamer_RegisterBufReadyCallback(struct AmbaStream_Streamer_s* stm, buf_ready_cb cb_func, void* ctx)
{
    if (stm == NULL) {
        return -1;
    }
    pthread_mutex_lock(&stm->mtx);
    stm->media.cb_br = cb_func;
    stm->media.ctx_cb_br = ctx;
    pthread_mutex_unlock(&stm->mtx);

    return 0;
}

int AmbaStreamer_GetGOPInfo(struct AmbaStream_Streamer_s* stm, unsigned short* gopM, unsigned short* gopN,
        unsigned short* gopSize, unsigned short* IRCycle, unsigned short* recovery_frame_cnt)
{
    if(stm == NULL || gopM == NULL || gopN == NULL || gopSize == NULL ||
            IRCycle == NULL || recovery_frame_cnt == NULL){
        fprintf(stderr,"%s: invalid parameter! gopM=%p, gopN=%p, gopSize=%p, \
                IRCycle=%p, recovery_frame_cnt=%p\n", __FUNCTION__, gopM, gopN,
                gopSize, IRCycle, recovery_frame_cnt);
        return -1;
    }

    pthread_mutex_lock(&stm->mtx);
    if(stm->param.video.updated == 0) {
        printf("%s: GOP info is not valid yet\n",__FUNCTION__);
        pthread_mutex_unlock(&stm->mtx);
        return 0;
    }
    *gopM = stm->param.video.gopM;
    *gopN = stm->param.video.gopN;
    *gopSize = stm->param.video.gopSize;
    *IRCycle = stm->param.video.IRCycle;
    *recovery_frame_cnt = stm->param.video.recoveryFrameCnt;
    pthread_mutex_unlock(&stm->mtx);

    return 1;

}

int AmbaStreamer_GetVPS(struct AmbaStream_Streamer_s* stm, u8 *VPS, int *VPS_len)
{
    if (stm == NULL || VPS == NULL || VPS_len == NULL ) {
        fprintf(stderr,"%s: invalid parameter! VPS=%p, VPS_len=%p\n",
                __FUNCTION__, VPS, VPS_len);
        return -1;
    }

    pthread_mutex_lock(&stm->mtx);
    if(stm->param.video.updated == 0) {
        printf("%s: GOP info is not valid yet\n",__FUNCTION__);
        pthread_mutex_unlock(&stm->mtx);
        return 0;
    }
    *VPS_len = stm->param.video.vps_len;
    memcpy(VPS, stm->param.video.vps, stm->param.video.vps_len);
    pthread_mutex_unlock(&stm->mtx);

    return 1;
}

int AmbaStreamer_GetSPSPPS(struct AmbaStream_Streamer_s* stm,
    u8 *SPS, int *SPS_len, u8 *PPS, int *PPS_len, unsigned int *ProfileLevelID)
{
    if (stm == NULL || SPS==NULL || SPS_len==NULL ||
        PPS==NULL || PPS_len==NULL || ProfileLevelID==NULL ) {
        fprintf(stderr,"%s: invalid parameter! SPS=%p, SPS_len=%p, PPS=%p, PPS_len=%p, ProfileLevelID=%p\n",
                __FUNCTION__, SPS, SPS_len, PPS, PPS_len, ProfileLevelID);
        return -1;
    }

    pthread_mutex_lock(&stm->mtx);
    if(stm->param.video.updated == 0) {
        printf("%s: SPS PPS info is not valid yet\n",__FUNCTION__);
        pthread_mutex_unlock(&stm->mtx);
        return 0;
    }
    *SPS_len = stm->param.video.sps_len;
    memcpy(SPS, stm->param.video.sps, stm->param.video.sps_len);
    *PPS_len = stm->param.video.pps_len;
    memcpy(PPS, stm->param.video.pps, stm->param.video.pps_len);
    *ProfileLevelID = stm->param.video.profileLevelid;

    pthread_mutex_unlock(&stm->mtx);
    return 1;
}

int AmbaStreamer_GetAudioConf(struct AmbaStream_Streamer_s* stm, int *format, int *SampleRate, int *channels)
{
    if (stm == NULL || format == NULL || SampleRate == NULL || channels == NULL) {
        return -1;
    }

    pthread_mutex_lock(&stm->mtx);
    if(stm->param.audio.updated == 0) {
        printf("%s: GOP info is not valid yet\n",__FUNCTION__);
        pthread_mutex_unlock(&stm->mtx);
        return 0;
    }
    *format = stm->media.mediaId;
    *SampleRate = stm->param.audio.SampleRate;
    *channels = stm->param.audio.channels;
    pthread_mutex_unlock(&stm->mtx);
    return 0;
}

int AmbaStreamer_GetTextConf(struct AmbaStream_Streamer_s* stm, unsigned int *TimeScale, unsigned int *TimePerFrame)
{
    if (stm == NULL || TimeScale == NULL || TimePerFrame == NULL) {
        return -1;
    }
    pthread_mutex_lock(&stm->mtx);

    *TimeScale = stm->param.text.TimeScale;
    *TimePerFrame = stm->param.text.TimePerFrame;

    pthread_mutex_unlock(&stm->mtx);
    return 0;
}

int AmbaStreamer_GetTrackType(struct AmbaStream_Streamer_s* stm)
{
    if (stm == NULL) {
        return -1;
    }
    return stm->trackType;
}

int AmbaStreamer_IsLive(struct AmbaStream_Streamer_s* stm)
{
    if (stm == NULL) {
        return -1;
    }
    return stm->isLive;
}

void* AmbaStreamer_GetCodec(struct AmbaStream_Streamer_s* stm)
{
    if (stm == NULL) {
        return NULL;
    }
    return stm->media.hCodec;
}

int AmbaStream_send_RRstat(struct AmbaStream_Streamer_s* stm, unsigned int fr_lost, unsigned jitter, double pg_delay)
{
    //RR_report_t report;
    /*printf("%s: send RR_STAT, fr_lost = %d, jitter = %d, pg_delay = %g!!\n",
      __FUNCTION__,fr_lost, jitter, pg_delay);*/
    //report.fr_lost = fr_lost;
    //report.jitter = jitter;
    //report.pg_delay = pg_delay;
    //AmbaStream_send_PbCmd(AMBASTREAM_SEND_RR_STAT, (void*)&report, stm->media.hCodec, 0, 0);
    return 0;
}

int AmbaStream_send_PbCmd(int type, void *value, void *hdlr, int bSize, void *result)
{
    int rval = 0;
    AMBA_NETFIFO_PLAYBACK_OP_PARAM_s in = {0};
    AMBA_NETFIFO_PLAYBACK_OP_PARAM_s out = {0};

    in.OP = type;
    rval = AmbaNetFifo_PlayBack_OP(&in, &out);
    if (rval < 0) {
        fprintf(stderr, "%s: fail to do AmbaNetFifo_PlayBack_OP(0x%08x), %d\n", __FUNCTION__, type, rval);
    } else {
        rval = out.OP; /* out.OP used as int result */
        if (result != NULL && bSize != 0) {
            memcpy(result, out.Param, bSize > 128 ? 128 : bSize); //limited by ipc struct definition
        }
    }

    return rval;
}

int AmbaStreamerLive_GetMediaID(const char* streamName, AmbaStreamer_MediaList_t* mediaList)
{
    unsigned int i,j;
    unsigned int mediaCount = 0;
    AmbaStream_Streamer_t* stm;
    if (streamName == NULL || mediaList == NULL) {
        fprintf(stderr, "%s: Invalid input", __FUNCTION__);
        return -1;
    }
    for (i = 0; i < g_ambastream->maxLiveGroup; i++) {
        if (pthread_mutex_lock(&g_ambastream->liveGroup[i].mtx) != 0) {
            fprintf(stderr, "%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        if (g_ambastream->liveGroup[i].name != NULL && strcmp(g_ambastream->liveGroup[i].name, streamName) == 0) {
            mediaCount = 0;
            for (j = 0; j < g_ambastream->maxStreamerPerLiveGroup; j++) {
                stm = g_ambastream->liveGroup[i].stms[j];
                if (stm == NULL){
                    continue;
                }
                if (pthread_mutex_lock(&stm->mtx) != 0) {
                    fprintf(stderr, "%s: Mutex acquire fail", __FUNCTION__);
                    continue;
                }
                mediaList[mediaCount].mediaId= stm->media.mediaId;
                mediaList[mediaCount++].stm = stm;
                pthread_mutex_unlock(&stm->mtx);
            }
            pthread_mutex_unlock(&g_ambastream->liveGroup[i].mtx);
            break;
        }
        pthread_mutex_unlock(&g_ambastream->liveGroup[i].mtx);
    }
    return mediaCount;
}

int AmbaStreamerLive_StartStream(void)
{
    unsigned int i, groupIndex, streamIndex;
    int status = -1, rval = -1, idx = 0;
    AmbaStream_Streamer_t* stm;
    AMBA_EXAMFRAMESHARERTSP_MEDIA_STREAMITEM_LIST_s sList;
    AMBA_EXAMFRAMESHARERTSP_MOVIE_INFO_CFG_s mInfo;

    if (g_ambastream == NULL) {
        return -1;
    }

    if (g_ambastream->liveState == AMBA_EXAMFRAMESHARERTSP_STATUS_START) {
        fprintf(stderr, "[StreamerMgr] live streams are already inited, ignore START_ENCODE request\n");
        return -1;
    }

    for (idx = 0; idx < g_ambastream->maxStreamer; idx++) {
        g_ambastream->streamerList[idx].media.handled_type = 0;
    }
    AmbaFrameShareRtsp_fifoCreate();
    AmbaFrameShareRtsp_Reg_cbFrameShareEvent(cbFrameReady, NULL);
    // get streamList
    rval = AmbaExamFrameShareRtsp_GetMediaStreamIDList(&sList);

    if (rval != 0 || sList.Amount < 1) {
        fprintf(stderr, "%s: no stream in streamList\n", __FUNCTION__);
        return -1;
    }

    for (streamIndex = 0, groupIndex = 0; streamIndex < sList.Amount; streamIndex++) {
        // check stream active flag
        if (sList.StreamItemList[streamIndex].Active == 0) {
            continue;
        }
        // check liveGroup space
        if (groupIndex >= g_ambastream->maxLiveGroup) {
            fprintf(stderr,"[StreamerMgr] No enough space for all active live stream\n");
            break;
        }
        // get media info of stream
        rval = AmbaExamFrameShareRtsp_GetMediaInfo(sList.StreamItemList[streamIndex].StrmId, &mInfo);
        if (rval != 0 || mInfo.nTrack == 0) {
            fprintf(stderr,"%s: no track in mediaInfo", __FUNCTION__);
            continue;
        } else if (mInfo.nTrack > g_ambastream->maxStreamerPerLiveGroup) {
            mInfo.nTrack = g_ambastream->maxStreamerPerLiveGroup;
            fprintf(stderr,"[StreamerMgr] No enough space to create all tracks in stream(%u)\n", streamIndex);
        }
        // lock this liveGroup
        if (pthread_mutex_lock(&g_ambastream->liveGroup[groupIndex].mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        // create vfifo of each track
        for (i=0; i < mInfo.nTrack; i++) {
            stm = AmbaStreamer_GetSteamerSlot();
            if (stm == NULL) {
                fprintf(stderr,"%s: no available streamer instance", __FUNCTION__);
                status = -1;
                break;
            }
            if (pthread_mutex_lock(&stm->mtx) != 0) {
                fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
                status = -1;
                break;
            }
            fprintf(stderr,"%s: i=%d", __FUNCTION__,i);
            AmbaStreamer_CopyTrackInfo(stm, &mInfo.Track[i], i);
            stm->isLive = 1;
            stm->strmId = sList.StreamItemList[streamIndex].StrmId;

            fprintf(stderr,"%s:[stm 0x%p] trktyp %d, strid %d\n", __FUNCTION__, stm, stm->trackType, stm->strmId);
            g_ambastream->liveGroup[groupIndex].stms[i] = stm; //bind streamer into LiveGroup
            status = 0;
            pthread_mutex_unlock(&stm->mtx);

        }
        if (status >=0) {
            int length = strlen(sList.StreamItemList[streamIndex].Name) + 1;
            g_ambastream->liveGroup[groupIndex].name = (char*) malloc(sizeof(char)*length);
            strncpy(g_ambastream->liveGroup[groupIndex].name, sList.StreamItemList[streamIndex].Name, length);
            g_ambastream->liveGroup[groupIndex].StrmId = sList.StreamItemList[streamIndex].StrmId;
            fprintf(stderr,"[StreamerMgr] Start stream(%u)(%s) StrmId# %d success\n", streamIndex, g_ambastream->liveGroup[groupIndex].name, \
                    g_ambastream->liveGroup[groupIndex].StrmId);
            pthread_mutex_unlock(&g_ambastream->liveGroup[groupIndex].mtx);
            groupIndex++;
        } else {
            pthread_mutex_unlock(&g_ambastream->liveGroup[groupIndex].mtx);
            break;
        }
    }

    for (i = 0; i < g_ambastream->maxStreamer; i++) {
        stm = &g_ambastream->streamerList[i];
        fprintf(stderr,"%s:[stm 0x%p] trktyp %d, strid %d\n", __FUNCTION__, stm, stm->trackType, stm->strmId);
    }

    if (status >= 0) {
        fprintf(stderr,"[StreamerMgr] Start all live streams success\n");
        g_ambastream->liveState = AMBA_EXAMFRAMESHARERTSP_STATUS_START;
        return 0;
    } else {
        fprintf(stderr,"%s fail", __FUNCTION__);
        return -1;
    }
}

int AmbaStreamerLive_StopStream(void)
{
    unsigned int i, j;
    AmbaStream_Streamer_t* stm;
    
    if (g_ambastream->liveState != AMBA_EXAMFRAMESHARERTSP_STATUS_START) {
        fprintf(stderr,"[StreamerMgr] live streams are not inited, ignore STOP_ENCODE request");
        return -1;
    }

    for (i = 0; i < g_ambastream->maxLiveGroup; i++) {
        if (pthread_mutex_lock(&g_ambastream->liveGroup[i].mtx) != 0) {
            fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
            continue;
        }
        if (g_ambastream->liveGroup[i].name != NULL) {
            for (j = 0; j < g_ambastream->maxStreamerPerLiveGroup; j++) {
                stm = g_ambastream->liveGroup[i].stms[j];
                if (stm == NULL){
                    continue;
                }
                if (pthread_mutex_lock(&stm->mtx) != 0) {
                    fprintf(stderr,"%s: Mutex acquire fail", __FUNCTION__);
                    continue;
                }
                stm->media.hCodec = 0;
                pthread_mutex_unlock(&stm->mtx);
                g_ambastream->liveGroup[i].stms[j] = NULL;
            }
            free(g_ambastream->liveGroup[i].name);
            g_ambastream->liveGroup[i].name = NULL;
            g_ambastream->liveGroup[i].StrmId = 0xFF;
        }
        pthread_mutex_unlock(&g_ambastream->liveGroup[i].mtx);
    }
    AmbaFrameShareRtsp_fifoStop();
    g_ambastream->liveState = AMBA_EXAMFRAMESHARERTSP_STATUS_END;

    return 0;
}

