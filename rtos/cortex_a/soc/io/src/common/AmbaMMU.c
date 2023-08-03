/**
 *  @file AmbaMMU.c
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
 *  @details Memory Management Unit Control Middleware APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"

#include "AmbaMMU.h"
#include "AmbaRTSL_MMU.h"

#ifdef CONFIG_ARM32

/**
 *  AmbaMMU_Virt32ToPhys32 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_Virt32ToPhys32(UINT32 VirtAddr, UINT32 *pPhysAddr)
{
    return AmbaRTSL_MmuVirt32ToPhys32(VirtAddr, pPhysAddr);
}

/**
 *  AmbaMMU_Phys32ToVirt32 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_Phys32ToVirt32(UINT32 PhysAddr, UINT32 *pVirtAddr)
{
    return AmbaRTSL_MmuPhys32ToVirt32(PhysAddr, pVirtAddr);
}

/**
 *  AmbaMMU_VirtToPhys - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_VirtToPhys(ULONG VirtAddr, const ULONG *pPhysAddr)
{
    UINT32 *pTmp;
    UINT32 RetVal;

    AmbaMisra_TypeCast(&pTmp, &pPhysAddr);
    RetVal = AmbaMMU_Virt32ToPhys32(VirtAddr, pTmp);
    AmbaMisra_TypeCast(&pPhysAddr, &pTmp);

    return RetVal;
}

/**
 *  AmbaMMU_PhysToVirt - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_PhysToVirt(ULONG PhysAddr, const ULONG *pVirtAddr)
{
    UINT32 *pTmp;
    UINT32 RetVal;

    AmbaMisra_TypeCast(&pTmp, &pVirtAddr);
    RetVal = AmbaMMU_Phys32ToVirt32(PhysAddr, pTmp);
    AmbaMisra_TypeCast(&pVirtAddr, &pTmp);

    return RetVal;
}

/**
 *  AmbaMMU_QueryMapInfo32 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pMemMap pointer to the mapping and attributes of a memory region
 *  @return error code
 */
UINT32 AmbaMMU_QueryMapInfo32(UINT32 VirtAddr, AMBA_MMU_MAP_INFO32_s * pMemMap)
{
    return AmbaRTSL_MmuQueryMapInfo32(VirtAddr, pMemMap);
}

/**
 *  AmbaRTSL_MmuRegLock - Change specified memory region to read only
 *  @param[in] Region region ID
 *  @return error code
 */
UINT32 AmbaMMU_MmuRegLock(UINT32 RegionId)
{
    return AmbaRTSL_MmuRegLock(RegionId);
}
#endif

#ifdef CONFIG_ARM64

/**
 *  AmbaMMU_Virt64ToPhys64 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_Virt64ToPhys64(ULONG VirtAddr, ULONG *pPhysAddr)
{
    return AmbaRTSL_MmuVirt64ToPhys64(VirtAddr, pPhysAddr);
}

/**
 *  AmbaMMU_Phys64ToVirt64 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_Phys64ToVirt64(ULONG PhysAddr, ULONG *pVirtAddr)
{
    return AmbaRTSL_MmuPhys64ToVirt64(PhysAddr, pVirtAddr);
}

/**
 *  AmbaMMU_VirtToPhys - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_VirtToPhys(ULONG VirtAddr, ULONG *pPhysAddr)
{
    /* hard code len to 1 */
    return AmbaMMU_Virt64ToPhys64(VirtAddr, pPhysAddr);
}

/**
 *  AmbaMMU_PhysToVirt - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_PhysToVirt(ULONG PhysAddr, ULONG *pVirtAddr)
{
    return AmbaRTSL_MmuPhys64ToVirt64(PhysAddr, pVirtAddr);
}

/**
 *  AmbaMMU_QueryMapInfo64 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pMemMap pointer to the mapping and attributes of a memory region
 *  @return error code
 */
UINT32 AmbaMMU_QueryMapInfo64(UINT64 VirtAddr, AMBA_MMU_MAP_INFO64_s * pMemMap)
{
    return AmbaRTSL_MmuQueryMapInfo64(VirtAddr, pMemMap);
}
#endif
