/**
 *  @file AmbaPCIE_Drv_LmInterrupt.c
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
 *  @details PCIe implementation of driver API functions (local management for interrupt).
 */
#include "AmbaPCIE_Drv.h"

#define PCIE_INVALID_PARAMETER 0xFFFFFFFFU
#define PCIE_LOCAL_ERR_ELEMENTS 26U
#define PCIE_INIT_MASK_CONDIT_ELEMENTS 12U

/****************************************************************************/
/* PCIE_Start                                                               */
/****************************************************************************/
UINT32
PCIE_Start(const PCIE_PrivateData *              pD,
           PCIE_MaskableLocalInterruptConditions maskableIntrParam)
{
    return (PCIE_CtrlMaskingOfLocInterrupt(pD, maskableIntrParam, PCIE_UNMASK_PARAM));
}

/****************************************************************************/
/* PCIE_Stop                                                                */
/****************************************************************************/
UINT32
PCIE_Stop(const PCIE_PrivateData *              pD,
          PCIE_MaskableLocalInterruptConditions maskableIntrParam)
{
    return (PCIE_CtrlMaskingOfLocInterrupt(pD, maskableIntrParam, PCIE_MASK_PARAM));
}

/****************************************************************************/
/* PCIE_Isr                                                                 */
/****************************************************************************/
UINT32
PCIE_Isr(const PCIE_PrivateData * pD)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check for errors reported from PCIe IP */
    if (CallErrorCallbackIfError(pD) != 0U) {
        result = PCIE_ERR_ARG;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to call the error callback, if errors are reported by the IP
 */
UINT32 CallErrorCallbackIfError(const PCIE_PrivateData * pD)
{
    PCIE_Bool le;
    UINT32 et;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (PCIE_AreThereLocalErrors(pD, &le, &et) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        if ((le == PCIE_TRUE) && (pD != NULL)) { /* Call the registered callback */
            if (pD->p_ec_func != NULL) {
                pD->p_ec_func(et);
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/*
 *       PCIE_Are_There_Local_Errors
 *
 * API function to check if there are any local errors
 */
UINT32 PCIE_AreThereLocalErrors(
    const PCIE_PrivateData * pD,
    PCIE_Bool *              pLocalErrorsPresent,
    UINT32 *               pErrorType)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pLocalErrorsPresent == NULL) || (pErrorType == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        *pErrorType = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_local_error_status_register));

        if (*pErrorType != 0U) {
            *pLocalErrorsPresent = PCIE_TRUE;
        } else {
            *pLocalErrorsPresent = PCIE_FALSE;
        }
    }
    return (result); /* 0 for succcess (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
/* EVENT MASKING ROUTINES                                                   */
/****************************************************************************/
/****************************************************************************/


/****************************************************************************
 * PCIE_Control_Masking_Of_Local_Interrupts
 *****************************************************************************/
static UINT32 ControlMaskingOfLocalIntsTool(
    PCIE_MaskOrUnmask maskOrUnmask,
    UINT32          mask,
    UINT32          value,
    UINT32 *        status)
{
    UINT32 temp;

    temp = PCIE_ERR_ARG;

    if (maskOrUnmask == PCIE_MASK_PARAM) {
        temp = value | mask;
    } else if (maskOrUnmask == PCIE_UNMASK_PARAM) {
        temp = value & ~mask;
    } else {
        /* Indicate incorrect input parameters */
        *status = PCIE_ERR_ARG;
    }

    return (temp);
}

static const UINT32 IntParameterArray[PCIE_LOCAL_ERR_ELEMENTS] = {
    [(UINT32)PCIE_PNP_RX_FIFO_PARITY_ERROR] = 1U, //<< LM_LOCAL_INT_MASK_PRFPE_SHIFT,
    [1U] = PCIE_INVALID_PARAMETER,
    [(UINT32)PCIE_PNP_RX_FIFO_OVERFLOW_CONDITION]  = (UINT32) 1U << LM_LOCAL_INT_MASK_PRFO_SHIFT,
            [3U] = PCIE_INVALID_PARAMETER,

            [(UINT32)PCIE_COMPLETION_RX_FIFO_PARITY_ERROR] = (UINT32) 1U << LM_LOCAL_INT_MASK_CRFPE_SHIFT,
            [(UINT32)PCIE_REPLAY_RAM_PARITY_ERROR]         = (UINT32) 1U << LM_LOCAL_INT_MASK_RRPE_SHIFT,
            [(UINT32)PCIE_COMPLETION_RX_FIFO_OVERFLOW_CONDITION] = (UINT32) 1U << LM_LOCAL_INT_MASK_CRFO_SHIFT,
            [(UINT32)PCIE_REPLAY_TIMEOUT_CONDITION]              = (UINT32) 1U << LM_LOCAL_INT_MASK_RT_SHIFT,
            [(UINT32)PCIE_REPLAY_TIMEOUT_ROLLOVER_CONDITION]     = (UINT32) 1U << LM_LOCAL_INT_MASK_RTR_SHIFT,
            [(UINT32)PCIE_PHY_ERROR]                             = (UINT32) 1U << LM_LOCAL_INT_MASK_PE_SHIFT,
            [(UINT32)PCIE_MALFORMED_TLP_RECEIVED]         = (UINT32) 1U << LM_LOCAL_INT_MASK_MTR_SHIFT,
            [(UINT32)PCIE_UNEXPECTED_COMPLETION_RECEIVED] = (UINT32) 1U << LM_LOCAL_INT_MASK_UCR_SHIFT,
            [(UINT32)PCIE_FLOW_CONTROL_ERROR]             = (UINT32) 1U << LM_LOCAL_INT_MASK_FCE_SHIFT,
            [(UINT32)PCIE_COMPLETION_TIMEOUT_CONDITION]   = (UINT32) 1U << LM_LOCAL_INT_MASK_CT_SHIFT,
            [14U] = PCIE_INVALID_PARAMETER,

            [15U] = PCIE_INVALID_PARAMETER,

            [(UINT32)PCIE_DMA_OB_ERROR] = (UINT32) 1U << LM_LOCAL_INT_MASK_OB_BUFF_ER_UN_SHIFT,
            [(UINT32)PCIE_DMA_IB_ERROR] = (UINT32) 1U << LM_LOCAL_INT_MASK_IB_BUFF_ER_UN_SHIFT,

            [(UINT32)PCIE_MSI_CHG_CONDITION] = (UINT32) 1U << LM_LOCAL_INT_MASK_MMVC_SHIFT,

            [(UINT32)PCIE_MSIX_CHG_CONDITION] = (UINT32) 1U << LM_LOCAL_INT_MASK_MSIXMSK_SHIFT,

            [(UINT32)PCIE_HAWCD_CONDITION] = (UINT32) 1U << LM_LOCAL_INT_MASK_HAWCD_SHIFT,

            [(UINT32)PCIE_AXIM_ECC_READ_ERROR ] = (UINT32) 1U << LM_LOCAL_INT_MASK_AXIMASTER_RFIFO_ER_UN_SHIFT,
            [(UINT32)PCIE_AXIS_ECC_WRITE_ERROR ] = (UINT32) 1U << LM_LOCAL_INT_MASK_AXISLAVE_WFIFO_ER_UN_SHIFT,
            [(UINT32)PCIE_REORDER_ERROR ] = (UINT32) 1U << LM_LOCAL_INT_MASK_REORDER_ER_UN_SHIFT,

            [24U] = PCIE_INVALID_PARAMETER,

            [25U] = PCIE_INVALID_PARAMETER,

};

UINT32 PCIE_CtrlMaskingOfLocInterrupt(
    const PCIE_PrivateData *              pD,
    PCIE_MaskableLocalInterruptConditions maskableIntrParam,
    PCIE_MaskOrUnmask                     maskOrUnmask)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 status;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        UINT32 interruptMask = PCIE_INVALID_PARAMETER;

        if ((UINT32) maskableIntrParam < PCIE_LOCAL_ERR_ELEMENTS) {
            interruptMask = IntParameterArray[(UINT32)maskableIntrParam];
        }
        /* Check that mask is valid */
        if ( interruptMask == PCIE_INVALID_PARAMETER) {
            result = PCIE_ERR_ARG;
        } else {
            status = PCIE_ERR_SUCCESS;
            /* Read register from PCIe IP */
            pcieAddr = pD->p_lm_base;
            regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_local_intrpt_mask_reg));
            regVal = ControlMaskingOfLocalIntsTool(maskOrUnmask,
                                                   interruptMask,
                                                   regVal,
                                                   &status);

            if (status != 0U) {
                result = status;
            } else {
                PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_local_intrpt_mask_reg), regVal); /* Write back to register */
                /* Check for errors reported from PCIe IP */
                result = CallErrorCallbackIfError(pD);
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Get_Local_Error_Status_Values                                       */
/****************************************************************************/
UINT32  PCIE_IsLocalError(
    const PCIE_PrivateData *              pD,
    PCIE_MaskableLocalInterruptConditions errorCondition,
    PCIE_Bool *                           pLocalError)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) || (pLocalError == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        UINT32 interruptMask = PCIE_INVALID_PARAMETER;

        if ((UINT32) errorCondition < PCIE_LOCAL_ERR_ELEMENTS) {
            interruptMask = IntParameterArray[(UINT32)errorCondition];
        }
        if (interruptMask == PCIE_INVALID_PARAMETER) {
            result = PCIE_ERR_ARG;
        } else {
            /* Read register from PCIe IP */
            pcieAddr = pD->p_lm_base;
            regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_local_error_status_register));

            if ((regVal & interruptMask) != 0U) {
                *pLocalError = PCIE_TRUE;
            } else {
                *pLocalError = PCIE_FALSE;
            }
        }
    }
    /* We do not call CallErrorCallbackifError() here, to allow this function to be called */
    /* from the error callback if required */
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Reset_Local_Error_Status_Condition                                  */
/****************************************************************************/
UINT32 PCIE_ResetLocalErrStatusCondit(
    const PCIE_PrivateData *              pD,
    PCIE_MaskableLocalInterruptConditions maskableIntrParam)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        UINT32 interruptMask = PCIE_INVALID_PARAMETER;

        if ((UINT32) maskableIntrParam < PCIE_LOCAL_ERR_ELEMENTS) {
            interruptMask = IntParameterArray[(UINT32)maskableIntrParam];
        }
        if (interruptMask == PCIE_INVALID_PARAMETER) {
            result = PCIE_ERR_ARG;
        } else {
            /* Read register from PCIe IP */
            pcieAddr = pD->p_lm_base;
            regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_local_error_status_register));

            /* Mask all WOCLR bits.*/
            regVal &= ~LM_LOCAL_ERROR_STATUS_WOCLR_MASK;

            /* Status is WOCLR, */
            regVal |= interruptMask;
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_local_error_status_register), regVal);
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to copy values to an array.
 */
static void FillInitParamMaskChangeDefArray(
    const PCIE_InitMaskableConditions initMaskableConditions,
    UINT32                          InitParamMasksChangeDef[PCIE_INIT_MASK_CONDIT_ELEMENTS])
{
    /*
     * Fill array with maskable condition parameter - changeDefault
     * If changeDefault is non-zero, the corresponding mask will be initialised
     * using the corresponding value in InitParamMasksNewVal
     * This is required so that we can map enum values to unsigned integer values in a
     * MISRA compliant way
     */
    InitParamMasksChangeDef[0]  = initMaskableConditions.maskPnpRxFifoParityError.changeDefault;
    InitParamMasksChangeDef[3]  = initMaskableConditions.maskPnpRxFifoOverflow.changeDefault;

    InitParamMasksChangeDef[1]  = initMaskableConditions.maskCompletionRxFifoParityError.changeDefault;
    InitParamMasksChangeDef[2]  = initMaskableConditions.maskReplayRamParityError.changeDefault;
    InitParamMasksChangeDef[4]  = initMaskableConditions.maskCompletionRxFifoOverflow.changeDefault;
    InitParamMasksChangeDef[5]  = initMaskableConditions.maskReplayTimeout.changeDefault;
    InitParamMasksChangeDef[6]  = initMaskableConditions.maskReplayTimerRollover.changeDefault;
    InitParamMasksChangeDef[7]  = initMaskableConditions.maskPhyError.changeDefault;
    InitParamMasksChangeDef[8]  = initMaskableConditions.maskMalformedTlpReceived.changeDefault;
    InitParamMasksChangeDef[9]  = initMaskableConditions.maskUnexpectedComplReceived.changeDefault;
    InitParamMasksChangeDef[10] = initMaskableConditions.maskFlowControlError.changeDefault;
    InitParamMasksChangeDef[11] = initMaskableConditions.maskCompletionTimeout.changeDefault;

    /* Returning after array duly filled */
    return;
}
/* Local function to copy values to an array.
 */
static void FillInitParamMaskNewValArray(
    const PCIE_InitMaskableConditions initMaskableConditions,
    UINT32                          InitParamMasksNewVal[PCIE_INIT_MASK_CONDIT_ELEMENTS])
{
    /*
     * Fill array with maskable condition parameter - changeDefault
     * This is required so that we can map enum values to unsigned integer values in a
     * MISRA compliant way
     */
    InitParamMasksNewVal[0]  = initMaskableConditions.maskPnpRxFifoParityError.newValue;
    InitParamMasksNewVal[3]  = initMaskableConditions.maskPnpRxFifoOverflow.newValue;

    InitParamMasksNewVal[1]  = initMaskableConditions.maskCompletionRxFifoParityError.newValue;
    InitParamMasksNewVal[2]  = initMaskableConditions.maskReplayRamParityError.newValue;
    InitParamMasksNewVal[4]  = initMaskableConditions.maskCompletionRxFifoOverflow.newValue;
    InitParamMasksNewVal[5]  = initMaskableConditions.maskReplayTimeout.newValue;
    InitParamMasksNewVal[6]  = initMaskableConditions.maskReplayTimerRollover.newValue;
    InitParamMasksNewVal[7]  = initMaskableConditions.maskPhyError.newValue;
    InitParamMasksNewVal[8]  = initMaskableConditions.maskMalformedTlpReceived.newValue;
    InitParamMasksNewVal[9]  = initMaskableConditions.maskUnexpectedComplReceived.newValue;
    InitParamMasksNewVal[10] = initMaskableConditions.maskFlowControlError.newValue;
    InitParamMasksNewVal[11] = initMaskableConditions.maskCompletionTimeout.newValue;
    /* Returning after array duly filled */
    return;
}
/* Static function to initialize maskable parameters
 */
UINT32 Init_MaskableParams(const PCIE_PrivateData * pD,
                           const PCIE_InitParam *   pInitParam)
{
    UINT32 retVal = 0U;

    UINT32 InitParamMasksChangeDef[PCIE_INIT_MASK_CONDIT_ELEMENTS];
    UINT32 InitParamMasksNewVal[PCIE_INIT_MASK_CONDIT_ELEMENTS];
    /* static const should not require memory allocation */
    static const PCIE_MaskableLocalInterruptConditions MaskableLocalInterruptCondArray[PCIE_INIT_MASK_CONDIT_ELEMENTS] = {
        [0] = PCIE_PNP_RX_FIFO_PARITY_ERROR,
        [3] = PCIE_PNP_RX_FIFO_OVERFLOW_CONDITION,

        [1] = PCIE_COMPLETION_RX_FIFO_PARITY_ERROR,
        [2] = PCIE_REPLAY_RAM_PARITY_ERROR,
        [4] = PCIE_COMPLETION_RX_FIFO_OVERFLOW_CONDITION,
        [5] = PCIE_REPLAY_TIMEOUT_CONDITION,
        [6] = PCIE_REPLAY_TIMEOUT_ROLLOVER_CONDITION,
        [7] = PCIE_PHY_ERROR,
        [8] = PCIE_MALFORMED_TLP_RECEIVED,
        [9] = PCIE_UNEXPECTED_COMPLETION_RECEIVED,
        [10] = PCIE_FLOW_CONTROL_ERROR,
        [11] = PCIE_COMPLETION_TIMEOUT_CONDITION
    };
    UINT32 idx = 0U;
    FillInitParamMaskChangeDefArray(pInitParam->initMaskableConditions, InitParamMasksChangeDef);
    FillInitParamMaskNewValArray(pInitParam->initMaskableConditions, InitParamMasksNewVal);

    while (idx < PCIE_INIT_MASK_CONDIT_ELEMENTS) {
        if (InitParamMasksChangeDef[idx] != 0U) {
            retVal = PCIE_CtrlMaskingOfLocInterrupt(
                         pD,
                         MaskableLocalInterruptCondArray[idx],
                         (InitParamMasksNewVal[idx] != 0U) ? PCIE_MASK_PARAM : PCIE_UNMASK_PARAM);
        }
        if (retVal != 0U) {
            break;
        }
        idx = idx + 1U;
    }

    return (retVal);
}
