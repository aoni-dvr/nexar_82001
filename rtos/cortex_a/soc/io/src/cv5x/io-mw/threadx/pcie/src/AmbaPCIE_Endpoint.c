/**
 *  @file AmbaPCIE_Endpoint.c
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
 *  @details PCIe driver for the endpoint.
 */
#include "AmbaPCIE_Drv.h"
#include "AmbaPCIE.h"


/*-------------------------------------------------------------------------------*/
/*  Called by core driver if local interrupt condition  reported in              */
/*   local_error_status_register)                                                */
/*-------------------------------------------------------------------------------*/
static void PCIE_CallbackHandler_ep (UINT32 et)
{

    PCIE_UtilityPrintUInt5(" CallbackHandler Called on EP with status 0x%x",et, 0, 0, 0, 0);
}

/*-------------------------------------------------------------------------------*/
/*  Treat a DMA interrupt reported by the driver.  Called by core driver if      */
/*  DMA interrupt reported in udma_int register                                  */
/*-------------------------------------------------------------------------------*/
static void
PCIE_DmaHandler_ep(UINT32 dmaChannel, PCIE_Bool isDone)
{
    if (isDone == PCIE_TRUE) {
        PCIE_UtilityPrintUInt5(" Received DMA done interrupt for channel %d",dmaChannel, 0, 0, 0, 0);
    } else {
        PCIE_UtilityPrintUInt5(" Received DMA error interrupt for channel %d",dmaChannel, 0, 0, 0, 0);
    }
}


/*-------------------------------------------------------------------------------*/
/* Probe for and setup the EP core driver                                        */
/*-------------------------------------------------------------------------------*/
static UINT32 Impl_ProbeAndInitEPDriver (PCIE_PrivateData *pD,UINT32 pd_size, UINT64 ep_base)
{

    UINT32 ret_val;

    UINT32 ii;


    PCIE_InitParam initParams;
    PCIE_SysReq     sysReq;

    const PCIE_OBJ * obj = PCIE_GetInstance();

    PCIE_UtilityMemorySet(& initParams, 0, sizeof (PCIE_InitParam));

    /*--- call core driver API - probe for EP --*/
    ret_val = obj->probe (PCIE_CFG_BASE, & sysReq);

    if ((ret_val != PCIE_ERR_SUCCESS) || (sysReq.pdataSize > pd_size)) {
        PCIE_UtilityPrint(">> Probe failed for EP");
    }

    /*-- initialize the callbacks for errors --*/
    initParams.initErrorCallback = PCIE_CallbackHandler_ep;

    /* If an interrupt handler has not been installed, mask all interrupts.  These will still
     * be reported in the local_error_status_register, which will be polled on each call to the
     * driver API.  */
    initParams.initMaskableConditions.maskPnpRxFifoParityError.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskPnpRxFifoParityError.changeDefault = 1;
    initParams.initMaskableConditions.maskPnpRxFifoOverflow.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskPnpRxFifoOverflow.changeDefault = 1;

    initParams.initMaskableConditions.maskCompletionRxFifoParityError.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskCompletionRxFifoParityError.changeDefault = 1;
    initParams.initMaskableConditions.maskReplayRamParityError.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskReplayRamParityError.changeDefault = 1;
    initParams.initMaskableConditions.maskCompletionRxFifoOverflow.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskCompletionRxFifoOverflow.changeDefault = 1;
    initParams.initMaskableConditions.maskReplayTimeout.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskReplayTimeout.changeDefault = 1;
    initParams.initMaskableConditions.maskReplayTimerRollover.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskReplayTimerRollover.changeDefault = 1;
    initParams.initMaskableConditions.maskPhyError.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskPhyError.changeDefault = 1;
    initParams.initMaskableConditions.maskMalformedTlpReceived.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskMalformedTlpReceived.changeDefault = 1;
    initParams.initMaskableConditions.maskUnexpectedComplReceived.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskUnexpectedComplReceived.changeDefault = 1;
    initParams.initMaskableConditions.maskFlowControlError.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskFlowControlError.changeDefault = 1;
    initParams.initMaskableConditions.maskCompletionTimeout.newValue = (UINT32)PCIE_MASK_PARAM;
    initParams.initMaskableConditions.maskCompletionTimeout.changeDefault = 1;


    /*-- initialize the callback for DMA --*/
    initParams.initUdmaCallback = PCIE_DmaHandler_ep;

    /* Now initialize the EP with these values */
    if (ret_val == PCIE_ERR_SUCCESS) {
        ret_val = obj->init (pD, ep_base, & initParams);
    }
#if 0
    /* Some Physical Function register fields can be initialised via the Local Management bus
     * This is not done via the core driver.  The following write will set the Device ID for
     * PF0 to 0x0100
     */
    PCIE_RegAddrWrite16(PCIE_CFG_BASE+0x200002U,0x0100);
#endif
    PCIE_RegAddrWrite16(PCIE_CFG_BASE+0xC8U,0x2810);

    /* If a DMA interrupt handler has not been installed, mask all DMA interrupts. */
    /* Done and Error interrupt status will still be set in the DMA interupt       */
    /* register, but DMA_INTERRUPT will not be asserted. */
    for (ii=0; ii < PCIE_NUM_UDMA_CHANNELS; ii++) {
        if (ret_val == PCIE_ERR_SUCCESS) {
            ret_val = obj->UDMA_ControlDoneInterrupts(pD,ii,PCIE_DISABLE_PARAM);
        }
        if (ret_val == PCIE_ERR_SUCCESS) {
            ret_val = obj->UDMA_ControlErrInterrupts(pD,ii,PCIE_DISABLE_PARAM);
        }
    }
    return ret_val;
}


UINT32 Impl_EpInit (UINT64 ep_base)
{

    PCIE_PrivateData *pd = PCIE_GetPrivateData();
    UINT32 ret_val;


    /* This will initialise the driver for use with the device at CPDREF_EP_BASE, */
    /* and will fail if the device isn't detected.                                */

    ret_val = Impl_ProbeAndInitEPDriver(pd, (UINT32)sizeof(PCIE_PrivateData), ep_base);
    if (ret_val != PCIE_ERR_SUCCESS) {
        PCIE_UtilityPrint(">> Unable to initialize driver for EP");
    }

    /* Read and display configuration info */
    ret_val = Impl_GetConfigurationInfo(pd);
    if (ret_val != PCIE_ERR_SUCCESS) {
        PCIE_UtilityPrint(">> Unable to display config info for EP");
    }


    return ret_val;
}

static PCIE_BarApertureSize PCIEE_IbBitsToAperture(UINT8 bits)
{
    PCIE_BarApertureSize size;
    // NumPassBits - 6
    switch(bits) {
    case 11:
        size = PCIE_APERTURE_SIZE_4K;
        break;
    case 12:
        size = PCIE_APERTURE_SIZE_8K;
        break;
    case 13:
        size = PCIE_APERTURE_SIZE_16K;
        break;
    case 14:
        size = PCIE_APERTURE_SIZE_32K;
        break;
    case 15:
        size = PCIE_APERTURE_SIZE_64K;
        break;
    case 16:
        size = PCIE_APERTURE_SIZE_128K;
        break;
    case 17:
        size = PCIE_APERTURE_SIZE_256K;
        break;
    case 18:
        size = PCIE_APERTURE_SIZE_512K;
        break;
    case 19:
        size = PCIE_APERTURE_SIZE_1M;
        break;
    case 20:
        size = PCIE_APERTURE_SIZE_2M;
        break;
    case 21:
        size = PCIE_APERTURE_SIZE_4M;
        break;
    case 22:
        size = PCIE_APERTURE_SIZE_8M;
        break;
    case 23:
        size = PCIE_APERTURE_SIZE_16M;
        break;
    case 24:
        size = PCIE_APERTURE_SIZE_32M;
        break;
    case 25:
        size = PCIE_APERTURE_SIZE_64M;
        break;
    case 26:
        size = PCIE_APERTURE_SIZE_128M;
        break;
    case 27:
        size = PCIE_APERTURE_SIZE_256M;
        break;
    case 28:
        size = PCIE_APERTURE_SIZE_512M;
        break;
    case 29:
        size = PCIE_APERTURE_SIZE_1G;
        break;
    case 30:
        size = PCIE_APERTURE_SIZE_2G;
        break;
    case 31:
        size = PCIE_APERTURE_SIZE_4G;
        break;
    case 32:
        size = PCIE_APERTURE_SIZE_8G;
        break;
    case 33:
        size = PCIE_APERTURE_SIZE_16G;
        break;
    case 34:
        size = PCIE_APERTURE_SIZE_32G;
        break;
    case 35:
        size = PCIE_APERTURE_SIZE_64G;
        break;
    case 36:
        size = PCIE_APERTURE_SIZE_128G;
        break;
    case 37:
        size = PCIE_APERTURE_SIZE_256G;
        break;
    // assign reset value
    default:
        size = PCIE_APERTURE_SIZE_2G;
        break;
    }
    return size;
}

UINT32 AmbaPCIED_Inbound(const PCIED_IB_PARAM_s *params)
{
    UINT32 ret_val;
    UINT32 flag = Impl_ApiFlagGet();

    if (flag != FLAG_PCIE_API_EP_INIT) {
        ret_val = PCIE_ERR_FLOW;
    } else if (params == NULL || ((params->Size & (params->Size - 1)) != 0UL)) {
        ret_val = PCIE_ERR_ARG;
    } else {
        const PCIE_PrivateData *pd = PCIE_GetPrivateData();
        const PCIE_OBJ * obj = PCIE_GetInstance();
        PCIE_BarControl control_val  = PCIE_PREFETCHABLE_64_BIT_MEM_BAR;
        PCIE_BarNumber Bar = PCIE_BAR_0;
        UINT32 Func = 0;
        UINT64 MemAddr = params->MemAddr;
        UINT8 NumPassBits;
        DOUBLE power;
        PCIE_BarApertureSize aperture_size;

        PCIE_UtilityLog2((DOUBLE)params->Size, &power);
        NumPassBits = (UINT8)power - 1U; // log2(size) - 1
        aperture_size = PCIEE_IbBitsToAperture(NumPassBits);

        //PCI address is assigned by RP. Just need to set aperture size. Set BAR# to AXI
        ret_val =  obj->setFuncBarControlSetting(pd, Func, Bar, control_val);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("Eror: setFuncBarControlSetting() ");
        }
        ret_val = obj->setFuncBarApertureSetting(pd, Func, Bar, aperture_size);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("Eror: setFuncBarApertureSetting() ");
        }
        /* Set inbound address for End Point BAR 0, PF0 */
        ret_val = obj->setupIbEPAddrTranslation (pd,Func,Bar,MemAddr);
        if(ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("Eror: setupIbRootPortAddrTransl() ");
        }
    }
    return ret_val;
}

UINT32 AmbaPCIED_Outbound(const PCIED_OB_PARAM_s *params)
{
    UINT32 ret_val;
    UINT32 flag = Impl_ApiFlagGet();

    if (flag != FLAG_PCIE_API_EP_START) {
        ret_val = PCIE_ERR_FLOW;
    } else if (params == NULL || ((params->Size & (params->Size - 1)) != 0UL)) {
        ret_val = PCIE_ERR_ARG;
    } else {
        const PCIE_PrivateData *pd = PCIE_GetPrivateData();
        const PCIE_OBJ * obj = PCIE_GetInstance();
        UINT8 Region = 0;
        UINT64 AxiAddr = params->AxiAddr, PciAddr = params->PciAddr;
        UINT8 NumPassBits;
        DOUBLE power;

        PCIE_UtilityLog2((DOUBLE)params->Size, &power);
        NumPassBits = (UINT8)power - 1U; // log2(size) - 1

        /* Set up Region 0 as size 1MB (19 valid bits in address)
        */
        ret_val = obj->setupObWrapperRegions (pd, Region, NumPassBits, (UINT32)(AxiAddr>>32U),(UINT32)(AxiAddr&0xFFFFFFFFU));
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("setupObWrapperRegions failed.");
        }
        /* Configure EP region 0 to address a BAR on RP for memory access.
         * 1 - AXI wrapper region number.
         * 19 - Number of address bits to use(wrapper will add 1 to this value). This determines addressable memory range.
         * This number is limited by the aperture size of the core.
         * CPDREF_PCIE_ADDR_RP - Prefix bits to be added to address. This will determine which bar is matched.
         */
        ret_val = obj->setupObWrapperMemIoAccess (pd, Region, NumPassBits, PciAddr);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("setupObWrapperMemIoAccess failed.");
        }
        Impl_ApiFlagSet(FLAG_PCIE_API_EP_READY);
    }
    return ret_val;
}
