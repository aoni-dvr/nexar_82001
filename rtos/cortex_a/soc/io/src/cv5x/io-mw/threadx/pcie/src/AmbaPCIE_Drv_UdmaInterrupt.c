/**
 *  @file AmbaPCIE_Drv_UdmaInterrupt.c
 *
 *  Copyright (c) 2021 Ambarella International LP
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
 *  @details PCIe UDMA driver higher level functions (for interrupt routine). Providing example usage of lower layer.
 */
#include "AmbaPCIE_Drv.h"

/****************************************************************************/
/*                                                                          */
/****************************************************************************/
static UINT32 checkInterrupts(
    PCIE_Bool errorInterrupt,
    PCIE_Bool error_expected_result,
    PCIE_Bool doneInterrupt,
    PCIE_Bool done_expected_result)
{
    UINT32 result = 0U;
    /* Check if one of flags is set to expected value
     * Return 1 if TRUE, otherwise 0 */
    if ( (doneInterrupt == error_expected_result) || (errorInterrupt == done_expected_result) ) {
        result = 1U;
    }

    return (result);
}

UINT32
PCIE_UDMA_Isr(const PCIE_PrivateData * pD)
{
    UINT32 ii;
    UINT32 result;
    PCIE_Bool errorInterrupt = PCIE_FALSE, doneInterrupt = PCIE_FALSE;
    PCIE_td_cfg *p_td_cfg;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        p_td_cfg   = pD->p_td_cfg;

        /* Check whether a DMA transfer has completed or error occurred
         * We only handle one interrupt at a time.  If more than one is
         * active, we will be called again
         */
        for (ii = 0; ii < PCIE_NUM_UDMA_CHANNELS; ii++) {
            (void)PCIE_UDMA_GetErrorInterrupt(pD, ii, &errorInterrupt);
            (void)PCIE_UDMA_GetDoneInterrupt(pD, ii, &doneInterrupt);
            if ( (errorInterrupt != PCIE_FALSE) || (doneInterrupt != PCIE_FALSE) )  {
                if (p_td_cfg != NULL) {
                    p_td_cfg->channels[ii].hwState = PCIE_CHANNEL_FREE;
                }
                break;
            }
        }
        /* If we received an interrupt, process the ALTD list, then callback to app if
         * we have a callback function
         */
        if ( checkInterrupts(errorInterrupt, PCIE_TRUE, doneInterrupt, PCIE_TRUE) == 1U ) {
            if (p_td_cfg != NULL) {
                (void)td_cfg_processTdsInAltd(pD);
            }
            if (pD->p_td_func != NULL) {
                pD->p_td_func(ii, (doneInterrupt != PCIE_FALSE) ? PCIE_TRUE : PCIE_FALSE);
            }
        }
        result = PCIE_ERR_SUCCESS;
    }
    return (result);
}

/****************************************************************************/
/* UDMA Enable or Disable Done interrupts                                   */
/****************************************************************************/
static UINT32 EnableUdmaInterruptDoneRegs(const PCIE_PrivateData * pD,UINT32 channel)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = 0U;
    struct PCIE_IClientUdma_s *pcieAddr;

    switch (channel) {
    case 0:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_DONE_WIDTH, DMA_INT_ENA_CH0_DONE_MASK,
                                  DMA_INT_ENA_DONE_WOSET, DMA_INT_ENA_DONE_WOCLR, regVal);
        break;
    case 1:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_DONE_WIDTH, DMA_INT_ENA_CH1_DONE_MASK,
                                  DMA_INT_ENA_DONE_WOSET, DMA_INT_ENA_DONE_WOCLR, regVal);
        break;
    case 2:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_DONE_WIDTH, DMA_INT_ENA_CH2_DONE_MASK,
                                  DMA_INT_ENA_DONE_WOSET, DMA_INT_ENA_DONE_WOCLR, regVal);
        break;
    case 3:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_DONE_WIDTH, DMA_INT_ENA_CH3_DONE_MASK,
                                  DMA_INT_ENA_DONE_WOSET, DMA_INT_ENA_DONE_WOCLR, regVal);
        break;
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    if (result == PCIE_ERR_SUCCESS) {
        pcieAddr = pD->p_ud_base;
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int_ena, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 DisableUdmaInterruptDoneRegs(const PCIE_PrivateData * pD,UINT32 channel)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = 0U;
    struct PCIE_IClientUdma_s *pcieAddr;

    switch (channel) {
    case 0:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_DONE_WIDTH, DMA_INT_DIS_CH0_DONE_MASK, DMA_INT_DIS_DONE_WOCLR, regVal);
        break;
    case 1:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_DONE_WIDTH, DMA_INT_DIS_CH1_DONE_MASK, DMA_INT_DIS_DONE_WOCLR, regVal);
        break;
    case 2:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_DONE_WIDTH, DMA_INT_DIS_CH2_DONE_MASK, DMA_INT_DIS_DONE_WOCLR, regVal);
        break;
    case 3:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_DONE_WIDTH, DMA_INT_DIS_CH3_DONE_MASK, DMA_INT_DIS_DONE_WOCLR, regVal);
        break;
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    if (result == PCIE_ERR_SUCCESS) {
        pcieAddr = pD->p_ud_base;
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int_dis, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
UINT32
PCIE_UDMA_ControlDoneInterrupts(
    const PCIE_PrivateData * pD,
    UINT32                 channel,
    PCIE_EnableOrDisable     enableOrDisable)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:
            result = EnableUdmaInterruptDoneRegs(pD, channel);
            break;
        case PCIE_DISABLE_PARAM:
            result = DisableUdmaInterruptDoneRegs(pD, channel);
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* UDMA Enable or Disable Error interrupts                                  */
/****************************************************************************/
static UINT32 EnableUdmaInterruptErrorRegs(const PCIE_PrivateData * pD,UINT32 channel)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = 0U;
    struct PCIE_IClientUdma_s *pcieAddr;

    switch (channel) {
    case 0:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_ERR_WIDTH, DMA_INT_ENA_CH0_ERR_MASK,
                                  DMA_INT_ENA_ERR_WOSET, DMA_INT_ENA_ERR_WOCLR, regVal);
        break;
    case 1:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_ERR_WIDTH, DMA_INT_ENA_CH1_ERR_MASK,
                                  DMA_INT_ENA_ERR_WOSET, DMA_INT_ENA_ERR_WOCLR, regVal);
        break;
    case 2:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_ERR_WIDTH, DMA_INT_ENA_CH2_ERR_MASK,
                                  DMA_INT_ENA_ERR_WOSET, DMA_INT_ENA_ERR_WOCLR, regVal);
        break;
    case 3:
        regVal = PCIE_RegFldClear(DMA_INT_ENA_ERR_WIDTH, DMA_INT_ENA_CH3_ERR_MASK,
                                  DMA_INT_ENA_ERR_WOSET, DMA_INT_ENA_ERR_WOCLR, regVal);
        break;
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    if (result == PCIE_ERR_SUCCESS) {
        pcieAddr = pD->p_ud_base;
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int_ena, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 DisableUdmaInterruptErrorRegs(const PCIE_PrivateData * pD,UINT32 channel)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = 0U;
    struct PCIE_IClientUdma_s *pcieAddr;

    switch (channel) {
    case 0:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_ERR_WIDTH, DMA_INT_DIS_CH0_ERR_MASK, DMA_INT_DIS_ERR_WOCLR, regVal);
        break;
    case 1:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_ERR_WIDTH, DMA_INT_DIS_CH1_ERR_MASK, DMA_INT_DIS_ERR_WOCLR, regVal);
        break;
    case 2:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_ERR_WIDTH, DMA_INT_DIS_CH2_ERR_MASK, DMA_INT_DIS_ERR_WOCLR, regVal);
        break;
    case 3:
        regVal = PCIE_RegFldSet(DMA_INT_DIS_ERR_WIDTH, DMA_INT_DIS_CH3_ERR_MASK, DMA_INT_DIS_ERR_WOCLR, regVal);
        break;
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    if (result == PCIE_ERR_SUCCESS) {
        pcieAddr = pD->p_ud_base;
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int_dis, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32
PCIE_UDMA_ControlErrInterrupts(
    const PCIE_PrivateData * pD,
    UINT32                 channel,
    PCIE_EnableOrDisable     enableOrDisable)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:
            result = EnableUdmaInterruptErrorRegs(pD, channel);
            break;
        case PCIE_DISABLE_PARAM:
            result = DisableUdmaInterruptErrorRegs(pD, channel);
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* UDMA check done interrupt for channel                                    */
/****************************************************************************/
static void GetUdmaIntDoneRegsStatusCh0(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if ((PCIE_RegFldRead(DMA_INT_CH0_DONE_MASK, DMA_INT_CH0_DONE_SHIFT, udmaReg)) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH0_DONE_MASK, DMA_INT_CH0_DONE_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaIntDoneRegsStatusCh1(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if ((PCIE_RegFldRead(DMA_INT_CH1_DONE_MASK, DMA_INT_CH1_DONE_SHIFT, udmaReg)) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH1_DONE_MASK, DMA_INT_CH1_DONE_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaIntDoneRegsStatusCh2(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if ((PCIE_RegFldRead(DMA_INT_CH2_DONE_MASK, DMA_INT_CH2_DONE_SHIFT, udmaReg)) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH2_DONE_MASK, DMA_INT_CH2_DONE_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaIntDoneRegsStatusCh3(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if ((PCIE_RegFldRead(DMA_INT_CH3_DONE_MASK, DMA_INT_CH3_DONE_SHIFT, udmaReg)) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH3_DONE_MASK, DMA_INT_CH3_DONE_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaInterruptDoneRegsStatus(
    UINT32                    channel,
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    struct PCIE_IClientUdma_s * udmaAddr = pcieAddr;
    PCIE_Bool * interruptFlag = pIntActive;
    static const UdmaInterruptRegsStatusFunctions GetUdmaIntDoneRegsFuncArray[PCIE_NUM_UDMA_CHANNELS] = {
        [0] = GetUdmaIntDoneRegsStatusCh0,
        [1] = GetUdmaIntDoneRegsStatusCh1,
        [2] = GetUdmaIntDoneRegsStatusCh2,
        [3] = GetUdmaIntDoneRegsStatusCh3,
    };
    GetUdmaIntDoneRegsFuncArray[channel](udmaReg, udmaAddr, interruptFlag);
    return;
}
UINT32
PCIE_UDMA_GetDoneInterrupt(
    const PCIE_PrivateData * pD,
    UINT32                 channel,
    PCIE_Bool *              pIntActive)
{
    struct PCIE_IClientUdma_s *pcieAddr;
    UINT32 udmaReg;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) ||
         (pIntActive == NULL) ||
         (channel >= (UINT32)PCIE_NUM_UDMA_CHANNELS) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ud_base;

        udmaReg = PCIE_RegPtrRead32(&pcieAddr->dma_common.common_udma_int);
        *pIntActive = PCIE_FALSE;
        GetUdmaInterruptDoneRegsStatus(channel, udmaReg, pcieAddr, pIntActive);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* UDMA Check error interrupt for channel                                   */
/****************************************************************************/
static void GetUdmaIntErrorRegsStatusCh0(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if (PCIE_RegFldRead(DMA_INT_CH0_ERR_MASK, DMA_INT_CH0_ERR_SHIFT, udmaReg) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH0_ERR_MASK, DMA_INT_CH0_ERR_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaIntErrorRegsStatusCh1(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if (PCIE_RegFldRead(DMA_INT_CH1_ERR_MASK, DMA_INT_CH1_ERR_SHIFT, udmaReg) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH1_ERR_MASK, DMA_INT_CH1_ERR_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaIntErrorRegsStatusCh2(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if (PCIE_RegFldRead(DMA_INT_CH2_ERR_MASK, DMA_INT_CH2_ERR_SHIFT, udmaReg) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH2_ERR_MASK, DMA_INT_CH2_ERR_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaIntErrorRegsStatusCh3(
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    if (PCIE_RegFldRead(DMA_INT_CH3_ERR_MASK, DMA_INT_CH3_ERR_SHIFT, udmaReg) != 0U) {
        UINT32 value = 0U;
        value = PCIE_RegFldWrite(DMA_INT_CH3_ERR_MASK, DMA_INT_CH3_ERR_SHIFT, value, 1);
        PCIE_RegPtrWrite32(&pcieAddr->dma_common.common_udma_int, value);
        *pIntActive = PCIE_TRUE;
    }
    return;
}
static void GetUdmaInterruptErrorRegsStatus(
    UINT32                    channel,
    UINT32                    udmaReg,
    struct PCIE_IClientUdma_s * pcieAddr,
    PCIE_Bool *                 pIntActive)
{
    struct PCIE_IClientUdma_s * udmaAddr = pcieAddr;
    PCIE_Bool * interruptFlag = pIntActive;
    static const UdmaInterruptRegsStatusFunctions GetUdmaIntErrorRegsFuncArray[PCIE_NUM_UDMA_CHANNELS] = {
        [0] = GetUdmaIntErrorRegsStatusCh0,
        [1] = GetUdmaIntErrorRegsStatusCh1,
        [2] = GetUdmaIntErrorRegsStatusCh2,
        [3] = GetUdmaIntErrorRegsStatusCh3,
    };
    GetUdmaIntErrorRegsFuncArray[channel](udmaReg, udmaAddr, interruptFlag);
    return;
}

UINT32
PCIE_UDMA_GetErrorInterrupt(
    const PCIE_PrivateData * pD,
    UINT32                 channel,
    PCIE_Bool *              pIntActive)
{
    struct PCIE_IClientUdma_s *pcieAddr;
    UINT32 udmaReg;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) ||
         (pIntActive == NULL) ||
         (channel >= (UINT32)PCIE_NUM_UDMA_CHANNELS) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ud_base;

        udmaReg = PCIE_RegPtrRead32(&pcieAddr->dma_common.common_udma_int);

        *pIntActive = PCIE_FALSE;

        GetUdmaInterruptErrorRegsStatus(channel, udmaReg, pcieAddr, pIntActive);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
