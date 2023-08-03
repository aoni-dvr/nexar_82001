/*
 * SPDX-License-Identifier: GPL-2.0
 */

/*
 * Sample character devices to demo ioctl().
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <linux/uaccess.h>

#include "hello.h"


static struct class *hello_class;

#define MAX_HELLO_MSG	256
char msg[MAX_HELLO_MSG];


/*
 * Forward ioctls to the underlying block device.
 */
static long
hello_ioctl(struct file *filp, unsigned int command, unsigned long arg)
{
	char *tmp;
	char ch;
	int ret = 0;

	(void)filp;
	switch (command) {
	case IOCTL_HELLO_SET:
		{
			int i = 0;

			tmp = (char *)arg;
			do {
				get_user(ch, tmp++);
				if (ch) {
					msg[i++] = ch;
				} else {
					break;
				}
			} while (i < (MAX_HELLO_MSG - 1));
			msg[i] = '\0';
			printk("Set Hello msg: %s\n", msg);
		}
		break;

	case IOCTL_HELLO_GET:
		{
			int i = 0;

			tmp = (char *)arg;
			do {
				if (msg[i]) {
					put_user(msg[i++], tmp++);
				} else {
					break;
				}
			} while (i < (MAX_HELLO_MSG - 1));
			*tmp = '\0';
		}
		break;

	default:
		printk("%s, Not handled. cmd: 0x%08X, arg %ld\n", __func__, command, arg);
		ret = -1;
		break;
	}

	return ret;
}



static const struct file_operations hello_fops = {
	.unlocked_ioctl = hello_ioctl,
	.owner		= THIS_MODULE,
};


static struct cdev hello_cdev;

static char *hello_devnode(struct device *dev, umode_t *mode)
{
	return kasprintf(GFP_KERNEL, "hello/%s", dev_name(dev));
}

struct device *dev_hello;

static int __init hello_init(void)
{
	int ret;

	hello_class = class_create(THIS_MODULE, "hello");
	if (IS_ERR(hello_class)) {
		printk(KERN_ERR "Error creating hello class.\n");
		ret = PTR_ERR(hello_class);
		goto error;
	}
	hello_class->devnode = hello_devnode;

	cdev_init(&hello_cdev, &hello_fops);
	ret = cdev_add(&hello_cdev, MKDEV(HELLO_MAJOR, 0), 1);
	if (ret)
		goto error;
	dev_hello = device_create(hello_class, NULL, MKDEV(HELLO_MAJOR, 0), NULL, "hello_amba");
	if (IS_ERR(dev_hello)) {
		printk(KERN_ERR "Error creating hello class.\n");
		ret = PTR_ERR(dev_hello);
		goto error;
	}

	memcpy(msg, "Ambarella", sizeof("Ambarella"));

	return 0;

error:
	return ret;
}

static void __exit hello_exit(void)
{
	device_destroy(hello_class, MKDEV(HELLO_MAJOR, 0));
	class_destroy(hello_class);
	cdev_del(&hello_cdev);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Ambarella Inc.");
MODULE_DESCRIPTION("Example module");
MODULE_LICENSE("GPL");

