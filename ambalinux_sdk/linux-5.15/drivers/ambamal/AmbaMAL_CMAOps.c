/**
 *  @file AmbaMAL_Dev.c
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
 *  @details Ambarella MAL (Memory Abstraction Layer) APIs
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_device.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include "AmbaMAL.h"
#include "AmbaMAL_KrnPriv.h"
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,10,0)
#include <plat/iav_helper.h>
#else
#include <soc/ambarella/iav_helper.h>
#endif

#define DEVICE_NAME     "ambamalc"

static INT32           AmbaMalCmaMajor = -1;
static struct class*    AmbaMalCmaClass;

static const struct file_operations ambamal_cma_fops = {
    .owner = THIS_MODULE,
};

UINT32 AmbaMAL_KrnCmaOpsInit(void)
{
    if (AmbaMalCmaMajor < 0) {
        AmbaMalCmaMajor = register_chrdev(0, DEVICE_NAME, &ambamal_cma_fops);
        if (AmbaMalCmaMajor < 0) {
            printk("[ERROR] AmbaMAL_KrnNodeCMAInit() : failed to register device %d.\n", AmbaMalCmaMajor);
            return 1;
        }

        AmbaMalCmaClass = class_create(THIS_MODULE, DEVICE_NAME);
        if (IS_ERR(AmbaMalCmaClass)) {
            unregister_chrdev(AmbaMalCmaMajor, DEVICE_NAME);
            AmbaMalCmaMajor = -1;
            printk("[ERROR] AmbaMAL_KrnNodeCMAInit() : failed to create class.\n");
            return 1;
        }
    }

    return 0U;
}

void AmbaMAL_KrnCmaOpsDeInit(void)
{
    if (AmbaMalCmaMajor >= 0) {
        device_destroy(AmbaMalCmaClass, MKDEV(AmbaMalCmaMajor, 0));
        class_destroy(AmbaMalCmaClass);
        unregister_chrdev(AmbaMalCmaMajor, DEVICE_NAME);
        AmbaMalCmaMajor = -1;
    }
}

UINT32 AmbaMAL_KrnCmaOpsRegister(AMBA_MAL_INFO_PRIV_s *pPriv, struct device_node *pNode)
{
    INT32 Rval = 0;
    struct device *pDev = NULL;

    pDev = device_create(AmbaMalCmaClass, AmbaMalDevice,
                         MKDEV(AmbaMalCmaMajor, pPriv->Info.Id), NULL,
                         "ambamalc%d", pPriv->Info.Id);
    if (IS_ERR(pDev)) {
        printk("[ERROR] AmbaMAL_KrnNodeCMAInit() : Id (%d) falied to create device \n",pPriv->Info.Id);
        Rval = -1;
    }
    if(Rval == 0) {
        pDev->of_node = pNode;
        pDev->bus     = &platform_bus_type;
        pDev->coherent_dma_mask = DMA_BIT_MASK(40);
        if (!pDev->dma_mask) {
            pDev->dma_mask = &pDev->coherent_dma_mask;
        }
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
        Rval = of_dma_configure(pDev, pDev->of_node);
#else
        Rval = of_dma_configure(pDev, pDev->of_node,true);
#endif
        if (Rval < 0) {
            printk("[ERROR] AmbaMAL_KrnNodeCMAInit() : Id (%d) of_dma_configure fail.\n",pPriv->Info.Id);
        } else {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5,4,0)
            pDev->archdata.dma_coherent = 1;
#else
#if (defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_DEVICE) || defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU) || defined(CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU_ALL))
            pDev->dma_coherent = 1;
#endif /* (?CONFIG_ARCH_HAS_SYNC_DMA_FOR_DEVICE || ?CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU || ?CONFIG_ARCH_HAS_SYNC_DMA_FOR_CPU_ALL) */
#endif
            Rval = of_reserved_mem_device_init(pDev);
            if (Rval < 0) {
                printk("[ERROR] AmbaMAL_KrnNodeCMAInit() : Id (%d) of_reserved_mem_device_init fail.\n",pPriv->Info.Id);
            }
        }

    }

    if (Rval == 0) {
        pPriv->pDev = pDev;
    } else {
        if(pDev != NULL) {
            device_destroy(AmbaMalCmaClass,
                           MKDEV(AmbaMalCmaMajor, pPriv->Info.Id));
        }
    }

    return Rval;
}

void* AmbaMAL_KrnCmaOpsAlloc(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 Size, UINT64 *pPhysAddr)
{
    return dma_alloc_wc(pPriv->pDev,Size, pPhysAddr, GFP_KERNEL);
}

void AmbaMAL_KrnCmaOpsFree(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void *pVirtAddr, UINT64 RealSize)
{
    dma_free_wc(pPriv->pDev, RealSize, pVirtAddr, PhysAddr);
}

