/**
 *  @file AmbaPCIE_Drv_RpMgmt.c
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
 *  @details PCIe implementation of driver API functions (root port management).
 */
#include "AmbaPCIE_Drv.h"


/*
 * Local function for ECAM Transfer Config Check
 */
static PCIE_Bool PCIE_EcamTransferCfgCheck(const TransferConfigData * ptransfer_cfg)
{
    return ((((ptransfer_cfg->bus) > (UINT16)PCIE_ECAM_MAX_BUS) ||
             ((ptransfer_cfg->dev) > (UINT16)PCIE_ECAM_MAX_DEV) ||
             ((ptransfer_cfg->func) > (UINT16)PCIE_ECAM_MAX_FUNC) ||
             ((ptransfer_cfg->offset) > (UINT32)PCIE_ECAM_MAX_OFFSET))
            ? PCIE_TRUE : PCIE_FALSE);
}
/*
 * Local function for ARI ECAM Transfer Config Check
 */
static PCIE_Bool PCIE_AriEcamTransferCfgCheck(const TransferConfigData * ptransfer_cfg)
{
    return ((((ptransfer_cfg->bus) > (UINT16)PCIE_ECAM_MAX_BUS) ||
             ((ptransfer_cfg->func) > (UINT16)PCIE_ARI_ECAM_MAX_FUNC) ||
             ((ptransfer_cfg->offset) > (UINT32)PCIE_ECAM_MAX_OFFSET))
            ? PCIE_TRUE : PCIE_FALSE);
}


/*
 * Local function for ECAM Transfer Config Read
 */
static UINT32 PCIE_DoEcamConfigRead(
    const PCIE_PrivateData *   pD,
    UINT32 *                 val,
    PCIE_SizeInBytes           size,
    const TransferConfigData * ptransfer_cfg)
{
    UINT64 pcieAddr;
    UINT64 eaddr;

    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (val == NULL) || (ptransfer_cfg == NULL) ||
         ( (PCIE_EcamTransferCfgCheck(ptransfer_cfg) == PCIE_TRUE) ||
           ( pD->p_ax_base == 0U ) ) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ax_base;

        eaddr = ECAMADDR(pcieAddr, (UINT64)(ptransfer_cfg->bus), (UINT64)(ptransfer_cfg->dev), (UINT64)(ptransfer_cfg->func), (UINT64)(ptransfer_cfg->offset));

        /**
         * read the required width
         */
        switch (size) {
        case PCIE_1_BYTE:
            *val = PCIE_RegAddrRead8(eaddr);
            break;
        case PCIE_2_BYTE:
            *val = PCIE_RegAddrRead16(eaddr);
            break;
        case PCIE_4_BYTE:
            *val = PCIE_RegAddrRead32(eaddr);
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* ECAM config write                                                        */
/****************************************************************************/
static UINT32 PCIE_DoEcamConfigWrite(
    const PCIE_PrivateData *   pD,
    UINT32                   data,
    PCIE_SizeInBytes           size,
    const TransferConfigData * ptransfer_cfg)
{
    UINT64 pcieAddr;
    UINT64 eaddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (ptransfer_cfg == NULL) ||
         ( (PCIE_EcamTransferCfgCheck(ptransfer_cfg) == PCIE_TRUE) ||
           ( pD->p_ax_base == 0U ) ) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ax_base;

        /**
         * Hardware is strapped (only) for ECAM addressing
         */
        eaddr = ECAMADDR(pcieAddr, (UINT64)(ptransfer_cfg->bus), (UINT64)(ptransfer_cfg->dev), (UINT64)(ptransfer_cfg->func), (UINT64)(ptransfer_cfg->offset));

        /**
         * write the required width
         */
        switch (size) {
        case PCIE_1_BYTE:
            PCIE_RegAddrWrite8(eaddr, (UINT8)(data & 0xFFU));
            break;
        case PCIE_2_BYTE:
            PCIE_RegAddrWrite16(eaddr, (UINT16)(data & 0xFFFFU));
            break;
        case PCIE_4_BYTE:
            PCIE_RegAddrWrite32(eaddr, data);
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
/* ECAM ARI config read                                                     */
/****************************************************************************/
static UINT32 PCIE_DoAriEcamConfigRead(
    const PCIE_PrivateData *   pD,
    UINT32 *                 val,
    PCIE_SizeInBytes           size,
    const TransferConfigData * ptransfer_cfg)
{
    UINT64 pcieAddr;
    UINT64 eaddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (val == NULL) || (ptransfer_cfg == NULL) ||
         ( pD->p_ax_base == 0U ) ||
         ( (PCIE_AriEcamTransferCfgCheck(ptransfer_cfg) == PCIE_TRUE) ) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ax_base;

        eaddr = ARI_ECAMADDR(pcieAddr, (UINT64)(ptransfer_cfg->bus), (UINT64)(ptransfer_cfg->func), (UINT64)(ptransfer_cfg->offset));

        /**
         * read the required width
         */
        switch (size) {
        case PCIE_1_BYTE:
            *val = PCIE_RegAddrRead8(eaddr);
            break;
        case PCIE_2_BYTE:
            *val = PCIE_RegAddrRead16(eaddr);
            break;
        case PCIE_4_BYTE:
            *val = PCIE_RegAddrRead32(eaddr);
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* ECAM config write with ARI form of addressing                            */
/****************************************************************************/
static UINT32 PCIE_DoAriEcamConfigWrite(
    const PCIE_PrivateData *   pD,
    UINT32                   data,
    PCIE_SizeInBytes           size,
    const TransferConfigData * ptransfer_cfg)
{
    UINT64 pcieAddr;
    UINT64 eaddr;

    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (ptransfer_cfg == NULL) ||
         ( pD->p_ax_base == 0U ) ||
         ( (PCIE_AriEcamTransferCfgCheck(ptransfer_cfg) == PCIE_TRUE) ) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ax_base;

        /**
         * Hardware is strapped (only) for ECAM addressing
         */
        eaddr = ARI_ECAMADDR(pcieAddr, (UINT64)(ptransfer_cfg->bus), (UINT64)(ptransfer_cfg->func), (UINT64)(ptransfer_cfg->offset));

        /**
         * write the required width
         */
        switch (size) {
        case PCIE_1_BYTE:
            PCIE_RegAddrWrite8(eaddr, (UINT8)(data & 0xFFU));
            break;
        case PCIE_2_BYTE:
            PCIE_RegAddrWrite16(eaddr, (UINT16)(data & 0xFFFFU));
            break;
        case PCIE_4_BYTE:
            PCIE_RegAddrWrite32(eaddr, data);
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
/* Config write                                                             */
/****************************************************************************/
UINT32 PCIE_DoConfigWrite(const PCIE_PrivateData * pD,
                          UINT16                 bdf,
                          UINT32                 offset,
                          PCIE_SizeInBytes         size,
                          UINT32                 data)
{
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    UINT32 result = PCIE_ERR_SUCCESS;

    TransferConfigData stransfer_cfg = {
        /* get upper 8 bits from BDF and shift right */
        .bus = ((bdf & 0xFF00U) >> 8U),
        /* get 5 dev bits from BDF and shift right */
        .dev = ((bdf & 0x00F8U) >> 3U),
        /* get 3 func bits from BDF */
        .func = (bdf & 0x0007U),
        .offset = offset,
    };

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (PCIE_DoEcamConfigWrite(pD, data, size, &stransfer_cfg) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Config read                                                              */
/****************************************************************************/
UINT32 PCIE_DoConfigRead(const PCIE_PrivateData * pD,
                         UINT16                 bdf,
                         UINT32                 offset,
                         PCIE_SizeInBytes         size,
                         UINT32 *               data)
{
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;
    UINT32 var;
    UINT32 result = PCIE_ERR_SUCCESS;

    TransferConfigData stransfer_cfg = {
        /* get upper 8 bits from BDF and shift right */
        .bus = ((bdf & 0xFF00U) >> 8U),
        /* get 5 dev bits from BDF and shift right */
        .dev = ((bdf & 0x00F8U) >> 3U),
        /* get 3 func bits from BDF */
        .func = (bdf & 0x0007U),
        .offset = offset,
    };

    if (data == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (PCIE_DoEcamConfigRead(pD, &var, size,  &stransfer_cfg) != 0U) {
        *data = 0;
        result = PCIE_ERR_ARG;
    } else {
        *data = var;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* ARI Config write                                                         */
/****************************************************************************/
UINT32 PCIE_DoAriConfigWrite(const PCIE_PrivateData * pD,
                             UINT16                 bdf,
                             UINT32                 offset,
                             PCIE_SizeInBytes         size,
                             UINT32                 data)
{
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    UINT32 result;

    TransferConfigData stransfer_cfg = {
        .bus = ((bdf & 0xFF00U) >> 8U),
        .dev = 0U,
        .func = (bdf & 0x00FFU),
        .offset = offset,
    };

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (PCIE_DoAriEcamConfigWrite(pD, data, size, &stransfer_cfg) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* ARI Config read                                                          */
/****************************************************************************/
UINT32 PCIE_DoAriConfigRead(const PCIE_PrivateData * pD,
                            UINT16                 bdf,
                            UINT32                 offset,
                            PCIE_SizeInBytes         size,
                            UINT32 *               data)
{
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;
    UINT32 var;
    UINT32 result;

    TransferConfigData stransfer_cfg = {
        .bus = ((bdf & 0xFF00U) >> 8U),
        .dev = 0U,
        .func = (bdf & 0x00FFU),
        .offset = offset,
    };

    if (data == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (PCIE_DoAriEcamConfigRead(pD, &var, size, &stransfer_cfg) != 0U) {
        *data = 0;
        result = PCIE_ERR_ARG;
    } else {
        *data = var;
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Read the value of RP BAR register mentioned in 'bar' parameter           */
/****************************************************************************/
UINT32 PCIE_GetRootPortBAR(const PCIE_PrivateData * pD,
                           UINT32                 bar,
                           UINT32 *               barVal)
{
    struct PCIE_IClientRc_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (barVal == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_rp_base;
        switch (bar) {
        case 0:
            /* Filling with 1 to find the writeable bits */
            PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0), 0xFFFFFFF0u);
            *barVal = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0));
            break;
        case 1:
            /* Filling with 1 to find the writeable bits */
            PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1), 0xFFFFFFF0u);
            *barVal = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1));
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}


/****************************************************************************/
/* Update BAR registers on RP                                               */
/* This is using local management bus, no need for config writes            */
/****************************************************************************/
static UINT32 ValidInputEnableRpMemBarAccess(
    PCIE_CoreEpOrRp *        pEpOrRp,
    const PCIE_PrivateData * pD)

{
    UINT32 result;
    if (PCIE_IsCoreStrappedAsEpOrRp(pD, pEpOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (*pEpOrRp == PCIE_CORE_EP_STRAPPED) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CheckWritableBitsInAddress(
    UINT32 barsize,
    UINT32 addr)
{
    /* return non-zero if some bits in address are not writable */
    return ((~barsize & addr) & 0xFFFFFFF0U);
}
static UINT32 WriteAddressFor64BitBar(
    struct PCIE_IClientRc_s * pcieAddr,
    UINT32                  addr_hi,
    UINT32                  addr_lo)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Bar is configured for 64bits, write the upper address in bar 1 */
    PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1), 0xFFFFFFFFU);
    UINT32 barsize = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1));
    if (CheckWritableBitsInAddress(barsize, addr_hi) != 0U) {
        /* Some bits in requested address are not writeable, fail. */
        result = PCIE_ERR_ARG;
    } else {
        PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1), addr_hi);
        PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0), addr_lo);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 ValidateBar0Config(
    struct PCIE_IClientRc_s * pcieAddr,
    UINT32                  addr_lo,
    UINT32                  addr_hi)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 barsize;
    PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0), 0xFFFFFFF0U);
    barsize = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0));
    /* Bar is configured for IO access OR
     * Some bits in requested address are not writeable, fail */
    if ( ((barsize & 0x1U) != 0U) ||
         (CheckWritableBitsInAddress(barsize, addr_lo) != 0U) ) {
        result = PCIE_ERR_ARG;
    } else if ((barsize & 0x4U) != 0U) {
        result = WriteAddressFor64BitBar(pcieAddr, addr_hi, addr_lo);
    } else {
        PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0), addr_lo);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 ValidateBar1Config(
    struct PCIE_IClientRc_s * pcieAddr,
    UINT32                  addr_lo)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 barsize;
    UINT32 bar0 = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_0));

    PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1), 0xFFFFFFF0U);
    barsize = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1));
    /* Bar is configured for IO access OR
     * Some bits in requested address are not writeable, fail */
    if ( ((barsize & 0x1U) != 0U) ||
         (CheckWritableBitsInAddress(barsize, addr_lo) != 0U) ) {
        result = PCIE_ERR_ARG;
    } else if ((bar0 & 0x4U) != 0U) {
        /* BAR0 is configured for 64bits, fail*/
        result = PCIE_ERR_ARG;
    } else {
        PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_RC_BAR_1), addr_lo);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}


/* Helper function - 8GT Read Link equalization preset */
static UINT32 PCIE_Rp8GTsRdLane0LnkEquPreset(UINT32                   regVal,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32*                  presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input parameter to read the corresponding field value */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_DNTP0_MASK, RP_LANE_EQ_CTRL0_DNTP0_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_DNRPH0_MASK, RP_LANE_EQ_CTRL0_DNRPH0_SHIFT, regVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_UPTP0_MASK, RP_LANE_EQ_CTRL0_UPTP0_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_UPRPH0_MASK, RP_LANE_EQ_CTRL0_UPRPH0_SHIFT, regVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;

}
/* Helper function - 8GT Read Link equalization preset */
static UINT32 PCIE_Rp8GTsRdLane1LnkEquPreset(UINT32                   regVal,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32*                  presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input parameter to read the corresponding field value */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_DNTP1_MASK, RP_LANE_EQ_CTRL0_DNTP1_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_DNRPH1_MASK, RP_LANE_EQ_CTRL0_DNRPH1_SHIFT, regVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_UPTP1_MASK, RP_LANE_EQ_CTRL0_UPTP1_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL0_UPRPH1_MASK, RP_LANE_EQ_CTRL0_UPRPH1_SHIFT, regVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;

}
/* Helper function - 8GT Read Link equalization preset */
static UINT32 PCIE_Rp8GTsRdLane2LnkEquPreset(UINT32                   regVal,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32*                  presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input parameter to read the corresponding field value */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_DNTP0_MASK, RP_LANE_EQ_CTRL1_DNTP0_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_DNRPH0_MASK, RP_LANE_EQ_CTRL1_DNRPH0_SHIFT, regVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_UPTP0_MASK, RP_LANE_EQ_CTRL1_UPTP0_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_UPRPH0_MASK, RP_LANE_EQ_CTRL1_UPRPH0_SHIFT, regVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;

}
/* Helper function - 8GT Read Link equalization preset */
static UINT32 PCIE_Rp8GTsRdLane3LnkEquPreset(UINT32                   regVal,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32*                  presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input parameter to read the corresponding field value */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_DNTP1_MASK, RP_LANE_EQ_CTRL1_DNTP1_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_DNRPH1_MASK, RP_LANE_EQ_CTRL1_DNRPH1_SHIFT, regVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_UPTP1_MASK, RP_LANE_EQ_CTRL1_UPTP1_SHIFT, regVal);
        } else {
            *presetVal = (UINT8)PCIE_RegFldRead(RP_LANE_EQ_CTRL1_UPRPH1_MASK, RP_LANE_EQ_CTRL1_UPRPH1_SHIFT, regVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;

}

UINT32 PCIE_GetRp8GTsLinkEquPreset(const PCIE_PrivateData *   pD,
                                   PCIE_TransmitOrReceiveSide txRxSide,
                                   PCIE_UpstreamOrDownstream  upDownStrm,
                                   UINT32                   reqLane,
                                   UINT32*                  presetValPtr)
{
    const struct PCIE_IClientRc_s *pcieAddr;
    UINT32 result = 0U;
    UINT32 regVal = 0U;

    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if ((presetValPtr == NULL) ||
        ((txRxSide != PCIE_RECEIVE_SIDE) &&
         (txRxSide != PCIE_TRANSMIT_SIDE))) {
        /* Invalid input parameters */
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* API got called with EP private data
         * This API is only valid if IP is strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_rp_base;
        switch (reqLane) {

        case 0:
            /* Reading LE register based on lane number and trigger the read function call */
            regVal  = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_0));
            result  = PCIE_Rp8GTsRdLane0LnkEquPreset( regVal, txRxSide, upDownStrm, presetValPtr);
            break;

        case 1:
            /* Reading LE register based on lane number and trigger the read function call */
            regVal  = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_0));
            result  = PCIE_Rp8GTsRdLane1LnkEquPreset( regVal, txRxSide, upDownStrm, presetValPtr);
            break;

        case 2:
            /* Reading LE register based on lane number and trigger the read function call */
            regVal  = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_1));
            result  = PCIE_Rp8GTsRdLane2LnkEquPreset( regVal, txRxSide, upDownStrm, presetValPtr);
            break;

        case 3:
            /* Reading LE register based on lane number and trigger the read function call */
            regVal  = PCIE_RegPtrRead32(&(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_1));
            result  = PCIE_Rp8GTsRdLane3LnkEquPreset( regVal, txRxSide, upDownStrm, presetValPtr);
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }
    }
    return (result); /* 0 for success (EOK) */
}

/* Helper function - 8GT process Link equalization preset*/
static UINT32 PCIE_Rp8GTsProcessLane0LePrst(UINT32*                  regValPtr,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32                   presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input paramenters to figure out the correct register field to update */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_DNTP0_MASK, RP_LANE_EQ_CTRL0_DNTP0_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_DNRPH0_MASK, RP_LANE_EQ_CTRL0_DNRPH0_SHIFT, *regValPtr, presetVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_UPTP0_MASK, RP_LANE_EQ_CTRL0_UPTP0_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_UPRPH0_MASK, RP_LANE_EQ_CTRL0_UPRPH0_SHIFT, *regValPtr, presetVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;
}
/* Helper function - 8GT process Link equalization preset*/
static UINT32 PCIE_Rp8GTsProcessLane1LePrst(UINT32*                  regValPtr,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32                   presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input paramenters to figure out the correct register field to update */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_DNTP1_MASK, RP_LANE_EQ_CTRL0_DNTP1_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_DNRPH1_MASK, RP_LANE_EQ_CTRL0_DNRPH1_SHIFT, *regValPtr, presetVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_UPTP1_MASK, RP_LANE_EQ_CTRL0_UPTP1_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL0_UPRPH1_MASK, RP_LANE_EQ_CTRL0_UPRPH1_SHIFT, *regValPtr, presetVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;
}
/* Helper function - 8GT process Link equalization preset*/
static UINT32 PCIE_Rp8GTsProcessLane2LePrst(UINT32*                  regValPtr,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32                   presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input paramenters to figure out the correct register field to update */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_DNTP0_MASK, RP_LANE_EQ_CTRL1_DNTP0_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_DNRPH0_MASK, RP_LANE_EQ_CTRL1_DNRPH0_SHIFT, *regValPtr, presetVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_UPTP0_MASK, RP_LANE_EQ_CTRL1_UPTP0_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_UPRPH0_MASK, RP_LANE_EQ_CTRL1_UPRPH0_SHIFT, *regValPtr, presetVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;
}
/* Helper function - 8GT process Link equalization preset*/
static UINT32 PCIE_Rp8GTsProcessLane3LePrst(UINT32*                  regValPtr,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32                   presetVal)
{
    UINT32 result = 0U;

    /* Switch based on input paramenters to figure out the correct register field to update */
    switch (upDownStrm) {
    case PCIE_DOWNSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_DNTP1_MASK, RP_LANE_EQ_CTRL1_DNTP1_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_DNRPH1_MASK, RP_LANE_EQ_CTRL1_DNRPH1_SHIFT, *regValPtr, presetVal);
        }
        break;
    case PCIE_UPSTREAM:
        if (txRxSide == PCIE_TRANSMIT_SIDE) {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_UPTP1_MASK, RP_LANE_EQ_CTRL1_UPTP1_SHIFT, *regValPtr, presetVal);
        } else {
            *regValPtr = PCIE_RegFldWrite(RP_LANE_EQ_CTRL1_UPRPH1_MASK, RP_LANE_EQ_CTRL1_UPRPH1_SHIFT, *regValPtr, presetVal);
        }
        break;
    default:
        /* Invalid input parameter */
        result = PCIE_ERR_ARG;
        break;
    }
    return result;
}

/* Helper function to write the preset value for the required lane */
static UINT32 PCIE_WrRp8GTsLanePair1LePrst(const PCIE_PrivateData *   pD,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32                   reqLane,
        UINT32                   presetVal)
{
    struct PCIE_IClientRc_s *pcieAddr;
    volatile UINT32*  regAddressPtr;
    UINT32 regVal = 0U;
    UINT32 result = 0U;

    pcieAddr = pD->p_rp_base;

    switch (reqLane) {

    case 2:
        regAddressPtr = &(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_1);
        regVal = PCIE_RegPtrRead32(regAddressPtr);
        result = PCIE_Rp8GTsProcessLane2LePrst( &regVal, txRxSide, upDownStrm, presetVal);
        if (result == (UINT32)EOK) {
            /* Write the user given preset value to the identified register.*/
            PCIE_RegPtrWrite32(regAddressPtr, regVal);
        }
        break;

    case 3:
        regAddressPtr = &(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_1);
        regVal = PCIE_RegPtrRead32(regAddressPtr);
        result = PCIE_Rp8GTsProcessLane3LePrst( &regVal, txRxSide, upDownStrm, presetVal);
        if (result == (UINT32)EOK) {
            /* Write the user given preset value to the identified register.*/
            PCIE_RegPtrWrite32(regAddressPtr, regVal);
        }
        break;

    default:
        result = PCIE_ERR_ARG;
        break;
    }

    return (result); /* 0 for success (EOK) */
}
/* Helper function to write the preset value for the required lane */
static UINT32 PCIE_WrRp8GTsLanePair0LePrst(const PCIE_PrivateData *   pD,
        PCIE_TransmitOrReceiveSide txRxSide,
        PCIE_UpstreamOrDownstream  upDownStrm,
        UINT32                   reqLane,
        UINT32                   presetVal)
{
    struct PCIE_IClientRc_s *pcieAddr;
    volatile UINT32*  regAddressPtr;
    UINT32 regVal = 0U;
    UINT32 result = 0U;

    pcieAddr = pD->p_rp_base;

    switch (reqLane) {

    case 0:
        regAddressPtr = &(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_0);
        regVal = PCIE_RegPtrRead32(regAddressPtr);
        result = PCIE_Rp8GTsProcessLane0LePrst( &regVal, txRxSide, upDownStrm, presetVal);
        if (result == (UINT32)EOK) {
            /* Write the user given preset value to the identified register.*/
            PCIE_RegPtrWrite32(regAddressPtr, regVal);
        }
        break;

    case 1:
        regAddressPtr = &(pcieAddr->i_rc_pcie_base.i_lane_equalization_control_0);
        regVal = PCIE_RegPtrRead32(regAddressPtr);
        result = PCIE_Rp8GTsProcessLane1LePrst( &regVal, txRxSide, upDownStrm, presetVal);
        if (result == (UINT32)EOK) {
            /* Write the user given preset value to the identified register.*/
            PCIE_RegPtrWrite32(regAddressPtr, regVal);
        }
        break;

    default:
        /* Calling the next pair of lane to see if it contains the user requested lane */
        result = PCIE_WrRp8GTsLanePair1LePrst( pD, txRxSide, upDownStrm, reqLane, presetVal);
        break;
    }

    return (result); /* 0 for success (EOK) */
}

UINT32 PCIE_SetRp8GTsLinkEquPreset(const PCIE_PrivateData *   pD,
                                   PCIE_TransmitOrReceiveSide txRxSide,
                                   PCIE_UpstreamOrDownstream  upDownStrm,
                                   UINT32                   reqLane,
                                   UINT32                   presetVal)
{
    UINT32 result = 0U;

    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if ((txRxSide != PCIE_RECEIVE_SIDE) &&
        (txRxSide != PCIE_TRANSMIT_SIDE)) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Splitting to smaller functions to reduce HIS VOCF metrics*/
        /* Writing the preset value depending on lane number */;
        result = PCIE_WrRp8GTsLanePair0LePrst(pD, txRxSide, upDownStrm, reqLane, presetVal);
    }
    return (result); /* 0 for success (EOK) */
}


UINT32 PCIE_EnableRpMemBarAccess(const PCIE_PrivateData * pD,
                                 PCIE_RpBarNumber       bar,
                                 UINT32                 addr_hi,
                                 UINT32                 addr_lo)
{

    struct PCIE_IClientRc_s *pcieAddr;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (ValidInputEnableRpMemBarAccess(
            &epOrRp,
            pD) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_rp_base;
        switch (bar) {
        case PCIE_RP_BAR_0:
            /* Validate input parameters */
            result = ValidateBar0Config(pcieAddr, addr_lo, addr_hi);
            break;
        case PCIE_RP_BAR_1:
            /* Validate input parameters */
            result = ValidateBar1Config(pcieAddr, addr_lo);
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }

        // enable bus-master and Mem access
        PCIE_RegPtrWrite32(&(pcieAddr->i_rc_pcie_base.i_command_status), 0x6U);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
