#include "app_base.h"
#include "aac_queue.h"

static aac_queue_s aac_queue;
static AMBA_KAL_MUTEX_t Mutex;
static unsigned int aac_cnt = 0;
int aac_queue_init(void)
{
    int i = 0;
    static int inited = 0;

    if (inited == 0) {
        if (AmbaKAL_MutexCreate(&Mutex, "aac_queue") != OK) {
            return -1;
        }       
        inited = 1;
    }

    aac_queue.write_index = -1;
    aac_queue.read_index = -1;
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    for (i = 0; i < AAC_QUEUE_MAX_SIZE; i++) {
        aac_queue.queue[i].len = 0;
        memset(aac_queue.queue[i].buffer, 0, sizeof(aac_queue.queue[i].buffer));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int aac_queue_clear(void)
{
    int i = 0;

    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    aac_queue.write_index = -1;
    aac_queue.read_index = -1;
    for (i = 0; i < AAC_QUEUE_MAX_SIZE; i++) {
        aac_queue.queue[i].len = 0;
        memset(aac_queue.queue[i].buffer, 0, sizeof(aac_queue.queue[i].buffer));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int aac_queue_get(aac_frame_info_s **aac_frame_info, int *index)
{
    int tmp_index = aac_queue.write_index;

    tmp_index += 1;
    if (tmp_index >= AAC_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    *index = tmp_index;
    *aac_frame_info = &aac_queue.queue[tmp_index];

    return 0;
}

int aac_queue_push(int index)
{
    aac_cnt += 1;
    aac_queue.write_index = index;

    return 0;
}

unsigned int aac_queue_get_history_cnt(void)
{
    return aac_cnt;
}

int aac_queue_pop(aac_frame_info_s *aac_frame_info)
{
    int tmp_index = aac_queue.read_index;

    if (aac_queue.write_index < 0) {
        return -1;
    }
    if (aac_queue.read_index == aac_queue.write_index) {
        return -1;
    }
    tmp_index += 1;
    if (tmp_index >= AAC_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    if (aac_queue.queue[tmp_index].len > 0) {
        aac_frame_info->len = aac_queue.queue[tmp_index].len;
        memcpy(aac_frame_info->buffer, aac_queue.queue[tmp_index].buffer, aac_queue.queue[tmp_index].len);
        aac_queue.read_index = tmp_index;
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int aac_queue_destroy(void)
{
    aac_cnt = 0;
    return 0;
}

