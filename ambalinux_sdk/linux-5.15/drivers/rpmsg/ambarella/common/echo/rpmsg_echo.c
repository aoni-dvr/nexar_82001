/*
 *
 * Copyright (C) 2012-2022, Ambarella, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/list.h>

#include <linux/uaccess.h>
#include <linux/uio.h>

#include <linux/rpmsg.h>
#include <linux/remoteproc.h>


/*
 * Store devices created to list
 */
static DEFINE_MUTEX(echo_list_mutex);
static LIST_HEAD(echo_list);

/*
 * For /dev/rpmsg_echoXX_NAME
 */
#define RP_ECHO_DEV_MAX	(MINORMASK + 1)
static dev_t rp_echo_major;
static struct class *rp_echo_class;
static DEFINE_IDA(rp_echo_minor_ida);

/*
 * Master will be created by rpmsg_ns
 * and will echo back strings.
 */
#define AMBA_RPMSG_ROLE_MST (0)
#define AMBA_RPMSG_ROLE_SLV (1)

/* For old compatible. Only one echo channel exist */
static char *example_printk = "written Message will be printed on Remote Processor";
static int probed = 0;


/**
 * struct amba_rpmsg_echo_drive - bind rpmsg_driver, rpmsg_device, cdev
 * @rp_drv:	RPMSG driver for endpoint.
 * @id_tbl:	ID of (RPMSG) endpoint.
 * @cdev:	control message echoed.
 * @dev:	control message echoed.
 * @rp_dev:	RPMSG device binded.
 * @echo_msg:	message to be sent and echoed.
 * @probed:	Is this driver probed by RPMSG device.
 * @role:	echo role, cf. AMBA_RPMSG_ROLE_xxx
 * @node:	Link list node.
 */
struct amba_rpmsg_echo_driver {
	/* RPMSG driver */
	struct rpmsg_driver rp_drv;
	struct rpmsg_device_id id_tbl;

	/* Control */
	struct cdev cdev;
	struct device dev;
	int has_dev;

	/* RPMSG device */
	struct rpmsg_device *rp_dev;

	/* Private data */
	char *echo_msg;
	uint32_t echo_msg_size;
	uint32_t echo_buf_size;
	uint32_t echo_msg_read_pos;
	int probed;
	int role;

	struct list_head node;
};


/* ============================================================================
 * Old compatible: send message by module-parameter
 */
static int rpmsg_example_printk(const char *val, const struct kernel_param *kp)
{
	char *data = strstrip((char *)val);
	/* Head is the one for old compatible */
	struct amba_rpmsg_echo_driver *echo_drv= container_of(echo_list.next, struct amba_rpmsg_echo_driver, node);
	struct rpmsg_device *rpdev_example = echo_drv->rp_dev;

	if ((rpdev_example) && (rpdev_example->ept) &&
		(rpdev_example->dst != RPMSG_ADDR_ANY)) {
		rpmsg_send(rpdev_example->ept, data, strlen(data) + 1);
	} else {
		if (rpdev_example)
			dev_err(&rpdev_example->dev, "rpmsg_send() failed.\n");
		else
			pr_err("rpmsg_send() failed due to NULL device.\n");
	}

	return param_set_charp(data, kp);
}

static int get_probed(char *buffer, const struct kernel_param *kp)
{
	struct amba_rpmsg_echo_driver *echo_drv= container_of(echo_list.next, struct amba_rpmsg_echo_driver, node);

	probed = echo_drv->probed;

	return param_get_int(buffer, kp);
}

static struct kernel_param_ops param_ops_example_printk = {
	/*.set = param_set_charp,*/
	.set = rpmsg_example_printk,
	.get = param_get_charp,
	.free = param_free_charp,
};

static struct kernel_param_ops param_ops_probed = {
	/*.get = param_get_int,*/
	.get = get_probed,
};

module_param_cb(probed, &param_ops_probed,
	&(probed), 0644);

module_param_cb(example_printk, &param_ops_example_printk,
	&(example_printk), 0644);

/* ============================================================================
 * Rpmsg APIs
 */
static int rpmsg_echo_cb(struct rpmsg_device *rpdev, void *data, int len,
			void *priv, u32 src)
{
	struct rpmsg_driver *rpdrv = container_of(rpdev->dev.driver, struct rpmsg_driver, drv);
	struct amba_rpmsg_echo_driver *echo_drv= container_of(rpdrv, struct amba_rpmsg_echo_driver, rp_drv);

	if (rpdev->dst == RPMSG_ADDR_ANY)
		rpdev->dst = src;

	printk("[ %20s ] recv msg: [%s] from 0x%x and len %d\n",
			__func__, (const char*)data, src, len);

	if (echo_drv->role == AMBA_RPMSG_ROLE_MST) {
		/* Echo the recved message back */
		return rpmsg_send(rpdev->ept, data, len);
	} else {
		return 0;
	}
}

static int rpmsg_echo_probe(struct rpmsg_device *rpdev)
{
	int ret = 0;
	struct rpmsg_channel_info chinfo;
	struct rpmsg_driver *rpdrv = container_of(rpdev->dev.driver, struct rpmsg_driver, drv);
	struct amba_rpmsg_echo_driver *echo_drv = container_of(rpdrv, struct amba_rpmsg_echo_driver, rp_drv);

	echo_drv->rp_dev = rpdev;
	/* Created by NS, need to echo back */
	if ((rpdev->src != RPMSG_ADDR_ANY) &&
		(rpdev->dst != RPMSG_ADDR_ANY)) {
		echo_drv->role = AMBA_RPMSG_ROLE_MST;
	}
	if (echo_drv->role == AMBA_RPMSG_ROLE_MST) {
		strncpy(chinfo.name, rpdev->id.name, sizeof(chinfo.name));
		chinfo.src = rpdev->src;
		chinfo.dst = rpdev->dst;
		rpmsg_send(rpdev->ept, &chinfo, sizeof(chinfo));
	}

	/*channel ready*/
	echo_drv->probed = 1;

	dev_info(rpdev->dev.parent, "Bind driver/device named as %s\n", rpdev->id.name);

	return ret;
}

static void rpmsg_echo_remove(struct rpmsg_device *rpdev)
{
	struct rpmsg_driver *rpdrv = container_of(rpdev->dev.driver, struct rpmsg_driver, drv);
	struct amba_rpmsg_echo_driver *echo_drv = container_of(rpdrv, struct amba_rpmsg_echo_driver, rp_drv);

	echo_drv->rp_dev = NULL;
	echo_drv->probed = 0;

	dev_info(rpdev->dev.parent, "Un-Bind driver/device named as %s\n", rpdev->id.name);
}

/* Old compatible */
static struct rpmsg_device_id rpmsg_echo_id_table[] = {
	{ .name	= "echo_ca9_b", },
	{ .name	= "echo_arm11", },
	{ .name	= "echo_cortex", },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_echo_id_table);

static struct amba_rpmsg_echo_driver rpmsg_echo_driver = {
	.rp_drv.drv.name	= KBUILD_MODNAME,
	.rp_drv.drv.owner	= THIS_MODULE,
	.rp_drv.id_table	= rpmsg_echo_id_table,
	.rp_drv.probe		= rpmsg_echo_probe,
	.rp_drv.callback	= rpmsg_echo_cb,
	.rp_drv.remove		= rpmsg_echo_remove,

	// Default must be slave, otherwise rpmsg_echo_probe() will send incorrect rpmsg.
	.role	= AMBA_RPMSG_ROLE_SLV,
};


/* ============================================================================
 * /dev/rpmsg_echoXX_NAME operations
 */
ssize_t rp_echo_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	struct file *filp = iocb->ki_filp;
	struct amba_rpmsg_echo_driver *echo_drv = filp->private_data;
	size_t sz = iov_iter_count(to);
	uint32_t len;

	/* TODO: assert(sz < RPMSG_SIZE) */
	if (sz > PAGE_SIZE) {
		dev_err(&echo_drv->dev, "Input must be less than page size: %ld\n", PAGE_SIZE);
		return -EINVAL;
	}

	if (echo_drv->echo_msg) {
		if (echo_drv->echo_msg_read_pos) {
			// End one read cycle
			echo_drv->echo_msg_read_pos = 0;
			return 0;
		}
		/* offset should be always 0 */
		len = copy_to_iter(echo_drv->echo_msg, echo_drv->echo_msg_size, to);
		if (len != echo_drv->echo_msg_size) {
			dev_err(&echo_drv->dev, "copy_to_iter(%d) but %d\n",
					echo_drv->echo_msg_size, len);
			return -EFAULT;
		} else {
			echo_drv->echo_msg_read_pos = len;
			return len;
		}
	} else {
		return -EBADF;
	}

	return -EFAULT;
}

ssize_t rp_echo_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *filp = iocb->ki_filp;
	struct amba_rpmsg_echo_driver *echo_drv = filp->private_data;
	size_t sz = iov_iter_count(from);
	uint32_t len;
	struct rpmsg_device *rpdev = echo_drv->rp_dev;

	/* TODO: assert(sz < RPMSG_SIZE) */
	if (sz > PAGE_SIZE) {
		dev_err(&echo_drv->dev, "Input must be less than page size: %ld\n", PAGE_SIZE);
		return -EINVAL;
	}

	if (echo_drv->echo_msg) {
		if (echo_drv->echo_buf_size <= sz) {
			kvfree(echo_drv->echo_msg);
			len = (sz % PAGE_SIZE) ? 1 : 0;
			len += (sz / PAGE_SIZE);
			len *= PAGE_SIZE;
			echo_drv->echo_msg = kvzalloc(len, GFP_KERNEL);
			if (!echo_drv->echo_msg) {
				return -ENOMEM;
			}
			echo_drv->echo_buf_size = len;
		}
	} else {
		len = (sz % PAGE_SIZE) ? 1 : 0;
		len += (sz / PAGE_SIZE);
		len *= PAGE_SIZE;
		echo_drv->echo_msg = kvzalloc(len, GFP_KERNEL);
		if (!echo_drv->echo_msg) {
			return -ENOMEM;
		}
		echo_drv->echo_buf_size = len;
	}

	/* offset should be always 0 */
	len = copy_from_iter(echo_drv->echo_msg, sz, from);
	if (len != sz) {
		dev_err(&echo_drv->dev, "copy_from_iter(%ld) but %d\n", sz, len);
		return -EFAULT;
	}
	echo_drv->echo_msg_size = len;

	if ((rpdev) && (rpdev->ept) &&
		(rpdev->dst != RPMSG_ADDR_ANY)) {
		// Strip end '\n', rx side should check '\0' and length
		if (echo_drv->echo_msg[len - 1] == '\n')
			len--;
		rpmsg_send(rpdev->ept, echo_drv->echo_msg, len);
	} else {
		if (rpdev)
			dev_err(&rpdev->dev, "rpmsg_send() failed.\n");
		else
			pr_err("rpmsg_send() failed due to NULL device.\n");
	}

	return sz;
}


static int rpmsg_ctrldev_open(struct inode *inode, struct file *filp)
{
	struct amba_rpmsg_echo_driver *echo_drv = container_of(inode->i_cdev, struct amba_rpmsg_echo_driver, cdev);

	get_device(&echo_drv->dev);
	filp->private_data = echo_drv;

	return 0;
}

static int rpmsg_ctrldev_release(struct inode *inode, struct file *filp)
{
	struct amba_rpmsg_echo_driver *echo_drv = container_of(inode->i_cdev, struct amba_rpmsg_echo_driver, cdev);

	(void)filp;
	put_device(&echo_drv->dev);

	return 0;
}


static const struct file_operations rp_echo_ctrldev_fops = {
	.owner = THIS_MODULE,
	.read_iter = rp_echo_read_iter,
	.write_iter = rp_echo_write_iter,
	.open = rpmsg_ctrldev_open,
	.release = rpmsg_ctrldev_release,
};


/* ============================================================================
 * Probe triggerd by DTS
 */
static void amba_echo_drv_release_device(struct device *dev)
{
	ida_simple_remove(&rp_echo_minor_ida, MINOR(dev->devt));
}

static int amba_echo_drv_probe(struct platform_device *pdev)
{
	int ret;
	const char *name;
	struct amba_rpmsg_echo_driver *echo_drv;

	ret = of_property_read_string(pdev->dev.of_node, "amb,rpmsg_id", &name);
	if (ret < 0) {
		dev_err(&pdev->dev, "amb,rpmsg_id is not specified\n");
		return -EINVAL;
	}

	/*
	 * rpmsg_ns or amba_rproc will create rpmsg_device
	 * We register driver to wait to be binded.
	 */
	echo_drv = kvzalloc(sizeof(*echo_drv), GFP_KERNEL);
	if (!echo_drv)
		return -ENOMEM;

	strlcpy(echo_drv->id_tbl.name, name, RPMSG_NAME_SIZE);
	echo_drv->rp_drv.drv.name	= name;
	echo_drv->rp_drv.drv.owner	= THIS_MODULE;
	echo_drv->rp_drv.id_table	= &(echo_drv->id_tbl);
	echo_drv->rp_drv.probe		= rpmsg_echo_probe;
	echo_drv->rp_drv.callback	= rpmsg_echo_cb;
	echo_drv->rp_drv.remove		= rpmsg_echo_remove;

	/* When binding with device by rpmsg_ns, role would be set as AMBA_RPMSG_ROLE_MST */
	echo_drv->role = AMBA_RPMSG_ROLE_SLV;

	ret = register_rpmsg_driver(&(echo_drv->rp_drv));
	if (ret) {
		dev_err(&pdev->dev, "register_rpmsg_driver() failed: %d\n", ret);
		kvfree(echo_drv);
		return ret;
	} else {
		mutex_lock(&echo_list_mutex);
		list_add_tail(&echo_drv->node, &echo_list);
		mutex_unlock(&echo_list_mutex);
	}

	device_initialize(&(echo_drv->dev));
	echo_drv->dev.class = rp_echo_class;
	echo_drv->dev.parent = &pdev->dev;
	dev_set_drvdata(&(echo_drv->dev), echo_drv);

	cdev_init(&(echo_drv->cdev), &rp_echo_ctrldev_fops);
	echo_drv->cdev.owner = THIS_MODULE;

	ret = ida_simple_get(&rp_echo_minor_ida, 0, RP_ECHO_DEV_MAX, GFP_KERNEL);
	if (ret < 0)
		goto free_drv;
	echo_drv->dev.devt = MKDEV(MAJOR(rp_echo_major), ret);
	echo_drv->dev.id = ret;
	dev_set_name(&(echo_drv->dev), "rpmsg_echo%d_%s", ret, name);
	ret = cdev_device_add(&(echo_drv->cdev), &(echo_drv->dev));
	if (ret)
		goto free_minor_ida;

	echo_drv->dev.release = amba_echo_drv_release_device;
	echo_drv->has_dev = true;

	dev_info(&pdev->dev, "Create app driver named as %s\n", name);

	return ret;

free_minor_ida:
	ida_simple_remove(&rp_echo_minor_ida, MINOR(echo_drv->dev.devt));
free_drv:
	put_device(&(echo_drv->dev));
	kvfree(echo_drv);

	return ret;
}


static const struct of_device_id ambarella_echo_dt_ids[] = {
	{ .compatible = "ambarella,echo" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ambarella_echo_dt_ids);

static struct platform_driver amba_echo_driver = {
	.probe = amba_echo_drv_probe,
	.driver = {
		.name		= "amba_echo",
		.owner		= THIS_MODULE,
		.of_match_table = ambarella_echo_dt_ids,
	},
};


static int __init rpmsg_echo_init(void)
{
	/* Head is the one for old compatible */
	int ret = register_rpmsg_driver(&(rpmsg_echo_driver.rp_drv));

	if (!ret) {
		mutex_lock(&echo_list_mutex);
		list_add_tail(&(rpmsg_echo_driver.node), &echo_list);
		mutex_unlock(&echo_list_mutex);

		ret = alloc_chrdev_region(&rp_echo_major, 0, RP_ECHO_DEV_MAX, "rp_echo");
		if (ret < 0) {
			pr_err("rpmsg: failed to allocate char dev region\n");
			goto unreg_rp_drv;
		}

		rp_echo_class = class_create(THIS_MODULE, "rp_echo");
		if (IS_ERR(rp_echo_class)) {
			pr_err("failed to create rpmsg class\n");
			unregister_chrdev_region(rp_echo_major, RP_ECHO_DEV_MAX);
			goto unreg_rp_drv;
			ret = PTR_ERR(rp_echo_class);
		}

		ret = platform_driver_register(&amba_echo_driver);
		if (ret) {
			class_destroy(rp_echo_class);
			unregister_chrdev_region(rp_echo_major, RP_ECHO_DEV_MAX);
		}
	}

	return ret;

unreg_rp_drv:
	unregister_rpmsg_driver(&(rpmsg_echo_driver.rp_drv));

	return ret;
}

static void __exit rpmsg_echo_fini(void)
{
	struct amba_rpmsg_echo_driver *d;
	struct amba_rpmsg_echo_driver *d_pre = NULL;
	struct rpmsg_channel_info chinfo;

	list_for_each_entry(d, &echo_list, node) {
		kvfree(d->echo_msg);
		unregister_rpmsg_driver(&(d->rp_drv));
		if (d->rp_dev) {
			chinfo.src = d->rp_dev->src;
			chinfo.dst = d->rp_dev->dst;
			strncpy(chinfo.name, d->rp_dev->id.name, RPMSG_NAME_SIZE);
			rpmsg_unregister_device(&(d->rp_dev->dev), &chinfo);
		}

		if (d->has_dev) {
			cdev_device_del(&(d->cdev), &(d->dev));
			put_device(&(d->dev));

			kvfree(d_pre);
			d_pre = d;
		}
	}
	if ((d_pre) && (d_pre->has_dev))
		kvfree(d_pre);

	class_destroy(rp_echo_class);
	unregister_chrdev_region(rp_echo_major, RP_ECHO_DEV_MAX);

	platform_driver_unregister(&amba_echo_driver);
}

module_init(rpmsg_echo_init);
module_exit(rpmsg_echo_fini);

MODULE_DESCRIPTION("RPMSG Echo Server");
MODULE_LICENSE("GPL");

