/*
 * kernel/private/platform/ambarella/debug/debug_core.c
 *
 * History:
 *    2008/04/10 - [Anthony Ginger] Create
 *
 * Copyright (C) 2016  Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

// #include <config.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/mman.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include "inc/iav_devnum.h"
#include "inc/amba_debug.h"
#include "inc/iav_utils.h"

#define AMBA_DEBUG_NAME			"ambad"
#define AMBA_DEBUG_DEV_NUM		1
#define DDR_START			DEFAULT_MEM_START
#define DDR_SIZE			(0x40000000)
static struct cdev amba_debug_cdev;
#if defined(CONFIG_DRIVER_VERSION_V5) || defined(CONFIG_DRIVER_VERSION_V6)
static struct class *amba_debug_class = NULL;
#else
#define AMBA_DEBUG_DEV_MAJOR		AMBA_DEV_MAJOR
#define AMBA_DEBUG_DEV_MINOR		(AMBA_DEV_MINOR_PUBLIC_END + 8)
static dev_t amba_debug_dev_id =
	MKDEV(AMBA_DEBUG_DEV_MAJOR, AMBA_DEBUG_DEV_MINOR);
#endif

DEFINE_MUTEX(amba_debug_mutex);

static long amba_debug_ioctl(struct file *filp,
	unsigned int cmd, unsigned long args)
{
	int				errorCode = 0;

	mutex_lock(&amba_debug_mutex);

	switch (cmd) {
	case AMBA_DEBUG_IOC_GET_DEBUG_FLAG:
		break;

	case AMBA_DEBUG_IOC_SET_DEBUG_FLAG:
		break;

	case AMBA_DEBUG_IOC_GET_GPIO:
	{
		struct amba_vin_test_gpio	gpio_data;

		errorCode = copy_from_user(&gpio_data,
			(struct amba_vin_test_gpio __user *)args,
			sizeof(gpio_data)) ? -EFAULT : 0;
		if (errorCode)
			break;

		errorCode = gpio_request(gpio_data.id, "gpio");
		if (errorCode < 0)
			break;

		errorCode = gpio_direction_input(gpio_data.id);
		if (errorCode >= 0)
			gpio_data.data = gpio_get_value_cansleep(gpio_data.id);

		errorCode = copy_to_user(
			(struct amba_vin_test_gpio __user *)args,
			&gpio_data, sizeof(gpio_data)) ? -EFAULT : 0;
		if (errorCode < 0)
			break;

		gpio_free(gpio_data.id);
	}
		break;

	case AMBA_DEBUG_IOC_SET_GPIO:
	{
		struct amba_vin_test_gpio	gpio_data;

		errorCode = copy_from_user(&gpio_data,
			(struct amba_vin_test_gpio __user *)args,
			sizeof(gpio_data)) ? -EFAULT : 0;
		if (errorCode)
			break;

		errorCode = gpio_request(gpio_data.id, "gpio");
		if (errorCode < 0)
			break;

		gpio_direction_output(gpio_data.id, gpio_data.data);

		gpio_free(gpio_data.id);
	}
		break;

	default:
		errorCode = -ENOIOCTLCMD;
		break;
	}

	mutex_unlock(&amba_debug_mutex);

	return errorCode;
}

static int amba_debug_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int		errorCode = 0;

	mutex_lock(&amba_debug_mutex);

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	if (remap_pfn_range(vma, vma->vm_start,	vma->vm_pgoff,
		vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
			errorCode = -EINVAL;
	}

	mutex_unlock(&amba_debug_mutex);
	return errorCode;
}

static int amba_debug_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int amba_debug_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations amba_debug_fops = {
	.owner		= THIS_MODULE,
	.open		= amba_debug_open,
	.release	= amba_debug_release,
	.unlocked_ioctl	= amba_debug_ioctl,
	.mmap		= amba_debug_mmap,
};

#if defined(CONFIG_DRIVER_VERSION_V5) || defined(CONFIG_DRIVER_VERSION_V6)
static char *amba_debug_devnode(struct device *dev, umode_t *mode)
{
	if (!mode) {
		return NULL;
	}
	*mode = 0666;

	return NULL;
}

static int __init amba_debug_init(void)
{
	int rval = 0;
	struct device *amba_debug_device = NULL;
	dev_t amba_debug_dev_id;

	rval = alloc_chrdev_region(&amba_debug_dev_id, 0, AMBA_DEBUG_DEV_NUM, AMBA_DEBUG_NAME);
	if (rval) {
		printk(KERN_ERR "failed to get dev region for %s.\n", AMBA_DEBUG_NAME);
		goto AMBA_DEBUG_INIT_EXIT;
	}

	cdev_init(&amba_debug_cdev, &amba_debug_fops);
	amba_debug_cdev.owner = THIS_MODULE;
	rval = cdev_add(&amba_debug_cdev, amba_debug_dev_id, AMBA_DEBUG_DEV_NUM);
	if (rval) {
		printk(KERN_ERR "amba_debug_init cdev_add failed %d.\n",
			rval);
		unregister_chrdev_region(amba_debug_dev_id, AMBA_DEBUG_DEV_NUM);
		goto AMBA_DEBUG_INIT_EXIT;
	}

	amba_debug_class = class_create(THIS_MODULE, AMBA_DEBUG_NAME);
	if (IS_ERR(amba_debug_class)) {
		printk(KERN_ERR "class_create failed for %s: %d\n", AMBA_DEBUG_NAME, rval);
		unregister_chrdev_region(amba_debug_cdev.dev, 1);
		rval = PTR_ERR(amba_debug_class);
		goto AMBA_DEBUG_INIT_EXIT;
	}

	amba_debug_class->devnode = amba_debug_devnode;
	amba_debug_device = device_create(amba_debug_class, NULL, amba_debug_cdev.dev, NULL, "%s", AMBA_DEBUG_NAME);
	if (IS_ERR(amba_debug_device)) {
		printk(KERN_ERR "device_create failed for %s: %d\n", AMBA_DEBUG_NAME, rval);
		class_destroy(amba_debug_class);
		unregister_chrdev_region(amba_debug_cdev.dev, 1);
		rval = PTR_ERR(amba_debug_device);
		goto AMBA_DEBUG_INIT_EXIT;
	}

	printk(KERN_INFO "amba_debug_init %d:%d.\n",
		MAJOR(amba_debug_cdev.dev), MINOR(amba_debug_cdev.dev));

AMBA_DEBUG_INIT_EXIT:
	return rval;
}

static void __exit amba_debug_exit(void)
{
	device_destroy(amba_debug_class, amba_debug_cdev.dev);
	class_destroy(amba_debug_class);
	cdev_del(&amba_debug_cdev);
	unregister_chrdev_region(amba_debug_cdev.dev, AMBA_DEBUG_DEV_NUM);
}
#else
static int __init amba_debug_init(void)
{
	int			errorCode = 0;

	errorCode = register_chrdev_region(amba_debug_dev_id,
		AMBA_DEBUG_DEV_NUM, AMBA_DEBUG_NAME);
	if (errorCode) {
		printk(KERN_ERR "amba_debug_init failed to get dev region.\n");
		goto amba_debug_init_exit;
	}

	cdev_init(&amba_debug_cdev, &amba_debug_fops);
	amba_debug_cdev.owner = THIS_MODULE;
	errorCode = cdev_add(&amba_debug_cdev,
		amba_debug_dev_id, AMBA_DEBUG_DEV_NUM);
	if (errorCode) {
		printk(KERN_ERR "amba_debug_init cdev_add failed %d.\n",
			errorCode);
		unregister_chrdev_region(amba_debug_dev_id, AMBA_DEBUG_DEV_NUM);
		goto amba_debug_init_exit;
	}
	printk(KERN_INFO "amba_debug_init %d:%d.\n",
		MAJOR(amba_debug_dev_id), MINOR(amba_debug_dev_id));

amba_debug_init_exit:
	return errorCode;
}

static void __exit amba_debug_exit(void)
{
	cdev_del(&amba_debug_cdev);
	unregister_chrdev_region(amba_debug_dev_id, AMBA_DEBUG_DEV_NUM);
}
#endif

module_init(amba_debug_init);
module_exit(amba_debug_exit);

MODULE_DESCRIPTION("Ambarella kernel debug driver");
MODULE_AUTHOR("Anthony Ginger, <hfjiang@ambarella.com>");
MODULE_LICENSE("GPL v2");

