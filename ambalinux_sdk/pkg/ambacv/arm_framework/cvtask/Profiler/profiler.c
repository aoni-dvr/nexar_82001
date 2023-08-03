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

/*
 * The "profiler" collects all profile informations and save them for
 * offline analysis.
 *
 * config[0]: set to non-zero to disable profiling data output
 * config[1]: frame number from which to start profiling.
 * config[2]: frame number from which to stop profiling.
 *            zero means forever.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#include "cvapi_logger_interface.h"
#include "cvapi_profiler_interface.h"
#include "build_version.h"

#define FILENAME_LABEL          "PROFILER_FILE_NAME"
#define FILENAME_DEFAULT        "profile.prf"
#define AMBAPROF_MAX_NAME_LEN   128
#define SCRATCHPAD_SIZE         4096

#define ARM_TRACK_BASE          0
#define VISORC_TRACK_BASE       4
#define TOTAL_TRACK             8

#define CMSG(...)               CVTASK_PRINTF(LVL_CRITICAL, __VA_ARGS__)
#define IMSG(...)               CVTASK_PRINTF(LVL_NORMAL,   __VA_ARGS__)
#define DMSG(...)               CVTASK_PRINTF(LVL_DEBUG,    __VA_ARGS__)
#define VMSG(...)               CVTASK_PRINTF(LVL_VERBOSE,  __VA_ARGS__)

struct ambaprof_section {
	char       name[24];
	uint32_t   size;
	uint32_t   base;
};

struct ambaprof_header {
	uint32_t   version;
	uint32_t   magic_num;
	uint32_t   section_count;
	uint32_t   padding;
};

struct priv_data {
	FILE                    *profile_fp;
	uint32_t                track;
	uint32_t                last_write_index[TOTAL_TRACK];
	void                    *scratchpad;
	char                    *base;
	uint32_t                frame_start;
	uint32_t                frame_end;
};

static const cvtask_entry_t entry;
static FILE *g_fp = NULL;

static const cvtask_memory_interface_t interface = {
	.num_inputs                       = 1,
	.Instance_private_storage_needed  = sizeof(struct priv_data),
	.DRAM_temporary_scratchpad_needed = SCRATCHPAD_SIZE,
	.input[0].io_name                 = "SYSTEM_LOGINFO",
};

static void write_profile_header(struct priv_data *pd)
{
	struct ambaprof_header header;
	struct ambaprof_section section[2];
	uint32_t i, uuid, sysflow_lines;
	FILE *pfp = pd->profile_fp;
	uint32_t *array = (uint32_t*)pd->scratchpad;

	memset(&header, 0, sizeof(header));
	header.version = 0x1;
	header.magic_num = 0xBABEFACE;
	header.section_count = 2;
	fwrite(&header, sizeof(header), 1, pfp);

	cvtask_get_sysflow_index_list(NULL, NULL, NULL, NULL,
		array, SCRATCHPAD_SIZE >> 2, &sysflow_lines);
	strcpy(section[0].name, "cvtask names");
	section[0].size = sysflow_lines * AMBAPROF_MAX_NAME_LEN;
	section[0].base = sizeof(header) + sizeof(section);

	strcpy(section[1].name, "profile events");
	section[1].base = section[0].base + section[0].size;
	fwrite(section, sizeof(section), 1, pfp);

	for (i = 0; i < sysflow_lines; i++) {
		char name[AMBAPROF_MAX_NAME_LEN];
		const char *task;
		cvtask_get_name(array[i], NULL, NULL, NULL, &task, &uuid);
		sprintf(name, "%s(%d)", task, (int)uuid);
		fwrite(name, sizeof(name), 1, pfp);
	}
}

static void write_profile_data(struct priv_data *pd, cvlog_buffer_info_t *info)
{
	cvlog_perf_entry_t *entry;
	uint32_t wi_prev, wi_curr, count;

	entry = (cvlog_perf_entry_t*) (pd->base + info->buffer_addr_offset);
	wi_prev = pd->last_write_index[pd->track];
	wi_curr = info->write_index;

	/* output the tail part if there is a wrap-around */
	if (wi_prev > wi_curr) {
		count = info->buffer_size_in_entries - wi_prev;
		fwrite(&entry[wi_prev], sizeof(*entry), count, pd->profile_fp);
		wi_prev = 0;
	}

	/* output the head part */
	count = wi_curr - wi_prev;
	fwrite(&entry[wi_prev], sizeof(*entry), count, pd->profile_fp);

	/* update the write index */
	pd->last_write_index[pd->track] = wi_curr;
}

static uint32_t profiler_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	CMSG("query cvtask [Profiler]");
	*dst = interface;
	return CVTASK_ERR_OK;
}

static uint32_t profiler_init(const cvtask_parameter_interface_t *e,
	const uint32_t *config)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	const char *path;

	memset(pd, 0, sizeof(*pd));
	if (config[0] != 0) {
		CMSG("!!! Profiler is disabled by config[0]\n");
		return CVTASK_ERR_OK;
	}

	pd->scratchpad = e->vpDRAM_temporary_scratchpad;
	pd->frame_start = config[1];
	pd->frame_end = config[2];

	if (cvtable_find(FILENAME_LABEL, (const void**)&path, NULL) != 0) {
		path = FILENAME_DEFAULT;
	}

	if (g_fp != NULL) {
		fclose(g_fp);
		g_fp = NULL;
	}

	pd->profile_fp = fopen(path, "wb");
	if (pd->profile_fp == NULL) {
		AmbaPrint("!!! Can not open profiler output file %s", path);
		exit(-1);
	} else {
		g_fp = pd->profile_fp;
		write_profile_header(pd);
	}

	CMSG("init cvtask [Profiler]");
	return CVTASK_ERR_OK;
}

static uint32_t profiler_run(const cvtask_parameter_interface_t *e)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	cvlogger_state_t *state;
	int i;

	DMSG("[Profiler @ %d]", e->cvtask_frameset_id);
	if (pd->profile_fp == 0 || pd->frame_start > e->cvtask_frameset_id)
		return CVTASK_ERR_OK;

	pd->base = e->vpInputBuffer[0];
	state = (cvlogger_state_t *)pd->base;

	/* output cortex perf data */
	for (i = 0; i < state->num_arm; i++) {
		pd->track = i + ARM_TRACK_BASE;
		write_profile_data(pd, &state->arm[i].perf_log);
	}

	/* output visorc perf data */
	for (i = 0; i < state->num_visorc; i++) {
		pd->track = i + VISORC_TRACK_BASE;
		write_profile_data(pd, &state->visorc[i].perf_log);
	}

	if (pd->frame_end != 0 && pd->frame_end == e->cvtask_frameset_id) {
		CMSG("Done profiling at frame %d, Exit.\n", (int)pd->frame_end);
		fclose(pd->profile_fp);
		g_fp = NULL;
		return CVTASK_ERR_GENERAL;
	}

	return CVTASK_ERR_OK;
}

static uint32_t profiler_get_info(
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
	.cvtask_name     = TASKNAME_PROFILER,
	.cvtask_type     = CVTASK_TYPE_ARM,
	.cvtask_query    = profiler_query,
	.cvtask_init     = profiler_init,
	.cvtask_run      = profiler_run,
	.cvtask_get_info = profiler_get_info,
};

uint32_t profiler_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (profiler_create);
