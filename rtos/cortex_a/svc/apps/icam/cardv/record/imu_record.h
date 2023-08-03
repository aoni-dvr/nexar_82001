#ifndef __IMU_RECORD_H__
#define __IMU_RECORD_H__

#define IMU_RECORD_FILE_OPEN          (1U)
#define IMU_RECORD_FILE_CLOSE         (2U)
#define IMU_RECORD_FILE_SPLIT         (3U)
#define IMU_RECORD_FILE_WRITE         (4U)
#define IMU_RECORD_CACHE_SPLIT        (5U)

#define IMU_FILE_STAT_OPEN       (0x00000001U)
#define IMU_FILE_STAT_SPLIT      (0x00000002U)

typedef struct _imu_record_s_ {
    char                filename[128U];
    void                *pFile;
    unsigned int        status;
    int                 cache_index;
    int                 save_index;
} imu_record_s;

int imu_record_control(unsigned int ctrl_type, unsigned int arg, const char *filename, void *param);
int imu_record_task_start(void);
int imu_record_task_stop(void);

#endif//__IMU_RECORD_H__

