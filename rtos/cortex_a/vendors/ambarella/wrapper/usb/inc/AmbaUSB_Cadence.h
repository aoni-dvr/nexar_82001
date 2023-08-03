/**
 *  @file AmbaUSB_Cadence.h
 *
 *  Copyright (c) 2020 Ambarella International LP
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
 *  @details Cadence IP USB header file.
 */

#ifndef AMBAUSB_CADENCE_H
#define AMBAUSB_CADENCE_H

#include <StdUSB.h>
#include <StdUSB_CH9.h>
#include <StdXHCI_Bits.h>
#include <AmbaUSB_CadencePhy.h>
#include <AmbaUSB_CadenceController.h>

#define USB32_LINUX_SWITCH_SUPPORT

typedef void (*CDNS3_PRINT_f)(const char *Str);

// Non-Secure Scratchpad registers
#define SCRATCHPAD_NONSEC_ADDRESS     (0x20E0024000U)
#define SCRATCHPAD_USB32_PMA_CTRL_REG (0x154U)
#define SCRATCHPAD_USB32C_CTRL_REG    (0x16CU)
#define SCRATCHPAD_USB32P_CTRL_REG    (0x174U)

// Definitions of Vendor-defined TRB types (Cadence)
#define    XHCI_TRB_NRDY_EVT              48U
#define    XHCI_TRB_SETUP_PROTO_ENDP_CMD  49U
#define    XHCI_TRB_GET_PROTO_ENDP_CMD    50U
#define    XHCI_TRB_SET_ENDPS_ENA_CMD     51U
#define    XHCI_TRB_GET_ENDPS_ENA_CMD     52U
#define    XHCI_TRB_ADD_TDL_CMD           53U
#define    XHCI_TRB_HALT_ENDP_CMD         ((UINT32)54U)
#define    XHCI_TRB_SETUP_STAGE1          55U
#define    XHCI_TRB_HALT_ENDP_CMD1        56U
#define    XHCI_TRB_DRBL_OVERFLOW_EVENT   57U
#define    XHCI_TRB_FLUSH_EP_CMD          58U
#define    XHCI_TRB_VF_SEC_VIOLN_EVT      59U

// Definitions of Vendor-defined TRB Completion Code (Cadence)
#define    XHCI_TRB_CMPL_CDNSDEF_ERCODES 192U


/*
 * These bits are Read Only (RO) and should be saved and written to the
 * registers: 0 (connect status) and  10:13 (port speed).
 * These bits are also sticky - meaning they're in the AUX well and they aren't
 * changed by a hot and warm.
 */
#define XHCI_DEV_USB23_PORT_RO    (XHCI_PORTSC_CCS_MASK | \
                                 XHCI_PORTSC_PORTSPEED_MASK)

/*
 * These bits are RW; writing a 0 clears the bit, writing a 1 sets the bit:
 * bits 5:8 (link state), 25:26  ("wake on" enable state)
 */
#define XHCI_DEV_USB23_PORT_RWS    (XHCI_PORTSC_PLS_MASK | \
                                 XHCI_PORTSC_WCE_MASK | \
                                 XHCI_PORTSC_WDE_MASK)

#define XHCI_DEV_PORTSC_CHANGE_BITS (XHCI_PORTSC_CSC_MASK | \
                                     XHCI_PORTSC_PEC_MASK | \
                                     XHCI_PORTSC_WRC_MASK | \
                                     XHCI_PORTSC_OCC_MASK | \
                                     XHCI_PORTSC_PRC_MASK | \
                                     XHCI_PORTSC_PLC_MASK | \
                                     XHCI_PORTSC_CEC_MASK)


// Cadence Miscellaneous 3xPort operation mode configuration register
#define CDN_CFG_3XPORT_DIS_UFP_MASK    0x00000001U
#define CDN_CFG_3XPORT_DIS_UFP_SHIFT            0U
#define CDN_CFG_3XPORT_DIS_UFP_WIDTH            1U
#define CDN_CFG_3XPORT_DIS_UFP_WOCLR            0U
#define CDN_CFG_3XPORT_DIS_UFP_WOSET            0U
#define CDN_CFG_3XPORT_DIS_DFP_MASK    0x00000002U
#define CDN_CFG_3XPORT_DIS_DFP_SHIFT            1U
#define CDN_CFG_3XPORT_DIS_DFP_WIDTH            1U
#define CDN_CFG_3XPORT_DIS_DFP_WOCLR            0U
#define CDN_CFG_3XPORT_DIS_DFP_WOSET            0U
#define CDN_CFG_3XPORT_SSP_SUP_MASK    0x80000000U
#define CDN_CFG_3XPORT_SSP_SUP_SHIFT           31U
#define CDN_CFG_3XPORT_SSP_SUP_WIDTH            1U
#define CDN_CFG_3XPORT_SSP_SUP_WOCLR            0U
#define CDN_CFG_3XPORT_SSP_SUP_WOSET            0U

#define CDN_D_XEC_CFG_3XPORT_CAP_ID     (0xC0U) //!< Device mode XHCI Extended config 3xPORT CAP ID

#define    AMBA_XHCI_DEFAULT_TIMEOUT       (1000U)
#define AMBA_XHCI_EXTENDED_CONTEXT      (1U)
#define AMBA_XHCI_PAGE_SIZE_IDX         (1U)
#define AMBA_XHCI_PAGE_SIZE             (4096U)
#define    AMBA_XHCI_SCRATCHPAD_BUFF_NUM   (1U)
#define AMBA_XHCI_MAX_DEV_SLOT_NUM      (2U)
#define    AMBA_XHCI_CONTEXT_WIDTH         (16U)
#define    AMBA_XHCI_MAX_EP_CONTEXT_NUM    (30U)
#define    AMBA_XHCI_EVENT_QUEUE_SIZE      (64U)
#define    AMBA_XHCI_PRODUCER_QUEUE_SIZE   (64U * 3U)
#define    AMBA_XHCI_STREAM_ARRAY_SIZE     (8U) //!< Should be calculated according to formula: STREAM_ARRAY_SIZE = 2 exp(MAX_STREMS_PER_EP + 1)
#define    AMBA_XHCI_INTERRUPTER_COUNT     (4U)
#define    AMBA_XHCI_EP_CONT_OFFSET        (2U) //!< Endpoint container offset value
#define    AMBA_XHCI_MAX_EXT_CAPS_COUNT    (8U)
#define    AMBA_XHCI_EP0_DATA_BUFF_SIZE    (1024U) //!< Endpoint 0 data buffer size - used in enumeration

/** given according to XHCI register value: 1 = 4 streams, 2 = 8 streams, 3 = 16 streams and so on */
#define    AMBA_XHCI_MAX_STREMS_PER_EP        (2U)
#define    AMBA_XHCI_MAX_EP_NUM_STRM_EN       (30U)

//
// Device mode ports settings
//
#define    USBCDNS3_DEV_MODE_20_PORT_IDX             (0U)
#define    USBCDNS3_DEV_MODE_32_PORT_IDX             (1U)

//
// CDN XHC mode
//
#define USBCDNS3_MODE_HOST               ((UINT32)0U) //!< Host mode
#define USBCDNS3_MODE_DEVICE             ((UINT32)1U) //!< Device mode

//
// CDN XHC device speed
//
#define USBCDNS3_DEV_SPEED_SSP           (1U)
#define USBCDNS3_DEV_SPEED_HS            (2U)
#define USBCDNS3_DEV_SPEED_SS            (3U)


#define USBCDNS3_EFLAGS_UNDEFINED        (0U)
#define USBCDNS3_EFLAGS_NODORBELL        (1U)
#define USBCDNS3_EFLAGS_FORCELINKTRB     (2U)

#define USBCDNS3_DEFAULT_TIMEOUT         ((UINT32)3000U)

//
// State of Control Endpoint in USB device driver
//
#define USBCDNS3_EP0_UNCONNECTED        ((UINT32)0U)
#define USBCDNS3_EP0_HALT_PENDING       ((UINT32)1U)
#define USBCDNS3_EP0_HALT_SETUP_PENDING ((UINT32)2U)
#define USBCDNS3_EP0_HALTED             ((UINT32)3U)
#define USBCDNS3_EP0_SETUP_PENDING      ((UINT32)4U)
#define USBCDNS3_EP0_SETUP_PHASE        ((UINT32)5U)
#define USBCDNS3_EP0_DATA_PHASE         ((UINT32)6U)
#define USBCDNS3_EP0_STATUS_PHASE       ((UINT32)7U)


struct USBD_UDC_ts;

typedef struct XHC_CDN_PRODUCER_QUEUE_ts  XHC_CDN_PRODUCER_QUEUE_s;
typedef struct XHC_CDN_DRV_RES_ts         XHC_CDN_DRV_RES_s;

/** Completion callback */
typedef void (*XHC_CDN_COMPLETE_f)(XHC_CDN_DRV_RES_s *Arg, UINT32 SlotID, UINT32 EpIndex, UINT32 Status, const XHCI_RING_ELEMENT_s *EventPtr, UINT8 *Buffer, UINT32 ActualLength);

#define SG_FLAG_DOORBELL        (0x1U)

typedef struct {
    const UINT64 *BufferArray;
    const UINT32 *SizeArray;
    UINT32        ElementConut;
    UINT32        Flags;
} USBCDNS3_SG_PARAM;

/**
 * Structure describes element of producer queue in conjuction to associated endpoint.
 * Note that the same structure is used in command ring - in this case, fields
 * referred to endpoint are not used
*/
struct XHC_CDN_PRODUCER_QUEUE_ts {
    XHCI_RING_ELEMENT_s *RingArray;    //!< Memory buffer for ring used by hardware
    XHCI_RING_ELEMENT_s *EnqueuePtr;   //!< Pointer to enqueued element
    XHCI_RING_ELEMENT_s *DequeuePtr;   //!< Pointer to dequeued element
    XHCI_RING_ELEMENT_s *CompletePtr;  //!< Pointer to last completed queue element
    UINT32               ToogleBit;    //!< Keeps actual value of Cycle bit inserted to TRB
    XHC_CDN_COMPLETE_f   CompleteFunc; //!< Callback function called on TRB complete event
    XHC_CDN_COMPLETE_f   AggregatedCompleteFunc; //!< Callback function called on an aggregated transfer completion
    UINT32               ContextIndex; //!< Keeps Device context index value of endpoint associated with this object
    /**
     * Auxiliary flag, set to 1 when any TD associated with this object is
     * issued to DMA and flag is set to 0 on complete event
    */
    UINT8   IsRunningFlag;
    UINT8   IsDisabledFlag; //!< Flag is active when endpoint is in stopped state
    UINT8   epDesc[USB_DESC_LENGTH_ENDPOINT + USB_DESC_LENGTH_SS_EP_COMP + USB_DESC_LENGTH_ISO_EP_COMP]; //!< Keeps copy of endpoint descriptor of associated endpoint
    UINT32  NumOfBytes;     //!< Number of bytes for last transfer
    UINT32  NumOfResidue;   //!< Number of residue bytes of last transfer
    UINT64  LastXferBufferPhyAddr; //!< Buffer address for last successful TD
    UINT32  LastXferActualLength; //!< Number of bytes actually transferred
    UINT8   CompletionCode; //!< Completion code of last transfer
    UINT32  InterrupterIdx; //!< Interrupter Index of the target interrupter
    XHCI_RING_ELEMENT_s* FirstQueuedTRB; //!< points to the first queuued TRB in a Ring
    XHCI_RING_ELEMENT_s* LastQueuedTRB; //!< points to the last queuued TRB in a Ring
    UINT8   ExtraFlags;
    UINT32  FrameID;
    UINT32  StreamCount; //!< Number of streams available
    UINT16  ActualSID;   //!< Keeps actually selected stream ID
    UINT16  EventSID;    //!< Keeps stream ID of the last event received from HW
    UINT32 *HWContext;   //!< Points to hardware endpoint context according to spec 6.2.3
    XHC_CDN_DRV_RES_s *Parent; //!< Points to this object owner
    UINT8   IgnoreShortPacket;  //!< Blocks calling complete callback when set to 1
    XHC_CDN_PRODUCER_QUEUE_s *StreamQueueArray[AMBA_XHCI_STREAM_ARRAY_SIZE]; // Streams container
};

/** Device context base array pointer structure (DCBAA) */
typedef struct {
    UINT64  ScratchPadBufferArrayBaseAddr; //!< Address of XHCI scratchpad buffer array base address
    UINT64  DeviceContextBaseAddr[AMBA_XHCI_MAX_DEV_SLOT_NUM]; //!< Address of XHCI Device Context base address
} XHCI_DCBAA_s;

/** Device/Output context structure, assuming CSZ = 1 (64 bytes per entry) */
typedef struct {
    UINT32 SlotContext[AMBA_XHCI_CONTEXT_WIDTH];  //!< Slot context structure
    UINT32 Ep0Context[AMBA_XHCI_CONTEXT_WIDTH];   //!< Endpoint 0 context structure */
    UINT32 EpContext[AMBA_XHCI_MAX_EP_CONTEXT_NUM][AMBA_XHCI_CONTEXT_WIDTH]; //!< Endpoints context structures */
} XHCI_OUTPUT_CONTEXT_s;

/** Input context structure */
typedef struct {
    UINT32 InputControlContext[AMBA_XHCI_CONTEXT_WIDTH]; //!< Input control context structure
    UINT32 SlotContext[AMBA_XHCI_CONTEXT_WIDTH];         //!< Slot context structure
    UINT32 Ep0Context[AMBA_XHCI_CONTEXT_WIDTH];          //!< Endpoint 0 context structure
    UINT32 EpContext[AMBA_XHCI_MAX_EP_CONTEXT_NUM][AMBA_XHCI_CONTEXT_WIDTH]; //!< Endpoints context structures
} XHCI_INPUT_CONTEXT_s;

/** Structure that describes single Extended Capability */
typedef struct  {
    UINT32  firstDwordVal; //!< Value of first 32bit word for this Ext. Cap. (DWORD[0])
    UINT8   capId; //!< Capability ID (DWORD[0].CapabilityID)
    UINT32 *firstCapSfrPtr; //!< Pointer to first SFR belonging to this capability
} XHC_CDN_EXT_CAP_ELEMENT_s;

typedef struct {
    UINT32 *extCapsBaseAddr;   //!< Address of first Extended Capabilities' SFR (XEC_USBLEGSUP)
    UINT32  usbLegSup;         //!< Contents of USBLEGSUP SFR
    UINT32  usbLegCtlSts;      //!< Contents of USBLEGCTLSTS SFR
    XHC_CDN_EXT_CAP_ELEMENT_s extCaps[AMBA_XHCI_MAX_EXT_CAPS_COUNT]; //!< Array with Extended Capabilities
    UINT8   extCapsCount;      //!< Number of Extended Capabilities recognized
} XHC_CDN_EXT_CAP_s;

typedef struct {
    XHCI_INPUT_CONTEXT_s  InputContext __attribute__((aligned(XHCI_CONTEXT_ALIGNMENT))); //!< Input Context
    XHCI_OUTPUT_CONTEXT_s OutputContext __attribute__((aligned(AMBA_XHCI_PAGE_SIZE)));   //!< Device/Output context structure for slot 0
    UINT64                Scratchpad[AMBA_XHCI_SCRATCHPAD_BUFF_NUM + 1U] __attribute__((aligned(AMBA_XHCI_PAGE_SIZE))); //!< Scratch pad buffers (extra element for last pointer = NULL)
    UINT8                 ScratchpadPool[AMBA_XHCI_SCRATCHPAD_BUFF_NUM * AMBA_XHCI_PAGE_SIZE] __attribute__((aligned(AMBA_XHCI_PAGE_SIZE))); //!< Scratch pad buffers pool
    XHCI_DCBAA_s          Dcbaa __attribute__((aligned(XHCI_DCBAA_ALIGNMENT)));

    UINT64                EventRingSegmentEntry[AMBA_XHCI_INTERRUPTER_COUNT * 2U] __attribute__((aligned(XHCI_ERST_ALIGNMENT))); //!< event ring segment entry

    XHCI_RING_ELEMENT_s   EpRingPool[AMBA_XHCI_PRODUCER_QUEUE_SIZE * (AMBA_XHCI_MAX_EP_CONTEXT_NUM + 2U)] __attribute__((aligned(1024))); //!< event ring segment entry
    XHCI_RING_ELEMENT_s   EventPool[AMBA_XHCI_EVENT_QUEUE_SIZE] __attribute__((aligned(1024))); //!< Event Ring

    XHC_CDN_PRODUCER_QUEUE_s StreamMemoryPool[AMBA_XHCI_MAX_EP_NUM_STRM_EN][AMBA_XHCI_STREAM_ARRAY_SIZE]; //!< allocated memory for stream objects
    XHCI_RING_ELEMENT_s      StreamRing[AMBA_XHCI_MAX_EP_NUM_STRM_EN][AMBA_XHCI_STREAM_ARRAY_SIZE][AMBA_XHCI_PRODUCER_QUEUE_SIZE] __attribute__((aligned(1024))); //!< allocation memory for stream rings

    UINT8                 Ep0Buffer[AMBA_XHCI_EP0_DATA_BUFF_SIZE] __attribute__((aligned(64))); //!< EP0 buffer
} XHC_CDN_MEMORY_RES_s;

typedef struct {
    XHCI_CAPABILITY_REG_s *xhciCapability;
    XHCI_OPERATION_REG_s  *xhciOperational;
    XHCI_PORT_s           *xhciPortControl;
    XHCI_RUNTIME_REG_s    *xhciRuntime;
    XHCI_INTERRUPT_REG_s  *xhciInterrupter;
    UINT32                *xhciDoorbell;
    XHC_CDN_EXT_CAP_s      xhciExtCaps; //!< xHCI capabilities are not handled as ordinary SFRs
} XHC_CDN_SFR_s;


typedef struct {
    XHCI_CAPABILITY_REG_s xHCCaps; //!< Copy of xhci capability register for quick access.
} XHC_CDN_QUICK_ACCESS_s;


/** No-Op completion callback */
typedef void (*USBSSP_NopComplete)(XHC_CDN_DRV_RES_s *arg);

/** Force Header complete callback function */
typedef void (*USBSSP_ForceHeaderComplete)(XHC_CDN_DRV_RES_s* arg);

typedef UINT32 (*USBDCDNS3_CB_SETUP_f)(struct USBD_UDC_ts *Udc, const USB_CH9_SETUP_s *SetupInfo);
typedef void   (*USBDCDNS3_CB_CONNECT_f)(XHC_CDN_DRV_RES_s *DrvRes);
typedef void   (*USBDCDNS3_CB_DISCONNECT_f)(XHC_CDN_DRV_RES_s *DrvRes);
typedef void   (*USBDCDNS3_CB_RESET_f)(XHC_CDN_DRV_RES_s *DrvRes);

/**
 * struct containing function pointers for event notification callbacks issued
 * by isr().
 * Each call passes the driver's privateData pointer for instance
 * identification if necessary, and may also pass data related to the event.
*/
typedef struct {
    USBDCDNS3_CB_DISCONNECT_f Disconnect;
    USBDCDNS3_CB_CONNECT_f    Connect;
    USBDCDNS3_CB_SETUP_f      Setup;
    USBDCDNS3_CB_RESET_f      Reset;
    //CUSBD_CdSuspend suspend;
    //CUSBD_CbResume resume;
    //CUSBD_CbbusInterval busInterval;
} USBDCDNS3_CALLBACKS_s;

typedef void  (*USBHCDNS3_CB_CONNECT_CHANGED_f)(UINT32 PortIndex);

typedef struct {
    USBHCDNS3_CB_CONNECT_CHANGED_f ConnectionChanged;
} USBHCDNS3_CALLBACKS_s;

/**
 * Structure represents USB SSP controller resources.
 * That structure must be aligned to USBSSP_PAGE_SIZE because of xhciResources.
*/
struct XHC_CDN_DRV_RES_ts {
    XHC_CDN_MEMORY_RES_s      *XhciMemResources;   //!< Structure represents USB SSP memory required by XHCI specification.
    XHCI_INPUT_CONTEXT_s      *InputContext;       //!< Input context structure
    XHCI_INPUT_CONTEXT_s       InputContextCopy;   //!< Input context copy structure
    XHCI_RING_ELEMENT_s       *EventPtr;           //!< Pointer to actual event ring element
    UINT8                      EventToogleBit;     //!< Keeps actual value of cycle bit for event ring
    struct XHC_CDN_PRODUCER_QUEUE_ts CommandQueue; //!< Command queue object
    struct XHC_CDN_PRODUCER_QUEUE_ts Ep0Queue;     //!< Default endpoint queue object
    struct XHC_CDN_PRODUCER_QUEUE_ts EpQueueArray[AMBA_XHCI_MAX_EP_CONTEXT_NUM + AMBA_XHCI_EP_CONT_OFFSET]; //!< Container of non default endpoint objects

    UINT32                     ActualSpeed;       //!< Keeps actual speed the port operate in
    UINT32                     ActualPort;        //!< Keeps port ID
    UINT32                     ActualDeviceSlot;  //!< Keeps actual device slot, when USBSSP works in device mode it is 1
    UINT8                      EnableSlotPending; //!< Indicates whether ENABLE_SLOT_COMMAND was sent but slot ID has not been set yet
    UINT8                      DevConfigFlag;   //!< Flag is active when USB SSP is in configured state
    UINT32                     DeviceModeFlag;  //!< 1 (USBCDNS3_MODE_DEVICE) - USB SSP works in device mode, 0 (USBCDNS3_MODE_HOST) - host mode
    UINT8                      UsbModeFlag;     //!< Indicates USB mode (2 - forced USB2 mode, 3 - forced USB3 mode, others - default)
    UINT8                      ExtendedTBCMode; //!< Extended TBC enable mode if ETC enabled (0: ETE always disabled, 1:ETE enabled for all speeds)
    UINT8                     *Ep0Buff;         //!< Internal buffer for control transfer
    UINT32                     ContextEntries;  //!< Stores actual value of context Entry
    UINT8                      Connected;       //!< Reflects actual state of Current Connect Status of PORTSC register, 0 - disconnected, 1 - connected
    XHC_CDN_QUICK_ACCESS_s     RegistersQuickAccess; //!< Local copy of some registers - for quick access
    XHC_CDN_SFR_s              Registers;      //!< Keeps addresses of all SFR's
    USBSSP_NopComplete         nopComplete;    //!< NOP complete callback function, diagnostic function
    USBSSP_ForceHeaderComplete forceHeaderComplete; //!< NOP complete callback function, diagnostic function
    UINT32               EnabledEndpsMask; //!< Keeps endpoint enabled information sync with input context (device mode only)
    UINT32               MaxDeviceSlot; //!< The maximum number of Device Context Structures and Doorbell Array entries this controller can support
    /** flag active when setup packet received and enable_slot command isn't completed yet, it delegates setup handling to enable slot command completion */
    UINT32               Ep0State; //!< Endpoint 0 state (device mode only)
    UINT8                DevAddress;      //!< Keep device address sent during SET_ADDRESS setup request - is active only in device mode
    UINT32               SetupID;         //!< Keeps SetupId value of actually handled setup packet (device mode only)
    USB_CH9_SETUP_s      DevSetupReq;     //!< stores the setup request currently being handled in device mode
    UINT8                LastEpIntIndex;  //!< Keeps index of non zero endpoint which generated latest interrupt
    UINT32               instanceNo;      //!< Keeps index of instance associated with this resource
    UINT32               HostConfigValue; //!< Device configuration value, used in host mode (Set Configuration)
    UINT8                DevCtrlRxPending;        //!< Flag for control RX data pending in device mode.
    USB_CH9_SETUP_s      DevCtrlSetupDataPending; //!< Setup packet for pending control RX data, passed to upper layer.


    USBDCDNS3_CALLBACKS_s  DeviceCallbacks; //!< USBSSP Call backs registered in device mode
    USBHCDNS3_CALLBACKS_s  HostCallbacks;   //!< USBSSP Call backs registered in host mode

    AMBA_KAL_SEMAPHORE_t  SemaphoreCommandQueue;
    AMBA_KAL_SEMAPHORE_t  SemaphoreEp0Transfer;
    XHCI_RING_ELEMENT_s   SavedCommandEvent;
    XHCI_RING_ELEMENT_s   SavedEp0Event;
    UINT8                 FlagDevConfigured;  //!< Indicate if the device is configured. 0: Not configured, 1: Configured
    UINT8                 FlagDevCommandWait; //!< Indicate if the Command TRB to be waited
    UINT32                DevCommandToWait;   //!< The TRB type of Command TRB to be waited
    struct USBD_UDC_ts   *Udc;
};

/**
 * auxiliary structure type used for variables traversing between functions when
 * TD is being created
 */
typedef struct {
    UINT8 IsLastBuffer;          //!< used for signaling last buffer in scatter/gather transfer
    UINT8 IsFirstPage;           //!< used for signaling first memory page
    UINT8 IsLastPage;            //!< used for signaling last memory page
    UINT8 IsFirstTrb;            //!< used for signaling first TRB
    UINT8 IsLastTrb;             //!< used for signaling last TRB
    UINT8 IsIsoTrbSet;
    UINT32 TrbTransferLengthSum; //!< used for TDSize calculation
    UINT32 PacketTransfered;     //!< used for TDSize calculation
    UINT32 TdPacketCount;        //!< used for TDSize calculation
    UINT16 EpMaxPacketSize;      //!< used for TDSize calculation
    UINT32 EpIndex;              //!< keeps endpoint context index
    // --- used in TD creation APIs ---------------------
    UINT64 PageStart;   //!< Start Page of this TD. Used for page number calculation
    UINT64 PageEnd;     //!< End Page of this TD. used for page number calculation
    UINT64 NumOfPages;  //!< keeps number of memory page used for TD
    UINT64 DataAddr;    //!< Address of data pointer
} USBCDNS3_TD_CREATE_PARAM_s;

//
// utility functions for both device and host mode
//

UINT32                    USBCDNS3_Ch9SpeedToXhciSpeed(UINT32 ch9_usb_speed);
UINT32                    USBCDNS3_CpuToLe32(UINT32 InputValue);
UINT64                    USBCDNS3_CpuToLe64(UINT64 Value);

void                      USBCDNS3_DeviceConfigFlagSet(XHC_CDN_DRV_RES_s *DrvRes, UINT8 NewCfgFlag);
void                      USBCDNS3_DoorbellWrite(const XHC_CDN_DRV_RES_s *DrvRes, UINT32 SlotID, UINT32 DBValue);
XHC_CDN_DRV_RES_s        *USBCDNS3_DrvInstanceGet(void);

void                      USBCDNS3_EpCtxTRDequeuePtrSet(XHC_CDN_PRODUCER_QUEUE_s * EpQueue);
UINT32                    USBCDNS3_EpStatusGet(XHC_CDN_DRV_RES_s const *DrvRes, UINT32 EpIndex);
UINT32                    USBCDNS3_EpXferEnabledCheck(XHC_CDN_DRV_RES_s const *DrvRes, UINT32 EpIndex);
void                      USBCDNS3_EventPtrUpdate(XHC_CDN_DRV_RES_s *DrvRes);
UINT32                    USBCDNS3_EventTrbCheck(const XHC_CDN_DRV_RES_s *DrvRes, const XHCI_RING_ELEMENT_s *Trb);
UINT32                    USBCDNS3_EpCompletionHandler(XHC_CDN_DRV_RES_s const *DrvRes, XHC_CDN_PRODUCER_QUEUE_s *Queue);

void                      USBCDNS3_HostCommandDoorbell(const XHC_CDN_DRV_RES_s *DrvRes);

UINT64                    USBCDNS3_PtrToU64PhyAddr(const void *Ptr);

void                      USBCDNS3_QueuePointerUpdate(XHC_CDN_PRODUCER_QUEUE_s *Queue, UINT32 LinkTrbChainFlag, const char *DebugString);

UINT32                    USBCDNS3_Read32(volatile const UINT32 *Address);
UINT64                    USBCDNS3_Read64(volatile const UINT64 *Address);

XHC_CDN_PRODUCER_QUEUE_s *USBCDNS3_TransferObjectGet(XHC_CDN_DRV_RES_s *DrvRes);

UINT32                    USBCDNS3_TrbCmplCodeGet(XHCI_RING_ELEMENT_s const *Trb);
const char               *USBCDNS3_TrbCmplCodeStringGet(UINT32 TrbCompletionCode);
UINT8                     USBCDNS3_TrbChainBitGet(XHCI_RING_ELEMENT_s const *Trb);
UINT64                    USBCDNS3_TrbDequeueAddrGet(const XHCI_RING_ELEMENT_s *Trb);
UINT8                     USBCDNS3_TrbEndpointIdGet(XHCI_RING_ELEMENT_s const *Trb);
void                      USBCDNS3_TrbInfoDisplay(const XHCI_RING_ELEMENT_s *Trb, const char *PrefixStr);
UINT8                     USBCDNS3_TrbPortIdGet(XHCI_RING_ELEMENT_s const *Trb);
UINT8                     USBCDNS3_TrbSlotIdGet(XHCI_RING_ELEMENT_s const *Trb);
UINT8                     USBCDNS3_TrbToggleBitGet(XHCI_RING_ELEMENT_s const *Trb);
UINT32                    USBCDNS3_TrbTypeGet(XHCI_RING_ELEMENT_s const *Trb);
const char               *USBCDNS3_TrbTypeStringGet(UINT32 TrbType);

UINT32                    USBCDNS3_SlotDisable(XHC_CDN_DRV_RES_s *DrvRes);
UINT32                    USBCDNS3_SlotEnable(XHC_CDN_DRV_RES_s *DrvRes);

UINT32                    USBCDNS3_U32BitsRead(UINT32 Mask, UINT32 Shift, UINT32 RegValue);
UINT32                    USBCDNS3_U32BitsWrite(UINT32 Mask, UINT32 Shift, UINT32 RegValue, UINT32 Value);
UINT32                    USBCDNS3_U32BitClear(UINT32 Width, UINT32 Mask, UINT32 Flag_WOSET, UINT32 Flag_WOCLR,  UINT32 RegValue);
UINT32                    USBCDNS3_U32BitSet(UINT32 Width, UINT32 Mask, UINT32 Flag_WOCLR, UINT32 RegValue);
XHCI_RING_ELEMENT_s      *USBCDNS3_U64AddrToTrbRingPtr(UINT64 phyaddr);
void                      USBCDNS3_U64ValueSet(volatile UINT32 *addrL, volatile UINT32 *addrH, UINT64 value);
struct USBD_UDC_ts       *USBCDNS3_UdcInstanceGet(void);
void                      USBCDNS3_UdcInstanceReset(UINT32 FlagResetAll);

void                      USBCDNS3_Write32(volatile UINT32 *Address, UINT32 Value);
void                      USBCDNS3_Write64(volatile UINT64 *Address, UINT64 Value);

XHCI_CAPABILITY_REG_s    *USBCDNS3_XhcCapabilityRegGet(void);
XHCI_OPERATION_REG_s     *USBCDNS3_XhcOperationRegGet(void);
void                      USBCDNS3_XhcParameterInit(UINT32 ControllerMode);
void                      USBCDNS3_XhcUsbStsEventClear(const XHC_CDN_DRV_RES_s *DrvRes);

void                      USBCDNS3_XHCInputContexDisplay(const XHCI_INPUT_CONTEXT_s *InputContext);
void                      USBCDNS3_XHCInputContexEpDisplay(const XHCI_INPUT_CONTEXT_s *InputContext, UINT32 EpOffset, UINT32 DegbugFlag);

//
// utility functions for device mode only
//
struct UDC_ENDPOINT_INFO_ts *USBDCDNS3_EndpointInfoGet(UINT32 PhysicalEpIndex);
void                         USBDCDNS3_EventCallbackSet(const USBDCDNS3_CALLBACKS_s *CallBacks);
UINT32                       USBDCDNS3_PhyEndpointIdxGet(UINT32 LogicalAddress);
void                         USBDCDNS3_XhcCmdCtrlStatusStage(XHC_CDN_DRV_RES_s *DrvRes, UINT32 statusResp);

//
// generic APIs
//
UINT32 USBCDNS3_DataTransfer(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex,  UINT64 Buffer,  UINT32 Size, XHC_CDN_COMPLETE_f CompleteFunc);
UINT32 USBCDNS3_DataTransfer2(XHC_CDN_DRV_RES_s *DrvRes,
                              UINT32 EpIndex,
                              USBCDNS3_SG_PARAM paramT,
                              XHC_CDN_COMPLETE_f CompleteFunc);
UINT32 USBCDNS3_DataTransfer3(XHC_CDN_DRV_RES_s *DrvRes,
                              UINT32 EpIndex,
                              USBCDNS3_SG_PARAM paramT,
                              XHC_CDN_COMPLETE_f CompleteFunc);

UINT32 USBCDNS3_ConfigurationSet(XHC_CDN_DRV_RES_s *DrvRes,
                                 UINT32  ConfigValue,
                                 const UINT8  *EpCfgBuffer,
                                 UINT16  EpCfgBufferLen,
                                 XHC_CDN_COMPLETE_f CompleteFunc);
void   USBCDNS3_ConnectSpeedUpdate(XHC_CDN_DRV_RES_s *DrvRes);
UINT32 USBCDNS3_EndpointDisable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 LogicalAddress);
UINT32 USBCDNS3_EndpointEnable(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *desc);
UINT32 USBCDNS3_EndpointFeatureSet(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT8 Feature);
UINT32 USBCDNS3_EndpointStop(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EndpointIdx);
UINT32 USBCDNS3_EndpointReset(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EndpointIndex);
UINT32 USBCDNS3_RootHubPortReset(XHC_CDN_DRV_RES_s const *DrvRes);

UINT8  USBSSP_CalcFsLsEPIntrptInterval(UINT8 bInterval);

UINT32 USBCDNS3_XhcInitRun(UINT32 ControllerMode);
UINT32 USBCDNS3_XhcStop(void);

void   USBCDNS3_XhcCmdConfigEndpoint(XHC_CDN_DRV_RES_s *DrvRes);
UINT32 USBCDNS3_XhcCmdDisableSlot(XHC_CDN_DRV_RES_s *DrvRes);
UINT32 USBCDNS3_XhcCmdEnableSlot(XHC_CDN_DRV_RES_s *DrvRes);
void   USBCDNS3_XhcCmdSetAddress(XHC_CDN_DRV_RES_s *DrvRes, UINT8 bsrVal);
UINT32 USBCDNS3_XhcCmdResetDevice(XHC_CDN_DRV_RES_s *DrvRes) ;
void   USBCDNS3_XhcCmdResetEndpoint(XHC_CDN_DRV_RES_s *DrvRes, UINT32 endpoint);
void   USBCDNS3_XhcCmdSetTrDequeuePtr(XHC_CDN_DRV_RES_s *DrvRes, UINT32 epIndex, UINT64 newDequeuePtr);

void   USBCDNS3_XhcU1TimeoutSet(const XHC_CDN_DRV_RES_s *DrvRes, UINT8 Time);
void   USBCDNS3_XhcU2TimeoutSet(const XHC_CDN_DRV_RES_s *DrvRes, UINT8 Time);



//
// APIs for Host mode
//

UINT32 USBHCDNS3_Isr(void);
UINT32 USBHCDNS3_ControllerInit(struct XHC_CDN_DRV_RES_ts **DrvRes);
UINT32 USBHCDNS3_ControlTransfer(XHC_CDN_DRV_RES_s *DrvRes, USB_CH9_SETUP_s const *Setup, UINT8 const *pData);

UINT32 USBHCDNS3_EndpointDisable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 LogicEpIndex, UINT32 FlagWait);
UINT32 USBHCDNS3_EndpointEnable(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *Desc, UINT32 FlagWait);
UINT32 USBHCDNS3_EndpointReset(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT32 FlagWait);
UINT32 USBHCDNS3_EndpointStop(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT32 FlagWait);
UINT32 USBHCDNS3_FrameNumberRead(XHC_CDN_DRV_RES_s *DrvRes, UINT32 *FrameNumber);
UINT32 USBHCDNS3_DeviceAddress(XHC_CDN_DRV_RES_s *DrvRes, UINT32 FlagWait);
UINT32 USBHCDNS3_PortStatusRead(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortID);
UINT32 USBHCDNS3_SlotDisable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 FlagWait);
UINT32 USBHCDNS3_SlotEnable(XHC_CDN_DRV_RES_s *DrvRes, UINT32 FlagWait);
UINT32 USBHCDNS3_TRDequeuePtrSet(XHC_CDN_DRV_RES_s *DrvRes, UINT32 EpIndex, UINT32 FlagWait);


//
// APIs for device mode
//
UINT32 USBDCDNS3_ControllerInit(const USBDCDNS3_CALLBACKS_s *CallBacks);
UINT32 USBDCDNS3_ControllerDeInit(void);
UINT32 USBDCDNS3_ControlTransfer(XHC_CDN_DRV_RES_s *DrvRes, UINT8 const *pdata, UINT32 length, UINT32 dirFlag);
UINT32 USBDCDNS3_IntDeInit(void);
UINT32 USBDCDNS3_IntInit(void);
UINT32 USBDCDNS3_IsrHandler(void);
UINT32 USBDCDNS3_EndpointCreate(struct USBD_UDC_ts *Udc, UX_SLAVE_ENDPOINT *Endpoint);
UINT32 USBDCDNS3_EndpointDestroy(struct USBD_UDC_ts *Udc, UX_SLAVE_ENDPOINT *Endpoint);
UINT32 USBDCDNS3_EndpointReset(struct USBD_UDC_ts *Udc, const UX_SLAVE_ENDPOINT *Endpoint);
void   USBDCDNS3_EndpointStall(const UX_SLAVE_ENDPOINT *Endpoint);
UINT32 USBDCDNS3_EndpointStop(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT32 FlagWait);
UINT32 USBDCDNS3_FrameNumberGet(struct USBD_UDC_ts *Udc, UINT32 *FrameNumber);
UINT32 USBDCDNS3_EndpointStatusGet(const struct USBD_UDC_ts *Udc, UINT32 LogicalAddress);
UINT32 USBDCDNS3_PortStatusRead(XHC_CDN_DRV_RES_s *DrvRes, UINT32 PortID);
UINT32 USBDCDNS3_TransferRequest(struct USBD_UDC_ts *Udc, UX_SLAVE_TRANSFER *TransferRequest);
UINT32 USBDCDNS3_TransferAbort(const UX_SLAVE_TRANSFER *TransferRequest);
UINT32 USBDCDNS3_TRDequeuePtrSet(XHC_CDN_DRV_RES_s *DrvRes, UINT8 EpIndex, UINT32 FlagWait);
UINT32 USBDCDNS3_UdcInstanceSafeGet(struct USBD_UDC_ts **Udc);

UX_SLAVE_TRANSFER *USBDCDNS3_UxTransferGet(const struct USBD_UDC_ts *Udc, UINT32 LogicalAddress);

UINT  ux_hcd_xhci_initialize(UX_HCD *hcd);
UINT  ux_hcd_xhci_stop(void);
UINT  ux_hcd_xhci_destroy(void);

void  USBCDNS3_Regdump(CDNS3_PRINT_f PrintFunc);

#endif

