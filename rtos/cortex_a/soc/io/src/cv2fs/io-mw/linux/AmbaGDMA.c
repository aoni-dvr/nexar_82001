/**
 *  @file AmbaGDMA.c
 *
 *  @copyright Copyright (c) 2020 Ambarella, Inc.
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
 *  @details 2D Graphics DMA Middleware APIs
 *
 */

#include "AmbaTypes.h"
#include "AmbaDef.h"
#include "AmbaMisraFix.h"
#include "AmbaKAL.h"

#include "AmbaDrvEntry.h"
#include "AmbaGDMA.h"
//#include "AmbaRTSL_GDMA.h"

#define AMBA_GDMA_TIMEOUT_FLAG      0x10000UL

typedef struct {
    UINT32  NumRequest;                 /* Number of the GDMA requests */
    UINT32  NumComplete;                /* Number of the finished GDMA requests */

    AMBA_GDMA_ISR_f CompletionCB[GDMA_NUM_INSTANCE];
    UINT32  CompletionCbArg[GDMA_NUM_INSTANCE];
} AMBA_GDMA_CTRL_s;

/**
 *  AmbaGDMA_DrvEntry - Initializes driver-wide data structures and resources
 *  @return error code
 *  @note this function is an internal using only API
 */
UINT32 AmbaGDMA_DrvEntry(void)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    return RetVal;
}

/**
 *  AmbaGDMA_GetInfo - Retrieve the controller status
 *  @param[out] pNumAvails Number of available transaction slots
 *  @return error code
 */
UINT32 AmbaGDMA_GetInfo(UINT32 * pNumAvails)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    (void) pNumAvails;

    return RetVal;
}

/**
 *  AmbaGDMA_WaitAllCompletion - Wait until the last transaction done
 *  @param[in] TimeOut The maximum wait time
 *  @return error code
 */
UINT32 AmbaGDMA_WaitAllCompletion(UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaGDMA_LinearCopy - Schedule a linear copy transaction
 *  @param[in] pLinearBlit The control block of the linear copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_LinearCopy(const AMBA_GDMA_LINEAR_s * pLinearBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;
    (void) pLinearBlit;
    (void) NotifierFunc;
    (void) NotifierFuncArg;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaGDMA_BlockCopy - Schedule a block copy transaction
 *  @param[in] pBlockBlit The control block of the block copy
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_BlockCopy(const AMBA_GDMA_BLOCK_s * pBlockBlit, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    (void) pBlockBlit;
    (void) NotifierFunc;
    (void) NotifierFuncArg;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaGDMA_ColorKeying - Schedule a chroma key compositing transaction
 *  @param[in] pBlockBlit The control block of the chroma key compositing
 *  @param[in] TransparentColor The color hue in foreground image treated as transparent
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_ColorKeying(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 TransparentColor, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    (void) pBlockBlit;
    (void) TransparentColor;
    (void) NotifierFunc;
    (void) NotifierFuncArg;
    (void) TimeOut;
    return RetVal;
}

/**
 *  AmbaGDMA_AlphaBlending - Schedule a alpha blending transaction
 *  @param[in] pBlockBlit The control block of the alpha blending
 *  @param[in] AlphaVal The global alpha value
 *  @param[in] BlendMode The alpha value is premultiplied or not
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 *  @param[in] TimeOut The maximum tolerance time to schedule this transaction
 *  @return error code
 */
UINT32 AmbaGDMA_AlphaBlending(const AMBA_GDMA_BLOCK_s * pBlockBlit, UINT32 AlphaVal, UINT32 BlendMode, AMBA_GDMA_ISR_f NotifierFunc, UINT32 NotifierFuncArg, UINT32 TimeOut)
{
    UINT32 RetVal = GDMA_ERR_NONE;

    (void) pBlockBlit;
    (void) AlphaVal;
    (void) BlendMode;
    (void) NotifierFunc;
    (void) NotifierFuncArg;
    (void) TimeOut;

    return RetVal;
}

/**
 *  GDMA_PushIsrFunc - Booking completion callback
 *  @param[in] NotifierFunc The completion callback
 *  @param[in] NotifierFuncArg The optional argument of the completion callback
 */

/**
 *  GDMA_TransferDoneISR - Completion interrupt handler
 *  @param[in] Arg Optional argument
 */

