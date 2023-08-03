/**
 *  @file AmbaPCIE_Drv_LmDbgMux.c
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
 *  @details PCIe implementation of driver API functions (local management for debug mux).
 */
#include "AmbaPCIE_Drv.h"

#define PCIE_DEBUG_PARAMS_ELEMENTS 14U
#define PCIE_INVALID_PARAMETER 0xFFFFFFFFU
#define PCIE_DEBUG_MUX_MODULE_ELEMENTS 11U
#define PCIE_INIT_DEBUG_PARAMETERS 10U
#define PCIE_CTRL_DEBUG_FUNC_ELEMENTS 3U
#define PCIE_CTRL_DEBUG_PARAMS_ELEMENTS 3U

/****************************************************************************/
/****************************************************************************/
/* DEBUG CONTROL                                                            */
/****************************************************************************/
/****************************************************************************/
/****************************************************************************
 * Access_Debug_Mux
 *****************************************************************************/
/* static const should not require memory allocation */
static const PCIE_DebugMuxModuleSelect DebugMuxModuleSelectArray[PCIE_DEBUG_MUX_MODULE_ELEMENTS] = {
    [0U]       = PCIE_DEBUG_PHY_LAYER_LTSSM_0,
    [1U]       = PCIE_DEBUG_PHY_LAYER_LTSSM_1,
    [4U] = PCIE_DEBUG_DATA_LINK_LAYER_TX_SIDE,
    [5U] = PCIE_DEBUG_DATA_LINK_LAYER_RX_SIDE,
    [8U]      = PCIE_DEBUG_TCTN_LAYER_TX_SIDE,
    [9U]    = PCIE_DEBUG_TCTN_LAYER_RX_SIDE_0,
    [10U]   = PCIE_DEBUG_TCTN_LAYER_RX_SIDE_1
};

UINT32 PCIE_GetDebugMux(
    const PCIE_PrivateData *    pD,
    PCIE_DebugMuxModuleSelect * muxSelect)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 DebugMuxModuleSelectArrayIndex = 0U;

    if ( (pD == NULL) || (muxSelect == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_debug_mux_control_reg));
        DebugMuxModuleSelectArrayIndex = PCIE_RegFldRead(LM_DBG_MUX_CTRL_MS_MASK, LM_DBG_MUX_CTRL_MS_SHIFT, regVal);
        if ( (DebugMuxModuleSelectArrayIndex == 2U ) ||
             (DebugMuxModuleSelectArrayIndex == 3U ) ||
             (DebugMuxModuleSelectArrayIndex == 6U ) ||
             (DebugMuxModuleSelectArrayIndex == 7U ) ||
             (DebugMuxModuleSelectArrayIndex > 10U ) ) {
            result = PCIE_ERR_ARG;
        } else {
            *muxSelect = DebugMuxModuleSelectArray[DebugMuxModuleSelectArrayIndex];
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_SetDebugMux(
    const PCIE_PrivateData *  pD,
    PCIE_DebugMuxModuleSelect muxSelect)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_debug_mux_control_reg));

        switch (muxSelect) {
        case PCIE_DEBUG_PHY_LAYER_LTSSM_0:
        case PCIE_DEBUG_PHY_LAYER_LTSSM_1:
        case PCIE_DEBUG_DATA_LINK_LAYER_TX_SIDE:
        case PCIE_DEBUG_DATA_LINK_LAYER_RX_SIDE:
        case PCIE_DEBUG_TCTN_LAYER_TX_SIDE:
        case PCIE_DEBUG_TCTN_LAYER_RX_SIDE_0:
        case PCIE_DEBUG_TCTN_LAYER_RX_SIDE_1:
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }
        if (result == PCIE_ERR_SUCCESS) {
            regVal = PCIE_RegFldWrite(LM_DBG_MUX_CTRL_MS_MASK, LM_DBG_MUX_CTRL_MS_SHIFT, regVal, (UINT32)muxSelect);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_debug_mux_control_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* PCIE_Control_Debug_Params                                                */
/****************************************************************************/
/* Local function to simplify handling of debug params
 */
static void ControlDebugParamsTool(
    PCIE_DebugParams debugParam,
    UINT32         clearOrSet,
    UINT32 *       value)
{
    UINT32 temp;
    UINT32 mask;
    UINT32 shift;

    temp = PCIE_ERR_ARG;
    mask = 0U;

    /* static const should not require memory allocation */
    static const UINT32 ShiftParameterArray[PCIE_DEBUG_PARAMS_ELEMENTS] = {
        [0U] = LM_DBG_MUX_CTRL_AWRPRI_SHIFT,
        [1U] = LM_DBG_MUX_CTRL_DSHEC_SHIFT,
        [2U] = LM_DBG_MUX_CTRL_DLRFE_SHIFT,
        [3U] = LM_DBG_MUX_CTRL_DLUC_SHIFT,
        [4U] = LM_DBG_MUX_CTRL_EFLT_SHIFT,
        [5U] = LM_DBG_MUX_CTRL_ESPC_SHIFT,

        [6U] = PCIE_INVALID_PARAMETER,

        [7U] = LM_DBG_MUX_CTRL_DGLUS_SHIFT,
        [8U] = LM_DBG_MUX_CTRL_DEI_SHIFT,
        [9U] = LM_DBG_MUX_CTRL_DFCUT_SHIFT,
        [10U] = LM_DBG_MUX_CTRL_DOC_SHIFT,
        [11U] = LM_DBG_MUX_CTRL_EFSRTCA_SHIFT,

        [12U] = PCIE_INVALID_PARAMETER,
        [13U] = PCIE_INVALID_PARAMETER,

    };

    shift = ShiftParameterArray[debugParam];

    /*
     * Sets or clears the appropriate bit in *value depending on the shift value.
     * If shift value is out of range for an UINT32, it will set *value to 0
     */
    if ( shift < 32U ) {
        mask = ~((UINT32)(1) << shift);
        if (clearOrSet != 0U) {
            temp = *value & mask;
        } else {
            temp = *value | ((UINT32)(1) << shift);
        }
    }
    *value = temp;
}
/* Local function to validate input parameters
 */
static UINT32 ValidateInputControlDebugParams (
    PCIE_CoreEpOrRp *          pEpOrRp,
    const PCIE_PrivateData *   pD,
    const PCIE_DebugParams     debugParam,
    const PCIE_EnableOrDisable enableOrDisable)
{
    UINT32 result;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, pEpOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if ( ( ( (*pEpOrRp == PCIE_CORE_RP_STRAPPED) ) &&
                  (debugParam ==  PCIE_SLOT_POWER_CAPTURE) ) ||
                ( ( (enableOrDisable != PCIE_DISABLE_PARAM) &&
                    (enableOrDisable != PCIE_ENABLE_PARAM) ) ) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallControlDebugParamsTool(
    PCIE_DebugParams     debugParam,
    PCIE_EnableOrDisable enableOrDisable,
    UINT32 *           pRegVal)
{
    UINT32 status = PCIE_ERR_SUCCESS;
    /* static const should not require memory allocation */
    static const UINT32 SetOrClearArray[PCIE_DEBUG_PARAMS_ELEMENTS] = {
        [0U] = (UINT32) PCIE_DISABLE_PARAM,
        [1U] = (UINT32)PCIE_ENABLE_PARAM,
        [2U] = (UINT32)PCIE_ENABLE_PARAM,
        [3U] = (UINT32)PCIE_ENABLE_PARAM,
        [4U] = (UINT32)PCIE_DISABLE_PARAM,
        [5U] = (UINT32) PCIE_DISABLE_PARAM,

        [6U] = PCIE_INVALID_PARAMETER,

        [7U] = (UINT32) PCIE_ENABLE_PARAM,
        [8U] = (UINT32)PCIE_ENABLE_PARAM,
        [9U] = (UINT32)PCIE_ENABLE_PARAM,
        [10U] = (UINT32)PCIE_ENABLE_PARAM,
        [11U] = (UINT32)PCIE_ENABLE_PARAM,

        [12U] = PCIE_INVALID_PARAMETER,

        [13U] = PCIE_INVALID_PARAMETER,

    };

    /* Check if the parameter is in valid range */
    if (((UINT32)debugParam >= PCIE_DEBUG_PARAMS_ELEMENTS) ||
        ((UINT32)SetOrClearArray[debugParam] == PCIE_INVALID_PARAMETER) ) {
        status = PCIE_ERR_ARG;
    } else if ( (UINT32) enableOrDisable == SetOrClearArray[debugParam]) {
        ControlDebugParamsTool( debugParam, 1U,pRegVal);
    } else {
        ControlDebugParamsTool( debugParam, 0U, pRegVal);
    }
    return (status);
}

UINT32 PCIE_ControlDebugParams(
    const PCIE_PrivateData * pD,
    PCIE_DebugParams         debugParam,
    PCIE_EnableOrDisable     enableOrDisable)
{
    struct PCIE_IClientLm_s *pcieAddr;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_RP_STRAPPED;
    UINT32 regVal;
    UINT32 status;
    UINT32 result = PCIE_ERR_SUCCESS;

    /*
     * Note - some debug features are enabled by setting a flag in the debug mux register
     * others are enabled by clearing the flag.  So sometimes the enableOrDisable param
     * is inverted.
     */
    /* Validate input parameters */
    if (ValidateInputControlDebugParams(&epOrRp, pD, debugParam, enableOrDisable) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_debug_mux_control_reg));

        status = CallControlDebugParamsTool(debugParam, enableOrDisable, &regVal);

        if (status != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_debug_mux_control_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillDbgParamsChangeDefArray(
    PCIE_InitDebugRelatedParams DebugParams,
    UINT32                    InitDebugParamsArray[PCIE_INIT_DEBUG_PARAMETERS])
{
    /*
     * Fill array with debug parameter - changeDefault
     */
    InitDebugParamsArray[0] = DebugParams.linkUpconfigureCapability.changeDefault;
    InitDebugParamsArray[1] = DebugParams.fastLinkTraining.changeDefault;
    InitDebugParamsArray[2] = DebugParams.electricalIdleInferInL0State.changeDefault;
    InitDebugParamsArray[3] = DebugParams.flowControlUpdateTimeout.changeDefault;
    InitDebugParamsArray[4] = DebugParams.orderingChecks.changeDefault;
    InitDebugParamsArray[5] = DebugParams.funcSpecificRprtType1CfgAccess.changeDefault;
    InitDebugParamsArray[6] = DebugParams.slotPowerCapture.changeDefault;
    InitDebugParamsArray[7] = DebugParams.syncHeaderFramingError.changeDefault;
    InitDebugParamsArray[8] = DebugParams.linkReTrainingFramingError.changeDefault;
    InitDebugParamsArray[9] = DebugParams.gen3LfsrUpdateFromSkp.changeDefault;
    /* Returning after array duly filled */
    return;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillDbgParamsNewValueArray(
    PCIE_InitDebugRelatedParams DebugParams,
    UINT32                    InitDebugParamsArray[PCIE_INIT_DEBUG_PARAMETERS])
{
    /*
     * Fill array with debug parameter - newValue
     */
    InitDebugParamsArray[0] = DebugParams.linkUpconfigureCapability.newValue;
    InitDebugParamsArray[1] = DebugParams.fastLinkTraining.newValue;
    InitDebugParamsArray[2] = DebugParams.electricalIdleInferInL0State.newValue;
    InitDebugParamsArray[3] = DebugParams.flowControlUpdateTimeout.newValue;
    InitDebugParamsArray[4] = DebugParams.orderingChecks.newValue;
    InitDebugParamsArray[5] = DebugParams.funcSpecificRprtType1CfgAccess.newValue;
    InitDebugParamsArray[6] = DebugParams.slotPowerCapture.newValue;
    InitDebugParamsArray[7] = DebugParams.syncHeaderFramingError.newValue;
    InitDebugParamsArray[8] = DebugParams.linkReTrainingFramingError.newValue;
    InitDebugParamsArray[9] = DebugParams.gen3LfsrUpdateFromSkp.newValue;
    /* Returning after array duly filled */
    return;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillCtrlDbgParamsChangeDefArray(
    PCIE_InitDebugRelatedParams DebugParams,
    UINT32                    CtrlDebugParamsChangeDefArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS])
{
    CtrlDebugParamsChangeDefArray[0U] = DebugParams.allPhyErrorReporting.changeDefault;

    CtrlDebugParamsChangeDefArray[1U] = DebugParams.txSwing.changeDefault;
    CtrlDebugParamsChangeDefArray[2U] = DebugParams.masterLoopback.changeDefault;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillCtrlDbgParamsNewValueArray(
    PCIE_InitDebugRelatedParams DebugParams,
    UINT32                    CtrlDebugParamsNewValueArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS])
{
    CtrlDebugParamsNewValueArray[0U] = DebugParams.allPhyErrorReporting.newValue;

    CtrlDebugParamsNewValueArray[1U] = DebugParams.txSwing.newValue;
    CtrlDebugParamsNewValueArray[2U] = DebugParams.masterLoopback.newValue;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillCtrlDebugArrays(
    const PCIE_InitParam * pInitParam,
    UINT32               CtrlDebugParamsChangeDefArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS],
    UINT32               CtrlDebugParamsNewValueArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS])
{
    FillCtrlDbgParamsChangeDefArray(pInitParam->initDebugParams, CtrlDebugParamsChangeDefArray);
    FillCtrlDbgParamsNewValueArray(pInitParam->initDebugParams, CtrlDebugParamsNewValueArray);
    return;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillDebugArrays(
    const PCIE_InitParam * pInitParam,
    UINT32               InitDebugParamsChangeDefArray[PCIE_INIT_DEBUG_PARAMETERS],
    UINT32               InitDebugParamsNewValueArray[PCIE_INIT_DEBUG_PARAMETERS])
{
    FillDbgParamsChangeDefArray(pInitParam->initDebugParams, InitDebugParamsChangeDefArray);
    FillDbgParamsNewValueArray(pInitParam->initDebugParams, InitDebugParamsNewValueArray);
    return;
}

/* Local function to Enable/Disable various Control parameters based on presence of user init values.
 */
static UINT32 EnableOrDisableElementsCtrl(
    const PCIE_PrivateData * pD,
    const UINT32  CtrlDebugParamsChangeDefArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS],
    const UINT32  CtrlDebugParamsNewValueArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS])
{
    UINT32 retVal = 0U;
    UINT32 idx;
    /* static const should not require memory allocation */
    static const ControlDebugParamsFunctions ControlDebugParamsFunction[PCIE_CTRL_DEBUG_FUNC_ELEMENTS] = {
        [0U] = PCIE_CtrlReportingOfAllPhyErr,
        [1U] = PCIE_ControlTxSwing,
        [2U] = PCIE_ControlRpMasterLoopback,
    };

    for (idx = 0U; idx < PCIE_CTRL_DEBUG_FUNC_ELEMENTS; ++idx) {
        if (CtrlDebugParamsChangeDefArray[idx] != 0U) {
            retVal = ControlDebugParamsFunction[idx]( pD, ((CtrlDebugParamsNewValueArray[idx] != 0U)
                     ? PCIE_ENABLE_PARAM : PCIE_DISABLE_PARAM) );
            if (retVal != 0U) {
                break;
            }
        }
    }
    return (retVal);
}

/* Local function to Enable/Disable various debug parameters based on presence user init values.
 */
static UINT32 EnableOrDisableParamsInit(
    const PCIE_PrivateData * pD,
    const UINT32  InitDebugParamsChangeDefArray[PCIE_INIT_DEBUG_PARAMETERS],
    const UINT32  InitDebugParamsNewValueArray[PCIE_INIT_DEBUG_PARAMETERS]
)
{
    UINT32 retVal = 0U;
    UINT32 idx;
    /* static const should not require memory allocation */
    static const PCIE_DebugParams DebugParamsArray[PCIE_INIT_DEBUG_PARAMETERS] = {
        [0U]  = PCIE_LINK_UPCONFIGURE_CAPABILITY,
        [1U]  = PCIE_FAST_LINK_TRAINING,
        [2U]  = PCIE_ELECTRICAL_IDLE_INFER_IN_L0_STATE,
        [3U]  = PCIE_FLOW_CONTROL_UPDATE_TIMEOUT,
        [4U]  = PCIE_ORDERING_CHECKS,
        [5U]  = PCIE_FUNC_SPECIFIC_RPRT_OF_TYPE1_CONF_ACC,
        [6U]  = PCIE_SLOT_POWER_CAPTURE,
        [7U]  = PCIE_SYNC_HEADER_FRAMING_ERROR,
        [8U]  = PCIE_LINK_RETRAINING_ON_FRAMING_ERROR,
        [9U]  = PCIE_GEN3_LFSR_UPDATE_FROM_SKP,
    };
    for (idx = 0U; idx < PCIE_INIT_DEBUG_PARAMETERS; ++idx) {
        if ( (InitDebugParamsChangeDefArray[idx] != 0U) &&
             (InitDebugParamsChangeDefArray[idx] != PCIE_INVALID_PARAMETER) ) {
            retVal = PCIE_ControlDebugParams(
                         pD,
                         DebugParamsArray[idx],
                         (InitDebugParamsNewValueArray[idx] != 0U) ? PCIE_ENABLE_PARAM : PCIE_DISABLE_PARAM);
            if (retVal != 0U) {
                break;
            }
        }
    }
    return (retVal);
}
/* Static function used to initialize debug parameters
 * Arrays are used to map enums to integer values
 */
UINT32 Init_DebugParams(const PCIE_PrivateData * pD,
                        const PCIE_InitParam *   pInitParam)
{
    UINT32 retVal;
    UINT32 DebugMuxModuleSelectArrayIndex;
    UINT32 InitDebugParamsChangeDefArray[PCIE_INIT_DEBUG_PARAMETERS];
    UINT32 InitDebugParamsNewValueArray[PCIE_INIT_DEBUG_PARAMETERS];
    UINT32 CtrlDebugParamsChangeDefArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS];
    UINT32 CtrlDebugParamsNewValueArray[PCIE_CTRL_DEBUG_PARAMS_ELEMENTS];

    /* Calling Local function to copy values to an array */
    FillDebugArrays(pInitParam, InitDebugParamsChangeDefArray, InitDebugParamsNewValueArray);
    FillCtrlDebugArrays(pInitParam, CtrlDebugParamsChangeDefArray, CtrlDebugParamsNewValueArray);

    /* Enable/Disable various Control parameters based on presence of user init values */
    retVal = EnableOrDisableElementsCtrl(pD, CtrlDebugParamsChangeDefArray, CtrlDebugParamsNewValueArray);

    if ( ((pInitParam->initDebugParams.muxSelect.changeDefault) != 0U) &&
         (retVal == 0U) ) {
        DebugMuxModuleSelectArrayIndex = pInitParam->initDebugParams.muxSelect.newValue;
        PCIE_DebugMuxModuleSelect val = DebugMuxModuleSelectArray[DebugMuxModuleSelectArrayIndex];
        retVal = PCIE_SetDebugMux(pD, val);
    }

    /* Enable/Disable various Debug parameters based on presence of user init values */
    retVal |= EnableOrDisableParamsInit(pD, InitDebugParamsChangeDefArray, InitDebugParamsNewValueArray);
    return (retVal);
}
