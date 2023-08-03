/**
 * @file AmbaPIO.c
 *  Peripheral Memory Allocator
 *
 * Copyright (c) 2020 Ambarella International LP
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
 */

#include "AmbaWrap.h"
#include "AmbaKAL.h"
#include "AmbaPIO.h"
#include "AmbaCache.h"
#include "AmbaMMU.h"
#include "AmbaMisraFix.h"

static AMBA_KAL_MUTEX_t PioMutex;

static UINT32 AmbaPIO_Init(void)
{
    UINT32 Ret = 0U;
    static char PioMutxName[16] = "PioMutex";

    if (AmbaKAL_MutexCreate(&PioMutex, PioMutxName) != 0U) {
        Ret = PIO_ERR_OSERR;
    }

    return Ret;
}

/**
 *  @brief      The function allocates Peripheral continuous buffer
 *
 *  @param      Size : [IN] The size of the request.
 *  @param      Cacheable : [IN] Cacheable (1) or not (0)
 *  @param      PioBuf : [OUT] The PIO buffer struture
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_MemAlloc(ULONG Size, UINT32 Cacheable, AMBA_PIO_s *PioBuf)
{
    static UINT32 inited = 0U;
    static ULONG PioCached = 0U;
    static ULONG PioNonCached = 0U;
    UINT32 Ret = 0U;
    /*todo: use dtb*/
    const ULONG PIO_NONCACHED_S = (ULONG)CONFIG_PIO_BASE;
    const ULONG PIO_NONCACHED_E = (ULONG)CONFIG_PIO_BASE + (ULONG)CONFIG_PIO_NONCHED_SIZE;
    const ULONG PIO_CACHED_S = PIO_NONCACHED_E;
    const ULONG PIO_CACHED_E = PIO_NONCACHED_S + (ULONG)CONFIG_PIO_SIZE;
    ULONG pC, pN;

    /* arg check */
    if (PioBuf == NULL) {
        Ret = PIO_ERR_EINVAL;
    } else {
        if (inited == 0U) {
            inited = 1U;
            PioNonCached = PIO_NONCACHED_S;
            PioCached = PIO_CACHED_S;
            (void)AmbaPIO_Init();
        }
        PioBuf->Cacheable = Cacheable;
        PioBuf->Size = (UINT32)Size;

        /* lock */
        if (AmbaKAL_MutexTake(&PioMutex, KAL_WAIT_FOREVER) != 0U) {
            Ret = PIO_ERR_OSERR;
        }

        /* non cache */
        if (Cacheable == 0U) {
            pN = PioNonCached + Size;
            if (pN <= PIO_NONCACHED_E) {
                PioBuf->PhyAddr = PioNonCached;
                PioNonCached = pN;
            } else {
                /* Not enough mem */
                Ret = PIO_ERR_ENOMEM;
            }
        }
        /* cache */
        else {
            pC = PioCached + Size;
            if (pC < PIO_CACHED_E) {
                PioBuf->PhyAddr = PioCached;
                PioCached = pC;
            } else {
                /* Not enough mem */
                Ret = PIO_ERR_ENOMEM;
            }
        }
        /* VirAddr */
        (void)AmbaMMU_PhysToVirt(PioBuf->PhyAddr, &PioBuf->VirAddr);

        /* unlock */
        (void)AmbaKAL_MutexGive(&PioMutex);
    }

    return Ret;
}

/**
 *  @brief      Sync CPU data cache to DRAM
 *
 *  @param      PioBuf : [IN] The PIO buffer struture used to run cache clean operation.
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_CacheClean(AMBA_PIO_s *PioBuf)
{
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(PioBuf);
#else
    (void)PioBuf;
#endif
    return AmbaCache_DataClean(PioBuf->VirAddr, PioBuf->Size);
}

/**
 *  @brief      invalidates CPU data cache
 *
 *  @param      PioBuf : [IN] The PIO buffer struture used to run cache invalidates operation.
 *
 *  @return     PIO error number
 */
UINT32 AmbaPIO_CacheInvalid(AMBA_PIO_s *PioBuf)
{
#ifdef AMBA_MISRA_FIX_H
    AmbaMisra_TouchUnused(PioBuf);
#else
    (void)PioBuf;
#endif
    return AmbaCache_DataInvalidate(PioBuf->VirAddr, PioBuf->Size);
}
