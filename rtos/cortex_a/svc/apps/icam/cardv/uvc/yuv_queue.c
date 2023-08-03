#include "app_base.h"

static yuv_queue_s yuv_queue;
static AMBA_KAL_MUTEX_t Mutex;
int uvc_yuv_queue_init(void)
{
    int i = 0;
    static int inited = 0;

    if (inited == 0) {
        if (AmbaKAL_MutexCreate(&Mutex, "uvc_yuv_queue") != OK) {
            return -1;
        }       
        inited = 1;
    }

    yuv_queue.write_index = -1;
    yuv_queue.read_index = -1;
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    for (i = 0; i < YUV_QUEUE_MAX_SIZE; i++) {
        memset(&yuv_queue.queue[i], 0, sizeof(yuv_queue.queue[i]));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int uvc_yuv_queue_get(yuv_frame_info_s **frame_info, int *index)
{
    int tmp_index = yuv_queue.write_index;

    tmp_index += 1;
    if (tmp_index >= YUV_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    *index = tmp_index;
    *frame_info = &yuv_queue.queue[tmp_index];

    return 0;
}

int uvc_yuv_queue_push(int index)
{
    yuv_queue.write_index = index;

    return 0;
}

int uvc_yuv_queue_pop(yuv_frame_info_s *frame_info)
{
    int tmp_index = yuv_queue.read_index;

    if (yuv_queue.write_index < 0) {
        return -1;
    }
    if (yuv_queue.read_index == yuv_queue.write_index) {
        return -1;
    }
    tmp_index += 1;
    if (tmp_index >= YUV_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    if (yuv_queue.queue[tmp_index].w > 0 && yuv_queue.queue[tmp_index].h > 0) {
        frame_info->w = yuv_queue.queue[tmp_index].w;
        frame_info->h = yuv_queue.queue[tmp_index].h;
        frame_info->color_fmt = yuv_queue.queue[tmp_index].color_fmt;
        memcpy(frame_info->yuv_buffer, yuv_queue.queue[tmp_index].yuv_buffer, sizeof(yuv_queue.queue[tmp_index].yuv_buffer));
        yuv_queue.read_index = tmp_index;
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int uvc_yuv_queue_destroy(void)
{
    return 0;
}

