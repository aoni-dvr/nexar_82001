/*
 * The "profiler" collects all profile informations and save them for
 * offline analysis.
 */
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "cvtask_api.h"
#include "cvtask_errno.h"
#include "cvtask_ossrv.h"
#include "cvapi_logger_interface.h"
#include "build_version.h"

#define FORMAT_LABEL       "LOGWRITER_FILE_NAME"
#define FORMAT_DEFAULT     "debug_%s_%s.log"

#define LOG_NOTHING        0x0
#define LOG_TO_FILE_ASCII  0x3
#define LOG_TO_FILE        0x2
#define LOG_TO_FILE_MASK   0x3
#define LOG_TO_CONSOLE     0x4

struct log_track {
	FILE                    *fp;
	uint32_t                config;
	uint32_t                last_write_index;
	char                    set_name[16];
	char                    *format_base; // printf format strings
	char                    *hotlink_base; // printf format strings (hotlink)
	char                    **base_addr;  // addr of pd->base
};

struct log_set {
	struct log_track        cvtask;
	struct log_track        sched;
};

struct priv_data {
	char                    *base;
	const char              *filename_format;
	struct log_set          vp;
	struct log_set          sod;
	struct log_set          arm[SYSTEM_MAX_NUM_CORTEX];
};

static const cvtask_memory_interface_t interface = {
	.num_inputs                       = 1,
	.Instance_private_storage_needed  = sizeof(struct priv_data),
	.input[0].io_name                 = "SYSTEM_LOGINFO",
};

static void sanitize_format(char *dst, const char *src)
{
	char *dst_curr;
	const char *src_curr;
	unsigned int state;

	dst_curr  = dst;
	src_curr  = src;
	state     = 0;
	while (src_curr[0] != 0x00)
	{
		dst_curr[0] = src_curr[0];
		if ((src_curr[0] == '%') && (state == 0))
		{
			state = 1;
		} /* if ((src_curr[0] == '%') && (state == 0)) */
		else if (state == 1)
		{
			state = 0;
			if (src_curr[0] == 's')
			{
				AmbaPrint("[WARNING] : cvlog detected printf with %%s contained (%%s) - replacing with %%d\n", src);
				dst_curr[0] = 'd';
			} /* if (src_curr[0] == 's') */
		} /* else if (state == 1) */
		src_curr++;
		dst_curr++;
	} /* while (src_curr[0] != 0x00) */
	dst_curr[0] = 0x00;
} /* sanitize_format() */

/*
 * Process one cvlog_cvtask_entry_t element
 */
static void process_entry(struct log_track *track, cvlog_cvtask_entry_t *e)
{
	char log[1024];
	char format_cleaned[1024];
	int num;
	const char *format;

	if (track->config == LOG_NOTHING) {
		return;
	}

	if ((track->config & LOG_TO_FILE_MASK) == LOG_TO_FILE) {
		fwrite(e, 1, sizeof(*e), track->fp);
		return;
	}

	/* calculate the string format address */
	if ((e->entry_string_offset >> 29) == 0)
	{
		format = track->format_base;
	} /* if ((e->entry_string_offset >> 29) == 0) */
	else if ((e->entry_string_offset >> 29) == 1)
	{
		format = track->hotlink_base;
	} /* if ((e->entry_string_offset >> 29) == 1) */
	else /* if ((e->entry_string_offset >> 29) > 1) */
	{
		format = NULL;
	} /* if ((e->entry_string_offset >> 29) > 1) */

	if (format != NULL)
	{
		format += (e->entry_string_offset & 0x1FFFFFFFU);

		/* replicate the output with a simple header */
		num  = sprintf(log, "<TH-%d:%u> ",
			       (int)e->hdr.entry_flags.src_thread,
			       (unsigned int)e->entry_time);

		sanitize_format(&format_cleaned[0], &format[0]);

		num += sprintf(log + num, format_cleaned,
			e->entry_arg1,
			e->entry_arg2,
			e->entry_arg3,
			e->entry_arg4,
			e->entry_arg5);

		/* add a newline if it is not found at the end of @format */
		if (format[strlen(format) - 1] != '\n') {
			num += sprintf(log + num, "\n");
		}

		if ((track->config & LOG_TO_FILE_MASK) == LOG_TO_FILE_ASCII) {
			fwrite(log, 1, num, track->fp);
		}

		if (track->config & LOG_TO_CONSOLE) {
			AmbaPrint(log);
		}
	} /* if (format != NULL) */
}

/*
 * Process a list of cvlog_cvtask_entry_t elements in a log buffer
 */
static void process_track(struct log_track *track, cvlog_buffer_info_t *info)
{
	char *base = *(track->base_addr);
	cvlog_cvtask_entry_t *entry;
	uint32_t wi_prev, wi_curr, i;

	entry = (cvlog_cvtask_entry_t*) (base + info->buffer_addr_offset);
	track->format_base  = base + info->binary_offset;
	track->hotlink_base = base + info->hotlink_offset;
	wi_prev = track->last_write_index;
	wi_curr = info->write_index;

	/* process the tail part in case of wrap-around */
	if (wi_prev > wi_curr) {
		for (i = wi_prev; i < info->buffer_size_in_entries; i++) {
			process_entry(track, &(entry[i]));
		}
		wi_prev = 0;
	}

	/* process the head part */
	for (i = wi_prev; i < wi_curr; i++) {
		process_entry(track, &(entry[i]));
	}

	/* update the write index */
	track->last_write_index = wi_curr;
}

/*
 * Process the log buffer set for a SOD/VP/ARM core
 */
static void process_set(struct log_set *set, cvlog_buffer_set_t *info)
{
	process_track(&set->cvtask, &info->cvtask_log);
	process_track(&set->sched,  &info->sched_log );
}

static void init_set(struct priv_data *pd, struct log_set *set,
		     const char *set_name, uint32_t flag)
{
	FILE *fp;
	char file_name[128];
	uint32_t config;
	const char *format = pd->filename_format;

	config = flag & 0xF;
	set->cvtask.base_addr = &(pd->base);
	set->cvtask.config = config;
	strcpy(set->cvtask.set_name, set_name);
	if (config & LOG_TO_FILE) {
		sprintf(file_name, format, set_name, "cvtask");
		fp = fopen(file_name, "wb");
		if (fp == NULL) {
			AmbaPrint("LogWrite can't open file %s!", file_name);
			assert(0);
		}
		set->cvtask.fp = fp;
	}

	config = flag >> 4;
	set->sched.base_addr = &(pd->base);
	set->sched.config = config;
	strcpy(set->sched.set_name, set_name);
	if (config & LOG_TO_FILE) {
		sprintf(file_name, format, set_name, "sched");
		fp = fopen(file_name, "wb");
		if (fp == NULL) {
			AmbaPrint("LogWrite can't open file %s!", file_name);
			assert(0);
		}
		set->sched.fp = fp;
	}
}

static uint32_t logwriter_query(uint32_t iid, const uint32_t *config,
	cvtask_memory_interface_t *dst)
{
	CVTASK_PRINTF(LVL_VERBOSE, "query cvtask [LogWriter]");

	*dst = interface;
	return CVTASK_ERR_OK;
}

static uint32_t logwriter_init(const cvtask_parameter_interface_t *e,
	const uint32_t *config)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	const char *format;
	static struct priv_data spd;
	int i;

	memset(pd, 0, sizeof(*pd));
	if (cvtable_find(FORMAT_LABEL, (const void**)&format, NULL) != 0) {
		format = FORMAT_DEFAULT;
	}
	pd->filename_format = format;

	if (spd.sod.sched.fp)
		fclose(spd.sod.sched.fp);
	if (spd.sod.cvtask.fp)
		fclose(spd.sod.cvtask.fp);
	if (spd.vp.sched.fp)
		fclose(spd.vp.sched.fp);
	if (spd.vp.cvtask.fp)
		fclose(spd.vp.cvtask.fp);
	for (i = 0; i <= 3; i++) {
		if (spd.arm[i].sched.fp)
			fclose(spd.arm[i].sched.fp);
		if (spd.arm[i].cvtask.fp)
			fclose(spd.arm[i].cvtask.fp);
	}

	init_set(pd, &pd->sod,    "sod",  (config[0] >> 0) & 0xFF);
	init_set(pd, &pd->vp,     "vp",   (config[0] >> 8) & 0xFF);
	init_set(pd, &pd->arm[0], "arm0", (config[0] >>16) & 0xFF);
	init_set(pd, &pd->arm[1], "arm1", (config[0] >>24) & 0xFF);
	init_set(pd, &pd->arm[2], "arm2", (config[1] >> 0) & 0xFF);
	init_set(pd, &pd->arm[3], "arm3", (config[1] >> 8) & 0xFF);

	memcpy(&spd, pd, sizeof(spd));
	CVTASK_PRINTF(LVL_NORMAL, "init cvtask [LogWriter] 0x%X", config[0]);
	return CVTASK_ERR_OK;
}

static uint32_t logwriter_run(const cvtask_parameter_interface_t *e)
{
	struct priv_data *pd = (struct priv_data*)e->vpInstance_private_storage;
	cvlogger_state_t *state;
	int i;

	CVTASK_PRINTF(LVL_NORMAL, "[LOGWRITER @ %d]", e->cvtask_frameset_id);

	/* setup "base" for all xxx_offset */
	pd->base = e->vpInputBuffer[0];
	state = (cvlogger_state_t *)pd->base;

	/* process each core */
	process_set(&pd->sod, &state->orcsod);
	process_set(&pd->vp,  &state->orcvp );
	for (i = 0; i < state->num_arm; i++) {
		process_set(&pd->arm[i], &state->arm[i]);
	}
	return CVTASK_ERR_OK;
}

static uint32_t logwriter_get_info(
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
	.cvtask_name     = "ARM_LOGWRITER",
	.cvtask_type     = CVTASK_TYPE_ARM_LOGGER,
	.cvtask_query    = logwriter_query,
	.cvtask_init     = logwriter_init,
	.cvtask_run      = logwriter_run,
	.cvtask_get_info = logwriter_get_info,
};

uint32_t logwriter_create(void)
{
	// register ourself to the framework so it knows our existence
	cvtask_register(&entry, CVTASK_API_VERSION);
	return CVTASK_ERR_OK;
}

cvtask_declare_create (logwriter_create);
