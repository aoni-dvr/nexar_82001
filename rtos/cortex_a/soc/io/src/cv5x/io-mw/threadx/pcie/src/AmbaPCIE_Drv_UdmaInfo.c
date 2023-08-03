/**
 *  @file AmbaPCIE_Drv_UdmaInfo.c
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
 *  @details PCIe UDMA information functions.
 */

#include "AmbaPCIE_Drv.h"

#define PCIE_BOOL_ELEMENTS 2U

/****************************************************************************/
/* UDMA get verion info for the udma core                                   */
/****************************************************************************/
UINT32
PCIE_UDMA_GetVersion(
    const PCIE_PrivateData * pD,
    UINT8 *                maj_ver,
    UINT8 *                min_ver)
{
    const struct PCIE_IClientUdma_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) ||
         (maj_ver == NULL) ||
         (min_ver == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ud_base;

        /** read the version register */
        regVal = PCIE_RegPtrRead32(&(pcieAddr->dma_common.common_udma_cap_ver));

        *maj_ver = (UINT8)PCIE_RegFldRead(DMA_CAP_VER_MAJ_MASK, DMA_CAP_VER_MAJ_SHIFT, regVal);
        *min_ver = (UINT8)PCIE_RegFldRead(DMA_CAP_VER_MIN_MASK, DMA_CAP_VER_MIN_SHIFT, regVal);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* UDMA get capability info for the udma core                               */
/****************************************************************************/
UINT32
PCIE_UDMA_GetConfiguration(
    const PCIE_PrivateData * pD,
    PCIE_UdmaConfiguration * pConf)
{
    const struct PCIE_IClientUdma_s *pcieAddr;
    UINT32 regVal;
    UINT32 result = PCIE_ERR_SUCCESS;

    /* static const should not require memory allocation */
    static const PCIE_Bool WideSysAddressArray[PCIE_BOOL_ELEMENTS] = {
        [PCIE_FALSE] = PCIE_FALSE,
        [PCIE_TRUE]  = PCIE_TRUE
    };

    /* static const should not require memory allocation */
    static const PCIE_Bool WideSysAttrArray[PCIE_BOOL_ELEMENTS] = {
        [PCIE_FALSE] = PCIE_FALSE,
        [PCIE_TRUE]  = PCIE_TRUE
    };

    /* static const should not require memory allocation */
    static const PCIE_Bool WideExtAddressArray[PCIE_BOOL_ELEMENTS] = {
        [PCIE_FALSE] = PCIE_FALSE,
        [PCIE_TRUE]  = PCIE_TRUE
    };

    /* static const should not require memory allocation */
    static const PCIE_Bool WideExtAttrArray[PCIE_BOOL_ELEMENTS] = {
        [PCIE_FALSE] = PCIE_FALSE,
        [PCIE_TRUE]  = PCIE_TRUE
    };
    if ( (pD == NULL) || (pConf == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ud_base;

        /** read the capability register */
        regVal = PCIE_RegPtrRead32(&pcieAddr->dma_common.common_udma_config);

        pConf->numChannels   = PCIE_RegFldRead(DMA_CONFIG_NUM_CHANNELS_MASK, DMA_CONFIG_NUM_CHANNELS_SHIFT, regVal);
        pConf->numPartitions = PCIE_RegFldRead(DMA_CONFIG_NUM_PARTITIONS_MASK, DMA_CONFIG_NUM_PARTITIONS_SHIFT, regVal);
        pConf->partitionSize = PCIE_RegFldRead(DMA_CONFIG_PARTITION_SIZE_MASK, DMA_CONFIG_PARTITION_SIZE_SHIFT,regVal);

        pConf->wideSysAddr = WideSysAddressArray[PCIE_RegFldRead(DMA_CONFIG_SYS_AW_GT_32_MASK, DMA_CONFIG_SYS_AW_GT_32_SHIFT, regVal)];

        pConf->wideSysAttr = WideSysAttrArray[PCIE_RegFldRead(DMA_CONFIG_SYS_TW_GT_32_MASK, DMA_CONFIG_SYS_TW_GT_32_SHIFT, regVal)];

        pConf->wideExtAddr = WideExtAddressArray[PCIE_RegFldRead(DMA_CONFIG_EXT_AW_GT_32_MASK, DMA_CONFIG_EXT_AW_GT_32_SHIFT,regVal)];

        pConf->wideExtAttr = WideExtAttrArray[PCIE_RegFldRead(DMA_CONFIG_EXT_TW_GT_32_MASK, DMA_CONFIG_EXT_TW_GT_32_SHIFT, regVal)];

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Get buffer error count                                                   */
/****************************************************************************/
UINT32
PCIE_UDMA_GetBufferErrorCount(
    const PCIE_PrivateData * pD,
    PCIE_UdmaBufferErrorType errorType,
    UINT32 *               pErrorCount)
{
    const struct PCIE_IClientUdma_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) ||
         (pErrorCount == NULL) ) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = pD->p_ud_base;

        switch (errorType) {
        case PCIE_UNCORRECTED_ECC_IB:
            *pErrorCount = PCIE_RegPtrRead32(&(pcieAddr->dma_common.common_udma_ib_ecc_uncorrectable_errors));
            break;
        case PCIE_CORRECTED_ECC_IB:
            *pErrorCount = PCIE_RegPtrRead32(&(pcieAddr->dma_common.common_udma_ib_ecc_correctable_errors));
            break;
        case PCIE_UNCORRECTED_ECC_OB:
            *pErrorCount = PCIE_RegPtrRead32(&(pcieAddr->dma_common.common_udma_ob_ecc_uncorrectable_errors));
            break;
        case PCIE_CORRECTED_ECC_OB:
            *pErrorCount = PCIE_RegPtrRead32(&(pcieAddr->dma_common.common_udma_ob_ecc_correctable_errors));
            break;
        default:
            result = PCIE_ERR_ARG;
            break;
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
