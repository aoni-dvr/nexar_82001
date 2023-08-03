/**
 *  @file osal_linux_kernel.c
 *
 *  @copyright Copyright (c) 2019 Ambarella, Inc.
 *
 *  This file and its contents ("Software") are protected by intellectual property rights including, without limitation,
 *  U.S. and/or foreign copyrights.  This Software is also the confidential and proprietary information of Ambarella, Inc.
 *  and its licensors.  You may not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative works
 *  of this Software or any portion thereof except pursuant to a signed license agreement or nondisclosure agreement with
 *  Ambarella, Inc. or its authorized affiliates.  In the absence of such an agreement, you agree to promptly notify and
 *  return this Software to Ambarella, Inc.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details Ambarella KAL (Kernel Abstraction Layer) APIs for ThreadX
 *
 */

#include "osal_linux.h"

MODULE_AUTHOR("Sam_Syu");
MODULE_LICENSE("GPL");

#define DEVICE_NAME     "osal"

static unsigned int     osal_major;
static struct class*    osal_class;
static struct device*   osal_device;

static int osal_open(struct inode *inode, struct file *filp)
{
    uint32_t retcode = KAL_ERR_NONE;

    //printk("osal: module open\n");
    return retcode;
}

static int osal_release(struct inode *inode, struct file *filp)
{
    uint32_t retcode = KAL_ERR_NONE;

    //printk("osal: module close\n");
    return retcode;
}

static long osal_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    uint32_t retcode = KAL_ERR_NONE;
    osal_get_value_t arg_value;

    switch (cmd) {
    case OSAL_SET_ALLOW_LIST:
        retcode = copy_from_user(&arg_value, (void *)arg, sizeof(arg_value));
        if(retcode == KAL_ERR_NONE) {
            retcode = OSAL_ModuleSetAllowList(arg_value.module, arg_value.enable);
        } else {
            retcode = KAL_ERR_ARG;
        }
        break;
    default:
        printk("[ERROR] osal_ioctl() : unknown IOCTL 0x%x\n", cmd);
        retcode = KAL_ERR_NO_FEATURE;
        break;
    }

    if(retcode != KAL_ERR_NONE) {
        printk("[ERROR] osal_ioctl() : cmd 0x%x fail ret 0x%x\n", cmd, retcode);
    }
    return retcode;
}


static const struct file_operations osal_fops = {
    .owner = THIS_MODULE,
    .open = osal_open,
    .release = osal_release,
    .unlocked_ioctl = osal_ioctl,
};

static int  __init osal_init(void)
{
    osal_major = register_chrdev(0, DEVICE_NAME, &osal_fops);
    if (osal_major < 0) {
        printk("osal: failed to register device %d.\n", osal_major);
        return osal_major;
    }

    osal_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(osal_class)) {
        unregister_chrdev(osal_major, DEVICE_NAME);
        printk("osal: failed to create class.\n");
        return PTR_ERR(osal_class);
    }

    osal_device = device_create(osal_class, NULL, MKDEV(osal_major, 0),
                                NULL, DEVICE_NAME);
    if (IS_ERR(osal_device)) {
        class_destroy(osal_class);
        unregister_chrdev(osal_major, DEVICE_NAME);
        printk("osal: falied to create device.\n");
        return PTR_ERR(osal_device);
    }

    printk("osal: module init\n");
    return 0;
}

static void __exit osal_exit(void)
{
    device_destroy(osal_class, MKDEV(osal_major, 0));
    class_destroy(osal_class);
    unregister_chrdev(osal_major, DEVICE_NAME);
    printk("osal: module exit\n");
    return;
}

module_init(osal_init);
module_exit(osal_exit);
