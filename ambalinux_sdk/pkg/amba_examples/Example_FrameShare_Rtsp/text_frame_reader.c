#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "text_frame_reader.h"
#include "ambastream.h"

#define MAX_BUF_SIZE            (64 << 10)    //64kb
#define MAX_FRAME_BUFFED        (64)
#define MAX_PB_BUF_SIZE         (256 << 10)    //256kb
#define MAX_PB_FRAME_BUFFED     (512)

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

static void frame_ready_pb_callback(AmbaStream_frameinfo_t* frameInfo, void* ctx)
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

    if(0) {//(priv->lastFrameNum == frameInfo->frame_num) {
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

struct frame_reader* text_frame_reader_create(
    int notify_fd, unsigned int nalu_offset, void* streamer)
{
    int rval = 0;
    struct frame_reader* thiz = (struct frame_reader*) malloc(sizeof(struct frame_reader));
    if (thiz == NULL) {
        return NULL;
    }
    if (AmbaStreamer_IsLive(streamer)) {//it's liveview
        rval = frame_reader_init(thiz, notify_fd, nalu_offset,
            MAX_BUF_SIZE, MAX_FRAME_BUFFED, streamer, NULL);
    } else {
        rval = frame_reader_init(thiz, notify_fd, nalu_offset,
            MAX_PB_BUF_SIZE, MAX_PB_FRAME_BUFFED, streamer, frame_ready_pb_callback);
        thiz->get_next_frame_meta_nonblock = get_chk_next_frame_meta_nonblock;
    }

    if(rval < 0) {
        return NULL;
    }
    return thiz;
}

void text_frame_reader_release(struct frame_reader* thiz) {
    if(thiz == NULL) {
        return;
    }
    frame_reader_deinit(thiz);
    free(thiz);
}

