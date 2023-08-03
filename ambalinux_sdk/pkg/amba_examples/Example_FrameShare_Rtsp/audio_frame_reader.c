#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

#include "audio_frame_reader.h"
#include "ambastream.h"

#define MAX_BUF_SIZE    (128 << 10)       //128KB
#define MAX_FRAME_BUFFED (256)
#define MAX_PB_BUF_SIZE    (1 << 20)      //1MB
#define MAX_PB_FRAME_BUFFED (1024)

static void frame_ready_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx)
{
    struct frame_reader* thiz = (struct frame_reader*)ctx;
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->priv;
    unsigned int len = frameInfo->size;
    write_vec_t write_vec[2];
    int write_ret;
    int i = 0;
    unsigned long long start_addr;

    if(frameInfo->size == 0) {
        write(priv->notify_fd, &priv->notify_eos, 1);
        return;
    }

    // 2015/8/14: A12's audio frame number is not reliable, we omit the checking

    if(0){//priv->lastFrameNum == frameInfo->frame_num)
        fprintf(stderr, "get the same audio frame %d\n", frameInfo->frame_num);
    } else {
        /*if((priv->lastFrameNum!=0) && (priv->lastFrameNum + 1 != frameInfo->frame_num)) {
            printf("%s: miss audio frame, last is %d, cur is %d\n", __FUNCTION__, priv->lastFrameNum, frameInfo->frame_num);
        }*/

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

        write_ret = framed_buf_write_one_frame2(priv->buf, priv->nalu_offset,
                        &write_vec[0], i, frameInfo);
        if (write_ret >= 0){
            if (priv->waiting == 1 ) {  //read only here, not use lock
                pthread_mutex_lock(&priv->mutex);
                pthread_cond_signal(&priv->cond);
                pthread_mutex_unlock(&priv->mutex);
            }
            write(priv->notify_fd, &priv->notify_data_available, 1);
        } else if (write_ret == -ENOMEM){
            printf("framed buffer is full\n");
        }
        priv->lastFrameNum = frameInfo->frame_num;
    }
}


/*
 *  Peek frame, don't advance frame buffer read pointer
 */
static int frame_reader_peek_next_frame_meta(struct audio_frame_reader* thiz,
        struct frame_info* frame)
{
    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*) thiz->super.priv;

    int ret = framed_buf_peek_frame(priv->buf);
    if (ret != 0) {
        return ret;
    }

    ret = framed_buf_get_cur_frame_addr_info(priv->buf, &frame->addr_info);

    if(ret>=0){
        AmbaStream_frameinfo_t* frameInfo =
            (AmbaStream_frameinfo_t*) framed_buf_get_cur_frame_extra_info(priv->buf);
        frame->frame_num = frameInfo->frame_num;
        frame->pts = frameInfo->pts;

    }
    return ret;
}

static int get_chk_next_frame_meta_nonblock(struct frame_reader* thiz,
                                                       struct frame_info* frame)
{
    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*) thiz->priv;

    int ret = framed_buf_next_frame(priv->buf);
    if (ret != 0) {
        return ret;
    }

    ret = framed_buf_get_cur_frame_addr_info(priv->buf, &frame->addr_info);

    // Check status of frame buffer
    framed_buf_check_fullness(priv->buf);

    if(ret>=0){
        AmbaStream_frameinfo_t* frameInfo =
            (AmbaStream_frameinfo_t*) framed_buf_get_cur_frame_extra_info(priv->buf);
        frame->frame_num = frameInfo->frame_num;
        frame->pts = frameInfo->pts;

    }

    return ret;
}
static int frame_reader_get_audio_config(audio_frame_reader_t* thiz,
                        int *format,
                        int* samplerate,
                        int* channel)
{
    frame_reader_priv_t* priv;
    if(thiz == NULL){
        printf("frame reader is null\n");
        return -1;
    }
    priv = thiz->super.priv;
    if(AmbaStreamer_GetAudioConf(priv->streamer, format, samplerate, channel)<0){
        printf("Fail to AmbaStream_get_AudioConf()! Terminate Audio!!\n");
        return -1;
    }
    return 0;
}

audio_frame_reader_t* audio_frame_reader_create(
    int notify_fd, unsigned int nalu_offset, void* streamer)
{
    int rval = 0;
    audio_frame_reader_t* thiz = (audio_frame_reader_t*) malloc(sizeof(audio_frame_reader_t));
    if (thiz == NULL) {
        return NULL;
    }

    if (AmbaStreamer_IsLive(streamer)) {//it's liveview
        rval = frame_reader_init(&thiz->super, notify_fd, nalu_offset,
            MAX_BUF_SIZE, MAX_FRAME_BUFFED, streamer, frame_ready_callback);
    } else {
        rval = frame_reader_init(&thiz->super, notify_fd, nalu_offset,
            MAX_PB_BUF_SIZE, MAX_PB_FRAME_BUFFED, streamer, frame_ready_callback);
        thiz->super.get_next_frame_meta_nonblock = get_chk_next_frame_meta_nonblock;
    }

    if (rval < 0) {
        return NULL;
    }

    thiz->get_audio_config = frame_reader_get_audio_config;
    thiz->peek_next_frame_meta = frame_reader_peek_next_frame_meta;


    return thiz;
}

void audio_frame_reader_release(struct audio_frame_reader* thiz)
{
    if(thiz == NULL) {
        return;
    }
    frame_reader_deinit(&thiz->super);
    free(thiz);
}
