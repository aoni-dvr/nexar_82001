/**
*  @file AmbaSvcWrap.h
 *
 * Copyright (c) [2020] Ambarella International LP
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella International LP and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella International LP.
 *
 * This file includes sample code and is only for internal testing and evaluation.  If you 
 * distribute this sample code (whether in source, object, or binary code form), it will be 
 * without any warranty or indemnity protection from Ambarella International LP or its affiliates.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*
*  @details svc wrap functions
*
*/

#ifndef AMBA_SVC_WRAP_H
#define AMBA_SVC_WRAP_H

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaWrap.h"
#include "AmbaPrint.h"

#if defined(CONFIG_KAL_THREADX_SMP_NUM_CORES)
#define CONFIG_SMP_NUM_CORES    (CONFIG_KAL_THREADX_SMP_NUM_CORES)
#else
#define CONFIG_SMP_NUM_CORES    (CONFIG_SMP_CORE_NUM)
#endif

#define AMBA_NVM_ID_NULL (0xFFFFFFFFU)
#define AMBA_PTB_ID_SYS  (0x0U)
#define AMBA_PTB_ID_USER (0x1U)

typedef struct {
    /* cache operations */
    UINT32 (*pAmbaSvcCacheClean)(ULONG Addr, ULONG Size);
    UINT32 (*pAmbaSvcCacheInvalidate)(ULONG Addr, ULONG Size);
    UINT32 (*pAmbaSvcCacheFlush)(ULONG Addr, ULONG Size);

    UINT32 (*pAmbaSvcCacheChk)(void *pStartAddr, UINT32 Size);

    UINT32 (*pAmbaSvcVir2Phys)(void *VirtAddr, void **PhysAddr);
} AMBA_SVC_WRAP_INIT_s;

extern UINT32 PtbNvmID;

static inline ULONG AmbaSvcWrap_GetAlignedAddr(ULONG Addr, ULONG Align)
{
    #if defined(CONFIG_ARM32)
    return GetAlignedValU32(Addr, Align);
    #else
    return (ULONG)GetAlignedValU64((UINT64)Addr, (UINT64)Align);
    #endif
}

static inline void AmbaSvcWrap_MisraMemcpy(void *pDst, const void *pSrc, SIZE_t num)
{
    if (AmbaWrap_memcpy(pDst, pSrc, num) != OK) {
        AmbaPrint_PrintStr5("[%s] fail to do memcpy", __func__, NULL, NULL, NULL, NULL);
    }
}

static inline void AmbaSvcWrap_MisraMemset(void *ptr, INT32 v, SIZE_t n)
{
    if (AmbaWrap_memset(ptr, v, n) != OK) {
        AmbaPrint_PrintStr5("[%s] fail to do memset", __func__, NULL, NULL, NULL, NULL);
    }
}

UINT32 AmbaSvcWrap_GetNVMID(UINT32 *pNvmID, UINT32 PtbID, UINT32 PartID);

void   AmbaSvcWrap_Init(const AMBA_SVC_WRAP_INIT_s *pInit);

UINT32 AmbaSvcWrap_CacheClean(ULONG Addr, ULONG Size);
UINT32 AmbaSvcWrap_CacheInvalidate(ULONG Addr, ULONG Size);
UINT32 AmbaSvcWrap_CacheFlush(ULONG Addr, ULONG Size);
UINT32 AmbaSvcWrap_CacheChk(ULONG Addr, ULONG Size);
UINT32 AmbaSvcWrap_Vir2Phys(void *VirtAddr, void **PhysAddr);

#endif  /* AMBA_SVC_WRAP_H */
