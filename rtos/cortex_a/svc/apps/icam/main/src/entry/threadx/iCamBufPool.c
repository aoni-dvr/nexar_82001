/**
*  @file iCamBufPool.c
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
*  @details icam buffer pool functions
*
*/

#include "AmbaTypes.h"
#include "AmbaMisraFix.h"
#include "AmbaDSP.h"
#include "AmbaPIO.h"
#if defined(CONFIG_BUILD_CV)
#include "cvapi_flexidag.h"
#include "cvapi_ambacv_flexidag.h"
#endif

#include "SvcLog.h"
#include "SvcMem.h"
#include "iCamBufPool.h"

#define SVC_LOG_BFP     "BFPL"

/**
 *  Initialization of buffer pool
 *  @param[in] pvMem memory information
 *  @return none
 */
void iCamBufPool_Init(void *pvMem)
{
    extern void *non_cache_heap_start, *cache_heap_start;
#if defined(CONFIG_ARM64) && (CONFIG_DDR_SIZE > 0xffe00000U)
    extern UINT64 *p_non_cache_heap_end, *p_cache_heap_end;   // Global variables in BSP
    #define NON_CACHE_HEAP_END      p_non_cache_heap_end
    #define CACHE_HEAP_END          p_cache_heap_end
#else
    extern void *non_cache_heap_end, *cache_heap_end;       // Linking script symbols
    #define NON_CACHE_HEAP_END      &non_cache_heap_end
    #define CACHE_HEAP_END          &cache_heap_end
#endif

    ULONG       Base, End;
    SVC_MEM_s   *pMem;
    const void  *pAddr;

    AmbaMisra_TypeCast(&pMem, &pvMem);

    pAddr = &non_cache_heap_start;
    AmbaMisra_TypeCast(&Base, &pAddr);
    pAddr = NON_CACHE_HEAP_END;
    AmbaMisra_TypeCast(&End, &pAddr);
    if (Base <= End) {
        pMem->Pool[SVC_MEM_TYPE_NC].BaseAddr = Base;
        pMem->Pool[SVC_MEM_TYPE_NC].Size     = (UINT32)(End - Base);
        pMem->Pool[SVC_MEM_TYPE_NC].PhyBaseAddr = Base;
    } else {
        SvcLog_NG(SVC_LOG_BFP, "## invalid non-cache memory base/end(0x%X/0x%X)"
                             , (UINT32)Base
                             , (UINT32)End);
    }

    pAddr = &cache_heap_start;
    AmbaMisra_TypeCast(&Base, &pAddr);
    pAddr = CACHE_HEAP_END;
    AmbaMisra_TypeCast(&End, &pAddr);
    if (Base <= End) {
        pMem->Pool[SVC_MEM_TYPE_CA].BaseAddr = Base;
        pMem->Pool[SVC_MEM_TYPE_CA].Size     = (UINT32)(End - Base);
        pMem->Pool[SVC_MEM_TYPE_CA].PhyBaseAddr = Base;
    } else {
        SvcLog_NG(SVC_LOG_BFP, "## invalid cache memory base/end(0x%X/0x%X)", (UINT32)Base, (UINT32)End);
    }

#if defined(CONFIG_DEVICE_TREE_SUPPORT)
    /* allocate dsp data buffer */
    {
        UINT32               Rval;
        AMBA_DSP_BUF_INFO_s  DspBufInfo;
    #if defined(CONFIG_PIO_MEM_ALLOC)
        AMBA_PIO_s           IoDataBuf;

        /* allocate io data buffer */
        Rval = AmbaPIO_MemAlloc(CONFIG_SVC_IO_HEAP_SIZE, 1U, &IoDataBuf);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_BFP, "## fail to alloc pio buffer, 0x%X", Rval, 0U);
            SvcLog_NG(SVC_LOG_BFP, "##   please increase CONFIG_PIO_SIZE", 0U, 0U);
        } else {
            pMem->Pool[SVC_MEM_TYPE_IO].BaseAddr    = IoDataBuf.PhyAddr;
            pMem->Pool[SVC_MEM_TYPE_IO].Size        = IoDataBuf.Size;
            pMem->Pool[SVC_MEM_TYPE_IO].PhyBaseAddr = IoDataBuf.PhyAddr;
            pMem->Pool[SVC_MEM_TYPE_IO].Cacheable   = IoDataBuf.Cacheable;
        }
    #endif

        Rval = AmbaDSP_MainGetBufInfo(DSP_BUF_TYPE_DATA, &DspBufInfo);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_BFP, "## fail to alloc dsp data buffer, 0x%X", Rval, 0U);
        } else {
            pMem->Pool[SVC_MEM_TYPE_DSP].BaseAddr    = DspBufInfo.BaseAddr;
            pMem->Pool[SVC_MEM_TYPE_DSP].Size        = DspBufInfo.Size;
            pMem->Pool[SVC_MEM_TYPE_DSP].PhyBaseAddr = DspBufInfo.BaseAddr;
            pMem->Pool[SVC_MEM_TYPE_DSP].Cacheable   = DspBufInfo.IsCached;
        }
    }
#endif

#if defined(CONFIG_BUILD_CV)
    {
        UINT32             ReqSize;
        flexidag_memblk_t  CvUsrMem;

#ifdef CONFIG_AMBALINK_BOOT_OS
        ReqSize = CONFIG_CV_MEM_RTOS_SIZE - CONFIG_ICAM_CV_USER_RESERVED;
#else
        ReqSize = (CONFIG_CV_MEM_SIZE - CONFIG_CV_MEM_SCHDR_SIZE) - CONFIG_ICAM_CV_USER_RESERVED;
#endif
        if (AmbaCV_UtilityCmaMemAlloc(ReqSize, 1U, &CvUsrMem) == OK) {
            pAddr = CvUsrMem.pBuffer;
            AmbaMisra_TypeCast(&Base, &pAddr);

            pMem->Pool[SVC_MEM_TYPE_CV].BaseAddr    = Base;
            pMem->Pool[SVC_MEM_TYPE_CV].Size        = CvUsrMem.buffer_size;
            pMem->Pool[SVC_MEM_TYPE_CV].PhyBaseAddr = CvUsrMem.buffer_daddr;
            pMem->Pool[SVC_MEM_TYPE_CV].Cacheable   = CvUsrMem.buffer_cacheable;
        } else {
            SvcLog_NG(SVC_LOG_BFP, "## fail to allocate cv user memory", 0U, 0U);
        }
    }
#endif

    AmbaMisra_TouchUnused(pvMem);
}
#ifdef CONFIG_BUILD_CV
extern void AmbaCV_UtilityCmaMemReset(void);
#endif
/**
 *  Initialization of CV buffer pool
 *  @param[in] pvMem memory information
 *  @return none
 */
void iCamBufPool_CV_Init(void *pvMem)
{
    ULONG       Base;
    SVC_MEM_s   *pMem;
    const void  *pAddr;

    AmbaMisra_TypeCast(&pMem, &pvMem);

#if defined(CONFIG_BUILD_CV)
    {
        UINT32             ReqSize;
        flexidag_memblk_t  CvUsrMem;

        AmbaCV_UtilityCmaMemReset();

#ifdef CONFIG_AMBALINK_BOOT_OS
        ReqSize = CONFIG_CV_MEM_RTOS_SIZE - CONFIG_ICAM_CV_USER_RESERVED;
#else
        ReqSize = (CONFIG_CV_MEM_SIZE - CONFIG_CV_MEM_SCHDR_SIZE) - CONFIG_ICAM_CV_USER_RESERVED;
#endif
        if (AmbaCV_UtilityCmaMemAlloc(ReqSize, 1U, &CvUsrMem) == OK) {
            pAddr = CvUsrMem.pBuffer;
            AmbaMisra_TypeCast(&Base, &pAddr);

            pMem->Pool[SVC_MEM_TYPE_CV].BaseAddr    = Base;
            pMem->Pool[SVC_MEM_TYPE_CV].Size        = CvUsrMem.buffer_size;
            pMem->Pool[SVC_MEM_TYPE_CV].PhyBaseAddr = CvUsrMem.buffer_daddr;
            pMem->Pool[SVC_MEM_TYPE_CV].Cacheable   = CvUsrMem.buffer_cacheable;
        } else {
            SvcLog_NG(SVC_LOG_BFP, "## fail to allocate cv user memory", 0U, 0U);
        }
    }
#endif

    AmbaMisra_TouchUnused(pvMem);
    AmbaMisra_TouchUnused(&pAddr);
    AmbaMisra_TouchUnused(&Base);
}
