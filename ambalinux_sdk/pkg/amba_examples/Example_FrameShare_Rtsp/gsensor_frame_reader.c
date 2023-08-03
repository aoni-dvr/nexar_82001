#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "ambastream.h"
#include "gsensor_frame_reader.h"

static int frame_reader_clr_buf(struct gsensor_frame_reader* thiz)
{
    if (thiz == NULL) {
        return -1;
    }
    gsensor_frame_reader_priv_t *priv = (gsensor_frame_reader_priv_t *)&thiz->priv;
    if (priv == NULL) {
        return -1;
    }
    if (priv->queue == NULL) {
        return -1;
    }
    gsensor_queue_reset(priv->queue);

    return 0;
}

static int frame_reader_start(struct gsensor_frame_reader* thiz)
{
    if (thiz == NULL) {
        return -1;
    }
    gsensor_frame_reader_priv_t *priv = (gsensor_frame_reader_priv_t*)&thiz->priv;
    if (priv == NULL) {
        return -1;
    }
    if (priv->enable == 0) {
        gsensor_queue_reset(priv->queue);
        //register callback
        AmbaStreamer_RegisterFrameReadyCallback(priv->streamer, priv->frame_ready_cb, thiz);
        AmbaStreamer_enable(priv->streamer);
        priv->enable = 1;
    }
    return 0;
}

static int frame_reader_stop(struct gsensor_frame_reader* thiz)
{
    if (thiz == NULL) {
        return -1;
    }
    gsensor_frame_reader_priv_t *priv = (gsensor_frame_reader_priv_t *)&thiz->priv;
    if (priv == NULL) {
        return -1;
    }
    if (priv->enable == 1) {
        AmbaStreamer_disable(priv->streamer);
        priv->enable = 0;
        //unregister callback
        AmbaStreamer_RegisterFrameReadyCallback(priv->streamer, NULL, NULL);
    }
    return 0;
}

static void frame_ready_callback(AmbaStream_frameinfo_t *frameInfo, void *ctx)
{
    gsensor_frame_reader_t *thiz = (gsensor_frame_reader_t *)ctx;

    if (thiz == NULL) {
        return;
    }
    gsensor_frame_reader_priv_t *priv = (gsensor_frame_reader_priv_t *)&thiz->priv;
    if (priv == NULL) {
        return;
    }
    if (frameInfo->size == 0) {
        fprintf(stderr, "%s: Got EOS. type=%u\n", __FUNCTION__, frameInfo->pic_type);
        write(priv->notify_fd, &priv->notify_eos, 1);
        return;
    }
    if (priv->queue == NULL) {
        return;
    }
    int index = 0;
    pthread_mutex_lock(&priv->queue->mutex);
    gsensor_frame_info_s *frame = NULL;
    gsensor_queue_get(priv->queue, &frame, &index);
    memcpy(frame->buffer, (unsigned char *)frameInfo->start_addr, frameInfo->size);
    frame->len = frameInfo->size;
    gsensor_queue_push(priv->queue, index);    
    pthread_mutex_unlock(&priv->queue->mutex);
    write(priv->notify_fd, &priv->notify_data_available, 1);
}

struct gsensor_frame_reader* gsensor_frame_reader_create(void* streamer, int notify_fd)
{
    gsensor_frame_reader_t *thiz = (gsensor_frame_reader_t *)malloc(sizeof(gsensor_frame_reader_t));

    if (thiz == NULL) {
        return NULL;
    }
    gsensor_queue_s *queue = gsensor_queue_create();
    if (queue == NULL) {
        return NULL;
    }
    thiz->start = frame_reader_start;
    thiz->stop = frame_reader_stop;
    thiz->clr_buf = frame_reader_clr_buf;

    gsensor_frame_reader_priv_t *priv = (gsensor_frame_reader_priv_t *)&thiz->priv;
    priv->notify_fd = notify_fd;
    priv->frame_ready_cb = frame_ready_callback;
    priv->streamer = streamer;
    priv->notify_data_available = 'd';
    priv->notify_eos = 'e';
    priv->queue = queue;

    return thiz;
}

void gsensor_frame_reader_release(gsensor_frame_reader_t *thiz)
{
    if (thiz == NULL) {
        return;
    }    
    gsensor_frame_reader_priv_t *priv = (gsensor_frame_reader_priv_t *)&thiz->priv;
    if (priv) {
        gsensor_queue_release(priv->queue);;
    }
    free(thiz);
}

