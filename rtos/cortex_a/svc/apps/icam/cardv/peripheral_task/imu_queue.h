#ifndef __IMU_QUEUE_H__
#define __IMU_QUEUE_H__

#define IMU_QUEUE_MAX_SIZE (200)

typedef struct _imu_frame_info_s_ {
    unsigned char flag;
    unsigned int sec;
    unsigned int usec;
    IMU_DATA_s imu_data;
    TEMP_DATA_s temp_data;
} imu_frame_info_s;

typedef struct _imu_queue_s_ {
    int write_index;
    int read_index;
    imu_frame_info_s queue[IMU_QUEUE_MAX_SIZE];
} imu_queue_s;

extern imu_queue_s imu_queue;

int imu_queue_init(void);
int imu_queue_clear(void);
int imu_queue_get(imu_frame_info_s **imu_frame_info, int *index);
int imu_queue_push(int index);
//int imu_queue_pop(imu_frame_info_s *imu_frame_info);
int imu_queue_get_write_index(void);
int imu_queue_get_mutex(void);
int imu_queue_release_mutex(void);
int imu_queue_destroy(void);

#endif//__IMU_QUEUE_H__

