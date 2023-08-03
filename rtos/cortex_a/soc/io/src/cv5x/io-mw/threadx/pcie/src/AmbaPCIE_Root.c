/**
 *  @file AmbaPCIE_Root.c
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
 *  @details PCIe driver for the root.
 */
#include "AmbaPCIE_Drv.h"
#include "AmbaPCIE.h"

typedef struct {
    UINT32 bus;
    UINT32 bar;
    UINT32 dev;
    UINT32 func;
    UINT32 offset;
    UINT32 addr_lo;
    UINT32 addr_hi;
} MemoryTypeData;


/* Local function to build BDF out of bus, device and function IDs */
static UINT16 Impl_BuildBDF ( UINT32 bus,
                              UINT32 dev,
                              UINT32 func)
{
    UINT16 bdf;

    /* set upper 8 bits - bus id - for BDF and shift left */
    bdf = (UINT16)((bus & 0x000000FFU) << 8U);
    /* set 5 dev bits for BDF and shift left */
    bdf |= (UINT16)((dev & 0x0000001FU) << 3U);
    /* set 3 func bits for BDF */
    bdf |= (UINT16)(func & 0x00000007U);

    return (bdf);
}


/*-------------------------------------------------------------------------------*/
/*  Called by core driver if local interrupt condition  reported in              */
/*   local_error_status_register)                                                */
/*-------------------------------------------------------------------------------*/
static void PCIE_CallbackHandler_rp (UINT32 et)
{

    PCIE_UtilityPrintUInt5(" CallbackHandler Called with status 0x%x",et, 0, 0, 0, 0);
}

/*-------------------------------------------------------------------------------*/
/*  Treat a DMA interrupt reported by the driver.  Called by core driver if      */
/*  DMA interrupt reported in udma_int register                                  */
/*-------------------------------------------------------------------------------*/
static void PCIE_DmaHandler_rp(UINT32 dmaChannel, PCIE_Bool isDone)
{
    if (isDone == PCIE_TRUE) {
        PCIE_UtilityPrintUInt5(" Received DMA done interrupt for channel %d",dmaChannel, 0, 0, 0, 0);
    } else {
        PCIE_UtilityPrintUInt5(" Received DMA error interrupt for channel %d",dmaChannel, 0, 0, 0, 0);
    }

}

/*-------------------------------------------------------------------------------*/
/* Probe for and setup the RP core driver                                        */
/*-------------------------------------------------------------------------------*/
static UINT32 Impl_ProbeAndInitRPDriver (PCIE_PrivateData *pD,UINT32 pd_size, UINT64 rp_base, UINT64 axi_base)
{

    UINT32 ret_val;
    UINT8 bitsInRegion0Address = 11; /* Configure Region 0 as size 4KB */

    PCIE_InitParam initParams;
    PCIE_SysReq     sysReq;

    UINT32 ii;


    const PCIE_OBJ * obj = PCIE_GetInstance();

    PCIE_UtilityMemorySet(&initParams, 0, sizeof (PCIE_InitParam));

    /*--- call core driver API - probe for RP --*/
    ret_val = obj->probe (PCIE_CFG_BASE, & sysReq);

    if ((ret_val != PCIE_ERR_SUCCESS) || (sysReq.pdataSize > pd_size)) {
        PCIE_UtilityPrintUInt5(">> Probe failed for RP 0x%x", ret_val, 0, 0, 0, 0);
    }

    /*-- initialize the callbacks for errors --*/
    initParams.initErrorCallback = PCIE_CallbackHandler_rp;

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
    initParams.initUdmaCallback = PCIE_DmaHandler_rp;


    /* Set the base address of AXI Region 0, for use in ECAM config read/writes   */
    initParams.initBaseAddresses.axBase.newValue = axi_base;
    initParams.initBaseAddresses.axBase.changeDefault = 1;


#if 0
    /* The vendor and subsystem vendor ID can be configured in RTL after            */
    /* delivery (see Cadence IP Core User Guide).  The following configuration      */
    /* is only necessary if you wish to change these at run time                    */

    initParams.initIdParams.vendorId.newValue = CADENCE_VENDOR_ID;
    initParams.initIdParams.vendorId.changeDefault = 1;
    initParams.initIdParams.subsystemVendorId.newValue = CADENCE_VENDOR_ID;
    initParams.initIdParams.subsystemVendorId.changeDefault = 1;
#endif

    /*--- call core driver API - init for RP --*/
    if (ret_val == PCIE_ERR_SUCCESS) {
        ret_val = obj->init (pD, rp_base, & initParams);
    }

    if (ret_val == PCIE_ERR_SUCCESS) {
        ret_val = obj->setupObWrapperRegions (pD, 0, bitsInRegion0Address, 0, PCIE_RP_OB_REGION0_AXI_OFFSET);
    }

    /* Set up AXI Region 0 for PCIE config space.                                   */
    /* This is required before using doConfigRead(), doConfigWrite() or             */
    /* enableBarAccess                                                              */
    if (ret_val == PCIE_ERR_SUCCESS) {
        ret_val = obj->setupObWrapperConfigAccess(pD, 0, bitsInRegion0Address, PCIE_RP_OB_REGION0_PCI);
    }


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

static UINT32 ValidateDataSize(UINT32 r)
{
    UINT32 sz;
    if ((r & 0xfffffff0U) != 0U) {
        sz = ~(r & ~0xfU) + 1U;
    } else {
        sz = 0;
    }
    return (sz);
}

static UINT32 Impl_EnableAccessMemType0(
    const PCIE_PrivateData* pD,
    MemoryTypeData *mem_type,
    UINT32 v,
    UINT32 r)
{
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT32 result;
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev,  mem_type->func);
    UINT32 sz;
    /**
     * get size
     */
    result = obj->doConfigWrite(pD, bdf, mem_type->offset, PCIE_4_BYTE, 0xfffffff0U | v);
    if (result == PCIE_ERR_SUCCESS) {
        result = obj->doConfigRead (pD, bdf, mem_type->offset, PCIE_4_BYTE, (UINT32 *)&r);
    }
    /**
     * restore BAR
     */
    if (result == PCIE_ERR_SUCCESS) {
        result = obj->doConfigWrite(pD, bdf, mem_type->offset, PCIE_4_BYTE, v);
    }

    if (result == PCIE_ERR_SUCCESS) {
        sz = ValidateDataSize(r);
        if (sz != 0U) {
            /**
             * map it and set BAR
             */
            mem_type->offset = 0x10U + (mem_type->bar << 2);

            result = obj->doConfigWrite(pD, bdf, mem_type->offset, PCIE_4_BYTE, mem_type->addr_lo | (v & 0xfU));

        }
    }
    return (result);
}
static UINT32 Impl_WriteNewMemorySizeConfig(
    const PCIE_PrivateData* pD,
    const MemoryTypeData *mem_type,
    const UINT32 *p_v)
{
    UINT32 result;
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev, mem_type->func);

    result = obj->doConfigWrite(pD, bdf, mem_type->offset, PCIE_4_BYTE, 0xfffffff0U | *p_v);

    if (result == PCIE_ERR_SUCCESS) {
        result = obj->doConfigWrite(pD, bdf, mem_type->offset + 4U, PCIE_4_BYTE, 0xffffffffU);
    }

    return (result);
}
static UINT32 Impl_GetMemorySize(
    const PCIE_PrivateData* pD,
    const MemoryTypeData *mem_type,
    UINT32 *p_v2,
    const UINT32 *p_v,
    UINT32 *p_r)
{
    UINT32 result;
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev, mem_type->func);
    UINT32 offset = mem_type->offset;
    UINT32 next_offset = mem_type->offset + 4U;
    const PCIE_OBJ * obj = PCIE_GetInstance();

    result = obj->doConfigRead(pD, bdf, next_offset, PCIE_4_BYTE, p_v2);

    if (result == PCIE_ERR_SUCCESS) {
        result = Impl_WriteNewMemorySizeConfig(pD, mem_type, p_v);
    }
    if (result == PCIE_ERR_SUCCESS) {
        result = obj->doConfigRead(pD, bdf, offset, PCIE_4_BYTE, p_r);
    }

    return (result);
}
static UINT32 Impl_RestoreBars(
    const PCIE_PrivateData* pD,
    const MemoryTypeData *mem_type,
    const UINT32 *p_v2,
    const UINT32 *p_v)
{
    UINT32 result;
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev, mem_type->func);

    UINT32 tmp_v = *p_v;
    UINT32 tmp_v2 = *p_v2;
    result = obj->doConfigWrite(pD, bdf, mem_type->offset, PCIE_4_BYTE, tmp_v);

    if (result == PCIE_ERR_SUCCESS) {
        result = obj->doConfigWrite(pD, bdf, mem_type->offset + 4U, PCIE_4_BYTE, tmp_v2);
    }
    return (result);
}

#if 0
static UINT32 Impl_MapMemory32bit(
    const PCIE_PrivateData* pD,
    const MemoryTypeData *mem_type,
    UINT32 v)
{
    /**
     * map it as 32-bit
     */
    UINT32 result;
    UINT32 tmp = 0x10U + (mem_type->bar << 2);
    PCIE_OBJ * obj = PCIE_GetInstance();
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev, mem_type->func);

    result = obj->doConfigWrite(pD, bdf, tmp, PCIE_4_BYTE, mem_type->addr_lo | (v & 0xfU));

    return (result);
}
#endif

static UINT32 Impl_MapMemory64bit(
    const PCIE_PrivateData* pD,
    const MemoryTypeData *mem_type,
    UINT32 v,
    UINT32 base64)
{
    /**
     * map it as full 64-bit
     */
    UINT32 result;
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev, mem_type->func);

    result = obj->doConfigWrite(pD, bdf, mem_type->offset + 4U, PCIE_4_BYTE, base64);

    if (result == PCIE_ERR_SUCCESS) {
        result = obj->doConfigWrite(pD, bdf, mem_type->offset, PCIE_4_BYTE, mem_type->addr_lo | (v & 0xfU));
    }
    return (result);
}
static UINT32 Impl_EnableAccessMemType2(
    const PCIE_PrivateData* pD,
    const MemoryTypeData *mem_type,
    UINT32 v,
    UINT32 r)
{
    /**
     * get size
     */
    UINT32 result, v2, sz = 0, sz2 = 0;
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT16 bdf = Impl_BuildBDF(mem_type->bus, mem_type->dev, mem_type->func);

    result = Impl_GetMemorySize(pD, mem_type, &v2, &v, &r);
    if (result == PCIE_ERR_SUCCESS) {
        sz = ~(r & ~0xfU) + 1U;
        result = obj->doConfigRead(pD, bdf, mem_type->offset + 4U, PCIE_4_BYTE, (UINT32 *)&r);
    }

    if (result == PCIE_ERR_SUCCESS) {
        if (r != 0U) {
            sz2 = ~r;
        } else {
            sz2 =  0;
        }

        /**
         * restore BARs
         */
        result = Impl_RestoreBars(pD, mem_type, &v, &v2);
    }
    /**
     * map it
     */
#if 0
    if ( (sz2 == 0U) && (sz < 0x00400000U)) {
        result = Impl_MapMemory32bit(pD, mem_type, v);
    } else {
        result = Impl_MapMemory64bit(pD, mem_type, v, mem_type->addr_hi);
    }
#else
    // No matter how long the size is, always set 64-bit address
    // Or, the address will be truncated.
    (void)sz2;
    (void)sz;
    if (result == PCIE_ERR_SUCCESS) {
        result = Impl_MapMemory64bit(pD, mem_type, v, mem_type->addr_hi);
    }
#endif
    return (result);
}

static UINT32 Impl_EnableBarAccess(const PCIE_PrivateData* pD,
                                   UINT32     bus,
                                   UINT32     dev,
                                   UINT32     func,
                                   UINT32     bar,
                                   UINT32     addr_hi,
                                   UINT32     addr_lo)
{
    UINT32 v, reg, io, memtype, r;
    UINT32 result;
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT16 bdf = Impl_BuildBDF(bus, dev, func);

    v = 0;
    r = 0;

    MemoryTypeData mem_data;

    reg = 0x10U + (bar << 2);

    result = obj->doConfigRead(pD, bdf, reg, PCIE_4_BYTE, (UINT32 *)&v);

    io = v & 1U;
    memtype = (v >> 1) & 3U;


    mem_data.bus = bus;
    mem_data.bar = bar;
    mem_data.dev = dev;
    mem_data.func = func;
    mem_data.offset = reg;
    mem_data.addr_lo = addr_lo;
    mem_data.addr_hi = addr_hi;

    if (io != 0U) {
        result = PCIE_ERR_ARG;
    } else {
        switch (memtype) { // check if 32-bit or 64 bit address
        case 0:
            result = Impl_EnableAccessMemType0(pD, &mem_data, v, r);
            break;
        case 2:
            result = Impl_EnableAccessMemType2(pD, &mem_data, v, r);
            break;
        default:
            /**
             * undefined memory type from device
             */
            result = PCIE_ERR_ARG;
            break;
        }
        /**
         * Enable memory access and  Bus Mastering
         */
        if (result == 0U) {
            result = obj->doConfigWrite(pD, bdf, 4, PCIE_4_BYTE, 6);
        }
    }
    return (result);
}

UINT32 Impl_RcInit(UINT64 rp_base, UINT64 axi_base)
{

    PCIE_PrivateData *pd = PCIE_GetPrivateData();
    UINT32 ret_val;

    /* This will initialise the driver for use with the RP device at CPDREF_RP_BASE, */
    /* and will fail if the hardware isn't detected.                                 */
    ret_val = Impl_ProbeAndInitRPDriver(pd, (UINT32)sizeof(PCIE_PrivateData), rp_base, axi_base);
    if (ret_val != PCIE_ERR_SUCCESS) {
        PCIE_UtilityPrint(">> Unable to initialize driver for RP");
    } else {
        /* Read and display configuration info */
        ret_val = Impl_GetConfigurationInfo(pd);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint(">> Unable to display config info for RP");
        }
    }

    return ret_val;
}

UINT32 Impl_RcGetEpBarInfo(void)
{
    const PCIE_PrivateData *pd = PCIE_GetPrivateData();
    UINT32 Bus = 0, Dev = 0, Func = 0;
    UINT16 bdf = Impl_BuildBDF(Bus, Dev, Func);
    const PCIE_OBJ * obj = PCIE_GetInstance();
    UINT32 v, v2 = 0, reg, io, memtype, r = 0, r2;
    UINT32 result;
    UINT32 bar = 0;
    UINT32 sz = 0, sz2;

    // parse all bar info, at most 6 for 32-bit address range
    while (bar < 6U) {
        reg = 0x10U + (bar << 2U);
        result = obj->doConfigRead(pd, bdf, reg, PCIE_4_BYTE, (UINT32 *)&v);
        io = v & 1U;
        memtype = (v >> 1U) & 3U;

        if (memtype == 0U) {// 32-bit
            // get size
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigWrite(pd, bdf, reg, PCIE_4_BYTE, 0xfffffff0U | v);
            }
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigRead (pd, bdf, reg, PCIE_4_BYTE, (UINT32 *)&r);
            }

            if (result == PCIE_ERR_SUCCESS) {
                if ((r & 0xfffffff0U) != 0U) {
                    sz = ~(r & ~0xfU) + 1U;
                } else {
                    sz = 0;
                }
                PCIE_UtilityPrintUInt5("Bar[%d] I/O = %d Memory type = %d, Size = %d", bar, io, memtype, sz, 0);
                // restore bar
                result = obj->doConfigWrite(pd, bdf, reg, PCIE_4_BYTE, v);
            }
            // next bar
            bar = bar + 1U;
        } else {// 64-bit
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigRead(pd, bdf, reg + 4U, PCIE_4_BYTE, &v2);
            }
            // get size 1
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigWrite(pd, bdf, reg, PCIE_4_BYTE, 0xfffffff0U | v);
            }
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigRead(pd, bdf, reg, PCIE_4_BYTE, &r);
            }
            // get size 2
            if (result == PCIE_ERR_SUCCESS) {
                if ((r & 0xfffffff0U) != 0U) {
                    sz = ~(r & ~0xfU) + 1U;
                } else {
                    sz = 0;
                }
                result = obj->doConfigWrite(pd, bdf, reg + 4U, PCIE_4_BYTE, 0xffffffffU);
            }

            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigRead(pd, bdf, reg + 4U, PCIE_4_BYTE, &r2);
                if (r2 != 0U) {
                    sz2 = ~r2;
                } else {
                    sz2 =  0;
                }
                PCIE_UtilityPrintUInt5("Bar[%d] I/O = %d Memory type = %d, Size = %d, Size2 = %d", bar, io, memtype, sz, sz2);
            }
            // restore bar
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigWrite(pd, bdf, reg, PCIE_4_BYTE, v);
            }
            if (result == PCIE_ERR_SUCCESS) {
                result = obj->doConfigWrite(pd, bdf, reg + 4U, PCIE_4_BYTE, v2);
            }

            bar = bar + 2U;
        }

    }

    return result;
}

static inline UINT32 PCIER_GetBarOffset(UINT32 Bar)
{
    return 0x10U + (Bar << 2U);
}
// Memory access to endpoint
UINT32 AmbaPCIER_Outbound(const PCIER_OB_PARAM_s *params)
{
    UINT32 ret_val;
    UINT32 flag = Impl_ApiFlagGet();

    if (flag != FLAG_PCIE_API_RC_START) {
        ret_val = PCIE_ERR_FLOW;
    } else if (params == NULL || ((params->Size & (params->Size - 1)) != 0UL)) {
        ret_val = PCIE_ERR_ARG;
    } else {
        const PCIE_PrivateData *pd = PCIE_GetPrivateData();
        const PCIE_OBJ * obj = PCIE_GetInstance();
        // Assumption
        UINT32 Bus = 0, Dev = 0, Func = 0, Bar = (UINT32)PCIE_BAR_0;
        UINT8 Region = 1;
        UINT32 pci_hi = 0, pci_lo;
        UINT32 offset;
        UINT16 bdf = Impl_BuildBDF(Bus, Dev, Func);
        UINT64 AxiAddr = params->AxiAddr, PciAddr = params->PciAddr;
        UINT8 NumPassBits;// = params->NumPassBits;
        DOUBLE power;

        PCIE_UtilityLog2((DOUBLE)params->Size, &power);
        NumPassBits = (UINT8)power - 1U; // log2(size) - 1

        /* Set up Region 1 as size 16KB.
        */
        ret_val = obj->setupObWrapperRegions (pd, Region,NumPassBits,(UINT32)(AxiAddr>>32U),(UINT32)(AxiAddr&0xFFFFFFFFU));
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("setupObWrapperRegions failed.");
        }

        /* Configure region 1 for a BAR on a device for memory access.
         * 1 - AXI wrapper region number.
         * 13 - Number of address bits to use(wrapper will add 1 to this value). This determines addressable memory range.
         * This number is limited by the aperture size of the core.
         * CPDREF_PCIE_ADDR_EP - Prefix bits to be added to address. This will determine which bar is matched.
         */
        ret_val = obj->setupObWrapperMemIoAccess (pd, Region, NumPassBits, PciAddr );
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("setupObWrapperMemIoAccess failed.");
        }

        /* Write the base addresses into BAR0. */
        ret_val = Impl_EnableBarAccess(pd,
                                       Bus,
                                       Dev,
                                       Func,
                                       Bar,
                                       (UINT32)(PciAddr>>32U),
                                       (UINT32)(PciAddr&0xFFFFFFFFU));
        if(ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("EnableBarAccess failed.");
        } else {
            offset = PCIER_GetBarOffset(Bar);
            ret_val = obj->doConfigRead(pd, bdf, offset, PCIE_4_BYTE, &pci_lo);
            if(ret_val == PCIE_ERR_SUCCESS) {
                ret_val = obj->doConfigRead(pd, bdf, offset + 4U, PCIE_4_BYTE, &pci_hi);
            }
            if(ret_val == PCIE_ERR_SUCCESS) {
                PCIE_UtilityPrintUInt5(" Enabled Bar[%d] :0x%x.%x ", Bar, pci_hi, pci_lo, 0, 0);
            }
        }
        Impl_ApiFlagSet(FLAG_PCIE_API_RC_READY);
    }
    return ret_val;
}

static PCIE_RpBarApertureSize PCIER_IbBitsToAperture(UINT8 bits)
{
    PCIE_RpBarApertureSize size;
    // NumPassBits - 1
    switch(bits) {
    case 11:
        size = PCIE_RP_APERTURE_SIZE_4K;
        break;
    case 12:
        size = PCIE_RP_APERTURE_SIZE_8K;
        break;
    case 13:
        size = PCIE_RP_APERTURE_SIZE_16K;
        break;
    case 14:
        size = PCIE_RP_APERTURE_SIZE_32K;
        break;
    case 15:
        size = PCIE_RP_APERTURE_SIZE_64K;
        break;
    case 16:
        size = PCIE_RP_APERTURE_SIZE_128K;
        break;
    case 17:
        size = PCIE_RP_APERTURE_SIZE_256K;
        break;
    case 18:
        size = PCIE_RP_APERTURE_SIZE_512K;
        break;
    case 19:
        size = PCIE_RP_APERTURE_SIZE_1M;
        break;
    case 20:
        size = PCIE_RP_APERTURE_SIZE_2M;
        break;
    case 21:
        size = PCIE_RP_APERTURE_SIZE_4M;
        break;
    case 22:
        size = PCIE_RP_APERTURE_SIZE_8M;
        break;
    case 23:
        size = PCIE_RP_APERTURE_SIZE_16M;
        break;
    case 24:
        size = PCIE_RP_APERTURE_SIZE_32M;
        break;
    case 25:
        size = PCIE_RP_APERTURE_SIZE_64M;
        break;
    case 26:
        size = PCIE_RP_APERTURE_SIZE_128M;
        break;
    case 27:
        size = PCIE_RP_APERTURE_SIZE_256M;
        break;
    case 28:
        size = PCIE_RP_APERTURE_SIZE_512M;
        break;
    case 29:
        size = PCIE_RP_APERTURE_SIZE_1G;
        break;
    case 30:
        size = PCIE_RP_APERTURE_SIZE_2G;
        break;
    case 31:
        size = PCIE_RP_APERTURE_SIZE_4G;
        break;
    case 32:
        size = PCIE_RP_APERTURE_SIZE_8G;
        break;
    case 33:
        size = PCIE_RP_APERTURE_SIZE_16G;
        break;
    case 34:
        size = PCIE_RP_APERTURE_SIZE_32G;
        break;
    case 35:
        size = PCIE_RP_APERTURE_SIZE_64G;
        break;
    case 36:
        size = PCIE_RP_APERTURE_SIZE_128G;
        break;
    case 37:
        size = PCIE_RP_APERTURE_SIZE_256G;
        break;
    // assign reset value
    default:
        size = PCIE_RP_APERTURE_SIZE_2G;
        break;
    }
    return size;
}
// Memory access from endpoint
UINT32 AmbaPCIER_Inbound(const PCIER_IB_PARAM_s *params)
{
    UINT32 ret_val;
    UINT32 flag = Impl_ApiFlagGet();

    if ((flag != FLAG_PCIE_API_RC_INIT) &&
        (flag != FLAG_PCIE_API_RC_START)) {
        ret_val = PCIE_ERR_FLOW;
    } else if (params == NULL || ((params->Size & (params->Size - 1)) != 0UL)) {
        ret_val = PCIE_ERR_ARG;
    } else {
        const PCIE_PrivateData *pd = PCIE_GetPrivateData();
        const PCIE_OBJ * obj = PCIE_GetInstance();
        UINT8 Region = 0;
        PCIE_RpBarNumber Bar = PCIE_RP_BAR_0;
        UINT64 MemAddr = params->MemAddr, PciAddr = params->PciAddr;
        UINT8 NumPassBits;
        DOUBLE power;
        PCIE_RpBarApertureSize aperture_size;

        PCIE_UtilityLog2((DOUBLE)params->Size, &power);
        NumPassBits = (UINT8)power - 1U; // log2(size) - 1
        aperture_size = PCIER_IbBitsToAperture(NumPassBits);

        /* Set inbound address for RP region 0 */
        ret_val = obj->setupIbRootPortAddrTransl(pd, Region, NumPassBits, MemAddr);
        if(ret_val != PCIE_ERR_SUCCESS ) {
            PCIE_UtilityPrint("Eror: setupIbRootPortAddrTransl()");
        }

        /* Changing Bar aperture size to allow sufficiant bits for writting Base address*/
        ret_val =  obj->setRootPortBarApertSetting(pd, Bar, aperture_size);
        if (ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("Eror: setRootPortBarApertSetting()");
        }

        /* Write the base addresses into BAR0. */
        ret_val = obj->enableRpMemBarAccess(pd,Bar,(UINT32)(PciAddr>>32U),(UINT32)(PciAddr&0xFFFFFFFFU));
        if(ret_val != PCIE_ERR_SUCCESS) {
            PCIE_UtilityPrint("Eror: enableRpMemBarAccess()");
        }
    }
    return ret_val;
}
