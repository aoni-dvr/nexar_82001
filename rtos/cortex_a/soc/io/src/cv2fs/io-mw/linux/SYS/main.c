/**
 *  @file main.c
 *
 *  @copyright Copyright (c) 2021 Ambarella International LP
 *
 *  This file and its contents ("Software") are protected by intellectual
 *  property rights including, without limitation, U.S. and/or foreign
 *  copyrights. This Software is also the confidential and proprietary
 *  information of Ambarella International LP and its licensors. You may not use, reproduce,
 *  disclose, distribute, modify, or otherwise prepare derivative works of this
 *  Software or any portion thereof except pursuant to a signed license agreement
 *  or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 *  In the absence of such an agreement, you agree to promptly notify and return
 *  this Software to Ambarella International LP.
 *
 *  THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 *  MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  @details SYS proc module
 *
 */

#include <linux/module.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <plat/chip.h>
#include <linux/of_address.h>

#include "osal.h"
#include "AmbaCSL_PLL.h"
#include "AmbaSYS_Priv.h"

MODULE_AUTHOR("C-J Chen");
MODULE_LICENSE("GPL");

AMBA_RCT_REG_s * pAmbaRCT_Reg;

static const char *feature_names[] = {
    "DSP", "MCTF", "CODEC", "HEVC", "CNN", "ALL",
};

static const int feature_code[] = {
    0x3144U, 0x10000U, 0x10U, 0xc000U, 0x5140000U, 0xfffffffU,
};

static int ambarella_bootmode_proc_show(struct seq_file *m, void *v)
{
    unsigned int BootDevice;

    if ( pAmbaRCT_Reg->SysConfig.UsbBoot != 0U ) {
        /* For debug purpose */
#if defined(CONFIG_ENABLE_NAND_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_NAND;
#elif defined(CONFIG_ENABLE_SPINAND_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_SPI_NAND;
#elif defined(CONFIG_ENABLE_EMMC_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_EMMC;
#elif defined(CONFIG_ENABLE_SPINOR_BOOT)
        BootDevice = AMBA_SYS_BOOT_FROM_SPI_NOR;
#else
        BootDevice = AMBA_SYS_BOOT_FROM_USB;
#endif
    } else {
        /* For normal case */
        switch (pAmbaRCT_Reg->SysConfig.BootMode) {
        case 0U:
            BootDevice = AMBA_SYS_BOOT_FROM_SPI_NOR;
            break;

        case 1U:
            if (pAmbaRCT_Reg->SysConfig.BootOption1 != 0U) {
                BootDevice = AMBA_SYS_BOOT_FROM_SPI_NAND;
            } else {
                BootDevice = AMBA_SYS_BOOT_FROM_NAND;
            }
            break;

        case 2U:
            BootDevice = AMBA_SYS_BOOT_FROM_EMMC;
            break;

        case 3U:
        default:
            BootDevice = AMBA_SYS_BOOT_FROM_EEPROM;
            break;
        }
    }
    seq_printf(m, "%d", BootDevice);
    return 0;
}

static int ambarella_sys_proc_show(struct seq_file *m, void *v)
{
    int i;
    seq_printf(m, "\nFeature Information:\n");
    unsigned int CurValue = AmbaCSL_PllGetClkCtrl();

    seq_printf(m, "\nRCT value :[0x%x]\n", CurValue);
    for (i = 0; i< ARRAY_SIZE(feature_names); i++) {
        unsigned int CmpValue = CurValue & feature_code[i];
        seq_printf(m, "\t%s:\t%s \n",
                   feature_names[i], ( CmpValue == feature_code[i] )
                   ?"enable":"disable");
    }

    return 0;
}

static ssize_t ambarella_sys_proc_write(struct file *file,
                                        const char __user *buffer, size_t count, loff_t *ppos)
{
    char *buf;
    int rval = count, freq;
    unsigned int CurValue, NewValue, SysFeature;

    //pr_warn("!!!DANGEROUS!!! You must know what you are doning!\n");

    buf = kmalloc(count, GFP_KERNEL);
    if (!buf)
        return -ENOMEM;

    if (copy_from_user(buf, buffer, count)) {
        rval = -EFAULT;
        goto exit;
    }

    sscanf(buf, "%d %d", &SysFeature, &freq);

    switch (SysFeature) {
    case AMBA_SYS_FEATURE_DSP:
        NewValue = 0x3144U;
        break;
    case AMBA_SYS_FEATURE_MCTF:
        NewValue = 0x10000U;
        break;
    case AMBA_SYS_FEATURE_CODEC:
        NewValue = 0x10U;
        break;
    case AMBA_SYS_FEATURE_HEVC:
        NewValue = 0xc000U;
        break;
    case AMBA_SYS_FEATURE_CNN:
        NewValue = 0x5140000U;
        break;
    case AMBA_SYS_FEATURE_ALL:
        NewValue = 0xfffffffU;
        break;
    default:
        NewValue = 0U;
        break;
    }

    CurValue = AmbaCSL_PllGetClkCtrl();

    if ( freq == 1U )
        AmbaCSL_PllSetClkCtrl(CurValue | NewValue);
    else
        AmbaCSL_PllSetClkCtrl(CurValue & (~(NewValue)) );

exit:
    kfree(buf);
    return rval;
}


static int ambarella_sys_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, ambarella_sys_proc_show, PDE_DATA(inode));
}

static const struct file_operations proc_clock_fops = {
    .owner = THIS_MODULE,
    .open = ambarella_sys_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .write = ambarella_sys_proc_write,
    .release = single_release,
};

static int ambarella_bootmode_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, ambarella_bootmode_proc_show, PDE_DATA(inode));
}

static const struct file_operations proc_bootmode_fops = {
    .owner = THIS_MODULE,
    .open = ambarella_bootmode_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};


#define AMBA_RCT_BASE_ADDR                      0xed080000
#define AMBA_RCT_MMAP_SIZE                      0x1000

static int __init ambarella_init_clk(void)
{
    struct device_node *np;
    struct resource res;
    void __iomem* VirtualAddr;

    np = of_find_compatible_node(NULL, NULL, "ambarella,pll-clock");
    if ( np != NULL ) {
        pr_err("%s:\n", np->name);
    } else {
        pr_err("%s: cannot access DTS for rct region\n", __func__);
        return -1;
    }

    if (of_address_to_resource(np, 0, &res))
        return -1;

    if ((VirtualAddr = ioremap_nocache(res.start,
                                       AMBA_RCT_MMAP_SIZE)) == NULL) {
        pr_err("%s: cannot access to rct region\n", __func__);
        return -1;
    } else {
        pAmbaRCT_Reg = (AMBA_RCT_REG_s *) VirtualAddr;
    }

    proc_create_data("ambasys", S_IRUGO, get_ambarella_proc_dir(),
                     &proc_clock_fops, NULL);

    proc_create_data("bootmode", S_IRUGO, get_ambarella_proc_dir(),
                     &proc_bootmode_fops, NULL);

    return 0;
}

late_initcall(ambarella_init_clk);

