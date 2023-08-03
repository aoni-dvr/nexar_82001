/*
 *
 *  imu sync
 *
 *  Copyright (C) ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd.
 */
#ifndef IMU_SYNC_H
#define IMU_SYNC_H

#include <linux/types.h>

#define eva_imu_sync_recoder_len_min (1)

typedef struct {
	/* output */
    uint32_t audio_tick_CapSeqNo;
	uint64_t audio_tick;
	uint64_t audio_tick_us;
    uint64_t imu_sync_tick;
    uint64_t imu_sync_tick_us;
} eva_imu_sync_cur_s;

/*
	get
*/
#define EVA_IMUSYNC_IOCTL_GET_CURRENT 		_IOWR('a', 100, eva_imu_sync_cur_s)			// get current audio tick/cap seq no/imu sync tick

#endif
