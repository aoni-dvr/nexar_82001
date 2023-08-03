// SPDX-License-Identifier: GPL-2.0
/**
*  @file ambarella_pio.c
*
*  @copyright Copyright (c) 2021 Ambarella International LP
*
*  @details peripheral IO memory allocator
*
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/io.h>

#include <linux/uaccess.h>
#include <soc/ambarella/iav_helper.h>

#include <linux/ioctl.h>

#define PIO_MAJOR     168
#define PIO_MINOR     0
#define PIO_MAX_DEV   1

/* Clean the cache */
#define IOCTL_PIO_CLEAN_CACHE    _IOR(PIO_MAJOR, 2, void *)

/* Invalid the cache */
#define IOCTL_PIO_INV_CACHE      _IOR(PIO_MAJOR, 8, void *)

/*make sure to sync with userspace*/
typedef struct
{
  unsigned long PhyAddr;
  unsigned long VirAddr;
  unsigned int  Size;
  unsigned int  Cacheable;
} AMBA_PIO_s;

static struct class *pio_class;
static void __iomem *io;

static unsigned long pio_base = 0xDEADFACE;
static unsigned long pio_size = 0xDEADBEEF;

module_param(pio_base, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pio_base, "PIO BASE ADDRESS");
module_param(pio_size, ulong, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pio_size, "PIO MEMORY SIZE");

static long pio_CacheClean(AMBA_PIO_s *PioBuf)
{
    unsigned long offset = PioBuf->PhyAddr - pio_base;
    pr_debug("pio_CacheClean start=%lx, size=%x\n", (unsigned long)io+offset, PioBuf->Size);
    ambcache_clean_range(io+offset, PioBuf->Size);
    return 0;
}

static long pio_CacheInvalid(AMBA_PIO_s *PioBuf)
{
    unsigned long offset = PioBuf->PhyAddr - pio_base;
    pr_debug("pio_CacheInvalid start=%lx, size=%x\n", (unsigned long)io+offset, PioBuf->Size);
    ambcache_inv_range(io+offset, PioBuf->Size);
    return 0;
}

/*
 * Forward ioctls to the underlying block device.
 */
static long
pio_ioctl(struct file *filp, unsigned int command, unsigned long arg)
{
    int ret = 0;
    void __user *argp = (void __user *)arg;
    AMBA_PIO_s data;

    (void)filp;

    if (copy_from_user((void *)&data, argp, sizeof(data)) != 0) {
        printk("%s, copy_from_user err\n", __func__);
        return -EFAULT;
    }
    switch (command) {
    case IOCTL_PIO_CLEAN_CACHE:
        {
            pio_CacheClean(&data);
        }
        break;

    case IOCTL_PIO_INV_CACHE:
        {
            pio_CacheInvalid(&data);
        }
        break;

    default:
        printk("%s, Not handled. cmd: 0x%08X, arg %ld\n", __func__, command, arg);
        ret = -1;
        break;
    }

    return ret;
}

#ifndef pgprot_cachedshared
#define pgprot_cachedshared(prot) \
    __pgprot_modify(prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_NORMAL) | PTE_PXN | PTE_UXN | PROT_DEFAULT)
#endif

static DEFINE_MUTEX(amba_pio_mutex);

static int pio_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int rval = -1;
    unsigned long vma_size;

    mutex_lock(&amba_pio_mutex);

    if (filp->f_flags & O_DSYNC) {
        pr_debug("PIO: pgprot_noncached\n");
        vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
    }
    else {
        pr_debug("PIO: pgprot_cachedshared\n");
        vma->vm_page_prot = pgprot_cachedshared(vma->vm_page_prot);
    }

    // Check (last offset + request size) <= max allocated size
    vma_size = vma->vm_end - vma->vm_start;
    rval = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                        vma_size, vma->vm_page_prot);
    if (rval) {
        printk("PIO: mmap failed with error %d.\n", rval);
        goto Done;
    }

    pr_debug("PIO: remap [%x---%x] pgoff=%x size=%lx \n", (unsigned int)vma->vm_start, (unsigned int)vma->vm_end, (unsigned int)vma->vm_pgoff, vma_size);
Done:
    mutex_unlock(&amba_pio_mutex);
    return rval;
}

static const struct file_operations pio_fops = {
    .unlocked_ioctl = pio_ioctl,
    .mmap = pio_mmap,
    .owner = THIS_MODULE,
};

static struct cdev pio_cdev;

struct device *dev_pio;

static int __init pio_init(void)
{
    int ret;

    pio_class = class_create(THIS_MODULE, "pio");
    if (IS_ERR(pio_class)) {
        printk(KERN_ERR "Error creating pio class.\n");
        ret = PTR_ERR(pio_class);
        goto error;
    }
    // pio_class->devnode = pio_devnode;

    cdev_init(&pio_cdev, &pio_fops);
    ret = cdev_add(&pio_cdev, MKDEV(PIO_MAJOR, 0), 1);
    if (ret)
        goto error;
    dev_pio = device_create(pio_class, NULL, MKDEV(PIO_MAJOR, 0), NULL, "pio");
    if (IS_ERR(dev_pio)) {
        printk(KERN_ERR "Error creating pio class.\n");
        ret = PTR_ERR(dev_pio);
        goto error;
    }

    io = ioremap(pio_base, pio_size);
    printk("pio_base=0x%08lx  pio_size=0x%08lx\n", pio_base, pio_size);
    printk("ioremap:%lx\n", (unsigned long)io);

    return 0;

error:
    return ret;
}

static void __exit pio_exit(void)
{
    if (io)
        iounmap(io);
    device_destroy(pio_class, MKDEV(PIO_MAJOR, 0));
    class_destroy(pio_class);
    cdev_del(&pio_cdev);
}

module_init(pio_init);
module_exit(pio_exit);

MODULE_AUTHOR("Ambarella Inc.");
MODULE_DESCRIPTION("PIO");
MODULE_LICENSE("GPL");

