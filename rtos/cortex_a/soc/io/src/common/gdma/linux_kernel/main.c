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
 *  @details GDMA device module
 *
 */

#include <linux/module.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/io.h>
#if defined(CONFIG_ARCH_AMBARELLA_CV5)
#include "AmbaCortexA76.h"
#define AMBA_GDMA_BASE  AMBA_CORTEX_A76_GDMA_BASE_ADDR
#else
#include "AmbaCortexA53.h"
#define AMBA_GDMA_BASE  AMBA_CORTEX_A53_GDMA_BASE_ADDR
#endif
#include "hw/gdma.h"
#include "osal.h"
#include "AmbaRTSL_GDMA.h"
#include "AmbaCSL_GDMA.h"

MODULE_AUTHOR("Sam Syu");
MODULE_LICENSE("GPL");

#define GDMA_DEV_NAME "gdma"

static unsigned int     gdma_major;
static struct class*    gdma_class;
static struct device*   gdma_device;


#define AMBA_GDMA_SIZE  0x1000

AMBA_GDMA_REG_s *pAmbaGDMA_Reg;

static AMBA_KAL_EVENT_FLAG_t AmbaGdmaEventFlags;

static void GDMA_TransferDoneISR(UINT32 Arg)
{
    OSAL_EventFlagSet(&AmbaGdmaEventFlags, (Arg));
}

static void GdmaWaitComplete(UINT32 TimeOut)
{
    UINT32 ActualFlags = 0;
    UINT32 Remain = 0U;
    do {
        OSAL_EventFlagGet(&AmbaGdmaEventFlags,
                          0x1U,
                          KAL_FLAGS_WAIT_ANY,
                          KAL_FLAGS_CLEAR_AUTO,
                          &ActualFlags,
                          0x10);
        Remain += 0x10;
        if(Remain > TimeOut) {
            break;
        }
    } while (AmbaRTSL_GdmaGetNumAvails() != GDMA_NUM_INSTANCE);
}

static void GdmaWaitAvaliable(UINT32 TimeOut)
{
    UINT32 ActualFlags = 0;
    UINT32 Remain = 0U;
    do {
        OSAL_EventFlagGet(&AmbaGdmaEventFlags,
                          0x1U,
                          KAL_FLAGS_WAIT_ANY,
                          AMBA_KAL_FLAGS_CLEAR_NONE,
                          &ActualFlags,
                          0x10);
        Remain += 0x10;
        if(Remain > TimeOut) {
            break;
        }
    } while (AmbaRTSL_GdmaGetNumAvails() != 0x0);
}
static long gdma_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    long err = 0;
    void __user *argp = (void __user *)arg;
    void *data = NULL;
    amba_gdma_linear_t *pGdmaLinear;
    amba_gdma_block_t *pGdmaBlock;
    amba_gdma_wait_t *pGdmaWait;
    AMBA_GDMA_LINEAR_s GdmaLiear = {0};
    AMBA_GDMA_BLOCK_s GdmaBlock = {0};


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

    if (0x0 == (AmbaRTSL_GdmaGetNumAvails())) {
        GdmaWaitAvaliable(AMBA_KAL_WAIT_FOREVER);
    }

    switch (cmd) {
    case DCMD_GDMA_LINEAR:
        pGdmaLinear = data;
        GdmaLiear.NumPixels = pGdmaLinear->NumPixels;
        OSAL_TypeCast(&GdmaLiear.pSrcImg,&pGdmaLinear->SrcImg);
        OSAL_TypeCast(&GdmaLiear.pDstImg,&pGdmaLinear->DstImg);
        GdmaLiear.PixelFormat = pGdmaLinear->PixelFormat;
        err = (long) AmbaRTSL_GdmaLinearCopy(&GdmaLiear);
        break;

    case DCMD_GDMA_BLOCK:
        pGdmaBlock = data;
        OSAL_TypeCast(&GdmaBlock.pSrcImg,&pGdmaBlock->SrcImg);
        OSAL_TypeCast(&GdmaBlock.pDstImg,&pGdmaBlock->DstImg);
        GdmaBlock.SrcRowStride = pGdmaBlock->SrcRowStride;
        GdmaBlock.DstRowStride = pGdmaBlock->DstRowStride;
        GdmaBlock.BltWidth = pGdmaBlock->BltWidth;
        GdmaBlock.BltHeight = pGdmaBlock->BltHeight;
        GdmaBlock.PixelFormat = pGdmaBlock->PixelFormat;
        err = (long) AmbaRTSL_GdmaBlockCopy(&GdmaBlock);
        break;

    case DCMD_GDMA_COLOTKEY:
        pGdmaBlock = data;
        OSAL_TypeCast(&GdmaBlock.pSrcImg,&pGdmaBlock->SrcImg);
        OSAL_TypeCast(&GdmaBlock.pDstImg,&pGdmaBlock->DstImg);
        GdmaBlock.SrcRowStride = pGdmaBlock->SrcRowStride;
        GdmaBlock.DstRowStride = pGdmaBlock->DstRowStride;
        GdmaBlock.BltWidth = pGdmaBlock->BltWidth;
        GdmaBlock.BltHeight = pGdmaBlock->BltHeight;
        GdmaBlock.PixelFormat = pGdmaBlock->PixelFormat;
        err = (long) AmbaRTSL_GdmaColorKeying(&GdmaBlock, pGdmaBlock->TransparentColor);
        break;

    case DCMD_GDMA_ALPHABLEND:
        pGdmaBlock = data;
        OSAL_TypeCast(&GdmaBlock.pSrcImg,&pGdmaBlock->SrcImg);
        OSAL_TypeCast(&GdmaBlock.pDstImg,&pGdmaBlock->DstImg);
        GdmaBlock.SrcRowStride = pGdmaBlock->SrcRowStride;
        GdmaBlock.DstRowStride = pGdmaBlock->DstRowStride;
        GdmaBlock.BltWidth = pGdmaBlock->BltWidth;
        GdmaBlock.BltHeight = pGdmaBlock->BltHeight;
        GdmaBlock.PixelFormat = pGdmaBlock->PixelFormat;
        err = (long) AmbaRTSL_GdmaAlphaBlending(&GdmaBlock, pGdmaBlock->AlphaVal, pGdmaBlock->BlendMode);
        break;

    case DCMD_GDMA_WAIT_COMPLETE:
        pGdmaWait = data;

        GdmaWaitComplete(pGdmaWait->TimeOut);
        break;
    default:
        err = -ENOTTY;
        break;
    }

    if (data != NULL) {
        kfree(data);
    }
    return 0;
}

static const struct file_operations gdma_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = gdma_ioctl,
};

static int __init gdma_init(void)
{
    void __iomem* VirtualAddr;

    printk("gdma: module init\n");

    gdma_major = register_chrdev(0, GDMA_DEV_NAME, &gdma_fops);
    if (gdma_major < 0) {
        printk("gdma: failed to register device %d.\n", gdma_major);
        return gdma_major;
    }

    gdma_class = class_create(THIS_MODULE, GDMA_DEV_NAME);
    if (IS_ERR(gdma_class)) {
        unregister_chrdev(gdma_major, GDMA_DEV_NAME);
        printk("gdma: failed to create class.\n");
        return PTR_ERR(gdma_class);
    }

    gdma_device = device_create(gdma_class,
                                NULL,
                                MKDEV(gdma_major, 0),
                                NULL,
                                GDMA_DEV_NAME);

    if (IS_ERR(gdma_device)) {
        class_destroy(gdma_class);
        unregister_chrdev(gdma_major, GDMA_DEV_NAME);
        printk("gdma: falied to create device.\n");
        return PTR_ERR(gdma_device);
    }

    if ((VirtualAddr = ioremap_nocache(AMBA_GDMA_BASE,
                                       AMBA_GDMA_SIZE)) == NULL) {
        pr_err("%s: cannot access to gdma region\n", __func__);
        return -1;
    } else {
        pAmbaGDMA_Reg = (AMBA_GDMA_REG_s *) VirtualAddr;
    }

    OSAL_EventFlagCreate(&AmbaGdmaEventFlags, NULL);
    AmbaRTSL_GdmaHookIntHandler(GDMA_TransferDoneISR);
    return 0;
}

static void __exit gdma_exit(void)
{
    iounmap((void __iomem*)pAmbaGDMA_Reg);

    gdma_device->bus = NULL; /* Remove bus link */
    device_destroy(gdma_class, MKDEV(gdma_major, 0));
    class_destroy(gdma_class);
    unregister_chrdev(gdma_major, GDMA_DEV_NAME);
    printk("gdma: module exit\n");
    return;
}

module_init(gdma_init);
module_exit(gdma_exit);
