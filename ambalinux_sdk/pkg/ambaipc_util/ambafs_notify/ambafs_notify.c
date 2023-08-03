/*
 * pkg/amba_util/ambafs_notify/ambafs_notify.c
 *
 * Author: Yuan-Ying Chang <yychang@ambarella.com>
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * No part of this file may be reproduced, stored in a retrieval
 * system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella Corporation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aipc_user.h"
#include "ambafs_notify.h"
#include "AmbaIPC_RpcProg_RT_AmbaFSNotify.h"

#define MAX_FILE_NAME	512
#define MAX_DRIVE_NAME	10
#define MAX_MOUNT_NODE	10

struct ambafs_node {
	char linux_root[MAX_FILE_NAME];
	char rtos_drive[MAX_DRIVE_NAME];
};

static struct ambafs_node nodes[MAX_MOUNT_NODE];
static int initial = 0;
static char store_buf[MAX_FILE_NAME];
static int store = 0;

static void convert_slash(char old, char new, char *str)
{
	int len, i;

	len = strlen(str);

	for(i=0; i<len; i++) {
		if(str[i] == old) {
			str[i] = new;
		}
	}

}
static void get_linux_path(char *rtos_path, char *linux_path)
{
	char drive[MAX_DRIVE_NAME];
	char copystr[MAX_FILE_NAME];
	char *token;
	int i;

	strcpy(copystr, rtos_path);
	token = strtok(copystr, ":");
	if(token != NULL) {
		strcpy(drive, token);
		strcat(drive, ":");
		token = strtok(NULL, ":");
		for(i=0; i<MAX_MOUNT_NODE; i++) {
			if(!strcmp(drive, nodes[i].rtos_drive)) {
				strcpy(linux_path, nodes[i].linux_root);
				strcat(linux_path, token);
				convert_slash('\\', '/', linux_path);
				return;
			}

		}
	}

	linux_path = NULL;
}

static void get_rtos_path(char *linux_path, char *rtos_path)
{
	int i;
	int len;
	char *part;

	for(i=0; i<MAX_MOUNT_NODE; i++) {
		if(!strcmp(nodes[i].linux_root, "")) {
			rtos_path = NULL;
			break;
		}
		part = strstr(linux_path, nodes[i].linux_root);
		if(part) {
			len = strlen(nodes[i].linux_root);
			strcpy(rtos_path, nodes[i].rtos_drive);
			strcat(rtos_path, &linux_path[len]);
			convert_slash('/', '\\', rtos_path);
			return;
		}
	}

	rtos_path = NULL;
}

static void create_mount_table()
{
	FILE *pFile;
	char *token;
	int index = 0;
	char drive[MAX_DRIVE_NAME];
	char buf[100];
	char dir[MAX_FILE_NAME];

	pFile = fopen("/proc/mounts", "r");
	if(pFile == NULL) {
		printf("%s open /proc/mounts failed\n", __func__);
		return;
	}

	memset(nodes, 0, MAX_MOUNT_NODE * sizeof(struct ambafs_node));
	while (fgets(buf, sizeof(buf), pFile)) {
		token = strtok(buf, " ");
		strcpy(drive, token);
		token = strtok(NULL, " ");
		strcpy(dir, token);
		token = strtok(NULL, " ");
		if(!strcmp(token, "ambafs")) {
			strcpy(nodes[index].linux_root, dir);
			strcpy(nodes[index].rtos_drive, drive);
			index++;
		}
	}
	fclose(pFile);
	initial = 1;
}

static void store_event(RT_AMBAFS_NOTIFY_EVENT_s *pEvent)
{
	memset(store_buf, 0, MAX_FILE_NAME);
	memcpy(store_buf, pEvent, sizeof(RT_AMBAFS_NOTIFY_EVENT_s) + pEvent->len);
	store = 1;
}

static void withdraw_event(RT_AMBAFS_NOTIFY_EVENT_s *pEvent)
{
	RT_AMBAFS_NOTIFY_EVENT_s *sEvent = (RT_AMBAFS_NOTIFY_EVENT_s *)store_buf;

	memcpy(pEvent, store_buf, sizeof(RT_AMBAFS_NOTIFY_EVENT_s) + sEvent->len);
	store = 0;
}

CLIENT_ID_t ambafs_notify_init(void)
{
	// create a rpc client for monitoring
	int rVal;
	CLIENT_ID_t client;
	RT_AMBAFS_NOTIFY_CTRL_s Ctrl;
	AMBA_IPC_REPLY_STATUS_e status;

	if(initial == 0)
		create_mount_table();

	client = ambaipc_clnt_create(AMBA_IPC_HOST_THREADX, RT_AMBAFS_NOTIFY_PROG_ID, RT_AMBAFS_NOTIFY_VER);
	if(!client)
		goto done;

	Ctrl.client_id = client;
	Ctrl.cmd = RT_AMBAFS_NOTIFY_INIT_CMD;

	status = RT_Ambafs_Notify_Ctrl_Clnt(&Ctrl, &rVal, client);
	if(status != 0) {
		printf("%s: %s\n", __func__, ambaipc_strerror(status));
	}
done:
	return client;
}

int ambafs_notify_add_watch(CLIENT_ID_t client_id, const char *pathname, unsigned int mask)
{
	RT_AMBAFS_NOTIFY_WATCH_s *pWatch;
	AMBA_IPC_REPLY_STATUS_e status;
	int watch_id;
	char fullpath[MAX_FILE_NAME];

	// transfer the Linux path to RTOS path
	get_rtos_path((char *)pathname, fullpath);
	if(fullpath == NULL) {
		printf("%s can't get rtos_path\n", __func__);
		watch_id = -1;
		goto done;
	}
	pWatch = malloc(sizeof(RT_AMBAFS_NOTIFY_WATCH_s) + strlen(fullpath) + 1);
	pWatch->client_id = client_id;
	pWatch->mask = mask;
	pWatch->len = strlen(fullpath) + 1;
	memcpy(pWatch->name, fullpath, pWatch->len);

	// call rpc call to add watch
	status = RT_Ambafs_Notify_AddWatch_Clnt(pWatch, &watch_id, client_id);
	if(status != 0) {
		watch_id = -1;
		printf("%s: %s\n", __func__, ambaipc_strerror(status));
		goto done;
	}

done:
	free(pWatch);
	return watch_id;
}

int ambafs_notify_read(CLIENT_ID_t client_id, void *buf, int count)
{
	RT_AMBAFS_NOTIFY_EVENT_s *pEvent;
	AMBA_IPC_REPLY_STATUS_e status;
	char linux_path[MAX_FILE_NAME];
	char *read_buf;
	int read_byte = 0;
	int copy_byte = 0;
	unsigned int *new_len;

	read_buf = (char *)buf;
	pEvent = malloc(sizeof(RT_AMBAFS_NOTIFY_EVENT_s) + MAX_FILE_NAME);
	memset(buf, 0, count);

	do {
		if(store == 1) { // to get the event which is not read at last call.
			withdraw_event(pEvent);
		} else {
			// call rpc to get event
			status = RT_Ambafs_Notify_GetEvent_Clnt(NULL, (void *)pEvent, client_id);
			if(status != 0) {
			//	printf("%s: %s\n", __func__, ambaipc_strerror(status));
				break;
			}
		}

		// convert rtos path to linux path
		get_linux_path(pEvent->name, linux_path);

		read_byte = read_byte + sizeof(RT_AMBAFS_NOTIFY_EVENT_s) + strlen(linux_path) + 1;
		if(read_byte > count) {
			// store the event to the store_buf to let user can read it next time.
			store_event(pEvent);
			break;
		}

		// first, copy the result except the path
		memcpy(&read_buf[copy_byte], pEvent, sizeof(RT_AMBAFS_NOTIFY_EVENT_s));
		copy_byte += sizeof(RT_AMBAFS_NOTIFY_EVENT_s);

		// recalculate the length for linux_path
		new_len = (unsigned int *) &read_buf[copy_byte - 4];
		*new_len = strlen(linux_path) + 1;

		// second, copy the path
		memcpy(&read_buf[copy_byte], linux_path, strlen(linux_path) + 1);
		copy_byte = read_byte;

	} while(read_byte < count);

	free(pEvent);
	return copy_byte;
}

int ambafs_notify_close(CLIENT_ID_t client_id)
{
	RT_AMBAFS_NOTIFY_CTRL_s Ctrl;
	int rVal;

	Ctrl.client_id = client_id;
	Ctrl.cmd = RT_AMBAFS_NOTIFY_CLOSE_CMD;
	// remove the watch and destroy the related source.
	RT_Ambafs_Notify_Ctrl_Clnt(&Ctrl, &rVal, client_id);

	ambaipc_clnt_destroy(client_id);

	return rVal;
}
