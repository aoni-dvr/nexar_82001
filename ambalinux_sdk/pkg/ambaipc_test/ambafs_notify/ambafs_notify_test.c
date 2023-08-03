#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "aipc_user.h"
#include "ambafs_notify.h"

int main(int argc, char* argv[])
{
	CLIENT_ID_t client_id;
	char buf[100];
	int wd[2], count, index;
	struct ambafs_notify_event *recv;
	int size = sizeof(struct ambafs_notify_event);

	client_id =  ambafs_notify_init();

	/* user can change the monitoring directory. */
	wd[0] = ambafs_notify_add_watch(client_id, "/tmp/SD0/DCIM", AMBAFS_IN_OPEN | AMBAFS_IN_CLOSE_WRITE);
	wd[1] = ambafs_notify_add_watch(client_id, "/tmp/SD0/usb_save", AMBAFS_IN_OPEN);

	while(1) {
		count = ambafs_notify_read(client_id, buf, 100);
		index = 0;
		while(index + 1 < count) {
			recv = (struct ambafs_notify_event *) &buf[index];
			if(recv->mask & AMBAFS_IN_OPEN)
				printf("AMBAFS_IN_OPEN: ");
			if(recv->mask & AMBAFS_IN_CLOSE_WRITE)
				printf("AMBAFS_IN_CLOSE_WRITE: ");
			printf("WATCHED in");

			if (recv->wd == wd[0]) {
				printf(" %s ", "tmp/SD0/DCIM");
			} else if (recv->wd == wd[1]) {
				printf(" %s ", "tmp/SD0/usb_save");
			} else {
				printf(" no such watch folder ");
			}
			printf("FILE is %s\n", recv->name);
			index = index + size + recv->len;
		}
	}

}
