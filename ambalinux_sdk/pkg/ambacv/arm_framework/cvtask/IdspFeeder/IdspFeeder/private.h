/*
* Copyright (c) 2017-2017 Ambarella, Inc.
* 
* This file and its contents ("Software") are protected by intellectual property rights including,
* without limitation, U.S. and/or foreign copyrights.  This Software is also the confidential and
* proprietary information of Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
* distribute, modify, or otherwise prepare derivative works of this Software or any portion thereof
* except pursuant to a signed license agreement or nondisclosure agreement with Ambarella, Inc. or
* its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
* return this Software to Ambarella, Inc.
* 
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
* TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __IDSPFEEDER_PRIVATE_H__
#define __IDSPFEEDER_PRIVATE_H__

#include <pthread.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "idsp_vis_msg.h"
#include "cvapi_idspfeeder_interface.h"
#include "cvapi_idsp_interface_internal.h"

#define MAX_FEEDER_FRAMES       32
#define EMSG(...)               CVTASK_PRINTF(LVL_CRITICAL, __VA_ARGS__)
#define IMSG(...)               CVTASK_PRINTF(LVL_NORMAL,   __VA_ARGS__)
#define DMSG(...)               CVTASK_PRINTF(LVL_DEBUG,    __VA_ARGS__)
#define VMSG(...)               CVTASK_PRINTF(LVL_VERBOSE,  __VA_ARGS__)

/* special scheduler call that are not exported for regular cvtask */
extern void*    ambacv_p2v(uint64_t);
extern uint64_t ambacv_v2p(void *);
extern int      ambacv_cache_clean(void *ptr, uint64_t size);
extern uint32_t schdr_send_vpmsg(idsp_vis_picinfo_t *msg);

struct frame_info {
	char*                   frame_base;
	uint32_t                file_num;
	idsp_vis_picinfo_t      picinfo;
};

struct fov_info {
	uint32_t                fov_id;
	uint32_t                frame_max_length;
	uint32_t                frame_buffer_entries;
	char*                   frame_base;
	char*                   frame_wpos;
	struct frame_info       frame_info[MAX_FEEDER_FRAMES];
};

struct priv_data {
	/* the instance ID, from config[0] */
	uint32_t                instance_id;

	/* the sysflow index of finisher cvtask, used to send "finish" msg */
	uint32_t                finisher_id;

	/* control the frame iteration */
	uint32_t                frame_start;
	uint32_t                num_frames;
	uint32_t                frame_load;
	uint32_t                frame_send;
	uint32_t                cap_seq_no;

	/* input file format */
	const char*             frame_format;
	const char*             gyrod_format;

	/* information about echo FOV */
	uint32_t                num_fov;
	struct fov_info         fov[2];

	/* simulated IDSP interval in mill-second */
	uint32_t                idsp_interval;
	pthread_t               sender_thread;
	pthread_t               loader_thread;

	uint32_t                pyramid_valid;
	idsp_pyramid_t          pyramid;
	uint32_t                lanedetect_valid;
	idsp_ld_info_t          ld_info;

	uint32_t is_gyro_disabled; ///< 0 Gyro Enabled, 1 Gyro Disabled
};

void load_frame_set(struct priv_data *pd);
void send_frame_set(struct priv_data *pd);

#endif //__IDSPFEEDER_PRIVATE_H__
