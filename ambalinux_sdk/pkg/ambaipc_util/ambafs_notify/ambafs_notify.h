/**
  * Copyright (c) 2015 by Ambarella Inc.

  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:

  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.

  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  * THE SOFTWARE.
 **/

#define AMBAFS_IN_ACCESS        0x0001  /* File was read. */
#define AMBAFS_IN_ATTRIB        0x0002  /* Attribute of the files are changed such as chmod. */
#define AMBAFS_IN_CLOSE_WRITE   0x0004  /* File opend for writing was closed. */
#define AMBAFS_IN_CLOSE_NOWRITE 0x0008  /* File or directory not openen for reading was closed. */
#define AMBAFS_IN_CREATE        0x0010  /* File/directory created in watched directory. */
#define AMBAFS_IN_DELETE        0x0020  /* File/directory deleted from watched directory. */
#define AMBAFS_IN_DELETE_SELF   0x0040  /* Watch file/directory was itself deleted. */
#define AMBAFS_IN_MODIFY        0x0080  /* File was modified. */
#define AMBAFS_IN_MOVE_SELF     0x0100  /* Watched file/directory was itself moved. */
#define AMBAFS_IN_MOVE_FROM     0x0200  /* Generated for the directory containing the old filename when a file is renamed. */
#define AMBAFS_IN_MOVE_TO       0x0400  /* Generated for the directory containing the new filename when a file is renamed. */
#define AMBAFS_IN_OPEN          0x0800  /* File or directory was opened. */

struct ambafs_notify_event {
	int      	wd;       	/* Watch descriptor */
	unsigned int 	mask;     	/* Mask describing event */
	unsigned int	len;      	/* Size of name field */
	char		name[0];   	/* Optional null-terminated name */
};

/* Initally, it creates an ambafs_notify client to inform RTOS to create an amba_notify
instance. As a result, it return client id. */
CLIENT_ID_t ambafs_notify_init(void);

/* It is used to monitor one file or directory along with some specific event. */
int ambafs_notify_add_watch(CLIENT_ID_t client_id, const char *pathname, unsigned int mask);

/* It is used to get information when the event is detected. It is blocking
unitl the event is monitored. */
int ambafs_notify_read(CLIENT_ID_t client_id, void *buf, int count);

/* When monitoring is over, the API is called to release all
the resource. */
int ambafs_notify_close(CLIENT_ID_t client_id);
