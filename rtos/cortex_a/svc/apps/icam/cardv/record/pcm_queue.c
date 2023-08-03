#include "app_base.h"
#include "pcm_queue.h"

static pcm_queue_s pcm_queue;
static AMBA_KAL_MUTEX_t Mutex;
static unsigned int pcm_cnt = 0;
int pcm_queue_init(void)
{
    int i = 0;
    static int inited = 0;

    if (inited == 0) {
        if (AmbaKAL_MutexCreate(&Mutex, "pcm_queue") != OK) {
            return -1;
        }       
        inited = 1;
    }

    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    pcm_queue.write_index = -1;
    pcm_queue.read_index = -1;
    for (i = 0; i < PCM_QUEUE_MAX_SIZE; i++) {
        pcm_queue.queue[i].len = 0;
        memset(pcm_queue.queue[i].buffer, 0, sizeof(pcm_queue.queue[i].buffer));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int pcm_queue_clear(void)
{
    int i = 0;

    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    pcm_queue.write_index = -1;
    pcm_queue.read_index = -1;
    for (i = 0; i < PCM_QUEUE_MAX_SIZE; i++) {
        pcm_queue.queue[i].len = 0;
        memset(pcm_queue.queue[i].buffer, 0, sizeof(pcm_queue.queue[i].buffer));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int pcm_queue_get(pcm_frame_info_s **pcm_frame_info, int *index)
{
    int tmp_index = pcm_queue.write_index;

    tmp_index += 1;
    if (tmp_index >= PCM_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    *index = tmp_index;
    *pcm_frame_info = &pcm_queue.queue[tmp_index];

    return 0;
}

int pcm_queue_push(int index)
{
    pcm_cnt += 1;
    pcm_queue.write_index = index;

    return 0;
}

unsigned int pcm_queue_get_history_cnt(void)
{
    return pcm_cnt;
}

int pcm_queue_pop(pcm_frame_info_s *pcm_frame_info)
{
    int tmp_index = pcm_queue.read_index;

    if (pcm_queue.write_index < 0) {
        return -1;
    }
    if (pcm_queue.read_index == pcm_queue.write_index) {
        return -1;
    }
    tmp_index += 1;
    if (tmp_index >= PCM_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    if (pcm_queue.queue[tmp_index].len > 0) {
        pcm_frame_info->len = pcm_queue.queue[tmp_index].len;
        memcpy(pcm_frame_info->buffer, pcm_queue.queue[tmp_index].buffer, pcm_queue.queue[tmp_index].len);
        pcm_queue.read_index = tmp_index;
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int pcm_queue_destroy(void)
{
    pcm_cnt = 0;
    return 0;
}

