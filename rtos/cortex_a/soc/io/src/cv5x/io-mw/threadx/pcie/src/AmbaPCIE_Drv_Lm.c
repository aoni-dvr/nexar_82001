/**
 *  @file AmbaPCIE_Drv_Lm.c
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
 *  @details PCIe implementation of driver API functions (local management).
 */
#include "AmbaPCIE_Drv.h"

#define PCIE_REG_OFFSET_LOCAL_MGNT 0x100000U
#define PCIE_REG_OFFSET_RP_MGNT    0x200000U
#define PCIE_REG_OFFSET_AXI_WRAPPER      0x400000U
#define PCIE_REG_OFFSET_UDMA             0x600000U


static struct PCIE_IClientRc_s*
ConvertPcieBaseToRcBase(UINT64 newValue, UINT32 offset)
{
    UINT64 Addr = newValue + offset;
    struct PCIE_IClientRc_s *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Addr);
    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

/****************************************************************************/
/* PCIE_Is_Core_Strapped_As_EP_Or_RP                                        */
/****************************************************************************/
UINT32
PCIE_IsCoreStrappedAsEpOrRp(const PCIE_PrivateData * pD,
                            PCIE_CoreEpOrRp *        pEpOrRp)
{
    UINT32 result;
    UINT32 regVal;
    const struct PCIE_IClientLm_s *pcieAddr;
    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (pEpOrRp == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_pl_config_0_reg));
        if ( (PCIE_RegFldRead(LM_PL_CONFIG0_LTD_MASK, LM_PL_CONFIG0_LTD_SHIFT, regVal)) != 0U) {
            *pEpOrRp = PCIE_CORE_RP_STRAPPED;
        } else {
            *pEpOrRp = PCIE_CORE_EP_STRAPPED;
        }
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}



/****************************************************************************/
/****************************************************************************/
/* VENDOR AND SUBSYSTEM VENDOR IDs                                          */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* Access_Vendor_ID_Subsystem_Vendor_ID                                     */
/****************************************************************************/
UINT32 PCIE_GetVendorIdSubsysVendId(
    const PCIE_PrivateData * pD,
    PCIE_AdvertisedIdType    id,
    UINT16 *               idValue)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if ( (pD == NULL) || (idValue == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_lm_base;
        UINT32 regVal;
        const volatile UINT32 *vendor_id_addr = &(pcieAddr->i_regf_lm_pcie_base.i_vendor_id_reg);
        switch (id) {
        case PCIE_VENDOR_ID: {
            regVal = PCIE_RegPtrRead32(vendor_id_addr); /* Read register from PCIe IP */
            *idValue = (UINT16)PCIE_RegFldRead(LM_VID_MASK, LM_VID_SHIFT, regVal);
            break;
        }
        case PCIE_SUBSYSTEM_VENDOR_ID: {
            regVal = PCIE_RegPtrRead32(vendor_id_addr);
            *idValue = (UINT16)PCIE_RegFldRead(LM_SVID_MASK, LM_SVID_SHIFT, regVal);
            break;
        }

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

UINT32 PCIE_SetVendorIdSubsysVendId(
    const PCIE_PrivateData * pD,
    PCIE_AdvertisedIdType    id,
    UINT16                 idValue)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Check input parameters are not NULL */
    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_lm_base;
        UINT32 regVal;
        volatile UINT32 *vendor_id_addr = &(pcieAddr->i_regf_lm_pcie_base.i_vendor_id_reg);
        switch (id) {
        case PCIE_VENDOR_ID: {
            regVal = PCIE_RegPtrRead32(vendor_id_addr); /* Read register from PCIe IP */
            regVal = PCIE_RegFldWrite(LM_VID_MASK, LM_VID_SHIFT, regVal, idValue);
            PCIE_RegPtrWrite32(vendor_id_addr, regVal); /* Write back to register */
            break;
        }
        case PCIE_SUBSYSTEM_VENDOR_ID: {
            regVal = PCIE_RegPtrRead32(vendor_id_addr); /* Read register from PCIe IP */
            regVal = PCIE_RegFldWrite(LM_SVID_MASK, LM_SVID_SHIFT, regVal, idValue);
            PCIE_RegPtrWrite32(vendor_id_addr, regVal); /* Write back to register */
            break;
        }

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





/*
 * Access_Root_Port_Requestor_ID
 *
 * API function to set RP requestor ID
 */
UINT32 PCIE_GetRootPortRequestorId(
    const PCIE_PrivateData * pD,
    UINT16 *               requestorId)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (requestorId == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_root_port_requestor_id_reg));
        *requestorId = (UINT16)PCIE_RegFldRead(LM_RP_REQUESTOR_ID_RPRI_MASK, LM_RP_REQUESTOR_ID_RPRI_SHIFT, regVal);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_SetRootPortRequestorId(
    const PCIE_PrivateData * pD,
    UINT16                 requestorId)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_root_port_requestor_id_reg));
        regVal = PCIE_RegFldWrite(LM_RP_REQUESTOR_ID_RPRI_MASK, LM_RP_REQUESTOR_ID_RPRI_SHIFT, regVal, requestorId);
        PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_root_port_requestor_id_reg), regVal); /* Write back to register */

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/****************************************************************************/
/* MESSAGE GENERATION CONTROL                                               */
/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/* PCIE_Control_Snoop_NoSnoop_Latency_Req_in_LTR_Msg                        */
/****************************************************************************/
UINT32
PCIE_CtrlSnpNoSnpLatReqInLtrMsg(
    const PCIE_PrivateData * pD,
    PCIE_SnoopOrNoSnoop      snoopOrNoSnoop,
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
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg));

        switch (snoopOrNoSnoop) {
        case PCIE_NO_SNOOP: {
            switch (enableOrDisable) {
            case PCIE_ENABLE_PARAM:
                /* Set 1 to enable */
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_NSLR_MASK, LM_LTR_SNOOP_LAT_NSLR_SHIFT, regVal, 1);
                break;
            case PCIE_DISABLE_PARAM:
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_NSLR_MASK, LM_LTR_SNOOP_LAT_NSLR_SHIFT, regVal, 0);
                break;
            default:
                result = PCIE_ERR_ARG;
                break;
            }
            break;
        }

        case PCIE_SNOOP: {
            switch (enableOrDisable) {
            case PCIE_ENABLE_PARAM:
                /* Set 1 to enable */
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_SL_MASK, LM_LTR_SNOOP_LAT_SL_SHIFT, regVal, 1);
                break;
            case PCIE_DISABLE_PARAM:
                regVal = PCIE_RegFldWrite(LM_LTR_SNOOP_LAT_SL_MASK, LM_LTR_SNOOP_LAT_SL_SHIFT, regVal, 0);
                break;
            default:
                result = PCIE_ERR_ARG;
                break;
            }
            break;
        }
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_snoop_lat_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Static function to wait until the SLM bit is cleared for the LTR message
 * Will only wait if pWaitFlag is PCIE_WAIT_TILL_COMPLETE
 */
static UINT32 WaitTillComplete(
    struct PCIE_IClientLm_s *            pcieAddr,
    const PCIE_WaitOrNoWaitForComplete * pWaitFlag)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg));

    regVal = PCIE_RegFldSet(LM_LTR_MSG_GEN_CTL_SLM_MASK, LM_LTR_MSG_GEN_CTL_SLM_SHIFT, LM_LTR_MSG_GEN_CTL_SLM_WOCLR, regVal);
    PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg), regVal); /* Write back to register */

    switch (*pWaitFlag) {
    case PCIE_WAIT_TILL_COMPLETE:
        do {
            regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg));
            regVal = PCIE_RegFldRead(LM_LTR_MSG_GEN_CTL_SLM_MASK, LM_LTR_MSG_GEN_CTL_SLM_SHIFT, regVal);
        } while (regVal != 0U);
        break;

    case PCIE_NO_WAIT_TILL_COMPLETE:
        break;

    default:
        result = PCIE_ERR_ARG;
        break;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* PCIE_Send_Message                                                        */
/****************************************************************************/
/* API function to send LTR message
 */
UINT32 PCIE_SendMessage(
    const PCIE_PrivateData *     pD,
    PCIE_MessageType             msgType,
    PCIE_WaitOrNoWaitForComplete waitFlag)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        /* Only valid if IP strapped as EP */
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_lm_base;

        switch (msgType) {
        case PCIE_LTR_MESSAGE: {
            result = WaitTillComplete(pcieAddr, &waitFlag);
            break;
        }

        default:
            /* Invalid Message Type parameter */
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
/* PCIE_Enable_Automatic_Message                                            */
/****************************************************************************/
UINT32 PCIE_EnableAutomaticMessage(
    const PCIE_PrivateData * pD,
    PCIE_AutoMessageType     msgType)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    PCIE_CoreEpOrRp epOrRp;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_RP_STRAPPED) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_lm_base;

        switch (msgType) {
        case PCIE_LTR_MESSAGE_ON_FUNCTION_POWER_STATE_CHANGE: {
            UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg));

            regVal = PCIE_RegFldWrite(LM_LTR_MSG_GEN_CTL_TMFPSC_MASK, LM_LTR_MSG_GEN_CTL_TMFPSC_SHIFT, regVal, 1);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg), regVal); /* Write back to register */
            break;
        }

        case PCIE_LTR_MESSAGE_ON_LTR_MECHANISM_ENABLE: {
            UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg));

            regVal = PCIE_RegFldWrite(LM_LTR_MSG_GEN_CTL_TMLMET_MASK, LM_LTR_MSG_GEN_CTL_TMLMET_SHIFT, regVal, 1);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_ltr_msg_gen_ctl_reg), regVal); /* Write back to register */
            break;
        }
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
/* PCIE_Probe                                                               */
/****************************************************************************/
UINT32
PCIE_Probe(UINT64     cfg,
           PCIE_SysReq * sysReq)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (sysReq == NULL) || (cfg == 0U) ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Check if we have the right device */
        if (PCIE_RegAddrRead32(cfg) != (UINT32)PCIE_DEFAULT_VENDOR_DEVICE_ID) {
            sysReq->pdataSize = 0;
            result = PCIE_ERR_ARG;
        } else {
            sysReq->pdataSize = (UINT32)sizeof(PCIE_PrivateData);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Destroy                                                             */
/****************************************************************************/
void
PCIE_Destroy(void)
{
    return;
}

/****************************************************************************/
/* PCIE_Init                                                                */
/****************************************************************************/
static struct PCIE_IClientLm_s*
ConvertPcieBaseToLmBase(UINT64 newValue, UINT32 offset)
{
    UINT64 Addr = newValue + offset;
    struct PCIE_IClientLm_s *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Addr);
    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}

/* Static function to initialize Local Management Base address
 */
static void InitLmBaseAddress(PCIE_PrivateData *     pD,
                              const PCIE_InitParam * pInitParam)
{
    if ( (pInitParam->initBaseAddresses.lmBase.changeDefault) != 0U) {
        pD->p_lm_base = pInitParam->initBaseAddresses.lmBase.newLmAddrValue;
    } else {
        /* Use Default address */
        pD->p_lm_base = ConvertPcieBaseToLmBase(pD->p_pcie_base, PCIE_REG_OFFSET_LOCAL_MGNT);
    }
    pD->p_rp_base =  ConvertPcieBaseToRcBase(pD->p_pcie_base, PCIE_REG_OFFSET_RP_MGNT);
    return;
}

static struct PCIE_IClientAtu_s*
ConvertPcieBaseToAwBase(UINT64 newValue, UINT32 offset)
{
    UINT64 Addr = newValue + offset;
    struct PCIE_IClientAtu_s *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Addr);
    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}
/* Static function to initialize AXI wrapper registers base address
 */
static void InitAwBaseAddress(PCIE_PrivateData *     pD,
                              const PCIE_InitParam * pInitParam)
{
    if ( (pInitParam->initBaseAddresses.awBase.changeDefault) != 0U) {
        pD->p_aw_base = pInitParam->initBaseAddresses.awBase.newAtuAddrValue;
    } else {
        pD->p_aw_base = ConvertPcieBaseToAwBase(pD->p_pcie_base, PCIE_REG_OFFSET_AXI_WRAPPER);
    }
    return;
}
/* Static function to initialize base address of AXI region
 * There is no default address
 */
static void InitAxBaseAddress(PCIE_PrivateData *     pD,
                              const PCIE_InitParam * pInitParam)
{
    UINT64 local_p = 0U;
    if ( (pInitParam->initBaseAddresses.axBase.changeDefault) != 0U) {
        local_p = pInitParam->initBaseAddresses.axBase.newValue;
    }
    pD->p_ax_base = local_p;
    return;
}

static struct PCIE_IClientUdma_s* ConvertBaseAddrToUdmaAddrmap(UINT64 input)
{
    UINT64 Addr = input;
    struct PCIE_IClientUdma_s *ptr_ret;
    const void *vptr = PCIE_UtilityU64AddrToPtr(Addr);
    if (AmbaWrap_memcpy(&ptr_ret, &vptr, sizeof(void *)) != 0U) {
        // action TBD
    }
    return ptr_ret;
}
static void InitUdBaseAddress(PCIE_PrivateData *     pD,
                              const PCIE_InitParam * pInitParam)
{
    if ( (pInitParam->initBaseAddresses.udBase.changeDefault) != 0U) {
        pD->p_ud_base = pInitParam->initBaseAddresses.udBase.newUdmaAddrValue;
    } else {
        pD->p_ud_base = ConvertBaseAddrToUdmaAddrmap
                        (((UINT64)pD->p_pcie_base) + PCIE_REG_OFFSET_UDMA);
    }
    return;
}
/* Local function to initialize register base addresses for the PCIe IP
 * Normally these are a fixed offset, but can be customized
 */
static UINT32 Init_BaseAddress(PCIE_PrivateData *     pD,
                               const PCIE_InitParam * pInitParam)
{

    InitLmBaseAddress(pD, pInitParam);
    InitAwBaseAddress(pD, pInitParam);
    InitAxBaseAddress(pD, pInitParam);
    InitUdBaseAddress(pD, pInitParam);
    return (0);
}

/* Static function to initialize vendor id parameters
 */
static UINT32 Init_IdParams(const PCIE_PrivateData * pD,
                            const PCIE_InitParam *   pInitParam)
{
    UINT32 retVal = 0U;

    if (pInitParam->initIdParams.vendorId.changeDefault != 0U) {
        UINT32 val_32 = pInitParam->initIdParams.vendorId.newValue;
        UINT16 val_16 = (UINT16)val_32;

        /* Check if the value to be written is in valid range */
        if ( ((val_32) >> LM_VID_WIDTH) != 0U) {
            retVal = PCIE_ERR_ARG;
        } else {
            retVal = PCIE_SetVendorIdSubsysVendId(
                         pD,
                         PCIE_VENDOR_ID,
                         val_16);
        }
    }

    if ( (pInitParam->initIdParams.subsystemVendorId.changeDefault != 0U) &&
         (retVal == PCIE_ERR_SUCCESS) ) {
        UINT32 val_32 = pInitParam->initIdParams.subsystemVendorId.newValue;
        UINT16 val_16 = (UINT16)val_32;

        /* Check if the value to be written is in valid range */
        if ( ((val_32) >> LM_SVID_WIDTH) != 0U) {
            retVal = PCIE_ERR_ARG;
        } else {
            retVal = PCIE_SetVendorIdSubsysVendId(
                         pD,
                         PCIE_SUBSYSTEM_VENDOR_ID,
                         val_16);
        }
    }
    return (retVal);
}
/* Local function to initialize target lanemap parameters
 */
static UINT32 Init_TargetParams(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam
)
{
    UINT32 retVal = 0U;

    if (pInitParam->initTargetLaneMap.targetLaneMap.changeDefault != 0U) {
        UINT32 val = pInitParam->initTargetLaneMap.targetLaneMap.newValue;
        retVal = PCIE_SetTargetLanemap(
                     pD,
                     val);
    }
    return (retVal);
}


/* Helper function to check and set 8GT Transmit side preset init values */
static UINT32 processInit8GTsTxLinkEquParams(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam,
    UINT32                 laneNum)
{
    UINT32 ret_val = 0U;
    UINT32 preSet8Val = 0U;

    /* if changeDefault is 0, do nothing */
    /* Checking and setting 8 GT preset Transmit side init values */
    if (pInitParam->initLinkEquParams.downstreamTxPs8GT[laneNum].changeDefault != 0U) {
        preSet8Val = pInitParam->initLinkEquParams.downstreamTxPs8GT[laneNum].newValue;
        ret_val += PCIE_SetRp8GTsLinkEquPreset (pD, PCIE_TRANSMIT_SIDE,
                                                PCIE_DOWNSTREAM, laneNum, preSet8Val);
    }
    if (pInitParam->initLinkEquParams.upstreamTxPs8GT[laneNum].changeDefault != 0U) {
        preSet8Val = pInitParam->initLinkEquParams.upstreamTxPs8GT[laneNum].newValue;
        ret_val += PCIE_SetRp8GTsLinkEquPreset (pD, PCIE_TRANSMIT_SIDE,
                                                PCIE_UPSTREAM, laneNum, preSet8Val);
    }
    return (ret_val);
}

/* Helper function to check and set 8GT Receiver side preset init values */
static UINT32 processInit8GTsRxLinkEquParams(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam,
    UINT32                 laneNum)
{
    UINT32 ret_val = 0U;
    UINT32 preSet8Val = 0U;

    /* if changeDefault is 0, do nothing */
    /* Checking and setting 8 GT Receiver side preset init values */
    if (pInitParam->initLinkEquParams.downstreamRxPs8GT[laneNum].changeDefault != 0U) {
        preSet8Val = pInitParam->initLinkEquParams.downstreamRxPs8GT[laneNum].newValue;
        ret_val += PCIE_SetRp8GTsLinkEquPreset (pD, PCIE_RECEIVE_SIDE,
                                                PCIE_DOWNSTREAM, laneNum, preSet8Val);
    }

    if (pInitParam->initLinkEquParams.upstreamRxPs8GT[laneNum].changeDefault != 0U) {
        preSet8Val = pInitParam->initLinkEquParams.upstreamRxPs8GT[laneNum].newValue;
        ret_val += PCIE_SetRp8GTsLinkEquPreset (pD, PCIE_RECEIVE_SIDE,
                                                PCIE_UPSTREAM, laneNum, preSet8Val);
    }
    return (ret_val);
}

static UINT32 Init_LinkEquParams(
    const PCIE_PrivateData * pD,
    const PCIE_InitParam *   pInitParam)
{
    UINT32 ret_val = (UINT32)PCIE_ERR_SUCCESS;
    UINT32 laneNum;

    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;
    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        ret_val = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* If IP is strapped as EP, no requirement to init LE */
    }

    /* Proceed only if a previous call was ok. */
    else {
        for ( laneNum = 0; laneNum < PCIE_LANE_COUNT; laneNum++) {

            if (ret_val == (UINT32)PCIE_ERR_SUCCESS) {
                /* Splitting to functions to reduce HIS VOCF */
                /* Calling functions to check and set the preset values */
                ret_val = processInit8GTsTxLinkEquParams( pD, pInitParam, laneNum);
                ret_val += processInit8GTsRxLinkEquParams( pD, pInitParam, laneNum);

            }
        }
    }
    return (ret_val);
}

static UINT32 PCIE_Params_PreInit(
    PCIE_PrivateData *     pD,
    const PCIE_InitParam * pInitParam)
{
    UINT32 retVal = 0U;

    /* Set UDMA Callback */
    if (pInitParam->initUdmaCallback != NULL) {
        pD->p_td_func = pInitParam->initUdmaCallback;
    } else {
        pD->p_td_func = NULL;
    }

    /* Set Error Callback */
    if (pInitParam->initErrorCallback != NULL) {
        pD->p_ec_func = pInitParam->initErrorCallback;
    } else {
        pD->p_ec_func = NULL;
    }

    /* Trigger initialization of Debug parameters */
    retVal += Init_DebugParams(pD, pInitParam);

    /* Trigger initialization of transmitted count values */
    retVal += Init_Transmitted(pD, pInitParam);

    /* Trigger initialization of Timing parameter values */
    retVal += Init_TimingParams(pD, pInitParam);

    retVal += Init_CreditParams(pD, pInitParam);

    retVal += Init_IdParams(pD, pInitParam);
    retVal += Init_TargetParams(pD, pInitParam);

    return (retVal);
}

static UINT32 Init_AllParams(
    PCIE_PrivateData *     pD,
    const PCIE_InitParam * pInitParam)
{
    UINT32 retVal;

    /* Init the register base addresses */
    retVal = Init_BaseAddress(pD, pInitParam);

    /* Initialize general parameters */
    retVal += PCIE_Params_PreInit(pD, pInitParam);

    /* Init maskable parameters */
    retVal += Init_MaskableParams(pD, pInitParam);

    /* Init Link equalization parameters */
    retVal += Init_LinkEquParams(pD, pInitParam);

    return (retVal);
}

static UINT32 PCIE_ValidateInitParams(
    const PCIE_PrivateData * pD,
    const UINT64          cfg,
    const PCIE_InitParam *   pInitParam)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    if ( (pD == NULL) || (pInitParam == NULL) || (cfg == 0U) ) {
        result = PCIE_ERR_ARG;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* API function called at startup to initialize the hardware and private data
 */
UINT32 PCIE_Init(PCIE_PrivateData *     pD,
                 UINT64              cfg,
                 const PCIE_InitParam * pInitParam)
{
    UINT32 retVal = PCIE_ERR_ARG;

    /* Validate input parameters */
    if (PCIE_ValidateInitParams(pD, cfg, pInitParam) == PCIE_ERR_SUCCESS) {
        pD->p_pcie_base = cfg;

        pD->p_td_cfg = NULL;
        retVal = Init_AllParams(pD, pInitParam);
        if (retVal == PCIE_ERR_SUCCESS) {
            /* Initialize parameters for PF BARS */
            retVal = CallAccessPortBarFuncs(pInitParam, retVal, pD);
        }
        if (retVal == PCIE_ERR_SUCCESS) {
            /* Initialize parameters for RP BARS */
            retVal = CallAccessRootPortBarFuncs(pInitParam, retVal, pD);
        }
        retVal = CallCtrlRootPortBarCheck(pD, retVal, pInitParam);
        if (retVal == PCIE_ERR_SUCCESS) {

        }
    }

    return (retVal);
}

