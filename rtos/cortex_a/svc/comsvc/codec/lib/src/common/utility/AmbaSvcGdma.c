/**
*  @file AmbaGdma.c
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
*  @details Gdma related APIs
*
*/

#include "AmbaTypes.h"
#include "AmbaKAL.h"
#include "AmbaPrint.h"
#include "AmbaGDMA.h"
#include "AmbaWrap.h"
#include "AmbaMisraFix.h"
#include "AmbaCodecCom.h"
#include "AmbaSvcWrap.h"
#include "AmbaSvcGdma.h"

#define GDMA_BLOCK_COPY_USE             (1U)
#define MIN_GDMA_COPY_SIZE              (0x1000UL)

/**
* mem copy by gdma
* @param [in]  pDst pointer to destimation buf
* @param [in]  pSrc pointer to source buf
* @param [in]  CopySize copy size
* @return ErrorCode
*/
UINT32 AmbaSvcGdma_Copy(UINT8 *pDst, UINT8 *pSrc, UINT32 CopySize)
{
    UINT8               *pSource = pSrc, *pDestination = pDst;
    UINT32              Err, Rval = CODEC_OK, DataSize = CopySize;

    if (CopySize < MIN_GDMA_COPY_SIZE) {
        /* if copy size is less than MIN_GDMA_COPY_SIZE, we use memcpy */
        Err = AmbaWrap_memcpy(pDst, pSrc, CopySize);
        if (Err != CODEC_OK) {
            AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaWrap_memcpy failed %u", Err, 0U, 0U, 0U, 0U);
        }
    } else {
        /* clear source cache */
        {
            ULONG  SrcAddr, AlignStart;
            ULONG  AlignSize;

            AmbaMisra_TypeCast(&SrcAddr, &pSource);

            if (0U < AmbaSvcWrap_CacheChk(SrcAddr, CopySize)) {
                AlignStart = SrcAddr & AMBA_CACHE_LINE_MASK;
                AlignSize  = (((SrcAddr + CopySize) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
                AlignSize  -= AlignStart;
                Err = AmbaSvcWrap_CacheClean(AlignStart, AlignSize);
                if (Err != CODEC_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaSvcWrap_CacheClean failed %u", Err, 0U, 0U, 0U, 0U);
                }
            }
        }

        /* gdma dst size and addr shoud be cache aligned */
        /* we use memcpy to copy to the addr not cache aligned */
        {
            ULONG  DstAddr, DstAligned, Offset;

            AmbaMisra_TypeCast(&DstAddr, &pDestination);
            DstAligned = (((DstAddr) + (AMBA_CACHE_LINE_SIZE - 1U)) & AMBA_CACHE_LINE_MASK);
            Offset     = DstAligned - DstAddr;

            if (Offset > 0U) {
                Err = AmbaWrap_memcpy(pDestination, pSource, Offset);
                if (Err != CODEC_OK) {
                    AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaWrap_memcpy failed %u", Err, 0U, 0U, 0U, 0U);
                }
                pSource      = &(pSource[Offset]);
                pDestination = &(pDestination[Offset]);
                DataSize     -= (UINT32)Offset;
            }
        }

        /* gdma copy */
#if defined(GDMA_BLOCK_COPY_USE)
        {
            AMBA_GDMA_BLOCK_s  BCopy;
            UINT32             Remainder, GdmaSize;

            Remainder         = DataSize & (UINT32)AMBA_CACHE_LINE_MASK;
            BCopy.PixelFormat = AMBA_GDMA_8_BIT;

            while (0U < Remainder) {
                Rval = CODEC_OK;

                BCopy.pSrcImg   = pSource;
                BCopy.pDstImg   = pDestination;
                BCopy.BltWidth  = Remainder;
                if (AMBA_GDMA_MAX_WIDTH < BCopy.BltWidth) {
                    BCopy.BltWidth = AMBA_GDMA_MAX_WIDTH;
                }

                BCopy.BltHeight = Remainder / BCopy.BltWidth;
                if (AMBA_GDMA_MAX_HEIGHT < BCopy.BltHeight) {
                    BCopy.BltHeight = AMBA_GDMA_MAX_HEIGHT;
                }

                BCopy.SrcRowStride = BCopy.BltWidth;
                BCopy.DstRowStride = BCopy.BltWidth;

            #if !defined (CONFIG_THREADX)
                /* for qnx and linux, we need to transfer virtual address to physical address */
                {
                    Err =  AmbaSvcWrap_Vir2Phys(BCopy.pSrcImg, &(BCopy.pSrcImg));
                    if (Err != CODEC_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID,"SvcBuffer_Vir2Phys failed %u", Err, 0U, 0U, 0U, 0U);
                        Rval = CODEC_ERROR_GENERAL_ERROR;
                    }

                    Err =  AmbaSvcWrap_Vir2Phys(BCopy.pDstImg, &(BCopy.pDstImg));
                    if (Err != CODEC_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID,"SvcBuffer_Vir2Phys failed %u", Err, 0U, 0U, 0U, 0U);
                        Rval = CODEC_ERROR_GENERAL_ERROR;
                    }
                }
            #endif

                Err = AmbaGDMA_BlockCopy(&BCopy, NULL, 0U, AMBA_KAL_NO_WAIT);
                if (Err != GDMA_ERR_NONE) {
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                    Err = AmbaKAL_TaskSleep(1U);
                    if (Err != CODEC_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_TaskSleep failed %u", Err, 0U, 0U, 0U, 0U);
                    }
                }

                if (Rval == CODEC_OK) {
                    GdmaSize = BCopy.BltWidth * BCopy.BltHeight;
                    Remainder -= GdmaSize;

                    pSource      = &(pSource[GdmaSize]);
                    pDestination = &(pDestination[GdmaSize]);
                    DataSize     -= GdmaSize;
                }
            }
        }
#else
        {
            AMBA_GDMA_LINEAR_s  LCopy;
            UINT32              Remainder;

            Remainder         = DataSize & AMBA_CACHE_LINE_MASK;
            LCopy.PixelFormat = AMBA_GDMA_8_BIT;

            while (0U < Remainder) {
                Rval = CODEC_OK;

                LCopy.pSrcImg   = pSource;
                LCopy.pDstImg   = pDestination;
                LCopy.NumPixels = Remainder;
                if (AMBA_GDMA_MAX_WIDTH < LCopy.NumPixels) {
                    LCopy.NumPixels = AMBA_GDMA_MAX_WIDTH;
                }

            #if !defined (CONFIG_THREADX)
                /* for qnx and linux, we need to transfer virtual address to physical address */
                {
                    Err =  AmbaSvcWrap_Vir2Phys(LCopy.pSrcImg, &(LCopy.pSrcImg));
                    if (Err != CODEC_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID,"SvcBuffer_Vir2Phys failed %u", Err, 0U, 0U, 0U, 0U);
                        Rval = CODEC_ERROR_GENERAL_ERROR;
                    }

                    Err =  AmbaSvcWrap_Vir2Phys(LCopy.pDstImg, &(LCopy.pDstImg));
                    if (Err != CODEC_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID,"SvcBuffer_Vir2Phys failed %u", Err, 0U, 0U, 0U, 0U);
                        Rval = CODEC_ERROR_GENERAL_ERROR;
                    }
                }
            #endif

                Err = AmbaGDMA_LinearCopy(&LCopy, NULL, 0U, AMBA_KAL_NO_WAIT);
                if (Err != GDMA_ERR_NONE) {
                    Rval = CODEC_ERROR_GENERAL_ERROR;
                    Err = AmbaKAL_TaskSleep(1UL);
                    if (Err != CODEC_OK) {
                        AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaKAL_TaskSleep failed %u", Err, 0U, 0U, 0U, 0U);
                    }
                }

                if (Rval == CODEC_OK) {
                    Remainder -= LCopy.NumPixels;

                    pSource      = &(pSource[LCopy.NumPixels]);
                    pDestination = &(pDestination[LCopy.NumPixels]);
                    DataSize     -= LCopy.NumPixels;
                }
            }
        }
#endif
        /* use memcpy to copy to the end of dst buffer */
        if (DataSize > 0U) {
            Err = AmbaWrap_memcpy(pDestination, pSource, DataSize);
            if (Err != CODEC_OK) {
                AmbaPrint_ModulePrintUInt5(AMBA_CODEC_PRINT_MODULE_ID, "AmbaWrap_memcpy failed %u", Err, 0U, 0U, 0U, 0U);
            }
        }
    }

    return Rval;
}
