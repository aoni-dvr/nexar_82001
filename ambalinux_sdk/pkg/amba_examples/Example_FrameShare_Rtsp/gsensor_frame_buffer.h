#ifndef __GSENSOR_FRAME_BUFFER_H__
#define __GSENSOR_FRAME_BUFFER_H__

#define GSENSOR_QUEUE_MAX_SIZE (30)
#define GSENSOR_FRAME_SIZE (64)

typedef struct _gsensor_frame_info_s_ {
    int len;
    unsigned char buffer[GSENSOR_FRAME_SIZE];
} gsensor_frame_info_s;

typedef struct _gsensor_queue_s_ {
    pthread_mutex_t mutex;
    int write_index;
    int read_index;
    gsensor_frame_info_s frames[GSENSOR_QUEUE_MAX_SIZE];
} gsensor_queue_s;

gsensor_queue_s *gsensor_queue_create(void);
int gsensor_queue_reset(gsensor_queue_s *queue);
int gsensor_queue_get(gsensor_queue_s *queue, gsensor_frame_info_s **gsensor_frame_info, int *index);
int gsensor_queue_push(gsensor_queue_s *queue, int index);
int gsensor_queue_pop(gsensor_queue_s *queue, gsensor_frame_info_s *gsensor_frame_info);
void gsensor_queue_release(gsensor_queue_s *queue);

#endif//__GSENSOR_FRAME_BUFFER_H__

