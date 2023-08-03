// SPDX-License-Identifier: GPL-2.0
/**
*  @file ambarella_rct_profile.c
*
*  @copyright Copyright (c) 2021 Ambarella International LP
*
*  @details profile by RCT
*
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>

#include <linux/uaccess.h>
#include <soc/ambarella/iav_helper.h>

#include <linux/ioctl.h>

#define RCT_PROFILE_MAJOR     169
#define RCT_PROFILE_MINOR     0
#define RCT_PROFILE_MAX_DEV   1
#define IOCTL_RCT_PROFILE_GET_LOAD_TIME      _IOR(RCT_PROFILE_MAJOR, 2, void *)
#define IOCTL_RCT_PROFILE_GET_BOOT_TIME      _IOR(RCT_PROFILE_MAJOR, 3, void *)

#define A53_BLD_LOAD_LINUX_START             (13U)
#define A53_BLD_LOAD_LINUX_DONE              (14U)
#define DRAM_RCT_PROFILE_ADDRESS             (0x00002C00U)
#define TIMER_INFO_LOAD_TIME_START_ADDR      (DRAM_RCT_PROFILE_ADDRESS + A53_BLD_LOAD_LINUX_START*sizeof(uint))
#define TIMER_INFO_LOAD_TIME_DONE_ADDR       (DRAM_RCT_PROFILE_ADDRESS + A53_BLD_LOAD_LINUX_DONE*sizeof(uint))
#define RCT_TIMER1_COUNTER_OFFSET            (0x494U)

#define AMBA_SYS_BOOT_CLK_MHz                (24U)

static struct class *rct_profile_class;
static void __iomem *io_timerInfo_ld_start;
static void __iomem *io_timerInfo_ld_done;
static void __iomem *io_timer1;

static uint rct_timerInfo_ld = 0U;
static uint rct_timer1 = 0U;

module_param(rct_timerInfo_ld, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(rct_timerInfo_ld, "RCT TIMER INFO LOAD TIME DONE BASE ADDRESS");

static int rct_profile_get_boot_tick(char *buffer, const struct kernel_param *kp)
{
    uint timer1_us = ioread32(io_timer1) / AMBA_SYS_BOOT_CLK_MHz;
    return sprintf(buffer, "%u\n", timer1_us);
}

static struct kernel_param_ops param_ops_rct_timer1 = {
    .get = rct_profile_get_boot_tick,
};

module_param_cb(rct_timer1, &param_ops_rct_timer1, &(rct_timer1), 0644);
MODULE_PARM_DESC(rct_timer1, "RCT TIMER1 BASE ADDRESS");

static ulong parse_and_get_addr(const char *compatible_str)
{
    struct device_node *node;
    const unsigned int *reg;
    ulong addr = 0xDEADFACE;

    node = of_find_compatible_node(NULL, NULL, compatible_str);
    if (node) {
        reg = of_get_property(node, "reg", NULL);
        addr = of_translate_address(node, reg);
    }
    else {
        printk("error: not found %s in dt\n", compatible_str);
    }
    return addr;
}

static void rct_profile_get_load_tick(void)
{
    uint r1, r2;
    r1 = ioread32(io_timerInfo_ld_start);
    r2 = ioread32(io_timerInfo_ld_done);
    if (r1 > r2) {
        printk("error: load os start time > end time\n");
    }
    else {
        rct_timerInfo_ld = (r2 - r1) / AMBA_SYS_BOOT_CLK_MHz;
    }
}

static struct cdev rct_profile_cdev;
struct device *dev_rct_profile;

static int __init rct_profile_init(void)
{
    int ret;
    ulong rct_timer1_base;

    rct_profile_class = class_create(THIS_MODULE, "rct_profile");
    if (IS_ERR(rct_profile_class)) {
        printk(KERN_ERR "Error creating rct_profile class.\n");
        ret = PTR_ERR(rct_profile_class);
        goto error;
    }
    // rct_profile_class->devnode = rct_profile_devnode;

    cdev_init(&rct_profile_cdev, NULL);
    ret = cdev_add(&rct_profile_cdev, MKDEV(RCT_PROFILE_MAJOR, 0), 1);
    if (ret)
        goto error;
    dev_rct_profile = device_create(rct_profile_class, NULL, MKDEV(RCT_PROFILE_MAJOR, 0), NULL, "rct_profile");
    if (IS_ERR(dev_rct_profile)) {
        printk(KERN_ERR "Error creating rct_profile class.\n");
        ret = PTR_ERR(dev_rct_profile);
        goto error;
    }

    /* parse base addr from device tree */
    rct_timer1_base = parse_and_get_addr("ambarella,rct");

    /* map physical address for kernel */
    io_timerInfo_ld_start = ioremap(TIMER_INFO_LOAD_TIME_START_ADDR, 0x4);
    io_timerInfo_ld_done = ioremap(TIMER_INFO_LOAD_TIME_DONE_ADDR, 0x4);
    io_timer1 = ioremap(rct_timer1_base+RCT_TIMER1_COUNTER_OFFSET, 0x4);

    printk("rct_profile init: get load time at 0x%08lx and 0x%08lx\n", TIMER_INFO_LOAD_TIME_START_ADDR, TIMER_INFO_LOAD_TIME_DONE_ADDR);
    printk("rct_profile init: get boot time at 0x%08lx\n", rct_timer1_base+RCT_TIMER1_COUNTER_OFFSET);
    // printk("[DBG] ioremap:0x%lx 0x%lx 0x%lx\n", (ulong)io_timerInfo_ld_start, (ulong)io_timerInfo_ld_done, (ulong)io_timer1);

    rct_profile_get_load_tick();

    return 0;

error:
    return ret;
}

static void __exit rct_profile_exit(void)
{
    if (io_timerInfo_ld_start) {
        iounmap(io_timerInfo_ld_start);
    }
    if (io_timerInfo_ld_done) {
        iounmap(io_timerInfo_ld_done);
    }
    if (io_timer1) {
        iounmap(io_timer1);
    }
    device_destroy(rct_profile_class, MKDEV(RCT_PROFILE_MAJOR, 0));
    class_destroy(rct_profile_class);
    cdev_del(&rct_profile_cdev);
}

module_init(rct_profile_init);
module_exit(rct_profile_exit);

MODULE_AUTHOR("Ambarella Inc.");
MODULE_DESCRIPTION("RCT_PROFILE");
MODULE_LICENSE("GPL");
