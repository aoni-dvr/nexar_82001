/**
 *  @file AmbaIOUTDiag_CmdNvme.c
 *
 *  @copyright Copyright (c) 2021 Ambarella, Inc.
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
 *  @details NVMe diagnostic Command
 */

#ifndef AMBA_KAL_H
#include <AmbaKAL.h>
#endif
#include <AmbaUtility.h>
#include <AmbaShell.h>
#include <AmbaIOUTDiag.h>
#include <AmbaPCIE.h>
#include "AmbaPrint.h"
#include "AppNVME.h"

#define UT_NVME_NG            -1
#define UT_NVME_NO_PROCESS    -2

#define PCIE_CACHE_MEM_SIZE 0x200000 // 2MB

UINT8 g_pcie_memory_cached[PCIE_CACHE_MEM_SIZE] __attribute__((section(".bss.noinit")));

static void AppNvme_Print(const char *pFmt)
{
    AmbaPrint_PrintUInt5(pFmt, 0, 0, 0, 0, 0);
    AmbaPrint_Flush();
}

static void NVME_TestUsage(void)
{
    AppNvme_Print(" nvme start : open a nvme device\n");
}

static void AppNvme_TestCmd(UINT32 Argc, char * const * Argv)
{
    INT32 nret = 0;

    if (Argc >= 2U) {
        if (AmbaUtility_StringCompare(Argv[1], "start", 5) == 0) {
            // get non-cache memory pool
            IOUT_MEMORY_INFO_s *noncache_mem_info = AmbaIOUT_NoncacheMemInfoGet(IOUT_MEMID_PCIE);
            NVME_ENABLE_PARM_s parm = {
                .StackCacheMemory        = g_pcie_memory_cached,
                .StackCacheMemorySize    = PCIE_CACHE_MEM_SIZE,
                .StackNonCacheMemory     = noncache_mem_info->Ptr,
                .StackNonCacheMemorySize = noncache_mem_info->Size,
            };
            AppNVME_Enable(&parm);

        } else {
            nret = UT_NVME_NO_PROCESS;
        }
    } else {
        nret = UT_NVME_NG;
    }


    if (nret != 0) {
        NVME_TestUsage();
    }

    return;
}

void AmbaIOUTDiag_CmdNvme(UINT32 ArgCount, char * const * pArgVector, AMBA_SHELL_PRINT_f PrintFunc)
{
    (void)PrintFunc;
    AppNvme_TestCmd(ArgCount, pArgVector);
}
