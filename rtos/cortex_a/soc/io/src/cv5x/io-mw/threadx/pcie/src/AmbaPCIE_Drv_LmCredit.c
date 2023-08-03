/**
 *  @file AmbaPCIE_Drv_LmCredit.c
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
 *  @details PCIe implementation of driver API functions (local management for credit).
 */
#include "AmbaPCIE_Drv.h"

#define PCIE_CREDIT_PARAMS_ELEMENTS 10U
#define PCIE_CREDIT_LIMIT_ELEMENTS 6U
#define PCIE_CREDIT_UPDATE_ELEMENTS 4U

/****************************************************************************/
/****************************************************************************/
/* TRANSMIT AND RECEIVE CREDIT LIMITS AND UPDATE INTERVAL ROUTINES          */
/****************************************************************************/
/****************************************************************************/

/* Local function returning the address of the receive register required
 *  The function return 0 for invalid input
 */
static volatile UINT32 *  rcvClRegArrayfunc(const PCIE_PrivateData * pD, UINT8 vcNum, UINT8 regNum)
{
    struct PCIE_IClientLm_s *pcieAddr;
    volatile UINT32 * result = NULL, *reg0 = NULL, *reg1 = NULL;

    pcieAddr = pD->p_lm_base;

    if (vcNum == 0U) {
        /* VC0 registers are named differently to other VC */
        reg0 = (&(pcieAddr->i_regf_lm_pcie_base.i_rcv_cred_lim_0_reg));
        reg1 = (&(pcieAddr->i_regf_lm_pcie_base.i_rcv_cred_lim_1_reg));
    }

    if (regNum == 0U) {
        result = reg0;
    } else {
        result = reg1;
    }
    return (result); /* 0 for failure */
}

/* Local function returning the address of the transmit register required
 * The function return 0 ,for invalid input
 */
static volatile UINT32 * transmClRegArrayfunc(const PCIE_PrivateData * pD, UINT8 vcNum, UINT8 regNum)
{
    struct PCIE_IClientLm_s *pcieAddr;
    volatile UINT32 * result = NULL, *reg0 = NULL, *reg1 = NULL;

    pcieAddr = pD->p_lm_base;

    if (vcNum == 0U) {
        /* VC0 registers are named differently to other VC */
        reg0 = (&(pcieAddr->i_regf_lm_pcie_base.i_transm_cred_lim_0_reg));
        reg1 = (&(pcieAddr->i_regf_lm_pcie_base.i_transm_cred_lim_1_reg));
    }

    if (regNum == 0U) {
        result = reg0;
    } else {
        result = reg1;
    }
    return (result); /* 0 for failure */
}


/****************************************************************************/
/* Access_Credit_Limit_Settings                                             */
/****************************************************************************/
/* Local function to read PPC Transmit register field
 */
static UINT32 AccessCLS_PpcTransmRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(transmClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_TRANSM_CRED_LIM0_PPC_MASK, LM_TRANSM_CRED_LIM0_PPC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read PPC Receive register field
 */
static UINT32 AccessCLS_PpcRcvRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_RCV_CRED_LIM0_PPC_MASK, LM_RCV_CRED_LIM0_PPC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check PPC value is valid
 */
static UINT32 ValidatePpcWriteValue(
    const UINT32 * pRdWrVal,
    UINT8          vcNum)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check if the value to be written is within valid range*/
    if (vcNum == 0U) {
        if ( ((*pRdWrVal) >> LM_RCV_CRED_LIM0_PPC_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        }
    }
    return (result);           /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 AccessCLS_PpcRcvWrite(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    const UINT32 *         pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if ( ValidatePpcWriteValue(pRdWrVal, vcNum) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        if (vcNum == 0U) {
            regVal = PCIE_RegFldWrite(LM_RCV_CRED_LIM0_PPC_MASK, LM_RCV_CRED_LIM0_PPC_SHIFT, regVal, *pRdWrVal);
        }
        PCIE_RegPtrWrite32(rcvClRegArrayfunc(pD, vcNum, 0), regVal);           /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read PHC Transmit register field
 */
static UINT32 AccessCLS_PhcTransmRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(transmClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_TRANSM_CRED_LIM0_PHC_MASK, LM_TRANSM_CRED_LIM0_PHC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read PHC Receive register field
 */
static UINT32 AccessCLS_PhcRcvRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal =  PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_RCV_CRED_LIM0_PHC_MASK, LM_RCV_CRED_LIM0_PHC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to check PHC value is valid
 */
static UINT32 ValidatePhcWriteValue(
    const UINT32 * pRdWrVal,
    UINT8          vcNum)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check if the value to be written is within valid range*/
    if (vcNum == 0U) {
        if ( ((*pRdWrVal) >> LM_RCV_CRED_LIM0_PHC_WIDTH) != 0U ) {
            result = PCIE_ERR_ARG;
        }
    }
    return (result);           /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to write PHC Receive register field
 */
static UINT32 AccessCLS_PhcRcvWrite(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    const UINT32 *         pRdWrVal)
{
    UINT32 regVal =  PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if ( ValidatePhcWriteValue(pRdWrVal, vcNum) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        if (vcNum == 0U) {
            regVal = PCIE_RegFldWrite(LM_RCV_CRED_LIM0_PHC_MASK, LM_RCV_CRED_LIM0_PHC_SHIFT, regVal, *pRdWrVal);
        }
        PCIE_RegPtrWrite32(rcvClRegArrayfunc(pD, vcNum, 0), regVal);           /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read NPPC Transmit register field
 */
static UINT32 AccessCLS_NppcTransmRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(transmClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_TRANSM_CRED_LIM0_NPPC_MASK, LM_TRANSM_CRED_LIM0_NPPC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read NPPC Receive register field
 */
static UINT32 AccessCLS_NppcRcvRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_RCV_CRED_LIM0_NPPC_MASK, LM_RCV_CRED_LIM0_NPPC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check NPPC value is valid
 */
static UINT32 ValidateNppcWriteValue(
    const UINT32 * pRdWrVal,
    UINT8          vcNum)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check if the value to be written is within valid range*/
    if (vcNum == 0U) {
        if ( ((*pRdWrVal) >> LM_RCV_CRED_LIM0_NPPC_WIDTH) != 0U ) {
            result = PCIE_ERR_ARG;
        }
    }
    return (result);           /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to write NPPC Receive register field
 */
static UINT32 AccessCLS_NppcRcvWrite(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    const UINT32 *         pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 0));
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if ( ValidateNppcWriteValue(pRdWrVal, vcNum) != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        if (vcNum == 0U) {
            regVal = PCIE_RegFldWrite(LM_RCV_CRED_LIM0_NPPC_MASK, LM_RCV_CRED_LIM0_NPPC_SHIFT, regVal, *pRdWrVal);
        }
        PCIE_RegPtrWrite32(rcvClRegArrayfunc(pD, vcNum, 0), regVal);           /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to read NPHC Transmit register field
 */
static UINT32 AccessCLS_NphcTransmRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(transmClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_TRANSM_CRED_LIM1_NPHC_MASK, LM_TRANSM_CRED_LIM1_NPHC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read NPHC Receive register field
 */
static UINT32 AccessCLS_NphcRcvRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_RCV_CRED_LIM1_NPHCL_MASK, LM_RCV_CRED_LIM1_NPHCL_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check NPHC value is valid
 */
static UINT32 ValidateNphcWriteValue(
    const UINT32 * pRdWrVal,
    UINT8          vcNum)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check if the value to be written is within valid range*/
    if (vcNum == 0U) {
        /* Check within range for VC0 */
        if ( ((*pRdWrVal) >> LM_RCV_CRED_LIM1_NPHCL_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        }
    }
    return (result);           /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to write NPHC Receive register field
 */
static UINT32 AccessCLS_NphcRcvWrite(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    const UINT32 *         pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if ( ValidateNphcWriteValue(pRdWrVal, vcNum) != 0U ) {
        result = PCIE_ERR_ARG;
    } else {
        if (vcNum == 0U) {
            regVal = PCIE_RegFldWrite(LM_RCV_CRED_LIM1_NPHCL_MASK, LM_RCV_CRED_LIM1_NPHCL_SHIFT, regVal, *pRdWrVal);
        }
        PCIE_RegPtrWrite32(rcvClRegArrayfunc(pD, vcNum, 1), regVal);           /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read CPC Transmit register field
 */
static UINT32 AccessCLS_CpcTransmRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(transmClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_TRANSM_CRED_LIM1_CPC_MASK, LM_TRANSM_CRED_LIM1_CPC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read CPC Receive register field
 */
static UINT32 AccessCLS_CpcRcvRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_RCV_CRED_LIM1_CPC_MASK, LM_RCV_CRED_LIM1_CPC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check CPC value is valid
 */
static UINT32 ValidateCpcWriteValue(
    const UINT32 * pRdWrVal,
    UINT8          vcNum)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check if the value to be written is within valid range*/
    if (vcNum == 0U) {
        if ( ((*pRdWrVal) >> LM_RCV_CRED_LIM1_CPC_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        }
    }
    return (result);           /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to write CPC Receive register field
 */
static UINT32 AccessCLS_CpcRcvWrite(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    const UINT32 *         pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if ( ValidateCpcWriteValue(pRdWrVal, vcNum) != 0U ) {
        result = PCIE_ERR_ARG;
    } else {
        if (vcNum == 0U) {
            regVal = PCIE_RegFldWrite(LM_RCV_CRED_LIM1_CPC_MASK, LM_RCV_CRED_LIM1_CPC_SHIFT, regVal, *pRdWrVal);
        }
        PCIE_RegPtrWrite32(rcvClRegArrayfunc(pD, vcNum, 1), regVal);           /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read CHC Transmit register field
 */
static UINT32 AccessCLS_ChcTransmRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(transmClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_TRANSM_CRED_LIM1_CHC_MASK, LM_TRANSM_CRED_LIM1_CHC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to read CHC Receive register field
 */
static UINT32 AccessCLS_ChcRcvRead(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (vcNum == 0U) {
        *pRdWrVal = PCIE_RegFldRead(LM_RCV_CRED_LIM1_CHC_MASK, LM_RCV_CRED_LIM1_CHC_SHIFT, regVal);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check CHC value is valid
 */
static UINT32 ValidateChcWriteValue(
    const UINT32 * pRdWrVal,
    UINT8          vcNum)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    /* Check if the value to be written is within valid range*/
    if (vcNum == 0U) {
        if ( ((*pRdWrVal) >> LM_RCV_CRED_LIM1_CHC_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        }
    }
    return (result);           /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to write CHC Receive register field
 */
static UINT32 AccessCLS_ChcRcvWrite(
    const PCIE_PrivateData * pD,
    UINT8                  vcNum,
    const UINT32 *         pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(rcvClRegArrayfunc(pD, vcNum, 1));
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Validate input parameters */
    if ( ValidateChcWriteValue(pRdWrVal, vcNum) != 0U ) {
        result = PCIE_ERR_ARG;
    } else {
        if (vcNum == 0U) {
            regVal = PCIE_RegFldWrite(LM_RCV_CRED_LIM1_CHC_MASK, LM_RCV_CRED_LIM1_CHC_SHIFT, regVal, *pRdWrVal);
        }
        PCIE_RegPtrWrite32(rcvClRegArrayfunc(pD, vcNum, 1), regVal);           /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check input parameters for credit limit settings
 */
static UINT32 PCIE_ValidateCreditLmtSettings(
    PCIE_CreditLimitTypes           creditLimitParam,
    PCIE_TransmitOrReceive          transmitOrReceive,
    const PCIE_ReadOrWrite_Config * rdOrWrCfg)
{
    UINT32 status = PCIE_ERR_SUCCESS;

    PCIE_ReadOrWrite rdOrWr_valid;
    PCIE_TransmitOrReceive transmitOrReceive_valid;
    PCIE_CreditLimitTypes creditLimitParam_valid;

    rdOrWr_valid = PCIE_DO_WRITE;
    transmitOrReceive_valid = PCIE_TRANSMIT;
    creditLimitParam_valid = PCIE_COMPLETION_HEADER_CREDIT;
    /* Check if input parameters are within valid range
     * Pointers have already been checked for NULL values
     */
    if ((transmitOrReceive > transmitOrReceive_valid) ||
        (creditLimitParam > creditLimitParam_valid) ||
        ((rdOrWrCfg->rdOrWr == rdOrWr_valid) && (transmitOrReceive == transmitOrReceive_valid)) ) {
        status = PCIE_ERR_ARG;
    }

    return (status);
}
static UINT32 PCIE_WriteCreditLimitSettings(
    const PCIE_PrivateData * pD,
    PCIE_CreditLimitTypes    creditLimitParam,
    PCIE_TransmitOrReceive   transmitOrReceive,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 status;

    PCIE_ReadOrWrite_Config sRdOrWrCfg;
    sRdOrWrCfg.rdOrWr = PCIE_DO_WRITE;
    sRdOrWrCfg.pRdWrVal = pRdWrVal;

    /* Container for pointers to AccessCLS_*Write
     * There are no _TransmWrite functions
     * static const should not require memory allocation
     */
    static const AccessCreditLimitSettingsWriteFunctions WriteFunctionsArray[6] = {
        [PCIE_POSTED_PAYLOAD_CREDIT    ] = AccessCLS_PpcRcvWrite,
        [PCIE_POSTED_HEADER_CREDIT     ] = AccessCLS_PhcRcvWrite,
        [PCIE_NONPOSTED_PAYLOAD_CREDIT ] = AccessCLS_NppcRcvWrite,
        [PCIE_NONPOSTED_HEADER_CREDIT  ] = AccessCLS_NphcRcvWrite,
        [PCIE_COMPLETION_PAYLOAD_CREDIT] = AccessCLS_CpcRcvWrite,
        [PCIE_COMPLETION_HEADER_CREDIT ] = AccessCLS_ChcRcvWrite,
    };

    /* Validate input parameters */
    status = PCIE_ValidateCreditLmtSettings(creditLimitParam, transmitOrReceive, &sRdOrWrCfg );
    if (status != 0U) {
        status = PCIE_ERR_ARG;
    } else {
        status = WriteFunctionsArray[creditLimitParam](pD, vcNum, (UINT32 *)pRdWrVal);

        if (status == PCIE_ERR_SUCCESS) {
            /* Check for errors reported from PCIe IP */
            status = CallErrorCallbackIfError(pD);
        }
    }
    return (status);
}

static UINT32 PCIE_ReadCreditLimitSettings(
    const PCIE_PrivateData * pD,
    PCIE_CreditLimitTypes    creditLimitParam,
    PCIE_TransmitOrReceive   transmitOrReceive,
    UINT8                  vcNum,
    UINT32 *               pRdWrVal)
{
    UINT32 status;

    PCIE_ReadOrWrite_Config sRdOrWrCfg;
    sRdOrWrCfg.rdOrWr = PCIE_DO_READ;
    sRdOrWrCfg.pRdWrVal = pRdWrVal;

    /* Container for pointers to AccessCLS_*Read
     * There are no _TransmWrite functions */
    static const AccessCreditLimitSettingsReadFunctions ReadFunctionsArrayRx[6] = {
        [PCIE_POSTED_PAYLOAD_CREDIT    ] = AccessCLS_PpcRcvRead,
        [PCIE_POSTED_HEADER_CREDIT     ] = AccessCLS_PhcRcvRead,
        [PCIE_NONPOSTED_PAYLOAD_CREDIT ] = AccessCLS_NppcRcvRead,
        [PCIE_NONPOSTED_HEADER_CREDIT  ] = AccessCLS_NphcRcvRead,
        [PCIE_COMPLETION_PAYLOAD_CREDIT] = AccessCLS_CpcRcvRead,
        [PCIE_COMPLETION_HEADER_CREDIT ] = AccessCLS_ChcRcvRead,
    };
    static const AccessCreditLimitSettingsReadFunctions ReadFunctionsArrayTx[6] = {
        [PCIE_POSTED_PAYLOAD_CREDIT    ] = AccessCLS_PpcTransmRead,
        [PCIE_POSTED_HEADER_CREDIT     ] = AccessCLS_PhcTransmRead,
        [PCIE_NONPOSTED_PAYLOAD_CREDIT ] = AccessCLS_NppcTransmRead,
        [PCIE_NONPOSTED_HEADER_CREDIT  ] = AccessCLS_NphcTransmRead,
        [PCIE_COMPLETION_PAYLOAD_CREDIT] = AccessCLS_CpcTransmRead,
        [PCIE_COMPLETION_HEADER_CREDIT ] = AccessCLS_ChcTransmRead
    };

    /* Validate input parameters */
    status = PCIE_ValidateCreditLmtSettings(creditLimitParam, transmitOrReceive, &sRdOrWrCfg );
    if (status != 0U) {
        status = PCIE_ERR_ARG;
    } else {
        if (transmitOrReceive == PCIE_TRANSMIT) {
            status = ReadFunctionsArrayTx[creditLimitParam](pD, vcNum, (UINT32 *)pRdWrVal);
        } else {
            status = ReadFunctionsArrayRx[creditLimitParam](pD, vcNum, (UINT32 *)pRdWrVal);
        }

        if (status == PCIE_ERR_SUCCESS) {
            /* Check for errors reported from PCIe IP */
            status = CallErrorCallbackIfError(pD);
        }
    }
    return (status);
}

UINT32 PCIE_SetCreditLimitSettings(
    const PCIE_PrivateData * pD,
    PCIE_CreditLimitTypes    creditLimitParam,
    PCIE_TransmitOrReceive   transmitOrReceive,
    UINT8                  vcNum,
    UINT32                 limit)
{
    UINT32 result;
    /*
     * Check input parameters are valid
     */
    if ( (pD == NULL) || (transmitOrReceive == PCIE_TRANSMIT) || (vcNum >= 1U) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = PCIE_WriteCreditLimitSettings(pD, creditLimitParam, transmitOrReceive, vcNum, &limit);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_GetCreditLimitSettings(
    const PCIE_PrivateData * pD,
    PCIE_CreditLimitTypes    creditLimitParam,
    PCIE_TransmitOrReceive   transmitOrReceive,
    UINT8                  vcNum,
    UINT32 *               limit)
{
    UINT32 result;
    /* vcNum should be less than total virtual channels allowed for this configuration. */
    if ( (pD == NULL) || (limit == NULL) || (vcNum >= 1U) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = PCIE_ReadCreditLimitSettings(pD, creditLimitParam, transmitOrReceive, vcNum, limit);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 SetMinPostedCreditUpdInterval(
    struct PCIE_IClientLm_s *pcieAddr,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pRdWrVal)
{

    UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_0_reg));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_TX_CRED_UPDATE_CFG0_MPUI_MASK, LM_TX_CRED_UPDATE_CFG0_MPUI_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE.  Check value is within range */
        if ( ((*pRdWrVal) >> LM_TX_CRED_UPDATE_CFG0_MPUI_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_TX_CRED_UPDATE_CFG0_MPUI_MASK, LM_TX_CRED_UPDATE_CFG0_MPUI_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_0_reg), regVal);
        }
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 SetMinNonPostedCreditUpdInterv(
    struct PCIE_IClientLm_s * pcieAddr,
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pRdWrVal)
{
    UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_0_reg));
    UINT32 result = PCIE_ERR_SUCCESS;

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_TX_CRED_UPDATE_CFG0_MNUI_MASK, LM_TX_CRED_UPDATE_CFG0_MNUI_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE.  Check value is within range */
        if ( ((*pRdWrVal) >> LM_TX_CRED_UPDATE_CFG0_MNUI_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_TX_CRED_UPDATE_CFG0_MNUI_MASK, LM_TX_CRED_UPDATE_CFG0_MNUI_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_0_reg), regVal); /* Write back to register */
        }
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 SetMinCompletionUpdateInterval(
    struct PCIE_IClientLm_s * pcieAddr,
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pRdWrVal)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_1_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_TX_CRED_UPDATE_CFG1_CUI_MASK, LM_TX_CRED_UPDATE_CFG1_CUI_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE.  Check value is within range */
        if ( ((*pRdWrVal) >> LM_TX_CRED_UPDATE_CFG1_CUI_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_TX_CRED_UPDATE_CFG1_CUI_MASK, LM_TX_CRED_UPDATE_CFG1_CUI_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_1_reg), regVal); /* Write back to register */
        }
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 SetMaxUpdateIntervalForAll(
    struct PCIE_IClientLm_s * pcieAddr,
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pRdWrVal)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_1_reg));

    if (rdOrWr == PCIE_DO_READ) {
        *pRdWrVal = PCIE_RegFldRead(LM_TX_CRED_UPDATE_CFG1_MUI_MASK, LM_TX_CRED_UPDATE_CFG1_MUI_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE.  Check value is within range */
        if ( ((*pRdWrVal) >> LM_TX_CRED_UPDATE_CFG1_MUI_WIDTH) != 0U) {
            result = PCIE_ERR_ARG;
        } else {
            regVal = PCIE_RegFldWrite(LM_TX_CRED_UPDATE_CFG1_MUI_MASK, LM_TX_CRED_UPDATE_CFG1_MUI_SHIFT, regVal, *pRdWrVal);
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_tx_cred_update_int_config_1_reg), regVal); /* Write back to register */
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* Access_Transmit_Credit_Update_Interval_Settings                          */
/****************************************************************************/
static UINT32 AccessTransCrdtUpdIntrvSet(
    const PCIE_PrivateData *   pD,
    PCIE_CreditUpdateIntervals creditUpdateIntervalParam,
    PCIE_ReadOrWrite           rdOrWr,
    UINT32 *                 pRdWrVal)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    pcieAddr = pD->p_lm_base;

    /*
     * To reduce complexity, each different type of parameter is handled in
     * a different function
     */
    switch (creditUpdateIntervalParam) {
    case PCIE_MIN_POSTED_CREDIT_UPDATE_INTERVAL: {
        result = SetMinPostedCreditUpdInterval(pcieAddr, rdOrWr, pRdWrVal);
        break;
    }

    case PCIE_MIN_NONPOSTED_CREDIT_UPDATE_INTERVAL: {
        result = SetMinNonPostedCreditUpdInterv(pcieAddr, rdOrWr, pRdWrVal);
        break;
    }

    case PCIE_MIN_COMPLETION_UPDATE_INTERVAL: {
        result = SetMinCompletionUpdateInterval(pcieAddr, rdOrWr, pRdWrVal);
        break;
    }

    case PCIE_MAX_UPDATE_INTERVAL_FOR_ALL: {
        result = SetMaxUpdateIntervalForAll(pcieAddr, rdOrWr, pRdWrVal);
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
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_SetTransCreditUpdInterval(
    const PCIE_PrivateData *   pD,
    PCIE_CreditUpdateIntervals creditUpdateIntervalParam,
    UINT32 *interval)
{
    UINT32 result;
    /*
     * Check input parameters are valid
     */
    if ( (pD == NULL) || ((UINT32)creditUpdateIntervalParam > (UINT32)PCIE_MAX_UPDATE_INTERVAL_FOR_ALL) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = AccessTransCrdtUpdIntrvSet(pD, creditUpdateIntervalParam, PCIE_DO_WRITE, interval);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_GetTransCreditUpdInterval(
    const PCIE_PrivateData *   pD,
    PCIE_CreditUpdateIntervals creditUpdateIntervalParam,
    UINT32 *                 interval)
{
    UINT32 result;
    /*
     * Check input parameters are valid
     */
    if ( (pD == NULL) || (interval == NULL) || ((UINT32)creditUpdateIntervalParam > (UINT32)PCIE_MAX_UPDATE_INTERVAL_FOR_ALL) ) {
        result = PCIE_ERR_ARG;
    } else {
        result = AccessTransCrdtUpdIntrvSet(pD, creditUpdateIntervalParam, PCIE_DO_READ, interval);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillCreditParamsChangeDefArray(
    PCIE_InitCreditRelatedParams CreditParams,
    UINT32                     CreditParamsChangeDefaultArray[PCIE_CREDIT_PARAMS_ELEMENTS])
{
    /*
     * Fill array with credit parameter - changeDefault
     */
    CreditParamsChangeDefaultArray[0] = CreditParams.postedPayloadCredit.changeDefault;
    CreditParamsChangeDefaultArray[1] = CreditParams.postedHeaderCredit.changeDefault;
    CreditParamsChangeDefaultArray[2] = CreditParams.nonPostedPayloadCredit.changeDefault;
    CreditParamsChangeDefaultArray[3] = CreditParams.nonPostedHeaderCredit.changeDefault;
    CreditParamsChangeDefaultArray[4] = CreditParams.completionPayloadCredit.changeDefault;
    CreditParamsChangeDefaultArray[5] = CreditParams.completionHeaderCredit.changeDefault;
    CreditParamsChangeDefaultArray[6] = CreditParams.minPostedUpdateInterval.changeDefault;
    CreditParamsChangeDefaultArray[7] = CreditParams.minNonPostedUpdateInterval.changeDefault;
    CreditParamsChangeDefaultArray[8] = CreditParams.completionUpdateInterval.changeDefault;
    CreditParamsChangeDefaultArray[9] = CreditParams.maxUpdateInterval.changeDefault;

    /* Returning after array duly filled */
    return;
}
/* Local function to copy values to an array.
 * This is required so that we can map enum values to unsigned integer values in a MISRA compliant way
 */
static void FillCreditParamsNewValueArray(
    PCIE_InitCreditRelatedParams CreditParams,
    UINT32                     InitCreditParamsNewValueArray[PCIE_CREDIT_PARAMS_ELEMENTS])
{
    /*
     * Fill array with credit parameter - newValue
     */
    InitCreditParamsNewValueArray[0] = CreditParams.postedPayloadCredit.newValue;
    InitCreditParamsNewValueArray[1] = CreditParams.postedHeaderCredit.newValue;
    InitCreditParamsNewValueArray[2] = CreditParams.nonPostedPayloadCredit.newValue;
    InitCreditParamsNewValueArray[3] = CreditParams.nonPostedHeaderCredit.newValue;
    InitCreditParamsNewValueArray[4] = CreditParams.completionPayloadCredit.newValue;
    InitCreditParamsNewValueArray[5] = CreditParams.completionHeaderCredit.newValue;
    InitCreditParamsNewValueArray[6] = CreditParams.minPostedUpdateInterval.newValue;
    InitCreditParamsNewValueArray[7] = CreditParams.minNonPostedUpdateInterval.newValue;
    InitCreditParamsNewValueArray[8] = CreditParams.completionUpdateInterval.newValue;
    InitCreditParamsNewValueArray[9] = CreditParams.maxUpdateInterval.newValue;

    /* Returning after array duly filled */
    return;
}
UINT32 Init_CreditParams(const PCIE_PrivateData * pD,
                         const PCIE_InitParam *   pInitParam)
{
    UINT32 CreditParamsChangeDefaultArray[PCIE_CREDIT_PARAMS_ELEMENTS];
    UINT32 CreditParamsNewValueArray[PCIE_CREDIT_PARAMS_ELEMENTS];
    /* static const should not require memory allocation */
    static const PCIE_CreditLimitTypes CreditLimitTypesArray[PCIE_CREDIT_LIMIT_ELEMENTS] = {
        [0] = PCIE_POSTED_PAYLOAD_CREDIT,
        [1] = PCIE_POSTED_HEADER_CREDIT,
        [2] = PCIE_NONPOSTED_PAYLOAD_CREDIT,
        [3] = PCIE_NONPOSTED_HEADER_CREDIT,
        [4] = PCIE_COMPLETION_PAYLOAD_CREDIT,
        [5] = PCIE_COMPLETION_HEADER_CREDIT
    };
    /* static const should not require memory allocation */
    static const PCIE_CreditUpdateIntervals CreditUpdateIntervalsArray[PCIE_CREDIT_UPDATE_ELEMENTS] = {
        [0] = PCIE_MIN_POSTED_CREDIT_UPDATE_INTERVAL,
        [1] = PCIE_MIN_NONPOSTED_CREDIT_UPDATE_INTERVAL,
        [2] = PCIE_MIN_COMPLETION_UPDATE_INTERVAL,
        [3] = PCIE_MAX_UPDATE_INTERVAL_FOR_ALL
    };
    UINT32 retVal = 0U;
    UINT32 idx = 0U;

    /* Fill arrays with data */
    FillCreditParamsChangeDefArray(pInitParam->initCreditParams, CreditParamsChangeDefaultArray);
    FillCreditParamsNewValueArray(pInitParam->initCreditParams, CreditParamsNewValueArray);

    while (idx < PCIE_CREDIT_PARAMS_ELEMENTS) {
        if (idx >= PCIE_CREDIT_LIMIT_ELEMENTS) {
            if (CreditParamsChangeDefaultArray[idx] != 0U) {
                UINT32 *pval = &(CreditParamsNewValueArray[idx]);
                retVal = PCIE_SetTransCreditUpdInterval(
                             pD,
                             CreditUpdateIntervalsArray[idx - PCIE_CREDIT_LIMIT_ELEMENTS],
                             pval);
            }
        } else {
            if (CreditParamsChangeDefaultArray[idx] != 0U) {
                const UINT32 *pval = &(CreditParamsNewValueArray[idx]);
                retVal = PCIE_SetCreditLimitSettings(
                             pD,
                             CreditLimitTypesArray[idx],
                             PCIE_RECEIVE,
                             0,
                             *pval);
            }
        }
        if (retVal != 0U) {
            break;
        }
        idx = idx + 1U;
    }
    return (retVal);
}
