/*
 * This is a simple example of cvtask implementaion.
 * The "adder" adds two input buffers into one output buffer.
 * All buffers are of size MEM_BUF_SIZE.
 */
#include <time.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "build_version.h"

#define SCRATCHPAD_SIZE         8
#define CVTASK_NAME             "ARM_ADDER"

struct priv_database {
	int data_width;
	int data_height;
};


static const cvtask_entry_t entry;

static cvtask_memory_interface_t interface = {
	.Instance_private_storage_needed = sizeof(struct priv_database),
	.DRAM_temporary_scratchpad_needed = SCRATCHPAD_SIZE,

	.num_inputs = 1,
	.input[0].io_name  = "ARM_ADDER_INPUT",

	.num_outputs = 1,
	.output[0].io_name = "ARM_ADDER_OUTPUT",

	.num_custom_msgpool = 1,
	.custom_msgpool_message_num[0] = 2,
	.custom_msgpool_message_size[0] = 64,
};

static uint32_t adder_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	CVTASK_PRINTF(LVL_NORMAL, "query cvtask [" CVTASK_NAME "]");

	interface.input[0].buffer_size = config[0] * config[1];
	interface.output[0].buffer_size = 4*config[1];
	*dst = interface;

	// test if we can do cvtable_find
	{
		const char *ptr;

		if (cvtable_find("ADDER_MNFT", (const void**)&ptr, NULL) != 0){
			AmbaPrint("cvtable can't find ADDER_MNFT!\n");
			assert(0);
		}
	}

	return CVTASK_ERR_OK;
}

static uint32_t adder_init(const cvtask_parameter_interface_t *env,
	const uint32_t *config)
{
	struct priv_database *db;

	db = (struct priv_database*)env->vpInstance_private_storage;
	db->data_width = config[0];
	db->data_height = config[1];

	// test default message sending at init stage
	{
		char message[32];

		sprintf(message, "Hello from init: default");
		cvtask_default_message_send(
			message, strlen(message)+1,
			env->cvtask_sysflow_index, 0);
	}

	// test custom message sending at init stage
	{
		char *message;
		uint32_t max_size;

		cvtask_message_create(0, (void**)&message, &max_size);
		strcpy(message, "Hello from init: custom");
		cvtask_message_send(
			message, strlen(message) + 1,
			env->cvtask_sysflow_index, 0);
	}

	CVTASK_PRINTF(LVL_NORMAL, "init cvtask [" CVTASK_NAME "] %d %d",
		db->data_width, db->data_height);
	return CVTASK_ERR_OK;
}

static uint32_t adder_process_messages(const cvtask_parameter_interface_t *env)
{
	int i;

	// log the message we got
	for (i = 0; i < env->cvtask_num_messages; i++) {
		AmbaPrint("[" CVTASK_NAME "@%02d] got message: %s",
			env->cvtask_frameset_id,
			(char*)env->vpMessagePayload[i]);
	}

	return CVTASK_ERR_OK;
}

static uint32_t adder_run(const cvtask_parameter_interface_t *env)
{
	const unsigned char *src;
	int *dst;
	int i, j, total;
	struct priv_database *db;

	db = (struct priv_database*)env->vpInstance_private_storage;
	src = env->vpInputBuffer[0];
	dst = (int*)env->vpOutputBuffer[0];

	// test if we can use scracthpad
	{
		char *pad = (char *)env->vpDRAM_temporary_scratchpad;
		for (i = 0; i < SCRATCHPAD_SIZE; i++) {
			pad[i] = i;
		}
	}

	// sum each line of input data
	for (i = 0; i < db->data_height; i++) {
		for (j = total = 0; j < db->data_width; j++) {
			total += *src++;
		}
		*dst++ = total;
	}

	// do some number crunching for performance comparison on Linux build
	{
		uint32_t k, start, stop, sum;

		start = cvtask_get_timestamp();
		for (k = sum = 0; k < 1024; k++) {
			src = env->vpInputBuffer[0];
			for (i = 0;  i < db->data_height; i++) {
				for (j = 0; j < db->data_width; j++) {
					sum += (*src++);
				}
			}
		}
		stop = cvtask_get_timestamp();
		AmbaPrint("[ARM_ADDDER] number crunching, sum=%u, tick=%lu",
			  sum, stop - start);
	}

	// send a default cvtask mesage to myself
	{
		char message[32];

		sprintf(message, "Hello from frame %d default",
			env->cvtask_frameset_id);
		cvtask_default_message_send(
			message, strlen(message)+1,
			env->cvtask_sysflow_index, env->cvtask_frameset_id+1);
	}

	// send a custom message sending to myself
	{
		char *message;
		uint32_t max_size;

		cvtask_message_create(0, (void**)&message, &max_size);
		sprintf(message, "Hello from frame %d custom %d",
			(int)env->cvtask_frameset_id, (int)max_size);
		cvtask_message_send(
			message, strlen(message) + 1,
			env->cvtask_sysflow_index, env->cvtask_frameset_id+1);
	}

	return CVTASK_ERR_OK;
}

static uint32_t adder_get_info(const cvtask_parameter_interface_t *env,
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

static uint32_t adder_finish(const cvtask_parameter_interface_t *env)
{
	AmbaPrint("[ARM_ADDDER] adder_finish() called\n");
	return CVTASK_ERR_OK;
} /* adder_finish() */

static const cvtask_entry_t entry = {
	.cvtask_name             = CVTASK_NAME,
	.cvtask_type             = CVTASK_TYPE_ARM,
	.cvtask_query            = adder_query,
	.cvtask_init             = adder_init,
	.cvtask_get_info         = adder_get_info,
	.cvtask_run              = adder_run,
	.cvtask_process_messages = adder_process_messages,
	.cvtask_finish           = adder_finish,
};

uint32_t adder_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (adder_create);
