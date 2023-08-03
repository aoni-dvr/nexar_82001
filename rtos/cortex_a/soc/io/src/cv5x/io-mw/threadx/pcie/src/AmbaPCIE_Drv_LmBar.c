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
 *  @details PCIe implementation of driver API functions (local management for bar).
 */
#include "AmbaPCIE_Drv.h"


#define PCIE_RP_APERTURE_SIZE_ELEMENTS 37U
#define PCIE_RP_BAR_CTRL_ELEMENTS 8U
#define PCIE_RP_BAR_NUM_ELEMENTS 3U

#define PCIE_APERTURE_SIZE_ELEMENTS 32U
#define PCIE_BAR_NUM_ELEMENTS 6U

#define PCIE_TYPE1_CONFIG_CONTROL_SIZE 16U

/****************************************************************************/
/****************************************************************************/
/* BAR APERTURE AND CONTROL CONFIGURATION                                   */
/****************************************************************************/
/****************************************************************************/

static PCIE_BarApertureSize GetApertureSize(UINT32 Index)
{
    PCIE_BarApertureSize size;
    switch(Index) {
    case 5:
        size = PCIE_APERTURE_SIZE_4K;
        break;
    case 6:
        size = PCIE_APERTURE_SIZE_8K;
        break;
    case 7:
        size = PCIE_APERTURE_SIZE_16K;
        break;
    case 8:
        size = PCIE_APERTURE_SIZE_32K;
        break;
    case 9:
        size = PCIE_APERTURE_SIZE_64K;
        break;
    case 10:
        size = PCIE_APERTURE_SIZE_128K;
        break;
    case 11:
        size = PCIE_APERTURE_SIZE_256K;
        break;
    case 12:
        size = PCIE_APERTURE_SIZE_512K;
        break;
    case 13:
        size = PCIE_APERTURE_SIZE_1M;
        break;
    case 14:
        size = PCIE_APERTURE_SIZE_2M;
        break;
    case 15:
        size = PCIE_APERTURE_SIZE_4M;
        break;
    case 16:
        size = PCIE_APERTURE_SIZE_8M;
        break;
    case 17:
        size = PCIE_APERTURE_SIZE_16M;
        break;
    case 18:
        size = PCIE_APERTURE_SIZE_32M;
        break;
    case 19:
        size = PCIE_APERTURE_SIZE_64M;
        break;
    case 20:
        size = PCIE_APERTURE_SIZE_128M;
        break;
    case 21:
        size = PCIE_APERTURE_SIZE_256M;
        break;
    case 22:
        size = PCIE_APERTURE_SIZE_512M;
        break;
    case 23:
        size = PCIE_APERTURE_SIZE_1G;
        break;
    case 24:
        size = PCIE_APERTURE_SIZE_2G;
        break;
    case 25:
        size = PCIE_APERTURE_SIZE_4G;
        break;
    case 26:
        size = PCIE_APERTURE_SIZE_8G;
        break;
    case 27:
        size = PCIE_APERTURE_SIZE_16G;
        break;
    case 28:
        size = PCIE_APERTURE_SIZE_32G;
        break;
    case 29:
        size = PCIE_APERTURE_SIZE_64G;
        break;
    case 30:
        size = PCIE_APERTURE_SIZE_128G;
        break;
    case 31:
        size = PCIE_APERTURE_SIZE_256G;
        break;
    default:
        size = PCIE_APERTURE_SIZE_NA;
        break;
    }
    return size;
}

static void CallFuncApertureBar0(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    UINT32 *          pApertureSize)
{
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        *pApertureSize = PCIE_RegFldRead(LM_PF_CONFIG0_BAR0A_MASK, LM_PF_CONFIG0_BAR0A_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR0A_MASK, LM_PF_CONFIG0_BAR0A_SHIFT, regVal, *pApertureSize);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }

    return;
}

static void CallFuncApertureBar1(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    UINT32 *          pApertureSize)
{
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        *pApertureSize  = PCIE_RegFldRead(LM_PF_CONFIG0_BAR1A_MASK, LM_PF_CONFIG0_BAR1A_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR1A_MASK, LM_PF_CONFIG0_BAR1A_SHIFT, regVal, *pApertureSize);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }

    return;
}

static void CallFuncApertureBar2(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    UINT32 *          pApertureSize)
{

    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        *pApertureSize  = PCIE_RegFldRead(LM_PF_CONFIG0_BAR2A_MASK, LM_PF_CONFIG0_BAR2A_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR2A_MASK, LM_PF_CONFIG0_BAR2A_SHIFT, regVal, *pApertureSize);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }

    return;
}

static void CallFuncApertureBar3(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    UINT32 *          pApertureSize)
{
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        *pApertureSize  = PCIE_RegFldRead(LM_PF_CONFIG0_BAR3A_MASK, LM_PF_CONFIG0_BAR3A_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR3A_MASK, LM_PF_CONFIG0_BAR3A_SHIFT, regVal, *pApertureSize);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }

    return;
}

static void CallFuncApertureBar4(
    volatile UINT32 * reg_1,
    PCIE_ReadOrWrite    rdOrWr,
    UINT32 *          pApertureSize)
{
    UINT32 regVal = PCIE_RegPtrRead32(reg_1);

    if (rdOrWr == PCIE_DO_READ) {
        *pApertureSize  = PCIE_RegFldRead(LM_PF_CONFIG1_BAR4A_MASK, LM_PF_CONFIG1_BAR4A_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG1_BAR4A_MASK, LM_PF_CONFIG1_BAR4A_SHIFT, regVal, *pApertureSize);
        PCIE_RegPtrWrite32(reg_1, regVal); /* Write back to register */
    }

    return;
}

static void CallFuncApertureBar5(
    volatile UINT32 * reg_1,
    PCIE_ReadOrWrite    rdOrWr,
    UINT32 *          pApertureSize)
{
    UINT32 regVal = PCIE_RegPtrRead32(reg_1);

    if (rdOrWr == PCIE_DO_READ) {
        *pApertureSize  = PCIE_RegFldRead(LM_PF_CONFIG1_BAR5A_MASK, LM_PF_CONFIG1_BAR5A_SHIFT, regVal);
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG1_BAR5A_MASK, LM_PF_CONFIG1_BAR5A_SHIFT, regVal, *pApertureSize);
        PCIE_RegPtrWrite32(reg_1, regVal); /* Write back to register */
    }

    return;
}
/****************************************************************************/
/*     Process_Physical_Func_Aperture                                       */
/****************************************************************************/
/* Local function to call the appropriate function to set PF Bar aperture
 * We call seperate functions for each BAR in order to reduce the
 * complexity of this function
 */
static UINT32
ProcessPhysicalFuncAperture(
    const PCIE_PrivateData *  pD,
    PCIE_BarNumber            barNumber,
    const PCIE_BarConfigRegisters * pConfigRegs,
    PCIE_ReadOrWrite          rdOrWr,
    UINT32 *                pApertureSize)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    switch (barNumber) {
    case PCIE_BAR_0: {
        CallFuncApertureBar0(pConfigRegs->config_reg_0, rdOrWr, pApertureSize); /* BAR 0 */
        break;
    }

    case PCIE_BAR_1: {
        CallFuncApertureBar1(pConfigRegs->config_reg_0, rdOrWr, pApertureSize); /* BAR 1 */
        break;
    }

    case PCIE_BAR_2: {
        CallFuncApertureBar2(pConfigRegs->config_reg_0, rdOrWr, pApertureSize); /* BAR 2 */
        break;
    }

    case PCIE_BAR_3: {
        CallFuncApertureBar3(pConfigRegs->config_reg_0, rdOrWr, pApertureSize); /* BAR 3 */
        break;
    }

    case PCIE_BAR_4: {
        CallFuncApertureBar4(pConfigRegs->config_reg_1, rdOrWr, pApertureSize); /* BAR 4 */
        break;
    }

    case PCIE_BAR_5: {
        CallFuncApertureBar5(pConfigRegs->config_reg_1, rdOrWr, pApertureSize); /* BAR 5 */
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

static PCIE_BarControl GetBarControl(UINT32 Index)
{
    PCIE_BarControl control;
    switch(Index) {
    case 0:
        control = PCIE_DISABLED_BAR;
        break;
    case 1:
        control = PCIE_IO_32_BIT_BAR;
        break;
    case 4:
        control = PCIE_NON_PREFETCHABLE_32_BIT_MEM_BAR;
        break;
    case 5:
        control = PCIE_PREFETCHABLE_32_BIT_MEM_BAR;
        break;
    case 6:
        control = PCIE_NON_PREFETCHABLE_64_BIT_MEM_BAR;
        break;
    case 7:
        control = PCIE_PREFETCHABLE_64_BIT_MEM_BAR;
        break;
    default:
        control = PCIE_DISABLED_BAR;
        break;
    }
    return control;
}

static UINT32 ConvertIntToBarControl(PCIE_BarControl * pBarControl, UINT32 barNumber)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    switch (barNumber) {
    case 0U:
    case 1U:
    case 4U:
    case 5U:
    case 6U:
    case 7U:
        /* It is a valid BAR control setting, read it from the array */
        *pBarControl = GetBarControl(barNumber);
        break;
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 CallPhysicalFuncControlBar0(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    PCIE_BarControl *   pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        result = ConvertIntToBarControl(pBarControl, PCIE_RegFldRead(LM_PF_CONFIG0_BAR0C_MASK, LM_PF_CONFIG0_BAR0C_SHIFT, regVal));
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR0C_MASK, LM_PF_CONFIG0_BAR0C_SHIFT, regVal, (UINT32)*pBarControl);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallPhysicalFuncControlBar1(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    PCIE_BarControl *   pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        result = ConvertIntToBarControl(pBarControl, PCIE_RegFldRead(LM_PF_CONFIG0_BAR1C_MASK, LM_PF_CONFIG0_BAR1C_SHIFT, regVal));
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR1C_MASK, LM_PF_CONFIG0_BAR1C_SHIFT, regVal, (UINT32)*pBarControl);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallPhysicalFuncControlBar2(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    PCIE_BarControl *   pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);
    if (rdOrWr == PCIE_DO_READ) {
        result = ConvertIntToBarControl(pBarControl, PCIE_RegFldRead(LM_PF_CONFIG0_BAR2C_MASK, LM_PF_CONFIG0_BAR2C_SHIFT, regVal));
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR2C_MASK, LM_PF_CONFIG0_BAR2C_SHIFT, regVal, (UINT32)*pBarControl);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallPhysicalFuncControlBar3(
    volatile UINT32 * reg_0,
    PCIE_ReadOrWrite    rdOrWr,
    PCIE_BarControl *   pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(reg_0);

    if (rdOrWr == PCIE_DO_READ) {
        result = ConvertIntToBarControl(pBarControl, PCIE_RegFldRead(LM_PF_CONFIG0_BAR3C_MASK, LM_PF_CONFIG0_BAR3C_SHIFT, regVal));
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG0_BAR3C_MASK, LM_PF_CONFIG0_BAR3C_SHIFT, regVal, (UINT32)*pBarControl);
        PCIE_RegPtrWrite32(reg_0, regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallPhysicalFuncControlBar4(
    volatile UINT32 * reg_1,
    PCIE_ReadOrWrite    rdOrWr,
    PCIE_BarControl *   pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(reg_1);

    if (rdOrWr == PCIE_DO_READ) {
        result = ConvertIntToBarControl(pBarControl, PCIE_RegFldRead(LM_PF_CONFIG1_BAR4C_MASK, LM_PF_CONFIG1_BAR4C_SHIFT, regVal));
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG1_BAR4C_MASK, LM_PF_CONFIG1_BAR4C_SHIFT, regVal, (UINT32)*pBarControl);
        PCIE_RegPtrWrite32(reg_1, regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 CallPhysicalFuncControlBar5(
    volatile UINT32 * reg_1,
    PCIE_ReadOrWrite    rdOrWr,
    PCIE_BarControl *   pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = PCIE_RegPtrRead32(reg_1);

    if (rdOrWr == PCIE_DO_READ) {
        result = ConvertIntToBarControl(pBarControl, PCIE_RegFldRead(LM_PF_CONFIG1_BAR5C_MASK, LM_PF_CONFIG1_BAR5C_SHIFT, regVal));
    } else {
        /* PCIE_DO_WRITE */
        regVal = PCIE_RegFldWrite(LM_PF_CONFIG1_BAR5C_MASK, LM_PF_CONFIG1_BAR5C_SHIFT, regVal, (UINT32)*pBarControl);
        PCIE_RegPtrWrite32(reg_1, regVal); /* Write back to register */
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/*     Process_Physical_Func_Control                                        */
/****************************************************************************/
/* Local function to call the appropriate function to set PF Bar control settings
 * We call seperate functions for each BAR in order to reduce the
 * complexity of this function
 */
static UINT32
ProcessPhysicalFuncControl(
    const PCIE_PrivateData *  pD,
    PCIE_BarNumber            barNumber,
    const PCIE_BarConfigRegisters * pConfigRegs,
    PCIE_ReadOrWrite          rdOrWr,
    PCIE_BarControl *         pBarControl)
{
    UINT32 result;

    switch (barNumber) {
    case PCIE_BAR_0:
        result = CallPhysicalFuncControlBar0(pConfigRegs->config_reg_0, rdOrWr, pBarControl); /* BAR 0 */
        break;

    case PCIE_BAR_1:
        result = CallPhysicalFuncControlBar1(pConfigRegs->config_reg_0, rdOrWr, pBarControl); /* BAR 1 */
        break;

    case PCIE_BAR_2:
        result = CallPhysicalFuncControlBar2(pConfigRegs->config_reg_0, rdOrWr, pBarControl); /* BAR 2 */
        break;

    case PCIE_BAR_3:
        result = CallPhysicalFuncControlBar3(pConfigRegs->config_reg_0, rdOrWr, pBarControl); /* BAR 3 */
        break;

    case PCIE_BAR_4:
        result = CallPhysicalFuncControlBar4(pConfigRegs->config_reg_1, rdOrWr, pBarControl); /* BAR 4 */
        break;

    case PCIE_BAR_5:
        result = CallPhysicalFuncControlBar5(pConfigRegs->config_reg_1, rdOrWr, pBarControl); /* BAR 5 */
        break;

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

/* Local function to vallidate the aperture size for BAR0,BAR2 or BAR4
 */
static UINT32 ValidateEvenBarWriteApertSize(

    const UINT32 * pApertureSize)
{
    UINT32 returnVal = PCIE_ERR_SUCCESS;
    UINT32 apertureSize = *pApertureSize;

    if ( (apertureSize < PCIE_APERTURE_SIZE_ELEMENTS) && ((UINT32)GetApertureSize(apertureSize) != 0U) ) {

        returnVal = 0U;
    } else {
        returnVal = PCIE_ERR_ARG;
    }

    return (returnVal);
}
/* Local function to vallidate the aperture size for BAR1,BAR3 or BAR5
 */
static UINT32 ValidateOddBarWriteApertSize(
    const UINT32 * pApertureSize)
{
    UINT32 result;
    if ((*pApertureSize >= PCIE_APERTURE_SIZE_ELEMENTS) || ((UINT32)GetApertureSize(*pApertureSize) == 0U)) {
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static void PCIE_GetPf0BarConfig(const PCIE_PrivateData * pD,
                                 UINT32                 funcNumber,
                                 PCIE_BarConfigRegisters *config)
{
    switch (funcNumber) {
    case 0:
        config->config_reg_0 = &(pD->p_lm_base->i_regf_lm_pcie_base.i_pf_0_BAR_config_0_reg);
        config->config_reg_1 = &(pD->p_lm_base->i_regf_lm_pcie_base.i_pf_0_BAR_config_1_reg);
        break;
    default:
        config->config_reg_0 = NULL;
        config->config_reg_1 = NULL;
        break;
    }
}

/*
 * Access_Function_BAR_Aperture_Setting
 *
 * Local function to process read or write of PF aperture size
 */
static UINT32 CallProcessingPhysicalAperture(
    UINT32                 funcNumber,
    const PCIE_PrivateData * pD,
    PCIE_BarNumber           barNumber,
    PCIE_ReadOrWrite         rdOrWr,
    UINT32 *               pApertureSize)
{
    PCIE_BarConfigRegisters config_regs;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (funcNumber < PCIE_NUM_PCI_FUNCTIONS) {
        PCIE_GetPf0BarConfig(pD, funcNumber, &config_regs);
        /* Read/write the aperture size if funcNumber was valid   */
        result = ProcessPhysicalFuncAperture( pD, barNumber, &config_regs, rdOrWr, pApertureSize);
    } else {
        result = PCIE_ERR_ARG;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 PCIE_WriteFuncBarApertureSet(
    const PCIE_PrivateData * pD,

    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    const PCIE_BarApertureSize *   pApertureSize)
{
    UINT32 apertureSize, returnVal = 0U;
    PCIE_ReadOrWrite rdOrWr = PCIE_DO_WRITE;

    apertureSize = (UINT32)*pApertureSize;

    /* if write, make sure that the aperture size being written is valid.  Minimum size is 4K */
    if (((UINT32)barNumber % 2U ) == 0U) {
        /* Validate input parameters */
        returnVal = ValidateEvenBarWriteApertSize(

                        &apertureSize);
    } else {
        /* Validate input parameters */
        returnVal = ValidateOddBarWriteApertSize(&apertureSize);
    }

    if (returnVal == PCIE_ERR_SUCCESS) {
        returnVal = CallProcessingPhysicalAperture(funcNumber, pD, barNumber, rdOrWr, &apertureSize);
    }

    return (returnVal);
}
/* Local function to process read or write of VF aperture size
 */
static UINT32 ReadFuncBarApertureSet(
    const PCIE_PrivateData * pD,

    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarApertureSize *   pApertureSize)
{
    UINT32 apertureSize, returnVal;
    PCIE_ReadOrWrite rdOrWr = PCIE_DO_READ;

    apertureSize = (UINT32)*pApertureSize;

    /* Call helper function to read the required PF aperture size from LM registers */
    returnVal = CallProcessingPhysicalAperture(funcNumber, pD, barNumber, rdOrWr, &apertureSize);
    if (returnVal == PCIE_ERR_SUCCESS) {

        *pApertureSize = GetApertureSize(apertureSize);
    }

    return (returnVal);
}
/* API function to read PF BAR apertures size
 */
UINT32 PCIE_GetFuncBarApertureSetting(
    const PCIE_PrivateData * pD,

    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarApertureSize *   apertureSize)
{
    UINT32 result;
    /*
     * Check input parameters are valid
     */
    if ((pD == NULL) ||

        (funcNumber > PCIE_NUM_PCI_FUNCTIONS) ||
        ((UINT32)barNumber > (UINT32)PCIE_BAR_5) ||
        (apertureSize == NULL)) {
        result = PCIE_ERR_ARG;
    } else {
        result = ReadFuncBarApertureSet(
                     pD,
                     funcNumber,
                     barNumber,
                     apertureSize);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* API function to write PF BAR aperture size
 */
UINT32 PCIE_SetFuncBarApertureSetting(
    const PCIE_PrivateData * pD,

    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarApertureSize     apertureSize)
{
    UINT32 result;
    /*
     * Check input parameters are valid
     */
    if ((pD == NULL) ||

        (funcNumber > PCIE_NUM_PCI_FUNCTIONS) ||
        ((UINT32)barNumber > (UINT32)PCIE_BAR_5) ||
        ((apertureSize > PCIE_APERTURE_SIZE_2G) && (((UINT32)barNumber % 2U) != 0U)) ||

        ((UINT32)apertureSize > (UINT32)PCIE_APERTURE_SIZE_256G)) {
        result = PCIE_ERR_ARG;
    } else {
        result = PCIE_WriteFuncBarApertureSet(
                     pD,
                     funcNumber,
                     barNumber,
                     &apertureSize);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to check input parameters
 */
static UINT32 CheckIfControlValueIsValid(
    const PCIE_BarNumber *  barNumber,
    const PCIE_BarControl * pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    switch (*barNumber) {
    case PCIE_BAR_0:
    case PCIE_BAR_2:
    case PCIE_BAR_4:
        /* Handle even numbered BAR
         * These can be 64 bit when combined with next BAR
         */
        switch (*pBarControl) {
        case PCIE_DISABLED_BAR:
        case PCIE_IO_32_BIT_BAR:
        case PCIE_NON_PREFETCHABLE_32_BIT_MEM_BAR:
        case PCIE_PREFETCHABLE_32_BIT_MEM_BAR:
        case PCIE_NON_PREFETCHABLE_64_BIT_MEM_BAR:
        case PCIE_PREFETCHABLE_64_BIT_MEM_BAR:
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        break;

    case PCIE_BAR_1:
    case PCIE_BAR_3:
    case PCIE_BAR_5:
        /* Handle odd numbered BAR
         * These can not be 64 bit
         */
        switch (*pBarControl) {
        case PCIE_DISABLED_BAR:
        case PCIE_IO_32_BIT_BAR:
        case PCIE_NON_PREFETCHABLE_32_BIT_MEM_BAR:
        case PCIE_PREFETCHABLE_32_BIT_MEM_BAR:
            break;
        case PCIE_NON_PREFETCHABLE_64_BIT_MEM_BAR:
        case PCIE_PREFETCHABLE_64_BIT_MEM_BAR:
        default:
            result = PCIE_ERR_ARG;
            break;
        }

        break;

    default:

        result = PCIE_ERR_ARG;
        break;
    }

    return result;
}
/* Local function to process read or write of PF BAR control
 */
static UINT32 CallProcessingPhysicalCtrl(
    UINT32                 funcNumber,
    const PCIE_PrivateData * pD,
    PCIE_BarNumber           barNumber,
    PCIE_ReadOrWrite         rdOrWr,
    PCIE_BarControl *        pBarControl)
{
    PCIE_BarConfigRegisters config_regs;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (funcNumber < PCIE_NUM_PCI_FUNCTIONS) {
        PCIE_GetPf0BarConfig(pD, funcNumber, &config_regs);
        /* Read/write the aperture size if funcNumber was valid   */
        result = ProcessPhysicalFuncControl(
                     pD,
                     barNumber,
                     &config_regs,
                     rdOrWr,
                     pBarControl);
    } else {
        result = PCIE_ERR_ARG;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* Access_Function_BAR_Control_Setting                                      */
/****************************************************************************/
static UINT32 ReadFuncBarCtrlSetting(
    const PCIE_PrivateData * pD,
    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarControl *        pBarControl)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    PCIE_ReadOrWrite rdOrWr = PCIE_DO_READ;
    /* Check input parameters are non NULL */
    if ( (pD == NULL) || (pBarControl == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {

        result = CallProcessingPhysicalCtrl(
                     funcNumber,
                     pD,
                     barNumber,
                     rdOrWr,
                     pBarControl);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static UINT32 PCIE_WriteFuncBarCtrlSetting(
    const PCIE_PrivateData * pD,
    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarControl *        pBarControl)
{
    UINT32 result = 0U;

    PCIE_ReadOrWrite rdOrWr = PCIE_DO_WRITE;
    /* Check input parameters are non NULL */
    if ( (pD == NULL) || (pBarControl == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {

        /* make sure that the control value being written is valid */
        result = CheckIfControlValueIsValid(&barNumber, pBarControl);

        if (result == PCIE_ERR_SUCCESS) {
            result = CallProcessingPhysicalCtrl(
                         funcNumber,
                         pD,
                         barNumber,
                         rdOrWr,
                         pBarControl);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* API function to write PF BAR control settings
 */
UINT32 PCIE_SetFuncBarControlSetting(
    const PCIE_PrivateData * pD,
    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarControl          barControl)
{
    return(PCIE_WriteFuncBarCtrlSetting(
               pD,
               funcNumber,
               barNumber,
               &barControl));

}
/* API function to read PF BAR control settings
 */
UINT32 PCIE_GetFuncBarControlSetting(
    const PCIE_PrivateData * pD,
    UINT32                 funcNumber,
    PCIE_BarNumber           barNumber,
    PCIE_BarControl *        barControl)
{
    return(ReadFuncBarCtrlSetting(
               pD,
               funcNumber,
               barNumber,
               barControl));
}

static PCIE_RpBarControl GetRpBarControl(UINT32 Index)
{
    PCIE_RpBarControl control;
    switch(Index) {
    case 0:
        control = PCIE_RP_DISABLED_BAR;
        break;
    case 1:
        control = PCIE_RP_32_BIT_IO_BAR;
        break;
    case 4:
        control = PCIE_RP_TYPE_0_32_BIT_MEM_BAR;
        break;
    case 5:
        control = PCIE_RP_TYPE_1_32_BIT_MEM_BAR;
        break;
    case 6:
        control = PCIE_RP_TYPE_0_64_BIT_MEM_BAR;
        break;
    case 7:
        control = PCIE_RP_TYPE_1_64_BIT_MEM_BAR;
        break;
    default:
        control = PCIE_RP_DISABLED_BAR;
        break;
    }
    return control;
}

static PCIE_RpBarApertureSize GetRpApertureSize(UINT32 Index)
{
    PCIE_RpBarApertureSize size;
    switch(Index) {
    case 10:
        size = PCIE_RP_APERTURE_SIZE_4K;
        break;
    case 11:
        size = PCIE_RP_APERTURE_SIZE_8K;
        break;
    case 12:
        size = PCIE_RP_APERTURE_SIZE_16K;
        break;
    case 13:
        size = PCIE_RP_APERTURE_SIZE_32K;
        break;
    case 14:
        size = PCIE_RP_APERTURE_SIZE_64K;
        break;
    case 15:
        size = PCIE_RP_APERTURE_SIZE_128K;
        break;
    case 16:
        size = PCIE_RP_APERTURE_SIZE_256K;
        break;
    case 17:
        size = PCIE_RP_APERTURE_SIZE_512K;
        break;
    case 18:
        size = PCIE_RP_APERTURE_SIZE_1M;
        break;
    case 19:
        size = PCIE_RP_APERTURE_SIZE_2M;
        break;
    case 20:
        size = PCIE_RP_APERTURE_SIZE_4M;
        break;
    case 21:
        size = PCIE_RP_APERTURE_SIZE_8M;
        break;
    case 22:
        size = PCIE_RP_APERTURE_SIZE_16M;
        break;
    case 23:
        size = PCIE_RP_APERTURE_SIZE_32M;
        break;
    case 24:
        size = PCIE_RP_APERTURE_SIZE_64M;
        break;
    case 25:
        size = PCIE_RP_APERTURE_SIZE_128M;
        break;
    case 26:
        size = PCIE_RP_APERTURE_SIZE_256M;
        break;
    case 27:
        size = PCIE_RP_APERTURE_SIZE_512M;
        break;
    case 28:
        size = PCIE_RP_APERTURE_SIZE_1G;
        break;
    case 29:
        size = PCIE_RP_APERTURE_SIZE_2G;
        break;
    case 30:
        size = PCIE_RP_APERTURE_SIZE_4G;
        break;
    case 31:
        size = PCIE_RP_APERTURE_SIZE_8G;
        break;
    case 32:
        size = PCIE_RP_APERTURE_SIZE_16G;
        break;
    case 33:
        size = PCIE_RP_APERTURE_SIZE_32G;
        break;
    case 34:
        size = PCIE_RP_APERTURE_SIZE_64G;
        break;
    case 35:
        size = PCIE_RP_APERTURE_SIZE_128G;
        break;
    case 36:
        size = PCIE_RP_APERTURE_SIZE_256G;
        break;
    default:
        size = PCIE_RP_APERTURE_SIZE_NA;
        break;
    }
    return size;
}

static UINT32 ModifyRcBarConfigRegister(
    UINT32 *                     regValAddr,
    PCIE_RpBarNumber               barNumber,
    const PCIE_RpBarApertureSize * pApertureSize)
{
    UINT32 result = PCIE_ERR_SUCCESS;

    /* Write */
    switch (barNumber) {
    case PCIE_RP_BAR_0:

        *regValAddr = PCIE_RegFldWrite(LM_RC_CONFIG_RCBAR0A_MASK, LM_RC_CONFIG_RCBAR0A_SHIFT, *regValAddr, (UINT32)*pApertureSize);
        break;

    case PCIE_RP_BAR_1:

        *regValAddr = PCIE_RegFldWrite(LM_RC_CONFIG_RCBAR1A_MASK, LM_RC_CONFIG_RCBAR1A_SHIFT, *regValAddr, (UINT32)*pApertureSize);
        break;

    default:
        result = PCIE_ERR_ARG;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
static UINT32 CallFuncRpBarApertureSize(
    UINT32                 regVal,
    PCIE_RpBarNumber         barNumber,
    PCIE_RpBarApertureSize * pApertureSize)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 RpBarApertureSizeArrayIndex = 0U;
    switch (barNumber) {
    case PCIE_RP_BAR_0:
        RpBarApertureSizeArrayIndex = PCIE_RegFldRead(LM_RC_CONFIG_RCBAR0A_MASK, LM_RC_CONFIG_RCBAR0A_SHIFT, regVal);
        if (RpBarApertureSizeArrayIndex < PCIE_RP_APERTURE_SIZE_ELEMENTS) {
            *pApertureSize = GetRpApertureSize(RpBarApertureSizeArrayIndex);
        } else {
            result = PCIE_ERR_ARG; /* Invalid aperture size read from IP */
        }
        break;

    case PCIE_RP_BAR_1:
        RpBarApertureSizeArrayIndex = PCIE_RegFldRead(LM_RC_CONFIG_RCBAR1A_MASK, LM_RC_CONFIG_RCBAR1A_SHIFT, regVal);
        if (RpBarApertureSizeArrayIndex < PCIE_RP_APERTURE_SIZE_ELEMENTS) {
            *pApertureSize = GetRpApertureSize(RpBarApertureSizeArrayIndex);
        } else {
            result = PCIE_ERR_ARG; /* Invalid aperture size read from IP */
        }
        break;

    default:
        result = PCIE_ERR_ARG;
        break;
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Access_Root_Port_BAR_Aperture_Setting                                    */
/****************************************************************************/
static UINT32 ValidSetRootBarApertSetting(
    const PCIE_PrivateData * pD,
    PCIE_RpBarApertureSize   apertureSize)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;
    /* Validate input parameters
     * Return PCIE_ERR_ARG or 0 */
    if ((UINT32)apertureSize > (UINT32)PCIE_RP_APERTURE_SIZE_256G) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        result = 0U;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
UINT32 PCIE_SetRootPortBarApertSetting(
    const PCIE_PrivateData * pD,
    PCIE_RpBarNumber         barNumber,
    PCIE_RpBarApertureSize   apertureSize)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( ValidSetRootBarApertSetting(pD, apertureSize) != 0U ) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        volatile UINT32 *bar_config_reg_addr = &(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg);
        regVal = PCIE_RegPtrRead32(bar_config_reg_addr);

        /* can we support this aperture size for this BAR */
        if ((UINT32)GetRpApertureSize((UINT32)apertureSize) == 0U) {
            result = PCIE_ERR_ARG;
        } else if ( (apertureSize > PCIE_RP_APERTURE_SIZE_2G) && (((UINT32)barNumber % 2U) != 0U) ) {
            result = PCIE_ERR_ARG;
        } else {
            result = ModifyRcBarConfigRegister(&regVal, barNumber, &apertureSize);

            if (result == PCIE_ERR_SUCCESS) {
                PCIE_RegPtrWrite32(bar_config_reg_addr, regVal); /* Write back to register */
                /* Check for errors reported from PCIe IP */
                result = CallErrorCallbackIfError(pD);
            }
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_GetRootPortBarApertSetting(
    const PCIE_PrivateData * pD,
    PCIE_RpBarNumber         barNumber,
    PCIE_RpBarApertureSize * apertureSize)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (apertureSize == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        const volatile UINT32 *bar_config_reg_addr = &(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg);
        regVal = PCIE_RegPtrRead32(bar_config_reg_addr);
        result = CallFuncRpBarApertureSize(regVal, barNumber, apertureSize);
        if (result == PCIE_ERR_SUCCESS) {
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/*
 * PCIE_Get_Root_Port_BAR_Control_Setting
 * API function read the current RP BAR control settings
 */
UINT32 PCIE_GetRootPortBarCtrlSetting(
    const PCIE_PrivateData * pD,
    PCIE_RpBarNumber         barNumber,
    PCIE_RpBarControl *      rpBarControl)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 RpBarControlArrayIndex = 0U;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (rpBarControl == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg));

        switch (barNumber) {
        case PCIE_RP_BAR_0:
            RpBarControlArrayIndex = PCIE_RegFldRead(LM_RC_CONFIG_RCBAR0C_MASK, LM_RC_CONFIG_RCBAR0C_SHIFT, regVal);
            *rpBarControl = GetRpBarControl(RpBarControlArrayIndex); /* Map integer value to enum */
            break;

        case PCIE_RP_BAR_1:
            RpBarControlArrayIndex = PCIE_RegFldRead(LM_RC_CONFIG_RCBAR1C_MASK, LM_RC_CONFIG_RCBAR1C_SHIFT, regVal);
            *rpBarControl = GetRpBarControl(RpBarControlArrayIndex); /* Map integer value to enum */
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
/* PCIE_Set_Root_Port_BAR_Control_Setting                                   */
/****************************************************************************/
UINT32 PCIE_SetRootPortBarCtrlSetting(
    const PCIE_PrivateData * pD,
    PCIE_RpBarNumber         barNumber,
    PCIE_RpBarControl        rpBarControl)
{
    struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if ((UINT32)rpBarControl >= (UINT32)PCIE_RP_BAR_CTRL_ELEMENTS) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (GetRpBarControl((UINT32)rpBarControl) != rpBarControl) {
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg));

        switch (barNumber) {
        case PCIE_RP_BAR_0:
            regVal = PCIE_RegFldWrite(LM_RC_CONFIG_RCBAR0C_MASK, LM_RC_CONFIG_RCBAR0C_SHIFT, regVal, (UINT32)rpBarControl);
            break;

        case PCIE_RP_BAR_1:
            if ( (GetRpBarControl((UINT32)rpBarControl) == PCIE_RP_TYPE_0_64_BIT_MEM_BAR) ||
                 (GetRpBarControl((UINT32)rpBarControl) == PCIE_RP_TYPE_1_64_BIT_MEM_BAR) ) {
                result = PCIE_ERR_ARG;
            } else {
                regVal = PCIE_RegFldWrite(LM_RC_CONFIG_RCBAR1C_MASK, LM_RC_CONFIG_RCBAR1C_SHIFT, regVal, (UINT32)rpBarControl);
            }
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1DisAll(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPME_WIDTH, LM_RC_CONFIG_RCBARPME_MASK,
                                LM_RC_CONFIG_RCBARPME_WOSET, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPMS_WIDTH, LM_RC_CONFIG_RCBARPMS_MASK,
                                LM_RC_CONFIG_RCBARPMS_WOSET, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIE_WIDTH, LM_RC_CONFIG_RCBARPIE_MASK,
                                LM_RC_CONFIG_RCBARPIE_WOSET, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIS_WIDTH, LM_RC_CONFIG_RCBARPIS_MASK,
                                LM_RC_CONFIG_RCBARPIS_WOSET, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetch32BitIODis(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPMS_WIDTH, LM_RC_CONFIG_RCBARPMS_MASK,
                                LM_RC_CONFIG_RCBARPMS_WOSET, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIE_WIDTH, LM_RC_CONFIG_RCBARPIE_MASK,
                                LM_RC_CONFIG_RCBARPIE_WOSET, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIS_WIDTH, LM_RC_CONFIG_RCBARPIS_MASK,
                                LM_RC_CONFIG_RCBARPIS_WOSET, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetch64BitIODis(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPMS_MASK, LM_RC_CONFIG_RCBARPMS_SHIFT, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIE_WIDTH, LM_RC_CONFIG_RCBARPIE_MASK,
                                LM_RC_CONFIG_RCBARPIE_WOSET, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIS_WIDTH, LM_RC_CONFIG_RCBARPIS_MASK,
                                LM_RC_CONFIG_RCBARPIS_WOSET, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function for RP Type1 Configuration setting.
 */
static UINT32 ModifyRpType1CfgRegsFirstGroup(
    UINT32 *                        pRegVal,
    const PCIE_RpType1ConfigControl * pRpType1Config)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = *pRegVal;

    /* Only some of the possible cases are treated here,
     * otherwise we return PCIE_ERR_ARG and we will try the next group
     * This is to reduce complexity of the switch statement
     */
    switch (*pRpType1Config) {
    case PCIE_RP_T1_DIS_ALL:
        *pRegVal = ModifyRpT1DisAll(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_32_BIT_IO_DIS:
        *pRegVal = ModifyRpT1PFetch32BitIODis(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_64_BIT_IO_DIS:
        *pRegVal = ModifyRpT1PFetch64BitIODis(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_DIS_IO_16_BIT:
    case PCIE_RP_T1_PFETCH_32_BIT_IO_16_BIT:
    case PCIE_RP_T1_PFETCH_64_BIT_IO_16_BIT:
    case PCIE_RP_T1_PFETCH_DIS_IO_32_BIT:
    case PCIE_RP_T1_PFETCH_32_BIT_IO_32_BIT:
    case PCIE_RP_T1_PFETCH_64_BIT_IO_32_BIT:
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetchDisIO16Bit(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPME_WIDTH, LM_RC_CONFIG_RCBARPME_MASK,
                                LM_RC_CONFIG_RCBARPME_WOSET, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPMS_WIDTH, LM_RC_CONFIG_RCBARPMS_MASK,
                                LM_RC_CONFIG_RCBARPMS_WOSET, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIS_WIDTH, LM_RC_CONFIG_RCBARPIS_MASK,
                                LM_RC_CONFIG_RCBARPIS_WOSET, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetchDisIO32Bit(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPME_WIDTH, LM_RC_CONFIG_RCBARPME_MASK,
                                LM_RC_CONFIG_RCBARPME_WOSET, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPMS_WIDTH, LM_RC_CONFIG_RCBARPMS_MASK,
                                LM_RC_CONFIG_RCBARPMS_WOSET, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIS_MASK, LM_RC_CONFIG_RCBARPIS_SHIFT, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetch32BitIO16Bit(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPMS_WIDTH, LM_RC_CONFIG_RCBARPMS_MASK,
                                LM_RC_CONFIG_RCBARPMS_WOSET, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIS_WIDTH, LM_RC_CONFIG_RCBARPIS_MASK,
                                LM_RC_CONFIG_RCBARPIS_WOSET, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function for RP Type1 Configuration setting.
 */
static UINT32 ModifyRpType1CfgRegsSecondGroup(
    UINT32 *                        pRegVal,
    const PCIE_RpType1ConfigControl * pRpType1Config)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = *pRegVal;

    /* Only some of the possible cases are treated here,
     * otherwise we return PCIE_ERR_ARG and we will try the next group
     * This is to reduce complexity of the switch statement
     */
    switch (*pRpType1Config) {
    case PCIE_RP_T1_PFETCH_DIS_IO_16_BIT:
        *pRegVal = ModifyRpT1PFetchDisIO16Bit(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_DIS_IO_32_BIT:
        *pRegVal = ModifyRpT1PFetchDisIO32Bit(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_32_BIT_IO_16_BIT:
        *pRegVal = ModifyRpT1PFetch32BitIO16Bit(&regVal);
        break;
    /** Disabled */
    case PCIE_RP_T1_DIS_ALL:
    case PCIE_RP_T1_PFETCH_32_BIT_IO_DIS:
    case PCIE_RP_T1_PFETCH_64_BIT_IO_DIS:
    case PCIE_RP_T1_PFETCH_64_BIT_IO_16_BIT:
    case PCIE_RP_T1_PFETCH_32_BIT_IO_32_BIT:
    case PCIE_RP_T1_PFETCH_64_BIT_IO_32_BIT:
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetch32BitIO32Bit(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPMS_WIDTH, LM_RC_CONFIG_RCBARPMS_MASK,
                                LM_RC_CONFIG_RCBARPMS_WOSET, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIS_MASK, LM_RC_CONFIG_RCBARPIS_SHIFT, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetch64BitIO16Bit(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPMS_MASK, LM_RC_CONFIG_RCBARPMS_SHIFT, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBARPIS_WIDTH, LM_RC_CONFIG_RCBARPIS_MASK,
                                LM_RC_CONFIG_RCBARPIS_WOSET, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function to set RP BAR config register
 */
static UINT32 ModifyRpT1PFetch64BitIO32Bit(UINT32 * pRegVal)
{
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, LM_RC_CONFIG_RCBARPME_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPMS_MASK, LM_RC_CONFIG_RCBARPMS_SHIFT, LM_RC_CONFIG_RCBARPMS_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, LM_RC_CONFIG_RCBARPIE_WOCLR, *pRegVal);
    *pRegVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBARPIS_MASK, LM_RC_CONFIG_RCBARPIS_SHIFT, LM_RC_CONFIG_RCBARPIS_WOCLR, *pRegVal);

    /* pRegVal is never NULL */
    return (*pRegVal);
}
/* Local function for RP Type1 Configuration setting.
 */
static UINT32 ModifyRpType1CfgRegsThirdGroup(
    UINT32 *                        pRegVal,
    const PCIE_RpType1ConfigControl * pRpType1Config)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 regVal = *pRegVal;

    /* Only some of the possible cases are treated here,
     * otherwise we return PCIE_ERR_ARG and we will try the next group
     * This is to reduce complexity of the switch statement
     */
    switch (*pRpType1Config) {
    case PCIE_RP_T1_PFETCH_32_BIT_IO_32_BIT:
        *pRegVal = ModifyRpT1PFetch32BitIO32Bit(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_64_BIT_IO_16_BIT:
        *pRegVal = ModifyRpT1PFetch64BitIO16Bit(&regVal);
        break;
    case PCIE_RP_T1_PFETCH_64_BIT_IO_32_BIT:
        *pRegVal = ModifyRpT1PFetch64BitIO32Bit(&regVal);
        break;
    case PCIE_RP_T1_DIS_ALL:
    case PCIE_RP_T1_PFETCH_32_BIT_IO_DIS:
    case PCIE_RP_T1_PFETCH_64_BIT_IO_DIS:
    case PCIE_RP_T1_PFETCH_DIS_IO_16_BIT:
    case PCIE_RP_T1_PFETCH_32_BIT_IO_16_BIT:
    case PCIE_RP_T1_PFETCH_DIS_IO_32_BIT:
    default:
        result = PCIE_ERR_ARG;
        break;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Access_Root_Port_Type1_Config_Setting                                    */
/****************************************************************************/
UINT32 PCIE_GetRootPortType1ConfSet(
    const PCIE_PrivateData *    pD,
    PCIE_RpType1ConfigControl * rpType1Config)
{
    const struct PCIE_IClientLm_s *pcieAddr;
    UINT32 regVal;
    UINT32 rpConfig;
    UINT32 result = PCIE_ERR_SUCCESS;
    /* static const should not require memory allocation */
    static const PCIE_RpType1ConfigControl rpType1ConfControlArray[PCIE_TYPE1_CONFIG_CONTROL_SIZE] = {
        [0x00] = PCIE_RP_T1_DIS_ALL,
        [0x01] = PCIE_RP_T1_PFETCH_32_BIT_IO_DIS,
        [0x02] = PCIE_RP_T1_DIS_ALL,
        [0x03] = PCIE_RP_T1_PFETCH_64_BIT_IO_DIS,
        [0x04] = PCIE_RP_T1_PFETCH_DIS_IO_16_BIT,
        [0x05] = PCIE_RP_T1_PFETCH_32_BIT_IO_16_BIT,
        [0x06] = PCIE_RP_T1_PFETCH_DIS_IO_16_BIT,
        [0x07] = PCIE_RP_T1_PFETCH_64_BIT_IO_16_BIT,
        [0x08] = PCIE_RP_T1_DIS_ALL,
        [0x09] = PCIE_RP_T1_PFETCH_32_BIT_IO_DIS,
        [0x0a] = PCIE_RP_T1_DIS_ALL,
        [0x0b] = PCIE_RP_T1_PFETCH_64_BIT_IO_DIS,
        [0x0c] = PCIE_RP_T1_PFETCH_DIS_IO_32_BIT,
        [0x0d] = PCIE_RP_T1_PFETCH_32_BIT_IO_32_BIT,
        [0x0e] = PCIE_RP_T1_PFETCH_DIS_IO_32_BIT,
        [0x0f] = PCIE_RP_T1_PFETCH_64_BIT_IO_32_BIT
    };
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (rpType1Config == NULL) {
        result = PCIE_ERR_ARG;
    } else if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else {
        /* Read register from PCIe IP */
        pcieAddr = pD->p_lm_base;
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg));

        rpConfig =   PCIE_RegFldRead(LM_RC_CONFIG_RCBARPME_MASK, LM_RC_CONFIG_RCBARPME_SHIFT, regVal);
        rpConfig += (PCIE_RegFldRead(LM_RC_CONFIG_RCBARPMS_MASK, LM_RC_CONFIG_RCBARPMS_SHIFT, regVal) << 1U);
        rpConfig += (PCIE_RegFldRead(LM_RC_CONFIG_RCBARPIE_MASK, LM_RC_CONFIG_RCBARPIE_SHIFT, regVal) << 2U);
        rpConfig += (PCIE_RegFldRead(LM_RC_CONFIG_RCBARPIS_MASK, LM_RC_CONFIG_RCBARPIS_SHIFT, regVal) << 3U);

        *rpType1Config = rpType1ConfControlArray[rpConfig];
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

UINT32 PCIE_SetRootPortType1ConfSet(
    const PCIE_PrivateData *  pD,
    PCIE_RpType1ConfigControl rpType1Config)
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
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg));

        /* These are split into groups, to avoid having too many cases in a switch
         * statement.
         */
        result = ModifyRpType1CfgRegsFirstGroup(&regVal, &rpType1Config);
        if (result == PCIE_ERR_ARG) {
            result = ModifyRpType1CfgRegsSecondGroup(&regVal, &rpType1Config);
            if (result == PCIE_ERR_ARG) {
                result = ModifyRpType1CfgRegsThirdGroup(&regVal, &rpType1Config);
            }
        }
        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }

    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* PCIE_Control_Root_Port_BAR_Check                                         */
/****************************************************************************/
UINT32 PCIE_ControlRootPortBarCheck(
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
        regVal = PCIE_RegPtrRead32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg));

        switch (enableOrDisable) {
        case PCIE_ENABLE_PARAM:

            regVal = PCIE_RegFldSet(LM_RC_CONFIG_RCBCE_MASK, LM_RC_CONFIG_RCBCE_SHIFT, LM_RC_CONFIG_RCBCE_WOCLR, regVal);
            break;

        case PCIE_DISABLE_PARAM:

            regVal = PCIE_RegFldClear(LM_RC_CONFIG_RCBCE_WIDTH, LM_RC_CONFIG_RCBCE_MASK,
                                      LM_RC_CONFIG_RCBCE_WOSET, LM_RC_CONFIG_RCBCE_WOCLR, regVal);
            break;

        default:
            result = PCIE_ERR_ARG;
            break;
        }

        if (result == PCIE_ERR_SUCCESS) {
            PCIE_RegPtrWrite32(&(pcieAddr->i_regf_lm_pcie_base.i_rc_BAR_config_reg), regVal); /* Write back to register */
            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/* Function to initialize RP BAR aperture and control settings  */
UINT32 CallAccessRootPortBarFuncs(
    const PCIE_InitParam *   pInitParam,
    UINT32                 retVal,
    const PCIE_PrivateData * pD)
{
    /* static const should not require memory allocation */
    static const PCIE_RpBarNumber RpBarNumberArray[PCIE_RP_BAR_NUM_ELEMENTS] = {
        [PCIE_RP_BAR_0] = PCIE_RP_BAR_0,
        [PCIE_RP_BAR_1] = PCIE_RP_BAR_1

    };

    UINT32 rpBarNum;
    UINT32 result = retVal;
    for (rpBarNum = 0U; rpBarNum < (UINT32)PCIE_MAX_BARS_IN_RP; rpBarNum++) {
        if ( (pInitParam->initRpBarParams.rpBarAperture[rpBarNum].changeDefault != 0U) &&
             (result == PCIE_ERR_SUCCESS) ) {
            /* Initialize BAR aperture settings */
            UINT32 RpBarApertureSizeArrayIndex = pInitParam->initRpBarParams.rpBarAperture[rpBarNum].newValue;
            result = PCIE_SetRootPortBarApertSetting(
                         pD,
                         RpBarNumberArray[rpBarNum],
                         GetRpApertureSize(RpBarApertureSizeArrayIndex));
        }

        if ( (pInitParam->initRpBarParams.rpBarControl[rpBarNum].changeDefault != 0U) &&
             (result == PCIE_ERR_SUCCESS) ) {
            /* Initialize BAR control settings */
            UINT32 RpBarControlArrayIndex = pInitParam->initRpBarParams.rpBarControl[rpBarNum].newValue;
            PCIE_RpBarControl val = GetRpBarControl(RpBarControlArrayIndex);
            result = PCIE_SetRootPortBarCtrlSetting(
                         pD,
                         RpBarNumberArray[rpBarNum],
                         val);
        }
    }
    return (result);
}

/* Function to initialize Function BAR aperture and control settings */
UINT32 CallAccessPortBarFuncs(
    const PCIE_InitParam *   pInitParam,
    UINT32                 retVal,
    const PCIE_PrivateData * pD)
{
    /* static const should not require memory allocation */
    static const PCIE_BarNumber BarNumberArray[PCIE_BAR_NUM_ELEMENTS] = {
        [PCIE_BAR_0] = PCIE_BAR_0,
        [PCIE_BAR_1] = PCIE_BAR_1,
        [PCIE_BAR_2] = PCIE_BAR_2,
        [PCIE_BAR_3] = PCIE_BAR_3,
        [PCIE_BAR_4] = PCIE_BAR_4,
        [PCIE_BAR_5] = PCIE_BAR_5
    };

    UINT32 result = retVal;
    UINT32 funcNum, barNum;

    for (funcNum = 0U; funcNum < 1U; funcNum++) {
        for (barNum = 0U; barNum < (UINT32)PCIE_MAX_BARS_IN_FUNC; barNum++) {
            if ( (pInitParam->initBarParams[funcNum].funcBarAperture[barNum].changeDefault != 0U) &&
                 (result == PCIE_ERR_SUCCESS) ) {
                /* Initialize BAR aperture settings */
                UINT32 BarApertureSizeArrayIndex = pInitParam->initBarParams[funcNum].funcBarAperture[barNum].newValue;
                result = PCIE_SetFuncBarApertureSetting(
                             pD,
                             funcNum,
                             BarNumberArray[barNum],
                             GetApertureSize(BarApertureSizeArrayIndex));
            }

            if ( (pInitParam->initBarParams[funcNum].funcBarControl[barNum].changeDefault != 0U) &&
                 (result == PCIE_ERR_SUCCESS) ) {
                /* Initialize BAR control settings */
                UINT32 BarControlArrayIndex = pInitParam->initBarParams[funcNum].funcBarControl[barNum].newValue;
                result = PCIE_SetFuncBarControlSetting(
                             pD,
                             funcNum,
                             BarNumberArray[barNum],
                             GetBarControl(BarControlArrayIndex));
            }
        }
    }
    return (result);
}

UINT32 CallCtrlRootPortBarCheck(
    const PCIE_PrivateData * pD,
    UINT32                 retVal,
    const PCIE_InitParam *   pInitParam)
{
    UINT32 result = retVal;

    /*
     * If a previous call failed, or changeDefault is 0, do nothing
     */
    if ( (pInitParam->initRpBarParams.enableRpBarCheck.changeDefault != 0U) &&
         (result == PCIE_ERR_SUCCESS) ) {

        PCIE_EnableOrDisable val = PCIE_DISABLE_PARAM;
        if (pInitParam->initRpBarParams.enableRpBarCheck.newValue == 0U) {
            val = PCIE_DISABLE_PARAM;
        } else {
            /* Value cannot be other than ENABLE/DISABLE */
            val = PCIE_ENABLE_PARAM;
        }
        result = PCIE_ControlRootPortBarCheck(
                     pD,
                     val);
    }
    return (result);
}
