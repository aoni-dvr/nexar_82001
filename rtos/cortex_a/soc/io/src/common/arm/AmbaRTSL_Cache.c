/**
 *  @file AmbaRTSL_Cache.c
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
 *  @details CPU Cache Function APIs (ARMv8 AArch32/AArch64)
 *
 */

#include "AmbaTypes.h"
#include "AmbaIntrinsics.h"
#include "AmbaWrap.h"
#include "AmbaRTSL_Cache.h"
#include "AmbaRTSL_Cache_Priv.h"

/**
 *  CACHE_DataCacheCleanInvalidateRange - Clean and invalidate data cache by specifying address range to PoC
 *  @param[in] StartAddr the first address to be clean&invalidate
 *  @param[in] EndAddr the last address to be clean&invalidate
 */
static void CACHE_DataCacheCleanInvalidateRange(ULONG StartAddr, ULONG EndAddr)
{
    ULONG Start = StartAddr;
    (void)EndAddr;

    /* clean invalidate data cache line by virtual address to PoC */
    while (Start <= EndAddr) {
#ifdef CONFIG_ARM64
        AmbaCache_DataCleanInvMvaC(Start);
#else
        AmbaCache_DataCleanInvMvaCA32(Start);
#endif
        Start += CACHE_LINE_SIZE;
    }

    /* Ensure completion of the clean&inval (no data lost) */
    AMBA_DSB();
}

/**
 *  CACHE_DataCacheCleanRange - Clean data cache by specifying address range to PoC
 *  @param[in] StartAddr the first address to be clean
 *  @param[in] EndAddr the last address to be clean
 */
static void CACHE_DataCacheCleanRange(ULONG StartAddr, ULONG EndAddr)
{
    ULONG Start = StartAddr;
    (void)EndAddr;

    /* clean data cache line by virtual address to PoC */
    while (Start <= EndAddr) {
#ifdef CONFIG_ARM64
        AmbaCache_DataCleanMvaC(Start);
#else
        AmbaCache_DataCleanMvaCA32(Start);
#endif
        Start += CACHE_LINE_SIZE;
    }

    /* Ensure completion of the clean */
    AMBA_DSB();
}

/**
 *  CACHE_DataCacheInvalidateRange - Invalidate data cache by specifying address range to PoC
 *  @param[in] StartAddr the first address to be invalidate
 *  @param[in] EndAddr the last address to be invalidate
 */
static void CACHE_DataCacheInvalidateRange(ULONG StartAddr, ULONG EndAddr)
{
    ULONG Start = StartAddr;

    (void)EndAddr;

    /* invalidate data cache line by virtual address to PoC */
    while (Start <= EndAddr) {
#ifdef CONFIG_ARM64
        AmbaCache_DataInvMvaC(Start);
#else
        AmbaCache_DataInvMvaCA32(Start);
#endif

        Start += CACHE_LINE_SIZE;
    }


    /* Ensure completion of the inval */
    AMBA_DSB();
}

/**
 *  AmbaRTSL_CacheInvalDataAll - Invalidate entire data cache.
 */
void AmbaRTSL_CacheInvalDataAll(void)
{
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    CACHE_DataCacheInvldL3();
#endif
    CACHE_DataCacheInvldL2();           /* Invalidate L2 */
    CACHE_DataCacheInvldL1();           /* Invalidate L1 */
}

/**
 *  AmbaRTSL_CacheCleanDataAll - Clean entire data cache.
 */
void AmbaRTSL_CacheCleanDataAll(void)
{
    CACHE_DataCacheCleanL1();           /* Clean L1 */
    CACHE_DataCacheCleanL2();           /* Clean L2 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    CACHE_DataCacheCleanL3();
#endif
}

/**
 *  AmbaRTSL_CacheFlushDataAll - Clean and Invalidate entire data cache.
 */
void AmbaRTSL_CacheFlushDataAll(void)
{
    CACHE_DataCacheCleanL1();           /* Clean L1 */
#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
    CACHE_DataCacheCleanL2();           /* Clean L2 */
    CACHE_DataCacheCleanInvldL3();      /* Clean and invalidate L3 */
#endif
    CACHE_DataCacheCleanInvldL2();      /* Clean and invalidate L2 */
    CACHE_DataCacheCleanInvldL1();      /* Clean and invalidate L1 */
}

/**
 *  AmbaRTSL_CacheInvalData - Invalidate data cache by specifying address range
 *  @param[in] VirtAddr the lowest address of a virtually contiguous memory
 *  @param[in] Size number of bytes
 *  @return error code
 */
UINT32 AmbaRTSL_CacheInvalData(ULONG VirtAddr, ULONG Size)
{
    ULONG StartAddr, EndAddr;
    UINT32 RetVal = CACHE_ERR_NONE;

    if (((VirtAddr & (CACHE_LINE_SIZE - 1U)) != 0U) || ((Size & (CACHE_LINE_SIZE - 1U)) != 0U)) {
        RetVal = CACHE_ERR_ARG;
    }

    if (Size != 0U) {
        StartAddr = VirtAddr & CACHE_LINE_MASK;
        EndAddr = VirtAddr + Size - 1U;

        CACHE_DataCacheInvalidateRange(StartAddr, EndAddr);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_CacheCleanData - Clean data cache by specifying address range
 *  @param[in] VirtAddr the lowest address of a virtually contiguous memory
 *  @param[in] Size number of bytes
 *  @return error code
 */
UINT32 AmbaRTSL_CacheCleanData(ULONG VirtAddr, ULONG Size)
{
    ULONG StartAddr, EndAddr;
    UINT32 RetVal = CACHE_ERR_NONE;

    if (((VirtAddr & (CACHE_LINE_SIZE - 1U)) != 0U) || ((Size & (CACHE_LINE_SIZE - 1U)) != 0U)) {
        RetVal = CACHE_ERR_ARG;
    }

    if (Size != 0U) {
        StartAddr = VirtAddr & CACHE_LINE_MASK;
        EndAddr = VirtAddr + Size - 1U;

        CACHE_DataCacheCleanRange(StartAddr, EndAddr);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_CacheFlushData - Clean and invalidate data cache by specifying address range
 *  @param[in] VirtAddr the lowest address of a virtually contiguous memory
 *  @param[in] Size number of bytes
 *  @return error code
 */
UINT32 AmbaRTSL_CacheFlushData(ULONG VirtAddr, ULONG Size)
{
    ULONG StartAddr, EndAddr;
    UINT32 RetVal = CACHE_ERR_NONE;

    if (((VirtAddr & (CACHE_LINE_SIZE - 1U)) != 0U) || ((Size & (CACHE_LINE_SIZE - 1U)) != 0U)) {
        RetVal = CACHE_ERR_ARG;
    }

    if (Size != 0U) {
        StartAddr = VirtAddr & CACHE_LINE_MASK;
        EndAddr = VirtAddr + Size - 1U;

        CACHE_DataCacheCleanInvalidateRange(StartAddr, EndAddr);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_CacheInvalDataPtr - Invalidate data cache by specifying address range
 *  @param[in] pVirtAddr the lowest address of a virtually contiguous memory
 *  @param[in] Size number of bytes
 *  @return error code
 */
UINT32 AmbaRTSL_CacheInvalDataPtr(const void * pVirtAddr, ULONG Size)
{
    ULONG Addr;
    UINT32 RetVal = AmbaWrap_memcpy(&Addr, &pVirtAddr, sizeof(Addr));

    if (RetVal == OK) {
        RetVal = AmbaRTSL_CacheInvalData(Addr, Size);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_CacheCleanDataPtr - Clean data cache by specifying address range
 *  @param[in] pVirtAddr the lowest address of a virtually contiguous memory
 *  @param[in] Size number of bytes
 *  @return error code
 */
UINT32 AmbaRTSL_CacheCleanDataPtr(const void * pVirtAddr, ULONG Size)
{
    ULONG Addr;
    UINT32 RetVal = AmbaWrap_memcpy(&Addr, &pVirtAddr, sizeof(Addr));

    if (RetVal == OK) {
        RetVal = AmbaRTSL_CacheCleanData(Addr, Size);
    }

    return RetVal;
}

/**
 *  AmbaRTSL_CacheFlushDataPtr - Clean and invalidate data cache by specifying address range
 *  @param[in] pVirtAddr the lowest address of a virtually contiguous memory
 *  @param[in] Size number of bytes
 *  @return error code
 */
UINT32 AmbaRTSL_CacheFlushDataPtr(const void * pVirtAddr, ULONG Size)
{
    ULONG Addr;
    UINT32 RetVal = AmbaWrap_memcpy(&Addr, &pVirtAddr, sizeof(Addr));

    if (RetVal == OK) {
        RetVal = AmbaRTSL_CacheFlushData(Addr, Size);
    }

    return RetVal;
}

