#ifndef __PCM_QUEUE_H__
#define __PCM_QUEUE_H__

#define PCM_QUEUE_MAX_SIZE (15)
#define PCM_FRAME_SIZE (4096)

typedef struct _pcm_frame_info_s_ {
    int len;
    unsigned char buffer[PCM_FRAME_SIZE];
} pcm_frame_info_s;

typedef struct _pcm_queue_s_ {
    int write_index;
    int read_index;
    pcm_frame_info_s queue[PCM_QUEUE_MAX_SIZE];
} pcm_queue_s;

int pcm_queue_init(void);
int pcm_queue_clear(void);
int pcm_queue_get(pcm_frame_info_s **pcm_frame_info, int *index);
int pcm_queue_push(int index);
unsigned int pcm_queue_get_history_cnt(void);
int pcm_queue_pop(pcm_frame_info_s *pcm_frame_info);
int pcm_queue_destroy(void);

#endif//__PCM_QUEUE_H__

