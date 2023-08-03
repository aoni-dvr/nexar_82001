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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#include "cvapi_sink_interface.h"
#include "build_version.h"

struct sink_context {
        uint32_t frame_num;
        char     path_pattern[256];
        uint8_t  store_output;
        uint8_t  store_mode;
};

static const cvtask_entry_t entry;

static uint32_t my_atoi(const char *str)
{
        uint32_t val = 0;
        char c;

        while ((c = *str++) != 0) {
                if (c >= '0' && c <= '9') {
                        val = (val*10) + (c - '0');
                }
        }

        return val;
}

static void dump_binary(char *path, char *buf, int size)
{
	FILE *fp = fopen(path, "wb");
	int bytes_written;

	if (fp == NULL) {
		AmbaPrint("can't open dump file %s, skip", path);
		return;
	}

	bytes_written = fwrite(buf, 1, size, fp);
	if (bytes_written != size) {
		AmbaPrint("Error while dumpping file %s, skip", path);
	}

	fclose(fp);
}

static uint32_t query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	char label[32] = SINK_LABEL_IONAME;
	const char *ioname;
	uint32_t str_size, ret;

	CVTASK_PRINTF(LVL_VERBOSE, "query cvtask [SINK]");

	memset(dst, 0, sizeof(*dst));
	dst->num_outputs = 1;
	strcpy(dst->output[0].io_name, "DUMMY");
	dst->Instance_private_storage_needed = sizeof(struct sink_context);

	label[12] = '0' + ((config[0] / 100) % 10);
	label[13] = '0' + ((config[0] /  10) % 10);
	label[14] = '0' + ((config[0]      ) % 10);
	ret = cvtable_find(label, (const void**)&ioname, &str_size);
	if (ret != CVTASK_ERR_OK) {
		AmbaPrint("can't find label %s", label);
		abort();
	}
	dst->num_inputs = 1;
	memcpy(dst->input[0].io_name, ioname, str_size);

	return CVTASK_ERR_OK;
}

static uint32_t init(const cvtask_parameter_interface_t *env,
	const uint32_t *config)
{
	char label[32], *path;
	const char *buf;
	uint32_t str_size, ret;
	struct sink_context *ctx;

	CVTASK_PRINTF(LVL_VERBOSE, "init cvtask [SINK]");
	ctx = (struct sink_context*)env->vpInstance_private_storage;

        /* get topdir, if user provides it */
        path = ctx->path_pattern;
        ret = cvtable_find(SINK_LABEL_TOPDIR, (const void **)&buf, NULL);
        if (ret == CVTASK_ERR_OK) {
                str_size = strlen(buf);
                memcpy(path, buf, str_size);
                path += str_size;
                /* add trailing '/' if it not provided */
                if (*(path-1) != '/') {
                        *path++ = '/';
                }
        }

        /* get starting frame num */
        ret = cvtable_find(SINK_LABEL_FRAME_BASE, (const void **)&buf, NULL);
        if (ret == CVTASK_ERR_OK) {
                ctx->frame_num = my_atoi(buf);
        }else{
            /* get starting frame num instance when frame base not found */
            strcpy(label, "SINK_FRAME_BASE_xxx");
            label[16] = '0' + ((config[0] / 100) % 10);
            label[17] = '0' + ((config[0] /  10) % 10);
            label[18] = '0' + ((config[0]      ) % 10);
            ret = cvtable_find(label, (const void **)&buf, NULL);
            if(ret == CVTASK_ERR_OK) {
                ctx->frame_num = my_atoi(buf);
            }else{
                ctx->frame_num = 0;
            }
        }

        strcpy(label, SINK_LABEL_OUTPUT);
        label[12] = '0' + ((config[0] / 100) % 10);
        label[13] = '0' + ((config[0] /  10) % 10);
        label[14] = '0' + ((config[0]      ) % 10);
	ret = cvtable_find(label, (const void **)&buf, &str_size);
	if (ret != CVTASK_ERR_OK) {
		AmbaPrint("can't find label %s", label);
		abort();
	}
	strcpy(path, buf);

	ret = cvtable_find("SINK_STORE_OUTPUT", (const void **)&buf, &str_size);
	if (ret == CVTASK_ERR_OK) {
		uint32_t op_conf = strtoul(buf, NULL, 0);
		ctx->store_output = (op_conf != 0);
		if (ctx->store_output == 0) {
			AmbaPrint("[Sink] [WARN] NOT Storing DAG output\n");
		}
	} else {
		ctx->store_output = 1;
	}
        ctx->store_mode = config[1];

	return CVTASK_ERR_OK;
}

static uint32_t proc_msg(const cvtask_parameter_interface_t *env)
{
	return CVTASK_ERR_OK;
}

static uint32_t run(const cvtask_parameter_interface_t *env)
{
	char path[256];
	struct sink_context *ctx;

	ctx = (struct sink_context*)env->vpInstance_private_storage;
	if (strlen(ctx->path_pattern) + 16 > sizeof(path)) {
		AmbaPrint("[Sink]: path pattern %s is too long!\n");
		abort();
	}

	if (ctx->store_output) {
                int size;
                sprintf(path, ctx->path_pattern, ctx->frame_num++);
                size = (ctx->store_mode == 0) ? cvtask_get_input_size(0) :
                        strlen(env->vpInputBuffer[0]);
                dump_binary(path, env->vpInputBuffer[0], size); 
	}
	return CVTASK_ERR_OK;
}

static uint32_t get_info(const cvtask_parameter_interface_t *env,
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
	.cvtask_name     = TASKNAME_SINK,
	.cvtask_type     = CVTASK_TYPE_ARM,
	.cvtask_query    = query,
	.cvtask_init     = init,
	.cvtask_get_info = get_info,
	.cvtask_process_messages = proc_msg,
	.cvtask_run      = run,
};

uint32_t sinker_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (sinker_create);
