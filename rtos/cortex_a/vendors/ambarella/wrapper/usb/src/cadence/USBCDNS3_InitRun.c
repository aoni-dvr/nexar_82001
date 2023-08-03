/**
 *  @file USBCDNS3_InitRun.c
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
 *  @details USB driver for Cadence USB device/host controller.
 */

#include <AmbaTypes.h>
#include <AmbaKAL.h>
#include <AmbaUSB_Generic.h>
#include <AmbaUSB_ErrCode.h>
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>

static XHC_CDN_MEMORY_RES_s *xhc_memory_resource = NULL;

static XHCI_RUNTIME_REG_s *getXhcRuntimeRegPtrOf64Addr(UINT64 RegAddr) {
    XHCI_RUNTIME_REG_s *ptr_ret;
    const void *vptr = IO_UtilityU64AddrToPtr(RegAddr);
    USB_UtilityMemoryCopy(&ptr_ret, &vptr, sizeof(void *));
    return ptr_ret;
}

static XHCI_OPERATION_REG_s *getXhcOperationRegPtrOf64Addr(UINT64 RegAddr) {
    XHCI_OPERATION_REG_s *ptr_ret;
    const void *vptr = IO_UtilityU64AddrToPtr(RegAddr);
    USB_UtilityMemoryCopy(&ptr_ret, &vptr, sizeof(void *));
    return ptr_ret;
}

/**
 * Function wait for register value
 * @param reg address of checked register
 * @param mask bitmap of relevant bits taken for checking
 * @param waitFor value of which we wait for
 * @param timeout timeout value
 * @return 0 on success or USB_ERR_XHCI_CONTROLLER_TIMEOUT on timeout
 */
static UINT32 waitForReg(volatile const UINT32 *reg, UINT32 mask, UINT32 waitFor, UINT32 timeout) {
    UINT32 counter = timeout;
    UINT32 ret = 0;

    while ((USBCDNS3_Read32(reg) & mask) != waitFor) {

        // break loop when timeout value is 0
        if (counter == 0U) {
            //vDbgMsg(USBSSP_DBG_DRV, DBG_CRIT, "%s() timeout after %d\n", __func__, timeout);
            ret = USB_ERR_XHCI_CONTROLLER_TIMEOUT;
            break;
        }
        counter--;
        USB_UtilityTaskSleep(1);
    }
    return ret;
}

/**
 * Check if the memory is aligned
 * @retval 0 Both address and page are Aligned
 * @retval 1 Address is NOT aligned
 * @retval 2 Page is NOT aligned
 * @retval 3 Neither of address and page are aligned
 */
static UINT32 checkStructAlign(UINT64 startPhysAddr, SIZE_t byteSize,
        SIZE_t alignBytes, SIZE_t pageBytes) {

    // calculate end of physical address
    UINT64 endPhysAddr = (startPhysAddr + (UINT64) byteSize) - (UINT64) 1U;
    UINT32 result = 0U;

    // check alignment violation
    if ((alignBytes > 1U) && ((startPhysAddr & ((UINT64) alignBytes - 1U)) != 0U)) {
        result |= 1U;
    }

    // check page size violation
    if (pageBytes > 1U) {
        UINT64 mask = (UINT64)pageBytes - 1U;
        UINT64 page_no_bit_mask = ~mask;

        if ((startPhysAddr & page_no_bit_mask) != (endPhysAddr & page_no_bit_mask)) {
            result |= 2U;
        }
    }

    return result;

}

/**
 * Halt xHC.
 * @param[in] OpRegs XHCI Operation Registers
 */
static UINT32 cdn_xhc_halt(XHCI_OPERATION_REG_s *OpRegs) {

    UINT32 uret;

    UINT32 usbCmdReg = USBCDNS3_Read32(&OpRegs->USBCMD);

    if ((usbCmdReg & XHCI_USBCMD_R_S_MASK) == XHCI_USBCMD_R_S_MASK) {
        // Clear Run/Stop bit in USBCMD
        usbCmdReg &= ~((UINT32) XHCI_USBCMD_R_S_MASK);
        USBCDNS3_Write32(&OpRegs->USBCMD, usbCmdReg);
    }

    // wait until HCH in USBSTS is 1
    uret = waitForReg(&OpRegs->USBSTS, XHCI_USBSTS_HCH_MASK, XHCI_USBSTS_HCH_MASK, AMBA_XHCI_DEFAULT_TIMEOUT);
    if (uret != 0U) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "cdn_xhc_halt(): can't halt XHC.");
        uret = USB_ERR_XHCI_NO_HALT;
    }

    return uret;
}

/**
 * Reset xHC.
 * @param[in] OpRegs XHCI Operation Registers
 */
static UINT32 cdn_xhc_reset(XHCI_OPERATION_REG_s *OpRegs) {

    UINT32 uret = 0;
    UINT32 usbCmdReg = USBCDNS3_Read32(&OpRegs->USBCMD);
    UINT32 usbStsReg = USBCDNS3_Read32(&OpRegs->USBSTS);

    /* Software shall not reset XHC if XHC is not halted */
    if ((usbStsReg & XHCI_USBSTS_HCH_MASK) == XHCI_USBSTS_HCH_MASK) {
        usbCmdReg |= (UINT32) XHCI_USBCMD_HCRST_MASK;
        USBCDNS3_Write32(&OpRegs->USBCMD, usbCmdReg);
        uret = waitForReg(&OpRegs->USBCMD, XHCI_USBCMD_HCRST_MASK, 0U, AMBA_XHCI_DEFAULT_TIMEOUT);
        if (uret != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "cdn_xhc_reset(): can't reset XHC.");
            uret = USB_ERR_XHCI_RESET;
        }
    } else {
        uret = USB_ERR_XHCI_RESET_NO_HALT;
    }
    return uret;
}

/**
 * set slot number in controller
 * @param CapRegs XHCI Operation Registers
 * @param OpRegs XHCI Operation Registers
 */
static void cdn_xhc_slot_num_setup(const XHCI_CAPABILITY_REG_s *CapRegs, XHCI_OPERATION_REG_s *OpRegs)
{
    UINT32 max_slot_num = USBCDNS3_U32BitsRead(XHCI_HCSPARAMS1_MAXSLOTS_MASK, XHCI_HCSPARAMS1_MAXSLOTS_SHIFT, CapRegs->HCSPARAMS1);

    if (max_slot_num > AMBA_XHCI_MAX_DEV_SLOT_NUM) {
        max_slot_num = AMBA_XHCI_MAX_DEV_SLOT_NUM;
    }

    // always enable maximum slots
    USBCDNS3_Write32(&OpRegs->CONFIG, max_slot_num);
}

/**
 * Program the device context base array fields
 * @param res driver resources
 * @return CDN_EOK on success, error code elsewhere
 */
static void cdn_xhc_dcbaa_setup(XHCI_OPERATION_REG_s *OpRegs) {

    UINT64 dcbaap_sfr_val;

    xhc_memory_resource->Dcbaa.DeviceContextBaseAddr[0] = USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(xhc_memory_resource->OutputContext.SlotContext));

    // Program the Device Context Base Address Array Pointer (DCBAAP) for the scratchpad (slot0)
    dcbaap_sfr_val = USBCDNS3_PtrToU64PhyAddr(&xhc_memory_resource->Dcbaa.ScratchPadBufferArrayBaseAddr);
    //dcbaap_sfr_val = xhc_dcbaa.ScratchPadBufferArrayBaseAddr;
    //CPS_MemoryBarrier();

    USBCDNS3_Write64(&OpRegs->DCBAAP, dcbaap_sfr_val);
}

/**
 * Setup of scratch pad buffers
 * @param res driver resources
 * @return CDN_EOK if setup successful, error code elsewhere
 */
static UINT32 cdn_xhc_scratchpad_setup(const XHCI_CAPABILITY_REG_s *CapRegs) {

    UINT16 i;
    UINT32 uret = 0;

    // check number of scratch pad buffers
    UINT32 max_scratch_buf = USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_MAXSPBUFHI_MASK, XHCI_HCSPARAMS2_MAXSPBUFHI_SHIFT, CapRegs->HCSPARAMS2);
    max_scratch_buf = max_scratch_buf << 5U;
    max_scratch_buf += USBCDNS3_U32BitsRead(XHCI_HCSPARAMS2_MAXSPBUFLO_MASK, XHCI_HCSPARAMS2_MAXSPBUFLO_SHIFT, CapRegs->HCSPARAMS2);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "cdn_xhc_scratchpad_setup(): Max Scratchpad Buffers = %d", max_scratch_buf, 0, 0, 0, 0);

    if (max_scratch_buf > AMBA_XHCI_SCRATCHPAD_BUFF_NUM) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_scratchpad_setup(): Max Scratchpad Buffers mismatch %d, expected %d", max_scratch_buf, AMBA_XHCI_SCRATCHPAD_BUFF_NUM, 0, 0, 0);
        uret = USB_ERR_XHCI_SCRATCHPAD_MISMATCH;
    }

    if (uret == 0U) {
        // set pointers to all buffers
        for (i = 0; i < max_scratch_buf; i++) {
            xhc_memory_resource->Scratchpad[i] = USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&xhc_memory_resource->ScratchpadPool[i * AMBA_XHCI_PAGE_SIZE]));
        }
        xhc_memory_resource->Scratchpad[max_scratch_buf] = 0;

        // set first element of DCBAA to 0 if no scratch pad buffers
        if (max_scratch_buf == 0U) {
            xhc_memory_resource->Dcbaa.ScratchPadBufferArrayBaseAddr = 0U;
        } else {
            xhc_memory_resource->Dcbaa.ScratchPadBufferArrayBaseAddr = USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(xhc_memory_resource->Scratchpad));
        }
    }
    return uret;
}

static UINT32 cdn_xhc_setup(const XHCI_CAPABILITY_REG_s *CapRegs, XHCI_OPERATION_REG_s *OpRegs)
{
    UINT32 uret = 0;
    UINT32 csz_value;

    cdn_xhc_slot_num_setup(CapRegs, OpRegs);

    csz_value = USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_CSZ_MASK, XHCI_HCCPARAMS1_CSZ_SHIFT, CapRegs->HCCPARAMS1);

    // check context size (should be 1 (64 byte Context data structures))
    if (csz_value != AMBA_XHCI_EXTENDED_CONTEXT) {
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_setup(): XHC Context Size (%d) should be 1, HCCPARAMS1 = 0x%X", csz_value, CapRegs->HCCPARAMS1, 0, 0, 0);
        uret = USB_ERR_XHCI_CTX_SIZE_MISMATCH;
    }

    // check page size
    if (uret == 0U) {
        UINT32 page_size = USBCDNS3_Read32(&OpRegs->PAGESIZE);
        if (page_size != AMBA_XHCI_PAGE_SIZE_IDX) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_setup(): XHC Page Size should be %d, not %d, HCCPARAMS1 = 0x%X",
                AMBA_XHCI_PAGE_SIZE_IDX,
                page_size,
                CapRegs->HCCPARAMS1,
                0, 0);
            uret = USB_ERR_XHCI_PAGE_SIZE_MISMATCH;
        }
    }

    if (uret == 0U) {
        uret = cdn_xhc_scratchpad_setup(CapRegs);
    }

    if (uret == 0U) {
        cdn_xhc_dcbaa_setup(OpRegs);
    }

    return uret;
}

/**
 * Program interrupters
 * @param res driver resources
 */
static void cdn_xhc_rings_intr_enable(const XHC_CDN_DRV_RES_s *DrvRes) {

    UINT64  erdp_sfr_val;
    UINT64  erstba_sfr_val;

    erstba_sfr_val = USBCDNS3_PtrToU64PhyAddr(DrvRes->XhciMemResources->EventRingSegmentEntry);

    //CPS_MemoryBarrier();

    // Program the Interrupter Event Ring Segment Table Size (ERSTSZ)
    USBCDNS3_Write32(&DrvRes->Registers.xhciRuntime->INTArray[0].ERSTSZ, 1); // one event ring

    // Program the Interrupter Event Ring Dequeue Pointer (ERDP)
    erdp_sfr_val = USBCDNS3_PtrToU64PhyAddr(&(DrvRes->EventPtr->DWord0));
    USBCDNS3_Write64(&DrvRes->Registers.xhciRuntime->INTArray[0].ERDP, erdp_sfr_val);

    // Program the Interrupter Event Ring Segment Table Base Address (ERSTBA)
    USBCDNS3_Write64(&DrvRes->Registers.xhciRuntime->INTArray[0].ERSTBA, erstba_sfr_val);

    // Initializing the Interval field of the Interrupt Moderation register
    // 2021/06/21 pollo:
    //     DO NOT set Interrupt Moderation Interval (IMODI) to 0
    //     Otherwise EVENT Ring would be broken and USB_ERR_XHCI_EVT_BABBLE reported
    USBCDNS3_Write32(&DrvRes->Registers.xhciRuntime->INTArray[0].IMOD, 4);

    // Enable the Interrupter by writing a '1' to the Interrupt Enable (IE)
    USBCDNS3_Write32(&DrvRes->Registers.xhciRuntime->INTArray[0].IMAN, XHCI_IMAN0_IE_MASK);
}

/**
 * initialize commend ring
 * @param res driver resources
 */
static void cdn_xhc_cmd_ring_init(XHC_CDN_DRV_RES_s *DrvRes)
{
    if (DrvRes != NULL) {

        XHC_CDN_DRV_RES_s *res = DrvRes;

        // set start of ring
        res->CommandQueue.RingArray = res->XhciMemResources->EpRingPool;

        // set toggle bit
        res->CommandQueue.ToogleBit = 1;

        // reset dequeue and enqueue pointer
        res->CommandQueue.DequeuePtr = res->CommandQueue.RingArray;
        res->CommandQueue.EnqueuePtr = res->CommandQueue.RingArray;
    }
}

/**
 * initialize endpoints rings
 * @param res driver resources
 */
static void cdn_xhc_ep_rings_init(XHC_CDN_DRV_RES_s *DrvRes) {

    UINT32 i;
    XHC_CDN_PRODUCER_QUEUE_s *ep;

    if (DrvRes != NULL) {
        // initialize all software endpoints
        for (i = 1U; i < (AMBA_XHCI_MAX_EP_CONTEXT_NUM + AMBA_XHCI_EP_CONT_OFFSET); i++) {

            // index 1 is a default endpoint
            if (i == 1U) {
                ep = &DrvRes->Ep0Queue;
            } else {
                // no default endpoint are organized in container
                ep = &DrvRes->EpQueueArray[i];
            }

            // set ring and enqueue/dequeue pointers
            ep->RingArray = &DrvRes->XhciMemResources->EpRingPool[AMBA_XHCI_PRODUCER_QUEUE_SIZE * i];
            ep->ToogleBit = 1;
            ep->EnqueuePtr = ep->RingArray;
            ep->DequeuePtr = ep->RingArray;

            USB_UtilityPrintUInt5(USB_PRINT_FLAG_L1,
                                  "cdn_xhc_ep_rings_init(): <%d> Endpoint context[%d] transfer ring v-address: 0x%X phy-addr: 0x%X.",
                                  DrvRes->instanceNo,
                                  i,
                                  IO_UtilityPtrToU32Addr(ep->RingArray),
                                  0,
                                  0);
        }
    }
}

/**
 * Initialization of SSPDriverResourcesT object: Setup of rings and interrupts.
 *
 * @param[in] res driver resources
 * @return CDN_EINVAL when unsuccessful
 * @return CDN_EOK if no errors
 */
static UINT32 cdn_xhc_rings_intr_init(XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT32  uret = 0;
    UINT64  crcr_sfr_val;

    if (DrvRes != NULL) {

        XHC_CDN_DRV_RES_s *res = DrvRes;
        UINT32 func_uret;

        // initialize software command ring
        cdn_xhc_cmd_ring_init(DrvRes);

        // get start address of command ring
        crcr_sfr_val = USBCDNS3_PtrToU64PhyAddr(&res->CommandQueue.RingArray[0].DWord0);

        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_INFO, "cdn_xhc_rings_intr_init(): crcr_sfr_val = 0x%X", crcr_sfr_val);

        USB_UtilityCacheFlushUInt8((UINT8 *)res->XhciMemResources->EventPool, (UINT32)sizeof(XHCI_RING_ELEMENT_s) * AMBA_XHCI_EVENT_QUEUE_SIZE);
        USB_UtilityCacheFlushUInt8((UINT8 *)res->XhciMemResources->EpRingPool, (UINT32)sizeof(XHCI_RING_ELEMENT_s) * AMBA_XHCI_PRODUCER_QUEUE_SIZE * (AMBA_XHCI_MAX_EP_CONTEXT_NUM + 2U));

        // set event pointer to start of allocated buffer
        res->EventPtr = res->XhciMemResources->EventPool;

        // check if pointers are allocated according to spec requirements
        func_uret = checkStructAlign(crcr_sfr_val, sizeof (XHCI_RING_ELEMENT_s) * AMBA_XHCI_PRODUCER_QUEUE_SIZE, XHCI_RING_ALIGNMENT, XHCI_RING_BOUNDARY);
        if (func_uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_rings_intr_init(): command ring memory is not aligned %d.",
                func_uret,
                0, 0, 0, 0);
            uret = USB_ERR_XHCI_NO_CMD_RING_ALIGN;
        }

        func_uret = checkStructAlign(USBCDNS3_PtrToU64PhyAddr(&res->EventPtr->DWord0), sizeof (XHCI_RING_ELEMENT_s) * AMBA_XHCI_EVENT_QUEUE_SIZE, XHCI_RING_ALIGNMENT, XHCI_RING_BOUNDARY);
        if (func_uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_rings_intr_init(): event ring memory is not aligned %d.",
                func_uret,
                0, 0, 0, 0);
            uret = USB_ERR_XHCI_NO_EVT_RING_ALIGN;
        }

        func_uret = checkStructAlign(USBCDNS3_PtrToU64PhyAddr(res->XhciMemResources->EventRingSegmentEntry), sizeof (UINT64) * (2U), XHCI_ERST_ALIGNMENT, XHCI_ERST_BOUNDARY);
        if (func_uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_rings_intr_init(): event ring segment table memory is not aligned %d.",
                func_uret,
                0, 0, 0, 0);
            uret = USB_ERR_XHCI_NO_ERST_ALIGN;
        }

        if (uret == 0U) {

            // Define the Command Ring Dequeue Pointer by programming the Command Ring Control Register
            USBCDNS3_Write64(&res->Registers.xhciOperational->CRCR, (UINT64) (crcr_sfr_val | res->CommandQueue.ToogleBit));

            // initialize endpoint rings
            cdn_xhc_ep_rings_init(res);

            // Allocate the Event Ring Segment Table (ERST) (section 6.5). Initialize ERST table entries
            // to point to and to define the size (in TRBs) of the respective Event Ring Segment.
            res->EventToogleBit = 1;
            res->XhciMemResources->EventRingSegmentEntry[0] = USBCDNS3_CpuToLe64(USBCDNS3_PtrToU64PhyAddr(&res->EventPtr->DWord0));
            res->XhciMemResources->EventRingSegmentEntry[1] = USBCDNS3_CpuToLe64((UINT64)AMBA_XHCI_EVENT_QUEUE_SIZE);

            USB_UtilityCacheFlushUInt8((UINT8 *)res->XhciMemResources->EventRingSegmentEntry, (UINT32)sizeof(UINT64) * AMBA_XHCI_INTERRUPTER_COUNT * 2U);

            // initialize interrupters
            cdn_xhc_rings_intr_enable(res);
        }
    } else {
        uret = USB_ERR_PARAMETER_NULL;
    }
    return uret;
}

/**
 * Clear port bits
 *
 * @param[in] portsc port control register
 * @param[in] portbits to clear
 */
static void devPortscTestClear(UINT32 * portsc, UINT32 portBits) {
    UINT32 temp;

    /* test and clear port bits */
    temp = USBCDNS3_Read32(portsc);
    if ((temp & portBits) != 0U) {
        temp &= XHCI_DEV_USB23_PORT_RO;
        temp &= XHCI_DEV_USB23_PORT_RWS;
        temp |= portBits;
        USBCDNS3_Write32(portsc, temp);
    }
}

/**
 * Set link state.
 *
 * @param[in] portsc port control register
 * @param[in] linkstate
 */
static void devPortscSetLinkState(UINT32 * portsc, UINT32 linkState) {
    UINT32 temp;

    /* set link state */
    temp = USBCDNS3_Read32(portsc);
    temp &= XHCI_DEV_USB23_PORT_RO;
    temp &= XHCI_DEV_USB23_PORT_RWS;
    temp &= ~XHCI_PORTSC_PLS_MASK;
    temp |= XHCI_PORTSC_LWS_MASK | linkState;
    USBCDNS3_Write32(portsc, temp);
}

/**
 * Initialization of SSPDriverResourcesT object: Set up port control registers.
 *
 * @param[in] res driver resources
 */
/* parasoft-begin-suppress MISRA2012-RULE-8_13_a "Pass parameter extCap3xPortRegs with const specifier, DRV-3806" */
static void setPortControlRegisters(const XHC_CDN_DRV_RES_s *DrvRes,  UINT32 *extCap3xPortRegs) {

    UINT32 *extCap3xPortMdReg    = &extCap3xPortRegs[1];
    UINT32  extCap3xPortMdRegVal = USBCDNS3_Read32(extCap3xPortMdReg);
    XHCI_PORT_s *port_regs;

    port_regs = &DrvRes->Registers.xhciPortControl[USBCDNS3_DEV_MODE_32_PORT_IDX];

    // clear pending interrupts and power down USB 3 port
    devPortscTestClear(&port_regs->PORTSC, XHCI_DEV_PORTSC_CHANGE_BITS | XHCI_PORTSC_PED_MASK);

    // disable port by setting PED = 1
    USBCDNS3_Write32(&port_regs->PORTSC, XHCI_PORTSC_PED_MASK);
    // wait until the port is disabled
    (void) waitForReg(&port_regs->PORTSC, XHCI_PORTSC_PED_MASK, 0, AMBA_XHCI_DEFAULT_TIMEOUT);

    if (DrvRes->UsbModeFlag == USBCDNS3_DEV_SPEED_HS) {
        // disable upstream and downstream for the port (USB High-Speed support?)
        extCap3xPortMdRegVal = USBCDNS3_U32BitSet(CDN_CFG_3XPORT_DIS_UFP_WIDTH,
                                                   CDN_CFG_3XPORT_DIS_UFP_MASK,
                                                   CDN_CFG_3XPORT_DIS_UFP_WOCLR,
                                                   extCap3xPortMdRegVal);
        extCap3xPortMdRegVal = USBCDNS3_U32BitSet(CDN_CFG_3XPORT_DIS_DFP_WIDTH,
                                                   CDN_CFG_3XPORT_DIS_DFP_MASK,
                                                   CDN_CFG_3XPORT_DIS_DFP_WOCLR,
                                                   extCap3xPortMdRegVal);
        USBCDNS3_Write32(extCap3xPortMdReg, extCap3xPortMdRegVal);
    } else {
        // enable upstream and downstream for the port (USB Super-Speed support?)
        extCap3xPortMdRegVal = USBCDNS3_Read32(extCap3xPortMdReg);
        extCap3xPortMdRegVal = USBCDNS3_U32BitClear(CDN_CFG_3XPORT_DIS_UFP_WIDTH,
                                                     CDN_CFG_3XPORT_DIS_UFP_MASK,
                                                     CDN_CFG_3XPORT_DIS_UFP_WOSET,
                                                     CDN_CFG_3XPORT_DIS_UFP_WOCLR,
                                                     extCap3xPortMdRegVal);
        extCap3xPortMdRegVal = USBCDNS3_U32BitClear(CDN_CFG_3XPORT_DIS_DFP_WIDTH,
                                                     CDN_CFG_3XPORT_DIS_DFP_MASK,
                                                     CDN_CFG_3XPORT_DIS_DFP_WOSET,
                                                     CDN_CFG_3XPORT_DIS_DFP_WOCLR,
                                                     extCap3xPortMdRegVal);

        if (DrvRes->UsbModeFlag == USBCDNS3_DEV_SPEED_SS) {
            // disable Super-Speed-Plus support
            extCap3xPortMdRegVal = USBCDNS3_U32BitClear(CDN_CFG_3XPORT_SSP_SUP_WIDTH,
                                                         CDN_CFG_3XPORT_SSP_SUP_MASK,
                                                         CDN_CFG_3XPORT_SSP_SUP_WOSET,
                                                         CDN_CFG_3XPORT_SSP_SUP_WOCLR,
                                                         extCap3xPortMdRegVal);
        } else {
            // enable Super-Speed-Plus support
            extCap3xPortMdRegVal = USBCDNS3_U32BitSet(CDN_CFG_3XPORT_SSP_SUP_WIDTH,
                                                       CDN_CFG_3XPORT_SSP_SUP_MASK,
                                                       CDN_CFG_3XPORT_SSP_SUP_WOCLR,
                                                       extCap3xPortMdRegVal);
        }
        USBCDNS3_Write32(extCap3xPortMdReg, extCap3xPortMdRegVal);
        // Set USB 3 PORT TO RX-detect
        devPortscSetLinkState(&port_regs->PORTSC, (XHCI_PLS_RXDETECT << XHCI_PORTSC_PLS_SHIFT));
    }
}

/**
 * Get Pointer to the Extended capability register
 * @param DrvRes Driver resources
 * @param CapId Capability ID
 * @return Pointer to the first register of the specified capability ID
 */
static UINT32 *cdn_xhc_ext_cap_ptr_get(const XHC_CDN_DRV_RES_s * const DrvRes, UINT8 CapId) {

    UINT32 ext_cap_idx;
    XHC_CDN_EXT_CAP_s const *extCapRegs = &DrvRes->Registers.xhciExtCaps;
    UINT32 *regPtr = NULL;

    for (ext_cap_idx = 0; ext_cap_idx < extCapRegs->extCapsCount; ext_cap_idx++) {
        if (extCapRegs->extCaps[ext_cap_idx].capId == CapId) {
            regPtr = extCapRegs->extCaps[ext_cap_idx].firstCapSfrPtr;
        }
    }

    return regPtr;
}

/**
 * Initialization of SSPDriverResourcesT object: Set up port control registers.
 *
 * @param[in] DrvRes driver resources
 * @retval CDN_EINVAL when unsuccessful
 * @retval 0 Success
 */
static UINT32 cdn_xhc_port_ctrl_init(const XHC_CDN_DRV_RES_s *DrvRes)
{

    UINT32 uret = 0;
    XHCI_PORT_s *port_regs;

    // In device mode set port PLS=RxDetect, LWS=1
    // For USB3.x active port is 2
    // cadence specific code
    if (DrvRes->DeviceModeFlag == USBCDNS3_MODE_DEVICE) {

        UINT32 *extCap3xPortRegs = cdn_xhc_ext_cap_ptr_get(DrvRes, CDN_D_XEC_CFG_3XPORT_CAP_ID);

        //vDbgMsg(USBSSP_DBG_DRV, DBG_HIVERB, "<%d> SSP works in DEVICE mode\n", res->instanceNo);

        if (extCap3xPortRegs != NULL) {
            setPortControlRegisters(DrvRes, extCap3xPortRegs);
        }

        port_regs = &DrvRes->Registers.xhciPortControl[USBCDNS3_DEV_MODE_20_PORT_IDX];

        devPortscTestClear(&port_regs->PORTSC,
                           XHCI_DEV_PORTSC_CHANGE_BITS);
        USBCDNS3_Write32(&port_regs->PORTSC, XHCI_PORTSC_PED_MASK);
        uret = waitForReg(&port_regs->PORTSC, XHCI_PORTSC_PED_MASK, 0, AMBA_XHCI_DEFAULT_TIMEOUT);

        if (uret != 0U) {
            USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "cdn_xhc_port_ctrl_init(): can't wait USB2 PORT disabled.",
                DrvRes->instanceNo,
                0, 0, 0, 0);
        }

        // Set USB 2 PORT TO RX-detect
        devPortscSetLinkState(&port_regs->PORTSC, XHCI_PLS_RXDETECT << XHCI_PORTSC_PLS_SHIFT);

    } else {
        // do nothing in HOST mode
        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "cdn_xhc_port_ctrl_init(): [%d] SSP works in HOST mode.",
            DrvRes->instanceNo,
            0, 0, 0, 0);
    }

    return uret;
}

/**
 * Initialization of SSPDriverResourcesT object: Enable xHC.
 *
 * @param[in] DrvRes driver resources
 * @return CDN_EINVAL when unsuccessful
 * @return 0 if no errors
 */
static void cdn_xhc_enable(const XHC_CDN_DRV_RES_s *DrvRes) {

    // Write the USBCMD (5.4.1) to turn the host controller ON via setting the Run/Stop (R/S) bit to 1.
    // Enable system bus interrupt generation by writing a '1' to the Interrupter Enable (INTE)

    UINT32 flags = XHCI_USBCMD_HSEE_MASK | XHCI_USBCMD_INTE_MASK | XHCI_USBCMD_R_S_MASK;
    UINT32 etc;

    etc = USBCDNS3_U32BitsRead(XHCI_HCCPARAMS2_ETC_MASK, XHCI_HCCPARAMS2_ETC_SHIFT, DrvRes->Registers.xhciCapability->HCCPARAMS2);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> hccparams2:(0x%08X) ete(0x%X)", DrvRes->instanceNo, DrvRes->Registers.xhciCapability->HCCPARAMS2, etc, 0, 0);

    // Check for device mode
    if (DrvRes->DeviceModeFlag == USBCDNS3_MODE_DEVICE) {
        flags |= XHCI_D_USBCMD_DEVEN_MASK;
    }

    // configure ETE
    if ((DrvRes->ExtendedTBCMode == 1U) && (etc != 0U)) {
        flags |= XHCI_USBCMD_ETE_MASK;
    }

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "<%d> Starting xHC with flags: (0x%08X)\n", DrvRes->instanceNo, flags, 0, 0, 0);

    USBCDNS3_Write32(&DrvRes->Registers.xhciOperational->USBCMD, flags);
}

UINT32 USBCDNS3_XhcStop(void)
{
    UINT32 uret = 0;
    XHCI_OPERATION_REG_s        *op_regs  = USBCDNS3_XhcOperationRegGet();

    if (op_regs == NULL) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcStop(): op register is NULL.");
        uret = USB_ERR_NO_INIT;
    } else {

        /* Ensure that XHC is halted */
        uret = cdn_xhc_halt(op_regs);

        if (uret == 0U) {
            /* reset XHC */
            uret = cdn_xhc_reset(op_regs);
        }
    }
    return uret;
}

/* initialize controller and let it run */
UINT32 USBCDNS3_XhcInitRun(UINT32 ControllerMode)
{
    UINT32 uret = 0;
    XHCI_OPERATION_REG_s        *op_regs  = USBCDNS3_XhcOperationRegGet();
    const XHCI_CAPABILITY_REG_s *cap_regs = USBCDNS3_XhcCapabilityRegGet();
    XHC_CDN_DRV_RES_s           *drv_res  = USBCDNS3_DrvInstanceGet();

    USB_UtilityPrint(USB_PRINT_FLAG_INFO, "USBCDNS3_XhcInitRun(): start.");

    USBCDNS3_ControllerUp(ControllerMode);

    if (op_regs == NULL) {
        USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcInitRun(): op register is NULL.");
        uret = USB_ERR_NO_INIT;
    } else {

        // wait until the Controller Not Ready (CNR) flag in the USBSTS is '0'
        uret = waitForReg(&op_regs->USBSTS, XHCI_USBSTS_CNR_MASK, 0U, AMBA_XHCI_DEFAULT_TIMEOUT);
        if (uret != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcInitRun(): can't wait until Controller Ready.");
            uret = USB_ERR_XHCI_NOT_READY;
        }

        if (uret == 0U) {
            /* Ensure that XHC is halted */
            uret = cdn_xhc_halt(op_regs);
        }

        if (uret == 0U) {
            /* reset XHC */
            uret = cdn_xhc_reset(op_regs);
        }

        if (uret == 0U) {
            uret = cdn_xhc_setup(cap_regs, op_regs);
        }

        if (uret == 0U) {
            // setup rings (control, transfer, event) and interrupts
            uret = cdn_xhc_rings_intr_init(drv_res);
        }

        if (uret == 0U) {
            uret = cdn_xhc_port_ctrl_init(drv_res);
        }

        if (uret == 0U) {

            // Enable interrupts and place Controller in Run state.
            cdn_xhc_enable(drv_res);

            //  Wait for HCHalted bit to be set in USBSTS to become 0
            uret = waitForReg(&op_regs->USBSTS, XHCI_USBSTS_HCH_MASK, 0, AMBA_XHCI_DEFAULT_TIMEOUT);

            if (uret != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcInitRun(): can't wait Controller Running.");
            }
        }
    }

    return uret;
}

/**
 * initialize extension capabilities
 * @param DrvRes driver resources
 */
static void cdn_xhc_ext_cap_init(XHC_CDN_DRV_RES_s *DrvRes)
{
    UINT8                numOfExtCaps;
    UINT8                nextCapPtr;
    UINT32              *extCapsBaseAddr;
    UINT32              *extCapSfrIter;
    XHC_CDN_SFR_s       *regs = &DrvRes->Registers;
    XHC_CDN_EXT_CAP_ELEMENT_s *extCapElemIter;

    USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_INFO, "cdn_xhc_ext_cap_init(): start, Addr = 0x%X", IO_UtilityPtrToU64Addr(regs->xhciExtCaps.extCapsBaseAddr));

    // Reading USBLEGSUP and USBLEGCTLSTS

    extCapsBaseAddr = regs->xhciExtCaps.extCapsBaseAddr;
    regs->xhciExtCaps.usbLegSup = extCapsBaseAddr[0];
    regs->xhciExtCaps.usbLegCtlSts = extCapsBaseAddr[1];

    numOfExtCaps = 0;

    extCapElemIter = &(regs->xhciExtCaps.extCaps[0]);
    // Read next pointer, it is 4-byte based offset.
    nextCapPtr = (UINT8)((regs->xhciExtCaps.usbLegSup & 0x0FF00UL) >> 8UL);
    extCapSfrIter = &extCapsBaseAddr[nextCapPtr];

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "cdn_xhc_ext_cap_init(): nextCapPtr = %d, usbLegSup = 0x%X", nextCapPtr, regs->xhciExtCaps.usbLegSup, 0, 0, 0);

    while ((numOfExtCaps < AMBA_XHCI_MAX_EXT_CAPS_COUNT) && (nextCapPtr > 0U)) {
        UINT8 cap_id;

        // Reading first DWORD of ExtCap and decoding next capability pointer / capability ID
        UINT32 firstExtCapDword = extCapSfrIter[0];
        // Next Extended Capabilities pointer is always in the same place
        nextCapPtr = (UINT8)((firstExtCapDword & 0x0FF00UL) >> 8UL);
        // Capability ID is always in the same place
        cap_id = (UINT8)(firstExtCapDword & 0x0FFUL);

        USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "cdn_xhc_ext_cap_init(): cap_id %d(0x%X), nextCapPtr = %d", cap_id, cap_id, nextCapPtr, 0, 0);

        if (cap_id > 0U) {
            // Storing first DWORD of data, Capability ID and pointer to first SFR
            extCapElemIter->firstDwordVal = firstExtCapDword;
            extCapElemIter->capId = cap_id;
            extCapElemIter->firstCapSfrPtr = extCapSfrIter;
        }

        // Advancing number of capabilities and moving pointers
        numOfExtCaps++;
        extCapElemIter++;
        extCapSfrIter = &extCapSfrIter[nextCapPtr];
    }

    // Updating Ext. Caps number
    regs->xhciExtCaps.extCapsCount = numOfExtCaps;
}

static void cdn_xhc_cmd_complete_func(XHC_CDN_DRV_RES_s *Arg, UINT32 SlotID, UINT32 EpIndex, UINT32 Status, const XHCI_RING_ELEMENT_s *EventPtr, UINT8 *Buffer, UINT32 ActualLength)
{
    (void)SlotID;
    (void)EpIndex;
    (void)Status;
    (void)EventPtr;
    (void)ActualLength;

    AmbaMisra_TouchUnused(Buffer);

    USB_UtilityPrintUInt5(USB_PRINT_FLAG_INFO, "cdn_xhc_cmd_complete_func(): SlotID %d, Status %d", SlotID, Status, 0, 0, 0);

    if (Arg != NULL) {
        // we have to save current Event data for later use
        Arg->SavedCommandEvent = *EventPtr;
        if (USB_UtilitySemaphoreGive(&Arg->SemaphoreCommandQueue) != 0U) {
            USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "cdn_xhc_cmd_complete_func(): failed to give semaphore");
        }
    }
}

// initialize XHC controller parameters, like:
//     memory for command/event/endpoint rings
//     runtime/operation memory addresses
//     xhci capabilities
void USBCDNS3_XhcParameterInit(UINT32 ControllerMode)
{
    static UINT32 init = 0;

    if (init == 0U) {

        // Initialize the UDC instance (set all 0)
        USBCDNS3_UdcInstanceReset(1);

        init = 1;

    } else {

        // do not reset all. Resources should not be deleted.
        USBCDNS3_UdcInstanceReset(0);

        // Initialize XHC memory
        USB_UtilityMemorySet(xhc_memory_resource, 0, sizeof(XHC_CDN_MEMORY_RES_s));

    }

    {
        UINT64 offset_operation;
        UINT64 offset_runtime;
        UINT64 offset_doorbell;
        UINT64 offset_ext_cap;
        UINT32 cap_length;
        UINT32 maxDeviceSlot;
        UINT64 xhci_base_addr = 0;
        const void   *void_ptr;
        UINT32       *uint32_ptr;
        const USB_HARDWARE_SETTING_s *hws = USB_UtilityHWInfoQuery();

        XHC_CDN_DRV_RES_s     *cdn_drv_res = USBCDNS3_DrvInstanceGet();
        XHCI_CAPABILITY_REG_s *xHCCaps = &cdn_drv_res->RegistersQuickAccess.xHCCaps;
        const XHC_CDN_SFR_s   *regs = &cdn_drv_res->Registers;

        {
            UINT8 *ptr;
            UINT32 size;

            if (USBCDNS3_MODE_DEVICE == ControllerMode) {
                if (USBD_SystemCtrlrMemGet(&ptr, &size) == 0U) {
                    if (size > sizeof(XHC_CDN_MEMORY_RES_s)) {
                        //xhc_memory_resource = (XHC_CDN_MEMORY_RES_s *)ptr;
                        AmbaMisra_TypeCast(&xhc_memory_resource, &ptr);
                    }
                } else {
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcParameterInit(): no device memory");
                    USB_UtilityAssert();
                }
            } else if (USBCDNS3_MODE_HOST == ControllerMode) {
                if (USBH_SystemCtrlrMemGet(&ptr, &size) == 0U) {
                    if (size > sizeof(XHC_CDN_MEMORY_RES_s)) {
                        //xhc_memory_resource = (XHC_CDN_MEMORY_RES_s *)ptr;
                        AmbaMisra_TypeCast(&xhc_memory_resource, &ptr);
                    }
                } else {
                    USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcParameterInit(): no host memory");
                    USB_UtilityAssert();
                }
            } else {
                USB_UtilityPrintUInt5(USB_PRINT_FLAG_ERROR, "USBCDNS3_XhcParameterInit(): unknown mode %d", ControllerMode, 0, 0, 0, 0);
                USB_UtilityAssert();
            }
        }

        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_INFO, "xhc_memory_resource = 0x%X", IO_UtilityPtrToU64Addr(xhc_memory_resource));

        // create semaphore
        {
            if (USB_UtilitySemaphoreCreate(&cdn_drv_res->SemaphoreCommandQueue, 0) != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "can't create semaphore for command queue");
            }
            if (USB_UtilitySemaphoreCreate(&cdn_drv_res->SemaphoreEp0Transfer, 0) != 0U) {
                USB_UtilityPrint(USB_PRINT_FLAG_ERROR, "can't create semaphore for EP0 queue");
            }
        }

        // Initialize and asign XHC memory
        USB_UtilityMemorySet(xhc_memory_resource, 0, (UINT32)sizeof(XHC_CDN_MEMORY_RES_s));
        USB_UtilityCacheFlushUInt8((UINT8 *)xhc_memory_resource, (UINT32)sizeof(XHC_CDN_MEMORY_RES_s));
        cdn_drv_res->XhciMemResources = xhc_memory_resource;
        cdn_drv_res->InputContext = &xhc_memory_resource->InputContext;
        cdn_drv_res->Ep0Buff = xhc_memory_resource->Ep0Buffer;

        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_INFO, "OutputContext = 0x%X", IO_UtilityPtrToU64Addr(&xhc_memory_resource->OutputContext));
        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_INFO, "InputContext  = 0x%X", IO_UtilityPtrToU64Addr(&xhc_memory_resource->InputContext));

        if (USBCDNS3_MODE_DEVICE == ControllerMode) {
            xhci_base_addr = hws->Udc32.BaseAddress;
        } else if (USBCDNS3_MODE_HOST == ControllerMode) {
            xhci_base_addr = hws->Xhci.BaseAddress;
            // register command complete function for Host mode only
            cdn_drv_res->CommandQueue.CompleteFunc = cdn_xhc_cmd_complete_func;
        } else {
            // impossible to be here
            // pass misra-c check
        }

        // assign XHC Capability registers
        //cdn_drv_res->regs.xhciCapability = (XHCI_CAPABILITY_REG_s *)xhci_base_addr;
        AmbaMisra_TypeCast(&cdn_drv_res->Registers.xhciCapability, &xhci_base_addr);

        // read and save Capability information
        xHCCaps->LENGTH_VER = USBCDNS3_Read32(&regs->xhciCapability->LENGTH_VER);
        xHCCaps->HCSPARAMS1 = USBCDNS3_Read32(&regs->xhciCapability->HCSPARAMS1);
        xHCCaps->HCSPARAMS2 = USBCDNS3_Read32(&regs->xhciCapability->HCSPARAMS2);
        xHCCaps->HCSPARAMS3 = USBCDNS3_Read32(&regs->xhciCapability->HCSPARAMS3);
        xHCCaps->DBOFF      = USBCDNS3_Read32(&regs->xhciCapability->DBOFF);
        xHCCaps->RTSOFF     = USBCDNS3_Read32(&regs->xhciCapability->RTSOFF);
        xHCCaps->HCCPARAMS1 = USBCDNS3_Read32(&regs->xhciCapability->HCCPARAMS1);
        xHCCaps->HCCPARAMS2 = USBCDNS3_Read32(&regs->xhciCapability->HCCPARAMS2);

        maxDeviceSlot = USBCDNS3_U32BitsRead(XHCI_HCSPARAMS1_MAXSLOTS_MASK, XHCI_HCSPARAMS1_MAXSLOTS_SHIFT, xHCCaps->HCSPARAMS1);
        if (maxDeviceSlot > AMBA_XHCI_MAX_DEV_SLOT_NUM) {
            cdn_drv_res->MaxDeviceSlot = AMBA_XHCI_MAX_DEV_SLOT_NUM;
            //vDbgMsg(USBSSP_DBG_DRV, DBG_WARN, "<%d> Number of hw slots (%d) greater than %d\n", res->instanceNo, MaxDeviceSlot, USBSSP_MAX_DEVICE_SLOT_NUM);
        } else {
            cdn_drv_res->MaxDeviceSlot = maxDeviceSlot;
        }

        cap_length = USBCDNS3_U32BitsRead(XHCI_LENGTH_VER_CAPLENGTH_MASK, XHCI_LENGTH_VER_CAPLENGTH_SHIFT, (xHCCaps->LENGTH_VER));

        // assing XHC Operation registers
        offset_operation = xhci_base_addr + cap_length;
        cdn_drv_res->Registers.xhciOperational = getXhcOperationRegPtrOf64Addr(offset_operation);
        cdn_drv_res->Registers.xhciPortControl = &(cdn_drv_res->Registers.xhciOperational->PORTArray[0]);

        // assing XHC Runtime registers
        offset_runtime = xhci_base_addr + xHCCaps->RTSOFF;
        cdn_drv_res->Registers.xhciRuntime = getXhcRuntimeRegPtrOf64Addr(offset_runtime);
        cdn_drv_res->Registers.xhciInterrupter = &(cdn_drv_res->Registers.xhciRuntime->INTArray[0]);

        // assing XHC Doorbell registers
        offset_doorbell = xhci_base_addr + xHCCaps->DBOFF;
        //cdn_drv_res->Registers.xhciDoorbell = (UINT32 *)(offset_doorbell);
        void_ptr = IO_UtilityU64AddrToPtr(offset_doorbell);
        AmbaMisra_TypeCast(&uint32_ptr, &void_ptr);
        cdn_drv_res->Registers.xhciDoorbell = uint32_ptr;

        // assing XHC Extended Capability registers
        offset_ext_cap = USBCDNS3_U32BitsRead(XHCI_HCCPARAMS1_XECP_MASK, XHCI_HCCPARAMS1_XECP_SHIFT, xHCCaps->HCCPARAMS1);

        USB_UtilityPrintUInt64_1(USB_PRINT_FLAG_INFO, "offset_ext_cap = 0x%X", offset_ext_cap);

        if (offset_ext_cap > 0U) {
            offset_ext_cap = xhci_base_addr + (offset_ext_cap << 2U); // offset is given in DWORDs so multiply by 4
            //cdn_drv_res->Registers.xhciExtCaps.extCapsBaseAddr = (UINT32 *)IO_UtilityU64AddrToPtr(offset_ext_cap);
            void_ptr = IO_UtilityU64AddrToPtr(offset_ext_cap);
            AmbaMisra_TypeCast(&uint32_ptr, &void_ptr);
            cdn_drv_res->Registers.xhciExtCaps.extCapsBaseAddr = uint32_ptr;
            // read xHCI Extended Capabilities
            cdn_xhc_ext_cap_init(cdn_drv_res);
        } else {
            cdn_drv_res->Registers.xhciExtCaps.extCapsBaseAddr = NULL;
        }

        // display XHC Capability Registers
        //USB_CDNXhcInfoDisplay(xHCCaps, NULL);

        // need to delay for 100 ms for Host mode
        // if not, the port detection will be strange, current code doesn't handle this strange flow.
        USB_UtilityTaskSleep(100);

        // set it as Device or Host mode
        cdn_drv_res->DeviceModeFlag = ControllerMode;
        cdn_drv_res->UsbModeFlag    = USBCDNS3_DEV_SPEED_SSP; // maximum speed is Super Speed Plus

    }
}
