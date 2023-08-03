/**
 *  @file AmbaMAL_Krn.c
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

/**
 *  @brief      Allocate buffer from AmbaMAL Memory ID (Only for AMBA_MAL_TYPE_ALLOCATABLE).
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      Size : [IN] Size to allocate.
 *
 *  @param      Align : [IN] Base and Size to align.
 *
 *  @param      pBuf : [OUT] Return buffer structure.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Alloc(UINT32 Id, UINT64 Size, UINT32 Align, AMBA_MAL_BUF_s *pBuf)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Alloc() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if (pBuf == NULL) {
        printk("[ERROR] AmbaMAL_Alloc() : Id [%d] pBuf == NULL \n",Id);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ALLOCATE) == 0U) {
        printk("[ERROR] AmbaMAL_Alloc() : Id [%d] Capability[0x%x] couldn't allocate \n",Id,AmbaMalPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        UINT64 PhysAddr = 0UL, RealSize = 0UL;

        Ret = AmbaMAL_KrnCmaAlloc(&AmbaMalPriv[Id], Size, Align, &PhysAddr, &RealSize, NULL);
        if(Ret == 0U) {
            pBuf->Id  = Id;
            pBuf->PhysAddr  = PhysAddr;
            pBuf->Size  = Size;
            pBuf->RealSize  = RealSize;
            pBuf->Align  = Align;
            Ret = AmbaMAL_KrnGlobal2Phys(pBuf->PhysAddr, &pBuf->GlobalAddr);
            if(Ret != 0U) {
                printk("[ERROR] AmbaMAL_Alloc() : Id [%d] AmbaMAL_KrnGlobal2Phys fail PhysAddr 0x%llx \n",Id,PhysAddr);
            }
        }
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Alloc);

/**
 *  @brief      Free buffer to AmbaMAL Memory ID (Only for AMBA_MAL_TYPE_ALLOCATABLE).
 *
 *  @param      pBuf : [IN] Buffer structure.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Free(UINT32 Id, AMBA_MAL_BUF_s *pBuf)
{
    UINT32 Ret = 0U;

    if (pBuf == NULL) {
        printk("[ERROR] AmbaMAL_Free() : Id [%d] pBuf == NULL \n",Id);
        Ret = 1U;
    } else if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Free() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_FREE) == 0U) {
        printk("[ERROR] AmbaMAL_Free() : Id [%d] Capability[0x%x] couldn't free \n",Id,AmbaMalPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        Ret = AmbaMAL_KrnCmaFree(&AmbaMalPriv[Id], pBuf->PhysAddr, pBuf->RealSize);
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Free);

/**
 *  @brief      Map buffer to get virtual address for CPU used.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      Size : [IN] Size to map.
 *
 *  @param      Attri : [IN] Attribute of mmap.
 *
 *  @param      ppVirtAddr : [OUT] Return virtual address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Map(UINT32 Id, UINT64 PhysAddr, UINT64 Size, UINT64 Attri, void **ppVirtAddr)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Map() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_MAP) == 0U) {
        printk("[ERROR] AmbaMAL_Map() : Id [%d] Capability[0x%x] couldn't map \n",Id,AmbaMalPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        if (AmbaMalPriv[Id].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
            Ret = AmbaMAL_KrnCmaMap(&AmbaMalPriv[Id], PhysAddr, Size, Attri, ppVirtAddr);
        } else {
            Ret = AmbaMAL_KrnDedicateMap(&AmbaMalPriv[Id], PhysAddr, Size, Attri, ppVirtAddr);
        }
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Map);

/**
 *  @brief      UnMap buffer.
 *
 *  @param      pVirtAddr : [IN] Virtual address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Unmap(UINT32 Id, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Unmap() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_UNMAP) == 0U) {
        printk("[ERROR] AmbaMAL_Unmap() : Id [%d] Capability[0x%x] couldn't unmap \n",Id,AmbaMalPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        if (AmbaMalPriv[Id].Info.Type == AMBA_MAL_TYPE_ALLOCATABLE) {
            Ret = AmbaMAL_KrnCmaUnMap(&AmbaMalPriv[Id], pVirtAddr, Size);
        } else {
            Ret = AmbaMAL_KrnDedicateUnMap(&AmbaMalPriv[Id], pVirtAddr, Size);
        }
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Unmap);

/**
 *  @brief      Data cache clean.
 *
 *  @param      pVirtAddr : [IN] Virtual address to clean.
 *
 *  @param      Size : [IN] Size to clean.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_CacheClean(UINT32 Id, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_CacheClean() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_CACHE_OPS) == 0U) {
        printk("[ERROR] AmbaMAL_CacheClean() : Id [%d] Capability[0x%x] couldn't do cache ops \n",Id,AmbaMalPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        Ret = AmbaMAL_KrnCacheClean(&AmbaMalPriv[Id], pVirtAddr, Size);
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_CacheClean);

/**
 *  @brief      Data cache invalidate.
 *
 *  @param      pVirtAddr : [IN] Virtual address to invalidate.
 *
 *  @param      Size : [IN] Size to invalidate.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_CacheInvalid(UINT32 Id, void *pVirtAddr, UINT64 Size)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_CACHE_OPS) == 0U) {
        printk("[ERROR] AmbaMAL_CacheInvalid() : Id [%d] Capability[0x%x] couldn't do cache ops \n",Id,AmbaMalPriv[Id].Info.Capability);
        Ret = 1U;
    } else {
        Ret = AmbaMAL_KrnCacheInvalid(&AmbaMalPriv[Id], pVirtAddr, Size);
    }

    return Ret;

}
EXPORT_SYMBOL(AmbaMAL_CacheInvalid);

/**
 *  @brief      Get AmbaMAL memory info by ID.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pInfo : [OUT] AmbaMAL memory info structure of AmbaMAL Memory ID.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_GetInfo(UINT32 Id, AMBA_MAL_INFO_s *pInfo)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_GetInfo() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else {
        memcpy(pInfo, &AmbaMalPriv[Id].Info, sizeof(AMBA_MAL_INFO_s));
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_GetInfo);

/**
 *  @brief      Get AmbaMAL memory info by Physical address.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      pInfo : [OUT] AmbaMAL memory info structure of AmbaMAL Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_GetInfoByPhys(UINT64 PhysAddr, AMBA_MAL_INFO_s *pInfo)
{
    UINT32 Ret = 0U;
    UINT32 i,Found = 0U;

    for(i = 0U; i < AMBA_MAL_ID_MAX; i++) {
        if(AmbaMalPriv[i].Info.Type != AMBA_MAL_TYPE_INVALID) {
            if((AmbaMalPriv[i].Info.PhysAddr <= PhysAddr) && (PhysAddr < (AmbaMalPriv[i].Info.PhysAddr + AmbaMalPriv[i].Info.Size))) {
                memcpy(pInfo, &AmbaMalPriv[i].Info, sizeof(AmbaMalPriv[i].Info));
                Found = 1U;
                break;
            }
        }
    }

    if(Found == 0U) {
        printk("[ERROR] AmbaMAL_GetInfoByPhys() : phys 0x%llx not found \n", PhysAddr);
        Ret = 1U;
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_GetInfoByPhys);

/**
 *  @brief      Physical to virtual address.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      Attri : [IN] AmbaMAL Memory ATTRI.
 *
 *  @param      ppVirtAddr : [OUT] Virtual address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Phys2Virt(UINT32 Id, UINT64 PhysAddr, UINT32 Attri, void **ppVirtAddr)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Phys2Virt() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ADDR_TRANS) == 0U) {
        Ret = 1U;
    } else {
        Ret = AmbaMAL_KrnMmbP2V(&AmbaMalPriv[Id], PhysAddr, ppVirtAddr);
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Phys2Virt);

/**
 *  @brief      Virtual to physical address.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @param      pVirtAddr : [IN] Virtual address.
 *
 *  @param      pPhysAddr : [OUT] Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Virt2Phys(UINT32 Id, void *pVirtAddr, UINT64 *pPhysAddr)
{
    UINT32 Ret = 0U;

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Virt2Phys() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
        Ret = 1U;
    } else if ((AmbaMalPriv[Id].Info.Capability & AMBA_MAL_CAPLTY_CAN_ADDR_TRANS) == 0U) {
        Ret = 1U;
    } else {
        Ret = AmbaMAL_KrnMmbV2P(&AmbaMalPriv[Id], pVirtAddr, pPhysAddr);
    }

    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Virt2Phys);

/**
 *  @brief      Physical to global address.
 *
 *  @param      PhysAddr : [IN] Physical address.
 *
 *  @param      pGlobalAddr : [OUT] Global address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Phys2Global(UINT64 PhysAddr, UINT64 *pGlobalAddr)
{
    UINT32 Ret = 0U;

    Ret = AmbaMAL_KrnPhys2Global(PhysAddr, pGlobalAddr);
    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Phys2Global);

/**
 *  @brief      Global to physical address.
 *
 *  @param      GlobalAddr : [IN] Global address.
 *
 *  @param      pPhysAddr : [OUT] Physical address.
 *
 *  @return     0            on success.    \n
 */
UINT32 AmbaMAL_Global2Phys(UINT64 GlobalAddr, UINT64 *pPhysAddr)
{
    UINT32 Ret = 0U;

    Ret = AmbaMAL_KrnGlobal2Phys(GlobalAddr, pPhysAddr);
    return Ret;
}
EXPORT_SYMBOL(AmbaMAL_Global2Phys);

/**
 *  @brief      Dump memory list by Id.
 *
 *  @param      Id : [IN] AmbaMAL Memory ID.
 *
 *  @return     0            on success.    \n
 */
void AmbaMAL_Dump(UINT32 Id)
{

    if (Id >= AMBA_MAL_ID_MAX) {
        printk("[ERROR] AmbaMAL_Dump() : Id [%d] >= AMBA_MAL_ID_MAX [%d]\n",Id,AMBA_MAL_ID_MAX);
    } else {
        AmbaMAL_KrnMmbDump(&AmbaMalPriv[Id], NULL);
    }
}

