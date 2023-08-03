/*
 * Copyright (C) 2017-2029, Ambarella, Inc.
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

#if defined(CONFIG_ARCH_AMBA_CAMERA)
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <soc/ambarella/misc.h>

//#define DRV_DBG 1
#define DRV_NAME "dramc"
/* Use 'd' as magic number */
#define DRAMC_IOC_MAGIC 'd'
#define DRAMC_IOC_RESET          _IO(DRAMC_IOC_MAGIC,  0)         // Reset records in this Linux devdrv
#define DRAMC_IOC_CHG_REG        _IOW(DRAMC_IOC_MAGIC, 1, int)    // Change target DRAMC reg offset for R/W
#define DRAMC_IOC_CHG_VAL        _IOW(DRAMC_IOC_MAGIC, 2, int)    // Change value to be written to DRAMC
#define DRAMC_IOC_W_DRAMC        _IO(DRAMC_IOC_MAGIC,  3)         // Write to DRAMC from Linux devdrv
#define DRAMC_IOC_R_DRAMC        _IOR(DRAMC_IOC_MAGIC, 4, int)    // Read from DRAMC to Linux devdrv
#define DRAMC_IOC_R_REG_VALUE    _IOR(DRAMC_IOC_MAGIC, 5, int)    // Read raw value of reg_value
#define DRAMC_IOC_R_REG_OFFSET   _IOR(DRAMC_IOC_MAGIC, 6, int)    // Read raw value of reg_offset

extern void ambarella_scm_dramc_write(u32 offset, u32 val);
extern u32 ambarella_scm_dramc_read(u32 offset);

static dev_t dev = 0;
static struct class *dev_class;
static struct cdev dramc_cdev;
//static unsigned long dramc_base = 0;
static u32 current_mode = 0;    // 0: offset, 1: value
static u32 offset = 0;
static u32 value = 0;

static int ambarella_dramc_open(struct inode *inode, struct file *file)
{
#if defined(DRV_DBG)
    pr_info("%s opened!\n", DRV_NAME);
#endif
	return 0;
}

static ssize_t ambarella_dramc_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    return 0;
}

static ssize_t ambarella_dramc_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    return 0;
}

static long ambarella_dramc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u32 data;

    switch(cmd) {
        case DRAMC_IOC_RESET:
            offset = 0;
            value = 0;
#if defined(DRV_DBG)
            pr_info("[%s]Reset Offset/Value = 0x0\n", DRV_NAME);
#endif
            break;
        case DRAMC_IOC_CHG_REG:
            if(copy_from_user(&data, (u32*)arg, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

            current_mode = 0;
            offset = data;
#if defined(DRV_DBG)
            pr_info("[%s]Reg offset = 0x%x\n", DRV_NAME, offset);
#endif
            break;
        case DRAMC_IOC_CHG_VAL:
            if(copy_from_user(&data, (u32*)arg, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

            current_mode = 1;
            value = data;
#if defined(DRV_DBG)
            pr_info("[%s]Value = 0x%x\n", DRV_NAME, value);
#endif
            break;
        case DRAMC_IOC_W_DRAMC:
            if(copy_from_user(&data, (u32*)arg, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

            ambarella_scm_dramc_write(offset, data);
#if defined(DRV_DBG)
            pr_info("[%s]Write: 0x%x to [0x%x]\n", DRV_NAME, data, offset);
#endif
            break;
        case DRAMC_IOC_R_DRAMC:
            current_mode = 1;
            value = ambarella_scm_dramc_read(offset);
            data = value;
            if(copy_to_user((u32*)arg, &data, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

#if defined(DRV_DBG)
            pr_info("[%s]Read: 0x%x\n", DRV_NAME, data);
#endif
            break;
        case DRAMC_IOC_R_REG_VALUE:
            current_mode = 1;
            data = value;
            if(copy_to_user((u32*)arg, &data, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

#if defined(DRV_DBG)
            pr_info("[%s]Read value: 0x%x\n", DRV_NAME, data);
#endif
            break;
        case DRAMC_IOC_R_REG_OFFSET:
            current_mode = 0;
            data = offset;
            if(copy_to_user((u32*)arg, &data, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

#if defined(DRV_DBG)
            pr_info("[%s]Read offset: 0x%x\n", DRV_NAME, data);
#endif
            break;
        default:
            break;
    }

    return 0;
}

static const struct file_operations dramc_fops = {
    .owner = THIS_MODULE,
	.open = ambarella_dramc_open,
    .read = ambarella_dramc_read,
    .write = ambarella_dramc_write,
	.unlocked_ioctl = ambarella_dramc_ioctl,
};

static int __init ambarella_dramc_init(void)
{
    int ret;
    int major;
    dev_t device;

    ret = alloc_chrdev_region(&dev, 0, 1, DRV_NAME);
    if(ret < 0) {
        pr_err("Cannot allocate device number...\n");
        goto r_alloc;
    }

    dev_class = class_create(THIS_MODULE, DRV_NAME);
    if(!dev_class) {
        pr_err("Cannot create device class...\n");
        goto r_class;
    }

    cdev_init(&dramc_cdev, &dramc_fops);
    major = MAJOR(dev);
    device = MKDEV(major, 0);
    ret = cdev_add(&dramc_cdev, device, 1);
    if(ret) {
        pr_err("%s: Failed in adding cdev to subsystem "
        "ret:%d\n", __func__, ret);
        goto r_class;
    }

    if(device_create(dev_class, NULL, device, NULL, "dramc") == NULL)
        goto r_device;

    /*
    dramc_base = ioremap(0x1000000000, 0x2000);
    pr_info("%s_base=0x%08lx, %s_size=0x%08lx\n", DRV_NAME, dramc_base, DRV_NAME, 0x2000);
    pr_info("ioremap:%lx\n", (unsigned long)dramc_base);
    */

    pr_info("Module %s init done!\n", DRV_NAME);
	return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, 1);
r_alloc:
    return -1;
}

static void __exit ambarella_dramc_exit(void)
{
    int major;
    dev_t device;

    cdev_del(&dramc_cdev);
    major = MAJOR(dev);
    device = MKDEV(major, 0);
    device_destroy(dev_class, device);

    class_destroy(dev_class);
    unregister_chrdev_region(dev, 1);
    pr_info("%s: In exit\n", __func__);
}

module_init(ambarella_dramc_init);
module_exit(ambarella_dramc_exit);

MODULE_AUTHOR("Ambarella Inc.");
MODULE_DESCRIPTION("DRAMC");
MODULE_LICENSE("GPL");

#endif

