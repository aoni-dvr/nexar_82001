/**
 *  @file AmbaPCIE_Drv_AxiConfig.c
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
 *  @details PCIe implementation of driver API functions (axi config).
 */
#include "AmbaPCIE_Drv.h"

#define AXI_WRAPPER_TYPE0_WRITE  0xAU
#define RID_VALID                0x800000U
#define MAX_AXI_IB_ROOTPORT_REGION_NUM   2
#define AXI_WRAPPER_MSG_WRITE    0xCU
#define AXI_WRAPPER_MEM_WRITE    0x2U

/****************************************************************************/
/* Clear AXI link down indicator bit                                        */
/****************************************************************************/
UINT32
PCIE_ClearLinkDownIndicator(const PCIE_PrivateData * pD)
{
    struct PCIE_IClientAtu_s *pcieAddr;
    UINT32 result = PCIE_ERR_SUCCESS;

    if (pD == NULL) {
        result = PCIE_ERR_ARG;
    } else {
        pcieAddr = (struct PCIE_IClientAtu_s *) pD->p_aw_base;
        /** clear the register */
        PCIE_RegPtrWrite32(&(pcieAddr->link_down_indicator_bit.L0),0 );

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* AXI Wrapper Functions                                                    */
/****************************************************************************/
#if 0
static volatile UINT32 * GetWrapperObAddr(
    const PCIE_PrivateData * pD,
    UINT8                  regionNo)
{
    UINT8 region = regionNo;
    volatile UINT32 * aw_base = &pD->p_aw_base->wrapper_ob_0.addr0;

    /* Set aw_base to the address of first element of next wrapper */
    return &aw_base[region * 8U];

}
#endif
static AxiWrapperOb * GetWrapperObAddr(
    const PCIE_PrivateData * pD,
    UINT8                  regionNo)
{
    UINT8 region = regionNo;
    AxiWrapperOb * aw_base = &pD->p_aw_base->ob0;
    switch (region) {
    case 0:
        aw_base = &pD->p_aw_base->ob0;
        break;
    case 1:
        aw_base = &pD->p_aw_base->ob1;
        break;
    case 2:
        aw_base = &pD->p_aw_base->ob2;
        break;
    case 3:
        aw_base = &pD->p_aw_base->ob3;
        break;
    case 4:
        aw_base = &pD->p_aw_base->ob4;
        break;
    case 5:
        aw_base = &pD->p_aw_base->ob5;
        break;
    case 6:
        aw_base = &pD->p_aw_base->ob6;
        break;
    case 7:
        aw_base = &pD->p_aw_base->ob7;
        break;
    default:
        /* should not be here */
        break;
    }


    /* Set aw_base to the address of first element of next wrapper */
    return aw_base;

}
/****************************************************************************/
/* Setup Traffic class for AXI Wrapper regions                              */
/****************************************************************************/
UINT32
PCIE_UpdObWrapperTrafficClass(const PCIE_PrivateData * pD,
                              UINT8                  regionNo,
                              UINT8                  trafficClass)
{
    UINT32 ob_desc_0 = 0U;
    AxiWrapperOb * ob_addr;
    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) || (regionNo >= (UINT8)PCIE_MAX_AXI_WRAPPER_REGION_NUM) ||
         (trafficClass >= (UINT8)PCIE_MAX_TRAFFIC_CLASS_NUM) ) {
        result = PCIE_ERR_ARG;
    } else {

        ob_addr = GetWrapperObAddr(pD, regionNo);
        /** Read the existing value of descriptor 0 */
        ob_desc_0 = PCIE_RegPtrRead32(&ob_addr->desc0);
        /* write ob-desc0.  Bits [19:17] define the PCIe traffic class */
        ob_desc_0 = AxiWrapObRegTrafficClassDataMod(ob_desc_0, trafficClass);

        /** write the register
         * descriptor 0 is 3rd element of ob wrapper */
        PCIE_RegPtrWrite32(&ob_addr->desc0, ob_desc_0);
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* AXI Wrapper Setup for MemIO region                                       */
/****************************************************************************/
static UINT32 ValidInputSetupObWrapper(const PCIE_PrivateData * pD,
                                       const UINT8            regionNo)
{
    UINT32 result = PCIE_ERR_SUCCESS;
    if ( (pD == NULL) || (regionNo >= (UINT8)PCIE_MAX_AXI_WRAPPER_REGION_NUM) ) {
        result = PCIE_ERR_ARG;
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
UINT32
PCIE_SetupObWrapperMemIoAccess(const PCIE_PrivateData * pD,
                               UINT8                  regionNo,
                               UINT8                  numPassedBits,
                               UINT64                 address)
{
    UINT32 ob_addr_0 = 0U;
    UINT32 ob_addr_1 = 0U;
    UINT32 ob_desc_0 = 0U;
    UINT32 ob_desc_1 = 0U;
    AxiWrapperOb * obw;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp;

    /* Checking of numPassedBits cannot be inside validation function,
       because MISRA tool reports lack of checking of shifting value in next condition */
    if ( (ValidInputSetupObWrapper(pD, regionNo) != 0U) ||
         ((numPassedBits + 1U) < (UINT8)PCIE_MIN_AXI_ADDR_BITS_PASSED) || (numPassedBits > 63U) ) {
        result = PCIE_ERR_ARG;
    }

    else {
        obw = GetWrapperObAddr(pD, regionNo);
        /* write ob-addr0 */
        ob_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_NUM_BITS_MASK, AXI_ADDR0_NUM_BITS_SHIFT, ob_addr_0, numPassedBits);
        ob_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_DATA_MASK, AXI_ADDR0_DATA_SHIFT, ob_addr_0, (UINT32)(0xFFFFFFU & (address >> 8U)));

        /* write ob-addr1 */
        ob_addr_1 = PCIE_RegFldWrite(AXI_ADDR1_DATA_MASK, AXI_ADDR1_DATA_SHIFT, ob_addr_1, (UINT32)(0xFFFFFFFFU & (address >> 32U)));

        /* write ob-desc0.  Bits [3:0] define the transaction type */
        if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
            result = PCIE_ERR_ARG;
        } else if (epOrRp == PCIE_CORE_RP_STRAPPED) {
            ob_desc_0 = PCIE_RegFldWrite(AXI_DESC0_DATA_MASK, AXI_DESC0_DATA_SHIFT, ob_desc_0, RID_VALID | AXI_WRAPPER_MEM_WRITE);
        } else {
            ob_desc_0 = PCIE_RegFldWrite(AXI_DESC0_DATA_MASK, AXI_DESC0_DATA_SHIFT, ob_desc_0, AXI_WRAPPER_MEM_WRITE);
        }

        if (result == PCIE_ERR_SUCCESS) {
            /** write the registers
             * addr0 is 1st element of ob wrapper
             * addr1 is 2nd, desc0 is 3rd, desc1 is 4th */
            PCIE_RegPtrWrite32(&(obw->addr0), ob_addr_0);
            PCIE_RegPtrWrite32(&(obw->addr1), ob_addr_1);
            PCIE_RegPtrWrite32(&(obw->desc0), ob_desc_0);
            PCIE_RegPtrWrite32(&(obw->desc1), ob_desc_1);

            /* Check for errors reported from PCIe IP */
            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* AXI Wrapper Setup for Message Access                                     */
/****************************************************************************/
UINT32
PCIE_SetupObWrapperMsgAccess(const PCIE_PrivateData * pD,
                             UINT8                  regionNo,
                             UINT64                 address)
{
    UINT32 ob_addr_0 = 0U;
    UINT32 ob_addr_1 = 0U;
    UINT32 ob_desc_0 = 0U;
    UINT32 ob_desc_1 = 0U;
    AxiWrapperOb * obw;
    UINT32 result = PCIE_ERR_SUCCESS;
    PCIE_CoreEpOrRp epOrRp;

    if ( (pD == NULL) || (regionNo >= (UINT8)PCIE_MAX_AXI_WRAPPER_REGION_NUM) ) {
        result = PCIE_ERR_ARG;
    } else {
        obw = GetWrapperObAddr(pD, regionNo);
        /* write ob-addr0 */
        ob_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_DATA_MASK, AXI_ADDR0_DATA_SHIFT, ob_addr_0, (UINT32)(0xFFFFFFU & (address >> 8U)));

        /* write ob-addr1 */
        ob_addr_1 = PCIE_RegFldWrite(AXI_ADDR1_DATA_MASK, AXI_ADDR1_DATA_SHIFT, ob_addr_1, (UINT32)(0xFFFFFFFFU & (address >> 32U)));

        /* write ob-desc0.  Bits [3:0] define the transaction type */
        if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
            result = PCIE_ERR_ARG;
        } else if (epOrRp == PCIE_CORE_RP_STRAPPED) {
            ob_desc_0 = PCIE_RegFldWrite(AXI_DESC0_DATA_MASK, AXI_DESC0_DATA_SHIFT, ob_desc_0, RID_VALID | AXI_WRAPPER_MSG_WRITE);
        } else {
            ob_desc_0 = PCIE_RegFldWrite(AXI_DESC0_DATA_MASK, AXI_DESC0_DATA_SHIFT, ob_desc_0, AXI_WRAPPER_MSG_WRITE);
        }

        if (result == PCIE_ERR_SUCCESS) {
            /** write the registers
             * addr0 is 1st element of ob wrapper
             * addr1 is 2nd, desc0 is 3rd, desc1 is 4th */
            PCIE_RegPtrWrite32(&(obw->addr0), ob_addr_0);
            PCIE_RegPtrWrite32(&(obw->addr1), ob_addr_1);
            PCIE_RegPtrWrite32(&(obw->desc0), ob_desc_0);
            PCIE_RegPtrWrite32(&(obw->desc1), ob_desc_1);

            result = CallErrorCallbackIfError(pD);
        }
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

static void PCIE_GetFunc0IbWrapper(const PCIE_PrivateData * pD,
                                   PCIE_BarNumber bar,
                                   volatile UINT32 **addr0,
                                   volatile UINT32 **addr1)
{
    switch (bar) {
    case PCIE_BAR_0:
        *addr0 = &(pD->p_aw_base->func0_wrapper_ib_ep_0.addr0);
        *addr1 = &(pD->p_aw_base->func0_wrapper_ib_ep_0.addr1);
        break;
    case PCIE_BAR_1:
        *addr0 = &(pD->p_aw_base->func0_wrapper_ib_ep_1.addr0);
        *addr1 = &(pD->p_aw_base->func0_wrapper_ib_ep_1.addr1);
        break;
    case PCIE_BAR_2:
        *addr0 = &(pD->p_aw_base->func0_wrapper_ib_ep_2.addr0);
        *addr1 = &(pD->p_aw_base->func0_wrapper_ib_ep_2.addr1);
        break;
    case PCIE_BAR_3:
        *addr0 = &(pD->p_aw_base->func0_wrapper_ib_ep_3.addr0);
        *addr1 = &(pD->p_aw_base->func0_wrapper_ib_ep_3.addr1);
        break;
    case PCIE_BAR_4:
        *addr0 = &(pD->p_aw_base->func0_wrapper_ib_ep_4.addr0);
        *addr1 = &(pD->p_aw_base->func0_wrapper_ib_ep_4.addr1);
        break;
    case PCIE_BAR_5:
        *addr0 = &(pD->p_aw_base->func0_wrapper_ib_ep_5.addr0);
        *addr1 = &(pD->p_aw_base->func0_wrapper_ib_ep_5.addr1);
        break;
    default:
        /* should not be here*/
        *addr0 = NULL;
        *addr1 = NULL;
        break;
    }

}

static AxiWrapperIb * GetWrapperIbAddr(
    const PCIE_PrivateData * pD,
    UINT8                  regionNo)
{
    UINT8 region = regionNo;
    AxiWrapperIb * aw_base = &pD->p_aw_base->ib0;
    switch (region) {
    case 0:
        aw_base = &pD->p_aw_base->ib0;
        break;
    case 1:
        aw_base = &pD->p_aw_base->ib1;
        break;
    case 7:
        aw_base = &pD->p_aw_base->ib7;
        break;
    default:
        /* should not be here */
        break;
    }

    /* Set aw_base to the address of first element of next wrapper */
    return aw_base;

}

/****************************************************************************/
/* AXI Wrapper Setup for Config region (Type 0)                             */
/****************************************************************************/
UINT32
PCIE_SetupObWrapperConfigAccess(const PCIE_PrivateData * pD,
                                UINT8                  regionNo,
                                UINT8                  numPassedBits,
                                UINT32                 busDevFuncAddr)
{
    UINT32 ob_addr_0 = 0U;
    UINT32 ob_addr_1 = 0U;
    UINT32 ob_desc_0 = 0U;
    UINT32 ob_desc_1 = 0U;
    AxiWrapperOb * obw;
    UINT32 result = 0U;
    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    }

    else if ( (regionNo >= (UINT8)PCIE_MAX_AXI_WRAPPER_REGION_NUM) ||
              ((numPassedBits + 1U) < (UINT8)PCIE_MIN_AXI_ADDR_BITS_PASSED) ||
              (numPassedBits > 63U) ) {
        result = PCIE_ERR_ARG;
    } else {
        obw = GetWrapperObAddr(pD, regionNo);
        /* write ob-addr0 */
        ob_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_NUM_BITS_MASK, AXI_ADDR0_NUM_BITS_SHIFT, ob_addr_0, numPassedBits);
        ob_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_DATA_MASK, AXI_ADDR0_DATA_SHIFT, ob_addr_0, busDevFuncAddr);

        /* write ob-desc0.  Bits [3:0] define the transaction type */
        ob_desc_0 = PCIE_RegFldWrite(AXI_DESC0_DATA_MASK, AXI_DESC0_DATA_SHIFT, ob_desc_0, RID_VALID | AXI_WRAPPER_TYPE0_WRITE);

        /** write the registers
         * addr0 is 1st element of ob wrapper
         * addr1 is 2nd, desc0 is 3rd, desc1 is 4th */
        PCIE_RegPtrWrite32(&(obw->addr0), ob_addr_0);
        PCIE_RegPtrWrite32(&(obw->addr1), ob_addr_1);
        PCIE_RegPtrWrite32(&(obw->desc0), ob_desc_0);
        PCIE_RegPtrWrite32(&(obw->desc1), ob_desc_1);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* Root Port Address Translation                                            */
/****************************************************************************/
UINT32
PCIE_SetupIbRootPortAddrTransl(const PCIE_PrivateData * pD,
                               UINT8                  regionNo,
                               UINT8                  numPassedBits,
                               UINT64                 address)
{
    UINT32 ib_addr_0 = 0U;

    UINT32 ib_addr_1 = 0U;
    volatile UINT32 * aw_base_addr0;

    volatile UINT32 * aw_base_addr1;

    UINT32 result = PCIE_ERR_SUCCESS;

    PCIE_CoreEpOrRp epOrRp = PCIE_CORE_EP_STRAPPED;

    if (PCIE_IsCoreStrappedAsEpOrRp(pD, &epOrRp) != 0U) {
        result = PCIE_ERR_ARG;
    } else if (epOrRp == PCIE_CORE_EP_STRAPPED) {
        /* Only valid if IP strapped as RP */
        result = PCIE_ERR_ARG;
    } else if (( regionNo > (UINT8)MAX_AXI_IB_ROOTPORT_REGION_NUM) ||
               ((numPassedBits + 1U) < (UINT8)PCIE_MIN_AXI_ADDR_BITS_PASSED) ||
               (numPassedBits > 63U) ) {
        result = PCIE_ERR_ARG;
    } else {
        AxiWrapperIb * ibw = GetWrapperIbAddr(pD, regionNo);

        aw_base_addr0 = &(ibw->addr0);
        aw_base_addr1 = &(ibw->addr1);

        /* write ib-addr0 */
        ib_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_NUM_BITS_MASK, AXI_ADDR0_NUM_BITS_SHIFT, ib_addr_0, numPassedBits);
        ib_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_DATA_MASK, AXI_ADDR0_DATA_SHIFT, ib_addr_0, (UINT32)(0xFFFFFFU & (address >> 8U)));

        /* write ib-addr1 */
        ib_addr_1 = PCIE_RegFldWrite(AXI_ADDR1_DATA_MASK, AXI_ADDR1_DATA_SHIFT, ib_addr_1, (UINT32)(0xFFFFFFFFU & (address >> 32U)));

        /* write the registers */
        PCIE_RegPtrWrite32((aw_base_addr0), ib_addr_0);

        PCIE_RegPtrWrite32((aw_base_addr1), ib_addr_1);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
/****************************************************************************/
/* End Point Address Translation                                            */
/****************************************************************************/
UINT32
PCIE_SetupIbEPAddrTranslation(const PCIE_PrivateData * pD,
                              UINT32                 functionNo,
                              PCIE_BarNumber         barNo,
                              UINT64                 address)
{
    UINT32 ib_addr_0 = 0U;
    UINT32 ib_addr_1 = 0U;

    volatile UINT32 * aw_base_addr0;
    volatile UINT32 * aw_base_addr1;

    UINT32 result = PCIE_ERR_SUCCESS;

    if ( (pD == NULL) || ((UINT32)barNo >= PCIE_MAX_BARS_IN_FUNC) ||
         (functionNo >= (PCIE_NUM_PCI_FUNCTIONS ) ) ) {
        result = PCIE_ERR_ARG;
    } else {

        PCIE_GetFunc0IbWrapper(pD, barNo, &aw_base_addr0, &aw_base_addr1);

        /* write ib-addr0 - Since the minimum aperture size allowed is 128 Bit, the lower 8 bits is ignored */
        ib_addr_0 = PCIE_RegFldWrite(AXI_ADDR0_DATA_MASK, AXI_ADDR0_DATA_SHIFT, ib_addr_0, (UINT32)(0xFFFFFFU & (address >> 8U)));

        /* write ib-addr1 */
        ib_addr_1 = PCIE_RegFldWrite(AXI_ADDR1_DATA_MASK, AXI_ADDR1_DATA_SHIFT, ib_addr_1, (UINT32)(0xFFFFFFFFU & (address >> 32U)));

        /** write the registers */
        PCIE_RegPtrWrite32(aw_base_addr0, ib_addr_0);

        PCIE_RegPtrWrite32(aw_base_addr1, ib_addr_1);

        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }
    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}

/****************************************************************************/
/* AXI Wrapper Setup for MemIO region                                       */
/****************************************************************************/
UINT32
PCIE_SetupObWrapperRegions(const PCIE_PrivateData * pD,
                           UINT8                  regionNo,
                           UINT8                  numPassedBits,
                           UINT32                 upperAddr,
                           UINT32                 lowerAddr)

{
    UINT32 result = PCIE_ERR_SUCCESS;
    UINT32 axi_addr0 = 0U;
    UINT32 axi_addr1 = 0U;
    AxiWrapperOb * obw;

    if ( (pD == NULL) ||
         (regionNo >= PCIE_MAX_AXI_WRAPPER_REGION_NUM) ||
         ((numPassedBits + 1U) < PCIE_MIN_AXI_ADDR_BITS_PASSED) ||
         (numPassedBits > 63U) ) {
        result = PCIE_ERR_ARG;
    } else {
        obw = GetWrapperObAddr(pD, regionNo);
        axi_addr0 = lowerAddr | numPassedBits;
        axi_addr1 = upperAddr;

        /** write the registers
         *  axi_addr0 is 7th element of wrapper structure
         *  axi_addr1 is 8th element of wrapper structure */
        PCIE_RegPtrWrite32(&(obw->axi_addr0), axi_addr0);
        PCIE_RegPtrWrite32(&(obw->axi_addr1), axi_addr1);
        /* Check for errors reported from PCIe IP */
        result = CallErrorCallbackIfError(pD);
    }

    return (result); /* 0 for success (PCIE_ERR_SUCCESS) */
}
