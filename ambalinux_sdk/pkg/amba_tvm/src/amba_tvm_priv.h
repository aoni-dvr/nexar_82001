/*******************************************************************************
 * amba_tvm_priv.h
 *
 * History:
 *    2020/05/06  - [Monica Yang] created
 *
 * Copyright [2020] Ambarella International LP.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

//#include "cavalry_ioctl.h"
//#include "cavalry_mem.h"
//#include "nnctrl.h"
#include "list.h"

#include "amba_tvm.h"

#define TVM_IO_NAME_MAX		(512	)

#ifndef DEVICE_TYPE_AMBA
#define DEVICE_TYPE_AMBA	(27)
#endif

#ifndef DEVICE_ID_AMBA_DSP
#define DEVICE_ID_AMBA_DSP	(255)
#endif

#define tvm_error(str, arg...) printf("[ TVM ERROR ] %s(%d) error: "str, __func__, __LINE__, ##arg)
#define tvm_warning(str, arg...) printf("[ TVM WARNING ] "str, ##arg)
#define tvm_log(str, arg...) printf("[ TVM LOG ] "str, ##arg)

#define TVM_INVALID_VALUE	(0xFFFF)
#define TVM_ENV_KEY_MAX_LEN		(256)
#define TVM_ENV_VAL_MAX_LEN		(2048)

typedef struct {
	uint8_t *virt_addr;
	uint32_t dram_addr;
	uint32_t dram_size;
	uint32_t data_size;
	flexidag_memblk_t fd_blk;

	struct io_dim dim;
	struct io_data_fmt data_fmt;

	char port_name[TVM_IO_NAME_MAX];
} tvm_net_io_match_t;

typedef struct {
	uint32_t cache_en : 1;
	uint32_t reserved : 31;

	unsigned long net_id;
	char net_name[TVM_NET_NAME_MAX];
	const char *net_fn;
        flexidag_memblk_t net_blk;
        AMBA_CV_FLEXIDAG_HANDLE_s handle;

	struct list_head net_node;

	uint32_t net_bw_sz; /* Total size */
	uint32_t net_bw_us; /* VP time, unit: us */
        struct net_mem net_m;
        flexidag_memblk_t state_blk;
        flexidag_memblk_t temp_blk;

	uint32_t in_num;
	uint32_t out_num;
	tvm_net_io_match_t in_pair[MAX_IO_NUM];
	tvm_net_io_match_t out_pair[MAX_IO_NUM];
} tvm_net_match_t;

typedef struct {
	int fd_cav;
	uint32_t init_done;
	uint32_t mod_num;
	pthread_mutex_t net_list_lock;
	struct list_head net_pair_list;
	pthread_mutex_t mod_env_lock;
} tvm_context_t;

void TVM_LOCK(pthread_mutex_t *lock)
{
	if (pthread_mutex_lock(lock)) {
		tvm_error("mutex_lock");
		perror("mutex_lock");
	}
}

void TVM_UNLOCK(pthread_mutex_t *lock)
{
	if (pthread_mutex_unlock(lock)) {
		tvm_error("mutex_unlock");
		perror("mutex_unlock");
	}
}

