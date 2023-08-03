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

UINT32 AmbaMAL_KrnPhys2Global(UINT64 PhysAddr, UINT64 *pGlobalAddr)
{
    UINT32 Ret = 0U;
    UINT32 Found = 0U;
    UINT32 i;

    *pGlobalAddr = 0xFF00000000000000UL;
    for (i = 0U; i < AMBA_MAL_SEG_NUM; i++) {
        if (0x0U != AmbaMalSegList[i].Size) {
            if ((PhysAddr >= AmbaMalSegList[i].PhysAddr) && (PhysAddr < AmbaMalSegList[i].PhysAddr + AmbaMalSegList[i].Size)) {
                *pGlobalAddr = AmbaMalSegList[i].GlobalAddr + (PhysAddr - AmbaMalSegList[i].PhysAddr);
                Found = 1U;
                break;
            }
        }
    }

    if(Found == 0U) {
        Ret = 1U;
    }
    return Ret;
}

UINT32 AmbaMAL_KrnGlobal2Phys(UINT64 GlobalAddr, UINT64 *pPhysAddr)
{
    UINT32 Ret = 0U;
    UINT32 Found = 0U;
    UINT32 i;

    *pPhysAddr = 0UL;
    for (i = 0U; i < AMBA_MAL_SEG_NUM; i++) {
        if (0x0U != AmbaMalSegList[i].Size) {
            if ((GlobalAddr >= AmbaMalSegList[i].GlobalAddr) && (GlobalAddr < AmbaMalSegList[i].GlobalAddr + AmbaMalSegList[i].Size)) {
                *pPhysAddr = AmbaMalSegList[i].PhysAddr + (GlobalAddr - AmbaMalSegList[i].GlobalAddr);
                Found = 1U;
                break;
            }
        }
    }

    if(Found == 0U) {
        Ret = 1U;
    }
    return Ret;
}

UINT32 AmbaMAL_KrnCmaAlloc(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 Size, UINT32 Align, UINT64 *pPhysAddr, UINT64 *pRealSize, void *Owner)
{
    UINT32 Ret = 0U;
    UINT64 AlignSize = 0UL;
    UINT64 PhysAddr = 0UL;
    void *pVirtAddr = NULL;
    AMBA_MAL_MMB_s *pMmb;

    AlignSize = PAGE_ALIGN(Size);
    if(pPriv->IsAma == 0U) {
        pVirtAddr = AmbaMAL_KrnCmaOpsAlloc(pPriv, AlignSize, &PhysAddr);
    } else {
        pVirtAddr = AmbaMAL_KrnAmaOpsAlloc(pPriv, AlignSize, &PhysAddr);
    }
    if ((pVirtAddr != NULL) && (PhysAddr != 0)) {
        Ret = AmbaMAL_KrnMmbAdd(pPriv, PhysAddr, pVirtAddr, AlignSize, Owner);
        if(Ret == 0U) {
            Ret = AmbaMAL_KrnMmbReference(pPriv, PhysAddr, AlignSize, &pMmb);
            if(Ret == 0U) {
                *pPhysAddr = PhysAddr;
                *pRealSize = AlignSize;
                printk(KERN_DEBUG "[KERN_DEBUG] AmbaMAL_KrnCmaAlloc() : Id [%d] PhysAddr 0x%llx Size 0x%llx\n",pPriv->Info.Id, PhysAddr, AlignSize);
            } else {
                printk("[ERROR] AmbaMAL_KrnCmaAlloc() : Id [%d] AmbaMAL_KrnMmbReference fail size: 0x%llx \n",pPriv->Info.Id, Size);
            }
        } else {
            if(pPriv->IsAma == 0U) {
                AmbaMAL_KrnCmaOpsFree(pPriv, PhysAddr, pVirtAddr, AlignSize);
            } else {
                AmbaMAL_KrnAmaOpsFree(pPriv, PhysAddr, pVirtAddr, AlignSize);
            }
            printk("[ERROR] AmbaMAL_KrnCmaAlloc() : Id [%d] AmbaMAL_KrnMmbAdd fail size: 0x%llx \n",pPriv->Info.Id, Size);
        }
    } else {
        printk("[ERROR] AmbaMAL_KrnCmaAlloc() : Id [%d] dma alloc fail size: 0x%llx \n",pPriv->Info.Id, Size);
        Ret = 1U;
    }

    return Ret;
}

UINT32 AmbaMAL_KrnCmaFree(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 RealSize)
{
    UINT32 Ret = 0U;
    UINT32 Rlease = 0U;
    AMBA_MAL_MMB_s Mmb;

    Ret = AmbaMAL_KrnMmbUnReference(pPriv, PhysAddr, RealSize, &Rlease, &Mmb);
    if(Ret == 0U) {
        if(Rlease == 1U) {
            Ret = AmbaMAL_KrnMmbRelease(pPriv, PhysAddr, RealSize);
            if(Ret == 0U) {
                if(pPriv->IsAma == 0U) {
                    AmbaMAL_KrnCmaOpsFree(pPriv, Mmb.PhysAddr, Mmb.pVirtAddr, Mmb.Size);
                } else {
                    AmbaMAL_KrnAmaOpsFree(pPriv, Mmb.PhysAddr, Mmb.pVirtAddr, Mmb.Size);
                }
                printk(KERN_DEBUG "[KERN_DEBUG] AmbaMAL_KrnCmaFree() : Id [%d] PhysAddr 0x%llx Size 0x%llx\n",pPriv->Info.Id, PhysAddr, RealSize);
            } else {
                printk("[ERROR] AmbaMAL_KrnCmaFree() : Id [%d] AmbaMAL_KrnMmbRelease fail \n",pPriv->Info.Id);
            }
        }
    } else {
        printk("[ERROR] AmbaMAL_KrnCmaFree() : Id [%d] AmbaMAL_KrnMmbUnReference fail \n",pPriv->Info.Id);
    }

    return Ret;
}

void AmbaMAL_KrnCmaReleaseCb(AMBA_MAL_INFO_PRIV_s *pPriv, AMBA_MAL_MMB_s *pMmb)
{
    if(pPriv->IsAma == 0U) {
        AmbaMAL_KrnCmaOpsFree(pPriv, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size);
    } else {
        AmbaMAL_KrnAmaOpsFree(pPriv, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size);
    }
    printk(KERN_DEBUG "[KERN_DEBUG] AmbaMAL_KrnCmaReleaseCb() : Id [%d] PhysAddr 0x%llx Size 0x%llx\n",pPriv->Info.Id, pMmb->PhysAddr, pMmb->Size);
}

UINT32 AmbaMAL_KrnCmaMap(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = NULL;
    void *pVirtAddr = NULL;

    Ret = AmbaMAL_KrnMmbReference(pPriv, PhysAddr, Size, &pMmb);
    if(Ret == 0U) {
        Ret = AmbaMAL_KrnMmbP2V(pPriv, PhysAddr, &pVirtAddr);
        *ppVirtAddr = pVirtAddr;
    } else {
        printk("[ERROR] AmbaMAL_KrnCmaMap() : Id [%d] AmbaMAL_KrnMmbP2V fail \n",pPriv->Info.Id);
    }

    return Ret;
}

UINT32 AmbaMAL_KrnCmaUnMap(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;
    UINT32 Rlease = 0U;
    AMBA_MAL_MMB_s Mmb = {0};
    UINT64 PhysAddr;

    Ret = AmbaMAL_KrnMmbV2P(pPriv, pVirtAddr, &PhysAddr);
    if(Ret == 0U) {
        Ret = AmbaMAL_KrnMmbUnReference(pPriv, PhysAddr, Size, &Rlease, &Mmb);
        if(Ret != 0U) {
            printk("[ERROR] AmbaMAL_KrnCmaUnMap() : Id [%d] AmbaMAL_KrnMmbUnReference fail \n",pPriv->Info.Id);
        }
    } else {
        printk("[ERROR] AmbaMAL_KrnCmaUnMap() : Id [%d] AmbaMAL_KrnMmbV2P fail \n",pPriv->Info.Id);
    }

    return Ret;
}

static void AmbaMAL_KrnCmaVmClose(struct vm_area_struct *vma)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = (AMBA_MAL_MMB_s *)vma->vm_private_data;

    if (pMmb->Id < AMBA_MAL_ID_MAX) {
        if (AmbaMalPriv[pMmb->Id].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
            Ret = AmbaMAL_KrnCmaFree(&AmbaMalPriv[pMmb->Id], pMmb->PhysAddr, pMmb->Size);
            if (Ret != 0U) {
                printk("[ERROR] AmbaMAL_KrnCmaVmClose() : AmbaMAL_KrnCmaFree fail (base=0x%llx size=0x%llx)\n", pMmb->PhysAddr, pMmb->Size);
            }
        } else {
            printk("[ERROR] AmbaMAL_KrnCmaVmClose() : Id[%d] Type [0x%x] is not AMBA_MAL_TYPE_ALLOCATABLE \n", pMmb->Id, AmbaMalPriv[pMmb->Id].Info.Type);
        }
    } else {
        printk("[ERROR] AmbaMAL_KrnCmaVmClose() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",pMmb->Id,AMBA_MAL_ID_MAX);
    }
    return;
}

static const struct vm_operations_struct AmbaMalCmaVmOps = {
    .close = AmbaMAL_KrnCmaVmClose,
};


UINT32 AmbaMAL_KrnCmaMapUser(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, struct vm_area_struct *vma)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = NULL;

    Ret = AmbaMAL_KrnMmbReference(pPriv, PhysAddr, Size, &pMmb);
    if(Ret == 0U) {
        vma->vm_ops = &AmbaMalCmaVmOps;
        vma->vm_private_data = pMmb;
    } else {
        printk("[ERROR] AmbaMAL_KrnCmaMapUser() : Id [%d] AmbaMAL_KrnMmbReference fail \n",pPriv->Info.Id);
    }

    return Ret;
}


UINT32 AmbaMAL_KrnDedicateMap(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr)
{
    UINT32 Ret = 0U;
    void *pVirtAddr = NULL;
    pgprot_t Prot;
    UINT64 AlignSize = 0UL;
    UINT64 AlignAddr = 0UL;
    AMBA_MAL_MMB_s *pMmb;

    AlignSize = PAGE_ALIGN(Size);
    AlignAddr = PAGE_ALIGN(PhysAddr);
    if(AlignAddr != PhysAddr) {
        printk("[ERROR] AmbaMAL_KrnDedicateMap() : Id [%d] PhysAddr(0x%llx) not page align  \n",pPriv->Info.Id,PhysAddr);
        Ret = 1U;
    } else if (AlignSize != Size) {
        printk("[ERROR] AmbaMAL_KrnDedicateMap() : Id [%d] Size(0x%llx) not page align  \n",pPriv->Info.Id,Size);
        Ret = 1U;
    } else {
        if(Attri == AMBA_MAL_ATTRI_CACHE) {
            Prot = __pgprot(PROT_NORMAL);
        } else {
            Prot = __pgprot(PROT_NORMAL_NC);
        }
        pVirtAddr = __ioremap(PhysAddr, Size, Prot);
        if(pVirtAddr != NULL) {
            Ret = AmbaMAL_KrnMmbAdd(pPriv, PhysAddr, pVirtAddr, Size, NULL);
            if(Ret == 0U) {
                Ret = AmbaMAL_KrnMmbReference(pPriv, PhysAddr, Size, &pMmb);
                *ppVirtAddr = pVirtAddr;
            } else {
                iounmap(pVirtAddr);
                printk("[ERROR] AmbaMAL_KrnCmaFree() : Id [%d] AmbaMAL_KrnMmbAdd fail \n",pPriv->Info.Id);
            }
        } else {
            printk("[ERROR] AmbaMAL_KrnCmaFree() : Id [%d] __ioremap fail \n",pPriv->Info.Id);
            Ret = 1U;
        }
    }

    return Ret;
}

UINT32 AmbaMAL_KrnDedicateUnMap(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;
    UINT64 PhysAddr = 0UL;
    UINT32 Rlease;
    AMBA_MAL_MMB_s RleaseMmb;

    Ret = AmbaMAL_KrnMmbV2P(pPriv, pVirtAddr, &PhysAddr);
    if(Ret == 0U) {
        Ret = AmbaMAL_KrnMmbUnReference(pPriv, PhysAddr, Size, &Rlease, &RleaseMmb);
        if(Rlease == 1U) {
            Ret = AmbaMAL_KrnMmbRelease(pPriv, PhysAddr, Size);
            if(Ret == 0U) {
                iounmap(pVirtAddr);
            }
        }
    } else {
        printk("[ERROR] AmbaMAL_KrnDedicateUnMap() : Id [%d] AmbaMAL_KrnMmbV2P fail \n",pPriv->Info.Id);
    }

    return Ret;
}

UINT32 AmbaMAL_KrnCacheClean(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    ambcache_clean_range(pVirtAddr, Size);
    return Ret;
}

UINT32 AmbaMAL_KrnCacheInvalid(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    ambcache_inv_range(pVirtAddr, Size);
    return Ret;
}

