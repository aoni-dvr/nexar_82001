/**
 *  @file AmbaDMA.c
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
 *  @details DMA Control APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaKAL.h"
#include "AmbaWrap.h"

#include "AmbaDrvEntry.h"
#include "AmbaDMA.h"

UINT32 AmbaDMA_DrvEntry(void)
{
    UINT32 RetVal = DMA_ERR_NONE;
    return RetVal;
}

/**
 *  AmbaDMA_ChannelAllocate - Allocate a dedidated dma channel for the specified purpose
 *  @param[in] DmaChanType The desired dma function
 *  @param[out] pDmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ChannelAllocate(UINT32 DmaChanType, UINT32 *pDmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;
    /* FIXED ME */
    (void)DmaChanType;
    (void)pDmaChanNo;

    return RetVal;
}

/**
 *  AmbaDMA_ChannelRelease - Release an allocated dma channel
 *  @param[in] DmaChanNo The allocated dma channel id
 *  @return error code
 */
UINT32 AmbaDMA_ChannelRelease(UINT32 DmaChanNo)
{
    UINT32 RetVal = DMA_ERR_NONE;
    /* FIXED ME */
    (void)DmaChanNo;
    return RetVal;
}

/**
 *  AmbaDMA_Transfer - Start the dma transfer
 *  @param[in] DmaChanNo A dma channel id (must acquire one before the transfer)
 *  @param[in] pDmaDesc The control block of the dma transfer
 *  @return error code
 */
UINT32 AmbaDMA_Transfer(UINT32 DmaChanNo, AMBA_DMA_DESC_s *pDmaDesc)
{
    UINT32 RetVal = DMA_ERR_NONE;
    /* FIXED ME */
    (void)DmaChanNo;
    (void)pDmaDesc;
    return RetVal;
}

/**
 *  AmbaDMA_Wait - Wait for dma transfer compeltion
 *  @param[in] DmaChanNo A dma channel id (must acquire one before the transfer to prevent conflict)
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
UINT32 AmbaDMA_Wait(UINT32 DmaChanNo, UINT32 TimeOut)
{
    UINT32 RetVal = DMA_ERR_NONE;
    /* FIXED ME */
    (void) DmaChanNo;
    (void) TimeOut;
    return RetVal;
}

