/*
 * Copyright (c) 2021 Ambarella International LP
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

#include "AmbaTypes.h"
#include "AmbaCSL_DMA.h"
#include "AmbaCSL_Scratchpad.h"

/**
 *  AmbaCSL_DmaSetChanSub0Func - Assign the DMA cannel type to the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] DmaChanFunc DMA channel function
 */
#if defined(CONFIG_SOC_CV2)
static void AmbaCSL_DmaSetChanSub0Func(UINT32 DmaChanNo, UINT32 DmaChanFunc)
{
    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_0:
        pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan0Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_1:
        pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan1Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_2:
        pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan2Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_3:
        pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan3Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_4:
        pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan4Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_5:
        pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan5Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_6:
        pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan6Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_7:
        pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan7Select = (UINT8)DmaChanFunc;
        break;
    default:
        /* Do nothing */
        break;
    }
}
#else
static void AmbaCSL_DmaSetChanSub0Func(UINT32 DmaChanNo, UINT32 DmaChanFunc)
{
    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_0:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan0Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_1:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan1Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_2:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan2Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_3:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan3Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_4:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan4Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_5:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan5Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_6:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan6Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_7:
        pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan7Select = (UINT8)DmaChanFunc;
        break;
    default:
        /* Do nothing */
        break;
    }
}
#endif

/**
 *  AmbaCSL_DmaSetChanSub1Func - Assign the DMA cannel type to the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] DmaChanFunc DMA channel function
 */
#if defined(CONFIG_SOC_CV2)
static void AmbaCSL_DmaSetChanSub1Func(UINT32 DmaChanNo, UINT32 DmaChanFunc)
{
    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_8:
        pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan0Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_9:
        pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan1Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_10:
        pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan2Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_11:
        pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan3Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_12:
        pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan4Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_13:
        pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan5Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_14:
        pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan6Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_15:
        pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan7Select = (UINT8)DmaChanFunc;
        break;

    default:
        /* Do nothing */
        break;
    }
}
#else
static void AmbaCSL_DmaSetChanSub1Func(UINT32 DmaChanNo, UINT32 DmaChanFunc)
{
    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_8:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan0Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_9:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan1Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_10:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan2Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_11:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan3Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_12:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan4Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_13:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan5Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_14:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan6Select = (UINT8)DmaChanFunc;
        break;
    case AMBA_DMA_CHANNEL_15:
        pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan7Select = (UINT8)DmaChanFunc;
        break;

    default:
        /* Do nothing */
        break;
    }
}
#endif

/**
 *  AmbaCSL_DmaSetChanFunc - Assign the DMA cannel type to the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @param[in] DmaChanFunc DMA channel function
 */
void AmbaCSL_DmaSetChanFunc(UINT32 DmaChanNo, UINT32 DmaChanFunc)
{
    if (DmaChanNo <= AMBA_DMA_CHANNEL_7) {
        AmbaCSL_DmaSetChanSub0Func(DmaChanNo, DmaChanFunc);
    } else {
        AmbaCSL_DmaSetChanSub1Func(DmaChanNo, DmaChanFunc);
    }
}

/**
 *  AmbaCSL_DmaGetChanSub0Func - Get the DMA cannel type from the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @return DMA channel function
 */
#if defined(CONFIG_SOC_CV2)
static UINT32 AmbaCSL_DmaGetChanSub0Func(UINT32 DmaChanNo)
{
    UINT32 DmaChanFunc = 0xFFFFFFFFU;

    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_0:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan0Select;
        break;
    case AMBA_DMA_CHANNEL_1:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan1Select;
        break;
    case AMBA_DMA_CHANNEL_2:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan2Select;
        break;
    case AMBA_DMA_CHANNEL_3:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect0.Chan3Select;
        break;
    case AMBA_DMA_CHANNEL_4:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan4Select;
        break;
    case AMBA_DMA_CHANNEL_5:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan5Select;
        break;
    case AMBA_DMA_CHANNEL_6:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan6Select;
        break;
    case AMBA_DMA_CHANNEL_7:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma0ChanSelect1.Chan7Select;
        break;

    default:
        /* Do nothing */
        break;
    }
    return DmaChanFunc;
}
#else
static UINT32 AmbaCSL_DmaGetChanSub0Func(UINT32 DmaChanNo)
{
    UINT32 DmaChanFunc = 0xFFFFFFFFU;

    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_0:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan0Select;
        break;
    case AMBA_DMA_CHANNEL_1:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan1Select;
        break;
    case AMBA_DMA_CHANNEL_2:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan2Select;
        break;
    case AMBA_DMA_CHANNEL_3:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect0.Chan3Select;
        break;
    case AMBA_DMA_CHANNEL_4:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan4Select;
        break;
    case AMBA_DMA_CHANNEL_5:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan5Select;
        break;
    case AMBA_DMA_CHANNEL_6:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan6Select;
        break;
    case AMBA_DMA_CHANNEL_7:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma0ChanSelect1.Chan7Select;
        break;

    default:
        /* Do nothing */
        break;
    }
    return DmaChanFunc;
}
#endif

/**
 *  AmbaCSL_DmaGetChanSub1Func - Get the DMA cannel type from the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @return DMA channel function
 */
#if defined(CONFIG_SOC_CV2)
/**
 *  AmbaCSL_DmaGetChanSub1Func - Get the DMA cannel type from the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @return DMA channel function
 */
static UINT32 AmbaCSL_DmaGetChanSub1Func(UINT32 DmaChanNo)
{
    UINT32 DmaChanFunc = 0xFFFFFFFFU;

    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_8:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan0Select;
        break;
    case AMBA_DMA_CHANNEL_9:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan1Select;
        break;
    case AMBA_DMA_CHANNEL_10:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan2Select;
        break;
    case AMBA_DMA_CHANNEL_11:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect0.Chan3Select;
        break;
    case AMBA_DMA_CHANNEL_12:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan4Select;
        break;
    case AMBA_DMA_CHANNEL_13:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan5Select;
        break;
    case AMBA_DMA_CHANNEL_14:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan6Select;
        break;
    case AMBA_DMA_CHANNEL_15:
        DmaChanFunc = pAmbaScratchpadS_Reg->Dma1ChanSelect1.Chan7Select;
        break;

    default:
        /* Do nothing */
        break;
    }
    return DmaChanFunc;
}
#else
static UINT32 AmbaCSL_DmaGetChanSub1Func(UINT32 DmaChanNo)
{
    UINT32 DmaChanFunc = 0xFFFFFFFFU;

    switch (DmaChanNo) {
    case AMBA_DMA_CHANNEL_8:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan0Select;
        break;
    case AMBA_DMA_CHANNEL_9:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan1Select;
        break;
    case AMBA_DMA_CHANNEL_10:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan2Select;
        break;
    case AMBA_DMA_CHANNEL_11:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect0.Chan3Select;
        break;
    case AMBA_DMA_CHANNEL_12:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan4Select;
        break;
    case AMBA_DMA_CHANNEL_13:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan5Select;
        break;
    case AMBA_DMA_CHANNEL_14:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan6Select;
        break;
    case AMBA_DMA_CHANNEL_15:
        DmaChanFunc = pAmbaScratchpadNS_Reg->Dma1ChanSelect1.Chan7Select;
        break;

    default:
        /* Do nothing */
        break;
    }
    return DmaChanFunc;
}
#endif

/**
 *  AmbaCSL_DmaiGetChanFunc - Get the DMA cannel type from the corresponding scrachpad location
 *  @param[in] DmaChanNo DMA channel number
 *  @return DMA channel function
 */
UINT32 AmbaCSL_DmaGetChanFunc(UINT32 DmaChanNo)
{
    UINT32 DmaChanFunc = 0xFFFFFFFFU;

    if (DmaChanNo <= AMBA_DMA_CHANNEL_7) {
        DmaChanFunc = AmbaCSL_DmaGetChanSub0Func(DmaChanNo);
    } else {
        DmaChanFunc = AmbaCSL_DmaGetChanSub1Func(DmaChanNo);
    }

    return DmaChanFunc;
}

/**
 *  AmbaCSL_DmaClearStatus - Clear status
 *  @param[in] pDmaChanReg pointer to dma channel control regsiters
 */
void AmbaCSL_DmaClearStatus(volatile AMBA_DMA_CHAN_REG_s *pDmaChanReg)
{
    pDmaChanReg->Status = 0U;
}

/**
 *  AmbaCSL_DmaGetIrqStatus - Get interrupt status
 *  @param[in] pDmaReg pointer to dma control registers
 */
UINT32 AmbaCSL_DmaGetIrqStatus(const AMBA_DMA_REG_s *pDmaReg)
{
    return pDmaReg->IrqStatus;
}
