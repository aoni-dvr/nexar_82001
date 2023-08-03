#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "gsensor_frame_buffer.h"

gsensor_queue_s *gsensor_queue_create(void)
{
    int i = 0;
    gsensor_queue_s *queue = malloc(sizeof(gsensor_queue_s));

    if (queue == NULL) {
        return NULL;
    }
    queue->write_index = -1;
    queue->read_index = -1;
    for (i = 0; i < GSENSOR_QUEUE_MAX_SIZE; i++) {
        queue->frames[i].len = 0;
        memset(queue->frames[i].buffer, 0, sizeof(queue->frames[i].buffer));
    }
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        fprintf(stderr,"%s: Fail to create mutex!\n",__FUNCTION__);
    }

    return queue;
}

int gsensor_queue_reset(gsensor_queue_s *queue)
{
    int i = 0;

    if (queue == NULL) {
        return -1;
    }
    pthread_mutex_lock(&queue->mutex);
    queue->write_index = -1;
    queue->read_index = -1;
    for (i = 0; i < GSENSOR_QUEUE_MAX_SIZE; i++) {
        queue->frames[i].len = 0;
        memset(queue->frames[i].buffer, 0, sizeof(queue->frames[i].buffer));
    }
    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

int gsensor_queue_get(gsensor_queue_s *queue, gsensor_frame_info_s **gsensor_frame_info, int *index)
{
    if (queue == NULL) {
        return -1;
    }    
    int tmp_index = queue->write_index;

    tmp_index += 1;
    if (tmp_index >= GSENSOR_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    *index = tmp_index;
    *gsensor_frame_info = &queue->frames[tmp_index];

    return 0;
}

int gsensor_queue_push(gsensor_queue_s *queue, int index)
{
    if (queue == NULL) {
        return -1;
    }
    queue->write_index = index;

    return 0;
}

int gsensor_queue_pop(gsensor_queue_s *queue, gsensor_frame_info_s *gsensor_frame_info)
{
    if (queue == NULL) {
        return -1;
    }    
    pthread_mutex_lock(&queue->mutex);
    int tmp_index = queue->read_index;
    if (queue->write_index < 0) {
        return -1;
    }
    if (queue->read_index == queue->write_index) {
        return -1;
    }
    tmp_index += 1;
    if (tmp_index >= GSENSOR_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    if (queue->frames[tmp_index].len > 0) {
        gsensor_frame_info->len = queue->frames[tmp_index].len;
        memcpy(gsensor_frame_info->buffer, queue->frames[tmp_index].buffer, queue->frames[tmp_index].len);
        queue->read_index = tmp_index;
    }
    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

void gsensor_queue_release(gsensor_queue_s *queue)
{
    if (queue == NULL) {
        return;
    }
    pthread_mutex_destroy(&queue->mutex);
    free(queue);
}

