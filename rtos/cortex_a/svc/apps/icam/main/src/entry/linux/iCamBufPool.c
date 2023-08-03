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

#include <sys/mman.h>

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
    static INT32  fdiCamCa = -1;
    static INT32  fdiCamIo = -1;

    void        *pVirtAddr;
    ULONG       Base;
    SVC_MEM_s   *pMem;

    AmbaMisra_TypeCast(&pMem, &pvMem);

    if (fdiCamCa == -1) {
        pVirtAddr = mmap(NULL,
                         CONFIG_SVC_CACHE_HEAP_SIZE,
                         PROT_READ | PROT_WRITE,
                         MAP_ANON | MAP_SHARED,
                         -1,
                         0);

        if (pVirtAddr == MAP_FAILED) {
            SvcLog_NG(SVC_LOG_BFP, "## fail to mmap icam_ca", 0U, 0U);
        } else {
            AmbaMisra_TypeCast(&Base, &pVirtAddr);
            pMem->Pool[SVC_MEM_TYPE_CA].BaseAddr = Base;
            pMem->Pool[SVC_MEM_TYPE_CA].Size     = CONFIG_SVC_CACHE_HEAP_SIZE;

            fdiCamCa = 1;
        }
    }

    if (fdiCamIo == -1) {
        UINT32               Rval;
        AMBA_DSP_BUF_INFO_s  DspBufInfo;
        AMBA_PIO_s           IoDataBuf;

        /* allocate io data buffer */
        Rval = AmbaPIO_MemAlloc(CONFIG_SVC_IO_HEAP_SIZE, 1U, &IoDataBuf);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_BFP, "## fail to alloc pio buffer, 0x%X", Rval, 0U);
            SvcLog_NG(SVC_LOG_BFP, "##   please increase CONFIG_PIO_SIZE", 0U, 0U);
        } else {
            pMem->Pool[SVC_MEM_TYPE_IO].BaseAddr    = IoDataBuf.VirAddr;
            pMem->Pool[SVC_MEM_TYPE_IO].Size        = IoDataBuf.Size;
            pMem->Pool[SVC_MEM_TYPE_IO].PhyBaseAddr = IoDataBuf.PhyAddr;
            pMem->Pool[SVC_MEM_TYPE_IO].Cacheable   = IoDataBuf.Cacheable;
        }

        /* allocate dsp data buffer */
        Rval = AmbaDSP_MainGetBufInfo(DSP_BUF_TYPE_DATA, &DspBufInfo);
        if (Rval != OK) {
            SvcLog_NG(SVC_LOG_BFP, "## fail to alloc dsp data buffer, 0x%X", Rval, 0U);
        } else {
            pMem->Pool[SVC_MEM_TYPE_DSP].BaseAddr    = DspBufInfo.BaseAddr;
            pMem->Pool[SVC_MEM_TYPE_DSP].Size        = DspBufInfo.Size;
            pMem->Pool[SVC_MEM_TYPE_DSP].PhyBaseAddr = DspBufInfo.BasePhysAddr;
            pMem->Pool[SVC_MEM_TYPE_DSP].Cacheable   = DspBufInfo.IsCached;
        }

        fdiCamIo = 1U;
    }

#if defined(CONFIG_BUILD_CV)
    {
        UINT32             ReqSize;
        const void         *pAddr;
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
