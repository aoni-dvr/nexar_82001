/**
 *  @file AmbaMMU_QNX.c
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
 *  @details Memory Management Unit Control Middleware APIs
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "AmbaTypes.h"
#include "AmbaMMU.h"

#ifdef CONFIG_ARM64

/**
 *  AmbaMMU_Virt64ToPhys64 - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] VirtAddr virtual address
 *  @param[out] pPhysAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_Virt64ToPhys64(ULONG VirtAddr, ULONG *pPhysAddr)
{
    /* hard code len to 1 */
    return posix_mem_offset64((const void *)VirtAddr, 1U, (off64_t *)pPhysAddr, NULL, NULL);
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
    return posix_mem_offset64((const void *)VirtAddr, 1U, (off64_t *)pPhysAddr, NULL, NULL);
}

/**
 *  AmbaMMU_PhysToVirt - The function performs the translation of virtual memory addresses to physical addresses.
 *  @param[in] PhysAddr physical address
 *  @param[out] pVirtAddr pointer that store tranfered address
 *  @return error code
 */
UINT32 AmbaMMU_PhysToVirt(ULONG PhysAddr, ULONG *pVirtAddr)
{
    /* It should not be called for QNX */
    (void)PhysAddr;
    (void)pVirtAddr;
    return 0xffffffffU;
}

#endif
