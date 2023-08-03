/**
 *  @file AmbaPCIE_Drv_LmCount.c
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
 *  @details PCIe implementation of driver API functions (local management for count).
 */
#include "AmbaPCIE_Drv.h"

/****************************************************************************/
/****************************************************************************/
/* COUNT STATISTICS ROUTINES                                                */
/****************************************************************************/
/****************************************************************************/
/* Local function to read saved Tx or Rx count values
 * These values cannot be written, but can be cleared
 */
static UINT32 AccessSavedRxTxCountValues(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_Bool                resetAfterRd,
    UINT32 *               pCountVal)
{
    struct PCIE_IClientLm_s *pcieAddr = pD->p_lm_base;
    UINT32 regVal = 0;
    UINT32 result = PCIE_ERR_SUCCESS;
    volatile UINT32* count_reg = NULL;

    if (countParam == PCIE_TRANSMIT_TLP_COUNT) {
        /* Read register from PCIe IP */
        count_reg = &(pcieAddr->i_regf_lm_pcie_base.i_transmit_tlp_count_reg);
        regVal = PCIE_RegPtrRead32(count_reg);
        *pCountVal = PCIE_RegFldRead(LM_TX_TLP_COUNT_TTC_MASK, LM_TX_TLP_COUNT_TTC_SHIFT, regVal);
        regVal = LM_TX_TLP_COUNT_TTC_MASK;
    } else if (countParam == PCIE_TRANSMIT_TLP_PAYLOAD_DWORD_COUNT)   {
        /* Read register from PCIe IP */
        count_reg = &(pcieAddr->i_regf_lm_pcie_base.i_transmit_tlp_payload_dword_count_reg);
        regVal = PCIE_RegPtrRead32(count_reg);
        *pCountVal = PCIE_RegFldRead(LM_TX_TLP_PLD_DW_COUNT_TTPBC_MASK, LM_TX_TLP_PLD_DW_COUNT_TTPBC_SHIFT, regVal);
        regVal = LM_TX_TLP_PLD_DW_COUNT_TTPBC_MASK;
    } else if (countParam == PCIE_RECEIVE_TLP_COUNT) {
        /* Read register from PCIe IP */
        count_reg = &(pcieAddr->i_regf_lm_pcie_base.i_receive_tlp_count_reg);
        regVal = PCIE_RegPtrRead32(count_reg);
        *pCountVal = PCIE_RegFldRead(LM_RX_TLP_COUNT_RTC_MASK, LM_RX_TLP_COUNT_RTC_SHIFT, regVal);
        regVal = LM_RX_TLP_COUNT_RTC_MASK;
    } else if (countParam == PCIE_RECEIVE_TLP_PAYLOAD_DWORD_COUNT)   {
        /* Read register from PCIe IP */
        count_reg = &(pcieAddr->i_regf_lm_pcie_base.i_receive_tlp_payload_dword_count_reg);
        regVal = PCIE_RegPtrRead32(count_reg);
        *pCountVal = PCIE_RegFldRead(LM_RX_TLP_PLD_DW_COUNT_RTPDC_MASK, LM_RX_TLP_PLD_DW_COUNT_RTPDC_SHIFT, regVal);
        regVal = LM_RX_TLP_PLD_DW_COUNT_RTPDC_MASK;
    } else {
        result = PCIE_ERR_ARG;
    }
    if ((resetAfterRd == PCIE_TRUE) && (result == PCIE_ERR_SUCCESS)) {
        /*
         * Set the count back to zero
         */
        PCIE_RegPtrWrite32(count_reg, regVal); /* Write back to register */
    }
    return (result);  /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to read or write crc error count values
 */
static UINT32 AccessTlpLcrcErrorCount(
    const PCIE_PrivateData * pD,
    PCIE_Bool                resetAfterRd,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pCountVal)
{
    struct PCIE_IClientLm_s *pcieAddr = pD->p_lm_base;
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal;
    if (rdOrWr != PCIE_DO_READ) {
        result = PCIE_ERR_ARG;
    } else {
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_lcrc_err_count_reg));
        *pCountVal = PCIE_RegFldRead(LM_LCRC_ERR_COUNT_LEC_MASK, LM_LCRC_ERR_COUNT_LEC_SHIFT, regVal);
        if (resetAfterRd == PCIE_TRUE) {
            /*
             * Set the count back to zero
             * Mask all WOCLR bits in regval
             */
            regVal &= ~LM_LCRC_ERR_COUNT_WOCLR_MASK;
            regVal |= LM_LCRC_ERR_COUNT_LEC_MASK;
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_lcrc_err_count_reg), regVal); /* Write back to register */
        }
    }
    return result;
}
/* Local function to read or write saved error count values */
static UINT32 AccessSavedErrorCountValues(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_Bool                resetAfterRd,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pCountVal)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    volatile UINT32* err_count_reg = &(pD->p_lm_base->i_regf_lm_pcie_base.i_ecc_corr_err_count_reg);
    UINT32 regVal = 0U;

    if (rdOrWr != PCIE_DO_READ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Switch though each of the saved count parameters */
        switch (countParam) {
        case PCIE_ECC_CORR_ERR_COUNT_PNP_FIFO_RAM:
            regVal = PCIE_RegPtrRead32(err_count_reg);
            *pCountVal = PCIE_RegFldRead(LM_ECC_CORR_ERR_COUNT_PFRCER_MASK, LM_ECC_CORR_ERR_COUNT_PFRCER_SHIFT, regVal);
            regVal |= LM_ECC_CORR_ERR_COUNT_PFRCER_MASK;
            break;
        case PCIE_ECC_CORR_ERR_COUNT_SC_FIFO_RAM:
            regVal = PCIE_RegPtrRead32(err_count_reg);
            *pCountVal = PCIE_RegFldRead(LM_ECC_CORR_ERR_COUNT_SFRCER_MASK, LM_ECC_CORR_ERR_COUNT_SFRCER_SHIFT, regVal);
            regVal |= LM_ECC_CORR_ERR_COUNT_SFRCER_MASK;
            break;
        case PCIE_ECC_CORR_ERR_COUNT_REPLAY_RAM:
            regVal = PCIE_RegPtrRead32(err_count_reg);
            *pCountVal = PCIE_RegFldRead(LM_ECC_CORR_ERR_COUNT_RRCER_MASK, LM_ECC_CORR_ERR_COUNT_RRCER_SHIFT, regVal);
            regVal |= LM_ECC_CORR_ERR_COUNT_RRCER_MASK;
            break;
        case PCIE_RECEIVE_TLP_COUNT:
        case PCIE_RECEIVE_TLP_PAYLOAD_DWORD_COUNT:
        case PCIE_TRANSMIT_TLP_COUNT:
        case PCIE_TRANSMIT_TLP_PAYLOAD_DWORD_COUNT:
        case PCIE_TLP_LCRC_ERROR_COUNT:
        case PCIE_RECEIVED_FTS_COUNT_FOR_2_5_GT_SPEED:
        case PCIE_RECEIVED_FTS_COUNT_FOR_5_GT_SPEED:
        case PCIE_RECEIVED_FTS_COUNT_FOR_8_GT_SPEED:
        case PCIE_TRN_FTS_COUNT_FOR_2_5_GT_SPEED:
        case PCIE_TRN_FTS_COUNT_FOR_5_GT_SPEED:
        case PCIE_TRN_FTS_COUNT_FOR_8_GT_SPEED:
        default:
            /* Invalid Saved Count Parameter */
            result = PCIE_ERR_ARG;
            break;
        }
        if ((resetAfterRd == PCIE_TRUE) && (result == PCIE_ERR_SUCCESS)) {
            /*
             * Set the count back to zero
             * Mask all WOCLR bits in regval
             */
            regVal &= ~LM_ECC_CORR_ERR_COUNT_WOCLR_MASK;
            PCIE_RegPtrWrite32(err_count_reg, regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to read saved receive count values
 * These values cannot be written
 */
static UINT32 AccessSavedReceivedCountValues(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pCountVal
)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    pcieAddr = pD->p_lm_base;
    UINT32 regVal;
    const volatile UINT32 *fts_count_reg = &(pcieAddr->i_regf_lm_pcie_base.i_receive_fts_count_reg);

    if (countParam == PCIE_RECEIVED_FTS_COUNT_FOR_2_5_GT_SPEED) {
        if (rdOrWr != PCIE_DO_READ) {
            /* These values cannot be written */
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));
            *pCountVal = PCIE_RegFldRead(LM_PL_CONFIG0_RFC_MASK, LM_PL_CONFIG0_RFC_SHIFT, regVal);
        }
    } else if (countParam == PCIE_RECEIVED_FTS_COUNT_FOR_5_GT_SPEED) {
        if (rdOrWr != PCIE_DO_READ) {
            /* These values cannot be written */
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegPtrRead32(fts_count_reg);
            *pCountVal = PCIE_RegFldRead(LM_RX_FTS_COUNT_RFC5S_MASK, LM_RX_FTS_COUNT_RFC5S_SHIFT, regVal);
        }
    } else if (countParam == PCIE_RECEIVED_FTS_COUNT_FOR_8_GT_SPEED) {
        if (rdOrWr != PCIE_DO_READ) {
            /* These values cannot be written */
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegPtrRead32(fts_count_reg);
            *pCountVal = PCIE_RegFldRead(LM_RX_FTS_COUNT_RFC8S_MASK, LM_RX_FTS_COUNT_RFC8S_SHIFT, regVal);
        }
    }

    else {
        result = PCIE_ERR_ARG;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallTransmittedFtsCnt_2_5_GT(
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pCountVal,
    struct PCIE_IClientLm_s * pcieAddr)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal;

    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pCountVal = PCIE_RegFldRead(LM_PL_CONFIG1_TFC1_MASK, LM_PL_CONFIG1_TFC1_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE:
         * Check if the value to be written is in valid range
         */
        if ( ((*pCountVal) >> LM_PL_CONFIG1_TFC1_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_PL_CONFIG1_TFC1_MASK, LM_PL_CONFIG1_TFC1_SHIFT, regVal, *pCountVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallTransmittedFtsCnt_5_GT(
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pCountVal,
    struct PCIE_IClientLm_s * pcieAddr)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal;

    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pCountVal = PCIE_RegFldRead(LM_PL_CONFIG1_TFC2_MASK, LM_PL_CONFIG1_TFC2_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE:
         * Check if the value to be written is in valid range
         */
        if ( ((*pCountVal) >> LM_PL_CONFIG1_TFC2_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_PL_CONFIG1_TFC2_MASK, LM_PL_CONFIG1_TFC2_SHIFT, regVal, *pCountVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallTransmittedFtsCnt_8_GT(
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pCountVal,
    struct PCIE_IClientLm_s * pcieAddr)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal;

    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pCountVal = PCIE_RegFldRead(LM_PL_CONFIG1_TFC3_MASK, LM_PL_CONFIG1_TFC3_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE:
         * Check if the value to be written is in valid range
         */
        if ( ((*pCountVal) >> LM_PL_CONFIG1_TFC3_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_PL_CONFIG1_TFC3_MASK, LM_PL_CONFIG1_TFC3_SHIFT, regVal, *pCountVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to read or write saved transmit count values
 */
static UINT32 AccessSavedTransmittedCountVal(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pCountVal)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    struct PCIE_IClientLm_s *pcieAddr;

    pcieAddr = pD->p_lm_base;

    /* Check and call the respective function based on the count parameter*/
    if (countParam == PCIE_TRN_FTS_COUNT_FOR_2_5_GT_SPEED) {
        result = CallTransmittedFtsCnt_2_5_GT(rdOrWr, pCountVal, pcieAddr);
    }
    if (countParam == PCIE_TRN_FTS_COUNT_FOR_5_GT_SPEED) {
        result = CallTransmittedFtsCnt_5_GT(rdOrWr, pCountVal, pcieAddr);
    }
    if (countParam == PCIE_TRN_FTS_COUNT_FOR_8_GT_SPEED) {
        result = CallTransmittedFtsCnt_8_GT(rdOrWr, pCountVal, pcieAddr);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to validate input parameters
 */
static UINT32 ValidateInputAccessSavedCntVal(
    const PCIE_PrivateData * pD,
    const PCIE_Bool          resetAfterRd,
    const UINT32 *         pCountVal)
{
    UINT32 result;
    if ( (pD == NULL) ||
         (pCountVal == NULL) ||
         ((resetAfterRd != PCIE_FALSE) && (resetAfterRd != PCIE_TRUE)) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read or write saved receive/transmit count values
 */
static UINT32 AccessSavedCountValues(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_Bool                resetAfterRd,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pCountVal)
{
    UINT32 regVal = 0U;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if (ValidateInputAccessSavedCntVal(pD, resetAfterRd, pCountVal) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        switch (countParam) {
        case PCIE_RECEIVE_TLP_COUNT:
        case PCIE_RECEIVE_TLP_PAYLOAD_DWORD_COUNT:
        case PCIE_TRANSMIT_TLP_COUNT:
        case PCIE_TRANSMIT_TLP_PAYLOAD_DWORD_COUNT:

            if (rdOrWr != PCIE_DO_READ) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = AccessSavedRxTxCountValues(pD, countParam, resetAfterRd, pCountVal);
            }
            break;

        case PCIE_TLP_LCRC_ERROR_COUNT:
            regVal = AccessTlpLcrcErrorCount(pD, resetAfterRd, rdOrWr, pCountVal);
            break;

        case PCIE_ECC_CORR_ERR_COUNT_PNP_FIFO_RAM:
        case PCIE_ECC_CORR_ERR_COUNT_SC_FIFO_RAM:
        case PCIE_ECC_CORR_ERR_COUNT_REPLAY_RAM:

            regVal = AccessSavedErrorCountValues(pD, countParam, resetAfterRd, rdOrWr, pCountVal);
            break;

        case PCIE_RECEIVED_FTS_COUNT_FOR_2_5_GT_SPEED:
        case PCIE_RECEIVED_FTS_COUNT_FOR_5_GT_SPEED:
        case PCIE_RECEIVED_FTS_COUNT_FOR_8_GT_SPEED:

            regVal = AccessSavedReceivedCountValues(pD, countParam, rdOrWr, pCountVal);
            break;

        case PCIE_TRN_FTS_COUNT_FOR_2_5_GT_SPEED:
        case PCIE_TRN_FTS_COUNT_FOR_5_GT_SPEED:
        case PCIE_TRN_FTS_COUNT_FOR_8_GT_SPEED:

            regVal = AccessSavedTransmittedCountVal(pD, countParam, rdOrWr, pCountVal);
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            /* Confirm if values were read correctly*/
            if (regVal > 0U) {
                result = regVal;
            } else {
                /* Check for errors reported from PCIe IP */
                result = CallErrorCallbackIfError(pD);
            }
        }
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* API function to set saved count values
 */
UINT32 PCIE_SetSavedCountValues(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_Bool                resetAfterRd,
    UINT32           *pCountVal)
{
    return (AccessSavedCountValues(pD, countParam, resetAfterRd, PCIE_DO_WRITE, pCountVal));
}
/* API function to get saved count values
 */
UINT32 PCIE_GetSavedCountValues(
    const PCIE_PrivateData * pD,
    PCIE_SavedCountParams    countParam,
    PCIE_Bool                resetAfterRd,
    UINT32 *               pCountVal)
{
    return(AccessSavedCountValues(pD, countParam, resetAfterRd, PCIE_DO_READ,  pCountVal));
}

/* Local function to reduce complexity
 */
static UINT32 InitTransmittedFtsCount2_5Gbs(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam)
{
    UINT32 val = pInitParam->initCountParams.transmittedFtsCount2_5Gbs.newValue;
    return PCIE_SetSavedCountValues(
               pD,
               PCIE_TRN_FTS_COUNT_FOR_2_5_GT_SPEED,
               PCIE_FALSE,
               &val);
}
/* Local function to reduce complexity
 */
static UINT32 InitTransmittedFtsCount5Gbs(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam)
{
    UINT32 val = pInitParam->initCountParams.transmittedFtsCount5Gbs.newValue;
    return PCIE_SetSavedCountValues(
               pD,
               PCIE_TRN_FTS_COUNT_FOR_5_GT_SPEED,
               PCIE_FALSE,
               &val);
}
/* Local function to reduce complexity
 */
static UINT32 InitTransmittedFtsCount8Gbs(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam)
{
    UINT32 val = pInitParam->initCountParams.transmittedFtsCount8Gbs.newValue;
    return PCIE_SetSavedCountValues(
               pD,
               PCIE_TRN_FTS_COUNT_FOR_8_GT_SPEED,
               PCIE_FALSE,
               &val);
}

/* Local function to initialize transmitted count values
 */
UINT32 Init_Transmitted(const PCIE_PrivateData * pD,
                        const PCIE_InitParam *   pInitParam)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pInitParam->initCountParams.transmittedFtsCount2_5Gbs.changeDefault != 0U) {
        result |= InitTransmittedFtsCount2_5Gbs(pD, pInitParam);
    }

    if (pInitParam->initCountParams.transmittedFtsCount5Gbs.changeDefault != 0U) {
        result |= InitTransmittedFtsCount5Gbs(pD, pInitParam);
    }

    if (pInitParam->initCountParams.transmittedFtsCount8Gbs.changeDefault != 0U) {
        result |= InitTransmittedFtsCount8Gbs(pD, pInitParam);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
