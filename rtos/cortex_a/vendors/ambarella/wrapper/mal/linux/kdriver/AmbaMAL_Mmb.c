/**
 *  @file AmbaMAL_Mmb.c
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

#include <linux/kernel.h>
#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>
#include <linux/export.h>
#include <linux/mm.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include "AmbaMAL.h"
#include "AmbaMAL_KrnPriv.h"

void AmbaMAL_KrnMmbInit(AMBA_MAL_INFO_PRIV_s *pPriv)
{
    mutex_init(&pPriv->MmbMutex);
    INIT_LIST_HEAD(&pPriv->MmbList);
    INIT_LIST_HEAD(&pPriv->FreeMmbList);
}

UINT32 AmbaMAL_KrnMmbAdd(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void *pVirtAddr, UINT64 Size, void *Owner)
{
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbSerch = NULL,*pMmbTmp = NULL;
    UINT32 Ret = 0U;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->FreeMmbList)) {
        pMmb = list_first_entry(&pPriv->FreeMmbList, AMBA_MAL_MMB_s, List);
        list_del_init(&pMmb->List);
    }

    if (pMmb == NULL) {
        pMmb = kmalloc(sizeof(AMBA_MAL_MMB_s), GFP_KERNEL);
        if (!pMmb) {
            printk("[ERROR] AmbaMAL_KrnMmbAlloc() : kmalloc fail \n");
            Ret = 1U;
        }
    }

    if(Ret == 0U) {
        pMmb->Id = pPriv->Info.Id;
        pMmb->PhysAddr = PhysAddr;
        pMmb->pVirtAddr = pVirtAddr;
        pMmb->Size = Size;
        pMmb->RefCnt = 0U;
        pMmb->Owner = Owner;
        INIT_LIST_HEAD(&pMmb->List);

        if (!list_empty(&pPriv->MmbList)) {
            list_for_each_entry_safe(pMmbSerch, pMmbTmp, &pPriv->MmbList, List) {
                if (pMmb->PhysAddr < pMmbSerch->PhysAddr) {
                    break;
                }
            }
            list_add(&pMmb->List, pMmbSerch->List.prev);
        } else {
            list_add_tail(&pMmb->List, &pPriv->MmbList);
        }

        pr_debug("[INFO] AmbaMAL_KrnMmbAdd() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u \n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
    }

    mutex_unlock(&pPriv->MmbMutex);

    return Ret;
}

UINT32 AmbaMAL_KrnMmbRelease(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size)
{
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;
    UINT32 Found = 0U,Ret = 0U;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if ((pMmb->PhysAddr == PhysAddr) && (pMmb->Size == Size)) {
                if(pMmb->RefCnt != 0U) {
                    printk("[ERROR] AmbaMAL_KrnMmbRelease() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u should be 0\n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                } else {
                    pr_debug("[INFO] AmbaMAL_KrnMmbRelease() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u \n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                }
                list_move(&pMmb->List, &pPriv->FreeMmbList);
                Found = 1U;
                break;
            }
        }
    }

    if (Found == 0U) {
        printk("[ERROR] AmbaMAL_KrnMmbRelease() : Id [%d] target_mmb : phys: 0x%llx, size 0x%llx not found \n", pPriv->Info.Id, PhysAddr, Size);
        Ret = 1;
    }

    mutex_unlock(&pPriv->MmbMutex);

    return Ret;

}

UINT32 AmbaMAL_KrnMmbReference(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, AMBA_MAL_MMB_s **ppMmb)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;
    UINT32 Found = 0U;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if ((pMmb->PhysAddr <= PhysAddr) && ((PhysAddr + Size) <= (pMmb->PhysAddr + pMmb->Size))) {
                ++pMmb->RefCnt;
                *ppMmb = pMmb;
                pr_debug("[INFO] AmbaMAL_KrnMmbReference() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u \n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                Found = 1;
                break;
            }
        }
    }

    if (Found == 0U) {
        printk("[ERROR] AmbaMAL_KrnMmbReference() : Id [%d] phys: 0x%llx, size 0x%llx not found \n", pPriv->Info.Id, PhysAddr, Size);
        Ret = 1U;
    }

    mutex_unlock(&pPriv->MmbMutex);
    return Ret;
}

UINT32 AmbaMAL_KrnMmbUnReference(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, UINT64 Size, UINT32 *pRlease, AMBA_MAL_MMB_s *pRleaseMmb)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;
    UINT32 Found = 0U;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if ((pMmb->PhysAddr <= PhysAddr) && ((PhysAddr + Size) <= (pMmb->PhysAddr + pMmb->Size))) {
                if(pMmb->RefCnt == 0U) {
                    printk("[ERROR] AmbaMAL_KrnMmbUnReference() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u should not be 0\n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                } else {
                    --pMmb->RefCnt;
                    pr_debug("[INFO] AmbaMAL_KrnMmbUnReference() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u \n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                }
                Found = 1U;
                if (pMmb->RefCnt == 0U) {
                    *pRlease = 1U;
                    if(pRleaseMmb != NULL) {
                        memcpy(pRleaseMmb, pMmb, sizeof(AMBA_MAL_MMB_s));
                    }
                }
                break;
            }
        }
    }

    if (Found == 0U) {
        printk("[ERROR] AmbaMAL_KrnMmbUnReference() : Id [%d] phys: 0x%llx , size 0x%llx not found \n", pPriv->Info.Id, PhysAddr, Size);
        Ret = 1U;
    }

    mutex_unlock(&pPriv->MmbMutex);

    return Ret;
}

UINT32 AmbaMAL_KrnMmbP2V(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 PhysAddr, void **ppVirtAddr)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;
    UINT32 Found = 0U;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if ((pMmb->PhysAddr <= PhysAddr) && (PhysAddr < (pMmb->PhysAddr + pMmb->Size))) {
                Found = 1;
                break;
            }
        }
    }

    if (Found) {
        UINT64 Offset;

        Offset      = PhysAddr - pMmb->PhysAddr;
        *ppVirtAddr  = (void *)(pMmb->pVirtAddr + Offset);
    } else {
        *ppVirtAddr = NULL;
        Ret = 1U;
    }

    mutex_unlock(&pPriv->MmbMutex);
    return Ret;
}

UINT32 AmbaMAL_KrnMmbV2P(AMBA_MAL_INFO_PRIV_s *pPriv, void *pVirtAddr, UINT64 *pPhysAddr)
{
    UINT32 Ret = 0U;
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;
    UINT32 Found = 0U;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if ((pMmb->pVirtAddr <= pVirtAddr) && (pVirtAddr < (pMmb->pVirtAddr + pMmb->Size))) {
                Found = 1;
                break;
            }
        }
    }

    if (Found) {
        UINT64 Offset;

        Offset      = pVirtAddr - pMmb->pVirtAddr;
        *pPhysAddr   = pMmb->PhysAddr + Offset;
    } else {
        *pPhysAddr  = 0UL;
        Ret = 1U;
    }

    mutex_unlock(&pPriv->MmbMutex);

    return Ret;
}

void AmbaMAL_KrnMmbReleaseByOwner(AMBA_MAL_INFO_PRIV_s *pPriv, void *Owner, void (*ReleaseCb)(AMBA_MAL_INFO_PRIV_s *pReleasePriv, AMBA_MAL_MMB_s *pReleaseMmb))
{
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;

    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if((pMmb->Owner != NULL) && (pMmb->Owner == Owner)) {
                if(pMmb->RefCnt == 0U) {
                    printk("[ERROR] AmbaMAL_KrnMmbUnReferenceByOwner() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u should not be 0\n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                } else {
                    --pMmb->RefCnt;
                    pr_debug("[INFO] AmbaMAL_KrnMmbUnReferenceByOwner() : Id [%d] phys: 0x%llx, virt %p, size 0x%llx , ref_cnt: %u \n", pPriv->Info.Id, pMmb->PhysAddr, pMmb->pVirtAddr, pMmb->Size, pMmb->RefCnt);
                }
                if (pMmb->RefCnt == 0U) {
                    ReleaseCb(pPriv, pMmb);
                    list_move(&pMmb->List, &pPriv->FreeMmbList);
                }
            }
        }
    }
    mutex_unlock(&pPriv->MmbMutex);
}

void AmbaMAL_KrnMmbGetUsedSize(AMBA_MAL_INFO_PRIV_s *pPriv, UINT64 *pUsedSize)
{
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;

    *pUsedSize = 0UL;
    mutex_lock(&pPriv->MmbMutex);

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            *pUsedSize += pMmb->Size;
        }
    }
    mutex_unlock(&pPriv->MmbMutex);
}

void AmbaMAL_KrnMmbDump(AMBA_MAL_INFO_PRIV_s *pPriv, struct seq_file *m)
{
    AMBA_MAL_MMB_s *pMmb = NULL, *pMmbTmp = NULL;

    mutex_lock(&pPriv->MmbMutex);

    if(m != NULL) {
        seq_printf(m, "Id [%d] Pa 0x%llx - 0x%llx Ga 0x%llx \n", pPriv->Info.Id, pPriv->Info.PhysAddr, (pPriv->Info.PhysAddr + pPriv->Info.Size),pPriv->Info.GlobalAddr);
    } else {
        printk("Id [%d] Pa 0x%llx - 0x%llx Ga 0x%llx \n", pPriv->Info.Id, pPriv->Info.PhysAddr, (pPriv->Info.PhysAddr + pPriv->Info.Size),pPriv->Info.GlobalAddr);
    }

    if (!list_empty(&pPriv->MmbList)) {
        list_for_each_entry_safe(pMmb, pMmbTmp, &pPriv->MmbList, List) {
            if(m != NULL) {
                seq_printf(m, "          Pa 0x%llx - 0x%llx ref_cnt: %u \n", pMmb->PhysAddr, pMmb->PhysAddr + pMmb->Size,  pMmb->RefCnt);
            } else {
                printk("          Pa 0x%llx - 0x%llx ref_cnt: %u \n", pMmb->PhysAddr, pMmb->PhysAddr + pMmb->Size,  pMmb->RefCnt);
            }
        }
    }
    mutex_unlock(&pPriv->MmbMutex);
}


