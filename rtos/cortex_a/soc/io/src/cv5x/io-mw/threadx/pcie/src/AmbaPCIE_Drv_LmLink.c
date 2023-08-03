/**
 *  @file AmbaPCIE_Drv_LmBar.c
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
 *  @details PCIe implementation of driver API functions (local management for link).
 */
#include "AmbaPCIE_Drv.h"


/****************************************************************************/
/* PHYSICAL LAYER TRAINING INFORMATION                                      */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* PCIE_Is_Link_Training_Complete                                           */
/****************************************************************************/
UINT32
PCIE_IsLinkTrainingComplete(const PCIE_PrivateData * pD,
                            PCIE_Bool *              pTrainingComplete)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;

    UINT32 result;

    /* Check input parameters are not NULL */
    if ((pD == NULL) || (pTrainingComplete == NULL)) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        if ( (PCIE_RegFldRead(LM_PL_CONFIG0_LS_MASK, LM_PL_CONFIG0_LS_SHIFT, regVal)) != 0U) {
            *pTrainingComplete = PCIE_TRUE;
        } else {
            *pTrainingComplete = PCIE_FALSE;
        }
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Get_Link_Training_State                                             */
/****************************************************************************/
UINT32
PCIE_GetLinkTrainingState(const PCIE_PrivateData * pD,
                          PCIE_LtssmState *        pLtssmState)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pLtssmState == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));
        UINT32 LtssmStateIdx = PCIE_RegFldRead(LM_PL_CONFIG0_LTSSM_MASK, LM_PL_CONFIG0_LTSSM_SHIFT, regVal);

        switch (LtssmStateIdx) {
        case 0:
            *pLtssmState = PCIE_DETECT_QUIET;
            break;
        case 1:
            *pLtssmState = PCIE_DETECT_ACTIVE;
            break;
        case 2:
            *pLtssmState = PCIE_POLLING_ACTIVE;
            break;
        case 3:
            *pLtssmState = PCIE_POLLING_COMPLIANCE;
            break;
        case 4:
            *pLtssmState = PCIE_POLLING_CONFIGURATION;
            break;
        case 5:
            *pLtssmState = PCIE_CONFIGURATION_LINKWIDTH_START;
            break;
        case 6:
            *pLtssmState = PCIE_CONFIGURATION_LINKWIDTH_ACCEPT;
            break;
        case 7:
            *pLtssmState = PCIE_CONFIGURATION_LANENUM_ACCEPT;
            break;
        case 8:
            *pLtssmState = PCIE_CONFIGURATION_LANENUM_WAIT;
            break;
        case 9:
            *pLtssmState = PCIE_CONFIGURATION_COMPLETE;
            break;
        case 10:
            *pLtssmState = PCIE_CONFIGURATION_IDLE;
            break;
        case 11:
            *pLtssmState = PCIE_RECOVERY_RCVRLOCK;
            break;
        case 12:
            *pLtssmState = PCIE_RECOVERY_SPEED;
            break;
        case 13:
            *pLtssmState = PCIE_RECOVERY_RCVRCFG;
            break;
        case 14:
            *pLtssmState = PCIE_RECOVERY_IDLE;
            break;
        case 16:
            *pLtssmState = PCIE_L0;
            break;
        case 17:
            *pLtssmState = PCIE_RX_L0S_ENTRY;
            break;
        case 18:
            *pLtssmState = PCIE_RX_L0S_IDLE;
            break;
        case 19:
            *pLtssmState = PCIE_RX_L0S_FTS;
            break;
        case 20:
            *pLtssmState = PCIE_TX_L0S_ENTRY;
            break;
        case 21:
            *pLtssmState = PCIE_TX_L0S_IDLE;
            break;
        case 22:
            *pLtssmState = PCIE_TX_L0S_FTS;
            break;
        case 23:
            *pLtssmState = PCIE_L1_ENTRY;
            break;
        case 24:
            *pLtssmState = PCIE_L1_IDLE;
            break;
        case 25:
            *pLtssmState = PCIE_L2_IDLE;
            break;
        case 26:
            *pLtssmState = PCIE_L2_TRANSMITWAKE;
            break;
        case 32:
            *pLtssmState = PCIE_DISABLED;
            break;
        case 33:
            *pLtssmState = PCIE_LOOPBACK_ENTRY_MASTER;
            break;
        case 34:
            *pLtssmState = PCIE_LOOPBACK_ACTIVE_MASTER;
            break;
        case 35:
            *pLtssmState = PCIE_LOOPBACK_EXIT_MASTER;
            break;
        case 36:
            *pLtssmState = PCIE_LOOPBACK_ENTRY_SLAVE;
            break;
        case 37:
            *pLtssmState = PCIE_LOOPBACK_ACTIVE_SLAVE;
            break;
        case 38:
            *pLtssmState = PCIE_LOOPBACK_EXIT_SLAVE;
            break;
        case 39:
            *pLtssmState = PCIE_HOT_RESET;
            break;
        case 40:
            *pLtssmState = PCIE_RECOVERY_EQ_PHASE_0;
            break;
        case 41:
            *pLtssmState = PCIE_RECOVERY_EQ_PHASE_1;
            break;
        case 42:
            *pLtssmState = PCIE_RECOVERY_EQ_PHASE_2;
            break;
        case 43:
            *pLtssmState = PCIE_RECOVERY_EQ_PHASE_3;
            break;
        default:
            *pLtssmState = PCIE_DETECT_QUIET;
            break;
        }

        if ( (LtssmStateIdx > (UINT32)PCIE_RECOVERY_EQ_PHASE_3) ||
             ((LtssmStateIdx > (UINT32)PCIE_L0) && (LtssmStateIdx < (UINT32)PCIE_TX_L0S_FTS)) ||
             ((LtssmStateIdx > (UINT32)PCIE_L2_TRANSMITWAKE) && (LtssmStateIdx < (UINT32)PCIE_LOOPBACK_ENTRY_MASTER)) ) {
            result = PCIE_ERR_ARG;
        } else {
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* PCIE_Get_Link_Training_Direction                                         */
/****************************************************************************/
UINT32
PCIE_GetLinkTrainingDirection(const PCIE_PrivateData *    pD,
                              PCIE_UpstreamOrDownstream * pUpOrDown)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pUpOrDown == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        if ( (PCIE_RegFldRead(LM_PL_CONFIG0_LTD_MASK, LM_PL_CONFIG0_LTD_SHIFT, regVal)) != 0U) {
            *pUpOrDown = PCIE_DOWNSTREAM;
        } else {
            *pUpOrDown = PCIE_UPSTREAM;
        }
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/****************************************************************************/
/* PHYSICAL LAYER LANE COUNT AND LINK SPEED INFORMATION                     */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* PCIE_Get_Negotiated_Lane_Count                                           */
/****************************************************************************/
UINT32
PCIE_GetNegotiatedLaneCount(const PCIE_PrivateData * pD,
                            PCIE_LaneCount *         nlc)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (nlc == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        switch (PCIE_RegFldRead(LM_PL_CONFIG0_NLC_MASK, LM_PL_CONFIG0_NLC_SHIFT, regVal)) {
        case 0U:
            *nlc = PCIE_LANECNT_X1;
            break;
        case 1U:
            *nlc = PCIE_LANECNT_X2;
            break;
        case 2U:
            *nlc = PCIE_LANECNT_X4;
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
/* PCIE_Get_Negotiated_Link_Speed                                           */
/****************************************************************************/
UINT32
PCIE_GetNegotiatedLinkSpeed(const PCIE_PrivateData * pD,
                            PCIE_LinkSpeed *         ns)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 ls;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (ns == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));
        ls = PCIE_RegFldRead(LM_PL_CONFIG0_NS_MASK, LM_PL_CONFIG0_NS_SHIFT, regVal);

        switch (ls) {
        /* Link speed is in GT (Gigatransfers per second) */
        case 0:
            *ns = PCIE_SPEED_2P5_GB;
            break;
        case 1:
            *ns = PCIE_SPEED_5_GB;
            break;
        case 2:
            *ns = PCIE_SPEED_8_GB;
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
/* PCIE_Get_Negotiated_Lane_Map                                             */
/****************************************************************************/
UINT32
PCIE_GetNegotiatedLaneBitMap(const PCIE_PrivateData * pD,
                             UINT32 *               pLaneMap)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pLaneMap == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_negotiated_lane_map_reg));

        *pLaneMap = (UINT8)(PCIE_RegFldRead(LM_NEGO_LANE_MAP_NLM_MASK, LM_NEGO_LANE_MAP_NLM_SHIFT, regVal));

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Are_Lane_Numbers_Reversed                                           */
/****************************************************************************/
UINT32
PCIE_AreLaneNumbersReversed(const PCIE_PrivateData * pD,
                            PCIE_Bool *              pReversed)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pReversed == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_negotiated_lane_map_reg));

        if ( (PCIE_RegFldRead(LM_NEGO_LANE_MAP_LRS_MASK, LM_NEGO_LANE_MAP_LRS_SHIFT, regVal)) != 0U) {
            *pReversed = PCIE_TRUE;
        } else {
            *pReversed = PCIE_FALSE;
        }
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/****************************************************************************/
/* PHYSICAL LAYER REMOTE INFO RECEIVED DURING TRAINING                      */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* PCIE_Get_Received_Link_ID                                                */
/****************************************************************************/
UINT32
PCIE_GetReceivedLinkId(const PCIE_PrivateData * pD,
                       UINT32 *               pLinkId)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pLinkId == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        *pLinkId = (UINT32)PCIE_RegFldRead(LM_PL_CONFIG0_RLID_MASK, LM_PL_CONFIG0_RLID_SHIFT, regVal);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Is_Remote_Linkwidth_Upconfigurable                                  */
/****************************************************************************/
UINT32
PCIE_IsRemoteLinkwidthUpconfig(const PCIE_PrivateData * pD,
                               PCIE_Bool *              upconfig)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (upconfig == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        if ( (PCIE_RegFldRead(LM_PL_CONFIG0_R0_MASK, LM_PL_CONFIG0_R0_SHIFT, regVal)) != 0U) {
            *upconfig = PCIE_TRUE;
        } else {
            *upconfig = PCIE_FALSE;
        }
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/****************************************************************************/
/* PHYSICAL_LAYER INFO SENT DURING TRAINING                                 */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* Access_Transmitted_Link_ID                                               */
/****************************************************************************/
UINT32
PCIE_GetTransmittedLinkId(
    const PCIE_PrivateData * pD,
    UINT8 *                pLinkId)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pLinkId == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg));

        *pLinkId = (UINT8)PCIE_RegFldRead(LM_PL_CONFIG1_TLI_MASK, LM_PL_CONFIG1_TLI_SHIFT, regVal);
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
UINT32
PCIE_SetTransmittedLinkId(
    const PCIE_PrivateData * pD,
    UINT8                  pLinkId)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg));

        regVal = PCIE_RegFldWrite(LM_PL_CONFIG1_TLI_MASK, LM_PL_CONFIG1_TLI_SHIFT, regVal, pLinkId);
        PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_1_reg), regVal);
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}


/****************************************************************************/
/* PCIE_ControlRpMaster_Loopback                                          */
/****************************************************************************/
UINT32 PCIE_ControlRpMasterLoopback(
    const PCIE_PrivateData * pD,
    PCIE_EnableOrDisable     enableOrDisable)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:
            regVal = PCIE_RegFldSet(LM_PL_CONFIG0_MLE_MASK, LM_PL_CONFIG0_MLE_SHIFT, LM_PL_CONFIG0_MLE_WOCLR, regVal);
            break;
        case PCIE_DISABLE_PARAM:
            regVal = PCIE_RegFldClear(LM_PL_CONFIG0_MLE_WIDTH, LM_PL_CONFIG0_MLE_MASK,
                                      LM_PL_CONFIG0_MLE_WOSET, LM_PL_CONFIG0_MLE_WOCLR, regVal);
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}


/*
 * PCIE_Control_Reporting_Of_All_Phy_Errors
 *
 * API function to control handling of PHY errors
 */
UINT32 PCIE_CtrlReportingOfAllPhyErr(
    const PCIE_PrivateData * pD,
    PCIE_EnableOrDisable     enableOrDisable)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:
            regVal = PCIE_RegFldSet(LM_PL_CONFIG0_APER_MASK, LM_PL_CONFIG0_APER_SHIFT, LM_PL_CONFIG0_APER_WOCLR, regVal);
            break;

        case PCIE_DISABLE_PARAM:
            regVal = PCIE_RegFldClear(LM_PL_CONFIG0_APER_WIDTH, LM_PL_CONFIG0_APER_MASK,
                                      LM_PL_CONFIG0_APER_WOSET, LM_PL_CONFIG0_APER_WOCLR, regVal);
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Control_Tx_Swing                                                    */
/****************************************************************************/
UINT32 PCIE_ControlTxSwing(
    const PCIE_PrivateData * pD,
    PCIE_EnableOrDisable     enableOrDisable)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));

        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:
            regVal = PCIE_RegFldSet(LM_PL_CONFIG0_TSS_MASK, LM_PL_CONFIG0_TSS_SHIFT, LM_PL_CONFIG0_TSS_WOCLR, regVal);
            break;

        case PCIE_DISABLE_PARAM:
            regVal = PCIE_RegFldClear(LM_PL_CONFIG0_TSS_WIDTH, LM_PL_CONFIG0_TSS_MASK,
                                      LM_PL_CONFIG0_TSS_WOSET, LM_PL_CONFIG0_TSS_WOCLR, regVal);
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }
        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg), regVal); /* Write back to register */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/****************************************************************************/
/* RP RETRAINING CONTROL                                                    */
/****************************************************************************/
/****************************************************************************/

static PCIE_Bool isLanemapValid(UINT32 laneMap)
{
    PCIE_Bool result;
    switch (laneMap) {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x08:
    case 0x0c:
    case 0x0f:

        result = PCIE_TRUE;
        break;
    default:
        result = PCIE_FALSE;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Access_Retraining_Target_Lanemap                                         */
/****************************************************************************/
UINT32 PCIE_GetTargetLanemap(
    const PCIE_PrivateData * pD,
    UINT32 *               lanemap)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp;
    if (lanemap == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_linkwidth_control_reg));
        *lanemap = PCIE_RegFldRead(LM_LINKWIDTH_CTRL_TLM_MASK, LM_LINKWIDTH_CTRL_TLM_SHIFT, regVal);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_SetTargetLanemap(
    const PCIE_PrivateData * pD,
    UINT32                 lanemap)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (isLanemapValid(lanemap) != PCIE_TRUE) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_linkwidth_control_reg));

        regVal = PCIE_RegFldWrite(LM_LINKWIDTH_CTRL_TLM_MASK, LM_LINKWIDTH_CTRL_TLM_SHIFT, regVal, lanemap);
        PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_linkwidth_control_reg), regVal); /* Write back to register */
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Retrain_Link                                                        */
/****************************************************************************/
static UINT32 WaitForRetrainCompletion(
    PCIE_WaitOrNot            waitForCompletion,
    const struct PCIE_IClientLm_s * pcieAddr,
    UINT32 *                pRegVal)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = *pRegVal;
    switch (waitForCompletion) {
    case PCIE_WAIT: {
        /* Wait for RL bit to be cleared.  There is no timeout, so will block here if does not complete */
        do {
            regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_linkwidth_control_reg));
            regVal = PCIE_RegFldRead(LM_LINKWIDTH_CTRL_RL_MASK, LM_LINKWIDTH_CTRL_RL_SHIFT, regVal);
        } while (regVal > 0U);
        *pRegVal = regVal;
        break;
    }

    case PCIE_DO_NOT_WAIT:
        /* Nothing to be done */
        break;

    default:
        result = PCIE_ERR_ARG;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static void SetLinkwidthRegister(
    struct PCIE_IClientLm_s * pcieAddr,
    UINT32 *                pRegVal)
{
    *pRegVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_linkwidth_control_reg)); /* Read register from PCIe IP */
    *pRegVal = PCIE_RegFldSet(LM_LINKWIDTH_CTRL_RL_MASK, LM_LINKWIDTH_CTRL_RL_SHIFT, LM_LINKWIDTH_CTRL_RL_WOCLR, *pRegVal);
    PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_linkwidth_control_reg), *pRegVal); /* Write back to register */

    return;
}
UINT32 PCIE_RetrainLink(
    const PCIE_PrivateData * pD,
    PCIE_WaitOrNot           waitForCompletion)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    PCIE_CoreEpOrRp epOrRp;
    UINT32 laneMap = 0U;
    UINT32 result = PCIE_ERR_SUCCESS;
    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_lm_base;
        SetLinkwidthRegister(pcieAddr, &regVal);

        /*
        ** Check that target lanemap is valid.
        */
        (void)PCIE_GetTargetLanemap(pD, &laneMap);
        if (isLanemapValid(laneMap) != PCIE_TRUE) {
            result = PCIE_ERR_ARG;
        } else {
            result = WaitForRetrainCompletion(waitForCompletion, pcieAddr, &regVal);

            if (result == PCIE_ERR_SUCCESS) {
                /* Check for errors reported from PCIe IP */
                result = CallErrorCallbackIfError(pD);
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Helper function to read Link Equalization Gen3 Debug register*/
static void PCIE_RdGen3DebugLane0DebugReg(const PCIE_PrivateData* pD,
        PCIE_LinkEquaDebugInfo* lEquaInfo)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;

    pcieAddr = pD->p_lm_base;

    /* Read the debug register and populate the debug info structure elements */
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_gen3_leq_debug_status_lane0));
    lEquaInfo->leqTxPr = (UINT8) PCIE_RegFldRead(LM_GEN3_LEQ_LANE0_LEQTXPR_MASK, LM_GEN3_LEQ_LANE0_LEQTXPR_SHIFT, regVal);
    if (PCIE_RegFldRead(LM_GEN3_LEQ_LANE0_LEQTXPRV_MASK, LM_GEN3_LEQ_LANE0_LEQTXPRV_SHIFT, regVal) == 0U) {
        lEquaInfo->leqTxPrValid = PCIE_FALSE;
    } else {
        lEquaInfo->leqTxPrValid = PCIE_TRUE;
    }
    lEquaInfo->leqTxCo = PCIE_RegFldRead(LM_GEN3_LEQ_LANE0_LEQTXCO_MASK, LM_GEN3_LEQ_LANE0_LEQTXCO_SHIFT, regVal);
}

/* Helper function to read Link Equalization Gen3 Debug register*/
static void PCIE_RdGen3DebugLane1DebugReg(const PCIE_PrivateData* pD,
        PCIE_LinkEquaDebugInfo* lEquaInfo)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;

    pcieAddr = pD->p_lm_base;

    /* Read the debug register and populate the debug info structure elements */
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_gen3_leq_debug_status_lane0));
    lEquaInfo->leqTxPr = (UINT8) PCIE_RegFldRead(LM_GEN3_LEQ_LANE1_LEQTXPR_MASK, LM_GEN3_LEQ_LANE1_LEQTXPR_SHIFT, regVal);
    if (PCIE_RegFldRead(LM_GEN3_LEQ_LANE1_LEQTXPRV_MASK, LM_GEN3_LEQ_LANE1_LEQTXPRV_SHIFT, regVal) == 0U) {
        lEquaInfo->leqTxPrValid = PCIE_FALSE;
    } else {
        lEquaInfo->leqTxPrValid = PCIE_TRUE;
    }
    lEquaInfo->leqTxCo = PCIE_RegFldRead(LM_GEN3_LEQ_LANE1_LEQTXCO_MASK, LM_GEN3_LEQ_LANE1_LEQTXCO_SHIFT, regVal);
}

/* Helper function to read Link Equalization Gen3 Debug register*/
static void PCIE_RdGen3DebugLane2DebugReg(const PCIE_PrivateData* pD,
        PCIE_LinkEquaDebugInfo* lEquaInfo)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;

    pcieAddr = pD->p_lm_base;

    /* Read the debug register and populate the debug info structure elements */
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_gen3_leq_debug_status_lane1));
    lEquaInfo->leqTxPr = (UINT8) PCIE_RegFldRead(LM_GEN3_LEQ_LANE2_LEQTXPR_MASK, LM_GEN3_LEQ_LANE2_LEQTXPR_SHIFT, regVal);
    if (PCIE_RegFldRead(LM_GEN3_LEQ_LANE2_LEQTXPRV_MASK, LM_GEN3_LEQ_LANE2_LEQTXPRV_SHIFT, regVal) == 0U) {
        lEquaInfo->leqTxPrValid = PCIE_FALSE;
    } else {
        lEquaInfo->leqTxPrValid = PCIE_TRUE;
    }
    lEquaInfo->leqTxCo = PCIE_RegFldRead(LM_GEN3_LEQ_LANE2_LEQTXCO_MASK, LM_GEN3_LEQ_LANE2_LEQTXCO_SHIFT, regVal);
}

/* Helper function to read Link Equalization Gen3 Debug register*/
static void PCIE_RdGen3DebugLane3DebugReg(const PCIE_PrivateData* pD,
        PCIE_LinkEquaDebugInfo* lEquaInfo)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;

    pcieAddr = pD->p_lm_base;

    /* Read the debug register and populate the debug info structure elements */
    regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_gen3_leq_debug_status_lane1));
    lEquaInfo->leqTxPr = (UINT8) PCIE_RegFldRead(LM_GEN3_LEQ_LANE3_LEQTXPR_MASK, LM_GEN3_LEQ_LANE3_LEQTXPR_SHIFT, regVal);
    if (PCIE_RegFldRead(LM_GEN3_LEQ_LANE3_LEQTXPRV_MASK, LM_GEN3_LEQ_LANE3_LEQTXPRV_SHIFT, regVal) == 0U) {
        lEquaInfo->leqTxPrValid = PCIE_FALSE;
    } else {
        lEquaInfo->leqTxPrValid = PCIE_TRUE;
    }
    lEquaInfo->leqTxCo = PCIE_RegFldRead(LM_GEN3_LEQ_LANE3_LEQTXCO_MASK, LM_GEN3_LEQ_LANE3_LEQTXCO_SHIFT, regVal);
}

UINT32 PCIE_GetLinkEquGen3DebugInfo(const PCIE_PrivateData * pD,
                                    UINT32                 reqLane,
                                    PCIE_LinkEquaDebugInfo*  lEquaInfo)
{
    UINT32 result = 0U;

    /* Check input parameters are not NULL */
    if ((pD == NULL) || (lEquaInfo == NULL)) {
        result = PCIE_ERR_ARG;
    } else {
        /* Switch based on lane number to call the corresponding read function */
        switch (reqLane) {

        case 0:
            PCIE_RdGen3DebugLane0DebugReg( pD, lEquaInfo);
            break;

        case 1:
            PCIE_RdGen3DebugLane1DebugReg( pD, lEquaInfo);
            break;

        case 2:
            PCIE_RdGen3DebugLane2DebugReg( pD, lEquaInfo);
            break;

        case 3:
            PCIE_RdGen3DebugLane3DebugReg( pD, lEquaInfo);
            break;

        default:
            /* Invalid lane number */
            result = PCIE_ERR_ARG;
            break;
        }
    }
    return result;
}


/*
 * API function to Enable/disable link down timer reset
 */
UINT32 PCIE_ControlLinkDownReset(const PCIE_PrivateData * pD,
                                 PCIE_EnableOrDisable     enableOrDisable)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ld_ctrl));

        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:
            regVal = PCIE_RegFldWrite(LM_LD_CTRL_AUTO_EN_MASK, LM_LD_CTRL_AUTO_EN_SHIFT, regVal, 1);
            break;

        case PCIE_DISABLE_PARAM:
            regVal = PCIE_RegFldWrite(LM_LD_CTRL_AUTO_EN_MASK, LM_LD_CTRL_AUTO_EN_SHIFT, regVal, 0);
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ld_ctrl), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
