/**
 *  @file USBCDNS3_Utility.c
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
#include <AmbaUSBD_API.h>
#include <AmbaUSBH_API.h>
#include <AmbaUSB_System.h>
#include <AmbaUSB_Utility.h>
#include <AmbaUSB_CadenceSanity.h>

static USBD_UDC_s         local_usbd_udc;// __attribute__((section(".bss.noinit")));

struct USBD_UDC_ts *USBCDNS3_UdcInstanceGet(void)
{
    return &local_usbd_udc;
}

XHC_CDN_DRV_RES_s *USBCDNS3_DrvInstanceGet(void)
{
    return &local_usbd_udc.CDNDrvRes;
}

void USBCDNS3_UdcInstanceReset(UINT32 FlagResetAll)
{
    if (FlagResetAll == 1U) {
        USB_UtilityMemorySet(&local_usbd_udc, 0, sizeof(USBD_UDC_s));
    } else {
        local_usbd_udc.UxDcdOwner = NULL;
        USB_UtilityMemorySet(local_usbd_udc.DcdEndpointsInfoArray, 0, sizeof(UDC_ENDPOINT_INFO_s) * UDC_MAX_ENDPOINT_NUM);
        local_usbd_udc.RxFifoMax  = 0;
        local_usbd_udc.TxFifoUsed = 0;
        local_usbd_udc.TxFifoMax  = 0;
        USB_UtilityMemorySet(&local_usbd_udc.EndpointMapInfo, 0, sizeof(UDC_ENDPOINT_MAP_INFO_s));
        local_usbd_udc.CurrentFrameNumber  = 0;
        local_usbd_udc.DisableDoubleBuffer = 0;
        USB_UtilityMemorySet(&local_usbd_udc.HostEndpointsInfoArray[0], 0, sizeof(UHC_ENDPOINT_INFO_s));
#if 0
        local_usbd_udc.CDNDrvRes.ActualSpeed = 0;
        local_usbd_udc.CDNDrvRes.ActualPort = 0;
        local_usbd_udc.CDNDrvRes.ActualDeviceSlot = 0;
        local_usbd_udc.CDNDrvRes.EnableSlotPending = 0;
        local_usbd_udc.CDNDrvRes.DevConfigFlag = 0;
        local_usbd_udc.CDNDrvRes.ContextEntries = 0;
        local_usbd_udc.CDNDrvRes.Connected = 0;
        local_usbd_udc.CDNDrvRes.EnabledEndpsMask = 0;
        local_usbd_udc.CDNDrvRes.Ep0State = 0;
        local_usbd_udc.CDNDrvRes.DevAddress = 0;
        local_usbd_udc.CDNDrvRes.SetupID = 0;
        local_usbd_udc.CDNDrvRes.DevCtrlRxPending = 0;
        local_usbd_udc.CDNDrvRes.FlagDevConfigured = 0;
        local_usbd_udc.CDNDrvRes.FlagDevCommandWait = 0;
        local_usbd_udc.CDNDrvRes.DevCommandToWait = 0;
        USB_UtilityMemorySet(&local_usbd_udc.CDNDrvRes.CommandQueue, 0, sizeof(struct XHC_CDN_PRODUCER_QUEUE_ts));
        USB_UtilityMemorySet(&local_usbd_udc.CDNDrvRes.Ep0Queue, 0, sizeof(struct XHC_CDN_PRODUCER_QUEUE_ts));
        USB_UtilityMemorySet(&local_usbd_udc.CDNDrvRes.EpQueueArray[0], 0, sizeof(local_usbd_udc.CDNDrvRes.EpQueueArray));
#endif
        USB_UtilityMemorySet(&local_usbd_udc.CDNDrvRes, 0, sizeof(struct XHC_CDN_DRV_RES_ts));
    }
    local_usbd_udc.CDNDrvRes.Udc = &local_usbd_udc;
}

XHCI_OPERATION_REG_s *USBCDNS3_XhcOperationRegGet(void)
{
    return local_usbd_udc.CDNDrvRes.Registers.xhciOperational;
}

XHCI_CAPABILITY_REG_s *USBCDNS3_XhcCapabilityRegGet(void)
{
    return &local_usbd_udc.CDNDrvRes.RegistersQuickAccess.xHCCaps;
}

struct UDC_ENDPOINT_INFO_ts *USBDCDNS3_EndpointInfoGet(UINT32 PhysicalEpIndex)
{
    UDC_ENDPOINT_INFO_s  *udc_ep_info = NULL;

    if (PhysicalEpIndex < UDC_MAX_ENDPOINT_NUM) {
        udc_ep_info = &local_usbd_udc.DcdEndpointsInfoArray[PhysicalEpIndex];
    }
    return udc_ep_info;
}

void USBDCDNS3_EventCallbackSet(const USBDCDNS3_CALLBACKS_s *CallBacks)
{
    local_usbd_udc.CDNDrvRes.DeviceCallbacks = *CallBacks;
}

UINT32 USBCDNS3_CpuToLe32(UINT32 InputValue)
{
    return InputValue;
}

UINT64 USBCDNS3_CpuToLe64(UINT64 Value)
{
    return Value;
}

/**
 * uncached memory write for xhci registers
 * @param Address pointer to 32 bit register
 * @param Value 32bit value at given address
 */
void USBCDNS3_Write32(volatile UINT32 *Address, UINT32 Value)
{
    *Address = Value;
}

/**
 * uncached 64-bit memory write register function
 * @param Address pointer to 64 bit register
 * @param Value 64bit value to write to given address
 */
void USBCDNS3_Write64(volatile UINT64 *Address, UINT64 Value)
{
    *Address = Value;
}

/**
 * uncached memory read for xhci registers
 * @param Address pointer to 32 bit register
 * @return 32bit value at given address
 */
UINT32 USBCDNS3_Read32(volatile const UINT32 *Address)
{
    UINT32 value;
    value = *Address;
    return value;
}

/**
 * uncached 64-bit memory read register function
 * @param Address pointer to 64 bit register
 * @return 64bit value read from given address
 */
UINT64 USBCDNS3_Read64(volatile const UINT64 *Address)
{
    UINT64 value = *Address;
    return (value);
}

UINT64 USBCDNS3_PtrToU64PhyAddr(const void *Ptr)
{
    // in ThreadX, physical addr = virtal addr
    return IO_UtilityPtrToU64Addr(Ptr);
}

/**
 *  \brief    Set bit within the register value.
 *  \param    Width       width of the bit-field
 *  \param    Mask        mask for the bit-field
 *  \param    Flag_WOCLR  is bit-field has 'write one to clear' flag set
 *  \param    RegValue   register value
 *  \return   modified register value
 */
UINT32 USBCDNS3_U32BitSet(UINT32 Width, UINT32 Mask, UINT32 Flag_WOCLR, UINT32 RegValue)
{
    UINT32 new_value = RegValue;
    if ((Width == 1U) && (Flag_WOCLR == 0U)) {
        new_value |= Mask;
    }

    return (new_value);
}

/**
 *  \brief    Clear bit within the register value.
 *
 *  \param    Width         Width of the bit-field.
 *  \param    Mask          Mask for the bit-field.
 *  \param    Flag_WOSET    Is bit-field has 'write one to set' flag set.
 *  \param    Flag_WOCLR    Is bit-field has 'write one to clear' flag set.
 *  \param    RegValue      Register value.
 *
 *  \return   Modified register value.
 */
UINT32 USBCDNS3_U32BitClear(UINT32 Width, UINT32 Mask, UINT32 Flag_WOSET, UINT32 Flag_WOCLR,  UINT32 RegValue)
{
    UINT32 new_value = RegValue;
    if ((Width == 1U) && (Flag_WOSET == 0U)) {
        new_value = (new_value & ~Mask) | ((Flag_WOCLR != 0U) ? Mask : 0U);
    }

    return (new_value);
}

/**
 *  \brief    Write a value of the bit-field into the register value.
 *  \param    Mask        mask for the bit-field
 *  \param    Shift       bit-field shift from LSB
 *  \param    RegValue    register value
 *  \param    Value       value to be written to bit-field
 *  \return   modified register value
 */
UINT32 USBCDNS3_U32BitsWrite(UINT32 Mask, UINT32 Shift, UINT32 RegValue, UINT32 Value)
{
    UINT32 new_value = (Value << Shift) & Mask;
    new_value = (RegValue & ~Mask) | new_value;
    return (new_value);
}

/**
 *  \brief    Read a value of bit-field from the register value.
 *  \param    Mask        mask for the bit-field
 *  \param    Shift       bit-field shift from LSB
 *  \param    RegValue    register value
 *  \return   bit-field value
 */
UINT32 USBCDNS3_U32BitsRead(UINT32 Mask, UINT32 Shift, UINT32 RegValue)
{
    UINT32 result = (RegValue & Mask) >> Shift;
    return (result);
}

/**
 * writes DRBL register
 * @param DrvRes  driver resources
 * @param SlotID  XHCI slot id
 * @param DBValue DRBL register value
 */
void USBCDNS3_DoorbellWrite(const XHC_CDN_DRV_RES_s *DrvRes, UINT32 SlotID, UINT32 DBValue)
{
    //CPS_MemoryBarrier();
    if (DrvRes != NULL) {
        USBCDNS3_Write32(&DrvRes->Registers.xhciDoorbell[SlotID], DBValue);
    }
}

const char *USBCDNS3_TrbCmplCodeStringGet(UINT32 TrbCompletionCode)
{
    const char *ptr;
    switch (TrbCompletionCode) {
        case XHCI_TRB_CMPL_INVALID          : ptr = "INVALID"; break;
        case XHCI_TRB_CMPL_SUCCESS          : ptr = "SUCCESS"; break;
        case XHCI_TRB_CMPL_DATA_BUFF_ER     : ptr = "DATA_BUFF_ER"; break;
        case XHCI_TRB_CMPL_BBL_DETECT_ER    : ptr = "BBL_DETECT_ER"; break;
        case XHCI_TRB_CMPL_USB_TRANSCN_ER   : ptr = "USB_TRANSCN_ER"; break;
        case XHCI_TRB_CMPL_TRB_ERROR        : ptr = "TRB_ERROR"; break;
        case XHCI_TRB_CMPL_STALL_ERROR      : ptr = "STALL_ERROR"; break;
        case XHCI_TRB_CMPL_RSRC_ER          : ptr = "RSRC_ER"; break;
        case XHCI_TRB_CMPL_BDWTH_ER         : ptr = "BDWTH_ER"; break;
        case XHCI_TRB_CMPL_NO_SLTS_AVL_ER   : ptr = "NO_SLTS_AVL_ER"; break;
        case XHCI_TRB_CMPL_INVSTRM_TYP_ER   : ptr = "INVSTRM_TYP_ER"; break;
        case XHCI_TRB_CMPL_SLT_NOT_EN_ER    : ptr = "SLT_NOT_EN_ER"; break;
        case XHCI_TRB_CMPL_EP_NOT_EN_ER     : ptr = "EP_NOT_EN_ER"; break;
        case XHCI_TRB_CMPL_SHORT_PKT        : ptr = "SHORT_PKT"; break;
        case XHCI_TRB_CMPL_RING_UNDERRUN    : ptr = "RING_UNDERRUN"; break;
        case XHCI_TRB_CMPL_RING_OVERRUN     : ptr = "RING_OVERRUN"; break;
        case XHCI_TRB_CMPL_VF_EVTRNGFL_ER   : ptr = "VF_EVTRNGFL_ER"; break;
        case XHCI_TRB_CMPL_PARAMETER_ER     : ptr = "PARAMETER_ER"; break;
        case XHCI_TRB_CMPL_BDWTH_OVRRN_ER   : ptr = "BDWTH_OVRRN_ER"; break;
        case XHCI_TRB_CMPL_CXT_ST_ER        : ptr = "CXT_ST_ER"; break;
        case XHCI_TRB_CMPL_NO_PNG_RSP_ER    : ptr = "NO_PNG_RSP_ER"; break;
        case XHCI_TRB_CMPL_EVT_RNG_FL_ER    : ptr = "EVT_RNG_FL_ER"; break;
        case XHCI_TRB_CMPL_INCMPT_DEV_ER    : ptr = "INCMPT_DEV_ER"; break;
        case XHCI_TRB_CMPL_MISSED_SRV_ER    : ptr = "MISSED_SRV_ER"; break;
        case XHCI_TRB_CMPL_CMD_RNG_STOPPED  : ptr = "CMD_RNG_STOPPED"; break;
        case XHCI_TRB_CMPL_CMD_ABORTED      : ptr = "CMD_ABORTED"; break;
        case XHCI_TRB_CMPL_STOPPED          : ptr = "STOPPED"; break;
        case XHCI_TRB_CMPL_STOP_LEN_INV     : ptr = "STOP_LEN_INV"; break;
        case XHCI_TRB_CMPL_STOP_SHORT_PKT   : ptr = "STOP_SHORT_PKT"; break;
        case XHCI_TRB_CMPL_MAXEXTLT_LG_ER   : ptr = "MAXEXTLT_LG_ER"; break;
        case XHCI_TRB_CMPL_ISO_BUFF_OVRUN   : ptr = "ISO_BUFF_OVRUN"; break;
        case XHCI_TRB_CMPL_EVT_LOST_ER      : ptr = "EVT_LOST_ER"; break;
        case XHCI_TRB_CMPL_UNDEFINED_ER     : ptr = "UNDEFINED_ER"; break;
        case XHCI_TRB_CMPL_INV_STRM_ID_ER   : ptr = "INV_STRM_ID_ER"; break;
        case XHCI_TRB_CMPL_SEC_BDWTH_ER     : ptr = "SEC_BDWTH_ER"; break;
        case XHCI_TRB_CMPL_SPLT_TRNSCN_ER   : ptr = "SPLT_TRNSCN_ER"; break;
        default: ptr = "UNKNOWN"; break;
    }
    return ptr;
}

const char *USBCDNS3_TrbTypeStringGet(UINT32 TrbType)
{
    const char *ptr;
    switch (TrbType) {
        case XHCI_TRB_NORMAL                : ptr = "NORMAL"; break;
        case XHCI_TRB_SETUP_STAGE           : ptr = "SETUP_STAGE"; break;
        case XHCI_TRB_DATA_STAGE            : ptr = "DATA_STAGE"; break;
        case XHCI_TRB_STATUS_STAGE          : ptr = "STATUS_STAGE"; break;
        case XHCI_TRB_ISOCH                 : ptr = "ISOCH"; break;
        case XHCI_TRB_LINK                  : ptr = "LINK"; break;
        case XHCI_TRB_EVENT_DATA            : ptr = "EVENT_DATA"; break;
        case XHCI_TRB_NO_OP                 : ptr = "NO_OP"; break;
        case XHCI_TRB_ENABLE_SLOT_COMMAND   : ptr = "ENABLE_SLOT_COMMAND"; break;
        case XHCI_TRB_DISABLE_SLOT_COMMAND  : ptr = "DISABLE_SLOT_COMMAND"; break;
        case XHCI_TRB_ADDR_DEV_CMD          : ptr = "ADDR_DEV_CMD"; break;
        case XHCI_TRB_CONF_EP_CMD           : ptr = "CONF_EP_CMD"; break;
        case XHCI_TRB_EVALUATE_CXT_CMD      : ptr = "EVALUATE_CXT_CMD"; break;
        case XHCI_TRB_RESET_EP_CMD          : ptr = "RESET_EP_CMD"; break;
        case XHCI_TRB_STOP_EP_CMD           : ptr = "STOP_EP_CMD"; break;
        case XHCI_TRB_SET_TR_DQ_PTR_CMD     : ptr = "SET_TR_DQ_PTR_CMD"; break;
        case XHCI_TRB_RESET_DEVICE_COMMAND  : ptr = "RESET_DEVICE_COMMAND"; break;
        case XHCI_TRB_FORCE_EVENT_COMMAND   : ptr = "FORCE_EVENT_COMMAND"; break;
        case XHCI_TRB_NEGOTIATE_BANDWIDTH   : ptr = "NEGOTIATE_BANDWIDTH"; break;
        case XHCI_TRB_SET_LAT_TOL_VAL_CMD   : ptr = "SET_LAT_TOL_VAL_CMD"; break;
        case XHCI_TRB_GET_PORT_BNDWTH_CMD   : ptr = "GET_PORT_BNDWTH_CMD"; break;
        case XHCI_TRB_FORCE_HEADER_COMMAND  : ptr = "FORCE_HEADER_COMMAND"; break;
        case XHCI_TRB_NO_OP_COMMAND         : ptr = "NO_OP_COMMAND"; break;
        case XHCI_TRB_TRANSFER_EVENT        : ptr = "TRANSFER_EVENT"; break;
        case XHCI_TRB_CMD_CMPL_EVT          : ptr = "CMD_CMPL_EVT"; break;
        case XHCI_TRB_PORT_ST_CHG_EVT       : ptr = "PORT_ST_CHG_EVT"; break;
        case XHCI_TRB_BNDWTH_RQ_EVT         : ptr = "BNDWTH_RQ_EVT"; break;
        case XHCI_TRB_DOORBELL_EVENT        : ptr = "DOORBELL_EVENT"; break;
        case XHCI_TRB_HOST_CTRL_EVT         : ptr = "HOST_CTRL_EVT"; break;
        case XHCI_TRB_DEV_NOTIFCN_EVT       : ptr = "DEV_NOTIFCN_EVT"; break;
        case XHCI_TRB_MFINDEX_WRAP_EVENT    : ptr = "MFINDEX_WRAP_EVENT"; break;
        case XHCI_TRB_NRDY_EVT              : ptr = "NRDY_EVT"; break;
        case XHCI_TRB_SETUP_PROTO_ENDP_CMD  : ptr = "SETUP_PROTO_ENDP_CMD"; break;
        case XHCI_TRB_GET_PROTO_ENDP_CMD    : ptr = "GET_PROTO_ENDP_CMD"; break;
        case XHCI_TRB_SET_ENDPS_ENA_CMD     : ptr = "SET_ENDPS_ENA_CMD"; break;
        case XHCI_TRB_GET_ENDPS_ENA_CMD     : ptr = "GET_ENDPS_ENA_CMD"; break;
        case XHCI_TRB_ADD_TDL_CMD           : ptr = "ADD_TDL_CMD"; break;
        case XHCI_TRB_HALT_ENDP_CMD         : ptr = "HALT_ENDP_CMD"; break;
        case XHCI_TRB_SETUP_STAGE1          : ptr = "SETUP_STAGE1"; break;
        case XHCI_TRB_HALT_ENDP_CMD1        : ptr = "HALT_ENDP_CMD1"; break;
        case XHCI_TRB_DRBL_OVERFLOW_EVENT   : ptr = "DRBL_OVERFLOW_EVENT"; break;
        case XHCI_TRB_FLUSH_EP_CMD          : ptr = "FLUSH_EP_CMD"; break;
        case XHCI_TRB_VF_SEC_VIOLN_EVT      : ptr = "VF_SEC_VIOLN_EVT"; break;
        default: ptr = "UNKNOWN"; break;
    }
    return ptr;
}

/*
 * Get the trb type of TRB
 */
UINT32 USBCDNS3_TrbTypeGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return ((Trb->DWord3 >> XHCI_TRB_TYPE_POS) & 0x3FU);
}

/*
 * Get the port ID of TRB
 */
UINT8 USBCDNS3_TrbPortIdGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return (UINT8) ((Trb->DWord0 >> 24U) & 0x0FFU);
}

/*
 * Get the chain of TRB
 */
UINT8 USBCDNS3_TrbChainBitGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return (UINT8) ((Trb->DWord3 >> 4U) & 0x01U);
}

/*
 * Get the toggle bit of TRB
 */
UINT8 USBCDNS3_TrbToggleBitGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return (UINT8) ((Trb->DWord3) & 1U);
}

/*
 * Get the endpoint of TRB
 */
UINT8 USBCDNS3_TrbEndpointIdGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return (UINT8) ((Trb->DWord3 >> XHCI_TRB_ENDPOINT_POS) & 0x01FU);
}

/*
 * Get the slot ID of TRB
 */
UINT8 USBCDNS3_TrbSlotIdGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return (UINT8) ((Trb->DWord3 >> XHCI_TRB_SLOT_ID_POS) & 0x0FFU);
}

/*
 * Get the completion code of TRB
 */
UINT32 USBCDNS3_TrbCmplCodeGet(XHCI_RING_ELEMENT_s const *Trb)
{
    return ((Trb->DWord2 >> XHCI_TRB_CMPL_CODE_POS) & 0xFFU);
}

/*
 * Get the Pointer LO/HI address of TRB
 */
UINT64 USBCDNS3_TrbDequeueAddrGet(const XHCI_RING_ELEMENT_s *Trb)
{
    UINT64 dequeue_addr = Trb->DWord1;

    dequeue_addr = dequeue_addr << 32UL;
    dequeue_addr = dequeue_addr + Trb->DWord0;

    return dequeue_addr;
}


void USBCDNS3_TrbInfoDisplay(const XHCI_RING_ELEMENT_s *Trb, const char *PrefixStr)
{

    static char str_buffer[255];
    static char int_buffer[255];
    UINT32 trb_type = USBCDNS3_TrbTypeGet(Trb);
    UINT32 print_flag = USB_PRINT_FLAG_L2;

    if (USB_UtilityPrintLevelSupport(print_flag) != 0U) {

        USB_UtilityPrint(print_flag, "");
        USB_UtilityPrint(print_flag, "======================");

        str_buffer[0] = '\0';
        IO_UtilityStringAppend(str_buffer, 255, PrefixStr);
        IO_UtilityStringAppend(str_buffer, 255, ": TRB Type ");
        if (IO_UtilityUInt32ToStr(int_buffer, 255, trb_type, 10) != 0U) {
            // ignore return value
        }
        IO_UtilityStringAppend(str_buffer, 255, int_buffer);
        IO_UtilityStringAppend(str_buffer, 255, " (");
        IO_UtilityStringAppend(str_buffer, 255, USBCDNS3_TrbTypeStringGet(trb_type));
        IO_UtilityStringAppend(str_buffer, 255, ")");
        USB_UtilityPrint(print_flag, str_buffer);

        USB_UtilityPrintUInt64_1(print_flag, "TRB 0x%08X", IO_UtilityPtrToU64Addr(Trb));
        USB_UtilityPrintUInt5(print_flag, "TRB[0-3] 0x%08X 0x%08X 0x%08X 0x%08X", Trb->DWord0, Trb->DWord1, Trb->DWord2, Trb->DWord3, 0);

        USB_UtilityPrint(print_flag, "======================");
    }
}

/**
 * Function returns 64bit integer value of C pointer to type uintptr
 * @param ptr pointer to 32bit type
 * @return 64bit integer of C pointer to type uintptr
 */

XHCI_RING_ELEMENT_s *USBCDNS3_U64AddrToTrbRingPtr(UINT64 phyaddr)
{
    XHCI_RING_ELEMENT_s *ptr_ret;
    #ifdef __aarch64__
    // 64-bit address to 64-bit pointer
    USB_UtilityMemoryCopy(&ptr_ret, &phyaddr, sizeof(void *));
    #else
    // 64-bit address to 32-bit pointer
    UINT32 u32_addr = (UINT32)phyaddr;
    USB_UtilityMemoryCopy(&ptr_ret, &u32_addr, sizeof(void *));
    #endif
    return ptr_ret;
}

/**
 * Function sets 64 value at address given in addr parameter
 * @param addrL pointer to uint32_t word low
 * @param addrH pointer to uint32_t word high
 * @param value 64-bit dword value to write
 */
/* parasoft-begin-suppress METRICS-36-3 "A function should not be called from more than 5 different functions, DRV-3823" */
void USBCDNS3_U64ValueSet(volatile UINT32 *addrL, volatile UINT32 *addrH, UINT64 value)
{
    *addrL = (UINT32) (value & 0x0FFFFFFFFU);
    *addrH = (UINT32) ((value >> 32UL) & 0x0FFFFFFFFU);
}

/**
 * Auxiliary function, returns actual speed field from endpoint context, note
 * that software speed enum values may differ from speed values coded in XHCI spec.
 * @param speed actual speed kept in res->ActualSpeed
 * @return speed value of speed given in values as XHCI specification states in
 *                endpoint context structure
 */

UINT32 USBCDNS3_Ch9SpeedToXhciSpeed(UINT32 ch9_usb_speed)
{

    UINT32 slotSpeed;

    // translate CH9_UsbSpeed value to integer values according to XHCI spec
    switch (ch9_usb_speed) {
            // low speed
        case CH9_USB_SPEED_LOW: slotSpeed = 2U;
            break;
            // full speed
        case CH9_USB_SPEED_FULL: slotSpeed = 1U;
            break;
            // high speed
        case CH9_USB_SPEED_HIGH: slotSpeed = 3U;
            break;
            // super speed
        case CH9_USB_SPEED_SUPER: slotSpeed = 4U;
            break;
            // super speed plus
        case CH9_USB_SPEED_SUPER_PLUS: slotSpeed = 5U;
            break;
        default: slotSpeed = 0U;
            break;
    }

    return (slotSpeed);
}

