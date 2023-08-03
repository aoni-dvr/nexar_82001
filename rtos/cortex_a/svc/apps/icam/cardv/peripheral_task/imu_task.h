#ifndef __IMU_TASK_H__
#define __IMU_TASK_H__

#include <app_base.h>

int imu_task_start(void);
int imu_task_stop(void);
int imu_task_set_debug(int enable);
int imu_task_get_fifo_data(int max_size, int *frame_num);
int imu_task_get_fifo_share_info(unsigned long long *share_addr, unsigned long long *phy_addr, unsigned int *share_size);

#endif//__IMU_TASK_H__

