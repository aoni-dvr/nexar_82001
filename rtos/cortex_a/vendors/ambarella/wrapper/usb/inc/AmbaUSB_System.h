/**
 *  @file AmbaUSB_System.h
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
 *  @details USB Device driver functions header file.
 */

/*! \mainpage USB drivers
 *
 * \section intro_sec Introduction
 *
 * USB drivers include device driver, host driver, and class drivers.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

#ifndef AMBAUSB_SYSTEM_H
#define AMBAUSB_SYSTEM_H

#include <AmbaRTSL_USB.h>
#ifndef UX_API_H
#include <usbx/ux_api.h>
#endif
#include <AmbaMisraFix.h>
#ifdef CONFIG_BUILD_CADENCE_USB3_DRIVER
#include <StdXHCI.h>
#include <AmbaUSB_Cadence.h>
#endif

#define USB_ATTR_ALIGN_CACHE_NO_INIT __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE))) __attribute__((section(".bss.noinit")))
#define USB_ATTR_ALIGN_CACHE __attribute__ ((aligned(AMBA_CACHE_LINE_SIZE)))
#define USB_ATTR_NO_INIT __attribute__((section(".bss.noinit")))

#define USBD_SUPPORT_ISO //!< Device Isochronous transfer is supported if it is defined.

#define USBD_SLAVE_CONTROLLER_TYPE_UDC2 0x01
#define USBD_SLAVE_CONTROLLER_TYPE_UDC3 0x02

#ifdef CONFIG_BUILD_SYNOPSYS_USB2_DRIVER
#define UDC_MAX_ENDPOINT_NUM            11U
#else
#define UDC_MAX_ENDPOINT_NUM            32U
#endif
#define USBD_UDC_ED_STATUS_UNUSED       0U
#define USBD_UDC_ED_STATUS_USED         0x00000001U
#define USBD_UDC_ED_STATUS_TRANSFER     0x00000002U
#define USBD_UDC_ED_STATUS_STALLED      0x00000004U
#define USBD_UDC_MAX_IN_ED_NUM          5U
#define USBD_UDC_MAX_OUT_ED_NUM         5U
#define USBD_EP_DIR_OUT                 0U //!< Endpoint IN direction. Used inside driver only.
#define USBD_EP_DIR_IN                  1U //!< Endpoint OUT direction. Used inside driver only.
#define USBD_DMA_STATUS_MASK            0xC0000000U
#define USBD_DMA_HOST_READY             0x00000000U
#define USBD_DMA_HOST_NOT_RDY           0xC0000000U
#define USBD_DMA_DONE                   0x80000000U
#define USBD_DMA_LAST_DESC              0x08000000U

#define USBD_BULK_IN_CHAIN_DESC_NUM     128U
#define USBD_BULK_OUT_CHAIN_DESC_NUM    64U
#define USBD_INTERRUPT_CHAIN_DESC_NUM   32U
#define USBD_ISOCHRONOUS_CHAIN_DESC_NUM 128U

#define USBD_UDC_EP_CONTROL             0U
#define USBD_UDC_EP_ISO                 1U
#define USBD_UDC_EP_BULK                2U
#define USBD_UDC_EP_INT                 3U
#define EP_OUT_RCV_DATA                 0x01U
#define EP_OUT_RCV_SETUP                0x02U
#define USBD_UDC_ISO_WRAPPER_NUM        20U

#if defined(CONFIG_SOC_CV5) || defined(CONFIG_SOC_CV52)
#define USBCTRLR_MEMORY_SIZE            (0x100000U)
#else
#define USBCTRLR_MEMORY_SIZE            (0x000000U)
#endif

typedef enum  {
    UDC_OWN_PORT = 0,
    UHC_OWN_PORT
} USB0_PORT_OWNER_e;

typedef enum {
    USB_PHY_ADJ_HS_PREEMPHASIS = 0,
    USB_PHY_ADJ_HS_TXRISETUNE,
    USB_PHY_ADJ_HS_TXVREFTUNE,
    USB_PHY_ADJ_SS_RXEQUALIZER,
    USB_PHY_ADJ_SS_TXFULLSWING,
    USB_PHY_ADJ_SS_RXLOS,
    USB_PHY_ADJ_SS_RXLOSMASK,
    USB_PHY_ADJ_FS_TXFSLSTUNE,
    USB_PHY_ADJ_NUM
} USB_PHY_ADJUST_e;

/*! \brief Mapping between logical and physical endpoint.
*/
typedef struct {
    UINT32 LogicalAddr;                  //!< Logical Endpoint address
    UINT32 Udc20Index;                   //!< UDC20 Endpoint Index
    UINT32 PhysicalIndex;                //!< Physical Endpoint address
    UINT32 Direction;                    //!< Endpoint Direction
    UINT32 Used;
} UDC_ENDPOINT_MAP_s;

/*! \brief Map information of all endpoints
*/
typedef struct {
    UDC_ENDPOINT_MAP_s EndpointMap[UDC_MAX_ENDPOINT_NUM];
} UDC_ENDPOINT_MAP_INFO_s;

/*! \brief USB setup packet data descriptor
*
* USB setup packet is always 8-byte long. It should be 64-byte width and alignment for cache operation. Please see databook for detail.
*/
typedef struct {
    UINT32 Status;
    UINT32 Reserved;
    UINT32 Data0; //!< byte [0:3] of USB setup packet sent from USB Host
    UINT32 Data1; //!< byte [4:7] of USB setup packet sent from USB Host
    UINT32 Padding[12];
} UDC_SETUP_DESC_s;

/*! \brief DMA descriptor for USB Device Controller.
*
* It should be 64-byte width and alignment for cache operation. Please see databook for detail.
*/
typedef struct {
    UINT32 Status;    //!< DMA status.
    UINT32 Reserved;
    UINT32 DataPtr;   //!< Point to buffer which data will be moved in/out through DMA engine. It should be physical address.
    UINT32 NextDescPtr; //!< Point to next DMA descriptor. It should be physical address.
    UINT32 Padding[12];
} UDC_DATA_DESC_s;


/*! \brief DMA descriptor information for isochronous transfer
* */
typedef struct {
    UINT32           Used;
    UDC_DATA_DESC_s *DataDesc;
} UDC_ISO_DESC_INFO_s;

/*! \brief isochronous transfer request
* */
typedef struct {
    UDC_ISO_DESC_INFO_s      *DescWrapper;    //!< Actual DMA descriptor
    UX_SLAVE_TRANSFER *      TransferRequest; //!< UX Transfer Request
    UINT32                   StartTimeStamp;  //!< Start time of this request
    UINT32                   EndTimeStamp;    //!< Expected complete time of this request
} UDC_ISO_REQUEST_s;

typedef struct UDC_RXFIFO_LIST_HEAD_s_ {
    struct UDC_RXFIFO_LIST_HEAD_s_ *next, *prev;
    struct UDC_RX_SWFIFO_INFO_s_ *  container;
} UDC_RXFIFO_LIST_HEAD_s;

typedef struct UDC_RX_SWFIFO_INFO_s_ {
    UINT8 *                Addr;
    UINT32                 Length;
    UDC_RXFIFO_LIST_HEAD_s List;
} UDC_RX_SWFIFO_INFO_s;

static inline void udc_rxfifo_init_list_head(UDC_RXFIFO_LIST_HEAD_s *list, UDC_RX_SWFIFO_INFO_s *container)
{
    list->next      = list;
    list->prev      = list;
    list->container = container;
}

static inline void udc_rxfifo_list_del(UDC_RXFIFO_LIST_HEAD_s *list)
{
    UINT32 UDC_LIST_POISON1 = 0x00100100;
    UINT32 UDC_LIST_POISON2 = 0x00200200;
    struct UDC_RXFIFO_LIST_HEAD_s_ *next, *prev;

    AmbaMisra_TypeCast32(&next, &UDC_LIST_POISON1);
    AmbaMisra_TypeCast32(&prev, &UDC_LIST_POISON2);

    list->prev->next = list->next;
    list->next->prev = list->prev;
    list->next       = next;
    list->prev       = prev;
    list->container  = NULL;
}

static inline void udc_rxfifo_list_add_impl(UDC_RXFIFO_LIST_HEAD_s *new,
        UDC_RXFIFO_LIST_HEAD_s *prev,
        UDC_RXFIFO_LIST_HEAD_s *next)
{
    next->prev = new;
    new->next  = next;
    new->prev  = prev;
    prev->next = new;
}

static inline UDC_RX_SWFIFO_INFO_s* udc_rxfifo_list_get_next_entry(const UDC_RXFIFO_LIST_HEAD_s *now)
{
    return now->next->container;
}

static inline UDC_RX_SWFIFO_INFO_s* udc_rxfifo_list_get_first_entry(const UDC_RXFIFO_LIST_HEAD_s *head)
{
    return head->next->container;
}

static inline UDC_RX_SWFIFO_INFO_s* udc_rxfifo_list_get_last_entry(const UDC_RXFIFO_LIST_HEAD_s *head)
{
    return head->prev->container;
}

static inline void udc_rxfifo_list_add(UDC_RXFIFO_LIST_HEAD_s *new, UDC_RXFIFO_LIST_HEAD_s *head, UDC_RX_SWFIFO_INFO_s *container)
{
    udc_rxfifo_list_add_impl(new, head, head->next);
    new->container = container;
}

static inline void udc_rxfifo_list_add_tail(UDC_RXFIFO_LIST_HEAD_s *new, UDC_RXFIFO_LIST_HEAD_s *head, UDC_RX_SWFIFO_INFO_s *container)
{
    udc_rxfifo_list_add_impl(new, head->prev, head);
    new->container = container;
}

static inline INT32 udc_rxfifo_list_empty(const UDC_RXFIFO_LIST_HEAD_s *head)
{
    INT32 nRet = 0;
    if (head->next == head) {
        nRet = 1;
    }
    return nRet;
}

static inline INT32 udc_rxfifo_list_is_last(const UDC_RXFIFO_LIST_HEAD_s *list, const UDC_RXFIFO_LIST_HEAD_s *head)
{
    INT32 nRet = 0;
    if (list->next == head) {
        nRet = 1;
    }
    return nRet;
}

#define UDC_RX_SWFIFO_DEPTH         18U
#define UDC_RX_DMA_STAT_IDLE        0x00000000U
#define UDC_RX_DMA_STAT_ALT_DESC    0x00000001U
#define UDC_RX_DMA_STAT_REQ_PENDING 0x00000002U

/*! \brief DMA descriptor information for multiple RX fix
*/
typedef struct {
    UDC_DATA_DESC_s DmaDesc;
    UINT8 *         Buffer;
} UDC_RDMA_SUB_DESC_INFO_s;

/*! \brief USB Device Endpoint information
*/
#ifdef CONFIG_BUILD_CADENCE_USB3_DRIVER
typedef struct UDC_ENDPOINT_INFO_ts {
#else
typedef struct {
#endif
    UINT32                   EpState;         //!< Endpoint Status
    UINT32                   UdcIndex;        //!< UDC20 Index for the endpoint
    UINT32                   PhysicalEpIndex; //!< Physical Endpoint Index
    UINT32                   Direction;       //!< Endpoint Direction, should be USBD_EP_DIR_IN or USBD_EP_DIR_OUT
    UX_SLAVE_ENDPOINT *      UxEndpoint;      //!< USBX Endpoint
    UINT32                   InterruptStatus; //!< Endpoint Interrupt Status (register value)
    UDC_DATA_DESC_s *        DmaDesc;         //!< DMA descriptors
    UINT32                   DmaDescSize;     //!< The total size of DMA descriptors in bytes.
    UINT32                   DmaDescNum;      //!< The total number of DMA descriptors.
    UINT32                   TxPending;
    UINT32                   RxDmaState;
    UINT8 *                  RxSwFifoBase;    //!< Base address of software RX fifo
    UINT32                   RxSwFifoSize;    //!< Size of software RX fifo
    UDC_RX_SWFIFO_INFO_s     RxSwFifoInfo[UDC_RX_SWFIFO_DEPTH];
    UDC_RXFIFO_LIST_HEAD_s   RxSwFifoList;
    UDC_RDMA_SUB_DESC_INFO_s SubDmaInfo;
#if defined(USBD_SUPPORT_ISO)
    UDC_ISO_DESC_INFO_s      IsoDescPool[USBD_UDC_ISO_WRAPPER_NUM]; //!< DMA Descriptors for isochronous requests
    AMBA_KAL_MSG_QUEUE_t     IsoReqQueue; //!< request queue for isochronous transfer
    UDC_ISO_REQUEST_s        IsoReq;
    UINT32                   LastFrameNumber;
#endif
} UDC_ENDPOINT_INFO_s;

#ifdef CONFIG_BUILD_CADENCE_USB3_DRIVER

/*! \brief USB Host Endpoint information
*/
typedef struct {
    UINT32                   EpState;         //!< Endpoint Status
    UINT32                   PhysicalEpIndex; //!< Physical Endpoint Index
    UINT32                   LogicalEpIndex;  //!< Logical Endpoint Index
    UX_ENDPOINT             *UxEndpoint;      //!< USBX Endpoint
    UX_TRANSFER             *Transfer;
} UHC_ENDPOINT_INFO_s;

#endif

typedef struct {
    UDC_DATA_DESC_s  CtrlInDataDesc;
    UDC_DATA_DESC_s  CtrlOutDataDesc;
    UDC_SETUP_DESC_s CtrlOutSetupDesc;
} UDC_CONTROL_EP_DESCS_s;

/*! \brief USB Device Controller Data
*/
#ifdef CONFIG_BUILD_CADENCE_USB3_DRIVER
typedef struct USBD_UDC_ts {
#else
typedef struct {
#endif
    UX_SLAVE_DCD *          UxDcdOwner; //!< Point to UX DCD
    UDC_ENDPOINT_INFO_s     DcdEndpointsInfoArray[UDC_MAX_ENDPOINT_NUM]; //!< Arrays for UDC endpoints information
    AMBA_KAL_TASK_t *       IsrThread;  //!< ISR task
    UINT32                  IsrThreadRunning;
    AMBA_KAL_MSG_QUEUE_t    IsrRequestQueue; //!< Request Queue for ISR task
    AMBA_KAL_MUTEX_t        RxMutex;
    AMBA_KAL_MUTEX_t        RxStateMutex;
    UINT32                  RxFifoMax;   //!< Maximum RX FIFO size in bytes. The value varies with different chips.
    UINT32                  TxFifoUsed;  //!< TX FIFO size used in bytes.
    UINT32                  TxFifoMax;   //!< Maximum TX FIFO size in bytes. The value varies with different chips.
    UDC_ENDPOINT_MAP_INFO_s EndpointMapInfo; //!< Endpoint map information between logical address, physical address, and udc20 register index.
    AMBA_KAL_TASK_t *       CtrlReqThread; //!< Control Request task
    UINT32                  CtrlReqThreadRunning;
    AMBA_KAL_EVENT_FLAG_t   CtrlReqEventFlag;
    UINT32                  CurrentFrameNumber;
    UINT32                  DisableDoubleBuffer;
#ifdef CONFIG_BUILD_CADENCE_USB3_DRIVER
    // belows is for Candence USB Device Controller IP
    XHC_CDN_DRV_RES_s       CDNDrvRes;
    UHC_ENDPOINT_INFO_s     HostEndpointsInfoArray[UDC_MAX_ENDPOINT_NUM]; //!< Arrays for UHC endpoints information
#endif
} USBD_UDC_s;

#define MAX_ISR_OPERATIONS             ((UINT32)30UL)
#define MAX_ISR_OPERATION_PARAM_COUNT  ((UINT32)5UL)
#define ISR_OPERATION_NOTIFY_INTERRUPT ((UINT32)0x20UL)
#define ISR_OPERATION_NOTIFY_EXIT      ((UINT32)0x40UL)
#define USBD_ISR_QUEUE_MEM_SIZE        (sizeof(UDC_ISR_REQUEST_s) * MAX_ISR_OPERATIONS)

/*! \brief ISR request.
*
* The request is sent/received by queue. Maximum size is 16*32 bytes, make sure it's 4-byte alignment
* It is sent from ISR routine and received by ISR task.
*/
typedef struct {
    UX_SLAVE_ENDPOINT *Endpoint;
    UINT32             Opcode;
    UINT32             Params[MAX_ISR_OPERATION_PARAM_COUNT];
} UDC_ISR_REQUEST_s;

// AmbaUSB_System.c
UINT32 USBD_SystemConfigGet(USBD_SYS_CONFIG_s **Config);
UINT32 USBD_SystemClassConfigGet(USBD_CLASS_CONFIG_s **Config);
UINT32 USBD_SystemIsInit(void);
UINT32 USBD_SystemIsClassConfiged(void);
UINT32 USBD_SystemCtrlrMemGet(UINT8 **Ptr, UINT32 *Size);
void   USBD_SystemAppEventNotify(UINT32 EventCode, UINT32 Param1, UINT32 Param2, UINT32 Param3);

// AmbaUSB_Drv_ConnectChange.c
UINT32 USBD_ConnectChgInit(void);
UINT32 USBD_ConnectChgDeInit(void);
void USBD_ConnectChgCfgNotifiedSet(UINT32 Notify);
void USBD_ConnectChgCfgCntStartSet(UINT32 Start);
UINT32 USBD_ConnectChgVbusStatusGet(void);

// AmbaUSB_Drv_Phy.c

#define UPORT_MODE_DEVICE           0U
#define UPORT_MODE_HOST             1U
#define UPORT_MODE_DEVICE_HOST      2U

void USB_PhyPllEnable(UINT32 OnOff, UINT32 PortMode);
UINT32 USB_PhyDataContactDetection(UINT32 T1, UINT32 T2, UINT32 *pContact);
void USB_PhyChargerDetection(UINT32 *pChargerType);
UINT32 USB_PhySignalQualityAdjust(UINT32 Adj, UINT32 Port, UINT32 Value);
void USB_PhyPhy0OwnerSet(USB0_PORT_OWNER_e Owner);

// AmbaUSB_Drv_Udc.c
void USBD_UdcDeviceDataConnSet(UINT32 value);
UINT32 USBD_UdcEndpointMapCreate(USBD_UDC_s *Udc, UINT32 LogAddr);
void USBD_UdcEndpointBufferAllocate(UX_SLAVE_ENDPOINT *Endpoint);
void USBD_UdcEndpointBufferDestroy(UX_SLAVE_ENDPOINT *Endpoint);
UINT32 USBD_UdcInit(void);
UINT32 USBD_UdcDeInit(void);
void USBD_UdcInitCtrlSetupDesc(void);
void USBD_UdcInitCtrlDataDesc(void);
void USBD_UdcInitDmaDesc(UDC_DATA_DESC_s *Desc, UINT32 Number);
void USBD_UdcDmaDescReset(const UDC_ENDPOINT_INFO_s *UdcEpInfo);
void USBD_UdcCtrlOutDataDescGet(UDC_DATA_DESC_s **Desc);
void USBD_UdcCtrlInDataDescGet(UDC_DATA_DESC_s **Desc);
UDC_DATA_DESC_s* USBD_UdcLastDescGet(UDC_DATA_DESC_s* Head, UINT32 MaxDescNum);
void USBD_UdcDmaFifoEmpty(void);
void USBD_UdcInitController(void);
UINT32 USBD_UdcUdcInstanceGet(USBD_UDC_s **Udc);
UDC_ENDPOINT_MAP_s *USBD_UdcEndpointMapGet(USBD_UDC_s *Udc, UINT32 LogAddr);
void USBD_UdcRxStateMutexGet(UINT32 Timeout);
void USBD_UdcRxStateMutexPut(void);
void USBD_UdcRxSwfifoPush(UDC_ENDPOINT_INFO_s *UdcEpInfo, const UX_SLAVE_TRANSFER *TransferRequest);
void USBD_UdcRxSwfifoPop(const UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *TransferRequest, UINT8 **DataPtr, UINT32 *Length);
void USBD_UdcTxDescSetup(UDC_ENDPOINT_INFO_s *UdcEpInfo, UX_SLAVE_TRANSFER *Transfer, UINT32 FlagIsoMode);
void USBD_UdcTxDescSetupZeroLength(const UDC_ENDPOINT_INFO_s *UdcEpInfo);
void USBD_UdcTxCtrlDescSetup(UX_SLAVE_TRANSFER *Transfer);
void USBD_UdcRxDescSetup(UDC_ENDPOINT_INFO_s *UdcEpInfo, const UX_SLAVE_TRANSFER *TransferRequest, const UINT8 *DataPtr, UINT32 Length, UINT32 Update);
void USBD_UdcRxSubDescSetup(UDC_ENDPOINT_INFO_s *UdcEpInfo);
void USBD_UdcCtrlNullPacketSend(void);
void USBD_UdcCtrlNullPacketSendEx(void);
UINT32 USBD_UdcEnumSpeedGet(void);
void USBD_UdcDmaMemoryAllocate(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *UsbxEndpoint);
void USBD_UdcDmaMemoryDestroy(USBD_UDC_s *Udc, const UX_SLAVE_ENDPOINT *UsbxEndpoint);
UINT32 USBD_UdcIsConfigured(void);
UINT32 USBD_UdcIsoDescPatch(const USBD_UDC_s *pUdc, UDC_ENDPOINT_INFO_s *pUdcEpInfo, UINT32 MaxDescNum, UINT32 IsNewTransfer);
void   USBD_UdcFlagIsoTxOngoingSet(UINT32 Flag);

// AmbaUSB_Drv_Function.c
UINT USBD_DrvDispatchFunction(const struct UX_SLAVE_DCD_STRUCT *Dcd, UINT Function, const void *Parameter);

// AmbaUSB_Drv_UdcInterrupt.c
UINT32 USBD_IntInit(void);
UINT32 USBD_IntDeInit(void);
UINT32 USBD_IntIsEnabled(void);
void USBD_IntEnable(void);
void USBD_IntDisable(void);

// AmbaUSB_Drv_SetupHandler.c
UINT32 USBD_SetupInit(void);
UINT32 USBD_SetupDeInit(void);
void USBD_SetupCtrlRequestNotify(void);

UINT32 USB_DrvInitComplete(void);

// USBDCDNS3_DrvInitComplete.c
UINT32 USBDCDNS3_DrvInitComplete(void);

// USBD_DrvTransferCallback.c
void USBD_DrvTransferCallback(USBD_UDC_s *Udc, UINT32 InOut, UINT32 EpNum, UX_SLAVE_TRANSFER *TransferRequest);

// AmbaUSB_Drv_EndpointStall.c
void USBD_DrvEndpointStall(const UX_SLAVE_ENDPOINT *Endpoint);

// AmbaUSB_Drv_FrameNumberGet.c
UINT32 USBD_DrvFrameNumberGet(USBD_UDC_s *Udc, UINT32 *FrameNumber);

// USBD_DrvTransferRequest.c
UINT32 USBD_DrvTransferRequest(USBD_UDC_s *Udc, UX_SLAVE_TRANSFER *TransferRequest);

// AmbaUSB_Drv_EndpointRest.c
UINT32 USBD_DrvEndpointReset(USBD_UDC_s *Udc, const UX_SLAVE_ENDPOINT *Endpoint);

// AmbaUSB_Drv_EndpointCreate.c
UINT32 USBD_DrvEndpointCreate(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *Endpoint);

// AmbaUSB_Drv_EndpointDestroy.c
UINT32 USBD_DrvEndpointDestroy(USBD_UDC_s *Udc, UX_SLAVE_ENDPOINT *Endpoint);

// AmbaUSB_Drv_EndpointStatus.c
UINT32 USBD_DrvEndpointStatusGet(const USBD_UDC_s *Udc, UINT32 EndpointIndex);

// USBD_DrvTransferAbort.c
UINT32 USBD_DrvTransferAbort(const UX_SLAVE_TRANSFER *TransferRequest);

// AmbaUSB_Drv_Uhc.c
void USBH_UhcInit(const USBH_CLASS_CONFIG_s *Config);
void USBH_UhcDeInit(void);

// AmbaUSB_Host_System.c
typedef UINT32 (*USBH_DEVICE_CHANGE_f)(UINT32 Operation, const void *Instance);
UINT32 USBH_SystemIsInit(void);
UINT32 USBH_SystemIsUSBXInited(void);
UINT32 USBH_DeviceChangeFuncHook(UINT8* pClassName, USBH_DEVICE_CHANGE_f ChangeFunc);
UINT32 USBH_DeviceChangeFuncUnHook(UINT8* pClassName);
UINT32 USBH_SystemCtrlrMemGet(UINT8 **Ptr, UINT32 *Size);

#endif

