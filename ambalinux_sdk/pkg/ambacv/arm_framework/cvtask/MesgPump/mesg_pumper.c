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

#include <stdio.h>
#include <string.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvapi_message_pump_interface.h"
#include "private.h"
#include "build_version.h"

static const cvtask_entry_t entry;

static void send_mesg(struct priv_data *db, int frame_num)
{
	int i;
	struct pumper_mesg *mesg;

	for (i = 0; i < db->pmesg_count; i++) {
		mesg = &(db->pmesg[i]);
		if (mesg->frame_send == frame_num) {
			FILE *fp = db->binary_fp[mesg->binary_id];
			fseek(fp, mesg->payload_off, SEEK_SET);
			fread(db->mesg_buffer, 1, mesg->payload_len, fp);
			cvtask_default_message_send(
				db->mesg_buffer, mesg->payload_len,
				db->sysflow_id[mesg->cvtask_id],
				mesg->frame_deliver);
			AmbaPrint("[PUMP@%02d]-->%s(%d), len %d",
				frame_num,
				db->sysflow_name[mesg->cvtask_id],
				db->uuid[mesg->cvtask_id],
				mesg->payload_len);
		}
	}
}

static uint32_t mesg_pump_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	CVTASK_PRINTF(LVL_VERBOSE, "query cvtask [MesgPump]");

	memset(dst, 0, sizeof(*dst));
	dst->Instance_private_storage_needed = sizeof(struct priv_data);
	dst->DRAM_temporary_scratchpad_needed = CVTASK_MSG_MAX_LENGTH;

	dst->num_inputs = 1;
	dst->input[0].dependency_only = 1;
	strcpy(dst->input[0].io_name, "DUMMY");

	return CVTASK_ERR_OK;
}

static uint32_t mesg_pump_init(const cvtask_parameter_interface_t *env,
	const uint32_t *config)
{
	struct priv_data *db;
	const char *name;
	FILE *fp;
	int i, size;

	CVTASK_PRINTF(LVL_VERBOSE, "init cvtask [MesgPump]");
	db = (struct priv_data*)env->vpInstance_private_storage;
	memset(db, 0, sizeof(*db));
	for (i = 0; i < MAX_CVTASK_COUNT; i++) {
		db->sysflow_id[i] = -1;
	}
	db->mesg_buffer = (char*)env->vpDRAM_temporary_scratchpad;

	/* get mpx file name specificed by MPX_FILE_NAME label */
	if(cvtable_find("MPX_FILE_NAME", (const void**)&name, NULL)) {
		name = "default.mpx";
	}

	fp = fopen(name, "rb");
	if (fp == NULL) {
		AmbaPrint("Can't open MesgPump file %s!", name);
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (size > sizeof(db->mpx_buff)) {
		AmbaPrint("MPX file(%d) is too big!", size);
		exit(-1);
	}
	fread(db->mpx_buff, 1, size, fp);
	fclose(fp);
	parse_message_manifest(db, db->mpx_buff, size);

	send_mesg(db, -1);
	return CVTASK_ERR_OK;
}

static uint32_t mesg_pump_run(const cvtask_parameter_interface_t *env)
{
	struct priv_data *db;

	db = (struct priv_data*)env->vpInstance_private_storage;
	db->mesg_buffer = (char*)env->vpDRAM_temporary_scratchpad;
	send_mesg(db, env->cvtask_frameset_id);

	return CVTASK_ERR_OK;
}

static uint32_t mesg_pump_get_info(const cvtask_parameter_interface_t *env,
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
	.cvtask_name     = TASKNAME_MESSAGE_PUMP,
	.cvtask_type     = CVTASK_TYPE_ARM,
	.cvtask_query    = mesg_pump_query,
	.cvtask_init     = mesg_pump_init,
	.cvtask_get_info = mesg_pump_get_info,
	.cvtask_run      = mesg_pump_run,
};

static uint32_t MesgPump_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (MesgPump_create);
