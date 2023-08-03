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
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#include "build_version.h"

#define TASKNAME "IOFEEDER"

struct priv_database
{
	const char *pFilename[CVTASK_MAX_OUTPUTS];
	uint32_t    max_size[CVTASK_MAX_OUTPUTS];
	uint32_t    num_outputs;
	uint32_t    frame_start;
	uint32_t    frame_end;
	uint32_t    frame_curr;
	uint32_t    finisher_index;
};

static void load_output(struct priv_database *pd, int output_id, void *buf)
{
	char filename[128];
	int size;
	FILE *ifp;

	/* open the data file */
	sprintf(filename, pd->pFilename[output_id], pd->frame_curr);
	ifp = fopen(filename, "rb");
	if (ifp == NULL) {
		AmbaPrint(TASKNAME " can't open file %s!", filename);
		assert(0);
	}

	/* check if the file size is too big */
	fseek(ifp, 0, SEEK_END);
	size = ftell(ifp);
	if (size > pd->max_size[output_id]) {
		AmbaPrint("File %s is too big! ", filename);
		AmbaPrint("please check IOFEED_OUTPUT%d_MAXSIZE\n", output_id);
		assert(0);
	}

	/* read data into target buffer */
	fseek(ifp, 0, SEEK_SET);
	if (size != fread(buf, 1, size, ifp)) {
		AmbaPrint("Faild to read %d\n", filename);
		assert(0);
	}

	fclose(ifp);
}

static uint32_t iofeeder_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *itf)
{
	char *value, label[32];
	int i;

	/* zero-out interface data structure */
	memset(itf, 0, sizeof(*itf));
	itf->Instance_private_storage_needed  = sizeof(struct priv_database);

	/* get the number of outputs */
	sprintf(label, "IOFEEDER_NUMBER_OF_OUTPUTS");
	if (cvtable_find(label, (const void**)&value, NULL) != 0) {
		AmbaPrint("cvtable can't find label %s !", label);
		assert(0);
	}
	itf->num_outputs = atoi(value);

	/* for each output, get its name and max size */
	for (i = 0; i < itf->num_outputs; i++) {
		sprintf(label, "IOFEEDER_OUTPUT%d_MAXSIZE", i);
		if (cvtable_find(label, (const void**)&value, NULL) != 0) {
			AmbaPrint("cvtable can't find label %s !", label);
			assert(0);
		}
		itf->output[i].buffer_size = atoi(value);

		sprintf(label, "IOFEEDER_OUTPUT%d_IONAME", i);
		if (cvtable_find(label, (const void**)&value, NULL) != 0) {
			AmbaPrint("cvtable can't find label %s !", label);
			assert(0);
		}
		strncpy(itf->output[i].io_name, value, NAME_MAX_LENGTH);
	}

	CVTASK_PRINTF(LVL_NORMAL, "query cvtask [" TASKNAME "]");
	return CVTASK_ERR_OK;
}

static uint32_t  iofeeder_init(const cvtask_parameter_interface_t *env,
	const uint32_t *config)
{
	struct priv_database *pd;
	char *value, label[32];
	int i;

	pd = (struct priv_database *)env->vpInstance_private_storage;

	/* get the number of outputs */
	sprintf(label, "IOFEEDER_NUMBER_OF_OUTPUTS");
	if (cvtable_find(label, (const void**)&value, NULL) != 0) {
		AmbaPrint("cvtable can't find label %s !", label);
		assert(0);
	}
	pd->num_outputs = atoi(value);

	/* get the start-frame number */
	sprintf(label, "IOFEEDER_FRAME_START");
	if (cvtable_find(label, (const void**)&value, NULL) != 0) {
		AmbaPrint("cvtable can't find label %s !", label);
		assert(0);
	}
	pd->frame_start = pd->frame_curr = atoi(value);

	/* get the end-frame number */
	sprintf(label, "IOFEEDER_FRAME_END");
	if (cvtable_find(label, (const void**)&value, NULL) != 0) {
		AmbaPrint("cvtable can't find label %s !", label);
		assert(0);
	}
	pd->frame_end = atoi(value);

	/* for each output, get its filename and max size */
	for (i = 0; i < pd->num_outputs; i++) {
		sprintf(label, "IOFEEDER_OUTPUT%d_FILENAME", i);
		if (cvtable_find(label, (const void**)&value, NULL) != 0) {
			AmbaPrint("cvtable can't find label %s !", label);
			assert(0);
		}
		pd->pFilename[i] = value;

		sprintf(label, "IOFEEDER_OUTPUT%d_MAXSIZE", i);
		if (cvtable_find(label, (const void**)&value, NULL) != 0) {
			AmbaPrint("cvtable can't find label %s !", label);
			assert(0);
		}
		pd->max_size[i] = atoi(value);
	}

	pd->finisher_index = 0xFFFF;
	cvtask_get_sysflow_index_list(
		NULL, NULL, NULL, "SIMULATION_FINISH_TASK",
		&pd->finisher_index, 1, NULL);
	if (pd->finisher_index == 0xFFFF) {
		AmbaPrint(TASKNAME " cannot find finisher task !");
		assert(0);
	}

	CVTASK_PRINTF(LVL_NORMAL, "init cvtask [" TASKNAME "]");
	return CVTASK_ERR_OK;
}

static uint32_t iofeeder_run(const cvtask_parameter_interface_t *env)
{
	struct priv_database *pd;
	int i;

	pd = (struct priv_database *)env->vpInstance_private_storage;
	CVTASK_PRINTF(LVL_NORMAL, "[" TASKNAME ": frame %d]", pd->frame_curr);

	for (i = 0; i < env->cvtask_num_outputs; i++) {
		load_output(pd,  i, env->vpOutputBuffer[i]);
	}

	pd->frame_curr++;
	if (pd->frame_curr == pd->frame_end) {
		/* send messsage to finisher if we reach the end of sequence */
		cvtask_default_message_send(
			(void*)pd, 4,
			pd->finisher_index,
			env->cvtask_frameset_id);
	}

	return CVTASK_ERR_OK;
}

static uint32_t iofeeder_get_info(
	const cvtask_parameter_interface_t *env,
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
	.cvtask_name     = TASKNAME,
	.cvtask_type     = CVTASK_TYPE_ARM_FEEDER,
	.cvtask_query    = iofeeder_query,
	.cvtask_init     = iofeeder_init,
	.cvtask_run      = iofeeder_run,
	.cvtask_get_info = iofeeder_get_info,
};

uint32_t iofeeder_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (iofeeder_create);
