#include "app_base.h"
#include "imu_queue.h"

imu_queue_s imu_queue;
static AMBA_KAL_MUTEX_t Mutex;
int imu_queue_init(void)
{
    int i = 0;
    static int inited = 0;

    if (inited == 0) {
        if (AmbaKAL_MutexCreate(&Mutex, "imu_queue") != OK) {
            return -1;
        }
        inited = 1;
    }
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    imu_queue.write_index = -1;
    imu_queue.read_index = -1;
    for (i = 0; i < IMU_QUEUE_MAX_SIZE; i++) {
        memset(&imu_queue.queue[i], 0, sizeof(imu_queue.queue[i]));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int imu_queue_clear(void)
{
    int i = 0;

    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    imu_queue.write_index = -1;
    imu_queue.read_index = -1;
    for (i = 0; i < IMU_QUEUE_MAX_SIZE; i++) {
        memset(&imu_queue.queue[i], 0, sizeof(imu_queue.queue[i]));
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

int imu_queue_get(imu_frame_info_s **imu_frame_info, int *index)
{
    int tmp_index = imu_queue.write_index;

    tmp_index += 1;
    if (tmp_index >= IMU_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    *index = tmp_index;
    *imu_frame_info = &imu_queue.queue[tmp_index];

    return 0;
}

int imu_queue_push(int index)
{
    imu_queue.write_index = index;

    return 0;
}

int imu_queue_get_write_index(void)
{
    return imu_queue.write_index;
}

int imu_queue_get_mutex(void)
{
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);

    return 0;
}

int imu_queue_release_mutex(void)
{
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}

#if 0
int imu_queue_pop(imu_frame_info_s *imu_frame_info)
{
    int tmp_index = imu_queue.read_index;

    if (imu_queue.write_index < 0) {
        return -1;
    }
    if (imu_queue.read_index == imu_queue.write_index) {
        return -1;
    }
    tmp_index += 1;
    if (tmp_index >= IMU_QUEUE_MAX_SIZE) {
        tmp_index = 0;
    }
    AmbaKAL_MutexTake(&Mutex, AMBA_KAL_WAIT_FOREVER);
    if (imu_queue.queue[tmp_index].flag != 0) {
        memcpy(imu_frame_info, &imu_queue.queue[tmp_index], sizeof(imu_queue.queue[tmp_index]));
        imu_queue.queue[tmp_index].flag = 0;
        imu_queue.read_index = tmp_index;
    }
    AmbaKAL_MutexGive(&Mutex);

    return 0;
}
#endif

int imu_queue_destroy(void)
{
    return 0;
}
