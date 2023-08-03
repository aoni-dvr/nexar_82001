#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>


#include "frame_reader.h"
#include "framed_buf.h"
#include "ambastream.h"


static int frame_reader_clr_buf(struct frame_reader* thiz)
{
    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*) thiz->priv;
    if (priv->enable) {
        framed_buf_reset(priv->buf);
        priv->lastFrameNum = 0;
        priv->got_first_idr = 0;
        priv->drop_frame = 0;
    }
    return 0;
}

static int frame_reader_start(struct frame_reader* thiz)
{
    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*) thiz->priv;
    if (priv->enable == 0) {
        framed_buf_reset(priv->buf);
        priv->lastFrameNum = 0;
        priv->got_first_idr = 0;
        priv->drop_frame = 0;

        //register callback
        AmbaStreamer_RegisterFrameReadyCallback(priv->streamer, priv->frame_ready_cb, thiz);
        AmbaStreamer_enable(priv->streamer);
        priv->enable = 1;
    }
    return 0;
}

static int frame_reader_stop(struct frame_reader* thiz)
{
    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*) thiz->priv;
    if (priv->enable == 1) {
        AmbaStreamer_disable(priv->streamer);
        priv->enable = 0;

        //unregister callback
        AmbaStreamer_RegisterFrameReadyCallback(priv->streamer, NULL, NULL);
    }
    return 0;
}

static int frame_reader_get_next_frame_meta(struct frame_reader* thiz,
        struct frame_info* frame)
{
    int ret;

    if (thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->priv;

    while (framed_buf_next_frame(priv->buf) == -EAGAIN ) {
        //wait here
        pthread_mutex_lock(&priv->mutex);
        priv->waiting = 1;  //change the waiting value only here, not in other threads
        pthread_cond_wait(&priv->cond, &priv->mutex);
        priv->waiting = 0;
        pthread_mutex_unlock(&priv->mutex);
    }

    ret = framed_buf_get_cur_frame_addr_info(priv->buf, &frame->addr_info);
    if (ret >= 0) {
        AmbaStream_frameinfo_t* frameInfo =
            (AmbaStream_frameinfo_t*) framed_buf_get_cur_frame_extra_info(priv->buf);
        frame->frame_num = frameInfo->frame_num;
        frame->pts = frameInfo->pts;
    }

    return ret;
}

static int frame_reader_get_next_frame_meta_nonblock(struct frame_reader* thiz,
        struct frame_info* frame)
{
    if(thiz == NULL) {
        return -EINVAL;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->priv;

    int ret = framed_buf_next_frame(priv->buf);
    if (ret != 0) {
        return ret;
    }

    ret = framed_buf_get_cur_frame_addr_info(priv->buf, &frame->addr_info);
    if (ret >= 0) {
        AmbaStream_frameinfo_t* frameInfo =
            (AmbaStream_frameinfo_t*)framed_buf_get_cur_frame_extra_info(priv->buf);
        frame->frame_num = frameInfo->frame_num;
        frame->pts = frameInfo->pts;
        frame->mark = frameInfo->mark;
    }

    return ret;
}

static void default_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx)
{
    struct frame_reader* thiz = (struct frame_reader*)ctx;
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->priv;
    unsigned len = frameInfo->size;
    write_vec_t write_vec[2];
    int write_ret;

    if(frameInfo->size == 0) {
        fprintf(stderr, "%s: Got EOS. type=%u\n", __FUNCTION__, frameInfo->pic_type);
        write(priv->notify_fd, &priv->notify_eos, 1);
        return;
    }

    if(priv->lastFrameNum == frameInfo->frame_num) {
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
        write_ret = framed_buf_write_one_frame2(priv->buf, priv->nalu_offset,
                &write_vec[0], i, frameInfo);
        if (write_ret >= 0) {
            if (priv->waiting == 1 ) {  //read only here, not use lock
                pthread_mutex_lock(&priv->mutex);
                pthread_cond_signal(&priv->cond);
                pthread_mutex_unlock(&priv->mutex);
            }
            write(priv->notify_fd, &priv->notify_data_available, 1);
        } else if (write_ret == -ENOMEM) {
            printf("framed buffer is full\n");
        }
        priv->lastFrameNum = frameInfo->frame_num;
    }
}

void* frame_reader_get_streamer(frame_reader_t* thiz)
{
    frame_reader_priv_t* priv;
    if (thiz == NULL) {
        return 0;
    }
    priv = (frame_reader_priv_t*) thiz->priv;
    return priv->streamer;
}

int frame_reader_init(frame_reader_t* thiz, int notify_fd,
        unsigned int nalu_offset, unsigned int buf_size, unsigned int max_frame_buffed,
        void* streamer, void* frame_ready_cb)
{
    if (thiz == NULL) {
        return -1;
    }
    memset(thiz, 0, sizeof(struct frame_reader));

    frame_reader_priv_t* priv = (frame_reader_priv_t*) malloc(sizeof(frame_reader_priv_t));
    if (priv == NULL) {
        free(thiz);
        return -1;
    }

    memset(priv, 0, sizeof(frame_reader_priv_t));
    priv->notify_fd = notify_fd;
    priv->nalu_offset = nalu_offset;
    priv->streamer = streamer;
    priv->frame_ready_cb = frame_ready_cb == NULL ? default_callback : frame_ready_cb;
    priv->notify_data_available = 'd';
    priv->notify_eos = 'e';
    priv->lastFrameNum = 0;
    priv->enable = 0;
    priv->got_first_idr = 0;
    priv->drop_frame = 0;

    priv->buf = framed_buf_create(buf_size, sizeof(AmbaStream_frameinfo_t), max_frame_buffed);
    if (priv->buf == NULL) {
        fprintf(stderr, "create framed buffer for stream(%p) fail\n", streamer);
        free(priv);
        free(thiz);
        return -1;
    }

    thiz->start = frame_reader_start;
    thiz->stop = frame_reader_stop;
    thiz->clr_buf = frame_reader_clr_buf;
    thiz->get_next_frame_meta = frame_reader_get_next_frame_meta;
    thiz->get_next_frame_meta_nonblock = frame_reader_get_next_frame_meta_nonblock;
    thiz->priv = priv;

    pthread_mutex_init(&priv->mutex, NULL);
    pthread_cond_init(&priv->cond, NULL);
    return 0;
}

void frame_reader_deinit(struct frame_reader* thiz)
{
    if (thiz == NULL) {
        return;
    }
    frame_reader_priv_t* priv = (frame_reader_priv_t*)thiz->priv;
    frame_reader_stop(thiz);

    AmbaStream_stop_waiting_encode(priv->streamer);

    pthread_mutex_destroy(&priv->mutex);
    pthread_cond_destroy(&priv->cond);

    framed_buf_release(priv->buf);
    free(priv);
}
