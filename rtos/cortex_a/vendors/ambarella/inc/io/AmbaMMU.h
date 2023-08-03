/**
 *  @file AmbaMMU.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Definitions & Constants for Memory Management Unit Control APIs
 *
 */

#ifndef AMBA_MMU_H
#define AMBA_MMU_H

#ifndef AMBA_MMU_DEF_H
#include "AmbaMMU_Def.h"
#endif

/*
 * Defined in an external file: i.e. AmbaMmuUserCtrl.c
 */
#ifdef CONFIG_ARM32
/* VMSAv8-32  */
extern AMBA_MMU_USER_CONFIG32_s AmbaMmuUserConfig;
#endif
#ifdef CONFIG_ARM64
/* VMSAv8-64 */
extern AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfig;
extern AMBA_MMU_USER_CONFIG64_s AmbaMmuUserConfigPreOSInit;
#endif

/*
 * Defined in an AmbaMMU.c
 */
#ifdef CONFIG_ARM32
/* VMSAv8-32  */
UINT32 AmbaMMU_Virt32ToPhys32(UINT32 VirtAddr, UINT32 *pPhysAddr);
UINT32 AmbaMMU_Phys32ToVirt32(UINT32 PhysAddr, UINT32 *pVirtAddr);
UINT32 AmbaMMU_VirtToPhys(ULONG VirtAddr, const ULONG *pPhysAddr);
UINT32 AmbaMMU_PhysToVirt(ULONG PhysAddr, const ULONG *pVirtAddr);
UINT32 AmbaMMU_QueryMapInfo32(UINT32 VirtAddr, AMBA_MMU_MAP_INFO32_s * pMemMap);
#endif
#ifdef CONFIG_ARM64
/* VMSAv8-64 */
UINT32 AmbaMMU_Virt64ToPhys64(ULONG VirtAddr, ULONG *pPhysAddr);
UINT32 AmbaMMU_Phys64ToVirt64(ULONG PhysAddr, ULONG *pVirtAddr);
UINT32 AmbaMMU_VirtToPhys(ULONG VirtAddr, ULONG *pPhysAddr);
UINT32 AmbaMMU_PhysToVirt(ULONG PhysAddr, ULONG *pVirtAddr);
UINT32 AmbaMMU_QueryMapInfo64(UINT64 VirtAddr, AMBA_MMU_MAP_INFO64_s * pMemMap);
#endif

UINT32 AmbaMMU_MmuRegLock(UINT32 RegionId);

#endif  /* AMBA_MMU_H */
