#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

#include "video_frame_reader.h"
#include "ambastream.h"
#include "amba_frameshare.h"


#define MAX_BUF_SIZE            (2 << 20)    // 2MB
#define MAX_FRAME_BUFFED        (1024)
#define MAX_PB_BUF_SIZE         (5 << 20)    // 5MB
#define MAX_PB_FRAME_BUFFED     (1024)

static void frame_ready_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx); //fwd
static void frame_ready_pb_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx); //fwd

static int get_chk_next_frame_meta_nonblock(struct frame_reader* thiz,
                                            struct frame_info* frame)
{
    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->priv;

    int ret = framed_buf_next_frame(priv->buf);
    if (ret != 0) {
        return ret;
    }

    ret = framed_buf_get_cur_frame_addr_info(priv->buf, &frame->addr_info);

    // Check status of frame buffer
    framed_buf_check_fullness(priv->buf);
    if (ret>=0) {
        AmbaStream_frameinfo_t* frameInfo =
            (AmbaStream_frameinfo_t*)framed_buf_get_cur_frame_extra_info(priv->buf);
        frame->frame_num = frameInfo->frame_num;
        frame->pts = frameInfo->pts;
        frame->mark = frameInfo->mark;
    }

    return ret;
}

static int frame_reader_get_sps_pps(struct video_frame_reader* thiz,
                unsigned char* sps, unsigned* sps_len,
                unsigned char* pps, unsigned* pps_len,
                unsigned* profile_id) {
    int rval;
    frame_reader_priv_t* priv;
    if (thiz == NULL) {
        return -1;
    }
    priv = (frame_reader_priv_t*) thiz->super.priv;
    rval = AmbaStreamer_GetSPSPPS(priv->streamer, sps, (int*)sps_len, pps, (int*)pps_len, profile_id);
    if(rval>=0){
        int i;
        if(sps_len>0){
            //Skip NAL Header (00 00 00 01)
            if(sps[0]==0 && sps[1]==0 && sps[2]==0 && sps[3]==1){
                for(i=0;i<(*sps_len)-4;i++){
                    sps[i]=sps[i+4];
                }
                (*sps_len)-=4;
            }
        }
        if(pps_len>0){
            //Skip NAL Header (00 00 00 01)
            if(pps[0]==0 && pps[1]==0 && pps[2]==0 && pps[3]==1){
                for(i=0;i<(*pps_len)-4;i++){
                    pps[i]=pps[i+4];
                }
                (*pps_len)-=4;
            }
        }
    }

    return rval;
}

static int frame_reader_get_vps_sps_pps(struct video_frame_reader* thiz,
                unsigned char* vps, unsigned* vps_len,
                unsigned char* sps, unsigned* sps_len,
                unsigned char* pps, unsigned* pps_len) {
    int rval;
    unsigned int profile_id;
    frame_reader_priv_t* priv;
    if (thiz == NULL) {
        return -1;
    }
    priv = (frame_reader_priv_t*) thiz->super.priv;
    rval = AmbaStreamer_GetVPS(priv->streamer, vps, (int*) vps_len);
    if(rval>0){
        int i;
        if(vps_len>0){
            //Skip NAL Header (00 00 00 01)
            if(vps[0]==0 && vps[1]==0 && vps[2]==0 && vps[3]==1){
                for(i=0;i<(*vps_len)-4;i++){
                    vps[i]=vps[i+4];
                }
                (*vps_len)-=4;
            }
        }
    }
    rval = AmbaStreamer_GetSPSPPS(priv->streamer, sps, (int*)sps_len, pps, (int*)pps_len, &profile_id);
    if(rval>0){
        int i;
        if(sps_len>0){
            //Skip NAL Header (00 00 00 01)
            if(sps[0]==0 && sps[1]==0 && sps[2]==0 && sps[3]==1){
                for(i=0;i<(*sps_len)-4;i++){
                    sps[i]=sps[i+4];
                }
                (*sps_len)-=4;
            }
        }
        if(pps_len>0){
            //Skip NAL Header (00 00 00 01)
            if(pps[0]==0 && pps[1]==0 && pps[2]==0 && pps[3]==1){
                for(i=0;i<(*pps_len)-4;i++){
                    pps[i]=pps[i+4];
                }
                (*pps_len)-=4;
            }
        }
    }

    return rval;
}

static int frame_reader_get_gop_info(struct video_frame_reader* thiz,
        unsigned short* gopM, unsigned short* gopN, unsigned short* gopSize,
        unsigned short* IRCycle, unsigned short* recovery_frame_cnt)
{
    int rval;
    frame_reader_priv_t* priv;
    if (thiz == NULL) {
        return -1;
    }
    priv = (frame_reader_priv_t*) thiz->super.priv;
    rval = AmbaStreamer_GetGOPInfo(priv->streamer, gopM, gopN, gopSize,
            IRCycle, recovery_frame_cnt);
    if(rval == 1 && *gopN == 0xff){
        priv->intra_refresh_mode = 1; // need to omit dropFrame/got_1st_idr flow
    }
    return rval;
}

static void frame_ready_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx)
{
    unsigned char pic_type = 0x0;
    video_frame_reader_t* thiz = (video_frame_reader_t*)ctx;
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->super.priv;
    unsigned int len = frameInfo->size;
    write_vec_t write_vec[2];
    int write_ret;

    if(frameInfo->size == 0) {
        fprintf(stderr, "%s: Got EOS. type=%u\n", __FUNCTION__, frameInfo->pic_type);
        write(priv->notify_fd, &priv->notify_eos, 1);
        return;
    }

    if(priv->got_first_idr == 0) {
        // only enqueue the frame after we got the IDR frame
        pic_type = frameInfo->pic_type;
        if(pic_type == AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME ||
                priv->intra_refresh_mode == 1) { //intra refresh mode, don't wait IDR
            priv->got_first_idr = 1;
        } else {
            return;
        }
    }

    if(priv->drop_frame) {
        pic_type = frameInfo->pic_type;
        if ((pic_type == AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME) ||
            (pic_type == AMBA_EXAMFRAMESHARERTSP_TYPE_I_FRAME) ||
            priv->intra_refresh_mode == 1) { // don't wait I in intra-refresh mode
            priv->drop_frame = 0;
        } else {
            return;
        }
    }

    #if 0   // h265 slices will be the same frame num in a frame.
    if(priv->lastFrameNum == frameInfo->frame_num) {
        fprintf(stderr, "%s:%d\n", __FUNCTION__, __LINE__);
        fprintf(stderr, "get the same frame %d\n", frameInfo->frame_num);
    } else
    #endif
    {
        int i = 0;
        unsigned long long start_addr;

        start_addr = frameInfo->start_addr;

        if((start_addr + len) > frameInfo->limit_addr){
            unsigned long long l = frameInfo->limit_addr - start_addr;
            write_vec[i].addr = (unsigned char*)start_addr;
            write_vec[i].size = l;
            start_addr = frameInfo->base_addr;
            len -= l;
            i++;
        }
        write_vec[i].addr = (unsigned char*)start_addr;
        write_vec[i].size = len;
        i++;

        write_ret = framed_buf_write_one_frame2(priv->buf, priv->nalu_offset, &write_vec[0], i, frameInfo);
        if (write_ret >= 0){
            if (priv->waiting == 1 ) {  //read only here, not use lock
                pthread_mutex_lock(&priv->mutex);
                pthread_cond_signal(&priv->cond);
                pthread_mutex_unlock(&priv->mutex);
            }
            write(priv->notify_fd, &priv->notify_data_available, 1);
        } else if (write_ret == -ENOMEM){
            printf("framed buffer is full\n");
            priv->drop_frame = 1;
        }

        priv->lastFrameNum = frameInfo->frame_num;
    }
}

static void frame_ready_pb_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx) {
    unsigned char pic_type = 0x0;
    video_frame_reader_t* thiz = (video_frame_reader_t*)ctx;
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->super.priv;
    unsigned len = frameInfo->size;
    write_vec_t write_vec[2];
    int write_ret;

    if(frameInfo->size == 0) {
        write(priv->notify_fd, &priv->notify_eos, 1);
        return;
    }

    if(priv->got_first_idr == 0) {
        // only enqueue the frame after we got the IDR frame
        pic_type = frameInfo->pic_type;
        if(pic_type == AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME
                || priv->intra_refresh_mode == 1) { //Intra refresh mode, do not wait IDR
            priv->got_first_idr = 1;
        } else {
            return;
        }
    }

    if(priv->drop_frame) {
        pic_type = frameInfo->pic_type;
        if ((pic_type == AMBA_EXAMFRAMESHARERTSP_TYPE_IDR_FRAME) ||
            (pic_type == AMBA_EXAMFRAMESHARERTSP_TYPE_I_FRAME) ||
            priv->intra_refresh_mode == 1) { // don't wait I in Intra refresh mode
            priv->drop_frame = 0;
        } else {
            return;
        }
    }

    if(0){//priv->lastFrameNum == frameInfo->frame_num
        //&& frameInfo->frame_num != 0) {
        fprintf(stderr, "get the same frame %d\n", frameInfo->frame_num);
    } else {
        int i = 0;
        unsigned long long start_addr;

        start_addr = frameInfo->start_addr;

        if((start_addr + len) > frameInfo->limit_addr){
            unsigned l = frameInfo->limit_addr - start_addr;
            write_vec[i].addr = (unsigned char*)start_addr;
            write_vec[i].size = l;
            start_addr = frameInfo->base_addr;
            len -= l;
            i++;
        }
        write_vec[i].addr = (unsigned char*)start_addr;
        write_vec[i].size = len;
        i++;

        write_ret = framed_buf_write_one_frame2(priv->buf, priv->nalu_offset, &write_vec[0], i, frameInfo);
        if (write_ret >= 0){
            if (priv->waiting == 1 ) {  //read only here, not use lock
                pthread_mutex_lock(&priv->mutex);
                pthread_cond_signal(&priv->cond);
                pthread_mutex_unlock(&priv->mutex);
            }
            write(priv->notify_fd, &priv->notify_data_available, 1);
        } else if (write_ret == -ENOMEM){
            printf("framed buffer is full\n");
            priv->drop_frame = 1;
        }

        priv->lastFrameNum = frameInfo->frame_num;
    }
}


struct video_frame_reader* video_frame_reader_create(
    int notify_fd, unsigned int nalu_offset, void* streamer)
{
    int rval = 0;
    video_frame_reader_t* thiz = (video_frame_reader_t*)malloc(sizeof(video_frame_reader_t));
    if (thiz == NULL) {
        return NULL;
    }

    if (AmbaStreamer_IsLive(streamer)) {//it's liveview
        rval = frame_reader_init(&thiz->super, notify_fd, nalu_offset,
            MAX_BUF_SIZE, MAX_FRAME_BUFFED, streamer, frame_ready_callback);
    } else {//it's playback
        rval = frame_reader_init(&thiz->super, notify_fd, nalu_offset,
                MAX_PB_BUF_SIZE, MAX_PB_FRAME_BUFFED, streamer, frame_ready_pb_callback);
        thiz->super.get_next_frame_meta_nonblock = get_chk_next_frame_meta_nonblock;
    }

    if (rval < 0) {
        return NULL;
    }

    thiz->get_sps_pps = frame_reader_get_sps_pps;
    thiz->get_vps_sps_pps = frame_reader_get_vps_sps_pps;
    thiz->get_gop_info = frame_reader_get_gop_info;

    return thiz;
}

void video_frame_reader_release(video_frame_reader_t* thiz)
{
    if(thiz == NULL) {
        return;
    }
    frame_reader_deinit(&thiz->super);
    free(thiz);
}
