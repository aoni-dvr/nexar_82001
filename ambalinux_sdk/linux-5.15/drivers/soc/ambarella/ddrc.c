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
#define DRV_NAME "ddrc"
#define DDRC_NUM 2
/* Use 'd' as magic number */
#define DDRC_IOC_MAGIC 'd'
#define DDRC_IOC_RESET          _IO(DDRC_IOC_MAGIC,  0)         // Reset records in this Linux devdrv
#define DDRC_IOC_CHG_REG        _IOW(DDRC_IOC_MAGIC, 1, int)    // Change target DDRC reg offset for R/W
#define DDRC_IOC_CHG_VAL        _IOW(DDRC_IOC_MAGIC, 2, int)    // Change value to be written to DDRC
#define DDRC_IOC_W_DDRC         _IO(DDRC_IOC_MAGIC,  3)         // Write to DDRC from Linux devdrv
#define DDRC_IOC_R_DDRC         _IOR(DDRC_IOC_MAGIC, 4, int)    // Read from DDRC to Linux devdrv
#define DDRC_IOC_R_REG_VALUE    _IOR(DDRC_IOC_MAGIC, 5, int)    // Read raw value of reg_value
#define DDRC_IOC_R_REG_OFFSET   _IOR(DDRC_IOC_MAGIC, 6, int)    // Read raw value of reg_offset

extern void ambarella_scm_ddrc_write(u32 ddrc, u32 offset, u32 val);
extern u32 ambarella_scm_ddrc_read(u32 ddrc, u32 offset);

static dev_t dev = 0;
static struct class *dev_class;
static struct cdev ddrc_cdev[DDRC_NUM];
//static unsigned long ddrc_base[DDRC_NUM] = {0};
static u32 current_mode[DDRC_NUM] = {0};    // 0: offset, 1: value
static u32 offset[DDRC_NUM] = {0};
static u32 value[DDRC_NUM] = {0};

static int ambarella_ddrc_open(struct inode *inode, struct file *file)
{
    u32 ddrc;
    u32 ret;

    ret = kstrtouint(strstr(file->f_path.dentry->d_name.name, DRV_NAME) + sizeof(DRV_NAME) - 1, 10, &ddrc);
    if(ret != 0)
        pr_err("Error: %s, line: %d, ret = %d\n", __FILE__, __LINE__, ret);

#if defined(DRV_DBG)
    pr_info("%s%d opened!\n", DRV_NAME, ddrc);
#endif
	return 0;
}

static ssize_t ambarella_ddrc_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    return 0;
}

static ssize_t ambarella_ddrc_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    return 0;
}

static long ambarella_ddrc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    u32 data;
    u32 ddrc;
    u32 ret;

    //pr_info("[DBG]: %s\n", file->f_path.dentry->d_name.name);
    ret = kstrtouint(strstr(file->f_path.dentry->d_name.name, DRV_NAME) + sizeof(DRV_NAME) - 1, 10, &ddrc);
    if(ret != 0) {
        pr_err("Error: %s, line: %d, ret = %d\n", __FILE__, __LINE__, ret);
        return ret;
    }
#if defined(DRV_DBG)
    pr_info("Targeting ioctl ddrc%d\n", ddrc);
#endif

    switch(cmd) {
        case DDRC_IOC_RESET:
            offset[ddrc] = 0;
            value[ddrc] = 0;
#if defined(DRV_DBG)
            pr_info("[%s]Reset Offset/Value = 0x0\n", DRV_NAME);
#endif
            break;
        case DDRC_IOC_CHG_REG:
            if(copy_from_user(&data, (u32*)arg, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

            current_mode[ddrc] = 0;
            offset[ddrc] = data;
#if defined(DRV_DBG)
            pr_info("[%s]Reg offset = 0x%x\n", DRV_NAME, offset);
#endif
            break;
        case DDRC_IOC_CHG_VAL:
            if(copy_from_user(&data, (u32*)arg, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

            current_mode[ddrc] = 1;
            value[ddrc] = data;
#if defined(DRV_DBG)
            pr_info("[%s]Value = 0x%x\n", DRV_NAME, value);
#endif
            break;
        case DDRC_IOC_W_DDRC:
            if(copy_from_user(&data, (u32*)arg, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

            ambarella_scm_ddrc_write(ddrc, offset[ddrc], data);
#if defined(DRV_DBG)
            pr_info("[%s]Write: 0x%x to [0x%x]\n", DRV_NAME, data, offset);
#endif
            break;
        case DDRC_IOC_R_DDRC:
            current_mode[ddrc] = 1;
            value[ddrc] = ambarella_scm_ddrc_read(ddrc, offset[ddrc]);
            data = value[ddrc];
            if(copy_to_user((u32*)arg, &data, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

#if defined(DRV_DBG)
            pr_info("[%s]Read: 0x%x\n", DRV_NAME, data);
#endif
            break;
        case DDRC_IOC_R_REG_VALUE:
            current_mode[ddrc] = 1;
            data = value[ddrc];
            if(copy_to_user((u32*)arg, &data, sizeof(data))) {
                pr_err("Error: %s, line: %d\n", __FILE__, __LINE__);
                return -1;
            }

#if defined(DRV_DBG)
            pr_info("[%s]Read value: 0x%x\n", DRV_NAME, data);
#endif
            break;
        case DDRC_IOC_R_REG_OFFSET:
            current_mode[ddrc] = 0;
            data = offset[ddrc];
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

static const struct file_operations ddrc_fops = {
    .owner = THIS_MODULE,
	.open = ambarella_ddrc_open,
    .read = ambarella_ddrc_read,
    .write = ambarella_ddrc_write,
	.unlocked_ioctl = ambarella_ddrc_ioctl,
};

static int __init ambarella_ddrc_init(void)
{
    int i, ret;
    int major;
    dev_t device;

    ret = alloc_chrdev_region(&dev, 0, DDRC_NUM, DRV_NAME);
    if(ret < 0) {
        pr_err("Cannot allocate device number...\n");
        goto r_alloc;
    }

    dev_class = class_create(THIS_MODULE, DRV_NAME);
    if(!dev_class) {
        pr_err("Cannot create device class...\n");
        goto r_class;
    }

    for(i = 0; i < DDRC_NUM; i++) {
        cdev_init(&ddrc_cdev[i], &ddrc_fops);
        major = MAJOR(dev);
        device = MKDEV(major, i);
        ret = cdev_add(&ddrc_cdev[i], device, 1);
        if(ret) {
            pr_err("%s: Failed in adding cdev to subsystem "
            "ret:%d\n", __func__, ret);
            goto r_class;
        }

        if(device_create(dev_class, NULL, device, NULL, "ddrc%d", i) == NULL)
            goto r_device;

        /*
        ddrc_base[i] = ioremap(0x1000005000 + 0x1000*i, 0x1000);
        pr_info("%s_base=0x%08lx, %s_size=0x%08lx\n", DRV_NAME, ddrc_base[i], DRV_NAME, 0x1000);
        pr_info("ioremap:%lx\n", (unsigned long)ddrc_base[i]);
        */
    }

    pr_info("Module %s init done!\n", DRV_NAME);
	return 0;

r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev, DDRC_NUM);
r_alloc:
    return -1;
}

static void __exit ambarella_ddrc_exit(void)
{
    int i = 0;
    int major;
    dev_t device;

    for (i = 0; i < DDRC_NUM; i++) {
    	cdev_del(&ddrc_cdev[i]);
        major = MAJOR(dev);
        device = MKDEV(major, i);
    	device_destroy(dev_class, device);
    }
    class_destroy(dev_class);
    unregister_chrdev_region(dev, DDRC_NUM);
    pr_info("%s: In exit\n", __func__);
}

module_init(ambarella_ddrc_init);
module_exit(ambarella_ddrc_exit);

MODULE_AUTHOR("Ambarella Inc.");
MODULE_DESCRIPTION("DDRC");
MODULE_LICENSE("GPL");

#endif

