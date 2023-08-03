#ifndef __GSENSOR_FRAME_READER_H__
#define __GSENSOR_FRAME_READER_H__

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "gsensor_frame_buffer.h"

typedef struct gsensor_frame_reader_priv {
    int enable;
    int notify_fd;    
    char notify_data_available;
    char notify_eos;
    gsensor_queue_s *queue;
    void *streamer;             // corresponding streamer in AmbaStream
    void (*frame_ready_cb)();
} gsensor_frame_reader_priv_t;

typedef struct gsensor_frame_reader {
    int (*start)(struct gsensor_frame_reader *thiz);
    int (*stop)(struct gsensor_frame_reader *thiz);
    int (*clr_buf)(struct gsensor_frame_reader *thiz);
    gsensor_frame_reader_priv_t priv;
} gsensor_frame_reader_t;

struct gsensor_frame_reader *gsensor_frame_reader_create(void *streamer, int notify_fd);
void gsensor_frame_reader_release(gsensor_frame_reader_t *thiz);

#endif//__GSENSOR_FRAME_READER_H__

