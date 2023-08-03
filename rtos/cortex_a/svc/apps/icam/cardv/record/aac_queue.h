#ifndef __AAC_QUEUE_H__
#define __AAC_QUEUE_H__

#define AAC_QUEUE_MAX_SIZE (15)
#define AAC_FRAME_SIZE (4096)

typedef struct _aac_frame_info_s_ {
    int len;
    unsigned char buffer[AAC_FRAME_SIZE];
} aac_frame_info_s;

typedef struct _aac_queue_s_ {
    int write_index;
    int read_index;
    aac_frame_info_s queue[AAC_QUEUE_MAX_SIZE];
} aac_queue_s;

int aac_queue_init(void);
int aac_queue_clear(void);
int aac_queue_get(aac_frame_info_s **aac_frame_info, int *index);
int aac_queue_push(int index);
unsigned int aac_queue_get_history_cnt(void);
int aac_queue_pop(aac_frame_info_s *aac_frame_info);
int aac_queue_destroy(void);

#endif//__AAC_QUEUE_H__

