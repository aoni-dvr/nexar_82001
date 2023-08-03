/**
*  @file SvcPlat.c
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
*  @details svc platform dependent functions
*
*/

#include "AmbaTypes.h"

#include "SvcLog.h"
#include "SvcErrCode.h"
#include "SvcPlat.h"

SVC_PLAT_CB_s  *g_pPlatCbEntry = NULL;

/**
* initialization of platfom callback functions
* @return none
*/
void SvcPlat_InitCB(SVC_PLAT_CB_s *pPlatCB)
{
    g_pPlatCbEntry = pPlatCB;
}

/**
* platform cache clean function
* @param [in] address of memory
* @param [in] size of memory
* @return 0-OK, 1-NG
*/
UINT32 SvcPlat_CacheClean(ULONG Addr, ULONG Size)
{
    UINT32  Rval = SVC_OK;

    /* For some os platform (like linux), there is no cache operations. */
    /* We keep silent and bypass it                                     */

    if (g_pPlatCbEntry->pAmbaCacheClean != NULL) {
        /* Align address and size to AMBA_CACHE_LINE_SIZE */
        ULONG AlignAddr, AlignSize;
        AlignAddr = Addr & AMBA_CACHE_LINE_MASK;
        AlignSize = (((Addr - AlignAddr) + Size) + (AMBA_CACHE_LINE_SIZE - 1UL)) & AMBA_CACHE_LINE_MASK;
        Rval = g_pPlatCbEntry->pAmbaCacheClean(AlignAddr, AlignSize);
    }

    return Rval;
}

/**
* platform cache invalidate function
* @param [in] address of memory
* @param [in] size of memory
* @return 0-OK, 1-NG
*/
UINT32 SvcPlat_CacheInvalidate(ULONG Addr, ULONG Size)
{
    UINT32  Rval = SVC_OK;

    /* For some os platform (like linux), there is no cache operations. */
    /* We keep silent and bypass it                                     */

    if (g_pPlatCbEntry->pAmbaCacheInvalidate != NULL) {
        ULONG AlignAddr, AlignSize;
        /* Align address and size to AMBA_CACHE_LINE_SIZE */
        AlignAddr = Addr & AMBA_CACHE_LINE_MASK;
        AlignSize = (((Addr - AlignAddr) + Size) + (AMBA_CACHE_LINE_SIZE - 1UL)) & AMBA_CACHE_LINE_MASK;
        Rval = g_pPlatCbEntry->pAmbaCacheInvalidate(AlignAddr, AlignSize);
    }

    return Rval;
}

/**
* platform cache flush function
* @param [in] address of memory
* @param [in] size of memory
* @return 0-OK, 1-NG
*/
UINT32 SvcPlat_CacheFlush(ULONG Addr, ULONG Size)
{
    UINT32  Rval = SVC_OK;

    /* For some os platform (like linux), there is no cache operations. */
    /* We keep silent and bypass it                                     */

    if (g_pPlatCbEntry->pAmbaCacheFlush != NULL) {
        Rval = g_pPlatCbEntry->pAmbaCacheFlush(Addr, Size);
    }

    return Rval;
}
