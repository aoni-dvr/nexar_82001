/*
 * SPDX-License-Identifier: GPL-2.0
 */

/*
 * Sample character devices to demo ioctl().
 */

#ifndef HELLO_H
#define HELLO_H

#include <linux/ioctl.h>

/* cf. <linux/major.h> */
#define HELLO_MAJOR     168
#define HELLO_MINOR     0
#define HELLO_MAX_DEV   1

/* Set the string */
#define IOCTL_HELLO_SET		_IOR(HELLO_MAJOR, 2, char *)

/* Get the string */
#define IOCTL_HELLO_GET		_IOR(HELLO_MAJOR, 8, char *)

#endif /* HELLO_H */

