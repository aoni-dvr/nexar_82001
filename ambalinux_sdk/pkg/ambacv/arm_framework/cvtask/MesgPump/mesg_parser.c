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
#include <stdio.h>
#include <stdlib.h>
#include "private.h"

#ifndef MPX_TEST
#include "cvtask_api.h"
#endif

static char *cursor;

void parse_int(const char *buf, const char *attr, int *val)
{
	char *start;

	start = strstr(buf, attr);
	if (start == NULL) {
		CMSG("Corrupted message pump manifest file\n");
		exit(-1);
	}
	start += strlen(attr) + 2;
	sscanf(start, "%d", val);
}


void parse_str(const char *buf, const char *attr, char *result)
{
	char *start, *stop;
	int size;

	start = strstr(buf, attr);
	if (start == NULL) {
		CMSG("Corrupted message pump manifest file\n");
		exit(-1);
	}
	start += strlen(attr) + 2;

	stop = strchr(start, '"');
	if (stop == NULL) {
		CMSG("Corrupted message pump manifest file\n");
		exit(-1);
	}
	size = stop - start;

	strncpy(result, start, size);
	result[size] = 0;
}

void parse_binary(struct priv_data *db, char *cursor)
{
	FILE *fp;
	int id;
	char path[128];
	static FILE* s_binary_fp[MAX_BINARY_COUNT] = {NULL};

	parse_int(cursor, "id",   &id);
	parse_str(cursor, "path", path);

#ifdef MPX_TEST
	printf("binary %d [%s]\n", id, path);
#else

	if (s_binary_fp[db->binary_count] != NULL) {
		fclose(s_binary_fp[db->binary_count]);
	}

	fp = fopen(path, "rb");
	if (fp == NULL) {
		AmbaPrint("[PUMPER}: Can't open binary file %s", path);
		assert(0);
	}
	db->binary_fp[db->binary_count] = fp;
	s_binary_fp[db->binary_count] = fp;
#endif

	db->binary_count++;
}

void parse_cvtask(struct priv_data *db, char *cursor)
{
	int id;
	uint32_t sid, num_found;
	char name[64], inst[64];

	parse_int(cursor, "id",   &id);
	parse_str(cursor, "name", name);
	parse_str(cursor, "instance", inst);

	if (id >= MAX_CVTASK_COUNT) {
		CMSG("cvtask id %d is too big, must under %d!\n",
		       id, MAX_CVTASK_COUNT);
		exit(-1);
	}

	if (db->sysflow_id[id] != -1) {
		CMSG("cvtask id %d is redefined, exit!\n", id);
		exit(-1);
	}

#ifdef MPX_TEST
	printf("cvtask %d [%s] [%s]\n", id, name, inst);
#else
	cvtask_get_sysflow_index_list(inst, NULL, NULL, name,
		(uint32_t*)&sid, 1, &num_found);
	if (num_found != 1) {
		AmbaPrint("[PUMPER]: Can't find [%s:%s] from sysflow table!",
			name, inst);
		exit(-1);
	}
	db->sysflow_id[id] = sid;
	cvtask_get_name(sid, NULL, NULL, NULL, NULL, &db->uuid[id]);
	strncpy(db->sysflow_name[id], name, NAME_MAX_LENGTH);
#endif
}

void parse_mesg(struct priv_data *db, char *cursor)
{
	int val;
	struct pumper_mesg *mesg = &(db->pmesg[db->pmesg_count]);

	/* get cvtask ID */
	parse_int(cursor, "cvtask", &val);
	if (val < 0 || val >= MAX_CVTASK_COUNT || db->sysflow_id[val] == -1) {
		CMSG("[PUMPER]: message %d has illegal cvtask id %d!\n",
			db->pmesg_count, val);
		exit(-1);
	}
	mesg->cvtask_id = val;

	/* get frame number */
	parse_int(cursor, "frame_send",    &mesg->frame_send);
	parse_int(cursor, "frame_deliver", &mesg->frame_deliver);

	/* get message payload info */
	parse_int(cursor, "binary",        &mesg->binary_id);
	parse_int(cursor, "payload_off",   &mesg->payload_off);
	parse_int(cursor, "payload_len",   &mesg->payload_len);

#ifdef MPX_TEST
	printf("mesg %d: send=%d, delv=%d, bin=%d, off=%d, size=%d\n",
	       db->pmesg_count, mesg->frame_send, mesg->frame_deliver,
	       mesg->binary_id, mesg->payload_off, mesg->payload_len);
#else
	if (mesg->payload_len > CVTASK_MSG_MAX_LENGTH) {
		CMSG("[PUMPER]: message %d is too long!\n", db->pmesg_count);
		exit(-1);
	}
#endif
	db->pmesg_count++;
}

void parse_message_manifest(struct priv_data *db, char *buf, int size)
{
	/* skip leading <CONFIG> */
	cursor = strstr(buf, "<CONFIG>");
	if (cursor == NULL) {
		CMSG("Corrupted message pump manifest file\n");
		exit(-1);
	}
	cursor += 8;

	while (NULL != (cursor = strchr(cursor, '<'))) {
		cursor++;
		//printf("%c%c%c\n", cursor[0], cursor[1], cursor[2]);
		if (!strncmp(cursor, "binary", 6)) {
			parse_binary(db, cursor);
		} else if (!strncmp(cursor, "cvtask", 6)) {
			parse_cvtask(db, cursor);
		} else if (!strncmp(cursor, "mesg", 4)) {
			parse_mesg(db, cursor);
		} else if (!strncmp(cursor, "!--", 3)) {
			continue;
		} else if (!strncmp(cursor, "/CONFIG>", 8)) {
			break;
		} else {
			CMSG("Corrupted message pump manifest file\n");
			exit(-1);
		}
	}
}
