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
#include "cvapi_file_feeder_interface.h"
#include "build_version.h"

#define TASKNAME TASKNAME_FILE_FEEDER

struct priv_database
{
	uint32_t    frame_width;
	uint32_t    frame_height;
	uint32_t    frame_pitch;
	uint32_t    frame_number;
	uint32_t    frame_number_max;
	uint32_t    source_frame_max;
	uint32_t    frame_offset;
	uint32_t    offset_config;    // byte offset for non-zero value
	char       *pFilename;
	uint32_t    finalstage_message_target;
	uint32_t    single_file_mode;
	void       *vpOutBufList[64];
};

static void load_binary(
	const char *name,
	char *dst,
	uint32_t offset,
	uint32_t width,
	uint32_t height,
	uint32_t pitch)
{
	FILE *ifp = fopen(name, "rb");
	uint32_t i, bytes;

	if (ifp == NULL) {
		AmbaPrint("FileFeeder can't load file %s!", name);
		assert(0);
	}

	if (fseek(ifp, offset, SEEK_SET) != 0) {
		AmbaPrint("FileFeeder fails to seek [%s] to 0x%X!", name);
		assert(0);
	}


	/* combine the read operation if width == pitch */
	if (width == pitch) {
		width *= height;
		height = 1;
		pitch = width;
	}

	for (i = 0; i < height; i++) {
		bytes = fread(dst, 1, width, ifp);
		if (bytes != width) {
			AmbaPrint("FileFeeder fails to read !!");
			AmbaPrint("     name: %s", name);
			AmbaPrint("   offset: %d", offset);
			AmbaPrint("    width: %d", width);
			AmbaPrint("   height: %d", height);
			AmbaPrint("    pitch: %d", pitch);
			assert(0);
		}
		dst += pitch;
	}
	fclose(ifp);
}

static uint32_t filefeeder_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *itf)
{
	char *ioname, label[32]="FEEDER_IONAME_###";

	label[14] = 0x30 + ((config[0] / 100) % 10);
	label[15] = 0x30 + ((config[0] /  10) % 10);
	label[16] = 0x30 + ((config[0]      ) % 10);

	if (cvtable_find(label, (const void**)&ioname, NULL) != 0) {
		AmbaPrint("cvtable can't find label %s !", label);
		assert(0);
	}

	memset(itf, 0, sizeof(*itf));
	itf->Instance_private_storage_needed  = sizeof(struct priv_database);
	itf->num_outputs = 1;
	itf->output[0].buffer_size = (config[3] == 0) ?
		config[1] : config[2] * config[3];
	strncpy(itf->output[0].io_name, ioname, NAME_MAX_LENGTH);

	CVTASK_PRINTF(LVL_NORMAL, "query cvtask [" TASKNAME "]");
	return CVTASK_ERR_OK;
}

static uint32_t  filefeeder_init(const cvtask_parameter_interface_t *env,
	const uint32_t *config)
{
	char *source, label[32] = "FEEDER_SOURCE_###";
	struct priv_database *pd;
	uint32_t  loop;

	label[14] = 0x30 + ((config[0] / 100) % 10);
	label[15] = 0x30 + ((config[0] /  10) % 10);
	label[16] = 0x30 + ((config[0]      ) % 10);
	if (cvtable_find(label, (const void**)&source, NULL) != 0) {
		AmbaPrint("cvtable can't fine label %s !", label);
		assert(0);
	}

	pd  = (struct priv_database *)env->vpInstance_private_storage;
	pd->frame_width       = (uint32_t)config[1];
	pd->frame_height      = (uint32_t)config[2];
	pd->frame_pitch       = (uint32_t)config[3];
	pd->frame_number      = 0;
	pd->frame_number_max  = (uint32_t)config[4];
	pd->source_frame_max  = (uint32_t)config[5];
	pd->frame_offset      = (uint32_t)config[6];
	pd->offset_config     = (uint32_t)config[7];
	pd->pFilename         = (char *)source;

	if (pd->source_frame_max == 0) {
		pd->source_frame_max = pd->frame_number_max;
	}

	if ((pd->frame_pitch != 0) && (pd->frame_pitch < pd->frame_width)) {
		pd->frame_pitch = pd->frame_width;
	}

	pd->finalstage_message_target = 0xFFFF;
	pd->single_file_mode          = 0;
	for (loop = 0; loop < 64; loop++)
	{
		pd->vpOutBufList[loop] = NULL;
	}

	cvtask_get_sysflow_index_list(
		NULL, NULL, NULL, "SIMULATION_FINISH_TASK",
		&pd->finalstage_message_target, 1, NULL
		);

	CVTASK_PRINTF(LVL_NORMAL, "init cvtask [" TASKNAME "]");
	return CVTASK_ERR_OK;
}

static uint32_t filefeeder_run(const cvtask_parameter_interface_t *env)
{
	char file_name[256];
	struct priv_database *pd;
	uint32_t file_num, offset, load_needed;

	pd = (struct priv_database *)env->vpInstance_private_storage;
	CVTASK_PRINTF(LVL_NORMAL, "[" TASKNAME "@ %d]", pd->frame_number);

	if (pd->frame_number >= pd->frame_number_max) {
		return CVTASK_ERR_OK;
	}

	file_num = (pd->frame_number + pd->frame_offset) / pd->source_frame_max;
	offset   = (pd->offset_config) ? pd->frame_offset :
		   (pd->frame_number + pd->frame_offset) % pd->source_frame_max;
	sprintf(file_name, pd->pFilename, file_num);
	if ((strcmp(file_name, pd->pFilename) == 0) && (pd->single_file_mode == 0) && (pd->offset_config == 0))
	{
		pd->single_file_mode = 1;
		CVTASK_PRINTF(LVL_NORMAL, "Using single file mode (reduces load bandwidth)\n", 0, 0, 0, 0, 0);
	} /* if ((strcmp(file_name, pd->pFilename) == 0) && (pd->single_file_mode == 0) && (pd->offset_config == 0)) */

	if (pd->single_file_mode == 0)
	{
		load_needed = 1;
	} /* if (pd->single_file_mode == 0) */
	else /* if (pd->single_file_mode != 0) */
	{
		uint32_t  done, loop;

		load_needed = 1;
		done        = 0;
		loop        = 0;

		while ((done == 0) && (loop < 64))
		{
			if (pd->vpOutBufList[loop] == NULL)
			{
				CVTASK_PRINTF(LVL_NORMAL, "[SFM] : Loading file into output buffer at 0x%llx\n", env->vpOutputBuffer[0], 0, 0, 0, 0);
				done        = 1;
				load_needed = 1;
				pd->vpOutBufList[loop] = env->vpOutputBuffer[0];
			} /* if (pd->vpOutBufList[loop] == NULL) */
			else if (pd->vpOutBufList[loop] == env->vpOutputBuffer[0])
			{
				CVTASK_PRINTF(LVL_NORMAL, "[SFM] : Reusing output buffer at 0x%llx\n", env->vpOutputBuffer[0], 0, 0, 0, 0);
				done        = 1;
				load_needed = 0;
			} /* if (pd->vpOutBufList[loop] == env->vpOutputBuffer[0]) */
			else /* if ((pd->vpOutBufList[loop] != NULL) && (pd->vpOutBufList[loop] != env->vpOutputBuffer[0])) */
			{
				loop ++;
			} /* if ((pd->vpOutBufList[loop] != NULL) && (pd->vpOutBufList[loop] != env->vpOutputBuffer[0])) */
		} /* while ((done == 0) && (loop < 64)) */
	} /* if (pd->single_file_mode != 0) */

	if (load_needed != 0)
	{
		if (pd->frame_pitch == 0) {
			offset = (pd->offset_config) ? (offset + pd->frame_width) :
				 (offset * pd->frame_width);
			load_binary(file_name,
					 env->vpOutputBuffer[0],
					 offset,
					 pd->frame_width,
					 1,
					 pd->frame_width);
		} else {
			offset = (pd->offset_config) ? (offset +
				 (pd->frame_width*pd->frame_height) * pd->frame_number):
				 (offset * (pd->frame_width * pd->frame_height));
			load_binary(file_name,
					env->vpOutputBuffer[0],
					offset,
					pd->frame_width,
					pd->frame_height,
					pd->frame_pitch);
		}
	} /* if (load_needed != 0) */

	pd->frame_number++;
	if (pd->frame_number >= pd->frame_number_max &&
	    pd->finalstage_message_target != 0xFFFF) {
		/* send messsage to finisher if we reach the end of sequence */
		cvtask_default_message_send(
			(void*)pd, 4,
			pd->finalstage_message_target,
			env->cvtask_frameset_id);
	}

	return CVTASK_ERR_OK;
}

static uint32_t filefeeder_get_info(
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
	.cvtask_query    = filefeeder_query,
	.cvtask_init     = filefeeder_init,
	.cvtask_run      = filefeeder_run,
	.cvtask_get_info = filefeeder_get_info,
};

uint32_t filefeeder_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (filefeeder_create);
