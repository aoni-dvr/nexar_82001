/**
 * History:
 *    2012/07/30 - [Tzu-Jung Lee] created file
 *
 * Copyright (C) 2012-2012, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/rpmsg.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/remoteproc.h>

#include <plat/rpdev.h>

static void rpdev_echo_cb(struct rpdev *rpdev, void *data, int len,
				 void *priv, u32 src)
{
//	printk("[ %20s ] recv message: [%s]\n", __func__, (char *)data);
	printk("[ %20s ] recv msg: [%s] from 0x%x and len %d\n",
	       __func__, (const char*)data, src, len);

	/* Echo the recved message back */
	rpdev_trysend(rpdev, data, len);

	return;
}

/* Self-Test, work only when app on host is ready. */
#define RPDEV_ECHO_SELFTEST (0)

static void rpdev_echo_init_work(struct work_struct *work)
{
	struct rpdev *rpdev;
#if RPDEV_ECHO_SELFTEST
	char *str = "Hello from CA9-B";
	char buf[64];
	int i;
#endif

	rpdev = rpdev_alloc("echo_ca9_b", 0, rpdev_echo_cb, NULL);

	rpdev_register(rpdev, "ca9_a_and_b");

#if RPDEV_ECHO_SELFTEST
	printk("[ %20s ] send message: [%s]\n", __func__, str);

	for (i = 0; i < 512; i++) {
		sprintf(buf, "%s  %d", str, i);
		printk("try sending: %s   OK\n", buf);
		while (rpdev_trysend(rpdev, buf, strlen(buf) + 1) != 0) {
			printk("retry sending: %s\n", buf);
			msleep(10);
		}
	}

	for (i = 0; i < 512; i++) {
		sprintf(buf, "%s %d (wait)", str, i);
		printk("sending: %s   OK\n", buf);
		rpdev_send(rpdev, buf, strlen(buf) + 1);
	}
#endif
}

static struct work_struct work;

static int rpdev_echo_init(void)
{
	INIT_WORK(&work, rpdev_echo_init_work);
	schedule_work(&work);
	return 0;
}

static void rpdev_echo_fini(void)
{
	printk(" Stop ==> Rpdev Echo Demo ");
}

module_init(rpdev_echo_init);
module_exit(rpdev_echo_fini);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("rpclnt example for Ambarella remote client");
