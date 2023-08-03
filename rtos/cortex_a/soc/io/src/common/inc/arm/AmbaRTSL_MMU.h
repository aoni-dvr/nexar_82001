/**
 *  @file AmbaRTSL_MMU.h
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
 *  @details Common definitions & constants for MMU RTSL Control APIs
 *
 */

#ifndef AMBA_RTSL_MMU_H
#define AMBA_RTSL_MMU_H

#ifndef AMBA_MMU_DEF_H
#include "AmbaMMU_Def.h"
#endif
#ifndef AMBA_CACHE_DEF_H
#include "AmbaCache_Def.h"
#endif

/*
 * Defined in AmbaRTSL_MMU.c
 */
void AmbaMMU_SetupPrimary(UINT32 NumSmpCores);
#ifdef CONFIG_ARM64
void AmbaMMU_SetupPrimaryPreOSInit(void);
#endif
void AmbaMMU_SetupNonPrimaryPreSCU(void);
void AmbaMMU_SetupNonPrimaryPostSCU(void);
void AmbaMMU_Enable(void);
void AmbaMMU_Disable(void);
void AmbaMMU_Disable_Training(void);

/*
 * Defined in both AmbaRTSL_MMU_VMSAv8-32.c and AmbaRTSL_MMU_VMSAv8-64.c
 */
#ifdef CONFIG_ARM64
UINT32 AmbaRTSL_MmuCpuSetup(UINT8 HighVectFlag);
void AmbaRTSL_MmuTlbConfig(void);
void AmbaRTSL_MmuTlbConfigPreOSInit(void);
void AmbaRTSL_MmuTlbInvalidateAll(void);
#if defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
void AmbaRTSL_MmuTlbConfigXenInit(void);
#endif // defined(CONFIG_XEN_SUPPORT) && !defined(AMBA_FWPROG)
#endif

#ifdef CONFIG_ARM32
/*
 * Defined in AmbaRTSL_MMU_VMSAv8-32.c
 */
UINT32 AmbaRTSL_MmuCpuSetupA32(UINT8 HighVectFlag);
void AmbaRTSL_MmuTlbConfigA32(void);
void AmbaRTSL_MmuTlbInvalidateAllA32(void);
UINT32 AmbaRTSL_MmuVirt32ToPhys32(UINT32 VirtAddr, UINT32 *pPhysAddr);
UINT32 AmbaRTSL_MmuPhys32ToVirt32(UINT32 PhysAddr, UINT32 *pVirtAddr);
UINT32 AmbaRTSL_MmuQueryMapInfo32(UINT32 VirtAddr, AMBA_MMU_MAP_INFO32_s * pMemMap);
UINT32 AmbaRTSL_MmuRegLock(UINT32 RegionId);
#endif

#ifdef CONFIG_ARM64
/*
 * Defined in AmbaRTSL_MMU_VMSAv8-64.c
 */
UINT32 AmbaRTSL_MmuVirt64ToPhys64(ULONG VirtAddr, ULONG *pPhysAddr);
UINT32 AmbaRTSL_MmuPhys64ToVirt64(ULONG PhysAddr, ULONG *pVirtAddr);
UINT32 AmbaRTSL_MmuQueryMapInfo64(UINT64 VirtAddr, AMBA_MMU_MAP_INFO64_s * pMemMap);
#endif

#endif  /* AMBA_RTSL_MMU_H */
