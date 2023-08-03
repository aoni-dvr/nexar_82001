// SPDX-License-Identifier: GPL-2.0 OR MIT
/**
 *  @file main.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  @details HDMI device module
 *
 */

#include <linux/module.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "osal.h"
#include "proto.h"
#include "AmbaReg_VOUT.h"
#include "AmbaCSL_HDMI.h"

MODULE_AUTHOR("Ambarella");
MODULE_LICENSE("GPL");

#define HDMI_DEV_NAME "hdmi"

static unsigned int     hdmi_major;
static struct class*    hdmi_class;
static struct device*   hdmi_device;

AMBA_RCT_REG_s                  *pAmbaRCT_Reg;
AMBA_HDMI_REG_s                 *pAmbaHDMI_Reg;
AMBA_SCRATCHPAD_NS_REG_s        *pAmbaScratchpadNS_Reg;
AMBA_VOUT_TOP_REG_s             *pAmbaVoutTop_Reg;
AMBA_VOUT_DISPLAY1_CONFIG_REG_s *pAmbaVoutDisplay1_Reg;
AMBA_VOUT_TVENC_CONTROL_REG_s   *pAmbaVoutTvEnc_Reg;

extern AMBA_VOUT_DISPLAY1_CONFIG_REG_s AmbaVOUT_Display1Config;

static long hdmi_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    long err = 0;
    void __user *argp = (void __user *)arg;
    size_t nbytes;
    void *data = NULL;
    UINT32 hdmiport;

    hdmi_config_msg_t       *hdmi_config_msg;
    hdmi_sinkinfo_msg_t     *hdmi_sinkinfo_msg;
    hdmi_disp_config_msg_t  *hdmi_disp_config_msg;
    hdmi_modeinfo_msg_t     *hdmi_modeinfo_msg;

    if (_IOC_DIR(cmd) != _IOC_NONE) {
        data = kzalloc(_IOC_SIZE(cmd), GFP_KERNEL);
        if (data == NULL) {
            return -ENOMEM;
        }
    }

    if (_IOC_DIR(cmd) & _IOC_WRITE) {
        if (copy_from_user(data, argp, _IOC_SIZE(cmd)) != 0) {
            kfree(data);
            return -EFAULT;
        }
    }

    nbytes = 0;
    switch (cmd) {
    case HDMI_IOC_TX_ENABLE:
        hdmi_config_msg = data;
        hdmiport = hdmi_config_msg->HdmiPort;

        if (hdmi_TxEnable(hdmiport) != 0)
            err = -EINVAL;
        break;

    case HDMI_IOC_TX_DISABLE:
        hdmi_config_msg = data;
        hdmiport = hdmi_config_msg->HdmiPort;

        if (hdmi_TxDisable(hdmiport) != 0)
            err = -EINVAL;
        break;

    case HDMI_IOC_TX_GET_SINKINFO:
        hdmi_sinkinfo_msg = data;
        if (hdmi_TxGetSinkInfo(hdmi_sinkinfo_msg->HdmiPort, &hdmi_sinkinfo_msg->HotPlugDetect, &hdmi_sinkinfo_msg->EdidDetect, &(hdmi_sinkinfo_msg->SinkInfo)) != 0)
            err = -EINVAL;
        break;

    case HDMI_IOC_TX_SET_MODE:
        hdmi_config_msg = data;
        if (hdmi_TxSetMode(hdmi_config_msg->HdmiPort, &(hdmi_config_msg->VideoConfig), &(hdmi_config_msg->AudioConfig), &(hdmi_config_msg->WorkingVideoConfig)) != 0) {
            err = -EINVAL;
        } else {
            nbytes = sizeof(hdmi_config_msg_t);
        }

        break;

    case HDMI_IOC_TX_GET_CURR_MODE:
        hdmi_modeinfo_msg = data;
        if (hdmi_TxGetCurrModeInfo(hdmi_modeinfo_msg->HdmiPort, &(hdmi_modeinfo_msg->ModeInfo), &(hdmi_modeinfo_msg->StrictFrameRate), &(hdmi_modeinfo_msg->FrameWidth), &(hdmi_modeinfo_msg->FrameHeight)) != 0) {
            err = -EINVAL;
        } else {
            nbytes = sizeof(hdmi_modeinfo_msg_t);
        }
        break;

    case HDMI_IOC_TX_START:
        hdmi_config_msg = data;
        if (hdmi_TxStart(hdmi_config_msg->HdmiPort) != 0) {
            err = -EINVAL;
        }
        break;

    case HDMI_IOC_TX_STOP:
        hdmi_config_msg = data;
        if (hdmi_TxStop(hdmi_config_msg->HdmiPort) != 0) {
            err = -EINVAL;
        }
        break;

    case HDMI_IOC_TX_SET_AVMUTE:
        hdmi_config_msg = data;
        if (hdmi_TxSetAvMute(hdmi_config_msg->HdmiPort) != 0) {
            err = -EINVAL;
        }
        break;

    case HDMI_IOC_TX_CLEAR_AVMUTE:
        hdmi_config_msg = data;
        if (hdmi_TxClearAvMute(hdmi_config_msg->HdmiPort) != 0) {
            err = -EINVAL;
        }
        break;

    case HDMI_IOC_TX_GET_DISP_CONFIG:
        hdmi_disp_config_msg = data;

        /* copy data */
        memcpy(hdmi_disp_config_msg->Out, &AmbaVOUT_Display1Config, sizeof(AMBA_VOUT_DISPLAY1_CONFIG_REG_s));
        nbytes = sizeof(hdmi_disp_config_msg_t);

        break;

    case HDMI_IOC_TX_ENABLE_TESTPATTERN:
        hdmi_disp_config_msg = data;

        AmbaCSL_HdmiSetVideoDataSource(1);
        break;
    }

    if ((_IOC_DIR(cmd) & _IOC_READ) && (nbytes != 0)) {
        if (copy_to_user(argp, data, nbytes) != 0) {
            err = -EFAULT;
        }
    }

    if (data != NULL) {
        kfree(data);
    }

    return (err);
}

static const struct file_operations hdmi_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = hdmi_ioctl,
};

static int __init hdmi_init(void)
{
    printk("hdmi: module init\n");

    hdmi_major = register_chrdev(0, HDMI_DEV_NAME, &hdmi_fops);
    if (hdmi_major < 0) {
        printk("hdmi: failed to register device %d.\n", hdmi_major);
        return hdmi_major;
    }

    hdmi_class = class_create(THIS_MODULE, HDMI_DEV_NAME);
    if (IS_ERR(hdmi_class)) {
        unregister_chrdev(hdmi_major, HDMI_DEV_NAME);
        printk("hdmi: failed to create class.\n");
        return PTR_ERR(hdmi_class);
    }

    hdmi_device = device_create(hdmi_class,
                                NULL,
                                MKDEV(hdmi_major, 0),
                                NULL,
                                HDMI_DEV_NAME);

    if (IS_ERR(hdmi_device)) {
        class_destroy(hdmi_class);
        unregister_chrdev(hdmi_major, HDMI_DEV_NAME);
        printk("hdmi: falied to create device.\n");
        return PTR_ERR(hdmi_device);
    }

    if (hdmi_dev_init() != 0U) {
        printk("hdmi_dev init fail %s(%d)\n", __func__, __LINE__);
    }

    return 0;
}

static void __exit hdmi_exit(void)
{
    hdmi_dev_dinit();

    hdmi_device->bus = NULL; /* Remove bus link */
    device_destroy(hdmi_class, MKDEV(hdmi_major, 0));
    class_destroy(hdmi_class);
    unregister_chrdev(hdmi_major, HDMI_DEV_NAME);
    printk("hdmi: module exit\n");
    return;
}

module_init(hdmi_init);
module_exit(hdmi_exit);

