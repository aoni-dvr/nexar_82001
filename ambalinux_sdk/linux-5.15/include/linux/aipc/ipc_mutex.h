/*
 * include/linux/aipc/ipc_mutex.h
 *
 * Authors:
 *	Joey Li <jli@ambarella.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * Copyright (C) 2013-2015, Ambarella Inc.
 */

#ifndef __AIPC_MUTEX_H__
#define __AIPC_MUTEX_H__

/* all OS define the same mutex id */
#define AMBA_IPC_MUTEX_I2C_CHANNEL0     0U
#define AMBA_IPC_MUTEX_I2C_CHANNEL1     1U
#define AMBA_IPC_MUTEX_I2C_CHANNEL2     2U
#define AMBA_IPC_MUTEX_SPI_CHANNEL0     3U
#define AMBA_IPC_MUTEX_SPI_CHANNEL1     4U
#define AMBA_IPC_MUTEX_SPI_CHANNEL2     5U
#define AMBA_IPC_MUTEX_SD0              6U
#define AMBA_IPC_MUTEX_SD1              7U
#define AMBA_IPC_MUTEX_SD2              8U
#define AMBA_IPC_MUTEX_NAND             9U
#define AMBA_IPC_MUTEX_GPIO            10U
#define AMBA_IPC_MUTEX_PLL             11U
#define AMBA_IPC_MUTEX_SPINOR          12U
#define AMBA_IPC_MUTEX_ENET            13U
#define AMBA_IPC_MUTEX_RPMSG           14U
#define AMBA_IPC_MUTEX_VIN             15U
#define AMBA_IPC_MUTEX_VOUT            16U
#define AMBA_IPC_MUTEX_DSPMON          17U
#define AMBA_IPC_NUM_MUTEX             18U

void aipc_mutex_lock(int id);
void aipc_mutex_unlock(int id);

#endif	/* __AIPC_MUTEX_H__ */

