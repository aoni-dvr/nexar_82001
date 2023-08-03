/*
 * Copyright (c) 2017-2017 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign copyrights.
 * This Software is also the confidential and proprietary information of
 * Ambarella, Inc. and its licensors.  You may not use, reproduce, disclose,
 * distribute, modify, or otherwise prepare derivative works of this Software
 * or any portion thereof except pursuant to a signed license agreement or
 * nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/timerfd.h>
#include "cvapi_idsp_feeder_interface.h"
#include "private.h"
#include "build_version.h"

#define TASKNAME                TASKNAME_IDSP_FEEDER
#define MAX_FEEDER_INSTANCES    32
#define LOCK_FEEDER()           pthread_mutex_lock(&mfeeder)
#define UNLOCK_FEEDER()         pthread_mutex_unlock(&mfeeder)
//#define DUMP_PICINFO

#define dump_imginfo(L, C, O)                                                  \
	DMSG("          luma: %4d %4d %4d %X", L.img_width, L.img_height,      \
	     L.buffer_pitch, L.buffer_addr - base);                            \
	DMSG("        chroma: %4d %4d %4d %X", C.img_width, C.img_height,      \
	     C.buffer_pitch, C.buffer_addr - base);                            \
	DMSG("        offset: %4d %4d 0x%08x 0x%08x",                          \
	     O.x_offset , O.y_offset, L.buffer_addr, C.buffer_addr);

struct feeder_db {
	int               count;
	int               done_first_frame;
	struct priv_data* pd[MAX_FEEDER_INSTANCES];
};

static pthread_mutex_t      mfeeder = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t       cfeeder = PTHREAD_COND_INITIALIZER;
static struct feeder_db     fdb;

#ifdef DUMP_PICINFO
static void dump_picinfo(idsp_vis_picinfo_t *info, uint32_t base)
{
	int i;

	DMSG("========picinfo, channel %d========", info->header.channel_id);
	DMSG("    seq_no: %d", info->header.cap_seq_no);
	DMSG("      mask: %X", info->header.ln_det_hier_mask);

	if (info->header.ln_det_hier_mask & (1<<MAX_HALF_OCTAVES)) {
		DMSG("    lane detection layer:");
		dump_imginfo(info->luma_lane_det_out,
			     info->chroma_lane_det_out,
			     info->lane_det_out_offset);
	}

	for (i = 0; i < MAX_HALF_OCTAVES; i++) {
		if (0 == (info->header.ln_det_hier_mask & (1<<i))) {
			continue;
		}
		DMSG("    scale[%d]:", i);
		dump_imginfo(info->luma_hier_outs[i],
			     info->chroma_hier_outs[i],
			     info->hier_out_offsets[i]);
	}
}
#else
#define dump_picinfo(info, base)
#endif

static void process_init_msg(const cvtask_parameter_interface_t *e, void *_msg)
{
	idspfeeder_idsp_init_msg_t *msg;
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	struct fov_info  *fov;
	vp_frame_list_descriptor_t *dsptr;
	uint32_t i, j, table_size, *table;

	msg = (idspfeeder_idsp_init_msg_t*)_msg;
	IMSG("=========[IDSP_FEEDER_IDSP_INIT]============");
	IMSG("       vp queue cb: 0x%08X", msg->vp_msg_queue_cb_addr);
	IMSG("       async queue: 0x%08X, %d",
		      msg->async_cmd_queue_addr, msg->async_cmd_queue_size);

	pd->num_fov = msg->num_fov;
	for (i = 0; i < pd->num_fov; i++) {
		fov = &(pd->fov[i]);
		dsptr = &(msg->frame_list_descriptor[i]);
		table_size = dsptr->frame_list_table_size;
		if (table_size > MAX_FEEDER_FRAMES) {
			printf("FEEDER: table size %d too big\n", table_size);
			exit(-1);
		}

		fov->fov_id = dsptr->fov_id;
		fov->frame_buffer_entries = table_size;
		fov->frame_max_length =
			dsptr->frame_buffer_length +
			dsptr->ld_buffer_length +
			dsptr->meta_buffer_length;

		IMSG("            fov[%d]: %d x 0x%X",
			      fov->fov_id, table_size, fov->frame_max_length);

		table = ambacv_p2v(dsptr->frame_list_table_addr);
		for (j = 0; j < table_size; j++) {
			fov->frame_info[j].frame_base = ambacv_p2v(table[j]);
			IMSG("                  : 0x%08X", table[j]);
		}
	}
}

static void process_stream_msg(const cvtask_parameter_interface_t *e,
			       void *_msg)
{
	idspfeeder_streaming_ctrl_msg_t *msg;

	msg = (idspfeeder_streaming_ctrl_msg_t *)_msg;
	VMSG("=========[IDSP_FEEDER_STREAM_CTL %d]=======", msg->ctrl_op);
}

static void process_pyramid_config_msg(const cvtask_parameter_interface_t *e,
				       void *_msg)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	idsp_pyramid_config_message_t *pMsg;

	pMsg = (idsp_pyramid_config_message_t *)_msg;
	pd->pyramid_valid = 1;
	pd->pyramid        = pMsg->pyramid;
}

static void process_lanedet_config_msg(const cvtask_parameter_interface_t *e,
					   void *_msg)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	idsp_lane_detect_config_message_t *pMsg;

	pMsg = (idsp_lane_detect_config_message_t *)_msg;
	pd->lanedetect_valid = 1;
	pd->ld_info = pMsg->ld_info;
}

static void* sender_entry(void *arg)
{
	struct priv_data *pd = (struct priv_data*)arg;
	int i, tfd;
	struct itimerspec interval;
	uint64_t exp;

	tfd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (tfd == -1) {
		printf("Can not create timer\n");
		exit(-1);
	}

	interval.it_interval.tv_sec  = pd->idsp_interval / 1000;
	interval.it_interval.tv_nsec = (pd->idsp_interval % 1000) * 1000000;
	interval.it_value.tv_sec     = 0;
	interval.it_value.tv_nsec    = 1;
	if (timerfd_settime(tfd, 0, &interval, NULL) < 0) {
		printf("Can not start timer, error is\n");
		exit(-1);
	}

	while(1) {
		read(tfd, &exp, sizeof(exp));
		for (i = 0; i < fdb.count; i++) {
			send_frame_set(fdb.pd[i]);
		}
	}

	return NULL;
}

static void* loader_entry(void *arg)
{
	struct priv_data *pd = (struct priv_data*)arg;
	int tfd;
	struct itimerspec interval;
	uint64_t exp;

	tfd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (tfd == -1) {
		printf("Can not create timer\n");
		exit(-1);
	}

	interval.it_interval.tv_sec  = pd->idsp_interval / 1000;
	interval.it_interval.tv_nsec = (pd->idsp_interval % 1000) * 1000000;
	interval.it_value.tv_sec     = interval.it_interval.tv_sec;
	interval.it_value.tv_nsec    = interval.it_interval.tv_nsec;

	/* load first frame set */
	load_frame_set(pd);

	/* wait util all instance finishes loading of first frame */
	LOCK_FEEDER();
	if (++fdb.done_first_frame < fdb.count) {
		pthread_cond_wait(&cfeeder, &mfeeder);
	} else {
		pthread_create(&pd->sender_thread, 0, sender_entry, pd);
		pthread_cond_broadcast(&cfeeder);
	}
	UNLOCK_FEEDER();
	//printf("loader%d wait done.\n", pd->instance_id);

	if (timerfd_settime(tfd, 0, &interval, NULL) < 0) {
		printf("Can not start timer, error is\n");
		exit(-1);
	}

	while(1) {
		// blocks here if loading completes within one frame time
		read(tfd, &exp, sizeof(exp));
		// loads one frame set
		load_frame_set(pd);
	}

	return NULL;
}

static uint32_t idspfeeder_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	memset(dst, 0, sizeof(*dst));
	dst->Instance_private_storage_needed  = sizeof(struct priv_data);
	IMSG("query cvtask [" TASKNAME "] %d", iid);
	return CVTASK_ERR_OK;
}

static uint32_t idspfeeder_init(const cvtask_parameter_interface_t *e,
	const uint32_t *config)
{
	struct priv_data *pd;
	const void *value;
	char key[128];
	uint32_t num_found;

	// Reset Private Data
	pd = (struct priv_data*)e->vpInstance_private_storage;
	memset(pd, 0, sizeof(*pd));
	pd->instance_id = config[0];
	pd->idsp_interval = config[1];
	pd->is_gyro_disabled = config[7];

	/* get finisher ID */
	cvtask_get_sysflow_index_list(NULL, NULL, NULL,
		"SIMULATION_FINISH_TASK", &pd->finisher_id, 1, &num_found);
	if (pd->idsp_interval != 0 && num_found > 0) {
		AmbaPrint("IDspFeeder is in timer-mode.\n");
		AmbaPrint("Please remove SIMULATION_FINISH_TASK.\n");
		return CVTASK_ERR_GENERAL;
	} else if (pd->idsp_interval == 0 && num_found != 1) {
		AmbaPrint("Sysflow must have exactly 1 SIMULATION_FINISH_TASK\n");
		return CVTASK_ERR_GENERAL;
	}

	/* get frame-data file name pattern */
	sprintf(key, "IdspFeeder%d_input", config[0]);
	if (cvtable_find(key, &value, NULL) != CVTASK_ERR_OK) {
		EMSG("Can't find key IdspFeeder%d_input", config[0]);
		return CVTASK_ERR_GENERAL;
	}
	pd->frame_format = (const char*)value;

	/* get input frame start number, default is 0 */
	sprintf(key, "IdspFeeder%d_frame_start", config[0]);
	if (cvtable_find(key, &value, NULL) == CVTASK_ERR_OK) {
		pd->frame_start = atoi(value);
	}

	/* get input frame end number */
	sprintf(key, "IdspFeeder%d_frame_end", config[0]);
	if (cvtable_find(key, &value, NULL) != CVTASK_ERR_OK) {
		EMSG("Can't find key IdspFeeder%d_frame_end", config[0]);
		return CVTASK_ERR_GENERAL;
	}
	pd->num_frames = atoi(value) - pd->frame_start;

	/* get gyro-data file name pattern */
	sprintf(key, "IMU_SOURCE_000");
	if (cvtable_find(key, &value, NULL) != CVTASK_ERR_OK) {
		EMSG("Can't find IMU_SOURCE_000, gyro data won't be loaded");
	}
	pd->gyrod_format = (const char*)value;

	/* fill in the global feeder info */
	LOCK_FEEDER();
	if (fdb.count >= MAX_FEEDER_INSTANCES) {
		printf("Too many feeder instances, max is %d\n",
		       MAX_FEEDER_INSTANCES);
	}
	fdb.pd[fdb.count] = pd;
	if (fdb.pd[0]->idsp_interval != pd->idsp_interval) {
		printf("IdspFeeder%d has different interval!", config[0]);
	}
	fdb.count++;
	UNLOCK_FEEDER();

	IMSG("init cvtask [" TASKNAME "%d], %d %d", config[0],
	     pd->frame_start, pd->num_frames);

	return CVTASK_ERR_OK;
}

static uint32_t idspfeeder_process_msg(const cvtask_parameter_interface_t *e)
{
	int i;

	for (i = 0; i < e->cvtask_num_messages; i++) {
		uint32_t *msg = (uint32_t*)e->vpMessagePayload[i];
		switch (*msg) {
		case IDSP_FEEDER_IDSP_INIT:
			process_init_msg(e, msg);
			break;
		case IDSP_FEEDER_STREAM_CTL:
			process_stream_msg(e, msg);
			break;
		case IDSP_MESSAGE_PYRAMID_CONFIG:
			process_pyramid_config_msg(e, msg);
			break;
		case IDSP_MESSAGE_LANE_DETECT_CONFIG:
			process_lanedet_config_msg(e, msg);
			break;
		default:
			EMSG("skip unkown idspfeeder msg 0x%X !!!", *msg);
			break;
		}
	}

	return CVTASK_ERR_OK;
}

static uint32_t idspfeeder_run(const cvtask_parameter_interface_t *e)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;

	idspfeeder_process_msg(e);

	/* timer-mode: just start loader thread */
	if (pd->idsp_interval != 0) {
		if (e->cvtask_frameset_id == 0) {
			pthread_create(&pd->loader_thread, 0, loader_entry, pd);
		} else {
			IMSG("Feeder skips cvtask_run in timer mode\n");
		}
		return CVTASK_ERR_OK;
	}

	/* on-demand mode: load/send the frame directly */
	if (pd->frame_load < pd->num_frames) {
		load_frame_set(pd);
		send_frame_set(pd);
	}

	if (pd->frame_load >= pd->num_frames) {
		cvtask_default_message_send(
			(void*)pd, 4, pd->finisher_id, e->cvtask_frameset_id);
	}

	IMSG("[" TASKNAME "%d @ frame %d] done",
	     pd->instance_id, e->cvtask_frameset_id);
	return CVTASK_ERR_OK;
}

static uint32_t idspfeeder_get_info(const cvtask_parameter_interface_t *env,
	uint32_t info_id, void *info_data)
{
	uint32_t err;
	const char **str;

	switch (info_id) {
	case CVTASK_INFO_ID_GET_SELF_INFO:
		str = (const char **)info_data;
		*str = LIBRARY_VERSION;
		err = CVTASK_ERR_OK;
		break;
	default:
		err = CVTASK_ERR_UNIMPLEMENTED;
	}

	return err;
}

static const cvtask_entry_t entry = {
	.cvtask_name             = TASKNAME,
	.cvtask_type             = CVTASK_TYPE_ARM_FEEDER,
	.cvtask_query            = idspfeeder_query,
	.cvtask_init             = idspfeeder_init,
	.cvtask_get_info         = idspfeeder_get_info,
	.cvtask_run              = idspfeeder_run,
	.cvtask_process_messages = idspfeeder_process_msg,
};

uint32_t idspfeeder_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (idspfeeder_create);
