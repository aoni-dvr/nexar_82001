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

static inline UINT64 AmaBitmapMaxno(UINT64 Count, UINT32 OrderPerBit)
{
    return Count >> OrderPerBit;
}

static UINT64 AmaBitmapAlignedMask(UINT32 AlignOrder, UINT32 OrderPerBit)
{
    if (AlignOrder <= OrderPerBit) {
        return 0U;
    } else {
        return (1UL << (AlignOrder - OrderPerBit)) - 1U;
    }
}

static UINT64 AmaBitmapAlignedOffset(UINT64 BasePfn, UINT32 AlignOrder, UINT32 OrderPerBit)
{
    return (BasePfn & ((1UL << AlignOrder) - 1U)) >> OrderPerBit;
}

static UINT64 AmaBitmapPagesToBits(UINT64 pages, UINT32 OrderPerBit)
{
    return ALIGN(pages, 1UL << OrderPerBit) >> OrderPerBit;
}

UINT32 AmbaMAL_KrnAmaOpsRegister(AMBA_MAL_INFO_PRIV_s *pPriv, struct device_node *pNode)
{
    INT32 Rval = 0;
    pgprot_t Prot = __pgprot(PROT_NORMAL);

    pPriv->Ama.pVirtAddr = __ioremap(pPriv->Info.PhysAddr, pPriv->Info.Size, Prot);
    if (pPriv->Ama.pVirtAddr != NULL) {
        pPriv->Ama.BasePfn = pPriv->Info.PhysAddr >> PAGE_SHIFT;
        pPriv->Ama.Count = pPriv->Info.Size >> PAGE_SHIFT;
        pPriv->Ama.OrderPerBit = 0U;   // 0: 1 bit reperesent 1 page; 1: 1 bit reperesent 2 page.
        pPriv->Ama.Align = 0U;         /* set it as zero */
        mutex_init(&pPriv->Ama.AmaMutex);

        pPriv->Ama.pBitmap = bitmap_zalloc(AmaBitmapMaxno(pPriv->Ama.Count, pPriv->Ama.OrderPerBit), GFP_KERNEL);
        if (pPriv->Ama.pBitmap == NULL) {
            printk("[ERROR] AmbaMAL_KrnAmaOpsRegister() : Id (%d) bitmap_zalloc fail  \n",pPriv->Info.Id);
            Rval = 1;
        }
    } else {
        printk("[ERROR] AmbaMAL_KrnAmaOpsRegister() : Id (%d) __ioremap fail  \n",pPriv->Info.Id);
        Rval = 1;
    }

    return Rval;
}

void* AmbaMAL_KrnAmaOpsAlloc(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 Size, UINT64 *pPhysAddr)
{
    UINT64 Mask, Offset;
    UINT64 BitmapMaxNo, BitmapNo, BitmapCount;
    void *pVirtAddr = NULL;
    UINT64 Count = Size >> PAGE_SHIFT;

    Mask = AmaBitmapAlignedMask(pPriv->Ama.Align, pPriv->Ama.OrderPerBit);
    Offset = AmaBitmapAlignedOffset(pPriv->Ama.BasePfn, pPriv->Ama.Align, pPriv->Ama.OrderPerBit);
    BitmapMaxNo = AmaBitmapMaxno(pPriv->Ama.Count, pPriv->Ama.OrderPerBit);
    BitmapCount = AmaBitmapPagesToBits(Count, pPriv->Ama.OrderPerBit);

    mutex_lock(&pPriv->Ama.AmaMutex);
    BitmapNo = bitmap_find_next_zero_area_off(pPriv->Ama.pBitmap,BitmapMaxNo, 0, BitmapCount, Mask, Offset);
    if (BitmapNo >= BitmapMaxNo) { /* Fail */
        printk("[ERROR] AmbaMAL_KrnAmaOpsAlloc() : Id (%d) alloc fail BitmapNo %lld ,BitmapMaxNo %lld\n",pPriv->Info.Id,BitmapNo,BitmapMaxNo);
        mutex_unlock(&pPriv->Ama.AmaMutex);
    } else {
        bitmap_set(pPriv->Ama.pBitmap, BitmapNo, BitmapCount);
        mutex_unlock(&pPriv->Ama.AmaMutex);
        pVirtAddr = pPriv->Ama.pVirtAddr + ((BitmapNo << pPriv->Ama.OrderPerBit) << PAGE_SHIFT);
        *pPhysAddr = pPriv->Info.PhysAddr + ((BitmapNo << pPriv->Ama.OrderPerBit) << PAGE_SHIFT);
    }

    return pVirtAddr;

}

void AmbaMAL_KrnAmaOpsFree(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void *pVirtAddr, UINT64 RealSize)
{
    UINT64 Pfn = PhysAddr >> PAGE_SHIFT;
    UINT64 Count = RealSize >> PAGE_SHIFT;
    UINT64 BitmapNo, BitmapCount;

    BitmapNo = (Pfn - pPriv->Ama.BasePfn) >> pPriv->Ama.OrderPerBit;
    BitmapCount = AmaBitmapPagesToBits(Count,pPriv->Ama.OrderPerBit);

    mutex_lock(&pPriv->Ama.AmaMutex);
    bitmap_clear(pPriv->Ama.pBitmap, BitmapNo, BitmapCount);
    mutex_unlock(&pPriv->Ama.AmaMutex);
}


